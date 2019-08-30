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
#include "segmentation.h"
///-----------------------------------------------------------------------------
namespace PLANNER_SPACE
{

    typedef enum
    {
        PLANNER_OK   = 0,
        PLANNER_FAIL = 1,
        PLANNER_INVALID_PARAMETER
    } PLANNER_STATE;

    double const defaultCycleTimeInSeconds = 0.001;
    unsigned int const defaultDegreesOfFreedom = 3;


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

            PLANNER_STATE checkReflexxes();
            void          deleteReflexxes();
            PLANNER_STATE createReflexxes(unsigned int DegreesOfFreedom,double CycleTimeInSeconds);

            QVector<double> _reflexPosition;
            QVector<double> _reflexVelocity;
            QVector<double> _reflexAcceleration;
            QVector<double> _reflexJerk;
            QVector<bool>   _reflexEnable;

            void reflexxesSetCurrentPosition(QVector<double> position);
            void reflexxesSetCurrentVelocity(QVector<double> velocity);
            void reflexxesSetCurrentAcceleration(QVector<double> acceleration);
            void reflexxesSetTargetPosition(QVector<double> position);
            void reflexxesSetTargetVelocity(QVector<double> velocity);
            void reflexxesEnableAxis(QVector<bool> enable);


            void reflexxesSetMaxJerkVector(QVector<double> jerk);
            void reflexxesSetMaxVelocityVector(QVector<double> velocity);
            void reflexxesSetMaxAccelerationVector(QVector<double> acceleration);


            bool reflexxesValidity();

            bool _debug;



            double _liner_begin_x;
            double _liner_begin_y;
            double _liner_begin_z;
            double _liner_end_x;
            double _liner_end_y;
            double _liner_end_z;

            double _current_x;
            double _current_y;
            double _current_z;
            double _current_a;
            double _current_b;
            double _current_c;

            double _current_velocity;
            double _current_acceleration;

        public:
            PlannerClass();
            PlannerClass(const PlannerClass&);
            ~PlannerClass();



            PLANNER_STATE moveStraightTraverse(double x,double y,double z,double a,double b,double c,double feed);


            void setDebug(bool enable);
            bool getDebug();
    };
} /// end PLANNER_SPACE
///-----------------------------------------------------------------------------
#endif /// PLANNER_H
///-----------------------------------------------------------------------------
