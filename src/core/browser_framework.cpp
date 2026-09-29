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


    IAppTabs *tab = m_tab_manager->currentTab();
    if (tab->view() == nullptr) return;
    BrowserView *view = tab->view();

    connect(main_pannel->nav_wgt->back_btn, SIGNAL(clicked()),
            view, SLOT(goBack()));
    connect(main_pannel->nav_wgt->forward_btn, SIGNAL(clicked()),
            view, SLOT(goForward()));
    connect(main_pannel->nav_wgt->reload_btn, SIGNAL(clicked()),
            view, SLOT(reloadPage()));
}

void Browser_Framework::connectAddressBar()
{

    connect(main_pannel->address_bar, &AddressBarWidget::navigateRequested,
            this, &Browser_Framework::updateAddressBar);

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


