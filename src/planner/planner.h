#ifndef PLANNER_H
#define PLANNER_H
///-----------------------------------------------------------------------------
#include <QVector>
///-----------------------------------------------------------------------------
#include "../reflexesII/RML/inc/ReflexxesAPI.h"
#include "../reflexesII/RML/inc/RMLPositionFlags.h"
#include "../reflexesII/RML/inc/RMLPositionInputParameters.h"
#include "../reflexesII/RML/inc/RMLPositionOutputParameters.h"
///-----------------------------------------------------------------------------
class PlannerClass
{
    private:
        unsigned int _reflexxesDegreesOfFreedom;
        double       _reflexxesCycleTimeInSeconds;
        int                          _reflexxesResultValue;
        ReflexxesAPI*                _REFLEXX_RML;
        RMLPositionInputParameters*  _REFLEXX_IP;
        RMLPositionOutputParameters* _REFLEXX_OP;
        RMLPositionFlags             _reflexxesFlags;

        void ReflexxesSetCurrentPosition(QVector<double> position);
        void ReflexxesSetCurrentVelocity(QVector<double> velocity);
        void ReflexxesSetCurrentAcceleration(QVector<double> acceleration);
        void ReflexxesSetTargetPosition(QVector<double> position);
        void ReflexxesSetTargetVelocity(QVector<double> velocity);
        void ReflexxesSetSelection(QVector<bool> enable);
        bool ReflexxesValidity();

    public:
        PlannerClass();
        PlannerClass(const PlannerClass&);
        ~PlannerClass();
};
///-----------------------------------------------------------------------------
#endif /// PLANNER_H
///-----------------------------------------------------------------------------
