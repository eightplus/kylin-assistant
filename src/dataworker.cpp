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

#include "dataworker.h"

#include <QDebug>
#include <QTimer>
#include <QApplication>

#include "../component/threadpool.h"
#include "../dbusproxy/youkersessiondbus.h"
#include "../dbusproxy/youkersystemdbus.h"

DataWorker::DataWorker(QString desk, QObject *parent)
    : QObject(parent)
    , desktop(desk)
{
    m_sensorTimer = new QTimer(this);
    connect(m_sensorTimer, SIGNAL(timeout()), this, SLOT(updateSensorValue()));
}

DataWorker::~DataWorker()
{
    if (m_sensorTimer) {
        disconnect(m_sensorTimer,SIGNAL(timeout()),this,SLOT(updateSensorValue()));
        if(m_sensorTimer->isActive()) {
            m_sensorTimer->stop();
        }
        delete m_sensorTimer;
        m_sensorTimer = nullptr;
    }

    if (iface) {//iface->deleteLater()
        delete iface;
        iface = nullptr;
    }
}

void DataWorker::initDataWorker()
{
    //power dbus
    iface = new QDBusInterface("org.gnome.SettingsDaemon",
                               "/org/gnome/SettingsDaemon/Power",
                               "org.gnome.SettingsDaemon.Power.Screen",
                               QDBusConnection::sessionBus());

    //session dbus
    sessioninterface = new SessionDispatcher;
    ThreadPool::Instance()->moveToNewThread(sessioninterface);
    this->m_existBattery = sessioninterface->judge_power_is_exists_qt();
    connect(sessioninterface, SIGNAL(string_value_notify(QString,QString)), this, SIGNAL(string_value_notify(QString,QString)));
    connect(sessioninterface, SIGNAL(bool_value_notify(QString,bool)), this, SIGNAL(bool_value_notify(QString,bool)));
    connect(sessioninterface, SIGNAL(int_value_notify(QString,int)), this, SIGNAL(int_value_notify(QString,int)));
    connect(sessioninterface, SIGNAL(double_value_notify(QString, double)), this, SIGNAL(double_value_notify(QString, double)));

    connect(sessioninterface, SIGNAL(isScanning(QString)), this, SIGNAL(isScanning(QString)));
    connect(sessioninterface, SIGNAL(finishScanWork(QString)), this, SIGNAL(finishScanWork(QString)));
    connect(sessioninterface, SIGNAL(tellScanResult(QString,QString)), this, SIGNAL(tellScanResult(QString,QString)));

    connect(sessioninterface, SIGNAL(tellCleanerDetailStatus(QString)), this, SIGNAL(tellCleanerDetailStatus(QString)));
    connect(sessioninterface, SIGNAL(tellCleanerDetailError(QString)), this, SIGNAL(tellCleanerDetailError(QString)));
    connect(sessioninterface, SIGNAL(tellCleanerDetailData(QStringList)), this, SIGNAL(tellCleanerDetailData(QStringList)));

    //system dbus
    systeminterface = new SystemDispatcher;
    ThreadPool::Instance()->moveToNewThread(systeminterface);
    /*QThread *systemThread = ThreadPool::Instance()->newThread();
    systeminterface->moveToThread(systemThread);
    systemThread->start();*/

    this->m_existSensor = systeminterface->judge_sensors_exists_qt();

    connect(systeminterface, SIGNAL(finishCleanWorkMain(QString)), this, SIGNAL(finishCleanWorkMain(QString)));
    connect(systeminterface, SIGNAL(finishCleanWorkMainError(QString)), this, SIGNAL(finishCleanWorkMainError(QString)));
    connect(systeminterface, SIGNAL(quickCleanProcess(QString,QString)), this, SIGNAL(quickCleanProcess(QString,QString)));

    connect(systeminterface, SIGNAL(sendCleanOverSignal()), this, SIGNAL(sendCleanOverSignal()));
    connect(systeminterface, SIGNAL(policykitCleanSignal(bool)), this, SIGNAL(policykitCleanSignal(bool)));
    connect(systeminterface, SIGNAL(tellCleanerMainData(QStringList)), this, SIGNAL(tellCleanerMainData(QStringList)));
    connect(systeminterface, SIGNAL(tellCleanerMainStatus(QString,QString)), this, SIGNAL(tellCleanerMainStatus(QString,QString)));
    connect(systeminterface, SIGNAL(sendCleanErrorSignal(QString)), this, SIGNAL(sendCleanErrorSignal(QString)));
}

void DataWorker::doWork()
{
    this->initDataWorker();
    emit dataLoadFinished();
}

bool DataWorker::deleteAppointedFile(QString filename)
{
    bool result = systeminterface->delete_file_qt(filename);
    return result;
}

bool DataWorker::copyAppointedFile(QString filename)
{
    bool result = systeminterface->copy_file_qt(filename);
    return result;
}

//---------------scan
void DataWorker::onStartScanSystem(QMap<QString, QVariant> itemsMap)
{
    sessioninterface->scanSystemCleanerItems(itemsMap);
}

//-------------clean
void DataWorker::onStartCleanSystem(QMap<QString, QVariant> itemsMap)
{
    systeminterface->set_user_homedir_qt();
    systeminterface->cleanAllSelectItems(itemsMap);
}

//system info
void DataWorker::onUpdateSystemRunnedTime()
{
    int time_value = systeminterface->get_time_value_qt().toInt();
    emit sendSystemRunnedTime(time_value);
}

void DataWorker::onRequestRefreshSystemInfo()
{
    QMap<QString, QVariant> tmpMap = systeminterface->get_computer_info_qt();
//    qDebug() << "==================sys tmpMap="<<tmpMap;
    emit sendSystemInfo(tmpMap);
}


//desktop info
void DataWorker::onRequestDesktopInfo()
{
    QMap<QString, QVariant> tmpMap = sessioninterface->get_system_message_qt();
    emit sendDesktopInfo(tmpMap);
}

//cpu info
void DataWorker::onRequestCpuInfo()
{
    QMap<QString, QVariant> tmpMap = systeminterface->get_cpu_info_qt();
    emit sendCpuInfo(tmpMap);
}

//memory info
void DataWorker::onRequestMemoryInfo()
{
    QMap<QString, QVariant> tmpMap = systeminterface->get_memory_info_qt();
    emit sendMemoryInfo(tmpMap);
}

//board info
void DataWorker::onRequestBoardInfo()
{
    QMap<QString, QVariant> tmpMap = systeminterface->get_board_info_qt();
    emit sendBoardInfo(tmpMap);
}

//hd info
void DataWorker::onRequestHDInfo()
{
    QMap<QString, QVariant> tmpMap = systeminterface->get_harddisk_info_qt();
    emit sendHDInfo(tmpMap);
}

//nic info
void DataWorker::onRequestNicInfo()
{
    QMap<QString, QVariant> tmpMap = systeminterface->get_networkcard_info_qt();
    emit sendNicInfo(tmpMap);
}

//monitor info
void DataWorker::onRequestMonitorInfo()
{
    QMap<QString, QVariant> tmpMap = systeminterface->get_monitor_info_qt();
    emit sendMonitorInfo(tmpMap);
}

//audio info
void DataWorker::onRequestAudioInfo()
{
    QMap<QString, QVariant> tmpMap = systeminterface->get_audiocard_info_qt();
    emit sendAudioInfo(tmpMap);
}

//battery info
void DataWorker::onRequestBatteryInfo()
{
    QMap<QString, QVariant> tmpMap = sessioninterface->read_battery_info_qt();
    emit sendBatteryInfo(tmpMap);
}

//sensor info
void DataWorker::onRequestSensorInfo()
{
    QMap<QString, QVariant> tmpMap = systeminterface->get_sensor_info_qt();
    emit sendSensorInfo(tmpMap);
    if (m_sensorTimer && !m_sensorTimer->isActive())
        m_sensorTimer->start(1000*4);
}

void DataWorker::updateSensorValue()
{
    QMap<QString, QVariant> tmpMap = systeminterface->get_sensor_info_qt();
    emit sendSensorInfo(tmpMap);
}


//--------------------theme
void DataWorker::onRequestThemeData()
{
    QString currentTheme = sessioninterface->get_theme_qt();
    QStringList themeList = sessioninterface->get_themes_qt();

    emit this->sendThemeList(currentTheme, themeList);
}

void DataWorker::onChangeSystemTheme(const QString &theme)
{
    sessioninterface->set_theme_qt(theme);
}

//---------------------icon
void DataWorker::onRequestIconData()
{
    QString current_icon_theme = sessioninterface->get_icon_theme_qt();
    QStringList iconlist  = sessioninterface->get_icon_themes_qt();
    emit this->sendIconThemeList(current_icon_theme, iconlist);

    bool computer = sessioninterface->get_show_computer_qt();
    bool folder = sessioninterface->get_show_homefolder_qt();
    bool network = sessioninterface->get_show_network_qt();
    bool recycle = sessioninterface->get_show_trash_qt();
    bool disk = sessioninterface->get_show_devices_qt();
    emit this->sendDisplayIconValue(computer, folder, network, recycle, disk);
}

void DataWorker::onResetIconTheme(const QString &theme)
{
    sessioninterface->set_icon_theme_qt(theme);
}

void DataWorker::onDisplayComputerIcon(bool b)
{
    sessioninterface->set_show_computer_qt(b);
}

void DataWorker::onDisplayFolderIcon(bool b)
{
    sessioninterface->set_show_homefolder_qt(b);
}

void DataWorker::onDisplayNetworkIcon(bool b)
{
    sessioninterface->set_show_network_qt(b);
}

void DataWorker::onDisplayRecycleBinIcon(bool b)
{
    sessioninterface->set_show_trash_qt(b);
}

void DataWorker::onDisplayDiskIcon(bool b)
{
    sessioninterface->set_show_devices_qt(b);
}


//---------------mouse
void DataWorker::onRequestMouseData()
{
    QString current_cursor_theme = sessioninterface->get_cursor_theme_qt();
    int default_cursor_value = sessioninterface->get_cursor_size_qt();
    QStringList themeList = sessioninterface->get_cursor_themes_qt();
    emit this->sendMouseThemeAndCusorSize(current_cursor_theme, themeList, default_cursor_value);
}

void DataWorker::onResetMouseCursorTheme(const QString &theme)
{
    sessioninterface->set_cursor_theme_qt(theme);
    systeminterface->set_cursor_theme_with_root_qt(theme);
}


void DataWorker::onResetMouseCursorSize(int cursorSize)
{
    sessioninterface->set_cursor_size_qt(cursorSize);
}


//---------------voice
void DataWorker::onRequestSoundData()
{
    QString current_sound = sessioninterface->get_sound_theme_qt();
    QStringList soundlist  = systeminterface->get_sound_themes_qt();

    emit this->sendSoundList(current_sound, soundlist);

    bool login_music = sessioninterface->get_login_music_enable_qt();
    bool sound_event = sessioninterface->get_sound_event_enable_qt();
    bool input_sound = sessioninterface->get_input_feedback_sounds_enable_qt();

    emit this->sendEnableSoundValue(login_music, sound_event, input_sound);
}

void DataWorker::onResetVoiceTheme(QString selectTheme)
{
    sessioninterface->set_sound_theme_qt(selectTheme);
}

void DataWorker::onResetLoginTipVoice(bool b)
{
    sessioninterface->set_login_music_enable_qt(b);
}

void DataWorker::onResetEventVoice(bool b)
{
    sessioninterface->set_sound_event_enable_qt(b);
}

void DataWorker::onResetInputFeedbackVoice(bool b)
{
    sessioninterface->set_input_feedback_sounds_enable_qt(b);
}


//----------------panel
void DataWorker::onRequestMateOrUnityPanelData(bool isMate)
{
    if (isMate) {
        bool app = sessioninterface->get_show_apps_qt();
        bool showDesktop = sessioninterface->get_show_desktop_qt();
        bool icon = sessioninterface->get_show_icon_qt();
        bool places = sessioninterface->get_show_places_qt();
        emit this->sendMatePanelValue(app, showDesktop, icon, places);
    }
    else {
        int blur = sessioninterface->get_dash_blur_experimental_qt();
        double transparent = sessioninterface->get_panel_transparency_qt();
        emit this->sendUnityBlurAndTransparencyValue(blur, transparent);

        QString time_format = sessioninterface->get_time_format_qt();
        QStringList timelist  = sessioninterface->get_all_time_format_qt();
        bool showSecond = sessioninterface->get_show_seconds_qt();
        bool showWeek = sessioninterface->get_show_week_qt();
        bool showDate = sessioninterface->get_show_date_qt();
        emit this->sendUnityTimeValue(time_format, timelist, showSecond, showWeek, showDate);

        QString cur_power = sessioninterface->get_power_icon_policy_qt();
        QStringList powerlist  = sessioninterface->get_all_power_icon_policy_qt();
        bool showPower = sessioninterface->get_show_power_percentage_qt();
        bool showBatteryTime = sessioninterface->get_show_power_time_qt();
        emit this->sendUnityPanelPowerValue(cur_power, powerlist, showPower, showBatteryTime);
    }
}

void DataWorker::onResetPanelTransparencyValue(double value)
{
    sessioninterface->set_panel_transparency_qt(value);
}

void DataWorker::onResetDateFormat(const QString &selected)
{
    sessioninterface->set_time_format_qt(selected);
}

void DataWorker::onResetShowBatteryIcon(const QString &selected)
{
    sessioninterface->set_power_icon_policy_qt(selected);
}

void DataWorker::onResetDashBlurExperimental(int value)
{
    sessioninterface->set_dash_blur_experimental_qt(value);
}

void DataWorker::onResetDisplaySeconds(bool b)
{
    sessioninterface->set_show_seconds_qt(b);
}

void DataWorker::onResetDisplayWeek(bool b)
{
    sessioninterface->set_show_week_qt(b);
}

void DataWorker::onResetDisplayDate(bool b)
{
    sessioninterface->set_show_date_qt(b);
}

void DataWorker::onResetDisplayBatteryPercentage(bool b)
{
    sessioninterface->set_show_power_percentage_qt(b);
}

void DataWorker::onResetDisplayBatteryTime(bool b)
{
    sessioninterface->set_show_power_time_qt(b);
}

void DataWorker::onResetShowApplications(bool b)
{
    sessioninterface->set_show_apps_qt(b);
}

void DataWorker::onResetShowDesktop(bool b)
{
    sessioninterface->set_show_desktop_qt(b);
}

void DataWorker::onResetShowIcon(bool b)
{
    sessioninterface->set_show_icon_qt(b);
}

void DataWorker::onResetShowPlaces(bool b)
{
    sessioninterface->set_show_places_qt(b);
}

//--------------------launcher menu
void DataWorker::onRequestMateOrUnityMenuData(bool isMate)
{
    if (isMate) {
        int topIconSize = sessioninterface->get_mate_panel_icon_size_qt("top");
        int bottomIconSize = sessioninterface->get_mate_panel_icon_size_qt("bottom");
        bool topIconHide = sessioninterface->get_mate_panel_autohide_qt("top");
        bool bottomIconHide = sessioninterface->get_mate_panel_autohide_qt("bottom");
        emit this->sendMatePanelIconValue(topIconSize, bottomIconSize, topIconHide, bottomIconHide);
    }
    else {
        int iconSize = sessioninterface->get_launcher_icon_size_qt();
        bool iconHide = sessioninterface->get_launcher_autohide_qt();
        bool desktopIconShowed = sessioninterface->get_launcher_have_showdesktopicon_qt();
        double launcherTransparency = sessioninterface->get_launcher_transparency_qt();//TODO:WTF  is 0????
        int iconColourIndex = sessioninterface->get_launcher_icon_colouring_qt();
        QStringList colourlist = sessioninterface->get_all_launcher_icon_colourings_qt();

        emit this->sendUnityIconValue(iconSize, iconHide, desktopIconShowed, launcherTransparency, iconColourIndex, colourlist);

        QString current_position = sessioninterface->get_current_launcher_position_qt();
        QStringList positionlist  = sessioninterface->get_all_launcher_position_qt();
//        qDebug() <<"positionlist="<<positionlist;
        emit this->sendUnityLauncherPosition(current_position, positionlist);
    }
}

void DataWorker::onResetIconSizeValue(int value)
{
    sessioninterface->set_launcher_icon_size_qt(value);
}

void DataWorker::onResetTransparencyValue(int value)
{
    sessioninterface->set_launcher_transparency_qt(value);
}

void DataWorker::onResetIconColouring(int value)
{
    sessioninterface->set_launcher_icon_colouring_qt(value);
}

void DataWorker::onResetAutoHide(bool b)
{
    sessioninterface->set_launcher_autohide_qt(b);
}

void DataWorker::onResetDisplayDesktopIcon(bool b)
{
    sessioninterface->set_launcher_have_showdesktopicon_qt(b);
}

void DataWorker::onResetTopIconSizeValue(int value)
{
    sessioninterface->set_mate_panel_icon_size_qt("top", value);
}

void DataWorker::onResetBottomIconSizeValue(int value)
{
    sessioninterface->set_mate_panel_icon_size_qt("bottom", value);
}

void DataWorker::onResetTopAutoHide(bool b)
{
    sessioninterface->set_mate_panel_autohide_qt("top", b);
}

void DataWorker::onResetBottomAutoHide(bool b)
{
    sessioninterface->set_mate_panel_autohide_qt("bottom", b);
}

void DataWorker::onResetLauncherPosition(const QString &position)
{
    sessioninterface->set_launcher_position_qt(position);
}


//-----------------window
void DataWorker::onRequesetWindowButtonAlign()
{
    QString current_value = sessioninterface->get_window_button_align_qt();
    emit this->sendWindowButtonAlign(current_value);
}

void DataWorker::onRequesetMenusHaveIcons()
{
    bool menuHaveIcons = sessioninterface->get_menus_have_icons_qt();
    emit this->sendMenusHaveIcons(menuHaveIcons);
}

void DataWorker::onRequesetWindowTitileTags()
{
    QString current_wheel_type = sessioninterface->get_current_titlebar_wheel_qt();
    QStringList wheellist = sessioninterface->get_titlebar_wheel_qt();
    QString current_double_type = sessioninterface->get_current_titlebar_double_qt();
    QStringList titlebarOptions  = sessioninterface->get_titlebar_options_qt();
    QString current_middle_type = sessioninterface->get_current_titlebar_middle_qt();
    QString current_right_type = sessioninterface->get_current_titlebar_right_qt();
    emit this->sendWindowTitileTags(current_wheel_type, wheellist, titlebarOptions, current_double_type, current_middle_type, current_right_type);
}

void DataWorker::onResetMenusHaveIcon(bool b)
{
    sessioninterface->set_menus_have_icons_qt(b);
}

void DataWorker::onResetTitlebarWheel(const QString &selected)
{
    sessioninterface->set_titlebar_wheel_qt(selected);
}

void DataWorker::onResetTitlebarDoubleClick(const QString &selected)
{
    sessioninterface->set_titlebar_double_qt(selected);
}

void DataWorker::onResetMouseMiddleClick(const QString &selected)
{
    sessioninterface->set_titlebar_middle_qt(selected);
}

void DataWorker::onResetMouseRightClick(const QString &selected)
{
    sessioninterface->set_titlebar_right_qt(selected);
}

void DataWorker::onResetWindowButtonLeftOrRightAlign(bool isLeft)
{
    if (isLeft)
        sessioninterface->set_window_button_align_left_qt();
    else
        sessioninterface->set_window_button_align_right_qt();
}



//-----------------------font
void DataWorker::onRequestFontData()
{
    QString current_font = sessioninterface->get_font_qt();
    emit sendFontValue(current_font);

    QString desktop_font = sessioninterface->get_desktop_font_qt();
    if(desktop_font.isEmpty())
    {
//        sessionproxy->set_desktop_font_qt_default();
        if (this->desktop == "mate" || this->desktop == "MATE" || this->desktop == "UKUI" || this->desktop == "ukui")
            desktop_font = "Sans 10";
        else
            desktop_font = "Ubuntu 11";
        sessioninterface->set_desktop_font_qt(desktop_font);
    }
    desktop_font = sessioninterface->get_desktop_font_qt();
    emit sendDesktopFontValue(desktop_font);

    QString monospace_font = sessioninterface->get_monospace_font_qt();
    emit sendMonospaceFontValue(monospace_font);

    QString document_font = sessioninterface->get_document_font_qt();
    emit sendDocumentFontValue(document_font);

    QString titlebar_font = sessioninterface->get_window_title_font_qt();
    emit sendTitlebarFontValue(titlebar_font);


    double fontZoom = 0.0;
    if (this->desktop != "mate" || this->desktop == "MATE") {
        fontZoom = sessioninterface->get_font_zoom_qt();
    }
    QString current_smooth = sessioninterface->get_smooth_style_qt();
    QStringList smoothlist  = sessioninterface->get_smooth_style_list_qt();
    QString current_antialiasing = sessioninterface->get_antialiasing_style_qt();
    QStringList antialiasinglist  = sessioninterface->get_antialiasing_style_list_qt();
    emit sendFontSmoothAndAntialiasingValue(fontZoom, current_smooth, smoothlist, current_antialiasing, antialiasinglist);
}

void DataWorker::onSetDefaultFontByName(const QString &cur_font)
{
    sessioninterface->set_font_qt(cur_font);
}

void DataWorker::onSetDesktopFontByName(const QString &cur_font)
{
    sessioninterface->set_desktop_font_qt(cur_font);
}

void DataWorker::onSetMonospaceFontByName(const QString &cur_font)
{
    sessioninterface->set_monospace_font_qt(cur_font);
}

void DataWorker::onSetDocumentFontByName(const QString &cur_font)
{
    sessioninterface->set_document_font_qt(cur_font);
}

void DataWorker::onSetTitlebarFontByName(const QString &cur_font)
{
    sessioninterface->set_window_title_font_qt(cur_font);
}

void DataWorker::onResetFontZoomScalingValue(double value)
{
    sessioninterface->set_font_zoom_qt(value);
}

void DataWorker::onResetFontHinting(const QString &selected)
{
    sessioninterface->set_smooth_style_qt(selected);
}

void DataWorker::onResetFontAntialiasing(const QString &selected)
{
    sessioninterface->set_antialiasing_style_qt(selected);
}

void DataWorker::onRestoreDefaultFont(bool isMate)
{
    if (isMate) {
        sessioninterface->set_default_font_string_qt("org.mate.interface", "font-name", "string");
        emit sendDesktopFontValue(sessioninterface->get_default_font_string_qt("org.mate.interface", "font-name"));
    }
    else {
        sessioninterface->set_default_font_string_qt("org.gnome.desktop.interface", "font-name", "string");
        emit sendDesktopFontValue(sessioninterface->get_default_font_string_qt("org.gnome.desktop.interface", "font-name"));
    }
}

void DataWorker::onRestoreDesktopDefaultFont(bool isMate)
{
    if (isMate) {
        sessioninterface->set_default_font_string_qt("org.mate.caja.desktop", "font", "string");
        emit sendDesktopFontValue(sessioninterface->get_default_font_string_qt("org.mate.caja.desktop", "font"));
    }
    else {
        sessioninterface->set_default_font_string_qt("org.gnome.nautilus.desktop", "font", "string");
        emit sendDesktopFontValue(sessioninterface->get_default_font_string_qt("org.gnome.nautilus.desktop", "font"));
    }
}

void DataWorker::onRestoreMonospaceDefaultFont(bool isMate)
{
    if (isMate) {
        sessioninterface->set_default_font_string_qt("org.mate.interface", "monospace-font-name", "string");
        emit sendMonospaceFontValue(sessioninterface->get_default_font_string_qt("org.mate.interface", "monospace-font-name"));
    }
    else {
        sessioninterface->set_default_font_string_qt("org.gnome.desktop.interface", "monospace-font-name", "string");
        emit sendMonospaceFontValue(sessioninterface->get_default_font_string_qt("org.gnome.desktop.interface", "monospace-font-name"));
    }
}

void DataWorker::onRestoreDocumentDefaultFont(bool isMate)
{
    if (isMate) {
        sessioninterface->set_default_font_string_qt("org.mate.interface", "document-font-name", "string");
        emit sendDocumentFontValue(sessioninterface->get_default_font_string_qt("org.mate.interface", "document-font-name"));
    }
    else {
        sessioninterface->set_default_font_string_qt("org.gnome.desktop.interface", "document-font-name", "string");
        emit sendDocumentFontValue(sessioninterface->get_default_font_string_qt("org.gnome.desktop.interface", "document-font-name"));
    }
}

void DataWorker::onRestoreTitlebarDefaultFont(bool isMate)
{
    if (isMate) {
        sessioninterface->set_default_font_string_qt("org.gnome.desktop.wm.preferences", "titlebar-font", "string");
        emit sendTitlebarFontValue(sessioninterface->get_default_font_string_qt("org.gnome.desktop.wm.preferences", "titlebar-font"));
    }
    else {
        sessioninterface->set_default_font_string_qt("org.gnome.desktop.wm.preferences", "titlebar-font", "string");
        emit sendTitlebarFontValue(sessioninterface->get_default_font_string_qt("org.gnome.desktop.wm.preferences", "titlebar-font"));
    }
}



//-------------------touchpad
void DataWorker::onRequestMateOrUnityTouchpadData(bool isMate)
{
    bool touchpadEnable = sessioninterface->get_touchpad_enable_qt();
    bool touchscrollingHorizontal = sessioninterface->get_touchscrolling_use_horizontal_qt();
    QString mode_value = sessioninterface->get_scrollbars_mode_qt();
    int scroll_int_value = 0;
    QString scroll_string_value = "";
    if (isMate)
        scroll_int_value = sessioninterface->get_mate_touchscrolling_mode_qt();
    else
        scroll_string_value = sessioninterface->get_touchscrolling_mode_qt();

    emit sendTouchPadValue(touchpadEnable, touchscrollingHorizontal, mode_value, scroll_int_value, scroll_string_value);
}

void DataWorker::onResetTouchpad(bool b)
{
    sessioninterface->set_touchpad_enable_qt(b);
}

void DataWorker::onResetHorizontalScrolling(bool b)
{
    sessioninterface->set_touchscrolling_use_horizontal_qt(b);
}

void DataWorker::onSetScrollbarOverlayOrLegacyMode(bool b)
{
    if (b) {
        sessioninterface->set_scrollbars_mode_overlay_qt();
    }
    else {
        sessioninterface->set_scrollbars_mode_legacy_qt();
    }
}

void DataWorker::onSetMateTouchscrollingMode(int flag)
{
    sessioninterface->set_mate_touchscrolling_mode_qt(flag);
}

void DataWorker::onSetUnityTouchscrollingMode(int flag)
{
    if (flag == 0) {
        sessioninterface->set_touchscrolling_mode_disabled_qt();
    }
    else if (flag == 1) {
        sessioninterface->set_touchscrolling_mode_edge_qt();
    }
    else if (flag == 2) {
        sessioninterface->set_touchscrolling_mode_twofinger_qt();
    }
}


//---------------------ac and battery
void DataWorker::onRequestPowerAndBatteryData()
{
    double gamma = sessioninterface->get_screen_gamma_qt();
    QStringList aclist = sessioninterface->get_sleep_timeout_list_qt();
    int brightnessValue = -1;
    QString sleep_timeout_display_battery = "";
    QString sleep_timeout_display_ac = "";
    if (this->desktop == "mate" || this->desktop == "MATE" || this->desktop == "UKUI" || this->desktop == "ukui") {
        sleep_timeout_display_battery = sessioninterface->get_current_sleep_timeout_display_battery_qt();
        sleep_timeout_display_ac = sessioninterface->get_current_sleep_timeout_display_ac_qt();
    }
    else {
        QDBusReply<int> reply = iface->call("GetPercentage");
        if (reply.isValid()) {
            brightnessValue = reply.value();
        }
    }
    QString idle_delay = sessioninterface->get_current_idle_delay_qt();
    QStringList idledelaylist  = sessioninterface->get_idle_delay_list_qt();

    emit sendIdleAndGammaData(gamma, aclist, brightnessValue, sleep_timeout_display_battery, sleep_timeout_display_ac, idle_delay, idledelaylist);

    bool lockEnabled = sessioninterface->get_lock_enabled_qt();
    QString lock_delay = sessioninterface->get_current_lock_delay_qt();
    QStringList lockdelaylist = sessioninterface->get_lock_delay_list_qt();
    QString critical_low = sessioninterface->get_current_critical_low_qt();
    QString laptop_battery = sessioninterface->get_current_lid_battery_qt();
    QString laptop_ac = sessioninterface->get_current_lid_ac_qt();
    QString sleep_timeout_battery = sessioninterface->get_current_sleep_timeout_battery_qt();
    QStringList batterylist  = sessioninterface->get_sleep_timeout_list_qt();
    QString sleep_timeout_ac = sessioninterface->get_current_sleep_timeout_ac_qt();

    emit sendLockAndSleepData(lockEnabled, lock_delay, lockdelaylist, critical_low, laptop_battery, laptop_ac, sleep_timeout_battery, batterylist, sleep_timeout_ac);
}

void DataWorker::onResetScreenGammaValue(double value)
{
    sessioninterface->adjust_screen_gamma_qt(value);
}

void DataWorker::onResetBrightnessValue(int value)
{
    iface->call("setPercentage", value);
}

void DataWorker::onResetIdleDelay(int index, int curValue)
{
    if (index == 0)
    {
        sessioninterface->set_current_idle_delay_qt(60);
    }
    else if (index == 1)
    {
        sessioninterface->set_current_idle_delay_qt(120);
    }
    else if (index == 2)
    {
        sessioninterface->set_current_idle_delay_qt(180);
    }
    else if (index == 3)
    {
        sessioninterface->set_current_idle_delay_qt(300);
    }
    else if (index == 4)
    {
        sessioninterface->set_current_idle_delay_qt(600);
    }
    else if (index == 5)
    {
        sessioninterface->set_current_idle_delay_qt(1800);
    }
    else if (index == 6)
    {
        sessioninterface->set_current_idle_delay_qt(3600);
    }
    else if (index == 7)
    {
        sessioninterface->set_current_idle_delay_qt(0);
    }
    else if (index == 8)
    {
        sessioninterface->set_current_idle_delay_qt(curValue);
    }
}

void DataWorker::onResetLockEnabled(bool b)
{
    sessioninterface->set_lock_enabled_qt(b);
}

void DataWorker::onResetLockDelay(int index, int curValue)
{
    if (index == 0)
    {
        sessioninterface->set_current_lock_delay_qt(30);
    }
    else if (index == 1)
    {
        sessioninterface->set_current_lock_delay_qt(60);
    }
    else if (index == 2)
    {
        sessioninterface->set_current_lock_delay_qt(120);
    }
    else if (index == 3)
    {
        sessioninterface->set_current_lock_delay_qt(180);
    }
    else if (index == 4)
    {
        sessioninterface->set_current_lock_delay_qt(300);
    }
    else if (index == 5)
    {
        sessioninterface->set_current_lock_delay_qt(600);
    }
    else if (index == 6)
    {
        sessioninterface->set_current_lock_delay_qt(1800);
    }
    else if (index == 7)
    {
        sessioninterface->set_current_lock_delay_qt(3600);
    }
    else if (index == 8)
    {
        sessioninterface->set_current_lock_delay_qt(0);
    }
    else if (index == 9)
    {
        sessioninterface->set_current_lock_delay_qt(curValue);
    }
}

void DataWorker::onSetCurrentCriticalLow(const QString &value)
{
    sessioninterface->set_current_critical_low_qt(value);
}

void DataWorker::onSetCurrentLidBattery(const QString &value)
{
    sessioninterface->set_current_lid_battery_qt(value);
}

void DataWorker::onSetCurrentLidAC(const QString &value)
{
    sessioninterface->set_current_lid_ac_qt(value);
}

void DataWorker::onResetSleepTimeoutBattery(int index, int value)
{
    if (index == 0)
    {
        sessioninterface->set_current_sleep_timeout_battery_qt(300);
    }
    else if (index == 1)
    {
        sessioninterface->set_current_sleep_timeout_battery_qt(600);
    }
    else if (index == 2)
    {
        sessioninterface->set_current_sleep_timeout_battery_qt(1200);
    }
    else if (index == 3)
    {
        sessioninterface->set_current_sleep_timeout_battery_qt(1800);
    }
    else if (index == 4)
    {
        sessioninterface->set_current_sleep_timeout_battery_qt(3600);
    }
    else if (index == 5)
    {
        sessioninterface->set_current_sleep_timeout_battery_qt(7200);
    }
    else if (index == 6)
    {
        sessioninterface->set_current_sleep_timeout_battery_qt(0);
    }
    else if (index == 7)
    {
        sessioninterface->set_current_sleep_timeout_battery_qt(value);
    }
}

void DataWorker::onResetSleepTimeoutAC(int index, int value)
{
    if (index == 0)
    {
        sessioninterface->set_current_sleep_timeout_ac_qt(300);
    }
    else if (index == 1)
    {
        sessioninterface->set_current_sleep_timeout_ac_qt(600);
    }
    else if (index == 2)
    {
        sessioninterface->set_current_sleep_timeout_ac_qt(1200);
    }
    else if (index == 3)
    {
        sessioninterface->set_current_sleep_timeout_ac_qt(1800);
    }
    else if (index == 4)
    {
        sessioninterface->set_current_sleep_timeout_ac_qt(3600);
    }
    else if (index == 5)
    {
        sessioninterface->set_current_sleep_timeout_ac_qt(7200);
    }
    else if (index == 6)
    {
        sessioninterface->set_current_sleep_timeout_ac_qt(0);
    }
    else if (index == 7)
    {
        sessioninterface->set_current_sleep_timeout_ac_qt(value);
    }
}

void DataWorker::onResetSleepTimeoutDisplayBattery(int index, int value)
{
    if (index == 0)
    {
        sessioninterface->set_current_sleep_timeout_display_battery_qt(300);
    }
    else if (index == 1)
    {
        sessioninterface->set_current_sleep_timeout_display_battery_qt(600);
    }
    else if (index == 2)
    {
        sessioninterface->set_current_sleep_timeout_display_battery_qt(1200);
    }
    else if (index == 3)
    {
        sessioninterface->set_current_sleep_timeout_display_battery_qt(1800);
    }
    else if (index == 4)
    {
        sessioninterface->set_current_sleep_timeout_display_battery_qt(3600);
    }
    else if (index == 5)
    {
        sessioninterface->set_current_sleep_timeout_display_battery_qt(7200);
    }
    else if (index == 6)
    {
        sessioninterface->set_current_sleep_timeout_display_battery_qt(0);
    }
    else if (index == 7)
    {
        sessioninterface->set_current_sleep_timeout_display_battery_qt(value);
    }
}

void DataWorker::onResetSleepTimeoutDisplayAC(int index, int value)
{
    if (index == 0)
    {
        sessioninterface->set_current_sleep_timeout_display_ac_qt(300);
    }
    else if (index == 1)
    {
        sessioninterface->set_current_sleep_timeout_display_ac_qt(600);
    }
    else if (index == 2)
    {
        sessioninterface->set_current_sleep_timeout_display_ac_qt(1200);
    }
    else if (index == 3)
    {
        sessioninterface->set_current_sleep_timeout_display_ac_qt(1800);
    }
    else if (index == 4)
    {
        sessioninterface->set_current_sleep_timeout_display_ac_qt(3600);
    }
    else if (index == 5)
    {
        sessioninterface->set_current_sleep_timeout_display_ac_qt(7200);
    }
    else if (index == 6)
    {
        sessioninterface->set_current_sleep_timeout_display_ac_qt(0);
    }
    else if (index == 7)
    {
        sessioninterface->set_current_sleep_timeout_display_ac_qt(value);
    }
}



//-------------------file manager
void DataWorker::onRequestFileManagerData()
{
    bool locationReplacePathbar = sessioninterface->get_location_replace_pathbar_qt();
    bool autoMountMedia = sessioninterface->get_auto_mount_media_qt();
    bool autoOpenFolder = sessioninterface->get_auto_open_folder_qt();
    bool promptAutorunPrograms = sessioninterface->get_prompt_autorun_programs_qt();
    int thumbnailIconSize = sessioninterface->get_thumbnail_icon_size_qt();
    int thumbnailCacheTime = sessioninterface->get_thumbnail_cache_time_qt();
    int thumbnailCacheSize = sessioninterface->get_thumbnail_cache_size_qt();

    emit sendFileManagerData(locationReplacePathbar, autoMountMedia, autoOpenFolder, promptAutorunPrograms, thumbnailIconSize, thumbnailCacheTime, thumbnailCacheSize);
}

void DataWorker::onResetLocationReplacePathbar(bool b)
{
    sessioninterface->set_location_replace_pathbar_qt(b);
}

void DataWorker::onResetAutoMountMedia(bool b)
{
    sessioninterface->set_auto_mount_media_qt(b);
}

void DataWorker::onResetAutoOpenFolder(bool b)
{
    sessioninterface->set_auto_open_folder_qt(b);
}

void DataWorker::onResetPromptAutorunPrograms(bool b)
{
    sessioninterface->set_prompt_autorun_programs_qt(b);
}

void DataWorker::onResetThumbnailIconSize(int value)
{
    sessioninterface->set_thumbnail_icon_size_qt(value);
}

void DataWorker::onResetThumbnailCacheTime(int value)
{
    sessioninterface->set_thumbnail_cache_time_qt(value);
}

void DataWorker::onResetThumbnailCacheSize(int value)
{
    sessioninterface->set_thumbnail_cache_size_qt(value);
}
