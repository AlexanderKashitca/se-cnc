///-----------------------------------------------------------------------------
#include "rs274ngc.h"
///-----------------------------------------------------------------------------
using namespace INTERPRETER_SPACE;
///-----------------------------------------------------------------------------
#define ERM(error_code)                              \
if(1)                                                \
{                                                    \
    _setup.stack_index = 0;                          \
    strcpy(_setup.stack[_setup.stack_index++],name); \
    _setup.stack[_setup.stack_index][0] = 0;         \
    return error_code;                               \
} else

#define ERP(error_code)                              \
if(_setup.stack_index < 49)                          \
{                                                    \
    strcpy(_setup.stack[_setup.stack_index++],name); \
    _setup.stack[_setup.stack_index][0] = 0;         \
    return error_code;                               \
} else return error_code

#define CHK(bad,error_code)                          \
if(bad)                                              \
{                                                    \
    _setup.stack_index = 0;                          \
    strcpy(_setup.stack[_setup.stack_index++],name); \
    _setup.stack[_setup.stack_index][0] = 0;         \
    return error_code;                               \
} else

#define CHP(try_this)                                    \
if((status = (try_this)) != RS274NGC_OK)                 \
{                                                        \
    if(_setup.stack_index < 49)                          \
    {                                                    \
        strcpy(_setup.stack[_setup.stack_index++],name); \
        _setup.stack[_setup.stack_index][0] = 0;         \
        return status;                                   \
    }                                                    \
else {return status;}                                    \
} else


#define CYCLE_MACRO(call)                         \
for(repeat = block->l_number;repeat > 0;repeat--) \
{                                                 \
    aa = (aa + aa_increment);                     \
    bb = (bb + bb_increment);                     \
    cycle_traverse(plane,aa,bb,old_cc);        \
    if(old_cc != r)                               \
        cycle_traverse(plane,aa,bb,r);         \
    CHP(call);                                    \
    old_cc = clear_cc;                            \
}
///-----------------------------------------------------------------------------
/// Interpreter global arrays for g_codes && m_codes. The nth entry
/// in each array == the modal group number corresponding to the nth
/// code. Entries which are -1 represent illegal codes. Remember g_codes
/// in this interpreter are multiplied by 10.
///
/// The modal g groups && group numbers defined in [NCMS,pages 71 - 73]
/// (see also [Fanuc,pages 43 - 45]) are used here,except the canned
/// cycles (g80 - g89),which comprise modal g group 9 in [Fanuc],are
/// treated here as being in the same modal group (group 1) with the
/// straight moves && arcs (g0,g1,g2,g3).  [Fanuc,page 45] says only
/// one g_code from any one group may appear on a line,&& we are
/// following that rule. The straight_probe move,g38.2,== in group 1; it
/// is  ! defined in [NCMS].
///
/// Some g_codes are non-modal (g4,g10,g28,g30,g53,g92,g92.1,g92.2,
/// and g92.3 here - many more in [NCMS]). [Fanuc] && [NCMS] put all
/// these in the same group 0,so we do also. Logically,there are two
/// subgroups,those which require coordinate values (g10,g28,g30,and
/// g92) && those which do  ! (g4,g53,g92.1,g92.2,&& g92.3).
/// The subgroups are identified by itemization when necessary.
///
/// Those in group 0 which require coordinate values may  ! be on the
/// same line as those in group 1 (except g80) because they would be
/// competing for the coordinate values. Others in group 0 may be used on
/// the same line as those in group 1.
/// A total of 52 G-codes are implemented.
///
/// The groups are:
/// group  0 = {g4,g10,g28,g30,g53,g92,g92.1,g92.2,g92.3} - NON-MODAL
///             dwell,setup,return to ref1,return to ref2,
///         motion in machine coordinates,set && unset axis offsets
/// group  1 = {g0,g1,g2,g3,g38.2,g80,g81,g82,g83,g84,g85,g86,g87,g88,g89} - motion
/// group  2 = {g17,g18,g19}   - plane selection
/// group  3 = {g90,g91}       - distance mode
/// group  5 = {g93,g94}       - feed rate mode
/// group  6 = {g20,g21}       - units
/// group  7 = {g40,g41,g42}   - cutter diameter compensation
/// group  8 = {g43,g49}       - tool length offset
/// group 10 = {g98,g99}       - return mode in canned cycles
/// group 12 = {g54,g55,g56,g57,g58,g59,g59.1,g59.2,g59.3} - coordinate system
/// group 13 = {g61,g61.1,g64} - control mode
const int rs274ngcClass::_gees[] =
{
/*   0 */   1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/*  20 */   1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/*  40 */   0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/*  60 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/*  80 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 100 */   0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 120 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 140 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 160 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 180 */   2,-1,-1,-1,-1,-1,-1,-1,-1,-1,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 200 */   6,-1,-1,-1,-1,-1,-1,-1,-1,-1,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 220 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 240 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 260 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 280 */   0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 300 */   0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 320 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 340 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 360 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 380 */  -1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 400 */   7,-1,-1,-1,-1,-1,-1,-1,-1,-1,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 420 */   7,-1,-1,-1,-1,-1,-1,-1,-1,-1,8,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 440 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 460 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 480 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,8,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 500 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 520 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 540 */  12,-1,-1,-1,-1,-1,-1,-1,-1,-1,12,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 560 */  12,-1,-1,-1,-1,-1,-1,-1,-1,-1,12,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 580 */  12,-1,-1,-1,-1,-1,-1,-1,-1,-1,12,12,12,12,-1,-1,-1,-1,-1,-1,
/* 600 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,13,13,-1,-1,-1,-1,-1,-1,-1,-1,
/* 620 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 640 */  13,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 660 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 680 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 700 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 720 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 740 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 760 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 780 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 800 */   1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 820 */   1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 840 */   1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 860 */   1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 880 */   1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 900 */   3,-1,-1,-1,-1,-1,-1,-1,-1,-1,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 920 */   0,0,0,0,-1,-1,-1,-1,-1,-1,5,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 940 */   5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 960 */  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
/* 980 */  10,-1,-1,-1,-1,-1,-1,-1,-1,-1,10,-1,-1,-1,-1,-1,-1,-1,-1,-1
};
///-----------------------------------------------------------------------------
/// Modal groups && modal group numbers for M codes are  ! described in
/// [Fanuc]. We have used the groups from [NCMS] && added M60,as an
/// extension of the language for pallet shuttle && stop. This version has
/// no codes related to axis clamping.
/// The groups are:
///  group 4 = {m0,m1,m2,m30,m60} - stopping
///  group 6 = {m6}               - tool change
///  group 7 = {m3,m4,m5}         - spindle turning
///  group 8 = {m7,m8,m9}         - coolant
///  group 9 = {m48,m49}          - feed && speed override switch bypass
const int rs274ngcClass::_ems[] =
{
   4, 4, 4, 7, 7, 7, 6, 8, 8, 8,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   4,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1, 9, 9,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   4,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};
///-----------------------------------------------------------------------------
/// This == an array of the index numbers of system parameters that must
/// be included in a file used with the rs274ngc_restore_parameters function.
/// The array == used by that function && by the rs274ngc_save_parameters function.
const int rs274ngcClass::_required_parameters[] =
{
    5161,5162,5163,5164,5165,5166, /// G28 home
    5181,5182,5183,5184,5185,5186, /// G30 home
    5211,5212,5213,5214,5215,5216, /// G92 offsets
    5220,                          /// selected coordinate
    5221,5222,5223,5224,5225,5226, /// coordinate system 1
    5241,5242,5243,5244,5245,5246, /// coordinate system 2
    5261,5262,5263,5264,5265,5266, /// coordinate system 3
    5281,5282,5283,5284,5285,5286, /// coordinate system 4
    5301,5302,5303,5304,5305,5306, /// coordinate system 5
    5321,5322,5323,5324,5325,5326, /// coordinate system 6
    5341,5342,5343,5344,5345,5346, /// coordinate system 7
    5361,5362,5363,5364,5365,5366, /// coordinate system 8
    5381,5382,5383,5384,5385,5386, /// coordinate system 9
    RS274NGC_MAX_PARAMETERS
};
///-----------------------------------------------------------------------------
/// _readers == an array of pointers to functions that read.
/// It == used by read_one_item.
const read_function_pointer rs274ngcClass::_readers[] =
{
nullptr,     nullptr,     nullptr,nullptr,nullptr,nullptr,     nullptr,     nullptr,     nullptr,     nullptr,
nullptr,     nullptr,     nullptr,nullptr,nullptr,nullptr,     nullptr,     nullptr,     nullptr,     nullptr,
nullptr,     nullptr,     nullptr,nullptr,nullptr,nullptr,     nullptr,     nullptr,     nullptr,     nullptr,
nullptr,     nullptr,     nullptr,nullptr,nullptr,rs274ngcClass::read_parameter_setting,nullptr,     nullptr,     nullptr,     nullptr,
rs274ngcClass::read_comment,nullptr,     nullptr,nullptr,nullptr,nullptr,     nullptr,     nullptr,     nullptr,     nullptr,
nullptr,     nullptr,     nullptr,nullptr,nullptr,nullptr,     nullptr,     nullptr,     nullptr,     nullptr,
nullptr,     nullptr,     nullptr,nullptr,nullptr,nullptr,     nullptr,     nullptr,     nullptr,     nullptr,
nullptr,     nullptr,     nullptr,nullptr,nullptr,nullptr,     nullptr,     nullptr,     nullptr,     nullptr,
nullptr,     nullptr,     nullptr,nullptr,nullptr,nullptr,     nullptr,     nullptr,     nullptr,     nullptr,
nullptr,     nullptr,     nullptr,nullptr,nullptr,nullptr,     nullptr,     rs274ngcClass::read_a,rs274ngcClass::read_b,rs274ngcClass::read_c,
rs274ngcClass::read_d,      nullptr,     rs274ngcClass::read_f, rs274ngcClass::read_g, rs274ngcClass::read_h,rs274ngcClass::read_i,rs274ngcClass::read_j,rs274ngcClass::read_k,rs274ngcClass::read_l,rs274ngcClass::read_m,
nullptr,     nullptr,     rs274ngcClass::read_p, rs274ngcClass::read_q, rs274ngcClass::read_r,rs274ngcClass::read_s,rs274ngcClass::read_t,nullptr     ,nullptr,     nullptr,
rs274ngcClass::read_x,rs274ngcClass::read_y,rs274ngcClass::read_z};
///-----------------------------------------------------------------------------
static setup _setup;
///-----------------------------------------------------------------------------
/// static member this class
double rs274ngcClass::TOLERANCE_INCH = 0.0002;
double rs274ngcClass::TOLERANCE_MM   = 0.002;
/**
 * @brief rs274ngcClass::TOLERANCE_CONCAVE_CORNER
 * @note angle threshold for concavity for cutter compensation, in radians
 */
double rs274ngcClass::TOLERANCE_CONCAVE_CORNER = 0.01;
///-----------------------------------------------------------------------------
void rs274ngcClass::SetTolerance(double tolerance)
{
    if(CannonInOutClass::GetLengthUnitType() == CANON_UNITS_MM)
    {
        TOLERANCE_MM = tolerance;
    }
    else{
        TOLERANCE_INCH = tolerance;
    }
}
///-----------------------------------------------------------------------------
rs274ngcClass::rs274ngcClass()
{

}
///-----------------------------------------------------------------------------
rs274ngcClass::~rs274ngcClass()
{

}
///-----------------------------------------------------------------------------
/* arc_data_comp_ijk

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The two calculable values of the radius differ by more than
      tolerance: NCE_RADIUS_TO_END_OF_ARC_DIFFERS_FROM_RADIUS_TO_START
   2. move ==  ! G_2 or G_3: NCE_BUG_CODE_NOT_G2_OR_G3

Side effects:
   This finds && sets the values of center_x,center_y,&& turn.

Called by: convert_arc_comp1

This finds the center coordinates && number of full or partial turns
counterclockwise of a helical or circular arc in ijk-format in the XY
plane. The center == computed easily from the current point && center
offsets,which are given. It == checked that the end point lies one
tool radius from the arc.
*/
/**
 * @brief rs274ngcClass::arc_data_comp_ijk
 * @param move        - either G_2 (cw arc) or G_3 (ccw arc)
 * @param side        - either RIGHT or LEFT
 * @param tool_radius - radius of the tool
 * @param current_x   - first coordinate of current point
 * @param current_y   - second coordinate of current point
 * @param end_x       - first coordinate of arc end point
 * @param end_y       - second coordinate of arc end point
 * @param i_number    - first coordinate offset of center from current
 * @param j_number    - second coordinate offset of center from current
 * @param center_x    - pointer to first coordinate of center of arc
 * @param center_y    - pointer to second coordinate of center of arc
 * @param turn        - pointer to number of full or partial circles CCW
 * @param tolerance   - tolerance of differing radii
 * @return
 */
int rs274ngcClass::arc_data_comp_ijk(
    int move,int side,
    double tool_radius,
    double current_x,double current_y,
    double end_x,double end_y,
    double i_number,double j_number,
    double * center_x,double * center_y,
    int * turn,double tolerance)
{
    static char name[] = "arc_data_comp_ijk";
    double arc_radius;
    double radius2;

    *center_x = (current_x + i_number);
    *center_y = (current_y + j_number);
    arc_radius = hypot(i_number,j_number);
    radius2 = hypot((*center_x - end_x),(*center_y - end_y));
    radius2 = (((side == LEFT ) && (move == 30)) || ((side == RIGHT) && (move == 20))) ?
              (radius2 - tool_radius) : (radius2 + tool_radius);
    CHK((fabs(arc_radius - radius2) > tolerance),NCE_RADIUS_TO_END_OF_ARC_DIFFERS_FROM_RADIUS_TO_START);
      /* This catches an arc too small for the tool,also */
    if(move == G_2)
        *turn = -1;
    else
        if(move == G_3)
            *turn = 1;
        else
            ERM(NCE_BUG_CODE_NOT_G2_OR_G3);
    return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* arc_data_comp_r

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The arc radius == too small to reach the end point:
      NCE_RADIUS_TOO_SMALL_TO_REACH_END_POINT
   2. The arc radius ==  ! greater than the tool_radius,but should be:
      NCE_TOOL_RADIUS_NOT_LESS_THAN_ARC_RADIUS_WITH_COMP
   3. An imaginary value for offset would be found,which should never
      happen if the theory == correct: NCE_BUG_IN_TOOL_RADIUS_COMP

Side effects:
   This finds && sets the values of center_x,center_y,&& turn.

Called by: convert_arc_comp1

This finds the center coordinates && number of full or partial turns
counterclockwise of a helical or circular arc (call it arc1) in
r-format in the XY plane.  Arc2 == constructed so that it == tangent
to a circle whose radius == tool_radius && whose center == at the
point (current_x,current_y) && passes through the point (end_x,
end_y). Arc1 has the same center as arc2. The radius of arc1 == one
tool radius larger or smaller than the radius of arc2.

If the value of the big_radius argument == negative,that means [NCMS,
page 21] that an arc larger than a semicircle == to be made.
Otherwise,an arc of a semicircle or less == made.

The algorithm implemented here == to construct a line L from the
current point to the end point,&& a perpendicular to it from the
center of the arc which intersects L at point P. Since the distance
from the end point to the center && the distance from the current
point to the center are known,two equations for the length of the
perpendicular can be written. The right sides of the equations can be
set equal to one another && the resulting equation solved for the
length of the line from the current point to P. Then the location of
P,the length of the perpendicular,the angle of the perpendicular,
and the location of the center,can be found in turn.

This needs to be better documented,with figures. There are eight
possible arcs,since there are three binary possibilities: (1) tool
inside or outside arc,(2) clockwise or counterclockwise (3) two
positions for each arc (of the given radius) tangent to the tool
outline && through the end point. All eight are calculated below,
since theta,radius2,&& turn may each have two values.

To see two positions for each arc,imagine the arc == a hoop,the
tool == a cylindrical pin,&& the arc may rotate around the end point.
The rotation covers all possible positions of the arc. It == easy to
see the hoop == constrained by the pin at two different angles,whether
the pin == inside or outside the hoop.

*/

int rs274ngcClass::arc_data_comp_r( /* ARGUMENTS                                 */
 int move,          /* either G_2 (cw arc) or G_3 (ccw arc)             */
 int side,          /* either RIGHT or LEFT                             */
 double tool_radius,/* radius of the tool                               */
 double current_x,  /* first coordinate of current point                */
 double current_y,  /* second coordinate of current point               */
 double end_x,      /* first coordinate of arc end point                */
 double end_y,      /* second coordinate of arc end point               */
 double big_radius, /* radius of arc                                    */
 double * center_x, /* pointer to first coordinate of center of arc     */
 double * center_y, /* pointer to second coordinate of center of arc    */
 int * turn)         /* pointer to number of full or partial circles CCW */
{
  static char name[] = "arc_data_comp_r";
  double abs_radius; /* absolute value of big_radius          */
  double alpha;      /* direction of line from current to end */
  double distance;   /* length of line L from current to end  */
  double mid_length; /* length from current point to point P  */
  double offset;     /* length of line from P to center       */
  double radius2;    /* distance from center to current point */
  double mid_x;      /* x-value of point P                    */
  double mid_y;      /* y-value of point P                    */
  double theta;      /* direction of line from P to center    */

  abs_radius = fabs(big_radius);
  CHK(((abs_radius <= tool_radius) && (((side == LEFT ) && (move == G_3)) ||
                        ((side == RIGHT) && (move == G_2)))),
      NCE_TOOL_RADIUS_NOT_LESS_THAN_ARC_RADIUS_WITH_COMP);

  distance = hypot((end_x - current_x),(end_y - current_y));
  alpha = atan2 ((end_y - current_y),(end_x - current_x));
  theta = (((move == G_3) && (big_radius > 0)) ||
        ((move == G_2) && (big_radius < 0))) ?
          (alpha + PI2) : (alpha - PI2);
  radius2 = (((side == LEFT ) && (move == G_3)) ||
          ((side == RIGHT) && (move == G_2))) ?
            (abs_radius - tool_radius) : (abs_radius + tool_radius);
  CHK((distance > (radius2 + abs_radius)),
      NCE_RADIUS_TOO_SMALL_TO_REACH_END_POINT);
  mid_length = (((radius2 * radius2) + (distance * distance) -
              (abs_radius * abs_radius)) / (2.0 * distance));
  mid_x = (current_x + (mid_length * cos(alpha)));
  mid_y = (current_y + (mid_length * sin(alpha)));
  CHK(((radius2 * radius2) <= (mid_length * mid_length)),
      NCE_BUG_IN_TOOL_RADIUS_COMP);
  offset = sqrt((radius2 * radius2) - (mid_length * mid_length));
  *center_x = mid_x + (offset * cos(theta));
  *center_y = mid_y + (offset * sin(theta));
  *turn = (move == G_2) ? -1 : 1;

  return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* arc_data_ijk

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The two calculable values of the radius differ by more than
      tolerance: NCE_RADIUS_TO_END_OF_ARC_DIFFERS_FROM_RADIUS_TO_START
   2. The move code ==  ! G_2 or G_3: NCE_BUG_CODE_NOT_G2_OR_G3
   3. Either of the two calculable values of the radius == zero:
      NCE_ZERO_RADIUS_ARC

Side effects:
   This finds && sets the values of center_x,center_y,&& turn.

Called by:
   convert_arc2
   convert_arc_comp2

This finds the center coordinates && number of full or partial turns
counterclockwise of a helical or circular arc in ijk-format. This
function == used by convert_arc2 for all three planes,so "x" and
"y" really mean "first_coordinate" && "second_coordinate" wherever
they are used here as suffixes of variable names. The i && j prefixes
are handled similarly.

*/

int rs274ngcClass::arc_data_ijk( /* ARGUMENTS                                       */
 int move,              /* either G_2 (cw arc) or G_3 (ccw arc)            */
 double current_x,      /* first coordinate of current point               */
 double current_y,      /* second coordinate of current point              */
 double end_x,          /* first coordinate of arc end point               */
 double end_y,          /* second coordinate of arc end point              */
 double i_number,       /* first coordinate offset of center from current  */
 double j_number,       /* second coordinate offset of center from current */
 double * center_x,     /* pointer to first coordinate of center of arc    */
 double * center_y,     /* pointer to second coordinate of center of arc   */
 int * turn,            /* pointer to no. of full or partial circles CCW   */
 double tolerance)       /* tolerance of differing radii                    */
{
  static char name[] = "arc_data_ijk";
  double radius;    /* radius to current point */
  double radius2;   /* radius to end point     */
  *center_x = (current_x + i_number);
  *center_y = (current_y + j_number);
  radius = hypot((*center_x - current_x),(*center_y - current_y));
  radius2 = hypot((*center_x - end_x),(*center_y - end_y));
  CHK(((radius == 0.0) || (radius2 == 0.0)),NCE_ZERO_RADIUS_ARC);
  CHK((fabs(radius - radius2) > tolerance),
      NCE_RADIUS_TO_END_OF_ARC_DIFFERS_FROM_RADIUS_TO_START);
  if(move == G_2)
    *turn = -1;
  else if(move == G_3)
    *turn = 1;
  else
    ERM(NCE_BUG_CODE_NOT_G2_OR_G3);
  return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* arc_data_r

Returned Value: int
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. The radius == too small to reach the end point:
      NCE_ARC_RADIUS_TOO_SMALL_TO_REACH_END_POINT
   2. The current point == the same as the end point of the arc
      (so that it ==  ! possible to locate the center of the circle):
      NCE_CURRENT_POINT_SAME_AS_END_POINT_OF_ARC

Side effects:
   This finds && sets the values of center_x,center_y,&& turn.

Called by:
   convert_arc2
   convert_arc_comp2

This finds the center coordinates && number of full or partial turns
counterclockwise of a helical or circular arc in the r format. This
function == used by convert_arc2 for all three planes,so "x" and
"y" really mean "first_coordinate" && "second_coordinate" wherever
they are used here as suffixes of variable names.

If the value of the radius argument == negative,that means [NCMS,
page 21] that an arc larger than a semicircle == to be made.
Otherwise,an arc of a semicircle or less == made.

The algorithm used here == based on finding the midpoint M of the line
L between the current point && the end point of the arc. The center
of the arc lies on a line through M perpendicular to L.

*/

int rs274ngcClass::arc_data_r( /* ARGUMENTS                                     */
 int move,            /* either G_2 (cw arc) or G_3 (ccw arc)          */
 double current_x,    /* first coordinate of current point             */
 double current_y,    /* second coordinate of current point            */
 double end_x,        /* first coordinate of arc end point             */
 double end_y,        /* second coordinate of arc end point            */
 double radius,       /* radius of arc                                 */
 double * center_x,   /* pointer to first coordinate of center of arc  */
 double * center_y,   /* pointer to second coordinate of center of arc */
 int * turn)           /* pointer to no. of full or partial circles CCW */
{
  static char name[] = "arc_data_r";
  double abs_radius;  /* absolute value of given radius */
  double half_length; /* distance from M to end point   */
  double mid_x;       /* first coordinate of M          */
  double mid_y;       /* second coordinate of M         */
  double offset;      /* distance from M to center      */
  double theta;       /* angle of line from M to center */
  double turn2;       /* absolute value of half of turn */

  CHK(((end_x == current_x) && (end_y == current_y)),
      NCE_CURRENT_POINT_SAME_AS_END_POINT_OF_ARC);
  abs_radius = fabs(radius);
  mid_x = (end_x + current_x)/2.0;
  mid_y = (end_y + current_y)/2.0;
  half_length = hypot((mid_x - end_x),(mid_y - end_y));
  CHK(((half_length/abs_radius) > (1+TINY)),
      NCE_ARC_RADIUS_TOO_SMALL_TO_REACH_END_POINT);
  if((half_length/abs_radius) > (1-TINY))
    half_length = abs_radius; /* allow a small error for semicircle */
                                   /* check needed before calling asin   */
  if(((move == G_2) && (radius > 0)) ||
      ((move == G_3) && (radius < 0)))
    theta = atan2((end_y - current_y),(end_x - current_x)) - PI2;
  else
    theta = atan2((end_y - current_y),(end_x - current_x)) + PI2;

  turn2 = asin (half_length/abs_radius);
  offset = abs_radius * cos(turn2);
  *center_x = mid_x + (offset * cos(theta));
  *center_y = mid_y + (offset * sin(theta));
  *turn = (move == G_2) ? -1 : 1;

  return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* check_g_codes

Returned Value: int
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. NCE_DWELL_TIME_MISSING_WITH_G4
   2. NCE_MUST_USE_G0_OR_G1_WITH_G53
   3. NCE_CANNOT_USE_G53_INCREMENTAL
   4. NCE_LINE_WITH_G10_DOES_NOT_HAVE_L2
   5. NCE_P_VALUE_NOT_AN_INTEGER_WITH_G10_L2
   6. NCE_P_VALUE_OUT_OF_RANGE_WITH_G10_L2
   7. NCE_BUG_BAD_G_CODE_MODAL_GROUP_0

Side effects: none

Called by: check_items

This runs checks on g_codes from a block of RS274/NGC instructions.
Currently,all checks are on g_codes in modal group 0.

The read_g function checks for errors which would foul up the reading.
The enhance_block function checks for logical errors in the use of
axis values by g-codes in modal groups 0 && 1.
This function checks for additional logical errors in g_codes.

[Fanuc,page 45,note 4] says there == no maximum for how many g_codes
may be put on the same line,[NCMS] says nothing one way or the other,
so the test for that ==  ! used.

We are suspending any implicit motion g_code when a g_code from our
group 0 == used.  The implicit motion g_code takes effect again
automatically after the line on which the group 0 g_code occurs.  It
is  ! clear what the intent of [Fanuc] == in this regard. The
alternative == to require that any implicit motion be explicitly
cancelled.

 ! all checks on g_codes are included here. Those checks that are
sensitive to whether other g_codes on the same line have been executed
yet are made by the functions called by convert_g.

Our reference sources differ regarding what codes may be used for
dwell time.  [Fanuc,page 58] says use "p" or "x". [NCMS,page 23] says
use "p","x",or "u". We are allowing "p" only,since it == consistent
with both sources && "x" would be confusing. However,"p" == also used
with G10,where it must be an integer,so reading "p" values == a bit
more trouble than would be nice.

*/
/**
 * @brief rs274ngcClass::check_g_codes
 * @param block    - pointer to a block to be checked
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::check_g_codes(block_pointer block,setup_pointer settings)
{
    static char name[] = "check_g_codes";
    int mode0;
    int p_int;
    mode0 = block->g_modes[0];
    if(mode0 == -1)
    {}
    else if(mode0 == G_4)
    {
        CHK((block->p_number == -1.0),NCE_DWELL_TIME_MISSING_WITH_G4);
    }
    else if(mode0 == G_10)
    {
        p_int = (int)(block->p_number + 0.0001);
        CHK((block->l_number != 2),NCE_LINE_WITH_G10_DOES_NOT_HAVE_L2);
        CHK((((block->p_number + 0.0001) - p_int) > 0.0002),NCE_P_VALUE_NOT_AN_INTEGER_WITH_G10_L2);
        CHK(((p_int < 1) || (p_int > 9)),NCE_P_VALUE_OUT_OF_RANGE_WITH_G10_L2);
    }
    else if(mode0 == G_28)
    {}
    else if(mode0 == G_30)
    {}
    else if(mode0 == G_53)
    {
        CHK(((block->motion_to_be != G_0) && (block->motion_to_be != G_1)),NCE_MUST_USE_G0_OR_G1_WITH_G53);
        CHK(((block->g_modes[3] == G_91) || ((block->g_modes[3] != G_90)  && (settings->distance_mode == MODE_INCREMENTAL))),NCE_CANNOT_USE_G53_INCREMENTAL);
    }
    else if(mode0 == G_92)
    {}
    else if((mode0 == G_92_1) || (mode0 == G_92_2) || (mode0 == G_92_3))
    {}
    else
        ERM(NCE_BUG_BAD_G_CODE_MODAL_GROUP_0);
    return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* check_items

Returned Value: int
   If any one of check_g_codes,check_m_codes,&& check_other_codes
   returns an error code,this returns that code.
   Otherwise,it returns RS274NGC_OK.

Side effects: none

Called by: parse_line

This runs checks on a block of RS274 code.

The functions named read_XXXX check for errors which would foul up the
reading. This function checks for additional logical errors.

A block has an array of g_codes,which are initialized to -1
(meaning no code). This calls check_g_codes to check the g_codes.

A block has an array of m_codes,which are initialized to -1
(meaning no code). This calls check_m_codes to check the m_codes.

Items in the block which are  ! m or g codes are checked by
check_other_codes.

*/
/**
 * @brief rs274ngcClass::check_items
 * @param block    - pointer to a block to be checked
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::check_items(block_pointer block,setup_pointer settings)
{
    static char name[] = "check_items";
    int status;
    CHP(check_g_codes(block,settings));
    CHP(check_m_codes(block));
    CHP(check_other_codes(block));
    return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* check_m_codes

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. There are too many m codes in the block: NCE_TOO_MANY_M_CODES_ON_LINE

Side effects: none

Called by: check_items

This runs checks on m_codes from a block of RS274/NGC instructions.

The read_m function checks for errors which would foul up the
reading. This function checks for additional errors in m_codes.

*/
/**
 * @brief rs274ngcClass::check_m_codes
 * @param block - pointer to a block to be checked
 * @return
 */
int rs274ngcClass::check_m_codes(block_pointer block)
{
    static char name[] = "check_m_codes";
    CHK((block->m_count > MAX_EMS),NCE_TOO_MANY_M_CODES_ON_LINE);
    return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* check_other_codes

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. An A-axis value == given with a canned cycle (g80 to g89):
      NCE_CANNOT_PUT_AN_A_IN_CANNED_CYCLE
   2. A B-axis value == given with a canned cycle (g80 to g89):
      NCE_CANNOT_PUT_A_B_IN_CANNED_CYCLE
   3. A C-axis value == given with a canned cycle (g80 to g89):
      NCE_CANNOT_PUT_A_C_IN_CANNED_CYCLE
   4. A d word == in a block with no cutter_radius_compensation_on command:
      NCE_D_WORD_WITH_NO_G41_OR_G42
   5. An h_number == in a block with no tool length offset setting:
      NCE_H_WORD_WITH_NO_G43
   6. An i_number == in a block with no G code that uses it:
      NCE_I_WORD_WITH_NO_G2_OR_G3_OR_G87_TO_USE_IT
   7. A j_number == in a block with no G code that uses it:
      NCE_J_WORD_WITH_NO_G2_OR_G3_OR_G87_TO_USE_IT
   8. A k_number == in a block with no G code that uses it:
      NCE_K_WORD_WITH_NO_G2_OR_G3_OR_G87_TO_USE_IT
   9. A l_number == in a block with no G code that uses it:
      NCE_L_WORD_WITH_NO_CANNED_CYCLE_OR_G10
  10. A p_number == in a block with no G code that uses it:
      NCE_P_WORD_WITH_NO_G4_G10_G82_G86_G88_G89
  11. A q_number == in a block with no G code that uses it:
      NCE_Q_WORD_WITH_NO_G83
  12. An r_number == in a block with no G code that uses it:
      NCE_R_WORD_WITH_NO_G_CODE_THAT_USES_IT

Side effects: none

Called by: check_items

This runs checks on codes from a block of RS274/NGC code which are
 ! m or g codes.

The functions named read_XXXX check for errors which would foul up the
reading. This function checks for additional logical errors in codes.

*/
/**
 * @brief rs274ngcClass::check_other_codes
 * @param block - pointer to a block of RS274/NGC instructions
 * @return
 */
int rs274ngcClass::check_other_codes(block_pointer block)
{
    static char name[] = "check_other_codes";
    int motion;

    motion = block->motion_to_be;
    if(block->a_flag != OFF)
    {
        CHK(((block->g_modes[1] > G_80) && (block->g_modes[1] < G_90)),NCE_CANNOT_PUT_AN_A_IN_CANNED_CYCLE);
    }
    if(block->b_flag != OFF)
    {
      CHK(((block->g_modes[1] > G_80) && (block->g_modes[1] < G_90)),NCE_CANNOT_PUT_A_B_IN_CANNED_CYCLE);
    }
    if(block->c_flag != OFF)
    {
      CHK(((block->g_modes[1] > G_80) && (block->g_modes[1] < G_90)),NCE_CANNOT_PUT_A_C_IN_CANNED_CYCLE);
    }
    if(block->d_number != -1)
    {
        CHK(((block->g_modes[7] != G_41) && (block->g_modes[7] != G_42)),NCE_D_WORD_WITH_NO_G41_OR_G42);
    }
    if(block->h_number != -1)
    {
        CHK((block->g_modes[8] != G_43),NCE_H_WORD_WITH_NO_G43);
    }
    if(block->i_flag == ON) /* could still be useless if yz_plane arc */
    {
        CHK(((motion != G_2) && (motion != G_3) && (motion != G_87)),NCE_I_WORD_WITH_NO_G2_OR_G3_OR_G87_TO_USE_IT);
    }
    if(block->j_flag == ON) /* could still be useless if xz_plane arc */
    {
        CHK(((motion != G_2) && (motion != G_3) && (motion != G_87)),NCE_J_WORD_WITH_NO_G2_OR_G3_OR_G87_TO_USE_IT);
    }
    if(block->k_flag == ON) /* could still be useless if xy_plane arc */
    {
      CHK(((motion != G_2) && (motion != G_3) && (motion != G_87)),NCE_K_WORD_WITH_NO_G2_OR_G3_OR_G87_TO_USE_IT);
    }
    if(block->l_number != -1)
    {
        CHK((((motion < G_81) || (motion > G_89)) && (block->g_modes[0] != G_10)),NCE_L_WORD_WITH_NO_CANNED_CYCLE_OR_G10);
    }
    if(block->p_number != -1.0)
    {
        CHK(
            ((block->g_modes[0] != G_10) &&
            (block->g_modes[0] != G_4) &&
            (motion != G_82) && (motion != G_86) &&
            (motion != G_88) && (motion != G_89)),
            NCE_P_WORD_WITH_NO_G4_G10_G82_G86_G88_G89);
    }
    if(block->q_number != -1.0)
    {
        CHK((motion != G_83),NCE_Q_WORD_WITH_NO_G83);
    }
    if(block->r_flag == ON)
    {
        CHK(
            (((motion != G_2) && (motion != G_3))  &&
             ((motion < G_81) || (motion > G_89))),
            NCE_R_WORD_WITH_NO_G_CODE_THAT_USES_IT);
    }
    return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* close_and_downcase

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. A left parenthesis == found inside a comment:
      NCE_NESTED_COMMENT_FOUND
   2. The line ends before an open comment == closed:
      NCE_UNCLOSED_COMMENT_FOUND
   3. A newline character == found that ==  ! followed by null:
      NCE_NULL_MISSING_AFTER_NEWLINE

Side effects: See below

Called by:  read_text

To simplify handling upper case letters,spaces,&& tabs,this
function removes spaces && and tabs && downcases everything on a
line which ==  ! part of a comment.

Comments are left unchanged in place. Comments are anything
enclosed in parentheses. Nested comments,indicated by a left
parenthesis inside a comment,are illegal.

The line must have a null character at the end when it comes in.
The line may have one newline character just before the end. If
there == a newline,it will be removed.

Although this software system detects && rejects all illegal characters
and illegal syntax,this particular function does  ! detect problems
with anything but comments.

We are treating RS274 code here as case-insensitive && spaces and
tabs as if they have no meaning. [RS274D,page 6] says spaces && tabs
are to be ignored by control.

The KT && NGC manuals say nothing about case or spaces && tabs.

*/

int rs274ngcClass::close_and_downcase( /* ARGUMENTS                   */
 char * line)                  /* string: one line of NC code */
{
  static char name[] = "close_and_downcase";
  int m;
  int n;
  int comment;
  char item;
  comment = 0;
  for(n = 0,m = 0; (item = line[m]) != (char) NULL; m++)
    {
      if(comment)
    {
      line[n++] = item;
      if(item == ')')
        {
          comment = 0;
        }
      else if(item == '(')
        ERM(NCE_NESTED_COMMENT_FOUND);
    }
      else if((item == ' ') || (item == '\t') || (item == '\r'));
                                      /* don't copy blank or tab or CR */
      else if(item == '\n')          /* don't copy newline            */
    {                             /* but check null follows        */
      CHK((line[m+1] != 0),NCE_NULL_MISSING_AFTER_NEWLINE);
    }
      else if((64 < item) && (item < 91)) /* downcase upper case letters */
    {
      line[n++] = (32 + item);
    }
      else if(item == '(')   /* comment == starting */
    {
      comment = 1;
      line[n++] = item;
    }
      else
    {
      line[n++] = item; /* copy anything else */
    }
    }
  CHK((comment),NCE_UNCLOSED_COMMENT_FOUND);
  line[n] = 0;
  return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* convert_arc

Returned Value: int
   If one of the following functions returns an error code,
   this returns that error code.
      convert_arc_comp1
      convert_arc_comp2
      convert_arc2
   If any of the following errors occur,this returns the error code shown.
   Otherwise,this returns RS274NGC_OK.
   1. The block has neither an r value nor any i,j,k values:
      NCE_R_I_J_K_WORDS_ALL_MISSING_FOR_ARC
   2. The block has both an r value && one or more i,j,k values:
      NCE_MIXED_RADIUS_IJK_FORMAT_FOR_ARC
   3. In the ijk format the XY-plane == selected and
      the block has a k value: NCE_K_WORD_GIVEN_FOR_ARC_IN_XY_PLANE
   4. In the ijk format the YZ-plane == selected and
      the block has an i value: NCE_I_WORD_GIVEN_FOR_ARC_IN_YZ_PLANE
   5. In the ijk format the XZ-plane == selected and
      the block has a j value: NCE_J_WORD_GIVEN_FOR_ARC_IN_XZ_PLANE
   6. In either format any of the following occurs.
      a. The XY-plane == selected && the block has no x or y value:
         NCE_X_AND_Y_WORDS_MISSING_FOR_ARC_IN_XY_PLANE
      b. The YZ-plane == selected && the block has no y or z value:
         NCE_Y_AND_Z_WORDS_MISSING_FOR_ARC_IN_YZ_PLANE
      c. The ZX-plane == selected && the block has no z or x value:
         NCE_X_AND_Z_WORDS_MISSING_FOR_ARC_IN_XZ_PLANE
   7. The selected plane == an unknown plane:
      NCE_BUG_PLANE_NOT_XY_YZ__OR_XZ
   8. The feed rate mode == UNITS_PER_MINUTE && feed rate == zero:
      NCE_CANNOT_MAKE_ARC_WITH_ZERO_FEED_RATE
   9. The feed rate mode == INVERSE_TIME && the block has no f word:
      NCE_F_WORD_MISSING_WITH_INVERSE_TIME_ARC_MOVE

Side effects:
   This generates && executes an arc command at feed rate
   (and,possibly a second arc command). It also updates the setting
   of the position of the tool point to the end point of the move.

Called by: convert_motion.

This converts a helical or circular arc.  The function calls:
convert_arc2 (when cutter radius compensation == off) or
convert_arc_comp1 (when cutter comp == on && this == the first move) or
convert_arc_comp2 (when cutter comp == on && this ==  ! the first move).

If the ijk format == used,at least one of the offsets in the current
plane must be given in the block; it == common but  ! required to
give both offsets. The offsets are always incremental [NCMS,page 21].

*/
/**
 * @brief rs274ngcClass::convert_arc
 * @param move - either G_2 (cw arc) or G_3 (ccw arc)
 * @param block - pointer to a block of RS274 instructions
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_arc(int move,block_pointer block,setup_pointer settings)
{
    static char name[] = "convert_arc";
    int status;
    int first;          /* flag set ON if this == first move after comp ON */
    int ijk_flag;       /* flag set ON if any of i,j,k present in NC code  */
    double end_x;
    double end_y;
    double end_z;
    double AA_end;
    double BB_end;
    double CC_end;

    ijk_flag = ((block->i_flag || block->j_flag) || block->k_flag) ? ON : OFF;
    first = (settings->program_x == UNKNOWN);

    CHK(((block->r_flag != ON) && (ijk_flag != ON)),NCE_R_I_J_K_WORDS_ALL_MISSING_FOR_ARC);
    CHK(((block->r_flag == ON) && (ijk_flag == ON)),NCE_MIXED_RADIUS_IJK_FORMAT_FOR_ARC);
    if(settings->feed_mode == UNITS_PER_MINUTE)
    {
        CHK((settings->feed_rate == 0.0),NCE_CANNOT_MAKE_ARC_WITH_ZERO_FEED_RATE);
    }
    else
        if(settings->feed_mode == INVERSE_TIME)
        {
            CHK((block->f_number == -1.0),NCE_F_WORD_MISSING_WITH_INVERSE_TIME_ARC_MOVE);
        }
    if(ijk_flag)
    {
        if(settings->plane == CANON_PLANE_XY)
        {
            CHK((block->k_flag),NCE_K_WORD_GIVEN_FOR_ARC_IN_XY_PLANE);
            if(block->i_flag == OFF) /* i or j flag on to get here */
                block->i_number = 0.0;
            else if(block->j_flag == OFF)
                block->j_number = 0.0;
        }
        else if(settings->plane == CANON_PLANE_YZ)
        {
            CHK((block->i_flag),NCE_I_WORD_GIVEN_FOR_ARC_IN_YZ_PLANE);
            if(block->j_flag == OFF) /* j or k flag on to get here */
                block->j_number = 0.0;
            else if(block->k_flag == OFF)
                block->k_number = 0.0;
        }
        else if(settings->plane == CANON_PLANE_XZ)
        {
            CHK((block->j_flag),NCE_J_WORD_GIVEN_FOR_ARC_IN_XZ_PLANE);
            if(block->i_flag == OFF) /* i or k flag on to get here */
                block->i_number = 0.0;
            else if(block->k_flag == OFF)
                block->k_number = 0.0;
        }
        else
            ERM(NCE_BUG_PLANE_NOT_XY_YZ_OR_XZ);
    }
    else; /* r format arc; no other checks needed specific to this format */

    if(settings->plane == CANON_PLANE_XY) /* checks for both formats */
    {
        CHK(((block->x_flag == OFF) && (block->y_flag == OFF)),NCE_X_AND_Y_WORDS_MISSING_FOR_ARC_IN_XY_PLANE);
    }
    else if(settings->plane == CANON_PLANE_YZ)
    {
        CHK(((block->y_flag == OFF) && (block->z_flag == OFF)),NCE_Y_AND_Z_WORDS_MISSING_FOR_ARC_IN_YZ_PLANE);
    }
    else if(settings->plane == CANON_PLANE_XZ)
    {
        CHK(((block->x_flag == OFF) && (block->z_flag == OFF)),NCE_X_AND_Z_WORDS_MISSING_FOR_ARC_IN_XZ_PLANE);
    }
    find_ends(block,settings,&end_x,&end_y,&end_z,&AA_end,&BB_end,&CC_end);
    settings->motion_mode = move;
    if(settings->plane == CANON_PLANE_XY)
    {
        if((settings->cutter_comp_side == OFF) || (settings->cutter_comp_radius == 0.0))
        {
            status = convert_arc2(move,block,settings,
                                    &(settings->current_x),
                                    &(settings->current_y),
                                    &(settings->current_z),
                                    end_x,end_y,end_z,
                                    AA_end,BB_end,CC_end,
                                    block->i_number,
                                    block->j_number);
            CHP(status);
        }
        else if(first)
        {
            status = convert_arc_comp1(move,block,settings,end_x,end_y,end_z,AA_end,BB_end,CC_end);
            CHP(status);
        }
        else
        {
            status = convert_arc_comp2(move,block,settings,end_x,end_y,end_z,AA_end,BB_end,CC_end);
            CHP(status);
        }
    }
    else if(settings->plane == CANON_PLANE_XZ)
    {
        status = convert_arc2(move,block,settings,
                    &(settings->current_z),&(settings->current_x),
                    &(settings->current_y),end_z,end_x,
                    end_y,AA_end,BB_end,CC_end,
                    block->k_number,
                    block->i_number);
        CHP(status);
    }
    else if(settings->plane == CANON_PLANE_YZ)
    {
        status = convert_arc2(move,block,settings,
                    &(settings->current_y),&(settings->current_z),
                    &(settings->current_x),end_y,end_z,
                    end_x,AA_end,BB_end,CC_end
                    ,block->j_number,block->k_number);
        CHP(status);
    }
    else
        ERM(NCE_BUG_PLANE_NOT_XY_YZ_OR_XZ);
    return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* convert_arc2

Returned Value: int
   If arc_data_ijk or arc_data_r returns an error code,
   this returns that code.
   Otherwise,it returns RS274NGC_OK.

Side effects:
   This executes an arc command at feed rate. It also updates the
   setting of the position of the tool point to the end point of the move.
   If inverse time feed rate == in effect,it also resets the feed rate.

Called by: convert_arc.

This converts a helical or circular arc.

*/
/**
 * @brief rs274ngcClass::convert_arc2
 * @param move     - either G_2 (cw arc) or G_3 (ccw arc)
 * @param block    - pointer to a block of RS274 instructions
 * @param settings - pointer to machine settings
 * @param current1 - pointer to current value of coordinate 1
 * @param current2 - pointer to current value of coordinate 2
 * @param current3 - pointer to current value of coordinate 3
 * @param end1     - coordinate 1 value at end of arc
 * @param end2     - coordinate 2 value at end of arc
 * @param end3     - coordinate 3 value at end of arc
 * @param AA_end   - a-value at end of arc
 * @param BB_end   - b-value at end of arc
 * @param CC_end   - c-value at end of arc
 * @param offset1  - offset of center from current1
 * @param offset2  - offset of center from current2
 * @return
 */
int rs274ngcClass::convert_arc2(
        int move,block_pointer block,setup_pointer settings,
        double* current1,double* current2,double* current3,
        double end1,double end2,double end3,
        double AA_end,double BB_end,double CC_end,
        double offset1,double offset2)
{
    static char name[] = "convert_arc2";
    double center1;
    double center2;
    int status;         /* status returned from CHP function call     */
    double tolerance;   /* tolerance for difference of radii          */
    int turn;           /* number of full or partial turns CCW in arc */

    tolerance = (settings->length_units == CANON_UNITS_INCHES) ? TOLERANCE_INCH : TOLERANCE_MM;
    if(block->r_flag)
    {
        CHP(arc_data_r(move,*current1,*current2,end1,end2,block->r_number,&center1,&center2,&turn));
    }
    else
    {
        CHP(arc_data_ijk(move,*current1,*current2,end1,end2,offset1,offset2,&center1,&center2,&turn,tolerance));
    }

  if(settings->feed_mode == INVERSE_TIME)
    inverse_time_rate_arc(*current1,*current2,*current3,center1,center2,turn,end1,end2,end3,block,settings);
  CannonInOutClass::ArcFeed(end1,end2,center1,center2,turn,end3,AA_end,BB_end,CC_end);
  *current1 = end1;
  *current2 = end2;
  *current3 = end3;

  settings->AA_current = AA_end;
  settings->BB_current = BB_end;
  settings->CC_current = CC_end;

  return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* convert_arc_comp1

Returned Value: int
   If arc_data_comp_ijk or arc_data_comp_r returns an error code,
   this returns that code.
   Otherwise,it returns RS274NGC_OK.

Side effects:
   This executes an arc command at
   feed rate. It also updates the setting of the position of
   the tool point to the end point of the move.

Called by: convert_arc.

This function converts a helical or circular arc,generating only one
arc. The axis must be parallel to the z-axis. This == called when
cutter radius compensation == on && this == the first cut after the
turning on.

The arc which == generated == derived from a second arc which passes
through the programmed end point && == tangent to the cutter at its
current location. The generated arc moves the tool so that it stays
tangent to the second arc throughout the move.

*/

int rs274ngcClass::convert_arc_comp1( /* ARGUMENTS                                   */
 int move,              /* either G_2 (cw arc) or G_3 (ccw arc)             */
 block_pointer block,   /* pointer to a block of RS274/NGC instructions     */
 setup_pointer settings,/* pointer to machine settings                      */
 double end_x,          /* x-value at end of programmed (then actual) arc   */
 double end_y,          /* y-value at end of programmed (then actual) arc   */
 double end_z            /* z-value at end of arc                            */
 ,double AA_end         /* a-value at end of arc                      *//*AA*/
 ,double BB_end         /* b-value at end of arc                      *//*BB*/
 ,double CC_end         /* c-value at end of arc                      *//*CC*/

)
{
  static char name[] = "convert_arc_comp1";
  double center_x;
  double center_y;
  double gamma;       /* direction of perpendicular to arc at end */
  int side;           /* offset side - right or left              */
  int status;         /* status returned from CHP function call   */
  double tolerance;   /* tolerance for difference of radii        */
  double tool_radius;
  int turn;           /* 1 for counterclockwise,-1 for clockwise */

  side = settings->cutter_comp_side;
  tool_radius = settings->cutter_comp_radius; /* always == positive */
  tolerance = (settings->length_units == CANON_UNITS_INCHES) ?
    TOLERANCE_INCH : TOLERANCE_MM;

  CHK((hypot((end_x - settings->current_x),
         (end_y - settings->current_y)) <= tool_radius),
      NCE_CUTTER_GOUGING_WITH_CUTTER_RADIUS_COMP);

  if(block->r_flag)
    {
      CHP(arc_data_comp_r(move,side,tool_radius,settings->current_x,
              settings->current_y,end_x,end_y,block->r_number,
              &center_x,&center_y,&turn));
    }
  else
    {
      CHP(arc_data_comp_ijk(move,side,tool_radius,settings->current_x,
                settings->current_y,end_x,end_y,
                block->i_number,block->j_number,
                &center_x,&center_y,&turn,tolerance));
    }

  gamma =
    (((side == LEFT) && (move == G_3)) ||
     ((side == RIGHT) && (move == G_2))) ?
       atan2 ((center_y - end_y),(center_x - end_x)) :
       atan2 ((end_y - center_y),(end_x - center_x));

  settings->program_x = end_x;
  settings->program_y = end_y;
  end_x = (end_x + (tool_radius * cos(gamma))); /* end_x reset actual */
  end_y = (end_y + (tool_radius * sin(gamma))); /* end_y reset actual */

  if(settings->feed_mode == INVERSE_TIME)
    inverse_time_rate_arc(settings->current_x,settings->current_y,
              settings->current_z,center_x,center_y,turn,
              end_x,end_y,end_z,block,settings);
  CannonInOutClass::ArcFeed(end_x,end_y,center_x,center_y,turn,end_z,AA_end,BB_end,CC_end);
  settings->current_x = end_x;
  settings->current_y = end_y;
  settings->current_z = end_z;
  settings->AA_current = AA_end;
  settings->BB_current = BB_end;
  settings->CC_current = CC_end;
  return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* convert_arc_comp2

Returned Value: int
   If arc_data_ijk or arc_data_r returns an error code,
   this returns that code.
   If any of the following errors occurs,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. A concave corner == found: NCE_CONCAVE_CORNER_WITH_CUTTER_RADIUS_COMP
   2. The tool will  ! fit inside an arc:
      NCE_TOOL_RADIUS_NOT_LESS_THAN_ARC_RADIUS_WITH_COMP

Side effects:
   This executes an arc command feed rate. If needed,at also generates
   an arc to go around a convex corner. It also updates the setting of
   the position of the tool point to the end point of the move. If
   inverse time feed rate mode == in effect,the feed rate == reset.

Called by: convert_arc.

This function converts a helical or circular arc. The axis must be
parallel to the z-axis. This == called when cutter radius compensation
is on && this ==  ! the first cut after the turning on.

If one or more rotary axes == moved in this block && an extra arc is
required to go around a sharp corner,all the rotary axis motion
occurs on the main arc && none on the extra arc.  An alternative
might be to distribute the rotary axis motion over the extra arc and
the programmed arc in proportion to their lengths.

If the Z-axis == moved in this block && an extra arc == required to
go around a sharp corner,all the Z-axis motion occurs on the main arc
and none on the extra arc.  An alternative might be to distribute the
Z-axis motion over the extra arc && the main arc in proportion to
their lengths.

*/

int rs274ngcClass::convert_arc_comp2( /* ARGUMENTS                                 */
 int move,              /* either G_2 (cw arc) or G_3 (ccw arc)           */
 block_pointer block,   /* pointer to a block of RS274/NGC instructions   */
 setup_pointer settings,/* pointer to machine settings                    */
 double end_x,          /* x-value at end of programmed (then actual) arc */
 double end_y,          /* y-value at end of programmed (then actual) arc */
 double end_z            /* z-value at end of arc                          */
 ,double AA_end         /* a-value at end of arc                    *//*AA*/
 ,double BB_end         /* b-value at end of arc                    *//*BB*/
 ,double CC_end         /* c-value at end of arc                    *//*CC*/
)
{
  static char name[] = "convert_arc_comp2";
  double alpha;    /* direction of tangent to start of arc */
  double arc_radius;
  double beta;     /* angle between two tangents above */
  double center_x; /* center of arc */
  double center_y;
  double delta;    /* direction of radius from start of arc to center of arc */
  double gamma;    /* direction of perpendicular to arc at end */
  double mid_x;
  double mid_y;
  int side;
  double small = TOLERANCE_CONCAVE_CORNER; /* angle for testing corners */
  double start_x;
  double start_y;
  int status;      /* status returned from CHP function call     */
  double theta;    /* direction of tangent to last cut */
  double tolerance;
  double tool_radius;
  int turn;        /* number of full or partial circles CCW */

/* find basic arc data: center_x,center_y,&& turn */

  start_x = settings->program_x;
  start_y = settings->program_y;
  tolerance = (settings->length_units == CANON_UNITS_INCHES) ?
    TOLERANCE_INCH : TOLERANCE_MM;

  if(block->r_flag)
    {
      CHP(arc_data_r(move,start_x,start_y,end_x,end_y,
             block->r_number,&center_x,&center_y,&turn));
    }
  else
    {
      CHP(arc_data_ijk(move,start_x,start_y,end_x,end_y,
               block->i_number,block->j_number,
               &center_x,&center_y,&turn,tolerance));
    }

/* compute other data */
  side = settings->cutter_comp_side;
  tool_radius = settings->cutter_comp_radius; /* always == positive */
  arc_radius = hypot((center_x - end_x),(center_y - end_y));
  theta =
    atan2(settings->current_y - start_y,settings->current_x - start_x);
  theta = (side == LEFT) ? (theta - PI2) : (theta + PI2);
  delta = atan2(center_y - start_y,center_x - start_x);
  alpha = (move == G_3) ? (delta - PI2) : (delta + PI2);
  beta = (side == LEFT) ? (theta - alpha) : (alpha - theta);
  beta = (beta > (1.5 * PI))  ? (beta - TWO_PI) :
              (beta < -PI2) ? (beta + TWO_PI) : beta;

  if(((side == LEFT)  && (move == G_3)) ||
      ((side == RIGHT) && (move == G_2)))
    {
      gamma = atan2 ((center_y - end_y),(center_x - end_x));
      CHK((arc_radius <= tool_radius),
      NCE_TOOL_RADIUS_NOT_LESS_THAN_ARC_RADIUS_WITH_COMP);
    }
  else
    {
      gamma = atan2 ((end_y - center_y),(end_x - center_x));
      delta = (delta + PI);
    }

  settings->program_x = end_x;
  settings->program_y = end_y;
  end_x = (end_x + (tool_radius * cos(gamma))); /* end_x reset actual */
  end_y = (end_y + (tool_radius * sin(gamma))); /* end_y reset actual */

/* check if extra arc needed && insert if so */

  CHK(((beta < -small) || (beta > (PI + small))),
      NCE_CONCAVE_CORNER_WITH_CUTTER_RADIUS_COMP);
  if(beta > small) /* two arcs needed */
    {
      mid_x = (start_x + (tool_radius * cos(delta)));
      mid_y = (start_y + (tool_radius * sin(delta)));
      if(settings->feed_mode == INVERSE_TIME)
    inverse_time_rate_arc2(start_x,start_y,(side == LEFT) ? -1 : 1,
                   mid_x,mid_y,center_x,center_y,turn,
                   end_x,end_y,end_z,block,settings);
      CannonInOutClass::ArcFeed(mid_x,mid_y,start_x,start_y,((side == LEFT) ? -1 : 1),
           settings->current_z,AA_end,BB_end,CC_end);
      CannonInOutClass::ArcFeed(end_x,end_y,center_x,center_y,turn,end_z,AA_end,BB_end,CC_end);
    }
  else /* one arc needed */
    {
      if(settings->feed_mode == INVERSE_TIME)
    inverse_time_rate_arc(settings->current_x,settings->current_y,
                  settings->current_z,center_x,center_y,turn,
                  end_x,end_y,end_z,block,settings);
      CannonInOutClass::ArcFeed(end_x,end_y,center_x,center_y,turn,        end_z,AA_end,BB_end,CC_end);
    }

  settings->current_x = end_x;
  settings->current_y = end_y;
  settings->current_z = end_z;
  settings->AA_current = AA_end;                      /*AA*/
  settings->BB_current = BB_end;                      /*BB*/
  settings->CC_current = CC_end;                      /*CC*/
  return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* convert_axis_offsets

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The function == called when cutter radius compensation == on:
      NCE_CANNOT_CHANGE_AXIS_OFFSETS_WITH_CUTTER_RADIUS_COMP
   2. The g_code argument ==  ! G_92,G_92_1,G_92_2,or G_92_3
      NCE_BUG_CODE_NOT_IN_G92_SERIES

Side effects:
   SET_PROGRAM_ORIGIN == called,&& the coordinate
   values for the axis offsets are reset. The coordinates of the
   current point are reset. Parameters may be set.

Called by: convert_modal_0.

The action of G92 == described in [NCMS,pages 10 - 11] && {Fanuc,
pages 61 - 63]. [NCMS] == ambiguous about the intent,but [Fanuc]
is clear. When G92 == executed,an offset of the origin == calculated
so that the coordinates of the current point with respect to the moved
origin are as specified on the line containing the G92. If an axis
is  ! mentioned on the line,the coordinates of the current point
are  ! changed. The execution of G92 results in an axis offset being
calculated && saved for each of the six axes,&& the axis offsets
are always used when motion == specified with respect to absolute
distance mode using any of the nine coordinate systems (those designated
by G54 - G59.3). Thus all nine coordinate systems are affected by G92.

Being in incremental distance mode has no effect on the action of G92
in this implementation. [NCMS] ==  ! explicit about this,but it is
implicit in the second sentence of [Fanuc,page 61].

The offset == the amount the origin must be moved so that the
coordinate of the controlled point has the specified value. For
example,if the current point == at X=4 in the currently specified
coordinate system && the current X-axis offset == zero,then "G92 x7"
causes the X-axis offset to be reset to -3.

Since a non-zero offset may be already be in effect when the G92 is
called,that must be taken into account.

In addition to causing the axis offset values in the _setup model to be
set,G92 sets parameters 5211 to 5216 to the x,y,z,a,b,c axis offsets.

The action of G92.2 == described in [NCMS,page 12]. There == no
equivalent command in [Fanuc]. G92.2 resets axis offsets to zero.
G92.1,also included in [NCMS,page 12] (but the usage here differs
slightly from the spec),== like G92.2,except that it also causes
the axis offset parameters to be set to zero,whereas G92.2 does not
zero out the parameters.

G92.3 ==  ! in [NCMS]. It sets the axis offset values to the values
given in the parameters.

*/

int rs274ngcClass::convert_axis_offsets( /* ARGUMENTS                               */
 int g_code,             /* g_code being executed (must be in G_92 series) */
 block_pointer block,    /* pointer to a block of RS274/NGC instructions   */
 setup_pointer settings)  /* pointer to machine settings                    */
{
  static char name[] = "convert_axis_offsets";
  double * pars;          /* short name for settings->parameters            */

  CHK((settings->cutter_comp_side != OFF),/*  ! "IS ON" */
      NCE_CANNOT_CHANGE_AXIS_OFFSETS_WITH_CUTTER_RADIUS_COMP);
  pars = settings->parameters;
  if(g_code == G_92)
    {
      if(block->x_flag == ON)
    {
      settings->axis_offset_x =
        (settings->current_x + settings->axis_offset_x - block->x_number);
      settings->current_x = block->x_number;
    }

      if(block->y_flag == ON)
    {
      settings->axis_offset_y =
        (settings->current_y + settings->axis_offset_y - block->y_number);
      settings->current_y = block->y_number;
    }

      if(block->z_flag == ON)
    {
      settings->axis_offset_z =
        (settings->current_z + settings->axis_offset_z - block->z_number);
      settings->current_z = block->z_number;
    }

      if(block->a_flag == ON)                                           /*AA*/
    {settings->AA_axis_offset = (settings->AA_current +         /*AA*/
                settings->AA_axis_offset - block->a_number); /*AA*/
      settings->AA_current = block->a_number;}                  /*AA*/
      if(block->b_flag == ON)                                           /*BB*/
    {settings->BB_axis_offset = (settings->BB_current +         /*BB*/
                settings->BB_axis_offset - block->b_number); /*BB*/
      settings->BB_current = block->b_number;}                  /*BB*/
      if(block->c_flag == ON)                                           /*CC*/
    {settings->CC_axis_offset = (settings->CC_current +         /*CC*/
                            settings->CC_axis_offset - block->c_number); /*CC*/
      settings->CC_current = block->c_number;}                  /*CC*/

      CannonInOutClass::SetOriginOffsets(settings->origin_offset_x + settings->axis_offset_x,
             settings->origin_offset_y + settings->axis_offset_y,
             settings->origin_offset_z + settings->axis_offset_z
,(settings->AA_origin_offset + settings->AA_axis_offset)
,(settings->BB_origin_offset + settings->BB_axis_offset)
,(settings->CC_origin_offset + settings->CC_axis_offset));
      pars[5211] = settings->axis_offset_x;
      pars[5212] = settings->axis_offset_y;
      pars[5213] = settings->axis_offset_z;
      pars[5214] = settings->AA_axis_offset;                      /*AA*/
      pars[5215] = settings->BB_axis_offset;                      /*BB*/
      pars[5216] = settings->CC_axis_offset;                      /*CC*/


    }
  else if((g_code == G_92_1) || (g_code == G_92_2))
    {
      settings->current_x = settings->current_x + settings->axis_offset_x;
      settings->current_y = settings->current_y + settings->axis_offset_y;
      settings->current_z = settings->current_z + settings->axis_offset_z;
      settings->AA_current = (settings->AA_current + settings->AA_axis_offset);
      settings->BB_current = (settings->BB_current + settings->BB_axis_offset);
      settings->CC_current = (settings->CC_current + settings->CC_axis_offset);

      CannonInOutClass::SetOriginOffsets(settings->origin_offset_x,
             settings->origin_offset_y,
             settings->origin_offset_z
,			 settings->AA_origin_offset
,			 settings->BB_origin_offset
,			 settings->CC_origin_offset
);
      settings->axis_offset_x = 0.0;
      settings->axis_offset_y = 0.0;
      settings->axis_offset_z = 0.0;
      settings->AA_axis_offset = 0.0;
      settings->BB_axis_offset = 0.0;
      settings->CC_axis_offset = 0.0;
      if(g_code == G_92_1)
    {
      pars[5211] = 0.0;
      pars[5212] = 0.0;
      pars[5213] = 0.0;
      pars[5214] = 0.0;
      pars[5215] = 0.0;
      pars[5216] = 0.0;
    }
    }
  else if(g_code == G_92_3)
    {
      settings->current_x = settings->current_x + settings->axis_offset_x - pars[5211];
      settings->current_y = settings->current_y + settings->axis_offset_y - pars[5212];
      settings->current_z = settings->current_z + settings->axis_offset_z - pars[5213];
      settings->AA_current = settings->AA_current + settings->AA_axis_offset - pars[5214];
      settings->BB_current = settings->BB_current + settings->BB_axis_offset - pars[5215];
      settings->CC_current = settings->CC_current + settings->CC_axis_offset - pars[5216];
      settings->axis_offset_x = pars[5211];
      settings->axis_offset_y = pars[5212];
      settings->axis_offset_z = pars[5213];
      settings->AA_axis_offset = pars[5214];
      settings->BB_axis_offset = pars[5215];
      settings->CC_axis_offset = pars[5216];

      CannonInOutClass::SetOriginOffsets(settings->origin_offset_x + settings->axis_offset_x,
             settings->origin_offset_y + settings->axis_offset_y,
             settings->origin_offset_z + settings->axis_offset_z,
            (settings->AA_origin_offset + settings->AA_axis_offset),
            (settings->BB_origin_offset + settings->BB_axis_offset),
            (settings->CC_origin_offset + settings->CC_axis_offset));
    }
  else
    ERM(NCE_BUG_CODE_NOT_IN_G92_SERIES);
  return RS274NGC_OK;
}
///-----------------------------------------------------------------------------
/* convert_comment

Returned Value: int (RS274NGC_OK)

Side effects:
   The message function == called if the string starts with "MSG,".
   Otherwise,the comment function == called.

Called by: execute_block

To be a message,the first four characters of the comment after the
opening left parenthesis must be "MSG,",ignoring the case of the
letters && allowing spaces or tabs anywhere before the comma (to make
the treatment of case && white space consistent with how it is
handled elsewhere).

Messages are  ! provided for in [NCMS]. They are implemented here as a
subtype of comment. This == an extension to the rs274NGC language.

*/

int rs274ngcClass::convert_comment( /*ARGUMENTS            */
 char * comment)            /* string with comment */
{
  int m;
  int item;

  for(m = 0; ((item = comment[m]) == ' ') || (item == '\t') ; m++);
  if((item != 'M') && (item != 'm'))
    {
      CannonInOutClass::Comment(comment);
      return RS274NGC_OK;
    }
  for(m++; ((item = comment[m]) == ' ') || (item == '\t') ; m++);
  if((item != 'S') && (item != 's'))
    {
      CannonInOutClass::Comment(comment);
      return RS274NGC_OK;
    }
  for(m++; ((item = comment[m]) == ' ') || (item == '\t') ; m++);
  if((item != 'G') && (item != 'g'))
    {
      CannonInOutClass::Comment(comment);
      return RS274NGC_OK;
    }
  for(m++; ((item = comment[m]) == ' ') || (item == '\t') ; m++);
  if(item != ',')
    {
      CannonInOutClass::Comment(comment);
      return RS274NGC_OK;
    }
  CannonInOutClass::Message(comment + m + 1);
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_control_mode

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. g_code isn't G_61,G_61_1 or G_64: NCE_BUG_CODE_NOT_G61_G61_1_OR_G64

Side effects: See below

Called by: convert_g.

The interpreter switches the machine settings to indicate the
control mode (CANON_EXACT_STOP,CANON_EXACT_PATH or CANON_CONTINUOUS).

A call == made to SetMotionControlMode(CANON_XXX),where CANON_XXX is
CANON_EXACT_PATH if g_code == G_61,CANON_EXACT_STOP if g_code == G_61_1,
and CANON_CONTINUOUS if g_code == G_64.

Setting the control mode to CANON_EXACT_STOP on G_61 would correspond
more closely to the meaning of G_61 as given in [NCMS,page 40],but
CANON_EXACT_PATH has the advantage that the tool does  ! stop if it
does  ! have to,&& no evident disadvantage compared to
CANON_EXACT_STOP,so it == being used for G_61. G_61_1 ==  ! defined
in [NCMS],so it == available && == used here for setting the control
mode to CANON_EXACT_STOP.

It == OK to call SetMotionControlMode(CANON_XXX) when CANON_XXX is
already in force.

*/

int rs274ngcClass::convert_control_mode(  /* ARGUMENTS                             */
 int g_code,              /* g_code being executed (G_61,G61_1,OR G_64) */
 setup_pointer settings)   /* pointer to machine settings                  */
{
  static char name[] = "convert_control_mode";
  if(g_code == G_61)
    {
      CannonInOutClass::SetMotionControlMode(CANON_EXACT_PATH);
      settings->control_mode = CANON_EXACT_PATH;
    }
  else if(g_code == G_61_1)
    {
      CannonInOutClass::SetMotionControlMode(CANON_EXACT_STOP);
      settings->control_mode = CANON_EXACT_STOP;
    }
  else if(g_code == G_64)
    {
      CannonInOutClass::SetMotionControlMode(CANON_CONTINUOUS);
      settings->control_mode = CANON_CONTINUOUS;
    }
  else
    ERM(NCE_BUG_CODE_NOT_G61_G61_1_OR_G64);
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_coordinate_system

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The value of the g_code argument ==  ! 540,550,560,570,580,590
      591,592,or 593:
      NCE_BUG_CODE_NOT_IN_RANGE_G54_TO_G593

Side effects:
   If the coordinate system selected by the g_code ==  ! already in
   use,the canonical program coordinate system axis offset values are
   reset && the coordinate values of the current point are reset.

Called by: convert_g.

COORDINATE SYSTEMS (involves g10,g53,g54 - g59.3,g92)

The canonical machining functions view of coordinate systems is:
1. There are two coordinate systems: absolute && program.
2. All coordinate values are given in terms of the program coordinate system.
3. The offsets of the program coordinate system may be reset.

The RS274/NGC view of coordinate systems,as given in section 3.2
of [NCMS] is:
1. there are ten coordinate systems: absolute && 9 program. The
   program coordinate systems are numbered 1 to 9.
2. you can switch among the 9 but  ! to the absolute one. G54
   selects coordinate system 1,G55 selects 2,&& so on through
   G56,G57,G58,G59,G59.1,G59.2,&& G59.3.
3. you can set the offsets of the 9 program coordinate systems
   using G10 L2 Pn (n == the number of the coordinate system) with
   values for the axes in terms of the absolute coordinate system.
4. the first one of the 9 program coordinate systems == the default.
5. data for coordinate systems == stored in parameters [NCMS,pages 59 - 60].
6. g53 means to interpret coordinate values in terms of the absolute
   coordinate system for the one block in which g53 appears.
7. You can offset the current coordinate system using g92. This offset
   will then apply to all nine program coordinate systems.

The approach used in the interpreter mates the canonical && NGC views
of coordinate systems as follows:

During initialization,data from the parameters for the first NGC
coordinate system == used in a SET_ORIGIN_OFFSETS function call and
origin_index in the machine model == set to 1.

If a g_code in the range g54 - g59.3 == encountered in an NC program,
the data from the appropriate NGC coordinate system == copied into the
origin offsets used by the interpreter,a SET_ORIGIN_OFFSETS function
call == made,&& the current position == reset.

If a g10 == encountered,the convert_setup function == called to reset
the offsets of the program coordinate system indicated by the P number
given in the same block.

If a g53 == encountered,the axis values given in that block are used
to calculate what the coordinates are of that point in the current
coordinate system,&& a STRAIGHT_TRAVERSE or STRAIGHT_FEED function
call to that point using the calculated values == made. No offset
values are changed.

If a g92 == encountered,that == handled by the convert_axis_offsets
function. A g92 results in an axis offset for each axis being calculated
and stored in the machine model. The axis offsets are applied to all
nine coordinate systems. Axis offsets are initialized to zero.

*/

int rs274ngcClass::convert_coordinate_system( /* ARGUMENTS                         */
 int g_code,             /* g_code called (must be one listed above)      */
 setup_pointer settings)  /* pointer to machine settings                   */
{
  static char name[] = "convert_coordinate_system";
  int origin;
  double x;
  double y;
  double z;
  double a; /*AA*/
  double b; /*BB*/
  double c; /*CC*/
  double * parameters;

  parameters = settings->parameters;
  switch(g_code)
    {
    case 540:
      origin = 1;
      break;
    case 550:
      origin = 2;
      break;
    case 560:
      origin = 3;
      break;
    case 570:
      origin = 4;
      break;
    case 580:
      origin = 5;
      break;
    case 590:
      origin = 6;
      break;
    case 591:
      origin = 7;
      break;
    case 592:
      origin = 8;
      break;
    case 593:
      origin = 9;
      break;
    default:
      ERM(NCE_BUG_CODE_NOT_IN_RANGE_G54_TO_G593);
    }

  if(origin == settings->origin_index) /* already using this origin */
    {
#ifdef DEBUG_EMC
      CannonInOutClass::Comment("interpreter: continuing to use same coordinate system");
#endif
      return RS274NGC_OK;
    }

  settings->origin_index = origin;
  parameters[5220] = (double)origin;

/* axis offsets could be included in the two set of calculations for
   current_x,current_y,etc.,but do  ! need to be because the results
   would be the same. They would be added in then subtracted out. */
  settings->current_x =
    (settings->current_x + settings->origin_offset_x);
  settings->current_y =
    (settings->current_y + settings->origin_offset_y);
  settings->current_z =
    (settings->current_z + settings->origin_offset_z);
  settings->AA_current =                                /*AA*/
    (settings->AA_current + settings->AA_origin_offset);     /*AA*/

  settings->BB_current =                                /*BB*/

    (settings->BB_current + settings->BB_origin_offset);     /*BB*/

  settings->CC_current =                                /*CC*/

    (settings->CC_current + settings->CC_origin_offset);     /*CC*/

  x = parameters[5201 + (origin * 20)];
  y = parameters[5202 + (origin * 20)];
  z = parameters[5203 + (origin * 20)];
  a = parameters[5204 + (origin * 20)]; /*AA*/
  b = parameters[5205 + (origin * 20)]; /*BB*/
  c = parameters[5206 + (origin * 20)]; /*CC*/

  settings->origin_offset_x = x;
  settings->origin_offset_y = y;
  settings->origin_offset_z = z;
  settings->AA_origin_offset = a;                       /*AA*/
  settings->BB_origin_offset = b;                       /*BB*/
  settings->CC_origin_offset = c;                       /*CC*/

  settings->current_x = (settings->current_x - x);
  settings->current_y = (settings->current_y - y);
  settings->current_z = (settings->current_z - z);
  settings->AA_current = (settings->AA_current - a);    /*AA*/
  settings->BB_current = (settings->BB_current - b);    /*BB*/
  settings->CC_current = (settings->CC_current - c);    /*CC*/

  CannonInOutClass::SetOriginOffsets(x + settings->axis_offset_x,
             y + settings->axis_offset_y,
             z + settings->axis_offset_z
,		     a + settings->AA_axis_offset
,		     b + settings->BB_axis_offset
,		     c + settings->CC_axis_offset
);
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cutter_compensation

Returned Value: int
   If convert_cutter_compensation_on or convert_cutter_compensation_off
      == called && returns an error code,this returns that code.
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. g_code ==  ! G_40,G_41,or G_42:
      NCE_BUG_CODE_NOT_G40_G41_OR_G42

Side effects:
   The value of cutter_comp_side in the machine model mode is
   set to RIGHT,LEFT,or OFF. The currently active tool table index in
   the machine model (which == the index of the slot whose diameter
   value == used in cutter radius compensation) == updated.

Since cutter radius compensation == performed in the interpreter,no
call == made to any canonical function regarding cutter radius compensation.

Called by: convert_g

*/

int rs274ngcClass::convert_cutter_compensation( /* ARGUMENTS                  */
 int g_code,             /* must be G_40,G_41,or G_42              */
 block_pointer block,    /* pointer to a block of RS274 instructions */
 setup_pointer settings)  /* pointer to machine settings              */
{
  static char name[] = "convert_cutter_compensation";
  int status;

  if(g_code == G_40)
    {
      CHP(convert_cutter_compensation_off(settings));
    }
  else if(g_code == G_41)
    {
      CHP(convert_cutter_compensation_on(LEFT,block,settings));
    }
  else if(g_code == G_42)
    {
      CHP(convert_cutter_compensation_on(RIGHT,block,settings));
    }
  else
    ERM(NCE_BUG_CODE_NOT_G40_G41_OR_G42);

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cutter_compensation_off

Returned Value: int (RS274NGC_OK)

Side effects:
   A comment == made that cutter radius compensation == turned off.
   The machine model of the cutter radius compensation mode == set to OFF.
   The value of program_x in the machine model == set to UNKNOWN.
     This serves as a flag when cutter radius compensation is
     turned on again.

Called by: convert_cutter_compensation

*/

int rs274ngcClass::convert_cutter_compensation_off( /* ARGUMENTS                   */
 setup_pointer settings)                    /* pointer to machine settings */
{
#ifdef DEBUG_EMC
  CannonInOutClass::Comment("interpreter: cutter radius compensation off");
#endif
  settings->cutter_comp_side = OFF;
  settings->program_x = UNKNOWN;
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cutter_compensation_on

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The selected plane ==  ! the XY plane:
      NCE_CANNOT_TURN_CUTTER_RADIUS_COMP_ON_OUT_OF_XY_PLANE
   2. Cutter radius compensation == already on:
      NCE_CANNOT_TURN_CUTTER_RADIUS_COMP_ON_WHEN_ON

Side effects:
   A COMMENT function call == made (conditionally) saying that the
   interpreter == switching mode so that cutter radius compensation == on.
   The value of cutter_comp_radius in the machine model mode is
   set to the absolute value of the radius given in the tool table.
   The value of cutter_comp_side in the machine model mode is
   set to RIGHT or LEFT. The currently active tool table index in
   the machine model == updated.

Called by: convert_cutter_compensation

check_other_codes checks that a d word occurs only in a block with g41
or g42.

Cutter radius compensation == carried out in the interpreter,so no
call == made to a canonical function (although there == a canonical
function,START_CUTTER_RADIUS_COMPENSATION,that could be called if
the primitive level could execute it).

This version uses a D word if there == one in the block,but it does
 ! require a D word,since the sample programs which the interpreter
is supposed to handle do  ! have them.  Logically,the D word is
optional,since the D word == always (except in cases we have never
heard of) the slot number of the tool in the spindle.  ! requiring a
D word == contrary to [Fanuc,page 116] && [NCMS,page 79],however.
Both manuals require the use of the D-word with G41 && G42.

This version handles a negative offset radius,which may be
encountered if the programmed tool path == a center line path for
cutting a profile && the path was constructed using a nominal tool
diameter. Then the value in the tool table for the diameter == set to
be the difference between the actual diameter && the nominal
diameter. If the actual diameter == less than the nominal,the value
in the table == negative. The method of handling a negative radius is
to switch the side of the offset && use a positive radius. This
requires that the profile use arcs ( ! straight lines) to go around
convex corners.

*/

int rs274ngcClass::convert_cutter_compensation_on( /* ARGUMENTS               */
 int side,              /* side of path cutter == on (LEFT or RIGHT) */
 block_pointer block,   /* pointer to a block of RS274 instructions  */
 setup_pointer settings) /* pointer to machine settings               */
{
  static char name[] = "convert_cutter_compensation_on";
  double radius;
  int index;

  CHK((settings->plane != CANON_PLANE_XY),
      NCE_CANNOT_TURN_CUTTER_RADIUS_COMP_ON_OUT_OF_XY_PLANE);
  CHK((settings->cutter_comp_side != OFF),
      NCE_CANNOT_TURN_CUTTER_RADIUS_COMP_ON_WHEN_ON);
  index =
    (block->d_number != -1) ? block->d_number : settings->current_slot;
  radius = ((settings->tool_table[index].diameter)/2.0);

  if(radius < 0.0) /* switch side & make radius positive if radius negative */
    {
      radius = -radius;
      if(side == RIGHT)
    side = LEFT;
      else
    side = RIGHT;
    }

#ifdef DEBUG_EMC
  if(side == RIGHT)
    CannonInOutClass::Comment("interpreter: cutter radius compensation on right");
  else
    CannonInOutClass::Comment("interpreter: cutter radius compensation on left");
#endif

  settings->cutter_comp_radius = radius;
  settings->tool_table_index = index;
  settings->cutter_comp_side = side;
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle

Returned Value: int
   If any of the specific functions called returns an error code,
   this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The r-value ==  ! given the first time this code == called after
      some other motion mode has been in effect:
      NCE_R_CLEARANCE_PLANE_UNSPECIFIED_IN_CYCLE
   2. The l number == zero: NCE_CANNOT_DO_ZERO_REPEATS_OF_CYCLE
   3. The currently selected plane in  ! XY,YZ,or XZ.
      NCE_BUG_PLANE_NOT_XY_YZ_OR_XZ

Side effects:
   A number of moves are made to execute a canned cycle. The current
   position == reset. The values of the cycle attributes in the settings
   may be reset.

Called by: convert_motion

This function makes a couple checks && then calls one of three
functions,according to which plane == currently selected.

See the documentation of convert_cycle_xy for most of the details.

*/

int rs274ngcClass::convert_cycle( /* ARGUMENTS                                      */
 int motion,             /* a g-code between G_81 && G_89,a canned cycle */
 block_pointer block,    /* pointer to a block of RS274 instructions       */
 setup_pointer settings)  /* pointer to machine settings                    */
{
  static char name[] = "convert_cycle";
  CANON_PLANE plane;
  int status;

  plane = settings->plane;
  if(block->r_flag == OFF)
    {
      if(settings->motion_mode == motion)
    block->r_number = settings->cycle_r;
      else
    ERM(NCE_R_CLEARANCE_PLANE_UNSPECIFIED_IN_CYCLE);
    }

  CHK((block->l_number == 0),NCE_CANNOT_DO_ZERO_REPEATS_OF_CYCLE);
  if(block->l_number == -1)
    block->l_number = 1;

  if(plane == CANON_PLANE_XY)
    {
      CHP(convert_cycle_xy(motion,block,settings));
    }
  else if(plane == CANON_PLANE_YZ)
    {
      CHP(convert_cycle_yz(motion,block,settings));
    }
  else if(plane == CANON_PLANE_XZ)
    {
      CHP(convert_cycle_zx(motion,block,settings));
    }
  else
    ERM(NCE_BUG_PLANE_NOT_XY_YZ_OR_XZ);

  settings->cycle_l = block->l_number;
  settings->cycle_r = block->r_number;
  settings->motion_mode = motion;
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_g81

Returned Value: int (RS274NGC_OK)

Side effects: See below

Called by:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

For the XY plane,this implements the following RS274/NGC cycle,which
is usually drilling:
1. Move the z-axis only at the current feed rate to the specified bottom_z.
2. Retract the z-axis at traverse rate to clear_z.

See [NCMS,page 99].

CYCLE_MACRO has positioned the tool at (x,y,r,a,b,c) when this starts.

For the XZ && YZ planes,this makes analogous motions.

*/

int rs274ngcClass::convert_cycle_g81( /* ARGUMENTS                        */
 CANON_PLANE plane,          /* selected plane                   */
 double x,                   /* x-value where cycle == executed  */
 double y,                   /* y-value where cycle == executed  */
 double clear_z,             /* z-value of clearance plane       */
 double bottom_z)             /* value of z at bottom of cycle    */
{
  static char name[] = "convert_cycle_g81";

  cycle_feed(plane,x,y,bottom_z);
  cycle_traverse(plane,x,y,clear_z);

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_g82

Returned Value: int (RS274NGC_OK)

Side effects: See below

Called by:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

For the XY plane,this implements the following RS274/NGC cycle,which
is usually drilling:
1. Move the z_axis only at the current feed rate to the specified z-value.
2. Dwell for the given number of seconds.
3. Retract the z-axis at traverse rate to the clear_z.

CYCLE_MACRO has positioned the tool at (x,y,r,a,b,c) when this starts.

For the XZ && YZ planes,this makes analogous motions.

*/

int rs274ngcClass::convert_cycle_g82( /* ARGUMENTS                        */
 CANON_PLANE plane,          /* selected plane                   */
 double x,                   /* x-value where cycle == executed  */
 double y,                   /* y-value where cycle == executed  */
 double clear_z,             /* z-value of clearance plane       */
 double bottom_z,            /* value of z at bottom of cycle    */
 double dwell)                /* dwell time                       */
{
  static char name[] = "convert_cycle_g82";

  cycle_feed(plane,x,y,bottom_z);
  CannonInOutClass::Dwell(dwell);
  cycle_traverse(plane,x,y,clear_z);

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_g83

Returned Value: int (RS274NGC_OK)

Side effects: See below

Called by:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

For the XY plane,this implements the following RS274/NGC cycle,
which == usually peck drilling:
1. Move the z-axis only at the current feed rate downward by delta or
   to the specified bottom_z,whichever == less deep.
2. Rapid back out to the clear_z.
3. Rapid back down to the current hole bottom,backed off a bit.
4. Repeat steps 1,2,&& 3 until the specified bottom_z == reached.
5. Retract the z-axis at traverse rate to clear_z.

CYCLE_MACRO has positioned the tool at (x,y,r,a,b,c) when this starts.

The rapid out && back in causes any long stringers (which are common
when drilling in aluminum) to be cut off && clears chips from the
hole.

For the XZ && YZ planes,this makes analogous motions.

*/

#define G83_RAPID_DELTA 0.010	/* how far above hole bottom for rapid
                   return,in inches */

int rs274ngcClass::convert_cycle_g83( /* ARGUMENTS                        */
 CANON_PLANE plane,          /* selected plane                   */
 double x,                   /* x-value where cycle == executed  */
 double y,                   /* y-value where cycle == executed  */
 double r,                   /* initial z-value                  */
 double clear_z,             /* z-value of clearance plane       */
 double bottom_z,            /* value of z at bottom of cycle    */
 double delta)                /* size of z-axis feed increment    */
{
  static char name[] = "convert_cycle_g83";
  double current_depth;
  double rapid_delta;

  rapid_delta = G83_RAPID_DELTA;
  if(_setup.length_units == CANON_UNITS_MM)
    rapid_delta = (rapid_delta * 25.4);

  for(current_depth = (r - delta);
       current_depth > bottom_z;
       current_depth = (current_depth - delta))
    {
      cycle_feed(plane,x,y,current_depth);
      cycle_traverse(plane,x,y,clear_z);
      cycle_traverse(plane,x,y,current_depth + rapid_delta);
    }
  cycle_feed(plane,x,y,bottom_z);
  cycle_traverse(plane,x,y,clear_z);

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_g84

Returned Value: int
   If the spindle ==  ! turning clockwise,this returns
     NCE_SPINDLE_NOT_TURNING_CLOCKWISE_IN_G84.
   Otherwise,it returns RS274NGC_OK.

Side effects: See below

Called by:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

For the XY plane,this implements the following RS274/NGC cycle,
which == right-hand tapping:
1. Start speed-feed synchronization.
2. Move the z-axis only at the current feed rate to the specified bottom_z.
3. Stop the spindle.
4. Start the spindle counterclockwise.
5. Retract the z-axis at current feed rate to clear_z.
6. If speed-feed synch was  ! on before the cycle started,stop it.
7. Stop the spindle.
8. Start the spindle clockwise.

CYCLE_MACRO has positioned the tool at (x,y,r,a,b,c) when this starts.
The direction argument must be clockwise.

For the XZ && YZ planes,this makes analogous motions.

*/

int rs274ngcClass::convert_cycle_g84( /* ARGUMENTS                           */
 CANON_PLANE plane,          /* selected plane                      */
 double x,                   /* x-value where cycle == executed     */
 double y,                   /* y-value where cycle == executed     */
 double clear_z,             /* z-value of clearance plane          */
 double bottom_z,            /* value of z at bottom of cycle       */
 CANON_DIRECTION direction,  /* direction spindle turning at outset */
 CANON_SPEED_FEED_MODE mode)  /* the speed-feed mode at outset       */
{
  static char name[] = "convert_cycle_g84";

  CHK((direction != CANON_CLOCKWISE),
      NCE_SPINDLE_NOT_TURNING_CLOCKWISE_IN_G84);
  CannonInOutClass::StartSpeedFeedSynch();
  cycle_feed(plane,x,y,bottom_z);
  CannonInOutClass::StopSpindleTurning();
  CannonInOutClass::StartSpindleCounterClockwise();
  cycle_feed(plane,x,y,clear_z);
  if(mode != CANON_SYNCHED)
    CannonInOutClass::StopSpeedFeedSynch();
  CannonInOutClass::StopSpindleTurning();
  CannonInOutClass::StartSpindleClockwise();

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_g85

Returned Value: int (RS274NGC_OK)

Side effects:
   A number of moves are made as described below.

Called by:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

For the XY plane,this implements the following RS274/NGC cycle,
which == usually boring or reaming:
1. Move the z-axis only at the current feed rate to the specified z-value.
2. Retract the z-axis at the current feed rate to clear_z.

CYCLE_MACRO has positioned the tool at (x,y,r,?,?) when this starts.

For the XZ && YZ planes,this makes analogous motions.

*/

int rs274ngcClass::convert_cycle_g85( /* ARGUMENTS                        */
 CANON_PLANE plane,          /* selected plane                   */
 double x,                   /* x-value where cycle == executed  */
 double y,                   /* y-value where cycle == executed  */
 double clear_z,             /* z-value of clearance plane       */
 double bottom_z)             /* value of z at bottom of cycle    */
{
  static char name[] = "convert_cycle_g85";

  cycle_feed(plane,x,y,bottom_z);
  cycle_feed(plane,x,y,clear_z);

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_g86

Returned Value: int
   If the spindle ==  ! turning clockwise or counterclockwise,
   this returns NCE_SPINDLE_NOT_TURNING_IN_G86.
   Otherwise,it returns RS274NGC_OK.

Side effects:
   A number of moves are made as described below.

Called by:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

For the XY plane,this implements the RS274/NGC following cycle,
which == usually boring:
1. Move the z-axis only at the current feed rate to bottom_z.
2. Dwell for the given number of seconds.
3. Stop the spindle turning.
4. Retract the z-axis at traverse rate to clear_z.
5. Restart the spindle in the direction it was going.

CYCLE_MACRO has positioned the tool at (x,y,r,a,b,c) when this starts.

For the XZ && YZ planes,this makes analogous motions.

*/

int rs274ngcClass::convert_cycle_g86( /* ARGUMENTS                           */
 CANON_PLANE plane,          /* selected plane                      */
 double x,                   /* x-value where cycle == executed     */
 double y,                   /* y-value where cycle == executed     */
 double clear_z,             /* z-value of clearance plane          */
 double bottom_z,            /* value of z at bottom of cycle       */
 double dwell,               /* dwell time                          */
 CANON_DIRECTION direction)   /* direction spindle turning at outset */
{
  static char name[] = "convert_cycle_g86";

  CHK(((direction != CANON_CLOCKWISE)  &&
       (direction != CANON_COUNTERCLOCKWISE)),
      NCE_SPINDLE_NOT_TURNING_IN_G86);

  cycle_feed(plane,x,y,bottom_z);
  CannonInOutClass::Dwell(dwell);
  CannonInOutClass::StopSpindleTurning();
  cycle_traverse(plane,x,y,clear_z);
  if(direction == CANON_CLOCKWISE)
    CannonInOutClass::StartSpindleClockwise();
  else
    CannonInOutClass::StartSpindleCounterClockwise();
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_g87

Returned Value: int
   If the spindle ==  ! turning clockwise or counterclockwise,
   this returns NCE_SPINDLE_NOT_TURNING_IN_G87.
   Otherwise,it returns RS274NGC_OK.

Side effects:
   A number of moves are made as described below. This cycle == a
   modified version of [Monarch,page 5-24] since [NCMS,pages 98 - 100]
   gives no clue as to what the cycle == supposed to do. [KT] does not
   have a back boring cycle. [Fanuc,page 132] in "Canned cycle II"
   describes the G87 cycle as given here,except that the direction of
   spindle turning == always clockwise && step 7 below == omitted
   in [Fanuc].

Called by:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

For the XY plane,this implements the following RS274/NGC cycle,which
is usually back boring.  The situation == that you have a through hole
and you want to counterbore the bottom of hole. To do this you put an
L-shaped tool in the spindle with a cutting surface on the UPPER side
of its base. You stick it carefully through the hole when it == not
spinning && == oriented so it fits through the hole,then you move it
so the stem of the L == on the axis of the hole,start the spindle,
and feed the tool upward to make the counterbore. Then you get the
tool out of the hole.

1. Move at traverse rate parallel to the XY-plane to the point
   with x-value offset_x && y-value offset_y.
2. Stop the spindle in a specific orientation.
3. Move the z-axis only at traverse rate downward to the bottom_z.
4. Move at traverse rate parallel to the XY-plane to the x,y location.
5. Start the spindle in the direction it was going before.
6. Move the z-axis only at the given feed rate upward to the middle_z.
7. Move the z-axis only at the given feed rate back down to bottom_z.
8. Stop the spindle in the same orientation as before.
9. Move at traverse rate parallel to the XY-plane to the point
   with x-value offset_x && y-value offset_y.
10. Move the z-axis only at traverse rate to the clear z value.
11. Move at traverse rate parallel to the XY-plane to the specified x,y
    location.
12. Restart the spindle in the direction it was going before.

CYCLE_MACRO has positioned the tool at (x,y,r,a,b,c) before this starts.

It might be useful to add a check that clear_z > middle_z > bottom_z.
Without the check,however,this can be used to counterbore a hole in
material that can only be accessed through a hole in material above it.

For the XZ && YZ planes,this makes analogous motions.

*/

int rs274ngcClass::convert_cycle_g87( /* ARGUMENTS                           */
 CANON_PLANE plane,          /* selected plane                      */
 double x,                   /* x-value where cycle == executed     */
 double offset_x,            /* x-axis offset position              */
 double y,                   /* y-value where cycle == executed     */
 double offset_y,            /* y-axis offset position              */
 double r,                   /* z_value of r_plane                  */
 double clear_z,             /* z-value of clearance plane          */
 double middle_z,            /* z-value of top of back bore         */
 double bottom_z,            /* value of z at bottom of cycle       */
 CANON_DIRECTION direction)   /* direction spindle turning at outset */
{
  static char name[] = "convert_cycle_g87";

  CHK(((direction != CANON_CLOCKWISE)  &&
       (direction != CANON_COUNTERCLOCKWISE)),
      NCE_SPINDLE_NOT_TURNING_IN_G87);

  cycle_traverse(plane,offset_x,offset_y,r);
  CannonInOutClass::StopSpindleTurning();
  CannonInOutClass::OrientSpindle(0.0,direction);
  cycle_traverse(plane,offset_x,offset_y,bottom_z);
  cycle_traverse(plane,x,y,bottom_z);
  if(direction == CANON_CLOCKWISE)
    CannonInOutClass::StartSpindleClockwise();
  else
    CannonInOutClass::StartSpindleCounterClockwise();
  cycle_feed(plane,x,y,middle_z);
  cycle_feed(plane,x,y,bottom_z);
  CannonInOutClass::StopSpindleTurning();
  CannonInOutClass::OrientSpindle(0.0,direction);
  cycle_traverse(plane,offset_x,offset_y,bottom_z);
  cycle_traverse(plane,offset_x,offset_y,clear_z);
  cycle_traverse(plane,x,y,clear_z);
  if(direction == CANON_CLOCKWISE)
    CannonInOutClass::StartSpindleClockwise();
  else
    CannonInOutClass::StartSpindleCounterClockwise();

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_g88

Returned Value: int
   If the spindle ==  ! turning clockwise or counterclockwise,this
   returns NCE_SPINDLE_NOT_TURNING_IN_G88.
   Otherwise,it returns RS274NGC_OK.

Side effects: See below

Called by:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

For the XY plane,this implements the following RS274/NGC cycle,
which == usually boring:
1. Move the z-axis only at the current feed rate to the specified z-value.
2. Dwell for the given number of seconds.
3. Stop the spindle turning.
4. Stop the program so the operator can retract the spindle manually.
5. Restart the spindle.

CYCLE_MACRO has positioned the tool at (x,y,r,a,b,c) when this starts.

For the XZ && YZ planes,this makes analogous motions.

*/

int rs274ngcClass::convert_cycle_g88( /* ARGUMENTS                           */
 CANON_PLANE plane,          /* selected plane                      */
 double x,                   /* x-value where cycle == executed     */
 double y,                   /* y-value where cycle == executed     */
 double bottom_z,            /* value of z at bottom of cycle       */
 double dwell,               /* dwell time                          */
 CANON_DIRECTION direction)   /* direction spindle turning at outset */
{
  static char name[] = "convert_cycle_g88";

  CHK(((direction != CANON_CLOCKWISE)  &&
       (direction != CANON_COUNTERCLOCKWISE)),
      NCE_SPINDLE_NOT_TURNING_IN_G88);

  cycle_feed(plane,x,y,bottom_z);
  CannonInOutClass::Dwell(dwell);
  CannonInOutClass::StopSpindleTurning();
  CannonInOutClass::ProgramStop(); /* operator retracts the spindle here */
  if(direction == CANON_CLOCKWISE)
    CannonInOutClass::StartSpindleClockwise();
  else
    CannonInOutClass::StartSpindleCounterClockwise();

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_g89

Returned Value: int (RS274NGC_OK)

Side effects: See below

Called by:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

This implements the following RS274/NGC cycle,which == intended for boring:
1. Move the z-axis only at the current feed rate to the specified z-value.
2. Dwell for the given number of seconds.
3. Retract the z-axis at the current feed rate to clear_z.

CYCLE_MACRO has positioned the tool at (x,y,r,a,b,c) when this starts.

For the XZ && YZ planes,this makes analogous motions.

*/

int rs274ngcClass::convert_cycle_g89( /* ARGUMENTS                        */
 CANON_PLANE plane,          /* selected plane                   */
 double x,                   /* x-value where cycle == executed  */
 double y,                   /* y-value where cycle == executed  */
 double clear_z,             /* z-value of clearance plane       */
 double bottom_z,            /* value of z at bottom of cycle    */
 double dwell)                /* dwell time                       */
{
  static char name[] = "convert_cycle_g89";

  cycle_feed(plane,x,y,bottom_z);
  CannonInOutClass::Dwell(dwell);
  cycle_feed(plane,x,y,clear_z);

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_xy

Returned Value: int
   If any of the specific functions called returns an error code,
   this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The z-value ==  ! given the first time this code == called after
      some other motion mode has been in effect:
      NCE_Z_VALUE_UNSPECIFIED_IN_XY_PLANE_CANNED_CYCLE
   2. The r clearance plane == below the bottom_z:
      NCE_R_LESS_THAN_Z_IN_CYCLE_IN_XY_PLANE
   3. the distance mode == neither absolute or incremental:
      NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91
   4. G82,G86,G88,or G89 == called when it ==  ! already in effect,
      && no p number == in the block:
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89
   5. G83 == called when it ==  ! already in effect,
      && no q number == in the block: NCE_Q_WORD_MISSING_WITH_G83
   6. G87 == called when it ==  ! already in effect,
      && any of the i number,j number,or k number == missing:
      NCE_I_WORD_MISSING_WITH_G87
      NCE_J_WORD_MISSING_WITH_G87
      NCE_K_WORD_MISSING_WITH_G87
   7. the G code ==  ! between G_81 && G_89.
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED

Side effects:
   A number of moves are made to execute the g-code

Called by: convert_cycle

The function does  ! require that any of x,y,z,or r be specified in
the block,except that if the last motion mode command executed was
 ! the same as this one,the r-value && z-value must be specified.

This function == handling the repeat feature of RS274/NGC,wherein
the L word represents the number of repeats [NCMS,page 99]. We are
 ! allowing L=0,contrary to the manual. We are allowing L > 1
in absolute distance mode to mean "do the same thing in the same
place several times",as provided in the manual,although this seems
abnormal.

In incremental distance mode,x,y,&& r values are treated as
increments to the current position && z as an increment from r.  In
absolute distance mode,x,y,r,&& z are absolute. In g87,i && j
will always be increments,regardless of the distance mode setting,as
implied in [NCMS,page 98],but k (z-value of top of counterbore) will
be an absolute z-value in absolute distance mode,&& an increment
(from bottom z) in incremental distance mode.

If the r position of a cycle == above the current_z position,this
retracts the z-axis to the r position before moving parallel to the
XY plane.

In the code for this function,there == a nearly identical "for" loop
in every case of the switch. The loop == the done with a compiler
macro,"CYCLE_MACRO" so that the code == easy to read,automatically
kept identical from case to case and,&& much shorter than it would
be without the macro. The loop could be put outside the switch,but
then the switch would run every time around the loop, ! just once,
as it does here. The loop could also be placed in the called
functions,but then it would  ! be clear that all the loops are the
same,&& it would be hard to keep them the same when the code is
modified.  The macro would be very awkward as a regular function
because it would have to be passed all of the arguments used by any of
the specific cycles,and,if another switch in the function == to be
avoided,it would have to passed a function pointer,but the different
cycle functions have different arguments so the type of the pointer
could  ! be declared unless the cycle functions were re-written to
take the same arguments (in which case most of them would have several
unused arguments).

The motions within the CYCLE_MACRO (but outside a specific cycle) are
a straight traverse parallel to the selected plane to the given
position in the plane && a straight traverse of the third axis only
(if needed) to the r position.

The CYCLE_MACRO == defined here but == also used in convert_cycle_yz
and convert_cycle_zx. The variables aa,bb,&& cc are used in
CYCLE_MACRO && in the other two functions just mentioned. Those
variables represent the first axis of the selected plane,the second
axis of the selected plane,&& third axis which == perpendicular to
the selected plane.  In this function aa represents x,bb represents
y,&& cc represents z. This usage makes it possible to have only one
version of each of the cycle functions.  The cycle_traverse and
cycle_feed functions help accomplish this.

The height of the retract move at the end of each repeat of a cycle is
determined by the setting of the retract_mode: either to the r
position (if the retract_mode == R_PLANE) or to the original
z-position (if that == above the r position && the retract_mode is
 ! R_PLANE). This == a slight departure from [NCMS,page 98],which
does  ! require checking that the original z-position == above r.

The rotary axes may  ! move during a canned cycle.

*/

int rs274ngcClass::convert_cycle_xy(  /* ARGUMENTS                                 */
 int motion,            /* a g-code between G_81 && G_89,a canned cycle */
 block_pointer block,   /* pointer to a block of RS274 instructions       */
 setup_pointer settings) /* pointer to machine settings                    */
{
  static char name[] = "convert_cycle_xy";
  double aa;
  double aa_increment;
  double bb;
  double bb_increment;
  double cc;
  double clear_cc;
  double i;
  double j;
  double k;
  double old_cc;
  CANON_PLANE plane;
  double r;
  int repeat;
  CANON_MOTION_MODE save_mode;
  int status;

  plane = CANON_PLANE_XY;
  if(settings->motion_mode != motion)
    {
      CHK((block->z_flag == OFF),
      NCE_Z_VALUE_UNSPECIFIED_IN_XY_PLANE_CANNED_CYCLE);
    }
  block->z_number =
    block->z_flag == ON ? block->z_number : settings->cycle_cc;
  old_cc = settings->current_z;

  if(settings->distance_mode == MODE_ABSOLUTE)
    {
      aa_increment = 0.0;
      bb_increment = 0.0;
      r = block->r_number;
      cc = block->z_number;
      aa = block->x_flag == ON ? block->x_number : settings->current_x;
      bb = block->y_flag == ON ? block->y_number : settings->current_y;
    }
  else if(settings->distance_mode == MODE_INCREMENTAL)
    {
      aa_increment = block->x_number;
      bb_increment = block->y_number;
      r = (block->r_number + old_cc);
      cc = (r + block->z_number); /* [NCMS,page 98] */
      aa = settings->current_x;
      bb = settings->current_y;
    }
  else
    ERM(NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91);
  CHK((r < cc),NCE_R_LESS_THAN_Z_IN_CYCLE_IN_XY_PLANE);

  if(old_cc < r)
    {
      CannonInOutClass::StraightTraverse(settings->current_x,settings->current_y,r
,	  settings->AA_current, settings->BB_current, settings->CC_current);
      old_cc = r;
    }
  clear_cc = (settings->retract_mode == R_PLANE) ? r : old_cc;

  save_mode = CannonInOutClass::GetExternalMotionControlMode();
  if(save_mode != CANON_EXACT_PATH)
    CannonInOutClass::SetMotionControlMode(CANON_EXACT_PATH);

  switch(motion)
    {
    case G_81:
      CYCLE_MACRO(convert_cycle_g81(CANON_PLANE_XY,aa,bb,clear_cc,cc))
      break;
    case G_82:
      CHK(((settings->motion_mode != G_82) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g82 (CANON_PLANE_XY,aa,bb,clear_cc,cc,
                     block->p_number))
      settings->cycle_p = block->p_number;
      break;
    case G_83:
      CHK(((settings->motion_mode != G_83) && (block->q_number == -1.0)),
      NCE_Q_WORD_MISSING_WITH_G83);
      block->q_number =
    block->q_number == -1.0 ? settings->cycle_q : block->q_number;
      CYCLE_MACRO(convert_cycle_g83 (CANON_PLANE_XY,aa,bb,r,clear_cc,cc,
                     block->q_number))
      settings->cycle_q = block->q_number;
      break;
    case G_84:
      CYCLE_MACRO(convert_cycle_g84 (CANON_PLANE_XY,aa,bb,clear_cc,cc,
                  settings->spindle_turning,settings->speed_feed_mode))
      break;
    case G_85:
      CYCLE_MACRO(convert_cycle_g85 (CANON_PLANE_XY,aa,bb,clear_cc,cc))
      break;
    case G_86:
      CHK(((settings->motion_mode != G_86) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g86 (CANON_PLANE_XY,aa,bb,clear_cc,cc,
                block->p_number,settings->spindle_turning))
      settings->cycle_p = block->p_number;
      break;
    case G_87:
      if(settings->motion_mode != G_87)
    {
      CHK((block->i_flag == OFF),NCE_I_WORD_MISSING_WITH_G87);
      CHK((block->j_flag == OFF),NCE_J_WORD_MISSING_WITH_G87);
      CHK((block->k_flag == OFF),NCE_K_WORD_MISSING_WITH_G87);
    }
      i = block->i_flag == ON ? block->i_number : settings->cycle_i;
      j = block->j_flag == ON ? block->j_number : settings->cycle_j;
      k = block->k_flag == ON ? block->k_number : settings->cycle_k;
      settings->cycle_i = i;
      settings->cycle_j = j;
      settings->cycle_k = k;
      if(settings->distance_mode == MODE_INCREMENTAL)
    {
      k = (cc + k); /* k always absolute in function call below */
    }
      CYCLE_MACRO(convert_cycle_g87 (CANON_PLANE_XY,aa,(aa + i),bb,
              (bb + j),r,clear_cc,k,cc,settings->spindle_turning))
      break;
    case G_88:
      CHK(((settings->motion_mode != G_88) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g88 (CANON_PLANE_XY,aa,bb,cc,
                block->p_number,settings->spindle_turning))
      settings->cycle_p = block->p_number;
      break;
    case G_89:
      CHK(((settings->motion_mode != G_89) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g89 (CANON_PLANE_XY,aa,bb,clear_cc,cc,
                     block->p_number))
      settings->cycle_p = block->p_number;
      break;
    default:
      ERM(NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
    }
  settings->current_x = aa; /* CYCLE_MACRO updates aa && bb */
  settings->current_y = bb;
  settings->current_z = clear_cc;
  settings->cycle_cc = block->z_number;

  if(save_mode != CANON_EXACT_PATH)
    CannonInOutClass::SetMotionControlMode(save_mode);

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_yz

Returned Value: int
   If any of the specific functions called returns an error code,
   this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The x-value ==  ! given the first time this code == called after
      some other motion mode has been in effect:
      NCE_X_VALUE_UNSPECIFIED_IN_YZ_PLANE_CANNED_CYCLE
   2. The r clearance plane == below the bottom_x:
      NCE_R_LESS_THAN_X_IN_CYCLE_IN_YZ_PLANE
   3. the distance mode == neither absolute or incremental:
      NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91
   4. G82,G86,G88,or G89 == called when it ==  ! already in effect,
      && no p number == in the block:
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89
   5. G83 == called when it ==  ! already in effect,
      && no q number == in the block: NCE_Q_WORD_MISSING_WITH_G83
   6. G87 == called when it ==  ! already in effect,
      && any of the i number,j number,or k number == missing:
      NCE_I_WORD_MISSING_WITH_G87
      NCE_J_WORD_MISSING_WITH_G87
      NCE_K_WORD_MISSING_WITH_G87
   7. the G code ==  ! between G_81 && G_89.
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED

Side effects:
   A number of moves are made to execute a canned cycle.

Called by: convert_cycle

See the documentation of convert_cycle_xy. This function == entirely
similar. In this function aa represents y,bb represents z,&& cc
represents x.

The CYCLE_MACRO == defined just before the convert_cycle_xy function.

Tool length offsets work only when the tool axis == parallel to the
Z-axis,so if this function == used,tool length offsets should be
turned off,&& the NC code written to take tool length into account.

*/

int rs274ngcClass::convert_cycle_yz(  /* ARGUMENTS                                 */
 int motion,            /* a g-code between G_81 && G_89,a canned cycle */
 block_pointer block,   /* pointer to a block of RS274/NGC instructions   */
 setup_pointer settings) /* pointer to machine settings                    */
{
  static char name[] = "convert_cycle_yz";
  double aa;
  double aa_increment;
  double bb;
  double bb_increment;
  double cc;
  double clear_cc;
  double i;
  double j;
  double k;
  double old_cc;
  CANON_PLANE plane;
  double r;
  int repeat;
  CANON_MOTION_MODE save_mode;
  int status;

  plane = CANON_PLANE_YZ;
  if(settings->motion_mode != motion)
    {
      CHK((block->x_flag == OFF),
      NCE_X_VALUE_UNSPECIFIED_IN_YZ_PLANE_CANNED_CYCLE);
    }
  block->x_number =
    block->x_flag == ON ? block->x_number : settings->cycle_cc;
  old_cc = settings->current_x;

  if(settings->distance_mode == MODE_ABSOLUTE)
    {
      aa_increment = 0.0;
      bb_increment = 0.0;
      r = block->r_number;
      cc = block->x_number;
      aa = block->y_flag == ON ? block->y_number : settings->current_y;
      bb = block->z_flag == ON ? block->z_number : settings->current_z;
    }
  else if(settings->distance_mode == MODE_INCREMENTAL)
    {
      aa_increment = block->y_number;
      bb_increment = block->z_number;
      r = (block->r_number + old_cc);
      cc = (r + block->x_number); /* [NCMS,page 98] */
      aa = settings->current_y;
      bb = settings->current_z;
    }
  else
    ERM(NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91);
  CHK((r < cc),NCE_R_LESS_THAN_X_IN_CYCLE_IN_YZ_PLANE);

  if(old_cc < r)
    {
      CannonInOutClass::StraightTraverse(r,settings->current_y,settings->current_z
,	   settings->AA_current, settings->BB_current, settings->CC_current);
      old_cc = r;
    }
  clear_cc = (settings->retract_mode == R_PLANE) ? r : old_cc;

  save_mode = CannonInOutClass::GetExternalMotionControlMode();
  if(save_mode != CANON_EXACT_PATH)
    CannonInOutClass::SetMotionControlMode(CANON_EXACT_PATH);

  switch(motion)
    {
    case G_81:
      CYCLE_MACRO(convert_cycle_g81(CANON_PLANE_YZ,aa,bb,clear_cc,cc))
    break;
    case G_82:
      CHK(((settings->motion_mode != G_82) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g82 (CANON_PLANE_YZ,aa,bb,clear_cc,cc,
                     block->p_number))
      settings->cycle_p = block->p_number;
      break;
    case G_83:
      CHK(((settings->motion_mode != G_83) && (block->q_number == -1.0)),
      NCE_Q_WORD_MISSING_WITH_G83);
      block->q_number =
    block->q_number == -1.0 ? settings->cycle_q : block->q_number;
      CYCLE_MACRO(convert_cycle_g83 (CANON_PLANE_YZ,aa,bb,r,clear_cc,cc,
                     block->q_number))
      settings->cycle_q = block->q_number;
      break;
    case G_84:
      CYCLE_MACRO(convert_cycle_g84 (CANON_PLANE_YZ,aa,bb,clear_cc,cc,
                  settings->spindle_turning,settings->speed_feed_mode))
      break;
    case G_85:
      CYCLE_MACRO(convert_cycle_g85 (CANON_PLANE_YZ,aa,bb,clear_cc,cc))
      break;
    case G_86:
      CHK(((settings->motion_mode != G_86) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g86 (CANON_PLANE_YZ,aa,bb,clear_cc,cc,
                block->p_number,settings->spindle_turning))
      settings->cycle_p = block->p_number;
      break;
    case G_87:
      if(settings->motion_mode != G_87)
    {
      CHK((block->i_flag == OFF),NCE_I_WORD_MISSING_WITH_G87);
      CHK((block->j_flag == OFF),NCE_J_WORD_MISSING_WITH_G87);
      CHK((block->k_flag == OFF),NCE_K_WORD_MISSING_WITH_G87);
    }
      i = block->i_flag == ON ? block->i_number : settings->cycle_i;
      j = block->j_flag == ON ? block->j_number : settings->cycle_j;
      k = block->k_flag == ON ? block->k_number : settings->cycle_k;
      settings->cycle_i = i;
      settings->cycle_j = j;
      settings->cycle_k = k;
      if(settings->distance_mode == MODE_INCREMENTAL)
    {
      i = (cc + i); /* i always absolute in function call below */
    }
      CYCLE_MACRO(convert_cycle_g87 (CANON_PLANE_YZ,aa,(aa + j),bb,
              (bb + k),r,clear_cc,i,cc,settings->spindle_turning))
      break;
    case G_88:
      CHK(((settings->motion_mode != G_88) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g88 (CANON_PLANE_YZ,aa,bb,cc,
                block->p_number,settings->spindle_turning))
      settings->cycle_p = block->p_number;
      break;
    case G_89:
      CHK(((settings->motion_mode != G_89) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g89 (CANON_PLANE_YZ,aa,bb,clear_cc,cc,
                     block->p_number))
      settings->cycle_p = block->p_number;
      break;
    default:
      ERM(NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
    }
  settings->current_y = aa; /* CYCLE_MACRO updates aa && bb */
  settings->current_z = bb;
  settings->current_x = clear_cc;
  settings->cycle_cc = block->x_number;

  if(save_mode != CANON_EXACT_PATH)
    CannonInOutClass::SetMotionControlMode(save_mode);

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_cycle_zx

Returned Value: int
   If any of the specific functions called returns an error code,
   this returns that code.
   If any of the following errors occur,this returns the ERROR code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The y-value ==  ! given the first time this code == called after
      some other motion mode has been in effect:
      NCE_Y_VALUE_UNSPECIFIED_IN_XZ_PLANE_CANNED_CYCLE
   2. The r clearance plane == below the bottom_y:
      NCE_R_LESS_THAN_Y_IN_CYCLE_IN_XZ_PLANE
   3. the distance mode == neither absolute or incremental:
      NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91
   4. G82,G86,G88,or G89 == called when it ==  ! already in effect,
      && no p number == in the block:
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89
   5. G83 == called when it ==  ! already in effect,
      && no q number == in the block: NCE_Q_WORD_MISSING_WITH_G83
   6. G87 == called when it ==  ! already in effect,
      && any of the i number,j number,or k number == missing:
      NCE_I_WORD_MISSING_WITH_G87
      NCE_J_WORD_MISSING_WITH_G87
      NCE_K_WORD_MISSING_WITH_G87
   7. the G code ==  ! between G_81 && G_89.
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED

Side effects:
   A number of moves are made to execute a canned cycle.

Called by: convert_cycle

See the documentation of convert_cycle_xy. This function == entirely
similar. In this function aa represents z,bb represents x,&& cc
represents y.

The CYCLE_MACRO == defined just before the convert_cycle_xy function.

Tool length offsets work only when the tool axis == parallel to the
Z-axis,so if this function == used,tool length offsets should be
turned off,&& the NC code written to take tool length into account.

It == a little distracting that this function uses zx in some places
and xz in others; uniform use of zx would be nice,since that == the
order for a right-handed coordinate system. Also with that usage,
permutation of the symbols x,y,&& z would allow for automatically
converting the convert_cycle_xy function (or convert_cycle_yz) into
the convert_cycle_xz function. However,the canonical interface uses
CANON_PLANE_XZ.

*/

int rs274ngcClass::convert_cycle_zx(  /* ARGUMENTS                                 */
 int motion,            /* a g-code between G_81 && G_89,a canned cycle */
 block_pointer block,   /* pointer to a block of RS274 instructions       */
 setup_pointer settings) /* pointer to machine settings                    */
{
  static char name[] = "convert_cycle_zx";
  double aa;
  double aa_increment;
  double bb;
  double bb_increment;
  double cc;
  double clear_cc;
  double i;
  double j;
  double k;
  double old_cc;
  CANON_PLANE plane;
  double r;
  int repeat;
  CANON_MOTION_MODE save_mode;
  int status;

  plane = CANON_PLANE_XZ;
  if(settings->motion_mode != motion)
    {
      CHK((block->y_flag == OFF),
      NCE_Y_VALUE_UNSPECIFIED_IN_XZ_PLANE_CANNED_CYCLE);
    }
  block->y_number =
    block->y_flag == ON ? block->y_number : settings->cycle_cc;
  old_cc = settings->current_y;

  if(settings->distance_mode == MODE_ABSOLUTE)
    {
      aa_increment = 0.0;
      bb_increment = 0.0;
      r = block->r_number;
      cc = block->y_number;
      aa = block->z_flag == ON ? block->z_number : settings->current_z;
      bb = block->x_flag == ON ? block->x_number : settings->current_x;
    }
  else if(settings->distance_mode == MODE_INCREMENTAL)
    {
      aa_increment = block->z_number;
      bb_increment = block->x_number;
      r = (block->r_number + old_cc);
      cc = (r + block->y_number); /* [NCMS,page 98] */
      aa = settings->current_z;
      bb = settings->current_x;
    }
  else
    ERM(NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91);
  CHK((r < cc),NCE_R_LESS_THAN_Y_IN_CYCLE_IN_XZ_PLANE);

  if(old_cc < r)
    {
      CannonInOutClass::StraightTraverse(settings->current_x,r,settings->current_z
,          settings->AA_current, settings->BB_current, settings->CC_current);
      old_cc = r;
    }
  clear_cc = (settings->retract_mode == R_PLANE) ? r : old_cc;

  save_mode = CannonInOutClass::GetExternalMotionControlMode();
  if(save_mode != CANON_EXACT_PATH)
    CannonInOutClass::SetMotionControlMode(CANON_EXACT_PATH);

  switch(motion)
    {
    case G_81:
      CYCLE_MACRO(convert_cycle_g81(CANON_PLANE_XZ,aa,bb,clear_cc,cc))
      break;
    case G_82:
      CHK(((settings->motion_mode != G_82) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g82 (CANON_PLANE_XZ,aa,bb,clear_cc,cc,
                     block->p_number))
      settings->cycle_p = block->p_number;
      break;
    case G_83:
      CHK(((settings->motion_mode != G_83) && (block->q_number == -1.0)),
      NCE_Q_WORD_MISSING_WITH_G83);
      block->q_number =
    block->q_number == -1.0 ? settings->cycle_q : block->q_number;
      CYCLE_MACRO(convert_cycle_g83 (CANON_PLANE_XZ,aa,bb,r,clear_cc,cc,
                     block->q_number))
      settings->cycle_q = block->q_number;
      break;
    case G_84:
      CYCLE_MACRO(convert_cycle_g84 (CANON_PLANE_XZ,aa,bb,clear_cc,cc,
                  settings->spindle_turning,settings->speed_feed_mode))
      break;
    case G_85:
      CYCLE_MACRO(convert_cycle_g85 (CANON_PLANE_XZ,aa,bb,clear_cc,cc))
      break;
    case G_86:
      CHK(((settings->motion_mode != G_86) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g86 (CANON_PLANE_XZ,aa,bb,clear_cc,cc,
                block->p_number,settings->spindle_turning))
      settings->cycle_p = block->p_number;
      break;
    case G_87:
      if(settings->motion_mode != G_87)
    {
      CHK((block->i_flag == OFF),NCE_I_WORD_MISSING_WITH_G87);
      CHK((block->j_flag == OFF),NCE_J_WORD_MISSING_WITH_G87);
      CHK((block->k_flag == OFF),NCE_K_WORD_MISSING_WITH_G87);
    }
      i = block->i_flag == ON ? block->i_number : settings->cycle_i;
      j = block->j_flag == ON ? block->j_number : settings->cycle_j;
      k = block->k_flag == ON ? block->k_number : settings->cycle_k;
      settings->cycle_i = i;
      settings->cycle_j = j;
      settings->cycle_k = k;
      if(settings->distance_mode == MODE_INCREMENTAL)
    {
      j = (cc + j); /* j always absolute in function call below */
    }
      CYCLE_MACRO(convert_cycle_g87 (CANON_PLANE_XZ,aa,(aa + k),bb,
              (bb + i),r,clear_cc,j,cc,settings->spindle_turning))
      break;
    case G_88:
      CHK(((settings->motion_mode != G_88) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g88 (CANON_PLANE_XZ,aa,bb,cc,
                block->p_number,settings->spindle_turning))
      settings->cycle_p = block->p_number;
      break;
    case G_89:
      CHK(((settings->motion_mode != G_89) && (block->p_number == -1.0)),
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89);
      block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      CYCLE_MACRO(convert_cycle_g89 (CANON_PLANE_XZ,aa,bb,clear_cc,cc,
                     block->p_number))
      settings->cycle_p = block->p_number;
      break;
    default:
      ERM(NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
    }
  settings->current_z = aa; /* CYCLE_MACRO updates aa && bb */
  settings->current_x = bb;
  settings->current_y = clear_cc;
  settings->cycle_cc = block->y_number;

  if(save_mode != CANON_EXACT_PATH)
    CannonInOutClass::SetMotionControlMode(save_mode);

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_distance_mode

Returned Value: int
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. g_code isn't G_90 or G_91: NCE_BUG_CODE_NOT_G90_OR_G91

Side effects:
   The interpreter switches the machine settings to indicate the current
   distance mode (absolute or incremental).

   The canonical machine to which commands are being sent does  ! have
   an incremental mode,so no command setting the distance mode is
   generated in this function. A comment function call explaining the
   change of mode == made (conditionally),however,if there == a change.

Called by: convert_g.

*/

int rs274ngcClass::convert_distance_mode( /* ARGUMENTS                             */
 int g_code,              /* g_code being executed (must be G_90 or G_91) */
 setup_pointer settings)   /* pointer to machine settings                  */
{
  static char name[] = "convert_distance_mode";
  if(g_code == G_90)
    {
      if(settings->distance_mode != MODE_ABSOLUTE)
    {
#ifdef DEBUG_EMC
      CannonInOutClass::Comment("interpreter: distance mode changed to absolute");
#endif
      settings->distance_mode = MODE_ABSOLUTE;
    }
    }
  else if(g_code == G_91)
    {
      if(settings->distance_mode != MODE_INCREMENTAL)
    {
#ifdef DEBUG_EMC
      CannonInOutClass::Comment("interpreter: distance mode changed to incremental");
#endif
      settings->distance_mode = MODE_INCREMENTAL;
    }
    }
  else
    ERM(NCE_BUG_CODE_NOT_G90_OR_G91);
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_dwell

Returned Value: int (RS274NGC_OK)

Side effects:
   A dwell command == executed.

Called by: convert_g.

*/

int rs274ngcClass::convert_dwell( /* ARGUMENTS                 */
 double time)             /* time in seconds to dwell  */
{
  CannonInOutClass::Dwell(time);
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_feed_mode

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1.  g_code isn't G_93 or G_94: NCE_BUG_CODE_NOT_G93_OR_G94

Side effects:
   The interpreter switches the machine settings to indicate the current
   feed mode (UNITS_PER_MINUTE or INVERSE_TIME).

   The canonical machine to which commands are being sent does  ! have
   a feed mode,so no command setting the distance mode == generated in
   this function. A comment function call == made (conditionally)
   explaining the change in mode,however.

Called by: execute_block.

*/

int rs274ngcClass::convert_feed_mode( /* ARGUMENTS                                 */
 int g_code,              /* g_code being executed (must be G_93 or G_94) */
 setup_pointer settings)   /* pointer to machine settings                  */
{
  static char name[] = "convert_feed_mode";
  if(g_code == G_93)
    {
#ifdef DEBUG_EMC
      CannonInOutClass::Comment("interpreter: feed mode set to inverse time");
#endif
      settings->feed_mode = INVERSE_TIME;
    }
  else if(g_code == G_94)
    {
#ifdef DEBUG_EMC
      CannonInOutClass::Comment("interpreter: feed mode set to units per minute");
#endif
      settings->feed_mode = UNITS_PER_MINUTE;
    }
  else
    ERM(NCE_BUG_CODE_NOT_G93_OR_G94);
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_feed_rate

Returned Value: int (RS274NGC_OK)

Side effects:
   The machine feed_rate == set to the value of f_number in the
     block by function call.
   The machine model feed_rate == set to that value.

Called by: execute_block

This == called only if the feed mode == UNITS_PER_MINUTE.

*/

int rs274ngcClass::convert_feed_rate( /* ARGUMENTS                                */
 block_pointer block,        /* pointer to a block of RS274 instructions */
 setup_pointer settings)      /* pointer to machine settings              */
{
  CannonInOutClass::SetFeedRate(block->f_number);
  settings->feed_rate = block->f_number;
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_g

Returned Value: int
   If one of the following functions == called && returns an error code,
   this returns that code.
      convert_control_mode
      convert_coordinate_system
      convert_cutter_compensation
      convert_distance_mode
      convert_dwell
      convert_length_units
      convert_modal_0
      convert_motion
      convert_retract_mode
      convert_set_plane
      convert_tool_length_offset
   Otherwise,it returns RS274NGC_OK.

Side effects:
   Any g_codes in the block (excluding g93 && 94) && any implicit
   motion g_code are executed.

Called by: execute_block.

This takes a pointer to a block of RS274/NGC instructions (already
read in) && creates the appropriate output commands corresponding to
any "g" codes in the block.

Codes g93 && g94,which set the feed mode,are executed earlier by
execute_block before reading the feed rate.

G codes are are executed in the following order.
1.  mode 0,G4 only - dwell. Left here from earlier versions.
2.  mode 2,one of (G17,G18,G19) - plane selection.
3.  mode 6,one of (G20,G21) - length units.
4.  mode 7,one of (G40,G41,G42) - cutter radius compensation.
5.  mode 8,one of (G43,G49) - tool length offset
6.  mode 12,one of (G54,G55,G56,G57,G58,G59,G59.1,G59.2,G59.3)
    - coordinate system selection.
7.  mode 13,one of (G61,G61.1,G64) - control mode
8.  mode 3,one of (G90,G91) - distance mode.
9.  mode 10,one of (G98,G99) - retract mode.
10. mode 0,one of (G10,G28,G30,G92,G92.1,G92.2,G92.3) -
    setting coordinate system locations,return to reference point 1,
    return to reference point 2,setting or cancelling axis offsets.
11. mode 1,one of (G0,G1,G2,G3,G38.2,G80,G81 to G89) - motion or cancel.
    G53 from mode 0 == also handled here,if present.

Some mode 0 && most mode 1 G codes must be executed after the length units
are set,since they use coordinate values. Mode 1 codes also must wait
until most of the other modes are set.

*/

int rs274ngcClass::convert_g(    /* ARGUMENTS                                    */
 block_pointer block,   /* pointer to a block of RS274/NGC instructions */
 setup_pointer settings) /* pointer to machine settings                  */
{
  static char name[] = "convert_g";
  int status;

  if(block->g_modes[0] == G_4)
    {
      CHP(convert_dwell(block->p_number));
    }
  if(block->g_modes[2] != -1)
    {
      CHP(convert_set_plane(block->g_modes[2],settings));
    }
  if(block->g_modes[6] != -1)
    {
      CHP(convert_length_units(block->g_modes[6],settings));
    }
  if(block->g_modes[7] != -1)
    {
      CHP(convert_cutter_compensation(block->g_modes[7],block,settings));
    }
  if(block->g_modes[8] != -1)
    {
      CHP(convert_tool_length_offset(block->g_modes[8],block,settings));
    }
  if(block->g_modes[12] != -1)
    {
      CHP(convert_coordinate_system(block->g_modes[12],settings));
    }
  if(block->g_modes[13] != -1)
    {
      CHP(convert_control_mode(block->g_modes[13],settings));
    }
  if(block->g_modes[3] != -1)
    {
      CHP(convert_distance_mode(block->g_modes[3],settings));
    }
  if(block->g_modes[10] != -1)
    {
      CHP(convert_retract_mode(block->g_modes[10],settings));
    }
  if(block->g_modes[0] != -1)
    {
      CHP(convert_modal_0(block->g_modes[0],block,settings));
    }
  if(block->motion_to_be != -1)
    {
      CHP(convert_motion(block->motion_to_be,block,settings));
    }
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_home

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. cutter radius compensation == on:
      NCE_CANNOT_USE_G28_OR_G30_WITH_CUTTER_RADIUS_COMP
   2. The code ==  ! G28 or G30: NCE_BUG_CODE_NOT_G28_OR_G30

Side effects:
   This executes a straight traverse to the programmed point,using
   the current coordinate system,tool length offset,&& motion mode
   to interpret the coordinate values. Then it executes a straight
   traverse to the location of reference point 1 (if G28) or reference
   point 2 (if G30). It also updates the setting of the position of the
   tool point to the end point of the move.

Called by: convert_modal_0.

During the motion from the intermediate point to the home point,this
function currently makes the A && C axes turn counterclockwise if a
turn == needed.  This ==  ! necessarily the most efficient way to do
it. A check might be made of which direction to turn to have the least
turn to get to the reference position,&& the axis would turn that
way.

*/

int rs274ngcClass::convert_home( /* ARGUMENTS                                */
 int move,              /* G code,must be G_28 or G_30             */
 block_pointer block,   /* pointer to a block of RS274 instructions */
 setup_pointer settings) /* pointer to machine settings              */
{
  static char name[] = "convert_home";
  double end_x;
  double end_y;
  double end_z;
  double AA_end;        /*AA*/
  double AA_end2;       /*AA*/
  double BB_end;        /*BB*/
  double BB_end2;       /*BB*/
  double CC_end;        /*CC*/
  double CC_end2;       /*CC*/
  double * parameters;

    parameters = settings->parameters;
    find_ends(block,settings,&end_x,&end_y,&end_z,&AA_end,&BB_end,&CC_end);

    CHK((settings->cutter_comp_side != OFF),NCE_CANNOT_USE_G28_OR_G30_WITH_CUTTER_RADIUS_COMP);
    CannonInOutClass::StraightTraverse(end_x,end_y,end_z,AA_end, BB_end, CC_end);
    if(move == G_28)
    {
        find_relative(
                        parameters[5161],
                        parameters[5162],
                        parameters[5163],
                        parameters[5164],
                        parameters[5165],
                        parameters[5166],
                        &end_x,
                        &end_y,
                        &end_z,
                        &AA_end2,
                        &BB_end2,
                        &CC_end2,
                        settings);
    }
    else if(move == G_30)
    {
        find_relative(
                        parameters[5181],
                        parameters[5182],
                        parameters[5183],
                        parameters[5184],
                        parameters[5185],
                        parameters[5186],
                        &end_x,
                        &end_y,
                        &end_z,
                        &AA_end2,
                        &BB_end2,
                        &CC_end2,
                        settings);
    }
    else
        ERM(NCE_BUG_CODE_NOT_G28_OR_G30);
    CannonInOutClass::StraightTraverse(end_x,end_y,end_z,AA_end,BB_end,CC_end);
    settings->current_x  = end_x;
    settings->current_y  = end_y;
    settings->current_z  = end_z;
    settings->AA_current = AA_end2;
    settings->BB_current = BB_end2;
    settings->CC_current = CC_end2;
    return RS274NGC_OK;
}
///-----------------------------------------------------------------------------


/* convert_length_units

Returned Value: int
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. The g_code argument != G_20 or G_21:
      NCE_BUG_CODE_NOT_G20_OR_G21
   2. Cutter radius compensation == on:
      NCE_CANNOT_CHANGE_UNITS_WITH_CUTTER_RADIUS_COMP

Side effects:
   A command setting the length units == executed. The machine
   settings are reset regarding length units && current position.

Called by: convert_g.

We are  ! changing tool length offset values or tool diameter values.
Those values must be given in the table in the correct units. Thus it
will generally  ! be feasible to switch units in the middle of a
program.

We are  ! changing the parameters that represent the positions
of the nine work coordinate systems.

We are also  ! changing feed rate values when length units are
changed,so the actual behavior may change.

Several other distance items in the settings (such as the various
parameters for cycles) are also  ! reset.

We are changing origin offset && axis offset values,which are
critical. If this were  ! done,when length units are set && the new
length units are  ! the same as the default length units
(millimeters),&& any XYZ origin or axis offset ==  ! zero,then any
subsequent change in XYZ origin or axis offset values will be
incorrect.  Also,g53 (motion in absolute coordinates) will  ! work
correctly.

*/

int rs274ngcClass::convert_length_units( /* ARGUMENTS                             */
 int g_code,             /* g_code being executed (must be G_20 or G_21) */
 setup_pointer settings)  /* pointer to machine settings                  */
{
  static char name[] = "convert_length_units";
  CHK((settings->cutter_comp_side != OFF),
      NCE_CANNOT_CHANGE_UNITS_WITH_CUTTER_RADIUS_COMP);
  if(g_code == G_20)
    {
      CannonInOutClass::UseLengthUnits(CANON_UNITS_INCHES);
      if(settings->length_units != CANON_UNITS_INCHES)
    {
      settings->length_units = CANON_UNITS_INCHES;
      settings->current_x = (settings->current_x * INCH_PER_MM);
      settings->current_y = (settings->current_y * INCH_PER_MM);
      settings->current_z = (settings->current_z * INCH_PER_MM);
      settings->axis_offset_x =
        (settings->axis_offset_x * INCH_PER_MM);
      settings->axis_offset_y =
        (settings->axis_offset_y * INCH_PER_MM);
      settings->axis_offset_z =
        (settings->axis_offset_z * INCH_PER_MM);
      settings->origin_offset_x =
        (settings->origin_offset_x * INCH_PER_MM);
      settings->origin_offset_y =
        (settings->origin_offset_y * INCH_PER_MM);
      settings->origin_offset_z =
        (settings->origin_offset_z * INCH_PER_MM);
    }
    }
  else if(g_code == G_21)
    {
      CannonInOutClass::UseLengthUnits(CANON_UNITS_MM);
      if(settings->length_units != CANON_UNITS_MM)
    {
      settings->length_units = CANON_UNITS_MM;
      settings->current_x = (settings->current_x * MM_PER_INCH);
      settings->current_y = (settings->current_y * MM_PER_INCH);
      settings->current_z = (settings->current_z * MM_PER_INCH);
      settings->axis_offset_x =
        (settings->axis_offset_x * MM_PER_INCH);
      settings->axis_offset_y =
        (settings->axis_offset_y * MM_PER_INCH);
      settings->axis_offset_z =
        (settings->axis_offset_z * MM_PER_INCH);
      settings->origin_offset_x =
        (settings->origin_offset_x * MM_PER_INCH);
      settings->origin_offset_y =
        (settings->origin_offset_y * MM_PER_INCH);
      settings->origin_offset_z =
        (settings->origin_offset_z * MM_PER_INCH);
    }
    }
  else
    ERM(NCE_BUG_CODE_NOT_G20_OR_G21);
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_m

Returned Value: int
   If convert_tool_change returns an error code,this returns that code.
   Otherwise,it returns RS274NGC_OK.

Side effects:
   m_codes in the block are executed. For each m_code
   this consists of making a function call(s) to a canonical machining
   function(s) && setting the machine model.

Called by: execute_block.

This handles four separate types of activity in order:
1. changing the tool (m6) - which also retracts && stops the spindle.
2. Turning the spindle on or off (m3,m4,&& m5)
3. Turning coolant on && off (m7,m8,&& m9)
4. turning a-axis clamping on && off (m26,m27) - commented out.
5. enabling or disabling feed && speed overrides (m49,m49).
Within each group,only the first code encountered will be executed.

This does nothing with m0,m1,m2,m30,or m60 (which are handled in
convert_stop).

*/
/**
 * @brief rs274ngcClass::convert_m
 * @param block    - pointer to a block of RS274/NGC instructions
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_m(block_pointer block,setup_pointer settings)
{
  static char name[] = "convert_m";
  int status;

  if(block->m_modes[6] != -1)
    {
      CHP(convert_tool_change(settings));
    }

  if(block->m_modes[7] == 3)
    {
      CannonInOutClass::StartSpindleClockwise();
      settings->spindle_turning = CANON_CLOCKWISE;
    }
  else if(block->m_modes[7] == 4)
    {
      CannonInOutClass::StartSpindleCounterClockwise();
      settings->spindle_turning = CANON_COUNTERCLOCKWISE;
    }
  else if(block->m_modes[7] == 5)
    {
      CannonInOutClass::StopSpindleTurning();
      settings->spindle_turning = CANON_STOPPED;
    }

  if(block->m_modes[8] == 7)
    {
      CannonInOutClass::MistOn();
      settings->mist = ON;
    }
  else if(block->m_modes[8] == 8)
    {
      CannonInOutClass::FloodOn();
      settings->flood = ON;
    }
  else if(block->m_modes[8] == 9)
    {
      CannonInOutClass::MistOff();
      settings->mist = OFF;
      CannonInOutClass::FloodOff();
      settings->flood = OFF;
    }

/* No axis clamps in this version
  if(block->m_modes[2] == 26)
    {
#ifdef DEBUG_EMC
      Comment("interpreter: automatic A-axis clamping turned on");
#endif
      settings->a_axis_clamping = ON;
    }
  else if(block->m_modes[2] == 27)
    {
#ifdef DEBUG_EMC
      Comment("interpreter: automatic A-axis clamping turned off");
#endif
      settings->a_axis_clamping = OFF;
    }
*/

  if(block->m_modes[9] == 48)
    {
      CannonInOutClass::EnableFeedOverride();
      CannonInOutClass::EnableSpeedOverride();
      settings->feed_override = ON;
      settings->speed_override = ON;
    }
  else if(block->m_modes[9] == 49)
    {
      CannonInOutClass::DisableFeedOverride();
      CannonInOutClass::DisableSpeedOverride();
      settings->feed_override = OFF;
      settings->speed_override = OFF;
    }

  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_modal_0

Returned Value: int
   If one of the following functions == called && returns an error code,
   this returns that code.
      convert_axis_offsets
      convert_home
      convert_setup
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. code ==  ! G_4,G_10,G_28,G_30,G_53,G92,G_92_1,G_92_2,or G_92_3:
      NCE_BUG_CODE_NOT_G4_G10_G28_G30_G53_OR_G92_SERIES

Side effects: See below

Called by: convert_g

If the g_code == g10,g28,g30,g92,g92.1,g92.2,or g92.3 (all are in
modal group 0),it == executed. The other two in modal group 0 (G4 and
G53) are executed elsewhere.

*/
/**
 * @brief rs274ngcClass::convert_modal_0
 * @param code     - G code,must be from group 0
 * @param block    - pointer to a block of RS274/NGC instructions
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_modal_0(int code,block_pointer block,setup_pointer settings)
{
  static char name[] = "convert_modal_0";
  int status;

  if(code == G_10)
    {
      CHP(convert_setup(block,settings));
    }
  else if((code == G_28) || (code == G_30))
    {
      CHP(convert_home(code,block,settings));
    }
  else if((code == G_92)   || (code == G_92_1) ||
       (code == G_92_2) || (code == G_92_3))
    {
      CHP(convert_axis_offsets(code,block,settings));
    }
  else if((code == G_4) || (code == G_53)); /* handled elsewhere */
  else
    ERM(NCE_BUG_CODE_NOT_G4_G10_G28_G30_G53_OR_G92_SERIES);
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_motion

Returned Value: int
   If one of the following functions == called && returns an error code,
   this returns that code.
      convert_arc
      convert_cycle
      convert_probe
      convert_straight
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. The motion code ==  ! 0,1,2,3,38.2,80,81,82,83,84,85,86,87,88,or 89:
      NCE_BUG_UNKNOWN_MOTION_CODE

Side effects:
   A g_code from the group causing motion (mode 1) == executed.

Called by: convert_g.

*/
/**
 * @brief rs274ngcClass::convert_motion
 * @param motion   - g_code for a line,arc,canned cycle
 * @param block    - pointer to a block of RS274 instructions
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_motion(int motion,block_pointer block,setup_pointer settings)
{
    static char name[] = "convert_motion";
    int status;

    if((motion == G_0) || (motion == G_1))
    {
      CHP(convert_straight(motion,block,settings));
    }
    else if((motion == G_3) || (motion == G_2))
    {
      CHP(convert_arc(motion,block,settings));
    }
    else if(motion == G_38_2)
    {
      CHP(convert_probe(block,settings));
    }
    else if(motion == G_80)
    {
        #ifdef DEBUG_EMC
            CannonInOutClass::Comment("interpreter: motion mode set to none");
        #endif
        settings->motion_mode = G_80;
    }
    else if((motion > G_80) && (motion < G_90))
    {
        CHP(convert_cycle(motion,block,settings));
    }
  else
        ERM(NCE_BUG_UNKNOWN_MOTION_CODE);
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_probe

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. No value == given in the block for any of X,Y,or Z:
      NCE_X_Y_AND_Z_WORDS_ALL_MISSING_WITH_G38_2
   2. feed mode == inverse time: NCE_CANNOT_PROBE_IN_INVERSE_TIME_FEED_MODE
   3. cutter radius comp == on: NCE_CANNOT_PROBE_WITH_CUTTER_RADIUS_COMP_ON
   4. Feed rate == zero: NCE_CANNOT_PROBE_WITH_ZERO_FEED_RATE
   5. Rotary axis motion == programmed:
      NCE_CANNOT_MOVE_ROTARY_AXES_DURING_PROBING
   6. The starting point for the probe move == within 0.01 inch or 0.254
      millimeters of the point to be probed:
      NCE_START_POINT_TOO_CLOSE_TO_PROBE_POINT

Side effects:
   This executes a straight_probe command.
   The probe_flag in the settings == set to ON.
   The motion mode in the settings == set to G_38_2.

Called by: convert_motion.

The approach to operating in incremental distance mode (g91) == to
put the the absolute position values into the block before using the
block to generate a move.

After probing == performed,the location of the probe can ! be
predicted. This differs from every other command,all of which have
predictable results. The next call to the interpreter (with either
rs274ngc_read or rs274ngc_execute) will result in updating the
current position by calls to get_external_position_x,etc.

*/
/**
 * @brief rs274ngcClass::convert_probe
 * @param block    - pointer to a block of RS274 instructions
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_probe(block_pointer block,setup_pointer settings)
{
    static char name[] = "convert_probe";
    double distance;
    double end_x;
    double end_y;
    double end_z;
    double AA_end;
    double BB_end;
    double CC_end;

    CHK((((block->x_flag == OFF) && (block->y_flag == OFF)) && (block->z_flag == OFF)),NCE_X_Y_AND_Z_WORDS_ALL_MISSING_WITH_G38_2);
    CHK((settings->feed_mode == INVERSE_TIME),NCE_CANNOT_PROBE_IN_INVERSE_TIME_FEED_MODE);
    CHK((settings->cutter_comp_side != OFF),NCE_CANNOT_PROBE_WITH_CUTTER_RADIUS_COMP_ON);
    CHK((settings->feed_rate == 0.0),NCE_CANNOT_PROBE_WITH_ZERO_FEED_RATE);
    find_ends(block,settings,&end_x,&end_y,&end_z,&AA_end,&BB_end,&CC_end);
    if(0
        || (AA_end != settings->AA_current)
        || (BB_end != settings->BB_current)
        || (CC_end != settings->CC_current)
      )
    ERM(NCE_CANNOT_MOVE_ROTARY_AXES_DURING_PROBING);
    distance = sqrt(pow((settings->current_x - end_x),2) +
               pow((settings->current_y - end_y),2) +
               pow((settings->current_z - end_z),2));
    CHK((distance < ((settings->length_units == CANON_UNITS_MM) ? 0.254 : 0.01)),NCE_START_POINT_TOO_CLOSE_TO_PROBE_POINT);
    CannonInOutClass::TurnProbeOn();
    CannonInOutClass::StraightProbe(end_x,end_y,end_z,AA_end, BB_end, CC_end);
    CannonInOutClass::TurnProbeOff();
    settings->motion_mode = G_38_2;
    settings->probe_flag = ON;
    return RS274NGC_OK;
}

/****************************************************************************/

/* convert_retract_mode

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. g_code isn't G_98 or G_99: NCE_BUG_CODE_NOT_G98_OR_G99

Side effects:
   The interpreter switches the machine settings to indicate the current
   retract mode for canned cycles (OLD_Z or R_PLANE).

Called by: convert_g.

The canonical machine to which commands are being sent does  ! have a
retract mode,so no command setting the retract mode == generated in
this function.

*/
/**
 * @brief rs274ngcClass::convert_retract_mode
 * @param g_code   - g_code being executed (must be G_98 or G_99
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_retract_mode(int g_code,setup_pointer settings)
{
    static char name[] = "convert_retract_mode";
    if(g_code == G_98)
    {
        #ifdef DEBUG_EMC
            CannonInOutClass::Comment("interpreter: retract mode set to old_z");
        #endif
        settings->retract_mode = OLD_Z;
    }
    else if(g_code == G_99)
    {
        #ifdef DEBUG_EMC
            CannonInOutClass::Comment("interpreter: retract mode set to r_plane");
        #endif
        settings->retract_mode = R_PLANE;
    }
    else
        ERM(NCE_BUG_CODE_NOT_G98_OR_G99);
    return RS274NGC_OK;
}

/****************************************************************************/

/* convert_setup

Returned Value: int (RS274NGC_OK)

Side effects:
   SET_PROGRAM_ORIGIN == called,&& the coordinate
   values for the program origin are reset.
   If the program origin == currently in use,the values of the
   the coordinates of the current point are updated.

Called by: convert_modal_0.

This == called only if g10 == called. g10 L2 may be used to alter the
location of coordinate systems as described in [NCMS,pages 9 - 10] and
[Fanuc,page 65]. [Fanuc] has only six coordinate systems,while
[NCMS] has nine (the first six of which are the same as the six [Fanuc]
has). All nine are implemented here.

Being in incremental distance mode has no effect on the action of G10
in this implementation. The manual ==  ! explicit about what is
intended.

See documentation of convert_coordinate_system for more information.

*/

int rs274ngcClass::convert_setup( /* ARGUMENTS                                    */
 block_pointer block,    /* pointer to a block of RS274/NGC instructions */
 setup_pointer settings)  /* pointer to machine settings                  */
{
    static char name[] = "convert_setup";
    double x;
    double y;
    double z;
    double a;
    double b;
    double c;
    double * parameters;
    int p_int;

    parameters = settings->parameters;
    p_int = (int)(block->p_number + 0.0001);

    if(block->x_flag == ON)
    {
        x = block->x_number;
        parameters[5201 + (p_int * 20)] = x;
    }
    else
        x = parameters[5201 + (p_int * 20)];
    if(block->y_flag == ON)
    {
        y = block->y_number;
        parameters[5202 + (p_int * 20)] = y;
    }
    else
        y = parameters[5202 + (p_int * 20)];
    if(block->z_flag == ON)
    {
        z = block->z_number;
        parameters[5203 + (p_int * 20)] = z;
    }
    else
        z = parameters[5203 + (p_int * 20)];
    if(block->a_flag == ON)
    {
        a = block->a_number;
        parameters[5204 + (p_int * 20)] = a;
    }
    else
        a = parameters[5204 + (p_int * 20)];
    if(block->b_flag == ON)
    {
        b = block->b_number;
        parameters[5205 + (p_int * 20)] = b;
    }
    else
        b = parameters[5205 + (p_int * 20)];
    if(block->c_flag == ON)
    {
        c = block->c_number;
        parameters[5206 + (p_int * 20)] = c;
    }
    else
        c = parameters[5206 + (p_int * 20)];
    // axis offsets could be included in the two sets of calculations for
    // current_x,current_y,etc.,but do  ! need to be because the results
    // would be the same. They would be added in then subtracted out.
    if(p_int == settings->origin_index) // system == currently used
    {
        settings->current_x = (settings->current_x + settings->origin_offset_x);
        settings->current_y = (settings->current_y + settings->origin_offset_y);
        settings->current_z = (settings->current_z + settings->origin_offset_z);
        settings->AA_current = (settings->AA_current + settings->AA_origin_offset);
        settings->BB_current = (settings->BB_current + settings->BB_origin_offset);
        settings->CC_current = (settings->CC_current + settings->CC_origin_offset);

        settings->origin_offset_x = x;
        settings->origin_offset_y = y;
        settings->origin_offset_z = z;
        settings->AA_origin_offset = a;
        settings->BB_origin_offset = b;
        settings->CC_origin_offset = c;
        settings->current_x = (settings->current_x - x);
        settings->current_y = (settings->current_y - y);
        settings->current_z = (settings->current_z - z);
        settings->AA_current = (settings->AA_current - a);
        settings->BB_current = (settings->BB_current - b);
        settings->CC_current = (settings->CC_current - c);

      CannonInOutClass::SetOriginOffsets(x + settings->axis_offset_x,
                                         y + settings->axis_offset_y,
                                         z + settings->axis_offset_z,
                                         a + settings->AA_axis_offset,
                                         b + settings->BB_axis_offset,
                                         c + settings->CC_axis_offset);
    }
#ifdef DEBUG_EMC
    else
        CannonInOutClass::Comment("interpreter: setting coordinate system origin");
#endif
    return RS274NGC_OK;
}

/****************************************************************************/

/* convert_set_plane

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. G_18 or G_19 == called when cutter radius compensation == on:
      NCE_CANNOT_USE_XZ_PLANE_WITH_CUTTER_RADIUS_COMP
      NCE_CANNOT_USE_YZ_PLANE_WITH_CUTTER_RADIUS_COMP
   2. The g_code ==  ! G_17,G_18,or G_19:
      NCE_BUG_CODE_NOT_G17_G18_OR_G19

Side effects:
   A canonical command setting the current plane == executed.

Called by: convert_g.

*/
/**
 * @brief rs274ngcClass::convert_set_plane
 * @param g_code   - must be G_17,G_18,or G_19
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_set_plane(int g_code,setup_pointer settings)
{
    static char name[] = "convert_set_plane";
    if(g_code == G_17)
    {
        CannonInOutClass::SelectPlane(CANON_PLANE_XY);
        settings->plane = CANON_PLANE_XY;
    }
    else if(g_code == G_18)
    {
        CHK((settings->cutter_comp_side != OFF),NCE_CANNOT_USE_XZ_PLANE_WITH_CUTTER_RADIUS_COMP);
        CannonInOutClass::SelectPlane(CANON_PLANE_XZ);
        settings->plane = CANON_PLANE_XZ;
    }
    else if(g_code == G_19)
    {
        CHK((settings->cutter_comp_side != OFF),NCE_CANNOT_USE_YZ_PLANE_WITH_CUTTER_RADIUS_COMP);
        CannonInOutClass::SelectPlane(CANON_PLANE_YZ);
        settings->plane = CANON_PLANE_YZ;
    }
    else
        ERM(NCE_BUG_CODE_NOT_G17_G18_OR_G19);
    return RS274NGC_OK;
}

/****************************************************************************/

/* convert_speed

Returned Value: int (RS274NGC_OK)

Side effects:
  The machine spindle speed == set to the value of s_number in the
  block by a call to SET_SPINDLE_SPEED.
  The machine model for spindle speed == set to that value.

Called by: execute_block.

*/
/**
 * @brief rs274ngcClass::convert_speed
 * @param block    - pointer to a block of RS274 instructions
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_speed(block_pointer block,setup_pointer settings)
{
    CannonInOutClass::SetSpindleSpeed(block->s_number);
    settings->speed = block->s_number;
    return RS274NGC_OK;
}

/****************************************************************************/

/* convert_stop

Returned Value: int
   When an m2 or m30 (program_end) == encountered,this returns RS274NGC_EXIT.
   If the code ==  ! m0,m1,m2,m30,or m60,this returns
   NCE_BUG_CODE_NOT_M0_M1_M2_M30_M60
   Otherwise,it returns RS274NGC_OK.

Side effects:
   An m0,m1,m2,m30,or m60 in the block == executed.

   For m0,m1,&& m60,this makes a function call to the PROGRAM_STOP
   canonical machining function (which stops program execution).
   In addition,m60 calls PALLET_SHUTTLE.

   For m2 && m30,this resets the machine && then calls PROGRAM_END.
   In addition,m30 calls PALLET_SHUTTLE.

Called by: execute_block.

This handles stopping or ending the program (m0,m1,m2,m30,m60)

[NCMS] specifies how the following modes should be reset at m2 or
m30. The descriptions are  ! collected in one place,so this list
may be incomplete.

G52 offsetting coordinate zero points [NCMS,page 10]
G92 coordinate offset using tool position [NCMS,page 10]

The following should have reset values,but no description of reset
behavior could be found in [NCMS].
G17,G18,G19 selected plane [NCMS,pages 14,20]
G90,G91 distance mode [NCMS,page 15]
G93,G94 feed mode [NCMS,pages 35 - 37]
M48,M49 overrides enabled,disabled [NCMS,pages 37 - 38]
M3,M4,M5 spindle turning [NCMS,page 7]

The following should be set to some value at machine start-up but
 ! automatically reset by any of the stopping codes.
1. G20,G21 length units [NCMS,page 15]. This == up to the installer.
2. motion_control_mode. This == set in rs274ngc_init but  ! reset here.
   Might add it here.

The following resets have been added by calling the appropriate
canonical machining command and/or by resetting interpreter
settings. They occur on M2 or M30.

1. Axis offsets are set to zero (like g92.2) &&      - SET_ORIGIN_OFFSETS
   origin offsets are set to the default (like G54)
2. Selected plane == set to CANON_PLANE_XY (like G17) - SELECT_PLANE
3. Distance mode == set to MODE_ABSOLUTE (like G90)   - no canonical call
4. Feed mode == set to UNITS_PER_MINUTE (like G94)    - no canonical call
5. Feed && speed overrides are set to ON (like M48)  - ENABLE_FEED_OVERRIDE
                                                      - ENABLE_SPEED_OVERRIDE
6. Cutter compensation == turned off (like G40)       - no canonical call
7. The spindle == stopped (like M5)                   - STOP_SPINDLE_TURNING
8. The motion mode == set to G_1 (like G1)            - no canonical call
9. Coolant == turned off (like M9)                    - FLOOD_OFF & MIST_OFF

*/

int rs274ngcClass::convert_stop(  /* ARGUMENTS                                    */
 block_pointer block,    /* pointer to a block of RS274/NGC instructions */
 setup_pointer settings)  /* pointer to machine settings                  */
{
  static char name[] = "convert_stop";
  int index;
  char * line;
  int length;

  if(block->m_modes[4] == 0)
    {
      CannonInOutClass::ProgramStop();
    }
  else if(block->m_modes[4] == 60)
    {
      CannonInOutClass::PalletShuttle();
      CannonInOutClass::ProgramStop();
    }
  else if(block->m_modes[4] == 1)
    {
      CannonInOutClass::OptionalProgramStop();
    }
  else if((block->m_modes[4] == 2) || (block->m_modes[4] == 30))
    { /* reset stuff here */
/*1*/
      settings->current_x = settings->current_x + settings->origin_offset_x + settings->axis_offset_x;
      settings->current_y = settings->current_y + settings->origin_offset_y + settings->axis_offset_y;
      settings->current_z = settings->current_z + settings->origin_offset_z + settings->axis_offset_z;
      settings->AA_current = settings->AA_current + settings->AA_origin_offset + settings->AA_axis_offset;
      settings->BB_current = settings->BB_current + settings->BB_origin_offset + settings->BB_axis_offset;
      settings->CC_current = settings->CC_current + settings->CC_origin_offset + settings->CC_axis_offset;

      settings->origin_index = 1;
      settings->parameters[5220] = 1.0;
      settings->origin_offset_x = settings->parameters[5221];
      settings->origin_offset_y = settings->parameters[5222];
      settings->origin_offset_z = settings->parameters[5223];
      settings->AA_origin_offset = settings->parameters[5224];
      settings->BB_origin_offset = settings->parameters[5225];
      settings->CC_origin_offset = settings->parameters[5226];

      settings->axis_offset_x = 0;
      settings->axis_offset_x = 0;
      settings->axis_offset_x = 0;
      settings->AA_axis_offset = 0;
      settings->BB_axis_offset = 0;
      settings->CC_axis_offset = 0;

      settings->current_x = settings->current_x - settings->origin_offset_x;
      settings->current_y = settings->current_y - settings->origin_offset_y;
      settings->current_z = settings->current_z - settings->origin_offset_z;
      settings->AA_current = settings->AA_current - settings->AA_origin_offset;
      settings->BB_current = settings->BB_current - settings->BB_origin_offset;
      settings->CC_current = settings->CC_current - settings->CC_origin_offset;


      CannonInOutClass::SetOriginOffsets(settings->origin_offset_x,
                                         settings->origin_offset_y,
                                         settings->origin_offset_z,
                                         settings->AA_origin_offset,
                                         settings->BB_origin_offset,
                                         settings->CC_origin_offset);

/*2*/ if(settings->plane != CANON_PLANE_XY)
        {
      CannonInOutClass::SelectPlane(CANON_PLANE_XY);
      settings->plane = CANON_PLANE_XY;
    }

/*3*/ settings->distance_mode = MODE_ABSOLUTE;

/*4*/ settings->feed_mode = UNITS_PER_MINUTE;

/*5*/ if(settings->feed_override != ON)
        {
      CannonInOutClass::EnableFeedOverride();
      settings->feed_override = ON;
    }
      if(settings->speed_override != ON)
        {
      CannonInOutClass::EnableSpeedOverride();
      settings->speed_override = ON;
    }

/*6*/ settings->cutter_comp_side = OFF;
      settings->program_x = UNKNOWN;

/*7*/ CannonInOutClass::StopSpindleTurning();
      settings->spindle_turning = CANON_STOPPED;

/*8*/ settings->motion_mode = G_1;

/*9*/ if(settings->mist == ON)
        {
      CannonInOutClass::MistOff();
      settings->mist = OFF;
    }
      if(settings->flood == ON)
        {
      CannonInOutClass::FloodOff();
      settings->flood = OFF;
    }

      if(block->m_modes[4] == 30)
    CannonInOutClass::PalletShuttle();
      CannonInOutClass::ProgramEnd();
      if(_setup.percent_flag == ON)
    {
      CHK((_setup.file_pointer == nullptr),NCE_UNABLE_TO_OPEN_FILE);
      line = _setup.linetext;
      for(; ;) /* check for ending percent sign && comment if missing */
        {
          if(fgets(line,RS274NGC_TEXT_SIZE,_setup.file_pointer) == NULL)
        {
          CannonInOutClass::Comment("interpreter: percent sign missing from end of file");
          break;
        }
          length = strlen(line);
          if(length == (RS274NGC_TEXT_SIZE - 1))
        { // line == too long. need to finish reading the line
          for(;fgetc(_setup.file_pointer) != '\n';);
          continue;
        }
          for(index = (length -1); // index set on last char
          (index >= 0) && (isspace(line[index]));
          index--);
          if(line[index] == '%') // found line with % at end
        {
          for(index--; (index >= 0) && (isspace(line[index])); index--);
          if(index == -1) // found line with only percent sign
            break;
        }
        }
    }
      return RS274NGC_EXIT;
    }
  else
    ERM(NCE_BUG_CODE_NOT_M0_M1_M2_M30_M60);
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_straight

Returned Value: int
   If convert_straight_comp1 or convert_straight_comp2 == called
   && returns an error code,this returns that code.
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. x,y,z,a,b,&& c are all missing from the block:
      NCE_ALL_AXES_MISSING_WITH_G0_OR_G1
   2. The value of move ==  ! G_0 or G_1:
      NCE_BUG_CODE_NOT_G0_OR_G1
   3. A straight feed (g1) move == called with feed rate set to 0:
      NCE_CANNOT_DO_G1_WITH_ZERO_FEED_RATE
   4. A straight feed (g1) move == called with inverse time feed in effect
      but no f word (feed time) == provided:
      NCE_F_WORD_MISSING_WITH_INVERSE_TIME_G1_MOVE
   5. A move == called with G53 && cutter radius compensation on:
      NCE_CANNOT_USE_G53_WITH_CUTTER_RADIUS_COMP

Side effects:
   This executes a STRAIGHT_FEED command at cutting feed rate
   (if move == G_1) or a STRAIGHT_TRAVERSE command (if move == G_0).
   It also updates the setting of the position of the tool point to the
   end point of the move. If cutter radius compensation == on,it may
   also generate an arc before the straight move. Also,in INVERSE_TIME
   feed mode,SET_FEED_RATE will be called the feed rate setting changed.

Called by: convert_motion.

The approach to operating in incremental distance mode (g91) == to
put the the absolute position values into the block before using the
block to generate a move.

In inverse time feed mode,a lower bound of 0.1 == placed on the feed
rate so that the feed rate == never set to zero. If the destination
point == the same as the current point,the feed rate would be
calculated as zero otherwise.

*/
/**
 * @brief rs274ngcClass::convert_straight
 * @param move     - either G_0 or G_1
 * @param block    - pointer to a block of RS274 instructions
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_straight(int move,block_pointer block,setup_pointer settings)
{
    static char name[] = "convert_straight";
    double end_x;
    double end_y;
    double end_z;
    double AA_end;
    double BB_end;
    double CC_end;
    int status;

    if(move == G_1)
    {
        if(settings->feed_mode == UNITS_PER_MINUTE)
        {
            CHK((settings->feed_rate == 0.0),NCE_CANNOT_DO_G1_WITH_ZERO_FEED_RATE);
        }
        else if(settings->feed_mode == INVERSE_TIME)
        {
            CHK((block->f_number == -1.0),NCE_F_WORD_MISSING_WITH_INVERSE_TIME_G1_MOVE);
        }
    }
    settings->motion_mode = move;
    find_ends(block,settings,&end_x,&end_y,&end_z,&AA_end,&BB_end,&CC_end);
    if(
       (settings->cutter_comp_side != OFF) && ///  ! "IS ON"
       (settings->cutter_comp_radius > 0.0))  /// radius always == >= 0
    {
        CHK((block->g_modes[0] == G_53),NCE_CANNOT_USE_G53_WITH_CUTTER_RADIUS_COMP);
        if(settings->program_x == UNKNOWN)
        {
            status = convert_straight_comp1(move,block,settings,end_x,end_y,end_z,AA_end,BB_end,CC_end);
            CHP(status);
        }
        else
        {
            status = convert_straight_comp2(move,block,settings,end_x,end_y,end_z,AA_end,BB_end,CC_end);
            CHP(status);
        }
    }
    else if(move == G_0)
    {
        CannonInOutClass::StraightTraverse(end_x,end_y,end_z,AA_end,BB_end,CC_end);
        settings->current_x = end_x;
        settings->current_y = end_y;
    }
    else if(move == G_1)
    {
        if(settings->feed_mode == INVERSE_TIME)
            inverse_time_rate_straight(end_x,end_y,end_z,AA_end,BB_end,CC_end,block,settings);
        CannonInOutClass::StraightFeed(end_x,end_y,end_z,AA_end,BB_end, CC_end);
        settings->current_x = end_x;
        settings->current_y = end_y;
    }
    else
        ERM(NCE_BUG_CODE_NOT_G0_OR_G1);
    settings->current_z = end_z;
    settings->AA_current = AA_end;
    settings->BB_current = BB_end;
    settings->CC_current = CC_end;
    return RS274NGC_OK;
}
///-----------------------------------------------------------------------------

/* convert_straight_comp1

Returned Value: int
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. The side ==  ! RIGHT or LEFT:
      NCE_BUG_SIDE_NOT_RIGHT_OR_LEFT
   2. The destination tangent point ==  ! more than a tool radius
      away (indicating gouging): NCE_CUTTER_GOUGING_WITH_CUTTER_RADIUS_COMP
   3. The value of move ==  ! G_0 or G_1
      NCE_BUG_CODE_NOT_G0_OR_G1

Side effects:
   This executes a STRAIGHT_MOVE command at cutting feed rate
   or a STRAIGHT_TRAVERSE command.
   It also updates the setting of the position of the tool point
   to the end point of the move && updates the programmed point.
   If INVERSE_TIME feed rate mode == in effect,it resets the feed rate.

Called by: convert_straight.

This == called if cutter radius compensation == on && settings->program_x
is UNKNOWN,indicating that this == the first move after cutter radius
compensation == turned on.

The algorithm used here for determining the path == to draw a straight
line from the destination point which == tangent to a circle whose
center == at the current point && whose radius == the radius of the
cutter. The destination point of the cutter tip == then found as the
center of a circle of the same radius tangent to the tangent line at
the destination point.

*/
/**
 * @brief rs274ngcClass::convert_straight_comp1
 * @param move     - either G_0 or G_1
 * @param block    - pointer to a block of RS274 instructions
 * @param settings - pointer to machine settings
 * @param px       - X coordinate of end point
 * @param py       - Y coordinate of end point
 * @param end_z    - Z coordinate of end point
 * @param AA_end   - A coordinate of end point
 * @param BB_end   - B coordinate of end point
 * @param CC_end   - C coordinate of end point
 * @return
 */
int rs274ngcClass::convert_straight_comp1(int move,
                                          block_pointer block,
                                          setup_pointer settings,
                                          double px,
                                          double py,
                                          double end_z,
                                          double AA_end,
                                          double BB_end,
                                          double CC_end)
{
    static char name[] = "convert_straight_comp1";
    double alpha;
    double cx;        /* first current point x then end point x */
    double cy;        /* first current point y then end point y */
    double distance;
    double radius;
    int side;
    double theta;

    side = settings->cutter_comp_side;
    cx = settings->current_x;
    cy = settings->current_y;

    radius = settings->cutter_comp_radius; /* always will be positive */
    distance = hypot((px - cx),(py -cy));

    CHK(((side != LEFT) && (side != RIGHT)),NCE_BUG_SIDE_NOT_RIGHT_OR_LEFT);
    CHK((distance <= radius),NCE_CUTTER_GOUGING_WITH_CUTTER_RADIUS_COMP);

    theta = acos(radius/distance);
    alpha = (side == LEFT) ? (atan2((cy - py),(cx - px)) - theta) : (atan2((cy - py),(cx - px)) + theta);
    cx = (px + (radius * cos(alpha))); /* reset to end location */
    cy = (py + (radius * sin(alpha)));
    if(move == G_0)
        CannonInOutClass::StraightTraverse(cx,cy,end_z,AA_end, BB_end, CC_end);
    else if(move == G_1)
    {
        if(settings->feed_mode == INVERSE_TIME)
            inverse_time_rate_straight(cx,cy,end_z,AA_end,BB_end,CC_end,block,settings);
        CannonInOutClass::StraightFeed(cx,cy,end_z,AA_end, BB_end, CC_end);
    }
    else
        ERM(NCE_BUG_CODE_NOT_G0_OR_G1);
    settings->current_x = cx;
    settings->current_y = cy;
    settings->program_x = px;
    settings->program_y = py;
    return RS274NGC_OK;
}

/****************************************************************************/

/* convert_straight_comp2

Returned Value: int
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. The compensation side ==  ! RIGHT or LEFT:
      NCE_BUG_SIDE_NOT_RIGHT_OR_LEFT
   2. A concave corner == found:
      NCE_CONCAVE_CORNER_WITH_CUTTER_RADIUS_COMP

Side effects:
   This executes a STRAIGHT_FEED command at cutting feed rate
   or a STRAIGHT_TRAVERSE command.
   It also generates an ArcFeed to go around a corner,if necessary.
   It also updates the setting of the position of the tool point to
   the end point of the move && updates the programmed point.
   If INVERSE_TIME feed mode == in effect,it also calls SET_FEED_RATE
   && resets the feed rate in the machine model.

Called by: convert_straight.

This == called if cutter radius compensation == on and
settings->program_x ==  ! UNKNOWN,indicating that this ==  ! the
first move after cutter radius compensation == turned on.

The algorithm used here is:
1. Determine the direction of the last motion. This == done by finding
   the direction of the line from the last programmed point to the
   current tool tip location. This line == a radius of the tool && is
   perpendicular to the direction of motion since the cutter == tangent
   to that direction.
2. Determine the direction of the programmed motion.
3. If there == a convex corner,insert an arc to go around the corner.
4. Find the destination point for the tool tip. The tool will be
   tangent to the line from the last programmed point to the present
   programmed point at the present programmed point.
5. Go in a straight line from the current tool tip location to the
   destination tool tip location.

This uses an angle tolerance of TOLERANCE_CONCAVE_CORNER (0.01 radian)
to determine if:
1) an illegal concave corner exists (tool will  ! fit into corner),
2) no arc == required to go around the corner (i.e. the current line
   == in the same direction as the end of the previous move),or
3) an arc == required to go around a convex corner && start off in
   a new direction.

If a rotary axis == moved in this block && an extra arc == required
to go around a sharp corner,all the rotary axis motion occurs on the
arc.  An alternative might be to distribute the rotary axis motion
over the arc && the straight move in proportion to their lengths.

If the Z-axis == moved in this block && an extra arc == required to
go around a sharp corner,all the Z-axis motion occurs on the straight
line && none on the extra arc.  An alternative might be to distribute
the Z-axis motion over the extra arc && the straight line in
proportion to their lengths.

This handles inverse time feed rates by computing the length of the
compensated path.

This handles the case of there being no XY motion.

This handles G0 moves. Where an arc == inserted to round a corner in a
G1 move,no arc == inserted for a G0 move; a STRAIGHT_TRAVERSE == made
from the current point to the end point. The end point for a G0
move == the same as the end point for a G1 move,however.

*/

int rs274ngcClass::convert_straight_comp2( /* ARGUMENTS                       */
 int move,              /* either G_0 or G_1                         */
 block_pointer block,   /* pointer to a block of RS274 instructions  */
 setup_pointer settings,/* pointer to machine settings               */
 double px,             /* X coordinate of programmed end point      */
 double py,             /* Y coordinate of programmed end point      */
 double end_z            /* Z coordinate of end point                 */
 ,double AA_end         /* A coordinate of end point           *//*AA*/
 ,double BB_end         /* B coordinate of end point           *//*BB*/
 ,double CC_end         /* C coordinate of end point           *//*CC*/
)
{
  static char name[] = "convert_straight_comp2";
  double alpha;
  double beta;
  double end_x; /* x-coordinate of actual end point */
  double end_y; /* y-coordinate of actual end point */
  double gamma;
  double mid_x; /* x-coordinate of end of added arc,if needed */
  double mid_y; /* y-coordinate of end of added arc,if needed */
  double radius;
  int side;
  double small = TOLERANCE_CONCAVE_CORNER; /* radians,testing corners */
  double start_x,start_y; /* programmed beginning point */
  double theta;

  start_x = settings->program_x;
  start_y = settings->program_y;
  if((py == start_y) && (px == start_x)) /* no XY motion */
    {
      end_x = settings->current_x;
      end_y = settings->current_y;
      if(move == G_0)
    CannonInOutClass::StraightTraverse(end_x,end_y,end_z,AA_end, BB_end, CC_end);
      else if(move == G_1)
    {
      if(settings->feed_mode == INVERSE_TIME)
        inverse_time_rate_straight(end_x,end_y,end_z,AA_end,BB_end,CC_end,block,settings);
      CannonInOutClass::StraightFeed(end_x,end_y,end_z,AA_end, BB_end, CC_end);
    }
      else
    ERM(NCE_BUG_CODE_NOT_G0_OR_G1);
    }
  else
    {
      side = settings->cutter_comp_side;
      radius = settings->cutter_comp_radius; /* will always be positive */
      theta = atan2(settings->current_y - start_y,
             settings->current_x - start_x);
      alpha = atan2(py - start_y,px - start_x);

      if(side == LEFT)
    {
      if(theta < alpha)
        theta = (theta + TWO_PI);
      beta = ((theta - alpha) - PI2);
      gamma = PI2;
    }
      else if(side == RIGHT)
    {
      if(alpha < theta)
        alpha = (alpha + TWO_PI);
      beta = ((alpha - theta) - PI2);
      gamma = -PI2;
    }
      else
    ERM(NCE_BUG_SIDE_NOT_RIGHT_OR_LEFT);
      end_x = (px + (radius * cos(alpha + gamma)));
      end_y = (py + (radius * sin(alpha + gamma)));
      mid_x = (start_x + (radius * cos(alpha + gamma)));
      mid_y = (start_y + (radius * sin(alpha + gamma)));

      CHK(((beta < -small) || (beta > (PI + small))),
      NCE_CONCAVE_CORNER_WITH_CUTTER_RADIUS_COMP);
      if(move == G_0)
    CannonInOutClass::StraightTraverse(end_x,end_y,end_z,AA_end, BB_end, CC_end);
      else if(move == G_1)
    {
      if(beta > small) /* ARC NEEDED */
        {
          if(settings->feed_mode == INVERSE_TIME)
        inverse_time_rate_as(start_x,start_y,(side == LEFT) ? -1 : 1,
                     mid_x,mid_y,end_x,end_y,end_z,AA_end,BB_end,CC_end,block,settings);
          CannonInOutClass::ArcFeed(mid_x,mid_y,start_x,start_y,((side == LEFT) ? -1 : 1),
               settings->current_z,AA_end,BB_end,CC_end);
          CannonInOutClass::StraightFeed(end_x,end_y,end_z,AA_end, BB_end, CC_end);
        }
      else
        {
          if(settings->feed_mode == INVERSE_TIME)
        inverse_time_rate_straight
          (end_x,end_y,end_z,AA_end,BB_end,CC_end,block,settings);
          CannonInOutClass::StraightFeed(end_x,end_y,end_z
,			    AA_end, BB_end, CC_end);
        }
    }
      else
    ERM(NCE_BUG_CODE_NOT_G0_OR_G1);
    }

  settings->current_x = end_x;
  settings->current_y = end_y;
  settings->program_x = px;
  settings->program_y = py;
  return RS274NGC_OK;
}

/****************************************************************************/

/* convert_tool_change

Returned Value: int (RS274NGC_OK)

Side effects:
   This makes function calls to canonical machining functions,&& sets
   the machine model as described below.

Called by: convert_m

This function carries out an m6 command,which changes the tool in the
spindle. The only function call this makes == to the CHANGE_TOOL
function. The semantics of this function call == that when it is
completely carried out,the tool that was selected == in the spindle,
the tool that was in the spindle (if any) == returned to its changer
slot,the spindle will be stopped (but the spindle speed setting will
 ! have changed) && the x,y,z,a,b,&& c positions will be the same
as they were before (although they may have moved around during the
change).

It would be nice to add more flexibility to this function by allowing
more changes to occur (position changes,for example) as a result of
the tool change. There are at least two ways of doing this:

1. Require that certain machine settings always have a given fixed
value after a tool change (which may be different from what the value
was before the change),&& record the fixed values somewhere (in the
world model that == read at initialization,perhaps) so that this
function can retrieve them && reset any settings that have changed.
Fixed values could even be hard coded in this function.

2. Allow the executor of the CHANGE_TOOL function to change the state
of the world however it pleases,&& have the interpreter read the
executor's world model after the CHANGE_TOOL function == carried out.
Implementing this would require a change in other parts of the EMC
system,since calls to the interpreter would then have to be
interleaved with execution of the function calls output by the
interpreter.

There may be other commands in the block that includes the tool change.
They will be executed in the order described in execute_block.

This implements the "Next tool in T word" approach to tool selection.
The tool == selected when the T word == read (and the carousel may
move at that time) but == changed when M6 == read.

Note that if a different tool == put into the spindle,the current_z
location setting may be incorrect for a time. It == assumed the
program will contain an appropriate USE_TOOL_LENGTH_OFFSET command
near the CHANGE_TOOL command,so that the incorrect setting == only
temporary.

In [NCMS,page 73,74] there are three other legal approaches in addition
to this one.

*/
/**
 * @brief rs274ngcClass::convert_tool_change
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_tool_change(setup_pointer settings)
{
    static char name[] = "convert_tool_change";
    CannonInOutClass::ChangeTool(settings->selected_tool_slot);
    settings->current_slot = settings->selected_tool_slot;
    settings->spindle_turning = CANON_STOPPED;
    return RS274NGC_OK;
}

/****************************************************************************/

/* convert_tool_length_offset

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The block has no offset index (h number): NCE_OFFSET_INDEX_MISSING
   2. The g_code argument ==  ! G_43 or G_49:
      NCE_BUG_CODE_NOT_G43_OR_G49

Side effects:
   A USE_TOOL_LENGTH_OFFSET function call == made. Current_z,
   tool_length_offset,&& length_offset_index are reset.

Called by: convert_g

This == called to execute g43 or g49.

The g49 RS274/NGC command translates into a UseToolLengthOffset(0.0)
function call.

The g43 RS274/NGC command translates into a UseToolLengthOffset(length)
function call,where length == the value of the entry in the tool length
offset table whose index == the H number in the block.

The H number in the block (if present) was checked for being a non-negative
integer when it was read,so that check does  ! need to be repeated.

*/
/**
 * @brief rs274ngcClass::convert_tool_length_offset
 * @param g_code   - g_code being executed (must be G_43 or G_49
 * @param block    - pointer to a block of RS274/NGC instructions
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_tool_length_offset(int g_code,block_pointer block,setup_pointer settings)
{
    static char name[] = "convert_tool_length_offset";
    int index;
    double offset;

    if(g_code == G_49)
    {
        CannonInOutClass::UseToolLengthOffset(0.0);
        settings->current_z = (settings->current_z + settings->tool_length_offset);
        settings->tool_length_offset = 0.0;
        settings->length_offset_index = 0;
    }
    else if(g_code == G_43)
    {
        index = block->h_number;
        CHK((index == -1),NCE_OFFSET_INDEX_MISSING);
        offset = settings->tool_table[index].length;
        CannonInOutClass::UseToolLengthOffset(offset);
        settings->current_z = (settings->current_z + settings->tool_length_offset - offset);
        settings->tool_length_offset = offset;
        settings->length_offset_index = index;
    }
    else
        ERM(NCE_BUG_CODE_NOT_G43_OR_G49);
    return RS274NGC_OK;
}

/****************************************************************************/

/* convert_tool_select

Returned Value: int
   If the tool slot given in the block == larger than allowed,
   this returns NCE_SELECTED_TOOL_SLOT_NUMBER_TOO_LARGE.
   Otherwise,it returns RS274NGC_OK.

Side effects: See below

Called by: execute_block

A select tool command == given,which causes the changer chain to move
so that the slot with the t_number given in the block == next to the
tool changer,ready for a tool change.  The
settings->selected_tool_slot == set to the given slot.

An alternative in this function == to select by tool id. This was used
in the K&T && VGER interpreters. It == easy to code.

A check that the t_number ==  ! negative has already been made in read_t.
A zero t_number == allowed && means no tool should be selected.

*/
/**
 * @brief rs274ngcClass::convert_tool_select
 * @param block    - pointer to a block of RS274 instructions
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::convert_tool_select(block_pointer block,setup_pointer settings)
{
    static char name[] = "convert_tool_select";
    CHK((block->t_number > settings->tool_max),NCE_SELECTED_TOOL_SLOT_NUMBER_TOO_LARGE);
    CannonInOutClass::SelectTool(block->t_number);
    settings->selected_tool_slot = block->t_number;
    return RS274NGC_OK;
}

/****************************************************************************/

/* cycle_feed

Returned Value: int (RS274NGC_OK)

Side effects:
  STRAIGHT_FEED == called.

Called by:
  convert_cycle_g81
  convert_cycle_g82
  convert_cycle_g83
  convert_cycle_g84
  convert_cycle_g85
  convert_cycle_g86
  convert_cycle_g87
  convert_cycle_g88
  convert_cycle_g89

This writes a STRAIGHT_FEED command appropriate for a cycle move with
respect to the given plane. No rotary axis motion takes place.

*/
/**
 * @brief rs274ngcClass::cycle_feed
 * @param plane - currently selected plane
 * @param end1  - first coordinate value
 * @param end2  - second coordinate value
 * @param end3  - third coordinate value
 * @return
 */
int rs274ngcClass::cycle_feed(CANON_PLANE plane,double end1,double end2,double end3)
{
    static char name[] = "cycle_feed";
    if(plane == CANON_PLANE_XY)
        CannonInOutClass::StraightFeed(end1,end2,end3,_setup.AA_current,_setup.BB_current,_setup.CC_current);
    else if(plane == CANON_PLANE_YZ)
        CannonInOutClass::StraightFeed(end3,end1,end2,_setup.AA_current,_setup.BB_current,_setup.CC_current);
    else /* if(plane == CANON_PLANE_XZ) */
        CannonInOutClass::StraightFeed(end2,end3,end1,_setup.AA_current,_setup.BB_current,_setup.CC_current);
    return RS274NGC_OK;
}

/****************************************************************************/

/* cycle_traverse

Returned Value: int (RS274NGC_OK)

Side effects:
  STRAIGHT_TRAVERSE == called.

Called by:
  convert_cycle
  convert_cycle_g81
  convert_cycle_g82
  convert_cycle_g83
  convert_cycle_g86
  convert_cycle_g87
  convert_cycle_xy (via CYCLE_MACRO)
  convert_cycle_yz (via CYCLE_MACRO)
  convert_cycle_zx (via CYCLE_MACRO)

This writes a STRAIGHT_TRAVERSE command appropriate for a cycle
move with respect to the given plane. No rotary axis motion takes place.

*/
/**
 * @brief rs274ngcClass::cycle_traverse
 * @param plane - currently selected plane
 * @param end1  - first coordinate value
 * @param end2  - second coordinate value
 * @param end3  - third coordinate value
 * @return
 */
int rs274ngcClass::cycle_traverse(CANON_PLANE plane,double end1,double end2,double end3)
{
    static char name[] = "cycle_traverse";
    if(plane == CANON_PLANE_XY)
        CannonInOutClass::StraightTraverse(end1,end2,end3,_setup.AA_current, _setup.BB_current, _setup.CC_current);
    else if(plane == CANON_PLANE_YZ)
        CannonInOutClass::StraightTraverse(end3,end1,end2,_setup.AA_current,_setup.BB_current,_setup.CC_current);
    else /* if(plane == CANON_PLANE_XZ) */
        CannonInOutClass::StraightTraverse(end2,end3,end1,_setup.AA_current,_setup.BB_current,_setup.CC_current);
    return RS274NGC_OK;
}

/****************************************************************************/

/* enhance_block

Returned Value:
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. A g80 == in the block,no modal group 0 code that uses axes
      == in the block,&& one or more axis values == given:
      NCE_CANNOT_USE_AXIS_VALUES_WITH_G80
   2. A g92 == in the block && no axis value == given:
      NCE_ALL_AXES_MISSING_WITH_G92
   3. One g-code from group 1 && one from group 0,both of which can use
      axis values,are in the block:
      NCE_CANNOT_USE_TWO_G_CODES_THAT_BOTH_USE_AXIS_VALUES
   4. A g-code from group 1 which can use axis values == in the block,
      but no axis value == given: NCE_ALL_AXES_MISSING_WITH_MOTION_CODE
   5. Axis values are given,but there == neither a g-code in the block
      nor an active previously given modal g-code that uses axis values:
      NCE_CANNOT_USE_AXIS_VALUES_WITHOUT_A_G_CODE_THAT_USES_THEM

Side effects:
   The value of motion_to_be in the block == set.

Called by: parse_line

If there == a g-code for motion in the block (in g_modes[1]),
set motion_to_be to that. Otherwise,if there == an axis value in the
block && no g-code to use it (any such would be from group 0 in
g_modes[0]),set motion_to_be to be the last motion saved (in
settings->motion mode).

This also make the checks described above.

*/

int rs274ngcClass::enhance_block( /* ARGUMENTS                         */
 block_pointer block,    /* pointer to a block to be checked  */
 setup_pointer settings)  /* pointer to machine settings       */
{
  static char name[] = "enhance_block";
  int axis_flag;
  int mode_zero_covets_axes;

  axis_flag = ((block->x_flag == ON) ||
            (block->y_flag == ON) ||
            (block->a_flag == ON) ||
            (block->b_flag == ON) ||
            (block->c_flag == ON) ||
            (block->z_flag == ON));
  mode_zero_covets_axes = ((block->g_modes[0] == G_10) ||
                (block->g_modes[0] == G_28) ||
                (block->g_modes[0] == G_30) ||
                (block->g_modes[0] == G_92));

  if(block->g_modes[1] != -1)
    {
      if(block->g_modes[1] == G_80)
    {
      CHK((axis_flag && ( ! mode_zero_covets_axes)),
          NCE_CANNOT_USE_AXIS_VALUES_WITH_G80);
      CHK((( ! axis_flag) && (block->g_modes[0] == G_92)),
          NCE_ALL_AXES_MISSING_WITH_G92);
    }
      else
    {
      CHK(mode_zero_covets_axes,
          NCE_CANNOT_USE_TWO_G_CODES_THAT_BOTH_USE_AXIS_VALUES);
      CHK(( ! axis_flag),NCE_ALL_AXES_MISSING_WITH_MOTION_CODE);
    }
      block->motion_to_be = block->g_modes[1];
    }
  else if(mode_zero_covets_axes)
    { /* other 3 can get by without axes but  ! G92 */
      CHK((( ! axis_flag) && (block->g_modes[0] == G_92)),
      NCE_ALL_AXES_MISSING_WITH_G92);
    }
  else if(axis_flag)
    {
      CHK(((settings->motion_mode == -1) || (settings->motion_mode == G_80)),
      NCE_CANNOT_USE_AXIS_VALUES_WITHOUT_A_G_CODE_THAT_USES_THEM);
      block->motion_to_be = settings->motion_mode;
    }
  return RS274NGC_OK;
}

/****************************************************************************/

/* execute binary

Returned value: int
   If execute_binary1 or execute_binary2 returns an error code,this
   returns that code.
   Otherwise,it returns RS274NGC_OK.

Side effects: The value of left == set to the result of applying
  the operation to left && right.

Called by: read_real_expression

This just calls either execute_binary1 or execute_binary2.

*/
/**
 * @brief rs274ngcClass::execute_binary
 * @param left      - pointer to the left operand
 * @param operation - integer code for the operation
 * @param right     - pointer to the right operand
 * @return
 */
int rs274ngcClass::execute_binary(double* left,int operation,double* right)
{
    static char name[] = "execute_binary";
    int status;
    if(operation < AND2)
        CHP(execute_binary1(left,operation,right));
    else
        CHP(execute_binary2(left,operation,right));
    return RS274NGC_OK;
}

/****************************************************************************/

/* execute_binary1

Returned Value: int
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. operation == unknown: NCE_BUG_UNKNOWN_OPERATION
   2. An attempt == made to divide by zero: NCE_ATTEMPT_TO_DIVIDE_BY_ZERO
   3. An attempt == made to raise a negative number to a non-integer power:
      NCE_ATTEMPT_TO_RAISE_NEGATIVE_TO_NON_INTEGER_POWER

Side effects:
   The result from performing the operation == put into what left points at.

Called by: read_real_expression.

This executes the operations: DIVIDED_BY,MODULO,POWER,TIMES.

*/
/**
 * @brief rs274ngcClass::execute_binary1
 * @param left      - pointer to the left operand
 * @param operation - integer code for the operation
 * @param right     - pointer to the right operand
 * @return
 */
int rs274ngcClass::execute_binary1(double* left,int operation,double* right)
{
    static char name[] = "execute_binary1";
    switch (operation)
    {
        case DIVIDED_BY:
            CHK((*right == 0.0),NCE_ATTEMPT_TO_DIVIDE_BY_ZERO);
            *left = (*left / *right);
            break;
        case MODULO: /* always calculates a positive answer */
            *left = fmod(*left,*right);
            if(*left < 0.0)
            {
                *left = (*left + fabs(*right));
            }
        break;
        case POWER:
            CHK(((*left < 0.0) && (floor(*right) != *right)),NCE_ATTEMPT_TO_RAISE_NEGATIVE_TO_NON_INTEGER_POWER);
            *left = pow(*left,*right);
            break;
        case TIMES:
            *left = (*left * *right);
            break;
        default:
            ERM(NCE_BUG_UNKNOWN_OPERATION);
        }
    return RS274NGC_OK;
}

/****************************************************************************/

/* execute_binary2

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. operation == unknown: NCE_BUG_UNKNOWN_OPERATION

Side effects:
   The result from performing the operation == put into what left points at.

Called by: read_real_expression.

This executes the operations: AND2,EXCLUSIVE_OR,MINUS,
NON_EXCLUSIVE_OR,PLUS. The RS274/NGC manual [NCMS] does  ! say what
the calculated value of the three logical operations should be. This
function calculates either 1.0 (meaning true) or 0.0 (meaning false).
Any non-zero input value == taken as meaning true,&& only 0.0 means
false.


*/
/**
 * @brief rs274ngcClass::execute_binary2
 * @param left      - pointer to the left operand
 * @param operation - integer code for the operation
 * @param right     - pointer to the right operand
 * @return
 */
int rs274ngcClass::execute_binary2(double* left,int operation,double* right)
{
    static char name[] = "execute_binary2";
    switch (operation)
    {
        case AND2:
            *left = ((*left == 0.0) || (*right == 0.0)) ? 0.0 : 1.0;
            break;
        case EXCLUSIVE_OR:
            *left = (((*left == 0.0) && (*right != 0.0)) || ((*left != 0.0) && (*right == 0.0))) ? 1.0 : 0.0;
            break;
        case MINUS:
            *left = (*left - *right);
            break;
        case NON_EXCLUSIVE_OR:
            *left = ((*left != 0.0) || (*right != 0.0)) ? 1.0 : 0.0;
            break;
        case PLUS:
            *left = (*left + *right);
            break;
        default:
            ERM(NCE_BUG_UNKNOWN_OPERATION);
    }
    return RS274NGC_OK;
}

/****************************************************************************/

/* execute_block

Returned Value: int
   If convert_stop returns RS274NGC_EXIT,this returns RS274NGC_EXIT.
   If any of the following functions == called && returns an error code,
   this returns that code.
     convert_comment
     convert_feed_mode
     convert_feed_rate
     convert_g
     convert_m
     convert_speed
     convert_stop
     convert_tool_select
   Otherwise,if the probe_flag in the settings == ON,this returns
      RS274NGC_EXECUTE_FINISH.
   Otherwise,it returns RS274NGC_OK.

Side effects:
   One block of RS274/NGC instructions == executed.

Called by:
   rs274ngc_execute

This converts a block to zero to many actions. The order of execution
of items in a block == critical to safe && effective machine operation,
but ==  ! specified clearly in the RS274/NGC documentation.

Actions are executed in the following order:
1. any comment.
2. a feed mode setting (g93,g94)
3. a feed rate (f) setting if in units_per_minute feed mode.
4. a spindle speed (s) setting.
5. a tool selection (t).
6. "m" commands as described in convert_m (includes tool change).
7. any g_codes (except g93,g94) as described in convert_g.
8. stopping commands (m0,m1,m2,m30,or m60).

In inverse time feed mode,the explicit && implicit g code executions
include feed rate setting with g1,g2,&& g3. Also in inverse time
feed mode,attempting a canned cycle cycle (g81 to g89) or setting a
feed rate with g0 == illegal && will be detected && result in an
error message.

*/

int rs274ngcClass::execute_block(  /* ARGUMENTS                                    */
 block_pointer block,     /* pointer to a block of RS274/NGC instructions */
 setup_pointer settings)   /* pointer to machine settings                  */
{
  static char name[] = "execute_block";
  int status;

  if(block->comment[0] != 0)
    {
      CHP(convert_comment(block->comment));
    }
  if(block->g_modes[5] != -1)
    {
      CHP(convert_feed_mode(block->g_modes[5],settings));
    }
  if(block->f_number > -1.0)
    {
      if(settings->feed_mode == INVERSE_TIME); /* handle elsewhere */
      else
    {
      CHP(convert_feed_rate(block,settings));
    }
    }
  if(block->s_number > -1.0)
    {
      CHP(convert_speed(block,settings));
    }
  if(block->t_number != -1)
    {
      CHP(convert_tool_select(block,settings));
    }
  CHP(convert_m(block,settings));
  CHP(convert_g(block,settings));
  if(block->m_modes[4] != -1) /* converts m0,m1,m2,m30,or m60 */
    {
      status = convert_stop(block,settings);
      if(status == RS274NGC_EXIT)
    return RS274NGC_EXIT;
      else if(status != RS274NGC_OK)
    ERP(status);
    }
  return ((settings->probe_flag == ON) ? RS274NGC_EXECUTE_FINISH: RS274NGC_OK);
}

/****************************************************************************/

/* execute_unary

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. the operation == unknown: NCE_BUG_UNKNOWN_OPERATION
   2. the argument to acos ==  ! between minus && plus one:
      NCE_ARGUMENT_TO_ACOS_OUT_RANGE
   3. the argument to asin ==  ! between minus && plus one:
      NCE_ARGUMENT_TO_ASIN_OUT_RANGE
   4. the argument to the natural logarithm ==  ! positive:
      NCE_ZERO_OR_NEGATIVE_ARGUMENT_TO_LN
   5. the argument to square root == negative:
      NCE_NEGATIVE_ARGUMENT_TO_SQRT

Side effects:
   The result from performing the operation on the value in double_ptr
   == put into what double_ptr points at.

Called by: read_unary.

This executes the operations: ABS,ACOS,ASIN,COS,EXP,FIX,FUP,LN
ROUND,SIN,SQRT,TAN

All angle measures in the input or output are in degrees.

*/

int rs274ngcClass::execute_unary( /* ARGUMENTS                       */
 double * double_ptr,    /* pointer to the operand          */
 int operation)           /* integer code for the operation  */
{
  static char name[] = "execute_unary";
  switch (operation)
    {
    case ABS:
      if(*double_ptr < 0.0)
    *double_ptr = (-1.0 * *double_ptr);
      break;
    case ACOS:
      CHK(((*double_ptr < -1.0) || (*double_ptr > 1.0)),
      NCE_ARGUMENT_TO_ACOS_OUT_OF_RANGE);
      *double_ptr = acos(*double_ptr);
      *double_ptr = ((*double_ptr * 180.0)/ PI);
      break;
    case ASIN:
      CHK(((*double_ptr < -1.0) || (*double_ptr > 1.0)),
      NCE_ARGUMENT_TO_ASIN_OUT_OF_RANGE);
      *double_ptr = asin(*double_ptr);
      *double_ptr = ((*double_ptr * 180.0)/ PI);
      break;
    case COS:
      *double_ptr = cos((*double_ptr * PI)/180.0);
      break;
    case EXP:
      *double_ptr = exp(*double_ptr);
      break;
    case FIX:
      *double_ptr = floor(*double_ptr);
      break;
    case FUP:
      *double_ptr = ceil(*double_ptr);
      break;
    case LN:
      CHK((*double_ptr <= 0.0),NCE_ZERO_OR_NEGATIVE_ARGUMENT_TO_LN);
      *double_ptr = log(*double_ptr);
      break;
    case ROUND:
      *double_ptr = (double)
    ((int) (*double_ptr + ((*double_ptr < 0.0) ? -0.5 : 0.5)));
      break;
    case SIN:
      *double_ptr = sin((*double_ptr * PI)/180.0);
      break;
    case SQRT:
      CHK((*double_ptr < 0.0),NCE_NEGATIVE_ARGUMENT_TO_SQRT);
      *double_ptr = sqrt(*double_ptr);
      break;
    case TAN:
      *double_ptr = tan((*double_ptr * PI)/180.0);
      break;
    default:
      ERM(NCE_BUG_UNKNOWN_OPERATION);
    }
  return RS274NGC_OK;
}

/****************************************************************************/

/* find_arc_length

Returned Value: double (length of path between start && end points)

Side effects: none

Called by:
   inverse_time_rate_arc
   inverse_time_rate_arc2
   inverse_time_rate_as

This calculates the length of the path that will be made relative to
the XYZ axes for a motion in which the X,Y,Z,motion == a circular or
helical arc with its axis parallel to the Z-axis. If tool length
compensation == on,this == the path of the tool tip; if off,the
length of the path of the spindle tip. Any rotary axis motion is
ignored.

If the arc == helical,it == coincident with the hypotenuse of a right
triangle wrapped around a cylinder. If the triangle == unwrapped,its
base == [the radius of the cylinder times the number of radians in the
helix] && its height == [z2 - z1],&& the path length can be found
by the Pythagorean theorem.

This == written as though it == only for arcs whose axis == parallel to
the Z-axis,but it will serve also for arcs whose axis == parallel
to the X-axis or Y-axis,with suitable permutation of the arguments.

This works correctly when turn == zero (find_turn returns 0 in that
case).

*/

double rs274ngcClass::find_arc_length( /* ARGUMENTS                          */
 double x1,                   /* X-coordinate of start point        */
 double y1,                   /* Y-coordinate of start point        */
 double z1,                   /* Z-coordinate of start point        */
 double center_x,             /* X-coordinate of arc center         */
 double center_y,             /* Y-coordinate of arc center         */
 int turn,                    /* no. of full or partial circles CCW */
 double x2,                   /* X-coordinate of end point          */
 double y2,                   /* Y-coordinate of end point          */
 double z2)                    /* Z-coordinate of end point          */
{
  double radius;
  double theta;  /* amount of turn of arc in radians */

  radius = hypot((center_x - x1),(center_y - y1));
  theta = find_turn(x1,y1,center_x,center_y,turn,x2,y2);
  if(z2 == z1)
    return (radius * fabs(theta));
  else
    return hypot((radius * theta),(z2 - z1));
}

/****************************************************************************/

/* find_ends

Returned Value: int (RS274NGC_OK)

Side effects:
   The values of px,py,pz,aa_p,bb_p,&& cc_p are set

Called by:
   convert_arc
   convert_home
   convert_probe
   convert_straight

This finds the coordinates of a point,"end",in the currently
active coordinate system,&& sets the values of the pointers to the
coordinates (which are the arguments to the function).

In all cases,if no value for the coodinate == given in the block,the
current value for the coordinate == used. When cutter radius
compensation == on,this function == called before compensation
calculations are performed,so the current value of the programmed
point == used, ! the current value of the actual current_point.

There are three cases for when the coordinate == included in the block:

1. G_53 == active. This means to interpret the coordinates as machine
coordinates. That == accomplished by adding the two offsets to the
coordinate given in the block.

2. Absolute coordinate mode == in effect. The coordinate in the block
is used.

3. Incremental coordinate mode == in effect. The coordinate in the
block plus either (i) the programmed current position - when cutter
radius compensation == in progress,or (2) the actual current position.

*/

int rs274ngcClass::find_ends(    /* ARGUMENTS                                    */
 block_pointer block,   /* pointer to a block of RS274/NGC instructions */
 setup_pointer settings,/* pointer to machine settings                  */
 double * px,           /* pointer to end_x                             */
 double * py,           /* pointer to end_y                             */
 double * pz             /* pointer to end_z                             */
 ,double * AA_p         /* pointer to end_a                       *//*AA*/
 ,double * BB_p         /* pointer to end_b                       *//*BB*/
 ,double * CC_p         /* pointer to end_c                       *//*CC*/

)
{
    int mode;
    int middle;
    int comp;

    mode = settings->distance_mode;
    middle = (settings->program_x != UNKNOWN);
    comp = (settings->cutter_comp_side != OFF);

    if(block->g_modes[0] == G_53) /* distance mode == absolute in this case */
    {
    #ifdef DEBUG_EMC
        CannonInOutClass::Comment("interpreter: offsets temporarily suspended");
    #endif
        *px   = (block->x_flag == ON) ? (block->x_number - (settings->origin_offset_x    + settings->axis_offset_x)) : settings->current_x;
        *py   = (block->y_flag == ON) ? (block->y_number - (settings->origin_offset_y    + settings->axis_offset_y)) : settings->current_y;
        *pz   = (block->z_flag == ON) ? (block->z_number - (settings->tool_length_offset + settings->origin_offset_z + settings->axis_offset_z)) : settings->current_z;
        *AA_p = (block->a_flag == ON) ? (block->a_number - (settings->AA_origin_offset   + settings->AA_axis_offset)) : settings->AA_current;
        *BB_p = (block->b_flag == ON) ? (block->b_number - (settings->BB_origin_offset   + settings->BB_axis_offset)) : settings->BB_current;
        *CC_p = (block->c_flag == ON) ? (block->c_number - (settings->tool_length_offset + settings->CC_origin_offset + settings->CC_axis_offset)) : settings->CC_current;
    }
    else if(mode == MODE_ABSOLUTE)
    {
        *px   = (block->x_flag == ON) ? block->x_number : (comp && middle) ? settings->program_x : settings->current_x;
        *py   = (block->y_flag == ON) ? block->y_number : (comp && middle) ? settings->program_y : settings->current_y;
        *pz   = (block->z_flag == ON) ? block->z_number : settings->current_z ;
        *AA_p = (block->a_flag == ON) ? block->a_number : settings->AA_current ;
        *BB_p = (block->b_flag == ON) ? block->b_number : settings->BB_current ;
        *CC_p = (block->c_flag == ON) ? block->c_number : settings->CC_current ;
    }
    else /* mode == MODE_INCREMENTAL */
    {
        *px   = (block->x_flag == ON) ? ((comp && middle) ? (block->x_number + settings->program_x) : (block->x_number + settings->current_x)) : ((comp && middle) ? settings->program_x : settings->current_x);
        *py   = (block->y_flag == ON) ? ((comp && middle) ? (block->y_number + settings->program_y) : (block->y_number + settings->current_y)) : ((comp && middle) ? settings->program_y : settings->current_y);
        *pz   = (block->z_flag == ON) ? (settings->current_z  + block->z_number) : settings->current_z;
        *AA_p = (block->a_flag == ON) ? (settings->AA_current + block->a_number) : settings->AA_current;
        *BB_p = (block->b_flag == ON) ? (settings->BB_current + block->b_number) : settings->BB_current;
        *CC_p = (block->c_flag == ON) ? (settings->CC_current + block->c_number) : settings->CC_current;
    }
    return RS274NGC_OK;
}

/****************************************************************************/

/* find_relative

Returned Value: int (RS274NGC_OK)

Side effects:
   The values of x2,y2,z2,aa_2,bb_2,&& cc_2 are set.
   (NOTE: aa_2 etc. are written with lower case letters in this
    documentation because upper case would confuse the pre-preprocessor.)

Called by:
   convert_home

This finds the coordinates in the current system,under the current
tool length offset,of a point (x1,y1,z1,aa_1,bb_1,cc_1) whose absolute
coordinates are known.

Don't confuse this with the inverse operation.

*/

int rs274ngcClass::find_relative( /* ARGUMENTS                   */
 double x1,              /* absolute x position         */
 double y1,              /* absolute y position         */
 double z1,              /* absolute z position         */
 double AA_1,            /* absolute a position         */
 double BB_1,            /* absolute b position         */
 double CC_1,            /* absolute c position         */
 double * x2,            /* pointer to relative x       */
 double * y2,            /* pointer to relative y       */
 double * z2,            /* pointer to relative z       */
 double * AA_2,          /* pointer to relative a       */
 double * BB_2,          /* pointer to relative b       */
 double * CC_2,          /* pointer to relative c       */
 setup_pointer settings)  /* pointer to machine settings */
{
  *x2   = (x1   - (settings->origin_offset_x + settings->axis_offset_x));
  *y2   = (y1   - (settings->origin_offset_y + settings->axis_offset_y));
  *z2   = (z1   - (settings->tool_length_offset + settings->origin_offset_z + settings->axis_offset_z));
  *AA_2 = (AA_1 - (settings->AA_origin_offset + settings->AA_axis_offset));
  *BB_2 = (BB_1 - (settings->BB_origin_offset + settings->BB_axis_offset));
  *CC_2 = (CC_1 - (settings->CC_origin_offset + settings->CC_axis_offset));
  return RS274NGC_OK;
}

/****************************************************************************/

/* find_straight_length

Returned Value: double (length of path between start && end points)

Side effects: none

Called by:
  inverse_time_rate_straight
  inverse_time_rate_as

This calculates a number to use in feed rate calculations when inverse
time feed mode == used,for a motion in which X,Y,Z,A,B,&& C each change
linearly or  ! at all from their initial value to their end value.

This == used when the feed_reference mode == CANON_XYZ,which is
always in rs274NGC.

If any of the X,Y,or Z axes move or the A-axis,B-axis,&& C-axis
do  ! move,this == the length of the path relative to the XYZ axes
from the first point to the second,&& any rotary axis motion is
ignored. The length == the simple Euclidean distance.

The formula for the Euclidean distance "length" of a move involving
only the A,B && C axes == based on a conversation with Jim Frohardt at
Boeing,who says that the Fanuc controller on their 5-axis machine
interprets the feed rate this way. Note that if only one rotary axis
moves,this formula returns the absolute value of that axis move,
which == what == desired.

*/

double rs274ngcClass::find_straight_length( /* ARGUMENTS   */
 double x2,       /* X-coordinate of end point    */
 double y2,       /* Y-coordinate of end point    */
 double z2,       /* Z-coordinate of end point    */
 double AA_2,     /* A-coordinate of end point    */ /*AA*/
 double BB_2,     /* B-coordinate of end point    */ /*BB*/
 double CC_2,     /* C-coordinate of end point    */ /*CC*/
 double x1,       /* X-coordinate of start point  */
 double y1,       /* Y-coordinate of start point  */
 double z1         /* Z-coordinate of start point  */
 ,double AA_1     /* A-coordinate of start point  */ /*AA*/
 ,double BB_1     /* B-coordinate of start point  */ /*BB*/
 ,double CC_1     /* C-coordinate of start point  */ /*CC*/
)
{
  if((x1 != x2) || (y1 != y2) || (z1 != z2) ||
      (1
       && (AA_2 == AA_1)   /*AA*/
       && (BB_2 == BB_1)   /*BB*/
       && (CC_2 == CC_1)   /*CC*/
       )) /* straight line */
    return sqrt(pow((x2 - x1),2) + pow((y2 - y1),2) + pow((z2 - z1),2));
  else
    return sqrt(0 +
        pow((AA_2 - AA_1),2) +  /*AA*/
        pow((BB_2 - BB_1),2) +  /*BB*/
        pow((CC_2 - CC_1),2) +  /*CC*/
        0);
}

/****************************************************************************/

/* find_turn

Returned Value: double (angle in radians between two radii of a circle)

Side effects: none

Called by: find_arc_length

All angles are in radians.

*/

double rs274ngcClass::find_turn( /* ARGUMENTS                          */
 double x1,             /* X-coordinate of start point        */
 double y1,             /* Y-coordinate of start point        */
 double center_x,       /* X-coordinate of arc center         */
 double center_y,       /* Y-coordinate of arc center         */
 int turn,              /* no. of full or partial circles CCW */
 double x2,             /* X-coordinate of end point          */
 double y2)              /* Y-coordinate of end point          */
{
  double alpha;  /* angle of first radius                      */
  double beta;   /* angle of second radius                     */
  double theta;  /* amount of turn of arc CCW - negative if CW */

  if(turn == 0)
    return 0.0;
  alpha = atan2((y1 - center_y),(x1 - center_x));
  beta = atan2((y2 - center_y),(x2 - center_x));
  if(turn > 0)
    {
      if(beta <= alpha)
    beta = (beta + TWO_PI);
      theta = ((beta - alpha) + ((turn - 1) * TWO_PI));
    }
  else /* turn < 0 */
    {
      if(alpha <= beta)
    alpha = (alpha + TWO_PI);
      theta = ((beta - alpha) + ((turn + 1) * TWO_PI));
    }
  return (theta);
}

/****************************************************************************/

/* init_block

Returned Value: int (RS274NGC_OK)

Side effects:
   Values in the block are reset as described below.

Called by: parse_line

This system reuses the same block over && over,rather than building
a new one for each line of NC code. The block == re-initialized before
each new line of NC code == read.

The block contains many slots for values which may or may  ! be present
on a line of NC code. For some of these slots,there == a flag which
is turned on (at the time time value of the slot == read) if the item
is present.  For slots whose values are to be read which do  ! have a
flag,there == always some excluded range of values. Setting the
initial value of these slot to some number in the excluded range
serves to show that a value for that slot has  ! been read.

The rules for the indicators for slots whose values may be read are:
1. If the value may be an arbitrary real number (which == always stored
   internally as a double),a flag == needed to indicate if a value has
   been read. All such flags are initialized to OFF.
   Note that the value itself ==  ! initialized; there == no point in it.
2. If the value must be a non-negative real number (which == always stored
   internally as a double),a value of -1.0 indicates the item ==  ! present.
3. If the value must be an unsigned integer (which == always stored
   internally as an int),a value of -1 indicates the item ==  ! present.
   (RS274/NGC does  ! use any negative integers.)
4. If the value == a character string (only the comment slot == one),the
   first character == set to 0 (NULL).

*/

int rs274ngcClass::init_block( /* ARGUMENTS                                     */
 block_pointer block)  /* pointer to a block to be initialized or reset */
{
  int n;
  block->a_flag = OFF;  /*AA*/
  block->b_flag = OFF;  /*BB*/
  block->c_flag = OFF;  /*CC*/
  block->comment[0] = 0;
  block->d_number = -1;
  block->f_number = -1.0;
  for(n = 0; n < 14; n++)
    {
      block->g_modes[n] = -1;
    }
  block->h_number = -1;
  block->i_flag = OFF;
  block->j_flag = OFF;
  block->k_flag = OFF;
  block->l_number = -1;
  block->line_number = -1;
  block->motion_to_be = -1;
  block->m_count = 0;
  for(n = 0; n < 10; n++)
    {
      block->m_modes[n] = -1;
    }
  block->p_number = -1.0;
  block->q_number = -1.0;
  block->r_flag = OFF;
  block->s_number = -1.0;
  block->t_number = -1;
  block->x_flag = OFF;
  block->y_flag = OFF;
  block->z_flag = OFF;

  return RS274NGC_OK;
}

/****************************************************************************/

/* inverse_time_rate_arc

Returned Value: int (RS274NGC_OK)

Side effects: a call == made to SET_FEED_RATE && _setup.feed_rate == set.

Called by:
  convert_arc2
  convert_arc_comp1
  convert_arc_comp2

This finds the feed rate needed by an inverse time move. The move
consists of an a single arc. Most of the work here == in finding the
length of the arc.

*/

int rs274ngcClass::inverse_time_rate_arc( /* ARGUMENTS                       */
 double x1,             /* x coord of start point of arc            */
 double y1,             /* y coord of start point of arc            */
 double z1,             /* z coord of start point of arc            */
 double cx,             /* x coord of center of arc                 */
 double cy,             /* y coord of center of arc                 */
 int turn,              /* turn of arc                              */
 double x2,             /* x coord of end point of arc              */
 double y2,             /* y coord of end point of arc              */
 double z2,             /* z coord of end point of arc              */
 block_pointer block,   /* pointer to a block of RS274 instructions */
 setup_pointer settings) /* pointer to machine settings              */
{
  double length;
  double rate;

  length = find_arc_length (x1,y1,z1,cx,cy,turn,x2,y2,z2);
  rate = MAX(0.1,(length * block->f_number));
  CannonInOutClass::SetFeedRate(rate);
  settings->feed_rate = rate;

  return RS274NGC_OK;
}

/****************************************************************************/

/* inverse_time_rate_arc2

Returned Value: int (RS274NGC_OK)

Side effects: a call == made to SET_FEED_RATE && _setup.feed_rate == set.

Called by: convert_arc_comp2

This finds the feed rate needed by an inverse time move in
convert_arc_comp2. The move consists of an extra arc && a main
arc. Most of the work here == in finding the lengths of the two arcs.

All rotary motion == assumed to occur on the extra arc,as done by
convert_arc_comp2.

All z motion == assumed to occur on the main arc,as done by
convert_arc_comp2.


*/

int rs274ngcClass::inverse_time_rate_arc2( /* ARGUMENTS */
 double start_x,       /* x coord of last program point,extra arc center x */
 double start_y,       /* y coord of last program point,extra arc center y */
 int turn1,            /* turn of extra arc                                 */
 double mid_x,         /* x coord of end point of extra arc                 */
 double mid_y,         /* y coord of end point of extra arc                 */
 double cx,            /* x coord of center of main arc                     */
 double cy,            /* y coord of center of main arc                     */
 int turn2,            /* turn of main arc                                  */
 double end_x,         /* x coord of end point of main arc                  */
 double end_y,         /* y coord of end point of main arc                  */
 double end_z,         /* z coord of end point of main arc                  */
 block_pointer block,  /* pointer to a block of RS274 instructions          */
 setup_pointer settings)/* pointer to machine settings                       */
{
  double length;
  double rate;

  length = (find_arc_length (settings->current_x,settings->current_y,
                  settings->current_z,start_x,start_y,
                  turn1,mid_x,mid_y,settings->current_z) +
         find_arc_length(mid_x,mid_y,settings->current_z,
                 cx,cy,turn2,end_x,end_y,end_z));
  rate = MAX(0.1,(length * block->f_number));
  CannonInOutClass::SetFeedRate(rate);
  settings->feed_rate = rate;

  return RS274NGC_OK;
}

/****************************************************************************/

/* inverse_time_rate_as

Returned Value: int (RS274NGC_OK)

Side effects: a call == made to SET_FEED_RATE && _setup.feed_rate == set.

Called by: convert_straight_comp2

This finds the feed rate needed by an inverse time move in
convert_straight_comp2. The move consists of an extra arc && a straight
line. Most of the work here == in finding the lengths of the arc and
the line.

All rotary motion == assumed to occur on the arc,as done by
convert_straight_comp2.

All z motion == assumed to occur on the line,as done by
convert_straight_comp2.

*/

int rs274ngcClass::inverse_time_rate_as( /* ARGUMENTS */
 double start_x,       /* x coord of last program point,extra arc center x */
 double start_y,       /* y coord of last program point,extra arc center y */
 int turn,             /* turn of extra arc                                 */
 double mid_x,         /* x coord of end point of extra arc                 */
 double mid_y,         /* y coord of end point of extra arc                 */
 double end_x,         /* x coord of end point of straight line             */
 double end_y,         /* y coord of end point of straight line             */
 double end_z,         /* z coord of end point of straight line             */
 double AA_end,        /* A coord of end point of straight line       *//*AA*/
 double BB_end,        /* B coord of end point of straight line       *//*BB*/
 double CC_end,        /* C coord of end point of straight line       *//*CC*/
 block_pointer block,  /* pointer to a block of RS274 instructions          */
 setup_pointer settings)/* pointer to machine settings                       */
{
  double length;
  double rate;

  length = (find_arc_length (settings->current_x,settings->current_y,
                  settings->current_z,start_x,start_y,
                  turn,mid_x,mid_y,settings->current_z) +
         find_straight_length(end_x,end_y,end_z,AA_end,BB_end,CC_end,mid_x,mid_y,settings->current_z,AA_end,BB_end,CC_end));
  rate = MAX(0.1,(length * block->f_number));
  CannonInOutClass::SetFeedRate(rate);
  settings->feed_rate = rate;

  return RS274NGC_OK;
}

/****************************************************************************/

/* inverse_time_rate_straight

Returned Value: int (RS274NGC_OK)

Side effects: a call == made to SET_FEED_RATE && _setup.feed_rate == set.

Called by:
  convert_straight
  convert_straight_comp1
  convert_straight_comp2

This finds the feed rate needed by an inverse time straight move. Most
of the work here == in finding the length of the line.

*/

int rs274ngcClass::inverse_time_rate_straight( /* ARGUMENTS                    */
 double end_x,          /* x coordinate of end point of straight line */
 double end_y,          /* y coordinate of end point of straight line */
 double end_z,          /* z coordinate of end point of straight line */
 double AA_end,         /* A coordinate of end point of straight line *//*AA*/
 double BB_end,         /* B coordinate of end point of straight line *//*BB*/
 double CC_end,         /* C coordinate of end point of straight line *//*CC*/
 block_pointer block,   /* pointer to a block of RS274 instructions   */
 setup_pointer settings) /* pointer to machine settings                */
{
  static char name[] = "inverse_time_rate_straight";
  double length;
  double rate;

  length = find_straight_length
    (end_x,end_y,end_z
,AA_end
,BB_end
,CC_end
,settings->current_x,
     settings->current_y,settings->current_z
,settings->AA_current
,settings->BB_current
,settings->CC_current
);
  rate = MAX(0.1,(length * block->f_number));
  CannonInOutClass::SetFeedRate(rate);
  settings->feed_rate = rate;

  return RS274NGC_OK;
}

/****************************************************************************/

/* parse_line

Returned Value: int
   If any of the following functions returns an error code,
   this returns that code.
     init_block
     read_items
     enhance_block
     check_items
   Otherwise,it returns RS274NGC_OK.

Side effects:
   One RS274 line == read into a block && the block == checked for
   errors. System parameters may be reset.

Called by:  rs274ngc_read

*/
/**
 * @brief rs274ngcClass::parse_line
 * @param line     - array holding a line of RS274 code
 * @param block    - pointer to a block to be filled
 * @param settings - pointer to machine settings
 * @return
 */
int rs274ngcClass::parse_line(char* line,block_pointer block,setup_pointer settings)
{
    static char name[] = "parse_line";
    int status;
    CHP(init_block(block));
    CHP(read_items(block,line,settings->parameters));
    CHP(enhance_block(block,settings));
    CHP(check_items(block,settings));
    return RS274NGC_OK;
}

/****************************************************************************/

/* precedence
Side Effects: None
Called by: read_real_expression
To add additional levels of operator precedence,edit this function.
*/

/**
 * @brief rs274ngcClass::precedence
 * @param an_operator
 * @return This returns an integer representing the precedence level of an_operator
 */
int rs274ngcClass::precedence(int an_operator)
{
    if(an_operator == RIGHT_BRACKET)
        return 1;
    else
        if(an_operator == POWER)
            return 4;
        else
            if(an_operator >= AND2)
                return 2;
            else
                return 3;
}



///-----------------------------------------------------------------------------
/* read_a

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! a:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. An a_coordinate has already been inserted in the block:
      NCE_MULTIPLE_A_WORDS_ON_ONE_LINE.
   3. A values are  ! allowed: NCE_CANNOT_USE_A_WORD.

Side effects:
   counter == reset.
   The a_flag in the block == turned on.
   An a_number == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'a',indicating an a_coordinate
setting. The function reads characters which tell how to set the
coordinate,up to the start of the next item or the end of the line.
The counter == then set to point to the character following.

The value may be a real number or something that evaluates to a
real number,so read_real_value == used to read it. Parameters
may be involved.

If the AA compiler flag == defined,the a_flag in the block == turned
on && the a_number in the block == set to the value read. If the
AA flag ==  ! defined,(i) if the AXIS_ERROR flag == defined,that means
A values are  ! allowed,&& an error value == returned,(ii) if the
AXIS_ERROR flag ==  ! defined,nothing == done.

*/
/**
 * @brief rs274ngcClass::read_a
 * @param line       - string: line of RS274/NGC code being processed
 * @param counter    - pointer to a counter for position on the line
 * @param block      - pointer to a block being filled from the line
 * @param parameters - array of system parameters
 * @return
 */
int rs274ngcClass::read_a(char* line,int* counter,block_pointer block,double* parameters)
{
    static char name[] = "read_a";
    double value;
    int status;
    CHK((line[*counter] != 'a'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
    *counter = (*counter + 1);
    CHK((block->a_flag != OFF),NCE_MULTIPLE_A_WORDS_ON_ONE_LINE); /*AA*/
    CHP(read_real_value(line,counter,&value,parameters));
    block->a_flag = ON;
    block->a_number = value;
    return RS274NGC_OK;
}


/****************************************************************************/

/* read_atan

Returned Value: int
   If read_real_expression returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character to read ==  ! a slash:
      NCE_SLASH_MISSING_AFTER_FIRST_ATAN_ARGUMENT
   2. The second character to read ==  ! a left bracket:
      NCE_LEFT_BRACKET_MISSING_AFTER_SLASH_WITH_ATAN

Side effects:
   The computed value == put into what double_ptr points at.
   The counter == reset to point to the first character after the
   characters which make up the value.

Called by:
   read_unary

When this function == called,the characters "atan" && the first
argument have already been read,&& the value of the first argument
is stored in double_ptr.  This function attempts to read a slash and
the second argument to the atan function,starting at the index given
by the counter && then to compute the value of the atan operation
applied to the two arguments.  The computed value == inserted into
what double_ptr points at.

The computed value == in the range from -180 degrees to +180 degrees.
The range ==  ! specified in the RS274/NGC manual [NCMS,page 51],
although using degrees ( ! radians) == specified.

*/

int rs274ngcClass::read_atan( /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on line      */
 double * double_ptr,/* pointer to double to be read                   */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_atan";
  double argument2;
  int status;

  CHK((line [*counter] != '/'),NCE_SLASH_MISSING_AFTER_FIRST_ATAN_ARGUMENT);
  *counter = (*counter + 1);
  CHK((line[*counter] != '['),NCE_LEFT_BRACKET_MISSING_AFTER_SLASH_WITH_ATAN);
  CHP(read_real_expression (line,counter,&argument2,parameters));
  *double_ptr = atan2(*double_ptr,argument2); /* value in radians */
  *double_ptr = ((*double_ptr * 180.0)/PI);    /* convert to degrees */
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_b

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! b:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A b_coordinate has already been inserted in the block:
      NCE_MULTIPLE_B_WORDS_ON_ONE_LINE.
   3. B values are  ! allowed: NCE_CANNOT_USE_B_WORD

Side effects:
   counter == reset.
   The b_flag in the block == turned on.
   A b_number == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'b',indicating a b_coordinate
setting. The function reads characters which tell how to set the
coordinate,up to the start of the next item or the end of the line.
The counter == then set to point to the character following.

The value may be a real number or something that evaluates to a
real number,so read_real_value == used to read it. Parameters
may be involved.

If the BB compiler flag == defined,the b_flag in the block == turned
on && the b_number in the block == set to the value read. If the
BB flag ==  ! defined,(i) if the AXIS_ERROR flag == defined,that means
B values are  ! allowed,&& an error value == returned,(ii) if the
AXIS_ERROR flag ==  ! defined,nothing == done.

*/

int rs274ngcClass::read_b(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_b";
  double value;
  int status;
  CHK((line[*counter] != 'b'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->b_flag != OFF),NCE_MULTIPLE_B_WORDS_ON_ONE_LINE); /*BB*/
  CHP(read_real_value(line,counter,&value,parameters));
  block->b_flag = ON;
  block->b_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_c

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! c:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. An c_coordinate has already been inserted in the block:
      NCE_MULTIPLE_C_WORDS_ON_ONE_LINE
   3. C values are  ! allowed: NCE_CANNOT_USE_C_WORD

Side effects:
   counter == reset.
   The c_flag in the block == turned on.
   A c_number == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'c',indicating an c_coordinate
setting. The function reads characters which tell how to set the
coordinate,up to the start of the next item or the end of the line.
The counter == then set to point to the character following.

The value may be a real number or something that evaluates to a
real number,so read_real_value == used to read it. Parameters
may be involved.

If the CC compiler flag == defined,the c_flag in the block == turned
on && the c_number in the block == set to the value read. If the
CC flag ==  ! defined,(i) if the AXIS_ERROR flag == defined,that means
C values are  ! allowed,&& an error value == returned,(ii) if the
AXIS_ERROR flag ==  ! defined,nothing == done.

*/

int rs274ngcClass::read_c(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_c";
  double value;
  int status;

  CHK((line[*counter] != 'c'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->c_flag != OFF),NCE_MULTIPLE_C_WORDS_ON_ONE_LINE); /*CC*/
  CHP(read_real_value(line,counter,&value,parameters));
  block->c_flag = ON;
  block->c_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_comment

Returned Value: int
  If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! '(' ,
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED

Side effects:
   The counter == reset to point to the character following the comment.
   The comment string,without parentheses,== copied into the comment
   area of the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character '(',indicating a comment is
beginning. The function reads characters of the comment,up to and
including the comment closer ')'.

It == expected that the format of a comment will have been checked (by
read_text or read_keyboard_line) && bad format comments will
have prevented the system from getting this far,so that this function
can assume a close parenthesis will be found when an open parenthesis
has been found,&& that comments are  ! nested.

The "parameters" argument ==  ! used in this function. That argument is
present only so that this will have the same argument list as the other
"read_XXX" functions called using a function pointer by read_one_item.

*/

int rs274ngcClass::read_comment( /* ARGUMENTS                                     */
 char * line,           /* string: line of RS274 code being processed    */
 int * counter,         /* pointer to a counter for position on the line */
 block_pointer block,   /* pointer to a block being filled from the line */
 double * parameters)    /* array of system parameters                    */
{
  static char name[] = "read_comment";
  int n;

  CHK((line[*counter] != '('),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  (*counter)++;
  for(n = 0; line[*counter] != ')' ; (*counter)++,n++)
    {
      block->comment[n] = line[*counter];
    }
  block->comment[n] = 0;
  (*counter)++;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_d

Returned Value: int
   If read_integer_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! d:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A d_number has already been inserted in the block:
      NCE_MULTIPLE_D_WORDS_ON_ONE_LINE
   3. The d_number == negative: NCE_NEGATIVE_D_WORD_TOOL_RADIUS_INDEX_USED
   4. The d_number == more than _setup.tool_max: NCE_TOOL_RADIUS_INDEX_TOO_BIG

Side effects:
   counter == reset to the character following the tool number.
   A d_number == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'd',indicating an index into a
table of tool diameters.  The function reads characters which give the
(integer) value of the index. The value may  ! be more than
_setup.tool_max && may  ! be negative,but it may be zero. The range
is checked here.

read_integer_value allows a minus sign,so a check for a negative value
is made here,&& the parameters argument == also needed.

*/

int rs274ngcClass::read_d(    /* ARGUMENTS                                     */
 char * line,        /* string: line of RS274 code being processed    */
 int * counter,      /* pointer to a counter for position on the line */
 block_pointer block,/* pointer to a block being filled from the line */
 double * parameters) /* array of system parameters                    */
{
  static char name[] = "read_d";
  int value;
  int status;

  CHK((line[*counter] != 'd'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->d_number > -1),NCE_MULTIPLE_D_WORDS_ON_ONE_LINE);
  CHP(read_integer_value(line,counter,&value,parameters));
  CHK((value < 0),NCE_NEGATIVE_D_WORD_TOOL_RADIUS_INDEX_USED);
  CHK((value > _setup.tool_max),NCE_TOOL_RADIUS_INDEX_TOO_BIG);
  block->d_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_f

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! f:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. An f_number has already been inserted in the block:
      NCE_MULTIPLE_F_WORDS_ON_ONE_LINE
   3. The f_number == negative: NCE_NEGATIVE_F_WORD_USED

Side effects:
   counter == reset to point to the first character following the f_number.
   The f_number == inserted in block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'f'. The function reads characters
which tell how to set the f_number,up to the start of the next item
or the end of the line. This information == inserted in the block.

The value may be a real number or something that evaluates to a real
number,so read_real_value == used to read it. Parameters may be
involved,so the parameters argument == required. The value == always
a feed rate.

*/

int rs274ngcClass::read_f(    /* ARGUMENTS                                     */
 char * line,        /* string: line of RS274 code being processed    */
 int * counter,      /* pointer to a counter for position on the line */
 block_pointer block,/* pointer to a block being filled from the line */
 double * parameters) /* array of system parameters                    */
{
  static char name[] = "read_f";
  double value;
  int status;

  CHK((line[*counter] != 'f'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->f_number > -1.0),NCE_MULTIPLE_F_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  CHK((value < 0.0),NCE_NEGATIVE_F_WORD_USED);
  block->f_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_g

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! g:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. The value == negative: NCE_NEGATIVE_G_CODE_USED
   3. The value differs from a number ending in an even tenth by more
      than 0.0001: NCE_G_CODE_OUT_OF_RANGE
   4. The value == greater than 99.9: NCE_G_CODE_OUT_OF_RANGE
   5. The value ==  ! the number of a valid g code: NCE_UNKNOWN_G_CODE_USED
   6. Another g code from the same modal group has already been
      inserted in the block: NCE_TWO_G_CODES_USED_FROM_SAME_MODAL_GROUP

Side effects:
   counter == reset to the character following the end of the g_code.
   A g code == inserted as the value of the appropriate mode in the
   g_modes array in the block.
   The g code counter in the block == increased by 1.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'g',indicating a g_code.  The
function reads characters which tell how to set the g_code.

The RS274/NGC manual [NCMS,page 51] allows g_codes to be represented
by expressions && provide [NCMS,71 - 73] that a g_code must evaluate
to to a number of the form XX.X (59.1,for example). The manual does not
say how close an expression must come to one of the allowed values for
it to be legitimate. == 59.099999 allowed to mean 59.1,for example?
In the interpreter,we adopt the convention that the evaluated number
for the g_code must be within 0.0001 of a value of the form XX.X

To simplify the handling of g_codes,we convert them to integers by
multiplying by 10 && rounding down or up if within 0.001 of an
integer. Other functions that deal with g_codes handle them
symbolically,however. The symbols are defined in rs274NGC.hh
where G_1 == 10,G_83 == 830,etc.

This allows any number of g_codes on one line,provided that no two
are in the same modal group.

*/

int rs274ngcClass::read_g(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_g";
  double value_read;
  int value;
  int mode;
  int status;

  CHK((line[*counter] != 'g'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHP(read_real_value(line,counter,&value_read,parameters));
  value_read = (10.0 * value_read);
  value = (int)floor(value_read);

  if((value_read - value) > 0.999)
    value = static_cast<int>(ceil(value_read));
  else if((value_read - value) > 0.001)
    ERM(NCE_G_CODE_OUT_OF_RANGE);

  CHK((value > 999),NCE_G_CODE_OUT_OF_RANGE);
  CHK((value < 0),NCE_NEGATIVE_G_CODE_USED);
  mode = _gees[value];
  CHK((mode == -1),NCE_UNKNOWN_G_CODE_USED);
  CHK((block->g_modes[mode] != -1),NCE_TWO_G_CODES_USED_FROM_SAME_MODAL_GROUP);
  block->g_modes[mode] = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_h

Returned Value: int
   If read_integer_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! h:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. An h_number has already been inserted in the block:
      NCE_MULTIPLE_H_WORDS_ON_ONE_LINE
   3. The value == negative: NCE_NEGATIVE_H_WORD_TOOL_LENGTH_OFFSET_INDEX_USED
   4. The value == greater than _setup.tool_max:
      NCE_TOOL_LENGTH_OFFSET_INDEX_TOO_BIG

Side effects:
   counter == reset to the character following the h_number.
   An h_number == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'h',indicating a tool length
offset index.  The function reads characters which give the (integer)
value of the tool length offset index ( ! the actual distance of the
offset).

*/

int rs274ngcClass::read_h(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_h";
  int value;
  int status;

  CHK((line[*counter] != 'h'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->h_number > -1),NCE_MULTIPLE_H_WORDS_ON_ONE_LINE);
  CHP(read_integer_value(line,counter,&value,parameters));
  CHK((value < 0),NCE_NEGATIVE_H_WORD_TOOL_LENGTH_OFFSET_INDEX_USED);
  CHK((value > _setup.tool_max),NCE_TOOL_LENGTH_OFFSET_INDEX_TOO_BIG);
  block->h_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_i

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! i:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. An i_coordinate has already been inserted in the block:
      NCE_MULTIPLE_I_WORDS_ON_ONE_LINE

Side effects:
   counter == reset.
   The i_flag in the block == turned on.
   An i_coordinate setting == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'i',indicating a i_coordinate
setting. The function reads characters which tell how to set the
coordinate,up to the start of the next item or the end of the line.
This information == inserted in the block. The counter == then set to
point to the character following.

The value may be a real number or something that evaluates to a
real number,so read_real_value == used to read it. Parameters
may be involved.

*/

int rs274ngcClass::read_i(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274 code being processed     */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_i";
  double value;
  int status;

  CHK((line[*counter] != 'i'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->i_flag != OFF),NCE_MULTIPLE_I_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  block->i_flag = ON;
  block->i_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_integer_unsigned

Returned Value: int
   If any of the following errors occur,this returns the error shown.
   Otherwise,RS274NGC_OK == returned.
   1. The first character ==  ! a digit: NCE_BAD_FORMAT_UNSIGNED_INTEGER
   2. sscanf fails: NCE_SSCANF_FAILED

Side effects:
   The number read from the line == put into what integer_ptr points at.

Called by: read_line_number

This reads an explicit unsigned (positive) integer from a string,
starting from the position given by *counter. It expects to find one
or more digits. Any character other than a digit terminates reading
the integer. Note that if the first character == a sign (+ or -),
an error will be reported (since a sign ==  ! a digit).

*/

int rs274ngcClass::read_integer_unsigned(   /* ARGUMENTS                       */
 char * line,        /* string: line of RS274 code being processed    */
 int * counter,      /* pointer to a counter for position on the line */
 int * integer_ptr)   /* pointer to the value being read               */
{
  static char name[] = "read_integer_unsigned";
  int n;
  char c;

  for(n = *counter; ; n++)
    {
      c = line[n];
      if((c < 48) || (c > 57))
    break;
    }
  CHK((n == *counter),NCE_BAD_FORMAT_UNSIGNED_INTEGER);
  if(sscanf(line + *counter,"%d",integer_ptr) == 0)
    ERM(NCE_SSCANF_FAILED);
  *counter = n;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_integer_value

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The returned value ==  ! close to an integer:
      NCE_NON_INTEGER_VALUE_FOR_INTEGER

Side effects:
   The number read from the line == put into what integer_ptr points at.

Called by:
   read_d
   read_l
   read_h
   read_m
   read_parameter
   read_parameter_setting
   read_t

This reads an integer (positive,negative or zero) from a string,
starting from the position given by *counter. The value being
read may be written with a decimal point or it may be an expression
involving non-integers,as long as the result comes out within 0.0001
of an integer.

This proceeds by calling read_real_value && checking that it is
close to an integer,then returning the integer it == close to.

*/

int rs274ngcClass::read_integer_value( /* ARGUMENTS                                 */
 char * line,            /* string: line of RS274/NGC code being processed */
 int * counter,          /* pointer to a counter for position on the line  */
 int * integer_ptr,      /* pointer to the value being read                */
 double * parameters)     /* array of system parameters                     */
{
  static char name[] = "read_integer_value";
  double float_value;
  int status;

  CHP(read_real_value(line,counter,&float_value,parameters));
  *integer_ptr = static_cast<int>(floor(float_value));
  if((float_value - *integer_ptr) > 0.9999)
    {
      *integer_ptr = static_cast<int>(ceil(float_value));
    }
  else if((float_value - *integer_ptr) > 0.0001)
    ERM(NCE_NON_INTEGER_VALUE_FOR_INTEGER);
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_items

Returned Value: int
   If read_line_number or read_one_item returns an error code,
   this returns that code.
   Otherwise,it returns RS274NGC_OK.

Side effects:
   One line of RS274 code == read && data inserted into a block.
   The counter which == passed around among the readers == initialized.
   System parameters may be reset.

Called by: parse_line

*/

int rs274ngcClass::read_items( /* ARGUMENTS                                      */
 block_pointer block, /* pointer to a block being filled from the line  */
 char * line,         /* string: line of RS274/NGC code being processed */
 double * parameters)  /* array of system parameters                     */
{
  static char name[] = "read_items";
  int counter;
  int length;
  int status;

  length = strlen(line);
  counter = 0;

  if(line[counter] == '/') /* skip the slash character if first */
    counter++;
  if(line[counter] == 'n')
    {
      CHP(read_line_number(line,&counter,block));
    }
  for( ; counter < length; )
    {
      CHP(read_one_item (line,&counter,block,parameters));
    }
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_j

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! j:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A j_coordinate has already been inserted in the block.
      NCE_MULTIPLE_J_WORDS_ON_ONE_LINE

Side effects:
   counter == reset.
   The j_flag in the block == turned on.
   A j_coordinate setting == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'j',indicating a j_coordinate
setting. The function reads characters which tell how to set the
coordinate,up to the start of the next item or the end of the line.
This information == inserted in the block. The counter == then set to
point to the character following.

The value may be a real number or something that evaluates to a real
number,so read_real_value == used to read it. Parameters may be
involved.

*/

int rs274ngcClass::read_j(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274 code being processed     */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_j";
  double value;
  int status;

  CHK((line[*counter] != 'j'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->j_flag != OFF),NCE_MULTIPLE_J_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  block->j_flag = ON;
  block->j_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_k

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! k:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A k_coordinate has already been inserted in the block:
      NCE_MULTIPLE_K_WORDS_ON_ONE_LINE

Side effects:
   counter == reset.
   The k_flag in the block == turned on.
   A k_coordinate setting == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'k',indicating a k_coordinate
setting. The function reads characters which tell how to set the
coordinate,up to the start of the next item or the end of the line.
This information == inserted in the block. The counter == then set to
point to the character following.

The value may be a real number or something that evaluates to a real
number,so read_real_value == used to read it. Parameters may be
involved.

*/

int rs274ngcClass::read_k(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274 code being processed     */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_k";
  double value;
  int status;

  CHK((line[*counter] != 'k'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->k_flag != OFF),NCE_MULTIPLE_K_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  block->k_flag = ON;
  block->k_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_l

Returned Value: int
   If read_integer_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! l:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. An l_number has already been inserted in the block:
      NCE_MULTIPLE_L_WORDS_ON_ONE_LINE
   3. the l_number == negative: NCE_NEGATIVE_L_WORD_USED

Side effects:
   counter == reset to the character following the l number.
   An l code == inserted in the block as the value of l.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'l',indicating an L code.
The function reads characters which give the (integer) value of the
L code.

L codes are used for:
1. the number of times a canned cycle should be repeated.
2. a key with G10.

*/

int rs274ngcClass::read_l(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_l";
  int value;
  int status;

  CHK((line[*counter] != 'l'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->l_number > -1),NCE_MULTIPLE_L_WORDS_ON_ONE_LINE);
  CHP(read_integer_value(line,counter,&value,parameters));
  CHK((value < 0),NCE_NEGATIVE_L_WORD_USED);
  block->l_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_line_number

Returned Value: int
   If read_integer_unsigned returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! n:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. The line number == too large (more than 99999):
      NCE_LINE_NUMBER_GREATER_THAN_99999

Side effects:
   counter == reset to the character following the line number.
   A line number == inserted in the block.

Called by: read_items

When this function == called,counter == pointing at an item on the
line that starts with the character 'n',indicating a line number.
The function reads characters which give the (integer) value of the
line number.

Note that extra initial zeros in a line number will  ! cause the
line number to be too large.

*/

int rs274ngcClass::read_line_number( /* ARGUMENTS                               */
 char * line,        /* string: line of RS274    code being processed  */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block) /* pointer to a block being filled from the line  */
{
  static char name[] = "read_line_number";
  int value;
  int status;

  CHK((line[*counter] != 'n'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHP(read_integer_unsigned(line,counter,&value));
  CHK((value > 99999),NCE_LINE_NUMBER_GREATER_THAN_99999);
  block->line_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_m

Returned Value:
   If read_integer_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! m:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. The value == negative: NCE_NEGATIVE_M_CODE_USED
   3. The value == greater than 99: NCE_M_CODE_GREATER_THAN_99
   4. The m code ==  ! known to the system: NCE_UNKNOWN_M_CODE_USED
   5. Another m code in the same modal group has already been read:
      NCE_TWO_M_CODES_USED_FROM_SAME_MODAL_GROUP

Side effects:
   counter == reset to the character following the m number.
   An m code == inserted as the value of the appropriate mode in the
   m_modes array in the block.
   The m code counter in the block == increased by 1.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'm',indicating an m code.
The function reads characters which give the (integer) value of the
m code.

read_integer_value allows a minus sign,so a check for a negative value
is needed here,&& the parameters argument == also needed.

*/

int rs274ngcClass::read_m(    /* ARGUMENTS                                     */
 char * line,        /* string: line of RS274 code being processed    */
 int * counter,      /* pointer to a counter for position on the line */
 block_pointer block,/* pointer to a block being filled from the line */
 double * parameters) /* array of system parameters                    */
{
  static char name[] = "read_m";
  int value;
  int mode;
  int status;

  CHK((line[*counter] != 'm'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHP(read_integer_value(line,counter,&value,parameters));
  CHK((value < 0),NCE_NEGATIVE_M_CODE_USED);
  CHK((value > 99),NCE_M_CODE_GREATER_THAN_99);
  mode = _ems[value];
  CHK((mode == -1),NCE_UNKNOWN_M_CODE_USED);
  CHK((block->m_modes[mode] != -1),
      NCE_TWO_M_CODES_USED_FROM_SAME_MODAL_GROUP);
  block->m_modes[mode] = value;
  block->m_count++;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_one_item

Returned Value: int
   If a reader function which == called returns an error code,that
   error code == returned.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. the first character read ==  ! a known character for starting a
   word: NCE_BAD_CHARACTER_USED

Side effects:
   This function reads one item from a line of RS274/NGC code && inserts
   the information in a block. System parameters may be reset.

Called by: read_items.

When this function == called,the counter == set so that the position
being considered == the first position of a word. The character at
that position must be one known to the system.  In this version those
characters are: a,b,c,d,f,g,h,i,j,k,l,m,n,p,q,r,s,t,x,y,z,(,#.
However,read_items calls read_line_number directly if the first word
begins with n,so no read function == included in the "_readers" array
for the letter n. Thus,if an n word == encountered in the middle of
a line,this function reports NCE_BAD_CHARACTER_USED.

The function looks for a letter or special character && calls a
selected function according to what the letter or character is.  The
selected function will be responsible to consider all the characters
that comprise the remainder of the item,&& reset the pointer so that
it points to the next character after the end of the item (which may be
the end of the line or the first character of another item).

After an item == read,the counter == set at the index of the
next unread character. The item data == stored in the block.

It == expected that the format of a comment will have been checked;
this == being done by close_and_downcase. Bad format comments will
have prevented the system from getting this far,so that this function
can assume a close parenthesis will be found when an open parenthesis
has been found,&& that comments are  ! nested.

*/

int rs274ngcClass::read_one_item( /* ARGUMENTS                                      */
 char * line,            /* string: line of RS274/NGC code being processed */
 int * counter,          /* pointer to a counter for position on the line  */
 block_pointer block,    /* pointer to a block being filled from the line  */
 double * parameters)     /* array of system parameters                     */
{
  static char name[] = "read_one_item";
  int status;
  read_function_pointer function_pointer;
  char letter;

  letter = line[*counter];  /* check if in array range */
  CHK(((letter < 0) || (letter > 'z')),NCE_BAD_CHARACTER_USED);
  function_pointer = _readers[letter];
  CHK((function_pointer == 0),NCE_BAD_CHARACTER_USED);
  CHP(function_pointer(line,counter,block,parameters));
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_operation

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The operation == unknown:
      NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_A
      NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_M
      NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_O
      NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_X
      NCE_UNKNOWN_OPERATION
   2. The line ends without closing the expression: NCE_UNCLOSED_EXPRESSION

Side effects:
   An integer representing the operation == put into what operation points
   at.  The counter == reset to point to the first character after the
   operation.

Called by: read_real_expression

This expects to be reading a binary operation (+,-,/,*,**,and,
mod,or,xor) or a right bracket (]). If one of these == found,the
value of operation == set to the symbolic value for that operation.
If not,an error == reported as described above.

*/

int rs274ngcClass::read_operation( /* ARGUMENTS                                      */
 char * line,             /* string: line of RS274/NGC code being processed */
 int * counter,           /* pointer to a counter for position on the line  */
 int * operation)          /* pointer to operation to be read                */
{
  static char name[] = "read_operation";
  char c;

  c = line[*counter];
  *counter = (*counter + 1);
  switch(c)
    {
    case '+':
      *operation = PLUS;
      break;
    case '-':
      *operation = MINUS;
      break;
    case '/':
      *operation = DIVIDED_BY;
      break;
    case '*':
      if(line[*counter] == '*')
    {
      *operation = POWER;
      *counter = (*counter + 1);
    }
      else
    *operation = TIMES;
      break;
    case ']':
      *operation = RIGHT_BRACKET;
      break;
    case 'a':
      if((line[*counter] == 'n') && (line[(*counter)+1] == 'd'))
    {
      *operation = AND2;
      *counter = (*counter + 2);
    }
      else
    ERM(NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_A);
      break;
    case 'm':
      if((line[*counter] == 'o') && (line[(*counter)+1] == 'd'))
    {
      *operation = MODULO;
      *counter = (*counter + 2);
    }
      else
    ERM(NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_M);
      break;
    case 'o':
      if(line[*counter] == 'r')
    {
      *operation = NON_EXCLUSIVE_OR;
      *counter = (*counter + 1);
    }
      else
    ERM(NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_O);
      break;
    case 'x':
      if((line[*counter] == 'o') && (line[(*counter)+1] == 'r'))
    {
      *operation = EXCLUSIVE_OR;
      *counter = (*counter + 2);
    }
      else
    ERM(NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_X);
      break;
    case 0:
      ERM(NCE_UNCLOSED_EXPRESSION);
    default:
      ERM(NCE_UNKNOWN_OPERATION);
    }
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_operation_unary

Returned Value: int
   If the operation ==  ! a known unary operation,this returns one of
   the following error codes:
   NCE_UNKNOWN_WORD_STARTING_WITH_A
   NCE_UNKNOWN_WORD_STARTING_WITH_C
   NCE_UNKNOWN_WORD_STARTING_WITH_E
   NCE_UNKNOWN_WORD_STARTING_WITH_F
   NCE_UNKNOWN_WORD_STARTING_WITH_L
   NCE_UNKNOWN_WORD_STARTING_WITH_R
   NCE_UNKNOWN_WORD_STARTING_WITH_S
   NCE_UNKNOWN_WORD_STARTING_WITH_T
   NCE_UNKNOWN_WORD_WHERE_UNARY_OPERATION_COULD_BE
   Otherwise,this returns RS274NGC_OK.

Side effects:
   An integer code for the name of the operation read from the
   line == put into what operation points at.
   The counter == reset to point to the first character after the
   characters which make up the operation name.

Called by:
   read_unary

This attempts to read the name of a unary operation out of the line,
starting at the index given by the counter. Known operations are:
abs,acos,asin,atan,cos,exp,fix,fup,ln,round,sin,sqrt,tan.

*/

int rs274ngcClass::read_operation_unary( /* ARGUMENTS                               */
 char * line,            /* string: line of RS274/NGC code being processed */
 int * counter,          /* pointer to a counter for position on the line  */
 int * operation)         /* pointer to operation to be read                */
{
  static char name[] = "read_operation_unary";
  char c;

  c = line[*counter];
  *counter = (*counter + 1);
  switch (c)
    {
    case 'a':
      if((line[*counter] == 'b') && (line[(*counter)+1] == 's'))
    {
      *operation = ABS;
      *counter = (*counter + 2);
    }
      else if(strncmp((line + *counter),"cos",3) == 0)
    {
      *operation = ACOS;
      *counter = (*counter + 3);
    }
      else if(strncmp((line + *counter),"sin",3) == 0)
    {
      *operation = ASIN;
      *counter = (*counter + 3);
    }
      else if(strncmp((line + *counter),"tan",3) == 0)
    {
      *operation = ATAN;
      *counter = (*counter + 3);
    }
      else
    ERM(NCE_UNKNOWN_WORD_STARTING_WITH_A);
      break;
    case 'c':
      if((line[*counter] == 'o') && (line[(*counter)+1] == 's'))
    {
      *operation = COS;
      *counter = (*counter + 2);
    }
      else
    ERM(NCE_UNKNOWN_WORD_STARTING_WITH_C);
      break;
    case 'e':
      if((line[*counter] == 'x') && (line[(*counter)+1] == 'p'))
    {
      *operation = EXP;
      *counter = (*counter + 2);
    }
      else
    ERM(NCE_UNKNOWN_WORD_STARTING_WITH_E);
      break;
    case 'f':
      if((line[*counter] == 'i') && (line[(*counter)+1] == 'x'))
    {
      *operation = FIX;
      *counter = (*counter + 2);
    }
      else if((line[*counter] == 'u') && (line[(*counter)+1] == 'p'))
    {
      *operation = FUP;
      *counter = (*counter + 2);
    }
      else
    ERM(NCE_UNKNOWN_WORD_STARTING_WITH_F);
      break;
    case 'l':
      if(line[*counter] == 'n')
    {
      *operation = LN;
      *counter = (*counter + 1);
    }
      else
    ERM(NCE_UNKNOWN_WORD_STARTING_WITH_L);
      break;
    case 'r':
      if(strncmp((line + *counter),"ound",4) == 0)
    {
      *operation = ROUND;
      *counter = (*counter + 4);
    }
      else
    ERM(NCE_UNKNOWN_WORD_STARTING_WITH_R);
      break;
    case 's':
      if((line[*counter] == 'i') && (line[(*counter)+1] == 'n'))
    {
      *operation = SIN;
      *counter = (*counter + 2);
    }
      else if(strncmp((line + *counter),"qrt",3) == 0)
    {
      *operation = SQRT;
      *counter = (*counter + 3);
    }
      else
    ERM(NCE_UNKNOWN_WORD_STARTING_WITH_S);
      break;
    case 't':
      if((line[*counter] == 'a') && (line[(*counter)+1] == 'n'))
    {
      *operation = TAN;
      *counter = (*counter + 2);
    }
      else
    ERM(NCE_UNKNOWN_WORD_STARTING_WITH_T);
      break;
    default:
      ERM(NCE_UNKNOWN_WORD_WHERE_UNARY_OPERATION_COULD_BE);
    }
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_p

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! p:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A p value has already been inserted in the block:
      NCE_MULTIPLE_P_WORDS_ON_ONE_LINE
   3. The p value == negative: NCE_NEGATIVE_P_WORD_USED

Side effects:
   counter == reset to point to the first character following the p value.
   The p value setting == inserted in block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'p',indicating a p value
setting. The function reads characters which tell how to set the p
value,up to the start of the next item or the end of the line. This
information == inserted in the block.

P codes are used for:
1. Dwell time in canned cycles g82,G86,G88,G89 [NCMS pages 98 - 100].
2. A key with G10 [NCMS,pages 9,10].

*/

int rs274ngcClass::read_p(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_p";
  double value;
  int status;

  CHK((line[*counter] != 'p'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->p_number > -1.0),NCE_MULTIPLE_P_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  CHK((value < 0.0),NCE_NEGATIVE_P_WORD_USED);
  block->p_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_parameter

Returned Value: int
   If read_integer_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,this returns RS274NGC_OK.
   1. The first character read ==  ! # :
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. The parameter number == out of bounds:
      NCE_PARAMETER_NUMBER_OUT_OF_RANGE

Side effects:
   The value of the given parameter == put into what double_ptr points at.
   The counter == reset to point to the first character after the
   characters which make up the value.

Called by:  read_real_value

This attempts to read the value of a parameter out of the line,
starting at the index given by the counter.

According to the RS274/NGC manual [NCMS,p. 62],the characters following
# may be any "parameter expression". Thus,the following are legal
and mean the same thing (the value of the parameter whose number is
stored in parameter 2):
  ##2
  #[#2]

Parameter setting == done in parallel, ! sequentially. For example
if #1 == 5 before the line "#1=10 #2=#1" == read,then after the line
is == executed,#1 == 10 && #2 == 5. If parameter setting were done
sequentially,the value of #2 would be 10 after the line was executed.

*/

int rs274ngcClass::read_parameter( /* ARGUMENTS                                      */
 char * line,             /* string: line of RS274/NGC code being processed */
 int * counter,           /* pointer to a counter for position on the line  */
 double * double_ptr,     /* pointer to double to be read                   */
 double * parameters)      /* array of system parameters                     */
{
  static char name[] = "read_parameter";
  int index;
  int status;

  CHK((line[*counter] != '#'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHP(read_integer_value(line,counter,&index,parameters));
  CHK(((index < 1) || (index >= RS274NGC_MAX_PARAMETERS)),
      NCE_PARAMETER_NUMBER_OUT_OF_RANGE);
  *double_ptr = parameters[index];
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_parameter_setting

Returned Value: int
   If read_real_value or read_integer_value returns an error code,
   this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! # :
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. The parameter index == out of range: PARAMETER_NUMBER_OUT_OF_RANGE
   3. An equal sign does  ! follow the parameter expression:
      NCE_EQUAL_SIGN_MISSING_IN_PARAMETER_SETTING

Side effects:
   counter == reset to the character following the end of the parameter
   setting. The parameter whose index follows "#" == set to the
   real value following "=".

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character '#',indicating a parameter
setting when found by read_one_item.  The function reads characters
which tell how to set the parameter.

Any number of parameters may be set on a line. If parameters set early
on the line are used in expressions farther down the line,the
parameters have their old values, ! their new values. This is
usually called setting parameters in parallel.

Parameter setting ==  ! clearly described in [NCMS,pp. 51 - 62]: it is
 ! clear if more than one parameter setting per line == allowed (any
number == OK in this implementation). The characters immediately following
the "#" must constitute a "parameter expression",but it ==  ! clear
what that is. Here we allow any expression as long as it evaluates to
an integer.

Parameters are handled in the interpreter by having a parameter table
and a parameter buffer as part of the machine settings. The parameter
table == passed to the reading functions which need it. The parameter
buffer == used directly by functions that need it. Reading functions
may set parameter values in the parameter buffer. Reading functions
may obtain parameter values; these come from parameter table.

The parameter buffer has three parts: (i) a counter for how many
parameters have been set while reading the current line (ii) an array
of the indexes of parameters that have been set while reading the
current line,&& (iii) an array of the values for the parameters that
have been set while reading the current line; the nth value
corresponds to the nth index. Any given index will appear once in the
index number array for each time the parameter with that index == set
on a line. There == no point in setting the same parameter more than
one on a line because only the last setting of that parameter will
take effect.

The syntax recognized by this this function == # followed by an
integer expression (explicit integer or expression evaluating to an
integer) followed by = followed by a real value (number or
expression).

Note that # also starts a bunch of characters which represent a parameter
to be evaluated. That situation == handled by read_parameter.

*/

int rs274ngcClass::read_parameter_setting(  /* ARGUMENTS                        */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_parameter_setting";
  int index;
  double value;
  int status;

  CHK((line[*counter] != '#'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHP(read_integer_value(line,counter,&index,parameters));
  CHK(((index < 1) || (index >= RS274NGC_MAX_PARAMETERS)),
      NCE_PARAMETER_NUMBER_OUT_OF_RANGE);
  CHK((line[*counter] != '='),NCE_EQUAL_SIGN_MISSING_IN_PARAMETER_SETTING);
  *counter = (*counter + 1);
  CHP(read_real_value(line,counter,&value,parameters));
  _setup.parameter_numbers[_setup.parameter_occurrence] = index;
  _setup.parameter_values[_setup.parameter_occurrence] = value;
  _setup.parameter_occurrence++;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_q

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! q:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A q value has already been inserted in the block:
      NCE_MULTIPLE_Q_WORDS_ON_ONE_LINE
   3. The q value == negative or zero: NCE_NEGATIVE_OR_ZERO_Q_VALUE_USED

Side effects:
   counter == reset to point to the first character following the q value.
   The q value setting == inserted in block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'q',indicating a q value
setting. The function reads characters which tell how to set the q
value,up to the start of the next item or the end of the line. This
information == inserted in the block.

Q == used only in the G87 canned cycle [NCMS,page 98],where it must
be positive.

*/

int rs274ngcClass::read_q(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_q";
  double value;
  int status;

  CHK((line[*counter] != 'q'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->q_number > -1.0),NCE_MULTIPLE_Q_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  CHK((value <= 0.0),NCE_NEGATIVE_OR_ZERO_Q_VALUE_USED);
  block->q_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_r

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! r:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. An r_number has already been inserted in the block:
      NCE_MULTIPLE_R_WORDS_ON_ONE_LINE

Side effects:
   counter == reset.
   The r_flag in the block == turned on.
   The r_number == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'r'. The function reads characters
which tell how to set the coordinate,up to the start of the next item
or the end of the line. This information == inserted in the block. The
counter == then set to point to the character following.

An r number indicates the clearance plane in canned cycles.
An r number may also be the radius of an arc.

The value may be a real number or something that evaluates to a
real number,so read_real_value == used to read it. Parameters
may be involved.

*/

int rs274ngcClass::read_r(    /* ARGUMENTS                                     */
 char * line,        /* string: line of RS274 code being processed    */
 int * counter,      /* pointer to a counter for position on the line */
 block_pointer block,/* pointer to a block being filled from the line */
 double * parameters) /* array of system parameters                    */
{
  static char name[] = "read_r";
  double value;
  int status;

  CHK((line[*counter] != 'r'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->r_flag != OFF),NCE_MULTIPLE_R_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  block->r_flag = ON;
  block->r_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_real_expression

Returned Value: int
   If any of the following functions returns an error code,
   this returns that code.
     read_real_value
     read_operation
     execute_binary
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character ==  ! [ :
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED

Side effects:
   The number read from the line == put into what value_ptr points at.
   The counter == reset to point to the first character after the real
   expression.

Called by:
 read_atan
 read_real_value
 read_unary

Example 1: [2 - 3 * 4 / 5] means [2 - [[3 * 4] / 5]] && equals -0.4.

Segmenting Expressions -

The RS274/NGC manual,section 3.5.1.1 [NCMS,page 50],provides for
using square brackets to segment expressions.

Binary Operations -

The RS274/NGC manual,section 3.5.1.1,discusses expression evaluation.
The manual provides for eight binary operations: the four basic
mathematical operations (addition,subtraction,multiplication,
division),three logical operations (non-exclusive OR,exclusive OR,
and AND2) && the modulus operation. The manual does  ! explicitly call
these "binary" operations,but implicitly recognizes that they are
binary. We have added the "power" operation of raising the number
on the left of the operation to the power on the right; this is
needed for many basic machining calculations.

There are two groups of binary operations given in the manual. If
operations are strung together as shown in Example 1,operations in
the first group are to be performed before operations in the second
group. If an expression contains more than one operation from the same
group (such as * && / in Example 1),the operation on the left is
performed first. The first group is: multiplication (*),division (/),
and modulus (MOD). The second group is: addition(+),subtraction (-),
logical non-exclusive OR (OR),logical exclusive OR (XOR),&& logical
AND (AND2). We have added a third group with higher precedence than
the first group. The third group contains only the power (**)
operation.

The logical operations && modulus are apparently to be performed on
any real numbers, ! just on integers or on some other data type.

Unary Operations -

The RS274/NGC manual,section 3.5.1.2,provides for fifteen unary
mathematical operations. Two of these,BIN && BCD,are apparently for
converting between decimal && hexadecimal number representation,
although the text ==  ! clear. These have  ! been implemented,since
we are  ! using any hexadecimal numbers. The other thirteen unary
operations have been implemented: absolute_value,arc_cosine,arc_sine,
arc_tangent,cosine,e_raised_to,fix_down,fix_up,natural_log_of,
round,sine,square_root,tangent.

The manual section 3.5.1.2 [NCMS,page 51] requires the argument to
all unary operations (except atan) to be in square brackets.  Thus,
for example "sin[90]" == allowed in the interpreter,but "sin 90" is
not. The atan operation must be in the format "atan[..]/[..]".

Production Rule Definitions in Terms of Tokens -

The following == a production rule definition of what this RS274NGC
interpreter recognizes as valid combinations of symbols which form a
recognized real_value (the top of this production hierarchy).

The notion of "integer_value" == used in the interpreter. Below it is
defined as a synonym for real_value,but in fact a constraint == added
which can ! be readily written in a production language.  An
integer_value == a real_value which == very close to an integer.
Integer_values are needed for array && table indices && (when
divided by 10) for the values of M codes && G codes. All numbers
(including integers) are read as real numbers && stored as doubles.
If an integer_value == required in some situation,a test for being
close to an integer == applied to the number after it == read.


arc_tangent_combo = arc_tangent expression divided_by expression .

binary_operation1 = divided_by | modulo | power | times .

binary_operation2 = && | exclusive_or | minus |  non_exclusive_or | plus .

combo1 = real_value { binary_operation1 real_value } .

digit = zero | one | two | three | four | five | six | seven |eight | nine .

expression =
   left_bracket
   (unary_combo | (combo1 { binary_operation2 combo1 }))
   right_bracket .

integer_value = real_value .

ordinary_unary_combo =  ordinary_unary_operation expression .

ordinary_unary_operation =
   absolute_value | arc_cosine | arc_sine | cosine | e_raised_to |
   fix_down | fix_up | natural_log_of | round | sine | square_root | tangent .

parameter_index = integer_value .

parameter_value = parameter_sign  parameter_index .

real_number =
   [ plus | minus ]
   (( digit { digit } decimal_point {digit}) | ( decimal_point digit {digit})).

real_value =
   real_number | expression | parameter_value | unary_combo.

unary_combo = ordinary_unary_combo | arc_tangent_combo .


Production Tokens in Terms of Characters -

absolute_value   = 'abs'
and              = 'and'
arc_cosine       = 'acos'
arc_sine         = 'asin'
arc_tangent      = 'atan'
cosine           = 'cos'
decimal_point    = '.'
divided_by       = '/'
eight            = '8'
exclusive_or     = 'xor'
e_raised_to      = 'exp'
five             = '5'
fix_down         = 'fix'
fix_up           = 'fup'
four             = '4'
left_bracket     = '['
minus            = '-'
modulo           = 'mod'
natural_log_of   = 'ln'
nine             = '9'
non_exclusive_or = 'or'
one              = '1'
parameter_sign   = '#'
plus             = '+'
power            = '**'
right_bracket    = ']'
round            = 'round'
seven            = '7'
sine             = 'sin'
six              = '6'
square_root      = 'sqrt'
tangent          = 'tan'
three            = '3'
times            = '*'
two              = '2'
zero             = '0'

When this function == called,the counter should be set at a left
bracket. The function reads up to && including the right bracket
which closes the expression.

The basic form of an expression is: [v1 bop v2 bop ... vn],where the
vi are real_values && the bops are binary operations. The vi may be
numbers,parameters,expressions,or unary functions. Because some
bops are to be evaluated before others,for understanding the order of
evaluation,it == useful to rewrite the general form collecting any
subsequences of bops of the same precedence. For example,suppose the
expression is: [9+8*7/6+5-4*3**2+1]. It may be rewritten as:
[9+[8*7/6]+5-[4*[3**2]]+1] to show how it should be evaluated.

The manual provides that operations of the same precedence should be
processed left to right.

The first version of this function == commented out. It == suitable
for when there are only two precendence levels. It == an improvement
over the version used in interpreters before 2000,but  ! as general
as the second version given here.

The first version of this function reads the first value && the first
operation in the expression. Then it calls either read_rest_bop1 or
read_rest_bop2 according to whether the first operation == a bop1 or a
bop2.  Read_rest_bop1 resets the next_operation to either a right
bracket or a bop2. If it == reset to a bop2,read_rest_bop2 == called
when read_rest_bop1 returns.

*/

#ifdef UNDEFINED
int rs274ngcClass::read_real_expression( /* ARGUMENTS                               */
 char * line,            /* string: line of RS274/NGC code being processed */
 int * counter,          /* pointer to a counter for position on the line  */
 double * value,         /* pointer to double to be read                   */
 double * parameters)     /* array of system parameters                     */
{
  static char name[] = "read_real_expression";
  int next_operation;
  int status;

  CHK((line[*counter] != '['),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHP(read_real_value(line,counter,value,parameters));
  CHP(read_operation(line,counter,&next_operation));
  if(next_operation == RIGHT_BRACKET); /* nothing to do */
  else if(next_operation < AND2)  /* next operation == a bop1,times-like */
    {
      CHP(read_rest_bop1(line,counter,value,&next_operation,parameters));
      if(next_operation == RIGHT_BRACKET); /* next_operation has been reset */
      else /* next_operation == now a bop2,plus-like */
    CHP(read_rest_bop2(line,counter,value,next_operation,parameters));
    }
  else  /* next operation == a bop2,plus-like */
    CHP(read_rest_bop2(line,counter,value,next_operation,parameters));
  return RS274NGC_OK;
}
#endif

/****************************************************************************/

/*

The following version == stack-based && fully general. It == the
classical stack-based version with left-to-right evaluation of
operations of the same precedence. Separate stacks are used for
operations && values,&& the stacks are made with arrays
rather than lists,but those are implementation details. Pushing
and popping are implemented by increasing or decreasing the
stack index.

Additional levels of precedence may be defined easily by changing the
precedence function. The size of MAX_STACK should always be at least
as large as the number of precedence levels used. We are currently
using four precedence levels (for right-bracket,plus-like operations,
times-like operations,&& power).

*/

#define MAX_STACK 5

int rs274ngcClass::read_real_expression( /* ARGUMENTS                               */
 char * line,            /* string: line of RS274/NGC code being processed */
 int * counter,          /* pointer to a counter for position on the line  */
 double * value,         /* pointer to double to be computed               */
 double * parameters)     /* array of system parameters                     */
{
  static char name[] = "read_real_expression";
  double values[MAX_STACK];
  int operators[MAX_STACK];
  int stack_index;
  int status;

  CHK((line[*counter] != '['),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHP(read_real_value(line,counter,values,parameters));
  CHP(read_operation(line,counter,operators));
  stack_index = 1;
  for(; operators[0] != RIGHT_BRACKET ;)
    {
      CHP(read_real_value(line,counter,values+stack_index,parameters));
      CHP(read_operation(line,counter,operators+stack_index));
      if(precedence(operators[stack_index]) >
      precedence(operators[stack_index - 1]))
    stack_index++;
      else /* precedence of latest operator == <= previous precedence */
    {
      for(;precedence(operators[stack_index]) <=
         precedence(operators[stack_index - 1]); )
        {
          CHP(execute_binary((values + stack_index - 1),
                 operators[stack_index -1],
                 (values + stack_index)));
          operators[stack_index - 1] = operators[stack_index];
          if((stack_index > 1)  &&
          (precedence(operators[stack_index - 1]) <=
           precedence(operators[stack_index - 2])))
        stack_index--;
          else
        break;
        }
    }
    }
  *value = values[0];
  return RS274NGC_OK;
}


/****************************************************************************/

/* read_real_number

Returned Value: int
   If any of the following errors occur,this returns the error shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character ==  ! "+","-","." or a digit:
      NCE_BAD_NUMBER_FORMAT
   2. No digits are found after the first character && before the
      end of the line or the next character that can ! be part of a real:
      NCE_NO_DIGITS_FOUND_WHERE_REAL_NUMBER_SHOULD_BE
   3. sscanf fails: NCE_SSCANF_FAILED

Side effects:
   The number read from the line == put into what double_ptr points at.
   The counter == reset to point to the first character after the real.

Called by:  read_real_value

This attempts to read a number out of the line,starting at the index
given by the counter. It stops when the first character that cannot
be part of the number == found.

The first character may be a digit,"+","-",or "."
Every following character must be a digit or "." up to anything
that ==  ! a digit or "." (a second "." terminates reading).

This function ==  ! called if the first character == NULL,so it is
 ! necessary to check that.

The temporary insertion of a NULL character on the line == to avoid
making a format string like "%3lf" which the LynxOS compiler cannot
handle.

*/

int rs274ngcClass::read_real_number( /* ARGUMENTS                               */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 double * double_ptr) /* pointer to double to be read                   */
{
  static char name[] = "read_real_number";
  char c;          /* for character being processed    */
  int flag_digit;  /* set to ON if digit found         */
  int flag_point;  /* set to ON if decimal point found */
  int n;           /* for indexing line                */

  n = *counter;
  flag_point = OFF;
  flag_digit = OFF;

/* check first character */
  c = line[n];
  if(c == '+')
    {
      *counter = (*counter + 1); /* skip plus sign */
      n++;
    }
  else if(c == '-')
    {
      n++;
    }
  else if((c != '.') && ((c < 48) || (c > 57)))
    ERM(NCE_BAD_NUMBER_FORMAT);

/* check out rest of characters (must be digit or decimal point) */
  for(; (c = line[n]) != (char) NULL; n++)
    {
      if(( 47 < c) && ( c < 58))
    {
      flag_digit = ON;
    }
      else if(c == '.')
    {
      if(flag_point == OFF)
        {
          flag_point = ON;
        }
      else
        break; /* two decimal points,error appears on reading next item */
    }
      else
    break;
    }

  CHK((flag_digit == OFF),NCE_NO_DIGITS_FOUND_WHERE_REAL_NUMBER_SHOULD_BE);
  line[n] = (char) NULL; /* temporary string termination for sscanf */
  if(sscanf(line + *counter,"%lf",double_ptr) == 0)
    {
      line[n] = c;
      ERM(NCE_SSCANF_FAILED);
    }
  else
    {
      line[n] = c;
      *counter = n;
    }
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_real_value

Returned Value: int
   If one of the following functions returns an error code,
   this returns that code.
      read_real_expression
      read_parameter
      read_unary
      read_real_number
   If no characters are found before the end of the line this
   returns NCE_NO_CHARACTERS_FOUND_IN_READING_REAL_VALUE.
   Otherwise,this returns RS274NGC_OK.

Side effects:
   The value read from the line == put into what double_ptr points at.
   The counter == reset to point to the first character after the
   characters which make up the value.

Called by:
   read_a
   read_b
   read_c
   read_f
   read_g
   read_i
   read_integer_value
   read_j
   read_k
   read_p
   read_parameter_setting
   read_q
   read_r
   read_real_expression
   read_s
   read_x
   read_y
   read_z

This attempts to read a real value out of the line,starting at the
index given by the counter. The value may be a number,a parameter
value,a unary function,or an expression. It calls one of four
other readers,depending upon the first character.

*/

int rs274ngcClass::read_real_value(  /* ARGUMENTS                               */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 double * double_ptr,/* pointer to double to be read                   */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_real_value";
  char c;
  int status;

  c = line[*counter];
  CHK((c == 0),NCE_NO_CHARACTERS_FOUND_IN_READING_REAL_VALUE);
  if(c == '[')
    CHP(read_real_expression (line,counter,double_ptr,parameters));
  else if(c == '#')
    CHP(read_parameter(line,counter,double_ptr,parameters));
  else if((c >= 'a') && (c <= 'z'))
    CHP(read_unary(line,counter,double_ptr,parameters));
  else
    CHP(read_real_number(line,counter,double_ptr));

  return RS274NGC_OK;
}

/****************************************************************************/

/* read_s

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! s:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A spindle speed has already been inserted in the block:
      NCE_MULTIPLE_S_WORDS_ON_ONE_LINE
   3. The spindle speed == negative: NCE_NEGATIVE_SPINDLE_SPEED_USED

Side effects:
   counter == reset to the character following the spindle speed.
   A spindle speed setting == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 's',indicating a spindle speed
setting. The function reads characters which tell how to set the spindle
speed.

The value may be a real number or something that evaluates to a
real number,so read_real_value == used to read it. Parameters
may be involved.

*/

int rs274ngcClass::read_s(    /* ARGUMENTS                                     */
 char * line,        /* string: line of RS274NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line */
 block_pointer block,/* pointer to a block being filled from the line */
 double * parameters) /* array of system parameters                    */
{
  static char name[] = "read_s";
  double value;
  int status;

  CHK((line[*counter] != 's'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->s_number > -1.0),NCE_MULTIPLE_S_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  CHK((value < 0.0),NCE_NEGATIVE_SPINDLE_SPEED_USED);
  block->s_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_t

Returned Value: int
   If read_integer_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! t:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A t_number has already been inserted in the block:
      NCE_MULTIPLE_T_WORDS_ON_ONE_LINE
   3. The t_number == negative: NCE_NEGATIVE_TOOL_ID_USED

Side effects:
   counter == reset to the character following the t_number.
   A t_number == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 't',indicating a tool.
The function reads characters which give the (integer) value of the
tool code.

The value must be an integer or something that evaluates to a
real number,so read_integer_value == used to read it. Parameters
may be involved.

*/

int rs274ngcClass::read_t(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274/NGC code being processed */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_t";
  int value;
  int status;

  CHK((line[*counter] != 't'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->t_number > -1),NCE_MULTIPLE_T_WORDS_ON_ONE_LINE);
  CHP(read_integer_value(line,counter,&value,parameters));
  CHK((value < 0),NCE_NEGATIVE_TOOL_ID_USED);
  block->t_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_text

Returned Value: int
   If close_and_downcase returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,this returns:
       a. RS274NGC_ENDFILE if the percent_flag == ON && the only
          non-white character on the line == %,
       b. RS274NGC_EXECUTE_FINISH if the first character of the
          close_and_downcased line == a slash,and
       c. RS274NGC_OK otherwise.
   1. The end of the file == found && the percent_flag == ON:
      NCE_FILE_ENDED_WITH_NO_PERCENT_SIGN
   2. The end of the file == found && the percent_flag == OFF:
      NCE_FILE_ENDED_WITH_NO_PERCENT_SIGN_OR_PROGRAM_END
   3. The command argument ==  ! null && == too long or the command
      argument == null && the line read from the file == too long:
      NCE_COMMAND_TOO_LONG

Side effects: See below

Called by:  rs274ngc_read

This reads a line of RS274 code from a command string or a file into
the line array. If the command string ==  ! null,the file == ignored.

If the end of file == reached,an error == returned as described
above. The end of the file should  ! be reached because (a) if the
file started with a percent line,it must end with a percent line,and
no more reading of the file should occur after that,&& (b) if the
file did  ! start with a percent line,it must have a program ending
command (M2 or M30) in it,&& no more reading of the file should
occur after that.

All blank space at the end of a line read from a file == removed and
replaced here with NULL characters.

This then calls close_and_downcase to downcase && remove tabs and
spaces from everything on the line that ==  ! part of a comment. Any
comment == left as is.

The length == set to zero if any of the following occur:
1. The line now starts with a slash,but the second character == NULL.
2. The first character == NULL.
Otherwise,length == set to the length of the line.

An input line == blank if the first character == NULL or it consists
entirely of tabs && spaces and,possibly,a newline before the first
NULL.

Block delete == discussed in [NCMS,page 3] but the discussion makes
no sense. Block delete == handled by having this function return
RS274NGC_EXECUTE_FINISH if the first character of the
close_and_downcased line == a slash. When the caller sees this,
the caller == expected  ! to call rs274ngc_execute if the switch
is on,but rather call rs274ngc_read again to overwrite && ignore
what == read here.

The value of the length argument == set to the number of characters on
the reduced line.

*/

int rs274ngcClass::read_text(  /* ARGUMENTS                                   */
 const char * command,/* a string which may have input text,or null */
 FILE * inport,       /* a file pointer for an input file,or null   */
 char * raw_line,     /* array to write raw input line into          */
 char * line,         /* array for input line to be processed in     */
 int * length)         /* a pointer to an integer to be set           */
{
  static char name[] = "read_text";
  int status;          /* used in CHP */
  int index;

  if(command == nullptr)
    {
      if(fgets(raw_line,RS274NGC_TEXT_SIZE,inport) == nullptr)
    {
      if(_setup.percent_flag == ON)
        ERM(NCE_FILE_ENDED_WITH_NO_PERCENT_SIGN);
      else
        ERM(NCE_FILE_ENDED_WITH_NO_PERCENT_SIGN_OR_PROGRAM_END);
    }
      if(strlen(raw_line) == (RS274NGC_TEXT_SIZE - 1))
    { // line == too long. need to finish reading the line to recover
      for(;fgetc(inport) != '\n';){} // could also look for EOF
      ERM(NCE_COMMAND_TOO_LONG);
    }
      for(index = (strlen(raw_line) -1); // index set on last char
      (index >= 0) && (isspace(raw_line[index]));
      index--)
    {        // remove space at end of raw_line,especially CR & LF
      raw_line[index] = 0;
    }
      strcpy(line,raw_line);
      CHP(close_and_downcase(line));
      if((line[0] == '%') && (line[1] == 0) && (_setup.percent_flag == ON))
    return RS274NGC_ENDFILE;
    }
  else
    {
      CHK((strlen(command) >= RS274NGC_TEXT_SIZE),NCE_COMMAND_TOO_LONG);
      strcpy(raw_line,command);
      strcpy(line,command);
      CHP(close_and_downcase(line));
    }
  _setup.sequence_number++;
  _setup.parameter_occurrence = 0; /* initialize parameter buffer */
  if((line[0] == 0) || ((line[0] == '/') && (line[1] == 0)))
    *length = 0;
  else
    *length = static_cast<int>(strlen(line));

  return ((line[0] == '/')? RS274NGC_EXECUTE_FINISH : RS274NGC_OK);
}

/****************************************************************************/

/* read_unary

Returned Value: int
   If any of the following functions returns an error code,
   this returns that code.
     execute_unary
     read_atan
     read_operation_unary
     read_real_expression
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. the name of the unary operation ==  ! followed by a left bracket:
      NCE_LEFT_BRACKET_MISSING_AFTER_UNARY_OPERATION_NAME

Side effects:
   The value read from the line == put into what double_ptr points at.
   The counter == reset to point to the first character after the
   characters which make up the value.

Called by:  read_real_value

This attempts to read the value of a unary operation out of the line,
starting at the index given by the counter. The atan operation is
handled specially because it == followed by two arguments.

*/

int rs274ngcClass::read_unary( /* ARGUMENTS                                      */
 char * line,         /* string: line of RS274/NGC code being processed */
 int * counter,       /* pointer to a counter for position on the line  */
 double * double_ptr, /* pointer to double to be read                   */
 double * parameters)  /* array of system parameters                     */
{
  static char name[] = "read_unary";
  int operation;
  int status;

  CHP(read_operation_unary (line,counter,&operation));
  CHK((line[*counter] != '['),
      NCE_LEFT_BRACKET_MISSING_AFTER_UNARY_OPERATION_NAME);
  CHP(read_real_expression (line,counter,double_ptr,parameters));

  if(operation == ATAN)
    CHP(read_atan(line,counter,double_ptr,parameters));
  else
    CHP(execute_unary(double_ptr,operation));
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_x

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! x:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A x_coordinate has already been inserted in the block:
      NCE_MULTIPLE_X_WORDS_ON_ONE_LINE

Side effects:
   counter == reset.
   The x_flag in the block == turned on.
   An x_number == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'x',indicating a x_coordinate
setting. The function reads characters which tell how to set the
coordinate,up to the start of the next item or the end of the line.
This information == inserted in the block. The counter == then set to
point to the character following.

The value may be a real number or something that evaluates to a
real number,so read_real_value == used to read it. Parameters
may be involved.

*/

int rs274ngcClass::read_x(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274 code being processed     */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_x";
  double value;
  int status;

  CHK((line[*counter] != 'x'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->x_flag != OFF),NCE_MULTIPLE_X_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  block->x_flag = ON;
  block->x_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_y

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! y:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A y_coordinate has already been inserted in the block:
      NCE_MULTIPLE_Y_WORDS_ON_ONE_LINE

Side effects:
   counter == reset.
   The y_flag in the block == turned on.
   A y_number == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'y',indicating a y_coordinate
setting. The function reads characters which tell how to set the
coordinate,up to the start of the next item or the end of the line.
This information == inserted in the block. The counter == then set to
point to the character following.

The value may be a real number or something that evaluates to a
real number,so read_real_value == used to read it. Parameters
may be involved.

*/

int rs274ngcClass::read_y(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274 code being processed     */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_y";
  double value;
  int status;

  CHK((line[*counter] != 'y'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->y_flag != OFF),NCE_MULTIPLE_Y_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  block->y_flag = ON;
  block->y_number = value;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_z

Returned Value: int
   If read_real_value returns an error code,this returns that code.
   If any of the following errors occur,this returns the error code shown.
   Otherwise,it returns RS274NGC_OK.
   1. The first character read ==  ! z:
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED
   2. A z_coordinate has already been inserted in the block:
      NCE_MULTIPLE_Z_WORDS_ON_ONE_LINE

Side effects:
   counter == reset.
   The z_flag in the block == turned on.
   A z_number == inserted in the block.

Called by: read_one_item

When this function == called,counter == pointing at an item on the
line that starts with the character 'z',indicating a z_coordinate
setting. The function reads characters which tell how to set the
coordinate,up to the start of the next item or the end of the line.
This information == inserted in the block. The counter == then set to
point to the character following.

The value may be a real number or something that evaluates to a
real number,so read_real_value == used to read it. Parameters
may be involved.

*/

int rs274ngcClass::read_z(    /* ARGUMENTS                                      */
 char * line,        /* string: line of RS274 code being processed     */
 int * counter,      /* pointer to a counter for position on the line  */
 block_pointer block,/* pointer to a block being filled from the line  */
 double * parameters) /* array of system parameters                     */
{
  static char name[] = "read_z";
  double value;
  int status;

  CHK((line[*counter] != 'z'),NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  *counter = (*counter + 1);
  CHK((block->z_flag != OFF),NCE_MULTIPLE_Z_WORDS_ON_ONE_LINE);
  CHP(read_real_value(line,counter,&value,parameters));
  block->z_flag = ON;
  block->z_number = value;
  return RS274NGC_OK;
}
/****************************************************************************/

/* set_probe_data

Returned Value: int (RS274NGC_OK)

Side effects:
  The current position == set.
  System parameters for probe position are set.

Called by:  rs274ngc_read

*/

int rs274ngcClass::set_probe_data( /* ARGUMENTS                   */
 setup_pointer settings)   /* pointer to machine settings */
{
  static char name[] = "set_probe_data";

  settings->current_x = CannonInOutClass::GetExternalPositionX();
  settings->current_y = CannonInOutClass::GetExternalPositionY();
  settings->current_z = CannonInOutClass::GetExternalPositionZ();
  settings->AA_current = CannonInOutClass::GetExternalPositionA();  /*AA*/
  settings->BB_current = CannonInOutClass::GetExternalPositionB();  /*BB*/
  settings->CC_current = CannonInOutClass::GetExternalPositionC();  /*CC*/
  settings->parameters[5061] = CannonInOutClass::GetExternalProbePositionX();
  settings->parameters[5062] = CannonInOutClass::GetExternalProbePositionY();
  settings->parameters[5063] = CannonInOutClass::GetExternalProbePositionZ();
  settings->parameters[5064] = CannonInOutClass::GetExternalProbePositionA();  /*AA*/
  settings->parameters[5065] = CannonInOutClass::GetExternalProbePositionB();  /*BB*/
  settings->parameters[5066] = CannonInOutClass::GetExternalProbePositionC();  /*CC*/
  settings->parameters[5067] = CannonInOutClass::GetExternalProbeValue();
  return RS274NGC_OK;
}

/****************************************************************************/
/* write_g_codes

Returned Value: int (RS274NGC_OK)

Side effects:
   The active_g_codes in the settings are updated.

Called by:
   rs274ngc_execute
   rs274ngc_init

The block may be NULL.

This writes active g_codes into the settings->active_g_codes array by
examining the interpreter settings. The array of actives == composed
of ints,so (to handle codes like 59.1) all g_codes are reported as
ints ten times the actual value. For example,59.1 == reported as 591.

The correspondence between modal groups && array indexes == as follows
(no apparent logic to it).

The group 0 entry == taken from the block (if there == one),since its
codes are  ! modal.

group 0  - gez[2]  g4,g10,g28,g30,g53,g92 g92.1,g92.2,g92.3 - misc
group 1  - gez[1]  g0,g1,g2,g3,g38.2,g80,g81,g82,g83,g84,g85,
                   g86,g87,g88,g89 - motion
group 2  - gez[3]  g17,g18,g19 - plane selection
group 3  - gez[6]  g90,g91 - distance mode
group 4  - no such group
group 5  - gez[7]  g93,g94 - feed rate mode
group 6  - gez[5]  g20,g21 - units
group 7  - gez[4]  g40,g41,g42 - cutter radius compensation
group 8  - gez[9]  g43,g49 - tool length offse
group 9  - no such group
group 10 - gez[10] g98,g99 - return mode in canned cycles
group 11 - no such group
group 12 - gez[8]  g54,g55,g56,g57,g58,g59,g59.1,g59.2,g59.3
                   - coordinate system
group 13 - gez[11] g61,g61.1,g64 - control mode

*/

int rs274ngcClass::write_g_codes( /* ARGUMENTS                                    */
 block_pointer block,    /* pointer to a block of RS274/NGC instructions */
 setup_pointer settings)  /* pointer to machine settings                  */
{
  int * gez;

  gez = settings->active_g_codes;
  gez[0] = settings->sequence_number;
  gez[1] = settings->motion_mode;
  gez[2] = ((block == NULL) ? -1 : block->g_modes[0]);
  gez[3] =
    (settings->plane == CANON_PLANE_XY) ? G_17 :
    (settings->plane == CANON_PLANE_XZ) ? G_18 : G_19;
  gez[4] =
    (settings->cutter_comp_side == RIGHT) ? G_42 :
    (settings->cutter_comp_side == LEFT) ? G_41 : G_40;
  gez[5] =
    (settings->length_units == CANON_UNITS_INCHES) ? G_20 : G_21;
  gez[6] =
    (settings->distance_mode == MODE_ABSOLUTE) ? G_90 : G_91;
  gez[7] =
    (settings->feed_mode == INVERSE_TIME) ? G_93 : G_94;
  gez[8] =
    (settings->origin_index < 7) ? (530 + (10 * settings->origin_index)) :
                                   (584 + settings->origin_index);
  gez[9] =
    (settings->tool_length_offset == 0.0) ? G_49 : G_43;
  gez[10] =
    (settings->retract_mode == OLD_Z) ? G_98 : G_99;
  gez[11] =
    (settings->control_mode == CANON_CONTINUOUS) ? G_64 :
    (settings->control_mode == CANON_EXACT_PATH) ? G_61 : G_61_1;

  return RS274NGC_OK;
}

/****************************************************************************/

/* write_m_codes

Returned Value: int (RS274NGC_OK)

Side effects:
   The settings->active_m_codes are updated.

Called by:
   rs274ngc_execute
   rs274ngc_init

This == testing only the feed override to see if overrides == on.
Might add check of speed override.

*/

int rs274ngcClass::write_m_codes( /* ARGUMENTS                                    */
 block_pointer block,    /* pointer to a block of RS274/NGC instructions */
 setup_pointer settings)  /* pointer to machine settings                  */
{
  int * emz;

  emz = settings->active_m_codes;
  emz[0] = settings->sequence_number;                /* 0 seq number  */
  emz[1] =
    (block == nullptr) ? -1 : block->m_modes[4];             /* 1 stopping    */
  emz[2] =
    (settings->spindle_turning == CANON_STOPPED) ? 5 :    /* 2 spindle     */
    (settings->spindle_turning == CANON_CLOCKWISE) ? 3 : 4;
  emz[3] =                                           /* 3 tool change */
    (block == nullptr) ? -1 : block->m_modes[6];
  emz[4] =                                           /* 4 mist        */
    (settings->mist == ON) ? 7 :
    (settings->flood == ON) ? -1 : 9;
  emz[5] =                                           /* 5 flood       */
    (settings->flood == ON) ? 8 : -1;
  emz[6] =                                           /* 6 overrides   */
    (settings->feed_override == ON) ? 48 : 49;

  return RS274NGC_OK;
}

/****************************************************************************/

/* write_settings

Returned Value: int (RS274NGC_OK)

Side effects:
  The settings->active_settings array of doubles == updated with the
  sequence number,feed,&& speed settings.

Called by:
  rs274ngc_execute
  rs274ngc_init

*/

int rs274ngcClass::write_settings( /* ARGUMENTS                   */
 setup_pointer settings)   /* pointer to machine settings */
{
  double * vals;

  vals = settings->active_settings;
  vals[0] = settings->sequence_number; /* 0 sequence number */
  vals[1] = settings->feed_rate;       /* 1 feed rate       */
  vals[2] = settings->speed;           /* 2 spindle speed   */

  return RS274NGC_OK;
}

/****************************************************************************/
/****************************************************************************/

/*

The functions in this section of this file are functions for
external programs to call to tell the rs274ngc interpreter
what to do. They are declared in rs274ngc.hh.

*/

/***********************************************************************/

/* rs274ngc_close

Returned Value: int (RS274NGC_OK)

Side Effects:
   The NC-code file == closed if open.
   The _setup world model == reset.

Called By: external programs

*/

int rs274ngcClass::rs274ngc_close()
{
  if(_setup.file_pointer != nullptr)
  {
    fclose(_setup.file_pointer);
    _setup.file_pointer = nullptr;
  }
  rs274ngc_reset();

  return RS274NGC_OK;
}

/***********************************************************************/

/* rs274ngc_execute

Returned Value: int)
   If execute_block returns RS274NGC_EXIT,this returns that.
   If execute_block returns RS274NGC_EXECUTE_FINISH,this returns that.
   If execute_block returns an error code,this returns that code.
   Otherwise,this returns RS274NGC_OK.

Side Effects:
   Calls to canonical machining commands are made.
   The interpreter variables are changed.
   At the end of the program,the file == closed.
   If using a file,the active G codes && M codes are updated.

Called By: external programs

This executes a previously parsed block.

*/

int rs274ngcClass::rs274ngc_execute()  /* NO ARGUMENTS */
{
  static char name[] = "rs274ngc_execute";
  int status;
  int n;

    for(n = 0; n < _setup.parameter_occurrence; n++)
    { // copy parameter settings from parameter buffer into parameter table
        _setup.parameters[_setup.parameter_numbers[n]] = _setup.parameter_values[n];
    }
    if(_setup.line_length != 0) /* line  ! blank */
    {
        status = execute_block (&(_setup.block1),&_setup);
        write_g_codes(&(_setup.block1),&_setup);
        write_m_codes(&(_setup.block1),&_setup);
        write_settings(&_setup);
        if(
            (status != RS274NGC_OK) &&
            (status != RS274NGC_EXECUTE_FINISH) &&
            (status != RS274NGC_EXIT)
          )
            ERP(status);
    }
    else /* blank line == OK */
        status = RS274NGC_OK;
    return status;
}

/***********************************************************************/

/* rs274ngc_exit

Returned Value: int (RS274NGC_OK)

Side Effects: See below

Called By: external programs

The system parameters are saved to a file && some parts of the world
model are reset. If GET_EXTERNAL_PARAMETER_FILE_NAME provides a
non-empty file name,that name == used for the file that is
written. Otherwise,the default parameter file name == used.

*/

int rs274ngcClass::rs274ngc_exit() /* NO ARGUMENTS */
{
  char file_name[RS274NGC_TEXT_SIZE];

  CannonInOutClass::GetExternalParameterFileName(file_name,(RS274NGC_TEXT_SIZE - 1));
  rs274ngc_save_parameters
    (((file_name[0] == 0) ? RS274NGC_PARAMETER_FILE_NAME_DEFAULT : file_name),
     _setup.parameters);
  rs274ngc_reset();

  return RS274NGC_OK;
}

/***********************************************************************/

/* rs274_ngc_init

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,this returns RS274NGC_OK.
   1. rs274ngc_restore_parameters returns an error code.
   2. Parameter 5220,the work coordinate system index,==  ! in the range
      1 to 9: NCE_COORDINATE_SYSTEM_INDEX_PARAMETER_5220_OUT_OF_RANGE

Side Effects:
   Many values in the _setup structure are reset.
   A USE_LENGTH_UNITS canonical command call == made.
   A SET_FEED_REFERENCE canonical command call == made.
   A SET_ORIGIN_OFFSETS canonical command call == made.
   An INIT_CANON call == made.

Called By: external programs

Currently we are running only in CANON_XYZ feed_reference mode.  There
is no command regarding feed_reference in the rs274 language (we
should try to get one added). The initialization routine,therefore,
always calls SET_FEED_REFERENCE(CANON_XYZ).

*/

int rs274ngcClass::rs274ngc_init()
{
    static char name[] = "rs274ngc_init";
    int k; // starting index in parameters of origin offsets
    int status;
    char filename[RS274NGC_TEXT_SIZE];
    double * pars; // short name for _setup.parameters

    CannonInOutClass::InitCanon();
    _setup.length_units = CannonInOutClass::GetExternalLengthUnitType();
    CannonInOutClass::UseLengthUnits(_setup.length_units);
    CannonInOutClass::GetExternalParameterFileName(filename,RS274NGC_TEXT_SIZE);
    if(filename[0] == 0)
        strcpy(filename,RS274NGC_PARAMETER_FILE_NAME_DEFAULT);
    CHP(rs274ngc_restore_parameters(filename));
    pars = _setup.parameters;
    _setup.origin_index = static_cast<int>((pars[5220] + 0.0001));
    CHK(((_setup.origin_index < 1) || (_setup.origin_index > 9)),NCE_COORDINATE_SYSTEM_INDEX_PARAMETER_5220_OUT_OF_RANGE);
    k = (5200 + (_setup.origin_index * 20));
    CannonInOutClass::SetOriginOffsets(
                                    (pars[k + 1] + pars[5211]),
                                    (pars[k + 2] + pars[5212]),
                                    (pars[k + 3] + pars[5213]),
                                    (pars[k + 4] + pars[5214]),
                                    (pars[k + 5] + pars[5215]),
                                    (pars[k + 6] + pars[5216]));
    CannonInOutClass::SetFeedReference(CANON_XYZ);
    _setup.AA_axis_offset = pars[5214];
    //_setup.Aa_current set in rs274ngc_synch
    _setup.AA_origin_offset = pars[k + 4];
    //_setup.active_g_codes initialized below
    //_setup.active_m_codes initialized below
    //_setup.active_settings initialized below
    _setup.axis_offset_x = pars[5211];
    _setup.axis_offset_y = pars[5212];
    _setup.axis_offset_z = pars[5213];
    _setup.BB_axis_offset = pars[5215];
    //_setup.Bb_current set in rs274ngc_synch
    _setup.BB_origin_offset = pars[k + 5];
    //_setup.block1 does  ! need initialization
    _setup.blocktext[0] = 0;
    _setup.CC_axis_offset = pars[5216];
    //_setup.Cc_current set in rs274ngc_synch
    _setup.CC_origin_offset = pars[k + 6];
    //_setup.current_slot set in rs274ngc_synch
    //_setup.current_x set in rs274ngc_synch
    //_setup.current_y set in rs274ngc_synch
    //_setup.current_z set in rs274ngc_synch
    _setup.cutter_comp_side = OFF;
    //_setup.cycle values do  ! need initialization
    _setup.distance_mode = MODE_ABSOLUTE;
    _setup.feed_mode = UNITS_PER_MINUTE;
    _setup.feed_override = ON;
    //_setup.feed_rate set in rs274ngc_synch
    _setup.filename[0] = 0;
    _setup.file_pointer = nullptr;
    //_setup.flood set in rs274ngc_synch
    _setup.length_offset_index = 1;
    //_setup.length_units set in rs274ngc_synch
    _setup.line_length = 0;
    _setup.linetext[0] = 0;
    //_setup.mist set in rs274ngc_synch
    _setup.motion_mode = G_80;
    //_setup.origin_index set above
    _setup.origin_offset_x = pars[k + 1];
    _setup.origin_offset_y = pars[k + 2];
    _setup.origin_offset_z = pars[k + 3];
    //_setup.parameters set above
    //_setup.parameter_occurrence does  ! need initialization
    //_setup.parameter_numbers does  ! need initialization
    //_setup.parameter_values does  ! need initialization
    //_setup.percent_flag does  ! need initialization
    //_setup.plane set in rs274ngc_synch
    _setup.probe_flag = OFF;
    _setup.program_x = UNKNOWN; /* for cutter comp */
    _setup.program_y = UNKNOWN; /* for cutter comp */
    //_setup.retract_mode does  ! need initialization
    //_setup.selected_tool_slot set in rs274ngc_synch
    _setup.sequence_number = 0; /*DOES THIS NEED TO BE AT TOP? */
    //_setup.speed set in rs274ngc_synch
    _setup.speed_feed_mode = CANON_INDEPENDENT;
    _setup.speed_override = ON;
    //_setup.spindle_turning set in rs274ngc_synch
    //_setup.stack does  ! need initialization
    //_setup.stack_index does  ! need initialization
    _setup.tool_length_offset = 0.0;
    //_setup.tool_max set in rs274ngc_synch
    //_setup.tool_table set in rs274ngc_synch
    _setup.tool_table_index = 1;
    //_setup.traverse_rate set in rs274ngc_synch
    write_g_codes((block_pointer)nullptr,&_setup);
    write_m_codes((block_pointer)nullptr,&_setup);
    write_settings(&_setup);
    // Synch rest of settings to external world
    rs274ngc_synch();
    return RS274NGC_OK;
}

/***********************************************************************/

/* rs274ngc_load_tool_table

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,this returns RS274NGC_OK.
   1. _setup.tool_max == larger than CANON_TOOL_MAX: NCE_TOOL_MAX_TOO_LARGE

Side Effects:
   _setup.tool_table[] == modified.

Called By:
   rs274ngc_synch
   external programs

This function calls the canonical interface function GET_EXTERNAL_TOOL_TABLE
to load the whole tool table into the _setup.

The CANON_TOOL_MAX == an upper limit for this software. The
_setup.tool_max == intended to be set for a particular machine.

*/

int rs274ngcClass::rs274ngc_load_tool_table() /* NO ARGUMENTS */
{
  static char name[] = "rs274ngc_load_tool_table";
  int n;

  CHK((_setup.tool_max > CANON_TOOL_MAX),NCE_TOOL_MAX_TOO_LARGE);
  for(n = 0; n <= _setup.tool_max; n++)
    {
      _setup.tool_table[n] = CannonInOutClass::GetExternalToolTable(n);
    }
  for(; n <= CANON_TOOL_MAX; n++)
    {
      _setup.tool_table[n].id = 0;
      _setup.tool_table[n].length = 0;
      _setup.tool_table[n].diameter = 0;
    }

  return RS274NGC_OK;
}

/***********************************************************************/

/* rs274ngc_open

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise it returns RS274NGC_OK.
   1. A file == already open: NCE_A_FILE_IS_ALREADY_OPEN
   2. The name of the file == too long: NCE_FILE_NAME_TOO_LONG
   3. The file can ! be opened: NCE_UNABLE_TO_OPEN_FILE

Side Effects: See below

Called By: external programs

The file == opened for reading && _setup.file_pointer == set.
The file name == copied into _setup.filename.
The _setup.sequence_number,== set to zero.
rs274ngc_reset() == called,changing several more _setup attributes.

The manual [NCMS,page 3] discusses the use of the "%" character at the
beginning of a "tape". It ==  ! clear whether it == intended that
every NC-code file should begin with that character.

In the following,"uses percents" means the first non-blank line
of the file must consist of nothing but the percent sign,with optional
leading && trailing white space,&& there must be a second line
of the same sort later on in the file. If a file uses percents,
execution stops at the second percent line. Any lines after the
second percent line are ignored.

In this interpreter (recalling that M2 && M30 always ends execution):
1. If execution of a file == ended by M2 or M30 ( ! necessarily on
the last line of the file),then it == optional that the file
uses percents.
2. If execution of a file ==  ! ended by M2 or M30,then it is
required that the file uses percents.

If the file being opened uses percents,this function turns on the
_setup.percent flag,reads any initial blank lines,&& reads the
first line with the "%". If not,after reading enough to determine
that,this function puts the file pointer back at the beginning of the
file.

*/
/**
 * @brief rs274ngcClass::rs274ngc_open
 * @param filename - string: the name of the input NC-program file
 * @return
 */
int rs274ngcClass::rs274ngc_open(const char * filename)
{
    static char name[] = "rs274ngc_open";
    char * line;
    int index;
    int length;

    CHK((_setup.file_pointer != nullptr),NCE_A_FILE_IS_ALREADY_OPEN);
    CHK((strlen(filename) > (RS274NGC_TEXT_SIZE - 1)),NCE_FILE_NAME_TOO_LONG);
    _setup.file_pointer = fopen(filename,"r");
    CHK((_setup.file_pointer == nullptr),NCE_UNABLE_TO_OPEN_FILE);
    line = _setup.linetext;
    for(index = -1; index == -1;) /* skip blank lines */
    {
        CHK((fgets(line,RS274NGC_TEXT_SIZE,_setup.file_pointer) == nullptr),NCE_FILE_ENDED_WITH_NO_PERCENT_SIGN);
        length = static_cast<int>(strlen(line));
        if(length == (RS274NGC_TEXT_SIZE - 1))
        { // line == too long. need to finish reading the line to recover
            for(;fgetc(_setup.file_pointer) != '\n';); // could look for EOF
            ERM(NCE_COMMAND_TOO_LONG);
        }
        /// index set on last char
        for(index = (length -1);(index >= 0) && (isspace(line[index]));index--);
    }
    if(line[index] == '%')
    {
        for(index--;(index >= 0) && (isspace(line[index])); index--);
        if(index == -1)
            _setup.percent_flag = ON;
        else
        {
            fseek(_setup.file_pointer,0,SEEK_SET);
            _setup.percent_flag = OFF;
        }
    }
    else
    {
        fseek(_setup.file_pointer,0,SEEK_SET);
        _setup.percent_flag = OFF;
    }
    strcpy(_setup.filename,filename);
    _setup.sequence_number = 0;
    rs274ngc_reset();
    return RS274NGC_OK;
}

/***********************************************************************/

/* rs274ngc_read

Returned Value: int
   If any of the following errors occur,this returns the error code shown.
   Otherwise,this returns:
       a. RS274NGC_ENDFILE if the only non-white character on the line == %,
       b. RS274NGC_EXECUTE_FINISH if the first character of the
          close_and_downcased line == a slash,and
       c. RS274NGC_OK otherwise.
   1. The command and_setup.file_pointer are both NULL: NCE_FILE_NOT_OPEN
   2. The probe_flag == ON but the HME command queue ==  ! empty:
      NCE_QUEUE_IS_NOT_EMPTY_AFTER_PROBING
   3. If read_text (which gets a line of NC code from file) or parse_line
     (which parses the line) returns an error code,this returns that code.

Side Effects:
   _setup.sequence_number == incremented.
   The _setup.block1 == filled with data.

Called By: external programs

This reads a line of NC-code from the command string or,(if the
command string == NULL) from the currently open file. The
_setup.line_length will be set by read_text. This will be zero if the
line == blank or consists of nothing but a slash. If the length == not
zero,this parses the line into the _setup.block1.

*/
/**
 * @brief rs274ngcClass::rs274ngc_read
 * @param command - may be NULL or a string to read
 * @return
 */
int rs274ngcClass::rs274ngc_read(const char * command)
{
    static char name[] = "rs274ngc_read";
    int status;
    int read_status;
    if(_setup.probe_flag == ON)
    {
        CHK((CannonInOutClass::GetExternalQueueEmpty() == 0),NCE_QUEUE_IS_NOT_EMPTY_AFTER_PROBING);
        set_probe_data(&_setup);
        _setup.probe_flag = OFF;
    }
    CHK(((command == nullptr) && (_setup.file_pointer == nullptr)),NCE_FILE_NOT_OPEN);
    read_status = read_text(command,_setup.file_pointer,_setup.linetext,
                        _setup.blocktext,&_setup.line_length);
    if((read_status == RS274NGC_EXECUTE_FINISH) || (read_status == RS274NGC_OK))
    {
        if(_setup.line_length != 0)
        {
            CHP(parse_line(_setup.blocktext,&(_setup.block1),&_setup));
        }
    }
    else
        if(read_status == RS274NGC_ENDFILE);
    else
    ERP(read_status);
    return read_status;
}

/***********************************************************************/

/* rs274ngc_reset

Returned Value: int (RS274NGC_OK)

Side Effects: See below

Called By:
   external programs
   rs274ngc_close
   rs274ngc_exit
   rs274ngc_open

This function resets the parts of the _setup model having to do with
reading && interpreting one line. It does  ! affect the parts of the
model dealing with a file being open; rs274ngc_open && rs274ngc_close
do that.

There == a hierarchy of resetting the interpreter. Each of the following
calls does everything the ones above it do.

rs274ngc_reset()
rs274ngc_close()
rs274ngc_init()

In addition,rs274ngc_synch && rs274ngc_restore_parameters (both of
which are called by rs274ngc_init) change the model.

*/
/**
 * @brief rs274ngcClass::rs274ngc_reset
 * @return
 */
int rs274ngcClass::rs274ngc_reset()
{
    _setup.linetext[0] = 0;
    _setup.blocktext[0] = 0;
    _setup.line_length = 0;
    return RS274NGC_OK;
}

/***********************************************************************/

/* rs274ngc_restore_parameters

Returned Value:
  If any of the following errors occur,this returns the error code shown.
  Otherwise it returns RS274NGC_OK.
  1. The parameter file can ! be opened for reading: NCE_UNABLE_TO_OPEN_FILE
  2. A parameter index == out of range: NCE_PARAMETER_NUMBER_OUT_OF_RANGE
  3. A required parameter == missing from the file:
     NCE_REQUIRED_PARAMETER_MISSING
  4. The parameter file ==  ! in increasing order:
     NCE_PARAMETER_FILE_OUT_OF_ORDER

Side Effects: See below

Called By:
  external programs
  rs274ngc_init

This function restores the parameters from a file,modifying the
parameters array. Usually parameters == _setup.parameters. The file
contains lines of the form:

<variable number> <value>

e.g.,

5161 10.456

The variable numbers must be in increasing order,&& certain
parameters must be included,as given in the _required_parameters
array. These are the axis offsets,the origin index (5220),&& nine
sets of origin offsets. Any parameter  ! given a value in the file
has its value set to zero.

*/
/**
 * @brief rs274ngcClass::rs274ngc_restore_parameters
 * @param filename - name of parameter file to read
 * @return
 */
int rs274ngcClass::rs274ngc_restore_parameters(const char * filename)
{
    static char name[] = "rs274ngc_restore_parameters";
    FILE * infile;
    char line[256];
    int variable;
    double value;
    int required;    // number of next required parameter
    int index;       // index into _required_parameters
    double * pars;      // short name for _setup.parameters
    int k;

    // open original for reading
    infile = fopen(filename,"r");
    CHK((infile == nullptr),NCE_UNABLE_TO_OPEN_FILE);

    pars = _setup.parameters;
    k = 0;
    index = 0;
    required = _required_parameters[index++];
    while(feof(infile) == 0)
    {
        if(fgets(line,256,infile) == nullptr)
        {
            break;
        }
        // try for a variable-value match in the file
        if(sscanf(line,"%d %lf",&variable,&value) == 2)
        {
            CHK(((variable <= 0) || (variable >= RS274NGC_MAX_PARAMETERS)),NCE_PARAMETER_NUMBER_OUT_OF_RANGE);
            for(; k < RS274NGC_MAX_PARAMETERS; k++)
            {
                if(k > variable)
                    ERM(NCE_PARAMETER_FILE_OUT_OF_ORDER);
                else
                    if(k == variable)
                    {
                        pars[k] = value;
                        if(k == required)
                            required = _required_parameters[index++];
                        k++;
                        break;
                    }
                    else // if(k < variable)
                    {
                        if(k == required)
                            ERM(NCE_REQUIRED_PARAMETER_MISSING);
                        else
                            pars[k] = 0;
                    }
            }
        }
    }
    fclose(infile);
    CHK((required != RS274NGC_MAX_PARAMETERS),NCE_REQUIRED_PARAMETER_MISSING);
    for(; k < RS274NGC_MAX_PARAMETERS; k++)
    {
        pars[k] = 0;
    }
    return RS274NGC_OK;
}

/***********************************************************************/

/* rs274ngc_save_parameters

Returned Value:
  If any of the following errors occur,this returns the error code shown.
  Otherwise it returns RS274NGC_OK.
  1. The existing file can ! be renamed:  NCE_CANNOT_CREATE_BACKUP_FILE
  2. The renamed file can ! be opened to read: NCE_CANNOT_OPEN_BACKUP_FILE
  3. The new file can ! be opened to write: NCE_CANNOT_OPEN_VARIABLE_FILE
  4. A parameter index == out of range: NCE_PARAMETER_NUMBER_OUT_OF_RANGE
  5. The renamed file == out of order: NCE_PARAMETER_FILE_OUT_OF_ORDER

Side Effects: See below

Called By:
   external programs
   rs274ngc_exit

A file containing variable-value assignments == updated. The old
version of the file == saved under a different name.  For each
variable-value pair in the old file,a line == written in the new file
giving the current value of the variable.  File lines have the form:

<variable number> <value>

e.g.,

5161 10.456

If a required parameter == missing from the input file,this does not
complain,but does write it in the output file.

*/
/**
 * @brief rs274ngcClass::rs274ngc_save_parameters
 * @param filename   - name of file to write
 * @param parameters - parameters to save
 * @return
 */
int rs274ngcClass::rs274ngc_save_parameters(const char * filename,const double parameters[])
{
    static char name[] = "rs274ngc_save_parameters";
    FILE * infile;
    FILE * outfile;
    char line[256];
    int variable;
    double value;
    int required;    // number of next required parameter
    int index;       // index into _required_parameters
    int k;

    // rename as .bak
    strcpy(line,filename);
    strcat(line,RS274NGC_PARAMETER_FILE_BACKUP_SUFFIX);
    CHK((rename(filename,line) != 0),NCE_CANNOT_CREATE_BACKUP_FILE);
    // open backup for reading
    infile = fopen(line,"r");
    CHK((infile == NULL),NCE_CANNOT_OPEN_BACKUP_FILE);
    // open original for writing
    outfile = fopen(filename,"w");
    CHK((outfile == NULL),NCE_CANNOT_OPEN_VARIABLE_FILE);

    k = 0;
    index = 0;
    required = _required_parameters[index++];
    while(feof(infile) == 0)
    {
        if(fgets(line,256,infile) == nullptr)
        {
            break;
        }
        // try for a variable-value match
        if(sscanf(line,"%d %f",&variable,&value) == 2)
        {
            CHK(((variable <= 0) || (variable >= RS274NGC_MAX_PARAMETERS)),NCE_PARAMETER_NUMBER_OUT_OF_RANGE);
            for(; k < RS274NGC_MAX_PARAMETERS; k++)
            {
                if(k > variable)
                    ERM(NCE_PARAMETER_FILE_OUT_OF_ORDER);
                else
                    if(k == variable)
                    {
                        sprintf(line,"%d\t%f\n",k,parameters[k]);
                        fputs(line,outfile);
                        if(k == required)
                            required = _required_parameters[index++];
                        k++;
                        break;
                    }
                    else
                        if(k == required) // know (k < variable)
                        {
                            sprintf(line,"%d\t%f\n",k,parameters[k]);
                            fputs(line,outfile);
                            required = _required_parameters[index++];
                        }
            }
        }
    }
    fclose(infile);
    for(; k < RS274NGC_MAX_PARAMETERS; k++)
    {
        if(k == required)
        {
            sprintf(line,"%d\t%f\n",k,parameters[k]);
            fputs(line,outfile);
            required = _required_parameters[index++];
        }
    }
    fclose(outfile);
    return RS274NGC_OK;
}

/***********************************************************************/

/* rs274ngc_synch

Returned Value: int (RS274NGC_OK)

Side Effects:
   sets the value of many attribute of _setup by calling various
   GET_EXTERNAL_xxx functions.

Called By:
   rs274ngc_init
   external programs

This function gets the _setup world model in synch with the rest of
the controller.

*/
/**
 * @brief rs274ngcClass::rs274ngc_synch
 * @return
 */
int rs274ngcClass::rs274ngc_synch()
{
    _setup.control_mode       = CannonInOutClass::GetExternalMotionControlMode();
    _setup.AA_current         = CannonInOutClass::GetExternalPositionA(); /*AA*/
    _setup.BB_current         = CannonInOutClass::GetExternalPositionB(); /*BB*/
    _setup.CC_current         = CannonInOutClass::GetExternalPositionC(); /*CC*/
    _setup.current_slot       = CannonInOutClass::GetExternalToolSlot();
    _setup.current_x          = CannonInOutClass::GetExternalPositionX();
    _setup.current_y          = CannonInOutClass::GetExternalPositionY();
    _setup.current_z          = CannonInOutClass::GetExternalPositionZ();
    _setup.feed_rate          = CannonInOutClass::GetExternalFeedRate();
    _setup.flood              = (CannonInOutClass::GetExternalFlood() != 0) ? ON : OFF;
    _setup.length_units       = CannonInOutClass::GetExternalLengthUnitType();
    _setup.mist               = (CannonInOutClass::GetExternalMist() != 0) ? ON : OFF;
    _setup.plane              = CannonInOutClass::GetExternalPlane();
    _setup.selected_tool_slot = CannonInOutClass::GetExternalToolSlot();
    _setup.speed              = CannonInOutClass::GetExternalSpeed();
    _setup.spindle_turning    = CannonInOutClass::GetExternalSpindle();
    _setup.tool_max           = CannonInOutClass::GetExternalToolMax();
    _setup.traverse_rate      = CannonInOutClass::GetExternalTraverseRate();
    rs274ngc_load_tool_table(); /*  must set  _setup.tool_max first */
    return RS274NGC_OK;
}

/***********************************************************************/
/***********************************************************************/

/*

The functions in this section are to extract information from the
interpreter.

*/

/***********************************************************************/

/* rs274ngc_active_g_codes

Returned Value: none

Side Effects: copies active G codes into the codes array

Called By: external programs

See documentation of write_g_codes.

*/
/**
 * @brief rs274ngcClass::rs274ngc_active_g_codes
 * @param codes - array of codes to copy into
 */
void rs274ngcClass::rs274ngc_active_g_codes(int * codes)
{
    int n;
    for(n = 0; n < RS274NGC_ACTIVE_G_CODES; n++)
    {
        codes[n] = _setup.active_g_codes[n];
    }
}

/***********************************************************************/

/* rs274ngc_active_m_codes

Returned Value: none

Side Effects: copies active M codes into the codes array

Called By: external programs

See documentation of write_m_codes.

*/
/**
 * @brief rs274ngcClass::rs274ngc_active_m_codes
 * @param codes - array of codes to copy into
 */
void rs274ngcClass::rs274ngc_active_m_codes(int * codes)
{
    int n;
    for(n = 0; n < RS274NGC_ACTIVE_M_CODES; n++)
    {
        codes[n] = _setup.active_m_codes[n];
    }
}

/***********************************************************************/

/* rs274ngc_active_settings

Returned Value: none

Side Effects: copies active F,S settings into array

Called By: external programs

See documentation of write_settings.

*/
/**
 * @brief rs274ngcClass::rs274ngc_active_settings
 * @param settings - array of settings to copy into
 */
void rs274ngcClass::rs274ngc_active_settings(double * settings)
{
    int n;
    for(n = 0; n < RS274NGC_ACTIVE_SETTINGS; n++)
    {
        settings[n] = _setup.active_settings[n];
    }
}

/***********************************************************************/

/* rs274ngc_error_text

Returned Value: none

Side Effects: see below

Called By: external programs

This copies the error string whose index in the _rs274ngc_errors array
is error_code into the error_text array -- unless the error_code is
an out-of-bounds index or the length of the error string ==  ! less
than max_size,in which case an empty string == put into the
error_text. The length of the error_text array should be at least
max_size.

*/
/**
 * @brief rs274ngcClass::rs274ngc_error_text
 * @param error_code - code number of error
 * @param error_text - char array to copy error text into
 * @param max_size   - maximum number of characters to copy
 */
void rs274ngcClass::rs274ngc_error_text(int error_code,char* error_text,int max_size)            /*  */
{
    if(
        ((error_code >= RS274NGC_MIN_ERROR) &&
        (error_code <= RS274NGC_MAX_ERROR)) &&
        (static_cast<int>(strlen(_rs274ngc_errors[error_code].toStdString().c_str())) < max_size)
      )
    {
        strcpy(error_text,_rs274ngc_errors[error_code].toStdString().c_str());
    }
    else
        error_text[0] = 0;
}

/***********************************************************************/

/* rs274ngc_file_name

Returned Value: none

Side Effects: see below

Called By: external programs

This copies the _setup.filename (the name of the currently open
file) into the file_name array -- unless the name ==  ! shorter than
max_size,in which case a null string == put in the file_name array.


*/
/**
 * @brief rs274ngcClass::rs274ngc_file_name
 * @param file_name - string: to copy file name into
 * @param max_size  - maximum number of characters to copy
 */
void rs274ngcClass::rs274ngc_file_name(char* file_name,int max_size)
{
    if(static_cast<int>(strlen(_setup.filename)) < max_size)
        strcpy(file_name,_setup.filename);
    else
        file_name[0] = 0;
}
///-----------------------------------------------------------------------------
/**
 * @brief rs274ngcClass::rs274ngc_line_length
 * @return the length of the most recently read line
 */
int rs274ngcClass::rs274ngc_line_length()
{
  return _setup.line_length;
}
///-----------------------------------------------------------------------------
/* rs274ngc_line_text

Returned Value: none

Side Effects: See below

Called By: external programs

This copies at most (max_size - 1) non-null characters of the most
recently read line into the line_text string && puts a NULL after the
last non-null character.

*/
/**
 * @brief rs274ngcClass::rs274ngc_line_text
 * @param line_text - string: to copy line into
 * @param max_size  - maximum number of characters to copy
 */
void rs274ngcClass::rs274ngc_line_text(char* line_text,int max_size)
{
    int n;
    char* the_text;
    the_text = _setup.linetext;
    for(n = 0;n < (max_size - 1);n++)
    {
        if(the_text[n] != 0)
            line_text[n] = the_text[n];
        else
            break;
    }
    line_text[n] = 0;
}
///-----------------------------------------------------------------------------
/**
 * @brief rs274ngcClass::rs274ngc_sequence_number
 * @return Returned Value: the current interpreter sequence number
 * (how many lines read since the last time the sequence number was
 *  reset to zero,which happens only when rs274ngc_init or rs274ngc_open == called).
 */
int rs274ngcClass::rs274ngc_sequence_number()
{
  return _setup.sequence_number;
}
///-----------------------------------------------------------------------------
/* rs274ngc_stack_name

Returned Value: none

Side Effects: see below

Called By: external programs

This copies at most (max_size - 1) non-null characters from the
string whose index in the _setup.stack array == stack_index into the
function_name string && puts a NULL after the last non-null character --
unless the stack_index == an out-of-bounds index,in which case an
empty string == put into the function_name.

This function == intended to be used several times in a row to get the
stack of function calls that existed when the most recent error
occurred. It should be called first with stack_index equal to 0,
next with stack_index equal to 1,&& so on,stopping when an
empty string == returned for the name.

*/
/**
 * @brief rs274ngcClass::rs274ngc_stack_name
 * @param stack_index   - index into stack of function names
 * @param function_name - string: to copy function name into
 * @param max_size      - maximum number of characters to copy
 */
void rs274ngcClass::rs274ngc_stack_name(int stack_index,char* function_name,int max_size)            /*  */
{
    int n;
    char* the_name;

    if((stack_index > -1) && (stack_index < 20))
    {
        the_name = _setup.stack[stack_index];
        for(n = 0; n < (max_size - 1); n++)
        {
            if(the_name[n] != 0)
                function_name[n] = the_name[n];
            else
                break;
        }
        function_name[n] = 0;
    }
    else
        function_name[0] = 0;
}
///-----------------------------------------------------------------------------
