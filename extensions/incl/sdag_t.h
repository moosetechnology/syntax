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


/* Beginning of sxtables for sdag [Thu Oct  2 13:03:34 2008] */
#define SXP_MAX 27
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{0, 2, 1, -12},
{0, 9, 1, -19},
{0, 3, 0, -12},
{0, 0, 2, -17},
{0, 1, 2, -17},
{2, 5, 0, -18},
{6, 6, 0, -18},
{1, 11, 0, 27},
{5, 4, 3, -1},
{3, 8, 1, -13},
{4, 7, 1, -13},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-7, 22, 0},
{0, 21, 0},
{-3, 0, 0},
{0, 8, 5},
{0, 14, 1},
{0, -4, 7},
{0, 15, 3},
{-11, 10, 6},
{0, 16, 7},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-18, -1, 3},
{-2, 0, 0},
{-12, 0, 0},
{-19, -13, 5},
{-17, 0, 0},
};
static struct SXP_item vector[]={
{0, 0},
{0, 0},
{2, 5},
{4, 9},
{5, 8},
{5, 6},
{0, 0},
{0, 0},
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
0};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%ld warnings and %ld errors are reported.";

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,14,3,14,3,14,14,3,3,14,3,14,14,3,14,14,14,14,
3,14,3,14,14,3,3,14,3,14,14,3,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,3,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,3,14,14,14,14,14,
14,14,14,3,3,3,3,3,3,3,3,3,4,5,6,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,7,8,3,3,3,9,10,11,12,3,3,3,3,
13,14,14,14,14,14,14,14,14,14,14,3,3,3,3,3,3,3,15,16,14,17,18,14,19,
14,20,14,14,14,14,21,14,14,14,14,22,14,14,14,14,14,14,14,23,24,25,3,3,3,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,26,27,28,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,14,3,14,3,14,14,3,3,14,3,14,
14,3,14,14,14,14,3,14,3,14,14,3,3,14,3,14,14,3,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,3,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
3,14,14,14,14,14,14,14,14,};
static SXSTMI SXS_transition[]={0,
0x0,0x4007,0x0,0x2402,0x2402,0x2402,0x2403,0x9002,0x2404,0x4403,0x4404,0x0,
0x2405,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2407,0x0,
0x0,0x9c01,0x0,0x0,
0x0,0x4000,0x0,0x1402,0x1402,0x1402,0x4000,0x4000,0x4000,0x4000,0x4000,0x0,
0x2405,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x0,
0x0,0x4000,0x0,0x0,
0x0,0x0,0x1c03,0x1c03,0x0,0x1c03,0x4405,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x2409,
0x1c03,0x1c03,0x1c03,0x1c03,
0x0,0x0,0x1c04,0x1c04,0x0,0x1c04,0x1c04,0x1c04,0x4405,0x1c04,0x1c04,0x1c04,
0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x240a,
0x1c04,0x1c04,0x1c04,0x1c04,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x240b,
0x240c,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x1c06,0x4005,0x4005,0x4005,0x4005,0x4005,0x4005,0x4005,0x4005,0x1c06,
0x4005,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,
0x1c06,0x4005,0x1c06,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x240d,0x0,
0x0,0x0,0x240f,0x240f,0x4400,0x240f,0x240f,0x2410,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x0,0x0,0x0,0x0,0x2c03,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c03,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c04,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c04,
0x0,0x0,0x0,0x0,
0x0,0x0,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x2411,
0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x140b,0x140b,0x140b,0x140b,
0x0,0x0,0x140c,0x140c,0x2402,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,
0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,
0x140c,0x140c,0x140c,0x140c,
0x0,0x0,0x1c0d,0x0,0x0,0x0,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,
0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x2412,
0x1c0d,0x1c0d,0x2413,0x1c0d,
0x0,0x0,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,
0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x2c14,
0x1c0e,0x9c04,0x1c0e,0x9006,
0x0,0x0,0x140f,0x140f,0x4400,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,
0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,
0x140f,0x140f,0x140f,0x140f,
0x0,0x0,0x240f,0x1410,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x2415,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x1411,
0x2402,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,
0x240b,0x240b,0x240b,0x240b,
0x0,0x0,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x4406,0x0,0x0,0x0,
0x0,0x0,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,
0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,
0x2c0e,0x2c0e,0x2c0e,0x2c0e,
0x0,0x0,0x240f,0x240f,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,0x2416,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x240f,0x240f,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x2417,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x240f,0x240f,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x2418,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x240f,0x1418,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x2419,0x240f,0x241a,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x240f,0x240f,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,0x240f,0x241b,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x240f,0x240f,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x241c,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x240f,0x240f,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x241d,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x240f,0x240f,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,0x241e,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x240f,0x240f,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x241f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x141e,0x141e,0x4402,0x141e,0x141e,0x141e,0x141e,0x141e,0x141e,0x141e,
0x141e,0x141e,0x141e,0x141e,0x141e,0x141e,0x141e,0x141e,0x141e,0x141e,0x141e,0x141e,
0x141e,0x141e,0x141e,0x141e,
0x0,0x0,0x240f,0x240f,0x4400,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x2420,0x240f,0x240f,0x240f,
0x240f,0x240f,0x240f,0x240f,
0x0,0x0,0x1420,0x1420,0x4401,0x1420,0x1420,0x1420,0x1420,0x1420,0x1420,0x1420,
0x1420,0x1420,0x1420,0x1420,0x1420,0x1420,0x1420,0x1420,0x1420,0x1420,0x1420,0x1420,
0x1420,0x1420,0x1420,0x1420,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[28],
&SXS_transition[56],
&SXS_transition[84],
&SXS_transition[112],
&SXS_transition[140],
&SXS_transition[168],
&SXS_transition[196],
&SXS_transition[224],
&SXS_transition[252],
&SXS_transition[280],
&SXS_transition[308],
&SXS_transition[336],
&SXS_transition[364],
&SXS_transition[392],
&SXS_transition[420],
&SXS_transition[448],
&SXS_transition[476],
&SXS_transition[504],
&SXS_transition[532],
&SXS_transition[560],
&SXS_transition[588],
&SXS_transition[616],
&SXS_transition[644],
&SXS_transition[672],
&SXS_transition[700],
&SXS_transition[728],
&SXS_transition[756],
&SXS_transition[784],
&SXS_transition[812],
&SXS_transition[840],
&SXS_transition[868],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x200e,4,0,1,0},
{0x2408,1,0,1,1},
{0x0,0,0,1,1},
{0x200e,12,0,1,0},
{0x200e,13,0,1,0},
{0x9c05,2,0,1,1},
{0x4c00,0,0,1,1},
};
static char *S_adrp[]={0,
"bodag",
"eodag",
"(",
")",
"%form",
"%SEMLEX",
"End Of File",
};
static SXINT S_is_a_keyword[]={
0};
static SXINT S_is_a_generic_terminal[]={
2,5,6};
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
#ifdef SEMACT
extern SXSEMACT_FUNCTION SEMACT;
#endif

SXTABLES sxtables={
52113, /* magic */
sxscanner,
sxparser,
{255, 7, 1, 3, 4, 28, 1, 32, 0, 0, 0, 
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
(SXSCANACT_FUNCTION *) NULL,
sxsrecovery,
(SXCHECK_KEYWORD_FUNCTION *) NULL
},
{2, 11, 11, 16, 16, 20, 22, 29, 7, 7, 11, 11, 9, 6, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-22,
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
(SXPARSACT_FUNCTION *) NULL,
(SXDESAMBIG_FUNCTION *) NULL,
#ifdef SEMACT
SEMACT
#else
(SXSEMACT_FUNCTION *) NULL
#endif
},
err_titles,
abstract,
(sxsem_tables *) NULL,
NULL
};
/* End of sxtables for sdag [Thu Oct  2 13:03:34 2008] */
