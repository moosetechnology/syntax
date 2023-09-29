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
/* 20030513 09:00 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 20000906 11:46 (phd):	Adaptation à MINGW			*/
/************************************************************************/
/* Ven 26 Nov 1999 10:36 (pb):	Remplacement de l'appel de 		*/
/*				state_nt_to_items par STATExNT_hd	*/
/************************************************************************/
/* Lun 25 Nov 1996 10:37 (pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 11-02-92 11:07 (pb):		Bug dans "stack_to_items"		*/
/************************************************************************/
/* 14-12-88 14:50 (pb):		Passage en LR(1)			*/
/************************************************************************/
/* 24-08-88 18:53 (bl):		Ajout de #ifdef MSDOS			*/
/************************************************************************/
/* 18-04-88 15:00 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/
#include "csynt_optim.h"
#if EBUG
static char	ME [] = "path";
#endif
char WHAT_PATH[] = "@(#)SYNTAX - $Id: path.c 627 2007-06-13 09:04:26Z rlacroix $" WHAT_DEBUG;

#define max_margin 100

extern VARSTR	cat_t_string ();
extern BOOLEAN	t_in_deriv ();
extern void	store_nt_items ();
VARSTR	path ();
static BOOLEAN	grow_items1 ();
static	void 	sample_path ();

static SXBA	*FSA, *FSA_plus;
static int	*approach;
static VARSTR	varstr;
static VARSTR	vtntstr;
static int	*state_path1, *state_path2;
static int	*state_to_clone;
static int	prev_xac2, first_state_in_C;
static SXBA	*FIRST, init_t_set, t1_subset, t2_subset;
static int	init_t;
static VARSTR	pre1, pre2, post1;
static int	*stack1, *check_stack1, *ntt_stack1;
static int	*stack2, *check_stack2, *ntt_stack2;
static int	prev_xxor_StNt;
static int	*items1, *items2;
static BOOLEAN	is_SCC, is_lr1, is_DUPLICATE, is_SAMPLE_PATH;
static int	init_item, item1, item2, init_nt_trans2, conf_state;
static char	blancs [ /* 132 */ ] =
"                                                                                                                                    "
     ;
static char	*blanks = &blancs [sizeof blancs - 1] ;
static char	pre [] = "   =>* ";
static char	postfixe [] = " ...";



static int	*reverse (reverse_stack1, reverse_stack2)
    register int	*reverse_stack1, *reverse_stack2;
{
    /* Remplit reverse_stack2 avec la pile inverse de reverse_stack1 */
    register int	X = 0;

    CLEAR (reverse_stack2);

    while ((X = reverse_stack1 [X]) >= 0) {
	PUSH (reverse_stack2, X);
    }

    return reverse_stack2;
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
	s1 = xor_StNts [nt_trans].state;

	while (start < item) {
	    spath [xspath--] = s1;
	    s1 = gotostar (s1, start, start + 1);
	    start++;
	}
    }
}



static VOID	items_to_spath (items, spath)
    register int	*items, *spath;
{
    /* Convertit le chemin d'items de items en un chemin d'etats dans spath */
    /* items[1] est un item de spath[*spath] */
    register int *lim;
    register int state, item, i, ls;

    state = spath [ls = *spath];
    lim = items + *items;

    for (items += 2; items <= lim ; items++) {
	if ((item = *items) < 0)
	    item = -item;
	else
	    item--;

	for (i = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].prolon ; i < item ; i++) {
	    state = spath [++ls] = gotostar (state, i, i+1);
	}
    }

    *spath = ls;
}


static int	nt_trans_to_item_no_i (nt_trans2, nt_trans1, i)
    int		nt_trans2, nt_trans1, i;
{
    /* On a nt_trans2 includes nt_trans1 */
    /* Calcule le i eme item de nt_trans2 compatible avec le couple precedent */
    register int	xtant, nt_trans_to_item_no_i_xptant, lim, state1, state2, nt1, rule_no, item, no;
    register struct xor_StNt	*aX;

    aX = xor_StNts + nt_trans1;
    state1 = aX->state;
    nt1 = bnf_ag.WS_INDPRO [tant [S1 [aX->xptant].ptant]].lispro;
    aX = xor_StNts + nt_trans2;
    state2 = aX->state;


/* Il existe dans state2 au moins un item de la forme : nt1 -> Y1 Y2 ... Yn .nt2 beta
       avec beta =>* vide et goto (state1, Y1 Y2 ... Yn) = state2, on retourne le i eme */

    if ((nt_trans_to_item_no_i_xptant = aX->xptant) > 0 /* cas normal */ ) {
	xtant = S1 [nt_trans_to_item_no_i_xptant].ptant;
	/* tant [xtant] est un item de la forme A -> alpha . nt2 gamma
	      C'est un bon si gamma =>* epsilon et A==nt1 et goto*(state1,alpha)==state2 */
	lim = S1 [nt_trans_to_item_no_i_xptant + 1].ptant;
	no = 0;

	while (xtant < lim) {
	    if (bnf_ag.WS_NBPRO [rule_no = bnf_ag.WS_INDPRO [item = tant [xtant++]].prolis].reduc == nt1 &&
		 sxba_bit_is_set (bnf_ag.NULLABLE, item + 1) && gotostar (state1, bnf_ag.WS_NBPRO [rule_no].prolon, item) ==
		 state2) {
		if (++no == i)
		    /* C'est le i eme */
		    return item + 1;
	    }
	}
    }
    else {
	/* cas d'une transition non terminale bidon dans l'etat en conflit,
	   -nt_trans_to_item_no_i_xptant designe un item reduce */
	if (i == 1)
	    return nt_trans_to_item_no_i_xptant;
    }

    return 0;
}



static int	compare (incl1, path1, compare_items1, incl2, path2, compare_items2, top)
    register int	*incl1, *incl2, *path1, *path2;
    int		*compare_items1, *compare_items2, *top;
{
    /* Cette fonction recursive verifie la condition LR1.
       Elle complete pathi avec un chemin (sequence d'etats) conforme a incli et
       issu de itemi.
       Si aucun pathi n'est inclus dans pathj, le conflit est resolvable en LR1,
       cette fonction retourne 0 et les deux chemins, top est l'indice de la partie
       commune.
       S'il existe pathi inclus dans pathj => non LR1:
           retourne 1 si path1 inclus dans path2
           retourne 2 si path2 inclus dans path1 */

    register int	x, item, top1, top2;
    int		lim, nt_trans1, nt_trans2, no;


/* path1[1:top] = path2[1:top] */

    top1 = *path1;
    top2 = *path2;

    if (*top < top1 && *top < top2) {
	/* Verification de l'inclusion */
	lim = min (top1, top2);

	for (x = *top + 1; x <= lim; x++) {
	    if (path1 [x] != path2 [x]) {
		/* Non inclusion */
		*top = x - 1;
		return 0;
	    }
	}

	/* inclusion locale, on continue */
	*top = lim;
    }

    if (*top == top1 && *incl1 != -1) {
	POP (incl1, nt_trans1);

	if ((nt_trans2 = *incl1) != -1) {
	    /* incl1 est non vide */
	    /* On a nt_trans1 includes nt_trans2 */
	    /* On calcule tous les items de nt_trans1 qui ont produit ce couple */

	    for (item = nt_trans_to_item_no_i (nt_trans1, nt_trans2, no = 1); item != 0; item = nt_trans_to_item_no_i (
		 nt_trans1, nt_trans2, ++no)) {
		*top = *path1 = top1;
		nt_trans_to_spath (nt_trans2, item, path1);

		if ((x = compare (incl1, path1, compare_items1, incl2, path2, compare_items2, top)) != 0) {
		    /* Inclusion */
		    compare_items1 [++*compare_items1] = item;
		    return x;
		}
	    }

	    /* Non inclusion, on remet tout en place */
	    PUSH (incl1, nt_trans1);
	    return 0;
	}
    }

    if (*top == top2 && *incl2 != -1) {
	POP (incl2, nt_trans1);

	if ((nt_trans2 = *incl2) != -1) {
	    /* incl2 est non vide */
	    /* On a nt_trans1 includes nt_trans2 */
	    /* On calcule tous les item de nt_trans1 qui ont produit ce couple */

	    for (item = nt_trans_to_item_no_i (nt_trans1, nt_trans2, no = 1); item != 0; item = nt_trans_to_item_no_i (
		 nt_trans1, nt_trans2, ++no)) {
		*top = *path2 = top2;
		nt_trans_to_spath (nt_trans2, item, path2);

		if ((x = compare (incl1, path1, compare_items1, incl2, path2, compare_items2, top)) != 0) {
		    /* Inclusion */
		    compare_items2 [++*compare_items2] = item;
		    return x;
		}
	    }

	    /* Non inclusion, on remet tout en place */
	    PUSH (incl2, nt_trans1);
	    return 0;
	}
    }

    /* Inclusion */
    return top1 <= top2 ? 1 : 2;
}



static BOOLEAN	grow_items2 (incl, grow_items2_path, grow_items2_items1, nt1, grow_items2_items2, nt2, grow_items2_item2, nt, top)
    register int	*incl, *grow_items2_path, *grow_items2_items1, *grow_items2_items2;
    int		grow_items2_item2, nt1, nt2, nt, top;
{
    int	state, xtant, lim, item, grow_items2_xptant, couple, x;

    if (nt2 == nt) {
	/* C'est fini pour grow_items2_items2 */
	if (nt1 == nt)
	    /* C'est fini pour grow_items2_items1 */
	    return TRUE;

	return grow_items1 (incl, grow_items2_path, grow_items2_items1, nt1, grow_items2_items2, nt2, grow_items2_item2, nt, top);
    }

    for (;;) {
	state = grow_items2_path [top];

	if (bnf_ag.WS_INDPRO [grow_items2_item2 - 1].lispro == 0) {
	    /* grow_items2_item2 : nt2 -> . alpha et nt2 != nt */
	    /* On parcourt les items de (state, nt2) */
	    /* Recherche des items de l'etat state ayant le marqueur LR devant nt2&i (nt2 est un .reduc) */
	    couple = XxY_is_set (&STATExNT_hd, state, nt2); /* Should be > 0 */

#if EBUG
	    if (couple == 0)
		sxtrap (ME, "grow_items2");
#endif

	    for (x = STATExNT2tnt [couple]; x != 0; x = tnt_next [x]) {
		/* Parcours des items de l'etat state ayant le marqueur LR devant nt2&i */
		grow_items2_xptant = tnt [x];
		xtant = S1 [grow_items2_xptant].ptant;
		lim = S1 [grow_items2_xptant + 1].ptant;

		while (xtant < lim) {
		    item = tant [xtant++];
		    /* item = B -> delta . nt2&i gamma */ 
		    grow_items2_items2 [++*grow_items2_items2] = item + 1;

		    if (grow_items2 (incl, grow_items2_path, grow_items2_items1, nt1, grow_items2_items2, bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc,
				     item, nt, top))
			return TRUE;

		    --*grow_items2_items2;
		}
	    }

	    return FALSE;
	}
	else {
	    /* grow_items2_item2 : nt2 -> X1 ... Xn . alpha et nt2 != nt */
	    if (*grow_items2_path == top)
		return grow_items1 (incl, grow_items2_path, grow_items2_items1, nt1, grow_items2_items2, nt2, grow_items2_item2, nt, top);

	    top++;

	    for (x = S1 [state].pprd; x != 0; x = lstprd [x].f2) {
		if (lstprd [x].f1 == grow_items2_path [top])
		    break;
	    }

	    if (x == 0)
		return FALSE;

	    grow_items2_item2--;
	}
    }
}



static BOOLEAN	grow_items1 (incl, grow_items1_path, grow_items1_items1, nt1, grow_items1_items2, nt2, grow_items1_item2, nt, top)
    register int	*incl, *grow_items1_path, *grow_items1_items1, *grow_items1_items2;
    int		grow_items1_item2, nt1, nt2, nt, top;
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
	top1 = *grow_items1_path;
	old_nt1 = nt1;

	for (item = nt_trans_to_item_no_i (nt_trans1, nt_trans2, no = 1); item != 0; item = nt_trans_to_item_no_i (
	     nt_trans1, nt_trans2, ++no)) {
	    nt_trans_to_spath (nt_trans2, item, grow_items1_path);

	    if (nt1 != nt) {
		grow_items1_items1 [++*grow_items1_items1] = item;
		nt1 = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc;

		if (nt1 == nt && nt2 == nt)
		    return TRUE;
	    }

	    if (nt2 != nt) {
		if (grow_items2 (incl, grow_items1_path, grow_items1_items1, nt1, grow_items1_items2, nt2, grow_items1_item2, nt, top)) {
		    return TRUE;
		}
	    }
	    else if (nt1 != nt) {
		if (grow_items1 (incl, grow_items1_path, grow_items1_items1, nt1, grow_items1_items2, nt2, grow_items1_item2, nt, top)) {
		    /* Inclusion */
		    return TRUE;
		}
	    }

	    *grow_items1_path = top1;
	    *grow_items1_items1 = xit1;
	    nt1 = old_nt1;
	}
    }


/* Non inclusion, on remet tout en place */

    incl [++*incl] = nt_trans1;
    return FALSE;
}



static VARSTR	varstr_shrink (varstr_shrink_vstr, new_lgth)
    VARSTR	varstr_shrink_vstr;
    int		new_lgth;
{
    /* Assume new_lgth <= varstr_length (varstr_shrink_vstr) */

    varstr_shrink_vstr->current = varstr_shrink_vstr->first + new_lgth;
    *(varstr_shrink_vstr->current) = NUL;
    return varstr_shrink_vstr;
}



static SXBA	DEBs (t_set, item)
    register SXBA	t_set;
    register int	item;
{
    /* Calcule et retourne t_set avec
       t_set = FIRST (beta) et item == A->alpha . beta */

    register int	DEBs_tnt;

    sxba_empty (t_set);

    while ((DEBs_tnt = bnf_ag.WS_INDPRO [item++].lispro) > 0) {
	/* Non terminal */
	sxba_or (t_set, FIRST [DEBs_tnt = XNT_TO_NT_CODE (DEBs_tnt)]);

	if (!sxba_bit_is_set (bnf_ag.BVIDE, DEBs_tnt))
	    break;
    }

    if (DEBs_tnt < 0)
	sxba_1_bit (t_set, -DEBs_tnt);

    return t_set;
}



static BOOLEAN	depth_first_traversal (stack, check_stack, nt_trans, f)
    register int	*stack, *check_stack;
    int		nt_trans;
    BOOLEAN	(*f) ();
{
    /* Rappelons qu'un noeud est une transition non terminale cad un couple (etat, liste d'items)
       ou chaque item de la liste est un element de "etat" et repere le meme non-terminal */
    /* Cette procedure recursive traverse en profondeur d'abord une fois et une seule chaque
       noeud du graphe induit par la relation includes.
       Sur chaque noeud et pour chaque item de la liste appelle la fonction f (path, item+1).
       path est le chemin menant de la racine a noeud.
       La fonction f a la possibilite d'interrompre la traversee en retournant TRUE */


    register int	x, X2;

    for (x = xor_StNts [nt_trans].X1_head; x != 0; x = includes [x].lnk) {
	if (check_stack [X2 = includes [x].X2] == 0 /* X2 n'est pas encore examine */ ) {
	    register int	depth_first_traversal_xptant, xtant, lim;
	    BOOLEAN	STOP;

	    PUSH (stack, X2);
	    PUSH (check_stack, X2);
	    depth_first_traversal_xptant = xor_StNts [X2].xptant;
	    xtant = S1 [depth_first_traversal_xptant].ptant;
	    /* tant [xtant] est un item de la forme A -> alpha . nt Y1 Y2 ... Yn
	       Les terminaux visibles sont ceux de FIRST (Y1 Y2 ... Yn) */
	    lim = S1 [depth_first_traversal_xptant + 1].ptant;

	    while (xtant < lim) {
		if (f (stack, tant [xtant++] + 1))
		    return TRUE;
	    }

	    STOP = depth_first_traversal (stack, check_stack, X2, f);
	    POP (stack, X2);

	    if (STOP)
		return TRUE;
	}
    }

    return FALSE;
}



static BOOLEAN	SCC (origine, X1, stack, check_stack)
    int		origine, X1, *stack, *check_stack;
{
    /* Non trivial Strongly Connected Component detection */
    /* Rend vrai ssi une composante fortement connexe comprenant origine
       est detectee. La composante est stockee dans stack.
       Sinon rend faux avec stack vide */

    register int	x, X2;

    PUSH (stack, X1);
    PUSH (check_stack, X1);

    for (x = xor_StNts [X1].X1_head; x != 0; x = includes [x].lnk) {
	X2 = includes [x].X2;

	if (check_stack [X2] == 0 /* X2 n'est pas encore examine */ ) {
	    if (SCC (origine, X2, stack, check_stack))
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



static VARSTR	b_cat (b_cat_vstr, str, col, b_cat_pre, right, post)
    VARSTR	b_cat_vstr;
    char	*str, *b_cat_pre, *post;
    int		*col, right;
{
    /* Bounded_CATenate : concatene str a b_cat_vstr en restant dans la limite right
       col est la colonne du dernier caractere de b_cat_vstr.
       Chaque ligne commence par b_cat_pre en colonne 1 et se termine par post (si non NULL)
       en colonne right */

    register int	strl;

    if (*col + (strl = strlen (str)) >= right) {
	if (post != NULL) {
	    varstr_catenate (b_cat_vstr, blanks - (right - *col - 1));
	    varstr_catenate (b_cat_vstr, post);
	}

	varstr_catenate (varstr_catenate (b_cat_vstr, "\n"), b_cat_pre);
	*col = strlen (b_cat_pre);
    }

    *col += strl;
    return varstr_catenate (b_cat_vstr, str);
}



static VARSTR	out_path_cat (out_path_cat_vstr, out_path_cat_path, top, col, lmargin, rmargin, post, item)
    VARSTR	out_path_cat_vstr;
    char	*post;
    int		*out_path_cat_path, top, *col, rmargin, lmargin, item;
{
    /* soit : out_path_cat_path [1] = S1 ... out_path_cat_path [top] = Sn
       Cette procedure concatene "beta" a out_path_cat_vstr en utilisant b_cat
       avec beta = X1 ... Xn-1 tel que goto (Si, Xi) = Si+1
       ecrit beta || nt ou nt est le nt en partie gauche de item et retourne beta */

    register int	x, out_path_cat_tnt, pre_col, vstr_lgth;
    int	nt;

    for (x = 2; x <= top; x++) {
	out_path_cat_tnt = approach [out_path_cat_path [x]];
	b_cat (out_path_cat_vstr, out_path_cat_tnt > 0 ? get_nt_string (out_path_cat_tnt) : varstr_tostr (cat_t_string (varstr_raz (vtntstr), out_path_cat_tnt)), col, blanks
	     - lmargin, rmargin, post);
	b_cat (out_path_cat_vstr, " ", col, blanks - lmargin, rmargin, post);
    }

    if ((nt = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc) > 1 /* pas axiome */ || top > 1 /* beta non vide */) {
	pre_col = *col;
	vstr_lgth = varstr_length (out_path_cat_vstr);
	b_cat (out_path_cat_vstr, get_nt_string (nt), col, blanks - lmargin, rmargin, post);
	b_cat (out_path_cat_vstr, postfixe, col, blanks - lmargin, rmargin, post);
	put_edit (1, varstr_tostr (out_path_cat_vstr));
	varstr_shrink (out_path_cat_vstr, vstr_lgth);
	*col = pre_col;
    }

    return out_path_cat_vstr;
}



static VARSTR	slice_cat (slice_cat_vstr, slice_cat_item1, slice_cat_item2, col, slice_cat_pre, right, post)
    VARSTR	slice_cat_vstr;
    char	*slice_cat_pre, *post;
    int		slice_cat_item1, slice_cat_item2, *col, right;
{
    /* soit : A->alpha beta gamma
       Cette procedure concatene "beta" a slice_cat_vstr en utilisabt b_cat
       avec slice_cat_item1 :  A->alpha .beta gamma
       et slice_cat_item2 :  A->alpha beta .gamma */

    register int	x, slice_cat_tnt;

    for (x = slice_cat_item1; x < slice_cat_item2; x++) {
	if ((slice_cat_tnt = bnf_ag.WS_INDPRO [x].lispro) > 0)
	    b_cat (slice_cat_vstr, get_nt_string (slice_cat_tnt), col, slice_cat_pre, right, post);
	else
	    b_cat (slice_cat_vstr, varstr_tostr (cat_t_string (varstr_raz (vtntstr), slice_cat_tnt)), col, slice_cat_pre, right, post);

	b_cat (slice_cat_vstr, " ", col, slice_cat_pre, right, post);
    }

    return slice_cat_vstr;
}



static VOID	outderiv (outderiv_pre, col, xitem, items, t_string, lmargin, rmargin, post, outderiv_postfixe)
    VARSTR	outderiv_pre;
    int		*col, xitem, *items;
    char	*t_string;
    int		lmargin, rmargin;
    char	*post, *outderiv_postfixe;
{

/* Ecrit la derivation contenue dans items, la chaine t_string est sortie a partir du niveau
   xitem */

    register int	x, lim, item;
    int		pre_col, pre_lgth, rule_no, outderiv_init_item;
    BOOLEAN	is_t_string;

    lim = items [0];
    is_t_string = xitem < 1;

    for (x = 1; x <= lim; x++) {
	item = items [x];

	if (item < 0) {
	    /* -item: A -> X1 ... Xn .  (Reduction en conflit) */
	    item = -item;
	}


/* Soit item : A -> alpha NT . beta
	   Ecrit:
	   Si beta non vide
	      outderiv_pre || alpha NT beta || is_t_string ? t_string  : NULL || ...
	   Puis:
	      outderiv_pre || alpha NT || is_t_string ? t_string : NULL || ...
	   et retourne outderiv_pre = outderiv_pre || alpha */
    
	/* concatenation de alpha */

	outderiv_init_item = bnf_ag.WS_NBPRO [rule_no = bnf_ag.WS_INDPRO [item].prolis].prolon;

	if (outderiv_init_item < item - 1 /* alpha non vide */ )
	    slice_cat (outderiv_pre, outderiv_init_item, item - 1, col, blanks - lmargin, rmargin, post);

	pre_lgth = varstr_length (outderiv_pre);
	pre_col = *col;


/* concatenation de NT beta */

	{
	    int		beta;

	    if ((beta = bnf_ag.WS_INDPRO [item].lispro) != 0 /* beta non vide */  && (x != xitem || beta > 0 || bnf_ag.
		 WS_INDPRO [item + 1].lispro != 0) /* beta != t au niveau xitem */ ) {
		/* Si NT existe, concatenation de NT beta sinon concatenation de beta */
		slice_cat (outderiv_pre, outderiv_init_item < item ? item - 1 : item, bnf_ag.WS_NBPRO [rule_no + 1].prolon - 1, col, blanks
		     - lmargin, rmargin, post);

		if (is_t_string && t_string != NULL)
		    b_cat (outderiv_pre, t_string, col, blanks - lmargin, rmargin, post);

		b_cat (outderiv_pre, outderiv_postfixe + 1, col, blanks - lmargin, rmargin, post);
		put_edit (1, varstr_tostr (outderiv_pre));
		varstr_shrink (outderiv_pre, pre_lgth);
		*col = pre_col;
	    }
	}

	is_t_string = x >= xitem;


/* concatenation de NT */

	if (outderiv_init_item < item)
	    slice_cat (outderiv_pre, item - 1, item, col, blanks - lmargin, rmargin, post);

	if (is_t_string && t_string != NULL)
	    b_cat (outderiv_pre, t_string, col, blanks - lmargin, rmargin, post);

	b_cat (outderiv_pre, outderiv_postfixe + 1, col, blanks - lmargin, rmargin, post);
	put_edit (1, varstr_tostr (outderiv_pre));
	varstr_shrink (outderiv_pre, pre_lgth);
	*col = pre_col;
    }
}



static VOID	stack_to_items (stack, items, nt_trans, item)
    register int	*stack, *items;
    int		nt_trans, item;
{
    /* Convertit la sequence de transition non terminale contenue dans stack en une
       sequence d'items */

    register int	X1, X2, x;

    items [x = 1] = item;
    X2 = nt_trans;
    X1 = 0;

    while ((X1 = stack [X1]) > 0) {
	items [++x] = nt_trans_to_item_no_i (X1, X2, 1);
	X2 = X1;
    }

    items [0] = x;
}




static BOOLEAN	check_item (check_item_stack2, item)
    int		*check_item_stack2, item;
{
    register int	nt_trans;
    char	*t_string;
    int		col1, state, top, lmargin;

    if (item != init_item)
	return FALSE /* Continue graph traversal */ ;


/* On tient un des chemins de derivation ayant provoque le look_ahead init_t */
    /* Le chemin est stocke dans check_item_stack2 */
    /* On fabrique le prefixe viable menant de l'etat initial a l'etat ou est cree
       le terminal du look-ahead implique dans le conflit. */

    state = xor_StNts [nt_trans = check_item_stack2 [0]].state;
    sample_path (1, state, state_path1);
    top = state_path1 [0] - (item - bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].prolon - 1);

    if (is_automaton)
	fprintf (listing, "\t\t\tin state %d\n", state);

    t_string = varstr_tostr (varstr_catenate (cat_t_string (varstr_raz (post1), init_t), " ."));
    /* Sinon on sort la (une?) derivation droite menant de cet etat a l'etat en conflit */
    put_edit (24, "by:");
    put_edit (21, get_nt_string (1 /* axiome */ ));
    POP (check_item_stack2, nt_trans);
    stack_to_items (check_item_stack2, items1, nt_trans, item);
    varstr_catenate (varstr_catenate (varstr_raz (pre1), blanks - 21), pre);
    lmargin = col1 = varstr_length (pre1);
    out_path_cat (pre1, state_path1, top, &col1, lmargin, max_margin, NULL, items1 [1]);
    outderiv (pre1, &col1, 1, items1, t_string, lmargin, max_margin, NULL, postfixe);
    PUSH (check_item_stack2, nt_trans);
    return TRUE /* Stop graph traversal */ ;
}



static BOOLEAN	check_SCC (check_SCC_stack2, item)
    int		*check_SCC_stack2, item;
{
    register int	nt_trans;
    char	*t_string;
    int		col1, nt, lmargin;

    if (item != init_item)
	return FALSE /* Continue graph traversal */ ;


/* On tient une des chemins de derivation ayant provoque le look_ahead init_t */
    /* Le chemin est stocke dans check_SCC_stack2 */

    nt_trans = check_SCC_stack2 [0];
    /* nt_trans est une transition non-terminale candidate pour tester un
       SCC eventuel */

    CLEAR (stack1);
    CLEAR (check_stack1);

    if (SCC (nt_trans, nt_trans, stack1, check_stack1)) {
	/* Cycle sur nt_trans stocke dans stack1: il y a ambiguite */
	is_SCC = TRUE;
	fputs ("\n    The grammar is AMBIGUOUS", listing);

/* Sortie des derivations montrant l'ambiguite */

	state_path1 [0] = 1;
	state_path1 [1] = xor_StNts [nt_trans].state;
	*items1 = 0;
	*items2 = 1;

        {
	    /* Remplit state_path2 avec la pile inverse de stack1 || nt_trans */
	    register int	X = 0, i;

	    state_path2 [i = 1] = nt_trans;

	    while ((X = stack1 [X]) > 0) {
		state_path2 [++i] = X;
	    }

	    *state_path2 = i;
	}

	items2 [1] = item;
	nt = bnf_ag.WS_INDPRO [item - 1].lispro;

	if (grow_items2 (state_path2, state_path1, items1, 0,
	     items2, bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc, item-1, nt, 1)) {
	    /* On fabrique items1 = inverse (items1)
	                   items2 = inverse (items2) */

	    inverse (items1);
	    inverse (items2);
	    cat_t_string (varstr_raz (post1), init_t);

	    {
		/* item = A -> X1 ... Xn nt . Y1 ... Yp avec Y1 ... Yp =>* init_t x */
		/* On regarde si x peut etre vide, dans ce cas on doit avoir
		       Y1 ... Yi-1 Yi Yi+1 ... Yn avec
		       Y1 ... Yi-1 =>* vide et Yi =>* init_t et Yi+1 ... Yp =>* vide
		   On se contente de Yi = init_t a la place de Yi =>* init_t */

		register int it = item, check_SCC_tnt;

		while ((check_SCC_tnt = bnf_ag.WS_INDPRO [it++].lispro) != 0) {
		    if (check_SCC_tnt <= 0 /* Forcement init_t */ || sxba_bit_is_set (FIRST [XNT_TO_NT_CODE (check_SCC_tnt)], -init_t)) {
			/* C'est le premier Yi tq Yi =>* init_t */
			if (init_t != check_SCC_tnt || !sxba_bit_is_set (bnf_ag.NULLABLE, it)) {
			    /* On ecrit "init_t ..." */
			    varstr_catenate (post1, " ...");	    
			}
			/* else On ecrit "init_t" */

			break;
		    }
		    /* else Forcement un nt qui derive dans le vide */
		}
	    }

	    t_string = varstr_tostr (post1);

/* Sortie de la premiere derivation */
	    fputs ("\n\nFirst derivation:\n", listing);
	    varstr_catenate (varstr_raz (pre1), pre);
	    lmargin = col1 = strlen (pre);
	    put_edit (1, get_nt_string (nt));
	    outderiv (pre1, &col1, *items1+1, items1, t_string, lmargin, max_margin, NULL, blanks-2);
	    outderiv (pre1, &col1, *items2, items2, t_string, lmargin, max_margin, NULL, blanks-2);

/* Sortie de la deuxieme derivation */
	    fputs ("\nSecond derivation:\n", listing);
	    varstr_catenate (varstr_raz (pre1), pre);
	    col1 = lmargin;
	    put_edit (1, get_nt_string (nt));
	    outderiv (pre1, &col1, *items2, items2, t_string, lmargin, max_margin, NULL, blanks-2);
	    outderiv (pre1, &col1, 0, items1, t_string, lmargin, max_margin, NULL, blanks-2);
	    put_edit_nl (1);
	}
	else {
	    fputs (
	     "Unable to exhibit two sample derivations.\nPlease send the offending grammar to\n\tPierre.Boullier@INRIA.Fr"
		 , listing);
	}

	return TRUE /* Stop graph traversal */ ;
    }

    return FALSE /* Continue graph traversal */ ;
}



VOID	out_reduce_derivation (t, item, nt_trans)
    int		t, item, nt_trans;
/* Terminal en conflit : t
   Item dont derive t : item - 1
   Transition non-terminale origine : nt_trans */
{
    init_item = item;
    init_t = t;
    CLEAR (stack2);
    CLEAR (check_stack2);
    PUSH (stack2, nt_trans);
    PUSH (check_stack2, nt_trans);
    depth_first_traversal (stack2, check_stack2, nt_trans, check_item);
}



BOOLEAN		check_ambig (t, item, nt_trans)
    int		t, item, nt_trans;
/* Terminal en conflit : t
   Item dont derive t : item - 1
   Transition non-terminale origine : nt_trans */
{

/* Si une transition non terminale (state, nt) est impliquee dans une composante
   fortement connexe non triviale du graphe induit par la relation "includes" et
   s'il existe une transition terminale (goto+(state,nt), t) alors la grammaire
   est ambigue (DeRemer, Pennello "Efficient Computation of LALR(1) Look-ahead Sets"
   ACM 79) */

/* Quand un conflit est detecte (apres application des regles de resolution utilisateur)
   dans un etat xac1, pour la reduction reduce sur le terminal t derivant de l'item item,
   on cherche une ambiguite eventuelle.
   On cherche toutes les transitions non-terminales (s,A) telles que nt_trans =
   (conf,nt_bidon) includes+ (s,A) ou nt_bidon est le nt bidon associe a reduce et
   tel que item-1 est un item de s.
   Pour chaque (s,A), composant non trivial d'un SCC, il y a ambiguite. On exhibe alors
   2 derivations differentes issues du meme non-terminal et derivant dans la meme
   proto-phrase */

    init_item = item;
    init_t = t;
    is_SCC = FALSE;
    CLEAR (stack2);
    CLEAR (check_stack2);
    PUSH (stack2, nt_trans);
    PUSH (check_stack2, nt_trans);
    depth_first_traversal (stack2, check_stack2, nt_trans, check_SCC);
    return is_SCC;
}



VOID	path_reallocation (first)
    SXBA	*first;
{
    register int	i;

    FIRST = first;

    if (prev_xac2 < xac2) {
	state_path1 = (prev_xac2 == 0) ? (int*) sxalloc (xac2 + 1, sizeof (int)) : (int*) sxrealloc (state_path1, xac2 +
	     1, sizeof (int));
	state_path2 = (prev_xac2 == 0) ? (int*) sxalloc (xac2 + 1, sizeof (int)) : (int*) sxrealloc (state_path2, xac2 +
	     1, sizeof (int));
	state_to_clone = (prev_xac2 == 0) ? (int*) sxalloc (xac2 + 1, sizeof (int)) : (int*) sxrealloc (state_to_clone,
	     xac2 + 1, sizeof (int));

	if (prev_xac2 == 0) {
	    t1_subset = sxba_calloc (bnf_ag.WS_TBL_SIZE.xtmax + 1);
	    t2_subset = sxba_calloc (bnf_ag.WS_TBL_SIZE.xtmax + 1);
	}

	for (i = prev_xac2 + 1; i <= xac2; i++) {
	    state_to_clone [i] = 0;
	}

	prev_xac2 = xac2;
    }

    if (prev_xxor_StNt < xxor_StNt) {
	stack1 = prev_xxor_StNt == 0 ? (int*) sxcalloc (xxor_StNt + 1, sizeof (int)) : (int*) sxrealloc (stack1,
	     xxor_StNt + 1, sizeof (int));
	check_stack1 = prev_xxor_StNt == 0 ? (int*) sxcalloc (xxor_StNt + 1, sizeof (int)) : (int*) sxrealloc (
	     check_stack1, xxor_StNt + 1, sizeof (int));
	ntt_stack1 = prev_xxor_StNt == 0 ? (int*) sxcalloc (xxor_StNt + 1, sizeof (int)) : (int*) sxrealloc (ntt_stack1,
	     xxor_StNt + 1, sizeof (int));
	items1 = prev_xxor_StNt == 0 ? (int*) sxcalloc (xxor_StNt + 1, sizeof (int)) : (int*) sxrealloc (items1,
	     xxor_StNt + 1, sizeof (int));
	stack2 = prev_xxor_StNt == 0 ? (int*) sxcalloc (xxor_StNt + 1, sizeof (int)) : (int*) sxrealloc (stack2,
	     xxor_StNt + 1, sizeof (int));
	check_stack2 = prev_xxor_StNt == 0 ? (int*) sxcalloc (xxor_StNt + 1, sizeof (int)) : (int*) sxrealloc (
	     check_stack2, xxor_StNt + 1, sizeof (int));
	ntt_stack2 = prev_xxor_StNt == 0 ? (int*) sxcalloc (xxor_StNt + 1, sizeof (int)) : (int*) sxrealloc (ntt_stack2,
	     xxor_StNt + 1, sizeof (int));
	items2 = prev_xxor_StNt == 0 ? (int*) sxcalloc (xxor_StNt + 1, sizeof (int)) : (int*) sxrealloc (items2,
	     xxor_StNt + 1, sizeof (int));
	stack1 [0] = -1;
	check_stack1 [0] = -1;
	stack2 [0] = -1;
	check_stack2 [0] = -1;

	for (i = prev_xxor_StNt + 1; i <= xxor_StNt; i++) {
	    stack1 [i] = 0;
	    check_stack1 [i] = 0;
	    ntt_stack1 [i] = 0;
	    items1 [i] = 0;
	    stack2 [i] = 0;
	    check_stack2 [i] = 0;
	    ntt_stack1 [i] = 0;
	    items1 [i] = 0;
	}

	prev_xxor_StNt = xxor_StNt;
    }
}



VOID	path_free ()
{
    if (prev_xxor_StNt < xxor_StNt) {
	sxfree (items2);
	sxfree (ntt_stack2);
	sxfree (check_stack2);
	sxfree (stack2);
	sxfree (items1);
	sxfree (ntt_stack1);
	sxfree (check_stack1);
	sxfree (stack1);
	prev_xxor_StNt = 0;
    }

    if (prev_xac2 == 0) {
	sxfree (t2_subset);
	sxfree (t1_subset);
	sxfree (state_to_clone);
	sxfree (state_path2);
	sxfree (state_path1);
	prev_xac2 = 0;
    }
}



VOID	build_fsa (build_fsa_nucl, first)
    int		*build_fsa_nucl;
    SXBA	*first;
{
    register int	i, x, lim, suc;
    register SXBA	fsai;
    register struct S1	*aS1;

    path_reallocation (first);
    varstr = varstr_alloc (128);
    vtntstr = varstr_alloc (32);
    FSA = sxbm_calloc (xac2, xac2);
    FSA_plus = sxbm_calloc (xac2, xac2);
 /* blanks = blancs + strlen (blancs); */
    pre1 = varstr_alloc (64);
    pre2 = varstr_alloc (64);
    post1 = varstr_alloc (32);

    for (i = 1; i < xac2; i++) {
	fsai = FSA [i];
	x = S1 [i].shftnt;
	lim = S1 [x + 1].ptnt;

	for (x = S1 [x].ptnt; x < lim; x++) {
	    aS1 = S1 + tnt [x];

	    if ((suc = aS1->tsucnt) > i) {
		sxba_1_bit (fsai, suc);
	    }
	}

	x = S1 [i].shftt;
	lim = S1 [x + 1].ptt;

	for (x = S1 [x].ptt; x < lim; x++) {
	    aS1 = S1 + tt [x];

	    if ((suc = aS1->tsuct) > i) {
		sxba_1_bit (fsai, suc);
	    }
	}

	sxba_copy (FSA_plus [i], fsai);
    }

    fermer (FSA_plus, xac2);
    approach = (int*) sxalloc (xac2 + 1, sizeof (int));
    approach [1] = bnf_ag.WS_TBL_SIZE.tmax;

    for (i = 2; i < xac2; i++) {
	approach [i] = bnf_ag.WS_INDPRO [build_fsa_nucl [S1 [i].etat] - 1].lispro;
    }
}



static	void sample_path (init, final, state_path)
    int		init, final, *state_path;
{
    /* remplit "state_path" avec un chemin (sequence d'etats) menant de init a final:
       "init etat1 ... etatn=final"
       avec init <= etat1 <= ... <= final */

    register int	i, k, x;

    state_path [x = 1] = init;

    for (i = init; i != final; i = k) {
	for (k = final; k > i; k--) {
	    if (sxba_bit_is_set (FSA [i], k) && (k == final || sxba_bit_is_set (FSA_plus [k], final)))
		break;
	}

	state_path [++x] = k;
    }

    state_path [0] = x;
}



VOID	out_shift_derivation (/* init, final, */item)
    int		/* init, final, */item;
{
    /* Imprime une derivation correspondant a une transition
       entre les etats init et final (final >= init) et utilisant item (item est un
       element de final) */
    /* Suppose que les derivations "reduce" ont deja ete sorties, on fait donc passer le chemin
       de init a final par un etat "responsable" du symbole de pre-vision en conflit pour le
       dernier reduce. state_path1 et items1 ont les valeurs qui conviennent */
    int		top, lmargin, col;
    static char	dot[2] = ".";

    put_edit (21, get_nt_string (1 /* axiome */ ));
    items_to_spath (items1, state_path1);
    top = state_path1 [0] - (item - (bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].prolon));
    varstr_catenate (varstr_catenate (varstr_raz (pre1), blanks - 21), pre);
    lmargin = col = varstr_length (pre1);
    out_path_cat (pre1, state_path1, top, &col, lmargin, max_margin, NULL, item);
    items1 [items1 [0] = 1] = item + 1;
    outderiv (pre1, &col, 1, items1, bnf_ag.WS_INDPRO [item + 1].lispro == 0 ? NULL : dot, lmargin, max_margin, NULL, postfixe);
}



VARSTR	path (init, final)
    int		init, final;
{
    /* retourne une chaine "init transition1 etat1 .... transitionn etatn=final"
     avec init <= etat1 <= ... <= final*/

    register int	lim, x, k, path_tnt;
    char		s [8];
    register char	*string;
    int			col;

    sample_path (init, final, state_path1);
    varstr_raz (varstr);
    col = 1;

    if (is_automaton) {
	sprintf (s, "%d ", init);
	varstr_catenate (varstr, s);
	col += strlen (s);
    }

    lim = state_path1 [0];

    for (x = 2; x <= lim; x++) {
	k = state_path1 [x];
	path_tnt = approach [k];
	string = path_tnt > 0 ? get_nt_string (path_tnt) : varstr_tostr (cat_t_string (varstr_raz (vtntstr), path_tnt));

	if ((col += (path_tnt = strlen (string))) > max_margin) {
	    varstr_catenate (varstr, "\n");
	    col = path_tnt + 1;
	}

	varstr_catenate (varstr, string);

	if (is_automaton) {
	    sprintf (s, " %d ", k);
	    varstr_catenate (varstr, s);
	    col += strlen (s);
	}
	else {
	    varstr_catenate (varstr, blanks - 1);
	    col++;
	}
    }

    return varstr;
}



static VOID	LR1_path (LR1_path_stack1, LR1_path_state_path1, LR1_path_items1, LR1_path_stack2, LR1_path_state_path2, LR1_path_items2, t)
     int		*LR1_path_stack1, *LR1_path_state_path1, *LR1_path_items1, *LR1_path_stack2, *LR1_path_state_path2, *LR1_path_items2,t;
{
    /* Le chemin 1 est inclus dans le chemin 2.
       Le chemin i se trouve dans LR1_path_stacki, LR1_path_state_pathi et LR1_path_itemsi :
       dans LR1_path_stacki sous forme d'une sequence de transitions non-terminale
       dans LR1_path_state_pathi sous forme d'une se'quence d'etats
       dans LR1_path_itemi sous forme d'une sequence d'items */
    int		top, col1, col2, nt_trans, lmargin, LR1_path_item2, LR1_path_item1, state2, top1;
    char	*t_string;
    

    top1 = *LR1_path_state_path1;
    /* state1 = LR1_path_state_path1 [top1]; */
    state2 = LR1_path_state_path2 [*LR1_path_state_path2];
    t_string = varstr_tostr (varstr_catenate (cat_t_string (varstr_raz (post1), t), " ."));

/* On sort, a titre d'exemple, les deux derivations correspondantes. */

    fputs ("\n    The grammar is not LR (1)", listing);

/* Sortie de la premiere derivation */

    varstr_catenate (varstr_raz (pre2), pre);
    lmargin = col2 = strlen (pre);
    sample_path (1, state2, LR1_path_state_path1) /* On ecrase LR1_path_state_path1 (il se trouve dans LR1_path_state_path2) */ ;
    LR1_path_item2 = LR1_path_items2 [1];
    top = LR1_path_state_path1 [0] - (LR1_path_item2 - bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [LR1_path_item2].prolis].prolon - 1);
    fputs ("\n\nFirst derivation:\n", listing);
    POP (LR1_path_stack2, nt_trans);
    put_edit (1, get_nt_string (1 /* axiome */ ));
    out_path_cat (pre2, LR1_path_state_path1, top, &col2, lmargin, max_margin, NULL, LR1_path_items2 [1]);
    outderiv (pre2, &col2, 1, LR1_path_items2, t_string, lmargin, max_margin, NULL, postfixe);

/* Sortie de la deuxieme derivation */

    varstr_catenate (varstr_raz (pre1), pre);
    lmargin = col1 = strlen (pre);


/* On complete LR1_path_state_path1 pour aller de state2 a state1 */

    {
	register int	x;

	for (x = *LR1_path_state_path2 - 1; x >= top1; x--) {
	    LR1_path_state_path1 [++*LR1_path_state_path1] = LR1_path_state_path2 [x];
	}
    }

    LR1_path_item1 = LR1_path_items1 [1];
    top = LR1_path_state_path1 [0] - (LR1_path_item1 - bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [LR1_path_item1].prolis].prolon - 1);
    fputs ("\nSecond derivation:\n", listing);
    POP (LR1_path_stack1, nt_trans);
    put_edit (1, get_nt_string (1 /* axiome */ ));
    out_path_cat (pre1, LR1_path_state_path1, top, &col1, lmargin, max_margin, NULL, LR1_path_items1[1]);
    outderiv (pre1, &col1, 1, LR1_path_items1, t_string, lmargin, max_margin, NULL, postfixe);
    put_edit_nl (1);
}



static BOOLEAN	is_in_C (q)
    int		q;
{
    /* Transition vers un etat de la partie commune */
    return state_to_clone [q] != 0;
}



static BOOLEAN	is_first_state_in_C (q)
    int		q;
{
    /* Transition vers le premier etat de la partie commune */
    return q == first_state_in_C;
}



static VOID	nt_copy (q1, q2, to_be_copied)
    int		q1, q2;
    BOOLEAN	(*to_be_copied) ();
{
    /* Duplication de l'etat q1 en q2 */
    /* Les occurrences des transitions non-terminales validees par to_be_copied
       sont copiees */

    register int	t, x, q, z;
    int		p, lim, lim2;

    p = S1 [q1].shftnt;
    lim = S1 [p + 1].ptnt;

    for (x = S1 [p].ptnt; x < lim; x++) {
	if (to_be_copied (q = S1 [z = tnt [x]].tsucnt)) {
	    lim2 = S1 [z + 1].ptant;

	    for (t = S1 [z].ptant; t < lim2; t++) {
		tant [xtant2] = tant [t];
		MAJ (&xtant2, 10);
	    }

	    S1 [xptant].tsucnt = state_to_clone [q];
	    tnt [xtnt2] = xptant;
	    MAJ (&xptant, 1);
	    S1 [xptant].ptant = xtant2;
	}
	else {
	    /* On ne touche pas aux autres transitions */
	    tnt [xtnt2] = z;
	}

	store_nt_items (q2, xtnt2); /* Ajoute le Ven 26 Nov 1999 10:36 */
	MAJ (&xtnt2, 11);
    }

    S1 [q2].shftnt = xptnt;
    MAJ (&xptnt, 1);
    S1 [xptnt].ptnt = xtnt2;
}



static VOID	t_copy (q1, q2, to_be_copied)
    int		q1, q2;
    BOOLEAN	(*to_be_copied) ();
{
    /* Duplication de l'etat q1 en q2 */
    /* Les occurrences des transitions terminales validees par to_be_copied
       sont copiees */

    register int	t, x, q, z;
    int		p, lim, lim2;

    p = S1 [q1].shftt;
    lim = S1 [p + 1].ptt;

    for (x = S1 [p].ptt; x < lim; x++) {
	if (to_be_copied (q = S1 [z = tt [x]].tsuct)) {
	    lim2 = S1 [z + 1].ptat;

	    for (t = S1 [z].ptat; t < lim2; t++) {
		tat [xtat2] = tat [t];
		MAJ (&xtat2, 9);
	    }

	    S1 [xptat].tsuct = state_to_clone [q];
	    tt [xtt2] = xptat;
	    MAJ (&xptat, 1);
	    S1 [xptat].ptat = xtat2;
	}
	else {
	    /* On ne touche pas aux autres transitions */
	    tt [xtt2] = z;
	}

	MAJ (&xtt2, 8);
    }

    S1 [q2].shftt = xptt;
    MAJ (&xptt, 1);
    S1 [xptt].ptt = xtt2;
}



static VOID	clone (clone_state_path1, clone_state_path2, top)
    register int	*clone_state_path1, *clone_state_path2;
    int		top;
{
    /* Chaque chemin clone_state_path1 et clone_state_path2 est forme de parties non vides
          clone_state_path1 = C P1
          clone_state_path2 = C P2
       ou C est la partie commune et Pi les parties propres.
       On dedouble chaque etat q de C en q et q' en remontant la branche commune C depuis la racine
       (etat en conflit).
       On duplique la racine r en r et r'
       Soit goto (q1, X) = q2 avec q1, q2 dans C et q1 != r, on sait que q2 a ete dedouble
       en q2 et q2', on duplique q1 en q1' en changeant goto(q1', X) en q2' et pred(q'2) = {q1'}
       Soit s1 et s2 les premiers etats de P1 et P2 tels que goto (s1, X) = q1 et goto (s2, X) = q1
       avec q1 de C, on duplique q1 en q1 et q1' en changeant goto (s2, X) en q1' et pred(q1')={s2}
    */

    int		last_state_in_P2;

    {
	/* Calcul des numeros des clones */
	register int	x, q1;

	for (x = 1; x <= top; x++) {
	    /* Attention le meme etat peut figurer plusieurs fois dans la partie commune
	       dans certains cas un peu tordus */
	    if (state_to_clone [q1 = clone_state_path1 [x]] == 0) {
		/* Nouvel etat de la partie commune */
		state_to_clone [q1] = clone_state_path2 [x] = xac2;
		S1 [xac2].StNt = 0;
		S1 [xac2].pprd = 0;
		MAJ (&xac2, 1);
	    }
	    /* else : on ne touche pas a clone_state_path2 */
	}

	first_state_in_C = clone_state_path1 [top];
	last_state_in_P2 = clone_state_path2 [x];
    }

    {
	/* On duplique */
	register int	q1, q2;
	int	xs;
	BOOLEAN		is_trans_to_C;

	for (xs = 1; xs <= top; xs++) {
	    if ((q1 = clone_state_path1 [xs]) != (q2 = clone_state_path2 [xs])) {
		/* Etat de la partie commune non encore traite */
		/* On duplique q1 en q2 */
		/* On remplit nucl */
		{
		    register int	x, lim;

		    lim = S1 [q1 + 1].etat;

		    for (x = S1 [q1].etat; x < lim; x++) {
			nucl [xnucl2] = nucl [x];
			MAJ (&xnucl2, 12);
		    }

		    S1 [q2 + 1].etat = xnucl2;

		    if ((x = S1 [q2].pred = S1 [q1].pred) != 0) {
			/* mise a jour de reduce_item_no */
			reduce_item_no += S1 [x + 1].ptr - S1 [x].ptr;
		    }
		}

		{
		    /* On remplit pred */
		    register int	x, q;
		    int		p, lim;

		    p = S1 [q1].shftnt;
		    is_trans_to_C = FALSE;
		    lim = S1 [p + 1].ptnt;

		    for (x = S1 [p].ptnt; x < lim; x++) {
			if ((q = S1 [tnt [x]].tsucnt) != 0) {
			    if (state_to_clone [q] != 0) {
				is_trans_to_C = TRUE;
				q = state_to_clone [q];
			    }

			    filprd (q2, q);
			}
		    }

		    if (is_trans_to_C) {
			/* Transition non-terminale vers un etat de la partie commune */
	    /* On copie tnt */
			nt_copy (q1, q2, is_in_C);
		    }
		    else {
			/* Transition non-terminale vers un etat hors de la partie commune */	    
			S1 [q2].shftnt = S1 [q1].shftnt;
		    }

		    p = S1 [q1].shftt;
		    is_trans_to_C = FALSE;
		    lim = S1 [p + 1].ptt;

		    for (x = S1 [p].ptt; x < lim; x++) {
			if ((q = S1 [tt [x]].tsuct) != 0) {
			    if (state_to_clone [q] != 0) {
				is_trans_to_C = TRUE;
				q = state_to_clone [q];
			    }

			    filprd (q2, q);
			}
		    }

		    if (is_trans_to_C) {
			/* Transition terminale vers un etat de la partie commune */
	    /* On copie tt */
			t_copy (q1, q2, is_in_C);
		    }
		    else {
			/* Transition vers un etat hors de la partie commune */	    
			S1 [q2].shftt = S1 [q1].shftt;
		    }
		}
	    }
	}
    }

    {
	int	X;
	int	first_state_in_clone_C = state_to_clone [first_state_in_C];

	X = bnf_ag.WS_INDPRO [nucl [S1 [first_state_in_C].etat] - 1].lispro;

	{
	    register int	x;

	    for (x = 1; x <= top; x++) {
		/* Suppression de (s,A) includes (s1,A1) pour tous les A, A1 tels que
	           s = clone_state_path1 [x] (etat de la partie commune)
	           s1 ->* last_state_in_P2 ->X first_state_in_C ->* s   ( -> = arc dans l'automate ) */
		cut (clone_state_path1 [x], last_state_in_P2, first_state_in_C);
	    }
	}


/* On change goto(last_state_in_P2,X) = first_state_in_C en
   goto(last_state_in_P2,X) = first_state_in clone_C */
/* On enleve last_state_in_P2 des predecesseurs de first_state_in_C
   et on ajoute last_state_in_P2 comme predecesseur de first_state_in_clone_C */

	{
	    register int	y1, y2;

	    for (y1 = 0, y2 = S1 [first_state_in_C].pprd; y2 != 0; y2 = lstprd [y1 = y2].f2) {
		if (lstprd [y2].f1 == last_state_in_P2) {
		    if (y1 == 0) {
			S1 [first_state_in_C].pprd = lstprd [y2].f2;
		    }
		    else {
			lstprd [y1].f2 = lstprd [y2].f2;
		    }

		    lstprd [y2].f2 = S1 [first_state_in_clone_C].pprd;
		    S1 [first_state_in_clone_C].pprd = y2;
		    break;
		}
	    }
	}


/* On change first_state_in_C en first_state_in_clone_C
   comme successeur de last_state_in_P2 sur X */

	if (X > 0 /* transition sur un non-terminal */ ) {
	    nt_copy (last_state_in_P2, last_state_in_P2, is_first_state_in_C);
	}
	else {
	    t_copy (last_state_in_P2, last_state_in_P2, is_first_state_in_C);
	}
    }

    {
	register int	x;

	for (x = 1; x <= top; x++) {
	    state_to_clone [clone_state_path1 [x]] = 0;
	}
    }
}



static BOOLEAN	check_stack2_sees_t (check_stack2_sees_t_stack2, item)
    int		*check_stack2_sees_t_stack2, item;
{
    int		top;

    if ((init_t = sxba_scan (sxba_and (DEBs (t2_subset, item), t1_subset), 0)) == -1)
	return FALSE /* Continue graph traversal */ ;


/* check_stack2_sees_t_stack2 contient un chemin X1 ... Xn tel que :
       nt_trans2 = Xn includes Xn-1 ... X2 includes X1 sees t2_subset */

    item2 = item;
    top = *state_path1 = *state_path2 = 1;
    state_path1 [1] = state_path2 [1] = conf_state;
    items1 [*items1 = 1] = item1;
    items2 [*items2 = 1] = item2;

    switch (compare (reverse (stack1, ntt_stack1), state_path1, items1, reverse (check_stack2_sees_t_stack2, ntt_stack2), state_path2, items2
	 , &top)) {
    case 0 /* state_path1 et state_path2 ont une partie commune */ :
	if (is_DUPLICATE) {
	    /* On duplique la partie commune */
	    clone (state_path1, state_path2, top);
	    return TRUE /* Stop graph traversal */ ;
	}

	break;

    case 1 /* state_path1 = state_path2 */ :
    case 2 /* state_path1 inclus dans state_path2 */ :
	is_lr1 = FALSE;

	if (is_SAMPLE_PATH)
	  LR1_path (stack1, state_path1, items1, check_stack2_sees_t_stack2, state_path2, items2, init_t/* , conf_state*/);

	break;

    case 3 /* state_path2 inclus dans state_path1 */ :
	is_lr1 = FALSE;

	if (is_SAMPLE_PATH)
	  LR1_path (check_stack2_sees_t_stack2, state_path2, items2, stack1, state_path1, items1, init_t/* , conf_state*/);

	break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #1");
#endif
      break;
    }

    return !is_lr1 /* Continue graph traversal if LR (1) */ ;
}



static BOOLEAN	check_stack1_sees_t (check_stack1_sees_t_stack1, item)
    int		*check_stack1_sees_t_stack1, item;
{
    if (sxba_is_empty (sxba_and (DEBs (t1_subset, item), init_t_set)))
	return FALSE /* Continue graph traversal */ ;


/* check_stack1_sees_t_stack1 contient un chemin X1 ... Xn tel que :
       nt_trans1 = Xn includes Xn-1 ... X2 includes X1 sees t1_subset */

    item1 = item;
    CLEAR (stack2);
    CLEAR (check_stack2);
    PUSH (stack2, init_nt_trans2);
    PUSH (check_stack2, init_nt_trans2);
    /* La poursuite de la traversee depend du resultat... */

    return depth_first_traversal (stack2, check_stack2, init_nt_trans2, check_stack2_sees_t);
}



BOOLEAN		check_lr1 (xac1, t_set, nt_trans1, nt_trans2, is_duplicate, is_sample_path)
    int		xac1, nt_trans1, nt_trans2;
    SXBA	t_set;
    BOOLEAN	is_duplicate, is_sample_path;
/* Etat en conflit : xac1
   Terminaux en conflit : t_set

   Transitions non-terminales origines : nt_trans1 et nt_trans2
   Si !(is_duplicate || is_sample_path)
      On regarde si xac1 est LR(1)
   Si is_duplicate
      On sait que xac1 est LR(1) => on duplique la partie commune a deux chemins
   Si is_sample_path
      Si xac1 n'est pas LR(1) => Sortie de deux derivations le montrant
*/
{

/* Pour chaque transition non-terminale (state, nt) nt_trans1 et nt_trans2 on calcule
   les ensembles de chemins X1 X2 .... Xn tel que:
   Xn = nt_trans1 includes Xn-1 ... X2 includes X1 sees t (t est dans t_set).
   Chacun de ces chemins est converti en chemin d'etat
   Soit C1 un chemin d'etat associe a nt_trans1 et C2 associe a nt_trans2. Si C1 est inclus dans
   C2 ou C2 inclus dans C1 alors la grammaire n'est pas LR (1) */

    init_t_set = t_set;
    conf_state = xac1;
    init_nt_trans2 = nt_trans2;
    is_DUPLICATE = is_duplicate;
    is_SAMPLE_PATH = is_sample_path;
    is_lr1 = TRUE;
    CLEAR (stack1);
    CLEAR (check_stack1);
    PUSH (stack1, nt_trans1);
    PUSH (check_stack1, nt_trans1);
    depth_first_traversal (stack1, check_stack1, nt_trans1, check_stack1_sees_t);
    return is_lr1;
}



VOID	erase_fsa ()
{
    varstr_free (post1);
    varstr_free (pre2);
    varstr_free (pre1);
    sxfree (approach);
    sxbm_free (FSA_plus);
    sxbm_free (FSA);
    varstr_free (vtntstr);
    varstr_free (varstr);
    path_free ();
}
