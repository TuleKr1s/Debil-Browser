#include "headerpanel.h"

HeaderPanel::HeaderPanel(QWidget *parent)
    : QWidget{parent}
{


    header_lay = new QHBoxLayout(this);
    header_lay->setContentsMargins(9, 0, 9, 0);
    header_lay->setSpacing(0);
    header_lay->addWidget(Mergered_TabsContent::getTabManager());



    this->setLayout(header_lay);
    this->setMinimumHeight(widget_height);
    this->setMaximumHeight(widget_height);
}


QPoint HeaderPanel::getMousePrevPos()
{
    return m_mousePrevPos;
}


void HeaderPanel::setMousePrevPos(QPoint prevPos)
{
    if (m_mousePrevPos == prevPos)
        return;
    m_mousePrevPos = prevPos;
}

void HeaderPanel::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        this->setMousePrevPos(e->pos());
    return QWidget::mousePressEvent(e);
}

void HeaderPanel::mouseMoveEvent(QMouseEvent* e)
{
    auto dx = e->pos().x() - m_mousePrevPos.x();
    auto dy = e->pos().y() - m_mousePrevPos.y();

    window()->setGeometry(window()->x()+dx, window()->y()+dy,\
                        window()->width(), window()->height());
    return QWidget::mouseMoveEvent(e);
}














