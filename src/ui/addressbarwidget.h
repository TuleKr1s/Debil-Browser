#ifndef ADDRESSBARWIDGET_H
#define ADDRESSBARWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QBoxLayout>
#include <QLabel>
#include <QStackedLayout>
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
    void setTitle(const QString& title);
    void updateSearchEngineForUrl(const QUrl& url);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QLineEdit* address_bar;
    QWidget* display_widget;
    QLabel* url_label;
    QLabel* title_label;
    QStackedLayout* address_stack;

    QHBoxLayout* addressBar_layout;
    ISearchEngine* current_search_engine;
    SearchEngineSelector* search_choice_menu;

    QUrl current_url;
    QString current_title;

    bool isUrlInput(const QString& text) const;
    void showDisplayMode();
    void showEditMode();
    void updateDisplayText();

public slots:

    void onReturnPressed();

private slots:

    void setSearchEngine(ISearchEngine*);

signals:

    void navigateRequested(const QUrl& url);
};

#endif // ADDRESSBARWIDGET_H
