#include "diskmodel.h"

#include <QDebug>

DiskModel::DiskModel(QObject *parent) :
    QObject(parent)
{

}

DiskModel::~DiskModel()
{

}

DiskInfo *DiskModel::getDiskInfo(const QString &devname)
{
    return m_diskInfoList.value(devname, nullptr);
}

QList<DiskInfo *> DiskModel::diskInfoList() const
{
    return m_diskInfoList.values();
}

void DiskModel::addDiskInfo(const QString &devname, DiskInfo *info)
{
    if (!m_diskInfoList.contains(devname)) {
        m_diskInfoList[devname] = info;
        emit oneDiskInfoAdded(info);
    }
}

void DiskModel::removeDiskInfo(const QString &devname)
{
    DiskInfo *info = getDiskInfo(devname);
    m_diskInfoList.remove(devname);

    if (info)
        emit oneDiskInfoRemoved(info);
}

bool DiskModel::contains(const QString &devname)
{
    return m_diskInfoList.contains(devname);
}
