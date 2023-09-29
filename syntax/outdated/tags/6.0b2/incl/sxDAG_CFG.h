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
/* 20030319 10:48 (phd):	Utilisation de "sxtimestamp"		*/
/************************************************************************/
/* Mer 25 Sep 1996 14:33:35	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/* Realise l'intersection d'un automate fini non-deterministe avec une grammaire non
   contextuelle.

   FSA = (Q, T, \delta, qi, {qf})
   Any \in T
   \forall t \in T,\delta (p, t) \subset Q et 
   FSA' = FSA U {r ->EOF qi, qf ->EOF r'}
   EOF \not \in T
   FSA ne contient pas de transition sur le vide (on les incorpore :
   si p -> \eps q ->t r, on met p ->t r)
   On interdit la transition vide entre qi et qf si qi != qf
   Dans un premier temps on suppose que si q \in \delta (p, t) alors p <= q
*/

#include <values.h>
/* Definit la constante HIBITS == 1<<15 pour les mots de 32! */


BOOLEAN		is_default_semantics, is_print_prod, is_print_time, is_no_semantics, is_parse_tree_number;


#define TIME_INIT	0
#define TIME_FINAL	1

static void
sxtime (what, str)
    int what;
    char *str;
{
  if (what == TIME_INIT) {
    sxtimestamp (INIT, NULL);
  }
  else {
    fputs (str, sxtty);

    if (is_print_time)
      sxtimestamp (ACTION, NULL);
    else
      putc ('\n', sxtty);
  }
}


#define PUSH(s,x)	(s)[++*(s)]=(x)
#define POP(s)		(s)[(*(s))--]
#define IS_EMPTY(s)	(*(s)==0)

static BOOLEAN		is_semact_fun, is_constraint_fun, is_prdct_fun;

static int		n;

#define SXBA_fdcl(n,l)	SXBA_ELT n [NBLONGS (l) + 1] = {l,}

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


#if is_rcvr
static SXBA	*non_kernel_item_sets;
#else
static SXBA_fdcl (non_kernel_item_set, itemmax + 1);
#endif

static struct recognize_item {
    int		*shift_NT_hd [ntmax + 1];
    int		items_stack [itemmax + 1];
    SXBA	index_sets [itemmax + 1];
}			*RT;

static SXBA	*item_is_used;


#include		"XxY.h"
static XxY_header	FSA_pXq_hd, FSA_pXt_hd, FSA_pqXt_hd;
#define	FSA_any		0


static int	FSA_final_state;

static SXBA_fdcl (T1_item_set, itemmax + 1);
static SXBA_fdcl (T2_item_set, itemmax + 1);
static SXBA	FSA_states_tbp;


static BOOLEAN		T2_has_non_kernel, T2_has_changed;
static int		**T1_shift_NT_hd, **T2_shift_NT_hd;
static int		*T1_items_stack, *T2_items_stack;
/* static int		*T1_shift_state_stack, *T2_shift_state_stack; */
static SXBA             *T2_index_sets;
/* static int		*T1_kernel_NT_stack, *T2_kernel_NT_stack; */
static SXBA		T2_non_kernel_item_set;
static SXBA		T2_item_is_used;
static SXBA		T2_left_corner;

static SXBA		*RT_left_corner;

static bag_header	shift_bag;

static SXBA		*ntXindex_set;

#if is_cyclic
static int		cyclic_stack [ntmax+1];
#endif

static SXBA		nt_hd [ntmax+1];

#if is_parser
#include "semact.h"

#include "SS.h"


static int		max_tree_nb;

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


static	int		xprod;

static int		*A2A0j [ntmax+1];

#define MAKE_A0j(A,j)	(A2A0j [A][j])
#define MAKE_Aij(A0j,i)	((A0j)+(i))

static SXBA             *T2_backward_index_sets;

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

/* si p: Ail -> Xij Yjk Zkl alors prod_core [] = {p, j, k} */
static ushort		prod_core [rhs_lgth];


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
  int			*reduce_NT_hd [ntmax + 1];
/*  int			reduce_NT_hd [ntmax + 1]; */
}			*PT;

static int		**T2_reduce_NT_hd;

static SXBA		level0_index_set;

static bag_header	pt2_bag;

static struct PT2_item {
    int			backward_shift_stack [itemmax - prodmax + 1];
    SXBA		index_sets [itemmax + 1];
    SXBA		backward_index_sets [itemmax + 1];
} *PT2;


static SXBA	RL_nt_set, prod_order_set;
static SXBA	*ap_sets;
static SXBA     *red_order_sets;

struct ARN_sem {
  float		val;
  ushort	prod_core [rhs_lgth];
};

static struct ARN_sem	*ARN_sem;

struct ARN_ksem {
    float 		val;
    unsigned long	vector;
    /* int		vector [rhs_lgth]; */
    ushort		prod_core [rhs_lgth];
};

static int		log_max_tree_nb, filtre_max_tree_nb;
static int		ARN_ksem_size;
static struct ARN_ksem	*ARN_ksem, **ARN_disp_ksem, *ARN_ksem_top;
static SXBA		ARN_ksem_is_marked;

static struct kvals {
    float		val;
    unsigned long	vector;
    /* int 	vector [rhs_lgth]; */
} *kvals, **disp_kvals;

static SXBA	Aij_set;

#if EBUG
static SXBA     Aij_rhs_set, Aij_lhs_set;
#endif

static SXBA     Aij_sem_lhs_set;



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
	if ((val2 = ((*bits_ptr2--) & (*bits_ptr3--))))
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
	if ((val = *bits_ptr4-- = (*bits_ptr2--) & (*bits_ptr3--) & (~(*bits_ptr1)))) {
	    is_set = TRUE;
	    *bits_ptr1-- |= val;
	}
	else
	    bits_ptr1--;
    }

    return is_set;
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
	    *lhs_bits_ptr-- = (SXBA_ELT)0;
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

static BOOLEAN
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
    SXBA_ELT		filtre = (~((SXBA_ELT)0)) >> left_shift;
    SXBA_ELT		op2val = (SXBA_ELT)0, prev_op2val;
    BOOLEAN		ret_val = FALSE;

#if EBUG
    if (*op1_bits_array < *op2_bits_array)
	sxtrap (ME, "AND3_SHIFT");
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


#else 	/* is_parser */
#if is_constraints
#include "semact.h"
#endif
#endif	



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
    

static BOOLEAN
shift (item_p, item_q, p, q)
    int item_p, item_q, p, q;
{
    int		**ptr2;
    int 	A, B, X, Y, YY, prod, new_p, prdct_no;
    SXBA	index_set_p, index_set_q, backward_index_set_q;

    /* On fait la transition sur Apq ou A \in T U NT */
    /* item_p = B -> \beta . Apq \gamma */
    /* item_q = B -> \beta Apq . \gamma */

    /* Le look_ahead est verifie */
    X = A = lispro [item_p];
    Y = lispro [item_q];
    index_set_p = RT [p].index_sets [item_p];
    index_set_q = T2_index_sets [item_q];

#if is_parser
    new_p = p;
#endif

#if is_epsilon
    while (Y != 0)
#else
    if (Y != 0)
#endif
    {
	/* On ne peut pas verifier le predicat quand le source est un FSA, ca sera fait + tard
	   ds complete */
	YY = Y < 0 ? 0 : Y;

	if (index_set_q == NULL) {
	    if (*(ptr2 = &(T2_shift_NT_hd [YY])) == NULL) {
		/* c'est la 1ere fois qu'on rencontre YY */
		/* rhs_nt2where [0] == 0 (pour les terminaux) */
		*ptr2 = &(T2_items_stack [rhs_nt2where [YY]]);

		if (Y > 0 && !SXBA_bit_is_set (T2_left_corner, Y)) {
		    sxba_or (T2_left_corner, left_corner [Y]);
		    T2_has_non_kernel = TRUE;
		    sxba_or (T2_non_kernel_item_set, nt2item_set [Y]);
		}
	    }

	    *((*ptr2)++) = item_q;

	    index_set_q = T2_index_sets [item_q] = bag_get (&shift_bag, q+1);

#if is_parser
#if EBUG
	    if (T2_backward_index_sets [item_q] != NULL)
		sxtrap (ME, "shift");
#endif

	    backward_index_set_q = T2_backward_index_sets [item_q] = bag_get (&shift_bag, q+1);
#endif
	}
#if is_parser
	else {
	    /* index_set_q != NULL */
#if EBUG
	    if (T2_backward_index_sets [item_q] == NULL)
		sxtrap (ME, "shift");
#endif

	    backward_index_set_q = T2_backward_index_sets [item_q];
	}
#endif

	if (index_set_p == NULL)
	    SXBA_1_bit (index_set_q, p);
	else
	    OR (index_set_q, index_set_p);

#if is_parser
	SXBA_1_bit (backward_index_set_q, new_p);
#endif

#if is_epsilon
	if (Y > 0 && SXBA_bit_is_set (BVIDE, Y)) {
#if is_prdct
	    if (is_prdct_fun &&
		(prdct_no = prdct [item_q]) != -1 &&
		!(*for_semact.prdct) (1 /* No de passe : reconnaisseur */,
				      item_q, q, q, prdct_no)) {
		/* Le predicat a echoue */
		return FALSE;
	    }
#endif

	    item_q++;

	    if (!SXBA_bit_is_set (T2_item_set, item_q))
		/* Le look-ahead a echoue */
		return FALSE;

#if is_parser
	    new_p = q;
#endif

	    X = Y;
	    Y = lispro [item_q];
	    index_set_q = T2_index_sets [item_q];
	}
	else
	    return FALSE;	/* Pas de scan_reduce */
#else
	/* is_epsilon == FALSE */
	return FALSE;	/* Pas de scan_reduce */
#endif
    }

    /* Y == 0 */
    /* item_q = B -> \alpha A \beta .  et \beta =>* \epsilon */
    prod = prolis [item_q];
    B = lhs [prod];

#if is_constraints
    if (is_constraint_fun && (prdct_no = constraints [prod]) != -1) {
	if (index_set_p == NULL) {
	    if (!(*for_semact.constraint) (1 /* No de passe : reconnaisseur */,
					   prdct_no, prod, NULL, p, q))
		return FALSE;
	}
	else {
	    /* Il semble qu'on peut directement modifier index_set_p!! */
	    if (!(*for_semact.constraint) (1 /* No de passe : reconnaisseur */,
					   prdct_no, prod, index_set_p, p, q))
		return FALSE;
	    /* ici index_set_p a pu etre touche */
	}
    }
#endif

#if is_parser
    /* On ne stocke pas les reduces ds le cas du reconnaisseur. */
    /* Pas de backward apres un terminal */
    if (index_set_q == NULL) {
	if (*(ptr2 = &(T2_reduce_NT_hd [B])) == NULL) {
	    /* c'est la 1ere fois qu'on fait un reduce vers B */
	    *ptr2 = &(T2_items_stack [lhs_nt2where [B]]);
	}

	*((*ptr2)++) = item_q;
		

	index_set_q = T2_index_sets [item_q] = bag_get (&shift_bag, q+1);


#if EBUG
	if (T2_backward_index_sets [item_q] != NULL)
	    sxtrap (ME, "shift");
#endif

	backward_index_set_q = T2_backward_index_sets [item_q] = bag_get (&shift_bag, q+1);
    }
    else {
	backward_index_set_q = T2_backward_index_sets [item_q];
#if EBUG
	if (backward_index_set_q == NULL)
	    sxtrap (ME, "shift");
#endif	
    }

    if (index_set_p == NULL)
	SXBA_1_bit (index_set_q, p);
    else
	OR (index_set_q, index_set_p);

    SXBA_1_bit (backward_index_set_q, new_p);
#endif

    if (index_set_p == NULL)
    {
	/* item_q = B -> A .  */
#if is_cyclic
	if (A > 0 &&		/* pas ds le cas scan */
	    SXBA_bit_is_reset_set (ntXindex_set [B], p) &&
	    nt2order [B] > nt2order [A]) {
	    /* On est ds le cas cyclique, B est nouveau et son ordre de passage
	       a deja ete examine, on force son re-examen */
	    PUSH (cyclic_stack, B);
	}
	else
#endif
	    SXBA_1_bit (ntXindex_set [B], p);
    }
    else
	/* item_q = B -> \alpha A \beta .
	   et \beta =>* \epsilon
	   et \alpha \not =>* \epsilon */
	OR (ntXindex_set [B], index_set_p);

    return TRUE;		/* is_scan_reduce */
}




static void
reduce (A, p, q)
    int A, p, q;
{
    /* 0 <= p < q <= n+1 */
    /* Toutes les reductions vers Ap ont ete effectuees
       Cas du reconnaisseur : on fait le "shift" sur A dans la table "p".
       cas du parser : en plus, on fait les reductions eventuelles B -> alpha Apq */
    int				item, new_item, *top_ptr, *bot_ptr, prdct_no;
    struct recognize_item	*RTp = &(RT [p]);

    top_ptr = RTp->shift_NT_hd [A];
#if EBUG
    if (top_ptr == NULL)
	sxtrap (ME, "reduce");
#endif
    bot_ptr = &(RTp->items_stack [rhs_nt2where [A]]);

    while (--top_ptr >= bot_ptr) {
	item = *top_ptr;
	/* item = B -> \beta . A \gamma */
	new_item = item+1;
	/* new_item = B -> \beta A . \gamma */

	if (SXBA_bit_is_set (T2_item_set, new_item)
#if is_prdct
	    && (!is_prdct_fun ||
		(prdct_no = prdct [item]) == -1 ||
		(*for_semact.prdct) (1 /* No de passe : reconnaisseur */,
				     item, p, q, prdct_no))
#endif
	    ) {
	    shift (item, new_item, p, q);
	}
    }
}



static void
scan_reduce (p, q)
    int 	p, q;
{
    register SXBA_ELT	filtre;
    register int	r, indice, order, A;

    filtre = 1 << MOD (p);
    indice = DIV (p) + 1;

    for (A = ntmax; A > 0; A--)
	nt_hd [A] = ntXindex_set [A] + indice;

    r = p;

    for (;;)
    {
	for (order = ntmax; order > 0; order--)
	{
	    A = order2nt [order];

	    /* if (SXBA_bit_is_set_reset (ntXindex_set [A], r)) */
	    if (*(nt_hd [A]) & filtre)
		reduce (A, r, q);
	}

#if is_cyclic
	if (!IS_EMPTY (cyclic_stack))
	{
	    do
	    {
		reduce (POP (cyclic_stack), r, q);
	    } while (!IS_EMPTY (cyclic_stack));
	}
#endif

	if ((filtre >>= 1) == 0)
	{
	    for (A = ntmax; A > 0; A--)
		*((nt_hd [A])--) = 0;

	    if (--r < 0)
		break;

	    filtre = 1 << MOD (r);
	}
	else
	    r--;
    }
}


static void
process_non_kernel (q)
    int q;
{
    /* On prepare les transitions depuis q en stockant ds T2 les items non-kernel (scan ou nt_trans). */
    int		**ptr2;
    int		item, A, X, prdct_no, prod;
    SXBA	index_set;
#if is_epsilon
    BOOLEAN	is_initial_item;
#endif
#if parser
    SXBA	backward_index_set;
#endif	

    sxba_and (T2_non_kernel_item_set, T2_item_set);

    item = -1;
    
    while ((item = 
#if is_rcvr
	    sxba_scan
#else
	    sxba_scan_reset
#endif
	    (T2_non_kernel_item_set, item)) >= 0) {
	/* item = A -> \alpha . X beta et \alpha =>* \epsilon et aq \in FIRST1 (A -> \alpha . X beta) */
	X = lispro [item];

#if is_epsilon
	index_set = T2_index_sets [item];
#if EBUG
	if (item == 0)
	    sxtrap (ME, "process_non_kernel");
#endif
	is_initial_item = lispro [item-1] == 0;
#endif

	if (X < 0) {
	    /* X == aq */
	    /* On ne peut pas verifier le predicat quand le source est un FSA, ca sera fait + tard
	       ds complete */
#if is_epsilon
	    if (SXBA_bit_is_reset_set (T2_item_is_used, item)) {
		/* C'est la premiere fois qu'on trouve item dans T2. */
		if (*(ptr2 = &(T2_shift_NT_hd [0])) == NULL) {
		    /* c'est la 1ere fois qu'on rencontre un terminal */
		    /* rhs_nt2where [0] == 0 (pour les terminaux) */
		    *ptr2 = &(T2_items_stack [0]);
		}

		*((*ptr2)++) = item;

		/* Si on n'est pas en debut de partie droite, il faut creer un index_set */
		if (!is_initial_item) {
		    index_set = T2_index_sets [item] = bag_get (&shift_bag, q+1);
		    SXBA_1_bit (index_set, q);
#if parser
		    backward_index_set = T2_backward_index_sets [item] = bag_get (&shift_bag, q+1);
		    SXBA_1_bit (backward_index_set, q);
#endif	
		}
	    }
	    else {
		if (!is_initial_item) {
#if EBUG
		    if (index_set == NULL)
			sxtrap (ME, "process_non_kernel");
#endif

		    /* \alpha \in NT+ et item a deja ete fabrique par "reduce ()". */
		    SXBA_1_bit (index_set, q);

#if parser
		    backward_index_set = T2_backward_index_sets [item];

#if EBUG
		    if (backward_index_set == NULL)
			sxtrap (ME, "process_non_kernel");
#endif

		    SXBA_1_bit (backward_index_set, q);
#endif
		}
	    }
#else
	    /* is_epsilon == FALSE */
	    /* On a obligatoirement \alpha == \epsilon et T2_index_sets [item] == NULL */
	    if (SXBA_bit_is_reset_set (T2_item_is_used, item)) {
		/* C'est la premiere fois qu'on trouve item dans T2. */
		if (*(ptr2 = &(T2_shift_NT_hd [0])) == NULL) {
		    /* c'est la 1ere fois qu'on rencontre un terminal */
		    /* rhs_nt2where [0] == 0 (pour les terminaux) */
		    *ptr2 = &(T2_items_stack [0]);
		}

		*((*ptr2)++) = item;
	    }
#endif
	}
	else {
	    /* X >= 0 */
	    /* A -> \alpha . X \beta et \alpha =>* \epsilon et ai \in FIRST1 (A -> \alpha . X \beta) */
	    /* Cet item peut cependant ne pas etre valide si le predicat associe au ai derive est faux.
	       Dans ce cas, il ne sera pas active par les sequences de reductions. */
#if is_epsilon
#if is_prdct
	    if (X > 0 &&
		SXBA_bit_is_set (BVIDE, X) &&
		is_prdct_fun &&
		(prdct_no = prdct [item]) != -1 &&
		!(*for_semact.prdct) (1 /* No de passe : reconnaisseur */,
				      item, q, q, prdct_no)) {
		/* Le predicat a echoue */
		/* On supprime les items de la meme production */
		item2 = item;

		do {
		    item2++;

		    if (!SXBA_bit_is_set_reset (T2_non_kernel_item_set, item2))
			break;
		} while (lispro [item2] != 0);

		continue;
	    }
#endif

	    if (SXBA_bit_is_reset_set (T2_item_is_used, item)) {
		/* C'est la premiere fois qu'on trouve item dans T2. */

		if (X > 0) {
		    if (*(ptr2 = &(T2_shift_NT_hd [X])) == NULL) {
			/* c'est la 1ere fois qu'on est devant un X */
			*ptr2 = &(T2_items_stack [rhs_nt2where [X]]);
		    }

		    *((*ptr2)++) = item;
		}
		else {
		    /* X == 0 */
		    /* item = A -> \alpha . et \alpha =>* \epsilon */
		    prod = prolis [item];

#if is_constraints
		    if (is_constraint_fun &&
			(prdct_no = constraints [prod]) != -1 &&
			!for_semact.constraint (1 /* No de passe : reconnaisseur */,
						prdct_no, prod, NULL /* index_set */, q, q)) {
			continue;
		    }
#endif
		    A = lhs [prod];

		    if (*(ptr2 = &(T2_reduce_NT_hd [A])) == NULL) {
			/* c'est la 1ere fois qu'on fait un reduce vers A */
			*ptr2 = &(T2_items_stack [lhs_nt2where [A]]);
		    }

		    *((*ptr2)++) = item;
		}
	    }

	    if (!is_initial_item) {
		/* On n'est pas en debut de partie droite */
		/* Le symbole precedent est un nt */
		/* On positionne index_set et backward_index_set */
		/* Si index_set est non NULL, il peut etre modifie de fac,on safe, il n'est
		   pas commun avec une autre table. */
		/* On pourrait meme utiliser l'index_set de item-1 si non NULL */
		if (index_set == NULL)
		    index_set = T2_index_sets [item] = bag_get (&shift_bag, q+1);

		SXBA_1_bit (index_set, q);
#if parser
		if ((backward_index_set = T2_backward_index_sets [item]) == NULL)
		    backward_index_set = T2_backward_index_sets [item] = bag_get (&shift_bag, q+1);

		SXBA_1_bit (backward_index_set, q);
#endif
	    }
#else
	    /* is_epsilon == FALSE */
	    /* On est obligatoirement en debut de partie droite */
	    /* X > 0 et T2_index_sets [item] == NULL */
	    if (SXBA_bit_is_reset_set (T2_item_is_used, item)) {
		/* C'est la premiere fois qu'on trouve item dans T2. */
		if (*(ptr2 = &(T2_shift_NT_hd [X])) == NULL) {
		    /* c'est la 1ere fois qu'on est devant un X */
		    *ptr2 = &(T2_items_stack [rhs_nt2where [X]]);
		}

		*((*ptr2)++) = item;
	    }
#endif
	}
    }
}


/* ATTENTION CHANGEMENT :
   Un symbole Aij ou i et j sont deux numeros de table signifie que :
   Aij =>* ai ai+1 ... aj-1 */

/* On commence par traiter le cas ou le FSA est un DAG */

static BOOLEAN
complete (p, q)
    int p, q;
{
    /* Traitement de la transition (p,q) du FSA */
    /* Les donnees d'entree sont dans T1 */
    /* T1_shift_NT_hd [0] repere la liste des items terminaux A -> \alpha .t \beta tels que
       \delta(p,t) est non vide */
    /* on va remplir T2 (initialement vide) */
    /* T1_item_set contient le sous-ensemble des items derivant ds les terminaux sur lesquels il existe des
       transitions depuis p vers q. */
    /* T2_item_set contient les items derivant ds un terminal sur lequel il existe une
       transition depuis q */

    /* Les predicats des items terminaux ne sont pas verifies (on ne connaissait pas les etats q) */
    /* Le look_ahead des items de T1 n'est pas verifie. */
    int				item, next_item, prdct_no;
    int				*top_ptr, *bot_ptr;
    BOOLEAN			is_scan_reduce = FALSE;

    top_ptr = T1_shift_NT_hd [0];

#if EBUG
    if (top_ptr == NULL)
	sxtrap (ME, "complete");
#endif

    /* Il y a des items de la forme : A -> \alpha . ai \beta */
    bot_ptr = &(T1_items_stack [0]);

    while (--top_ptr >= bot_ptr) {
	item = *top_ptr;

	if (SXBA_bit_is_set (T1_item_set, item)
#if is_prdct
	    && (!is_prdct_fun
		|| (prdct_no = prdct [item]) == -1
		|| (*for_semact.prdct) (1 /* No de passe : reconnaisseur */,
					item, p, q, prdct_no))
#endif
	    ) {
	    /* C'est une bonne (predicat compris) transition terminale entre p et q */
	    next_item = item+1;

	    if (SXBA_bit_is_set (T2_item_set, next_item)) {
		/* De plus le look-ahead est verifie */
		if (shift (item, next_item, p, q))
		    is_scan_reduce = TRUE;
	    }
	}
    }

    if (is_scan_reduce)
	scan_reduce (p, q);

    if (T2_has_non_kernel)
	process_non_kernel (q);

    return (T2_shift_NT_hd [0] != NULL);
}



static void
process_FSA_state (p)
    int p;
{
    /* RT [p] est non vide, on calcule les parties des RT [q] dues aux transitions depuis p. */
    int				q, pXq, qXt, t, pqXt;
    struct recognize_item	*RTp, *RTq;
    struct parse_item		*PTq;

    RTp = &(RT [p]);

    T1_shift_NT_hd = &(RTp->shift_NT_hd [0]);
    T1_items_stack = &(RTp->items_stack [0]);

    XxY_Xforeach (FSA_pXq_hd, p, pXq) {
	q = XxY_Y (FSA_pXq_hd, pXq);

	if (q > FSA_final_state)
	    continue;

	RTq = &(RT [q]);

	T2_has_non_kernel = FALSE;
	T2_shift_NT_hd = &(RTq->shift_NT_hd [0]);
	T2_items_stack = &(RTq->items_stack [0]);
	T2_index_sets = &(RTq->index_sets [0]);
	T2_left_corner = RT_left_corner [q];
	T2_item_is_used = item_is_used [q];

#if is_rcvr
	T2_non_kernel_item_set = non_kernel_item_sets [q];
#endif

#if is_parser
	PTq = &(PT [q]);
	T2_backward_index_sets = &(PTq->backward_index_sets [0]);
	T2_reduce_NT_hd = &(PTq->reduce_NT_hd [0]);
#endif

	sxba_empty (T2_item_set);

	XxY_Xforeach (FSA_pXt_hd, q, qXt) {
	    t = XxY_Y (FSA_pXt_hd, qXt);
	    sxba_or (T2_item_set, t2item_set [t]);
	}

	sxba_empty (T1_item_set);

	XxY_Xforeach (FSA_pqXt_hd, pXq, pqXt) {
	    t = XxY_Y (FSA_pqXt_hd, pqXt);
	    sxba_or (T1_item_set, t2item_set [t]);
	}

	if (complete (p, q))
	    SXBA_1_bit (FSA_states_tbp, q);
    }
}



static BOOLEAN
recognize ()
{
    int			p;
    BOOLEAN		is_in_LG;
    SXBA		index_set;

    /* A AJOUTER DS LALC1 */
    SXBA_1_bit (t2item_set [-tmax], 0); /*  S' -> . $ S $  derive ds $ */

    RT [0].items_stack [0] = 0; /*  S' -> . $ S $ */
    RT [0].shift_NT_hd [0] = &(RT [0].items_stack [1]);
    SXBA_1_bit (FSA_states_tbp, 0);
    sxplocals.atok_no = -1;

    while ((p = sxba_scan (FSA_states_tbp, sxplocals.atok_no)) >= 0 &&
	    p <= FSA_final_state) {
	process_FSA_state (sxplocals.atok_no = p);
    }

    is_in_LG = sxplocals.atok_no == FSA_final_state &&
	(index_set = RT [FSA_final_state].index_sets [2]) != NULL && SXBA_bit_is_set (index_set, 0);

    if (!is_in_LG) {
	printf ("Syntax error.\n");
    }

    return is_in_LG;
}



#if is_parser

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


static void
RL_mreduction_item (item_j, I, j)
    int 	item_j, j;
    SXBA	I;
{
    /* item_j = A -> \alpha . \gamma  et \alpha != \epsilon */
    int				Y, bot_item, i, k, Z, item_k, item, order;
    SXBA			backward_index_set, prev_index_set;
    SXBA			/* tbp_set, */ ap_set;
    struct recognize_item	*RTj;
    struct parse_item		*PTj;
    struct PT2_item		*PT2j;
    BOOLEAN			is_new;

    SXBA			ids, nbis;
    int				*top_ptr, *bot_ptr;


    bot_item = prolon [prolis [item_j]];

    PTj = &(PT [j]);
    PT2j = &(PT2 [j]);

    item_k = item_j - 1;
    Y = lispro [item_k];
    /* item_k = X -> \alpha . Y x \gamma */
    if (Y > 0) {
	ap_set = ap_sets [Y];

	if (is_new = SXBA_bit_is_reset_set (RL_nt_set, Y))
	{
	    ap_set [0] = j+1;	/* On change la taille!! */

	    RTj = &(RT [j]);

	    top_ptr = PTj->reduce_NT_hd [Y];
#if EBUG
	    if (top_ptr == NULL)
		sxtrap (ME, "RL_mreduction_item");
#endif
	    bot_ptr = &(RTj->items_stack [lhs_nt2where [Y]]);

	    while (--top_ptr >= bot_ptr) {
		item = *top_ptr;
		order = prod2order [prolis [item]];
		SXBA_1_bit (prod_order_set, order);
	    }
	}
    }

    nbis = PT2j->backward_index_sets [item_j];

    if (item_k == bot_item)
    {
	/* \alpha = \epsilon */
	if (Y > 0)
	    OR (ap_set, I);
	
	/* item_k = X -> . Y \gamma */
	if (nbis == NULL)
	    PT2j->backward_index_sets [item_j] = PT2j->index_sets [item_j];
    }
    else
    {
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
		    nbis = PT2j->backward_index_sets [item_j] = bag_get (&pt2_bag, j+1);

		SXBA_1_bit (nbis, k);

		if (Y > 0 && (is_new || !SXBA_bit_is_set (ap_set, k)))
		{
		    SXBA_1_bit (ap_set, k);
		}

#if is_epsilon
		if (k == j)
		{
		    if ((ids = PT2j->index_sets [item_k]) == NULL)
			ids = PT2j->index_sets [item_k] = bag_get (&pt2_bag, j+1);

		    if (OR_AND (ids, prev_index_set, I))
		    {
			/* Il y a du nouveau */
			RL_mreduction_item (item_k, ids, j);
		    }
		}
		else
#endif
		{
		    if ((ids = PT2 [k].index_sets [item_k]) == NULL)
		    {
			ids = PT2 [k].index_sets [item_k] = bag_get (&pt2_bag, k+1);
			PUSH (PT2 [k].backward_shift_stack, item_k);
		    }

		    OR_AND (ids, prev_index_set, I);
		}
	    }
	}
    }
}

#if 0
static void
RL_mreduction ()
{
    int				i, j, A, order, new_order, A0j, Aij;
    SXBA			ap_set;
    struct parse_item		*PTj;
    struct recognize_item	*RTj;
    struct PT2_item		*PT2j;

    int				*PT2j_backward_shift_stack, x, item, prod, X;
    SXBA			*PT2j_index_set, *PT2j_backward_index_set, I, B;
    SXBA			index_set, new_index_set, red_order_set, wis;
    BOOLEAN			is_good, is_rr, is_modif;

    RL_nt_set = sxba_calloc (ntmax+1);
    prod_order_set = sxba_calloc (prodmax+1);

    wis = sxba_calloc (/*n+1*/n+1);
	
    index_set = PT2 [/*n+1*/n].index_sets [2 /* S' -> $ S. $ */] = bag_get (&pt2_bag, /*n+1*/n+1);
    SXBA_1_bit (index_set, 0); /* {0} */

    PUSH (PT2 [/*n+1*/n].backward_shift_stack, 2); /* S' -> $ S. $ */

    for (j = /*n+1*/n; j >= 0; j--) {
	PT2j = &(PT2 [j]);
	PTj = &(PT [j]);
	RTj = &(RT [j]);
#if 0
	/* fait ds OR_AND_MINUS! */
	wis [0] = j+1; /* On change la taille!! */
#endif
	PT2j_index_set = &(PT2j->index_sets [0]);
	PT2j_backward_index_set = &(PT2j->backward_index_sets [0]);
	PT2j_backward_shift_stack = &(PT2j->backward_shift_stack [0]);

	for (x = PT2j_backward_shift_stack [0]; x > 0; x--) {
	    item = PT2j_backward_shift_stack [x];
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
	is_rr = FALSE;
	order = 0;

	while ((order = sxba_scan (prod_order_set, order)) > 0) {
	    prod = order2prod [order];
	    item = prolon [prod+1]-1;
	    A = lhs [prod];
	    ap_set = ap_sets [A];

	    is_good = FALSE;
				
#if is_epsilon
	    if (item == prolon [prod])
	    {
		/* item = A -> . */
		if (SXBA_bit_is_set (ap_set, j))
		    is_good = TRUE;
	    }
	    else
#endif
	    {
		/* item = A -> \alpha B . */
		index_set = RTj->index_sets [item];

		if ((new_index_set = PT2j->index_sets [item]) == NULL)
		    PT2j->index_sets [item] = new_index_set = bag_get (&pt2_bag, j+1);

		if (OR_AND_MINUS (new_index_set, index_set, ap_set, wis))
		    /* calcule wis = index_set & ap_set - new_index_set
		       et new_index_set |= wis
		       retourne vrai ssi wis est non vide */
		{
		    /* Il y a du nouveau */

		    /* MODIF: c'est wis qui decide l'arret des boucles */
		    if (is_rr)
			is_modif = TRUE;

		    RL_mreduction_item (item, wis, j);
		    is_good = TRUE;
		}

		new_order = prod_order2next [order];

		if (new_order < order) {
		    /* Recursivite droite => boucle */
		    if (!is_rr) {
			/* 1ere detection de la fin de la boucle */
			is_rr = TRUE;
			is_modif = FALSE;
			order = new_order; /* On reboucle */
		    }
		    else {
			/* Fin du 2eme, 3eme, ... parcourt */
			if (is_modif) {
			    is_modif = FALSE;
			    order = new_order; /* On reboucle */
			}
			else
			    is_rr = FALSE; /* On continue en sequence */
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

	if (for_semact.need_pack_unpack || for_semact.need_Aij2A_i_j) {
	    while ((A = sxba_scan_reset (RL_nt_set, A)) > 0) {
		A0j = MAKE_A0j (A, j);
		ap_set = ap_sets [A];

		i = -1;

		while((i = sxba_scan_reset (ap_set, i)) >= 0) {
		    Aij = MAKE_Aij (A0j, i);

		    if (for_semact.need_Aij2A_i_j) {
			/* Seuls les "i" utiles sont entres. */
			Aij2A_i_j [Aij] = i;
		    }

		    if (for_semact.need_pack_unpack) {
			Aij2pAij [Aij] = ++pAij_top;
			pAij2Aij [pAij_top] = Aij;
		    }
		}
	    }
	}
	else {
	    while ((A = sxba_scan_reset (RL_nt_set, A)) > 0) {
		sxba_empty (ap_sets [A]);
	    }
	}

	sxba_empty (prod_order_set);
    }
    
    sxfree (RL_nt_set);
    sxfree (prod_order_set);
    sxfree (wis);
}
#endif

static void
RL_mreduction ()
{
    int				i, j, A, order, A0j, Aij;
    SXBA			ap_set;
    struct parse_item		*PTj;
    struct recognize_item	*RTj;
    struct PT2_item		*PT2j;

    int				*PT2j_backward_shift_stack, x, item, prod, X;
    SXBA			*PT2j_index_set, *PT2j_backward_index_set, I, B;
    SXBA			index_set, new_index_set, red_order_set, wis;
    BOOLEAN			is_good;
#if is_right_recursive
    int				max_order, maxo;
#endif

    RL_nt_set = sxba_calloc (ntmax+1);
    prod_order_set = sxba_calloc (prodmax+1);

    wis = sxba_calloc (		/*n+1*/n+1);
	
    index_set = PT2 [		/*n+1*/n].index_sets [2 /* S' -> $ S. $ */] = bag_get (&pt2_bag, /*n+1*/n+1);
    SXBA_1_bit (index_set, 0);	/* {0} */

    PUSH (PT2 [			/*n+1*/n].backward_shift_stack, 2); /* S' -> $ S. $ */

    for (j =			/*n+1*/n; j >= 0; j--) {
	PT2j = &(PT2 [j]);
	PTj = &(PT [j]);
	RTj = &(RT [j]);
#if 0
	/* fait ds OR_AND_MINUS! */
	wis [0] = j+1;		/* On change la taille!! */
#endif
	PT2j_index_set = &(PT2j->index_sets [0]);
	PT2j_backward_index_set = &(PT2j->backward_index_sets [0]);
	PT2j_backward_shift_stack = &(PT2j->backward_shift_stack [0]);

	for (x = PT2j_backward_shift_stack [0]; x > 0; x--) {
	    item = PT2j_backward_shift_stack [x];
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
	if ((order = sxba_scan_reset (prod_order_set, 0)) > 0) {
#if is_right_recursive
	    max_order = prodmax+1;
#endif

	    do {
		prod = order2prod [order];
		item = prolon [prod+1]-1;
		A = lhs [prod];
		ap_set = ap_sets [A];

		is_good = FALSE;

#if is_epsilon
		if (item == prolon [prod])
		{
		    /* item = A -> . */
		    if (SXBA_bit_is_set (ap_set, j))
			is_good = TRUE;
		}
		else
#endif
		{
		    /* item = A -> \alpha B . */
		    index_set = RTj->index_sets [item];

		    if ((new_index_set = PT2j->index_sets [item]) == NULL)
			PT2j->index_sets [item] = new_index_set = bag_get (&pt2_bag, j+1);

		    if (OR_AND_MINUS (new_index_set, index_set, ap_set, wis))
			/* calcule wis = index_set & ap_set - new_index_set
			   et new_index_set |= wis
			   retourne vrai ssi wis est non vide */
		    {
			/* Il y a du nouveau */
			/* MODIF: c'est wis qui decide l'arret des boucles */
			RL_mreduction_item (item, wis, j);
			is_good = TRUE;
		    }
		}

		if (is_good)
		{
		    SXBA_1_bit (red_order_set, order);

#if is_right_recursive
		    if ((maxo = prod_order2max [order]) != 0) {
			/* production recursive droite */
			SXBA_1_bit (prod_order_set, order); /* devra etre reexecutee */
			max_order = maxo;
		    }
#endif	    
		}

		order = sxba_scan_reset (prod_order_set, order);

#if is_right_recursive
		if (order > max_order || order == -1 && max_order < prodmax+1) {
		    /* Il y a des productions recursives droites qui ont deja ete executees car
		       max_order < prodmax+1
		       On vient de terminer un parcourt de la boucle (order > max_order)
		       */
		    SXBA_1_bit (prod_order_set, order); /* On le remet */
		    order = prod_order2min [order]-1; /* ordre du debut de boucle-1 */
		    max_order = prodmax+1;
		    order = sxba_scan_reset (prod_order_set, order);
		}
#endif
	    } while (order > 0);

	    A = 0;

	    if (for_semact.need_pack_unpack || for_semact.need_Aij2A_i_j) {
		while ((A = sxba_scan_reset (RL_nt_set, A)) > 0) {
		    A0j = MAKE_A0j (A, j);
		    ap_set = ap_sets [A];

		    i = -1;

		    while((i = sxba_scan_reset (ap_set, i)) >= 0) {
			Aij = MAKE_Aij (A0j, i);

			if (for_semact.need_Aij2A_i_j) {
			    /* Seuls les "i" utiles sont entres. */
			    Aij2A_i_j [Aij] = i;
			}

			if (for_semact.need_pack_unpack) {
			    Aij2pAij [Aij] = ++pAij_top;
			    pAij2Aij [pAij_top] = Aij;
			}
		    }
		}
	    }
	    else {
		while ((A = sxba_scan_reset (RL_nt_set, A)) > 0) {
		    sxba_empty (ap_sets [A]);
		}
	    }
	}
    }
    
    sxfree (RL_nt_set);
    sxfree (prod_order_set);
    sxfree (wis);
}


static void
print_symb (symb, i, j)
    int	   symb, i, j;
{
    int 	ste;
    char	*str;

    if (symb < 0)
    {
#if 0
	ste = SXGET_TOKEN (i).string_table_entry;
#endif
	ste = EMPTY_STE;
	str = ste == EMPTY_STE ? tstring [-symb] : sxstrget (ste);
    }
    else
    {
	str = ntstring [symb];
    }

    printf ("%s[%i..%i] ", str, i, j);
}

static void
print_prod (i, k, prod_core)
    int 	i, k;
    ushort	*prod_core;
{
    int		prod, h, X, Y, j, A, x, item;

    prod = prod_core [0];
    A = lhs [prod];
    xprod++;
    printf ("%i\t: ", xprod);
    print_symb (A, i, k);
    fputs ("\t = ", stdout);

    x = 0;
    j = i;
    item = prolon [prod];

    if ((Y = lispro [item]) != 0) {
	do {
	    X = Y;
	    Y = lispro [++item];
	    h = Y == 0 ? k : prod_core [++x];
	    print_symb (X, j, h);
	    j = h;
	} while (Y != 0);
#if EBUG
    if (j != k)
	sxtrap (ME, "print_prod");
#endif
    }

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
    proba = (j-i)/n;

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
ARN_ksemact (i, j, prod_core, rhs_stack)
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
    proba = (j-i)/n;

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
ARN_semact (i, j, prod_core, rhs_stack)
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
	Aij = rhs_stack [0];

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


static void
parse_item (A0k, item, I, i_left, j, k, nbnt)
    int 	A0k, i_left, item, j, k, nbnt;
    SXBA	I;
{
    /* A0k n'est significatif que ds le cas "EBUG" ou for_semact.semact != NULL. */
    /* rhs_stack n'est rempli que si for_semact.semact != NULL. */
    /* item = AIk -> X ... Y . B?j \betajk et |X ... Y B| == nbnt > 0 */
    /* Si I == NULL, i_left est positionne */
       
    int		i, new_i, B, Z, B0j, Bij, Aik, prod, prdct_no;
    BOOLEAN     is_ok, is_local_prdct;
    SXBA        backward_J, ap_set, index_set;

    backward_J = PT2 [j].backward_index_sets [item+1];

    prod = prolis [item];

    /* Ca permet d'appeler la semantique par defaut (comptage des arbres) meme si certaines regles
       n'ont pas d'actions. */
    is_local_prdct =
#if is_prdct
	is_prdct_fun && (prdct_no = prdct [item]) != -1
#else
	    FALSE
#endif
		;

#if EBUG
    if ((B = lispro [item]) > 0)
	B0j = MAKE_A0j (B, j);
#else
    if ((is_semact_fun || is_local_prdct) && (B = lispro [item]) > 0)
	B0j = MAKE_A0j (lispro [item], j);
#endif

    if ((Z = lispro [item+1]) != 0)
	prod_core [nbnt] = j;

    if (nbnt == 1)
    {
	/* |X ... Y| == 0 : item est initial */
	if (I != NULL)
	{
	    if (Z != 0)
	    {
		AND3 (level0_index_set, I, backward_J);
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
	    i = new_i;

	    /* Dans tous les cas Bij a une valeur semantique differente de bottom. */
	    /* Cette valeur peut eventuellement encore evoluer ds le cas de grammaires cycliques!.
	       Mais il ne faut pas regenerer les memes productions, seulement reevaluer la sem. */
#if EBUG
	    Aik = MAKE_Aij (A0k, new_i);

	    if (B > 0) {
		Bij = MAKE_Aij (B0j, i);

		if (!SXBA_bit_is_set (Aij_lhs_set, Bij))
		    sxtrap (ME, "parse_item");

		SXBA_1_bit (Aij_rhs_set, Bij);
	    }

	    if (SXBA_bit_is_set (Aij_rhs_set, Aik))
		sxtrap (ME, "parse_item");

	    SXBA_1_bit (Aij_lhs_set, Aik);
#endif

	    if ((is_semact_fun || is_local_prdct) && B > 0)
		Bij = MAKE_Aij (B0j, i);

	    if (!is_local_prdct || (*for_semact.prdct) (2 /* No de passe : analyseur */,
							item, i, j, prdct_no))
	    {
		if (is_print_prod)
		    print_prod (new_i, k, prod_core);
		else
		    xprod++;

		if (is_semact_fun)
		{
		    rhs_stack [1] = B > 0 ? (MAKE_Aij (B0j, i)) : 0;
		    rhs_stack [0] = MAKE_Aij (A0k, new_i);
		    is_ok = (*for_semact.semact) (new_i, k, prod_core, rhs_stack);
		}
		else
		    is_ok = TRUE;
	    
		if (is_ok)
		    SXBA_1_bit (ap_set, new_i);
	    }

	    if (index_set != NULL)
		new_i = sxba_1_rlscan (index_set, new_i);
	    else
		new_i = -1;
	}
    }
    else
    {
	i = j+1;
	
	while ((i = sxba_1_rlscan (backward_J, i)) >= 0)
	{
#if EBUG
	    if (B > 0) {
		Bij = MAKE_Aij (B0j, i);

		if (!SXBA_bit_is_set (Aij_lhs_set, Bij))
		    sxtrap (ME, "parse_item");

		SXBA_1_bit (Aij_rhs_set, Bij);
	    }
#endif

	    if ((is_semact_fun || is_local_prdct) && B > 0)
		Bij = MAKE_Aij (B0j, i);

	    if (!is_local_prdct || (*for_semact.prdct) (2 /* No de passe : analyseur */,
							item, i, j, prdct_no))
	    {
		if (is_semact_fun)
		    rhs_stack [nbnt] = B > 0 ? Bij : 0;

		parse_item (A0k, item-1, I, i_left, i, k, nbnt-1);
	    }
	}
    }
}



static int		tbp_item_stack [itemmax+1];
static int		blocked_stack [itemmax+1];

#if is_right_recursive
static SXBA		i_index_set, i2nt_nyp;
static int		**i2prod_stack, *gprod_stacks;
#endif


#if is_cyclic
static SXBA		item_cycle_set, cyclic_citem_set, cycle_red_order_set;

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
    BOOLEAN		is_high_level, should_loop, is_local_prdct;

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

    item = -1;

    while ((item = sxba_scan_reset (item_cycle_set, item)) >= 0)
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
		
	if (is_semact_fun)
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

	    is_local_prdct =
#if is_prdct
		is_prdct_fun && (prdct_no = prdct [cur_item-1]) != -1
#else
		    FALSE
#endif
			;
	    if (is_semact_fun || is_local_prdct)
	    {
		B0j = MAKE_A0j (B, m);
		Bkj = MAKE_Aij (B0j, l);

		if (is_semact_fun)
		    cur_rhs_stack [nbnt] = Bkj;
	    }

	    if (is_local_prdct && !(*for_semact.prdct) (2 /* No de passe : analyseur */,
							cur_item-1, l, m, prdct_no))
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

	    if (!is_semact_fun || (*for_semact.semact) (k, j, cur_prod_core, cur_rhs_stack))
		SXBA_1_bit (ap_sets [A], k); /* Aij est (partiellement) calcule. */
	}
	else
	    tbp_stack [x] = 0;
    }
	    
    if (is_semact_fun)
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

		    if ((*for_semact.semact) (k, j, cur_prod_core, cur_rhs_stack))
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
#endif

static SXBA	red_order_set;

#if is_right_recursive

static void
rr_generate (i, h, j, item)
    int i, h, j, item;
{
    /* item = Aij -> \alphaih Bhj . \betajj */
    /* Bhj \betajj sont evalues */
    int 	X, prod, A, A0j, Aij, cur_item, prod_lgth, k, B, B0j, Bkj, prdct_no;
    BOOLEAN	is_local_prdct;

#if is_epsilon
    if (h == j && lispro [item-2] > 0)
    {
	PUSH (tbp_item_stack, item-1);
	return;
    }
#endif

    /* On traite le suffixe */
    prod = prolis [item];
    A = lhs [prod];

#if EBUG
    A0j = MAKE_A0j (A, j);
    Aij = MAKE_Aij (A0j, i);

    if (SXBA_bit_is_set (Aij_rhs_set, Aij))
	sxtrap (ME, "rr_generate");

    SXBA_1_bit (Aij_lhs_set, Aij);
#endif
		
    prod_core [0] = prod;

    if (is_semact_fun)
    {
	A0j = MAKE_A0j (A, j);
	Aij = MAKE_Aij (A0j, i);
	rhs_stack [0] = Aij;
    }

    cur_item = prolon [prod+1]-1;
    prod_lgth = cur_item - prolon [prod];

    while (item <= cur_item)
    {
	k = item == cur_item ? h : j;
	cur_item--;
	B = lispro [cur_item];

	is_local_prdct =
#if is_prdct
	    is_prdct_fun && (prdct_no = prdct [cur_item]) != -1
#else
		FALSE
#endif
		    ;
	
	if (is_semact_fun || is_local_prdct)
	{
	    B0j = MAKE_A0j (B, j);
	    Bkj = MAKE_Aij (B0j, k);

	    if (is_semact_fun)
		rhs_stack [prod_lgth] = Bkj;
	}

	

	if (is_local_prdct &&
	    !(*for_semact.prdct) (2 /* No de passe : analyseur */,
				  cur_item, k, j, prdct_no))
	    return;
#if EBUG
	B0j = MAKE_A0j (B, j);
	Bkj = MAKE_Aij (B0j, k);

	if (!SXBA_bit_is_set (Aij_lhs_set, Bkj))
	    sxtrap (ME, "rr_generate");

	SXBA_1_bit (Aij_rhs_set, Bkj);
#endif

	if (--prod_lgth)
	    prod_core [prod_lgth] = k;
    }

    /* On traite le prefixe */

    if (prod_lgth == 0)
    {
	if (is_print_prod)
	    print_prod (i, j, prod_core);
	else
	    xprod++;

	if (!is_semact_fun || (*for_semact.semact) (i, j, prod_core, rhs_stack))
	    SXBA_1_bit (ap_sets [A], i); /* Aij est (partiellement) calcule. */
	/* Aij peut se calculer autrement (autre production ou meme
	   production mais chemin (autre k) different). */
    }
    else
    {
	parse_item (A0j, item-2, NULL, i, h, j, prod_lgth);
    }
}



static void
right_recursive (j, init_prod_order)
     int j, init_prod_order;
{
    /* un ensemble de productions mutuellement recursives droites vient d'etre detecte'
       Les productions de la foret partagee sont triees dynamiquement de facon telle
       qu'elles ne sont executees que quand la semantique de la rhs est completement
       connue. */
    int 		prod_order, min_prod_order, prod, A, right_item, i, item, B, k, nbt;
    int			*i_prod_stack;
    struct PT2_item	*PT2j;
    SXBA 		index_set, backward_index_set;
    BOOLEAN 		nothing_has_changed, is_ok;

    prod_order = init_prod_order;
    min_prod_order = prod_order2min [prod_order];

    PT2j = &(PT2 [j]);

    i_index_set [0] = j+1;

    do {
	/* on parcourt le cycle */
	prod = order2prod [prod_order];
	A = lhs [prod];
	right_item = prolon [prod+1]-1;
	index_set = PT2j->index_sets [right_item];

	i = -1;

	while ((i = sxba_scan (index_set, i)) >= 0)
	    PUSH (i2prod_stack [i], prod);

	OR (i_index_set, index_set);
    } while ((prod_order = sxba_1_rlscan (red_order_set, prod_order)) >= min_prod_order);

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
			rr_generate (i, i, j, item);
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
				rr_generate (i, k, j, item);
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
				    rr_generate (i, i, j, item);
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
		    sxtrap (ME, "right_recursive");
#endif

#if is_cyclic
		process_cycle (i, j);
#endif
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

#endif


static void
parse ()
{
    struct PT2_item	*PT2j;
    int                 *PT2j_backward_shift_stack;
    SXBA	        backward_index_set, index_set;
    int		        item, new_item, new_j, j, prod_order, A, A0j, Aij, B, prod, prod_lgth, x;
    BOOLEAN             is_ok;
    SXBA		lhs_nt_set;
#if is_right_recursive
    int			mino;
#endif

    /* On genere toutes les reductions finissant en j */
  
    lhs_nt_set = sxba_calloc (ntmax+1);

#if is_cyclic
    cycle_red_order_set = sxba_calloc (prodmax+1);
#endif

    for (j = 1 /* pas de reduce en 0! */; j <= /*n+1*/n; j++)
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

	    prod_lgth = prolon [prod+1]-prolon [prod]-1;
#if 0
	    nbt = item2nbt [item];
	    nbnt = prod2nbnt [prod];
#endif
#if EBUG
	    A0j = MAKE_A0j (A, j);
#else
	    if (is_semact_fun)
		A0j = MAKE_A0j (A, j);
#endif
	    
#if is_epsilon
	    if (prod_lgth == 0)
	    {
#if 0
		/* RHS \in T* */
		new_j = j - nbt;
#endif
		/* A -> \epsilon */
		new_j = j;

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

		if (is_semact_fun)
		{
		    rhs_stack [0] = MAKE_Aij (A0j, new_j);
		    is_ok = (*for_semact.semact) (new_j, j, prod_core, rhs_stack);
		}
		else
		    is_ok = TRUE;

		if (is_ok)
		    SXBA_1_bit (ap_sets [A], new_j);
	    }
	    else
#endif
	    {
		if ((B = lispro [item-1]) > 0)
		{
		    /* item = A -> \alpha B . */
#if is_right_recursive
		    if ((mino = prod_order2min [prod_order]) != 0) {
			right_recursive (j, prod_order);
			prod_order = mino; /* Sortie "basse" du cycle */
		    }
		    else
#endif
		    {
			if (AND (PT2j->backward_index_sets [item], ap_sets [B]))
			{
			    parse_item (A0j, item-1, PT2j->index_sets [item], -1, j, j, prod_lgth);
			}
		    }
		}
		else
		{
		    /* item = A -> \alpha t . */
		    parse_item (A0j, item-1, PT2j->index_sets [item], -1, j, j, prod_lgth);
#if 0
		    /* new_item = A -> \alpha B . t1 ... tnbt */
		    new_item = item - nbt;
		    new_j = j - nbt;
		    index_set = PT2 [new_j].index_sets [new_item];
		    parse_item (A0j, new_item - 1, index_set, -1, new_j, j, nbnt);
		    /* ap_sets [A] est positionne' */
#endif
		}
	    }
	}

	/* Il faut repercuter les ap_sets sur les backwards des shifts. */
	PT2j_backward_shift_stack = &(PT2j->backward_shift_stack [0]);

	for (x = PT2j_backward_shift_stack [0]; x > 0; x--)
	{
	    item = PT2j_backward_shift_stack [x];
	    /* item = B -> \alpha A . X \beta */

	    if ((A = lispro [item-1]) > 0)
		AND (PT2j->backward_index_sets [item], ap_sets [A]);
	}

	A = 0;

	while ((A = sxba_scan_reset (lhs_nt_set, A)) > 0)
	    sxba_empty (ap_sets [A]);
	
    }

#if is_cyclic
    sxfree (cycle_red_order_set);
#endif

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
    spf_count = (double*) sxcalloc (pAij_top+1, sizeof (double));
}


static void
spf_count_final (size)
    int size;
{
    sxfree (spf_count);
}


static BOOLEAN
spf_count_semact (i, j, prod_core, rhs_stack)
    int 	i, j;
    ushort	*prod_core;
    int		rhs_stack [];
{
#define __NO_MATH_INLINES
#include <math.h>

    int		pAij;
    double	val;
    int 	prod, nbnt;

    prod = prod_core [0];
    pAij = unpack2pack (rhs_stack [0]);

    if (prod & HIBITS)
    {
	/* implique' ds un cycle et 2eme passage */
	spf_count [pAij] = HUGE_VAL; /* Produces IEEE Infinity. */
	return FALSE;
    }
    else
    {
	nbnt = prod2nbnt [prod]; 

	if (nbnt > 0)
	{
	    val = spf_count [unpack2pack (rhs_stack [nbnt])];

	    while (--nbnt > 0)
		val *= spf_count [unpack2pack (rhs_stack [nbnt])];
	}
	else
	    val = 1.0;

	spf_count [pAij] += val;

	return TRUE;
    }
}


static BOOLEAN
spf_count_sem_pass (S0n)
    int	S0n;
{
    double val = spf_count [unpack2pack (S0n)];

    if (val < 1.0E9)
	printf ("Nombre d'arbres de la foret partagee : %.f\n", val);
    else
	printf ("Nombre d'arbres de la foret partagee : %e\n", val);

    return TRUE;
}

static void
free_A2A0j ()
{
    if (A2A0j [1] != NULL) {
	sxfree (A2A0j [1]), A2A0j [1] = NULL;

	if (Aij2A_i_j != NULL)
	    sxfree (Aij2A_i_j), Aij2A_i_j = NULL;

	if (Aij2pAij != NULL) {
	    sxfree (Aij2pAij), Aij2pAij = NULL;
	    sxfree (pAij2Aij);
	}
    }
}

static void
set_A2A0j ()
{
    int		*p, A, i, j, h;
    ushort	*q;

    p = (int*) sxalloc (ntmax * (/*n+1*/n+1), sizeof (int));

    A0j2Aij_size = ntmax*((/*n+1*/n+1)+(/*n+1*/n)*(/*n+1*/n+1)/2);

    if (for_semact.need_Aij2A_i_j) {
	A0j2Aij_size += 2 * ntmax * (/*n+1*/n+1); /* place pour A et j */
	q = Aij2A_i_j = (ushort*) sxalloc (A0j2Aij_size, sizeof (ushort));
    }

    if (for_semact.need_pack_unpack) {
	Aij2pAij = (int*) sxalloc (A0j2Aij_size, sizeof (int));
	pAij2Aij = (int*) sxalloc (A0j2Aij_size, sizeof (int));
    }

    h = 0;

    for (A = 1; A <= ntmax; A++)
    {
	A2A0j [A] = p;

	for (j = 0; j <= /*n+1*/n; j++, p++)
	{
	    if (for_semact.need_Aij2A_i_j) {
		h += 2;
		*q++ = A;
		*q++ = j;
		q += j+1;
	    }

	    *p = h;
#if 0
	    /* Seuls les "i" utiles seront entres ds RL_mreduction */
	    if (for_semact.need_Aij2A_i_j) {
		for (i = 0; i <= j; i++) {
		    *q++ = i;
		}
	    }
#endif
	    h += j+1;
	}
    }

#if EBUG
    if (h != A0j2Aij_size)
	sxtrap (ME, "set_A2A0j");
#endif

}
#endif



static BOOLEAN
sxparse_it ()
{
    int 	t, nt, size, S0n;
    BOOLEAN	ret_val;

    /* delta_n est un #define donne a la compilation qui donne le nombre maximal
       d'accroissement possible (par correction d'erreurs) de la chaine source. */

    bag_alloc (&shift_bag, "shift_bag", (/*n+1*/n+delta_n + 1) * prodmax * NBLONGS (/*n+1*/n+delta_n + 1));

    RT = (struct recognize_item*) sxcalloc (/*n+1*/n+delta_n+1, sizeof (struct recognize_item));
    item_is_used = sxbm_calloc (/*n+1*/n+delta_n+1, itemmax+1);
    RT_left_corner = sxbm_calloc (n+delta_n+1, ntmax+1);

#if is_rcvr
    non_kernel_item_sets = sxbm_calloc (/*n+1*/n+delta_n+1, itemmax+1);
#else
    T2_non_kernel_item_set = non_kernel_item_set;
#endif

    ntXindex_set = sxbm_calloc (ntmax+1, /*n+1*/n+delta_n+1);

#if is_parser
    PT = (struct parse_item*) sxcalloc (/*n+1*/n+delta_n+1, sizeof (struct parse_item));

    if (is_no_semantics)
    {
	for_semact.sem_init = NULL;
	for_semact.sem_final = NULL;
	for_semact.semact = NULL;
	for_semact.parsact = NULL;
	for_semact.prdct = NULL;
	for_semact.constraint = NULL;
	for_semact.sem_pass = NULL;
	for_semact.need_Aij2A_i_j = FALSE;
	for_semact.need_pack_unpack = FALSE;
    }
    else
    {
	if (is_parse_tree_number)
	{
	    for_semact.sem_init = spf_count_init;
	    for_semact.sem_final = spf_count_final;
	    for_semact.semact = spf_count_semact;
	    for_semact.parsact = NULL;
	    for_semact.prdct = NULL;
	    for_semact.constraint = NULL;
	    for_semact.sem_pass = spf_count_sem_pass;
	    for_semact.need_Aij2A_i_j = FALSE;
	    for_semact.need_pack_unpack = TRUE;
	}
    }

    /* else is_default_semantics, on execute la semantique specifiee par la grammaire. */

#if 0
    if (max_tree_nb == 1)
    {
	for_semact.sem_init = ARN_sem_init;
	for_semact.sem_final = ARN_sem_final;
	for_semact.semact = ARN_semact;
	for_semact.parsact = NULL;
	for_semact.prdct = NULL;
	for_semact.constraint = ARN_constraint;
	for_semact.sem_pass = NULL;
    }
    else
    {
	for_semact.sem_init = ARN_ksem_init;
	for_semact.sem_final = ARN_ksem_final;
	for_semact.semact = ARN_ksemact;
	for_semact.parsact = NULL;
	for_semact.prdct = NULL;
	for_semact.constraint = ARN_constraint;
	for_semact.sem_pass = NULL;
    }
#endif
#endif

    is_semact_fun = for_semact.semact != NULL;
    is_constraint_fun = for_semact.constraint != NULL;
    is_prdct_fun = for_semact.prdct != NULL;

    ret_val = recognize ();

    if (is_print_time)
	sxtime (TIME_FINAL, "\tRecognizer");


#if is_parser
    if (sxerrmngr.nbmess [2] == 0) {
	PT2 = (struct PT2_item*) sxcalloc (/*n+1*/n+1, sizeof (struct PT2_item));
	level0_index_set = sxba_calloc (/*n+1*/n+1);
	bag_alloc (&pt2_bag, "pt2_bag", (/*n+1*/n + 1) * prodmax * NBLONGS (/*n+1*/n + 1));
	ap_sets = sxbm_calloc (ntmax+1, /*n+1*/n+1);
	red_order_sets = sxbm_calloc (/*n+1*/n+1, prodmax+1);

	/* ATTENTION CHANGEMENT :
	   Un symbole Aij ou i et j sont deux numeros de table signifie que :
	   Aij =>* ai ai+1 ... aj-1 */

#if EBUG
	set_A2A0j ();
	Aij_rhs_set = sxba_calloc (A0j2Aij_size);
	Aij_lhs_set = sxba_calloc (A0j2Aij_size);
#else
	if (is_semact_fun)
	    set_A2A0j ();
#endif

	RL_mreduction ();

	if (is_print_time)
	    sxtime (TIME_FINAL, "\tReducer");
    }
#endif

    bag_free (&shift_bag);
    sxbm_free (ntXindex_set);
    sxfree (RT);
    sxbm_free (item_is_used);
    sxbm_free (RT_left_corner);
#if is_rcvr
    sxbm_free (non_kernel_item_sets);
#endif

#if is_parser
    if (sxerrmngr.nbmess [2] == 0) {
	if (is_semact_fun)
	{
	    if (max_tree_nb > 1)
	    {
		log_max_tree_nb = sxlast_bit (max_tree_nb);
		filtre_max_tree_nb = ~((~(0L))<<log_max_tree_nb);
	    }

	    if (for_semact.sem_init != NULL)
		(*for_semact.sem_init) (A0j2Aij_size);
	}

#if is_right_recursive
{
    int	*p, i;

    i_index_set = sxba_calloc (/*n+1*/n+1);
    i2nt_nyp = sxba_calloc (ntmax+1);
    i2prod_stack = (int**) sxalloc (/*n+1*/n+1, sizeof (int*));
    p = gprod_stacks = (int*) sxalloc ((/*n+1*/n+1)*(prodmax+1), sizeof (int));

    for (i = 0; i <= /*n+1*/n; i++)
    {
	i2prod_stack [i] = p;
	*p = 0;
	p += prodmax+1;
    }

#if is_cyclic
    item_cycle_set = sxba_calloc (itemmax+1);
    cyclic_citem_set = sxba_calloc (itemmax+1);
#endif
}
#endif

	parse ();

#if EBUG
	sxfree (Aij_rhs_set);
	sxfree (Aij_lhs_set);
#endif

#if is_right_recursive
	sxfree (i_index_set);
	sxfree (i2nt_nyp);
	sxfree (i2prod_stack);
	sxfree (gprod_stacks);

#if is_cyclic
	sxfree (cyclic_citem_set);
	sxfree (item_cycle_set);
#endif

#endif
    
	if (is_print_time) {
	    printf ("Nombre de productions de la foret partagee : %i\n", xprod);
	    sxtime (TIME_FINAL, "\tParser");
	}

	if (for_semact.sem_pass != NULL) {
	    S0n = MAKE_Aij (MAKE_A0j (1 /* start symbol */, /*n+1*/n), 1);
	    (*for_semact.sem_pass) (S0n);
	}

#if 0
	if (!is_no_semantics)
	{
	    S0n = MAKE_Aij (MAKE_A0j (1 /* start symbol */, /*n+1*/n), 1);

	    if (is_parse_tree_number)
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

		    if (is_print_time)
			sxtime (TIME_FINAL, "\tShared Parse Forest");
		}
		else
		    fputs ("La foret partagee est vide\n", stdout);
	    }

	    if (is_semact_fun)
	    {
		free_A2A0j ();

		if (for_semact.sem_final != NULL)
		    (*for_semact.sem_final) (A0j2Aij_size);
	    }
	}
#endif

	if (for_semact.sem_final != NULL)
	    (*for_semact.sem_final) (A0j2Aij_size);
	

#if EBUG
	free_A2A0j ();
#endif

	sxfree (level0_index_set);
	sxfree (PT2);
	bag_free (&pt2_bag);
	sxbm_free (ap_sets);
	sxbm_free (red_order_sets);
    }
#endif

#if EBUG
    size = sizeof (struct recognize_item) * (/*n+1*/n+1) /* RT */
	+ sizeof (SXBA_ELT) * NBLONGS (itemmax+1) /* init_state_set */
        + sizeof (SXBA_ELT) * NBLONGS (ntmax+1) * (/*n+1*/n+1) /* ntXindex_set */ ;
    printf ("Size of recognize structures = %i bytes\n", size);
#endif

#if is_parser
    if (sxerrmngr.nbmess [2] == 0) {
	sxfree (PT);

#if EBUG
	size = sizeof (struct parse_item) * (/*n+1*/n+1) /* PT */
	    ;

	if (is_semact_fun)
	{
	    size += sizeof (int) * ntmax * (/*n+1*/n+1); /* A2A0j */

	    if (is_parse_tree_number)
		size += sizeof (double) * A0j2Aij_size;
	}

	printf ("Size of parse structures = %i bytes\n", size);
#endif
    }
#endif

    if (is_print_time)
	sxtime (TIME_FINAL, "\tDone");

    return ret_val;
}



void
fill_inputG ()
{
    /* ntmax ... sont des define! */
    inputG.maxnt = ntmax;
    inputG.maxt = tmax;
    inputG.maxitem = itemmax;
    inputG.maxprod = prodmax;

    inputG.lispro = lispro;
    inputG.prolis = prolis;
    inputG.prolon = prolon;
    inputG.lhs = lhs;
    inputG.semact = semact;
#if is_prdct
    inputG.prdct = prdct;
#endif
#if is_parser
    inputG.prod2nbnt = prod2nbnt;
#endif
}


BOOLEAN
sxearley_parser (what_to_do, arg)
    int			what_to_do;
    struct sxtables	*arg;
{
    switch (what_to_do) {
    case BEGIN:
	return TRUE;

    case OPEN:

	sxplocals.sxtables = arg;
	sxplocals.SXP_tables = arg->SXP_tables;
	sxtkn_mngr (OPEN, 2);
#if is_rcvr
	(*sxplocals.SXP_tables.recovery) (OPEN);
#endif
	return TRUE;

    case INIT:
#if 0
    {
	struct sxtoken tok;

	sxtkn_mngr (INIT, 0);

	/* terminal_token EOF */
	tok.lahead = tmax;
	tok.string_table_entry = EMPTY_STE;
	tok.source_index = sxsrcmngr.source_coord;
	tok.comment = NULL;
	/* Le token 0 est "EOF" */
	SXGET_TOKEN (0) = tok;
    }
#endif
    
	/* analyse normale */
	/* valeurs par defaut qui peuvent etre changees ds les scan_act ou pars_act. */
	sxplocals.mode.look_back = 0; /* All tokens are kept. */
	sxplocals.mode.mode = SXPARSER;
	sxplocals.mode.kind = SXWITH_RECOVERY | SXWITH_CORRECTION;
	sxplocals.mode.local_errors_nb = 0;
	sxplocals.mode.global_errors_nb = 0;
	sxplocals.mode.is_prefixe = FALSE;
	sxplocals.mode.is_silent = FALSE;
	sxplocals.mode.with_semact = TRUE;
	sxplocals.mode.with_parsact = TRUE;
	sxplocals.mode.with_parsprdct = TRUE;
	sxplocals.mode.with_do_undo = FALSE;
	return TRUE;

    case ACTION:
    {
	int	v;
	BOOLEAN	ret_val;

	if (is_print_time)
	    sxtime (TIME_INIT);

#if 0
	do {
	    (*(sxplocals.SXP_tables.scanit)) ();
	} while (SXGET_TOKEN (sxplocals.Mtok_no).lahead != -tmax);

	n = sxplocals.Mtok_no - 1;
#endif

	XxY_alloc (&FSA_pXq_hd, "FSA_pXq_hd", 10, 1, 1, 0, NULL,
#if EBUG
		   stdout
#else
		   NULL
#endif
		   );
	XxY_alloc (&FSA_pXt_hd, "FSA_pXt_hd", 10, 1, 1, 0, NULL,
#if EBUG
		   stdout
#else
		   NULL
#endif
		   );
	XxY_alloc (&FSA_pqXt_hd, "FSA_pqXt_hd", 10, 1, 1, 0, NULL,
#if EBUG
		   stdout
#else
		   NULL
#endif
		   );

#define A	1
#define C	2
#define G	3
#define U	4
#define DOL	(-tmax)

	/* 0 ->DOL 1
	   1 ->A 2
	   1 ->C 4
	   2 ->any 3
	   3 ->U 4
	   4 ->DOL 5
	   */
	n = FSA_final_state = 4;

	XxY_set (&FSA_pXq_hd, 0, 1, &v);
	XxY_set (&FSA_pqXt_hd, v, DOL, &v);
	XxY_set (&FSA_pXt_hd, 0, DOL, &v);

	XxY_set (&FSA_pXq_hd, 1, 2, &v);
	XxY_set (&FSA_pqXt_hd, v, A, &v);
	XxY_set (&FSA_pXt_hd, 1, A, &v);

	XxY_set (&FSA_pXq_hd, 1, 4, &v);
	XxY_set (&FSA_pqXt_hd, v, C, &v);
	XxY_set (&FSA_pXt_hd, 1, C, &v);
	
	XxY_set (&FSA_pXq_hd, 2, 3, &v);
	XxY_set (&FSA_pqXt_hd, v, FSA_any, &v);
	XxY_set (&FSA_pXt_hd, 2, FSA_any, &v);
	
	XxY_set (&FSA_pXq_hd, 3, 4, &v);
	XxY_set (&FSA_pqXt_hd, v, U, &v);
	XxY_set (&FSA_pXt_hd, 3, U, &v);
	   
	XxY_set (&FSA_pXq_hd, 4, 5, &v);
	XxY_set (&FSA_pqXt_hd, v, DOL, &v);
	XxY_set (&FSA_pXt_hd, 4, DOL, &v);
	
	FSA_states_tbp = sxba_calloc (FSA_final_state+1);

	if (is_print_time)
	    sxtime (TIME_FINAL, "\tScanner");

	ret_val = sxparse_it ();

	XxY_free (&FSA_pXq_hd);
	XxY_free (&FSA_pXt_hd);
	XxY_free (&FSA_pqXt_hd);

	sxfree (FSA_states_tbp);

	return ret_val;
    }

    case FINAL:
	sxtkn_mngr (FINAL, 0);
	return TRUE;

    case CLOSE:
	/* end of language: free the local arrays */
	sxtkn_mngr (CLOSE, 0);
#if is_rcvr
	(*sxplocals.SXP_tables.recovery) (CLOSE);
#endif

	return TRUE;

    case END:
	/* free everything */
	return TRUE;

    default:
	fprintf (sxstderr, "The function \"sxearley_parser\" is out of date with respect to its specification.\n");
	abort ();
    }
}

#if is_rcvr
#include "sxp_rcvr_earley_lc.h"
#endif

void
RNA_actions (){}
    
