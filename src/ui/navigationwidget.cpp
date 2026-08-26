#include "navigationwidget.h"

NavigationWidget::NavigationWidget(QWidget *parent)
    : QWidget{parent}
{

    back_btn = new QPushButton(this);
    forward_btn = new QPushButton(this);
    reload_btn = new QPushButton(this);

    back_btn->setIcon(QIcon(":/assets/left_arrow.png"));
    forward_btn->setIcon(QIcon(":/assets/right_arrow.png"));
    reload_btn->setIcon(QIcon(":/assets/refresh.png"));

    back_btn->setFixedSize(QSize(25,25));
    forward_btn->setFixedSize(QSize(25,25));
    reload_btn->setFixedSize(QSize(25,25));

    main_lay = new QHBoxLayout(this);
    main_lay->addWidget(back_btn);
    main_lay->addWidget(forward_btn);
    main_lay->addWidget(reload_btn);

    main_lay->addStretch(1);

    this->setLayout(main_lay);

}
