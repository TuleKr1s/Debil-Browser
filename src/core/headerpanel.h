#ifndef HEADERPANEL_H
#define HEADERPANEL_H

#include <QWidget>
#include <QHBoxLayout>

#include "../custom_widgets/Tabs/mergered_tabscontent.h"

class HeaderPanel : public QWidget
{
    Q_OBJECT
public:
    explicit HeaderPanel(QWidget *parent = nullptr);

private:

    int widget_height = 35;

    QHBoxLayout* header_lay;

};

#endif // HEADERPANEL_H
