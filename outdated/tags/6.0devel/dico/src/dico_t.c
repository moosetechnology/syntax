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


/* Beginning of sxtables for dico [Thu Nov 29 14:04:15 2012] */
#define SXP_MAX 15
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{1, 6, 1, -11},
{3, 4, 1, -10},
{0, 0, 2, -9},
{4, 3, 1, -10},
{6, 1, 1, -9},
{5, 2, 1, -9},
{2, 5, 2, -11},
};
static struct SXP_bases t_bases[]={{0,0,0},
{0, 12, 1},
{0, -3, 3},
{-5, 4, 1},
{-6, 13, 1},
{-2, 1, 2},
{0, 7, 2},
{0, 8, 3},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-11, 15, 0},
};
static struct SXP_item vector[]={
{1, -9},
{2, -10},
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
3,3,3,3,3,3,3,3,7,3,7,3,7,7,3,3,7,3,7,7,3,7,7,7,7,
3,7,3,7,7,3,3,7,3,7,7,3,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,3,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,3,7,7,7,7,7,
7,7,7,3,3,3,3,3,3,3,3,3,4,5,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,6,3,3,3,3,7,3,3,3,3,3,7,8,
9,10,10,10,10,10,10,10,10,11,11,3,3,3,3,3,3,3,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,3,12,3,3,7,3,
7,13,7,7,7,14,7,7,7,7,7,7,7,15,7,7,7,16,7,17,7,18,7,7,7,
7,3,8,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,7,3,7,3,7,7,3,3,7,3,7,
7,3,7,7,7,7,3,7,3,7,7,3,3,7,3,7,7,3,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,3,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
3,7,7,7,7,7,7,7,7,};
static SXSTMI SXS_transition[]={0,
0x0,0x4003,0x0,0x2402,0x2402,0x2403,0x2c04,0x0,0x2405,0x2c06,0x2c06,0x0,
0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,0x2c04,
0x0,0x4000,0x0,0x1402,0x1402,0x4000,0x4000,0x0,0x4000,0x4000,0x4000,0x0,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x0,0x0,0x1c03,0x1c03,0x0,0x4401,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x2407,
0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x0,0x4001,0x0,0x4001,0x4001,0x4001,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x0,
0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,0x1c04,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2408,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x4002,0x0,0x4002,0x4002,0x4002,0x4002,0x0,0x4002,0x1c06,0x1c06,0x0,
0x4002,0x4002,0x4002,0x4002,0x4002,0x4002,
0x0,0x0,0x2c03,0x2c03,0x2403,0x2c03,0x2c03,0x2c03,0x2c03,0x9001,0x2c03,0x2c03,
0x9402,0x9403,0x9404,0x9405,0x9406,0x9407,
0x0,0x0,0x1408,0x1408,0x4400,0x1408,0x1408,0x1408,0x1408,0x1408,0x1408,0x1408,
0x1408,0x1408,0x1408,0x1408,0x1408,0x1408,
0x0,0x0,0x9008,0x9008,0x0,0x9008,0x9008,0x9008,0x9008,0x2c0a,0x9008,0x9008,
0x9008,0x9008,0x9008,0x9008,0x9008,0x9008,
0x0,0x0,0x9008,0x9008,0x0,0x9008,0x9008,0x9008,0x9008,0x9c08,0x9008,0x9008,
0x9008,0x9008,0x9008,0x9008,0x9008,0x9008,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[18],
&SXS_transition[36],
&SXS_transition[54],
&SXS_transition[72],
&SXS_transition[90],
&SXS_transition[108],
&SXS_transition[126],
&SXS_transition[144],
&SXS_transition[162],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2c09,10,0,1,0},
{0x2003,9,8,1,0},
{0x2003,9,12,1,0},
{0x2003,9,10,1,0},
{0x2003,9,13,1,0},
{0x2003,9,9,1,0},
{0x2003,9,11,1,0},
{0x2003,1,0,0,0},
};
static char *S_adrp[]={0,
"%string",
"%integer",
"End Of File",
};
static SXINT S_is_a_keyword[]={
0};
static SXINT S_is_a_generic_terminal[]={
2,1,2};
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
{sxscanner,(SXPARSER_T) sxparser}, {255, 3, 1, 3, 4, 18, 0, 10, 1, 1, 0, 
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
{0, 7, 7, 8, 8, 14, 16, 18, 3, 3, 6, 6, 7, 2, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-16,
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
/* End of sxtables for dico [Thu Nov 29 14:04:15 2012] */
