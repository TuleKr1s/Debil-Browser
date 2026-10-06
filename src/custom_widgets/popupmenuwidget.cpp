#include "popupmenuwidget.h"

PopupMenuWidget::PopupMenuWidget(QWidget *parent)
    : QWidget{parent}
{

    QVBoxLayout* mainBtn_lay = new QVBoxLayout(this);
    mainBtn_lay->setContentsMargins(0, 0, 0, 0);

    main_btn = new QPushButton(this);
    main_btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    mainBtn_lay->addWidget(main_btn);


    popup_menu = new QWidget(nullptr, Qt::Popup);
    popup_lay = new QVBoxLayout(popup_menu);
    popup_menu->setLayout(popup_lay);


    connect(main_btn, &QPushButton::clicked,
            this, &PopupMenuWidget::showPopup);


}


QSize PopupMenuWidget::sizeHint() const
{
    return main_btn->sizeHint();
}

QPushButton* PopupMenuWidget::createBtn(QString name, QIcon icon)
{
    QPushButton* btn = new QPushButton(name, popup_menu);
    if (!icon.isNull())
        btn->setIcon(icon);
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

void PopupMenuWidget::setMainBtnText(const QString& text)
{
    main_btn->setText(text);
    main_btn->setMinimumSize(main_btn->sizeHint());
    main_btn->adjustSize();
    setMinimumSize(main_btn->sizeHint());
    updateGeometry();
}

void PopupMenuWidget::setMainBtnIcon(const QIcon& icon)
{
    main_btn->setIcon(icon);
    main_btn->adjustSize();
    updateGeometry();
}

void PopupMenuWidget::showPopup()
{
    QPoint pos = main_btn->mapToGlobal(QPoint(0, main_btn->height()));

    popup_menu->move(pos);
    popup_menu->show();
}
