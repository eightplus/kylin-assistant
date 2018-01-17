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


#ifndef DISKINFO_H
#define DISKINFO_H

#include <QObject>
#include <QString>

class DiskInfo : public QObject
{
    Q_OBJECT

public:
    explicit DiskInfo(QObject *parent = 0);

    const QString devname() const;
    void setDevName(const QString &name);

    const QString mountdir() const;
    const QString disktype() const;
    const QString totalcapacity() const;
    const QString availcapacity() const;
    const QString usedcapactiy() const;
    const QString percentage() const;
    void setOtherDiskInfo(QString mountDir, QString diskType, QString totalCapacity,  QString availCapacity, QString usedCapactiy, QString percentage);

private:
    QString m_devName;
    QString m_mountDir;
    QString m_diskType;
    QString m_totalCapacity;
    QString m_availCapacity;
    QString m_usedCapactiy;
    QString m_percentage;
};


#endif // DISKINFO_H

