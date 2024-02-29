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

char WHAT[] = "@(#)ARN_parsact.c	- SYNTAX [unix] - Mer 31 Mai 1995 10:50:06"

static char	ME [] = "ARN_parsact";

# include 	"sxnd.h"
# include 	"ARN_parsact.h"

static struct ARN	ARN;

static  int
ARN_symbols_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ARN.symbol_set = sxba_resize (ARN.symbol_set, new_size + 1);
    ARN.symb2attr = (struct symb2attr*) sxrealloc (ARN.symb2attr,
						   new_size +1,
						   sizeof (struct symb2attr));
    return 0;
}

static  int
ARN_parsers_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ARN.parser2attr = (struct parser2attr*) sxrealloc (ARN.parser2attr,
						       new_size +1,
						       sizeof (struct parser2attr));
    return 0;
}

static  int
ARN_sons_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ARN.son2attr = (struct son2attr*) sxrealloc (ARN.son2attr,
						 new_size +1,
						 sizeof (struct son2attr));
    return 0;
}

static  void
ARN_paths_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ARN.path2attr = (struct path2attr*) sxrealloc (ARN.path2attr,
						   new_size +1,
						   sizeof (struct path2attr));
}

static  void
ARN_local_trans_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ARN.local_trans2attr = (struct local_trans2attr*) sxrealloc (ARN.local_trans2attr,
								 new_size +1,
								 sizeof (struct local_trans2attr));
}


static int
ARN_GC ()
{
    /* sxndparser vient de faire un GC, on en profite... */
    /* Il est possible de l'appeler a n'importe quel moment. */
    return 0;
}


static int
ARN_action_pop (level, son, father)
    int level, son, father;
{
    /* On est au niveau level du depilage courant entre les parser son et father */
    int				path;

    XxY_set (&ARN.paths, son, father, &path);

    ARN.path2attr [path].val = (parse_stack.local_link > 0) ? ARN.parser2attr [father].val
	: ARN.son2attr [parse_stack.local_link].val;

    return 0;
}

static int
ARN_action_top (xtriple)
    int	xtriple;
{
    /* On est ds "reducer", on va lancer une reduction sur le triple xtriple. */
    /* parse_stack.ared est positionne' */
    /* On positionne les variables utiles pour le traitement de la reduction. */
    struct triple	*ptriple;
    int 		path, trans;

    XxY_clear (&ARN.paths);
    ARN.act_no = parse_stack.ared->action;
    ARN.level = parse_stack.ared->lgth;
    parse_stack.for_parsact.action_pop = ARN.act_no > 0 ? ARN_action_pop : NULL;

    ptriple = parse_stack.for_reducer.triples + xtriple;

    if (ptriple->is_shift_reduce)
    {
	ARN.level++;
	trans = XxY_is_set (&ARN.local_trans, ptriple->parser, -xtriple);

#if EBUG
	if (trans == 0)
	    sxtrap (ME, "ARN_action_top");
#endif

	XxY_set (&ARN.paths, ptriple->parser, -xtriple, &path);
	ARN.path2attr [path].val = ARN.local_trans2attr [trans].val;
    }

    if (ARN.level > ARN.stack_size)
	ARN.stack = (int*) sxrealloc (ARN.stack,
				      (ARN.stack_size = ARN.level) + 1,
				      sizeof (int));

    return 0;
}


static void
ARN_action ()
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

    /* Si new_top < 0 => on range ds local_trans2attr
       Si new_top > 0 => Si bot est le fils principal de new_top, on range ds parser2attr
                         sinon on range ds sons2attr. */
}


static void
walk_paths (bot, level)
    int bot, level;
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
ARN_action_bot (bot)
    int bot;
{
    /* On vient de terminer le depilage, on se contente de noter le parser atteint
       La parsact sera effectuee avec le test du predicat. */
    ARN.bot = bot;

    return 0;
}



static int
ARN_action_new_top (bot, new_top, symbol)
    int bot, new_top, symbol;
{
    /* Si new_top == 0, echec syntaxique. */
    /* Attention, il peut exister le meme "symbol" avec 2 "new_top" differents. */
    int		local_trans, symbXtrans;

    if (symbol == 0 || new_top == 0)
	return 0;

    ARN.is_old_trans = XxY_set (&ARN.local_trans, bot, new_top, &local_trans);

    if (ARN.act_no <= 0)
	return 0;

    ARN.cur_trans = local_trans;
    ARN.cur_symb = symbol;

    if (SXBA_bit_is_reset_set (ARN.symbol_set, symbol))
    {
	SS_push (ARN.symbol_stack, symbol);
	ARN.symb2attr [symbol].trans = local_trans;
    }
    else
    {
	XxY_set (&ARN.symbXtrans, symbol, local_trans, &symbXtrans);
	ARN.symb2attr [symbol].trans |= ARN_80;
    }

    walk_paths (bot, 0);

    return 0;
}


static int
ARN_action_final ()
{
    /* On a termine' l'e'valuation d'un niveau, On prepare le niveau suivant. */
    int		trans, symbol, symbXtrans;

    XxY_clear (&ARN.local_trans);

    while (!SS_is_empty (ARN.symbol_stack)) {
	symbol = SS_pop (ARN.symbol_stack);
	SXBA_0_bit (ARN.symbol_set, symbol);

	trans = ARN.symb2attr [symbol].trans;

	if (trans & ARN_80)
	{
	    /* symbol est "utilise" ds plusieurs transitions, on conserve la bonne. */
	    XxY_Xforeach (ARN.symbXtrans, symbol, symbXtrans)
	    {
		/* "Supprimer" la transition. */
		trans = XxY_Y (ARN.symbXtrans, symbXtrans);
	    }
	}
    }

    XxY_clear (&ARN.symbXtrans);

    return 0;
}



int ARN_parsact (which, arg)
    int		which;
    struct sxtables	*arg;
{
    switch (which)
    {
    case SXOPEN:
	ARN.sxtables = arg;

	ARN.symbol_set = sxba_calloc (XxY_size (parse_stack.symbols) + 1);
	ARN.symbol_stack = SS_alloc (XxY_size (parse_stack.symbols) + 1);
	ARN.symb2attr = (struct symb2attr*) sxalloc (XxY_size (parse_stack.symbols) + 1,
						     sizeof (struct symb2attr));

	ARN.parser2attr = (struct parser2attr*) sxalloc (XxY_size (parse_stack.parsers) + 1,
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

	XxY_alloc (&ARN.local_trans, "ARN.local_trans", 500, 1, 0, 0, ARN_local_trans_oflw,
#ifdef EBUG
		   stdout
#else
		   NULL
#endif
		   ); 
	ARN.local_trans2attr =
	    (struct local_trans2attr*) sxalloc (XxY_size (ARN.local_trans) + 1,
						sizeof (struct local_trans2attr));

	XxY_alloc (&ARN.symbXtrans, "ARN.symbXtrans", 50, 1, 0, 0, NULL,
#ifdef EBUG
		   stdout
#else
		   NULL
#endif
		   ); 

	ARN.stack = (int*) sxalloc ((ARN.stack_size = 10) + 1, sizeof (int));

	parse_stack.for_parsact.action_top = ARN_action_top;
	/* action_pop est initialise au coup par coup ds action_top. */
	parse_stack.for_parsact.action_bot = ARN_action_bot;
	parse_stack.for_parsact.action_new_top = ARN_action_new_top;
	parse_stack.for_parsact.action_final = ARN_action_final;
	parse_stack.for_parsact.GC = ARN_GC;
	parse_stack.for_parsact.parsers_oflw = ARN_parsers_oflw;
	parse_stack.for_parsact.sons_oflw = ARN_sons_oflw;
	parse_stack.for_parsact.symbols_oflw = ARN_symbols_oflw;

	return 0;

    case SXCLOSE:
	sxfree (ARN.symbol_set);
	SS_free (ARN.symbol_stack);
	sxfree (ARN.symb2attr);
	sxfree (ARN.parser2attr);
	sxfree (ARN.son2attr);

	XxY_free (&ARN.paths);
	sxfree (ARN.path2attr);

	XxY_free (&ARN.local_trans);
	sxfree (ARN.local_trans2attr);

	XxY_free (&ARN.symbXtrans);

	sxfree (ARN.stack);

	return 0;

    case SXINIT:
	sxplocals.mode.with_do_undo = true;

	return 0;

    case SXFINAL:
	return 0;

    case SXACTION:
	/* La memorisation se fait ds "PREDICATE:" */
	return 0;

    case SXDO:
	return 0;


    case SXUNDO:
	return 0;

    default:
	fputs ("The function \"ARN_parsact\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }

    return 0;
}
