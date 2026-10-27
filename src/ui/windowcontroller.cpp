#include "windowcontroller.h"

#include <QGuiApplication>
#include <QMouseEvent>
#include <QPlatformSurfaceEvent>
#include <QPushButton>
#include <QTabBar>
#include <QTimer>
#include <QWindow>
#include <QtMath>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <windowsx.h>
#include <dwmapi.h>

namespace {
// Undocumented theme-paint messages also handled by Chromium's custom frame.
// They can draw the standard caption/frame directly, bypassing WM_NCPAINT.
constexpr UINT kNcUahDrawCaption = 0x00AE;
constexpr UINT kNcUahDrawFrame = 0x00AF;

void applySquareCorners(HWND hwnd, bool fullScreen)
{
    const DWM_WINDOW_CORNER_PREFERENCE squareCorners = DWMWCP_DONOTROUND;
    if (SUCCEEDED(DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                                       &squareCorners, sizeof(squareCorners))) || IsIconic(hwnd))
        return;

    // Windows 10 otherwise clips just the upper corners of a captioned HWND.
    // An explicit rectangular region overrides that clipping without changing
    // the window styles or replacing native caption/resize hit testing.
    HRGN current = CreateRectRgn(0, 0, 0, 0);
    if (!current)
        return;
    const int regionType = GetWindowRgn(hwnd, current);
    if (fullScreen || IsZoomed(hwnd))
    {
        if (regionType != ERROR)
            SetWindowRgn(hwnd, nullptr, TRUE);
    }
    else
    {
        RECT frame{};
        if (GetWindowRect(hwnd, &frame))
        {
            HRGN rectangle = CreateRectRgn(0, 0, frame.right - frame.left, frame.bottom - frame.top);
            if (!rectangle)
            {
                DeleteObject(current);
                return;
            }
            if (regionType != ERROR && EqualRgn(current, rectangle))
                DeleteObject(rectangle);
            else if (!SetWindowRgn(hwnd, rectangle, TRUE))
                DeleteObject(rectangle);
            // On success Windows owns rectangle. Comparing first also avoids
            // recursively changing an unchanged region during WM_SIZE.
        }
    }
    DeleteObject(current);
}
}
#endif

WindowController::WindowController(QWidget* parent)
    : QWidget(parent)
{
#ifdef Q_OS_WIN
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                   | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    setAttribute(Qt::WA_ContentsMarginsRespectsSafeArea, false);
#endif
#endif
}

void WindowController::initializeWindow(QWidget* titleBar, QWidget* dragArea, QSize defaultSize)
{
    m_titleBar = titleBar;
    m_dragArea = dragArea;
    if (m_titleBar)
        m_titleBar->installEventFilter(this);
    if (m_dragArea && m_dragArea != m_titleBar)
        m_dragArea->installEventFilter(this);
#ifdef Q_OS_WIN
    // Create the custom frame before setting the restore size.
    if (QGuiApplication::platformName() == "windows")
        winId();
#endif
    resize(defaultSize);
    showMaximized();
#ifdef Q_OS_WIN
    if (QGuiApplication::platformName() == "windows")
        windowHandle()->installEventFilter(this);
#endif
}

void WindowController::closeWindow(QWidget* window)
{
    window->close();
}

void WindowController::minimizeWindow(QWidget* window)
{
#ifdef Q_OS_WIN
    if (QGuiApplication::platformName() == "windows")
    {
        PostMessageW(reinterpret_cast<HWND>(window->winId()), WM_SYSCOMMAND, SC_MINIMIZE, 0);
        return;
    }
#endif
    window->showMinimized();
}

void WindowController::toggleMaximized(QWidget* window)
{
#ifdef Q_OS_WIN
    if (QGuiApplication::platformName() == "windows" && !window->isFullScreen())
    {
        const auto hwnd = reinterpret_cast<HWND>(window->winId());
        PostMessageW(hwnd, WM_SYSCOMMAND, IsZoomed(hwnd) ? SC_RESTORE : SC_MAXIMIZE, 0);
        return;
    }
#endif
    window->isMaximized() ? window->showNormal() : window->showMaximized();
}

bool WindowController::isDragArea(const QPoint& windowPosition) const
{
    if (!m_titleBar || !m_titleBar->rect().contains(m_titleBar->mapFrom(this, windowPosition)))
        return false;
    QWidget* child = childAt(windowPosition);
    auto* tabBar = qobject_cast<QTabBar*>(child);
    return child == m_titleBar || child == m_dragArea
        || (tabBar && tabBar->tabAt(tabBar->mapFrom(this, windowPosition)) < 0);
}

bool WindowController::eventFilter(QObject* watched, QEvent* event)
{
#ifdef Q_OS_WIN
    if (watched == windowHandle() && event->type() == QEvent::PlatformSurface)
    {
        auto* surfaceEvent = static_cast<QPlatformSurfaceEvent*>(event);
        if (surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)
        {
            WINDOWPLACEMENT placement{sizeof(WINDOWPLACEMENT)};
            if (!isFullScreen()
                && GetWindowPlacement(reinterpret_cast<HWND>(windowHandle()->winId()), &placement))
            {
                const RECT& rect = placement.rcNormalPosition;
                m_preservedNormalPlacement = QRect(rect.left, rect.top,
                                                    rect.right - rect.left, rect.bottom - rect.top);
            }
        }
    }

#endif
    if ((watched == m_titleBar || watched == m_dragArea) && !isFullScreen())
    {
        auto* widget = static_cast<QWidget*>(watched);
        switch (event->type())
        {
        case QEvent::MouseButtonPress:
        {
            auto* mouse = static_cast<QMouseEvent*>(event);
            if (mouse->button() == Qt::LeftButton
                && isDragArea(widget->mapTo(this, mouse->position().toPoint())))
            {
                m_isDragging = false;
                if (auto* handle = windowHandle(); !handle || !handle->startSystemMove())
                {
                    m_previousGlobalPosition = mouse->globalPosition().toPoint();
                    m_isDragging = true;
                }
                mouse->accept();
                return true;
            }
            break;
        }
        case QEvent::MouseMove:
        {
            auto* mouse = static_cast<QMouseEvent*>(event);
            if (m_isDragging && (mouse->buttons() & Qt::LeftButton))
            {
                const QPoint global = mouse->globalPosition().toPoint();
                move(pos() + global - m_previousGlobalPosition);
                m_previousGlobalPosition = global;
                mouse->accept();
                return true;
            }
            break;
        }
        case QEvent::MouseButtonRelease:
            m_isDragging = false;
            break;
        case QEvent::MouseButtonDblClick:
        {
            auto* mouse = static_cast<QMouseEvent*>(event);
            if (mouse->button() == Qt::LeftButton
                && isDragArea(widget->mapTo(this, mouse->position().toPoint())))
            {
                toggleMaximized(this);
                mouse->accept();
                return true;
            }
            break;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

#ifdef Q_OS_WIN
bool WindowController::event(QEvent* event)
{
    const bool handled = QWidget::event(event);
    if (event->type() == QEvent::WinIdChange && internalWinId()
        && QGuiApplication::platformName() == "windows")
    {
        // A render-surface change can replace the QWindow as well as its HWND.
        windowHandle()->installEventFilter(this);
        applySquareCorners(reinterpret_cast<HWND>(internalWinId()), isFullScreen());
        if (m_preservedNormalPlacement.isValid())
        {
            // WebEngine can recreate a maximized HWND using its current large
            // size as its restore size. Reapply the previous workspace rectangle
            // once Qt has finished showing/configuring the replacement HWND.
            const WId replacement = windowHandle()->winId();
            const QRect normal = m_preservedNormalPlacement;
            QTimer::singleShot(0, this, [this, replacement, normal] {
                if (internalWinId() != replacement || isFullScreen())
                    return;
                const auto hwnd = reinterpret_cast<HWND>(replacement);
                WINDOWPLACEMENT placement{sizeof(WINDOWPLACEMENT)};
                if (GetWindowPlacement(hwnd, &placement))
                {
                    placement.rcNormalPosition = {normal.x(), normal.y(),
                                                  normal.x() + normal.width(), normal.y() + normal.height()};
                    SetWindowPlacement(hwnd, &placement);
                }
            });
        }
    }
    return handled;
}

bool WindowController::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
    auto* msg = static_cast<MSG*>(message);
    switch (msg->message)
    {
    case WM_ACTIVATE:
    case WM_SIZE:
    {
        const int margin = isFullScreen() || IsZoomed(msg->hwnd) ? 0 : 1;
        const MARGINS margins{margin, margin, margin, margin};
        DwmExtendFrameIntoClientArea(msg->hwnd, &margins);
        applySquareCorners(msg->hwnd, isFullScreen());
        break;
    }
    case WM_NCCALCSIZE:
        if (msg->wParam)
        {
            auto* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
            if (!isFullScreen() && IsZoomed(msg->hwnd))
            {
                // A maximized HWND extends beyond the work area by its invisible
                // resize borders. Keep client widgets inside the visible area.
                MONITORINFO monitor{sizeof(MONITORINFO)};
                const HMONITOR screen = MonitorFromRect(&params->rgrc[0], MONITOR_DEFAULTTONEAREST);
                if (GetMonitorInfoW(screen, &monitor))
                    params->rgrc[0] = monitor.rcWork;
            }
            *result = 0;
            return true;
        }
        break;
    case WM_NCPAINT:
    case kNcUahDrawCaption:
    case kNcUahDrawFrame:
        *result = 0;
        return true;
    case WM_NCACTIVATE:
        // Preserve activation processing without painting the old caption.
        *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, -1);
        return true;
    case WM_NCHITTEST:
    {
        POINT point{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
        if (!isFullScreen() && !IsZoomed(msg->hwnd))
        {
            RECT frame{};
            GetWindowRect(msg->hwnd, &frame);
            const UINT dpi = GetDpiForWindow(msg->hwnd);
            const int borderX = GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi)
                              + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
            const int borderY = GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi)
                              + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
            const bool left = minimumWidth() < maximumWidth() && point.x < frame.left + borderX;
            const bool right = minimumWidth() < maximumWidth() && point.x >= frame.right - borderX;
            const bool top = minimumHeight() < maximumHeight() && point.y < frame.top + borderY;
            const bool bottom = minimumHeight() < maximumHeight() && point.y >= frame.bottom - borderY;
            if (left || right || top || bottom)
            {
                *result = top ? (left ? HTTOPLEFT : right ? HTTOPRIGHT : HTTOP)
                             : bottom ? (left ? HTBOTTOMLEFT : right ? HTBOTTOMRIGHT : HTBOTTOM)
                                      : left ? HTLEFT : HTRIGHT;
                return true;
            }
        }
        ScreenToClient(msg->hwnd, &point);
        const QPoint local(qFloor(point.x / devicePixelRatioF()),
                           qFloor(point.y / devicePixelRatioF()));
        QWidget* child = childAt(local);
        if (!isFullScreen() && child && child->objectName() == "windowMaximizeButton")
        {
            *result = HTMAXBUTTON; // Enables the Windows 11 Snap Layouts flyout.
            return true;
        }
        if (!isFullScreen() && m_titleBar && m_titleBar->geometry().contains(local))
        {
            auto* tabBar = qobject_cast<QTabBar*>(child);
            if (child == m_titleBar || child == m_dragArea
                || (tabBar && tabBar->tabAt(tabBar->mapFrom(this, local)) < 0))
            {
                *result = HTCAPTION; // Native dragging, double-click, Snap and Aero Shake.
                return true;
            }
        }
        *result = HTCLIENT;
        return true;
    }
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONDBLCLK:
        if (msg->wParam == HTMAXBUTTON)
        {
            m_maximizePressed = true;
            if (auto* button = findChild<QPushButton*>("windowMaximizeButton"))
                button->setDown(true);
            *result = 0;
            return true;
        }
        break;
    case WM_NCLBUTTONUP:
        if (msg->wParam == HTMAXBUTTON)
        {
            const bool pressed = m_maximizePressed;
            m_maximizePressed = false;
            if (auto* button = findChild<QPushButton*>("windowMaximizeButton"))
                button->setDown(false);
            if (pressed)
                PostMessageW(msg->hwnd, WM_SYSCOMMAND,
                             IsZoomed(msg->hwnd) ? SC_RESTORE : SC_MAXIMIZE, 0);
            *result = 0;
            return true;
        }
        break;
    case WM_NCMOUSEMOVE:
        if (msg->wParam == HTMAXBUTTON)
        {
            if (auto* button = findChild<QPushButton*>("windowMaximizeButton"))
            {
                button->setAttribute(Qt::WA_UnderMouse, true);
                button->update();
            }
            TRACKMOUSEEVENT tracking{sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_NONCLIENT, msg->hwnd, 0};
            TrackMouseEvent(&tracking);
        }
        else if (auto* button = findChild<QPushButton*>("windowMaximizeButton"))
        {
            m_maximizePressed = false;
            button->setDown(false);
            button->setAttribute(Qt::WA_UnderMouse, false);
            button->update();
        }
        break;
    case WM_NCMOUSELEAVE:
    case WM_CANCELMODE:
        m_maximizePressed = false;
        if (auto* button = findChild<QPushButton*>("windowMaximizeButton"))
        {
            button->setDown(false);
            button->setAttribute(Qt::WA_UnderMouse, false);
            button->update();
        }
        break;
    }
    return QWidget::nativeEvent(eventType, message, result);
}
#endif
