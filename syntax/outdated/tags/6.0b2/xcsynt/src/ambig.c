/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1984 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 05-08-94 10:15 (pb):		mauvaise utilisation de XT_TO_T_CODE	*/
/************************************************************************/
/* 15-10-92 16:45 (pb):		Bug ds grow_items2 (ajout de from)	*/
/************************************************************************/
/* 06-07-89 11:45 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

static char	ME [] = "ambig";

#include "csynt_optim.h"
char WHAT_XCSYNTAMBIG[] = "@(#)SYNTAX - $Id: ambig.c 632 2007-06-18 11:33:14Z rlacroix $" WHAT_DEBUG;

#include "RLR.h"
#include "bstr.h"
#include "SS.h"

#define max_rmargin 100
#define min_lmargin 16
static struct bstr	*pre;

extern VARSTR	cat_t_string ();

static int	*StNt_stack, *read_empty;
static SXBA	StNt_set, StNt_set2;
static SXBA	ambig_orig_set, proto_ambig_StNt_set, ambig_StNt_set;
static int	*state_path1, *state_path2, *items1, *items2;
static VARSTR	post1;
static int	next_state, init_t;

static XxY_header	reads_hd;
static SXBA		cycle_set;
static int		cycle_pivot;

static SXBA		NT_set, S_set;
static int		*S_stack, *NT_stack;
static VARSTR		cycle_vstr;
static BOOLEAN		(*includes_F) ();
static SXBA		init_t_set, ambig_orig_sub_set, to_be_processed_t_set;

static SXBA	not_nullable_set, not_nullable_already_checked_set;



static char	*left, *right, *ambig_eof;
static char	empty_string [] = "";
static char	der_star [] = "=>* ";
static char	der_plus [] = "=>+ ";
static char	equal [] = "==  ";
static char	der [] = "=>  ";
static char	dots [] = " ... ";
static char	equals [] = " === ";
static BOOLEAN	grow_items1 ();



static VOID	oflw_StNt_reads_cycle (old_size, new_size)
    int		old_size, new_size;
{
    StNt_to_reads_cycle_nb = (int*) sxrealloc (StNt_to_reads_cycle_nb, new_size +
	 1, sizeof (int));
}


static VOID	nt_trans_to_spath (nt_trans, item, spath)
    register int	*spath;
    int		nt_trans, item;
{
    /* spath contient une sequence d'etats (chemin) de l'automate LR(0) dont
       l'arrivee est a l'indice spath[0]
       nt_trans=(s1, nt1)
       si item < 0 (item origine)
           -item: nt1 -> X1 X2 ... Xi . 
       si item > 0
           item: nt1 -> X1 X2 ... Xi nt2 . alpha et
       s2 = goto (s1, X1), s3 = goto (s2, X2) ... et si+1 = goto (si, Xi)
       Si X1 X2 ... Xi est vide, nt_trans_to_spath ne fait rien (un seul etat pour
       les items de la fermeture)
       Sinon on empile en pas a pas le cheminsi, si-1, ..., s1 */

    register int	start, xspath, s1, nt_trans_to_spath_delta;

    item = item < 0 ? -item : item - 1;

    if ((nt_trans_to_spath_delta = item - (start = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].prolon)) > 0) {
	xspath = spath [0] += nt_trans_to_spath_delta;
	s1 = XxY_X (Q0xV_hd, nt_trans);

	while (start < item) {
	    spath [xspath--] = s1;
	    s1 = gotostar (s1, start, start + 1);
	    start++;
	}
    }
}



static int	nt_trans_to_item_no_i (nt_trans2, nt_trans1, i)
    int		nt_trans2, nt_trans1, i;
{
    /* On a nt_trans2 includes nt_trans1 */
    /* Calcule le i eme item de nt_trans2 compatible avec le couple precedent */
    register int	next, n, no, item;
    int			nt1, rule_no, state1, state2;

    state1 = XxY_X (Q0xV_hd, nt_trans1);
    nt1 = XxY_Y (Q0xV_hd, nt_trans1);
    nt1 = XNT_TO_NT_CODE (nt1);
    state2 = XxY_X (Q0xV_hd, nt_trans2);
    next = Q0xV_to_Q0 [nt_trans2];
    no = n = 0;

/* Il existe dans state2 au moins un item de la forme : nt1 -> Y1 Y2 ... Yn .nt2 beta
   avec beta =>* vide et goto (state1, Y1 Y2 ... Yn) = state2, on retourne le i eme */

    while ((item = nucl_i (next, ++n)) > 0) {
	if (bnf_ag.WS_NBPRO [rule_no = bnf_ag.WS_INDPRO [item].prolis].reduc == nt1 &&
	    SXBA_bit_is_set (bnf_ag.NULLABLE, item + 1) &&
	    gotostar (state1, bnf_ag.WS_NBPRO [rule_no].prolon, item) == state2) {
	    if (++no == i)
		/* C'est le i eme */
		return item + 1;
	}
    }

    return 0;
}



static BOOLEAN	grow_items2 (incl, path, grow_items2_items1, nt1, grow_items2_items2, nt2, item2, grow_items2_nt, top, from)
    register int	*incl, *path, *grow_items2_items1, *grow_items2_items2;
    int		item2, nt1, nt2, grow_items2_nt, top, from;
{
    int x;

    if (nt2 == grow_items2_nt) {
	/* C'est fini pour grow_items2_items2 */
	if (nt1 == grow_items2_nt)
	    /* C'est fini pour grow_items2_items1 */
	    return TRUE;

	return grow_items1 (incl, path, grow_items2_items1, nt1, grow_items2_items2, nt2, item2, grow_items2_nt, top);
    }

    while (TRUE) {
	int	state;

	state = path [top];

	if (bnf_ag.WS_INDPRO [item2 - 1].lispro == 0) {
	    /* item2 : nt2 -> . alpha et nt2 != grow_items2_nt */
	/* On parcourt les items de (state, nt2) */

	    register int	next, n, item;

	    /* Recherche des items de l'etat state ayant le marqueur LR devant nt2 */
	    next = Q0_V_to_next (state, nt2);
	    n = 0;

	    while ((item = nucl_i (next, ++n)) > 0) {
		/* item = B -> delta . nt2 gamma */ 
		for (x = *grow_items2_items2; x >= from; x--) {
		    if (grow_items2_items2 [x] == item + 1)
			break;
		}
		
		if (x < from) {
		    grow_items2_items2 [++*grow_items2_items2] = item + 1;

		    if (grow_items2 (incl, path, grow_items2_items1, nt1, grow_items2_items2,
				     bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc,
				     item, grow_items2_nt, top, from))
			return TRUE;

		    --*grow_items2_items2;
		}
	    }

	    return FALSE;
	}
	else {
	    /* item2 : nt2 -> X1 ... Xn . alpha et nt2 != grow_items2_nt */

	    if (*path == top)
		return grow_items1 (incl, path, grow_items2_items1, nt1, grow_items2_items2, nt2, item2, grow_items2_nt, top);

	    top++;

	    if (XxY_is_set (&Q0xQ0_hd, path [top], state) == 0)
		return FALSE;

	    item2--;
	}
    }
}



static BOOLEAN	grow_items1 (incl, path, grow_items1_items1, nt1, grow_items1_items2, nt2, item2, grow_items1_nt, top)
    register int	*incl, *path, *grow_items1_items1, *grow_items1_items2;
    int		item2, nt1, nt2, grow_items1_nt, top;
{
    register int	nt_trans1, nt_trans2, item;
    int		xit1, top1, old_nt1, no;

    nt_trans1 = incl [(*incl)--];

    if (*incl != 0) {
	/* incl est non vide */
	nt_trans2 = incl [*incl];
	/* On a nt_trans1 includes nt_trans2 */
	/* On calcule tous les items de nt_trans1 qui ont produit nt_trans2 */

	xit1 = *grow_items1_items1;
	top1 = *path;
	old_nt1 = nt1;

	for (item = nt_trans_to_item_no_i (nt_trans1, nt_trans2, no = 1); item != 0; item = nt_trans_to_item_no_i (
	     nt_trans1, nt_trans2, ++no)) {
	    nt_trans_to_spath (nt_trans2, item, path);

	    if (nt1 != grow_items1_nt) {
		grow_items1_items1 [++*grow_items1_items1] = item;
		nt1 = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc;

		if (nt1 == grow_items1_nt && nt2 == grow_items1_nt)
		    return TRUE;
	    }

	    if (nt2 != grow_items1_nt) {
		if (grow_items2 (incl, path, grow_items1_items1, nt1, grow_items1_items2, nt2, item2, grow_items1_nt, top, *grow_items1_items2)) {
		    return TRUE;
		}
	    }
	    else if (nt1 != grow_items1_nt) {
		if (grow_items1 (incl, path, grow_items1_items1, nt1, grow_items1_items2, nt2, item2, grow_items1_nt, top)) {
		    /* Inclusion */
		    return TRUE;
		}
	    }

	    *path = top1;
	    *grow_items1_items1 = xit1;
	    nt1 = old_nt1;
	}
    }


/* Non inclusion, on remet tout en place */

    incl [++*incl] = nt_trans1;
    return FALSE;
}



static VOID	slice_cat (item1, item2)
    int		item1, item2;
{
    /* soit : A->alpha beta gamma
       Cette procedure concatene "beta" a pre en utilisabt bstr_cat_str
       avec item1 :  A->alpha .beta gamma
       et item2 :  A->alpha beta .gamma */
    register int	x, tnt;

    for (x = item1; x < item2; x++) {
	bstr_cat_strsp (pre, (tnt = bnf_ag.WS_INDPRO [x].lispro) > 0 ? get_nt_string (tnt) : xt_to_str (tnt));
    }
}


static VOID	outderiv (xitem, items, t_string, pf)
    int		xitem, *items;
    char	*t_string;
    char	**pf;
{

/* Ecrit la derivation contenue dans items, la chaine t_string est sortie a partir du niveau
   xitem */
    register int	x, outderiv_lim, item;
    int		pre_col, pre_lgth, red_no, init_item;
    BOOLEAN	is_t_string;

    outderiv_lim = items [0];
    is_t_string = xitem < 1;

    for (x = 1; x <= outderiv_lim; x++) {
	item = items [x];

	if (item < 0) {
	    /* -item: A -> X1 ... Xn .  (Reduction en conflit) */
	    item = -item;
	}


/* Soit item : A -> alpha NT . beta
	   Ecrit:
	   Si beta non vide
	      pre || alpha NT beta || is_t_string ? t_string  : NULL || ...
	   Puis:
	      pre || alpha NT || is_t_string ? t_string : NULL || ...
	   et retourne pre = pre || alpha */
    
	/* concatenation de alpha */

	init_item = bnf_ag.WS_NBPRO [red_no = bnf_ag.WS_INDPRO [item].prolis].prolon;

	if (red_no != 0) {
	    put_edit_fnb (11, 4, red_no);
	    put_edit_nnl (16, der);
	}
	else
	    put_edit_nnl (16, der_star);

	if (init_item < item - 1 /* alpha non vide */ )
	    slice_cat (init_item, item - 1);

	bstr_get_attr (pre, &pre_lgth, &pre_col);


/* concatenation de NT beta */

	{
	    int		beta;

	    if ((beta = bnf_ag.WS_INDPRO [item].lispro) != 0 /* beta non vide */  && (x != xitem || beta > 0 || bnf_ag.
		 WS_INDPRO [item + 1].lispro != 0) /* beta != t au niveau xitem */ ) {
		/* Si NT existe, concatenation de NT beta sinon concatenation de beta */
		slice_cat (init_item < item ? item - 1 : item, bnf_ag.WS_NBPRO [red_no + 1].prolon - 1);

		if (is_t_string && t_string != NULL)
		    bstr_cat_str (pre, t_string);

		bstr_cat_str (pre, *pf);
		put_edit_apnnl (bstr_tostr (pre));
		bstr_shrink (pre, pre_lgth, pre_col);
		red_no = -1;
	    }
	}

	is_t_string = x >= xitem;
	
	if (x < xitem && !SXBA_bit_is_set (bnf_ag.NULLABLE, item) && **pf == NUL)
	    *pf -= 4;

/* concatenation de NT */

	if (init_item < item)
	    slice_cat (item - 1, item);

	if (is_t_string && t_string != NULL)
	    bstr_cat_str (pre, t_string);

	bstr_cat_str (pre, *pf);

	if (red_no < 0)
	    put_edit_nnl (16, der_star);

	put_edit_apnnl (bstr_tostr (pre));
	bstr_shrink (pre, pre_lgth, pre_col);
    }
}




static int *inverse (stack)
    int *stack;
{
    register int *top, *bot, trans;

    top = (bot = stack) + *stack + 1;

    while (++bot < --top) {
	trans = *top;
	*top = *bot;
	*bot = trans;
    }

    return stack;
}



static char	*make_t_string (make_t_string_post1, item, make_t_string_init_t)
    VARSTR make_t_string_post1;
    int	item, make_t_string_init_t;
{
    register int	tnt;

    varstr_catenate (cat_t_string (varstr_raz (make_t_string_post1), make_t_string_init_t), " ");

    /* item = A -> X1 ... Xn nt . Y1 ... Yp avec Y1 ... Yp =>* make_t_string_init_t x */
    /* On regarde si x peut etre vide, dans ce cas on doit avoir
       Y1 ... Yi-1 Yi Yi+1 ... Yn avec
       Y1 ... Yi-1 =>* vide et Yi =>* make_t_string_init_t et Yi+1 ... Yp =>* vide
       On se contente de Yi = make_t_string_init_t a la place de Yi =>* make_t_string_init_t */

    while ((tnt = bnf_ag.WS_INDPRO [item++].lispro) != 0) {
	if (tnt <= 0 /* Forcement make_t_string_init_t */  || SXBA_bit_is_set (bnf_ag.FIRST [XNT_TO_NT_CODE (tnt)], -make_t_string_init_t)) {
	    /* C'est le premier Yi tq Yi =>* make_t_string_init_t */
	    if (make_t_string_init_t != tnt || !SXBA_bit_is_set (bnf_ag.NULLABLE, item)) {
		/* On ecrit "make_t_string_init_t ..." */
		varstr_catenate (make_t_string_post1, dots + 1);
	    }

	    /* else On ecrit "make_t_string_init_t" */

	    break;
	}
	/* else Forcement un nt qui derive dans le vide */
    }

    return varstr_tostr (make_t_string_post1);
}




BOOLEAN is_t_trans (item)
    register int	item;
{
    /* Cette fonction retourne vrai ssi FIRST (Y1 Y2 ... Yn) est non vide.
       avec item: A -> alpha . Y1 Y2 ... Yn. */
    register int	tnt;

    while ((tnt = bnf_ag.WS_INDPRO [item++].lispro) != 0) {
	if (tnt < 0 || !SXBA_bit_is_set (bnf_ag.BVIDE, tnt))
	    return TRUE;
    }

    return FALSE;
}



static SXBA first (t_set, item)
    register SXBA	t_set;
    register int	item;
{
  /* item : A -> alpha . X1 --- Xn */
    /* Cette fonction retourne t_set U FIRST1 (X1 ---Xn) */
    register int	tnt;

    while ((tnt = bnf_ag.WS_INDPRO [item++].lispro) != 0) {
	if (tnt < 0) {
	    tnt = XT_TO_T_CODE (-tnt);
	    SXBA_1_bit (t_set, tnt);
	    break;
	}

	sxba_or (t_set, bnf_ag.FIRST [XNT_TO_NT_CODE (tnt)]);
	
	if (!SXBA_bit_is_set (bnf_ag.BVIDE, tnt))
	    break;
    }

    return t_set;
}


BOOLEAN is_t_in_first (t, item)
    register int	t, item;
{
  /* item : A -> alpha . X1 --- Xn */
    /* Cette fonction retourne TRUE ssi
          X1 --- Xn =>* epsilon ou
	  t est dans FIRST1 (X1 ---Xn) */
    register int	tnt;

    while ((tnt = bnf_ag.WS_INDPRO [item].lispro) != 0) {
	if (tnt < 0)
	    return t == tnt;

	if (SXBA_bit_is_set (bnf_ag.FIRST [XNT_TO_NT_CODE (tnt)], -t))
	    return TRUE;
	
	if (!SXBA_bit_is_set (bnf_ag.BVIDE, tnt))
	    return FALSE;

	item++;
    }

    return TRUE;
}



static BOOLEAN	SCC (origine, X1, stack, check_set)
    int		origine, X1, *stack;
    register SXBA	check_set;
{
    /* Non trivial Strongly Connected Component detection */
    /* Rend vrai ssi une composante fortement connexe comprenant origine
       est detectee. La composante est stockee dans stack.
       Sinon rend faux avec stack vide */

    register int	x, X2;

    PUSH (stack, X1);
    SXBA_1_bit (check_set, X1);

    XxY_Xforeach (includes_hd, X1, x) {
	X2 = XxY_Y (includes_hd, x);

	if (!SXBA_bit_is_set (check_set, X2) /* X2 n'est pas encore examine */ ) {
	    if (SCC (origine, X2, stack, check_set))
		/* SCC sur origine detecte */
		return TRUE;
	}
	else if (X2 == origine) {
	    /* detection */
	    return TRUE;
	}
    }

    POP (stack, x);
    return FALSE;
}





static VOID	print_ambiguity (stack1, item, nt_trans)
    int		*stack1, item, nt_trans;
{
    char	*t_string, *pf;
    int		print_ambiguity_nt;


/* Sortie des derivations montrant l'ambiguite */

    state_path1 [0] = 1;
    state_path1 [1] = XxY_X (Q0xV_hd, nt_trans);

    {
	/* Remplit state_path2 avec la pile inverse de stack1 || nt_trans */
	register int	X = 0, i;

	state_path2 [i = 1] = nt_trans;

	while ((X = stack1 [X]) > 0) {
	    state_path2 [++i] = X;
	}

	*state_path2 = i;
    }

    *items1 = 0;
    *items2 = 1;
    items2 [1] = item;
    print_ambiguity_nt = bnf_ag.WS_INDPRO [item - 1].lispro;

    if (grow_items2 (state_path2, state_path1, items1, 0, items2, bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc,
	 item - 1, print_ambiguity_nt, 1, *items2)) {
	inverse (items1);
	inverse (items2);
	t_string = make_t_string (post1, item, init_t);

/* Sortie de la premiere derivation */

	fputs ("\nFirst derivation:\n", listing);
	bstr_raz (pre);
	put_edit (13, get_nt_string (print_ambiguity_nt));
	pf = equals + 5;
	outderiv (*items1 + 1, items1, t_string, &pf);
	outderiv (*items2, items2, t_string, &pf);

/* Sortie de la deuxieme derivation */

	put_edit_nl (2);
	fputs ("Second derivation:\n", listing);
	bstr_raz (pre);
	put_edit (13, get_nt_string (print_ambiguity_nt));
	pf = equals + 5;
	outderiv (*items2, items2, t_string, &pf);
	outderiv (0, items1, t_string, &pf);
	put_edit_nl (1);
    }
    else {
	sxtrap (ME, "print_ambiguity");
    }
}



static BOOLEAN	includes_traversal (X)
    int		X;
{
    /* Cette procedure recursive traverse en profondeur d'abord une fois et une seule chaque
       noeud du graphe induit par la relation includes. */
    /* Sur chaque  nouveau noeud X, appelle la fonction includes_F (X). Si cette
       fonction retourne TRUE, la traversee est interrompue. */
    register int	x, Y;

    XxY_Xforeach (includes_hd, X, x) {
	Y = XxY_Y (includes_hd, x);

	if (!SXBA_bit_is_set (StNt_set, Y) /* Y n'est pas encore examine */ ) {
	    if (includes_F != NULL && (*includes_F) (Y))
		return TRUE;

	    SXBA_1_bit (StNt_set, Y);

	    if (includes_traversal (Y))
		return TRUE;
	}
    }

    return FALSE;
}



static VOID NT_closure (XNT_set, XNT)
    register SXBA	XNT_set;
    register int	XNT;
{
    register int j, k, NT;

    SXBA_1_bit (XNT_set, XNT);
    NT = XNT_TO_NT_CODE (XNT);

    for (k = bnf_ag.WS_NTMAX [NT + 1].npg, j = bnf_ag.WS_NTMAX [NT].npg; j < k; j++) {
	if ((XNT = bnf_ag.WS_INDPRO [bnf_ag.WS_NBPRO [bnf_ag.WS_NBPRO [j].numpg].prolon].lispro) > 0) {
	    if (!SXBA_bit_is_set (XNT_set, XNT)) {
		NT_closure (XNT_set, XNT);
	    }
	}
    }
}


static BOOLEAN check_item (item, check_item_S_stack, check_item_NT_stack)
    int 		item;
    register int	*check_item_S_stack;
    int			*check_item_NT_stack;
{
    /* item : A -> alpha X . beta est dans NOYAU (TOP (check_item_S_stack)) */

    /* check_item_S_stack  = s1 s2 ... sp (sommet a gauche)
       check_item_NT_stack = X1 X2 ... Xp (Xi est un NT)

          X1    X2        Xp
       s1 -> s2 -> ... sp ->
       
       Verifie si beta =>* check_item_NT_stack ...
    */

    register int	next, n;
    int			Y, NT_top, S_top;

    if (SS_is_empty (check_item_NT_stack))
	/* check_item_NT_stack == vide */
	return TRUE;

    if ((Y = bnf_ag.WS_INDPRO [item].lispro) <= 0)
	/* beta == epsilon ou beta = t beta' */
	return FALSE;
    
    /* beta == Y beta' */
    NT_top = SS_pop (check_item_NT_stack);
    POP (check_item_S_stack, S_top);
    sxba_empty (NT_set);
    NT_closure (NT_set, Y);

    if (SXBA_bit_is_set (NT_set, NT_top)) {
	next = Q0_V_to_next (S_top, NT_top);
	n = 0;

	while ((item = nucl_i (next, ++n)) > 0) {
	    if (check_item (item + 1, check_item_S_stack, check_item_NT_stack))
		return TRUE;
	}
    }

    SS_push (check_item_NT_stack, NT_top);
    PUSH (check_item_S_stack, S_top);
    return FALSE;
}


static VOID print_not_LRpi (cycle_stack, kind)
    register int	*cycle_stack;
    int			kind;
    /* Un cycle de la relation reads est stocke dans cycle_stack. */
{
    /* On cherche un etat s tel que s n'appartient pas au cycle et
       goto (s, Xn) = s1, s1 dans le cycle. */
    register int	x, y, z, n;
    int 		s, s1, stop, item, A, Xi;
    BOOLEAN		is_cycle_equal_Xi;

    sxba_empty (S_set);
    x = 0;

    while ((x = cycle_stack [x]) > 0) {
	y = XxY_X (Q0xV_hd, x);
	SXBA_1_bit (S_set, y);
    }

    sxinitialise (s1);
    x = 0;

    while ((x = cycle_stack [x]) > 0) {
	s1 = XxY_X (Q0xV_hd, x);

	XxY_Yforeach (Q0xQ0_hd, s1, y) {
	    s = XxY_X (Q0xQ0_hd, y);

	    if (!SXBA_bit_is_set (S_set, s))
		break;
	}

	if (y != 0)
	    break;
    }

    /*
       On recherche un non terminal A tel que dans le noyau de s1 un item de la
       forme (il doit exister)
          A -> Xi ... Xn-1 Xn . beta
       avec beta non vide t.q.
          beta =>* X1 ... Xi-1 A ...
    */

    /* Xn = XNT_TO_NT_CODE (bnf_ag.WS_INDPRO [nucl [S1 [s1].etat] - 1].lispro); */

    stop = x;

    if ((item = nucl_i (s1, 2)) == 0) {
	/* item unique dans le noyau, c'est forcement le bon */
	A = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item = nucl_i (s1, 1) + 1].prolis].reduc;
    }
    else {
        sxinitialise (A);
	z = 0;

	do {
	    item++;
	    
	    if (z != 2 /* traite par z == 0 */ && bnf_ag.WS_INDPRO [item].lispro != 0) {
		n = item - bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].prolon /* |Xi ... Xn-1 Xn| */;
		y = stop;

		while (n-- > 0) {
		    if ((y = cycle_stack [y]) <= 0)
			y = cycle_stack [0];
		}

		CLEAR (S_stack);
		SS_clear (NT_stack);
		x = XxY_X (Q0xV_hd, y);
		PUSH (S_stack, x);
		A = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc;
		SS_push (NT_stack, A);

		while (y != stop) {
		    if ((y = cycle_stack [y]) <= 0)
			y = cycle_stack [0];

		    x = XxY_X (Q0xV_hd, y);
		    PUSH (S_stack, x);
		    x = XxY_Y (Q0xV_hd, y);
		    SS_push (NT_stack, x);
		}

		if (check_item (item, S_stack, NT_stack))
		    break;
	    }
	} while ((item = nucl_i (s1, ++z)) > 0);
    }

    varstr_raz (cycle_vstr);
    is_cycle_equal_Xi = TRUE;
    sxinitialise (Xi);

    for (x = n = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].prolon; x < item;x++) {
	if (x > n) {
	    is_cycle_equal_Xi = FALSE;
	    varstr_catenate (cycle_vstr, " ");
	}
	else
	    Xi = bnf_ag.WS_INDPRO [x].lispro;
	
	varstr_catenate (cycle_vstr, get_nt_string (bnf_ag.WS_INDPRO [x].lispro));
    }

    n = item - n;
    y = stop;

    while (n-- > 0) {
	if ((y = cycle_stack [y]) <= 0)
	    y = cycle_stack [0];
    }

    SS_clear (NT_stack);

    while (y != stop) {
	y = cycle_stack [y];

	if (y <= 0)
	    y = cycle_stack [0];

	x = XxY_Y (Q0xV_hd, y);
	SS_push (NT_stack, x);
    }

    while (!SS_is_empty (NT_stack)) {
	is_cycle_equal_Xi = FALSE;
	z = SS_pop (NT_stack);
	varstr_catenate (cycle_vstr, " ");
	varstr_catenate (cycle_vstr, get_nt_string (z));
    }

    ambig_eof = xt_to_str (bnf_ag.WS_TBL_SIZE.tmax) /* "END OF FILE" */;

    if (A == 1 /* axiome */) {
	left = empty_string;
	right = ambig_eof;
    }
    else
	left = right = dots + 1;

    put_edit_nl (2);
    put_edit (1, kind == AMBIGUOUS_ ? "AMBIGUOUS" : "NOT LR (pi)");
    fputs ("Let \"###\" and \"===\" be terminal strings (\"===\" is not empty).\nWe have:\n", listing);
    put_edit (1, get_nt_string (A));
    put_edit_nnl (4, der_plus);
    put_edit_apnnl (varstr_tostr (cycle_vstr));
    put_edit_apnnl (" ");
    put_edit_apnnl (get_nt_string (A));
    put_edit_ap (equals);
    put_edit (1, "Where");
    put_edit (1, varstr_tostr (cycle_vstr));
    put_edit_nnl (4, der_plus);
    put_edit_ap ("EmptyString");

    if (kind == AMBIGUOUS_) {
	put_edit (1, "and");
	put_edit (1, varstr_tostr (cycle_vstr));
	put_edit_nnl (4, der_plus);
	put_edit_ap ("NotEmptyString");
	fputs ("\n\nDerivations:\n", listing);
    }
    else
	fputs ("\n\nFirst derivation:\n", listing);

    put_edit_nnl (1, get_nt_string (1));
    put_edit (put_edit_get_col() + 1, ambig_eof);

    if (A != 1) {
	put_edit_nnl (4, der_plus);
	put_edit_apnnl (left);
	put_edit_apnnl (get_nt_string (A));
	put_edit (put_edit_get_col() + 1, right);
    }

    put_edit_nnl (4, der_plus);
    put_edit_apnnl (left);
    put_edit_apnnl ("(");
    put_edit_apnnl (varstr_tostr (cycle_vstr));
    put_edit_apnnl (")k ");
    put_edit_apnnl (get_nt_string (A));
    put_edit_apnnl (" (===)k");
    put_edit (put_edit_get_col() + 1, right);
    put_edit_nnl (4, der_plus);
    put_edit_apnnl (left);
    put_edit_apnnl ("(");
    put_edit_apnnl (varstr_tostr (cycle_vstr));
    put_edit_apnnl (")k ");
    put_edit_apnnl ("### (===)k");
    put_edit (put_edit_get_col() + 1, right);

    if (kind == AMBIGUOUS_) {
	put_edit_nnl (4, der_plus);
	put_edit_apnnl (left);
	put_edit_apnnl ("(NotEmptyString | EmptyString)k ### (===)k");
	put_edit (put_edit_get_col() + 1, right);
	put_edit_nnl (4, equal);
	put_edit_apnnl (left);
	put_edit_apnnl ("(NotEmptyString)h ### (===)k");
	put_edit (put_edit_get_col() + 1, right);
	put_edit (1, "\nWhere (NotEmptyString)h, if 0<h<k, contains multiple empty handles.");
	return;
    }

    fputs ("\n\nSecond derivation:\n", listing);
    put_edit_nnl (1, get_nt_string (1));
    put_edit (put_edit_get_col() + 1, ambig_eof);

    if (A != 1) {
	put_edit_nnl (4, der_plus);
	put_edit_apnnl (left);
	put_edit_apnnl (get_nt_string (A));
	put_edit (put_edit_get_col() + 1, right);
    }

    put_edit_nnl (4, der_plus);
    put_edit_apnnl (left);
    put_edit_apnnl ("(");
    put_edit_apnnl (varstr_tostr (cycle_vstr));
    put_edit_apnnl (")k ");
    put_edit_apnnl (get_nt_string (A));
    put_edit_apnnl (" (===)k");
    put_edit (put_edit_get_col() + 1, right);
    put_edit_nnl (4, der_plus);
    put_edit_apnnl (left);
    put_edit_apnnl ("(");
    put_edit_apnnl (varstr_tostr (cycle_vstr));
    put_edit_apnnl (")k ");
    put_edit_apnnl (varstr_tostr (cycle_vstr));
    put_edit_apnnl (" ");
    put_edit_apnnl (get_nt_string (A));
    put_edit_apnnl (" === (===)k");
    put_edit (put_edit_get_col() + 1, right);
    put_edit_nnl (4, der_plus);
    put_edit_apnnl (left);
    put_edit_apnnl ("(");
    put_edit_apnnl (varstr_tostr (cycle_vstr));
    put_edit_apnnl (")k ");
    put_edit_apnnl (varstr_tostr (cycle_vstr));
    put_edit_apnnl (" ### === (===)k");
    put_edit (put_edit_get_col() + 1, right);

    if (!is_cycle_equal_Xi) {
	put_edit_nnl (4, der_star);
	put_edit_apnnl (left);
	put_edit_apnnl ("(");
	put_edit_apnnl (varstr_tostr (cycle_vstr));
	put_edit_apnnl (")k ");
	put_edit_apnnl (get_nt_string (Xi));
	put_edit_apnnl (" ### === (===)k");
	put_edit (put_edit_get_col() + 1, right);
    }

    put_edit_nnl (4, der_plus);
    put_edit_apnnl (left);
    put_edit_apnnl ("(");
    put_edit_apnnl (varstr_tostr (cycle_vstr));
    put_edit_apnnl (")k ");
    put_edit_apnnl ("### (===)k+1");
    put_edit (put_edit_get_col() + 1, right);
}

static BOOLEAN	read_non_empty (read_non_empty_next_state)
    int read_non_empty_next_state;
{
    register int 	n, item, kind;

    if (read_empty == NULL)
	read_empty = (int*) sxcalloc (xac2, sizeof (int));

    if ((kind = read_empty [read_non_empty_next_state]) == 0) {
	/* Pas encore examine. */
	n = 0;

	while ((item = nucl_i (read_non_empty_next_state, ++n)) > 0) {
	    item++;
	    /* On regarde tous les items du nucleus */
	    /* item: A -> alpha nt . Y1 Y2 ... Yn */

	    if (is_t_trans (item)) {
		/* FIRST (Y1 Y2 ... Yn) est non vide */
		read_empty [read_non_empty_next_state] = 2 /* read est non vide */;
		return TRUE;
	    }
	}

	read_empty [read_non_empty_next_state] = 1 /* read_non_empty_next_state est examine et read est vide */;
	return FALSE;
    }

    return kind == 2;
}



static BOOLEAN	ambiguity_check (X1)
    int		X1;
{
    /* Cette procedure recursive traverse en profondeur d'abord une fois et
       une seule chaque noeud du graphe induit par la relation includes. */
    /* Chaque fois qu'une ambiguite est detectee, elle est stockee. */
    register int	x, X, X2, ambiguity_check_next_state;
    BOOLEAN		is_local_ambiguity = FALSE;

    XxY_Xforeach (includes_hd, X1, x) {
	X2 = XxY_Y (includes_hd, x);

	if (SXBA_bit_is_reset_set (StNt_set, X2)) {
	    /* X2 n'a encore jamais ete examine */
	    PUSH (StNt_stack, X2);

	    if (ambiguity_check (X2)) {
		is_local_ambiguity = TRUE;
		/* Une ambiguite est atteinte depuis X2 */
		SXBA_1_bit (proto_ambig_StNt_set, X2);
	    }

	    POP (StNt_stack, X2);
	}
	else if (StNt_stack [X2] != 0) {
	    /* Un cycle sur X2 a ete detecte. */
	    /* Pour chaque nt-trans = (p, A) du cycle on regarde la condition
	       suffisante d'ambiguite : Read (p, A) non vide. */
	    X = 0;

	    do {
		if ((ambiguity_check_next_state = Q0xV_to_Q0 [X = StNt_stack [X]]) > 0 && read_non_empty (ambiguity_check_next_state)) {
		    /* Ambiguite detectee, on stocke le cycle. */
		    if (ambig_StNt_set == NULL) {
			ambig_StNt_set = sxba_calloc (X = XxY_top (Q0xV_hd) + 1);
			/* Contiendra tous les StNt tels que StNt includes* X et
			   X dans ambig_StNt_set. */
			proto_ambig_StNt_set = sxba_calloc (X);
			ambig_orig_set = sxba_calloc (X);
		    }

		    is_local_ambiguity = TRUE;
		    X = 0;

		    do {
			if ((ambiguity_check_next_state = Q0xV_to_Q0 [X = StNt_stack [X]]) > 0 && read_non_empty (ambiguity_check_next_state)) {
			    SXBA_1_bit (ambig_orig_set, X);
			}

			SXBA_1_bit (ambig_StNt_set, X);
		    } while (X != X2);
		}
	    } while (X != X2);
	}
	else if (proto_ambig_StNt_set != NULL && SXBA_bit_is_set (proto_ambig_StNt_set, X2))
	    is_local_ambiguity = TRUE;
	/* X2 ne se trouve pas dans StNt_stack, on n'est donc
	   pas en train de visiter ses descendants or X2 a deja ete visite,
	   la visite de X2 et de tous ses descendants est donc terminee et
	   toutes les infos deja recoltees. */
	/* Puisque les origines (racines) des graphes de includes ne sont jamais
	   impliquees dans des cycles, on ne parcoura pas plusieurs
	   fois le meme cycle. */
    }

    return is_local_ambiguity;
}




BOOLEAN	ambiguity (conflict_set, ambig_root_set)
    register SXBA	conflict_set, ambig_root_set;
{
    /* conflict_set est l'ensemble des etats conflictuels (au sens LALR(1)
       du terme) de l'automate LR(0).
       ambig_root_set est l'ensemble des transitions non-terminales racines
       de graphes de la relation includes qui comportent des cycles. */
    /* Retourne TRUE si la condition suffisante d'ambiguite est detectee.
       De plus ambig_root_set contient les origines (racines) des graphes
       d'includes ayant permis ces detections et ambig_StNt_set l'ensemble
       des transitions non-terminales impliquees dans des cycles dont au moins
       un representant (p, A) verifie Read (p, A) est non vide. */
    register int	StNt, St;
    BOOLEAN		ambiguity_is_ambiguous = FALSE;

    if (StNt_stack == NULL) {
	StNt_set = sxba_calloc (XxY_top (Q0xV_hd) + 1);
	StNt_stack = (int*) sxcalloc (XxY_top (Q0xV_hd) + 1, sizeof (int));
	StNt_stack [0] = -1;
    }

    StNt = 0;

    while ((StNt = sxba_scan (ambig_root_set, StNt)) > 0) {
	St = XxY_X (Q0xV_hd, StNt);

	if (SXBA_bit_is_set (conflict_set, St)) {
	    if (ambiguity_check (StNt))
		ambiguity_is_ambiguous = TRUE;
	    else
		SXBA_0_bit (ambig_root_set, StNt);
	}
	else
	    SXBA_0_bit (ambig_root_set, StNt);
    }

    if (ambig_StNt_set != NULL)
	sxfree (proto_ambig_StNt_set);

    if (read_empty != NULL)
	sxfree (read_empty);

    return ambiguity_is_ambiguous;
}



BOOLEAN is_ambig_on_StNt (StNt)
    int StNt;
{
    return SXBA_bit_is_set (ambig_StNt_set, StNt);
}

VOID	free_ambiguities ()
{
    if (ambig_orig_set != NULL) {
	sxfree (ambig_orig_set), ambig_orig_set = NULL;
    }

    if (init_t_set != NULL) {
	sxfree (init_t_set), init_t_set = NULL;
	sxfree (to_be_processed_t_set);
	sxfree (ambig_orig_sub_set);
	sxfree (state_path1);
	sxfree (state_path2);
	sxfree (items1);
	sxfree (items2);
	bstr_free (pre);
	varstr_free (post1);
    }
}



static BOOLEAN ambig_orig_F (X)
    int X;
{
    if (SXBA_bit_is_set (ambig_orig_set, X))
	SXBA_1_bit (ambig_orig_sub_set, X);

    return FALSE;
}

VOID	print_ambiguities (/* xac1, */t_set, StNt)
     int	/* xac1, */StNt;
     SXBA	t_set;
{
    /* On sait que:
          - l'etat xac1 est conflictuel en LALR (1)
	  - t_set est le sous-ensemble des terminaux en conflit
	    sur lesquels il faut sortir un message
	  - StNt est la racine d'un arbre includes dont au moins un
	    noeud est implique dans une ambiguite. */ 
    /* A chaque transition non-terminale (p, nt) origine d'une ambiguite et
       appartenant a l'arbre issu de StNt on associe un ensemble d'items
       A -> alpha nt . beta du coeur de l'etat q = p ->nt.
       Pour chacun de ces items on calcule init_t_set = t_set & FIRST1 (beta)
       Si cet ensemble est non vide, tous les terminaux qui le composent jouent
       un role analogue vis-a-vis des derivations, on en choisit un. */

    if (init_t_set == NULL) {
	int l = XxY_top (Q0xV_hd) + 1;

	init_t_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xtmax + 1);
	to_be_processed_t_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xtmax + 1);
	ambig_orig_sub_set = sxba_calloc (l);
	state_path1 = (int*) sxcalloc (l, sizeof (int));
	state_path2 = (int*) sxcalloc (l, sizeof (int));
	items1 = (int*) sxcalloc (l, sizeof (int));
	items2 = (int*) sxcalloc (l, sizeof (int));
	pre = bstr_alloc (128, min_lmargin, max_rmargin);
	post1 = varstr_alloc (32);
    }

    sxba_empty (StNt_set);
    includes_F = ambig_orig_F;
    sxba_empty (ambig_orig_sub_set);
    includes_traversal (StNt);

    if ((StNt = sxba_scan (ambig_orig_sub_set, 0)) > 0) {
	sxba_copy (to_be_processed_t_set, t_set);
	put_edit (1, "AMBIGUOUS");

	do {
	    register int	n, item;

/* On cherche un des terminaux impliques dans l'ambiguite. */

	    next_state = Q0xV_to_Q0 [StNt];
	    n = 0;

	    while ((item = nucl_i (next_state, ++n)) > 0) {
		item++;
		/* On regarde tous les items du nucleus */
		if ((init_t = -sxba_scan (sxba_and (first (sxba_empty (init_t_set), item), to_be_processed_t_set), 0)) < 0) {
		    /* init_t_set est un sous-ensemble de FIRST (Y1 Y2 ... Yn) */
		    sxba_minus (to_be_processed_t_set, init_t_set);
		    CLEAR (StNt_stack);
		    sxba_empty (StNt_set);
		    SCC (StNt, StNt, StNt_stack, StNt_set) /* TRUE */ ;


/* On est dans les conditions suivantes:
   StNt = (s1, nt)
   next_state = goto (s1, nt) est non nul et il contient
   it2: A -> gamma nt . Y1 ... Yn
   it1: B -> sigma nt . Z1 ... Zp avec
   init_t dans FIRST (Y1 ... Yn) et
   Z1 ... Zp =>* empty-string
   (it1 et it2 peuvent etre identiques)
   StNt_stack contient le cycle sur la relation includes
   
   On va fabriquer une sequence d'items
   nt    -> gamma1 B1 delta1
   .
   .
   .
   Bi-1 -> gammai Bi deltai = B -> gamma nt Z1 ... Zp
   en "remontant" le long du cycle a partir de it1 (deltaj =>* empty-string)
   et une autre sequence d'items qui lui "correspond"
   nt    -> sigma1 A1 ro1
   .
   .
   .
   Aj-1 -> sigmaj Aj roi = A -> sigma nt . Y1 ... Yn
   en "remontant" par le chemin precedent a partir de it2
   
   On sort les derivations:
   nt
   =>* gamma1 B1 delta1
   =>* gamma1 B1
   .
   .
   .
   =>* gamma1 ... gammai-1 B
   =>* gamma1 ... gammai-1 gamma nt Z1 ... Zp
   =>* gamma1 ... gammai-1 gamma nt
   =>* gamma1 ... gammai-1 gamma sigma1 A1 ro1
   =>* gamma1 ... gammai-1 gamma sigma1 A1 ...
   .
   .
   .
   =>* gamma1 ... gammai-1 gamma sigma1 ... sigmaj-1 A ...
   =>* gamma1 ... gammai-1 gamma sigma1 ... sigmaj-1 sigma nt Y1 ... Yn ...
   =>* gamma1 ... gammai-1 gamma sigma1 ... sigmaj-1 sigma nt init_t ...
   
   et
   nt
   =>* sigma1 A1 ro1
   =>* sigma1 A1 ...
   .
   .
   .
   =>* sigma1 ... sigmaj-1 A ...
   =>* sigma1 ... sigmaj-1 sigma nt Y1 ... Yn ...
   =>* sigma1 ... sigmaj-1 sigma nt init_t ...
   =>* sigma1 ... sigmaj-1 sigma gamma1 B1 delta1 init_t ...
   =>* sigma1 ... sigmaj-1 sigma gamma1 B1 init_t ...
   .
   .
   .
   =>* sigma1 ... sigmaj-1 sigma gamma1 ... gammai-1 B init_t ...	   
   =>* sigma1 ... sigmaj-1 sigma gamma1 ... gammai-1 gamma nt Z1 ... Zp init_t ...
   =>* sigma1 ... sigmaj-1 sigma gamma1 ... gammai-1 gamma nt init_t ...
   
   Or la remontee s'effectue par le meme chemin donc
   gamma1 ... gammai-1 gamma = sigma1 ... sigmaj-1 sigma
   les prefixes
   gamma1 ... gammai-1 gamma sigma1 ... sigmaj-1 sigma
   et
   sigma1 ... sigmaj-1 sigma gamma1 ... gammai-1 gamma
   sont identiques. */
		    
		    print_ambiguity (StNt_stack, item, StNt);
		}
	    }
	} while ((StNt = sxba_scan (ambig_orig_sub_set, StNt)) > 0);
    }
}






BOOLEAN	is_not_nullable (xnt)
    int xnt;
{
    /* On sait que is_not_nullable_nt =>+ vide. On regarde si is_not_nullable_nt peut aussi ne pas deriver ds le vide. */
    register int	x, item, is_not_nullable_nt, nt1;
    int			is_not_nullable_lim;

    if (not_nullable_set == NULL) {
	not_nullable_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1);
	not_nullable_already_checked_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1);
    }

    is_not_nullable_nt = XNT_TO_NT_CODE (xnt);

    if (SXBA_bit_is_set (not_nullable_set, is_not_nullable_nt))
	return TRUE;

    if (!SXBA_bit_is_reset_set (not_nullable_already_checked_set, is_not_nullable_nt))
	return FALSE;

    /* Evite de boucler sur les tests recursifs. */

    is_not_nullable_lim = bnf_ag.WS_NTMAX [is_not_nullable_nt + 1].npg;
    
    for (x = nt1 = bnf_ag.WS_NTMAX [is_not_nullable_nt].npg; x < is_not_nullable_lim; x++) {
	item = bnf_ag.WS_NBPRO [bnf_ag.WS_NBPRO [x].numpg].prolon;

	if (!SXBA_bit_is_set (bnf_ag.NULLABLE, item)) {
	    SXBA_1_bit (not_nullable_set, is_not_nullable_nt);
	    return TRUE;
	}
    }

    for (x = nt1; x < is_not_nullable_lim; x++) {
	item = bnf_ag.WS_NBPRO [bnf_ag.WS_NBPRO [x].numpg].prolon;

	while ((nt1 = bnf_ag.WS_INDPRO [item++].lispro) != 0) {
	    /* item : a -> alpha .nt1 beta avec nt1 dans N et nt1 =>+ vide. */
	    if (is_not_nullable (nt1)) {
		SXBA_1_bit (not_nullable_set, is_not_nullable_nt);
		return TRUE;
	    }
	}
    }

    return FALSE;
}



int	is_cycle_in_reads ()
{
/* Calcul de la relation reads :
                           A    B           +
   (p,A) reads (q,B) <=> p -> q -> r   et B => epsilon et r != 0 */

    BOOLEAN	is_cycle, is_ambig;

    if (sxba_scan (bnf_ag.BVIDE, 0) < 0)
	return FALSE;

    is_cycle = is_ambig = FALSE;
    XxY_alloc (&reads_hd, "reads", xac2, 4, 1, 0, NULL, statistics_file);
    StNt_to_reads_cycle_nb = NULL;

{
    register int	x, y, q, StNt2;
    int			r, B, A, unused;

    for (r = 2; r < xac2; r++) {
	if ((B = Q0_to_tnt_trans (r)) > 0 && SXBA_bit_is_set (bnf_ag.BVIDE, B)) {
	    XxY_Yforeach (Q0xQ0_hd, r, x) {
		q = XxY_X (Q0xQ0_hd, x);

		if ((A = Q0_to_tnt_trans (q)) > 0) {
		    StNt2 = XxY_is_set (&Q0xV_hd, q, B);

		    XxY_Yforeach (Q0xQ0_hd, q, y) {
			/* On a StNt1 reads StNt2 */
			XxY_set (&reads_hd, /* StNt1 */XxY_is_set (&Q0xV_hd, /* p = */XxY_X (Q0xQ0_hd, y), A), StNt2, &unused);
		    }
		}
	    }
	}
    }
}
    
    
    /* Recherche des cycles de reads */
    
    if (XxY_top (reads_hd) > 0) {
	/* reads est non vide */
	/* On calcule la fermeture transitive de reads */
	register int	i, x, y;
	register SXBA	index_set;
	register int	*indexes;
	SXBA		*reads_plus = NULL;
	X_header	reads_index_hd;
	int		reads_index_size;
	
	XxY_to_SXBM (&reads_hd, NULL, XxY_top (reads_hd), &reads_index_hd, &reads_plus);
	reads_index_size = X_top (reads_index_hd) + 1;
	fermer (reads_plus, reads_index_size);
	
	/* On calcule l'ensemble des transitions impliquees dans des cycles */
	/* Soit p ->A une transition impliquee ds un cycle. On sait que A =>+ vide.
	   Si de plus on a A =>+ x et x non vide, la grammaire est ambigue. */
	index_set = sxba_calloc (reads_index_size);
	
	for (i = 1; i < reads_index_size;i++) {
	    if (SXBA_bit_is_set (reads_plus [i], i)) {
		SXBA_1_bit (index_set, i);
		is_cycle = TRUE;

		if (!is_ambig && is_not_nullable (XxY_Y (Q0xV_hd, X_X (reads_index_hd, i))))
		    is_ambig = TRUE;
	    }
	}
	
	/* On partitionne cet ensemble en sous-ensembles qui contiennent les
	   cycles [disjoints]. */
	
	if (is_cycle) {
	    int l = XxY_top (Q0xV_hd) + 1;

	    StNt_set2 = sxba_calloc (l);

	    if (StNt_stack == NULL) {
		StNt_set = sxba_calloc (l);
		StNt_stack = (int*) sxcalloc (l, sizeof (int));
		StNt_stack [0] = -1;
	    }

	    indexes = (int*) sxcalloc (reads_index_size, sizeof (int));
	    /* p et l (ici 5 et 3) devraient etre tels que |index_set| = 2**p * (l + 1) */
	    X_alloc (&StNt_reads_cycle_hd, "StNt_reads_cycle", xac2, 3, oflw_StNt_reads_cycle, statistics_file);  
	    StNt_to_reads_cycle_nb = (int*) sxalloc (X_size (StNt_reads_cycle_hd) + 1, sizeof (int));

	    x = i = 0;
	    
	    while ((i++, x = sxba_scan_reset (index_set, x)) > 0) {
		indexes [x] = i;
		y = x;
		
		while ((y = sxba_scan (index_set, y)) > 0) {
		    if (SXBA_bit_is_set (reads_plus [y], x) && SXBA_bit_is_set (reads_plus [x], y)) {
			SXBA_0_bit (index_set, y);
			indexes [y] = i;
		    }
		}
	    }
	    
	    reads_cycle_sets = sxbm_calloc (reads_cycle_sets_line_nb = i, l);
	    
	    for (i = 1; i < reads_index_size;i++) {
		int	X;

		if ((x = indexes [i]) != 0) {
		    y = X_X (reads_index_hd, i);
		    X_set (&StNt_reads_cycle_hd, y, &X);
		    StNt_to_reads_cycle_nb [X] = x;
		    SXBA_1_bit (reads_cycle_sets [0], y);
		    SXBA_1_bit (reads_cycle_sets [x], y);
		}
	    }

	    sxfree (indexes);
	    X_lock (&StNt_reads_cycle_hd);
	}

	sxbm_free (reads_plus);
	X_free (&reads_index_hd);
	sxfree (index_set);
    }

    return is_cycle ? (is_ambig ? AMBIGUOUS_ : NOT_LRPI_) : NO_CONFLICT_;
}


SXBA is_not_LRpi_on_StNt (StNt)
    int StNt;
{
    sxba_empty (StNt_set);
    includes_F = NULL;
    includes_traversal (StNt);
    /* StNt_set contient tous les fils de StNt */
    return sxba_is_empty (sxba_and (sxba_copy (StNt_set2, StNt_set), reads_cycle_sets [0]))
	? NULL
	    : StNt_set2;
    /* StNt_set2 contient tous les fils de StNt impliques dans un cycle de reads */
}

VOID	free_not_LRpi ()
{
    if (NT_set != NULL) {
	varstr_free (cycle_vstr);
	SS_free (NT_stack);
	sxfree (S_stack);
	sxfree (S_set);
	sxfree (NT_set), NT_set = NULL;
    }

    if (reads_hd.display != NULL)
	XxY_free (&reads_hd);
    
    if (not_nullable_set != NULL) {
	sxfree (not_nullable_set), not_nullable_set = NULL;
	sxfree (not_nullable_already_checked_set);
    }

    if (StNt_set2 != NULL)
	sxfree (StNt_set2), StNt_set2 = NULL;

    if (StNt_stack != NULL) {
	sxfree (StNt_set);
	sxfree (StNt_stack), StNt_stack = NULL;
    }
}



VOID free_cycles ()
{
    if (reads_cycle_sets != NULL) {
	sxbm_free (reads_cycle_sets), reads_cycle_sets = NULL;
	X_free (&StNt_reads_cycle_hd);
	sxfree (StNt_to_reads_cycle_nb);
    }

    if (ambig_StNt_set != NULL)
	sxfree (ambig_StNt_set), ambig_StNt_set = NULL;
}

static BOOLEAN cycle_set_to_path (X)
    register int	X;
{
    /* cycle_set contient un cycle de la relation reads.
       Cette fonction recursive met dans StNt_stack un cycle autour
       de cycle_pivot. */
    register int x, X2;

    PUSH (StNt_stack, X);

    XxY_Xforeach (reads_hd, X, x) {
	if ((X2 = XxY_Y (reads_hd, x)) == cycle_pivot ||
	    ((SXBA_bit_is_set (cycle_set, X2) && StNt_stack [X2] == 0) && cycle_set_to_path (X2)))
	    return TRUE;
    }

    POP (StNt_stack, x);

    return FALSE;
}


VOID	print_not_LRpi_conflicts (kind)
    int kind;
{
    /* kind == AMBIGUOUS_ ou kind == NOT_LRPI_ */
    /* StNt_set2 contient des StNt impliques dans des cycles de reads. */
    /* Soit
          X1    X2    X3    Xn-1   Xn
       s1 -> s2 -> s3 -> ... -> sn -> s1
       un cycle de la relation reads tel que
                                                           Xn
       il existe un etat s n'appartenant pas au cycle et s -> s1
       On recherche un non terminal A tel que dans le noyau de s1 un item de la
       forme (il doit exister)
          A -> Xi ... Xn-1 Xn . beta
       avec beta non vide t.q.
          beta =>* X1 ... Xi-1 A ...

       Soit s' un etat tel que
          Xi ... Xn-1
       s' ----------> s
       et sigma un prefixe viable de s', on peut imprimer:
          S =>* sigma A ...

       Or on sait que
                          cycle               non vide
                /-----------^------------\     /-^-\
          A =>* Xi Xi+1 ... Xn X1 ... Xi-1  A  ===

	  (avec Xj =>+ epsilon)
	  (=== est non vide car sinon A =>+ A et INTRO aurait detecte l'ambiguite)


          S =>* sigma A ...
	    =>* sigma (cycle)k A (===)k ...

	    =>* sigma (cycle)k ### (===)k ...


          S =>* sigma A ...
	    =>* sigma (cycle)k A (===)k ...

	    =>* sigma (cycle)k cycle A === (===)k ...
	    =>* sigma (cycle)k Xi ### (===)k === ...
	    =>* sigma (cycle)k ### (===)k === ...


      Pour ameliorer on peut chercher une derivation
          A =>* x B z => x y z
	              =  ###
      et l'inserer.
    */

    /* Si kind == AMBIGUOUS_, on sait de plus que "cycle" peut deriver dans autre
       chose que le vide. On a donc :
	  S =>* sigma (cycle)k ### (===)k === ...       
	    =>+ sigma (cycle)h ### (===)k === ...
       ou (cycle)h, si 1<h<k, contient de multiples manches vides, ce qui montre
       l'ambiguite. */

    /*
      Recherche dans s1 du non terminal A.
      a) Un seul item dans le noyau, c'est forcement
         A -> Xi ... Xn-1 Xn . beta

      b) Tous les items du noyau ont meme lhs, c'est forcement A

      c) default = lhs ayant le + grand nombre d'items
         Pour chaque lhs != default
	     Pour chaque item de la forme lhs -> Xi ... Xn-1 Xn . beta (beta != epsilon)
	         Appeler check

	 Si echec A = default
     */
    register int StNt;

    if (NT_set == NULL) {
	NT_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xntmax + 1);
	S_set = sxba_calloc (xac2);
	S_stack = (int*) sxcalloc (xac2, sizeof (int));
	S_stack [0] = -1;
	NT_stack = SS_alloc (bnf_ag.WS_TBL_SIZE.xntmax + 1);
	cycle_vstr = varstr_alloc (64);
    }

    StNt = 0;

    while ((StNt = sxba_scan (StNt_set2, StNt)) > 0) {
	cycle_set = reads_cycle_sets [StNt_to_reads_cycle_nb [X_is_set (&StNt_reads_cycle_hd, StNt)]];
	sxba_minus (StNt_set2, cycle_set);
	CLEAR (StNt_stack);

	if (!cycle_set_to_path (cycle_pivot = StNt))
	    sxtrap (ME, "print_not_LRpi_conflicts");

	print_not_LRpi (StNt_stack, kind);
    }
}
