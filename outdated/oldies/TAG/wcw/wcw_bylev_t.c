/* Beginning of sxtables for wcw [Fri Jan 20 12:01:58 1995] */
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={
{0, 5, 0, -64},
{10000, 6, 0, -33},
{10005, 11, 0, -35},
{10001, 7, 1, 74},
{10003, 19, 0, -33},
{10003, 26, 0, -33},
{10007, 18, 0, -47},
{10003, 12, 0, -33},
{10001, 14, 1, 74},
{10009, 25, 0, -53},
{10001, 21, 1, 74},
{10002, 10, 1, 78},
{10006, 17, 1, 82},
{10008, 24, 1, 86},
{0, 8, 0, -61},
{0, 15, 0, -62},
{0, 22, 1, -63},
{0, 4, 1, 73},
{0, 0, 2, -64},
{10003, 9, 1, -35},
{10003, 12, 1, -33},
{0, 27, 0, -14},
{0, 13, 0, -12},
{0, 20, 0, -13},
{10003, 16, 1, -47},
{10003, 19, 1, -33},
{10003, 23, 1, -53},
{10003, 26, 1, -33},
};
static struct SXP_bases t_bases[]={
{0, 90, 0},
{0, 93, 0},
{0, 93, 0},
{0, 93, 0},
{0, 93, 0},
{0, -19, 4},
{-21, -20, 4},
{-26, -25, 4},
{-28, -27, 4},
{-18, 87, 0},
{0, 60, 4},
};
static struct SXP_bases nt_bases[]={
{-14, 70, 0},
{-64, 92, 0},
{-41, 94, 74},
{-49, 94, 99},
{-55, 94, 97},
};
static struct SXP_item vector[]={
{5, -31},
{10, -12},
{0, 0},
{8, -35},
{9, -37},
{14, -13},
{0, 0},
{0, 0},
{13, -39},
{1, -57},
{2, -57},
{3, -57},
{1, 22},
{2, 23},
{3, 24},
{1, 68},
{2, 15},
{3, 16},
{5, -29},
{4, -2},
{7, -61},
{6, -33},
{7, -61},
{5, -51},
{11, -62},
{5, -43},
{11, -62},
{9, -45},
{15, -63},
{9, -45},
{15, -63},
{13, -39},
{12, -47},
{0, 0},
{16, -53},
{0, 0},
{0, 0},
{17, -41},
};
static struct SXP_prdct prdcts[]={
{10000, -1},
{-1, 0},
{10005, -3},
{-1, 0},
{10004, -59},
{-1, 0},
{10002, -4},
{-1, 0},
{10003, -65},
{-1, 0},
{10003, -5},
{-1, 0},
{10003, -6},
{-1, 0},
{10007, -7},
{-1, 0},
{10003, -8},
{-1, 0},
{10006, -9},
{-1, 0},
{10003, -66},
{-1, 0},
{10009, -10},
{-1, 0},
{10008, -11},
{-1, 0},
{10003, -67},
{-1, 0},
{20001, -17},
{20000, -18},
};
static struct SXP_prdct_list prdct_list[]={
{5, 1, 10000},
{7, 2, 10002},
{9, 1, 10003},
{10, 1, 10004},
{11, 1, 10005},
{12, 1, 10003},
{14, 2, 10006},
{16, 1, 10003},
{17, 1, 10004},
{18, 1, 10007},
{19, 1, 10003},
{21, 2, 10008},
{23, 1, 10003},
{24, 1, 10004},
{25, 1, 10009},
{26, 1, 10003},
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
extern SXPARSACT_FUNCTION PARSACT;
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
3,4,5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,};
static SXSTMI SXS_transition[]={
0x0,0x4004,0x4402,0x4403,0x4401,
};
static SXSTMI *S_transition_matrix[]={
&SXS_transition[0]-1,
};
static char *S_adrp[]={
"c",
"a",
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

/********************* start of LIG tables ********************/
static int SXLIG_prdct_or_act_disp [] = {
 0, 1, 6, 12, 17, 22, 28, 34, 40, 46, 52,
};

static int SXLIG_prdct_or_act_code [] = {
/* (&|@)10000 */ 0, /* ( ) */
/* (&|@)10001 */ 1, 1, 1, 0, -1, /* ( .. ) */
/* (&|@)10002 */ 1, 0, 0, 0, 10, -1, /* ( .. "2" ) */
/* (&|@)10003 */ 1, 0, 0, 0, -1, /* ( .. ) */
/* (&|@)10004 */ 1, 0, 1, 0, -1, /* ( .. ) */
/* (&|@)10005 */ 1, 0, 1, 1, 10, -1, /* ( .. "2" ) */
/* (&|@)10006 */ 1, 0, 0, 0, 16, -1, /* ( .. "3" ) */
/* (&|@)10007 */ 1, 0, 1, 1, 16, -1, /* ( .. "3" ) */
/* (&|@)10008 */ 1, 0, 0, 0, 22, -1, /* ( .. "4" ) */
/* (&|@)10009 */ 1, 0, 1, 1, 22, -1, /* ( .. "4" ) */
};


static int SXLIG_map_disp [] = {
 0, 3, 5, 7, 9, 11, 13, 15, 18, 20, 22, 25, 27, 29, 31, 34, 36, 38, 41, 43, 45, 47, 50, 52, 54, 57, 59, 61, 63,
};


static int SXLIG_map_code [] = {
/* 0	: */ -1, -1, -1,
/* 1	: */ -1, -1,
/* 2	: */ -1, -1,
/* 3	: */ -1, -1,
/* 4	: */ -1, -1,
/* 5	: */ -1, 10000,
/* 6	: */ 10000, -1,
/* 7	: */ 10001, -1, 10002,
/* 8	: */ -1, -1,
/* 9	: */ 10003, 10003,
/* 10	: */ 10002, 10004, -1,
/* 11	: */ 10005, 10005,
/* 12	: */ 10003, 10003,
/* 13	: */ -1, -1,
/* 14	: */ 10001, -1, 10006,
/* 15	: */ -1, -1,
/* 16	: */ 10003, 10003,
/* 17	: */ 10006, 10004, -1,
/* 18	: */ 10007, 10007,
/* 19	: */ 10003, 10003,
/* 20	: */ -1, -1,
/* 21	: */ 10001, -1, 10008,
/* 22	: */ -1, -1,
/* 23	: */ 10003, 10003,
/* 24	: */ 10008, 10004, -1,
/* 25	: */ 10009, 10009,
/* 26	: */ 10003, 10003,
/* 27	: */ -1, -1,
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
extern SXPARSER_FUNCTION sxndparser;
extern SXDESAMBIG_FUNCTION ESAMBIG;
extern SXSEMACT_FUNCTION SEMACT;

SXTABLES sxtables={
52113, /* magic */
sxscanner,
sxndparser,
{255, 4, 1, 3, 4, 5, 0, 1, 0, 1, 0, 
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
{14, 28, 58, 60, 63, 69, 79, 116, 4, 18, 27, 27, 11, 6, 16, 9, 4, 7, 2, 5, 11, 5, 4,
reductions-1,
t_bases-1,
nt_bases-1,
vector-79,
#if SXBITS_PER_LONG == 32
NULL,
#else
NULL,
#endif
prdcts-29,
NULL,
prdct_list-1,
P_lregle-1,
P_right_ctxt_head-1,
SXP_local_mess-1,
P_no_delete,
P_no_insert,
P_global_mess,
PER_tset,
sxscan_it,
sxndprecovery,
PARSACT
,ESAMBIG
,SEMACT},
err_titles,
abstract,
(sxsem_tables*)NULL,
&sxligparsact,
};
/* End of sxtables for wcw [Fri Jan 20 12:01:58 1995] */
