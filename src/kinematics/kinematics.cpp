///-----------------------------------------------------------------------------
#include "kinematics.h"
//#include "kinematics3Rod.h"
//#include "kinematics5AxisGimbalAB.h"
//#include "kinematics5AxisGimbalCB.h"
//#include "kinematics5AxisTableAC.h"
//#include "kinematics5AxisTableBC.h"
//#include "kinematics5AxisTableAGimbalB.h"
//#include "kinematicsGeppetto.h"
//#include "kinematicsGeppettoExtrude.h"
///-----------------------------------------------------------------------------
#define sqr(x) ((x)*(x))
///-----------------------------------------------------------------------------
KINEMATICS_SPACE::CPT3D::CPT3D()
{
    y = 0.0;
    x = 0.0;
    z = 0.0;
    c = 0.0;
}
///-----------------------------------------------------------------------------
KINEMATICS_SPACE::CPT3D::~CPT3D()
{

}
///-----------------------------------------------------------------------------
KINEMATICS_SPACE::KinematicsClass::KinematicsClass()
{
    _motionParams.breakAngle = 30.0;
    _motionParams.tPLookahead = 3.0;

    _motionParams.maxAccelX = 1.0;
    _motionParams.maxAccelY = 1.0;
    _motionParams.maxAccelZ = 1.0;
    _motionParams.maxAccelA = 1.0;
    _motionParams.maxAccelB = 1.0;
    _motionParams.maxAccelC = 1.0;

    _motionParams.maxVelX = 1.0;
    _motionParams.maxVelY = 1.0;
    _motionParams.maxVelZ = 1.0;
    _motionParams.maxVelA = 1.0;
    _motionParams.maxVelB = 1.0;
    _motionParams.maxVelC = 1.0;



    _motionParams.maxRapidJerkC = 10.0;
    _motionParams.maxRapidJerkB = 10.0;
    _motionParams.maxRapidJerkA = 10.0;
    _motionParams.maxRapidJerkX = 10.0;
    _motionParams.maxRapidJerkY = 10.0;
    _motionParams.maxRapidJerkZ = 10.0;


    _motionParams.maxRapidAccelX = 1.0;
    _motionParams.maxRapidAccelY = 1.0;
    _motionParams.maxRapidAccelZ = 1.0;
    _motionParams.maxRapidAccelA = 1.0;
    _motionParams.maxRapidAccelB = 1.0;
    _motionParams.maxRapidAccelC = 1.0;


    _motionParams.maxRapidVelX = 1.0;
    _motionParams.maxRapidVelY = 1.0;
    _motionParams.maxRapidVelZ = 1.0;
    _motionParams.maxRapidVelA = 1.0;
    _motionParams.maxRapidVelB = 1.0;
    _motionParams.maxRapidVelC = 1.0;

    _motionParams.countsPerInchX = 100.0;
    _motionParams.countsPerInchY = 100.0;
    _motionParams.countsPerInchZ = 100.0;
    _motionParams.countsPerInchA = 100.0;
    _motionParams.countsPerInchB = 100.0;
    _motionParams.countsPerInchC = 100.0;

    _motionParams.maxLinearLength  = 1e30;  //Infinity for default case
    _motionParams.maxAngularChange = 1e30;  // limit the segment angle change for nonlinear systems
    _motionParams.maxRapidFRO = 1.0;
    _motionParams.collinearTol = 0.0002;
    _motionParams.cornerTol = 0.0002;
    _motionParams.facetAngle = 0.5;
    _motionParams.useOnlyLinearSegments = false;
    _motionParams.doRapidsAsFeeds = false;
    _motionParams.degreesA = false;
    _motionParams.degreesB = false;
    _motionParams.degreesC = false;

    _motionParams.softLimitNegX = -1e30;
    _motionParams.softLimitNegY = -1e30;
    _motionParams.softLimitNegZ = -1e30;
    _motionParams.softLimitNegA = -1e30;
    _motionParams.softLimitNegB = -1e30;
    _motionParams.softLimitNegC = -1e30;


    _motionParams.softLimitPosX = 1e30;
    _motionParams.softLimitPosY = 1e30;
    _motionParams.softLimitPosZ = 1e30;
    _motionParams.softLimitPosA = 1e30;
    _motionParams.softLimitPosB = 1e30;
    _motionParams.softLimitPosC = 1e30;

    _motionParams.TCP_Active = false;
    _motionParams.TCP_X = 0.0;
    _motionParams.TCP_Y = 0.0;
    _motionParams.TCP_Z = 0.0;

    _geoTableEnable = false;
    _geoTable       = nullptr;

}
///-----------------------------------------------------------------------------
KINEMATICS_SPACE::KinematicsClass::~KinematicsClass()
{
    if (_geoTable) delete [] _geoTable;
}
///-----------------------------------------------------------------------------
int KINEMATICS_SPACE::KinematicsClass::TransformCADtoActuators(
                                        double x,
                                        double y,
                                        double z,
                                        double a,
                                        double b,
                                        double c,
                                        double* Acts)
{
    if(!_geoTableEnable)
    {
        Acts[0] = x * _motionParams.countsPerInchX;
        Acts[1] = y * _motionParams.countsPerInchY;
        Acts[2] = z * _motionParams.countsPerInchZ;
        Acts[3] = a * _motionParams.countsPerInchA;
        Acts[4] = b * _motionParams.countsPerInchB;
        Acts[5] = c * _motionParams.countsPerInchC;
    }
    else
    {
        //GeoCorrect(x, y, z, &x, &y, &z);
    }
	return 0;
}
///-----------------------------------------------------------------------------
int KINEMATICS_SPACE::KinematicsClass::TransformActuatorstoCAD(
                                        double* Acts,
                                        double* x,
                                        double* y,
                                        double* z,
                                        double* a,
                                        double* b,
                                        double* c)
{
    if(!_geoTableEnable)
    {
        *x = Acts[0] / _motionParams.countsPerInchX;
        *y = Acts[1] / _motionParams.countsPerInchY;
        *z = Acts[2] / _motionParams.countsPerInchZ;
        *a = Acts[3] / _motionParams.countsPerInchA;
        *b = Acts[4] / _motionParams.countsPerInchB;
        *c = Acts[5] / _motionParams.countsPerInchC;
    }
    else
    {

    }
    return 0;
}
///-----------------------------------------------------------------------------
//int KINEMATICS_SPACE::KinematicsClass::solve(double *A,int N)
//{
//
//}
///-----------------------------------------------------------------------------
int KINEMATICS_SPACE::KinematicsClass::maxAccelInDirection(double dx,double dy,double dz,double da,double db,double dc,double *accel)
{
    double Max(1e99);
    double AccelToUse(1e99);

    double fdx = fabs(dx);
    double fdy = fabs(dy);
    double fdz = fabs(dz);
    double fda = fabs(da);
    double fdb = fabs(db);
    double fdc = fabs(dc);

    bool pure_angle;

    // compute total distance tool will move by considering both linear and angular movements

    double d = 0.0;
//	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &_motionParams, &pure_angle);

    // limit accel based on proportion in that direction
    if(pure_angle)
    {
        if(fda > 0 && _motionParams.maxAccelA < AccelToUse * fda / d) AccelToUse = _motionParams.maxAccelA * d / fda;
        if(fdb > 0 && _motionParams.maxAccelB < AccelToUse * fdb / d) AccelToUse = _motionParams.maxAccelB * d / fdb;
        if(fdc > 0 && _motionParams.maxAccelC < AccelToUse * fdc / d) AccelToUse = _motionParams.maxAccelC * d / fdc;
    }
    else
    {
        if(fdx > 0 && _motionParams.maxAccelX < AccelToUse * fdx / d) AccelToUse = _motionParams.maxAccelX * d / fdx;
        if(fdy > 0 && _motionParams.maxAccelY < AccelToUse * fdy / d) AccelToUse = _motionParams.maxAccelY * d / fdy;
        if(fdz > 0 && _motionParams.maxAccelZ < AccelToUse * fdz / d) AccelToUse = _motionParams.maxAccelZ * d / fdz;

        if(fda > 0)
        {
            Max = _motionParams.maxAccelA;
            if(Max < AccelToUse * fda / d) AccelToUse = Max * d / fda;
        }
        if(fdb > 0)
        {
            Max = _motionParams.maxAccelB;
            if(Max < AccelToUse * fdb / d) AccelToUse = Max * d / fdb;
        }
        if(fdc > 0)
        {
            Max = _motionParams.maxAccelC;
            if(Max < AccelToUse * fdc / d) AccelToUse = Max * d / fdc;
        }
    }
    *accel = AccelToUse;
    return 0;
}
///-----------------------------------------------------------------------------
int KINEMATICS_SPACE::KinematicsClass::maxRateInDirection(double dx,double dy,double dz,double da,double db,double dc,double *rate)
{
    double Max(1e99);
    double FeedRateToUse(1e99);

    double fdx = fabs(dx);
    double fdy = fabs(dy);
    double fdz = fabs(dz);
    double fda = fabs(da);
    double fdb = fabs(db);
    double fdc = fabs(dc);

    bool pure_angle;
    // compute total distance tool will move by considering both linear and angular movements
    double d(0.0);
//	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &_motionParams, &pure_angle);

    /// limit speeds based on proportion in that direction
    if(pure_angle)
    {
        if(fda > 0 && _motionParams.maxVelA < FeedRateToUse * fda / d) FeedRateToUse = _motionParams.maxVelA * d / fda;
        if(fdb > 0 && _motionParams.maxVelB < FeedRateToUse * fdb / d) FeedRateToUse = _motionParams.maxVelB * d / fdb;
        if(fdc > 0 && _motionParams.maxVelC < FeedRateToUse * fdc / d) FeedRateToUse = _motionParams.maxVelC * d / fdc;
    }
    else
    {
        if(fdx>0 && _motionParams.maxVelX < FeedRateToUse * fdx / d) FeedRateToUse = _motionParams.maxVelX * d / fdx;
        if(fdy>0 && _motionParams.maxVelY < FeedRateToUse * fdy / d) FeedRateToUse = _motionParams.maxVelY * d / fdy;
        if(fdz>0 && _motionParams.maxVelZ < FeedRateToUse * fdz / d) FeedRateToUse = _motionParams.maxVelZ * d / fdz;

        if(fda > 0)
        {
            Max = _motionParams.maxVelA;
            if(Max < FeedRateToUse * fda / d) FeedRateToUse = Max * d / fda;
        }
        if(fdb > 0)
        {
            Max = _motionParams.maxVelB;
            if(Max < FeedRateToUse * fdb / d) FeedRateToUse = Max * d / fdb;
        }
        if(fdc > 0)
        {
            Max = _motionParams.maxVelC;
            if(Max < FeedRateToUse * fdc / d) FeedRateToUse = Max * d / fdc;
        }
    }
    *rate = FeedRateToUse;
    return 0;
}
///-----------------------------------------------------------------------------
int KINEMATICS_SPACE::KinematicsClass::maxRapidRateInDirection(double dx,double dy,double dz,double da,double db,double dc,double *rate)
{
    bool   pure_angle;
    double Max(1e99);
    double FeedRateToUse(1e99);

    double fdx = fabs(dx);
    double fdy = fabs(dy);
    double fdz = fabs(dz);
    double fda = fabs(da);
    double fdb = fabs(db);
    double fdc = fabs(dc);

    double d(0.0);
//    double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &_motionParams, &pure_angle);

    // limit speeds based on proportion in that direction

    if (pure_angle)
    {
        if(fda > 0 && _motionParams.maxRapidVelA < FeedRateToUse * fda / d) FeedRateToUse = _motionParams.maxRapidVelA * d / fda;
        if(fdb > 0 && _motionParams.maxRapidVelB < FeedRateToUse * fdb / d) FeedRateToUse = _motionParams.maxRapidVelB * d / fdb;
        if(fdc > 0 && _motionParams.maxRapidVelC < FeedRateToUse * fdc / d) FeedRateToUse = _motionParams.maxRapidVelC * d / fdc;
    }
    else
    {
        if(fdx > 0 && _motionParams.maxRapidVelX < FeedRateToUse * fdx / d) FeedRateToUse = _motionParams.maxRapidVelX * d / fdx;
        if(fdy > 0 && _motionParams.maxRapidVelY < FeedRateToUse * fdy / d) FeedRateToUse = _motionParams.maxRapidVelY * d / fdy;
        if(fdz > 0 && _motionParams.maxRapidVelZ < FeedRateToUse * fdz / d) FeedRateToUse = _motionParams.maxRapidVelZ * d / fdz;

        if(fda > 0)
        {
            Max = _motionParams.maxRapidVelA;
            if(Max < FeedRateToUse * fda / d) FeedRateToUse = Max * d / fda;
        }
        if (fdb > 0)
        {
            Max = _motionParams.maxRapidVelB;
            if(Max < FeedRateToUse * fdb / d) FeedRateToUse = Max * d / fdb;
        }
        if (fdc > 0)
        {
            Max = _motionParams.maxRapidVelC;
            if(Max < FeedRateToUse * fdc / d) FeedRateToUse = Max * d / fdc;
        }
    }
    *rate = FeedRateToUse;
    return 0;
}
///-----------------------------------------------------------------------------
int KINEMATICS_SPACE::KinematicsClass::maxRapidJerkInDirection(double dx,double dy,double dz,double da,double db,double dc,double *jerk)
{
    double Max(1e99);
    double JerkToUse(1e99);

    double fdx = fabs(dx);
    double fdy = fabs(dy);
    double fdz = fabs(dz);
    double fda = fabs(da);
    double fdb = fabs(db);
    double fdc = fabs(dc);

    bool pure_angle;

    // compute total distance tool will move by considering both linear and angular movements

    double d(0.0);
//	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &_motionParams, &pure_angle);

    // limit Jerk based on proportion in that direction
    if(pure_angle)
    {
        if(fda > 0 && _motionParams.maxRapidJerkA < JerkToUse * fda / d) JerkToUse = _motionParams.maxRapidJerkA * d / fda;
        if(fdb > 0 && _motionParams.maxRapidJerkB < JerkToUse * fdb / d) JerkToUse = _motionParams.maxRapidJerkB * d / fdb;
        if(fdc > 0 && _motionParams.maxRapidJerkC < JerkToUse * fdc / d) JerkToUse = _motionParams.maxRapidJerkC * d / fdc;
    }
    else
    {
        if(fdx > 0 && _motionParams.maxRapidJerkX < JerkToUse * fdx / d) JerkToUse = _motionParams.maxRapidJerkX * d / fdx;
        if(fdy > 0 && _motionParams.maxRapidJerkY < JerkToUse * fdy / d) JerkToUse = _motionParams.maxRapidJerkY * d / fdy;
        if(fdz > 0 && _motionParams.maxRapidJerkZ < JerkToUse * fdz / d) JerkToUse = _motionParams.maxRapidJerkZ * d / fdz;

        if(fda>0)
        {
            Max = _motionParams.maxRapidJerkA;
            if (Max < JerkToUse * fda / d) JerkToUse = Max * d / fda;
        }
        if(fdb>0)
        {
            Max = _motionParams.maxRapidJerkB;
            if (Max < JerkToUse * fdb / d) JerkToUse = Max * d / fdb;
        }
        if(fdc>0)
        {
            Max = _motionParams.maxRapidJerkC;
            if (Max < JerkToUse * fdc / d) JerkToUse = Max * d / fdc;
        }
    }

    *jerk = JerkToUse;

    return 0;
}
///-----------------------------------------------------------------------------
int KINEMATICS_SPACE::KinematicsClass::maxRapidAccelInDirection(double dx,double dy,double dz,double da,double db,double dc,double *accel)
{
    double Max(1e99);
    double AccelToUse(1e99);

    double fdx = fabs(dx);
    double fdy = fabs(dy);
    double fdz = fabs(dz);
    double fda = fabs(da);
    double fdb = fabs(db);
    double fdc = fabs(dc);

    bool pure_angle;

    // compute total distance tool will move by considering both linear and angular movements
    double d(0.0);
//    double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &_motionParams, &pure_angle);

    // limit accel based on proportion in that direction
    if(pure_angle)
    {
        if(fda > 0 && _motionParams.maxRapidAccelA < AccelToUse * fda / d) AccelToUse = _motionParams.maxRapidAccelA * d / fda;
        if(fdb > 0 && _motionParams.maxRapidAccelB < AccelToUse * fdb / d) AccelToUse = _motionParams.maxRapidAccelB * d / fdb;
        if(fdc > 0 && _motionParams.maxRapidAccelC < AccelToUse * fdc / d) AccelToUse = _motionParams.maxRapidAccelC * d / fdc;
    }
    else
    {
        if(fdx > 0 && _motionParams.maxRapidAccelX < AccelToUse * fdx / d) AccelToUse = _motionParams.maxRapidAccelX * d / fdx;
        if(fdy > 0 && _motionParams.maxRapidAccelY < AccelToUse * fdy / d) AccelToUse = _motionParams.maxRapidAccelY * d / fdy;
        if(fdz > 0 && _motionParams.maxRapidAccelZ < AccelToUse * fdz / d) AccelToUse = _motionParams.maxRapidAccelZ * d / fdz;

        if(fda > 0)
        {
            Max = _motionParams.maxRapidAccelA;
            if(Max < AccelToUse * fda / d) AccelToUse = Max * d / fda;
        }
        if(fdb > 0)
        {
            Max = _motionParams.maxRapidAccelB;
            if(Max < AccelToUse * fdb / d) AccelToUse = Max * d / fdb;
        }
        if(fdc > 0)
        {
            Max = _motionParams.maxRapidAccelC;
            if(Max < AccelToUse * fdc / d) AccelToUse = Max * d / fdc;
        }
    }
    *accel = AccelToUse;
    return 0;
}
///-----------------------------------------------------------------------------
