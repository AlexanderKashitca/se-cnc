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
            ../../../src/planner/reflexesII/src/ReflexxesAPI.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLCalculatePositionalExtrems.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLDecisionTree1A.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLDecisionTree1B.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLDecisionTree1C.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLDecisionTree2.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLDecisions.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLFallBackStrategy.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLIsPhaseSynchronizationPossible.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLPolynomial.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLPosition.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLQuicksort.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLSetupModifiedSelectionVector.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLStep1.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLStep1IntermediateProfiles.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLStep1Profiles.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLStep2.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLStep2IntermediateProfiles.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLStep2PhaseSynchronization.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLStep2Profiles.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLStep2WithoutSynchronization.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLStep3.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLVelocity.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLVelocityCalculatePositionalExtrems.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLVelocityFallBackStrategy.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLVelocityIsPhaseSynchronizationPossible.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLVelocityMethods.cpp \
            ../../../src/planner/reflexesII/src/TypeIIRMLVelocitySetupPhaseSyncSelectionVector.cpp \
            main.cpp \
            mainform.cpp

HEADERS += \
            ../../../src/planner/reflexesII/inc/RMLFlags.h \
            ../../../src/planner/reflexesII/inc/RMLInputParameters.h \
            ../../../src/planner/reflexesII/inc/RMLOutputParameters.h \
            ../../../src/planner/reflexesII/inc/RMLPositionFlags.h \
            ../../../src/planner/reflexesII/inc/RMLPositionInputParameters.h \
            ../../../src/planner/reflexesII/inc/RMLPositionOutputParameters.h \
            ../../../src/planner/reflexesII/inc/RMLVector.h \
            ../../../src/planner/reflexesII/inc/RMLVelocityFlags.h \
            ../../../src/planner/reflexesII/inc/RMLVelocityInputParameters.h \
            ../../../src/planner/reflexesII/inc/RMLVelocityOutputParameters.h \
            ../../../src/planner/reflexesII/inc/ReflexxesAPI.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLDecisionTree1A.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLDecisionTree1B.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLDecisionTree1C.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLDecisionTree2.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLDecisions.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLMath.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLPolynomial.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLPosition.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLQuicksort.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLStep1IntermediateProfiles.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLStep1Profiles.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLStep2IntermediateProfiles.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLStep2Profiles.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLStep2WithoutSynchronization.h \
            ../../../src/planner/reflexesII/inc/TypeIIRMLVelocity.h \
            mainform.h

#--------------------------------------------------------------------------------
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
#--------------------------------------------------------------------------------
INCLUDEPATH += /home/evil/Programming/Qt/se-cnc/src/planner/reflexesII/inc
INCLUDEPATH += /home/evil/Programming/Qt/se-cnc/src/planner/reflexesII/src

FORMS += \
    mainform.ui
