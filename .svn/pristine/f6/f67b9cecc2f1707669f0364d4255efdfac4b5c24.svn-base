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


/* Beginning of sxtables for re [Thu Dec 10 11:46:09 2009] */
#define SXP_MAX 43
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{2, 8, 1, -18},
{6, 9, 1, -18},
{2, 4, 2, -26},
{6, 5, 2, -26},
{1, 17, 1, -28},
{5, 16, 1, -28},
{0, 0, 2, -24},
{1, 1, 1, -24},
{0, 2, 1, 33},
{0, 3, 1, 33},
{0, 6, 1, 35},
{0, 7, 1, 35},
{1, 11, 1, 37},
{1, 12, 1, 37},
{1, 13, 1, 37},
{0, 10, 1, 37},
{0, 14, 2, -28},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-11, 34, 0},
{-12, 34, 0},
{0, 34, 0},
{0, 34, 0},
{0, 34, 0},
{0, 34, 0},
{0, -7, 10},
{0, 8, 1},
{-9, 22, 2},
{-10, 23, 2},
{-16, 33, 0},
{0, 17, 7},
{-6, 5, 9},
{0, 20, 10},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-1, -28, 7},
{-2, -28, 7},
{-28, 41, 32},
{-29, 41, 36},
{-3, 43, 0},
{-4, 43, 0},
};
static struct SXP_item vector[]={
{1, -24},
{2, -25},
{0, 0},
{3, 13},
{4, 14},
{5, 15},
{0, 0},
{6, 21},
{0, 0},
{8, 30},
{7, -28},
{3, -26},
{4, -27},
{5, -18},
{6, -19},
{5, -18},
{6, -19},
{7, -28},
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
extern SXBOOLEAN sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no);

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,4,5,4,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,3,3,3,6,3,3,7,8,9,10,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,11,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,12,13,14,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,15,16,17,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x400a,0x2c02,0x2403,0x4401,0x2404,0x4406,0x4407,0x4403,0x4404,0x4405,0x2c05,
0x2406,0x2c02,0x9c01,0x4402,0x0,
0x0,0x0,0x1c02,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x1c02,
0x2407,0x1c02,0x4008,0x4008,0x0,
0x0,0x4000,0x4000,0x1403,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x9c01,0x4000,0x0,
0x0,0x0,0x1404,0x1404,0x4401,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x1404,0x1404,0x1404,0x1404,0x1404,
0x0,0x0,0x2c02,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x2c02,
0x2407,0x2c02,0x4008,0x9c02,0x0,
0x0,0x0,0x2c02,0x0,0x4400,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,
0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,
0x0,0x0,0x2c02,0x0,0x0,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,
0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,
0x0,0x0,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,
0x2c0a,0x1c08,0x9c03,0x1c08,0x9005,
0x0,0x0,0x1c09,0x0,0x0,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,
0x240b,0x1c09,0x1c09,0x240c,0x1c09,
0x0,0x0,0x2c08,0x2c08,0x2c08,0x2c08,0x2c08,0x2c08,0x2c08,0x2c08,0x2c08,0x2c08,
0x2c08,0x2c08,0x2c08,0x2c08,0x2c08,
0x0,0x0,0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,
0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x4409,0x0,0x0,0x0,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[17],
&SXS_transition[34],
&SXS_transition[51],
&SXS_transition[68],
&SXS_transition[85],
&SXS_transition[102],
&SXS_transition[119],
&SXS_transition[136],
&SXS_transition[153],
&SXS_transition[170],
&SXS_transition[187],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2008,4,0,1,0},
{0x2009,5,0,1,0},
{0x2008,12,0,1,0},
{0x2008,13,0,1,0},
{0x9c04,2,0,1,1},
{0x2c03,0,0,1,1},
};
static char *S_adrp[]={0,
"%eof",
"|",
"*",
"+",
"?",
"(",
")",
"%word",
"%SEMLEX",
"End Of File",
};
static SXINT S_is_a_keyword[]={
0};
static SXINT S_is_a_generic_terminal[]={
3,1,8,9};
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
{6,1},{0,3},{0,4},{0,5},{2,7},{0,9},{0,10},{0,11},{8,13},{0,15},{4,16},{3,17},
{5,18},{0,19},{0,20},{11,21},{9,22},{0,24},};
static SXINT T_ter_to_node_name[]={
0,10,0,0,0,0,0,0,11,7,};
static char *T_node_name[]={NULL,
"ERROR",
"ALTERNATIVE_S",
"KLEENE_PLUS",
"KLEENE_STAR",
"OPTION",
"RE_ROOT",
"SEMLEX",
"SEQUENCE_S",
"WORD_SEMLEX",
"eof",
"word",
};
extern SXINT sempass(SXINT what, struct sxtables *sxtables_ptr);
static char T_stack_schema[]={0,
0,1,0,0,0,2,0,2,0,0,0,1,0,1,0,1,1,1,1,0,0,0,1,0,};

static struct SXT_tables SXT_tables=
{SXT_node_info, T_ter_to_node_name, T_stack_schema, sempass, T_node_name};
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxatc(SXINT what, ...);

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(SXPARSER_T) sxparser}, {255, 10, 1, 3, 4, 17, 1, 12, 0, 0, 0, 
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
(SXRECOVERY_T) sxsrecovery,
NULL,
},
{4, 17, 17, 20, 23, 31, 33, 50, 10, 8, 17, 17, 14, 7, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-33,
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
/* End of sxtables for re [Thu Dec 10 11:46:09 2009] */
