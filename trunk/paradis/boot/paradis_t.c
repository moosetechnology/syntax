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

/* Beginning of sxtables for paradis */
#define SXP_MAX 58
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{6, 3, 0, -30},
{2, 2, 1, -30},
{2, 5, 1, -31},
{1, 16, 1, -41},
{1, 17, 1, -41},
{1, 18, 1, -41},
{1, 19, 1, -41},
{1, 20, 1, -41},
{1, 21, 1, -41},
{1, 22, 1, -41},
{6, 6, 1, -31},
{0, 0, 2, -40},
{0, 1, 1, -40},
{0, 4, 1, 53},
{5, 7, 0, -3},
{5, 8, 0, -3},
{5, 9, 0, -3},
{5, 10, 0, -3},
{5, 11, 0, -3},
{5, 12, 0, -3},
{1, 15, 0, -41},
{1, 14, 0, -41},
{1, 28, 0, -4},
{1, 23, 0, 58},
{1, 13, 2, -3},
{5, 27, 1, -4},
{5, 26, 1, -4},
{5, 24, 2, 58},
{5, 25, 3, -6},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-13, 42, 2},
{0, 54, 0},
{-22, 55, 0},
{-23, 57, 0},
{-24, 45, 18},
{-24, 46, 18},
{0, 42, 2},
{-24, 46, 18},
{-24, 46, 18},
{-24, 46, 18},
{0, -12, 23},
{0, 25, 10},
{0, 11, 3},
{0, 26, 19},
{0, 27, 19},
{0, 56, 0},
{0, 47, 19},
{0, 28, 20},
{0, 49, 19},
{0, 29, 20},
{0, 36, 23},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-2, -31, 4},
{-3, 0, 0},
{-41, 0, 0},
{-4, 0, 0},
{-6, -5, 8},
{-7, 0, 0},
{-31, 51, 0},
{-8, 0, 0},
{-9, 0, 0},
{-10, 0, 0},
};
static struct SXP_item vector[]={
{1, -40},
{2, -30},
{3, -1},
{1, 14},
{0, 0},
{0, 0},
{4, 15},
{5, 16},
{6, 17},
{7, 18},
{8, 19},
{9, 20},
{10, 32},
{0, 0},
{11, 21},
{12, 33},
{13, 34},
{14, 35},
{15, 37},
{16, 38},
{17, 39},
{0, 0},
{0, 0},
{19, 47},
{0, 0},
{21, 48},
{21, 43},
{22, 44},
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
"Tilda",
"%sParsing stops on Tilda.",
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
3,3,3,3,3,3,3,3,3,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,3,3,3,3,3,3,3,3,4,5,6,7,7,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,8,9,10,11,9,12,13,9,14,15,16,17,9,18,9,
9,19,19,19,19,19,19,19,19,19,19,9,20,21,22,23,9,24,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,9,26,9,9,27,9,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,9,9,9,28,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,};
static SXSTMI SXS_transition[]={0,
0x0,0x4017,0x0,0x0,0x9021,0x9023,0x940b,0x9025,0x940c,0x940d,0x940e,0x940f,
0x9410,0x9027,0x9029,0x902b,0x902d,0x902f,0x9031,0x9411,0x9033,0x9035,0x940c,0x9413,
0x9037,0x940c,0x940c,0x9039,
0x0,0x0,0x1402,0x1402,0x1402,0x240b,0x1402,0x1402,0x1402,0x1402,0x1402,0x1402,
0x1402,0x1402,0x1402,0x1402,0x1402,0x1402,0x1402,0x1402,0x1402,0x1402,0x1402,0x1402,
0x1402,0x1402,0x1402,0x1402,
0x0,0x0,0x0,0x0,0x9040,0x9042,0x940b,0x9044,0x9001,0x9001,0x9001,0x9001,
0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,
0x9001,0x9001,0x9001,0x9001,
0x0,0x0,0x0,0x0,0x9002,0x9002,0x0,0x9002,0x940c,0x940c,0x940c,0x940c,
0x940c,0x940c,0x940c,0x9046,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,
0x940c,0x940c,0x940c,0x940c,
0x0,0x0,0x1405,0x9448,0x9449,0x0,0x1405,0x1405,0x1405,0x944a,0x1405,0x1405,
0x1405,0x1405,0x1405,0x1405,0x1405,0x1405,0x1405,0x1405,0x1405,0x1405,0x1405,0x1405,
0x1405,0x944b,0x1405,0x1405,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x940c,0x940c,0x940c,0x940c,
0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,
0x940c,0x940c,0x940c,0x940c,
0x0,0x0,0x0,0x0,0x9002,0x9002,0x0,0x9002,0x940c,0x940c,0x940c,0x940c,
0x940c,0x940c,0x940c,0x9046,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,0x940c,
0x944c,0x940c,0x940c,0x940c,
0x0,0x0,0x0,0x0,0x9003,0x9003,0x0,0x9003,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x9003,0x0,0x0,0x9410,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x9412,0x9412,0x9412,0x9412,0x9412,
0x9412,0x9412,0x9412,0x9412,0x9412,0x9412,0x9412,0x9412,0x9412,0x9412,0x944d,0x9412,
0x9412,0x9412,0x9412,0x9412,
0x0,0x0,0x0,0x0,0x9004,0x9004,0x0,0x9004,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x9004,0x0,0x0,0x9413,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x4000,0x0,0x0,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x904e,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,
0x0,0x4000,0x0,0x0,0x9050,0x9052,0x4000,0x9054,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,
0x0,0x0,0x0,0x0,0x4013,0x4013,0x0,0x4013,0x0,0x0,0x0,0x0,
0x0,0x0,0x4013,0x4013,0x0,0x0,0x9c1f,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x4013,
0x0,0x0,0x0,0x0,0x3000,0x3000,0x0,0x3000,0x0,0x0,0x0,0x0,
0x0,0x3000,0x0,0x3000,0x3000,0x3000,0x0,0x0,0x0,0x0,0x0,0x0,
0x9c20,0x0,0x0,0x3000,
0x0,0x0,0x940d,0x9448,0x9449,0x9457,0x940d,0x940d,0x940d,0x940d,0x940d,0x940d,
0x940d,0x940d,0x940d,0x940d,0x940d,0x940d,0x940d,0x940d,0x940d,0x940d,0x940d,0x940d,
0x940d,0x940d,0x940d,0x940d,
0x0,0x0,0x0,0x0,0x9008,0x9008,0x0,0x9008,0x940c,0x940c,0x940c,0x940c,
0x940c,0x940c,0x940c,0x9059,0x940c,0x940c,0x944c,0x940c,0x940c,0x940c,0x940c,0x940c,
0x944c,0x940c,0x9458,0x940c,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x9456,0x9456,0x9456,0x9456,0x9456,
0x9456,0x9456,0x9456,0x9456,0x9456,0x9456,0x9456,0x9456,0x9456,0x9456,0x945b,0x9456,
0x9456,0x9456,0x9456,0x9456,
0x0,0x0,0x0,0x0,0x9002,0x9002,0x0,0x9002,0x940c,0x940c,0x940c,0x940c,
0x940c,0x940c,0x940c,0x9046,0x940c,0x940c,0x944c,0x940c,0x940c,0x940c,0x940c,0x940c,
0x944c,0x940c,0x940c,0x940c,
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
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x6009,2,0,0,0},
{0x5006,2,0,0,0},
{0x6008,2,0,0,0},
{0x6005,2,0,0,0},
{0x6003,2,0,0,0},
{0x600a,2,0,0,0},
{0x6004,2,0,0,0},
{0x6007,2,0,0,0},
{0x2003,3,0,0,0},
{0x903b,12,1,1,0},
{0x2003,12,2,1,0},
{0x2004,12,2,1,0},
{0x2005,12,2,1,0},
{0x2006,12,2,1,0},
{0x2007,12,2,1,0},
{0x2008,12,2,1,0},
{0x4001,1,0,0,0},
{0x2009,12,2,1,0},
{0x200a,12,2,1,0},
{0x200c,12,2,1,0},
{0x200c,3,0,0,0},
{0x200c,3,2,1,0},
{0x903c,3,0,1,0},
{0x903d,4,0,1,0},
{0x903e,3,0,1,0},
{0x903f,4,0,1,0},
{0x4012,12,2,1,0},
{0x4014,12,2,1,0},
{0x4016,12,2,1,0},
{0x4015,12,2,1,0},
{0x200d,12,2,1,0},
{0x200e,12,2,1,0},
{0x9415,2,0,1,1},
{0x9409,0,0,1,1},
{0x9416,2,0,1,1},
{0x940a,0,0,1,1},
{0x9414,2,0,1,1},
{0x940b,0,0,1,1},
{0x941b,2,0,1,1},
{0x940c,0,0,1,1},
{0x941c,2,0,1,1},
{0x940c,0,0,1,1},
{0x2402,1,0,1,1},
{0x940c,0,0,1,1},
{0x941d,2,0,1,1},
{0x940c,0,0,1,1},
{0x941e,2,0,1,1},
{0x940c,0,0,1,1},
{0x9c1f,2,0,1,1},
{0x940c,0,0,1,1},
{0x9417,1,0,1,1},
{0x9412,0,0,1,1},
{0x9418,2,0,1,1},
{0x940c,0,0,1,1},
{0x9c20,2,0,1,1},
{0x940c,0,0,1,1},
{0x9419,3,0,1,1},
{0x941a,0,0,1,1},
{0x2003,3,2,1,0},
{0x9056,3,2,1,0},
{0x9005,12,2,1,0},
{0x400a,12,2,1,0},
{0x9006,12,2,1,0},
{0x9001,2,0,1,1},
{0x9409,0,0,1,1},
{0x9001,2,0,1,1},
{0x940a,0,0,1,1},
{0x9001,2,0,1,1},
{0x940b,0,0,1,1},
{0x9002,1,0,1,1},
{0x940c,0,0,1,1},
{0x2005,13,2,1,0},
{0x2005,3,0,0,0},
{0x9002,12,2,1,0},
{0x200f,12,2,1,0},
{0x2010,12,2,1,0},
{0x9007,12,2,1,0},
{0x2402,1,0,1,1},
{0x4000,0,0,1,1},
{0x9415,2,0,1,1},
{0x4000,0,0,1,1},
{0x9416,2,0,1,1},
{0x4000,0,0,1,1},
{0x9414,2,0,1,1},
{0x4000,0,0,1,1},
{0x2011,12,2,1,0},
{0x905c,12,1,1,0},
{0x2012,12,2,1,0},
{0x9008,1,0,1,1},
{0x940c,0,0,1,1},
{0x4002,12,2,1,0},
{0x2005,3,2,1,0},
};
static char *S_adrp[]={0,
";",
"LHS_NON_TERMINAL",
"%LHS",
"%NON_TERMINAL",
"%ACTION",
"%TERMINAL",
"%GENERIC_TERMINAL",
"%PREDICATE",
"%BLANKS",
"~",
"INH",
"MARGIN",
"SPACE",
"COL",
"SKIP",
"PAGE",
"TAB",
"(",
"%NUMBER",
")",
"-",
"+",
"End Of File",
};
static SXINT S_is_a_keyword[]={
7,11,12,13,14,15,16,17};
static SXINT S_is_a_generic_terminal[]={
8,3,4,5,6,7,8,9,19};
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
"Tilda",
"%sScanning stops on Tilda.",
};
#ifdef SCANACT
extern SXINT SCANACT(SXINT what, SXINT act_no);
#endif /* SCANACT */
extern SXINT sxscan_it(void);
extern SXBOOLEAN sxsrecovery (SXINT sxsrecovery_what, SXINT state_no, unsigned char *class);
static SXINT check_keyword(char *init_string, SXINT init_length);
static struct SXT_node_info SXT_node_info[]={{0,0},
{0,1},{0,2},{16,4},{0,5},{0,6},{20,8},{13,9},{2,10},{19,11},{8,12},{15,13},{4,14},
{14,15},{21,16},{9,16},{11,16},{17,17},{17,18},{5,19},{12,20},{6,21},{18,22},{21,23},{7,23},
{3,24},{7,25},{3,26},{21,27},{0,27},};
static SXINT T_ter_to_node_name[]={
0,0,0,10,};
static char *T_node_name[]={NULL,
"ERROR",
"ACTION",
"BACKWARD",
"BLANKS",
"COLUMN",
"FORM_FEED",
"FORWARD",
"GENERIC_TERMINAL",
"INHIBITION",
"LHS",
"MARGIN",
"NEW_LINE",
"NON_TERMINAL",
"PP_SPEC",
"PREDICATE",
"RULE_S",
"SPACE",
"TABULATION",
"TERMINAL",
"VOCABULARY_S",
"VOID",
};
extern SXINT sempass(SXINT what, struct sxtables *sxtables_ptr);
static char T_stack_schema[]={0,
0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,0,
0,0,};

static struct SXT_tables SXT_tables=
{SXT_node_info, T_ter_to_node_name, T_stack_schema, sempass, T_node_name};
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxatc(SXINT what, ...);

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(SXPARSER_T) sxparser}, {255, 23, 1, 3, 4, 28, 3, 18, 1, 1, 0, 
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
(SXSCANACT_T) SCANACT,
#else /* SCANACT */
(SXSCANACT_T) NULL,
#endif /* SCANACT */
(SXRECOVERY_T) sxsrecovery,
check_keyword,
},
{10, 29, 29, 36, 39, 50, 52, 80, 23, 9, 28, 28, 21, 11, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-52,
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
(SXPARSER_T) NULL,
(SXDESAMBIG_T) NULL,
(SXSEMACT_T) sxatc
},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables,
NULL,
};

#include	"sxdico.h"

#define KW_NB		7
#define INIT_BASE	1
#define INIT_SUFFIX	0
#define CLASS_MASK	7
#define STOP_MASK	8
#define BASE_SHIFT	5
#define SUFFIX_MASK	16

static SXINT kw_code2t_code [8] = {0,
17, 16, 15, 14, 13, 12, 11, 
};

static SXINT kw_code2lgth [8] = {0,
3, 4, 4, 3, 5, 6, 3, 
};

static SXUINT comb_vector [11] = {
0, 0, 137, 234, 203, 76, 101, 172, 47, 
110, 0, 
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
'\000', '\000', '\000', '\000', '\001', '\000', '\000', '\000', 
'\000', '\000', '\002', '\000', '\006', '\000', '\003', '\000', 
'\000', '\004', '\000', '\000', '\005', '\007', '\000', '\000', 
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
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', 
};

#include	"sxcheck_keyword.h"

/* End of sxtables for paradis */
