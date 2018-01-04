/*
 * Copyright (C) 2013 ~ 2015 National University of Defense Technology(NUDT) & Kylin Ltd.
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
#include <QApplication>
#include <QIcon>
#include <glib/gi18n.h>
#include <glib.h>
#include <glibtop/procstate.h>
#include "util.h"
#include <QDirIterator>
#include <fstream>
#include <sstream>
#include <qdiriterator.h>


static inline unsigned divide(unsigned *q, unsigned *r, unsigned d)
{
    *q = *r / d;
    *r = *r % d;
    return *q != 0;
}

QString formatDurationForDisplay(unsigned centiseconds)
{
    unsigned weeks = 0, days = 0, hours = 0, minutes = 0, seconds = 0;

    (void)(divide(&seconds, &centiseconds, 100)
           && divide(&minutes, &seconds, 60)
           && divide(&hours, &minutes, 60)
           && divide(&days, &hours, 24)
           && divide(&weeks, &days, 7));

    QString formatTime;
    gchar *duration = NULL;

    if (weeks) {
        /* xgettext: weeks, days */
        duration = g_strdup_printf("%uw%ud", weeks, days);
        formatTime = QString(QLatin1String(duration));
        if (duration) {
            g_free(duration);
            duration = NULL;
        }
        return formatTime;
    }

    if (days) {
        /* xgettext: days, hours (0 -> 23) */
        duration = g_strdup_printf("%ud%02uh", days, hours);
        formatTime = QString(QLatin1String(duration));
        if (duration) {
            g_free(duration);
            duration = NULL;
        }
        return formatTime;
    }

    if (hours) {
        /* xgettext: hours (0 -> 23), minutes, seconds */
        duration = g_strdup_printf("%u:%02u:%02u", hours, minutes, seconds);
        formatTime = QString(QLatin1String(duration));
        if (duration) {
            g_free(duration);
            duration = NULL;
        }
        return formatTime;
    }

    /* xgettext: minutes, seconds, centiseconds */
    duration = g_strdup_printf("%u:%02u.%02u", minutes, seconds, centiseconds);
    formatTime = QString(QLatin1String(duration));
    if (duration) {
        g_free(duration);
        duration = NULL;
    }
    return formatTime;
}

std::string getDesktopFileFromName(int pid, QString procName, QString cmdline)
{
    QDirIterator dir("/usr/share/applications", QDirIterator::Subdirectories);
    std::string desktopFile;

    // Convert to lower characters.
    QString procname = procName.toLower();

    // Replace "_" instead "-", avoid some applications desktop file can't found, such as, sublime text.
    procname.replace("_", "-");

    // Concat desktop file.
    QString processFilename = procname + ".desktop";

    while(dir.hasNext()) {
        if (dir.fileInfo().suffix() == "desktop") {
            if (dir.fileName().toLower().contains(processFilename)) {
                desktopFile = dir.filePath().toStdString();
                break;
            }
        }
        dir.next();
    }

    return desktopFile;
}

QPixmap getDesktopFileIcon(std::string desktopFile, int iconSize)
{
    std::ifstream in;
    in.open(desktopFile);
    QIcon defaultExecutableIcon = QIcon::fromTheme("application-x-executable");
    QIcon icon;
    QString iconName;
    while(!in.eof()) {
        std::string line;
        std::getline(in,line);
        iconName = QString::fromStdString(line);

        if (iconName.startsWith("Icon=")) {
            iconName.remove(0,5); // remove the first 5 chars
        } else {
            continue;
        }

        if (iconName.contains("/")) {
            // this is probably a path to the file, use that instead of the theme icon name
            icon = QIcon(iconName);
        } else {
            icon = QIcon::fromTheme(iconName, defaultExecutableIcon);
            break;
        }
    }
    in.close();

    qreal devicePixelRatio = qApp->devicePixelRatio();

    QPixmap pixmap = icon.pixmap(iconSize * devicePixelRatio, iconSize * devicePixelRatio);
    pixmap.setDevicePixelRatio(devicePixelRatio);

    return pixmap;
}


QString getDisplayNameFromName(QString procName, std::string desktopFile)
{
    if (desktopFile.size() == 0) {
        return procName;
    }

    std::ifstream in;
    in.open(desktopFile);
    QString displayName = procName;
    while(!in.eof()) {
        std::string line;
        std::getline(in,line);

        QString lineContent = QString::fromStdString(line);

        QString localNameFlag = QString("Name[%1]=").arg(QLocale::system().name());
        QString nameFlag = "Name=";
        QString genericNameFlag = QString("GenericName[%1]=").arg(QLocale::system().name());

        if (lineContent.startsWith(localNameFlag)) {
            displayName = lineContent.remove(0, localNameFlag.size());

            break;
        } else if (lineContent.startsWith(genericNameFlag)) {
            displayName = lineContent.remove(0, genericNameFlag.size());

            break;
        } else if (lineContent.startsWith(nameFlag)) {
            displayName = lineContent.remove(0, nameFlag.size());

            continue;
        } else {
            continue;
        }
    }
    in.close();

    return displayName;
}

QString getImagePath(QString imageName)
{
    QDir dir(qApp->applicationDirPath());
    dir.cdUp();

    return QDir(dir.filePath("image")).filePath(imageName);
}


QString formatProcessState(guint state)
{
    QString status;
    switch (state)
    {
        case GLIBTOP_PROCESS_RUNNING:
            status = QString(QObject::tr("Running"));//运行中
            break;

        case GLIBTOP_PROCESS_STOPPED:
            status = QString(QObject::tr("Stopped"));//已停止
            break;

        case GLIBTOP_PROCESS_ZOMBIE:
            status = QString(QObject::tr("Zombie"));//僵死
            break;

        case GLIBTOP_PROCESS_UNINTERRUPTIBLE:
            status = QString(QObject::tr("Uninterruptible"));//不可中断
            break;

        default:
            status = QString(QObject::tr("Sleeping"));//睡眠中
            break;
    }

    return status;
}

QString getNiceLevel(int nice)
{
    if (nice < -7)
        return QObject::tr("Very High");
    else if (nice < -2)
        return QObject::tr("High");
    else if (nice < 3)
        return QObject::tr("Normal");
    else if (nice < 7)
        return QObject::tr("Low");
    else
        return QObject::tr("Very Low");
}

void setFontSize(QPainter &painter, int textSize)
{
    QFont font = painter.font() ;
    font.setPointSize(textSize);
    painter.setFont(font);
}

QString formatUnitSize(double v, const char** orders, int nb_orders)
{
    int order = 0;
    while (v >= 1024 && order + 1 < nb_orders) {
        order++;
        v  = v/1024;
    }
    char buffer1[30];
    snprintf(buffer1, sizeof(buffer1), "%.1lf %s", v, orders[order]);

    return QString(buffer1);
}

QString formatByteCount(double v)
{
    static const char* orders[] = { "B", "KB", "MB", "GB", "TB" };

    return formatUnitSize(v, orders, sizeof(orders)/sizeof(orders[0]));
}

