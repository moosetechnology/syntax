#include "rcg_glbl.h"
#include "sxcommon.h"
char WHAT_RCGTESTPRIME6[] = "@(#)SYNTAX - $Id: prime6_parsact.c 807 2007-10-08 14:37:01Z rlacroix $" WHAT_DEBUG;

SXINT	Is_OK_arity = 1, Set_OK_arity = 1;
SXINT	Is_OK_pid, Set_OK_pid;

static SXINT	local_pid;
static BOOLEAN	OK;

BOOLEAN
_Is_OK ()
{
    return OK;
}

BOOLEAN
_Set_OK ()
{
    return OK = TRUE;
}

#if is_parser
#if is_multiple_pass
BOOLEAN
_walk_Is_OK ()
{
}

BOOLEAN
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
