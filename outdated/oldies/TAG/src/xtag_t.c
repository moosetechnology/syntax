/* Beginning of sxtables for xtag [Fri Dec 30 13:05:00 1994] */
#include "sxunix.h"
static struct SXP_reductions reductions[]={
{6, 3, 0, -28},
{2, 2, 1, -28},
{6, 11, 0, 67},
{2, 5, 1, 65},
{2, 10, 1, 67},
{1, 12, 1, -5},
{1, 13, 1, -5},
{1, 14, 1, -5},
{0, 15, 1, -5},
{0, 0, 2, -43},
{1, 1, 1, -43},
{1, 4, 3, 64},
{6, 6, 2, 65},
{1, 17, 0, -5},
{1, 18, 0, -5},
{1, 19, 0, -5},
{1, 20, 0, -5},
{1, 21, 0, -5},
{1, 22, 0, -5},
{1, 23, 0, -5},
{1, 24, 0, -5},
{0, 8, 1, -4},
{1, 25, 0, 70},
{1, 26, 0, 70},
{5, 16, 1, -5},
{0, 7, 2, -4},
{1, 9, 6, -3},
};
static struct SXP_bases t_bases[]={
{0, 62, 65},
{0, 46, 2},
{0, 42, 1},
{0, 32, 2},
{0, -10, 21},
{-11, 42, 1},
{0, 60, 2},
{0, 50, 2},
{0, 81, 63},
{0, 38, 2},
{-22, 81, 0},
{0, 55, 4},
{0, 66, 0},
{0, 25, 4},
{0, 67, 0},
{0, 62, 64},
{0, 56, 5},
{0, 57, 4},
{0, 58, 3},
{0, 27, 3},
{0, 61, 21},
};
static struct SXP_bases nt_bases[]={
{-30, 0, 0},
{-29, 0, 0},
{-1, 71, 0},
};
static struct SXP_item vector[]={
{0, 0},
{2, 33},
{0, 0},
{3, 13},
{3, 26},
{3, 12},
{2, 46},
{0, 0},
{4, 55},
{1, -43},
{2, -28},
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
{19, 23},
{20, 24},
{6, 34},
{7, 35},
{8, 36},
{9, 37},
{10, 52},
{11, 14},
{12, 15},
{13, 16},
{14, 17},
{15, 18},
{16, 19},
{17, 20},
{18, 21},
{0, 0},
{0, 0},
{0, 0},
};
static struct SXP_goto gotos[]={
{-2, 6},
{-5, 9},
{-5, 11},
{62, 16},
{-59, 7},
{-31, 10},
{-6, 13},
{-7, 13},
{-8, 13},
{-9, 13},
{63, 15},
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
1,3};

static char *err_titles[SXSEVERITIES]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};
static char abstract []= "%d warnings and %d errors are reported.";

static unsigned char S_char_to_simple_class[]={
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,3,3,3,3,3,3,3,3,3,4,5,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,4,3,6,3,3,3,3,3,7,8,3,3,3,9,10,
3,3,3,3,3,3,3,3,3,3,3,11,12,3,3,3,13,3,14,14,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,3,15,3,3,3,3,
14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
14,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,};
static SXSTMI SXS_transition[]={
0x0,0x4015,0x0,0x2402,0x2402,0x2403,0x9004,0x9401,0x0,0x4405,0x2c04,0x2405,
0x0,0x9c02,0x0,
0x0,0x4000,0x0,0x1402,0x1402,0x4000,0x4000,0x4000,0x0,0x4000,0x4000,0x2405,
0x0,0x4000,0x0,
0x0,0x0,0x1c03,0x1c03,0x1c03,0x4404,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,0x1c03,
0x1c03,0x1c03,0x2407,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x9c02,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2408,
0x0,0x0,0x0,
0x0,0x0,0x1406,0x1406,0x1406,0x240a,0x9406,0x9009,0x1406,0x1406,0x1406,0x1406,
0x1406,0x1406,0x1406,
0x0,0x0,0x0,0x0,0x0,0x2c03,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x2c03,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x240b,
0x0,0x0,0x0,
0x0,0x0,0x0,0x3000,0x3000,0x3000,0x3000,0x3000,0x2c04,0x0,0x3000,0x3000,
0x3c00,0x9c02,0x0,
0x0,0x0,0x140a,0x140a,0x140a,0x2406,0x140a,0x140a,0x140a,0x140a,0x140a,0x140a,
0x140a,0x140a,0x240c,
0x0,0x0,0x140b,0x140b,0x2402,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,0x140b,
0x140b,0x140b,0x140b,
0x0,0x0,0x0,0x0,0x0,0x240a,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x240a,
};
static SXSTMI *S_transition_matrix[]={
&SXS_transition[0]-1,
&SXS_transition[15]-1,
&SXS_transition[30]-1,
&SXS_transition[45]-1,
&SXS_transition[60]-1,
&SXS_transition[75]-1,
&SXS_transition[90]-1,
&SXS_transition[105]-1,
&SXS_transition[120]-1,
&SXS_transition[135]-1,
&SXS_transition[150]-1,
&SXS_transition[165]-1,
};
static struct SXS_action_or_prdct_code SXS_action_or_prdct_code[]={
{16387,1,0,0,0},
{8201,2,0,1,0},
{16386,12,1,1,0},
{37891,2,0,1,1},
{9222,0,0,1,1},
{8198,12,1,1,0},
{16385,3,0,1,0},
{8198,13,1,1,0},
{37896,2,1,1,1},
{37895,0,0,1,1},
};
static char *S_adrp[]={
"feature",
"(",
")",
"%string",
".",
":footp",
":headp",
":substp",
":display-feature?",
":constraints",
":constraint-type",
":na",
":oa",
":none",
":dummy",
":connector",
":line",
":shape",
"t",
"nil",
"End Of File",
};
static short S_is_a_keyword[]={
15,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
static short S_is_a_generic_terminal[]={
1,4};
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
{12,1},{0,2},{14,4},{13,5},{0,6},{8,8},{0,9},{0,10},{7,11},{0,13},{2,15},{3,16},
{4,17},{11,18},{0,19},{10,20},{16,21},{5,21},{9,21},{16,21},{16,21},{16,21},{16,21},{16,21},
{15,21},{6,21},{0,21},};
static short T_ter_to_node_name[]={
0,0,0,0,17,};
static char *T_node_name[]={
"ERROR",
"ATTRIBUT_S",
"FOOTP",
"HEADP",
"NA",
"NIL",
"NODE_NAME",
"NODE_S",
"OA",
"SA",
"SUBSTP",
"TAG",
"TREE",
"TREE_S",
"TRUE",
"VOID",
"string",
};
extern SXSEMPASS_FUNCTION SEMPASS;
static char T_stack_schema[]={
0,0,1,0,1,0,1,1,1,0,2,4,0,1,0,0,0,0,0,0,0,};

static struct SXT_tables SXT_tables={
SXT_node_info-1, T_ter_to_node_name, T_stack_schema-1, SEMPASS, T_node_name-1
};
extern SXSEMACT_FUNCTION sxatc;
static SXCHECK_KEYWORD_FUNCTION sxcheck_keyword;

SXTABLES sxtables={
56431012, /* magic */
sxscanner,
sxparser, 
{255, 21, 1, 3, 4, 15, 2, 1, 1, 
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
sxcheck_keyword
},
{9, 27, 27, 38, 58, 61, 63, 102, 21, 9, 26, 21, 9, 4, 7, 2, 5, 11, 5, 4,
reductions-1,
t_bases-1,
nt_bases-1,
vector-63,
gotos-28,
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
NULL,
sxatc},
err_titles,
abstract,
(sxsem_tables*)&SXT_tables
};
/* ********************************************************************
   *  This program has been generated from xtag.lecl                  *
   *  on Fri Dec 30 13:04:44 1994                                     *
   *  by the SYNTAX (*) lexical constructor LECL                      *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */



static SXINT sxcheck_keyword (char *string, SXINT length)
{
   register int  t_code, delta;
   register char *keyword;

   switch (length) {
   case 1:
      return *string == 't' ? 19 /* "t" */: 0;

   case 3:
      switch (*string++) {
      case ':':
         switch (*string++) {
         case 'n':
            return *string == 'a' ? 12 /* ":na" */: 0;

         case 'o':
            return *string == 'a' ? 13 /* ":oa" */: 0;

         default:
            return 0;
         }

      case 'n':
         return *string++ == 'i' && *string == 'l' ? 20 /* "nil" */: 0;

      default:
         return 0;
      }

   case 5:
      switch (string [1]) {
      case 'l':
         t_code = 17 /* ":line" */;
         goto cmp_0_0;

      case 'n':
         t_code = 14 /* ":none" */;
         goto cmp_0_0;

      default:
         return 0;
      }

   case 6:
      switch (string [5]) {
      case 'e':
         t_code = 18 /* ":shape" */;
         goto cmp_1_0;

      case 'p':
         switch (string [4]) {
         case 'd':
            t_code = 7 /* ":headp" */;
            goto cmp_2_0;

         case 't':
            t_code = 6 /* ":footp" */;
            goto cmp_2_0;

         default:
            return 0;
         }

      case 'y':
         t_code = 15 /* ":dummy" */;
         goto cmp_1_0;

      default:
         return 0;
      }

   case 7:
      t_code = 8 /* ":substp" */;
      goto cmp_0_0;

   case 10:
      t_code = 16 /* ":connector" */;
      goto cmp_0_0;

   case 12:
      t_code = 10 /* ":constraints" */;
      goto cmp_0_0;

   case 16:
      t_code = 11 /* ":constraint-type" */;
      goto cmp_0_0;

   case 17:
      t_code = 9 /* ":display-feature?" */;
      goto cmp_0_0;

   default:
      return 0;
   }

cmp_2_0:
   length -= 2;
   delta = 0;
   goto compare;

cmp_1_0:
   length -= 1;
   delta = 0;
   goto compare;

cmp_0_0:
   delta = 0;
compare:
   keyword = sxtables.SXS_tables.S_adrp [t_code] + delta;

   do {
      if (*string++ != *keyword++)
          return 0;
   } while (--length > 0) ;

   return t_code;

}

/* End of sxtables for xtag [Fri Dec 30 13:05:00 1994] */
