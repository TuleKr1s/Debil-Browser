#include "tabmanager.h"

#include <QWebEngineNewWindowRequest>


TabManager::TabManager(TabContent* tab_content, QWidget *parent)
    : QWidget{parent}, m_tabContent(tab_content)
{

    m_tabBar = new QTabBar(this);
    m_tabBar->setTabsClosable(true);
    m_tabBar->setMovable(true);
    m_tabBar->setExpanding(false);
    m_tabBar->setElideMode(Qt::ElideRight);
    m_tabBar->setStyleSheet("QTabBar::tab { width: 180px; }");


    m_newTabButton = new QToolButton(this);
    m_newTabButton->setCheckable(true);
    m_newTabButton->setToolTip(tr("New tab"));
    m_newTabButton->setAutoRaise(true);
    m_newTabButton->setFixedSize(28, 28);
    m_newTabButton->setStyleSheet("QToolButton {background-color: #E5E5E5;"
                                  "             border: 1px solid #C8C8C8;}\
                                   QToolButton:hover {background-color: #D0D0D0;}"
                                  "QToolButton:checked {background-color: #D0D0D0;}");
    this->updateNewTabBtn();


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
        this->onNewTabClicked();
    });

    connect(m_tabBar, &QTabBar::tabMoved,
            this, &TabManager::onTabMoved);

    // only one home page tab
    // deletion will result in program closing
    // always have index: 0
    this->createTab(HomePage);
    const int homepage_index = m_tabBar->currentIndex();

    // invisible tab to warmup QWebEngine
    // never be visible or deleted
    // always have index: 1
    // m_warmup_view = createWebTab();
    // m_warmup_view->setURL(QUrl("about:blank"));
    // const int warmup_index = m_tabBar->currentIndex();
    // m_tabBar->setTabVisible(warmup_index, false);
    // m_tabBar->setCurrentIndex(homepage_index);
}


void TabManager::createTab(const QUrl& url)
{
    BrowserView* view = createWebTab();
    view->setURL(url);
}

BrowserView* TabManager::createWebTab()
{
    qDebug() << "creating tab";
    BrowserView* view = new BrowserView(m_tabContent);
    m_tabContent->addView(view);

    auto tab = AppTabs_Factory::createWebTab(view, "New Tab");

    const QString title = tab->title();
    m_tabs.push_back(std::move(tab));



    int index = m_tabBar->addTab(title);


    m_tabBar->setCurrentIndex(index);
    this->updateNewTabBtn();

    connect(view->page(), &QWebEnginePage::newWindowRequested, this,
            [this](QWebEngineNewWindowRequest& request)
            {
                BrowserView* newView = createWebTab();
                request.openIn(newView->page());
            });

    connect(view, &QWebEngineView::urlChanged, this,
            [this, view](const QUrl& url)
            {
                IAppTabs* activeTab = currentTab();
                if (activeTab != nullptr && view == activeTab->view())
                    emit currentUrlChanged(url);
            });

    connect(view, &QWebEngineView::titleChanged, this,
            [this, view](const QString& title)
            {
                for (int index = 0; index < static_cast<int>(m_tabs.size()); ++index)
                {
                    if (m_tabs[index]->view() != view)
                        continue;

                    const QString tabTitle = title.isEmpty() ? tr("New Tab") : title;
                    m_tabs[index]->setTitle(tabTitle);
                    m_tabBar->setTabText(index, tabTitle);

                    if (index == m_tabBar->currentIndex())
                        emit currentTitleChanged(tabTitle);

                    break;
                }
            });

    return view;
}

void TabManager::createTab(const TabType tab_type)
{

    std::unique_ptr<IAppTabs> tab;


    switch(tab_type)
    {
    case HomePage:
        tab = AppTabs_Factory::createHomePage();

        break;
    default:
        break;
    }

    QString title = tab->title();


    m_tabContent->addView(tab->widget());

    int index = m_tabBar->addTab(title);

    m_tabs.push_back(std::move(tab));

    m_tabBar->setCurrentIndex(index);

    if (tab_type == HomePage)
        m_tabBar->setTabVisible(index, false);

    this->updateNewTabBtn();


}

void TabManager::onNewTabClicked()
{
    IAppTabs* current = currentTab();
    if (!current)
        return;

    if (current->type() == HomePage)
    {
        if (!m_lastWebView)
            return;

        for (int index = 0; index < static_cast<int>(m_tabs.size()); ++index)
        {
            if (m_tabs[index]->view() == m_lastWebView)
            {
                m_tabBar->setCurrentIndex(index);
                return;
            }
        }

        return;
    }

    for (int index = 0; index < static_cast<int>(m_tabs.size()); ++index)
    {
        if (m_tabs[index]->type() == HomePage)
        {
            m_tabBar->setCurrentIndex(index);
            return;
        }
    }
}

void TabManager::closeTab(int index)
{


    if (index < 0 || index >= static_cast<int>(m_tabs.size()))
        return;
    // if (m_tabs[index]->view() == m_warmup_view)
    //     return;

    IAppTabs* tab = m_tabs[index].get();

    if (tab->view() != nullptr) {
        BrowserView* view = tab->view();

        m_tabContent->removeView(view);

    }

    m_tabs.erase(m_tabs.begin() + index);

    m_tabBar->removeTab(index);

    if (m_tabs.empty())
    {
        this->createTab(HomePage);
    }

    if (m_tabBar->currentIndex() == -1)
    {
        for (int homeIndex = 0; homeIndex < static_cast<int>(m_tabs.size()); ++homeIndex)
        {
            if (m_tabs[homeIndex]->type() == HomePage)
            {
                m_tabBar->setCurrentIndex(homeIndex);
                break;
            }
        }
    }

    this->updateNewTabBtn();
}

IAppTabs* TabManager::currentTab() const
{
    int index = m_tabBar->currentIndex();

    if (index < 0 || index >= static_cast<int>(m_tabs.size()))
        return nullptr;

    return m_tabs[index].get();
}

QUrl TabManager::currentUrl() const
{
    IAppTabs* tab = currentTab();
    return tab != nullptr && tab->view() != nullptr ? tab->view()->getURL() : QUrl();
}


void TabManager::onCurrentTabChanged(int index)
{
    if (index < 0 || index >= static_cast<int>(m_tabs.size()))
        return;

    m_tabBar->setCurrentIndex(index);
    IAppTabs* tab = m_tabs[index].get();

    if (tab->view() != nullptr)
    {
        m_lastWebView = tab->view();
        m_tabContent->showView(tab->view());
    }
    else
        m_tabContent->showView(tab->widget());

    this->updateNewTabBtn();

    emit currentTabChanged(index);
    emit currentUrlChanged(currentUrl());
    emit currentTitleChanged(getTabTitle());
}

void TabManager::onTabCloseRequested(int index)
{
    this->closeTab(index);
}


void TabManager::onTabMoved(int from, int to)
{
    if (from == to)
        return;


    if (from < to) {
        std::rotate(m_tabs.begin() + from,
                    m_tabs.begin() + from + 1,
                    m_tabs.begin() + to + 1);
    } else {
        std::rotate(m_tabs.begin() + to,
                    m_tabs.begin() + from,
                    m_tabs.begin() + from + 1);
    }

    m_tabContent->moveView(from, to);
}

void TabManager::updateNewTabBtn()
{
    // button visibility based on tabs count
    if (m_tabBar->count() <= 1) // has only home page or nothing
        m_newTabButton->hide();
    else
        m_newTabButton->show(); // has another tabs

    IAppTabs* tab = currentTab();
    // dysplaying specific symbol depending on current tab
    if (tab && tab->type() == HomePage) // current tab is home page
    {
        m_newTabButton->setText("X");
        m_newTabButton->setChecked(true);
    }
    else
    {
        m_newTabButton->setText("+"); // another tabs
        m_newTabButton->setChecked(false);
    }

}

QString TabManager::getTabTitle() const
{
    IAppTabs* tab = currentTab();
    if (tab == nullptr || tab->type() == HomePage)
        return QString();

    return tab->title();
}









