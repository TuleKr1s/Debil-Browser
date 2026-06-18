#include "browser_framework.h"

Browser_Framework::Browser_Framework(QWidget *parent)
    : QWidget{parent}
{

    viewing_window = new BrowserView(this);


    main_pannel = new MainPannel(this);


    connectNavigation();


    main_lay = new QVBoxLayout(this);
    main_lay->addWidget(main_pannel);
    main_lay->addWidget(viewing_window);

    this->setLayout(main_lay);

}


// method connecting interactive buttons with page
void Browser_Framework::connectNavigation()
{
    connect(main_pannel->nav_wgt->back_btn, SIGNAL(clicked()),
            viewing_window, SLOT(goBack()));
    connect(main_pannel->nav_wgt->forward_btn, SIGNAL(clicked()),
            viewing_window, SLOT(goForward()));
    connect(main_pannel->nav_wgt->reload_btn, SIGNAL(clicked()),
            viewing_window, SLOT(reloadPage()));
}
