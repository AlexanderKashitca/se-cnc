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
namespace PLANNER_SPACE
{
    /**
     * @brief The PlannerClass class
     */
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

            void reflexxesSetCurrentPosition(QVector<double> position);
            void reflexxesSetCurrentVelocity(QVector<double> velocity);
            void reflexxesSetCurrentAcceleration(QVector<double> acceleration);
            void reflexxesSetTargetPosition(QVector<double> position);
            void reflexxesSetTargetVelocity(QVector<double> velocity);
            void reflexxesSetSelection(QVector<bool> enable);
            bool reflexxesValidity();

            bool _debug;

        public:
            PlannerClass();
            PlannerClass(const PlannerClass&);
            ~PlannerClass();

            void setDebug(bool enable);
            bool getDebug();
    };
} /// end PLANNER_SPACE
///-----------------------------------------------------------------------------
#endif /// PLANNER_H
///-----------------------------------------------------------------------------
