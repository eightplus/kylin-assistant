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

#include "startupitem.h"
#include "../../component/kylinswitcher.h"
#include "startupdata.h"

#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QLabel>

inline QPixmap getAppIconPix(const QString &iconName, int iconSize)
{
    QIcon defaultExecutableIcon = QIcon::fromTheme("application-x-executable");
    QIcon icon;

    if (iconName.contains("/")) {
        icon = QIcon(iconName);
    }
    else {
        icon = QIcon::fromTheme(iconName, defaultExecutableIcon);
    }

    qreal devicePixelRatio = qApp->devicePixelRatio();

    QPixmap pixmap = icon.pixmap(iconSize * devicePixelRatio, iconSize * devicePixelRatio);
    pixmap.setDevicePixelRatio(devicePixelRatio);

    return pixmap;
}

StartupItem::StartupItem(StartupData info, QWidget *parent) : QWidget(parent)
  ,isEntered(false)
{
    item = new QListWidgetItem();
//    item->setSizeHint(QSize(400, 60));

    m_layout = new QHBoxLayout();
    m_leftLayout = new QHBoxLayout();
    m_switchLayout = new QHBoxLayout();

    m_appIcon = new QLabel();
    m_appIcon->setFixedSize(40, 40);
    m_appIcon->setScaledContents(true);//自动缩放,显示图像大小自动调整为Qlabel大小
    m_appIcon->setPixmap(getAppIconPix(info.icon, 40));

    m_appNameLabel = new QLabel();
    this->setAppName(info.name);
    m_appDescLabel = new QLabel();
    m_appDescLabel->setText(info.comment);

    switcher = new KylinSwitcher();
    switcher->switchedOn = info.enabled;
//    connect(switcher, SIGNAL(clicked()), this, SLOT()
    connect(switcher, &KylinSwitcher::clicked, [=] () {
        //changeAutoStartAppStatus
        emit changeStartup(info.exec, switcher->switchedOn);
    });
    m_switchLayout->addWidget(switcher, 0, Qt::AlignCenter);

    m_leftLayout->addWidget(m_appIcon);

    m_labelWidget = new QWidget();
    m_labelLayout = new QVBoxLayout(m_labelWidget);
    m_labelLayout->addWidget(m_appNameLabel);
    m_labelLayout->addWidget(m_appDescLabel);
    m_leftLayout->addWidget(m_labelWidget);

    m_layout->addLayout(m_leftLayout);
    m_layout->addStretch();
    m_layout->addLayout(m_switchLayout);
    m_layout->setContentsMargins(10, 0, 10, 0);
    this->setLayout(m_layout);
}

QListWidgetItem* StartupItem::getItem()
{
    return item;
}

void StartupItem::setItemHovered()
{
    isEntered = true;
    repaint();
}

void StartupItem::unsetItemHovered()
{
    isEntered = false;
    repaint();
}

void StartupItem::setAppName(const QString &name)
{
    m_appName = name;
    m_appNameLabel->setText(name);
}

QString StartupItem::getAppName()
{
    return this->m_appName;
}

void StartupItem::enterEvent(QEvent *event)
{
    emit this->enter();

    QWidget::enterEvent(event);
}

void StartupItem::paintEvent(QPaintEvent *event)
{
    if (isEntered) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QPainterPath path;
        path.addRoundedRect(QRectF(rect()), 2, 2);
        painter.fillPath(path, QColor(135, 206, 250, 127));
    }

    QWidget::paintEvent(event);
}


