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


/* Beginning of sxtables for amlgm_cmpd [Tue Feb 23 17:49:56 2010] */
#define SXP_MAX 28
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{6, 3, 0, -12},
{6, 6, 0, -13},
{2, 2, 1, -12},
{2, 5, 1, -13},
{6, 8, 0, -14},
{2, 7, 1, -14},
{5, 9, 0, 24},
{5, 10, 0, 24},
{0, 0, 2, -17},
{1, 1, 1, -17},
{1, 4, 3, 21},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-10, 26, 0},
{0, 26, 27},
{0, 26, 25},
{0, 26, 0},
{0, 26, 0},
{0, -9, 5},
{0, 15, 5},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-3, 19, 28},
{-4, 0, 0},
{-6, 0, 0},
{-2, 19, 23},
{-5, -14, 5},
};
static struct SXP_item vector[]={
{4, -13},
{1, -17},
{2, -12},
{3, -1},
{2, 11},
{1, 16},
{3, 7},
{4, 8},
{0, 0},
{0, 0},
{0, 0},
{6, -2},
};

static  SXBA_ELT SXPBA_kt2
#if SXBITS_PER_LONG==32
 [2] = {8, 0X000000BE, }
#else
 [2] = {8, 0X00000000000000BE, }
#endif
/* End SXPBA_kt2 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt2[0],
};
static SXINT P_lrgl[]={
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
};
static SXINT P_right_ctxt_head[]={0,
1,2,2,1,1,1,1,2,1,};
static SXINT P_param_ref[]={
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
{2,"%s\"%s\" is inserted before \"%s\".",&P_param_ref[0]},
{2,"%s\"%s\" is replaced by \"%s\".",&P_param_ref[2]},
{1,"%s\"%s\" is deleted.",&P_param_ref[4]},
{3,"%s\"%s %s\" is inserted before \"%s\".",&P_param_ref[5]},
{3,"%s\"%s\" is inserted before \"%s %s\".",&P_param_ref[8]},
{3,"%s\"%s\" before \"%s\" is replaced by \"%s\".",&P_param_ref[11]},
{2,"%s\"%s\" before \"%s\" is deleted.",&P_param_ref[14]},
{3,"%s\"%s %s\" is replaced by \"%s\".",&P_param_ref[16]},
{4,"%s\"%s\" before \"%s\" is replaced by \"%s %s\".",&P_param_ref[19]},
{2,"%s\"%s\" is forced before \"%s\".",&P_param_ref[23]},
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
1,2};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%ld warnings and %ld errors are reported.";
extern SXBOOLEAN sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no);

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,12,3,12,3,12,12,3,3,12,3,12,12,3,12,12,12,12,
3,12,3,12,12,3,3,12,3,12,12,3,12,12,12,12,12,12,12,12,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,12,12,12,3,12,12,12,12,12,12,12,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,3,12,12,12,12,12,
12,12,12,3,3,3,3,3,3,3,3,3,4,5,4,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,6,7,8,3,3,3,6,3,3,9,3,3,10,3,
11,6,6,6,6,6,6,6,6,6,6,12,13,3,3,14,3,3,12,12,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,3,15,3,3,12,3,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
12,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,12,3,12,3,12,12,3,3,12,3,12,
12,3,12,12,12,12,3,12,3,12,12,3,3,12,3,12,12,3,12,12,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,3,12,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
3,12,12,12,12,12,12,12,12,};
static SXSTMI SXS_transition[]={0,
0x0,0x4005,0x0,0x2402,0x2402,0x0,0x2403,0x2404,0x0,0x2405,0x2406,0x2c07,
0x4402,0x0,0x0,
0x0,0x4000,0x0,0x1402,0x1402,0x0,0x4000,0x2404,0x0,0x4000,0x2406,0x4000,
0x4000,0x0,0x0,
0x0,0x0,0x1c03,0x1c03,0x0,0x1c03,0x4404,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x1c03,0x2408,
0x0,0x0,0x1404,0x1404,0x2402,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x1404,0x1404,0x1404,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2409,0x0,0x0,
0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x240a,0x0,0x2404,0x0,
0x0,0x0,0x0,
0x0,0x0,0x0,0x4003,0x4003,0x1c07,0x4003,0x4003,0x0,0x1c07,0x4003,0x1c07,
0x4003,0x0,0x0,
0x0,0x0,0x0,0x0,0x2403,0x0,0x2c03,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x2c03,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x4401,0x0,
0x0,0x0,0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,0x240b,0x140a,0x140a,0x140a,
0x140a,0x140a,0x140a,
0x0,0x0,0x240a,0x240a,0x240a,0x240a,0x240a,0x240a,0x140b,0x240a,0x2402,0x240a,
0x240a,0x240a,0x240a,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[15],
&SXS_transition[30],
&SXS_transition[45],
&SXS_transition[60],
&SXS_transition[75],
&SXS_transition[90],
&SXS_transition[105],
&SXS_transition[120],
&SXS_transition[135],
&SXS_transition[150],
};
static char *S_adrp[]={0,
"-->",
";",
"%IDENT",
"%CHAINE",
"End Of File",
};
static SXINT S_is_a_keyword[]={
0};
static SXINT S_is_a_generic_terminal[]={
2,3,4};
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
extern SXINT sxscan_it(void);
extern SXBOOLEAN sxsrecovery (SXINT sxsrecovery_what, SXINT state_no, unsigned char *class);
static struct SXT_node_info SXT_node_info[]={{0,0},
{2,1},{0,2},{8,4},{7,5},{0,7},{5,9},{0,10},{6,12},{4,13},{3,14},{0,15},};
static SXINT T_ter_to_node_name[]={
0,};
static char *T_node_name[]={NULL,
"ERROR",
"AMLGM_CMPD_ROOT",
"CHAINE",
"IDENT",
"LHS_S",
"RHS_S",
"RULE",
"RULE_S",
};
extern SXINT sempass(SXINT what, struct sxtables *sxtables_ptr);
static char T_stack_schema[]={0,
0,0,1,0,1,3,0,1,0,0,1,0,0,0,0,};

static struct SXT_tables SXT_tables=
{SXT_node_info, T_ter_to_node_name, T_stack_schema, sempass, T_node_name};
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxatc(SXINT what, ...);

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(SXPARSER_T) sxparser}, {255, 5, 1, 3, 4, 15, 0, 11, 0, 1, 0, 
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
(SXRECOVERY_T) sxsrecovery,
NULL,
},
{6, 11, 11, 15, 16, 18, 23, 34, 5, 6, 10, 10, 7, 6, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-23,
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif 
SXPBM_trans,
NULL,
NULL,
NULL,
P_lregle,
P_right_ctxt_head,
SXP_local_mess,
P_no_delete,
P_no_insert,
P_global_mess,PER_tset,sxscan_it,(SXRECOVERY_T) sxprecovery,
NULL,
NULL,
(SXSEMACT_T) sxatc
},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables,
NULL,
};
/* End of sxtables for amlgm_cmpd [Tue Feb 23 17:49:56 2010] */
