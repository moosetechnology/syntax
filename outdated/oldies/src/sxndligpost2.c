/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1994 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */

/* Execute les actions et predicats syntaxiques produits a partir d'une
   Linear Indexed Grammar par le traducteur lig2bnf dans le cas d'une
   analyse non deterministe. */
/* Cette version NE CALCULE PAS LES PILES EXPLICITEMENT.
   Chaque pile vide initiale est mise en relation avec ses piles vides finales
   correspondantes
   Chaque empilage de l'objet "a" est mis en correspondance avec les depilages
   correspondants
   Chaque depilage de l'objet "a" est mis en correspondance avec les empilages
   correspondants . */
/* la foret partagee est elaguee en fonction de la relation precedente. */
/* Cette version ne detecte pas les cycles dans la grammaire. */

/*
  Definition d'une foret partagee SF pour une grammaire G = (N, T, P, S) en forme normale de Chomsky
  pour un texte source a1 ... an

  SF = (R, H)

  R = { <rp, [i, j, k]> = A [i..j] B [j..k] | rp : L = A B ; et 0 <= i <= j <= k <= n+1}

  H = { A [i .. k] | A dans N, 0 <= i <= k <= n+1}

  A [i .. k] = { <rp, [i, j, k]> | LHS (rp) == A et i <= j <= k}

  Taille de SF :

  |R| = O(n^3)
  |H| = O(n^2)
  |A [i .. k]| = O(n)

  Piles (bottom-up) :

  PI (<rp, [i, j, k]>) = (@p, &p) PI (A [i..j])   (cas ou A est PRIMAIRE dans rp)
  PI (A [i..k]) = U PI (<rp, [i, j, k]>)

  En fait les piles figurent implicitement dans la foret partagee

  On definit egalement les piles top-down

  Un element r = <rp, [i, j, k]> de R est valide ssi l'intersection des piles top-down et
  bottom-up de r est non vide (et correspondent a des piles valides i.e. verifiee par les
  predicats et pas d'underflow).

  Regarder la complexite de cette evaluation et la composition de sequences (@p, &p)

  Quel est le nb d'occurrence d'un A [i..j] (pour i et j fixe') en rhs des R : O(n)
  C'est le nombre de piles associe a A [i..j] pour la passe top-down

  Remarque: On peut invalider tout un "arbre" de pile sans avoir a les calculer s'il
  y a incompabilite entre les actions/predicats de leurs suffixes.
*/

/*
   A FAIRE :
   Appels des post_action et post_prdct.
*/  

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030507 09:09 (phd):	Correction d'un "==" grâce à SGI	*/
/************************************************************************/
/* 18-01-95 14:20 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)sxndligpost2.c\t- SYNTAX [unix] -  7 mai 2003";

#if 0
static char	ME [] = "sxndligpost2";
#endif

#include 	"sxnd.h"
#include 	"sxndlig_post2.h"

extern int      sxndlig_post_do_it ();
extern int      sxndligpost (int which, SXTABLES *arg);

#ifdef EBUG
# define stdout_or_NULL	stdout
#else
# define stdout_or_NULL	NULL
#endif

struct sxndlig_common	sxndlig_common; /* Global */

static struct sxndlig	sxndlig;


static  void
memo_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.memo2attr = (struct memo2attr*) sxrealloc (sxndlig.memo2attr,
						       new_size + 1,
						       sizeof (struct memo2attr));
}


/*
   On traite :

   ?           -> ... <A> ( p1 a b c) ...
   <A> ( p2 c) -> ... <B> ( p3 d e ) ...

   ou prdct_no = ( p1 a b c) et act_no = ( p2 c)
   p1, p2 et p3 designent les prefixes des piles. Par definition p2 == p3

   On calcule trois types de relations qui caracterise le passage de p1 a p2 (ou p3)

   La relation "<" qui indique que le passage de p1 a p2 a necessite un (ou des) push
   La relation "=" qui indique que le passage de p1 == p2
   La relation ">" qui indique que le passage de p1 a p2 a necessite un (ou des) pop

   Ces relations sont caracterisees par un entier relatif kind avec la sigification:

   kind		<0	0	>0
   rel		<	=	>

   de plus |kind| indique le nombre de pop ou de push

   Bien sur ces relations ne sont vraies que si Predicat et action sont coherents.

   Attention entre 2 items i et j il peut y avoir PLUSIEURS relations.
*/

static bool
perform_action_prdct (prdct_no, act_no, kind)
    int	prdct_no, act_no, *kind;
{
    int			push_nb, pop_nb, memo;
    int			*pc, *pclim, *pprdct_bot, *pprdct_top;
    bool		prdct_secondary, act_secondary, is_memo;
    struct memo2attr	*pattr;

    sxinitialise(act_secondary);/* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(pclim); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(pc); /* pour faire taire "gcc -Wuninitialized" */

    is_memo = XxY_set (&sxndlig.memo, prdct_no, act_no, &memo);
    pattr = sxndlig.memo2attr + memo;
    
    if (is_memo)
    {
	*kind = pattr->kind;
    }
    else
    {
	pattr->kind = *kind = 0;

	if (act_no == 0)
	    push_nb = 0;
	else
	{
	    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no];

	    if (!(act_secondary = (*pc == SECONDARY)))
		pc += 3;

	    pclim = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no + 1] - 2;

	    push_nb = pclim - pc;
	}

	if (prdct_no == 0)
	    return pattr->result = push_nb == 0;

	pprdct_bot = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no];

	if (!(prdct_secondary = (*pprdct_bot == SECONDARY)))
	    pprdct_bot += 3;

	pprdct_top = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no + 1] - 2;

	pop_nb = pprdct_top - pprdct_bot;

	if (act_no == 0)
	    return pattr->result = pop_nb == 0;

	while (pprdct_top > pprdct_bot && pclim > pc)
	{
	    if (*pprdct_top-- != *pclim--)
		return pattr->result = false;
	}

	if (prdct_secondary && act_secondary)
	    return pattr->result = pop_nb == push_nb;

	pattr->kind = *kind = push_nb - pop_nb;
    }

    return pattr->result = true;
}


static bool
check_action_prdct (prdct_no, act_no, kind)
    int	prdct_no, act_no, kind;
{
    /* prdct_no et act_no sont non_vides (donc non nuls) */
    int		*pc, *pclim, *pprdct_bot, *pprdct_top;
    bool	prdct_secondary, act_secondary;

#ifdef EBUG
    if (prdct_no == 0 || act_no == 0)
	sxtrap (ME, "check_action_prdct");
#endif
    
    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no];
    pprdct_bot = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no];

    if (!(act_secondary = (*pc == SECONDARY)))
	pc += 3;

    if (!(prdct_secondary = (*pprdct_bot == SECONDARY)))
	pprdct_bot += 3;

    if ((prdct_secondary && !act_secondary) || (!prdct_secondary && act_secondary))
	return false;

    pclim = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no + 1] - 2;
    pprdct_top = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no + 1] - 2;


    if (kind >=	0)
	pc += kind;
    else
	pprdct_bot += -kind;

    while (++pc <= pclim && ++pprdct_bot <= pprdct_top)
    {
	if (*pc != *pprdct_bot)
	    return false;
    }

    return true;
}




static bool
compose (left_item, right_item, kind)
    int left_item, right_item, kind;
{
    /* On a left_item <+ item et item >+ right_item
       On regarde s'il y a compatibilite'. */
    int 	prdct_no, act_no;

    prdct_no = sxndlig.grammar2attr [left_item].prdct_no - 10000;
    act_no = sxndlig.rule2attr [parse_stack.grammar [right_item].lhs_rule].act_no - 10000;

    return check_action_prdct (prdct_no, act_no, kind);
}




static bool
fill_Rk (Rk, left_item, right_item, left_kind, right_kind)
    struct R	*Rk;
    int left_item, right_item, left_kind, right_kind;
{
    int 		indice, kind = left_kind + right_kind;
    bool		do_graph = false;
    struct R_attr 	*pattr;

    if (left_item == right_item)
    {
	if (!sxndlig.is_cyclic)
	{
	    sxndlig.is_cyclic = true;
	    fputs ("Warning: This grammar is cyclic.\n", sxtty);
	}

	return false;
    }

    if (left_kind < 0 && right_kind > 0)
    {
	/* On a left_item <  <>* > right_item */
	if (!compose (left_item, right_item, kind))
	    return false;

	/* Si kind == 0 On fabrique left_item <>+ right_item. */
	if (kind <= 0)
	{
	    /* On fabrique left_item <  <>+ right_item. */
	    /* On vient de verifier (partiellement si kind != 0) une relation <> */
	    SXBA_0_bit (sxndlig.sup_final_set, right_item);
	}

	if (kind >= 0)
	{
	    /* On fabrique left_item <>+ > right_item. */
	    /* On vient de verifier (partiellement si kind != 0) une relation <> */
	    SXBA_0_bit (sxndlig.inf_orig_set, left_item);
	}
    }

    if (kind == 0)
    {
	if (SXBA_bit_is_set (sxndlig.orig_set, left_item) &&
	    SXBA_bit_is_set (sxndlig.final_set, right_item))
	{
	    /* <>+ entre un predicat secondaire et une action secondaire non encore verifies. */
	    SXBA_0_bit (sxndlig.valid_set, left_item);
	    SXBA_0_bit (sxndlig.valid_set, right_item);
	}
    }

    pattr = &(Rk->right_attr [left_item]);

    if (SXBA_bit_is_reset_set (Rk->left_item_set, left_item))
    {
	/* Premiere occurrence de left_item */
	pattr->item = right_item;
	pattr->kind = kind;
    }
    else if (pattr->kind != kind || (pattr->item & LIG_7F) != (unsigned int)right_item)
    {
	pattr->item |= LIG_80;
	do_graph = true;
    }

    pattr = &(Rk->left_attr [right_item]);

    if (SXBA_bit_is_reset_set (Rk->right_item_set, right_item))
    {
	/* Premiere occurrence de right_item */
	pattr->item = left_item;
	pattr->kind = kind;
    }
    else if (pattr->kind != kind || (pattr->item & LIG_7F) != (unsigned int)left_item)
    {
	pattr->item |= LIG_80;
	do_graph = true;
    }

    if (do_graph)
    {
	XxYxZ_set (&Rk->graph, left_item, kind, right_item, &indice);
    }

    return true; /* Ajoute' au pif le 14/05/2003 */
}



static bool
fill_trans_k (Rkm1, Rk)
    struct R	*Rkm1, *Rk;
{
    /* On fabrique Rk a partir de R1 et Rkm1. */
    int		item, rindice, rkind, lkind, left_item, right_item, lindice;
    int		litem, ritem, left_kind, right_kind;
    bool	is_new = false, is_right_foreach, is_left_foreach, is_left, is_right;
    SXBA	middle_set;

#ifdef EBUG
    sxndlig.fill_trans_k_called_nb++;
#endif

    if (Rkm1 == &sxndlig.R1)
	/* Calcul de trans_2 */
	/* On ne doit pas modifier "R1" */
	/* On suppose Rk != R3 on utilise R3.left_item_set. */
	middle_set = sxba_copy (sxndlig.R3.left_item_set, sxndlig.R1.left_item_set);
    else
	middle_set = Rkm1->left_item_set;

    sxba_and (middle_set, sxndlig.R1.right_item_set);

    item = 0;

    while ((item = sxba_scan (middle_set, item)) > 0)
    {
	right_item = Rkm1->right_attr [item].item;
	right_kind = Rkm1->right_attr [item].kind;
	left_item = sxndlig.R1.left_attr [item].item;
	left_kind = sxndlig.R1.left_attr [item].kind;

	if ((is_right_foreach = ((right_item & LIG_80) != 0)))
	    right_item &= LIG_7F;

	if ((is_left_foreach = ((left_item & LIG_80) != 0)))
	    left_item &= LIG_7F; 

	if (is_right_foreach)
	{
	    XxYxZ_Xforeach (Rkm1->graph, item, rindice)
	    {
		if ((ritem = XxYxZ_Z (Rkm1->graph, rindice)) == right_item)
		    is_right = true;

		if ((rkind = XxYxZ_Y (Rkm1->graph, rindice)) >= 0 /* > ou = */)
		{
		    if (is_left_foreach)
		    {
			XxYxZ_Zforeach (sxndlig.R1.graph, item, lindice)
			{
			    if ((litem = XxYxZ_X (sxndlig.R1.graph, lindice)) == left_item)
				is_left = true;

			    if ((lkind = XxYxZ_Y (sxndlig.R1.graph, lindice)) <= 0 /* < ou = */ &&
				fill_Rk (Rk, litem, ritem, lkind, rkind))
				is_new = true;
			}
		    }
		    else
			is_left = false;

		    if (!is_left &&
			left_kind <= 0 /* < ou = */ &&
			fill_Rk (Rk, left_item, ritem, left_kind, rkind))
			is_new = true;
		}
	    }
	}
	else
	    is_right = false;

	if (!is_right && right_kind >= 0 /* > ou = */)
	{
	    if (is_left_foreach)
	    {
		XxYxZ_Zforeach (sxndlig.R1.graph, item, lindice)
		{
		    if ((litem = XxYxZ_X (sxndlig.R1.graph, lindice)) == left_item)
			is_left = true;

		    if ((lkind = XxYxZ_Y (sxndlig.R1.graph, lindice)) <= 0 /* < ou = */ &&
			fill_Rk (Rk, litem, right_item, lkind, right_kind))
			is_new = true;
		}
	    }
	    else
		is_left = false;

	    if (!is_left
		&& left_kind <= 0 /* < ou = */ &&
		fill_Rk (Rk, left_item, right_item, left_kind, right_kind))
		is_new = true;
	}
    }

    if (Rkm1 == &sxndlig.R1)
    {
	/* Calcul de trans_2 */
	sxba_empty (sxndlig.R3.left_item_set);
	return is_new;
    }

    sxba_and (Rkm1->right_item_set, sxndlig.R1.left_item_set);

    item = 0;

    while ((item = sxba_scan (Rkm1->right_item_set, item)) > 0)
    {
	left_item = Rkm1->left_attr [item].item;
	left_kind = Rkm1->left_attr [item].kind;
	right_item = sxndlig.R1.right_attr [item].item;
	right_kind = sxndlig.R1.right_attr [item].kind;

	if ((is_left_foreach = ((left_item & LIG_80) != 0)))
	    left_item &= LIG_7F;

	if ((is_right_foreach = ((right_item & LIG_80) != 0)))
	    right_item &= LIG_7F; 

	if (is_left_foreach)
	{
	    XxYxZ_Zforeach (Rkm1->graph, item, lindice)
	    {
		if ((litem = XxYxZ_Z (Rkm1->graph, lindice)) == left_item)
		    is_left = true;

		if ((lkind = XxYxZ_Y (Rkm1->graph, lindice)) <= 0 /* < ou = */)
		{
		    if (is_right_foreach)
		    {
			XxYxZ_Xforeach (sxndlig.R1.graph, item, rindice)
			{
			    if ((ritem = XxYxZ_X (sxndlig.R1.graph, rindice)) == right_item)
				is_right = true;

			    if ((rkind = XxYxZ_Y (sxndlig.R1.graph, rindice)) >= 0 /* > ou = */ &&
				fill_Rk (Rk, litem, ritem, lkind, rkind))
				is_new = true;
			}
		    }
		    else
			is_right = false;

		    if (!is_right &&
			right_kind >= 0 /* > ou = */ &&
			fill_Rk (Rk, litem, right_item, lkind, right_kind))
			is_new = true;
		}
	    }
	}
	else
	    is_left = false;

	if (!is_left && left_kind <= 0 /* < ou = */)
	{
	    if (is_right_foreach)
	    {
		XxYxZ_Xforeach (sxndlig.R1.graph, item, rindice)
		{
		    if ((ritem = XxYxZ_X (sxndlig.R1.graph, rindice)) == right_item)
			is_right = true;

		    if ((rkind = XxYxZ_Y (sxndlig.R1.graph, rindice)) >= 0 /* > ou = */ &&
			fill_Rk (Rk, left_item, ritem, left_kind, rkind))
			is_new = true;
		}
	    }
	    else
		is_right = false;

	    if (!is_right &&
		right_kind >= 0 /* > ou = */ &&
		fill_Rk (Rk, left_item, right_item, left_kind, right_kind))
		is_new = true;
	}
    }

    return is_new;
}


static bool
fill_trans (item, prdct_no, rhs_rule_no)
    int item, prdct_no, rhs_rule_no;
{
    int next_item, lhs_symb, reduce, *pprdct_no, act_no, kind, *pc;

    next_item = parse_stack.rule [rhs_rule_no].hd;
    lhs_symb = parse_stack.grammar [next_item].rhs_symb;
    reduce = XxY_X (parse_stack.symbols, lhs_symb);
#if 0
/* Le 14/4/95 le 1er champ de symbols devient un reduce (au lieu d'une ref) */
    red_no = XxY_X (parse_stack.symbols, lhs_symb); /* red_no >= 0 */

    /* if (red_no > sxplocals.SXP_tables.Mred)
       red_no -= sxplocals.SXP_tables.Mred;
       est inutile (il n'y a plus de regles tronquees). */

    reduce = (red_no == 0) ? 0 : sxplocals.SXP_tables.reductions [red_no].reduce;
#endif
    pprdct_no = sxndlig_common.code.map_code + sxndlig_common.code.map_disp [reduce];

    act_no = *pprdct_no;

#ifdef EBUG
    if (act_no < 10000)
	sxtrap (ME, "fill_trans");
#endif
	      
    prdct_no -= 10000;
    act_no -= 10000;

    if (perform_action_prdct (prdct_no, act_no, &kind))
    {
	if (sxndlig_common.code.prdct_or_act_code [sxndlig_common.code.prdct_or_act_disp [prdct_no]] == SECONDARY)
	{
	    /* On note les origines... */
	    SXBA_1_bit (sxndlig.orig_set, item);
	}

	pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no];

	if (*pc == SECONDARY)
	{
	    /* fin : on pointe sur la "LHS" */
	    /* ...et les fins */
	    SXBA_1_bit (sxndlig.final_set, next_item);
	}
	else
	{
	    next_item += 1 + pc [1];
	    /* On pointe sur l'item du predicat primaire de la RHS */
	}

	if (kind < 0)
	{
	    /* On note les origines... */
	    SXBA_1_bit (sxndlig.inf_orig_set, item);
	    fill_Rk (&sxndlig.R1, item, next_item, kind, 0);
	}
	else if (kind > 0)
	{
	    /* On note les extremites... */
	    SXBA_1_bit (sxndlig.sup_final_set, next_item);
	    fill_Rk (&sxndlig.R1, item, next_item, 0, kind);
	}
	else
	    fill_Rk (&sxndlig.R1, item, next_item, 0, 0);

	return true;
	
    }

    return false;
}



static int
sxndlig_post2_do_it ()
{
    static int	graph_foreach [] = {1, 0, 1, 0, 0, 0};

    struct R	*pR;
    bool	not_done, checked;

    int 	lhs_rule_no, rule_lgth, item, lhs_symb, reduce, *pprdct_no, lim, indice, rhs_rule_no;
    int		prdct_no, next_item, next_rule_lgth, next_lim;
    bool 	ret_val;

    sxndlig.R0_size = parse_stack.rule [parse_stack.hook_rule - 1].hd +
	parse_stack.rule [parse_stack.hook_rule - 1].lgth;

    sxndlig.working_sets = sxbm_calloc (11, sxndlig.R0_size);

    sxndlig.orig_set = sxndlig.working_sets [0];
    sxndlig.final_set = sxndlig.working_sets [1];
    sxndlig.R1.left_item_set = sxndlig.working_sets [2];
    sxndlig.R1.right_item_set = sxndlig.working_sets [3];
    sxndlig.Rkm1 = &(sxndlig.R2);
    sxndlig.R2.left_item_set = sxndlig.working_sets [4];
    sxndlig.R2.right_item_set = sxndlig.working_sets [5];
    sxndlig.Rk = &(sxndlig.R3);
    sxndlig.R3.left_item_set = sxndlig.working_sets [6];
    sxndlig.R3.right_item_set = sxndlig.working_sets [7];
    sxndlig.inf_orig_set = sxndlig.working_sets [8];
    sxndlig.sup_final_set = sxndlig.working_sets [9];
    sxndlig.valid_set = sxndlig.working_sets [10];

    sxndlig.rule2attr = (struct rule2attr *) sxcalloc ((SXUINT)parse_stack.rule_top + 1, sizeof (struct rule2attr));
    sxndlig.grammar2attr = (struct grammar2attr *) sxalloc (parse_stack.grammar_top + 1,
							    sizeof (struct grammar2attr));


    XxYxZ_alloc (&sxndlig.R1.graph, "sxndlig.R1.graph", parse_stack.grammar_top, 1,
		 graph_foreach, NULL, stdout_or_NULL);

    XxYxZ_alloc (&sxndlig.R2.graph, "sxndlig.R2.graph", parse_stack.grammar_top, 1,
		 graph_foreach, NULL, stdout_or_NULL);

    XxYxZ_alloc (&sxndlig.R3.graph, "sxndlig.R3.graph", parse_stack.grammar_top, 1,
		 graph_foreach, NULL, stdout_or_NULL);


    sxndlig.R1.left_attr = (struct R_attr*) sxalloc (sxndlig.R0_size, sizeof (struct R_attr));
    sxndlig.R1.right_attr = (struct R_attr*) sxalloc (sxndlig.R0_size, sizeof (struct R_attr));
    
    sxndlig.R2.left_attr = (struct R_attr*) sxalloc (sxndlig.R0_size, sizeof (struct R_attr));
    sxndlig.R2.right_attr = (struct R_attr*) sxalloc (sxndlig.R0_size, sizeof (struct R_attr));
    
    sxndlig.R3.left_attr = (struct R_attr*) sxalloc (sxndlig.R0_size, sizeof (struct R_attr));
    sxndlig.R3.right_attr = (struct R_attr*) sxalloc (sxndlig.R0_size, sizeof (struct R_attr));

    /* sizeof (sxndlig_common.code.prdct_or_act_disp) est inaccessible... */
    XxY_alloc (&sxndlig.memo, "sxndlig.memo", sxplocals.SXP_tables.Mred + 1, 1,
	       0, 0, memo_oflw, stdout_or_NULL);

    sxndlig.memo2attr = (struct memo2attr*) sxalloc (XxY_size (sxndlig.memo) + 1,
						     sizeof (struct memo2attr));

    for (lhs_rule_no = 1; lhs_rule_no < parse_stack.hook_rule; lhs_rule_no++)
    {
	if ((rule_lgth = parse_stack.rule [lhs_rule_no].lgth) > 0 && lhs_rule_no != parse_stack.root)
	{
	    item = parse_stack.rule [lhs_rule_no].hd;
	    lhs_symb = parse_stack.grammar [item].rhs_symb;
	    reduce = XxY_X (parse_stack.symbols, lhs_symb);
#if 0
	    /* Le 14/4/95 le 1er champ de symbols devient un reduce (au lieu d'une ref) */
	    red_no = XxY_X (parse_stack.symbols, lhs_symb); /* red_no >= 0 */

	    /* if (red_no > sxplocals.SXP_tables.Mred)
	       red_no -= sxplocals.SXP_tables.Mred;
	       est inutile (il n'y a plus de regles tronquees). */

	    reduce = (red_no == 0) ? 0 : sxplocals.SXP_tables.reductions [red_no].reduce;
#endif
	    pprdct_no = sxndlig_common.code.map_code + sxndlig_common.code.map_disp [reduce];

	    sxndlig.rule2attr [lhs_rule_no].act_no = *pprdct_no;
	    lim = item + rule_lgth;
	    indice = 0;

	    while (++item < lim)
	    {
		indice++;

		if ((rhs_rule_no = parse_stack.grammar [item].rhs_symb) > 0)
		{
		    /* Non terminal */
		    /* On recupere le predicat associe */
		    prdct_no = pprdct_no [indice];
		    sxndlig.grammar2attr [item].prdct_no = prdct_no;

		    if (prdct_no >= 10000)
		    {
			if (rhs_rule_no < parse_stack.hook_rule)
			{
			    if (!fill_trans (item, prdct_no, rhs_rule_no))
				CTRL_PUSH (parse_stack.erased, lhs_rule_no);
			}
			else if ((next_rule_lgth = parse_stack.rule [rhs_rule_no].lgth) > 0)
			{
			    next_item = parse_stack.rule [rhs_rule_no].hd;
			    checked = false;
			    next_lim = next_item + next_rule_lgth;

			    while (++next_item < next_lim)
				if (fill_trans (item, prdct_no, parse_stack.grammar [next_item].rhs_symb))
				    checked = true;

			    if (!checked)
				CTRL_PUSH (parse_stack.erased, lhs_rule_no);
			}	
		    }
		}
	    }
	}
    }

    /* les hooks ont deja ete traites */

    sxba_or (sxba_copy (sxndlig.valid_set, sxndlig.orig_set), sxndlig.final_set);

    /* Calcul de trans_2 */
    not_done = fill_trans_k (&sxndlig.R1, sxndlig.Rkm1);

    if (not_done)
	/* Calcul de trans_3 */
	not_done = fill_trans_k (sxndlig.Rkm1, sxndlig.Rk);
    else
	sxndlig.Rk = sxndlig.Rkm1;

    while (not_done)
    {
	pR = sxndlig.Rkm1;
	sxba_empty (pR->left_item_set);
	sxba_empty (pR->right_item_set);
	XxYxZ_clear (&pR->graph);
	sxndlig.Rkm1 = sxndlig.Rk;
	sxndlig.Rk = pR;

	/* Calcul de trans_k */
	not_done = fill_trans_k (sxndlig.Rkm1, sxndlig.Rk);
    }
    
#ifdef EBUG
    fprintf (sxtty, "Fixed point reached after #%i iterations.\n", sxndlig.fill_trans_k_called_nb);
#endif

    /* CNS de validite : */
    /* Tous les b des "a < b" doivent etre antecedents par =* d'un c > d */
    /* Tous les c des "c > d" doivent etre successeurs par =* d'un a < b */
    /* Tous les items a reperant des predicats secondaires doivents etre lies
       par "=+" a un item reperant une action secondaire. */
    /* Tous les items a reperant des actions secondaires doivents etre lies
       par "=+" a un item reperant un predicat secondaire. */

    sxba_or (sxndlig.valid_set, sxndlig.inf_orig_set);
    sxba_or (sxndlig.valid_set, sxndlig.sup_final_set);

    item = 0;

    while ((item = sxba_scan (sxndlig.valid_set, item)) > 0)
    {
	lhs_rule_no = parse_stack.grammar [item].lhs_rule;
	CTRL_PUSH (parse_stack.erased, lhs_rule_no);
    }

    sxbm_free (sxndlig.working_sets);
    XxYxZ_free (&sxndlig.R1.graph);
    XxYxZ_free (&sxndlig.R2.graph);
    XxYxZ_free (&sxndlig.R3.graph);
    sxfree (sxndlig.R1.left_attr);
    sxfree (sxndlig.R1.right_attr);
    sxfree (sxndlig.R2.left_attr);
    sxfree (sxndlig.R2.right_attr);
    sxfree (sxndlig.R3.left_attr);
    sxfree (sxndlig.R3.right_attr);

    XxY_free (&sxndlig.memo);
    sxfree (sxndlig.memo2attr);
    sxfree (sxndlig.rule2attr);
    sxfree (sxndlig.grammar2attr);

    if ((ret_val = sxndparse_clean_forest ()))
    {
	if (parse_stack.is_ambiguous)
	    /* Ici, on fait au plus simple pour "deplier", on lance "post". */
	    sxndlig_post_do_it ();
	else
	    sxndparse_pack_forest ();
    }

    return (int) ret_val;
}





bool sxndligpost2 (int which, SXTABLES *arg)
{
  switch (which) {
  case SXOPEN:
    sxndlig_common.sxtables = arg;
    sxndlig_common.code = *(arg->sxligparsact);

/*  inutile : valeurs par defaut... 
    parse_stack.for_parsact.action_top = NULL;
    parse_stack.for_parsact.action_pop = NULL;
    parse_stack.for_parsact.action_bot = NULL;
    parse_stack.for_parsact.action_new_top = NULL;
    parse_stack.for_parsact.action_final = NULL;
    parse_stack.for_parsact.GC = NULL;
    parse_stack.for_parsact.third_field = NULL;
*/
    parse_stack.for_parsact.post = sxndlig_post2_do_it;

    (*sxndlig_common.code.parsact) (which, arg);

    return SXANY_BOOL;

  case SXCLOSE:
    (*sxndlig_common.code.parsact) (which, arg);

    return SXANY_BOOL;

  case SXINIT:
    sxplocals.mode.with_semact = false;
    sxplocals.mode.with_parsact = false;
    sxplocals.mode.with_parsprdct = false;
    (*sxndlig_common.code.parsact) (which, arg);

    return SXANY_BOOL;

  case SXFINAL:
    sxndligpost (SXFINAL, 0); /* Pour un free... */

    (*sxndlig_common.code.parsact) (which, arg);

    return SXANY_BOOL;

  case SXACTION:
    return SXANY_BOOL;

  case SXDO:
    return SXANY_BOOL;

  case SXUNDO:
    return SXANY_BOOL;

  case SXPREDICATE:
    return true;

  default:
    fputs ("The function \"sxndligpost2\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
  }

  /* NOTREACHED return SXANY_BOOL; */
}
