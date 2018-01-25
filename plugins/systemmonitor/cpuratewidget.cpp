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

#include "cpuratewidget.h"
#include "cpuballwidget.h"

#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>

CpuRateWidget::CpuRateWidget(QWidget *parent) : QWidget(parent)
{
//    this->setFixedSize(302, 140);

    /*QVBoxLayout **/mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *w = new QWidget;
    w->setFixedSize(300, 300);
    m_widgetLayout = new QVBoxLayout(w);
    m_widgetLayout->setContentsMargins(6, 0, 0, 0);
    m_widgetLayout->setSpacing(0);

    m_title = new QLabel(tr("CPU"));
    m_title->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_title->setStyleSheet("font-size: 22px; color:#303030");

    QFont font = m_title->font();
    font.setPointSize(22);
    font.setWeight(QFont::Light);
    m_title->setFont(font);

    m_cpuBall = new CpuBallWidget;
//    m_cpuBall->setFixedSize(100, 100);

    m_widgetLayout->addWidget(m_title, 0, Qt::AlignLeft);
    m_widgetLayout->addWidget(m_cpuBall, 0, Qt::AlignCenter);

    mainLayout->addWidget(w, 1, Qt::AlignCenter);


//    mainLayout->addWidget(m_title, 0, Qt::AlignLeft);
//    mainLayout->addWidget(m_cpuBall, 0, Qt::AlignCenter);

    /*sudo dmidecode -t processor
    u32 len;
    u8 *buf;
    size_t size = len;
    buf = read_file(&size, devmem);
    len = size;
//    dmi_table_decode(buf, len, num, ver, flags);
//    dmi_decode(&h, ver);*/

    m_cpuBall->startTimer();
}

CpuRateWidget::~CpuRateWidget()
{
    delete m_title;
    delete m_cpuBall;
    QLayoutItem *child;
    while ((child = m_widgetLayout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
    delete mainLayout;
}

void CpuRateWidget::onUpdateCpuPercent(double value)
{
    m_cpuBall->updateCpuPercent(value);
}

void CpuRateWidget::startTimer()
{
    m_cpuBall->startTimer();
}

void CpuRateWidget::stopTimer()
{
    m_cpuBall->stopTimer();
}
