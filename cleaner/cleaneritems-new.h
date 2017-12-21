﻿/*
 * Copyright (C) 2013 ~ 2015 National University of Defense Technology(NUDT) & Kylin Ltd.
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

#ifndef CLEANERITEMS_H
#define CLEANERITEMS_H

#include <QWidget>
#include <QPushButton>

class MainWindow;
class QCheckBox;
class QGroupBox;

class CleanerItems : public QWidget
{
    Q_OBJECT
public:
    explicit CleanerItems(QStringList &arglist, QStringList &statuslist, QString skin = ":/background/res/skin/1.png", int height = 0, const QString title_text = "UbuntuKylin", QWidget *parent = 0);
    ~CleanerItems();
    void setParentWindow(MainWindow* window) { p_mainwindow = window;}
    void setLanguage();
    void initConnect();
    int getItemCount();
    QStringList getSelectedItems();

public slots:
    void resetSubCheckbox(int status);
    void scanAllSubCheckbox();
    void onCloseButtonClicked();

signals:
    void notifyMainCheckBox(int status);
    void sendoksignal();

private:
    QList<QCheckBox *> checkbox_list;
    MainWindow *p_mainwindow;
    QString titleName;
    QPushButton *okBtn;
    QGroupBox *group_box;
};

#endif // CLEANERITEMS_H




