#include "headerpanel.h"

HeaderPanel::HeaderPanel(QWidget *parent)
    : QWidget{parent}
{




    header_lay = new QHBoxLayout(this);
    header_lay->setContentsMargins(0, 0, 0, 0);
    header_lay->setSpacing(0);
    header_lay->addWidget(Mergered_TabsContent::getTabManager());



    this->setLayout(header_lay);
    this->setMinimumHeight(widget_height);
    this->setMaximumHeight(widget_height);
}
