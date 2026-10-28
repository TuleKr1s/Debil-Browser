

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QWidget>
#include <QWebEngineProfile>
#include <QWebEnginePage>

#include "core/browser_framework.h"


int main(int argc, char *argv[])
{
    // Using Rendering Hardware Infterface with program starts
    // instead with the first WebTab creating
    // to avoid the visual bug which recreating the window
    qputenv("QT_WIDGETS_RHI", "1");

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts); // hardware acceleration
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/assets/Debil_Browser.ico"));

    Browser_Framework browser;
    browser.show();



    return a.exec();
}
