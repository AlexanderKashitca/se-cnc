///-----------------------------------------------------------------------------
#ifndef RS274NGC_RETURN_H
#define RS274NGC_RETURN_H
///-----------------------------------------------------------------------------
#include <QString>
///-----------------------------------------------------------------------------
static QString _rs274ngc_errors[] = {
/*   0 */ "No error",
/*   1 */ "No error",
/*   2 */ "No error",
/*   3 */ "No error",
/*   4 */ "A file is already open", // rs274ngc_open
/*   5 */ "All axes missing with g92", // enhance_block
/*   6 */ "All axes missing with motion code", // enhance_block
/*   7 */ "Arc radius too small to reach end point", // arc_data_r
/*   8 */ "Argument to acos out of range", // execute_unary
/*   9 */ "Argument to asin out of range", // execute_unary
/*  10 */ "Attempt to divide by zero", // execute_binary1
/*  11 */ "Attempt to raise negative to non integer power", // execute_binary1
/*  12 */ "Bad character used", // read_one_item
/*  13 */ "Bad format unsigned integer", // read_integer_unsigned
/*  14 */ "Bad number format", // read_real_number
/*  15 */ "Bug bad g code modal group 0", // check_g_codes
/*  16 */ "Bug code not g0 or g1", // convert_straight, convert_straight_comp1, convert_straight_comp2
/*  17 */ "Bug code not g17 g18 or g19", // convert_set_plane
/*  18 */ "Bug code not g20 or g21", // convert_length_units
/*  19 */ "Bug code not g28 or g30", // convert_home
/*  20 */ "Bug code not g2 or g3", // arc_data_comp_ijk, arc_data_ijk
/*  21 */ "Bug code not g40 g41 or g42", // convert_cutter_compensation
/*  22 */ "Bug code not g43 or g49", // convert_tool_length_offset
/*  23 */ "Bug code not g4 g10 g28 g30 g53 or g92 series", // convert_modal_0
/*  24 */ "Bug code not g61 g61 1 or g64", // convert_control_mode
/*  25 */ "Bug code not g90 or g91", // convert_distance_mode
/*  26 */ "Bug code not g93 or g94", // convert_feed_mode
/*  27 */ "Bug code not g98 or g99", // convert_retract_mode
/*  28 */ "Bug code not in g92 series", // convert_axis_offsets
/*  29 */ "Bug code not in range g54 to g593", // convert_coordinate_system
/*  30 */ "Bug code not m0 m1 m2 m30 m60", // convert_stop
/*  31 */ "Bug distance mode not g90 or g91", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  32 */ "Bug function should not have been called", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx, read_a, read_b, read_c, read_comment, read_d, read_f, read_g, read_h, read_i, read_j, read_k, read_l, read_line_number, read_m, read_p, read_parameter, read_parameter_setting, read_q, read_r, read_real_expression, read_s, read_t, read_x, read_y, read_z
/*  33 */ "Bug in tool radius comp", // arc_data_comp_r
/*  34 */ "Bug plane not xy yz or xz", // convert_arc, convert_cycle
/*  35 */ "Bug side not right or left", // convert_straight_comp1, convert_straight_comp2
/*  36 */ "Bug unknown motion code", // convert_motion
/*  37 */ "Bug unknown operation", // execute_binary1, execute_binary2, execute_unary
/*  38 */ "Cannot change axis offsets with cutter radius comp", // convert_axis_offsets
/*  39 */ "Cannot change units with cutter radius comp", // convert_length_units
/*  40 */ "Cannot create backup file", // rs274ngc_save_parameters
/*  41 */ "Cannot do g1 with zero feed rate", // convert_straight
/*  42 */ "Cannot do zero repeats of cycle", // convert_cycle
/*  43 */ "Cannot make arc with zero feed rate", // convert_arc
/*  44 */ "Cannot move rotary axes during probing", // convert_probe
/*  45 */ "Cannot open backup file", // rs274ngc_save_parameters
/*  46 */ "Cannot open variable file", // rs274ngc_save_parameters
/*  47 */ "Cannot probe in inverse time feed mode", // convert_probe
/*  48 */ "Cannot probe with cutter radius comp on", // convert_probe
/*  49 */ "Cannot probe with zero feed rate", // convert_probe
/*  50 */ "Cannot put a b in canned cycle", // check_other_codes
/*  51 */ "Cannot put a c in canned cycle", // check_other_codes
/*  52 */ "Cannot put an a in canned cycle", // check_other_codes
/*  53 */ "Cannot turn cutter radius comp on out of xy plane", // convert_cutter_compensation_on
/*  54 */ "Cannot turn cutter radius comp on when on", // convert_cutter_compensation_on
/*  55 */ "Cannot use a word", // read_a
/*  56 */ "Cannot use axis values with g80", // enhance_block
/*  57 */ "Cannot use axis values without a g code that uses them", // enhance_block
/*  58 */ "Cannot use b word", // read_b
/*  59 */ "Cannot use c word", // read_c
/*  60 */ "Cannot use g28 or g30 with cutter radius comp", // convert_home
/*  61 */ "Cannot use g53 incremental", // check_g_codes
/*  62 */ "Cannot use g53 with cutter radius comp", // convert_straight
/*  63 */ "Cannot use two g codes that both use axis values", // enhance_block
/*  64 */ "Cannot use xz plane with cutter radius comp", // convert_set_plane
/*  65 */ "Cannot use yz plane with cutter radius comp", // convert_set_plane
/*  66 */ "Command too long", // read_text, rs274ngc_open
/*  67 */ "Concave corner with cutter radius comp", // convert_arc_comp2, convert_straight_comp2
/*  68 */ "Coordinate system index parameter 5220 out of range", // rs274ngc_init
/*  69 */ "Current point same as end point of arc", // arc_data_r
/*  70 */ "Cutter gouging with cutter radius comp", // convert_arc_comp1, convert_straight_comp1
/*  71 */ "D word with no g41 or g42", // check_other_codes
/*  72 */ "Dwell time missing with g4", // check_g_codes
/*  73 */ "Dwell time p word missing with g82", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  74 */ "Dwell time p word missing with g86", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  75 */ "Dwell time p word missing with g88", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  76 */ "Dwell time p word missing with g89", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  77 */ "Equal sign missing in parameter setting", // read_parameter_setting
/*  78 */ "F word missing with inverse time arc move", // convert_arc
/*  79 */ "F word missing with inverse time g1 move", // convert_straight
/*  80 */ "File ended with no percent sign", // read_text, rs274ngc_open
/*  81 */ "File ended with no percent sign or program end", // read_text
/*  82 */ "File name too long", // rs274ngc_open
/*  83 */ "File not open", // rs274ngc_read
/*  84 */ "G code out of range", // read_g
/*  85 */ "H word with no g43", // check_other_codes
/*  86 */ "I word given for arc in yz plane", // convert_arc
/*  87 */ "I word missing with g87", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  88 */ "I word with no g2 or g3 or g87 to use it", // check_other_codes
/*  89 */ "J word given for arc in xz plane", // convert_arc
/*  90 */ "J word missing with g87", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  91 */ "J word with no g2 or g3 or g87 to use it", // check_other_codes
/*  92 */ "K word given for arc in xy plane", // convert_arc
/*  93 */ "K word missing with g87", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  94 */ "K word with no g2 or g3 or g87 to use it", // check_other_codes
/*  95 */ "L word with no canned cycle or g10", // check_other_codes
/*  96 */ "Left bracket missing after slash with atan", // read_atan
/*  97 */ "Left bracket missing after unary operation name", // read_unary
/*  98 */ "Line number greater than 99999", // read_line_number
/*  99 */ "Line with g10 does not have l2", // check_g_codes
/* 100 */ "M code greater than 99", // read_m
/* 101 */ "Mixed radius ijk format for arc", // convert_arc
/* 102 */ "Multiple a words on one line", // read_a
/* 103 */ "Multiple b words on one line", // read_b
/* 104 */ "Multiple c words on one line", // read_c
/* 105 */ "Multiple d words on one line", // read_d
/* 106 */ "Multiple f words on one line", // read_f
/* 107 */ "Multiple h words on one line", // read_h
/* 108 */ "Multiple i words on one line", // read_i
/* 109 */ "Multiple j words on one line", // read_j
/* 110 */ "Multiple k words on one line", // read_k
/* 111 */ "Multiple l words on one line", // read_l
/* 112 */ "Multiple p words on one line", // read_p
/* 113 */ "Multiple q words on one line", // read_q
/* 114 */ "Multiple r words on one line", // read_r
/* 115 */ "Multiple s words on one line", // read_s
/* 116 */ "Multiple t words on one line", // read_t
/* 117 */ "Multiple x words on one line", // read_x
/* 118 */ "Multiple y words on one line", // read_y
/* 119 */ "Multiple z words on one line", // read_z
/* 120 */ "Must use g0 or g1 with g53", // check_g_codes
/* 121 */ "Negative argument to sqrt", // execute_unary
/* 122 */ "Negative d word tool radius index used", // read_d
/* 123 */ "Negative f word used", // read_f
/* 124 */ "Negative g code used", // read_g
/* 125 */ "Negative h word tool length offset index used", // read_h
/* 126 */ "Negative l word used", // read_l
/* 127 */ "Negative m code used", // read_m
/* 128 */ "Negative or zero q value used", // read_q
/* 129 */ "Negative p word used", // read_p
/* 130 */ "Negative spindle speed used", // read_s
/* 131 */ "Negative tool id used", // read_t
/* 132 */ "Nested comment found", // close_and_downcase
/* 133 */ "No characters found in reading real value", // read_real_value
/* 134 */ "No digits found where real number should be", // read_real_number
/* 135 */ "Non integer value for integer", // read_integer_value
/* 136 */ "Null missing after newline", // close_and_downcase
/* 137 */ "Offset index missing", // convert_tool_length_offset
/* 138 */ "P value not an integer with g10 l2", // check_g_codes
/* 139 */ "P value out of range with g10 l2", // check_g_codes
/* 140 */ "P word with no g4 g10 g82 g86 g88 g89", // check_other_codes
/* 141 */ "Parameter file out of order", // rs274ngc_restore_parameters, rs274ngc_save_parameters
/* 142 */ "Parameter number out of range", // read_parameter, read_parameter_setting, rs274ngc_restore_parameters, rs274ngc_save_parameters
/* 143 */ "Q word missing with g83", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/* 144 */ "Q word with no g83", // check_other_codes
/* 145 */ "Queue is not empty after probing", // rs274ngc_read
/* 146 */ "R clearance plane unspecified in cycle", // convert_cycle
/* 147 */ "R i j k words all missing for arc", // convert_arc
/* 148 */ "R less than x in cycle in yz plane", // convert_cycle_yz
/* 149 */ "R less than y in cycle in xz plane", // convert_cycle_zx
/* 150 */ "R less than z in cycle in xy plane", // convert_cycle_xy
/* 151 */ "R word with no g code that uses it", // check_other_codes
/* 152 */ "Radius to end of arc differs from radius to start", // arc_data_comp_ijk, arc_data_ijk
/* 153 */ "Radius too small to reach end point", // arc_data_comp_r
/* 154 */ "Required parameter missing", // rs274ngc_restore_parameters
/* 155 */ "Selected tool slot number too large", // convert_tool_select
/* 156 */ "Slash missing after first atan argument", // read_atan
/* 157 */ "Spindle not turning clockwise in g84", // convert_cycle_g84
/* 158 */ "Spindle not turning in g86", // convert_cycle_g86
/* 159 */ "Spindle not turning in g87", // convert_cycle_g87
/* 160 */ "Spindle not turning in g88", // convert_cycle_g88
/* 161 */ "Sscanf failed", // read_integer_unsigned, read_real_number
/* 162 */ "Start point too close to probe point", // convert_probe
/* 163 */ "Too many m codes on line", // check_m_codes
/* 164 */ "Tool length offset index too big", // read_h
/* 165 */ "Tool max too large", // rs274ngc_load_tool_table
/* 166 */ "Tool radius index too big", // read_d
/* 167 */ "Tool radius not less than arc radius with comp", // arc_data_comp_r, convert_arc_comp2
/* 168 */ "Two g codes used from same modal group", // read_g
/* 169 */ "Two m codes used from same modal group", // read_m
/* 170 */ "Unable to open file", // convert_stop, rs274ngc_open, rs274ngc_restore_parameters
/* 171 */ "Unclosed comment found", // close_and_downcase
/* 172 */ "Unclosed expression", // read_operation
/* 173 */ "Unknown g code used", // read_g
/* 174 */ "Unknown m code used", // read_m
/* 175 */ "Unknown operation", // read_operation
/* 176 */ "Unknown operation name starting with a", // read_operation
/* 177 */ "Unknown operation name starting with m", // read_operation
/* 178 */ "Unknown operation name starting with o", // read_operation
/* 179 */ "Unknown operation name starting with x", // read_operation
/* 180 */ "Unknown word starting with a", // read_operation_unary
/* 181 */ "Unknown word starting with c", // read_operation_unary
/* 182 */ "Unknown word starting with e", // read_operation_unary
/* 183 */ "Unknown word starting with f", // read_operation_unary
/* 184 */ "Unknown word starting with l", // read_operation_unary
/* 185 */ "Unknown word starting with r", // read_operation_unary
/* 186 */ "Unknown word starting with s", // read_operation_unary
/* 187 */ "Unknown word starting with t", // read_operation_unary
/* 188 */ "Unknown word where unary operation could be", // read_operation_unary
/* 189 */ "X and y words missing for arc in xy plane", // convert_arc
/* 190 */ "X and z words missing for arc in xz plane", // convert_arc
/* 191 */ "X value unspecified in yz plane canned cycle", // convert_cycle_yz
/* 192 */ "X y and z words all missing with g38 2", // convert_probe
/* 193 */ "Y and z words missing for arc in yz plane", // convert_arc
/* 194 */ "Y value unspecified in xz plane canned cycle", // convert_cycle_zx
/* 195 */ "Z value unspecified in xy plane canned cycle", // convert_cycle_xy
/* 196 */ "Zero or negative argument to ln", // execute_unary
/* 197 */ "Zero radius arc", // arc_data_ijk
          "The End"
};
///-----------------------------------------------------------------------------
enum
{
    RS274NGC_OK                                               =  0,
    RS274NGC_EXIT                                             =  1,
    RS274NGC_EXECUTE_FINISH                                   =  2,
    RS274NGC_ENDFILE                                          =  3,
    NCE_A_FILE_IS_ALREADY_OPEN                                =  4,
    NCE_ALL_AXES_MISSING_WITH_G92                             =  5,
    NCE_ALL_AXES_MISSING_WITH_MOTION_CODE                     =  6,
    NCE_ARC_RADIUS_TOO_SMALL_TO_REACH_END_POINT               =  7,
    NCE_ARGUMENT_TO_ACOS_OUT_OF_RANGE                         =  8,
    NCE_ARGUMENT_TO_ASIN_OUT_OF_RANGE                         =  9,
    NCE_ATTEMPT_TO_DIVIDE_BY_ZERO                             =  10,
    NCE_ATTEMPT_TO_RAISE_NEGATIVE_TO_NON_INTEGER_POWER        =  11,
    NCE_BAD_CHARACTER_USED                                    =  12,
    NCE_BAD_FORMAT_UNSIGNED_INTEGER                           =  13,
    NCE_BAD_NUMBER_FORMAT                                     =  14,
    NCE_BUG_BAD_G_CODE_MODAL_GROUP_0                          =  15,
    NCE_BUG_CODE_NOT_G0_OR_G1                                 =  16,
    NCE_BUG_CODE_NOT_G17_G18_OR_G19                           =  17,
    NCE_BUG_CODE_NOT_G20_OR_G21                               =  18,
    NCE_BUG_CODE_NOT_G28_OR_G30                               =  19,
    NCE_BUG_CODE_NOT_G2_OR_G3                                 =  20,
    NCE_BUG_CODE_NOT_G40_G41_OR_G42                           =  21,
    NCE_BUG_CODE_NOT_G43_OR_G49                               =  22,
    NCE_BUG_CODE_NOT_G4_G10_G28_G30_G53_OR_G92_SERIES         =  23,
    NCE_BUG_CODE_NOT_G61_G61_1_OR_G64                         =  24,
    NCE_BUG_CODE_NOT_G90_OR_G91                               =  25,
    NCE_BUG_CODE_NOT_G93_OR_G94                               =  26,
    NCE_BUG_CODE_NOT_G98_OR_G99                               =  27,
    NCE_BUG_CODE_NOT_IN_G92_SERIES                            =  28,
    NCE_BUG_CODE_NOT_IN_RANGE_G54_TO_G593                     =  29,
    NCE_BUG_CODE_NOT_M0_M1_M2_M30_M60                         =  30,
    NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91                      =  31,
    NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED              =  32,
    NCE_BUG_IN_TOOL_RADIUS_COMP                               =  33,
    NCE_BUG_PLANE_NOT_XY_YZ_OR_XZ                             =  34,
    NCE_BUG_SIDE_NOT_RIGHT_OR_LEFT                            =  35,
    NCE_BUG_UNKNOWN_MOTION_CODE                               =  36,
    NCE_BUG_UNKNOWN_OPERATION                                 =  37,
    NCE_CANNOT_CHANGE_AXIS_OFFSETS_WITH_CUTTER_RADIUS_COMP    =  38,
    NCE_CANNOT_CHANGE_UNITS_WITH_CUTTER_RADIUS_COMP           =  39,
    NCE_CANNOT_CREATE_BACKUP_FILE                             =  40,
    NCE_CANNOT_DO_G1_WITH_ZERO_FEED_RATE                      =  41,
    NCE_CANNOT_DO_ZERO_REPEATS_OF_CYCLE                       =  42,
    NCE_CANNOT_MAKE_ARC_WITH_ZERO_FEED_RATE                   =  43,
    NCE_CANNOT_MOVE_ROTARY_AXES_DURING_PROBING                =  44,
    NCE_CANNOT_OPEN_BACKUP_FILE                               =  45,
    NCE_CANNOT_OPEN_VARIABLE_FILE                             =  46,
    NCE_CANNOT_PROBE_IN_INVERSE_TIME_FEED_MODE                =  47,
    NCE_CANNOT_PROBE_WITH_CUTTER_RADIUS_COMP_ON               =  48,
    NCE_CANNOT_PROBE_WITH_ZERO_FEED_RATE                      =  49,
    NCE_CANNOT_PUT_A_B_IN_CANNED_CYCLE                        =  50,
    NCE_CANNOT_PUT_A_C_IN_CANNED_CYCLE                        =  51,
    NCE_CANNOT_PUT_AN_A_IN_CANNED_CYCLE                       =  52,
    NCE_CANNOT_TURN_CUTTER_RADIUS_COMP_ON_OUT_OF_XY_PLANE     =  53,
    NCE_CANNOT_TURN_CUTTER_RADIUS_COMP_ON_WHEN_ON             =  54,
    NCE_CANNOT_USE_A_WORD                                     =  55,
    NCE_CANNOT_USE_AXIS_VALUES_WITH_G80                       =  56,
    NCE_CANNOT_USE_AXIS_VALUES_WITHOUT_A_G_CODE_THAT_USES_THEM=  57,
    NCE_CANNOT_USE_B_WORD                                     =  58,
    NCE_CANNOT_USE_C_WORD                                     =  59,
    NCE_CANNOT_USE_G28_OR_G30_WITH_CUTTER_RADIUS_COMP         =  60,
    NCE_CANNOT_USE_G53_INCREMENTAL                            =  61,
    NCE_CANNOT_USE_G53_WITH_CUTTER_RADIUS_COMP                =  62,
    NCE_CANNOT_USE_TWO_G_CODES_THAT_BOTH_USE_AXIS_VALUES      =  63,
    NCE_CANNOT_USE_XZ_PLANE_WITH_CUTTER_RADIUS_COMP           =  64,
    NCE_CANNOT_USE_YZ_PLANE_WITH_CUTTER_RADIUS_COMP           =  65,
    NCE_COMMAND_TOO_LONG                                      =  66,
    NCE_CONCAVE_CORNER_WITH_CUTTER_RADIUS_COMP                =  67,
    NCE_COORDINATE_SYSTEM_INDEX_PARAMETER_5220_OUT_OF_RANGE   =  68,
    NCE_CURRENT_POINT_SAME_AS_END_POINT_OF_ARC                =  69,
    NCE_CUTTER_GOUGING_WITH_CUTTER_RADIUS_COMP                =  70,
    NCE_D_WORD_WITH_NO_G41_OR_G42                             =  71,
    NCE_DWELL_TIME_MISSING_WITH_G4                            =  72,
    NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82                    =  73,
    NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86                    =  74,
    NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88                    =  75,
    NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89                    =  76,
    NCE_EQUAL_SIGN_MISSING_IN_PARAMETER_SETTING               =  77,
    NCE_F_WORD_MISSING_WITH_INVERSE_TIME_ARC_MOVE             =  78,
    NCE_F_WORD_MISSING_WITH_INVERSE_TIME_G1_MOVE              =  79,
    NCE_FILE_ENDED_WITH_NO_PERCENT_SIGN                       =  80,
    NCE_FILE_ENDED_WITH_NO_PERCENT_SIGN_OR_PROGRAM_END        =  81,
    NCE_FILE_NAME_TOO_LONG                                    =  82,
    NCE_FILE_NOT_OPEN                                         =  83,
    NCE_G_CODE_OUT_OF_RANGE                                   =  84,
    NCE_H_WORD_WITH_NO_G43                                    =  85,
    NCE_I_WORD_GIVEN_FOR_ARC_IN_YZ_PLANE                      =  86,
    NCE_I_WORD_MISSING_WITH_G87                               =  87,
    NCE_I_WORD_WITH_NO_G2_OR_G3_OR_G87_TO_USE_IT              =  88,
    NCE_J_WORD_GIVEN_FOR_ARC_IN_XZ_PLANE                      =  89,
    NCE_J_WORD_MISSING_WITH_G87                               =  90,
    NCE_J_WORD_WITH_NO_G2_OR_G3_OR_G87_TO_USE_IT              =  91,
    NCE_K_WORD_GIVEN_FOR_ARC_IN_XY_PLANE                      =  92,
    NCE_K_WORD_MISSING_WITH_G87                               =  93,
    NCE_K_WORD_WITH_NO_G2_OR_G3_OR_G87_TO_USE_IT              =  94,
    NCE_L_WORD_WITH_NO_CANNED_CYCLE_OR_G10                    =  95,
    NCE_LEFT_BRACKET_MISSING_AFTER_SLASH_WITH_ATAN            =  96,
    NCE_LEFT_BRACKET_MISSING_AFTER_UNARY_OPERATION_NAME       =  97,
    NCE_LINE_NUMBER_GREATER_THAN_99999                        =  98,
    NCE_LINE_WITH_G10_DOES_NOT_HAVE_L2                        =  99,
    NCE_M_CODE_GREATER_THAN_99                                =  100,
    NCE_MIXED_RADIUS_IJK_FORMAT_FOR_ARC                       =  101,
    NCE_MULTIPLE_A_WORDS_ON_ONE_LINE                          =  102,
    NCE_MULTIPLE_B_WORDS_ON_ONE_LINE                          =  103,
    NCE_MULTIPLE_C_WORDS_ON_ONE_LINE                          =  104,
    NCE_MULTIPLE_D_WORDS_ON_ONE_LINE                          =  105,
    NCE_MULTIPLE_F_WORDS_ON_ONE_LINE                          =  106,
    NCE_MULTIPLE_H_WORDS_ON_ONE_LINE                          =  107,
    NCE_MULTIPLE_I_WORDS_ON_ONE_LINE                          =  108,
    NCE_MULTIPLE_J_WORDS_ON_ONE_LINE                          =  109,
    NCE_MULTIPLE_K_WORDS_ON_ONE_LINE                          =  110,
    NCE_MULTIPLE_L_WORDS_ON_ONE_LINE                          =  111,
    NCE_MULTIPLE_P_WORDS_ON_ONE_LINE                          =  112,
    NCE_MULTIPLE_Q_WORDS_ON_ONE_LINE                          =  113,
    NCE_MULTIPLE_R_WORDS_ON_ONE_LINE                          =  114,
    NCE_MULTIPLE_S_WORDS_ON_ONE_LINE                          =  115,
    NCE_MULTIPLE_T_WORDS_ON_ONE_LINE                          =  116,
    NCE_MULTIPLE_X_WORDS_ON_ONE_LINE                          =  117,
    NCE_MULTIPLE_Y_WORDS_ON_ONE_LINE                          =  118,
    NCE_MULTIPLE_Z_WORDS_ON_ONE_LINE                          =  119,
    NCE_MUST_USE_G0_OR_G1_WITH_G53                            =  120,
    NCE_NEGATIVE_ARGUMENT_TO_SQRT                             =  121,
    NCE_NEGATIVE_D_WORD_TOOL_RADIUS_INDEX_USED                =  122,
    NCE_NEGATIVE_F_WORD_USED                                  =  123,
    NCE_NEGATIVE_G_CODE_USED                                  =  124,
    NCE_NEGATIVE_H_WORD_TOOL_LENGTH_OFFSET_INDEX_USED         =  125,
    NCE_NEGATIVE_L_WORD_USED                                  =  126,
    NCE_NEGATIVE_M_CODE_USED                                  =  127,
    NCE_NEGATIVE_OR_ZERO_Q_VALUE_USED                         =  128,
    NCE_NEGATIVE_P_WORD_USED                                  =  129,
    NCE_NEGATIVE_SPINDLE_SPEED_USED                           =  130,
    NCE_NEGATIVE_TOOL_ID_USED                                 =  131,
    NCE_NESTED_COMMENT_FOUND                                  =  132,
    NCE_NO_CHARACTERS_FOUND_IN_READING_REAL_VALUE             =  133,
    NCE_NO_DIGITS_FOUND_WHERE_REAL_NUMBER_SHOULD_BE           =  134,
    NCE_NON_INTEGER_VALUE_FOR_INTEGER                         =  135,
    NCE_NULL_MISSING_AFTER_NEWLINE                            =  136,
    NCE_OFFSET_INDEX_MISSING                                  =  137,
    NCE_P_VALUE_NOT_AN_INTEGER_WITH_G10_L2                    =  138,
    NCE_P_VALUE_OUT_OF_RANGE_WITH_G10_L2                      =  139,
    NCE_P_WORD_WITH_NO_G4_G10_G82_G86_G88_G89                 =  140,
    NCE_PARAMETER_FILE_OUT_OF_ORDER                           =  141,
    NCE_PARAMETER_NUMBER_OUT_OF_RANGE                         =  142,
    NCE_Q_WORD_MISSING_WITH_G83                               =  143,
    NCE_Q_WORD_WITH_NO_G83                                    =  144,
    NCE_QUEUE_IS_NOT_EMPTY_AFTER_PROBING                      =  145,
    NCE_R_CLEARANCE_PLANE_UNSPECIFIED_IN_CYCLE                =  146,
    NCE_R_I_J_K_WORDS_ALL_MISSING_FOR_ARC                     =  147,
    NCE_R_LESS_THAN_X_IN_CYCLE_IN_YZ_PLANE                    =  148,
    NCE_R_LESS_THAN_Y_IN_CYCLE_IN_XZ_PLANE                    =  149,
    NCE_R_LESS_THAN_Z_IN_CYCLE_IN_XY_PLANE                    =  150,
    NCE_R_WORD_WITH_NO_G_CODE_THAT_USES_IT                    =  151,
    NCE_RADIUS_TO_END_OF_ARC_DIFFERS_FROM_RADIUS_TO_START     =  152,
    NCE_RADIUS_TOO_SMALL_TO_REACH_END_POINT                   =  153,
    NCE_REQUIRED_PARAMETER_MISSING                            =  154,
    NCE_SELECTED_TOOL_SLOT_NUMBER_TOO_LARGE                   =  155,
    NCE_SLASH_MISSING_AFTER_FIRST_ATAN_ARGUMENT               =  156,
    NCE_SPINDLE_NOT_TURNING_CLOCKWISE_IN_G84                  =  157,
    NCE_SPINDLE_NOT_TURNING_IN_G86                            =  158,
    NCE_SPINDLE_NOT_TURNING_IN_G87                            =  159,
    NCE_SPINDLE_NOT_TURNING_IN_G88                            =  160,
    NCE_SSCANF_FAILED                                         =  161,
    NCE_START_POINT_TOO_CLOSE_TO_PROBE_POINT                  =  162,
    NCE_TOO_MANY_M_CODES_ON_LINE                              =  163,
    NCE_TOOL_LENGTH_OFFSET_INDEX_TOO_BIG                      =  164,
    NCE_TOOL_MAX_TOO_LARGE                                    =  165,
    NCE_TOOL_RADIUS_INDEX_TOO_BIG                             =  166,
    NCE_TOOL_RADIUS_NOT_LESS_THAN_ARC_RADIUS_WITH_COMP        =  167,
    NCE_TWO_G_CODES_USED_FROM_SAME_MODAL_GROUP                =  168,
    NCE_TWO_M_CODES_USED_FROM_SAME_MODAL_GROUP                =  169,
    NCE_UNABLE_TO_OPEN_FILE                                   =  170,
    NCE_UNCLOSED_COMMENT_FOUND                                =  171,
    NCE_UNCLOSED_EXPRESSION                                   =  172,
    NCE_UNKNOWN_G_CODE_USED                                   =  173,
    NCE_UNKNOWN_M_CODE_USED                                   =  174,
    NCE_UNKNOWN_OPERATION                                     =  175,
    NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_A                =  176,
    NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_M                =  177,
    NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_O                =  178,
    NCE_UNKNOWN_OPERATION_NAME_STARTING_WITH_X                =  179,
    NCE_UNKNOWN_WORD_STARTING_WITH_A                          =  180,
    NCE_UNKNOWN_WORD_STARTING_WITH_C                          =  181,
    NCE_UNKNOWN_WORD_STARTING_WITH_E                          =  182,
    NCE_UNKNOWN_WORD_STARTING_WITH_F                          =  183,
    NCE_UNKNOWN_WORD_STARTING_WITH_L                          =  184,
    NCE_UNKNOWN_WORD_STARTING_WITH_R                          =  185,
    NCE_UNKNOWN_WORD_STARTING_WITH_S                          =  186,
    NCE_UNKNOWN_WORD_STARTING_WITH_T                          =  187,
    NCE_UNKNOWN_WORD_WHERE_UNARY_OPERATION_COULD_BE           =  188,
    NCE_X_AND_Y_WORDS_MISSING_FOR_ARC_IN_XY_PLANE             =  189,
    NCE_X_AND_Z_WORDS_MISSING_FOR_ARC_IN_XZ_PLANE             =  190,
    NCE_X_VALUE_UNSPECIFIED_IN_YZ_PLANE_CANNED_CYCLE          =  191,
    NCE_X_Y_AND_Z_WORDS_ALL_MISSING_WITH_G38_2                =  192,
    NCE_Y_AND_Z_WORDS_MISSING_FOR_ARC_IN_YZ_PLANE             =  193,
    NCE_Y_VALUE_UNSPECIFIED_IN_XZ_PLANE_CANNED_CYCLE          =  194,
    NCE_Z_VALUE_UNSPECIFIED_IN_XY_PLANE_CANNED_CYCLE          =  195,
    NCE_ZERO_OR_NEGATIVE_ARGUMENT_TO_LN                       =  196,
    NCE_ZERO_RADIUS_ARC                                       =  197
};
///-----------------------------------------------------------------------------
enum
{
    RS274NGC_MIN_ERROR = 3,
    RS274NGC_MAX_ERROR = 197
};
///-----------------------------------------------------------------------------
#endif /// RS274NGC_RETURN_H
///-----------------------------------------------------------------------------
