char WHAT[] = "@(#)SLC(1)/Earley automaton for wcw - SYNTAX [unix] - Tue Nov 19 15:41:58 1996";

#if 0
static char	ME [] = "wcw_earley_lc_parser";
#endif

#include "sxunix.h"

#define ntmax	2
#define tmax	(-4)
#define itemmax	25
#define prodmax	8
#define initial_state	0
#define rhs_lgth	3
#define rhs_maxnt	1
#define is_right_recursive	1
#define is_cyclic	0
#define is_epsilon	0
#define is_constraints	0
#define is_prdct	0


static int lispro [] = {
/* 0 [0..3] */	-4, 1, -4, 0,
/* 1 [4..6] */	1, -1, 0,
/* 2 [7..9] */	1, -2, 0,
/* 3 [10..12] */	1, -3, 0,
/* 4 [13..14] */	2, 0,
/* 5 [15..17] */	-1, 2, 0,
/* 6 [18..20] */	-2, 2, 0,
/* 7 [21..23] */	-3, 2, 0,
/* 8 [24..25] */	-3, 0,
};


static int prolis [] = {
0, 0, 0, 0,
1, 1, 1,
2, 2, 2,
3, 3, 3,
4, 4,
5, 5, 5,
6, 6, 6,
7, 7, 7,
8, 8,
};


static int prolon [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	7,
/* 3 */	10,
/* 4 */	13,
/* 5 */	15,
/* 6 */	18,
/* 7 */	21,
/* 8 */	24,
/* 9 */	26,
};


static int lhs [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	1,
/* 3 */	1,
/* 4 */	1,
/* 5 */	2,
/* 6 */	2,
/* 7 */	2,
/* 8 */	2,
};

#if 0
static int semact [] = {
/* 0 */	0,
/* 1 */	0,
/* 2 */	0,
/* 3 */	0,
/* 4 */	0,
/* 5 */	0,
/* 6 */	0,
/* 7 */	0,
/* 8 */	0,
};
#endif

static int nt2order [] = {0,
/* 1 = <S> */	1,
/* 2 = <T> */	2,
};


static int order2nt [] = {0,
/* 1 */	1 /* <S> */,
/* 2 */	2 /* <T> */,
};

#if 0
static int rhs_nt2where [] = {
/* 0 */	0 /* terminals */,
/* 1 */	9 /* <S> */,
/* 2 */	13 /* <T> */,
};

static int lhs_nt2where [] = {
/* 0 */	17 /* <Super Start Symbol> */,
/* 1 */	18 /* <S> */,
/* 2 */	22 /* <T> */,
};
#endif

static  SXBA_ELT t2item_set [5]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {26, 0X03FFFFF2, },
/* 1 */ {26, 0X02DBF6F2, },
/* 2 */ {26, 0X02DF77D2, },
/* 3 */ {26, 0X03FB7ED2, },
/* 4 */ {26, 0X0292524D, },
#else
 [2] = {
/* 0 */ {26, 0X0000000003FFFFF2, },
/* 1 */ {26, 0X0000000002DBF6F2, },
/* 2 */ {26, 0X0000000002DF77D2, },
/* 3 */ {26, 0X0000000003FB7ED2, },
/* 4 */ {26, 0X000000000292524D, },
#endif
} /* End t2item_set */;

#if 0
static  SXBA_ELT left_corner [3]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {3, 0X00000000, },
/* 1 */ {3, 0X00000006, },
/* 2 */ {3, 0X00000004, },
#else
 [2] = {
/* 0 */ {3, 0X0000000000000000, },
/* 1 */ {3, 0X0000000000000006, },
/* 2 */ {3, 0X0000000000000004, },
#endif
} /* End left_corner */;
#endif

static  SXBA_ELT nt2item_set [3]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {26, 0X00000000, },
/* 1 */ {26, 0X0124A490, },
/* 2 */ {26, 0X01248000, },
#else
 [2] = {
/* 0 */ {26, 0X0000000000000000, },
/* 1 */ {26, 0X000000000124A490, },
/* 2 */ {26, 0X0000000001248000, },
#endif
} /* End nt2item_set */;

#if is_parser


static int item2nbnt [] = {
/* 0 */	0, 0, 1, 1,
/* 1 */	0, 1, 1,
/* 2 */	0, 1, 1,
/* 3 */	0, 1, 1,
/* 4 */	0, 1,
/* 5 */	0, 0, 1,
/* 6 */	0, 0, 1,
/* 7 */	0, 0, 1,
/* 8 */	0, 0,
};


static int item2nbt [] = {
/* 0 */	0, 1, 0, 1,
/* 1 */	0, 0, 1,
/* 2 */	0, 0, 1,
/* 3 */	0, 0, 1,
/* 4 */	0, 0,
/* 5 */	0, 1, 0,
/* 6 */	0, 1, 0,
/* 7 */	0, 1, 0,
/* 8 */	0, 1,
};


static int prod2nbnt [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	1,
/* 3 */	1,
/* 4 */	1,
/* 5 */	1,
/* 6 */	1,
/* 7 */	1,
/* 8 */	0,
};


static int prodXnt2nbt [prodmax+1] [rhs_maxnt+1] = {
/* 0 */	{1, 1},
/* 1 */	{1, 0},
/* 2 */	{1, 0},
/* 3 */	{1, 0},
/* 4 */	{0, 0},
/* 5 */	{0, 1},
/* 6 */	{0, 1},
/* 7 */	{0, 1},
/* 8 */	{1, 0},
};


static int prod2order [] = {
/* 0 */	0,
/* 1 */	8,
/* 2 */	7,
/* 3 */	6,
/* 4 */	1,
/* 5 */	4,
/* 6 */	3,
/* 7 */	2,
/* 8 */	5,
};


static int order2prod [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	7,
/* 3 */	6,
/* 4 */	5,
/* 5 */	8,
/* 6 */	3,
/* 7 */	2,
/* 8 */	1,
};

#if 0
static int prod_order2min [] = {
/* 0 */	0,
/* 1 */	0,
/* 2 */	2,
/* 3 */	2,
/* 4 */	2,
/* 5 */	0,
/* 6 */	0,
/* 7 */	0,
/* 8 */	0,
};

static int prod_order2max [] = {
/* 0 */	0,
/* 1 */	0,
/* 2 */	4,
/* 3 */	4,
/* 4 */	4,
/* 5 */	0,
/* 6 */	0,
/* 7 */	0,
/* 8 */	0,
};
#endif

static char *ntstring [] = {"",
/* 1 */	"S",
/* 2 */	"T",
};

#if 0
static char *tstring [] = {"",
/* 1 */	"a",
/* 2 */	"b",
/* 3 */	"c",
/* 4 */	"END OF FILE",
};
#endif
#endif

#include "sxparser_earley_lc.c"
