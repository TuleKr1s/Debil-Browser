#ifndef YANDEXSEARCHENGINE_H
#define YANDEXSEARCHENGINE_H

#include <QUrl>
#include "isearchengine.h"

class YandexSearchEngine : public ISearchEngine
{
public:

    QString createSearchURL(const QString& query) const override
    {
        return "https://yandex.ru/search/?text=" +
               QUrl::toPercentEncoding(query);
    }

    QString getSearchEngineName() const override
    {
        return "yandex";
    }
};

#endif // YANDEXSEARCHENGINE_H
