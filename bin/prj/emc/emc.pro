QT -= gui

CONFIG += c++11 console
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
        ../../../src/emc/canonmodule.cc \
        ../../../src/emc/gcodemodule.cc \
        ../../../src/emc/interp_arc.cc \
        ../../../src/emc/interp_array.cc \
        ../../../src/emc/interp_base.cc \
        ../../../src/emc/interp_check.cc \
        ../../../src/emc/interp_convert.cc \
        ../../../src/emc/interp_cycles.cc \
        ../../../src/emc/interp_execute.cc \
        ../../../src/emc/interp_find.cc \
        ../../../src/emc/interp_internal.cc \
        ../../../src/emc/interp_inverse.cc \
        ../../../src/emc/interp_namedparams.cc \
        ../../../src/emc/interp_o_word.cc \
        ../../../src/emc/interp_python.cc \
        ../../../src/emc/interp_queue.cc \
        ../../../src/emc/interp_read.cc \
        ../../../src/emc/interp_remap.cc \
        ../../../src/emc/interp_setup.cc \
        ../../../src/emc/interp_write.cc \
        ../../../src/emc/interpmodule.cc \
        ../../../src/emc/nurbs_additional_functions.cc \
        ../../../src/emc/pyarrays.cc \
        ../../../src/emc/pyblock.cc \
        ../../../src/emc/pyemctypes.cc \
        ../../../src/emc/pyinterp1.cc \
        ../../../src/emc/pyparamclass.cc \
        ../../../src/emc/rs274ngc_pre.cc \
        ../../../src/emc/tool_parse.cc \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../../src/emc/array1.hh \
    ../../../src/emc/interp_array_types.hh \
    ../../../src/emc/interp_base.hh \
    ../../../src/emc/interp_internal.hh \
    ../../../src/emc/interp_python.hh \
    ../../../src/emc/interp_queue.hh \
    ../../../src/emc/paramclass.hh \
    ../../../src/emc/rs274ngc.hh \
    ../../../src/emc/rs274ngc_interp.hh \
    ../../../src/emc/rs274ngc_return.hh \
    ../../../src/emc/tool_parse.h \
    ../../../src/emc/units.h
