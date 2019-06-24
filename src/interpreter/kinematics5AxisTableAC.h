///-----------------------------------------------------------------------------
/// Kinematics5AxisTableAC.h: interface for the CKinematics5AxisTableAC class.
///-----------------------------------------------------------------------------
#ifndef KINEMATICS_5_AXIS_TABLE_AC_H
#define KINEMATICS_5_AXIS_TABLE_AC_H
///-----------------------------------------------------------------------------
#include <math.h>
///-----------------------------------------------------------------------------
#include "kinematics.h"
///-----------------------------------------------------------------------------
class Kinematics5AxisTableACClass : public KinematicsClass
{
public:
    Kinematics5AxisTableACClass();
    virtual ~Kinematics5AxisTableACClass();
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);
	int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);

	void Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
								double *xp,double *yp,double *zp);

        double CTableAZeroZZero;
        double ASaddleYCenterpoint;
        double ASaddleZCenterpoint;
        double CTableAZeroXCenterpoint;
        double CTableAZeroYCenterpoint;
};
///-----------------------------------------------------------------------------
#endif /// KINEMATICS_5_AXIS_TABLE_AC_H
///-----------------------------------------------------------------------------
