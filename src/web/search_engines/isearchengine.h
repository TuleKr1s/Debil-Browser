#ifndef ISEARCHENGINE_H
#define ISEARCHENGINE_H

#include <QString>
#include <QUrl>
#include <QUrlQuery>

class ISearchEngine
{
public:
    virtual ~ISearchEngine() = default;

    virtual QString createSearchURL(const QString& query) const = 0;
    virtual QString getSearchEngineName() const = 0;

    virtual bool matchesSearchUrl(const QUrl& url) const
    {
        static const QString probeQuery = QStringLiteral("debil_browser_search_probe");
        const QUrl searchUrl(createSearchURL(probeQuery));

        if (!searchUrl.isValid() ||
            searchUrl.host().compare(url.host(), Qt::CaseInsensitive) != 0 ||
            searchUrl.path() != url.path())
        {
            return false;
        }

        const QUrlQuery expectedQuery(searchUrl);
        const QUrlQuery actualQuery(url);
        for (const auto& item : expectedQuery.queryItems())
        {
            if (item.second == probeQuery && !actualQuery.hasQueryItem(item.first))
                return false;
        }

        return true;
    }
};

#endif // ISEARCHENGINE_H
