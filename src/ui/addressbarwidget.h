#ifndef ADDRESSBARWIDGET_H
#define ADDRESSBARWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QBoxLayout>
#include <QStackedLayout>
#include <QUrl>

#include "../web/searchengineselector.h"

class AddressBarDisplayWidget;

class AddressBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AddressBarWidget(QWidget *parent = nullptr);
    ~AddressBarWidget() override;

    // remove choice search button as needed
    void setSearchBtnVisible(bool);
    void setDisplayWidgetEnabled(bool enabled);
    void setPlaceholderText(const QString& text);
    void setShowDraftInDisplayMode(bool enabled);

public slots:
    void setUrl(const QUrl& url);
    void setTitle(const QString& title);
    void updateSearchEngineForUrl(const QUrl& url);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QLineEdit* address_bar;
    AddressBarDisplayWidget* display_widget;
    QStackedLayout* address_stack;


    QHBoxLayout* addressBar_layout;
    ISearchEngine* current_search_engine;
    SearchEngineSelector* search_choice_menu;

    QUrl current_url;
    bool display_widget_enabled;
    bool has_unsubmitted_text = false;
    bool show_draft_in_display_mode = true;

    bool isUrlInput(const QString& text) const;
    void showDisplayMode();
    void showEditMode();

public slots:

    void onReturnPressed();

private slots:

    void setSearchEngine(ISearchEngine*);

signals:

    void navigateRequested(const QUrl& url);
};

#endif // ADDRESSBARWIDGET_H
