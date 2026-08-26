#include "windowbar.h"

WindowBar::WindowBar(QWidget *parent)
    : QWidget{parent}
{

    m_menuBtn = new QPushButton(this);
    m_minimizeBtn = new QPushButton(this);
    m_fullWindowBtn = new QPushButton(this);
    m_closeWindowBtn = new QPushButton(this);

    m_menuBtn->setIcon(QIcon(":/assets/menu_btn.png"));
    m_minimizeBtn->setIcon(QIcon(":/assets/minimize_btn.png"));
    //m_fullWindowBtn->setIcon(QIcon(":/assets/half_window_btn.png"));
    m_closeWindowBtn->setIcon(QIcon(":/assets/close_btn.png"));

    window()->isMaximized() ? m_fullWindowBtn->
                              setIcon(QIcon(":/assets/full_window_btn.png")) :
                            m_fullWindowBtn->
                                setIcon(QIcon(":/assets/half_window_btn.png"));



    connect(m_closeWindowBtn, &QPushButton::clicked,
            this, &WindowBar::onCloseBtnClicked);

    connect(m_minimizeBtn, &QPushButton::clicked,
            this, &WindowBar::onMinimizeBtnClicked);

    connect(m_fullWindowBtn, &QPushButton::clicked,
            this, &WindowBar::onFullWindowBtnClicked);

    window_bar_lay = new QHBoxLayout(this);
    window_bar_lay->addWidget(m_menuBtn);
    window_bar_lay->addWidget(m_minimizeBtn);
    window_bar_lay->addWidget(m_fullWindowBtn);
    window_bar_lay->addWidget(m_closeWindowBtn);





    window_bar_lay->setContentsMargins(0,0,0,0);
    window_bar_lay->setSpacing(0);

    this->setLayout(window_bar_lay);

}



void WindowBar::onCloseBtnClicked()
{
    window()->close();
}


void WindowBar::onMinimizeBtnClicked()
{
    window()->showMinimized();
}


void WindowBar::onFullWindowBtnClicked()
{
    if (window()->isMaximized()) {
        window()->showNormal();
        m_fullWindowBtn->setIcon(QIcon(":/assets/full_window_btn.png"));
    }
    else {
        window()->showMaximized();
        m_fullWindowBtn->setIcon(QIcon(":/assets/half_window_btn.png"));
    }

}


void WindowBar::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        if (window()->isMaximized())
            m_fullWindowBtn->setIcon(QIcon(":/assets/full_window_btn.png"));
        else
            m_fullWindowBtn->setIcon(QIcon(":/assets/half_window_btn.png"));
    }

    QWidget::changeEvent(event);
}







