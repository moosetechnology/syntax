/* Beginning of sxtables for lecl [Thu May 19 11:12:48 1994] */
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={
{6, 5, 0, -246},
{6, 22, 0, -248},
{2, 4, 2, -246},
{0, 10, 0, 330},
{0, 12, 0, 331},
{0, 14, 0, 332},
{6, 39, 0, -185},
{2, 21, 2, -248},
{0, 118, 0, -188},
{1, 23, 2, 336},
{0, 103, 0, 372},
{0, 105, 0, 373},
{0, 113, 0, 374},
{0, 114, 0, 374},
{1, 24, 2, 336},
{0, 31, 0, 342},
{0, 34, 0, 343},
{2, 38, 1, -185},
{1, 1, 4, -245},
{6, 153, 0, -261},
{0, 154, 0, 384},
{0, 155, 0, 384},
{0, 156, 0, 384},
{1, 8, 2, 330},
{1, 9, 2, 330},
{1, 15, 2, -6},
{1, 104, 1, 373},
{1, 117, 1, -188},
{1, 102, 1, 372},
{0, 134, 0, -31},
{1, 115, 1, -13},
{0, 133, 0, -31},
{1, 35, 1, 343},
{1, 30, 1, 342},
{0, 60, 0, 357},
{0, 62, 0, 357},
{0, 63, 0, 357},
{0, 49, 0, -39},
{1, 42, 2, -253},
{0, 50, 0, -39},
{6, 77, 0, -269},
{6, 89, 0, 367},
{6, 53, 0, -192},
{2, 146, 2, 381},
{6, 147, 2, 381},
{2, 152, 2, -261},
{1, 84, 2, -37},
{1, 90, 1, 368},
{1, 64, 2, -35},
{0, 66, 0, -51},
{1, 65, 2, -35},
{6, 72, 0, -277},
{2, 52, 1, -192},
{2, 76, 2, -269},
{2, 88, 2, 367},
{1, 91, 2, 368},
{1, 87, 3, -37},
{1, 56, 2, -273},
{0, 162, 0, 389},
{6, 161, 0, -280},
{6, 71, 2, -277},
{2, 70, 2, -277},
{2, 160, 2, -280},
{1, 2, 0, -182},
{5, 7, 0, -247},
{0, 0, 2, -245},
{1, 19, 0, -183},
{5, 25, 0, -249},
{5, 26, 0, -250},
{1, 36, 0, -184},
{0, 3, 3, -182},
{5, 16, 0, 333},
{5, 17, 0, 333},
{5, 18, 0, 333},
{5, 45, 0, -254},
{5, 44, 0, -254},
{5, 43, 0, -254},
{1, 46, 0, -254},
{1, 47, 0, -254},
{1, 48, 0, -254},
{1, 142, 0, -186},
{1, 6, 3, 328},
{0, 13, 1, 332},
{0, 20, 3, -183},
{5, 119, 0, -14},
{1, 120, 0, -14},
{1, 121, 0, -14},
{1, 122, 0, -14},
{1, 123, 0, -14},
{1, 124, 0, -14},
{1, 130, 0, -14},
{1, 131, 0, -14},
{5, 33, 0, 343},
{5, 137, 0, 344},
{1, 138, 0, 344},
{1, 139, 0, 344},
{0, 37, 2, -184},
{5, 148, 0, 382},
{5, 149, 0, 382},
{1, 150, 0, -19},
{0, 11, 2, 331},
{1, 99, 1, 338},
{0, 101, 1, 371},
{5, 132, 0, -31},
{1, 135, 0, -32},
{1, 136, 0, -32},
{0, 116, 1, -13},
{1, 27, 1, -15},
{0, 29, 1, 341},
{1, 81, 0, 348},
{5, 61, 0, 357},
{6, 145, 1, -258},
{1, 107, 2, 374},
{1, 32, 2, 343},
{5, 95, 0, -191},
{5, 97, 0, -191},
{5, 96, 0, -191},
{1, 93, 0, -191},
{1, 92, 0, -191},
{1, 94, 0, -191},
{1, 40, 3, 346},
{1, 41, 3, 346},
{6, 59, 1, 349},
{5, 57, 0, -274},
{0, 143, 3, -186},
{0, 151, 2, -19},
{1, 112, 3, 374},
{1, 110, 3, 374},
{1, 106, 3, 374},
{1, 111, 3, 374},
{1, 109, 3, 374},
{1, 108, 3, 374},
{5, 125, 3, -14},
{5, 126, 3, -14},
{5, 127, 3, -14},
{5, 128, 3, -14},
{1, 129, 3, -14},
{1, 100, 3, 371},
{5, 140, 3, 344},
{5, 141, 3, 344},
{1, 28, 3, 341},
{0, 75, 2, -36},
{1, 82, 2, -37},
{1, 69, 0, 361},
{1, 68, 0, 361},
{5, 73, 0, 364},
{5, 74, 0, 364},
{2, 58, 2, 349},
{0, 51, 2, -40},
{2, 144, 3, -258},
{1, 78, 2, 366},
{1, 79, 2, 366},
{1, 80, 2, 366},
{5, 98, 0, -56},
{1, 85, 3, -37},
{1, 69, 1, 361},
{0, 67, 1, -51},
{1, 163, 0, 389},
{5, 158, 4, -22},
{5, 159, 4, -23},
{1, 83, 4, -37},
{1, 54, 3, 354},
{1, 55, 3, 354},
{5, 164, 1, 389},
{0, 157, 5, -21},
{1, 86, 5, -37},
};
static struct SXP_bases t_bases[]={
{-67, 194, 12},
{-70, 197, 16},
{-81, 201, 60},
{-97, 391, 0},
{-100, 289, 61},
{-103, 424, 385},
{-107, 432, 390},
{-109, 432, 387},
{-122, 453, 378},
{-110, 217, 34},
{0, 414, 0},
{0, 65, 4},
{0, 392, 0},
{-71, 65, 4},
{0, 415, 379},
{0, 391, 0},
{-84, 392, 0},
{0, 424, 385},
{0, 432, 387},
{0, 402, 0},
{0, 415, 0},
{0, 424, 0},
{0, 432, 0},
{-110, 453, 380},
{0, 424, 386},
{0, 398, 0},
{0, 360, 396},
{0, 124, 4},
{-125, 402, 0},
{-126, 289, 61},
{-110, 216, 27},
{0, 360, 397},
{-145, 144, 25},
{-145, 383, 395},
{0, 360, 0},
{0, 154, 4},
{-110, 242, 27},
{-110, 453, 380},
{0, 415, 384},
{0, 383, 0},
{-64, 193, 1},
{-138, 424, 385},
{-141, 432, 387},
{-163, 453, 377},
{0, 415, 379},
{0, 415, 379},
{0, 415, 379},
{0, 424, 385},
{0, 415, 0},
{0, 424, 385},
{0, 424, 385},
{0, 432, 387},
{0, 415, 0},
{0, 424, 385},
{0, 432, 387},
{0, 402, 0},
{0, 402, 0},
{0, 398, 0},
{0, 360, 0},
{0, 424, 385},
{0, 360, 0},
{0, 383, 0},
{0, 415, 384},
{0, -66, 69},
{0, 195, 2},
{0, 196, 3},
{0, 198, 2},
{0, 199, 3},
{0, 200, 3},
{-82, 393, 0},
{-83, 202, 9},
{0, 205, 2},
{0, 206, 3},
{0, 393, 381},
{-102, 235, 14},
{-108, 236, 14},
{0, 210, 2},
{-112, 237, 26},
{0, 238, 3},
{0, 211, 2},
{0, 399, 382},
{0, 399, 388},
{0, 399, 394},
{0, 409, 0},
{-121, -170, 4},
{0, 123, 2},
{0, 137, 8},
{-142, 239, 26},
{-143, 240, 26},
{0, -175, 4},
{0, 148, 2},
{0, 219, 2},
{0, 241, 23},
{-150, 237, 26},
{-155, 240, 26},
{-157, 243, 26},
{-161, 240, 26},
{-162, -177, 4},
{-165, 244, 26},
{-166, 240, 26},
{0, 290, 7},
{0, 291, 7},
{0, 292, 7},
{0, 293, 7},
{0, 234, 7},
{0, 294, 7},
{0, 295, 7},
{0, 389, 0},
{0, 303, 46},
{0, 304, 46},
{0, 305, 46},
{0, 306, 46},
{0, 307, 46},
{0, 308, 46},
{0, 214, 4},
{0, 213, 32},
{0, 311, 63},
{0, 312, 64},
{0, 313, 64},
{-129, 410, 0},
{-132, 411, 0},
{0, 133, 8},
{0, 134, 8},
{0, 135, 8},
{0, 136, 8},
{0, 139, 8},
{0, 140, 8},
{0, 314, 30},
{0, 315, 30},
{0, 220, 64},
{0, 317, 46},
{0, 318, 46},
{0, 416, 0},
{0, 153, 29},
{-156, 221, 27},
{0, 159, 66},
{0, 160, 66},
{0, 164, 68},
{0, 222, 69},
};
static struct SXP_bases nt_bases[]={
{-183, 0, 0},
{-184, 0, 0},
{-186, 0, 0},
{-18, 406, 358},
{-23, 463, 436},
{-29, 494, 326},
{-32, 478, 0},
{-34, -17, 24},
{-272, 333, 473},
{-48, 0, 0},
{-53, 479, 446},
{-247, 527, 0},
{-250, 344, 422},
{-3, -247, 9},
{-6, 511, 0},
{-254, 406, 348},
{-8, 344, 421},
{-188, 494, 442},
{-17, 340, 497},
{-260, 470, 0},
{-26, 0, 0},
{-27, 352, 441},
{-33, 0, 0},
{-37, 333, 339},
{-40, 494, 452},
{-41, -269, 45},
{-191, 321, 357},
{-274, 479, 429},
{-275, -260, 62},
{-46, 463, 330},
{-47, 0, 0},
{-276, 321, 365},
{-49, 0, 0},
{-52, 484, 0},
{-278, 321, 365},
{-56, 0, 0},
{-57, 0, 0},
{-225, 333, 418},
{-60, 352, 373},
{-61, 0, 0},
{-182, -245, 1},
{-29, 494, 326},
{-34, -17, 24},
{-272, 333, 473},
{-255, 352, 508},
{-24, 352, 480},
{-25, 352, 480},
{-262, 494, 458},
{-28, 352, 0},
{-263, 494, 458},
{-264, 494, 458},
{-265, 340, 433},
{-268, 0, 0},
{-223, 494, 459},
{-224, 509, 0},
{-44, 0, 0},
{-45, 0, 0},
{-54, 0, 0},
{-55, -191, 49},
{-58, 494, 427},
{-281, 321, 365},
{-62, 0, 0},
{-63, 352, 363},
};
static struct SXP_item vector[]={
{17, -249},
{22, -189},
{23, -16},
{4, 115},
{13, -252},
{14, -6},
{28, -266},
{29, -190},
{48, -42},
{10, 116},
{38, -35},
{39, -36},
{25, -185},
{26, -7},
{12, -59},
{37, -267},
{17, 117},
{18, 118},
{19, 119},
{20, 120},
{4, -179},
{4, -172},
{57, -188},
{4, -167},
{12, -59},
{7, 226},
{4, 146},
{30, -254},
{8, 101},
{8, 301},
{6, 203},
{6, 203},
{10, 147},
{7, 233},
{4, 75},
{4, 68},
{67, -23},
{5, 227},
{6, 228},
{13, 93},
{10, 76},
{15, 204},
{13, 104},
{47, -270},
{13, 69},
{4, 98},
{21, 209},
{17, 77},
{18, 78},
{19, 79},
{20, 80},
{10, 99},
{14, 235},
{49, -191},
{5, 127},
{5, 130},
{8, 114},
{4, 124},
{4, 72},
{25, 316},
{25, 212},
{25, 218},
{14, 236},
{36, 113},
{10, 73},
{11, 74},
{29, 309},
{4, 72},
{31, 310},
{46, 319},
{7, 229},
{38, 302},
{27, -253},
{10, 73},
{11, 74},
{22, 149},
{15, -248},
{16, -2},
{15, 230},
{19, -250},
{68, -280},
{6, -19},
{53, 105},
{54, 106},
{29, 151},
{28, -279},
{31, 152},
{24, 85},
{39, 128},
{39, 131},
{62, 298},
{68, 158},
{12, -9},
{65, 299},
{37, -267},
{67, 300},
{24, -17},
{40, -37},
{35, 231},
{18, -10},
{37, 232},
{33, -192},
{34, -43},
{40, 86},
{41, 87},
{42, 88},
{43, 89},
{44, 90},
{45, 282},
{18, -38},
{47, 283},
{48, 284},
{49, 285},
{50, 286},
{51, 91},
{52, 92},
{24, 111},
{51, -256},
{52, -187},
{53, -11},
{28, 207},
{36, -274},
{31, -39},
{57, -188},
{32, 208},
{33, 297},
{55, 94},
{56, 95},
{57, 96},
{58, 287},
{59, 288},
{12, -5},
{51, -256},
{52, -187},
{53, -11},
{55, -13},
{56, -14},
{57, -188},
{63, -261},
{64, -20},
{28, -271},
{24, -30},
{51, -256},
{52, -187},
{53, -11},
{12, -9},
{13, -252},
{14, -6},
{57, -188},
{52, -187},
{53, -11},
{53, -11},
{40, -37},
{35, -273},
{57, -188},
{57, -188},
{20, -15},
{21, -257},
{11, -4},
{12, -5},
{10, -251},
{11, -4},
{12, -5},
{13, -252},
{41, -50},
{42, -51},
{43, -277},
{65, -21},
{66, -22},
{60, -258},
{61, -259},
{23, -16},
{24, -17},
{7, -246},
{8, -1},
{58, -31},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{54, -12},
{55, -13},
{56, -14},
};
static struct SXP_prdct prdcts[]={
{2, -110},
{1, 296},
{-1, -110},
{2, 215},
{-1, -121},
{2, -110},
{1, 296},
{-1, -122},
{2, 215},
{-1, 0},
{2, 215},
{-1, -162},
{2, -110},
{1, 296},
{-1, -163},
};
static SXBA_ELT SXPBA_kt2[]={
140,319896126,4672,406921994,0,2048,};
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
1,2};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%d warnings and %d errors are reported.";
extern SXPARSACT_FUNCTION PARSACT;

static unsigned char S_char_to_simple_class[]={
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,3,3,3,3,3,3,3,3,4,5,6,5,5,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,5,3,7,8,3,9,10,3,11,12,13,14,15,16,17,
3,18,19,19,19,19,19,19,19,20,20,21,22,3,23,24,3,25,26,26,26,26,26,26,26,
26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,27,28,29,30,31,3,
26,32,26,26,26,33,26,26,26,26,26,26,26,34,26,26,26,35,26,36,26,37,26,26,26,
26,38,39,40,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,};
static SXSTMI SXS_transition[]={
0x0,0x4045,0x0,0x0,0x2402,0x2402,0x2c03,0x2c04,0x2c05,0x2c06,0x4407,0x4408,
0x4427,0x4405,0x441a,0x2407,0x2408,0x2c09,0x2c09,0x2c09,0x4417,0x4402,0x4403,0x441e,
0x2c0a,0x2c0b,0x4423,0x0,0x4424,0x440f,0x0,0x2c0b,0x2c0b,0x2c0b,0x2c0b,0x2c0b,
0x2c0b,0x4425,0x440e,0x4426,
0x0,0x4000,0x0,0x0,0x1402,0x1402,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x0,0x4000,0x4000,0x0,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,
0x0,0x0,0x2c0c,0x2c0c,0x2c0c,0x0,0x0,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,
0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,
0x2c0c,0x2c0c,0x2c0c,0x240d,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,
0x2c0c,0x2c0c,0x2c0c,0x2c0c,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x2c0e,0x2c0e,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x2c0f,0x0,0x0,0x0,0x0,0x0,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x2c0f,0x0,0x0,0x0,
0x0,0x0,0x0,0x2410,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x7011,0x2c12,0x2c12,0x0,0x0,0x0,0x0,
0x0,0x2c13,0x0,0x0,0x0,0x0,0x0,0x2c13,0x2c13,0x2c13,0x2c13,0x2c13,
0x2c13,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x4006,0x4006,0x4006,0x4006,0x0,0x0,0x4006,0x0,
0x0,0x0,0x0,0x2414,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x4006,0x4006,0x4006,0x0,0x0,0x4006,0x0,0x4006,0x4006,0x4006,0x4006,0x4006,
0x4006,0x4006,0x0,0x0,
0x0,0x0,0x0,0x0,0x4022,0x4022,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x4022,0x4409,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x4022,0x0,0x0,0x0,0x0,0x0,0x4022,0x4022,0x4022,0x4022,0x4022,
0x4022,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x402e,0x402e,0x0,0x0,0x0,0x0,0x0,0x402e,
0x0,0x0,0x0,0x402e,0x0,0x1c09,0x1c09,0x1c09,0x0,0x0,0x0,0x0,
0x0,0x402e,0x0,0x0,0x0,0x0,0x0,0x402e,0x402e,0x402e,0x402e,0x402e,
0x402e,0x0,0x0,0x0,
0x0,0x0,0x0,0x2415,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x7016,0x2c17,0x2c17,0x0,0x0,0x0,0x0,
0x0,0x2c18,0x0,0x0,0x0,0x0,0x0,0x2c18,0x2c18,0x2c18,0x2c18,0x2c18,
0x2c18,0x0,0x0,0x0,
0x0,0x0,0x0,0x9404,0x9003,0x9003,0x9003,0x9003,0x9003,0x9003,0x9003,0x9003,
0x0,0x9003,0x9003,0x9003,0x9003,0x2c19,0x2c19,0x2c19,0x9003,0x9003,0x9003,0x9003,
0x9003,0x2c19,0x9003,0x0,0x9003,0x9003,0x2c1a,0x2c19,0x2c19,0x2c19,0x2c19,0x2c19,
0x2c19,0x9003,0x9003,0x9003,
0x0,0x0,0x1c0c,0x1c0c,0x1c0c,0x0,0x4c0a,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,
0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,
0x1c0c,0x1c0c,0x1c0c,0x240d,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,0x1c0c,
0x1c0c,0x1c0c,0x1c0c,0x1c0c,
0x0,0x0,0x2c0c,0x2c0c,0x2c0c,0x240c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,
0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x9005,0x9005,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,
0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x2c0c,0x9406,0x9407,0x9408,0x9409,0x940a,
0x940b,0x2c0c,0x2c0c,0x2c0c,
0x0,0x0,0x0,0x0,0x400b,0x400b,0x400b,0x400b,0x0,0x400b,0x400b,0x400b,
0x0,0x400b,0x400b,0x400b,0x400b,0x1c0e,0x1c0e,0x0,0x0,0x400b,0x0,0x0,
0x400b,0x400b,0x400b,0x0,0x400b,0x400b,0x0,0x400b,0x400b,0x400b,0x400b,0x400b,
0x400b,0x400b,0x400b,0x400b,
0x0,0x0,0x0,0x0,0x900c,0x900c,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x900c,0x900c,0x900c,0x1c0f,0x1c0f,0x1c0f,0x0,0x900c,0x900c,0x0,
0x0,0x1c0f,0x0,0x0,0x0,0x0,0x2c1b,0x1c0f,0x1c0f,0x1c0f,0x1c0f,0x1c0f,
0x1c0f,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x241c,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x1,0x0,0x0,0x0,0x4c37,0x4c37,0x4c37,0x4c37,0x0,0x4c37,0x4c37,0x4c37,
0x0,0x0,0x0,0x4c37,0x0,0x9001,0x9001,0x9001,0x0,0x4c37,0x0,0x0,
0x4c37,0x4c37,0x4c37,0x0,0x4c37,0x4c37,0x0,0x4c37,0x4c37,0x4c37,0x4c37,0x4c37,
0x4c37,0x4c37,0x4c37,0x4c37,
0x0,0x0,0x0,0x0,0x4037,0x4037,0x4037,0x4037,0x0,0x4037,0x4037,0x4037,
0x0,0x0,0x0,0x4037,0x0,0x1c12,0x1c12,0x1c12,0x0,0x4037,0x0,0x0,
0x4037,0x4037,0x4037,0x0,0x4037,0x4037,0x0,0x4037,0x4037,0x4037,0x4037,0x4037,
0x4037,0x4037,0x4037,0x4037,
0x0,0x0,0x0,0x0,0x900d,0x900d,0x900d,0x900d,0x0,0x900d,0x900d,0x900d,
0x0,0x0,0x0,0x900d,0x0,0x1c13,0x1c13,0x1c13,0x0,0x900d,0x900d,0x0,
0x900d,0x1c13,0x900d,0x0,0x900d,0x900d,0x2c1d,0x1c13,0x1c13,0x1c13,0x1c13,0x1c13,
0x1c13,0x900d,0x900d,0x900d,
0x0,0x0,0x1414,0x1414,0x1414,0x4400,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,
0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,
0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,0x1414,
0x1414,0x1414,0x1414,0x1414,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x241e,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x1,0x0,0x0,0x0,0x4c18,0x4c18,0x4c18,0x4c18,0x0,0x0,0x4c18,0x4c18,
0x0,0x0,0x0,0x4c18,0x0,0x9002,0x9002,0x9002,0x0,0x4c18,0x0,0x0,
0x4c18,0x4c18,0x4c18,0x0,0x4c18,0x4c18,0x0,0x4c18,0x4c18,0x4c18,0x4c18,0x4c18,
0x4c18,0x4c18,0x4c18,0x4c18,
0x0,0x0,0x0,0x0,0x4018,0x4018,0x4018,0x4018,0x0,0x0,0x4018,0x4018,
0x0,0x0,0x0,0x4018,0x0,0x1c17,0x1c17,0x1c17,0x0,0x4018,0x0,0x0,
0x4018,0x4018,0x4018,0x0,0x4018,0x4018,0x0,0x4018,0x4018,0x4018,0x4018,0x4018,
0x4018,0x4018,0x4018,0x4018,
0x0,0x0,0x0,0x940f,0x900e,0x900e,0x900e,0x900e,0x0,0x0,0x900e,0x900e,
0x0,0x0,0x0,0x900e,0x0,0x0,0x0,0x0,0x0,0x900e,0x0,0x0,
0x900e,0x1c18,0x900e,0x0,0x900e,0x900e,0x2c1f,0x1c18,0x1c18,0x1c18,0x1c18,0x1c18,
0x1c18,0x900e,0x900e,0x900e,
0x0,0x0,0x0,0x0,0x9003,0x9003,0x9003,0x9003,0x9003,0x9003,0x9003,0x9003,
0x0,0x9003,0x9003,0x9003,0x9003,0x1c19,0x1c19,0x1c19,0x9003,0x9003,0x9003,0x9003,
0x9003,0x1c19,0x9003,0x0,0x9003,0x9003,0x2c1a,0x1c19,0x1c19,0x1c19,0x1c19,0x1c19,
0x1c19,0x9003,0x9003,0x9003,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x2c19,0x2c19,0x2c19,0x0,0x0,0x0,0x0,
0x0,0x2c19,0x0,0x0,0x0,0x0,0x0,0x2c19,0x2c19,0x2c19,0x2c19,0x2c19,
0x2c19,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x2c0f,0x2c0f,0x2c0f,0x0,0x0,0x0,0x0,
0x0,0x2c0f,0x0,0x0,0x0,0x0,0x0,0x2c0f,0x2c0f,0x2c0f,0x2c0f,0x2c0f,
0x2c0f,0x0,0x0,0x0,
0x0,0x0,0x0,0x2410,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x2c22,0x0,0x0,0x0,0x0,0x0,0x2c22,0x2c22,0x2c22,0x2c22,0x2c22,
0x2c22,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x2c13,0x2c13,0x2c13,0x0,0x0,0x0,0x0,
0x0,0x2c13,0x0,0x0,0x0,0x0,0x0,0x2c13,0x2c13,0x2c13,0x2c13,0x2c13,
0x2c13,0x0,0x0,0x0,
0x0,0x0,0x0,0x2415,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x2c18,0x0,0x0,0x0,0x0,0x0,0x2c18,0x2c18,0x2c18,0x2c18,0x2c18,
0x2c18,0x0,0x0,0x0,
0x0,0x0,0x0,0x2425,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x2c18,0x0,0x0,0x0,0x0,0x0,0x2c18,0x2c18,0x2c18,0x2c18,0x2c18,
0x2c18,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x2426,0x0,0x0,0x0,0x0,0x0,0x2426,0x2426,0x2426,0x2426,0x2426,
0x2426,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x1c21,0x9010,0x9010,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x9411,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x1c23,0x9012,0x9012,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x2418,0x0,0x0,0x0,0x0,0x0,0x2418,0x2418,0x2418,0x2418,0x2418,
0x2418,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x241f,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x9404,0x900e,0x900e,0x900e,0x900e,0x900e,0x900e,0x0,0x0,
0x0,0x0,0x900e,0x900e,0x900e,0x900e,0x900e,0x900e,0x0,0x900e,0x900e,0x900e,
0x0,0x2c28,0x0,0x0,0x0,0x0,0x0,0x2c28,0x2c28,0x2c28,0x2c28,0x2c28,
0x2c28,0x0,0x0,0x0,
0x0,0x0,0x9013,0x9013,0x9013,0x0,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,
0x9013,0x9013,0x9013,0x9013,0x9013,0x2c29,0x2c29,0x9013,0x9013,0x9013,0x9013,0x9013,
0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,
0x9013,0x9013,0x9013,0x9013,
0x0,0x0,0x0,0x9404,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x9013,0x9013,0x9013,0x0,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,
0x9013,0x9013,0x9013,0x9013,0x9013,0x9c13,0x9c13,0x9013,0x9013,0x9013,0x9013,0x9013,
0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,0x9013,
0x9013,0x9013,0x9013,0x9013,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x242b,0x0,0x0,0x0,0x0,0x0,0x242b,0x242b,0x242b,0x242b,0x242b,
0x242b,0x0,0x0,0x0,
0x0,0x0,0x0,0x9411,0x900e,0x900e,0x900e,0x900e,0x0,0x900e,0x900e,0x900e,
0x0,0x0,0x0,0x900e,0x0,0x0,0x0,0x0,0x0,0x900e,0x0,0x0,
0x900e,0x2c22,0x900e,0x0,0x900e,0x900e,0x2c2c,0x2c22,0x2c22,0x2c22,0x2c22,0x2c22,
0x2c22,0x900e,0x900e,0x900e,
0x0,0x0,0x0,0x242d,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x242e,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x242d,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x2c22,0x0,0x0,0x0,0x0,0x0,0x2c22,0x2c22,0x2c22,0x2c22,0x2c22,
0x2c22,0x0,0x0,0x0,
};
static SXSTMI *S_transition_matrix[]={
&SXS_transition[0]-1,
&SXS_transition[40]-1,
&SXS_transition[80]-1,
&SXS_transition[120]-1,
&SXS_transition[160]-1,
&SXS_transition[200]-1,
&SXS_transition[240]-1,
&SXS_transition[280]-1,
&SXS_transition[320]-1,
&SXS_transition[360]-1,
&SXS_transition[400]-1,
&SXS_transition[440]-1,
&SXS_transition[480]-1,
&SXS_transition[520]-1,
&SXS_transition[560]-1,
&SXS_transition[600]-1,
&SXS_transition[640]-1,
&SXS_transition[680]-1,
&SXS_transition[720]-1,
&SXS_transition[760]-1,
&SXS_transition[800]-1,
&SXS_transition[840]-1,
&SXS_transition[880]-1,
&SXS_transition[920]-1,
&SXS_transition[960]-1,
&SXS_transition[1000]-1,
&SXS_transition[1040]-1,
&SXS_transition[1080]-1,
&SXS_transition[1120]-1,
&SXS_transition[1160]-1,
&SXS_transition[1200]-1,
&SXS_transition[1240]-1,
&SXS_transition[1280]-1,
&SXS_transition[1320]-1,
&SXS_transition[1360]-1,
&SXS_transition[1400]-1,
&SXS_transition[1440]-1,
&SXS_transition[1480]-1,
&SXS_transition[1520]-1,
&SXS_transition[1560]-1,
&SXS_transition[1600]-1,
&SXS_transition[1640]-1,
&SXS_transition[1680]-1,
&SXS_transition[1720]-1,
&SXS_transition[1760]-1,
&SXS_transition[1800]-1,
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={
{11297,10,0,1,0},
{11299,10,0,1,0},
{12292,7,0,1,0},
{8224,1,0,0,0},
{11303,10,0,1,0},
{8204,9,8,1,0},
{8204,9,12,1,0},
{8204,9,10,1,0},
{8204,9,13,1,0},
{8204,9,9,1,0},
{8204,9,11,1,0},
{16401,7,0,1,0},
{12301,7,0,1,0},
{12288,7,0,1,0},
{8228,1,0,0,0},
{11282,5,0,1,0},
{8234,1,0,0,0},
{11287,5,0,1,0},
{8204,2,0,0,0},
};
static char *S_adrp[]={
"CLASSES",
";",
"=",
"%IDENTIFIER",
"+",
"-",
"(",
")",
"..",
"%STRING_LITERAL",
"%OCTAL_CODE",
"ABBREVIATIONS",
"%PREDICATE_NAME",
"|",
"^",
"TOKENS",
"%GENERIC_NAME",
"EOF",
"COMMENTS",
"INCLUDE",
"UNION",
"END",
":",
"%ACTION_NO",
"ALL",
",",
"BUT",
"PRIORITY",
"REDUCE",
">",
"SHIFT",
"CONTEXT",
"UNBOUNDED",
".",
"[",
"]",
"{",
"}",
"*",
"@UPPER_CASE",
"@LOWER_CASE",
"@LOWER_TO_UPPER",
"@UPPER_TO_LOWER",
"@ERASE",
"@SET",
"%INTEGER_NUMBER",
"@RESET",
"@INCR",
"@DECR",
"@PUT",
"@MARK",
"@RELEASE",
"&TRUE",
"&FALSE",
"%PREDICATE_NO",
"&IS_FIRST_COL",
"&IS_LAST_COL",
"&IS_SET",
"&IS_RESET",
"SYNONYMS",
"FOR",
"INTERNAL",
"CODE",
"USE",
"BYTE",
"BITS",
"WORD",
"UNUSED",
"End Of File",
"END_OF_FILE",
};
static short S_syno[]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,70,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static short S_is_a_keyword[]={
42,1,12,16,18,19,20,21,22,25,27,28,29,31,32,33,40,41,42,43,44,45,47,48,49,50,51,52,53,54,56,57,58,59,60,61,62,63,64,65,66,67,68};
static short S_is_a_generic_terminal[]={
8,4,10,11,13,17,24,46,55};
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
extern SXSCANACT_FUNCTION SCANACT;
static struct SXT_node_info SXT_node_info[]={
{67,1},{93,6},{0,6},{0,7},{12,9},{10,10},{11,12},{58,13},{52,15},{0,17},{0,18},{0,19},
{0,20},{0,21},{80,22},{33,24},{81,25},{56,26},{93,27},{0,27},{0,28},{5,30},{2,31},{2,33},
{4,35},{3,36},{61,37},{63,38},{0,40},{60,41},{0,43},{31,44},{64,45},{0,46},{62,47},{93,48},
{0,48},{0,49},{87,51},{84,52},{84,54},{85,56},{48,58},{48,59},{48,60},{29,61},{14,61},{36,61},
{0,61},{0,62},{0,63},{0,64},{16,66},{15,67},{15,69},{17,71},{18,73},{0,74},{28,76},{0,77},
{59,78},{0,79},{0,80},{46,81},{46,84},{0,87},{0,88},{93,89},{93,89},{0,89},{54,91},{45,92},
{47,93},{47,94},{0,95},{0,96},{66,98},{69,99},{70,99},{79,99},{93,99},{21,99},{76,100},{76,101},
{89,102},{90,103},{90,104},{0,105},{86,107},{20,108},{20,110},{22,112},{23,112},{24,112},{25,112},{25,113},
{25,114},{19,115},{72,116},{8,117},{0,119},{77,120},{0,122},{30,123},{0,124},{31,125},{57,126},{71,127},
{71,128},{71,129},{88,130},{88,131},{0,132},{0,133},{32,134},{0,136},{53,137},{0,138},{7,139},{91,140},
{49,140},{50,140},{92,140},{6,140},{78,140},{75,141},{37,142},{26,143},{68,144},{51,145},{73,145},{64,145},
{0,146},{0,147},{44,148},{39,148},{65,148},{40,149},{41,149},{43,149},{42,150},{93,151},{0,151},{0,152},
{83,154},{0,155},{27,157},{35,159},{82,160},{93,161},{0,161},{0,162},{74,164},{0,165},{0,166},{0,167},
{0,168},{9,169},{94,170},{0,171},{13,173},{0,174},{55,175},{38,175},{0,176},};
static short T_ter_to_node_name[]={
0,0,0,0,34,};
static char *T_node_name[]={
"ERROR",
"ABBREVIATION",
"ABBREVIATION_PRDCT_NAME",
"ABBREVIATION_RE_NAME",
"ABBREVIATION_S",
"ACTION_ERASE",
"ACTION_NO",
"ALTERNATIVE",
"BYTE_LENGTH",
"CLASS",
"CLASS_NAME",
"CLASS_S",
"COLLATING_S",
"COMMENTS",
"COMPONENT",
"COMPONENTS_S",
"COMPONENT_DEF",
"COMPONENT_NAME_DEF",
"COMPONENT_NAME_REF",
"COMPONENT_REF",
"CONTEXT",
"CONTEXT_COMMENTS",
"CONTEXT_EOF",
"CONTEXT_INCLUDE",
"CONTEXT_NAME",
"DECR",
"DENOTATION_S",
"ENVIRONMENT_S",
"EOF",
"ERASE",
"EXPRESSION",
"EXTENDED_CLASS_REF",
"ID",
"IDENTIFIER",
"ID_DENOTATION",
"INCLUDE",
"INCR",
"INTEGER_NUMBER",
"IS_FALSE",
"IS_FIRST_COL",
"IS_LAST_COL",
"IS_RESET",
"IS_SET",
"IS_TRUE",
"KEYWORDS",
"KEYWORDS_SPEC",
"KEYWORD_NAME",
"LEXICAL_UNIT_NAME",
"LOWER_CASE",
"LOWER_TO_UPPER",
"MARK",
"MINUS",
"NOT",
"NOT_KEYWORDS",
"NO_SPECIFIED",
"OCTAL_CODE",
"OPTION",
"PLUS",
"POST_ACTION",
"PRDCT_ET",
"PRDCT_EXPRESSION",
"PRDCT_NOT",
"PRDCT_OR",
"PREDICATE_NAME",
"PREDICATE_NO",
"PRIORITY_KIND_S",
"PROGRAM_ROOT_LECL",
"PUT",
"REDUCE_REDUCE",
"REDUCE_SHIFT",
"REF_TRANS_CLOSURE",
"REGULAR_EXPRESSION",
"RELEASE",
"REPR_SPEC_S",
"RESET",
"RESTRICTED_CONTEXT",
"SEQUENCE",
"SET",
"SHIFT_REDUCE",
"SLICE",
"STRING",
"STRING_DENOTATION",
"SYNONYM_S",
"TOKEN",
"TOKEN_DEF",
"TOKEN_REF_S",
"TOKEN_S",
"TRANS_CLOSURE",
"UNBOUNDED_CONTEXT",
"UNBOUNDED_RESTRICTED_CONTEXT",
"UPPER_CASE",
"UPPER_TO_LOWER",
"VOID",
"WORD_LENGTH",
};
extern SXSEMPASS_FUNCTION SEMPASS;
static char T_stack_schema[]={
0,1,2,3,4,1,0,2,0,0,2,0,0,2,0,2,0,1,0,0,0,0,2,0,0,
0,1,0,2,0,0,2,0,2,0,0,0,0,2,0,0,1,0,1,0,0,0,0,0,1,
0,0,2,0,2,0,2,0,0,0,0,0,1,0,1,0,0,2,0,2,0,2,0,1,2,
1,0,0,0,0,0,1,2,0,1,2,0,0,0,2,0,0,0,0,0,0,2,0,0,0,
0,0,0,0,0,2,0,0,1,0,2,0,0,0,0,0,0,2,0,0,1,0,0,0,1,
1,1,2,2,2,2,0,0,0,1,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,
1,0,2,0,0,2,0,2,0,0,1,0,2,0,0,0,0,0,1,1,0,2,0,0,1,
0,};

static struct SXT_tables SXT_tables={
SXT_node_info-1, T_ter_to_node_name, T_stack_schema-1, SEMPASS, T_node_name-1
};
extern SXSEMACT_FUNCTION sxatc;
static SXCHECK_KEYWORD_FUNCTION sxcheck_keyword;

SXTABLES sxtables={
52113, /* magic */
sxscanner
sxparser,
{255, 69, 1, 3, 4, 40, 0, 46, 1, 1, 0, 
S_is_a_keyword,S_is_a_generic_terminal,S_transition_matrix-1,
SXS_action_or_prdct_code-1,
S_adrp-1,
S_syno-1,
SXS_local_mess-1,
S_char_to_simple_class+128,
S_no_delete,S_no_insert,
S_global_mess-1,
S_lregle-1,
SCANACT,
sxsrecovery,
sxcheck_keyword
},
{63, 166, 181, 222, 244, 320, 361, 550, 69, 69, 164, 164, 139, 64, 0, 11, 4, 7, 2, 5, 11, 2, 6,
reductions-1,
t_bases-1,
nt_bases-1,
vector-361,
#if SXBITS_PER_LONG == 32
SXPBM_trans-1,
#else
NULL,
#endif
prdcts-167,
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
sxprecovery,
PARSACT,
NULL,
sxatc},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables
};
/* ********************************************************************
   *  This program has been generated from lecl.lecl                  *
   *  on Thu May 19 11:09:50 1994                                     *
   *  by the SYNTAX (*) lexical constructor LECL                      *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */



static SXINT sxcheck_keyword (char *string, SXINT length)
{
   register int  t_code, delta;
   register char *keyword;
   register int t_syno;

   switch (length) {
   case 3:
      switch (string [2]) {
      case 'D':
         return *string++ == 'E' && *string == 'N' ? 22 /* "END" */: 0;

      case 'E':
         return *string++ == 'U' && *string == 'S' ? 64 /* "USE" */: 0;

      case 'F':
         return *string++ == 'E' && *string == 'O' ? 18 /* "EOF" */: 0;

      case 'L':
         return *string++ == 'A' && *string == 'L' ? 25 /* "ALL" */: 0;

      case 'R':
         return *string++ == 'F' && *string == 'O' ? 61 /* "FOR" */: 0;

      case 'T':
         return *string++ == 'B' && *string == 'U' ? 27 /* "BUT" */: 0;

      default:
         return 0;
      }

   case 4:
      switch (*string++) {
      case '@':
         switch (*string++) {
         case 'P':
            return *string++ == 'U' && *string == 'T' ? 50 /* "@PUT" */: 0;

         case 'S':
            return *string++ == 'E' && *string == 'T' ? 45 /* "@SET" */: 0;

         default:
            return 0;
         }

      case 'B':
         switch (*string++) {
         case 'I':
            return *string++ == 'T' && *string == 'S' ? 66 /* "BITS" */: 0;

         case 'Y':
            return *string++ == 'T' && *string == 'E' ? 65 /* "BYTE" */: 0;

         default:
            return 0;
         }

      case 'C':
         t_code = 63 /* "CODE" */;
         goto cmp_1_1;

      case 'W':
         t_code = 67 /* "WORD" */;
         goto cmp_1_1;

      default:
         return 0;
      }

   case 5:
      switch (string [4]) {
      case 'E':
         t_code = 53 /* "&TRUE" */;
         goto cmp_1_0;

      case 'K':
         t_code = 51 /* "@MARK" */;
         goto cmp_1_0;

      case 'N':
         t_code = 21 /* "UNION" */;
         goto cmp_1_0;

      case 'R':
         switch (string [1]) {
         case 'D':
            t_code = 49 /* "@DECR" */;
            goto cmp_1_0;

         case 'I':
            t_code = 48 /* "@INCR" */;
            goto cmp_1_0;

         default:
            return 0;
         }

      case 'T':
         t_code = 31 /* "SHIFT" */;
         goto cmp_1_0;

      default:
         return 0;
      }

   case 6:
      switch (*string++) {
      case '&':
         t_code = 54 /* "&FALSE" */;
         goto cmp_1_1;

      case '@':
         switch (*string++) {
         case 'E':
            t_code = 44 /* "@ERASE" */;
            goto cmp_2_2;

         case 'R':
            t_code = 47 /* "@RESET" */;
            goto cmp_2_2;

         default:
            return 0;
         }

      case 'R':
         t_code = 29 /* "REDUCE" */;
         goto cmp_1_1;

      case 'T':
         t_code = 16 /* "TOKENS" */;
         goto cmp_1_1;

      case 'U':
         t_code = 68 /* "UNUSED" */;
         goto cmp_1_1;

      default:
         return 0;
      }

   case 7:
      switch (*string++) {
      case '&':
         t_code = 58 /* "&IS_SET" */;
         goto cmp_1_1;

      case 'C':
         switch (*string++) {
         case 'L':
            t_code = 1 /* "CLASSES" */;
            goto cmp_2_2;

         case 'O':
            t_code = 32 /* "CONTEXT" */;
            goto cmp_2_2;

         default:
            return 0;
         }

      case 'I':
         t_code = 20 /* "INCLUDE" */;
         goto cmp_1_1;

      default:
         return 0;
      }

   case 8:
      switch (*string++) {
      case '@':
         t_code = 52 /* "@RELEASE" */;
         goto cmp_1_1;

      case 'C':
         t_code = 19 /* "COMMENTS" */;
         goto cmp_1_1;

      case 'I':
         t_code = 62 /* "INTERNAL" */;
         goto cmp_1_1;

      case 'P':
         t_code = 28 /* "PRIORITY" */;
         goto cmp_1_1;

      case 'S':
         t_code = 60 /* "SYNONYMS" */;
         goto cmp_1_1;

      default:
         return 0;
      }

   case 9:
      switch (*string++) {
      case '&':
         t_code = 59 /* "&IS_RESET" */;
         goto cmp_1_1;

      case 'U':
         t_code = 33 /* "UNBOUNDED" */;
         goto cmp_1_1;

      default:
         return 0;
      }

   case 11:
      switch (*string++) {
      case '@':
         switch (*string++) {
         case 'L':
            t_code = 41 /* "@LOWER_CASE" */;
            goto cmp_2_2;

         case 'U':
            t_code = 40 /* "@UPPER_CASE" */;
            goto cmp_2_2;

         default:
            return 0;
         }

      case 'E':
         t_code = 18 /* "EOF" */;
         t_syno = 70 /* "END_OF_FILE" */;
         goto cmp_syno_1_1;

      default:
         return 0;
      }

   case 12:
      t_code = 57 /* "&IS_LAST_COL" */;
      goto cmp_0_0;

   case 13:
      switch (*string++) {
      case '&':
         t_code = 56 /* "&IS_FIRST_COL" */;
         goto cmp_1_1;

      case 'A':
         t_code = 12 /* "ABBREVIATIONS" */;
         goto cmp_1_1;

      default:
         return 0;
      }

   case 15:
      switch (string [1]) {
      case 'L':
         t_code = 42 /* "@LOWER_TO_UPPER" */;
         goto cmp_0_0;

      case 'U':
         t_code = 43 /* "@UPPER_TO_LOWER" */;
         goto cmp_0_0;

      default:
         return 0;
      }

   default:
      return 0;
   }

cmp_0_0:
   delta = 0;
   goto compare;

cmp_2_2:
   length -= 2;
   delta = 2;
   goto compare;

cmp_1_0:
   length -= 1;
   delta = 0;
   goto compare;

cmp_1_1:
   length -= 1;
   delta = 1;
compare:
   keyword = sxtables.SXS_tables.S_adrp [t_code] + delta;

   do {
      if (*string++ != *keyword++)
          return 0;
   } while (--length > 0) ;

   return t_code;

cmp_syno_1_1:
   length -= 1;
   delta = 1;
compare_syno:
   keyword = sxtables.SXS_tables.S_adrp [t_syno] + delta;

   do {
      if (*string++ != *keyword++)
          return 0;
   } while (--length > 0) ;

   return t_code;

}

/* End of sxtables for lecl [Thu May 19 11:12:49 1994] */
