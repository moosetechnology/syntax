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
 

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 31-05-95 10:50 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)ARN_parsact.c	- SYNTAX [unix] - Mer 31 Mai 1995 10:50:06";

static char	ME [] = "ARN_parsact";

# include 	"sxnd.h"
# include 	"ARN_parsact.h"

static struct ARN	ARN;

static  int
ARN_symbols_oflw (SXINT old_size, SXINT new_size)
{
    (void) old_size;
    (void) new_size;
#if 0
    ARN.symbol_set = sxba_resize (ARN.symbol_set, new_size + 1);
    ARN.symb2attr = (struct symb2attr*) sxrealloc (ARN.symb2attr,
						   new_size +1,
						   sizeof (struct symb2attr));
#endif
    return 0;
}

static  int
ARN_parsers_oflw (SXINT old_size, SXINT new_size)
{
    (void) old_size;
    (void) new_size;
    ARN.vanished_parser_set = sxba_resize (ARN.vanished_parser_set, new_size + 1);

#if 0
    ARN.parser2attr = (struct parser2attr*) sxrealloc (ARN.parser2attr,
						       new_size +1,
						       sizeof (struct parser2attr));
#endif

    return 0;
}

#if 0
static  int
ARN_sons_oflw (SXINT old_size, SXINT new_size)
{
    (void) old_size;
    (void) new_size;
    ARN.son2attr = (struct son2attr*) sxrealloc (ARN.son2attr,
						 new_size +1,
						 sizeof (struct son2attr));
    return 0;
}

static  void
ARN_paths_oflw (SXINT old_size, SXINT new_size)
{
    (void) old_size;
    (void) new_size;
    ARN.path2attr = (struct path2attr*) sxrealloc (ARN.path2attr,
						   new_size +1,
						   sizeof (struct path2attr));
}
#endif

static  void
ARN_level_trans_oflw (SXINT old_size, SXINT new_size)
{
    (void) old_size;
    (void) new_size;
    ARN.level_trans2attr = (struct level_trans2attr*) sxrealloc (ARN.level_trans2attr,
								 new_size +1,
								 sizeof (struct level_trans2attr));
}

static  void
ARN_symbol_set_oflw (SXINT old_size, SXINT new_size)
{
    (void) old_size;
    (void) new_size;
    ARN.symbol_set2attr = (struct symbol_set2attr*) sxrealloc (ARN.symbol_set2attr,
							       new_size +1,
							       sizeof (struct symbol_set2attr));
}

static  void
ARN_pXs_oflw (SXINT old_size, SXINT new_size)
{
    (void) old_size;
    (void) new_size;
    ARN.pXs2attr = (struct pXs2attr*) sxrealloc (ARN.pXs2attr,
						 new_size +1,
						 sizeof (struct pXs2attr));
}


static  void
ARN_local_trans_oflw (SXINT old_size, SXINT new_size)
{
    (void) old_size;
    (void) new_size;
    ARN.local_trans2attr = (struct local_trans2attr*) sxrealloc (ARN.local_trans2attr,
								 new_size +1,
								 sizeof (struct local_trans2attr));
}

#if 0
static int
ARN_GC (void)
{
    /* sxndparser vient de faire un GC, on en profite... */
    /* Il est possible de l'appeler a n'importe quel moment. */
    return 0;
}


static int
ARN_action_pop (int level, int son, int father)
{
    /* On est au niveau level du depilage courant entre les parser son et father */
    SXINT				path;

    XxY_set (&ARN.paths, son, father, &path);

    ARN.path2attr [path].val = (parse_stack.local_link > 0) ? ARN.parser2attr [father].val
	: ARN.son2attr [parse_stack.local_link].val;

    return 0;
}


static int
ARN_action_top (int xtriple)
{
    /* On est ds "reducer", on va lancer une reduction sur le triple xtriple. */
    /* parse_stack.ared est positionne' */
    /* On positionne les variables utiles pour le traitement de la reduction. */
    struct triple	*ptriple;
    SXINT 		path;
    int 		trans;

    XxY_clear (&ARN.paths);
    ARN.act_no = parse_stack.ared->action;
    ARN.level = parse_stack.ared->lgth;
    parse_stack.for_parsact.action_pop = ARN.act_no > 0 ? ARN_action_pop : NULL;

    ptriple = parse_stack.for_reducer.triples + xtriple;

    if (ptriple->is_shift_reduce)
    {
	ARN.level++;
	trans = XxY_is_set (&ARN.level_trans, ptriple->parser, -xtriple);

#if EBUG
	if (trans == 0)
	    sxtrap (ME, "ARN_action_top");
#endif

	XxY_set (&ARN.paths, ptriple->parser, -xtriple, &path);
	ARN.path2attr [path].val = ARN.level_trans2attr [trans].val;
    }

    if (ARN.level > ARN.stack_size)
	ARN.stack = (int*) sxrealloc (ARN.stack,
				      (ARN.stack_size = ARN.level) + 1,
				      sizeof (int));
    return 0;
}


static void
ARN_action (void)
{
    int	value;

    switch (ARN.act_no)
    {
    case 1:
	break;

    default:
	fputs ("The function \"ARN_action\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }

    /* Si new_top < 0 => on range ds level_trans2attr
       Si new_top > 0 => Si bot est le fils principal de new_top, on range ds parser2attr
                         sinon on range ds sons2attr. */
}


static void
walk_paths (int bot, int level)
{
    int		path;
    
    if (level == ARN.level)
    {
	ARN_action ();
    }
    else
    {
	XxY_Xforeach (ARN.paths, bot, path)
	{
	    ARN.stack [level] = path;
	    walk_paths (XxY_Y (ARN.paths, path), level + 1);
	}
    }
}


static int
ARN_action_bot (int bot)
{
    /* On vient de terminer le depilage, on se contente de noter le parser atteint
       La parsact sera effectuee avec le test du predicat. */
    ARN.bot = bot;

    return 0;
}
#endif


static int
ARN_action_new_top (int bot, int new_top, int symbol)
{
    /* Si new_top == 0, echec syntaxique. */
    /* Attention, il peut exister le meme "symbol" avec 2 "new_top" differents. */
    int		prev_bot;
    SXINT	pXs, x;
    // bool	first_time;

    if (symbol == 0 || new_top == 0)
	return 0;

    if (X_set (&ARN.symbol_set, symbol, &x))
    {
	prev_bot = ARN.symbol_set2attr [x].bot;

	if ((prev_bot & ARN_7F) != bot)
	{
	    if ((prev_bot & ARN_80) == 0)
	    {
		ARN.symbol_set2attr [x].bot |= ARN_80;
		ARN.symbol_set2attr [x].multiple_symbol = ARN.multiple_symbol;
		ARN.multiple_symbol = x;
	    }

	    if (!XxY_set (&ARN.pXs, bot, symbol, &pXs))
	    {
		ARN.pXs2attr [pXs].new_top = new_top;
		ARN.pXs2attr [pXs].local_link = parse_stack.local_link;
	    }
	}
    }
    else
    {
	/* 1ere fois */
	ARN.symbol_set2attr [x].bot = bot;
	ARN.symbol_set2attr [x].new_top = new_top;
	ARN.symbol_set2attr [x].local_link = parse_stack.local_link;
    }

#if 0
    if (XxY_set (&ARN.level_trans, bot, new_top, &level_trans))
	return 0;

    if (first_time = SXBA_bit_is_reset_set (ARN.symbol_set, symbol))
    {
	SS_push (ARN.symbol_stack, symbol);
    }

    ARN.level_trans2attr [level_trans].to_be_kept = first_time;
#endif
    return 0;
}

static void
ARN_vanish_trans (int son, int father, int link)
{
    /* On fait disparaitre la transition entre les parser son et father. */
    struct  parsers_attr	*pattr;
    int				n, local_link, new_main_son;
    // int new_main_symbol;

    if (father > 0)
    {
	/* On est dans la parse_stack */
	pattr = &(parse_stack.parser_to_attr [father]);

	if (pattr->son == son || pattr->son == -son)
	{
	    /* C'est la liaison vers le fils principal qui disparait */
#ifdef EBUG
	    if (link != 0)
		sxtrap (ME, "ARN_vanish_trans");
#endif

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
		pattr->son = 0;

		if (SXBA_bit_is_reset_set (ARN.vanished_parser_set, father))
		    SS_push (ARN.vanished_parser_stack, father);

	    }
	}
	else
	{
	    /* Il y a des fils secondaires */
	    /* son est obligatoirement l'un de ceux-la */

#ifdef EBUG
	    local_link = XxY_is_set (&parse_stack.sons, father, son);

	    if (pattr->son > 0 || local_link == 0 || local_link != link)
		sxtrap (ME, "ARN_vanish_trans");
#endif

	    XxY_erase (parse_stack.sons, link);

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
	parse_stack.for_reducer.triples [-father].is_valid = false;
    }
}


static int
ARN_forward_walk (int son)
{
    int	local_trans, father, symbol;
    // int x;
    SXINT rhs;

    rhs = -1;
    XxY_Xforeach (ARN.local_trans, son, local_trans)
    {
	father = XxY_Y (ARN.local_trans, local_trans);
	symbol = ARN.local_trans2attr [local_trans].symb;

	/* if (symbol <= 0 || !X_set (&ARN.rhs_set, symbol, &x)) */
	{
	    if (father < 0)
	    {
		if (symbol == 0)
		    rhs = 0;
		else
		    XxY_set (&parse_stack.sf_rhs, symbol, 0, &rhs);
	    }
	    else
	    {
		rhs = ARN_forward_walk (father);

		if (rhs >= 0)
		{
		    XxY_set (&parse_stack.sf_rhs, symbol, rhs, &rhs);
		    /* XxY_erase (ARN.local_trans, local_trans); */
		}
	    }
	
	    break;
	}
    }

    return rhs;
}




static void
ARN_build_a_rule (int parser, int rhs)
{
    int lhs, x;
    
    if (new_symbol (parse_stack.ared->reduce,
		    0,
		    parse_stack.parser_to_attr [parser].milestone,
		    parse_stack.current_milestone,
		    &lhs))
#ifdef EBUG
	if (index == 0)
	    sxtrap (ME, "ARN_build_a_rule")
#endif
		;
    
    x = X_is_set (&ARN.symbol_set, lhs);

#ifdef EBUG
    if (x == 0)
	sxtrap (ME, "ARN_build_a_rule");
#endif    


    if (ARN.symbol_set2attr [x].bot == parser)
    {
	if (rhs == -1)
	    rhs = ARN_forward_walk (parser);

	if (rhs >= 0)
	    set_rule (lhs, rhs);
    }
}

static void
ARN_walk_backward (int p, bool is_in_set, int l)
{
    int		x, son, father, first_son, major_son;
    SXINT       local_trans;
    SXBA	fathers_set, sons_set, s;
    int		son_i, father_i;
    bool	is_sons, is_first_time, is_in_sons_set;

    fathers_set = parse_stack.parsers_set [0];
    father_i = 0;

    sons_set = parse_stack.parsers_set [1];
    son_i = 1;

    first_son = p;

    /* lorsqu'on a de'pile' de la bonne longueur, les "fils" sont correctement
       positionnes. */
    while (l-- > 0) {
	/* Les fils du coup precedent deviennent les peres du coup suivant. */
	s = fathers_set, fathers_set = sons_set, sons_set = s;
	x = father_i, father_i = son_i, son_i = x;

	father = first_son;
	first_son = 0;

	is_first_time = true;
	is_in_sons_set = false;

	for (;;) {
#ifdef EBUG
	    parse_stack.red_trans_nb++;    
#endif    

	    if ((is_sons = (major_son = parse_stack.parser_to_attr [father].son) < 0))
		major_son = -major_son;

	    XxY_set (&ARN.local_trans, major_son, father, &local_trans);
	    ARN.local_trans2attr [local_trans].symb = parse_stack.parser_to_attr [father].symbol;
	    
	    if (is_first_time)
		first_son = major_son;
	
	    if (first_son != major_son)
	    {
		is_in_sons_set = true;
		SXBA_1_bit (sons_set, major_son);
	    }
	    
	    if (is_sons) {
		XxY_Xforeach (parse_stack.sons, father, x) {
#ifdef EBUG
		    parse_stack.red_trans_nb++;    
#endif    

		    son = XxY_Y (parse_stack.sons, x);
		    XxY_set (&ARN.local_trans, son, father, &local_trans);
		    ARN.local_trans2attr [local_trans].symb = parse_stack.transitions [x];

		    if (first_son != son)
		    {
			is_in_sons_set = true;
			SXBA_1_bit (sons_set, son);
		    }
		}
	    }

	    if (is_first_time) {
		is_first_time = false;
		father = 0;
	    }

	    if (!is_in_set || (father = sxba_scan_reset (fathers_set, father)) < 0)
		break;
	}

	is_in_set = is_in_sons_set;
    }

    if (first_son > 0)
    {
	ARN_build_a_rule (first_son, -1);
    }

    if (is_in_set)
    {
	son = 0;

	while ((son = sxba_scan_reset (sons_set, son)) > 0)
	{
	    ARN_build_a_rule (son, -1);
	}
    }
}



static void
ARN_parse_forest (void)
{
    /* The construction of the parse forest is not performed within the
       recognizer in order to avoid the multiple definition of the same
       sub-string with different productions. This may arise when a new
       link is added between previous parsers, triggering a future call
       to do_limited_pops and adds a path such that a node with a
       single father becomes multi_fathered and hence produces an inter
       mediate non-terminal which describes a path which has already be
       en processed previously. One solution is to have a two step proc
       ess: first, the recognizer which set all the pathes and the non-
       terminals which are the names of the transitions and second, the
       creation of the parse forest (ie the grammar rules) which uses
       the previous transitions and pathes. */
    short		ref;
    int			parser, xtriple;
    // int		x, symbol;
    SXINT		local_trans, rhs;
    SXBA		parser_set;
    struct triple	*triple;
    bool		is_first = true;

    while ((ref = parse_stack.for_reducer.refs [0]) > 0)
    {
	parse_stack.for_reducer.refs [0] = parse_stack.for_reducer.refs [ref];
	parse_stack.for_reducer.refs [ref] = 0;

	triple = parse_stack.for_reducer.triples + (xtriple = parse_stack.for_reducer.triples_hd [ref]);
	parse_stack.ared = sxplocals.SXP_tables.reductions + ref;

	if (parse_stack.ared->reduce == 0) {
	    /* Halt */
	    /* Normalement chaque parser a un fils unique (son == 2) */
	  do {
#if EBUG
	      if (/* triple->link > 0 || */ !triple->is_valid || !parse_stack.halt_hit ||
		  triple->parser == 0 || parse_stack.parser_to_attr [triple->parser].son <= 0)
		  sxtrap (ME, "ARN_parse_forest");
#endif

	      if (is_first)
	      {
		  is_first = false;
		  set_start_symbol (parse_stack.parser_to_attr [triple->parser].symbol);
	      }

	      triple = parse_stack.for_reducer.triples + triple->link;
	  } while (triple > parse_stack.for_reducer.triples);
	}
	else
	{
	    if (parse_stack.ared->lgth == 0)
	    {
		do
		{
		    if (triple->is_valid)
		    {
			if (triple->is_shift_reduce)
			{
#ifdef EBUG
			    parse_stack.red_trans_nb += 1;

			    if (triple->symbol == 0)
				sxtrap (ME, "ARN_parse_forest");
#endif	    
	    
			    XxY_set (&parse_stack.sf_rhs, triple->symbol, 0, &rhs);
			}
			else
			    rhs = 0;
			
			ARN_build_a_rule (triple->parser, rhs);
		    }

		    triple = parse_stack.for_reducer.triples + triple->link;
		} while (triple > parse_stack.for_reducer.triples);  
	    }
	    else
	    {
		parser = 0;
		parser_set = NULL;

		do
		{
		    if (triple->is_valid)
		    {
			XxY_set (&ARN.local_trans, triple->parser, -xtriple, &local_trans);

#ifdef EBUG
			 if (triple->is_shift_reduce)
			 {
			     parse_stack.red_trans_nb += 1;

			     if (triple->symbol == 0)
				 sxtrap (ME, "parse_forest");
			 }
#endif	    

			ARN.local_trans2attr [local_trans].symb = triple->is_shift_reduce ? triple->symbol : 0;
			
			if (parser == 0)
			{
			    parser = triple->parser;
			}
			else if (triple->parser != parser)
			{
			    if (parser_set == NULL)
				parser_set = parse_stack.parsers_set [1];

			    SXBA_1_bit (parser_set, triple->parser);
			}
		    }

		    triple = parse_stack.for_reducer.triples + (xtriple = triple->link);
		} while (xtriple > 0);  

		if (parser > 0)
		    ARN_walk_backward (parser, parser_set != NULL, parse_stack.ared->lgth);

		XxY_clear (&ARN.local_trans);
	    }
	}
    }

    parse_stack.for_reducer.triples_hd [0] = 0; /* cas ref == 0 */
}

static void
ARN_finalize_current_level (void)
{
    /* On modifie la parse_stack en prenant en compte les transitions
       qui ont disparues. */
    int son, father, parser, level_trans, x;
    int *pp, *pnfs;


#if 0
    XxY_foreach (ARN.level_trans, level_trans)
    {
	if (!ARN.level_trans2attr [level_trans].to_be_kept)
	    ARN_vanish_trans (XxY_X (ARN.level_trans, level_trans), XxY_Y (ARN.level_trans, level_trans));
    }
#endif

    if (!SS_is_empty (ARN.vanished_parser_stack))
    {
	/* Des parser ont disparu, on regarde les consequences... */
	do
	{
	    son = SS_pop (ARN.vanished_parser_stack);

	    XxY_Xforeach (ARN.level_trans, son, level_trans)
	    {
		father = XxY_Y (ARN.level_trans, level_trans);
		ARN_vanish_trans (son, father);
	    }
	} while (!SS_is_empty (ARN.vanished_parser_stack));

	/* On s'occupe des "xtriple" pas is_shift_reduce */
	for (x = 1; x <= parse_stack.for_reducer.top; x++)
	{
	    parser = parse_stack.for_reducer.triples [x].parser;

	    if (SXBA_bit_is_set (ARN.vanished_parser_set, parser))
		parse_stack.for_reducer.triples [x].is_valid = false;
	}

	/* On s'occupe maintenant des "for_scanner" */
	pp = parse_stack.for_scanner.next_hd;

	while ((parser = *pp) > 0)
	{
	    pnfs = &(parse_stack.parser_to_attr [father].next_for_scanner);

	    if (SXBA_bit_is_set (ARN.vanished_parser_set, parser))
		*pp = *pnfs;
	    else
		pp = pnfs;
	}

	sxba_empty (ARN.vanished_parser_set);
    }
}

static int
ARN_action_final (void)
{
    /* On a termine' l'e'valuation d'un niveau, On prepare le niveau suivant. */
    int		x, bot, new_top, keep, perhaps, symbol, pXs;
    // int	trans;

    if ((x = ARN.multiple_symbol) != 0)
    {
	/* Le meme symbole est utilise sur des transitions differentes au niveau courant. */
	/* On conserve une seule transition. On donne priorite a une transition ou new_top est
	   positif et qui est dans for_scanner. */
	ARN.multiple_symbol = 0;

	do {
	    symbol = X_X (ARN.symbol_set, x);
	    new_top = ARN.symbol_set2attr [x].new_top;
	    keep = -1;
	    perhaps = -1;
	    
	    if (new_top > 0)
	    {
		if (parse_stack.parser_to_attr [new_top].scan_ref != 0)
		    keep = 0;
		else
		    perhaps = 0;
	    }
	    else
	    {
		XxY_Yforeach (ARN.pXs, symbol, pXs)
		{
		    new_top = ARN.pXs2attr [pXs].new_top;
		    
		    if (new_top > 0)
		    {
			if (parse_stack.parser_to_attr [new_top].scan_ref != 0)
			{
			    keep = pXs;
			    break;
			}
			else
			{
			    perhaps = pXs;
			}
		    }
		}
	    }

	    if (keep == -1)
	    {
		if (perhaps == -1)
		    keep = 0;
		else
		    keep = perhaps;
	    }

	    if (keep > 0)
		ARN_vanish_trans (ARN.symbol_set2attr [x].bot & ARN_7F,
				  ARN.symbol_set2attr [x].new_top,
				  ARN.symbol_set2attr [x].local_link);

	    XxY_Yforeach (ARN.pXs, symbol, pXs)
	    {
		bot = XxY_X (ARN.pXs, x);

		if (pXs != keep)
		{
		    ARN_vanish_trans (bot,
				      ARN.pXs2attr [x].new_top,
				      ARN.pXs2attr [x].local_link);
		}
		else
		{
		    ARN.symbol_set2attr [x].bot = bot;
		    ARN.symbol_set2attr [x].new_top = ARN.pXs2attr [x].new_top;
		    ARN.symbol_set2attr [x].local_link = ARN.pXs2attr [x].local_link;
		}
	    }
		    
	} while ((x = ARN.symbol_set2attr [x].multiple_symbol) > 0);

	XxY_clear (&ARN.pXs);

	ARN_finalize_current_level ();
    }

    ARN_parse_forest ();

    X_clear (&ARN.symbol_set);
    X_clear (&ARN.rhs_set);

    /* On "supprime" les transitions de level_trans */
    XxY_clear (&ARN.level_trans);

    return 0;
}



bool ARN_parsact (int which, SXTABLES *arg)
{
    switch (which)
    {
    case SXOPEN:
	ARN.sxtables = arg;

	ARN.vanished_parser_set = sxba_calloc (XxYxZ_size (parse_stack.parsers) + 1);
	ARN.vanished_parser_stack = SS_alloc (XxYxZ_size (parse_stack.parsers) + 1);

#if 0
	ARN.symbol_set = sxba_calloc (XxY_size (parse_stack.symbols) + 1);
	ARN.symbol_stack = SS_alloc (XxY_size (parse_stack.symbols) + 1);

	ARN.symb2attr = (struct symb2attr*) sxalloc (XxY_size (parse_stack.symbols) + 1,
						     sizeof (struct symb2attr));

	ARN.parser2attr = (struct parser2attr*) sxalloc (XxYxZ_size (parse_stack.parsers) + 1,
							 sizeof (struct parser2attr));

	ARN.son2attr = (struct son2attr*) sxalloc (XxY_size (parse_stack.sons) + 1,
						   sizeof (struct son2attr));


	XxY_alloc (&ARN.paths, "ARN.paths", 30, 1, 1, 0, ARN_paths_oflw,
#ifdef EBUG
		   stdout
#else
		   NULL
#endif
		   );
	ARN.path2attr = (struct path2attr*) sxalloc (XxY_size (ARN.paths) + 1,
						     sizeof (struct path2attr));
#endif

	XxY_alloc (&ARN.level_trans, "ARN.level_trans", 50, 1, 1, 0, ARN_level_trans_oflw,
#ifdef EBUG
		   stdout
#else
		   NULL
#endif
		   ); 

	ARN.level_trans2attr =
	    (struct level_trans2attr*) sxalloc (XxY_size (ARN.level_trans) + 1,
						sizeof (struct level_trans2attr));

#if 0
	XxY_alloc (&ARN.symbXtrans, "ARN.symbXtrans", 50, 1, 0, 0, NULL,
#ifdef EBUG
		   stdout
#else
		   NULL
#endif
		   ); 

	ARN.stack = (int*) sxalloc ((ARN.stack_size = 10) + 1, sizeof (int));
#endif

	X_alloc (&ARN.symbol_set, "ARN.symbol_set", 30, 1, ARN_symbol_set_oflw,
#ifdef EBUG
		   stdout
#else
		   NULL
#endif
		   );
	ARN.symbol_set2attr = (struct symbol_set2attr*) sxalloc (XxY_size (ARN.symbol_set) + 1,
						     sizeof (struct symbol_set2attr));

	XxY_alloc (&ARN.pXs, "ARN.pXs", 30, 1, 0, 1, ARN_pXs_oflw,
#ifdef EBUG
		   stdout
#else
		   NULL
#endif
		   );
	ARN.pXs2attr = (struct pXs2attr*) sxalloc (XxY_size (ARN.pXs) + 1,
						     sizeof (struct pXs2attr));

	XxY_alloc (&ARN.local_trans, "ARN.local_trans", 30, 1, 1, 0, ARN_local_trans_oflw,
#ifdef EBUG
		   stdout
#else
		   NULL
#endif
		   );
	ARN.local_trans2attr = (struct local_trans2attr*) sxalloc (XxY_size (ARN.local_trans) + 1,
						     sizeof (struct local_trans2attr));

	X_alloc (&ARN.rhs_set, "ARN.rhs_set", 30, 1, NULL,
#ifdef EBUG
		   stdout
#else
		   NULL
#endif
		   );

	parse_stack.for_parsact.action_new_top = ARN_action_new_top;
	parse_stack.for_parsact.symbols_oflw = ARN_symbols_oflw;
	parse_stack.for_parsact.action_final = ARN_action_final;
	parse_stack.for_parsact.parsers_oflw = ARN_parsers_oflw;

#if 0
	parse_stack.for_parsact.action_top = ARN_action_top;
	/* action_pop est initialise au coup par coup ds action_top. */
	parse_stack.for_parsact.action_bot = ARN_action_bot;
	parse_stack.for_parsact.GC = ARN_GC;
	parse_stack.for_parsact.sons_oflw = ARN_sons_oflw;
#endif

	return SXANY_BOOL;

    case SXCLOSE:
	sxfree (ARN.vanished_parser_set);
	SS_free (ARN.vanished_parser_stack);

#if 0
	sxfree (ARN.symbol_set);
	SS_free (ARN.symbol_stack);

	sxfree (ARN.symb2attr);
	sxfree (ARN.parser2attr);
	sxfree (ARN.son2attr);

	XxY_free (&ARN.paths);
	sxfree (ARN.path2attr);
	sxfree (ARN.level_trans2attr);

	XxY_free (&ARN.symbXtrans);

	sxfree (ARN.stack);
#endif
	XxY_free (&ARN.level_trans);

	X_free (&ARN.symbol_set);
	sxfree (ARN.symbol_set2attr);

	XxY_free (&ARN.pXs);
	sxfree (ARN.pXs2attr);

	XxY_free (&ARN.local_trans);
	sxfree (ARN.local_trans2attr);

	X_free (&ARN.rhs_set);

	return SXANY_BOOL;

    case SXINIT:
	sxplocals.mode.with_do_undo = true;
	ARN.multiple_symbol = 0;

	return SXANY_BOOL;

    case SXFINAL:
	return SXANY_BOOL;

    case SXACTION:
	/* La memorisation se fait ds "PREDICATE:" */
	return SXANY_BOOL;

    case SXDO:
	return 0;


    case SXUNDO:
	return 0;

    default:
	fputs ("The function \"ARN_parsact\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }

    return SXANY_BOOL;
}
