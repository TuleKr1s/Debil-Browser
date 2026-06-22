#ifndef SEARCHENGINE_FACTORY_H
#define SEARCHENGINE_FACTORY_H

#include <memory>

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
};

#endif // SEARCHENGINE_FACTORY_H
