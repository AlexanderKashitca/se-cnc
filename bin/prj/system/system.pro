#-------------------------------------------------------------------------------
QT += gui
#-------------------------------------------------------------------------------
CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle
#-------------------------------------------------------------------------------
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#-------------------------------------------------------------------------------
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#-------------------------------------------------------------------------------
INCLUDEPATH += /usr/local/lib
DEPENDPATH  += /usr/local/lib
LIBS        += -L/usr/local/lib/ -lftd2xx
#-------------------------------------------------------------------------------
SOURCES += \
            ../../../src/planner/planner.cpp           \
            ../../../src/planner/segmentation.cpp      \
            ../../../src/parser/command.cpp            \
            ../../../src/parser/parser.cpp             \
            ../../../src/interpreter/cannon_in_out.cpp \
            ../../../src/interpreter/interpreter.cpp   \
            ../../../src/interpreter/rs274ngc.cpp      \
            ../../../src/coordinate/coordinate.cpp     \
            ../../../src/kinematics/kinematics.cpp     \
            ../../../src/motion/common.cpp             \
            ../../../src/motion/direct.cpp             \
            ../../../src/motion/motion_io.cpp          \
            ../../../src/system/system.cpp             \
            main.cpp
#--------------------------------------------------------------------------------
HEADERS += \
            ../../../src/planner/motion_param.h        \
            ../../../src/planner/planner.h             \
            ../../../src/planner/segmentation.h        \
            ../../../src/parser/command.h              \
            ../../../src/parser/parser.h               \
            ../../../src/interpreter/cannon_in_out.h   \
            ../../../src/interpreter/interpreter.h     \
            ../../../src/interpreter/canon.h           \
            ../../../src/interpreter/rs274ngc.h        \
            ../../../src/interpreter/rs274ngc_define.h \
            ../../../src/interpreter/rs274ngc_return.h \
            ../../../src/coordinate/coordinate.h       \
            ../../../src/ftdi/WinTypes.h               \
            ../../../src/ftdi/ftd2xx.h                 \
            ../../../src/kinematics/kinematics.h       \
            ../../../src/motion/common.h               \
            ../../../src/motion/direct.h               \
            ../../../src/motion/motion_io.h            \
            ../../../src/system/system.h

#--------------------------------------------------------------------------------
INCLUDEPATH += /home/evil/Programming/qt/se-cnc/src/reflexesII/RML/inc
INCLUDEPATH += /home/evil/Programming/qt/se-cnc/src/reflexesII/RML/src
#-------------------------------------------------------------------------------
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
#-------------------------------------------------------------------------------
