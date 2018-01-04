#ifndef FILESYSTEMWORKER_H
#define FILESYSTEMWORKER_H

#include <QObject>
#include "diskinfo.h"
#include "diskmodel.h"

class FileSystemWorker : public QObject
{
    Q_OBJECT

public:
    explicit FileSystemWorker(DiskModel *diskList, QObject *parent = 0);

    void removeDiskItem(const QString &devname);

public slots:
    void onFileSystemListChanged();
    void removeUserByName(const QString &name);


private:
    DiskModel *m_diskModel;
};

#endif // FILESYSTEMWORKER_H

