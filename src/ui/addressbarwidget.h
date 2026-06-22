#ifndef ADDRESSBARWIDGET_H
#define ADDRESSBARWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>

#include "../custom_widgets/popupmenuwidget.h"

class AddressBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AddressBarWidget(QWidget *parent = nullptr);

private:

    QLineEdit* address_bar;

    QHBoxLayout* addressBar_layout;

public slots:

    void onReturnPressed();

signals:

    void navigateRequested(const QUrl& url);
};

#endif // ADDRESSBARWIDGET_H
