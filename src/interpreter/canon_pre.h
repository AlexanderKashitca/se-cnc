///-----------------------------------------------------------------------------
#ifndef CANON_PRE_H
#define CANON_PRE_H
///-----------------------------------------------------------------------------
#include "canon.h"
///-----------------------------------------------------------------------------
/**
 * @brief Not static. Driver writes
 */
extern char _parameter_file_name[100];
extern int  _tool_max;         /*Not static. Driver reads  */
extern CANON_TOOL_TABLE         _tools[CANON_TOOL_MAX]; /*Not static. Driver writes */
///-----------------------------------------------------------------------------
#endif /// CANON_PRE_H
///-----------------------------------------------------------------------------
