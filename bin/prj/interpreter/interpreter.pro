#-------------------------------------------------
#
# Project created by QtCreator 2019-06-14T14:01:32
#
#-------------------------------------------------

QT       += testlib dbus

QT       -= gui

TARGET = interpreter
TEMPLATE = lib

DEFINES += INTERPRETER_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../src/interpreter/PT2D.cpp \
        ../../../src/interpreter/PT3D.cpp \
        ../../../src/interpreter/canon_stand_alone.cpp \
        ../../../src/interpreter/common.cpp \
        ../../../src/interpreter/coordMotion.cpp \
        ../../../src/interpreter/driver.cpp \
        ../../../src/interpreter/kinematics.cpp \
        ../../../src/interpreter/kinematics3Rod.cpp \
        ../../../src/interpreter/kinematics5AxisGimbalAB.cpp \
        ../../../src/interpreter/kinematics5AxisGimbalCB.cpp \
        ../../../src/interpreter/kinematics5AxisTableAC.cpp \
        ../../../src/interpreter/kinematics5AxisTableAGimbalB.cpp \
        ../../../src/interpreter/kinematics5AxisTableBC.cpp \
        ../../../src/interpreter/kinematicsGeppetto.cpp \
        ../../../src/interpreter/kinematicsGeppettoExtrude.cpp \
        ../../../src/interpreter/rs274ngc.cpp \
        ../../../src/interpreter/setupTracker.cpp \
        ../../../src/interpreter/trajectoryPlanner.cpp \
        interpreter.cpp

HEADERS += \
        ../../../src/interpreter/PT2D.h \
        ../../../src/interpreter/PT3D.h \
        ../../../src/interpreter/canon.h \
        ../../../src/interpreter/common.h \
        ../../../src/interpreter/coordMotion.h \
        ../../../src/interpreter/driver.h \
        ../../../src/interpreter/kinematics.h \
        ../../../src/interpreter/kinematics3Rod.h \
        ../../../src/interpreter/kinematics5AxisGimbalAB.h \
        ../../../src/interpreter/kinematics5AxisGimbalCB.h \
        ../../../src/interpreter/kinematics5AxisTableAC.h \
        ../../../src/interpreter/kinematics5AxisTableAGimbalB.h \
        ../../../src/interpreter/kinematics5AxisTableBC.h \
        ../../../src/interpreter/kinematicsGeppetto.h \
        ../../../src/interpreter/kinematicsGeppettoExtrude.h \
        ../../../src/interpreter/rs274ngc.h \
        ../../../src/interpreter/rs274ngc_errors.h \
        ../../../src/interpreter/rs274ngc_return.h \
        ../../../src/interpreter/setupTracker.h \
        ../../../src/interpreter/trajectoryPlanner.h \
        interpreter.h \
        interpreter_global.h 

unix {
    target.path = /usr/lib
    INSTALLS += target
}
