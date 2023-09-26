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


/* Une regle de la foret partagee issue de la production p = A -> w0 B1 ... Bl wl
   est un doublet (Aik, m) avec m = (... ((-p, j1), j2), ..., jl) ou les j1, ..., jl sont les
   index finals de B1, ..., Bl.
   Ex: Si on a la prod p = A -> B C qui a donne les 4 regles
   Aim -> Bil Clm
   Aim -> Bik Ckm
   Ajm -> Bjl Clm
   Ajm -> Bjk Ckm
   qui utilisent 8 symboles differents et 12 occur
   elles sont representees par
   (Aim, (-p,l))
   (Aim, (-p,k))
   (Ajm, (-p,l))
   (Ajm, (-p,k))
   4 symbols differents et 8 occur.
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
#include "X.h"
#include "SS.h"

static SXBOOLEAN		is_default_semantics, is_print_prod, print_time;
static int		max_tree_nb;

#define TIME_INIT	0
#define TIME_FINAL	1

static void
sxtime (what, str)
    int what;
    char *str;
{
#include <sys/time.h>
#include <sys/resource.h>
    extern void getrusage ();
    static struct rusage prev_usage, next_usage;
    register long t;

    if (what == TIME_INIT) {
	getrusage (RUSAGE_SELF, &prev_usage);
    }
    else
    {
	getrusage (RUSAGE_SELF, &next_usage);
	t =  (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
	prev_usage.ru_utime.tv_sec = next_usage.ru_utime.tv_sec;
	prev_usage.ru_utime.tv_usec = next_usage.ru_utime.tv_usec;

	printf ("%s (%dms)\n", str, t);
    }
}


static int
sxlast_bit (nb)
    int nb;
{
    /* retourne :
       si nb est nul 0
       sinon k tel que :
        k-1          k
       2    <= nb < 2
    */
static int	LASTBIT [] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};

    register int	bit = 0;

    while ((nb & ~0xFFFF) != 0) {
	nb >>= 16;
	bit += 16;
    }

    if ((nb & 0xFF00) != 0) {
	nb >>= 8;
	bit += 8;
    }

    if ((nb & 0xF0) != 0) {
	nb >>= 4;
	bit += 4;
    }

    return bit + LASTBIT [nb & 0xF];
}


#if 0
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
#endif


FILE	*sxtty, *sxstdout = {stdout}, *sxstderr = {stderr};

static struct LC0 {
  int NKshift, lgth;
};

struct for_parsact {
    void	(*sem_init) (),
                (*sem_final) ();

    SXBOOLEAN	(*prdct) (),
                (*action) (),
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

30: <Base>		@16 = A ;
31: <Base>		@16 = U ;
32: <Base>		@16 = C ;
33: <Base>		@16 = G ;

34: <SingleStrand>	@14 = <SingleStrand> &1 <Base> ;
35: <SingleStrand>	@15 = <Base> ;

* @7 : len = 0;
* @8 : len = $1.len +1;
* @9 : len = $2.len +1;

* @14 :len = S1.len + 1;
* @15 : len = 0;

* &1 SXFALSE <==> len > 30

*/



#define ntmax		6
#define tmax		(-5)
#define itemmax		116 
#define prodmax		35
#define initial_state	1
#define	rhs_lgth	3
/* rhs_maxnt nb max d'occur de nt en rhs */
#define	rhs_maxnt	2
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

static int prod2nbnt [] = 	{
1,

2,
1,
2,
1,

1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,
1,

1,
2,
2,

2,
1,
2,
1,

0,
0,
0,
0,

2,
1
};


/* Associe a une production et son pieme non terminal en rhs
   le nombre de terminaux separant le p-1 eme nt du p eme.
   si p == 0, c'est le nombre de terminaux en fin de rhs. */
static int prodXnt2nbt [prodmax+1][rhs_maxnt+1] = {
{0, 0, 0},	/* 0: 		= <RNA> $ ; */

{0, 0, 0},	/* 1: <RNA>		= <RNA> <DoubleStrand> ; */
{0, 0, 0},	/* 2: <RNA>		= <DoubleStrand> ; */
{0, 0, 0},	/* 3: <RNA>		= <RNA> <Base> ; */
{0, 0, 0},	/* 4: <RNA>		= <Base> ; */

{1, 1, 0},	/* 5: <DoubleStrand>	= A <SingleStrand> U ; */
{1, 1, 0},	/* 6: <DoubleStrand>	= A <InternalLoop> U ; */
{1, 1, 0},	/* 7: <DoubleStrand>	= A <MultipleLoop> U ; */
{1, 1, 0},	/* 8: <DoubleStrand>	= G <SingleStrand> C ; */
{1, 1, 0},	/* 9: <DoubleStrand>	= G <InternalLoop> C ; */
{1, 1, 0},	/*10: <DoubleStrand>	= G <MultipleLoop> C ; */
{1, 1, 0},	/*11: <DoubleStrand>	= G <SingleStrand> U ; */
{1, 1, 0},	/*12: <DoubleStrand>	= G <InternalLoop> U ; */
{1, 1, 0},	/*13: <DoubleStrand>	= G <MultipleLoop> U ; */
{1, 1, 0},	/*14: <DoubleStrand>	= U <SingleStrand> A ; */
{1, 1, 0},	/*15: <DoubleStrand>	= U <InternalLoop> A ; */
{1, 1, 0},	/*16: <DoubleStrand>	= U <MultipleLoop> A ; */
{1, 1, 0},	/*17: <DoubleStrand>	= C <SingleStrand> G ; */
{1, 1, 0},	/*18: <DoubleStrand>	= C <InternalLoop> G ; */
{1, 1, 0},	/*19: <DoubleStrand>	= C <MultipleLoop> G ; */
{1, 1, 0},	/*20: <DoubleStrand>	= U <SingleStrand> G ; */
{1, 1, 0},	/*21: <DoubleStrand>	= U <InternalLoop> G ; */
{1, 1, 0},	/*22: <DoubleStrand>	= U <MultipleLoop> G ; */

{0, 0, 0},	/*23: <InternalLoop>	= <DoubleStrand> ; */
{0, 0, 0},	/*24: <InternalLoop>	= <InternalLoop> <Base> ; */
{0, 0, 0},	/*25: <InternalLoop>	= <Base> <InternalLoop> ; */

{0, 0, 0},	/*26: <MultipleLoop>	= <MultipleLoop> <DoubleStrand> ; */
{0, 0, 0},	/*27: <MultipleLoop>	= <DoubleStrand> ; */
{0, 0, 0},	/*28: <MultipleLoop>	= <MultipleLoop> <Base> ; */
{0, 0, 0},	/*29: <MultipleLoop>	= <Base> ; */

{1, 0, 0},	/*30: <Base>		= A ; */
{1, 0, 0},	/*31: <Base>		= U ; */
{1, 0, 0},	/*32: <Base>		= C ; */
{1, 0, 0},	/*33: <Base>		= G ; */

{0, 0, 0},	/*34: <SingleStrand>	= <SingleStrand> <Base> ; */
{0, 0, 0},	/*35: <SingleStrand>	= <Base> ; */
};

static float probabilities [] = {
0.,

0.4,
0.3,
0.2,
0.1,

0.2,
0.3,
0.5,
0.2,
0.3,
0.5,
0.2,
0.3,
0.5,
0.2,
0.3,
0.5,
0.2,
0.3,
0.5,
0.2,
0.3,
0.5,

0.6,
0.2,
0.2,

0.5,
0.3,
0.1,
0.1,

1.0,
1.0,
1.0,
1.0,

0.5,
0.5
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

16,
16,
16,
16,

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

/* Si A -> \alpha B => A <= B */
/* Construction (approximative ds le cas general) manuelle d'une relation d'ordre total
   verifiant les contraintes precedentes.  Ca marche pour la grammaire RNA. Le faire
   automatiquement pour toutes les grammaires (heuristique). */
static int rlorder2nt [] = {0,
			      1, /* "RNA" */
			      5, /* "InternalLoop" */
			      6, /* "MultipleLoop" */
			      4, /* "SingleStrand" */
			      2, /* "DoubleStrand" */
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
#define itemmax		8 
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

static int			sprodmax;
static int			*sprod2order;
static SXBA			sprod_set;
static struct sprod2attr	*sprod2attr;
static int			item2nbt [itemmax+1];
static int			order2prod [prodmax+1], prod2order [prodmax+1],
                                order2next [prodmax+1], parse_order2next [prodmax+1];
static SXBA                     is_right_recursive_prod;
static SXBOOLEAN                  is_right_recursive;
static SXBA			*INIT, *FIRST_1, *NT2CHAIN, RHS_IN_T;



static char	ME [] = "earley_LC";

static char	Usage [] = "\
Usage:\t%s ('R' | 'r' |'P' | 'p') ['p'] \"source string\"\n";


static	int		xprod;

static int		n;
static int		logn, filtren;

static int		*A2A0j [ntmax+1];

static int		A0j2Aij_size, A0j2Aij_top;

#define MAKE_Ai(A,i)	(((A)<<logn)+(i))
#define MAKE_Ai2i(Ai) (((Ai)&filtren))
#define MAKE_Ai2A(Ai) (((Ai)>>logn))

#define MAKE_A0j(A,j)	(A2A0j [A][j])
#define MAKE_Aij(A0j,i)	((A0j)+(i))


static int		*TOK;
static char		*source;

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


static SXBOOLEAN		is_parser;

static struct recognize_item {
    int		shift_NT_hd [ntmax + 1];
    int		shift_state_stack [itemmax + 1];
    SXBA	index_sets [itemmax + 1];
}			*RT;

static int		*T1_shift_NT_hd, *T2_shift_NT_hd;
static int		*T1_shift_state_stack, *T2_shift_state_stack;
static SXBA             *T1_index_sets, *T2_index_sets, *T2_backward_index_sets;

static int		shift_NT_stack_1 [ntmax + 1],
                        shift_NT_stack_2 [ntmax + 1];
static int		*T1_shift_NT_stack, *T2_shift_NT_stack;


static bag_header	shift_bag;


static SXBA		*global_state_sets;

static SXBA		init_state_set;

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

static int		*reduce_hd [prodmax + 1];
static int		*global_reduce_hd /* [prodmax * (n+1) ] */;

static struct reduce_list {
    int		prod, next;
    SXBA	index_set;
}			*reduce_list;
static int		reduce_list_cur, reduce_list_top;


static struct parse_item {
  SXBA			backward_index_sets [itemmax + 1];
  int			reduce_NT_hd [ntmax + 1];
}			*PT;

static int		*T2_reduce_NT_hd;

static SXBA		level0_index_set;

static bag_header	pt2_bag;

static struct PT2_item {
    int			shift_NT_stack [itemmax + 1];
    SXBA		index_sets [itemmax + 1];
    SXBA		backward_index_sets [itemmax + 1];
} *PT2;


static SXBA	RL_nt_set, prod_order_set;
static SXBA	*ap_sets;
static SXBA     *red_order_sets;

struct ARN_sem {
  int	val;
  int   rhs_stack [rhs_maxnt+1];
};

static struct ARN_sem	*ARN_sem;

struct ARN_ksem {
    float 	val;
    int		vector [rhs_maxnt+1];
    int 	rhs_stack [rhs_maxnt+1];
    SXBOOLEAN	is_marked;
};

static struct ARN_ksem	*ARN_ksem, **ARN_disp_ksem, *ARN_ksem_top;

static struct kvals {
    float	val;
    int 	vector [rhs_maxnt+1];
} *kvals, **disp_kvals;

static SXBA	Aij_set;

#if EBUG
static SXBA     Aij_rhs_set, Aij_lhs_set;
#endif

static SXBA     Aij_sem_lhs_set;
/* end is_parser */

static int		nt2nb [ntmax+1];
static int		nt_stack [ntmax+1];
static XxY_header	order_graph;
static SXBA		nyp_gset;
static SXBA		nt_gset;
static SXBA		nt_gset2;
static SXBA		prod_gset;
static int		gorder;

static void
found_cycle (B)
    int B;
{
    /* nt_gset contient un cycle, on le complete */
    int	prod, C, couple;

    SXBA_1_bit (nt_gset2, B);

    XxY_Yforeach (order_graph, B, couple)
    {
	prod = XxY_X (order_graph, couple);
	C = lhs [prod];

	if (SXBA_bit_is_set (nt_gset, C))
	{
	    sxba_or (nt_gset, nt_gset2);
	}
	else
	    if (!SXBA_bit_is_set (nt_gset, C))
		found_cycle (C);
    }

    SXBA_0_bit (nt_gset2, B);
}


static void
process_cycle (A)
    int A;
{
    int card, card2, B, couple, prod, next_prod, C;

    card2 = sxba_cardinal (nt_gset);

    do
    {
	card = card2;
	found_cycle (A);
	card2 = sxba_cardinal (nt_gset);
    } while (card2 > card);

    /* traitement du cycle */

    B = 0;

    while ((B = sxba_scan (nt_gset, B)) > 0)
    {
	XxY_Yforeach (order_graph, B, couple)
	{
	    prod = XxY_X (order_graph, couple);
	    C = lhs [prod];

	    if (SXBA_bit_is_set (nt_gset, C))
	    {
		SXBA_1_bit (prod_gset, prod);
	    }
	}
    }

    sxba_or (is_right_recursive_prod, prod_gset);

    card = sxba_cardinal (prod_gset);

    next_prod = -1;
    prod = sxba_scan_reset (prod_gset, 0);

    {
	order2prod [gorder] = prod;
	prod2order [prod] = gorder;
	order2next [gorder] = next_prod < 0 ? gorder-card : gorder; /* sequence */
	next_prod = sxba_scan_reset (prod_gset, prod);
	parse_order2next [gorder] = next_prod < 0 ? gorder+card : gorder; /* sequence */
	gorder--;
	B = lhs [prod];
	nt2nb [B]--;
	nt2nb [0]--;

	if (nt2nb [B] == 0)
	{
	    PUSH (nt_stack, B);
	    SXBA_0_bit (nyp_gset, B);
	}
    } while ((prod = next_prod) > 0);
}


static SXBOOLEAN
look_cycle (A, B)
    int A, B;
{
    /* On cherche un cycle autour de A */
    /* On est sur B */
    int C, prod, couple;

    SXBA_1_bit (nt_gset, B);

    XxY_Yforeach (order_graph, B, couple)
    {
	prod = XxY_X (order_graph, couple);
	C = lhs [prod];

	if (C == A)
	{
	    process_cycle (A);
	    break;
	}
	else
	    if (!SXBA_bit_is_set (nt_gset, C) && look_cycle (A, C))
		break;
    }

    SXBA_0_bit (nt_gset, B);

    return couple != 0;
}

static void
grammar ()
{
    int		prod, state, X, Y, A, B, x, lim, t, item, tnb, couple;
    SXBA	*NT2NT, *NT2T;

    /* calcul de order2prod, prod2order, order2next et parse_order2next */
    /* order2next est exploite par valeurs croissantes de order
       parse_order2next est exploite par valeurs decroissantes de order */

    is_right_recursive_prod = sxba_calloc (prodmax+1);

    for (prod = 1; prod <= prodmax; prod++)
	nt2nb [lhs [prod]]++;

    nt2nb [0] = prodmax;

    nyp_gset = sxba_calloc (ntmax+1);
    nt_gset = sxba_calloc (ntmax+1);
    nt_gset2 = sxba_calloc (ntmax+1);
    prod_gset = sxba_calloc (prodmax+1);

    sxba_fill (nyp_gset);
    
    XxY_alloc (&order_graph, "order_graph", prodmax, 1, 0, 1, NULL, NULL);
    gorder = prodmax;

    for (prod = 1; prod <= prodmax; prod++)
    {
	item = prolon [prod+1] - 2;
	A = lhs [prod];

	if ((Y = lispro [item]) < 0)
	{
	    /* item = A -> \alpha t. */
	    order2prod [gorder] = prod;
	    prod2order [prod] = gorder;
	    order2next [gorder] = gorder; /* en sequence */
	    parse_order2next [gorder] = gorder; /* en sequence */
	    gorder--;
	    nt2nb [A]--;
	    nt2nb [0]--;
	}
	else
	{
	    XxY_set (&order_graph, prod, Y, &couple);
	}

	if (nt2nb [A] == 0)
	{
	    PUSH (nt_stack, A);
	    SXBA_0_bit (nyp_gset, A);
	}
    }

    while (nt2nb [0] > 0)
    {
	/* Au boulot */
	while (!IS_EMPTY (nt_stack))
	{
	    B = POP (nt_stack);
	    /* Toutes les B-productions ont ete ordonnees. */

	    XxY_Yforeach (order_graph, B, couple)
	    {
		prod = XxY_X (order_graph, couple);
		XxY_erase (order_graph, couple);

		A = lhs [prod];
		order2prod [gorder] = prod;
		prod2order [prod] = gorder;
		order2next [gorder] = gorder; /* en sequence */
		parse_order2next [gorder] = gorder; /* en sequence */
		gorder--;
		nt2nb [A]--;
		nt2nb [0]--;

		if (nt2nb [A] == 0)
		{
		    PUSH (nt_stack, A);
		    SXBA_0_bit (nyp_gset, A);
		}
	    }
	}

	if (nt2nb [0] > 0)
	{
	    /* ici, cycle ds order_graph car recursivite droite. */
	    /* on cherche les cycles */
	    is_right_recursive = SXTRUE;
	    A = 0;

	    while ((A = sxba_scan (nyp_gset, A)) > 0)
	    {
		look_cycle (A, A);
	    }
	}
    }

    XxY_free (&order_graph);
    sxfree (nyp_gset);
    sxfree (nt_gset);
    sxfree (nt_gset2);
    sxfree (prod_gset);

    /* calcul de item2nbt */
    for (prod = 0; prod <= prodmax; prod++)
    {
	item = prolon [prod];
	/* item2nbt [item] == 0 (static) */

	tnb = 0;
	
	while ((Y = lispro [item++]) != 0)
	{
	    if (Y < 0)
		tnb++;
	    else
		tnb = 0;

	    item2nbt [item] = tnb;
	}
    }
	

    /* On remplit RHS_IN_T */
    RHS_IN_T = sxba_calloc (prodmax +1);

    for (prod = 0; prod <= prodmax; prod++)
    {
	state = prolon [prod];

	while ((Y = lispro [state++]) < 0);

	if (Y == 0)
	    SXBA_1_bit (RHS_IN_T, prod);
    }
	    

    /* On remplit INIT [A] = {B -> . alpha | B <* A} */
    INIT = sxbm_calloc (ntmax + 1, itemmax + 1);

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

    NT2CHAIN = sxbm_calloc (ntmax + 1, itemmax + 1);

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

    FIRST_1 = sxbm_calloc (-tmax + 1, itemmax + 1);
    
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

    chains2attr = (struct chains2attr*) sxalloc (XxY_size (chains)+1, sizeof (struct chains2attr));

    chain_set = sxba_calloc (ntmax+1);
    nb0 = sxba_calloc (ntmax+1);
    /* nb = (int*) sxcalloc (ntmax+1, sizeof (int)); */

    x = 4;

    while (x <= itemmax)
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
    sxfree (RHS_IN_T);
    sxfree (is_right_recursive_prod);
}




static SXBOOLEAN
NON_EQUAL (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- != *rhs_bits_ptr--)
	    return SXTRUE;
    }

    return SXFALSE;
}


static SXBOOLEAN
IS_INCLUDE (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));
    register SXBA_ELT	val;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	val = *lhs_bits_ptr--;

	if ((val & *rhs_bits_ptr--) != val)
	    return SXFALSE;
    }

    return SXTRUE;
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


static SXBOOLEAN
OR_CHANGED (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));
    register SXBA_ELT	val1, val2;
    SXBOOLEAN		has_changed = SXFALSE;

#if EBUG
    if (*lhs_bits_array < *rhs_bits_array)
	sxtrap (ME, "OR");
#endif
    
    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	val1 = *lhs_bits_ptr;
	val2 = val1 | *rhs_bits_ptr--;

	if (val1 != val2)
	{
	    has_changed = SXTRUE;
	    *lhs_bits_ptr = val2;
	}

	lhs_bits_ptr--;
    }

    return has_changed;
}


static void
OR_MINUS (bits_array1, bits_array2, bits_array3)
    SXBA	bits_array1, bits_array2, bits_array3;
{
    /* bits_array1 or (bits_array2 - bits_array3) */
    register SXBA	bits_ptr1, bits_ptr2, bits_ptr3;
    register int	slices_number = NBLONGS (BASIZE (bits_array2));

#if EBUG
    if (*bits_array1 < *bits_array2 || *bits_array3 < *bits_array2)
	sxtrap (ME, "OR_MINUS");
#endif
   
    bits_ptr1 = bits_array1 + slices_number, bits_ptr2 = bits_array2 + slices_number, bits_ptr3 = bits_array3 + slices_number;

    while (slices_number-- > 0)
    {
	*bits_ptr1-- |= (*bits_ptr2-- & (~(*bits_ptr3--)));
    }
}

static SXBOOLEAN
OR_AND (bits_array1, bits_array2, bits_array3)
    SXBA	bits_array1, bits_array2, bits_array3;
{
    /* bits_array1 or (bits_array2 - bits_array3) */
    register SXBA	bits_ptr1, bits_ptr2, bits_ptr3;
    register int	slices_number = NBLONGS (BASIZE (bits_array2));
    register SXBA_ELT	val1, val2;
    SXBOOLEAN		has_changed = SXFALSE;

    bits_ptr1 = bits_array1 + slices_number, bits_ptr2 = bits_array2 + slices_number, bits_ptr3 = bits_array3 + slices_number;

    while (slices_number-- > 0)
    {
	if ((val2 = ((*bits_ptr2--) & (*bits_ptr3--))) != 0L)
	{
	    val1 = *bits_ptr1;
	    val2 |= val1;

	    if (val1 != val2)
	    {
		*bits_ptr1 = val2;
		has_changed = SXTRUE;
	    }
	}

	bits_ptr1--;
    }

    return has_changed;
}


static SXBOOLEAN
OR_AND_MINUS (bits_array1, bits_array2, bits_array3, bits_array4)
    SXBA	bits_array1, bits_array2, bits_array3, bits_array4;
{
    /* bits_array1 or (bits_array2 - bits_array3) */
    register SXBA	bits_ptr1, bits_ptr2, bits_ptr3, bits_ptr4;
    register int	slices_number = NBLONGS (BASIZE (bits_array2));
    register SXBA_ELT	val;
    SXBOOLEAN		is_set = SXFALSE;

    bits_ptr1 = bits_array1 + slices_number, bits_ptr2 = bits_array2 + slices_number, bits_ptr3 = bits_array3 + slices_number, bits_ptr4 = bits_array4 + slices_number;

    while (slices_number-- > 0)
    {
	if ((val = (*bits_ptr2--) & (*bits_ptr3--) & (~(*bits_ptr4--))) != 0L)
	    is_set = SXTRUE;

	*bits_ptr1-- |= val;
    }

    return is_set;
}


static void
OR_RAZ (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));

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
COPY_RAZ (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    /* *lhs_bits_array <= *rhs_bits_array */
    
    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	*lhs_bits_ptr-- = *rhs_bits_ptr;
	*rhs_bits_ptr-- = 0L;
    }
}

static SXBOOLEAN
AND (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));
    register int	lhs_slices_number = NBLONGS (BASIZE (lhs_bits_array));
    SXBOOLEAN		ret_val = SXFALSE;

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
	    ret_val = SXTRUE;
    }

    return ret_val;
}


static SXBOOLEAN
AND3 (lhs_bits_array, op1_bits_array, op2_bits_array)
    SXBA	lhs_bits_array, op1_bits_array, op2_bits_array;
{
    /* On suppose que lhs_bits_array est vide au-dela de op2_bits_array */

    register SXBA	lhs_bits_ptr, op1_bits_ptr, op2_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (op2_bits_array));
    SXBOOLEAN		ret_val = SXFALSE;

#if EBUG
    if (*op1_bits_array < *op2_bits_array)
	sxtrap (ME, "AND3");
#endif
    
    lhs_bits_ptr = lhs_bits_array + slices_number,
    op1_bits_ptr = op1_bits_array + slices_number,
    op2_bits_ptr = op2_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- = (*op1_bits_ptr-- & *op2_bits_ptr--))
	    ret_val = SXTRUE;
    }

    return ret_val;
}
AND3_SHIFT (lhs_bits_array, op1_bits_array, op2_bits_array, shift)
    SXBA	lhs_bits_array, op1_bits_array, op2_bits_array;
{
    /* Identique a AND3 excepte' que op2_bits_array est decale a droite de shift */
    /* Shift > 0 */

    register SXBA	lhs_bits_ptr, op1_bits_ptr, op2_bits_ptr;
    int			delta = DIV (shift);
    register int	slices_number = NBLONGS (BASIZE (op2_bits_array)) - delta;
    int			right_shift = MOD (shift);
    int			left_shift = SXBITS_PER_LONG - right_shift;
    SXBA_ELT		filtre = (~(0L)) >> left_shift;
    SXBA_ELT		op2val = 0L, prev_op2val;
    SXBOOLEAN		ret_val = SXFALSE;

#if EBUG
    if (*op1_bits_array < *op2_bits_array)
	sxtrap (ME, "AND3");
#endif
    
    lhs_bits_ptr = lhs_bits_array + slices_number,
    op1_bits_ptr = op1_bits_array + slices_number,
    op2_bits_ptr = op2_bits_array + (slices_number + delta);

    while (slices_number-- > 0)
    {
	prev_op2val = op2val;
	op2val = *op2_bits_ptr--;

	if (*lhs_bits_ptr-- = (*op1_bits_ptr-- & ((op2val >> right_shift) | ((prev_op2val & filtre) << left_shift))))
	    ret_val = SXTRUE;
    }

    return ret_val;
}


static SXBOOLEAN
MINUS (lhs_bits_array, rhs_bits_array, l)
    SXBA	lhs_bits_array, rhs_bits_array;
    int		l;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (l);
    SXBOOLEAN		ret_val = SXFALSE;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- &= ~(*rhs_bits_ptr--))
	    ret_val = SXTRUE;
    }

    return ret_val;
}



static SXBOOLEAN
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
	    return SXTRUE;
    }

    return SXFALSE;
}

#if 0
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


static SXBOOLEAN
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
	    return SXTRUE;
    }

    return SXFALSE;
}


static SXBOOLEAN
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
	    return SXFALSE;
    }

    return SXTRUE;
}
#endif

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
    





static void
reduce (A, j, i)
    int A, j, i;
{
    /* Toutes les reductions vers Aj ont ete effectuees
       Cas du reconnaisseur : on fait le "shift" sur A dans la liste "j".
       cas du parser : en plus, on fait les reductions eventuelles B -> alpha Aji */
    int		state, new_state, B, Y, prdct_no, hd;
    SXBA	index_set, index_set2, backward_index_set;
    struct recognize_item	*RTj;

    if ((prdct_no = constraints [A]) != -1 &&
	for_parsact.constraint != NULL &&
	!for_parsact.constraint (prdct_no, A, j, i))
	return;

    RTj = &(RT [j]);

    for (state = RTj->shift_NT_hd [A]; state > 0; state = RTj->shift_state_stack [state])
    {
	/* state = B -> \beta . A \gamma */
	new_state = state+1;

	index_set = RTj->index_sets [state];

	if ((Y = lispro [new_state]) == 0)
	{
	    /* B -> beta A . */
	    B = lhs [prolis [new_state]];

	    if (is_parser)
	    {
		/* On ne stocke pas les reduces ds le cas du reconnaisseur. */
		if ((index_set2 = T2_index_sets [new_state]) == NULL)
		{
		    T2_shift_state_stack [new_state] = T2_reduce_NT_hd [B];
		    T2_reduce_NT_hd [B] = new_state;
		    index_set2 = T2_index_sets [new_state] = bag_get (&shift_bag, i+1);
		    backward_index_set = T2_backward_index_sets [new_state] = bag_get (&shift_bag, i+1);
		}
		else
		{
		    backward_index_set = T2_backward_index_sets [new_state];
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
		    hd = T2_shift_NT_hd [Y];
		    
		    if (Y > 0 && hd == 0)
			PUSH (T2_shift_NT_stack, Y);

		    if ((index_set2 = T2_index_sets [new_state]) == NULL)
		    {
			T2_shift_state_stack [new_state] = hd;
			T2_shift_NT_hd [Y] = new_state;
			index_set2 = T2_index_sets [new_state] = bag_get (&shift_bag, i+1);

			if (is_parser)
			    backward_index_set = T2_backward_index_sets [new_state] = bag_get (&shift_bag, i+1);
		    }
		    else
		    {
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

static void
scan_reduce (i)
    register int 	i;
{
    register SXBA_ELT	filtre;
    register int	j, indice, order, A;

    int			state;
    SXBA		index_set, index_set2;

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
}


static SXBOOLEAN
complete (i)
    int i;
{
    int		hd, state, next_state, X, Y, A, B, j, prdct_no, prod, Aik, start, end;
    SXBA	index_set, backward_index_set;
    SXBOOLEAN	is_tok, is_scan_reduce = SXFALSE;

    /* Le look-ahead est verifie pour tous les state de T1_state_set. */
    /* De plus si A -> \alpha X . ai+1 \beta, le predicat de ai+1 est verifie'
       ainsi que FIRST1[\beta]. */

    if ((state = T1_shift_NT_hd [0]) != 0)
    {
	/* A -> \alpha X . ai+1 \beta */
	is_tok = SXTRUE;
	T1_shift_NT_hd [0] = 0;

	do
	{
	    next_state = state + 1;

	    Y = lispro [next_state];
	    index_set = T1_index_sets [state];

	    if (Y != 0)
	    {
		if (Y < 0)
		{
		    /* A -> \alpha X . ai+1 ai+2 \beta' */
		    if (((prdct_no = prdct [next_state]) == -1 ||
			 for_parsact.prdct == NULL ||
			 (*for_parsact.prdct) (-i-2, prdct_no)))
			Y = 0;
		}

		if (Y >= 0)
		{
		    hd = T2_shift_NT_hd [Y];

		    if (Y > 0 && hd == 0)
			PUSH (T2_shift_NT_stack, Y);

		    T2_shift_NT_hd [Y] = next_state;
		    T2_shift_state_stack [next_state] = hd;
		    T2_index_sets [next_state] = index_set;
		}
	    }
	    else
	    {
		/* A -> \alpha X . ai+1 */
		is_scan_reduce = SXTRUE;

		A = lhs [prolis [state]];
		OR (ntXindex_set [A], index_set);

		if (is_parser)
		{
		    /* index_set et backward_index_set == NULL */
		    T2_shift_state_stack [next_state] = T2_reduce_NT_hd [A];
		    T2_reduce_NT_hd [A] = next_state;
		}
	    }
	} while ((state = T1_shift_state_stack [state]) != 0);
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
		T1_shift_state_stack [state] = T1_shift_NT_hd [X];
		T1_shift_NT_hd [X] = state;
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
			if (Y < 0)
			{
			    if (((prdct_no = prdct [state]) == -1 ||
				 for_parsact.prdct == NULL ||
				 (*for_parsact.prdct) (-i-2, prdct_no)))
				/* A -> \alpha X . ai+1 ai+2 \beta' */
				Y = 0;
			}

			if (Y >= 0)
			{
			    hd = T2_shift_NT_hd [Y];

			    if (Y > 0 && hd == 0)
				PUSH (T2_shift_NT_stack, Y);

			    T2_shift_NT_hd [Y] = next_state;
			    T2_shift_state_stack [next_state] = hd;
			    index_set = T2_index_sets [next_state] = bag_get (&shift_bag, i+1);
			    SXBA_1_bit (index_set, i);
			}
		    }
		    else
		    {
			/* A -> . ai+1, i */
			is_scan_reduce = SXTRUE;

			A = lhs [prolis [state]];
			SXBA_1_bit (ntXindex_set [A], i);

			if (is_parser)
			{
			    T2_shift_state_stack [next_state] = T2_reduce_NT_hd [A];
			    T2_reduce_NT_hd [A] = next_state;
			}
		    }
		}
	    }
	}
    }

    if (is_scan_reduce)
	scan_reduce (i);

    return is_tok;
}




static int
recognize ()
{
    int			i;
    int			*T0_shift_NT_stack;
    SXBOOLEAN		is_in_LG;

    /* initial_state ne vaut pas toujours 1 (cas ou L(G)={epsilon}). */
    T1_shift_NT_hd  = &(RT [0].shift_NT_hd [0]);
    T2_shift_NT_hd  = &(RT [1].shift_NT_hd [0]);

    T1_shift_state_stack = &(RT [0].shift_state_stack [0]);
    T2_shift_state_stack = &(RT [1].shift_state_stack [0]);

    T1_index_sets = &(RT [0].index_sets [0]);
    T2_index_sets = &(RT [1].index_sets [0]);

    T1_shift_NT_stack = shift_NT_stack_1;
    T2_shift_NT_stack = shift_NT_stack_2;

    if (is_parser)
    {
	T2_backward_index_sets = &(PT [1].backward_index_sets [0]);
	T2_reduce_NT_hd = &(PT [1].reduce_NT_hd [0]);
    }

    PUSH (T1_shift_NT_stack, 1 /* start symbol */);
    T1_shift_NT_hd [1] = initial_state;
    /* T1_shift_state_stack [initial_state] = 0; pas de suivant */

    if (SXBA_bit_is_set (FIRST_1[-TOK [1]], initial_state))
    {
	i = 0;

	for (;;)
	{
	    if (!complete (i)) break;

	    if (++i == n)
		break;

	    T1_shift_NT_hd = T2_shift_NT_hd;
	    T1_shift_state_stack = T2_shift_state_stack;
	    T1_index_sets = T2_index_sets;
	    T2_shift_NT_hd  = &(RT [i+1].shift_NT_hd [0]);
	    T2_shift_state_stack = &(RT [i+1].shift_state_stack [0]);
	    T2_index_sets = &(RT [i+1].index_sets [0]);

	    T0_shift_NT_stack = T1_shift_NT_stack,
	    T1_shift_NT_stack = T2_shift_NT_stack,
	    T2_shift_NT_stack = T0_shift_NT_stack;

	    if (is_parser)
	    {
		T2_backward_index_sets = &(PT [i+1].backward_index_sets [0]);
		T2_reduce_NT_hd = &(PT [i+1].reduce_NT_hd [0]);
	    }
	}
    }

    is_in_LG = T2_index_sets [2] != NULL && SXBA_bit_is_set (T2_index_sets [2], 0);
    
    return is_in_LG ? n+1 : i+1;
}




/******************************************************************************************

                             P A R S E R


 ******************************************************************************************/





#if 0
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
#endif

static void
ARN_sem_init (size)
    int size;
{
#if EBUG
    Aij_rhs_set = sxba_calloc (size);
    Aij_lhs_set = sxba_calloc (size);
#endif

    Aij_sem_lhs_set = sxba_calloc (size);
    ARN_sem = (struct ARN_sem*) sxalloc (size, sizeof (struct ARN_sem));
}


static void
ARN_sem_final (size)
    int size;
{
#if EBUG
    sxfree (Aij_rhs_set);
    sxfree (Aij_lhs_set);
#endif

    sxfree (Aij_sem_lhs_set);
    sxfree (ARN_sem);
}


static SXBOOLEAN
ARN_constraint (prdct_no, A, i, j)
    int A, prdct_no, i, j;
{
    if (prdct_no == 1)
    {
	return j - i <= 30;
    }
    else
    {
	fprintf (sxstderr, "The function \"ARN_constraint\" is out of date with respect to its specification.\n");
	abort ();
    }
}


static SXBOOLEAN
RL_mreduction_item (item_j, I, j)
    int 	item_j, j;
    SXBA	I;
{
    /* item_j = A -> \alpha . \gamma  et \alpha != \epsilon */
    int				Y, bot_item, i, k, Z, new_k, nbt, new_item, item_k, item, order;
    SXBA			backward_index_set, prev_index_set;
    SXBA			/* tbp_set, */ ap_set;
    struct recognize_item	*RTj;
    struct parse_item		*PTj;
    SXBOOLEAN			is_tbp, is_new;

    SXBA			ids, nbis;

    bot_item = prolon [prolis [item_j]];

    RTj = &(RT [j]);
    PTj = &(PT [j]);

    backward_index_set = PTj->backward_index_sets [item_j];
	    
    item_k = item_j - 1;
    Y = lispro [item_k];
    /* item_k = X -> \alpha . Y x \gamma */

#if EBUG
    if (Y <= 0)
	sxtrap (ME, "RL_mreduction_item");
#endif

    ap_set = ap_sets [Y];

    if (is_new = SXBA_bit_is_reset_set (RL_nt_set, Y))
    {
	ap_set [0] = j+1; /* On change la taille!! */

	for (item = PTj->reduce_NT_hd [Y]; item > 0; item = RTj->shift_state_stack [item])
	{
	    order = prod2order [prolis [item]];
	    SXBA_1_bit (prod_order_set, order);
	}
    }

    if (item_k == bot_item)
    {
	/* \alpha = \epsilon */

	is_tbp = OR_CHANGED (ap_set, I); /* ap_set a t'il change' ? */
	
	/* item_k = X -> . Y \gamma */
	PT2 [j].backward_index_sets [item_j] = I;
    }
    else
    {
	is_tbp = SXFALSE;
	nbt = item2nbt [item_k];
	new_item = item_k - nbt;

	nbis = PT2 [j].backward_index_sets [item_j];

	k = -1;

	while ((k = sxba_scan (backward_index_set, k)) >= 0)
	{
	    prev_index_set = RT [k].index_sets [item_k];
#if EBUG
	    if (prev_index_set == NULL)
		sxtrap (ME, "RL_mreduction_item");
#endif
	    if (IS_AND (prev_index_set, I))
	    {
		/* C'est un bon k */
		SXBA_0_bit (backward_index_set, k); /* inutile de le reessayer + tard, il a marche */

		if (nbis == NULL)
		    nbis = PT2 [j].backward_index_sets [item_j] = bag_get (&pt2_bag, j+1);

		SXBA_1_bit (nbis, k);

		if ((is_new || !SXBA_bit_is_set (ap_set, k)))
		{
		    SXBA_1_bit (ap_set, k);
		    is_tbp = SXTRUE;
		}

		if (new_item > bot_item)
		{
		    /* new_item pointe derriere un nt. */
		    new_k = k-nbt;

		    if ((ids = PT2 [new_k].index_sets [new_item]) == NULL)
		    {
			ids = PT2 [new_k].index_sets [new_item] = bag_get (&pt2_bag, k+1);
			PUSH (PT2 [new_k].shift_NT_stack, new_item);
		    }

		    OR_AND (ids, prev_index_set, I);
		}
	    }
	}
    }
	
    return is_tbp;
}

static void
RL_mreduction ()
{
    int				i, j, A, order, tnb;
    SXBA			ap_set, reduce_set;
    struct parse_item		*PTj;
    struct recognize_item	*RTj;
    struct PT2_item		*PT2j;

    int				*PT2j_shift_NT_stack, x, item, new_item, new_j, prod, X;
    SXBA			*PT2j_index_set, *PT2j_backward_index_set, I, B, ids, nbis;
    SXBA			index_set, backward_index_set, new_index_set, red_order_set;
    SXBOOLEAN			is_good;

    RL_nt_set = sxba_calloc (ntmax+1);
    prod_order_set = sxba_calloc (prodmax+1);

    index_set = PT2 [n].index_sets [2] = bag_get (&pt2_bag, n+1);
    SXBA_1_bit (index_set, 0); /* {0} */

#if EBUG
    if (sxba_scan (T2_index_sets [2], 0) > 0)
	sxtrap (ME, "RL_mreduction");
#endif

    PUSH (PT2 [n].shift_NT_stack, 2); /* S' -> S . $ */

    for (j = n; j >= 0; j--)
    {
	PT2j = &(PT2 [j]);
	PTj = &(PT [j]);
	RTj = &(RT [j]);

	PT2j_index_set = &(PT2j->index_sets [0]);
	PT2j_backward_index_set = &(PT2j->backward_index_sets [0]);
	PT2j_shift_NT_stack = &(PT2j->shift_NT_stack [0]);

	for (x = PT2j_shift_NT_stack [0]; x > 0; x--)
	{
	    item = PT2j_shift_NT_stack [x];
	    /* item = B -> \alpha A . X \beta */
	    I = PT2j_index_set [item];
#if EBUG
	    if (I == NULL || PT2j_backward_index_set [item] != NULL)
		sxtrap (ME, "RL_mreduction");
#endif

	    RL_mreduction_item (item, I, j);
	}

	red_order_set = red_order_sets [j];

	/* prod_order_set est positionne par RL_mreduction_item par le traitement des shifts. */
	order = 0;

	while ((order = sxba_scan (prod_order_set, order)) > 0)
	{
	    prod = order2prod [order];
	    item = prolon [prod+1]-1;
	    A = lhs [prod];
	    tnb = item2nbt [item];
	    ap_set = ap_sets [A];

	    is_good = SXFALSE;
				
	    if (tnb == 0)
	    {
		/* item = A -> \alpha B . */
		index_set = RTj->index_sets [item];

		if (IS_AND (index_set, ap_set))
		{
		    if ((new_index_set = PT2j->index_sets [item]) == NULL)
			PT2j->index_sets [item] = new_index_set = bag_get (&pt2_bag, j+1);

		    if (OR_AND (new_index_set, index_set, ap_set))
		    {
			/* Il y a du nouveau */
			if (RL_mreduction_item (item, new_index_set, j))
			{
			    /* ap_sets [B] a ete modifie */
			    /* rupture de sequence (eventuel) calcule statiquement */
			    order = order2next [order];
			}
			/* else order est pris en sequence */

			is_good = SXTRUE;
		    }
		}
	    }
	    else
	    {
		new_j = j - tnb;

		if (prolon [prod] == (new_item = item - tnb))
		{
		    /* Que des t en rhs */
		    if (SXBA_bit_is_set (ap_set, new_j))
			is_good = SXTRUE;
		}
		else
		{
		    /* new_item = A -> \alpha B . t1 ... tnbt */
		    index_set = RT [new_j].index_sets [new_item];

		    if (IS_AND (index_set, ap_set))
		    {
			if ((ids = PT2 [new_j].index_sets [new_item]) == NULL)
			{
			    ids = PT2 [new_j].index_sets [new_item] = bag_get (&pt2_bag, new_j+1);
			    PUSH (PT2 [new_j].shift_NT_stack, new_item);
			}

			if (OR_AND (ids, index_set, ap_set))
			    is_good = SXTRUE;
		    }
		}
	    }

	    if (is_good)
	    {
	      x = prod2order [prod];
	      SXBA_1_bit (red_order_set, x);
	    }
	}

	A = 0;

	while ((A = sxba_scan_reset (RL_nt_set, A)) > 0)
	    sxba_empty (ap_sets [A]);

	sxba_empty (prod_order_set);
    }
    
    sxfree (RL_nt_set);
    sxfree (prod_order_set);
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

static void
print_prod (prod, i, k, rhs_stack)
    int 	prod, i, k, *rhs_stack;
{
static int	indexes_stack [rhs_lgth+1];
    int		h, B, j, x, A, item, Bjh, B0h;

    A = lhs [prod];
    xprod++;
    printf ("%i\t: ", xprod);
    print_symb (A, i, k);
    fputs ("\t = ", stdout);

    x = prod2nbnt [prod];

    if (x > 0)
    {
	/* On calcule les j intermediaires a partir de k */
	item = prolon [prod+1] - 1;
	h = k;

	do
	{
	    while ((B = lispro [--item]) < 0)
		h--;

	    Bjh = rhs_stack [x];
	    B0h = MAKE_A0j (B, h);
	    j = Bjh - B0h;
	    indexes_stack [x] = h;
	    h = j;
	} while (--x > 0);
    }

    x = 1;
    j = i;
    item = prolon [prod];

    while ((B = lispro [item++]) != 0)
    {
	h = (B > 0) ? indexes_stack [x++] : j+1;
	print_symb (B, j, h);
	j = h;
    }
    
#if EBUG
    if (j != k)
	sxtrap (ME, "print_prod");
#endif

    fputs (";\n", stdout);

}

static void
ARN_ksem_init (size)
    int size;
{
    int	x;

#if EBUG
    Aij_rhs_set = sxba_calloc (size);
    Aij_lhs_set = sxba_calloc (size);
#endif

    Aij_sem_lhs_set = sxba_calloc (size);
    ARN_disp_ksem = (struct ARN_ksem**) sxalloc (size, sizeof (struct ARN_ksem*));
    ARN_ksem_top = ARN_ksem = (struct ARN_ksem*) sxalloc (max_tree_nb*size, sizeof (struct ARN_ksem));

    disp_kvals = (struct kvals**) sxalloc (max_tree_nb, sizeof (struct kvals*));
    kvals = (struct kvals*) sxalloc (max_tree_nb, sizeof (struct kvals));

    for (x = 0; x < max_tree_nb; x++)
	disp_kvals [x] = &(kvals [x]);
}


static void
ARN_ksem_final (size)
    int size;
{
#if EBUG
    sxfree (Aij_rhs_set);
    sxfree (Aij_lhs_set);
#endif

    sxfree (Aij_sem_lhs_set);
    sxfree (ARN_disp_ksem);
    sxfree (ARN_ksem);
    sxfree (disp_kvals);
    sxfree (kvals);
}


static int	ksem_vector [rhs_maxnt+1];
static SXBOOLEAN
ksem_incr (nbnt)
    int nbnt;
{
    int x, i;

    for (x = nbnt; x > 0; x--)
    {
	i = ++ksem_vector [x];

	if (i == max_tree_nb || ARN_disp_ksem [rhs_stack [x]][i].val == -1)
	    ksem_vector [x] = 0;
	else
	{
	    ksem_vector [x] = i;
	    return SXTRUE;
	}
    }

    return SXFALSE;
}

static int
ARN_keval (Aij, i, j, rhs_stack)
     int Aij, i, j, rhs_stack [];
{
    /* remplit les [0..top-1] premiers elements de kvals */
    int	top, x, min_x, prod, nbnt, Bhk;
    float val, min_val, cur_val, proba;
    struct ARN_ksem	*psem;

    top = 0;

    prod = rhs_stack [0];
    nbnt = prod2nbnt [prod];
    proba = probabilities [prod] * (j-i);

    if (nbnt == 0)
    {
	kvals [0].val = proba;
	top = 1;
    }
    else
    {
	if (nbnt == 1)
	{
	    Bhk = rhs_stack [nbnt];

	    for (psem = ARN_disp_ksem [Bhk], top = 0;
		 (val = psem->val) != -1 && top < max_tree_nb;
		 psem++, top++)
	    {
		disp_kvals [top]->val = psem->val*proba;
		disp_kvals [top]->vector [1] = top;
	    }
	}
	else
	{
	    top = 0;

	    do
	    {
		val = proba;

		for (x = 1; x <= nbnt; x++)
		    val *= ARN_disp_ksem [rhs_stack [x]][ksem_vector [x]].val;

		/* Si val est mieux, on le place */

		if (top < max_tree_nb)
		{
		    disp_kvals [top]->val = val;

		    for (x = 1; x <= nbnt; x++)
			disp_kvals [top]->vector [x] = ksem_vector [x];

		    top++;
		}
		else
		{
		    /* On recherche la + petite valeur et on la remplace */
		    min_val = -1;

		    for (x = 0; x < max_tree_nb; x++)
		    {
			if ((cur_val = disp_kvals [x]->val) < val)
			{
			    if (min_val == -1 || cur_val < min_val)
			    {
				min_val = cur_val;
				min_x = x;
			    }
			}
		    }

		    if (min_val != -1)
		    {
			disp_kvals [min_x]->val = val;

			for (x = 1; x <= nbnt; x++)
			    disp_kvals [min_x]->vector [x] = ksem_vector [x];
		    }
		}

	    } while (ksem_incr (nbnt));
	}
    }

    return top;
}

static SXBOOLEAN
ARN_kparsact (Aij, i, j, rhs_stack)
    int 	Aij, i, j;
    int		rhs_stack [];
{
    int			nbnt, *p, *q, *r, *s, x, top;
    float		val;
    struct ARN_ksem	*psem, *psem2, *Aij_disp_ksem;
    
    if ((top = ARN_keval (Aij, i, j, rhs_stack)) > 0)
    {
	/* kvals est positionne */
	if (SXBA_bit_is_reset_set (Aij_sem_lhs_set, Aij))
	{
	    /* 1er calcul correct sur Aij en lhs */
	    Aij_disp_ksem = ARN_disp_ksem [Aij] = ARN_ksem_top;

	    for (x = 1; x <= max_tree_nb; x++)
	    {
		ARN_ksem_top->val = -1; /* empty */
		ARN_ksem_top->is_marked = SXFALSE;
		ARN_ksem_top++;
	    }
	}
	else
	    Aij_disp_ksem = ARN_disp_ksem [Aij];
	    
	/* On conserve les max_tree_nb meilleurs! */
	/* On recherche le plus mauvais et on le remplace si tout est complet. */

	while (--top >= 0)
	{
	    val = disp_kvals [top]->val;
	    psem2 = NULL;

	    for (psem = Aij_disp_ksem, x = 0; x < max_tree_nb; psem++, x++)
	    {
		if (psem->val == -1)
		{
		    /* stockage en psem */
		    psem2 = psem;
		    break;
		}

		if (val > psem->val && (psem2 == NULL || psem->val < psem2->val))
		    psem2 = psem;
	    }

	    if (psem2 != NULL)
	    {
		psem2->val = val;
		nbnt = prod2nbnt [rhs_stack [0]];
		p = &(rhs_stack [nbnt]), q = &(psem2->rhs_stack [nbnt]);
		r = &(disp_kvals [top]->vector [nbnt]), s = &(psem2->vector [nbnt]);

		do
		{
		    *q-- = *p--;
		    *s-- = *r--;
		} while (p >= rhs_stack);
	    }
	}

	return SXTRUE;
    }

    return SXFALSE;
}


static void
ksem_td_mark (Aij, x)
    int Aij, x;
{
    int			nbnt;
    struct ARN_ksem	*p;

    p = &(ARN_disp_ksem [Aij][x]);

    if (p->is_marked)
	return;

    p->is_marked = SXTRUE;
    SXBA_1_bit (Aij_set, Aij);

    nbnt = prod2nbnt [p->rhs_stack [0]];

    if (nbnt > 0)
    {
	do
	{
	    ksem_td_mark (p->rhs_stack [nbnt], p->vector [nbnt]);
	} while (--nbnt > 0);
    }
}

static void
ksem_bu_extract ()
{
    /* Extrait les productions valides de la foret partagee. */
    int			i, j, order, A, A0j, Aij, prod, nbnt, x, y, z;
    struct ARN_ksem	*psem, *psem2, *Aij_disp_ksem;

    xprod = 0;

    for (j = 0; j <= n; j++)
    {
	for (order = ntmax; order > 0; order--)
	{
	    A = order2nt [order];
	    A0j = MAKE_A0j (A, j);

	    Aij = MAKE_Aij (A0j, j); /* Ajj */

	    while ((Aij = sxba_1_rlscan (Aij_set, Aij)) >= A0j)
	    {
		/* Il y a des Aij */
		/* Attention, la meme production de la foret partagee peut etre marquee
		   plusieurs fois (avec des semantiques differentes). */
		i = Aij - A0j;
		Aij_disp_ksem = ARN_disp_ksem [Aij];

		for (psem = Aij_disp_ksem, x = 0; x < max_tree_nb; psem++, x++)
		{
		    if (psem->val == -1)
			break;

		    if (psem->is_marked)
		    {
			prod = psem->rhs_stack [0];
			nbnt = prod2nbnt [prod];

			if (is_print_prod)
			    print_prod (prod, i, j, &(psem->rhs_stack [0]));
			else
			    xprod++;

			psem2 = psem;
			y = x;

			while (++y < max_tree_nb)
			{
			    psem2++;

			    if (psem2->val == -1)
				break;

			    if (psem2->is_marked && psem2->rhs_stack [0] == prod)
			    {
				for (z = nbnt; z > 0; z--)
				{
				    if (psem->rhs_stack [z] != psem2->rhs_stack [z])
					break;
				}

				if (z == 0)
				    psem2->is_marked = SXFALSE;
			    }
			}
		    }
		}
	    }
	}
    }
}


static SXBOOLEAN
ARN_eval (Aij, i, j, rhs_stack, val)
     int Aij, i, j, rhs_stack [], *val;
{
  *val = 0;
  return SXTRUE;
}

static SXBOOLEAN
ARN_parsact (Aij, i, j, rhs_stack)
    int 	Aij, i, j;
    int		rhs_stack [];
{
    int		nbnt, val, *p, *q;
    
    if (ARN_eval (Aij, i, j, rhs_stack, &val))
    {
	/* val est positionne */
	if (SXBA_bit_is_reset_set (Aij_sem_lhs_set, Aij) /* 1er calcul correct sur Aij en lhs */ ||
	    ARN_sem [Aij].val > val /* Le calcul courant est meilleur */)
	{
	    /* On stocke val et la rhs_stack */
	    ARN_sem [Aij].val = val;
	    nbnt = prod2nbnt [rhs_stack [0]];
	    p = &(rhs_stack [nbnt]), q = &(ARN_sem [Aij].rhs_stack [nbnt]);

	    do
	    {
		*q-- = *p--;
	    } while (p >= rhs_stack);
	}

	return SXTRUE;
    }

    return SXFALSE;
}


#if 0
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

#endif


static SXBOOLEAN
parse_item (A0k, item, I, j, k, nbnt)
    int 	A0k, item, j, k, nbnt;
    SXBA	I;
{
    /* item = A -> \alpha . B \beta */
       
    int		i, new_i, nbt, B0j, Bij, Aik, prod;
    SXBOOLEAN       ret_val = SXFALSE, is_ok;
    SXBA          backward_J, ap_set, index_set;

    backward_J = PT2 [j].backward_index_sets [item+1];

    nbt = item2nbt [item];
    B0j = MAKE_A0j (lispro [item], j);

    if (nbnt == 1)
    {
	prod = prolis [item];

#if EBUG
	if (prod2nbnt [prod] == 1 && NON_EQUAL (I, PT2 [j].index_sets [item+1]))
	    sxtrap (ME, "parse_item");

	/* La propriete precedente n'est pas vraie si \beta contient (au moins) un nt.
	   Pour le k donne, le traitement RL a pu restreindre les valeurs de i pour Aik
	   alors que pour un autre k' ca marche. Ex:
	   Aik -> Bij Cjk et Ai'k' -> Bi'j Cjk' peuvent etre corrects bien que
	   Ai'k -> Bi'j Cjk est faux car Ai'k est non accessible depuis l'axiome. */
#endif

	if (prod2nbnt [prod] > 1)
	{
	    if (nbt == 0)
		AND3 (level0_index_set, I, backward_J);
	    else
		AND3_SHIFT (level0_index_set, I, backward_J, nbt);

	    index_set = level0_index_set;
	}
	else
	    index_set = I;

	ap_set = ap_sets [lhs [prod]];
	new_i = j;

	while ((new_i = sxba_1_rlscan (index_set, new_i)) >= 0)
	{
	    i = new_i + nbt;

	    Bij = MAKE_Aij (B0j, i);
	    /* Dans tous les cas Bij a une valeur semantique differente de bottom. */
	    /* Cette valeur peut eventuellement encore evoluer ds le cas de grammaires cycliques!.
	       Mais il ne faut pas regenerer les memes productions, seulement reevaluer la sem. */
	    rhs_stack [1] = Bij;

	    Aik = MAKE_Aij (A0k, new_i);
#if EBUG
	    if (!SXBA_bit_is_set (Aij_lhs_set, Bij))
		sxtrap (ME, "parse_item");

	    SXBA_1_bit (Aij_rhs_set, Bij);

	    if (SXBA_bit_is_set (Aij_rhs_set, Aik))
		sxtrap (ME, "parse_item");

	    SXBA_1_bit (Aij_lhs_set, Aik);
#endif

	    if (is_print_prod)
		print_prod (prod, new_i, k, rhs_stack);
	    else
		xprod++;

	    if (for_parsact.action != NULL && parsact [prod] > 0)
		is_ok = (*for_parsact.action) (Aik, new_i, k, rhs_stack);
	    else
		is_ok = SXTRUE;
	    
	    if (is_ok && SXBA_bit_is_reset_set (ap_set, new_i))
		ret_val = SXTRUE;
	}
    }
    else
    {
	item -= nbt;

	i = j;
	
	while ((i = sxba_1_rlscan (backward_J, i)) >= 0)
	{
	    Bij = rhs_stack [nbnt] = MAKE_Aij (B0j, i);

#if EBUG
	    if (!SXBA_bit_is_set (Aij_lhs_set, Bij))
		sxtrap (ME, "parse_item");

	    SXBA_1_bit (Aij_rhs_set, Bij);
#endif

	    if (parse_item (A0k, item-1, I, i - nbt, k, nbnt-1))
		ret_val = SXTRUE;
	}
    }

    return ret_val;		/* ap_sets [A] a ete modifie */
}


static struct store {
  int prod, next;
} *store;

static struct RR {
  int p_hd, nb;
} *RR;

static int *Ai_stack, *Bk_stack;
static SXBA Ai_set; /* if EBUG */
static int RR_size, store_size;


static void
store_oflw ()
{
  store = (struct store*) sxrealloc (store, store_size *= 2, sizeof (struct store));
}

static void
right_recursive (j, init_prod_order, out_prod_order)
     int j, init_prod_order, *out_prod_order;
{
    /* un ensemble de productions mutuellement recursives droites vient d'etre detecte'
       Les productions de la foret partagee sont triees dynamiquement de facon telle
       qu'elles ne sont executees que quand la semantique de la rhs est completement
       connue. */
    int 		prod_order, next_prod_order, p, *ap, prod, item,
                        A, Ai, A0j, Aij, B, Bk, B0j, Bkj, nbnt, nbt, i, k;
    SXBA 		index_set, backward_index_set, red_order_set;
    struct PT2_item	*PT2j;
    SXBOOLEAN 		is_ok, ret_val;

    prod_order = init_prod_order;
    red_order_set = red_order_sets [j];

    PT2j = &(PT2 [j]);
    p = 0;

    do
    {
	if (SXBA_bit_is_set (red_order_set, prod_order))
	{
	    /* on parcourt le cycle */
	    prod = order2prod [prod_order];
	    item = prolon [prod+1]-1;
	    B = lispro [item-1];
	    A = lhs [prod];
	    nbnt = prod2nbnt [prod];
	    backward_index_set = PT2j->backward_index_sets [item];
	    nbt = item2nbt [item-1];
	  
	    k = -1;

	    while ((k = sxba_scan (backward_index_set, k)) >= 0)
	    {
		if (++p >= store_size)
		    store_oflw();

		store [p].prod = prod;

		Bk = MAKE_Ai (B,k);
#if EBUG
		SXBA_1_bit (Ai_set, Bk);
#endif

		if ((store [p].next = RR [Bk].p_hd) == 0)
		{
		    /* C'est la 1ere fois qu'on trouve Bk en rhs */
		    PUSH (Bk_stack, Bk);
		}

		RR [Bk].p_hd = p;

		if (nbnt == 1)
		{
		    Ai = MAKE_Ai (A,k-nbt);
#if EBUG
		    SXBA_1_bit (Ai_set, Ai);
#endif
		    RR [Ai].nb++;
		}
		else
		{
		    /* l'inclusion n'est pas assuree k etant une union pour differents j du fait
		       de l'accessibilite depuis l'axiome. */
		    AND3 (level0_index_set,
			  PT2j->index_sets [item],
			  PT2 [k-nbt].index_sets [item-1-nbt]);

		    i = k-nbt;

		    while ((i = sxba_1_rlscan (level0_index_set, i)) >= 0)
		    {
			Ai = MAKE_Ai (A,i);
#if EBUG
			SXBA_1_bit (Ai_set, Ai);
#endif
			RR [Ai].nb++;
		    }
		}
	    }
	}

	next_prod_order = parse_order2next [prod_order];

	if (next_prod_order > prod_order)
	    /* Sortie de la boucle */
	    *out_prod_order = prod_order;

	prod_order = next_prod_order-1;
    } while (prod_order != init_prod_order);

    while (!IS_EMPTY (Bk_stack))
    {
	Bk = POP (Bk_stack);

	if (RR [Bk].nb == 0)
	    PUSH (Ai_stack, Bk);
    }

    while(!IS_EMPTY (Ai_stack))
    {
	Bk = POP (Ai_stack);
	k = MAKE_Ai2i (Bk);
	B = MAKE_Ai2A (Bk);
	B0j = MAKE_A0j (B, j);
	Bkj = MAKE_Aij (B0j, k);

	for (p = *(ap = &(RR [Bk].p_hd)); *ap = 0, p != 0; p = *(ap = &(store [p].next)))
	{
	    prod = store [p].prod;

	    nbnt = prod2nbnt [prod];
	    item = prolon [prod+1]-1;
	    nbt = item2nbt [item-1];
	    i = k - nbt;
	    A = lhs [prod];
	    A0j = MAKE_A0j (A, j);
	    rhs_stack [nbnt] = Bkj;

#if EBUG
	    if (!SXBA_bit_is_set (Aij_lhs_set, Bkj))
		sxtrap (ME, "parse_item");

	    SXBA_1_bit (Aij_rhs_set, Bkj);
#endif

	    if (nbnt == 1)
	    {
		/* faire l'appel direct */
		Aij = MAKE_Aij (A0j, i);
#if EBUG
		if (SXBA_bit_is_set (Aij_rhs_set, Aij))
		    sxtrap (ME, "right_recursive");

		SXBA_1_bit (Aij_lhs_set, Aij);
#endif
	      
		if (is_print_prod)
		    print_prod (prod, i, j, rhs_stack);
		else
		    xprod++;

		if (for_parsact.action != NULL && parsact [prod] > 0)
		    is_ok = (*for_parsact.action) (Aij, i, j, rhs_stack);
		else
		    is_ok = SXTRUE;
	    
		if (is_ok && SXBA_bit_is_reset_set (ap_sets [A], i))
		    ret_val = SXTRUE;

		Ai = MAKE_Ai (A, i);

		if (--RR [Ai].nb == 0)
		    PUSH (Ai_stack, Ai);
	    }
	    else
	    {
		/* faire l'appel direct backward = {k} */
		item -= nbt;

		AND3 (level0_index_set,
		      PT2j->index_sets [item],
		      PT2 [i].index_sets [item-1-nbt]);

		if (parse_item (A0j, item-2-nbt, level0_index_set, i, j, nbnt-1))
		    ret_val = SXTRUE;

		while ((i = sxba_1_rlscan (level0_index_set, i)) >= 0)
		{
		    Ai = MAKE_Ai (A,i);

		    if (--RR [Ai].nb == 0)
			PUSH (Ai_stack, Ai);
		}
	    }
	}
    }

#if EBUG
    Ai = -1;

    while((Ai = sxba_scan_reset (Ai_set, Ai)) >= 0)
    {
	if (RR [Ai].nb != 0 || RR [Ai].p_hd != 0)
	    sxtrap (ME, "right_recursive");
    }
#endif
}


static void
parse ()
{
  SXBA                  working_backward_index_set;
  struct PT2_item	*PT2j;
  int                   *PT2j_shift_NT_stack;
  SXBA	                backward_index_set, index_set, red_order_set;
  int		        item, new_item, new_j, j, prod_order, A, A0j, Aij, B, nbt, nbnt, prod, x;
  SXBOOLEAN               is_ok;

  /* On genere toutes les reductions finissant en j */
  
  working_backward_index_set = sxba_calloc (n+1);

    for (j = 0; j <= n; j++)
    {
	for (A = 1; A <= ntmax; A++)
	{
	    MAKE_A0j (A, j) = A0j2Aij_top;
	    A0j2Aij_top += j+1;
	}

        working_backward_index_set [0] = j+1; /* On change la taille ! */
	PT2j = &(PT2 [j]);
	red_order_set = red_order_sets [j];
	/* red_order_set est positionne par le traitement des PT2. */
	/* il contient les A -> \alpha t. */
	prod_order = prodmax+1;

	while ((prod_order = sxba_1_rlscan (red_order_set, prod_order)) > 0)
	{
	  /* Examine' du plus grand vers le + petit. */
	    rhs_stack [0] = prod = order2prod [prod_order];
	    item = prolon [prod+1]-1;
	    A = lhs [prod];
	    ap_sets [A] [0] = j+1; /* On change la taille! */
	    nbt = item2nbt [item];
	    nbnt = prod2nbnt [prod];
	    A0j = MAKE_A0j (A, j);

	    if (nbt == 0)
	    {
		/* item = A -> \alpha B . */
	      if (SXBA_bit_is_set (is_right_recursive_prod, prod))
		{
		  right_recursive (j, prod_order, &prod_order);
		}
	      else
		{
		  B = lispro [item-1];

		  if (AND (PT2j->backward_index_sets [item], ap_sets [B]))
		    {
		      if (parse_item (A0j, item-1, PT2j->index_sets [item], j, j, nbnt))
			/* ap_sets [A] a ete modifie */
			/* rupture de sequence (eventuel) calcule statiquement */
			prod_order = parse_order2next [prod_order];
		    }
		}
	    }
	    else
	    {
		new_j = j - nbt;

		if (nbnt == 0)
		  {
		    /* Que des t en rhs */
		    Aij = MAKE_Aij (A0j, new_j);

#if EBUG
		    if (SXBA_bit_is_set (Aij_rhs_set, Aij))
		      sxtrap (ME, "parse");

		    SXBA_1_bit (Aij_lhs_set, Aij);
#endif

		    if (is_print_prod)
		      print_prod (prod, new_j, j, rhs_stack);
		    else
		      xprod++;

		    if (for_parsact.action != NULL && parsact [prod] > 0)
		      is_ok = (*for_parsact.action) (Aij, new_j, j, rhs_stack);
		    else
		      is_ok = SXTRUE;

		    if (is_ok)
		      SXBA_1_bit (ap_sets [A], new_j);
		  }
		else
		{
		    /* new_item = A -> \alpha B . t1 ... tnbt */
		    new_item = item - nbt;
		    index_set = PT2 [new_j].index_sets [new_item];
		    parse_item (A0j, new_item - 1, index_set, new_j, j, nbnt);
		    /* ap_sets [A] est positionne' */
		}
	    }
	}

	/* Il faut repercuter les ap_sets sur les backwards des shifts. */
	PT2j_shift_NT_stack = &(PT2j->shift_NT_stack [0]);

	for (x = PT2j_shift_NT_stack [0]; x > 0; x--)
	{
	    item = PT2j_shift_NT_stack [x];
	    /* item = B -> \alpha A . X \beta */
	    AND (PT2j->backward_index_sets [item], ap_sets [lispro [item-1]]);
	}
    }

    sxfree (working_backward_index_set);
}



static int	*left_stack, *right_stack, *right_stack2;

static void
sem_mark_extract (Aik, i, k)
    int Aik, i, k;
{
    /* On genere recursivement toutes les productions dont Aik est la partie gauche. */
    int	*rhs_stack;
    int prod, x, nbnt, item, h, j, Bjh, B0h, A, B, bot;

    rhs_stack = &(ARN_sem [Aik].rhs_stack [0]);
    prod = rhs_stack [0];
    x = nbnt = prod2nbnt [prod];

    if (nbnt > 0)
    {
	/* On calcule les j intermediaires a partir de k */
	item = prolon [prod+1] - 1;
	h = k;

	do
	{
	    while ((B = lispro [--item]) < 0)
		h--;

	    Bjh = rhs_stack [x];
	    B0h = MAKE_A0j (B, h);
	    j = Bjh - B0h;
	    PUSH (right_stack, h);
	    PUSH (left_stack, j);
	    h = j;
	} while (--x > 0);

	if (is_print_prod)
	    bot = right_stack2 [0];

	for (x = 1; x <= nbnt; x++)
	{
	    h = POP (right_stack);
	    sem_mark_extract (rhs_stack [x], POP (left_stack), h);

	    if (is_print_prod)
		PUSH (right_stack2, h);
	}
    }

    xprod++;

    if (is_print_prod)
    {
	A = lhs [prod];
	printf ("%i\t: ", xprod);
	print_symb (A, i, k);
	fputs ("\t = ", stdout);

	if (nbnt > 0)
	    right_stack2 [0] = bot;

	j = i;
	item = prolon [prod];

	while ((B = lispro [item++]) != 0)
	{
	    h = (B > 0) ? right_stack2 [++bot] : j+1;
	    print_symb (B, j, h);
	    j = h;
	}
    
	fputs (";\n", stdout);
    }
}

static double	*spf_count;

static void
spf_count_init (size)
    int size;
{
    spf_count = (double*) sxcalloc (size, sizeof (double));

#if EBUG
    Aij_rhs_set = sxba_calloc (size);
    Aij_lhs_set = sxba_calloc (size);
#endif
}


static void
spf_count_final (size)
    int size;
{
    sxfree (spf_count);

#if EBUG
    sxfree (Aij_rhs_set);
    sxfree (Aij_lhs_set);
#endif
}


static SXBOOLEAN
spf_count_parsact (Aij, i, j, rhs_stack)
    int 	Aij, i, j;
    int		rhs_stack [];
{
    double	val;
    int 	nbnt;

    nbnt = prod2nbnt [rhs_stack [0]]; 

    if (nbnt > 0)
    {
	val = spf_count [rhs_stack [nbnt]];

	while (--nbnt > 0)
	    val *= spf_count [rhs_stack [nbnt]];
    }
    else
	val = 1.0;

    spf_count [Aij] += val;

    return SXTRUE;
}


main (argc, argv)
    int		argc;
    char	*argv [];
{
    int 	l, t, *tok, nt, size;
    char	*s, c;

#include <ctype.h>

    if (argc != 3)
    {
	printf (Usage, ME);
	return 1;
    }

    sxopentty ();

    is_parser = SXFALSE;
    is_print_prod = SXFALSE;
    is_default_semantics = SXTRUE;
    max_tree_nb = 1;

    l = 0;

    while ((c = argv [1] [l++]) != '\0')
    {
	if (isdigit (c))
	{
	    max_tree_nb = c - '0';

	    while (isdigit (c = argv [1] [l]))
	    {
		l++;
		max_tree_nb *= 10;
		max_tree_nb += c - '0';
	    }
	}
	else
	{
	    if (isupper (c))
		c = tolower (c);

	    switch (c)
	    {
	    case 'd':
		is_default_semantics = SXTRUE;
		break;

	    case 'p':
		if (!is_parser)
		    is_parser = SXTRUE;
		else
		    is_print_prod = SXTRUE;
		break;

	    case 'r':
		is_parser = SXFALSE;
		break;

	    case 's':
		is_default_semantics = SXFALSE;
		break;

	    default:
		printf (Usage, ME);
		return 1;
	    
	    }
	}
    }

    s = source = argv [2];
    n = strlen (source);

    if (n == 0)
    {
	printf (Usage, ME);
	return 1;
    }

    print_time = SXTRUE;

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

    if (print_time)
	sxtime (TIME_INIT);

    grammar ();

    sprod2order = (int*) sxcalloc (prodmax+1, sizeof (int));

    grammar2 ();

    sprod_set = sxba_calloc (sprodmax+1);
    sprod2attr = (struct sprod2attr*) sxalloc (sprodmax+1, sizeof (struct sprod2attr));

    if (print_time)
	sxtime (TIME_FINAL, "Grammar Processing");

    bag_alloc (&shift_bag, "shift_bag", (n + 1) * prodmax * NBLONGS (n + 1));

    RT = (struct recognize_item*) sxcalloc (n+1, sizeof (struct recognize_item));

    global_state_sets = sxbm_calloc (4, itemmax+1);

    init_state_set = global_state_sets [0];

    ntXindex_set = sxbm_calloc (ntmax+1, n+1);

    if (is_parser)
    {
	PT = (struct parse_item*) sxcalloc (n+1, sizeof (struct parse_item));

	level0_index_set = sxba_calloc (n+1);
    }

    if (is_parser)
    {
	if (is_default_semantics)
	{
	    for_parsact.sem_init = spf_count_init;
	    for_parsact.sem_final = spf_count_final;
	    for_parsact.action = spf_count_parsact;
	    for_parsact.prdct = NULL;
	    for_parsact.constraint = NULL;
	}
	else
	{
	    if (max_tree_nb == 1)
	    {
		for_parsact.sem_init = ARN_sem_init;
		for_parsact.sem_final = ARN_sem_final;
		for_parsact.action = ARN_parsact;
		for_parsact.prdct = NULL;
		for_parsact.constraint = ARN_constraint;
	    }
	    else
	    {
		for_parsact.sem_init = ARN_ksem_init;
		for_parsact.sem_final = ARN_ksem_final;
		for_parsact.action = ARN_kparsact;
		for_parsact.prdct = NULL;
		for_parsact.constraint = ARN_constraint;
	    }
	}
    }

    l = recognize ();

    if (print_time)
	sxtime (TIME_FINAL, "Recognizer");

    if (l <= n)
	printf ("Syntax error at %i\n", l);
    else
    {
	if (is_parser)
	{
	    PT2 = (struct PT2_item*) sxcalloc (n+1, sizeof (struct PT2_item));
	    bag_alloc (&pt2_bag, "pt2_bag", (n + 1) * prodmax * NBLONGS (n + 1));
	    ap_sets = sxbm_calloc (ntmax+1, n+1);
	    red_order_sets = sxbm_calloc (n+1, prodmax+1);

	    RL_mreduction ();

	    bag_free (&shift_bag);
	    sxbm_free (ntXindex_set);

	    if (print_time)
		sxtime (TIME_FINAL, "Reducer");

	    if (for_parsact.action != NULL)
	    {
		int	*p, A;

		logn = sxlast_bit (n);
		filtren = ~((~(0L))<<logn);

		p = (int*) sxalloc (ntmax * (n+1), sizeof (int));

		for (A = 1; A <= ntmax; A++)
		{
		    A2A0j [A] = p;
		    p += n+1;
		}

		A0j2Aij_size = ntmax * (n+1) * (n+2) / 2;

		if (for_parsact.sem_init != NULL)
		    (*for_parsact.sem_init) (A0j2Aij_size);
	    }

	    if (is_right_recursive)
	    {
		store_size = RR_size = (ntmax+1)<<logn;

		RR = (struct RR*) sxcalloc (RR_size, sizeof (struct RR));
		Ai_stack = (int*) sxalloc (RR_size, sizeof (int));
		Ai_stack [0] = 0;
		Bk_stack = (int*) sxalloc (RR_size, sizeof (int));
		Bk_stack [0] = 0;
		store = (struct store*) sxalloc (store_size, sizeof (struct store));
#if EBUG
		Ai_set = sxba_calloc (RR_size);
#endif
	    }

	    parse ();
	    printf ("Nombre de productions de la foret partagee : %i\n", xprod);

	    if (is_right_recursive)
	    {
		sxfree (RR);
		sxfree (Ai_stack);
		sxfree (Bk_stack);
		sxfree (store);
#if EBUG
		sxfree (Ai_set);
#endif
	    }

	    
	    if (print_time)
		sxtime (TIME_FINAL, "Parser");

	    if (is_default_semantics)
	    {
		double val = spf_count [MAKE_Aij (MAKE_A0j (1 , n), 0) /* S0n */];

		if (val < 1.0E9)
		    printf ("Nombre d'arbres de la foret partagee : %.f\n", val);
		else
		    printf ("Nombre d'arbres de la foret partagee : %e\n", val);
	    }
	    else
	    {
		int S0n = MAKE_Aij (MAKE_A0j (1 /* start symbol */, n), 0);

		if (max_tree_nb == 1)
		{
		    left_stack = (int*) sxalloc (n+1, sizeof (int));
		    left_stack [0] = 0;
		    right_stack = (int*) sxalloc (n+1, sizeof (int));
		    right_stack [0] = 0;
		    right_stack2 = (int*) sxalloc (n+1, sizeof (int));
		    right_stack2 [0] = 0;

		    xprod = 0;

		    if (SXBA_bit_is_set (Aij_sem_lhs_set, S0n))
			sem_mark_extract (S0n, 0, n);

		    sxfree (left_stack);
		    sxfree (right_stack);
		    sxfree (right_stack2);
		}
		else
		{
		    int			x;
		    struct ARN_ksem	*psem;

		    Aij_set = sxba_calloc (A0j2Aij_size);

		    for (psem = ARN_disp_ksem [S0n], x = 0; x < max_tree_nb; psem++, x++)
		    {
			if (psem->val == -1)
			    break;

			ksem_td_mark (S0n, x);
		    }

		    ksem_bu_extract ();

		    sxfree (Aij_set);
		}

		printf ("Nombre de productions de la foret partagee (avec semantique) : %i\n", xprod);

		if (print_time)
		    sxtime (TIME_FINAL, "Shared Parse Forest");
	    }

	    if (for_parsact.action != NULL)
	    {
		sxfree (A2A0j [1]);

		if (for_parsact.sem_final != NULL)
		    (*for_parsact.sem_final) (A0j2Aij_size);
	    }

	    sxfree (PT2);
	    bag_free (&pt2_bag);
	    sxbm_free (ap_sets);
	    sxbm_free (red_order_sets);
	}
    }

    if (l <= n || !is_parser)
    {
	bag_free (&shift_bag);
	sxbm_free (ntXindex_set);
    }

    grammar_free ();

    sxfree (sprod2order);
    sxfree (sprod_set);
    sxfree (sprod2attr);

    sxbm_free (global_state_sets);

#if EBUG
    size = sizeof (struct recognize_item) * (n+1) /* RT */
	+ sizeof (SXBA_ELT) * NBLONGS (itemmax+1) * (4) /* global_state_sets */ +
	    sizeof (SXBA_ELT) * NBLONGS (ntmax+1) * (n+1) /* ntXindex_set */ ;
    printf ("Size of recognize structures = %i bytes\n", size);
#endif

    if (is_parser)
    {
	sxfree (PT);
	sxfree (level0_index_set);


#if EBUG
	size = sizeof (struct parse_item) * (n+1) /* PT */
	    ;

	if (for_parsact.action != NULL)
	{
	    size += sizeof (int) * ntmax * (n+1); /* A2A0j */

	    if (is_default_semantics)
		size += sizeof (double) * A0j2Aij_size;
	}

	printf ("Size of parse structures = %i bytes\n", size);
#endif
    }	

    sxfree (TOK);

    if (print_time)
	sxtime (TIME_FINAL, "Done");

    return 0;
}
