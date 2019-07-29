///-----------------------------------------------------------------------------
#ifndef RS274NGC_H
#define RS274NGC_H
///-----------------------------------------------------------------------------
#include "rs274ngc_define.h"
#include "rs274ngc_return.h"

#include "cannon_in_out.h"


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define DEBUG_EMC
///-----------------------------------------------------------------------------
class rs274ngcClass
{
    public:
        rs274ngcClass();
        virtual ~rs274ngcClass();

        static void SetTolerance(double tolerance);
    private:

        static double TOLERANCE_INCH;
        static double TOLERANCE_MM;
        static double TOLERANCE_CONCAVE_CORNER;

        /* There are four global variables. The first three are _gees, _ems,
        and _readers. The last one, declared here, == for interpreter settings */

        static const int _gees[];
        static const int _ems[];
        static const read_function_pointer _readers[];
        static const int _required_parameters[];

        static int arc_data_comp_ijk(int move,int side,double tool_radius,double current_x,double current_y, double end_x, double end_y,          double i_number,double j_number,double* center_x,double* center_y,int* turn,double tolerance);
        static int arc_data_comp_r(int move,int side,double tool_radius,double current_x,double current_y, double end_x, double end_y,          double big_radius,double* center_x,double* center_y,int* turn);
        static int arc_data_ijk(int move,double current_x,double current_y,double end_x,double end_y, double i_number, double j_number,          double * center_x,double* center_y,int* turn,double tolerance);
        static int arc_data_r(int move,double current_x, double current_y,double end_x,double end_y, double radius, double * center_x,          double * center_y,int* turn);
        static int check_g_codes(block_pointer block,setup_pointer settings);
        static int check_items(block_pointer block,setup_pointer settings);
        static int check_m_codes(block_pointer block);
        static int check_other_codes(block_pointer block);
        static int close_and_downcase(char* line);
        static int convert_arc(int move,block_pointer block,setup_pointer settings);
        static int convert_arc2(int move,block_pointer block,setup_pointer settings,double* current1,double* current2,double* current3,double end1,double end2,double end3,double AA_end,double BB_end,double CC_end,double offset1,double offset2);
        static int convert_arc_comp1(int move,block_pointer block,setup_pointer settings,double end_x,double end_y,double end_z,double AA_end,double BB_end,double CC_end);
        static int convert_arc_comp2(int move,block_pointer block,setup_pointer settings,double end_x,double end_y,double end_z,double AA_end,double BB_end,double CC_end);
        static int convert_axis_offsets(int g_code,block_pointer block,setup_pointer settings);
        static int convert_comment(char* comment);
        static int convert_control_mode(int g_code,setup_pointer settings);
        static int convert_coordinate_system(int g_code,setup_pointer settings);
        static int convert_cutter_compensation(int g_code,block_pointer block,setup_pointer settings);
        static int convert_cutter_compensation_off(setup_pointer settings);
        static int convert_cutter_compensation_on(int side,block_pointer block,setup_pointer settings);
        static int convert_cycle(int motion,block_pointer block,setup_pointer settings);
        static int convert_cycle_g81(CANON_PLANE plane,double x,double y,double clear_z,double bottom_z);
        static int convert_cycle_g82(CANON_PLANE plane,double x,double y,double clear_z,double bottom_z,double dwell);
        static int convert_cycle_g83(CANON_PLANE plane,double x,double y,double r,double clear_z,double bottom_z,double delta);
        static int convert_cycle_g84(CANON_PLANE plane,double x,double y,double clear_z,double bottom_z,CANON_DIRECTION direction,CANON_SPEED_FEED_MODE mode);
        static int convert_cycle_g85(CANON_PLANE plane,double x,double y,double clear_z,double bottom_z);
        static int convert_cycle_g86(CANON_PLANE plane,double x,double y,double clear_z,double bottom_z,double dwell,CANON_DIRECTION direction);
        static int convert_cycle_g87(CANON_PLANE plane,double x,double offset_x,double y,double offset_y,double r,double clear_z,double middle_z,double bottom_z,CANON_DIRECTION direction);
        static int convert_cycle_g88(CANON_PLANE plane,double x,double y,double bottom_z,double dwell,CANON_DIRECTION direction);
        static int convert_cycle_g89(CANON_PLANE plane,double x,double y,double clear_z,double bottom_z,double dwell);
        static int convert_cycle_xy(int motion,block_pointer block,setup_pointer settings);
        static int convert_cycle_yz(int motion,block_pointer block,setup_pointer settings);
        static int convert_cycle_zx(int motion,block_pointer block,setup_pointer settings);
        static int convert_distance_mode(int g_code,setup_pointer settings);
        static int convert_dwell(double time);
        static int convert_feed_mode(int g_code,setup_pointer settings);
        static int convert_feed_rate(block_pointer block,setup_pointer settings);
        static int convert_g(block_pointer block,setup_pointer settings);
        static int convert_home(int move,block_pointer block,setup_pointer settings);
        static int convert_length_units(int g_code,setup_pointer settings);
        static int convert_m(block_pointer block, setup_pointer settings);
        static int convert_modal_0(int code,block_pointer block,setup_pointer settings);
        static int convert_motion(int motion,block_pointer block,setup_pointer settings);
        static int convert_probe(block_pointer block,setup_pointer settings);
        static int convert_retract_mode(int g_code,setup_pointer settings);
        static int convert_setup(block_pointer block,setup_pointer settings);
        static int convert_set_plane(int g_code,setup_pointer settings);
        static int convert_speed(block_pointer block,setup_pointer settings);
        static int convert_stop(block_pointer block,setup_pointer settings);
        static int convert_straight(int move,block_pointer block,setup_pointer settings);
        static int convert_straight_comp1(int move,block_pointer block,setup_pointer settings,double px,double py,double end_z,double AA_end,double BB_end,double CC_end);
        static int convert_straight_comp2(int move,block_pointer block,setup_pointer settings,double px,double py,double end_z,double AA_end,double BB_end,double CC_end);
        static int convert_tool_change(setup_pointer settings);
        static int convert_tool_length_offset(int g_code,block_pointer block,setup_pointer settings);
        static int convert_tool_select(block_pointer block,setup_pointer settings);
        static int cycle_feed(CANON_PLANE plane,double end1,double end2,double end3);
        static int cycle_traverse(CANON_PLANE plane,double end1,double end2,double end3);
        static int enhance_block(block_pointer block,setup_pointer settings);
        static int execute_binary(double* left,int operation,double* right);
        static int execute_binary1(double* left,int operation,double* right);
        static int execute_binary2(double* left,int operation,double* right);
        static int execute_block(block_pointer block,setup_pointer settings);
        static int execute_unary(double* double_ptr,int operation);
        static double find_arc_length(double x1,double y1, double z1,double center_x,double center_y,int turn,double x2,double y2,double z2);
        static int    find_ends(block_pointer block,setup_pointer settings,double* px,double* py,double* pz,double* AA_p,double* BB_p,double* CC_p);
        static int    find_relative(double x1,double y1,double z1,double AA_1, double BB_1, double CC_1,double* x2,double* y2,double* z2,double* AA_2,double * BB_2,double * CC_2,setup_pointer settings);
        static double find_straight_length(double x2,double y2,double z2,double AA_2,double BB_2,double CC_2,double x1,double y1,double z1,double AA_1,double BB_1,double CC_1);
        static double find_turn(double x1,double y1,double center_x,double center_y,int turn, double x2, double y2);
        static int init_block(block_pointer block);
        static int inverse_time_rate_arc(double x1,double y1,double z1,double cx,double cy,int turn,double x2, double y2, double z2,block_pointer block, setup_pointer settings);
        static int inverse_time_rate_arc2(double start_x,double start_y,int turn1,double mid_x,double mid_y,double cx,double cy,int turn2, double end_x,double end_y,double end_z,block_pointer block,setup_pointer settings);
        static int inverse_time_rate_as(double start_x,double start_y,int turn,double mid_x,double mid_y,double end_x,double end_y,double end_z,double AA_end,double BB_end,double CC_end,block_pointer block,setup_pointer settings);
        static int inverse_time_rate_straight(double end_x, double end_y,double end_z,double AA_end,double BB_end,double CC_end,block_pointer block,setup_pointer settings);
        static int parse_line(char * line,block_pointer block,setup_pointer settings);
        static int precedence(int an_operator);
    public:
        static int read_a(char* line,int* counter,block_pointer block,double* parameters);
        static int read_atan(char* line,int* counter,double* double_ptr,double* parameters);
        static int read_b(char* line,int* counter,block_pointer block,double* parameters);
        static int read_c(char* line,int* counter,block_pointer block,double* parameters);
        static int read_comment(char* line,int* counter,block_pointer block,double* parameters);
        static int read_d(char* line,int* counter,block_pointer block,double* parameters);
        static int read_f(char* line,int* counter,block_pointer block,double* parameters);
        static int read_g(char* line,int* counter,block_pointer block,double* parameters);
        static int read_h(char* line,int* counter,block_pointer block,double* parameters);
        static int read_i(char* line,int* counter,block_pointer block,double* parameters);
        static int read_integer_unsigned(char* line,int* counter,int* integer_ptr);
        static int read_integer_value(char* line,int* counter,int* integer_ptr,double * parameters);
        static int read_items(block_pointer block,char* line,double* parameters);
        static int read_j(char* line,int* counter,block_pointer block,double* parameters);
        static int read_k(char* line,int* counter,block_pointer block,double* parameters);
        static int read_l(char* line,int* counter,block_pointer block,double* parameters);
        static int read_line_number(char* line,int* counter, block_pointer block);
        static int read_m(char* line,int* counter,block_pointer block,double* parameters);
        static int read_one_item(char* line,int* counter,block_pointer block,double* parameters);
        static int read_operation(char* line,int* counter,int* operation);
        static int read_operation_unary(char* line,int* counter,int* operation);
        static int read_p(char* line,int* counter,block_pointer block,double* parameters);
        static int read_parameter(char* line,int* counter,double* double_ptr,double* parameters);
        static int read_parameter_setting(char* line,int* counter,block_pointer block,double* parameters);
        static int read_q(char* line,int* counter,block_pointer block,double* parameters);
        static int read_r(char* line,int* counter,block_pointer block,double* parameters);
        static int read_real_expression(char* line,int* counter,double* hold2,double* parameters);
        static int read_real_number(char* line,int* counter,double* double_ptr);
        static int read_real_value(char* line,int* counter,double* double_ptr,double* parameters);
        static int read_s(char * line, int * counter, block_pointer block,double* parameters);
        static int read_t(char * line, int * counter, block_pointer block,double* parameters);
        static int read_text(const char* command,FILE* inport,char* raw_line,char* line,int* length);
        static int read_unary(char* line,int* counter,double* double_ptr,double* parameters);
        static int read_x(char* line,int* counter,block_pointer block,double* parameters);
        static int read_y(char* line,int* counter,block_pointer block,double* parameters);
        static int read_z(char* line,int* counter,block_pointer block,double* parameters);
        static int set_probe_data(setup_pointer settings);
        static int write_g_codes(block_pointer block, setup_pointer settings);
        static int write_m_codes(block_pointer block, setup_pointer settings);
        static int write_settings(setup_pointer settings);
    public:
        static int  rs274ngc_close();
        static int  rs274ngc_execute();
        static int  rs274ngc_exit();
        static int  rs274ngc_init();
        static int  rs274ngc_load_tool_table();
        static int  rs274ngc_open(const char *filename);
        static int  rs274ngc_read(const char * mdi = nullptr);
        static int  rs274ngc_reset();
        static int  rs274ngc_restore_parameters(const char * filename);
        static int  rs274ngc_save_parameters(const char * filename, const double parameters[]);
        static int  rs274ngc_synch();
        static void rs274ngc_active_g_codes(int * codes);
        static void rs274ngc_active_m_codes(int * codes);
        static void rs274ngc_active_settings(double * settings);
        static void rs274ngc_error_text(int error_code, char * error_text,int max_size);
        static void rs274ngc_file_name(char* file_name, int max_size);
        static int  rs274ngc_line_length();
        static void rs274ngc_line_text(char * line_text, int max_size);
        static int  rs274ngc_sequence_number();
        static void rs274ngc_stack_name(int stack_index, char * function_name,int max_size);
};
///-----------------------------------------------------------------------------
#endif /// RS274NGC_H
///-----------------------------------------------------------------------------
