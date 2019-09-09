#ifndef PLANNER_H
#define PLANNER_H
///-----------------------------------------------------------------------------
#include <QVector>
#include <QVector3D>
///-----------------------------------------------------------------------------
#include "segmentation.h"
#include "../interpreter/canon.h"
///-----------------------------------------------------------------------------
namespace PLANNER_SPACE
{

    typedef enum
    {
        PLANNER_OK   = 0,
        PLANNER_FAIL = 1,
        PLANNER_INVALID_PARAMETER
    } PLANNER_STATE;



    /**
     * @brief The PlannerClass class
     */
    class PlannerClass
    {
        //private:
        public :

            double _length;
            double _x_delta;
            double _y_delta;
            double _z_delta;
            double _velocity;
            double _velocity_x;
            double _velocity_y;
            double _velocity_z;
            double _cos_alpha;
            double _cos_beta;
            double _cos_gamma;
            double _current_i;
            double _current_j;
            double _current_k;

            double _length_delta;
            double _angle_degree_delta;

            QVector<QVector3D> _coord_vector;
            QVector3D _coordinate;

            PLANNER_STATE calcStraightLength(double x_begin,double y_begin,double z_begin,double x_end,double y_end,double z_end);
            PLANNER_STATE calcStraightVelocity(double velocity);


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

            SEGMENTATION_SPACE::SEGMENT_POINT point;
            SEGMENTATION_SPACE::SegmentationClass segment;

            PLANNER_STATE moveStraight(double x,double y,double z,double feed);
            PLANNER_STATE moveArc(INTERPRETER_SPACE::CANON_PLANE plane,double x,double y,double z,double i,double j,double k,bool orientation,double feed);

            void setDebug(bool enable);
            bool getDebug();
    };
} /// end PLANNER_SPACE
///-----------------------------------------------------------------------------
#endif /// PLANNER_H
///-----------------------------------------------------------------------------
