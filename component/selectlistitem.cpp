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

#include "selectlistitem.h"

SelectListItem::SelectListItem(QWidget *parent, QString description, QString tipMsg, bool hasTip, int itemWidth) :
    QWidget(parent)
    , m_description(description)
    , m_tip(tipMsg)
    , m_hasTip(hasTip)
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setSpacing(5);
    m_mainLayout->setMargin(0);

    m_checkBox = new QCheckBox(this);
    m_checkBox->setFocusPolicy(Qt::NoFocus);
    m_checkBox->setChecked(true);

    connect(m_checkBox, &QCheckBox::clicked, [=] (bool checked) {
        emit this->selectedSignal(checked, this->itemDescription());
    });

    m_descLabel = new QLabel(this);
    m_descLabel->setFixedWidth(itemWidth - m_checkBox->width() - 10);
    m_descLabel->setWordWrap(true);
    m_descLabel->setText(description);

    m_mainLayout->setAlignment(Qt::AlignLeft);
    m_mainLayout->addWidget(m_checkBox);
    m_mainLayout->addWidget(m_descLabel);
}

SelectListItem::~SelectListItem()
{

}

bool SelectListItem::itemIsChecked()
{
    return m_checkBox->isChecked();
}

QString SelectListItem::itemDescription()
{
    if (m_hasTip)
        return this->m_tip;
    else
        return this->m_description;
}
