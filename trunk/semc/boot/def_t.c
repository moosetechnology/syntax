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

/* Beginning of sxtables for def */
#define SXP_MAX 48
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{0, 3, 0, -21},
{0, 2, 1, -21},
{4, 4, 1, 36},
{9, 9, 0, -23},
{8, 8, 1, -23},
{11, 11, 1, 40},
{15, 15, 2, -25},
{10, 10, 3, 40},
{5, 5, 0, -22},
{0, 0, 2, -29},
{1, 1, 1, -29},
{12, 12, 0, -24},
{7, 7, 0, -3},
{0, 6, 1, -3},
{14, 14, 0, -26},
{19, 19, 0, -30},
{16, 16, 0, 42},
{18, 18, 1, -30},
{0, 17, 1, -6},
{13, 13, 2, -26},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-11, 9, 1},
{-13, 12, 4},
{-14, 12, 4},
{-15, 31, 2},
{-16, 0, 0},
{0, 42, 0},
{0, 17, 6},
{0, 9, 1},
{0, -10, 7},
{-19, 18, 3},
{0, 27, 3},
{0, 20, 3},
{0, 28, 7},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-2, -22, 4},
{-25, 34, 38},
{-5, 34, 36},
{-26, 0, 0},
{-30, -6, 11},
{-7, 0, 0},
{-8, 0, 0},
{-22, 48, 0},
};
static struct SXP_item vector[]={
{8, -24},
{5, -3},
{6, -23},
{7, -4},
{10, -25},
{5, 32},
{6, 17},
{1, -29},
{2, -21},
{3, -1},
{0, 0},
};

static  SXBA_ELT SXPBA_kt1
#if SXBITS_PER_LONG==32
 [3] = {14, 0X00002102, 0X00000000, }
#else
 [2] = {14, 0X0000000000002102, }
#endif
/* End SXPBA_kt1 */;

static  SXBA_ELT SXPBA_kt4
#if SXBITS_PER_LONG==32
 [3] = {14, 0X0000210E, 0X00000000, }
#else
 [2] = {14, 0X000000000000210E, }
#endif
/* End SXPBA_kt4 */;

static  SXBA_ELT SXPBA_kt6
#if SXBITS_PER_LONG==32
 [3] = {14, 0X000029DE, 0X00000000, }
#else
 [2] = {14, 0X00000000000029DE, }
#endif
/* End SXPBA_kt6 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt1[0],
&SXPBA_kt4[0],
&SXPBA_kt6[0],
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
1,1};
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
"End of Definition",
"%sParsing stops on the \"End of Definition\".",
};
static SXINT PER_tset[]={
3,1,4,6};

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
7,7,7,3,3,3,3,3,3,3,3,3,4,5,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,6,7,8,7,9,10,7,11,12,13,14,7,15,7,7,
16,17,17,17,17,17,17,17,17,18,18,7,7,19,20,21,7,7,22,22,22,22,22,22,22,
22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,7,23,7,7,24,7,
22,25,22,22,22,26,22,22,22,22,22,22,22,27,22,22,22,28,22,29,22,30,22,22,22,
22,7,7,7,7,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,};
static SXSTMI SXS_transition[]={0,
0x0,0x4007,0x9003,0x9005,0x9007,0x9005,0x9003,0x9009,0x900b,0x9003,0x900d,0x9003,
0x9003,0x900f,0x9011,0x9013,0x9003,0x9003,0x9015,0x9017,0x9003,0x9003,0x9003,0x9003,
0x9003,0x9003,0x9003,0x9003,0x9003,0x9003,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c0b,0x0,0x0,
0x2c0b,0x2c0b,0x2c0b,0x2c0b,0x2c0b,0x2c0b,
0x0,0x4000,0x0,0x1403,0x4000,0x1403,0x0,0x0,0x4000,0x0,0x0,0x0,
0x0,0x0,0x4000,0x0,0x0,0x0,0x4000,0x4000,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x1404,0x1404,0x2409,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x0,0x0,0x1c05,0x1c05,0x0,0x1c05,0x1c05,0x2c09,0x1c05,0x1c05,0x1c05,0x1c05,
0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x901a,0x1c05,0x1c05,0x1c05,0x2c0c,0x1c05,
0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,
0x0,0x0,0x1c06,0x1c06,0x0,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x2c09,0x1c06,
0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x901c,0x1c06,0x1c06,0x1c06,0x2c0d,0x1c06,
0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,
0x0,0x4000,0x9003,0x9003,0x901e,0x9003,0x9003,0x9009,0x4000,0x9003,0x900d,0x9003,
0x9003,0x2c0e,0x901e,0x9013,0x9003,0x9003,0x9020,0x901e,0x9003,0x9003,0x9003,0x9003,
0x9003,0x9003,0x9003,0x9003,0x9003,0x9003,
0x0,0x4000,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,0x9009,0x4000,0x1c08,0x900d,0x1c08,
0x1c08,0x900f,0x1c08,0x9013,0x1c08,0x1c08,0x9020,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,
0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,
0x0,0x4000,0x9003,0x9003,0x901e,0x9003,0x9003,0x9009,0x4000,0x9003,0x900d,0x9003,
0x9003,0x900f,0x901e,0x9013,0x9003,0x9003,0x9020,0x901e,0x9003,0x9003,0x9003,0x9003,
0x9003,0x9003,0x9003,0x9003,0x9003,0x9003,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c0f,0x0,0x0,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x0,0x902a,0x902a,0x902a,0x902a,0x902a,0x902a,0x902a,0x902a,0x902a,0x902a,0x902a,
0x902a,0x902a,0x902a,0x902a,0x1c0b,0x1c0b,0x902a,0x902a,0x902a,0x1c0b,0x902a,0x2c10,
0x1c0b,0x1c0b,0x1c0b,0x1c0b,0x1c0b,0x1c0b,
0x0,0x0,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,
0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,
0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,0x2c05,
0x0,0x0,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,
0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,
0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,
0x0,0x0,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,
0x1c0e,0x2c11,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x902b,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,
0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,
0x0,0x0,0x9031,0x9031,0x9031,0x9031,0x9031,0x9031,0x0,0x9031,0x9031,0x9031,
0x9031,0x9031,0x9031,0x9031,0x1c0f,0x1c0f,0x9031,0x9031,0x9031,0x1c0f,0x9031,0x2c12,
0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x2c0b,0x2c0b,0x0,0x0,0x0,0x2c0b,0x0,0x0,
0x2c0b,0x2c0b,0x2c0b,0x2c0b,0x2c0b,0x2c0b,
0x0,0x0,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,
0x2c0e,0x1c11,0x2c0e,0x2c09,0x2c0e,0x2c0e,0x902b,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,
0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x2c0f,0x2c0f,0x0,0x0,0x0,0x2c0f,0x0,0x0,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x0,0x4003,0x4003,0x1413,0x4003,0x1413,0x4003,0x4003,0x4003,0x4003,0x4003,0x2415,
0x4003,0x4003,0x4003,0x4003,0x4003,0x4003,0x4003,0x4003,0x4003,0x4003,0x4003,0x4003,
0x4003,0x4003,0x4003,0x4003,0x4003,0x4003,
0x0,0x0,0x9032,0x1414,0x9032,0x1414,0x9032,0x9032,0x0,0x9032,0x9032,0x2416,
0x9032,0x9032,0x9032,0x9032,0x9032,0x9032,0x9032,0x9032,0x9032,0x9032,0x9032,0x9032,
0x9032,0x9032,0x9032,0x9032,0x9032,0x9032,
0x0,0x0,0x0,0x1415,0x0,0x1415,0x0,0x2417,0x0,0x2c18,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x2c19,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x1416,0x0,0x1416,0x0,0x241a,0x0,0x2c1b,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x2c1c,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x2c1d,0x2c1d,0x9033,0x2c1d,0x2c1d,0x0,0x2c1d,0x2c1d,0x2c1d,0x2c1d,
0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x241e,0x2c1d,
0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c1f,0x0,0x0,
0x2c1f,0x2c1f,0x2c1f,0x2c1f,0x2c1f,0x2c1f,
0x0,0x0,0x0,0x0,0x9034,0x1c19,0x1c19,0x1c19,0x1c19,0x1c19,0x1c19,0x1c19,
0x1c19,0x1c19,0x1c19,0x1c19,0x1c19,0x1c19,0x1c19,0x1c19,0x9c35,0x1c19,0x1c19,0x1c19,
0x1c19,0x1c19,0x1c19,0x1c19,0x1c19,0x1c19,
0x0,0x0,0x2c20,0x2c20,0x9036,0x2c20,0x2c20,0x0,0x2c20,0x2c20,0x2c20,0x2c20,
0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2421,0x2c20,
0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c22,0x0,0x0,
0x2c22,0x2c22,0x2c22,0x2c22,0x2c22,0x2c22,
0x0,0x0,0x0,0x0,0x9037,0x1c1c,0x1c1c,0x1c1c,0x1c1c,0x1c1c,0x1c1c,0x1c1c,
0x1c1c,0x1c1c,0x1c1c,0x1c1c,0x1c1c,0x1c1c,0x1c1c,0x1c1c,0x9c38,0x1c1c,0x1c1c,0x1c1c,
0x1c1c,0x1c1c,0x1c1c,0x1c1c,0x1c1c,0x1c1c,
0x0,0x0,0x1c1d,0x1c1d,0x9033,0x1c1d,0x1c1d,0x943a,0x1c1d,0x1c1d,0x1c1d,0x1c1d,
0x1c1d,0x1c1d,0x1c1d,0x1c1d,0x1c1d,0x1c1d,0x903c,0x1c1d,0x1c1d,0x1c1d,0x241e,0x1c1d,
0x1c1d,0x1c1d,0x1c1d,0x1c1d,0x1c1d,0x1c1d,
0x0,0x0,0x2c1d,0x2c1d,0x241d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,
0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x903e,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,0x2c1d,
0x943f,0x9440,0x9441,0x9442,0x9443,0x9444,
0x0,0x0,0x0,0x9035,0x0,0x9035,0x0,0x0,0x0,0x0,0x9035,0x0,
0x9035,0x0,0x0,0x0,0x1c1f,0x1c1f,0x0,0x0,0x0,0x1c1f,0x0,0x2c23,
0x1c1f,0x1c1f,0x1c1f,0x1c1f,0x1c1f,0x1c1f,
0x0,0x0,0x1c20,0x1c20,0x9036,0x1c20,0x1c20,0x9447,0x1c20,0x1c20,0x1c20,0x1c20,
0x1c20,0x1c20,0x1c20,0x1c20,0x1c20,0x1c20,0x9049,0x1c20,0x1c20,0x1c20,0x2421,0x1c20,
0x1c20,0x1c20,0x1c20,0x1c20,0x1c20,0x1c20,
0x0,0x0,0x2c20,0x2c20,0x2420,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,
0x2c20,0x2c20,0x2c20,0x2c20,0x904b,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,0x2c20,
0x944c,0x944d,0x944e,0x944f,0x9450,0x9451,
0x0,0x0,0x0,0x9038,0x0,0x9038,0x0,0x0,0x0,0x0,0x9038,0x0,
0x9038,0x0,0x0,0x0,0x1c22,0x1c22,0x0,0x0,0x0,0x1c22,0x0,0x2c25,
0x1c22,0x1c22,0x1c22,0x1c22,0x1c22,0x1c22,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x2c1f,0x2c1f,0x0,0x0,0x0,0x2c1f,0x0,0x0,
0x2c1f,0x2c1f,0x2c1f,0x2c1f,0x2c1f,0x2c1f,
0x0,0x0,0x0,0x1424,0x0,0x1424,0x0,0x0,0x0,0x0,0x2c28,0x0,
0x4403,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x2c22,0x2c22,0x0,0x0,0x0,0x2c22,0x0,0x0,
0x2c22,0x2c22,0x2c22,0x2c22,0x2c22,0x2c22,
0x0,0x0,0x0,0x1426,0x0,0x1426,0x0,0x0,0x0,0x0,0x2c2a,0x0,
0x9432,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x242b,0x0,0x242b,0x0,0x0,0x0,0x0,0x2c28,0x0,
0x4403,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x242d,0x0,0x242d,0x0,0x0,0x0,0x0,0x1c28,0x0,
0x4403,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x242e,0x0,0x242e,0x0,0x0,0x0,0x0,0x2c2a,0x0,
0x9432,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x2430,0x0,0x2430,0x0,0x0,0x0,0x0,0x1c2a,0x0,
0x9432,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x142b,0x0,0x142b,0x0,0x0,0x0,0x0,0x2c28,0x0,
0x4403,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,
0x905b,0x905b,0x905b,0x905b,0x2c31,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,
0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,
0x0,0x0,0x0,0x142d,0x0,0x142d,0x0,0x0,0x0,0x0,0x0,0x0,
0x4403,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x142e,0x0,0x142e,0x0,0x0,0x0,0x0,0x2c2a,0x0,
0x9432,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,
0x905c,0x905c,0x905c,0x905c,0x2c32,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,
0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,
0x0,0x0,0x0,0x1430,0x0,0x1430,0x0,0x0,0x0,0x0,0x0,0x0,
0x9432,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,
0x905b,0x905b,0x905b,0x905b,0x9c5b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,
0x905b,0x905b,0x905b,0x905b,0x905b,0x905b,
0x0,0x0,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,
0x905c,0x905c,0x905c,0x905c,0x9c5c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,
0x905c,0x905c,0x905c,0x905c,0x905c,0x905c,
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
&SXS_transition[930],
&SXS_transition[960],
&SXS_transition[990],
&SXS_transition[1020],
&SXS_transition[1050],
&SXS_transition[1080],
&SXS_transition[1110],
&SXS_transition[1140],
&SXS_transition[1170],
&SXS_transition[1200],
&SXS_transition[1230],
&SXS_transition[1260],
&SXS_transition[1290],
&SXS_transition[1320],
&SXS_transition[1350],
&SXS_transition[1380],
&SXS_transition[1410],
&SXS_transition[1440],
&SXS_transition[1470],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x4001,3,0,0,0},
{0x4006,4,0,1,0},
{0x2c08,3,0,1,1},
{0x0,0,0,1,1},
{0x2403,2,0,1,1},
{0x2c08,0,0,1,1},
{0x9402,2,0,1,1},
{0x2c08,0,0,1,1},
{0x2c05,3,0,1,1},
{0x0,0,0,1,1},
{0x2c0a,1,0,1,1},
{0x2c02,0,0,1,1},
{0x2c06,3,0,1,1},
{0x0,0,0,1,1},
{0x9024,4,0,1,1},
{0x9022,0,0,1,1},
{0x4405,2,0,1,1},
{0x2c08,0,0,1,1},
{0x2c07,3,0,1,1},
{0x0,0,0,1,1},
{0x9028,4,0,1,1},
{0x9026,0,0,1,1},
{0x4402,2,0,1,1},
{0x2c08,0,0,1,1},
{0x2009,0,0,0,0},
{0x9419,1,0,1,1},
{0x2c05,0,0,1,1},
{0x9419,1,0,1,1},
{0x2c06,0,0,1,1},
{0x4000,2,0,1,1},
{0x2c08,0,0,1,1},
{0x902f,4,0,1,1},
{0x902d,0,0,1,1},
{0x2404,1,0,1,1},
{0x0,0,0,1,1},
{0x2c09,3,0,1,1},
{0x9022,0,0,1,1},
{0x9401,1,0,1,1},
{0x0,0,0,1,1},
{0x2c09,3,0,1,1},
{0x9026,0,0,1,1},
{0x2013,9,63,1,0},
{0x9419,1,0,1,1},
{0x2c0e,0,0,1,1},
{0x4000,1,0,1,1},
{0x0,0,0,1,1},
{0x2c09,3,0,1,1},
{0x902d,0,0,1,1},
{0x2014,9,63,1,0},
{0x4004,3,0,1,0},
{0x9039,10,0,1,0},
{0x9045,10,0,1,0},
{0x2024,9,63,1,0},
{0x9046,10,0,1,0},
{0x9052,10,0,1,0},
{0x2026,9,63,1,0},
{0x9053,9,34,1,0},
{0x2027,9,63,1,0},
{0x4003,0,0,0,0},
{0x943b,1,0,1,1},
{0x2c1d,0,0,1,1},
{0x2c2c,10,0,1,0},
{0x201d,9,8,1,0},
{0x201d,9,12,1,0},
{0x201d,9,10,1,0},
{0x201d,9,13,1,0},
{0x201d,9,9,1,0},
{0x201d,9,11,1,0},
{0x9054,9,62,1,0},
{0x9055,9,34,1,0},
{0x2029,9,63,1,0},
{0x9032,0,0,0,0},
{0x9448,1,0,1,1},
{0x2c20,0,0,1,1},
{0x2c2f,10,0,1,0},
{0x2020,9,8,1,0},
{0x2020,9,12,1,0},
{0x2020,9,10,1,0},
{0x2020,9,13,1,0},
{0x2020,9,9,1,0},
{0x2020,9,11,1,0},
{0x9056,9,62,1,0},
{0x9057,2,0,0,0},
{0x9058,2,0,0,0},
{0x9059,2,0,0,0},
{0x905a,2,0,0,0},
{0x9c5d,9,41,1,0},
{0x9c5e,9,41,1,0},
{0x9c5f,9,41,1,0},
{0x9c60,9,41,1,0},
{0x201d,1,0,0,0},
{0x2020,1,0,0,0},
{0x201d,11,0,1,0},
{0x2019,11,0,1,0},
{0x2020,11,0,1,0},
{0x201c,11,0,1,0},
};
static char *S_adrp[]={0,
"%RULE",
"=",
"%ATTR",
"%ATTR_DEF",
",",
"%FIN",
"End Of File",
};
static SXINT S_is_a_keyword[]={
0};
static SXINT S_is_a_generic_terminal[]={
4,1,3,4,6};
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
3,10,34,60};
static SXINT S_no_insert[]={
2,34,60};
static char *S_global_mess[]={0,
"%s\"%s\" is deleted.",
"%sThis unknown keyword is erased.",
"End Of Line",
"End of File",
"%sScanning stops on End of File.",
};
#ifdef SCANACT
extern SXSCANACT_FUNCTION SCANACT;
#endif
#ifdef SEMACT
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
extern SXSEMACT_FUNCTION SEMACT;
#pragma GCC diagnostic pop
#endif

struct sxtables sxtables={
52113, /* magic */
sxscanner,
sxparser,
{255, 7, 1, 3, 4, 30, 1, 50, 1, 0, 0, 
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
#ifdef SCANACT
SCANACT,
#else
(SXSCANACT_FUNCTION *) NULL,
#endif
sxsrecovery,
(SXCHECK_KEYWORD_FUNCTION *) NULL
},
{8, 20, 20, 28, 28, 33, 42, 52, 7, 13, 19, 19, 13, 9, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-42,
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
P_global_mess,PER_tset,
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
/* End of sxtables for def */
