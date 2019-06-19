///-----------------------------------------------------------------------------
#ifndef COMMON_H
#define COMMON_H
///-----------------------------------------------------------------------------
//#define DEBUG_DOWNLOAD  //enable this to capture download times
//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000
//
//#define WINVER _WIN32_WINNT_WINXP
//#define _WIN32_WINNT _WIN32_WINNT_WINXP
//#define _WIN32_IE _WIN32_WINNT_WINXP
//#define NTDDI_VERSION NTDDI_WINXP
// Insert your headers here
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#define _CRT_SECURE_NO_DEPRECATE 1
//#define NO_WARN_MBCS_MFC_DEPRECATION
//
//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000
// Insert your headers here
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//
//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#include <afxole.h>         // MFC OLE classes
//#include <afxodlgs.h>       // MFC OLE dialog classes
//#include <afxdisp.h>        // MFC OLE automation classes
//#include <afxpriv.h>		// for AfxLoadString
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT
//#include <afxmt.h>

//#include <conio.h>
//#include <mmsystem.h>
//#include "canon.h"
//#include "rs274ngc.h"
//#include "driver.h"
//#include "KMotionDLL.h"

//#include "Kinematics3Rod.h"
//#include "KinematicsGeppetto.h"
//#include "KinematicsGeppettoExtrude.h"
//#include "Kinematics5AxisGimbalAB.h"
//#include "Kinematics5AxisTableAGimbalB.h"
//#include "Kinematics5AxisGimbalCB.h"
//#include "Kinematics5AxisTableAC.h"
//#include "Kinematics5AxisTableBC.h"


#include <math.h>
#include <locale.h>
#include <stdint.h>

#include "canon.h"

#include "PT2D.h"
#include "PT3D.h"



#include "trajectoryPlanner.h"
#include "kinematics.h"

#include "gcodeinterpreter.h"

///-----------------------------------------------------------------------------
#endif // COMMON_H
///-----------------------------------------------------------------------------
