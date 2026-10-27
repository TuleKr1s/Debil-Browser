#ifndef BROWSER_FRAMEWORK_H
#define BROWSER_FRAMEWORK_H

#include <QWidget>
#include <QBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QEvent>

#include "../custom_widgets/Tabs/mergered_tabscontent.h"
#include "mainpannel.h"
#include "headerpanel.h"

#include "../custom_widgets/Tabs/apptabs_factory.h"
#include "../custom_widgets/Tabs/browserview.h"
// Core structure for whole browser
// It connects main widgets at display and logic levels

class Browser_Framework : public QWidget
{
    Q_OBJECT
public:
    explicit Browser_Framework(QWidget *parent = nullptr);

protected:
#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
#endif

private:


    TabManager* m_tab_manager;

    MainPannel* main_pannel;
    HeaderPanel* header_panel = nullptr;
#ifdef Q_OS_WIN
    bool m_maximizePressed = false;
    QRect m_preservedNormalPlacement;
#endif


    QVBoxLayout* main_lay;

    // method connecting interactive buttons with page
    void connectNavigation();
    //
    void connectAddressBar();

    // updating address bar behavior based on current tab
    void updateAddressBar(const QUrl& url);
    // updating navigation bar
    void updateNavigation();


};

#endif // BROWSER_FRAMEWORK_H
