#include "addressbarwidget.h"

#include "../web/searchengine_factory.h"

#include <QRegularExpression>
#include <QUrl>

AddressBarWidget::AddressBarWidget(QWidget *parent)
    : QWidget{parent}
{

    address_bar = new QLineEdit();
    search_choice_menu = new PopupMenuWidget(this);
    current_search_engine = nullptr;

    connect(address_bar, SIGNAL(returnPressed()),
            this, SLOT(onReturnPressed()));

    setupSearchEngines();

    addressBar_layout = new QHBoxLayout();
    addressBar_layout->addWidget(search_choice_menu);
    addressBar_layout->addWidget(address_bar);


    this->setLayout(addressBar_layout);

}

void AddressBarWidget::setupSearchEngines()
{
    search_engines.push_back(SearchEngine_Factory::createGoogle());
    search_engines.push_back(SearchEngine_Factory::createYandex());

    for (const auto& search_engine : search_engines)
    {
        QPushButton* btn = search_choice_menu->createBtn(search_engine->getSearchEngineName());
        ISearchEngine* engine = search_engine.get();

        connect(btn, &QPushButton::clicked, this, [this, engine]()
        {
            current_search_engine = engine;
            search_choice_menu->getMainBtn()->setText(engine->getSearchEngineName());
        });
    }

    if (!search_engines.empty())
    {
        current_search_engine = search_engines.front().get();
        search_choice_menu->getMainBtn()->setText(current_search_engine->getSearchEngineName());
    }
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
