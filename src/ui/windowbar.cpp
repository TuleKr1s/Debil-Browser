#include "windowbar.h"

WindowBar::WindowBar(QWidget *parent)
    : QWidget{parent}
{

    m_menuBtn = new QPushButton(this);
    m_minimizeBtn = new QPushButton(this);
    m_fullWindowBtn = new QPushButton(this);
    m_closeWindowBtn = new QPushButton(this);

    full_window_btn.addFile(":/assets/WindowBar/full_window_btn.png");
    half_window_btn.addFile(":/assets/WindowBar/half_window_btn.png");
    menu_btn.addFile(":/assets/WindowBar/menu_btn.png");
    minimize_btn.addFile(":/assets/WindowBar/minimize_btn.png");
    close_btn.addFile(":/assets/WindowBar/close_btn.png");

    m_menuBtn->setIcon(menu_btn);
    m_minimizeBtn->setIcon(minimize_btn);
    m_closeWindowBtn->setIcon(close_btn);



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
    }
    else {
        window()->showMaximized();
    }

}

void WindowBar::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);


    // If these lines are placed in the constructor,
    // the top level widget will be TabManager rather than main window
    window()->installEventFilter(this);
    window()->isMaximized() ? m_fullWindowBtn->
                              setIcon(half_window_btn) :
                                m_fullWindowBtn->
                                setIcon(full_window_btn);
}

bool WindowBar::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == window() && event->type() == QEvent::WindowStateChange)
    {
        if(window()->isMaximized())
            m_fullWindowBtn->setIcon(half_window_btn);
        else
            m_fullWindowBtn->setIcon(full_window_btn);

    }

    return QWidget::eventFilter(obj, event);
}







