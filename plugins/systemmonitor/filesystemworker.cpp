#include "filesystemworker.h"
#include "diskinfo.h"

#include <stddef.h>
#include <glibtop/mountlist.h>
#include <glibtop/fsusage.h>
/*For PRIu64*/
#include <inttypes.h>
//#include <gio/gio.h>

typedef struct _DISK_INFO
{
    char devname[256];
    char mountdir[256];
    char type[256];
    gint percentage;
    guint64 btotal;
    guint64 bavail;
    guint64 bused;
    gint valid;
} DISK_INFO;

static void fsusage_stats(const glibtop_fsusage *buf,
              guint64 *bused, guint64 *bfree, guint64 *bavail, guint64 *btotal,
              gint *percentage)
{
    guint64 total = buf->blocks * buf->block_size;

    if (!total) {
        /* not a real device */
        *btotal = *bfree = *bavail = *bused = 0ULL;
        *percentage = 0;
    } else {
        int percent;
        *btotal = total;
        *bfree = buf->bfree * buf->block_size;
        *bavail = buf->bavail * buf->block_size;
        *bused = *btotal - *bfree;
        /* percent = 100.0f * *bused / *btotal; */
        percent = 100 * *bused / (*bused + *bavail);
        *percentage = CLAMP(percent, 0, 100);
    }
}

DISK_INFO add_disk(const glibtop_mountentry *entry, gboolean show_all_fs)
{
    DISK_INFO disk;
    memset(&disk, 0, sizeof(disk));
    disk.valid = 0;
    glibtop_fsusage usage;
    guint64 bused, bfree, bavail, btotal;
    gint percentage;
    glibtop_get_fsusage(&usage, entry->mountdir);
    if (usage.blocks == 0) {
        return disk;
    }
    if(strcmp(entry->devname,"none")==0 || strcmp(entry->devname,"tmpfs")==0){
        return disk;
    }
    if(strstr(entry->type, "tmpfs")) {
        return disk;
    }
    fsusage_stats(&usage, &bused, &bfree, &bavail, &btotal, &percentage);
    memcpy(disk.devname, entry->devname, strlen(entry->devname));
    memcpy(disk.mountdir, entry->mountdir, strlen(entry->mountdir));
    memcpy(disk.type, entry->type, strlen(entry->type));
    disk.percentage = percentage;
    disk.btotal = btotal;
    disk.bavail = bavail;
    disk.bused = bused;
    disk.valid = 1;
//    qDebug() << disk.devname;//设备
//    qDebug() << disk.mountdir;//目录
//    qDebug() << disk.type;//类型
//    qDebug() << disk.percentage;
//    qDebug() << g_format_size_full(disk.btotal, G_FORMAT_SIZE_DEFAULT);//总数
//    qDebug() << g_format_size_full(disk.bavail, G_FORMAT_SIZE_DEFAULT);//可用
//    qDebug() << g_format_size_full(disk.bused, G_FORMAT_SIZE_DEFAULT);//已用
    return disk;
}

//void hello(gpointer data)
//{
//    g_print ("Hello World\n");
//}

FileSystemWorker::FileSystemWorker(DiskModel *diskList, QObject *parent)
    : QObject(parent),
      m_diskModel(diskList)
{
    onFileSystemListChanged();
//    GVolumeMonitor * monitor;
//    monitor = g_volume_monitor_get();
//    g_signal_connect(monitor, "mount-added", G_CALLBACK(hello), NULL);
}

void FileSystemWorker::onFileSystemListChanged()
{
    glibtop_mountentry *entries;
    glibtop_mountlist mountlist;
    guint i;
    gboolean show_all_fs = TRUE;
    entries = glibtop_get_mountlist(&mountlist, show_all_fs);
    for (i = 0; i < mountlist.number; i++) {
        DISK_INFO disk = add_disk(&entries[i], show_all_fs);
        if (disk.valid == 1) {
            QString dev_name = QString(QLatin1String(disk.devname));
            if (!m_diskModel->contains(dev_name)) {
                DiskInfo *info = new DiskInfo(this);
                info->setDevName(dev_name);
                info->setOtherDiskInfo(QString(QLatin1String(disk.mountdir)), QString(QLatin1String(disk.type)), QString(QLatin1String(g_format_size_full(disk.btotal, G_FORMAT_SIZE_DEFAULT))),  QString(QLatin1String(g_format_size_full(disk.bavail, G_FORMAT_SIZE_DEFAULT))), QString(QLatin1String(g_format_size_full(disk.bused, G_FORMAT_SIZE_DEFAULT))), QString::number(disk.percentage).append("%"));
                m_diskModel->addDiskInfo(dev_name, info);
            }
            else {//update info which had exists
                DiskInfo *info = m_diskModel->getDiskInfo(dev_name);
                if (info) {
                    info->setOtherDiskInfo(QString(QLatin1String(disk.mountdir)), QString(QLatin1String(disk.type)), QString(QLatin1String(g_format_size_full(disk.btotal, G_FORMAT_SIZE_DEFAULT))),  QString(QLatin1String(g_format_size_full(disk.bavail, G_FORMAT_SIZE_DEFAULT))), QString(QLatin1String(g_format_size_full(disk.bused, G_FORMAT_SIZE_DEFAULT))), QString::number(disk.percentage).append("%"));
                }
//                for (DiskInfo *info : m_diskModel->diskInfoList()) {
//                }
            }
        }
    }
    g_free(entries);
}

void FileSystemWorker::removeDiskItem(const QString &devname)
{
    m_diskModel->removeDiskInfo(devname);
}

