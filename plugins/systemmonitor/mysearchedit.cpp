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
    this->setStyleSheet("QFrame{border-radius: 4px;}");
    initInsideFrame();

    m_searchBtn = new QLabel;
    m_searchBtn->setStyleSheet("QLabel{background-color:transparent;margin: 2 -1 2 4 px;border-image:url(:/res/search.png);}");
    m_searchBtn->setFixedSize(16, 16);
    m_clearBtn = new MyImageButton;
    m_clearBtn->setObjectName("ClearIcon");
    m_clearBtn->hide();
    m_edit = new QLineEdit;
    m_edit->setStyleSheet("QLineEdit{background-color:transparent;border-radius:0px;color:#303030;padding-right:15px;padding-bottom: 3px;}");

    m_animation = new QPropertyAnimation(m_edit, "minimumWidth");

    m_size = QSize(m_searchBtn->sizeHint().width() + m_edit->sizeHint().width() + m_clearBtn->sizeHint().width() + 6,
                   qMax(m_searchBtn->sizeHint().height(), m_edit->sizeHint().height()));
    m_edit->setFixedWidth(0);
    m_edit->installEventFilter(this);

    QHBoxLayout *layout = new QHBoxLayout(m_insideFrame);
    layout->addStretch();
    layout->addWidget(m_searchBtn);
    layout->setAlignment(m_searchBtn, Qt::AlignCenter);
    layout->addWidget(m_edit);
    layout->setAlignment(m_edit, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(m_clearBtn);
    layout->setAlignment(m_clearBtn, Qt::AlignCenter);
    layout->setSpacing(0);
    layout->setContentsMargins(3, 0, 3, 0);

    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);

    connect(m_clearBtn, &MyImageButton::clicked, m_edit, static_cast<void (QLineEdit::*)()>(&QLineEdit::setFocus));
    connect(m_clearBtn, &MyImageButton::clicked, this, &MySearchEdit::clear);
    connect(m_edit, &QLineEdit::textChanged, [this] {m_clearBtn->setVisible(!m_edit->text().isEmpty());});
    connect(m_edit, &QLineEdit::textChanged, this, &MySearchEdit::textChanged, Qt::DirectConnection);
}

MySearchEdit::~MySearchEdit()
{
    m_animation->deleteLater();
}

const QString MySearchEdit::text() const
{
    return m_edit->text();
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
        }
    }

    return QFrame::eventFilter(object, event);
}

QLineEdit *MySearchEdit::getLineEdit() const
{
    return m_edit;
}

void MySearchEdit::setEditFocus()
{
    m_animation->stop();
    m_animation->setStartValue(0);
    m_animation->setEndValue(m_size.width() - m_searchBtn->width() - 6);
    m_animation->setEasingCurve(m_showCurve);
    m_animation->start();

    m_edit->setFocus();
}

void MySearchEdit::initInsideFrame()
{
    m_insideFrame = new QFrame(this);
    m_insideFrame->raise();
    m_insideFrame->setStyleSheet("QFrame{background-color: white;border: 1px solid;border-radius: 4px;border-color: rgba(0, 0, 0, 0.08);}");
    QHBoxLayout *insideLayout = new QHBoxLayout(this);
    insideLayout->setContentsMargins(0, 0, 0, 1);
    insideLayout->setSpacing(0);
    insideLayout->addWidget(m_insideFrame);
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
