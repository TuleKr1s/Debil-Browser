#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QWidget>
#include <QTabBar>
#include <QVector>
#include <QHBoxLayout>
#include <QToolButton>

#include "../custom_widgets/browsertab.h"
#include "browserview.h"
#include "../ui/tabcontent.h"

class TabManager : public QWidget
{
    Q_OBJECT
public:
    explicit TabManager(TabContent* tab_content, QWidget *parent = nullptr);

    void createTab(const QUrl& url);
    void closeTab(int index);

    BrowserTab* currentTab() const;

private:

    QTabBar* m_tabBar;
    QToolButton* m_newTabButton;
    TabContent* m_tabContent;

    QVector<BrowserTab*> m_tabs;

    QHBoxLayout* tabManager_lay;

private slots:

    void onCurrentTabChanged(int index);
    void onTabCloseRequested(int index);

};

#endif // TABMANAGER_H
