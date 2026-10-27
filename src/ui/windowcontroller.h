#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include <QPoint>
#include <QPointer>
#include <QRect>
#include <QSize>
#include <QWidget>

// Base window with custom chrome and native Windows behavior.
class WindowController : public QWidget
{
    Q_OBJECT
public:
    explicit WindowController(QWidget* parent = nullptr);

    void initializeWindow(QWidget* titleBar, QWidget* dragArea = nullptr,
                          QSize defaultSize = QSize(916, 739));

    static void closeWindow(QWidget* window);
    static void minimizeWindow(QWidget* window);
    static void toggleMaximized(QWidget* window);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
    bool event(QEvent* event) override;
#endif

private:
    bool isDragArea(const QPoint& windowPosition) const;
    QPointer<QWidget> m_titleBar;
    QPointer<QWidget> m_dragArea;
    bool m_isDragging = false;
    QPoint m_previousGlobalPosition;
#ifdef Q_OS_WIN
    bool m_maximizePressed = false;
    QRect m_preservedNormalPlacement;
#endif
};

#endif // WINDOWCONTROLLER_H
