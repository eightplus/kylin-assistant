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

#include "filesystemdialog.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QEvent>
#include "diskitemlist.h"
#include "diskitem.h"
#include "diskmodel.h"
#include "diskinfo.h"
#include "filesystemworker.h"

FileSystemDialog::FileSystemDialog(QWidget *parent)
    :QWidget(parent)
    ,m_diskItemList(new DiskItemList)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setAcceptDrops(true);
    setAttribute(Qt::WA_NoMousePropagation);
//    setAttribute(Qt::WA_TranslucentBackground);

    this->setObjectName("FileSystemDialog");

    m_centralLayout = new QVBoxLayout;
    m_centralLayout->addWidget(m_diskItemList);
    m_centralLayout->setSpacing(10);
    m_centralLayout->setMargin(0);

    setLayout(m_centralLayout);


    m_diskModelList = new DiskModel;
    connect(m_diskModelList, SIGNAL(oneDiskInfoAdded(DiskInfo*)), this, SLOT(addDiskInfoItem(DiskInfo*)));
    connect(m_diskModelList, SIGNAL(oneDiskInfoRemoved(DiskInfo*)), this, SLOT(removeDiskInfoItemByDevName(DiskInfo*)));
    m_fileSystemWorker = new FileSystemWorker(m_diskModelList);

    m_fileSystemWorker->moveToThread(qApp->thread());
    m_diskModelList->moveToThread(qApp->thread());
}

FileSystemDialog::~FileSystemDialog()
{
    m_diskModelList->deleteLater();
    m_fileSystemWorker->deleteLater();

    QList<DiskItem *> items = findChildren<DiskItem*>();
    for (DiskItem *item : items) {
        m_diskItemList->removeItem(item);
        item->deleteLater();
    }
    if (m_diskItemList) {
        delete m_diskItemList;
        m_diskItemList = 0;
    }
}

void FileSystemDialog::addDiskInfoItem(DiskInfo *info)
{
    DiskItem *w = new DiskItem;
    m_diskItemList->appendItem(w);
    w->setDevName(info->devname());
    w->setMountDir(info->mountdir());
    w->setDiskType(info->disktype());
    w->setTotalCapacity(info->totalcapacity());
    w->setAvailCapacity(info->availcapacity());
    w->setUsedCapactiy(info->usedcapactiy());
    w->setPercentage(info->percentage());
}

void FileSystemDialog::removeDiskInfoItemByDevName(DiskInfo *info)
{
    QList<DiskItem *> items = findChildren<DiskItem*>();
    for (DiskItem *item : items) {
        if (item->devName() == info->devname()) {
            m_diskItemList->removeItem(item);
            item->deleteLater();
            break;
        }
    }
}

bool FileSystemDialog::event(QEvent *event)
{
    if (event->type() == QEvent::LayoutRequest)
        setFixedHeight(m_centralLayout->sizeHint().height());

    return QWidget::event(event);
}
