///-----------------------------------------------------------------------------
#ifndef KINEMATICS_H
#define KINEMATICS_H
///-----------------------------------------------------------------------------
#include <QtMath>
#include <QString>
///-----------------------------------------------------------------------------
#include "../planner/motion_param.h"
///-----------------------------------------------------------------------------
namespace KINEMATICS_SPACE
{
    /**
     * @brief The CPT3D class
     */
    class CPT3D
    {
        public:
            double y;
            double x;
            double z;
            double c;
            CPT3D();
            virtual ~CPT3D();
    };
    ///-------------------------------------------------------------------------
    /**
     * @brief The CPT2D class
     */
    class CPT2D
    {
        public:
            double y;
            double x;
            CPT2D();
            virtual ~CPT2D();
    };
    ///-------------------------------------------------------------------------
    #define MAX_ACTUATORS 8
    ///-------------------------------------------------------------------------
    class KinematicsClass
    {
        private :
            bool    _geoTableEnable;
            CPT3D*  _geoTable;
        public :
            int maxRateInDirection(double dx,double dy,double dz,double da,double db,double dc,double *rate);
            int maxAccelInDirection(double dx,double dy,double dz,double da,double db,double dc,double *accel);

            int maxRapidRateInDirection(double dx,double dy,double dz,double da,double db,double dc,double *rate);
            int maxRapidJerkInDirection(double dx,double dy,double dz,double da,double db,double dc,double *jerk);
            int maxRapidAccelInDirection(double dx,double dy,double dz,double da,double db,double dc,double *accel);

            virtual int TransformCADtoActuators(double x,double y,double z,double a,double b,double c,double* Acts);
            virtual int TransformActuatorstoCAD(double* Acts,double* x,double* y,double* z,double* a,double* b,double* c);

            KinematicsClass();
            virtual ~KinematicsClass();

            PLANNER_SPACE::MOTION_PARAMS _motionParams;
    };
} /// end KINEMATICS_SPACE
///-----------------------------------------------------------------------------
#endif /// KINEMATICS_H
///-----------------------------------------------------------------------------
