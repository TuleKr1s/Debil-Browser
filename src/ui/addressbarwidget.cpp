#include "addressbarwidget.h"
#include "addressbardisplaywidget.h"
#include "../web/searchengine_factory.h"

#include <QApplication>
#include <QRegularExpression>
#include <QEvent>
#include <QMouseEvent>
#include <QUrl>
#include <QVariant>

namespace {
QString textForEditing(const QUrl& url)
{
    const auto engines = SearchEngine_Factory::createAll();
    const QString probe = QStringLiteral("debil_browser_search_probe");
    for (const auto& engine : engines)
    {
        if (!engine->matchesSearchUrl(url))
            continue;

        const QUrlQuery templateQuery(QUrl(engine->createSearchURL(probe)));
        for (const auto& item : templateQuery.queryItems())
        {
            if (item.second == probe)
                return QUrlQuery(url).queryItemValue(item.first, QUrl::FullyDecoded);
        }
    }
    return url.toDisplayString(QUrl::FullyDecoded);
}
}

AddressBarWidget::AddressBarWidget(QWidget *parent)
    : QWidget{parent}
    , display_widget_enabled(true)
{
    address_bar = new QLineEdit(this);
    address_bar->setFrame(false);
    address_bar->setTextMargins(8, 0, 0, 0);
    display_widget = new AddressBarDisplayWidget(this);

    address_stack = new QStackedLayout();
    address_stack->setContentsMargins(0, 0, 0, 0);
    address_stack->addWidget(display_widget);
    address_stack->addWidget(address_bar);
    address_stack->setCurrentWidget(display_widget);

    qApp->installEventFilter(this);

    current_search_engine = nullptr;
    search_choice_menu = new SearchEngineSelector(this);
    connect(search_choice_menu, &SearchEngineSelector::engineChanged,
            this, &AddressBarWidget::setSearchEngine);
    setSearchEngine(search_choice_menu->getCurrentSearchEngine());

    connect(address_bar, &QLineEdit::returnPressed,
            this, &AddressBarWidget::onReturnPressed);
    connect(address_bar, &QLineEdit::textEdited, this, [this] {
        has_unsubmitted_text = true;
    });
    connect(display_widget, &AddressBarDisplayWidget::editRequested,
            this, &AddressBarWidget::showEditMode);


    addressBar_layout = new QHBoxLayout();
    addressBar_layout->addWidget(search_choice_menu);
    addressBar_layout->addLayout(address_stack, 1);


    this->setLayout(addressBar_layout);

    this->setMinimumWidth(325);
    //this->setMaximumWidth(1000);
    this->setFixedHeight(32);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // temp measure !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    this->setObjectName("addressBar");
    this->setAttribute(Qt::WA_StyledBackground, true);

    this->setStyleSheet("AddressBarWidget#addressBar {"
                        "   border: 1px solid #555;"
                        "   background-color: white;"
                        "}");

    addressBar_layout->setContentsMargins(2,2,2,2);




}

AddressBarWidget::~AddressBarWidget()
{
    qApp->removeEventFilter(this);
}

void AddressBarWidget::setUrl(const QUrl& url)
{
    current_url = url;
    if (!has_unsubmitted_text)
        address_bar->setText(url.toDisplayString(QUrl::FullyDecoded));
    display_widget->setUrl(url);
}

void AddressBarWidget::setTabContext(QObject* tab, const QUrl& url)
{
    if (current_tab_context == tab)
        return;

    if (current_tab_context)
    {
        current_tab_context->setProperty("addressBarDraft",
            has_unsubmitted_text ? QVariant(address_bar->text()) : QVariant());
        current_tab_context->setProperty("addressBarDraftCursor", address_bar->cursorPosition());
    }

    address_bar->clearFocus();
    current_tab_context = tab;
    const QVariant draft = tab ? tab->property("addressBarDraft") : QVariant();
    has_unsubmitted_text = draft.isValid();
    current_url = url;
    address_bar->setText(has_unsubmitted_text ? draft.toString()
                                            : url.toDisplayString(QUrl::FullyDecoded));
    if (has_unsubmitted_text)
        address_bar->setCursorPosition(tab->property("addressBarDraftCursor").toInt());
    display_widget->setUrl(url);
    showDisplayMode();
}

void AddressBarWidget::discardDraft()
{
    has_unsubmitted_text = false;
    if (current_tab_context)
    {
        current_tab_context->setProperty("addressBarDraft", QVariant());
        current_tab_context->setProperty("addressBarDraftCursor", QVariant());
    }
    address_bar->setText(current_url.toDisplayString(QUrl::FullyDecoded));
    display_widget->setDraftText(QString(), false);
}

void AddressBarWidget::setTitle(const QString& title)
{
    display_widget->setTitle(title);
}

void AddressBarWidget::updateSearchEngineForUrl(const QUrl& url)
{
    search_choice_menu->selectSearchEngineForUrl(url);
}

bool AddressBarWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress && address_bar->hasFocus())
    {
        const auto* mouse_event = static_cast<QMouseEvent*>(event);
        const bool clicked_inside_editor = address_bar->rect().contains(
            address_bar->mapFromGlobal(mouse_event->globalPosition().toPoint()));

        if (!clicked_inside_editor)
        {
            address_bar->clearFocus();
            showDisplayMode();
        }
    }

    if (display_widget_enabled && watched == address_bar && event->type() == QEvent::FocusOut)
        showDisplayMode();

    return QWidget::eventFilter(watched, event);
}

void AddressBarWidget::showDisplayMode()
{
    display_widget->setDraftText(address_bar->text(),
                               has_unsubmitted_text && show_draft_in_display_mode);
    address_stack->setCurrentWidget(display_widget_enabled
                                        ? static_cast<QWidget*>(display_widget)
                                        : static_cast<QWidget*>(address_bar));
}

void AddressBarWidget::showEditMode()
{
    if (address_bar->hasFocus() && address_stack->currentWidget() == address_bar)
        return;

    if (!has_unsubmitted_text)
        address_bar->setText(textForEditing(current_url));
    address_stack->setCurrentWidget(address_bar);
    address_bar->setFocus();
    if (!has_unsubmitted_text)
        address_bar->selectAll();
}

bool AddressBarWidget::isUrlInput(const QString& text) const
{
    QString trimmed_text = text.trimmed();

    QUrl url = QUrl::fromUserInput(trimmed_text);

    if (!url.isValid() || url.host().isEmpty())
    {
        return false;
    }

    if (!url.scheme().isEmpty() &&
        (trimmed_text.startsWith("http://") || trimmed_text.startsWith("https://")))
    {
        return true;
    }

    if (trimmed_text.contains(' '))
        return false;

    static const QRegularExpression ip_address_pattern("^\\d{1,3}(\\.\\d{1,3}){3}$");
    QString host = url.host();

    return host.contains('.') ||
           host == "localhost" ||
           ip_address_pattern.match(host).hasMatch();
}


void AddressBarWidget::onReturnPressed()
{
    QString text = address_bar->text();
    has_unsubmitted_text = false;

    if (!isUrlInput(text) && current_search_engine)
    {
        emit navigateRequested(QUrl(current_search_engine->createSearchURL(text)));
        showDisplayMode();
        return;
    }

    if (!text.startsWith("http://") &&
        !text.startsWith("https://"))
    {
        text = "https://" + text;
    }

    emit navigateRequested(QUrl(text));
    showDisplayMode();
}

void AddressBarWidget::setSearchEngine(ISearchEngine* search_engine)
{
    current_search_engine = search_engine;
}

void AddressBarWidget::setSearchBtnVisible(bool flag)
{
    if (flag)
        search_choice_menu->show();
    else
        search_choice_menu->hide();
}

void AddressBarWidget::setDisplayWidgetEnabled(bool enabled)
{
    if (display_widget_enabled == enabled)
        return;

    display_widget_enabled = enabled;
    showDisplayMode();
}

void AddressBarWidget::setPlaceholderText(const QString& text)
{
    address_bar->setPlaceholderText(text);
    display_widget->setPlaceholderText(text);
}

void AddressBarWidget::setShowDraftInDisplayMode(bool enabled)
{
    show_draft_in_display_mode = enabled;
    if (address_stack->currentWidget() == display_widget)
        showDisplayMode();
}

AddressBarDisplayWidget* AddressBarWidget::getAddressBarDisplay()
{
    return display_widget;
}










