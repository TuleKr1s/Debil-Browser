#ifndef MERGERED_TABSCONTENT_H
#define MERGERED_TABSCONTENT_H

#include <QObject>

#include "tabmanager.h"
#include "tabcontent.h"

class Mergered_TabsContent : public QObject
{
    Q_OBJECT
public:
    explicit Mergered_TabsContent(QObject *parent = nullptr);

    static TabContent* getTabContent();
    static TabManager* getTabManager();

private:

    static TabContent* m_content;
    static TabManager* m_tabManager;
};

#endif // MERGERED_TABSCONTENT_H
