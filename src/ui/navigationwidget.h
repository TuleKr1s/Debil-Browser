#ifndef NAVIGATIONWIDGET_H
#define NAVIGATIONWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>

// Widget with basic buttons for interacting with page
// Widget is purely visual, without implementing any button logic

class NavigationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NavigationWidget(QWidget *parent = nullptr);

    QPushButton* back_btn;
    QPushButton* forward_btn;
    QPushButton* reload_btn;

    QHBoxLayout* main_lay;

signals:
};

#endif // NAVIGATIONWIDGET_H
