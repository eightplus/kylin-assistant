#-------------------------------------------------
#
# Project created by QtCreator 2015-01-21T08:48:08
#
#-------------------------------------------------
TARGET = kylin-assistant
DESTDIR = ../
TEMPLATE = app
CONFIG += c++11
CONFIG += qt warn_on
CONFIG += release
CONFIG += link_pkgconfig

QT += dbus network

inst1.files += res/kylin-assistant.png
inst1.path = /usr/share/pixmaps
inst2.files += ../kylin-assistant.desktop
inst2.path = /usr/share/applications
inst3.files += res/default/
inst3.path = /var/lib/kylin-assistant-daemon/
inst4.files += res/custom/
inst4.path = /var/lib/kylin-assistant-daemon/
inst5.files += res/app-autostart-default.png
inst5.path = /usr/share/pixmaps
target.source += $$TARGET
target.path = /usr/bin
INSTALLS += inst1 \
    inst2 \
    inst3 \
    inst4 \
    inst5 \
    target

QMAKE_CPPFLAGS *= $(shell dpkg-buildflags --get CPPFLAGS)
QMAKE_CFLAGS   *= $(shell dpkg-buildflags --get CFLAGS)
QMAKE_CXXFLAGS *= $(shell dpkg-buildflags --get CXXFLAGS)
QMAKE_LFLAGS   *= $(shell dpkg-buildflags --get LDFLAGS)

isEqual(QT_MAJOR_VERSION, 5) {
    QT += widgets gui
}

unix {
    UI_DIR = .ui
    MOC_DIR = .moc
    OBJECTS_DIR = .obj
}

SOURCES += main.cpp\
    mainwindow.cpp \
    titlewidget.cpp \
    loginwidget.cpp \
    actionwidget.cpp \
    homeactionwidget.cpp \
    infoactionwidget.cpp \
    cleaneractionwidget.cpp \
    settingactionwidget.cpp \
    boxactionwidget.cpp \
    toolwidget.cpp \
    contentwidget.cpp \
    infowidget.cpp \
    cleanerwidget.cpp \
    settingwidget.cpp \
    boxwidget.cpp \
    pluginmanager.cpp \
    ../component/kylinlistmodel.cpp \
    ../component/kylinbutton.cpp \
    ../component/kylinmenu.cpp \
    ../component/kylintoolbutton.cpp \
    ../component/kylinswitcher.cpp \
    ../component/systembutton.cpp \
    ../component/kylintitlebar.cpp \
    ../component/threadpool.cpp \
    homepage.cpp \
    ../info/systemwidget.cpp \
    ../info/desktopwidget.cpp \
    ../info/cpuwidget.cpp \
    ../info/memorywidget.cpp \
    ../info/boardwidget.cpp \
    ../info/hdwidget.cpp \
    ../info/nicwidget.cpp \
    ../info/monitorwidget.cpp \
    ../info/audiowidget.cpp \
    ../info/cdrowwidget.cpp \
    ../info/batterywidget.cpp \
    ../info/driverwidget.cpp \
    ../info/sensorwidget.cpp \
    ../info/computerpage.cpp \
    ../setting/themewidget.cpp \
    ../setting/iconwidget.cpp \
    ../setting/mousewidget.cpp \
    ../setting/voicewidget.cpp \
    ../setting/launcherwidget.cpp \
    ../setting/panelwidget.cpp \
    ../setting/windowwidget.cpp \
    ../setting/fontwidget.cpp \
    ../setting/touchpadwidget.cpp \
    ../setting/conservewidget.cpp \
    ../setting/nautiluswidget.cpp \
    ../component/scrollwidget.cpp \
    ../component/agentlistitem.cpp \
    ../component/quibo.cpp \
    ../component/kylinfontdialog.cpp \
    skincenter.cpp \
    ../component/kylinlistwidgetitem.cpp \
    ../component/kylinlistwidget.cpp \
    ../component/loadinglabel.cpp \
    ../component/alertdialog.cpp \
    ../component/toolkits.cpp \
    ../cleaner/cacheactionwidget.cpp \
    ../cleaner/cleanerdetailwidget.cpp \
    ../cleaner/cleanermainwidget.cpp \
    ../cleaner/cleaneritems.cpp \
    ../dbusproxy/youkersystemdbus.cpp \
    ../dbusproxy/youkersessiondbus.cpp \
    ../info/devicemanager.cpp \
    ../component/settingaction.cpp \
    autostartwidget.cpp \
    ../component/autogroup.cpp \
    kthread.cpp \
    aboutdialog.cpp \
    ../cleaner/cleanlistwidget.cpp \
    ../component/tipdialog.cpp \
    ../component/toolbutton.cpp \
    ../component/cleangroup.cpp \
    ../component/cleanbutton.cpp \
    ../component/cleansubgroup.cpp \
    ../component/cleansubbutton.cpp \
    ../component/cardwidget.cpp \
    ../component/itemcard.cpp \
    upgradedialog.cpp \
    ../component/normalwidget.cpp \
    ../component/normalcard.cpp \
    shadowwidget.cpp \
    ../component/basewidget.cpp

HEADERS  += mainwindow.h \
    kpplication.h \
    titlewidget.h \
    actionwidget.h \
    homeactionwidget.h \
    infoactionwidget.h \
    cleaneractionwidget.h \
    settingactionwidget.h \
    boxactionwidget.h \
    toolwidget.h \
    loginwidget.h \
    contentwidget.h \
    infowidget.h \
    cleanerwidget.h \
    settingwidget.h \
    boxwidget.h \
    pluginmanager.h \
    ../component/workthread.h \
    ../component/plugininterface.h \
    ../component/kylinlistmodel.h \
    ../component/utils.h \
    ../component/kylinbutton.h \
    ../component/kylinmenu.h \
    ../component/kylintoolbutton.h \
    ../component/kylinswitcher.h \
    ../component/systembutton.h \
    ../component/kylintitlebar.h \
    ../component/threadpool.h \
    homepage.h \
    ../info/systemwidget.h \
    ../info/desktopwidget.h \
    ../info/cpuwidget.h \
    ../info/memorywidget.h \
    ../info/boardwidget.h \
    ../info/hdwidget.h \
    ../info/nicwidget.h \
    ../info/monitorwidget.h \
    ../info/audiowidget.h \
    ../info/cdrowwidget.h \
    ../info/batterywidget.h \
    ../info/driverwidget.h \
    ../info/sensorwidget.h \
    ../info/computerpage.h \
    ../setting/themewidget.h \
    ../setting/iconwidget.h \
    ../setting/mousewidget.h \
    ../setting/voicewidget.h \
    ../setting/launcherwidget.h \
    ../setting/panelwidget.h \
    ../setting/windowwidget.h \
    ../setting/fontwidget.h \
    ../setting/touchpadwidget.h \
    ../setting/conservewidget.h \
    ../setting/nautiluswidget.h \
    ../component/scrollwidget.h \
    ../component/agentlistitem.h \
    ../component/quibo.h \
    ../component/kylinfontdialog.h \
    skincenter.h \
    ../component/kylinlistwidgetitem.h \
    ../component/kylinlistwidget.h \
    ../component/loadinglabel.h \
    ../component/alertdialog.h \
    ../component/toolkits.h \
    ../cleaner/cacheactionwidget.h \
    ../cleaner/cleanerdetailwidget.h \
    ../cleaner/cleanermainwidget.h \
    ../cleaner/cleaneritems.h \
    ../dbusproxy/youkersystemdbus.h \
    ../dbusproxy/youkersessiondbus.h \
    ../info/devicemanager.h \
    ../component/settingaction.h \
    autostartwidget.h \
    ../component/autogroup.h \
    kthread.h \
    aboutdialog.h \
    ../cleaner/cleanlistwidget.h \
    ../component/tipdialog.h \
    ../component/toolbutton.h \
    ../component/cleangroup.h \
    ../component/cleanbutton.h \
    ../component/cleansubgroup.h \
    ../component/cleansubbutton.h \
    ../component/cardwidget.h \
    ../component/itemcard.h \
    upgradedialog.h \
    ../component/normalwidget.h \
    ../component/normalcard.h \
    shadowwidget.h \
    ../component/basewidget.h

FORMS    += \
    ../component/quibo.ui \
    ../component/alertdialog.ui \
    autostartwidget.ui \
    ../cleaner/cleanlistwidget.ui \
    ../cleaner/cleanerdetailwidget.ui \
    ../component/itemcard.ui \
    ../component/normalcard.ui

RESOURCES += \
    img.qrc


INCLUDEPATH += qtsingleapplication
DEPENDPATH += qtsingleapplication

SOURCES += qtsingleapplication/qtsingleapplication.cpp qtsingleapplication/qtlocalpeer.cpp
HEADERS += qtsingleapplication/qtsingleapplication.h qtsingleapplication/qtlocalpeer.h
