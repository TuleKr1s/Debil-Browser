#ifndef BROWSERTAB_H
#define BROWSERTAB_H

#include <QString>
#include <QUrl>

#include "../core/browserview.h"

class BrowserTab
{
public:
    BrowserTab(BrowserView* view, const QString& title = "New tab");


    BrowserView* view() const
    {
        return m_view;
    }

    QString title() const
    {
        return m_title;
    }

    void setTitle(const QString& title)
    {
        m_title = title;
    }

private:

    BrowserView* m_view;
    QString m_title;

};

#endif // BROWSERTAB_H
