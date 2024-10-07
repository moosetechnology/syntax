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
/* Cette version elague la foret partagee complete construite sur la trame
   non contextuelle. */
/* Fonctionne en deux passes :
      - une passe ascendante au cours de laquelle les piles sont construites
        de droite a gauche (les valeurs de pile en RHS sont utilisees pour fabriquer
	les piles de la LHS)
      - une passe descendante au cours de laquelle les piles sont construites
        de gauche a droite (les valeurs de pile en LHS sont utilisees pour fabriquer
	les piles de la RHS)

   Un noeud donne est valide ssi les valeurs ascendantes et descendantes sont a
   intersection non vide. */
/* Cette version detecte les cycles dans la grammaire (et appelle sxtrap) mais
   ne sait pas les traiter. */

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
/* 20030507 09:09 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 18-01-95 14:20 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT_SXNDLIGPOST[] = "@(#)sxndligpost.c	- SYNTAX [unix] - Lundi 6 Mars 1995";

static char	ME [] = "sxndligpost";

# include 	"sxnd.h"
# include 	"sxndlig_post.h"


#ifdef EBUG
# define stdout_or_NULL	stdout
#else
# define stdout_or_NULL	NULL
#endif

static struct sxndlig	sxndlig;


struct sxndlig_common	sxndlig_common; /* Global */

static  void
addresses_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.uf.new_rule = (struct rule*) sxrealloc (sxndlig.uf.new_rule,
						     new_size + 1,
						     sizeof (struct rule));
}


static  void
hooks_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.uf.hook2attr = (struct uf_hook2attr*) sxrealloc (sxndlig.uf.hook2attr,
						       XxY_size (sxndlig.uf.hooks) + 1,
						       sizeof (struct uf_hook2attr));
}


static int
init_pile (prdct_or_act_no)
  int prdct_or_act_no;
{
    /* Calcule une pile from scratch en utiliant une action ou un predicat */
    /* Action ou Predicat secondaire */
    int *pc, *pclim, pile;

    if (prdct_or_act_no == 0)
	/* ( ) */
	return 0;
  
    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_or_act_no];
    /* *pc == SECONDARY */
    pclim = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_or_act_no + 1] - 2;

    pile = 0;

    while (++pc <= pclim)
	XxY_set (&sxndlig.piles, pile, *pc, &pile);

    return pile;
}


static void
put_bu_stack (rule_no, pile)
  int		rule_no, pile;
{
    /* ajoute a rule_no la pile "pile" si elle n'existe pas deja */
    struct rule2attr	*pattr;
    int			object;
  
    if (pile != SET_NIL)
    {
	pattr = sxndlig.rule2attr + rule_no;

	if (pattr->pile_nb == 0)
	{
	    pattr->pile_nb = 1;
	    pattr->pile = pile;
	}
	else
	{
	    if (pattr->pile != pile)
	    {
		/* ce n'est pas la pile principale */
		if (!XxY_set (&sxndlig.objects, rule_no, pile, &object))
		{
		    /* Nouvel objet */
		    pattr->pile_nb++;
		}
	    }
	}
    }
}




static int
bu_action (act_no, pile)
    int	act_no, pile;
{
    /* act_no > 0 et primaire */
    int	pop_nb, *pc, *pclim;

    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no];

#ifdef EBUG
    if (act_no == 0 || *pc == SECONDARY)
	sxtrap (ME, "bu_action");
#endif

    pclim = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no + 1] - 2;

    if ((pop_nb = pc [2] - pc [3]) > 0) {
	/* pc [2] == longueur du test du predicat
	   pc [3] == longueur du prefixe commun entre action et predicat. */
	do {
#ifdef EBUG
	    if (pile == 0)
		sxtrap (ME, "bu_action");
#endif

	    pile = XxY_X (sxndlig.piles, pile);
	} while (--pop_nb > 0);
    }

    pc += 3 + pc [3];

    while (++pc <= pclim)
	XxY_set (&sxndlig.piles, pile, *pc, &pile);

    return pile;
}


static bool
bu_prdct (prdct_no, pile)
    int prdct_no, pile;
{
    /* On regarde si pile verifie le predicat prdct_no */
    int	*pc, *pclim;

#ifdef EBUG
    if (pile == SET_NIL)
	sxtrap (ME, "bu_prdct");
#endif

    if (prdct_no == 0)
	/* ( ) */
	return pile == 0;

    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no];

    if (*pc == PRIMARY)
	pc += 3;

    pclim = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no + 1] - 2;

    if (pc == pclim)
	/* ( .. ) */
	return true;

    do {
	if (pile == 0 || XxY_Y (sxndlig.piles, pile) != *pclim)
	    return false;

	pile = XxY_X (sxndlig.piles, pile);
    } while (pc < --pclim);

    return true;
}



static void
compute_bu_stacks (item, rhs_rule_no)
  int		item, rhs_rule_no;
{
    /* On est ds la passe bottom_up, on applique aux piles associees a rhs_rule_no le traitement
       prdct_no, act_no et on range le resultat ds les piles associees a lhs_rule_no. */
    /* Les piles associees a rhs_rule_no sont calculees, on les propage eventuellement
       sur lhs_rule_no */
    struct rule2attr	*prhs_attr;
    struct rule2attr	*plhs_attr;
    int			lhs_rule_no, act_no, prdct_no, object, pile;
    bool		checked;
    int			*pc;
    struct rule		*plhs;

    lhs_rule_no = parse_stack.grammar [item].lhs_rule;

    prhs_attr = sxndlig.rule2attr + rhs_rule_no;
    plhs_attr = sxndlig.rule2attr + lhs_rule_no;
    plhs = parse_stack.rule + lhs_rule_no;

    if (lhs_rule_no >= parse_stack.hook_rule)
    {
	/* on est dans un hook */
	if (prhs_attr->act_no >= 10000)
	{
	    /* Il y a eu un calcul de pile */
	    if (prhs_attr->pile_nb > 0)
	    {
		/* Il y a (au moins) une pile, on recopie */
		put_bu_stack (lhs_rule_no, prhs_attr->pile);

		if (prhs_attr->pile_nb > 1)
		{
		    XxY_Xforeach (sxndlig.objects, rhs_rule_no, object)
			put_bu_stack (lhs_rule_no, XxY_Y (sxndlig.objects, object));
		}
	    }
	    else
	    {
		/* On supprime "item" */
		sxndparse_erase_hook_item (item);
	    }
	}

	if (--plhs_attr->count == 0)
	{
	    /* hook completement evalue' */
	    CTRL_PUSH (sxndlig.reached, lhs_rule_no);
	}
    }
    else
    {
	act_no = plhs_attr->act_no;
	prdct_no = sxndlig.grammar2attr [item].prdct_no;

	if (prdct_no < 10000)
	    /* Il n'y a pas de pile */
	    checked = true;
	else
	{
	    checked = false;

	    if (prhs_attr->pile_nb > 0)
	    {
		/* rhs_rule_no est valide */
		prdct_no -= 10000;
		pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no];

		if (bu_prdct (prdct_no, prhs_attr->pile))
		{
		    checked = true;

		    if (*pc == PRIMARY)
			put_bu_stack (lhs_rule_no, bu_action (act_no - 10000, prhs_attr->pile));
		}

		if (prhs_attr->pile_nb > 1)
		{
		    XxY_Xforeach (sxndlig.objects, rhs_rule_no, object)
		    {
			pile = XxY_Y (sxndlig.objects, object);
	  
			if (bu_prdct (prdct_no, pile))
			{
			    checked = true;

			    if (*pc == PRIMARY)
				put_bu_stack (lhs_rule_no, bu_action (act_no - 10000, pile));
			}
		    }
		}
	    }
	}

	plhs_attr->count--;

	if (!checked)
	{
	    plhs_attr->pile_nb = 0;
	    /* plhs->lgth = 0; */
	    CTRL_PUSH (parse_stack.erased, lhs_rule_no);
	}

	if (plhs_attr->count == 0)
	{
	    /* La RHS est verifiee et il y a une pile en LHS */
	    if (plhs->lgth > 0 &&
		act_no >= 10000 &&
		sxndlig_common.code.prdct_or_act_code [sxndlig_common.code.prdct_or_act_disp [act_no - 10000]]
		== SECONDARY)
	    {
		/* La RHS est verifiee et correcte
		   il y a une pile en LHS qui ne depend pas de la RHS
		   on la calcule */
		plhs_attr->pile = init_pile (act_no - 10000);
		plhs_attr->pile_nb = 1;
	    }

	    CTRL_PUSH (sxndlig.reached, lhs_rule_no);
	}
    }
}


static int
td_action (act_no, pile)
    int	act_no, pile;
{
    /* On est en passe top-down
       Similaire a bu_action excepte que :
       - act_no est en fait un predicat
       - que les piles calculees doivent deja exister (elles ont deja dues etre calculees
       en passe bottom-up */
    /* act_no > 0 et primaire */
    int	*pc, *pclim;

    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no];

#ifdef EBUG
    if (pile == SET_NIL || act_no == 0 || *pc == SECONDARY)
	sxtrap (ME, "td_action");
#endif

    pc += 3;
    pclim = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no + 1] - 2;

    while (++pc <= pclim)
	if (!XxY_set (&sxndlig.piles, pile, *pc, &pile))
	    /* Nouvelle pile */
	    return SET_NIL;

    return pile;
}



static int
td_prdct (prdct_no, pile)
    int prdct_no, pile;
{
    /* On est en passe top-down
       Similaire a bu_prdct excepte que :
       - prdct_no est en fait un act_no
       - que l'on retourne effectivement la pile testee (apres depilage) car elle
       va etre utilisee par le td_action (dans ce sens il n'y a aucun partage
       de prefixe). */
    /* prdct_no > 0 et PRIMARY */
    int	*pc, *pclim;

    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no];

#ifdef EBUG
    if (pile == SET_NIL || prdct_no == 0 || *pc == SECONDARY)
	sxtrap (ME, "td_prdct");
#endif

    pc += 3;
    pclim = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no + 1] - 2;

    if (pc < pclim)
    {
	do {
	    if (pile == 0 || XxY_Y (sxndlig.piles, pile) != *pclim)
		return SET_NIL;

	    pile = XxY_X (sxndlig.piles, pile);
	} while (pc < --pclim);
    }
    /* else ( .. ) */

    return pile;
}

static bool
td_check_stack (rule_no, pile)
  int rule_no, pile;
{
    /* verifie si pile est un membre de l'ensemble des piles de rule_no.
       Si oui marque la pile correspondante. */
    struct rule2attr	*pattr = sxndlig.rule2attr + rule_no;
    int			object;
  
    if (pattr->pile_nb == 0)
	return false;

    if ((pattr->pile & LIG_7F) == (unsigned int)pile)
    {
	/* C'est la pile principale */
	pattr->pile |= LIG_80;	/* On la marque */
	return true;
    }

    if (pattr->pile_nb == 1)
	return false;

    /* ce n'est pas la pile principale */
    object = XxY_is_set (&sxndlig.objects, rule_no, pile);

    if (object == 0)
	return false;

    /* On le marque */
    SXBA_1_bit (sxndlig.object_set, object);

    return true;
}

static void
td_filter (rule_no)
  int rule_no;
{
    /* Supprime les piles de rule_no qui ne sont pas marquees */
    struct rule2attr	*pattr = sxndlig.rule2attr + rule_no;
    int			object, pile_nb, last_object;
  
    if ((pile_nb = pattr->pile_nb) == 0)
	return;

    if (pattr->pile & LIG_80)
    {
	/* la pile principale est marquee, on la revalide */
	pattr->pile &= LIG_7F;
    }
    else
    {
	pattr->pile = SET_NIL;
	pattr->pile_nb--;
    }

    if (pile_nb > 1)
    {
	last_object = 0;

	XxY_Xforeach (sxndlig.objects, rule_no, object)
	{
	    if (SXBA_bit_is_set_reset (sxndlig.object_set, object))
	    {
		/* marque', on se souvient du dernier */
		last_object = object;
	    }
	    else
	    {
		/* On le supprime */
		XxY_erase (sxndlig.objects, object);
		pattr->pile_nb--;
	    }
	}

	if (pattr->pile == SET_NIL && last_object != 0)
	{
	    pattr->pile = XxY_Y (sxndlig.objects, last_object);
	    XxY_erase (sxndlig.objects, last_object);
	}
    }
}


static void
compute_td_stacks (rhs_rule_no)
  int rhs_rule_no;
{
    /* Cette procedure a ete "declanchee" par la presence de la "derniere" occurrence de
       "rhs_rule_no" en RHS dans "grammar". On calcule les piles associees a toutes les occurrences
       de "rhs_rule_no" en RHS. */
    int			item, next_item, lhs_rule_no, object, pile, prdct_no, act_no;
    bool		checked;
    int			*pc;
    struct rule2attr	*plhs_attr;

    for (item = parse_stack.rule [rhs_rule_no].item_hd;
	 item != 0;
	 item = next_item)
    {
	next_item = parse_stack.grammar [item].next_item;

	if (parse_stack.grammar [item].rhs_symb == rhs_rule_no)
	{
	    lhs_rule_no = parse_stack.grammar [item].lhs_rule;
	    plhs_attr = sxndlig.rule2attr + lhs_rule_no;

	    if (lhs_rule_no >= parse_stack.hook_rule)
	    {
		/* C'est un hook */
		/* Si il n'y a pas de pile associee a rhs_rule_no, il n'y en a pas
		   non plus ni sur lhs_rule_no ni sur les autres composants du hook.
		   on se contente de continuer */
		if (sxndlig.rule2attr [rhs_rule_no].act_no >= 10000)
		{
		    /* Il y a des piles */
		    checked = false;

		    if (plhs_attr->pile_nb > 0)
		    {
			if (td_check_stack (rhs_rule_no, plhs_attr->pile))
			    checked = true;

			if (plhs_attr->pile_nb > 1)
			{
			    XxY_Xforeach (sxndlig.objects, lhs_rule_no, object)
			    {
				if (td_check_stack (rhs_rule_no, XxY_Y (sxndlig.objects, object)))
				    checked = true;
			    }
			}
		    }
		}

		if (!checked)
		{
		    /* On supprime rhs_rule_no de la RHS du hook. */
		    sxndparse_erase_hook_item (item);
		}
	    }
	    else
	    {
		prdct_no = sxndlig.grammar2attr [item].prdct_no;

		if (prdct_no >= 10000)
		{
		    prdct_no -= 10000;
		    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no];

		    if (*pc == SECONDARY)
		    {
			pile = init_pile (prdct_no);

			if (td_check_stack (rhs_rule_no, pile))
			{
			}
#ifdef EBUG
			else
			    /* Les piles secondaires on deja ete verifiees par la phase bottom-up */
			    sxtrap (ME, "top-down pass");
#endif
		    }
		    else
		    {
			checked = false;

			if (plhs_attr->pile_nb > 0)
			{
			    /* lhs_rule_no est valide */
			    act_no = plhs_attr->act_no - 10000; /* PRIMARY */

			    /* En passe top-down les actions sont utilisees comme des predicats
			       et les predicats comme des actions. */
			    if ((pile = td_prdct (act_no, plhs_attr->pile)) != SET_NIL)
			    {
				if ((pile = td_action (prdct_no, pile)) != SET_NIL)
				{
				    if (td_check_stack (rhs_rule_no, pile))
				    {
					/* Cette pile a deja ete calculee en bottom-up pour rhs_rule_no */
					checked = true;
				    }
				}
			    }

			    if (plhs_attr->pile_nb > 1)
			    {
				XxY_Xforeach (sxndlig.objects, lhs_rule_no, object)
				{
				    pile = XxY_Y (sxndlig.objects, object);
	  
				    if ((pile = td_prdct (act_no, pile)) != SET_NIL)
				    {
					if ((pile = td_action (prdct_no, pile)) != SET_NIL)
					{
					    if (td_check_stack (rhs_rule_no, pile))
					    {
						/* Cette pile a deja ete calculee en bottom-up
						   pour rhs_rule_no */
						checked = true;
					    }
					}
				    }
				}
			    }
			}

			if (!checked)
			{
			    plhs_attr->pile_nb = 0;
			    /* plhs->lgth = 0; */
			    CTRL_PUSH (parse_stack.erased, lhs_rule_no);
			}
		    }
		}
		/* else Pas de pile */
	    }
	}
    }

    /* Si il n'y a pas de pile associee aux occurrences de rhs_rule_no en rhs
       la regle rhs_rule_no est supprimee ssi toutes les regles ou rhs_rule_no
       apparait en rhs sont supprimees */

    /* On supprime les piles non validees */
    td_filter (rhs_rule_no);

    /* Dans tous les cas on "valide" rhs_rule_no, meme en cas d'echec total */
    CTRL_PUSH (sxndlig.reached, rhs_rule_no);
}



static void
check_new_grammar_size (lgth)
    int lgth;
{
    int new_top;

    if ((new_top = sxndlig.uf.new_grammar_top + lgth) > sxndlig.uf.new_grammar_size)
    {
	while (new_top > (sxndlig.uf.new_grammar_size *= 2));

	sxndlig.uf.new_grammar = (struct grammar*) sxrealloc (sxndlig.uf.new_grammar,
							      sxndlig.uf.new_grammar_size + 1,
							      sizeof (struct grammar));
    }
}

static void
put_new_rule_rhs_hook (new_rule, new_hook)
    int new_rule, new_hook;
{
    sxndlig.uf.new_grammar [sxndlig.uf.new_grammar_top].lhs_rule = new_rule;

    sxndlig.uf.new_grammar [sxndlig.uf.new_grammar_top].next_item =
	sxndlig.uf.hook2attr [new_hook].item;
    sxndlig.uf.hook2attr [new_hook].item = sxndlig.uf.new_grammar_top;

    sxndlig.uf.new_grammar_top++;
}


static void
put_new_rule_rhs_elem (new_rule, new_symbol)
    int new_rule, new_symbol;
{
    struct grammar	*pgram = sxndlig.uf.new_grammar + sxndlig.uf.new_grammar_top;

    pgram->lhs_rule = new_rule;

    if (new_symbol > 0)
    {
	pgram->next_item = sxndlig.uf.new_rule [new_symbol].item_hd;
	sxndlig.uf.new_rule [new_symbol].item_hd = sxndlig.uf.new_grammar_top;
    }
    else
	pgram->rhs_symb = new_symbol;

    sxndlig.uf.new_grammar_top++;
}



static void
unfold_rule (rule_no, pile)
    int rule_no, pile;
{
    int rule_lgth, item, lhs_symb, act_no, address, new_symbol, lim, rhs_symb, prdct_no, rhs_pile, hook; 
    int	new_rule_no;

    rule_lgth = parse_stack.rule [rule_no].lgth;
    parse_stack.G.G += rule_lgth;
    parse_stack.G.P++;
    parse_stack.G.N++;

    check_new_grammar_size (rule_lgth);

    item = parse_stack.rule [rule_no].hd;
    lhs_symb = parse_stack.grammar [item].rhs_symb;
    act_no = sxndlig.rule2attr [rule_no].act_no;

    if (!XxY_set (&sxndlig.uf.addresses, rule_no, pile, &new_rule_no))
	sxndlig.uf.new_rule [new_rule_no].item_hd = 0;

    sxndlig.uf.new_rule [new_rule_no].hd = sxndlig.uf.new_grammar_top;
    sxndlig.uf.new_rule [new_rule_no].lgth = rule_lgth;

    XxY_set (&sxndlig.uf.new_symbols, lhs_symb, pile, &new_symbol);
    
    sxndlig.uf.new_grammar [sxndlig.uf.new_grammar_top].rhs_symb = new_symbol;
    sxndlig.uf.new_grammar [sxndlig.uf.new_grammar_top].lhs_rule = new_rule_no;
    sxndlig.uf.new_grammar_top++;

    lim = item + rule_lgth;

    while (++item < lim)
    {
	if ((rhs_symb = parse_stack.grammar [item].rhs_symb) > 0)
	{
	    /* Non terminal */
	    prdct_no = sxndlig.grammar2attr [item].prdct_no;

	    if (prdct_no < 10000)
		rhs_pile =  -1;
	    else
	    {
		prdct_no -= 10000;

		if (sxndlig_common.code.prdct_or_act_code [sxndlig_common.code.prdct_or_act_disp [prdct_no]] == SECONDARY)
		    rhs_pile = init_pile (prdct_no);
		else
		{
		    rhs_pile = td_prdct (act_no - 10000, pile);

#ifdef EBUG
		    if (rhs_pile == SET_NIL)
			sxtrap (ME, "unfold_rule");
#endif

		    rhs_pile = td_action (prdct_no, rhs_pile);

#ifdef EBUG
		    if (rhs_pile == SET_NIL)
			sxtrap (ME, "unfold_rule");
#endif		    

		}
	    }

	    if (rhs_symb >= parse_stack.hook_rule)
	    {
		/* On reference un hook, on differe son calcul exact car :
		   - il peut disparaitre
		   - sa valeur doit etre superieure aux adresses normales */
		if (!XxY_set (&sxndlig.uf.hooks, rhs_symb, rhs_pile, &hook))
		    sxndlig.uf.hook2attr [hook].item = 0;

		put_new_rule_rhs_hook (new_rule_no, hook);
	    }
	    else
	    {
		if (!XxY_set (&sxndlig.uf.addresses, rhs_symb, rhs_pile, &address))
		    sxndlig.uf.new_rule [address].item_hd = 0;

		put_new_rule_rhs_elem (new_rule_no, address);
	    }
	}
	else
	    /* terminal */
	    put_new_rule_rhs_elem (new_rule_no, rhs_symb);
    }
}

static bool
unfold_check_stack (rule_no, pile)
  int rule_no, pile;
{
    /* verifie si pile est un membre de l'ensemble des piles de rule_no. */
    struct rule2attr	*pattr = sxndlig.rule2attr + rule_no;
  
    if (pattr->pile_nb == 0)
	return false;

    if ((pattr->pile & LIG_7F) == (unsigned int)pile)
	/* C'est la pile principale */
	return true;

    if (pattr->pile_nb == 1)
	return false;

    /* ce n'est pas la pile principale */
    return XxY_is_set (&sxndlig.objects, rule_no, pile) > 0;
}

static void
unfold_hook (rule_no, pile)
    int rule_no, pile;
{
    /* Si pile == -1, on "recopie" le hook sinon on extrait de sa RHS les adresses qui ont
     "pile" comme pile associee. */
    int rule_lgth, item, lhs_symb, address, new_symbol, lgth, lim, rhs_rule_no, rhs_address,
        grammar_top, hook, *p;

    sxinitialise(rhs_address); /* pour faire taire "gcc -Wu" uninitialized" */
    rule_lgth = parse_stack.rule [rule_no].lgth;
    check_new_grammar_size (rule_lgth);

    item = parse_stack.rule [rule_no].hd;
    lhs_symb = parse_stack.grammar [item].rhs_symb;

    grammar_top = sxndlig.uf.new_grammar_top;

    sxndlig.uf.new_grammar_top++; /* bidon, on reserve la place */
    lgth = 1;

    lim = item + rule_lgth;

    while (++item < lim)
    {
	rhs_rule_no = parse_stack.grammar [item].rhs_symb;

	if (pile == -1 || unfold_check_stack (rhs_rule_no, pile))
	{
	    rhs_address = XxY_is_set (&sxndlig.uf.addresses, rhs_rule_no, pile);

#ifdef EBUG
	    if (rhs_address == 0)
		sxtrap (ME, "unfold_hook");
#endif

	    sxndlig.uf.new_grammar [sxndlig.uf.new_grammar_top++].rhs_symb = rhs_address;

	    lgth++;
	}
    }

#ifdef EBUG
    if (lgth == 1)
	sxtrap (ME, "unfold_hook");
#endif
	
    hook = XxY_is_set (&sxndlig.uf.hooks, rule_no, pile);

#ifdef EBUG
    if (hook == 0)
	sxtrap (ME, "unfold_hook");
#endif

    /* Si lgth == 2, ce n'est pas un hook, il faut donc remplacer toutes les occurrences de
       address dans new_grammar par rhs_address et "supprimer" ce hook. */
    if (lgth == 2)
    {
	/* On concatene les listes "rhs-address" */
	address = rhs_address;

	p = &(sxndlig.uf.hook2attr [hook].item);

#ifdef EBUG
	if (*p <= 0)
	    sxtrap (ME, "unfold_hook");
#endif

	while (*(p = &(sxndlig.uf.new_grammar [*p].next_item)) > 0);

	*p = sxndlig.uf.new_rule [address].item_hd;
	sxndlig.uf.new_rule [address].item_hd = sxndlig.uf.hook2attr [hook].item;

	/* On recupere la place ds new_grammar */
	sxndlig.uf.new_grammar_top = grammar_top;
    }
    else
    {
	if (!XxY_set (&sxndlig.uf.addresses, rule_no, pile, &address))
	    sxndlig.uf.new_rule [address].item_hd = 0;
#ifdef EBUG
	else
	    sxtrap (ME, "unfold_hook");
#endif

	sxndlig.uf.new_rule [address].item_hd = sxndlig.uf.hook2attr [hook].item;

	XxY_set (&sxndlig.uf.new_symbols, lhs_symb, pile, &new_symbol);
	sxndlig.uf.new_grammar [grammar_top].rhs_symb = new_symbol;
	sxndlig.uf.new_grammar [grammar_top].lhs_rule = address;
	sxndlig.uf.new_rule [address].hd = grammar_top;
	sxndlig.uf.new_rule [address].lgth = lgth;
	parse_stack.G.G += lgth;
	parse_stack.G.P++;
	parse_stack.G.N++;

	while (++grammar_top < sxndlig.uf.new_grammar_top)
	{
	    rhs_address = sxndlig.uf.new_grammar [grammar_top].rhs_symb;
	    sxndlig.uf.new_grammar [grammar_top].next_item = sxndlig.uf.new_rule [rhs_address].item_hd;
	    sxndlig.uf.new_rule [rhs_address].item_hd = grammar_top;
	    sxndlig.uf.new_grammar [grammar_top].lhs_rule = address;
	}
    }
}


#if 0
static int
sxndlig_third_field (psymb, pstring)
    int	*psymb;
    char **pstring;
{
    /* retourne dans pstring une chaine caracteristique de la pile associee au
       symbole repere' par psymb. */
    int		pile;
    char	t [8];

    pile = XxY_Y (sxndlig.uf.new_symbols, *psymb);
    *psymb = XxY_X (sxndlig.uf.new_symbols, *psymb);

    if (pile == -1)
	*pstring = NULL;
    else
    {
	sprintf (t, "%i", pile);
	*pstring = t;
    }
}

#endif

static void
sxndlig_unfold ()
{
    /* La foret est ambigue. On la deplie pour accorder les partages syntaxiques et semantiques */
    /* Un symbole est un triplet <reduce p, [i..j], pile> (si pile == -1 => pas de pile.
       une adresse est un doublet (rule_nb, pile) */
    int			lhs_rule_no, object, item;
    struct rule2attr	*plhs_attr;

    XxY_alloc (&sxndlig.uf.addresses, "sxndlig.uf.addresses", parse_stack.G.P * 2, 1, 0, 0, addresses_oflw, stdout_or_NULL);

    
    sxndlig.uf.new_rule = (struct rule*) sxalloc (XxY_size (sxndlig.uf.addresses) + 1,
						   sizeof (struct rule));

   
    XxY_alloc (&sxndlig.uf.new_symbols, "sxndlig.uf.new_symbols", parse_stack.G.N * 2, 1, 0, 0, NULL, stdout_or_NULL);
    sxndlig.uf.is_new_symbols = true;

    XxY_alloc (&sxndlig.uf.hooks, "sxndlig.uf.hooks", (parse_stack.rule_top - parse_stack.hook_rule + 2) * 2,
	       1, 0, 0, hooks_oflw, stdout_or_NULL);

    sxndlig.uf.hook2attr = (struct uf_hook2attr*) sxalloc (XxY_size (sxndlig.uf.hooks) + 1,
							   sizeof (struct uf_hook2attr));

    sxndlig.uf.new_grammar =
	(struct grammar*) sxalloc ((sxndlig.uf.new_grammar_size = 2 * parse_stack.G.G) + 1,
				   sizeof (struct grammar));
    sxndlig.uf.new_grammar_top = 1;

    parse_stack.G.G = parse_stack.G.N = parse_stack.G.P = 0;

    lhs_rule_no = 0;

    while ((lhs_rule_no = sxba_scan (CTRL2SXBA (parse_stack.reached), lhs_rule_no)) > 0 &&
	   lhs_rule_no < parse_stack.hook_rule)
    {
	plhs_attr = sxndlig.rule2attr + lhs_rule_no;

	if (plhs_attr->act_no < 10000)
	    unfold_rule (lhs_rule_no, -1);
	else
	{
#ifdef EBUG
	    if (plhs_attr->pile_nb == 0)
		sxtrap (ME, "sxndlig_unfold");
#endif
	    unfold_rule (lhs_rule_no, plhs_attr->pile);

	    if (plhs_attr->pile_nb > 1)
	    {
		XxY_Xforeach (sxndlig.objects, lhs_rule_no, object)
		    unfold_rule (lhs_rule_no, XxY_Y (sxndlig.objects, object));
	    }
	}
    }

    sxndlig.uf.hook_rule = XxY_top (sxndlig.uf.addresses) + 1;

    /* On parcourt les hook */
    lhs_rule_no = parse_stack.hook_rule - 1;

    while ((lhs_rule_no = sxba_scan (CTRL2SXBA (parse_stack.reached), lhs_rule_no)) > 0)
    {
	plhs_attr = sxndlig.rule2attr + lhs_rule_no;

	if (plhs_attr->pile_nb == 0)
	    /* hook sans pile */
	    unfold_hook (lhs_rule_no, -1);
	else
	{
	    unfold_hook (lhs_rule_no, plhs_attr->pile);

	    if (plhs_attr->pile_nb > 1)
	    {
		XxY_Xforeach (sxndlig.objects, lhs_rule_no, object)
		    unfold_hook (lhs_rule_no, XxY_Y (sxndlig.objects, object));
	    }
	}
    }

    sxndlig.uf.rule_top = XxY_top (sxndlig.uf.addresses);

    /* Mise en place definitive des adresses en RHS des nouvelles regles. */
    for (lhs_rule_no = 1; lhs_rule_no <= sxndlig.uf.rule_top; lhs_rule_no++)
    {
	for (item = sxndlig.uf.new_rule [lhs_rule_no].item_hd;
	     item > 0;
	     item = sxndlig.uf.new_grammar [item].next_item)
	    sxndlig.uf.new_grammar [item].rhs_symb = lhs_rule_no;
    }

    /* On remplace l'ancienne foret par la nouvelle foret depliee. */
    sxfree (parse_stack.rule);
    sxfree (parse_stack.grammar);

    parse_stack.rule = sxndlig.uf.new_rule;
    parse_stack.grammar = sxndlig.uf.new_grammar;
    parse_stack.rule_top = sxndlig.uf.rule_top;
    parse_stack.hook_rule = sxndlig.uf.hook_rule;

    /* On libere les structures inutiles (pas new_symbols qui ressert dans third_field) */
    XxY_free (&sxndlig.uf.addresses);
   
    XxY_free (&sxndlig.uf.hooks);
    sxfree (sxndlig.uf.hook2attr);
}


int
sxndlig_post_do_it ()
{
    /* On construit une representation de la foret partagee sur laquelle on va evaluer les piles. */
    /* symbol si -> (#pred, pi_set)
       A chaque symbole <red i, [p..q]> on associe :
       - #pred nb de predecesseurs de si non encore calcules
       - pi_set ensemble des piles (initialise a SET_NIL)
       */
    int	rhs_rule_no, lim, item, rule_lgth, prdct_no, indice, reduce, act_no, lhs_symb, lhs_rule_no, next_item, ret_val;
    int *pprdct_no;

    sxinitialise(pprdct_no); /* pour faire taire "gcc -Wu" uninitialized" */
    sxndlig.rule2attr = (struct rule2attr *) sxcalloc ((SXUINT)parse_stack.rule_top + 1, sizeof (struct rule2attr));
    sxndlig.grammar2attr = (struct grammar2attr *) sxalloc (parse_stack.grammar_top + 1,
							    sizeof (struct grammar2attr));


    XxY_alloc (&sxndlig.piles, "sxndlig.piles", parse_stack.grammar_top /* why not */, 1, 0, 0, NULL, stdout_or_NULL);

    XxY_alloc (&sxndlig.objects, "sxndlig.objects", parse_stack.rule_top, 1, 1, 0, NULL, stdout_or_NULL);

    CTRL_ALLOC (sxndlig.reached, parse_stack.rule_top + 1);

    sxndlig.rule2attr [parse_stack.root].act_no = -1;

    for (lhs_rule_no = 1; lhs_rule_no < parse_stack.hook_rule; lhs_rule_no++)
    {
	if ((rule_lgth = parse_stack.rule [lhs_rule_no].lgth) > 0)
	{
	    item = parse_stack.rule [lhs_rule_no].hd;
	    lhs_symb = parse_stack.grammar [item].rhs_symb;

	    if (lhs_rule_no != parse_stack.root)
	    {
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
	    }
	    else
		act_no = -1;

	    sxndlig.rule2attr [lhs_rule_no].act_no = act_no;
	    lim = item + rule_lgth;
	    indice = 0;

	    while (++item < lim)
	    {
		indice++;

		if ((rhs_rule_no = parse_stack.grammar [item].rhs_symb) > 0)
		{
		    /* Non terminal */
		    /* On recupere le predicat associe */
		    prdct_no = (lhs_rule_no == parse_stack.root) ? -1 : pprdct_no [indice];
		    sxndlig.grammar2attr [item].prdct_no = prdct_no;
		    sxndlig.rule2attr [lhs_rule_no].count++;
		}
	    }

	    if (sxndlig.rule2attr [lhs_rule_no].count == 0)
	    {
		CTRL_PUSH (sxndlig.reached, lhs_rule_no);

		if (act_no >= 10000)
		    /* La pile de la LHS est secondaire */
		    put_bu_stack (lhs_rule_no, init_pile (act_no - 10000));
	    }
	}
    }

    /* On parcourt les hook */
    for (lhs_rule_no = parse_stack.hook_rule; lhs_rule_no <= parse_stack.rule_top; lhs_rule_no++)
    {
	if ((rule_lgth = parse_stack.rule [lhs_rule_no].lgth) > 0)
	{
	    sxndlig.rule2attr [lhs_rule_no].count = rule_lgth - 1;
	}
    }

    /* On effectue la passe bottom-up */

    while (!CTRL_IS_EMPTY (sxndlig.reached))
    {
	rhs_rule_no = CTRL_POP (sxndlig.reached);
      
	for (item = parse_stack.rule [rhs_rule_no].item_hd;
	     item != 0;
	     item = next_item)
	{
	    next_item = parse_stack.grammar [item].next_item;

	    if (parse_stack.grammar [item].rhs_symb == rhs_rule_no)
		compute_bu_stacks (item, rhs_rule_no);
	}
    }
  
    SXBA_1_bit (CTRL2SXBA (sxndlig.reached), parse_stack.root);

    if (sxba_0_rlscan (CTRL2SXBA (sxndlig.reached), parse_stack.hook_rule) > 0)
	/* Normalement, il ne reste des regles non evaluees que dans le cas de
	   grammaires cycliques !!! */
	sxtrap (ME, "Processing of cyclic grammars is not implemented.");


    /* A FAIRE : Traitement des erreurs */

    /* On regarde l'effet de la passe bottom-up :
       si ce n'est meme plus un arbre
       si c'est non ambigu (arbre unique, donc evaluation unique (c'est obligatoirement
       la bonne donc passe top-down inutile
       */


    /* On effectue la passe top_down */
    sxndlig.object_set = sxba_calloc (XxY_top (sxndlig.objects) + 1);

    /* On reinitialise "count" a 0 */
    for (lhs_rule_no = 1; lhs_rule_no <= parse_stack.rule_top; lhs_rule_no++)
	sxndlig.rule2attr [lhs_rule_no].count = 0;

    for (lhs_rule_no = 1; lhs_rule_no <= parse_stack.rule_top; lhs_rule_no++)
    {
	if ((rule_lgth = parse_stack.rule [lhs_rule_no].lgth) > 0)
	{
	    item = parse_stack.rule [lhs_rule_no].hd;
	    lim = item + rule_lgth;

	    while (++item < lim)
	    {
		if ((rhs_rule_no = parse_stack.grammar [item].rhs_symb) > 0)
		    /* Non terminal */
		    sxndlig.rule2attr [rhs_rule_no].count++;
	    }
	}
    }

    CTRL_CLEAR (sxndlig.reached);
    CTRL_PUSH (sxndlig.reached, parse_stack.root);

    do
    {
	lhs_rule_no = CTRL_POP (sxndlig.reached);
	rule_lgth = parse_stack.rule [lhs_rule_no].lgth;

	item = parse_stack.rule [lhs_rule_no].hd;
	lim = item + rule_lgth;      

	/* On examine de la droite vers la gauche pour eviter les problemes
	   avec les hook : le courant peut etre supprime et "remplace" par le
	   plus a droite qui serait donc "oublie'". */
	while (item < --lim)
	{
	    if ((rhs_rule_no = parse_stack.grammar [lim].rhs_symb) > 0)
	    {
		/* Non terminal */
		if (--sxndlig.rule2attr [rhs_rule_no].count == 0)
		{
		    /* Toutes les occurrences de rhs_rule_no en RHS se trouvent dans des
		       regles dont les piles en LHS sont calculees : on s'en occupe */
		    compute_td_stacks (rhs_rule_no);
		}
	    }
	}
    } while (!CTRL_IS_EMPTY (sxndlig.reached));
  
    if ((ret_val = sxndparse_clean_forest ()))
    {
	if (parse_stack.is_ambiguous)
	{
	    /* On effectue le depliage, les ambiguites vont rester. */
	    sxndlig_unfold ();

#if 0
	    /* impression du champ "pile" ds les symboles de la foret depliee. */
	    parse_stack.for_parsact.third_field = sxndlig_third_field;
#endif
	}
	else
	    sxndparse_pack_forest ();
    }

    sxfree (sxndlig.object_set);

    CTRL_FREE (sxndlig.reached);

    sxfree (sxndlig.rule2attr);
    sxfree (sxndlig.grammar2attr);
    XxY_free (&sxndlig.piles);
    XxY_free (&sxndlig.objects);

    return (int) ret_val;
}



bool sxndligpost (which, arg)
    int		which;
    SXTABLES	*arg;
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
    parse_stack.for_parsact.post = sxndlig_post_do_it;

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
    if (sxndlig.uf.is_new_symbols)
	XxY_free (&sxndlig.uf.new_symbols);

    if (sxndlig_common.code.parsact != NULL)
	/* SXFINAL peut etre appele "directement" depuis ceux qui ont appele "post_do_it"
	   (sxndligparsact, sxndligpost2, ...) */
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
    fputs ("The function \"sxndligpost\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
  }

  /* NOTREACHED return SXANY_BOOL; */
}

