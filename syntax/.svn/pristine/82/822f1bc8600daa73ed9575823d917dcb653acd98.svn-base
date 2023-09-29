#define WHAT	"@(#)SLC(1)/Earley automaton for anbncn - SYNTAX [unix] - Tue Jan 28 16:28:37 1997"

#if 0
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};
static char	ME [] = "anbncn_earley_lc_parser";
#endif

#include "sxunix.h"

#define ntmax	2
#define tmax	(-4)
#define itemmax	13
#define prodmax	4
#define initial_state	0
#define rhs_lgth	3
#define rhs_maxnt	1
#define is_right_recursive	0
#define is_cyclic	0
#define is_epsilon	1
#define is_constraints	0
#define is_prdct	0


static int lispro [] = {
/* 0 [0..3] */	-4, 1, -4, 0,
/* 1 [4..6] */	1, -1, 0,
/* 2 [7..8] */	2, 0,
/* 3 [9..12] */	-2, 2, -3, 0,
/* 4 [13..13] */	0,
};


static int prolis [] = {
0, 0, 0, 0,
1, 1, 1,
2, 2,
3, 3, 3, 3,
4,
};


static int prolon [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	7,
/* 3 */	9,
/* 4 */	13,
/* 5 */	14,
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
/* 1 = <S> */	1,
/* 2 = <AB> */	2,
};


static int order2nt [] = {0,
/* 1 */	1 /* <S> */,
/* 2 */	2 /* <AB> */,
};


static int rhs_nt2where [] = {
/* 0 */	0 /* terminals */,
/* 1 */	5 /* <S> */,
/* 2 */	7 /* <AB> */,
};


static int lhs_nt2where [] = {
/* 0 */	9 /* <Super Start Symbol> */,
/* 1 */	10 /* <S> */,
/* 2 */	12 /* <AB> */,
};

static  SXBA_ELT t2item_set [5]
#if SXBITS_PER_LONG==32
 [2] = {
/* 0 */ {14, 0X00003FF2, },
/* 1 */ {14, 0X000031F2, },
/* 2 */ {14, 0X00000692, },
/* 3 */ {14, 0X00003C00, },
/* 4 */ {14, 0X000031CF, },
#else
 [2] = {
/* 0 */ {14, 0X0000000000003FF2, },
/* 1 */ {14, 0X00000000000031F2, },
/* 2 */ {14, 0X0000000000000692, },
/* 3 */ {14, 0X0000000000003C00, },
/* 4 */ {14, 0X00000000000031CF, },
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
/* 0 */ {14, 0X00000000, },
/* 1 */ {14, 0X000023B0, },
/* 2 */ {14, 0X00002200, },
#else
 [2] = {
/* 0 */ {14, 0X0000000000000000, },
/* 1 */ {14, 0X00000000000023B0, },
/* 2 */ {14, 0X0000000000002200, },
#endif
} /* End nt2item_set */;

static  SXBA_ELT BVIDE
#if SXBITS_PER_LONG==32
 [2] = {3, 0X00000006, }
#else
 [2] = {3, 0X0000000000000006, }
#endif
/* End BVIDE */;

#if is_parser


static int item2nbnt [] = {
/* 0 */	0, 0, 1, 1,
/* 1 */	0, 1, 1,
/* 2 */	0, 1,
/* 3 */	0, 0, 1, 1,
/* 4 */	0,
};


static int item2nbt [] = {
/* 0 */	0, 1, 0, 1,
/* 1 */	0, 0, 1,
/* 2 */	0, 0,
/* 3 */	0, 1, 0, 1,
/* 4 */	0,
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
/* 1 */	{1, 0},
/* 2 */	{0, 0},
/* 3 */	{1, 1},
/* 4 */	{0, 0},
};


static int prod2order [] = {
/* 0 */	0,
/* 1 */	4,
/* 2 */	1,
/* 3 */	3,
/* 4 */	2,
};


static int order2prod [] = {
/* 0 */	0,
/* 1 */	2,
/* 2 */	4,
/* 3 */	3,
/* 4 */	1,
};


static char *ntstring [] = {"",
/* 1 */	"S",
/* 2 */	"AB",
};


static char *tstring [] = {"",
/* 1 */	"c",
/* 2 */	"a",
/* 3 */	"b",
/* 4 */	"END OF FILE",
};

#endif


#include "sxparser_earley_lc.c"
