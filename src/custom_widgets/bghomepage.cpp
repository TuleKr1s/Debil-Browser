#include "bghomepage.h"

BGHomePage::BGHomePage(QWidget *parent)
    : QWidget{parent}
{

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_videoSink = new QVideoSink(this);


    m_player->setAudioOutput(m_audioOutput);

    m_player->setVideoOutput(m_videoSink);

    m_audioOutput->setVolume(1.0f);

    //m_player->setLoops(QMediaPlayer::Infinite);
    connect(m_player, &QMediaPlayer::mediaStatusChanged,
           this, [this](QMediaPlayer::MediaStatus status)
           {
                if (status == QMediaPlayer::EndOfMedia) {
                    m_player->setPosition(0);
                    m_player->play();
                }
            });

    connect(m_videoSink, &QVideoSink::videoFrameChanged,
            this, [this](const QVideoFrame& frame) {
        if (!frame.isValid()) return;

        QImage image = frame.toImage();
        if (image.isNull()) return;

        m_currentFrame = std::move(image);

        update();
    });

}


void BGHomePage::setVideo(const QString& file_path)
{


    m_player->setSource(QUrl(file_path));

    m_player->play();
}

void BGHomePage::play()
{
    m_player->play();
}

void BGHomePage::pause()
{
    m_player->pause();
}

void BGHomePage::stop()
{
    m_player->stop();
}

void BGHomePage::setVolume(float volume)
{
    m_audioOutput->setVolume(volume);
}

void BGHomePage::setMuted(bool muted)
{
    m_audioOutput->setMuted(muted);
}

void BGHomePage::paintEvent(QPaintEvent* event)
{

    Q_UNUSED(event);

    QPainter painter(this);

    if (m_currentFrame.isNull())
    {
        painter.fillRect(rect(), Qt::black);
        return;
    }

    const QSize widgetSize = size();
    const QSize imageSize = m_currentFrame.size();

    QSize scaledSize = imageSize;

    scaledSize.scale(widgetSize, Qt::KeepAspectRatioByExpanding);

    const int x = (widgetSize.width() - scaledSize.width()) / 2;
    const int y = (widgetSize.height() - scaledSize.height()) / 2;

    QRect targetRect(x, y, scaledSize.width(), scaledSize.height());

    painter.drawImage(targetRect, m_currentFrame);
}



















