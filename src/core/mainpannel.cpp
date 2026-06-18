#include "mainpannel.h"

MainPannel::MainPannel(QWidget *parent)
    : QWidget{parent}
{

    nav_wgt = new NavigationWidget(this);

    pannel_lay = new QHBoxLayout(this);
    pannel_lay->addWidget(nav_wgt);


    this->setLayout(pannel_lay);
}
