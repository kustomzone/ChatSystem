#-------------------------------------------------
#
# Project created by QtCreator 2017-06-11T16:33:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChatSystem
TEMPLATE = app


SOURCES += main.cpp\
        chatsystem.cpp \
    tcpclient.cpp \
    tcpserver.cpp

HEADERS  += chatsystem.h \
    tcpclient.h \
    tcpserver.h

FORMS    += chatsystem.ui \
    tcpclient.ui \
    tcpserver.ui

#RESOURCES += \
#    images.qrc

RESOURCES += \
    images.qrc
