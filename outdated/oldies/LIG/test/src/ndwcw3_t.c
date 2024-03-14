/* Beginning of sxtables for ndwcw3 [Tue Mar  7 15:29:53 1995] */
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={
{0, 1, 0, -34},
{10005, 5, 0, -11},
{10006, 6, 1, 41},
{10007, 7, 1, 41},
{10008, 8, 1, 41},
{10000, 9, 1, 41},
{0, 0, 2, -34},
{10001, 2, 1, -11},
{10001, 3, 1, -11},
{10001, 4, 1, -11},
};
static struct SXP_bases t_bases[]={
{-6, 45, 0},
{0, 42, 0},
{0, 42, 0},
{0, 42, 0},
{0, -7, 4},
{0, 10, 3},
{0, 9, 2},
{0, 8, 1},
{0, 31, 4},
};
static struct SXP_bases nt_bases[]={
{-22, 0, 0},
{-16, 39, 0},
{-18, 0, 0},
{-20, 0, 0},
};
static struct SXP_item vector[]={
{1, -34},
{2, -11},
{0, 0},
{1, 32},
{2, 33},
{3, 30},
{1, -24},
{2, -26},
{3, -28},
{0, 0},
};
static struct SXP_prdct prdcts[]={
{10004, -35},
{10003, -36},
{10002, -37},
{10000, -1},
{-1, 0},
{10005, -2},
{-1, 0},
{10001, -3},
{-1, 0},
{10001, -4},
{-1, 0},
{10001, -5},
{-1, 0},
{20001, -6},
{20000, 32},
{20001, -6},
{20000, 33},
{20001, -6},
{20000, 30},
};
static struct SXP_prdct_list prdct_list[]={
{1, 1, 10000},
{2, 1, 10002},
{3, 1, 10003},
{4, 1, 10004},
{5, 1, 10005},
{6, 2, 10001},
{7, 2, 10001},
{8, 2, 10001},
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
extern int PARSACT();
extern bool sxndprecovery();

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
3,4,5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,};
static SXSTMI SXS_transition[]={
0x0,0x4004,0x4401,0x4402,0x4403,
};
static SXSTMI *S_transition_matrix[]={
&SXS_transition[0]-1,
};
static char *S_adrp[]={
"a",
"b",
"c",
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
extern int sxscan_it();
extern bool sxsrecovery();

/********************* start of LIG tables ********************/
static int SXLIG_prdct_or_act_disp [] = {
 0, 1, 6, 12, 18, 24, 29, 35, 41, 47,
};

static int SXLIG_prdct_or_act_code [] = {
/* (&|@)10000 */ 0, /* ( ) */
/* (&|@)10001 */ 1, 0, 1, 0, -1, /* ( .. ) */
/* (&|@)10002 */ 1, 0, 0, 0, 3, -1, /* ( .. "a" ) */
/* (&|@)10003 */ 1, 0, 0, 0, 4, -1, /* ( .. "b" ) */
/* (&|@)10004 */ 1, 0, 0, 0, 5, -1, /* ( .. "c" ) */
/* (&|@)10005 */ 1, 0, 0, 0, -1, /* ( .. ) */
/* (&|@)10006 */ 1, 1, 0, 0, 3, -1, /* ( .. "a" ) */
/* (&|@)10007 */ 1, 1, 0, 0, 4, -1, /* ( .. "b" ) */
/* (&|@)10008 */ 1, 1, 0, 0, 5, -1, /* ( .. "c" ) */
};


static int SXLIG_map_disp [] = {
 0, 3, 5, 8, 11, 14, 16, 19, 22, 25, 27,
};


static int SXLIG_map_code [] = {
/* 0	: */ -1, -1, -1,
/* 1	: */ -1, 10000,
/* 2	: */ 10001, 10002, -1,
/* 3	: */ 10001, 10003, -1,
/* 4	: */ 10001, 10004, -1,
/* 5	: */ 10005, 10005,
/* 6	: */ 10006, -1, 10001,
/* 7	: */ 10007, -1, 10001,
/* 8	: */ 10008, -1, 10001,
/* 9	: */ 10000, -1,
};

int PARSACT_2 ();

static struct sxligparsact sxligparsact = {
 SXLIG_prdct_or_act_disp,
 SXLIG_prdct_or_act_code,
 SXLIG_map_disp,
 SXLIG_map_code,
 PARSACT_2
};
/********************* end of LIG tables ********************/
extern int sxscanner();
extern int sxndparser();
extern int ESAMBIG();
extern int SEMACT();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,sxndparser}, {255, 4, 1, 3, 4, 5, 0, 1, 0, 1, 0, 
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
NULL,
},
{5, 10, 29, 31, 33, 38, 40, 49, 4, 3, 9, 9, 9, 5, 8, 9, 4, 7, 2, 5, 11, 5, 4,
reductions-1,
t_bases-1,
nt_bases-1,
vector-40,
#if SXBITS_PER_LONG == 32
NULL,
#else
NULL,
#endif
prdcts-11,
NULL,
prdct_list-1,
P_lregle-1,P_right_ctxt_head-1,
SXP_local_mess-1,
P_no_delete,P_no_insert,
P_global_mess,PER_tset,sxscan_it,sxndprecovery,
PARSACT
,ESAMBIG
,SEMACT},
err_titles,
abstract,
(sxsem_tables*)NULL,
&sxligparsact,
};
/* End of sxtables for ndwcw3 [Tue Mar  7 15:29:53 1995] */
