/* Beginning of sxtables for re [Thu Mar  6 12:56:07 2008] */
#define SXP_MAX 43
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{2, 8, 1, -17},
{6, 9, 1, -17},
{2, 4, 2, -25},
{6, 5, 2, -25},
{5, 15, 0, -27},
{0, 0, 2, -23},
{1, 1, 1, -23},
{0, 2, 1, 31},
{0, 3, 1, 31},
{0, 6, 1, 33},
{0, 7, 1, 33},
{1, 11, 1, 35},
{1, 12, 1, 35},
{1, 13, 1, 35},
{0, 10, 1, 35},
{0, 14, 2, -27},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-10, 38, 0},
{-11, 38, 0},
{0, 38, 0},
{0, 38, 0},
{0, 38, 0},
{0, 38, 0},
{0, -6, 9},
{0, 7, 1},
{-8, 21, 2},
{-9, 22, 2},
{-15, 37, 0},
{0, 16, 7},
{0, 19, 9},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-1, -27, 7},
{-2, -27, 7},
{-27, 30, 36},
{-28, 30, 40},
{-3, 43, 0},
{-4, 43, 0},
};
static struct SXP_item vector[]={
{3, -25},
{4, -26},
{5, -17},
{6, -18},
{1, -23},
{2, -24},
{0, 0},
{3, 12},
{4, 13},
{5, 14},
{0, 0},
{6, 20},
{0, 0},
{8, 5},
{7, -27},
{5, -17},
{6, -18},
{7, -27},
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
static char abstract []= "%d warnings and %d errors are reported.";
extern SXBOOLEAN sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no);

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,4,5,4,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,3,3,3,6,3,3,7,8,9,10,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,11,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,12,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,13,14,15,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x4009,0x2c02,0x2403,0x4401,0x2404,0x4406,0x4407,0x4403,0x4404,0x4405,0x2405,
0x9c01,0x4402,0x0,
0x0,0x0,0x1c02,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x2406,
0x4008,0x4008,0x0,
0x0,0x4000,0x4000,0x1403,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x9c01,0x4000,0x0,
0x0,0x0,0x1404,0x1404,0x4401,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x1404,0x1404,0x1404,
0x0,0x0,0x2c02,0x0,0x4400,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,
0x2c02,0x2c02,0x2c02,
0x0,0x0,0x2c02,0x0,0x0,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,
0x2c02,0x2c02,0x2c02,
0x0,0x0,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x2c08,
0x9c02,0x1c07,0x9004,
0x0,0x0,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,0x2c07,
0x2c07,0x2c07,0x2c07,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[15],
&SXS_transition[30],
&SXS_transition[45],
&SXS_transition[60],
&SXS_transition[75],
&SXS_transition[90],
&SXS_transition[105],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2007,4,0,1,0},
{0x2007,12,0,1,0},
{0x2007,13,0,1,0},
{0x9c03,2,0,1,1},
{0x2c03,0,0,1,1},
};
static char *S_adrp[]={0,
"%eof",
"|",
"*",
"+",
"?",
"(",
")",
"%word",
"End Of File",
};
static SXINT S_is_a_keyword[]={
0};
static SXINT S_is_a_generic_terminal[]={
2,1,8};
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
static struct SXT_node_info SXT_node_info[]={{0,0},
{6,1},{0,3},{0,4},{0,5},{2,7},{0,9},{0,10},{0,11},{7,13},{0,15},{4,16},{3,17},
{5,18},{0,19},{8,20},{0,21},};
static SXINT T_ter_to_node_name[]={
0,9,};
static char *T_node_name[]={NULL,
"ERROR",
"ALTERNATIVE_S",
"KLEENE_PLUS",
"KLEENE_STAR",
"OPTION",
"RE_ROOT",
"SEQUENCE_S",
"WORD",
"eof",
};
extern SXINT sempass(SXINT what, struct sxtables *sxtables_ptr);
static char T_stack_schema[]={0,
0,1,0,0,0,2,0,2,0,0,0,1,0,1,0,1,1,1,1,0,0,};

static struct SXT_tables SXT_tables=
{SXT_node_info, T_ter_to_node_name, T_stack_schema, sempass, T_node_name};
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxatc(SXINT what, ...);

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(PARSER_T) sxparser}, {255, 9, 1, 3, 4, 15, 1, 8, 0, 0, 0, 
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
(RECOVERY_T) sxsrecovery,
NULL,
},
{4, 16, 16, 19, 22, 29, 33, 50, 9, 7, 15, 15, 13, 7, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
vector-33,
NULL,
NULL,
NULL,
NULL,
P_lregle,
P_right_ctxt_head,
SXP_local_mess,
P_no_delete,
P_no_insert,
P_global_mess,PER_tset,sxscan_it,(RECOVERY_T) sxprecovery,
NULL,
NULL,
(SEMACT_T) sxatc},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables,
NULL,
};
/* End of sxtables for re [Thu Mar  6 12:56:07 2008] */
