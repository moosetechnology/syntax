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

/* Beginning of sxtables for bnf */
#define sxtables bnf_tables /* due to option "-name" of tables_c */
#define SXP_MAX 26
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{6, 3, 0, -15},
{2, 2, 1, -15},
{2, 5, 1, -16},
{6, 6, 1, -16},
{0, 0, 2, -18},
{1, 1, 1, -18},
{0, 4, 1, 25},
{5, 9, 0, -3},
{5, 12, 0, -19},
{5, 13, 0, -19},
{1, 8, 1, -3},
{5, 7, 1, -3},
{1, 11, 1, -3},
{0, 10, 1, -3},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-6, 20, 2},
{0, 26, 0},
{0, 20, 2},
{0, -5, 9},
{-14, 13, 5},
{0, 4, 3},
{-12, 11, 5},
{0, 17, 9},
};
static struct SXP_bases nt_bases[]={{0,0,0},
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

static  SXBA_ELT SXPBA_kt1
#if SXBITS_PER_LONG==32
 [3] = {9, 0X0000010E, 0X00000000, }
#else
 [2] = {9, 0X000000000000010E, }
#endif
/* End SXPBA_kt1 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt1[0],
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
1,1};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%ld warnings and %ld errors are reported.";

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,3,3,3,3,3,3,3,3,3,4,5,4,4,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,6,7,8,9,7,10,11,7,7,7,12,7,7,7,7,
7,13,14,14,14,14,14,14,14,15,15,7,16,17,18,19,7,20,21,21,21,21,21,21,21,
21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,7,22,7,7,23,7,
21,24,21,21,21,25,21,21,21,21,21,21,21,26,21,21,21,27,21,28,21,29,21,21,21,
21,7,7,7,30,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,};
static SXSTMI SXS_transition[]={0,
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
0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x4c04,0x1c09,0x1c09,0x2412,0x1c09,0x1c09,
0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x7013,0x2c14,0x2c14,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x140b,0x140b,0x2415,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x140b,0x140b,0x140b,0x140b,0x140b,0x2402,
0x0,0x0,0x140c,0x140c,0x2403,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,
0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,
0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,
0x0,0x0,0x1c0d,0x1c0d,0x0,0x1c0d,0x1c0d,0x4407,0x1c0d,0x1c0d,0x1c0d,0x1c0d,
0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x2417,0x1c0d,0x1c0d,
0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,
0x0,0x0,0x2c0d,0x2c0d,0x2418,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x900b,0x900b,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x940c,
0x940d,0x940e,0x940f,0x9410,0x9411,0x2c0d,
0x0,0x0,0x0,0x4008,0x4008,0x4008,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,
0x1c0f,0x1c0f,0x1c0f,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x1c0f,0x2c04,0x2c19,0x1c0f,
0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x2c04,
0x1,0x0,0x0,0x9c01,0x9c01,0x9c01,0x0,0x0,0x0,0x0,0x0,0x0,
0x9009,0x9009,0x9009,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x9c01,
0x0,0x0,0x0,0x9001,0x9001,0x9001,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c11,0x1c11,0x1c11,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x9001,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x9412,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x2c09,0x0,0x2c09,0x0,0x0,0x9413,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x1,0x0,0x0,0x4c06,0x4c06,0x4c06,0x0,0x0,0x0,0x0,0x0,0x0,
0x900a,0x900a,0x900a,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x4c06,
0x0,0x0,0x0,0x4006,0x4006,0x4006,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c14,0x1c14,0x1c14,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x4006,
0x0,0x0,0x240b,0x240b,0x1415,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,
0x240b,0x240b,0x240b,0x240b,0x0,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,
0x240b,0x240b,0x240b,0x240b,0x240b,0x2402,
0x0,0x0,0x0,0x0,0x0,0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,
0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,0x4c02,0x1c16,0x1c16,0x241a,0x1c16,0x1c16,
0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,
0x0,0x0,0x2c0d,0x2c0d,0x241b,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x900b,0x900b,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x940c,
0x940d,0x940e,0x940f,0x9410,0x9411,0x2c0d,
0x0,0x0,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x240e,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x0,0x0,0x0,0x4007,0x4007,0x4007,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,
0x2c0f,0x2c0f,0x2c0f,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c0f,0x2c04,0x2c04,0x2c0f,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c04,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x9416,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x2c16,0x0,0x2c16,0x0,0x0,0x9417,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x4407,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x0,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2417,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c1c,0x9018,0x9018,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c1d,0x9019,0x9019,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x901c,0x901c,0x0,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,
0x2c1f,0x2c1f,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,
0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,
0x0,0x0,0x901c,0x901c,0x0,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,
0x9c1c,0x9c1c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,
0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[30],
&SXS_transition[60],
&SXS_transition[90],
&SXS_transition[120],
&SXS_transition[150],
&SXS_transition[180],
&SXS_transition[210],
&SXS_transition[240],
&SXS_transition[270],
&SXS_transition[300],
&SXS_transition[330],
&SXS_transition[360],
&SXS_transition[390],
&SXS_transition[420],
&SXS_transition[450],
&SXS_transition[480],
&SXS_transition[510],
&SXS_transition[540],
&SXS_transition[570],
&SXS_transition[600],
&SXS_transition[630],
&SXS_transition[660],
&SXS_transition[690],
&SXS_transition[720],
&SXS_transition[750],
&SXS_transition[780],
&SXS_transition[810],
&SXS_transition[840],
&SXS_transition[870],
&SXS_transition[900],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x6005,3,0,0,0},
{0x4001,1,0,0,0},
{0x2016,3,0,1,0},
{0x4003,4,0,1,0},
{0x9c03,1,0,1,1},
{0x2c09,0,0,1,1},
{0x9404,2,0,1,1},
{0x2c04,0,0,1,1},
{0x2c1c,10,0,1,0},
{0x2c1d,10,0,1,0},
{0x2c1e,10,0,1,0},
{0x200d,9,8,1,0},
{0x200d,9,12,1,0},
{0x200d,9,10,1,0},
{0x200d,9,13,1,0},
{0x200d,9,9,1,0},
{0x200d,9,11,1,0},
{0x9014,9,92,1,0},
{0x9015,9,92,1,0},
{0x2009,9,34,1,0},
{0x2009,9,92,1,0},
{0x901a,9,92,1,0},
{0x901b,9,92,1,0},
{0x2c11,5,0,1,0},
{0x2c14,5,0,1,0},
{0x2016,9,34,1,0},
{0x2016,9,92,1,0},
{0x200d,2,0,0,0},
};
static char *S_adrp[]={0,
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
static SXINT S_is_a_keyword[]={
0};
static SXINT S_is_a_generic_terminal[]={
6,2,4,5,6,7,8};
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
#ifdef SCANACT
#warning "compiling with -DSCANACT is deprecated; define sxscanner_action() or invoke sxset_scanner_action()"
#endif
static struct SXT_node_info SXT_node_info[]={{0,0},
{3,1},{0,2},{8,4},{0,5},{0,6},{10,8},{6,9},{11,10},{2,12},{0,13},{12,14},{9,16},
{4,17},{0,18},};
static SXINT T_ter_to_node_name[]={
0,0,5,0,6,7,};
static char *T_node_name[]={NULL,
"ERROR",
"ACTION",
"BNF",
"GENERIC_TERMINAL",
"LHS_NON_TERMINAL",
"NON_TERMINAL",
"PREDICATE",
"RULE_S",
"TERMINAL",
"VOCABULARY_S",
"X_NON_TERMINAL",
"X_TERMINAL",
};
extern SXSEMPASS_FUNCTION SEMPASS;
static char T_stack_schema[]={0,
0,0,1,0,1,0,1,1,0,0,1,0,0,0,1,0,0,0,};

static struct SXT_tables SXT_tables={
SXT_node_info, T_ter_to_node_name, T_stack_schema, SEMPASS, T_node_name
};
extern SXSEMACT_FUNCTION sxatc;

SXTABLES sxtables={
52113, /* magic */
sxscanner,
sxparser,
{255, 9, 1, 3, 4, 30, 1, 31, 1, 1, 0, 
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
sxscanner_action,
sxsrecovery,
(SXCHECK_KEYWORD_FUNCTION *) NULL
},
{3, 14, 14, 17, 17, 22, 24, 35, 9, 6, 13, 13, 8, 4, 0, 9, 4, 7, 2, 5, 11, 5, 4,
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
SXPBM_trans,
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
(SXPARSACT_FUNCTION *) NULL,
(SXDESAMBIG_FUNCTION *) NULL,
sxatc
},
err_titles,
abstract,
(sxsem_tables *) &SXT_tables,
NULL
};
/* End of sxtables for bnf */
