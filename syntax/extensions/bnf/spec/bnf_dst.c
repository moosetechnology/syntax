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
/* Beginning of sxtables for bnf [Thu Jun  9 11:32:55 1994] */
#include "sxunix.h"
#include "sxba.h"
char WHAT_BNFSPECDST[] = "@(#)SYNTAX - $Id: bnf_dst.c 3303 2023-05-26 08:47:54Z garavel $" WHAT_DEBUG;

static struct SXP_reductions reductions[]={
{0, 3, 0, -15},
{0, 2, 1, -15},
{0, 5, 1, -16},
{0, 6, 1, -16},
{0, 0, 2, -18},
{0, 1, 1, -18},
{0, 4, 1, 25},
{0, 9, 0, -3},
{0, 12, 0, -19},
{0, 13, 0, -19},
{0, 8, 1, -3},
{0, 7, 1, -3},
{0, 11, 1, -3},
{0, 10, 1, -3},
};
static struct SXP_bases t_bases[]={
{-6, 20, 2},
{0, 26, 0},
{0, 20, 2},
{0, -5, 9},
{-14, 13, 5},
{0, 4, 3},
{-12, 11, 5},
{0, 17, 9},
};
static struct SXP_bases nt_bases[]={
{-2, -16, 4},
{-19, -3, 5},
{-16, 23, 0},
};
static struct SXP_item vector[]={
{1, -18},
{2, -15},
{3, -1},
{1, 7},
{0, 0},
{0, 0},
{4, 21},
{0, 0},
{6, 8},
{7, 9},
{8, 10},
{0, 0},
};
static SXBA_ELT SXPBA_kt1[]={
9,270,};
static SXBA SXPBM_trans [] = {
&SXPBA_kt1[0],
};
static short P_lrgl[]={
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
static short *P_lregle[]={
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
static SXINT P_right_ctxt_head[]={
1,2,2,1,1,1,1,2,1,};
static short P_param_ref[]={
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
static struct SXP_local_mess SXP_local_mess[]={
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
static short P_no_delete[]={
0};
static short P_no_insert[]={
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
static short PER_tset[]={
1,1};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%ld warnings and %ld errors are reported.";
extern SXBOOLEAN sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no);

static unsigned char S_char_to_simple_class[]={
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,3,3,3,3,3,3,3,3,3,4,5,4,4,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,6,7,8,9,7,10,11,7,7,7,12,7,7,7,7,
7,13,14,14,14,14,14,14,14,15,15,7,16,17,18,19,7,20,21,21,21,21,21,21,21,
21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,7,22,7,7,23,7,
21,24,21,21,21,25,21,21,21,21,21,21,21,26,21,21,21,27,21,28,21,29,21,21,21,
21,7,7,7,30,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,};
static SXSTMI SXS_transition[]={
0x0,0x4009,0x0,0x2402,0x2403,0x2402,0x2c04,0x2405,0x2406,0x2c07,0x2c08,0x2c04,
0x2c04,0x2c04,0x2c04,0x9402,0x9005,0x9007,0x2c04,0x2c0a,0x2c04,0x2c04,0x2c04,0x2c04,
0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x240b,
0x0,0x4000,0x0,0x1402,0x2403,0x1402,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x240b,
0x0,0x4000,0x0,0x2402,0x1403,0x2402,0x4000,0x4000,0x4000,0x4000,0x4000,0x240c,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x240b,
0x0,0x0,0x0,0x4007,0x4007,0x4007,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,
0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,
0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,
0x0,0x0,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x240e,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x0,0x0,0x0,0x0,0x0,0x0,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,
0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,
0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,
0x0,0x0,0x0,0x4007,0x4007,0x4007,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,
0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c0f,0x2c04,0x2c04,0x2c0f,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c04,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x7010,0x2c11,0x2c11,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,
0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x4c04,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,
0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x7012,0x2c13,0x2c13,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x140b,0x140b,0x2414,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x140b,0x140b,0x140b,0x140b,0x140b,0x2402,
0x0,0x0,0x140c,0x140c,0x2403,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,
0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,
0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,
0x0,0x0,0x1c0d,0x1c0d,0x0,0x1c0d,0x1c0d,0x4407,0x1c0d,0x1c0d,0x1c0d,0x1c0d,
0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x2416,0x1c0d,0x1c0d,
0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,
0x0,0x0,0x2c0d,0x2c0d,0x2417,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x900b,0x900b,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x940c,
0x940d,0x940e,0x940f,0x9410,0x9411,0x2c0d,
0x0,0x0,0x0,0x4008,0x4008,0x4008,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,
0x1c0f,0x1c0f,0x1c0f,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x1c0f,0x2c04,0x2c18,0x1c0f,
0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x2c04,
0x1,0x0,0x0,0x9c01,0x9c01,0x9c01,0x0,0x0,0x0,0x0,0x0,0x0,
0x9009,0x9009,0x9009,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x9c01,
0x0,0x0,0x0,0x9001,0x9001,0x9001,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c11,0x1c11,0x1c11,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x9001,
0x1,0x0,0x0,0x4c06,0x4c06,0x4c06,0x0,0x0,0x0,0x0,0x0,0x0,
0x900a,0x900a,0x900a,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x4c06,
0x0,0x0,0x0,0x4006,0x4006,0x4006,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c13,0x1c13,0x1c13,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x4006,
0x0,0x0,0x240b,0x240b,0x1414,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,
0x240b,0x240b,0x240b,0x240b,0x0,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,
0x240b,0x240b,0x240b,0x240b,0x240b,0x2402,
0x0,0x0,0x0,0x0,0x0,0x1c15,0x1c15,0x1c15,0x1c15,0x1c15,0x1c15,0x1c15,
0x1c15,0x1c15,0x1c15,0x1c15,0x1c15,0x1c15,0x4c02,0x1c15,0x1c15,0x1c15,0x1c15,0x1c15,
0x1c15,0x1c15,0x1c15,0x1c15,0x1c15,0x1c15,
0x0,0x0,0x2c0d,0x2c0d,0x2419,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x900b,0x900b,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x940c,
0x940d,0x940e,0x940f,0x9410,0x9411,0x2c0d,
0x0,0x0,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x240e,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x0,0x0,0x0,0x4007,0x4007,0x4007,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,
0x2c0f,0x2c0f,0x2c0f,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c0f,0x2c04,0x2c04,0x2c0f,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c04,
0x0,0x0,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x4407,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2416,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c1a,0x9012,0x9012,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c1b,0x9013,0x9013,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x9014,0x9014,0x0,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,
0x2c1d,0x2c1d,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,
0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,
0x0,0x0,0x9014,0x9014,0x0,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,
0x9c14,0x9c14,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,
0x9014,0x9014,0x9014,0x9014,0x9014,0x9014,
};
static SXSTMI *S_transition_matrix[]={
&SXS_transition[0]-1,
&SXS_transition[30]-1,
&SXS_transition[60]-1,
&SXS_transition[90]-1,
&SXS_transition[120]-1,
&SXS_transition[150]-1,
&SXS_transition[180]-1,
&SXS_transition[210]-1,
&SXS_transition[240]-1,
&SXS_transition[270]-1,
&SXS_transition[300]-1,
&SXS_transition[330]-1,
&SXS_transition[360]-1,
&SXS_transition[390]-1,
&SXS_transition[420]-1,
&SXS_transition[450]-1,
&SXS_transition[480]-1,
&SXS_transition[510]-1,
&SXS_transition[540]-1,
&SXS_transition[570]-1,
&SXS_transition[600]-1,
&SXS_transition[630]-1,
&SXS_transition[660]-1,
&SXS_transition[690]-1,
&SXS_transition[720]-1,
&SXS_transition[750]-1,
&SXS_transition[780]-1,
&SXS_transition[810]-1,
&SXS_transition[840]-1,
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={
{24581,3,0,0,0},
{16385,1,0,0,0},
{8213,3,0,1,0},
{16387,4,0,1,0},
{39939,1,0,1,1},
{11273,0,0,1,1},
{37892,2,0,1,1},
{11268,0,0,1,1},
{11290,10,0,1,0},
{11291,10,0,1,0},
{11292,10,0,1,0},
{8205,9,8,1,0},
{8205,9,12,1,0},
{8205,9,10,1,0},
{8205,9,13,1,0},
{8205,9,9,1,0},
{8205,9,11,1,0},
{11281,5,0,1,0},
{11283,5,0,1,0},
{8205,2,0,0,0},
};
static char *S_adrp[]={
";",
"%LHS_NON_TERMINAL",
"=",
"%NON_TERMINAL",
"%PREDICATE",
"%ACTION",
"%TERMINAL",
"%GENERIC_TERMINAL",
"End Of File",
};
static short S_is_a_keyword[]={
0};
static short S_is_a_generic_terminal[]={
6,2,4,5,6,7,8};
static short S_lrgl[]={
4,1,2,3,4,0,
5,-1,1,2,3,4,
5,-1,0,1,2,3,
};
static short *S_lregle[]={
&S_lrgl[0],
&S_lrgl[6],
&S_lrgl[12],
};
static short S_param_ref[]={
0,
0,-1,
-1,0,
};
static struct SXS_local_mess SXS_local_mess[]={
{1,"%sThe invalid character \"%s\" is deleted.",&S_param_ref[0]},
{2,"%sThe invalid character \"%s\" is replaced by \"%s\".",&S_param_ref[1]},
{2,"%sThe character \"%s\" is inserted before \"%s\".",&S_param_ref[3]},
};
static short S_no_delete[]={
0};
static short S_no_insert[]={
0};
static char *S_global_mess[]={
"%s\"%s\" is deleted.",
"%sThis unknown keyword is erased.",
"End Of Line",
"End Of File",
"%sScanning stops on End Of File.",
};
extern SXINT SCANACT(SXINT what, SXINT act_no);
extern SXINT sxscan_it();
extern SXBOOLEAN sxsrecovery (SXINT sxsrecovery_what, SXINT state_no, unsigned char *class);
extern SXINT sxscanner();
extern SXINT sxparser();
extern SXINT SEMACT();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(SXPARSER_T) sxparser}, {255, 9, 1, 3, 4, 30, 1, 29, 1, 1, 0, 
S_is_a_keyword,S_is_a_generic_terminal,S_transition_matrix-1,
SXS_action_or_prdct_code-1,
S_adrp-1,
NULL,
SXS_local_mess-1,
S_char_to_simple_class+128,
S_no_delete,S_no_insert,
S_global_mess-1,
S_lregle-1,
(SXSCANACT_T) SCANACT,
sxsrecovery,
NULL,
},
{3, 14, 14, 17, 17, 22, 24, 35, 9, 6, 13, 13, 8, 4, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions-1,
t_bases-1,
nt_bases-1,
vector-24,
#if SXBITS_PER_LONG == 32
SXPBM_trans-1,
#else
NULL,
#endif
NULL,
NULL,
NULL,
P_lregle-1,P_right_ctxt_head-1,
SXP_local_mess-1,
P_no_delete,P_no_insert,
P_global_mess,PER_tset,sxscan_it,sxprecovery,
NULL
,NULL
,(SXSEMACT_T) SEMACT},
err_titles,
abstract,
(sxsem_tables*)NULL
};

#include "sxdynam_scanner.h"


static struct {
	struct sxword_string	*next;
	char	chars [247];
} DYNAM_SCANNER_sc_names_string = {
NULL,
"\000\000NUL\000ANY\000SOH\000STX\000ETX\000EOT\000ENQ\000ACK\000BEL\000BS\000HT\000LF\000NL\000EOL\000VT\000FF\000CR\000S\
O\000SI\000DLE\000DC1\000DC2\000DC3\000DC4\000NAK\000SYN\000ETB\000CAN\000EM\000SUB\000ESC\000FS\000GS\000RS\000US\000\
SP\000QUOTE\000DEL\000LETTER\000UPPER\000LOWER\000A\000B\000C\000D\000E\000F\000G\000H\000I\000J\000K\000L\000M\000N\000O\000P\000\
Q\000R\000S\000T\000U\000V\000W\000X\000Y\000Z\000DIGIT\000PRINT\000T_HEADER\000NT_BODY\000OCTAL\000"} /* End DYNAM_SCANNER_sc_names_string */;


static SXUINT DYNAM_SCANNER_sc_names_head [] = {
/* 0 */ 1, 0, 0, 0, 0, 0, 0, 0, 
/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 16 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 24 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 32 */ 6, 0, 0, 0, 0, 0, 0, 0, 
/* 40 */ 0, 0, 0, 0, 39, 0, 0, 12, 
/* 48 */ 0, 0, 0, 0, 0, 0, 28, 0, 
/* 56 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 64 */ 34, 5, 0, 0, 0, 32, 0, 31, 
/* 72 */ 0, 0, 0, 0, 10, 0, 0, 16, 
/* 80 */ 33, 0, 0, 0, 0, 0, 0, 0, 
/* 88 */ 0, 0, 0, 0, 0, 70, 0, 0, 
/* 96 */ 36, 0, 0, 0, 25, 40, 30, 22, 
/* 104 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 112 */ 0, 0, 0, 0, 0, 7, 0, 0, 
/* 120 */ 0, 0, 29, 0, 0, 15, 69, 0, 
/* 128 */ 0, 18, 0, 37, 72, 0, 73, 0, 
/* 136 */ 8, 0, 20, 0, 0, 2, 0, 0, 
/* 144 */ 35, 0, 0, 0, 0, 0, 0, 0, 
/* 152 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 160 */ 42, 0, 4, 0, 0, 0, 0, 0, 
/* 168 */ 68, 67, 66, 65, 64, 63, 62, 61, 
/* 176 */ 60, 59, 58, 57, 56, 55, 54, 53, 
/* 184 */ 52, 51, 50, 49, 48, 47, 46, 45, 
/* 192 */ 44, 43, 0, 0, 21, 0, 0, 0, 
/* 200 */ 0, 0, 0, 0, 0, 0, 26, 0, 
/* 208 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 216 */ 14, 0, 0, 0, 0, 0, 0, 0, 
/* 224 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 232 */ 0, 0, 0, 0, 38, 0, 0, 0, 
/* 240 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 248 */ 0, 0, 0, 71, 27, 0, 13, 0, 
/* 256 */ 0, 
} /* End DYNAM_SCANNER_sc_names_head */;


static struct sxword_table DYNAM_SCANNER_sc_names_table [132] = {
/* 0 */ {&(DYNAM_SCANNER_sc_names_string.chars [0]), 0, 0},
/* 1 */ {&(DYNAM_SCANNER_sc_names_string.chars [1]), 0, 0},
/* 2 */ {&(DYNAM_SCANNER_sc_names_string.chars [2]), 3, 0},
/* 3 */ {&(DYNAM_SCANNER_sc_names_string.chars [6]), 3, 0},
/* 4 */ {&(DYNAM_SCANNER_sc_names_string.chars [10]), 3, 0},
/* 5 */ {&(DYNAM_SCANNER_sc_names_string.chars [14]), 3, 0},
/* 6 */ {&(DYNAM_SCANNER_sc_names_string.chars [18]), 3, 0},
/* 7 */ {&(DYNAM_SCANNER_sc_names_string.chars [22]), 3, 0},
/* 8 */ {&(DYNAM_SCANNER_sc_names_string.chars [26]), 3, 0},
/* 9 */ {&(DYNAM_SCANNER_sc_names_string.chars [30]), 3, 0},
/* 10 */ {&(DYNAM_SCANNER_sc_names_string.chars [34]), 3, 0},
/* 11 */ {&(DYNAM_SCANNER_sc_names_string.chars [38]), 2, 0},
/* 12 */ {&(DYNAM_SCANNER_sc_names_string.chars [41]), 2, 0},
/* 13 */ {&(DYNAM_SCANNER_sc_names_string.chars [44]), 2, 0},
/* 14 */ {&(DYNAM_SCANNER_sc_names_string.chars [47]), 2, 0},
/* 15 */ {&(DYNAM_SCANNER_sc_names_string.chars [50]), 3, 9},
/* 16 */ {&(DYNAM_SCANNER_sc_names_string.chars [54]), 2, 0},
/* 17 */ {&(DYNAM_SCANNER_sc_names_string.chars [57]), 2, 0},
/* 18 */ {&(DYNAM_SCANNER_sc_names_string.chars [60]), 2, 0},
/* 19 */ {&(DYNAM_SCANNER_sc_names_string.chars [63]), 2, 0},
/* 20 */ {&(DYNAM_SCANNER_sc_names_string.chars [66]), 2, 0},
/* 21 */ {&(DYNAM_SCANNER_sc_names_string.chars [69]), 3, 0},
/* 22 */ {&(DYNAM_SCANNER_sc_names_string.chars [73]), 3, 0},
/* 23 */ {&(DYNAM_SCANNER_sc_names_string.chars [77]), 3, 0},
/* 24 */ {&(DYNAM_SCANNER_sc_names_string.chars [81]), 3, 0},
/* 25 */ {&(DYNAM_SCANNER_sc_names_string.chars [85]), 3, 0},
/* 26 */ {&(DYNAM_SCANNER_sc_names_string.chars [89]), 3, 0},
/* 27 */ {&(DYNAM_SCANNER_sc_names_string.chars [93]), 3, 0},
/* 28 */ {&(DYNAM_SCANNER_sc_names_string.chars [97]), 3, 0},
/* 29 */ {&(DYNAM_SCANNER_sc_names_string.chars [101]), 3, 0},
/* 30 */ {&(DYNAM_SCANNER_sc_names_string.chars [105]), 2, 23},
/* 31 */ {&(DYNAM_SCANNER_sc_names_string.chars [108]), 3, 0},
/* 32 */ {&(DYNAM_SCANNER_sc_names_string.chars [112]), 3, 0},
/* 33 */ {&(DYNAM_SCANNER_sc_names_string.chars [116]), 2, 0},
/* 34 */ {&(DYNAM_SCANNER_sc_names_string.chars [119]), 2, 0},
/* 35 */ {&(DYNAM_SCANNER_sc_names_string.chars [122]), 2, 11},
/* 36 */ {&(DYNAM_SCANNER_sc_names_string.chars [125]), 2, 0},
/* 37 */ {&(DYNAM_SCANNER_sc_names_string.chars [128]), 2, 0},
/* 38 */ {&(DYNAM_SCANNER_sc_names_string.chars [131]), 5, 0},
/* 39 */ {&(DYNAM_SCANNER_sc_names_string.chars [137]), 3, 0},
/* 40 */ {&(DYNAM_SCANNER_sc_names_string.chars [141]), 6, 24},
/* 41 */ {&(DYNAM_SCANNER_sc_names_string.chars [148]), 5, 0},
/* 42 */ {&(DYNAM_SCANNER_sc_names_string.chars [154]), 5, 0},
/* 43 */ {&(DYNAM_SCANNER_sc_names_string.chars [160]), 1, 0},
/* 44 */ {&(DYNAM_SCANNER_sc_names_string.chars [162]), 1, 3},
/* 45 */ {&(DYNAM_SCANNER_sc_names_string.chars [164]), 1, 0},
/* 46 */ {&(DYNAM_SCANNER_sc_names_string.chars [166]), 1, 0},
/* 47 */ {&(DYNAM_SCANNER_sc_names_string.chars [168]), 1, 0},
/* 48 */ {&(DYNAM_SCANNER_sc_names_string.chars [170]), 1, 0},
/* 49 */ {&(DYNAM_SCANNER_sc_names_string.chars [172]), 1, 0},
/* 50 */ {&(DYNAM_SCANNER_sc_names_string.chars [174]), 1, 0},
/* 51 */ {&(DYNAM_SCANNER_sc_names_string.chars [176]), 1, 0},
/* 52 */ {&(DYNAM_SCANNER_sc_names_string.chars [178]), 1, 0},
/* 53 */ {&(DYNAM_SCANNER_sc_names_string.chars [180]), 1, 0},
/* 54 */ {&(DYNAM_SCANNER_sc_names_string.chars [182]), 1, 0},
/* 55 */ {&(DYNAM_SCANNER_sc_names_string.chars [184]), 1, 0},
/* 56 */ {&(DYNAM_SCANNER_sc_names_string.chars [186]), 1, 0},
/* 57 */ {&(DYNAM_SCANNER_sc_names_string.chars [188]), 1, 0},
/* 58 */ {&(DYNAM_SCANNER_sc_names_string.chars [190]), 1, 0},
/* 59 */ {&(DYNAM_SCANNER_sc_names_string.chars [192]), 1, 0},
/* 60 */ {&(DYNAM_SCANNER_sc_names_string.chars [194]), 1, 0},
/* 61 */ {&(DYNAM_SCANNER_sc_names_string.chars [196]), 1, 41},
/* 62 */ {&(DYNAM_SCANNER_sc_names_string.chars [198]), 1, 0},
/* 63 */ {&(DYNAM_SCANNER_sc_names_string.chars [200]), 1, 0},
/* 64 */ {&(DYNAM_SCANNER_sc_names_string.chars [202]), 1, 0},
/* 65 */ {&(DYNAM_SCANNER_sc_names_string.chars [204]), 1, 0},
/* 66 */ {&(DYNAM_SCANNER_sc_names_string.chars [206]), 1, 0},
/* 67 */ {&(DYNAM_SCANNER_sc_names_string.chars [208]), 1, 0},
/* 68 */ {&(DYNAM_SCANNER_sc_names_string.chars [210]), 1, 0},
/* 69 */ {&(DYNAM_SCANNER_sc_names_string.chars [212]), 5, 0},
/* 70 */ {&(DYNAM_SCANNER_sc_names_string.chars [218]), 5, 17},
/* 71 */ {&(DYNAM_SCANNER_sc_names_string.chars [224]), 8, 0},
/* 72 */ {&(DYNAM_SCANNER_sc_names_string.chars [233]), 7, 19},
/* 73 */ {&(DYNAM_SCANNER_sc_names_string.chars [241]), 5, 0},
} /* End DYNAM_SCANNER_sc_names_table */;


static SXINT DYNAM_SCANNER_sc_names_to_char_sets [132] = {
/* 0 */ 0, 0, 0, 1, 2, 3, 4, 5, 
/* 8 */ 6, 7, 8, 9, 10, 11, 12, 13, 
/* 16 */ 14, 15, 16, 17, 18, 19, 20, 21, 
/* 24 */ 22, 23, 24, 25, 26, 27, 28, 29, 
/* 32 */ 30, 31, 32, 33, 34, 35, 36, 37, 
/* 40 */ 38, 39, 40, 41, 42, 43, 44, 45, 
/* 48 */ 46, 47, 48, 49, 50, 51, 52, 53, 
/* 56 */ 54, 55, 56, 57, 58, 59, 60, 61, 
/* 64 */ 62, 63, 64, 65, 66, 67, 68, 69, 
/* 72 */ 70, 71, 
} /* End DYNAM_SCANNER_sc_names_to_char_sets */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_0 [10] = {257 /* size */,
 /* 1 */ 0X2, 
} /* End DYNAM_SCANNER_char_sets_line_0 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_1 [10] = {257 /* size */,
 /* 1 */ 0XFFFFFFFE, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 
 /* 5 */ 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_1 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_2 [10] = {257 /* size */,
 /* 1 */ 0X4, 
} /* End DYNAM_SCANNER_char_sets_line_2 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_3 [10] = {257 /* size */,
 /* 1 */ 0X8, 
} /* End DYNAM_SCANNER_char_sets_line_3 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_4 [10] = {257 /* size */,
 /* 1 */ 0X10, 
} /* End DYNAM_SCANNER_char_sets_line_4 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_5 [10] = {257 /* size */,
 /* 1 */ 0X20, 
} /* End DYNAM_SCANNER_char_sets_line_5 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_6 [10] = {257 /* size */,
 /* 1 */ 0X40, 
} /* End DYNAM_SCANNER_char_sets_line_6 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_7 [10] = {257 /* size */,
 /* 1 */ 0X80, 
} /* End DYNAM_SCANNER_char_sets_line_7 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_8 [10] = {257 /* size */,
 /* 1 */ 0X100, 
} /* End DYNAM_SCANNER_char_sets_line_8 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_9 [10] = {257 /* size */,
 /* 1 */ 0X200, 
} /* End DYNAM_SCANNER_char_sets_line_9 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_10 [10] = {257 /* size */,
 /* 1 */ 0X400, 
} /* End DYNAM_SCANNER_char_sets_line_10 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_11 [10] = {257 /* size */,
 /* 1 */ 0X800, 
} /* End DYNAM_SCANNER_char_sets_line_11 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_12 [10] = {257 /* size */,
 /* 1 */ 0X800, 
} /* End DYNAM_SCANNER_char_sets_line_12 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_13 [10] = {257 /* size */,
 /* 1 */ 0X800, 
} /* End DYNAM_SCANNER_char_sets_line_13 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_14 [10] = {257 /* size */,
 /* 1 */ 0X1000, 
} /* End DYNAM_SCANNER_char_sets_line_14 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_15 [10] = {257 /* size */,
 /* 1 */ 0X2000, 
} /* End DYNAM_SCANNER_char_sets_line_15 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_16 [10] = {257 /* size */,
 /* 1 */ 0X4000, 
} /* End DYNAM_SCANNER_char_sets_line_16 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_17 [10] = {257 /* size */,
 /* 1 */ 0X8000, 
} /* End DYNAM_SCANNER_char_sets_line_17 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_18 [10] = {257 /* size */,
 /* 1 */ 0X10000, 
} /* End DYNAM_SCANNER_char_sets_line_18 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_19 [10] = {257 /* size */,
 /* 1 */ 0X20000, 
} /* End DYNAM_SCANNER_char_sets_line_19 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_20 [10] = {257 /* size */,
 /* 1 */ 0X40000, 
} /* End DYNAM_SCANNER_char_sets_line_20 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_21 [10] = {257 /* size */,
 /* 1 */ 0X80000, 
} /* End DYNAM_SCANNER_char_sets_line_21 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_22 [10] = {257 /* size */,
 /* 1 */ 0X100000, 
} /* End DYNAM_SCANNER_char_sets_line_22 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_23 [10] = {257 /* size */,
 /* 1 */ 0X200000, 
} /* End DYNAM_SCANNER_char_sets_line_23 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_24 [10] = {257 /* size */,
 /* 1 */ 0X400000, 
} /* End DYNAM_SCANNER_char_sets_line_24 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_25 [10] = {257 /* size */,
 /* 1 */ 0X800000, 
} /* End DYNAM_SCANNER_char_sets_line_25 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_26 [10] = {257 /* size */,
 /* 1 */ 0X1000000, 
} /* End DYNAM_SCANNER_char_sets_line_26 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_27 [10] = {257 /* size */,
 /* 1 */ 0X2000000, 
} /* End DYNAM_SCANNER_char_sets_line_27 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_28 [10] = {257 /* size */,
 /* 1 */ 0X4000000, 
} /* End DYNAM_SCANNER_char_sets_line_28 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_29 [10] = {257 /* size */,
 /* 1 */ 0X8000000, 
} /* End DYNAM_SCANNER_char_sets_line_29 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_30 [10] = {257 /* size */,
 /* 1 */ 0X10000000, 
} /* End DYNAM_SCANNER_char_sets_line_30 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_31 [10] = {257 /* size */,
 /* 1 */ 0X20000000, 
} /* End DYNAM_SCANNER_char_sets_line_31 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_32 [10] = {257 /* size */,
 /* 1 */ 0X40000000, 
} /* End DYNAM_SCANNER_char_sets_line_32 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_33 [10] = {257 /* size */,
 /* 1 */ 0X80000000, 
} /* End DYNAM_SCANNER_char_sets_line_33 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_34 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_34 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_35 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X2, 
} /* End DYNAM_SCANNER_char_sets_line_35 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_36 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X8, 
} /* End DYNAM_SCANNER_char_sets_line_36 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_37 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0X0, 
 /* 5 */ 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_37 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_38 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0XFFFFFFC, 0XFFFFFFC, 
} /* End DYNAM_SCANNER_char_sets_line_38 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_39 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0XFFFFFFC, 
} /* End DYNAM_SCANNER_char_sets_line_39 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_40 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0XFFFFFFC, 
} /* End DYNAM_SCANNER_char_sets_line_40 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_41 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X4, 0X4, 
} /* End DYNAM_SCANNER_char_sets_line_41 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_42 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X8, 0X8, 
} /* End DYNAM_SCANNER_char_sets_line_42 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_43 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X10, 0X10, 
} /* End DYNAM_SCANNER_char_sets_line_43 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_44 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X20, 0X20, 
} /* End DYNAM_SCANNER_char_sets_line_44 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_45 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X40, 0X40, 
} /* End DYNAM_SCANNER_char_sets_line_45 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_46 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X80, 0X80, 
} /* End DYNAM_SCANNER_char_sets_line_46 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_47 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X100, 0X100, 
} /* End DYNAM_SCANNER_char_sets_line_47 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_48 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X200, 0X200, 
} /* End DYNAM_SCANNER_char_sets_line_48 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_49 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X400, 0X400, 
} /* End DYNAM_SCANNER_char_sets_line_49 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_50 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X800, 0X800, 
} /* End DYNAM_SCANNER_char_sets_line_50 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_51 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X1000, 0X1000, 
} /* End DYNAM_SCANNER_char_sets_line_51 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_52 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X2000, 0X2000, 
} /* End DYNAM_SCANNER_char_sets_line_52 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_53 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X4000, 0X4000, 
} /* End DYNAM_SCANNER_char_sets_line_53 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_54 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X8000, 0X8000, 
} /* End DYNAM_SCANNER_char_sets_line_54 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_55 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X10000, 0X10000, 
} /* End DYNAM_SCANNER_char_sets_line_55 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_56 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X20000, 0X20000, 
} /* End DYNAM_SCANNER_char_sets_line_56 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_57 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X40000, 0X40000, 
} /* End DYNAM_SCANNER_char_sets_line_57 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_58 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X80000, 0X80000, 
} /* End DYNAM_SCANNER_char_sets_line_58 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_59 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X100000, 0X100000, 
} /* End DYNAM_SCANNER_char_sets_line_59 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_60 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X200000, 0X200000, 
} /* End DYNAM_SCANNER_char_sets_line_60 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_61 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X400000, 0X400000, 
} /* End DYNAM_SCANNER_char_sets_line_61 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_62 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X800000, 0X800000, 
} /* End DYNAM_SCANNER_char_sets_line_62 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_63 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X1000000, 0X1000000, 
} /* End DYNAM_SCANNER_char_sets_line_63 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_64 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X2000000, 0X2000000, 
} /* End DYNAM_SCANNER_char_sets_line_64 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_65 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X4000000, 0X4000000, 
} /* End DYNAM_SCANNER_char_sets_line_65 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_66 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X8000000, 0X8000000, 
} /* End DYNAM_SCANNER_char_sets_line_66 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_67 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X7FE0000, 
} /* End DYNAM_SCANNER_char_sets_line_67 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_68 [10] = {257 /* size */,
 /* 1 */ 0X0, 0XFFFFFFFC, 0XFFFFFFFF, 0XFFFFFFFF, 
} /* End DYNAM_SCANNER_char_sets_line_68 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_69 [10] = {257 /* size */,
 /* 1 */ 0X0, 0XCFFFFF64, 0XFFFFFFFD, 0X7FFFFFFF, 
} /* End DYNAM_SCANNER_char_sets_line_69 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_70 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X7FFFFFFE, 0XFFFFFFFF, 0XFFFFFFFF, 
} /* End DYNAM_SCANNER_char_sets_line_70 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_71 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X1FE0000, 
} /* End DYNAM_SCANNER_char_sets_line_71 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_72 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X20000, 
} /* End DYNAM_SCANNER_char_sets_line_72 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_73 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X20000, 
} /* End DYNAM_SCANNER_char_sets_line_73 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_74 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0X80000000, 
} /* End DYNAM_SCANNER_char_sets_line_74 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_75 [10] = {257 /* size */,
 /* 1 */ 0XFFFFF7FE, 0XFFFFFFFF, 0XFFFFFFFF, 0X7FFFFFFF, 
 /* 5 */ 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_75 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_76 [10] = {257 /* size */,
 /* 1 */ 0XFFFFF7FE, 0XDFFFFFFF, 0XFFFFFFFF, 0X7FFFFFFF, 
 /* 5 */ 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_76 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_77 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0X80000000, 
} /* End DYNAM_SCANNER_char_sets_line_77 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_78 [10] = {257 /* size */,
 /* 1 */ 0XFFFFF7FE, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 
 /* 5 */ 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_78 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_79 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X800, 
} /* End DYNAM_SCANNER_char_sets_line_79 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_80 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X80000000, 
} /* End DYNAM_SCANNER_char_sets_line_80 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_81 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X20000000, 
} /* End DYNAM_SCANNER_char_sets_line_81 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_82 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X80000000, 
} /* End DYNAM_SCANNER_char_sets_line_82 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_83 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X20000000, 
} /* End DYNAM_SCANNER_char_sets_line_83 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_84 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X10, 
} /* End DYNAM_SCANNER_char_sets_line_84 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_85 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X20000000, 
} /* End DYNAM_SCANNER_char_sets_line_85 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_86 [10] = {257 /* size */,
 /* 1 */ 0XFFFFF7FE, 0XFFFFFFF7, 0XDFFFFFFF, 0XFFFFFFFF, 
 /* 5 */ 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_86 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_87 [10] = {257 /* size */,
 /* 1 */ 0XFFFFF7FE, 0XDFFFFFF7, 0XDFFFFFFF, 0XFFFFFFFF, 
 /* 5 */ 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_87 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_88 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0X8000, 
} /* End DYNAM_SCANNER_char_sets_line_88 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_89 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0X8, 
} /* End DYNAM_SCANNER_char_sets_line_89 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_90 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0X200000, 
} /* End DYNAM_SCANNER_char_sets_line_90 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_91 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0X800000, 
} /* End DYNAM_SCANNER_char_sets_line_91 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_92 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0X80, 
} /* End DYNAM_SCANNER_char_sets_line_92 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_93 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0X80000, 
} /* End DYNAM_SCANNER_char_sets_line_93 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_94 [10] = {257 /* size */,
 /* 1 */ 0XFFFFF7FE, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF577F77, 
 /* 5 */ 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_94 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_95 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X20000000, 
} /* End DYNAM_SCANNER_char_sets_line_95 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_96 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X20000000, 
} /* End DYNAM_SCANNER_char_sets_line_96 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_97 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X0, 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_97 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_98 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X40, 
} /* End DYNAM_SCANNER_char_sets_line_98 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_99 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X0, 0X2, 
} /* End DYNAM_SCANNER_char_sets_line_99 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_100 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X80, 
} /* End DYNAM_SCANNER_char_sets_line_100 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_101 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X40000000, 
} /* End DYNAM_SCANNER_char_sets_line_101 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_102 [10] = {257 /* size */,
 /* 1 */ 0X0, 0X10000000, 
} /* End DYNAM_SCANNER_char_sets_line_102 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_103 [10] = {257 /* size */,
 /* 1 */ 0X1, 
} /* End DYNAM_SCANNER_char_sets_line_103 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_104 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_104 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_105 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_105 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_106 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_106 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_107 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_107 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_108 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_108 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_109 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_109 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_110 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_110 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_111 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_111 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_112 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_112 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_113 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_113 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_114 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_114 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_115 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_115 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_116 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_116 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_117 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_117 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_118 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_118 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_119 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_119 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_120 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_120 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_121 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_121 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_122 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_122 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_123 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_123 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_124 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_124 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_125 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_125 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_126 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_126 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_127 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_127 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_128 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_128 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_129 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_129 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_130 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_130 */;


static  SXBA_ELT DYNAM_SCANNER_char_sets_line_131 [10] = {257 /* size */,
} /* End DYNAM_SCANNER_char_sets_line_131 */;


static  SXBA DYNAM_SCANNER_char_sets [132] = {
DYNAM_SCANNER_char_sets_line_0,
DYNAM_SCANNER_char_sets_line_1,
DYNAM_SCANNER_char_sets_line_2,
DYNAM_SCANNER_char_sets_line_3,
DYNAM_SCANNER_char_sets_line_4,
DYNAM_SCANNER_char_sets_line_5,
DYNAM_SCANNER_char_sets_line_6,
DYNAM_SCANNER_char_sets_line_7,
DYNAM_SCANNER_char_sets_line_8,
DYNAM_SCANNER_char_sets_line_9,
DYNAM_SCANNER_char_sets_line_10,
DYNAM_SCANNER_char_sets_line_11,
DYNAM_SCANNER_char_sets_line_12,
DYNAM_SCANNER_char_sets_line_13,
DYNAM_SCANNER_char_sets_line_14,
DYNAM_SCANNER_char_sets_line_15,
DYNAM_SCANNER_char_sets_line_16,
DYNAM_SCANNER_char_sets_line_17,
DYNAM_SCANNER_char_sets_line_18,
DYNAM_SCANNER_char_sets_line_19,
DYNAM_SCANNER_char_sets_line_20,
DYNAM_SCANNER_char_sets_line_21,
DYNAM_SCANNER_char_sets_line_22,
DYNAM_SCANNER_char_sets_line_23,
DYNAM_SCANNER_char_sets_line_24,
DYNAM_SCANNER_char_sets_line_25,
DYNAM_SCANNER_char_sets_line_26,
DYNAM_SCANNER_char_sets_line_27,
DYNAM_SCANNER_char_sets_line_28,
DYNAM_SCANNER_char_sets_line_29,
DYNAM_SCANNER_char_sets_line_30,
DYNAM_SCANNER_char_sets_line_31,
DYNAM_SCANNER_char_sets_line_32,
DYNAM_SCANNER_char_sets_line_33,
DYNAM_SCANNER_char_sets_line_34,
DYNAM_SCANNER_char_sets_line_35,
DYNAM_SCANNER_char_sets_line_36,
DYNAM_SCANNER_char_sets_line_37,
DYNAM_SCANNER_char_sets_line_38,
DYNAM_SCANNER_char_sets_line_39,
DYNAM_SCANNER_char_sets_line_40,
DYNAM_SCANNER_char_sets_line_41,
DYNAM_SCANNER_char_sets_line_42,
DYNAM_SCANNER_char_sets_line_43,
DYNAM_SCANNER_char_sets_line_44,
DYNAM_SCANNER_char_sets_line_45,
DYNAM_SCANNER_char_sets_line_46,
DYNAM_SCANNER_char_sets_line_47,
DYNAM_SCANNER_char_sets_line_48,
DYNAM_SCANNER_char_sets_line_49,
DYNAM_SCANNER_char_sets_line_50,
DYNAM_SCANNER_char_sets_line_51,
DYNAM_SCANNER_char_sets_line_52,
DYNAM_SCANNER_char_sets_line_53,
DYNAM_SCANNER_char_sets_line_54,
DYNAM_SCANNER_char_sets_line_55,
DYNAM_SCANNER_char_sets_line_56,
DYNAM_SCANNER_char_sets_line_57,
DYNAM_SCANNER_char_sets_line_58,
DYNAM_SCANNER_char_sets_line_59,
DYNAM_SCANNER_char_sets_line_60,
DYNAM_SCANNER_char_sets_line_61,
DYNAM_SCANNER_char_sets_line_62,
DYNAM_SCANNER_char_sets_line_63,
DYNAM_SCANNER_char_sets_line_64,
DYNAM_SCANNER_char_sets_line_65,
DYNAM_SCANNER_char_sets_line_66,
DYNAM_SCANNER_char_sets_line_67,
DYNAM_SCANNER_char_sets_line_68,
DYNAM_SCANNER_char_sets_line_69,
DYNAM_SCANNER_char_sets_line_70,
DYNAM_SCANNER_char_sets_line_71,
DYNAM_SCANNER_char_sets_line_72,
DYNAM_SCANNER_char_sets_line_73,
DYNAM_SCANNER_char_sets_line_74,
DYNAM_SCANNER_char_sets_line_75,
DYNAM_SCANNER_char_sets_line_76,
DYNAM_SCANNER_char_sets_line_77,
DYNAM_SCANNER_char_sets_line_78,
DYNAM_SCANNER_char_sets_line_79,
DYNAM_SCANNER_char_sets_line_80,
DYNAM_SCANNER_char_sets_line_81,
DYNAM_SCANNER_char_sets_line_82,
DYNAM_SCANNER_char_sets_line_83,
DYNAM_SCANNER_char_sets_line_84,
DYNAM_SCANNER_char_sets_line_85,
DYNAM_SCANNER_char_sets_line_86,
DYNAM_SCANNER_char_sets_line_87,
DYNAM_SCANNER_char_sets_line_88,
DYNAM_SCANNER_char_sets_line_89,
DYNAM_SCANNER_char_sets_line_90,
DYNAM_SCANNER_char_sets_line_91,
DYNAM_SCANNER_char_sets_line_92,
DYNAM_SCANNER_char_sets_line_93,
DYNAM_SCANNER_char_sets_line_94,
DYNAM_SCANNER_char_sets_line_95,
DYNAM_SCANNER_char_sets_line_96,
DYNAM_SCANNER_char_sets_line_97,
DYNAM_SCANNER_char_sets_line_98,
DYNAM_SCANNER_char_sets_line_99,
DYNAM_SCANNER_char_sets_line_100,
DYNAM_SCANNER_char_sets_line_101,
DYNAM_SCANNER_char_sets_line_102,
DYNAM_SCANNER_char_sets_line_103,
DYNAM_SCANNER_char_sets_line_104,
DYNAM_SCANNER_char_sets_line_105,
DYNAM_SCANNER_char_sets_line_106,
DYNAM_SCANNER_char_sets_line_107,
DYNAM_SCANNER_char_sets_line_108,
DYNAM_SCANNER_char_sets_line_109,
DYNAM_SCANNER_char_sets_line_110,
DYNAM_SCANNER_char_sets_line_111,
DYNAM_SCANNER_char_sets_line_112,
DYNAM_SCANNER_char_sets_line_113,
DYNAM_SCANNER_char_sets_line_114,
DYNAM_SCANNER_char_sets_line_115,
DYNAM_SCANNER_char_sets_line_116,
DYNAM_SCANNER_char_sets_line_117,
DYNAM_SCANNER_char_sets_line_118,
DYNAM_SCANNER_char_sets_line_119,
DYNAM_SCANNER_char_sets_line_120,
DYNAM_SCANNER_char_sets_line_121,
DYNAM_SCANNER_char_sets_line_122,
DYNAM_SCANNER_char_sets_line_123,
DYNAM_SCANNER_char_sets_line_124,
DYNAM_SCANNER_char_sets_line_125,
DYNAM_SCANNER_char_sets_line_126,
DYNAM_SCANNER_char_sets_line_127,
DYNAM_SCANNER_char_sets_line_128,
DYNAM_SCANNER_char_sets_line_129,
DYNAM_SCANNER_char_sets_line_130,
DYNAM_SCANNER_char_sets_line_131,
} /* End DYNAM_SCANNER_char_sets */;


static struct {
	struct sxword_string	*next;
	char	chars [21];
} DYNAM_SCANNER_abbrev_names_string = {
NULL,
"\000\000NUMBER_NORMAL_FORM\000"} /* End DYNAM_SCANNER_abbrev_names_string */;


static SXUINT DYNAM_SCANNER_abbrev_names_head [] = {
/* 0 */ 1, 0, 0, 0, 0, 0, 0, 0, 
/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 16 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 24 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 32 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 40 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 48 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 56 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 64 */ 0, 0, 2, 
} /* End DYNAM_SCANNER_abbrev_names_head */;


static struct sxword_table DYNAM_SCANNER_abbrev_names_table [50] = {
/* 0 */ {&(DYNAM_SCANNER_abbrev_names_string.chars [0]), 0, 0},
/* 1 */ {&(DYNAM_SCANNER_abbrev_names_string.chars [1]), 0, 0},
/* 2 */ {&(DYNAM_SCANNER_abbrev_names_string.chars [2]), 18, 0},
} /* End DYNAM_SCANNER_abbrev_names_table */;


static struct abbrev_to_attr DYNAM_SCANNER_abbrev_to_attr [51] = {
/* 0 */ {0, 0, 0}, {0, 0, 0}, {0, 9, 0}, 
} /* End struct abbrev_to_attr DYNAM_SCANNER_abbrev_to_attr */;


static struct {
	struct sxword_string	*next;
	char	chars [102];
} DYNAM_SCANNER_reg_exp_names_string = {
NULL,
"\000\000COMMENTS\000%LHS_NON_TERMINAL\000%NON_TERMINAL\000%TERMINAL\000%GENERIC_TE\
RMINAL\000%ACTION\000%PREDICATE\000\"=\"\000\";\"\000EOF\000"} /* End DYNAM_SCANNER_reg_exp_names_string */;


static SXUINT DYNAM_SCANNER_reg_exp_names_head [] = {
/* 0 */ 1, 0, 0, 0, 0, 0, 0, 0, 
/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 16 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 24 */ 0, 3, 0, 0, 7, 0, 0, 0, 
/* 32 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 40 */ 0, 0, 0, 0, 0, 0, 10, 0, 
/* 48 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 56 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 64 */ 4, 0, 0, 0, 0, 11, 0, 0, 
/* 72 */ 0, 5, 0, 0, 0, 9, 0, 0, 
/* 80 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 88 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 96 */ 0, 0, 0, 0, 0, 0, 0, 6, 
/* 104 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 112 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 120 */ 2, 0, 0, 0, 8, 0, 0, 0, 
/* 128 */ 0, 0, 0, 
} /* End DYNAM_SCANNER_reg_exp_names_head */;


static struct sxword_table DYNAM_SCANNER_reg_exp_names_table [100] = {
/* 0 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [0]), 0, 0},
/* 1 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [1]), 0, 0},
/* 2 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [2]), 8, 0},
/* 3 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [11]), 17, 0},
/* 4 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [29]), 13, 0},
/* 5 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [43]), 9, 0},
/* 6 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [53]), 17, 0},
/* 7 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [71]), 7, 0},
/* 8 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [79]), 10, 0},
/* 9 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [90]), 3, 0},
/* 10 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [94]), 3, 0},
/* 11 */ {&(DYNAM_SCANNER_reg_exp_names_string.chars [98]), 3, 0},
} /* End DYNAM_SCANNER_reg_exp_names_table */;


static struct reg_exp_to_attr DYNAM_SCANNER_reg_exp_to_attr [101] = {
/* 0 */ {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {10, -1, 0, 1, 2, 1, 1}, {26, -1, 2, 14, 1, 0, 1}, 
/* 4 */ {33, -1, 4, 12, 1, 0, 1}, {38, -1, 7, 9, 0, 1, 1}, {75, -1, 8, 8, 0, 5, 1}, {82, -1, 6, 10, 0, 0, 1}, 
/* 8 */ {93, 0, 5, 11, 0, 0, 1}, {104, -1, 3, 13, 1, 0, 1}, {109, -1, 1, 15, 1, 0, 1}, {113, -1, 9, 7, 0, 0, 1}, 
} /* End struct reg_exp_to_attr DYNAM_SCANNER_reg_exp_to_attr */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_0 [5] = {100 /* size */,
 /* 1 */ 0X7FC, 
} /* End DYNAM_SCANNER_context_sets_line_0 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_1 [5] = {100 /* size */,
 /* 1 */ 0XFFC, 
} /* End DYNAM_SCANNER_context_sets_line_1 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_2 [5] = {100 /* size */,
 /* 1 */ 0X4, 
} /* End DYNAM_SCANNER_context_sets_line_2 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_3 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_3 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_4 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_4 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_5 [5] = {100 /* size */,
 /* 1 */ 0X4, 
} /* End DYNAM_SCANNER_context_sets_line_5 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_6 [5] = {100 /* size */,
 /* 1 */ 0X4, 
} /* End DYNAM_SCANNER_context_sets_line_6 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_7 [5] = {100 /* size */,
 /* 1 */ 0X4, 
} /* End DYNAM_SCANNER_context_sets_line_7 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_8 [5] = {100 /* size */,
 /* 1 */ 0X4, 
} /* End DYNAM_SCANNER_context_sets_line_8 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_9 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_9 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_10 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_10 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_11 [5] = {100 /* size */,
 /* 1 */ 0X800, 
} /* End DYNAM_SCANNER_context_sets_line_11 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_12 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_12 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_13 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_13 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_14 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_14 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_15 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_15 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_16 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_16 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_17 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_17 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_18 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_18 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_19 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_19 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_20 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_20 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_21 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_21 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_22 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_22 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_23 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_23 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_24 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_24 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_25 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_25 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_26 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_26 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_27 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_27 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_28 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_28 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_29 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_29 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_30 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_30 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_31 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_31 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_32 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_32 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_33 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_33 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_34 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_34 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_35 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_35 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_36 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_36 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_37 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_37 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_38 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_38 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_39 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_39 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_40 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_40 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_41 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_41 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_42 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_42 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_43 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_43 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_44 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_44 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_45 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_45 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_46 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_46 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_47 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_47 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_48 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_48 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_49 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_49 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_50 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_50 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_51 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_51 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_52 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_52 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_53 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_53 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_54 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_54 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_55 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_55 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_56 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_56 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_57 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_57 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_58 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_58 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_59 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_59 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_60 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_60 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_61 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_61 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_62 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_62 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_63 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_63 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_64 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_64 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_65 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_65 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_66 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_66 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_67 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_67 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_68 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_68 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_69 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_69 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_70 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_70 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_71 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_71 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_72 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_72 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_73 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_73 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_74 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_74 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_75 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_75 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_76 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_76 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_77 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_77 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_78 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_78 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_79 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_79 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_80 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_80 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_81 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_81 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_82 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_82 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_83 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_83 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_84 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_84 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_85 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_85 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_86 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_86 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_87 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_87 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_88 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_88 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_89 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_89 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_90 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_90 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_91 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_91 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_92 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_92 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_93 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_93 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_94 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_94 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_95 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_95 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_96 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_96 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_97 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_97 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_98 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_98 */;


static  SXBA_ELT DYNAM_SCANNER_context_sets_line_99 [5] = {100 /* size */,
} /* End DYNAM_SCANNER_context_sets_line_99 */;


static  SXBA DYNAM_SCANNER_context_sets [100] = {
DYNAM_SCANNER_context_sets_line_0,
DYNAM_SCANNER_context_sets_line_1,
DYNAM_SCANNER_context_sets_line_2,
DYNAM_SCANNER_context_sets_line_3,
DYNAM_SCANNER_context_sets_line_4,
DYNAM_SCANNER_context_sets_line_5,
DYNAM_SCANNER_context_sets_line_6,
DYNAM_SCANNER_context_sets_line_7,
DYNAM_SCANNER_context_sets_line_8,
DYNAM_SCANNER_context_sets_line_9,
DYNAM_SCANNER_context_sets_line_10,
DYNAM_SCANNER_context_sets_line_11,
DYNAM_SCANNER_context_sets_line_12,
DYNAM_SCANNER_context_sets_line_13,
DYNAM_SCANNER_context_sets_line_14,
DYNAM_SCANNER_context_sets_line_15,
DYNAM_SCANNER_context_sets_line_16,
DYNAM_SCANNER_context_sets_line_17,
DYNAM_SCANNER_context_sets_line_18,
DYNAM_SCANNER_context_sets_line_19,
DYNAM_SCANNER_context_sets_line_20,
DYNAM_SCANNER_context_sets_line_21,
DYNAM_SCANNER_context_sets_line_22,
DYNAM_SCANNER_context_sets_line_23,
DYNAM_SCANNER_context_sets_line_24,
DYNAM_SCANNER_context_sets_line_25,
DYNAM_SCANNER_context_sets_line_26,
DYNAM_SCANNER_context_sets_line_27,
DYNAM_SCANNER_context_sets_line_28,
DYNAM_SCANNER_context_sets_line_29,
DYNAM_SCANNER_context_sets_line_30,
DYNAM_SCANNER_context_sets_line_31,
DYNAM_SCANNER_context_sets_line_32,
DYNAM_SCANNER_context_sets_line_33,
DYNAM_SCANNER_context_sets_line_34,
DYNAM_SCANNER_context_sets_line_35,
DYNAM_SCANNER_context_sets_line_36,
DYNAM_SCANNER_context_sets_line_37,
DYNAM_SCANNER_context_sets_line_38,
DYNAM_SCANNER_context_sets_line_39,
DYNAM_SCANNER_context_sets_line_40,
DYNAM_SCANNER_context_sets_line_41,
DYNAM_SCANNER_context_sets_line_42,
DYNAM_SCANNER_context_sets_line_43,
DYNAM_SCANNER_context_sets_line_44,
DYNAM_SCANNER_context_sets_line_45,
DYNAM_SCANNER_context_sets_line_46,
DYNAM_SCANNER_context_sets_line_47,
DYNAM_SCANNER_context_sets_line_48,
DYNAM_SCANNER_context_sets_line_49,
DYNAM_SCANNER_context_sets_line_50,
DYNAM_SCANNER_context_sets_line_51,
DYNAM_SCANNER_context_sets_line_52,
DYNAM_SCANNER_context_sets_line_53,
DYNAM_SCANNER_context_sets_line_54,
DYNAM_SCANNER_context_sets_line_55,
DYNAM_SCANNER_context_sets_line_56,
DYNAM_SCANNER_context_sets_line_57,
DYNAM_SCANNER_context_sets_line_58,
DYNAM_SCANNER_context_sets_line_59,
DYNAM_SCANNER_context_sets_line_60,
DYNAM_SCANNER_context_sets_line_61,
DYNAM_SCANNER_context_sets_line_62,
DYNAM_SCANNER_context_sets_line_63,
DYNAM_SCANNER_context_sets_line_64,
DYNAM_SCANNER_context_sets_line_65,
DYNAM_SCANNER_context_sets_line_66,
DYNAM_SCANNER_context_sets_line_67,
DYNAM_SCANNER_context_sets_line_68,
DYNAM_SCANNER_context_sets_line_69,
DYNAM_SCANNER_context_sets_line_70,
DYNAM_SCANNER_context_sets_line_71,
DYNAM_SCANNER_context_sets_line_72,
DYNAM_SCANNER_context_sets_line_73,
DYNAM_SCANNER_context_sets_line_74,
DYNAM_SCANNER_context_sets_line_75,
DYNAM_SCANNER_context_sets_line_76,
DYNAM_SCANNER_context_sets_line_77,
DYNAM_SCANNER_context_sets_line_78,
DYNAM_SCANNER_context_sets_line_79,
DYNAM_SCANNER_context_sets_line_80,
DYNAM_SCANNER_context_sets_line_81,
DYNAM_SCANNER_context_sets_line_82,
DYNAM_SCANNER_context_sets_line_83,
DYNAM_SCANNER_context_sets_line_84,
DYNAM_SCANNER_context_sets_line_85,
DYNAM_SCANNER_context_sets_line_86,
DYNAM_SCANNER_context_sets_line_87,
DYNAM_SCANNER_context_sets_line_88,
DYNAM_SCANNER_context_sets_line_89,
DYNAM_SCANNER_context_sets_line_90,
DYNAM_SCANNER_context_sets_line_91,
DYNAM_SCANNER_context_sets_line_92,
DYNAM_SCANNER_context_sets_line_93,
DYNAM_SCANNER_context_sets_line_94,
DYNAM_SCANNER_context_sets_line_95,
DYNAM_SCANNER_context_sets_line_96,
DYNAM_SCANNER_context_sets_line_97,
DYNAM_SCANNER_context_sets_line_98,
DYNAM_SCANNER_context_sets_line_99,
} /* End DYNAM_SCANNER_context_sets */;


static struct item_to_attr DYNAM_SCANNER_item_to_attr [501] = {
/* 0 */ {0, 0, 0, 0, 0}, {-2, -10, 0, 0, 4}, {-2, 72, 0, 0, 1}, {-2, -2, 0, 0, 8}, 
/* 4 */ {-2, -5, 0, 0, 4}, {-2, 67, 0, 0, 1}, {-2, 73, 0, 0, 1}, {-2, -2, 0, 0, 8}, 
/* 8 */ {-2, 67, 0, 0, 1}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {2, 35, 0, 1, 1}, 
/* 12 */ {2, 10, 0, 1, 1}, {2, 14, 0, 1, 1}, {2, 15, 0, 1, 1}, {2, 77, 0, 1, 1}, 
/* 16 */ {2, 75, 0, 1, 1}, {2, 13, 0, 1, 1}, {2, 76, 0, 1, 1}, {2, 13, 0, 1, 1}, 
/* 20 */ {2, 74, 0, 1, 1}, {2, 13, 0, 1, 1}, {2, 79, 0, 1, 1}, {2, 78, 0, 1, 1}, 
/* 24 */ {2, 13, 0, 1, 1}, {2, 0, 0, 0, 2}, {0, 0, 0, 0, 0}, {3, 81, 0, 0, 1}, 
/* 28 */ {3, -3, 0, 0, 8}, {3, -3, 0, 0, 4}, {3, 70, 0, 0, 1}, {3, 80, 0, 0, 1}, 
/* 32 */ {3, 0, 0, 0, 2}, {0, 0, 0, 0, 0}, {4, 83, 0, 0, 1}, {4, 70, 0, 0, 1}, 
/* 36 */ {4, 82, 0, 0, 1}, {4, 0, 0, 0, 2}, {0, 0, 0, 0, 0}, {5, 84, 0, 1, 1}, 
/* 40 */ {5, 68, 0, 0, 1}, {5, 69, 0, 0, 1}, {5, 68, 0, 0, 1}, {5, 36, 0, 1, 1}, 
/* 44 */ {5, 86, 0, 0, 1}, {5, 96, 0, 1, 1}, {5, 13, 0, 1, 1}, {5, 95, 0, 1, 1}, 
/* 48 */ {5, 13, 0, 1, 1}, {5, 87, 0, 0, 1}, {5, 88, 0, 1, 1}, {5, -9, 10, 0, 4}, 
/* 52 */ {5, 89, 0, 1, 1}, {5, -9, 8, 0, 4}, {5, 90, 0, 1, 1}, {5, -9, 9, 0, 4}, 
/* 56 */ {5, 91, 0, 1, 1}, {5, -9, 11, 0, 4}, {5, 92, 0, 1, 1}, {5, -9, 12, 0, 4}, 
/* 60 */ {5, 93, 0, 1, 1}, {5, -9, 13, 0, 4}, {5, -10, 0, 0, 4}, {5, 71, 0, 0, 1}, 
/* 64 */ {5, -2, 0, 0, 8}, {5, 71, 0, 0, 1}, {5, -2, 0, 0, 8}, {5, 71, 0, 0, 1}, 
/* 68 */ {5, -2, 0, 0, 8}, {5, 0, 0, 0, 4}, {5, 94, 0, 0, 1}, {5, 85, 0, 1, 1}, 
/* 72 */ {5, 13, 0, 1, 1}, {5, 36, 0, 1, 1}, {5, 0, 0, 0, 2}, {0, 0, 0, 0, 0}, 
/* 76 */ {6, 98, 0, 0, 1}, {6, 38, 0, 0, 1}, {6, 97, 0, 0, 1}, {6, 38, 0, 0, 1}, 
/* 80 */ {6, 67, 0, 0, 1}, {6, 0, 0, 0, 2}, {0, 0, 0, 0, 0}, {7, 99, 0, 0, 1}, 
/* 84 */ {7, -10, 0, 0, 4}, {7, 72, 0, 0, 1}, {7, -2, 0, 0, 8}, {7, -5, 0, 0, 4}, 
/* 88 */ {7, 67, 0, 0, 1}, {7, 73, 0, 0, 1}, {7, -2, 0, 0, 8}, {7, 67, 0, 0, 1}, 
/* 92 */ {7, 0, 0, 0, 2}, {0, 0, 0, 0, 0}, {8, 100, 0, 0, 1}, {8, -10, 0, 0, 4}, 
/* 96 */ {8, 72, 0, 0, 1}, {8, -2, 0, 0, 8}, {8, -5, 0, 0, 4}, {8, 67, 0, 0, 1}, 
/* 100 */ {8, 73, 0, 0, 1}, {8, -2, 0, 0, 8}, {8, 67, 0, 0, 1}, {8, 0, 0, 0, 2}, 
/* 104 */ {0, 0, 0, 0, 0}, {9, 101, 0, 1, 1}, {9, -4, 0, 1, 8}, {9, -4, 0, 0, 4}, 
/* 108 */ {9, 0, 0, 0, 2}, {0, 0, 0, 0, 0}, {10, 102, 0, 1, 1}, {10, 0, 0, 0, 4}, 
/* 112 */ {10, 0, 0, 0, 2}, {11, 0, 0, 0, 0}, {11, 103, 0, 1, 1}, {11, 0, 0, 0, 2}, 
/* 116 */ {0, 0, 0, 0, 0}, 
} /* End struct item_to_attr DYNAM_SCANNER_item_to_attr */;


static struct XxY_elem DYNAM_SCANNER_nfsa_display [522] = {
/* 0 */ {0, 0}, {0, 1}, {0, 6}, {0, 8}, 
/* 4 */ {5, 5}, {5, 9}, {4, 5}, {2, 3}, 
/* 8 */ {3, 2}, {3, 4}, {1, 2}, {6, 7}, 
/* 12 */ {7, 9}, {8, 8}, {8, 9}, {10, 11}, 
/* 16 */ {10, 12}, {10, 13}, {10, 14}, {10, 15}, 
/* 20 */ {10, 21}, {11, 11}, {11, 12}, {11, 13}, 
/* 24 */ {11, 14}, {11, 15}, {11, 21}, {11, 25}, 
/* 28 */ {12, 11}, {12, 12}, {12, 13}, {12, 14}, 
/* 32 */ {12, 15}, {12, 21}, {12, 25}, {13, 11}, 
/* 36 */ {13, 12}, {13, 13}, {13, 14}, {13, 15}, 
/* 40 */ {13, 21}, {13, 25}, {14, 11}, {14, 12}, 
/* 44 */ {14, 13}, {14, 14}, {14, 15}, {14, 21}, 
/* 48 */ {14, 25}, {20, 11}, {20, 12}, {20, 13}, 
/* 52 */ {20, 14}, {20, 15}, {20, 21}, {20, 25}, 
/* 56 */ {19, 19}, {19, 20}, {16, 16}, {16, 17}, 
/* 60 */ {16, 19}, {16, 20}, {18, 16}, {18, 17}, 
/* 64 */ {18, 19}, {18, 20}, {17, 17}, {17, 18}, 
/* 68 */ {15, 16}, {15, 17}, {15, 19}, {15, 20}, 
/* 72 */ {24, 11}, {24, 12}, {24, 13}, {24, 14}, 
/* 76 */ {24, 15}, {24, 21}, {24, 22}, {24, 25}, 
/* 80 */ {23, 23}, {23, 24}, {22, 23}, {22, 24}, 
/* 84 */ {21, 11}, {21, 12}, {21, 13}, {21, 14}, 
/* 88 */ {21, 15}, {21, 21}, {21, 22}, {21, 25}, 
/* 92 */ {26, 27}, {31, 32}, {30, 30}, {30, 31}, 
/* 96 */ {29, 30}, {29, 31}, {27, 28}, {28, 29}, 
/* 100 */ {33, 34}, {36, 37}, {35, 35}, {35, 36}, 
/* 104 */ {34, 35}, {34, 36}, {38, 39}, {38, 41}, 
/* 108 */ {38, 43}, {42, 42}, {42, 74}, {40, 42}, 
/* 112 */ {40, 74}, {39, 40}, {41, 42}, {41, 74}, 
/* 116 */ {73, 74}, {72, 71}, {72, 73}, {71, 72}, 
/* 120 */ {44, 44}, {44, 45}, {44, 71}, {44, 73}, 
/* 124 */ {49, 44}, {49, 45}, {49, 71}, {49, 73}, 
/* 128 */ {48, 49}, {51, 44}, {51, 45}, {51, 71}, 
/* 132 */ {51, 73}, {50, 51}, {53, 44}, {53, 45}, 
/* 136 */ {53, 71}, {53, 73}, {52, 53}, {55, 44}, 
/* 140 */ {55, 45}, {55, 71}, {55, 73}, {54, 55}, 
/* 144 */ {57, 44}, {57, 45}, {57, 71}, {57, 73}, 
/* 148 */ {56, 57}, {59, 44}, {59, 45}, {59, 71}, 
/* 152 */ {59, 73}, {58, 59}, {61, 44}, {61, 45}, 
/* 156 */ {61, 71}, {61, 73}, {60, 61}, {69, 44}, 
/* 160 */ {69, 45}, {69, 71}, {69, 73}, {67, 68}, 
/* 164 */ {68, 69}, {65, 66}, {66, 67}, {66, 69}, 
/* 168 */ {63, 64}, {64, 65}, {64, 69}, {62, 63}, 
/* 172 */ {70, 44}, {70, 45}, {70, 71}, {70, 73}, 
/* 176 */ {47, 46}, {47, 48}, {47, 50}, {47, 52}, 
/* 180 */ {47, 54}, {47, 56}, {47, 58}, {47, 60}, 
/* 184 */ {47, 62}, {47, 70}, {46, 47}, {45, 46}, 
/* 188 */ {45, 48}, {45, 50}, {45, 52}, {45, 54}, 
/* 192 */ {45, 56}, {45, 58}, {45, 60}, {45, 62}, 
/* 196 */ {45, 70}, {43, 44}, {43, 45}, {75, 76}, 
/* 200 */ {79, 78}, {79, 79}, {79, 80}, {79, 81}, 
/* 204 */ {80, 78}, {80, 79}, {80, 80}, {80, 81}, 
/* 208 */ {78, 79}, {78, 80}, {77, 78}, {77, 79}, 
/* 212 */ {77, 80}, {77, 81}, {76, 77}, {82, 83}, 
/* 216 */ {84, 85}, {85, 86}, {86, 87}, {86, 85}, 
/* 220 */ {87, 88}, {88, 92}, {88, 88}, {89, 90}, 
/* 224 */ {90, 92}, {91, 92}, {91, 91}, {83, 84}, 
/* 228 */ {83, 89}, {83, 91}, {93, 94}, {95, 96}, 
/* 232 */ {96, 97}, {97, 98}, {97, 96}, {98, 99}, 
/* 236 */ {99, 103}, {99, 99}, {100, 101}, {101, 103}, 
/* 240 */ {102, 103}, {102, 102}, {94, 95}, {94, 100}, 
/* 244 */ {94, 102}, {104, 105}, {107, 108}, {105, 106}, 
/* 248 */ {106, 107}, {109, 110}, {111, 112}, {110, 111}, 
/* 252 */ {113, 114}, {114, 115}, 
};


static struct X_elem DYNAM_SCANNER_nfsa_X_hd_0_display [258] = {
/* 0 */ {0}, {0}, {5}, {4}, {2}, {3}, {1}, {6}, 
/* 8 */ {7}, {8}, {10}, {11}, {12}, {13}, {14}, {20}, 
/* 16 */ {19}, {16}, {18}, {17}, {15}, {24}, {23}, {22}, 
/* 24 */ {21}, {26}, {31}, {30}, {29}, {27}, {28}, {33}, 
/* 32 */ {36}, {35}, {34}, {38}, {42}, {40}, {39}, {41}, 
/* 40 */ {73}, {72}, {71}, {44}, {49}, {48}, {51}, {50}, 
/* 48 */ {53}, {52}, {55}, {54}, {57}, {56}, {59}, {58}, 
/* 56 */ {61}, {60}, {69}, {67}, {68}, {65}, {66}, {63}, 
/* 64 */ {64}, {62}, {70}, {47}, {46}, {45}, {43}, {75}, 
/* 72 */ {79}, {80}, {78}, {77}, {76}, {82}, {84}, {85}, 
/* 80 */ {86}, {87}, {88}, {89}, {90}, {91}, {83}, {93}, 
/* 88 */ {95}, {96}, {97}, {98}, {99}, {100}, {101}, {102}, 
/* 96 */ {94}, {104}, {107}, {105}, {106}, {109}, {111}, {110}, 
/* 104 */ {113}, {114}, 
};


static SXINT DYNAM_SCANNER_nfsa_X_hd_0_hash_display [257] = {
/* 0 */ 1, 6, 4, 5, 3, 2, 7, 8, 
/* 8 */ 9, 0, 10, 11, 12, 13, 14, 20, 
/* 16 */ 17, 19, 18, 16, 15, 24, 23, 22, 
/* 24 */ 21, 0, 25, 29, 30, 28, 27, 26, 
/* 32 */ 0, 31, 34, 33, 32, 0, 35, 38, 
/* 40 */ 37, 39, 36, 70, 43, 69, 68, 67, 
/* 48 */ 45, 44, 47, 46, 49, 48, 51, 50, 
/* 56 */ 53, 52, 55, 54, 57, 56, 65, 63, 
/* 64 */ 64, 61, 62, 59, 60, 58, 66, 42, 
/* 72 */ 41, 40, 0, 71, 76, 75, 74, 72, 
/* 80 */ 73, 0, 77, 86, 78, 79, 80, 81, 
/* 88 */ 82, 83, 84, 85, 0, 87, 96, 88, 
/* 96 */ 89, 90, 91, 92, 93, 94, 95, 0, 
/* 104 */ 97, 99, 100, 98, 0, 101, 103, 102, 
/* 112 */ 0, 104, 105, 
};


static SXINT DYNAM_SCANNER_nfsa_X_hd_0_hash_lnk [258] /* empty */;


static X_header DYNAM_SCANNER_nfsa_X_hd_0 =
{NULL, /* name */
DYNAM_SCANNER_nfsa_X_hd_0_hash_display, /* hash_display */
DYNAM_SCANNER_nfsa_X_hd_0_hash_lnk, /* hash_lnk */
257, /* hash_size */
105, /* top */
257, /* size */
-2147483648, /* free_buckets */
0, /* has_free_buckets */
NULL, /* (*system_oflw) () */
NULL, /* (*user_oflw) () */
NULL, /* (*cmp) () */
NULL, /* (*scrmbl) () */
NULL, /* (*suppress) () */
NULL, /* (*assign) () */
NULL, /* stat_file */
0, /* is_locked */
SXTRUE, /* is_static */
DYNAM_SCANNER_nfsa_X_hd_0_display /* display */
} /* End X_header DYNAM_SCANNER_nfsa_X_hd_0 */;


static SXINT DYNAM_SCANNER_nfsa_lnk_hd_0 [258] = {
/* 0 */ 0, 3, 5, 6, 7, 9, 10, 11, 
/* 8 */ 12, 14, 20, 27, 34, 41, 48, 55, 
/* 16 */ 57, 61, 65, 67, 71, 79, 81, 83, 
/* 24 */ 91, 92, 93, 95, 97, 98, 99, 100, 
/* 32 */ 101, 103, 105, 108, 110, 112, 113, 115, 
/* 40 */ 116, 118, 119, 123, 127, 128, 132, 133, 
/* 48 */ 137, 138, 142, 143, 147, 148, 152, 153, 
/* 56 */ 157, 158, 162, 163, 164, 165, 167, 168, 
/* 64 */ 170, 171, 175, 185, 186, 196, 198, 199, 
/* 72 */ 203, 207, 209, 213, 214, 215, 216, 217, 
/* 80 */ 219, 220, 222, 223, 224, 226, 229, 230, 
/* 88 */ 231, 232, 234, 235, 237, 238, 239, 241, 
/* 96 */ 244, 245, 246, 247, 248, 249, 250, 251, 
/* 104 */ 252, 253, 
};


static SXINT DYNAM_SCANNER_nfsa_lnk_0 [522] = {
/* 0 */ 0, 0, 1, 2, 0, 4, 0, 0, 
/* 8 */ 0, 8, 0, 0, 0, 0, 13, 0, 
/* 16 */ 15, 16, 17, 18, 19, 0, 21, 22, 
/* 24 */ 23, 24, 25, 26, 0, 28, 29, 30, 
/* 32 */ 31, 32, 33, 0, 35, 36, 37, 38, 
/* 40 */ 39, 40, 0, 42, 43, 44, 45, 46, 
/* 48 */ 47, 0, 49, 50, 51, 52, 53, 54, 
/* 56 */ 0, 56, 0, 58, 59, 60, 0, 62, 
/* 64 */ 63, 64, 0, 66, 0, 68, 69, 70, 
/* 72 */ 0, 72, 73, 74, 75, 76, 77, 78, 
/* 80 */ 0, 80, 0, 82, 0, 84, 85, 86, 
/* 88 */ 87, 88, 89, 90, 0, 0, 0, 94, 
/* 96 */ 0, 96, 0, 0, 0, 0, 0, 102, 
/* 104 */ 0, 104, 0, 106, 107, 0, 109, 0, 
/* 112 */ 111, 0, 0, 114, 0, 0, 117, 0, 
/* 120 */ 0, 120, 121, 122, 0, 124, 125, 126, 
/* 128 */ 0, 0, 129, 130, 131, 0, 0, 134, 
/* 136 */ 135, 136, 0, 0, 139, 140, 141, 0, 
/* 144 */ 0, 144, 145, 146, 0, 0, 149, 150, 
/* 152 */ 151, 0, 0, 154, 155, 156, 0, 0, 
/* 160 */ 159, 160, 161, 0, 0, 0, 0, 166, 
/* 168 */ 0, 0, 169, 0, 0, 172, 173, 174, 
/* 176 */ 0, 176, 177, 178, 179, 180, 181, 182, 
/* 184 */ 183, 184, 0, 0, 187, 188, 189, 190, 
/* 192 */ 191, 192, 193, 194, 195, 0, 197, 0, 
/* 200 */ 0, 200, 201, 202, 0, 204, 205, 206, 
/* 208 */ 0, 208, 0, 210, 211, 212, 0, 0, 
/* 216 */ 0, 0, 0, 218, 0, 0, 221, 0, 
/* 224 */ 0, 0, 225, 0, 227, 228, 0, 0, 
/* 232 */ 0, 0, 233, 0, 0, 236, 0, 0, 
/* 240 */ 0, 240, 0, 242, 243, 0, 0, 0, 
/* 248 */ 0, 0, 0, 0, 0, 0, 
};


static SXINT DYNAM_SCANNER_nfsa_hash_display [521] = {
/* 0 */ 184, 0, 0, 217, 0, 0, 0, 0, 
/* 8 */ 0, 70, 60, 0, 64, 56, 0, 0, 
/* 16 */ 245, 0, 0, 0, 0, 0, 0, 0, 
/* 24 */ 213, 0, 203, 207, 0, 0, 0, 0, 
/* 32 */ 0, 18, 24, 243, 38, 45, 148, 0, 
/* 40 */ 0, 0, 0, 52, 87, 0, 0, 75, 
/* 48 */ 0, 122, 0, 199, 0, 0, 126, 0, 
/* 56 */ 131, 5, 136, 12, 141, 0, 146, 0, 
/* 64 */ 242, 0, 156, 0, 0, 0, 0, 0, 
/* 72 */ 0, 98, 161, 174, 0, 252, 0, 0, 
/* 80 */ 0, 0, 0, 0, 9, 0, 186, 0, 
/* 88 */ 223, 0, 0, 0, 0, 0, 0, 0, 
/* 96 */ 0, 82, 80, 165, 0, 0, 0, 0, 
/* 104 */ 0, 0, 0, 0, 0, 111, 114, 109, 
/* 112 */ 216, 0, 219, 0, 0, 0, 0, 0, 
/* 120 */ 0, 67, 0, 158, 0, 0, 0, 0, 
/* 128 */ 0, 0, 0, 0, 0, 0, 212, 209, 
/* 136 */ 202, 206, 0, 181, 0, 0, 0, 17, 
/* 144 */ 23, 30, 37, 44, 0, 235, 237, 0, 
/* 152 */ 0, 51, 86, 0, 0, 74, 93, 0, 
/* 160 */ 196, 0, 185, 0, 0, 0, 0, 0, 
/* 168 */ 0, 0, 13, 133, 0, 230, 0, 0, 
/* 176 */ 0, 0, 0, 0, 0, 0, 92, 0, 
/* 184 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 192 */ 228, 7, 0, 187, 0, 176, 0, 0, 
/* 200 */ 0, 0, 0, 0, 0, 0, 90, 0, 
/* 208 */ 169, 78, 246, 0, 0, 0, 0, 0, 
/* 216 */ 0, 107, 194, 0, 183, 227, 0, 0, 
/* 224 */ 0, 0, 0, 0, 0, 69, 59, 236, 
/* 232 */ 63, 239, 240, 0, 112, 115, 110, 0, 
/* 240 */ 0, 0, 105, 103, 211, 208, 201, 205, 
/* 248 */ 0, 0, 0, 0, 0, 16, 22, 29, 
/* 256 */ 143, 43, 233, 0, 0, 0, 0, 50, 
/* 264 */ 85, 0, 97, 95, 0, 116, 0, 0, 
/* 272 */ 0, 0, 0, 0, 189, 0, 178, 0, 
/* 280 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 288 */ 0, 170, 0, 167, 0, 164, 0, 250, 
/* 296 */ 0, 0, 0, 0, 0, 0, 10, 198, 
/* 304 */ 121, 20, 220, 222, 40, 125, 0, 130, 
/* 312 */ 0, 135, 0, 140, 89, 168, 0, 248, 
/* 320 */ 0, 155, 0, 0, 0, 0, 0, 0, 
/* 328 */ 113, 160, 215, 0, 0, 0, 0, 0, 
/* 336 */ 244, 0, 0, 68, 58, 153, 62, 0, 
/* 344 */ 241, 0, 0, 0, 0, 0, 123, 0, 
/* 352 */ 104, 102, 210, 127, 200, 204, 0, 137, 
/* 360 */ 0, 142, 0, 147, 21, 152, 35, 232, 
/* 368 */ 0, 0, 0, 0, 0, 49, 84, 162, 
/* 376 */ 175, 94, 118, 0, 0, 0, 2, 0, 
/* 384 */ 0, 0, 0, 27, 221, 128, 224, 225, 
/* 392 */ 0, 0, 0, 0, 55, 91, 0, 0, 
/* 400 */ 79, 0, 163, 0, 251, 0, 0, 0, 
/* 408 */ 0, 0, 0, 1, 0, 197, 120, 218, 
/* 416 */ 0, 0, 0, 124, 71, 129, 0, 134, 
/* 424 */ 57, 139, 171, 144, 247, 149, 0, 154, 
/* 432 */ 0, 0, 0, 0, 0, 106, 193, 159, 
/* 440 */ 172, 0, 0, 0, 19, 25, 32, 39, 
/* 448 */ 46, 0, 0, 0, 238, 0, 53, 88, 
/* 456 */ 0, 0, 76, 0, 0, 100, 0, 214, 
/* 464 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 472 */ 0, 0, 138, 0, 231, 0, 234, 0, 
/* 480 */ 0, 0, 0, 0, 0, 99, 0, 119, 
/* 488 */ 0, 253, 0, 0, 0, 229, 0, 0, 
/* 496 */ 188, 4, 177, 0, 0, 226, 0, 0, 
/* 504 */ 0, 0, 0, 0, 83, 81, 0, 166, 
/* 512 */ 0, 249, 0, 0, 0, 0, 108, 195, 
};


static SXINT DYNAM_SCANNER_nfsa_hash_lnk [522] = {
/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 16 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 24 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 32 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 40 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 48 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 56 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 64 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 72 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 80 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 88 */ 0, 0, 0, 0, 0, 0, 72, 73, 
/* 96 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 104 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 112 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 120 */ 0, 8, 0, 0, 0, 47, 0, 0, 
/* 128 */ 41, 61, 0, 190, 191, 0, 65, 0, 
/* 136 */ 179, 180, 0, 0, 54, 14, 0, 36, 
/* 144 */ 0, 0, 0, 15, 0, 0, 77, 0, 
/* 152 */ 28, 0, 0, 0, 0, 42, 0, 0, 
/* 160 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 168 */ 145, 0, 0, 0, 182, 0, 0, 96, 
/* 176 */ 0, 0, 11, 0, 0, 0, 0, 0, 
/* 184 */ 0, 3, 0, 0, 6, 0, 0, 0, 
/* 192 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 200 */ 0, 0, 0, 0, 132, 0, 192, 0, 
/* 208 */ 0, 0, 0, 0, 101, 0, 0, 173, 
/* 216 */ 0, 0, 0, 0, 26, 34, 33, 0, 
/* 224 */ 48, 0, 0, 0, 0, 0, 0, 0, 
/* 232 */ 157, 0, 0, 0, 66, 0, 0, 0, 
/* 240 */ 0, 0, 151, 31, 0, 0, 0, 0, 
/* 248 */ 150, 0, 0, 0, 117, 
};


static struct {
	struct sxword_string	*next;
	char	chars [85];
} DYNAM_SCANNER_terminals_string = {
NULL,
"\000\000%GENERIC_TERMINAL\000%TERMINAL\000%ACTION\000%PREDICATE\000%NON_TERMINAL\000=\
\000%LHS_NON_TERMINAL\000;\000"} /* End DYNAM_SCANNER_terminals_string */;


static SXUINT DYNAM_SCANNER_terminals_head [] = {
/* 0 */ 1, 0, 0, 0, 2, 0, 7, 0, 
/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 16 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 24 */ 0, 8, 0, 0, 0, 0, 9, 0, 
/* 32 */ 0, 0, 3, 0, 0, 0, 0, 0, 
/* 40 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 48 */ 0, 0, 0, 4, 0, 0, 0, 0, 
/* 56 */ 0, 5, 0, 0, 0, 0, 0, 0, 
/* 64 */ 0, 0, 0, 
} /* End DYNAM_SCANNER_terminals_head */;


static struct sxword_table DYNAM_SCANNER_terminals_table [50] = {
/* 0 */ {&(DYNAM_SCANNER_terminals_string.chars [0]), 0, 0},
/* 1 */ {&(DYNAM_SCANNER_terminals_string.chars [1]), 0, 0},
/* 2 */ {&(DYNAM_SCANNER_terminals_string.chars [2]), 17, 0},
/* 3 */ {&(DYNAM_SCANNER_terminals_string.chars [20]), 9, 0},
/* 4 */ {&(DYNAM_SCANNER_terminals_string.chars [30]), 7, 0},
/* 5 */ {&(DYNAM_SCANNER_terminals_string.chars [38]), 10, 0},
/* 6 */ {&(DYNAM_SCANNER_terminals_string.chars [49]), 13, 0},
/* 7 */ {&(DYNAM_SCANNER_terminals_string.chars [63]), 1, 6},
/* 8 */ {&(DYNAM_SCANNER_terminals_string.chars [65]), 17, 0},
/* 9 */ {&(DYNAM_SCANNER_terminals_string.chars [83]), 1, 0},
} /* End DYNAM_SCANNER_terminals_table */;


static struct terminal_to_attr DYNAM_SCANNER_terminal_to_attr [51] = {
/* 0 */ {0, 0, 0, 0}, {0, 0, 0, 0}, {8, 0, 0, 1}, {7, 0, 0, 1}, 
/* 4 */ {6, 0, 0, 1}, {5, 0, 0, 1}, {4, 0, 0, 1}, {3, 0, 0, 0}, 
/* 8 */ {2, 0, 0, 1}, {1, 0, 0, 0}, 
} /* End struct terminal_to_attr DYNAM_SCANNER_terminal_to_attr */;


static SXINT DYNAM_SCANNER_token_no_to_terminal [100] = {
/* 0 */ 0, 9, 8, 7, 6, 5, 4, 3, 
/* 8 */ 2, 0, 0, 0, 0, 0, 0, 0, 
/* 16 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 24 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 32 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 40 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 48 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 56 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 64 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 72 */ 0, 0, 0, 0, 0, 0, 0, 0, 
/* 80 */ 0, 0, 640, 0, 0, 0, 0, 2687040, 
/* 88 */ 131075, 327752, 1572949, 458862, 8061054, 7864362, 3145809, 7864362, 
/* 96 */ 4653138, 3276882, 0, 0, 
} /* End DYNAM_SCANNER_token_no_to_terminal */;


struct sxdynam_scanner DYNAM_SCANNER = {
{
DYNAM_SCANNER_sc_names_table,
(struct sxword_string*) &DYNAM_SCANNER_sc_names_string,
DYNAM_SCANNER_sc_names_head,
132 /* tablength */,
74 /* top */,
257 /* hashsiz */,
247 /* strlength */,
247 /* strnext */,
NULL /* area_name */,
0L /* MACRO */,
NULL /* (*malloc) () */,
NULL /* (*calloc) () */,
NULL /* (*realloc) () */,
NULL /* (*resize) () */,
NULL /* (*free) () */,
NULL /* (*oflw) () */,
NULL /* *stat_file */,
SXTRUE /* is_string_static */,
SXTRUE /* is_head_table_static */
},
DYNAM_SCANNER_sc_names_to_char_sets /* SXINT *sc_names_to_char_sets */,
DYNAM_SCANNER_char_sets /* SXBA *char_sets */,
103 /* SXINT char_sets_top */,
132 /* SXINT char_sets_size */,
{
DYNAM_SCANNER_abbrev_names_table,
(struct sxword_string*) &DYNAM_SCANNER_abbrev_names_string,
DYNAM_SCANNER_abbrev_names_head,
50 /* tablength */,
3 /* top */,
67 /* hashsiz */,
21 /* strlength */,
21 /* strnext */,
NULL /* area_name */,
0L /* MACRO */,
NULL /* (*malloc) () */,
NULL /* (*calloc) () */,
NULL /* (*realloc) () */,
NULL /* (*resize) () */,
NULL /* (*free) () */,
NULL /* (*oflw) () */,
NULL /* *stat_file */,
SXTRUE /* is_string_static */,
SXTRUE /* is_head_table_static */
},
DYNAM_SCANNER_abbrev_to_attr /* struct abbrev_to_attr *abbrev_to_attr */,
{
DYNAM_SCANNER_reg_exp_names_table,
(struct sxword_string*) &DYNAM_SCANNER_reg_exp_names_string,
DYNAM_SCANNER_reg_exp_names_head,
100 /* tablength */,
12 /* top */,
131 /* hashsiz */,
102 /* strlength */,
102 /* strnext */,
NULL /* area_name */,
0L /* MACRO */,
NULL /* (*malloc) () */,
NULL /* (*calloc) () */,
NULL /* (*realloc) () */,
NULL /* (*resize) () */,
NULL /* (*free) () */,
NULL /* (*oflw) () */,
NULL /* *stat_file */,
SXTRUE /* is_string_static */,
SXTRUE /* is_head_table_static */
},
DYNAM_SCANNER_reg_exp_to_attr /* struct reg_exp_to_attr *reg_exp_to_attr */,
10 /* SXINT current_reg_exp_or_abbrev_no */,
15 /* SXINT current_reduce_id */,
110 /* SXINT delta */,
DYNAM_SCANNER_context_sets /* SXBA *context_sets */,
NULL /* SXBA *tfirst */,
0 /* SXINT tfirst_line_nb */,
0 /* SXINT tfirst_col_nb */,
DYNAM_SCANNER_item_to_attr /* struct item_to_attr *item_to_attr */,
500 /* SXINT item_size */,
116 /* SXINT item_top */,
{NULL, /* name */
DYNAM_SCANNER_nfsa_hash_display, /* hash_display */
DYNAM_SCANNER_nfsa_hash_lnk, /* hash_lnk */
521, /* hash_size */
253, /* top */
521, /* size */
-2147483648, /* free_buckets */
0, /* has_free_buckets */
NULL, /* (*system_oflw) () */
NULL, /* (*user_oflw) () */
NULL, /* (*cmp) () */
NULL, /* (*scrmbl) () */
NULL, /* (*suppress) () */
NULL, /* (*assign) () */
NULL, /* stat_file */
0, /* is_locked */
SXTRUE, /* is_static */
DYNAM_SCANNER_nfsa_display, /* display */
{&DYNAM_SCANNER_nfsa_X_hd_0, NULL}, /* X_hd [2] */
{DYNAM_SCANNER_nfsa_lnk_hd_0, NULL}, /* lnk_hd [2] */
{DYNAM_SCANNER_nfsa_lnk_0, NULL}, /* lnk [2] */
{SXTRUE, SXTRUE} /* X_is_static [2] */
},
{
DYNAM_SCANNER_terminals_table,
(struct sxword_string*) &DYNAM_SCANNER_terminals_string,
DYNAM_SCANNER_terminals_head,
50 /* tablength */,
10 /* top */,
67 /* hashsiz */,
85 /* strlength */,
85 /* strnext */,
NULL /* area_name */,
0L /* MACRO */,
NULL /* (*malloc) () */,
NULL /* (*calloc) () */,
NULL /* (*realloc) () */,
NULL /* (*resize) () */,
NULL /* (*free) () */,
NULL /* (*oflw) () */,
NULL /* *stat_file */,
SXTRUE /* is_string_static */,
SXTRUE /* is_head_table_static */
},
DYNAM_SCANNER_terminal_to_attr /* struct terminal_to_attr *terminal_to_attr */,
DYNAM_SCANNER_token_no_to_terminal /* SXINT *token_no_to_terminal */,
100 /* SXINT token_no_to_terminal_size */,
NULL /* char *ws */,
0 /* SXINT ws_lgth */,
DYNAM_SCANNER_context_sets_line_0 /* SXBA ctxt_set */,
DYNAM_SCANNER_context_sets_line_1 /* SXBA filter */,
0 /* SXINT counters_size */,
9 /* SXINT eof_code */,
11 /* SXINT eof_reg_exp_name */,
SXTRUE /* is_char_sets_static */,
SXTRUE /* is_item_to_attr_static */,
SXTRUE /* is_token_no_to_terminal_static */,
0 /* SXBOOLEAN is_smp_error */,
/* ... */
} /* struct sxdynam_scanner DYNAM_SCANNER */;
;
/* End of sxtables for bnf [Thu Jun  9 11:32:55 1994] */
