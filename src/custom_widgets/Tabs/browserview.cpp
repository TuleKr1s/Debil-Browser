#include "browserview.h"

BrowserView::BrowserView(QWidget* parent)
    : QWebEngineView(parent)
{

    connect(this, &QWebEngineView::loadStarted,
            this, [](){qDebug() << "Loading page started";});
    connect(this, &QWebEngineView::loadProgress,
            this, [](int progress) {qDebug() << "Loading: " << progress << "%";});
    connect(this, &QWebEngineView::loadFinished,
            this, [](bool result) {qDebug() << "Loading finished: " << result;});


}


void BrowserView::setURL(const QUrl& url)
{
    this->load(url);
}

QUrl BrowserView::getURL()
{
    return this->url();
}

// implementation of button functionality

void BrowserView::goBack() {
    this->back();
}

void BrowserView::goForward() {
    this->forward();
}

void BrowserView::reloadPage() {
    this->reload();
}




// ========================================
