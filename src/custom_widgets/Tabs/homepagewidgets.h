#ifndef HOMEPAGEWIDGETS_H
#define HOMEPAGEWIDGETS_H

#include <QWidget>
#include <QGridLayout>

#include "../clockwidget.h"
#include "../../ui/addressbarwidget.h"

class HomePageWidgets : public QWidget
{
    Q_OBJECT
public:
    explicit HomePageWidgets(QWidget *parent = nullptr);

    AddressBarWidget* m_addressbar_widget;


private:

    QGridLayout* homepage_widgets_lay;

    ClockWidget* m_clock_widget;

signals:
};

#endif // HOMEPAGEWIDGETS_H
