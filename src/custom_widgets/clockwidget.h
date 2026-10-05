#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QResizeEvent>



class ClockWidget : public QLabel
{
    Q_OBJECT
public:
    explicit ClockWidget(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:

    const int MIN_FONT_SIZE = 20, MAX_FONT_SIZE = 50;

    QTimer* m_timer;


private slots:
    void updateTime();

};

#endif // CLOCKWIDGET_H
