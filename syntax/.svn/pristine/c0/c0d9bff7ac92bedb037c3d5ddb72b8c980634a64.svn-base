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
  char		**hd, *cur_top, *cur_bot, *pool_lim;
  int		hd_size, hd_top, pool_size, elem_size;
  int		used_size, total_size; /* #if EBUG */
} pool_header;

#define pool_next(b,t)	(((((b).cur_top+=(b).elem_size)>=(b).pool_lim)?(t*)pool_oflw(&(b)):(t*)((b).cur_top)))

#define pool_close(b,t,bot,top) 			\
                        (bot = ((t*)(b).cur_bot),	\
                        top = ((t*)(b).cur_top),	\
			(b).cur_bot = (b).cur_top + (b).elem_size)


#include <memory.h>


static void
pool_alloc (ppool, pool_size, elem_size)
    pool_header	*ppool;
    int 	pool_size, elem_size;
{
    ppool->hd_top = 0;
    ppool->hd_size = 1;
    ppool->elem_size = sizeof (char) * elem_size;
    ppool->pool_size = pool_size * ppool->elem_size;
    ppool->hd = (char**) sxalloc (ppool->hd_size, sizeof (char*));
    ppool->pool_lim = ppool->cur_top = ppool->cur_bot = ppool->hd [ppool->hd_top] =
	    (char*) sxalloc (ppool->pool_size, ppool->elem_size);
    ppool->cur_bot += ppool->elem_size;
    ppool->pool_lim += ppool->pool_size;

#if EBUG
    ppool->total_size = ppool->pool_size;
#endif
}



static char*
pool_oflw (ppool)
    pool_header	*ppool;
{
    char	*ptr = ppool->cur_bot;
    int 	n = ppool->pool_lim - ppool->cur_bot;

    if (++ppool->hd_top >= ppool->hd_size)
	ppool->hd = (char**) sxrealloc (ppool->hd, ppool->hd_size *= 2, sizeof (char*));

#if EBUG
    printf ("New pool of size %i is created.\n", ppool->pool_size);
    ppool->used_size = ppool->total_size;
    ppool->total_size += ppool->pool_size;
#endif

    ppool->pool_lim = ppool->cur_bot = ppool->hd [ppool->hd_top] =
	(char*) sxalloc (ppool->pool_size, sizeof (char));

    ppool->cur_bot += ppool->elem_size;
    ppool->pool_lim += ppool->pool_size;

    memcpy (ppool->cur_bot, ptr, n);

    return ppool->cur_top = ppool->cur_bot + n;
}



static void
pool_free (ppool)
    pool_header	*ppool;
{
#if EBUG
    printf ("Pool: used_size = %i bytes, total_size = %i bytes\n",
	    (ppool->used_size + (ppool->cur_bot - ppool->hd [ppool->hd_top]))
	     + (ppool->hd_top + 1) * sizeof (int),
    	    ppool->total_size + ppool->hd_size * sizeof (int));
#endif

    while (--ppool->hd_top >= 0)
	sxfree (ppool->hd [ppool->hd_top]);

    sxfree (ppool->hd);
}

/*   END of POOL_MNGR */


FILE	*sxtty, *sxstdout = {stdout}, *sxstderr = {stderr};

static struct LC0 {
  int NKshift, lgth;
};

struct for_parsact {
    void	(*new_symbol) (),
                (*Aij_pool_oflw) ();

    SXBOOLEAN	(*action) (),
    		(*prdct) (),
                (*constraint) ();
};


/* #if 0 */
/*
<RNA>		@1 = <RNA> <DoubleStrand> ;
<RNA>		@2 = <DoubleStrand> ;
<RNA>		@2 = <RNA> <Base> ;
<RNA>		@3 = <Base> ;

<DoubleStrand>	@4 = A <SingleStrand> &1 U ;
<DoubleStrand>	@5 = A <InternalLoop> &1 U ;
<DoubleStrand>	@6 = A <MultipleLoop> U ;
<DoubleStrand>	@4 = G <SingleStrand> &1 C ;
<DoubleStrand>	@5 = G <InternalLoop> &1 C ;
<DoubleStrand>	@6 = G <MultipleLoop> C ;
<DoubleStrand>	@4 = G <SingleStrand> &1 U ;
<DoubleStrand>	@5 = G <InternalLoop> &1 U ;
<DoubleStrand>	@6 = G <MultipleLoop> U ;
<DoubleStrand>	@4 = U <SingleStrand> &1 A ;
<DoubleStrand>	@5 = U <InternalLoop> &1 A ;
<DoubleStrand>	@6 = U <MultipleLoop> A ;
<DoubleStrand>	@4 = C <SingleStrand> &1 G ;
<DoubleStrand>	@5 = C <InternalLoop> &1 G ;
<DoubleStrand>	@6 = C <MultipleLoop> G ;
<DoubleStrand>	@4 = U <SingleStrand> &1 G ;
<DoubleStrand>	@5 = U <InternalLoop> &1 G ;
<DoubleStrand>	@6 = U <MultipleLoop> G ;

<InternalLoop>	@7 = <DoubleStrand> ;
<InternalLoop>	@8 = <InternalLoop> &1 <Base> ;
<InternalLoop>	@9 = <Base> <InternalLoop> &1 ;

<MultipleLoop>	@10 = <MultipleLoop> <DoubleStrand> ;
<MultipleLoop>	@11 = <DoubleStrand> ;
<MultipleLoop>	@12 = <MultipleLoop> <Base> ;
<MultipleLoop>	@13 = <Base> ;

<Base>		= A ;
<Base>		= U ;
<Base>		= C ;
<Base>		= G ;

<SingleStrand>	@14 = <SingleStrand> &1 <Base> ;
<SingleStrand>	@15 = <Base> ;

* @7 : len = 0;
* @8 : len = $1.len +1;
* @9 : len = $2.len +1;

* @14 :len = S1.len + 1;
* @15 : len = 0;

* &1 SXFALSE <==> len > 30

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

static char	ME [] = "earley";

static char	Usage [] = "\
Usage:\t%s ('R' | 'r' |'P' | 'p') \"source string\"\n";


static int		n;
static int		*TOK;
static char		*source;

static SXBA		*INIT;
static SXBA		*FIRST_1;
static SXBA		*NT2CHAIN;

typedef struct {
  SXBA_ELT	**hd, *pool_top, *pool_lim;
  int		hd_size, hd_top, pool_size, room;
  int		used_size, total_size; /* #if EBUG */
} bag_header;


static SXBOOLEAN		is_parser;

typedef struct {
  int			state;
  SXBA			index_set;
} sis;

static pool_header	sis_pool;


struct R_shift {
  sis		*bot, *top;
  int		*Aij;
};

static struct recognize_item {
  struct R_shift	nt [ntmax+1];
}			*RT;

static bag_header	shift_bag;

static struct working_item_set {
  int		state_stack [statemax + 1];
  SXBA		state_set;
  SXBA		index_sets [statemax + 1];
}			T1, T2;


static SXBA		*global_state_sets;

static SXBA		NT_set;

static SXBA		init_state_set;

static SXBA		*ntXindex_set;

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
	SXBOOLEAN		is_erased;
    } *lhs;
};

static struct spf	spf;

static int		rhs_stack [rhs_lgth+1];

static struct Aij_pool {
  int		A, i, j, first_lhs, first_rhs;
  SXBOOLEAN	is_erased;
}			*Aij_pool;
static int		Aij_top, Aij_size;

static int		*symbols;
/* Acces a l'element A,i,j se fait par RT [i].nt [A].Aij [j-i]. */

/* end is_parser */





static void
grammar ()
{
    /* On remplit INIT, NT2CHAIN et FIRST_1 */
    int		prod, state, X, Y, A, B, x, lim, t;
    SXBA	*NT2NT, *NT2T;

    INIT = sxbm_calloc (ntmax + 1, statemax + 1);

    for (X = 1; X <= ntmax; X++)
    {
	for (x = LC0 [X].NKshift, lim = x + LC0 [X].lgth; x < lim; x++)
	{
	    state = NKshift [x] - 1;
	    SXBA_1_bit (INIT [X], state);
	}
    }

    /* NT2CHAIN [A] = { U -> alpha V. X beta | V ->* A} */

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

    /* FIRST_1 [a] = {U -> alpha . beta | a \in T et a \in FIRST(beta)LOOK-AHEAD(U -> alpha V beta)} */

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

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	*lhs_bits_ptr-- |= *rhs_bits_ptr--;
    }
}


static void
bag_alloc (pbag, size)
    bag_header	*pbag;
    int 	size;
{
    SXBA_ELT	*ptr;

    pbag->hd_top = 0;
    pbag->hd_size = 1;
    pbag->pool_size = size;
    pbag->hd = (SXBA_ELT**) sxalloc (pbag->hd_size, sizeof (SXBA_ELT*));
    pbag->pool_lim = pbag->pool_top = pbag->hd [pbag->hd_top] =
	    (SXBA_ELT*) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));
    pbag->pool_lim += pbag->pool_size + 1;
    pbag->room = MUL (pbag->pool_size);

#if EBUG
    pbag->total_size = pbag->pool_size + 2;
#endif
}



static SXBA
bag_get (pbag, size)
    bag_header	*pbag;
    int		size;
{
    int 	slice_nb = NBLONGS (size) + 1;
    SXBA	set;

    if (size > pbag->room)
    {
	if (++pbag->hd_top >= pbag->hd_size)
	    pbag->hd = (SXBA*) sxrealloc (pbag->hd, pbag->hd_size *= 2, sizeof (SXBA));

	while (size > (pbag->room = MUL (pbag->pool_size)))
	    pbag->pool_size *= 2;

#if EBUG
	printf ("New bag of size %i is created.\n", pbag->pool_size);
	pbag->used_size = pbag->total_size;
	pbag->total_size += pbag->pool_size + 2;
#endif

	pbag->pool_lim = pbag->pool_top = pbag->hd [pbag->hd_top] =
	    (SXBA) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));

	pbag->pool_lim += pbag->pool_size + 1;
    }

    *(set = pbag->pool_top) = size;
    pbag->pool_top += slice_nb;
    pbag->room -= MUL (slice_nb);

    return set;
}



static void
bag_free (pbag)
    bag_header	*pbag;
{
#if EBUG
    printf ("Bag: used_size = %i bytes, total_size = %i bytes\n",
	    (pbag->used_size + (pbag->pool_top - pbag->hd [pbag->hd_top])) * sizeof (SXBA_ELT)
	     + (pbag->hd_top + 1) * sizeof (int),
    	    pbag->total_size * sizeof (SXBA_ELT) + pbag->hd_size * sizeof (int));
#endif

    while (--pbag->hd_top >= 0)
	sxfree (pbag->hd [pbag->hd_top]);

    sxfree (pbag->hd);
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

    if (for_parsact.action == NULL ||
	(*for_parsact.action) (rhs_stack, top, spf.G.lhs_top, init_prod))
    {

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
	plhs->is_erased = SXFALSE;

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

    spf.G.lhs_top--;
    return 0;
}


static SXBOOLEAN
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
    SXBOOLEAN	ret_val;
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
	ret_val = SXFALSE;

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
    SXBOOLEAN	constraint_checked;


    if (is_parser)
    {
	Aji = RT [j].nt [A].Aij [i-j];

#if EBUG
	if (Aji == 0)
	    sxtrap (ME, "reduce");
#endif
	
	constraint_checked = SXTRUE;

	/* On est su^r que toutes les reductions vers Aji ont ete effectuees. */
	if ((constraint_no = constraints [A]) == -1 ||
	    for_parsact.constraint == NULL ||
	    (constraint_checked = (*for_parsact.constraint) (Aji, constraint_no)))
	{
	}
	else
	    if (!constraint_checked)
		Aij_pool [Aji].is_erased = SXTRUE;

	if (!constraint_checked)
	    return;
    }

    for (top = RT [j].nt [A].top, bot = RT [j].nt [A].bot; bot <= top; bot++)
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
		    /* B -> . A , j*/
		    SXBA_1_bit (ntXindex_set [B], j);

		    if (is_parser)
		    {
			if ((Bji = RT [j].nt [B].Aij [i-j]) == 0)
			    RT [j].nt [B].Aij [i-j] = Bji = set_symbol (B, j, i);

			rhs_stack [0] = Bji;
			rhs_stack [1] = Aji;
			set_rule (prod, 1);
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

			    rhs_stack [0] = Bki;
			    start = prolon [prod];
			    end = state - start;
			    rhs_stack [end] = Aji;
			    parse_tree (start, 1, end, k, j);
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
		    if (SXBA_bit_is_reset_set (T2.state_set, state))
			PUSH (T2.state_stack, state);

		    if ((index_set2 = T2.index_sets [state]) == NULL)
			index_set2 = T2.index_sets [state] = bag_get (&shift_bag, j+1);

		    if (index_set == NULL)
			SXBA_1_bit (index_set2, j);
		    else
			OR (index_set2, index_set);
		}
	    }
	}
    }
}


static void
scan_reduce (i)
    register int 	i;
{
    static SXBA		nt_hd [ntmax+1];

    register SXBA_ELT	filtre;
    register int	j, indice, order, A;

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
		return;

	    filtre = 1 << MOD (j);
	}
	else
	    j--;
    }
}



static SXBOOLEAN
complete (i)
    int i;
{
    static int	shift_state_stack [ntmax + 1] [statemax + 1];
    static int	NT_stack [ntmax+1];

    int		state, next_state, X, Y, A, B, j, prdct_no, prod, Aik, start, end;
    SXBA	index_set;
    SXBOOLEAN	is_tok, is_scan_reduce = SXFALSE;

    sis		*psis, *bot, *top;
    int		*sss;
    /* Le look-ahead est verifie pour tous les state de T1.state_set. */

    do
    {
	/* A -> alpha Z . X Y beta	ou
	   S' -> . S eof 		pour l'etat initial. */
	state = POP (T1.state_stack);
	SXBA_0_bit (T1.state_set, state);
	X = lispro [state];

	if (X > 0)
	{
	    if (SXBA_bit_is_reset_set (NT_set, X))
	    {
		PUSH (NT_stack, X);
		sxba_or (init_state_set, INIT [X]);
	    }

	    PUSH (shift_state_stack [X], state);
	}
	else
	{
	    next_state = state + 1;

	    if (SXBA_bit_is_set (FIRST_1[-TOK [i+2]], next_state) &&
		((prdct_no = prdct [state]) == -1 ||
		 for_parsact.prdct == NULL ||
		 (*for_parsact.prdct) (-i-1, prdct_no)))
	    {
		is_tok = SXTRUE;
		Y = lispro [next_state];
		index_set = T1.index_sets [state];

		if (Y != 0)
		{
		    SXBA_1_bit (T2.state_set, next_state);
		    PUSH (T2.state_stack, next_state);
		    T2.index_sets [next_state] = index_set;
		}
		else
		{
		    /* A -> alpha X . ai+1 */
		    is_scan_reduce = SXTRUE;
		    A = lhs [prod = prolis [state]];

		    OR (ntXindex_set [A], index_set);

		    if (is_parser)
		    {
			j = i;

			while ((j = sxba_1_rlscan (index_set, j)) >= 0)
			{
			    if ((Aik = RT [j].nt [A].Aij [i+1-j]) == 0)
				RT [j].nt [A].Aij [i+1-j] = Aik = set_symbol (A, j, i+1);

			    rhs_stack [0] = Aik;
			    start = prolon [prod];
			    end = next_state - start;
			    rhs_stack [end] = -i - 1;
			    parse_tree (start, 1, end, j, i);
			}
		    }
		}

		T1.index_sets [state] = NULL;
	    }
	}
    } while (!IS_EMPTY (T1.state_stack));

    if (!IS_EMPTY (NT_stack))
    {
	sxba_and (init_state_set, FIRST_1[-TOK [i+1]]);

	state = 0;

	while ((state = sxba_scan_reset (init_state_set, state)) > 0)
	{
	    /* A -> . X beta */
	    X = lispro [state];

	    if (X > 0)
	    {
		if (SXBA_bit_is_reset_set (NT_set, X))
		{
		    PUSH (NT_stack, X);
		}

		PUSH (shift_state_stack [X], -state);
	    }
	    else
	    {
		next_state = state + 1;

		if (SXBA_bit_is_set (FIRST_1[-TOK [i+2]], next_state) &&
		    ((prdct_no = prdct [state]) == -1 ||
		     for_parsact.prdct == NULL ||
		     (*for_parsact.prdct) (-i-1, prdct_no)))
		{
		    is_tok = SXTRUE;
		    Y = lispro [next_state];

		    if (Y != 0)
		    {
			/* A -> . ai+1 Y beta, i */
			SXBA_1_bit (T2.state_set, next_state);
			PUSH (T2.state_stack, next_state);

			index_set = T2.index_sets [next_state] = bag_get (&shift_bag, i+1);

			SXBA_1_bit (index_set, i);
		    }
		    else
		    {
			/* A -> . ai+1, i */
			is_scan_reduce = SXTRUE;
			A = lhs [prod = prolis [state]];

			SXBA_1_bit (ntXindex_set [A], i);

			if (is_parser)
			{
#if EBUG
			    if (RT [i].nt [A].Aij [1] != 0)
				sxtrap (ME, "complete");
#endif

			    rhs_stack [0] = RT [i].nt [A].Aij [1] = set_symbol (A, i, i+1);
			    rhs_stack [1] = -i - 1;
			    set_rule (prod, 1);
			}
		    }
		}
	    }
	}

#if 0
	if (is_parser) {
	    order = ntmax+1;

	    while ((order = sxba_1_rlscan (order_set, order)) > 0)
	    {
		X = order2nt [order];
		XxY_set (&Ai_hd, X, i, &(RT [i].nt [X].Ai));
	    }
	}
#endif

	do
	{
	    X = POP (NT_stack);
	    SXBA_0_bit (NT_set, X);
	    sss = shift_state_stack [X];

	    do
	    {
		state = POP (sss);
		psis = pool_next (sis_pool, sis);

		if (state > 0)
		{
		    psis->index_set = T1.index_sets [state];
		    T1.index_sets [state] = NULL;
		}
		else
		{
		    state = -state;
		    psis->index_set = NULL;
		}

		psis->state = state;
	    } while (!IS_EMPTY (sss));

	    pool_close (sis_pool, sis, RT [i].nt [X].bot, RT [i].nt [X].top);
	} while (!IS_EMPTY (NT_stack));
    }

    if (is_scan_reduce)
	scan_reduce (i);

    return is_tok;
}




static int
recognize ()
{
    int			i;
    struct working_item_set	T0;
    SXBOOLEAN		is_in_LG;

    /* initial_state ne vaut pas toujours 1 (cas ou L(G)={epsilon}). */

    SXBA_1_bit (T1.state_set, initial_state);
    PUSH (T1.state_stack, initial_state);
    /* SXBA_1_bit (T1.index_sets [initial_state], 0); */

    if (SXBA_bit_is_set (FIRST_1[-TOK [1]], initial_state))
    {
	i = 0;

	for (;;)
	{
	    if (!complete (i)) break;

	    T0 = T1, T1 = T2, T2 = T0;

	    i++;

	    if (i == n)
		break;
	}
    }

    is_in_LG = SXBA_bit_is_set (T1.state_set, 2) && SXBA_bit_is_set (T1.index_sets [2], 0);
    
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
	pet->is_erased = SXTRUE;

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
	    spf.lhs [prod].is_erased = SXTRUE;
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

static SXBOOLEAN
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
	return SXTRUE;
       
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
	return SXTRUE;
    case 15:
	psem->len = 1;
	return SXTRUE;

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
	    return SXFALSE;
	    /* erase_elementary_tree (elementary_tree); n'a pas (encore) ete entre'! */
	}
    else
    {
	erase_elementary_tree (psem->elementary_tree);
	psem->elementary_tree = elementary_tree;
	psem->len = new_len;
    }

    return SXTRUE;
}

static SXBOOLEAN
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

    sxopentty ();

    c = argv [1] [0];

    if (c == 'R' || c == 'r')
	is_parser = SXFALSE;
    else
	if (c == 'P' || c == 'p')
	    is_parser = SXTRUE;
    else
    {
	printf (Usage, ME);
	return 1;
    }
	    
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

    pool_alloc (&sis_pool, (n+1)*ntmax, sizeof (sis));

    bag_alloc (&shift_bag, (n + 1) * ntmax * NBLONGS (n + 1));

    RT = (struct recognize_item*) sxalloc (n+1, sizeof (struct recognize_item));

    global_state_sets = sxbm_calloc (3, statemax+1);

    init_state_set = global_state_sets [0];
    T1.state_set = global_state_sets [1];
    T2.state_set = global_state_sets [2];

    NT_set = sxba_calloc (ntmax+1);

    ntXindex_set = sxbm_calloc (ntmax+1, n+1);

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
	

	spf.G.lhs_size = ntmax * (n+1) * (n+1);
	spf.lhs = (struct lhs*) sxalloc (spf.G.lhs_size+1, sizeof (struct lhs));

	spf.G.rhs_size = spf.G.lhs_size * (rhs_lgth+1);
	spf.rhs = (struct rhs*) sxalloc (spf.G.rhs_size+1, sizeof (struct rhs));

	ARN_sem = (struct ARN_sem*) sxalloc (Aij_size+1, sizeof (struct ARN_sem));
    }


    grammar ();

    sprod2order = (int*) sxcalloc (prodmax+1, sizeof (int));

    grammar2 ();

    sprod_set = sxba_calloc (sprodmax+1);
    sprod2attr = (struct sprod2attr*) sxalloc (sprodmax+1, sizeof (struct sprod2attr));

    for_parsact.action = ARN_parsact;
    for_parsact.new_symbol = ARN_new_symbol;
    for_parsact.Aij_pool_oflw = ARN_sem_oflw;
    for_parsact.prdct = NULL;
    for_parsact.constraint = ARN_constraint;

    if ((l = recognize ()) <= n)
	printf ("Syntax error at %i\n", l);
    else
    {
	if (is_parser)
	{
	    spf.G.N = Aij_top;
	    spf.G.P = spf.G.lhs_top;
	    spf.G.T = n;
	    spf.G.G = spf.G.rhs_top;

	    if (spf.G.start_symbol != 0)
		useless_symbol_elimination ();

#if EBUG
	    output_spf ();
#endif
	}
    }

    grammar_free ();

    sxfree (sprod2order);
    sxfree (sprod_set);
    sxfree (sprod2attr);

    pool_free (&sis_pool);

    bag_free (&shift_bag);

    sxfree (RT);
    sxbm_free (global_state_sets);
    sxfree (NT_set);

    sxbm_free (ntXindex_set);

    if (is_parser)
    {
	sxfree (Aij_pool);

	sxfree (symbols);

	sxfree (spf.rhs);
	sxfree (spf.lhs);

	sxfree (ARN_sem);
    }

    sxfree (TOK);

    return 0;
}

