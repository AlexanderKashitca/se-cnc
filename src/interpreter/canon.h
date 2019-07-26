///-----------------------------------------------------------------------------
#ifndef CANON_H
#define CANON_H
///-----------------------------------------------------------------------------
typedef enum
{
    CANON_PLANE_XY = 1,
    CANON_PLANE_YZ = 2,
    CANON_PLANE_XZ = 3
}CANON_PLANE;

typedef enum
{
    CANON_UNITS_INCHES = 1,
    CANON_UNITS_MM     = 2,
    CANON_UNITS_CM     = 3
}CANON_UNITS;

typedef enum
{
    CANON_EXACT_STOP = 1,
    CANON_EXACT_PATH = 2,
    CANON_CONTINUOUS = 3
}CANON_MOTION_MODE;

typedef enum
{
    CANON_SYNCHED     = 1,
    CANON_INDEPENDENT = 2
}CANON_SPEED_FEED_MODE;

typedef enum
{
    CANON_STOPPED          = 1,
    CANON_CLOCKWISE        = 2,
    CANON_COUNTERCLOCKWISE = 3
}CANON_DIRECTION;

typedef enum
{
    CANON_WORKPIECE = 1,
    CANON_XYZ = 2
}CANON_FEED_REFERENCE;

typedef enum
{
    CANON_SIDE_RIGHT = 1,
    CANON_SIDE_LEFT  = 2,
    CANON_SIDE_OFF   = 3
}CANON_SIDE;
typedef enum
{
    CANON_AXIS_X = 1,
    CANON_AXIS_Y = 2,
    CANON_AXIS_Z = 3,
    CANON_AXIS_A = 4,
    CANON_AXIS_B = 5,
    CANON_AXIS_C = 6
}CANON_AXIS;
/*
struct CANON_VECTOR
{
  CANON_VECTOR() {}
  CANON_VECTOR(double _x, double _y, double _z) {x = _x; y = _y; z = _z;}
  double x, y, z;
};
*/
/*
struct CANON_POSITION
{
  CANON_POSITION() {}
  CANON_POSITION(double _x,double _y,double _z,double _a,double _b,double _c)
  {
    x = _x; y = _y; z = _z;
    a = _a; b = _b; c = _c;
  }
  double x,y,z,a,b,c;
};
*/
/**
 * @brief CANON_TOOL_MAX - max size of carousel handled
 * @brief CANON_TOOL_ENTRY_LEN - how long each file line can be
 * @note Tools are numbered 1..CANON_TOOL_MAX, with tool 0 meaning no tool.
 */
const int CANON_TOOL_MAX       = 128;
const int CANON_TOOL_ENTRY_LEN = 256;

struct CANON_TOOL_TABLE
{
  int id;
  double length;
  double diameter;
};
///-----------------------------------------------------------------------------
#endif /// ifndef CANON_H
///-----------------------------------------------------------------------------
