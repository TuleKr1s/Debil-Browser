#ifndef BROWSERVIEW_H
#define BROWSERVIEW_H

#include <QObject>
#include <QWidget>
#include <QWebEngineView>

// Class with viewing window logic

class BrowserView : public QWebEngineView
{
    Q_OBJECT
public:
    BrowserView(QWidget* parent = nullptr);

    void setURL(const QUrl& url);


public slots:
    // implementation of button functionality
    void goBack();
    void goForward();
    void reloadPage();
    // ============
};

#endif // HOMEPAGE_H
