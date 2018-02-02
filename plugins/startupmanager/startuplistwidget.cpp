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
#include <QFileSystemWatcher>
#include <fstream>
#include <qdiriterator.h>
#include <sstream>
#include <stdio.h>
#include <string>

#include <glib-object.h>
#include <glib.h>

// /usr/share/gnome-session/sessions/ubuntu.session

inline QStringList autoStartupDirectorys()
{
    QStringList dirList;
    const gchar *config_dir = g_get_user_config_dir();
    std::string formatted_result(make_string(g_strdup(config_dir)));//std::string formatted_result = make_string(g_strdup(config_dir));
    QString userdirPath = QString::fromStdString(formatted_result);
    if (userdirPath.endsWith("/"))
        userdirPath = QString("%1autostart").arg(userdirPath);
    else
        userdirPath = QString("%1/autostart").arg(userdirPath);
    dirList.append(userdirPath);

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

    //this->m_watherList = new QList<QFileSystemWatcher*>();

    m_startupWorker = new StartupWorker;
    m_startupWorker->moveToThread(qApp->thread());
    connect(m_startupWorker, SIGNAL(refreshUI()), this, SLOT(onRefreshUI()), Qt::QueuedConnection);

    //gsp_app_manager_fill
    QStringList autoDir = autoStartupDirectorys();
    int i = 0;
    /*QList<QString>::Iterator it = autoDir.begin(), itend = autoDir.end();
    for(;it != itend; it++,i++) {
        qDebug() << *it;
    }*/
    foreach (auto dir, autoDir) {
        qDebug() << "dir="<<dir;
        GspXdgDir xdgDir;
        if (m_startupWorker->getDirIndex(dir) >= 0) {
            i++;
            continue;
        }
        xdgDir.dir = dir;
        xdgDir.index = i;
        xdgDir.wather = m_startupWorker->createFileSystemMonitor(xdgDir.dir);
        i++;
//        m_startupWorker->appendXdgDirData(xdgDir);//append dir

        listAllDesktopFileFromDir(xdgDir);
//        GspXdgDir *xdgdir;
//        if (gsp_app_manager_get_dir_index (manager, autostart_dirs[i]) >= 0) {
//                continue;
//        }
//        xdgdir = _gsp_xdg_dir_new (autostart_dirs[i], i);
//        manager->priv->dirs = g_slist_prepend (manager->priv->dirs,xdgdir);
//        _gsp_app_manager_fill_from_dir (manager, xdgdir);
    }

    this->clear();

    this->displayAutoStartupItems();
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
    /*qDeleteAll(this->m_watherList->begin(), this->m_watherList->end());
    this->m_watherList->clear();
    delete this->m_watherList;*/

    /*for (int i=0; i<m_watherList.count(); i++) {
        QFileSystemWatcher *wather = m_watherList.at(i);
//        wather->removePath(m_monitorFile);
        delete wather;
        wather = NULL;
    }
    m_watherList.clear();*/

//    QMap<QString,QVariant>::iterator it; //遍历map
//    for (it = m_watherMap.begin(); it != m_watherMap.end(); ++it) {
//        QString path = it.key();
//        QFileSystemWatcher *wather = static_cast<QFileSystemWatcher *>(it.value());
//        wather->removePath(path);
//        delete wather;
//        wather = NULL;
//    }
//    m_watherMap.clear();

    m_startupWorker->deleteLater();

    this->clearUI();
//    this->clear();
}

void StartupListWidget::clearUI()
{
    for(int i = 0; i < this->count(); i++) {
        QListWidgetItem *listItem = item(i);
        StartupItem *item = static_cast<StartupItem *>(itemWidget(listItem));
        if (item) {
            item->deleteLater();
        }
    }
    this->clear();
}

void StartupListWidget::onRefreshUI()
{
    this->clearUI();
//    this->clear();
    this->displayAutoStartupItems();
}

void StartupListWidget::displayAutoStartupItems()
{
    for (StartupData info : m_startupWorker->getStartupInfoList()) {
        //show or hide
        if (!info.hidden && info.shown && !info.no_display) {
            //show
            loadItem(info);
        }
    }
    this->verticalScrollBar()->setValue(0);
}

///*QFileSystemWatcher **/void StartupListWidget::createFileSystemMonitor(const QString &path)
//{
//    /*int fd = inotify_init();
//    int wd = inotify_add_watch (fd, path, mask);
////    int ret = inotify_rm_watch (fd, wd);*/


//    QFileSystemWatcher *m_fileSystemMonitor = new QFileSystemWatcher(this);
//    m_fileSystemMonitor->addPath(path);
////    QFileInfo info(m_monitorFile);
////    m_fileSystemMonitor->addPath(info.absoluteFilePath());

//    connect(m_fileSystemMonitor, &QFileSystemWatcher::directoryChanged, [=] (const QString &path) {
//        qDebug()<< "directoryChanged path===================="<<path;
//    });
////    m_watherList.append(m_fileSystemMonitor);
//    if (m_watherMap.contains(path))
//        m_watherMap.insert(path, m_fileSystemMonitor);

////    return m_fileSystemMonitor;


////    connect(m_fileSystemMonitor, &QFileSystemWatcher::fileChanged, [=] (const QString &path) {
////        qDebug()<< "fileChanged path===================="<<path;
////    });
//}

//static gboolean
//gsp_app_manager_xdg_dir_monitor (GFileMonitor      *monitor,
//                                 GFile             *child,
//                                 GFile             *other_file,
//                                 GFileMonitorEvent  flags,
//                                 gpointer           data)
//{
//        GspAppManager *manager;
//        GspApp        *old_app;
//        GspApp        *app;
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

//_gsp_app_manager_fill_from_dir
void StartupListWidget::listAllDesktopFileFromDir(GspXdgDir xdgDir)
{
    //start to call monitor
//    GFile      *file;
//    GDir       *dir;
//    const char *name;
//    file = g_file_new_for_path (xdgDir.dir.toStdString().data());
//    xdgdir->monitor = g_file_monitor_directory (file, G_FILE_MONITOR_NONE,NULL, NULL);
//    g_object_unref (file);
//    if (xdgdir->monitor) {
//            g_signal_connect (xdgdir->monitor, "changed",G_CALLBACK (gsp_app_manager_xdg_dir_monitor),manager);
//    }

//    m_startupWorker->createFileSystemMonitor(xdgDir.dir);

    xdgDir.fileList.clear();

    QDirIterator dir(xdgDir.dir, QDirIterator::Subdirectories);
    while(dir.hasNext()) {
        if (dir.fileInfo().suffix() == "desktop") {
            QString desktopFile = dir.filePath();//dir.fileName().toLower()
//            qDebug() << "desktopFile="<<desktopFile;
            xdgDir.fileList.append(desktopFile);
            m_startupWorker->newStartupInfo(desktopFile, xdgDir.index);
        }
        dir.next();
    }
    m_startupWorker->appendXdgDirData(xdgDir);//append xdgdir
}

//gsp_app_set_enabled (gboolean enabled)
void StartupListWidget::setAppAutoStartup(/*StartupData info,*/const QString &exec, bool enabled)
{
    StartupData info = m_startupWorker->getStartupInfo(exec);
//    qDebug() << "info.enabled="<<info.enabled << ",enabled="<<enabled;
    if (info.enabled == enabled)
        return;

    info.enabled = enabled;
    info.save_mask |= SAVE_MASK_ENABLED;
    m_startupWorker->updateEnable(info.exec, info.enabled);
    m_startupWorker->updateSaveMask(info.exec, info.save_mask);
    m_startupWorker->_gsp_app_queue_save(info);
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
//    StartupData data = m_startupWorker->getStartupInfo(exec);
    this->setAppAutoStartup(/*data,*/exec, active);
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
