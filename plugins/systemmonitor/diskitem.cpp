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

#include "diskitem.h"
#include "../widgets/myimagebutton.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QStyle>

DiskItem::DiskItem(QWidget *parent)
    : QFrame(parent)
      ,m_isHead(false)
      ,m_isTail(false)
      ,m_nameLabel(new QLabel)
      ,m_mountLabel(new QLabel)
      ,m_typeLabel(new QLabel)
      ,m_totoalLabel(new QLabel)
      ,m_availLabel(new QLabel)
      ,m_usedLabel(new QLabel)
      ,m_percentageLabel(new QLabel)
      ,m_detailBtn(new MyImageButton)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setFixedHeight(36);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainLayout->setContentsMargins(20, 0, 10, 0);
    mainLayout->addWidget(m_nameLabel);
    mainLayout->addWidget(m_mountLabel);
    mainLayout->addWidget(m_typeLabel);
    mainLayout->addWidget(m_totoalLabel);
    mainLayout->addWidget(m_availLabel);
    mainLayout->addWidget(m_usedLabel);
    mainLayout->addWidget(m_percentageLabel);
    mainLayout->addSpacing(8);
//    mainLayout->addStretch();
    mainLayout->addWidget(m_detailBtn);
    setLayout(mainLayout);

    m_detailBtn->setObjectName("DiskDetailButton");
    connect(m_detailBtn, &MyImageButton::clicked, this, [=] {
        //TODO: show detail dialog
    });
}

DiskItem::~DiskItem()
{
    delete m_nameLabel;
    delete m_mountLabel;
    delete m_typeLabel;
    delete m_totoalLabel;
    delete m_availLabel;
    delete m_usedLabel;
    delete m_percentageLabel;
    delete m_detailBtn;
}

void DiskItem::setIsHead(bool head)
{
    if (head == m_isHead) return;
    m_isHead = head;

    style()->unpolish(this);
    style()->polish(this);
}

void DiskItem::setIsTail(bool tail)
{
    if (tail == m_isTail) return;
    m_isTail = tail;

    style()->unpolish(this);
    style()->polish(this);
}

void DiskItem::setDevName(const QString &name)
{
    m_nameLabel->setText(name);

    setAccessibleName(name);
}

void DiskItem::setMountDir(const QString &mountdir)
{
    m_mountLabel->setText(mountdir);
}

void DiskItem::setDiskType(const QString &disktype)
{
    m_typeLabel->setText(disktype);
}

void DiskItem::setTotalCapacity(const QString &totalcapacity)
{
    m_totoalLabel->setText(totalcapacity);
}

void DiskItem::setAvailCapacity(const QString &availcapacity)
{
    m_availLabel->setText(availcapacity);
}

void DiskItem::setUsedCapactiy(const QString &usedcapactiy)
{
    m_usedLabel->setText(usedcapactiy);
}

void DiskItem::setPercentage(const QString &percentage)
{
    m_percentageLabel->setText(percentage);
}

void DiskItem::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();

    emit selected();
    emit clicked();
}

