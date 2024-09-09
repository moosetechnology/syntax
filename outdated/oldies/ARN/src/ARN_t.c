/* Beginning of sxtables for ARN [Wed May 31 17:37:34 1995] */
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={
{2, 2, 0, -192},
{3, 4, 0, -192},
{1, 1, 1, -192},
{2, 3, 1, -192},
{7, 23, 0, 237},
{0, 34, 1, 236},
{8, 24, 1, 237},
{10, 26, 1, 238},
{12, 28, 1, 238},
{0, 30, 1, 235},
{0, 31, 1, 235},
{0, 33, 1, 235},
{0, 32, 1, 235},
{0, 0, 2, -192},
{11, 27, 1, 238},
{7, 23, 1, 237},
{0, 35, 1, 236},
{13, 29, 1, 238},
{0, 30, 0, 235},
{0, 33, 0, 235},
{0, 32, 0, 235},
{0, 31, 0, 235},
{9, 25, 2, 237},
{4, 5, 3, 234},
{5, 6, 3, 234},
{6, 7, 3, 234},
{4, 14, 3, 234},
{4, 20, 3, 234},
{5, 15, 3, 234},
{5, 21, 3, 234},
{6, 16, 3, 234},
{6, 22, 3, 234},
{4, 11, 3, 234},
{4, 8, 3, 234},
{5, 12, 3, 234},
{5, 9, 3, 234},
{6, 13, 3, 234},
{6, 10, 3, 234},
{4, 17, 3, 234},
{5, 18, 3, 234},
{6, 19, 3, 234},
};
static struct SXP_bases t_bases[]={
{0, 296, 342},
{0, 321, 0},
{0, 298, 335},
{0, 298, 336},
{0, 323, 285},
{-10, 313, 0},
{-11, 305, 0},
{-12, 317, 0},
{-13, 301, 0},
{0, 296, 345},
{0, 324, 275},
{0, 324, 274},
{0, 323, 282},
{0, 298, 339},
{0, 298, 340},
{0, 270, 0},
{0, 324, 279},
{0, 324, 278},
{0, 296, 343},
{0, 296, 345},
{0, 352, 0},
{-26, 262, 0},
{-31, 368, 0},
{-32, 250, 0},
{-37, 328, 0},
{-38, 292, 0},
{-41, 309, 0},
{0, 348, 0},
{-24, 266, 0},
{-25, 356, 0},
{-27, 360, 0},
{-28, 258, 0},
{-29, 364, 0},
{-30, 254, 0},
{-33, 372, 0},
{-34, 246, 0},
{-35, 376, 0},
{-36, 242, 0},
{-39, 332, 0},
{-40, 288, 0},
{0, 201, 5},
};
static struct SXP_bases nt_bases[]={
{-4, -3, 2},
{-212, 383, 0},
{-6, 0, 0},
{-7, 0, 0},
{-9, -8, 2},
{-196, 239, 235},
{-204, 239, 379},
{-207, 239, 233},
{-210, 239, 377},
{-2, 386, 0},
{-6, 0, 0},
{-7, 0, 0},
{-9, -8, 2},
{-6, 0, 0},
{-7, 0, 0},
{-9, -8, 2},
{-6, 0, 0},
{-7, 0, 0},
{-9, -8, 2},
{-212, 383, 0},
{-7, 0, 0},
{-204, 239, 379},
{-196, 239, 235},
{-207, 239, 233},
{-204, 239, 379},
{-210, 239, 377},
{-207, 239, 233},
};
static struct SXP_item vector[]={
{4, -205},
{5, -206},
{4, -194},
{5, -195},
{2, -219},
{3, -193},
{1, -150},
{2, -150},
{3, -150},
{4, -150},
{1, -146},
{2, -146},
{3, -146},
{4, -146},
{1, -132},
{2, -135},
{3, -138},
{4, -141},
{1, -118},
{2, -118},
{3, -118},
{4, -118},
{1, -114},
{2, -114},
{3, -114},
{4, -114},
{1, -100},
{2, -103},
{3, -106},
{4, -109},
{1, -96},
{2, -96},
{3, -96},
{4, -96},
{1, 197},
{2, 216},
{3, 199},
{4, 217},
{1, 224},
{1, 222},
{3, 225},
{3, 223},
{1, 19},
{1, 19},
{3, 231},
{3, 230},
{1, 214},
{2, 198},
{3, 215},
{1, 197},
{2, 213},
{3, 199},
{1, -178},
{2, -178},
{3, -178},
{4, -178},
{1, -164},
{2, -167},
{3, -170},
{4, -173},
{1, 197},
{2, 198},
{1, 19},
{4, 200},
{3, 20},
{1, -66},
{2, -68},
{3, -70},
{4, -72},
{1, -50},
{2, -52},
{3, -54},
{4, -56},
{1, -180},
{2, -183},
{3, -186},
{4, -189},
{1, -42},
{2, -44},
{3, -46},
{4, -48},
{1, -58},
{2, -60},
{3, -62},
{4, -64},
{1, -76},
{2, -79},
{3, -82},
{4, -85},
{2, 22},
{4, 200},
{4, 21},
{1, -152},
{2, -155},
{3, -158},
{4, -161},
{1, -176},
{2, -176},
{3, -176},
{4, -176},
{2, 220},
{2, 221},
{4, 21},
{4, 21},
{2, 226},
{2, 228},
{4, 227},
{4, 229},
{3, 199},
{3, 218},
{5, -14},
{3, 199},
{1, -74},
{2, -74},
{3, -74},
{4, -74},
{1, -88},
{2, -90},
{3, -92},
{4, -94},
{1, -98},
{2, -98},
{3, -98},
{4, -98},
{1, -112},
{2, -112},
{3, -112},
{4, -112},
{1, -116},
{2, -116},
{3, -116},
{4, -116},
{1, -120},
{2, -123},
{3, -126},
{4, -129},
{1, -144},
{2, -144},
{3, -144},
{4, -144},
{1, -148},
{2, -148},
{3, -148},
{4, -148},
{4, -208},
{5, -209},
{4, -202},
{5, -203},
{2, -5},
{3, -211},
{1, -192},
{2, -1},
{0, 0},
};
static struct SXP_prdct prdcts[]={
{20001, -10},
{20000, 197},
{20001, -10},
{20000, 198},
{20001, -10},
{20000, 199},
{20001, -10},
{20000, 200},
{20001, -11},
{20000, 197},
{20001, -11},
{20000, 198},
{20001, -11},
{20000, 199},
{20001, -11},
{20000, 200},
{20001, -12},
{20000, 197},
{20001, -12},
{20000, 198},
{20001, -12},
{20000, 199},
{20001, -12},
{20000, 200},
{20001, -13},
{20000, 197},
{20001, -13},
{20000, 198},
{20001, -13},
{20000, 199},
{20001, -13},
{20000, 200},
{20001, -15},
{20000, -16},
{20002, -17},
{20001, -18},
{20000, 197},
{20002, -17},
{20001, -18},
{20000, 198},
{20002, -17},
{20001, -18},
{20000, 199},
{20002, -17},
{20001, -18},
{20000, 200},
{20001, -23},
{20000, 19},
{20001, -23},
{20000, 22},
{20001, -23},
{20000, 20},
{20001, -23},
{20000, 21},
{20001, -11},
{20000, -24},
{20001, -11},
{20000, -25},
{20002, -11},
{20001, -26},
{20000, 197},
{20002, -11},
{20001, -26},
{20000, 198},
{20002, -11},
{20001, -26},
{20000, 199},
{20002, -11},
{20001, -26},
{20000, 200},
{20001, -10},
{20000, -27},
{20001, -12},
{20000, -28},
{20001, -10},
{20000, -29},
{20001, -12},
{20000, -30},
{20002, -10},
{20001, -31},
{20000, 197},
{20002, -10},
{20001, -31},
{20000, 198},
{20002, -10},
{20001, -31},
{20000, 199},
{20002, -10},
{20001, -31},
{20000, 200},
{20002, -12},
{20001, -32},
{20000, 197},
{20002, -12},
{20001, -32},
{20000, 198},
{20002, -12},
{20001, -32},
{20000, 199},
{20002, -12},
{20001, -32},
{20000, 200},
{20001, -11},
{20000, -33},
{20001, -13},
{20000, -34},
{20001, -11},
{20000, -35},
{20001, -13},
{20000, -36},
{20002, -11},
{20001, -37},
{20000, 197},
{20002, -11},
{20001, -37},
{20000, 198},
{20002, -11},
{20001, -37},
{20000, 199},
{20002, -11},
{20001, -37},
{20000, 200},
{20002, -13},
{20001, -38},
{20000, 197},
{20002, -13},
{20001, -38},
{20000, 198},
{20002, -13},
{20001, -38},
{20000, 199},
{20002, -13},
{20001, -38},
{20000, 200},
{20001, -12},
{20000, -39},
{20001, -12},
{20000, -40},
{20002, -12},
{20001, -41},
{20000, 197},
{20002, -12},
{20001, -41},
{20000, 198},
{20002, -12},
{20001, -41},
{20000, 199},
{20002, -12},
{20001, -41},
{20000, 200},
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
static char abstract []= "%d errors and %d warnings are reported.";
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
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,4,1,1,1,5,
1,1,1,1,1,1,1,1,1,1,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,};
static SXSTMI SXS_transition[]={
0x0,0x4005,0x4401,0x4404,0x4403,0x4402,
};
static SXSTMI *S_transition_matrix[]={
&SXS_transition[0]-1,
};
static char *S_adrp[]={
"A",
"U",
"G",
"C",
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
// extern int ESAMBIG();
// extern SXSEMACT_FUNCTION SEMACT;

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
{9, 41, 191, 201, 218, 232, 237, 389, 5, 6, 35, 35, 41, 0, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions-1,
t_bases-1,
nt_bases-1,
vector-237,
#if SXBITS_PER_LONG == 32
NULL,
#else
NULL,
#endif
prdcts-42,
NULL,
NULL,
P_lregle-1,P_right_ctxt_head-1,
SXP_local_mess-1,
P_no_delete,P_no_insert,
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
NULL,
};
/* End of sxtables for ARN [Wed May 31 17:37:34 1995] */
