#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QUrl>

#include "../custom_widgets/popupmenuwidget.h"
#include "../ui/addressbarwidget.h"
#include "../web/searchengine_factory.h"
#include "../web/searchengineselector.h"

class BrowserComponentsTest : public QObject
{
    Q_OBJECT

private slots:
    void googleSearchEngineEncodesQuery();
    void yandexSearchEngineEncodesQuery();
    void searchEngineFactoryCreatesExpectedEngines();
    void searchEngineSelectorDefaultsToGoogle();
    void searchEngineSelectorChangesEngineAfterButtonClick();
    void addressBarNavigatesToHttpUrl();
    void addressBarAddsHttpsToHostInput();
    void addressBarSearchesWithSelectedEngine();

private:
    static QLineEdit *addressInput(AddressBarWidget &addressBar);
};

void BrowserComponentsTest::googleSearchEngineEncodesQuery()
{
    const auto google = SearchEngine_Factory::createGoogle();

    QCOMPARE(google->getSearchEngineName(), QStringLiteral("google"));
    QCOMPARE(google->createSearchURL(QStringLiteral("C++ guide")),
             QStringLiteral("https://www.google.com/search?q=C%2B%2B%20guide"));
}

void BrowserComponentsTest::yandexSearchEngineEncodesQuery()
{
    const auto yandex = SearchEngine_Factory::createYandex();

    QCOMPARE(yandex->getSearchEngineName(), QStringLiteral("yandex"));
    QCOMPARE(yandex->createSearchURL(QStringLiteral("news & weather")),
             QStringLiteral("https://yandex.ru/search/?text=news%20%26%20weather"));
}

void BrowserComponentsTest::searchEngineFactoryCreatesExpectedEngines()
{
    const auto google = SearchEngine_Factory::createGoogle();
    const auto yandex = SearchEngine_Factory::createYandex();

    QVERIFY(google != nullptr);
    QVERIFY(yandex != nullptr);
    QVERIFY(google.get() != yandex.get());
}

void BrowserComponentsTest::searchEngineSelectorDefaultsToGoogle()
{
    SearchEngineSelector selector;

    QCOMPARE(selector.getCurrentSearchEngine()->getSearchEngineName(), QStringLiteral("google"));

    const auto menu = selector.findChild<PopupMenuWidget *>();
    QVERIFY(menu != nullptr);
    QCOMPARE(menu->getMainBtn()->text(), QStringLiteral("google"));
    QCOMPARE(menu->btn_list.size(), std::size_t{2});
}

void BrowserComponentsTest::searchEngineSelectorChangesEngineAfterButtonClick()
{
    SearchEngineSelector selector;
    QSignalSpy engineChangedSpy(&selector, &SearchEngineSelector::engineChanged);

    const auto menu = selector.findChild<PopupMenuWidget *>();
    QVERIFY(menu != nullptr);
    QVERIFY(menu->btn_list.size() >= 2);

    QTest::mouseClick(menu->btn_list.at(1), Qt::LeftButton);

    QCOMPARE(engineChangedSpy.count(), 1);
    QCOMPARE(selector.getCurrentSearchEngine()->getSearchEngineName(), QStringLiteral("yandex"));
    QCOMPARE(menu->getMainBtn()->text(), QStringLiteral("yandex"));
}

void BrowserComponentsTest::addressBarNavigatesToHttpUrl()
{
    AddressBarWidget addressBar;
    QSignalSpy navigationSpy(&addressBar, &AddressBarWidget::navigateRequested);

    addressInput(addressBar)->setText(QStringLiteral("https://qt.io/docs"));
    addressBar.onReturnPressed();

    QCOMPARE(navigationSpy.count(), 1);
    QCOMPARE(qvariant_cast<QUrl>(navigationSpy.constFirst().constFirst()),
             QUrl(QStringLiteral("https://qt.io/docs")));
}

void BrowserComponentsTest::addressBarAddsHttpsToHostInput()
{
    AddressBarWidget addressBar;
    QSignalSpy navigationSpy(&addressBar, &AddressBarWidget::navigateRequested);

    addressInput(addressBar)->setText(QStringLiteral("example.com"));
    addressBar.onReturnPressed();

    QCOMPARE(navigationSpy.count(), 1);
    QCOMPARE(qvariant_cast<QUrl>(navigationSpy.constFirst().constFirst()),
             QUrl(QStringLiteral("https://example.com")));
}

void BrowserComponentsTest::addressBarSearchesWithSelectedEngine()
{
    AddressBarWidget addressBar;
    QSignalSpy navigationSpy(&addressBar, &AddressBarWidget::navigateRequested);

    const auto selector = addressBar.findChild<SearchEngineSelector *>();
    QVERIFY(selector != nullptr);

    const auto menu = selector->findChild<PopupMenuWidget *>();
    QVERIFY(menu != nullptr);
    QTest::mouseClick(menu->btn_list.at(1), Qt::LeftButton);

    addressInput(addressBar)->setText(QStringLiteral("C++ guide"));
    addressBar.onReturnPressed();

    QCOMPARE(navigationSpy.count(), 1);
    QCOMPARE(qvariant_cast<QUrl>(navigationSpy.constFirst().constFirst()),
             QUrl(QStringLiteral("https://yandex.ru/search/?text=C%2B%2B%20guide")));
}

QLineEdit *BrowserComponentsTest::addressInput(AddressBarWidget &addressBar)
{
    const auto input = addressBar.findChild<QLineEdit *>();
    Q_ASSERT(input != nullptr);
    return input;
}

QTEST_MAIN(BrowserComponentsTest)

#include "browser_components_test.moc"
