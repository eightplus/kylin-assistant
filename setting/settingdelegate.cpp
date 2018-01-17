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

#include "settingdelegate.h"
#include "settingmodel.h"

#include <QPainter>
#include <QDebug>
#include <QImageReader>

SettingDelegate::SettingDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

void SettingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QString module = index.data().toString();
    if (module.isEmpty())
        return;

    QPainterPath path;//QPainterPath path(option.rect.topLeft());
    painter->setRenderHint(QPainter::Antialiasing, true);
    path.addRect(QRectF(option.rect));
//    QRectF r(0 / 2.0, 0 / 2.0, option.rect.width() - 0, option.rect.height() - 0);//左边 上边 右边 下边
//    path.addRoundedRect(r, 4, 4);painter.drawRoundedRect(r, 4, 4);

    QPen pen;
    pen.setColor(QColor("#0B95D7"));
    pen.setWidth(0.8);
    painter->setPen(pen);//painter->setPen(Qt::NoPen);

    if (index.data(SettingModel::HoverRole).toBool()) {
        painter->setOpacity(1);
        painter->drawPixmap(QPoint(option.rect.x() + 5, option.rect.y() + 5), QPixmap("://res/clothes.png"));
        painter->fillPath(path, QColor(135, 206, 250, 127));//QColor::fromRgbF(1, 1, 1, 0.4)
    }
    else {
        painter->setOpacity(0.5);
        painter->fillPath(path, QColor(255, 250, 250, 127));
    }

    painter->drawPath(path);

    auto font = option.font;
    if (index.data(SettingModel::HoverRole).toBool()) {
        font.setPixelSize(15);
        painter->setFont(font);
        painter->setPen(Qt::white);
    }
    else {
        font.setPixelSize(12);
        painter->setFont(font);
        painter->setPen(QPen(QColor("#363636"), 1));
    }
    painter->drawText(option.rect, Qt::AlignCenter, module);
}

QSize SettingDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    return index.data(Qt::SizeHintRole).toSize();
}
