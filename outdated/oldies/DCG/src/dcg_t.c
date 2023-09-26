/* Beginning of sxtables for dcg [Thu Feb 27 10:12:38 1997] */
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={
{6, 3, 0, -66},
{1, 27, 1, 128},
{2, 2, 1, -66},
{1, 5, 1, -99},
{0, 38, 0, 139},
{6, 36, 0, 141},
{0, 56, 0, 147},
{0, 39, 0, 139},
{0, 52, 0, 145},
{0, 54, 0, 146},
{0, 14, 0, 132},
{0, 13, 0, 132},
{6, 11, 0, 131},
{0, 15, 0, 132},
{1, 6, 2, -99},
{1, 53, 1, 146},
{6, 19, 0, -107},
{0, 28, 0, 135},
{6, 22, 0, -108},
{0, 23, 0, 137},
{0, 25, 0, 137},
{1, 60, 1, 148},
{2, 35, 2, 141},
{1, 49, 2, 145},
{1, 50, 2, 145},
{1, 51, 2, 145},
{2, 10, 2, 131},
{1, 59, 2, 148},
{1, 61, 2, 148},
{2, 18, 2, -107},
{1, 24, 2, 137},
{2, 21, 2, -108},
{1, 63, 0, 129},
{0, 0, 2, -98},
{1, 1, 1, -98},
{5, 33, 0, 142},
{5, 40, 0, 139},
{5, 31, 0, 142},
{5, 32, 0, 142},
{6, 12, 0, 131},
{1, 57, 1, 144},
{5, 41, 1, 139},
{1, 26, 3, 128},
{0, 37, 1, 139},
{0, 48, 1, 143},
{1, 17, 1, 130},
{5, 30, 0, 135},
{5, 29, 0, 135},
{1, 4, 3, 125},
{6, 9, 1, -103},
{0, 58, 2, 144},
{0, 55, 2, 147},
{0, 16, 2, 130},
{5, 33, 1, 142},
{0, 20, 2, -14},
{1, 62, 2, 148},
{1, 34, 3, 138},
{1, 46, 3, 143},
{1, 47, 3, 143},
{2, 7, 3, -103},
{2, 8, 3, -103},
{1, 42, 3, -21},
{1, 43, 3, -21},
{1, 44, 3, -21},
{1, 45, 3, -21},
};
static struct SXP_bases t_bases[]={
{-35, 69, 12},
{-33, 72, 3},
{-33, 158, 0},
{-33, 70, 13},
{0, 157, 170},
{-40, 182, 0},
{0, 76, 6},
{0, 157, 180},
{0, 157, 177},
{0, 157, 0},
{0, 181, 189},
{0, 181, 195},
{0, 182, 0},
{0, 196, 0},
{0, 181, 169},
{0, 69, 12},
{0, 157, 170},
{0, 157, 170},
{0, 157, 177},
{0, 157, 177},
{0, 157, 177},
{0, 157, 177},
{0, 157, 177},
{-40, 182, 0},
{-40, 182, 0},
{0, 157, 170},
{0, 157, 170},
{0, 181, 195},
{0, 157, 177},
{0, 157, 177},
{0, 157, 177},
{0, 157, 177},
{0, -34, 27},
{0, 71, 1},
{0, 162, 199},
{-44, 197, 0},
{-45, 190, 0},
{0, 173, 0},
{-50, 78, 3},
{0, 51, 7},
{0, 197, 175},
{0, 161, 0},
{0, 160, 0},
{0, 197, 191},
{0, 162, 166},
{-58, 190, 0},
{-59, 190, 0},
{-60, 78, 3},
{-61, 78, 3},
{-62, 197, 0},
{-63, 197, 0},
{-64, 197, 0},
{-65, 197, 0},
{-42, 82, 13},
{-54, 92, 11},
{0, 82, 13},
{0, 81, 27},
};
static struct SXP_bases nt_bases[]={
{-3, 146, 134},
{-4, 0, 0},
{-22, 0, 0},
{-2, 0, 0},
{-10, 201, 137},
{-14, 216, 139},
{-15, 0, 0},
{-105, 203, 130},
{-106, 203, 0},
{-16, -7, 20},
{-18, 123, 0},
{-109, 203, 127},
{-27, 216, 144},
{-29, 0, 0},
{-30, -18, 20},
{-67, 146, 136},
{-110, 203, 213},
{-23, 203, 150},
{-111, 201, 132},
{-112, 201, 132},
{-24, 219, 0},
{-25, 219, 0},
{-26, 219, 0},
{-113, 216, 227},
{-114, 216, 227},
{-28, 203, 130},
{-31, 203, 150},
{-32, 203, 215},
{-115, 203, 0},
{-116, 203, 0},
{-117, 203, 0},
{-118, 203, 0},
};
static struct SXP_item vector[]={
{12, -107},
{13, -17},
{1, -98},
{2, -66},
{3, -1},
{6, -67},
{14, -108},
{15, -19},
{16, -20},
{5, -103},
{18, -21},
{16, -5},
{17, -68},
{9, -104},
{10, -13},
{4, -99},
{21, -101},
{22, -8},
{16, -5},
{17, -6},
{11, -14},
{19, -100},
{25, -10},
{21, -101},
{22, -8},
{23, -102},
{3, 91},
{4, 79},
{3, 93},
{3, 80},
{3, 83},
{16, -5},
{10, 36},
{7, 53},
{9, 55},
{13, 74},
{21, -101},
{22, -8},
{16, 38},
{17, 39},
{2, 49},
{6, 73},
{4, 89},
{5, 90},
{10, 36},
{14, 57},
{12, 47},
{12, 119},
{0, 0},
{15, 48},
{15, 37},
{6, 73},
{7, 41},
{6, 76},
{14, 52},
{8, 77},
{0, 0},
{12, 119},
{23, 75},
{12, 69},
{15, 37},
{7, 46},
{16, 38},
{17, 39},
{10, 36},
{23, 75},
{12, 47},
{6, 73},
{23, 75},
{15, 48},
{10, 120},
{10, 56},
{12, 121},
{13, 74},
{18, 94},
{19, 95},
{20, 96},
{21, 97},
{14, 43},
{24, 86},
{25, 87},
{26, 88},
{0, 0},
{23, 75},
{22, 84},
{23, 85},
{20, -7},
{6, -11},
{20, -7},
{8, -12},
{24, -9},
{23, -102},
{24, -9},
{25, -10},
{16, -5},
{17, -6},
{16, -20},
{0, 0},
{18, -21},
{21, -101},
{22, -8},
{21, -109},
{10, -13},
{11, -14},
{20, -7},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{25, -10},
};
static SXBA_ELT SXPBA_kt2[]={
58,65602,33554500,};
static SXBA_ELT SXPBA_kt4[]={
58,520581498,63121612,};
static SXBA SXPBM_trans [] = {
&SXPBA_kt2[0],
&SXPBA_kt4[0],
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
2,2,4};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%d warnings and %d errors are reported.";
extern SXBOOLEAN sxprecovery();

static unsigned char S_char_to_simple_class[]={
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,3,3,3,3,3,3,3,3,3,4,5,4,4,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,6,7,8,3,9,8,10,11,12,13,8,14,15,16,
17,18,18,18,18,18,18,18,18,18,18,8,19,8,8,8,8,8,20,20,20,20,21,20,20,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,22,3,23,8,24,8,
25,25,25,25,26,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,27,28,29,8,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,};
static SXSTMI SXS_transition[]={
0x0,0x401b,0x0,0x2c02,0x2c02,0x9c01,0x2c03,0x2c04,0x2c05,0x2406,0x9402,0x9403,
0x2c04,0x4403,0x2c04,0x9009,0x2c07,0x2c08,0x900b,0x2c09,0x2c09,0x9404,0x9405,0x2c09,
0x2c0a,0x2c0a,0x9406,0x4404,0x9407,
0x0,0x900d,0x0,0x1c02,0x1c02,0x900d,0x900d,0x900d,0x2c05,0x900d,0x900d,0x900d,
0x900d,0x900d,0x900d,0x900d,0x2c0b,0x900d,0x900d,0x900d,0x900d,0x900d,0x900d,0x900d,
0x900d,0x900d,0x900d,0x900d,0x900d,
0x0,0x0,0x1c03,0x1c03,0x1c03,0x1c03,0x2c0c,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x0,0x0,0x0,0x9001,0x9001,0x9001,0x9001,0x1c04,0x9001,0x9001,0x9001,0x9001,
0x1c04,0x9001,0x1c04,0x900e,0x2c0d,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,
0x9001,0x9001,0x9001,0x9001,0x9001,
0x0,0x0,0x1c05,0x1c05,0x2c02,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,
0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,
0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,
0x0,0x0,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x240e,0x1c06,0x1c06,
0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,
0x1c06,0x1c06,0x1c06,0x1c06,0x1c06,
0x0,0x0,0x0,0x9001,0x9001,0x9001,0x9001,0x2c04,0x9001,0x9001,0x9001,0x9001,
0x2c0f,0x9001,0x2c04,0x900e,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,
0x9001,0x9001,0x9001,0x9001,0x9001,
0x0,0x0,0x0,0x4010,0x4010,0x4010,0x0,0x4010,0x4010,0x2c10,0x0,0x4010,
0x4010,0x4010,0x4010,0x2c11,0x4010,0x2c12,0x4010,0x0,0x0,0x0,0x4010,0x0,
0x4010,0x4010,0x0,0x4010,0x4010,
0x0,0x0,0x0,0x400a,0x400a,0x400a,0x0,0x400a,0x400a,0x400a,0x0,0x400a,
0x400a,0x400a,0x400a,0x400a,0x400a,0x1c09,0x400a,0x1c09,0x1c09,0x0,0x400a,0x1c09,
0x1c09,0x1c09,0x0,0x400a,0x400a,
0x0,0x0,0x0,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,
0x9001,0x9001,0x9001,0x9001,0x9001,0x1c0a,0x9001,0x1c0a,0x1c0a,0x9001,0x9001,0x1c0a,
0x1c0a,0x1c0a,0x9001,0x9001,0x9001,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x2c0f,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x400f,0x400f,0x0,0x2c03,0x0,0x400f,0x0,0x0,0x400f,
0x0,0x400f,0x0,0x0,0x400f,0x0,0x0,0x0,0x0,0x0,0x400f,0x0,
0x0,0x0,0x0,0x400f,0x400f,
0x0,0x0,0x0,0x9001,0x9001,0x9001,0x9001,0x2c04,0x9001,0x9001,0x9001,0x9001,
0x9001,0x9001,0x2c04,0x900e,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,
0x9001,0x9001,0x9001,0x9001,0x9001,
0x0,0x0,0x0,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x2c06,0x9001,0x9001,
0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,0x9001,
0x9001,0x9001,0x9001,0x9001,0x9001,
0x0,0x0,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,
0x2c13,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,
0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,
0x0,0x0,0x0,0x4010,0x4010,0x4010,0x0,0x4010,0x4010,0x4010,0x0,0x4010,
0x4010,0x4010,0x4010,0x4010,0x4010,0x1c10,0x4010,0x1c10,0x1c10,0x0,0x4010,0x0,
0x1c10,0x1c10,0x0,0x4010,0x4010,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x2c14,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x4010,0x4010,0x4010,0x0,0x4010,0x4010,0x2c10,0x0,0x4010,
0x4010,0x4010,0x4010,0x2c11,0x4010,0x2c15,0x4010,0x0,0x0,0x0,0x4010,0x0,
0x4010,0x4010,0x0,0x4010,0x4010,
0x0,0x0,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x1c13,0x2c0f,0x2c0f,0x2c0f,0x2c02,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x0,0x0,0x0,0x4011,0x4011,0x4011,0x0,0x4011,0x4011,0x4011,0x0,0x4011,
0x4011,0x4011,0x4011,0x4011,0x4011,0x1c14,0x4011,0x0,0x2c16,0x0,0x4011,0x0,
0x4011,0x2c16,0x0,0x4011,0x4011,
0x0,0x0,0x0,0x4010,0x4010,0x4010,0x0,0x4010,0x4010,0x4010,0x0,0x4010,
0x4010,0x4010,0x4010,0x2c11,0x4010,0x1c15,0x4010,0x0,0x0,0x0,0x4010,0x0,
0x4010,0x4010,0x0,0x4010,0x4010,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x2c17,0x0,0x0,0x2c18,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x2c18,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x4011,0x4011,0x4011,0x0,0x4011,0x4011,0x4011,0x0,0x4011,
0x4011,0x4011,0x4011,0x4011,0x4011,0x1c18,0x4011,0x0,0x0,0x0,0x4011,0x0,
0x4011,0x4011,0x0,0x4011,0x4011,
};
static SXSTMI *S_transition_matrix[]={
&SXS_transition[0]-1,
&SXS_transition[29]-1,
&SXS_transition[58]-1,
&SXS_transition[87]-1,
&SXS_transition[116]-1,
&SXS_transition[145]-1,
&SXS_transition[174]-1,
&SXS_transition[203]-1,
&SXS_transition[232]-1,
&SXS_transition[261]-1,
&SXS_transition[290]-1,
&SXS_transition[319]-1,
&SXS_transition[348]-1,
&SXS_transition[377]-1,
&SXS_transition[406]-1,
&SXS_transition[435]-1,
&SXS_transition[464]-1,
&SXS_transition[493]-1,
&SXS_transition[522]-1,
&SXS_transition[551]-1,
&SXS_transition[580]-1,
&SXS_transition[609]-1,
&SXS_transition[638]-1,
&SXS_transition[667]-1,
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={
{20492,1,0,0,0},
{16397,12,1,1,0},
{16398,13,1,1,0},
{16390,12,0,1,0},
{16391,13,0,1,0},
{16392,3,2,1,0},
{16393,4,2,1,0},
{36880,4,0,1,0},
{39944,3,1,1,1},
{11268,0,0,1,1},
{17413,3,1,1,1},
{39937,0,0,1,1},
{16384,5,0,1,0},
{36865,3,1,1,1},
{11268,0,0,1,1},
{16386,4,2,1,0},
};
static char *S_adrp[]={
"-->",
".",
",",
"|",
";",
"[",
"]",
"{",
"}",
"%variable",
"is",
"%atom",
"(",
")",
"%string",
"%integer_number",
"%floating_point_number",
"<",
"=<",
">",
">=",
"+",
"-",
"*",
"/",
"mod",
"End Of File",
};
static short S_is_a_keyword[]={
11,1,11,18,19,20,21,22,23,24,25,26};
static short S_is_a_generic_terminal[]={
5,10,12,15,16,17};
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
extern int SCANACT();
extern int sxscan_it();
extern SXBOOLEAN sxsrecovery();
static int check_keyword();
static struct SXT_node_info SXT_node_info[]={
{5,1},{0,2},{21,4},{20,5},{11,7},{11,9},{0,11},{0,13},{3,15},{0,16},{19,18},{19,19},
{0,19},{0,20},{0,21},{0,22},{27,23},{0,23},{25,25},{0,26},{0,27},{18,29},{0,30},{8,31},
{0,33},{13,34},{13,36},{0,38},{31,39},{28,40},{30,41},{29,42},{32,43},{24,44},{0,46},{2,48},
{0,49},{0,50},{0,51},{31,52},{28,53},{9,54},{10,56},{22,58},{23,60},{17,62},{14,64},{0,66},
{16,67},{6,69},{15,71},{0,73},{26,74},{0,75},{0,76},{0,77},{7,78},{0,78},{12,79},{12,81},
{4,83},{4,85},{27,87},{0,87},};
static short T_ter_to_node_name[]={
0,0,0,0,0,0,0,0,0,0,32,0,28,};
static char *T_node_name[]={
"ERROR",
"ARGUMENT_S",
"BODY_S",
"CATENATE",
"DCG",
"DIV",
"EMPTY_LIST",
"IS",
"LESS",
"LESS_EQUAL",
"LHS",
"LIST_BODY",
"MAIN_FUNCTOR",
"MINUS",
"MOD",
"MULT",
"PLUS",
"PROLOG_S",
"RHS_ITEM_S",
"RULE",
"RULE_S",
"SUP",
"SUP_EQUAL",
"TERM",
"TERMINAL_S",
"UNARY_MINUS",
"VOID",
"atom",
"floating_point_number",
"integer_number",
"string",
"variable",
};
extern int sempass();
static char T_stack_schema[]={
0,0,1,0,1,3,0,1,0,2,0,2,0,2,0,0,2,0,0,0,0,1,0,2,0,
1,0,2,0,0,0,2,0,1,3,0,1,0,0,0,0,0,0,1,3,0,2,0,0,0,
0,0,0,0,2,0,2,0,2,0,2,0,2,0,2,0,0,2,0,2,0,2,0,0,0,
1,0,1,0,2,0,1,0,2,0,2,0,};

static struct SXT_tables SXT_tables=
{SXT_node_info-1, T_ter_to_node_name, T_stack_schema-1, sempass, T_node_name-1};
extern int sxscanner();
extern int sxparser();
extern int sxatc();

struct sxtables sxtables={
52113, /* magic */
{sxscanner,sxparser}, {255, 27, 1, 3, 4, 29, 3, 24, 1, 0, 0, 
S_is_a_keyword,S_is_a_generic_terminal,S_transition_matrix-1,
SXS_action_or_prdct_code-1,
S_adrp-1,
NULL,
SXS_local_mess-1,
S_char_to_simple_class+128,
S_no_delete,S_no_insert,
S_global_mess-1,
S_lregle-1,
SCANACT,
sxsrecovery,
check_keyword,
},
{32, 65, 65, 81, 97, 122, 135, 244, 27, 26, 63, 63, 57, 33, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions-1,
t_bases-1,
nt_bases-1,
vector-135,
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
,sxatc},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables,
NULL,
};
/* ********************************************************************
   *  This program has been generated from dcg.lecl                   *
   *  on Thu Feb 27 10:12:35 1997                                     *
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
   case 1:
      switch (*string) {
      case '*':
         return 24 /* "*" */;

      case '+':
         return 22 /* "+" */;

      case '-':
         return 23 /* "-" */;

      case '/':
         return 25 /* "/" */;

      case '<':
         return 18 /* "<" */;

      case '>':
         return 20 /* ">" */;

      default:
         return 0;
      }

   case 2:
      switch (*string++) {
      case '=':
         return *string == '<' ? 19 /* "=<" */: 0;

      case '>':
         return *string == '=' ? 21 /* ">=" */: 0;

      case 'i':
         return *string == 's' ? 11 /* "is" */: 0;

      default:
         return 0;
      }

   case 3:
      switch (*string++) {
      case '-':
         return *string++ == '-' && *string == '>' ? 1 /* "-->" */: 0;

      case 'm':
         return *string++ == 'o' && *string == 'd' ? 26 /* "mod" */: 0;

      default:
         return 0;
      }

   default:
      return 0;
   }

}

/* End of sxtables for dcg [Thu Feb 27 10:12:38 1997] */
