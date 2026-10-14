#include <QApplication>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QStackedWidget>
#include <QTabBar>
#include <QTest>
#include <QTimer>
#include <QToolButton>
#include <QUrl>

#include "src/core/browser_framework.h"
#include "src/core/headerpanel.h"
#include "src/core/mainpannel.h"
#include "src/custom_widgets/clockwidget.h"
#include "src/custom_widgets/popupmenuwidget.h"
#include "src/custom_widgets/Tabs/apptabs_factory.h"
#include "src/custom_widgets/Tabs/bghomepage.h"
#include "src/custom_widgets/Tabs/browserview.h"
#include "src/custom_widgets/Tabs/homepagetab.h"
#include "src/custom_widgets/Tabs/homepagewidgets.h"
#include "src/custom_widgets/Tabs/mergered_tabscontent.h"
#include "src/custom_widgets/Tabs/tabcontent.h"
#include "src/custom_widgets/Tabs/tabmanager.h"
#include "src/custom_widgets/Tabs/webtab.h"
#include "src/ui/addressbarwidget.h"
#include "src/ui/addressbardisplaywidget.h"
#include "src/ui/navigationwidget.h"
#include "src/ui/windowbar.h"
#include "src/web/searchengine_factory.h"
#include "src/web/searchengineselector.h"

class BrowserAllComponentsTest : public QObject
{
    Q_OBJECT

private slots:
    void resourcesAreEmbedded();
    void googleSearchEngineEncodesQuery();
    void yandexSearchEngineEncodesQuery();
    void searchEngineFactoryCreatesExpectedEngines();
    void searchEnginesRecognizeOwnResultUrls();
    void searchEngineSelectorDefaultsToGoogle();
    void searchEngineSelectorChangesEngineAfterButtonClick();
    void searchEngineSelectorSupportsProgrammaticSelection();
    void popupMenuCreatesAndShowsActions();
    void addressBarNavigatesToHttpUrl();
    void addressBarAddsHttpsToHostInput();
    void addressBarSearchesWithSelectedEngine();
    void addressBarUpdatesTextAndEngineFromUrl();
    void addressBarCanDisableDisplayWidget();
    void addressBarDisplaysPlaceholderAndCenteredTitle();
    void addressBarDoesNotReselectTextWhenAlreadyFocused();
    void navigationWidgetProvidesAllControls();
    void mainPanelComposesNavigationAndAddressBar();
    void clockWidgetShowsTimeAndRunsTimer();
    void homePageWidgetsComposeClockAndAddressBar();
    void backgroundHomePageAcceptsMediaSettings();
    void browserViewAcceptsUrl();
    void webTabAndFactoriesExposeExpectedContracts();
    void homePageTabExposesExpectedContract();
    void tabContentAddsMovesShowsAndRemovesViews();
    void tabManagerCreatesAndSwitchesTabs();
    void windowBarControlsItsWindow();
    void mergedTabsAndHeaderPanelAreConnected();
    void browserFrameworkStartsAndNavigatesFromHomePage();
    void tabManagerClosesTabs();

private:
    static QLineEdit *addressInput(AddressBarWidget &addressBar);
};

void BrowserAllComponentsTest::resourcesAreEmbedded()
{
    const QStringList resources{
        QStringLiteral(":/assets/WindowBar/close_btn.png"),
        QStringLiteral(":/assets/WindowBar/full_window_btn.png"),
        QStringLiteral(":/assets/WindowBar/half_window_btn.png"),
        QStringLiteral(":/assets/NavigationBar/left_arrow.png"),
        QStringLiteral(":/assets/WindowBar/menu_btn.png"),
        QStringLiteral(":/assets/WindowBar/minimize_btn.png"),
        QStringLiteral(":/assets/NavigationBar/refresh.png"),
        QStringLiteral(":/assets/NavigationBar/right_arrow.png"),
        QStringLiteral(":/assets/multimedia/DebilBrowser_MainTheme.mp3"),
        QStringLiteral(":/assets/multimedia/DebilBrowser_MainTheme.mp4"),
        QStringLiteral(":/assets/Homepage_wallpapers/main_bg_homepage_dark.png"),
        QStringLiteral(":/assets/Homepage_wallpapers/main_bg_homepage_light.png"),
        QStringLiteral(":/css/ClockWidget.css")
    };

    for (const QString &resource : resources)
        QVERIFY2(QFile::exists(resource), qPrintable(resource));
}

void BrowserAllComponentsTest::googleSearchEngineEncodesQuery()
{
    const auto google = SearchEngine_Factory::createGoogle();

    QCOMPARE(google->getSearchEngineName(), QStringLiteral("google"));
    QCOMPARE(google->createSearchURL(QStringLiteral("C++ guide")),
             QStringLiteral("https://www.google.com/search?q=C%2B%2B%20guide"));
}

void BrowserAllComponentsTest::yandexSearchEngineEncodesQuery()
{
    const auto yandex = SearchEngine_Factory::createYandex();

    QCOMPARE(yandex->getSearchEngineName(), QStringLiteral("yandex"));
    QCOMPARE(yandex->createSearchURL(QStringLiteral("news & weather")),
             QStringLiteral("https://yandex.ru/search/?text=news%20%26%20weather"));
}

void BrowserAllComponentsTest::searchEngineFactoryCreatesExpectedEngines()
{
    auto engines = SearchEngine_Factory::createAll();

    QCOMPARE(engines.size(), std::size_t{2});
    QCOMPARE(engines.at(0)->getSearchEngineName(), QStringLiteral("google"));
    QCOMPARE(engines.at(1)->getSearchEngineName(), QStringLiteral("yandex"));
    QVERIFY(engines.at(0).get() != engines.at(1).get());
}

void BrowserAllComponentsTest::searchEnginesRecognizeOwnResultUrls()
{
    const auto google = SearchEngine_Factory::createGoogle();
    const auto yandex = SearchEngine_Factory::createYandex();

    QVERIFY(google->matchesSearchUrl(QUrl(QStringLiteral("https://www.google.com/search?q=qt"))));
    QVERIFY(!google->matchesSearchUrl(QUrl(QStringLiteral("https://www.google.com/maps?q=qt"))));
    QVERIFY(yandex->matchesSearchUrl(QUrl(QStringLiteral("https://yandex.ru/search/?text=qt"))));
    QVERIFY(!yandex->matchesSearchUrl(QUrl(QStringLiteral("https://example.com/search/?text=qt"))));
}

void BrowserAllComponentsTest::searchEngineSelectorDefaultsToGoogle()
{
    SearchEngineSelector selector;

    QCOMPARE(selector.getCurrentSearchEngine()->getSearchEngineName(), QStringLiteral("google"));
    const auto menu = selector.findChild<PopupMenuWidget *>();
    QVERIFY(menu != nullptr);
    QCOMPARE(menu->getMainBtn()->text(), QStringLiteral("google"));
    QCOMPARE(menu->btn_list.size(), std::size_t{2});
}

void BrowserAllComponentsTest::searchEngineSelectorChangesEngineAfterButtonClick()
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

void BrowserAllComponentsTest::searchEngineSelectorSupportsProgrammaticSelection()
{
    SearchEngineSelector selector;
    QSignalSpy engineChangedSpy(&selector, &SearchEngineSelector::engineChanged);

    QVERIFY(selector.selectSearchEngine(QStringLiteral("YANDEX")));
    QCOMPARE(selector.getCurrentSearchEngine()->getSearchEngineName(), QStringLiteral("yandex"));
    QCOMPARE(engineChangedSpy.count(), 1);
    QVERIFY(selector.selectSearchEngineForUrl(
        QUrl(QStringLiteral("https://www.google.com/search?q=widgets"))));
    QCOMPARE(selector.getCurrentSearchEngine()->getSearchEngineName(), QStringLiteral("google"));
    QCOMPARE(engineChangedSpy.count(), 2);
    QVERIFY(!selector.selectSearchEngine(QStringLiteral("unknown")));
}

void BrowserAllComponentsTest::popupMenuCreatesAndShowsActions()
{
    PopupMenuWidget menu;
    auto *first = menu.createBtn(QStringLiteral("first"));
    auto *second = menu.createBtn(QStringLiteral("second"));

    QCOMPARE(menu.btn_list.size(), std::size_t{2});
    QCOMPARE(first->text(), QStringLiteral("first"));
    QCOMPARE(second->text(), QStringLiteral("second"));
    menu.setMainBtnText(QStringLiteral("engines"));
    QCOMPARE(menu.getMainBtn()->text(), QStringLiteral("engines"));
    QVERIFY(menu.sizeHint().isValid());

    QTest::mouseClick(menu.getMainBtn(), Qt::LeftButton);
    QVERIFY(first->parentWidget()->isVisible());
    QTest::mouseClick(first, Qt::LeftButton);
    QVERIFY(!first->parentWidget()->isVisible());
}

void BrowserAllComponentsTest::addressBarNavigatesToHttpUrl()
{
    AddressBarWidget addressBar;
    QSignalSpy navigationSpy(&addressBar, &AddressBarWidget::navigateRequested);

    addressInput(addressBar)->setText(QStringLiteral("https://qt.io/docs"));
    addressBar.onReturnPressed();

    QCOMPARE(navigationSpy.count(), 1);
    QCOMPARE(qvariant_cast<QUrl>(navigationSpy.constFirst().constFirst()),
             QUrl(QStringLiteral("https://qt.io/docs")));
}

void BrowserAllComponentsTest::addressBarAddsHttpsToHostInput()
{
    AddressBarWidget addressBar;
    QSignalSpy navigationSpy(&addressBar, &AddressBarWidget::navigateRequested);

    addressInput(addressBar)->setText(QStringLiteral("example.com"));
    addressBar.onReturnPressed();

    QCOMPARE(navigationSpy.count(), 1);
    QCOMPARE(qvariant_cast<QUrl>(navigationSpy.constFirst().constFirst()),
             QUrl(QStringLiteral("https://example.com")));
}

void BrowserAllComponentsTest::addressBarSearchesWithSelectedEngine()
{
    AddressBarWidget addressBar;
    QSignalSpy navigationSpy(&addressBar, &AddressBarWidget::navigateRequested);
    const auto selector = addressBar.findChild<SearchEngineSelector *>();

    QVERIFY(selector != nullptr);
    QVERIFY(selector->selectSearchEngine(QStringLiteral("yandex")));
    addressInput(addressBar)->setText(QStringLiteral("C++ guide"));
    addressBar.onReturnPressed();

    QCOMPARE(navigationSpy.count(), 1);
    QCOMPARE(qvariant_cast<QUrl>(navigationSpy.constFirst().constFirst()),
             QUrl(QStringLiteral("https://yandex.ru/search/?text=C%2B%2B%20guide")));
}

void BrowserAllComponentsTest::addressBarUpdatesTextAndEngineFromUrl()
{
    AddressBarWidget addressBar;
    const QUrl searchUrl(QStringLiteral("https://yandex.ru/search/?text=browser"));

    addressBar.setUrl(searchUrl);
    QCOMPARE(addressInput(addressBar)->text(), searchUrl.toDisplayString(QUrl::FullyDecoded));
    addressBar.updateSearchEngineForUrl(searchUrl);

    const auto selector = addressBar.findChild<SearchEngineSelector *>();
    QVERIFY(selector != nullptr);
    QCOMPARE(selector->getCurrentSearchEngine()->getSearchEngineName(), QStringLiteral("yandex"));

    addressBar.setSearchBtnVisible(false);
    QVERIFY(selector->isHidden());
    addressBar.setSearchBtnVisible(true);
    QVERIFY(!selector->isHidden());
}

void BrowserAllComponentsTest::addressBarCanDisableDisplayWidget()
{
    AddressBarWidget addressBar;
    addressBar.show();
    QVERIFY(addressBar.isVisible());

    auto* input = addressInput(addressBar);
    auto* display = addressBar.findChild<AddressBarDisplayWidget*>();
    QVERIFY(display != nullptr);
    QVERIFY(!display->isHidden());

    addressBar.setDisplayWidgetEnabled(false);
    QVERIFY(!input->isHidden());
    QVERIFY(display->isHidden());

    input->setText(QStringLiteral("example.com"));
    addressBar.onReturnPressed();
    QVERIFY(!input->isHidden());

    addressBar.setDisplayWidgetEnabled(true);
    QVERIFY(!display->isHidden());
    QVERIFY(input->isHidden());
}

void BrowserAllComponentsTest::addressBarDisplaysPlaceholderAndCenteredTitle()
{
    AddressBarWidget addressBar;
    addressBar.resize(800, 32);
    addressBar.setPlaceholderText(QStringLiteral("Enter a query or address"));
    addressBar.setUrl(QUrl());
    addressBar.show();
    QCoreApplication::processEvents();

    auto* display = addressBar.findChild<AddressBarDisplayWidget*>();
    auto* urlLabel = addressBar.findChild<QLabel*>(QStringLiteral("addressBarUrlLabel"));
    auto* titleLabel = addressBar.findChild<QLabel*>(QStringLiteral("addressBarTitleLabel"));
    QVERIFY(display != nullptr);
    QVERIFY(urlLabel != nullptr);
    QVERIFY(titleLabel != nullptr);
    QCOMPARE(urlLabel->text(), QStringLiteral("Enter a query or address"));

    addressBar.setTitle(QStringLiteral("Centered page title"));
    QCoreApplication::processEvents();
    QCOMPARE(titleLabel->text(), QStringLiteral("Centered page title"));
    QVERIFY(qAbs(titleLabel->geometry().center().x() - display->rect().center().x()) <= 1);
}

void BrowserAllComponentsTest::addressBarDoesNotReselectTextWhenAlreadyFocused()
{
    AddressBarWidget addressBar;
    addressBar.resize(800, 32);
    addressBar.setUrl(QUrl(QStringLiteral("https://example.com/some/path")));
    addressBar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&addressBar));

    auto* display = addressBar.findChild<AddressBarDisplayWidget*>();
    auto* input = addressInput(addressBar);
    QVERIFY(display != nullptr);

    QTest::mouseClick(display, Qt::LeftButton, Qt::NoModifier, QPoint(20, 16));
    QCOMPARE(input->selectedText(), input->text());

    // A mouse press can propagate to the parent while still being inside the editor.
    const QPoint editorPoint(20, 16);
    QTest::mouseClick(&addressBar, Qt::LeftButton, Qt::NoModifier,
                      input->mapTo(&addressBar, editorPoint));
    QVERIFY(input->isVisible());
    QVERIFY(input->hasFocus());

    QTest::mouseClick(input, Qt::LeftButton, Qt::NoModifier, QPoint(20, 16));
    QVERIFY(input->selectedText().isEmpty());
    QVERIFY(input->cursorPosition() < input->text().size());
}

void BrowserAllComponentsTest::navigationWidgetProvidesAllControls()
{
    NavigationWidget navigation;

    QVERIFY(navigation.back_btn != nullptr);
    QVERIFY(navigation.forward_btn != nullptr);
    QVERIFY(navigation.reload_btn != nullptr);
    QCOMPARE(navigation.findChildren<QPushButton *>().size(), 3);
    QCOMPARE(navigation.back_btn->size(), QSize(25, 25));
    QCOMPARE(navigation.forward_btn->size(), QSize(25, 25));
    QCOMPARE(navigation.reload_btn->size(), QSize(25, 25));
    QVERIFY(!navigation.back_btn->icon().isNull());
    QVERIFY(!navigation.forward_btn->icon().isNull());
    QVERIFY(!navigation.reload_btn->icon().isNull());
}

void BrowserAllComponentsTest::mainPanelComposesNavigationAndAddressBar()
{
    MainPannel panel;

    QVERIFY(panel.nav_wgt != nullptr);
    QVERIFY(panel.address_bar != nullptr);
    QCOMPARE(panel.minimumHeight(), 55);
    QCOMPARE(panel.maximumHeight(), 55);
    QVERIFY(panel.layout() != nullptr);
}

void BrowserAllComponentsTest::clockWidgetShowsTimeAndRunsTimer()
{
    ClockWidget clock;
    const QRegularExpression format(QStringLiteral("^\\d{2}:\\d{2}\\n.+$"));

    QVERIFY(format.match(clock.text()).hasMatch());
    QCOMPARE(clock.objectName(), QStringLiteral("ClockWidget"));
    const auto timer = clock.findChild<QTimer *>();
    QVERIFY(timer != nullptr);
    QVERIFY(timer->isActive());
    QCOMPARE(timer->interval(), 1000);
    QVERIFY(!clock.styleSheet().isEmpty());
}

void BrowserAllComponentsTest::homePageWidgetsComposeClockAndAddressBar()
{
    HomePageWidgets widgets;
    widgets.show();

    QVERIFY(widgets.m_addressbar_widget != nullptr);
    QVERIFY(widgets.findChild<ClockWidget *>() != nullptr);
    QVERIFY(qobject_cast<QGridLayout *>(widgets.layout()) != nullptr);
    const auto selector = widgets.m_addressbar_widget->findChild<SearchEngineSelector *>();
    QVERIFY(selector != nullptr);
    QVERIFY(selector->isHidden());

    auto* input = addressInput(*widgets.m_addressbar_widget);
    QCOMPARE(input->placeholderText(), QStringLiteral("Enter a query or address"));
    input->setFocus();
    QCoreApplication::processEvents();
    QCOMPARE(input->placeholderText(), QStringLiteral("Enter a query or address"));
}

void BrowserAllComponentsTest::backgroundHomePageAcceptsMediaSettings()
{
    BGHomePage background;
    background.resize(320, 180);

    background.loadImage(QFINDTESTDATA("assets/Homepage_wallpapers/main_bg_homepage_dark.png"));
    background.setVolume(0.25f);
    background.setMuted(true);
    background.setActive(true);
    background.loadVideo(QStringLiteral("qrc:/assets/multimedia/DebilBrowser_MainTheme.mp4"));
    background.setActive(false);
    background.loadVideo(QString());

    const QPixmap rendered = background.grab();
    QVERIFY(!rendered.isNull());
    QCOMPARE(rendered.size(), QSize(320, 180));
}

void BrowserAllComponentsTest::browserViewAcceptsUrl()
{
    BrowserView view;
    const QUrl url(QStringLiteral("about:blank"));

    view.setURL(url);
    QTRY_COMPARE_WITH_TIMEOUT(view.getURL(), url, 20000);
}

void BrowserAllComponentsTest::webTabAndFactoriesExposeExpectedContracts()
{
    BrowserView view;
    WebTab tab(&view, QStringLiteral("Docs"));

    QCOMPARE(tab.view(), &view);
    QCOMPARE(tab.widget(), nullptr);
    QCOMPARE(tab.title(), QStringLiteral("Docs"));
    QCOMPARE(tab.type(), TabType::Web);
    tab.setTitle(QStringLiteral("Updated"));
    QCOMPARE(tab.title(), QStringLiteral("Updated"));

    const auto web = AppTabs_Factory::createWebTab(&view, QStringLiteral("Factory"));
    QCOMPARE(web->view(), &view);
    QCOMPARE(web->title(), QStringLiteral("Factory"));
}

void BrowserAllComponentsTest::homePageTabExposesExpectedContract()
{
    HomePageTab home;

    QCOMPARE(home.widget(), &home);
    QCOMPARE(home.title(), QStringLiteral("Home Page"));
    QCOMPARE(home.type(), TabType::HomePage);
    QVERIFY(home.view() == nullptr);
    QVERIFY(home.addressBar() != nullptr);
    home.setTitle(QStringLiteral("Start"));
    QCOMPARE(home.title(), QStringLiteral("Start"));
}

void BrowserAllComponentsTest::tabContentAddsMovesShowsAndRemovesViews()
{
    TabContent content;
    QPointer<QWidget> first = new QWidget;
    QPointer<QWidget> second = new QWidget;
    content.addView(first);
    content.addView(second);

    const auto stack = content.findChild<QStackedWidget *>();
    QVERIFY(stack != nullptr);
    QCOMPARE(stack->count(), 2);
    content.showView(second);
    QCOMPARE(stack->currentWidget(), second.data());
    content.moveView(1, 0);
    QCOMPARE(stack->widget(0), second.data());
    QCOMPARE(stack->currentWidget(), second.data());
    content.removeView(first);
    QVERIFY(first.isNull());
    QCOMPARE(stack->count(), 1);
}

void BrowserAllComponentsTest::tabManagerCreatesAndSwitchesTabs()
{
    TabContent content;
    TabManager manager(&content);
    const auto tabBar = manager.findChild<QTabBar *>();
    const auto newTabButton = manager.findChild<QToolButton *>();

    QVERIFY(tabBar != nullptr);
    QVERIFY(newTabButton != nullptr);
    QCOMPARE(tabBar->count(), 1);
    QCOMPARE(manager.currentTab()->type(), TabType::HomePage);
    QVERIFY(newTabButton->isHidden());

    QSignalSpy tabChangedSpy(&manager, &TabManager::currentTabChanged);
    QSignalSpy titleChangedSpy(&manager, &TabManager::currentTitleChanged);
    manager.createTab(QUrl(QStringLiteral("about:blank")));
    QCOMPARE(tabBar->count(), 2);
    QCOMPARE(manager.currentTab()->type(), TabType::Web);
    QVERIFY(!newTabButton->isHidden());
    QVERIFY(tabChangedSpy.count() >= 1);

    QTest::mouseClick(newTabButton, Qt::LeftButton);
    QCOMPARE(manager.currentTab()->type(), TabType::HomePage);
    QVERIFY(!titleChangedSpy.isEmpty());
    QCOMPARE(titleChangedSpy.constLast().constFirst().toString(), QString());
    QTest::mouseClick(newTabButton, Qt::LeftButton);
    QCOMPARE(manager.currentTab()->type(), TabType::Web);

}

void BrowserAllComponentsTest::tabManagerClosesTabs()
{
    TabContent content;
    TabManager manager(&content);
    const auto tabBar = manager.findChild<QTabBar *>();

    QVERIFY(tabBar != nullptr);
    QSignalSpy currentUrlSpy(&manager, &TabManager::currentUrlChanged);
    manager.createTab(QUrl(QStringLiteral("about:blank")));
    QCOMPARE(tabBar->count(), 2);
    currentUrlSpy.clear();
    manager.closeTab(tabBar->currentIndex());
    QCOMPARE(tabBar->count(), 1);
    QCOMPARE(tabBar->currentIndex(), 0);
    QVERIFY(manager.currentTab() != nullptr);
    QCOMPARE(manager.currentTab()->type(), TabType::HomePage);
    QCOMPARE(currentUrlSpy.count(), 1);
    QCOMPARE(qvariant_cast<QUrl>(currentUrlSpy.constFirst().constFirst()), QUrl());
    manager.closeTab(-1);
    manager.closeTab(99);
    QCOMPARE(tabBar->count(), 1);
}

void BrowserAllComponentsTest::windowBarControlsItsWindow()
{
    QWidget host;
    auto *windowBar = new WindowBar(&host);
    host.showNormal();
    QCoreApplication::processEvents();

    const auto buttons = windowBar->findChildren<QPushButton *>();
    QCOMPARE(buttons.size(), 4);
    for (const auto button : buttons)
        QVERIFY(!button->icon().isNull());
    QVERIFY(QMetaObject::invokeMethod(windowBar, "onFullWindowBtnClicked"));
    QCoreApplication::processEvents();
    QVERIFY(host.isMaximized());
    QVERIFY(QMetaObject::invokeMethod(windowBar, "onFullWindowBtnClicked"));
    QCoreApplication::processEvents();
    QVERIFY(!host.isMaximized());
    QVERIFY(QMetaObject::invokeMethod(windowBar, "onMinimizeBtnClicked"));
    QCoreApplication::processEvents();
    QVERIFY(host.isMinimized());
    host.showNormal();
    QVERIFY(QMetaObject::invokeMethod(windowBar, "onCloseBtnClicked"));
    QCoreApplication::processEvents();
    QVERIFY(!host.isVisible());
}

void BrowserAllComponentsTest::mergedTabsAndHeaderPanelAreConnected()
{
    Mergered_TabsContent merged;
    QVERIFY(Mergered_TabsContent::getTabContent() != nullptr);
    QVERIFY(Mergered_TabsContent::getTabManager() != nullptr);

    HeaderPanel header;
    QCOMPARE(header.minimumHeight(), 35);
    QCOMPARE(header.maximumHeight(), 35);
    QVERIFY(header.layout() != nullptr);
    QCOMPARE(Mergered_TabsContent::getTabManager()->parentWidget(), &header);
}

void BrowserAllComponentsTest::browserFrameworkStartsAndNavigatesFromHomePage()
{
    Browser_Framework browser;
    QCoreApplication::processEvents();

    QVERIFY(browser.isVisible());
    QVERIFY(browser.isMaximized());
    const auto mainPanel = browser.findChild<MainPannel *>();
    const auto headerPanel = browser.findChild<HeaderPanel *>();
    const auto manager = browser.findChild<TabManager *>();
    const auto content = browser.findChild<TabContent *>();
    QVERIFY(mainPanel != nullptr);
    QVERIFY(headerPanel != nullptr);
    QVERIFY(manager != nullptr);
    QVERIFY(content != nullptr);
    QCOMPARE(manager->currentTab()->type(), TabType::HomePage);

    const auto tabBar = manager->findChild<QTabBar *>();
    QVERIFY(tabBar != nullptr);
    emit mainPanel->address_bar->navigateRequested(QUrl(QStringLiteral("about:blank")));
    QTRY_COMPARE_WITH_TIMEOUT(tabBar->count(), 2, 5000);
    QCOMPARE(manager->currentTab()->type(), TabType::Web);

    manager->closeTab(tabBar->currentIndex());
    QTRY_COMPARE(manager->currentTab()->type(), TabType::HomePage);
    QTRY_VERIFY(addressInput(*mainPanel->address_bar)->text().isEmpty());

    browser.close();
}

QLineEdit *BrowserAllComponentsTest::addressInput(AddressBarWidget &addressBar)
{
    const auto input = addressBar.findChild<QLineEdit *>();
    Q_ASSERT(input != nullptr);
    return input;
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    BrowserAllComponentsTest tests;
    return QTest::qExec(&tests, argc, argv);
}

#include "browser_all_components_test.moc"
