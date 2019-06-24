///-----------------------------------------------------------------------------
/// Kinematics5AxisTableBC.h: interface for the CKinematics5AxisTableBC class.
///-----------------------------------------------------------------------------
#ifndef KINEMATICS_5_AXIS_TABLE_BC_H
#define KINEMATICS_5_AXIS_TABLE_BC_H
///-----------------------------------------------------------------------------
#include <math.h>
///-----------------------------------------------------------------------------
#include "kinematics.h"
///-----------------------------------------------------------------------------
class Kinematics5AxisTableBCClass : public KinematicsClass
{
public:
        Kinematics5AxisTableBCClass();
        virtual ~Kinematics5AxisTableBCClass();
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);
	int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);

	void Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
								double *xp,double *yp,double *zp);

        double CTableBZeroZZero;
        double BSaddleXCenterpoint;
        double BSaddleZCenterpoint;
        double CTableBZeroXCenterpoint;
        double CTableBZeroYCenterpoint;
};
///-----------------------------------------------------------------------------
#endif /// KINEMATICS_5_AXIS_TABLE_BC_H
///-----------------------------------------------------------------------------
