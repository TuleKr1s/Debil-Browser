#ifndef APPTABS_FACTORY_H
#define APPTABS_FACTORY_H

#include <memory>

#include "homepagetab.h"
#include "webtab.h"

class AppTabs_Factory
{
public:
    static std::unique_ptr<IAppTabs> createWebTab(BrowserView* view, const QString& title = "New Tab")
    {
        return std::make_unique<WebTab>(view, title);
    }
    static std::unique_ptr<IAppTabs> createHomePage()
    {
        return std::make_unique<HomePageTab>();
    }
};

#endif // APPTABS_FACTORY_H
