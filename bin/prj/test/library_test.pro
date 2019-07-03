#-------------------------------------------------------------------------------
QT -= gui

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0




SOURCES += \
#        ../../../src/motion/common.cpp \
#        ../../../src/motion/hirestimer.cpp \
#        ../../../src/motion/motion_io.cpp \
#        ../direct/direct.cpp \
        main.cpp
HEADERS += \
#    ../../../src/ftdi/WinTypes.h \
#    ../../../src/ftdi/ftd2xx.h \
#    ../../../src/motion/common.h \
#    ../../../src/motion/hirestimer.h \
#    ../../../src/motion/motion_io.h \
    ../direct/direct.h


#INCLUDEPATH += /usr/local/lib
#DEPENDPATH  += /usr/local/lib
#LIBS        += -L/usr/local/lib/ -lftd2xx

INCLUDEPATH += /home/evil/Programming/Qt/se-cnc/bin/build/build-direct-Desktop_Qt_5_12_3_GCC_64bit-Release
DEPENDPATH  += /home/evil/Programming/Qt/se-cnc/bin/build/build-direct-Desktop_Qt_5_12_3_GCC_64bit-Release
LIBS        += -L/home/evil/Programming/Qt/se-cnc/bin/build/build-direct-Desktop_Qt_5_12_3_GCC_64bit-Release/ -ldirect

#INCLUDEPATH += /home/evil/Programming/Qt/se-cnc/src/ftdi
#DEPENDPATH  += /home/evil/Programming/Qt/se-cnc/src/ftdi
#LIBS        += -L/home/evil/Programming/Qt/se-cnc/src/ftdi/ -lftd2xx



