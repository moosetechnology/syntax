/* Idee pour le traitement "simultane" de la semantique.

   Le point d'entre "action" traite simultanement toutes les regles
   Aik -> X Y ... Z
   action (prod_no, i, j_set, k)
   ou j_set contient tous les indices j de Zjk, dernier symbole de la rhs.
   La fonction action doit retourner la ou les regles retenues apres evaluation
   sous la forme d'un ensemble de p-uplets ou p = |rhs|-1.  Chaque p-uplet
   (i1, i2, ..., ip) est tel que Aik -> Xii1 Yi1i2 ... Zipk est une regle valide
   (bien entendu ip \in j_set). On peut peut etre ne noter que les non-terminaux.

   Avantage on traite simultanement O(n) regles dont on ne conserve eventuellement
   qu'une seule (et peut etre aucune si la "contrainte" n'est pas verifiee.)
*/


/* Ordre ds l'execution des actions et predicats.

   On suppose que la grammaire est ;
      - non cyclique
      - epsilon-free (pour le moment)

   Condition :
      - un predicat associe au symbole X[i..j] ou une action associee a une regle
        dont X[i..j] est un symbole en RHS ne peuvent s'executer que s'il n'y a pas
	de reduction ulterieure produisant (en LHS) le symbole X[i..j].

   Theoreme :
      cette condition est verifiee si on impose les conditions suivantes :
      - les items de X[j] sont generes avant (et ont donc des identifiants inferieurs)
        a ceux de Y[l] si ;
	- j < l
	- ou j == l et X ->+ Y (chaine de reductions simples de X a Y).

   Demonstration :
      - soit (A -> ... X., h) de X[j]
      - soit (B -> ... Y., k) de Y[l]

      Par hypothese (A -> ... X., h) < (B -> ... Y., k) cad que la reduction B -> ... Y
      sera effectuee avant A -> ... X
      On suppose que cette sequence d'action est "lancee" apres le scan(/reduce) de ai.
      On montre que A[h..i] est different de Y[l..i].

      1) Si j < l
         on sait que h <= j => h < l et donc A[h..i] # Y[l..i]

      2) Si j < l
            - Si |... X| > 1 => h < j => h < l et donc A[h..i] # Y[l..i]
	    - Si |... X| == 1 => j == l == h et A -> ... X est une production simple et les A[j] sont
	      generes avant les X[j]. Si A[h..i] == Y[l..i] => A == Y et on a une contradiction
	      avec le fait que X[j] est genere avant Y[l]==A[j].
*/

#include "sxalloc.h"
#include "sxba.h"
#include "XxYxZ.h"
#include "XxY.h"
#include "SS.h"

/*   POOL_MNGR */
typedef struct {
    char	*name;
    char	**hd, *cur_top, *cur_bot, *pool_lim;
    int		hd_size, hd_top, pool_size;
    int		used_size, total_size; /* #if EBUG */
} pool_header;

#define pool_open(b,t,n) (((((b).cur_top+(sizeof(t))*(n))>(b).pool_lim)?pool_oflw(&(b)):NULL))

#define pool_next(x,b,t) (x=(t*)((b).cur_top),(b).cur_top+=sizeof(t))

#define pool_close(b,t,bot,top)	(bot = ((t*)(b).cur_bot), top = ((t*)((b).cur_bot = (b).cur_top)))


static void
pool_alloc (ppool, name, pool_size, elem_size)
    pool_header	*ppool;
    char	*name;
    int 	pool_size, elem_size;
{
    ppool->name = name;
    ppool->hd_top = 0;
    ppool->hd_size = 1;
    ppool->pool_size = pool_size * elem_size;
    ppool->hd = (char**) sxalloc (ppool->hd_size, sizeof (char*));
    ppool->pool_lim = ppool->cur_top = ppool->cur_bot = ppool->hd [ppool->hd_top] =
	    (char*) sxalloc (ppool->pool_size, elem_size);
    ppool->pool_lim += ppool->pool_size;

#if EBUG
    ppool->total_size = ppool->pool_size;
#endif
}



static char*
pool_oflw (ppool)
    pool_header	*ppool;
{
    if (++ppool->hd_top >= ppool->hd_size)
	ppool->hd = (char**) sxrealloc (ppool->hd, ppool->hd_size *= 2, sizeof (char*));

#if EBUG
    printf ("Pool %s: new pool of size %i is created.\n", ppool->name, ppool->pool_size);
    ppool->used_size = ppool->total_size;
    ppool->total_size += ppool->pool_size;
#endif

    ppool->pool_lim = ppool->cur_top = ppool->cur_bot = ppool->hd [ppool->hd_top] =
	(char*) sxalloc (ppool->pool_size, sizeof (char));

    ppool->pool_lim += ppool->pool_size;

    return NULL;
}



static void
pool_free (ppool)
    pool_header	*ppool;
{
#if EBUG
    printf ("Pool %s: used_size = %i bytes, total_size = %i bytes\n",
	    ppool->name,
	    (ppool->used_size + (ppool->cur_bot - ppool->hd [ppool->hd_top]))
	     + (ppool->hd_top + 1) * sizeof (int),
    	    ppool->total_size + ppool->hd_size * sizeof (int));
#endif

    while (--ppool->hd_top >= 0)
	sxfree (ppool->hd [ppool->hd_top]);

    sxfree (ppool->hd);
}

/*   END of POOL_MNGR */


/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;


struct LC0 {
  int NKshift, lgth;
};

struct for_parsact {
    void	(*new_symbol) (),
                (*Aij_pool_oflw) ();

    bool	(*action) (),
    		(*prdct) (),
                (*constraint) ();
};


/* #if 0 */
/*
 1: <RNA>		@1 = <RNA> <DoubleStrand> ;
 2: <RNA>		@2 = <DoubleStrand> ;
 3: <RNA>		@2 = <RNA> <Base> ;
 4: <RNA>		@3 = <Base> ;

 5: <DoubleStrand>	@4 = A <SingleStrand> &1 U ;
 6: <DoubleStrand>	@5 = A <InternalLoop> &1 U ;
 7: <DoubleStrand>	@6 = A <MultipleLoop> U ;
 8: <DoubleStrand>	@4 = G <SingleStrand> &1 C ;
 9: <DoubleStrand>	@5 = G <InternalLoop> &1 C ;
10: <DoubleStrand>	@6 = G <MultipleLoop> C ;
11: <DoubleStrand>	@4 = G <SingleStrand> &1 U ;
12: <DoubleStrand>	@5 = G <InternalLoop> &1 U ;
13: <DoubleStrand>	@6 = G <MultipleLoop> U ;
14: <DoubleStrand>	@4 = U <SingleStrand> &1 A ;
15: <DoubleStrand>	@5 = U <InternalLoop> &1 A ;
16: <DoubleStrand>	@6 = U <MultipleLoop> A ;
17: <DoubleStrand>	@4 = C <SingleStrand> &1 G ;
18: <DoubleStrand>	@5 = C <InternalLoop> &1 G ;
19: <DoubleStrand>	@6 = C <MultipleLoop> G ;
20: <DoubleStrand>	@4 = U <SingleStrand> &1 G ;
21: <DoubleStrand>	@5 = U <InternalLoop> &1 G ;
22: <DoubleStrand>	@6 = U <MultipleLoop> G ;

23: <InternalLoop>	@7 = <DoubleStrand> ;
24: <InternalLoop>	@8 = <InternalLoop> &1 <Base> ;
25: <InternalLoop>	@9 = <Base> <InternalLoop> &1 ;

26: <MultipleLoop>	@10 = <MultipleLoop> <DoubleStrand> ;
27: <MultipleLoop>	@11 = <DoubleStrand> ;
28: <MultipleLoop>	@12 = <MultipleLoop> <Base> ;
29: <MultipleLoop>	@13 = <Base> ;

30: <Base>		= A ;
31: <Base>		= U ;
32: <Base>		= C ;
33: <Base>		= G ;

34: <SingleStrand>	@14 = <SingleStrand> &1 <Base> ;
35: <SingleStrand>	@15 = <Base> ;

* @7 : len = 0;
* @8 : len = $1.len +1;
* @9 : len = $2.len +1;

* @14 :len = S1.len + 1;
* @15 : len = 0;

* &1 false <==> len > 30

*/



#define ntmax		6
#define tmax		(-5)
#define statemax	116 
#define prodmax		35
#define initial_state	1
#define	rhs_lgth	3
/* maxlon est le nombre maximal d'occurrence d'un nt (ici B) en position reduce (A -> alpha B)
   ici c'est <Base> */
#define maxntlon	7
#define maxtlon		7


static int lispro [] = 	{0,
1, -5, 0,
1, 2, 0,
2, 0,
1, 3, 0,
3, 0,
-1, 4, -2, 0,
-1, 5, -2, 0,
-1, 6, -2, 0,
-3, 4, -4, 0,
-3, 5, -4, 0,
-3, 6, -4, 0,
-3, 4, -2, 0,
-3, 5, -2, 0,
-3, 6, -2, 0,
-2, 4, -1, 0,
-2, 5, -1, 0,
-2, 6, -1, 0,
-4, 4, -3, 0,
-4, 5, -3, 0,
-4, 6, -3, 0,
-2, 4, -3, 0,
-2, 5, -3, 0,
-2, 6, -3, 0,

2, 0,
5, 3, 0,
3, 5, 0,

6, 2, 0,
2, 0,
6, 3, 0,
3, 0,

-1, 0,
-2, 0,
-4, 0,
-3, 0,

4, 3, 0,
3, 0
};

static int prolis [] = 	{0,
0, 0, 0,
1, 1, 1,
2, 2,
3, 3, 3,
4, 4,
5, 5, 5, 5,
6, 6, 6, 6,
7, 7, 7, 7,
8, 8, 8, 8,
9, 9, 9, 9,
10, 10, 10, 10,
11, 11, 11, 11,
12, 12, 12, 12,
13, 13, 13, 13,
14, 14, 14, 14,
15, 15, 15, 15,
16, 16, 16, 16,
17, 17, 17, 17,
18, 18, 18, 18,
19, 19, 19, 19,
20, 20, 20, 20,
21, 21, 21, 21,
22, 22, 22, 22,

23, 23,
24, 24, 24,
25, 25, 25,

26, 26, 26,
27, 27,
28, 28, 28,
29, 29,

30, 30,
31, 31,
32, 32,
33, 33,

34, 34, 34,
35, 35
};


static int prdct [] = 	{-1,
-1, -1, -1,

-1, -1, -1,
-1, -1,
-1, -1, -1,
-1, -1,

-1, 1, -1, -1,
-1, 1, -1, -1,
-1, -1, -1, -1,
-1, 1, -1, -1,
-1, 1, -1, -1,
-1, -1, -1, -1,
-1, 1, -1, -1,
-1, 1, -1, -1,
-1, -1, -1, -1,
-1, 1, -1, -1,
-1, 1, -1, -1,
-1, -1, -1, -1,
-1, 1, -1, -1,
-1, 1, -1, -1,
-1, -1, -1, -1,
-1, 1, -1, -1,
-1, 1, -1, -1,
-1, -1, -1, -1,

-1, -1,
1, -1, -1,
-1, 1, -1,

-1, -1, -1,
-1, -1,
-1, -1, -1,
-1, -1,

-1, -1,
-1, -1,
-1, -1,
-1, -1,

1, -1, -1,
-1, -1
};


static int prolon [] = {
  1,

  4,
  7,
  9,
  12,

  14,
  18,
  22,
  26,
  30,
  34,
  38,
  42,
  46,
  50,
  54,
  58,
  62,
  66,
  70,
  74,
  78,
  82,

  86,
  88,
  91,

  94,
  97,
  99,
  102,

  104,
  106,
  108,
  110,

  112,
  115,
  117
};


static int lhs [] = 	{
0,

1,
1,
1,
1,

2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,
2,

5,
5,
5,

6,
6,
6,
6,

3,
3,
3,
3,

4,
4
};


static int parsact [] = 	{
-1,

1,
2,
2,
3,

4,
5,
6,
4,
5,
6,
4,
5,
6,
4,
5,
6,
4,
5,
6,
4,
5,
6,

7,
8,
9,

10,
11,
12,
13,

-1,
-1,
-1,
-1,

14,
15
};

static int NKshift [] = {0,
/* RNA = 26 */
111,
109,
107,
105,
83,
79,
75,
71,
67,
63,
59,
55,
51,
47,
43,
39,
35,
31,
27,
23,
19,
15,
13,
10,
8,
5,

/* DoubleStrand = 18 */
83,
79,
75,
71,
67,
63,
59,
55,
51,
47,
43,
39,
35,
31,
27,
23,
19,
15,

/* Base = 4 */
111,
109,
107,
105,

/* SingleStrand = 6 */
116,
113,
111,
109,
107,
105,

/* InternalLoop = 25 */
92,
89,
87,
111,
109,
107,
105,
83,
79,
75,
71,
67,
63,
59,
55,
51,
47,
43,
39,
35,
31,
27,
23,
19,
15,

/* MultipleLoop = 26 */
103,
100,
98,
95,
111,
109,
107,
105,
83,
79,
75,
71,
67,
63,
59,
55,
51,
47,
43,
39,
35,
31,
27,
23,
19,
15
};


static struct LC0 LC0 [] ={
              /* 0 */ {0, 0},
	      /* 1 */ {1, 26},
	      /* 2 */ {27, 18},
	      /* 3 */ {45, 4},
	      /* 4 */ {49, 6},
	      /* 5 */ {55, 25},
	      /* 6 */ {80, 26}
};


static int constraints [] = {-1,
				 -1,
				 -1,
				 -1,
				 1,
				 1,
				 -1
			 };

static int nt2order [] = {0,
			      1,
			      4,
			      6,
			      5,
			      2,
			      3
			  };

static int order2nt [] = {0,
			      1, /* "RNA" */
			      5, /* "InternalLoop" */
			      6, /* "MultipleLoop" */
			      2, /* "DoubleStrand" */
			      4, /* "SingleStrand" */
			      3  /* "Base" */
			  };

static char	*ntstring [] = {"",
				    "RNA",
				    "DoubleStrand",
				    "Base",
				    "SingleStrand",
				    "InternalLoop",
				    "MultipleLoop"
				};

static char	*tstring [] = {"",
				    "A",
				    "U",
				    "G",
				    "C"
				};

static int char2tok (c)
    char c;
{
    int tok;

    switch (c)
    {
    case 'A': tok = -1; break;
    case 'U': tok = -2; break;
    case 'G': tok = -3; break;
    case 'C': tok = -4; break;
    default: tok = 0; break;
    }

    return tok;
}
/* #endif */



#if 0


/* On code l'automate LC de la grammaire
   S' -> 1 S 2 $ 3
   S  -> 4 S 5 S 6
   S  -> 7 a 8
   a la main
   */

static int lispro [] = 	{0,
			     1, -2, 0,
			     1, 1, 0,
			     -1, 0
			 };

static int prolis [] = 	{0,
			     0, 0, 0,
			     1, 1, 1,
			     2, 2
			 };

static int prdct [] = 	{-1,
			     -1, -1, -1,
			     -1, -1, -1,
			     -1, -1
			 };

static int prolon [] = 	{1,
			     4,
			     7,
			     9
			 };

static int lhs [] = 	{0,
			     1,
			     1
			 };


/* Ne contient pas les transitions "kernel" */
static struct LC0 LC0 [] = {
{0, 0},
{1, 2}
};
static int NKshift [] = {0, 5, 8};

static int constraints [] = {-1,
				 -1
			     };
static int nt2order [] = {0,
			      1
			  };

static int order2nt [] = {0,
			      1 /* "S" */
			  };

static char	*ntstring [] = {"",
				    "S"
				};

static char	*tstring [] = {"",
				    "a"
				};

static int char2tok (c)
    char c;
{
    return (c != 'a') ? 0 : -1;
}

#define ntmax		1
#define tmax		(-2)
#define statemax	8 
#define prodmax		2
#define initial_state	1
#define	rhs_lgth	2
#define maxntlon	1
#define maxtlon		1

#endif

#define PUSH(s,x)	(s)[++*(s)]=(x)
#define POP(s)		(s)[(*(s))--]
#define IS_EMPTY(s)	(*(s)==0)

struct sprod2attr {
  int	A, B, state;
};

struct chains2attr {
  int	prod;
};

static int		sprodmax;
static int		*sprod2order;
static SXBA		sprod_set;
static struct sprod2attr	*sprod2attr;

static char	ME [] = "earley_LC";

static char	Usage [] = "\
Usage:\t%s ('R' | 'r' |'P' | 'p') ['p'] \"source string\"\n";


static bool		is_print_prod;
static	int		xprod;

static int		n;
static int		*TOK;
static char		*source;

static SXBA		*INIT, *FIRST_1, *NT2CHAIN;

typedef struct {
    struct bag_disp_hd {
	int		size;
	SXBA_ELT	*set;
    }		*hd;
    SXBA_ELT	*pool_top;
    int		hd_size, hd_top, hd_high, pool_size, room;
    char	*name;
    int		used_size, prev_used_size, total_size; /* #if EBUG */
} bag_header;


static bool		is_parser;

typedef struct {
  int			state;
  SXBA			index_set;
} sis;

static pool_header	sis_pool;


typedef struct {
  int			state;
  SXBA			index_set;
  SXBA			backward_index_set;
} sir;

static pool_header	sir_pool;


struct R_shift {
  sis		*bot, *top;
  int		*Aij;
};

struct R_reduce {
  sir		*bot, *top;
};

struct tbp {
    SXBA	set;
    int		card;
};

static struct recognize_item {
  struct R_shift	nt [ntmax+1];
  struct R_shift	shift [ntmax+1];
  struct R_reduce	reduce [ntmax+1];
  SXBA			lhs [ntmax+1];
  SXBA			wset [ntmax+1];
  SXBA			wset3 [ntmax+1];
  SXBA			backward [ntmax+1];
  struct tbp		tbp [ntmax+1];
  SXBA			p [ntmax+1];
  int			nt_hd [ntmax+1];
  int			state_stack [statemax + 1];
}			*RT;

static struct recognize_item
                        *RT1, *RT2;

static struct RL_item {
  SXBA			nt [ntmax+1];
}			*HT;

static bag_header	shift_bag;

static struct T_index_sets {
  SXBA		index_sets [statemax + 1];
  SXBA		backward_index_sets [statemax + 1];
}			*T_index_sets;

static SXBA		T1_shift_state_set, T2_shift_state_set;
static int		shift_NT_stack_1 [ntmax + 1],
                        shift_NT_stack_2 [ntmax + 1];
static int		*T1_shift_NT_stack, *T2_shift_NT_stack;
static SXBA             *T1_index_sets, *T2_index_sets, *T2_backward_index_sets;

static SXBA		*global_state_sets;

static SXBA		reduce_NT_set;
static int		reduce_NT_stack [ntmax+1];
static int		reduce_NT_hd [ntmax+1];
static int		reduce_state_stack [statemax + 1];

static SXBA		init_state_set, state_set, old_state_set, reduce_state_set;

static int		nk_scan_reduce_state_stack [statemax+1], k_scan_reduce_state_stack [statemax+1],
                        state_stack [statemax+1];
static sis		*state2psis [statemax+1];

static SXBA		*ntXindex_set;
static SXBA		nt_hd [ntmax+1];


/* is_parser */

static struct for_parsact
                        for_parsact;


struct spf /* shared_parse_forest */
{
    struct G
    {
	int		N, T, P, G, start_symbol, rhs_top, rhs_size, lhs_top, lhs_size;
    } G;

    struct rhs
    {
	int		lispro, prolis, next_rhs;
    } *rhs;

    struct lhs
    {
	int		prolon, reduc, next_lhs, init_prod;
	bool		is_erased;
    } *lhs;
};

static struct spf	spf;

static int		rhs_stack [rhs_lgth+1];

static struct Aij_pool {
  int		A, i, j, first_lhs, first_rhs;
  bool	is_erased;
}			*Aij_pool;
static int		Aij_top, Aij_size;

static int		*symbols;
/* Acces a l'element A,i,j se fait par RT [i].nt [A].Aij [j-i]. */

static int		*reduce_hd [prodmax + 1];
static int		*global_reduce_hd /* [prodmax * (n+1) ] */;

static struct reduce_list {
    int		prod, next;
    SXBA	index_set;
}			*reduce_list;
static int		reduce_list_cur, reduce_list_top;

static bag_header	reduce_bag;

static SXBA		reduce_index_sets [statemax + 1], backward_reduce_index_sets [statemax + 1];

#if EBUG
static SXBA		debug_set1, debug_set2;
#endif

static SXBA		*level_index_sets, working_set;

/* end is_parser */





static void
grammar ()
{
    int		prod, state, X, Y, A, B, x, lim, t;
    SXBA	*NT2NT, *NT2T;

    /* On remplit INIT [A] = {B -> . alpha | B <* A} */
    INIT = sxbm_calloc (ntmax + 1, statemax + 1);

    for (X = 1; X <= ntmax; X++)
    {
	for (x = LC0 [X].NKshift, lim = x + LC0 [X].lgth; x < lim; x++)
	{
	    state = NKshift [x] - 1;
	    SXBA_1_bit (INIT [X], state);
	}
    }

    /* On remplit NT2CHAIN [A] = { U -> alpha V. X beta | V ->* A} */

    NT2NT = sxbm_calloc (ntmax + 1, ntmax + 1);

    NT2CHAIN = sxbm_calloc (ntmax + 1, statemax + 1);

    for (prod = 0; prod <= prodmax; prod++)
    {
	X = lispro [state];
	Y = lispro [state+1];

	if (X > 0 && Y == 0)
	{
	    /* A -> X */
	    A = lhs [prod];
	    SXBA_1_bit (NT2NT [A], X);
	}
    }

    fermer (NT2NT, ntmax+1);

    for (prod = 0; prod <= prodmax; prod++)
    {
	state = prolon [prod];

	X = lispro [state];

	while ((Y = lispro [++state]) != 0)
	{
	    if (X > 0)
	    {
		/* A -> alpha X . Y beta */
		SXBA_1_bit (NT2CHAIN [X], state);

		B = 0;

		while ((B = sxba_scan (NT2NT [X], B)) > 0)
		{
		    SXBA_1_bit (NT2CHAIN [B], state);
		}
	    }

	    X = Y;
	}
    }

    sxbm_free (NT2NT);

    /* On remplit FIRST_1 [a] = {U -> alpha . beta | a \in T et
       a \in FIRST(beta)LOOK-AHEAD(U -> alpha beta)} */

    NT2NT = sxbm_calloc (ntmax + 1, ntmax + 1);
    NT2T = sxbm_calloc (ntmax + 1, -tmax + 1);

    FIRST_1 = sxbm_calloc (-tmax + 1, statemax + 1);
    
    for (prod = 0; prod <= prodmax; prod++)
    {
	state = prolon [prod];
	A = lhs [prod];
	X = lispro [state];

	if (X < 0)
	    SXBA_1_bit (NT2T [A], -X);
	else
	    SXBA_1_bit (NT2NT [A], X);
    }

    fermer (NT2NT, ntmax+1);

    for (prod = 0; prod <= prodmax; prod++)
    {
	state = prolon [prod];

	while ((X = lispro [state]) != 0)
	{
	    if (X < 0)
		SXBA_1_bit (FIRST_1 [-X], state);

	    else
	    {
		t = 0;

		while ((t = sxba_scan (NT2T [X], t)) > 0)
		{
		    SXBA_1_bit (FIRST_1 [t], state);
		}

		Y = 0;

		while ((Y = sxba_scan (NT2NT [X], Y)) > 0)
		{
		    t = 0;

		    while ((t = sxba_scan (NT2T [Y], t)) > 0)
		    {
			SXBA_1_bit (FIRST_1 [t], state);
		    }
		}
	    }

	    state++;
	}

	/* POUR L"INSTANT, ON FAIT DS LE GROSSIER!!! */
	/* ON CALCULERA LE VRAI LOOK-AHEAD + TARD */
	for (t = 1; t <= -tmax; t++)
	    SXBA_1_bit (FIRST_1 [t], state);
    }

    sxbm_free (NT2NT);
    sxbm_free (NT2T);
}


static    struct chains2attr	*chains2attr;

static  void
chains_oflw (old_size, new_size)
    int		old_size, new_size;
{
    chains2attr = (struct chains2attr*) sxrealloc (chains2attr, new_size+1, sizeof (struct chains2attr));
}


static void
grammar2 ()
{
    /* A chaque production simple r: A -> B on affecte un entier n(r) t.q.
       r1: A -> B et r2: B -> C => n(r1) < n(r2). */
    XxY_header		chains;
    SXBA		chain_set, nb0;
    int			nb [ntmax + 1];

    int			prod, x, A, B, AxB;

    XxY_alloc (&chains, "chains", prodmax, 1, 1, 0, chains_oflw,
#ifdef EBUG
	       stdout
#else
	       NULL
#endif
	       );

    chains2attr = (struct chains2attr*) sxalloc (XxY_size (chains)+1, sizeof (chains2attr));

    chain_set = sxba_calloc (ntmax+1);
    nb0 = sxba_calloc (ntmax+1);
    /* nb = (int*) sxcalloc (ntmax+1, sizeof (int)); */

    x = 4;

    while (x <= statemax)
    {
	if ((B = lispro [x]) > 0 && lispro [x+1] == 0)
	{
	    /* production simple */
	    prod = prolis [x];
	    A = lhs [prod];
	    XxY_set (&chains, A, B, &AxB);
	    chains2attr [AxB].prod = prod;

	    if (SXBA_bit_is_reset_set (chain_set, A))
	    {
		SXBA_1_bit (nb0, A);
	    }

	    if (SXBA_bit_is_reset_set (chain_set, B))
	    {
		nb [B] = 1;
	    }
	    else
	    {
		if (nb [B] == 0)
		    SXBA_0_bit (nb0, B);

		nb [B]++;
	    }
	}

	while (lispro [x++] != 0);
    }
    
    sprodmax = 0;

    while ((A = sxba_scan_reset (nb0, 0)) > 0)
    {
	SXBA_0_bit (chain_set, A);

	XxY_Xforeach (chains, A, AxB)
	{
	    prod = chains2attr [AxB].prod;
	    sprod2order [prod] = ++sprodmax;

	    B = XxY_Y (chains, AxB);

	    if (--nb [B] == 0)
		SXBA_1_bit (nb0, B);
	}
    }

    XxY_free (&chains);
    sxfree (chains2attr);
    sxfree (chain_set);
    sxfree (nb0);
    /* sxfree (nb); */

}

static void
grammar_free ()
{
    sxbm_free (INIT);
    sxbm_free (NT2CHAIN);
    sxbm_free (FIRST_1);
}




static void
OR (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    /* La memoisation de OR par
       if (XxY_set (&include_sets_hd, rhs_bits_array, lhs_bits_array, &dum))
       return;
       sur ARN_186 a donne' :
          - nombre d'appels de OR : 		185206
	  - nombre d'appels deja memoise : 	 99190
	  - nombre d'inclusion non memoise :	  7508
	  - AUGMENTATION du temps d'execution total de 0.76s a 3.27s

       Il est donc beaucoup plus couteux de memoiser que de calculer des OR inutiles. */

    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));

#if EBUG
    if (*lhs_bits_array < *rhs_bits_array)
	sxtrap (ME, "OR");
#endif
    
    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	*lhs_bits_ptr-- |= *rhs_bits_ptr--;
    }
}


static void
OR_RAZ (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    /* *lhs_bits_array <= *rhs_bits_array */
    
    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	*lhs_bits_ptr-- |= *rhs_bits_ptr;
	*rhs_bits_ptr-- = 0L;
    }
}

static void
COPY (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- >= /* On copie aussi la dimension de  rhs_bits_array!!! */ 0)
    {
	*lhs_bits_ptr-- = *rhs_bits_ptr--;
    }
}

static void
OR_SHIFT_RESET (lhs_bits_array, rhs_bits_array, n)
    SXBA	lhs_bits_array, rhs_bits_array;
    int		n;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));
    SXBA_ELT		rhs, r, l;
    int			ls = SXBITS_PER_LONG - n;
    SXBA_ELT		filtre = (~(0L)) >> ls;

#if EBUG
    if (*lhs_bits_array < *rhs_bits_array || n <= 0)
	sxtrap (ME, "OR_SHIFT_RESET");
#endif
    
    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;
    l = 0;

    while (slices_number-- > 0)
    {
	if ((r = rhs = *rhs_bits_ptr) != 0L)
	{
	    *rhs_bits_ptr-- = 0L;
	    r &= filtre;
	    r <<= ls;
	    rhs >>= n;
	}
	else
	    rhs_bits_ptr--;

	if (l != 0)
	    rhs |= l;

	*lhs_bits_ptr-- |= rhs;
	l = r;
    }
}


static bool
AND (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));
    register int	lhs_slices_number = NBLONGS (BASIZE (lhs_bits_array));
    bool		ret_val = false;

#if EBUG
    if (*lhs_bits_array < *rhs_bits_array)
	sxtrap (ME, "AND");
#endif
    
    lhs_bits_ptr = lhs_bits_array + lhs_slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    if (lhs_slices_number > slices_number)
    {
	do
	{
	    *lhs_bits_ptr-- = 0L;
	} while (--lhs_slices_number > slices_number);
    }

    while (slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- &= *rhs_bits_ptr--)
	    ret_val = true;
    }

    return ret_val;
}


static bool
IS_AND (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));
    register int	lhs_slices_number = NBLONGS (BASIZE (lhs_bits_array));

    if (lhs_slices_number < slices_number)
	slices_number = lhs_slices_number;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- & *rhs_bits_ptr--)
	    return true;
    }

    return false;
}


static bool
IS_AND_MINUS (bits_array1, bits_array2, bits_array3)
    SXBA	bits_array1, bits_array2, bits_array3;
{
    /* bits_array1 et (bits_array2 - bits_array3) */
    register SXBA	bits_ptr1, bits_ptr2, bits_ptr3;
    register int	slices_number1 = NBLONGS (BASIZE (bits_array1));
    register int	slices_number2 = NBLONGS (BASIZE (bits_array2));
    register int	slices_number3 = NBLONGS (BASIZE (bits_array3));

    if (slices_number2 < slices_number1)
	slices_number1 = slices_number2;

    if (slices_number3 < slices_number1)
	slices_number1 = slices_number3;

    bits_ptr1 = bits_array1 + slices_number1, bits_ptr2 = bits_array2 + slices_number1, bits_ptr3 = bits_array3 + slices_number1;

    while (slices_number1-- > 0)
    {
	if (*bits_ptr1-- & (*bits_ptr2-- & (~(*bits_ptr3--))))
	    return true;
    }

    return false;
}


static bool
IS_SUBSET (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	lhs_slices_number = NBLONGS (BASIZE (lhs_bits_array));
    SXBA_ELT		lhs, rhs;

#if EBUG
    if (*lhs_bits_array > *rhs_bits_array)
	sxtrap (ME, "IS_SUBSET");
#endif

    lhs_bits_ptr = lhs_bits_array + lhs_slices_number, rhs_bits_ptr = rhs_bits_array + lhs_slices_number;

    while (lhs_slices_number-- > 0)
    {
	lhs = *lhs_bits_ptr--;
	rhs = *rhs_bits_ptr--;

	if (lhs != (lhs & rhs))
	    return false;
    }

    return true;
}


static void
bag_alloc (pbag, name, size)
    bag_header	*pbag;
    char	*name;
    int 	size;
{
    SXBA_ELT	*ptr;

    pbag->name = name;
    pbag->hd_top = 0;
    pbag->hd_high = 0;
    pbag->hd_size = 1;
    pbag->pool_size = size;
    pbag->hd = (struct bag_disp_hd*) sxalloc (pbag->hd_size, sizeof (struct bag_disp_hd));
    pbag->pool_top = pbag->hd [0].set = (SXBA_ELT*) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));
    pbag->hd [0].size = pbag->pool_size;
    pbag->room = pbag->pool_size;

#if EBUG
    pbag->total_size = pbag->pool_size + 2;
    pbag->used_size = pbag->prev_used_size = 0;
#endif
}



static SXBA
bag_get (pbag, size)
    bag_header	*pbag;
    int		size;
{
    int 	slice_nb = NBLONGS (size) + 1;
    SXBA	set;

    if (slice_nb > pbag->room)
    {
	if (++pbag->hd_top >= pbag->hd_size)
	    pbag->hd = (struct bag_disp_hd*) sxrealloc (pbag->hd,
							pbag->hd_size *= 2,
							sizeof (struct bag_disp_hd));

	if (pbag->hd_top <= pbag->hd_high) {
	    pbag->pool_size = pbag->hd [pbag->hd_top].size;

	    while (slice_nb > (pbag->room = pbag->pool_size)) {
		if (++pbag->hd_top >  pbag->hd_high)
		    break;

		pbag->pool_size = pbag->hd [pbag->hd_top].size;
	    }
	}

	if (pbag->hd_top > pbag->hd_high) {	    
	    while (slice_nb > (pbag->room = pbag->pool_size))
		pbag->pool_size *= 2;

#if EBUG
	    printf ("Bag %s: New bag of size %i is created.\n", pbag->name, pbag->pool_size);
	    pbag->total_size += pbag->pool_size + 2;
#endif

	    pbag->hd_high = pbag->hd_top;
	    pbag->hd [pbag->hd_top].set = (SXBA) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));
	    pbag->hd [pbag->hd_top].size = pbag->pool_size;
	}

	pbag->pool_top = pbag->hd [pbag->hd_top].set;
    }

    *(set = pbag->pool_top) = size;
    pbag->pool_top += slice_nb;
    pbag->room -= slice_nb;

#if EBUG
    pbag->used_size += slice_nb;
#endif

    return set;
}



static void
bag_free (pbag)
    bag_header	*pbag;
{
#if EBUG
    printf ("Bag %s: used_size = %i bytes, total_size = %i bytes\n",
	    pbag->name,
	    (pbag->used_size > pbag->prev_used_size ? pbag->used_size : pbag->prev_used_size) *
	    sizeof (SXBA_ELT) + (pbag->hd_high + 1) * sizeof (struct bag_disp_hd),
    	    pbag->total_size * sizeof (SXBA_ELT) + pbag->hd_size * sizeof (struct bag_disp_hd));
#endif

    do {
	sxfree (pbag->hd [pbag->hd_high].set);
    } while (--pbag->hd_high >= 0);

    sxfree (pbag->hd);
}
    

static void
bag_clear (pbag)
    bag_header	*pbag;
{
    /* On suppose que les SXBA sont empty. */
    pbag->hd_top = 0;
    pbag->pool_top = pbag->hd [0].set;
    pbag->pool_size = pbag->hd [0].size;
    pbag->room = pbag->pool_size;

#if EBUG
    if (pbag->prev_used_size < pbag->used_size)
	pbag->prev_used_size = pbag->used_size;

    pbag->used_size = 0;
#endif
}
    



static int
set_symbol (A, i, j)
    int A, i, j;
{
    struct Aij_pool	*ppool;
    int			old_size;

    if (++Aij_top >= Aij_size) {
	old_size = Aij_size;
	Aij_pool = (struct Aij_pool*) sxrealloc (Aij_pool, Aij_size *= 2, sizeof (struct Aij_pool));

	if (for_parsact.Aij_pool_oflw != NULL)
	    (*for_parsact.Aij_pool_oflw) (old_size, Aij_size);
    }

    if (is_parser) {
	if (A == 1 /* axiome */ && i == 0 && j == n)
	    spf.G.start_symbol = Aij_top;
    }

    ppool = Aij_pool + Aij_top;

    ppool->A = A;
    ppool->i = i;
    ppool->j = j;
    ppool->first_lhs = 0;
    ppool->first_rhs = 0;

    if (for_parsact.new_symbol != NULL)
	(*for_parsact.new_symbol) (Aij_top);

    return RT [i].nt [A].Aij [j-i] = Aij_top;
}


static void
print_symb (symb, i, j)
    int	   symb, i, j;
{
    if (symb < 0)
    {
	printf ("%c(%i) ", source [i], j);
    }
    else
    {
	printf ("%s[%i..%i] ", ntstring [symb], i, j);
    }
}


static bool
print_prod (rhs_stack, item, top)
    int rhs_stack[], item, top;
{
    /* Il ne peut y avoir 2 fois la meme! */
    int			bot, symbol;

    xprod++;

    if (is_print_prod)
    {
	printf ("%i\t: ", xprod);
	print_symb (lhs [prolis [item]], rhs_stack [0], rhs_stack [top]);
	fputs ("\t = ", stdout);

	for (bot = 1; symbol = lispro [item], bot <= top; item++, bot++)
	    print_symb (symbol, rhs_stack [bot-1], rhs_stack [bot]);
    
	fputs (";\n", stdout);
    }

    return true;
}

static void
print_v (symb)
    int	   symb;
{
    if ((symb < 0))
	printf ("%s ", tstring [-symb]);
    else
	printf ("<%s> ", ntstring [symb]);
}


static void
print_item (item)
    int item;
{
    int		i, X, prod;

    print_v (lhs [prod = prolis [item]]);
    fputs ("= ", stdout);
    
    for (i = prolon [prod]; (X = lispro [i]) != 0; i++)
    {
	if (i == item)
	    fputs (". ", stdout);

	print_v (X);
    }
}



static void
print_SXBA (set)
    SXBA	set;
{
    int		i, j;
    
    fputs ("{", stdout);
    
    i = sxba_scan (set, -1);

    if (i >= 0)
    {
	while ((j = sxba_scan (set, i)) != -1)
	{
	    printf ("%i, ", i);
	    i = j;
	}

	printf ("%i", i);
    }

    fputs ("}", stdout);
}

#if EBUG
static void
output_symb (symb)
    int	   symb;
{
    struct Aij_pool	*ppool;

    if (symb < 0)
    {
	printf ("%c(%i) ", source [-symb-1], -symb);
    }
    else
    {
	ppool = Aij_pool + symb;
	printf ("%s[%i..%i] ", ntstring [ppool->A], ppool->i, ppool->j);
    }
}


static void
output_prod (prod)
    int prod;
{
    int			x, symbol;
    struct lhs		*plhs;

    plhs = spf.lhs+prod;

    if (plhs->is_erased)
	fputs ("*", stdout);

    printf ("%i\t: ", prod);
    output_symb (plhs->reduc);
    fputs ("\t = ", stdout);

    for (x = plhs->prolon; (symbol = spf.rhs [x].lispro) != 0; x++)
	output_symb (symbol);
    
    fputs (";\n", stdout);
}
#endif


static int
set_rule (init_prod, top)
    int init_prod, top;
{
    int		x, X, Xij, prdct_no, state;
    int		Aik, k, A, i;
    struct lhs	*plhs;
    struct rhs	*prhs;
    struct Aij_pool	*ppool;

#if EBUG
    /* On verifie la coherence de rhs_stack */
    ppool = Aij_pool + rhs_stack [0];
    k = ppool->j;
    i = ppool->i;
    A = ppool->A;

    if (A != lhs [init_prod])
	sxtrap (ME, "set_rule");

    for (x = 1, state = prolon [init_prod]; x <= top; x++, state++)
    {
	if ((Xij = rhs_stack [x]) > 0)
	{
	    ppool = Aij_pool + Xij;

	    if (ppool->i != i || ppool->A != lispro [state])
		sxtrap (ME, "set_rule");

	    i = ppool->j;
	}
	else
	{
	    i++;

	    if (-i != Xij || TOK [-Xij] != lispro [state])
		sxtrap (ME, "set_rule");
	}
    }

    if (lispro [state] != 0 || i != k)
	sxtrap (ME, "set_rule");
#endif

    if (++spf.G.lhs_top >  spf.G.lhs_size)
    {
	spf.G.lhs_size *= 2;
	spf.lhs = (struct lhs*) sxrealloc (spf.lhs, spf.G.lhs_size+1, sizeof (struct lhs));
    }

    if (spf.G.rhs_top+top >=  spf.G.rhs_size)
    {
	spf.G.rhs_size *= 2;
	spf.rhs = (struct rhs*) sxrealloc (spf.rhs, spf.G.rhs_size+1, sizeof (struct rhs));
    }

    plhs = spf.lhs+spf.G.lhs_top;
    Aik = rhs_stack [0];
    ppool = Aij_pool + Aik;
    plhs->prolon = ++spf.G.rhs_top;
    plhs->reduc = Aik;
    plhs->next_lhs = ppool->first_lhs;
    ppool->first_lhs = spf.G.lhs_top;
    plhs->init_prod = init_prod;
    plhs->is_erased = false;

    for (x = 1; x <= top; x++)
    {
	X = rhs_stack [x];
	prhs = spf.rhs+spf.G.rhs_top;
	prhs->lispro = X;
	prhs->prolis = spf.G.lhs_top;

	if (X > 0)
	{
	    ppool = Aij_pool + X;
	    prhs->next_rhs = ppool->first_rhs;
	    ppool->first_rhs = spf.G.rhs_top;
	}

	spf.G.rhs_top++;
    }

    prhs = spf.rhs+spf.G.rhs_top;
    prhs->lispro = 0;
    prhs->prolis = spf.G.lhs_top;


#if EBUG
    output_prod (spf.G.lhs_top);
#endif	

    return spf.G.lhs_top;
}


static bool
parse_tree (state, bot, top, j, l)
    int bot, top, state, j, l;
{
    /* state =  A -> X1  Xh . X  Xp Xp+1
       bot = h+1
       top = p+1
       Cette procedure recursive genere toutes les split-chaines de "X ... Xp"
       qui s'etendent entre j et l et les range entre bot et top-1

       Quand c'est complet, appelle set_rule. */
    int		X, Xjl, Xjk, k, prdct_no;
    bool	ret_val;
    int		*pcur, *ptop;

#if EBUG
    if (bot >= top || j >= l)
	sxtrap (ME, "parse_tree");
#endif

    X = lispro [state];

    if (X < 0)
    {
	/* X \in T */
	k = j+1;

	if (X == TOK [k])
	{
	    rhs_stack [bot] = -(k);

	    if (bot+1 == top)
	    {
#if EBUG
		if (k != l)
		    sxtrap (ME, "parse_tree");
#endif
		ret_val = set_rule (prolis [state], top) > 0;
	    }
	    else
	    {
		ret_val = parse_tree (state+1, bot+1, top, k, l);
	    }
	}
    }
    else
    {
	ret_val = false;

	/* X \in N */
	/* Xj = XxY_is_set (&Ai_hd, X, j); */

	if (bot+1 == top)
	{
	    Xjl = RT [j].nt [X].Aij [l-j];

	    if (Xjl > 0 && Aij_pool [Xjl].first_lhs > 0)
	    {
		/* Si ==0 alors tous les arbres le definissant ont ete supprimes par
		   "erase_elementary_tree". */
		rhs_stack [bot] = Xjl;
		ret_val = set_rule (prolis [state], top) > 0;
	    }
	}
	else
	{
	    Xjk = 0;

	    for (ptop = (pcur = RT [j].nt [X].Aij) + (l-j), k = j; pcur < ptop; pcur++, k++) {
		Xjk = *pcur;

		if (Xjk > 0 && Aij_pool [Xjk].first_lhs > 0)
		{
		    rhs_stack [bot] = Xjk;
		    ret_val |= parse_tree (state+1, bot+1, top, k, l);
		}
	    }
	}
    }

    return ret_val;
}



#if 0
static bool
bu_parse_tree (rhs_stack, item, bot, top, i0)
    int rhs_stack[], item, bot, top, i0;
{
    /* item =  A -> X1  Xbot . X  Xtop
       rhs_stack [bot .. top] contient ibot ... itop
       genere tous les splits possibles de A[i0,itop] pour la prod A -> X1  Xbot X  Xtop
       
       Quand c'est complet, appelle set_rule. */
    sis		*pbot, *ptop;
    int		ibot, ibotm1, Xbot, state, prod, new_top;
    bool	ret_val;
    SXBA	index_set, index_set2;
    int		new_rhs_stack [rhs_lgth+1];

#if EBUG
    if (bot == 0)
	sxtrap (ME, "bu_parse_tree");
#endif

    ibot = rhs_stack [bot];
    /* (item, i0) est un element de la liste Tibot */
    item--;
    bot--;
    Xbot = lispro [item];

    if (Xbot < 0)
    {
	/* X \in T */
	if (TOK [ibot] != Xbot)
	    ret_val = false;
	else
	{
	    if (bot == 0)
	    {
		if (ibot-1 != i0)
		    ret_val = false;
		else
		{
		    rhs_stack [0] = i0;

		    print_prod (rhs_stack, item, top);

		    ret_val = true;
		}
	    }
	    else
	    {
		rhs_stack [bot] = ibot - 1;
		ret_val = bu_parse_tree (rhs_stack, item, bot, top, i0);
	    }
	}
    }
    else
    {
	/* On recherche Xbot -> gamma ., I ds les "reduce" de Tibot */
	/* A ameliorer */
	ret_val = false;

	for (ptop = RT [ibot].reduce [Xbot].top, pbot = RT [ibot].reduce [Xbot].bot; pbot < ptop; pbot++)
	{
	    state = pbot->state;
	    prod = prolis [state];
	    index_set = pbot->index_set;

#if EBUG
	    if (index_set == NULL || lhs [prod] != Xbot)
		sxtrap (ME, "bu_parse_tree");
#endif
		
	    if (RT [ibot].lhs [Xbot] == NULL)
		index_set2 = RT [ibot].lhs [Xbot] = bag_get (&shift_bag, ibot);
	    else
		index_set2 = RT [ibot].lhs [Xbot];
			    
	    new_top = state - prolon [prod];
	    new_rhs_stack [new_top] = ibot;

	    if (bot == 0)
	    {
		if (SXBA_bit_is_set_reset (index_set, i0) &&
		    bu_parse_tree (new_rhs_stack, state, new_top, new_top, i0))
		{
		    ret_val = true;
		    SXBA_1_bit (index_set2, i0);
		}
	    }
	    else
	    {
		ibotm1 = ibot;

		while ((ibotm1 = sxba_1_rlscan (index_set, ibotm1)) >= 0)
		{
		    if (ibotm1 <= i0)
			break;

		    SXBA_0_bit (index_set, ibotm1);

		    if (bu_parse_tree (new_rhs_stack, state, new_top, new_top, ibotm1))
		    {
			ret_val = true;
			SXBA_1_bit (index_set2, ibotm1);
		    }
		}
	    }

	    if (!ret_val && (ibotm1 = sxba_scan (index_set2, i0-1)) >= i0 && ibotm1 < ibot)
		ret_val = true;
	}
	    
	if (ret_val)
	{
	    if (bot == 0)
	    {
		rhs_stack [0] = i0;

		print_prod (rhs_stack, item, top);
	    }
	    else
	    {
		ret_val = false;
		ibotm1 = ibot;

		while ((ibotm1 = sxba_1_rlscan (index_set2, ibotm1)) >= 0)
		{
		    if (ibotm1 <= i0)
			break;

		    rhs_stack [bot] = ibotm1;

		    if (bu_parse_tree (rhs_stack, item, bot, top, i0))
			ret_val = true;
		}
	    }
	}
    }

    return ret_val;
}


static bool
sem (state, I_set, j, k)
    int 	state, j, k;
    SXBA	I_set;
{
    /* Suppose au plus 2 nt en rhs */
#if EBUG
    if (I_set == NULL || sxba_scan (I_set, -1) < 0)
	sxtrap (ME, "sem");
#endif

    if (is_print_prod)
    {
	fputs ("sem (", stdout);
	print_item (state);
	fputs (", ", stdout);
	print_SXBA (I_set);
	printf (", %i, %i)\n", j, k);
    }

    return true;
}

static bool
sem_Xij (X, I_set, j)
    int		X, j;
    SXBA	I_set;
{
#if EBUG
    if (I_set == NULL || sxba_scan (I_set, -1) < 0)
	sxtrap (ME, "sem_Xij");
#endif

    if (is_print_prod)
    {
	fputs ("sem_Xij (", stdout);
	print_v (X);
	fputs (", ", stdout);
	print_SXBA (I_set);
	printf (", %i)\n", j);
    }

    return true;
}
#endif

#if 0
static bool
RL_reduce (state, I_set, j, k, H_set)
    int		state, j, k;
    SXBA	I_set, H_set;
{
    /* state = A -> \alpha . X \beta et X \in NT */
    sis		*pbot, *ptop;
    int		X, Y, l, bot_state, new_state,i, h;
    SXBA	index_set, index_set2, index_set3;
    bool	is_empty, is_initial;


    X = lispro [state];

#if EBUG
    if (X <= 0)
	sxtrap (ME, "RL_reduce");
#endif

    if ((index_set = HT [j].nt [X]) == NULL)
    {
	index_set = HT [j].nt [X] = bag_get (&shift_bag, j);
	/* FAIRE QQCHOSE POUR REUTILISER LES index_set3 */
	index_set3 = bag_get (&shift_bag, j);
	is_empty = true;
	is_initial = prolon [prolis [state]] == state;

	for (ptop = RT [j].reduce [X].top, pbot = RT [j].reduce [X].bot; pbot < ptop; pbot++)
	{
#if EBUG
	    if (pbot->index_set == NULL)
		sxtrap (ME, "RL_reduce");
#endif

	    l = -1;

	    while ((l = sxba_scan (pbot->index_set, l)) >= 0)
	    {
		if (is_initial)
		{
		    if (SXBA_bit_is_set (I_set, l))
			SXBA_1_bit (index_set3, l);
		}
		else
		{
		    if ((index_set2 = T_index_sets [l].index_sets [state]) != NULL)
		    {
			/* Normalement index_set2 est un sous ensemble (non vide) de I_set */
#if EBUG
			if (!IS_SUBSET (index_set2, I_set))
			    sxtrap (ME, "RL_reduce");
#endif

			    SXBA_1_bit (index_set3, l);
		    }
		}
	    }

	    bot_state = prolon [prolis [new_state = pbot->state]];

	    /* On saute les terminaux en fin de partie droite */
	    i = j;

	    while ((Y = lispro [--new_state]) < 0 && new_state > bot_state)
	    {
		i--;
	    }
		
	    if (Y > 0)
	    {
		if (RL_reduce (new_state, index_set3, i, j, index_set))
		    is_empty = false;
	    }
	    else
	    {
		if (sem (bot_state, index_set3, i, j))
		{
		    is_empty = false;
		    OR (index_set, index_set3);
		}
	    }

	    sxba_empty (index_set3);
	}

	if (!is_empty)
	    is_empty = !sem_Xij (X, index_set, j);
	else
	{
	    /* trouver un moyen de neutraliser HT [j].nt [X] */
	}
    }
    else
	is_empty = (sxba_scan (index_set, -1) < 0);

    if (!is_empty)
    {
	is_empty = !sem (state, index_set, j, k);

	if (!is_empty)
	{
	    bot_state = prolon [prolis [state]];

	    if (state == bot_state)
	    {
		OR (H_set, index_set);
	    }
	    else
	    {
		i = 0; /* nombre de terminaux */

		while (--state >= bot_state && (X = lispro [state]) < 0)
		{
		    i++;
		}

		if (X > 0)
		{
		    is_empty = true;
		    h = -1;

		    while ((h = sxba_scan (index_set, h)) >= 0)
		    {
			if (RL_reduce (state, I_set, h-i, k, H_set))
			    is_empty = false;
		    }
		}
		else
		    OR_SHIFT (H_set, index_set, i);
	    }
	}
    }

    return !is_empty;
}


static bool
RL_reduce (state, I_set, j, k, H_set)
    int		state, j, k;
    SXBA	I_set, H_set;
{
    /* state = A -> \alpha . X \beta et X \in NT */
    sir		*pbot, *ptop;
    int		X, Y, l, bot_state, new_bot_state, new_state,i, h;
    SXBA	index_set, index_set2, index_set3;
    bool	is_empty, is_initial, processed, is_index_set3_empty;


    X = lispro [state];

#if EBUG
    if (X <= 0 || sxba_is_empty (I_set))
	sxtrap (ME, "RL_reduce");
#endif

    is_initial = (bot_state = prolon [prolis [state]]) == state;

    if ((pbot = RT [j].reduce [X].bot) != NULL)
    {
	/* Non entierement traite' */
	if ((index_set = RT [j].wset [X]) == NULL)
	    index_set = RT [j].wset [X] = bag_get (&shift_bag, j);

	index_set3 = bag_get (&shift_bag, j); /* REUTILISATION DES index_set3 !!! */
#if 0
	if ((index_set3 = RT [j].wset3 [X]) == NULL)
	    index_set3 = RT [j].wset3 [X] = bag_get (&shift_bag, j);
#endif

#if EBUG
	if (!sxba_is_empty (index_set3))
	    sxtrap (ME, "RL_reduce");
#endif

	is_empty = processed = true;

	for (ptop = RT [j].reduce [X].top; pbot < ptop; pbot++)
	{
	    if (pbot->index_set != NULL)
	    {
		/* non traite' */
		/* state =  A -> X1 ... .X  ... Xp */
		/* On verifie que pour l dans pbot->index_set, (A -> X1 ... .X  ... Xp, L)
		   existe ds Tl. Ds ce cas, L doit etre un sous-ensemble de I_set. */
		is_index_set3_empty = true;
		l = -1;

		while ((l = sxba_scan (pbot->index_set, l)) >= 0)
		{
		    if (is_initial)
		    {
			if (SXBA_bit_is_set (I_set, l))
			{
			    is_index_set3_empty = false;
			    SXBA_1_bit (index_set3, l);
			    SXBA_0_bit (pbot->index_set, l);
			}
		    }
		    else
		    {
			if ((index_set2 = T_index_sets [l].index_sets [state]) != NULL)
			{
			    if (IS_AND (index_set2, I_set))
			    {
				is_index_set3_empty = false;
				SXBA_1_bit (index_set3, l);
				SXBA_0_bit (pbot->index_set, l);
			    }
			}
		    }
		}

		if (!is_index_set3_empty)
		{
#if EBUG
		    if (sxba_is_empty (index_set3))
			sxtrap (ME, "RL_reduce");
#endif


		    /* On effectue les reductions (pbot->state, index_set3) */
		    new_bot_state = prolon [prolis [new_state = pbot->state]];

		    /* On saute les terminaux en fin de partie droite */
		    i = j;

		    while ((Y = lispro [--new_state]) < 0 && new_state > new_bot_state)
			i--;
		
		    if (Y > 0)
		    {
			if (RL_reduce (new_state, index_set3, i, j, index_set))
			{
			    is_empty = false;
			}
		    }
		    else
		    {
			if (sem (new_bot_state, index_set3, i, j))
			{
			    is_empty = false;
			    OR (index_set, index_set3);
			}
		    }

		    /* sxba_empty (index_set3); pour le coup d'apres... */

		    
		    if (pbot->index_set != NULL)
		    {
			if (sxba_scan (pbot->index_set, -1) == -1)
			    pbot->index_set = NULL;
			else
			    processed = false;
		    }
		}
		else
		    processed = false;
	    }
	}

	if (processed)
	    RT [j].reduce [X].bot = NULL;

	if (!is_empty)
	    is_empty = !sem_Xij (X, index_set, j);

	if (!is_empty)
	{
	    if ((index_set2 = RT [j].lhs [X]) == NULL)
		index_set2 = RT [j].lhs [X] = bag_get (&shift_bag, j);

	    OR (index_set2, index_set);

	    if (!processed)
		sxba_empty (index_set); /* pour le coup d'apres... */
	}
    }

    /* si initial => I_set & RT [j].lhs [X]
       sinon T_index_sets [l].index_sets [state] != NULL, l \in RT [j].lhs [X] */

    if ((index_set = RT [j].lhs [X]) == NULL)
	return false;

    if (is_initial)
    {
	IS_AND (I_set, index_set);
	is_empty = !sem (state, I_set, j, k);

	if (!is_empty)
	    OR (H_set, I_set);
    }
    else
    {
	index_set3 = bag_get (&shift_bag, j); /* REUTILISATION DES index_set3 !!! */
#if 0
	index_set3 = RT [j].wset3 [X];
#endif

	l = -1;

	while ((l = sxba_scan (index_set, l)) >= 0)
	{
	    if (T_index_sets [l].index_sets [state] != NULL)
		SXBA_1_bit (index_set3, l);
	}

	is_empty = !sem (state, index_set3, j, k);

	if (!is_empty)
	{
	    i = 0;		/* nombre de terminaux */

	    while (--state >= bot_state && (X = lispro [state]) < 0)
		i++;

	    if (X > 0)
	    {
		is_empty = true;
		h = -1;

		while ((h = sxba_scan_reset (index_set3, h)) >= 0)
		{
		    if (RL_reduce (state, I_set, h-i, k, H_set))
			is_empty = false;
		}
	    }
	    else
		OR_SHIFT_RESET (H_set, index_set3, i);

	}
    }

    return !is_empty;
}



static bool
bu_parse_tree (rhs_stack, item, bot, cur, top)
    int rhs_stack[], item, bot, cur, top;
{
    /* item =  A -> X1  Xbot ... Xcur .  ... Xtop
       rhs_stack [0 ... bot : cur ... top] contient les index connus
       genere tous les splits possibles de A[rhs_stack [0], rhs_stack [itop]]
       pour la prod A -> X1 ...  Xbot Xcur ...  Xtop
       
       Quand c'est complet, appelle print_prod. */
    sir		*pbot, *ptop;
    int		icur, icurm1, Xcur, state, prod, new_top, i0, new_cur, bot_state, cur_state, inew_cur, Xnew_cur;
    int		new_bot, Xnew_bot, inew_bot, ibot;
    bool	ret_val, processed, is_first_time;
    SXBA	index_set, index_set2, index_set3;
    int		new_rhs_stack [rhs_lgth+1];

#if EBUG
    if (bot == cur)
	sxtrap (ME, "bu_parse_tree");
#endif

    ibot = rhs_stack [bot];
    icur = rhs_stack [cur--];
    /* (item, ibot) est un element de la liste Ticur */
    Xcur = lispro [--item];

    if (Xcur < 0)
    {
	/* X \in T */
#if EBUG
	if (TOK [icur] != Xcur || bot == cur)
	    sxtrap (ME, "bu_parse_tree");
#endif

	rhs_stack [cur] = icur - 1;
	ret_val = bu_parse_tree (rhs_stack, item, bot, cur, top);
    }
    else
    {
	/* On recherche Xcur -> gamma ., I ds les "reduce" de Ticur */
	/* A ameliorer */
	ret_val = false;
	i0 = rhs_stack [0];

	if ((pbot = RT [icur].reduce [Xcur].bot) != NULL)
	{
	    /* Non entierement traite' */
	    processed = true;

	    for (ptop = RT [icur].reduce [Xcur].top; pbot < ptop; pbot++)
	    {
		if ((index_set = pbot->index_set) != NULL)
		{
		    /* non traite' */
		    /* item =  A -> X1 ... Xbot ... .Xcur  ... Xtop */
		    /* On verifie que pour j dans index_set, (A -> X1 ... Xbot ... .Xcur ... Xtop, i0)
		       est un element de la liste Tj. */
		    is_first_time = true;
		    
		    icurm1 = bot == cur ? ibot-1 : ibot;

		    while ((icurm1 = sxba_scan (index_set, icurm1)) >= 0)
		    {
			if (bot == cur ||
			    (index_set3 = T_index_sets [icurm1].index_sets [item]) != NULL &&
			    SXBA_bit_is_set (index_set3, i0))
			{
			    /* On est sur que Xcur -> gamma . , icurm1 est utile */
			    SXBA_0_bit (index_set, icurm1);

			    if (is_first_time)
			    {
				/* On traite les terminaux aux extremites de gamma. */
				is_first_time = false;
				state = pbot->state;
				prod = prolis [state];

#if EBUG
				if (lhs [prod] != Xcur)
				    sxtrap (ME, "bu_parse_tree");
#endif
		
				if (RT [icur].lhs [Xcur] == NULL)
				    index_set2 = RT [icur].lhs [Xcur] = bag_get (&shift_bag, icur);
				else
				    index_set2 = RT [icur].lhs [Xcur];
			    
				new_cur = new_top = state - (bot_state = prolon [prod]);
				new_rhs_stack [new_top] = inew_cur = icur;
	    
				while (state > bot_state && (Xnew_cur = lispro [state - 1]) < 0)
				{
#if EBUG
				    if (TOK [inew_cur] != Xnew_cur)
					sxtrap (ME, "bu_parse_tree");
#endif

				    new_rhs_stack [--new_cur] = --inew_cur;
				    state--;
				}

				if (state == bot_state)
				{
				    /* La rhs ne contient que des terminaux */
#if EBUG
				    if (new_cur != 0 || inew_cur != icurm1)
					sxtrap (ME, "bu_parse_tree");
#endif

				    print_prod (new_rhs_stack, bot_state, new_top);

				    SXBA_1_bit (index_set2, inew_cur);
				    ret_val = true;
				    break;
				}
			    }

			    /* on regarde si la rhs commence par des terminaux */
			    new_rhs_stack [new_bot = 0] = inew_bot = icurm1;
			    cur_state = bot_state;

			    while ((Xnew_bot = lispro [cur_state]) < 0)
			    {
				/* La partie droite commence par des terminaux */
#if EBUG
				if (TOK [inew_bot+1] != Xnew_bot)
				    sxtrap (ME, "bu_parse_tree");
#endif

				new_rhs_stack [++new_bot] = ++inew_bot;
				cur_state++;
			    }

			    if (bu_parse_tree (new_rhs_stack, state, new_bot, new_cur, new_top))
			    {
				ret_val = true;
				SXBA_1_bit (index_set2, icurm1);
			    }

			}

			if (bot == cur)
			    break;
		    }

		    if (sxba_scan (index_set, -1) == -1)
			pbot->index_set = NULL;
		    else
			processed = false;
		}
	    }

	    if (processed)
		RT [icur].reduce [Xcur].bot = NULL;
	}
	    
	index_set2 = RT [icur].lhs [Xcur];

	if (!ret_val && (icurm1 = sxba_scan (index_set2, ibot-1)) >= ibot && icurm1 < icur)
	    ret_val = true;

	if (ret_val)
	{
	    if (bot == cur)
	    {
		print_prod (rhs_stack, prolon [prolis [item]], top);
	    }
	    else
	    {
		ret_val = false;
		icurm1 = ibot;

		while ((icurm1 = sxba_scan (index_set2, icurm1)) >= 0)
		{
		    rhs_stack [cur] = icurm1;

		    if (bu_parse_tree (rhs_stack, item, bot, cur, top))
			ret_val = true;
		}
	    }
	}
    }

    return ret_val;
}
#endif


static bool
call_semact (A, i, k)
    int A, i, k;
{
    /* Faux si le index_set retourne' n'est pas caracteristique des reductions. */
    /* il faudrait retourner un ensemble de p_uplets. A VOIR */
    int 		hd, d, j, X, Xjk, end, start, nb;
    struct reduce_list	*pred;
    bool		ret_val = false;

    rhs_stack [0] = RT [i].nt [A].Aij [k-i];
    nb = 0;

    for (hd = reduce_hd [A] [i]; hd > 0; hd = pred->next) {
	pred = reduce_list + hd;
	nb++;

	if (for_parsact.action == NULL ||
	    (*for_parsact.action) (pred->prod, i, pred->index_set, k, nb)) {
	    /* Il en reste */
	    ret_val = true;

	    start = prolon [pred->prod];
	    end = prolon [pred->prod + 1] - 1;
	    X = lispro [end-1];
	    end -= start;

	    d = -1;

	    while ((d = sxba_scan_reset (pred->index_set, d)) >= 0)
	    {
		j = d + i;

		if (X > 0)
		    Xjk = RT [j].nt [X].Aij [k-j];
		else
		    Xjk = -k;

		rhs_stack [end] = Xjk;

		if (end == 1)
		    set_rule (pred->prod, 1);
		else
		    parse_tree (start, 1, end, i, j);
	    }
	}

#if EBUG
	if (!sxba_is_empty (pred->index_set))
	    sxtrap (ME, "call_semact");
#endif
    }

    reduce_hd [A] [i] = 0;

    return ret_val;
}



static void
install (prod, i, j, k)
    int prod, i, j, k;
{
    /* is_parser est true.
       i <= j < k
       On vient de detecter la reduction prod : Aik -> alpha Xjk
       On la stocke afin de grouper les appels a la semantique sur tous les
       j intermediaires. */
    int 		A, prev_hd, hd;
    struct reduce_list	*pr;

    A = lhs [prod];
    
    for (prev_hd = 0, hd = reduce_hd [A] [i]; hd > 0; prev_hd = hd, hd = pr->next) {
	pr = reduce_list + hd;

	if (pr->prod == prod)
	    break;
    }

    if (hd == 0) {
	if (++reduce_list_cur >= reduce_list_top) {
	    reduce_list = (struct reduce_list*) sxrealloc (reduce_list,
							   reduce_list_top *= 2,
							   sizeof (struct reduce_list));
	}

	pr = reduce_list + reduce_list_cur;

	pr->next = 0;
	pr->prod = prod;
	pr->index_set = bag_get (&reduce_bag, k-i);

	if (prev_hd == 0)
	    reduce_hd [A] [i] = reduce_list_cur;
	else
	    reduce_list [prev_hd].next = reduce_list_cur;
    }

    SXBA_1_bit (pr->index_set, j-i);
}

#if 0
static void
reduce (A, j, i)
    int A, j, i;
{
    /* Toutes les reductions vers Aj ont ete effectuees
       Cas du reconnaisseur : on fait le "shift" sur A dans la liste "j".
       cas du parser : en plus, on fait les reductions eventuelles B -> alpha Aji */
    sis		*bot, *top, *pbot, *ptop;
    int		state, X, Y, B, order, k, prod;
    SXBA	index_set, index_set2;

    int 	Aji, Bji, Bki, prdct_no, start, end, constraint_no;
    bool	constraint_checked;

    if (is_parser)
    {
	/* On effectue les reductions vers Aji */
	if (constraint_checked = call_semact (A, j, i))
	{
	    Aji = RT [j].nt [A].Aij [i-j];

#if EBUG
	    if (Aji == 0)
		sxtrap (ME, "reduce");
#endif
	
	    /* On est su^r que toutes les reductions vers Aji ont ete effectuees. */
	    if ((constraint_no = constraints [A]) == -1 ||
		for_parsact.constraint == NULL ||
		(constraint_checked = (*for_parsact.constraint) (Aji, constraint_no)))
	    {
	    }
	    else
		if (!constraint_checked)
		    Aij_pool [Aji].is_erased = true;
	}

	if (!constraint_checked)
	    return;
    }

    for (top = RT [j].nt [A].top, bot = RT [j].nt [A].bot; bot < top; bot++)
    {
	state = bot->state;

#if EBUG
	if (lispro [state] != A)
	    sxtrap (ME, "reduce");
#endif

	if (!is_parser ||
	    (prdct_no = prdct [state]) == -1 ||
	    for_parsact.prdct == NULL ||
	    (*for_parsact.prdct) (Aji, prdct_no))
	{
	    state++;
	    index_set = bot->index_set;

	    Y = lispro [state];

	    if (Y == 0)
	    {
		/* B -> beta . A */
		B = lhs [prod = prolis [state]];

		if (index_set == NULL)
		{
		    /* B -> . A , j */
		    SXBA_1_bit (ntXindex_set [B], j);

		    if (is_parser)
		    {
			if ((Bji = RT [j].nt [B].Aij [i-j]) == 0)
			    RT [j].nt [B].Aij [i-j] = Bji = set_symbol (B, j, i);

			install (prod, j, j, i);
#if 0
			rhs_stack [0] = Bji;
			rhs_stack [1] = Aji;
			set_rule (prod, 1);
#endif
		    }
		}
		else
		{
		    OR (ntXindex_set [B], index_set);

		    if (is_parser)
		    {
			k = j;

			while ((k = sxba_1_rlscan (index_set, k)) >= 0)
			{
			    if ((Bki = RT [k].nt [B].Aij [i-k]) == 0)
				RT [k].nt [B].Aij [i-k] = Bki = set_symbol (B, k, i);

			    install (prod, k, j, i);
#if 0
			    rhs_stack [0] = Bki;
			    start = prolon [prod];
			    end = state - start;
			    rhs_stack [end] = Aji;
			    parse_tree (start, 1, end, k, j);
#endif
			}
		    }
		}
	    }
	    else
	    {
		/* B -> beta . A Y gamma */
		/* On met B -> beta A . Y gamma dans T2 */
		if (SXBA_bit_is_set (FIRST_1[-TOK [i+1]], state))
		{
		    if (SXBA_bit_is_reset_set (T2_state_set, state))
			PUSH (T2_state_stack, state);

		    if ((index_set2 = T2_index_sets [state]) == NULL)
			index_set2 = T2_index_sets [state] = bag_get (&shift_bag, i+1);

		    if (index_set == NULL)
			SXBA_1_bit (index_set2, j);
		    else
			OR (index_set2, index_set);
		}
	    }
	}
    }
}
#endif

#if 0
/* modifie' pour permettre de structurer directement T2 par nt et ai+1. */
static void
reduce (A, j, i)
    int A, j, i;
{
    /* Toutes les reductions vers Aj ont ete effectuees
       Cas du reconnaisseur : on fait le "shift" sur A dans la liste "j".
       cas du parser : en plus, on fait les reductions eventuelles B -> alpha Aji */
    sis		*bot, *top;
    int		state, B;
    SXBA	index_set, index_set2, backward_index_set;

    int 	ixqxj;

    for (top = RT [j].nt [A].top, bot = RT [j].nt [A].bot; bot < top; bot++)
    {
	state = bot->state+1;

#if EBUG
	if (lispro [state-1] != A)
	    sxtrap (ME, "reduce");
#endif

	index_set = bot->index_set;

	if (lispro [state] == 0)
	{
	    /* B -> beta . A */
	    B = lhs [prolis [state]];

	    if (is_parser)
	    {
		/* On ne stocke pas les reduces ds le cas du reconnaisseur. */
		if (SXBA_bit_is_reset_set (NT_set, B))
		    PUSH (NT_stack, B);

		if (SXBA_bit_is_reset_set (reduce_state_set, state))
		{
		    PUSH (reduce_state_stack [B], state);
		    index_set2 = reduce_index_sets [state] = bag_get (&shift_bag, i+1);
		    backward_index_set = backward_reduce_index_sets [state] = bag_get (&shift_bag, i+1);
		}
		else
		{
		    index_set2 = reduce_index_sets [state];
		    backward_index_set = backward_reduce_index_sets [state];
		}

		SXBA_1_bit (backward_index_set, j);

		if (index_set == NULL)
		    SXBA_1_bit (index_set2, j);
		else
		    OR (index_set2, index_set);
	    }

	    if (index_set == NULL)
	    {
		/* B -> . A , j */
		SXBA_1_bit (ntXindex_set [B], j);
	    }
	    else
	    {
		OR (ntXindex_set [B], index_set);
	    }
	}
	else
	{
	    /* B -> beta . A Y gamma */
	    /* On met B -> beta A . Y gamma dans T2 */
	    if (SXBA_bit_is_set (FIRST_1[-TOK [i+1]], state))
	    {
		if (SXBA_bit_is_reset_set (T2_state_set, state))
		{
		    PUSH (T2_state_stack, state);
		    index_set2 = T2_index_sets [state] = bag_get (&shift_bag, i+1);
		    backward_index_set = T2_backward_index_sets [state] = bag_get (&shift_bag, i+1);
		}
		else
		{
		    index_set2 = T2_index_sets [state];
		    backward_index_set = T2_backward_index_sets [state];
		}

		SXBA_1_bit (backward_index_set, j);

		if (index_set == NULL)
		    SXBA_1_bit (index_set2, j);
		else
		    OR (index_set2, index_set);
	    }
	}
    }
}
#endif


#if 0
/* suppression de "RT [j].nt [A]" qui est calcule' a la demande. */
static void
reduce (A, j, i)
    int A, j, i;
{
    /* Toutes les reductions vers Aj ont ete effectuees
       Cas du reconnaisseur : on fait le "shift" sur A dans la liste "j".
       cas du parser : en plus, on fait les reductions eventuelles B -> alpha Aji */
    sis		*bot, *top;
    int		new_state, B, Y, prdct_no;
    SXBA	index_set, index_set2, backward_index_set;

    for (top = RT [j].nt [A].top, bot = RT [j].nt [A].bot; bot < top; bot++)
    {
	new_state = bot->state+1;

#if EBUG
	if (lispro [new_state-1] != A)
	    sxtrap (ME, "reduce");
#endif

	index_set = bot->index_set;

	if ((Y = lispro [new_state]) == 0)
	{
	    /* B -> beta A . */
	    B = lhs [prolis [new_state]];

	    if (is_parser)
	    {
		/* On ne stocke pas les reduces ds le cas du reconnaisseur. */
		if (SXBA_bit_is_reset_set (reduce_NT_set, B))
		    PUSH (reduce_NT_stack, B);

		if (SXBA_bit_is_reset_set (reduce_state_set, new_state))
		{
		    reduce_state_stack [new_state] = reduce_NT_hd [B];
		    reduce_NT_hd [B] = new_state;
		    index_set2 = reduce_index_sets [new_state] = bag_get (&shift_bag, i+1);
		    backward_index_set = backward_reduce_index_sets [new_state] = bag_get (&shift_bag, i+1);
		}
		else
		{
		    index_set2 = reduce_index_sets [new_state];
		    backward_index_set = backward_reduce_index_sets [new_state];
		}

		SXBA_1_bit (backward_index_set, j);

		if (index_set == NULL)
		    SXBA_1_bit (index_set2, j);
		else
		    OR (index_set2, index_set);
	    }

	    if (index_set == NULL)
		/* B -> A ., j */
		SXBA_1_bit (ntXindex_set [B], j);
	    else
		/* B -> beta A . et beta \neq \epsilon */
		OR (ntXindex_set [B], index_set);
	}
	else
	{
	    /* new_state = B -> beta A . Y gamma */
	    /* On met B -> beta A . Y gamma dans T2 */
	    if (SXBA_bit_is_set (FIRST_1[-TOK [i+1]], new_state))
	    {
		if (Y < 0)
		{
		    /* new_state = B -> beta A . ai+1 gamma */
		    if (((prdct_no = prdct [new_state]) == -1 ||
			 for_parsact.prdct == NULL ||
			 (*for_parsact.prdct) (-i-1, prdct_no)))
			Y = 0;
		}
		    
		if (Y >= 0)
		{
		    if (Y > 0 && T2_shift_NT_hd [Y] == 0)
			PUSH (T2_shift_NT_stack, Y);

		    if (SXBA_bit_is_reset_set (T2_shift_state_set, new_state))
		    {
			T2_shift_state_stack [new_state] = T2_shift_NT_hd [Y];
			T2_shift_NT_hd [Y] = new_state;
			index_set2 = T2_index_sets [new_state] = bag_get (&shift_bag, i+1);

			if (is_parser)
			    backward_index_set = T2_backward_index_sets [new_state] = bag_get (&shift_bag, i+1);
		    }
		    else
		    {
			index_set2 = T2_index_sets [new_state];

			if (is_parser)
			    backward_index_set = T2_backward_index_sets [new_state];
		    }

		    if (is_parser)
			SXBA_1_bit (backward_index_set, j);

		    if (index_set == NULL)
			SXBA_1_bit (index_set2, j);
		    else
			OR (index_set2, index_set);
		}
	    }
	}
    }
}
#endif

static void
reduce (A, j, i)
    int A, j, i;
{
    /* Toutes les reductions vers Aj ont ete effectuees
       Cas du reconnaisseur : on fait le "shift" sur A dans la liste "j".
       cas du parser : en plus, on fait les reductions eventuelles B -> alpha Aji */
    int		state, new_state, B, Y, prdct_no;
    SXBA	index_set, index_set2, backward_index_set;

    int		*state_stack = &(RT [j].state_stack [0]);

    for (state = RT [j].nt_hd [A]; state != 0; state = state_stack [state])
    {
	if (state > 0)
	{
	    index_set = T_index_sets [j].index_sets [state];
	}
	else
	{
	    state = -state;
	    index_set = NULL;
	}

	new_state = state + 1;
	
	if ((Y = lispro [new_state]) == 0)
	{
	    /* B -> beta A . */
	    B = lhs [prolis [new_state]];

	    if (is_parser)
	    {
		/* On ne stocke pas les reduces ds le cas du reconnaisseur. */
		if (SXBA_bit_is_reset_set (reduce_NT_set, B))
		    PUSH (reduce_NT_stack, B);

		if (SXBA_bit_is_reset_set (reduce_state_set, new_state))
		{
		    reduce_state_stack [new_state] = reduce_NT_hd [B];
		    reduce_NT_hd [B] = new_state;
		    index_set2 = reduce_index_sets [new_state] = bag_get (&shift_bag, i+1);
		    backward_index_set = backward_reduce_index_sets [new_state] = bag_get (&shift_bag, i+1);
		}
		else
		{
		    index_set2 = reduce_index_sets [new_state];
		    backward_index_set = backward_reduce_index_sets [new_state];
		}

		SXBA_1_bit (backward_index_set, j);

		if (index_set == NULL)
		    SXBA_1_bit (index_set2, j);
		else
		    OR (index_set2, index_set);
	    }

	    if (index_set == NULL)
		/* B -> A ., j */
		SXBA_1_bit (ntXindex_set [B], j);
	    else
		/* B -> beta A . et beta \neq \epsilon */
		OR (ntXindex_set [B], index_set);
	}
	else
	{
	    /* new_state = B -> beta A . Y gamma */
	    /* On met B -> beta A . Y gamma dans T2 */
	    if (SXBA_bit_is_set (FIRST_1[-TOK [i+1]], new_state))
	    {
		if (Y < 0)
		{
		    /* new_state = B -> beta A . ai+1 gamma */
		    if (((prdct_no = prdct [new_state]) == -1 ||
			 for_parsact.prdct == NULL ||
			 (*for_parsact.prdct) (-i-1, prdct_no)))
			Y = 0;
		}
		else
		    if (RT2->nt_hd [Y] == 0)
			PUSH (T2_shift_NT_stack, Y);
		    
		if (Y >= 0)
		{
		    if (SXBA_bit_is_reset_set (T2_shift_state_set, new_state))
		    {
			RT2->state_stack [new_state] = RT2->nt_hd [Y];
			RT2->nt_hd [Y] = new_state;
			index_set2 = T2_index_sets [new_state] = bag_get (&shift_bag, i+1);

			if (is_parser)
			    backward_index_set = T2_backward_index_sets [new_state] = bag_get (&shift_bag, i+1);
		    }
		    else
		    {
			index_set2 = T2_index_sets [new_state];

			if (is_parser)
			    backward_index_set = T2_backward_index_sets [new_state];
		    }

		    if (is_parser)
			SXBA_1_bit (backward_index_set, j);

		    if (index_set == NULL)
			SXBA_1_bit (index_set2, j);
		    else
			OR (index_set2, index_set);
		}
	    }
	}
    }
}

#if 0
static void
fill_psis (state, index_set, j, k)
    int		state, j, k;
    SXBA	index_set;
{
    /* state : B -> beta A . gamma */
    sis		*psis;
    SXBA	index_set2;

#if EBUG
    if (state <= 0)
	sxtrap (ME, "fill_psis");
#endif

    if (SXBA_bit_is_reset_set (state_set, state))
    {
	pool_next (psis, sis_pool, sis);

	if (index_set == NULL)
	{
	    index_set = bag_get (&shift_bag, k+1);
	    SXBA_1_bit (index_set, j);
	    psis->state = -state;
	}
	else
	    psis->state = state;

	psis->index_set = index_set;
	PUSH (state_stack, state);
	state2psis [state] = psis;
    }
    else
    {
	psis = state2psis [state];

	if (psis->state > 0)
	{
	    psis->state = -state;
	    index_set2 = bag_get (&shift_bag, k+1);
	    OR (index_set2, psis->index_set);
	    psis->index_set = index_set2;
	}

	if (index_set != NULL)
	    OR (psis->index_set, index_set);
	else
	    SXBA_1_bit (psis->index_set, j);
    }
}
#endif

#if 0
static bool
shift (j, A, i)
    int j, A, i;
{
    /* On calcule dans Tj, l'effet de la transition sur A, en effectuant recursivement
       (avec memoization) les reductions. */
    sis		*x, *bot, *top, *pbot, *ptop;
    int		state, B, k;
    SXBA	index_set, index_set2;

    if (RT [j].shift [A].bot != NULL)
	/* deja calcule */
	return true;

    for (top = RT [j].nt [A].top, x = bot = RT [j].nt [A].bot; x < top; x++)
    {
	state = x->state + 1;
	index_set = x->index_set;

#if EBUG
	if (lispro [state - 1] != A)
	    sxtrap (ME, "shift");
#endif

	if (lispro [state] == 0)
	{
	    B = lhs [prolis [state]];

	    if (index_set == NULL)
	    {
		/* B -> A . , j */
		shift (j, B, i); /* On suppose non cyclique, A VOIR */
	    }
	    else
	    {
		k = j+1;

		while ((k = sxba_1_rlscan (index_set, k)) >= 0)
		{
		    shift (k, B, i);
		}
	    }
	}
    }

    pool_open (sis_pool, sis, statemax);

    for (x = bot; x < top; x++)
    {
	state = x->state + 1;
	index_set = x->index_set;

	if (lispro [state] == 0)
	{
	    /* B -> beta A . */
	    /* On peut essayer de mettre plusieurs fois RT [k].shift [B] dans RT [j].shift [A]
	       il faut peut etre le noter afin de ne pas recommencer !! */
	    B = lhs [prolis [state]];

	    if (is_parser)
	    {
		/* On ne stocke pas les reduces ds le cas du reconnaisseur. */
		if (SXBA_bit_is_reset_set (NT_set, B))
		    PUSH (NT_stack, B);

		if (SXBA_bit_is_reset_set (reduce_state_set, state))
		{
		    PUSH (reduce_state_stack [B], state);
		    index_set2 = reduce_index_sets [state] = bag_get (&shift_bag, i+1);
		}
		else
		    index_set2 = reduce_index_sets [state];

		if (index_set == NULL)
		    SXBA_1_bit (index_set2, j);
		else
		    OR (index_set2, index_set);
	    }

	    if (index_set == NULL)
	    {
		/* B -> A . , j */
		for (ptop = RT [j].shift [B].top, pbot = RT [j].shift [B].bot; pbot < ptop; pbot++)
		    fill_psis (pbot->state, pbot->index_set, j, j);
	    }
	    else
	    {
		k = j+1;

		while ((k = sxba_1_rlscan (index_set, k)) >= 0)
		{
		    for (ptop = RT [k].shift [B].top, pbot = RT [k].shift [B].bot; pbot < ptop; pbot++)
			fill_psis (pbot->state, pbot->index_set, k, j);
		}
	    }
	}
	else
	    fill_psis (state, index_set, j, j);
    }

    while (!IS_EMPTY (state_stack))
    {
	state = POP (state_stack);
	SXBA_0_bit (state_set, state);
	state2psis [state]->state = state; /* ils sont positifs */
    }

    pool_close (sis_pool, sis, RT [j].shift [A].bot, RT [j].shift [A].top);

    return false;
}
#endif


static void
scan_reduce (i)
    register int 	i;
{
    register SXBA_ELT	filtre;
    register int	j, indice, order, A;

    int			state;
    SXBA		index_set, index_set2;
    sir			*psir;

    filtre = 1 << MOD (i);
    indice = DIV (i) + 1;

    for (A = ntmax; A > 0; A--)
	nt_hd [A] = ntXindex_set [A] + indice;

    j = i;

    for (;;)
    {
	for (order = ntmax; order > 0; order--)
	{
	    A = order2nt [order];

	    /* if (SXBA_bit_is_set_reset (ntXindex_set [A], j)) */
	    if (*(nt_hd [A]) & filtre)
		reduce (A, j, i+1);
	}

	if ((filtre >>= 1) == 0)
	{
	    for (A = ntmax; A > 0; A--)
		*((nt_hd [A])--) = 0;

	    if (--j < 0)
		break;

	    filtre = 1 << MOD (j);
	}
	else
	    j--;
    }

    if (is_parser)
    {
	while (!IS_EMPTY (reduce_NT_stack))
	{
	    A = POP (reduce_NT_stack);
	    SXBA_0_bit (reduce_NT_set, A);
	    state = reduce_NT_hd [A];
	    reduce_NT_hd [A] = 0;

	    pool_open (sir_pool, sir, prodmax);

	    do
	    {
		SXBA_0_bit (reduce_state_set, state);
		pool_next (psir, sir_pool, sir);
		psir->state = state;
		psir->index_set = reduce_index_sets [state];
		psir->backward_index_set = backward_reduce_index_sets [state];
		backward_reduce_index_sets [state] = NULL;
	    } while ((state = reduce_state_stack [state]) != 0);

	    pool_close (sir_pool, sir, RT [i+1].reduce [A].bot, RT [i+1].reduce [A].top);
	}
    }
}

#if 0
static void
fill_T2 (j, A, i)
    int j, A, i;
{
    sis		*top, *bot, *psis;
    int		state;
    SXBA	index_set, index_set2;

    bot = RT [j].shift [A].bot;

#if EBUG
    if (bot == NULL)
	sxtrap (ME, "fill_T2");
#endif

    for (top = RT [j].shift [A].top; bot < top; bot++)
    {
	state = bot->state;

#if EBUG
	if (state <= 0)
	    sxtrap (ME, "fill_T2");
#endif

	if (lispro [state] != 0 && SXBA_bit_is_set (FIRST_1[-TOK [i+2]], state))
	{
	    /* On met B -> beta A . Y gamma dans T2 */
	    index_set = bot->index_set;

	    if (SXBA_bit_is_reset_set (T2_state_set, state))
	    {
		if (index_set != NULL)
		{
		    SXBA_1_bit (old_state_set, state);
		    PUSH (T2_state_stack, state);
		    T2_index_sets [state] = index_set;
		}
		else
		{
		    SXBA_0_bit (old_state_set, state);
		    T2_index_sets [state] = index_set2 = bag_get (&shift_bag, i+1);
		    SXBA_1_bit (index_set2, i);
		}
	    }
	    else
	    {
		if (SXBA_bit_is_set_reset (old_state_set, state))
		{
		    index_set2 = bag_get (&shift_bag, i+1);
		    OR (index_set2, T2_index_sets [state]);
		    T2_index_sets [state] = index_set2;
		}

		if (index_set != NULL)
		    OR (T2_index_sets [state], index_set);
		else
		    SXBA_1_bit (T2_index_sets [state], i);
	    }
	}
    }
}
#endif

#if 0
static void
scan_reduce (i)
    register int 	i;
{
    int 	A, state, k, x;
    SXBA	index_set, index_set2;
    int		*rss;
    sis		*psis;

    /* scan/reduce sur ai+1 */

    while (!IS_EMPTY (nk_scan_reduce_state_stack))
    {
	state = POP (nk_scan_reduce_state_stack);/* state = A -> . ai+1 */
	A = lhs [prolis [state]];

	if (is_parser)
	{
	    if (SXBA_bit_is_reset_set (NT_set, A))
		PUSH (NT_stack, A);

	    state++;
	    SXBA_1_bit (reduce_state_set, state);
	    PUSH (reduce_state_stack [A], state);
	    index_set2 = reduce_index_sets [state] = bag_get (&shift_bag, i+1);
	    SXBA_1_bit (index_set2, i);
	}

	if (!shift (i, A, i))
	    fill_T2 (i, A, i);
    }

    while (!IS_EMPTY (k_scan_reduce_state_stack))
    {
	state = POP (k_scan_reduce_state_stack); /* state = A -> alpha . ai+1 */
	A = lhs [prolis [state]];
	index_set = T1_index_sets [state];

	if (is_parser)
	{
	    if (SXBA_bit_is_reset_set (NT_set, A))
		PUSH (NT_stack, A);

	    state++;
	    SXBA_1_bit (reduce_state_set, state);
	    PUSH (reduce_state_stack [A], state);
	    index_set2 = reduce_index_sets [state] = bag_get (&shift_bag, i+1);
	    OR (index_set2, index_set);
	}

	k = i;

	while ((k = sxba_1_rlscan (index_set, k)) >= 0)
	{
	    if (!shift (k, A, i))
		fill_T2 (k, A, i);
	}
    }

    if (is_parser)
    {
	while (!IS_EMPTY (NT_stack))
	{
	    A = POP (NT_stack);
	    SXBA_0_bit (NT_set, A);
	    rss = reduce_state_stack [A];

	    pool_open (sis_pool, sis, prodmax);

	    while (!IS_EMPTY (rss))
	    {
		state = POP (rss);
		SXBA_0_bit (reduce_state_set, state);
		pool_next (psis, sis_pool, sis);
		psis->state = state;
		psis->index_set = reduce_index_sets [state];
		psis->backward_index_set = backward_reduce_index_sets [state];
	    }

	    pool_close (sis_pool, sis, RT [i+1].reduce [A].bot, RT [i+1].reduce [A].top);
	}
    }
}
#endif


static bool
complete (i)
    int i;
{
    int		x, state, next_state, X, Y, A, B, j, prdct_no, prod, Aik, start, end;
    SXBA	index_set, backward_index_set;
    bool	is_tok, is_scan_reduce = false;

    sis		*psis, *bot, *top;

    /* Le look-ahead est verifie pour tous les state de T1_state_set. */
    /* De plus si A -> \alpha X . ai+1 \beta, le predicat de ai+1 est verifie'
       ainsi que FIRST1[\beta]. */

    if ((state = RT1->nt_hd [0]) != 0)
    {
	/* A -> \alpha X . ai+1 \beta */
	is_tok = true;
	RT1->nt_hd [0] = 0;

	do
	{
	    SXBA_0_bit (T1_shift_state_set, state);
	    next_state = state + 1;

	    Y = lispro [next_state];
	    index_set = T1_index_sets [state];

	    if (Y != 0)
	    {
		if (Y < 0)
		{
		    /* A -> \alpha X . ai+1 ai+2 \beta' */
		    if (((prdct_no = prdct [state]) == -1 ||
			 for_parsact.prdct == NULL ||
			 (*for_parsact.prdct) (-i-2, prdct_no)))
			Y = 0;
		}

		if (Y >= 0)
		{
		    if (Y > 0 && RT2->nt_hd [Y] == 0)
			PUSH (T2_shift_NT_stack, Y);

		    SXBA_1_bit (T2_shift_state_set, next_state);
		    RT2->state_stack [next_state] = RT2->nt_hd [Y];
		    RT2->nt_hd [Y] = next_state;
		    T2_index_sets [next_state] = index_set;
		}
	    }
	    else
	    {
		/* A -> \alpha X . ai+1 */
		is_scan_reduce = true;

		A = lhs [prolis [state]];
		OR (ntXindex_set [A], index_set);

		if (is_parser)
		{
		    if (SXBA_bit_is_reset_set (reduce_NT_set, A))
			PUSH (reduce_NT_stack, A);

		    SXBA_1_bit (reduce_state_set, next_state);
		    reduce_state_stack [next_state] = reduce_NT_hd [A];
		    reduce_NT_hd [A] = next_state;
		}
	    }
	} while ((state = RT1->state_stack [state]) != 0);
    }

    if (!IS_EMPTY (T1_shift_NT_stack))
    {
	do
	{
	    sxba_or (init_state_set, INIT [POP (T1_shift_NT_stack)]);
	} while (!IS_EMPTY (T1_shift_NT_stack));

	sxba_and (init_state_set, FIRST_1[-TOK [i+1]]);

	state = 0;

	while ((state = sxba_scan_reset (init_state_set, state)) > 0)
	{
	    /* A -> . X beta */
	    X = lispro [state];

	    if (X > 0)
	    {
		RT1->state_stack [state] = RT1->nt_hd [X];
		RT1->nt_hd [X] = -state;
			    
	    }
	    else
	    {
		next_state = state + 1;

		if (SXBA_bit_is_set (FIRST_1[-TOK [i+2]], next_state) &&
		    ((prdct_no = prdct [state]) == -1 ||
		     for_parsact.prdct == NULL ||
		     (*for_parsact.prdct) (-i-1, prdct_no)))
		{
		    is_tok = true;
		    Y = lispro [next_state];

		    if (Y != 0)
		    {
			/* A -> . ai+1 Y beta, i */
			if (Y < 0)
			{
			    if (((prdct_no = prdct [state]) == -1 ||
				 for_parsact.prdct == NULL ||
				 (*for_parsact.prdct) (-i-2, prdct_no)))
				/* A -> \alpha X . ai+1 ai+2 \beta' */
				Y = 0;
			}
			else
			    if (RT2->nt_hd [Y] == 0)
				PUSH (T2_shift_NT_stack, Y);

			if (Y >= 0)
			{
			    SXBA_1_bit (T2_shift_state_set, next_state);
			    RT2->state_stack [next_state] = RT2->nt_hd [Y];
			    RT2->nt_hd [Y] = next_state;
			    index_set = T2_index_sets [next_state] = bag_get (&shift_bag, i+1);
			    SXBA_1_bit (index_set, i);
			}
		    }
		    else
		    {
			/* A -> . ai+1, i */
			is_scan_reduce = true;

			A = lhs [prolis [state]];
			SXBA_1_bit (ntXindex_set [A], i);

			if (is_parser)
			{
			    if (SXBA_bit_is_reset_set (reduce_NT_set, A))
				PUSH (reduce_NT_stack, A);

			    SXBA_1_bit (reduce_state_set, next_state);
			    reduce_state_stack [next_state] = reduce_NT_hd [A];
			    reduce_NT_hd [A] = next_state;
			}
		    }
		}
	    }
	}
    }

    sxba_empty (T1_shift_state_set);

    if (is_scan_reduce)
	scan_reduce (i);

    return is_tok;
}





static int
recognize ()
{
    int			i;
    int			*T0_shift_NT_stack;
    SXBA		T0_shift_state_set;
    bool		is_in_LG;

    /* initial_state ne vaut pas toujours 1 (cas ou L(G)={epsilon}). */
    T1_shift_NT_stack = shift_NT_stack_1;
    T2_shift_NT_stack = shift_NT_stack_2;

    T1_index_sets = &(T_index_sets [0].index_sets [0]);
    T2_index_sets = &(T_index_sets [1].index_sets [0]);

    if (is_parser)
	T2_backward_index_sets = &(T_index_sets [1].backward_index_sets [0]);

    PUSH (T1_shift_NT_stack, 1 /* start symbol */);
    SXBA_1_bit (T1_shift_state_set, initial_state);
    RT1 = &(RT [0]);
    RT2 = RT1 + 1;

    RT1->nt_hd [1] = initial_state;
    /* RT1->state_stack [initial_state] = 0; pas de suivant */

    if (SXBA_bit_is_set (FIRST_1[-TOK [1]], initial_state))
    {
	i = 0;

	for (;;)
	{
	    if (!complete (i)) break;

	    if (++i == n)
		break;

	    RT1 = RT2, RT2 += 1;

	    T0_shift_state_set = T1_shift_state_set,
	    T1_shift_state_set = T2_shift_state_set,
	    T2_shift_state_set = T0_shift_state_set;

	    T0_shift_NT_stack = T1_shift_NT_stack,
	    T1_shift_NT_stack = T2_shift_NT_stack,
	    T2_shift_NT_stack = T0_shift_NT_stack;

	    T1_index_sets = T2_index_sets;
	    T2_index_sets = &(T_index_sets [i+1].index_sets [0]);

	    if (is_parser)
	    {
		T2_backward_index_sets = &(T_index_sets [i+1].backward_index_sets [0]);
	    }
	}
    }

    is_in_LG = SXBA_bit_is_set (T2_shift_state_set, 2) && SXBA_bit_is_set (T2_index_sets [2], 0);
    
    return is_in_LG ? n+1 : i+1;
}






/******************************************************************************************

                             P A R S E R


 ******************************************************************************************/





#if EBUG
static void
output_spf ()
{
    int		prod;

    printf ("\n*\t(|N| = %i, |T| = %i, |P| = %i, |G| = %i)\n",
	  spf.G.N,  
	  spf.G.T,  
	  spf.G.P,  
	  spf.G.G);

    fputs ("*\tThe start symbol is ", stdout);

    if (spf.G.start_symbol > 0)
	output_symb (spf.G.start_symbol);
    else
	fputs ("undefined", stdout);

    fputs ("\n\n", stdout);

    for (prod = 1; prod <= spf.G.lhs_top; prod++)
	output_prod (prod);
}
#endif	


erase_elementary_tree (elementary_tree)
    int elementary_tree;
{
    int		lhs_symb;
    int		*p, *q;
    struct lhs	*pet = spf.lhs+elementary_tree;

    if (!pet->is_erased)
    {
	pet->is_erased = true;

	for (q = p = &(Aij_pool [pet->reduc].first_lhs); *p != 0; p = &(spf.lhs [*p].next_lhs))
	{
	    if (*p == elementary_tree)
	    {
		*p = spf.lhs [*p].next_lhs;
		break;
	    }
	}

	if (*q == 0)
	{
	    /* plus aucun arbre n'a lhs_symb comme racine. On supprime les arbres elementaires
	       qui ont lhs_symb en RHS. */
	    for (q = p = &(Aij_pool [pet->reduc].first_rhs); *p != 0; p = &(spf.rhs [*p].next_rhs))
	    {
		erase_elementary_tree (spf.rhs [*p].prolis);
	    }

	    *q = 0;
	}
    }
}





static void
useless_symbol_elimination ()
{
    int		*symbol_stack;
    SXBA	Aij_set;
    int		symbol, prod, x;

    symbol_stack = SS_alloc (Aij_top+3);
    Aij_set = sxba_calloc (Aij_top+1);

    SXBA_1_bit (Aij_set, spf.G.start_symbol);
    SS_push (symbol_stack, spf.G.start_symbol);
    spf.G.N = 1;
    spf.G.P = spf.G.G = 0;

    do
    {
	symbol = SS_pop (symbol_stack);

	for (prod = Aij_pool [symbol].first_lhs; prod != 0; prod = spf.lhs [prod].next_lhs)
	{
	    if (!spf.lhs [prod].is_erased)
	    {
		spf.G.P++;
		spf.G.G++;

		for (x = spf.lhs [prod].prolon; (symbol = spf.rhs [x].lispro) != 0; x++)
		{
		    spf.G.G++;

		    if (symbol > 0 && SXBA_bit_is_reset_set (Aij_set, symbol))
		    {
			spf.G.N++;
			SS_push (symbol_stack, symbol);
		    }
		}
	    }
	}
    } while (!SS_is_empty (symbol_stack));

    symbol = Aij_top+1;

    while ((symbol = sxba_0_rlscan (Aij_set, symbol)) > 0)
    {
	/* symbol est inaccessible */
	for (prod = Aij_pool [symbol].first_lhs; prod != 0; prod = spf.lhs [prod].next_lhs)
	{
	    /* prod est inaccessible */
	    spf.lhs [prod].is_erased = true;
	}

	Aij_pool [symbol].first_lhs = Aij_pool [symbol].first_rhs = 0;
    }

    SS_free (symbol_stack);
    sxfree (Aij_set);
}




struct ARN_sem {
  int	elementary_tree, len;
};

static struct ARN_sem	*ARN_sem;


static void
ARN_sem_oflw (old_size, new_size)
    int old_size, new_size;
{
    ARN_sem = (struct ARN_sem*) sxrealloc (ARN_sem, new_size+1, sizeof (struct ARN_sem));
}


static void
ARN_new_symbol (new_symbol)
    int new_symbol;
{
    ARN_sem [new_symbol].elementary_tree = ARN_sem [new_symbol].len = 0;
}

#if 0
static bool
ARN_parsact (rhs_stack, top, elementary_tree, init_prod)
    int *rhs_stack, top, elementary_tree, init_prod;
{
    int			new_len;
    struct ARN_sem	*psem;

    psem = ARN_sem+rhs_stack [0];

    switch (parsact [init_prod])
    {
    case -1:
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 10:
    case 11:
    case 12:
    case 13:
	return true;
       
    case 7:
	new_len = 1;
	break;
    case 8:
	new_len = ARN_sem [rhs_stack [1]].len+1;
	break;
    case 9:
	new_len = ARN_sem [rhs_stack [2]].len+1;
	break;

    case 14:
	psem->len = ARN_sem [rhs_stack [1]].len+1; 
	return true;
    case 15:
	psem->len = 1;
	return true;

    default:
	fprintf (sxstderr, "The function \"ARN_parsact\" is out of date with respect to its specification.\n");
	abort ();
    }

    if (psem->elementary_tree == 0)
    {
	/* 1ere occurrence */
	psem->elementary_tree = elementary_tree;
	psem->len = new_len;
    }
    else
	if (psem->len <= new_len)
	{
	    /* On conserve la precedente */
	    return false;
	    /* erase_elementary_tree (elementary_tree); n'a pas (encore) ete entre'! */
	}
    else
    {
	erase_elementary_tree (psem->elementary_tree);
	psem->elementary_tree = elementary_tree;
	psem->len = new_len;
    }

    return true;
}
#endif

static bool
ARN_parsact (init_prod, i, j_set, k, nb)
    int 	init_prod, i, k, nb;
    SXBA	j_set;
{
    /* A FAIRE */
    /* Pour l'instant, on conserve un seul element ds j_set */
    int d, prev_d;

    if (nb > 1) {
	sxba_empty (j_set);
	return false;
    }

    d = sxba_scan (j_set, -1);
    sxba_empty (j_set);

#if EBUG
    if (d < 0)
	sxtrap (ME, "ARN_parsact");
#endif

    SXBA_1_bit (j_set, d);
    return true;
}


static bool
ARN_constraint (symbol, prdct_no)
    int symbol, prdct_no;
{
    if (prdct_no == 1)
    {
	return ARN_sem [symbol].len <= 2 /* 30 */;
    }
    else
    {
	fprintf (sxstderr, "The function \"ARN_constraint\" is out of date with respect to its specification.\n");
	abort ();
    }
}


static void
RL_put (X, i, j)
    int X, i, j;
{
    SXBA	tbp_set, p_set;

#if EBUG
    if (i >= j)
	sxtrap (ME, "RL_put");
#endif

    if ((tbp_set = RT [j].tbp [X].set) == NULL)
    {
	tbp_set = RT [j].tbp [X].set = bag_get (&shift_bag, j+1);
	p_set = RT [j].p [X] = bag_get (&shift_bag, j+1);
	SXBA_1_bit (ntXindex_set [X], j);
    }
    else
    {
	p_set = RT [j].p [X];

	if (SXBA_bit_is_set (p_set, i) || SXBA_bit_is_set (tbp_set, i))
	    return;
    }

    SXBA_1_bit (tbp_set, i);
    RT [j].tbp [X].card++;
    RT [j].tbp [0].card++;
}

#if 0
/* Ca marche mais on essaie avec des ensembles!! */
static void
RL_reduction_item (item, i, j, pbot)
    int 	item, i, j;
    sir		*pbot;
{
    /* item = A -> \alpha . \gamma  et \alpha != \epsilon */
    int		X, Y, bot_state, k, new_j;
    SXBA	index_set, backward_index_set, prev_index_set;
    bool	is_reduce;

    /* On saute les terminaux a droit de \alpha */
    bot_state = prolon [prolis [item]];
    is_reduce = pbot != NULL;
    new_j = j;

    while ((Y = lispro [item-1]) < 0)
    {
	is_reduce = false;
	new_j--;

	if (--item == bot_state)
	{
#if EBUG
	    if (i != new_j)
		sxtrap (ME, "RL_reduction_item");
#endif
	    return;
	}
    }

    if (is_reduce)
    {
	index_set = pbot->index_set;
	backward_index_set = pbot->backward_index_set;
    }
    else
    {
	index_set = T_index_sets [new_j].index_sets [item];
	backward_index_set = T_index_sets [new_j].backward_index_sets [item];
    }


    if (SXBA_bit_is_set (index_set, i))
    {
	item--;
	/* item = X -> \alpha . Y x \gamma */

	if (item == bot_state)
	{
	    /* \alpha = \epsilon */
#if EBUG
	    if (!SXBA_bit_is_set (backward_index_set, i))
		sxtrap (ME, "RL_reduction_item");
#endif    

	    RL_put (Y, i, new_j);
	}
	else
	{
	    k = i;

	    while ((k = sxba_scan (backward_index_set, k)) >= 0)
	    {
		prev_index_set = T_index_sets [k].index_sets [item];
#if EBUG
		if (prev_index_set == NULL)
		    sxtrap (ME, "RL_reduction_item");
#endif
		if (SXBA_bit_is_set (prev_index_set, i))
		{
		    /* C'est un bon k */
		    RL_put (Y, k, new_j);
		    RL_reduction_item (item, i, k, NULL);
		}
	    }
	}
    }
}



static void
RL_reduction ()
{
    int		i, j, A, *gcard, *card;
    SXBA	tbp_set, p_set;
    sir		*pbot, *ptop;

    for (j = n; j >= 0; j--)
    {
	gcard = &(RT [j].tbp [0].card);

	while (*gcard > 0)
	{
	    for (A = 1; A <= ntmax; A++)
	    {
		if (*(card = &(RT [j].tbp [A].card)) > 0)
		{
		    tbp_set = RT [j].tbp [A].set;
		    p_set = RT [j].p [A];
		    
		    do
		    {
			i = -1;

			while ((i = sxba_scan_reset (tbp_set, i)) >= 0)
			{
			    SXBA_1_bit (p_set, i);
			    (*gcard)--;
			    (*card)--;
			
			    for (ptop = RT [j].reduce [A].top, pbot = RT [j].reduce [A].bot; pbot < ptop; pbot++)
				RL_reduction_item (pbot->state, i, j, pbot);
			}
		    } while (*card > 0);
		}
	    }
	}
    }
}
#endif

static void
RL_mreduction_item (item, I, j, pbot)
    int 	item, j;
    sir		*pbot;
    SXBA	I;
{
    /* item = A -> \alpha . \gamma  et \alpha != \epsilon */
    int		Y, bot_state, i, k, new_j;
    SXBA	index_set, backward_index_set, prev_index_set;
    bool	is_reduce;

    /* On saute les terminaux a droite de \alpha */
    bot_state = prolon [prolis [item]];
    is_reduce = pbot != NULL;
    new_j = j;

    while ((Y = lispro [item-1]) < 0)
    {
	is_reduce = false;
	new_j--;

	if (--item == bot_state)
	{
#if EBUG
	    if (!SXBA_bit_is_set (I, new_j))
		sxtrap (ME, "RL_mreduction_item");
#endif
	    return;
	}
    }

    if (is_reduce)
    {
	index_set = pbot->index_set;
	backward_index_set = pbot->backward_index_set;
    }
    else
    {
	index_set = T_index_sets [new_j].index_sets [item];
	backward_index_set = T_index_sets [new_j].backward_index_sets [item];
    }

    if (IS_AND (index_set, I))
    {
	item--;
	/* item = X -> \alpha . Y x \gamma */

	if (item == bot_state)
	{
	    /* \alpha = \epsilon */
#if EBUG
	    if (!IS_AND (backward_index_set, I))
		sxtrap (ME, "RL_mreduction_item");
#endif    

	    i = -1;

	    while ((i = sxba_scan (index_set, i)) >= 0)
	    {
		if (SXBA_bit_is_set (I, i))
		    RL_put (Y, i, new_j);
	    }
	}
	else
	{
	    k = -1;

	    while ((k = sxba_scan (backward_index_set, k)) >= 0)
	    {
		prev_index_set = T_index_sets [k].index_sets [item];
#if EBUG
		if (prev_index_set == NULL)
		    sxtrap (ME, "RL_mreduction_item");
#endif
		if (IS_AND (prev_index_set, I))
		{
		    /* C'est un bon k */
		    RL_put (Y, k, new_j);
		    RL_mreduction_item (item, I, k, NULL);
		}
	    }
	}
    }
}



static void
RL_mreduction ()
{
    int		i, j, A, *gcard, *card;
    SXBA	tbp_set, p_set;
    sir		*pbot, *ptop;

    for (j = n; j >= 0; j--)
    {
	gcard = &(RT [j].tbp [0].card);

	while (*gcard > 0)
	{
	    for (A = 1; A <= ntmax; A++)
	    {
		if (*(card = &(RT [j].tbp [A].card)) > 0)
		{
		    tbp_set = RT [j].tbp [A].set;

#if EBUG
		    if (*card != sxba_cardinal (tbp_set))
			sxtrap (ME, "RL_mreduction");
#endif

		    p_set = RT [j].p [A];
		    
		    do
		    {
			*gcard -= *card;
			*card = 0;
			COPY (working_set, tbp_set); /* On copie aussi la dimension de tbp_set!!! */
			OR_RAZ (p_set, tbp_set);

			for (ptop = RT [j].reduce [A].top, pbot = RT [j].reduce [A].bot; pbot < ptop; pbot++)
			    RL_mreduction_item (pbot->state, working_set, j, pbot);
		    } while (*card > 0);
		}
	    }
	}
    }
}



sem_item (state, I_set, level)
    int 	state, level;
    SXBA	I_set;
{
    int		h, i, symbol, j, k, x, A, item;

#if EBUG
    if (I_set == NULL || sxba_scan (I_set, -1) < 0)
	sxtrap (ME, "sem");
#endif

    /* Si la rhs commence par des terminaux, la partie correspondante aux terminaux
       de rhs_stack n'est pas remplie. */

    if (level == 0)
    {
	if (is_print_prod)
	{
	    k = rhs_stack [*rhs_stack];
	    A = lhs [prolis [state]];
	    i = -1;

	    while ((i = sxba_scan (I_set, i)) >= 0)
	    {
		/* sortir (i,j,k) */
		xprod++;
		printf ("%i\t: ", xprod);
		print_symb (A, i, k);
		fputs ("\t = ", stdout);

		x = 1;
		j = i;
		item = state;

		while ((symbol = lispro [item++]) != 0)
		{
		    h = (symbol > 0) ? rhs_stack [x] : j+1;
		    x++;
		    print_symb (symbol, j, h);
		    j = h;
		}
    
		fputs (";\n", stdout);
	    }
	}
	else
	    xprod += sxba_cardinal (I_set);
    }

    return true;
}

static bool
sem_Xij (X, I_set, j)
    int		X, j;
    SXBA	I_set;
{
#if EBUG
    if (I_set == NULL || sxba_scan (I_set, -1) < 0)
	sxtrap (ME, "sem_Xij");
#endif

    if (is_print_prod)
    {
	printf ("* fin de la generation des %s[", ntstring [X]);
	print_SXBA (I_set); 
	printf (", %i]\n", j);
    }

    return true;
}


static bool
parse_item (item, I, backward_J, level, new_I)
    int 	item, level;
    SXBA	I, backward_J, new_I;
{
    sir		*pbot, *ptop;
    int		Y, bot_state, j, d, new_j, nbt;
    bool	ret_val = false;
    SXBA	index_set, level_index_set, backward_index_set;

    level_index_set = level_index_sets [level]; /* empty */

    nbt = 0;

    if (item > (bot_state = prolon [prolis [item]]))
    {
	/* On saute les terminaux precedant X */
	while ((Y = lispro [item-1]) < 0)
	{
	    nbt++;

	    if (--item == bot_state)
		break;
	}

	level -= nbt;
    }

    j = -1;

    while ((j = sxba_scan (backward_J, j)) >= 0)
    {
	new_j = j - nbt;

	if (item == bot_state && SXBA_bit_is_set (I, new_j) ||
	    (index_set = T_index_sets [new_j].index_sets [item]) != NULL && IS_AND (index_set, I))
	    SXBA_1_bit (level_index_set, new_j);
    }

    if (sem_item (item, level_index_set, level))
    {
	/* level_index_set a pu etre reduit. */
	if (item == bot_state)
	{
	    OR_RAZ (new_I, level_index_set);
	    ret_val = true;
	}
	else
	{
	    new_j = -1;

	    while ((new_j = sxba_scan_reset (level_index_set, new_j)) >= 0)
	    {
		if (IS_AND (T_index_sets [new_j].index_sets [item], I))
		{

		    rhs_stack [level] = new_j;

		    if (nbt > 0)
		    {
			d = 1;

			do
			{
			    rhs_stack [level+d] = new_j+d;
			} while (++d <= nbt);
		    }

		    if (parse_item (item-1, I, T_index_sets [new_j].backward_index_sets [item], level-1, new_I))
			ret_val = true;
		}
	    }
	}
    }

    return ret_val;
}


static bool
parse_Xij (X, j)
    int 	X, j;
{
    sir		*pbot, *ptop;
    SXBA	I, backward_set, backward_index_set, level_index_set, index_set;
    int		level, item, bot_state, new_j, Y;
    bool	is_reduce, ret_val = false;
    /* On genere toutes les reductions dont Xij est la partie gauche et i \in backward_set */
    /* backward_set est repositionne' */

    backward_set = bag_get (&shift_bag, j+1);

    I = RT [j].p [X];

    for (ptop = RT [j].reduce [X].top, pbot = RT [j].reduce [X].bot; pbot < ptop; pbot++)
    {
	/* On saute les terminaux les + a droite */
	item = pbot->state;
	bot_state = prolon [prolis [item]];
	level = item - bot_state;
	rhs_stack [0] = level;
	rhs_stack [level] = j;
	is_reduce = true;
	new_j = j;

	while ((Y = lispro [item-1]) < 0)
	{
	    is_reduce = false;
	    new_j--;
	    level--;
	    item--;

	    if (level == 0)
	    {
		/* La rhs ne contient que des terminaux */
		if (SXBA_bit_is_set (I, new_j))
		{

		    level_index_set = level_index_sets [0];
		    SXBA_1_bit (level_index_set, new_j);
		
		    if (sem_item (item, level_index_set, 0))
		    {
			SXBA_0_bit (level_index_set, new_j);
			SXBA_1_bit (backward_set, new_j);
			ret_val = true;
		    }
		}

		break;
	    }
	    else
		rhs_stack [level] = new_j;
	}
	
	if (level > 0)
	{
	    if (is_reduce)
	    {
		index_set = pbot->index_set;
		backward_index_set = pbot->backward_index_set;
	    }
	    else
	    {
		index_set = T_index_sets [new_j].index_sets [item];
		backward_index_set = T_index_sets [new_j].backward_index_sets [item];
	    }

	    if (IS_AND (index_set, I))
	    {
		if (parse_item (item-1, I, backward_index_set, level-1, backward_set))
		    ret_val = true;
	    }
	}
    }

    if (ret_val)
    {
	RT [j].p [X] = backward_set;
	sem_Xij (X, backward_set, j);
    }
    else
	RT [j].p [X] = NULL;
}



static void
parse ()
{
    register SXBA_ELT	filtre;
    register int	j, order, A;

    for (A = ntmax; A > 0; A--)
	nt_hd [A] = ntXindex_set [A] + 1;

    filtre = 1;

    for (j = 0; j <= n;j++)
    {
	for (order = ntmax; order > 0; order--)
	{
	    A = order2nt [order];

	    /* if (SXBA_bit_is_set (ntXindex_set [A], j)) */
	    if (*(nt_hd [A]) & filtre)
		parse_Xij (A, j);
	}

	if ((filtre <<= 1) == 0)
	{
	    filtre = 1;

	    for (A = ntmax; A > 0; A--)
		(nt_hd [A])++; 
	}
    }
}




#if 0
static void
reduce_Delta ()
{
    /* Si (i, A-> \alpha .X \beta, j) \in Delta et (j, A-> \alpha X . \beta, k) \not\in Delta
       => on supprime (i, A-> \alpha .X \beta, j) de Delta */
    int	erased = 0;
    int	j, ixqxj, jxrxk, q, r;
    bool	found;

    for (j = n; j >= 0; j--)
    {
	XxYxZ_Zforeach (Delta, j, ixqxj)
	{
	    q = XxYxZ_Y (Delta, ixqxj);

	    if (lispro [r = q+1] != 0)
	    {
		found = false;

		XxYxZ_XYforeach (Delta, j, r, jxrxk)
		{
		    found = true;
		    break;
		}

		if (!found)
		{
		    XxYxZ_erase (Delta, ixqxj);
		    erased++;
		}
	    
	    }
	}
    }

    printf ("Delta_top = %i, Delta_erased = %i\n", XxYxZ_top (Delta), erased);
}
#endif





main (argc, argv)
    int		argc;
    char	*argv [];
{
    int l, t, *tok, nt;
    char	*s, c;

    if (argc != 3)
    {
	printf (Usage, ME);
	return 1;
    }

    if (sxstdout == NULL) {
	sxstdout = stdout;
    }
    if (sxstderr == NULL) {
	sxstderr = stderr;
    }

    sxopentty ();

    c = argv [1] [0];

    if (c == 'R' || c == 'r')
	is_parser = false;
    else
	if (c == 'P' || c == 'p')
	    is_parser = true;
    else
    {
	printf (Usage, ME);
	return 1;
    }
	    
    c = argv [1] [1];

    if (c == 'p')
	is_print_prod = true;
    else
	is_print_prod = false;
	    
    s = source = argv [2];
    n = strlen (source);

    if (n == 0)
    {
	printf (Usage, ME);
	return 1;
    }

    tok = TOK = (int*) sxalloc (n + 2, sizeof (int));

    while ((c = *s++) != '\0')
    {
	if ((t = char2tok (c)) == 0)
	{
	    printf ("\
\t%s Lexical error at char #%i\n", ME, (TOK - tok) + 1);
	    return 1;
	}

	*++tok = t;
    }

    *++tok = tmax;

    pool_alloc (&sis_pool, "sis_pool", (n+1)*prodmax + statemax, sizeof (sis));

    pool_alloc (&sir_pool, "sir_pool", (n+1)*prodmax + statemax, sizeof (sir));

    bag_alloc (&shift_bag, "shift_bag", (n + 1) * prodmax * NBLONGS (n + 1));

    RT = (struct recognize_item*) sxcalloc (n+1, sizeof (struct recognize_item));
    T_index_sets = (struct T_index_sets*) sxcalloc (n+1, sizeof (struct T_index_sets));

    if (is_parser)
    {
#if EBUG
	debug_set1 = sxba_calloc (n+1);
	debug_set2 = sxba_calloc (n+1);
#endif

	level_index_sets = sxbm_calloc (rhs_lgth + 2, n+1);
	working_set = level_index_sets [rhs_lgth + 1];
    }

    global_state_sets = sxbm_calloc (6, statemax+1);

    init_state_set = global_state_sets [0];
    T1_shift_state_set = global_state_sets [1];
    T2_shift_state_set = global_state_sets [2];
    state_set = global_state_sets [3];
    old_state_set = global_state_sets [4];
    reduce_state_set = global_state_sets [5];

    reduce_NT_set = sxba_calloc (ntmax+1);

    ntXindex_set = sxbm_calloc (ntmax+1, n+1);

#if 0
    if (is_parser)
    {
	Aij_pool = (struct Aij_pool*) sxalloc (Aij_size = ntmax * (n+1), sizeof (struct Aij_pool));
	/* Aij_top = 0; */

        {
	    int			*CP;
	    int 		A, i, d;

	    /* Acces a l'element A,i,j de symbols se fait par symbols [A] [i] [j-i]. */
	    CP = symbols = (int*) sxcalloc (((n+1)*(n+2)/2)*ntmax, sizeof (int));

	    for (i = 0, d = n+1; i <= n; i++, d--) {
		for (A = 1; A <= ntmax; A++) {
		    RT [i].nt [A].Aij = CP;
		    CP += d;
		}
	    }
	}
	
        {
	    int	*p, prod;

	    p = global_reduce_hd = (int*) sxcalloc (prodmax * (n+1), sizeof (int));

	    for (prod = 1; prod <= prodmax; prod++) {
		reduce_hd [prod] = p;
		p += n+1;
	    }
	}

	reduce_list = (struct reduce_list*) sxalloc (reduce_list_top = ntmax * (n+1),
						     sizeof (struct reduce_list));
	reduce_list_cur = 0;

	bag_alloc (&reduce_bag, "reduce_bag", (n + 1) * ntmax * NBLONGS (n + 1));

	spf.G.lhs_size = ntmax * (n+1) * (n+1);
	spf.lhs = (struct lhs*) sxalloc (spf.G.lhs_size+1, sizeof (struct lhs));

	spf.G.rhs_size = spf.G.lhs_size * (rhs_lgth+1);
	spf.rhs = (struct rhs*) sxalloc (spf.G.rhs_size+1, sizeof (struct rhs));

	ARN_sem = (struct ARN_sem*) sxalloc (Aij_size+1, sizeof (struct ARN_sem));
    }
#endif

    grammar ();

    sprod2order = (int*) sxcalloc (prodmax+1, sizeof (int));

    grammar2 ();

    sprod_set = sxba_calloc (sprodmax+1);
    sprod2attr = (struct sprod2attr*) sxalloc (sprodmax+1, sizeof (struct sprod2attr));

    for_parsact.action = ARN_parsact;
    for_parsact.new_symbol = ARN_new_symbol;
    for_parsact.Aij_pool_oflw = ARN_sem_oflw;
    for_parsact.prdct = NULL;
    for_parsact.constraint = NULL; /* ARN_constraint */

    if ((l = recognize ()) <= n)
	printf ("Syntax error at %i\n", l);
    else
    {
	if (is_parser)
	{
	    RL_put (1 /* axiome */, 0, n);
#if 0
	    RL_reduction ();
#endif
	    RL_mreduction ();

	    parse ();
	    printf ("Nombre de productions de la foret partagee : %i\n", xprod);

	}

#if 0
	if (is_parser)
	{
	    sir		*pbot, *ptop;
	    int		state, i, bot_state, X;
	    bool	is_empty;
	    SXBA	index_set, index_set2;

	    HT = (struct RL_item*) sxcalloc (n+1, sizeof (struct RL_item));
    
	    /* On cherche ds RT [n].reduce les S -> gamma., I */
	    HT [n].nt [1] = index_set = bag_get (&shift_bag, n+1);
	    index_set2 = bag_get (&shift_bag, n+1);
	    SXBA_1_bit (index_set2, 0);
	    is_empty = true;

	    for (ptop = RT [n].reduce [1].top, pbot = RT [n].reduce [1].bot; pbot < ptop; pbot++)
	    {
		if (SXBA_bit_is_set (pbot->index_set, 0))
		{
		    state = pbot->state;
		    bot_state = prolon [prolis [state]];

		    /* On saute les terminaux en fin de partie droite */
		    i = n;

		    while ((X = lispro [--state]) < 0 && state > bot_state)
			i--;
		
		    if (X > 0)
		    {
			if (RL_reduce (state, index_set2, i, n, index_set))
			    is_empty = false;
		    }
		    else
		    {
			if (sem (bot_state, index_set2, i, n))
			{
			    is_empty = false;
			    SXBA_1_bit (index_set, 0);
			}
		    }
		}
	    }

	    if (!is_empty)
	    {
		sem_Xij (1 /* axiome */, index_set, n);

		/* Il faut sortir la foret partagee */
	    }

	    sxfree (HT);

#if 0
	    spf.G.N = Aij_top;
	    spf.G.P = spf.G.lhs_top;
	    spf.G.T = n;
	    spf.G.G = spf.G.rhs_top;

	    if (spf.G.start_symbol != 0)
		useless_symbol_elimination ();

#if EBUG
	    output_spf ();
#endif

#endif
	}
#endif

#if 0
	if (is_parser)
	{
	    sis		*pbot, *ptop;
	    int		state, prod, bot, top, i, cur, bot_state, X;
	    bool	ret_val;

	    reduce_Delta ();

	    /* On cherche ds RT [n].reduce les S -> gamma., I */
	    for (ptop = RT [n].reduce [1].top, pbot = RT [n].reduce [1].bot; pbot < ptop; pbot++)
	    {
		state = pbot->state;
		prod = prolis [state];

		if (SXBA_bit_is_set_reset (pbot->index_set, 0))
		{
		    cur = top = state - (bot_state = prolon [prod]);
		    rhs_stack [top] = i = n;

		    while (state > bot_state && (X = lispro [state - 1]) < 0)
		    {
#if EBUG
			if (TOK [i] != X)
			    sxtrap (ME, "main");
#endif

			rhs_stack [--cur] = --i;
			state--;
		    }		    

		    if (state == bot_state)
		    {
			/* La rhs ne contient que des terminaux */
			print_prod (rhs_stack, bot_state, top);

#if EBUG
			if (cur != 0 || i != 0)
			    sxtrap (ME, "main");
#endif

			/* index_set doit etre vide */
#if EBUG
			if (sxba_scan (pbot->index_set, -1) != -1)
			    sxtrap (ME, "bu_parse_tree");
#endif	

		    }
		    else
		    {
			rhs_stack [bot = 0] = i = 0;

			while ((X = lispro [bot_state]) < 0)
			{
			    /* La partie droite commence par des terminaux */
#if EBUG
			    if (TOK [i+1] != X)
				sxtrap (ME, "main");
#endif

			    rhs_stack [++bot] = ++i;
			    bot_state++;
			}

			ret_val = bu_parse_tree (rhs_stack, state, bot, cur, top);

#if EBUG
			if (!ret_val)
			    sxtrap (ME, "main");
#endif
		    }
		}
	    }

	    printf ("Nombre de productions de la foret partagee : %i\n", xprod);

#if 0
	    spf.G.N = Aij_top;
	    spf.G.P = spf.G.lhs_top;
	    spf.G.T = n;
	    spf.G.G = spf.G.rhs_top;

	    if (spf.G.start_symbol != 0)
		useless_symbol_elimination ();

#if EBUG
	    output_spf ();
#endif

#endif
	}
#endif	
    }

    grammar_free ();

    sxfree (sprod2order);
    sxfree (sprod_set);
    sxfree (sprod2attr);

    pool_free (&sis_pool);

    pool_free (&sir_pool);

    bag_free (&shift_bag);

    sxfree (T_index_sets);
    sxfree (RT);
    sxbm_free (global_state_sets);

    sxfree (reduce_NT_set);

    sxbm_free (ntXindex_set);

    if (is_parser)
    {
#if EBUG
	sxfree (debug_set1);
	sxfree (debug_set2);
#endif
	sxbm_free (level_index_sets);
    }	


#if 0
    if (is_parser)
    {
	sxfree (Aij_pool);

	sxfree (symbols);

	sxfree (global_reduce_hd);
	sxfree (reduce_list);
	bag_free (&reduce_bag);

	sxfree (spf.rhs);
	sxfree (spf.lhs);

	sxfree (ARN_sem);
    }
#endif

    sxfree (TOK);

    return 0;
}
