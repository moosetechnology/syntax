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


/* Beginning of sxtables for dec [Thu Nov 29 14:04:25 2012] */
#define SXP_MAX 20
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{3, 3, 0, -9},
{2, 2, 1, -9},
{5, 5, 0, -13},
{0, 0, 2, -12},
{1, 1, 1, -12},
{6, 6, 0, -14},
{4, 4, 4, 20},
{7, 7, 2, -14},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-5, 3, 4},
{0, 6, 5},
{0, 3, 4},
{0, -4, 7},
{0, 10, 1},
{0, 18, 0},
{0, 7, 3},
{0, 8, 5},
{0, 11, 7},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-2, -13, 4},
{-14, 0, 0},
{-13, 20, 0},
};
static struct SXP_item vector[]={
{0, 0},
{2, 15},
{1, -12},
{2, -9},
{3, -1},
{6, 16},
{0, 0},
};

static  SXBA_ELT SXPBA_kt4
#if SXBITS_PER_LONG==32
 [3] = {10, 0X0000020A, 0X00000000, }
#else
 [2] = {10, 0X000000000000020A, }
#endif
/* End SXPBA_kt4 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt4[0],
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
1,4};

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
3,3,3,3,3,3,3,3,3,3,3,3,4,5,4,4,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,6,3,7,3,3,8,9,10,11,3,12,3,3,
13,14,14,14,14,14,14,14,14,14,14,3,15,16,3,17,3,3,18,18,18,18,18,18,18,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,3,19,3,3,20,3,
18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
18,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x4007,0x9004,0x9006,0x9008,0x900a,0x900c,0x900e,0x9010,0x9012,0x9014,0x9016,
0x9018,0x9004,0x901a,0x901c,0x9004,0x9004,0x9004,0x9004,
0x0,0x4000,0x4000,0x901e,0x9020,0x4000,0x4000,0x4000,0x4000,0x4000,0x9022,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x0,0x0,0x1403,0x1403,0x2c0a,0x1403,0x1403,0x1403,0x1403,0x1403,0x1403,0x1403,
0x1403,0x1403,0x1403,0x1403,0x1403,0x1403,0x1403,0x1403,
0x0,0x4000,0x1c04,0x1c04,0x1c04,0x900a,0x9024,0x900e,0x1c04,0x1c04,0x9026,0x1c04,
0x9018,0x1c04,0x1c04,0x9024,0x1c04,0x1c04,0x1c04,0x1c04,
0x0,0x4000,0x2c04,0x2c04,0x2c04,0x900a,0x9024,0x900e,0x2c04,0x2c04,0x2c0c,0x2c04,
0x9018,0x2c04,0x2c04,0x9024,0x2c04,0x2c04,0x2c04,0x2c04,
0x0,0x0,0x1c06,0x1c06,0x0,0x2c0d,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,
0x1c06,0x1c06,0x1c06,0x9029,0x1c06,0x1c06,0x2c0e,0x1c06,
0x0,0x0,0x1c07,0x1c07,0x0,0x1c07,0x1c07,0x2c0d,0x1c07,0x1c07,0x1c07,0x1c07,
0x1c07,0x1c07,0x1c07,0x902b,0x1c07,0x1c07,0x2c0f,0x1c07,
0x0,0x0,0x1c08,0x1c08,0x0,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,0x1c08,
0x1c08,0x1c08,0x1c08,0x1c08,0x4c05,0x1c08,0x1c08,0x1c08,
0x0,0x0,0x1409,0x1409,0x2c02,0x1409,0x1409,0x1409,0x1409,0x1409,0x1409,0x1409,
0x1409,0x1409,0x1409,0x1409,0x1409,0x1409,0x1409,0x1409,
0x0,0x4000,0x9004,0x9006,0x9008,0x900a,0x9024,0x900e,0x903d,0x903d,0x9014,0x903d,
0x9018,0x9004,0x903f,0x9041,0x9004,0x9004,0x9004,0x9004,
0x0,0x0,0x140b,0x140b,0x2c0d,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x0,0x0,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x2c11,0x1c0c,
0x1c0c,0x1c0c,0x1c0c,0x904b,0x1c0c,0x1c0c,0x1c0c,0x1c0c,
0x0,0x4000,0x9004,0x9004,0x9004,0x900a,0x9024,0x900e,0x9004,0x9004,0x9026,0x9004,
0x9018,0x9004,0x9004,0x9024,0x9004,0x9004,0x9004,0x9004,
0x0,0x0,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,
0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,0x2c06,
0x0,0x0,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,
0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x2412,0x0,0x0,0x0,0x0,
0x0,0x0,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x1c11,0x2c0c,
0x2c0d,0x2c0c,0x2c0c,0x904b,0x2c0c,0x2c0c,0x2c0c,0x2c0c,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x2c13,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x1c13,0x0,0x0,0x4c04,0x1c13,0x0,0x2c14,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x2c13,0x0,0x0,0x0,0x2c13,0x0,0x0,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[20],
&SXS_transition[40],
&SXS_transition[60],
&SXS_transition[80],
&SXS_transition[100],
&SXS_transition[120],
&SXS_transition[140],
&SXS_transition[160],
&SXS_transition[180],
&SXS_transition[200],
&SXS_transition[220],
&SXS_transition[240],
&SXS_transition[260],
&SXS_transition[280],
&SXS_transition[300],
&SXS_transition[320],
&SXS_transition[340],
&SXS_transition[360],
&SXS_transition[380],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x4007,11,0,1,0},
{0x2010,3,0,1,0},
{0x4003,4,0,1,0},
{0x2c04,3,0,1,1},
{0x0,0,0,1,1},
{0x2c04,3,0,1,1},
{0x2402,0,0,1,1},
{0x2c04,3,0,1,1},
{0x2c02,0,0,1,1},
{0x2c06,3,0,1,1},
{0x0,0,0,1,1},
{0x902f,4,0,1,1},
{0x902d,0,0,1,1},
{0x2c07,3,0,1,1},
{0x0,0,0,1,1},
{0x2c04,3,0,1,1},
{0x4401,0,0,1,1},
{0x2c04,3,0,1,1},
{0x4402,0,0,1,1},
{0x9033,4,0,1,1},
{0x9031,0,0,1,1},
{0x2c04,3,0,1,1},
{0x4406,0,0,1,1},
{0x2c05,3,0,1,1},
{0x0,0,0,1,1},
{0x9403,2,0,1,1},
{0x2c04,0,0,1,1},
{0x9037,4,0,1,1},
{0x9035,0,0,1,1},
{0x4000,3,0,1,1},
{0x2402,0,0,1,1},
{0x4000,3,0,1,1},
{0x2c02,0,0,1,1},
{0x903b,4,0,1,1},
{0x9039,0,0,1,1},
{0x9045,4,0,1,1},
{0x9043,0,0,1,1},
{0x9049,4,0,1,1},
{0x9047,0,0,1,1},
{0x200d,0,0,0,0},
{0x9428,1,0,1,1},
{0x2c06,0,0,1,1},
{0x9428,1,0,1,1},
{0x2c07,0,0,1,1},
{0x9c02,1,0,1,1},
{0x0,0,0,1,1},
{0x2c04,3,0,1,1},
{0x902d,0,0,1,1},
{0x2403,1,0,1,1},
{0x0,0,0,1,1},
{0x2c04,3,0,1,1},
{0x9031,0,0,1,1},
{0x9c01,1,0,1,1},
{0x0,0,0,1,1},
{0x2c08,2,0,1,1},
{0x2c04,0,0,1,1},
{0x2409,1,0,1,1},
{0x0,0,0,1,1},
{0x4000,3,0,1,1},
{0x9039,0,0,1,1},
{0x2c04,3,0,1,1},
{0x4000,0,0,1,1},
{0x4000,2,0,1,1},
{0x2c04,0,0,1,1},
{0x903f,4,0,1,1},
{0x9043,0,0,1,1},
{0x4000,1,0,1,1},
{0x0,0,0,1,1},
{0x2c04,3,0,1,1},
{0x9043,0,0,1,1},
{0x240b,1,0,1,1},
{0x0,0,0,1,1},
{0x2c04,3,0,1,1},
{0x9047,0,0,1,1},
{0x9428,1,0,1,1},
{0x2c0c,0,0,1,1},
};
static char *S_adrp[]={0,
"(",
")",
";",
"%ATTR",
"%NT",
",",
"End Of File",
};
static SXINT S_is_a_keyword[]={
0};
static SXINT S_is_a_generic_terminal[]={
2,4,5};
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
"%sLexical analysis of declarations stops.",
};
#ifdef SCANACT
extern SXINT SCANACT(SXINT what, SXINT act_no);
#endif /* SCANACT */
extern SXINT sxscan_it(void);
extern SXBOOLEAN sxsrecovery (SXINT sxsrecovery_what, SXINT state_no, unsigned char *class);
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
#ifdef SEMACT
#ifndef SCANACT_AND_SEMACT_ARE_IDENTICAL
extern SXINT SEMACT(SXINT what, struct sxtables *arg);
#endif
#endif /* SEMACT */

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(SXPARSER_T) sxparser}, {255, 7, 1, 3, 4, 20, 1, 20, 1, 0, 0, 
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
NULL,
},
{2, 8, 8, 11, 11, 17, 19, 25, 7, 5, 7, 7, 9, 4, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-19,
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
/* End of sxtables for dec [Thu Nov 29 14:04:25 2012] */
