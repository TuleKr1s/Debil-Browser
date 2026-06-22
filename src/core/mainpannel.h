#ifndef MAINPANNEL_H
#define MAINPANNEL_H

#include <QWidget>
#include <QBoxLayout>


#include "../ui/navigationwidget.h"
#include "../ui/addressbarwidget.h"

// Class that combines widgets related to main pannel

class MainPannel : public QWidget
{
    Q_OBJECT
public:
    explicit MainPannel(QWidget *parent = nullptr);


    NavigationWidget* nav_wgt;
    AddressBarWidget* address_bar;

private:



    QHBoxLayout* pannel_lay;

signals:
};

#endif // MAINPANNEL_H
