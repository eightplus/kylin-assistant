#-------------------------------------------------
#
# Project created by QtCreator 2015-01-26T09:16:38
#
#-------------------------------------------------

isEqual(QT_MAJOR_VERSION, 5) {
    QT += widgets gui
}

TARGET = processmanager
TEMPLATE = lib
INCLUDEPATH += ../processmanager
DESTDIR = ../libs
CONFIG += plugin c++11 link_pkgconfig
PKGCONFIG += libgtop-2.0

UI_DIR += $$PWD/../tmp/processmanager/

RCC_DIR += $$PWD/../tmp/processmanager/

MOC_DIR += $$PWD/../tmp/processmanager/

OBJECTS_DIR = $$PWD/../obj/processmanager


target.source += $$TARGET
target.path = /var/lib/kylin-assistant-daemon/libs/
INSTALLS +=target

HEADERS += \
    ../component/plugininterface.h \
    processmanager.h \
    processdialog.h \
    procman.h \
    procapp.h \
    util.h \
    filelist.h \
    ../component/alertdialog.h \
    ../component/toolkits.h \
    ../component/utils.h \
    ../component/kylintitlebar.h \
    ../component/systembutton.h

SOURCES += \
    processmanager.cpp \
    processdialog.cpp \
    procman.cpp \
    util.cpp \
    filelist.cc \
    ../component/alertdialog.cpp \
    ../component/toolkits.cpp \
    ../component/kylintitlebar.cpp \
    ../component/systembutton.cpp

FORMS += \
    ../component/alertdialog.ui

RESOURCES += \
    ../src/img.qrc
