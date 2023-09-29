/* Beginning of sxtables for ppc_args [Tue May 27 16:47:09 2003] */
#define SXP_MAX 64
#include "sxunix.h"
#include "sxba.h"
char WHAT_PPCARGST[] = "@(#)SYNTAX - $Id: ppc_args_t.c 1191 2008-03-11 13:24:01Z rlacroix $" WHAT_DEBUG;

static struct SXP_reductions reductions[]={{0,0,0,0},

{0, 8, 1, -44},
{5, 14, 1, -1},
{5, 15, 1, -1},
{20, 41, 1, -1},
{20, 42, 1, -1},
{0, 3, 1, -45},
{0, 9, 0, -44},
{0, 0, 2, -51},
{0, 5, 0, 64},
{0, 6, 0, 64},
{0, 7, 0, 64},
{3, 10, 0, -1},
{3, 11, 0, -1},
{4, 12, 0, -1},
{4, 13, 0, -1},
{6, 16, 0, -1},
{6, 17, 0, -1},
{8, 20, 0, -1},
{8, 21, 0, -1},
{9, 22, 0, -1},
{9, 23, 0, -1},
{10, 24, 0, -1},
{10, 25, 0, -1},
{11, 26, 0, -1},
{11, 27, 0, -1},
{12, 28, 0, -1},
{12, 29, 0, -1},
{14, 32, 0, -1},
{15, 33, 0, -1},
{18, 37, 0, -1},
{18, 38, 0, -1},
{19, 39, 0, -1},
{19, 40, 0, -1},
{1, 1, 1, -51},
{7, 18, 1, -1},
{7, 19, 1, -1},
{13, 30, 1, -1},
{13, 31, 1, -1},
{16, 34, 1, -1},
{17, 35, 1, -1},
{17, 36, 1, -1},
{21, 43, 1, -1},
{2, 2, 2, -51},
};
static struct SXP_bases t_bases[]={{0,0,0},

{-34, 61, 62},
{-43, 61, 0},
{0, 61, 0},
{-7, 0, 0},
{0, 61, 0},
{0, 61, 0},
{0, 61, 0},
{0, -8, 38},
{0, 35, 2},
{0, 36, 2},
{0, 37, 3},
{0, 38, 3},
{0, 39, 3},
{0, 40, 3},
{0, 41, 3},
{0, 42, 3},
{0, 47, 38},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-45, -1, 5},
{-6, 0, 0},
{-2, 0, 0},
{-44, -51, 1},
{-3, 0, 0},
{-4, 0, 0},
{-5, 0, 0},
};
static struct SXP_item vector[]={
{1, 9},
{2, 10},
{3, 11},
{0, 0},
{4, 12},
{5, 13},
{6, 14},
{7, 15},
{8, 46},
{9, 48},
{10, 16},
{11, 17},
{12, 52},
{13, 53},
{14, 18},
{15, 19},
{16, 20},
{17, 21},
{18, 22},
{19, 23},
{20, 24},
{21, 25},
{22, 26},
{23, 27},
{24, 54},
{25, 55},
{26, 28},
{27, 29},
{28, 56},
{29, 57},
{30, 58},
{31, 30},
{32, 31},
{33, 32},
{34, 33},
{35, 49},
{36, 50},
{37, 59},
{0, 0},
};
static SXINT P_lrgl[]={
3,0,-2,2,0,
2,-2,1,0,0,
4,0,2,3,4,
4,1,2,3,4,
};
static SXINT *P_lregle[]={NULL,
&P_lrgl[0],
&P_lrgl[5],
&P_lrgl[10],
&P_lrgl[15],
};
static SXINT P_right_ctxt_head[]={0,
2,1,2,1,};
static SXINT P_param_ref[]={
1,-2,
0,-1,
1,
0,
-1,1,
};
static struct SXP_local_mess SXP_local_mess[]={{0,NULL,NULL},
{2,"%sSpeling eror: \"%s\" should read \"%s\".",&P_param_ref[0]},
{2,"%sSpeling eror: \"%s\" should read \"%s\".",&P_param_ref[2]},
{1,"%s\"%s\" is erased.",&P_param_ref[4]},
{1,"%s\"%s\" is erased.",&P_param_ref[5]},
{2,"%s\"%s\" is inserted before \"%s\".",&P_param_ref[6]},
};
static SXINT P_no_delete[]={
0};
static SXINT P_no_insert[]={
0};
static char *P_global_mess[]={
"%s\"%s\" expected.",
"%s\"%s\" (or \"%s\") expected.",
"%sCommand garbled...",
"%s... Restarting analysis from \"%s\".",
"EOF",
"%s... Too much noise.",
};
static SXINT PER_tset[]={
0};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Error (ppc.args): ",
"\002Error (ppc.args): ",
};
extern SXBOOLEAN sxprecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no);

static unsigned char S_char_to_simple_class[]={
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,3,3,3,3,3,3,3,3,3,4,5,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,6,6,7,8,6,6,6,6,6,6,6,6,6,6,
6,9,9,9,9,9,9,9,9,9,9,6,6,6,6,6,6,6,10,10,10,10,10,10,10,
10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,6,6,6,6,10,6,
10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
10,11,6,12,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,};
static SXSTMI SXS_transition[]={0,
0x0,0x4026,0x2402,0x2403,0x2402,0x2c04,0x9002,0x9401,0x2c05,0x2c06,0x2c04,0x2c04,

0x0,0x4000,0x1402,0x2403,0x1402,0x4000,0x9004,0x4000,0x4000,0x4000,0x4000,0x4000,

0x0,0x4000,0x2402,0x1403,0x2402,0x4000,0x2407,0x4000,0x4000,0x4000,0x4000,0x4000,

0x0,0x3001,0x3001,0x3001,0x3001,0x1c04,0x9006,0x9401,0x1c04,0x1c04,0x1c04,0x1c04,

0x0,0x4003,0x4003,0x4003,0x4003,0x2c04,0x9008,0x9401,0x1c05,0x2c04,0x2c04,0x2c04,

0x0,0x4002,0x4002,0x4002,0x4002,0x2c04,0x900a,0x9401,0x1c06,0x1c06,0x2c04,0x2c04,

0x0,0x0,0x1407,0x1407,0x2402,0x1407,0x1407,0x1407,0x1407,0x1407,0x1407,0x1407,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c09,0x240a,0x0,

0x0,0x900c,0x900c,0x900c,0x900c,0x900c,0x900c,0x900c,0x1c09,0x1c09,0x900c,0x900c,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c0b,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1c0b,0x1c0b,0x0,0x940c,

};
static SXSTMI *S_transition_matrix[]={NULL,
&SXS_transition[0],
&SXS_transition[12],
&SXS_transition[24],
&SXS_transition[36],
&SXS_transition[48],
&SXS_transition[60],
&SXS_transition[72],
&SXS_transition[84],
&SXS_transition[96],
&SXS_transition[108],
&SXS_transition[120],
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={{0,0,0,0,0},
{0x2008,10,0,1,0},
{0x2407,1,0,1,1},
{0x2c04,0,0,1,1},
{0x2407,1,0,1,1},
{0x4000,0,0,1,1},
{0x3001,1,0,1,1},
{0x2c04,0,0,1,1},
{0x4003,1,0,1,1},
{0x2c04,0,0,1,1},
{0x4002,1,0,1,1},
{0x2c04,0,0,1,1},
{0x2004,3,0,0,0},
};
static char *S_adrp[]={0,
"%PATH",
"%IDENTIFIER",
"%NUMBER",
"-verbose",
"-v",
"-noverbose",
"-nv",
"-output_file",
"-of",
"-nooutput_file",
"-nof",
"-TYPE",
"-T",
"-kwuc",
"-key_words_upper_cased",
"-kwlc",
"-key_words_lower_cased",
"-kwci",
"-key_words_capitalised_initial",
"-kwdk",
"-key_words_darkened",
"-nkwdk",
"-nokey_words_darkened",
"-ll",
"-line_length",
"-tabs",
"-notabs",
"-every",
"-mm",
"-max_margin",
"-sm",
"-shift_margin",
"-nsm",
"-no_shift_margin",
"-directory",
"-d",
"-style",
"End Of File",
};
static SXINT S_is_a_keyword[]={
34,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
static SXINT S_is_a_generic_terminal[]={
3,1,2,3};
static SXINT S_lrgl[]={
1,0,
};
static SXINT *S_lregle[]={NULL,
&S_lrgl[0],
};
static SXINT S_param_ref[]={
0,
};
static struct SXS_local_mess SXS_local_mess[]={{0,NULL,NULL},
{1,"%s%s",&S_param_ref[0]},
};
static SXINT S_no_delete[]={
0};
static SXINT S_no_insert[]={
0};
static char *S_global_mess[]={0,
"%s\"%s\" is erased.",
"%s",
"\\n",
"EOF",
"%sLexical analysis stops.",
};
extern SXINT SCANACT(SXINT what, SXINT act_no);
extern SXINT sxscan_it(void);
extern SXBOOLEAN sxsrecovery (SXINT sxsrecovery_what, SXINT state_no, unsigned char *class);
static SXINT check_keyword(char *init_string, SXINT init_length);
extern SXINT sxscanner(SXINT what_to_do, struct sxtables *arg);
extern SXINT sxparser(SXINT what_to_do, struct sxtables *arg);
extern SXINT SEMACT(SXINT what, struct sxtables *arg);

struct sxtables sxtables={
52113, /* magic */
{sxscanner,(PARSER_T) sxparser}, {255, 38, 1, 1, 0, 12, 0, 11, 1, 1, 0, 
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
{6, 43, 43, 47, 50, 60, 62, 100, 38, 5, 43, 43, 17, 8, 0, 4, 4, 4, 1, 2, 8, 2, 3,
reductions,
t_bases,
nt_bases,
vector-62,
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
NULL,
(sxsem_tables*)NULL,
NULL,
};

#include	"sxdico.h"

#define KW_NB		34
#define INIT_BASE	1
#define INIT_SUFFIX	64
#define CLASS_MASK	31
#define STOP_MASK	32
#define BASE_SHIFT	7
#define SUFFIX_MASK	64

static SXINT kw_code2t_code [35] = {0,
37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 
22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 
6, 5, 4, 
};

static SXINT kw_code2lgth [35] = {0,
6, 2, 10, 16, 4, 13, 3, 11, 3, 6, 7, 5, 12, 3, 21, 
6, 19, 5, 30, 5, 22, 5, 22, 5, 2, 5, 4, 14, 3, 12, 3, 
10, 2, 8, 
};

static SXUINT comb_vector [91] = {
0, 0, 3361, 3234, 3395, 2884, 0, 2758, 6278, 
5704, 7561, 7690, 7435, 7308, 553, 7177, 811, 7051, 2609, 
3282, 7827, 1716, 1449, 3798, 1961, 2218, 1828, 1078, 295, 
840, 2852, 3109, 4263, 422, 4009, 1320, 1591, 935, 8590, 
681, 3915, 2089, 2346, 2485, 3497, 4137, 3625, 3085, 5316, 
6794, 3759, 1206, 3887, 172, 4402, 7947, 2724, 2981, 8081, 
4364, 8203, 6024, 4999, 1927, 3463, 1671, 647, 4103, 4487, 
4615, 4743, 0, 0, 0, 0, 0, 0, 6669, 0, 
0, 0, 272, 8335, 0, 0, 0, 0, 0, 0, 
0, 0, 
};

static unsigned char char2class [256] = {
'\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\007', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\001', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\002', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\r', '\000', '\025', '\000', '\003', '\006', '\b', '\023', 
'\000', '\024', '\021', '\000', '\n', '\004', '\026', '\t', 
'\017', '\000', '\000', '\020', '\013', '\027', '\005', '\022', 
'\016', '\000', '\f', '\000', '\000', '\000', '\000', '\000', 
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

/* End of sxtables for ppc_args [Tue May 27 16:47:09 2003] */
