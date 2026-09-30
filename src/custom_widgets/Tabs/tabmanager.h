#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QWidget>
#include <QTabBar>
#include <QHBoxLayout>
#include <QPointer>
#include <QToolButton>

#include <memory>
#include <vector>
#include <algorithm>

#include "apptabs_factory.h"
#include "browserview.h"
#include "tabcontent.h"
#include "../../ui/windowbar.h"



class TabManager : public QWidget
{
    Q_OBJECT
public:
    explicit TabManager(TabContent* tab_content, QWidget *parent = nullptr);

    void createTab(const QUrl& url);
    void createTab(const TabType tab_type);

    void closeTab(int index);

    IAppTabs* currentTab() const;

private:

    BrowserView* createWebTab();
    // updating button appearance based on presence of tabs or tab type
    void updateNewTabBtn();

    QTabBar* m_tabBar;
    QToolButton* m_newTabButton;
    TabContent* m_tabContent;
    QPointer<BrowserView> m_lastWebView;

    WindowBar* m_windowBar;

    std::vector<std::unique_ptr<IAppTabs>> m_tabs;

    QHBoxLayout* tabManager_lay;

    void updateTabBarWidth();

private slots:

    void onCurrentTabChanged(int index);
    void onTabCloseRequested(int index);
    void onTabMoved(int from, int to);
    void onNewTabClicked();

signals:

    void currentTabChanged(int index);

};

#endif // TABMANAGER_H
