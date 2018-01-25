#include "memorywidget.h"
#include "memorycircle.h"

#include <QDebug>
#include <QObject>

MemoryWidget::MemoryWidget(QWidget *parent)
    : QWidget(parent)
{
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *w = new QWidget;
    w->setFixedSize(500, 400);
    m_widgetLayout = new QVBoxLayout(w);
    m_widgetLayout->setContentsMargins(6, 0, 0, 0);
    m_widgetLayout->setSpacing(0);

    m_title = new QLabel(tr("Memory"));
    m_title->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_title->setStyleSheet("font-size: 22px; color:#303030");

    QFont font = m_title->font();
    font.setPointSize(22);
    font.setWeight(QFont::Light);
    m_title->setFont(font);

    m_memoryCircle = new MemoryCircle;
    connect(m_memoryCircle, SIGNAL(rebackMemoryInfo(QString,double)), this, SIGNAL(rebackMemoryInfo(QString,double)));

    m_widgetLayout->addWidget(m_title, 0, Qt::AlignTop);
    m_widgetLayout->addWidget(m_memoryCircle);

    mainLayout->addWidget(w, 0, Qt::AlignCenter);
}

MemoryWidget::~MemoryWidget()
{
    delete m_title;
    delete m_memoryCircle;
    QLayoutItem *child;
    while ((child = m_widgetLayout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
    delete mainLayout;
}

void MemoryWidget::onUpdateMemoryStatus()
{
    m_memoryCircle->onUpdateMemoryStatus();
}
