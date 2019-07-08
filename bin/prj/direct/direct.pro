#-------------------------------------------------------------------------------
# Project created by QtCreator 2019-06-25T09:15:16 -----------------------------
#-------------------------------------------------------------------------------

QT      -= gui
TARGET   = direct
TEMPLATE = lib
DEFINES += DIRECT_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#INCLUDEPATH += /home/evil/Programming/Qt/se-cnc/src/ftdi
#DEPENDPATH  += /home/evil/Programming/Qt/se-cnc/src/ftdi
#LIBS        += -L/home/evil/Programming/Qt/se-cnc/src/ftdi/ -lftd2xx

INCLUDEPATH += /usr/local/lib
DEPENDPATH  += /usr/local/lib
LIBS        += -L/usr/local/lib/ -lftd2xx


SOURCES += \
        ../../../src/motion/common.cpp \
        ../../../src/motion/motion_io.cpp \
        direct.cpp

HEADERS += \
        ../../../src/ftdi/WinTypes.h \
        ../../../src/ftdi/ftd2xx.h \
        ../../../src/motion/common.h \
        ../../../src/motion/motion_io.h \
        direct.h \
        direct_global.h
