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
    tcpserver.cpp \
    login.cpp

HEADERS  += chatsystem.h \
    tcpclient.h \
    tcpserver.h \
    login.h

FORMS    += chatsystem.ui \
    tcpclient.ui \
    tcpserver.ui \
    login.ui

RESOURCES += \
    images.qrc

win32 {
    RC_FILE = logo.rc
}

#macx{
#    ICON = logo.icns
#}

#unix:!macx{
#    kde-config --path icon
#}
