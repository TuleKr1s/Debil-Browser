#ifndef IAPPTABS_H
#define IAPPTABS_H

#include <QString>

#include "../core/browserview.h"

class IAppTabs
{
public:
    virtual ~IAppTabs() = default;

    virtual BrowserView* view() const {return nullptr;}
    virtual QString title() const = 0;
    virtual void setTitle(const QString& title) = 0;
};

#endif // IAPPTABS_H
