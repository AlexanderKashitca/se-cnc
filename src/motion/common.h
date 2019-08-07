///-----------------------------------------------------------------------------
#ifndef COMMON_H
#define COMMON_H
///-----------------------------------------------------------------------------
#include <math.h>
#include <locale.h>
#include <string.h>
///-----------------------------------------------------------------------------
/**
 *  @brief SE_MOTION State
 */
typedef enum
{
    SE_MOTION_OK = 0,
    SE_MOTION_TIMEOUT,
    SE_MOTION_READY,
    SE_MOTION_ERROR
}SE_MOTION_STATE;
/**
 *  @brief Board Type
 */
typedef enum
{
    BOARD_TYPE_UNKNOWN = 0,
    BOARD_TYPE_KMOTION = 1,
    BOARD_TYPE_KFLOP   = 2,
}SE_MOTION_BOARD_TYPE;
/**
 *  @brief SEMotionLocked Return Codes
 */
typedef enum
{
    SE_MOTION_LOCKED = 0,        // (and token is locked) if KMotion is available for use
    SE_MOTION_IN_USE = 1,        // if already in use
    SE_MOTION_NOT_CONNECTED = 2, // if error or not able to connect
}SE_MOTION_LOCK_STATE;
///-----------------------------------------------------------------------------
#endif /// COMMON_H
///-----------------------------------------------------------------------------
