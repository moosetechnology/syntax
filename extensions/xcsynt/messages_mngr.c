/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

static char	ME [] = "messages_mngr";

#include "P_tables.h"
#include "csynt_optim.h"
#include "SS.h"
#include "RLR.h"
#include "bstr.h"

char WHAT_XCSYNTMESSAGESMNGR[] = "@(#)SYNTAX - $Id: messages_mngr.c 3652 2023-12-24 09:43:15Z garavel $" WHAT_DEBUG;

extern char	*conflict_kind_to_string (VARSTR result, SXINT conflict_kind, SXINT h_val, SXINT k_val);
extern bool	is_t_in_first (SXINT t, SXINT item);
extern bool	ttrue (SXINT item, SXINT LA_ref, bool is_prefixe);
extern bool	earley (SXINT item, 
			SXINT q, 
			SXINT LA, 
			SXINT LA_lgth, 
			XH_header *hd, 
			SXINT BS2);
extern bool	is_pss_include (SXINT *bot1, SXINT *top1, SXINT *bot2, SXINT *top2);
extern SXBA	fill_pss_set (struct ARC_struct *ARC, 
			      SXBA pss_set, 
			      SXINT ARC_state, 
			      SXINT t, 
			      SXINT type, 
			      SXBA set);
extern bool	bh_ARC_one_LA (struct ARC_struct *ARC, SXINT qtq);
extern bool	bs_ARC_one_LA (struct ARC_struct *ARC, SXINT qtq);

static XH_header	duo_params_hd;
static SXINT		duo_params_germe;

static SXINT		LA_length;
static XH_header	stack_hd, quad_hd;

static SXBA	PSS_SET, PREV_PSS_SET;
static SXINT	LA;
static SXINT	BS1, BS2;
static SXINT	ORIG_ITEM;
static SXINT	*BOT_items, *TOP_items;

static struct bstr	*pre;
static VARSTR	vsuffixe, vname;
static SXINT	*pre_stack, *post_stack;


static char	*c_name;
static SXINT	TYPE1, TYPE2, FORK;


static SXBA	LR0_statesN_set;
static SXINT	LR0_statesN_set_size;

static struct bstr	*bstr_cat_Vs (struct bstr *bstr, SXINT Vs, bool is_bot_up)
{
    /* Vs designe dans stack_hd une liste de symboles
       met dans vstr la chaine correspondante. */
    SXINT *bot, *top, *x, *lim, tnt;

    top = STACK_TOP (Vs);
    bot = STACK_BOT (Vs);

    if (is_bot_up) {
	x = bot;
	lim = top;
    }
    else {
	x = top - 1;
	lim = bot - 1;
    }

    while (x != lim) {
	bstr = bstr_cat_strsp (bstr, (tnt = *x) > 0 ? get_nt_string (tnt) : xt_to_str (tnt));
	x += is_bot_up ? 1 : -1;
    }

    return bstr;
}

static struct bstr	*bstr_cat_pre_stack (struct bstr *bstr)
{
    /* pre_stack contient une pile de symboles terminaux ou non-terminaux
       Concatene de bas en haut, a bstr, la chaine de symboles correspondants */
    SXINT x, tnt, top;;

    for (top = SS_top (pre_stack), x = SS_bot (pre_stack); x < top; x++) {
	tnt = SS_get (pre_stack, x);
	bstr = bstr_cat_strsp (bstr, tnt > 0 ? get_nt_string (tnt) : xt_to_str (tnt));
    }

    return bstr;
}

static struct bstr	*bstr_cat_post_stack (struct bstr *bstr)
{
    /* post_stack contient une pile de symboles terminaux
       Concatene de haut en bas, a bstr, la chaine de symboles correspondants */
    SXINT	x, bot;

    bot = SS_bot (post_stack);

    for (x = SS_top (post_stack) - 1 ; x >= bot; x--) {
	bstr = bstr_cat_strsp (bstr, xt_to_str (SS_get (post_stack, x)));
    }

    return bstr;
}



static void	xcsynt_write (SXINT red_no, SXINT item)
{
    /* item : A -> alpha . beta
       ecrit si red_no == 0
          =>* pre pre_stack post_stack suffixe
       sinon
          red_no => pre pre_stack beta post_stack suffixe
    */
    static char		der_star [] = "=>* ";
    static char		der [] = "=>  ";
    SXINT		pre_lgth, pre_col, tnt;

    if (red_no != 0) {
	put_edit_fnb (11, 4, red_no);
	put_edit_nnl (16, der);
    }
    else
	put_edit_nnl (16, der_star);

    bstr_get_attr (pre, &pre_lgth, &pre_col);
    pre = bstr_cat_pre_stack (pre);

    if (item != 0) {
	while ((tnt = bnf_ag.WS_INDPRO [item++].lispro) != 0)
	    pre = bstr_cat_strsp (pre, tnt > 0 ? get_nt_string (tnt) : xt_to_str (tnt));
    }

    pre = bstr_cat_post_stack (pre);
    pre = bstr_cat_str (pre, varstr_tostr (vsuffixe));
    put_edit_apnnl (bstr_tostr (pre));
    pre = bstr_shrink (pre, pre_lgth, pre_col);
}



static SXINT items_index (SXINT cur_index, SXINT top_index, SXINT *nb)
{
    /* cur_index repere dans stack_hd une liste d'items. */
    /* retourne le pointeur correspondant a l'exploitation de l'item_courant */
    SXINT	item, init_item, tnt, tail_item;

    if (cur_index == top_index)
	return top_index;

    if ((item = XH_list_elem (stack_hd, cur_index++)) == 2)/* S' -> S . eof */
	return items_index (cur_index, top_index, nb);

    init_item = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [tail_item = item].prolis].prolon;

    do {
	if ((tnt = bnf_ag.WS_INDPRO [item].lispro) > 0) {
	    if ((cur_index = items_index (cur_index, top_index, nb)) == top_index)
		return top_index;
	}
	else if (tnt < 0 && item < tail_item)
	    (*nb)++;
    } while (--item >= init_item);

    return cur_index;
}




#if 0
static void compute_items_np (SXINT cur_index, SXINT top_index, SXINT write_index, bool is_shift)
{
    SXINT	cur_item, new_index, tnt, items_nb = 0;
    SXINT			nb, old_nb, item, init_item, total_nb = 0;
    bool		should_write;

    while (cur_index < top_index) {
	should_write = cur_index == write_index;
	cur_item = item = XH_list_elem (stack_hd, cur_index++);
	/* item : A -> X1 --- Xp . Xp+1 --- Xq */
	init_item = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].prolon;
	nb = 0;

	if (cur_index < top_index) {
	    do {
		if ((tnt = bnf_ag.WS_INDPRO [cur_item].lispro) > 0) {
		    old_nb = nb;
		    
		    if ((new_index = items_index (cur_index, top_index, &nb)) == top_index) {
			nb = old_nb;
			break;
		    }
		    else
			cur_index = new_index;
		}
		else if (tnt < 0 && cur_item < item)
		    nb++;
	    } while (--cur_item >= init_item);
	}
	
	if (init_item < cur_item || should_write || nb > 0 || cur_index == write_index || cur_index == top_index) {
	    /* si should_write
	       red_no =>  X1 --- Xp Xp+1 --- Xq
	       sinon
	              =>* X1 --- Xp Xp+1 --- Xq
	       
	              =>* X1 --- Xp t1 --- tnb */

	    if (should_write) {
		item_nb_conflict = items_nb;

		if (is_shift) {
		    /* Shift */
		    XH_push (stack_hd, ORIG_ITEM);
		    XH_push (stack_hd, XH_X (stack_hd, LA + 1) - XH_X (stack_hd, LA) - total_nb - 1);
		    return;
		}
	    }
	    
	    XH_push (stack_hd, cur_item);
	    XH_push (stack_hd, nb);
	    total_nb += nb;
	    items_nb += 2;
	}
    }
}
#endif /* 0 */



static bool is_conflict_prod_reached (SXINT **bot_items)
{
    /* bot_items et TOP_items sont des pointeurs dans stack_hd dans une liste d'items. */
    SXINT	item, init_item;
    
    if (*bot_items == TOP_items)
	return true;

    if ((item = *(*bot_items)++) == 2)/* S' -> S . eof */
	return true; /* derive obligatoirement de l'axiome. */

    init_item = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].prolon;

    while (item >= init_item) {
	if (bnf_ag.WS_INDPRO [item--].lispro > 0 && is_conflict_prod_reached (bot_items))
	    return true;
    }

    return *bot_items == TOP_items;
}


static bool ecrire (bool should_write)
{
    /* BOT_items et TOP_items sont des pointeurs dans stack_hd dans une liste d'items. */
    SXINT	x, item, tnt, nb;
    SXINT			red_no, *ptr;
    bool		done, sub_write;
    
    if (BOT_items == TOP_items)
	return true;

    if ((item = *BOT_items++) == 2)/* S' -> S . eof */
	return ecrire (should_write);

    nb = item - (x = bnf_ag.WS_NBPRO [red_no = bnf_ag.WS_INDPRO [item].prolis].prolon);

    while (x < item) {
	SS_push (pre_stack, bnf_ag.WS_INDPRO [x++].lispro);
    }

    if (red_no != 0 && should_write)
	xcsynt_write (red_no, item);

    if ((tnt = bnf_ag.WS_INDPRO [item].lispro) != 0) {
	nb++;
	SS_push (pre_stack, tnt);

	if (should_write && bnf_ag.WS_INDPRO [item + 1].lispro != 0) {
	    varstr_catenate (vsuffixe, ".");
	    xcsynt_write ((SXINT)0, (SXINT)0);
	}
    }
	
    while (--nb >= 0) {
	if ((tnt = SS_pop (pre_stack)) > 0) {
	    if (conflict_derivation_kind == 2)
		sub_write = true;
	    else if (should_write)
		sub_write = is_conflict_prod_reached ((ptr = BOT_items, &ptr));
	    else
		sub_write = false;

	    done = ecrire (sub_write); /* (sub_write == false) => (done == false) */

	    if (should_write && !sub_write)
		xcsynt_write ((SXINT)0, (SXINT)0);

	    if (done)
		return true;
	}
	else
	    SS_push (post_stack, tnt);
    }

    return BOT_items == TOP_items;
}


static void	print_a_quad (bool is_eof, SXINT Vs, SXINT items/* , Ts */ /*, delta_items */)
{
    /* Vs, items et Ts sont des identifiants de stack_hd
       Vs (de bas en haut) est une liste de symbole designant un prefixe
       items est une liste d'items LR(0), s'exploite de bas en haut
       Ts (de bas en haut) est une liste de T designant une chaine
          de look_ahead. */
    SXINT	lhs;
    char	*print_a_quad_eof;

    put_edit_nnl (10, conflict_derivation_kind == 2 ? "detailed " : "");
    put_edit_ap ("sample derivation:");
    put_edit_nnl (13, get_nt_string (1 /* axiome */ ));
    put_edit_apnnl (" ");
    put_edit_ap (print_a_quad_eof = xt_to_str (bnf_ag.WS_TBL_SIZE.tmax) /* "END OF FILE" */ );
    pre = bstr_raz (pre);
    SS_clear_ss (pre_stack);
    SS_clear_ss (post_stack);

    if (Vs != 0)
	pre = bstr_cat_Vs (pre, Vs, false);

    varstr_raz (vsuffixe);

    if (is_eof) {
	varstr_catenate (vsuffixe, print_a_quad_eof);
    }
    else {
	varstr_catenate (vsuffixe, "...");
	lhs = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [*STACK_BOT (items)].prolis].reduc;

	if (lhs == 0)
	    lhs = 1; /* axiome */

/*	if (conflict_derivation_kind == 2 || delta_items == 0) { */
	    SS_push (pre_stack, lhs);
	    xcsynt_write ((SXINT)0, (SXINT)0);
	    SS_decr (pre_stack);
/*	} */
    }

    BOT_items = STACK_BOT (items);
    TOP_items = STACK_TOP (items);

    ecrire (true);
    put_edit_nl (1);
}


SXINT get_quad_nb (void)
{
    SXINT	q, nb = 0;
    SXINT			lim;

    if (quad_hd.display == NULL)
	return 0;

    for (lim = XH_top (quad_hd), q = 1; q < lim; q++) {
	if (!XH_is_erased (quad_hd, q))
	    nb++;
    }
    
    return nb;
}

bool erase_quads (SXINT ARC_kind, SXINT qtq)
{
    /* Supprime les quad de ARC dont la derniere transition est qtq. */
    SXINT x, bot;
    bool done = false;

    if (quad_hd.display != NULL) {
	for (x = XH_top (quad_hd) - 1; x > 0; x--) {
	    if (!XH_is_erased (quad_hd, x)) {
		bot = XH_X (quad_hd, x);

		if (XH_list_elem (quad_hd, bot + 4) == ARC_kind &&
		    XH_list_elem (quad_hd, bot + 5) == qtq) {
		    XH_erase (quad_hd, x);
		    done = true;
		}
	    }
	}
    }

    return done;
}

static SXINT install_a_quad (SXINT p0, SXINT p1, SXINT p2, SXINT p3, SXINT p4)
{
    SXINT	indice;

    XH_push (quad_hd, p0); /* prefixe viable */
    XH_push (quad_hd, p1); /* items_list */
    XH_push (quad_hd, p2); /* LookAhead list */
    XH_push (quad_hd, p3); /* item d'origine du conflit */
    XH_push (quad_hd, p4); /* [X]ARC_kind */
    XH_push (quad_hd, QTQ); /* Transition ds l'[X]ARC conduisant au conflit */
    XH_set (&quad_hd, &indice);
    return indice;
}


static SXINT	install_a_LR0_path (SXINT final);


bool make_a_quad (SXINT etat_charniere, SXINT item_charniere, SXINT *items_stack)
{
    /* item_charniere : A -> X1  Xp . Xp+1  Xn est dans etat_charniere
       Le sommet de items_stack repere un item de la meme regle (>=) */
    SXINT	*bot, *top;
    SXINT			r1, r2, bs1_bot, bs1_top;

    if (conflict_derivation_kind == 0) {
	install_a_quad ((SXINT)0, (SXINT)0, LA, ORIG_ITEM, XX_ARC_kind);
	return true;
    }

    top = items_stack + SS_top (items_stack);
    bot = items_stack + SS_bot (items_stack);
    
    do {
	XH_push (stack_hd, *bot);
    } while (++bot < top);
    
    XH_set (&stack_hd, &r2);

    if (!XX_is_XARC)
	BS1 = install_a_LR0_path (etat_charniere);
    /* else Dans le cas d'un XARC, a deja ete calcule en tenant compte des autres types
       en conflit. */

    /* BS1 contient contient une transition de V* entre l'etat initial et etat_charniere */

    /* On fabrique une transition de V* dans l'automate LR(0) entre l'etat initial
       et un etat contenant l'item A -> . X1  Xp Xp+1  Xn.
       Ce chemin doit etre un prefixe de BS1 */
    
    if (bnf_ag.WS_INDPRO [item_charniere - 1].lispro == 0) {
	r1 = BS1;
    }
    else {
	bs1_top = XH_X (stack_hd, BS1 + 1);
	bs1_bot = XH_X (stack_hd, BS1) + (item_charniere - bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item_charniere].prolis].prolon);
	
	/* On saute X1   Xp */
	if (bs1_bot == bs1_top)
	    r1 = 0;
	else {
	    while (bs1_bot < bs1_top) {
		XH_push (stack_hd, XH_list_elem (stack_hd, bs1_bot++));
	    }
	    
	    XH_set (&stack_hd, &r1); /* prefixe viable */
	}
    }
    
    /* item_nb_conflict : index relatif dans r2 [d'un successeur] de l'item conflictuel */
    install_a_quad (r1, r2, LA, ORIG_ITEM, XX_ARC_kind);
    
    return is_unique_derivation_per_conflict;
}

static SXINT fill_PREV_PSS_SET (struct ARC_struct *XARC,
				SXBA prev_pss_set,
				SXINT XARC_state,
				SXINT t, 
				SXINT type, 
				SXINT fill_PREV_PSS_SET_fork)
{
    SXINT	xfks1, xfks, xpis;
    SXINT			lim1, lim2, lim3, xpfks, xpfks1, cur_fork, cur_pss, cur_type, prev_bfsa, indice; 

    sxinitialise (prev_bfsa); /* pour faire taire gcc -Wuninitialized */
    sxba_empty (prev_pss_set);

    /* On cherche pour "type" dans l'etat XARC_state de l'XARC l'ensemble des forks
       (dont on extrait les pss) dont la transition sur t contient "fill_PREV_PSS_SET_fork". */
    /* Retourne le bfsa de l'un de ces forks. C'est un prefixe du bfsa de "fill_PREV_PSS_SET_fork". */
    for (lim1 = XH_X (XARC->Q_hd, XARC_state + 1), xpis = XH_X (XARC->Q_hd, XARC_state); xpis <
	 lim1; xpis++) {
	if ((cur_type = -XH_list_elem (XARC->Q_hd, ++xpis)) == type ||
	    (cur_type < grand && type < grand) /* shift (etat 1) */) {
	    xpfks = XH_list_elem (XARC->Q_hd, xpis - 1);
	    
	    for (lim2 = XH_X (fks_hd, xpfks + 1), xfks = XH_X (fks_hd, xpfks); xfks
		 < lim2; xfks++) {
		cur_fork = XH_list_elem (fks_hd, xfks);
		
		if ((indice = XxY_is_set (&FORKxT_hd, cur_fork, t)) != 0) {
		    /* Il y a une transition terminale sur t */
		    xpfks1 = FORKxT_to_fks [indice];
		    
		    for (lim3 = XH_X (fks_hd, xpfks1 + 1), xfks1 = XH_X (fks_hd, xpfks1); xfks1
			 < lim3; xfks1++) {
			if (XH_list_elem (fks_hd, xfks1) == fill_PREV_PSS_SET_fork) {
			    cur_pss = XxY_Y (forks, cur_fork);
			    SXBA_1_bit (prev_pss_set, cur_pss);
			    prev_bfsa = XxY_X (forks, cur_fork);
			    break;
			}
		    }
		}
	    }
	    
	    break;
	}
    }

    return prev_bfsa;
}


static bool call_earley (SXINT call_earley_fork)
{
    SXINT	indice, LA_init;
    bool	ret_val;

    if (duo_params_hd.display == NULL) {
	XH_alloc (&duo_params_hd, "duo_params_hd", xac2, 4, 2, NULL, statistics_file);
	duo_params_germe = germe;
    }
    else if (duo_params_germe != germe) {
	XH_clear (&duo_params_hd);
	duo_params_germe = germe;
    }

    XH_push (duo_params_hd, call_earley_fork);
    XH_push (duo_params_hd, BS2);
    XH_push (duo_params_hd, ORIG_ITEM);
    XH_push (duo_params_hd, LA);
    XH_push (duo_params_hd, BS1);

    if (XH_set (&duo_params_hd, &indice)) /* deja traite.. */
	return true;

    if (is_unique_derivation_per_conflict) {
	/* On regarde si une derivation pour ORIG_ITEM conduisant a la proto_phrase
	   "BS1 BS2 LA ..." a ete trouvee. On n'en cherche pas d'autres. */
	XH_push (duo_params_hd, BS2);
	XH_push (duo_params_hd, ORIG_ITEM);
	XH_push (duo_params_hd, LA);
	XH_push (duo_params_hd, BS1);

	if (XH_is_set (&duo_params_hd)) /* deja traite.. */
	    return true;
    }

    if (bnf_ag.WS_INDPRO [ORIG_ITEM].lispro == 0) {
	/* Reduce */
	ret_val = earley (ORIG_ITEM, germe, LA, LA_length, &stack_hd, BS2);
    }
    else {
	/* Shift */
	LA_init = XH_X (stack_hd, LA);

	if (earley_parse (LA_init, LA_init + LA_length, &stack_hd, ORIG_ITEM, ttrue))
	    ret_val = earley (ORIG_ITEM, germe, LA, LA_length, &stack_hd, BS2);
	else
	    ret_val = false;
    }

    if (is_unique_derivation_per_conflict && ret_val) {
	/* Une derivation pour ORIG_ITEM conduisant a la proto_phrase
	   "BS1 BS2 LA ..." a ete trouvee. On n'en cherche pas d'autres. */
	XH_push (duo_params_hd, BS2);
	XH_push (duo_params_hd, ORIG_ITEM);
	XH_push (duo_params_hd, LA);
	XH_push (duo_params_hd, BS1);
	XH_set (&duo_params_hd, &indice);
    }

    return ret_val;
}


static void build_XARC_path (/* XARC, XARC_state, */ SXINT type, SXINT build_XARC_path_fork)
{
    /* On est dans le cas XARC et UNBOUNDED_. */
    /* LA contient un chemin t1 t2 ... tn tel que
       q0 ->t1 q1 ->t2 q2 .... ->tn qn de XARC
       et qn == XARC_state est en conflit.
       qn-1 == prev_XARC_state
       build_XARC_path_fork = (bfsa, pss) de type "type" dans qn y participe
       BS2 est une chaine de bfsa
       On construit les sequences d'items, conformes a LA et BS2
       conduisant a fork. */
    /* Dans le cas LR(pi), c'est complique de sortir des bons messages.
       Rappelons qu'on a trouve un bfsa
          - inclus dans bfsa1 et
	  - prefixe de bfsa2
       Il faudrait trouver UN BON chemin c, sans boucle (eventuellement avec
       repetitions) tel que c soit dans bfsa et c2 = c || c' dans bfsa2
       Or, les bfsa sont des unions de chemins, c peut ne pas exister.
       Dans un premier temps, on fait simple. On lance le LR (k) avec
       k = |LA| =>
          - pas de conflit => OK
	  - clonable => OK
	  - sinon messages du LR (k)
    */

    SXINT	n, next, red_no;
    bool		has_pb = true;

    /* Calcul des valeurs initiales */
    if (-type < grand) {
	/* type shift */
	next = Q0xV_to_Q0 [-type];

	if (conflict_derivation_kind == 0 && nucl_i (next, 2) == 0) {
	    /* Un seul item shift, c'est obligatoirement le bon! */
	    install_a_quad ((SXINT)0, (SXINT)0, LA, ORIG_ITEM = nucl_i (next, 1), XX_ARC_kind);
	    has_pb = false;
	}
	else {
	    n = 0;

	    while ((ORIG_ITEM = nucl_i (next, ++n)) > 0) {
		if (call_earley (build_XARC_path_fork))
		    has_pb = false;
	    }
	}
    }
    else {
	red_no = -type - grand;
	n = 0;

	while ((ORIG_ITEM = orig_item_i (red_no, ++n)) > 0) {
	    if (conflict_derivation_kind == 0) {
		install_a_quad ((SXINT)0, (SXINT)0, LA, ORIG_ITEM, XX_ARC_kind);
		has_pb = false;
	    }
	    else
		if (call_earley (build_XARC_path_fork))
		    has_pb = false;
	}
    }

    if (has_pb)
	/* Pour le type courant, LA n'a pas ete confirme par earley comme etant
	   un contexte droit correct. Pb... */
	sxtrap (ME, "build_XARC_path");
}


static SXINT	get_a_pred_state (SXINT state)
{
    /* State est un etat non initial de l'automate LR (0).
       Retourne le plus petit predecesseur de state. */ 
    SXINT	x, y, pred = state;

    XxY_Yforeach (Q0xQ0_hd, state, x)
	if ((y = XxY_X (Q0xQ0_hd, x)) < pred)
	    pred = y;

    return pred;
}



static bool conformity_parse_stack_pss (SXINT *parse_stack, SXINT pss)
{
    /* On regarde si parse_stack et pss sont "compatibles".
       parse_stack est une SS_stack.
       pss est de type qcq. */
    SXINT	*pss_bot, *ps_bot;
    SXINT			*pss_top, *ps_top;

    ps_top = parse_stack + SS_top (parse_stack);
    ps_bot = parse_stack + SS_bot (parse_stack);
    pss_bot = PSS_BOT (pss);
    pss_top = PSS_TOP (pss);

    if (pss_kind == UNBOUNDED_) {
	/* Cas LALR */
	return is_pss_include (ps_bot, ps_top, pss_bot, pss_top);
    }

    if (pss_kind == BOUNDED_) {
	/* Cas R(h)LALR */
	while (pss_bot <= --pss_top) {
	    if (*pss_top != *--ps_top)
		return false;
	}
    }
    else {
	/* Cas R(oxo)LALR */
	SXINT		first, last, couple;
	SXINT	*x;

	if (*pss_bot != (first = *ps_bot) || *--pss_top != *--ps_top)
	    return false;

	while (++ps_bot <= ps_top) {
	    last = *ps_bot;
	    x = pss_bot;

	    while (++x < pss_top) {
		couple = *x;

		if (XxY_Y (Q0xQ0_hd, couple) == last && XxY_X (Q0xQ0_hd, couple) == first) {
		    first = last;
		    break;
		}
	    }

	    if (x == pss_top) /* Echec */
		return false;
	}
    }

    return true;
}

static bool conformity_LALR (SXINT *parse_stack)
{
    /* On verifie que parse_stack est un chemin
       dans un pss de PREV_PSS_SET. */
    SXINT pss = 0;

    while ((pss = sxba_scan (PREV_PSS_SET, pss)) > 0) {
	if (conformity_parse_stack_pss (parse_stack, pss)) {
	    return true;
	}
    }

    return false;
}


bool check_real_conformity (SXINT *parse_stack)
{
    /* Verifie que parse_stack est conforme a [l'un des] pss|fork de PREV_PSS_SET
       responsable du conflit. */
    if (PREV_PSS_SET == NULL)
	/* Pas de verification si inutile */
	return true;
    
    return conformity_LALR (parse_stack);
}


static void	print_title (char *shift_or_reduce, char *suffixe, char *conflict_name)
{
    char	str [8];

    put_edit_nnl (1, conflict_name);
    pre = bstr_raz (pre);
    pre = bstr_cat_str (pre, shift_or_reduce);
    pre = bstr_cat_str (pre, "-Reduce conflict in state #");
    sprintf (str, "%ld ", (SXINT) germe);
    pre = bstr_cat_str (pre, str);
    pre = bstr_cat_str (pre, "on ");
    pre = bstr_cat_Vs (pre, LA, true);
    pre = bstr_cat_str (pre, suffixe);
    pre = bstr_cat_str (pre, "between:");
    put_edit_ap (bstr_tostr (pre));
}


static SXINT	ARP_xprefixe (SXINT ARP_state, SXINT type, SXINT cn, SXINT bfsa, SXINT nmax)
{
    /* ARP_state est un etat de l'ARP
       cn = (nn, qn) est un item de ARP_state de type "type" et nn repere
       l'interieur de bfsa. */
    /* Met dans stack_hd un chemin (c1, c2, ..., cn-1, cn) de l'ARP entre */
    /* MODIF : */
    /* Met dans stack_hd un chemin (s1, s2, ..., sn-1, sn) de l'ARP entre
       l'etat initial de l'ARP et ARP_state tel que
          - ci = (ni, qi) est un item de si de type "type" de bfsa
	  - s1 = 1
	  - ARP_state = sn
	  - i < j => si < sj et ni <= nj
	  - s1 ->q1 s2 ->q2 s3 ... ->qn-1 sn
       Retourne qn-1 */
    SXINT	x, y, item, nnm1;
    SXINT			qN, qnm1, pred, is, bfsa_bot;
    SXINT	*bot, *top, *b1, *t1;

    if (ARP_state == 1) {
	if ((qN = XxY_Y (ARP_items, cn)) > xac2)
	    qN = XxY_X (Q0xN_hd, qN - xac2);

	return qN;
    }


/* On recherche le plus petit predecesseur de ARP_state */

    sxinitialise (qnm1); /* pour faire taire gcc -Wuninitialized */
    pred = ARP_state;

    for (x = XxY_top (ARPxTRANS_hd); x > 0; x--) {
	if (ARPxTRANS_to_next [x] == ARP_state && (y = XxY_X (ARPxTRANS_hd, x)) < pred) {
	    pred = y;
	    qnm1 = XxY_Y (ARPxTRANS_hd, x);
	}
    }


/* On recherche dans pred un "bon" item */

    nnm1 = XxY_X (ARP_items, cn);
    top = ARP_STATES_TOP (pred);
    bot = ARP_STATES_BOT (pred);
    bfsa_bot = XH_X (bfsa_hd, bfsa);
    sxinitialise (item); /* pour faire taire gcc -Wuninitialized */

    while (bot < top) {
	/* On parcourt les types */
	if (type == *(bot + 1)) {
	    is = labs (*bot);
	    b1 = ARP_ITEMS_BOT (is);
	    t1 = ARP_ITEMS_TOP (is);

	    while (b1 < t1) {
		x = *b1++;

		if ((qN = XxY_Y (ARP_items, x)) > xac2)
		    qN = XxY_X (Q0xN_hd, qN - xac2);

		if (qN == qnm1 && (y = XxY_X (ARP_items, x))
		     <= nnm1 && y >= bfsa_bot && XxY_is_set (&ARP_ItemxItem_hd, x, cn) > 0) {
		    /* On verifie de plus que l'item trouve x par transition
		       sur qnm1 est responsable de la fabrication de l'item cn. */
		    item = x, nnm1 = y;
		}
	    }

	    break;
	}
	else
	    bot += 2;
    }

    x = ARP_xprefixe (pred, type, item, bfsa, nmax);

    /* On verifie qu'on ne "depasse" pas nmax */
    if (nnm1 < nmax) {
	XH_push (stack_hd, qnm1);
	return qnm1;
    }

    return x;
}


static bool	ARP_knot_xpath (SXINT entree, SXINT sortie, SXINT *kbot, SXINT *ktop)
{
    /* Met dans stack_hd un chemin de knot entre entree (exclus) et
       sortie (inclus) */
    SXINT			*bot;
    SXINT	p, q, couple, next;
    bool		is_XQ0;

    bot = kbot;

    while (kbot < ktop) {
	is_XQ0 = ((couple = *kbot++) > Q0xQ0_top);
	q = is_XQ0 ? (couple -= Q0xQ0_top, XxY_Y (XQ0xXQ0_hd, couple)) :
	    XxY_Y (Q0xQ0_hd, couple);
	    
	if (q == entree) {
	    p = is_XQ0 ? XxY_X (XQ0xXQ0_hd, couple) : XxY_X (Q0xQ0_hd, couple);

	    if (SXBA_bit_is_reset_set (LR0_statesN_set, p)) {
		next = p < xac2 ? p : XxY_X (Q0xN_hd, p - xac2);
		XH_push (stack_hd, next);

		if (p == sortie)
		    return true;

		if (ARP_knot_xpath (p, sortie, bot, ktop))
		    return true;

		XH_pop (stack_hd, next);
		SXBA_0_bit (LR0_statesN_set, p);
	    }
	}
    }

    return false;
}



static void	ARP_call_knot_xpath (SXINT entree, SXINT sortie, SXINT knot)
{
    sxba_empty (LR0_statesN_set);
    SXBA_1_bit (LR0_statesN_set, entree);

    if (!ARP_knot_xpath (entree, sortie, BFSA_BOT (knot), BFSA_TOP (knot)))
	sxtrap (ME, "ARP_call_knot_path");
}



static SXINT	ARP_xpath (SXINT ARP_state, SXINT type, SXINT item, SXINT bfsa1, SXINT prev_bfsa1)
{
    /* item = (n, q) est un item de type "type" de ARP_state
       et designe un emplacement dans bfsa1.
       prev_bfsa1 est un bfsa qui est un prefixe de bfsa1
       Il existe un sous-chemin de l'ARP, prefixe d'un chemin menant a ARP_state,
       qui est un chemin de prev_bfsa1
       Cette fonction
       Range dans stack_hd un "chemin" c1 de prev_bfsa1 qui "passe" par item.
       Le prefixe c1 est calcule par ARP_xprefixe, le suffixe (au-dela de n)
       des chemins de prev_bfsa1 est calcule "au plus juste".
       Retourne l'etat du sommet. */
    SXINT	n, state, knot, in;
    SXINT	*bot;
    SXINT			out, prev_state, nmax;

    if (ARP_state != 0) {
	n = XH_X (bfsa_hd, prev_bfsa1 + 1) - XH_X (bfsa_hd, prev_bfsa1) - 1 /* a cause du
									       sommet nul */;
	prev_state = ARP_xprefixe (ARP_state, type, item, bfsa1, nmax = XH_X (bfsa_hd, bfsa1) + n);
	n = XxY_X (ARP_items, item);

	if (n >= nmax)
	    /* fin de prev_bfsa1 */
	    return prev_state;

	n += XH_X (bfsa_hd, prev_bfsa1) - XH_X (bfsa_hd, bfsa1);
	in = XxY_Y (ARP_items, item);
	knot = XH_list_elem (bfsa_hd, n);
	bot = BFSA_BOT (knot);

    }
    else {
	n = XH_X (bfsa_hd, prev_bfsa1);
	knot = XH_list_elem (bfsa_hd, n);
	in = *(bot = BFSA_BOT (knot));
    }

    state = in < xac2 ? in : XxY_X (Q0xN_hd, in - xac2);
    XH_push (stack_hd, state);
    n++;
    /* On extrait du knot un chemin entre in et out */
    
    if (in != (out = *(bot + 2))) {
	ARP_call_knot_xpath (in, out, *(bot + 1));
    }
    
    while ((knot = XH_list_elem (bfsa_hd, n++)) > 0) {
	/* On extrait du knot un chemin entre in et out */
	bot = BFSA_BOT (knot);

	if ((in = *bot) != (out = *(bot + 2))) {
	    ARP_call_knot_xpath (in, out, *(bot + 1));
	}
    }

    return out < xac2 ? out : XxY_X (Q0xN_hd, out - xac2);
}


static SXINT   install_a_LR0_path (SXINT final)
{
    /* Met dans stack_hd un chemin de l'automate LR (0) entre final et
       l'etat initial */
    /* Retourne l'identifiant unique de ce chemin. */
    /* Tout etat s (sauf l'etat initial) a un predecesseur s' tel que s' < s. */
    SXINT		result;

    if (final == 1)
	return 0;

    while (final != 1) {
	/* On cherche son plus petit predecesseur */
	XH_push (stack_hd, Q0_to_tnt_trans (final));
	final = get_a_pred_state (final);
    }

    XH_set (&stack_hd, &result);
    return result;
}



static SXINT	cat_a_LR0_path (SXINT bs1, SXINT bs2, SXINT bs_2)
{
    /* Met dans stack_hd le chemin de l'automate LR (0) :
       (bs2 - bs_2) || bs1 */
    /* Retourne l'identifiant unique de ce chemin. */
    SXINT	bs_bot, bs_top, state;
    SXINT		result;

    if (bs2 == bs_2)
	return bs1;

    bs_top = XH_X (stack_hd, bs2 + 1) - 1;
    bs_bot = XH_X (stack_hd, bs2) + (XH_X (stack_hd, bs_2 + 1) - XH_X (stack_hd,
	 bs_2)) - 1;

    while (bs_bot < bs_top) {
	state = XH_list_elem (stack_hd, bs_bot++);
	XH_push (stack_hd, Q0_to_tnt_trans (state));
    }

    if (bs1 != 0) {
	bs_top = XH_X (stack_hd, bs1 + 1);
	bs_bot = XH_X (stack_hd, bs1);

	while (bs_bot < bs_top) {
	    state = XH_list_elem (stack_hd, bs_bot++); /* c'est un V */
	    XH_push (stack_hd, state);
	}
    }

    XH_set (&stack_hd, &result);
    return result;
}




void messages_mngr_alloc (void)
{
    SXINT l;

    if (is_lr_constructor) {
	if (LR0_statesN_set == NULL)
	    LR0_statesN_set = sxba_calloc (LR0_statesN_set_size = xac2 + XxY_top (Q0xN_hd) + 1);
	else if (LR0_statesN_set_size < (l = xac2 + XxY_top (Q0xN_hd) + 1))
	    LR0_statesN_set = sxba_resize (LR0_statesN_set, LR0_statesN_set_size = l);
    }

    if (pre_stack == NULL) {
	pre = bstr_alloc (64, 16, 100);
	vsuffixe = varstr_alloc (8);
	vname = varstr_alloc (64);
	pre_stack = SS_alloc (xac2);
	post_stack = SS_alloc (xac2);
	XH_alloc (&stack_hd, "stack_hd", 4 * xac2, 4, 2, NULL, statistics_file);
	XH_alloc (&quad_hd, "quad_hd", xac2, 4, 5, NULL, statistics_file);
    }
}


void messages_mngr_free (void)
{

    if (duo_params_hd.display != NULL)
	XH_free (&duo_params_hd);

    if (pre_stack != NULL) {
	bstr_free (pre);
	varstr_free (vsuffixe);
	varstr_free (vname);
	SS_free (pre_stack), pre_stack = NULL;
	SS_free (post_stack);
	XH_free (&stack_hd);
	XH_free (&quad_hd);

	if (LR0_statesN_set != NULL)
	    sxfree (LR0_statesN_set);
    }
}


static bool one_ambig_LA (struct ARC_struct *XARC,
			     SXINT prev_XARC_state, 
			     SXINT t/* , XARC_state */ )
{
    SXINT state, prev_bfsa;

    prev_bfsa = fill_PREV_PSS_SET (XARC, PREV_PSS_SET = pss_sets [work_set_no], prev_XARC_state, -t, -TYPE1, FORK);
    state = ARP_xpath ((SXINT)0, (SXINT)0, (SXINT)0, XxY_X (forks, FORK), prev_bfsa);
    XH_set (&stack_hd, &BS2);
    BS1 = install_a_LR0_path (state);
    build_XARC_path (/* XARC, XARC_state, */TYPE1, FORK);
    fill_PREV_PSS_SET (XARC, PREV_PSS_SET, prev_XARC_state, -t, -TYPE2, FORK);
    build_XARC_path (/* XARC, XARC_state, */TYPE2, FORK);
    return is_unique_conflict_per_state;
}


void mm_process_one_LA (struct ARC_struct *ARC)
{
    /* Un (nouveau) LA vient d'etre trouve. */
    /* LA est global */
    SXINT	x;
    SXINT			qtq, t, prev_ARC_state, next_ARC_state, type_nb, red_no,
                        /* quad_hd_top, */conflict_kind, LA_init, lim, n, next, type;
    bool		is_new_mess;

    sxinitialise (t); /* pour faire taire gcc -Wuninitialized */
    sxinitialise (qtq); /* pour faire taire gcc -Wuninitialized */

    for (x = SS_top (ws) - 1, lim = SS_bot (ws); x > lim; x--) {
	qtq = SS_get (ws, x);
	t = -XxYxZ_Y (ARC->QxTxQ_hd, qtq);
	XH_push (stack_hd, t);
    }
		
    XH_set (&stack_hd, &LA);
    LA_length = XH_list_lgth (stack_hd, LA);
    QTQ = qtq; /* Global pour install_a_quad */
    prev_ARC_state = XxYxZ_X (ARC->QxTxQ_hd, qtq);
    next_ARC_state = XxYxZ_Z (ARC->QxTxQ_hd, qtq);
    XX_ARC_kind = ARC->kind;
    conflict_kind = ARC->attr [next_ARC_state].conflict_kind;
    /* Devra-t-on verifier la coherence des piles calculees (par earley) avec les pps?
       uniquement s'il y a eu restriction. */
    XX_is_pss_check = !(pss_kind == UNBOUNDED_ &&
		     (conflict_kind == NOT_LALRk_ || conflict_kind == NOT_LALR_));
    LA_init = XH_X (stack_hd, LA);
    type_nb = XH_list_lgth (ARC->Q_hd, next_ARC_state) >> 1;

    if (ARC->is_XARC) {
	XX_is_XARC = true;

	if (!ARC->is_ARP)
	    /* Egalite des forks. */
	    one_ambig_LA (ARC, prev_ARC_state, t/* , next_ARC_state */);
	else
	    call_ARP_conflict_messages (ARC, prev_ARC_state, t, next_ARC_state);
    }
    else {
	XX_is_XARC = false;

	for (x = 0; x < type_nb; x++) {
	    if (!sxba_is_empty (PSS_SET = pss_sets [x])) {
		type = -kas [x].type;
		PREV_PSS_SET = XX_is_pss_check ?
		    fill_pss_set (ARC, and_set, prev_ARC_state, t, type, PSS_SET) : NULL;

		if (XX_is_pss_check) {
		    /* On stocke dans ws, pour chaque transition de LA les pss_list qui
		       conduisent a PSS_SET. */
		    SXINT	y;
		    SXBA	pss_set1, pss_set2;
		    bool		loop;

		    lim = SS_top (ws);
		    y = SS_bot (ws) + 1; /* On saute le "fond" qui a servi a construire ws. */
		    SS_open (ws);
		    pss_set1 = pss_sets [work_set_no];
		    pss_set2 = pss_sets [work_set_no + 4];
		    sxba_copy (pss_set2, PSS_SET);

		    do {
			qtq = SS_get (ws, y);
			fill_pss_set (ARC, pss_set1, XxYxZ_X (ARC->QxTxQ_hd, qtq), -XxYxZ_Y (ARC->QxTxQ_hd, qtq), type, pss_set2);
			SS_push (ws, pss_set_to_pss_list (pss_set1));

			if ((loop = (++y < lim)))
			    sxba_copy (pss_set2, pss_set1);

		    } while (loop);
		}

/*		quad_hd_top = XH_top (quad_hd); */
		is_new_mess = false;
		
		/* Calcul des valeurs initiales */
		if (type < grand) {
		    /* type shift */
		    next = Q0xV_to_Q0 [type];
		    
		    if (conflict_derivation_kind == 0 && pss_kind == UNBOUNDED_ &&
			nucl_i (next, 2) == 0) {
			/* Un seul item shift, c'est obligatoirement le bon! */
			install_a_quad ((SXINT)0, (SXINT)0, LA, ORIG_ITEM = nucl_i (next, 1),
					XX_ARC_kind);
			is_new_mess = true;
		    }
		    else {
			n = 0;
			
			while ((ORIG_ITEM = nucl_i (next, ++n)) > 0) {
			    if (earley_parse (LA_init, LA_init + LA_length, &stack_hd,
					      ORIG_ITEM, ttrue))
				if (earley (ORIG_ITEM, germe, LA, LA_length, &stack_hd, 0))
				    is_new_mess = true;
			}
		    }
		}
		else {
		    red_no = type - grand;
		    n = 0;
		    
		    while ((ORIG_ITEM = orig_item_i (red_no, ++n)) > 0) {
			if (conflict_derivation_kind == 0) {
			    install_a_quad ((SXINT)0, (SXINT)0, LA, ORIG_ITEM, XX_ARC_kind);
			   is_new_mess = true; 
			}
			else {
			    if (earley (ORIG_ITEM, germe, LA, LA_length, &stack_hd, 0))
				is_new_mess = true;
			}
		    }
		}
		
		if (XX_is_pss_check)
		    SS_close (ws);

/* Test errone', un nouveau message a pu prendre la place d'un "erased"
   et ne pas augmenter XH_top (quad_hd).
		if (quad_hd_top == XH_top (quad_hd)) {
*/
		if (!is_new_mess) {
		    if (pss_kind == UNBOUNDED_)
			sxtrap (ME, "mm_process_one_LA");
		    /* pss_kind != UNBOUNDED_.
		       Pour le type courant, LA n'a pas ete confirme comme etant
		       un contexte droit correct en LALR => pas de message. */
		    break;
		}
	    }
	}
    }
}


void	sambig_conflict_messages (struct ARC_struct *XARC,
				  SXINT	qtq, 
				  SXINT sambig_conflict_messages_fork, 
				  SXINT type1, 
				  SXINT type2)
{
    /* On est dans le cas d'une ambiguite simple, pss_kind == UNBOUNDED_ */
    SXINT				la_lgth;
    struct ARC_ATTR	*attr;

    attr = XARC->attr + XxYxZ_Z (XARC->QxTxQ_hd, qtq);
    attr->conflict_kind = AMBIGUOUS_;
    messages_mngr_alloc ();
		
    if ((la_lgth = attr->lgth) == -1)
	la_lgth = k_value;
    
    FORK = sambig_conflict_messages_fork;
    TYPE1 = type1;
    TYPE2 = type2;
    SS_open (ws);
    SS_push (ws, la_lgth);
    ARC_walk_backward (XARC, qtq, bh_ARC_one_LA, bs_ARC_one_LA);
    SS_close (ws);
}

static bool is_in (SXINT path1, SXINT path2)
{
    /* Verifie que pathi est inclus ds pathj */
    /* Retourne true ssi path1 <= path2 */
    SXINT *bot1, *top1, *bot2, *top2;

    if (path1 == path2)
	return true;

    top1 = STACK_TOP (path1);
    bot1 = STACK_BOT (path1);
    top2 = STACK_TOP (path2);
    bot2 = STACK_BOT (path2);

    while (bot1 < top1 && bot2 < top2) {
	if (*bot1++ != *bot2++)
	    sxtrap (ME, "ARP_conflict_messages");
    }

    return bot1 == top1;
}


static bool fork_couple_not_processed (SXINT fork_couple_not_processed_fork1, SXINT fork2)
{
    /* Retourne true et stocke le couple ssi il n'existe pas deja dans lpss. */
    SXINT f1, f2, bot, top;

    if (fork_couple_not_processed_fork1 <= fork2) {
	f1 = fork_couple_not_processed_fork1;
	f2 = fork2;
    }
    else {
	f1 = fork2;
	f2 = fork_couple_not_processed_fork1;
    }

    for (top = SS_top (lpss), bot = SS_bot (lpss); bot < top; bot++) {
	if (f1 == SS_get (lpss, bot++) && f2 == SS_get (lpss, bot)) {
	    return false;
	}
    }

    SS_push (lpss, f1);
    SS_push (lpss, f2);
    return true;
}


void	ARP_conflict_messages (struct ARC_struct *XARC, 
			       SXINT prev_XARC_state, 
			       SXINT t, 
			       SXINT next_XARC_state, 
			       SXINT ARP_state,
			       bool is_ambig, 
			       SXBA ARP_conflict_messages_and_set)
{
    /* ARP_state est un etat de l'ARP non clonable.
       On prepare la sortie des messages denoncant les conflits. */
    SXINT	*p1, *ibot1, *p2, *ibot2;
    SXINT			*bot, *top, *itop1, *itop2;
    SXINT	x1, x2, ARP_conflict_messages_fork1, fork2;
    SXINT			pss, item1, item2, type1, type2, state1, state2,
                        n1, n2, is, wst1, wst2, prev_bfsa1, prev_bfsa2;
    SXINT			pb1_path, pb2_path;
    bool		der1_seen, der2_seen;
    
    bot = ARP_STATES_BOT (ARP_state);
    top = ARP_STATES_TOP (ARP_state);
    SS_open (lpss);

    for (p1 = bot; p1 < top; p1++) {
	/* On parcourt les types */
	if ((is = *p1++) < 0) {
	    /* Recherche d'un type ayant un item complet. */
	    type1 = *p1;
	    ibot1 = ARP_ITEMS_BOT (-is);
	    itop1 = ARP_ITEMS_TOP (-is);
	    der1_seen = false;
	
	    while (ibot1 < itop1) {
		if (XxY_Y (ARP_items, item1 = *ibot1++) == 0) {
		    n1 = XxY_X (ARP_items, item1);
		    SS_open (ws);
		    n_to_forks (XARC, next_XARC_state, n1, type1);
		
		    for (wst1 = SS_top (ws), x1 = SS_bot (ws); x1 < wst1; x1++) {
			ARP_conflict_messages_fork1 = SS_get (ws, x1);
			pss = XxY_Y (forks, ARP_conflict_messages_fork1);
		    
			if (!is_ambig || SXBA_bit_is_set (ARP_conflict_messages_and_set, pss)) {
			    for (p2 = bot; p2 < top; p2++) {
				is = labs (*p2);
				type2 = *++p2;

				if (type2 != type1) {
				    /* On parcourt les autres types */
				    ibot2 = ARP_ITEMS_BOT (is);
				    itop2 = ARP_ITEMS_TOP (is);
				    der2_seen = false;

				    while (ibot2 < itop2) {
					item2 = *ibot2++;
					n2 = XxY_X (ARP_items, item2);
					SS_open (ws);
					n_to_forks (XARC, next_XARC_state, n2, type2);
					
					for (wst2 = SS_top (ws), x2 = SS_bot (ws); x2 < wst2; x2++) {
					    fork2 = SS_get (ws, x2);
					    pss = XxY_Y (forks, fork2);
					    
					    if ((!is_ambig || SXBA_bit_is_set (ARP_conflict_messages_and_set, pss)) &&
						fork_couple_not_processed (ARP_conflict_messages_fork1, fork2)) {
						/* non encore traite */
						prev_bfsa1 =
						    fill_PREV_PSS_SET (XARC,
								       pss_sets [work_set_no],
								       prev_XARC_state, -t,
								       -type1, ARP_conflict_messages_fork1);
						state1 = ARP_xpath (ARP_state, type1, item1,
								    XxY_X (forks, ARP_conflict_messages_fork1),
								    prev_bfsa1);
						/* pb1_path est un chemin de prev_bfsa1 */
						XH_set (&stack_hd, &pb1_path);
						prev_bfsa2 =
						    fill_PREV_PSS_SET (XARC,
								       pss_sets [work_set_no + 4],
								       prev_XARC_state, -t,
								       -type2, fork2);
						state2 = ARP_xpath (ARP_state, type2, item2,
								    XxY_X (forks, fork2),
								    prev_bfsa2);
						/* pb2_path est un chemin de prev_bfsa2 */
						XH_set (&stack_hd, &pb2_path);
						/* Normalement pbi_path est un prefixe de
						   pbj_path */
						
						if (is_in (pb1_path, pb2_path)) {
						    /* 1 <= 2 */
						    /* On fabrique BS1 un chemin de l'etat final
						       de prev_bfsa2 a l'etat initial de
						       l'automate LR(0) */	
						    BS2 = pb2_path;
						    BS1 = install_a_LR0_path (state2);
						    PREV_PSS_SET = pss_sets [work_set_no + 4];
						    build_XARC_path (/* XARC, next_XARC_state, */
								     type2, fork2);
						    BS2 = pb1_path;
						    BS1 = cat_a_LR0_path (BS1, pb2_path, BS2);
						    PREV_PSS_SET = pss_sets [work_set_no];
						    build_XARC_path (/* XARC, next_XARC_state, */
								     type1, ARP_conflict_messages_fork1);
						}
						else {
						    /* 2 < 1 */
						    /* On fabrique BS1 un chemin de l'etat final
						       de prev_bfsa1 a l'etat initial de
						       l'automate LR(0) */	
						    BS2 = pb1_path;
						    BS1 = install_a_LR0_path (state1);
						    PREV_PSS_SET = pss_sets [work_set_no];
						    build_XARC_path (/* XARC, next_XARC_state, */
								     type1, ARP_conflict_messages_fork1);
						    BS2 = pb2_path;
						    BS1 = cat_a_LR0_path (BS1, pb1_path, BS2);
						    PREV_PSS_SET = pss_sets [work_set_no + 4];
						    build_XARC_path (/* XARC, next_XARC_state, */
								     type2, fork2);
						}

						if (is_unique_derivation_per_conflict &&
						    type2 < -grand) {
						    /* Dans le cas Shift, on regarde tous les items
						       pour eviter de manquer des cas shifts. */
						    der2_seen = true;
						    break;
						}
					    }
					}
					
					SS_close (ws);

					if (der2_seen)
					    break;
				    }
				}
			    }

			    if (is_unique_derivation_per_conflict && type1 < -grand) {
				/* Cas Reduce. */
				der1_seen = true;
				break;
			    }
			}
		    }

		    SS_close (ws);

		    if (der1_seen)
			break;
		}
	    }

	    if (der1_seen)
		break;
	}
    }

    SS_close (lpss);
}


void conflict_messages_output (SXBA conflict_t_set)
{
    /* Sortie de tous les messages accumules. */
    /* Extrait de quad_hd les messages correspondant a l'etat state apres les
       avoir tries par LA puis par type. */
    SXINT		conflict_messages_output_quad, nquad, orig_item, q, *bot;
    SXINT				lim, ARC_state, ARC_kind, ck, kind, from;
    struct ARC_struct	*ARC;
    bool		is_shift_reduce_conflict = false, is_eof;
    /* La1 != LA2 => state1 != state2 */

    sxinitialise(ARC_kind); /* pour faire taire gcc -Wuninitialized */
    sxinitialise(ARC); /* pour faire taire gcc -Wuninitialized */
    if ((lim = XH_top (quad_hd)) > 1) {
	/* Pour l'instant, suppression de la sortie de l'ER denotant les chaines
	   de look-ahead en conflit. */
	
	do {
	    ARC_state = 0;
	    
	    for (q = 1; q < lim; q++) {
		if (!XH_is_erased (quad_hd, q)) {
		    bot = QUAD_BOT (q);
		    
		    if (ARC_state == 0) {
			ARC_kind = *(bot + 4);
			ARC = &(ARCs [ARC_kind]);
			ARC_state = XxYxZ_Z (ARC->QxTxQ_hd, *(bot + 5));
		    }
		    
		    if (*(bot + 4) == ARC_kind &&
			XxYxZ_Z (ARC->QxTxQ_hd, *(bot + 5)) == ARC_state) {
			LA = *(bot + 2);
			is_eof = *(STACK_TOP (LA) - 1) == bnf_ag.WS_TBL_SIZE.tmax;
			ck = ARC->attr [ARC_state].conflict_kind;
			from = ARC_state;
			kind = ARC_kind;
			
			while ((from = ARCs [kind].attr [from].from) != 0) {
			    kind = ARCs [kind].ARC->kind;
			    ck |= ARCs [kind].attr [from].conflict_kind;
			}
			
			/* On commence par regarder si le conflit est Shift/Reduce ? */
			
			for (conflict_messages_output_quad = q; conflict_messages_output_quad < lim; conflict_messages_output_quad++) {
			    if (!XH_is_erased (quad_hd, conflict_messages_output_quad)) {
				bot = QUAD_BOT (conflict_messages_output_quad);
				
				if (*(bot + 2) == LA &&
				    *(bot + 4) == ARC_kind &&
				    XxYxZ_Z (ARC->QxTxQ_hd, *(bot + 5)) == ARC_state &&
				    bnf_ag.WS_INDPRO [*(bot + 3) /* orig_item */].lispro != 0) {
				    is_shift_reduce_conflict = true;
				    break;
				}
			    }
			}
			
			c_name = conflict_kind_to_string (vname, ck, ARC->h_value,
							  ARC->attr [ARC_state].lgth);
			
			print_title (is_shift_reduce_conflict ? "Shift" : "Reduce",
				     is_eof ? "" :
				     (ck == NOT_LALRk_ || ck == NOT_LRk_ || ck == NOT_RhLALRk_ || ck == NOT_RoxoLALRk_ || ck == NOT_RoxoLRk_) ? "" : "... ", c_name);
			
			for (conflict_messages_output_quad = q; conflict_messages_output_quad < lim; conflict_messages_output_quad++) {
			    if (!XH_is_erased (quad_hd, conflict_messages_output_quad)) {
				bot = QUAD_BOT (conflict_messages_output_quad);
				
				if (*(bot + 2) == LA &&
				    *(bot + 4) == ARC_kind &&
				    XxYxZ_Z (ARC->QxTxQ_hd, *(bot + 5)) == ARC_state) {
				    orig_item = *(bot + 3);
				    put_edit (3, bnf_ag.WS_INDPRO [orig_item].lispro == 0 ?
					      "- Reduce:" : "- Shift:");
				    soritem (orig_item, 3, 81, "", "");
				    
				    for (nquad = conflict_messages_output_quad; nquad < lim; nquad++) {
					if (!XH_is_erased (quad_hd, nquad)) {
					    bot = QUAD_BOT (nquad);
					    
					    if (*(bot + 2) == LA &&
						*(bot + 3) == orig_item &&
						*(bot + 4) == ARC_kind &&
						XxYxZ_Z (ARC->QxTxQ_hd, *(bot + 5)) == ARC_state) {
						if (conflict_derivation_kind != 0)
						  print_a_quad (is_eof, *bot, *(bot + 1)/* , *(bot + 2) */);
						
						XH_erase (quad_hd, nquad);
					    }
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	} while (ARC_state != 0);
	
	put_edit_nl (1);
	
	print_conclusions (germe, conflict_t_set);
	
	XH_clear (&quad_hd);
    }
    
    XH_clear (&stack_hd);
}

