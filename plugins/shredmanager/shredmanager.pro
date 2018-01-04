#-------------------------------------------------
#
# Project created by QtCreator 2015-01-26T09:16:38
#
#-------------------------------------------------

#QT       += core
isEqual(QT_MAJOR_VERSION, 5) {
    QT += widgets gui
}
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shredmanager
TEMPLATE = lib
CONFIG += plugin c++11
#INCLUDEPATH += ../shredmanager
#DESTDIR = ../libs
DESTDIR = $$_PRO_FILE_PWD_/../

#UI_DIR += $$PWD/../tmp/shredmanager/
#RCC_DIR += $$PWD/../tmp/shredmanager/
#MOC_DIR += $$PWD/../tmp/shredmanager/
#OBJECTS_DIR = $$PWD/../obj/shredmanager
unix {
    UI_DIR = .ui
    MOC_DIR = .moc
    OBJECTS_DIR = .obj
}

#target.source += $$TARGET
#target.path = /var/lib/kylin-assistant-daemon/libs/
target.path = $${PREFIX}/lib/kylin-assistant/plugins/
INSTALLS += target

HEADERS += \
    filewipe.h \
    ../../component/plugininterface.h \
    shredmanager.h \
    shreddialog.h \
    ../../component/toolkits.h \
    ../../component/alertdialog.h \
    ../../component/utils.h \
    ../../component/toolkits.h \
    ../../component/kylintitlebar.h \
    ../../component/systembutton.h \
    ../../component/kylineditbutton.h

SOURCES += \
    filewipe.cpp \
    shredmanager.cpp \
    shreddialog.cpp \
    ../../component/alertdialog.cpp \
    ../../component/toolkits.cpp \
    ../../component/kylintitlebar.cpp \
    ../../component/systembutton.cpp \
    ../../component/kylineditbutton.cpp

OTHER_FILES += \
    shred.json

FORMS += \
    ../../component/alertdialog.ui

RESOURCES += \
    ../../src/img.qrc
