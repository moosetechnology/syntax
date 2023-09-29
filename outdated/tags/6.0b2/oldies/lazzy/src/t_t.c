/* Beginning of sxtables for t [Thu Sep 16 17:17:05 1993] */
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={
{0, 2, 0, -10},
{0, 1, 1, -10},
{0, 5, 0, -11},
{0, 4, 1, -11},
{0, 6, 0, 18},
{0, 7, 0, 18},
{0, 8, 0, 18},
{0, 0, 2, -10},
{0, 3, 3, 17},
};
static struct SXP_bases t_bases[]={
{0, 16, 19},
{0, 16, 20},
{0, 16, 0},
{0, 16, 0},
{0, 13, 1},
{0, 12, 6},
};
static struct SXP_bases nt_bases[]={
{-2, -14, 3},
{-4, 0, 0},
{-14, 22, 0},
{-11, -3, 3},
};
static struct SXP_item vector[]={
{0, 0},
{0, 0},
{3, 5},
{4, 6},
{5, 7},
{2, 9},
{1, -10},
{2, -1},
{6, -8},
{0, 0},
};
static SXBA_ELT SXPBA_kt2[]={
7,126,};
static SXBA SXPBM_trans [] = {
&SXPBA_kt2[0],
};
static short P_lrgl[]={
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
&P_lrgl[72],
&P_lrgl[80],
};
static int P_right_ctxt_head[]={
2,1,1,2,2,1,1,1,1,2,1,};
static short P_param_ref[]={
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
static struct SXP_local_mess SXP_local_mess[]={
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
static short P_no_delete[]={
1,1};
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
1,2};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%d errors and %d warnings are reported.";
extern SXBOOLEAN sxprecovery();

static unsigned char S_char_to_simple_class[]={
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,3,3,3,3,3,3,3,3,3,4,5,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,3,6,3,3,3,3,3,3,3,3,3,3,3,
3,7,7,7,7,7,7,7,7,7,7,3,8,3,9,3,3,3,10,10,10,10,10,10,10,
10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,3,3,3,3,3,3,
10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
10,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,};
static SXSTMI SXS_transition[]={
0x0,0x4006,0x0,0x2402,0x2402,0x2403,0x2c04,0x4402,0x4401,0x2c05,
0x0,0x4000,0x0,0x1402,0x1402,0x4000,0x4000,0x4000,0x4000,0x4000,
0x0,0x0,0x1c03,0x1403,0x4400,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x0,0x0,0x0,0x4005,0x4005,0x4005,0x1c04,0x4005,0x4005,0x4005,
0x0,0x0,0x0,0x3004,0x3004,0x3004,0x1c05,0x3004,0x3004,0x1c05,
};
static SXSTMI *S_transition_matrix[]={
&SXS_transition[0]-1,
&SXS_transition[10]-1,
&SXS_transition[20]-1,
&SXS_transition[30]-1,
&SXS_transition[40]-1,
};
static char *S_adrp[]={
"=",
";",
"if",
"%id",
"%int",
"End Of File",
};
static short S_is_a_keyword[]={
1,3};
static short S_is_a_generic_terminal[]={
2,4,5};
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
extern int sxscan_it();
extern SXBOOLEAN sxsrecovery();
static int check_keyword();
extern int sxscanner();
extern int sxparser();
extern int SEMACT();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,sxparser}, {255, 6, 1, 3, 4, 10, 0, 5, 0, 0, 0, 
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
check_keyword,
},
{4, 9, 9, 12, 13, 15, 17, 26, 6, 4, 8, 8, 6, 0, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions-1,
t_bases-1,
nt_bases-1,
vector-17,
#if SXBITS_PER_LONG == 32
SXPBM_trans-1,
#else
NULL,
#endif
NULL,
NULL,
NULL,
P_lregle-1,P_right_ctxt_head-1,
SXP_local_mess-1,
P_no_delete,P_no_insert,
P_global_mess,PER_tset,sxscan_it,sxprecovery,
NULL
,NULL
,SEMACT},
err_titles,
abstract,
(sxsem_tables*)NULL
};
/* ********************************************************************
   *  This program has been generated from t.lecl                     *
   *  on Thu Sep 16 17:16:24 1993                                     *
   *  by the SYNTAX (*) lexical constructor LECL                      *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */



static int check_keyword (string, length)
register char	*string;
register int	length;
{
   register int  t_code, delta;
   register char *keyword;

   switch (length) {
   case 2:
      return *string++ == 'i' && *string == 'f' ? 3 /* "if" */: 0;

   default:
      return 0;
   }

}

/* End of sxtables for t [Thu Sep 16 17:17:05 1993] */
