#include "browser_framework.h"

Browser_Framework::Browser_Framework(QWidget *parent)
    : QWidget{parent}
{

    static Mergered_TabsContent mergered_Tabs;

    m_tab_manager = mergered_Tabs.getTabManager();



    // panel with address bar, navigations, extensions
    main_pannel = new MainPannel(this);

    // panel with window hint, tabs
    header_panel = new HeaderPanel(this);


    // necessary connections
    connectNavigation();
    connectAddressBar();
    connect(m_tab_manager, &TabManager::currentUrlChanged,
            main_pannel->address_bar, &AddressBarWidget::setUrl);
    connect(m_tab_manager, &TabManager::currentUrlChanged,
            main_pannel->address_bar, &AddressBarWidget::updateSearchEngineForUrl);
    main_pannel->address_bar->setUrl(m_tab_manager->currentUrl());
    main_pannel->address_bar->updateSearchEngineForUrl(m_tab_manager->currentUrl());


    main_lay = new QVBoxLayout(this);
    main_lay->setContentsMargins(0, 0, 0, 0);
    main_lay->setSpacing(0);

    main_lay->addWidget(header_panel);
    main_lay->addWidget(main_pannel);
    main_lay->addWidget(Mergered_TabsContent::getTabContent());


    this->setLayout(main_lay);
    this->showMaximized();
}


// method connecting interactive buttons with page
void Browser_Framework::connectNavigation()
{

    connect(main_pannel->nav_wgt->back_btn, &QPushButton::clicked,
            this, &Browser_Framework::updateNavigation);
    connect(main_pannel->nav_wgt->forward_btn, &QPushButton::clicked,
            this, &Browser_Framework::updateNavigation);
    connect(main_pannel->nav_wgt->reload_btn, &QPushButton::clicked,
            this, &Browser_Framework::updateNavigation);
}

void Browser_Framework::connectAddressBar()
{

    // connecting address bar from main panel
    connect(main_pannel->address_bar, &AddressBarWidget::navigateRequested,
            this, &Browser_Framework::updateAddressBar);

    // connecting address bar from home page
    auto* homePage = dynamic_cast<HomePageTab*>(m_tab_manager->currentTab());
    if (homePage)
    {
        connect(homePage->addressBar(), &AddressBarWidget::navigateRequested,
                this, &Browser_Framework::updateAddressBar);
    }

}


void Browser_Framework::updateAddressBar(const QUrl& url)
{
    IAppTabs* tab = m_tab_manager->currentTab();

    if (tab == nullptr || tab->view() == nullptr)
    {
        m_tab_manager->createTab(url);
        return;
    }

    tab->view()->setURL(url);
}

void Browser_Framework::updateNavigation()
{
    IAppTabs* tab = m_tab_manager->currentTab();
    if (tab == nullptr || tab->view() == nullptr)
        return;

    BrowserView* view = tab->view();

    if (sender() == main_pannel->nav_wgt->back_btn)
        view->goBack();
    else if (sender() == main_pannel->nav_wgt->forward_btn)
        view->goForward();
    else if (sender() == main_pannel->nav_wgt->reload_btn)
        view->reloadPage();
}

