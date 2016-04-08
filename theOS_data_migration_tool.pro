#-------------------------------------------------
#
# Project created by QtCreator 2016-04-05T08:51:11
#
#-------------------------------------------------

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = datamigration
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    harddiskdialog.cpp \
    udisks2.cpp \
    countworker.cpp \
    newuser.cpp \
    transferprogress.cpp \
    storagemediaworker.cpp

HEADERS  += mainwindow.h \
    harddiskdialog.h \
    udisks2.h \
    countworker.h \
    newuser.h \
    transferprogress.h \
    storagemediaworker.h

FORMS    += mainwindow.ui \
    harddiskdialog.ui \
    newuser.ui \
    transferprogress.ui

RESOURCES += \
    resources.qrc
