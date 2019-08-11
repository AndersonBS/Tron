#-------------------------------------------------
#
# Project created by QtCreator 2016-06-09T19:46:13
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PG1_TRON
TEMPLATE = app
CONFIG += c++1 app


SOURCES += main.cpp\
    player.cpp \
    gameboard.cpp \
    game.cpp \
    resizeableplayer.cpp \
    aiplayer.cpp

HEADERS  += \
    player.h \
    gameboard.h \
    game.h \
    resizeableplayer.h \
    aiplayer.h

RESOURCES += \
    Resources/resources.qrc
