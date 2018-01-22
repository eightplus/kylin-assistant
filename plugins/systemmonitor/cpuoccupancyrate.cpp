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

#include "cpuoccupancyrate.h"
#include "cpuballwidget.h"

#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>

CpuOccupancyRate::CpuOccupancyRate(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(302, 140);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_title = new QLabel(tr("CPU"));
    m_title->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_title->setStyleSheet("font-size: 22px; color:#303030");

    QFont font = m_title->font();
    font.setPointSize(22);
    font.setWeight(QFont::Light);
    m_title->setFont(font);

    m_cpuBall = new CpuBallWidget;
    m_cpuBall->setFixedSize(100, 100);

    mainLayout->addWidget(m_title, 0, Qt::AlignLeft);
    mainLayout->addWidget(m_cpuBall, 0, Qt::AlignCenter);

    m_cpuBall->startTimer();
}

CpuOccupancyRate::~CpuOccupancyRate()
{
    delete m_title;
    delete m_cpuBall;
}

void CpuOccupancyRate::onUpdateCpuPercent(double value)
{
    m_cpuBall->updateCpuPercent(value);
}

void CpuOccupancyRate::startTimer()
{
    m_cpuBall->startTimer();
}

void CpuOccupancyRate::stopTimer()
{
    m_cpuBall->stopTimer();
}
