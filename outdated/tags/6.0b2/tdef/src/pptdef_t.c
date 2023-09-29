/* Beginning of sxtables for pptdef [Thu Jun 21 15:52:11 2007] */
#define SXP_MAX 19
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{1, 2, 1, -9},
{0, 0, 2, -12},
{1, 1, 1, -12},
{1, 6, 0, -13},
{1, 8, 0, -13},
{1, 7, 0, -13},
{1, 5, 3, 19},
{1, 4, 3, 19},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-3, 14, 1},
{0, 17, 0},
{0, 14, 1},
{0, -2, 6},
{0, 8, 3},
{0, 10, 2},
{0, 7, 3},
{0, 11, 6},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-1, 0, 0},
{-13, 0, 0},
{-9, -12, 1},
};
static struct SXP_item vector[]={
{1, 4},
{2, 5},
{0, 0},
{4, 15},
{5, 6},
{0, 0},
};

static  SXBA_ELT SXPBA_kt3
#if SXBITS_PER_LONG==32
 [2] = {9, 0X000001EE, }
#else
 [2] = {9, 0X00000000000001EE, }
#endif
/* End SXPBA_kt3 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt3[0],
};
static int P_lrgl[]={
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
};
static int P_right_ctxt_head[]={0,
1,2,2,1,1,1,1,2,1,};
static int P_param_ref[]={
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
"End Of File",
"%sParsing stops on End Of File.",
};
static int PER_tset[]={
1,3};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%d warnings and %d errors are reported.";
extern SXBOOLEAN sxprecovery();

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,3,3,3,3,3,3,3,3,3,4,5,4,4,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,6,7,8,9,7,7,10,7,7,7,11,7,7,7,7,
7,12,12,12,12,12,12,12,12,12,12,7,13,14,15,16,7,10,17,17,17,17,17,17,17,
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,7,18,7,7,19,7,
17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
17,7,7,7,10,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,};
static SXSTMI SXS_transition[]={0,
0x0,0x4006,0x0,0x2402,0x2402,0x2402,0x2c03,0x2c04,0x2c05,0x0,0x9001,0x2c03,
0x4403,0x2c06,0x2c07,0x2c03,0x2c08,0x2c03,0x2c03,
0x0,0x4000,0x0,0x1402,0x1402,0x1402,0x4000,0x4000,0x4000,0x0,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x0,0x0,0x0,0x4005,0x4005,0x4005,0x1c03,0x1c03,0x1c03,0x1c03,0x9003,0x1c03,
0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x0,0x0,0x2c0a,0x2c0a,0x0,0x2c0a,0x2c0a,0x0,0x2c0a,0x2c0a,0x2c0a,0x2c0a,
0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0b,0x2c0a,
0x0,0x0,0x0,0x0,0x0,0x0,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x0,0x0,0x0,0x0,0x0,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,
0x1c06,0x1c06,0x1c06,0x4c04,0x1c06,0x1c06,0x1c06,
0x0,0x0,0x0,0x4002,0x4002,0x4002,0x2c03,0x2c03,0x2c03,0x2c03,0x9005,0x2c03,
0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x0,0x0,0x0,0x4001,0x4001,0x4001,0x2c03,0x2c03,0x2c03,0x2c03,0x9007,0x1c08,
0x2c03,0x2c03,0x2c03,0x2c03,0x1c08,0x2c03,0x2c0c,
0x0,0x0,0x1c09,0x1c09,0x4c00,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,
0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,0x1c09,
0x0,0x0,0x1c0a,0x1c0a,0x0,0x1c0a,0x1c0a,0x4c05,0x1c0a,0x1c0a,0x1c0a,0x1c0a,
0x1c0a,0x1c0a,0x1c0a,0x1c0a,0x1c0a,0x2c0b,0x1c0a,
0x0,0x0,0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0a,
0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0a,0x2c0a,
0x0,0x0,0x0,0x4005,0x4005,0x4005,0x2c03,0x2c03,0x2c03,0x2c03,0x9003,0x2c08,
0x2c03,0x2c03,0x2c03,0x2c03,0x2c08,0x2c03,0x2c03,
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
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2c09,1,0,1,1},
{0x2c03,0,0,1,1},
{0x4005,1,0,1,1},
{0x2c03,0,0,1,1},
{0x4002,1,0,1,1},
{0x2c03,0,0,1,1},
{0x4001,1,0,1,1},
{0x2c03,0,0,1,1},
};
static char *S_adrp[]={0,
"%T_NAME",
"=",
";",
"%NON_TERMINAL",
"%T_SYMB",
"End Of File",
};
static int S_is_a_keyword[]={
0};
static int S_is_a_generic_terminal[]={
3,1,4,5};
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
"End Of File",
"%sScanning stops on End Of File.",
};
extern int sxscan_it();
extern SXBOOLEAN sxsrecovery();
static struct SXPP_schema SXPP_schema[]={
{6,0},{1,1},{8,0},{6,0},{1,1},{5,0},{6,0},{8,0},{6,0},{8,0},{6,0},{4,1},
{6,2},{3,1},{6,0},{4,1},{6,3},{8,0},{6,2},{8,0},};

static struct SXPP_schema *PP_tables[]={NULL,
&SXPP_schema[0],
&SXPP_schema[3],
&SXPP_schema[8],
&SXPP_schema[10],
&SXPP_schema[10],
&SXPP_schema[8],
&SXPP_schema[8],
&SXPP_schema[18],
};
extern int sxscanner();
extern int sxparser();
extern int sxatcpp();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,sxparser}, {255, 6, 1, 3, 4, 19, 0, 12, 0, 0, 0, 
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
NULL,
sxsrecovery,
NULL,
},
{1, 8, 8, 11, 11, 16, 18, 23, 6, 4, 8, 8, 8, 4, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions,
t_bases,
nt_bases,
vector-18,
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
sxatcpp},
err_titles,
abstract,
(sxsem_tables*)PP_tables,
NULL,
};
/* End of sxtables for pptdef [Thu Jun 21 15:52:11 2007] */
