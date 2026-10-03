#include "bghomepage.h"

#include <QGuiApplication>

BGHomePage::BGHomePage(QWidget *parent)
    : QWidget{parent}
{
    m_player.setAudioOutput(&m_audioOutput);
    m_player.setVideoOutput(&m_videoSink);

    m_audioOutput.setVolume(1.0f);

    connect(&m_player, &QMediaPlayer::mediaStatusChanged,
            this, [this](QMediaPlayer::MediaStatus status)
            {
                if (status == QMediaPlayer::EndOfMedia) {
                    m_player.setPosition(0);

                    if (m_isActive)
                        m_player.play();
                }
            });

    connect(&m_videoSink, &QVideoSink::videoFrameChanged,
            this, [this](const QVideoFrame& frame) {
                if (!frame.isValid()) return;

                QImage image = frame.toImage();
                if (image.isNull()) return;

                m_currentFrame = std::move(image);

                update();
            });

    connect(qGuiApp, &QGuiApplication::applicationStateChanged,
            this, [this](Qt::ApplicationState) {
                updateMutedState();
            });

    updateMutedState();
}

void BGHomePage::loadImage(const QString& file_path)
{
    if (!m_currentFrame.load(file_path))
    {
        qDebug() << "Failed to load homepage wallpaper";
        return;
    }
    qDebug() << "Homepage wallpaper successful loaded";

    m_player.stop();
    m_player.setSource({});
    m_hasVideo = false;
    update();
}

void BGHomePage::loadVideo(const QString& file_path)
{
    m_hasVideo = !file_path.isEmpty();
    m_player.setSource(m_hasVideo ? QUrl(file_path) : QUrl{});

    if (m_hasVideo && m_isActive)
        m_player.play();
}

void BGHomePage::setActive(bool active)
{
    m_isActive = active;

    if (!m_hasVideo)
        return;

    if (m_isActive)
        m_player.play();
    else
        m_player.pause();
}

void BGHomePage::setVolume(float volume)
{
    m_audioOutput.setVolume(volume);
}

void BGHomePage::setMuted(bool muted)
{
    m_isMuted = muted;
    updateMutedState();
}

void BGHomePage::updateMutedState()
{
    const bool applicationIsInactive =
        QGuiApplication::applicationState() != Qt::ApplicationActive;

    m_audioOutput.setMuted(m_isMuted || applicationIsInactive);
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

    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawImage(targetRect, m_currentFrame);
}



















