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

#include "startupworker.h"

#include <QSettings>
#include <QDebug>
#include <QFileInfo>

//#include <gio/gio.h>
//static gboolean
//gsp_app_manager_xdg_dir_monitor (GFileMonitor      *monitor,
//                                 GFile             *child,
//                                 GFile             *other_file,
//                                 GFileMonitorEvent  flags,
//                                 gpointer           data)
//{
////        GspAppManager *manager;
////        GspApp        *old_app;
////        GspApp        *app;
//        GFile         *parent;
//        char          *basename;
//        char          *dir;
//        char          *path;
//        int            index;

//        manager = GSP_APP_MANAGER (data);

//        basename = g_file_get_basename (child);
//        if (!g_str_has_suffix (basename, ".desktop")) {
//                /* not a desktop file, we can ignore */
//                g_free (basename);
//                return TRUE;
//        }
//        old_app = gsp_app_manager_find_app_with_basename (manager, basename);

//        parent = g_file_get_parent (child);
//        dir = g_file_get_path (parent);
//        g_object_unref (parent);

//        index = gsp_app_manager_get_dir_index (manager, dir);
//        if (index < 0) {
//                /* not a directory we know; should never happen, though */
//                g_free (dir);
//                return TRUE;
//        }

//        path = g_file_get_path (child);
//        switch (flags) {
//        case G_FILE_MONITOR_EVENT_CHANGED:
//        case G_FILE_MONITOR_EVENT_CREATED:
//                /* we just do as if it was a new file: GspApp is clever enough
//                 * to do the right thing */
//                app = gsp_app_new (path, (unsigned int) index);

//                /* we didn't have this app before, so add it */
//                if (old_app == NULL && app != NULL) {
//                        gsp_app_manager_add (manager, app);
//                        g_object_unref (app);
//                }
//                /* else: it was just updated, GspApp took care of
//                 * sending the event */
//                break;
//        case G_FILE_MONITOR_EVENT_DELETED:
//                if (!old_app) {
//                        /* it got deleted, but we don't know about it, so
//                         * nothing to do */
//                        break;
//                }

//                _gsp_app_manager_handle_delete (manager, old_app,
//                                                basename, index);
//                break;
//        default:
//                break;
//        }

//        g_free (path);
//        g_free (dir);
//        g_free (basename);

//        return TRUE;
//}

//GFileMonitor *monitor;
//monitor = g_file_monitor_directory (file, G_FILE_MONITOR_NONE, NULL, NULL);
//if (monitor) {
//    g_signal_connect (monitor, "changed", G_CALLBACK (gsp_app_manager_xdg_dir_monitor), manager);
//}
//g_file_monitor_cancel (monitor);
//g_object_unref (monitor);
//monitor = NULL;

inline QString getCurrentDesktopEnvironment()
{
    QString current_desktop;

    current_desktop = qgetenv("XDG_CURRENT_DESKTOP");//g_getenv
    if(current_desktop.isEmpty()) {
        current_desktop = qgetenv("XDG_SESSION_DESKTOP");
        if(current_desktop.isEmpty())
            current_desktop = "GNOME";
    }

    return current_desktop;
}

bool getShownFromDesktopFile(const QString &desktopFile, const QString &desktopEnvironment)
{
    if (desktopEnvironment.isNull() || desktopEnvironment.isEmpty())
        return true;

    bool found;
    QSettings setting(desktopFile, QSettings::IniFormat);
    setting.beginGroup(KEY_FILE_DESKTOP_GROUP);
    QStringList onlyShowIn = setting.value(KEY_FILE_DESKTOP_KEY_ONLY_SHOW_IN).toStringList();
    QStringList notShowIn = setting.value(KEY_FILE_DESKTOP_KEY_NOT_SHOW_IN).toStringList();
    setting.endGroup();

    if (!onlyShowIn.isEmpty()) {
        found = false;
        foreach (auto dekstopEnv, onlyShowIn) {
            if (dekstopEnv == desktopEnvironment) {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }

    if (!notShowIn.isEmpty()) {
        found = false;
        foreach (auto dekstopEnv, notShowIn) {
            if (dekstopEnv == desktopEnvironment) {
                found = true;
                break;
            }
        }
        if (found)
            return false;
    }

    return true;
}

StartupWorker::StartupWorker(QObject *parent)
    : QObject(parent)
{

}

StartupWorker::~StartupWorker()
{
    m_startupInfoList.clear();
}

void StartupWorker::addStartupInfo(const QString &desktopFile)
{
    QSettings setting(desktopFile, QSettings::IniFormat);
    setting.beginGroup(KEY_FILE_DESKTOP_GROUP);

    bool hidden = setting.value(KEY_FILE_DESKTOP_KEY_HIDDEN, false).toBool();
    bool no_display = setting.value(KEY_FILE_DESKTOP_KEY_NO_DISPLAY, false).toBool();
    bool enabled = setting.value(KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLED, true).toBool();
    bool shown = getShownFromDesktopFile(desktopFile, getCurrentDesktopEnvironment());
    QString basename = QFileInfo(desktopFile).fileName();
    QString name = setting.value(KEY_FILE_DESKTOP_KEY_NAME).toString();
    QString exec = setting.value(KEY_FILE_DESKTOP_KEY_EXEC).toString();
    QString comment = setting.value(KEY_FILE_DESKTOP_KEY_COMMENT).toString();
    QString icon = setting.value(KEY_FILE_DESKTOP_KEY_ICON).toString();
    if (name.isEmpty() || name.isNull())
        name = exec;
    setting.endGroup();

    //get description
    QString primary;
    QString secondary;
    if (!name.isEmpty()) {
        primary = name;
    } else if (!exec.isEmpty()) {
        primary = exec;
    } else {
        primary = tr("No name");
    }
    if (!comment.isEmpty()) {
        secondary = comment;
    } else {
        secondary = tr("No description");
    }
    QString description = QString("<b>%1</b>\n%2").arg(primary).arg(secondary);

    StartupData info;
    info.basename = basename;
    info.path = desktopFile;
    info.hidden = hidden;
    info.no_display = no_display;
    info.enabled = enabled;
    info.shown = shown;
    info.name = name;
    info.exec = exec;
    info.comment = comment;
    info.icon = icon;
    info.description = description;

    //printf("hidden=%s\n", hidden ? "Yes" : "No");

    //show or hide
    if (!hidden && shown && !no_display) {
        //show
//        if (isExecContains(exec)) {

//        }
        m_startupInfoList[exec] = info;

    }
    else {
        //hide
        if (isExecContains(exec))
            m_startupInfoList.remove(exec);
    }
}

bool StartupWorker::isExecContains(const QString &exec)
{
    return m_startupInfoList.keys().contains(exec);
}

QList<StartupData> StartupWorker::getStartupInfoList() const
{
    return m_startupInfoList.values();
}

StartupData StartupWorker::getStartupInfo(const QString &exec)
{
    return m_startupInfoList.value(exec, StartupData());//nullptr
}
