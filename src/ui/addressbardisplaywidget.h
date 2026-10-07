#ifndef ADDRESSBARDISPLAYWIDGET_H
#define ADDRESSBARDISPLAYWIDGET_H

#include <QUrl>
#include <QWidget>

class QLabel;
class QEvent;
class QMouseEvent;
class QPushButton;
class QResizeEvent;
class PopupMenuWidget;

class AddressBarDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddressBarDisplayWidget(QWidget* parent = nullptr);

    void setUrl(const QUrl& url);
    void setTitle(const QString& title);
    void setPlaceholderText(const QString& text);

signals:
    void editRequested();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateDisplayText();
    void updateTitleGeometry();

    QLabel* url_label;
    QLabel* title_label;
    QWidget* btn_menu;
    QPushButton* browser_protection_btn;
    PopupMenuWidget* page_menu_btn;

    QUrl current_url;
    QString current_title;
    QString placeholder_text;
};

#endif // ADDRESSBARDISPLAYWIDGET_H
