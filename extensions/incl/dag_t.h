/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/


/* Beginning of sxtables for dag [Thu Dec 10 11:46:09 2009] */
#define SXP_MAX 31
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{6, 6, 0, 26},
{2, 5, 1, 26},
{5, 9, 0, 27},
{0, 0, 2, -17},
{1, 1, 1, -17},
{1, 2, 1, -18},
{6, 4, 1, 25},
{1, 7, 2, 27},
{1, 8, 2, 27},
{2, 3, 3, 25},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-7, 29, 0},
{0, 29, 0},
{-10, 29, 0},
{0, 29, 0},
{0, 29, 0},
{0, 29, 0},
{0, -4, 8},
{0, 5, 1},
{-6, 16, 2},
{0, 25, 26},
{0, 25, 27},
{0, 12, 8},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-2, 0, 0},
{-1, 31, 23},
{-2, 0, 0},
{-20, 31, 24},
{-21, 31, 24},
{-13, -1, 5},
};
static struct SXP_item vector[]={
{1, -17},
{2, -18},
{3, -19},
{2, 16},
{0, 0},
{5, -1},
{4, 8},
{0, 0},
{3, 14},
{6, 9},
{5, 15},
{4, -11},
{7, 3},
{0, 0},
};
static SXINT P_lrgl[]={
3,0,-2,2,0,0,0,0,
2,-2,1,0,0,0,0,0,
5,0,-1,1,2,3,0,0,
5,0,-1,2,3,4,0,0,
4,0,2,3,4,0,0,0,
7,0,-1,-1,1,2,3,4,
5,-1,0,1,2,3,0,0,
5,-1,1,2,3,4,0,0,
4,1,2,3,4,0,0,0,
4,-1,2,3,4,0,0,0,
5,-1,-1,1,2,3,0,0,
};
static SXINT *P_lregle[]={NULL,
&P_lrgl[0],
&P_lrgl[8],
&P_lrgl[16],
&P_lrgl[24],
&P_lrgl[32],
&P_lrgl[40],
&P_lrgl[48],
&P_lrgl[56],
&P_lrgl[64],
&P_lrgl[72],
&P_lrgl[80],
};
static SXINT P_right_ctxt_head[]={0,
2,1,1,2,2,1,1,1,1,2,1,};
static SXINT P_param_ref[]={
1,-2,
0,1,-1,
-2,1,
1,-2,
1,
-2,-3,1,
-1,0,1,
0,1,-1,
0,1,
0,1,-1,
0,1,-1,-2,
-1,1,
};
static struct SXP_local_mess SXP_local_mess[]={{0,NULL,NULL},
{2,"%sMisspelling of \"%s\" which is replaced by the keyword \"%s\".",&P_param_ref[0]},
{3,"%sMisspelling of \"%s\" before \"%s\" which is replaced by the keyword \"%s\".",&P_param_ref[2]},
{2,"%s\"%s\" is inserted before \"%s\".",&P_param_ref[5]},
{2,"%s\"%s\" is replaced by \"%s\".",&P_param_ref[7]},
{1,"%s\"%s\" is deleted.",&P_param_ref[9]},
{3,"%s\"%s %s\" is inserted before \"%s\".",&P_param_ref[10]},
{3,"%s\"%s\" is inserted before \"%s %s\".",&P_param_ref[13]},
{3,"%s\"%s\" before \"%s\" is replaced by \"%s\".",&P_param_ref[16]},
{2,"%s\"%s\" before \"%s\" is deleted.",&P_param_ref[19]},
{3,"%s\"%s %s\" is replaced by \"%s\".",&P_param_ref[21]},
{4,"%s\"%s\" before \"%s\" is replaced by \"%s %s\".",&P_param_ref[24]},
{2,"%s\"%s\" is forced before \"%s\".",&P_param_ref[28]},
};
static SXINT P_no_delete[]={
0};
static SXINT P_no_insert[]={
0};
static char *P_global_mess[]={
"%s\"%s\" is expected",
"%s\"%s\", \"%s\" is expected",
"%s\"%s\", \"%s\", \"%s\" is expected",
"%s\"%s\", \"%s\", \"%s\", \"%s\" is expected",
"%s\"%s\", \"%s\", \"%s\", \"%s\", \"%s\" is expected",
"%sGlobal recovery.",
"%sParsing resumes on \"%s\"",
"End Of File",
"%sParsing stops on End Of File.",
};
static SXINT PER_tset[]={
0};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%ld warnings and %ld errors are reported.";

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,4,5,4,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,3,3,3,6,3,3,7,8,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,9,10,11,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,12,13,14,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x4008,0x2c02,0x2403,0x4401,0x2404,0x4403,0x4404,0x4405,0x2405,0x4406,0x9c01,
0x4402,0x0,
0x0,0x0,0x1c02,0x4007,0x4007,0x4007,0x4007,0x4007,0x4007,0x2406,0x4007,0x4007,
0x4007,0x0,
0x0,0x4000,0x4000,0x1403,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x9c01,
0x4000,0x0,
0x0,0x0,0x1404,0x1404,0x4401,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x1404,0x1404,
0x0,0x0,0x2c02,0x0,0x4400,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,
0x2c02,0x2c02,
0x0,0x0,0x2c02,0x0,0x0,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,
0x2c02,0x2c02,
0x0,0x0,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x2c08,0x1c07,0x9c02,
0x1c07,0x9004,
0x0,0x0,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,
0x2c07,0x2c07,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[14],
&SXS_transition[28],
&SXS_transition[42],
&SXS_transition[56],
&SXS_transition[70],
&SXS_transition[84],
&SXS_transition[98],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2007,4,0,1,0},
{0x2007,12,0,1,0},
{0x2007,13,0,1,0},
{0x9c03,2,0,1,1},
{0x2c03,0,0,1,1},
};
static char *S_adrp[]={0,
"eol",
"|",
"(",
")",
"[",
"]",
"%word",
"End Of File",
};
static SXINT S_is_a_keyword[]={
0};
static SXINT S_is_a_generic_terminal[]={
1,7};
static SXINT S_lrgl[]={
4,1,2,3,4,0,
5,-1,1,2,3,4,
5,-1,0,1,2,3,
};
static SXINT *S_lregle[]={NULL,
&S_lrgl[0],
&S_lrgl[6],
&S_lrgl[12],
};
static SXINT S_param_ref[]={
0,
0,-1,
-1,0,
};
static struct SXS_local_mess SXS_local_mess[]={{0,NULL,NULL},
{1,"%sThe invalid character \"%s\" is deleted.",&S_param_ref[0]},
{2,"%sThe invalid character \"%s\" is replaced by \"%s\".",&S_param_ref[1]},
{2,"%sThe character \"%s\" is inserted before \"%s\".",&S_param_ref[3]},
};
static SXINT S_no_delete[]={
0};
static SXINT S_no_insert[]={
0};
static char *S_global_mess[]={0,
"%s\"%s\" is deleted.",
"%sThis unknown keyword is erased.",
"End Of Line",
"End Of File",
"%sScanning stops on End Of File.",
};
static struct SXT_node_info SXT_node_info[]={{0,0},
{5,1},{7,2},{0,3},{2,5},{0,6},{6,8},{3,9},{4,10},{8,11},{0,12},};
static SXINT T_ter_to_node_name[]={
0,};
static char *T_node_name[]={NULL,
"ERROR",
"ALTERNATIVE_S",
"EXPRESSION",
"OPTION",
"PROGRAM_ROOT_DAG",
"SEQUENCE_S",
"SIMPLE_REGULAR_EXPRESSION",
"WORD",
};
extern SXSEMPASS_FUNCTION SEMPASS;
static char T_stack_schema[]={0,
1,0,0,2,0,0,1,0,1,1,0,0,};

static struct SXT_tables SXT_tables={
SXT_node_info, T_ter_to_node_name, T_stack_schema, SEMPASS, T_node_name
};
extern SXSEMACT_FUNCTION sxatc;

SXTABLES sxtables={
52113, /* magic */
sxscanner,
sxparser,
{255, 8, 1, 3, 4, 14, 1, 8, 0, 0, 0, 
S_is_a_keyword,S_is_a_generic_terminal,S_transition_matrix,
SXS_action_or_prdct_code,
S_adrp,
NULL,
SXS_local_mess,
S_char_to_simple_class+128,
S_no_delete,
S_no_insert,
S_global_mess,
S_lregle,
NULL,
sxsrecovery,
NULL
},
{2, 10, 10, 12, 16, 22, 24, 37, 8, 5, 9, 9, 12, 7, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-24,
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif 
NULL,
NULL,
NULL,
NULL,
P_lregle,
P_right_ctxt_head,
SXP_local_mess,
P_no_delete,
P_no_insert,
P_global_mess,
PER_tset,
sxscan_it,
sxprecovery,
NULL,
NULL,
sxatc
},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables,
NULL,
};
/* End of sxtables for dag [Thu Dec 10 11:46:09 2009] */
