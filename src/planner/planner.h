#ifndef PLANNER_H
#define PLANNER_H
///-----------------------------------------------------------------------------
#include <QVector>
#include <QVector3D>
///-----------------------------------------------------------------------------
#include "segmentation.h"
///-----------------------------------------------------------------------------
namespace PLANNER_SPACE
{

    typedef enum
    {
        PLANNER_OK   = 0,
        PLANNER_FAIL = 1,
        PLANNER_INVALID_PARAMETER,
        PLANNER_REFLEXXESS_ERROR
    } PLANNER_STATE;


    /**
     * @brief The PlannerClass class
     */
    class PlannerClass
    {
        private:

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

            SEGMENTATION_SPACE::SegmentPoint point;
            SEGMENTATION_SPACE::SegmentationClass segment;

            PLANNER_STATE moveStraightTraverse(double x,double y,double z,double a,double b,double c,double feed);

            void setDebug(bool enable);
            bool getDebug();
    };
} /// end PLANNER_SPACE
///-----------------------------------------------------------------------------
#endif /// PLANNER_H
///-----------------------------------------------------------------------------
