#include "addressbarwidget.h"

//#include "../web/searchengine_factory.h"

#include <QRegularExpression>
#include <QEvent>
#include <QTimer>
#include <QUrl>

AddressBarWidget::AddressBarWidget(QWidget *parent)
    : QWidget{parent}
{

    address_bar = new QLineEdit();


    current_search_engine = nullptr;
    search_choice_menu = new SearchEngineSelector(this);
    connect(search_choice_menu, &SearchEngineSelector::engineChanged,
            this, &AddressBarWidget::setSearchEngine);
    setSearchEngine(search_choice_menu->getCurrentSearchEngine());

    connect(address_bar, SIGNAL(returnPressed()),
            this, SLOT(onReturnPressed()));
    address_bar->installEventFilter(this);


    addressBar_layout = new QHBoxLayout();
    addressBar_layout->addWidget(search_choice_menu);
    addressBar_layout->addWidget(address_bar);



    this->setMinimumWidth(1000); // !!!!!!!!! replace with a more flexible version
    this->setLayout(addressBar_layout);

}

void AddressBarWidget::setUrl(const QUrl& url)
{
    address_bar->setText(url.toDisplayString(QUrl::FullyDecoded));
}

void AddressBarWidget::updateSearchEngineForUrl(const QUrl& url)
{
    search_choice_menu->selectSearchEngineForUrl(url);
}

bool AddressBarWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == address_bar && event->type() == QEvent::MouseButtonPress)
        QTimer::singleShot(0, address_bar, &QLineEdit::selectAll);

    return QWidget::eventFilter(watched, event);
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
        return;
    }

    if (!text.startsWith("http://") &&
        !text.startsWith("https://"))
    {
        text = "https://" + text;
    }

    emit navigateRequested(QUrl(text));

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












