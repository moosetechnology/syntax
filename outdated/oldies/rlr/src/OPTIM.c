/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                        (PB)                          *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* Mar 26 Nov 1996 18:26 (PB):	Adaptation a LINUX			*/
/************************************************************************/
/* 19-08-94 14:32 (pb):		Les "codes" des sequences non		*/
/*				deterministes sont superieures a 20000	*/
/*				(et non plus a 10000)			*/
/************************************************************************/
/* 31-03-93 10:50 (pb):		Ajout de prdct_list dans le cas non	*/
/*				deterministe et predicats		*/
/************************************************************************/
/* 22-03-93 16:25 (pb):		Modif de germe_to_gr_act dans le cas du	*/
/*				non determinisme			*/
/************************************************************************/
/* 01-03-93 14:00 (pb):		Ajout de nd_degree (non determinisme?)	*/
/************************************************************************/
/* 19-02-93 10:30 (pb):		Suppression des branchements en pile	*/
/*				pour le rattrapage global et 		*/
/*				reorganisation des ref = (etat, etq)	*/
/*				Suppression de la table gotos		*/
/************************************************************************/
/* 19-01-93 11:35 (pb):		Bug ds l'appel de "gen_branch" sur les	*/
/*				predicats associes aux non-terminaux.	*/
/************************************************************************/
/* 18-01-93 15:05 (pb):		Bug ds "gen_prdct_seq", le cas erreur	*/
/*				n'etait pas genere.			*/
/************************************************************************/
/* 13-10-92 15:52 (pb):		Inhibition partielle des spe ds le cas	*/
/*				du non determinisme (cf comment local).	*/
/************************************************************************/
/* 14-04-92 12:20 (pb):		Non execution des predicats sur reduce	*/
/*				quand ce n'est pas necessaire.		*/
/************************************************************************/
/* 13-03-89 10:52 (pb):		Adaptation au RLR			*/
/************************************************************************/
/* 24-08-88 18:53 (bl):		Ajout de #ifdef MSDOS			*/
/************************************************************************/
/* 04-05-88 09:40 (pb):		RAZ de tables statiques pour appels	*/
/*				multiples				*/
/************************************************************************/
/* 22-02-88 08:41 (pb):		Adaptation a la V3.2 (xnt)		*/
/************************************************************************/
/* 16-10-87 10:50 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)OPTIM.c - SYNTAX [unix] - Mardi 26 Novembre 1996";

static char	ME [] = "OPTIM";

#include "P_tables.h"

#include "rlr_optim.h"


extern int	equality_sort ();
extern int	put_in_fe ();
extern VARSTR	cat_t_string ();
extern void	parser_write (), parser_free ();
extern void	sxtime ();
struct xt_to_act {
	   int	action, lnk;
       };
static struct row {
	   int	hash, lgth, start;
       }	*rows;
static struct P_item	*cols, *ITEMS;
static struct parser_ag_item	parser_ag;
static int	*tnt_fe_to_new, *lnks;
static int	hashs [HASH_SIZE];

static int	t_items_size, nt_items_size, tnt_fe_size;
static int	prdct_list_size;


static bool	row_le (i, j)
    int		i, j;
{
    register int	li, lj, vali, valj;
    register struct row		*arowi = rows + i, *arowj = rows + j;
    register struct P_item	*ai, *aj;

    if ((vali = arowi->hash) != (valj = arowj->hash))
	return vali < valj;

    if ((li = arowi->lgth) != (lj = arowj->lgth))
	return li < lj;

    for (ai = ITEMS + arowi->start, aj = ITEMS + arowj->start; li > 0; ai++, aj++, li--) {
	while ((vali = ai->action) == 0)
	    ai++;

	while ((valj = aj->action) == 0)
	    aj++;

	if (vali != valj)
	    return vali < valj;

	if ((vali = ai->check) != (valj = aj->check))
	    return vali < valj;
    }

    return true;
}



static bool	row_eq (i, j)
    int		i, j;
{
    register int	li, vali, valj;
    register struct row		*arowi = rows + i, *arowj = rows + j;
    register struct P_item	*ai, *aj;

    if (arowi->hash != arowj->hash)
	return false;

    if ((li = arowi->lgth) != arowj->lgth)
	return false;

    for (ai = ITEMS + arowi->start, aj = ITEMS + arowj->start; li > 0; ai++, aj++, li--) {
	while ((vali = ai->action) == 0)
	    ai++;

	while ((valj = aj->action) == 0)
	    aj++;

	if (vali != valj)
	    return false;

	if (ai->check != aj->check)
	    return false;
    }

    return true;
}



static int	put_in_reductions (hashs, lnks, afe)
    register struct floyd_evans		*afe;
    int		*hashs, *lnks;
{
    register int	hash, x, y;
    register struct P_reduction		*ared;

    hash = (afe->reduce + afe->pspl + afe->next) % HASH_SIZE;

    for (y = 0, x = hashs [hash]; x != 0; y = x, x = lnks [x]) {
	ared = reductions + x;

	if (ared->reduce == afe->reduce && ared->lgth == afe->pspl && ared->lhs == afe->next)
	    return x;
    }

    ared = reductions + (x = ++*lnks /* nouvelle taille de reductions */ );
    ared->reduce = afe->reduce;
    ared->lgth = afe->pspl;
    ared->lhs = afe->next;
    lnks [x] = 0;

    if (y == 0)
	hashs [hash] = x;
    else
	lnks [y] = x;

    return x;
}



static bool	is_0_indistinguable (afei, afej)
    register struct floyd_evans		*afei, *afej;
{
    /* Regarde si les productions de floyd_evans parametres sont 0_indistinguables */

    register int	i, lgth;
    register struct P_item	*ai, *aj;

    if (afei->codop != afej->codop)
	return false;

    switch (afei->codop) {
    case Shift:
    case ScanShift:
	if (afei->pspl == 0 && afej->pspl != 0 || afei->pspl != 0 && afej->pspl == 0)
	    return false;

	break;

    case Reduce:
    case ScanReduce:
	if (afei->next != afej->next || afei->reduce != afej->reduce || afei->pspl != afej->pspl)
	    return false;

	break;

    case LaScanShift:
	break;

    case Error:
	if (afei->next != afej->next)
	    return false;

	break;

    case Prdct:
    case NonDeter:
    default:
	sxtrap (ME, "is_0_indistinguable");
    }

    return true;
}



static bool	is_k_indistinguable (afei, afej)
    register struct floyd_evans		*afei, *afej;
{
    /* Regarde (dans le cas [Scan]Shift) si les etats references par les productions de
       floyd_evans parametres sont k_indistinguables; *afei et *afej sont deja supposes
       etre k-1_indistinguables */

    register int	i, lgth;
    register struct P_item	*ai, *aj;

    switch (afei->codop) {
    case Shift:
    case ScanShift:
	return t_state_equiv_class [afei->next] == t_state_equiv_class [afej->next] && nt_state_equiv_class [afei->next]
	     == nt_state_equiv_class [afej->next];

    case LaScanShift:
	return t_state_equiv_class [afei->next] == t_state_equiv_class [afej->next];

    default:
	return true;
    }
}



static void	zero_indistinguable (state_no, equiv_class, states, items, fe, partitions, eq_lnks, partition_no)
    int		state_no;
    register int	*equiv_class;
    struct state	*states;
    struct P_item	*items;
    struct floyd_evans	*fe;
    int		*partitions, *eq_lnks, *partition_no;
{
    register int	i, j, k, l;
    register struct state	*astatei, *astatej;
    register struct P_item	*aitemi, *aitemj;
    int		prev;

    for (i = 1; i < state_no; i++) {
	if ((l = (astatei = states + i)->lgth) != 0 && equiv_class [i] == i) {
	    (*partition_no)++;
	    prev = i;
	    eq_lnks [i] = 0 /* tail */ ;

	    for (j = i + 1; j < state_no; j++) {
		if ((astatej = states + j)->lgth == l && equiv_class [j] == j) {
		    for (k = 0, aitemi = items + astatei->start, aitemj = items + astatej->start; k < l; k++, aitemi++,
			 aitemj++) {
			if (aitemi->check != aitemj->check)
			    break;

			if (!is_0_indistinguable (fe + aitemi->action, fe + aitemj->action))
			    break;
		    }

		    if (k == l) {
			equiv_class [j] = i;
			eq_lnks [prev] = j;
			eq_lnks [j] = 0 /* tail */ ;
			prev = j;
		    }
		}
	    }

	    if (prev != i) {
		/* classe d'equivalence multiple, on la stocke dans partitions */
		partitions [++*partitions] = i;
	    }
	}
    }
}



static bool	is_stable (equiv_class, states, items, fe, partitions, eq_lnks, partition_no)
    register int	*equiv_class;
    struct state	*states;
    struct P_item	*items;
    struct floyd_evans	*fe;
    int		*partitions, *eq_lnks, *partition_no;
{
    register int	x, i, j, l;
    register struct state	*astatei;
    register struct P_item	*aitemi, *aitemj;
    int		prev, new_part;
    bool	is_a_new_partition = false;

    for (x = 1; x <= *partitions; x++) {
	astatei = states + (i = partitions [x]);
	aitemi = items + astatei->start;
	eq_lnks [0] = 0;
	new_part = 0;

	for (j = eq_lnks [i]; j != 0; j = eq_lnks [i]) {
	    /* On regarde si les etats i et j sont toujours indistinguables */
	    aitemj = items + states [j].start;

	    for (l = astatei->lgth - 1; l >= 0; l--) {
		if (!is_k_indistinguable (fe + aitemi [l].action, fe + aitemj [l].action)) {
		    eq_lnks [i] = eq_lnks [j];
		    eq_lnks [j] = 0;
		    eq_lnks [new_part] = j;
		    new_part = j;
		    equiv_class [j] = eq_lnks [0];
		    /* nouveau representant */
		    break;
		}
	    }

	    if (l < 0)
		i = j;
	}

	if ((new_part = eq_lnks [0]) != 0) {
	    /* une nouvelle partition a ete creee */
	    (*partition_no)++;

	    if (eq_lnks [new_part] != 0)
		/* Cette partition a au moins deux elements, on la stocke */
		if (eq_lnks [partitions [x]] == 0)
		    /* la partition x n'a plus qu'un element, on reutilise la place
		       dans partitions et on l'examine immediatement */
		    partitions [x--] = new_part;
		else
		    partitions [++*partitions] = new_part;

	    is_a_new_partition = true;
	}
    }

    return !is_a_new_partition;
}



static void	insert_xnt_lnk (xnt, xnt_to_act)
    register int	xnt;
    register struct xt_to_act	*xnt_to_act;
{
    register int	prev, next, prdct;

    for (next = xnt_to_act [prev = XNT_TO_NT_CODE (xnt)].lnk, prdct = XNT_TO_PRDCT_CODE (xnt); next != 0; prev = next,
	 next = xnt_to_act [next].lnk) {
	if (XNT_TO_PRDCT_CODE (next) < prdct)
	    break;
    }

    xnt_to_act [xnt].lnk = next;
    xnt_to_act [prev].lnk = xnt;
}



void	fe_to_equiv (afe1, afe2)
    register struct floyd_evans		*afe1;
    struct floyd_evans	*afe2;
{
    register int	k;

    *afe1 = *afe2;

    if (afe1->codop == Shift || afe1->codop == ScanShift) {
	if ((k = nt_state_equiv_class [afe1->pspl]) > 0)
	    /* On ne touche pas aux adresses de branchement directes */
	    afe1->pspl = k;

	afe1->next = t_state_equiv_class [afe1->next];
    }
}

static int gen_nd_seq (hd, links, t_items, fe)
    int			hd, *links;
    struct P_item	*t_items;
    struct floyd_evans	*fe;
{
    /* genere le non_determinisme.
       retourne le floyd-evans permettant d'y acceder. */
    /* On genere en premier les reduce => degree le plus grand. */
    register int		x = hd, y, degree = 20000;
    register int		*pact;
    register struct floyd_evans	*afe;

    do {
	pact = &(t_items [x].action);

	if ((afe = fe + *pact)->codop != Reduce) {
	    build_branch (degree, afe, 0);
	    *pact = 0;
	    degree++;
	}

	x = links [x];
    } while (x != 0);

    x = hd;

    do {
	if (*(pact = &(t_items [x].action)) != 0) {
	    build_branch (degree, fe + *pact, 0);
	    *pact = 0;
	    degree++;
	}

	x = links [y = x];
	links [y] = 0;
    } while (x != 0);

    return gen_tree (NonDeter, 0);
}

static int gen_prdct_seq (hd, links, t_items, fe)
    int			hd, *links;
    struct P_item	*t_items;
    struct floyd_evans	*fe;
{
    /* genere la sequence de predicats definie ds t_items par
       les liens links de tete hd.
       Attention on peut avoir du non_determinisme.
       retourne le floyd-evans permettant d'y acceder. */
    register int		x, y, z, xt;
    register struct P_item	*aitem;


    /* On traite d'abord le non_determinisme.
       S'il y en a, les elements sont consecutifs. */

    x = hd;

    do {
	aitem = t_items + x;
	xt = aitem->check;
	z = y = x;

	while ((y = links [y]) != 0 && t_items [y].check == xt)
	    z = y;

	if (z != x) {
	    /* Non determinisme */
	    links [z] = 0; /* pour gen_nd_seq */
	    aitem->action = -gen_nd_seq (x, links, t_items, fe);
	    links [x] = y;
	}
    } while ((x = y) != 0);

    /* Ok pour la sequence de predicats. */
    if (t_items [hd].check > -bnf_ag.WS_TBL_SIZE.tmax) {
	/* On genere explicitement le cas erreur. */
	build_branch (-1 /* &Else */, NULL, 0);
    }

    x = hd;

    do {
	aitem = t_items + x;
	xt = aitem->check;
	build_branch (XT_TO_PRDCT_CODE (xt), aitem->action > 0 ? t_fe + aitem->action : tnt_fe - aitem->action, 0);
	x = links [y = x];
	aitem->action = 0;
	links [y] = 0;
    } while (x != 0);

    return gen_tree (Prdct, 0);
}


static void compute_action_set (xac1, t_items, action_set)
    int			xac1;
    struct P_item	*t_items;
    SXBA		action_set;
{
    /* xac1 est un etat interne d'un ARC atteint depuis l'etat initial
       dont on calcule recursivement la liste des
       actions de l'automate sous-jacent qu'il peut declencher. */
    /* 		germe_to_gr_act [next]		signification

		         0			     vide
			 >0			calcule' (fe) ou
			 			  a` calculer
			 <0			   en cours
    */
    struct P_item	*bot_aitem, *aitem;
    struct state	*astate;
    int			action, codop, next;

    germe_to_gr_act [xac1] = -germe_to_gr_act [xac1] - 1; /* < 0 */
    astate = t_states + xac1;
    aitem = (bot_aitem = t_items + astate->start) + astate->lgth;

    while (--aitem >= bot_aitem) {
	if ((action = aitem->action) != 0) {
	    if ((codop = t_fe [action].codop) == LaScanShift) {
		if (germe_to_gr_act [next = t_fe [action].next] >= 0) {
		    compute_action_set (next, t_items, action_set);
		}
	    }
	    else if (codop == Reduce
		     || codop == ScanReduce
		     || codop == ScanShift) {
		SXBA_1_bit (action_set, action);
	    }
	}
    }
}

static void reset_germe_to_gr_act ()
{
    /* germe_to_gr_act a ete modifie' par l'appel precedent de compute_action_set
       afin de ne pas boucler et d'eviter des recalculs. On remet comme avant. */
    int xac1, val;

    for (xac1 = init_state + 1; xac1 < xe1; xac1++) {
	if ((val = germe_to_gr_act [xac1]) < 0)
	  germe_to_gr_act [xac1] = -val - 1;  
    }
}


static int compute_germe_to_gr_act (action_set)
    SXBA	action_set;
{
    /* Retourne l'adresse du fe referencant la sequence non deterministe correspondant
       aux actions de action_set. */
    int		degree, action;
    struct floyd_evans		*afe;

    degree = 20000;

    /* On genere les reduce en dernier. */
    action = 0;

    while ((action = sxba_scan (action_set, action)) > 0) {
	if ((afe = t_fe + action)->codop != Reduce) {
	    SXBA_0_bit (action_set, action);
	    build_branch (degree, afe, 0);
	    degree++;
	}
    }

    action = 0;

    while ((action = sxba_scan_reset (action_set, action)) > 0) {
	build_branch (degree, t_fe + action, 0);
	degree++;
    }

    return gen_tree (NonDeter, 0);
}



int	OPTIM ()
{
    if (sxverbosep) {
	fprintf (sxtty, "%s\n", ME);
	sxttycol1p = true;
    }

    stats.t.i.lines = xe1 - 1;
    stats.nt.i.lines = xac2 - 1;
    stats.t.i.cols = bnf_ag.WS_TBL_SIZE.xtmax;
    stats.nt.i.cols = bnf_ag.WS_TBL_SIZE.ntmax;
    single_prod = sxba_calloc (bnf_ag.WS_TBL_SIZE.xnbpro + 1);

    if (is_tspe && !is_non_deterministic_automaton) {
	/*   E L I M I N A T I O N   T O T A L E   D E S   P R O D U C T I O N S   S I M P L E S   */

	int	XNTMAX = bnf_ag.WS_TBL_SIZE.xntmax;

	if (sxverbosep) {
	    fprintf (sxtty, "\tTotal Single Productions Elimination ... ");
	    sxttycol1p = false;
	}

	total_spe (single_prod);
	XxY_free (&Q0xV_hd);
	sxfree (Q0xV_to_Q0);
	XxY_free (&Q0xQ0_hd);
	sxfree (Q0), Q0 = NULL;
	XH_free (&nucleus_hd);

	if (clone_to_lr0_state != NULL)
	    sxfree (clone_to_lr0_state);

	if (XNTMAX < bnf_ag.WS_TBL_SIZE.xntmax) {
	    /* reallocation de nt_to_ad et nt_to_nt */
	    register int	i;

	    nt_to_ad = (int*) sxrealloc (nt_to_ad, bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (int));
	    nt_to_nt = (int*) sxrealloc (nt_to_nt, bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (int));

	    for (i = XNTMAX + 1; i <= bnf_ag.WS_TBL_SIZE.xntmax; i++) {
		nt_to_nt [i] = i;
		nt_to_ad [i] = 0;
	    }
	}

	if (sxverbosep) {
	    sxtime (SXFINAL, "done");
	    sxttycol1p = true;
	}
    }

    {
	/*   C L A S S E S   D ' E Q U I V A L E N C E S   */

	/* Ce bloc partitionne un ensemble d'etats (bi_automate) en classes
   d'equivalences. Chaque automate est represente sous forme matricielle de
   dimension state_no * tmax ou state_no est le nombre de lignes (etats),
   tmax est le nombre de colonnes (symboles)

   Pour chaque automate on construit un vecteur equiv_class de taille
   0:state_no (attention, 0 est utilise comme memoire de manoeuvre) ou
   chaque element est un doublet (repr, eq_lnk). Pour un etat s, repr(s)
   est le representant de la classe d'equivalence a laquelle appartient
   s. Soit s' = eq_lnk(s) si s'>0 alors s' est l'etat suivant (s'>s) de
   la meme classe d'equivalence si s=0 alors s est le dernier etat de
   cette classe d'equivalence.

   Deux etats sont 0_indistinguables ssi les actions correspondant a un
   symbole sont 0_indistinguables.

   Methode
   On partitionne tout d'abord l'ensemble des etats en sous_ensembles dont les
   elements sont 0_indistinguables. Puis on raffine ces partitions.
   Deux etats sont k+1_indistinguables ssi leurs actions shift correspondantes
   referencent des etats k_indistinguables. Les classes d'equivalences sont
   obtenues lorsque la partition est stable */


	register int	i;
	int	*t_partitions, *t_eq_lnks /* 1:xe1 */ , *nt_partitions, *nt_eq_lnks /* 1:xac2 */ ;

	if (sxverbosep) {
	    fprintf (sxtty, "\tAutomaton Reduction ... ");
	    sxttycol1p = false;
	}

	t_partitions = (int*) sxalloc (xe1 + 1, sizeof (int));
	t_eq_lnks = (int*) sxalloc (xe1 + 1, sizeof (int));
	nt_partitions = (int*) sxalloc (xac2 + 1, sizeof (int));
	nt_eq_lnks = (int*) sxalloc (xac2 + 1, sizeof (int));

/* Partitionnement des etats en sous_ensembles dont les elements sont
   0_indistinguables */

	t_partitions [0] = 0;
	stats.t.r.lines = stats.nt.r.lines = 0;
	zero_indistinguable (xe1, t_state_equiv_class, t_states, t_items, t_fe, t_partitions, t_eq_lnks, &stats.t.r.lines
	     );
	nt_partitions [0] = 0;
	zero_indistinguable (xac2, nt_state_equiv_class, nt_states, nt_items, nt_fe, nt_partitions, nt_eq_lnks, &stats.nt
	     .r.lines);

	while (!is_stable (t_state_equiv_class, t_states, t_items, t_fe, t_partitions, t_eq_lnks, &stats.t.r.lines) || !
	     is_stable (nt_state_equiv_class, nt_states, nt_items, nt_fe, nt_partitions, nt_eq_lnks, &stats.nt.r.lines))
	    ;

	t_items_size = 0;

	for (i = 1; i < xe1; i++) {
	    if (t_state_equiv_class [i] != i) {
		t_states [i].lgth = 0;
	    }
	    else
		t_items_size += t_states [i].lgth;
	}

	nt_items_size = 0;

	for (i = 1; i < xac2; i++) {
	    if (nt_state_equiv_class [i] != i) {
		nt_states [i].lgth = 0;
	    }
	    else
		nt_items_size += t_states [i].lgth;
	}

	sxfree (nt_eq_lnks);
	sxfree (nt_partitions);
	sxfree (t_eq_lnks);
	sxfree (t_partitions);

	if (sxverbosep) {
	    sxtime (SXFINAL, "done");
	    sxttycol1p = true;
	}
    }

/* Dans le cas du non-determinisme, on n'elimine pas les productions simples :
   - le cas tspe n'a pas ete etudie, donc, dans le doute...
   - le cas pspe peut conduire a des detections d'ambiguites inexistantes.
     Soit A -> B une p.s. dont une occurrence dans les NT_tables est eliminee
     et une autre occurrence dans les T_tables est conservee. Il est possible
     d'avoir deux sous_arbres d'analyse identiques, excepte que l'un utilise
     A -> B et pas l'autre.
*/
    if (is_pspe) {
	/*   E L I M I N A T I O N   P A R T I E L L E   D E S   P R O D U C T I O N S   S I M P L E S   */

	SXBA	ne_set;

	if (sxverbosep) {
	    fprintf (sxtty, "\tPartial Single Productions Elimination ... ");
	    sxttycol1p = false;
	}

	if (is_non_deterministic_automaton) {
	    /* On note dans ne_set les numeros des productions simples qui sont
	       utilisees ailleurs que dans les NT_tables. Ces dernieres ne seront
	       pas eliminees (voir le commentaire ci_dessus). */
	    int		i;
	    register struct P_item	*aitem, *lim;
	    register struct state	*astate;
	    register struct floyd_evans		*afe;

	    ne_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xnbpro + 1);

	    for (i = 1; i < xe1; i++) {
		if ((astate = t_states + i)->lgth > 0) {
		    for (lim = (aitem = t_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
			if (aitem->action != 0) {
			    if ((afe = t_fe + aitem->action)->codop == Reduce &&
				bnf_ag.WS_NBPRO [afe->reduce].bprosimpl)
				SXBA_1_bit (ne_set, afe->reduce);
			}
		    }
		}
	    }
	}
	else
	    ne_set = NULL;

	partial_spe (single_prod, ne_set);

	if (ne_set != NULL)
	    sxfree (ne_set);

	if (sxverbosep) {
	    sxtime (SXFINAL, "done");
	    sxttycol1p = true;
	}
    }

    if (sxverbosep) {
	fprintf (sxtty, "\tCompaction\n");
	sxttycol1p = true;
    }


/*   C O M P A C T I O N   D E S   T _ T A B L E S   */


    if (sxverbosep) {
	fprintf (sxtty, "\t\tT_Tables ... ");
	sxttycol1p = false;
    }

    {
	/* On cree les instructions "test scan reduce" quand c'est possible */
	/* LALR1 a pu ne pas les engendrer toutes dans les cas suivants :
	   	- Option "-tspe"
		- Application des regles de priorite de l'utilisateur (suppression des
		  actions "scan" de l'etat courant */

	int	*state_to_tsr;
	bool		is_test_scan_reduce;

	state_to_tsr = (int*) sxcalloc (xe1 + 1, sizeof (int));
	is_test_scan_reduce = false;

	{
	    /* on remplit state_to_tsr */

	    register int	i, red_act;
	    register bool	is_tsr;
	    register struct P_item	*aitem, *lim;
	    register struct state	*astate;
	    register struct floyd_evans		*afe;

	    for (i = 1; i < xe1; i++) {
		if ((astate = t_states + i)->lgth > 0) {
		    is_tsr = true;
		    red_act = 0;

		    for (lim = (aitem = t_items + astate->start) + astate->lgth - 1; aitem <= lim; aitem++) {
			if (aitem->action != 0) {
			    if ((afe = t_fe + aitem->action)->codop == Reduce && afe->reduce != 0 /* pas Halt */  && afe
				 ->pspl >= 1 /* pas une production vide */ ) {
				if (red_act == 0)
				    red_act = aitem->action;
				else if (red_act != aitem->action)
				    is_tsr = false;
			    }
			    else if (aitem->action != t_error)
				is_tsr = false;
			}
		    }

		    if (is_tsr) {
			state_to_tsr [i] = red_act;
			is_test_scan_reduce = true;
		    }
		}
	    }
	}

	if (is_test_scan_reduce) {
	    /* On genere dans les t_tables des "test scan reduce" la ou c'est possible */
	    /* On supprime des t_tables les etats inaccessibles grace a l'introduction
	       des instructions ScanReduce */

	    register int	i, j;
	    register struct P_item	*aitem, *lim;
	    register struct state	*astate;
	    register struct floyd_evans		*afe;

	    for (i = 1; i < xe1; i++) {
		if ((astate = t_states + i)->lgth > 0) {
		    if (state_to_tsr [i] != 0)
			t_states [i].lgth = 0;
		    else
			for (lim = (aitem = t_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
			    if (aitem->action != 0) {
				if ((afe = t_fe + aitem->action)->codop == ScanShift && (j = state_to_tsr [afe->next]) !=
				     0) {
				    t_fe [0] = t_fe [j];
				    t_fe->codop = ScanReduce;
				    t_fe->pspl--;
				    aitem->action = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
				}
			    }
			}
		}
	    }
	}

	if (is_test_scan_reduce) {
	    /* On genere dans les nt_tables des "test shift reduce" la ou c'est possible */

	    register int	i, j;
	    register struct P_item	*aitem, *lim;
	    register struct state	*astate;
	    register struct floyd_evans		*afe;

	    for (i = 1; i < xac2; i++) {
		if ((astate = nt_states + i)->lgth > 0) {
		    for (lim = (aitem = nt_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
			if (aitem->action != 0) {
			    if ((afe = nt_fe + aitem->action)->codop == Shift && (j = state_to_tsr [afe->next]) != 0) {
				nt_fe [0] = t_fe [j];
				nt_fe->pspl--;
				aitem->action = put_in_fe (nt_fe, nt_hash, nt_lnks, nt_fe);
			    }
			}
		    }
		}
	    }
	}

	sxfree (state_to_tsr);

	if (sxverbosep) {
	    sxtime (SXFINAL, "done");
	    sxttycol1p = true;
	}
    }


/* Le floyd_evans des transitions terminales, non-terminales et des predicats est regenere
   dans une structure unique tnt */

    {
	/*   C O M P A C T I O N   D E S   N T _ T A B L E S   */

	register int			i, k, nt, xnt;
	register struct P_item		*aitem, *lim;
	register struct state		*astate;
	register struct xt_to_act	*pact;
	register struct floyd_evans	*afe;
	int				l, x, hash_code, action;

	if (sxverbosep) {
	    fprintf (sxtty, "\t\tNT_Tables ... ");
	    sxttycol1p = false;
	}

	tnt_fe = (struct floyd_evans*)
	    sxalloc (tnt_fe_size = *t_lnks + *nt_lnks + xpredicates + xprdct_items + 1,
		     sizeof (struct floyd_evans));
	tnt_lnks = (int*) sxalloc (tnt_fe_size, sizeof (int));
	tnt_fe_to_new = (int*) sxcalloc (tnt_fe_size, sizeof (int));
	reductions = (struct P_reduction*) sxalloc (tnt_fe_size, sizeof (struct P_reduction));
	lnks = (int*) sxalloc (tnt_fe_size, sizeof (int));
	lnks [0] = 0 /* taille de reductions */ ;

	if (xprdct_items > 0) {
	    predicates = (struct state*) sxalloc (xpredicates + 1, sizeof (struct state));
	    prdct_lnks = (int*) sxalloc (xpredicates + 1, sizeof (int));
	    prdcts = (struct P_prdct*) sxalloc (xprdct_items + 1, sizeof (struct P_prdct));
	    prdct_lnks [0] = 0;
	    xprdct_mngr (SXINIT);
	}
	else {
	    prdcts = NULL;
	    predicates = NULL;
	    prdct_lnks = NULL;
	}

	xpredicates = xprdct_items = 0;

	for (i = 0; i < HASH_SIZE; i++)
	    tnt_hash [i] = prdct_hash [i] = 0;

	tnt_lnks [0] = 0;
	t_error = put_in_fe (tnt_fe, tnt_hash, tnt_lnks, t_fe + t_error);

	{
	    /* On genere egalement les sequences de predicats */
	    int		action;
	    struct P_item	*old_aitem, *old_nt_items = nt_items;
	    SXBA	nt_set;
	    struct xt_to_act	*xnt_to_act;

	    nt_items = (struct P_item*) sxalloc (xnt_items + 1, sizeof (struct P_item));
	    nt_set = sxba_calloc (NTMAX + 1);
	    xnt_to_act = (struct xt_to_act*) sxcalloc (bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (struct xt_to_act));
	    xnt_items = 1;

	    for (i = 1; i < xac2; i++) {
		if ((l = (astate = nt_states + i)->lgth) > 0) {
		    astate->lgth = 0;
		    lim = (old_aitem = old_nt_items + astate->start) + l;
		    astate->start = xnt_items;

		    if (xnt_state_set != NULL && SXBA_bit_is_set (xnt_state_set, i)) {
			/* Il y a des predicats dans le coup */
			for (aitem = old_aitem; aitem < lim; aitem++) {
			    if (aitem->action != 0) {
				xnt = aitem->check;

				if (XNT_TO_PRDCT_CODE (xnt) != -1) {
				    insert_xnt_lnk (xnt, xnt_to_act);
				    xnt_to_act [xnt].action = aitem->action;
				    nt = XNT_TO_NT_CODE (xnt);
				}
				else {
				    xnt_to_act [nt = xnt].action = aitem->action;
				}

				SXBA_1_bit (nt_set, nt);
			    }
			}

			nt = 0;

			while ((nt = sxba_scan_reset (nt_set, nt)) > 0) {
			    if ((xnt = (pact = xnt_to_act + nt)->lnk) != 0) {
				/* Traitement des predicats */
				for (xnt = nt; xnt != 0; xnt = xnt_to_act [xnt].lnk) {
				    build_branch (XNT_TO_PRDCT_CODE (xnt), nt_fe + xnt_to_act [xnt].action, 0);
				}

				action = gen_tree (Prdct, 0);
			    }
			    else {
				/* Cas simple */
				fe_to_equiv (tnt_fe, nt_fe + pact->action);
				action = put_in_fe (tnt_fe, tnt_hash, tnt_lnks, tnt_fe);
			    }

			    aitem = nt_items + xnt_items++;
			    aitem->check = nt;
			    aitem->action = action;
			    astate->lgth++;
			    pact->lnk = pact->action = 0;
			}
		    }
		    else {
			/* Pas de predicat pour cet etat */
			for (aitem = old_aitem; aitem < lim; aitem++) {
			    if (aitem->action != 0) {
				fe_to_equiv (tnt_fe, nt_fe + aitem->action);
				nt_items [xnt_items].check = aitem->check;
				nt_items [xnt_items++].action = put_in_fe (tnt_fe, tnt_hash, tnt_lnks, tnt_fe);
				astate->lgth++;
			    }
			}
		    }
		}
	    }

	    nt_fe_size = *tnt_lnks;
	    sxfree (nt_set);
	    sxfree (xnt_to_act);
	    sxfree (nt_lnks), nt_lnks = NULL;
	    sxfree (nt_fe), nt_fe = NULL;
	    sxfree (old_nt_items);

	    if (xnt_state_set != NULL)
		sxfree (xnt_state_set);
	}

	rows = (struct row*) sxcalloc (max (bnf_ag.WS_TBL_SIZE.xntmax, xe1) + 1, sizeof (struct row));

	{
	    /* Si deux colonnes sont identiques (on ne prend pas compatible a cause du rattrapage
	       d'erreurs) on supprime une des colonnes et le non terminal devient equivalent a
	       l'autre (sauf sur les non-terminaux etendus). */
	    /* On commence par calculer la "taille" de chaque "colonne" */

	    int		cols_size;

	    cols_size = 0;

	    for (i = 1; i < xac2; i++) {
		if ((k = (astate = nt_states + i)->lgth) != 0) {
		    for (lim = (aitem = nt_items + astate->start) + k; aitem < lim; aitem++) {
			if (aitem->action != 0) {
			    ++(rows [aitem->check].lgth);
			    ++cols_size;
			}
		    }
		}
	    }

	    cols = (struct P_item*) sxalloc (cols_size + 1, sizeof (struct P_item));

/* On remplit rows.start */

	    rows [0].start = 1;

	    for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xntmax; i++) {
		rows [i].start = rows [i - 1].start + rows [i].lgth;
	    }


/* On remplit cols et rows.hash */

	    for (i = xac2 - 1; i > 0; i--) {
		if ((k = (astate = nt_states + i)->lgth) != 0) {
		    for (aitem = (lim = nt_items + astate->start) + k - 1; aitem >= lim; aitem--) {
			if (aitem->action != 0) {
			    k = --(rows [aitem->check].start);
			    cols [k].check = i;
			    cols [k].action = aitem->action;
			    rows [aitem->check].hash += i + aitem->action;
			}
		    }
		}
	    }

	    ITEMS = cols;
	    equality_sort (nt_to_nt, 1, bnf_ag.WS_TBL_SIZE.xntmax, row_le, row_eq);
	    sxfree (cols), cols = NULL;
	}


/* Si pour un non terminal donne, quelque soit l'etat considere ayant une
   transition sur ce non terminal, l'adresse de branchement est la meme,
   cette adresse est connue statiquement et figurera dans le champ goto de
   l'instruction correspondante. */

	/* On construit un tableau nt_to_ad qui pour chaque non terminal simple nt indique
	   l'adresse unique si elle existe ou -1 sinon. */

	for (i = 1; i < xac2; i++) {
	    if ((k = (astate = nt_states + i)->lgth) != 0) {
		for (lim = (aitem = nt_items + astate->start) + k; aitem < lim; aitem++) {
		    if ((k = nt_to_ad [aitem->check]) >= 0) {
			if (k == 0)
			    nt_to_ad [aitem->check] = aitem->action;
			else if (k != aitem->action)
			    nt_to_ad [aitem->check] = -1;
		    }
		}
	    }
	}

#if 0
/* Si un etat comporte une seule adresse, cet etat est supprime et c'est l'adresse
   qui sera empilee. Ce fait est note par nt_state_equiv_class [state_no] = -action.
   On en profite pour supprimer les items dont le non terminal est different du
   representant. */
/* On supprime egalement les items dont le non terminal nt est tel que nt_to_ad [nt] = a
   (a != -1) tels que:
   - a reference un Reduce (les action ulterieures seront atteintes par ailleurs)
   ou
   - il existe dans le meme etat un item (nt', a') qui est conserve
   et tel que a == a'.
   Ces items ne peuvent pas etre tous supprimes a cause du traitement des erreurs
   globales. */

	{
	    int		*occur_nb;
	    bool	is_nt_to_ad;

	    occur_nb = (int*) sxcalloc (nt_fe_size + 1, sizeof (int));
	    stats.nt.c.lines = stats.nt.r.lines;

	    for (i = 1; i < xac2; i++) {
		if ((k = (astate = nt_states + i)->lgth) != 0) {
		    action = 0;
		    is_nt_to_ad = false;

		    for (lim = (aitem = nt_items + astate->start) + k; aitem < lim; aitem++) {
			if (nt_to_nt [aitem->check] != aitem->check)
			    aitem->check = aitem->action = 0;
			else {
			    if (nt_to_ad [aitem->check] != -1)
				is_nt_to_ad = true;

			    if (action == 0)
				action = aitem->action;
			    else if (aitem->action != action)
				action = -1;
			}
		    }

		    if (action > 0) {
			/* Dans l'etat i toutes les transitions conduisent a la meme action */
			stats.nt.c.lines--;
			nt_state_equiv_class [i] = -action;
			nt_states [i].lgth = 0;
		    }
		    else if (is_nt_to_ad) {
			for (aitem = nt_items + nt_states [i].start; aitem < lim; aitem++)
			    if (aitem->action != 0 && nt_to_ad [aitem->check] == -1)
				occur_nb [aitem->action]++;

			for (aitem = nt_items + nt_states [i].start; aitem < lim; aitem++)
			    if (aitem->action != 0 && nt_to_ad [aitem->check] != -1)
				if (occur_nb [aitem->action] != 0 || tnt_fe [aitem->action].codop == Reduce)
				    aitem->check = aitem->action = 0;
				else
				    occur_nb [aitem->action]++;

			for (aitem = nt_items + nt_states [i].start; aitem < lim; aitem++)
			    if (aitem->action != 0) {
				occur_nb [aitem->action] = 0;
			    }
		    }
		}
	    }

	    sxfree (occur_nb);
	}
#endif

/* MODIFS 19/02/93
   L'idee est de ne plus perdre d'info pour le rattrapage d'erreur global.
   On veut savoir si depuis un etat il existe une sequence (eventuellement vide)
   de transitions (terminales ou non-terminales) menant a un etat sur lequel
   il existe une transition sur l'un des terminaux cles (definis ds recor).
*/
	
/* On supprime donc les adresses de branchement en pile et on conserve dans les
   etats les transitions non-terminales reperees par nt_to_ad [nt] = a (a != -1).
   Les branchements directs sur reduction etant conserves, ces transitions ne
   seront jamais utilisees en analyse normale (seul le rattrapage global les
   utilisera).
*/

/* Maintenant, on a toujours nt_state_equiv_class [state_no] >= 0
   On en profite pour supprimer les items dont le non terminal est different du
   representant. */

/* De plus on reorganise les valeurs des refs (voir commentaire au debut de
   "T A B L E S   O U T P U T" */

	stats.nt.c.lines = stats.nt.r.lines;
	
	for (i = 1; i < xac2; i++) {
	    if ((k = (astate = nt_states + i)->lgth) != 0) {
		/* On a nt_state_equiv_class [i] == i */
		for (lim = (aitem = nt_items + astate->start) + k; aitem < lim; aitem++) {
		    if (nt_to_nt [aitem->check] != aitem->check)
			aitem->check = aitem->action = 0;
		}
	    }
	}


/* signification de j = nt_state_equiv_class [i] :
      - j==0  => L'etat i n'a pas de transition non-terminale
      - 0<j<i => j est equivalent a i (i est le representant)
      - j==i  => i est le representant et il reste des transitions
*/


/* On remplit reductions et tnt_fe_to_new pour les transitions non_terminales */

	for (i = 0; i < HASH_SIZE; i++)
	    /* ca permet de traiter plusieurs constructions par un seul appel de rlr */
	    hashs [i] = 0;

	for (i = 1; i <= nt_fe_size; i++) {
	    if ((afe = tnt_fe + i)->codop == Reduce)
		tnt_fe_to_new [i] = -put_in_reductions (hashs, lnks, afe);
	}

	mMrednt = -lnks [0];
    }


    {

/* On regenere le floyd_evans des transitions terminales afin de profiter
   (eventuellement) des identites introduites par les classes d'equivalence.
   On n'exploite pas ni les adresses de branchement directes ni l'equivalence
   des non terminaux (ca ne peut pas produire d'identites supplementaires) et
   ces adresses ne sont pas definitives */
/* On genere egalement les sequences de predicats */
/* On genere dans les t_tables la reduction la plus populaire */
/* Attention le test de "EOF" doit etre conserve sur l'instruction "Halt" */

	SXBA				t_set, action_set;
	struct P_item			*old_aitem, *old_t_items = t_items;
	register int			i, t, xt, plulong;
	register struct P_item		*aitem, *lim;
	register struct state		*astate;
	register struct floyd_evans	*afe;
	int				l, x, hash_code, action, xlink, old_xlink, old_xlink_lim;
	int				*reduces, *link, *xt_to_link;
	bool				is_force_error;

	t_items = (struct P_item*) sxalloc (xt_items + 1, sizeof (struct P_item));
	link = (int*) sxalloc (xt_items + 1, sizeof (int));
	xt_to_link = (int*) sxcalloc (any + 1, sizeof (int));
	reduces = (int*) sxcalloc (*t_lnks + 1, sizeof (int));
	t_set = sxba_calloc (any + 1);
	xt_items = 1;

	if (is_non_deterministic_automaton && germe_to_gr_act != NULL) {
	    /* On est dans le cas non deterministe et il y a des ARCs */
	    action_set = sxba_calloc (tnt_fe_size);
	}
	else
	    action_set = NULL;

	for (i = 1; i < xe1; i++) {
	    if ((l = (astate = t_states + i)->lgth) > 0) {
		lim = (old_aitem = old_t_items + astate->start) + l - 1;

		if (action_set != NULL && germe_to_gr_act [i] != 0) {
		    /* generation pour la recuperation ds le cas non deterministe */
		    compute_action_set (i, old_t_items, action_set);
		    reset_germe_to_gr_act ();
		    germe_to_gr_act [i] = compute_germe_to_gr_act (action_set);
		}

		astate->lgth = 0;
		old_xlink_lim = (old_xlink = astate->start) + l - 1; 

		if (xt_state_set != NULL && SXBA_bit_is_set (xt_state_set, i)) {
		    /* Il y a des predicats dans le coup */
		    /* On ne regarde pas l'instruction associee a any. */
		    for (aitem = lim - 1, xlink = old_xlink_lim - 1; aitem >= old_aitem; aitem--, xlink--) {
			/* On n'execute pas un predicat en look-ahead si son execution
			   n'est pas necessaire a la reconnaissance de la reduction
			   associee. On commence par detecter ce cas. */
			/* Attention au cas nondeterministe. */
			if ((action = aitem->action) != 0) {
			    if ((t = aitem->check) > -bnf_ag.WS_TBL_SIZE.tmax) {
				/* On lie ensemble le fe declanche par des terminaux
				   etendus compatibles. */
				t = XT_TO_T_CODE (t);
				link [xlink] = xt_to_link [t];
				xt_to_link [t] = xlink;
				SXBA_1_bit (t_set, t);
			    }
			    else if (SXBA_bit_is_set (t_set, t)) {
				link [xlink] = xt_to_link [t];
				xt_to_link [t] = xlink;
			    }
			}
		    }

		    for (aitem = old_aitem, xlink = old_xlink; aitem < lim; aitem++, xlink++) {
			if (aitem->action != 0) {
			    xt = aitem->check;
			    t = XT_TO_T_CODE (xt);
				
			    if (SXBA_bit_is_set (t_set, t)) {
				/* Tete de lien sur les predicats. */
				register int x = xt_to_link [t], y;

				if (t_fe [action = aitem->action].codop == Reduce) {
				    /* Candidat */
				    while ((x = link [x]) != 0) {
					if (old_t_items [x].action != action)
					    /* Non compatibles */
					    break;
				    }
				}

				if (x == 0) {
				    /* Compatibles, on supprime les predicats. */
				    aitem->check = t; /* au cas ou */
				    /* RAZ des elements lies (sauf la tete) */
				    x = xt_to_link [t];

				    while ((x = link [y = x]) != 0) {
					link [y] = 0;
					old_t_items [x].action = 0;
				    }
				}
				else
				    /* Les references ds tnt_fe (predicats) sont codees
				       en negatif. */
				    aitem->action =
					-gen_prdct_seq (xt_to_link [t], link, old_t_items, t_fe);
			    }
			    else {
				/* pas de predicat */
				link [xlink] = xt_to_link [t];
				xt_to_link [t] = xlink;
				SXBA_1_bit (t_set, t);
			    }
			}
		    }
		}
		else {
		    for (aitem = old_aitem, xlink = old_xlink; aitem < lim; aitem++, xlink++) {
			if ((action = aitem->action) != 0) {
			    /* pas de predicat */
			    t = aitem->check;
			    link [xlink] = xt_to_link [t];
			    xt_to_link [t] = xlink;
			    SXBA_1_bit (t_set, t);
			}
		    }
		}

		/* Calcul de plulong */
		/* On ne genere pas d'action par defaut ds le cas non-determinisme + ARC
		   pour avoir une detection/correction plus fine. */

		plulong = 0;

		if (lim->action == t_error && (action_set == NULL || i <= init_state)) {
		    t = 0;

		    while ((t = sxba_scan (t_set, t)) > 0) {
			if (link [xlink = xt_to_link [t]] == 0 &&
			    (action = old_t_items [xlink].action) > 0) {
			    /* Un seul element dans la liste, il ne s'agit donc
			       pas de non-determinisme. C'est donc un candidat. */
			    /* action > 0, ce n'est donc pas un predicat, le fe
			       se trouve ds t_fe. */
			    if ((afe = t_fe + action)->codop == Reduce &&
				afe->reduce != 0 /* pas Halt */  &&
				++reduces [action] > reduces [plulong])
				plulong = action;
			}
		    }
		}

		astate->start = xt_items;
		t = 0;

		while ((t = sxba_scan_reset (t_set, t)) > 0) {
		    if (link [xlink = xt_to_link [t]] != 0) {
			/* Non_determinisme */
			action = gen_nd_seq (xlink, link, old_t_items, t_fe);
		    }
		    else {
			action = old_t_items [xlink].action; 
			reduces [action] = 0;
			
			if (plulong == action)
			    action = 0;
			else {
			    /* Cas simple */
			    fe_to_equiv (tnt_fe, action > 0 ? t_fe + action : tnt_fe - action);
			    action = put_in_fe (tnt_fe, tnt_hash, tnt_lnks, tnt_fe);
			}
		    }
		    
		    if (action != 0) {
			aitem = t_items + xt_items++;
			aitem->check = t;
			aitem->action = action;
			astate->lgth++;
		    }
		    
		    xt_to_link [t] = 0;
		}

		is_force_error = false;

		if (plulong != 0) {
		    fe_to_equiv (tnt_fe, t_fe + plulong);

		    if (i > init_state)
			/* is_non_deterministic_automaton == false */
			germe_to_gr_act [i] = 0;
		}
		else if (t_fe [lim->action].codop != ForceError)
		    fe_to_equiv (tnt_fe, t_fe + lim->action);
		else
		    is_force_error = true;

		aitem = t_items + xt_items++;
		aitem->check = any;
		aitem->action = is_force_error ? t_error : put_in_fe (tnt_fe, tnt_hash, tnt_lnks, tnt_fe);
		astate->lgth++;
	    }
	}

	if (action_set != NULL)
	    sxfree (action_set);

	sxfree (reduces);
	sxfree (t_set);
	sxfree (link);
	sxfree (xt_to_link);

	if (prdct_lnks != NULL) {
	    sxfree (prdct_lnks), prdct_lnks = NULL;
	    xprdct_mngr (SXCLOSE);
	}

	sxfree (t_lnks);
	sxfree (old_t_items);

	if (xt_state_set != NULL)
	    sxfree (xt_state_set);

	if (nd_state_set != NULL)
	    sxfree (nd_state_set);


/* On remplit reductions et tnt_fe_to_new pour les transitions terminales
   Elles doivent etre situees a des adresses differentes des transitions non terminales car
   l'interpretation dynamique (a l'analyse) du nombre de depilages est different et seule
   l'adresse permet de savoir si on a a faire a une transition terminale ou non terminale */

	for (i = 0; i < HASH_SIZE; i++)
	    /* ca permet d'ignorer les transitions non terminales */
	    hashs [i] = 0;

	for (i = nt_fe_size + 1; i <= *tnt_lnks; i++) {
	    if ((afe = tnt_fe + i)->codop == Reduce)
		tnt_fe_to_new [i] = -put_in_reductions (hashs, lnks, afe);
	    else if (afe->codop == ScanReduce)
		tnt_fe_to_new [i] = put_in_reductions (hashs, lnks, afe);
	}

	if (!is_non_deterministic_automaton && germe_to_gr_act != NULL) {
	    /* Il y a des ARCs */
	    /* Dans le cas deterministe germe_to_gr_act[germe] est un indice dans t_fe vers un
	       reduce (deja range dans tnt_fe et reductions). On modifie germe_to_gr_act
	       afin qu'il reference reductions (en negatif car il n'y a pas de scan) */
	    /* Dans le cas non deterministe germe_to_gr_act[germe] est un indice vers le debut
	       de la sequence non deterministe des actions du germe encore executables dans
	       le sous ARC de racine i. (traite' plus loin) */
	    for (i = init_state + 1; i < xe1; i++) {
		if ((t = germe_to_gr_act [i]) > 0)
		    germe_to_gr_act [i] = -put_in_reductions (hashs, lnks, t_fe + t);
	    }
	}

	Mred = lnks [0];
	sxfree (t_fe);
	sxfree (lnks);
	mMred = -Mred;

	if (sxverbosep) {
	    sxtime (SXFINAL, "done");
	    sxttycol1p = true;
	}
    }

    {

/*   L I N E A R I S A T I O N   D E S   M A T R I C E S   */


	int	*state_to_state, *witness, xw;
	SXBA	bases_set;

	if (sxverbosep) {
	    fprintf (sxtty, "\tMatrix Linearization\n");
	    sxttycol1p = true;
	}

	state_to_state = (int*) sxalloc (xe1 + 1, sizeof (int));
	witness = (int*) sxalloc (xe1 + 1, sizeof (int));

	{
	    /* Linearisation des T_tables */
	    /* Creation des defauts */
	    /* Si un etat contient une seule action essentielle, elle est implantee
	       directement dans (propre, commun) = (check, action ) */
	    /* Apres extraction de l'action par defaut, 2 etats peuvent devenir identiques! */

	    register struct P_item	*aitem, *lim;
	    register struct state	*astate;
	    register int	i, k, l;

	    if (sxverbosep) {
		fprintf (sxtty, "\t\tT_tables ... ");
		sxttycol1p = false;
	    }

	    t_bases = (struct P_base*) sxcalloc (xe1 + 1, sizeof (struct P_base));
	    vector = (struct P_item*) sxcalloc ((l = 3 * xt_items) + 1, sizeof (struct P_item));
	    vector [0].action = l;
	    bases_set = sxba_calloc (l + 1);
	    xw = 0;

	    for (i = 1; i < xe1; i++) {
		if ((l = t_states [i].lgth) > 0) {
		    register struct P_base	*abase = t_bases + i;

		    lim = (aitem = t_items + t_states [i].start) + l - 1;
		    abase->defaut = lim->action;
		    abase->propre = grand;
		    abase->commun = grand;
		    lim->check = lim->action = 0;

		    if (l == 1) {
			t_states [i].lgth = 0;
		    }
		    else if (l == 2) {
			abase->propre += aitem->check;
			abase->commun += aitem->action;
			t_states [i].lgth = 0;
		    }
		    else {
			register struct row	*arowi = rows + i;

			state_to_state [++xw] = i;
			witness [xw] = i;
			arowi->lgth = 0;
			arowi->hash = 0;
			arowi->start = t_states [i].start;

			for (; aitem < lim; aitem++) {
			    (arowi->lgth)++;
			    arowi->hash += aitem->check + aitem->action;
			}
		    }
		}
	    }

	    ITEMS = t_items;
	    equality_sort (state_to_state, 1, xw, row_le, row_eq);

	    for (i = 1; i <= xw; i++) {
		if (state_to_state [i] != (k = witness [i]))
		    t_states [k].lgth = 0;
	    }

	    MIN = MAX = 1;
	    delta = 0;
	    algoV (false, xe1, -bnf_ag.WS_TBL_SIZE.tmax, xt_items, t_states, t_items, t_bases, &vector, &bases_set, &MIN,
		 &MAX, &delta);
	    t_vector_size = MAX - MIN + 1;
	    t_MIN = MIN;
	    t_delta = delta;
	    t_check_set = sxba_calloc (t_vector_size + 1);

	    for (i = MIN, k = 1; i <= MAX; i++, k++)
		if (vector [i].check != 0)
		    SXBA_1_bit (t_check_set, k);

	    for (i = 1; i <= xw; i++) {
		if ((k = state_to_state [i]) != (l = witness [i])) {
		    t_bases [l].propre = t_bases [k].propre;
		    t_bases [l].commun = t_bases [k].commun;
		}
	    }

	    if (sxverbosep) {
		sxtime (SXFINAL, "done");
		sxttycol1p = true;
	    }
	}


/* Linearisation des NT_tables */
/* Creation des defauts */
/* Si un etat contient une seule action essentielle, elle est implantee
   directement dans (propre, commun) = (check, action ) */

	if (sxverbosep) {
	    fprintf (sxtty, "\t\tNT_tables ... ");
	    sxttycol1p = false;
	}

	nt_bases = (struct P_base*) sxcalloc (xe1 + 1, sizeof (struct P_base));


/* Apres extraction de l'action par defaut, 2 etats peuvent devenir identiques. */
/* On prend comme defaut l'adresse la plus populaire et en cas d'egalite la plus
   elevee. */ 

	{
	    register struct P_item	*aitem, *lim;
	    register struct state	*astate;
	    register int	i, k, l, nt;
	    register int	*occur_nb;
	    struct P_item	*pi;

	    occur_nb = (int*) sxcalloc (nt_fe_size + 1, sizeof (int));
	    xw = 0;

	    for (i = 1; i < xac2; i++) {
		if ((l = nt_states [i].lgth) > 0) {
		    register struct P_base	*abase = nt_bases + i;

		    for (lim = (aitem = nt_items + nt_states [i].start) + l; aitem < lim; aitem++) {
			if (aitem->action != 0) {
			    if ((k = ++(occur_nb [aitem->action])) > (nt = occur_nb [occur_nb [0]]) || k == nt && aitem->
				 action > occur_nb [0])
				occur_nb [0] = aitem->action;
			}
		    }

		    abase->defaut = k = occur_nb [0];
		    abase->propre = grand;
		    abase->commun = grand;
		    occur_nb [0] = 0;

		    for (aitem = nt_items + nt_states [i].start; aitem < lim; aitem++) {
			if (aitem->action != 0) {
			    occur_nb [aitem->action] = 0;

			    if (aitem->action == k)
				/* On conserve aitem->check pour algoV (mesure de la "size" d'un vecteur */
				aitem->action = 0;
			    else {
				(occur_nb [0])++;
				pi = aitem;
			    }
			}
		    }

		    if (occur_nb [0] == 0) {
			nt_states [i].lgth = 0;
		    }
		    else if (occur_nb [0] == 1) {
			abase->propre += pi->check;
			abase->commun += pi->action;
			nt_states [i].lgth = 0;
		    }
		    else {
			register struct row	*arowi = rows + i;

			state_to_state [++xw] = i;
			witness [xw] = i;
			arowi->lgth = 0;
			arowi->hash = 0;
			arowi->start = nt_states [i].start;

			for (aitem = nt_items + nt_states [i].start; aitem < lim; aitem++) {
			    if (aitem->action != 0) {
				(arowi->lgth)++;
				arowi->hash += aitem->check + aitem->action;
			    }
			}
		    }

		    occur_nb [0] = 0;
		}
	    }

	    sxfree (occur_nb);
	    ITEMS = nt_items;
	    equality_sort (state_to_state, 1, xw, row_le, row_eq);

	    for (i = 1; i <= xw; i++) {
		if (state_to_state [i] != (k = witness [i]))
		    nt_states [k].lgth = 0;
	    }

	    algoV (true, xac2, NTMAX, xnt_items, nt_states, nt_items, nt_bases, &vector, &bases_set, &MIN, &MAX, &delta);
	    nt_vector_size = MAX - MIN + 1 - t_vector_size;
	    t_delta = t_MIN - MIN + delta - t_delta;

	    for (i = 1; i <= xw; i++) {
		if ((k = state_to_state [i]) != (l = witness [i])) {
		    nt_bases [l].propre = nt_bases [k].propre;
		    nt_bases [l].commun = nt_bases [k].commun;
		}
	    }

	    sxfree (bases_set);
	}

	sxfree (witness);
	sxfree (state_to_state);

	if (sxverbosep) {
	    sxtime (SXFINAL, "done");
	    sxttycol1p = true;
	}
    }

    is_listing_opened = false;

    if (is_abstract || is_floyd_evans || is_long_listing)
	optim_lo (language_name, ME);

/* IDEE:
   Nouvelle organisation des ref qui representent les couples (etat, etq)
   Une seule valeur de ref qui permet d'indexer directement les [n]t_bases.
   Par valeur croissante des ref on trouve :
   - Les ref tels que etat==etq
   - les ref tels que etat!=0
   - les ref tels que etat==0
   Cette organisation necessite evidemment de dupliquer certaines lignes des
   [n]t_bases. Ex, si ref1 = (p, q1) et ref2 = (p, q2) alors la ligne p de
   t_bases sera dupliquee aux indices ref1 et ref2.
*/

    {
	/*   T A B L E S   O U T P U T   */

	register struct floyd_evans	*fe, *lim;
	struct P_base	*old_t_bases, *old_nt_bases;
	struct P_item	*old_vector;
	register int	i, j;
	int		minref, indx;
	int		M_eq;


	if (sxverbosep) {
	    fprintf (sxtty, "\tTables Output ... ");
	    sxttycol1p = false;
	}

	XxY_alloc (&ref_to_ee, "ref_to_ee", 2 * xac2, 4, 0, 0, NULL, NULL);

	for (lim = (fe = tnt_fe + 1) + tnt_lnks [0]; fe < lim; fe++) {
	    if (fe->codop == Shift || fe->codop == ScanShift) {
		if ((i = nt_state_equiv_class [fe->pspl]) > 0
		    && i == t_state_equiv_class [fe->next]) {
		    XxY_set (&ref_to_ee, i, i, &indx);
		}
	    }
	}

	M_eq = XxY_top (ref_to_ee);

	for (fe = tnt_fe + 1; fe < lim; fe++) {
	    if (fe->codop == Shift || fe->codop == ScanShift) {
		if ((i = nt_state_equiv_class [fe->pspl]) > 0
		    && i != (j = t_state_equiv_class [fe->next])) {
		    XxY_set (&ref_to_ee, i, j, &indx);
		}
	    }
	}

	Mntstate = XxY_top (ref_to_ee);

	for (fe = tnt_fe + 1; fe < lim; fe++) {
	    if (fe->codop == Shift || fe->codop == ScanShift) {
		if (nt_state_equiv_class [fe->pspl] == 0) {
		    XxY_set (&ref_to_ee, 0, t_state_equiv_class [fe->next], &indx);
		}
	    }
	}

	/* Place de l'etat initial */
	XxY_set (&ref_to_ee, 0, t_state_equiv_class [init_state], &indx);

	Mtstate = XxY_top (ref_to_ee);


	old_t_bases = t_bases;
	old_nt_bases = nt_bases;
	old_vector = vector;

	if (germe_to_gr_act != NULL) {
	    int *old_germe_to_gr_act = germe_to_gr_act;

	    mgerme = Mgerme = Mtstate;

	    /* On commence par les la_states dont le germe_to_gr_act est non nul. */
	    for (i = init_state + 1; i < xe1; i++) {
		if (germe_to_gr_act [i] != 0) {
		    Mgerme++;
		}
	    }

	    if (Mgerme > mgerme) {
		int action;

		germe_to_gr_act = (int*) sxalloc (Mgerme - mgerme, sizeof (int));

		for (i = init_state + 1; i < xe1; i++) {
		    if ((action = old_germe_to_gr_act [i]) != 0) {
			XxY_set (&ref_to_ee, 0, t_state_equiv_class [i], &indx);
			germe_to_gr_act [Mtstate - mgerme] = action;
			Mtstate++;
		    }
		}
	    }
	    else
		germe_to_gr_act = NULL;

	    for (i = init_state + 1; i < xe1 /* on les prend tous */; i++) {
		if (old_germe_to_gr_act [i] == 0) {
		    XxY_set (&ref_to_ee, 0, t_state_equiv_class [i], &indx);
		}
	    }

	    sxfree (old_germe_to_gr_act);
	    Mtstate = XxY_top (ref_to_ee);
	}
	else
	    mgerme = Mgerme = 0;

	{
	    register struct P_base	*abase;


/* On remplit t_bases et nt_bases */
/* On calcule la plus petite reference dans vector */

	    t_bases = (struct P_base*) sxalloc (Mtstate + 1, sizeof (struct P_base));
	    nt_bases = (struct P_base*) sxalloc (Mntstate + 1, sizeof (struct P_base));
	    minref = MAX;

	    for (i = 1, abase = t_bases + 1; i <= Mtstate; i++, abase++) {
		*abase = old_t_bases [XxY_Y (ref_to_ee, i)];		

		if ((j = abase->commun) < grand /* reference dans vector */ ) {
		    minref = min (minref, j);

		    if ((j = abase->propre) != 0)
			minref = min (minref, j);
		}
	    }

	    for (i = 1, abase = nt_bases + 1; i <= Mntstate; i++, abase++) {
		*abase = old_nt_bases [XxY_X (ref_to_ee, i)];		

		if ((j = abase->commun) < grand /* reference dans vector */ ) {
		    minref = min (minref, j);

		    if ((j = abase->propre) != 0)
			minref = min (minref, j);
		}
	    }

	    sxfree (old_nt_bases);
	    sxfree (old_t_bases);
	}


/* On remplit vector */

	Mvec = MAX - MIN + 1;
	vector = (struct P_item*) sxcalloc (Mvec + 1, sizeof (struct P_item));

	for (i = MIN, j = 1; i <= MAX; i++, j++)
	    vector [j] = old_vector [i];

	sxfree (old_vector);

/* On remplit prdcts */

	Mprdct = Mred;

	if (xprdct_items > 0)
	    for (lim = (fe = tnt_fe + (i = 1)) + tnt_lnks [0]; fe < lim; fe++, i++)
		if (fe->codop == Prdct || fe->codop == NonDeter) {
		    tnt_fe_to_new [i] = -(++Mprdct);
		    Mprdct += predicates [fe->next].lgth - 1;
		}

	mMprdct = -Mprdct;

	if (germe_to_gr_act != NULL && is_non_deterministic_automaton) {
	    /* Il y a des ARCs */
	    /* Dans le cas deterministe germe_to_gr_act[germe] est un indice dans t_fe vers un
	       reduce (deja range dans tnt_fe et reductions). On modifie germe_to_gr_act
	       afin qu'il reference reductions (en negatif car il n'y a pas de scan)
	       (le traitement a deja ete effectue') */
	    /* Dans le cas non deterministe germe_to_gr_act[germe] est un indice vers le debut
	       de la sequence non deterministe des actions du germe encore executables dans
	       le sous ARC de racine i. (traite' plus loin) */
	    for (i = Mgerme - mgerme - 1; i >= 0; i--) {
		germe_to_gr_act [i] = tnt_fe_to_new [germe_to_gr_act [i]];
	    }
	}

	Mfe = Mprdct + M_eq;
	mMfe = -Mfe;
	M0ref = Mprdct + Mntstate;
	Mref = Mprdct + Mtstate;
	deltavec = Mref + MIN - (delta + minref);


/* On complete tnt_fe_to_new */

	for (lim = (fe = tnt_fe + (i = 1)) + tnt_lnks [0]; fe < lim; fe++, i++) {
	    int k;

	    if (tnt_fe_to_new [i] == 0) {
		switch (fe->codop) {
		case LaScanShift:
		case ScanShift:
		case Shift:
		    j = XxY_is_set (&ref_to_ee,
				    nt_state_equiv_class [fe->pspl],
				    t_state_equiv_class [fe->next]);

		    if (fe->codop == LaScanShift)
			j += Mref;
		    else
			j += Mprdct;

		    tnt_fe_to_new [i] = (fe->codop == ScanShift) ? j : -j;

		default:
		    break;
		}
	    }
	}

	{
	    register struct P_base	*abase;


/* Modifications de t_bases */

	    for (i = 1, abase = t_bases + 1; i <= Mtstate; i++, abase++) {
		abase->defaut = (abase->defaut == t_error) ? 0 : tnt_fe_to_new [abase->defaut];
		abase->propre = ((j = abase->propre) >= grand /* codage direct */ ) ? j - grand : j - minref + Mref + 1;
		abase->commun = ((j = abase->commun) >= grand /* codage direct */ ) ? (((j -= grand) == 0) ? 0 :
		     tnt_fe_to_new [j]) : j - minref + Mref + 1;
	    }


/* Modifications de nt_bases */

	    for (i = 1, abase = nt_bases + 1; i <= Mntstate; i++, abase++) {
		abase->defaut = tnt_fe_to_new [abase->defaut];
		abase->propre = ((j = abase->propre) >= grand /* codage direct */ ) ? j - grand : j - minref + Mref + 1;
		abase->commun = ((j = abase->commun) >= grand /* codage direct */ ) ? (((j -= grand) == 0) ? 0 :
		     tnt_fe_to_new [j]) : j - minref + Mref + 1;
	    }
	}

	{
	    register struct P_reduction		*ared;


/* Modifications de reductions */

	    for (ared = reductions + (i = 1); i <= Mred; i++, ared++) {
		j = nt_to_ad [ared->lhs];
		ared->lhs = (j == -1) ? Mref + nt_to_nt [ared->lhs] : tnt_fe_to_new [j];
		ared->action = bnf_ag.WS_NBPRO [ared->reduce].action;
	    }
	}

	{
	    register struct P_item	*avec;
	    register int	k;


/* Modifications de vector */

	    for (i = 1, avec = vector + 1; i <= Mvec; i++, avec++) {
		if ((j = avec->action) != 0)
		    avec->action = tnt_fe_to_new [j];
	    }
	}

	if (xprdct_items > 0) {
	    register struct P_prdct	*aprdct;


/* Modifications de prdcts */

	    for (i = 1, aprdct = prdcts + 1; i <= xprdct_items; i++, aprdct++) {
		aprdct->action = tnt_fe_to_new [aprdct->action];
	    }
	}

	prdct_list_top = 0;
            
	if (is_non_deterministic_automaton
	    && ((prdct_list_size = bnf_ag.WS_TBL_SIZE.xtmax + bnf_ag.WS_TBL_SIZE.tmax
		+ bnf_ag.WS_TBL_SIZE.xntmax - bnf_ag.WS_TBL_SIZE.ntmax) > 0)) {
/* Creation de prdct_list. */
/* Permet, a l'analyse (par dichotomie), de retrouver un predicat a partir
   du couple (red_no, position). */
	    int	item, lim, v, red_no, pos, prdct;

	    prdct_list = (struct P_prdct_list*) sxalloc (prdct_list_size + 1,
							   sizeof (struct P_prdct_list));
	    for (lim = bnf_ag.WS_TBL_SIZE.indpro, item = 1;
		 item < lim;
		 item++) {
		v = bnf_ag.WS_INDPRO [item].lispro;

		if (v > bnf_ag.WS_TBL_SIZE.ntmax || v < bnf_ag.WS_TBL_SIZE.tmax) {
		    /* un predicat */
		    red_no = bnf_ag.WS_INDPRO [item].prolis;
		    pos = item - bnf_ag.WS_NBPRO [red_no].prolon + 1;
		    prdct = (v > 0) ? XNT_TO_PRDCT_CODE (v) : XT_TO_PRDCT_CODE (-v);

		    if (++prdct_list_top > prdct_list_size)
			prdct_list = (struct P_prdct_list*)
			    sxrealloc (prdct_list,
				       (prdct_list_size *= 2) + 1,
				       sizeof (struct P_prdct_list));

		    prdct_list [prdct_list_top].red_no = red_no;
		    prdct_list [prdct_list_top].pos = pos;
		    prdct_list [prdct_list_top].prdct = prdct;
		}
	    }
	}
	else
	  prdct_list = NULL;  
	    

	if (is_listing_opened) {
	    if (is_long_listing)
		optim_ll ();

	    if (is_abstract || is_floyd_evans || is_long_listing)
		optim_sizes ();
	}

	parser_ag.bnf_modif_time = bnf_ag.bnf_modif_time;
	parser_ag.constants.tmax = -bnf_ag.WS_TBL_SIZE.tmax;
	parser_ag.constants.xtmax = bnf_ag.WS_TBL_SIZE.xtmax;
	parser_ag.constants.nbpro = bnf_ag.WS_TBL_SIZE.nbpro;
	parser_ag.constants.xnbpro = bnf_ag.WS_TBL_SIZE.xnbpro;
	parser_ag.constants.ntmax = NTMAX;
	parser_ag.constants.Mtstate = Mtstate;
	parser_ag.constants.Mntstate = Mntstate;
	parser_ag.constants.Mvec = Mvec;
	parser_ag.constants.mgerme = mgerme;
	parser_ag.constants.Mgerme = Mgerme;
	parser_ag.constants.deltavec = deltavec;
	parser_ag.constants.init_state =
	    XxY_is_set (&ref_to_ee, 0, t_state_equiv_class [init_state]);
	parser_ag.constants.final_state =
	    XxY_is_set (&ref_to_ee, 0, t_state_equiv_class [final_state]);
	parser_ag.constants.mMprdct = mMprdct;
	parser_ag.constants.mMred = mMred;
	parser_ag.constants.mMrednt = mMrednt;
	parser_ag.constants.Mprdct = Mprdct;
	parser_ag.constants.Mfe = Mfe;
	parser_ag.constants.M0ref = M0ref;
	parser_ag.constants.Mref = Mref;
	parser_ag.constants.nd_degree = nd_degree;
	parser_ag.constants.Mprdct_list = prdct_list_top;
	parser_ag.constants.sem_kind = bnf_ag.WS_TBL_SIZE.sem_kind;
	parser_ag.reductions = reductions;
	parser_ag.t_bases = t_bases;
	parser_ag.nt_bases = nt_bases;
	parser_ag.vector = vector;
	parser_ag.prdcts = prdcts;
	parser_ag.germe_to_gr_act = germe_to_gr_act;
	parser_ag.prdct_list = prdct_list;

	parser_write (&parser_ag, language_name);

	parser_free (&parser_ag);
	sxfree (tnt_fe_to_new), tnt_fe_to_new = NULL;

	XxY_free (&ref_to_ee);

	if (sxverbosep) {
	    sxtime (SXFINAL, "done");
	    sxttycol1p = true;
	}
    }

    sxfree (t_check_set), t_check_set = NULL;
    sxfree (rows), rows = NULL;
    sxfree (nt_to_nt), nt_to_nt = NULL;
    sxfree (nt_to_ad), nt_to_ad = NULL;
    sxfree (nt_state_equiv_class), nt_state_equiv_class = NULL;
    sxfree (t_state_equiv_class), t_state_equiv_class = NULL;
    sxfree (single_prod);
    sxfree (nt_items), nt_items = NULL;
    sxfree (nt_states), nt_states = NULL;
    sxfree (tnt_lnks), tnt_lnks = NULL;
    sxfree (tnt_fe), tnt_fe = NULL;
    sxfree (t_items), t_items = NULL;
    sxfree (t_states), t_states = NULL;

    if (predicates != NULL)
	sxfree (predicates), predicates = NULL;

    if (is_listing_opened) {
	is_listing_opened = false;
	put_edit_finalize ();
    }

    free_alpha_table ();

    return 0;
}
