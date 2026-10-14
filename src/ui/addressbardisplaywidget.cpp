#include "addressbardisplaywidget.h"

#include "../custom_widgets/popupmenuwidget.h"

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QResizeEvent>

AddressBarDisplayWidget::AddressBarDisplayWidget(QWidget* parent)
    : QWidget(parent)
    , url_label(new QLabel(this))
    , title_label(new QLabel(this))
    , btn_menu(new QWidget(this))
    , browser_protection_btn(new QPushButton(btn_menu))
    , page_menu_btn(new PopupMenuWidget(btn_menu))
{
    url_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    title_label->setAlignment(Qt::AlignCenter);
    url_label->setObjectName(QStringLiteral("addressBarUrlLabel"));
    title_label->setObjectName(QStringLiteral("addressBarTitleLabel"));
    url_label->installEventFilter(this);
    title_label->installEventFilter(this);

    page_menu_btn->setMainBtnIcon(QIcon(":/assets/MainPanel/page_menu_btn.png"));
    page_menu_btn->setToolTip("Page menu");
    page_menu_btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    browser_protection_btn->setIcon(QIcon(":/assets/Debil_Browser.ico"));
    browser_protection_btn->setToolTip("Debil protection");
    browser_protection_btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto* button_layout = new QHBoxLayout(btn_menu);
    button_layout->addWidget(page_menu_btn);
    button_layout->addWidget(browser_protection_btn);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(0);

    auto* display_layout = new QGridLayout(this);
    display_layout->setContentsMargins(8, 0, 8, 0);
    display_layout->setSpacing(6);
    display_layout->addWidget(url_label, 0, 0);
    display_layout->addWidget(btn_menu, 0, 1, Qt::AlignRight | Qt::AlignVCenter);
    display_layout->setColumnStretch(0, 1);
    display_layout->setColumnStretch(1, 0);

    title_label->raise();
}

void AddressBarDisplayWidget::setUrl(const QUrl& url)
{
    current_url = url;
    updateDisplayText();
}

void AddressBarDisplayWidget::setTitle(const QString& title)
{
    current_title = title;
    updateDisplayText();
}

void AddressBarDisplayWidget::setPlaceholderText(const QString& text)
{
    placeholder_text = text;
    updateDisplayText();
}

void AddressBarDisplayWidget::setDraftText(const QString& text, bool enabled)
{
    draft_text = text;
    showing_draft = enabled;
    updateDisplayText();
}

bool AddressBarDisplayWidget::eventFilter(QObject* watched, QEvent* event)
{
    if ((watched == url_label || watched == title_label) &&
        event->type() == QEvent::MouseButtonPress) {
        emit editRequested();
        return true;
    }

    return QWidget::eventFilter(watched, event);
}

void AddressBarDisplayWidget::mousePressEvent(QMouseEvent* event)
{
    emit editRequested();
    event->accept();
}

void AddressBarDisplayWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateDisplayText();
}

void AddressBarDisplayWidget::updateDisplayText()
{
    QString domain = current_url.host().isEmpty()
                         ? current_url.toDisplayString(QUrl::FullyDecoded)
                         : current_url.host();

    if (showing_draft)
        domain = draft_text;
    else if (domain.startsWith(QStringLiteral("www."), Qt::CaseInsensitive))
        domain.remove(0, 4);

    const bool show_placeholder = domain.isEmpty();
    url_label->setText(show_placeholder
                           ? placeholder_text
                           : url_label->fontMetrics().elidedText(
                                 domain, Qt::ElideRight, url_label->width()));

    QPalette url_palette = url_label->palette();
    url_palette.setColor(QPalette::WindowText,
                         palette().color(show_placeholder
                                             ? QPalette::PlaceholderText
                                             : QPalette::WindowText));
    url_label->setPalette(url_palette);

    url_label->setToolTip(show_placeholder
                              ? QString()
                              : (showing_draft ? draft_text : current_url.toDisplayString(QUrl::FullyDecoded)));
    title_label->setToolTip(current_title);
    updateTitleGeometry();
}

void AddressBarDisplayWidget::updateTitleGeometry()
{
    const int center = width() / 2;
    const int right_limit = btn_menu->geometry().left() - 6;
    const int half_width = qMax(0, qMin(center - 8, right_limit - center));
    const int maximum_width = half_width * 2;
    const QString visible_title = title_label->fontMetrics().elidedText(
        showing_draft ? QString() : current_title, Qt::ElideRight, maximum_width);

    title_label->setText(visible_title);
    const int title_width = qMin(title_label->sizeHint().width(), maximum_width);
    title_label->setGeometry(center - title_width / 2, 0, title_width, height());

    const int url_width = visible_title.isEmpty()
                              ? right_limit - 8
                              : title_label->geometry().left() - 14;
    url_label->setMaximumWidth(qMax(0, url_width));
    title_label->raise();
}
