#ifndef BROWSER_FRAMEWORK_H
#define BROWSER_FRAMEWORK_H

#include <QWidget>
#include <QBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QEvent>

#include "mergered_tabscontent.h"
#include "mainpannel.h"
#include "headerpanel.h"

#include "../custom_widgets/apptabs_factory.h"
#include "browserview.h"
// Core structure for whole browser
// It connects main widgets at display and logic levels

class Browser_Framework : public QWidget
{
    Q_OBJECT
public:
    explicit Browser_Framework(QWidget *parent = nullptr);

private:




    MainPannel* main_pannel;
    HeaderPanel* header_panel;


    QVBoxLayout* main_lay;

    // method connecting interactive buttons with page
    void connectNavigation();
    //
    void connectAddressBar();

    QMediaPlayer* m_player;
    QAudioOutput* m_audio_output;
    void musicInit();

protected:

    //void changeEvent(QEvent* event);

signals:

    //void windowStateChanged(bool maximized);

};

#endif // BROWSER_FRAMEWORK_H
