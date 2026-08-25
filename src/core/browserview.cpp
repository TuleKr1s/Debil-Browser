#include "BrowserView.h"

BrowserView::BrowserView(QWidget* parent)
{



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
