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

    void setVideo(const QString& file_path);

    void play();
    void pause();
    void stop();

    void setVolume(float volume);
    void setMuted(bool muted);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    QVideoSink* m_videoSink;

    QImage m_currentFrame;


};

#endif // BGHOMEPAGE_H
