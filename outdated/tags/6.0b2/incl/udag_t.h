/* Beginning of sxtables for udag [Thu Jun 21 15:51:59 2007] */
#define SXP_MAX 15
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{0, 2, 1, -7},
{0, 4, 0, -11},
{0, 5, 0, -11},
{0, 0, 2, -10},
{0, 1, 1, -10},
{1, 6, 2, 15},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-5, 8, 2},
{0, 13, 0},
{0, 8, 2},
{0, -4, 3},
{0, 6, 2},
{0, 9, 3},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-1, 0, 0},
{-11, 0, 0},
{-7, -10, 1},
};
static struct SXP_item vector[]={
{1, 2},
{2, 3},
{0, 0},
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
3,3,3,3,3,3,3,3,3,3,4,3,3,3,3,6,3,3,7,7,3,3,3,3,3,
3,8,8,8,8,8,8,8,8,8,8,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,7,9,7,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,10,7,11,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x4003,0x2c02,0x2403,0x2403,0x2404,0x0,0x2c05,0x2406,0x9c01,0x2c02,
0x0,0x0,0x1c02,0x4001,0x4001,0x4001,0x0,0x1c02,0x2406,0x4001,0x1c02,
0x0,0x4000,0x4000,0x1403,0x1403,0x2404,0x0,0x4000,0x4000,0x9c01,0x4000,
0x0,0x0,0x1404,0x1404,0x2403,0x1404,0x1404,0x1404,0x1404,0x1404,0x1404,
0x0,0x4002,0x2c02,0x4002,0x4002,0x4002,0x0,0x1c05,0x2406,0x4002,0x2c02,
0x0,0x0,0x2c02,0x0,0x0,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,0x2c02,
0x0,0x0,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x1c07,0x9c02,0x9004,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[11],
&SXS_transition[22],
&SXS_transition[33],
&SXS_transition[44],
&SXS_transition[55],
&SXS_transition[66],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2007,4,0,1,0},
{0x2007,12,0,1,0},
{0x2007,13,0,1,0},
{0x9c03,2,0,1,1},
{0x2c03,0,0,1,1},
};
static char *S_adrp[]={0,
"%form",
"%state",
"End Of File",
};
static int S_is_a_keyword[]={
0};
static int S_is_a_generic_terminal[]={
2,1,2};
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
extern int sxscanner();
extern int sxparser();
extern int SEMACT();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,sxparser}, {255, 3, 1, 3, 4, 11, 1, 7, 0, 0, 0, 
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
{1, 6, 6, 9, 9, 12, 14, 16, 3, 4, 6, 6, 6, 4, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions,
t_bases,
nt_bases,
vector-14,
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
SEMACT},
err_titles,
abstract,
(sxsem_tables*)NULL,
NULL,
};
/* End of sxtables for udag [Thu Jun 21 15:51:59 2007] */
