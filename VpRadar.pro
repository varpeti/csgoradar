QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11
TARGET = VpRadar
SOURCES += \
    main.cpp \
    hack.cpp \
    remote.cpp \
    window.cpp


HEADERS += \
    hack.h \
    remote.h \
    window.h

INCLUDEPATH += /usr/include/boost

LIBS += \
    -lpthread
