#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QFile>


class ClockWidget : public QLabel
{
    Q_OBJECT
public:
    explicit ClockWidget(QWidget *parent = nullptr);


private:


    QTimer* m_timer;


private slots:
    void updateTime();

};

#endif // CLOCKWIDGET_H
