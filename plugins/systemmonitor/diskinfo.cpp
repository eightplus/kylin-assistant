#include "diskinfo.h"
#include <QDebug>

DiskInfo::DiskInfo(QObject *parent)
    : QObject(parent)
{
}

const QString DiskInfo::devname() const
{
    return m_devName;
}

void DiskInfo::setDevName(const QString &name)
{
    if (name != m_devName)
        m_devName = name;
}

const QString DiskInfo::mountdir() const
{
    return m_mountDir;
}

const QString DiskInfo::disktype() const
{
    return m_diskType;
}

const QString DiskInfo::totalcapacity() const
{
    return m_totalCapacity;
}

const QString DiskInfo::availcapacity() const
{
    return m_availCapacity;
}

const QString DiskInfo::usedcapactiy() const
{
    return m_usedCapactiy;
}

const QString DiskInfo::percentage() const
{
    return m_percentage;
}

void DiskInfo::setOtherDiskInfo(QString mountDir, QString diskType, QString totalCapacity, QString availCapacity, QString usedCapactiy, QString percentage)
{
    if (mountDir != m_mountDir)
        m_mountDir = mountDir;
    if (diskType != m_diskType)
        m_diskType = diskType;
    if (totalCapacity != m_totalCapacity)
        m_totalCapacity = totalCapacity;
    if (availCapacity != m_availCapacity)
        m_availCapacity = availCapacity;
    if (usedCapactiy != m_usedCapactiy)
        m_usedCapactiy = usedCapactiy;
    if (percentage != m_percentage)
        m_percentage = percentage;
}
