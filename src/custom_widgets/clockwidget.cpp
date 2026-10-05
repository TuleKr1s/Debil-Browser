#include "clockwidget.h"

#include <algorithm>

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

    setMinimumSize(QSize(300,120));
    setMaximumSize(QSize(800, 300));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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


}

void ClockWidget::resizeEvent(QResizeEvent* event)
{
    QLabel::resizeEvent(event);

    const int calculated_size = std::min(event->size().width() / 10,
                                         event->size().height() / 3);

    const int font_size = qBound(MIN_FONT_SIZE, calculated_size, MAX_FONT_SIZE);

    QFont current_font = font();
    current_font.setPixelSize(font_size);
    current_font.setBold(true);
    this->setFont(current_font);


}










