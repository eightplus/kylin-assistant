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

#include <QWidget>

class DiskItemList;
class DiskModel;
class DiskInfo;
class FileSystemWorker;
class QVBoxLayout;

class FileSystemDialog : public QWidget
{
    Q_OBJECT

public:
    explicit FileSystemDialog(QWidget* parent = 0);
    ~FileSystemDialog();

public slots:
    void addDiskInfoItem(DiskInfo *info);
    void removeDiskInfoItemByDevName(DiskInfo *info);

private:
    bool event(QEvent *event);

private:
    QVBoxLayout *m_centralLayout = nullptr;
    DiskItemList *m_diskItemList;
    DiskModel *m_diskModelList;
    FileSystemWorker *m_fileSystemWorker;
};
