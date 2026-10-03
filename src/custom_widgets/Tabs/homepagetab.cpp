#include "homepagetab.h"

HomePageTab::HomePageTab(QWidget *parent)
    : QWidget{parent}
{

    m_backgroundMedia = new BGHomePage(this);
    //m_backgroundMedia->loadVideo("qrc:/assets/multimedia/DebilBrowser_MainTheme.mp4");
    m_backgroundMedia->loadImage(":/assets/Homepage_wallpapers/main_bg_homepage_dark.png");


    m_homepage_widgets = new HomePageWidgets(this);

    homepage_lay = new QStackedLayout(this);
    homepage_lay->setStackingMode(QStackedLayout::StackAll);

    homepage_lay->addWidget(m_backgroundMedia);
    homepage_lay->addWidget(m_homepage_widgets);
    homepage_lay->setCurrentWidget(m_homepage_widgets);


    this->setLayout(homepage_lay);
}

QWidget* HomePageTab::widget()
{
    return this;
}

QString HomePageTab::title() const
{
    return m_title;
}

void HomePageTab::setTitle(const QString& title)
{
    m_title = title;
}

bool HomePageTab::event(QEvent* event)
{
    if (event->type() == QEvent::ParentChange)
        connectToStack();

    return QWidget::event(event);
}

bool HomePageTab::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_browserWindow &&
        event->type() == QEvent::WindowStateChange)
    {
        if (m_browserWindow->isMinimized())
            m_backgroundMedia->setActive(false);
        else if (isVisible())
            m_backgroundMedia->setActive(true);
    }

    return QWidget::eventFilter(watched, event);
}

void HomePageTab::connectToStack()
{
    if (m_connectedToStack)
        return;

    auto* stack = qobject_cast<QStackedWidget*>(parentWidget());
    if (!stack)
        return;

    m_connectedToStack = true;

    connect(stack, &QStackedWidget::currentChanged,
            this, [this, stack](int index)
            {
                const bool active = (stack->widget(index) == this);
                m_backgroundMedia->setActive(active);
            });

    m_backgroundMedia->setActive(stack->currentWidget() == this);

}

void HomePageTab::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    installWindowFilter();
}

void HomePageTab::installWindowFilter()
{
    QWidget* newWindow = window();

    if (m_browserWindow == newWindow)
        return;

    if (m_browserWindow)
        m_browserWindow->removeEventFilter(this);

    m_browserWindow = newWindow;
    m_browserWindow->installEventFilter(this);
}

AddressBarWidget* HomePageTab::addressBar() const
{
    return m_homepage_widgets->m_addressbar_widget;
}

TabType HomePageTab::type() const
{
    return m_tab_type;
}
