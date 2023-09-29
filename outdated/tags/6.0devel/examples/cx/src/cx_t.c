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


/* Beginning of sxtables for cx [Mon Jul 21 14:35:43 2008] */
#define SXP_MAX 12
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{0, 2, 1, -6},
{1, 4, 0, 12},
{0, 5, 0, 12},
{0, 0, 2, -8},
{0, 1, 1, -8},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-5, 10, 0},
{0, 10, 0},
{0, -4, 3},
{0, 7, 3},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-1, 0, 0},
{-6, -8, 1},
};
static struct SXP_item vector[]={
{1, 2},
{2, 3},
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
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,5,3,6,3,3,7,3,3,8,9,3,9,10,
11,12,13,13,13,13,13,13,13,14,14,3,3,3,3,3,3,3,15,15,15,15,16,15,17,
17,17,17,17,18,17,17,17,17,17,17,17,17,17,17,17,19,17,17,3,20,3,3,21,3,
22,22,22,22,23,22,24,24,24,24,24,25,24,24,24,24,24,24,24,24,24,24,24,26,24,
24,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x4003,0x2402,0x2402,0x2403,0x2402,0x2404,0x2402,0x2402,0x2405,0x2406,0x2407,
0x2408,0x2408,0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,0x2402,0x2402,0x2c0a,0x2c0a,0x2c0a,
0x2c0a,0x2c0a,
0x0,0x4000,0x1402,0x1402,0x4000,0x1402,0x4000,0x1402,0x1402,0x1402,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x1402,0x1402,0x4000,0x4000,0x4000,
0x4000,0x4000,
0x0,0x0,0x1403,0x0,0x4400,0x1403,0x1403,0x1403,0x1403,0x1403,0x1403,0x1403,
0x1403,0x1403,0x1403,0x1403,0x1403,0x1403,0x1403,0x240b,0x1403,0x1403,0x1403,0x1403,
0x1403,0x1403,
0x0,0x0,0x240c,0x0,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,
0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x240d,0x240c,0x240c,0x240c,0x240c,
0x240c,0x240c,
0x0,0x4000,0x2402,0x2402,0x4000,0x2402,0x4000,0x2402,0x2402,0x2402,0x4000,0x240e,
0x240e,0x240e,0x4000,0x4000,0x4000,0x4000,0x4000,0x2402,0x2402,0x4000,0x4000,0x4000,
0x4000,0x4000,
0x0,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x240f,0x4000,0x4000,0x2410,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,
0x0,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x240e,0x4000,0x2408,
0x2408,0x2408,0x4000,0x2411,0x4000,0x4400,0x2412,0x4000,0x4000,0x4000,0x2411,0x4000,
0x4400,0x2412,
0x0,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x240e,0x4000,0x1408,
0x1408,0x1408,0x4000,0x2411,0x4000,0x4400,0x4000,0x4000,0x4000,0x4000,0x2411,0x4000,
0x4400,0x4000,
0x0,0x4001,0x4001,0x4001,0x4001,0x2c13,0x4001,0x4001,0x4001,0x4001,0x4001,0x1c09,
0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x4001,0x1c09,0x1c09,0x1c09,0x1c09,
0x1c09,0x1c09,
0x0,0x3001,0x3001,0x3001,0x3001,0x2c13,0x3001,0x3001,0x3001,0x3001,0x3001,0x2c09,
0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,0x2c09,0x3001,0x2c09,0x1c0a,0x1c0a,0x1c0a,
0x1c0a,0x1c0a,
0x0,0x0,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,
0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,0x2403,
0x2403,0x2403,
0x0,0x0,0x0,0x0,0x0,0x0,0x4400,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,
0x0,0x0,0x240c,0x0,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x2414,
0x2414,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,0x240c,
0x240c,0x240c,
0x0,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x140e,
0x140e,0x140e,0x4000,0x2411,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x2411,0x4000,
0x4000,0x4000,
0x0,0x0,0x140f,0x140f,0x140f,0x140f,0x140f,0x2415,0x140f,0x140f,0x140f,0x140f,
0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,0x140f,
0x140f,0x140f,
0x0,0x0,0x1410,0x4400,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,
0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,
0x1410,0x1410,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2416,0x0,0x0,0x2417,
0x2417,0x2417,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2418,
0x2418,0x2418,0x2418,0x2418,0x0,0x0,0x0,0x0,0x0,0x2418,0x2418,0x0,
0x0,0x0,
0x0,0x4001,0x4001,0x4001,0x4001,0x4001,0x4001,0x4001,0x4001,0x4001,0x4001,0x1c13,
0x1c13,0x1c13,0x1c13,0x1c13,0x1c13,0x1c13,0x1c13,0x4001,0x1c13,0x1c13,0x1c13,0x1c13,
0x1c13,0x1c13,
0x0,0x0,0x0,0x0,0x0,0x0,0x4400,0x0,0x0,0x0,0x0,0x2419,
0x2419,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,
0x0,0x0,0x240f,0x240f,0x240f,0x240f,0x240f,0x1415,0x240f,0x240f,0x4400,0x240f,
0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,0x240f,
0x240f,0x240f,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2417,
0x2417,0x2417,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,
0x0,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x1417,
0x1417,0x1417,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,
0x0,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x1418,
0x1418,0x1418,0x1418,0x1418,0x4000,0x4400,0x4000,0x4000,0x4000,0x1418,0x1418,0x4000,
0x4400,0x4000,
0x0,0x0,0x0,0x0,0x0,0x0,0x4400,0x0,0x0,0x0,0x0,0x240c,
0x240c,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[26],
&SXS_transition[52],
&SXS_transition[78],
&SXS_transition[104],
&SXS_transition[130],
&SXS_transition[156],
&SXS_transition[182],
&SXS_transition[208],
&SXS_transition[234],
&SXS_transition[260],
&SXS_transition[286],
&SXS_transition[312],
&SXS_transition[338],
&SXS_transition[364],
&SXS_transition[390],
&SXS_transition[416],
&SXS_transition[442],
&SXS_transition[468],
&SXS_transition[494],
&SXS_transition[520],
&SXS_transition[546],
&SXS_transition[572],
&SXS_transition[598],
&SXS_transition[624],
};
static char *S_adrp[]={0,
"%ID",
"auto",
"End Of File",
"break",
"case",
"char",
"continue",
"default",
"do",
"double",
"else",
"enum",
"extern",
"float",
"for",
"goto",
"if",
"int",
"long",
"register",
"return",
"short",
"sizeof",
"static",
"struct",
"switch",
"typedef",
"union",
"unsigned",
"while",
};
static SXINT S_syno[]={0,
0,30,0,0,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
25,26,27,28,29,};
static SXINT S_is_a_keyword[]={
1,2};
static SXINT S_is_a_generic_terminal[]={
1,1};
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
{sxscanner,(SXPARSER_T) sxparser}, {255, 3, 1, 3, 4, 26, 0, 25, 0, 1, 0, 
S_is_a_keyword,S_is_a_generic_terminal,S_transition_matrix,
NULL,
S_adrp,
S_syno,
SXS_local_mess,
S_char_to_simple_class+128,
S_no_delete,
S_no_insert,
S_global_mess,
S_lregle,
NULL,
(SXRECOVERY_T) sxsrecovery,
check_keyword,
},
{1, 5, 5, 7, 7, 9, 11, 13, 3, 3, 5, 5, 4, 3, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-11,
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

#include	"sxdico.h"

#define KW_NB		28
#define INIT_BASE	1
#define INIT_SUFFIX	0
#define CLASS_MASK	31
#define STOP_MASK	32
#define BASE_SHIFT	7
#define SUFFIX_MASK	64

#define SYNO
#define EOF_CODE	3
static SXINT syno2t_code [27] = {
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
};

static SXINT kw_code2t_code [29] = {0,
2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 
18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 
};

static SXINT kw_code2lgth [29] = {0,
4, 5, 4, 4, 8, 7, 2, 6, 4, 4, 6, 5, 3, 4, 2, 
3, 4, 8, 6, 5, 6, 6, 6, 6, 7, 5, 8, 5, 
};

static SXUINT comb_vector [53] = {
0, 0, 161, 290, 3843, 0, 0, 2630, 1543, 
3240, 2217, 0, 2849, 4364, 1837, 3598, 3535, 2064, 3633, 
3410, 2596, 1193, 1322, 1451, 1416, 933, 2991, 1063, 808, 
0, 2734, 417, 3494, 3121, 548, 677, 3370, 2474, 2090, 
1701, 1964, 0, 2351, 1577, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 
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
'\000', '\000', '\001', '\002', '\003', '\006', '\007', '\f', 
'\r', '\004', '\016', '\000', '\000', '\t', '\000', '\n', 
'\005', '\000', '\000', '\017', '\020', '\b', '\022', '\000', 
'\021', '\013', '\000', '\000', '\000', '\000', '\000', '\000', 
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

/* End of sxtables for cx [Mon Jul 21 14:35:43 2008] */
