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

    tabManager_lay = new QHBoxLayout(this);
    tabManager_lay->setContentsMargins(0,0,0,0);
    tabManager_lay->setSpacing(0);

    tabManager_lay->addWidget(m_tabBar);
    tabManager_lay->addWidget(m_newTabButton);


    connect(m_tabBar, &QTabBar::currentChanged, this, &TabManager::onCurrentTabChanged);
    connect(m_tabBar, &QTabBar::tabCloseRequested, this, &TabManager::onTabCloseRequested);
    connect(m_newTabButton, &QToolButton::clicked, this, [this]() {
        createTab(QUrl("https://www.google.com"));
    });


    this->createTab(QUrl("https://www.google.com"));
}


void TabManager::createTab(const QUrl& url)
{

    BrowserView* view = new BrowserView();

    view->setURL(url);


    BrowserTab* tab = new BrowserTab(view, "New Tab");

    m_tabs.append(tab);

    m_tabContent->addView(view);

    int index = m_tabBar->addTab(tab->title());


    m_tabBar->setCurrentIndex(index);

}


void TabManager::closeTab(int index)
{
    if (index < 0 || index >= m_tabs.size())
        return;

    BrowserTab* tab = m_tabs[index];

    BrowserView* view = tab->view();

    m_tabContent->removeView(view);

    m_tabs.removeAt(index);

    delete tab;

    m_tabBar->removeTab(index);

    if (m_tabs.isEmpty())
    {
        this->createTab(QUrl("https://www.google.com"));
    }
}

BrowserTab* TabManager::currentTab() const
{
    int index = m_tabBar->currentIndex();

    if (index < 0 || index >= m_tabs.size())
        return nullptr;

    return m_tabs[index];
}


void TabManager::onCurrentTabChanged(int index)
{
    if (index < 0 || index >= m_tabs.size())
        return;


    BrowserTab* tab = m_tabs[index];

    m_tabContent->showView(tab->view());
}

void TabManager::onTabCloseRequested(int index)
{
    this->closeTab(index);
}
















