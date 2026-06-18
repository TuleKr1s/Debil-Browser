#include "BrowserView.h"

BrowserView::BrowserView(QWidget* parent)
{

    setUrl(QUrl("https://www.google.com"));

}


void BrowserView::setURL(const QUrl& url)
{
    this->load(url);
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
