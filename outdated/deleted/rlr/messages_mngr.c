/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1990 by Institut National de Recherche *
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
/* 04-12-92 13:15 (pb):		Le 6eme element des quads devient une	*/
/*				transition (qtq) et non pas l'etat final*/
/************************************************************************/
/* 01-12-92 11:30 (pb):	Bug ds "ecrire" si conflict_derivation_kind == 1*/
/************************************************************************/
/* 01-08-91 14:45 (pb):		Utilisation des XxYxZ			*/
/************************************************************************/
/* 18-05-90 10:05 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT_MESSAGES_MNGR[] = "@(#)messages_mngr.c	- SYNTAX [unix] - 4 D�cembre 1992";

static char	ME [] = "messages_mngr";

#include "P_tables.h"

#include "rlr_optim.h"

#include "SS.h"
#include "RLR.h"
#include "bstr.h"

extern char	*xt_to_str ();
extern char	*conflict_kind_to_string ();
extern bool	is_t_in_first ();
extern bool	ttrue ();
extern bool	earley ();
extern bool	is_pss_include ();
extern SXBA	fill_pss_set ();
extern bool	bh_ARC_one_LA ();
extern bool	bs_ARC_one_LA ();

static XH_header	duo_params_hd;
static int		duo_params_germe;

static int		LA_length;
static XH_header	stack_hd, quad_hd;

static SXBA	PSS_SET, PREV_PSS_SET;
static int	LA;
static int	BS2_BOT, BS2_TOP;
static int	BS1, BS2, FORK_GOAL, BFSA_GOAL, FORK_INTERM;
static int	DUO_CHARNIERE, ORIG_ITEM;
static int	*BOT_items, *TOP_items;

static struct bstr	*pre;
static VARSTR	vsuffixe, vname;
static int	*pre_stack, *post_stack;

static int	item_nb, item_nb_conflict;

static int	xkas, max_LALR_length, max_not_LALR_length;
static char	*c_name;
static bool	should_print_re;
static int	TYPE1, TYPE2, FORK;


static SXBA	LR0_statesN_set;
static int	LR0_statesN_set_size;

static struct bstr	*bstr_cat_Vs (bstr, Vs, is_bot_up)
    register struct bstr	*bstr;
    int	Vs;
    bool is_bot_up;
{
    /* Vs designe dans stack_hd une liste de symboles
       met dans vstr la chaine correspondante. */
    register int *bot, *top, *x, *lim, tnt;

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
	bstr_cat_strsp (bstr, (tnt = *x) > 0 ? get_nt_string (tnt) : xt_to_str (tnt));
	x += is_bot_up ? 1 : -1;
    }

    return bstr;
}

static struct bstr	*bstr_cat_pre_stack (bstr)
    register struct bstr	*bstr;
{
    /* pre_stack contient une pile de symboles terminaux ou non-terminaux
       Concatene de bas en haut, a bstr, la chaine de symboles correspondants */
    register int x, tnt, top;;

    for (top = SS_top (pre_stack), x = SS_bot (pre_stack); x < top; x++) {
	tnt = SS_get (pre_stack, x);
	bstr_cat_strsp (bstr, tnt > 0 ? get_nt_string (tnt) : xt_to_str (tnt));
    }
    
}

static struct bstr	*bstr_cat_post_stack (bstr)
    register struct bstr	*bstr;
{
    /* post_stack contient une pile de symboles terminaux
       Concatene de haut en bas, a bstr, la chaine de symboles correspondants */
    register int	x, bot;

    bot = SS_bot (post_stack);

    for (x = SS_top (post_stack) - 1 ; x >= bot; x--) {
	bstr_cat_strsp (bstr, xt_to_str (SS_get (post_stack, x)));
    }

    return bstr;
}



static void	rlr_write (red_no, item)
    int		red_no, item;
{
    /* item : A -> alpha . beta
       ecrit si red_no == 0
          =>* pre pre_stack post_stack suffixe
       sinon
          red_no => pre pre_stack beta post_stack suffixe
    */
    static char		der_star [] = "=>* ";
    static char		der [] = "=>  ";
    int		pre_lgth, pre_col, tnt;

    if (red_no != 0) {
	put_edit_fnb (11, 4, red_no);
	put_edit_nnl (16, der);
    }
    else
	put_edit_nnl (16, der_star);

    bstr_get_attr (pre, &pre_lgth, &pre_col);
    bstr_cat_pre_stack (pre);

    if (item != 0) {
	while ((tnt = bnf_ag.WS_INDPRO [item++].lispro) != 0)
	    bstr_cat_strsp (pre, tnt > 0 ? get_nt_string (tnt) : xt_to_str (tnt));
    }

    bstr_cat_post_stack (pre);
    bstr_cat_str (pre, varstr_tostr (vsuffixe));
    put_edit_apnnl (bstr_tostr (pre));
    bstr_shrink (pre, pre_lgth, pre_col);
}



static int items_index (cur_index, top_index, nb)
    int	cur_index, top_index, *nb;
{
    /* cur_index repere dans stack_hd une liste d'items. */
    /* retourne le pointeur correspondant a l'exploitation de l'item_courant */
    register int	item, init_item, tnt, tail_item;

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




static void compute_items_np (cur_index, top_index, write_index, is_shift)
    register int	cur_index;
    int			top_index, write_index;
    bool		is_shift;
{
    register int	cur_item, new_index, tnt, items_nb = 0;
    int			nb, old_nb, item, init_item, total_nb = 0;
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



static bool is_conflict_prod_reached (bot_items)
    int **bot_items;
{
    /* bot_items et TOP_items sont des pointeurs dans stack_hd dans une liste d'items. */
    register int	item, init_item;
    
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


static bool ecrire (should_write)
    bool	should_write;
{
    /* BOT_items et TOP_items sont des pointeurs dans stack_hd dans une liste d'items. */
    register int	x, item, tnt, nb;
    int			red_no, *ptr;
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
	rlr_write (red_no, item);

    if ((tnt = bnf_ag.WS_INDPRO [item].lispro) != 0) {
	nb++;
	SS_push (pre_stack, tnt);

	if (should_write && bnf_ag.WS_INDPRO [item + 1].lispro != 0) {
	    varstr_catenate (vsuffixe, ".");
	    rlr_write (0, 0);
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
		rlr_write (0, 0);

	    if (done)
		return true;
	}
	else
	    SS_push (post_stack, tnt);
    }

    return BOT_items == TOP_items;
}


static int	install_a_LR0_path (); /* Plus loin */


static void	print_a_quad (is_eof, Vs, items, Ts/*, delta_items*/)
    bool	is_eof;
    int Vs, items, Ts/*, delta_items*/;
{
    /* Vs, items et Ts sont des identifiants de stack_hd
       Vs (de bas en haut) est une liste de symbole designant un prefixe
       items est une liste d'items LR(0), s'exploite de bas en haut
       Ts (de bas en haut) est une liste de T designant une chaine
          de look_ahead. */
    int	lhs;
    char	*eof;

    put_edit_nnl (10, conflict_derivation_kind == 2 ? "detailed " : "");
    put_edit_ap ("sample derivation:");
    put_edit_nnl (13, get_nt_string (1 /* axiome */ ));
    put_edit_apnnl (" ");
    put_edit_ap (eof = xt_to_str (bnf_ag.WS_TBL_SIZE.tmax) /* "END OF FILE" */ );
    bstr_raz (pre);
    SS_clear_ss (pre_stack);
    SS_clear_ss (post_stack);

    if (Vs != 0)
	pre = bstr_cat_Vs (pre, Vs, false);

    varstr_raz (vsuffixe);

    if (is_eof) {
	varstr_catenate (vsuffixe, eof);
    }
    else {
	varstr_catenate (vsuffixe, "...");
	lhs = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [*STACK_BOT (items)].prolis].reduc;

	if (lhs == 0)
	    lhs = 1; /* axiome */

/*	if (conflict_derivation_kind == 2 || delta_items == 0) { */
	    SS_push (pre_stack, lhs);
	    rlr_write (0, 0);
	    SS_pop (pre_stack);
/*	} */
    }

    BOT_items = STACK_BOT (items);
    TOP_items = STACK_TOP (items);

    ecrire (true);
    put_edit_nl (1);
}


int get_quad_nb ()
{
    register int	q, nb = 0;
    int			lim;

    if (quad_hd.display == NULL)
	return 0;

    for (lim = XH_top (quad_hd), q = 1; q < lim; q++) {
	if (!XH_is_erased (quad_hd, q))
	    nb++;
    }
    
    return nb;
}

bool erase_quads (ARC_kind, qtq)
    int	ARC_kind, qtq;
{
    /* Supprime les quad de ARC dont la derniere transition est qtq. */
    register int x, bot;
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

static int install_a_quad (p0, p1, p2, p3, p4)
    int	p0, p1, p2, p3, p4;
{
    int	indice;

    XH_push (quad_hd, p0); /* prefixe viable */
    XH_push (quad_hd, p1); /* items_list */
    XH_push (quad_hd, p2); /* LookAhead list */
    XH_push (quad_hd, p3); /* item d'origine du conflit */
    XH_push (quad_hd, p4); /* [X]ARC_kind */
    XH_push (quad_hd, QTQ); /* Transition ds l'[X]ARC conduisant au conflit */
    XH_set (&quad_hd, &indice);
    return indice;
}

bool make_a_quad (etat_charniere, item_charniere, items_stack)
    int	etat_charniere, item_charniere, *items_stack;
{
    /* item_charniere : A -> X1  Xp . Xp+1  Xn est dans etat_charniere
       Le sommet de items_stack repere un item de la meme regle (>=) */
    register int	*bot, *top;
    int			r1, r2, bs1_bot, bs1_top;

    if (conflict_derivation_kind == 0) {
	install_a_quad (0, 0, LA, ORIG_ITEM, XX_ARC_kind);
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

static int fill_PREV_PSS_SET (XARC, prev_pss_set, XARC_state, t, type, fork)
    struct ARC_struct	*XARC;
    register SXBA	prev_pss_set;
    int XARC_state, t, type, fork;
{
    register int	xfks1, xfks, xpis;
    int			lim1, lim2, lim3, xpfks, xpfks1, cur_fork, cur_pss, cur_type,
                        prev_bfsa, indice, fks;

    sxba_empty (prev_pss_set);

    /* On cherche pour "type" dans l'etat XARC_state de l'XARC l'ensemble des forks
       (dont on extrait les pss) dont la transition sur t contient "fork". */
    /* Retourne le bfsa de l'un de ces fork. C'est un prefixe du bfsa de "fork". */
    for (lim1 = XH_X (XARC->Q_hd, XARC_state + 1), xpis = XH_X (XARC->Q_hd, XARC_state); xpis <
	 lim1; xpis++) {
	if ((cur_type = -XH_list_elem (XARC->Q_hd, ++xpis)) == type ||
	    cur_type < grand && type < grand /* shift (etat 1) */) {
	    xpfks = XH_list_elem (XARC->Q_hd, xpis - 1);
	    
	    for (lim2 = XH_X (fks_hd, xpfks + 1), xfks = XH_X (fks_hd, xpfks); xfks
		 < lim2; xfks++) {
		cur_fork = XH_list_elem (fks_hd, xfks);
		
		if ((indice = XxY_is_set (&FORKxT_hd, cur_fork, t)) != 0) {
		    /* Il y a une transition terminale sur t */
		    xpfks1 = FORKxT_to_fks [indice];
		    
		    for (lim3 = XH_X (fks_hd, xpfks1 + 1), xfks1 = XH_X (fks_hd, xpfks1); xfks1
			 < lim3; xfks1++) {
			if (XH_list_elem (fks_hd, xfks1) == fork) {
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


static bool call_earley (fork)
    int		fork;
{
    int	indice, LA_init;
    bool	ret_val;

    if (duo_params_hd.display == NULL) {
	XH_alloc (&duo_params_hd, "duo_params_hd", xac2, 4, 2, NULL, statistics_file);
	duo_params_germe = germe;
    }
    else if (duo_params_germe != germe) {
	XH_clear (&duo_params_hd);
	duo_params_germe = germe;
    }

    XH_push (duo_params_hd, fork);
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


static void build_XARC_path (XARC, XARC_state, type, fork)
    struct ARC_struct	*XARC;
    int	XARC_state, type, fork;
{
    /* On est dans le cas XARC et UNBOUNDED_. */
    /* LA contient un chemin t1 t2 ... tn tel que
       q0 ->t1 q1 ->t2 q2 .... ->tn qn de XARC
       et qn == XARC_state est en conflit.
       qn-1 == prev_XARC_state
       fork = (bfsa, pss) de type "type" dans qn y participe
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

    register int	n, next, red_no;
    bool		has_pb = true;

    /* Calcul des valeurs initiales */
    if (-type < grand) {
	/* type shift */
	next = Q0xV_to_Q0 [-type];

	if (conflict_derivation_kind == 0 && nucl_i (next, 2) == 0) {
	    /* Un seul item shift, c'est obligatoirement le bon! */
	    install_a_quad (0, 0, LA, ORIG_ITEM = nucl_i (next, 1), XX_ARC_kind);
	    has_pb = false;
	}
	else {
	    n = 0;

	    while ((ORIG_ITEM = nucl_i (next, ++n)) > 0) {
		if (call_earley (fork))
		    has_pb = false;
	    }
	}
    }
    else {
	red_no = -type - grand;
	n = 0;

	while ((ORIG_ITEM = orig_item_i (red_no, ++n)) > 0) {
	    if (conflict_derivation_kind == 0) {
		install_a_quad (0, 0, LA, ORIG_ITEM, XX_ARC_kind);
		has_pb = false;
	    }
	    else
		if (call_earley (fork))
		    has_pb = false;
	}
    }

    if (has_pb)
	/* Pour le type courant, LA n'a pas ete confirme par earley comme etant
	   un contexte droit correct. Pb... */
	sxtrap (ME, "build_XARC_path");
}


static int	get_a_pred_state (state)
    int		state;
{
    /* State est un etat non initial de l'automate LR (0).
       Retourne le plus petit predecesseur de state. */ 
    register int	x, y, pred = state;

    XxY_Yforeach (Q0xQ0_hd, state, x)
	if ((y = XxY_X (Q0xQ0_hd, x)) < pred)
	    pred = y;

    return pred;
}



static bool conformity_parse_stack_pss (parse_stack, pss)
    int *parse_stack, pss;
{
    /* On regarde si parse_stack et pss sont "compatibles".
       parse_stack est une SS_stack.
       pss est de type qcq. */
    register int	*pss_bot, *ps_bot;
    int			*pss_top, *ps_top;

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
	int		first, last, couple;
	register int	*x;

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

static bool conformity_LALR (parse_stack)
    int	*parse_stack;
{
    /* On verifie que parse_stack est un chemin
       dans un pss de PREV_PSS_SET. */
    register int pss = 0;

    while ((pss = sxba_scan (PREV_PSS_SET, pss)) > 0) {
	if (conformity_parse_stack_pss (parse_stack, pss)) {
	    return true;
	}
    }

    return false;
}


bool check_real_conformity (parse_stack)
    int	*parse_stack;
{
    /* Verifie que parse_stack est conforme a [l'un des] pss|fork de PREV_PSS_SET
       responsable du conflit. */
    if (PREV_PSS_SET == NULL)
	/* Pas de verification si inutile */
	return true;
    
    return conformity_LALR (parse_stack);
}



static void	print_title (shift_or_reduce, suffixe, conflict_name)
    char	*shift_or_reduce, *suffixe, *conflict_name;
{
    char	str [8];

    put_edit_nnl (1, conflict_name);
    bstr_cat_str (bstr_raz (pre), shift_or_reduce);
    bstr_cat_str (pre, "-Reduce conflict in state #");
    sprintf (str, "%d ", germe);
    bstr_cat_str (pre, str);
    bstr_cat_str (pre, "on ");
    bstr_cat_Vs (pre, LA, true);
    bstr_cat_str (pre, suffixe);
    bstr_cat_str (pre, "between:");
    put_edit_ap (bstr_tostr (pre));
}


static int	ARP_xprefixe (ARP_state, type, cn, bfsa, nmax)
    int		ARP_state, type, cn, bfsa, nmax;
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
    register int	x, y, item, nnm1;
    int			qN, qnm1, pred, is, bfsa_bot;
    register int	*bot, *top, *b1, *t1;

    if (ARP_state == 1) {
	if ((qN = XxY_Y (ARP_items, cn)) > xac2)
	    qN = XxY_X (Q0xN_hd, qN - xac2);

	return qN;
    }


/* On recherche le plus petit predecesseur de ARP_state */

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

    while (bot < top) {
	/* On parcourt les types */
	if (type == *(bot + 1)) {
	    is = abs (*bot);
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


static bool	ARP_knot_xpath (entree, sortie, kbot, ktop)
    int		sortie, entree;
    register int	*ktop, *kbot;
{
    /* Met dans stack_hd un chemin de knot entre entree (exclus) et
       sortie (inclus) */
    int			*bot;
    register int	p, q, couple, next;
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



static void	ARP_call_knot_xpath (entree, sortie, knot)
    int		sortie, entree, knot;
{
    sxba_empty (LR0_statesN_set);
    SXBA_1_bit (LR0_statesN_set, entree);

    if (!ARP_knot_xpath (entree, sortie, BFSA_BOT (knot), BFSA_TOP (knot)))
	sxtrap (ME, "ARP_call_knot_path");
}



static int	ARP_xpath (ARP_state, type, item, bfsa1, prev_bfsa1)
    int		ARP_state, type, item, bfsa1, prev_bfsa1;
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
    register int	n, state, knot, in;
    register int	*bot;
    int			out, prev_item, prev_state, nmax;

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


static int	install_a_LR0_path (final)
    int	final;
{
    /* Met dans stack_hd un chemin de l'automate LR (0) entre final et
       l'etat initial */
    /* Retourne l'identifiant unique de ce chemin. */
    /* Tout etat s (sauf l'etat initial) a un predecesseur s' tel que s' < s. */
    int		result;

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



static int	cat_a_LR0_path (bs1, bs2, bs_2)
    int		bs1, bs2, bs_2;
{
    /* Met dans stack_hd le chemin de l'automate LR (0) :
       (bs2 - bs_2) || bs1 */
    /* Retourne l'identifiant unique de ce chemin. */
    register int	bs_bot, bs_top, state;
    int		result;

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




void messages_mngr_alloc ()
{
    register int l;

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


void messages_mngr_free ()
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


static bool one_ambig_LA (XARC, prev_XARC_state, t, XARC_state)
    struct ARC_struct	*XARC;
    int prev_XARC_state, t, XARC_state;
{
    int state, prev_bfsa;

    prev_bfsa = fill_PREV_PSS_SET (XARC, PREV_PSS_SET = pss_sets [work_set_no], prev_XARC_state, -t, -TYPE1, FORK);
    state = ARP_xpath (0, 0, 0, XxY_X (forks, FORK), prev_bfsa);
    XH_set (&stack_hd, &BS2);
    BS1 = install_a_LR0_path (state);
    build_XARC_path (XARC, XARC_state, TYPE1, FORK);
    fill_PREV_PSS_SET (XARC, PREV_PSS_SET, prev_XARC_state, -t, -TYPE2, FORK);
    build_XARC_path (XARC, XARC_state, TYPE2, FORK);
    return is_unique_conflict_per_state;
}


void mm_process_one_LA (ARC)
    struct ARC_struct	*ARC;
{
    /* Un (nouveau) LA vient d'etre trouve. */
    /* LA est global */
    register int	x;
    int			qtq, t, prev_ARC_state, next_ARC_state, type_nb, red_no,
                        /* quad_hd_top, */conflict_kind, LA_init, lim, n, next, type;
    bool		is_new_mess;

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
	    one_ambig_LA (ARC, prev_ARC_state, t, next_ARC_state);
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
		    register int	y;
		    register SXBA	pss_set1, pss_set2;
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

			if (loop = (++y < lim))
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
			install_a_quad (0, 0, LA, ORIG_ITEM = nucl_i (next, 1),
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
			    install_a_quad (0, 0, LA, ORIG_ITEM, XX_ARC_kind);
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


void	sambig_conflict_messages (XARC, qtq, fork, type1, type2)
    struct ARC_struct	*XARC;
    int			qtq, fork, type1, type2;
{
    /* On est dans le cas d'une ambiguite simple, pss_kind == UNBOUNDED_ */
    int				la_lgth;
    register struct ARC_ATTR	*attr;

    attr = XARC->attr + XxYxZ_Z (XARC->QxTxQ_hd, qtq);
    attr->conflict_kind = AMBIGUOUS_;
    messages_mngr_alloc ();
		
    if ((la_lgth = attr->lgth) == -1)
	la_lgth = k_value;
    
    FORK = fork;
    TYPE1 = type1;
    TYPE2 = type2;
    SS_open (ws);
    SS_push (ws, la_lgth);
    ARC_walk_backward (XARC, qtq, bh_ARC_one_LA, bs_ARC_one_LA);
    SS_close (ws);
}

static bool is_in (path1, path2)
    int path1, path2;
{
    /* Verifie que pathi est inclus ds pathj */
    /* Retourne true ssi path1 <= path2 */
    register int *bot1, *top1, *bot2, *top2;

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


static bool fork_couple_not_processed (fork1, fork2)
    int fork1, fork2;
{
    /* Retourne true et stocke le couple ssi il n'existe pas deja dans lpss. */
    register int f1, f2, bot, top;

    if (fork1 <= fork2) {
	f1 = fork1;
	f2 = fork2;
    }
    else {
	f1 = fork2;
	f2 = fork1;
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


void	ARP_conflict_messages (XARC, prev_XARC_state, t, next_XARC_state, ARP_state,
			       is_ambig, and_set)
    struct ARC_struct	*XARC;
    int			prev_XARC_state, t, next_XARC_state, ARP_state;
    SXBA		and_set;
    bool		is_ambig;
{
    /* ARP_state est un etat de l'ARP non clonable.
       On prepare la sortie des messages denoncant les conflits. */
    register int	*p1, *ibot1, *p2, *ibot2;
    int			*bot, *top, *itop1, *itop2;
    register int	x1, x2, fork1, fork2;
    int			pss, item1, item2, type1, type2, state1, state2,
                        n1, n2, is, wst1, wst2, y, prev_bfsa1, prev_bfsa2;
    int			pb1_path, pb2_path;
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
			fork1 = SS_get (ws, x1);
			pss = XxY_Y (forks, fork1);
		    
			if (!is_ambig || SXBA_bit_is_set (and_set, pss)) {
			    for (p2 = bot; p2 < top; p2++) {
				is = abs (*p2);
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
					    
					    if ((!is_ambig || SXBA_bit_is_set (and_set, pss)) &&
						fork_couple_not_processed (fork1, fork2)) {
						/* non encore traite */
						prev_bfsa1 =
						    fill_PREV_PSS_SET (XARC,
								       pss_sets [work_set_no],
								       prev_XARC_state, -t,
								       -type1, fork1);
						state1 = ARP_xpath (ARP_state, type1, item1,
								    XxY_X (forks, fork1),
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
						    build_XARC_path (XARC, next_XARC_state,
								     type2, fork2);
						    BS2 = pb1_path;
						    BS1 = cat_a_LR0_path (BS1, pb2_path, BS2);
						    PREV_PSS_SET = pss_sets [work_set_no];
						    build_XARC_path (XARC, next_XARC_state,
								     type1, fork1);
						}
						else {
						    /* 2 < 1 */
						    /* On fabrique BS1 un chemin de l'etat final
						       de prev_bfsa1 a l'etat initial de
						       l'automate LR(0) */	
						    BS2 = pb1_path;
						    BS1 = install_a_LR0_path (state1);
						    PREV_PSS_SET = pss_sets [work_set_no];
						    build_XARC_path (XARC, next_XARC_state,
								     type1, fork1);
						    BS2 = pb2_path;
						    BS1 = cat_a_LR0_path (BS1, pb1_path, BS2);
						    PREV_PSS_SET = pss_sets [work_set_no + 4];
						    build_XARC_path (XARC, next_XARC_state,
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


void conflict_messages_output (conflict_t_set)
    SXBA conflict_t_set;
{
    /* Sortie de tous les messages accumules. */
    /* Extrait de quad_hd les messages correspondant a l'etat state apres les
       avoir tries par LA puis par type. */
    register int		quad, nquad, orig_item, q, *bot;
    int				lim, indice, ARC_state, ARC_kind, ck, kind, from;
    register struct ARC_struct	*ARC;
    bool		is_shift_reduce_conflict = false, is_eof;
    /* La1 != LA2 => state1 != state2 */
    
    if ((lim = XH_top (quad_hd)) > 1) {
	put_edit_nl (1);
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
			
			for (quad = q; quad < lim; quad++) {
			    if (!XH_is_erased (quad_hd, quad)) {
				bot = QUAD_BOT (quad);
				
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
			
			for (quad = q; quad < lim; quad++) {
			    if (!XH_is_erased (quad_hd, quad)) {
				bot = QUAD_BOT (quad);
				
				if (*(bot + 2) == LA &&
				    *(bot + 4) == ARC_kind &&
				    XxYxZ_Z (ARC->QxTxQ_hd, *(bot + 5)) == ARC_state) {
				    orig_item = *(bot + 3);
				    put_edit (3, bnf_ag.WS_INDPRO [orig_item].lispro == 0 ?
					      "- Reduce:" : "- Shift:");
				    soritem (orig_item, 3, 81, "", "");
				    
				    for (nquad = quad; nquad < lim; nquad++) {
					if (!XH_is_erased (quad_hd, nquad)) {
					    bot = QUAD_BOT (nquad);
					    
					    if (*(bot + 2) == LA &&
						*(bot + 3) == orig_item &&
						*(bot + 4) == ARC_kind &&
						XxYxZ_Z (ARC->QxTxQ_hd, *(bot + 5)) == ARC_state) {
						if (conflict_derivation_kind != 0)
						    print_a_quad (is_eof, *bot, *(bot + 1), *(bot + 2));
						
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

