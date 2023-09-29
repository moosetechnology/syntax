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
#include "XxY.h"
#include "SS.h";

FILE	*sxtty, *sxstdout = {stdout}, *sxstderr = {stderr};

static struct LC0 {
  int NKshift, lgth;
};

struct for_parsact {
    void	(*action) (),
    		(*new_symbol) (),
                (*symbols_oflw) ();

    SXBOOLEAN	(*prdct) ();
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

struct state_Bi {
  int	state, Bi;
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
static struct state_Bi	*sprod2state_Bi;

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
static SXBOOLEAN		is_scan_reduces, is_single_prods;

static int		T1_nb;
static int		PT_true, PT_false;

static SXBOOLEAN		is_parser;

/* cas du parser */
static SXBA		*reduce_sets;
static XxY_header	symbols;
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

erase_elementary_tree (elementary_tree)
    int elementary_tree;
{
    int		lhs_symb;
    int		*p, *q;
    struct lhs	*pet = spf.lhs + elementary_tree;

    if (!pet->is_erased)
    {
	pet->is_erased = SXTRUE;

	for (q = p = &(symbols2attr [pet->reduc].first_lhs); *p != 0; p = &(spf.lhs [*p].next_lhs))
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
	    for (q = p = &(symbols2attr [pet->reduc].first_rhs); *p != 0; p = &(spf.rhs [*p].next_rhs))
	    {
		erase_elementary_tree (spf.rhs [*p].prolis);
	    }

	    *q = 0;
	}
    }
}




static int
set_rule (init_prod, top)
    int top;
{
    int		lhs, x, X;
    struct lhs	*plhs;
    struct rhs	*prhs;

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

    if (for_parsact.action != NULL)
	(*for_parsact.action) (rhs_stack, top, spf.G.lhs_top, init_prod);

#if EBUG
    output_prod (spf.G.lhs_top);
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


static SXBOOLEAN
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
    SXBOOLEAN	ret_val;

    X = lispro [state];

    if (j == l && X == 0)
    {
	set_rule (prolis [state], top);
	ret_val = SXTRUE;
    }
    else
    {
	if (X < 0)
	{
	    /* X \in T */
	    rhs_stack [top+1] = -(j+1);
	    ret_val = parse_tree (top+1, state+1, j+1, l);
	}
	else
	{
	    ret_val = SXFALSE;

	    if (X > 0)
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

		    if (Xjl > 0 && symbols2attr [Xjl].first_lhs > 0)
		    {
			/* Si ==0 alors tous les arbres le definissant ont ete supprimes par
			   "erase_elementary_tree". */
			rhs_stack [top+1] = Xjl;
			ret_val = parse_tree (top+1, state+1, l, l);
		    }
		}
		else
		{
		    XxY_Xforeach (symbols, Xj, Xjk)
		    {
			if ((k = XxY_Y (symbols, Xjk)) < l && symbols2attr [Xjk].first_lhs > 0)
			{
			    rhs_stack [top+1] = Xjk;
			    ret_val |= parse_tree (top+1, state+1, k, l);
			}
		    }
		}
	    }
	}

	if (ret_val)
	    PT_true++;
	else
	    PT_false++;

	return ret_val;
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

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0) {
      if (rhs_val = *rhs_bits_ptr--)
      {
	  *lhs_bits_ptr-- |= rhs_val;
      }
      else
      {
	  lhs_bits_ptr--;
      }
    }

    return lhs_bits_array;
}

static void
chain (state, Bi)
    int state, Bi;
{
    /* On a state = A -> B. */
    int		n;

    is_single_prods = SXTRUE;
    n = sprod2order [prolis [state]];

#if EBUG
    if (n == 0)
	sxtrap (ME, "chain");
#endif

    SXBA_1_bit (sprod_set, n);
    sprod2state_Bi [n].state = state;
    sprod2state_Bi [n].Bi = Bi;
}


static void
unchain (i)
    int i;
{
    /* On suppose que la grammaire est non circulaire */
    int		n, state, Bi, new_item;

    is_single_prods = SXFALSE;
    
    n = 0;

    while ((n = sxba_scan_reset (sprod_set, n)) > 0)
    {
	state = sprod2state_Bi [n].state;
	Bi = sprod2state_Bi [n].Bi;
	XxY_set (&items, state, i, &new_item);
	SXBA_1_bit (reduce_sets [Bi], new_item);
    }
}



static void
reduce (item_set, k)
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

	    if ((prdct_no = prdct [shift_state-1]) == -1 ||
		for_parsact.prdct != NULL && (*for_parsact.prdct) (Aik, prdct_no))
	    {
		SXBA_1_bit (T2, shift_item);
	    }
	}

	while ((reduce_item = sxba_1_rlscan (reduce_set, reduce_item)) > 0)
	{
	    reduce_state = XxY_X (items, reduce_item);

	    if ((prdct_no = prdct [reduce_state-1]) == -1 ||
		for_parsact.prdct != NULL && (*for_parsact.prdct) (Aik, prdct_no))
	    {
#if EBUG
		if (reduce_item >= item)
		    sxtrap (ME, "reduce");
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

    reduce (ntXi2attr [0], i+1);
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
			    for_parsact.prdct != NULL && (*for_parsact.prdct) (-i-1, prdct_no))
			{
			    if (lispro [state+1] == 0)
			    {
				/* beta == epsilon */
				/* les items scan-reduce doivent etre generes en dernier */
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

		    if (lispro [state] == 0)
		    {
			/* delta == epsilon */
			/* production simple A -> Y
			   les items de la partie gauche doivent etre generes avant ceux de Y[i]. */
			chain (state, Yi);
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
			    if ((prdct_no = prdct [state-1]) == -1 ||
				for_parsact.prdct != NULL && (*for_parsact.prdct) (-i-1, prdct_no))
			    {
				if (lispro [state] == 0)
				{
				    /* delta == epsilon */
				    /* les items scan-reduce doivent etre generes en dernier */
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
    spf.G.N = 1;
    spf.G.P = spf.G.G = 0;

    do
    {
	symbol = SS_pop (symbol_stack);

	for (prod = symbols2attr [symbol].first_lhs; prod != 0; prod = spf.lhs [prod].next_lhs)
	{
	    if (!spf.lhs [prod].is_erased)
	    {
		spf.G.P++;
		spf.G.G++;

		for (x = spf.lhs [prod].prolon; (symbol = spf.rhs [x].lispro) != 0; x++)
		{
		    spf.G.G++;

		    if (symbol > 0 && SXBA_bit_is_reset_set (symbol_set, symbol))
		    {
			spf.G.N++;
			SS_push (symbol_stack, symbol);
		    }
		}
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
	}

	symbols2attr [symbol].first_lhs = symbols2attr [symbol].first_rhs = 0;
    }

    SS_free (symbol_stack);
    sxfree (symbol_set);
}

static int
parse ()
{
    int		item, i;
    SXBA	T;
    SXBOOLEAN	is_in_LG;

    /* initial_state ne vaut pas toujours 1 (cas ou L(G)={epsilon}). */
    XxY_set (&items, initial_state, 0, &item);
    SXBA_1_bit (T1, item);	/* T1 = {(S'->.S$, 0)} */

    for (i = 0; i < n; i++)
    {
	if (!complete (i)) break;

	if (is_single_prods)
	    unchain (i);
       
	slices [i] = NBLONGS (XxY_top (items) + 1);

	if (is_scan_reduces)
	    scan_reduce (i);

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


struct ARN_sem {
  int	elementary_tree, len;
};

static struct ARN_sem	*ARN_sem;


static void
ARN_sem_oflw (old_size, new_size)
    int old_size, new_size;
{
    ARN_sem = (struct ARN_sem*) sxrealloc (ARN_sem, new_size + 1, sizeof (struct ARN_sem));
}


static void
ARN_new_symbol (new_symbol)
    int new_symbol;
{
    ARN_sem [new_symbol].elementary_tree = ARN_sem [new_symbol].len = 0;
}

static void
ARN_parsact (rhs_stack, top, elementary_tree, init_prod)
    int *rhs_stack, top, elementary_tree, init_prod;
{
    int			new_len;
    struct ARN_sem	*psem;

    psem = ARN_sem + rhs_stack [0];

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
	return;
       
    case 7:
	new_len = 1;
	break;
    case 8:
	new_len = ARN_sem [rhs_stack [1]].len + 1;
	break;
    case 9:
	new_len = ARN_sem [rhs_stack [2]].len + 1;
	break;

    case 14:
	psem->len = ARN_sem [rhs_stack [1]].len + 1; 
	return;
    case 15:
	psem->len = 1;
	return;

    default:
	fprintf (sxstderr, "The function \"ARN_action\" is out of date with respect to its specification.\n");
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
	    erase_elementary_tree (elementary_tree);
	}
    else
    {
	erase_elementary_tree (psem->elementary_tree);
	psem->elementary_tree = elementary_tree;
	psem->len = new_len;
    }
    
}

static SXBOOLEAN
ARN_prdct (symbol, prdct_no)
    int symbol, prdct_no;
{
    if (prdct_no == 1)
    {
	return ARN_sem [symbol].len <= 2 /* 30 */;
    }
    else
    {
	fprintf (sxstderr, "The function \"ARN_prdct\" is out of date with respect to its specification.\n");
	abort ();
    }
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

	spf.G.lhs_size = ntmax * (n + 1);
	spf.lhs = (struct lhs*) sxalloc (spf.G.lhs_size + 1, sizeof (struct lhs));

	spf.G.rhs_size = spf.G.lhs_size * (rhs_lgth + 1);
	spf.rhs = (struct rhs*) sxalloc (spf.G.rhs_size + 1, sizeof (struct rhs));
    }

    scan_reduces = SS_alloc (30);

    sprod2order = (int*) sxcalloc (prodmax + 1, sizeof (int));

    grammar ();

    sprod_set = sxba_calloc (sprodmax + 1);
    sprod2state_Bi = (struct state_Bi*) sxalloc (sprodmax + 1, sizeof (struct state_Bi));

    ARN_sem = (struct ARN_sem*) sxalloc (XxY_size (symbols) + 1, sizeof (struct ARN_sem));

    for_parsact.action = ARN_parsact;
    for_parsact.new_symbol = ARN_new_symbol;
    for_parsact.symbols_oflw = ARN_sem_oflw;
    for_parsact.prdct = ARN_prdct;

    if ((l = parse ()) <= n)
	printf ("Syntax error at %i\n", l);

    printf ("\nPT_true = %i, PT_false = %i, T1_nb = %i\n",
	    PT_true, PT_false, T1_nb);

    sxfree (sprod2order);
    sxfree (sprod_set);
    sxfree (sprod2state_Bi);

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

	sxfree (spf.rhs);
	sxfree (spf.lhs);
    }

    sxfree (T1);
    sxfree (T2);    
    SS_free (scan_reduces);

    sxfree (slices);
    sxfree (TOK);

    sxfree (ARN_sem);

    return 0;
}
