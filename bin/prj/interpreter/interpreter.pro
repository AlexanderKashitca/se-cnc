#-------------------------------------------------------------------------------
QT -= gui
QT += core
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
SOURCES += \
            ../../../src/interpreter/cannon_in_out.cpp \
            ../../../src/interpreter/interpreter.cpp \
            ../../../src/interpreter/parser_g_code.cpp \
#            ../../../src/interpreter/canon_pre.cpp \
            ../../../src/interpreter/rs274ngc.cpp \
#            ../../../src/interpreter/rs274ngc_pre.cpp \
            main.cpp
#-------------------------------------------------------------------------------
HEADERS += \
            ../../../src/interpreter/cannon_in_out.h \
            ../../../src/interpreter/interpreter.h \
            ../../../src/interpreter/parser_g_code.h \
            ../../../src/interpreter/canon.h \
#            ../../../src/interpreter/canon_pre.h \
            ../../../src/interpreter/rs274ngc.h \
            ../../../src/interpreter/rs274ngc_define.h \
            ../../../src/interpreter/rs274ngc_return.h

#-------------------------------------------------------------------------------
DISTFILES += \
            ../../../src/interpreter/rs274ngc.tool_default \
            ../../../src/interpreter/rs274ngc.var \
            ../../../src/interpreter/rs274ngc_tool
#-------------------------------------------------------------------------------
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
#-------------------------------------------------------------------------------
