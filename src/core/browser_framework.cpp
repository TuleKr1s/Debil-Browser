#include "browser_framework.h"

Browser_Framework::Browser_Framework(QWidget *parent)
    : QWidget{parent}
{

    viewing_window = new BrowserView(this);

    musicInit();

    main_pannel = new MainPannel(this);

    // necessary connections
    connectNavigation();
    connectAddressBar();


    main_lay = new QVBoxLayout(this);
    main_lay->addWidget(main_pannel);
    main_lay->addWidget(viewing_window);


    this->setLayout(main_lay);
    this->showMaximized();
}


// method connecting interactive buttons with page
void Browser_Framework::connectNavigation()
{
    connect(main_pannel->nav_wgt->back_btn, SIGNAL(clicked()),
            viewing_window, SLOT(goBack()));
    connect(main_pannel->nav_wgt->forward_btn, SIGNAL(clicked()),
            viewing_window, SLOT(goForward()));
    connect(main_pannel->nav_wgt->reload_btn, SIGNAL(clicked()),
            viewing_window, SLOT(reloadPage()));
}

void Browser_Framework::connectAddressBar()
{
    connect(main_pannel->address_bar, SIGNAL(navigateRequested(QUrl)),
            viewing_window, SLOT(setURL(QUrl)));
}

void Browser_Framework::musicInit()
{
    m_player = new QMediaPlayer(this);
    m_audio_output = new QAudioOutput(this);

    m_player->setAudioOutput(m_audio_output);

    m_audio_output->setVolume(1.);

    m_player->setSource(QUrl("qrc:/assets/multimedia/DebilBrowser_MainTheme.mp3"));

    m_player->play();

    connect(m_player, &QMediaPlayer::mediaStatusChanged, this,
            [this](QMediaPlayer::MediaStatus status)
            {
                if (status != QMediaPlayer::EndOfMedia)
                    return;

                m_player->setPosition(0);
                m_player->play();
            });
}
