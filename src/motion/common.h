///-----------------------------------------------------------------------------
#ifndef COMMON_H
#define COMMON_H
///-----------------------------------------------------------------------------
#include <math.h>
#include <locale.h>
///-----------------------------------------------------------------------------
#include "../ftdi/ftd2xx.h"
#include "../dsp/PC-DSP.h"
///-----------------------------------------------------------------------------
#include "hirestimer.h"
#include "motion_io.h"
///-----------------------------------------------------------------------------
/**
 *  @brief SE_MOTION State
 */
enum
{
    SE_MOTION_OK = 0,
    SE_MOTION_TIMEOUT,
    SE_MOTION_READY,
    SE_MOTION_ERROR
};
/**
 *  @brief Board Type
 */
enum
{
    BOARD_TYPE_UNKNOWN=0,
    BOARD_TYPE_KMOTION=1,
    BOARD_TYPE_KFLOP=2,
};
/**
 *  @brief SEMotionLocked Return Codes
 */
enum
{
    SE_MOTION_LOCKED = 0,        // (and token is locked) if KMotion is available for use
    SE_MOTION_IN_USE = 1,        // if already in use
    SE_MOTION_NOT_CONNECTED = 2, // if error or not able to connect
};
///-----------------------------------------------------------------------------
#endif /// COMMON_H
///-----------------------------------------------------------------------------
