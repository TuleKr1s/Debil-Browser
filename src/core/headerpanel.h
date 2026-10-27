#ifndef HEADERPANEL_H
#define HEADERPANEL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPoint>
#include <QMouseEvent>

#include "../custom_widgets/Tabs/mergered_tabscontent.h"

class HeaderPanel : public QWidget
{
    Q_OBJECT
public:
    explicit HeaderPanel(QWidget *parent = nullptr);

    QPoint getMousePrevPos();

private:

    QPoint m_mousePrevPos;
    bool m_isDragging = false;
    int widget_height = 35;

    QHBoxLayout* header_lay;

private slots:

    void setMousePrevPos(QPoint prevPos);

protected:

    void mousePressEvent(QMouseEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

};

#endif // HEADERPANEL_H
