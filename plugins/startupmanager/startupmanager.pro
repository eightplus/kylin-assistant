#-------------------------------------------------
#
# Project created by QtCreator 2015-01-26T09:16:38
#
#-------------------------------------------------

QT += core

isEqual(QT_MAJOR_VERSION, 5) {
    QT += widgets gui svg
}

TARGET = startupmanager
TEMPLATE = lib
DESTDIR = $$_PRO_FILE_PWD_/../
CONFIG += plugin c++11 link_pkgconfig
PKGCONFIG += glib-2.0 gobject-2.0

QMAKE_CXXFLAGS += -fpermissive

target.path = $${PREFIX}/lib/kylin-assistant/plugins/
INSTALLS += target

unix {
    UI_DIR = .ui
    MOC_DIR = .moc
    OBJECTS_DIR = .obj
}

HEADERS += \
    startupmanager.h \
    startupwidget.h \
    ../../component/plugininterface.h \
    startuptitlewidget.h \
    ../widgets/mytristatebutton.h \
    startuplistwidget.h \
    startupitem.h \
    ../../component/kylinswitcher.h \
    startupworker.h \
    startupdata.h \
    util.h

SOURCES += \
    startupmanager.cpp \
    startupwidget.cpp \
    startuptitlewidget.cpp \
    ../widgets/mytristatebutton.cpp \
    startuplistwidget.cpp \
    startupitem.cpp \
    ../../component/kylinswitcher.cpp \
    startupworker.cpp \
#    startupdata.cpp \
    util.cpp

OTHER_FILES += \
    startupmanager.json

RESOURCES += \
    ../../src/img.qrc
