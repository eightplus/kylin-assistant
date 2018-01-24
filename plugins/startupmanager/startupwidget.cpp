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

#include "startupwidget.h"
#include "startuptitlewidget.h"
#include "startuplistwidget.h"

#include <QFileSystemWatcher>
#include <QLabel>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>

StartupWidget::StartupWidget(QWidget *parent)
    : QFrame(parent)
    , mousePressed(false)
{
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint  | Qt::WindowCloseButtonHint);//去掉边框
    this->setAttribute(Qt::WA_TranslucentBackground);//背景透明

    this->setAutoFillBackground(true);
    this->setMouseTracking(true);

    this->setFixedSize(500, 645);

    m_titleWidget = new StartupTitleWidget(this);
    m_titleWidget->setFixedSize(this->width(), 39);

    m_layout = new QVBoxLayout();
    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0,0,0,0);
    setLayout(m_layout);

    m_startupView = new StartupListWidget(this);
    m_layout->addWidget(m_titleWidget, 0, Qt::AlignTop);
    m_layout->addWidget(m_startupView, 0, Qt::AlignHCenter);

    this->moveCenter();
}

StartupWidget::~StartupWidget()
{
    if (m_titleWidget) {
        delete m_titleWidget;
        m_titleWidget = nullptr;
    }
    if (m_startupView) {
        delete m_startupView;
        m_startupView = nullptr;
    }
    delete m_layout;
}

void StartupWidget::moveCenter()
{
    QPoint pos = QCursor::pos();
    QRect primaryGeometry;
    for (QScreen *screen : qApp->screens()) {
        if (screen->geometry().contains(pos)) {
            primaryGeometry = screen->geometry();
        }
    }

    if (primaryGeometry.isEmpty()) {
        primaryGeometry = qApp->primaryScreen()->geometry();
    }

    this->move(primaryGeometry.x() + (primaryGeometry.width() - this->width())/2,
               primaryGeometry.y() + (primaryGeometry.height() - this->height())/2);
}

void StartupWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void StartupWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPainterPath path;
    path.addRect(QRectF(rect()));
    painter.setOpacity(1);
    painter.fillPath(path, QColor("#FFFFFF"));
}

void StartupWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->dragPosition = event->globalPos() - frameGeometry().topLeft();
        this->mousePressed = true;
    }
    QFrame::mousePressEvent(event);
}

void StartupWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->mousePressed = false;
    }

    QFrame::mouseReleaseEvent(event);
}

void StartupWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (this->mousePressed) {
        move(event->globalPos() - this->dragPosition);
    }

    QFrame::mouseMoveEvent(event);
}
