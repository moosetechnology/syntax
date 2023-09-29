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

static int		ntmax = 1;
static int		tmax = -2;
static int		statemax = 8;
static int		prodmax = 2;
static int		initial_state = 1;
#define	rhs_lgth	2

#endif

struct state_AiBi {
  int	state, Ai, Bi;
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
static struct state_AiBi	*sprod2state_AiBi;

static char	ME [] = "earley";

static char	Usage [] = "\
Usage:\t%s \"source string\"\n";


static int		n;
static int		*TOK;
static char		*source;

static int		*slices;

static SXBA		state_set;

static int		*NT_hd, *NT_state_stack, *NT_stack, NT_stack_top;
static int		*scan_stack, scan_stack_top;
static int		*nt2Ai;

static XxY_header	Ai_hd;
static struct Ai2attr {
  int		Aij, bot;
}			*Ai2attr;
static XxY_header	items;
static struct items2attr {
  int		next_item;
}			*items2attr;
static SXBA		T1, T2;

static SXBA		*shift_sets;

static int		*scan_reduces;
static BOOLEAN		is_single_prods;

static int		T1_nb;
static int		PT_true, PT_false;

static BOOLEAN		is_parser;

/* cas du parser */
static struct reduce_lists {
  int		state, Ai;
}			*reduce_lists;
static int		reduce_lists_top, reduce_lists_size;
/* static XxY_header	reduce_sets; */
/* static SXBA		*reduce_sets; */
static XxY_header	Aij_hd;
static struct Aij2attr {
  int		first_lhs, first_rhs;
}			*Aij2attr;
static SXBA		Ai_set;
static int		rhs_stack [rhs_lgth+1];

static int		*nt_shifts;

static SXBA		order_set;

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
Aij_hd_oflw (old_size, new_size)
    int		old_size, new_size;
{
    Aij2attr = (struct Aij2attr*) sxrealloc (Aij2attr, new_size + 1, sizeof (struct Aij2attr));

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
    struct lhs	*pet = spf.lhs + elementary_tree;

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
	spf.lhs = (struct lhs*) sxrealloc (spf.lhs, spf.G.lhs_size + 1, sizeof (struct lhs));
    }

    if (for_parsact.action == NULL ||
	(*for_parsact.action) (rhs_stack, top, spf.G.lhs_top, init_prod))
    {

	if (spf.G.rhs_top + top >=  spf.G.rhs_size)
	{
	    spf.G.rhs_size *= 2;
	    spf.rhs = (struct rhs*) sxrealloc (spf.rhs, spf.G.rhs_size + 1, sizeof (struct rhs));
	}

	plhs = spf.lhs + spf.G.lhs_top;
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
	    prhs = spf.rhs + spf.G.rhs_top;
	    prhs->lispro = X;
	    prhs->prolis = spf.G.lhs_top;

	    if (X > 0)
	    {
		prhs->next_rhs = Aij2attr [X].first_rhs;
		Aij2attr [X].first_rhs = spf.G.rhs_top;
	    }

	    spf.G.rhs_top++;
	}

	prhs = spf.rhs + spf.G.rhs_top;
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
	psymb = Aij2attr + *Xjk;
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
    shift_sets = sxbm_resize (shift_sets, old_size + 1, new_size + 1, XxY_size (items) + 1);
    Ai2attr = (struct Ai2attr*) sxrealloc (Ai2attr, new_size + 1, sizeof (struct Ai2attr));

    if (is_parser)
    {
	Ai_set = sxba_resize (Ai_set, new_size + 1);
	/* reduce_sets = sxbm_resize (reduce_sets, old_size + 1, new_size + 1, XxY_size (items) + 1); */
    }
}

static  void
items_oflw (old_size, new_size)
    int		old_size, new_size;
{
    T1 = sxba_resize (T1, new_size + 1);
    T2 = sxba_resize (T2, new_size + 1);
    shift_sets = sxbm_resize (shift_sets, XxY_size (Ai_hd) + 1, XxY_size (Ai_hd) + 1, new_size + 1);
    items2attr = (struct items2attr*) sxrealloc (items2attr, new_size + 1, sizeof (struct items2attr));

#if 0
    if (is_parser)
    {
	reduce_sets = sxbm_resize (reduce_sets, XxY_size (Ai_hd) + 1, XxY_size (Ai_hd) + 1, new_size + 1);
    }
#endif
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
    int		X, Xj, Xjl, Xjk, k;
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
		ret_val = set_rule (prolis [state], top);
	    }
	    else
		ret_val = parse_tree (state+1, bot+1, top, j+1, l);
	}
    }
    else
    {
	ret_val = FALSE;

	/* X \in N */
	Xj = XxY_is_set (&Ai_hd, X, j);

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
		rhs_stack [bot] = Xjl;
		ret_val = set_rule (prolis [state], top);
	    }
	}
	else
	{
	    XxY_Xforeach (Aij_hd, Xj, Xjk)
	    {
		if ((k = XxY_Y (Aij_hd, Xjk)) < l && Aij2attr [Xjk].first_lhs > 0)
		{
		    rhs_stack [bot] = Xjk;
		    ret_val |= parse_tree (state+1, bot+1, top, k, l);
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




static void
reduce (k)
    int		k;
{
    /* Ai_set = {((A, i)} : ensemble des reduces du niveau courant (on vient de faire
       une transition sur ak). */
    int		i, A, Ai, Aik, j, Bj, Bjk, prod, reduce_item, reduce_state, top, start, x, lim;
    int		prdct_no, constraint_no;
    SXBA	reduce_set;

    Ai = XxY_top (Ai_hd) + 1;

    while ((Ai = sxba_1_rlscan (Ai_set, Ai)) > 0)
    {
	SXBA_0_bit (Ai_set, Ai);
	Aik = Ai2attr [Ai].Aij;
	A = XxY_X (Ai_hd, Ai);

	/* On est su^r que toutes les reductions vers Aik ont ete effectuees. */
	if ((constraint_no = constraints [A]) == -1 ||
	    for_parsact.constraint == NULL ||
	    (*for_parsact.constraint) (Aik, constraint_no))
	{

	    SS_push (nt_shifts, Aik);

	    i = XxY_Y (Ai_hd, Ai);

#if 0
	    reduce_item = MUL (slices [i]);
	    reduce_set = reduce_sets [Ai];

	    while ((reduce_item = sxba_1_rlscan (reduce_set, reduce_item)) > 0)
	    XxY_Xforeach (reduce_sets, Ai, AiXitem)
#endif
	    for (lim = Ai2attr [Ai+1].bot, x = Ai2attr [Ai].bot; x < lim; x++)
	    {
		/* (B -> beta A., Bj) */
		reduce_state = reduce_lists [x].state;
		
		if ((prdct_no = prdct [reduce_state-1]) == -1 ||
		    for_parsact.prdct == NULL ||
		    (*for_parsact.prdct) (Aik, prdct_no))
		{
		   prod = prolis [reduce_state];
		   Bj = reduce_lists [x].Ai;
		   j = XxY_Y (Ai_hd, Bj);
		   set_symbol (Bj, k, &Bjk);
		   rhs_stack [0] = Bjk;
		   start = prolon [prod];
		   top = reduce_state - start;
		   rhs_stack [top] = Aik;

		   if (top == 1)
			set_rule (prod, 1);
		    else
			parse_tree (start, 1, top, j, i);

		   SXBA_1_bit (Ai_set, Bj);
		   Ai2attr [Bj].Aij = Bjk;
		}
	    }
	}
    }
}


static void
scan_reduce (k)
    int k;
{
    int		state, Ai, i, prod, Aik, reduce_item, x, top, start;
    int		reduce_state, shift_item, shift_state, prdct_no, Xjh;
    SXBA	shift_set;

    do
    {
	Ai = SS_pop (scan_reduces);
	state = SS_pop (scan_reduces);
	i = XxY_Y (Ai_hd, Ai);
	prod = prolis [state];

	set_symbol (Ai, k, &Aik);

	rhs_stack [0] = Aik;
	start = prolon [prod];
	top = state - start;
	rhs_stack [top] = -k;

	if (top == 1)
	    set_rule (prod, 1);
	else
	    parse_tree (start, 1, top, i, k-1);

	SXBA_1_bit (Ai_set, Ai);
	Ai2attr [Ai].Aij = Aik;
    } while (!SS_is_empty (scan_reduces));

    reduce (k);

    while (!SS_is_empty (nt_shifts))
    {
	Aik = SS_pop (nt_shifts);
	Ai = XxY_X (Aij_hd, Aik);
	i = XxY_Y (Ai_hd, Ai);

	shift_set = shift_sets [Ai];

	if (for_parsact.prdct == NULL)
	    OR (T2, shift_set, slices [i]);
	else {
	  shift_item = MUL (slices [i]);

	  while ((shift_item = sxba_1_rlscan (shift_set, shift_item)) > 0)
	  {
	      /* On pourrait partitionner les shift_sets en 2 (avec ou sans predicat)? */
	      shift_state = XxY_X (items, shift_item);

	      if ((prdct_no = prdct [shift_state]) == -1 ||
		  (*for_parsact.prdct) (Aik, prdct_no))
	      {
		  SXBA_1_bit (T2, shift_item);
	      }
	  }
      }
    }
}



#if 0
static BOOLEAN
look_ahead (state, i)
    int state, i;
{
    int t = -TOK [i];

    return SXBA_bit_is_set (FIRST [state], t);
}
#endif

#define look_ahead(i,j)		TRUE



static BOOLEAN
complete (i)
{
    int			item, new_item, state, k, X, Xi, A, Ak, Y, x, lim, top, xs, prod, prdct_no, order;
    BOOLEAN		is_tok = FALSE, is_NT = FALSE;
    int			lgth = 0;
    struct reduce_lists	*preduce;

    /* Les Ai_hd doivent etre generes ds le bon ordre :
       (A, i) < (B, j) <= i<j || i==j && A->+B. */

    item = XxY_top (items) + 1;

    while ((item = sxba_1_rlscan (T1, item)) > 0)
    {
	/* item = (state, Ak) */
	/* state = A -> alpha . X beta et
	   X ne derive pas que ds le vide */
	T1_nb++;
	SXBA_0_bit (T1, item);
	state = XxY_X (items, item);

	if (look_ahead (state, i+1))
	{
	    X = lispro [state];

	    if (X > 0)
	    {
		/* NT */
		is_NT = TRUE;
		order = nt2order [X];
		
		if (SXBA_bit_is_reset_set (order_set, order))
		{
		    NT_hd [X] = 0;

		    for (x = LC0 [X].NKshift, lim = x + LC0 [X].lgth; x < lim; x++)
		    {
			state = NKshift [x];

			if (SXBA_bit_is_reset_set (state_set, state))
			{
			    if (look_ahead (state-1, i+1))
			    {
				Y = lispro [state - 1];

				if (Y > 0)
				{
				    order = nt2order [Y];
		
				    if (SXBA_bit_is_reset_set (order_set, order))
					NT_hd [Y] = 0;

				    NT_state_stack [state] = NT_hd [Y];
				    NT_hd [Y] = -state;
				    lgth++;
				}
				else
				{
				    if (Y == TOK [i+1])
					/* Test inutile quand "look-ahead" sera implante' */
				    {
					if (look_ahead (state, i+2) &&
					    ((prdct_no = prdct [state-1]) == -1 ||
					     for_parsact.prdct == NULL ||
					     (*for_parsact.prdct) (-i-1, prdct_no)))
					    scan_stack [scan_stack_top++] = state;
				    }
				}
			    }
			}
		    }
		}

		items2attr [item].next_item = NT_hd [X];
		NT_hd [X] = item;
		lgth++;
	    }
	    else
	    {
		if (X == TOK [i+1]) /* toujours vrai! quand le look-ahead sera implante' */
		{
		    state = XxY_X (items, item) + 1;

		    if (look_ahead (state, i+2) &&
			((prdct_no = prdct [state-1]) == -1 ||
			 for_parsact.prdct == NULL ||
			 (*for_parsact.prdct) (-i-1, prdct_no)))
		    {
			Ak = XxY_Y (items, item);

			if (lispro [state] == 0)
			{
			    /* delta == epsilon */
			    /* les items scan-reduce doivent etre generes en dernier */
			    SS_push (scan_reduces, state);
			    SS_push (scan_reduces, Ak);
			}
			else
			{
			    /* delta != epsilon */
			    XxY_set (&items, state, Ak, &new_item);
			    SXBA_1_bit (T2, new_item);
			}

			is_tok = TRUE;
		    }
		}
	    }
	}
    }
	    
    if (is_NT)
    {
	order = 0;

	while ((order = sxba_scan_reset (order_set, order)) > 0)
	{
	    X = order2nt [order];
	    XxY_set (&Ai_hd, X, i, &Xi);
	    nt2Ai [X] = Xi;
	    NT_stack [NT_stack_top++] = X;
	}

	sxba_empty (state_set);
    }

    if (NT_stack_top > 0)
    {
	if (reduce_lists_top + lgth > reduce_lists_size)
	{
	    reduce_lists_size += reduce_lists_top + lgth;
	    reduce_lists = (struct reduce_lists*) sxrealloc (reduce_lists,
							   reduce_lists_size + 1,
							   sizeof (struct reduce_lists));
	}

	for (x = 0; x < NT_stack_top; x++)
	{
	    X = NT_stack [x];
	    Xi = nt2Ai [X];
	    preduce = reduce_lists + reduce_lists_top;
	    item = NT_hd [X];

	    while (item != 0)
	    {
		if (item < 0)
		{
		    state = -item;
		    A = lhs [prolis [state]];
		    Ak = nt2Ai [A];
		    item = NT_state_stack [state];
		}
		else
		{
		    Ak = XxY_Y (items, item);
		    state = XxY_X (items, item) + 1;
		    item = items2attr [item].next_item;
		}

		if (lispro [state] == 0)
		{
		    /* delta == epsilon */
		    /* production simple A -> Y
		       les items de la partie gauche doivent etre generes avant ceux de Y[i]. */
		    /* SXBA_1_bit (reduce_sets [Xi], new_item); */
		    preduce->state = state;
		    preduce->Ai = Ak;
		    preduce++;
		}
		else
		{
		    /* delta != epsilon */
		    XxY_set (&items, state, Ak, &new_item);
		    SXBA_1_bit (shift_sets [Xi], new_item);
		}
	    }

	    Ai2attr [Xi+1].bot = reduce_lists_top = preduce - reduce_lists;
	}

	NT_stack_top = 0;
    }

    if (scan_stack_top > 0)
    {
	do
	{
	    state = scan_stack [--scan_stack_top];
	    A = lhs [prolis [state]];
	    Ak = nt2Ai [A];

	    if (lispro [state] == 0)
	    {
		/* delta == epsilon */
		/* les items scan-reduce doivent etre generes en dernier */
		SS_push (scan_reduces, state);
		SS_push (scan_reduces, Ak);
	    }
	    else
	    {
		/* delta != epsilon */
		XxY_set (&items, state, Ak, &new_item);
		SXBA_1_bit (T2, new_item);
	    }

	    is_tok = TRUE;
	} while (scan_stack_top > 0);
    }

    return is_tok;
}



static void
useless_symbol_elimination ()
{
    int		*symbol_stack;
    SXBA	Aij_set;
    int		symbol, prod, x;

    symbol_stack = SS_alloc (XxY_top (Aij_hd) + 3);
    Aij_set = sxba_calloc (XxY_top (Aij_hd) + 1);

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

    symbol = XxY_top (Aij_hd) + 1;

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

static int
parse ()
{
    int		item, i, S0;
    SXBA	T;
    BOOLEAN	is_in_LG;

    /* initial_state ne vaut pas toujours 1 (cas ou L(G)={epsilon}). */

    XxY_set (&Ai_hd, 0, 0, &S0); /* (S', 0) */
    XxY_set (&items, initial_state, S0, &item);
    SXBA_1_bit (T1, item);	/* T1 = {(S'->.S$, (S', 0))} */

    for (i = 0; i < n; i++)
    {
	if (!complete (i)) break;

	slices [i] = NBLONGS (XxY_top (items) + 1);

	if (!SS_is_empty (scan_reduces))
	    scan_reduce (i+1);

	T = T1, T1 = T2, T2 = T;
    }

    is_in_LG = (item = XxY_is_set (&items, 2 /* final_state */, S0)) > 0 /* (S' -> S . $, 0) */ &&
	SXBA_bit_is_set (T1, item);

    if (is_parser)
    {
	spf.G.N = XxY_top (Aij_hd);
	spf.G.P = spf.G.lhs_top;
	spf.G.T = i;
	spf.G.G = spf.G.rhs_top;

	if (is_in_LG)
	{
	    spf.G.start_symbol = XxY_is_set (&Aij_hd, XxY_is_set (&Ai_hd, 1 /* axiome */, 0), n);

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

static BOOLEAN
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

    is_parser = TRUE;

    slices = (int*) sxalloc (n + 1, sizeof (int));

    nt2Ai = (int*) sxalloc (ntmax + 1, sizeof (int));
    order_set = sxba_calloc (ntmax+1);
    NT_hd = (int*) sxalloc (ntmax + 1, sizeof (int));
    NT_stack = (int*) sxalloc (ntmax + 1, sizeof (int));

    state_set = sxba_calloc (statemax + 1);
    scan_stack = (int*) sxalloc (statemax + 1, sizeof (int));
    NT_state_stack = (int*) sxalloc (statemax + 1, sizeof (int));

    XxY_alloc (&Ai_hd, "Ai_hd", ntmax * (n + 1), 1, 0, 0, Ai_hd_oflw,
#ifdef EBUG
stdout
#else
NULL
#endif
	       );
    Ai2attr = (struct Ai2attr*) sxalloc (XxY_size (Ai_hd) + 1, sizeof (struct Ai2attr));

    Ai2attr [1].bot = 0 /* = reduce_list_top */;

    XxY_alloc (&items, "items", (statemax - 2*prodmax + 1) * (n + 1), 1, 0, 0, items_oflw,
#ifdef EBUG
stdout
#else
NULL
#endif
	       );
    items2attr = (struct items2attr*) sxalloc (XxY_size (items) + 1, sizeof (struct items2attr));

    T1 = sxba_calloc (XxY_size (items) + 1);
    T2 = sxba_calloc (XxY_size (items) + 1);

    shift_sets = sxbm_calloc (XxY_size (Ai_hd) + 1, XxY_size (items) + 1);

    if (is_parser)
    {
	Ai_set = sxba_calloc (XxY_size (Ai_hd) + 1);

	reduce_lists_size = ntmax * (n + 1);
	reduce_lists = (struct reduce_lists*) sxalloc (reduce_lists_size + 1, sizeof (struct reduce_lists));
	/* reduce_sets = sxbm_calloc (XxY_size (Ai_hd) + 1, XxY_size (items) + 1); */
	/* XxY_alloc (&reduce_sets, "reduce_sets", ntmax * (n + 1), 1, 1, 0, NULL,
#ifdef EBUG
stdout
#else
NULL
#endif
		   ); */
	
	XxY_alloc (&Aij_hd, "Aij_hd", ntmax * (n + 1), 1, 1, 0, Aij_hd_oflw,
#ifdef EBUG
stdout
#else
NULL
#endif
		   );
	Aij2attr = (struct Aij2attr*) sxalloc (XxY_size (Aij_hd) + 1, sizeof (struct Aij2attr));

	nt_shifts = SS_alloc (ntmax + 3);

	spf.G.lhs_size = ntmax * (n + 1);
	spf.lhs = (struct lhs*) sxalloc (spf.G.lhs_size + 1, sizeof (struct lhs));

	spf.G.rhs_size = spf.G.lhs_size * (rhs_lgth + 1);
	spf.rhs = (struct rhs*) sxalloc (spf.G.rhs_size + 1, sizeof (struct rhs));
    }

    scan_reduces = SS_alloc (30);

    sprod2order = (int*) sxcalloc (prodmax + 1, sizeof (int));

    grammar ();

    sprod_set = sxba_calloc (sprodmax + 1);
    sprod2state_AiBi = (struct state_AiBi*) sxalloc (sprodmax + 1, sizeof (struct state_AiBi));

    ARN_sem = (struct ARN_sem*) sxalloc (XxY_size (Aij_hd) + 1, sizeof (struct ARN_sem));

    for_parsact.action = ARN_parsact;
    for_parsact.new_symbol = ARN_new_symbol;
    for_parsact.Aij_hd_oflw = ARN_sem_oflw;
    for_parsact.prdct = NULL;
    for_parsact.constraint = ARN_constraint;

    if ((l = parse ()) <= n)
	printf ("Syntax error at %i\n", l);

    printf ("\nPT_true = %i, PT_false = %i, T1_nb = %i\n",
	    PT_true, PT_false, T1_nb);

    sxfree (sprod2order);
    sxfree (sprod_set);
    sxfree (sprod2state_AiBi);

    sxfree (NT_stack);
    sxfree (nt2Ai);
    sxfree (order_set);
    sxfree (NT_hd);
    sxfree (NT_state_stack);

    sxfree (state_set);
    sxfree (scan_stack);

    XxY_free (&Ai_hd);
    sxfree (Ai2attr);
    XxY_free (&items);
    sxfree (items2attr);
    sxbm_free (shift_sets);

    if (is_parser)
    {
	sxfree (Ai_set);
	sxfree (reduce_lists);
	/* sxbm_free (reduce_sets); */
	/* XxY_free (&reduce_sets); */
	XxY_free (&Aij_hd);
	sxfree (Aij2attr);

	SS_free (nt_shifts);

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
