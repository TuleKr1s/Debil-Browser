#ifndef SEARCHENGINESELECTOR_H
#define SEARCHENGINESELECTOR_H

#include <QWidget>
#include <memory>
#include <vector>
#include <QBoxLayout>
#include <QPushButton>

#include "../custom_widgets/popupmenuwidget.h"
#include "search_engines/isearchengine.h"



class SearchEngineSelector : public QWidget
{
    Q_OBJECT
public:
    explicit SearchEngineSelector(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    ISearchEngine* getCurrentSearchEngine();
private:

    QHBoxLayout* search_choice_lay;

    PopupMenuWidget* search_choice_menu;
    std::vector<std::unique_ptr<ISearchEngine>> search_engines;
    ISearchEngine* current_search_engine;

    void setupSearchEngine();
    void setCurrentSearchEngine(ISearchEngine* search_engine);

signals:
    void engineChanged(ISearchEngine*);
};

#endif // SEARCHENGINESELECTOR_H
