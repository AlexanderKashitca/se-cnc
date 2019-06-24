///-----------------------------------------------------------------------------
#ifndef DRIVER_H
#define DRIVER_H
///-----------------------------------------------------------------------------
#include "rs274ngc.h"
///-----------------------------------------------------------------------------
extern char  _interpreter_linetext[];
extern char  _interpreter_blocktext[];
///-----------------------------------------------------------------------------
int read_tool_file
(
    char * tool_file,      /// name of tool file
    setup_pointer settings /// pointer to machine settings
);
///-----------------------------------------------------------------------------
int read_setup_file
(
    char * setup_file,     /// name of setup file
    setup_pointer settings /// pointer to machine settings
);
///-----------------------------------------------------------------------------
#endif /// DRIVER_H
///-----------------------------------------------------------------------------
