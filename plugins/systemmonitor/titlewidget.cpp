#include "titlewidget.h"
#include "myimagebutton.h"
#include "mytipimagebutton.h"
#include "util.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleFactory>

TitleWidget::TitleWidget(QWidget *parent)
    :QFrame(parent)
{
    installEventFilter(this);
    setMouseTracking(true);
    setFixedHeight(TOP_TITLE_WIDGET_HEIGHT);

    m_topBorderColor = QColor(255, 255, 255, 153);
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#0d87ca"));
    this->setPalette(palette);

    initWidgets();
}

TitleWidget::~TitleWidget()
{
    //Segmentation fault
    QLayoutItem *child;
    while ((child = m_lLayout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }

    while ((child = m_rLayout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
    delete m_layout;
}

void TitleWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        if (window()->isMaximized())
            window()->showNormal();
        else if (! window()->isFullScreen())
            window()->showMaximized();
    }

    QFrame::mouseDoubleClickEvent(e);
}

void TitleWidget::paintEvent(QPaintEvent *e)
{

    QFrame::paintEvent(e);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QColor tc(m_topBorderColor);
    int borderHeight = 1;
    QPainterPath tPath;
    tPath.moveTo(QPointF(x(), y() + borderHeight - 0.5));
    tPath.lineTo(QPointF(x() + width(), y() + borderHeight - 0.5));
    p.setPen(QPen(tc));
    p.drawPath(tPath);
}

void TitleWidget::initLeftContent()
{
    QWidget *w = new QWidget;
    m_lLayout = new QHBoxLayout(w);
    m_lLayout->setContentsMargins(6, 0, 0, 0);
    m_lLayout->setSpacing(0);

    QLabel *label = new QLabel;
    label->setStyleSheet("QLabel{border-image: url(:/res/kylin-assistant.png);}");
    label->setFixedSize(24, 24);
    m_lLayout->addWidget(label);

    QLabel *titleLabel = new QLabel;
    titleLabel->setStyleSheet("QLabel{background-color:transparent;color:#ffffff; font-size:12px;}");
    titleLabel->setText(tr("Kylin System Monitor"));
    m_lLayout->addSpacing(5);
    m_lLayout->addWidget(titleLabel);

    m_layout->addWidget(w, 1, Qt::AlignLeft);
}

void TitleWidget::initRightContent()
{
    QWidget *w = new QWidget;
    m_rLayout = new QHBoxLayout(w);
    m_rLayout->setContentsMargins(0, 0, 6, 0);
    m_rLayout->setSpacing(0);

    m_layout->addWidget(w, 1, Qt::AlignRight);

    MyImageButton *minBtn = new MyImageButton;
    minBtn->setObjectName("MinButton");
    connect(minBtn, &MyImageButton::clicked, this, [=] {
        if (parentWidget() && parentWidget()->parentWidget()) {
            parentWidget()->parentWidget()->showMinimized();
        }
    });
    MyImageButton *maxBtn = new MyImageButton;
    maxBtn->setObjectName("MaxButton");
    connect(maxBtn, &MyImageButton::clicked, this, [=] {
        if (window()->isMaximized()) {
            window()->showNormal();
            maxBtn->setObjectName("MaxButton");
        }
        else {
            window()->showMaximized();
            maxBtn->setObjectName("UnMaxButton");
        }
    });
    connect(this, &TitleWidget::updateMaxBtn, this, [=]{
        if (window()->isMaximized()) {
            maxBtn->setObjectName("UnMaxButton");
        } else {
            maxBtn->setObjectName("MaxButton");
        }
    });
    MyImageButton *closeBtn = new MyImageButton;
    closeBtn->setObjectName("CloseButton");
    connect(closeBtn, &MyImageButton::clicked, this, [=] {
        window()->close();
    });

    m_rLayout->addWidget(minBtn);
    m_rLayout->addWidget(maxBtn);
    m_rLayout->addWidget(closeBtn);
}

void TitleWidget::initWidgets()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    this->setLayout(m_layout);

    initLeftContent();
    initRightContent();
}
