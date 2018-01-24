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

#ifndef CPUOCCUPANCYRATE_H
#define CPUOCCUPANCYRATE_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class CpuBallWidget;

class CpuOccupancyRate : public QWidget
{
    Q_OBJECT

public:
    CpuOccupancyRate(QWidget *parent = 0);
    ~CpuOccupancyRate();

    void startTimer();
    void stopTimer();

public slots:
    void onUpdateCpuPercent(double value);

private:
    QLabel *m_title = nullptr;
    CpuBallWidget *m_cpuBall = nullptr;
    QHBoxLayout *m_titleLeftLayout = nullptr;
    QVBoxLayout *mainLayout = nullptr;
};

#endif // CPUOCCUPANCYRATE_H
