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

/* Beginning of sxtables for prio */
#define SXP_MAX 63
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{0, 2, 1, -26},
{0, 11, 1, -27},
{4, 10, 0, 46},
{7, 18, 0, -5},
{0, 15, 1, -29},
{8, 17, 0, -5},
{0, 8, 1, -30},
{0, 13, 2, -2},
{9, 25, 1, -8},
{0, 3, 0, -26},
{0, 0, 2, -34},
{1, 5, 0, -28},
{2, 6, 0, -28},
{3, 7, 0, -28},
{5, 12, 0, -27},
{0, 1, 2, -34},
{0, 24, 0, -36},
{0, 23, 0, -36},
{6, 16, 1, -29},
{0, 14, 1, -35},
{0, 4, 2, -1},
{10, 22, 1, 47},
{0, 21, 1, 47},
{11, 20, 1, -6},
{0, 19, 1, -6},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-15, 55, 0},
{-16, 37, 5},
{0, 54, 0},
{-20, 54, 52},
{-21, 54, 0},
{0, 33, 10},
{-10, 0, 0},
{0, 54, 0},
{0, -11, 11},
{0, 31, 4},
{-23, 22, 8},
{0, 19, 6},
{-25, 24, 8},
{0, 32, 11},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-28, 63, 0},
{-29, 44, 0},
{-30, 40, 53},
{-6, 40, 57},
{-7, 40, 53},
{-8, 0, 0},
{-26, -34, 1},
{-9, -36, 15},
};
static struct SXP_item vector[]={
{9, -2},
{10, -35},
{15, -36},
{1, 12},
{2, 13},
{3, 14},
{7, 38},
{6, 17},
{8, -3},
{0, 0},
{9, 18},
{0, 0},
{8, -4},
{3, -27},
{4, -1},
{0, 0},
{0, 0},
{13, -5},
{0, 0},
};

static  SXBA_ELT SXPBA_kt1
#if SXBITS_PER_LONG==32
 [3] = {15, 0X00004082, 0X00000000, }
#else
 [2] = {15, 0X0000000000004082, }
#endif
/* End SXPBA_kt1 */;

static  SXBA_ELT SXPBA_kt2
#if SXBITS_PER_LONG==32
 [3] = {15, 0X00004082, 0X00000000, }
#else
 [2] = {15, 0X0000000000004082, }
#endif
/* End SXPBA_kt2 */;

static  SXBA_ELT SXPBA_kt3
#if SXBITS_PER_LONG==32
 [3] = {15, 0X00004082, 0X00000000, }
#else
 [2] = {15, 0X0000000000004082, }
#endif
/* End SXPBA_kt3 */;

static  SXBA_ELT SXPBA_kt4
#if SXBITS_PER_LONG==32
 [3] = {15, 0X00004486, 0X00000000, }
#else
 [2] = {15, 0X0000000000004486, }
#endif
/* End SXPBA_kt4 */;

static  SXBA_ELT SXPBA_kt5
#if SXBITS_PER_LONG==32
 [3] = {15, 0X00004086, 0X00000000, }
#else
 [2] = {15, 0X0000000000004086, }
#endif
/* End SXPBA_kt5 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt1[0],
&SXPBA_kt2[0],
&SXPBA_kt3[0],
&SXPBA_kt4[0],
&SXPBA_kt5[0],
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
5,1,2,3,4,5};

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
7,13,14,14,14,14,14,14,14,15,15,7,16,17,18,19,7,20,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,21,7,7,7,7,
22,23,22,22,22,24,22,22,22,22,22,22,22,25,22,22,22,26,22,27,22,28,22,22,22,
22,7,7,7,29,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,};
static SXSTMI SXS_transition[]={0,
0x0,0x400b,0x0,0x2402,0x2402,0x2402,0x2c03,0x2404,0x2405,0x9001,0x2c06,0x9003,
0x2c03,0x2c03,0x2c03,0x4404,0x9005,0x2c08,0x2c03,0x9007,0x2c03,0x2c03,0x2c03,0x2c03,
0x2c03,0x2c03,0x2c03,0x2c03,0x240a,
0x0,0x4000,0x0,0x1402,0x1402,0x1402,0x4000,0x4000,0x4000,0x4000,0x4000,0x9009,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,0x240a,
0x0,0x3009,0x0,0x3009,0x3009,0x3009,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x900b,
0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x0,0x0,0x2c0f,0x2c0f,0x0,0x2c0f,0x2c0f,0x0,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2410,0x2c0f,0x2c0f,0x2c0f,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x0,0x0,0x0,0x0,0x0,0x0,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x7011,0x2c12,0x2c12,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,
0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x4c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,
0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,
0x0,0x4006,0x0,0x4006,0x4006,0x4006,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x900e,
0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x7013,0x2c14,0x2c14,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x140a,0x140a,0x2415,0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,
0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,
0x140a,0x140a,0x140a,0x140a,0x2402,
0x0,0x0,0x140b,0x140b,0x2402,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x140b,0x140b,0x140b,0x140b,0x140b,
0x0,0x0,0x0,0x0,0x0,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,
0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x4c05,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,
0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x7016,0x2c17,0x2c17,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c18,0x2c18,0x2c18,
0x2c18,0x2c18,0x2c18,0x2c18,0x0,
0x0,0x0,0x1c0f,0x1c0f,0x0,0x1c0f,0x1c0f,0x3409,0x1c0f,0x1c0f,0x1c0f,0x1c0f,
0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x2419,0x1c0f,0x1c0f,0x1c0f,
0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,
0x0,0x0,0x2c0f,0x2c0f,0x241a,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x9012,0x9012,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x9413,0x9414,
0x9415,0x9416,0x9417,0x9418,0x2c0f,
0x1,0x0,0x0,0x4c08,0x4c08,0x4c08,0x0,0x0,0x0,0x0,0x0,0x4c08,
0x900d,0x900d,0x900d,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x4c08,
0x0,0x0,0x0,0x4008,0x4008,0x4008,0x0,0x0,0x0,0x0,0x0,0x4008,
0x1c12,0x1c12,0x1c12,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x4008,
0x1,0x0,0x0,0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,
0x9010,0x9010,0x9010,0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,
0x4c07,0x4c07,0x4c07,0x4c07,0x4c07,
0x0,0x0,0x0,0x4007,0x4007,0x4007,0x4007,0x4007,0x4007,0x4007,0x4007,0x4007,
0x1c14,0x1c14,0x1c14,0x4007,0x4007,0x4007,0x4007,0x4007,0x4007,0x4007,0x4007,0x4007,
0x4007,0x4007,0x4007,0x4007,0x4007,
0x0,0x0,0x240a,0x240a,0x1415,0x240a,0x240a,0x240a,0x240a,0x240a,0x240a,0x240a,
0x240a,0x240a,0x240a,0x240a,0x0,0x240a,0x240a,0x240a,0x240a,0x240a,0x240a,0x240a,
0x240a,0x240a,0x240a,0x240a,0x2402,
0x1,0x0,0x0,0x4c05,0x4c05,0x4c05,0x0,0x0,0x0,0x0,0x0,0x4c05,
0x9011,0x9011,0x9011,0x0,0x0,0x4c05,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x4c05,
0x0,0x0,0x0,0x4005,0x4005,0x4005,0x0,0x0,0x0,0x0,0x0,0x4005,
0x1c17,0x1c17,0x1c17,0x0,0x0,0x4005,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x4005,
0x0,0x0,0x0,0x3000,0x3000,0x3000,0x0,0x0,0x0,0x0,0x0,0x3000,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1c18,0x1c18,0x1c18,
0x1c18,0x1c18,0x1c18,0x1c18,0x3000,
0x0,0x0,0x2c0f,0x2c0f,0x241b,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x9012,0x9012,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x9413,0x9414,
0x9415,0x9416,0x9417,0x9418,0x2c0f,
0x0,0x0,0x2c0f,0x2c0f,0x0,0x2c0f,0x2c0f,0x0,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x0,0x2c0f,0x2c0f,0x2c0f,0x2410,0x2c0f,0x2c0f,0x2c0f,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x0,0x0,0x2c0f,0x2c0f,0x0,0x2c0f,0x2c0f,0x3409,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x0,0x2c0f,0x2c0f,0x2c0f,0x2419,0x2c0f,0x2c0f,0x2c0f,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c1c,0x9019,0x9019,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c1d,0x901a,0x901a,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x1c1e,0x901b,0x901b,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x901c,0x901c,0x0,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,
0x2c20,0x2c20,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,
0x901c,0x901c,0x901c,0x901c,0x901c,
0x0,0x0,0x901c,0x901c,0x0,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,
0x9c1c,0x9c1c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,0x901c,
0x901c,0x901c,0x901c,0x901c,0x901c,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[29],
&SXS_transition[58],
&SXS_transition[87],
&SXS_transition[116],
&SXS_transition[145],
&SXS_transition[174],
&SXS_transition[203],
&SXS_transition[232],
&SXS_transition[261],
&SXS_transition[290],
&SXS_transition[319],
&SXS_transition[348],
&SXS_transition[377],
&SXS_transition[406],
&SXS_transition[435],
&SXS_transition[464],
&SXS_transition[493],
&SXS_transition[522],
&SXS_transition[551],
&SXS_transition[580],
&SXS_transition[609],
&SXS_transition[638],
&SXS_transition[667],
&SXS_transition[696],
&SXS_transition[725],
&SXS_transition[754],
&SXS_transition[783],
&SXS_transition[812],
&SXS_transition[841],
&SXS_transition[870],
&SXS_transition[899],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2c0e,1,0,1,1},
{0x2c03,0,0,1,1},
{0x240b,1,0,1,1},
{0x2c03,0,0,1,1},
{0x2c0c,1,0,1,1},
{0x2c07,0,0,1,1},
{0x2c0d,1,0,1,1},
{0x2c09,0,0,1,1},
{0x240b,1,0,1,1},
{0x4000,0,0,1,1},
{0x3009,1,0,1,1},
{0x2c03,0,0,1,1},
{0x2c1c,10,0,1,0},
{0x4006,1,0,1,1},
{0x2c03,0,0,1,1},
{0x2c1d,10,0,1,0},
{0x2c1e,10,0,1,0},
{0x2c1f,10,0,1,0},
{0x200f,9,8,1,0},
{0x200f,9,12,1,0},
{0x200f,9,10,1,0},
{0x200f,9,13,1,0},
{0x200f,9,9,1,0},
{0x200f,9,11,1,0},
{0x2c12,5,0,1,0},
{0x2c14,5,0,1,0},
{0x2c17,5,0,1,0},
{0x200f,1,0,0,0},
};
static char *S_adrp[]={0,
"%left",
"%right",
"%nonassoc",
";",
"%LHS_NON_TERMINAL",
"=",
"%NON_TERMINAL",
"%PREDICATE",
"%TERMINAL",
"%prec",
"End Of File",
};
static SXINT S_is_a_keyword[]={
4,1,2,3,10};
static SXINT S_is_a_generic_terminal[]={
4,5,7,8,9};
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
static SXCHECK_KEYWORD_FUNCTION sxcheck_keyword;
#ifdef SEMACT
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
extern SXSEMACT_FUNCTION SEMACT;
#pragma GCC diagnostic pop
#endif

SXTABLES sxtables={
52113, /* magic */
sxscanner,
sxparser,
{255, 11, 1, 3, 4, 29, 0, 32, 1, 1, 0, 
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
sxcheck_keyword
},
{9, 25, 25, 32, 33, 39, 53, 71, 11, 15, 25, 25, 14, 9, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-53,
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

#include "sxdico.h"

#define KW_NB		4
#define INIT_BASE	7
#define INIT_SUFFIX	16
#define CLASS_MASK	7
#define STOP_MASK	8
#define BASE_SHIFT	5
#define SUFFIX_MASK	16

static SXINT kw_code2t_code [5] = {0,
10, 3, 2, 1, 
};

static SXINT kw_code2lgth [5] = {0,
5, 9, 6, 5, 
};

static SXUINT comb_vector [14] = {
0, 0, 0, 0, 42, 75, 141, 110, 81, 
0, 0, 52, 0, 0, 
};

static unsigned char char2class [256] = {
'\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\001', '\000', '\002', '\005', 
'\000', '\006', '\000', '\000', '\000', '\000', '\000', '\000', 
'\003', '\000', '\000', '\000', '\000', '\004', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', 
};

#include "sxcheck_keyword.h"

/* End of sxtables for prio */
