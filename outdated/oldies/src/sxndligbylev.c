/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1995 by Institut National de Recherche *
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
/* Cette version qualifiee de bylev est l'une des trois versions disponible.
   Les piles sont calculees et stockees apres l'analyse non deterministe complete
   d'un niveau et avant la generation de la foret. Ces piles sont utilisees pour
   filtrer la foret partagee au cours d'une passe top-down executee apres l'analyse. */
/* On associe a un symbole <rp, [i..k]> l'ensemble de ses piles. Or dans la
   foret partagee des arbres elementaires (regles) qui ont <rp, [i..k]> en LHS
   il y en a O(n) (et encore si la grammaire est en forme normale de Chomsky).
   Au cours de la phase top-down quand on va traiter la regle :
   rpijk: <rp, [i..k]>@a = <A [i..j]>&p <B [j..k]>
   l'ensemble des piles utilise pour la LHS est U(pi(rpijk))/j au lieu de
   pi(rpijk) si on avait pu etre plus fin. Cela dit, il semble que ca n'a pas d'importance
   car on applique a cet ensemble le predicat @a et l'action &p (les actions et les
   predicats s'inversent ds la passe top-down) et on ne conserve de l'ensemble
   obtenu que les elements des piles associees a <A [i..j]> qui elles sont exactes
   (ce sont les piles associees ao hook <A [i..j]> = rnil1j rnil2j ... rmis1j rmis2j ...
   ou LHS (rn) = RHS(rm) = ... = A). La transformee d'une pile pi excedentaire ne
   peut etre conservee apres filtrage. En effet soit pi' = &p (@a (pi)). Si pi'
   est une pile de <A [i..j]> elle sera conservee. Mais dans ce cas, au cours de la phase
   bottom-up la pile pi' transformee par @a (&p (pi')) aurait donne pi ce qui contredit
   le fait que pi n'est pas dans les piles de rpijk. */
/* Attention a la coexistance des modes PARSER/RECOGNIZER :
      - doit permettre en RECOGNIZER l'acces aux piles calculees ds les 2 modes
      - la fin du mode RECOGNIZER est signalee par un GC des parsers crees
      - on doit pouvoir gerer un futur agenda */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030507 09:09 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 22-02-95 10:40 (pb):		Ajout de "unfold"			*/
/************************************************************************/
/* 27-01-95 13:22 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#define WHAT	"@(#)sxndligbylev.c\t- SYNTAX [unix] - Jeudi 20 Mars 2003"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

static char	ME [] = "sxndligbylev";

# include 	"sxnd.h"
# include 	"sxndlig_bylev.h"
#define sxinitialise(x) (x) = 0

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
    sxndlig.main_trans2attr = (struct main_trans2attr*) sxrealloc (sxndlig.main_trans2attr,
								 new_size + 1,
								 sizeof (struct main_trans2attr));

    sxndlig.main_trans_to_be_erased = (int *) sxrealloc (sxndlig.main_trans_to_be_erased,
								 new_size + 1,
								 sizeof (int));

    sxndlig.main_trans_stack = (int *) sxrealloc (sxndlig.main_trans_stack,
								 new_size + 1,
								 sizeof (int));

    sxndlig.main_trans_set = sxba_resize (sxndlig.main_trans_set, new_size + 1);
}


static  void
dependancies_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.dependancies2attr =
	(struct dependancies2attr*) sxrealloc (sxndlig.dependancies2attr,
					       new_size +1,
					       sizeof (struct dependancies2attr));
}

static  SXVOID
addresses_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.uf.new_rule = (struct rule*) sxrealloc (sxndlig.uf.new_rule,
						     new_size + 1,
						     sizeof (struct rule));
}


static  SXVOID
hooks_oflw (old_size, new_size)
    int		old_size, new_size;
{
    sxndlig.uf.hook2attr = (struct uf_hook2attr*) sxrealloc (sxndlig.uf.hook2attr,
						       XxY_size (sxndlig.uf.hooks) + 1,
						       sizeof (struct uf_hook2attr));
}


static void
erase_main_trans (main_trans)
    int main_trans;
{
    int		object;

    XxY_Xforeach (sxndlig.object, main_trans, object)
	XxY_erase (sxndlig.object, object);

    XxY_erase (sxndlig.main_trans, main_trans);
}



static int
sxndlig_GC ()
{
    /* sxndparser vient de faire un GC, on en profite... */
    /* Il est possible de l'appeler a n'importe quel moment. */
    /* On ne fait pas de GC sur les piles */
    int main_trans, son, father;

    XxY_foreach (sxndlig.main_trans, main_trans) {
	son = XxY_X (sxndlig.main_trans, main_trans);
	father = XxY_Y (sxndlig.main_trans, main_trans);

	if (XxYxZ_is_erased (parse_stack.parsers, son) ||	
	    (father > 0 && XxYxZ_is_erased (parse_stack.parsers, father))) {
	    erase_main_trans (main_trans);
	}
    }

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
		sxndlig.is_paths = SXTRUE;
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
	sxndlig.is_paths = SXFALSE;
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




static SXBOOLEAN
walk_paths (bot, level, f)
    int bot, level;
    SXBOOLEAN	(*f)();
{
    int			arc, main_trans;
    SXBOOLEAN		ret_val = SXFALSE;

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


/* ATTENTION: suivant l'ordre dans lequel les reductions sont effectue'es le resultat
   peut etre different. Ex: une reduction de symbole primaire P conduit a un predicat
   faux. Plus tard une pile de P (associee a un symbole principal ou auxiliaire) est
   modifiee. Si cette modif conduit a une reevaluation a vrai du predicat, il faut
   recommencer. */
/* On note donc les main_trans et l'action/prdct qui conduit a une evaluation faux.
   Si une valeur est ajoutee a elle-meme ou a l'un de ses symbolesu auxiliaire et
   si cette valeur verifie l'action/predicat alors on relance une evaluation par
   le parser. */

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

static int
execute_prdct (prdct_no, pile)
    int prdct_no, pile;
{
    /* On regarde si pile verifie le predicat prdct_no */
    int	*pc, *pclim, post_prdct, new_pile;

    if (pile == SET_NIL)
	return SET_NIL;

    if (prdct_no == 0)
	/* ( ) */
	return pile == 0 ? 0 : SET_NIL;

    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no];

    if (*pc == PRIMARY)
      pc += 3;

    pclim = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no + 1] - 1;
    post_prdct = *pclim;
    new_pile = pile;

    while (pc < --pclim) {
      if (new_pile == 0 || XxY_Y (sxndlig.piles, new_pile) != *pclim)
	return SET_NIL;

      new_pile = XxY_X (sxndlig.piles, new_pile);
    }

    /* On execute le post_prdct eventuel */
    if (post_prdct >= 0 && !(*sxndlig_common.code.parsact) (SXPREDICATE, post_prdct))
	/* le post prdct est faux. */
	return SET_NIL;

    return pile;
}



static void
fill_symb2attr (symbol, main_trans, pile)
  int symbol, main_trans, pile;
{
    /* pile != SET_NIL. Retourne SXTRUE si object est nouveau */
    struct main_trans2attr	*pattr;
    int				symbXpile, object;

#ifdef EBUG
    if (pile == SET_NIL)
	sxtrap (ME, "fill_symb2attr");
#endif

    pattr = sxndlig.main_trans2attr + main_trans;

    if (pattr->pile == SET_NIL)
	pattr->pile = pile;
    else if ((pattr->pile & LIG_7F) != (unsigned int)pile)
    {
	XxY_set (&sxndlig.object, main_trans, pile, &object);
	pattr->pile |= LIG_80;
    }

    if (sxplocals.mode.mode == SXPARSER)
    {
	struct symb2attr	*pattr;

	pattr = sxndlig.symb2attr + symbol;

	if (pattr->pile == SET_NIL)
	    pattr->pile = pile;
	else if ((pattr->pile & LIG_7F) != (unsigned int)pile)
	{
	    XxY_set (&sxndlig.symbXpile, symbol, pile, &symbXpile);
	    pattr->pile |= LIG_80;
	}
    }
}

static void
fill_hook2attr (hook, pile)
  int hook, pile;
{
  struct hook2attr	*pattr = sxndlig.hook2attr + hook;
  int			hookXpile;

#ifdef EBUG
  if (pile == SET_NIL)
    sxtrap (ME, "fill_hook2attr");
#endif

  if (pattr->pile == SET_NIL)
    pattr->pile = pile;
  else if ((pattr->pile & LIG_7F) != (unsigned int)pile)
  {
    XxY_set (&sxndlig.hookXpile, hook, pile, &hookXpile);
    pattr->pile |= LIG_80;
  }
}



static void
propagate (main_trans, pile)
    int main_trans, pile;
{
    /* Attention, on propage les piles "SET_NIL". */
    int				dependancy, new_pile, prev_symb, prev_main_trans, count;
    struct dependancies2attr	*pd;

    XxY_Yforeach (sxndlig.dependancies, main_trans, dependancy)
    {
	pd = sxndlig.dependancies2attr + dependancy;
	new_pile = (pile == SET_NIL || pd->act_no == 0) ? pile : execute_action (pd->act_no, pile);
	new_pile = execute_prdct (pd->prdct_no, new_pile);

	prev_main_trans = XxY_X (sxndlig.dependancies, dependancy);
	prev_symb = sxndlig.main_trans2attr [prev_main_trans].main_symbol;
	count = --sxndlig.main_trans2attr [prev_main_trans].count;

#ifdef EBUG
	if (count < 0)
	    sxtrap (ME, "eval_current_level");
#endif

	if (count == 0)
	    sxndlig.main_trans_stack [++sxndlig.mts_top] = prev_main_trans;

	if (new_pile != SET_NIL)
	    fill_symb2attr (prev_symb, prev_main_trans, new_pile);
    }
}

#if 0
static void
propagate (main_trans, pile)
    int main_trans, pile;
{
    int				dependancy, prev_trans, new_pile, prev_symb, prev_main_trans;
    struct dependancies2attr	*pd;

    XxY_Yforeach (sxndlig.dependancies, main_trans, dependancy) {
	pd = sxndlig.dependancies2attr + dependancy;
	new_pile = (pd->act_no == 0) ? pile : execute_action (pd->act_no, pile);

	if (execute_prdct (pd->prdct_no, new_pile)) {
	    prev_trans = XxY_X (sxndlig.dependancies, dependancy);

	    if (prev_trans & LIG_80)
	    {
		prev_main_trans = XxY_X (sxndlig.aux_trans, prev_trans & LIG_7F);
		prev_symb = XxY_Y (sxndlig.aux_trans, prev_trans & LIG_7F);
	    }
	    else
	    {
		prev_main_trans = prev_trans;
		prev_symb = sxndlig.main_trans2attr [prev_main_trans].main_symbol;
	    }

	    fill_symb2attr (prev_symb, prev_main_trans, new_pile);
	}
    }
}
#endif



static SXBOOLEAN
store_dependancy (prev_main_trans)
    int	prev_main_trans;
{
    struct dependancies2attr	*pd;
    int				dependancy;
    SXBOOLEAN			is_old;

#ifdef EBUG
    if (prev_main_trans == 0)
	sxtrap (ME, "store_dependancy");
#endif

    is_old = XxY_set (&sxndlig.dependancies, sxndlig.cur_trans, prev_main_trans, &dependancy);
    pd = sxndlig.dependancies2attr + dependancy;

    if (is_old)
    {
#ifdef EBUG
	if (pd->act_no != sxndlig.act_no || pd->prdct_no != sxndlig.prdct_no)
	    sxtrap (ME, "store_dependancy");
#endif
    }
    else
    {
	pd->act_no = sxndlig.act_no;
	pd->prdct_no = sxndlig.prdct_no;
	
	/* On cumule toujours dans la transition principale */
	sxndlig.main_trans2attr [sxndlig.cur_main_trans].count++;

	if (sxndlig.main_trans2attr [prev_main_trans].count == 0)
	    /* Candidat eventuel */
	    SXBA_1_bit (sxndlig.main_trans_set, prev_main_trans);
    }

    return SXTRUE;
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
    int				new_pile, *pc, main_trans, dummy;
    struct main_trans2attr	*pm;

    /* On note toutes les transitions locales a ce niveau */
    if (new_top != 0)
	XxY_set (&sxndlig.level_trans, bot, new_top, &dummy);

    if (parse_stack.ared->action >= 10000 && new_top != 0)
    {
	if (symbol >= BASIZE (sxndlig.symbol_set))
	{
	    sxndlig.symbol_set = sxba_resize (sxndlig.symbol_set, XxY_size (parse_stack.symbols) + 1);
	    sxndlig.symb2attr = (struct symb2attr*) sxrealloc (sxndlig.symb2attr,
							       XxY_size (parse_stack.symbols) + 1,
							       sizeof (struct symb2attr));
	}

	if (SXBA_bit_is_reset_set (sxndlig.symbol_set, symbol))
	    /* C'est la premiere fois qu'on trouve symbol */
	    sxndlig.symb2attr [symbol].pile = SET_NIL;

	sxndlig.is_old_main_trans = XxY_set (&sxndlig.main_trans, bot, new_top, &main_trans);
	pm = sxndlig.main_trans2attr + main_trans;
	sxndlig.cur_trans = sxndlig.cur_main_trans = main_trans;
	sxndlig.cur_symbol = symbol;

#if 0
	if (!sxndlig.is_old_main_trans) {
	    /* First main */
	    sxndlig.main_symbol = pm->main_symbol = symbol;
	    pm->count = 0;
	    /* On stocke les main_trans locales pour une utilisation ds "sxndlig_action_final ". */
	    sxndlig.main_trans_to_be_erased [++sxndlig.mttbe_top] = main_trans;
	}
	else if (sxplocals.mode.mode == SXPARSER) {
	    sxndlig.main_symbol = pm->main_symbol & LIG_7F;

	    if (sxndlig.main_symbol != symbol) {
		/* symbol est auxiliaire */
		XxY_set (&sxndlig.aux_trans,
			 main_trans,
			 symbol,
			 &sxndlig.cur_trans);
		sxndlig.cur_trans |= LIG_80;
		pm->main_symbol |= LIG_80;
	    }
	}
#endif

	sxndlig.main_symbol = symbol;

	if (!sxndlig.is_old_main_trans) {
	    /* First main */
	    pm->main_symbol = symbol;
	    pm->count = 0;
	    pm->pile = SET_NIL;
	    /* On stocke les main_trans locales pour une utilisation ds "sxndlig_action_final ". */
	    sxndlig.main_trans_to_be_erased [++sxndlig.mttbe_top] = main_trans;
	}
	else
	    {
#ifdef EBUG
		if (pm->main_symbol != symbol)
		    sxtrap (ME, "sxndlig_action_new_top");
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

	    if (execute_prdct (sxndlig.prdct_no, new_pile) != SET_NIL)
		fill_symb2attr (sxndlig.cur_symbol, sxndlig.cur_main_trans, new_pile);
	}
	else {
	    if (sxndlig.is_paths)
		/* Primaires multiples */ 
		walk_paths (sxndlig.bot,
			    sxndlig.level,
			    store_dependancy);
	    else
		/* Primaire unique */
		store_dependancy (sxndlig.prev_main_trans);
	}
    }
    
    if (sxndlig.for_postparsact.action_new_top != NULL)
	(*sxndlig.for_postparsact.action_new_top) (bot, new_top, symbol);

    return 0;
}


static void
eval_current_level ()
{
    int				main_trans, object, pile;
    struct main_trans2attr	*pattr;

    /* Il faut propager meme les piles SET_NIL, a cause de "count". */
    main_trans = 0;

    while ((main_trans = sxba_scan_reset (sxndlig.main_trans_set, main_trans)) > 0)
    {
	if (sxndlig.main_trans2attr [main_trans].count == 0)
	    sxndlig.main_trans_stack [++sxndlig.mts_top] = main_trans;
    }


    while (sxndlig.mts_top > 0)
    {
	main_trans = sxndlig.main_trans_stack [sxndlig.mts_top--];
	pattr = sxndlig.main_trans2attr + main_trans;

	if (pattr->pile == SET_NIL)
	    SS_push (sxndlig.vanished_main_trans_stack, main_trans);

	propagate (main_trans, pattr->pile);

	if (pattr->pile & LIG_80)
	{
	    XxY_Xforeach (sxndlig.object, main_trans, object)
	    {
		pile = XxY_Y (sxndlig.object, object);
		propagate (main_trans, pile);
	    }
	}
    }
}

#if 0
static void
eval_current_level ()
{
    int prev_main_trans, object, pile, dependancy, next_trans, next_main_trans, count, main_symbol,
    aux_trans, aux_symbol, prdct_no, post_prdct;
    int *pmain_pile;

    while (sxndlig.mts_top > 0)
    {
	prev_main_trans = sxndlig.main_trans_stack [sxndlig.mts_top--];
	/* C'est une transition principale */
	/* main_trans_set ne sert qu'a l'initialisation */
	SXBA_0_bit (sxndlig.main_trans_set, prev_main_trans);

	XxY_Xforeach (sxndlig.object, prev_main_trans, object)
	{
	    pile = XxY_Y (sxndlig.object, object);
	    propagate (prev_main_trans, pile);
	}

	XxY_Yforeach (sxndlig.dependancies, prev_main_trans, dependancy)
	{
	    next_trans = XxY_X (sxndlig.dependancies, dependancy);
	    next_main_trans =
		(next_trans & LIG_80) ? XxY_X (sxndlig.aux_trans, next_trans & LIG_7F) : next_trans;

	    count = --sxndlig.main_trans2attr [next_main_trans].count;

#ifdef EBUG
	    if (count < 0)
		sxtrap (ME, "eval_current_level");
#endif

	    if (count == 0)
	    {
		sxndlig.main_trans_stack [++sxndlig.mts_top] = next_main_trans;

		/* next_main_trans est completement evalue, on regarde s'il existe au moins un composant
		   qui est evalue a vide. */
		/* On en profite pour appeler le post_prdct. */
		main_symbol = sxndlig.main_trans2attr [next_main_trans].main_symbol;
		pmain_pile = &(sxndlig.symb2attr [main_symbol /* & LIG_7F */].pile);

		if (*pmain_pile == SET_NIL)
		    SS_push (sxndlig.vanished_main_trans_stack, next_main_trans);
		else if (main_symbol & LIG_80)
		{
		    prdct_no = sxndlig.dependancies2attr [dependancy].prdct_no;

		    if (prdct_no >= 1)
		    {
			post_prdct = sxndlig_common.code.prdct_or_act_code
			    [sxndlig_common.code.prdct_or_act_disp [prdct_no + 1] - 1];

			if (post_prdct >= 0 && !(*sxndlig_common.code.parsact) (SXPREDICATE, post_prdct))
			{
			    /* le post prdct est faux, on simule l'echec par un echec du calcul
			       sur les piles. */
			    *pmain_pile = SET_NIL;
			}
		    }

		    if (*pmain_pile != SET_NIL)
		    {
			XxY_Xforeach (sxndlig.aux_trans, next_main_trans, aux_trans)
			{
			    aux_symbol = XxY_Y (sxndlig.aux_trans, aux_trans);

			    if (sxndlig.symb2attr [aux_symbol].pile == SET_NIL)
			    {
				SS_push (sxndlig.vanished_main_trans_stack, next_main_trans);
				break;
			    }
			}
		    }
		}
	    }
	}
    }
}
#endif

static void
vanish_trans (son, father)
    int son, father;
{
    /* On fait disparaitre la transition entre les parser son et father. */
    struct  parsers_attr	*pattr;
    int				n, local_link, new_main_son;

    if (father > 0)
    {
	/* On est dans la parse_stack */
	pattr = &(parse_stack.parser_to_attr [father]);

	if (pattr->son == son || pattr->son == -son)
	{
	    /* C'est la liaison vers le fils principal qui disparait */
	    if (pattr->son < 0)
	    {
		/* Il y a des fils secondaires */
		/* On en choisit un qui va devenir principal. */
		n = 0;

		XxY_Xforeach (parse_stack.sons, father, local_link)
		{
		    if (n == 0)
		    {
			new_main_son = XxY_Y (parse_stack.sons, local_link);
			/* new_main_symbol = parse_stack.transitions [local_link]; */
			XxY_erase (parse_stack.sons, local_link);
		    }

		    if (++n > 1)
			break;
		}

		pattr->son = n == 1 ? new_main_son : -new_main_son;
		/* pattr->symbol = new_main_symbol; */
	    }
	    else
	    {
		/* Il n'y a plus de fils, father doit disparaitre */
		if (SXBA_bit_is_reset_set (sxndlig.vanished_parser_set, father))
		    SS_push (sxndlig.vanished_parser_stack, father);
	    }
	}
	else
	{
	    /* Il y a des fils secondaires */
	    /* son est obligatoirement l'un de ceux-la */
	    local_link = XxY_is_set (&parse_stack.sons, father, son);

#ifdef EBUG
	    if (pattr->son > 0 || local_link == 0)
		sxtrap (ME, "vanish_trans");
#endif

		XxY_erase (parse_stack.sons, local_link);

	    XxY_Xforeach (parse_stack.sons, father, local_link)
		break;

	    if (local_link == 0)
		/* plus de fils secondaire */
		pattr->son = -pattr->son;
	}
    }
    else
    {
	/* father est un dummy parser, c'est un index ds triples */
	parse_stack.for_reducer.triples [-father].is_valid = SXFALSE;
    }
}


#if 0
static void
change_trans_name (son, father, old_main_symbol, new_main_symbol)
    int	son, father, old_main_symbol, new_main_symbol;
{
    /* On change le nom de la transition entre son et father */
    int				local_link;
    struct  parsers_attr	*pattr;
    int				*ps;

    if (father < 0)
	/* C'est un xtriple */
	ps = &(parse_stack.for_reducer.triples [-father].symbol);
    else
    {
	pattr = &(parse_stack.parser_to_attr [father]);

	if (pattr->son == son || pattr->son == -son)
	    /* Son est le fils principal */
	    ps = &(pattr->symbol);
	else
	{
	    /* Il y a des fils secondaires */
	    /* son est obligatoirement l'un de ceux-la */
	    local_link = XxY_is_set (&parse_stack.sons, father, son);

#ifdef EBUG
	    if (pattr->son > 0 || local_link == 0)
		sxtrap (ME, "change_trans_name");
#endif

	    ps = &(parse_stack.transitions [local_link]);
	}
    }

#ifdef EBUG
    if (*ps != old_main_symbol)
	sxtrap (ME, "change_trans_name");
#endif

    *ps = new_main_symbol;
}
#endif


#if 0
static void
finalize_current_level ()
{
    /* On modifie la parse_stack et les unit_rules en prenant en compte les transitions
       qui ont disparues. */
    int main_trans, object, main_symbol, aux_trans, new_main_symbol, aux_symbol, rule_no,
        son, father, parser, level_trans;
    int *pp, *pnfs;

    if (XxYxZ_top (parse_stack.parsers) >= BASIZE (sxndlig.vanished_parser_set))
	sxndlig.vanished_parser_set = sxba_resize (sxndlig.vanished_parser_set,
						   XxYxZ_size (parse_stack.parsers) + 1);

    do {
	main_trans = SS_pop (sxndlig.vanished_main_trans_stack);

	XxY_Xforeach (sxndlig.object, main_trans, object)
	    break;

	if (object == 0)
	{
	    /* Toute la transition disparait */
	    vanish_trans (XxY_X (sxndlig.main_trans, main_trans),
			  XxY_Y (sxndlig.main_trans, main_trans));
	}
	else
	{
	    /* Il va en rester. */
	    main_symbol = sxndlig.main_trans2attr [main_trans].main_symbol & LIG_7F;

	    if (sxndlig.symb2attr [main_symbol].pile == SET_NIL)
	    {
		/* damned le symbole principal disparait */
		/* On cherche un remplacant */
		XxY_Xforeach (sxndlig.aux_trans, main_trans, aux_trans)
		{
		    aux_symbol = XxY_Y (sxndlig.aux_trans, aux_trans);

		    if (sxndlig.symb2attr [aux_symbol].pile != SET_NIL)
			/* On prend le premier */
			break;
		}

		new_main_symbol = aux_symbol;

#if 0
		/* On refabrique les unit_rules */
		XxY_Xforeach (sxndlig.aux_trans, main_trans, aux_trans)
		{
		    aux_symbol = XxY_Y (sxndlig.aux_trans, aux_trans);

		    /* Le symbol auxiliaire disparait */
		    rule_no = XxY_is_set (&parse_stack.unit_rules, main_symbol, aux_symbol);

#ifdef EBUG
		    if (rule_no == 0)
			sxtrap (ME, "finalize_current_level");
#endif

		    XxY_erase (parse_stack.unit_rules, rule_no);

		    if (sxndlig.symb2attr [aux_symbol].pile != SET_NIL && aux_symbol != new_main_symbol)
			/* On refabrique une unit_rule */
			XxY_set (&parse_stack.unit_rules, new_main_symbol, aux_symbol, &rule_no);
		}

		/* On change le "nom" de la transition principale */
		change_trans_name (XxY_X (sxndlig.main_trans, main_trans),
				   XxY_Y (sxndlig.main_trans, main_trans),
				   main_symbol,
				   new_main_symbol);
#endif		
	    }
	    else
	    {
#if 0
		/* Le symbole principal est conserve, on se contente de supprimer les unit
		   rules dont la partie droite disparait */
		XxY_Xforeach (sxndlig.aux_trans, main_trans, aux_trans)
		{
		    aux_symbol = XxY_Y (sxndlig.aux_trans, aux_trans);

		    if (sxndlig.symb2attr [aux_symbol].pile == SET_NIL)
		    {
			/* Le symbol auxiliaire disparait */
			rule_no = XxY_is_set (&parse_stack.unit_rules, main_symbol, aux_symbol);

#ifdef EBUG
			if (rule_no == 0)
			    sxtrap (ME, "finalize_current_level");
#endif

			XxY_erase (parse_stack.unit_rules, rule_no);
		    }
		}
#endif
	    }
	}
    } while (!SS_is_empty (sxndlig.vanished_main_trans_stack));

    if (!SS_is_empty (sxndlig.vanished_parser_stack))
    {
	/* Des parser ont disparu, on regarde les consequences... */
	do
	{
	    son = SS_pop (sxndlig.vanished_parser_stack);

	    XxY_Xforeach (sxndlig.level_trans, son, level_trans)
	    {
		father = XxY_Y (sxndlig.main_trans, level_trans);
		vanish_trans (son, father);
	    }
	} while (!SS_is_empty (sxndlig.vanished_parser_stack));

	/* On s'occupe maintenant des "for_scanner" */
	pp = parse_stack.for_scanner.next_hd;

	while ((parser = *pp) > 0)
	{
	    pnfs = &(parse_stack.parser_to_attr [father].next_for_scanner);

	    if (SXBA_bit_is_set (sxndlig.vanished_parser_set, parser))
		*pp = *pnfs;
	    else
		pp = pnfs;
	}

	sxba_empty (sxndlig.vanished_parser_set);
    }
}
#endif


static void
finalize_current_level ()
{
    /* On modifie la parse_stack en prenant en compte les transitions
       qui ont disparues. */
    int main_trans, son, father, parser, level_trans, x;
    int *pp, *pnfs;

    if (XxYxZ_top (parse_stack.parsers) >= BASIZE (sxndlig.vanished_parser_set))
	sxndlig.vanished_parser_set = sxba_resize (sxndlig.vanished_parser_set,
						   XxYxZ_size (parse_stack.parsers) + 1);

    do {
	main_trans = SS_pop (sxndlig.vanished_main_trans_stack);

	vanish_trans (XxY_X (sxndlig.main_trans, main_trans),
			  XxY_Y (sxndlig.main_trans, main_trans));
    } while (!SS_is_empty (sxndlig.vanished_main_trans_stack));

    if (!SS_is_empty (sxndlig.vanished_parser_stack))
    {
	/* Des parser ont disparu, on regarde les consequences... */
	do
	{
	    son = SS_pop (sxndlig.vanished_parser_stack);

	    XxY_Xforeach (sxndlig.level_trans, son, level_trans)
	    {
		father = XxY_Y (sxndlig.main_trans, level_trans);
		vanish_trans (son, father);
	    }
	} while (!SS_is_empty (sxndlig.vanished_parser_stack));

	/* On s'occupe des "xtriple" pas is_shift_reduce */
	for (x = 1; x <= parse_stack.for_reducer.top; x++)
	{
	    parser = parse_stack.for_reducer.triples [x].parser;

	    if (SXBA_bit_is_set (sxndlig.vanished_parser_set, parser))
		parse_stack.for_reducer.triples [x].is_valid = SXFALSE;
	}

	/* On s'occupe maintenant des "for_scanner" */
	pp = parse_stack.for_scanner.next_hd;

	while ((parser = *pp) > 0)
	{
	    pnfs = &(parse_stack.parser_to_attr [father].next_for_scanner);

	    if (SXBA_bit_is_set (sxndlig.vanished_parser_set, parser))
		*pp = *pnfs;
	    else
		pp = pnfs;
	}

	sxba_empty (sxndlig.vanished_parser_set);
    }
}




static int
sxndlig_action_final ()
{
    int main_trans;

    eval_current_level ();

    if (!SS_is_empty (sxndlig.vanished_main_trans_stack))
	finalize_current_level ();

    /* On a termine' l'e'valuation d'un niveau, On prepare le niveau suivant. */
    XxY_clear (&sxndlig.dependancies);
    /* XxY_clear (&sxndlig.aux_trans); */
    XxY_clear (&sxndlig.level_trans);

    while (sxndlig.mttbe_top > 0)
    {
	main_trans = sxndlig.main_trans_to_be_erased [sxndlig.mttbe_top--];

	if (sxndlig.main_trans2attr [main_trans].count != 0)
	{
	    /* Normalement, il ne reste des transitions non evaluees que dans le cas de
	       grammaires cycliques !!! */
	    sxtrap (ME, "Processing of cyclic grammars is not implemented.");
	}

	if (XxY_Y (sxndlig.main_trans, main_trans) < 0)
	    /* On supprime les main_trans dont le 2eme composant est negatif (OBLIGATOIRE!). */
	    erase_main_trans (main_trans);
    }

    if (sxndlig.for_postparsact.action_final != NULL)
	(*sxndlig.for_postparsact.action_final) (); 

    return 0;
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


static int
td_action (act_no, pile)
    int	act_no, pile;
{
  /* On est en passe top-down
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

static SXBOOLEAN
td_check_stack (rule_no, pile)
  int rule_no, pile;
{
  /* verifie si pile est un membre de l'ensemble des piles de rule_no.
     Si oui marque la pile correspondante. */
  int	old_pile, symbol, symbXpile;
  
#ifdef EBUG
  if (rule_no >= parse_stack.hook_rule)
    sxtrap (ME, "td_check_stack");
#endif

  symbol = sxndlig.rule2attr [rule_no].symbol;

  if ((old_pile = sxndlig.symb2attr [symbol].pile) == SET_NIL)
    return SXFALSE;

  if ((old_pile & LIG_7F) == (unsigned int)pile)
  {
    SXBA_1_bit (sxndlig.symbXpile_set, 0);
    return SXTRUE;
  }

  if ((old_pile & LIG_80) == 0)
    return SXFALSE;

  if ((symbXpile = XxY_is_set (&sxndlig.symbXpile, symbol, pile)) > 0)
  {
    /* C'est un membre */
    /* On le marque */
    SXBA_1_bit (sxndlig.symbXpile_set, symbXpile);
  }

  return symbXpile > 0;
}

static void
td_filter (rule_no)
  int rule_no;
{
  /* Supprime les piles de rule_no qui ne sont pas marquees */
  struct symb2attr	*pattr;
  int			symbol, symbXpile, old_pile, n, last_symbXpile;
  
#ifdef EBUG
  if (rule_no >= parse_stack.hook_rule)
    sxtrap (ME, "td_filter");
#endif

  symbol = sxndlig.rule2attr [rule_no].symbol;
  pattr = sxndlig.symb2attr + symbol;

  old_pile = pattr->pile;

  if (!SXBA_bit_is_set_reset (sxndlig.symbXpile_set, 0))
    pattr->pile = SET_NIL;

  if ((old_pile & LIG_80) != 0)
  {
    n = last_symbXpile = 0;

    XxY_Xforeach (sxndlig.symbXpile, symbol, symbXpile)
      {
	if (SXBA_bit_is_set_reset (sxndlig.symbXpile_set, symbXpile))
	{
	  n++;
	  last_symbXpile = symbXpile;
	}
	else
	  XxY_erase (sxndlig.symbXpile, symbXpile);
      }

    if (pattr->pile == SET_NIL && last_symbXpile != 0)
    {
      pattr->pile = XxY_Y (sxndlig.symbXpile, last_symbXpile);
      XxY_erase (sxndlig.symbXpile, last_symbXpile);
      n--;
    }

    if (n > 0)
      sxndlig.symb2attr [symbol].pile |= LIG_80;
  }
}



static void
compute_td_stacks (rhs_rule_no)
  int rhs_rule_no;
{
  /* Cette procedure a ete "declanchee" par la presence de la "derniere" occurrence de
     "rhs_rule_no" en RHS dans "grammar". On calcule les piles associees a toutes les occurrences
     de "rhs_rule_no" en RHS. */
  int			item, next_item, lhs_rule_no, pile, prdct_no, act_no;
  int			symbXpile, hookXpile, new_pile;
  SXBOOLEAN		checked;
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
	  checked = SXFALSE;

	  if ((pile = sxndlig.hook2attr [lhs_rule_no].pile) != SET_NIL)
	  {
	    if (td_check_stack (rhs_rule_no, pile & LIG_7F))
	      checked = SXTRUE;

	    if (pile & LIG_80)
	    {
	      XxY_Xforeach (sxndlig.hookXpile, lhs_rule_no, hookXpile)
		{
		  pile = XxY_Y (sxndlig.hookXpile, hookXpile);

		  if (td_check_stack (rhs_rule_no, pile))
		    checked = SXTRUE;
		}
	    }
	  }

	  if (!checked)
	  {
	    /* On supprime rhs_rule_no de la RHS du hook. */
	    sxndparse_erase_hook_item (item);
	  }
	}
      }
      else
      {
	/* La LHS n'est pas un hook. */
	prdct_no = sxndlig.grammar2attr [item].prdct_no;

	if (rhs_rule_no >= parse_stack.hook_rule)
	{
	  /* La RHS est un hook, on effectue le calcul des piles */
	  if (prdct_no >= 10000)
	  {
	    prdct_no -= 10000;
	    pc = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no];

	    if (*pc == SECONDARY)
	    {
	      pile = init_pile (prdct_no);
	      fill_hook2attr (rhs_rule_no, pile);
	    }
	    else
	    {
	      act_no = plhs_attr->act_no - 10000; /* PRIMARY */

	      if ((pile = sxndlig.symb2attr [plhs_attr->symbol].pile) != SET_NIL)
	      {
		if ((new_pile = td_prdct (act_no, pile & LIG_7F)) != SET_NIL)
		  {
		    if ((new_pile = td_action (prdct_no, new_pile)) != SET_NIL)
		    {
		      fill_hook2attr (rhs_rule_no, new_pile);
		    }
		  }

		if (pile & LIG_80)
		{
		  XxY_Xforeach (sxndlig.symbXpile, plhs_attr->symbol, symbXpile)
		    {
		      pile = XxY_Y (sxndlig.symbXpile, symbXpile);

		      if ((new_pile = td_prdct (act_no, pile & LIG_7F)) != SET_NIL)
		      {
			if ((new_pile = td_action (prdct_no, new_pile)) != SET_NIL)
			{
			  fill_hook2attr (rhs_rule_no, new_pile);
			}
		      }
		    }
		}
	      }
	    }
	  }
	  /* else Pas de pile */
	}
	else
	{
	  /* La rhs est une regle, on filtre. */
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
	      if ((pile = sxndlig.symb2attr [plhs_attr->symbol].pile) != SET_NIL)
	      {
		checked = SXFALSE;
		act_no = plhs_attr->act_no - 10000; /* PRIMARY */

		if ((new_pile = td_prdct (act_no, pile & LIG_7F)) != SET_NIL)
		  {
		    if ((new_pile = td_action (prdct_no, new_pile)) != SET_NIL)
		    {
		      if (td_check_stack (rhs_rule_no, new_pile))
		      {
			/* Cette pile a deja ete calculee en bottom-up pour rhs_rule_no */
			checked = SXTRUE;
		      }
		    }
		  }

		if (pile & LIG_80)
		{
		  XxY_Xforeach (sxndlig.symbXpile, plhs_attr->symbol, symbXpile)
		    {
		      pile = XxY_Y (sxndlig.symbXpile, symbXpile);

		      if ((new_pile = td_prdct (act_no, pile & LIG_7F)) != SET_NIL)
		      {
			if ((new_pile = td_action (prdct_no, new_pile)) != SET_NIL)
			{
			  if (td_check_stack (rhs_rule_no, new_pile))
			  {
			    /* Cette pile a deja ete calculee en bottom-up pour rhs_rule_no */
			    checked = SXTRUE;
			  }
			}
		      }
		    }
		}
		
		if (!checked) {
		  sxndlig.symb2attr [plhs_attr->symbol].pile = SET_NIL;
		  CTRL_PUSH (parse_stack.erased, lhs_rule_no);
	        }
	      }
	    }
	  }
	  /* else Pas de pile */
	}
      }
    }
  }

  /* Si il n'y a pas de pile associee aux occurrences de rhs_rule_no en rhs
     la regle rhs_rule_no est supprimee ssi toutes les regles ou rhs_rule_no
     apparait en rhs sont supprimees */

  /* On supprime les piles non validees */
  if (rhs_rule_no < parse_stack.hook_rule)
    td_filter (rhs_rule_no);

  /* Dans tous les cas on "valide" rhs_rule_no, meme en cas d'echec total */
  if (SXBA_bit_is_reset_set (sxndlig.already_computed, rhs_rule_no))
    sxndlig.to_be_processed [++sxndlig.tbp_top] = rhs_rule_no;
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

static SXBOOLEAN
unfold_check_stack (rule_no, pile)
  int rule_no, pile;
{
    /* verifie si pile est un membre de l'ensemble des piles de rule_no. */
    int pile2, symbol;
  
    if (rule_no >= parse_stack.hook_rule)
	pile2 = sxndlig.hook2attr [rule_no].pile;
    else
    {
	symbol = sxndlig.rule2attr [rule_no].symbol;
	pile2 = sxndlig.symb2attr [symbol].pile;
    }

    if (pile2 == SET_NIL) return SXFALSE; /* Pas de pile */

    if (pile == (pile2 & LIG_7F)) return SXTRUE; /* C'est la pile principale */

    if ((pile2 & LIG_80) == 0) return SXFALSE; /* pas de pile secondaire */


    return XxY_is_set ((rule_no >= parse_stack.hook_rule) ? &sxndlig.hookXpile : &sxndlig.symbXpile,
		       rule_no,
		       pile) > 0; /* C'est une pile secondaire */
}




static void
unfold_hook (rule_no, pile)
    int rule_no, pile;
{
    /* Si pile == -1, on "recopie" le hook sinon on extrait de sa RHS les adresses qui ont
     "pile" comme pile associee. */
    int rule_lgth, item, lhs_symb, address, new_symbol, lgth, lim, rhs_rule_no, rhs_address,
        grammar_top, hook, *p;

    sxinitialise(rhs_address); /* pour faire taire "gcc -Wuninitialized" */
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
    int		item, lhs_rule_no, pile, hookXpile, symbol, symbXpile;

    XxY_alloc (&sxndlig.uf.addresses, "sxndlig.uf.addresses", parse_stack.G.P * 2, 1, 0, 0, addresses_oflw, stdout_or_NULL);

    
    sxndlig.uf.new_rule = (struct rule*) sxalloc (XxY_size (sxndlig.uf.addresses) + 1,
						      sizeof (struct rule));

   
    XxY_alloc (&sxndlig.uf.new_symbols, "sxndlig.uf.new_symbols", parse_stack.G.N * 2, 1, 0, 0, NULL, stdout_or_NULL);
    sxndlig.uf.is_new_symbols = SXTRUE;

    XxY_alloc (&sxndlig.uf.hooks, "sxndlig.uf.hooks", (parse_stack.rule_top - parse_stack.hook_rule + 2) * 2,
	       1, 0, 0, hooks_oflw, stdout_or_NULL);

    sxndlig.uf.hook2attr = (struct uf_hook2attr*) sxalloc (XxY_size (sxndlig.uf.hooks) + 1,
							sizeof (struct uf_hook2attr));

    sxndlig.uf.new_grammar = (struct grammar*)
	sxalloc (sxndlig.uf.new_grammar_size = 2 * parse_stack.G.G,
		 sizeof (struct grammar));
    sxndlig.uf.new_grammar_top = 1;

    parse_stack.G.G = parse_stack.G.N = parse_stack.G.P = 0;

    lhs_rule_no = 0;

    while ((lhs_rule_no = sxba_scan (CTRL2SXBA (parse_stack.reached), lhs_rule_no)) > 0 &&
	   lhs_rule_no < parse_stack.hook_rule)
    {
	if (sxndlig.rule2attr [lhs_rule_no].act_no < 10000)
	{
	    unfold_rule (lhs_rule_no, -1);
	}
	else
	{
	    symbol = sxndlig.rule2attr [lhs_rule_no].symbol;
	    pile = sxndlig.symb2attr [symbol].pile;

	    unfold_rule (lhs_rule_no, pile & LIG_7F);

	    if (pile & LIG_80)
	    {
		XxY_Xforeach (sxndlig.symbXpile, lhs_rule_no, symbXpile)
		    unfold_rule (lhs_rule_no, XxY_Y (sxndlig.symbXpile, symbXpile));
	    }
	}
    }

    sxndlig.uf.hook_rule = XxY_top (sxndlig.uf.addresses) + 1;

    /* On parcourt les hook */
    lhs_rule_no = parse_stack.hook_rule - 1;

    while ((lhs_rule_no = sxba_scan (CTRL2SXBA (parse_stack.reached), lhs_rule_no)) > 0)
    {
	if ((pile = sxndlig.hook2attr [lhs_rule_no].pile) == SET_NIL)
	{
	    /* hook sans pile */
	    unfold_hook (lhs_rule_no, -1);
	}
	else
	{
	    unfold_hook (lhs_rule_no, pile & LIG_7F);

	    if (pile & LIG_80)
	    {
		XxY_Xforeach (sxndlig.hookXpile, lhs_rule_no, hookXpile)
		    unfold_hook (lhs_rule_no, XxY_Y (sxndlig.hookXpile, hookXpile));
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

static int
bylev_init ()
{
    /* On construit une representation de la foret partagee sur laquelle on va evaluer les piles. */
    /* symbol si -> (#pred, pi_set)
       A chaque symbole <red i, [p..q]> on associe :
       - #pred nb de predecesseurs de si non encore calcules
       - pi_set ensemble des piles (initialise a SET_NIL)
       */
    int	rhs_rule_no, lim, item, rule_lgth, prdct_no, indice, reduce, lhs_symb, lhs_rule_no, ret_val;
    int *pprdct_no;

    sxndlig.symbXpile_set = sxba_calloc (XxY_top (sxndlig.symbXpile) + 1);

    sxndlig.hook2attr = (struct hook2attr*) sxalloc (parse_stack.rule_top + 1, sizeof (struct hook2attr));
    sxndlig.to_be_processed = (int *) sxalloc (parse_stack.rule_top + 1, sizeof (int));
    sxndlig.already_computed = sxba_calloc (parse_stack.rule_top + 1);

    sxndlig.rule2attr = (struct rule2attr *) sxcalloc ((SXUINT)parse_stack.rule_top + 1, sizeof (struct rule2attr));
    sxndlig.grammar2attr = (struct grammar2attr *) sxalloc (parse_stack.grammar_top + 1,
							    sizeof (struct grammar2attr));

    XxY_alloc (&sxndlig.hookXpile, "sxndlig.hookXpile", parse_stack.grammar_top /* why not */, 1, 1, 0, NULL, stdout_or_NULL);


    for (lhs_rule_no = 1; lhs_rule_no < parse_stack.hook_rule; lhs_rule_no++)
    {
	if ((rule_lgth = parse_stack.rule [lhs_rule_no].lgth) > 0 && lhs_rule_no != parse_stack.root)
	{
	    item = parse_stack.rule [lhs_rule_no].hd;
	    sxndlig.rule2attr [lhs_rule_no].symbol = lhs_symb = parse_stack.grammar [item].rhs_symb;
	    reduce = XxY_X (parse_stack.symbols, lhs_symb);
#if 0
/* le 14/4/95 le 1er champ de symbols est un no de regle (et pas une ref) */
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
		    sxndlig.rule2attr [rhs_rule_no].count++;
		    /* On recupere le predicat associe */
		    prdct_no = pprdct_no [indice];
		    sxndlig.grammar2attr [item].prdct_no = prdct_no;
		}
	    }
	}
    }

    /* On parcourt les hook */
    for (lhs_rule_no = parse_stack.hook_rule; lhs_rule_no <= parse_stack.rule_top; lhs_rule_no++)
    {
	if ((rule_lgth = parse_stack.rule [lhs_rule_no].lgth) > 0)
	{
	    sxndlig.hook2attr [lhs_rule_no].pile = SET_NIL;
	    item = parse_stack.rule [lhs_rule_no].hd;

	    lim = item + rule_lgth;

	    while (++item < lim)
	    {
		rhs_rule_no = parse_stack.grammar [item].rhs_symb;
		sxndlig.rule2attr [rhs_rule_no].count++;
	    }
	}
    }

    /* On effectue la passe top_down */
    SXBA_1_bit (sxndlig.already_computed, parse_stack.start_symbol);
    sxndlig.to_be_processed [sxndlig.tbp_top = 1] = parse_stack.start_symbol;

    do
    {
	lhs_rule_no = sxndlig.to_be_processed [sxndlig.tbp_top--];
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
    } while (sxndlig.tbp_top > 0);
  
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

    sxfree (sxndlig.symbXpile_set);
    XxY_free (&sxndlig.hookXpile);
    sxfree (sxndlig.hook2attr);

    sxfree (sxndlig.to_be_processed);
    sxfree (sxndlig.already_computed);
    sxfree (sxndlig.rule2attr);
    sxfree (sxndlig.grammar2attr);

    return ret_val;
}


int sxndligbylev (which, arg)
    int		which;
    struct sxtables	*arg;
{
    int act_no;

    switch (which) {
    case SXOPEN:
	sxndlig_common.sxtables = arg;
	sxndlig_common.code = *(arg->sxligparsact);

	XxY_alloc (&sxndlig.main_trans, "sxndlig.main_trans", 500, 1, 0, 0, main_trans_oflw, stdout_or_NULL); 
	sxndlig.main_trans2attr =
	    (struct main_trans2attr*) sxalloc (XxY_size (sxndlig.main_trans) + 1,
					       sizeof (struct main_trans2attr));
	sxndlig.main_trans_to_be_erased = (int *) sxalloc (XxY_size (sxndlig.main_trans) + 1,
								 sizeof (int));
	sxndlig.main_trans_stack = (int *) sxalloc (XxY_size (sxndlig.main_trans) + 1,
						    sizeof (int));
	sxndlig.main_trans_set = sxba_calloc (XxY_size (sxndlig.main_trans) + 1);

#if 0
	XxY_alloc (&sxndlig.aux_trans, "sxndlig.aux_trans", 100, 1, 1, 0, NULL, stdout_or_NULL);
#endif

	XxY_alloc (&sxndlig.object, "sxndlig.object", 1000, 1, 1, 0, NULL, stdout_or_NULL);

	XxY_alloc (&sxndlig.dependancies, "sxndlig.dependancies", 100, 1,
		   0, 1, dependancies_oflw, stdout_or_NULL);
	sxndlig.dependancies2attr =
	    (struct dependancies2attr*) sxalloc (XxY_size (sxndlig.dependancies) +1,
						 sizeof (struct dependancies2attr));
	
	XxY_alloc (&sxndlig.piles, "sxndlig.piles", 500, 1, 0, 0, NULL, stdout_or_NULL);

	XxY_alloc (&sxndlig.paths, "sxndlig.paths", 30, 1, 1, 0, NULL, stdout_or_NULL);

	sxndlig.symbol_set = sxba_calloc (XxY_size (parse_stack.symbols) + 1);
	sxndlig.symb2attr = (struct symb2attr*) sxalloc (XxY_size (parse_stack.symbols) + 1,
							     sizeof (struct symb2attr));

	XxY_alloc (&sxndlig.symbXpile, "sxndlig.symbXpile", 1000, 1, 1, 0, NULL, stdout_or_NULL);

	XxY_alloc (&sxndlig.level_trans, "sxndlig.level_trans", 50, 1, 1, 0, NULL, stdout_or_NULL);

	sxndlig.vanished_main_trans_stack = SS_alloc (20);
	sxndlig.vanished_parser_set = sxba_calloc (XxYxZ_size (parse_stack.parsers) + 1);
	sxndlig.vanished_parser_stack = SS_alloc (20);

	parse_stack.for_parsact.action_top = sxndlig_action_top;
	/* action_pop est initialise au coup par coup ds action_top. */
	parse_stack.for_parsact.action_bot = sxndlig_action_bot;
	parse_stack.for_parsact.action_new_top = sxndlig_action_new_top;
	parse_stack.for_parsact.action_final = sxndlig_action_final;
	parse_stack.for_parsact.GC = sxndlig_GC;
	parse_stack.for_parsact.post = bylev_init;

	(*sxndlig_common.code.parsact) (which, arg);

	return 0;

    case SXCLOSE:
	XxY_free (&sxndlig.main_trans);
	sxfree (sxndlig.main_trans2attr);
	sxfree (sxndlig.main_trans_to_be_erased);
	sxfree (sxndlig.main_trans_stack);
	sxfree (sxndlig.main_trans_set);

	/* XxY_free (&sxndlig.aux_trans); */

	XxY_free (&sxndlig.object);

	XxY_free (&sxndlig.dependancies);
	sxfree (sxndlig.dependancies2attr);

	XxY_free (&sxndlig.piles);

	XxY_free (&sxndlig.paths);

	sxfree (sxndlig.symbol_set);
	sxfree (sxndlig.symb2attr);

	XxY_free (&sxndlig.symbXpile);

	XxY_free (&sxndlig.level_trans);
	SS_free (sxndlig.vanished_main_trans_stack);
	sxfree (sxndlig.vanished_parser_set);
	SS_free (sxndlig.vanished_parser_stack);

	(*sxndlig_common.code.parsact) (which, arg);

	return 0;

    case SXINIT:
	sxplocals.mode.with_do_undo = SXTRUE;
	XxY_clear (&sxndlig.main_trans);
	sxndlig.is_paths = SXFALSE;
	sxndlig.mttbe_top = sxndlig.mts_top = 0;

	(*sxndlig_common.code.parsact) (which, arg);

	return 0;

    case SXFINAL:
	if (sxndlig.uf.is_new_symbols)
	    XxY_free (&sxndlig.uf.new_symbols);

	(*sxndlig_common.code.parsact) (which, arg);

	return 0;

    case SXACTION:
	/* La memorisation se fait ds "SXPREDICATE:" */
	return 0;

    case SXDO:
	return 0;


    case SXUNDO:
	return 0;

    case SXPREDICATE:
	act_no = (long) arg;

	if (act_no >= 10000) {
	    sxndlig.prdct_no = act_no  - 10000;
	    return 1;
	}

	/* Devrait sans doute etre execute' "par niveau" avec les predicats des LIG !!! */
	return (*sxndlig_common.code.parsact) (which, act_no);

    default:
	fputs ("The function \"sxndligbylev\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }

    /* NOTREACHED return 0; */
}
