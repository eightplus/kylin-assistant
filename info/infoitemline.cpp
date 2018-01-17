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

#include "infoitemline.h"

#include <QHBoxLayout>

InfoItemLine::InfoItemLine(QFrame *parent)
    : QFrame(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setFixedHeight(36);

    m_key = "";
    m_keyLabel = new QLabel;
    m_valueLabel = new QLabel;

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(20, 0, 10, 0);
    mainLayout->addWidget(m_keyLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_valueLabel);
    mainLayout->addStretch();

    this->setLayout(mainLayout);
}

InfoItemLine::~InfoItemLine()
{
    delete m_keyLabel;
    delete m_valueLabel;
}

void InfoItemLine::setInfoKey(const QString &key)
{
    m_keyLabel->setText(key);
    m_key = key;
}

void InfoItemLine::setInfoValue(const QString &value)
{
    m_valueLabel->setText(value);
}

QString InfoItemLine::getLineKey()
{
    return m_key;
}

void InfoItemLine::createCpuSwitcherGroup()
{
    /*ondemand_radio = new QRadioButton();
    ondemand_radio->setFocusPolicy(Qt::NoFocus);
    ondemand_radio->setFixedHeight(30);
    ondemand_radio->setText(tr("Ondemand"));
    ondemand_radio->setObjectName("ondemandradio");
    ondemand_radio->setChecked(false);
    ondemand_radio->hide();
    powersave_radio = new QRadioButton();
    powersave_radio->setFixedHeight(30);
    powersave_radio->setFocusPolicy(Qt::NoFocus);
    powersave_radio->setText(tr("Powersave"));
    powersave_radio->setObjectName("powersaveradio");
    powersave_radio->setChecked(false);
    powersave_radio->hide();
    performance_radio = new QRadioButton();
    performance_radio->setFixedHeight(30);
    performance_radio->setFocusPolicy(Qt::NoFocus);
    performance_radio->setText(tr("Performance"));
    performance_radio->setObjectName("performanceradio");
    performance_radio->setChecked(false);
    performance_radio->hide();
    connect(ondemand_radio, SIGNAL(clicked()), this, SLOT(setRadioButtonRowStatus()));
    connect(powersave_radio, SIGNAL(clicked()), this, SLOT(setRadioButtonRowStatus()));
    connect(performance_radio, SIGNAL(clicked()), this, SLOT(setRadioButtonRowStatus()));
    layout = new QHBoxLayout();
    layout->setSpacing(10);
    layout->addWidget(ondemand_radio);
    layout->addWidget(powersave_radio);
    layout->addWidget(performance_radio);
    layout->addStretch();

    QString cur_cpu = systemProxy->get_current_cpufreq_scaling_governer_qt();
    QList<QString>::Iterator it = cpulist.begin(), itend = cpulist.end();
    bool showed = false;
    for(;it != itend; it++)
    {
        if(*it == "ondemand") {
            if (cur_cpu == *it) {
                ondemand_radio->setChecked(true);
                powersave_radio->setChecked(false);
                performance_radio->setChecked(false);
            }
            ondemand_radio->show();
            showed = true;
        }
        else if(*it == "powersave") {
            if (cur_cpu == *it) {
                powersave_radio->setChecked(true);
                ondemand_radio->setChecked(false);
                performance_radio->setChecked(false);
            }
            powersave_radio->show();
            showed = true;
        }
        else if(*it == "performance") {
            if (cur_cpu == *it) {
                performance_radio->setChecked(true);
                ondemand_radio->setChecked(false);
                powersave_radio->setChecked(false);
            }
            performance_radio->show();
            showed = true;
        }
    }*/
}

void InfoItemLine::setRadioButtonRowStatus()
{
    //emit this->clicked("aaa");

    /*QObject *obj = sender(); //返回发出信号的对象，用QObject类型接收
    QRadioButton* pbtn = qobject_cast<QRadioButton*>(obj);
    QString obj_name = pbtn->objectName();
    if(obj_name == "ondemandradio") {
        systemProxy->adjust_cpufreq_scaling_governer_qt("ondemand");
    }
    else if(obj_name == "powersaveradio") {
        systemProxy->adjust_cpufreq_scaling_governer_qt("powersave");
    }
    else if(obj_name == "performanceradio") {
        systemProxy->adjust_cpufreq_scaling_governer_qt("performance");
    }*/
}
