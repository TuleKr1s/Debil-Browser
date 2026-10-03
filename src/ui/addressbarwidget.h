#ifndef ADDRESSBARWIDGET_H
#define ADDRESSBARWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QUrl>


#include "../web/searchengineselector.h"


class AddressBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AddressBarWidget(QWidget *parent = nullptr);

    // remove choice search button as needed
    void setSearchBtnVisible(bool);

public slots:
    void setUrl(const QUrl& url);
    void updateSearchEngineForUrl(const QUrl& url);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:

    QLineEdit* address_bar;


    QHBoxLayout* addressBar_layout;
    ISearchEngine* current_search_engine;
    SearchEngineSelector* search_choice_menu;


    bool isUrlInput(const QString& text) const;

public slots:

    void onReturnPressed();

private slots:

    void setSearchEngine(ISearchEngine*);

signals:

    void navigateRequested(const QUrl& url);
};

#endif // ADDRESSBARWIDGET_H
