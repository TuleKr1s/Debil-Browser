#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QStackedLayout>
#include <QString>
#include <QEvent>
#include <QStackedWidget>

#include "iapptabs.h"
#include "bghomepage.h"

class HomePageTab : public QWidget, public IAppTabs
{
    Q_OBJECT
public:
    explicit HomePageTab(QWidget *parent = nullptr);

    QWidget* widget() override;
    QString title() const override;
    void setTitle(const QString& title) override;

private:

    QStackedLayout* homepage_lay;
    BGHomePage* m_backgroundMedia;

    QString m_title {"Home Page"};

    void connectToStack();
    bool m_connectedToStack = false;

    void installWindowFilter();
    QWidget* m_browserWindow = nullptr;

protected:
    bool event(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

};

#endif // HOMEPAGE_H
