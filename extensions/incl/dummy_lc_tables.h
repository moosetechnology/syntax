/* ************************************************************************************************
	This (partial) C tables has been extracted on Sunday, 30 June, 2024 at 10:17:22
	by the SYNTAX(*) BNF processor running with the "-huge" option
	from the CFG "dummy" in BNF form last modified on Saturday, 29 June, 2024 at 18:29:06
***************************************************************************************************
	(*) SYNTAX is a trademark of INRIA.
*************************************************************************************************** */



#define LANGUAGE_NAME "dummy"

#define HUGE_CFG

#define bnf_modif_time	1719678546

#define ntmax      (SXINT)1
#define tmax       (SXINT)(-1)
#define EOF_CODE   (SXINT)1
#define itemmax    (SXINT)4
#define prodmax    (SXINT)1
#define is_epsilon true


#ifdef def_lispro
static SXINT lispro [5] = {
/* 0 */ -1,
/* 1 */ 1,
/* 2 */ -1,
/* 3 */ 0,
/* 4 */ 0,
};
#endif /* def_lispro */


#ifdef def_prolis
static SXINT prolis [5] = {
/* 0 */ 0,
/* 1 */ 0,
/* 2 */ 0,
/* 3 */ 0,
/* 4 */ 1,
};
#endif /* def_prolis */


#ifdef def_prolon
static SXINT prolon [3] = {
/* 0 */ 1,
/* 1 */ 4,
/* 2 */ 5,
};
#endif /* def_prolon */
#define rhs_lgth    2
#define rhs_maxnt    rhs_lgth /* ATTENTION : valeur non defaut !! */


#ifdef def_lhs
static SXINT lhs [2] = {
/* 0 */ 0,
/* 1 */ 1,
};
#endif /* def_lhs */


#ifdef def_item2nbt
static SXINT item2nbt [] = {
/* 0 */	0, 0, 1,
/* 1 */	0,
};
#endif /* def_item2nbt */


#ifdef def_BVIDE

static  SXBA_ELT BVIDE
#if SXBITS_PER_LONG==32
 [3] = {2, 0X00000002, 0X00000000, }
#else
 [2] = {2, 0X0000000000000002, }
#endif
/* End BVIDE */;
#endif /* def_BVIDE */


#ifdef def_tstring
static char *tstring [2] = {
/* 0 */ "",
/* -1 */ "END OF FILE",
};
#endif /* def_tstring */


#ifdef def_sxdfa_comb_inflected_form
#include "fsa.h"

static SXUINT sxdfa_comb_inflected_form_comb_vector [3] = {
/* 0 */ 12, 0, 0, 
};
static unsigned char sxdfa_comb_inflected_form_char2class [256] = {
/* 0 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 10 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 20 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 30 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 40 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 50 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 60 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 70 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 80 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 90 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 100 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 110 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 120 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 130 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 140 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 150 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 160 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 170 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 180 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 190 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 200 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 210 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 220 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 230 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 240 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 250 */ '\000', '\000', '\000', '\000', '\000', '\000', 
};

static struct sxdfa_comb sxdfa_comb_inflected_form = {
2 /* max */, 0 /* init_base */, 3 /* class_mask */, 4 /* stop_mask */, 3 /* base_shift */,
sxdfa_comb_inflected_form_char2class, sxdfa_comb_inflected_form_comb_vector,
true /* is_static */, true /* is_a_dag */,
NULL /* base2stack */, NULL /* char_stack_list */, 0,
"sxdfa_comb_inflected_form" /* name */, 0,
{true /* from_left_to_right */, {NULL} /* private ptr */}
};
#endif /* def_sxdfa_comb_inflected_form */


#ifdef def_sxdfa_comb_non_terminal_names
#include "fsa.h"

static SXUINT sxdfa_comb_non_terminal_names_comb_vector [3] = {
/* 0 */ 12, 0, 0, 
};
static unsigned char sxdfa_comb_non_terminal_names_char2class [256] = {
/* 0 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 10 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 20 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 30 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 40 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 50 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 60 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 70 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 80 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 90 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 100 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 110 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 120 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 130 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 140 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 150 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 160 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 170 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 180 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 190 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 200 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 210 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 220 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 230 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 240 */ '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', '\000', 
/* 250 */ '\000', '\000', '\000', '\000', '\000', '\000', 
};

static struct sxdfa_comb sxdfa_comb_non_terminal_names = {
2 /* max */, 0 /* init_base */, 3 /* class_mask */, 4 /* stop_mask */, 3 /* base_shift */,
sxdfa_comb_non_terminal_names_char2class, sxdfa_comb_non_terminal_names_comb_vector,
true /* is_static */, true /* is_a_dag */,
NULL /* base2stack */, NULL /* char_stack_list */, 0,
"sxdfa_comb_non_terminal_names" /* name */, 0,
{true /* from_left_to_right */, {NULL} /* private ptr */}
};
#endif /* def_sxdfa_comb_non_terminal_names */


#ifdef def_ntstring
static char *ntstring [2] = {
/* 0 */ "",
/* 1 */ "S",
};
#endif /* def_ntstring */


#ifdef def_multiple_t_prod_set
#define multiple_anchor_prod_nb 0
static SXBA multiple_t_prod_set;
#endif /* def_multiple_t_prod_set */


#ifdef def_t2prod_hd

static  SXBA_ELT unlexicalized_prod_set
#if SXBITS_PER_LONG==32
 [3] = {2, 0X00000002, 0X00000000, }
#else
 [2] = {2, 0X0000000000000002, }
#endif
/* End unlexicalized_prod_set */;
static SXINT t2prod_hd [3] = {
/* 0 */ 0,
/* 1 */ 0,
/* 2 */ 1,
};
static SXINT t2prod_list [2] = {
/* unlexicalized prods */ 
/* 1 "END OF FILE" */ 0, 
};
#endif /* def_t2prod_hd */


#ifdef def_nt2gen_t_set
#if SXBITS_PER_LONG==32 
static SXBA_ELT nt2gen_t_set_0 [3] = {2, 0X00000000, 0X00000000, };
#else
static SXBA_ELT nt2gen_t_set_0 [2] = {2, 0X0000000000000000, };
#endif


static  SXBA nt2gen_t_set [2] = {
/* 0 */ nt2gen_t_set_0, NULL, } /* End nt2gen_t_set */;
static SXINT *nt2gen_t_list;
static SXINT *gen_t_list;
#endif /* def_nt2gen_t_set */


#ifdef def_npg
static SXINT npg [3] = {
/* 0 */ 0,
/* 1 */ 1,
/* 2 */ 2,
};
#endif /* def_npg */


#ifdef def_numpg
static SXINT numpg [3] = {
/* 0 */ 0,
/* 1 */ 1,
/* 2 */ 0,
};
#endif /* def_numpg */


#ifdef def_npd
static SXINT npd [3] = {
/* 0 */ 0,
/* 1 */ 1,
/* 2 */ 3,
};
#endif /* def_npd */


#ifdef def_numpd
static SXINT numpd [4] = {
/* 0 */ 0,
/* 1 */ 1,
/* 2 (frontiere) */ 0,
/* 3 */ 0,
};
#endif /* def_numpd */


#ifdef def_tpd
static SXINT tpd [3] = {
/* 0 */ 0,
/* -1 */ 1,
/* -2 */ 2,
};
#endif /* def_tpd */


#ifdef def_tnumpd
static SXINT tnumpd [3] = {
/* 0 */ 0,
/* 1 */ 2,
/* 2 */ 0,
};
#endif /* def_tnumpd */


#ifdef def_segment
static SXINT segment [5] = {
/* prod=0 */ 1, 0, -1, 0,
/* prod=1 */ 0,
};
static SXINT segment_list [3] = {0,
/* seg_id=1 */ 1, 0,
};
#endif /* def_segment */
