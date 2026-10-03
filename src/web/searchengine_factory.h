#ifndef SEARCHENGINE_FACTORY_H
#define SEARCHENGINE_FACTORY_H

#include <memory>
#include <vector>

#include "search_engines/googlesearchengine.h"
#include "search_engines/yandexsearchengine.h"

class SearchEngine_Factory
{
public:
    static std::unique_ptr<ISearchEngine> createGoogle()
    {
        return std::make_unique<GoogleSearchEngine>();
    }

    static std::unique_ptr<ISearchEngine> createYandex()
    {
        return std::make_unique<YandexSearchEngine>();
    }

    static std::vector<std::unique_ptr<ISearchEngine>> createAll()
    {
        std::vector<std::unique_ptr<ISearchEngine>> engines;
        engines.push_back(createGoogle());
        engines.push_back(createYandex());
        return engines;
    }
};

#endif // SEARCHENGINE_FACTORY_H
