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

#include "themewidget.h"
#include "theme/themeview.h"

#include <QStandardItemModel>
#include <QSignalMapper>
#include <QListWidgetItem>
#include <QLabel>
#include <QDebug>
#include <QGridLayout>

ThemeWidget::ThemeWidget(QWidget *parent) :
    SettingModulePage(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    this->resize(parent->size());
//    this->setFixedSize(900, 403);

    label = new QLabel(this);
    label->setObjectName("tipLabel");
    label->setFixedHeight(30);
//    label->setGeometry(QRect(30, 15, 860, 50));
    label->setText(tr("Please choose theme which you need"));

    /*list_widget = new NormalWidget(119, 139, 20, this);
    list_widget->setGeometry(QRect(30, 55, 860, 330));
    list_widget->calculate_data();*/

    m_themeView = new ThemeView(this);
    connect(m_themeView, SIGNAL(sendSelectThemeName(QString)), this, SLOT(changeTheme(QString)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);
    layout->setMargin(0);
    layout->setContentsMargins(10, 5, 10, 0);
    layout->addWidget(label);
    layout->addWidget(m_themeView);
    this->setLayout(layout);
}

QString ThemeWidget::settingModuleName()
{
    return "ThemePage";
}

void ThemeWidget::onReceiveThemeList(const QString &currentTheme, const QStringList &themeList)
{
//    qDebug() << "currentTheme="<<currentTheme<<", themeList="<<themeList;

    if (!themeList.isEmpty()) {
        m_themeView->clearData();

        syslist.clear();
        syslist = themeList;
        //kobe test 2018

        for (const QString theme : themeList) {
            if(currentTheme == theme)
                m_themeView->loadOneTheme(theme, true);
            else
                m_themeView->loadOneTheme(theme, false);
        }


        /*this->resetUI();
        list_widget->resetData();

        QSignalMapper *signal_mapper = new QSignalMapper(this);
        for(int i = 0; i<syslist.length(); ++i)
        {
            NormalCard *card = new NormalCard(syslist[i], list_widget->cardPanel);
            card_list.append(card);
            if(currentTheme == syslist[i]) {
                card->showUsingLogo(true);
            }
            list_widget->add_card(card);
            card->show();
            connect(card, SIGNAL(sendSelectThemeName(QString)), signal_mapper, SLOT(map()));
            signal_mapper->setMapping(card, QString::number(i, 10));
            connect(signal_mapper, SIGNAL(mapped(QString)), this, SLOT(switchUsingLogo(QString)));
            connect(card, SIGNAL(sendSelectThemeName(QString)), this, SLOT(changeTheme(QString)));
        }*/
    }
}


void ThemeWidget::initSettingData()
{
    emit this->requestThemeData();

//    list_widget = new NormalWidget(119, 139, 20, this);
//    list_widget->setGeometry(QRect(30, 55, 860, 330));
//    list_widget->calculate_data();

//    QString current_theme = sessionproxy->get_theme_qt();
//    syslist = sessionproxy->get_themes_qt();
//    card_list.clear();
//    QSignalMapper *signal_mapper = new QSignalMapper(this);
//    for(int i = 0; i<syslist.length(); ++i)
//    {
//        NormalCard *card = new NormalCard(syslist[i], list_widget->cardPanel);
//        card_list.append(card);
//        if(current_theme == syslist[i]) {
//            card->showUsingLogo(true);
//        }
//        list_widget->add_card(card);
//        connect(card, SIGNAL(sendSelectThemeName(QString)), signal_mapper, SLOT(map()));
//        signal_mapper->setMapping(card, QString::number(i, 10));
//        connect(signal_mapper, SIGNAL(mapped(QString)), this, SLOT(switchUsingLogo(QString)));
//        connect(card, SIGNAL(sendSelectThemeName(QString)), this, SLOT(changeTheme(QString)));
//    }

    this->initConnect();
}

ThemeWidget::~ThemeWidget()
{
    this->resetUI();
}

void ThemeWidget::resetUI()
{
    //kobe test 2018
    /*for(int i=0; i<card_list.count(); i++)
    {
        NormalCard *card = card_list.at(i);
        delete card;
        card = NULL;
    }
    card_list.clear();*/
}

void ThemeWidget::initConnect() {

//    connect(sessionproxy, SIGNAL(string_value_notify(QString, QString)), this, SLOT(themewidget_notify_string(QString, QString)));
}

void ThemeWidget::switchUsingLogo(QString index)
{
    //kobe test 2018
    /*bool ok;
    int current_index = index.toInt(&ok, 10);
    for(int i=0; i<card_list.count(); i++)
    {
        NormalCard *card = card_list.at(i);
        if(current_index == i)
        {
            card->showUsingLogo(true);
        }
        else
        {
            card->showUsingLogo(false);
        }
    }*/
}

void ThemeWidget::changeTheme(QString name)
{
    emit changeSystemTheme(name);
//    sessionproxy->set_theme_qt(name);
}


void ThemeWidget::themewidget_notify_string(QString key, QString value)
{
    if (key == "gtk-theme")
    {
        QList<QString>::Iterator it = syslist.begin(), itend = syslist.end();
        int index = -1;
        bool exist = false;
        for(;it != itend; it++)
        {
            ++index;
            if(*it == value) {
                exist = true;
                break;
            }
        }
        if(exist)
        {
            exist = false;
            this->switchUsingLogo(QString::number(index, 10));
        }
    }
}
