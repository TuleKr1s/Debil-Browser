#include "mainpannel.h"

MainPannel::MainPannel(QWidget *parent)
    : QWidget{parent}
{

    m_bookmark_btn = new QPushButton(this);
    m_bookmark_btn->setToolTip("Add page to bookmark");
    m_bookmark_btn->setIcon(QIcon(":/assets/MainPanel/add_bookmark_btn.png"));


    nav_wgt = new NavigationWidget(this);
    address_bar = new AddressBarWidget(this);

    pannel_lay = new QHBoxLayout(this);

    pannel_lay->addWidget(nav_wgt);
    pannel_lay->addWidget(m_bookmark_btn);
    pannel_lay->addWidget(address_bar, 1);
    pannel_lay->addStretch(1);

    this->setLayout(pannel_lay);
    this->setMinimumHeight(widget_height);
    this->setMaximumHeight(widget_height);

}
