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


/* Beginning of sxtables for dec [Thu Nov 29 14:04:23 2012] */
#define SXP_MAX 53
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{0, 13, 0, -22},
{1, 1, 1, -27},
{0, 12, 1, -22},
{5, 5, 2, -2},
{3, 3, 0, -21},
{17, 17, 0, -29},
{4, 4, 0, -21},
{0, 0, 2, -27},
{7, 7, 0, -23},
{2, 2, 1, -21},
{14, 14, 1, 43},
{15, 15, 1, 43},
{6, 6, 1, -23},
{9, 9, 0, -24},
{18, 18, 0, -30},
{8, 8, 1, -24},
{11, 11, 0, -4},
{0, 16, 3, -28},
{10, 10, 1, -4},
{19, 19, 2, -30},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-9, 31, 2},
{0, 44, 45},
{-14, 32, 3},
{-17, 33, 4},
{0, 15, 10},
{-7, 44, 47},
{0, -8, 12},
{0, 46, 0},
{0, 25, 7},
{0, 48, 0},
{0, 13, 1},
{0, 16, 1},
{0, 19, 1},
{0, 20, 10},
{0, 26, 12},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-23, -2, 3},
{-3, 36, 37},
{-24, 0, 0},
{-4, 0, 0},
{-30, 0, 0},
{-29, 36, 53},
};
static struct SXP_item vector[]={
{9, -28},
{1, 10},
{10, -29},
{1, 5},
{0, 0},
{0, 0},
{5, 11},
{6, 12},
{9, 6},
{1, -27},
{2, -21},
{8, 18},
{4, -22},
{0, 0},
{11, 34},
{0, 0},
{8, -1},
{0, 0},
{0, 0},
};

static  SXBA_ELT SXPBA_kt1
#if SXBITS_PER_LONG==32
 [3] = {16, 0X0000B85E, 0X00000000, }
#else
 [2] = {16, 0X000000000000B85E, }
#endif
/* End SXPBA_kt1 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt1[0],
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
"the Axiom",
"%sParsing stops on the Axiom.",
};
static SXINT PER_tset[]={
1,1};

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
3,3,3,3,3,3,3,3,3,3,3,3,4,5,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,3,3,6,3,3,3,7,8,9,3,10,3,3,
11,12,12,12,12,12,12,12,12,12,12,13,14,15,16,17,3,3,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,3,3,3,3,19,3,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
18,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x400c,0x0,0x2402,0x9003,0x9005,0x4407,0x4408,0x9007,0x440b,0x0,0x0,
0x2403,0x2404,0x9009,0x0,0x0,0x900b,0x0,
0x0,0x4000,0x0,0x1402,0x900d,0x4000,0x4000,0x4000,0x9007,0x4000,0x0,0x0,
0x4000,0x4000,0x4000,0x0,0x0,0x4000,0x0,
0x0,0x0,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x2409,0x9010,0x1c03,0x1c03,0x1c03,0x1c03,
0x0,0x0,0x1404,0x1404,0x240a,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x0,0x0,0x1c05,0x1c05,0x0,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,
0x1c05,0x1c05,0x1c05,0x1c05,0x4c0a,0x1c05,0x1c05,
0x0,0x0,0x1406,0x1406,0x2402,0x1406,0x1406,0x1406,0x1406,0x1406,0x1406,0x1406,
0x1406,0x1406,0x1406,0x1406,0x1406,0x1406,0x1406,
0x0,0x0,0x0,0x2c0c,0x4c01,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x2c0d,0x0,
0x0,0x0,0x0,0x240e,0x9012,0x9012,0x0,0x0,0x9012,0x0,0x0,0x0,
0x0,0x0,0x0,0x240f,0x0,0x1c08,0x0,
0x0,0x0,0x1409,0x1409,0x2410,0x1409,0x1409,0x1409,0x1409,0x1409,0x1409,0x1409,
0x1409,0x1409,0x1409,0x1409,0x1409,0x1409,0x1409,
0x0,0x0,0x2404,0x2404,0x9013,0x4006,0x2404,0x2404,0x4006,0x2404,0x2404,0x2404,
0x2404,0x2404,0x0,0x2404,0x2404,0x2404,0x2404,
0x0,0x4000,0x1c0b,0x1c0b,0x1c0b,0x9015,0x1c0b,0x1c0b,0x9017,0x1c0b,0x2c11,0x1c0b,
0x1c0b,0x1c0b,0x9015,0x1c0b,0x1c0b,0x1c0b,0x1c0b,
0x0,0x0,0x0,0x1c0c,0x4c01,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x4009,0x4009,0x0,0x4009,0x0,0x4009,0x0,0x0,0x1c0d,
0x0,0x0,0x0,0x0,0x0,0x1c0d,0x2c13,
0x0,0x0,0x0,0x140e,0x9012,0x9012,0x0,0x0,0x9012,0x0,0x0,0x0,
0x0,0x0,0x0,0x240f,0x0,0x0,0x0,
0x0,0x0,0x0,0x140f,0x9012,0x9012,0x0,0x0,0x9012,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x2409,0x2409,0x9019,0x4005,0x2409,0x2409,0x4005,0x2409,0x2409,0x2409,
0x2409,0x2409,0x0,0x2409,0x2409,0x2409,0x2409,
0x0,0x4000,0x2c0b,0x2c0b,0x2c0b,0x9015,0x2c0b,0x2c0b,0x2c14,0x2c0b,0x1c11,0x2c0b,
0x2c0b,0x2c0b,0x9015,0x2c0b,0x2c0b,0x2c0b,0x2c0b,
0x0,0x0,0x1412,0x1412,0x240b,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,
0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c0d,
0x0,0x0,0x0,0x0,0x0,0x2c0d,0x0,
0x0,0x0,0x1c14,0x1c14,0x1c14,0x1c14,0x1c14,0x1c14,0x2c15,0x1c14,0x1c14,0x1c14,
0x1c14,0x1c14,0x1c14,0x1c14,0x1c14,0x1c14,0x1c14,
0x0,0x0,0x2c14,0x2c14,0x2c14,0x2c14,0x2c14,0x2c14,0x1c15,0x2c14,0x2c0b,0x2c14,
0x2c14,0x2c14,0x2c14,0x2c14,0x2c14,0x2c14,0x2c14,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[19],
&SXS_transition[38],
&SXS_transition[57],
&SXS_transition[76],
&SXS_transition[95],
&SXS_transition[114],
&SXS_transition[133],
&SXS_transition[152],
&SXS_transition[171],
&SXS_transition[190],
&SXS_transition[209],
&SXS_transition[228],
&SXS_transition[247],
&SXS_transition[266],
&SXS_transition[285],
&SXS_transition[304],
&SXS_transition[323],
&SXS_transition[342],
&SXS_transition[361],
&SXS_transition[380],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x200b,4,0,1,0},
{0x400c,11,0,1,0},
{0x9401,2,0,1,1},
{0x2402,0,0,1,1},
{0x2c07,1,0,1,1},
{0x0,0,0,1,1},
{0x2406,1,0,1,1},
{0x0,0,0,1,1},
{0x9c02,1,0,1,1},
{0x2c05,0,0,1,1},
{0x2c08,1,0,1,1},
{0x0,0,0,1,1},
{0x4000,2,0,1,1},
{0x2402,0,0,1,1},
{0x4005,0,0,0,0},
{0x940f,1,0,1,1},
{0x2c03,0,0,1,1},
{0x3000,3,0,1,0},
{0x4006,2,0,1,1},
{0x240a,0,0,1,1},
{0x4000,1,0,1,1},
{0x2c0b,0,0,1,1},
{0x2412,1,0,1,1},
{0x2c0b,0,0,1,1},
{0x4005,2,0,1,1},
{0x2410,0,0,1,1},
};
static char *S_adrp[]={0,
"%DOLLAR",
"decl",
"open",
"close",
"%TYPE",
";",
"(",
")",
"%ATTR",
"%NT",
",",
"End Of File",
};
static SXINT S_is_a_keyword[]={
3,2,3,4};
static SXINT S_is_a_generic_terminal[]={
4,1,5,9,10};
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
"<",
"%sScanning stops on the Axiom.",
};
#ifdef SCANACT
extern SXINT SCANACT(SXINT what, SXINT act_no);
#endif /* SCANACT */
extern SXINT sxscan_it(void);
extern SXBOOLEAN sxsrecovery (SXINT sxsrecovery_what, SXINT state_no, unsigned char *class);
static SXINT check_keyword(char *init_string, SXINT init_length);
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
#ifdef SEMACT
#ifndef SCANACT_AND_SEMACT_ARE_IDENTICAL
extern SXINT SEMACT(SXINT what, struct sxtables *arg);
#endif
#endif /* SEMACT */

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(SXPARSER_T) sxparser}, {255, 12, 1, 3, 4, 19, 1, 21, 1, 0, 0, 
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
NULL,
#endif /* SCANACT */
(SXRECOVERY_T) sxsrecovery,
check_keyword,
},
{4, 20, 20, 26, 26, 35, 45, 63, 12, 11, 19, 19, 15, 7, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-45,
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
(SXPARSACT_T) NULL,
(SXDESAMBIG_T) NULL,
#ifdef SEMACT
(SXSEMACT_T) SEMACT
#else /* SEMACT */
NULL,
#endif /* SEMACT */
},
err_titles,
abstract,
(sxsem_tables*)NULL,
NULL,
};

#include	"sxdico.h"

#define KW_NB		3
#define INIT_BASE	1
#define INIT_SUFFIX	0
#define CLASS_MASK	3
#define STOP_MASK	4
#define BASE_SHIFT	4
#define SUFFIX_MASK	8

static SXINT kw_code2t_code [4] = {0,
4, 3, 2, 
};

static SXINT kw_code2lgth [4] = {0,
5, 4, 4, 
};

static SXUINT comb_vector [5] = {
0, 0, 21, 54, 39, 
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
'\000', '\000', '\000', '\000', '\001', '\002', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\003', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
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

/* End of sxtables for dec [Thu Nov 29 14:04:23 2012] */
