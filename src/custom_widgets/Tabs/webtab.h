#ifndef BROWSERTAB_H
#define BROWSERTAB_H

#include <QString>
#include <QUrl>

#include "browserview.h"
#include "iapptabs.h"

class WebTab : public IAppTabs
{
public:
    WebTab(BrowserView* view, const QString& title = "New tab");

    BrowserView* view() const override
    {
        return m_view;
    }

    QString title() const override
    {
        return m_title;
    }

    void setTitle(const QString& title) override
    {
        m_title = title;
    }

    TabType type() const override
    {
        return m_tab_type;
    }

private:

    BrowserView* m_view;
    QString m_title;

    TabType m_tab_type {TabType::Web};

};

#endif // BROWSERTAB_H
