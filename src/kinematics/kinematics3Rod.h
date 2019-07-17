///-----------------------------------------------------------------------------
#ifndef KINEMATICS_3_ROD_H
#define KINEMATICS_3_ROD_H
///-----------------------------------------------------------------------------
#include <math.h>
///-----------------------------------------------------------------------------
#include "kinematics.h"
///-----------------------------------------------------------------------------
/**
 * @brief The Kinematics3RodClass class
 */
class Kinematics3RodClass : public KinematicsClass
{
    public:
        //CPT3D Act0Center;
        //CPT3D Act1Center;
        //CPT3D Act2Center;
        double Act0Off;
        double Act1Off;
        double Act2Off;
        Kinematics3RodClass();
        virtual ~Kinematics3RodClass();
        virtual int TransformCADtoActuators(
                    double x,
                    double y,
                    double z,
                    double a,
                    double b,
                    double c,
                    double *Acts,
                    bool NoGeo = false
                );
        virtual int TransformActuatorstoCAD(
                    double *Acts,
                    double *x,
                    double *y,
                    double *z,
                    double *a,
                    double *b,
                    double *c,
                    bool NoGeo = false
                );
};
///-----------------------------------------------------------------------------
#endif /// KINEMATICS_3_ROD_H
///-----------------------------------------------------------------------------
