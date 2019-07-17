///-----------------------------------------------------------------------------
/// Kinematics5AxisTableAGimbalB.h: interface for the CKinematics5AxisTableAGimbalB class.
///-----------------------------------------------------------------------------
#ifndef KINEMATICS_5_AXIS_A_GIMBA_B_H
#define KINEMATICS_5_AXIS_A_GIMBA_B_H
///-----------------------------------------------------------------------------
#include <math.h>
///-----------------------------------------------------------------------------
//#include "kinematics.h"
///-----------------------------------------------------------------------------
class Kinematics5AxisTableAGimbalBClass : public KinematicsClass
{
public:
    Kinematics5AxisTableAGimbalBClass();
    virtual ~Kinematics5AxisTableAGimbalBClass();
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);
	int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);

	void Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
								double *xp,double *yp,double *zp);

	double PivotToChuckLength;
};
///-----------------------------------------------------------------------------
#endif /// KINEMATICS_5_AXIS_A_GIMBA_B_H
///-----------------------------------------------------------------------------
