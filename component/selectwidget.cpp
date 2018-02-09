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

#include "selectwidget.h"
#include "utils.h"

#include <QApplication>

SelectWidget::SelectWidget(bool needMin, QWidget *parent)
    : QDialog(parent)
    , m_mousePressed(false)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(464, 500);

    QWidget *containerW = new QWidget(this);
    m_mainLayout = new QVBoxLayout(containerW);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);
    m_titleBar = new MyTitleBar(needMin, this);
    m_titleBar->setFixedSize(this->width(), TITILE_BAR_HEIGHT);
    m_listWidget = new SelectListWidget(this);
    m_listWidget->setFixedSize(this->width(), this->height() - TITILE_BAR_HEIGHT);
    m_mainLayout->addWidget(m_titleBar);
    m_mainLayout->addWidget(m_listWidget);

    connect(m_titleBar, SIGNAL(minSignal()),this, SLOT(hide()));
    connect(m_titleBar, SIGNAL(closeSignal()),this, SLOT(close()));
    connect(m_listWidget, SIGNAL(notifyMainCheckBox(int)), this, SIGNAL(notifyMainCheckBox(int)));

    QDesktopWidget* desktop = QApplication::desktop();
    this->move((desktop->width() - this->width())/2, (desktop->height() - this->height())/3);
}

SelectWidget::~SelectWidget()
{

}

void SelectWidget::loadData(const QString &title, const QStringList &cachelist)
{
    m_listWidget->loadListItems(title, cachelist, this->width());
}

void SelectWidget::moveCenter()
{
    /*QPoint pos = QCursor::pos();
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
    this->show();
    this->raise();*/
}


void SelectWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        this->m_mousePressed = true;
    }

    QDialog::mousePressEvent(event);
}

void SelectWidget::mouseReleaseEvent(QMouseEvent *event)
{
    this->m_mousePressed = false;
    setWindowOpacity(1);

    QDialog::mouseReleaseEvent(event);
}

void SelectWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (this->m_mousePressed) {
        move(event->globalPos() - this->m_dragPosition);
        setWindowOpacity(0.9);
    }

    QDialog::mouseMoveEvent(event);
}
