#ifndef PROCESSLISTITEM_H
#define PROCESSLISTITEM_H

#include <QObject>
#include <QPainter>
#include <QPen>
#include "processdata.h"

class ProcessListItem : public QObject
{
    Q_OBJECT
    
public:
    ProcessListItem(ProcData info);
    
    bool isSameItem(ProcessListItem *item);
    void drawBackground(QRect rect, QPainter *painter, int index, bool isSelect);
    void drawForeground(QRect rect, QPainter *painter, int column, int index, bool isSelect);
    static bool doSearch(const ProcessListItem *item, QString text);
    static bool sortByName(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort);
    static bool sortByUser(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort);
    static bool sortByStatus(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort);
    static bool sortByCPU(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort);
    static bool sortByPid(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort);
    static bool sortByCommand(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort);
    static bool sortByMemory(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort);
    static bool sortByPriority(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort);

    QString getProcessName() const;
    QString getDisplayName() const;
    QString getUser() const;
    double getCPU() const;
    pid_t getPid() const;
    long getMemory() const;
    QString getStatus() const;
    long getNice() const;
    QString getCommandLine() const;
    
private:
    ProcData m_data;
    int iconSize;
    int padding;
    int textPadding;
};

#endif // PROCESSLISTITEM_H
