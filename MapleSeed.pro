#-------------------------------------------------
#
# Project created by QtCreator 2019-04-12T09:04:17
#
#-------------------------------------------------

QT += core gui xml network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MapleSeed
TEMPLATE = app

# Use Precompiled headers (PCH)
#CONFIG+= cmdline PRECOMPILED_HEADER
#PRECOMPILED_HEADER = precompiled.h

RC_FILE = resources\mapleseed.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

INCLUDEPATH += C:\OpenSSL-Win64\include

LIBS += -LC:\OpenSSL-Win64\lib -llibcrypto

SOURCES += \
    main.cpp \
    mapleseed.cpp \
    gamelibrary.cpp \
    downloadmanager.cpp \
    titleinfo.cpp \
    decrypt.cpp \
    configuration.cpp \
    libraryentry.cpp \
    QtCompressor.cpp \
    titleitem.cpp

HEADERS += \
    mapleseed.h \
    gamelibrary.h \
    downloadmanager.h \
    titleinfo.h \
    titleinfoitem.h \
    decrypt.h \
    configuration.h \
    titleitem.h \
    versioninfo.h \
    libraryentry.h \
    QtCompressor.h

FORMS += \
        mainwindow.ui \
        titleitem.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=

DISTFILES +=
