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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPixmap>
#include <QSettings>
#include <QDesktopWidget>

#include "titlewidget.h"
#include "actionwidget.h"
#include "toolwidget.h"
//#include "loginwidget.h"
#include "bottomcontentwidget.h"
//#include "homepage.h"
#include "infowidget.h"
#include "settingwidget.h"
#include "cleanerwidget.h"
#include "boxwidget.h"
//#include "skincenter.h"
#include "../component/kylinmenu.h"
#include "../component/utils.h"
#include "../component/toolkits.h"
//#include "homeactionwidget.h"
//#include "infoactionwidget.h"
//#include "cleaneractionwidget.h"
//#include "settingactionwidget.h"
//#include "boxactionwidget.h"
#include "aboutdialog.h"
//#include "upgradedialog.h"
//#include "shadowwidget.h"
class QParallelAnimationGroup;
class SessionDispatcher;
class SystemDispatcher;
//class ShadowWidget;
#include "autostartwidget.h"
//#include "cameramanager.h"

class DataWorker;
class SystemDbusProxy;
class SessionDbusProxy;

class MainTopWidget;
class MiddleWidget;
class MainBottomWidget;
class TopBaseWidget;

//class MainWindow : public QDialog
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
//    explicit MainWindow(QString cur_arch = "", int d_count = 0, QWidget *parent = 0);
    explicit MainWindow(QString cur_arch = "", int d_count = 0, QWidget* parent = 0/*, Qt::WindowFlags flags = 0*/);
    ~MainWindow();
    void setTranslator(QTranslator* translator);
    void initConnect();
    void initHomePage();
    void initOtherPages();
    void reViewThePointSkin(QString pciture);
    void reViewTheOrgSkin();
    void changeSkin(QString pciture);
    void restoreSkin();
    void initAnimation();
    int getCurrentBackgroundIndex();
    QString getCurrentBackgroundName();
    QString getCurrentBackgroundAbsName();
    QStringList filterSkin();
    bool deleteFile(QString filename);
    bool CopyFile(QString filename);
    QString accessOSName();
    void createAboutDialog();

    void initWidgets();
    void moveCenter();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *);
    virtual void paintEvent(QPaintEvent *event);

public slots:
    void openSkinCenter();
    void openUpgradePage(/*QStringList version_list*/);
//    void openUpgradePageAgain();
    void showMainMenu();
    void closeYoukerAssistant();
    void setCurrentPageIndex(int index);
    void changeLanguage(LANGUAGE language);
    void newFeatures();
    void setupConfigure();
    void aboutUs();
    void startDbusDaemon();
    void displaySubPage(int index);
    void upAnimFinished();
    void closeAnimFinished();

//    void displayMainWindow(/*int display*/);


    void onInitDataFinished();

signals:
    void chanegBoxToolStatus();

private:
    QStackedWidget *m_topStack = nullptr;
    QStackedWidget *m_bottomStack = nullptr;

    MainTopWidget *m_mainTopWidget = nullptr;
    MiddleWidget *m_middleWidget = nullptr;
    MainBottomWidget *m_mainBottomWidget = nullptr;
    MainTopWidget *cleaner_action_widget;
    TopBaseWidget *info_action_widget = nullptr;
    TopBaseWidget *setting_action_widget = nullptr;
    TopBaseWidget *box_action_widget = nullptr;


//    QGridLayout *top_grid_layout;
//    QGridLayout *bottom_grid_layout;
//    TitleWidget *title_widget = nullptr;
//    ActionWidget *default_action_widget;
//    ActionWidget *other_action_widget;
//    ToolWidget *tool_widget = nullptr;
//    LoginWidget *login_widget;
//    BottomContentWidget *default_content_widget = nullptr;
//    BottomContentWidget *other_content_widget = nullptr;
//    HomePage *home_page;
    InfoWidget *info_widget;
    CleanerWidget *cleaner_widget;
    SettingWidget *setting_widget;
    BoxWidget *box_widget;
//    HomeActionWidget *home_action_widget;
//    InfoActionWidget *info_action_widget;
//    CleanerActionWidget *cleaner_action_widget;
//    SettingActionWidget *setting_action_widget;
//    BoxActionWidget *box_action_widget;
//    SkinCenter *skin_center;
    KylinMenu *main_menu;
//    QString version;
    QPoint m_dragPosition; //移动的距离
    bool m_mousePressed; //按下鼠标左键
    QTranslator* translator; //翻译器
    LANGUAGE current_language; //当前语言
    QPixmap main_skin_pixmap;
    QString last_skin_path;
    QPixmap review_skin_pixmap;
//    SystemDispatcher *systeminterface;
//    SessionDispatcher *sessioninterface;
    Toolkits *toolKits;
    AboutDialog *aboutDlg;
    QSettings *mSettings;
    QString desktop;
//    QString machine;
    QString osName;
    QString arch;
    bool battery;
    bool sensor;
    int display_count;
    QStringList m_cpulist;
    QString m_currentCpuMode;
    QParallelAnimationGroup *spreadGroup;
    QParallelAnimationGroup *gatherGroup;
    PAGESTATUS status;
    bool statusFlag;
//    AutoStartWidget *auto_start;
//    CameraManager *camera_manager;
//    UpgradeDialog *upgrade_dialog;
//    QStringList skinlist;
//    ShadowWidget *shadow_widget;

    DataWorker *m_dataWorker = nullptr;
    QWidget *centralWidget = nullptr;
};

class GlobalData // define by hebing,just for transmit var
{

public:
    static QString globalarch;
};

#endif // MAINWINDOW_H
