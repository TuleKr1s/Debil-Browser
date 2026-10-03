#ifndef BGHOMEPAGE_H
#define BGHOMEPAGE_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>
#include <QVideoFrame>
#include <QImage>

#include <QPainter>
#include <QFileInfo>

class BGHomePage : public QWidget
{
    Q_OBJECT
public:
    explicit BGHomePage(QWidget *parent = nullptr);

    void setActive(bool active);

    void setVolume(float volume);
    void setMuted(bool muted);

    void loadVideo(const QString& file_path);
    void loadImage(const QString& file_path);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QMediaPlayer m_player;
    QAudioOutput m_audioOutput;
    QVideoSink m_videoSink;

    QImage m_currentFrame;
    bool m_isActive = false;
    bool m_hasVideo = false;
    bool m_isMuted = false;

    void updateMutedState();

};

#endif // BGHOMEPAGE_H
