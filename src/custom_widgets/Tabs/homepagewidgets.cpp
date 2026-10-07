#include "homepagewidgets.h"

HomePageWidgets::HomePageWidgets(QWidget *parent)
    : QWidget{parent}
{

    m_clock_widget = new ClockWidget();

    m_addressbar_widget = new AddressBarWidget(this);
    m_addressbar_widget->setSearchBtnVisible(false);
    m_addressbar_widget->setDisplayWidgetEnabled(false);
    m_addressbar_widget->setPlaceholderText(tr("Enter a query or address"));

    homepage_widgets_lay = new QGridLayout;

    // Explicitly specify QGridLayout dimensions for rows and columns
    for (int row = 0; row < 3; ++row)
        homepage_widgets_lay->setRowStretch(row, 1);
    for (int col = 0; col < 3; ++col)
        homepage_widgets_lay->setColumnStretch(col, 1);

    homepage_widgets_lay->addWidget(m_clock_widget, 0,1, Qt::AlignCenter);
    homepage_widgets_lay->addWidget(m_addressbar_widget, 1,0,1,3, Qt::AlignCenter);


    this->setLayout(homepage_widgets_lay);
}
