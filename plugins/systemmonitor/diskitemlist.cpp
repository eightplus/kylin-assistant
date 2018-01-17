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

#include "diskitemlist.h"
#include "diskitem.h"

#include <QVBoxLayout>
#include <QEvent>
#include <QDebug>

DiskItemList::DiskItemList(QFrame *parent) :
    QWidget(parent),
    m_layout(new QVBoxLayout),
    m_updateHTimer(new QTimer(this)),
    m_updateHeadTimer(new QTimer(this))
{
    m_layout->setMargin(0);
    m_layout->setSpacing(1);

    m_updateHTimer->setSingleShot(true);
    m_updateHTimer->setInterval(10);

    m_updateHeadTimer->setSingleShot(true);
    m_updateHeadTimer->setInterval(10);

    connect(m_updateHTimer, &QTimer::timeout, this, &DiskItemList::updateHeight, Qt::QueuedConnection);
    connect(m_updateHeadTimer, &QTimer::timeout, this, &DiskItemList::updateHeadTail, Qt::QueuedConnection);

    setLayout(m_layout);
}

DiskItemList::~DiskItemList()
{

}

void DiskItemList::appendItem(DiskItem *item)
{
    m_layout->insertWidget(m_layout->count(), item);
    item->installEventFilter(this);

    m_updateHeadTimer->start();
    m_updateHTimer->start();
}

void DiskItemList::removeItem(DiskItem *item)
{
    m_layout->removeWidget(item);
    item->removeEventFilter(this);

    m_updateHeadTimer->start();
    m_updateHTimer->start();
}

void DiskItemList::moveItem(DiskItem *item, const int index)
{
    const int oldIndex = m_layout->indexOf(item);
    if (oldIndex == index)
        return;

    m_layout->removeWidget(item);
    m_layout->insertWidget(index, item);

    const int max = m_layout->count() - 1;
    if (index == 0 || index == max ||
        oldIndex == 0 || oldIndex == max)
        m_updateHeadTimer->start();
}

void DiskItemList::setSpacing(const int spaceing)
{
    m_layout->setSpacing(spaceing);

    m_updateHTimer->start();
}

int DiskItemList::itemCount() const
{
    return m_layout->count();
}

void DiskItemList::clear()
{
    const int index = 0;
    const int count = m_layout->count();

    for (int i(index); i != count; ++i)
    {
        QLayoutItem *item = m_layout->takeAt(index);
        QWidget *w = item->widget();
        w->removeEventFilter(this);
        w->setParent(nullptr);
        delete item;
    }

    m_updateHeadTimer->start();
    m_updateHTimer->start();
}

DiskItem *DiskItemList::getItem(int index)
{
    if(index < 0)
        return NULL;

    if(index < itemCount())
    {
        return qobject_cast<DiskItem *>(m_layout->itemAt(index)->widget());
    }

    return NULL;
}

bool DiskItemList::eventFilter(QObject *, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Show:
    case QEvent::Hide:          m_updateHeadTimer->start();
    case QEvent::Resize:        m_updateHTimer->start();       break;
    default:;
    }

    return false;
}

void DiskItemList::updateHeadTail()
{
    DiskItem *head = nullptr;
    DiskItem *tail = nullptr;

    const int count = m_layout->count();
    for (int i(0); i != count; ++i)
    {
        DiskItem *item = qobject_cast<DiskItem *>(m_layout->itemAt(i)->widget());
        Q_ASSERT(item);

        if (!item->isVisible())
            continue;

        item->setIsHead(false);
        item->setIsTail(false);

        if (!head)
            head = item;
        tail = item;
    }

    if (head)
        head->setIsHead();
    if (tail)
        tail->setIsTail();
}

void DiskItemList::updateHeight()
{
    Q_ASSERT(sender() == m_updateHTimer);

    setFixedHeight(m_layout->sizeHint().height());
}
