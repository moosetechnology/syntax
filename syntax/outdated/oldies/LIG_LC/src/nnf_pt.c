#define WHAT	"@(#)SLC(1)/Earley automaton for nnf - SYNTAX [unix] - Tue Nov 19 17:55:18 1996"

#if 0
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};
static char	ME [] = "nnf_earley_lc_parser";
#endif

#include "sxunix.h"

#define ntmax	5
#define tmax	(-3)
#define itemmax	15
#define prodmax	5
#define initial_state	0
#define rhs_lgth	3
#define rhs_maxnt	3
#define is_right_recursive	0
#define is_cyclic	0
#define is_epsilon	0
#define is_constraints	0
#define is_prdct	0


static int lispro [] = {
/* 0 [0..3] */	-3, 1, -3, 0,
/* 1 [4..5] */	2, 0,
/* 2 [6..9] */	3, 4, 5, 0,
/* 3 [10..11] */	-1, 0,
/* 4 [12..13] */	-2, 0,
/* 5 [14..15] */	3, 0,
};


static int prolis [] = {
0, 0, 0, 0,
1, 1,
2, 2, 2, 2,
3, 3,
4, 4,
5, 5,
};


static int prolon [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	6,
/* 3 */	10,
/* 4 */	12,
/* 5 */	14,
/* 6 */	16,
};


static int lhs [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	2,
/* 3 */	3,
/* 4 */	4,
/* 5 */	5,
};


static int semact [] = {
/* 0 */	0,
/* 1 */	0,
/* 2 */	0,
/* 3 */	0,
/* 4 */	0,
/* 5 */	0,
};


static int nt2order [] = {0,
/* 1 = <S> */	1,
/* 2 = <A> */	5,
/* 3 = <X> */	4,
/* 4 = <B> */	3,
/* 5 = <Y> */	2,
};


static int order2nt [] = {0,
/* 1 */	1 /* <S> */,
/* 2 */	5 /* <A> */,
/* 3 */	4 /* <X> */,
/* 4 */	3 /* <B> */,
/* 5 */	2 /* <Y> */,
};


static int rhs_nt2where [] = {
/* 0 */	0 /* terminals */,
/* 1 */	4 /* <S> */,
/* 2 */	5 /* <A> */,
/* 3 */	6 /* <X> */,
/* 4 */	8 /* <B> */,
/* 5 */	9 /* <Y> */,
};


static int lhs_nt2where [] = {
/* 0 */	10 /* <Super Start Symbol> */,
/* 1 */	11 /* <S> */,
/* 2 */	12 /* <A> */,
/* 3 */	13 /* <X> */,
/* 4 */	14 /* <B> */,
/* 5 */	15 /* <Y> */,
};

static  SXBA_ELT t2item_set [4]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {16, 0X00007DD2, },
/* 1 */ {16, 0X00006552, },
/* 2 */ {16, 0X00001880, },
/* 3 */ {16, 0X00008A2D, },
#else
 [2] = {
/* 0 */ {16, 0X0000000000007DD2, },
/* 1 */ {16, 0X0000000000006552, },
/* 2 */ {16, 0X0000000000001880, },
/* 3 */ {16, 0X0000000000008A2D, },
#endif
} /* End t2item_set */;

static  SXBA_ELT left_corner [6]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {6, 0X00000000, },
/* 1 */ {6, 0X0000000E, },
/* 2 */ {6, 0X0000000C, },
/* 3 */ {6, 0X00000008, },
/* 4 */ {6, 0X00000010, },
/* 5 */ {6, 0X00000028, },
#else
 [2] = {
/* 0 */ {6, 0X0000000000000000, },
/* 1 */ {6, 0X000000000000000E, },
/* 2 */ {6, 0X000000000000000C, },
/* 3 */ {6, 0X0000000000000008, },
/* 4 */ {6, 0X0000000000000010, },
/* 5 */ {6, 0X0000000000000028, },
#endif
} /* End left_corner */;

static  SXBA_ELT nt2item_set [6]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {16, 0X00000000, },
/* 1 */ {16, 0X00000450, },
/* 2 */ {16, 0X00000440, },
/* 3 */ {16, 0X00000400, },
/* 4 */ {16, 0X00001000, },
/* 5 */ {16, 0X00004400, },
#else
 [2] = {
/* 0 */ {16, 0X0000000000000000, },
/* 1 */ {16, 0X0000000000000450, },
/* 2 */ {16, 0X0000000000000440, },
/* 3 */ {16, 0X0000000000000400, },
/* 4 */ {16, 0X0000000000001000, },
/* 5 */ {16, 0X0000000000004400, },
#endif
} /* End nt2item_set */;

#if is_parser


static int item2nbnt [] = {
/* 0 */	0, 0, 1, 1,
/* 1 */	0, 1,
/* 2 */	0, 1, 2, 3,
/* 3 */	0, 0,
/* 4 */	0, 0,
/* 5 */	0, 1,
};


static int item2nbt [] = {
/* 0 */	0, 1, 0, 1,
/* 1 */	0, 0,
/* 2 */	0, 0, 0, 0,
/* 3 */	0, 1,
/* 4 */	0, 1,
/* 5 */	0, 0,
};


static int prod2nbnt [] = {
/* 0 */	1,
/* 1 */	1,
/* 2 */	3,
/* 3 */	0,
/* 4 */	0,
/* 5 */	1,
};


static int prodXnt2nbt [prodmax+1] [rhs_maxnt+1] = {
/* 0 */	{1, 1, 0, 0},
/* 1 */	{0, 0, 0, 0},
/* 2 */	{0, 0, 0, 0},
/* 3 */	{1, 0, 0, 0},
/* 4 */	{1, 0, 0, 0},
/* 5 */	{0, 0, 0, 0},
};


static int prod2order [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	2,
/* 3 */	5,
/* 4 */	4,
/* 5 */	3,
};


static int order2prod [] = {
/* 0 */	0,
/* 1 */	1,
/* 2 */	2,
/* 3 */	5,
/* 4 */	4,
/* 5 */	3,
};


static char *ntstring [] = {"",
/* 1 */	"S",
/* 2 */	"A",
/* 3 */	"X",
/* 4 */	"B",
/* 5 */	"Y",
};


static char *tstring [] = {"",
/* 1 */	"a",
/* 2 */	"b",
/* 3 */	"END OF FILE",
};

#endif


#include "sxparser_earley_lc.c"
