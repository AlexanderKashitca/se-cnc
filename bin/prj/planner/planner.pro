#--------------------------------------------------------------------------------
QT -= gui
QT += core
QT += widgets
QT += datavisualization
#--------------------------------------------------------------------------------
CONFIG += c++11
CONFIG -= console
CONFIG -= app_bundle
#--------------------------------------------------------------------------------
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#--------------------------------------------------------------------------------
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#--------------------------------------------------------------------------------
SOURCES += \
            ../../../src/reflexesII/RML/src/ReflexxesAPI.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLCalculatePositionalExtrems.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLDecisionTree1A.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLDecisionTree1B.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLDecisionTree1C.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLDecisionTree2.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLDecisions.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLFallBackStrategy.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLIsPhaseSynchronizationPossible.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLPolynomial.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLPosition.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLQuicksort.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLSetupModifiedSelectionVector.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLStep1.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLStep1IntermediateProfiles.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLStep1Profiles.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLStep2.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLStep2IntermediateProfiles.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLStep2PhaseSynchronization.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLStep2Profiles.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLStep2WithoutSynchronization.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLStep3.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLVelocity.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLVelocityCalculatePositionalExtrems.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLVelocityFallBackStrategy.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLVelocityIsPhaseSynchronizationPossible.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLVelocityMethods.cpp \
            ../../../src/reflexesII/RML/src/TypeIIRMLVelocitySetupPhaseSyncSelectionVector.cpp \
            main.cpp \
            mainform.cpp

HEADERS += \
            ../../../src/reflexesII/RML/inc/RMLFlags.h \
            ../../../src/reflexesII/RML/inc/RMLInputParameters.h \
            ../../../src/reflexesII/RML/inc/RMLOutputParameters.h \
            ../../../src/reflexesII/RML/inc/RMLPositionFlags.h \
            ../../../src/reflexesII/RML/inc/RMLPositionInputParameters.h \
            ../../../src/reflexesII/RML/inc/RMLPositionOutputParameters.h \
            ../../../src/reflexesII/RML/inc/RMLVector.h \
            ../../../src/reflexesII/RML/inc/RMLVelocityFlags.h \
            ../../../src/reflexesII/RML/inc/RMLVelocityInputParameters.h \
            ../../../src/reflexesII/RML/inc/RMLVelocityOutputParameters.h \
            ../../../src/reflexesII/RML/inc/ReflexxesAPI.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLDecisionTree1A.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLDecisionTree1B.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLDecisionTree1C.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLDecisionTree2.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLDecisions.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLMath.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLPolynomial.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLPosition.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLQuicksort.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLStep1IntermediateProfiles.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLStep1Profiles.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLStep2IntermediateProfiles.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLStep2Profiles.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLStep2WithoutSynchronization.h \
            ../../../src/reflexesII/RML/inc/TypeIIRMLVelocity.h \
            mainform.h

#--------------------------------------------------------------------------------
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
#--------------------------------------------------------------------------------
INCLUDEPATH += /home/evil/Programming/Qt/se-cnc/src/reflexesII/RML/inc
INCLUDEPATH += /home/evil/Programming/Qt/se-cnc/src/reflexesII/RML/src
#--------------------------------------------------------------------------------
FORMS += \
        mainform.ui
#--------------------------------------------------------------------------------
