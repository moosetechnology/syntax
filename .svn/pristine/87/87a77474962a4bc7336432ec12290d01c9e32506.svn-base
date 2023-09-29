/* Beginning of sxtables for dec [Thu Jun 21 15:52:10 2007] */
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
 [2] = {16, 0X0000B85E, }
#else
 [2] = {16, 0X000000000000B85E, }
#endif
/* End SXPBA_kt1 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt1[0],
};
static int P_lrgl[]={
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
static int *P_lregle[]={NULL,
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
static int P_right_ctxt_head[]={0,
2,1,1,2,2,1,1,1,1,2,1,};
static int P_param_ref[]={
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
static struct SXP_local_mess SXP_local_mess[]={{0,NULL,0},
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
static int P_no_delete[]={
0};
static int P_no_insert[]={
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
static int PER_tset[]={
1,1};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%d warnings and %d errors are reported.";
extern SXBOOLEAN sxprecovery();

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
static int S_is_a_keyword[]={
3,2,3,4};
static int S_is_a_generic_terminal[]={
4,1,5,9,10};
static int S_lrgl[]={
4,1,2,3,4,0,
5,-1,1,2,3,4,
5,-1,0,1,2,3,
};
static int *S_lregle[]={NULL,
&S_lrgl[0],
&S_lrgl[6],
&S_lrgl[12],
};
static int S_param_ref[]={
0,
0,-1,
-1,0,
};
static struct SXS_local_mess SXS_local_mess[]={{0, NULL, 0},
{1,"%sThe invalid character \"%s\" is deleted.",&S_param_ref[0]},
{2,"%sThe invalid character \"%s\" is replaced by \"%s\".",&S_param_ref[1]},
{2,"%sThe character \"%s\" is inserted before \"%s\".",&S_param_ref[3]},
};
static int S_no_delete[]={
0};
static int S_no_insert[]={
0};
static char *S_global_mess[]={0,
"%s\"%s\" is deleted.",
"%sThis unknown keyword is erased.",
"End Of Line",
"<",
"%sScanning stops on the Axiom.",
};
extern int SCANACT();
extern int sxscan_it();
extern SXBOOLEAN sxsrecovery();
static int check_keyword();
extern int sxscanner();
extern int sxparser();
extern int SEMACT();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,sxparser}, {255, 12, 1, 3, 4, 19, 1, 21, 1, 0, 0, 
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
SCANACT,
sxsrecovery,
check_keyword,
},
{4, 20, 20, 26, 26, 35, 45, 63, 12, 11, 19, 19, 15, 7, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
vector-45,
SXPBM_trans,
NULL,
NULL,
NULL,
P_lregle,
P_right_ctxt_head,
SXP_local_mess,
P_no_delete,
P_no_insert,
P_global_mess,PER_tset,sxscan_it,sxprecovery,
NULL,
NULL,
SEMACT},
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

static int kw_code2t_code [4] = {0,
4, 3, 2, 
};

static int kw_code2lgth [4] = {0,
5, 4, 4, 
};

static unsigned int comb_vector [5] = {
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

/* End of sxtables for dec [Thu Jun 21 15:52:10 2007] */
