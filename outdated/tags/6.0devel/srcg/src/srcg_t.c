/* Beginning of sxtables for srcg [Wed Jun 18 17:41:35 2008] */
#define SXP_MAX 116
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{6, 3, 0, -44},
{1, 5, 1, -67},
{1, 6, 1, -67},
{2, 2, 1, -44},
{6, 11, 0, 87},
{6, 21, 0, -46},
{2, 13, 1, -45},
{1, 23, 1, 93},
{1, 24, 1, 93},
{2, 20, 1, -46},
{2, 10, 2, 87},
{6, 42, 0, -47},
{6, 40, 0, -72},
{2, 19, 2, -46},
{2, 43, 1, -47},
{2, 39, 2, -72},
{1, 8, 6, 86},
{1, 36, 6, 95},
{0, 0, 2, -66},
{0, 1, 1, -66},
{6, 14, 0, -45},
{1, 26, 0, -70},
{1, 27, 0, -70},
{1, 28, 0, -70},
{1, 29, 0, -70},
{1, 30, 0, -70},
{1, 31, 0, -70},
{1, 32, 0, -70},
{1, 33, 0, -70},
{1, 34, 0, -70},
{6, 17, 0, -69},
{5, 15, 0, 91},
{5, 16, 0, 91},
{0, 12, 1, 89},
{1, 4, 3, 83},
{0, 18, 1, -69},
{6, 37, 0, 97},
{0, 38, 1, 97},
{0, 41, 1, 99},
{1, 25, 3, 93},
{1, 9, 0, 88},
{1, 7, 8, 86},
{1, 35, 8, 95},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-20, 98, 0},
{-34, 101, 0},
{-36, 110, 104},
{-39, 101, 0},
{0, 49, 5},
{-21, 0, 0},
{-31, 110, 0},
{0, 52, 5},
{-37, 101, 0},
{0, 110, 0},
{0, 101, 0},
{-41, 64, 5},
{-41, 65, 5},
{0, 98, 0},
{0, 49, 5},
{0, 52, 5},
{-21, 0, 0},
{-37, 101, 0},
{-21, 0, 0},
{-37, 101, 0},
{-21, 0, 0},
{-37, 101, 0},
{0, -19, 19},
{0, 50, 1},
{0, 105, 102},
{0, 35, 2},
{0, 61, 5},
{0, 79, 6},
{-38, 54, 7},
{0, 40, 6},
{0, 105, 100},
{0, 56, 6},
{0, 105, 99},
{0, 43, 6},
{0, 62, 5},
{0, 63, 5},
{0, 57, 19},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-4, -67, 4},
{-7, 0, 0},
{-10, -70, 16},
{-15, 0, 0},
{-2, 0, 0},
{-45, 88, 86},
{-70, 91, 0},
{-8, 0, 0},
{-12, 111, 81},
{-14, -70, 16},
{-12, 96, 0},
{-17, 0, 0},
{-18, 0, 0},
{-67, 116, 0},
{-3, 0, 0},
{-9, 0, 0},
{-11, -45, 10},
{-12, 111, 90},
{-74, 88, 85},
{-12, 111, 83},
{-76, 88, 85},
{-12, 111, 82},
};
static struct SXP_item vector[]={
{0, 0},
{7, -68},
{0, 0},
{10, -45},
{5, -69},
{9, -5},
{17, -71},
{17, -77},
{17, -75},
{3, 48},
{4, 58},
{12, -46},
{13, -6},
{6, 42},
{6, 55},
{17, -73},
{6, 78},
{8, 32},
{9, 33},
{7, 53},
{7, 60},
{3, 51},
{4, 59},
{19, -16},
{20, -47},
{1, -66},
{2, -44},
{3, -1},
{10, 22},
{11, 23},
{12, 24},
{13, 25},
{14, 26},
{15, 27},
{16, 28},
{17, 29},
{18, 30},
{18, -72},
{19, -13},
{20, -47},
{0, 0},
};

static  SXBA_ELT SXPBA_kt2
#if SXBITS_PER_LONG==32
 [3] = {38, 0X05004082, 0X00000020, }
#else
 [2] = {38, 0X0000002005004082, }
#endif
/* End SXPBA_kt2 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt2[0],
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
1,2};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%d warnings and %d errors are reported.";
extern SXBOOLEAN sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no);

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,23,3,23,3,3,3,3,23,23,23,23,
23,3,3,23,23,3,3,3,3,23,3,3,3,3,23,3,23,23,3,3,3,24,3,24,3,
3,3,3,24,24,24,24,24,3,3,24,24,3,3,3,3,24,3,3,3,3,24,3,24,24,
3,3,3,3,3,3,3,3,3,3,3,3,4,5,4,4,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,6,3,3,7,8,3,9,10,11,3,12,13,14,
15,16,16,16,16,16,16,16,16,16,16,3,14,3,17,18,3,3,19,19,19,19,19,19,19,
19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,3,20,3,3,21,3,
22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,
22,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,23,3,23,3,3,
3,3,23,23,23,23,23,3,3,23,23,3,3,3,3,23,3,3,3,3,23,3,23,23,3,
3,3,24,3,24,3,3,3,3,24,24,24,24,24,3,3,24,24,3,3,3,3,24,3,3,
3,3,24,3,24,24,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x4013,0x0,0x2402,0x2402,0x2c03,0x2404,0x2405,0x9402,0x9403,0x0,0x4407,
0x2c06,0x4402,0x2407,0x9005,0x9c04,0x0,0x9007,0x2408,0x0,0x9009,0x900b,0x900d,

0x0,0x4000,0x0,0x1402,0x1402,0x4000,0x2404,0x4000,0x4000,0x4000,0x0,0x4000,
0x4000,0x4000,0x2407,0x4000,0x4000,0x0,0x4000,0x4000,0x0,0x4000,0x4000,0x4000,

0x0,0x0,0x1c03,0x1c03,0x1c03,0x4409,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x240d,0x1c03,0x1c03,0x1c03,0x1c03,

0x0,0x0,0x1404,0x1404,0x2402,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x900f,0x0,0x0,0x900f,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x2c0f,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2410,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,

0x0,0x0,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,
0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,0x4c09,

0x0,0x0,0x0,0x4003,0x4003,0x0,0x4003,0x0,0x4003,0x0,0x0,0x0,
0x0,0x0,0x4003,0x1c09,0x0,0x0,0x1c09,0x0,0x1c09,0x1c09,0x0,0x0,

0x0,0x0,0x0,0x4008,0x4008,0x4008,0x4008,0x0,0x0,0x4008,0x0,0x4008,
0x0,0x0,0x4008,0x1c0a,0x0,0x0,0x1c0a,0x4008,0x1c0a,0x1c0a,0x1c0a,0x1c0a,

0x0,0x0,0x0,0x4009,0x4009,0x4009,0x4009,0x0,0x0,0x4009,0x0,0x4009,
0x0,0x0,0x4009,0x1c0b,0x0,0x0,0x1c0b,0x4009,0x1c0b,0x1c0b,0x1c0b,0x1c0b,

0x0,0x0,0x0,0x4009,0x4009,0x4009,0x4009,0x0,0x0,0x4009,0x0,0x4009,
0x0,0x0,0x4009,0x1c0c,0x0,0x0,0x4009,0x4009,0x0,0x4009,0x4009,0x4009,

0x0,0x0,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,

0x0,0x0,0x0,0x9001,0x9001,0x0,0x9001,0x0,0x9001,0x0,0x0,0x0,
0x0,0x0,0x9001,0x1c0e,0x0,0x0,0x1c0e,0x0,0x1c0e,0x1c0e,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x9c04,0x0,0x0,0x0,0x0,0x0,0x0,

0x0,0x0,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x2411,0x1410,
0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,0x1410,

0x0,0x0,0x2410,0x2410,0x2410,0x2410,0x2410,0x2410,0x2410,0x2410,0x1411,0x2410,
0x2410,0x2410,0x2402,0x2410,0x2410,0x2410,0x2410,0x2410,0x2410,0x2410,0x2410,0x2410,

};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[24],
&SXS_transition[48],
&SXS_transition[72],
&SXS_transition[96],
&SXS_transition[120],
&SXS_transition[144],
&SXS_transition[168],
&SXS_transition[192],
&SXS_transition[216],
&SXS_transition[240],
&SXS_transition[264],
&SXS_transition[288],
&SXS_transition[312],
&SXS_transition[336],
&SXS_transition[360],
&SXS_transition[384],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x5004,1,0,0,0},
{0x4005,3,2,1,0},
{0x4006,4,2,1,0},
{0x4001,5,0,1,0},
{0x2c0c,2,2,1,1},
{0x0,0,0,1,1},
{0x2c0a,2,2,1,1},
{0x2c09,0,2,1,1},
{0x2c0b,2,2,1,1},
{0x2c09,0,2,1,1},
{0x2c0a,2,2,1,1},
{0x0,0,0,1,1},
{0x2c0b,2,2,1,1},
{0x0,0,0,1,1},
{0x2c0e,3,2,1,1},
{0x0,0,0,1,1},
};
static char *S_adrp[]={0,
"-->",
".",
"%internal_predicate",
"%external_predicate",
"(",
")",
",",
"%variable",
"%terminal",
"streq",
"streqlen",
"strlen",
"first",
"last",
"false",
"cut",
"true",
"lex",
"End Of File",
};
static SXINT S_is_a_keyword[]={
9,10,11,12,13,14,15,16,17,18};
static SXINT S_is_a_generic_terminal[]={
4,3,4,8,9};
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
static SXINT check_keyword(char *init_string, SXINT init_length);
static struct SXT_node_info SXT_node_info[]={{0,0},
{0,1},{0,2},{4,4},{3,5},{5,7},{5,9},{11,11},{11,14},{8,17},{0,17},{2,19},{0,20},
{0,21},{7,23},{13,23},{12,24},{6,25},{0,25},{0,26},{0,28},{6,30},{0,31},{5,32},{5,34},
{5,36},{8,38},{8,38},{8,38},{8,38},{8,38},{8,38},{8,38},{8,38},{8,38},{11,38},{11,41},
{2,44},{0,44},{0,45},{2,47},{0,48},{7,49},{0,50},{0,52},{0,53},};
static SXINT T_ter_to_node_name[]={
0,0,0,10,9,};
static char *T_node_name[]={NULL,
"ERROR",
"ARGUMENT_S",
"CLAUSE",
"CLAUSE_S",
"PREDICATE",
"PREDICATE_S",
"STRING_S",
"VOID",
"external_predicate",
"internal_predicate",
"profile",
"terminal",
"variable",
};
extern SXINT sempass(SXINT what, struct sxtables *sxtables_ptr);
static char T_stack_schema[]={0,
0,0,1,0,1,3,0,1,0,1,1,4,7,0,2,5,0,2,0,0,0,1,0,0,0,
0,2,0,1,0,0,0,1,0,1,1,3,1,4,7,0,2,5,0,0,2,0,0,0,0,
1,0,0,};

static struct SXT_tables SXT_tables=
{SXT_node_info, T_ter_to_node_name, T_stack_schema, sempass, T_node_name};
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxatc(SXINT what, ...);

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(SXPARSER_T) sxparser}, {255, 19, 1, 3, 4, 24, 3, 17, 1, 1, 0, 
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
{18, 43, 43, 57, 65, 80, 92, 132, 19, 21, 44, 44, 37, 23, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions,
t_bases,
nt_bases,
#ifdef __INTEL_COMPILER
#pragma warning(push ; disable:170)
#endif
vector-92,
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
(SXSEMACT_T) sxatc
},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables,
NULL,
};

#include	"sxdico.h"

#define KW_NB		9
#define INIT_BASE	1
#define INIT_SUFFIX	0
#define CLASS_MASK	15
#define STOP_MASK	16
#define BASE_SHIFT	6
#define SUFFIX_MASK	32

static SXINT kw_code2t_code [10] = {0,
18, 17, 16, 15, 14, 13, 12, 11, 10, 
};

static SXINT kw_code2lgth [10] = {0,
3, 4, 3, 5, 4, 5, 6, 8, 5, 
};

static SXUINT comb_vector [27] = {
0, 0, 209, 962, 0, 0, 709, 0, 359, 
0, 153, 469, 534, 776, 339, 0, 603, 86, 275, 
404, 394, 649, 0, 0, 0, 0, 0, 
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
'\000', '\000', '\003', '\000', '\001', '\000', '\006', '\002', 
'\000', '\000', '\004', '\000', '\000', '\005', '\000', '\b', 
'\000', '\000', '\013', '\n', '\007', '\t', '\000', '\000', 
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

/* End of sxtables for srcg [Wed Jun 18 17:41:35 2008] */
