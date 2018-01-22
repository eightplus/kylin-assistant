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


#ifndef PROPERTIESSDIALOG_H
#define PROPERTIESSDIALOG_H

#include <QLabel>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QPoint>
#include <QTimer>

class QMouseEvent;
class MyImageButton;

//TODO: add timer to refresh

class PropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    PropertiesDialog(QWidget *parent = 0, pid_t pid=-1);
    ~PropertiesDialog();

    pid_t getPid();
    QRect getParentGeometry() const;
    void moveToCenter();
    void initProcproperties();

public slots:
    void refreshProcproperties();

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
//    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    MyImageButton *closeButton;
    QHBoxLayout *cmdlineLayout;
    QHBoxLayout *userLayout;
    QHBoxLayout *nameLayout;
    QHBoxLayout *cpuDurationLayout;
    QHBoxLayout *startTimeLayout;
    QLabel *cmdlineLabel;
    QLabel *cmdlineTitleLabel;
    QLabel *userLabel;
    QLabel *userTitleLabel;
    QLabel *iconLabel;
    QLabel *m_appNameLabel;
    QLabel *nameTitleLabel;
    QLabel *cpuDurationLabel;
    QLabel *cpuDurationTitleLabel;
    QLabel *startTimeLabel;
    QLabel *startTimeTitleLabel;
    QLabel *titleLabel;
    QVBoxLayout *layout;
    pid_t pid;
    QPoint dragPosition;
    bool mousePressed;
    QTimer *timer;
};

#endif // PROPERTIESSDIALOG_H
