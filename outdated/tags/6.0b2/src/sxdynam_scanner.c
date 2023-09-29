/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.	  *
   *							  *
   ******************************************************** */





/* Gestionnaire dynamique d'expressions regulieres (a la LECL) */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 29-04-94 10:32 (pb):	Ajout de cette rubrique "modifications" 	*/
/************************************************************************/


#include "sxcommon.h"
#include "sxdynam_scanner.h"

char WHAT_SXDYNAM_SCANNER[] = "@(#)SYNTAX - $Id: sxdynam_scanner.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;


#define ts_put(i,c)						\
(SXDS->dslv [i].token_string [SXDS->dslv [i].ts_lgth++] = c,	\
 (SXDS->dslv [i].ts_lgth >= SXDS->dslv [i].ts_size)		\
	? SXDS->dslv [i].token_string =				\
         (char *) sxrealloc (SXDS->dslv [i].token_string,	\
			     SXDS->dslv [i].ts_size *= 2,	\
			     sizeof (char))			\
	: NULL							\
)


#define ts_null(i)	(SXDS->dslv [i].token_string [SXDS->dslv [i].ts_lgth] = NUL)


#define read_a_char()	(SXDS->previous_char = SXDS->current_char,		\
			 SXDS->current_char = sxnext_char ())


static char *err_titles[]={
"\000",
"\001Warning:\t",
"\002Error:\t",
};

static BOOLEAN
process_predicate (lvno, state)
    int lvno, state;
{
    int		prdct;
    BOOLEAN	is_prdct_satisfied;

    sxinitialise(is_prdct_satisfied); /* pour faire taire "gcc -Wuninitialized" */
    prdct = SXDS->item_to_attr [state].val;

    if (prdct >= 0)
    {
	/* Predicat utilisateur */
	ts_null (lvno);
	    
	is_prdct_satisfied = SXDS->scanact == NULL ? TRUE : (SXDS->mode.with_user_prdct
	    ? (*SXDS->scanact) (PREDICATE, lvno, prdct) : FALSE);
    }
    else
    {
	/* Predicat systeme */
	switch (prdct)
	{
	case IsFalse:
	    is_prdct_satisfied = FALSE;
	    break;

	case IsTrue:
	    is_prdct_satisfied = TRUE;
	    break;

	case IsFirstCol:
	    is_prdct_satisfied = SXDS->previous_char == NEWLINE;
	    break;

	case IsSet:
	    is_prdct_satisfied =
		(SXDS->dslv [lvno].counters [SXDS->item_to_attr [state].param] != 0);
	    break;
		
	case IsReset:
	    is_prdct_satisfied =
		(SXDS->dslv [lvno].counters [SXDS->item_to_attr [state].param] == 0);
	    break;

	case IsLastCol:
	    {
		SHORT		sxchar;
		
		if (SXDS->mode.mode == SXNORMAL_SCAN)
		    sxchar = sxlafirst_char ();
		else
		{
		    sxchar = sxlanext_char ();
		    sxlaback (1);
		}
		
		is_prdct_satisfied = sxchar == NEWLINE || sxchar == EOF;
	    }
	default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
	  sxtrap("sxdynam_scanner","unknown switch case #1");
#endif
	  break;
	}
    }

    return is_prdct_satisfied;
}


static void
process_action (lvno, action, param)
    int	lvno, action, param;
{
    int			i;
    char		*pstr;
    struct sxdslv	*plv;

    if (action < 0 && SXDS->mode.with_system_act) {
	plv = SXDS->dslv + lvno;

	switch (action) {
	case LowerToUpper:
	    /* Lower_To_Upper (last char) */
	    pstr = &(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1]);
	    *pstr = sxtoupper (*pstr);
	    break;

	case UpperToLower:
	    /* Upper_To_Lower (last char) */
	    pstr = &(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1]);
	    *pstr = sxtolower (*pstr);
	    break;

	case Set:
	    plv->counters [param] = 1;
	    break;
	    
	case Reset:
	    plv->counters [param] = 0;
	    break;
	    
	case Erase:
	    if (plv->ts_lgth > plv->mark.index)
		plv->ts_lgth = plv->mark.index == -1 ? 0 : plv->mark.index;
	    
	    plv->mark.index = -1 /* no more Mark */ ;
	    break;
	    
	case Include:
	    /* Not yet processed */
	    break;
	    
	case UpperCase:
	    /* Upper_Case (token_string) */
	    for (i = 0; i < sxsvar.sxlv.ts_lgth; i++) {
		pstr = &(sxsvar.sxlv_s.token_string [i]);
		*pstr = sxtoupper (*pstr);
	    }
	    
	    break;
	    
	case LowerCase:
	    /* Lower_Case (token_string)_To_Lower */
	    for (i = 0; i < sxsvar.sxlv.ts_lgth; i++) {
		pstr = &(sxsvar.sxlv_s.token_string [i]);
		*pstr = sxtolower (*pstr);
	    }
	    
	    break;
	    
	case Put:
	    /* Unput (char) */
	    ts_put (lvno, param);
	    break;
	    
	case Mark:
	    plv->mark.source_coord = sxsrcmngr.source_coord;
	    plv->mark.index = plv->ts_lgth;
	    plv->mark.previous_char = SXDS->previous_char;
	    break;
	    
	case Release:
	    if (plv->mark.index == -1)
	    {
		plv->mark.source_coord = SXDS->terminal_token.source_index;
		plv->mark.index = 0;
		plv->mark.previous_char = SXDS->previous_char;
	    }

	    if (plv->ts_lgth > plv->mark.index) {
		ts_null (lvno);
		plv->ts_lgth = plv->mark.index;
		SXDS->previous_char = plv->mark.previous_char;
		SXDS->current_char = plv->token_string [plv->ts_lgth];
		sxsrcpush (SXDS->previous_char,
			   &(plv->token_string [plv->ts_lgth]),
			   plv->mark.source_coord);
	    }
	    
	    plv->mark.index = -1 /* no more Mark */ ;
	    break;
	    
	case Incr:
	    plv->counters [param]++;
	    break;
	    
	case Decr:
	    plv->counters [param]--;
	    break;
	default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
	  sxtrap("sxdynam_scanner","unknown switch case #2");
#endif
	  break;
	} /* case  */
    } /* end system action */
    else {
	/* user action */
	/* Probleme de la synchronisation entre une action qui lit du source
	   et les autres scanners non-deterministes en parallele.
	   La description lexicale doit se faire par :
	       ... @k {Any &l} ...
	   @k : est l'action qui traite du source jusqu'en (par exemple ligne
	        i colonne j). Le texte doit etre lu en look-ahead (afin de pouvoir etre
		relu).
	   &l : retourne vrai tant que le caractere courant n'est pas en ligne i colonne j
	*/
	ts_null (lvno);
	
	if (SXDS->scanact != NULL && SXDS->mode.with_user_act)
	    (VOID) (*SXDS->scanact) (ACTION, lvno, action);
	
	/* cas tordu :
	   
	   %STRING		= QUOTE {
	   ^QUOTE |
	   @Mark -EOL&True @Put (QUOTE) @Put (EOL) @Release @1
	   }
	   QUOTE ;
	   
	   -- @1 : Error message "Illegal occurrence of EOL in STRING".
	   
	   */
    }
}

static void
sxds_lvcpy (j, i, all)
    int		j, i;
    BOOLEAN	all;
{
    struct sxdslv	*plv_j, *plv_i;
    int			cs;

    /* On copie i ds j */
    plv_j = SXDS->dslv + j;
    plv_i = SXDS->dslv + i;

    if (plv_j->token_string == NULL) {
	plv_j->token_string =
	    (char*) sxalloc (plv_j->ts_size = plv_i->ts_size, sizeof (char));
    }
    else
	if (all && (plv_j->ts_size <= plv_i->ts_lgth))
	{
	    plv_j->token_string = (char*) sxrealloc (plv_j->token_string,
						       plv_j->ts_size = plv_i->ts_size,
						       sizeof (char));
	}
    
    if (all)
    {
	memcpy (plv_j->token_string, plv_i->token_string, (size_t)plv_i->ts_lgth + 1);
	plv_j->ts_lgth = plv_i->ts_lgth;
    }
    else
	plv_j->ts_lgth = 0;

    if ((cs = SXDS->counters_size) >= 0)
    {
	if (plv_j->counters == NULL) {
	    plv_j->counters = (long*) sxcalloc ((unsigned long int) cs + 1, sizeof (long));		
	}

	do {
	    plv_j->counters [cs] = plv_i->counters [cs];
	} while (--cs >= 0);

    }

    plv_j->mark = plv_i->mark;
}


static int
sxds_clone (old, current_state, all)
    int		old, current_state;
    BOOLEAN	all;
{
    /* Si all == FALSE, token_string est initialise' a vide. */
    int			i;
    struct sxdslv	*plv_top;

    if (++SXDS->lv_top >= SXDS->lv_size)
    {
	SXDS->dslv = (struct sxdslv*) sxrealloc (SXDS->dslv,
						 SXDS->lv_size *= 2,
						 sizeof (struct sxdslv));

	for (i = SXDS->lv_top; i < SXDS->lv_size; i++)
	{
	    SXDS->dslv [i].token_string = NULL;
	    SXDS->dslv [i].counters = NULL;
	}

	SXDS->lvno_set = sxba_resize (SXDS->lvno_set, SXDS->lv_size);
	SXDS->lvno_clones = (int*) sxrealloc (SXDS->lvno_clones, SXDS->lv_size, sizeof (int));
    }

    sxds_lvcpy (SXDS->lv_top, old, all);

    plv_top = SXDS->dslv + SXDS->lv_top;

    plv_top->prev_lvno = old;
    plv_top->char_no = SXDS->current_char_no;
    plv_top->state = current_state;

    return SXDS->lv_top;
}




static void
fill_ctxt_set (re)
    int re;
{
    char	context_kind;

    context_kind = SXDS->reg_exp_to_attr [re].context_kind;

    if (context_kind == Context || context_kind == UnboundedContext)
	sxba_copy (SXDS->ctxt_set, SXDS->context_sets [re]);
    else
	if (context_kind == ContextAll || context_kind == UnboundedContextAll)
	    sxba_fill (SXDS->ctxt_set);
	else
	{
	    /* ContextAllBut */
	    sxba_copy (SXDS->ctxt_set, SXDS->context_sets [re]);
	    sxba_not (SXDS->ctxt_set);
	}
}


#define GetPriorityKind(s)	SXDS->reg_exp_to_attr[SXDS->item_to_attr[s].reg_exp_no].priority_kind
#define GetActionId(s)		(SXDS->item_to_attr[s].kind!=KREDUCE)?0:	\
                                SXDS->reg_exp_to_attr[SXDS->item_to_attr[s].reg_exp_no].reduce_id


static BOOLEAN
check_action_path (lvno1, lvno2)
    int lvno1, lvno2;
{
    /* lvno1 > 0 et lvno2 > 0 et lvno1 != lvno2 et leur "state" reference des actions.
       leur "char_no" est le courant. */
    struct sxdslv	*plv1, *plv2;

    plv1 = SXDS->dslv + lvno1;
    plv2 = SXDS->dslv + lvno2;

    do
    {
	if (SXDS->item_to_attr [plv1->state].val != SXDS->item_to_attr [plv2->state].val ||
	    SXDS->item_to_attr [plv1->state].param != SXDS->item_to_attr [plv2->state].param)
	    return FALSE;

	lvno1 = plv1->prev_lvno;
	lvno2 = plv2->prev_lvno;

	if (lvno1 == lvno2)
	    return TRUE;

	plv1 = lvno1 > 0 ? SXDS->dslv + lvno1 : NULL;

	if (plv1 != NULL && (plv1->char_no != SXDS->current_char_no ||
			     SXDS->item_to_attr [plv1->state].kind != KACTION))
	    plv1 = NULL;

	plv2 = lvno2 > 0 ? SXDS->dslv + lvno2 : NULL;

	if (plv2 != NULL && (plv2->char_no != SXDS->current_char_no ||
			     SXDS->item_to_attr [plv2->state].kind != KACTION))
	    plv2 = NULL;
    } while (plv1 != NULL && plv2 != NULL);

    return plv1 == NULL && plv2 == NULL;
}


#define is_reduce_0_la(p)	(p->current.state == p->prev.state && \
                                 SXDS->item_to_attr[p->current.state].kind == KREDUCE)
#define HasAction(n)		n > 0 &&						\
                                SXDS->dslv [n].char_no == SXDS->current_char_no &&	\
                                SXDS->item_to_attr [SXDS->dslv [n].state].kind == KACTION
#define GetConflictKind(c)	(c->top==0?0:c->conflict_kind)


static void
Push (pconfigs, init_state, init_lvno, prev_state, prev_lvno, current_state, current_lvno)
    struct configs	*pconfigs;
    int			init_state, init_lvno, prev_state, prev_lvno, current_state, current_lvno;
{
    struct config	*pconfig;
    int			first_reduce_id, first_lvno;
    int			current_reduce_id, reduce_id;
    int			current_action_id, action_id;
    int			prdct_nb, is_true_nb;
    char		conflict_kind;
    char		priority_kind;
    BOOLEAN		is_true, has_prdct, current_is_true, current_has_prdct,
                        current_has_action;

    sxinitialise(first_lvno); /* pour faire taire "gcc -Wuninitialized" */
    /* Si on empile une configuration verifiee par un predicat, elle invalide
       toutes les configurations shifts non verifiees par un predicat.
       Si de plus ce predicat est &True, toutes les configurations shifts n'ayant
       pas &True doivent etre supprimees. */

    current_has_prdct = SXDS->item_to_attr [current_state].kind == KPREDICATE;
    current_is_true = current_has_prdct && SXDS->item_to_attr [current_state].val == IsTrue;

    if (pconfigs->top == 0)
    {
	pconfigs->prdct_nb = 0;
	pconfigs->is_true_nb = 0;
    }

    if ((pconfigs->prdct_nb > 0 && !current_has_prdct) ||
	(pconfigs->is_true_nb > 0 && !current_is_true))
	return;

    current_has_action = HasAction (current_lvno);

    first_reduce_id = -1;
    conflict_kind = 0;
    priority_kind = 0;
    prdct_nb = 0;
    is_true_nb = 0;
    current_reduce_id =
	SXDS->reg_exp_to_attr [SXDS->item_to_attr [init_state].reg_exp_no].reduce_id;
    current_action_id = GetActionId (init_state);
    
    ForeachConfig (pconfigs, pconfig)
    {
	has_prdct = SXDS->item_to_attr [pconfig->current.state].kind == KPREDICATE;
	is_true = has_prdct && SXDS->item_to_attr [pconfig->current.state].val == IsTrue;

	if (current_has_prdct)
	{
	    if (!is_reduce_0_la (pconfig))
	    {
		if (!has_prdct ||
		    (current_is_true && !is_true))
		{
		    EraseConfig (pconfig);
		    continue;
		}
	    }
	}

	if (pconfig->current.lvno != current_lvno &&
	    current_has_action &&
	    HasAction (pconfig->current.lvno))
	{
	    /* On regarde si la sequence d'actions est identique. */
	    if (check_action_path (pconfig->current.lvno, current_lvno))
		current_lvno = pconfig->current.lvno;
	}

	action_id = GetActionId (pconfig->init.state);

	if (pconfig->current.state == current_state)
	{
	    if (pconfig->current.lvno == current_lvno)
	    {
		if (SXDS->item_to_attr [pconfig->prev.state].is_erased ==
		    SXDS->item_to_attr [prev_state].is_erased)
		{
		    /* prise en compte des "is_erased" */
		    action_id = GetActionId (pconfig->init.state);

		    if (current_action_id == action_id)
		    {
			/* Le test "current_action_id == action_id" permet de ne pas confondre,
			   ds le cas "0la", (A->alpha .a, A->alpha a.) avec
			   (A->alpha a., A->alpha a.) */
			reduce_id = SXDS->reg_exp_to_attr
			    [SXDS->item_to_attr [pconfig->init.state].reg_exp_no].reduce_id;

			if (current_reduce_id == reduce_id)
			{
			    /* Les 2 chemins ont meme etat initial et meme etat final et ont une
			       intersection non vide. */
			    return;
			}

			conflict_kind |= Ambiguous;
		    }
		}
	    }
	    /* Ici, les actions different, des predicats futurs pourront peut-etre
	       differencier ces chemins? */
	}

	if (has_prdct)
	{
	    prdct_nb++;

	    if (is_true)
		is_true_nb++;
	}

	if (action_id == 0)
	    conflict_kind |= Shift;
	else
	{
	    priority_kind |= GetPriorityKind (pconfig->init.state);

	    if (first_reduce_id < 0)
	    {
		first_reduce_id = action_id;
		first_lvno = pconfig->init.lvno;
		conflict_kind |= FirstReduce;
	    }
	    else
	    {
		if (action_id != first_reduce_id || pconfig->init.lvno != first_lvno)
		    conflict_kind |= NextReduce;
	    }
	}
    }


    if (++pconfigs->top >= pconfigs->size)
	pconfigs->t = (struct config*) sxrealloc (pconfigs->t,
						  pconfigs->size *= 2,
						  sizeof (struct config));
    
    pconfigs->conflict_kind = conflict_kind;

    if (current_has_prdct)
    {
	prdct_nb++;

	if (current_is_true)
	    is_true_nb++;
    }

    pconfigs->prdct_nb = prdct_nb;
    pconfigs->is_true_nb = is_true_nb;
    
    if (current_action_id == 0)
	/* Shift */
	pconfigs->conflict_kind |= Shift;
    else
    {
	pconfigs->priority_kind = priority_kind | GetPriorityKind (init_state);

	if (first_reduce_id < 0)
	    pconfigs->conflict_kind |= FirstReduce;
	else
	    if (first_reduce_id != current_action_id || first_lvno != current_lvno)
		pconfigs->conflict_kind |= NextReduce;
    }

    pconfigs->last = pconfig = pconfigs->t + pconfigs->top;

    pconfig->init.state = init_state;
    pconfig->init.lvno = init_lvno;
    pconfig->prev.state = prev_state;
    pconfig->prev.lvno = prev_lvno;
    pconfig->current.state = current_state;
    pconfig->current.lvno = current_lvno;
}



static void
SetConflict (pconfigs, check_ambiguity)
    struct configs	*pconfigs;
    BOOLEAN		check_ambiguity;
{
    struct config	*pconfig, *pconfig2;
    int			current_action_id, action_id, current_state, first_reduce_id, first_lvno;

    pconfigs->conflict_kind = 0;
    pconfigs->priority_kind = 0;
    pconfigs->prdct_nb = 0;
    pconfigs->is_true_nb = 0;

    sxinitialise(first_lvno); /* pour faire taire "gcc -Wuninitialized" */
    first_reduce_id = -1;

    ForeachConfig (pconfigs, pconfig)
    {
	pconfigs->last = pconfig;
	action_id = GetActionId (pconfig->init.state);
	
	if (SXDS->item_to_attr [pconfig->current.state].kind == KPREDICATE)
	{
	    pconfigs->prdct_nb++;

	    if (SXDS->item_to_attr [pconfig->current.state].val == IsTrue)
		pconfigs->is_true_nb++;
	}

	if (action_id == 0)
	    pconfigs->conflict_kind |= Shift;
	else
	{
	    pconfigs->priority_kind |= GetPriorityKind (pconfig->init.state);

	    if (first_reduce_id < 0)
	    {
		first_reduce_id = action_id;
		first_lvno = pconfig->init.lvno;
		pconfigs->conflict_kind |= FirstReduce;
	    }
	    else
	    {
		if (action_id != first_reduce_id ||
		    pconfig->init.lvno != first_lvno)
		    pconfigs->conflict_kind |= NextReduce;
	    }
	}

    }

    if (check_ambiguity &&
	pconfigs->conflict_kind & FirstReduce  /* Y-a du Reduce */ &&
	(pconfigs->conflict_kind & Shift /* Y-a du Shift */ ||
	 pconfigs->conflict_kind & NextReduce /* ou du Reduce */))
    {
	/* Conflit */
	/* On regarde s'il y a ambiguite'. */
	ForeachConfig (pconfigs, pconfig2)
	{
	    current_action_id = GetActionId (pconfig->init.state);
	    current_state = pconfig2->current.state;

	    for (pconfig = pconfig2 - 1; pconfig > pconfigs->t; pconfig--)
	    {
		if (pconfig->current.state == current_state)
		{
		    if (GetActionId (pconfig->init.state) != current_action_id)
		    {
			pconfigs->conflict_kind |= Ambiguous;
			return;
		    }
		}
	    }
	}
    }
}


static void
test_0la (pconfigs, current_state, current_lvno)
    struct configs	*pconfigs;
    int			current_state, current_lvno;
{
    int			new_lvno, couple, next_state, char_code;
    struct item_to_attr	*pattr = SXDS->item_to_attr + current_state;

    switch (pattr->kind)
    {
    case KCLASS:
	char_code = SXDS->current_char + 1;

	if (SXBA_bit_is_set (SXDS->char_sets [pattr->val], char_code))
	    XxY_Xforeach (SXDS->nfsa, current_state, couple)
	    {
		next_state = XxY_Y (SXDS->nfsa, couple);
		    
		if (SXDS->item_to_attr [next_state].kind != KPREDICATE ||
		    process_predicate (current_lvno, next_state))
		    Push (pconfigs,
			  current_state, current_lvno,
			  current_state, current_lvno,
			  next_state, current_lvno);
	    }

	break;

    case KACTION:
	new_lvno = sxds_clone (current_lvno, current_state, TRUE);

	process_action (new_lvno, pattr->val, pattr->param);

	XxY_Xforeach (SXDS->nfsa, current_state, couple)
	    test_0la (pconfigs, XxY_Y (SXDS->nfsa, couple), new_lvno);

	break;

    case KREDUCE:
	Push (pconfigs,
	      current_state, current_lvno,
	      current_state, current_lvno,
	      current_state, current_lvno);
	
	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxdynam_scanner","unknown switch case #3");
#endif
      break;
    }
}



static void
test_1la (pconfigs, init_state, init_lvno, current_state, current_lvno)
    struct configs	*pconfigs;
    int			init_state, init_lvno, current_state, current_lvno;
{
    int			new_lvno, couple, next_state, re, init_item, char_code;
    struct item_to_attr	*pattr = SXDS->item_to_attr + current_state;

    switch (pattr->kind)
    {
    case KCLASS:
	char_code = SXDS->current_char + 1;

	if (SXBA_bit_is_set (SXDS->char_sets [pattr->val], char_code))
	    XxY_Xforeach (SXDS->nfsa, current_state, couple)
	    {
		next_state = XxY_Y (SXDS->nfsa, couple);
		    
		if (SXDS->item_to_attr [next_state].kind != KPREDICATE ||
		    process_predicate (current_lvno, next_state))
		    Push (pconfigs,
			  init_state, init_lvno,
			  current_state, current_lvno,
			  next_state, current_lvno);
	    }

	break;

    case KACTION:
	new_lvno = sxds_clone (current_lvno, current_state, TRUE);

	process_action (new_lvno, pattr->val, pattr->param);

	XxY_Xforeach (SXDS->nfsa, current_state, couple)
	    test_1la (pconfigs, init_state, init_lvno,
		      XxY_Y (SXDS->nfsa, couple), new_lvno);

	break;

    case KREDUCE:
	/* faut-il executer les post-actions ? */
	/* Il semblerait que oui : (modif des counters, ...) */
	/* Pour l'instant, on ne fait rien, on restreint donc leur effet au
	   terminal_token. */
	fill_ctxt_set (SXDS->item_to_attr [current_state].reg_exp_no);
	sxba_and (SXDS->ctxt_set, SXDS->filter);

	re = -1;
	while ((re = sxba_scan (SXDS->ctxt_set, re)) >= 0)
	{
	    init_item = SXDS->reg_exp_to_attr [re].init_item;

	    XxY_Xforeach (SXDS->nfsa, init_item, couple)
		test_1la (pconfigs, init_state, init_lvno,
			  XxY_Y (SXDS->nfsa, couple), current_lvno);
	}

	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxdynam_scanner","unknown switch case #4");
#endif
      break;
    }
}


static void
test_unbounded (pconfigs, init_state, init_lvno, current_state, current_lvno)
    struct configs	*pconfigs;
    int			init_state, init_lvno, current_state, current_lvno;
{
    int			new_lvno, couple, next_state, re, init_item, char_code;
    struct item_to_attr	*pattr = SXDS->item_to_attr + current_state;

    switch (pattr->kind)
    {
    case KCLASS:
	char_code = SXDS->current_char + 1;

	if (SXBA_bit_is_set (SXDS->char_sets [pattr->val], char_code))
	    XxY_Xforeach (SXDS->nfsa, current_state, couple)
	    {
		next_state = XxY_Y (SXDS->nfsa, couple);
		    
		if (SXDS->item_to_attr [next_state].kind != KPREDICATE ||
		    process_predicate (current_lvno, next_state))
		    Push (pconfigs,
			  init_state, init_lvno,
			  current_state, current_lvno,
			  next_state, current_lvno);
	    }

	break;

    case KACTION:
	new_lvno = sxds_clone (current_lvno, current_state, TRUE);

	process_action (new_lvno, pattr->val, pattr->param);

	XxY_Xforeach (SXDS->nfsa, current_state, couple)
	    test_unbounded (pconfigs, init_state, init_lvno,
			    XxY_Y (SXDS->nfsa, couple), new_lvno);

	break;

    case KREDUCE:
	/* faut-il executer les post-actions ? */
	re = SXDS->item_to_attr [current_state].reg_exp_no;

	if (SXDS->reg_exp_to_attr [re].context_kind >= UnboundedContext)
	{
	    /* Unbounded : on conserve */
	    fill_ctxt_set (re);
	    sxba_and (SXDS->ctxt_set, SXDS->filter);
	    new_lvno = sxds_clone (current_lvno, current_state, FALSE);

	    re = -1;
	    while ((re = sxba_scan (SXDS->ctxt_set, re)) >= 0)
	    {
		init_item = SXDS->reg_exp_to_attr [re].init_item;

		XxY_Xforeach (SXDS->nfsa, init_item, couple)
		    test_unbounded (pconfigs, init_state, init_lvno,
				    XxY_Y (SXDS->nfsa, couple), new_lvno);
	    }
	}

	break;
    default: /* pour faire taire gcc -Switch-default */
#if EBUG
    sxtrap("sxdynam_scanner","unknown switch case #5");
#endif
      break;
    }
}


static void
comments_put (str, lgth)
    char	*str;
    int		lgth;
{
    int	lgth_comment;

    if (SXDS->terminal_token.comment == NULL)
	strcpy (SXDS->terminal_token.comment = (char*) sxalloc (lgth + 1, sizeof (char)), str);
    else {
	lgth_comment = strlen (SXDS->terminal_token.comment);
	strcpy ((SXDS->terminal_token.comment =
		 (char*) sxrealloc (SXDS->terminal_token.comment, lgth_comment + lgth + 1,
				    sizeof (char))) + lgth_comment,
		str);
    }
}


static void
process_shift (pconfigs)
    struct configs	*pconfigs;
{
    int			new_lvno;
    struct config	*pconfig;
    struct config_elem	*pelem;
    struct item_to_attr	*pattr;

    /* Traitement du is_erased */
    sxba_empty (SXDS->lvno_set);

    ForeachConfig (pconfigs, pconfig)
    {
	pelem = &(pconfig->prev);
	pattr = SXDS->item_to_attr + pelem->state;

	/* Les "-" sont traites comme des actions ! */
	
	if (SXBA_bit_is_reset_set (SXDS->lvno_set, pelem->lvno))
	{
	    new_lvno = pelem->lvno;
	    SXDS->lvno_clones [pelem->lvno] = pattr->is_erased ? -(new_lvno + 1) : (new_lvno + 1);

	    if (!pattr->is_erased)
		ts_put (pelem->lvno, SXDS->current_char);
	}
	else
	{
	    new_lvno = SXDS->lvno_clones [pelem->lvno];

	    if (new_lvno == pelem->lvno + 1)
	    {
		if (pattr->is_erased)
		{
		    new_lvno = sxds_clone (pelem->lvno, pelem->state, TRUE);
		    /* Clone apres "ts_put", on enleve le caractere. */
		    SXDS->dslv [new_lvno].ts_lgth--;
		    SXDS->lvno_clones [pelem->lvno] = -(new_lvno + 1);
		}
		else
		    new_lvno = pelem->lvno;
	    }
	    else if (new_lvno == -(pelem->lvno + 1))
	    {
		if (!pattr->is_erased)
		{
		    new_lvno = sxds_clone (pelem->lvno, pelem->state, TRUE);
		    ts_put (new_lvno, SXDS->current_char);
		    SXDS->lvno_clones [pelem->lvno] = new_lvno + 1;
		}
		else
		    new_lvno = pelem->lvno;
	    }
	    else
	    {
		/* Le 17/03/2003, merci Mr alpha
		new_lvno = new_lvno > 0 ? (pattr->is_erased ? pelem->lvno : --new_lvno)
		    : (pattr->is_erased ? -(new_lvno - 1) : pelem->lvno);
		    */
		new_lvno = new_lvno > 0 ? (pattr->is_erased ? pelem->lvno : (new_lvno-1))
		    : (pattr->is_erased ? -(new_lvno - 1) : pelem->lvno);
	    }
	}

	pconfig->current.lvno = new_lvno;
    }
}


static struct config_elem*
sxds_read_a_tok ()
{
    int				re, state, couple, init_item, lvno, new_lvno;
    int				current_char_no;
    struct configs		*swap_configs;
    struct config_elem		*pelem;
    struct config		*pconfig;
    struct item_to_attr		*pattr;
    BOOLEAN			first_time, is_conflict, is_la_conflict;
    char			priority_kind, pk, conflict_kind, la_conflict_kind;
    SHORT			current_char, previous_char;

    SXDS->lv_top = 0;
    SXDS->dslv [0].ts_lgth = 0;
    SXDS->dslv [0].mark.index = -1 /* no explicit Mark yet */ ;
    SXDS->dslv [0].prev_lvno = -1;
    SXDS->dslv [0].char_no = SXDS->current_char_no = 1;
    SXDS->dslv [0].state = 0; /* ?? */

    SXDS->terminal_token.source_index = sxsrcmngr.source_coord;

    if (SXDS->terminal_token.lahead > 0)
	SXDS->terminal_token.comment = NULL;

    /* On calcule la valeur initiale de ctxt_set */
    sxba_and (SXDS->ctxt_set, SXDS->filter);

    SXDS->target_configs = &(SXDS->configs [SXDS->target = 1]);
    SXDS->target_configs->top = 0;

    re = -1;
    while ((re = sxba_scan (SXDS->ctxt_set, re)) >= 0)
    {
	init_item = SXDS->reg_exp_to_attr [re].init_item;

	XxY_Xforeach (SXDS->nfsa, init_item, couple)
	{
	    state = XxY_Y (SXDS->nfsa, couple);
	    test_0la (SXDS->target_configs, state, 0);
	}
    }

    for (;;)
    {
	SXDS->source_configs = SXDS->target_configs;
	SXDS->target_configs = &(SXDS->configs [SXDS->target ^= 1]);
	SXDS->target_configs->top = 0;

	conflict_kind = GetConflictKind (SXDS->source_configs);
	pconfig = SXDS->source_configs->last;

	if (conflict_kind & FirstReduce  /* Y-a du Reduce */ &&
	    (conflict_kind & Shift /* Y-a du Shift */ ||
	     conflict_kind & NextReduce/* ou du Reduce */))
	{
	    /* Conflit Shift/Reduce ou Reduce/Reduce,
	       on applique les priorites de l'utilisateur (s'il y en a). */
	    priority_kind = SXDS->source_configs->top == 0
		? 0 : SXDS->source_configs->priority_kind;

	    if (priority_kind != 0 /* Y-a des priorites utilisateur */)
	    {
		if ((priority_kind & ReduceReduce) &&
		    (conflict_kind & FirstReduce) &&
		    (conflict_kind & NextReduce))
		{
		    /* Conflit Reduce/Reduce */
		    /* On supprime les reduces qui n'ont pas Reduce>Reduce. */
		    priority_kind = 0;

		    ForeachConfig (SXDS->source_configs, pconfig)
		    {
			pattr = SXDS->item_to_attr + pconfig->init.state;

			if (pattr->kind == KREDUCE)
			{
			    pk = SXDS->reg_exp_to_attr [pattr->reg_exp_no].priority_kind;

			    if (pk & ReduceReduce)
				priority_kind |= pk;
			    else
				EraseConfig (pconfig);
			}
		    }
		}

		/* Ici il y a toujours des reduces */
		if ((conflict_kind & Shift) && (priority_kind & ReduceShift))
		{
		    /* On supprime les shifts (apres les actions). */
		    ForeachConfig (SXDS->source_configs, pconfig)
		    {
			pattr = SXDS->item_to_attr + pconfig->init.state;

			if (pattr->kind != KREDUCE)
			    EraseConfig (pconfig);
		    }

		    conflict_kind &= ~Shift;
		}

		if ((conflict_kind & Shift) && (priority_kind & ShiftReduce))
		{
		    /* On supprime les reduces ayant Priority Shift > Reduce. */
		    ForeachConfig (SXDS->source_configs, pconfig)
		    {
			pattr = SXDS->item_to_attr + pconfig->init.state;

			if (pattr->kind == KREDUCE &&
			    SXDS->reg_exp_to_attr [pattr->reg_exp_no].priority_kind & ShiftReduce)
			    EraseConfig (pconfig);
		    }
		}
	
		SetConflict (SXDS->source_configs, FALSE /* sans test d'ambiguite' */);
		conflict_kind = GetConflictKind (SXDS->source_configs);
		pconfig = SXDS->source_configs->last;

		is_conflict = (conflict_kind & FirstReduce  /* Y-a du Reduce */ &&
			       (conflict_kind & Shift /* Y-a du Shift */ ||
				conflict_kind & NextReduce /* ou du Reduce */));
	    }
	    else
		is_conflict = TRUE;

	    if (is_conflict)
	    {
		/* Conflit "0la" non resolu par les priorites de l'utilisateur. */
		/* On essaie avec un look-ahead de un caractere. */
		sxba_empty (SXDS->lvno_set);

		ForeachConfig (SXDS->source_configs, pconfig)
		{
		    pattr = SXDS->item_to_attr + pconfig->init.state;

		    if (pattr->kind == KREDUCE)
		    {
			if (SXBA_bit_is_reset_set (SXDS->lvno_set, pconfig->init.lvno))
			{
			    SXDS->lvno_clones [pconfig->init.lvno] =
				new_lvno = sxds_clone (pconfig->init.lvno,
						       pconfig->init.state,
						       FALSE);
			}
			else
			    new_lvno = SXDS->lvno_clones [pconfig->init.lvno];

			state = pconfig->init.state;
			lvno = pconfig->init.lvno;
			EraseConfig (pconfig);
			test_1la (SXDS->source_configs,
				  state, lvno,
				  state, new_lvno);
		    }
		    /* else
		           On ne touche pas aux shifts */
		}

		SetConflict (SXDS->source_configs, FALSE /* sans test d'ambiguite' */);

		conflict_kind = GetConflictKind (SXDS->source_configs);
		pconfig = SXDS->source_configs->last;

		if (conflict_kind & FirstReduce  /* Y-a du Reduce */ &&
		    (conflict_kind & Shift /* Y-a du Shift */ ||
		     conflict_kind & NextReduce /* ou du Reduce */))
		{
		    /* Conflit non resolu par un seul caractere de pre'-vision. */
		    /* On prend en compte les "unbounded". */
		    ForeachConfig (SXDS->source_configs, pconfig)
		    {
			pattr = SXDS->item_to_attr + pconfig->init.state;

			if (pattr->kind != KREDUCE /* Shift */ ||
			    SXDS->reg_exp_to_attr [pattr->reg_exp_no].context_kind >=
			    UnboundedContext /* Unbounded : on conserve */)
			{
			    Push (SXDS->target_configs,
				  pconfig->init.state, pconfig->init.lvno,
				  pconfig->prev.state, pconfig->prev.lvno,
				  pconfig->current.state, pconfig->current.lvno);
			}
		    }

		    first_time = TRUE;
		    previous_char = SXDS->previous_char;
		    current_char = SXDS->current_char;
		    current_char_no = SXDS->current_char_no;
		    SXDS->la_configs = &(SXDS->configs [2]);

		    for (;;)
		    {
			SXDS->la_configs->top = 0;
			
			la_conflict_kind = GetConflictKind (SXDS->target_configs);
			pconfig = SXDS->target_configs->last;
			is_la_conflict = (la_conflict_kind & FirstReduce  /* Y-a du Reduce */ &&
					  (la_conflict_kind & Shift /* Y-a du Shift */ ||
					   la_conflict_kind & NextReduce /* ou du Reduce */));

			if (la_conflict_kind == 0 ||
			    (la_conflict_kind & Ambiguous) ||
			    (is_la_conflict && (SXDS->current_char == EOF)))
			{
			    /* Ambiguity ou erreur en look_ahead */
			    if (la_conflict_kind != 0 && SXDS->desambig != NULL)
			    {
				(*SXDS->desambig) (SXDS->source_configs, SXDS->target_configs);
				SetConflict (SXDS->source_configs, FALSE);
				conflict_kind = GetConflictKind (SXDS->source_configs);
				pconfig = SXDS->target_configs->last;
			    }

			    is_conflict = (conflict_kind & FirstReduce /* Y-a du Reduce */ &&
					   (conflict_kind & Shift /* Y-a du Shift */ ||
					    conflict_kind & NextReduce /* ou du Reduce */));

			    if (is_conflict)
			    {
				/* "(*SXDS->desambig) (SXDS->target_configs)" n'a pas
				   supprime les conflits, on le fait:
				   - priorite au shift
				   - ou un reduce qcq. */
				if (!SXDS->mode.is_silent)
				    sxput_error (SXDS->terminal_token.source_index,
						 "%sthis token seems ambiguous.",
						 err_titles [2]);
			    }
			    
			    break;
			}

			if (is_la_conflict)
			{
			    if (first_time && (la_conflict_kind & Shift))
			    {
				/* Il faut cloner les "shifts", pour avoir ds "source_configs"
				   un resultat correct si la resolution se fait en faveur
				   de shift. */
				sxba_empty (SXDS->lvno_set);

				ForeachConfig (SXDS->target_configs, pconfig)
				{
				    pattr = SXDS->item_to_attr + pconfig->init.state;

				    if (pattr->kind != KREDUCE)
				    {
					if (SXBA_bit_is_reset_set (SXDS->lvno_set,
								   pconfig->prev.lvno))
					    SXDS->lvno_clones [pconfig->prev.lvno] =
						new_lvno = sxds_clone (pconfig->prev.lvno,
								       pconfig->prev.state,
								       TRUE);
					else
					    new_lvno = SXDS->lvno_clones [pconfig->prev.lvno];

					pconfig->prev.lvno = new_lvno;
					pconfig->current.lvno = new_lvno;
				    } 
				}
			    }

			    process_shift (SXDS->target_configs);
			    SXDS->mode.mode = SXLA_SCAN;
			    SXDS->current_char_no++;
			    SXDS->previous_char = SXDS->current_char;

			    SXDS->current_char = first_time ?
				(first_time = FALSE, sxlafirst_char ()) :
				    sxlanext_char ();

			    ForeachConfig (SXDS->target_configs, pconfig)
			    {
				test_unbounded (SXDS->la_configs,
						pconfig->init.state, pconfig->init.lvno,
						pconfig->current.state, pconfig->current.lvno);
			    }

			    swap_configs = SXDS->la_configs;
			    SXDS->la_configs = SXDS->target_configs;
			    SXDS->target_configs = swap_configs;
			}
			else
			{
			    conflict_kind = la_conflict_kind;
			    break;
			}
		    }

		    if (conflict_kind & Shift)
		    {
			/* On enleve les "reduce" de "source" */
			ForeachConfig (SXDS->source_configs, pconfig)
			{
			    pattr = SXDS->item_to_attr + pconfig->init.state;

			    if (pattr->kind == KREDUCE)
				EraseConfig (pconfig);
			}

			conflict_kind = Shift;
		    }
		    else
		    {
			/* FirstReduce */
			ForeachConfig (SXDS->source_configs, pconfig)
			{
			    pattr = SXDS->item_to_attr + pconfig->init.state;

			    if (pattr->kind == KREDUCE)
				break;
			}

			conflict_kind = FirstReduce;
		    }

		    SXDS->previous_char = previous_char;
		    SXDS->current_char = current_char;
		    SXDS->current_char_no = current_char_no;
		    SXDS->target_configs = &(SXDS->configs [SXDS->target]);
		    SXDS->mode.mode = SXNORMAL_SCAN;
		}
	    }
	}

	if (conflict_kind == 0)
	{
	    /* Empty => Lexical error */
	    /* Not yet implemented */
	    if (!SXDS->mode.is_silent)
		sxput_error (sxsrcmngr.source_coord,
			     "%slexical error.",
			     err_titles [2]);

	    /* Lecture du caractere suivant */
	    read_a_char ();

	    return NULL;
	}

	/* Ici, un choix unique a ete realise dans source_configs. */
	if (conflict_kind & FirstReduce)
	{
	    /* Reduce Unique */
	    break;
	}

	/* Shift */
	process_shift (SXDS->source_configs);

	/* Lecture du caractere suivant */
	SXDS->current_char_no++;
	read_a_char ();

	ForeachConfig (SXDS->source_configs, pconfig)
	{
	    pelem = &pconfig->current;
	    state = pelem->state;

	    if (SXDS->item_to_attr [state].kind == KPREDICATE)
	    {
		XxY_Xforeach (SXDS->nfsa, state, couple)
		    test_0la (SXDS->target_configs, XxY_Y (SXDS->nfsa, couple), pelem->lvno);
	    }
	    else
		test_0la (SXDS->target_configs, state, pelem->lvno);
	}
    }

    return &pconfig->init;
}




void
sxds_scanit ()
{
    /* Point d'entree avec traitement de la reconnaissance du token. */
    int				state, lvno, kw, re;
    struct config_elem		*pelem;
    struct reg_exp_to_attr	*pattr;
    struct sxdslv		*plv;

    if ((pelem = sxds_read_a_tok ()) != NULL)
    {
	state = pelem->state;
	lvno = pelem->lvno;

	re = SXDS->item_to_attr [state].reg_exp_no;
	pattr = SXDS->reg_exp_to_attr + re;
	plv = SXDS->dslv + lvno;
	ts_null (lvno);

	SXDS->terminal_token.lahead = pattr->token_no;

	if ((pattr->kw_kind & KW_YES) &&
	    (kw = sxword_2retrieve (&SXDS->terminals,
				    plv->token_string,
				    (unsigned long) plv->ts_lgth)) != ERROR_STE &&
	    SXDS->terminal_to_attr [kw].is_kw)
	{
	    SXDS->terminal_token.lahead = SXDS->terminal_to_attr [kw].token_no;
	    SXDS->terminal_token.string_table_entry = EMPTY_STE;
	}
	else if (plv->ts_lgth == 0)
	    SXDS->terminal_token.string_table_entry = EMPTY_STE;
	else if (SXDS->terminal_token.lahead > 0)
	    SXDS->terminal_token.string_table_entry = sxstr2save (plv->token_string, plv->ts_lgth);
	else if (SXDS->terminal_token.lahead == 0)
	    comments_put (plv->token_string, plv->ts_lgth);
	
	fill_ctxt_set (re);

	if (lvno != 0)
	    sxds_lvcpy (0, lvno, TRUE);

	if (pattr->post_action >= 0 && SXDS->scanact != NULL && SXDS->mode.with_user_act)
	    (VOID) (*SXDS->scanact) (ACTION, 0, pattr->post_action);
    }
}



void
sxds_putit ()
{
    /* Point d'entree avec appel de "sxput_token". */
    sxds_scanit ();

    if (SXDS->terminal_token.lahead > 0)
	sxput_token (SXDS->terminal_token);
}


int
sxdynam_scanner (what_to_do)
    int	what_to_do;
{
    int i;
    
    switch (what_to_do) {
    case OPEN:
	/* the global variable "SXDS" must have been saved in case of */
	/* recursive invocation */
	
	SXDS->dslv = (struct sxdslv*) sxcalloc ((unsigned long int) (SXDS->lv_size = 32) /* pourquoi pas */,
					       sizeof (struct sxdslv));

	SXDS->lvno_set = sxba_calloc (SXDS->lv_size);
	SXDS->lvno_clones = (int*) sxalloc (SXDS->lv_size, sizeof (int));

	SXDS->dslv [0].token_string = (char*) sxalloc (SXDS->dslv [0].ts_size = 120,
						       sizeof (char));

	if (SXDS->counters_size >= 0)
	    SXDS->dslv [0].counters =
		(long*) sxcalloc ((unsigned long int)SXDS->counters_size + 1, sizeof (long));

	SXDS->configs [0].t = (struct config*) sxalloc (SXDS->configs [0].size = 50,
							sizeof (struct config));
	SXDS->configs [1].t = (struct config*) sxalloc (SXDS->configs [1].size = 50,
							sizeof (struct config));
	SXDS->configs [2].t = (struct config*) sxalloc (SXDS->configs [2].size = 50,
							sizeof (struct config));

	SXDS->mode.errors_nb = 0;
	SXDS->mode.mode = SXNORMAL_SCAN;
	SXDS->mode.is_silent = FALSE;
	SXDS->mode.with_system_act = TRUE;
	SXDS->mode.with_user_act = TRUE;
	SXDS->mode.with_system_prdct = TRUE;
	SXDS->mode.with_user_prdct = TRUE;
	
	SXDS->terminal_token.lahead = 0;
	SXDS->terminal_token.comment = NULL;

	if (SXDS->recovery != NULL)
	    (*SXDS->recovery) (OPEN);

	break;
	
    case INIT:
	/* Beginning of new source */
	/* read the character that will be current next scan */
	SXDS->current_char = NEWLINE;
	read_a_char ();
	sxba_copy (SXDS->ctxt_set, SXDS->filter);

	break;
	
    case ACTION:
	sxds_putit ();
	break;
	
    case FINAL:
	/* End of source */
	break;

    case CLOSE:
	/* End of language */
	for (i = 0; i < SXDS->lv_size; i++)
	    if (SXDS->dslv [i].token_string)
		sxfree (SXDS->dslv [i].token_string);

	if (SXDS->counters_size >= 0)
	    for (i = 0; i < SXDS->lv_size; i++)
		if (SXDS->dslv [i].counters)
		    sxfree (SXDS->dslv [i].counters);

	sxfree (SXDS->configs [0].t);
	sxfree (SXDS->configs [1].t);
	sxfree (SXDS->configs [2].t);

	sxfree (SXDS->dslv);
	sxfree (SXDS->lvno_set);
	sxfree (SXDS->lvno_clones);

	if (SXDS->recovery != NULL)
	    (*SXDS->recovery) (CLOSE);

	break;

    default:
	fprintf (sxstderr, "The function \"sxdynam_scanner\" is out of date with respect to its specification.\n");
	abort ();
    }

    return 0;
}

struct configs*
sxds_string_to_token (string, is_set)
    char *string;
    BOOLEAN	is_set;
{
    /* On suppose que le scanner a deja ete initialise' par ailleurs
       Si "is_set", SXDS->ctxt_set a deja ete positionne'. */
    int				re, state, couple, init_item;
    struct config_elem		*pelem;
    struct config		*pconfig;
    struct item_to_attr		*pattr;
    char			conflict_kind;

    sxsrc_mngr (INIT, NULL, string);

    SXDS->current_char = NEWLINE;
    read_a_char ();

    if (!is_set)
	sxba_copy (SXDS->ctxt_set, SXDS->filter);

    /* Analyse du nom du token. */

    SXDS->lv_top = 0;
    SXDS->dslv [0].ts_lgth = 0;
    SXDS->dslv [0].mark.index = -1 /* no explicit Mark yet */ ;
    SXDS->dslv [0].prev_lvno = -1;
    SXDS->dslv [0].char_no = SXDS->current_char_no = 1;
    SXDS->dslv [0].state = 0; /* ?? */

    SXDS->terminal_token.source_index = sxsrcmngr.source_coord;

    if (SXDS->terminal_token.lahead > 0)
	SXDS->terminal_token.comment = NULL;

    /* On calcule la valeur initiale de ctxt_set */
    sxba_and (SXDS->ctxt_set, SXDS->filter);

    SXDS->target_configs = &(SXDS->configs [SXDS->target = 1]);
    SXDS->target_configs->top = 0;

    re = -1;
    while ((re = sxba_scan (SXDS->ctxt_set, re)) >= 0)
    {
	init_item = SXDS->reg_exp_to_attr [re].init_item;

	XxY_Xforeach (SXDS->nfsa, init_item, couple)
	{
	    state = XxY_Y (SXDS->nfsa, couple);
	    test_0la (SXDS->target_configs, state, 0);
	}
    }

    do
    {
	SXDS->source_configs = SXDS->target_configs;
	SXDS->target_configs = &(SXDS->configs [SXDS->target ^= 1]);
	SXDS->target_configs->top = 0;

	conflict_kind = GetConflictKind (SXDS->source_configs);

	if (conflict_kind & FirstReduce  /* Y-a du Reduce */)
	{
	    /* On les supprime */
	    ForeachConfig (SXDS->source_configs, pconfig)
	    {	
		pattr = SXDS->item_to_attr + pconfig->init.state;

		if (pattr->kind == KREDUCE)
		    EraseConfig (pconfig);
	    }
	}
	
	if (conflict_kind & Shift)
	{
	    process_shift (SXDS->source_configs);

	    /* Lecture du caractere suivant */
	    SXDS->current_char_no++;
	    read_a_char ();

	    ForeachConfig (SXDS->source_configs, pconfig)
	    {
		pelem = &pconfig->current;
		state = pelem->state;

		if (SXDS->item_to_attr [state].kind == KPREDICATE)
		{
		    XxY_Xforeach (SXDS->nfsa, state, couple)
			test_0la (SXDS->target_configs, XxY_Y (SXDS->nfsa, couple), pelem->lvno);
		}
		else
		    test_0la (SXDS->target_configs, state, pelem->lvno);
	    }
	}
	else
	    break;
    } while (SXDS->current_char != EOF_char);

    sxsrc_mngr (FINAL);

    return SXDS->target_configs;
}
