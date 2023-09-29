/* Beginning of sxtables for dico [Thu Jun 21 15:52:00 2007] */
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
extern int SCANACT();
extern int sxscan_it();
extern SXBOOLEAN sxsrecovery();
extern int sxscanner();
extern int sxparser();
extern int SEMACT();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,sxparser}, {255, 3, 1, 3, 4, 18, 0, 10, 1, 1, 0, 
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
{0, 7, 7, 8, 8, 14, 16, 18, 3, 3, 6, 6, 7, 2, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
vector-16,
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
/* End of sxtables for dico [Thu Jun 21 15:52:00 2007] */
