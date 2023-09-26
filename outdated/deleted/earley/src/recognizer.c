#include "sxalloc.h"
#include "sxba.h"
#include "XxY.h"
#include "SS.h";

FILE	*sxtty, *sxstdout = {stdout}, *sxstderr = {stderr};

static struct LC0 {
  int NKshift, lgth;
};

struct for_parsact {
    int		(*action) (),
    		(*prdct) (),
    		(*new_symbol) (),
                (*symbols_oflw) ();
};


/* #if 0 */
/*
<RNA>		@1 = <RNA> <DoubleStrand> ;
<RNA>		@2 = <DoubleStrand> ;
<RNA>		@2 = <RNA> <Base> ;
<RNA>		@3 = <Base> ;

<DoubleStrand>	@4 = A <SingleStrand> U ;
<DoubleStrand>	@5 = A <InternalLoop> U ;
<DoubleStrand>	@6 = A <MultipleLoop> U ;
<DoubleStrand>	@4 = G <SingleStrand> C ;
<DoubleStrand>	@5 = G <InternalLoop> C ;
<DoubleStrand>	@6 = G <MultipleLoop> C ;
<DoubleStrand>	@4 = G <SingleStrand> U ;
<DoubleStrand>	@5 = G <InternalLoop> U ;
<DoubleStrand>	@6 = G <MultipleLoop> U ;
<DoubleStrand>	@4 = U <SingleStrand> A ;
<DoubleStrand>	@5 = U <InternalLoop> A ;
<DoubleStrand>	@6 = U <MultipleLoop> A ;
<DoubleStrand>	@4 = C <SingleStrand> G ;
<DoubleStrand>	@5 = C <InternalLoop> G ;
<DoubleStrand>	@6 = C <MultipleLoop> G ;
<DoubleStrand>	@4 = U <SingleStrand> G ;
<DoubleStrand>	@5 = U <InternalLoop> G ;
<DoubleStrand>	@6 = U <MultipleLoop> G ;

<InternalLoop>	@7 = <DoubleStrand> ;
<InternalLoop>	@8 = <InternalLoop> <Base> ;
<InternalLoop>	@9 = <Base> <InternalLoop> ;

<MultipleLoop>	@10 = <MultipleLoop> <DoubleStrand> ;
<MultipleLoop>	@11 = <DoubleStrand> ;
<MultipleLoop>	@12 = <MultipleLoop> <Base> ;
<MultipleLoop>	@13 = <Base> ;

<Base>		= A ;
<Base>		= U ;
<Base>		= C ;
<Base>		= G ;

<SingleStrand>	= <SingleStrand> <Base> ;
<SingleStrand>	= <Base> ;
*/




static int		ntmax = 6;
static int		tmax = -5;
static int		statemax = 116;
static int		prodmax = 35;
static int		initial_state = 1;
#define	rhs_lgth	3

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
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,
-1, -1, -1, -1,

-1, -1,
-1, -1, -1,
-1, -1, -1,

-1, -1, -1,
-1, -1,
-1, -1, -1,
-1, -1,

-1, -1,
-1, -1,
-1, -1,
-1, -1,

-1, -1, -1,
-1, -1
};


static int prolon [] = 	{
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
115
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
			     7
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

static char	*ntstring [] = {"",
				    "S"
				};

static int char2tok (c)
    char c;
{
    return (c != 'a') ? 0 : -1;
}

static int		ntmax = 1;
static int		tmax = -2;
static int		statemax = 8;
static int		prodmax = 2;
static int		initial_state = 1;
#define	rhs_lgth	2

#endif

struct AiBi {
  int	Ai, Bi;
};

struct chains2attr {
  int	prod;
};

static XxY_header	chains;
static struct chains2attr	*chains2attr;
static SXBA		chain_set, nb0;
static int		*nb;

static int		sprodmax;
static int		*sprod2order;
static SXBA		sprod_set;
static struct AiBi	*sprod2AiBi;

static char	ME [] = "S_SS";

static char	Usage [] = "\
Usage:\t%s \"source string\"\n";


static int		n;
static int		*TOK;
static char		*source;

static int		*slices;

static SXBA		state_set;
static int		*state_stack;

static SXBA		NT_set;
static int		*NT_stack;
static int		*nt2ntXi;

static XxY_header	ntXi;
static XxY_header	items;
static SXBA		T1, T2;

static SXBA		*ntXi2attr;

static int		*scan_reduces;
static SXBOOLEAN		is_scan_reduces;

static int		OR_ee, OR_nee, OR_nb, OR_unchain, OR_complete, OR_scan, T1_nb;

static SXBOOLEAN		is_parser;

/* cas du parser */
static SXBA		*reduce_sets;
static XxY_header	symbols;
static int		*root_stack, *prod_stack;
static struct symbols2attr {
  int	first_lhs, first_rhs;
} *symbols2attr;
static int		rhs_stack [rhs_lgth+1];

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


static  void
chains_oflw (old_size, new_size)
    int		old_size, new_size;
{
    chains2attr = (struct chains2attr*) sxrealloc (chains2attr, new_size + 1, sizeof (chains2attr));
}

static void
grammar ()
{
    /* A chaque production simple r: A -> B on affecte un entier n(r) t.q.
       r1: A -> B et r2: B -> C => n(r1) < n(r2). */
    int		prod, x, A, B, AxB;

    XxY_alloc (&chains, "chains", prodmax, 1, 1, 0, chains_oflw,
#ifdef EBUG
stdout
#else
NULL
#endif
	       );

    chains2attr = (struct chains2attr*) sxalloc (XxY_size (chains) + 1, sizeof (chains2attr));

    chain_set = sxba_calloc (ntmax + 1);
    nb0 = sxba_calloc (ntmax + 1);
    nb = (int*) sxcalloc (ntmax + 1, sizeof (int));

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
    sxfree (nb);

}

static  void
symbols_oflw (old_size, new_size)
    int		old_size, new_size;
{
    symbols2attr = (struct symbols2attr*) sxrealloc (symbols2attr, new_size + 1, sizeof (struct symbols2attr));

    if (for_parsact.symbols_oflw != NULL)
	(*for_parsact.symbols_oflw) (old_size, new_size);
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
	Ai = XxY_X (symbols, symb);
	j = XxY_Y (symbols, symb);
	A = XxY_X (ntXi, Ai);
	i = XxY_Y (ntXi, Ai);
	printf ("%s[%i..%i] ", ntstring [A], i, j);
    }
}

static void
output_prod (prod)
    int prod;
{
    int			x, symbol;
    struct lhs		*plhs;

    plhs = spf.lhs + prod;

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
    output_symb (spf.G.start_symbol);
    fputs ("\n\n", stdout);

    for (prod = 1; prod <= spf.G.lhs_top; prod++)
	output_prod (prod);
}
#endif	

static int
set_rule (init_prod, top)
    int top;
{
    int		lhs, x, X;
    struct lhs	*plhs;
    struct rhs	*prhs;

    if (for_parsact.action != NULL)
	(*for_parsact.action) (rhs_stack, top, init_prod);

    if (++spf.G.lhs_top >  spf.G.lhs_size)
    {
	spf.G.lhs_size *= 2;
	spf.lhs = (struct lhs*) sxrealloc (spf.lhs, spf.G.lhs_size + 1, sizeof (struct lhs));
    }

    if (spf.G.rhs_top + top >=  spf.G.rhs_size)
    {
	spf.G.rhs_size *= 2;
	spf.rhs = (struct rhs*) sxrealloc (spf.rhs, spf.G.rhs_size + 1, sizeof (struct rhs));
    }

    plhs = spf.lhs + spf.G.lhs_top;
    lhs = rhs_stack [0];
    plhs->prolon = ++spf.G.rhs_top;
    plhs->reduc = lhs;
    plhs->next_lhs = symbols2attr [lhs].first_lhs;
    symbols2attr [lhs].first_lhs = spf.G.lhs_top;
    plhs->init_prod = init_prod;
    plhs->is_erased = SXFALSE;

    for (x = 1; x <= top; x++)
    {
	X = rhs_stack [x];
	prhs = spf.rhs + spf.G.rhs_top;
	prhs->lispro = X;
	prhs->prolis = spf.G.lhs_top;

	if (X > 0)
	{
	    prhs->next_rhs = symbols2attr [X].first_rhs;
	    symbols2attr [X].first_rhs = spf.G.rhs_top;
	}

	spf.G.rhs_top++;
    }

    prhs = spf.rhs + spf.G.rhs_top;
    prhs->lispro = 0;
    prhs->prolis = spf.G.lhs_top;

#if EBUG
    printf ("%i\t: ", spf.G.lhs_top);
    output_symb (lhs);
    printf ("\t = ");

    for (x = 1; x <= top; x++)
    {
	output_symb (rhs_stack [x]);
    }
    
    printf (";\n");
#endif	

    return spf.G.lhs_top;
}



static int
set_symbol (Xj, k)
    int Xj, k;

{
    int	Xjk;

    if (!XxY_set (&symbols, Xj, k, &Xjk))
    {
	symbols2attr [Xjk].first_lhs = symbols2attr [Xjk].first_rhs = 0;

	if (for_parsact.new_symbol != NULL)
	    (*for_parsact.new_symbol) (Xjk);
    }

#if EBUG
    /* Les reductions doivent se faire dans l'ordre (creation des LHS avant toute
       utilisation en RHS). */
    else
	if (symbols2attr [Xjk].first_rhs != 0)
	    sxtrap (ME, "set_symbol");

#endif

    return Xjk;
}


static void
parse_tree (top, state, j, l)
    int top, state, j, l;
{
    /* Cette procedure recursive genere toutes les regles (sous-arbres) dont la racine
       est lhs.
       state =  A -> X1  Xh . X  Xp
       lhs = A[i..l]
       rhs = X1[i..i1] ... Xh[ih-1..j]
    */
    int		X, Xj, Xjl, Xjk, k;

    X = lispro [state];

    if (j == l && X == 0)
    {
	set_rule (prolis [state], top);
	return;
    }

    if (X == 0) return;

    if (X < 0)
    {
	/* X \in T */
	rhs_stack [top+1] = -(j+1);
	parse_tree (top+1, state+1, j+1, l);
    }
    else
    {
	/* X \in N */
	Xj = XxY_is_set (&ntXi, X, j);

#if EBUG
	if (Xj == 0)
	    sxtrap (ME, "parse_tree");
#endif

	if (lispro [state+1] == 0)
	{
	    Xjl = XxY_is_set (&symbols, Xj, l, &Xjl);

	    if (Xjl > 0)
	    {
		
#if EBUG
		if (symbols2attr [Xjl].first_lhs == 0)
		    sxtrap (ME, "parse_tree");
#endif

		rhs_stack [top+1] = Xjl;
		parse_tree (top+1, state+1, l, l);
	    }
	}
	else
	{
	    XxY_Xforeach (symbols, Xj, Xjk)
	    {
		if ((k = XxY_Y (symbols, Xjk)) < l)
		{
		    rhs_stack [top+1] = Xjk;
		    parse_tree (top+1, state+1, k, l);
		}
	    }
	}
    }
}




static  void
ntXi_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ntXi2attr = sxbm_resize (ntXi2attr, old_size + 1, new_size + 1, XxY_size (items) + 1);

    if (is_parser)
    {
	reduce_sets = sxbm_resize (reduce_sets, old_size + 1, new_size + 1, XxY_size (items) + 1);
    }
}

static  void
items_oflw (old_size, new_size)
    int		old_size, new_size;
{
    T1 = sxba_resize (T1, new_size + 1);
    T2 = sxba_resize (T2, new_size + 1);
    ntXi2attr = sxbm_resize (ntXi2attr, XxY_size (ntXi) + 1, XxY_size (ntXi) + 1, new_size + 1);

    if (is_parser)
    {
	reduce_sets = sxbm_resize (reduce_sets, XxY_size (ntXi) + 1, XxY_size (ntXi) + 1, new_size + 1);
    }
}



static SXBA
OR (lhs_bits_array, rhs_bits_array, slices_number)
    SXBA	lhs_bits_array, rhs_bits_array;
/*
 * "OR" puts into its first argument the result of the bitwise
 * "OR" of the prefix of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register SXBA_ELT	rhs_val;

    OR_nb++;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0) {
      if (rhs_val = *rhs_bits_ptr--)
      {
	  OR_nee++;
	  *lhs_bits_ptr-- |= rhs_val;
      }
      else
      {
	  OR_ee++;
	  lhs_bits_ptr--;
      }
    }

    return lhs_bits_array;
}

static void
chain (prod, Ai, Bi)
    int Ai, Bi;
{
    /* On a [A -> B. , i] donc A[i] \in B[i] */
    int		n;

    n = sprod2order [prod];
    SXBA_1_bit (sprod_set, n);
    sprod2AiBi [n].Ai = Ai;
    sprod2AiBi [n].Bi = Bi;
}


static void
unchain (i)
    int i;
{
    /* On suppose que la grammaire est non circulaire */
    int		n, Ai, Bi, slice = slices [i];

    while ((n = sxba_scan_reset (sprod_set, 0)) > 0)
    {
	Ai = sprod2AiBi [n].Ai;
	Bi = sprod2AiBi [n].Bi;
	OR (ntXi2attr [Bi], ntXi2attr [Ai], slice);
	OR_unchain++;

	if (is_parser)
	{
	    OR (reduce_sets [Bi], reduce_sets [Ai], slice);
	}
    }
}


static void
reduce (item_set, k)
    SXBA	item_set;
    int		k;
{
    /* item_set = {(A-> alpha. , i)} : ensemble des reduces du niveau courant (on vient de faire
       une transition sur ak). */
    int item, state, i, A, Ai, Aik, prod;

    /* On commence par calculer l'ensemble des Aik. */
    item = XxY_top (items) + 1;

    while ((item = sxba_1_rlscan (item_set, item)) > 0)
    {
	SXBA_0_bit (item_set, item);
	state = XxY_X (items, item);
	i = XxY_Y (items, item);
	A = lhs [prod = prolis [state]];
	Ai = XxY_is_set (&ntXi, A, i);
	    
#if EBUG
	if (Ai == 0)
	    sxtrap (ME, "reduce");
#endif

	Aik = set_symbol (Ai, k);
	SS_push (root_stack, Aik);
	SS_push (prod_stack, prod);
    }

    /* On lance parse_tree */
    while (!SS_is_empty (root_stack))
    {
	rhs_stack [0] = Aik = SS_pop (root_stack);
	prod = SS_pop (prod_stack);
	parse_tree (0, prolon [prod], XxY_Y (ntXi, XxY_X (symbols, Aik)) /* i */, k);
    }
}


static void
parser_reduce (item_set, k)
    SXBA	item_set;
    int		k;
{
    /* item_set = {(A-> alpha. , i)} : ensemble des reduces du niveau courant (on vient de faire
       une transition sur ak). */
    int item, state, i, A, Ai, Aik, prod, shift_item, reduce_item, shift_state, reduce_state, prdct_no;
    SXBA	shift_set, reduce_set;

    /* On commence par calculer l'ensemble des Aik. */
    
    item = XxY_top (items) + 1;
    while ((item = sxba_1_rlscan (item_set, item)) > 0)
    {
	SXBA_0_bit (item_set, item);
	state = XxY_X (items, item);
	i = XxY_Y (items, item);
	A = lhs [prod = prolis [state]];
	Ai = XxY_is_set (&ntXi, A, i);
	    
#if EBUG
	if (Ai == 0)
	    sxtrap (ME, "reduce");
#endif

	Aik = set_symbol (Ai, k);

	rhs_stack [0] = Aik;
	parse_tree (0, prolon [prod], i, k);

	shift_set = ntXi2attr [Ai], reduce_set = reduce_sets [Ai];

	reduce_item = shift_item = MUL (slices [i]);

	while ((shift_item = sxba_1_rlscan (shift_set, shift_item)) > 0)
	{
	    shift_state = XxY_X (items, shift_item);

	    if ((prdct_no = prdct [state-1]) == -1 ||
		for_parsact.prdct != NULL && (*for_parsact.prdct) (prdct_no))
	    {
		SXBA_1_bit (T2, shift_item);
	    }
			    
	}

	while ((reduce_item = sxba_1_rlscan (reduce_set, reduce_item)) > 0)
	{
	    reduce_state = XxY_X (items, reduce_item);

	    if ((prdct_no = prdct [state-1]) == -1 ||
		for_parsact.prdct != NULL && (*for_parsact.prdct) (prdct_no))
	    {
#if EBUG
		if (reduce_item >= item)
		    sxtrap (ME, "parser_reduce");
#endif

		SXBA_1_bit (item_set, reduce_item);
	    }
	}
    }
}



static void
scan_reduce (i)
    int i;
{
    int	Ak, k;

    do
    {
	Ak = SS_pop (scan_reduces);
	k = XxY_Y (ntXi, Ak);
	OR (T2, ntXi2attr [Ak], slices [k]);
	OR_scan++;

	if (is_parser)
	    OR (ntXi2attr [0], reduce_sets [Ak], slices [k]);
    } while (!SS_is_empty (scan_reduces));

    if (is_parser)
	reduce (ntXi2attr [0], i+1);
}



static void
parser_scan_reduce (i)
    int i;
{
    int	item, state, new_item;

    do
    {
	item = SS_pop (scan_reduces);

	if (item < 0)
	{
	    state = -item; /* A -> ai+1 . */
	    XxY_set (&items, state, i, &new_item);
	}
	else
	    XxY_set (&items, XxY_X (items, item) + 1, XxY_Y (items, item), &new_item);

	SXBA_1_bit (ntXi2attr [0], new_item);
    } while (!SS_is_empty (scan_reduces));

    parser_reduce (ntXi2attr [0], i+1);
}



#if 0
static SXBOOLEAN
look_ahead (state, i)
    int state, i;
{
    int t = -TOK [i];

    return SXBA_bit_is_set (FIRST [state], t);
}
#endif

#define look_ahead(i,j)		SXTRUE


static SXBOOLEAN
complete (i)
{
    int item, new_item, state, k, X, Xi, A, Ak, Ai, Y, Yi, x, lim, top, xs, prod;
    SXBOOLEAN is_tok = SXFALSE;

    item = XxY_top (items) + 1;

    while ((item = sxba_1_rlscan (T1, item)) > 0)
    {
	/* item = (state, k) */
	/* state = A -> alpha . X beta et
	   X ne derive pas que ds le vide */
	T1_nb++;
	SXBA_0_bit (T1, item);
	state = XxY_X (items, item);

	if (look_ahead (state, i+1))
	{
	    k = XxY_Y (items, item);
	    X = lispro [state];

	    if (X > 0)
	    {
		/* NT */
		if (SXBA_bit_is_reset_set (NT_set, X))
		{
		    SS_push (NT_stack, X);
		    XxY_set (&ntXi, X, i, &Xi);
		    nt2ntXi [X] = Xi;
		}
		else
		    Xi = nt2ntXi [X];

		if (lispro [state+1] == 0)
		{
		    /* beta == epsilon */
		    A = lhs [prolis [state]];
		    Ak = XxY_is_set (&ntXi, A, k);
	
#if EBUG
		    if (k == i || Ak == 0)
			sxtrap (ME, "complete");
#endif

		    OR (ntXi2attr [Xi], ntXi2attr [Ak], slices [k]);
		    OR_complete++;

		    if (is_parser)
		    {
			XxY_set (&items, state + 1, k, &new_item);
			SXBA_1_bit (reduce_sets [Xi], new_item);
			OR (reduce_sets [Xi], reduce_sets [Ak], slices [k]);
		    }
		}
		else
		{
		    /* beta != epsilon */
		    XxY_set (&items, state + 1, k, &new_item);
		    SXBA_1_bit (ntXi2attr [Xi], new_item);
		}
	    }
	    else
	    {
		if (X == TOK [i+1]) /* toujours vrai! quand le look-ahead sera implante' */
		{
		    if (look_ahead (state+1, i+2))
		    {
			if (lispro [state+1] == 0)
			{
			    /* beta == epsilon */
			    A = lhs [prod = prolis [state]];
			    Ak = XxY_is_set (&ntXi, A, k);

#if EBUG
			    if (Ak == 0 || k == i)
				sxtrap (ME, "complete");
#endif

			    SS_push (scan_reduces, Ak);
			    is_scan_reduces = SXTRUE;

			    if (is_parser)
			    {
				rhs_stack [0] = set_symbol (Ak, i+1);
				parse_tree (0, prolon [prod], k, i+1);
			    }
			}
			else
			{
			    /* beta != epsilon */
			    XxY_set (&items, state + 1, k, &new_item);
			    SXBA_1_bit (T2, new_item);
			}

			is_tok = SXTRUE;
		    }
		}
	    }
	}
    }

    if (!SS_is_empty (NT_stack))
    {
	for (top = SS_top (NT_stack), xs = SS_bot (NT_stack); xs < top; xs++)
	{
	    X = SS_get (NT_stack, xs);

	    for (x = LC0 [X].NKshift, lim = x + LC0 [X].lgth; x < lim; x++)
	    {
		/* state = A -> gamma Y . delta */
		state = NKshift [x];
		Y = lispro [state - 1];

		if (Y > 0 || Y == TOK [i+1])
		{
		    if (SXBA_bit_is_reset_set (state_set, state))
		    {
			SS_push (state_stack, state);
		    }
		}
	    }
	}

	do
	{
	    state = SS_pop (state_stack);
	    SXBA_0_bit (state_set, state);

	    if (look_ahead (state-1, i+1))
	    {
		Y = lispro [state - 1];

		if (Y > 0)
		{
		    /* NT */
		    if (SXBA_bit_is_reset_set (NT_set, Y))
		    {
			SS_push (NT_stack, Y);
			XxY_set (&ntXi, Y, i, &Yi);
			nt2ntXi [Y] = Yi;
		    }
		    else
			Yi = nt2ntXi [Y];

		    if (lispro [state] == 0)
		    {
			/* delta == epsilon */
			A = lhs [prod = prolis [state]];

			if (SXBA_bit_is_reset_set (NT_set, A))
			{
			    SS_push (NT_stack, A);
			    XxY_set (&ntXi, A, i, &Ai);
			    nt2ntXi [A] = Ai;
			}
			else
			    Ai = nt2ntXi [A];

			chain (prod, Ai, Yi);

			if (is_parser)
			{
			    XxY_set (&items, state, i, &new_item);
			    SXBA_1_bit (reduce_sets [Yi], new_item);
			}
		    }
		    else
		    {
			/* delta != epsilon */
			XxY_set (&items, state, i, &new_item);
			SXBA_1_bit (ntXi2attr [Yi], new_item);
		    }
		}
		else
		    if (Y == TOK [i+1]) /* toujours vrai! quand le look-ahead sera implante' */
		    {
			if (look_ahead (state, i+2))
			{
			    if (lispro [state] == 0)
			    {
				/* delta == epsilon */
				A = lhs [prod = prolis [state]];

				if (SXBA_bit_is_reset_set (NT_set, A))
				{
				    SS_push (NT_stack, A);
				    XxY_set (&ntXi, A, i, &Ai);
				    nt2ntXi [A] = Ai;
				}
				else
				    Ai = nt2ntXi [A];

				SS_push (scan_reduces, Ai);
				is_scan_reduces = SXTRUE;

				if (is_parser)
				{
				    rhs_stack [0] = set_symbol (Ai, i+1);
				    parse_tree (0, prolon [prod], i, i+1);
				}
			    }
			    else
			    {
				/* delta != epsilon */
				XxY_set (&items, state, i, &new_item);
				SXBA_1_bit (T2, new_item);
			    }

			    is_tok = SXTRUE;
			}
		    }
	    }
	} while (!SS_is_empty (state_stack));

	do
	{
	    X = SS_pop (NT_stack);
	    SXBA_0_bit (NT_set, X);
	} while (!SS_is_empty (NT_stack));
    }

    slices [i] = NBLONGS (XxY_top (items) + 1);

    return is_tok;
}



static SXBOOLEAN
parser_complete (i)
{
    int item, new_item, state, k, X, Xi, A, Ak, Ai, Y, Yi, x, lim, top, xs, prod, prdct_no;
    SXBOOLEAN is_tok = SXFALSE;

    item = XxY_top (items) + 1;

    while ((item = sxba_1_rlscan (T1, item)) > 0)
    {
	/* item = (state, k) */
	/* state = A -> alpha . X beta et
	   X ne derive pas que ds le vide */
	T1_nb++;
	SXBA_0_bit (T1, item);
	state = XxY_X (items, item);

	if (look_ahead (state, i+1))
	{
	    k = XxY_Y (items, item);
	    X = lispro [state];

	    if (X > 0)
	    {
		/* NT */
		if (SXBA_bit_is_reset_set (NT_set, X))
		{
		    SS_push (NT_stack, X);
		    XxY_set (&ntXi, X, i, &Xi);
		    nt2ntXi [X] = Xi;
		}
		else
		    Xi = nt2ntXi [X];

		XxY_set (&items, state + 1, k, &new_item);

		if (lispro [state+1] == 0)
		{
		    /* beta == epsilon */
		    SXBA_1_bit (reduce_sets [Xi], new_item);
		}
		else
		{
		    /* beta != epsilon */
		    SXBA_1_bit (ntXi2attr [Xi], new_item);
		}
	    }
	    else
	    {
		if (X == TOK [i+1]) /* toujours vrai! quand le look-ahead sera implante' */
		{
		    if (look_ahead (state+1, i+2))
		    {
			if ((prdct_no = prdct [state]) == -1 ||
			    for_parsact.prdct != NULL && (*for_parsact.prdct) (prdct_no))
			{
			    if (lispro [state+1] == 0)
			    {
				/* beta == epsilon */
				SS_push (scan_reduces, item);
				is_scan_reduces = SXTRUE;
			    }
			    else
			    {
				/* beta != epsilon */
				XxY_set (&items, state + 1, k, &new_item);
				SXBA_1_bit (T2, new_item);
			    }

			    is_tok = SXTRUE;
			}
		    }
		}
	    }
	}
    }

    if (!SS_is_empty (NT_stack))
    {
	for (top = SS_top (NT_stack), xs = SS_bot (NT_stack); xs < top; xs++)
	{
	    X = SS_get (NT_stack, xs);

	    for (x = LC0 [X].NKshift, lim = x + LC0 [X].lgth; x < lim; x++)
	    {
		/* state = A -> gamma Y . delta */
		state = NKshift [x];
		Y = lispro [state - 1];

		if (Y > 0 || Y == TOK [i+1])
		{
		    if (SXBA_bit_is_reset_set (state_set, state))
		    {
			SS_push (state_stack, state);
		    }
		}
	    }
	}

	do
	{
	    state = SS_pop (state_stack);
	    SXBA_0_bit (state_set, state);

	    if (look_ahead (state-1, i+1))
	    {
		Y = lispro [state - 1];

		if (Y > 0)
		{
		    /* NT */
		    if (SXBA_bit_is_reset_set (NT_set, Y))
		    {
			SS_push (NT_stack, Y);
			XxY_set (&ntXi, Y, i, &Yi);
			nt2ntXi [Y] = Yi;
		    }
		    else
			Yi = nt2ntXi [Y];

		    XxY_set (&items, state, i, &new_item);

		    if (lispro [state] == 0)
		    {
			/* delta == epsilon */
			SXBA_1_bit (reduce_sets [Yi], new_item);
		    }
		    else
		    {
			/* delta != epsilon */
			SXBA_1_bit (ntXi2attr [Yi], new_item);
		    }
		}
		else
		    if (Y == TOK [i+1]) /* toujours vrai! quand le look-ahead sera implante' */
		    {
			if (look_ahead (state, i+2))
			{
			    if ((prdct_no = prdct [state]) == -1 ||
				for_parsact.prdct != NULL && (*for_parsact.prdct) (prdct_no))
			    {
				if (lispro [state] == 0)
				{
				    /* delta == epsilon */
				    SS_push (scan_reduces, -state);
				    is_scan_reduces = SXTRUE;
				}
				else
				{
				    /* delta != epsilon */
				    XxY_set (&items, state, i, &new_item);
				    SXBA_1_bit (T2, new_item);
				}

				is_tok = SXTRUE;
			    }
			}
		    }
	    }
	} while (!SS_is_empty (state_stack));

	do
	{
	    X = SS_pop (NT_stack);
	    SXBA_0_bit (NT_set, X);
	} while (!SS_is_empty (NT_stack));
    }

    slices [i] = NBLONGS (XxY_top (items) + 1);

    return is_tok;
}

static void
useless_symbol_elimination ()
{
    int		*symbol_stack;
    SXBA	symbol_set;
    int		symbol, prod, x;

    symbol_stack = SS_alloc (XxY_top (symbols) + 3);
    symbol_set = sxba_calloc (XxY_top (symbols) + 1);

    SXBA_1_bit (symbol_set, spf.G.start_symbol);
    SS_push (symbol_stack, spf.G.start_symbol);

    do
    {
	symbol = SS_pop (symbol_stack);

	for (prod = symbols2attr [symbol].first_lhs; prod != 0; prod = spf.lhs [prod].next_lhs)
	{
	    for (x = spf.lhs [prod].prolon; (symbol = spf.rhs [x].lispro) != 0; x++)
	    {
		if (symbol > 0 && SXBA_bit_is_reset_set (symbol_set, symbol))
		    SS_push (symbol_stack, symbol);
	    }
	}
    } while (!SS_is_empty (symbol_stack));

    symbol = XxY_top (symbols) + 1;

    while ((symbol = sxba_0_rlscan (symbol_set, symbol)) > 0)
    {
	/* symbol est inaccessible */
	for (prod = symbols2attr [symbol].first_lhs; prod != 0; prod = spf.lhs [prod].next_lhs)
	{
	    /* prod est inaccessible */
	    spf.lhs [prod].is_erased = SXTRUE;
	    spf.G.P--;
	    spf.G.G--;
	    
	    for (x = spf.lhs [prod].prolon; spf.rhs [x].lispro != 0; x++)
		spf.G.G--;
	}

	XxY_erase (symbols, symbol);
	spf.G.N--;
    }

    SS_free (symbol_stack);
    sxfree (symbol_set);
}

static int
recognize ()
{
    int		item, i;
    SXBA	T;
    SXBOOLEAN	is_in_LG;

    /* initial_state ne vaut pas toujours 1 (cas ou L(G)={epsilon}). */
    XxY_set (&items, initial_state, 0, &item);
    SXBA_1_bit (T1, item);	/* T1 = {(S'->.S$, 0)} */

    for (i = 0; i < n; i++)
    {
	if (!parser_complete (i)) break;

	/* if (XxY_top (chains) > 0)
	    unchain (i); */
       
	if (is_scan_reduces)
	    parser_scan_reduce (i);

	T = T1, T1 = T2, T2 = T;
    }

    is_in_LG = (item = XxY_is_set (&items, 2 /* final_state */, 0)) > 0 /* (S' -> S . $, 0) */ &&
	SXBA_bit_is_set (T1, item);

    if (is_parser)
    {
	spf.G.N = XxY_top (symbols);
	spf.G.P = spf.G.lhs_top;
	spf.G.T = i;
	spf.G.G = spf.G.rhs_top;

	if (is_in_LG)
	{
	    spf.G.start_symbol = XxY_is_set (&symbols, XxY_is_set (&ntXi, 1 /* axiome */, 0), n);

	    if (spf.G.start_symbol != 0)
		useless_symbol_elimination ();
	}

#if EBUG
	output_spf ();
#endif
    }

    return is_in_LG ? n+1 : i+1;
}


main (argc, argv)
    int		argc;
    char	*argv [];
{
    int l, t, *tok;
    char	*s, c;

    if (argc == 1) {
	printf (Usage, ME);
	return 1;
    }

    sxopentty ();

    s = source = argv [1];
    n = strlen (source);

    if (n == 0) {
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

    is_parser = SXTRUE;

    slices = (int*) sxalloc (n + 1, sizeof (int));

    NT_set = sxba_calloc (ntmax + 1);
    NT_stack = SS_alloc (ntmax + 3);
    nt2ntXi = (int*) sxalloc (ntmax + 1, sizeof (int));

    state_set = sxba_calloc (statemax + 1);
    state_stack = SS_alloc (statemax + 3);

    XxY_alloc (&ntXi, "ntXi", ntmax * (n + 1), 1, 0, 0, ntXi_oflw,
#ifdef EBUG
stdout
#else
NULL
#endif
	       );

    XxY_alloc (&items, "items", (statemax - 2*prodmax + 1) * (n + 1), 1, 0, 0, items_oflw,
#ifdef EBUG
stdout
#else
NULL
#endif
	       );

    T1 = sxba_calloc (XxY_size (items) + 1);
    T2 = sxba_calloc (XxY_size (items) + 1);

    ntXi2attr = sxbm_calloc (XxY_size (ntXi) + 1, XxY_size (items) + 1);

    if (is_parser)
    {
	reduce_sets = sxbm_calloc (XxY_size (ntXi) + 1, XxY_size (items) + 1);
	
	XxY_alloc (&symbols, "symbols", ntmax * (n + 1), 1, 1, 0, symbols_oflw,
#ifdef EBUG
stdout
#else
NULL
#endif
		   );
	symbols2attr = (struct symbols2attr*) sxalloc (XxY_size (symbols) + 1, sizeof (struct symbols2attr));

	root_stack = SS_alloc (XxY_size (items) + 3);
	prod_stack = SS_alloc (XxY_size (items) + 3);

	spf.G.lhs_size = ntmax * (n + 1);
	spf.lhs = (struct lhs*) sxalloc (spf.G.lhs_size + 1, sizeof (struct lhs));

	spf.G.rhs_size = spf.G.lhs_size * (rhs_lgth + 1);
	spf.rhs = (struct rhs*) sxalloc (spf.G.rhs_size + 1, sizeof (struct rhs));
    }

    scan_reduces = SS_alloc (30);

    sprod2order = (int*) sxcalloc (prodmax + 1, sizeof (int));

    grammar ();

    sprod_set = sxba_calloc (sprodmax + 1);
    sprod2AiBi = (struct AiBi*) sxalloc (sprodmax + 1, sizeof (struct AiBi));

    if ((l = recognize ()) <= n)
	printf ("Syntax error at %i\n", l);

    printf ("\nOR_nee = %i, OR_ee = %i, OR_nb = %i,\n\
OR_unchain = %i, OR_complete = %i, OR_scan = %i, T1_nb = %i\n",
	    OR_nee, OR_ee, OR_nb, OR_unchain,
	    OR_complete, OR_scan, T1_nb);

    sxfree (sprod2order);
    sxfree (sprod_set);
    sxfree (sprod2AiBi);

    sxfree (NT_set);
    SS_free (NT_stack);
    sxfree (nt2ntXi);

    sxfree (state_set);
    SS_free (state_stack);

    XxY_free (&ntXi);
    XxY_free (&items);
    sxbm_free (ntXi2attr);

    if (is_parser)
    {
	sxbm_free (reduce_sets);
	XxY_free (&symbols);
	sxfree (symbols2attr);
	SS_free (root_stack);
	SS_free (prod_stack);

	sxfree (spf.rhs);
	sxfree (spf.lhs);
    }

    sxfree (T1);
    sxfree (T2);    
    SS_free (scan_reduces);

    sxfree (slices);
    sxfree (TOK);

    return 0;
}

