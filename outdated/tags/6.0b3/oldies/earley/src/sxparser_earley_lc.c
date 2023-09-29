/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1996 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Jul 23 1996 11:08:		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#define WHAT	"@(#)sxparser_earley_lc.c	- SYNTAX [unix] -  Jul 23 1996 11:08:56"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

static char	ME [] = "sxparser_earley_lc";


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
#include "sxalloc.h"
#include "sxba.h"
#include "SS.h"

/*
   Ce programme doit etre compile avec l'option :
   -Dtables_h=\"..._t.h\"
   ou ..._t.h est le nom des tables d'analyse produites par LALC1
*/

#include tables_h

/* Provisoire : char2tok pour le langage courant */
/* -Dscanner_h=\"..._s.h\" */
#include scanner_h


static BOOLEAN		is_default_semantics, is_print_prod, print_time, is_no_semantics;
static int		max_tree_nb;

#include	<stdio.h>
FILE	*sxtty, *sxstdout = {stdout}, *sxstderr = {stderr};

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
else {
    getrusage (RUSAGE_SELF, &next_usage);
    t =  (long) (1000 * (next_usage.ru_utime.tv_sec - prev_usage.ru_utime.tv_sec) +
		  (next_usage.ru_utime.tv_usec  - prev_usage.ru_utime.tv_usec) / 1000);
    prev_usage.ru_utime.tv_sec = next_usage.ru_utime.tv_sec;
    prev_usage.ru_utime.tv_usec = next_usage.ru_utime.tv_usec;
    fputs (str, sxtty);

    if (print_time)
	fprintf (sxtty, " (%dms)\n", t);
    else
	putc ('\n', sxtty);
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


struct for_parsact {
    void	(*sem_init) (),
                (*sem_final) ();

    BOOLEAN	(*prdct) (),
                (*action) (),
                (*constraint) ();
};

static BOOLEAN	is_action_fun, is_prdct_fun, is_constraint_fun;



#define PUSH(s,x)	(s)[++*(s)]=(x)
#define POP(s)		(s)[(*(s))--]
#define IS_EMPTY(s)	(*(s)==0)

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


static BOOLEAN		is_parser;

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
	BOOLEAN		is_erased;
    } *lhs;
};

static struct spf	spf;

static int		rhs_stack [rhs_lgth+1];

typedef unsigned short	ushort;
static ushort		prod_core [rhs_maxnt];


static struct Aij_pool {
  int		A, i, j, first_lhs, first_rhs;
  BOOLEAN	is_erased;
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



static BOOLEAN
NON_EQUAL (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- != *rhs_bits_ptr--)
	    return TRUE;
    }

    return FALSE;
}


static BOOLEAN
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
	    return FALSE;
    }

    return TRUE;
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


static BOOLEAN
OR_CHANGED (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));
    register SXBA_ELT	val1, val2;
    BOOLEAN		has_changed = FALSE;

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
	    has_changed = TRUE;
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

static BOOLEAN
OR_AND (bits_array1, bits_array2, bits_array3)
    SXBA	bits_array1, bits_array2, bits_array3;
{
    /* bits_array1 or (bits_array2 - bits_array3) */
    register SXBA	bits_ptr1, bits_ptr2, bits_ptr3;
    register int	slices_number = NBLONGS (BASIZE (bits_array2));
    register SXBA_ELT	val1, val2;
    BOOLEAN		has_changed = FALSE;

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
		has_changed = TRUE;
	    }
	}

	bits_ptr1--;
    }

    return has_changed;
}


static BOOLEAN
OR_AND_MINUS (bits_array1, bits_array2, bits_array3, bits_array4)
    SXBA	bits_array1, bits_array2, bits_array3, bits_array4;
{
    /* bits_array4 =  bits_array2 & bits_array3 - bits_array1
       bits_array1 |= bits_array4
       retourne vrai ssi bits_array4 est non vide */
    register SXBA	bits_ptr1, bits_ptr2, bits_ptr3, bits_ptr4;
    /* On change la taille! */
    register int	slices_number = NBLONGS (BASIZE (bits_array4) = BASIZE (bits_array2));
    register SXBA_ELT	val;
    BOOLEAN		is_set = FALSE;

    bits_ptr1 = bits_array1 + slices_number,
    bits_ptr2 = bits_array2 + slices_number,
    bits_ptr3 = bits_array3 + slices_number,
    bits_ptr4 = bits_array4 + slices_number;

    while (slices_number-- > 0)
    {
	if ((val = *bits_ptr4-- = (*bits_ptr2--) & (*bits_ptr3--) & (~(*bits_ptr1))) != 0L)
	    is_set = TRUE;

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

static BOOLEAN
AND (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));
    register int	lhs_slices_number = NBLONGS (BASIZE (lhs_bits_array));
    BOOLEAN		ret_val = FALSE;

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
	    ret_val = TRUE;
    }

    return ret_val;
}


static BOOLEAN
AND3 (lhs_bits_array, op1_bits_array, op2_bits_array)
    SXBA	lhs_bits_array, op1_bits_array, op2_bits_array;
{
    /* On suppose que lhs_bits_array est vide au-dela de op2_bits_array */

    register SXBA	lhs_bits_ptr, op1_bits_ptr, op2_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (op2_bits_array));
    BOOLEAN		ret_val = FALSE;

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
	    ret_val = TRUE;
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
    BOOLEAN		ret_val = FALSE;

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
	    ret_val = TRUE;
    }

    return ret_val;
}


static BOOLEAN
MINUS (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (rhs_bits_array));
    BOOLEAN		ret_val = FALSE;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- &= ~(*rhs_bits_ptr--))
	    ret_val = TRUE;
    }

    return ret_val;
}



static BOOLEAN
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
	    return TRUE;
    }

    return FALSE;
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
    


static SXBA	item_set_i1, item_set_i2, item_set_i3; 


static void
reduce (A, j, i)
    int A, j, i;
{
    /* Toutes les reductions vers Aj ont ete effectuees
       Cas du reconnaisseur : on fait le "shift" sur A dans la liste "j".
       cas du parser : en plus, on fait les reductions eventuelles B -> alpha Aji */
    int				state, new_state, B, Y, YY, prdct_no, hd, new_j;
    SXBA			index_set, index_set2, backward_index_set;
    struct recognize_item	*RTj;

    if ((prdct_no = constraints [A]) != -1 &&
	is_constraint_fun &&
	!for_parsact.constraint (prdct_no, A, j, i))
	return;

    RTj = &(RT [j]);

/*
    item_set_i2 == t2item_set [-TOK [i+1]]
    item_set_i3 == t2item_set [-TOK [i+2]]
*/

    for (state = RTj->shift_NT_hd [A]; state > 0; state = RTj->shift_state_stack [state])
    {
	/* state = B -> \beta . A \gamma */
	new_state = state+1;

	if (SXBA_bit_is_set (item_set_i2 /* t2item_set [-TOK [i+1]] */, new_state))
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
			(!SXBA_bit_is_set (item_set_i3 /* t2item_set [-TOK [i+2]] */, new_state+1) ||
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

		if (is_epsilon && Y > 0 && SXBA_bit_is_set (BVIDE, Y))
		{
		    new_state++;

		    if (!SXBA_bit_is_set (item_set_i2 /* t2item_set [-TOK [i+1]] */, new_state))
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

	    filtre = 1 << MOD (j);
	}
	else
	    j--;
    }
}



static BOOLEAN
complete (i)
    int i;
{
    int		hd, state, next_state, X, Y, YY, A, B, j, prdct_no, prod, Aik, start, end;
    SXBA	index_set, index_set2, backward_index_set;
    BOOLEAN	keep, is_tok = FALSE, is_scan_reduce = FALSE;


    /* Le look-ahead est verifie pour tous les state de T1_state_set. */
    /* De plus si A -> \alpha X . ai+1 \beta, le predicat de ai+1 est verifie'
       ainsi que t2item_set [\beta]. */

    item_set_i1 = item_set_i2; /* ai+1 */
    item_set_i2 = item_set_i3; /* ai+2 ou eof */
    item_set_i3 = i+1 < n ? t2item_set [-TOK [i+3]] : item_set_i2; /* ai+3 ou eof */
    
    if ((state = T1_shift_NT_hd [0]) != 0)
    {
	/* A -> \alpha X . ai+1 \beta */
	is_tok = TRUE;
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
			if (!SXBA_bit_is_set (item_set_i3 /* t2item_set [-TOK [i+3]] */, next_state+1) ||
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

			if (Y > 0 && SXBA_bit_is_set (BVIDE, Y))
			{
			    next_state++;

			    if (SXBA_bit_is_set (item_set_i2 /* t2item_set [-TOK [i+2]] */, next_state))
				continue;
			}
		    }

		    break;
		}

		if (Y == 0)
		{
		    /* next_state = A -> \alpha X  ai+1 \beta .  et \beta =>* \epsilon */
		    is_scan_reduce = TRUE;

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
	    sxba_or (init_state_set, nt2item_set [POP (T1_shift_NT_stack)]);
	} while (!IS_EMPTY (T1_shift_NT_stack));

	sxba_and (init_state_set, item_set_i1 /* t2item_set [-TOK [i+1]] */);

	state = 0;

	while ((state = sxba_scan_reset (init_state_set, state)) > 0)
	{
	    /* A -> \alpha . X beta et \alpha =>* \epsilon */
	    X = lispro [state];

	    if (X < 0)
	    {
		next_state = state + 1;

		if (!SXBA_bit_is_set (item_set_i2 /* t2item_set [-TOK [i+2]] */, next_state) ||
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
		    /* is_epsilon == TRUE */
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
		is_tok = TRUE;

		while ((Y = lispro [next_state]) != 0)
		{
		    if (Y < 0)
		    {
			/* Y == ai+2 */
			/* next_state = A -> \alpha ai+1 \beta . ai+2 \gamma et \alpha \beta =>* \epsilon */
			if (!SXBA_bit_is_set (item_set_i3 /* t2item_set [-TOK [i+3]] */, next_state+1) ||
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

			if (Y > 0 && SXBA_bit_is_set (BVIDE, Y))
			{
			    next_state++;

			    if (SXBA_bit_is_set (item_set_i2 /* t2item_set [-TOK [i+2]] */, next_state))
				continue;
			}
		    }

		    break;
		}

		if (Y == 0)
		{
		    /* next_state = A -> \alpha ai+1 \beta .  et \alpha \beta =>* \epsilon */
		    is_scan_reduce = TRUE;

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




static int
recognize ()
{
    int			i;
    int			*T0_shift_NT_stack;
    BOOLEAN		is_in_LG;
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

    item_set_i2 = t2item_set [-TOK [1]]; /* a1 */

    if (SXBA_bit_is_set (item_set_i2, initial_state))
    {
	if (is_epsilon)
	{
	    if (n == 0 && SXBA_bit_is_set (BVIDE, 1))
	    {
		/* un peu bidouilleux!! */
		index_set = T2_index_sets [2] = bag_get (&shift_bag, 1);
		SXBA_1_bit (index_set, 0);
	    }
	}

	item_set_i3 = n > 0 ? t2item_set [-TOK [2]] : item_set_i2; /* a2 ou eof */

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


static void
ARN_sem_init (size)
    int size;
{
    Aij_sem_lhs_set = sxba_calloc (size);
    ARN_sem = (struct ARN_sem*) sxalloc (size, sizeof (struct ARN_sem));
}


static void
ARN_sem_final (size)
    int size;
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


static BOOLEAN
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


static BOOLEAN
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
    BOOLEAN			is_tbp, is_new;

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

    if (is_new = SXBA_bit_is_reset_set (RL_nt_set, Y))
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
	is_tbp = FALSE;
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
		/* On ne doit pas le supprimer, il peut servir + tard. */
		if (nbis == NULL)
		    nbis = PT2 [j].backward_index_sets [item_j] = bag_get (&pt2_bag, j+1);

		SXBA_1_bit (nbis, k);

		if ((is_new || !SXBA_bit_is_set (ap_set, k)))
		{
		    SXBA_1_bit (ap_set, k);
		    is_tbp = TRUE;
		}

		if (new_item > bot_item)
		{
		    /* new_item pointe derriere un nt. */
		    new_k = k-nbt;

		    if (new_k == j)
		    {
			/* is_epsilon == TRUE */
			if ((ids = PT2 [new_k].index_sets [new_item]) == NULL)
			    ids = PT2 [new_k].index_sets [new_item] = bag_get (&pt2_bag, j+1);

			if (OR_AND (ids, prev_index_set, I))
			{
			    /* Il y a du nouveau */
			    if (RL_mreduction_item (new_item, ids, j))
				is_tbp = TRUE;
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
    BOOLEAN			is_good;

    RL_nt_set = sxba_calloc (ntmax+1);
    prod_order_set = sxba_calloc (prodmax+1);

    index_set = PT2 [n].index_sets [2] = bag_get (&pt2_bag, n+1);
    SXBA_1_bit (index_set, 0); /* {0} */

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

	    is_good = FALSE;
				
	    if (tnb == 0)
	    {
		if (is_epsilon && item == prolon [prod])
		{
		    /* item = A -> . */
		    if (SXBA_bit_is_set (ap_set, j))
			is_good = TRUE;
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

			is_good = TRUE;
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
			is_good = TRUE;
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
			    is_good = TRUE;
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
print_prod (i, k, prod_core)
    int 	i, k;
    ushort	*prod_core;
{
    int		prod, h, B, j, A, x, item, nbnt;

    prod = prod_core [0];
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
	h = (B > 0) ? ((x < nbnt) ? prod_core [x++] : k - prodXnt2nbt [prod][0]) : j+1;
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

    Aij_sem_lhs_set = sxba_calloc (size);
    ARN_disp_ksem = (struct ARN_ksem**) sxalloc (size, sizeof (struct ARN_ksem*));
    ARN_ksem_top = ARN_ksem = (struct ARN_ksem*) sxalloc (ARN_ksem_size = max_tree_nb*size, sizeof (struct ARN_ksem));
    ARN_ksem_is_marked = sxba_calloc (ARN_ksem_size);

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
static BOOLEAN
ksem_incr (nbnt, rhs_stack)
    int nbnt, *rhs_stack;
{
    int x, i, shift;

    shift = 0;

    for (x = 0; x < nbnt; x++, shift += log_max_tree_nb)
    {
	i = (ksem_vector >> shift) & filtre_max_tree_nb;
	ksem_vector &= ~(filtre_max_tree_nb << shift);

	i++;

	if (i < max_tree_nb && ARN_disp_ksem [rhs_stack [x+1]][i].val != -1)
	{
	    ksem_vector |= (i << shift);
	    return TRUE;
	}
    }

    return FALSE;
}

static int
ARN_keval (Aij, i, j, rhs_stack)
     int Aij, i, j, *rhs_stack;
{
    /* remplit les [0..top-1] premiers elements de kvals */
    /* ksem_vector == 0 */
    int			top, x, min_x, prod, nbnt, Bhk, shift, vectx;
    float		val, min_val, cur_val, proba;
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
	    Bhk = rhs_stack [1];

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
		    val *= ARN_disp_ksem [rhs_stack [x+1]][vectx].val;
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

	    } while (ksem_incr (nbnt, rhs_stack));
	}
    }

    return top;
}


static BOOLEAN
ARN_kparsact (i, j, prod_core, rhs_stack)
    int 	i, j;
    ushort	*prod_core;
    int		rhs_stack [];
{
    int			Aij, nbnt, x, top, prod;
    ushort		*p, *q;
    float		val;
    struct ARN_ksem	*psem, *psem2, *Aij_disp_ksem;
    
    prod = prod_core [0];

    if ((top = ARN_keval (prod, i, j, rhs_stack)) > 0)
    {
	/* kvals est positionne */
	Aij = rhs_stack [0];

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
		    psem2->prod_core [0] = prod_core [0];
		else
		{
		    psem2->vector = disp_kvals [top]->vector;
		    
		    nbnt--;
		    p = &(prod_core [nbnt]), q = &(psem2->prod_core [nbnt]);

		    do
		    {
			*q-- = *p--;
		    } while (p >= prod_core);
		}
	    }
	}

	return TRUE;
    }

    return FALSE;
}


static void
ksem_td_mark (Aij, i, j, x)
    int Aij, i, j, x;
{
    int			nbnt, h, k, delta, item, B, Bhk, prod, xsem, shift;
    struct ARN_ksem	*p;
    ushort		*prod_core;

    p = &(ARN_disp_ksem [Aij][x]);

    xsem = p - ARN_ksem;

#if EBUG
    if (xsem >= ARN_ksem_size)
	sxtrap (ME, "ksem_td_mark");
#endif

    if (SXBA_bit_is_reset_set (ARN_ksem_is_marked, xsem))
    {
	SXBA_1_bit (Aij_set, Aij);
	prod_core = &(p->prod_core [0]);
	prod = prod_core [0];
	nbnt = prod2nbnt [prod];

	if (nbnt > 0)
	{
	    k = (nbnt == 1) ? (j-prodXnt2nbt [prod][0]) : prod_core [1];

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
		    k = prod_core [x];
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

static void
ksem_bu_extract ()
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


static BOOLEAN
ARN_eval (prod, i, j, rhs_stack, val)
    int 	prod, i, j, *rhs_stack;
    float	*val;
{
    int		Aij, nbnt, Bhk;
    float	proba;

    Aij = rhs_stack [0];
    nbnt = prod2nbnt [prod];
    proba = probabilities [prod] * (j-i);;

    if (nbnt > 0)
    {
	do
	{
	    Bhk = rhs_stack [nbnt];
	    proba *= ARN_sem [Bhk].val;
	} while (--nbnt > 0);
    }

    *val = proba;

    return TRUE;
}



static BOOLEAN
ARN_parsact (i, j, prod_core, rhs_stack)
    int 	i, j;
    ushort	*prod_core;
    int		rhs_stack [];
{
    int 	Aij, prod;
    int		nbnt;
    float	val;
    ushort	*p, *q;
    
    prod = prod_core [0];

    if (ARN_eval (prod, i, j, rhs_stack, &val))
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

	    p = &(prod_core [nbnt]), q = &(ARN_sem [Aij].prod_core [nbnt]);

	    do
	    {
		*q-- = *p--;
	    } while (p >= prod_core);
	}

	return TRUE;
    }

    return FALSE;
}


static BOOLEAN
parse_item (A0k, item, I, i_left, j, k, nbnt)
    int 	A0k, i_left, item, j, k, nbnt;
    SXBA	I;
{
    /* A0k n'est significatif que ds le cas "EBUG" ou for_parsact.action != NULL. */
    /* rhs_stack n'est rempli que si for_parsact.action != NULL. */
    /* item = A -> \alpha . B \beta */
    /* Si I == NULL, i_left est positionne */
       
    int		i, new_i, nbt, B0j, Bij, Aik, prod, prdct_no;
    BOOLEAN     ret_val = FALSE, is_ok, is_prdct;
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
		    is_ok = TRUE;
	    
		if (is_ok && SXBA_bit_is_reset_set (ap_set, new_i))
		    ret_val = TRUE;
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
		    ret_val = TRUE;
	    }
	}
    }

    return ret_val;		/* ap_sets [A] a ete modifie */
}


static SXBA	cycle_red_order_set;


static SXBA		i_index_set, i2nt_nyp;
static int		**i2prod_stack, *gprod_stacks;
static int		tbp_item_stack [itemmax+1];
static int		blocked_stack [itemmax+1];
static SXBA		item_cycle_set, cyclic_citem_set;

static void
process_cycle (k, j)
    int		k, j;
{
    /* On cherche les items de blocked_stack impliques ds le cycle de plus haut niveau */
static int	rhs_stacks [prodmax+1][rhs_lgth+1];
static ushort	prod_cores [prodmax+1][rhs_lgth];
static int	tbp_stack [prodmax+1], citem2nb [itemmax+1],
                cur_item2next [itemmax+1], nt2cur_item [ntmax+1];

    int			*cur_rhs_stack;
    ushort		*cur_prod_core;
    int 		item, citem, cur_item, right_item, prod, cur_prod,
                        A, A0j, Aij, B, B0j, Bkj, x, nbnt, l, m, prdct_no, max, cycle_nb;
    int			*p, *q;
    BOOLEAN		is_high_level, should_loop, is_prdct;

    max = 0;

    for (x = blocked_stack [0]; x > 0; x--)
    {
	item =  blocked_stack [x]-1;
	cycle_nb = item2cycle_nb [item];

	if (cycle_nb > max) {
	    max = cycle_nb;
	    sxba_empty (item_cycle_set);
	    SXBA_1_bit (item_cycle_set, item);
	}
	else {
	    if (cycle_nb == max)
		SXBA_1_bit (item_cycle_set, item);
	}
    }

#if EBUG
    if (max == 0)
	sxtrap (ME, "process_cycle");
#endif


    /* C'est le cycle de + haut niveau */

    /* On l'enleve de blocked_stack */
    for (x = blocked_stack [0]; x > 0; x--)
    {
	item =  blocked_stack [x]-1;

	if (SXBA_bit_is_set (item_cycle_set, item))
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

    item = 0;

    while ((item = sxba_scan_reset (item_cycle_set, item)) > 0)
    {
	/* Si k == j, des items differents peuvent designer la meme prod. */
	B = lispro [item];
	prod = prolis [item];

	citem = (k == j) ? prolon [prod] : item+1;
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
		citem = (k == j) ? prolon [cur_prod] : cur_item+1;

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

	cur_prod_core = &(prod_cores [prod][0]);
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
	    cur_rhs_stack = &(rhs_stacks [prod] [0]);
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

	    if (cur_item == item)
		m = l;
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
	    should_loop = FALSE;

	    for (x = 1; x <= tbp_stack [0]; x++)
	    {
		if ((citem = tbp_stack [x]) > 0)
		{
		    prod = prolis [citem];
		    cur_prod_core = &(prod_cores [prod][0]);
		    cur_prod_core [0] |= HIBITS; /* On previent la semantique qu'il y a un
						    cycle et que cette production a deja
						    ete traitee. */
		    cur_rhs_stack = &(rhs_stacks [prod][0]);

		    if ((*for_parsact.action) (k, j, cur_prod_core, cur_rhs_stack))
			should_loop = TRUE;
		    else
			/* point fixe atteint, on l'enleve */
			tbp_stack [x] = 0;
		}
	    }
	} while (should_loop);
    }

    tbp_stack [0] = 0;
}


static void
mrr_generate (i, h, j, item)
    int i, h, j, item;
{
    /* item = Aij -> \alphaih Bhj . \betajj */
    /* Bhj \betajj sont evalues */
    int 	X, prod, A, A0j, Aij, cur_item, nbnt, nbt, k, B, B0j, Bkj, prdct_no;
    BOOLEAN	is_prdct;

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


static void
mutual_right_recursive (j, init_prod_order, out_prod_order)
     int j, init_prod_order, *out_prod_order;
{
    /* un ensemble de productions mutuellement recursives droites vient d'etre detecte'
       Les productions de la foret partagee sont triees dynamiquement de facon telle
       qu'elles ne sont executees que quand la semantique de la rhs est completement
       connue. */
    int 		prod_order, next_prod_order, prod, A, right_item, i, item, B, k, nbt;
    int			*i_prod_stack;
    struct PT2_item	*PT2j;
    SXBA 		index_set, backward_index_set, red_order_set;
    BOOLEAN 		nothing_has_changed, is_ok;

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
	    nothing_has_changed = TRUE;

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
			nothing_has_changed = FALSE;
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
				nothing_has_changed = FALSE;
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
				    nothing_has_changed = FALSE;
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
#if EBUG
		if (!is_cyclic)
		    sxtrap (ME, "mutual_right_recursive");
#endif

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



static void
parse ()
{
    struct PT2_item	*PT2j;
    int                 *PT2j_shift_NT_stack;
    SXBA	        backward_index_set, index_set, red_order_set;
    int		        item, new_item, new_j, j, prod_order, A, A0j, Aij, B, nbt, nbnt, prod, x;
    BOOLEAN             is_ok;
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
		    is_ok = TRUE;

		if (is_ok)
		    SXBA_1_bit (ap_sets [A], new_j);
	    }
	    else
	    {
		if (nbt == 0)
		{
		    /* item = A -> \alpha B . */
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



static void
sem_td_mark (Aij, i, j)
    int Aij, i, j;
{
    /* On genere recursivement toutes les productions dont Aij est la partie gauche. */
    ushort	*prod_core;
    int 	prod, nbnt, item, h, k, Bhk, B, delta, x;

    prod_core = &(ARN_sem [Aij].prod_core [0]);
    SXBA_1_bit (Aij_set, Aij);
    prod = prod_core [0];
    nbnt = prod2nbnt [prod];

    if (nbnt > 0)
    {
	k = (nbnt == 1) ? (j-prodXnt2nbt [prod][0]) : prod_core [1];

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
		k = prod_core [x];
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

static void
sem_bu_extract ()
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

static void
spf_count_init (size)
    int size;
{
    spf_count = (double*) sxcalloc (size, sizeof (double));
}


static void
spf_count_final (size)
    int size;
{
    sxfree (spf_count);
}


static BOOLEAN
spf_count_parsact (i, j, prod_core, rhs_stack)
    int 	i, j;
    ushort	*prod_core;
    int		rhs_stack [];
{
#define __NO_MATH_INLINES
#include <math.h>

    int		Aij;
    double	val;
    int 	prod, nbnt;

    prod = prod_core [0];
    Aij = rhs_stack [0];

    if (prod & HIBITS)
    {
	/* implique' ds un cycle et 2eme passage */
	spf_count [Aij] = HUGE_VAL; /* Produces IEEE Infinity. */
	return FALSE;
    }
    else
    {
	nbnt = prod2nbnt [prod]; 

	if (nbnt > 0)
	{
	    val = spf_count [rhs_stack [nbnt]];

	    while (--nbnt > 0)
		val *= spf_count [rhs_stack [nbnt]];
	}
	else
	    val = 1.0;

	spf_count [Aij] += val;

	return TRUE;
    }
}


static void
set_A2A0j ()
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


main (argc, argv)
    int		argc;
    char	*argv [];
{
    int 	l, t, *tok, nt, size, S0n;
    char	*s, c;

#include <ctype.h>

    if (argc != 3)
    {
	printf (Usage, ME);
	return 1;
    }

    sxopentty ();

    is_parser = FALSE;
    is_print_prod = FALSE;
    is_no_semantics = TRUE;
    is_default_semantics = TRUE;
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
		is_default_semantics = TRUE;
		is_no_semantics = FALSE;
		break;

	    case 'n':
		is_no_semantics = TRUE;
		break;

	    case 'p':
		if (!is_parser)
		    is_parser = TRUE;
		else
		    is_print_prod = TRUE;
		break;

	    case 'r':
		is_parser = FALSE;
		break;

	    case 's':
		is_default_semantics = FALSE;
		is_no_semantics = FALSE;
		break;

	    default:
		printf (Usage, ME);
		return 1;
	    
	    }
	}
    }

    print_time = TRUE;

    s = source = argv [2];
    n = strlen (source);
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

    if (print_time)
	sxtime (TIME_INIT);

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
	    bag_alloc (&pt2_bag, "pt2_bag", (n + 1) * prodmax * NBLONGS (n + 1));
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
		    item_cycle_set = sxba_calloc (itemmax+1);
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
		    sxfree (cyclic_citem_set);
		    sxfree (item_cycle_set);
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
