

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QWidget>
#include <QWebEngineProfile>
#include <QWebEnginePage>

#include "core/browser_framework.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts); // hardware acceleration
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/assets/Debil_Browser.ico"));

    Browser_Framework browser;
    browser.show();



    return a.exec();
}
