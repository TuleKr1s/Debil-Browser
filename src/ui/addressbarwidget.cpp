#include "addressbarwidget.h"
#include "addressbardisplaywidget.h"

#include <QApplication>
#include <QRegularExpression>
#include <QEvent>
#include <QUrl>

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
    connect(display_widget, &AddressBarDisplayWidget::editRequested,
            this, &AddressBarWidget::showEditMode);


    addressBar_layout = new QHBoxLayout();
    addressBar_layout->addWidget(search_choice_menu);
    addressBar_layout->addLayout(address_stack, 1);


    this->setLayout(addressBar_layout);

    this->setMinimumWidth(325);
    this->setMaximumWidth(1000);
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
    address_bar->setText(url.toDisplayString(QUrl::FullyDecoded));
    display_widget->setUrl(url);
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
        auto* clicked_widget = qobject_cast<QWidget*>(watched);
        const bool clicked_inside_editor = clicked_widget &&
            (clicked_widget == address_bar || address_bar->isAncestorOf(clicked_widget));

        if (!clicked_inside_editor)
            showDisplayMode();
    }

    if (display_widget_enabled && watched == address_bar && event->type() == QEvent::FocusOut)
        showDisplayMode();

    return QWidget::eventFilter(watched, event);
}

void AddressBarWidget::showDisplayMode()
{
    address_stack->setCurrentWidget(display_widget_enabled
                                        ? static_cast<QWidget*>(display_widget)
                                        : static_cast<QWidget*>(address_bar));
}

void AddressBarWidget::showEditMode()
{
    address_bar->setText(current_url.toDisplayString(QUrl::FullyDecoded));
    address_stack->setCurrentWidget(address_bar);
    address_bar->setFocus();
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











