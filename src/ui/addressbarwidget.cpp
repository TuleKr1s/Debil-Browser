#include "addressbarwidget.h"

#include <QApplication>
#include <QGridLayout>
#include <QRegularExpression>
#include <QEvent>
#include <QResizeEvent>
#include <QUrl>

AddressBarWidget::AddressBarWidget(QWidget *parent)
    : QWidget{parent}
{
    address_bar = new QLineEdit(this);

    display_widget = new QWidget(this);
    url_label = new QLabel(display_widget);
    title_label = new QLabel(display_widget);

    url_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    title_label->setAlignment(Qt::AlignCenter);

    btn_menu = new QWidget(this);
    m_page_menu_btn = new PopupMenuWidget(btn_menu);
    m_page_menu_btn->setMainBtnIcon(QIcon(":/assets/MainPanel/page_menu_btn.png"));
    m_page_menu_btn->setToolTip("Page menu");
    m_page_menu_btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    browser_protection_btn = new QPushButton(btn_menu);
    browser_protection_btn->setIcon(QIcon(":/assets/Debil_Browser_icon.png"));
    browser_protection_btn->setToolTip("Debil protection");
    browser_protection_btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    btn_menu_lay = new QHBoxLayout(btn_menu);
    btn_menu_lay->addWidget(m_page_menu_btn);
    btn_menu_lay->addWidget(browser_protection_btn);
    btn_menu_lay->setContentsMargins(0,0,0,0);
    btn_menu_lay->setSpacing(0);

    auto* display_layout = new QGridLayout(display_widget);
    display_layout->setContentsMargins(8, 0, 8, 0);
    display_layout->setSpacing(6);
    display_layout->addWidget(url_label, 0, 0);
    display_layout->addWidget(title_label, 0, 1);
    display_layout->addWidget(btn_menu, 0, 2, Qt::AlignRight | Qt::AlignVCenter);
    display_layout->setColumnStretch(0, 1);
    display_layout->setColumnStretch(1, 1);
    display_layout->setColumnStretch(2, 0);

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

void AddressBarWidget::setUrl(const QUrl& url)
{
    current_url = url;
    address_bar->setText(url.toDisplayString(QUrl::FullyDecoded));
    updateDisplayText();
}

void AddressBarWidget::setTitle(const QString& title)
{
    current_title = title;
    updateDisplayText();
}

void AddressBarWidget::updateSearchEngineForUrl(const QUrl& url)
{
    search_choice_menu->selectSearchEngineForUrl(url);
}

bool AddressBarWidget::eventFilter(QObject* watched, QEvent* event)
{
    const bool display_clicked = watched == display_widget ||
                                 watched == url_label ||
                                 watched == title_label;

    if (display_clicked && event->type() == QEvent::MouseButtonPress)
    {
        showEditMode();
        return true;
    }

    if (event->type() == QEvent::MouseButtonPress && address_bar->hasFocus())
    {
        auto* clicked_widget = qobject_cast<QWidget*>(watched);
        const bool clicked_inside_editor = clicked_widget &&
            (clicked_widget == address_bar || address_bar->isAncestorOf(clicked_widget));

        if (!clicked_inside_editor)
            showDisplayMode();
    }

    if (watched == address_bar && event->type() == QEvent::FocusOut)
        showDisplayMode();

    return QWidget::eventFilter(watched, event);
}

void AddressBarWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateDisplayText();
}

void AddressBarWidget::showDisplayMode()
{
    address_stack->setCurrentWidget(display_widget);
    updateDisplayText();
}

void AddressBarWidget::showEditMode()
{
    address_bar->setText(current_url.toDisplayString(QUrl::FullyDecoded));
    address_stack->setCurrentWidget(address_bar);
    address_bar->setFocus();
    address_bar->selectAll();
}

void AddressBarWidget::updateDisplayText()
{
    QString domain = current_url.host().isEmpty()
                         ? current_url.toDisplayString(QUrl::FullyDecoded)
                         : current_url.host();

    if (domain.startsWith(QStringLiteral("www."), Qt::CaseInsensitive))
        domain.remove(0, 4);

    url_label->setText(url_label->fontMetrics().elidedText(
        domain, Qt::ElideRight, url_label->width()));
    title_label->setText(title_label->fontMetrics().elidedText(
        current_title, Qt::ElideRight, title_label->width()));

    url_label->setToolTip(current_url.toDisplayString(QUrl::FullyDecoded));
    title_label->setToolTip(current_title);
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











