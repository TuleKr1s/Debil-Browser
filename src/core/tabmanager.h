#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QWidget>
#include <QTabBar>
#include <QHBoxLayout>
#include <QToolButton>

#include <memory>
#include <vector>
#include <algorithm>

#include "../custom_widgets/apptabs_factory.h"
#include "browserview.h"
#include "../ui/tabcontent.h"
#include "../ui/windowbar.h"

enum TabType {
    WebTab,
    HomePageTab
};

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

    QTabBar* m_tabBar;
    QToolButton* m_newTabButton;
    TabContent* m_tabContent;

    WindowBar* m_windowBar;

    std::vector<std::unique_ptr<IAppTabs>> m_tabs;

    QHBoxLayout* tabManager_lay;

private slots:

    void onCurrentTabChanged(int index);
    void onTabCloseRequested(int index);
    void onTabMoved(int from, int to);

};

#endif // TABMANAGER_H
