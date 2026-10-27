#include "browser_framework.h"

#include <QWindow>
#include <QTabBar>
#include <QtMath>
#include <QPlatformSurfaceEvent>
#include <QTimer>
#include <QGuiApplication>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <windowsx.h>
#include <dwmapi.h>

namespace {
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

Browser_Framework::Browser_Framework(QWidget *parent)
    : QWidget{parent}
{
#ifdef Q_OS_WIN
    // Keep WS_CAPTION/WS_THICKFRAME and the system commands. Only the
    // non-client layout/painting is replaced in nativeEvent().
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                   | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    setAttribute(Qt::WA_ContentsMarginsRespectsSafeArea, false);
#endif
#endif
    static Mergered_TabsContent mergered_Tabs;

    m_tab_manager = mergered_Tabs.getTabManager();



    // panel with address bar, navigations, extensions
    main_pannel = new MainPannel(this);

    // panel with window hint, tabs
    header_panel = new HeaderPanel(this);


    // necessary connections
    connectNavigation();
    connectAddressBar();
    auto updateAddressBarMode = [this] {
        auto* tab = m_tab_manager->currentTab();
        main_pannel->address_bar->setTabContext(
            tab ? (tab->view() ? static_cast<QObject*>(tab->view()) : tab->widget()) : nullptr,
            m_tab_manager->currentUrl());
        main_pannel->address_bar->setShowDraftInDisplayMode(
            tab && tab->type() == HomePage);
    };
    connect(m_tab_manager, &TabManager::currentTabChanged,
            this, updateAddressBarMode);
    updateAddressBarMode();
    connect(m_tab_manager, &TabManager::currentUrlChanged,
            main_pannel->address_bar, &AddressBarWidget::setUrl);
    connect(m_tab_manager, &TabManager::currentTitleChanged,
            main_pannel->address_bar, &AddressBarWidget::setTitle);
    connect(m_tab_manager, &TabManager::currentUrlChanged,
            main_pannel->address_bar, &AddressBarWidget::updateSearchEngineForUrl);
    main_pannel->address_bar->setUrl(m_tab_manager->currentUrl());
    main_pannel->address_bar->setTitle(m_tab_manager->getTabTitle());
    main_pannel->address_bar->updateSearchEngineForUrl(m_tab_manager->currentUrl());


    main_lay = new QVBoxLayout(this);
    main_lay->setContentsMargins(0, 0, 0, 0);
    main_lay->setSpacing(0);

    main_lay->addWidget(header_panel);
    main_lay->addWidget(main_pannel);
    main_lay->addWidget(Mergered_TabsContent::getTabContent());


    this->setLayout(main_lay);
#ifdef Q_OS_WIN
    // Create the custom frame first so Qt does not add the standard caption
    // and border dimensions to the initial restore size.
    if (QGuiApplication::platformName() == "windows")
        winId();
#endif
    resize(916, 739);
    this->showMaximized();
#ifdef Q_OS_WIN
    if (QGuiApplication::platformName() == "windows")
        windowHandle()->installEventFilter(this);
#endif
}

#ifdef Q_OS_WIN
bool Browser_Framework::eventFilter(QObject* watched, QEvent* event)
{
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
    return QWidget::eventFilter(watched, event);
}

bool Browser_Framework::event(QEvent* event)
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

bool Browser_Framework::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
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
        if (!isFullScreen() && header_panel && header_panel->geometry().contains(local))
        {
            auto* tabBar = qobject_cast<QTabBar*>(child);
            if (child == header_panel || child == m_tab_manager
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


// method connecting interactive buttons with page
void Browser_Framework::connectNavigation()
{

    connect(main_pannel->nav_wgt->back_btn, &QPushButton::clicked,
            this, &Browser_Framework::updateNavigation);
    connect(main_pannel->nav_wgt->forward_btn, &QPushButton::clicked,
            this, &Browser_Framework::updateNavigation);
    connect(main_pannel->nav_wgt->reload_btn, &QPushButton::clicked,
            this, &Browser_Framework::updateNavigation);
}

void Browser_Framework::connectAddressBar()
{

    // connecting address bar from main panel
    connect(main_pannel->address_bar, &AddressBarWidget::navigateRequested,
            this, &Browser_Framework::updateAddressBar);

    // connecting address bar from home page
    auto* homePage = dynamic_cast<HomePageTab*>(m_tab_manager->currentTab());
    if (homePage)
    {
        connect(homePage->addressBar(), &AddressBarWidget::navigateRequested,
                this, &Browser_Framework::updateAddressBar);
    }

}


void Browser_Framework::updateAddressBar(const QUrl& url)
{
    IAppTabs* tab = m_tab_manager->currentTab();

    if (tab == nullptr || tab->view() == nullptr)
    {
        m_tab_manager->createTab(url);
        return;
    }

    tab->view()->setURL(url);
}

void Browser_Framework::updateNavigation()
{
    IAppTabs* tab = m_tab_manager->currentTab();
    if (tab == nullptr || tab->view() == nullptr)
        return;

    BrowserView* view = tab->view();

    if (sender() == main_pannel->nav_wgt->back_btn)
        view->goBack();
    else if (sender() == main_pannel->nav_wgt->forward_btn)
        view->goForward();
    else if (sender() == main_pannel->nav_wgt->reload_btn)
    {
        main_pannel->address_bar->discardDraft();
        view->reloadPage();
    }
}

