/*
 * Copyright (C) 2013 ~ 2018 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
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

#include "processlistitem.h"
#include <QCollator>
#include <QDebug>
#include <QLocale>
#include "util.h"

ProcessListItem::ProcessListItem(ProcData info)
{
    m_data = info;
    iconSize = 24;
    padding = 14;
    textPadding = 5;
}

bool ProcessListItem::isSameItem(ProcessListItem *item)
{
    return m_data.pid == ((static_cast<ProcessListItem*>(item)))->m_data.pid;
}

void ProcessListItem::drawBackground(QRect rect, QPainter *painter, int index, bool isSelect)
{
    QPainterPath path;
    path.addRect(QRectF(rect));

    if (isSelect) {
        painter->setOpacity(1.0);
        painter->fillPath(path, QColor("#3f96e4"));
    }
    else {
        painter->setOpacity(0.02);
        if (index % 2 == 0) {
            painter->fillPath(path, QColor("#000000"));
        } else {
            painter->fillPath(path, QColor("#e9eef0"));
        }
    }
}

void ProcessListItem::drawForeground(QRect rect, QPainter *painter, int column, int, bool isSelect)
{
    setFontSize(*painter, 9);
    painter->setOpacity(1);
    if (isSelect) {
//        //this->setToolTip();//kobe neet to draw tooltip
        painter->setPen(QPen(QColor("#ffffff")));
    } else {
        painter->setPen(QPen(QColor("#000000")));
    }

    // Draw icon and process's name
    if (column == 0) {
        painter->drawPixmap(QRect(rect.x() + padding, rect.y() + (rect.height() - iconSize) / 2, iconSize, iconSize), m_data.iconPixmap);
        QString name = m_data.processName;
        if (m_data.m_status == tr("Stopped")) {//已停止
            painter->setPen(QPen(QColor("#FA7053")));
            name = QString("(%1) %2").arg(tr("Suspend")).arg(m_data.processName);
        }
        else if (m_data.m_status == tr("Zombie")) {//僵死
            painter->setPen(QPen(QColor("#808080")));
            name = QString("(%1) %2").arg(tr("No response")).arg(m_data.processName);

        }
        else if (m_data.m_status == tr("Uninterruptible")) {//不可中断
            painter->setPen(QPen(QColor("#A52A2A")));
            name = QString("(%1) %2").arg(tr("Uninterruptible")).arg(m_data.processName);
        }
        else {//Sleeping 睡眠中  Running 运行中

        }
        int nameMaxWidth = rect.width() - iconSize - padding * 3;
        QFont font = painter->font();
        QFontMetrics fm(font);
        QString procName = fm.elidedText(name, Qt::ElideRight, nameMaxWidth);
        painter->drawText(QRect(rect.x() + iconSize + padding * 2, rect.y(), nameMaxWidth, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, procName);
    }
    // Draw User.
    else if (column == 1) {
        if (!m_data.user.isEmpty()) {
            painter->drawText(QRect(rect.x(), rect.y(), rect.width() - textPadding, rect.height()), Qt::AlignCenter, m_data.user);
        }
    }
    // Draw Status.
    else if (column == 2) {
        if (!m_data.m_status.isEmpty()) {
            painter->drawText(QRect(rect.x(), rect.y(), rect.width() - textPadding, rect.height()), Qt::AlignCenter, m_data.m_status);
        }
    }
    // Draw CPU.
    else if (column == 3) {
        painter->drawText(QRect(rect.x(), rect.y(), rect.width() - textPadding, rect.height()), Qt::AlignCenter, QString("%1%").arg(m_data.cpu));
    }
    // Draw pid.
    else if (column == 4) {
        painter->drawText(QRect(rect.x(), rect.y(), rect.width() - padding, rect.height()), Qt::AlignCenter, QString("%1").arg(m_data.pid));
    }
    // Draw Command.
    else if (column == 5) {
        int commandMaxWidth = rect.width();
        QFont font = painter->font();
        QFontMetrics fm(font);
        QString command = fm.elidedText(m_data.commandLine, Qt::ElideRight, commandMaxWidth);
        painter->drawText(QRect(rect.x(), rect.y(), commandMaxWidth, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, command);
    }
    // Draw memory.
    else if (column == 6) {
        if (m_data.m_memory > 0) {
            QString memory = QString(g_format_size_full(m_data.m_memory, G_FORMAT_SIZE_IEC_UNITS));
            painter->drawText(QRect(rect.x(), rect.y(), rect.width() - textPadding, rect.height()), Qt::AlignCenter, memory);
        }
    }
    // Draw Priority.
    else if (column == 7) {
        painter->drawText(QRect(rect.x(), rect.y(), rect.width() - textPadding, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, getNiceLevel(m_data.m_nice));
    }
}

bool ProcessListItem::doSearch(const ProcessListItem *item, QString text)
{
    const ProcessListItem *procItem = static_cast<const ProcessListItem*>(item);
    QString content = text.toLower();
    return procItem->getProcessName().toLower().contains(content) || QString::number(procItem->getPid()).contains(content) || procItem->getDisplayName().toLower().contains(content) || procItem->getUser().toLower().contains(content);
}

bool ProcessListItem::sortByName(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort)
{
    QString name1 = (static_cast<const ProcessListItem*>(item1))->getDisplayName();
    QString name2 = (static_cast<const ProcessListItem*>(item2))->getDisplayName();
    bool sortOrder;

    // Sort item with cpu if name is same.
    if (name1 == name2) {
        double cpu1 = static_cast<const ProcessListItem*>(item1)->getCPU();
        double cpu2 = (static_cast<const ProcessListItem*>(item2))->getCPU();

        sortOrder = cpu1 > cpu2;
    }
    // Otherwise sort by name.
    else {
        QCollator qco(QLocale::system());
        int result = qco.compare(name1, name2);

        sortOrder = result < 0;
    }

    return descendingSort ? sortOrder : !sortOrder;
}

bool ProcessListItem::sortByUser(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort)
{
    QString user1 = (static_cast<const ProcessListItem*>(item1))->getUser();
    QString user2 = (static_cast<const ProcessListItem*>(item2))->getUser();
    bool sortOrder;

    // Sort item with cpu if user is same.
    if (user1 == user2) {
        double cpu1 = static_cast<const ProcessListItem*>(item1)->getCPU();
        double cpu2 = (static_cast<const ProcessListItem*>(item2))->getCPU();

        sortOrder = cpu1 > cpu2;
    }
    // Otherwise sort by user.
    else {
        QCollator qco(QLocale::system());
        int result = qco.compare(user1, user2);

        sortOrder = result < 0;
    }

    return descendingSort ? sortOrder : !sortOrder;
}

bool ProcessListItem::sortByStatus(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort)
{
    QString status1 = (static_cast<const ProcessListItem*>(item1))->getStatus();
    QString status2 = (static_cast<const ProcessListItem*>(item2))->getStatus();
    bool sortOrder;

    // Sort item with cpu if status is same.
    if (status1 == status2) {
        double cpu1 = static_cast<const ProcessListItem*>(item1)->getCPU();
        double cpu2 = (static_cast<const ProcessListItem*>(item2))->getCPU();

        sortOrder = cpu1 > cpu2;
    }
    // Otherwise sort by status.
    else {
        QCollator qco(QLocale::system());
        int result = qco.compare(status1, status2);

        sortOrder = result < 0;
    }

    return descendingSort ? sortOrder : !sortOrder;
}


bool ProcessListItem::sortByCPU(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort)
{
    double cpu1 = (static_cast<const ProcessListItem*>(item1))->getCPU();
    double cpu2 = (static_cast<const ProcessListItem*>(item2))->getCPU();
    bool sortOrder;

    // Sort item with memory if cpu is same.
    if (cpu1 == cpu2) {
        long memory1 = static_cast<const ProcessListItem*>(item1)->getMemory();
        long memory2 = (static_cast<const ProcessListItem*>(item2))->getMemory();

        sortOrder = memory1 > memory2;
    }
    // Otherwise sort by cpu.
    else {
        sortOrder = cpu1 > cpu2;
    }

    return descendingSort ? sortOrder : !sortOrder;
}

bool ProcessListItem::sortByPid(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort)
{
    bool sortOrder = (static_cast<const ProcessListItem*>(item1))->getPid() > (static_cast<const ProcessListItem*>(item2))->getPid();

    return descendingSort ? sortOrder : !sortOrder;
}

bool ProcessListItem::sortByCommand(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort)
{
    QString command1 = (static_cast<const ProcessListItem*>(item1))->getCommandLine();
    QString command2 = (static_cast<const ProcessListItem*>(item2))->getCommandLine();
    bool sortOrder;

    // Sort item with cpu if command is same.
    if (command1 == command2) {
        double cpu1 = static_cast<const ProcessListItem*>(item1)->getCPU();
        double cpu2 = (static_cast<const ProcessListItem*>(item2))->getCPU();

        sortOrder = cpu1 > cpu2;
    }
    // Otherwise sort by command.
    else {
        QCollator qco(QLocale::system());
        int result = qco.compare(command1, command2);

        sortOrder = result < 0;
    }

    return descendingSort ? sortOrder : !sortOrder;
}

bool ProcessListItem::sortByMemory(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort)
{
    long memory1 = (static_cast<const ProcessListItem*>(item1))->getMemory();
    long memory2 = (static_cast<const ProcessListItem*>(item2))->getMemory();
    bool sortOrder;

    // Sort item with cpu if memory is same.
    if (memory1 == memory2) {
        double cpu1 = static_cast<const ProcessListItem*>(item1)->getCPU();
        double cpu2 = (static_cast<const ProcessListItem*>(item2))->getCPU();

        sortOrder = cpu1 > cpu2;
    }
    // Otherwise sort by memory.
    else {
        sortOrder = memory1 > memory2;
    }

    return descendingSort ? sortOrder : !sortOrder;
}

bool ProcessListItem::sortByPriority(const ProcessListItem *item1, const ProcessListItem *item2, bool descendingSort)
{
    long nice1 = (static_cast<const ProcessListItem*>(item1))->getNice();
    long nice2 = (static_cast<const ProcessListItem*>(item2))->getNice();
    bool sortOrder;

    // Sort item with cpu if nice is same.
    if (nice1 == nice2) {
        double cpu1 = static_cast<const ProcessListItem*>(item1)->getCPU();
        double cpu2 = (static_cast<const ProcessListItem*>(item2))->getCPU();

        sortOrder = cpu1 > cpu2;
    }
    // Otherwise sort by nice.
    else {
        sortOrder = nice1 > nice2;
    }

    return descendingSort ? sortOrder : !sortOrder;
}

QString ProcessListItem::getProcessName() const
{
    return m_data.processName;
}

QString ProcessListItem::getDisplayName() const
{
    return m_data.displayName;
}

QString ProcessListItem::getUser() const
{
    return m_data.user;
}

QString ProcessListItem::getStatus() const
{
    return m_data.m_status;
}

double ProcessListItem::getCPU() const
{
    return m_data.cpu;
}

pid_t ProcessListItem::getPid() const
{
    return m_data.pid;
}

long ProcessListItem::getMemory() const
{
    return m_data.m_memory;
}

long ProcessListItem::getNice() const
{
    return m_data.m_nice;
}

QString ProcessListItem::getCommandLine() const
{
    return m_data.commandLine;
}
