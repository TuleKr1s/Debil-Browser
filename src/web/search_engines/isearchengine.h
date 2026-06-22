#ifndef ISEARCHENGINE_H
#define ISEARCHENGINE_H

#include <QString>

class ISearchEngine
{
public:
    virtual ~ISearchEngine() = default;

    virtual QString createSearchURL(const QString& query) const = 0;
    virtual QString getSearchEngineName() const = 0;
};

#endif // ISEARCHENGINE_H
