#include "tabcontent.h"

TabContent::TabContent(QWidget *parent)
    : QWidget{parent}
{

    m_stackedView = new QStackedWidget(this);

    content_lay = new QVBoxLayout(this);

    content_lay->addWidget(m_stackedView);


    content_lay->setContentsMargins(0,0,0,0);
    content_lay->setSpacing(0);

    this->setLayout(content_lay);

}


void TabContent::addView(QWidget* view)
{
    m_stackedView->addWidget(view);
}

void TabContent::removeView(QWidget* view)
{
    m_stackedView->removeWidget(view);

    delete view;
}

void TabContent::showView(QWidget* view)
{
    m_stackedView->setCurrentWidget(view);
}
