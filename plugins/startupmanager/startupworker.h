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


#ifndef STARTUPWORKER_H
#define STARTUPWORKER_H

#include <QObject>
#include <QMap>

#include "startupdata.h"

#define KEY_FILE_DESKTOP_GROUP "Desktop Entry"
#define KEY_FILE_DESKTOP_KEY_HIDDEN "Hidden"
#define KEY_FILE_DESKTOP_KEY_NO_DISPLAY "NoDisplay"
#define KEY_FILE_DESKTOP_KEY_ONLY_SHOW_IN "OnlyShowIn"
#define KEY_FILE_DESKTOP_KEY_NOT_SHOW_IN "NotShowIn"
#define KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLED "X-GNOME-Autostart-enabled"
#define KEY_FILE_DESKTOP_KEY_NAME "Name"
#define KEY_FILE_DESKTOP_KEY_EXEC "Exec"
#define KEY_FILE_DESKTOP_KEY_COMMENT "Comment"
#define KEY_FILE_DESKTOP_KEY_ICON "Icon"

#define SAVE_MASK_ENABLED    0x0002

class StartupWorker : public QObject
{
    Q_OBJECT

public:
    explicit StartupWorker(QObject *parent = 0);
    ~StartupWorker();
    void addStartupInfo(const QString &desktopFile);
    bool isExecContains(const QString &exec);
    QList<StartupData> getStartupInfoList() const;
    StartupData getStartupInfo(const QString &exec);

private:
    QMap<QString, StartupData> m_startupInfoList;
};

#endif // STARTUPWORKER_H

