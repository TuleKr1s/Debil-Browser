#ifndef IAPPTABS_H
#define IAPPTABS_H

#include <QString>

#include "browserview.h"

enum TabType {
    WebTab,
    HomePage
};

class IAppTabs
{
public:
    virtual ~IAppTabs() = default;

    virtual BrowserView* view() const {return nullptr;}
    virtual QWidget* widget() {return nullptr;}
    virtual QString title() const = 0;
    virtual void setTitle(const QString& title) = 0;
    virtual TabType type() const = 0;

};

#endif // IAPPTABS_H
