#ifndef GOOGLESEARCHENGINE_H
#define GOOGLESEARCHENGINE_H

#include <QUrl>
#include "isearchengine.h"

class GoogleSearchEngine : public ISearchEngine
{
public:

    QString createSearchURL(const QString& query) const override
    {
        return "https://www.google.com/search?q=" + QUrl::toPercentEncoding(query);
    }

    QString getSearchEngineName() const override
    {
        return "google";
    }
};

#endif // GOOGLESEARCHENGINE_H
