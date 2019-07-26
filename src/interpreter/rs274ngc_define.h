///-----------------------------------------------------------------------------
#ifndef RS274NGC_DEFINE_H
#define RS274NGC_DEFINE_H
///-----------------------------------------------------------------------------
#include <stdio.h>
#include "canon.h"
///-----------------------------------------------------------------------------
#ifndef MAX
    #define MAX(x, y)        ((x) > (y) ? (x) : (y))
#endif

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef TWO_PI
    #define TWO_PI  6.2831853071795864
#endif

#ifndef PI
    #define PI      3.1415926535897932
#endif

#ifndef PI2
    #define PI2     1.5707963267948966
#endif

/**
 * @brief RS274NGC_TEXT_SIZE
 * @brief TINY - for arc_data_r
 * @brief UNKNOWN
 * @brief RS274NGC_MAX_PARAMETERS - number of parameters in parameter table
 * @brief MAX_EMS - max number of m codes on one line
 */
const int    RS274NGC_TEXT_SIZE = 256;
const double TINY    = 1e-12;
const double UNKNOWN = 1e-20;
const int    RS274NGC_MAX_PARAMETERS = 5400;
const int    MAX_EMS = 4;

/// array sizes
const int RS274NGC_ACTIVE_G_CODES  = 12;
const int RS274NGC_ACTIVE_M_CODES  = 7;
const int RS274NGC_ACTIVE_SETTINGS = 3;

/// name of parameter file for saving/restoring interpreter variables
#define RS274NGC_PARAMETER_FILE_NAME_DEFAULT "rs274ngc.var"
#define RS274NGC_PARAMETER_FILE_BACKUP_SUFFIX ".bak"

/**
 * @brief MM_PER_INCH
 * @brief INCH_PER_MM
 * @note English - Metric conversion (long number keeps error buildup down)
 */
const double MM_PER_INCH = 25.4;
const double INCH_PER_MM = 0.039370078740157477;

/**
 * @brief OFF
 * @brief ON
 * @note on-off switch settings
 */
const int OFF = 0;
const int ON  = 1;

/**
 * @brief UNITS_PER_MINUTE
 * @brief INVERSE_TIME
 * @note feed_mode
 */
const int UNITS_PER_MINUTE = 0;
const int INVERSE_TIME     = 1;

/**
 * @brief RIGHT
 * @brief LEFT
 * @note cutter radius compensation mode, OFF already defined to 0
 *  not using CANON_SIDE since interpreter handles cutter radius comp
 */
const int RIGHT = 1;
const int LEFT  = 2;

/**
 * @note unary operations These are not enums because
 *  the "&" operator is used in reading the operation
 *  names and is illegal with an enum
 */
#define ABS 1
#define ACOS 2
#define ASIN 3
#define ATAN 4
#define COS 5
#define EXP 6
#define FIX 7
#define FUP 8
#define LN 9
#define ROUND 10
#define SIN 11
#define SQRT 12
#define TAN 13

/// binary operations
#define NO_OPERATION 0
#define DIVIDED_BY 1
#define MODULO 2
#define POWER 3
#define TIMES 4
#define AND2 5
#define EXCLUSIVE_OR 6
#define MINUS 7
#define NON_EXCLUSIVE_OR 8
#define PLUS 9
#define RIGHT_BRACKET 10
/// G Codes are symbolic to be dialect-independent in source code
#define G_0      0
#define G_1     10
#define G_2     20
#define G_3     30
#define G_4     40
#define G_10   100
#define G_17   170
#define G_18   180
#define G_19   190
#define G_20   200
#define G_21   210
#define G_28   280
#define G_30   300
#define G_38_2 382
#define G_40   400
#define G_41   410
#define G_42   420
#define G_43   430
#define G_49   490
#define G_53   530
#define G_54   540
#define G_55   550
#define G_56   560
#define G_57   570
#define G_58   580
#define G_59   590
#define G_59_1 591
#define G_59_2 592
#define G_59_3 593
#define G_61   610
#define G_61_1 611
#define G_64   640
#define G_80   800
#define G_81   810
#define G_82   820
#define G_83   830
#define G_84   840
#define G_85   850
#define G_86   860
#define G_87   870
#define G_88   880
#define G_89   890
#define G_90   900
#define G_91   910
#define G_92   920
#define G_92_1 921
#define G_92_2 922
#define G_92_3 923
#define G_93   930
#define G_94   940
#define G_98   980
#define G_99   990
///-----------------------------------------------------------------------------
/**
 * @note distance_mode
 */
typedef enum
{
    MODE_ABSOLUTE,
    MODE_INCREMENTAL
} DISTANCE_MODE;

/**
 * @note retract_mode for cycles
 */
typedef enum
{
    R_PLANE,
    OLD_Z
} RETRACT_MODE;

typedef int ON_OFF;

typedef struct block_struct
{
  ON_OFF   a_flag;
  double   a_number;
  ON_OFF   b_flag;
  double   b_number;
  ON_OFF   c_flag;
  double   c_number;
  char     comment[256];
  int      d_number;
  double   f_number;
  int      g_modes[14];
  int      h_number;
  ON_OFF   i_flag;
  double   i_number;
  ON_OFF   j_flag;
  double   j_number;
  ON_OFF   k_flag;
  double   k_number;
  int      l_number;
  int      line_number;
  int      motion_to_be;
  int      m_count;
  int      m_modes[10];
  double   p_number;
  double   q_number;
  ON_OFF   r_flag;
  double   r_number;
  double   s_number;
  int      t_number;
  ON_OFF   x_flag;
  double   x_number;
  ON_OFF   y_flag;
  double   y_number;
  ON_OFF   z_flag;
  double   z_number;
} block;

typedef block * block_pointer;
///-----------------------------------------------------------------------------
/// The current_x, current_y, and current_z are the location of the tool
/// in the current coordinate system. current_x and current_y differ from
/// program_x and program_y when cutter radius compensation is on.
/// current_z is the position of the tool tip in program coordinates when
/// tool length compensation is using the actual tool length; it is the
/// position of the spindle when tool length is zero.
/// In a setup, the axis_offset values are set by g92 and the origin_offset
/// values are set by g54 - g59.3. The net origin offset uses both values
/// and is not represented here
typedef struct setup_struct
{
    double AA_axis_offset;             // A-axis g92 offset
    double AA_current;                 // current A-axis position
    double AA_origin_offset;           // A-axis origin offset
    double BB_axis_offset;             // B-axis g92 offset
    double BB_current;                 // current B-axis position
    double BB_origin_offset;           // B-axis origin offset
    double CC_axis_offset;             // C-axis g92 offset
    double CC_current;                 // current C-axis position
    double CC_origin_offset;           // C-axis origin offset
    int active_g_codes[RS274NGC_ACTIVE_G_CODES];     // array of active G codes
    int active_m_codes[RS274NGC_ACTIVE_M_CODES];     // array of active M codes
    double active_settings[RS274NGC_ACTIVE_SETTINGS];    // array of feed, speed, etc.
    double axis_offset_x;              // X-axis g92 offset
    double axis_offset_y;              // Y-axis g92 offset
    double axis_offset_z;              // Z-axis g92 offset
    block block1;                      // parsed next block
    char blocktext[RS274NGC_TEXT_SIZE];// linetext downcased, white space gone
    CANON_MOTION_MODE control_mode;    // exact path or cutting mode
    int current_slot;                  // carousel slot number of current tool
    double current_x;                  // current X-axis position
    double current_y;                  // current Y-axis position
    double current_z;                  // current Z-axis position
    double cutter_comp_radius;         // current cutter compensation radius
    int cutter_comp_side;              // current cutter compensation side
    double cycle_cc;                   // cc-value (normal) for canned cycles
    double cycle_i;                    // i-value for canned cycles
    double cycle_j;                    // j-value for canned cycles
    double cycle_k;                    // k-value for canned cycles
    int cycle_l;                       // l-value for canned cycles
    double cycle_p;                    // p-value (dwell) for canned cycles
    double cycle_q;                    // q-value for canned cycles
    double cycle_r;                    // r-value for canned cycles
    DISTANCE_MODE distance_mode;       // absolute or incremental
    int feed_mode;                     // G_93 (inverse time) or G_94 units/min
    ON_OFF feed_override;              // whether feed override is enabled
    double feed_rate;                  // feed rate in current units/min
    char filename[RS274NGC_TEXT_SIZE]; // name of currently open NC code file
    FILE * file_pointer;               // file pointer for open NC code file
    ON_OFF flood;                      // whether flood coolant is on
    int length_offset_index;           // for use with tool length offsets
    CANON_UNITS length_units;          // millimeters or inches
    int line_length;                   // length of line last read
    char linetext[RS274NGC_TEXT_SIZE]; // text of most recent line read
    ON_OFF mist;                       // whether mist coolant is on
    int motion_mode;                   // active G-code for motion
    int origin_index;                  // active origin (1=G54 to 9=G59.3)
    double origin_offset_x;            // origin offset x
    double origin_offset_y;            // origin offset y
    double origin_offset_z;            // origin offset z
    double parameters[RS274NGC_MAX_PARAMETERS];     // system parameters
    int parameter_occurrence;          // parameter buffer index
    int parameter_numbers[50];         // parameter number buffer
    double parameter_values[50];       // parameter value buffer
    ON_OFF percent_flag;               // ON means first line was percent sign
    CANON_PLANE plane;                 // active plane, XY-, YZ-, or XZ-plane
    ON_OFF probe_flag;                 // flag indicating probing done
    double program_x;                  // program x, used when cutter comp on
    double program_y;                  // program y, used when cutter comp on
    RETRACT_MODE retract_mode;         // for cycles, old_z or r_plane
    int selected_tool_slot;            // tool slot selected but not active
    int sequence_number;               // sequence number of line last read
    double speed;                      // current spindle speed in rpm
    CANON_SPEED_FEED_MODE speed_feed_mode;   // independent or synched
    ON_OFF speed_override;             // whether speed override is enabled
    CANON_DIRECTION spindle_turning;   // direction spindle is turning
    char stack[50][80];                // stack of calls for error reporting
    int stack_index;                   // index into the stack
    double tool_length_offset;         // current tool length offset
    int tool_max;                      // highest number tool slot in carousel
    CANON_TOOL_TABLE tool_table[CANON_TOOL_MAX + 1];         // index is slot number
    int tool_table_index;              // tool index used with cutter comp
    double traverse_rate;              // rate for traverse motions
}setup;
typedef setup* setup_pointer;
/// pointer to function that reads
typedef int (*read_function_pointer) (char *,int *,block_pointer,double *);
///-----------------------------------------------------------------------------
#endif /// RS274NGC_DEFINE_H
///-----------------------------------------------------------------------------
