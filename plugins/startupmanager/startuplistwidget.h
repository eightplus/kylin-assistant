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

#ifndef STARTUPLISTWIDGET_H
#define STARTUPLISTWIDGET_H

#include "startupitem.h"

#include <QListWidget>

class StartupWorker;
class StartupData;

class StartupListWidget : public QListWidget
{
    Q_OBJECT

public:
    StartupListWidget(QWidget *parent=0);
    ~StartupListWidget();

    void loadItem(StartupData info);
    void loadItems(QStringList items, int scrollValue);

    void listAllDesktopFileFromDir(QString directory);
    void setAppAutoStartup(StartupData info, bool enabled);

public slots:
    void onChangeStartup(const QString &exec, bool active);
    void onMouseEnter();

private:
    StartupWorker *m_startupWorker = nullptr;
};

#endif // STARTUPLISTWIDGET_H
