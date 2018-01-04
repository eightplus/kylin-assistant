/*
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

#include "processworker.h"
#include "../../component/utils.h"
#include "mydialog.h"
#include "processlistitem.h"
#include "myactiongroup.h"
#include "myactiongroupitem.h"
#include "myaction.h"
#include "processlistwidget.h"

#include <QLabel>
#include <QMap>
#include <QMenu>
#include <QPixmap>
#include <QString>
#include <QWidget>
#include <QDialog>
#include <QTimer>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QDebug>
#include <QHeaderView>
#include <QSettings>

class ProcessManager;

class ProcessDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessDialog(QList<bool> columnShowOrHideFlags, int sortIndex, bool sortOrder, QSettings *settings, QWidget* parent = 0);
    ~ProcessDialog();

    ProcessListWidget* getProcessView();
    void displayAllProcess();
    void displayActiveProcess();
    void displayCurrentUserProcess();
    void clearOriginProcList();

signals:
    void changeColumnVisible(int index, bool visible, QList<bool> columnVisible);
    void changeSortStatus(int index, bool sortOrder);

public slots:
    void focusProcessView();
    void onSearch(QString text);
    void stopProcesses();
    void continueProcesses();
    void endProcesses();
    void killProcesses();
    void popupMenu(QPoint pos, QList<ProcessListItem*> items);
    void showPropertiesDialog();
    void showEndProcessDialog();
    void showKillProcessDialog();
    void endDialogButtonClicked(int index, QString buttonText);
    void killDialogButtonClicked(int index, QString buttonText);
    void updateStatus(QList<ProcessListItem*> items);
    void onActiveWhoseProcess(int index);
    void changeProcPriority(int nice);
    void refreshProcessList();

private:
  QTimer *timer;
  QSettings *proSettings;
  guint64 cpu_total_time;
  guint64 cpu_total_time_last;
  MyDialog *killProcessDialog;
  MyDialog *endProcessDialog;
  ProcessListWidget *m_processListWidget;
  QAction *m_propertiyAction;
  QAction *m_stopAction;//停止
  QAction *m_continueAction;//继续进程
  QAction *m_endAction;//结束
  QAction *m_killAction;//杀死
//  QMenu *m_priorityMenu;
//  MyActionGroup * priorityGroup;
//  MyAction *veryHighAction;
//  MyAction *highAction;
//  MyAction *normalAction;
//  MyAction *lowAction;
//  MyAction *veryLowAction;
//  MyAction *customAction;
  QList<pid_t> *actionPids;
  QMenu *m_menu;
  QString whose_processes;
  gint num_cpus;
  unsigned frequency;
};
