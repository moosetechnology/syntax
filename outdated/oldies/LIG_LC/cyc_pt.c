char WHAT[] = "@(#)SLC(1)/Earley automaton for cyc - SYNTAX [unix] - Thu Nov 21 10:14:55 1996";

#if 0
static char	ME [] = "cyc_earley_lc_parser";
#endif

#include "sxunix.h"

#define ntmax	2
#define tmax	(-2)
#define itemmax	11
#define prodmax	4
#define initial_state	0
#define rhs_lgth	3
#define rhs_maxnt	1
#define is_right_recursive	1
#define is_cyclic	1
#define is_epsilon	0
#define is_constraints	0
#define is_prdct	0


static int lispro [] = {
/* 0 [0..3] */	-2, 1, -2, 0,
/* 1 [4..5] */	1, 0,
/* 2 [6..7] */	2, 0,
/* 3 [8..9] */	2, 0,
/* 4 [10..11] */	-1, 0,
};


static int prolis [] = {
0, 0, 0, 0,
1, 1,
2, 2,
3, 3,
4, 4,
};


static int prolon [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	6,
/* 3 */	8,
/* 4 */	10,
/* 5 */	12,
};


static int lhs [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	1,
/* 3 */	2,
/* 4 */	2,
};


static int semact [] = {
/* 0 */	0,
/* 1 */	0,
/* 2 */	0,
/* 3 */	0,
/* 4 */	0,
};


static int nt2order [] = {0,
/* 1 = <A> */	1,
/* 2 = <B> */	2,
};


static int order2nt [] = {0,
/* 1 */	1 /* <A> */,
/* 2 */	2 /* <B> */,
};


static int rhs_nt2where [] = {
/* 0 */	0 /* terminals */,
/* 1 */	3 /* <A> */,
/* 2 */	5 /* <B> */,
};


static int lhs_nt2where [] = {
/* 0 */	7 /* <Super Start Symbol> */,
/* 1 */	8 /* <A> */,
/* 2 */	10 /* <B> */,
};

static  SXBA_ELT t2item_set [3]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {12, 0X00000552, },
/* 1 */ {12, 0X00000552, },
/* 2 */ {12, 0X00000AAD, },
#else
 [2] = {
/* 0 */ {12, 0X0000000000000552, },
/* 1 */ {12, 0X0000000000000552, },
/* 2 */ {12, 0X0000000000000AAD, },
#endif
} /* End t2item_set */;

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

static  SXBA_ELT nt2item_set [3]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {12, 0X00000000, },
/* 1 */ {12, 0X00000550, },
/* 2 */ {12, 0X00000500, },
#else
 [2] = {
/* 0 */ {12, 0X0000000000000000, },
/* 1 */ {12, 0X0000000000000550, },
/* 2 */ {12, 0X0000000000000500, },
#endif
} /* End nt2item_set */;

#if is_parser


static int item2nbnt [] = {
/* 0 */	0, 0, 1, 1,
/* 1 */	0, 1,
/* 2 */	0, 1,
/* 3 */	0, 1,
/* 4 */	0, 0,
};


static int item2nbt [] = {
/* 0 */	0, 1, 0, 1,
/* 1 */	0, 0,
/* 2 */	0, 0,
/* 3 */	0, 0,
/* 4 */	0, 1,
};


static int prod2nbnt [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	1,
/* 3 */	1,
/* 4 */	0,
};


static int prodXnt2nbt [prodmax+1] [rhs_maxnt+1] = {
/* 0 */	{1, 1},
/* 1 */	{0, 0},
/* 2 */	{0, 0},
/* 3 */	{0, 0},
/* 4 */	{1, 0},
};


static int prod2order [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	2,
/* 3 */	3,
/* 4 */	4,
};


static int order2prod [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	2,
/* 3 */	3,
/* 4 */	4,
};


static int prod_order2min [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	0,
/* 3 */	3,
/* 4 */	0,
};


static int prod_order2max [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	0,
/* 3 */	3,
/* 4 */	0,
};


static int item2cycle_nb [] = {
/* 0 */	0, 0, 0, 0,
/* 1 */	1, 0,
/* 2 */	0, 0,
/* 3 */	2, 0,
/* 4 */	0, 0,
};


static char *ntstring [] = {"",
/* 1 */	"A",
/* 2 */	"B",
};


static char *tstring [] = {"",
/* 1 */	"a",
/* 2 */	"END OF FILE",
};

#endif


#include "sxparser_earley_lc.c"
