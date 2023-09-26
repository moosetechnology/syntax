/* Beginning of sxtables for ppada_args [Tue May 27 16:47:06 2003] */
#define SXP_MAX 70
#include "sxunix.h"
#include "sxba.h"
char WHAT_PPADAARGST[] = "@(#)SYNTAX - $Id: ppada_args_t.c 1366 2008-05-27 10:27:47Z rlacroix $";

static struct SXP_reductions reductions[]={{0,0,0,0},

{0, 8, 1, -53},
{5, 14, 1, -1},
{5, 15, 1, -1},
{20, 39, 1, -1},
{20, 40, 1, -1},
{0, 3, 1, -54},
{0, 9, 0, -53},
{0, 0, 2, -60},
{0, 5, 0, 70},
{0, 6, 0, 70},
{0, 7, 0, 70},
{3, 10, 0, -1},
{3, 11, 0, -1},
{4, 12, 0, -1},
{4, 13, 0, -1},
{6, 16, 0, -1},
{6, 17, 0, -1},
{8, 18, 0, -1},
{8, 19, 0, -1},
{9, 20, 0, -1},
{9, 21, 0, -1},
{10, 22, 0, -1},
{10, 23, 0, -1},
{11, 24, 0, -1},
{11, 25, 0, -1},
{12, 26, 0, -1},
{12, 27, 0, -1},
{14, 30, 0, -1},
{15, 31, 0, -1},
{18, 35, 0, -1},
{18, 36, 0, -1},
{19, 37, 0, -1},
{19, 38, 0, -1},
{21, 41, 0, -1},
{21, 42, 0, -1},
{22, 43, 0, -1},
{22, 44, 0, -1},
{23, 45, 0, -1},
{23, 46, 0, -1},
{24, 47, 0, -1},
{24, 48, 0, -1},
{25, 49, 0, -1},
{25, 50, 0, -1},
{26, 51, 0, -1},
{26, 52, 0, -1},
{1, 1, 1, -60},
{13, 28, 1, -1},
{13, 29, 1, -1},
{16, 32, 1, -1},
{17, 33, 1, -1},
{17, 34, 1, -1},
{2, 2, 2, -60},
};
static struct SXP_bases t_bases[]={{0,0,0},

{-46, 67, 68},
{-52, 67, 0},
{0, 67, 0},
{-7, 0, 0},
{0, 67, 0},
{0, 67, 0},
{0, 67, 0},
{0, -8, 47},
{0, 47, 3},
{0, 48, 3},
{0, 49, 3},
{0, 50, 3},
{0, 51, 3},
{0, 56, 47},
};
static struct SXP_bases nt_bases[]={{0,0,0},
{-54, -1, 5},
{-6, 0, 0},
{-2, 0, 0},
{-53, -60, 1},
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
{8, 55},
{9, 57},
{10, 16},
{11, 17},
{12, 18},
{13, 19},
{14, 20},
{15, 21},
{16, 22},
{17, 23},
{18, 24},
{19, 25},
{20, 26},
{21, 27},
{22, 61},
{23, 62},
{24, 28},
{25, 29},
{26, 63},
{27, 64},
{28, 65},
{29, 30},
{30, 31},
{31, 32},
{32, 33},
{33, 58},
{34, 59},
{35, 34},
{36, 35},
{37, 36},
{38, 37},
{39, 38},
{40, 39},
{41, 40},
{42, 41},
{43, 42},
{44, 43},
{45, 44},
{46, 45},
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
"\001Error (ppada.args): ",
"\002Error (ppada.args): ",
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
0x0,0x402f,0x2402,0x2403,0x2402,0x2c04,0x9002,0x9401,0x2c05,0x2c06,0x2c04,0x2c04,

0x0,0x4000,0x1402,0x2403,0x1402,0x4000,0x9004,0x4000,0x4000,0x4000,0x4000,0x4000,

0x0,0x4000,0x2402,0x1403,0x2402,0x4000,0x2407,0x4000,0x4000,0x4000,0x4000,0x4000,

0x0,0x3001,0x3001,0x3001,0x3001,0x1c04,0x9006,0x9401,0x1c04,0x1c04,0x1c04,0x1c04,

0x0,0x4003,0x4003,0x4003,0x4003,0x2c04,0x9008,0x9401,0x1c05,0x2c04,0x2c04,0x2c04,

0x0,0x4002,0x4002,0x4002,0x4002,0x2c04,0x900a,0x9401,0x2c09,0x2c09,0x2c04,0x2c04,

0x0,0x0,0x1407,0x1407,0x2402,0x1407,0x1407,0x1407,0x1407,0x1407,0x1407,0x1407,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c0a,0x240b,0x0,

0x0,0x4002,0x4002,0x4002,0x4002,0x4002,0x4002,0x4002,0x1c09,0x1c09,0x4002,0x4002,

0x0,0x900c,0x900c,0x900c,0x900c,0x900c,0x900c,0x900c,0x1c0a,0x1c0a,0x900c,0x900c,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2c0c,0x0,0x0,

0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1c0c,0x1c0c,0x0,0x940c,

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
&SXS_transition[132],
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
"-iduc",
"-identifiers_upper_cased",
"-idlc",
"-identifiers_lower_cased",
"-idci",
"-identifiers_capitalised_initial",
"-nidm",
"-noidentifier_modification",
"-iddk",
"-identifiers_darkened",
"-niddk",
"-noidentifiers_darkened",
"End Of File",
};
static SXINT S_is_a_keyword[]={
43,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46};
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
{sxscanner,(SXPARSER_T) sxparser}, {255, 47, 1, 1, 0, 12, 0, 12, 1, 1, 0, 
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
(SXSCANACT_T) SCANACT,
(SXRECOVERY_T) sxsrecovery,
check_keyword,
},
{6, 52, 52, 56, 59, 66, 68, 115, 47, 5, 52, 52, 14, 8, 0, 4, 4, 4, 1, 2, 8, 2, 3,
reductions,
t_bases,
nt_bases,
vector-68,
NULL,
NULL,
NULL,
NULL,
P_lregle,
P_right_ctxt_head,
SXP_local_mess,
P_no_delete,
P_no_insert,
P_global_mess,PER_tset,sxscan_it,(SXRECOVERY_T) sxprecovery,
NULL,
NULL,
(SXSEMACT_T) SEMACT},
err_titles,
NULL,
(sxsem_tables*)NULL,
NULL,
};

#include	"sxdico.h"

#define KW_NB		43
#define INIT_BASE	1
#define INIT_SUFFIX	64
#define CLASS_MASK	31
#define STOP_MASK	32
#define BASE_SHIFT	7
#define SUFFIX_MASK	64

static SXINT kw_code2t_code [44] = {0,
46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 
31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 
15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 
};

static SXINT kw_code2lgth [44] = {0,
23, 6, 21, 5, 26, 5, 32, 5, 24, 5, 24, 5, 2, 10, 16, 
4, 13, 3, 11, 3, 6, 7, 5, 12, 3, 21, 6, 19, 5, 30, 5, 
22, 5, 22, 5, 4, 14, 3, 12, 3, 10, 2, 8, 
};

static SXUINT comb_vector [123] = {
0, 0, 8769, 2882, 7235, 803, 5251, 9986, 711, 
3272, 9609, 9482, 1960, 1932, 3532, 9742, 2220, 9868, 12945, 
9170, 3123, 3078, 341, 2613, 3234, 2485, 3976, 553, 3754, 
12168, 425, 3626, 2856, 2342, 7433, 2088, 683, 2989, 937, 
3882, 297, 3498, 9236, 1065, 4010, 4648, 5288, 4776, 4907, 
4130, 5035, 1186, 4389, 7052, 1445, 399, 5554, 11145, 10250, 
169, 3370, 12680, 1702, 6855, 518, 12556, 1571, 4516, 1315, 
4260, 8386, 6032, 1827, 8133, 8196, 5131, 2727, 5414, 3718, 
5160, 2310, 4358, 4742, 4870, 4998, 0, 0, 10119, 0, 
8976, 12291, 10631, 0, 0, 10380, 10761, 9361, 6288, 10511, 
11022, 10889, 11661, 11272, 12039, 6150, 2694, 6411, 8838, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 
};

static unsigned char char2class [256] = {
'\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\006', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
'\020', '\000', '\024', '\000', '\001', '\003', '\007', '\016', 
'\000', '\023', '\t', '\000', '\n', '\002', '\025', '\b', 
'\013', '\000', '\000', '\017', '\f', '\r', '\005', '\022', 
'\004', '\000', '\021', '\000', '\000', '\000', '\000', '\000', 
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

/* End of sxtables for ppada_args [Tue May 27 16:47:06 2003] */
