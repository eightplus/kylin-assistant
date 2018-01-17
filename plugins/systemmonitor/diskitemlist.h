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


#ifndef DISKITEMLIST_H
#define DISKITEMLIST_H

#include <QFrame>
#include <QTimer>


class QVBoxLayout;

class DiskItem;

class DiskItemList : public QWidget
{
    Q_OBJECT

public:
    explicit DiskItemList(QFrame *parent = 0);
    explicit DiskItemList(const QString &title, QFrame *parent = 0);
    ~DiskItemList();

    DiskItem* getItem(int index);
    void appendItem(DiskItem * item);
    void removeItem(DiskItem * item);
    void moveItem(DiskItem *item, const int index);
    void setSpacing(const int spaceing);

    int itemCount() const;
    void clear();

private:
    bool eventFilter(QObject *, QEvent *event);
    void updateHeadTail();

private slots:
    void updateHeight();

private:
    QVBoxLayout *m_layout;
    QTimer *m_updateHTimer;
    QTimer *m_updateHeadTimer;
};

#endif // DISKITEMLIST_H
