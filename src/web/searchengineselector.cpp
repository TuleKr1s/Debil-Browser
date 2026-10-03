#include "searchengineselector.h"

#include "searchengine_factory.h"

#include <QDebug>
#include <QSizePolicy>


SearchEngineSelector::SearchEngineSelector(QWidget *parent)
    : QWidget{parent}
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    search_choice_menu = new PopupMenuWidget(this);
    current_search_engine = nullptr;

    search_choice_lay = new QHBoxLayout(this);
    search_choice_lay->setContentsMargins(0, 0, 0, 0);
    search_choice_lay->addWidget(search_choice_menu);


    this->setLayout(search_choice_lay);

    setupSearchEngine();

}


QSize SearchEngineSelector::sizeHint() const
{
    return search_choice_menu->sizeHint();
}

void SearchEngineSelector::setupSearchEngine()
{
    search_engines = SearchEngine_Factory::createAll();


    for (const auto& search_engine : search_engines)
    {
        QPushButton* search_engine_btn = search_choice_menu->createBtn(search_engine->getSearchEngineName());
        ISearchEngine* engine = search_engine.get();

        connect(search_engine_btn, &QPushButton::clicked, this, [this, engine]()
        {
            setCurrentSearchEngine(engine);
        });
    }

    // default search engine
    if (!search_engines.empty())
    {
        setCurrentSearchEngine(search_engines.front().get());
    }
}


ISearchEngine* SearchEngineSelector::getCurrentSearchEngine()
{
    return current_search_engine;
}

bool SearchEngineSelector::selectSearchEngine(const QString& name)
{
    for (const auto& searchEngine : search_engines)
    {
        if (searchEngine->getSearchEngineName().compare(name, Qt::CaseInsensitive) == 0)
        {
            if (current_search_engine != searchEngine.get())
                setCurrentSearchEngine(searchEngine.get());
            return true;
        }
    }

    return false;
}

bool SearchEngineSelector::selectSearchEngineForUrl(const QUrl& url)
{
    for (const auto& searchEngine : search_engines)
    {
        if (!searchEngine->matchesSearchUrl(url))
            continue;

        if (current_search_engine != searchEngine.get())
            setCurrentSearchEngine(searchEngine.get());
        return true;
    }

    return false;
}

void SearchEngineSelector::setCurrentSearchEngine(ISearchEngine* search_engine)
{
    if (!search_engine)
    {
        return;
    }

    current_search_engine = search_engine;
    search_choice_menu->setMainBtnText(current_search_engine->getSearchEngineName());
    setMinimumSize(sizeHint());
    updateGeometry();

    qDebug() << "search engine changed" << current_search_engine->getSearchEngineName();
    emit engineChanged(current_search_engine);
}












