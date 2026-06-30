#ifndef POPUPMENUWIDGET_H
#define POPUPMENUWIDGET_H

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QSizePolicy>

#include <vector>

class PopupMenuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupMenuWidget(QWidget *parent = nullptr);

    std::vector<QPushButton*> btn_list;

    QSize sizeHint() const override;
    QPushButton* createBtn(QString name);
    QPushButton* getMainBtn();
    void setMainBtnText(const QString& text);

private:

    QPushButton* main_btn;
    QVBoxLayout* popup_lay;
    QWidget* popup_menu;


    void showPopup();
};

#endif // POPUPMENUWIDGET_H
