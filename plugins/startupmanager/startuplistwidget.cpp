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

#include "startuplistwidget.h"
#include "startupitem.h"

#include <QDebug>
#include <QLabel>
#include <QListWidgetItem>
#include <QScrollBar>
#include <QDirIterator>
#include <QStandardPaths>
#include <QLibraryInfo>
#include <QApplication>
#include <fstream>
#include <qdiriterator.h>
#include <sstream>
#include <stdio.h>
#include <string>

#include <glib-object.h>
#include <glib.h>

#include "startupworker.h"

using namespace std;

///usr/share/gnome-session/sessions/ubuntu.session


//static void _gsp_ensure_user_autostart_dir (void)
//{
//        char *dir;

//        dir = g_build_filename (g_get_user_config_dir (), "autostart", NULL);
//        g_mkdir_with_parents (dir, S_IRWXU);

//        g_free (dir);
//}

//static inline void _gsp_app_save_done_success (GspApp *app)
//{
//        app->priv->save_mask = 0;

//        if (app->priv->old_system_path) {
//                g_free (app->priv->old_system_path);
//                app->priv->old_system_path = NULL;
//        }
//}

//static gboolean _gsp_app_user_equal_system (GspApp  *app,
//                            char   **system_path)
//{
//        GspAppManager *manager;
//        const char    *system_dir;
//        char          *path;
//        char          *str;
//        GKeyFile      *keyfile;

//        manager = gsp_app_manager_get ();
//        system_dir = gsp_app_manager_get_dir (manager,
//                                              app->priv->xdg_system_position);
//        g_object_unref (manager);
//        if (!system_dir) {
//                return FALSE;
//        }

//        path = g_build_filename (system_dir, app->priv->basename, NULL);

//        keyfile = g_key_file_new ();
//        if (!g_key_file_load_from_file (keyfile, path, G_KEY_FILE_NONE, NULL)) {
//                g_free (path);
//                g_key_file_free (keyfile);
//                return FALSE;
//        }
//        if (gsp_key_file_get_boolean (keyfile,
//                                      G_KEY_FILE_DESKTOP_KEY_HIDDEN,
//                                      FALSE) != app->priv->hidden ||
//            gsp_key_file_get_boolean (keyfile,
//                                      GSP_KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLED,
//                                      TRUE) != app->priv->enabled ||
//            gsp_key_file_get_shown (keyfile,
//                                    _gsp_get_current_desktop ()) != app->priv->shown) {
//                g_free (path);
//                g_key_file_free (keyfile);
//                return FALSE;
//        }

//        if (gsp_key_file_get_boolean (keyfile,
//                                      G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY,
//                                      FALSE) != app->priv->no_display) {
//                g_free (path);
//                g_key_file_free (keyfile);
//                return FALSE;
//        }

//        str = gsp_key_file_get_locale_string (keyfile,
//                                              G_KEY_FILE_DESKTOP_KEY_NAME);
//        if (!_gsp_str_equal (str, app->priv->name)) {
//                g_free (str);
//                g_free (path);
//                g_key_file_free (keyfile);
//                return FALSE;
//        }
//        g_free (str);
//        str = gsp_key_file_get_locale_string (keyfile,
//                                              G_KEY_FILE_DESKTOP_KEY_COMMENT);
//        if (!_gsp_str_equal (str, app->priv->comment)) {
//                g_free (str);
//                g_free (path);
//                g_key_file_free (keyfile);
//                return FALSE;
//        }
//        g_free (str);

//        str = gsp_key_file_get_string (keyfile,
//                                       G_KEY_FILE_DESKTOP_KEY_EXEC);
//        if (!_gsp_str_equal (str, app->priv->exec)) {
//                g_free (str);
//                g_free (path);
//                g_key_file_free (keyfile);
//                return FALSE;
//        }
//        g_free (str);

//        str = gsp_key_file_get_locale_string (keyfile,
//                                              G_KEY_FILE_DESKTOP_KEY_ICON);
//        if (!_gsp_str_equal (str, app->priv->icon)) {
//                g_free (str);
//                g_free (path);
//                g_key_file_free (keyfile);
//                return FALSE;
//        }
//        g_free (str);

//        g_key_file_free (keyfile);

//        *system_path = path;

//        return TRUE;
//}


//static gboolean
//_gsp_app_save (gpointer data)
//{
//        GspApp   *app;
//        char     *use_path;
//        GKeyFile *keyfile;
//        GError   *error;

//        app = GSP_APP (data);

//        /* first check if removing the data from the user dir and using the
//         * data from the system dir is enough -- this helps us keep clean the
//         * user config dir by removing unneeded files */
//        if (_gsp_app_user_equal_system (app, &use_path)) {
//                if (g_file_test (app->priv->path, G_FILE_TEST_EXISTS)) {
//                        g_remove (app->priv->path);
//                }

//                g_free (app->priv->path);
//                app->priv->path = use_path;

//                app->priv->xdg_position = app->priv->xdg_system_position;

//                _gsp_app_save_done_success (app);
//                return FALSE;
//        }

//        if (app->priv->old_system_path)
//                use_path = app->priv->old_system_path;
//        else
//                use_path = app->priv->path;

//        keyfile = g_key_file_new ();

//        error = NULL;
//        g_key_file_load_from_file (keyfile, use_path,
//                                   G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS,
//                                   &error);

//        if (error) {
//                g_error_free (error);
//                gsp_key_file_populate (keyfile);
//        }

//        if (app->priv->save_mask & GSP_ASP_SAVE_MASK_HIDDEN) {
//                gsp_key_file_set_boolean (keyfile,
//                                          G_KEY_FILE_DESKTOP_KEY_HIDDEN,
//                                          app->priv->hidden);
//        }
//        if (app->priv->save_mask & GSP_ASP_SAVE_MASK_NO_DISPLAY) {
//                gsp_key_file_set_boolean (keyfile,
//                                          G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY,
//                                          app->priv->no_display);
//        }

//        if (app->priv->save_mask & GSP_ASP_SAVE_MASK_ENABLED) {
//                gsp_key_file_set_boolean (keyfile,
//                                          GSP_KEY_FILE_DESKTOP_KEY_AUTOSTART_ENABLED,
//                                          app->priv->enabled);
//        }

//        if (app->priv->save_mask & GSP_ASP_SAVE_MASK_NAME) {
//                gsp_key_file_set_locale_string (keyfile,
//                                                G_KEY_FILE_DESKTOP_KEY_NAME,
//                                                app->priv->name);
//                gsp_key_file_ensure_C_key (keyfile, G_KEY_FILE_DESKTOP_KEY_NAME);
//        }

//        if (app->priv->save_mask & GSP_ASP_SAVE_MASK_COMMENT) {
//                gsp_key_file_set_locale_string (keyfile,
//                                                G_KEY_FILE_DESKTOP_KEY_COMMENT,
//                                                app->priv->comment);
//                gsp_key_file_ensure_C_key (keyfile, G_KEY_FILE_DESKTOP_KEY_COMMENT);
//        }

//        if (app->priv->save_mask & GSP_ASP_SAVE_MASK_EXEC) {
//                gsp_key_file_set_string (keyfile,
//                                         G_KEY_FILE_DESKTOP_KEY_EXEC,
//                                         app->priv->exec);
//        }
//        _gsp_ensure_user_autostart_dir ();
//        if (gsp_key_file_to_file (keyfile, app->priv->path, NULL)) {
//                app->priv->skip_next_monitor_event = TRUE;
//                _gsp_app_save_done_success (app);
//        } else {
//                g_warning ("Could not save %s file", app->priv->path);
//        }

//        g_key_file_free (keyfile);

//        app->priv->save_timeout = 0;
//        return FALSE;
//}

//static void
//_gsp_app_queue_save (StartupData info/*GspApp *app*/)
//{
//    /* if the file was not in the user directory, then we'll create a copy
//     * there */
//    if (info.xdg_position != 0) {
//            info.xdg_position = 0;

//            if (info.old_system_path.isEmpty()) {
//                    info.old_system_path = info.path;
//                    /* if old_system_path was not NULL, then it means we
//                     * tried to save and we failed; in that case, we want
//                     * to try again and use the old file as a basis again */
//            }

//            info.path = g_build_filename (g_get_user_config_dir (),
//                                                "autostart",
//                                                info.basename, NULL);
//    }


////        if (app->priv->save_timeout) {
////                g_source_remove (app->priv->save_timeout);
////                app->priv->save_timeout = 0;
////        }

////        /* if the file was not in the user directory, then we'll create a copy
////         * there */
////        if (app->priv->xdg_position != 0) {
////                app->priv->xdg_position = 0;

////                if (app->priv->old_system_path == NULL) {
////                        app->priv->old_system_path = app->priv->path;
////                        /* if old_system_path was not NULL, then it means we
////                         * tried to save and we failed; in that case, we want
////                         * to try again and use the old file as a basis again */
////                }

////                app->priv->path = g_build_filename (g_get_user_config_dir (),
////                                                    "autostart",
////                                                    app->priv->basename, NULL);
////        }

////        app->priv->save_timeout = g_timeout_add_seconds (GSP_APP_SAVE_DELAY,
////                                                         _gsp_app_save,
////                                                         app);
//}

//void
//gsp_app_delete (GspApp *app)
//{
//        g_return_if_fail (GSP_IS_APP (app));

//        if (app->priv->xdg_position == 0 &&
//            app->priv->xdg_system_position == G_MAXUINT) {
//                /* exists in user directory only */
//                if (app->priv->save_timeout) {
//                        g_source_remove (app->priv->save_timeout);
//                        app->priv->save_timeout = 0;
//                }

//                if (g_file_test (app->priv->path, G_FILE_TEST_EXISTS)) {
//                        g_remove (app->priv->path);
//                }

//                /* for extra safety */
//                app->priv->hidden = TRUE;
//                app->priv->save_mask |= GSP_ASP_SAVE_MASK_HIDDEN;

//                _gsp_app_emit_removed (app);
//        } else {
//                /* also exists in system directory, so we have to keep a file
//                 * in the user directory */
//                app->priv->hidden = TRUE;
//                app->priv->save_mask |= GSP_ASP_SAVE_MASK_HIDDEN;

//                _gsp_app_queue_save (app);
////                _gsp_app_emit_changed (app);
//        }
//}

std::string make_string(char *c_str)
{
    if (!c_str) {
        return string();
    }
    string s(c_str);
    g_free(c_str);
    return s;
}

inline QStringList autoStartDirectorys()
{
    QStringList dirList;
    const gchar *config_dir = g_get_user_config_dir();
    std::string formatted_result(make_string(g_strdup(config_dir)));//std::string formatted_result = make_string(g_strdup(config_dir));
    dirList.append(QString::fromStdString(formatted_result));

    const char * const *system_config_dirs;
    const char * const *system_data_dirs;
    int i;
    system_data_dirs = g_get_system_data_dirs();
    for (i = 0; system_data_dirs[i]; i++) {
        std::string formatted_result(make_string(g_strdup(system_data_dirs[i])));
        QString dirPath = QString::fromStdString(formatted_result);
        if (dirPath.endsWith("/"))
            dirPath = QString("%1gnome/autostart").arg(dirPath);
        else
            dirPath = QString("%1/gnome/autostart").arg(dirPath);
        if (!dirList.contains(dirPath) && QDir(dirPath).exists())
            dirList.append(dirPath);
    }

    system_config_dirs = g_get_system_config_dirs();
    for (i = 0; system_config_dirs[i]; i++) {
        std::string formatted_result(make_string(g_strdup(system_config_dirs[i])));
        QString dirPath = QString::fromStdString(formatted_result);
        if (dirPath.endsWith("/"))
            dirPath = dirPath + "autostart";
        else
            dirPath = dirPath + QLatin1Char('/') + "autostart";
        if (!dirList.contains(dirPath) && QDir(dirPath).exists())
            dirList.append(dirPath);
    }

    return dirList;
}


StartupListWidget::StartupListWidget(QWidget *parent) : QListWidget(parent)
{
//    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setFixedWidth(parent->width());
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, &StartupListWidget::itemDoubleClicked, [=] (QListWidgetItem *item) {
            StartupItem *fileItem = static_cast<StartupItem *>(itemWidget(item));
            qDebug() << fileItem->getAppName();
        });


    m_startupWorker = new StartupWorker;
    m_startupWorker->moveToThread(qApp->thread());

    QStringList autoDir = autoStartDirectorys();
    foreach (auto dir, autoDir) {
//        qDebug() << "dir="<<dir;
        listAllDesktopFileFromDir(dir);
    }

//    QList<FileSystemListItem*> items;
    this->clear();
    for (StartupData info : m_startupWorker->getStartupInfoList()) {
        loadItem(info);
    }
    this->verticalScrollBar()->setValue(0);

//    qDebug()<<"GenericDataLocation=" << QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
//    qDebug()<<"ConfigLocation=" << QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
//    qDebug()<<"DocumentsLocation1=" << QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
//    qDebug()<<"DocumentsLocation2=" << QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
//    qDebug()<<"PicturesLocation=" << QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
//    QStringList directory = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
//    qDebug()<<"HomeLocation=" <<directory;
//    qDebug()<<"DataLocation=" << QStandardPaths::standardLocations(QStandardPaths::DataLocation);
//    qDebug()<<"DesktopLocation=" << QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
//    qDebug()<<"TempLocation=" << QStandardPaths::standardLocations(QStandardPaths::TempLocation);
//    qDebug()<<"GenericCacheLocation=" << QStandardPaths::standardLocations(QStandardPaths::GenericCacheLocation);
//    qDebug()<<"RuntimeLocation=" << QStandardPaths::standardLocations(QStandardPaths::RuntimeLocation);
//    qDebug()<<"CacheLocation=" << QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
//    qDebug()<<"ApplicationsLocation=" << QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
//    qDebug()<<"configPath=" << QDir(QDir(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first()).filePath(qApp->organizationName())).filePath(qApp->applicationName());
//    qDebug()<<"AppDataLocation=" << QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
//    const QString settingsPath = QLibraryInfo::location(QLibraryInfo::SettingsPath);
//    if (!settingsPath.isEmpty()) { // SettingsPath is empty unless specified in qt.conf.
////        const QFileInfo fi(settingsPath + QLatin1Char('/') + fileName);
////        if (fi.isFile())
////            qDebug() << fi.absoluteFilePath();
//        qDebug() << "settingsPath="<<settingsPath;///etc/xdg
//    }
//    qDebug() << QStandardPaths::locate(QStandardPaths::ConfigLocation, fileName);
}

StartupListWidget::~StartupListWidget()
{
    m_startupWorker->deleteLater();

    for(int i = 0; i < this->count(); i++) {
        QListWidgetItem *listItem = item(i);
        StartupItem *item = static_cast<StartupItem *>(itemWidget(listItem));
        if (item) {
            item->deleteLater();
        }
    }
}

void StartupListWidget::listAllDesktopFileFromDir(QString directory)
{
    QDirIterator dir(directory, QDirIterator::Subdirectories);

    while(dir.hasNext()) {
        if (dir.fileInfo().suffix() == "desktop") {
            QString desktopFile = dir.filePath();//dir.fileName().toLower()
//            qDebug() << "desktopFile="<<desktopFile;
            m_startupWorker->addStartupInfo(desktopFile);
        }
        dir.next();
    }
}

//gsp_app_set_enabled (gboolean enabled)
void StartupListWidget::setAppAutoStartup(StartupData info, bool enabled)
{
    if (info.enabled == enabled)
        return;

    info.enabled = enabled;
    info.save_mask |= SAVE_MASK_ENABLED;

//    _gsp_app_queue_save (info);


//    static void
//    _app_changed (GsmPropertiesDialog *dialog,
//                  GspApp              *app)
//    {
//            GtkTreeIter iter;

//            if (!find_by_app (GTK_TREE_MODEL (dialog->priv->list_store),
//                              &iter, app)) {
//                    return;
//            }

//            _fill_iter_from_app (dialog->priv->list_store, &iter, app);
//    }

//            _gsp_app_emit_changed (app);//g_signal_emit (G_OBJECT (app), gsp_app_signals[CHANGED], 0);


}

void StartupListWidget::loadItem(StartupData info)
{
    StartupItem *item = new StartupItem(info);
    connect(item, SIGNAL(changeStartup(QString,bool)), this, SLOT(onChangeStartup(QString,bool)));
    connect(item, SIGNAL(enter()), this, SLOT(onMouseEnter()));
    addItem(item->getItem());
    item->getItem()->setSizeHint(QSize(this->width() - 10, 60));
    setItemWidget(item->getItem(), item);
}

void StartupListWidget::loadItems(QStringList items, int scrollValue)
{
//    clear();

//    foreach (auto item, items) {
//        loadItem(item);
//    }

//    this->verticalScrollBar()->setValue(scrollValue);
}

void StartupListWidget::onChangeStartup(const QString &exec, bool active)
{
//    QString appName = ((StartupItem*) sender())->getAppName();
    StartupData data = m_startupWorker->getStartupInfo(exec);
    this->setAppAutoStartup(data, active);
}

void StartupListWidget::onMouseEnter()
{
    for(int i = 0; i < this->count(); i++) {
        QListWidgetItem *listItem = item(i);
        StartupItem *item = static_cast<StartupItem *>(itemWidget(listItem));
        if (item->getAppName() == ((StartupItem*) sender())->getAppName()) {
            item->setItemHovered();
        } else {
            item->unsetItemHovered();
        }
    }
}
