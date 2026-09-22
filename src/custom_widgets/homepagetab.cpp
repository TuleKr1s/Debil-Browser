#include "homepagetab.h"

HomePageTab::HomePageTab(QWidget *parent)
    : QWidget{parent}
{

    homepage_lay = new QGridLayout(this);



    this->setLayout(homepage_lay);
}

QWidget* HomePageTab::widget()
{
    return this;
}

QString HomePageTab::title() const
{
    return m_title;
}

void HomePageTab::setTitle(const QString& title)
{
    m_title = title;
}
