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

#ifndef CPURATEWIDGET_H
#define CPURATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class CpuBallWidget;

class CpuRateWidget : public QWidget
{
    Q_OBJECT

public:
    CpuRateWidget(QWidget *parent = 0);
    ~CpuRateWidget();

    void startTimer();
    void stopTimer();

public slots:
    void onUpdateCpuPercent(double value);

private:
    QLabel *m_title = nullptr;
    CpuBallWidget *m_cpuBall = nullptr;
    QVBoxLayout *m_widgetLayout = nullptr;
    QHBoxLayout *mainLayout = nullptr;
};

#endif // CPURATEWIDGET_H
