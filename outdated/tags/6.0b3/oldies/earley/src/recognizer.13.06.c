#include "sxalloc.h"
#include "sxba.h"
#include "XxY.h"
#include "SS.h";

FILE	*sxtty, *sxstderr;

static struct LC0 {
  int NKshift, lgth;
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




static ntmax = 6;
static tmax = -5;
static statemax = 116;
static prodmax = 35;

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

static int char2tok (c)
    char c;
{
    return (c != 'a') ? 0 : -1;
}

static ntmax = 1;
static tmax = -2;
static statemax = 8;
static prodmax = 2;

#endif



static char	ME [] = "S_SS";

static char	Usage [] = "\
Usage:\t%s \"source string\"\n";


static int		n;
static int		*TOK;
static int		*slices;

static XxY_header	chains;

static SXBA		state_set;
static int		*state_stack;

static SXBA		NT_set, chain_set, nb0;
static int		*NT_stack, *chain_Ai, *nb;
static int		*nt2ntXi;

static XxY_header	ntXi;
static XxY_header	items;
static SXBA		T1, T2;

static SXBA		*ntXi2attr;

static int		*scans;
static BOOLEAN		is_scans;

static int		OR_ee, OR_nee, OR_nb, OR_unchain, OR_complete, OR_T2, OR_scan, T1_nb;

static  void
ntXi_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ntXi2attr = sxbm_resize (ntXi2attr, old_size + 1, new_size + 1, XxY_size (items) + 1);
}

static  void
items_oflw (old_size, new_size)
    int		old_size, new_size;
{
    T1 = sxba_resize (T1, new_size + 1);
    T2 = sxba_resize (T2, new_size + 1);
    ntXi2attr = sxbm_resize (ntXi2attr, XxY_size (ntXi) + 1, XxY_size (ntXi) + 1, new_size + 1);
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
chain (Ai, Bi)
    int Ai, Bi;
{
    int A, B, chain;
    /* On a [Ai -> Bi. , i] donc A[i] \in B[i] */

    A = XxY_X (ntXi, Ai);
    B = XxY_X (ntXi, Bi);

    if (!XxY_set (&chains, A, B, &chain))
    {
	if (SXBA_bit_is_reset_set (chain_set, A))
	{
	    SXBA_1_bit (nb0, A);
	    chain_Ai [A] = Ai;
	}

	if (SXBA_bit_is_reset_set (chain_set, B))
	{
	    nb [B] = 1;
	    chain_Ai [B] = Bi;
	}
	else
	{
	    if (nb [B] == 0)
		SXBA_0_bit (nb0, B);

	    nb [B]++;
	}
    }
}


static void
unchain (i)
    int i;
{
    /* On suppose que la grammaire est non circulaire */
    int		A, Ai, B, Bi, chain, slice = slices [i];
    SXBA	item_set;

    while ((A = sxba_scan_reset (nb0, 0)) > 0)
    {
	SXBA_0_bit (chain_set, A);
	Ai = chain_Ai [A];
	item_set = ntXi2attr [Ai];

	XxY_Xforeach (chains, A, chain)
	{
	    B = XxY_Y (chains, chain);
	    Bi = chain_Ai [B];
	    OR (ntXi2attr [Bi], item_set, slice);
	    OR_unchain++;

	    if (--nb [B] == 0)
		SXBA_1_bit (nb0, B);

	    XxY_erase (chains, chain);
	}
    }
}


static BOOLEAN
complete (i)
{
    int item, new_item, state, k, X, Xi, A, Ak, Ai, Y, Yi, x, lim, top, xs;
    BOOLEAN is_tok = FALSE;

    /* Ca correspond au nombre de tranches du 2eme operande des OR (k < i), qui
       ne change pas au cours de "complete". D'autres elements peuvent etre ajoutes
       directement au 1er operande. */

    item = XxY_top (items) + 1;

    while ((item = sxba_1_rlscan (T1, item)) > 0)
    {
	/* item = (state, k) */
	/* state = A -> alpha . X beta */
	T1_nb++;
	SXBA_0_bit (T1, item);
	state = XxY_X (items, item);
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
	    }
	    else
	    {
		/* beta != epsilon */
		XxY_set (&items, state + 1, k, &new_item);
		SXBA_1_bit (ntXi2attr [Xi], new_item);
	    }
	}
	else
	    if (X == TOK [i+1])
	    {
		if (lispro [state+1] == 0)
		{
		    /* beta == epsilon */
		    A = lhs [prolis [state]];
		    Ak = XxY_is_set (&ntXi, A, k);

#if EBUG
		    if (Ak == 0 || k == i)
			sxtrap (ME, "complete");
#endif

		    OR (T2, ntXi2attr [Ak], slices [k]);
		    OR_T2++;
		}
		else
		{
		    /* beta != epsilon */
		    XxY_set (&items, state + 1, k, &new_item);
		    SXBA_1_bit (T2, new_item);
		}

		is_tok = TRUE;
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
	    }

	    if (lispro [state] == 0)
	    {
		/* delta == epsilon */
		A = lhs [prolis [state]];

		if (SXBA_bit_is_reset_set (NT_set, A))
		{
		    SS_push (NT_stack, A);
		    XxY_set (&ntXi, A, i, &Ai);
		    nt2ntXi [A] = Ai;
		}
		else
		    Ai = nt2ntXi [A];

		if (Y > 0)
		{
		    /* NT */
		    chain (Ai, Yi);
		}
		else
		{
		    SS_push (scans, Ai);
		    is_tok = TRUE;
		    is_scans = TRUE;
		}
	    }
	    else
	    {
		/* delta != epsilon */
		XxY_set (&items, state, i, &new_item);

		if (Y > 0)
		{
		    /* NT */
		    SXBA_1_bit (ntXi2attr [Yi], new_item);
		}
		else
		{
		    SXBA_1_bit (T2, new_item);
		    is_tok = TRUE;
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
scan (i)
    int i;
{
    int	Ai, slice = slices [i];

    do
    {
	Ai = SS_pop (scans);
	OR (T2, ntXi2attr [Ai], slice);
	OR_scan++;
    } while (!SS_is_empty (scans));
}



static int
recognize ()
{
    int	item, i;
    SXBA	T;

    XxY_set (&items, 1, 0, &item);
    SXBA_1_bit (T1, item); /* T1 = {(S'->.S$, 0)} */

    for (i = 0; i < n; i++)
    {
       if (!complete (i)) return i;

       if (XxY_top (chains) > 0)
          unchain (i);
       
       if (is_scans)
          scan (i);

       T = T1, T1 = T2, T2 = T;
    }

    return (item = XxY_is_set (&items, 2, 0)) > 0 /* (S' -> S . $, 0) */ &&
	    SXBA_bit_is_set (T1, item) ? n+1 : n;
}


main (argc, argv)
    int		argc;
    char	*argv [];
{
    int l, t, *tok;
    char	*source, c;

    if (argc == 1) {
	printf (Usage, ME);
	return 1;
    }

    sxopentty ();

    source = argv [1];
    n = strlen (source);

    if (n == 0) {
	printf (Usage, ME);
	return 1;
    }

    tok = TOK = (int*) sxalloc (n + 2, sizeof (int));

    while ((c = *source++) != '\0')
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

    slices = (int*) sxalloc (n + 1, sizeof (int));

    XxY_alloc (&chains, "chains", 30, 1, 1, 0, NULL,
#ifdef EBUG
stdout
#else
NULL
#endif
	       );


    NT_set = sxba_calloc (ntmax + 1);
    NT_stack = SS_alloc (ntmax + 3);
    chain_set = sxba_calloc (ntmax + 1);
    nb0 = sxba_calloc (ntmax + 1);
    chain_Ai = (int*) sxcalloc (ntmax + 1, sizeof (int));
    nb = (int*) sxcalloc (ntmax + 1, sizeof (int));
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

    scans = SS_alloc (30);

    if ((l = recognize ()) <= n)
	printf ("Syntax error at %i\n", l);

    printf ("\nOR_nee = %i, OR_ee = %i, OR_nb = %i,\n\
OR_unchain = %i, OR_complete = %i, OR_T2 = %i, OR_scan = %i, T1_nb = %i\n",
	    OR_nee, OR_ee, OR_nb, OR_unchain,
	    OR_complete, OR_T2, OR_scan, T1_nb);

    XxY_free (&chains);
    sxfree (NT_set);
    SS_free (NT_stack);
    sxfree (chain_set);
    sxfree (chain_Ai);
    sxfree (nb0);
    sxfree (nb);
    sxfree (nt2ntXi);

    sxfree (state_set);
    SS_free (state_stack);

    XxY_free (&ntXi);
    XxY_free (&items);
    sxbm_free (ntXi2attr);

    sxfree (T1);
    sxfree (T2);    
    SS_free (scans);

    sxfree (slices);
    sxfree (TOK);

    return 0;
}
