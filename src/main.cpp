

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QWidget>

#include "core/browser_framework.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Browser_Framework browser;
    browser.show();



    return a.exec();
}
