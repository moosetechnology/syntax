/* Beginning of sxtables for anbm_lr [Thu Dec 19 15:44:40 1996] */
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={
{0, 2, 0, -11},
{0, 4, 0, 15},
{0, 5, 1, 16},
{0, 6, 0, 16},
{0, 1, 1, -11},
{0, 0, 2, -11},
{0, 3, 2, 15},
};
static struct SXP_bases t_bases[]={
{-4, 8, 4},
{-4, 16, 0},
{-4, 16, 0},
{0, 18, 0},
{0, 7, 3},
{0, 9, 5},
};
static struct SXP_bases nt_bases[]={
{-3, 0, 0},
{-2, 14, 0},
{-12, -2, 3},
};
static struct SXP_item vector[]={
{1, -11},
{2, -1},
{0, 0},
{2, 10},
{1, 5},
{4, 8},
{0, 0},
{0, 0},
{5, -6},
};
static short P_lrgl[]={
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
static short *P_lregle[]={
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
static int P_right_ctxt_head[]={
1,2,2,1,1,1,1,2,1,};
static short P_param_ref[]={
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
static struct SXP_local_mess SXP_local_mess[]={
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
static short P_no_delete[]={
0};
static short P_no_insert[]={
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
static short PER_tset[]={
0};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%d warnings and %d errors are reported.";
extern SXPRECOVERY_FUNCTION sxndprecovery;

static unsigned char S_char_to_simple_class[]={
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
3,4,5,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,};
static SXSTMI SXS_transition[]={
0x0,0x4005,0x4402,0x4404,0x4403,0x4401,
};
static SXSTMI *S_transition_matrix[]={
&SXS_transition[0]-1,
};
static char *S_adrp[]={
"d",
"a",
"c",
"b",
"End Of File",
};
static short S_is_a_keyword[]={
0};
static short S_is_a_generic_terminal[]={
0};
static short S_lrgl[]={
4,1,2,3,4,0,
5,-1,1,2,3,4,
5,-1,0,1,2,3,
};
static short *S_lregle[]={
&S_lrgl[0],
&S_lrgl[6],
&S_lrgl[12],
};
static short S_param_ref[]={
0,
0,-1,
-1,0,
};
static struct SXS_local_mess SXS_local_mess[]={
{1,"%sThe invalid character \"%s\" is deleted.",&S_param_ref[0]},
{2,"%sThe invalid character \"%s\" is replaced by \"%s\".",&S_param_ref[1]},
{2,"%sThe character \"%s\" is inserted before \"%s\".",&S_param_ref[3]},
};
static short S_no_delete[]={
0};
static short S_no_insert[]={
0};
static char *S_global_mess[]={
"%s\"%s\" is deleted.",
"%sThis unknown keyword is erased.",
"End Of Line",
"End Of File",
"%sScanning stops on End Of File.",
};
extern SXPARSER_FUNCTION sxndparser;
extern int ESAMBIG();
extern SXSEMACT_FUNCTION SEMACT;

SXTABLES sxtables={
52113, /* magic */
sxscanner,
sxndparser,
{255, 5, 1, 3, 4, 6, 0, 1, 0, 1, 0, 
S_is_a_keyword,S_is_a_generic_terminal,S_transition_matrix-1,
NULL,
S_adrp-1,
NULL,
SXS_local_mess-1,
S_char_to_simple_class+128,
S_no_delete,S_no_insert,
S_global_mess-1,
S_lregle-1,
NULL,
sxsrecovery,
NULL
},
{3, 7, 7, 9, 10, 13, 15, 23, 5, 3, 6, 6, 6, 4, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions-1,
t_bases-1,
nt_bases-1,
vector-15,
#if SXBITS_PER_LONG == 32
NULL,
#else
NULL,
#endif
NULL,
NULL,
NULL,
P_lregle-1,
P_right_ctxt_head-1,
SXP_local_mess-1,
P_no_delete,
P_no_insert,
P_global_mess,
PER_tset,
sxscan_it,
sxndprecovery,
NULL
,ESAMBIG
,SEMACT},
err_titles,
abstract,
(sxsem_tables*)NULL,
NULL,
};
/* End of sxtables for anbm_lr [Thu Dec 19 15:44:40 1996] */
