/* Beginning of sxtables for sdag [Mon Feb 11 18:01:25 2008] */
#define SXP_MAX 27
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{0, 3, 1, -13},
{0, 10, 1, -19},
{0, 4, 0, -13},
{0, 0, 2, -17},
{0, 1, 1, -17},
{6, 7, 0, -18},
{2, 6, 0, -18},
{0, 2, 1, -17},
{1, 12, 0, 27},
{5, 5, 3, -1},
{3, 9, 1, -14},
{4, 8, 1, -14},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-8, 21, 0},
{0, 23, 0},
{0, 9, 4},
{-3, 0, 0},
{0, -4, 6},
{0, 15, 2},
{-12, 11, 5},
{0, 16, 6},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-18, -1, 3},
{-2, 0, 0},
{-19, -14, 5},
{-13, -17, 1},
};
static struct SXP_item vector[]={
{1, 5},
{2, -6},
{0, 0},
{4, 7},
{3, 10},
{4, 9},
{0, 0},
{0, 0},
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
3,3,3,3,3,3,3,3,14,3,14,3,14,14,3,3,14,3,14,14,3,14,14,14,14,
3,14,3,14,14,3,3,14,3,14,14,3,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,3,14,14,14,14,14,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,3,14,14,14,14,14,
14,14,14,3,3,3,3,3,3,3,3,3,4,5,4,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,6,7,8,3,3,9,10,11,12,3,3,3,3,
13,14,14,14,14,14,14,14,14,14,14,3,3,3,3,3,3,3,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,16,17,3,3,3,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,18,19,20,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,14,3,14,3,14,14,3,3,14,3,14,
14,3,14,14,14,14,3,14,3,14,14,3,3,14,3,14,14,3,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,3,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
3,14,14,14,14,14,14,14,14,};
static SXSTMI SXS_transition[]={0,
0x0,0x4006,0x0,0x2402,0x2402,0x2403,0x9002,0x4401,0x2404,0x4402,0x4403,0x0,
0x2405,0x2c06,0x2407,0x0,0x0,0x9c01,0x0,0x0,
0x0,0x4000,0x0,0x1402,0x1402,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x0,
0x2405,0x4000,0x4000,0x0,0x0,0x4000,0x0,0x0,
0x0,0x0,0x1c03,0x1c03,0x0,0x4404,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x1c03,0x1c03,0x2409,0x1c03,0x1c03,0x1c03,0x1c03,
0x0,0x0,0x1c04,0x1c04,0x0,0x1c04,0x1c04,0x1c04,0x4404,0x1c04,0x1c04,0x1c04,
0x1c04,0x1c04,0x1c04,0x240a,0x1c04,0x1c04,0x1c04,0x1c04,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x240b,
0x240c,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x1c06,0x4004,0x4004,0x4004,0x4004,0x1c06,0x4004,0x4004,0x4004,0x1c06,
0x4004,0x1c06,0x1c06,0x1c06,0x1c06,0x4004,0x1c06,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x240d,0x0,
0x0,0x0,0x1408,0x1408,0x4400,0x1408,0x1408,0x1408,0x1408,0x1408,0x1408,0x1408,
0x1408,0x1408,0x1408,0x1408,0x1408,0x1408,0x1408,0x1408,
0x0,0x0,0x0,0x0,0x0,0x2c03,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x2c03,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c04,0x0,0x0,0x0,
0x0,0x0,0x0,0x2c04,0x0,0x0,0x0,0x0,
0x0,0x0,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x240f,
0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x0,0x0,0x140c,0x140c,0x2402,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,
0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,0x140c,
0x0,0x0,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x1c0d,
0x1c0d,0x1c0d,0x1c0d,0x2410,0x1c0d,0x1c0d,0x2411,0x1c0d,
0x0,0x0,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,0x1c0e,
0x1c0e,0x1c0e,0x1c0e,0x2c12,0x1c0e,0x9c04,0x1c0e,0x9006,
0x0,0x0,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x140f,
0x2402,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,0x240b,
0x0,0x0,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,0x2c0d,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x4405,0x0,0x0,0x0,
0x0,0x0,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,
0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,0x2c0e,
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
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x200e,4,0,1,0},
{0x2408,1,0,1,1},
{0x0,0,0,1,1},
{0x200e,12,0,1,0},
{0x200e,13,0,1,0},
{0x9c05,2,0,1,1},
{0x4c00,0,0,1,1},
};
static char *S_adrp[]={0,
"end_of_source",
"(",
")",
"%form",
"%SEMLEX",
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
"End Of File",
"%sScanning stops on End Of File.",
};
extern SXINT sxscan_it(void);
extern SXBOOLEAN sxsrecovery (SXINT sxsrecovery_what, SXINT state_no, unsigned char *class);
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
#ifndef SCANACT_AND_SEMACT_ARE_IDENTICAL
extern SXINT SEMACT(SXINT what, struct sxtables *arg);
#endif
struct sxtables sxtables={
52113, /* magic */
{sxscanner,(PARSER_T) sxparser}, {255, 6, 1, 3, 4, 20, 1, 18, 0, 0, 0, 
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
{2, 12, 12, 16, 16, 20, 22, 29, 6, 7, 12, 12, 8, 5, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions,
t_bases,
nt_bases,
vector-22,
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
(SEMACT_T) SEMACT},
err_titles,
abstract,
(sxsem_tables*)NULL,
NULL,
};
/* End of sxtables for sdag [Mon Feb 11 18:01:25 2008] */
