#ifndef ADDRESSBARWIDGET_H
#define ADDRESSBARWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>

#include "../custom_widgets/popupmenuwidget.h"
#include "../web/search_engines/isearchengine.h"

#include <memory>
#include <vector>

class AddressBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AddressBarWidget(QWidget *parent = nullptr);

private:

    QLineEdit* address_bar;
    PopupMenuWidget* search_choice_menu;

    QHBoxLayout* addressBar_layout;
    std::vector<std::unique_ptr<ISearchEngine>> search_engines;
    ISearchEngine* current_search_engine;

    void setupSearchEngines();
    bool isUrlInput(const QString& text) const;

public slots:

    void onReturnPressed();

signals:

    void navigateRequested(const QUrl& url);
};

#endif // ADDRESSBARWIDGET_H
