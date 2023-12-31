/* Beginning of sxtables for pprecor [Thu Mar  6 12:56:21 2008] */
#define SXP_MAX 185
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{1, 7, 0, -60},
{1, 3, 2, -100},
{1, 6, 2, -59},
{1, 31, 0, -64},
{1, 1, 3, -99},
{1, 9, 2, -1},
{1, 10, 1, 160},
{1, 13, 1, -63},
{1, 38, 1, 161},
{1, 33, 2, -4},
{1, 30, 3, -61},
{1, 36, 2, 161},
{1, 37, 2, 161},
{1, 18, 1, 164},
{1, 23, 5, -3},
{1, 44, 2, -106},
{1, 49, 2, 182},
{1, 46, 2, -108},
{1, 54, 7, -11},
{1, 5, 0, 156},
{0, 0, 2, -99},
{1, 2, 2, -58},
{1, 8, 0, -60},
{1, 15, 0, 165},
{1, 16, 0, 165},
{1, 17, 0, 165},
{1, 32, 0, -64},
{1, 64, 0, -5},
{1, 52, 0, 167},
{1, 65, 2, -5},
{1, 21, 1, 166},
{1, 22, 1, 166},
{1, 12, 3, 162},
{1, 20, 1, 166},
{1, 53, 2, -67},
{1, 40, 3, 178},
{1, 50, 0, 184},
{1, 51, 0, 184},
{1, 42, 3, 179},
{1, 34, 3, 161},
{1, 35, 3, 161},
{1, 61, 3, 168},
{1, 39, 4, 178},
{0, 48, 1, 182},
{1, 41, 4, 179},
{1, 24, 3, -69},
{1, 25, 3, -70},
{1, 57, 0, -109},
{1, 28, 3, 171},
{1, 56, 4, -71},
{1, 59, 3, -73},
{1, 29, 3, 172},
{1, 58, 2, -109},
{1, 55, 5, -71},
{1, 27, 0, -93},
{1, 60, 5, -92},
{1, 62, 3, -75},
};
static struct SXP_bases t_bases[]={{0,0,0},
{0, 77, 5},
{0, 79, 18},
{0, 80, 12},
{-28, 110, 35},
{-29, 161, 172},
{0, 161, 169},
{0, 96, 27},
{-29, 0, 0},
{0, 120, 13},
{0, 84, 12},
{-29, 178, 170},
{0, 123, 15},
{0, 127, 16},
{0, 129, 29},
{0, 132, 17},
{0, 134, 30},
{0, 144, 34},
{-55, 127, 16},
{0, 20, 4},
{-23, 78, 6},
{0, 161, 0},
{-27, 95, 6},
{0, 113, 33},
{-29, 178, 0},
{-29, 112, 23},
{-29, 111, 19},
{0, 124, 28},
{0, 179, 174},
{0, 179, 175},
{0, 179, 0},
{0, 162, 0},
{0, 76, 1},
{-29, 161, 172},
{-29, 178, 173},
{0, 113, 33},
{0, 132, 17},
{0, 20, 4},
{0, 161, 0},
{-29, 178, 0},
{0, 179, 0},
{0, 179, 0},
{0, -21, 36},
{0, 176, 0},
{0, 82, 2},
{0, 83, 2},
{0, 34, 4},
{0, 40, 2},
{0, 41, 2},
{0, 177, 0},
{-44, 97, 24},
{0, 171, 0},
{0, 163, 0},
{0, 114, 4},
{0, 115, 20},
{0, 116, 20},
{0, 119, 14},
{0, 30, 2},
{0, 85, 21},
{0, 86, 21},
{0, 31, 4},
{0, 32, 4},
{0, 121, 4},
{0, 122, 14},
{0, 42, 2},
{0, 125, 4},
{0, 126, 14},
{0, 128, 20},
{0, 46, 2},
{0, 130, 4},
{0, 131, 14},
{0, 88, 21},
{0, 133, 20},
{0, 47, 2},
{0, 135, 4},
{0, 136, 14},
{0, 138, 9},
{0, 139, 31},
{0, 49, 2},
{0, 140, 4},
{0, 50, 2},
{0, 51, 2},
{0, 143, 32},
{0, 52, 2},
{0, 53, 25},
{0, 54, 2},
{0, 145, 14},
{0, 146, 14},
{0, 147, 4},
{0, 148, 4},
{0, 56, 2},
{0, 57, 2},
{0, 89, 36},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-59, 0, 0},
{-61, 0, 0},
{-3, 0, 0},
{-5, 0, 0},
{-63, 185, 150},
{-8, 0, 0},
{-67, 0, 0},
{-9, 0, 0},
{-69, 0, 0},
{-11, 0, 0},
{-14, -103, 18},
{-70, 0, 0},
{-72, 0, 0},
{-73, 0, 0},
{-15, 0, 0},
{-92, 0, 0},
{-75, 0, 0},
{-93, 0, 0},
{-100, 0, 0},
{-1, -60, 8},
{-63, 154, 0},
{-4, -64, 25},
{-66, 0, 0},
{-68, -103, 18},
{-104, -12, 18},
{-105, -13, 18},
{-71, 0, 0},
{-106, -107, 35},
{-108, -107, 35},
{-16, -107, 35},
{-109, 0, 0},
{-58, -99, 1},
{-63, 185, 151},
{-14, -103, 18},
{-74, 0, 0},
{-19, 0, 0},
{-2, 0, 0},
{-90, 167, 0},
{-91, -103, 18},
{-17, 0, 0},
{-18, -107, 35},
};
static struct SXP_item vector[]={
{12, -6},
{12, -10},
{0, 0},
{11, -62},
{3, 141},
{13, -62},
{7, 24},
{8, 25},
{9, 26},
{2, 81},
{2, 33},
{0, 0},
{3, 98},
{2, 35},
{0, 0},
{0, 0},
{2, 22},
{3, 94},
{3, 87},
{14, -63},
{0, 0},
{16, -63},
{22, 137},
{22, 142},
{0, 0},
{25, 48},
{10, 117},
{11, 118},
{0, 0},
{19, 111},
{0, 0},
{22, 45},
{0, 0},
{23, 112},
{22, 36},
{22, 39},
{13, -7},
{22, 43},
{0, 0},
{0, 0},
{0, 0},
{18, -65},
{25, 37},
{26, 38},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{29, -101},
{30, -102},
};

static  SXBA_ELT SXPBA_kt2
#if SXBITS_PER_LONG==32
 [4] = {93, 0X8FFFFEFE, 0XC331B8DF, 0X1FEFFFFF, }
#else
 [3] = {93, 0XC331B8DF8FFFFEFE, 0X000000001FEFFFFF, }
#endif
/* End SXPBA_kt2 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt2[0],
};
static SXINT P_lrgl[]={
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
&P_lrgl[72],
&P_lrgl[80],
};
static SXINT P_right_ctxt_head[]={0,
2,1,1,2,2,1,1,1,1,2,1,};
static SXINT P_param_ref[]={
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
static struct SXP_local_mess SXP_local_mess[]={{0,NULL,NULL},
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
1,2};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%d warnings and %d errors are reported.";
extern SXBOOLEAN sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no);

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,4,5,6,5,5,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,5,3,7,8,9,10,3,3,3,3,3,3,11,12,13,
3,14,14,14,14,14,14,14,14,15,15,16,17,18,19,3,3,3,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,3,21,3,3,22,3,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
20,23,3,24,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,};
static SXSTMI SXS_transition[]={0,
0x0,0x4024,0x0,0x0,0x2402,0x2402,0x9003,0x2c04,0x2c05,0x2c06,0x4403,0x2c07,
0x2408,0x9005,0x9005,0x440e,0x4402,0x2409,0x4414,0x2c0a,0x0,0x0,0x9401,0x9402,

0x0,0x4000,0x0,0x0,0x1402,0x1402,0x4000,0x4000,0x4000,0x4000,0x4000,0x2c07,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x0,0x0,0x4000,0x4000,

0x0,0x0,0x1c03,0x1c03,0x1c03,0x0,0x4c04,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x2c0d,0x1c03,0x1c03,0x1c03,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x2c0e,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x4c0a,0x4c0a,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x4c0b,0x4c0b,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c0f,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x4418,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x9407,0x0,0x0,0x0,0x0,0x0,

0x0,0x0,0x0,0x9409,0x9008,0x9008,0x9008,0x0,0x9008,0x9008,0x0,0x9008,
0x0,0x9008,0x9008,0x9008,0x9008,0x9008,0x9008,0x1c0a,0x0,0x2c10,0x0,0x0,

0x0,0x0,0x0,0x0,0x900a,0x900a,0x0,0x0,0x0,0x0,0x0,0x900a,
0x0,0x1c0b,0x1c0b,0x900a,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,

0x0,0x0,0x1c0c,0x1c0c,0x1c0c,0x0,0x4c19,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,
0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x2c11,0x1c0c,0x1c0c,0x1c0c,

0x0,0x0,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,
0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,0x2c03,

0x0,0x0,0x0,0x0,0x401a,0x401a,0x0,0x0,0x0,0x0,0x401a,0x401a,
0x401a,0x1c0e,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x401a,

0x0,0x0,0x1c0f,0x1c0f,0x1c0f,0x2c02,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,
0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,

0x0,0x0,0x0,0x2413,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c0a,0x0,0x0,0x0,0x0,

0x0,0x0,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,
0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x240a,0x0,0x0,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2410,0x0,0x0,

};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[24],
&SXS_transition[48],
&SXS_transition[72],
&SXS_transition[96],
&SXS_transition[120],
&SXS_transition[144],
&SXS_transition[168],
&SXS_transition[192],
&SXS_transition[216],
&SXS_transition[240],
&SXS_transition[264],
&SXS_transition[288],
&SXS_transition[312],
&SXS_transition[336],
&SXS_transition[360],
&SXS_transition[384],
&SXS_transition[408],
&SXS_transition[432],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x4015,3,0,1,0},
{0x4016,4,0,1,0},
{0x2c0c,2,0,1,1},
{0x2c03,0,0,1,1},
{0x2c0b,2,1,1,1},
{0x4c09,0,0,1,1},
{0x401f,3,1,1,0},
{0x3000,7,0,1,0},
{0x2012,1,0,0,0},
{0x4020,4,1,1,0},
};
static char *S_adrp[]={0,
"TITLES",
";",
",",
"%STRING",
"SCANNER",
"LOCAL",
"X",
"S",
"%DIGIT",
"%S_PARAM",
"%C_PARAM",
"GLOBAL",
"KEYWORD",
":",
"EOL",
"EOF",
"HALT",
"PARSER",
"DONT_DELETE",
"=",
"{",
"}",
"DONT_INSERT",
"..",
"%C_STRING",
"%OCTAL_CODE",
"FORCED_INSERTION",
"KEY_TERMINALS",
"VALIDATION_LENGTH",
"FOLLOWERS_NUMBER",
"<=",
"%NUMBER",
"DETECTION",
"RESTARTING",
"ABSTRACT",
"End Of File",
};
static SXINT S_is_a_keyword[]={
20,1,5,6,7,8,12,13,15,16,17,18,19,23,27,28,29,30,33,34,35};
static SXINT S_is_a_generic_terminal[]={
7,4,9,10,11,25,26,32};
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
extern SXINT SCANACT(SXINT what, SXINT act_no);
extern SXINT sxscan_it(void);
extern SXBOOLEAN sxsrecovery (SXINT sxsrecovery_what, SXINT state_no, unsigned char *class);
static SXINT check_keyword(char *init_string, SXINT init_length);
static struct SXPP_schema SXPP_schema[]={
{6,0},{1,2},{5,0},{6,0},{1,2},{5,0},{6,0},{1,2},{5,0},{6,0},{1,1},{8,0},
{6,1},{1,1},{4,1},{6,0},{6,2},{8,0},{6,0},{6,3},{1,1},{5,0},{6,0},{8,0},
{6,0},{8,0},{6,5},{6,0},{1,2},{4,1},{6,0},{8,0},{1,1},{4,1},{6,0},{8,0},
{8,0},{6,6},{1,1},{4,2},{6,0},{1,2},{4,2},{6,0},{8,0},{6,0},{1,1},{5,0},
{6,0},{8,0},{6,0},{7,31},{6,2},{3,1},{6,0},{6,2},{8,0},{6,0},{6,0},{8,0},
{6,7},{3,1},{8,0},{6,8},{3,1},{8,0},{6,0},{3,1},{8,0},{6,0},{3,1},{6,0},
{8,0},{6,12},{1,1},{4,2},{6,0},{1,1},{4,2},{6,0},{1,1},{4,2},{6,0},{1,1},
{4,2},{6,0},{1,1},{4,2},{6,0},{8,0},{6,13},{7,31},{6,14},{3,1},{6,0},{6,2},
{8,0},{6,15},{7,31},{6,14},{3,1},{6,0},{6,2},{8,0},{8,1},{6,16},{7,31},{6,14},
{3,1},{6,0},{6,2},{8,0},{6,17},{7,31},{6,14},{3,1},{6,0},{6,2},{8,0},{6,18},
{6,0},{1,2},{4,1},{6,0},{1,2},{4,1},{6,0},{8,0},{6,0},{6,2},{1,1},{5,0},
{6,0},{6,2},{8,0},{6,0},{6,2},{6,0},{8,0},{6,19},{3,1},{6,20},{3,1},{6,21},
{6,0},{6,22},{8,0},{6,19},{3,1},{6,20},{3,1},{6,21},{6,22},{8,0},{6,23},{3,1},
{6,20},{3,1},{6,21},{6,0},{6,22},{8,0},{6,23},{3,1},{6,20},{3,1},{6,21},{6,22},
{8,0},{6,0},{6,3},{3,1},{6,0},{8,0},{6,0},{6,24},{6,0},{8,0},{6,27},{1,1},
{4,2},{6,0},{3,1},{6,2},{8,0},{6,12},{1,1},{4,2},{6,0},{1,1},{4,2},{6,0},
{1,1},{4,2},{6,0},{1,1},{4,2},{6,0},{1,1},{4,2},{6,0},{1,1},{4,2},{6,0},
{1,1},{4,2},{6,0},{8,0},{6,28},{3,1},{6,20},{3,1},{6,21},{6,0},{6,22},{6,2},
{8,0},{6,28},{3,1},{6,20},{3,1},{6,21},{6,22},{6,2},{8,0},{6,29},{3,1},{6,20},
{3,1},{6,0},{6,2},{8,0},{6,30},{3,1},{6,31},{3,1},{6,0},{7,31},{6,14},{3,1},
{6,0},{6,2},{8,0},{6,33},{7,31},{6,14},{3,1},{6,0},{6,2},{8,0},{6,34},{7,31},
{6,14},{3,1},{6,0},{6,2},{8,0},{6,35},{1,1},{4,1},{6,0},{6,2},{8,0},};

static struct SXPP_schema *PP_tables[]={NULL,
&SXPP_schema[0],
&SXPP_schema[12],
&SXPP_schema[18],
&SXPP_schema[24],
&SXPP_schema[24],
&SXPP_schema[26],
&SXPP_schema[32],
&SXPP_schema[36],
&SXPP_schema[37],
&SXPP_schema[45],
&SXPP_schema[24],
&SXPP_schema[50],
&SXPP_schema[57],
&SXPP_schema[24],
&SXPP_schema[60],
&SXPP_schema[63],
&SXPP_schema[66],
&SXPP_schema[69],
&SXPP_schema[24],
&SXPP_schema[57],
&SXPP_schema[69],
&SXPP_schema[69],
&SXPP_schema[73],
&SXPP_schema[90],
&SXPP_schema[97],
&SXPP_schema[24],
&SXPP_schema[104],
&SXPP_schema[105],
&SXPP_schema[112],
&SXPP_schema[119],
&SXPP_schema[32],
&SXPP_schema[36],
&SXPP_schema[37],
&SXPP_schema[128],
&SXPP_schema[128],
&SXPP_schema[135],
&SXPP_schema[135],
&SXPP_schema[57],
&SXPP_schema[139],
&SXPP_schema[147],
&SXPP_schema[154],
&SXPP_schema[162],
&SXPP_schema[24],
&SXPP_schema[169],
&SXPP_schema[24],
&SXPP_schema[169],
&SXPP_schema[24],
&SXPP_schema[24],
&SXPP_schema[174],
&SXPP_schema[24],
&SXPP_schema[24],
&SXPP_schema[36],
&SXPP_schema[178],
&SXPP_schema[185],
&SXPP_schema[208],
&SXPP_schema[217],
&SXPP_schema[24],
&SXPP_schema[169],
&SXPP_schema[225],
&SXPP_schema[232],
&SXPP_schema[243],
&SXPP_schema[250],
&SXPP_schema[24],
&SXPP_schema[104],
&SXPP_schema[257],
};
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxatcpp(SXINT what, ...);

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(PARSER_T) sxparser}, {255, 36, 1, 3, 4, 24, 2, 19, 1, 0, 0, 
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
(SCANACT_T) SCANACT,
(RECOVERY_T) sxsrecovery,
check_keyword,
},
{19, 57, 57, 89, 98, 149, 162, 215, 36, 41, 65, 65, 92, 42, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions,
t_bases,
nt_bases,
vector-162,
SXPBM_trans,
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
(SEMACT_T) sxatcpp},
err_titles,
abstract,
(sxsem_tables*)PP_tables,
NULL,
};

#include	"sxdico.h"

#define KW_NB		20
#define INIT_BASE	1
#define INIT_SUFFIX	0
#define CLASS_MASK	31
#define STOP_MASK	32
#define BASE_SHIFT	7
#define SUFFIX_MASK	64

static SXINT kw_code2t_code [21] = {0,
35, 34, 33, 30, 29, 28, 27, 23, 19, 18, 17, 16, 15, 13, 12, 
8, 7, 6, 5, 1, 
};

static SXINT kw_code2lgth [21] = {0,
8, 10, 9, 16, 17, 13, 16, 11, 11, 6, 4, 3, 3, 7, 6, 
1, 1, 5, 7, 6, 
};

static SXUINT comb_vector [41] = {
0, 0, 161, 1986, 2115, 1826, 2597, 2246, 2343, 
296, 1961, 1450, 459, 3084, 1325, 812, 687, 2224, 1187, 
420, 1061, 932, 1574, 1703, 2080, 552, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 2478, 
0, 0, 
};

static unsigned char char2class [256] = {
'\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\001', '\000', '\016', '\002', '\003', '\006', 
'\t', '\n', '\000', '\000', '\013', '\007', '\000', '\004', 
'\000', '\r', '\000', '\b', '\f', '\005', '\000', '\017', 
'\000', '\020', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', 
};

#include	"sxcheck_keyword.h"

/* End of sxtables for pprecor [Thu Mar  6 12:56:21 2008] */
