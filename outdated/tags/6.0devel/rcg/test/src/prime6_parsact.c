/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 
#include "rcg_glbl.h"
#include "sxcommon.h"
char WHAT_RCGTESTPRIME6[] = "@(#)SYNTAX - $Id: prime6_parsact.c 1289 2008-05-13 13:35:23Z rlacroix $" WHAT_DEBUG;

SXINT	Is_OK_arity = 1, Set_OK_arity = 1;
SXINT	Is_OK_pid, Set_OK_pid;

static SXINT	local_pid;
static SXBOOLEAN	OK;

SXBOOLEAN
_Is_OK ()
{
    return OK;
}

SXBOOLEAN
_Set_OK ()
{
    return OK = SXTRUE;
}

#if is_parser
#if is_multiple_pass
SXBOOLEAN
_walk_Is_OK ()
{
}

SXBOOLEAN
_walk_Set_OK ()
{
}
#endif
#endif



void _prime6_parsact_if () {
    local_pid = global_pid++;
    first_pass_init_fun [local_pid] = second_pass_init_fun [local_pid] = first_pass_final_fun [local_pid] = second_pass_final_fun [local_pid] = sxvoid;

    Is_OK_pid = Set_OK_pid = local_pid;
}
