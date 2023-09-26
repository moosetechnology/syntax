/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/
/* Beginning of sxtables for dag [Wed Jul 21 13:27:15 2004] */
#define SXP_MAX 30
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{1, 1, 0, -17},
{6, 6, 0, 25},
{2, 5, 1, 25},
{5, 9, 0, 26},
{0, 0, 2, -17},
{1, 2, 1, -1},
{6, 4, 1, 24},
{1, 7, 2, 26},
{1, 8, 2, 26},
{2, 3, 3, 24},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-7, 29, 0},
{0, 29, 0},
{-10, 29, 0},
{0, 29, 0},
{0, 29, 0},
{0, 29, 0},
{0, -5, 7},
{-6, 16, 1},
{0, 25, 26},
{0, 25, 27},
{0, 12, 7},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-3, 0, 0},
{-2, 30, 22},
{-3, 0, 0},
{-19, 30, 23},
{-20, 30, 23},
{-13, -2, 5},
};
static struct SXP_item vector[]={
{1, -17},
{2, -1},
{3, -18},
{1, 16},
{0, 0},
{5, -2},
{3, 8},
{0, 0},
{2, 14},
{5, 9},
{4, 15},
{4, -11},
{6, 4},
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
static struct SXP_local_mess SXP_local_mess[]={{0},
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
extern SXBOOLEAN sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no);

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,3,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,3,6,6,6,6,6,
6,6,6,3,3,3,3,3,3,3,3,3,4,5,4,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,6,7,3,3,8,3,6,9,10,3,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,3,3,3,6,3,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,11,3,12,3,6,3,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,9,13,10,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,3,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
3,6,6,6,6,6,6,6,6,};
static SXSTMI SXS_transition[]={0,
0x0,0x4007,0x0,0x2402,0x0,0x2c03,0x2404,0x2405,0x4402,0x4403,0x4404,0x4405,
0x4401,
0x0,0x4000,0x0,0x1402,0x0,0x4000,0x4000,0x2405,0x4000,0x4000,0x4000,0x4000,
0x4000,
0x0,0x4006,0x0,0x4006,0x0,0x1c03,0x4006,0x4006,0x4006,0x4006,0x4006,0x4006,
0x4006,
0x0,0x0,0x2c06,0x2c06,0x2c06,0x2c06,0x0,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,
0x2c06,
0x0,0x0,0x1405,0x1405,0x2402,0x1405,0x1405,0x1405,0x1405,0x1405,0x1405,0x1405,
0x1405,
0x0,0x0,0x1c06,0x1c06,0x1c06,0x1c06,0x4406,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,
0x1c06,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[13],
&SXS_transition[26],
&SXS_transition[39],
&SXS_transition[52],
&SXS_transition[65],
};
static char *S_adrp[]={0,
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
1,6};
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
static struct SXS_local_mess SXS_local_mess[]={{0},
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
extern SXINT sxscan_it();
extern SXBOOLEAN sxsrecovery (SXINT sxsrecovery_what, SXINT state_no, unsigned char *class);
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
extern SXINT sempass();
static char T_stack_schema[]={0,
0,0,0,2,0,0,1,0,1,1,0,0,};

static struct SXT_tables SXT_tables=
{SXT_node_info, T_ter_to_node_name, T_stack_schema, sempass, T_node_name};
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxatc();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(SXPARSER_T) sxparser}, {255, 7, 1, 3, 4, 13, 0, 6, 0, 1, 0, 
S_is_a_keyword,S_is_a_generic_terminal,S_transition_matrix,
NULL,
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
NULL,
},
{3, 10, 10, 12, 16, 21, 23, 36, 7, 5, 9, 9, 11, 7, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
vector-23,
NULL,
NULL,
NULL,
NULL,
P_lregle,
P_right_ctxt_head,
SXP_local_mess,
P_no_delete,
P_no_insert,
P_global_mess,PER_tset,sxscan_it,sxprecovery,
NULL,
NULL,
sxatc},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables,
NULL,
};
/* End of sxtables for dag [Wed Jul 21 13:27:15 2004] */
