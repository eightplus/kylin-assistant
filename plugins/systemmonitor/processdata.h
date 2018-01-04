#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QSharedPointer>

class ProcData
{
public:
    pid_t pid;
    uint cpu;
    long m_memory;
    long m_nice;
    QPixmap iconPixmap;
    QString processName;
    QString displayName;
    QString commandLine;
    QString path;
    QString user;
    QString m_status;
    QString m_session;
    QString cpu_duration_time;
};


typedef QSharedPointer<ProcData>  ProcDataPtr;
typedef QList<ProcDataPtr>  ProcDataPtrList;

Q_DECLARE_METATYPE(ProcData)
Q_DECLARE_METATYPE(ProcDataPtr)
Q_DECLARE_METATYPE(ProcDataPtrList)


#endif // PROCESSDATA_H
