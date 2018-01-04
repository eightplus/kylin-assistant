#ifndef DISKMODEL_H
#define DISKMODEL_H

#include <QObject>
#include <QMap>

#include "diskinfo.h"

class DiskModel : public QObject
{
    Q_OBJECT
public:
    explicit DiskModel(QObject *parent = 0);
    ~DiskModel();

    DiskInfo *getDiskInfo(const QString &devname);
    QList<DiskInfo *> diskInfoList() const;
    void addDiskInfo(const QString &devname, DiskInfo *info);
    void removeDiskInfo(const QString &devname);
    bool contains(const QString &devname);

signals:
    void oneDiskInfoAdded(DiskInfo *info);
    void oneDiskInfoRemoved(DiskInfo *info);

private:
    QMap<QString, DiskInfo *> m_diskInfoList;
};

#endif // DISKMODEL_H

