/* Beginning of sxtables for dag [Thu Jun 21 15:51:59 2007] */
#define SXP_MAX 30
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{1, 1, 0, -17},
{6, 6, 0, 25},
{2, 5, 1, 25},
{5, 9, 0, 26},
{0, 0, 2, -17},
{1, 2, 1, -1},
{6, 4, 1, 24},
{1, 7, 2, 26},
{1, 8, 2, 26},
{2, 3, 3, 24},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-7, 29, 0},
{0, 29, 0},
{-10, 29, 0},
{0, 29, 0},
{0, 29, 0},
{0, 29, 0},
{0, -5, 7},
{-6, 16, 1},
{0, 25, 26},
{0, 25, 27},
{0, 12, 7},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-3, 0, 0},
{-2, 30, 22},
{-3, 0, 0},
{-19, 30, 23},
{-20, 30, 23},
{-13, -2, 5},
};
static struct SXP_item vector[]={
{1, -17},
{2, -1},
{3, -18},
{1, 16},
{0, 0},
{5, -2},
{3, 8},
{0, 0},
{2, 14},
{5, 9},
{4, 15},
{4, -11},
{6, 4},
{0, 0},
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
"End Of File",
"%sParsing stops on End Of File.",
};
static int PER_tset[]={
0};

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
3,3,3,3,3,3,3,3,3,3,3,3,4,5,4,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,3,3,3,6,3,3,7,8,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,9,10,11,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,12,13,14,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x4007,0x2c02,0x2403,0x2403,0x2404,0x4402,0x4403,0x4404,0x2405,0x4405,0x9c01,
0x4401,0x2c02,
0x0,0x4006,0x1c02,0x4006,0x4006,0x4006,0x4006,0x4006,0x4006,0x2405,0x4006,0x4006,
0x4006,0x1c02,
0x0,0x4000,0x4000,0x1403,0x1403,0x2404,0x4000,0x4000,0x4000,0x4000,0x4000,0x9c01,
0x4000,0x4000,
0x0,0x0,0x1404,0x1404,0x2403,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x1404,0x1404,
0x0,0x0,0x2c02,0x0,0x0,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,
0x2c02,0x2c02,
0x0,0x0,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x9c02,
0x1c06,0x9004,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[14],
&SXS_transition[28],
&SXS_transition[42],
&SXS_transition[56],
&SXS_transition[70],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2006,4,0,1,0},
{0x2006,12,0,1,0},
{0x2006,13,0,1,0},
{0x9c03,2,0,1,1},
{0x2c03,0,0,1,1},
};
static char *S_adrp[]={0,
"|",
"(",
")",
"[",
"]",
"%word",
"End Of File",
};
static int S_is_a_keyword[]={
0};
static int S_is_a_generic_terminal[]={
1,6};
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
static struct SXT_node_info SXT_node_info[]={{0,0},
{5,1},{7,2},{0,3},{2,5},{0,6},{6,8},{3,9},{4,10},{8,11},{0,12},};
static int T_ter_to_node_name[]={
0,};
static char *T_node_name[]={NULL,
"ERROR",
"ALTERNATIVE_S",
"EXPRESSION",
"OPTION",
"PROGRAM_ROOT_DAG",
"SEQUENCE_S",
"SIMPLE_REGULAR_EXPRESSION",
"WORD",
};
extern int sempass();
static char T_stack_schema[]={0,
0,0,0,2,0,0,1,0,1,1,0,0,};

static struct SXT_tables SXT_tables=
{SXT_node_info, T_ter_to_node_name, T_stack_schema, sempass, T_node_name};
extern int sxscanner();
extern int sxparser();
extern int sxatc();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,sxparser}, {255, 7, 1, 3, 4, 14, 1, 6, 0, 0, 0, 
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
{3, 10, 10, 12, 16, 21, 23, 36, 7, 5, 9, 9, 11, 7, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
vector-23,
NULL,
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
sxatc},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables,
NULL,
};
/* End of sxtables for dag [Thu Jun 21 15:51:59 2007] */
