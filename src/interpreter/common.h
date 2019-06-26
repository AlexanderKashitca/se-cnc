///-----------------------------------------------------------------------------
#ifndef COMMON_H
#define COMMON_H
///-----------------------------------------------------------------------------
///#define DEBUG_DOWNLOAD  //enable this to capture download times
///-----------------------------------------------------------------------------
#ifndef THETA_SIGMA
    #define THETA_SIGMA 1e-13
#endif


#include "QtMath"

#include <math.h>
#include <locale.h>
#include <stdint.h>


#include "canon.h"
#include "PT2D.h"
#include "PT3D.h"


#include "kinematics3Rod.h"
#include "kinematicsGeppetto.h"
#include "kinematicsGeppettoExtrude.h"
#include "kinematics5AxisGimbalAB.h"
#include "kinematics5AxisTableAGimbalB.h"
#include "kinematics5AxisGimbalCB.h"
#include "kinematics5AxisTableAC.h"
#include "kinematics5AxisTableBC.h"

#include "kinematics.h"
#include "trajectoryPlanner.h"

#include "canon.h"
#include "rs274ngc.h"
#include "driver.h"

#include "../motion/motion.h"
#include "../motion/hirestimer.h"

///-----------------------------------------------------------------------------
#endif // COMMON_H
///-----------------------------------------------------------------------------
