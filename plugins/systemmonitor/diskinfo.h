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

