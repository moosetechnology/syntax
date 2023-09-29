static int	IT, IT_incl;

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

FILE	*sxtty, *sxstdout = {stdout}, *sxstderr = {stderr};

static struct LC0 {
  int NKshift, lgth;
};

struct for_parsact {
    void	(*new_symbol) (),
                (*Aij_hd_oflw) ();

    BOOLEAN	(*action) (),
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

* &1 FALSE <==> len > 30

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


static SXBA		to_be_processed;

static BOOLEAN		is_parser;


static XxY_header	shift_items;

static struct recognize_item {
  int			max_item;
  SXBA_ELT		*nt [ntmax+1];
}			*RT;

static bag_header	shift_bag;

static struct working_item_set {
  int		state_stack [statemax + 1];
  SXBA		state_set;
  SXBA		*index_sets;
}			T1, T2;


static SXBA		*global_state_sets, *global_index_sets;

static SXBA		NT_set, order_shift_set;
static int		NT_stack [ntmax+1];

static int		shift_state_stack [ntmax + 1] [statemax + 1];

static SXBA		init_state_set, shift_state_set, shift_reduce_state_set, scan_reduce_state_set;
/* static int		global_state_stacks [4*(statemax+1)]; */
static int		shift_reduce_state_stack [statemax+1], scan_reduce_state_stack [statemax+1];

/* is_parser */
struct shift_reduce {
  int		top, Ai, Aik;
  int		state [maxntlon];
  SXBA		index_set [maxntlon];
};

struct scan_reduce {
  int		top;
  int		state [maxtlon];
  SXBA		index_set [maxtlon];
};

static struct parse_item {
  struct shift_reduce	nt [ntmax + 1];
  struct scan_reduce	t;
  int			max_Ai;
}			*PT;


static SXBA		*index_set_pool;
static int		index_set_pool_top;


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
	BOOLEAN		is_erased;
    } *lhs;
};

static struct spf	spf;

static int		rhs_stack [rhs_lgth+1];

static XxY_header	Ai_hd;
static SXBA		Ai_set;


static XxY_header	Aij_hd;
static struct Aij2attr {
  int		first_lhs, first_rhs;
}			*Aij2attr;

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


static  void
shift_items_oflw (old_size, new_size)
    int		old_size, new_size;
{
    to_be_processed = sxba_resize (to_be_processed, new_size + 1);
}

#if 0
static void
EMPTY (bits_array, slices_number)
    SXBA	bits_array;
    int		slices_number;
{
    SXBA	bits_ptr = bits_array + slices_number;

    do
    {
	*bits_ptr-- = 0;
    } while (bits_ptr > bits_array);
}


static void
MINUS (lhs_bits_array, rhs_bits_array, slices_number)
    SXBA	lhs_bits_array, rhs_bits_array;
    int		slices_number;
/*
 * "OR" puts into its first argument the result of the bitwise
 * "OR" of the prefix of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
      *lhs_bits_ptr-- &= ~(*rhs_bits_ptr--)l
}


static void
COPY (lhs_bits_array, rhs_bits_array, slices_number)
    SXBA	lhs_bits_array, rhs_bits_array;
    int		slices_number;
/*
 * "OR" puts into its first argument the result of the bitwise
 * "OR" of the prefix of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
      *lhs_bits_ptr-- = *rhs_bits_ptr--;
}


static void
SHIFT (bits_array)
    SXBA	bits_array;
{
#define SHIFT_80 (~((~(0L))>>1))

    register SXBA	ptop = bits_array + NBLONGS (BASIZE (bits_array));
    BOOLEAN 		is_r2, is_r = FALSE;


    while (++bits_array <= ptop)
    {
	is_r2 = *bits_array & SHIFT_80;

	*bits_array <<= 1;

	if (is_r)
	    *bits_array |= 1;

	is_r = is_r2;
    }
}
#endif




static void
OR (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
      *lhs_bits_ptr-- |= *rhs_bits_ptr--;
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



#define bag_open(b)	(b).pool_top



static SXBA_ELT*
bag_oflw (pbag, bit)
    bag_header	*pbag;
    int		bit;
{
    SXBA_ELT	*ptr;

    if (++pbag->hd_top >= pbag->hd_size)
	pbag->hd = (SXBA_ELT**) sxrealloc (pbag->hd, pbag->hd_size *= 2, sizeof (SXBA_ELT*));

     while (bit >= (pbag->room = MUL (pbag->pool_size)))
    {
	pbag->pool_size *= 2;
    }

#if EBUG
    printf ("New bag of size %i is created.\n", pbag->pool_size);
    pbag->used_size = pbag->total_size;
    pbag->total_size += pbag->pool_size + 2;
#endif

    ptr = pbag->hd [pbag->hd_top] = (SXBA_ELT*) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));

    while (pbag->pool_top < pbag->pool_lim)
	*ptr++ = *pbag->pool_top++;

    pbag->pool_lim = pbag->pool_top = pbag->hd [pbag->hd_top];
    pbag->pool_lim += pbag->pool_size + 1;

    return pbag->pool_top;
}



static void
bag_close (pbag, i)
    bag_header	*pbag;
    int i;
{
    int slice_nb;

    *(pbag->pool_top) = i+1;

    slice_nb = NBLONGS (i+1) + 1;
    pbag->pool_top += slice_nb;

    pbag->room -= MUL (slice_nb);
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
    
    
static void
bag_or (lhs_bag_ptr, rhs_bag_ptr, slices_number)
    SXBA_ELT	*lhs_bag_ptr, *rhs_bag_ptr;
    int		slices_number;
{
    register SXBA_ELT	*lhs_bits_ptr = lhs_bag_ptr + slices_number, *rhs_bits_ptr = rhs_bag_ptr + slices_number;

    while (slices_number-- > 0)
      *--lhs_bits_ptr |= *--rhs_bits_ptr;
}


static void
bag_copy_reset (lhs_bag_ptr, rhs_bag_ptr, slices_number)
    SXBA_ELT	*lhs_bag_ptr, *rhs_bag_ptr;
    int		slices_number;
{
    register SXBA_ELT	*lhs_bits_ptr = lhs_bag_ptr + slices_number, *rhs_bits_ptr = rhs_bag_ptr + slices_number;

    while (slices_number-- > 0)
    {
	*--lhs_bits_ptr = *--rhs_bits_ptr;
	*rhs_bits_ptr = 0;
    }
}


static BOOLEAN
complete (i)
    int i;
{
    int		state, next_state, X, Y, A, B, j, prod, item, prev_state, prev_index, order;
    int		max_item, prdct_no;
    SXBA	item_set, index_set;
    BOOLEAN	is_tok, is_chain, is_shift = FALSE;
    int		*stack;
    SXBA_ELT	**p_Ti_ntB;

    struct shift_reduce	*pntc;
    int			x;
    struct scan_reduce	*ptc;


    /* Le look-ahead est verifie pour tous les state de T1.state_set. */

    do
    {
	/* A -> alpha Z . X Y beta	ou
	   S' -> . S eof 		pour l'etat initial. */
	state = POP (T1.state_stack);
	SXBA_0_bit (T1.state_set, state);
	X = lispro [state];
	Y = lispro [state+1];

	if (X > 0)
	{
	    if (SXBA_bit_is_reset_set (NT_set, X))
		PUSH (NT_stack, X);

	    if (Y != 0)
	    {
		is_shift = TRUE;

		if (SXBA_bit_is_reset_set (shift_state_set, state))
		{
		    order = nt2order [X];
		    SXBA_1_bit (order_shift_set, order);

		    PUSH (shift_state_stack [X], state);
		}
	    }
	    else
	    {
		if (SXBA_bit_is_reset_set (shift_reduce_state_set, state))
		    PUSH (shift_reduce_state_stack, state);
	    }
	}
	else
	{
	    next_state = state + 1;

	    if (SXBA_bit_is_set (FIRST_1[-TOK [i+2]], next_state) &&
		((prdct_no = prdct [state]) == -1 ||
		 for_parsact.prdct == NULL ||
		 (*for_parsact.prdct) (-i-1, prdct_no)))
	    {
		is_tok = TRUE;

		if (Y != 0)
		{
		    if (SXBA_bit_is_reset_set (T2.state_set, next_state))
		    {
			PUSH (T2.state_stack, next_state);
			index_set = T1.index_sets [state];
			T1.index_sets [state] = T2.index_sets [next_state];
			T2.index_sets [next_state] = index_set;
		    }
		}
		else
		{
		    if (SXBA_bit_is_reset_set (scan_reduce_state_set, state))
			PUSH (scan_reduce_state_stack, state);
		}
	    }
	}
    } while (!IS_EMPTY (T1.state_stack));

    is_chain = FALSE;

    if (!IS_EMPTY (NT_stack))
    {
	do
	{
	    X = POP (NT_stack);
	    SXBA_0_bit (NT_set, X);
	    sxba_or (init_state_set, INIT [X]);
	} while (!IS_EMPTY (NT_stack));
	
	sxba_and (init_state_set, FIRST_1[-TOK [i+1]]);

	state = 0;

	while ((state = sxba_scan_reset (init_state_set, state)) > 0)
	{
	    /* A -> . X beta */
	    X = lispro [state];
	    Y = lispro [state + 1];

	    if (is_parser)
	    {
		/* Pour creer les Ai dans le bon ordre :
		   Ai < Bj => i < j ou i == j et nt2order [A] < nt2order [B] */
		order = nt2order [lhs [prolis [state]]];
		SXBA_1_bit (NT_set, order);
	    }

	    if (X > 0)
	    {
		if (Y != 0)
		{
		    is_shift = TRUE;

		    if (SXBA_bit_is_reset_set (shift_state_set, state))
		    {
			order = nt2order [X];
			SXBA_1_bit (order_shift_set, order);

			PUSH (shift_state_stack [X], state);
		    }
		}
		else
		{
		    A = lhs [prod = prolis [state]];
		    order = sprod2order [prod];
		    SXBA_1_bit (sprod_set, order);
		    sprod2attr [order].A = A;
		    sprod2attr [order].B = X;

		    if (is_parser)
			sprod2attr [order].state = state;

		    is_chain = TRUE;
		}
	    }
	    else
	    {
		next_state = state + 1;

		if (SXBA_bit_is_set (FIRST_1[-TOK [i+2]], next_state) &&
		    ((prdct_no = prdct [state]) == -1 ||
		     for_parsact.prdct == NULL ||
		     (*for_parsact.prdct) (-i-1, prdct_no)))
		{
		    is_tok = TRUE;

		    if (Y != 0)
		    {
			if (SXBA_bit_is_reset_set (T2.state_set, next_state))
			    PUSH (T2.state_stack, next_state);

			SXBA_1_bit (T2.index_sets [next_state], i);
		    }
		    else
		    {
			if (SXBA_bit_is_reset_set (scan_reduce_state_set, state))
			    PUSH (scan_reduce_state_stack, state);
		    }
		}
	    }
	}

	if (is_parser)
	{
	    struct parse_item	*ppi = PT + i;

	    order = 0;

	    while ((order = sxba_scan_reset (NT_set, order)) > 0)
	    {
		A = order2nt [order];
		XxY_set (&Ai_hd, A, i, &(ppi->nt [A].Ai));
	    }

	    ppi->max_Ai = XxY_top (Ai_hd);
	}

	if (is_shift)
	{
	    /* Les NT doivent etre traites ds le bon ordre pour eviter le debordement des
	       bags sur les OR des productions simples. */
	    order = 0;

	    while ((order = sxba_scan_reset (order_shift_set, order)) > 0)
	    {
		B = order2nt [order];
		stack = shift_state_stack [B];
		p_Ti_ntB = &(RT [i].nt [B]); /* pour shift_items_oflw */
		*p_Ti_ntB = bag_open (shift_bag);

		do
		{
		    state = POP (stack);
		    SXBA_0_bit (shift_state_set, state);

		    if (prolon [prolis [state]] == state)
		    {
			/* A -> . B Y beta, i */
			XxY_set (&shift_items, state+1, i, &item);

			if (item >= shift_bag.room)
			    *p_Ti_ntB = bag_oflw (&shift_bag, item);

			SXBA_1_bit (*p_Ti_ntB, item);
		    }
		    else
		    {
			/* A -> alpha X . B Y beta */
			j = i;
			index_set = T1.index_sets [state];

			while ((j = sxba_1_rlscan (index_set, j)) >= 0)
			{
			    SXBA_0_bit (index_set, j);
			    XxY_set (&shift_items, state+1, j, &item);

			    if (item >= shift_bag.room)
				*p_Ti_ntB = bag_oflw (&shift_bag, item);

			    SXBA_1_bit (*p_Ti_ntB, item);
			}
		    }
		} while (!IS_EMPTY (stack));

		bag_close (&shift_bag, XxY_top (shift_items));
	    }
	}
    }

    max_item = RT [i].max_item = XxY_top (shift_items);

    while (!IS_EMPTY (shift_reduce_state_stack))
    {
	state = POP (shift_reduce_state_stack);
	SXBA_0_bit (shift_reduce_state_set, state);
	B = lispro [state];
	A = lhs [prolis [state]];

	/* A -> alpha X . B */
	
	index_set = T1.index_sets [state];

	j = sxba_1_rlscan (index_set, i);

	if (j >= 0)
	{

	    if ((item_set = RT [i].nt [B]) == NULL)
	    {
		item = RT [j].max_item;
		item_set = RT [i].nt [B] = (item >= shift_bag.room)
		    ? bag_oflw (&shift_bag, item) : bag_open (shift_bag);
		bag_close (&shift_bag, item);
	    }
	    
	    do
	    {
		if (!is_parser)
		    SXBA_0_bit (index_set, j);

		OR (item_set, RT [j].nt [A]);
	    } while ((j = sxba_1_rlscan (index_set, j)) >= 0);

	    if (is_parser)
	    {
		pntc = &(PT [i].nt [B]);
		x = pntc->top;
		pntc->state [x] = state;
		pntc->index_set [x] = T1.index_sets [state];
		T1.index_sets [state] = index_set_pool [++index_set_pool_top];
		pntc->top++;
	    }
	}
    }

    if (is_chain)
    {
	order = 0;

	while ((order = sxba_scan_reset (sprod_set, order)) > 0)
	{
	    /* A -> . B , i */
	    A = sprod2attr [order].A;
	    B = sprod2attr [order].B;

	    if ((item_set = RT [i].nt [B]) == NULL)
	    {
		item_set = RT [i].nt [B] = (max_item >= shift_bag.room)
		    ? bag_oflw (&shift_bag, item) : bag_open (shift_bag);
		bag_close (&shift_bag, max_item);
	    }

	    OR (item_set, RT [i].nt [A]);

	    if (is_parser)
	    {
		pntc = &(PT [i].nt [B]);
		x = pntc->top;
		pntc->state [x] = sprod2attr [order].state;
		/* pntc->index_set [x] == NULL */
		pntc->top++;
	    }
	}
    }

    if (!IS_EMPTY (scan_reduce_state_stack))
    {
	do
	{
	    state = POP (scan_reduce_state_stack);
	    SXBA_0_bit (scan_reduce_state_set, state);
	    A = lhs [prolis [state]];

	    if (prolon [prolis [state]] == state)
	    {
		/* A -> . ai+1, i */
		/* On met RT [i].nt [A] dans T2 */
		OR (to_be_processed, RT [i].nt [A]);

		if (is_parser)
		{
		    ptc = &(PT [i].t);
		    x = ptc->top;
		    ptc->state [x] = state;
		    /* ptc->index_set [x] == NULL */
		    ptc->top++;
		}
	    }
	    else
	    {
		/* A -> alpha X . ai+1 */
		int dum;

		j = i;
		index_set = T1.index_sets [state];

		while ((j = sxba_1_rlscan (index_set, j)) >= 0)
		{
		    if (!is_parser)
			SXBA_0_bit (index_set, j);

		    OR (to_be_processed, RT [j].nt [A]);
		}

		if (is_parser)
		{
		    ptc = &(PT [i].t);
		    x = ptc->top;
		    ptc->state [x] = state;
		    ptc->index_set [x] = index_set;
		    T1.index_sets [state] = index_set_pool [++index_set_pool_top];
		    ptc->top++;
		}
	    }
	} while (!IS_EMPTY (scan_reduce_state_stack));

	item = XxY_top (shift_items) + 1;

	/* while ((item = sxba_1_rlscan (item_set, item)) > 0) */
	while ((item = sxba_1_rlscan (to_be_processed, item)) > 0)
	{
	    SXBA_0_bit (to_be_processed, item);
	    prev_state = XxY_X (shift_items, item);

	    if (SXBA_bit_is_set (FIRST_1[-TOK [i+2]], prev_state))
	    {
		prev_index = XxY_Y (shift_items, item);

		if (SXBA_bit_is_reset_set (T2.state_set, prev_state))
		    PUSH (T2.state_stack, prev_state);

		SXBA_1_bit (T2.index_sets [prev_state], prev_index);
	    }
	}
    }

#if EBUG
    if (!sxba_is_empty (T1.state_set))
	sxtrap (ME, "complete");

    for (state = 1; state <= statemax; state++)
	if (!sxba_is_empty (T1.index_sets [state]))
	    sxtrap (ME, "complete");
#endif

#if EBUG
{
    /* On regarde s'il existe un j tq RT [j].nt [A] < RT [i].nt [A] */
    SXBA	item_set2;

    for (A = 1; A <= ntmax; A++)
    {
	if ((item_set = RT [i].nt [A]) != NULL)
	{
	    IT++;

	    j = i;

	    while (--j >= 0)
	    {
		if ((item_set2 = RT [j].nt [A]) != NULL)
		{
		    x = -1;

		    while ((x = sxba_scan (item_set2, x)) >= 0)
		    {
			if (!SXBA_bit_is_set (item_set, x))
			    break;
		    }

		    if (x < 0)
		    {
			IT_incl++;
			break;
		    }
		}
	    }
	}
    }
}

#endif

    return is_tok;
}
    




static int
recognize ()
{
    int			i;
    struct working_item_set	T0;
    BOOLEAN		is_in_LG;

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







static  void
Aij_hd_oflw (old_size, new_size)
    int		old_size, new_size;
{
    Aij2attr = (struct Aij2attr*) sxrealloc (Aij2attr, new_size+1, sizeof (struct Aij2attr));

    if (for_parsact.Aij_hd_oflw != NULL)
	(*for_parsact.Aij_hd_oflw) (old_size, new_size);
}

#if EBUG
static void
output_symb (symb)
    int	   symb;
{
    int Ai, A, i, j;

    if (symb < 0)
    {
	printf ("%c(%i) ", source [-symb-1], -symb);
    }
    else
    {
	Ai = XxY_X (Aij_hd, symb);
	j = XxY_Y (Aij_hd, symb);
	A = XxY_X (Ai_hd, Ai);
	i = XxY_Y (Ai_hd, Ai);
	printf ("%s[%i..%i] ", ntstring [A], i, j);
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
	pet->is_erased = TRUE;

	for (q = p = &(Aij2attr [pet->reduc].first_lhs); *p != 0; p = &(spf.lhs [*p].next_lhs))
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
	    for (q = p = &(Aij2attr [pet->reduc].first_rhs); *p != 0; p = &(spf.rhs [*p].next_rhs))
	    {
		erase_elementary_tree (spf.rhs [*p].prolis);
	    }

	    *q = 0;
	}
    }
}




static int
set_rule (init_prod, top)
    int init_prod, top;
{
    int		Aik, x, X, Xij, prdct_no, state;
    int		Ai, k, A, i, Xi;
    struct lhs	*plhs;
    struct rhs	*prhs;

#if EBUG
    /* On verifie la coherence de rhs_stack */
    Aik = rhs_stack [0];
    k = XxY_Y (Aij_hd, Aik);
    Ai = XxY_X (Aij_hd, Aik);
    i = XxY_Y (Ai_hd, Ai);
    A = XxY_X (Ai_hd, Ai);

    if (A != lhs [init_prod])
	sxtrap (ME, "set_rule");

    for (x = 1, state = prolon [init_prod]; x <= top; x++, state++)
    {
	Xij = rhs_stack [x];

	if (Xij > 0)
	{
	    Xi = XxY_X (Aij_hd, Xij);

	    if (XxY_Y (Ai_hd, Xi) != i || XxY_X (Ai_hd, Xi) != lispro [state])
		sxtrap (ME, "set_rule");

	    i = XxY_Y (Aij_hd, Xij);
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
	plhs->prolon = ++spf.G.rhs_top;
	plhs->reduc = Aik;
	plhs->next_lhs = Aij2attr [Aik].first_lhs;
	Aij2attr [Aik].first_lhs = spf.G.lhs_top;
	plhs->init_prod = init_prod;
	plhs->is_erased = FALSE;

	for (x = 1; x <= top; x++)
	{
	    X = rhs_stack [x];
	    prhs = spf.rhs+spf.G.rhs_top;
	    prhs->lispro = X;
	    prhs->prolis = spf.G.lhs_top;

	    if (X > 0)
	    {
		prhs->next_rhs = Aij2attr [X].first_rhs;
		Aij2attr [X].first_rhs = spf.G.rhs_top;
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



static BOOLEAN
set_symbol (Xj, k, Xjk)
    int Xj, k, *Xjk;

{
    struct Aij2attr	*psymb;
    BOOLEAN is_new;

    if (is_new = !XxY_set (&Aij_hd, Xj, k, Xjk))
    {
	psymb = Aij2attr+*Xjk;
	psymb->first_lhs = psymb->first_rhs = 0;

	if (for_parsact.new_symbol != NULL)
	    (*for_parsact.new_symbol) (*Xjk);
    }

#if EBUG
    /* Les reductions doivent se faire dans l'ordre (creation des LHS avant toute
       utilisation en RHS). */
    else
	if (Aij2attr [*Xjk].first_rhs != 0)
	    sxtrap (ME, "set_symbol");

#endif

    return is_new;
}



static  void
Ai_hd_oflw (old_size, new_size)
    int		old_size, new_size;
{
    Ai_set = sxba_resize (Ai_set, new_size+1);
}



static BOOLEAN
parse_tree (state, bot, top, j, l)
    int top, state, j, l;
{
    /* state =  A -> X1  Xh . X  Xp Xp+1
       bot = h+1
       top = p+1
       Cette procedure recursive genere toutes les split-chaines de "X ... Xp"
       qui s'etendent entre j et l et les range entre bot et top-1

       Quand c'est complet, appelle set_rule. */
    int		X, Xj, Xjl, Xjk, k, prdct_no;
    BOOLEAN	ret_val;

#if EBUG
    if (bot >= top || j >= l)
	sxtrap (ME, "parse_tree");
#endif

    X = lispro [state];

    if (X < 0)
    {
	/* X \in T */
	if (X == TOK [j+1])
	{
	    rhs_stack [bot] = -(j+1);

	    if (bot+1 == top)
	    {
#if EBUG
		if (j+1 != l)
		    sxtrap (ME, "parse_tree");
#endif
		ret_val = set_rule (prolis [state], top) > 0;
	    }
	    else
		ret_val = parse_tree (state+1, bot+1, top, j+1, l);
	}
    }
    else
    {
	ret_val = FALSE;

	/* X \in N */
	/* Xj = XxY_is_set (&Ai_hd, X, j); */
	Xj = PT [j].nt [X].Ai;

#if EBUG
	if (Xj == 0)
	    sxtrap (ME, "parse_tree");
#endif

	if (bot+1 == top)
	{
	    Xjl = XxY_is_set (&Aij_hd, Xj, l, &Xjl);

	    if (Xjl > 0 && Aij2attr [Xjl].first_lhs > 0)
	    {
		/* Si ==0 alors tous les arbres le definissant ont ete supprimes par
		   "erase_elementary_tree". */
		
		if ((prdct_no = prdct [state]) == -1 ||
		    for_parsact.prdct == NULL ||
		    (*for_parsact.prdct) (Xjl, prdct_no))
		{
		    rhs_stack [bot] = Xjl;
		    ret_val = set_rule (prolis [state], top) > 0;
		}
	    }
	}
	else
	{
	    XxY_Xforeach (Aij_hd, Xj, Xjk)
	    {
		if ((k = XxY_Y (Aij_hd, Xjk)) < l && Aij2attr [Xjk].first_lhs > 0)
		{
		    if ((prdct_no = prdct [state]) == -1 ||
		    for_parsact.prdct == NULL ||
		    (*for_parsact.prdct) (Xjk, prdct_no))
		    {
			rhs_stack [bot] = Xjk;
			ret_val |= parse_tree (state+1, bot+1, top, k, l);
		    }
		}
	    }
	}
    }

    return ret_val;
}




static void
reduce (k)
    int		k;
{
    /* Ai_set = {((A, i)} : ensemble des reduces du niveau courant (on vient de faire
       une transition sur ak). */
    int			i, A, Ai, Aik, j, B, Bi, Bik, Bj, Bjk, prod, top, start, x, state;
    int			prdct_no, constraint_no;
    SXBA		index_set;
    struct shift_reduce	*pntc, *pntc2;
    BOOLEAN		constraint_checked;

    Ai = PT [k].max_Ai+1;

    while ((Ai = sxba_1_rlscan (Ai_set, Ai)) > 0)
    {
	SXBA_0_bit (Ai_set, Ai);
	A = XxY_X (Ai_hd, Ai);
	i = XxY_Y (Ai_hd, Ai);
	pntc = &(PT [i].nt [A]);
	Aik = pntc->Aik;
	
	constraint_checked = TRUE;

	/* On est su^r que toutes les reductions vers Aik ont ete effectuees. */
	if ((constraint_no = constraints [A]) == -1 ||
	    for_parsact.constraint == NULL ||
	    (constraint_checked = (*for_parsact.constraint) (Aik, constraint_no)))
	{
	    for (x = pntc->top - 1; x >= 0; x--)
	    {
		/* B -> beta . A */
		state = pntc->state [x];

		if ((prdct_no = prdct [state]) == -1 ||
		    for_parsact.prdct == NULL ||
		    (*for_parsact.prdct) (Aik, prdct_no))
		{
		    index_set = pntc->index_set [x];
		    prod = prolis [state];
		    B = lhs [prod];

		    if (index_set == NULL)
		    {
			/* state : B -> . A */
			pntc2 = &(PT [i].nt [B]);
			Bi = pntc2->Ai;
			Bik = pntc2->Aik;

			if (Bik == 0)
			{
			    set_symbol (Bi, k+1, &Bik);
			    pntc2->Aik = Bik;
			}
#if EBUG
			else
			    if (XxY_Y (Aij_hd, Bik) != k+1)
				sxtrap (ME, "reduce");
#endif

			rhs_stack [0] = Bik;
			rhs_stack [1] = Aik;

			set_rule (prod, 1);
			SXBA_1_bit (Ai_set, Bi);
		    }
		    else
		    {
			/* state : B -> alpha X . Aik
			   index_set = {j1, ..., jp} et les j < i */
			j = i;

			while ((j = sxba_1_rlscan (index_set, j)) >= 0)
			{
			    pntc2 = &(PT [j].nt [B]);
			    Bj = pntc2->Ai;
			    Bjk = pntc2->Aik;

			    if (Bjk == 0)
			    {
				set_symbol (Bj, k+1, &Bjk);
				pntc2->Aik = Bjk;
			    }
#if EBUG
			    else
				if (XxY_Y (Aij_hd, Bjk) != k+1)
				    sxtrap (ME, "reduce");
#endif

			    rhs_stack [0] = Bjk;
			    start = prolon [prod];
			    top = state - start + 1;
			    rhs_stack [top] = Aik;

			    parse_tree (start, 1, top, j, i);
			    SXBA_1_bit (Ai_set, Bj);
			}
		    }
		}
	    }
	}
	else
	    if (!constraint_checked)
		XxY_erase (Aij_hd, Aik);

	pntc->Aik = 0;
    }
}


static void
scan_reduce (k, ptc)
    int k;
    struct scan_reduce	*ptc;
{
    int			state, A, Ai, i_A, j, prod, Aik, top, start, x;
    SXBA		index_set;
    struct shift_reduce	*pntc;

    for (x = ptc->top - 1; x >= 0; x--)
    {
	state = ptc->state [x];
	index_set = ptc->index_set [x];
	prod = prolis [state];
	A = lhs [prod];

	if (index_set == NULL)
	{
	    /* state : A -> . ak+1 */
	    pntc = &(PT [k].nt [A]);
	    Ai = pntc->Ai;
	    Aik = pntc->Aik;

	    if (Aik == 0)
	    {
		set_symbol (Ai, k+1, &Aik);
		pntc->Aik = Aik;
	    }
#if EBUG
	    else
		if (XxY_Y (Aij_hd, Aik) != k+1)
		    sxtrap (ME, "scan_reduce");
#endif

	    rhs_stack [0] = Aik;
	    rhs_stack [1] = -k - 1;

	    set_rule (prod, 1);
	    SXBA_1_bit (Ai_set, Ai);
	}
	else
	{
	    /* state : A -> alpha X . ak+1
	       index_set = {j1, ..., jp} et les j < k */
	    j = k;

	    while ((j = sxba_1_rlscan (index_set, j)) >= 0)
	    {
		pntc = &(PT [j].nt [A]);
		Ai = pntc->Ai;
		Aik = pntc->Aik;

		if (Aik == 0)
		{
		    set_symbol (Ai, k+1, &Aik);
		    pntc->Aik = Aik;
		}
#if EBUG
		else
		    if (XxY_Y (Aij_hd, Aik) != k+1)
			sxtrap (ME, "scan_reduce");
#endif

		rhs_stack [0] = Aik;
		start = prolon [prod];
		top = state - start + 1;
		rhs_stack [top] = -k - 1;

		parse_tree (start, 1, top, j, k);
		SXBA_1_bit (Ai_set, Ai);
	    }
	}
    }
}



static void
useless_symbol_elimination ()
{
    int		*symbol_stack;
    SXBA	Aij_set;
    int		symbol, prod, x;

    symbol_stack = SS_alloc (XxY_top (Aij_hd)+3);
    Aij_set = sxba_calloc (XxY_top (Aij_hd)+1);

    SXBA_1_bit (Aij_set, spf.G.start_symbol);
    SS_push (symbol_stack, spf.G.start_symbol);
    spf.G.N = 1;
    spf.G.P = spf.G.G = 0;

    do
    {
	symbol = SS_pop (symbol_stack);

	for (prod = Aij2attr [symbol].first_lhs; prod != 0; prod = spf.lhs [prod].next_lhs)
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

    symbol = XxY_top (Aij_hd)+1;

    while ((symbol = sxba_0_rlscan (Aij_set, symbol)) > 0)
    {
	/* symbol est inaccessible */
	for (prod = Aij2attr [symbol].first_lhs; prod != 0; prod = spf.lhs [prod].next_lhs)
	{
	    /* prod est inaccessible */
	    spf.lhs [prod].is_erased = TRUE;
	}

	Aij2attr [symbol].first_lhs = Aij2attr [symbol].first_rhs = 0;
    }

    SS_free (symbol_stack);
    sxfree (Aij_set);
}



static void
parse ()
{
    int			k;
    struct scan_reduce	*ptc;

    for (k = 0; k < n ; k++)
    {
	ptc = &(PT [k].t);

	if (ptc->top > 0)
	{
	    scan_reduce (k, ptc);
	    reduce (k);
	}
    }

    spf.G.N = XxY_top (Aij_hd);
    spf.G.P = spf.G.lhs_top;
    spf.G.T = n;
    spf.G.G = spf.G.rhs_top;

    spf.G.start_symbol = XxY_is_set (&Aij_hd, XxY_is_set (&Ai_hd, 1 /* axiome */, 0), n);

    if (spf.G.start_symbol != 0)
	useless_symbol_elimination ();

#if EBUG
    output_spf ();
#endif
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

static BOOLEAN
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
	return TRUE;
       
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
	return TRUE;
    case 15:
	psem->len = 1;
	return TRUE;

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
	    return FALSE;
	    /* erase_elementary_tree (elementary_tree); n'a pas (encore) ete entre'! */
	}
    else
    {
	erase_elementary_tree (psem->elementary_tree);
	psem->elementary_tree = elementary_tree;
	psem->len = new_len;
    }

    return TRUE;
}

static BOOLEAN
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
	is_parser = FALSE;
    else
	if (c == 'P' || c == 'p')
	    is_parser = TRUE;
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

    XxY_alloc (&shift_items, "shift_items", ntmax * (n+1), 1, 0, 0, shift_items_oflw,
#ifdef EBUG
	       stdout
#else
	       NULL
#endif
	       );

    bag_alloc (&shift_bag, (n + 1) * NBLONGS (XxY_size (shift_items) + 1));

    to_be_processed = sxba_calloc (XxY_size (shift_items) + 1);

#if EBUG
    RT = (struct recognize_item*) sxcalloc (n+1, sizeof (struct recognize_item));
#else
    RT = (struct recognize_item*) sxalloc (n+1, sizeof (struct recognize_item));
#endif

    global_state_sets = sxbm_calloc (6, statemax+1);
    global_index_sets = sxbm_calloc ((statemax+1)*2, n+1);

    init_state_set = global_state_sets [0];

    /* T1.state_stack = global_state_stacks;
    T1.state_stack [0] = 0; */
    T1.state_set = global_state_sets [1];
    T1.index_sets = global_index_sets;

    /* T2.state_stack = T1.state_stack + statemax + 1;
    T2.state_stack [0] = 0; */
    T2.state_set = global_state_sets [2];
    T2.index_sets = global_index_sets+statemax+1;

    shift_reduce_state_set = global_state_sets [3];
    /* shift_reduce_state_stack = T2.state_stack + statemax + 1;
    shift_reduce_state_stack [0] = 0; */

    scan_reduce_state_set = global_state_sets [4];
    /* scan_reduce_state_stack = shift_reduce_state_stack + statemax + 1;
    scan_reduce_state_stack [0] = 0; */

    shift_state_set = global_state_sets [5];

    NT_set = sxba_calloc (ntmax+1);

    order_shift_set = sxba_calloc (ntmax+1);

    if (is_parser)
    {
	PT = (struct parse_item*) sxalloc (n+1, sizeof (struct parse_item));

	index_set_pool = sxbm_calloc ((prodmax+1)*(n+1), n+1);

	XxY_alloc (&Ai_hd, "Ai_hd", ntmax * (n+1), 1, 0, 0, Ai_hd_oflw,
#ifdef EBUG
		   stdout
#else
		   NULL
#endif
		   );

	Ai_set = sxba_calloc (XxY_size (Ai_hd) + 1);

	XxY_alloc (&Aij_hd, "Aij_hd", ntmax * (n+1), 1, 1, 0, Aij_hd_oflw,
#ifdef EBUG
stdout
#else
NULL
#endif
		   );
	Aij2attr = (struct Aij2attr*) sxalloc (XxY_size (Aij_hd)+1, sizeof (struct Aij2attr));

	spf.G.lhs_size = ntmax * (n+1);
	spf.lhs = (struct lhs*) sxalloc (spf.G.lhs_size+1, sizeof (struct lhs));

	spf.G.rhs_size = spf.G.lhs_size * (rhs_lgth+1);
	spf.rhs = (struct rhs*) sxalloc (spf.G.rhs_size+1, sizeof (struct rhs));


	ARN_sem = (struct ARN_sem*) sxalloc (XxY_size (Aij_hd)+1, sizeof (struct ARN_sem));
    }


    grammar ();

    sprod2order = (int*) sxcalloc (prodmax+1, sizeof (int));

    grammar2 ();

    sprod_set = sxba_calloc (sprodmax+1);
    sprod2attr = (struct sprod2attr*) sxalloc (sprodmax+1, sizeof (struct sprod2attr));

    for_parsact.action = ARN_parsact;
    for_parsact.new_symbol = ARN_new_symbol;
    for_parsact.Aij_hd_oflw = ARN_sem_oflw;
    for_parsact.prdct = NULL;
    for_parsact.constraint = ARN_constraint;

    if ((l = recognize ()) <= n)
	printf ("Syntax error at %i\n", l);
    else
	if (is_parser)
	    parse ();

#if EBUG
    printf ("Nombre de RT [i].nt [A] = %i, Nombre de sur ensembles = %i\n", IT, IT_incl);
#endif

    grammar_free ();

    sxfree (sprod2order);
    sxfree (sprod_set);
    sxfree (sprod2attr);

    XxY_free (&shift_items);
    sxfree (to_be_processed);

    bag_free (&shift_bag);

    sxfree (RT);
    sxbm_free (global_state_sets);
    sxbm_free (global_index_sets);
    sxfree (NT_set);

    sxfree (order_shift_set);

    if (is_parser)
    {
	sxfree (PT);
	sxbm_free (index_set_pool);
	XxY_free (&Ai_hd);
	sxfree (Ai_set);
	XxY_free (&Aij_hd);
	sxfree (Aij2attr);

	sxfree (spf.rhs);
	sxfree (spf.lhs);

	sxfree (ARN_sem);
    }

    sxfree (TOK);

    return 0;
}

