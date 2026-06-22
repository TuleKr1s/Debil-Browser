#ifndef POPUPMENUWIDGET_H
#define POPUPMENUWIDGET_H

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>

#include <vector>

class PopupMenuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupMenuWidget(QWidget *parent = nullptr);

    std::vector<QPushButton*> btn_list;
    QVBoxLayout* popup_lay;

    QPushButton* createBtn(QString name);

private slots:

    void update();

signals:
    void btnCreated();
};

#endif // POPUPMENUWIDGET_H
