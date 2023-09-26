/* Beginning of sxtables for make_proper [Thu Jun 21 15:51:59 2007] */
#define SXP_MAX 11
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{0, 2, 1, -5},
{1, 4, 0, 11},
{0, 0, 2, -7},
{0, 1, 1, -7},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-4, 2, 1},
{0, 2, 1},
{0, -3, 2},
{0, 6, 2},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-1, 0, 0},
{-5, -7, 1},
};
static struct SXP_item vector[]={
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
3,3,3,3,3,3,3,3,3,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,3,3,3,3,3,3,3,3,3,4,5,4,4,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,6,7,6,6,6,6,6,6,6,6,6,4,6,6,
6,8,8,8,8,8,8,8,8,6,6,6,4,9,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,10,4,6,6,6,
6,11,6,6,6,12,6,6,6,6,6,6,6,13,6,6,6,14,6,15,6,16,6,6,6,
6,4,6,4,6,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,};
static SXSTMI SXS_transition[]={0,
0x0,0x4002,0x0,0x2402,0x2402,0x2c03,0x2404,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x2c03,0x2c03,0x2c03,0x2c03,
0x0,0x4000,0x0,0x1402,0x1402,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,
0x0,0x0,0x0,0x4001,0x4001,0x1c03,0x0,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x1c03,0x1c03,0x1c03,
0x0,0x0,0x2c05,0x2c05,0x0,0x2c05,0x0,0x2c05,0x2c05,0x2406,0x2c05,0x2c05,
0x2c05,0x2c05,0x2c05,0x2c05,
0x0,0x0,0x1c05,0x1c05,0x0,0x1c05,0x4401,0x1c05,0x1c05,0x2407,0x1c05,0x1c05,
0x1c05,0x1c05,0x1c05,0x1c05,
0x0,0x0,0x2c05,0x2c05,0x2408,0x2c05,0x2c05,0x9001,0x2c05,0x2c05,0x9402,0x9403,
0x9404,0x9405,0x9406,0x9407,
0x0,0x0,0x2c05,0x2c05,0x2409,0x2c05,0x2c05,0x9001,0x2c05,0x2c05,0x9402,0x9403,
0x9404,0x9405,0x9406,0x9407,
0x0,0x0,0x2c05,0x2c05,0x0,0x2c05,0x0,0x2c05,0x0,0x2406,0x2c05,0x2c05,
0x2c05,0x2c05,0x2c05,0x2c05,
0x0,0x0,0x2c05,0x2c05,0x0,0x2c05,0x4401,0x2c05,0x0,0x2407,0x2c05,0x2c05,
0x2c05,0x2c05,0x2c05,0x2c05,
0x0,0x0,0x9008,0x9008,0x0,0x9008,0x9008,0x2c0b,0x9008,0x9008,0x9008,0x9008,
0x9008,0x9008,0x9008,0x9008,
0x0,0x0,0x9008,0x9008,0x0,0x9008,0x9008,0x9c08,0x9008,0x9008,0x9008,0x9008,
0x9008,0x9008,0x9008,0x9008,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[16],
&SXS_transition[32],
&SXS_transition[48],
&SXS_transition[64],
&SXS_transition[80],
&SXS_transition[96],
&SXS_transition[112],
&SXS_transition[128],
&SXS_transition[144],
&SXS_transition[160],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2c0a,10,0,1,0},
{0x2005,9,8,1,0},
{0x2005,9,12,1,0},
{0x2005,9,10,1,0},
{0x2005,9,13,1,0},
{0x2005,9,9,1,0},
{0x2005,9,11,1,0},
{0x2005,1,0,0,0},
};
static char *S_adrp[]={0,
"%TERMINAL",
"End Of File",
};
static int S_is_a_keyword[]={
0};
static int S_is_a_generic_terminal[]={
1,1};
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
extern int SCANACT();
extern int sxscan_it();
extern SXBOOLEAN sxsrecovery();
extern int sxscanner();
extern int sxparser();
extern int SEMACT();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,sxparser}, {255, 2, 1, 3, 4, 16, 0, 11, 1, 1, 0, 
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
NULL,
},
{1, 4, 4, 6, 6, 8, 9, 9, 2, 3, 4, 4, 4, 3, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions,
t_bases,
nt_bases,
vector-9,
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
/* End of sxtables for make_proper [Thu Jun 21 15:51:59 2007] */
