/* Beginning of sxtables for lfg [Thu Mar  6 12:56:14 2008] */
#define SXP_MAX 513
#include "sxunix.h"
#include "sxba.h"
static struct SXP_reductions reductions[]={{0,0,0,0},
{2, 2, 1, -144},
{6, 72, 0, -145},
{2, 71, 1, -145},
{2, 74, 1, -146},
{1, 20, 1, 304},
{2, 67, 1, 318},
{6, 68, 1, 318},
{6, 36, 0, -238},
{6, 104, 0, 315},
{6, 60, 0, 307},
{1, 86, 1, 325},
{1, 84, 1, 325},
{1, 85, 1, 325},
{1, 115, 1, 331},
{1, 125, 1, 332},
{1, 121, 1, 332},
{1, 123, 1, 332},
{2, 35, 2, -238},
{1, 37, 2, 310},
{1, 80, 2, 324},
{6, 106, 0, -250},
{1, 87, 2, 325},
{1, 116, 2, 331},
{1, 126, 2, 332},
{1, 117, 2, 331},
{1, 129, 2, 332},
{1, 122, 2, 332},
{1, 124, 2, 332},
{1, 127, 2, 332},
{1, 128, 2, 332},
{2, 59, 2, 307},
{2, 103, 2, 315},
{6, 55, 0, -253},
{2, 105, 2, -250},
{2, 138, 2, -252},
{6, 139, 2, -252},
{2, 54, 2, -253},
{6, 3, 0, -144},
{0, 0, 2, -229},
{5, 145, 0, 302},
{5, 146, 0, 302},
{1, 147, 0, 302},
{5, 70, 0, 319},
{5, 69, 0, 319},
{1, 1, 2, -229},
{5, 76, 0, -4},
{1, 4, 2, -1},
{6, 19, 1, -147},
{5, 27, 0, -5},
{1, 26, 0, -5},
{1, 5, 2, -1},
{6, 13, 1, 297},
{1, 6, 2, -1},
{1, 7, 2, -1},
{1, 8, 2, -1},
{6, 64, 1, -150},
{1, 9, 2, -1},
{6, 66, 1, -152},
{1, 11, 2, -1},
{1, 17, 1, 303},
{0, 16, 1, 303},
{6, 15, 1, -153},
{6, 75, 2, -146},
{6, 79, 0, -154},
{2, 18, 2, -147},
{5, 28, 1, -5},
{2, 12, 2, 297},
{2, 63, 2, -150},
{2, 65, 2, -152},
{5, 39, 0, 310},
{5, 40, 0, -191},
{1, 10, 3, -1},
{2, 14, 2, -153},
{1, 73, 3, 320},
{5, 77, 2, -4},
{5, 143, 0, 335},
{1, 109, 0, 333},
{1, 108, 0, 333},
{1, 30, 0, -241},
{5, 23, 3, -148},
{5, 34, 2, 300},
{1, 33, 2, 300},
{1, 42, 0, 312},
{1, 43, 0, 312},
{1, 44, 0, 312},
{5, 118, 0, 331},
{5, 118, 1, 331},
{2, 78, 2, -154},
{1, 89, 0, -159},
{1, 91, 0, -159},
{1, 90, 0, -159},
{1, 93, 0, -159},
{1, 92, 0, -159},
{0, 101, 1, 329},
{0, 102, 1, 329},
{1, 21, 4, -148},
{0, 29, 1, -241},
{0, 62, 1, 317},
{5, 41, 2, -191},
{1, 32, 3, 300},
{0, 31, 3, 300},
{0, 120, 2, 332},
{0, 83, 2, 325},
{0, 114, 2, 331},
{0, 119, 2, 332},
{1, 112, 0, 334},
{1, 111, 0, 334},
{1, 132, 1, 337},
{0, 130, 1, 336},
{1, 131, 1, 337},
{0, 133, 1, 337},
{5, 25, 5, -148},
{1, 61, 2, 317},
{5, 24, 5, -148},
{0, 46, 1, -19},
{1, 81, 3, 324},
{1, 100, 1, -22},
{1, 98, 1, -22},
{1, 99, 1, -22},
{5, 94, 1, -22},
{5, 95, 1, -22},
{0, 97, 1, -22},
{0, 134, 2, 338},
{0, 136, 2, -246},
{0, 137, 2, -246},
{1, 58, 0, -255},
{5, 53, 2, -19},
{1, 52, 2, -19},
{5, 49, 2, -19},
{1, 48, 2, -19},
{1, 22, 7, -148},
{1, 38, 5, 310},
{1, 51, 3, -19},
{0, 50, 3, -19},
{1, 47, 3, -19},
{0, 134, 3, 338},
{0, 114, 3, 331},
{0, 136, 3, -246},
{0, 119, 3, 332},
{1, 113, 4, 334},
{1, 117, 3, 331},
{0, 56, 3, -255},
{1, 57, 4, -255},
};
static struct SXP_bases t_bases[]={{0,0,0},
{0, 351, 0},
{-45, 260, 28},
{0, 357, 0},
{-47, 261, 12},
{-50, 503, 0},
{-51, 356, 0},
{-55, 361, 0},
{0, 361, 374},
{-57, 361, 0},
{-59, 356, 0},
{-74, 483, 420},
{0, 487, 0},
{-94, 499, 379},
{-95, 499, 375},
{0, 499, 425},
{0, 499, 444},
{-126, 224, 18},
{0, 499, 382},
{0, 261, 12},
{0, 356, 0},
{0, 361, 0},
{0, 167, 16},
{-64, 0, 0},
{0, 353, 388},
{-79, 499, 453},
{0, 459, 0},
{0, 479, 0},
{0, 483, 390},
{0, 499, 464},
{0, 353, 0},
{0, 483, 420},
{0, 499, 415},
{0, 499, 372},
{0, 499, 424},
{0, 368, 0},
{0, 499, 465},
{0, 467, 0},
{0, 499, 450},
{0, 499, 406},
{-142, 499, 464},
{-38, 0, 0},
{-53, 356, 0},
{-54, 356, 0},
{0, 361, 0},
{0, 361, 377},
{0, 361, 380},
{0, 361, 383},
{0, 459, 0},
{0, 499, 379},
{0, 499, 404},
{-94, 499, 400},
{-95, 499, 398},
{0, 499, 400},
{0, 499, 398},
{0, 499, 392},
{0, 499, 385},
{0, 499, 446},
{0, 499, 422},
{0, 499, 419},
{0, 499, 411},
{0, 356, 0},
{0, 356, 0},
{0, 361, 0},
{0, 356, 0},
{0, 479, 0},
{0, 483, 390},
{0, 479, 0},
{0, 479, 0},
{0, 483, 390},
{0, 499, 415},
{0, 499, 415},
{0, 499, 415},
{0, 499, 464},
{0, 356, 0},
{0, 479, 381},
{0, 479, 376},
{0, 499, 372},
{0, 467, 0},
{0, 499, 415},
{0, 499, 415},
{0, 499, 464},
{0, 167, 16},
{0, 467, 0},
{0, 499, 406},
{0, 499, 406},
{0, -39, 41},
{0, 48, 9},
{0, 52, 9},
{-61, 60, 11},
{0, 62, 9},
{0, 65, 9},
{0, 67, 9},
{0, 72, 9},
{0, 73, 9},
{0, 445, 0},
{0, 88, 9},
{-97, 443, 373},
{0, 278, 15},
{-98, 177, 38},
{0, 217, 23},
{0, 103, 25},
{-109, 108, 40},
{-111, 110, 40},
{0, 113, 25},
{-115, 177, 38},
{0, 116, 25},
{-122, 181, 38},
{0, 223, 38},
{0, 417, 0},
{0, 442, 0},
{0, 407, 0},
{0, 132, 23},
{0, 443, 393},
{-141, 355, 0},
{-143, 216, 21},
{0, 165, 8},
{0, 262, 29},
{0, 264, 8},
{0, 63, 19},
{0, 266, 12},
{0, 358, 0},
{0, 66, 20},
{0, 75, 25},
{0, 80, 17},
{0, 274, 16},
{-82, 81, 20},
{0, 275, 12},
{-87, 174, 31},
{0, 212, 24},
{0, 277, 17},
{0, 279, 17},
{0, 99, 25},
{-101, 100, 20},
{0, 112, 14},
{0, 283, 15},
{0, 114, 19},
{0, 118, 17},
{0, 120, 30},
{0, 121, 30},
{0, 286, 15},
{-128, 127, 20},
{-130, 129, 20},
{0, 131, 14},
{-134, 133, 20},
{0, 135, 20},
{-137, 462, 0},
{-139, 466, 0},
{0, 184, 41},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-146, 317, 0},
{-3, -146, 30},
{-4, 0, 0},
{-234, -148, 14},
{-5, 0, 0},
{-235, 0, 0},
{-189, -187, 28},
{-6, 0, 0},
{-190, -187, 28},
{-237, -232, 11},
{-155, 493, 509},
{-159, 0, 0},
{-245, 428, 490},
{-245, 428, 501},
{-9, 325, 0},
{-21, 493, 488},
{-255, 0, 0},
{-14, 0, 0},
{-148, 318, 0},
{-231, -149, 6},
{-187, 315, 0},
{-236, 0, 0},
{-154, 0, 0},
{-191, 309, 0},
{-9, 513, 346},
{-243, 0, 0},
{-193, -192, 40},
{-155, 292, 0},
{-9, 324, 344},
{-18, -191, 20},
{-20, 493, 480},
{-245, 428, 504},
{-251, 497, 491},
{-32, 0, 0},
{-253, -33, 9},
{-9, -254, 24},
{-161, 0, 0},
{-34, 0, 0},
{-23, 0, 0},
{-9, 513, 449},
{-144, -229, 1},
{-235, 0, 0},
{-235, 0, 0},
{-7, 0, 0},
{-6, 0, 0},
{-6, 0, 0},
{-6, 0, 0},
{-158, 0, 0},
{-245, 428, 490},
{-245, 428, 501},
{-245, 428, 490},
{-245, 428, 501},
{-245, 428, 490},
{-245, 428, 501},
{-245, 428, 490},
{-245, 428, 501},
{-245, 428, 490},
{-245, 428, 501},
{-14, 0, 0},
{-14, 0, 0},
{-185, -231, 11},
{-186, -231, 11},
{-188, 316, 0},
{-233, 340, 0},
{-11, 493, 0},
{-155, 493, 364},
{-13, 493, 0},
{-197, -196, 40},
{-155, 493, 397},
{-245, 428, 506},
{-245, 428, 508},
{-245, 428, 510},
{-9, 324, 362},
{-160, 0, 0},
{-193, -192, 40},
{-197, -196, 40},
{-251, 497, 489},
{-202, 0, 0},
{-35, 497, 0},
{-36, 497, 0},
{-9, 513, 343},
{-37, 0, 0},
{-203, 0, 0},
{-257, 0, 0},
{-25, 0, 0},
};
static struct SXP_item vector[]={
{3, -145},
{4, -1},
{7, -150},
{5, -147},
{8, -152},
{0, 0},
{34, -244},
{18, -238},
{19, -8},
{0, 0},
{0, 0},
{13, -230},
{40, -194},
{41, -195},
{9, -19},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{0, 0},
{27, -151},
{0, 0},
{28, -187},
{0, 0},
{29, -2},
{22, -19},
{24, -242},
{24, -248},
{10, -153},
{11, -232},
{1, 162},
{2, 163},
{3, 204},
{4, 205},
{5, 164},
{6, 206},
{7, 259},
{16, -256},
{7, 70},
{10, 207},
{16, -240},
{17, -241},
{9, -140},
{12, 71},
{9, 166},
{11, 40},
{12, 41},
{12, 46},
{26, -247},
{13, 168},
{11, 43},
{12, 44},
{16, 267},
{22, 169},
{18, 268},
{24, 270},
{22, 42},
{28, 260},
{25, -140},
{24, 263},
{14, 284},
{9, 56},
{16, 167},
{13, 170},
{9, 58},
{14, 96},
{26, -31},
{9, 68},
{15, 214},
{0, 0},
{9, 69},
{38, -140},
{15, 175},
{14, 117},
{24, 220},
{15, 182},
{34, -12},
{24, 176},
{15, 214},
{25, 119},
{30, 86},
{24, 176},
{29, 215},
{17, 269},
{24, 226},
{15, 175},
{29, 213},
{24, 176},
{25, 124},
{29, 227},
{19, 183},
{15, 214},
{29, 215},
{15, 175},
{24, 176},
{25, 123},
{14, 102},
{15, 214},
{30, 86},
{29, 213},
{24, 176},
{25, 105},
{24, 176},
{25, 104},
{15, 182},
{29, 215},
{24, 176},
{29, 213},
{24, 226},
{34, -249},
{25, 288},
{29, 215},
{15, 182},
{24, 226},
{25, 123},
{15, 214},
{13, 178},
{24, 176},
{29, 228},
{16, 167},
{25, 125},
{24, 226},
{25, 104},
{38, 177},
{24, 176},
{25, 290},
{29, 228},
{24, 179},
{30, 271},
{29, 215},
{32, 272},
{29, 77},
{29, 77},
{38, 222},
{14, 287},
{13, 218},
{34, 78},
{34, 78},
{16, 280},
{15, 175},
{17, 276},
{21, 225},
{21, 216},
{16, -258},
{21, 173},
{8, 83},
{24, 219},
{16, 273},
{24, 176},
{25, 289},
{0, 0},
{29, 106},
{30, 86},
{29, 213},
{48, -246},
{24, 172},
{34, 107},
{29, 106},
{36, 281},
{37, 282},
{29, 77},
{39, 180},
{34, 107},
{26, 84},
{27, 85},
{34, 78},
{24, 172},
{39, 180},
{25, 285},
{24, 221},
{13, 170},
{29, 77},
{29, 77},
{8, 89},
{13, 170},
{30, 86},
{34, 78},
{34, 78},
{38, -136},
{0, 0},
{40, -136},
{24, 211},
{38, -138},
{18, 90},
{40, -138},
{24, 171},
{11, -245},
{30, 86},
{11, 40},
{12, 41},
{29, 208},
{26, 91},
{27, 92},
{35, -155},
{33, 209},
{34, 210},
{38, -155},
{20, 76},
{0, 0},
{22, 42},
{35, 93},
{20, 49},
{21, 265},
{37, -22},
{38, -22},
{39, -250},
{0, 0},
{40, -200},
{41, -201},
{40, -198},
{41, -199},
{40, -156},
{41, -157},
{45, -14},
{46, -15},
{24, -242},
{49, -252},
{26, -10},
{49, -252},
{44, -245},
{33, -239},
{34, -239},
{47, -245},
{48, -246},
{45, -16},
{46, -17},
{0, 0},
{45, -23},
{46, -24},
{45, -25},
{46, -26},
{45, -27},
{46, -28},
{45, -29},
{46, -30},
{0, 0},
};

static  SXBA_ELT SXPBA_kt9
#if SXBITS_PER_LONG==32
 [6] = {149, 0X00F80FDE, 0XE000EE00, 0X7D800001, 0X00100001, 0X00100000, }
#else
 [4] = {149, 0XE000EE0000F80FDE, 0X001000017D800001, 0X0000000000100000, }
#endif
/* End SXPBA_kt9 */;
static SXBA SXPBM_trans[]={NULL,
&SXPBA_kt9[0],
};
static SXINT P_lrgl[]={
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
};
static SXINT P_right_ctxt_head[]={0,
1,2,2,1,1,1,1,2,1,};
static SXINT P_param_ref[]={
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
1,9};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%d warnings and %d errors are reported.";
extern SXBOOLEAN sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no);

static unsigned char S_char_to_simple_class[]={
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,29,3,29,3,29,29,3,3,29,3,29,29,3,29,29,29,29,
3,29,3,29,29,3,3,29,3,29,29,3,29,29,29,29,29,29,29,29,29,29,29,29,29,
29,29,29,29,29,29,29,29,29,29,29,29,3,29,29,29,29,29,29,29,29,29,29,29,29,
29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,3,29,29,29,29,29,
29,29,29,3,3,3,3,3,3,3,3,3,4,5,6,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,7,8,9,10,3,11,12,13,14,15,16,17,18,19,
20,21,21,21,21,21,21,21,21,21,21,22,23,24,25,26,27,28,29,29,30,31,32,29,29,
29,33,29,29,34,29,35,29,29,29,29,29,29,36,29,29,29,29,29,37,38,39,3,22,3,
29,29,40,31,32,29,29,29,33,29,29,34,29,35,29,29,29,29,29,29,36,29,29,29,29,
29,41,42,43,44,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,29,3,29,3,29,29,3,3,29,3,29,
29,3,29,29,29,29,3,29,3,29,29,3,3,29,3,29,29,3,29,29,29,29,29,29,29,
29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,3,29,29,29,29,29,29,
29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
3,29,29,29,29,29,29,29,29,};
static SXSTMI SXS_transition[]={0,
0x0,0x4029,0x0,0x2402,0x2402,0x2402,0x0,0x4417,0x9002,0x2403,0x2c04,0x2405,
0x4418,0x4419,0x4428,0x2406,0x4415,0x441d,0x440f,0x2407,0x2c08,0x9004,0x4409,0x4412,
0x240a,0x4413,0x4420,0x240b,0x9004,0x9004,0x9004,0x9004,0x9004,0x9004,0x9004,0x9004,
0x4410,0x0,0x4411,0x9004,0x440d,0x4c26,0x440e,0x4421,
0x0,0x4000,0x0,0x1402,0x1402,0x1402,0x0,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x240e,0x4000,0x4000,0x4000,0x4000,
0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x4000,0x0,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x9406,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c0f,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x4027,0x0,0x4027,0x4027,0x4027,0x2c10,0x0,0x4027,0x4027,0x0,0x0,
0x4027,0x0,0x0,0x0,0x0,0x2c10,0x0,0x4027,0x2c10,0x2c10,0x0,0x0,
0x0,0x0,0x0,0x0,0x2c10,0x2c10,0x2c10,0x2c10,0x2c10,0x2c10,0x2c10,0x2c10,
0x0,0x0,0x0,0x2c10,0x0,0x0,0x0,0x0,
0x0,0x0,0x1c05,0x1c05,0x0,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x440b,
0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,
0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,
0x1c05,0x2411,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,0x1c05,
0x0,0x4022,0x0,0x4022,0x4022,0x4022,0x0,0x4022,0x4022,0x4022,0x0,0x0,
0x4022,0x4022,0x0,0x0,0x4022,0x0,0x4022,0x4022,0x0,0x0,0x4022,0x0,
0x4423,0x4022,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x4022,0x0,0x4022,0x4022,0x4022,0x0,
0x0,0x9007,0x0,0x9007,0x9007,0x9007,0x0,0x0,0x9007,0x0,0x9007,0x9007,
0x9007,0x0,0x2412,0x9007,0x0,0x9007,0x0,0x2413,0x9007,0x9007,0x0,0x0,
0x0,0x0,0x0,0x0,0x9007,0x9007,0x9007,0x9007,0x9007,0x9007,0x9007,0x9007,
0x0,0x0,0x0,0x9007,0x0,0x0,0x0,0x0,
0x0,0x4014,0x0,0x4014,0x4014,0x4014,0x0,0x4014,0x4014,0x0,0x0,0x4014,
0x4014,0x4014,0x4014,0x4014,0x4014,0x4014,0x4014,0x4014,0x1c08,0x4014,0x4014,0x4014,
0x4014,0x4014,0x0,0x0,0x4014,0x4014,0x4014,0x4014,0x4014,0x4014,0x4014,0x4014,
0x0,0x0,0x4014,0x4014,0x0,0x4014,0x4014,0x0,
0x0,0x300c,0x0,0x300c,0x300c,0x300c,0x1c09,0x300c,0x300c,0x300c,0x0,0x300c,
0x300c,0x300c,0x300c,0x300c,0x300c,0x1c09,0x300c,0x300c,0x1c09,0x1c09,0x300c,0x300c,
0x300c,0x300c,0x300c,0x0,0x9008,0x9008,0x9008,0x9008,0x9008,0x9008,0x9008,0x9008,
0x0,0x0,0x300c,0x9008,0x300c,0x300c,0x300c,0x300c,
0x0,0x4008,0x0,0x4008,0x4008,0x4008,0x0,0x4008,0x4008,0x4008,0x4008,0x4008,
0x4008,0x0,0x0,0x4008,0x0,0x4008,0x0,0x4008,0x4008,0x4008,0x0,0x4008,
0x0,0x0,0x2414,0x0,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,0x4008,
0x4008,0x0,0x0,0x2415,0x4008,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x2c16,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x2c16,0x0,0x0,0x2c16,0x2c16,0x0,0x0,
0x0,0x0,0x0,0x0,0x2c16,0x2c16,0x2c16,0x2c16,0x2c16,0x2c16,0x2c16,0x2c16,
0x0,0x0,0x0,0x2c16,0x0,0x0,0x0,0x0,
0x0,0x0,0x2417,0x140c,0x4400,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2418,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x0,0x301c,0x0,0x301c,0x301c,0x301c,0x1c0d,0x301c,0x301c,0x301c,0x0,0x1c0d,
0x301c,0x301c,0x301c,0x301c,0x301c,0x1c0d,0x1c0d,0x1c0d,0x1c0d,0x900a,0x301c,0x301c,
0x301c,0x301c,0x301c,0x0,0x900a,0x900a,0x900a,0x900a,0x900a,0x900a,0x900a,0x900a,
0x0,0x0,0x301c,0x900a,0x301c,0x301c,0x301c,0x301c,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x2412,0x0,0x0,0x0,0x0,0x2413,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x401e,0x0,0x401e,0x401e,0x401e,0x0,0x0,0x401e,0x0,0x0,0x401e,
0x401e,0x401e,0x0,0x401e,0x0,0x401e,0x401e,0x401e,0x1c0f,0x401e,0x401e,0x401e,
0x401e,0x0,0x0,0x0,0x401e,0x401e,0x401e,0x401e,0x401e,0x401e,0x401e,0x401e,
0x0,0x0,0x0,0x401e,0x0,0x0,0x0,0x0,
0x0,0x3000,0x0,0x3000,0x3000,0x3000,0x1c10,0x0,0x3000,0x3000,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x1c10,0x0,0x3000,0x1c10,0x1c10,0x0,0x0,
0x0,0x0,0x0,0x0,0x1c10,0x1c10,0x1c10,0x1c10,0x1c10,0x1c10,0x1c10,0x1c10,
0x0,0x0,0x0,0x1c10,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x2405,0x0,0x0,0x0,0x0,0x0,0x0,0x2c05,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x2c05,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,
0x1412,0x1412,0x2419,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,
0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,
0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,0x1412,
0x0,0x0,0x1413,0x1413,0x2402,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,
0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,
0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,
0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,0x1413,
0x0,0x0,0x9c0c,0x441b,0x441b,0x441b,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,
0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,
0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,
0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,0x9c0c,
0x0,0x0,0x9c0d,0x441a,0x441a,0x441a,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,
0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,
0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,
0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,0x9c0d,
0x0,0x4007,0x0,0x4007,0x4007,0x4007,0x1c16,0x0,0x4007,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x4007,0x1c16,0x0,0x4007,0x1c16,0x1c16,0x0,0x0,
0x4007,0x0,0x0,0x0,0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,0x1c16,
0x0,0x0,0x4007,0x1c16,0x0,0x0,0x0,0x0,
0x0,0x0,0x1417,0x1417,0x4400,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,
0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,
0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,
0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,0x1417,
0x0,0x0,0x2417,0x2417,0x4400,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x241a,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x0,0x0,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,
0x2412,0x2412,0x1419,0x2412,0x2412,0x2412,0x2412,0x2402,0x2412,0x2412,0x2412,0x2412,
0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,
0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,0x2412,
0x0,0x0,0x2417,0x2417,0x4400,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x241b,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x241b,0x2417,0x2417,0x2417,0x2417,
0x0,0x0,0x2417,0x2417,0x4400,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x241c,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x0,0x0,0x2417,0x2417,0x4400,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x241d,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x0,0x0,0x2417,0x2417,0x4400,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x241e,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x0,0x0,0x2417,0x2417,0x4400,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x241f,0x2417,0x2417,0x2417,0x2417,
0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,0x2417,
0x0,0x0,0x0,0x2420,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x2c21,0x1420,0x0,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,
0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,
0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,
0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,0x2c21,
0x0,0x0,0x1c21,0x2422,0x9412,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,
0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,
0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,
0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,0x1c21,
0x0,0x0,0x0,0x1422,0x9412,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[44],
&SXS_transition[88],
&SXS_transition[132],
&SXS_transition[176],
&SXS_transition[220],
&SXS_transition[264],
&SXS_transition[308],
&SXS_transition[352],
&SXS_transition[396],
&SXS_transition[440],
&SXS_transition[484],
&SXS_transition[528],
&SXS_transition[572],
&SXS_transition[616],
&SXS_transition[660],
&SXS_transition[704],
&SXS_transition[748],
&SXS_transition[792],
&SXS_transition[836],
&SXS_transition[880],
&SXS_transition[924],
&SXS_transition[968],
&SXS_transition[1012],
&SXS_transition[1056],
&SXS_transition[1100],
&SXS_transition[1144],
&SXS_transition[1188],
&SXS_transition[1232],
&SXS_transition[1276],
&SXS_transition[1320],
&SXS_transition[1364],
&SXS_transition[1408],
&SXS_transition[1452],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2001,6,2,1,0},
{0x240c,1,0,1,1},
{0x0,0,0,1,1},
{0x2c0d,1,0,1,1},
{0x2c09,0,0,1,1},
{0x401e,9,48,1,0},
{0x4026,3,0,0,0},
{0x300c,1,0,1,1},
{0x2c09,0,0,1,1},
{0x301c,1,0,1,1},
{0x2c0d,0,0,1,1},
{0x900e,4,0,0,0},
{0x900f,4,0,0,0},
{0x9010,11,0,1,0},
{0x9011,11,0,1,0},
{0x401b,5,0,1,0},
{0x401a,5,0,1,0},
{0x9001,1,0,0,0},
};
static char *S_adrp[]={0,
"ATTRIBUTE",
"ARGUMENT",
"CATEGORY",
"TERMINAL",
"AMALGAM",
"COMPOUND",
"%MACRO_NAME",
"=",
";",
"COMPLETE",
"%CHAINE",
"%IDENT",
"{",
"}",
".",
"[",
"]",
"<",
">",
"%ENTIER",
",",
"pred",
"\"",
"(",
")",
"CONTRAINT",
"OPTIONAL",
"%MOT_1COL",
"-",
"%NODE_REF",
":",
"?",
"~",
"+",
"ADD",
"&lex",
"&lexeme",
"|",
"&",
"*",
"End Of File",
};
static SXINT S_is_a_keyword[]={
11,1,2,3,4,5,6,10,22,31,36,37};
static SXINT S_is_a_generic_terminal[]={
6,7,11,12,20,28,30};
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
static struct SXT_node_info SXT_node_info[]={{0,0},
{50,1},{0,3},{28,5},{11,5},{5,6},{21,7},{81,8},{3,9},{24,10},{53,11},{23,13},{0,14},
{6,16},{0,17},{67,19},{0,20},{66,21},{0,23},{13,25},{12,26},{15,28},{16,30},{18,32},{14,33},
{17,34},{85,35},{37,35},{37,36},{0,37},{85,38},{0,38},{57,39},{36,41},{34,41},{0,42},{20,44},
{19,45},{70,48},{51,51},{41,52},{63,53},{43,54},{42,54},{44,54},{0,54},{0,55},{56,56},{31,58},
{33,58},{0,59},{55,60},{30,62},{32,62},{0,63},{74,65},{0,66},{73,67},{35,69},{0,69},{29,71},
{62,72},{0,73},{0,74},{4,76},{0,77},{25,79},{0,80},{46,82},{45,84},{22,85},{0,86},{72,88},
{71,89},{0,91},{84,93},{45,94},{64,95},{0,96},{38,98},{39,98},{68,100},{0,101},{0,102},{59,103},
{40,104},{60,105},{82,106},{0,109},{83,110},{26,110},{27,110},{2,110},{61,110},{49,110},{48,111},{0,112},
{0,113},{36,114},{31,114},{30,114},{0,114},{0,115},{0,116},{10,118},{0,119},{10,121},{0,122},{9,123},
{8,123},{0,123},{9,124},{8,124},{7,124},{0,126},{77,127},{78,129},{75,131},{58,133},{0,134},{0,135},
{79,136},{80,138},{79,140},{80,142},{79,144},{80,146},{76,148},{76,150},{76,152},{0,154},{47,155},{47,156},
{0,157},{0,158},{0,159},{0,160},{0,161},{0,162},{65,164},{0,166},{0,167},{0,168},{37,169},{0,170},
{22,171},{45,172},{69,173},{0,173},};
static SXINT T_ter_to_node_name[]={
0,0,0,0,0,0,0,52,0,0,0,22,45,0,0,0,0,0,0,0,37,0,0,0,0,
0,0,0,54,0,58,};
static char *T_node_name[]={NULL,
"ERROR",
"ADD_OPER",
"AMALGAM",
"AMALGAM_S",
"ARGUMENT",
"ARGUMENT_S",
"ATOM_CONCAT",
"ATOM_MINUS",
"ATOM_PLUS",
"ATOM_S",
"ATTRIBUTE",
"ATTRIBUTE_PASS_NB",
"ATTRIBUTE_S",
"ATTR_ARG_STRUCT",
"ATTR_ATOMS",
"ATTR_DOTS",
"ATTR_SET_OF_STRUCT",
"ATTR_STRUCT",
"ATTR_VAL",
"ATTR_VAL_S",
"CATEGORY",
"CHAINE",
"COMPLETE",
"COMPOUND",
"COMPOUND_S",
"CONTRAINTE_OPER",
"DANS_OPER",
"DECLARATION_S",
"DISJONCTION_S",
"EMPTY_ADJUNCT",
"EMPTY_ATOM",
"EMPTY_NAMED_ADJUNCT",
"EMPTY_NAMED_ATOM",
"EMPTY_NAMED_STRUCT",
"EMPTY_SOUS_CAT",
"EMPTY_STRUCT",
"ENTIER",
"EQUATION_S",
"ETIQUETTE",
"EXISTE",
"FIELD",
"FIELD_CONSTRAINT",
"FIELD_EQUAL",
"FIELD_OPTIONAL",
"IDENT",
"IDENT_S",
"KLEENE",
"LEXEME_NODE_REF",
"LEX_NODE_REF",
"LFG",
"MACRO",
"MACRO_NAME",
"MACRO_STRUCTURE",
"MOT_1COL",
"NAMED_ADJUNCT",
"NAMED_ATOM",
"NAMED_STRUCTURE",
"NODE_REF",
"NON",
"NONEXISTE",
"OPTION_OPER",
"OPT_DISJONCTION",
"OPT_FIELD",
"OPT_NON_TERMINAL",
"OU_S",
"PAIR",
"PAIR_S",
"POSSIBLE",
"PRED",
"PRED_FIELD",
"RULE",
"RULES_S",
"SOUS_CAT",
"STRUCTURE_S",
"TERME_CHAMP",
"TERME_CHAMP_RE",
"TERME_CHEMIN",
"TERME_CHEMIN_DOT",
"TERME_CHEMIN_RE",
"TERME_CHEMIN_RE_DOT",
"TERMINAL",
"UNIFIER",
"UNIF_OPER",
"VOCABULARY_S",
"VOID",
};
extern SXINT sempass(SXINT what, struct sxtables *sxtables_ptr);
static char T_stack_schema[]={0,
0,1,0,1,0,0,0,0,0,0,1,3,0,1,2,1,1,2,1,0,0,1,1,2,1,
0,1,1,4,4,7,3,5,5,0,0,0,1,0,2,0,0,2,0,0,1,2,1,2,4,
0,0,1,0,0,0,2,0,1,0,2,0,0,2,0,1,0,2,0,2,0,1,0,1,2,
1,1,2,1,0,1,0,1,0,0,0,1,0,0,2,0,1,2,0,1,1,2,0,2,1,
0,1,0,0,0,0,1,2,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,2,
1,0,1,0,2,0,2,0,1,1,0,1,0,2,0,1,0,2,0,1,0,2,0,2,0,
2,0,2,0,1,1,0,1,0,1,1,0,2,0,2,0,0,0,0,0,0,0,0,};

static struct SXT_tables SXT_tables=
{SXT_node_info, T_ter_to_node_name, T_stack_schema, sempass, T_node_name};
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxatc(SXINT what, ...);

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(PARSER_T) sxparser}, {255, 41, 1, 3, 4, 44, 0, 34, 1, 1, 0, 
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
{37, 143, 143, 184, 228, 291, 320, 557, 41, 51, 147, 147, 148, 86, 0, 9, 4, 7, 2, 5, 11, 5, 4,
reductions,
t_bases,
nt_bases,
vector-320,
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
(SEMACT_T) sxatc},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables,
NULL,
};

#include	"sxdico.h"

#define KW_NB		11
#define INIT_BASE	1
#define INIT_SUFFIX	32
#define CLASS_MASK	15
#define STOP_MASK	16
#define BASE_SHIFT	6
#define SUFFIX_MASK	32

static SXINT kw_code2t_code [12] = {0,
37, 36, 31, 22, 10, 6, 5, 4, 3, 2, 1, 
};

static SXINT kw_code2lgth [12] = {0,
7, 4, 1, 4, 8, 8, 7, 8, 8, 8, 9, 
};

static SXUINT comb_vector [23] = {
0, 0, 209, 402, 643, 0, 0, 534, 471, 
664, 601, 282, 91, 156, 724, 341, 0, 0, 0, 
0, 0, 0, 0, 
};

static unsigned char char2class [256] = {
'\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\001', '\000', '\000', '\000', '\000', 
'\000', '\000', '\004', '\000', '\005', '\002', '\003', '\000', 
'\000', '\000', '\000', '\000', '\000', '\006', '\007', '\000', 
'\000', '\000', '\000', '\000', '\000', '\b', '\000', '\000', 
'\000', '\000', '\t', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\n', '\013', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\f', '\000', '\000', '\000', '\000', '\000', '\000', 
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

/* End of sxtables for lfg [Thu Mar  6 12:56:14 2008] */
