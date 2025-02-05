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

#include <values.h>
/* Definit la constante HIBITS == 1<<15 pour les mots de 32! */
#include "sxunix.h"
#include "sxalloc.h"
#include "sxba.h"
#include "XxYxZ.h"
#include "XxY.h"
#include "X.h"
#include "SS.h"

#include <ctype.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>

static bool		is_default_semantics, is_print_prod, print_time, is_no_semantics;
static int		max_tree_nb;

#define TIME_INIT	0
#define TIME_FINAL	1

#if 0
 
static void sxtime (int what, char *str)
{
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

       printf ("%s (%ldms)\n", str, t);
    }
}

static int sxlast_bit (int nb)
{
    /* retourne :
       si nb est nul 0
       sinon k tel que :
        k-1          k
       2    <= nb < 2
    */
static int     LASTBIT [] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};

    register int       bit = 0;

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

typedef unsigned short	ushort;

struct LC0 {
  int NKshift, lgth;
};

struct for_parsact {
    void	(*sem_init) (int),
                (*sem_final) (int);

    bool	(*prdct) (int, int),
                (*action) (int, int, ushort *, int *),
                (*constraint) (int, int, int, int);
};

static bool	is_action_fun, is_prdct_fun, is_constraint_fun;



#if 0
/* Grammaire regles vides + cyclique */

/*
 1: <S>	= <X> <A> <X> ;
 2: <A>	= <S> ;
 3: <A>	= ;
 4: <X> = <X> <X> ;
 5: <X> = x ;
 6: <X> = ;
*/


#define ntmax		3
#define tmax		(-2)
#define itemmax		16
#define prodmax		6
#define initial_state	1
#define	rhs_lgth	3
/* rhs_maxnt nb max d'occur de nt en rhs */
#define	rhs_maxnt	3
#define is_cyclic	true
#define is_epsilon	true


static int lispro [] = 	{0,
			     1, -2, 0,
			     2, 3, 2, 0,
			     1, 0,
			     0,
			     2, 2, 0,
			     -1, 0,
			     0
};

static int prolis [] = 	{0,
			     0, 0, 0,
			     1, 1, 1, 1,
			     2, 2,
			     3,
			     4, 4, 4,
			     5, 5,
			     6
};

static int prdct [] = 	{-1,
			     -1, -1, -1,
			     -1, -1, -1, -1,
			     -1, -1,
			     -1,
			     -1, -1, -1,
			     -1, -1,
			     -1
};

static int item2nbnt [] = {0,
			       0,1,1,
			       0,1,2,3,
			       0,1,
			       0,
			       0,1,2,
			       0,0,
			       0
};

static int prolon [] = {1,
			    4,
			    8,
			    10,
			    11,
			    14,
			    16,
			    17
};

static int lhs [] = {0,
			 1,
			 3,
			 3,
			 2,
			 2,
			 2
};

static bool nt2eps [ntmax+1] = {0,
			      true, /* <S> */
			      true, /* <X> */
			      true /* <A> */
		      };


static bool item2eps [itemmax+1] = {0,
				false, false, true,
				true, true, true, true,
				true, true,
				true,
				true, true, true,
				false, true,
				true
			};



static int prod2nbnt [] = {1,
			       3,
			       1,
			       0,
			       2,
			       0,
			       0
};


static int prodXnt2nbt [prodmax+1][rhs_maxnt+1] = {
{1, 0, 0, 0},				/* 0: 		= <S> $ ; */

{0, 0, 0, 0},				/* 1: <S>	= <X> <A> <X> ; */
{0, 0, 0, 0},				/* 2: <A>	= <S> ; */
{0, 0, 0, 0},				/* 3: <A>	= ; */
{0, 0, 0, 0},				/* 4: <X> 	= <X> <X> ; */
{1, 0, 0, 0},				/* 5: <X> 	= x ; */
{0, 0, 0, 0}				/* 6: <X> 	= ; */
};

static float probabilities [] = {1.0,
1.0,
0.3,
0.7,
0.2,
0.5,
0.3
};


static int parsact [] = {-1,
-1,
-1,
-1,
-1,
-1,
-1
};

static struct LC0 LC0 [] ={
              /* 0 */ {0, 0},
	      /* S */ {1, 12},
	      /* X */ {13, 5},
	      /* A */ {18, 12}
};

static int NKshift [] = {0,
/* S */
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16,
/* X */
11, 12, 13, 14, 16,
/* A */
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16
};

static int constraints [] = {-1,
-1,
-1,
-1
};

static int nt2order [] = {0,
1,
2,
3
};

static int order2nt [] = {0,
1,
2,
3
};

/* Grammaire cyclique. Indexe' par prod_order, ici cycle sur {1, 2} et {4} */
/* Attention ces cycles ne sont pas toujours actifs! */
static int cyclic_parse_order2next [] = {0,
2,
1,
3,
0,
0,
0
};

static char	*ntstring [] = {"",
"S",
"X",
"A"
};

static char	*tstring [] = {"",
"x"
};


static int char2tok (c)
    char c;
{
    return (c != 'x') ? 0 : -1;
}

#endif



#if 0
/* Grammaire cyclique */

/*
 1: <S>	= <A> ;
 2: <A>	= <S> ;
 3: <A>	= a ;
*/


#define ntmax		2
#define tmax		(-2)
#define itemmax		9
#define prodmax		3
#define initial_state	1
#define	rhs_lgth	1
/* rhs_maxnt nb max d'occur de nt en rhs */
#define	rhs_maxnt	1
#define is_cyclic	true
#define is_epsilon	false


static int lispro [] = 	{0,
			     1, -2, 0,
			     2, 0,
			     1, 0,
			     -1, 0
};

static int prolis [] = 	{0,
			     0, 0, 0,
			     1, 1,
			     2, 2,
			     3, 3
};

static int prdct [] = 	{-1,
			     -1, -1, -1,
			     -1, -1,
			     -1, -1,
			     -1, -1
};

static int prolon [] = {1,
			    4,
			    6,
			    8,
			    10
};

static int lhs [] = {0,
			 1,
			 2,
			 2
};

static bool nt2eps [ntmax+1];

static bool item2eps [itemmax+1];

static int prod2nbnt [] = {1,
			       1,
			       1,
			       0
};


static int prodXnt2nbt [prodmax+1][rhs_maxnt+1] = {
{1, 0},				/* 0: 		= <S> $ ; */

{0, 0},				/* 1: <S>	= <A> ; */
{0, 0},				/* 2: <A>	= <S> ; */
{1, 0}				/* 3: <A>	= A ; */
};

static float probabilities [] = {1.0,
1.0,
0.3,
0.7
};


static int parsact [] = {-1,
-1,
-1,
-1
};

static struct LC0 LC0 [] ={
              /* 0 */ {0, 0},
	      /* S */ {1, 3},
	      /* A */ {4, 3}
};

static int NKshift [] = {0,
/* S */
8, 6, 4,
/* A */
8, 6, 4,
};

static int constraints [] = {-1,
-1,
-1,
-1
};

static int nt2order [] = {0,
1,
2
};

static int order2nt [] = {0,
1,
2
};

/* Grammaire cyclique. Indexe' par prod_order, ici cycle sur {1, 2} */
static int cyclic_parse_order2next [] = {0,
2,
1,
0
};

static char	*ntstring [] = {"",
"S",
"A"
};

static char	*tstring [] = {"",
"a"
};


static int char2tok (c)
    char c;
{
    return (c != 'a') ? 0 : -1;
}

#endif


/* #if 0 */

/*
 1: <RNA>		@1 = <RNA> <DoubleStrand> ;
 2: <RNA>		@2 = <DoubleStrand> ;
 3: <RNA>		@2 = <RNA> <Base> ;
 4: <RNA>		@3 = <Base> ;

 5: <DoubleStrand>	@4 = A <SingleStrand> U ;
 6: <DoubleStrand>	@5 = A <InternalLoop> U ;
 7: <DoubleStrand>	@6 = A <MultipleLoop> U ;
 8: <DoubleStrand>	@4 = G <SingleStrand> C ;
 9: <DoubleStrand>	@5 = G <InternalLoop> C ;
10: <DoubleStrand>	@6 = G <MultipleLoop> C ;
11: <DoubleStrand>	@4 = G <SingleStrand> U ;
12: <DoubleStrand>	@5 = G <InternalLoop> U ;
13: <DoubleStrand>	@6 = G <MultipleLoop> U ;
14: <DoubleStrand>	@4 = U <SingleStrand> A ;
15: <DoubleStrand>	@5 = U <InternalLoop> A ;
16: <DoubleStrand>	@6 = U <MultipleLoop> A ;
17: <DoubleStrand>	@4 = C <SingleStrand> G ;
18: <DoubleStrand>	@5 = C <InternalLoop> G ;
19: <DoubleStrand>	@6 = C <MultipleLoop> G ;
20: <DoubleStrand>	@4 = U <SingleStrand> G ;
21: <DoubleStrand>	@5 = U <InternalLoop> G ;
22: <DoubleStrand>	@6 = U <MultipleLoop> G ;

23: <InternalLoop>	@7 = <DoubleStrand> ;
24: <InternalLoop>	@8 = <InternalLoop> <Base> ;
25: <InternalLoop>	@9 = <Base> <InternalLoop> ;

26: <MultipleLoop>	@10 = <MultipleLoop> <DoubleStrand> ;
27: <MultipleLoop>	@11 = <DoubleStrand> ;
28: <MultipleLoop>	@12 = <MultipleLoop> <Base> ;
29: <MultipleLoop>	@13 = <Base> ;

30: <Base>		@16 = A ;
31: <Base>		@16 = U ;
32: <Base>		@16 = C ;
33: <Base>		@16 = G ;

34: <SingleStrand>	@14 = <SingleStrand> <Base> ;
35: <SingleStrand>	@15 = <Base> ;

* @7 : len = 0;
* @8 : len = $1.len +1;
* @9 : len = $2.len +1;

* @14 :len = S1.len + 1;
* @15 : len = 0;

*/



#define ntmax		6
#define tmax		(-5)
#define itemmax		116 
#define prodmax		35
#define initial_state	1
#define	rhs_lgth	3
/* rhs_maxnt nb max d'occur de nt en rhs */
#define	rhs_maxnt	2
#define is_cyclic	false
#define is_epsilon	false

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

static bool nt2eps [ntmax+1];

#if 0
static bool item2eps [itemmax+1];
#endif

static int item2nbnt [] = {0,
/* 0 */	0, 1, 1,
/* 1 */	0, 1, 2,
/* 2 */	0, 1,
/* 3 */	0, 1, 2,
/* 4 */	0, 1,
/* 5 */	0, 0, 1, 1,
/* 6 */	0, 0, 1, 1,
/* 7 */	0, 0, 1, 1,
/* 8 */	0, 0, 1, 1,
/* 9 */	0, 0, 1, 1,
/* 10 */	0, 0, 1, 1,
/* 11 */	0, 0, 1, 1,
/* 12 */	0, 0, 1, 1,
/* 13 */	0, 0, 1, 1,
/* 14 */	0, 0, 1, 1,
/* 15 */	0, 0, 1, 1,
/* 16 */	0, 0, 1, 1,
/* 17 */	0, 0, 1, 1,
/* 18 */	0, 0, 1, 1,
/* 19 */	0, 0, 1, 1,
/* 20 */	0, 0, 1, 1,
/* 21 */	0, 0, 1, 1,
/* 22 */	0, 0, 1, 1,
/* 23 */	0, 1,
/* 24 */	0, 1, 2,
/* 25 */	0, 1, 2,
/* 26 */	0, 1, 2,
/* 27 */	0, 1,
/* 28 */	0, 1, 2,
/* 29 */	0, 1,
/* 30 */	0, 0,
/* 31 */	0, 0,
/* 32 */	0, 0,
/* 33 */	0, 0,
/* 34 */	0, 1, 2,
/* 35 */	0, 1,
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
{1, 0, 0},	/* 0: 		= <RNA> $ ; */

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
1.,

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

#if 0
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
#endif

static int NKshift [] = {0,
/* RNA = 26 */
111-1,
109-1,
107-1,
105-1,
83-1,
79-1,
75-1,
71-1,
67-1,
63-1,
59-1,
55-1,
51-1,
47-1,
43-1,
39-1,
35-1,
31-1,
27-1,
23-1,
19-1,
15-1,
13-1,
10-1,
8-1,
5-1,

/* DoubleStrand = 18 */
83-1,
79-1,
75-1,
71-1,
67-1,
63-1,
59-1,
55-1,
51-1,
47-1,
43-1,
39-1,
35-1,
31-1,
27-1,
23-1,
19-1,
15-1,

/* Base = 4 */
111-1,
109-1,
107-1,
105-1,

/* SingleStrand = 6 */
116-1,
113-1,
111-1,
109-1,
107-1,
105-1,

/* InternalLoop = 25 */
92-1,
89-1,
87-1,
111-1,
109-1,
107-1,
105-1,
83-1,
79-1,
75-1,
71-1,
67-1,
63-1,
59-1,
55-1,
51-1,
47-1,
43-1,
39-1,
35-1,
31-1,
27-1,
23-1,
19-1,
15-1,

/* MultipleLoop = 26 */
103-1,
100-1,
98-1,
95-1,
111-1,
109-1,
107-1,
105-1,
83-1,
79-1,
75-1,
71-1,
67-1,
63-1,
59-1,
55-1,
51-1,
47-1,
43-1,
39-1,
35-1,
31-1,
27-1,
23-1,
19-1,
15-1
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

#if 0
static char	*tstring [] = {"",
				    "A",
				    "U",
				    "G",
				    "C"
				};
#endif

static int char2tok (char c)
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


static bool nt2eps [ntmax+1];

static bool item2eps [itemmax+1];

/* Ne contient pas les transitions "kernel" */
static struct LC0 LC0 [] = {
{0, 0},
{1, 2}
};
static int NKshift [] = {0, 4, 7};

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
                                prod_order2next [prodmax+1], parse_order2next [prodmax+1];
static SXBA                     is_mutual_right_recursive_prod;
static bool                  is_mutual_right_recursive;
static SXBA			*INIT, *FIRST_1, *NT2CHAIN, RHS_IN_T;



static char	ME [] = "earley_LC";

static char	Usage [] = "\
Usage:\t%s ('R' | 'r' |'P' | 'p') ['p'] \"source string\"\n";


static	int		xprod;

static int		n;
static int		logn, filtren;

static int		*A2A0j [ntmax+1];

static int		A0j2Aij_size;

#define MAKE_Ai(A,i)	(((A)<<logn)+(i))
#define MAKE_Ai2i(Ai) (((Ai)&filtren))
#define MAKE_Ai2A(Ai) (((Ai)>>logn))

#define MAKE_A0j(A,j)	(A2A0j [A][j])
#define MAKE_Aij(A0j,i)	((A0j)+(i))


static int		*TOK;
static char		*source;

static int		cyclic_stack [ntmax+1];

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

static struct recognize_item {
    int		shift_NT_hd [ntmax + 1];
    int		shift_state_stack [itemmax + 1];
    SXBA	index_sets [itemmax + 1];
}			*RT;

static int		*T1_shift_NT_hd, *T2_shift_NT_hd;
static int		*T1_shift_state_stack, *T2_shift_state_stack;
static SXBA             *T1_index_sets, *T2_index_sets, *T1_backward_index_sets, *T2_backward_index_sets;

static int		shift_NT_stack_1 [ntmax + 1],
                        shift_NT_stack_2 [ntmax + 1];
static int		*T1_shift_NT_stack, *T2_shift_NT_stack;


static bag_header	shift_bag;


static SXBA		*global_state_sets;

static SXBA		init_state_set;

static SXBA		*ntXindex_set;

static SXBA		*constraint_to_be_checked; /* si is_epsilon et for_parsact.constraint != NULL */

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

static int		rhs_stack [rhs_lgth+1];

static ushort		prod_core [rhs_maxnt];

#if 0
/* unused */

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

#endif

static struct parse_item {
  SXBA			backward_index_sets [itemmax + 1];
  int			reduce_NT_hd [ntmax + 1];
}			*PT;

static int		*T1_reduce_NT_hd, *T2_reduce_NT_hd;

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
static SXBA	*wis;

struct ARN_sem {
  float		val;
  ushort	prod_core [rhs_maxnt];
};

static struct ARN_sem	*ARN_sem;

struct ARN_ksem {
    float 		val;
    unsigned long	vector;
    /* int		vector [rhs_maxnt]; */
    ushort		prod_core [rhs_maxnt];
};

static int		log_max_tree_nb, filtre_max_tree_nb;
static int		ARN_ksem_size;
static struct ARN_ksem	*ARN_ksem, **ARN_disp_ksem, *ARN_ksem_top;
static SXBA		ARN_ksem_is_marked;

static struct kvals {
    float		val;
    unsigned long	vector;
    /* int 	vector [rhs_maxnt]; */
} *kvals, **disp_kvals;

static SXBA	Aij_set;

#if EBUG
static SXBA     Aij_rhs_set, Aij_lhs_set;
#endif

static SXBA     Aij_sem_lhs_set;
/* end is_parser */

#if 0
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

    sxba_or (is_mutual_right_recursive_prod, prod_gset);

    card = sxba_cardinal (prod_gset);

    next_prod = -1;
    prod = sxba_scan_reset (prod_gset, 0);

    do
    {
	order2prod [gorder] = prod;
	prod2order [prod] = gorder;
	prod_order2next [gorder] = next_prod < 0 ? gorder-card : gorder; /* sequence */
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


static bool
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
#endif


static XxY_header	prod_graph;
static SXBA		prod_set, prod_cycle, prod_gset, prod_gset2;
static int		prod_gstack [prodmax+1];


static void found_prod_cycle (int q)
{
    /* prod_gset contient un cycle, on le complete */
    int	prod, p, couple;

    SXBA_1_bit (prod_gset2, q);

    XxY_Xforeach (prod_graph, q, couple)
    {
	prod = XxY_Y (prod_graph, couple);

	if (SXBA_bit_is_set (prod_set, prod))
	{
	    if (SXBA_bit_is_set (prod_cycle, prod))
	    {
		p = 0;

		while ((p = sxba_scan (prod_gset2, p)) > 0)
		{
		    if (SXBA_bit_is_reset_set (prod_cycle, p))
			PUSH (prod_gstack, p);
		}
	    }
	    else
		if (!SXBA_bit_is_set (prod_gset2, prod))
		    found_prod_cycle (prod);
	}
    }

    SXBA_0_bit (prod_gset2, q);
}

static void process_prod_cycle ()
{
    int prod = 0;

    while ((prod = sxba_scan (prod_gset, prod)) > 0)
    {
	SXBA_1_bit (prod_cycle, prod);
	PUSH (prod_gstack, prod);
    }

    while (!IS_EMPTY (prod_gstack))
    {
	found_prod_cycle (POP (prod_gstack));
    }
}

static bool look_prod_cycle (int q, int p)
{
    /* On cherche un cycle autour de q */
    /* On est sur p */
    int prod, couple;

    SXBA_1_bit (prod_gset, p);

    XxY_Xforeach (prod_graph, p, couple)
    {
	prod = XxY_Y (prod_graph, couple);

	if (SXBA_bit_is_set (prod_set, prod))
	{
	    if (prod == q)
	    {
		/* on a trouve un cycle */
		process_prod_cycle ();
		break;
	    }
	    else
		if (!SXBA_bit_is_set (prod_gset, prod) && look_prod_cycle (q, prod))
		    break;
	}
    }

    SXBA_0_bit (prod_gset, p);

    return couple != 0;
}


static void grammar ()
{
    int		prod, state, X, Y, A, B, x, lim, t, item, tnb, couple2, q, p, cycle_nb, cq, cp, card;
    SXINT	couple, qXp, cqXcp;
    SXBA	*NT2NT, *NT2T, T;

    /* calcul de order2prod, prod2order, prod_order2next et parse_order2next */
    /* prod_order2next est exploite par valeurs croissantes de order
       parse_order2next est exploite par valeurs decroissantes de order */
    /* Soient ;
       p = A -> \alpha B \beta
       q = B -> \gamma
       Si \beta =>* \epsilon alors q doit s'executer avant p -- on note (q, p) (order (q) > order (p)).
       Si des productions sont mutuellement recursives droites, il n'y a pas d'ordre total. */
    /* On appelle cycle un ensemble de productions C = {p1, p2, ..., pn} tel que
       \forall pi \in C il existe (pi, q1), (q1, q2), ..., (qm, pi) avec qj \in C.
       Un cycle C est maximal si \not \exists cycle C' avec C \includes C'. */
    
    /* Si la grammaire est mutuellement recursive droite, dans le cas general il n'est pas
       possible de calculer un ordre d'execution statique qui puisse s'appliquer a tous les
       cas dynamiques ex:
       I = A -> \alpha B
       II = B -> A
       III = A -> t1
       IV = B -> t2
       l'ordre d'execution des prod I et II depend de la reconnaissance de t1 ou t2. */

static int	prod_stack [prodmax+1];
#if 0
static int	prod_nb = prodmax;
#endif

    SXBA	nt_set, cycle_set;
    XxY_header	pXnt, ntXp, cycle_graph;
    int		gorder;

    is_mutual_right_recursive_prod = sxba_calloc (prodmax+1);

    prod_set = sxba_calloc (prodmax+1);
    sxba_fill (prod_set);
    SXBA_0_bit (prod_set, 0);

    XxY_alloc (&pXnt, "pXnt", itemmax, 1, 0, 1, NULL, NULL);
    XxY_alloc (&ntXp, "ntXp", itemmax, 1, 1, 0, NULL, NULL);

    nt_set = sxba_calloc (ntmax+1);

    gorder = prodmax;

    for (prod = 1; prod <= prodmax; prod++)
    {
	A = lhs [prod];
	XxY_set (&ntXp, A, prod, &couple);

	item = prolon [prod+1] - 2;

	if ((Y = lispro [item]) <= 0)
	{
	    /* item = A -> \alpha t. ou item = A -> \epsilon */
	    order2prod [gorder] = prod;
	    prod2order [prod] = gorder;
	    prod_order2next [gorder] = gorder; /* en sequence */
	    parse_order2next [gorder] = gorder; /* en sequence */
	    gorder--;
	    PUSH (prod_stack, prod);
	    SXBA_0_bit (prod_set, prod);
	}
	else
	{
	    SXBA_1_bit (nt_set, Y);
	    XxY_set (&pXnt, prod, Y, &couple);

	    while ((Y = lispro [--item]) > 0)
	    {
		SXBA_1_bit (nt_set, Y);
		XxY_set (&pXnt, prod, Y, &couple);
	    }
	}
    }

    if (gorder > 0)
    {
	static int prod2nb [prodmax+1];

	XxY_alloc (&prod_graph, "prod_graph", itemmax, 1, 1, 0, NULL, NULL);

	A = 0;

	while ((A = sxba_scan (nt_set, A)) > 0)
	{
	    XxY_Yforeach (pXnt, A, couple)
	    {
		p = XxY_X (pXnt, couple);

		XxY_Xforeach (ntXp, A, couple2)
		{
		    q = XxY_Y (ntXp, couple2);
		    XxY_set (&prod_graph, q, p, &qXp);
		    prod2nb [p]++;
		}
	    }
	}

#if EBUG
	if (IS_EMPTY (prod_stack))
	    sxtrap (ME, "grammar");
#endif

	while (!IS_EMPTY (prod_stack))
	{
	    q = POP (prod_stack);

	    XxY_Xforeach (prod_graph, q, qXp)
	    {
		prod = XxY_Y (prod_graph, qXp);
		XxY_erase (prod_graph, qXp);

		if (--prod2nb [prod] == 0)
		{
		    order2prod [gorder] = prod;
		    prod2order [prod] = gorder;
		    prod_order2next [gorder] = gorder; /* en sequence */
		    parse_order2next [gorder] = gorder; /* en sequence */
		    gorder--;
		    PUSH (prod_stack, prod);
		    SXBA_0_bit (prod_set, prod);
		}
	    }
	}

	if (gorder > 0)
	{
	    /* Il y a des cycles sur certains elements de prod_set */
	    static int prod2cycle_nb [prodmax+1];
	    static int prod2next_cycle_prod [prodmax+1];
	    static int cycle_nb2hd [prodmax+1];
	    static int cycle_nb2card [prodmax+1];
	    static int cycle2nb [prodmax+1];

	    is_mutual_right_recursive = true;

	    prod_cycle = sxba_calloc (prodmax+1);
	    prod_gset = sxba_calloc (prodmax+1);
	    prod_gset2 = sxba_calloc (prodmax+1);

	    cycle_nb = 0;
	    prod = 0;

	    while ((prod = sxba_scan (prod_set, prod)) > 0)
	    {
		cycle_nb++;

		if (look_prod_cycle (prod, prod))
		{
		    /* prod_cycle contient le cycle maximal autour de prod */
		    p = prod;

		    do
		    {
			SXBA_0_bit (prod_set, p);
			SXBA_0_bit (prod_cycle, p);
			SXBA_1_bit (is_mutual_right_recursive_prod, p);
			prod2cycle_nb [p] = cycle_nb;
			prod2next_cycle_prod [p] = cycle_nb2hd [cycle_nb];
			cycle_nb2hd [cycle_nb] = p;
			cycle_nb2card [cycle_nb]++;
		    } while ((p = sxba_scan (prod_cycle, p)) > 0);
		}
		else
		{
		    /* pas de cycle sur prod */
		    SXBA_0_bit (prod_set, prod);
		    prod2cycle_nb [prod] = cycle_nb;
		    cycle_nb2hd [cycle_nb] = prod;
		    cycle_nb2card [cycle_nb] = 0;
		}
	    }

	    cycle_set = sxba_calloc (cycle_nb+1);
	    sxba_fill (cycle_set);
	    SXBA_0_bit (cycle_set, 0);

	    XxY_alloc (&cycle_graph, "cycle_graph", XxY_top (prod_graph), 1, 1, 0, NULL, NULL);

	    /* prod_graph est reecrit avec les "cycle_nb". */
	    XxY_foreach (prod_graph, qXp)
	    {
		q = XxY_X (prod_graph, qXp);
		cq = prod2cycle_nb [q];
		p = XxY_Y (prod_graph, qXp);
		cp = prod2cycle_nb [p];

#if EBUG
		if (cp == 0 || cq == 0)
		    sxtrap (ME, "grammar");
#endif

		if (cq != cp)
		{
		    /* pas ds le meme cycle */
		    XxY_set (&cycle_graph, cq, cp, &cqXcp);
		    cycle2nb [cp]++;
		    SXBA_0_bit (cycle_set, cp);
		}
	    }

	    /* On exploite cycle_graph */

	    while ((cycle_nb = sxba_scan_reset (cycle_set, 0)) > 0)
	    {
		card = cycle_nb2card [cycle_nb];
		prod = cycle_nb2hd [cycle_nb];

		order2prod [gorder] = prod;
		prod2order [prod] = gorder;
		prod_order2next [gorder] = gorder-card;
		parse_order2next [gorder] = card == 1 ? gorder+1 : gorder;

		gorder--;

		if (card > 1)
		{
		    prod = prod2next_cycle_prod [prod];

		    do
		    {
			order2prod [gorder] = prod;
			prod2order [prod] = gorder;
			prod_order2next [gorder] = gorder;
			prod = prod2next_cycle_prod [prod];
			parse_order2next [gorder] = prod == 0 ? gorder+card : gorder;

			gorder--;
		    } while (prod != 0);
		}

		XxY_Xforeach (cycle_graph, cycle_nb, cqXcp)
		{
		    cp = XxY_Y (cycle_graph, cqXcp);

		    if (--cycle2nb [cp] == 0)
			SXBA_1_bit (cycle_set, cp);
		}
	    }

	    sxfree (cycle_set);
	    XxY_free (&cycle_graph);

	    sxfree (prod_cycle);
	    sxfree (prod_gset);
	    sxfree (prod_gset2);
	}

	XxY_free (&prod_graph);
    }

#if EBUG
    if (gorder != 0)
	sxtrap (ME, "grammar");
#endif

    sxfree (prod_set);
    XxY_free (&pXnt);
    XxY_free (&ntXp);
    sxfree (nt_set);


#if 0
    is_mutual_right_recursive_prod = sxba_calloc (prodmax+1);

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

	if ((Y = lispro [item]) <= 0)
	{
	    /* item = A -> \alpha t. ou item = A -> \epsilon */
	    order2prod [gorder] = prod;
	    prod2order [prod] = gorder;
	    prod_order2next [gorder] = gorder; /* en sequence */
	    parse_order2next [gorder] = gorder; /* en sequence */
	    gorder--;
	    nt2nb [A]--;
	    nt2nb [0]--;
	}
	else
	{
	    XxY_set (&order_graph, prod, Y, &couple);

	    while ((Y = lispro [--item]) > 0)
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
		prod_order2next [gorder] = gorder; /* en sequence */
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
	    is_mutual_right_recursive = true;
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
#endif

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
	    state = NKshift [x];
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
    /* Pour l'instant, LOOK-AHEAD(U -> alpha beta) = {} */

    NT2NT = sxbm_calloc (ntmax + 1, ntmax + 1);
    NT2T = sxbm_calloc (ntmax + 1, -tmax + 1);

    if (is_epsilon)
    {
	T = sxba_calloc (-tmax + 1);
	sxba_fill (T);
	SXBA_0_bit (T, 0);
    }

    FIRST_1 = sxbm_calloc (-tmax + 1, itemmax + 1);
    
    for (prod = 0; prod <= prodmax; prod++)
    {
	item = prolon [prod];
	A = lhs [prod];
	X = lispro [item];

	while ((X = lispro [item++]) != 0)
	{
	    if (X < 0)
	    {
		SXBA_1_bit (NT2T [A], -X);
		break;
	    }
	    else
	    {
		SXBA_1_bit (NT2NT [A], X);

		if (nt2eps [X] == false)
		    break;
	    }
	}

	if (X == 0)
	    sxba_or (NT2T [A], T);
    }

    if (is_epsilon)
	sxfree (T);

    fermer (NT2NT, ntmax+1);

    for (prod = 0; prod <= prodmax; prod++)
    {
	item = prolon [prod];

	while ((X = lispro [item]) != 0)
	{
	    if (X < 0)
		SXBA_1_bit (FIRST_1 [-X], item);

	    else
	    {
		t = 0;

		while ((t = sxba_scan (NT2T [X], t)) > 0)
		{
		    SXBA_1_bit (FIRST_1 [t], item);
		}

		Y = 0;

		while ((Y = sxba_scan (NT2NT [X], Y)) > 0)
		{
		    t = 0;

		    while ((t = sxba_scan (NT2T [Y], t)) > 0)
		    {
			SXBA_1_bit (FIRST_1 [t], item);
		    }
		}
	    }

	    item++;
	}

	/* POUR L'INSTANT, ON FAIT DS LE GROSSIER!!! */
	/* ON CALCULERA LE VRAI LOOK-AHEAD + TARD */
	for (t = 1; t <= -tmax; t++)
	    SXBA_1_bit (FIRST_1 [t], item);
    }

    sxbm_free (NT2NT);
    sxbm_free (NT2T);
}


static    struct chains2attr	*chains2attr;

static  void chains_oflw (SXINT old_size, SXINT new_size)
{
    (void) old_size;
    chains2attr = (struct chains2attr*) sxrealloc (chains2attr, new_size+1, sizeof (struct chains2attr));
}


static void grammar2 ()
{
    /* A chaque production simple r: A -> B on affecte un entier n(r) t.q.
       r1: A -> B et r2: B -> C => n(r1) < n(r2). */
    XxY_header		chains;
    SXBA		chain_set, nb0;
    int			nb [ntmax + 1];
    int			prod, x, A, B;
    SXINT		AxB;

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

static void grammar_free ()
{
    sxbm_free (INIT);
    sxbm_free (NT2CHAIN);
    sxbm_free (FIRST_1);
    sxfree (RHS_IN_T);
    sxfree (is_mutual_right_recursive_prod);
}




static bool NON_EQUAL (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- != *rhs_bits_ptr--)
	    return true;
    }

    return false;
}

#if 0
/* defined but not used */

static bool IS_INCLUDE (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));
    register SXBA_ELT	val;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	val = *lhs_bits_ptr--;

	if ((val & *rhs_bits_ptr--) != val)
	    return false;
    }

    return true;
}
#endif

static void OR (SXBA lhs_bits_array, SXBA rhs_bits_array)
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
    register int	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));

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


static bool OR_CHANGED (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
    register SXBA_ELT	val1, val2;
    bool		has_changed = false;

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
	    has_changed = true;
	    *lhs_bits_ptr = val2;
	}

	lhs_bits_ptr--;
    }

    return has_changed;
}

#if 0
/* defined but not used */
static void OR_MINUS (SXBA bits_array1, SXBA bits_array2, SXBA bits_array3)
{
    /* bits_array1 or (bits_array2 - bits_array3) */
    register SXBA	bits_ptr1, bits_ptr2, bits_ptr3;
    register int	slices_number = SXNBLONGS (SXBASIZE (bits_array2));

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
#endif

static bool OR_AND (SXBA bits_array1, SXBA bits_array2, SXBA bits_array3)
{
    /* bits_array1 or (bits_array2 - bits_array3) */
    register SXBA	bits_ptr1, bits_ptr2, bits_ptr3;
    register int	slices_number = SXNBLONGS (SXBASIZE (bits_array2));
    register SXBA_ELT	val1, val2;
    bool		has_changed = false;

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
		has_changed = true;
	    }
	}

	bits_ptr1--;
    }

    return has_changed;
}


static bool OR_AND_MINUS (SXBA bits_array1, SXBA bits_array2, SXBA bits_array3, SXBA bits_array4)
{
    /* bits_array4 =  bits_array2 & bits_array3 - bits_array1
       bits_array1 |= bits_array4
       retourne vrai ssi bits_array4 est non vide */
    register SXBA	bits_ptr1, bits_ptr2, bits_ptr3, bits_ptr4;
    /* On change la taille! */
    register int	slices_number = SXNBLONGS (SXBASIZE (bits_array4) = SXBASIZE (bits_array2));
    register SXBA_ELT	val;
    bool		is_set = false;

    bits_ptr1 = bits_array1 + slices_number,
    bits_ptr2 = bits_array2 + slices_number,
    bits_ptr3 = bits_array3 + slices_number,
    bits_ptr4 = bits_array4 + slices_number;

    while (slices_number-- > 0)
    {
	if ((val = *bits_ptr4-- = (*bits_ptr2--) & (*bits_ptr3--) & (~(*bits_ptr1))) != 0L)
	    is_set = true;

	*bits_ptr1-- |= val;
    }

    return is_set;
}

#if 0
/* defined but not used */
static void OR_RAZ (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));

    /* *lhs_bits_array <= *rhs_bits_array */
    
    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	*lhs_bits_ptr-- |= *rhs_bits_ptr;
	*rhs_bits_ptr-- = 0L;
    }
}

static void COPY (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- >= /* On copie aussi la dimension de  rhs_bits_array!!! */ 0)
    {
	*lhs_bits_ptr-- = *rhs_bits_ptr--;
    }
}

static void COPY_RAZ (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));

    /* *lhs_bits_array <= *rhs_bits_array */
    
    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	*lhs_bits_ptr-- = *rhs_bits_ptr;
	*rhs_bits_ptr-- = 0L;
    }
}
#endif

static bool AND (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
    register int	lhs_slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));
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


static bool AND3 (SXBA lhs_bits_array, SXBA op1_bits_array, SXBA op2_bits_array)
{
    /* On suppose que lhs_bits_array est vide au-dela de op2_bits_array */

    register SXBA	lhs_bits_ptr, op1_bits_ptr, op2_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (op2_bits_array));
    bool		ret_val = false;

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
	    ret_val = true;
    }

    return ret_val;
}

static bool AND3_SHIFT (SXBA lhs_bits_array, SXBA op1_bits_array, SXBA op2_bits_array, int shift)
{
    /* Identique a AND3 excepte' que op2_bits_array est decale a droite de shift */
    /* Shift > 0 */

    register SXBA	lhs_bits_ptr, op1_bits_ptr, op2_bits_ptr;
    int			delta = SXDIV (shift);
    register int	slices_number = SXNBLONGS (SXBASIZE (op2_bits_array)) - delta;
    int			right_shift = SXMOD (shift);
    int			left_shift = SXBITS_PER_LONG - right_shift;
    SXBA_ELT		filtre = (~(0L)) >> left_shift;
    SXBA_ELT		op2val = 0L, prev_op2val;
    bool		ret_val = false;

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
	    ret_val = true;
    }

    return ret_val;
}


static bool MINUS (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
    bool		ret_val = false;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- &= ~(*rhs_bits_ptr--))
	    ret_val = true;
    }

    return ret_val;
}



static bool IS_AND (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
    register int	lhs_slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));

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

#if 0
static void OR_SHIFT_RESET (SXBA lhs_bits_array, SXBA rhs_bits_array, int n)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
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


static bool IS_AND_MINUS (SXBA bits_array1, SXBA bits_array2, SXBA bits_array3)
{
    /* bits_array1 et (bits_array2 - bits_array3) */
    register SXBA	bits_ptr1, bits_ptr2, bits_ptr3;
    register int	slices_number1 = SXNBLONGS (SXBASIZE (bits_array1));
    register int	slices_number2 = SXNBLONGS (SXBASIZE (bits_array2));
    register int	slices_number3 = SXNBLONGS (SXBASIZE (bits_array3));

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


static bool IS_SUBSET (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	lhs_slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));
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
#endif

static void bag_alloc (bag_header *pbag, char *name, int size)
{
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



static SXBA bag_get (bag_header *pbag, int size)
{
    int 	slice_nb = SXNBLONGS (size) + 1;
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



static void bag_free (bag_header *pbag)
{
#if EBUG
    printf ("Bag %s: used_size = %ld bytes, total_size = %ld bytes\n",
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
    
#if 0
/* defined but not used */

static void bag_clear (bag_header *pbag)
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
    
#endif

static void reduce (int A, int j, int i)
{
    /* Toutes les reductions vers Aj ont ete effectuees
       Cas du reconnaisseur : on fait le "shift" sur A dans la liste "j".
       cas du parser : en plus, on fait les reductions eventuelles B -> alpha Aji */
    int		state, new_state, B, Y, YY, prdct_no, hd, new_j;
    SXBA	index_set, index_set2, backward_index_set;
    struct recognize_item	*RTj;

    if ((prdct_no = constraints [A]) != -1 &&
	is_constraint_fun &&
	!for_parsact.constraint (prdct_no, A, j, i))
	return;

    RTj = &(RT [j]);

    for (state = RTj->shift_NT_hd [A]; state > 0; state = RTj->shift_state_stack [state])
    {
	/* state = B -> \beta . A \gamma */
	new_state = state+1;

	if (SXBA_bit_is_set (FIRST_1[-TOK [i+1]], new_state))
	{
	    index_set = RTj->index_sets [state];

	    new_j = j;

	    while ((Y = lispro [new_state]) != 0)
	    {
		if (Y < 0)
		{
		    /* Y == ai+1 */
		    /* new_state = B -> \beta A \alpha . ai+1 \gamma et \alpha =>* \epsilon */
		    if (i < n &&
			(!SXBA_bit_is_set (FIRST_1[-TOK [i+2]], new_state+1) ||
			((prdct_no = prdct [new_state]) != -1 &&
			 is_prdct_fun &&
			 !(*for_parsact.prdct) (-i-1, prdct_no))))
		    {
			/* Le look-ahead ou le predicat a echoue */
			break;
		    }
		}

		if ((index_set2 = T2_index_sets [new_state]) == NULL)
		{
		    if (Y > 0)
		    {
			YY = Y;

			if ((hd = T2_shift_NT_hd [Y]) == 0)
			    PUSH (T2_shift_NT_stack, Y);
		    }
		    else
		    {
			hd = T2_shift_NT_hd [0];
			YY = 0;
		    }

		    T2_shift_state_stack [new_state] = hd;
		    T2_shift_NT_hd [YY] = new_state;

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
		    SXBA_1_bit (backward_index_set, new_j);

		if (index_set == NULL)
		    SXBA_1_bit (index_set2, new_j);
		else
		    OR (index_set2, index_set);

		if (is_epsilon && Y > 0 && nt2eps [Y])
		{
		    new_state++;

		    if (!SXBA_bit_is_set (FIRST_1 [-TOK [i+1]], new_state))
			break;

		    new_j = i;
		}
		else
		    break;
	    }

	    if (Y == 0)
	    {
		/* new_state = B -> \alpha A \beta .  et \beta =>* \epsilon */
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

		    SXBA_1_bit (backward_index_set, new_j);

		    if (index_set == NULL)
			SXBA_1_bit (index_set2, new_j);
		    else
			OR (index_set2, index_set);
		}

		if (index_set == NULL)
		{
		    /* new_state = B -> \alpha A \beta .  et \alpha \beta =>* \epsilon */
		    if (is_cyclic) {
			if (SXBA_bit_is_reset_set (ntXindex_set [B], j) &&
			    nt2order [B] > nt2order [A]) {
			    /* On est ds le cas cyclique, B est nouveau et son ordre de passage
			       a deja ete examine, on force son re-examen */
			    PUSH (cyclic_stack, B);
			}
		    }
		    else
			SXBA_1_bit (ntXindex_set [B], j);
		}
		else
		    /* new_state = B -> \alpha A \beta .
		       et \beta =>* \epsilon
		       et \alpha \not =>* \epsilon */
		    OR (ntXindex_set [B], index_set);
	    }
	}
    }
}

static void scan_reduce (int i)
{
    register SXBA_ELT	filtre;
    register int	j, indice, order, A;

    filtre = 1 << SXMOD (i);
    indice = SXDIV (i) + 1;

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

	if (is_cyclic && !IS_EMPTY (cyclic_stack))
	{
	    do
	    {
		reduce (POP (cyclic_stack), j, i+1);
	    } while (!IS_EMPTY (cyclic_stack));
	}

	if ((filtre >>= 1) == 0)
	{
	    for (A = ntmax; A > 0; A--)
		*((nt_hd [A])--) = 0;

	    if (--j < 0)
		break;

	    filtre = 1 << SXMOD (j);
	}
	else
	    j--;
    }
}


static bool complete (int i)
{
    int		hd, state, next_state, X, Y, YY, A, prdct_no;
    SXBA	index_set, index_set2, backward_index_set;
    bool	is_tok, is_scan_reduce = false;

    /* Le look-ahead est verifie pour tous les state de T1_state_set. */
    /* De plus si A -> \alpha X . ai+1 \beta, le predicat de ai+1 est verifie'
       ainsi que FIRST_1 [\beta]. */

    if ((state = T1_shift_NT_hd [0]) != 0)
    {
	/* A -> \alpha X . ai+1 \beta */
	is_tok = true;
	T1_shift_NT_hd [0] = 0;

	if (i < n)
	{
	    do
	    {
		index_set = T1_index_sets [state];
		next_state = state+1;

		while ((Y = lispro [next_state]) != 0)
		{
		    if (Y < 0)
		    {
			/* Y == ai+2 */
			/* new_state = A -> \alpha X ai+1 \beta . ai+2 \gamma et \beta =>* \epsilon */
			if (!SXBA_bit_is_set (FIRST_1[-TOK [i+3]], next_state+1) ||
			    ((prdct_no = prdct [next_state]) != -1 &&
			     is_prdct_fun &&
			     !(*for_parsact.prdct) (-i-2, prdct_no)))
			{
			    /* Le look-ahead de next_state ou le predicat de ai+2 a echoue */
			    break;
			}
		    }

		    if (Y > 0)
		    {
			YY = Y;

			if ((hd = T2_shift_NT_hd [Y]) == 0)
			    PUSH (T2_shift_NT_stack, Y);
		    }
		    else
		    {
			hd = T2_shift_NT_hd [0];
			YY = 0;
		    }

		    T2_shift_state_stack [next_state] = hd;
		    T2_shift_NT_hd [YY] = next_state;

		    T2_index_sets [next_state] = index_set;

		    if (is_epsilon)
		    {
			if (is_parser && lispro [next_state-1] > 0)
			{
			    backward_index_set = T2_backward_index_sets [next_state] = bag_get (&shift_bag, i+1);
			    SXBA_1_bit (backward_index_set, i);
			}

			if (Y > 0 && nt2eps [Y])
			{
			    next_state++;

			    if (SXBA_bit_is_set (FIRST_1 [-TOK [i+2]], next_state))
				continue;
			}
		    }

		    break;
		}

		if (Y == 0)
		{
		    /* next_state = A -> \alpha X  ai+1 \beta .  et \beta =>* \epsilon */
		    is_scan_reduce = true;

		    A = lhs [prolis [state]];
		    OR (ntXindex_set [A], index_set);

		    if (is_parser)
		    {
			/* index_set et backward_index_set == NULL apres transition sur un terminal */
			T2_shift_state_stack [next_state] = T2_reduce_NT_hd [A];
			T2_reduce_NT_hd [A] = next_state;

			if (is_epsilon)
			{
			    if (lispro [next_state-1] > 0)
			    {
				T2_index_sets [next_state] = index_set;
				backward_index_set = T2_backward_index_sets [next_state] = bag_get (&shift_bag, i+1);
				SXBA_1_bit (backward_index_set, i);
			    }
			}
		    }
		}
	    } while ((state = T1_shift_state_stack [state]) != 0);
	}
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
	    /* A -> \alpha . X beta et \alpha =>* \epsilon */
	    X = lispro [state];

	    if (X < 0)
	    {
		next_state = state + 1;

		if (!SXBA_bit_is_set (FIRST_1[-TOK [i+2]], next_state) ||
		    ((prdct_no = prdct [state]) != -1 &&
		    is_prdct_fun &&
		    !(*for_parsact.prdct) (-i-1, prdct_no)))
		    /* Le look-ahead ou le predicat a echoue' */
		    continue;
	    }
	    else
	    {
		if (X > 0)
		{
		    if (T1_index_sets [state] == NULL)
		    {
			/* c'est la 1ere fois qu'on rencontre state */
			T1_shift_state_stack [state] = T1_shift_NT_hd [X];
			T1_shift_NT_hd [X] = state;
		    }
		}
		else
		{
		    /* X == 0 */
		    /* is_epsilon == true */
		    /* state = A -> \alpha . et \alpha =>* \epsilon */
		    /* Pour l'instant, on stocke les contraintes sur le vide
		       qui seront evaluees et exploitees plus tard. */
		    A = lhs [prolis [state]];

		    if (is_constraint_fun &&
			constraints [A] != -1)
			SXBA_1_bit (constraint_to_be_checked [i], A);

		    if (is_parser)
		    {
			/* On stocke dans T1 les reductions vides. */
			/* On est en position reduce. */
			/* state a deja pu etre trouve par un "reduce ()" precedent. */
			
			if (T1_index_sets [state] == NULL)
			{
			    T1_shift_state_stack [state] = T1_reduce_NT_hd [A];
			    T1_reduce_NT_hd [A] = state;
			}
		    }
		}
	    }

	    if (is_epsilon)
	    {
		if ((X != 0 || is_parser) && lispro [state-1] > 0)
		{
		    /* Le symbole precedent est un nt */
		    /* On positionne index_set */
		    index_set = T1_index_sets [state];

		    if (is_parser)
		    {
			if (index_set == NULL)
			    backward_index_set = T1_backward_index_sets [state] = bag_get (&shift_bag, i+1);
			else
			    backward_index_set = T1_backward_index_sets [state];

			SXBA_1_bit (backward_index_set, i);
		    }

		    if (index_set == NULL)
			index_set = T1_index_sets [state] = bag_get (&shift_bag, i+1);

		    SXBA_1_bit (index_set, i);
		}
	    }

	    if (X < 0)
	    {
		/* X == ai+1 */
		/* state = A -> \alpha . ai+1 beta et \alpha =>* \epsilon */
		/* next_state = A -> \alpha ai+1 . beta et \alpha =>* \epsilon */
		/* le look-ahead de next_state est teste' */
		is_tok = true;

		while ((Y = lispro [next_state]) != 0)
		{
		    if (Y < 0)
		    {
			/* Y == ai+2 */
			/* next_state = A -> \alpha ai+1 \beta . ai+2 \gamma et \alpha \beta =>* \epsilon */
			if (!SXBA_bit_is_set (FIRST_1[-TOK [i+3]], next_state+1) ||
			    ((prdct_no = prdct [next_state]) != -1 &&
			     is_prdct_fun &&
			     !(*for_parsact.prdct) (-i-2, prdct_no)))
			{
			    /* Le look-ahead ou le predicat a echoue */
			    break;
			}
		    }

		    if (Y > 0)
		    {
			YY = Y;

			if ((hd = T2_shift_NT_hd [Y]) == 0)
			    PUSH (T2_shift_NT_stack, Y);
		    }
		    else
		    {
			hd = T2_shift_NT_hd [0];
			YY = 0;
		    }

		    T2_shift_state_stack [next_state] = hd;
		    T2_shift_NT_hd [YY] = next_state;

		    index_set = T2_index_sets [next_state];
		    index_set2 = T2_index_sets [next_state] = bag_get (&shift_bag, i+1);

		    if (index_set != NULL)
			OR (index_set2, index_set);

		    SXBA_1_bit (index_set2, i);

		    if (is_epsilon)
		    {
			if (is_parser && lispro [next_state-1] > 0)
			{
			    /* backward_index_set n'a pas a etre recopie'. */
			    if ((backward_index_set = T2_backward_index_sets [next_state]) == NULL)
				backward_index_set = T2_backward_index_sets [next_state] = bag_get (&shift_bag, i+1);

			    SXBA_1_bit (backward_index_set, i);
			}

			if (Y > 0 && nt2eps [Y])
			{
			    next_state++;

			    if (SXBA_bit_is_set (FIRST_1 [-TOK [i+2]], next_state))
				continue;
			}
		    }

		    break;
		}

		if (Y == 0)
		{
		    /* next_state = A -> \alpha ai+1 \beta .  et \alpha \beta =>* \epsilon */
		    is_scan_reduce = true;

		    A = lhs [prolis [state]];
		    SXBA_1_bit (ntXindex_set [A], i);

		    if (is_parser)
		    {
			/* index_set et backward_index_set == NULL apres transition sur un terminal */
			T2_shift_state_stack [next_state] = T2_reduce_NT_hd [A];
			T2_reduce_NT_hd [A] = next_state;

			if (is_epsilon)
			{
			    if (lispro [next_state-1] > 0)
			    {
				T2_index_sets [next_state] = index_set;
				backward_index_set = T2_backward_index_sets [next_state] = bag_get (&shift_bag, i+1);
				SXBA_1_bit (backward_index_set, i);
			    }
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




static int recognize ()
{
    int			i;
    int			*T0_shift_NT_stack;
    bool		is_in_LG;
    SXBA		index_set;

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
	if (is_epsilon)
	{
	    T1_backward_index_sets = &(PT [0].backward_index_sets [0]);
	    T1_reduce_NT_hd = &(PT [0].reduce_NT_hd [0]);
	}

	T2_backward_index_sets = &(PT [1].backward_index_sets [0]);
	T2_reduce_NT_hd = &(PT [1].reduce_NT_hd [0]);
    }

    PUSH (T1_shift_NT_stack, 1 /* start symbol */);
    T1_shift_NT_hd [1 /* start symbol */] = initial_state;
    /* T1_shift_state_stack [initial_state] = 0; pas de suivant */

    i = 0;

    if (SXBA_bit_is_set (FIRST_1 [-TOK [1]], initial_state))
    {
	if (is_epsilon)
	{
	    if (n == 0 && nt2eps [1])
	    {
		/* un peu bidouilleux!! */
		index_set = T2_index_sets [2] = bag_get (&shift_bag, 1);
		SXBA_1_bit (index_set, 0);
	    }
	}

	while (complete (i) && ++i <= n)
	{
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
		if (is_epsilon)
		{
		    T1_backward_index_sets = T2_backward_index_sets;
		    T1_reduce_NT_hd = T2_reduce_NT_hd;
		}

		T2_backward_index_sets = &(PT [i+1].backward_index_sets [0]);
		T2_reduce_NT_hd = &(PT [i+1].reduce_NT_hd [0]);
	    }
	}
    }

    is_in_LG = T1_index_sets [2] != NULL && SXBA_bit_is_set (T1_index_sets [2], 0);
    
    return is_in_LG ? n+2 : i+1;
}




/******************************************************************************************

                             P A R S E R


 ******************************************************************************************/

#if 0
#if EBUG
static void output_spf ()
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


void erase_elementary_tree (int elementary_tree)
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

static void useless_symbol_elimination ()
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

#endif

static void ARN_sem_init (int size)
{
    Aij_sem_lhs_set = sxba_calloc (size);
    ARN_sem = (struct ARN_sem*) sxalloc (size, sizeof (struct ARN_sem));
}


static void ARN_sem_final (int size)
{
#if EBUG
    int Aij_unused;
	
    Aij_unused = (size - sxba_cardinal (Aij_sem_lhs_set));

    printf ("Aij_elems = %i, Aij_unused = %i(%i%%)\n",
	    size, Aij_unused, 100*Aij_unused/size);
#endif

    sxfree (Aij_sem_lhs_set);
    sxfree (ARN_sem);
}


static bool ARN_constraint (int prdct_no, int A, int i, int j)
{
    (void) A;
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


static bool RL_mreduction_item (int item_j, SXBA I, int j)
{
    /* item_j = A -> \alpha . \gamma  et \alpha != \epsilon */
    int				Y, bot_item, k, new_k, nbt, new_item, item_k, item, order;
    SXBA			backward_index_set, prev_index_set;
    SXBA			/* tbp_set, */ ap_set;
    struct recognize_item	*RTj;
    struct parse_item		*PTj;
    bool			is_tbp, is_new;

    SXBA			ids, nbis;

    bot_item = prolon [prolis [item_j]];

    PTj = &(PT [j]);

    item_k = item_j - 1;
    Y = lispro [item_k];
    /* item_k = X -> \alpha . Y x \gamma */

#if EBUG
    if (Y <= 0)
	sxtrap (ME, "RL_mreduction_item");
#endif

    ap_set = ap_sets [Y];

    if ((is_new = SXBA_bit_is_reset_set (RL_nt_set, Y)))
    {
	ap_set [0] = j+1; /* On change la taille!! */

	RTj = &(RT [j]);

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
	is_tbp = false;
	nbt = item2nbt [item_k];
	new_item = item_k - nbt;

	nbis = PT2 [j].backward_index_sets [item_j];

	backward_index_set = PTj->backward_index_sets [item_j];
	    
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
#if 0
		/* On ne doit pas le supprimer, il peut servir + tard. */
		SXBA_0_bit (backward_index_set, k); /* inutile de le reessayer + tard, il a marche */
#endif
		if (nbis == NULL)
		    nbis = PT2 [j].backward_index_sets [item_j] = bag_get (&pt2_bag, j+1);

		SXBA_1_bit (nbis, k);

		if ((is_new || !SXBA_bit_is_set (ap_set, k)))
		{
		    SXBA_1_bit (ap_set, k);
		    is_tbp = true;
		}

		if (new_item > bot_item)
		{
		    /* new_item pointe derriere un nt. */
		    new_k = k-nbt;

		    if (new_k == j)
		    {
			/* is_epsilon == true */
			if ((ids = PT2 [new_k].index_sets [new_item]) == NULL)
			    ids = PT2 [new_k].index_sets [new_item] = bag_get (&pt2_bag, j+1);

#if 0
			if (OR_AND_MINUS (ids, prev_index_set, I, wis [item2nbnt [new_item]-1]))
#endif
			if (OR_AND (ids, prev_index_set, I))
			{
			    /* Il y a du nouveau */
			    if (RL_mreduction_item (new_item, ids, j))
				is_tbp = true;
			}
		    }
		    else
		    {
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
    }
	
    return is_tbp;
}

static void RL_mreduction ()
{
    int				j, A, order, tnb;
    SXBA			ap_set;
    struct parse_item		*PTj;
    struct recognize_item	*RTj;
    struct PT2_item		*PT2j;

    int				*PT2j_shift_NT_stack, x, item, new_item, new_j, prod;
    SXBA			*PT2j_index_set, *PT2j_backward_index_set, I, ids;
    SXBA			index_set, new_index_set, red_order_set;
    bool			is_good;

    RL_nt_set = sxba_calloc (ntmax+1);
    prod_order_set = sxba_calloc (prodmax+1);

    index_set = PT2 [n].index_sets [2] = bag_get (&pt2_bag, n+1);
    SXBA_1_bit (index_set, 0); /* {0} */

#if 0
    if (sxba_scan (T1_index_sets [2], 0) > 0)
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

	    is_good = false;
				
	    if (tnb == 0)
	    {
		if (is_epsilon && item == prolon [prod])
		{
		    /* item = A -> . */
		    if (SXBA_bit_is_set (ap_set, j))
			is_good = true;
		}
		else
		{
		    /* item = A -> \alpha B . */
		    index_set = RTj->index_sets [item];

		    if ((new_index_set = PT2j->index_sets [item]) == NULL)
			    PT2j->index_sets [item] = new_index_set = bag_get (&pt2_bag, j+1);

		    if (OR_AND_MINUS (new_index_set, index_set, ap_set, wis [item2nbnt [item]-1]))
			/* calcule wis = index_set & ap_set - new_index_set
			           new_index_set |= wis
			   retourne vrai ssi wis est non vide */
		    {
			/* Il y a du nouveau */
			if (RL_mreduction_item (item, new_index_set, j))
			{
			    /* ap_sets [B] a ete modifie */
			    /* rupture de sequence (eventuel) calcule statiquement */
			    order = prod_order2next [order];
			}
			/* else order est pris en sequence */

			is_good = true;
		    }

#if 0
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
				order = prod_order2next [order];
			    }
			    /* else order est pris en sequence */

			    is_good = true;
			}
		    }
#endif		    
		}
	    }
	    else
	    {
		new_j = j - tnb;

		if (prolon [prod] == (new_item = item - tnb))
		{
		    /* Que des t en rhs */
		    if (SXBA_bit_is_set (ap_set, new_j))
			is_good = true;
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
			    is_good = true;
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



static void print_symb (int symb, int i, int j)
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

static void print_prod (int i, int k, ushort *tmp_prod_core)
{
    int		prod, h, B, j, A, x, item, nbnt;

    prod = tmp_prod_core [0];
    A = lhs [prod];
    xprod++;
    printf ("%i\t: ", xprod);
    print_symb (A, i, k);
    fputs ("\t = ", stdout);

    nbnt = prod2nbnt [prod];
    x = 1;
    j = i;
    item = prolon [prod];

    while ((B = lispro [item++]) != 0)
    {
	h = (B > 0) ? ((x < nbnt) ? tmp_prod_core [x++] : k - prodXnt2nbt [prod][0]) : j+1;
	print_symb (B, j, h);
	j = h;
    }
    
#if EBUG
    if (j != k)
	sxtrap (ME, "print_prod");
#endif

    fputs (";\n", stdout);

}

static void ARN_ksem_init (int size)
{
    int	x;

    Aij_sem_lhs_set = sxba_calloc (size);
    ARN_disp_ksem = (struct ARN_ksem**) sxalloc (size, sizeof (struct ARN_ksem*));
    ARN_ksem_top = ARN_ksem = (struct ARN_ksem*) sxalloc (ARN_ksem_size = max_tree_nb*size, sizeof (struct ARN_ksem));
    ARN_ksem_is_marked = sxba_calloc (ARN_ksem_size);

    disp_kvals = (struct kvals**) sxalloc (max_tree_nb, sizeof (struct kvals*));
    kvals = (struct kvals*) sxalloc (max_tree_nb, sizeof (struct kvals));

    for (x = 0; x < max_tree_nb; x++)
	disp_kvals [x] = &(kvals [x]);
}


static void ARN_ksem_final (int size)
{
#if EBUG
    int Aij_unused, unused, Aij, x;
	
    Aij_unused = (size - sxba_cardinal (Aij_sem_lhs_set));

    unused = Aij_unused * max_tree_nb;
    Aij = -1;

    while ((Aij = sxba_scan (Aij_sem_lhs_set, Aij)) >= 0)
    {
	for (x = max_tree_nb -1; x >= 0; x--)
	{
	    if (ARN_disp_ksem [Aij][x].val != -1)
		break;

	    unused++;
	}
    }

    printf ("Aij_elems = %i, Aij_unused = %i(%i%%), ARN_ksem_elems = %i, ARN_ksem_unused = %i (%i%%)\n",
	    size, Aij_unused, 100*Aij_unused/size, max_tree_nb*size, unused, 100 * unused / (max_tree_nb*size));
#endif

    sxfree (Aij_sem_lhs_set);
    sxfree (ARN_disp_ksem);
    sxfree (ARN_ksem);
    sxfree (disp_kvals);
    sxfree (ARN_ksem_is_marked);
    sxfree (kvals);
}

static unsigned long	ksem_vector;

static bool ksem_incr (int nbnt, int *tmp_rhs_stack)
{
    int x, i, shift;

    shift = 0;

    for (x = 0; x < nbnt; x++, shift += log_max_tree_nb)
    {
	i = (ksem_vector >> shift) & filtre_max_tree_nb;
	ksem_vector &= ~(filtre_max_tree_nb << shift);

	i++;

	if (i < max_tree_nb && ARN_disp_ksem [tmp_rhs_stack [x+1]][i].val != -1)
	{
	    ksem_vector |= (i << shift);
	    return true;
	}
    }

    return false;
}

static int ARN_keval (int Aij, int i, int j, int *tmp_rhs_stack)
{
    /* remplit les [0..top-1] premiers elements de kvals */
    /* ksem_vector == 0 */
    int			top, x, min_x, prod, nbnt, Bhk, shift, vectx;
    float		val, min_val, cur_val, proba;
    struct ARN_ksem	*psem;

    (void) Aij;

    top = 0;

    prod = tmp_rhs_stack [0];
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
	    Bhk = tmp_rhs_stack [1];

	    for (psem = ARN_disp_ksem [Bhk], top = 0;
		 (val = psem->val) != -1 && top < max_tree_nb;
		 psem++, top++)
	    {
		disp_kvals [top]->val = psem->val*proba;
		disp_kvals [top]->vector = top;
	    }
	}
	else
	{
	    top = 0;

	    do
	    {
		val = proba;
		shift = 0;

		for (x = 0; x < nbnt; x++, shift += log_max_tree_nb)
		{
		    vectx = (ksem_vector >> shift) & filtre_max_tree_nb;
		    val *= ARN_disp_ksem [tmp_rhs_stack [x+1]][vectx].val;
		}

		/* Si val est mieux, on le place */

		if (top < max_tree_nb)
		{
		    disp_kvals [top]->val = val;
		    disp_kvals [top]->vector = ksem_vector;
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
			disp_kvals [min_x]->vector = ksem_vector;
		    }
		}

	    } while (ksem_incr (nbnt, tmp_rhs_stack));
	}
    }

    return top;
}


#if 0
static int	ksem_vector [rhs_maxnt];
static bool ksem_incr (int nbnt, int *tmp_rhs_stack)
{
    int x, i;

    for (x = nbnt-1; x >= 0; x--)
    {
	i = ++ksem_vector [x];

	if (i == max_tree_nb || ARN_disp_ksem [tmp_rhs_stack [x+1]][i].val == -1)
	    ksem_vector [x] = 0;
	else
	{
	    ksem_vector [x] = i;
	    return true;
	}
    }

    return false;
}


static int ARN_keval (int prod, int i, int j, int *tmp_rhs_stack)
{
    /* remplit les [0..top-1] premiers elements de kvals */
    /* ksem_vector == 0 */
    int			top, x, min_x, prod, nbnt, Bhk;
    float		val, min_val, cur_val, proba;
    struct ARN_ksem	*psem;

    top = 0;

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
	    Bhk = tmp_rhs_stack [1];

	    for (psem = ARN_disp_ksem [Bhk], top = 0;
		 (val = psem->val) != -1 && top < max_tree_nb;
		 psem++, top++)
	    {
		disp_kvals [top]->val = psem->val*proba;
		disp_kvals [top]->vector [0] = top;
	    }
	}
	else
	{
	    top = 0;

	    do
	    {
		val = proba;

		for (x = 0; x < nbnt; x++)
		    val *= ARN_disp_ksem [tmp_rhs_stack [x+1]][ksem_vector [x]].val;

		/* Si val est mieux, on le place */

		if (top < max_tree_nb)
		{
		    disp_kvals [top]->val = val;

		    for (x = 0; x < nbnt; x++)
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

			for (x = 0; x < nbnt; x++)
			    disp_kvals [min_x]->vector [x] = ksem_vector [x];
		    }
		}

	    } while (ksem_incr (nbnt, tmp_rhs_stack));
	}
    }

    return top;
}
#endif


static bool ARN_kparsact (int i, int j, ushort *tmp_prod_core, int tmp_rhs_stack[])
{
    int			Aij, nbnt, x, top, prod;
    ushort		*p, *q;
    float		val;
    struct ARN_ksem	*psem, *psem2, *Aij_disp_ksem;
    
    prod = tmp_prod_core [0];

    if ((top = ARN_keval (prod, i, j, tmp_rhs_stack)) > 0)
    {
	/* kvals est positionne */
	Aij = tmp_rhs_stack [0];

	if (SXBA_bit_is_reset_set (Aij_sem_lhs_set, Aij))
	{
	    /* 1er calcul correct sur Aij en lhs */
	    Aij_disp_ksem = ARN_disp_ksem [Aij] = ARN_ksem_top;

	    for (x = 1; x <= max_tree_nb; x++)
	    {
		ARN_ksem_top->val = -1; /* empty */
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
		nbnt = prod2nbnt [prod];

		if (nbnt == 0)
		    psem2->prod_core [0] = tmp_prod_core [0];
		else
		{
		    psem2->vector = disp_kvals [top]->vector;
		    
		    nbnt--;
		    p = &(tmp_prod_core [nbnt]), q = &(psem2->prod_core [nbnt]);

		    do
		    {
			*q-- = *p--;
		    } while (p >= tmp_prod_core);
		}
	    }
	}

	return true;
    }

    return false;
}


static void ksem_td_mark (int Aij, int i, int j, int x)
{
    int			nbnt, h, k, delta, item, B, Bhk, prod, xsem, shift;
    struct ARN_ksem	*p;
    ushort		*tmp_prod_core;

    p = &(ARN_disp_ksem [Aij][x]);

    xsem = p - ARN_ksem;

#if EBUG
    if (xsem >= ARN_ksem_size)
	sxtrap (ME, "ksem_td_mark");
#endif

    if (SXBA_bit_is_reset_set (ARN_ksem_is_marked, xsem))
    {
	SXBA_1_bit (Aij_set, Aij);
	tmp_prod_core = &(p->prod_core [0]);
	prod = tmp_prod_core [0];
	nbnt = prod2nbnt [prod];

	if (nbnt > 0)
	{
	    k = (nbnt == 1) ? (j-prodXnt2nbt [prod][0]) : tmp_prod_core [1];

	    delta = prodXnt2nbt [prod][1];
	    h = i + delta;
	    item = prolon [prod] + delta;
	    B = lispro [item];
	    Bhk = MAKE_Aij (MAKE_A0j (B, k), h);

	    ksem_td_mark (Bhk, h, k, p->vector & filtre_max_tree_nb);

	    if (nbnt > 1)
	    {
		h = k;
		shift = 0;

		for (x = 2; shift += log_max_tree_nb, x < nbnt; x++)
		{
		    delta = prodXnt2nbt [prod][x];
		    h += delta;
		    k = tmp_prod_core [x];
		    item += delta+1;
		    B = lispro [item];
		    Bhk = MAKE_Aij (MAKE_A0j (B, k), h);
		    ksem_td_mark (Bhk, h, k, (p->vector >> shift) & filtre_max_tree_nb);
		    h = k;
		}
	    
		delta = prodXnt2nbt [prod][nbnt];
		h += delta;
		k = j - prodXnt2nbt [prod][0];
		item += delta+1;
		B = lispro [item];
		Bhk = MAKE_Aij (MAKE_A0j (B, k), h);
		ksem_td_mark (Bhk, h, k, (p->vector >> shift) & filtre_max_tree_nb);
	    }
	}
    }
}

static void ksem_bu_extract ()
{
    /* Extrait les productions valides de la foret partagee. */
    int			i, j, order, A, A0j, Aij, prod, nbnt, x, y, z, xsem, xsem2;
    struct ARN_ksem	*psem, *psem2, *Aij_disp_ksem;

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
		xsem = Aij_disp_ksem - ARN_ksem;

		for (psem = Aij_disp_ksem, x = 0; x < max_tree_nb; psem++, x++, xsem++)
		{
		    if (psem->val == -1)
			break;

		    if (SXBA_bit_is_set (ARN_ksem_is_marked, xsem))
		    {
			prod = psem->prod_core [0];
			nbnt = prod2nbnt [prod];

			if (is_print_prod)
			    print_prod (i, j, &(psem->prod_core [0]));
			else
			    xprod++;

			psem2 = psem;
			xsem2 = xsem;
			y = x;

			while (++y < max_tree_nb)
			{
			    psem2++;
			    xsem2++;

			    if (psem2->val == -1)
				break;

			    if (SXBA_bit_is_set (ARN_ksem_is_marked, xsem2) && psem2->prod_core [0] == prod)
			    {
				for (z = nbnt-1; z > 0; z--)
				{
				    if (psem->prod_core [z] != psem2->prod_core [z])
					break;
				}

				if (z == 0)
				    SXBA_0_bit (ARN_ksem_is_marked, xsem2);
			    }
			}
		    }
		}

#if EBUG
		if (xsem > ARN_ksem_size)
		    sxtrap (ME, "ksem_bu_extract");
#endif

	    }
	}
    }
}


static bool ARN_eval (int prod, int i, int j, int *tmp_rhs_stack, float *val)
{
    int		Aij, nbnt, Bhk;
    float	proba;

    Aij = tmp_rhs_stack [0];
    nbnt = prod2nbnt [prod];
    proba = probabilities [prod] * (j-i);;

    if (nbnt > 0)
    {
	do
	{
	    Bhk = tmp_rhs_stack [nbnt];
	    proba *= ARN_sem [Bhk].val;
	} while (--nbnt > 0);
    }

    *val = proba;

    return true;
}



static bool ARN_parsact (int i, int j, ushort *tmp_prod_core, int tmp_rhs_stack[])
{
    int 	Aij, prod;
    int		nbnt;
    float	val;
    ushort	*p, *q;
    
    prod = tmp_prod_core [0];

    if (ARN_eval (prod, i, j, tmp_rhs_stack, &val))
    {
	/* val est positionne */
	if (SXBA_bit_is_reset_set (Aij_sem_lhs_set, Aij) /* 1er calcul correct sur Aij en lhs */ ||
	    ARN_sem [Aij].val < val /* Le calcul courant est meilleur */)
	{
	    /* On stocke val et la prod_core */
	    ARN_sem [Aij].val = val;
	    nbnt = prod2nbnt [prod];

	    if (nbnt > 0)
		nbnt--;

	    p = &(tmp_prod_core [nbnt]), q = &(ARN_sem [Aij].prod_core [nbnt]);

	    do
	    {
		*q-- = *p--;
	    } while (p >= tmp_prod_core);
	}

	return true;
    }

    return false;
}


#if 0
static void print_SXBA (SXBA set)
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


static bool parse_item (int A0k, int item, SXBA I, int i_left, int j, int k, int nbnt)
{
    /* A0k n'est significatif que ds le cas "EBUG" ou for_parsact.action != NULL. */
    /* rhs_stack n'est rempli que si for_parsact.action != NULL. */
    /* item = A -> \alpha . B \beta */
    /* Si I == NULL, i_left est positionne */
       
    int		i, new_i, nbt, B0j, Bij, Aik, prod, prdct_no;
    bool     ret_val = false, is_ok, is_prdct;
    SXBA        backward_J, ap_set, index_set;

    backward_J = PT2 [j].backward_index_sets [item+1];

    nbt = item2nbt [item];
    prod = prolis [item];

    /* Ca permet d'appeler la semantique par defaut (comptage des arbres) meme si certaines regles
       n'ont pas d'actions. */
    is_prdct = (is_prdct_fun && (prdct_no = prdct [item]) != -1);

#if EBUG
    B0j = MAKE_A0j (lispro [item], j);
#else
    if (is_action_fun || is_prdct)
	B0j = MAKE_A0j (lispro [item], j);
#endif

    if (prod2nbnt [prod] > nbnt)
	prod_core [nbnt] = j;

    if (nbnt == 1)
    {
#if EBUG
	if (I!= NULL && prod2nbnt [prod] == 1 && NON_EQUAL (I, PT2 [j].index_sets [item+1]))
	    sxtrap (ME, "parse_item");

	/* La propriete precedente n'est pas vraie si \beta contient (au moins) un nt.
	   Pour le k donne, le traitement RL a pu restreindre les valeurs de i pour Aik
	   alors que pour un autre k' ca marche. Ex:
	   Aik -> Bij Cjk et Ai'k' -> Bi'j Cjk' peuvent etre corrects bien que
	   Ai'k -> Bi'j Cjk est faux car Ai'k est non accessible depuis l'axiome. */
#endif

	if (I != NULL)
	{
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

	    new_i = sxba_1_rlscan (index_set, j+1);
	}
	else
	{
	    index_set = NULL;
	    new_i = i_left;
	}

	ap_set = ap_sets [lhs [prod]];

	while (new_i >= 0)
	{
	    i = new_i + nbt;

	    /* Dans tous les cas Bij a une valeur semantique differente de bottom. */
	    /* Cette valeur peut eventuellement encore evoluer ds le cas de grammaires cycliques!.
	       Mais il ne faut pas regenerer les memes productions, seulement reevaluer la sem. */
#if EBUG
	    Bij = MAKE_Aij (B0j, i);
	    Aik = MAKE_Aij (A0k, new_i);

	    if (!SXBA_bit_is_set (Aij_lhs_set, Bij))
		sxtrap (ME, "parse_item");

	    SXBA_1_bit (Aij_rhs_set, Bij);

	    if (SXBA_bit_is_set (Aij_rhs_set, Aik))
		sxtrap (ME, "parse_item");

	    SXBA_1_bit (Aij_lhs_set, Aik);
#endif

	    if (is_action_fun || is_prdct)
		Bij = MAKE_Aij (B0j, i);

	    if (!is_prdct || (*for_parsact.prdct) (Bij, prdct_no))
	    {
		if (is_print_prod)
		    print_prod (new_i, k, prod_core);
		else
		    xprod++;

		if (is_action_fun)
		{
		    rhs_stack [1] = MAKE_Aij (B0j, i);
		    rhs_stack [0] = MAKE_Aij (A0k, new_i);
		    is_ok = (*for_parsact.action) (new_i, k, prod_core, rhs_stack);
		}
		else
		    is_ok = true;
	    
		if (is_ok && SXBA_bit_is_reset_set (ap_set, new_i))
		    ret_val = true;
	    }

	    if (index_set != NULL)
		new_i = sxba_1_rlscan (index_set, new_i);
	    else
		new_i = -1;
	}
    }
    else
    {
	item -= nbt;

	i = j+1;
	
	while ((i = sxba_1_rlscan (backward_J, i)) >= 0)
	{
#if EBUG
	    Bij = MAKE_Aij (B0j, i);

	    if (!SXBA_bit_is_set (Aij_lhs_set, Bij))
		sxtrap (ME, "parse_item");

	    SXBA_1_bit (Aij_rhs_set, Bij);
#endif

	    if (is_action_fun || is_prdct)
		Bij = MAKE_Aij (B0j, i);

	    if (!is_prdct || (*for_parsact.prdct) (Bij, prdct_no))
	    {
		if (is_action_fun)
		    rhs_stack [nbnt] = Bij;

		if (parse_item (A0k, item-1, I, i_left, i - nbt, k, nbnt-1))
		    ret_val = true;
	    }
	}
    }

    return ret_val;		/* ap_sets [A] a ete modifie */
}


static SXBA	cycle_red_order_set;

#if 0
static SXBA	cycle_nt_set;

cyclic_sem (int j, int init_prod_order, int *out_prod_order)
{
    /* C'est la semantique qui indique la fin du traitement d'un cycle */
    SXBA 		backward_index_set, red_order_set;

    red_order_set = red_order_sets [j];

    prod_order = init_prod_order;

    do
    {
	if (SXBA_bit_is_set (red_order_set, prod_order))
	{
	    prod = order2prod [prod_order];
	    A = lhs [prod];
	    SXBA_1_bit (cycle_nt_set, A);
	}

	next_prod_order = cyclic_parse_order2next [prod_order];

	if (next_prod_order >= prod_order)
	    /* Sortie de la boucle */
	    *out_prod_order = prod_order;

	prod_order = next_prod_order;
    }  while (prod_order != init_prod_order);

    PT2j = &(PT2 [j]);
    prod_order = init_prod_order;
    first_loop = true;

    while (first_loop)
    {
	first_loop = false;

	do
	{
	    if (SXBA_bit_is_set (red_order_set, prod_order))
	    {
		prod = order2prod [prod_order];
		A = lhs [prod];

		if (is_action_fun)
		    A0j = MAKE_A0j (A, j);

		item = prolon [prod+1] - 1;
		left_item = prolon [prod];
		prod_core [0] = prod;

		while ((B = lispro [--item]) > 0)
		{
		    backward_index_set = PT2j->backward_index_sets [item+1];

		    if (is_action_fun)
			B0j = MAKE_A0j (B, j);

		    if (SXBA_bit_is_set (cycle_nt_set, B))
		    {
			if (AND3 (level0_index_set, backward_index_set, ap_sets [B]))
			{
			    i = -1;

			    while ((i = sxba_scan_reset (level0_index_set, i)) >= 0)
			    {

				if (item == left_item)
				{
				    /* item = A -> . B et B est ds le cycle et (partiellement) calcule */
				    if (is_print_prod)
					print_prod (i, j, prod_core);
				    else
					xprod++;

				    if (is_action_fun)
				    {
					rhs_stack [1] = Bij = MAKE_Aij (B0j, i);
					rhs_stack [0] = MAKE_Aij (A0j, i);
					is_ok = (*for_parsact.action) (i, j, prod_core, rhs_stack);

					if (is_ok)
					{
					    SXBA_1_bit (cycle_red_order_set, prod_order);
					    should_loop = true;
					}
					else
					    SXBA_0_bit (backward_index_set, i);
				    }
				    else
					is_ok = true;
	    
				    if (is_ok)
					SXBA_1_bit (ap_sets [A], i);
				}
				else
				{
				    /* item = A -> \alpha X . B et B est ds le cycle et (partiellement) calcule */

				    index_set = PT2 [i].index_sets [item];

				    if (SXBA_bit_is_set (index_set, i))
				    {
					/* \alpha X =>+ \epsilon */
					/* Attention, si i == j, ds \alpha X il peut y avoir des nt du cycle qui ne
					   sont pas du tout calcule's */
					delta = item - left_item;

					if (is_action_fun)
					    rhs_stack [delta+1] = Bij = MAKE_Aij (B0j, i);

					prod_core [delta] = i;

					while ((B = lispro [--item]) > 0)
					{
					    if (i != j || SXBA_bit_is_set (ap_sets [B], i))
					    {
						B0i = MAKE_A0j (B, i);
						delta--;

						if (is_action_fun)
						    rhs_stack [delta+1] = MAKE_Aij (B0i, i);

						if (delta > 0)
						    prod_core [delta] = i;
					    }
					    else
						break;
					}

					if (B == 0)
					{
					    if (is_print_prod)
						print_prod (i, j, prod_core);
					    else
						xprod++;

					    if (is_action_fun)
					    {
						rhs_stack [0] = MAKE_Aij (A0j, i);
						is_ok = (*for_parsact.action) (i, j, prod_core, rhs_stack);

						if (is_ok)
						{
						    SXBA_1_bit (cycle_red_order_set, prod_order);
						    should_loop = true;
						    /* (prod, i) est cyclique, il faut reexecuter
						       la semantique dessus. */
						}
						else
						    SXBA_0_bit (backward_index_set, i);
					    }
					    else
						is_ok = true;
	    
					    if (is_ok)
						SXBA_1_bit (ap_sets [A], i);

					    /* Ne doit pas etre reexamine' */
					    /* A FAIRE */
					}
					else
					    /* sera genere au prochain tour de boucle. */
					    first_loop = true;
				    }

				    /* item = A -> \alpha X . B et B est ds le cycle et (partiellement) calcule */
				    /* Les elements de index_set differents de i ne peuvent etre impliques
				       ds un cycle (mais ils peuvent etre mutuellement recursifs). */
				    /* A FAIRE */
				}
			    }
			}
		    }
		    else
		    {
			if (SXBA_bit_is_set (backward_index_set, j))
			{
			    /* Bjj est entierement calcule */
			    delta = item - left_item;

			    if (is_action_fun)
				rhs_stack [delta+1] = MAKE_Aij (B0j, j);

			    prod_core [delta] = j;
			}

			/* Les autres elements ne font pas partie du cycle */
			/* A FAIRE */
		    }
		}
	    }
	}  while (prod_order != init_prod_order);
    }
}
#endif


#if 0
/* A FAIRE. CYCLIQUE NE MARCHE PAS. */
static void cyclic_sem (int j, int init_prod_order, int *out_prod_order)
{
    /* C'est la semantique qui indique la fin du traitement d'un cycle */
    int			next_prod_order, prod_order, prod, item, B, B0j, Bij, A, A0j, Aij, i;
    SXBA 		backward_index_set, red_order_set;
    struct PT2_item	*PT2j;
    bool		is_ok, should_loop = false;

    red_order_set = red_order_sets [j];
    PT2j = &(PT2 [j]);

    /* On fait le premier tour de boucle */
    prod_order = init_prod_order;

    do
    {
	if (SXBA_bit_is_set (red_order_set, prod_order))
	{
	    /* on parcourt le cycle */
	    prod = order2prod [prod_order];

	    item = prolon [prod+1]-1;
	    B = lispro [item-1];
	    B0j = MAKE_A0j (B, j);
	    A = lhs [prod];
	    A0j = MAKE_A0j (A, j);
	    backward_index_set = PT2j->backward_index_sets [item];
	    prod_core [0] = prod;
	    i = -1;

	    while ((i = sxba_scan (backward_index_set, i)) >= 0)
	    {
		if (is_print_prod)
		    print_prod (i, j, prod_core);
		else
		    xprod++;

		if (is_action_fun)
		{
		    rhs_stack [1] = Bij = MAKE_Aij (B0j, i);
		    rhs_stack [0] = MAKE_Aij (A0j, i);
		    is_ok = (*for_parsact.action) (i, j, prod_core, rhs_stack);

		    if (is_ok)
		    {
			SXBA_1_bit (cycle_red_order_set, prod_order);
			should_loop = true;
		    }
		    else
			SXBA_0_bit (backward_index_set, i);
		}
		else
		    is_ok = true;
	    
		if (is_ok)
		    SXBA_1_bit (ap_sets [A], i);
	    }
	    
	    /* cyclic_parse_order2next reference les bons, changer parse_order2next de facon analogue. */
	    next_prod_order = cyclic_parse_order2next [prod_order];

	    if (next_prod_order >= prod_order)
		/* Sortie de la boucle */
		*out_prod_order = prod_order;

	    prod_order = next_prod_order /* -1 */;
	}
    } while (prod_order != init_prod_order);


    /* On fait les suivants */
    while (should_loop)
    {
	should_loop = false;
	prod_order = init_prod_order+1;

	while ((prod_order = sxba_1_rlscan (cycle_red_order_set, prod_order)) > 0)
	{
	    SXBA_0_bit (cycle_red_order_set, prod_order);
	    prod = order2prod [prod_order];

	    item = prolon [prod+1]-1;
	    B = lispro [item-1];
	    B0j = MAKE_A0j (B, j);
	    A = lhs [prod];
	    A0j = MAKE_A0j (A, j);
	    backward_index_set = PT2j->backward_index_sets [item];
	    prod_core [0] = (prod | HIBITS); /* On previent la semantique qu'il y a un cycle et que
						cette production a deja ete traitee. */
	    i = -1;

	    while ((i = sxba_scan (backward_index_set, i)) >= 0)
	    {
		rhs_stack [1] = Bij = MAKE_Aij (B0j, i);
		rhs_stack [0] = MAKE_Aij (A0j, i);
		is_ok = (*for_parsact.action) (i, j, prod_core, rhs_stack);
	    
		if (is_ok)
		{
		    should_loop = true;
		    SXBA_1_bit (cycle_red_order_set, prod_order);
		}
		else
		    SXBA_0_bit (backward_index_set, i);
	    }
	}
    }
}
#endif

static SXBA		i_index_set, i2nt_nyp;
static int		**i2prod_stack, *gprod_stacks;
static int		tbp_item_stack [itemmax+1], blocked_stack [itemmax+1];


static int		maxritem, item2ritem [itemmax+1], ritem2item [itemmax+1];
static int		ritem_cycle_stack [itemmax+1];
static SXBA		k_ritem_set, ritem_cycle_set, ritem_gset, ritem_gset2, cyclic_citem_set;
static SXBA		*ritem_graph;
static int		tbp_stack [itemmax+1], citem2nb [itemmax+1],
                        cur_item2next [itemmax+1], nt2cur_item [ntmax+1];

static void found_item_cycle (int q)
{
    /* ritem_gset contient un cycle, on le complete */
    int		ritem, p;
    SXBA	ritem_set;

    SXBA_1_bit (ritem_gset2, q);

    ritem_set = ritem_graph [q];
    ritem = maxritem;

    while((ritem = sxba_1_rlscan (ritem_set, ritem)) >= 0)
    {
	if (SXBA_bit_is_set (k_ritem_set, ritem))
	{
	    if (SXBA_bit_is_set (ritem_cycle_set, ritem))
	    {
		p = maxritem;

		while ((p = sxba_1_rlscan (ritem_gset2, p)) >= 0)
		{
		    if (SXBA_bit_is_reset_set (ritem_cycle_set, p))
			PUSH (ritem_cycle_stack, p);
		}
	    }
	    else
		if (!SXBA_bit_is_set (ritem_gset2, ritem))
		    found_item_cycle (ritem);
	}
    }

    SXBA_0_bit (ritem_gset2, q);
}

static void process_item_cycle ()
{
    int ritem = maxritem;

    while ((ritem = sxba_1_rlscan (ritem_gset, ritem)) >= 0)
    {
	SXBA_1_bit (ritem_cycle_set, ritem);
	PUSH (ritem_cycle_stack, ritem);
    }

    while (!IS_EMPTY (ritem_cycle_stack))
    {
	found_item_cycle (POP (ritem_cycle_stack));
    }
}

static bool max_item_cycle (int q, int p)
{
    /* met ds ritem_cycle_set le cycle maximal autour de q */
    int		ritem;
    bool	has_cycle = false;
    SXBA	ritem_set;

    SXBA_1_bit (ritem_gset, p);

    ritem_set = ritem_graph [p];
    ritem = maxritem;

    while((ritem = sxba_1_rlscan (ritem_set, ritem)) >= 0)
    {
	if (SXBA_bit_is_set (k_ritem_set, ritem))
	{
	    if (ritem == q)
	    {
		/* on a trouve un cycle */
		process_item_cycle ();
		has_cycle = true;
		break;
	    }
	    else
	    {
		if (!SXBA_bit_is_set (ritem_gset, ritem) && max_item_cycle (q, ritem))
		{
		    has_cycle = true;
		    break;
		}
	    }
	}
    }

    SXBA_0_bit (ritem_gset, p);

    return has_cycle;
}

static void process_cycle (int k, int j)
{
    /* remplit item_graph avec tous les couples
       (Akj -> \alphakk Bkj . \betajj, Ckj -> \gammakk Akj . \deltajj) */
static int	tmp_rhs_stacks [itemmax+1][rhs_lgth+1];
static ushort	tmp_prod_cores [itemmax+1][rhs_lgth];
static int	nt2item_stack [ntmax+1] [itemmax+1], *A2item_stack;

    int			*cur_rhs_stack;
    ushort		*cur_prod_core;
    int 		item, citem, ritem, ritem2, ritem3, left_item, cur_item, right_item, prod, cur_prod,
                        A, A0j, Aij, B, B0j, Bkj, x, nbnt, l, m, prdct_no;
    int			*p;
    struct PT2_item	*PT2j;
    bool		is_high_level, should_loop, is_prdct;
    SXBA		ritem_set;

    PT2j = &(PT2 [j]);

    /* k_ritem_set, item_graph et nt2item_stack doivent etre vides. */
    maxritem = 0;
    item = 0;

    for (x = blocked_stack [0]; x > 0; x--)
    {
	item =  blocked_stack [x];

	ritem = maxritem++;
	item2ritem [item] = ritem;
	ritem2item [ritem] = item;
	PUSH (nt2item_stack [lispro [item-1]], ritem);

	if (k == j)
	{
	    /* Les items du prefixe peuvent etre impliques ds le cycle. */
	    prod = prolis [item];
	    left_item = prolon [prod];
	    cur_item = item;

	    while (--cur_item > left_item)
	    {
		B = lispro [cur_item-1];

		if (SXBA_bit_is_set (i2nt_nyp, B))
		{
		    ritem = maxritem++;
		    item2ritem [cur_item] = ritem;
		    ritem2item [ritem] = cur_item;
		    PUSH (nt2item_stack [B], ritem);
		}
	    }
	}
    }
		
    /* On change les tailles */
    k_ritem_set [0] = (SXBA_ELT) maxritem;
    sxba_fill (k_ritem_set);
    ritem_cycle_set [0] = (SXBA_ELT) maxritem;
    ritem_gset [0] = (SXBA_ELT) maxritem;
    ritem_gset2 [0] = (SXBA_ELT) maxritem;

    for (ritem =  maxritem-1; ritem >= 0; ritem--)
	ritem_graph [ritem][0] = (SXBA_ELT) maxritem;

    ritem = maxritem;

    for (ritem =  maxritem-1; ritem >= 0; ritem--)
    {
	A = lhs [prolis [ritem2item [ritem]]];
	A2item_stack = nt2item_stack [A];

	for (x = A2item_stack [0]; x > 0; x--)
	{
	    ritem2 = A2item_stack [x];
	    SXBA_1_bit (ritem_graph [ritem], ritem2);
	}
    }
	
    for (ritem =  maxritem-1; ritem >= 0; ritem--)
    {
	A = lhs [prolis [ritem2item [ritem]]];
	nt2item_stack [A] [0] = 0;
    }

    /* Il faut extraire un cycle de plus haut niveau :
       \not \exists i1 : i1 \not \in cycle et (i1, i2) et i2 \in cycle */
    
    ritem = maxritem;

    while ((ritem = sxba_1_rlscan (k_ritem_set, ritem)) >= 0)
    {
	if (max_item_cycle (ritem, ritem))
	{
	    /* ritem_cycle_set contient le cycle maximal autour de item */
	    /* On regarde si ce cycle est de plus haut niveau */

	    is_high_level = true;

	    if (MINUS (k_ritem_set, ritem_cycle_set))
	    {
		ritem2 = maxritem;

		while ((ritem2 = sxba_1_rlscan (k_ritem_set, ritem2)) >= 0)
		{
		    ritem_set = ritem_graph [ritem2];
		    ritem3 = maxritem;

		    while((ritem3 = sxba_1_rlscan (ritem_set, ritem3)) >= 0)
		    {
			if (SXBA_bit_is_set (ritem_cycle_set, ritem3))
			{
			    is_high_level = false;
			    break;
			}
		    }

		    if (!is_high_level)
			break;
		}
	
	    }

	    if (is_high_level)
	    {
		/* C'est un cycle de + haut niveau */

		/* On l'enleve de blocked_stack */
		for (x = blocked_stack [0]; x > 0; x--)
		{
		    item =  blocked_stack [x];
		    ritem = item2ritem [item];

		    if (SXBA_bit_is_set (ritem_cycle_set, ritem))
			blocked_stack [x] = 0;
		}

		/* On cherche un ordre d'execution. */
		/* Si k == j, tous les nt de la RHS ont la forme Bjj.
		   Une production de la grammaire initiale A -> \alpha done une
		   production Ajj -> \alphajj de la foret partagee.
		   Si k < j, pour une production de la foret partagee,
		   tous les nt de la RHS ont la forme Bkk ou Bjj, excepte
		   le nt d'adresse item-1 qui est de la forme Bkj. Attention, une production
		   par exemple X -> X X de la grammaire initiale, peut donner plusieurs
		   productions (X01 -> X00 X01 . et X01 -> X01 . X11) de la foret
		   partagee. */

		ritem = maxritem;

		while ((ritem = sxba_1_rlscan (ritem_cycle_set, ritem)) >= 0)
		{
		    /* Si k == j, des items differents peuvent designer la meme prod. */
		    SXBA_0_bit (ritem_cycle_set, ritem);
		    item = ritem2item [ritem];

		    B = lispro [item-1];
		    prod = prolis [item];

		    citem = (k == j) ? prolon [prod] : item;

		    /* citem est caracteristique d'une production de la foret partagee. */

		    SXBA_1_bit (cyclic_citem_set, citem);

		    if (!SXBA_bit_is_set (ap_sets [B], k))
		    {
			/* Non partiellement calcule */
			citem2nb [citem]++;
			cur_item2next [item] = nt2cur_item [B];
			nt2cur_item [B] = item;
		    }
		}

		citem = -1;

		while ((citem = sxba_scan (cyclic_citem_set, citem)) >= 0)
		{
		    if (citem2nb [citem] == 0)
		    {
			SXBA_0_bit (cyclic_citem_set, citem);
			PUSH (tbp_stack, citem);
		    }
		}

#if EBUG
		if (tbp_stack [0] == 0)
		    /* Pas de point d'entree */
		    sxtrap (ME, "process_cycle");
#endif
	    
		for (x = 1; x <= tbp_stack [0]; x++)
		{
		    citem = tbp_stack [x];
		    prod = prolis [citem];
		    A = lhs [prod];
		    p = &(nt2cur_item [A]);

		    if ((cur_item = *p) > 0)
		    {
			*p = 0;

			do
			{
			    cur_prod = prolis [cur_item];
			    citem = (k == j) ? prolon [cur_prod] : cur_item;

			    if (--citem2nb [citem] == 0)
			    {
				PUSH (tbp_stack, citem);
				SXBA_0_bit (cyclic_citem_set, citem);
			    }
			} while ((cur_item = cur_item2next [cur_item]) > 0);
		    }
		}

#if EBUG
		if (sxba_scan (cyclic_citem_set, -1) >= 0)
		    sxtrap (ME, "process_cycle"); 
#endif


		/* On fait la premiere passe */
		for (x = 1; x <= tbp_stack [0]; x++)
		{
		    citem = tbp_stack [x];
		    prod = prolis [citem];

		    item = (k < j) ? citem : 0;

		    cur_prod_core = &(tmp_prod_cores [citem][0]);
		    cur_prod_core [0] = prod;
			
		    A = lhs [prod];
		
#if EBUG
		    A0j = MAKE_A0j (A, j);
		    Aij = MAKE_Aij (A0j, k);
		    /* On est ds le cas cyclique, Aij a pu deja etre utilise en rhs. */
		    SXBA_1_bit (Aij_lhs_set, Aij);
#endif
		
		    if (is_action_fun)
		    {
			A0j = MAKE_A0j (A, j);
			Aij = MAKE_Aij (A0j, k);
			cur_rhs_stack = &(tmp_rhs_stacks [citem] [0]);
			cur_rhs_stack [0] = Aij;
		    }

		    right_item = prolon [prod+1]-1;
		    nbnt = prod2nbnt [prod];
		    l = m = j;

		    for (cur_item = right_item; nbnt > 0; cur_item--)
		    {
			B = lispro [cur_item-1];

			if (cur_item == item)
			    l = k;

			is_prdct = is_prdct_fun && (prdct_no = prdct [cur_item-1]) != -1;

			if (is_action_fun || is_prdct)
			{
			    B0j = MAKE_A0j (B, m);
			    Bkj = MAKE_Aij (B0j, l);

			    if (is_action_fun)
				cur_rhs_stack [nbnt] = Bkj;
			}

			if (is_prdct && !(*for_parsact.prdct) (Bkj, prdct_no))
			    break;
#if EBUG
			B0j = MAKE_A0j (B, m);
			Bkj = MAKE_Aij (B0j, l);

			/* meme ds le cas cyclique, Bkj a du etre deja (partiellement) calcule. */
			if (!SXBA_bit_is_set (Aij_lhs_set, Bkj))
			    sxtrap (ME, "process_cycle");

			SXBA_1_bit (Aij_rhs_set, Bkj);
#endif

			if (--nbnt > 0)
			    cur_prod_core [nbnt] = l;
		    }

		    if (nbnt == 0)
		    {
			if (is_print_prod)
			    print_prod (k, j, cur_prod_core);
			else
			    xprod++;

			if (!is_action_fun || (*for_parsact.action) (k, j, cur_prod_core, cur_rhs_stack))
			    SXBA_1_bit (ap_sets [A], k); /* Aij est (partiellement) calcule. */
		    }
		    else
			tbp_stack [x] = 0;
		}
	    
		if (is_action_fun)
		{
		    /* On fait les suivants (calcul des points fixes). */
		    /* Il faut verifier le calcul partiel car les actions du coup
		       precedent ont pu faire echouer le calcul! */
		    do
		    {
			should_loop = false;

			for (x = 1; x <= tbp_stack [0]; x++)
			{
			    citem = tbp_stack [x];

			    if ((citem = tbp_stack [x]) > 0)
			    {
				prod = prolis [citem];
				cur_prod_core = &(tmp_prod_cores [citem][0]);
				cur_prod_core [0] |= HIBITS; /* On previent la semantique qu'il y a un
								    cycle et que cette production a deja
								    ete traitee. */
				cur_rhs_stack = &(tmp_rhs_stacks [citem][0]);

				if ((*for_parsact.action) (k, j, cur_prod_core, cur_rhs_stack))
				    should_loop = true;
				else
				    /* point fixe atteint, on l'enleve */
				    tbp_stack [x] = 0;
			    }
			}
		    } while (should_loop);
		}

		tbp_stack [0] = 0;

		break;
	    }
	}
    }


    sxba_empty (k_ritem_set);
    sxba_empty (ritem_cycle_set);

    for (ritem =  maxritem-1; ritem >= 0; ritem--)
	sxba_empty (ritem_graph [ritem]);
}


static void mrr_generate (int i, int h, int j, int item)
{
    /* item = Aij -> \alphaih Bhj . \betajj */
    /* Bhj \betajj sont evalues */
    int 	prod, A, A0j, Aij, cur_item, nbnt, nbt, k, B, B0j, Bkj, prdct_no;
    bool	is_prdct;

    if (h == j && lispro [item-2] > 0)
    {
	PUSH (tbp_item_stack, item-1);
	return;
    }

    /* On traite le suffixe */
    prod = prolis [item];
    A = lhs [prod];

#if EBUG
    A0j = MAKE_A0j (A, j);
    Aij = MAKE_Aij (A0j, i);

    if (SXBA_bit_is_set (Aij_rhs_set, Aij))
	sxtrap (ME, "mrr_generate");

    SXBA_1_bit (Aij_lhs_set, Aij);
#endif
		
    prod_core [0] = prod;

    if (is_action_fun)
    {
	A0j = MAKE_A0j (A, j);
	Aij = MAKE_Aij (A0j, i);
	rhs_stack [0] = Aij;
    }

    cur_item = prolon [prod+1]-1;
    nbnt = prod2nbnt [prod];

    while (item <= cur_item)
    {
	k = item == cur_item ? h : j;
	cur_item--;
	B = lispro [cur_item];
	is_prdct = is_prdct_fun && (prdct_no = prdct [cur_item]) != -1;

	if (is_action_fun || is_prdct)
	{
	    B0j = MAKE_A0j (B, j);
	    Bkj = MAKE_Aij (B0j, k);

	    if (is_action_fun)
		rhs_stack [nbnt] = Bkj;
	}

	if (is_prdct &&
	    !(*for_parsact.prdct) (Bkj, prdct_no))
	    return;
#if EBUG
	B0j = MAKE_A0j (B, j);
	Bkj = MAKE_Aij (B0j, k);

	if (!SXBA_bit_is_set (Aij_lhs_set, Bkj))
	    sxtrap (ME, "mrr_generate");

	SXBA_1_bit (Aij_rhs_set, Bkj);
#endif

	if (--nbnt)
	    prod_core [nbnt] = k;
    }

    /* On traite le prefixe */

    if (nbnt == 0)
    {
	if (is_print_prod)
	    print_prod (i, j, prod_core);
	else
	    xprod++;

	if (!is_action_fun || (*for_parsact.action) (i, j, prod_core, rhs_stack))
	    SXBA_1_bit (ap_sets [A], i); /* Aij est (partiellement) calcule. */
	/* Aij peut se calculer autrement (autre production ou meme
	   production mais chemin (autre k) different). */
    }
    else
    {
	nbt = item2nbt [item-1];
	parse_item (A0j, item-2-nbt, NULL, i, h-nbt, j, nbnt);
    }
}


static void mutual_right_recursive (int j, int init_prod_order, int *out_prod_order)
{
    /* un ensemble de productions mutuellement recursives droites vient d'etre detecte'
       Les productions de la foret partagee sont triees dynamiquement de facon telle
       qu'elles ne sont executees que quand la semantique de la rhs est completement
       connue. */
    int 		prod_order, next_prod_order, prod, A, right_item, i, item, B, k, nbt;
    int			*i_prod_stack;
    struct PT2_item	*PT2j;
    SXBA 		index_set, backward_index_set, red_order_set;
    bool 		nothing_has_changed, is_ok;

    prod_order = init_prod_order;
    red_order_set = red_order_sets [j];

    PT2j = &(PT2 [j]);

    i_index_set [0] = j+1;

    do
    {
	if (SXBA_bit_is_set (red_order_set, prod_order))
	{
	    /* on parcourt le cycle */
	    prod = order2prod [prod_order];
	    A = lhs [prod];
	    right_item = prolon [prod+1]-1;
	    index_set = PT2j->index_sets [right_item];

	    i = -1;

	    while ((i = sxba_scan (index_set, i)) >= 0)
		PUSH (i2prod_stack [i], prod);

	    OR (i_index_set, index_set);
	}

	next_prod_order = parse_order2next [prod_order];

	if (next_prod_order > prod_order)
	    /* Sortie de la boucle */
	    *out_prod_order = prod_order;

	prod_order = next_prod_order-1;
    } while (prod_order != init_prod_order);

    i = j+1;

    while ((i = sxba_1_rlscan (i_index_set, i)) >= 0)
    {
	/* On parcourt tous les index par ordre decroissant. */
	SXBA_0_bit (i_index_set, i);
	i_prod_stack = &(i2prod_stack [i][0]);

	do
	{
	    prod = POP (i_prod_stack);
	    right_item = prolon [prod+1]-1;
	    PUSH (tbp_item_stack, right_item);
	    A = lhs [prod];
	    SXBA_1_bit (i2nt_nyp, A);
	} while (!IS_EMPTY (i_prod_stack));

	while (!IS_EMPTY (tbp_item_stack))
	{
	    nothing_has_changed = true;

	    do
	    {
		item = POP (tbp_item_stack);
		/* item = A -> \alpha B . \beta et \betajj =>* \epsilon */
		/* i \in index_set [item] */

		if (item < 0)
		{
		    item = -item;
		    /* item = Aij -> \alphaii Bij . \betajj */
		    /* De plus l'existence de \alphaii a deja ete testee. */
		    B = lispro [item-1];

		    if (SXBA_bit_is_set (i2nt_nyp, B))
			/* Bij n'est pas calcule */
			PUSH (blocked_stack, item);
		    else
		    {
			/* Bij est calcule */
			mrr_generate (i, i, j, item);
			nothing_has_changed = false;
		    }
		}
		else
		{
		    backward_index_set = PT2j->backward_index_sets [item];
		    /* Soit k \in backward_index_set */
		    /* Si i < k => Bkj est calcule */
		    /* Si i = k => Bij est calcule <=> B \not \in i2nt_nyp */

		    B = lispro [item-1];

		    k = i-1;

		    while ((k = sxba_scan (backward_index_set, k)) >= 0)
		    {
			nbt = item2nbt [item-1];
    
			/* On teste que i est bien ds le index_set de k */
			if (lispro [item-2-nbt] == 0)
			    /* \alphaik \in T* */
			    is_ok = (i+nbt == k);
			else
			    is_ok = SXBA_bit_is_set (PT2 [k-nbt].index_sets [item-1-nbt], i);

			if (is_ok) {
			    if (i < k)
			    {
				/* Bkj est calcule */
				/* Si k == j, peut empiler ds tbp_item_stack */
				mrr_generate (i, k, j, item);
				nothing_has_changed = false;
			    }
			    else
			    {
				/* i == k */
				if (SXBA_bit_is_set (i2nt_nyp, B))
				    /* Bij n'est pas entierement calcule */
				    PUSH (blocked_stack, item);
				else
				{
				    /* Bij est calcule */
				    mrr_generate (i, i, j, item);
				    nothing_has_changed = false;
				}
			    }
			}
		    }
		}
	    } while (!IS_EMPTY (tbp_item_stack));

	    if (IS_EMPTY (blocked_stack))
		/* OK pour i */
		break;

	    if (nothing_has_changed)
	    {
		/* cas cyclique */
		process_cycle (i, j);
	    }

	    sxba_empty (i2nt_nyp);

	    while (!IS_EMPTY (blocked_stack))
	    {
		if ((item = POP (blocked_stack)) > 0)
		{
		    prod = prolis [item];
		    A = lhs [prod];
		    SXBA_1_bit (i2nt_nyp, A);
		    PUSH (tbp_item_stack, -item); /* item repere un Bij */
		}
	    }
	}
    }
}



static void parse ()
{
    struct PT2_item	*PT2j;
    int                 *PT2j_shift_NT_stack;
    SXBA	        index_set, red_order_set;
    int		        item, new_item, new_j, j, prod_order, A, A0j, Aij, B, nbt, nbnt, prod, x;
    bool             is_ok;
    SXBA		lhs_nt_set;

    /* On genere toutes les reductions finissant en j */
  
    lhs_nt_set = sxba_calloc (ntmax+1);

    if (is_cyclic)
	cycle_red_order_set = sxba_calloc (prodmax+1);

    for (j = 0; j <= n; j++)
    {
	PT2j = &(PT2 [j]);
	red_order_set = red_order_sets [j];
	/* red_order_set est positionne par le traitement des PT2. */
	/* il contient les A -> \alpha t. */
	prod_order = prodmax+1;

	while ((prod_order = sxba_1_rlscan (red_order_set, prod_order)) > 0)
	{
	    /* Examine' du plus grand vers le + petit. */
	    rhs_stack [0] = prod_core [0] = prod = order2prod [prod_order];
	    item = prolon [prod+1]-1;
	    A = lhs [prod];

	    if (SXBA_bit_is_reset_set (lhs_nt_set, A))
		ap_sets [A] [0] = j+1; /* On change la taille! */

	    nbt = item2nbt [item];
	    nbnt = prod2nbnt [prod];

#if EBUG
	    A0j = MAKE_A0j (A, j);
#else
	    if (is_action_fun)
		A0j = MAKE_A0j (A, j);
#endif
	    
	    if (nbnt == 0)
	    {
		/* RHS \in T* */
		new_j = j - nbt;

#if EBUG
		Aij = MAKE_Aij (A0j, new_j);

		if (SXBA_bit_is_set (Aij_rhs_set, Aij))
		    sxtrap (ME, "parse");

		SXBA_1_bit (Aij_lhs_set, Aij);
#endif

		if (is_print_prod)
		    print_prod (new_j, j, prod_core);
		else
		    xprod++;

		if (is_action_fun)
		{
		    rhs_stack [0] = MAKE_Aij (A0j, new_j);
		    is_ok = (*for_parsact.action) (new_j, j, prod_core, rhs_stack);
		}
		else
		    is_ok = true;

		if (is_ok)
		    SXBA_1_bit (ap_sets [A], new_j);
	    }
	    else
	    {
		if (nbt == 0)
		{
		    /* item = A -> \alpha B . */
#if 0
		    if (is_cyclic && cyclic_parse_order2next [prod_order] != 0)
		    {
			cyclic_sem (j, prod_order, &prod_order);
		    }
		    else
#endif
		    {
			if (SXBA_bit_is_set (is_mutual_right_recursive_prod, prod))
			{
			    mutual_right_recursive (j, prod_order, &prod_order);
			}
			else
			{
			    B = lispro [item-1];

			    if (AND (PT2j->backward_index_sets [item], ap_sets [B]))
			    {
				if (parse_item (A0j, item-1, PT2j->index_sets [item], -1, j, j, nbnt))
				    /* ap_sets [A] a ete modifie */
				    /* rupture de sequence (eventuel) calcule statiquement */
				    prod_order = parse_order2next [prod_order];
			    }
			}
		    }
		}
		else
		{
		    /* new_item = A -> \alpha B . t1 ... tnbt */
		    new_item = item - nbt;
		    new_j = j - nbt;
		    index_set = PT2 [new_j].index_sets [new_item];
		    parse_item (A0j, new_item - 1, index_set, -1, new_j, j, nbnt);
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

	A = 0;

	while ((A = sxba_scan_reset (lhs_nt_set, A)) > 0)
	    sxba_empty (ap_sets [A]);
	
    }

    if (is_cyclic)
	sxfree (cycle_red_order_set);

    sxfree (lhs_nt_set);
}



static void sem_td_mark (int Aij, int i, int j)
{
    /* On genere recursivement toutes les productions dont Aij est la partie gauche. */
    ushort	*tmp_prod_core;
    int 	prod, nbnt, item, h, k, Bhk, B, delta, x;

    tmp_prod_core = &(ARN_sem [Aij].prod_core [0]);
    SXBA_1_bit (Aij_set, Aij);
    prod = tmp_prod_core [0];
    nbnt = prod2nbnt [prod];

    if (nbnt > 0)
    {
	k = (nbnt == 1) ? (j-prodXnt2nbt [prod][0]) : tmp_prod_core [1];

	delta = prodXnt2nbt [prod][1];
	h = i + delta;
	item = prolon [prod] + delta;
	B = lispro [item];
	Bhk = MAKE_Aij (MAKE_A0j (B, k), h);

	sem_td_mark (Bhk, h, k);

	if (nbnt > 1)
	{
	    h = k;

	    for (x = 2; x < nbnt; x++)
	    {
		delta = prodXnt2nbt [prod][x];
		h += delta;
		k = tmp_prod_core [x];
		item += delta+1;
		B = lispro [item];
		Bhk = MAKE_Aij (MAKE_A0j (B, k), h);
		sem_td_mark (Bhk, h, k);
		h = k;
	    }
	    
	    delta = prodXnt2nbt [prod][nbnt];
	    h += delta;
	    k = j - prodXnt2nbt [prod][0];
	    item += delta+1;
	    B = lispro [item];
	    Bhk = MAKE_Aij (MAKE_A0j (B, k), h);
	    sem_td_mark (Bhk, h, k);
	}
    }
}

static void sem_bu_extract ()
{
    /* Extrait les productions valides de la foret partagee et les generent
       dans le bon ordre. */
    int		i, j, order, A, A0j, Aij;

    if (is_print_prod)
    {
	for (j = 0; j <= n; j++)
	{
	    for (order = ntmax; order > 0; order--)
	    {
		A = order2nt [order];
		A0j = MAKE_A0j (A, j);
		Aij = MAKE_Aij (A0j, j); /* Ajj */

		while ((Aij = sxba_1_rlscan (Aij_set, Aij)) >= A0j)
		{
		    i = Aij - A0j;
		    print_prod (i, j, &(ARN_sem [Aij].prod_core [0]));
		}
	    }
	}
    }
    else
	xprod = sxba_cardinal (Aij_set);
}

static double	*spf_count;

static void spf_count_init (int size)
{
    spf_count = (double*) sxcalloc (size, sizeof (double));
}

static void spf_count_final (int size)
{
    (void) size;
    sxfree (spf_count);
}

static bool spf_count_parsact (int i, int j, ushort *tmp_prod_core, int tmp_rhs_stack []) 
{
// #define __NO_MATH_INLINES

    int		Aij;
    double	val;
    int 	prod, nbnt;

    (void) i;
    (void) j;

    prod = tmp_prod_core [0];
    Aij = tmp_rhs_stack [0];

    if (prod & HIBITS)
    {
	/* implique' ds un cycle et 2eme passage */
	spf_count [Aij] = HUGE_VAL; /* Produces IEEE Infinity. */
	return false;
    }
    else
    {
	nbnt = prod2nbnt [prod]; 

	if (nbnt > 0)
	{
	    val = spf_count [tmp_rhs_stack [nbnt]];

	    while (--nbnt > 0)
		val *= spf_count [tmp_rhs_stack [nbnt]];
	}
	else
	    val = 1.0;

	spf_count [Aij] += val;

	return true;
    }
}

#if 0
{
    double	val;
    int 	nbnt, delta, B, h, k, Bhk, item, prod, x;

    prod = prod_core [0];
    nbnt = prod2nbnt [prod]; 

    if (nbnt == 0)
	val = 1.0;
    else
    {
	k = (nbnt == 1) ? j : prod_core [1];

	delta = prodXnt2nbt [prod][1];
	h = i + delta;
	k -= prodXnt2nbt [prod][0];
	item = prolon [prod] + delta;
	B = lispro [item];
	Bhk = MAKE_Aij (MAKE_A0j (B, k), h);

	val = spf_count [Bhk];

	if (nbnt > 1)
	{
	    h = k;

	    for (x = 2; x < nbnt; x++)
	    {
		delta = prodXnt2nbt [prod][x];
		h += delta;
		k = prod_core [x];
		item += delta+1;
		B = lispro [item];
		Bhk = MAKE_Aij (MAKE_A0j (B, k), h);
		val *= spf_count [Bhk];
		h = k;
	    }
	    
	    delta = prodXnt2nbt [prod][nbnt];
	    h += delta;
	    k = j - prodXnt2nbt [prod][0];
	    item += delta+1;
	    B = lispro [item];
	    Bhk = MAKE_Aij (MAKE_A0j (B, k), h);
	    val *= spf_count [Bhk];
	}
    }

    spf_count [Aij] += val;

    return true;
}
#endif


static void set_A2A0j ()
{
    int	*p, A, j;

    p = (int*) sxalloc (ntmax * (n+1), sizeof (int));

    for (A = 1; A <= ntmax; A++)
    {
	A2A0j [A] = p;

	for (j = 0; j <= n; j++, p++)
	{
	    *p = A0j2Aij_size;
	    A0j2Aij_size += j+1;
	}
    }
}


int main (int argc, char *argv [])
{
    int 	l, t, *tok, size, S0n;
    char	*s, c;

    sxstdout = stdout, sxstderr = stderr;

    if (argc != 3)
    {
	printf (Usage, ME);
	return 1;
    }

    sxopentty ();

    is_parser = false;
    is_print_prod = false;
    is_no_semantics = true;
    is_default_semantics = true;
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
		is_default_semantics = true;
		is_no_semantics = false;
		break;

	    case 'n':
		is_no_semantics = true;
		break;

	    case 'p':
		if (!is_parser)
		    is_parser = true;
		else
		    is_print_prod = true;
		break;

	    case 'r':
		is_parser = false;
		break;

	    case 's':
		is_default_semantics = false;
		is_no_semantics = false;
		break;

	    default:
		printf (Usage, ME);
		return 1;
	    
	    }
	}
    }

    print_time = true;

    s = source = argv [2];
    n = strlen (source);
    tok = TOK = (int*) sxalloc (n + 2, sizeof (int));

    while ((c = *s++) != '\0')
    {
	if ((t = char2tok (c)) == 0)
	{
	    printf ("\
\t%s Lexical error at char #%ld\n", ME, (TOK - tok) + 1);
	    return 1;
	}

	*++tok = t;
    }

    *++tok = tmax;

    if (print_time)
	sxtime (TIME_INIT, "Init");

    grammar ();

    sprod2order = (int*) sxcalloc (prodmax+1, sizeof (int));

    grammar2 ();

    sprod_set = sxba_calloc (sprodmax+1);
    sprod2attr = (struct sprod2attr*) sxalloc (sprodmax+1, sizeof (struct sprod2attr));

    if (print_time)
	sxtime (TIME_FINAL, "Grammar Processing");

    bag_alloc (&shift_bag, "shift_bag", (n + 1) * prodmax * SXNBLONGS (n + 1));

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
	if (is_no_semantics)
	{
	    for_parsact.sem_init = NULL;
	    for_parsact.sem_final = NULL;
	    for_parsact.action = NULL;
	    for_parsact.prdct = NULL;
	    for_parsact.constraint = NULL;

	}
	else
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
    }

    is_action_fun = for_parsact.action != NULL;
    is_prdct_fun = for_parsact.prdct != NULL;
    is_constraint_fun = for_parsact.constraint != NULL;

    if (is_epsilon && is_constraint_fun)
	constraint_to_be_checked = sxbm_calloc (n+1, ntmax+1);

    l = recognize ();

    if (print_time)
	sxtime (TIME_FINAL, "Recognizer");

    if (l < n+2)
	printf ("Syntax error at %i\n", l);
    else
    {
	if (is_parser)
	{
	    PT2 = (struct PT2_item*) sxcalloc (n+1, sizeof (struct PT2_item));
	    bag_alloc (&pt2_bag, "pt2_bag", (n + 1) * prodmax * SXNBLONGS (n + 1));
	    ap_sets = sxbm_calloc (ntmax+1, n+1);
	    wis = sxbm_calloc (rhs_maxnt, n+1);
	    red_order_sets = sxbm_calloc (n+1, prodmax+1);

	    RL_mreduction ();

	    bag_free (&shift_bag);
	    sxbm_free (ntXindex_set);

	    if (print_time)
		sxtime (TIME_FINAL, "Reducer");
	    
#if EBUG
	    set_A2A0j ();
	    Aij_rhs_set = sxba_calloc (A0j2Aij_size);
	    Aij_lhs_set = sxba_calloc (A0j2Aij_size);
#else
	    if (is_action_fun)
		set_A2A0j ();
#endif

	    if (is_action_fun)
	    {
		if (max_tree_nb > 1)
		{
		    log_max_tree_nb = sxlast_bit (max_tree_nb);
		    filtre_max_tree_nb = ~((~(0L))<<log_max_tree_nb);
		}

		if (for_parsact.sem_init != NULL)
		    (*for_parsact.sem_init) (A0j2Aij_size);
	    }

	    if (is_mutual_right_recursive)
	    {
		int	*p, i;

		logn = sxlast_bit (n);
		filtren = ~((~(0L))<<logn);

		i_index_set = sxba_calloc (n+1);
		i2nt_nyp = sxba_calloc (ntmax+1);
		i2prod_stack = (int**) sxalloc (n+1, sizeof (int*));
		p = gprod_stacks = (int*) sxalloc ((n+1)*(prodmax+1), sizeof (int));

		for (i = 0; i <= n; i++)
		{
		    i2prod_stack [i] = p;
		    *p = 0;
		    p += prodmax+1;
		}

		if (is_cyclic)
		{
		    k_ritem_set = sxba_calloc (itemmax+1);
		    ritem_cycle_set = sxba_calloc (itemmax+1);
		    ritem_gset = sxba_calloc (itemmax+1);
		    ritem_gset2 = sxba_calloc (itemmax+1);
		    ritem_graph = sxbm_calloc (itemmax+1, itemmax+1);
		    cyclic_citem_set = sxba_calloc (itemmax+1);
		}
	    }

	    parse ();
	    printf ("Nombre de productions de la foret partagee : %i\n", xprod);

#if EBUG
	    sxfree (Aij_rhs_set);
	    sxfree (Aij_lhs_set);
#endif

	    if (is_mutual_right_recursive)
	    {
		sxfree (i_index_set);
		sxfree (i2nt_nyp);
		sxfree (i2prod_stack);
		sxfree (gprod_stacks);

		if (is_cyclic)
		{
		    sxfree (k_ritem_set);
		    sxfree (ritem_cycle_set);
		    sxfree (ritem_gset);
		    sxfree (ritem_gset2);
		    sxbm_free (ritem_graph);
		    sxfree (cyclic_citem_set);
		}
	    }

	    
	    if (print_time)
		sxtime (TIME_FINAL, "Parser");

	    if (!is_no_semantics)
	    {
		S0n = MAKE_Aij (MAKE_A0j (1 /* start symbol */, n), 0);

		if (is_default_semantics)
		{
		    double val = spf_count [S0n];

		    if (val < 1.0E9)
			printf ("Nombre d'arbres de la foret partagee : %.f\n", val);
		    else
			printf ("Nombre d'arbres de la foret partagee : %e\n", val);
		}
		else
		{
		    if (SXBA_bit_is_set (Aij_sem_lhs_set, S0n))
		    {
			Aij_set = sxba_calloc (A0j2Aij_size);
			xprod = 0;

			if (max_tree_nb == 1)
			{
			    sem_td_mark (S0n, 0, n);
			    sem_bu_extract ();
			}
			else
			{
			    int		x;
			    struct ARN_ksem	*psem;

			    log_max_tree_nb = sxlast_bit (max_tree_nb);
			    filtre_max_tree_nb = ~((~(0L))<<log_max_tree_nb);

			    for (psem = ARN_disp_ksem [S0n], x = 0; x < max_tree_nb; psem++, x++)
			    {
				if (psem->val == -1)
				    break;

				ksem_td_mark (S0n, 0, n, x);
			    }

			    ksem_bu_extract ();
			}

			sxfree (Aij_set);

			printf ("Nombre de productions de la foret partagee (avec semantique) : %i\n", xprod);

			if (print_time)
			    sxtime (TIME_FINAL, "Shared Parse Forest");
		    }
		    else
			fputs ("La foret partagee est vide\n", stdout);
		}

		if (is_action_fun)
		{
		    sxfree (A2A0j [1]), A2A0j [1] = NULL;

		    if (for_parsact.sem_final != NULL)
			(*for_parsact.sem_final) (A0j2Aij_size);
		}
	    }

#if EBUG
	    if	(A2A0j [1] != NULL)
		sxfree (A2A0j [1]), A2A0j [1] = NULL;
#endif

	    sxfree (PT2);
	    bag_free (&pt2_bag);
	    sxbm_free (ap_sets);
	    sxbm_free (wis);
	    sxbm_free (red_order_sets);
	}
    }

    if (is_epsilon && is_constraint_fun)
	sxbm_free (constraint_to_be_checked);

    if (l < n+2 || !is_parser)
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
	+ sizeof (SXBA_ELT) * SXNBLONGS (itemmax+1) * (4) /* global_state_sets */ +
	    sizeof (SXBA_ELT) * SXNBLONGS (ntmax+1) * (n+1) /* ntXindex_set */ ;
    printf ("Size of recognize structures = %i bytes\n", size);
#endif

    if (is_parser)
    {
	sxfree (PT);
	sxfree (level0_index_set);


#if EBUG
	size = sizeof (struct parse_item) * (n+1) /* PT */
	    ;

	if (is_action_fun)
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
