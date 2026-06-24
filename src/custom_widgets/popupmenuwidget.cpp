#include "popupmenuwidget.h"

PopupMenuWidget::PopupMenuWidget(QWidget *parent)
    : QWidget{parent}
{

    QVBoxLayout* mainBtn_lay = new QVBoxLayout(this);

    main_btn = new QPushButton("Open", this);
    mainBtn_lay->addWidget(main_btn);


    popup_menu = new QWidget(nullptr, Qt::Popup);
    popup_lay = new QVBoxLayout(popup_menu);
    popup_menu->setLayout(popup_lay);


    connect(main_btn, &QPushButton::clicked,
            this, &PopupMenuWidget::showPopup);


}


QPushButton* PopupMenuWidget::createBtn(QString name)
{
    QPushButton* btn = new QPushButton(name);
    btn_list.push_back(btn);

    popup_lay->addWidget(btn);

    connect(btn, SIGNAL(clicked()),
            popup_menu, SLOT(close()));

    return btn;
}

QPushButton* PopupMenuWidget::getMainBtn()
{
    return main_btn;
}

void PopupMenuWidget::showPopup()
{
    QPoint pos = main_btn->mapToGlobal(QPoint(0, main_btn->height()));

    popup_menu->move(pos);
    popup_menu->show();
}
