/*
 * Copyright (C) 2013 ~ 2018 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    xiangli@ubuntukylin.com/kobe24_lixiang@126.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "toolbar.h"
#include "myimagebutton.h"
#include "mytipimagebutton.h"
#include "processcategory.h"
#include "mysearchedit.h"
#include "util.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleFactory>

ToolBar::ToolBar(QSettings *settings, QWidget *parent)
    :QFrame(parent)
    ,proSettings(settings)
{
    installEventFilter(this);
    setMouseTracking(true);
    setFixedHeight(TOP_TITLE_WIDGET_HEIGHT);

    m_topBorderColor = QColor(255, 255, 255, 153);
//    this->setAutoFillBackground(true);
//    QPalette palette;
//    palette.setColor(QPalette::Background, QColor("#0d87ca"));
//    this->setPalette(palette);

    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    connect(m_searchTimer, SIGNAL(timeout()), this, SLOT(onRefreshSearchResult()));

    initWidgets();
}

ToolBar::~ToolBar()
{
    delete processLabel;
    delete m_searchEdit;
    delete m_cancelSearchBtn;
    delete processCategory;

    if (m_searchTimer) {
        disconnect(m_searchTimer, SIGNAL(timeout()), this, SLOT(onRefreshSearchResult()));
        if(m_searchTimer->isActive()) {
            m_searchTimer->stop();
        }
        delete m_searchTimer;
        m_searchTimer = nullptr;
    }

    //Segmentation fault
    QLayoutItem *child;
    while ((child = m_lLayout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
    while ((child = m_mLayout->takeAt(0)) != 0) {
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

bool ToolBar::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        if (obj == this) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Escape) {
                m_searchEdit->clear();
                emit canelSearchEditFocus();
            }
        }
        else if (obj == m_searchEdit->getLineEdit()) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Tab) {
                emit canelSearchEditFocus();
            }
        }
    }

    return QFrame::eventFilter(obj, event);
}

void ToolBar::setSearchEditFocus()
{
    if (m_searchEdit->text() != "") {
        m_searchEdit->getLineEdit()->setFocus();
    } else {
        m_searchEdit->setFocus();
    }
}

void ToolBar::onRefreshSearchResult()
{
    if (m_searchEdit->text() == searchTextCache) {
        emit this->searchSignal(searchTextCache);
    }
}

void ToolBar::handleSearchTextChanged()
{
    searchTextCache = m_searchEdit->text();

    this->m_cancelSearchBtn->setVisible(!searchTextCache.isEmpty());

    if (m_searchTimer->isActive()) {
        m_searchTimer->stop();
    }
    m_searchTimer->start(300);
}

void ToolBar::onCancelSearchBtnClicked(bool b)
{
    this->m_cancelSearchBtn->setVisible(false);
    m_searchEdit->clear();
    emit canelSearchEditFocus();
}

void ToolBar::paintEvent(QPaintEvent *e)
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


void ToolBar::initLeftContent()
{
    QWidget *w = new QWidget;
    m_lLayout = new QHBoxLayout(w);
    m_lLayout->setContentsMargins(6, 0, 0, 0);
    m_lLayout->setSpacing(0);

    processLabel = new QLabel("");
    processLabel->setStyleSheet("QLabel { background-color : transparent; color : #505050; }");
    m_lLayout->addWidget(processLabel);

    m_layout->addWidget(w, 1, Qt::AlignLeft);
}

void ToolBar::initMiddleContent()
{
    QWidget *w = new QWidget;
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_mLayout = new QHBoxLayout(w);
    m_mLayout->setContentsMargins(0, 0, 0, 0);
    m_mLayout->setSpacing(0);

    MyTipImageButton *processButton = new MyTipImageButton();
    processButton->setCheckable(true);
    processButton->setChecked(true);
    processButton->setObjectName("ProcessBtn");

    MyTipImageButton *resourcesButton = new MyTipImageButton();
    resourcesButton->setCheckable(true);
    resourcesButton->setChecked(false);
    resourcesButton->setObjectName("ResourcesBtn");

    MyTipImageButton *disksButton = new MyTipImageButton();
    disksButton->setCheckable(true);
    disksButton->setChecked(false);
    disksButton->setObjectName("DisksBtn");

    connect(processButton, &MyTipImageButton::clicked, this, [=] {
        emit this->changePage(0);
        processButton->setChecked(true);
        resourcesButton->setChecked(false);
        disksButton->setChecked(false);
        if (!m_searchEdit->isVisible())
            m_searchEdit->setVisible(true);
        if (!processCategory->isVisible())
            processCategory->setVisible(true);
    });
    connect(resourcesButton, &MyTipImageButton::clicked, this, [=] {
        emit this->changePage(1);
        processButton->setChecked(false);
        resourcesButton->setChecked(true);
        disksButton->setChecked(false);
        if (m_searchEdit->isVisible())
            m_searchEdit->setVisible(false);
        if (processCategory->isVisible())
            processCategory->setVisible(false);
        m_searchEdit->clear();
        emit canelSearchEditFocus();
    });
    connect(disksButton, &MyTipImageButton::clicked, this, [=] {
        emit this->changePage(2);
        processButton->setChecked(false);
        resourcesButton->setChecked(false);
        disksButton->setChecked(true);
        if (m_searchEdit->isVisible())
            m_searchEdit->setVisible(false);
        if (processCategory->isVisible())
            processCategory->setVisible(false);
        m_searchEdit->clear();
        emit canelSearchEditFocus();
    });
    processButton->setToolTip(tr("Processes"));
    resourcesButton->setToolTip(tr("Resources"));
    disksButton->setToolTip(tr("File Systems"));

    m_mLayout->setContentsMargins(0, 0, 0, 0);
    m_mLayout->setSpacing(10);
    m_mLayout->addStretch();
    m_mLayout->addWidget(processButton);
    m_mLayout->addWidget(resourcesButton);
    m_mLayout->addWidget(disksButton);
    m_mLayout->addStretch();

    m_layout->addWidget(w);
}

void ToolBar::initRightContent()
{
    QWidget *w = new QWidget;
    m_rLayout = new QHBoxLayout(w);
    m_rLayout->setContentsMargins(0, 3, 6, 3);
    m_rLayout->setSpacing(5);

    m_cancelSearchBtn = new QPushButton;
    m_cancelSearchBtn->setObjectName("blackButton");
    m_cancelSearchBtn->setText(tr("Cancel"));
    m_cancelSearchBtn->setFocusPolicy(Qt::NoFocus);
    m_cancelSearchBtn->setFixedSize(46, 25);
    m_cancelSearchBtn->setVisible(false);
    connect(m_cancelSearchBtn, SIGNAL(clicked(bool)), SLOT(onCancelSearchBtnClicked(bool)));

    connect(m_searchEdit, &MySearchEdit::textChanged, this, &ToolBar::handleSearchTextChanged, Qt::QueuedConnection);
    m_rLayout->addWidget(m_searchEdit);
    m_rLayout->addWidget(m_cancelSearchBtn);

    QString whose_processes = "user";
    proSettings->beginGroup("PROCESS");
    whose_processes = proSettings->value("WhoseProcesses", whose_processes).toString();
    proSettings->endGroup();
    int tabIndex = 1;
    if (whose_processes == "active") {
        tabIndex = 0;
    }
    else if (whose_processes == "all") {
        tabIndex = 2;
    }
    else {
        tabIndex = 1;
    }
    processCategory = new ProcessCategory(tabIndex);
    connect(processCategory, SIGNAL(activeWhoseProcessList(int)), this, SIGNAL(activeWhoseProcessList(int)));
    m_rLayout->addSpacing(10);
    m_rLayout->addWidget(processCategory, 0, Qt::AlignVCenter);
    m_layout->addWidget(w, 1, Qt::AlignRight);
}

void ToolBar::initWidgets()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    this->setLayout(m_layout);

    m_searchEdit = new MySearchEdit();
    m_searchEdit->setFixedWidth(222);
    m_searchEdit->getLineEdit()->installEventFilter(this);

    initLeftContent();
    initMiddleContent();
    initRightContent();
}
