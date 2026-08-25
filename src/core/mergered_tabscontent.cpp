#include "mergered_tabscontent.h"

TabContent* Mergered_TabsContent::m_content = nullptr;
TabManager* Mergered_TabsContent::m_tabManager = nullptr;

Mergered_TabsContent::Mergered_TabsContent(QObject *parent)
    : QObject{parent}
{

    m_content = new TabContent();

    m_tabManager = new TabManager(m_content);



}

TabContent* Mergered_TabsContent::getTabContent()
{
    return m_content;
}

TabManager* Mergered_TabsContent::getTabManager()
{
    return m_tabManager;
}

