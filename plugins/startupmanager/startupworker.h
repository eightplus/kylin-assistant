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
#define KEY_FILE_DESKTOP_KEY_TRY_EXEC "TryExec"
#define KEY_FILE_DESKTOP_KEY_COMMENT "Comment"
#define KEY_FILE_DESKTOP_KEY_ICON "Icon"
#define KEY_FILE_DESKTOP_KEY_TYPE "Type"
#define KEY_FILE_DESKTOP_TYPE_APPLICATION "Application"
#define KEY_FILE_DESKTOP_TYPE_LINK "Link"
#define KEY_FILE_DESKTOP_KEY_URL "URL"
#define KEY_FILE_DESKTOP_KEY_STARTUP_NOTIFY "StartupNotify"
#define KEY_FILE_DESKTOP_KEY_CATEGORIES "Categories"
#define KEY_FILE_DESKTOP_KEY_MIME_TYPE "MimeType"
#define KEY_FILE_DESKTOP_KEY_TERMINAL "Terminal"
#define KEY_FILE_DESKTOP_TYPE_DIRECTORY "Directory"


#define SAVE_MASK_HIDDEN     0x0001
#define SAVE_MASK_ENABLED    0x0002
#define SAVE_MASK_NAME       0x0004
#define SAVE_MASK_EXEC       0x0008
#define SAVE_MASK_COMMENT    0x0010
#define SAVE_MASK_NO_DISPLAY 0x0020
#define SAVE_MASK_ALL        0xffff

typedef struct {
        QString dir;
        int index;
//        GFileMonitor *monitor;
} GspXdgDir;

class StartupWorker : public QObject
{
    Q_OBJECT

public:
    explicit StartupWorker(QObject *parent = 0);
    ~StartupWorker();
    void newStartupInfo(const QString &desktopFile, unsigned int xdg_position);
    bool isExecContains(const QString &exec);
    QList<StartupData> getStartupInfoList() const;
    StartupData getStartupInfo(const QString &exec);

//    void gsp_key_file_ensure_C_key(QString filename, QString key, QString locale);

    void updateEnable(const QString &exec, bool enabled);
    void updateSaveMask(const QString &exec, unsigned int save_mask);
    void updateXdgPosition(const QString &exec, unsigned int xdg_position);
    void updateXdgSystemPosition(const QString &exec, unsigned int xdg_system_position);
    void updateOldSystemPath(const QString &exec, QString old_system_path);
    void updatePath(const QString &exec, QString path);

    void AddDirWithIndex(GspXdgDir dir);
    int getDirIndex(QString dir);
    QString gsp_app_manager_get_dir(unsigned int index);
    QList<GspXdgDir> getAllDirs() { return dirs; }

    void _gsp_ensure_user_autostart_dir(void);
    void _gsp_app_save_done_success (StartupData info);
//    bool _gsp_app_user_equal_system (StartupData info, QString &system_path, QString locale);
    bool _gsp_app_user_equal_system (StartupData info, char **system_path);
    bool _gsp_app_save(StartupData info);
    void _gsp_app_queue_save(StartupData info);
    StartupData gsp_app_manager_find_app_with_basename(QString &basename);

private:
    QMap<QString, StartupData> m_startupInfoList;
    QList<GspXdgDir> dirs;
};

#endif // STARTUPWORKER_H

