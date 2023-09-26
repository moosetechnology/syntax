#define WHAT	"@(#)SLC(1)/Earley automaton for anbmcndm - SYNTAX [unix] - Mon Dec 16 15:54:07 1996"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

static char	ME [] = "anbmcndm_earley_lc_parser";

#include "sxunix.h"

#define ntmax	3
#define tmax	(-5)
#define itemmax	18
#define prodmax	6
#define initial_state	0
#define rhs_lgth	3
#define rhs_maxnt	1
#define is_right_recursive	1
#define is_cyclic	0
#define is_epsilon	1
#define is_constraints	0
#define is_prdct	0


static int lispro [] = {
/* 0 [0..3] */	-5, 1, -5, 0,
/* 1 [4..6] */	1, -1, 0,
/* 2 [7..8] */	2, 0,
/* 3 [9..12] */	-2, 2, -3, 0,
/* 4 [13..14] */	3, 0,
/* 5 [15..17] */	-4, 3, 0,
/* 6 [18..18] */	0,
};


static int prolis [] = {
0, 0, 0, 0,
1, 1, 1,
2, 2,
3, 3, 3, 3,
4, 4,
5, 5, 5,
6,
};


static int prolon [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	7,
/* 3 */	9,
/* 4 */	13,
/* 5 */	15,
/* 6 */	18,
/* 7 */	19,
};


static int lhs [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	1,
/* 3 */	2,
/* 4 */	2,
/* 5 */	3,
/* 6 */	3,
};


static int semact [] = {
/* 0 */	0,
/* 1 */	0,
/* 2 */	0,
/* 3 */	0,
/* 4 */	0,
/* 5 */	0,
/* 6 */	0,
};


static int nt2order [] = {0,
/* 1 = <S> */	1,
/* 2 = <T> */	2,
/* 3 = <B> */	3,
};


static int order2nt [] = {0,
/* 1 */	1 /* <S> */,
/* 2 */	2 /* <T> */,
/* 3 */	3 /* <B> */,
};


static int rhs_nt2where [] = {
/* 0 */	0 /* terminals */,
/* 1 */	6 /* <S> */,
/* 2 */	8 /* <T> */,
/* 3 */	10 /* <B> */,
};


static int lhs_nt2where [] = {
/* 0 */	12 /* <Super Start Symbol> */,
/* 1 */	13 /* <S> */,
/* 2 */	15 /* <T> */,
/* 3 */	17 /* <B> */,
};

static  SXBA_ELT t2item_set [6]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {19, 0X0007FFF2, },
/* 1 */ {19, 0X000771F2, },
/* 2 */ {19, 0X00000692, },
/* 3 */ {19, 0X00077C00, },
/* 4 */ {19, 0X0001A492, },
/* 5 */ {19, 0X000771CF, },
#else
 [2] = {
/* 0 */ {19, 0X000000000007FFF2, },
/* 1 */ {19, 0X00000000000771F2, },
/* 2 */ {19, 0X0000000000000692, },
/* 3 */ {19, 0X0000000000077C00, },
/* 4 */ {19, 0X000000000001A492, },
/* 5 */ {19, 0X00000000000771CF, },
#endif
} /* End t2item_set */;

static  SXBA_ELT left_corner [4]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {4, 0X00000000, },
/* 1 */ {4, 0X0000000E, },
/* 2 */ {4, 0X0000000C, },
/* 3 */ {4, 0X00000008, },
#else
 [2] = {
/* 0 */ {4, 0X0000000000000000, },
/* 1 */ {4, 0X000000000000000E, },
/* 2 */ {4, 0X000000000000000C, },
/* 3 */ {4, 0X0000000000000008, },
#endif
} /* End left_corner */;

static  SXBA_ELT nt2item_set [4]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {19, 0X00000000, },
/* 1 */ {19, 0X0004E3B0, },
/* 2 */ {19, 0X0004E200, },
/* 3 */ {19, 0X00048000, },
#else
 [2] = {
/* 0 */ {19, 0X0000000000000000, },
/* 1 */ {19, 0X000000000004E3B0, },
/* 2 */ {19, 0X000000000004E200, },
/* 3 */ {19, 0X0000000000048000, },
#endif
} /* End nt2item_set */;

static  SXBA_ELT BVIDE
#if SXBITS_PER_LONG==32
 [2] = {4, 0X0000000E, }
#else
 [2] = {4, 0X000000000000000E, }
#endif
/* End BVIDE */;

#if is_parser


static int item2nbnt [] = {
/* 0 */	0, 0, 1, 1,
/* 1 */	0, 1, 1,
/* 2 */	0, 1,
/* 3 */	0, 0, 1, 1,
/* 4 */	0, 1,
/* 5 */	0, 0, 1,
/* 6 */	0,
};


static int item2nbt [] = {
/* 0 */	0, 1, 0, 1,
/* 1 */	0, 0, 1,
/* 2 */	0, 0,
/* 3 */	0, 1, 0, 1,
/* 4 */	0, 0,
/* 5 */	0, 1, 0,
/* 6 */	0,
};


static int prod2nbnt [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	1,
/* 3 */	1,
/* 4 */	1,
/* 5 */	1,
/* 6 */	0,
};


static int prodXnt2nbt [prodmax+1] [rhs_maxnt+1] = {
/* 0 */	{1, 1},
/* 1 */	{1, 0},
/* 2 */	{0, 0},
/* 3 */	{1, 1},
/* 4 */	{0, 0},
/* 5 */	{0, 1},
/* 6 */	{0, 0},
};


static int prod2order [] = {
/* 0 */	0,
/* 1 */	6,
/* 2 */	1,
/* 3 */	5,
/* 4 */	2,
/* 5 */	3,
/* 6 */	4,
};


static int order2prod [] = {
/* 0 */	0,
/* 1 */	2,
/* 2 */	4,
/* 3 */	5,
/* 4 */	6,
/* 5 */	3,
/* 6 */	1,
};


static int prod_order2min [] = {
/* 0 */	0,
/* 1 */	0,
/* 2 */	0,
/* 3 */	3,
/* 4 */	0,
/* 5 */	0,
/* 6 */	0,
};


static int prod_order2max [] = {
/* 0 */	0,
/* 1 */	0,
/* 2 */	0,
/* 3 */	3,
/* 4 */	0,
/* 5 */	0,
/* 6 */	0,
};


static char *ntstring [] = {"",
/* 1 */	"S",
/* 2 */	"T",
/* 3 */	"B",
};


static char *tstring [] = {"",
/* 1 */	"d",
/* 2 */	"a",
/* 3 */	"c",
/* 4 */	"b",
/* 5 */	"END OF FILE",
};

#endif


#include "sxparser_earley_lc.h"
