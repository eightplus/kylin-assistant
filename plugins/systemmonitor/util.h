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

#include <glib.h>
#include <iostream>
#include <map>
#include <QString>
#include <QObject>
#include <QPainter>

#define TOP_TITLE_WIDGET_HEIGHT 39

using std::string;

std::string getDesktopFileFromName(int pid, QString procName, QString cmdline);
QPixmap getDesktopFileIcon(std::string desktopFile, int iconSize = 24);
QString getDisplayNameFromName(QString procName, std::string desktopFile);
QString getImagePath(QString imageName);

inline string make_string(char *c_str)
{
    if (!c_str) {
        return string();
    }
    string s(c_str);
    g_free(c_str);
    return s;
}

QString formatProcessState(guint state);
QString getNiceLevel(int nice);
QString formatUnitSize(double v, const char** orders, int nb_orders);
QString formatByteCount(double v);
void setFontSize(QPainter &painter, int textSize);
QString formatDurationForDisplay(unsigned centiseconds);
