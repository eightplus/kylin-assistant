#include "mysearchedit.h"

#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPropertyAnimation>
#include <QDebug>
#include <QEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QTimer>

MySearchEdit::MySearchEdit(QWidget *parent)
    : QFrame(parent)
{
//    this->setStyleSheet("QFrame{background-color:#00376a;border-radius:0px;}");
    this->setStyleSheet("QFrame{background-color:rgb(0, 55, 106, 127);border-radius:0px;}");

    m_searchBtn = new QLabel;
    m_searchBtn->setStyleSheet("QLabel{background-color:transparent;border:none;background-image:url(:/res/search.png);}");
    m_searchBtn->setFixedSize(16, 16);

    m_clearBtn = new MyTristateButton;
    m_clearBtn->setObjectName("ClearIcon");
    m_clearBtn->hide();

    m_edit = new QLineEdit;
    m_edit->setStyleSheet("QLineEdit{background-color:transparent;border-radius:0px;color:#ffffff;padding-right:12px;padding-bottom: 2px;}");
    //m_edit->setPlaceholderText("enter process info");

    m_placeHolder = new QLabel;
    m_placeHolder->setStyleSheet("QLabel{background-color:transparent;color:#808080;font-size:12px;margin: 2 0 0 0 px;} QLabel:hover{color:#ffffff;font-size:12px;}");

    m_animation = new QPropertyAnimation(m_edit, "minimumWidth");

    m_size = QSize(m_searchBtn->sizeHint().width() + m_edit->sizeHint().width() + m_clearBtn->sizeHint().width() + 6,
                   qMax(m_searchBtn->sizeHint().height(), m_edit->sizeHint().height()));
    m_edit->setFixedWidth(0);
    m_edit->installEventFilter(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addStretch();
    layout->addWidget(m_searchBtn);
    layout->setAlignment(m_searchBtn, Qt::AlignCenter);
    layout->addWidget(m_placeHolder);
    layout->setAlignment(m_placeHolder, Qt::AlignCenter);
    layout->addWidget(m_edit);
    layout->setAlignment(m_edit, Qt::AlignCenter);
//    layout->addStretch();
//    layout->addWidget(m_searchBtn);
//    layout->setAlignment(m_searchBtn, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(m_clearBtn);
    layout->setAlignment(m_clearBtn, Qt::AlignCenter);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

//    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);

    connect(m_clearBtn, &MyTristateButton::clicked, m_edit, static_cast<void (QLineEdit::*)()>(&QLineEdit::setFocus));
    connect(m_clearBtn, &MyTristateButton::clicked, this, &MySearchEdit::clearEdit);
    connect(m_edit, &QLineEdit::textChanged, [this] {m_clearBtn->setVisible(!m_edit->text().isEmpty());});
    connect(m_edit, &QLineEdit::textChanged, this, &MySearchEdit::textChanged, Qt::DirectConnection);
//    connect(m_edit, &QLineEdit::textChanged, this, [=] {
//        emit this->textChanged();
//    });
}

MySearchEdit::~MySearchEdit()
{
    m_animation->deleteLater();
}

const QString MySearchEdit::text() const
{
    return m_edit->text();
}

void MySearchEdit::clearEdit()
{
    m_edit->clear();
//    this->setStyleSheet("QFrame{background-color:#00376a;border-radius:0px;}");
    this->setStyleSheet("QFrame{background-color:rgb(0, 55, 106, 127);border-radius:0px;}");
}

void MySearchEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return QFrame::mousePressEvent(event);

    setEditFocus();
    event->accept();
}

void MySearchEdit::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
}

bool MySearchEdit::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_edit && event->type() == QEvent::FocusOut && m_edit->text().isEmpty()) {
        auto focusEvent = dynamic_cast<QFocusEvent *>(event);
        if (focusEvent && focusEvent->reason() != Qt::PopupFocusReason) {
            m_animation->stop();
            m_animation->setStartValue(m_edit->width());
            m_animation->setEndValue(0);
            m_animation->setEasingCurve(m_hideCurve);
            m_animation->start();
            connect(m_animation, &QPropertyAnimation::finished, m_placeHolder, &QLabel::show);
        }
    }

    return QFrame::eventFilter(object, event);
}

void MySearchEdit::setEditFocus()
{
    if (!m_placeHolder->isVisible()) {
        return;
    }
    disconnect(m_animation, &QPropertyAnimation::finished, m_placeHolder, &QLabel::show);
    m_animation->stop();
    m_animation->setStartValue(0);
    m_animation->setEndValue(m_size.width() - m_searchBtn->width() - 6);
    m_animation->setEasingCurve(m_showCurve);
    m_animation->start();
    m_placeHolder->hide();
    m_edit->setFocus();
//    this->setStyleSheet("QFrame{background-color:#00376a;border:1px solid #47ccf3;border-radius:0px;}");
    this->setStyleSheet("QFrame{background-color:rgb(0, 55, 106, 127);border:1px solid #47ccf3;border-radius:0px;}");
}


QLineEdit *MySearchEdit::getLineEdit() const
{
    return m_edit;
}

void MySearchEdit::resizeEvent(QResizeEvent *event)
{
    m_size = event->size();
    m_edit->setFixedHeight(m_size.height());
}

bool MySearchEdit::event(QEvent *event)
{
    if (event->type() == QEvent::FocusIn) {
        const QFocusEvent *ev = static_cast<QFocusEvent*>(event);

        if (ev->reason() == Qt::TabFocusReason
                || ev->reason() == Qt::BacktabFocusReason
                || ev->reason() == Qt::OtherFocusReason
                || ev->reason() == Qt::ShortcutFocusReason) {
            setEditFocus();
        }
    }

    return QFrame::event(event);
}
