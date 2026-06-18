#ifndef BROWSER_FRAMEWORK_H
#define BROWSER_FRAMEWORK_H

#include <QWidget>
#include <QBoxLayout>


#include "browserview.h"
#include "mainpannel.h"

// Core structure for whole browser
// It connects main widgets at display and logic levels

class Browser_Framework : public QWidget
{
    Q_OBJECT
public:
    explicit Browser_Framework(QWidget *parent = nullptr);

private:

    BrowserView* viewing_window;
    MainPannel* main_pannel;


    QVBoxLayout* main_lay;

    // method connecting interactive buttons with page
    void connectNavigation();

signals:
};

#endif // BROWSER_FRAMEWORK_H
