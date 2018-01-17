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

#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include "titlewidget.h"
#include "toolbar.h"
#include "processdialog.h"
#include "resourcesdialog.h"
#include "filesystemdialog.h"

#include <QFrame>
#include <QStackedWidget>
#include <QLabel>
#include <QSettings>

enum PDragState {NOT_PDRAGGING, START_PDRAGGING, PDRAGGING};

class SystemMonitor : public QFrame
{
    Q_OBJECT

public:
    SystemMonitor(QWidget *parent = 0);
    ~SystemMonitor();

    void initTitleWidget();
    void initToolBar();
    void initPanelStack();
    void initConnections();

    QList<bool> getColumnHideFlags();
    bool getSortOrder();
    int getSortIndex();
    void moveCenter();
//    void moveDialog(QPoint diff);

public slots:
    void recordVisibleColumn(int, bool, QList<bool> columnVisible);
    void recordSortStatus(int index, bool sortOrder);
    void onChangePage(int index);

protected:
    void resizeEvent(QResizeEvent *e) override;
//    bool eventFilter(QObject *, QEvent *);
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *event);
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    QStackedWidget  *m_sysMonitorStack = nullptr;
    TitleWidget  *m_titleWidget = nullptr;
    ToolBar *m_toolBar = nullptr;
    ProcessDialog *process_dialog = nullptr;
    ResouresDialog *resources_dialog = nullptr;
    FileSystemDialog *filesystem_dialog = nullptr;
//    PDragState drag_state;
//    QPoint start_drag;
    QSettings *proSettings;

    QPoint dragPosition;
    bool mousePressed;
};

#endif // SYSTEMMONITOR_H
