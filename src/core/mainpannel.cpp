#include "mainpannel.h"

MainPannel::MainPannel(QWidget *parent)
    : QWidget{parent}
{

    nav_wgt = new NavigationWidget(this);
    address_bar = new AddressBarWidget(this);

    pannel_lay = new QHBoxLayout(this);
    pannel_lay->addWidget(nav_wgt);
    pannel_lay->addWidget(address_bar);


    this->setLayout(pannel_lay);
}
