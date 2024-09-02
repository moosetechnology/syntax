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
   *     Produit de l'equipe ChLoE.			  *
   *                                                      *
   ******************************************************** */

/* Execute les actions et predicats syntaxiques produits a partir d'une
   Linear Indexed Grammar par le traducteur lig2bnf dans le cas d'une
   analyse non deterministe. */
/* Cette version detecte les cycles dans la grammaire (et appelle sxtrap) mais
   ne sait pas les traiter. */
/* Apres la passe semantique la foret obtenue n'est pas depliee vis-a-vis de la
   semantique. Cad que des partages syntaxiques ne sont pas valides semantiquement.
   Par exemple on peut avoir:
   A -> C
   B -> C
   C -> D
   C -> E
   ou C partage les contextes A et B
   et C designe les sous-arbres D et E
   or il se peut que semantiquement seuls les chemins
   A -> C -> D et B -> C -> E soient corrects et les chemins
   A -> C -> E et B -> C -> D soient invalides.
   Ce n'est pas fait ds cette version (c'est fait dans "mixt", "bylev" et "post")
   car il ne semble pas que ca vaille la peine de la developper davantage! */
/* On fait au plus simple : on appelle "post_do_it" */   

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030506 11:21 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 26-04-95 15:48 (pb):		Suppression des "aux_trans"		*/
/************************************************************************/
/* 26-08-94 14:06 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)sxndligparsact.c\t- SYNTAX [unix] - Mercredi 26 Avril 1995";

static char	ME [] = "sxndligparsact";

# include 	"sxnd.h"
# include 	"sxndlig_parsact.h"

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
main_trans_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.main_trans2attr = (struct main_trans2attr*)
		sxrealloc (sxndlig.main_trans2attr, new_size +1, sizeof (struct main_trans2attr));
}

#if 0
static  void
aux_trans_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.aux_trans2attr = (struct aux_trans2attr*)
		sxrealloc (sxndlig.aux_trans2attr, new_size +1, sizeof (struct aux_trans2attr));
}
#endif

static  void
sigma_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.sigma_set = sxba_resize (sxndlig.sigma_set, new_size + 1);
}


static  void
object_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.object2attr = (struct object2attr*)
		sxrealloc (sxndlig.object2attr, new_size +1, sizeof (struct object2attr));
}


static  void
dependancies_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.dependancies2attr =	(struct dependancies2attr*)
		sxrealloc (sxndlig.dependancies2attr, new_size +1, sizeof (struct dependancies2attr));
}


static  void
recognizer_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.recognizer2attr = (struct recognizer2attr*)
		sxrealloc (sxndlig.recognizer2attr, new_size +1, sizeof (struct recognizer2attr));
}


static void
erase_main_trans (main_trans)
    int main_trans;
{
    struct main_trans2attr	*pm;
    int				object, /* aux_trans, */ recognizer;

    pm = sxndlig.main_trans2attr + main_trans;

    if (pm->sigma & LIG_80) {
	XxY_Xforeach (sxndlig.object, main_trans, object) {
	    XxY_erase (sxndlig.object, object);
	}
    }

    if (pm->pile & LIG_80) {
	XxY_Xforeach (sxndlig.recognizer, main_trans, recognizer) {
	    XxY_erase (sxndlig.recognizer, recognizer);
	}
    } 

#if 0
    if (pm->main_symbol < 0) {
	XxY_Xforeach (sxndlig.aux_trans, main_trans, aux_trans) {
	    if (sxndlig.aux_trans2attr [aux_trans].sigma & LIG_80) {
		XxY_Xforeach (sxndlig.object, -aux_trans, object) {
		    XxY_erase (sxndlig.object, object);
		}
	    }

	    XxY_erase (sxndlig.aux_trans, aux_trans);
	}
    }
#endif

    XxY_erase (sxndlig.main_trans, main_trans);
}


static void
mark_sigma_pile (main_trans)
    int main_trans;
{
    struct main_trans2attr	*pm;
    /* struct aux_trans2attr	*pa; */
    struct object2attr		*po;
    int				object, /* aux_trans, */ sigma, recognizer, pile;

    pm = sxndlig.main_trans2attr + main_trans;

    if (pm->sigma & LIG_80) {
	XxY_Xforeach (sxndlig.object, main_trans, object) {
	    sigma = XxY_Y (sxndlig.object, object);
	    SXBA_1_bit (sxndlig.GC_sigma_set, sigma);

	    po = sxndlig.object2attr + object;
	    if (po->pile != SET_NIL) SXBA_1_bit (sxndlig.GC_pile_set, po->pile);
	    if (po->temp != SET_NIL) SXBA_1_bit (sxndlig.GC_pile_set, po->temp);
	}

	sigma = pm->sigma & LIG_7F;
    }
	
    SXBA_1_bit (sxndlig.GC_sigma_set, sigma);
    if (pm->pile != SET_NIL) pile = (pm->pile & LIG_7F), SXBA_1_bit (sxndlig.GC_pile_set, pile);
    if (pm->temp != SET_NIL) SXBA_1_bit (sxndlig.GC_pile_set, pm->temp);

    if (pm->pile & LIG_80) {
	XxY_Xforeach (sxndlig.recognizer, main_trans, recognizer) {
	    pile = XxY_Y (sxndlig.recognizer, recognizer);
	    if (pile != SET_NIL) SXBA_1_bit (sxndlig.GC_pile_set, pile);
	    pile = sxndlig.recognizer2attr [recognizer].temp;
	    if (pile != SET_NIL) SXBA_1_bit (sxndlig.GC_pile_set, pile);
	}
    }

#if 0
    if (pm->main_symbol < 0) {
	XxY_Xforeach (sxndlig.aux_trans, main_trans, aux_trans) {
	    pa = sxndlig.aux_trans2attr + aux_trans;

	    if (pa->sigma & LIG_80) {
		XxY_Xforeach (sxndlig.object, -aux_trans, object) {
		    sigma = XxY_Y (sxndlig.object, object);
		    SXBA_1_bit (sxndlig.GC_sigma_set, sigma);

		    po = sxndlig.object2attr + object;
		    if (po->pile != SET_NIL) SXBA_1_bit (sxndlig.GC_pile_set, po->pile);
		    if (po->temp != SET_NIL) SXBA_1_bit (sxndlig.GC_pile_set, po->temp);
		}

		sigma = pa->sigma & LIG_7F;
	    }

	    SXBA_1_bit (sxndlig.GC_sigma_set, sigma);
	    if (pa->pile != SET_NIL) SXBA_1_bit (sxndlig.GC_pile_set, pa->pile);
	    if (pa->temp != SET_NIL) SXBA_1_bit (sxndlig.GC_pile_set, pa->temp);
	}
    }
#endif
}


static int
sxndlig_GC ()
{
    /* sxndparser vient de faire un GC, on en profite... */
    /* Il est possible de l'appeler a n'importe quel moment. */
    int main_trans, son, father, sigma, pile;

    if (sxndlig.GC_sigma_set == NULL) {
	sxndlig.GC_sigma_set = sxba_calloc (XxY_size (sxndlig.sigma) + 1);
	sxndlig.GC_pile_set = sxba_calloc (XxY_size (sxndlig.piles) + 1);
	sxndlig.GC_to_be_processed = SS_alloc (BASIZE (sxndlig.GC_pile_set));
    }
    else {
	sxba_empty (sxndlig.GC_sigma_set);
	sxba_empty (sxndlig.GC_pile_set);

	if (XxY_top (sxndlig.sigma) >= BASIZE (sxndlig.GC_sigma_set))
	    sxndlig.GC_sigma_set = sxba_resize (sxndlig.GC_sigma_set,
						XxY_size (sxndlig.sigma) + 1);

	if (XxY_top (sxndlig.piles) >= BASIZE (sxndlig.GC_pile_set))
	    sxndlig.GC_pile_set = sxba_resize (sxndlig.GC_pile_set,
					       XxY_size (sxndlig.piles) + 1);
    }

    XxY_foreach (sxndlig.main_trans, main_trans) {
	son = XxY_X (sxndlig.main_trans, main_trans);
	father = XxY_Y (sxndlig.main_trans, main_trans);

	if (XxYxZ_is_erased (parse_stack.parsers, son) ||	
	    (father > 0 && XxYxZ_is_erased (parse_stack.parsers, father))) {
	    erase_main_trans (main_trans);
	}
	else
	    mark_sigma_pile (main_trans);
    }

	/* On "ajoute" les sigma utilises pour l'elagage de la foret partagee. */
    sxba_or (sxndlig.GC_sigma_set, sxndlig.sigma_set);

    sigma = 0;

    while ((sigma = sxba_scan (sxndlig.GC_sigma_set, sigma)) > 0)
	SS_push (sxndlig.GC_to_be_processed, sigma);

    while (!SS_is_empty (sxndlig.GC_to_be_processed)) {
	sigma = SS_pop (sxndlig.GC_to_be_processed);

	while ((sigma = XxY_Y (sxndlig.sigma, sigma)) > 0) {
	    if (SXBA_bit_is_reset_set (sxndlig.GC_sigma_set, sigma))
		SS_push (sxndlig.GC_to_be_processed, sigma);
	    else
		break;
	}
    }

    sigma = XxY_top (sxndlig.sigma) + 1;

    while ((sigma = sxba_0_rlscan (sxndlig.GC_sigma_set, sigma)) > 0)
	XxY_erase (sxndlig.sigma, sigma);

    pile = 0;

    while ((pile = sxba_scan (sxndlig.GC_pile_set, pile)) > 0)
	SS_push (sxndlig.GC_to_be_processed, pile);

    while (!SS_is_empty (sxndlig.GC_to_be_processed)) {
	pile = SS_pop (sxndlig.GC_to_be_processed);

	while ((pile = XxY_Y (sxndlig.piles, pile)) > 0) {
	    if (SXBA_bit_is_reset_set (sxndlig.GC_pile_set, pile))
		SS_push (sxndlig.GC_to_be_processed, pile);
	    else
		break;
	}
    }

    pile = XxY_top (sxndlig.piles) + 1;

    while ((pile = sxba_0_rlscan (sxndlig.GC_pile_set, pile)) > 0) /* A VERIFIER : un peu au pif, la correction... */
	XxY_erase (sxndlig.piles, pile);

    if (sxndlig.for_postparsact.GC != NULL)
	(*sxndlig.for_postparsact.GC) ();

    return 0;
}


static int
sxndlig_action_pop (level, son, father)
    int level, son, father;
{
    /* On est au niveau level du depilage courant entre les parser son et father */
    int path;

    /* Tout le niveau sxndlig.level est traite' avant les niveaux inferieurs. */
    if (level == sxndlig.level) {
	/* C'est un primaire */
	if (sxndlig.prev_main_trans == 0) {
	    /* C'est le premier */
	    /* S'il est unique, il appartient forcemment a tous les chemins. */
	    sxndlig.prev_main_trans = XxY_is_set (&sxndlig.main_trans, son, father);
#ifdef EBUG
	    if (sxndlig.prev_main_trans == 0)
		sxtrap (ME, "action_pop");
#endif
	}
	else {
	    if (!sxndlig.is_paths) {
		sxndlig.is_paths = true;
		/* On installe le premier */
		XxY_set (&sxndlig.paths,
			 XxY_X (sxndlig.main_trans, sxndlig.prev_main_trans),
			 XxY_Y (sxndlig.main_trans, sxndlig.prev_main_trans),
			 &path);
	    }

	    XxY_set (&sxndlig.paths, son, father, &path);
	}
    }
    else
	if (level < sxndlig.level && sxndlig.is_paths) {
	    /* On note les suffixes des depilages pour pouvoir retrouver les primaires.*/
	    XxY_set (&sxndlig.paths, son, father, &path);
	}

    if (sxndlig.for_postparsact.action_pop != NULL)
	(*sxndlig.for_postparsact.action_pop) (level, son, father);

    return 0;
}

static int
sxndlig_action_top (xtriple)
    int	xtriple;
{
    /* On est ds "reducer", on va lancer une reduction sur le triple xtriple. */
    /* parse_stack.ared est positionne' */
    /* On positionne les variables utiles pour le traitement de la reduction. */
    struct triple	*ptriple;
    int 		act_no, post_act;

    act_no = parse_stack.ared->action;

    /* Les fonctions de for_postparsact sont appelees (si non nulles) meme si l'action ou le
       predicat vaut -1 (ce sont elles qui decident quoi faire). */
    if (act_no < 10000) {
	/* On appelle le point d'entree pop (depuis ndparser) ssi les postactprdct l'on demande' */
	/* Les lig n'en ont pas besoin */
	parse_stack.for_parsact.action_pop = sxndlig.for_postparsact.action_pop;

	if (sxndlig.for_postparsact.action_top != NULL)
	    (*sxndlig.for_postparsact.action_top) (act_no);

	return 0;
    }

    sxndlig.xtriple = xtriple;

    if (sxndlig.is_paths) {
	sxndlig.is_paths = false;
	XxY_clear (&sxndlig.paths);
    }

    act_no -= 10000;

    sxndlig.act_no = act_no;
    sxndlig.pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no];
    sxndlig.pclim = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no + 1] -
      (act_no == 0 ? 1 : 2);
    post_act = (act_no == 0 ? -1 : sxndlig.pclim [1]);
    sxndlig.prev_main_trans = 0;

    if ((sxndlig.is_secondary = *sxndlig.pc == SECONDARY)) {
	/* Creation d'une pile from scratch, on ne note rien ds action_pop. */
	sxndlig.level = -1;
	parse_stack.for_parsact.action_pop = sxndlig.for_postparsact.action_pop;

	if (sxndlig.for_postparsact.action_top != NULL)
	    (*sxndlig.for_postparsact.action_top) (post_act);

	return 0;
    }

    parse_stack.for_parsact.action_pop = sxndlig_action_pop;

    sxndlig.level = sxndlig.pc [1];
    sxndlig.pc += 3;

    ptriple = parse_stack.for_reducer.triples + xtriple;

    if (ptriple->is_shift_reduce && parse_stack.ared->lgth == sxndlig.level) {
	/* On note le primaire.*/
	sxndlig.prev_main_trans = XxY_is_set (&sxndlig.main_trans, ptriple->parser, -xtriple);

#ifdef EBUG
    if (sxndlig.prev_main_trans == 0)
	sxtrap (ME, "sxndlig_action_top");
#endif
    }

    if (sxndlig.for_postparsact.action_top != NULL)
	(*sxndlig.for_postparsact.action_top) (post_act);

    return 0;
}



static int
do_action (pile)
    int			pile;
{
    int			pop_nb, *pc;

#ifdef EBUG
    if (sxndlig.act_no == 0 || sxndlig.is_secondary)
      sxtrap (ME, "do_action");
#endif

    if ((pop_nb = sxndlig.pc [-1] - sxndlig.pc [0]) > 0) {
	do {
#ifdef EBUG
	    if (pile == 0)
		sxtrap (ME, "do_action");
#endif

	    pile = XxY_X (sxndlig.piles, pile);
	} while (--pop_nb > 0);
    }

    /* On decale de la longueur du prefixe commun */
    pc = sxndlig.pc + *sxndlig.pc;

    while (++pc <= sxndlig.pclim)
      XxY_set (&sxndlig.piles, pile, *pc, &pile);

    return pile;
}



static bool
do_prdct (pile)
    int pile;
{
    /* On regarde si pile verifie le predicat prdct_no */
    int		*pclim;

#ifdef EBUG
    if (pile == SET_NIL)
	sxtrap (ME, "do_prdct");
#endif

    if (sxndlig.prdct_no == 0)
      /* () Empty Stack */
      return pile == 0;

    /* Cas general : ([..] a b c ... ) */
    if (sxndlig.pprdct_bot == sxndlig.pprdct_top)
      /* ( .. ) */
      return true;

    pclim = sxndlig.pprdct_top;

    do {
      if (pile == 0 || XxY_Y (sxndlig.piles, pile) != *pclim)
	return false;

      pile = XxY_X (sxndlig.piles, pile);
    } while (sxndlig.pprdct_bot < --pclim);

    return true;
}

static int
do_action_prdct (old_pile)
    int	old_pile;
{
    int	new_pile;

    new_pile = (sxndlig.act_no == 0) ? old_pile : do_action (old_pile);
		
    if (!do_prdct (new_pile))
	new_pile = SET_NIL;

    return new_pile;
}



static bool
perform_action_prdct (main_trans)
    int	main_trans;
{
    int				/* aux_trans, */ object;
    /* struct aux_trans2attr	*pt; */
    struct main_trans2attr	*pm;
    struct object2attr		*po;
    bool			ret_val = false;

#ifdef EBUG
    if (main_trans == 0)
	sxtrap (ME, "perform_action_prdct");
#endif

    /* Traitement du symbole principal */
    pm = sxndlig.main_trans2attr + main_trans;

    if ((pm->temp = do_action_prdct (pm->pile)) != SET_NIL)
	ret_val = true;

    if (pm->sigma & LIG_80) {
	XxY_Xforeach (sxndlig.object, main_trans, object) {
	    po = sxndlig.object2attr + object;

	    if ((po->temp = do_action_prdct (po->pile)) != SET_NIL)
		ret_val = true;
	}
    }

#if 0
    if (pm->main_symbol < 0) {
	/* Il y a des auxiliaires */
	XxY_Xforeach (sxndlig.aux_trans, main_trans, aux_trans) {
	    pt = sxndlig.aux_trans2attr + aux_trans;

	    if ((pt->temp = do_action_prdct (pt->pile)) != SET_NIL)
		ret_val = true;

	    if (pt->sigma & LIG_80) {
		XxY_Xforeach (sxndlig.object, -aux_trans, object) {
		    po = sxndlig.object2attr + object;

		    if ((po->temp = do_action_prdct (po->pile)) != SET_NIL)
			ret_val = true;
		}
	    }
	}
    }
#endif

    return ret_val;
}

static bool
REC_perform_action_prdct (main_trans)
    int	main_trans;
{
    int				rec;
    struct main_trans2attr	*pm;
    struct recognizer2attr	*pr;
    bool			ret_val = false;

#ifdef EBUG
    if (main_trans == 0)
	sxtrap (ME, "REC_perform_action_prdct");
#endif

    /* Traitement du symbole principal */
    pm = sxndlig.main_trans2attr + main_trans;

    if (pm->pile == SET_NIL)
	pm->temp = SET_NIL;
    else
	if ((pm->temp = do_action_prdct (pm->pile & LIG_7F)) != SET_NIL)
	    ret_val = true;

    if (pm->pile & LIG_80) {
	/* Il y a des piles auxiliaires */
	XxY_Xforeach (sxndlig.recognizer, main_trans, rec) {
	    pr = sxndlig.recognizer2attr + rec;

	    if ((pr->temp = do_action_prdct (XxY_Y (sxndlig.recognizer, rec))) != SET_NIL)
		ret_val = true;
	}
    }

    return ret_val;
}


static bool
walk_paths (bot, level, f)
    int bot, level;
    bool	(*f)();
{
    int			arc, main_trans;
    bool		ret_val = false;

    if (level == 0) {
	/* bot est le fils d'un object primaire, on calcule l'union des primaires
	   ayant ce fils. De plus, on "memoize", car bot peut etre utilise' plusieurs
	   fois sur la reduction courante. */

	XxY_Xforeach (sxndlig.paths, bot, arc) {
	    main_trans = XxY_is_set (&sxndlig.main_trans,
				    XxY_X (sxndlig.paths, arc),
				    XxY_Y (sxndlig.paths, arc));

	    ret_val |= f (main_trans);
	}
    }
    else {
	XxY_Xforeach (sxndlig.paths, bot, arc)
	    ret_val |= walk_paths (XxY_Y (sxndlig.paths, arc), level - 1, f);
    }

    return ret_val;
}


static bool
put_in_failed (main_trans)
    int main_trans;
{
    int failed;

    return XxYxZ_set (&sxndlig.failed, main_trans, sxndlig.xtriple, sxndlig.prdct_no, &failed);
}

static void
prdct_failed ()
{
    /* On memorise les "echecs". */
    int failed;

    if (sxndlig.is_paths)
	walk_paths (sxndlig.bot, sxndlig.level, put_in_failed);
    else
	XxYxZ_set (&sxndlig.failed, sxndlig.prev_main_trans, sxndlig.xtriple,
		   sxndlig.prdct_no, &failed);
}

/* ATTENTION: suivant l'ordre dans lequel les reductions sont effectue'es le resultat
   peut etre different. Ex: une reduction de symbole primaire P conduit a un predicat
   faux. Plus tard une pile de P (associee a un symbole principal ou auxiliaire) est
   modifiee. Si cette modif conduit a une reevaluation a vrai du predicat, il faut
   recommencer. */
/* On note donc les main_trans et l'action/prdct qui conduit a une evaluation faux.
   Si une valeur est ajoutee a elle-meme ou a l'un de ses symbolesu auxiliaire et
   si cette valeur verifie l'action/predicat alors on relance une evaluation par
   le parser. */

static bool
perform_prdct ()
{
    /* On effectue en une operation la composition parsact, parsprdct. */
    int		*pact_top, *pprdct_top;
    bool	ret_val;

    /* On commence par verifier la compatibilite des push de l'action avec le
       predicat. */

    if (sxndlig.prdct_no == 0)
      {
	if ((sxndlig.pclim - sxndlig.pc) != 0)
	  {
	    prdct_failed ();
	    return false;
	  }
      }
    else
    {
      sxndlig.pprdct_bot = sxndlig_common.code.prdct_or_act_code +
	sxndlig_common.code.prdct_or_act_disp [sxndlig.prdct_no];

      if (*sxndlig.pprdct_bot == PRIMARY)
	sxndlig.pprdct_bot += 3;

      pprdct_top = sxndlig.pprdct_top = sxndlig_common.code.prdct_or_act_code +
	sxndlig_common.code.prdct_or_act_disp [sxndlig.prdct_no + 1] - 2;

      pact_top = sxndlig.pclim;

      while (pprdct_top > sxndlig.pprdct_bot && pact_top > sxndlig.pc)
	{
	  if (*pprdct_top-- != *pact_top--)
	    {
	      prdct_failed ();
	      return false;
	    }
	}

      if (sxndlig.level < 0 /* Action secondaire: ( a b c ) */ &&
	  sxndlig.pclim - sxndlig.pc < sxndlig.pprdct_top - sxndlig.pprdct_bot
	  /* longueur  incompatibles */)
	{
	  prdct_failed ();
	  return false;
	}
    }

    if (sxndlig.level < 0)
	/* Le calcul de la pile initiale sera fait ds action_new_top. */
	return true;

    ret_val = sxndlig.is_paths ?
	/* Primaire multiples */ walk_paths (sxndlig.bot, sxndlig.level,
					     sxplocals.mode.mode == SXPARSER
					     ? perform_action_prdct
					     : REC_perform_action_prdct) :
        (/* Primaire unique */ sxplocals.mode.mode == SXPARSER
	 ? perform_action_prdct (sxndlig.prev_main_trans)
	 : REC_perform_action_prdct (sxndlig.prev_main_trans));

    if (!ret_val)
	prdct_failed ();

    return ret_val;
}

static int
sxndlig_action_bot (bot)
    int bot;
{
    /* On vient de terminer le depilage, on se contente de noter le parser atteint
       La parsact sera effectuee avec le test du predicat. */
    sxndlig.bot = bot;

    if (sxndlig.for_postparsact.action_bot != NULL)
	(*sxndlig.for_postparsact.action_bot) (bot);

    return 0;
}


static int
execute_action (act_no, pile)
    int	act_no, pile;
{
    /* act_no > 0 et primaire */
    int	pop_nb, *pc, *pclim;

    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no];

#ifdef EBUG
    if (act_no == 0 || *pc == SECONDARY)
      sxtrap (ME, "execute_action");
#endif

    pclim = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no + 1] - 2;

    if ((pop_nb = pc [2] - pc [3]) > 0) {
      /* pc [2] == longueur du test du predicat
	 pc [3] == longueur du prefixe commun entre action et predicat. */
	do {
#ifdef EBUG
	    if (pile == 0)
		sxtrap (ME, "execute_action");
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
execute_prdct (prdct_no, pile)
    int prdct_no, pile;
{
    /* On regarde si pile verifie le predicat prdct_no */
    int	*pc, *pclim;

#ifdef EBUG
    if (pile == SET_NIL)
	sxtrap (ME, "execute_prdct");
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
process_failure (trans, pile)
    int trans, pile;
{
    /* Une nouvelle pile a ete ajoutee a trans, on regarde si les predicats
       qui en dependent peuvent maintenant s'evaluer a vrai. */
    int main_trans, x, xtriple, ref, act_no, prdct_no, new_pile;

/*    main_trans = (trans > 0) ? trans : XxY_X (sxndlig.aux_trans, -trans > 0); */
#ifdef EBUG
    if (trans <= 0)
	sxtrap (ME, "process_failure");
#endif

    main_trans = trans;

    XxYxZ_Xforeach (sxndlig.failed, main_trans, x) {
	xtriple = XxYxZ_Y (sxndlig.failed, x);
	ref = parse_stack.for_reducer.triples [xtriple].ref;
	act_no = sxplocals.SXP_tables.reductions [ref].action - 10000;
	prdct_no = XxYxZ_Z (sxndlig.failed, x);

	new_pile = (act_no == 0) ? pile : execute_action (act_no, pile);

	if (execute_prdct (prdct_no, new_pile)) {
	    /* Il faut "reevaluer" xtriple */
	    if (parse_stack.for_reducer.triples [xtriple].re_do == 0) {
		parse_stack.for_reducer.triples [xtriple].re_do =
		    parse_stack.for_reducer.re_do;
		parse_stack.for_reducer.re_do = xtriple;
	    }

	    XxYxZ_erase (sxndlig.failed, x);
	}
    }
}



static void
propagate (aux_trans, sigma, pile)
    int aux_trans, sigma, pile;
{
    int				main_trans, dependancy, prev_trans, new_object, new_pile, new_sigma, new_new_sigma, prev_symb;
    struct dependancies2attr	*pattr;
    bool			start = true, is_secondary;
#if 0
    int		 prev_main_trans, prev_main_symb, main_symbol;
#endif

#ifdef EBUG
    if (aux_trans <= 0)
	sxtrap (ME, "process_failure");
#endif

#if 0
    if (aux_trans > 0)
	main_trans = aux_trans;
    else
	main_trans = XxY_X (sxndlig.aux_trans, -aux_trans);
#endif

    main_trans = aux_trans;
    
    XxY_Yforeach (sxndlig.dependancies, main_trans, dependancy) {
	pattr = sxndlig.dependancies2attr + dependancy;
	new_pile = (pattr->act_no == 0) ? pile : execute_action (pattr->act_no, pile);

	if (execute_prdct (pattr->prdct_no, new_pile)) {
	    if (start) {
		start = false;

#if 0
		if (aux_trans < 0) {
		    /* l'objet source est associe a un symbole auxiliaire */
		    if ((main_symbol = sxndlig.main_trans2attr [main_trans].main_symbol) < 0)
			main_symbol = -main_symbol;

		    XxY_set (&sxndlig.sigma, main_symbol, sigma, &sigma);
		}
#endif
	    }

	    prev_trans = XxY_X (sxndlig.dependancies, dependancy);

	    if (prev_trans == sxndlig.cur_trans) {
		/* La detection des grammaires cycliques peut se faire ds csynt. */
		sxtrap (ME, "Processing of cyclic grammars is not yet implemented.");
	    }

	    is_secondary = (sxndlig_common.code.prdct_or_act_code [sxndlig_common.code.prdct_or_act_disp
								   [pattr->prdct_no]] == SECONDARY);
	    
#ifdef EBUG
	    if (prev_trans <= 0)
		sxtrap (ME, "process_failure");
#endif

#if 0
	    if (prev_trans > 0) {
		/* C'est un main_trans */
		prev_symb = sxndlig.main_trans2attr [prev_trans].main_symbol;
		
		if (prev_symb < 0)
		    prev_symb = -prev_symb;
	    }
	    else {
		/* C'est une aux_trans */
		prev_symb = XxY_Y (sxndlig.aux_trans, -prev_trans);

		if (is_secondary)
		{
		    prev_main_trans = XxY_X (sxndlig.aux_trans, -prev_trans);
		    prev_main_symb = -sxndlig.main_trans2attr [prev_main_trans].main_symbol;
		}
	    }
#endif

	    prev_symb = sxndlig.main_trans2attr [prev_trans].main_symbol;

#ifdef EBUG
	    if (prev_symb <= 0)
		sxtrap (ME, "process_failure");
#endif
	    

	    XxY_set (&sxndlig.sigma, prev_symb, sigma, &new_sigma);

	    if (is_secondary)
	    {
	      /* new_pile est "correct" et c'est un objet secondaire qui ne sera plus jamais
		 "manipule". On note donc new_sigma qui permettra de valider les arbres d'analyse
		 correspondant (sachant qu'ils n'appartiendront a la foret d'analyse que s'ils sont
		 accessibles depuis la racine). */
	      /* ATTENTION, sxndlig.cur_symb peut ne pas etre le symbol principal */
#if 0
		if (prev_trans < 0)
		    XxY_set (&sxndlig.sigma, prev_main_symb, new_sigma, &new_new_sigma);
		else
		    new_new_sigma = new_sigma;
#endif
		
		new_new_sigma = new_sigma;
		SXBA_1_bit (sxndlig.sigma_set, new_new_sigma);
	    }

	    if (XxY_set (&sxndlig.object, prev_trans, new_sigma, &new_object)) {
#ifdef EBUG
		if (sxndlig.object2attr [new_object].pile != new_pile)
		    sxtrap (ME, "propagate");
#endif
	    }
	    else {
#if 0
		if (prev_trans > 0)
		    sxndlig.main_trans2attr [prev_trans].sigma |= LIG_80;
		else
		    sxndlig.aux_trans2attr [-prev_trans].sigma |= LIG_80;
#endif

		sxndlig.main_trans2attr [prev_trans].sigma |= LIG_80;
		
		sxndlig.object2attr [new_object].pile = new_pile;
		process_failure (prev_trans, new_pile);
		propagate (prev_trans, new_sigma, new_pile);
	    }
	}
    }
}



/* cas de SXRECOGNIZER */
static void
REC_propagate (main_trans, pile)
    int main_trans, pile;
{
    int				dependancy, prev_trans, new_pile, new_rec;
    struct dependancies2attr	*pd;
    struct main_trans2attr	*pm;

    XxY_Yforeach (sxndlig.dependancies, main_trans, dependancy) {
	pd = sxndlig.dependancies2attr + dependancy;
	new_pile = (pd->act_no == 0) ? pile : execute_action (pd->act_no, pile);

	if (execute_prdct (pd->prdct_no, new_pile)) {
	    prev_trans = XxY_X (sxndlig.dependancies, dependancy);

	    if (prev_trans == sxndlig.cur_trans) {
		/* La detection des grammaires cycliques peut se faire ds csynt. */
		sxtrap (ME, "Processing of cyclic grammars is not yet implemented.");
	    }

	    pm = sxndlig.main_trans2attr + prev_trans;

	    if ((pm->pile & LIG_7F) != (unsigned int)new_pile) {
		if (!XxY_set (&sxndlig.recognizer, prev_trans, new_pile, &new_rec)) {
		    pm->pile |= LIG_80;

		    sxndlig.recognizer2attr [new_rec].temp = SET_NIL;
		    process_failure (prev_trans, new_pile);
		    REC_propagate (prev_trans, new_pile);
		}
	    }
	}
    }
}

static void
install_attr (old_sigma, new_pile)
    int	old_sigma, new_pile;
{
    int				new_sigma, new_object;
    bool			is_secondary;
    struct main_trans2attr	*pm;
    /* struct aux_trans2attr	*pt; */
#if 0
    int		new_new_sigma, main_trans, main_symb;
#endif

    if (new_pile != SET_NIL) {
      XxY_set (&sxndlig.sigma, sxndlig.cur_symb, old_sigma, &new_sigma);

      /* new_pile est "correct" et c'est un objet secondaire qui ne sera plus jamais
	 "manipule". On note donc new_sigma qui permettra de valider les arbres d'analyse
	 correspondant (sachant qu'ils n'appartiendront a la foret d'analyse que s'ils sont
	 accessibles depuis la racine). */
      /* ATTENTION, sxndlig.cur_symb peut ne pas etre le symbol principal */
      is_secondary = (sxndlig_common.code.prdct_or_act_code
		      [sxndlig_common.code.prdct_or_act_disp [sxndlig.prdct_no]] == SECONDARY);
	    
#ifdef EBUG
      if (sxndlig.cur_trans <= 0)
	  sxtrap (ME, "install_attr");
#endif

#if 0
      if (sxndlig.cur_trans > 0) {
	/* main */
	if (is_secondary)
	    SXBA_1_bit (sxndlig.sigma_set, new_sigma);

	pm = sxndlig.main_trans2attr + sxndlig.cur_trans;

	if (pm->sigma != SET_NIL) {
	  XxY_set (&sxndlig.object, sxndlig.cur_trans, new_sigma, &new_object);
	  sxndlig.object2attr [new_object].pile = new_pile;
	  pm->sigma |= LIG_80;
      }
	else {
	  /* 1er objet */
	  pm->sigma = new_sigma;
	  pm->pile = new_pile;
      }
    }
      else {
	/* Auxiliaire */
	if (is_secondary)
	{
	    main_trans = XxY_X (sxndlig.aux_trans, -sxndlig.cur_trans);
	    main_symb = -sxndlig.main_trans2attr [main_trans].main_symbol;
	    XxY_set (&sxndlig.sigma, main_symb, new_sigma, &new_new_sigma);
	    SXBA_1_bit (sxndlig.sigma_set, new_new_sigma);
	}

	pt = sxndlig.aux_trans2attr - sxndlig.cur_trans;

	if (pt->sigma != SET_NIL) {
	  XxY_set (&sxndlig.object, sxndlig.cur_trans, new_sigma, &new_object);
	  sxndlig.object2attr [new_object].pile = new_pile;
	  pt->sigma |= LIG_80;
      }
	else {
	  /* 1er objet */
	  pt->sigma = new_sigma;
	  pt->pile = new_pile;
      }
    }
#endif

      if (is_secondary)
	  SXBA_1_bit (sxndlig.sigma_set, new_sigma);

      pm = sxndlig.main_trans2attr + sxndlig.cur_trans;

      if (pm->sigma != SET_NIL) {
	XxY_set (&sxndlig.object, sxndlig.cur_trans, new_sigma, &new_object);
	sxndlig.object2attr [new_object].pile = new_pile;
	pm->sigma |= LIG_80;
    }
      else {
	/* 1er objet */
	pm->sigma = new_sigma;
	pm->pile = new_pile;
    }

      if (sxndlig.is_old_main_trans) {
	process_failure (sxndlig.cur_trans, new_pile);
	propagate (sxndlig.cur_trans, new_sigma, new_pile);
    }
  }
}


static void
REC_install_attr (new_pile)
    int	new_pile;
{
    int				new_rec;
    struct main_trans2attr	*pm;

    if (new_pile != SET_NIL) {
	pm = sxndlig.main_trans2attr + sxndlig.cur_trans;

	if (pm->pile == SET_NIL) {
	    /* 1ere pile */
	    pm->pile = new_pile;
	}
	else {
	    if ((pm->pile & LIG_7F) != (unsigned int)new_pile) {
		if (!XxY_set (&sxndlig.recognizer, sxndlig.cur_trans, new_pile, &new_rec)) {
		    sxndlig.recognizer2attr [new_rec].temp = SET_NIL;
		    pm->pile |= LIG_80;
		}
	    }
	}

	if (sxndlig.is_old_main_trans) {
	    process_failure (sxndlig.cur_trans, new_pile);
	    REC_propagate (sxndlig.cur_trans, new_pile);
	}
    }
}



static bool
use_action_prdct (main_trans)
    int	main_trans;
{
    struct dependancies2attr	*pd;
    /* struct aux_trans2attr	*pt; */
    struct main_trans2attr	*pm;
    int				sigma, dependancy, object, pile;
    bool			is_old;
#if 0
    int				aux_trans, main_symbol;
#endif

#ifdef EBUG
    if (main_trans == 0)
	sxtrap (ME, "use_action_prdct");
#endif

    is_old = XxY_set (&sxndlig.dependancies, sxndlig.cur_trans, main_trans, &dependancy);
    pd = sxndlig.dependancies2attr + dependancy;

    if (is_old) {
#ifdef EBUG
	if (pd->act_no != sxndlig.act_no || pd->prdct_no != sxndlig.prdct_no)
	    sxtrap (ME, "use_action_prdct");
#endif
    }
    else {
	pd->act_no = sxndlig.act_no;
	pd->prdct_no = sxndlig.prdct_no;
    }
	
    /* Traitement du symbole principal */
    pm = sxndlig.main_trans2attr + main_trans;

    install_attr (pm->sigma & LIG_7F, pm->temp /* new_pile */);

    if (pm->sigma & LIG_80) {
	XxY_Xforeach (sxndlig.object, main_trans, object) {
	  if ((pile = sxndlig.object2attr [object].temp) != SET_NIL) {
	    sigma = XxY_Y (sxndlig.object, object);
	    install_attr (sigma, pile);
	  }
	}
    }

#if 0
    if (pm->main_symbol < 0) {
	/* Il y a des auxiliaires */
	XxY_Xforeach (sxndlig.aux_trans, main_trans, aux_trans) {
	    pt = sxndlig.aux_trans2attr + aux_trans;

	    if (pt->temp != SET_NIL) {
		XxY_set (&sxndlig.sigma, -pm->main_symbol, pt->sigma & LIG_7F, &sigma);
		install_attr (sigma, pt->temp);
	      }

	    if (pt->sigma & LIG_80) {
		XxY_Xforeach (sxndlig.object, -aux_trans, object) {
		  if ((pile = sxndlig.object2attr [object].temp) != SET_NIL) {
		    sigma = XxY_Y (sxndlig.object, object);
		    XxY_set (&sxndlig.sigma, -pm->main_symbol, sigma, &sigma);
		    install_attr (sigma, pile);
		  }
		}
	    }
	}
    }
#endif

    return true;
}




static bool
REC_use_action_prdct (main_trans)
    int	main_trans;
{
    struct dependancies2attr	*pd;
    struct main_trans2attr	*pm;
    struct recognizer2attr	*pr;
    int				dependancy, rec;
    bool			is_old;

#ifdef EBUG
    if (main_trans == 0)
	sxtrap (ME, "REC_use_action_prdct");
#endif

    is_old = XxY_set (&sxndlig.dependancies, sxndlig.cur_trans, main_trans, &dependancy);
    pd = sxndlig.dependancies2attr + dependancy;

    if (is_old) {
#ifdef EBUG
	if (pd->act_no != sxndlig.act_no || pd->prdct_no != sxndlig.prdct_no)
	    sxtrap (ME, "REC_use_action_prdct");
#endif
    }
    else {
	pd->act_no = sxndlig.act_no;
	pd->prdct_no = sxndlig.prdct_no;
    }
	
    /* Traitement du symbole principal */
    pm = sxndlig.main_trans2attr + main_trans;

    if (pm->temp != SET_NIL)
	REC_install_attr (pm->temp /* new_pile */);

    if (pm->pile & LIG_80) {
	/* Il y a des piles auxiliaires */
	XxY_Xforeach (sxndlig.recognizer, main_trans, rec) {
	    pr = sxndlig.recognizer2attr + rec;

	    if (pr->temp != SET_NIL)
		REC_install_attr (pr->temp);

	}
    }

    return true;
}

/* AXIOME: Si une aux_trans est reevaluee, aucun objet precedent ne peut disparaitre.
   Une reevaluation peut ne pas etre "complete", cas ou l'appel se fait depuis un
   do_limited. */
/* Le resultat de l'execution d'une parsact est associe a la aux_trans (son, father),
   ce resultat peut etre modifie' par une nouvelle evaluation.  Ce qui veut dire que le
   resultat de la premiere evaluation a pu conduire a un predicat e'value' a faux (et
   les aux_trans correspondantes non construites) alors que le nouveau calcul peut
   conduire au meme predicat evalue' a vrai.  Il faut donc forcer la re'e'valuation.
   Pour ce faire parse_stack.triples_bot est mis a 1. On pourrait etre moins brutal
   et envisager un mecanisme plus selectif permettant une reexecution des "triplets"
   de triples au cas par cas. Si une main_trans de bot a new_top existe deja et si
   cette aux_trans est primaire (a deja ete utilisee comme primaire par au moins
   une reduction precedente) et si un nouvel objet est ajoute' alors on reevalue
   tous les triplets ayant new_top comme parser. */
static int
sxndlig_action_new_top (bot, new_top, symbol)
    int bot, new_top, symbol;
{
    /* Si new_top == 0, echec syntaxique. */
    /* Attention, il peut exister le meme "symbol" avec 2 "new_top" differents. */
    int				new_pile, *pc, main_trans;
    bool			is_last_main_trans;
    struct main_trans2attr	*pm;
    /* struct aux_trans2attr	*pa; */
#if 0
    int		main_symbol;
#endif

	/* pas obligatoirement "parse_stack.ared->reduce == 1" a cause des productions
	   simples. */
    is_last_main_trans = parse_stack.current_lahead == sxplocals.SXP_tables.P_tmax &&
      new_top > 0 && XxYxZ_X (parse_stack.parsers, new_top) == sxplocals.SXP_tables.final_state;
    /* Fin de l'analyse */
	
    if (is_last_main_trans)
	sxndlig.last_main_symb = symbol;

    if (parse_stack.ared->action < 10000) 
      {
	    /* On conserve les LHS des regles qui n'ont pas d'action sur les piles (et donc pas
	       de predicat), afin de les valider en fin d'analyse. */
	if (symbol >= BASIZE (sxndlig.symbol_set))
	  sxndlig.symbol_set = sxba_resize (sxndlig.symbol_set, XxY_size (parse_stack.symbols) + 1);

	SXBA_1_bit (sxndlig.symbol_set, symbol);

	if (sxndlig.for_postparsact.action_new_top != NULL)
	    (*sxndlig.for_postparsact.action_new_top) (bot, new_top, symbol);

	return 0;
      }

    if (new_top != 0) {
	sxndlig.is_old_main_trans = XxY_set (&sxndlig.main_trans, bot, new_top, &main_trans);
	pm = sxndlig.main_trans2attr + main_trans;

	if (!sxndlig.is_old_main_trans) {
	    /* First main */
	    pm->main_symbol = symbol;
	    pm->sigma = SET_NIL;
	    pm->pile = SET_NIL; /* pour GC () */
	    pm->temp = SET_NIL; /* pour GC () */
	    sxndlig.cur_trans = main_trans;
	    sxndlig.cur_symb = symbol;

	    if (new_top <= 0)
		SS_push (sxndlig.main_trans_to_be_erased, main_trans);
	}
	else {
#ifdef EBUG
	  if (pm->main_symbol != symbol)
	      sxtrap (ME, "sxndlig_action_new_top");
#endif

	  sxndlig.cur_trans = main_trans;
	  sxndlig.cur_symb = symbol;

#if 0
	  main_symbol = pm->main_symbol;
	    if (main_symbol == symbol || main_symbol == -symbol) {
		/* On retrouve le main symbol */
		sxndlig.cur_trans = main_trans;
		sxndlig.cur_symb = symbol;
	    }
	    else {
		/* symbol est auxiliaire */
		sxndlig.cur_symb = -symbol;

		if (!XxY_set (&sxndlig.aux_trans,
			      main_trans,
			      sxndlig.cur_symb,
			      &sxndlig.cur_trans)) {
		    pa = sxndlig.aux_trans2attr + sxndlig.cur_trans;
		    pa->sigma = SET_NIL;
		    pa->pile = SET_NIL; /* pour GC () */
		    pa->temp = SET_NIL; /* pour GC () */
		}

		sxndlig.cur_trans = -sxndlig.cur_trans;

		if (main_symbol > 0)
		    pm->main_symbol = -main_symbol;
	    }
#endif
	}

	if (sxndlig.is_secondary) {
	    /* On fabrique une nouvelle pile from scratch. */
	    new_pile = 0;

	    if (sxndlig.pc < sxndlig.pclim) {
		pc = sxndlig.pc + 1;

		do {
		    XxY_set (&sxndlig.piles, new_pile, *pc, &new_pile);
		} while (++pc <= sxndlig.pclim);
	    }

	    /* Pour l'instant, On n'a verifie' que la compatibilite des  "longueurs" */
	    if (execute_prdct (sxndlig.prdct_no, new_pile))
	    {
		if (symbol == 0)
		    REC_install_attr (new_pile);
		else
		    install_attr (0, new_pile);
	    }
	}
	else {
	    if (sxndlig.is_paths)
		/* Primaires multiples */ 
		walk_paths (sxndlig.bot,
			    sxndlig.level,
			    (symbol == 0) ? REC_use_action_prdct : use_action_prdct);
	    else {
		/* Primaire unique */
		if (symbol == 0)
		    REC_use_action_prdct (sxndlig.prev_main_trans);
		else
		    use_action_prdct (sxndlig.prev_main_trans);
	    }
	}
    }

    if (sxndlig.for_postparsact.action_new_top != NULL)
	(*sxndlig.for_postparsact.action_new_top) (bot, new_top, symbol);

    return 0;
}


static SXBA		dependancy_set, should_be_kept;
#if 0
static SXBA		unit_rule_set;
#endif
static XxY_header	sxndlig_rule_pattern, rhsXrule;

static void
exploit_sigma (sigma)
    int sigma;
{
    int	prev_symbol, next_symbol, rule;
#if 0
    int		unit_rule;
#endif

#ifdef EBUG
    if (sigma == SET_NIL || sigma == 0)
	sxtrap (ME, "exploit_sigma");
#endif

    prev_symbol = XxY_X (sxndlig.sigma, sigma);

#ifdef EBUG
    if (prev_symbol <= 0)
	sxtrap (ME, "exploit_sigma");
#endif

    do {
	sigma = XxY_Y (sxndlig.sigma, sigma);

	if (sigma == 0)
	    next_symbol = 0;
	else {
	    if (!SXBA_bit_is_reset_set (dependancy_set, sigma))
		break;

	    next_symbol = XxY_X (sxndlig.sigma, sigma);
	}

	XxY_set (&sxndlig_rule_pattern, prev_symbol, next_symbol, &rule);

#if 0
/* 21/4/95 A VOIR */
	if (next_symbol >= 0)
	    XxY_set (&sxndlig_rule_pattern, prev_symbol, next_symbol, &rule);

	else {
	    next_symbol = -next_symbol;
	    unit_rule = XxY_is_set (&parse_stack.unit_rules, prev_symbol, next_symbol);

#ifdef EBUG
	    if (unit_rule == 0)
		sxtrap (ME, "exploit_sigma");
#endif
	    SXBA_1_bit (unit_rule_set, unit_rule);
	}
#endif

	prev_symbol = next_symbol;
    } while (sigma != 0);
}




static bool
search_in_suffix (lhs_symb, rhs_symb)
    int	lhs_symb, rhs_symb;
{
    /* Retourne true ssi rhs_symb se trouve en RHS de l'une
       des regles ayant lhs_symb en LHS. */
    int		rule_no, rhs, tnt;
    bool	found = false;

    XxY_Xforeach (parse_stack.sf_rule, lhs_symb, rule_no) {
	rhs = XxY_Y (parse_stack.sf_rule, rule_no);
	tnt = 0;

	while (rhs > 0) {
	    tnt = XxY_X (parse_stack.sf_rhs, rhs);

	    if (tnt == rhs_symb) {
		/* On l'a trouve' */
		SXBA_1_bit (should_be_kept, rule_no);
		found = true;
		break;
	    }

	    rhs = XxY_Y (parse_stack.sf_rhs, rhs);
	}
    
	if (rhs == 0 &&
	    tnt > 0 &&
	    XxY_X (parse_stack.symbols, tnt) > sxplocals.SXP_tables.P_xnbpro) {
	    /* NT decrivant le partage d'un suffixe */
	    /* On l'explore recursivement */
	    if (search_in_suffix (tnt, rhs_symb)) {
		SXBA_1_bit (should_be_kept, rule_no);
		found = true;
	    }
	}
    }

    return found;
}



static int
sxndlig_action_final ()
{
    /* On a termine' l'e'valuation d'un niveau, On prepare le niveau suivant. */
    int				main_trans, rule_no, tnt, rule_pattern,
                                lhs_symb, rhs_symb, rhs, sigma, unused, x;
    SXBA                        symb_set;
    int                         *tbp_stack;
#if 0
    int		unit_rule;
#endif

    XxY_clear (&sxndlig.dependancies);
    XxYxZ_clear (&sxndlig.failed);

    /* On supprime les main_trans dont le 2eme composant est negatif (OBLIGATOIRE!). */
    while (!SS_is_empty (sxndlig.main_trans_to_be_erased)) {
	main_trans = SS_pop (sxndlig.main_trans_to_be_erased);
	erase_main_trans (main_trans);
    }

    if (parse_stack.start_symbol != 0 &&
	sxplocals.mode.mode == SXPARSER &&
	sxndlig.last_main_symb != 0) {
	/* Fin correcte, on exploite les "piles" restantes */
	dependancy_set = sxba_calloc (XxY_top (sxndlig.sigma) + 1);

#if 0
	if (parse_stack.is_unit_rules)
	    unit_rule_set = sxba_calloc (XxY_top (parse_stack.unit_rules) + 1);
#endif

	XxY_alloc (&sxndlig_rule_pattern, "sxndlig_rule_pattern", 2 * XxY_top (sxndlig.sigma),
		   1, 0, 0, NULL, stdout_or_NULL);

	/*
	   Faux : il peut y avoir plusieurs "last_main_symb" ds le cas ou l'axiome est
	   defini par plusieurs regles ambigues.
	   XxY_set (&sxndlig_rule_pattern, parse_stack.start_symbol, sxndlig.last_main_symb, &rule_pattern);
	   */

	symb_set = sxba_calloc (XxY_top (parse_stack.symbols) + 1);
	tbp_stack = SS_alloc (XxY_top (parse_stack.symbols) + 1);
	
	sigma = 0;
	
	while ((sigma = sxba_scan (sxndlig.sigma_set, sigma)) > 0)
	  exploit_sigma (sigma);
	
	sxfree (dependancy_set);

	should_be_kept = sxba_calloc (XxY_top (parse_stack.sf_rule) + 1);
	SXBA_1_bit (sxndlig.symbol_set, parse_stack.start_symbol);

	/* On s'occupe des regles n'ayant pas de pile. */
	/* Les normales... */
	for (rule_no = XxY_top (parse_stack.sf_rule); rule_no > 0; rule_no--) 
	  {
	    lhs_symb = XxY_X (parse_stack.sf_rule, rule_no);

	    if (SXBA_bit_is_set (sxndlig.symbol_set, lhs_symb))
	      SXBA_1_bit (should_be_kept, rule_no);
	  }
	
#if 0
	/* ... puis les unit_rules */
	if (parse_stack.is_unit_rules)
	{
	    for (rule_no = XxY_top (parse_stack.unit_rules); rule_no > 0; rule_no--) 
	    {
		lhs_symb = XxY_X (parse_stack.unit_rules, rule_no);

		if (SXBA_bit_is_set (sxndlig.symbol_set, lhs_symb))
		{
		    rhs_symb = XxY_Y (parse_stack.unit_rules, rule_no);
		    unit_rule = XxY_is_set (&parse_stack.unit_rules, lhs_symb, rhs_symb);

#ifdef EBUG
		    if (unit_rule == 0)
			sxtrap (ME, "sxndlig_action_final");
#endif
		    SXBA_1_bit (unit_rule_set, unit_rule);
		}
	    }
	}
#endif

	/* On s'occupe des regles ayant meme LHS. */
	XxY_foreach (sxndlig_rule_pattern, rule_pattern) {
	    /* On note les regles lhs_symb -> ... rhs_symb ... dans should_be_kept */
	    lhs_symb = XxY_X (sxndlig_rule_pattern, rule_pattern);
	    rhs_symb = XxY_Y (sxndlig_rule_pattern, rule_pattern);

	    XxY_Xforeach (parse_stack.sf_rule, lhs_symb, rule_no) {
		if (rhs_symb == 0)
		    SXBA_1_bit (should_be_kept, rule_no);
		else {
		    rhs = XxY_Y (parse_stack.sf_rule, rule_no);
		    tnt = 0;

		    while (rhs > 0) {
			tnt = XxY_X (parse_stack.sf_rhs, rhs);

			if (tnt == rhs_symb) {
			    /* On l'a trouve' */
			    SXBA_1_bit (should_be_kept, rule_no);
			    break;
			}

			rhs = XxY_Y (parse_stack.sf_rhs, rhs);
		    }

		    if (rhs == 0 &&
			tnt > 0 &&
			XxY_X (parse_stack.symbols, tnt) > sxplocals.SXP_tables.P_xnbpro) {
			/* NT decrivant le partage d'un suffixe */
			/* On l'explore recursivement */
			if (search_in_suffix (tnt, rhs_symb))
			    SXBA_1_bit (should_be_kept, rule_no);
		    }
		}
	    }
	}

	rule_no = 0;

	while ((rule_no = sxba_0_lrscan (should_be_kept, rule_no)) > 0) 
	  {
	    lhs_symb = XxY_X (parse_stack.sf_rule, rule_no);
	    SXBA_1_bit (symb_set, lhs_symb);
	    XxY_erase (parse_stack.sf_rule, rule_no);
	  }

#if 0
	/* Puis on s'occupe des unit rules */
	if (parse_stack.is_unit_rules) {
	    rule_no = 0;

	    while ((rule_no = sxba_0_lrscan (unit_rule_set, rule_no)) > 0)
	      {
		lhs_symb = XxY_X (parse_stack.unit_rules, rule_no);
		SXBA_1_bit (symb_set, lhs_symb);
		XxY_erase (parse_stack.unit_rules, rule_no);
	      }
	}

	if (parse_stack.is_unit_rules)
	    sxfree (unit_rule_set);
#endif

	XxY_free (&sxndlig_rule_pattern);

	sxfree (should_be_kept);
	
	    /* Si une lhs disparait (toutes les regles la definissant sont enlevees)
	       toutes les regles ou elle apparait en RHS sont supprimees. */

	    /* symb de symb_set sont des candidats */

	lhs_symb = 0;
	
	while ((lhs_symb = sxba_scan_reset (symb_set, lhs_symb)) > 0)
	  {
	    XxY_Xforeach (parse_stack.sf_rule, lhs_symb, rule_no) {
	      break;
	    }

#if 0
	    if (rule_no == 0) 
	      {
		XxY_Xforeach (parse_stack.unit_rules, lhs_symb, rule_no) {
		  break;
		}
	      }
#endif

	    if (rule_no == 0)
	      SS_push (tbp_stack, lhs_symb);
	  }
	
	if (!SS_is_empty (tbp_stack)) 
	  {
	    XxY_alloc (&rhsXrule, "rhsXrule", XxY_top (parse_stack.sf_rhs),
		       1, 1, 0, NULL, stdout_or_NULL);

	    for (rule_no = XxY_top (parse_stack.sf_rule); rule_no > 0; rule_no--) 
	      {
		rhs = XxY_Y (parse_stack.sf_rule, rule_no);

		while (rhs > 0) {
		  tnt = XxY_X (parse_stack.sf_rhs, rhs);

		  if (tnt > 0)
		    XxY_set (&rhsXrule, tnt, rule_no, &unused);

		  rhs = XxY_Y (parse_stack.sf_rhs, rhs);
		}
	      }
	
#if 0
	    for (rule_no = XxY_top (parse_stack.unit_rules); rule_no > 0; rule_no--) 
	      {
		rhs = XxY_Y (parse_stack.unit_rules, rule_no);
		XxY_set (&rhsXrule, rhs, -rule_no, &unused);
	      }
#endif

	    while(!SS_is_empty (tbp_stack))
	      {
		while(!SS_is_empty (tbp_stack)) 
		  {
		    lhs_symb = SS_pop (tbp_stack);

		    XxY_Xforeach (rhsXrule, lhs_symb, x)
		      {
			rule_no = XxY_Y (rhsXrule, x);
		    
			if (rule_no > 0)
			  {
			    if (!XxY_is_erased (parse_stack.sf_rule, rule_no))
			      {
				lhs_symb = XxY_X (parse_stack.sf_rule, rule_no);
				SXBA_1_bit (symb_set, lhs_symb);
				XxY_erase (parse_stack.sf_rule, rule_no);
			      }
			  }
			else
			  {
			    rule_no = -rule_no;
			
#if 0
			    if (!XxY_is_erased (parse_stack.unit_rules, rule_no))
			      {
				lhs_symb = XxY_X (parse_stack.unit_rules, rule_no);
				SXBA_1_bit (symb_set, lhs_symb);
				XxY_erase (parse_stack.unit_rules, rule_no);
			      }
#endif			    
			  }
		      }
		  }

		    /* symb de symb_set sont des candidats */

		lhs_symb = 0;
	
		while ((lhs_symb = sxba_scan_reset (symb_set, lhs_symb)) > 0)
		  {
		    XxY_Xforeach (parse_stack.sf_rule, lhs_symb, rule_no) {
		      break;
		    }

#if 0
		    if (rule_no == 0) 
		      {
			XxY_Xforeach (parse_stack.unit_rules, lhs_symb, rule_no) {
			  break;
			}
		      }
#endif

		    if (rule_no == 0)
		      SS_push (tbp_stack, lhs_symb);
		  }
	      }
	    
	    XxY_free (&rhsXrule);
	  }

	sxfree (symb_set);
	SS_free (tbp_stack);
    }

   if (sxndlig.for_postparsact.action_final != NULL)
       (*sxndlig.for_postparsact.action_final) (); 

    return 0;
}


static int
sxndligparsact_post_do_it ()
{
    if (parse_stack.is_ambiguous)
	/* Ici, on fait au plus simple pour "deplier", on lance "post". */
	sxndlig_post_do_it ();

    return 1;
}



bool sxndligparsact (int which, SXTABLES	*arg)
{
    int act_no;

    static int	failed_foreach [] = {1, 0, 0, 0, 0, 0};

    switch (which) {
    case SXOPEN:
	sxndlig_common.sxtables = arg;
	sxndlig_common.code = *(arg->sxligparsact);

	XxY_alloc (&sxndlig.main_trans, "sxndlig.main_trans", 500, 1, 0, 0, main_trans_oflw, stdout_or_NULL);
	sxndlig.main_trans2attr =
	    (struct main_trans2attr*) sxalloc (XxY_size (sxndlig.main_trans) + 1,
					       sizeof (struct main_trans2attr));
	sxndlig.main_trans_to_be_erased = SS_alloc (50);

#if 0
	XxY_alloc (&sxndlig.aux_trans, "sxndlig.aux_trans", 100, 1, 1, 0,
		   aux_trans_oflw, stdout_or_NULL);
	sxndlig.aux_trans2attr =
	    (struct aux_trans2attr*) sxalloc (XxY_size (sxndlig.aux_trans) + 1,
					      sizeof (struct aux_trans2attr));
#endif

	XxY_alloc (&sxndlig.object, "sxndlig.object", 100, 1, 1, 0, object_oflw, stdout_or_NULL);
	sxndlig.object2attr = (struct object2attr*) sxalloc (XxY_size (sxndlig.object) + 1,
							     sizeof (struct object2attr));
	
	XxY_alloc (&sxndlig.sigma, "sxndlig.sigma", 1000, 1, 0, 0, sigma_oflw, stdout_or_NULL);

	sxndlig.sigma_set = sxba_calloc (XxY_size (sxndlig.sigma) + 1);

	XxY_alloc (&sxndlig.dependancies, "sxndlig.dependancies", 30, 1,
		   0, 1, dependancies_oflw, stdout_or_NULL);
	sxndlig.dependancies2attr =
	    (struct dependancies2attr*) sxalloc (XxY_size (sxndlig.dependancies) +1,
						 sizeof (struct dependancies2attr));
	
	XxY_alloc (&sxndlig.piles, "sxndlig.piles", 500, 1, 0, 0, NULL, stdout_or_NULL);

	XxY_alloc (&sxndlig.paths, "sxndlig.paths", 30, 1, 1, 0, NULL, stdout_or_NULL);

	XxYxZ_alloc (&sxndlig.failed, "sxndlig.failed", 30, 1, failed_foreach, NULL, stdout_or_NULL);

	XxY_alloc (&sxndlig.recognizer, "sxndlig.recognizer", 100, 1, 1, 0, recognizer_oflw, stdout_or_NULL);
	sxndlig.recognizer2attr =
	    (struct recognizer2attr*) sxalloc (XxY_size (sxndlig.recognizer) + 1,
					       sizeof (struct recognizer2attr));

	sxndlig.symbol_set = sxba_calloc (XxY_size (parse_stack.symbols) + 1);

	parse_stack.for_parsact.action_top = sxndlig_action_top;
	/* action_pop est initialise au coup par coup ds action_top. */
	parse_stack.for_parsact.action_bot = sxndlig_action_bot;
	parse_stack.for_parsact.action_new_top = sxndlig_action_new_top;
	parse_stack.for_parsact.action_final = sxndlig_action_final;
	parse_stack.for_parsact.GC = sxndlig_GC;
	parse_stack.for_parsact.post = sxndligparsact_post_do_it;

	(*sxndlig_common.code.parsact) (which, arg);

	return SXANY_BOOL;

    case SXCLOSE:
	XxY_free (&sxndlig.main_trans);
	sxfree (sxndlig.main_trans2attr);
	SS_free (sxndlig.main_trans_to_be_erased);

#if 0
	XxY_free (&sxndlig.aux_trans);
	sxfree (sxndlig.aux_trans2attr);
#endif

	XxY_free (&sxndlig.object);
	sxfree (sxndlig.object2attr);

	XxY_free (&sxndlig.sigma);
	sxfree (sxndlig.sigma_set);

	XxY_free (&sxndlig.dependancies);
	sxfree (sxndlig.dependancies2attr);

	XxY_free (&sxndlig.piles);

	XxY_free (&sxndlig.paths);

	XxYxZ_free (&sxndlig.failed);

	if (sxndlig.GC_sigma_set != NULL) {
	    sxfree (sxndlig.GC_sigma_set);
	    sxfree (sxndlig.GC_pile_set);
	    SS_free (sxndlig.GC_to_be_processed);
	}

	sxfree (sxndlig.symbol_set);

	XxY_free (&sxndlig.recognizer);
	sxfree (sxndlig.recognizer2attr);

	(*sxndlig_common.code.parsact) (which, arg);

	return SXANY_BOOL;

    case SXINIT:
	sxplocals.mode.with_do_undo = true;
	XxY_clear (&sxndlig.main_trans);
	sxndlig.is_paths = false;
	sxndlig.last_main_symb = 0;

	(*sxndlig_common.code.parsact) (which, arg);

	return SXANY_BOOL;

    case SXFINAL:
	sxndligpost (SXFINAL, 0); /* Pour un free... */

	(*sxndlig_common.code.parsact) (which, arg);

	return SXANY_BOOL;

    case SXACTION:
	/* La memorisation se fait ds "SXPREDICATE:" */
	return SXANY_BOOL;

    case SXDO:
	return SXANY_BOOL;


    case SXUNDO:
	return SXANY_BOOL;

    case SXPREDICATE:
	act_no = (long) arg;

	if (act_no >= 10000) {
	    sxndlig.prdct_no = act_no  - 10000;

	    if (perform_prdct ()) {
		if (sxndlig.prdct_no >= 1) {
		    int post_prdct;

		    post_prdct = sxndlig.pprdct_top [1];

		    if (post_prdct >= 0)
			return (*sxndlig_common.code.parsact) (SXPREDICATE, post_prdct);
		}

		return true;
	    }

	    return false;
	}
	else
	    return (*sxndlig_common.code.parsact) (which, act_no);

    default:
	fputs ("The function \"sxndligparsact\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }

    /* NOTREACHED return SXANY_BOOL; */
}
