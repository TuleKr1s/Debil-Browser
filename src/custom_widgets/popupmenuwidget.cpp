#include "popupmenuwidget.h"

PopupMenuWidget::PopupMenuWidget(QWidget *parent)
    : QWidget{parent, Qt::Popup}
{



    popup_lay = new QVBoxLayout(this);
    foreach (QPushButton* btn, btn_list) {
        popup_lay->addWidget(btn);
    }

    this->setLayout(popup_lay);
}


QPushButton* PopupMenuWidget::createBtn(QString name)
{
    QPushButton* btn = new QPushButton();
    btn_list.push_back(btn);

    emit btnCreated();

    return btn;
}

void PopupMenuWidget::update()
{
    foreach (QPushButton* btn, btn_list) {
        popup_lay->addWidget(btn);
    }
}
