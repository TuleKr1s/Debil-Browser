#include "tabmanager.h"


TabManager::TabManager(TabContent* tab_content, QWidget *parent)
    : QWidget{parent}, m_tabContent(tab_content)
{

    m_tabBar = new QTabBar(this);
    m_tabBar->setTabsClosable(true);
    m_tabBar->setMovable(true);
    m_tabBar->setExpanding(false);
    m_tabBar->setElideMode(Qt::ElideRight);

    m_newTabButton = new QToolButton(this);
    m_newTabButton->setText("+");
    m_newTabButton->setToolTip(tr("New tab"));
    m_newTabButton->setAutoRaise(true);
    m_newTabButton->setFixedSize(28, 28);
    m_newTabButton->setStyleSheet("QToolButton {background-color: #E5E5E5;"
                                  "             border: 1px solid #C8C8C8;}\
                                   QToolButton:hover {background-color: #D0D0D0;}");


    m_windowBar = new WindowBar(this);


    tabManager_lay = new QHBoxLayout(this);
    tabManager_lay->setContentsMargins(0,0,0,0);
    tabManager_lay->setSpacing(0);

    tabManager_lay->addWidget(m_tabBar);
    tabManager_lay->addWidget(m_newTabButton);
    tabManager_lay->addStretch(1);
    tabManager_lay->addWidget(m_windowBar);


    connect(m_tabBar, &QTabBar::currentChanged, this, &TabManager::onCurrentTabChanged);
    connect(m_tabBar, &QTabBar::tabCloseRequested, this, &TabManager::onTabCloseRequested);
    connect(m_newTabButton, &QToolButton::clicked, this, [this]() {
        this->createTab(QUrl("https://www.google.com"));
    });


    this->createTab(QUrl("https://www.google.com"));
}


void TabManager::createTab(const QUrl& url)
{

    BrowserView* view = new BrowserView();

    view->setURL(url);

    auto tab = AppTabs_Factory::createWebTab(view, "New Tab");

    const QString title = tab->title();
    m_tabs.push_back(std::move(tab));

    m_tabContent->addView(view);

    int index = m_tabBar->addTab(title);


    m_tabBar->setCurrentIndex(index);

}

void TabManager::closeTab(int index)
{
    if (index < 0 || index >= static_cast<int>(m_tabs.size()))
        return;

    IAppTabs* tab = m_tabs[index].get();

    BrowserView* view = tab->view();

    m_tabContent->removeView(view);

    m_tabs.erase(m_tabs.begin() + index);

    m_tabBar->removeTab(index);

    if (m_tabs.empty())
    {
        this->createTab(QUrl("https://www.google.com"));
    }
}

IAppTabs* TabManager::currentTab() const
{
    int index = m_tabBar->currentIndex();

    if (index < 0 || index >= static_cast<int>(m_tabs.size()))
        return nullptr;

    return m_tabs[index].get();
}


void TabManager::onCurrentTabChanged(int index)
{
    if (index < 0 || index >= static_cast<int>(m_tabs.size()))
        return;


    IAppTabs* tab = m_tabs[index].get();

    m_tabContent->showView(tab->view());
}

void TabManager::onTabCloseRequested(int index)
{
    this->closeTab(index);
}















