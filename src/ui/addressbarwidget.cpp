#include "addressbarwidget.h"

AddressBarWidget::AddressBarWidget(QWidget *parent)
    : QWidget{parent}
{

    address_bar = new QLineEdit();

    connect(address_bar, SIGNAL(returnPressed()),
            this, SLOT(onReturnPressed()));


    addressBar_layout = new QHBoxLayout();
    addressBar_layout->addWidget(address_bar);


    this->setLayout(addressBar_layout);

}


void AddressBarWidget::onReturnPressed()
{

    QString text = address_bar->text();

    if (!text.startsWith("http://") &&
        !text.startsWith("https://"))
    {
        text = "https://" + text;
    }

    emit navigateRequested(QUrl(text));

}
