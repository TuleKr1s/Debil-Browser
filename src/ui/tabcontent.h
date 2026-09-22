#ifndef TABCONTENT_H
#define TABCONTENT_H

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "../core/browserview.h"

class TabContent : public QWidget
{
    Q_OBJECT
public:
    explicit TabContent(QWidget *parent = nullptr);

    void addView(QWidget* view);
    void removeView(QWidget* view);
    void showView(QWidget* view);



private:

    QStackedWidget* m_stackedView;

    QVBoxLayout* content_lay;
};

#endif // TABCONTENT_H
