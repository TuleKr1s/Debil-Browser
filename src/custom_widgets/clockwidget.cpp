#include "clockwidget.h"

ClockWidget::ClockWidget(QWidget *parent)
    : QLabel{parent}, m_timer(new QTimer(this))
{

    this->setObjectName("ClockWidget");

    QFile styleFile(":/css/ClockWidget.css");
    if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cant open CSS:" << styleFile.errorString();
    } else {
        this->setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    setAlignment(Qt::AlignCenter);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(m_timer, &QTimer::timeout,
            this, &ClockWidget::updateTime);

    m_timer->start(1000);


    updateTime();
}


void ClockWidget::updateTime()
{
    const QDateTime current_time = QDateTime::currentDateTime();


    setText(current_time.toString("HH:mm") +
            "\n" +
            current_time.toString("dddd, d MMMM yyyy"));

    adjustSize();
    setFixedSize(sizeHint());
}
