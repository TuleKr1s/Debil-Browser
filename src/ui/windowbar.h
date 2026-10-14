#ifndef WINDOWBAR_H
#define WINDOWBAR_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QEvent>


class WindowBar : public QWidget
{
    Q_OBJECT
public:
    explicit WindowBar(QWidget *parent = nullptr);

private:

    QPushButton* m_menuBtn;
    QPushButton* m_minimizeBtn;
    QPushButton* m_fullWindowBtn;
    QPushButton* m_closeWindowBtn;

    QHBoxLayout* window_bar_lay;

    QIcon full_window_btn, half_window_btn;
    QIcon menu_btn, minimize_btn, close_btn;


protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:

    void onCloseBtnClicked();
    void onMinimizeBtnClicked();
    void onFullWindowBtnClicked();

};

#endif // WINDOWBAR_H
