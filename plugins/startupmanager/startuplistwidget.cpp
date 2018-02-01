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

// /usr/share/gnome-session/sessions/ubuntu.session

inline QStringList autoStartupDirectorys()
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


    //gsp_app_manager_fill
    QStringList autoDir = autoStartupDirectorys();
    int i = 0;
    /*QList<QString>::Iterator it = autoDir.begin(), itend = autoDir.end();
    for(;it != itend; it++,i++) {
        qDebug() << *it;
    }*/
    foreach (auto dir, autoDir) {
//        qDebug() << "dir="<<dir;
        GspXdgDir xdgDir;
        if (m_startupWorker->getDirIndex(dir) >= 0) {
            i++;
            continue;
        }
        xdgDir.dir = dir;
        xdgDir.index = i;
        i++;
        m_startupWorker->AddDirWithIndex(xdgDir);//append dir
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
    for (StartupData info : m_startupWorker->getStartupInfoList()) {
        //show or hide
        if (!info.hidden && info.shown && !info.no_display) {
            //show
            loadItem(info);
        }
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

//_gsp_app_manager_fill_from_dir
void StartupListWidget::listAllDesktopFileFromDir(GspXdgDir xdgDir)
{
    QDirIterator dir(xdgDir.dir, QDirIterator::Subdirectories);

    while(dir.hasNext()) {
        if (dir.fileInfo().suffix() == "desktop") {
            QString desktopFile = dir.filePath();//dir.fileName().toLower()
//            qDebug() << "desktopFile="<<desktopFile;
            m_startupWorker->newStartupInfo(desktopFile, xdgDir.index);
        }
        dir.next();
    }
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
