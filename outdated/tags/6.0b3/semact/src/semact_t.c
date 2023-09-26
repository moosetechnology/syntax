/* Beginning of sxtables for semact [Thu Mar  6 12:56:22 2008] */
#define SXP_MAX 5
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{1, 1, 0, -5},
{0, 2, 0, -5},
{0, 0, 2, -5},
};
static struct SXP_bases t_bases[]={{0,0,0},
{-2, 1, 1},
{0, -3, 2},
{0, 4, 2},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-5, 0, 0},
};
static struct SXP_item vector[]={
{0, 0},
};
static SXINT P_lrgl[]={
4,0,2,3,4,
4,1,2,3,4,
};
static SXINT *P_lregle[]={NULL,
&P_lrgl[0],
&P_lrgl[5],
};
static SXINT P_right_ctxt_head[]={0,
2,1,};
static SXINT P_param_ref[]={
1,
0,1,
-1,1,
};
static struct SXP_local_mess SXP_local_mess[]={{0,NULL,NULL},
{1,"%s\"%s\" is deleted.",&P_param_ref[0]},
{2,"%s\"%s\" before \"%s\" is deleted.",&P_param_ref[1]},
{2,"%s\"%s\" is forced before \"%s\". ",&P_param_ref[3]},
};
static SXINT P_no_delete[]={
0};
static SXINT P_no_insert[]={
0};
static char *P_global_mess[]={
"%s\"%s\" is expected",
"%sGlobal recovery.",
"%sParsing resumes on \"%s\"",
"End Of Rule",
"%sParsing stops on End Of Rule.",
};
static SXINT PER_tset[]={
0};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning (semact):\t",
"\002Error (semact):\t",
};
static char abstract []= "%d warnings and %d errors are reported.";
extern SXBOOLEAN sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no);

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,4,5,4,4,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,3,3,3,3,3,3,3,3,6,3,3,3,3,
3,7,7,7,7,7,7,7,7,7,7,3,3,8,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x4002,0x0,0x2402,0x2403,0x0,0x2c04,0x9002,
0x0,0x4000,0x0,0x1402,0x2403,0x0,0x4000,0x4000,
0x0,0x4000,0x0,0x2402,0x1403,0x2405,0x4000,0x4000,
0x0,0x4001,0x0,0x4001,0x4001,0x0,0x1c04,0x4001,
0x0,0x0,0x1405,0x1405,0x2403,0x1405,0x1405,0x1405,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[8],
&SXS_transition[16],
&SXS_transition[24],
&SXS_transition[32],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x4002,11,0,1,0},
{0x9c01,1,0,1,1},
{0x0,0,0,1,1},
};
static char *S_adrp[]={0,
"%ACTION",
"End Of File",
};
static SXINT S_is_a_keyword[]={
0};
static SXINT S_is_a_generic_terminal[]={
1,1};
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
"%s",
"End Of Line",
"< or End Of File",
"%sScanning stops on End Of Rule.",
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
{sxscanner,(PARSER_T) sxparser}, {255, 2, 1, 3, 4, 8, 0, 5, 0, 1, 0, 
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
{0, 3, 3, 4, 4, 6, 7, 7, 2, 1, 2, 2, 3, 2, 0, 2, 4, 4, 2, 1, 8, 2, 1,
reductions,
t_bases,
nt_bases,
vector-7,
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
/* End of sxtables for semact [Thu Mar  6 12:56:22 2008] */
