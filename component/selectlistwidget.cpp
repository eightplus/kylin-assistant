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

#include "selectlistwidget.h"
#include <QDebug>

SelectListWidget::SelectListWidget(QWidget *parent) :
    QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_titleLabel = new QLabel;
    m_titleLabel->setFixedSize(80,30);
    m_titleLabel->setText(tr("Items:"));
    m_countLabel = new QLabel;
    m_countLabel->setFixedSize(100,30);
    m_widget = new QWidget;
    m_widget->setObjectName("transparentWidget");
    m_listAreaWidgetLayout = new QVBoxLayout(m_widget);
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_widget);

    m_gridLayout->addWidget(m_titleLabel,0,0,1,1);
    m_gridLayout->addItem(new QSpacerItem(10,10),0,0,1,3);
    m_gridLayout->addWidget(m_countLabel,0,1,1,1);
    m_gridLayout->addWidget(m_scrollArea,1,0,5,5);

    resetToDefault();
}

SelectListWidget::~SelectListWidget()
{
    this->resetToDefault();
}

void SelectListWidget::loadListItems(const QString &title, const QStringList &cachelist, int itemWidth)
{
    m_itemsMap.clear();

    int count = cachelist.count();
    m_countLabel->setText(QString::number(count));

    foreach (QString cache, cachelist) {
        SelectListItem *item = new SelectListItem(0, cache, itemWidth);
        connect(item, SIGNAL(selectedSignal(bool,QString)), this, SLOT(onSelectedSignal(bool,QString)));
        item->setMaximumSize(itemWidth, 30);
        m_listAreaWidgetLayout->addWidget(item);
        m_itemsMap.insert(cache, item);
    }
    m_listAreaWidgetLayout->addStretch();
}

void SelectListWidget::removeOneItem(const QString &description)
{
    SelectListItem *item = m_itemsMap.value(description);
    if(item == Q_NULLPTR)
        return;
}

QStringList SelectListWidget::getSelectedItems()
{
    QStringList text_list;

    /*foreach (QString text, m_itemsMap.keys()) {

    }*/
    QMap<QString, SelectListItem*>::iterator it;
    for (it = m_itemsMap.begin(); it != m_itemsMap.end(); ++it) {
        SelectListItem *item = static_cast<SelectListItem *>(it.value());
        if (item->itemIsChecked())
            text_list.append(item->itemDescription());
    }

    return text_list;
}

void SelectListWidget::scanAllSubCheckbox()
{
    int selectedCount = 0;
    QMap<QString, SelectListItem*>::iterator it;
    for (it = m_itemsMap.begin(); it != m_itemsMap.end(); ++it) {
        SelectListItem *item = static_cast<SelectListItem *>(it.value());
        if (item->itemIsChecked())
            selectedCount += 1;
    }
    m_countLabel->setText(QString::number(selectedCount));
    if (selectedCount == 0) {
        emit this->notifyMainCheckBox(0);
    }
    else if (selectedCount == m_itemsMap.count()) {
        emit this->notifyMainCheckBox(2);
    }
    else {
        emit this->notifyMainCheckBox(1);
    }

    /*int count = checkbox_list.count();
    int m = 0;
    for(int i=0; i<count; i++)
    {
        QCheckBox *checkbox = checkbox_list.at(i);
        if (checkbox->isChecked()) {
            m +=1;
        }
    }
    num_label->setText(QString::number(m));
    if (m == 0) {
        emit this->notifyMainCheckBox(0);
    }
    else if (m == count) {
        emit this->notifyMainCheckBox(2);
    }
    else {
        emit this->notifyMainCheckBox(1);
    }*/
}

void SelectListWidget::resetSubCheckbox(int status)
{
    /*if(status == 0) {
        for(int i=0; i<checkbox_list.count(); i++)
        {
            QCheckBox *checkbox = checkbox_list.at(i);
            checkbox->setChecked(false);
        }
        num_label->setText("0");
    }
    else if(status == 2) {
        for(int i=0; i<checkbox_list.count(); i++)
        {
            QCheckBox *checkbox = checkbox_list.at(i);
            checkbox->setChecked(true);
        }
        int count = checkbox_list.count();
        num_label->setText(QString::number(count));
    }*/
}

void SelectListWidget::onSelectedSignal(bool checked, QString description)
{

}

void SelectListWidget::resetToDefault()
{
    m_itemsMap.clear();
    while(m_listAreaWidgetLayout->count() > 0) {
        QWidget* widget = m_listAreaWidgetLayout->itemAt(0)->widget();
        m_listAreaWidgetLayout->removeWidget(widget);
        delete widget;
    }
}
