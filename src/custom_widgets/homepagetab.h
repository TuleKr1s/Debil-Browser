#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QString>

#include "iapptabs.h"

class HomePageTab : public QWidget, public IAppTabs
{
    Q_OBJECT
public:
    explicit HomePageTab(QWidget *parent = nullptr);

    QString title() const override;
    void setTitle(const QString& title) override;

private:

    QGridLayout* homepage_lay;
    QString m_title {"Home"};

};

#endif // HOMEPAGE_H
