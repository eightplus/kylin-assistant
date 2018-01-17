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


#ifndef DISKITEM_H
#define DISKITEM_H

#include <QFrame>
#include <QMap>
#include <QMapNode>
#include <QLabel>

class MyImageButton;

class DiskItem : public QFrame
{
    Q_OBJECT

public:
    explicit DiskItem(QWidget *parent = 0);
    ~DiskItem();

    inline QString devName() const { return m_nameLabel->text(); }
    void setDevName(const QString &name);
    void setMountDir(const QString &mountdir);
    void setDiskType(const QString &disktype);
    void setTotalCapacity(const QString &totalcapacity);
    void setAvailCapacity(const QString &availcapacity);
    void setUsedCapactiy(const QString &usedcapactiy);
    void setPercentage(const QString &percentage);
    void setIsHead(bool head = true);
    void setIsTail(bool tail = true);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

signals:
    void clicked() const;
    void selected() const;
    void acceptNextPage() const;

private:
    QLabel *m_nameLabel;
    QLabel *m_mountLabel;
    QLabel *m_typeLabel;
    QLabel *m_totoalLabel;
    QLabel *m_availLabel;
    QLabel *m_usedLabel;
    QLabel *m_percentageLabel;
    MyImageButton *m_detailBtn;
    bool m_isHead;
    bool m_isTail;
};

#endif // DISKITEM_H

