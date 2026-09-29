#include "addressbarwidget.h"

//#include "../web/searchengine_factory.h"

#include <QRegularExpression>
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


    addressBar_layout = new QHBoxLayout();
    addressBar_layout->addWidget(search_choice_menu);
    addressBar_layout->addWidget(address_bar);



    this->setMinimumWidth(1000); // !!!!!!!!! replace with a more flexible version
    this->setLayout(addressBar_layout);

}


bool AddressBarWidget::isUrlInput(const QString& text) const
{
    QString trimmed_text = text.trimmed();

    if (trimmed_text.contains(' '))
    {
        return false;
    }

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












