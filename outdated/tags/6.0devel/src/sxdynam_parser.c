/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/

static char	ME [] = "DYNAM_PARSER";

#define SX_GLOBAL_VAR_SXDG 

#include "sxversion.h"
#include "sxcommon.h"
# include "sxdynam_parser.h"
#include <memory.h>
/* Pour open () Vasy #056 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char WHAT_SXDYNAM_PARSER[] = "@(#)SYNTAX - $Id: sxdynam_parser.c 2270 2020-03-09 18:48:41Z garavel $" WHAT_DEBUG;


/* 
  Parser dont l'automate LR (0) est construit incrementalement (au fur et a mesure de
  l'analyse du texte source). De plus la grammaire peut elle-meme evoluer incrementalement
  i.e. des action syntaxiques peuvent ajouter ou supprimer des productions.
*/

/*
  SXDG.P :		rule		= (lhs, rhs)

  SXDG.rhs :		rhs		= 0
                        rhs		= (symbol, rhs)

  SXDG.items :		item		= (rule, rhs)

  SXDG.transitions :	transition	= (state, symbol)

  SXDG.item_occurs :	item_occur	= (transition, item)

  SXDG.transitionxnext_state :	
                        index		= (transition, state)

  SXDG.states :		{0 ou -1, item1, item2, ... | item1 < item2}
                        implante' par des sxlist

  SXDG.parse_stacks :	pstack		= 0
                        pstack		= (pstack, state, ptok_no)

*/


#define parse_stack_oflw()						\
    sxpglobals.parse_stack =						\
	(struct sxparstack*) sxrealloc (sxpglobals.parse_stack,		\
					(SXDG.lgt1 *= 2) + 1,		\
					sizeof (struct sxparstack))



static struct pstack_to_attr empty_attr;



static  SXVOID
P_oflw (SXINT old_size, SXINT new_size)
{
  SXINT *new_ptr;
  SXBA new_sxba;

  if (XxY_is_static (SXDG.P))
    {
      memcpy (new_ptr = sxalloc (new_size + 1, sizeof (SXINT)),
	      SXDG.rule_to_rhs_lgth,
	      sizeof (SXINT) * (old_size + 1));
      SXDG.rule_to_rhs_lgth = (SXINT*) new_ptr;
	
      memcpy (new_ptr = sxalloc (new_size + 1, sizeof (SXINT)),
	      SXDG.rule_to_first_item,
	      sizeof (SXINT) * (old_size + 1));
      SXDG.rule_to_first_item = (SXINT*) new_ptr;
	
      memcpy (new_ptr = sxalloc (new_size + 1, sizeof (SXINT)),
	      SXDG.rule_to_action,
	      sizeof (SXINT) * (old_size + 1));
      SXDG.rule_to_action = (SXINT*) new_ptr;
	
      sxba_copy (new_sxba = sxba_calloc (old_size + 1), SXDG.active_rule_set);
      SXDG.active_rule_set = sxba_resize (new_sxba, new_size + 1);
	
      if (SXDG.r_priorities != NULL)
	{
	  memcpy (new_ptr = sxalloc (new_size + 1, sizeof (struct priority)),
		  SXDG.r_priorities,
		  sizeof (struct priority) * (old_size + 1));
	  SXDG.r_priorities = (struct priority*) new_ptr;
	}
    }
  else
    {
      SXDG.rule_to_rhs_lgth = (SXINT*) sxrealloc (SXDG.rule_to_rhs_lgth, new_size + 1, sizeof (SXINT));
      SXDG.rule_to_first_item = (SXINT*) sxrealloc (SXDG.rule_to_first_item, new_size + 1, sizeof (SXINT));
      SXDG.rule_to_action = (SXINT*) sxrealloc (SXDG.rule_to_action, new_size + 1, sizeof (SXINT));
      SXDG.active_rule_set = sxba_resize (SXDG.active_rule_set, new_size + 1);
	
      if (SXDG.r_priorities != NULL)
	SXDG.r_priorities = (struct priority*) sxrealloc (SXDG.r_priorities,
							  new_size + 1,
							  sizeof (struct priority));
    }
}


static  SXVOID
items_oflw (SXINT old_size, SXINT new_size)
{
  SXINT *new_ptr;
  SXBA new_sxba;

  if (XxY_is_static (SXDG.items))
    {
      memcpy (new_ptr = sxalloc (new_size + 1, sizeof (SXINT)),
	       SXDG.item_to_next_item,
	       sizeof (SXINT) * (old_size + 1));
      SXDG.item_to_next_item = (SXINT*) new_ptr;

      sxba_copy (new_sxba = sxba_calloc (old_size + 1), SXDG.item_set);
      SXDG.item_set = sxba_resize (new_sxba, new_size + 1);
    }
  else
    {
      SXDG.item_to_next_item = (SXINT*) sxrealloc (SXDG.item_to_next_item,
						 new_size + 1,
						 sizeof (SXINT));
      SXDG.item_set = sxba_resize (SXDG.item_set, new_size + 1);
    }
}


static SXINT
states_oflw (SXINT old_size, SXINT new_size)
{
  SXINT *new_ptr;
  SXBA new_sxba;

   if (sxlist_is_static (SXDG.states))
    {
      memcpy (new_ptr = sxalloc (new_size + 1, sizeof (SXINT)),
	       SXDG.state_to_action,
	       sizeof (SXINT) * (old_size + 1));
      SXDG.state_to_action = (SXINT*) new_ptr;

      memcpy (new_ptr = sxalloc (new_size + 1, sizeof (SXINT)),
	       SXDG.state_to_in_symbol,
	       sizeof (SXINT) * (old_size + 1));
      SXDG.state_to_in_symbol = (SXINT*) new_ptr;

      memcpy (new_ptr = sxalloc (new_size + 1, sizeof (SXINT)),
	       SXDG.state_to_used_lnk,
	       sizeof (SXINT) * (old_size + 1));
      SXDG.state_to_used_lnk = (SXINT*) new_ptr;

      sxba_copy (new_sxba = sxba_calloc (old_size + 1), SXDG.is_state_linked);
      SXDG.is_state_linked = sxba_resize (new_sxba, new_size + 1);
    }
  else
    {
      SXDG.state_to_action = (SXINT*) sxrealloc (SXDG.state_to_action, new_size + 1, sizeof (SXINT));
      SXDG.state_to_in_symbol = (SXINT*) sxrealloc (SXDG.state_to_in_symbol,
						  new_size + 1, sizeof (SXINT));
      SXDG.state_to_used_lnk = (SXINT*) sxrealloc (SXDG.state_to_used_lnk,
						 new_size + 1, sizeof (SXINT));
      SXDG.is_state_linked = sxba_resize (SXDG.is_state_linked, new_size + 1);
    }
   return 0;
}

static  SXVOID
xt_oflw (SXINT old_size, SXINT new_size)
{
  struct priority *new_ptr;

  if (SXDG.t_priorities != NULL)
  {
    if (XxY_is_static (SXDG.xt))
      {
	memcpy (new_ptr = sxalloc (new_size + 1, sizeof (struct priority)),
		 SXDG.t_priorities,
		 sizeof (struct priority) * (old_size + 1));
	SXDG.t_priorities = new_ptr;
      }
    else
      {						       
	SXDG.t_priorities = (struct priority*) sxrealloc (SXDG.t_priorities,
							  new_size + 1,
							  sizeof (struct priority));
      }
  }
}

static  SXVOID
transitions_oflw (SXINT old_size, SXINT new_size)
{

  sxuse(old_size); /* pour faire taire gcc -Wunused */
  if (XxY_is_static (SXDG.transitions))
    {
      SXDG.trans_to_next_state = (SXINT*) sxalloc (new_size + 1, sizeof (SXINT));
      SXDG.trans_has_prdct = sxba_calloc (new_size + 1);
    }
  else
    {
      SXDG.trans_to_next_state = (SXINT*) sxrealloc (SXDG.trans_to_next_state,
						   new_size + 1, sizeof (SXINT));
      SXDG.trans_has_prdct = sxba_resize (SXDG.trans_has_prdct, new_size + 1);
    }
}



static  SXVOID
parse_stacks_oflw (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    SXDG.pstack_to_attr = (struct pstack_to_attr*) sxrealloc (SXDG.pstack_to_attr,
							   new_size + 1,
							   sizeof (struct pstack_to_attr));
}


static SXVOID
update_automaton (SXINT rule)
{
    SXINT lhs, transition, state, couple;

    lhs = XxY_X (SXDG.P, rule);

    XxY_Yforeach (SXDG.transitions, lhs, transition)
    {
	state = XxY_X (SXDG.transitions, transition); /* Toujours > 0 */
	
	if (SXBA_bit_is_set_reset (SXDG.is_state_linked, state))
	{
	    /* cut_in_transitions */
	    SXDG.has_been_updated = SXTRUE;

	    XxY_Yforeach (SXDG.transitionxnext_state, state, couple)
	    {
		SXDG.trans_to_next_state [XxY_X (SXDG.transitionxnext_state, couple)] = 0;
		XxY_erase (SXDG.transitionxnext_state, couple);
	    }
	}
    }
}

SXINT
sxdp_rule_is_set (SXINT rhs_lgth, SXINT *symbols)
{
    SXINT rhs, i;

    rhs = 0;

    for (i = rhs_lgth; i > 0; i--)
	XxY_set (&SXDG.rhs, symbols [i], rhs, &rhs);

    return XxY_is_set (&SXDG.P, symbols [0], rhs);
}


SXBOOLEAN
sxdp_rule_is_activated (SXINT rule)
{
   return SXBA_bit_is_set (SXDG.active_rule_set, rule) ;
}


SXBOOLEAN
sxdp_rule_set (SXINT rhs_lgth, SXINT *symbols, SXINT *rule, SXINT action_nb, SXINT prio, SXINT assoc)
{
    /* retourne SXTRUE ssi la regle est nouvelle. */
    SXINT	rhs, i, item, next_item;
    SXBOOLEAN	is_new;

    rhs = 0;

    for (i = rhs_lgth; i > 0; i--)
	XxY_set (&SXDG.rhs, symbols [i], rhs, &rhs);

    if ((is_new = !XxY_set (&SXDG.P, symbols [0], rhs, rule)))
    {
	/* On entre la nouvelle regle ds la grammaire. */
	SXDG.rule_to_rhs_lgth [*rule] = rhs_lgth;
	XxY_set (&SXDG.items, *rule, rhs, &item);
	SXDG.rule_to_first_item [*rule] = item;

	while (rhs != 0)
	{
	    rhs = XxY_Y (SXDG.rhs, rhs);
	    XxY_set (&SXDG.items, *rule, rhs, &next_item);
	    SXDG.item_to_next_item [item] = next_item;
	    item = next_item;
	}

	SXDG.item_to_next_item [item] = 0;
	SXDG.rule_to_action [*rule] = action_nb;

	if (SXDG.r_priorities != NULL)
	{
	    SXDG.r_priorities [*rule].value = prio;
	    SXDG.r_priorities [*rule].assoc = assoc;
	}

	SXBA_0_bit (SXDG.active_rule_set, *rule); /* pas prise en compte ds l'automate */
    }

    return is_new;
}


SXVOID
sxdp_rule_get_attr (SXINT rule, SXINT *action_nb, SXINT *prio, SXINT *assoc)
{
    *action_nb = SXDG.rule_to_action [rule];
    *prio = SXDG.r_priorities == NULL ? 0 : SXDG.r_priorities [rule].value;
    *assoc = SXDG.r_priorities == NULL ? 0 : SXDG.r_priorities [rule].assoc;
}


SXVOID
sxdp_rule_set_attr (SXINT rule, SXINT action_nb, SXINT prio, SXINT assoc)
{
    SXDG.rule_to_action [rule] = action_nb;

    if (SXDG.r_priorities != NULL)
    {
	SXDG.r_priorities [rule].value = prio;
	SXDG.r_priorities [rule].assoc = assoc;
    }
}


SXVOID
sxdp_rule_activate (SXINT rule)
{
    if (SXBA_bit_is_reset_set (SXDG.active_rule_set, rule))
	update_automaton (rule);
}


SXVOID
sxdp_rule_deactivate (SXINT rule)
{
    if (SXBA_bit_is_set_reset (SXDG.active_rule_set, rule))
	update_automaton (rule);
}


static SXVOID
GC_states (void)
{
    /* "Recupere" les etats de l'automate */
    /* Un etat est "utile" ssi il est accessible depuis un etat qui
       figure dans une parse_stack. */
    SXINT ps, state, next_state, transition, itoc, used_hd;

    used_hd = -1;

    /* On commence par noter tous les etats des parse_stack. */
    XxYxZ_foreach (SXDG.parse_stacks, ps)
    {
	state = XxYxZ_Y (SXDG.parse_stacks, ps);

	if (SXDG.state_to_used_lnk [state] == 0)
	{
	    SXDG.state_to_used_lnk [state] = used_hd;
	    used_hd = state;
	}
    }

    /* Puis tous les etats accessibles */
    for (state = used_hd; state > 0; state = SXDG.state_to_used_lnk [state])
    {
	XxY_Xforeach (SXDG.transitions, state, transition)
	{
	    if ((next_state = SXDG.trans_to_next_state [transition]) != 0)
	    {
		if (next_state < 0)
		    next_state = -next_state;

		if (SXDG.state_to_used_lnk [next_state] == 0)
		{
		    SXDG.state_to_used_lnk [next_state] = SXDG.state_to_used_lnk [state];
		    SXDG.state_to_used_lnk [state] = next_state;
		}
	    }
	}
    }

    sxlist_foreach_list (SXDG.states, state)
    {
	if (SXDG.state_to_used_lnk [state] != 0)
	    /* accessible */
	    SXDG.state_to_used_lnk [state] = 0;
	else
	{
	    /* Non accessible */
	    /* On "recupere" l'etat state. */
	    XxY_Xforeach (SXDG.transitions, state, transition)
	    {
		XxY_Xforeach (SXDG.item_occurs, transition, itoc)
		    XxY_erase (SXDG.item_occurs, itoc);

		SXDG.trans_to_next_state [transition] = 0;
		XxY_erase (SXDG.transitions, transition);
	    }

	    sxlist_erase (&SXDG.states, state);
	}
    }

    SXDG.has_been_erased = SXFALSE;
}

SXVOID
sxdp_rule_delete (SXINT rule)
{
    /* Suppression de rule : A -> alpha */
    SXINT		rhs, item, item2;

    item = SXDG.rule_to_first_item [rule];

    /* Supprime toutes les utilisations d'item */

    do {
	rhs = XxY_Y (SXDG.items, item);
	XxY_erase (SXDG.items, item);

	XxY_Yforeach (SXDG.items, rhs, item2)
	    break;

	if (item2 == 0)
	    /* rhs est inutilise' */
	    XxY_erase (SXDG.rhs, rhs);
    } while ((item = SXDG.item_to_next_item [item]) > 0);

    XxY_erase (SXDG.P, rule);

    sxdp_rule_deactivate (rule);
    SXDG.has_been_erased = SXTRUE;
}



static SXBOOLEAN
new_pstack (SXINT ops, SXINT state, SXINT ptok_no, SXINT *nps)
{
#if EBUG
    if (state <= 0)
	sxtrap (ME, "new_pstack");
#endif

    if (XxYxZ_set (&SXDG.parse_stacks, ops, state, ptok_no, nps))
	return SXFALSE;

    SXDG.pstack_to_attr [*nps] = empty_attr;
    return SXTRUE;
}

static SXBOOLEAN
closure (SXINT item)
{
    /* retourne SXTRUE ssi l'item existe deja.
       Ca signifie que cet item est initial et que les regles ayant cet lhs
       ont deja ete examinees ou sont en cours d'examen et qu'on peut donc
       arreter la branche courante. */
    SXINT rhs, symbol, rule;

    if (SXBA_bit_is_reset_set (SXDG.item_set, item))
    {
	if ((rhs = XxY_Y (SXDG.items, item)) > 0
	    && (symbol = XxY_X (SXDG.rhs, rhs)) > 0)
	{
	    XxY_Xforeach (SXDG.P, symbol, rule)
		if (SXBA_bit_is_set (SXDG.active_rule_set, rule)
		    && closure (SXDG.rule_to_first_item [rule]))
		    break;
	}

	return SXFALSE;
    }

    return SXTRUE;
}



SXBOOLEAN
sxdp_add_new_state (SXINT transition, SXINT *result)
{
    /* Construction de l'etat atteint par transition */
    /* Retourne SXTRUE ssi l'etat *result est nouveau */
    SXINT next_state, nucleus, item_occur, item, inter_trans, symbol, couple, prdct_no, x, scrmbl;
    SXBOOLEAN has_prdct, is_new_state;

    has_prdct = SXBA_bit_is_set (SXDG.trans_has_prdct, transition);

    XxY_Xforeach (SXDG.item_occurs, transition, item_occur)
    {
	/* Du fait que l'automate est unique alors que les grammaires evoluent, la
	   notion de nucleus caracteristique d'un etat n'est plus valable. Un etat
	   est donc l'ensemble de ses items. */
	item = XxY_Y (SXDG.item_occurs, item_occur);

	if (has_prdct)
	    SXBA_1_bit (SXDG.item_set, item);
	else
	    closure (SXDG.item_to_next_item [item]);
    }

    /* pour ne pas confondre les etats "intermediaires" avec les "vrais". */
    if (has_prdct)
        SS_push (SXDG.item_stack, scrmbl = -1);
    else
        scrmbl = 0;

    item = 0;

    while ((item = sxba_scan_reset (SXDG.item_set, item)) > 0)
    {
      scrmbl += item;
      SS_push (SXDG.item_stack, item);
    }

    is_new_state = sxlist_set (&SXDG.states, scrmbl, SXDG.item_stack, &nucleus);

    next_state = has_prdct ? -nucleus : nucleus;

    if (is_new_state)
    {
	/* Nouvel etat */
	SXDG.state_to_used_lnk [nucleus] = 0;
	SXDG.state_to_in_symbol [nucleus] = XxY_Y (SXDG.transitions, transition);

	sxlist_foreach_elem (SXDG.states, nucleus, x)
	{
	    if ((item = sxlist_elem (SXDG.states, x)) > 0)
	    {
		prdct_no = -1;

		if ((symbol = XxY_Y (SXDG.items, item)) != 0)
		{
		    /* pas reduce */
		    if ((symbol = XxY_X (SXDG.rhs, symbol)) < 0 && next_state > 0)
		    {
			/* transition sur le terminal de base */
			prdct_no =  XT_TO_PRDCT (SXDG, symbol);
			symbol = XT_TO_T (SXDG, symbol);
		    }
		}

		if (!XxY_set (&SXDG.transitions, next_state, symbol, &inter_trans))
		{
		    /* Nouvelle transition, elle est donc non calculee. */
		    SXDG.trans_to_next_state [inter_trans] = 0;

		    if (prdct_no != -1)
			SXBA_1_bit (SXDG.trans_has_prdct, inter_trans);
		    else
			SXBA_0_bit (SXDG.trans_has_prdct, inter_trans);
		}

		XxY_set (&SXDG.item_occurs, inter_trans, item, &item_occur);
	    }
	}
    }

    SXDG.trans_to_next_state [transition] = next_state;
    XxY_set (&SXDG.transitionxnext_state, transition, next_state, &couple);
    SXBA_1_bit (SXDG.is_state_linked, nucleus);

    *result = next_state;
    return is_new_state;
}


static SXBOOLEAN
check_viable_prefix (SXINT xps, SXINT *parse_stack)
{
    SXINT x, prev_state, state, next_state, xsymbol, symbol, transition, old_pstack, pstack;
    SXBOOLEAN stack_has_changed;
    
    /* On utilise "sxpglobals.parse_stack [x].state" comme structure temporaire
       pour deplier pstack. */

    x = xps;
    old_pstack = *parse_stack;

    do {
	sxpglobals.parse_stack [x--].state = old_pstack;
    } while ((old_pstack = XxYxZ_X (SXDG.parse_stacks, old_pstack)) > 0);

    pstack = 0;
    stack_has_changed = SXFALSE;

    /* On regarde si un etat de la pile a ete "isole'". */
    prev_state = SXDG.init_state;
    x = 1;

    while (++x <= xps)
    {
	old_pstack = sxpglobals.parse_stack [x].state;
	state = XxYxZ_Y (SXDG.parse_stacks, old_pstack);
	symbol = ((xsymbol = SXDG.state_to_in_symbol [state]) < 0)
	    ? XT_TO_T (SXDG, xsymbol) : xsymbol;

	if ((transition = XxY_is_set (&SXDG.transitions, prev_state, symbol)) == 0)
	    break;

	if ((next_state = SXDG.trans_to_next_state [transition]) == 0)
	    /* On le calcule */
	    sxdp_add_new_state (transition, &next_state);

	if (next_state < 0)
	{
	    if ((transition = XxY_is_set (&SXDG.transitions, next_state, xsymbol)) == 0)
		break;

	    if ((next_state = SXDG.trans_to_next_state [transition]) == 0)
		/* On le calcule */
		sxdp_add_new_state (transition, &next_state);
	}

	if (next_state != state)
	    stack_has_changed = SXTRUE;

	if (stack_has_changed)
	    new_pstack (pstack, next_state, XxYxZ_Z (SXDG.parse_stacks, old_pstack), &pstack);
	else
	    pstack = old_pstack;

	prev_state = next_state;
    }

    if (x <= xps)
    {
	if (!sxplocals.mode.is_silent)
	    sxerror (SXGET_TOKEN (sxplocals.ptok_no).source_index,
		     sxplocals.sxtables->err_titles [2][0],
		     "%sDamned, I guess you sawed off the branch you're ridiiiinnn...",
		     sxplocals.sxtables->err_titles [2]+1);

	sxtrap (ME, "check_viable_prefix");
    }

    *parse_stack = pstack;
    SXDG.has_been_updated = SXFALSE;

    return stack_has_changed;
}


static SXVOID
reactivate_path (SXINT xps, SXINT pstack)
{
    /* On est sur que les etats de pstack existent, on reactive les liens
       entre 2 etats consecutifs. */
    SXINT prev_state, state, inter_state, xsymbol, symbol, transition, couple;
    
    state = XxYxZ_Y (SXDG.parse_stacks, pstack);

    while ((pstack = XxYxZ_X (SXDG.parse_stacks, pstack)) != 0) {
	xps--;
	prev_state = XxYxZ_Y (SXDG.parse_stacks, pstack);

	if (SXBA_bit_is_reset_set (SXDG.is_state_linked, state))
	{
	    symbol = ((xsymbol = SXDG.state_to_in_symbol [state]) < 0) ? XT_TO_T (SXDG, xsymbol) : xsymbol;

	    if (symbol != xsymbol)
	    {
		/* Il y a des predicats */
		if ((transition = XxY_is_set (&SXDG.transitions, prev_state, symbol)) == 0
		    || (inter_state = SXDG.trans_to_next_state [transition]) == 0)
		    break;
	    }
	    else
		inter_state = prev_state;

	    if ((transition = XxY_is_set (&SXDG.transitions, inter_state, xsymbol)) == 0
		|| SXDG.trans_to_next_state [transition] != 0)
		break;
		
	    SXDG.trans_to_next_state [transition] = state;
	    XxY_set (&SXDG.transitionxnext_state, transition, state, &couple); 

	}
	/* else on le suppose lie' avec sa sous-pstack */

	state = prev_state;
    }

#if EBUG
    if (pstack != 0)
	sxtrap (ME, "reactivate_path");
#endif

    SXDG.has_been_updated = SXFALSE;
}


SXVOID
sxdp_init_state (void)
{
    SXINT init_item, item_occur;

    /* etat vide d'identifiant 0 */
    sxlist_set (&SXDG.states, 0, SXDG.item_stack, &SXDG.init_state);
    SXDG.state_to_in_symbol [SXDG.init_state] = 0;
    SXDG.state_to_used_lnk [SXDG.init_state] = 0;

    init_item = SXDG.rule_to_first_item [SXDG.super_rule];
    SS_push (SXDG.item_stack, 0); /* etat normal */
    SS_push (SXDG.item_stack, init_item);
    sxlist_set (&SXDG.states, init_item, SXDG.item_stack, &SXDG.init_state);
    SXDG.state_to_in_symbol [SXDG.init_state] = 0;
    SXDG.state_to_used_lnk [SXDG.init_state] = 0;
    XxY_set (&SXDG.transitions, SXDG.init_state, SXDG.eof_code, &SXDG.init_transition);
    SXBA_0_bit (SXDG.trans_has_prdct, SXDG.init_transition);
    SXDG.trans_to_next_state [SXDG.init_transition] = 0; /* non calcule'. */
    XxY_set (&SXDG.item_occurs, SXDG.init_transition, init_item, &item_occur);
}



static SXVOID
push_next_state (SXINT state, SXINT symbol)
{
    /* state et symbol sont negatifs */
    SXINT trans, next_state;

    if (symbol != 0 && (trans = XxY_is_set (&SXDG.transitions, state, symbol)) > 0)
    {
	if ((next_state = SXDG.trans_to_next_state [trans]) == 0)
	    sxdp_add_new_state (trans, &next_state);

	SS_push (SXDG.next_state_stack, next_state);
    }
}


static SXVOID
get_next_states (SXINT transition)
{
    SXINT 	prdct_true_symbol, prdct_no, state, trans, symbol;
    SXBOOLEAN	is_dynam_prdct_already_processed;

    if ((state = SXDG.trans_to_next_state [transition]) == 0)
	sxdp_add_new_state (transition, &state);

    if (state < 0)
    {
	/* Il y a des predicats ds le coup */
	prdct_true_symbol = 0;
	is_dynam_prdct_already_processed = SXFALSE;

	XxY_Xforeach (SXDG.transitions, state, trans)
	{
	    symbol = XxY_Y (SXDG.transitions, trans);
	    prdct_no = XT_TO_PRDCT (SXDG, symbol);

	    if (prdct_no == -1)
		prdct_true_symbol = symbol;
	    else
		if (prdct_no < -1)
		{
		    if (!is_dynam_prdct_already_processed)
		    {
			/* Les predicats dynamiques sont traites "simultanement"
			   On pourrait de plus rendre accessible l'ensemble des
			   predicats attendus ?. */
			is_dynam_prdct_already_processed = SXTRUE;

			if (sxplocals.mode.with_parsprdct)
			    /* Remplit SXDG.prdct_stack avec tous les predicats corrects. */
			    (*sxplocals.SXP_tables.parsact) (SXPREDICATE, prdct_no);
		    }
		}
		else
		    if (sxplocals.mode.with_parsprdct
			&& (*sxplocals.SXP_tables.parsact) (SXPREDICATE, prdct_no))
			SS_push (SXDG.prdct_stack, prdct_no);
	}

	symbol = XxY_Y (SXDG.transitions, transition);

	while (!SS_is_empty (SXDG.prdct_stack))
	    push_next_state (state, -XxY_is_set (&SXDG.xt, symbol, SS_pop (SXDG.prdct_stack)));

	if (SS_is_empty (SXDG.next_state_stack) && prdct_true_symbol != 0)
	    push_next_state (state, prdct_true_symbol);
    }
    else
	SS_push (SXDG.next_state_stack, state);
}


static SXBOOLEAN
solve_conflicts (SXINT parse_stack, SXINT *la_lgth, SXINT *rule_or_trans, SXINT *pstack1);


static SXVOID
read_a_tok (SXINT ptok_no)
{
    SXINT Mtok_no;

    if (ptok_no > sxplocals.Mtok_no) {
	do {
	    Mtok_no = sxplocals.Mtok_no;
	    (*(sxplocals.SXP_tables.scanit)) ();

	    if (sxplocals.Mtok_no > Mtok_no && SXDG.token_filter != NULL)
		/* On le filtre */
		(*SXDG.token_filter) (sxplocals.Mtok_no);
	} while (ptok_no > sxplocals.Mtok_no);
    }
}




static SXINT
check_new_pstack (SXINT pstack_orig, SXINT pstack, SXINT state, SXINT ptok_no)
{
    SXINT		conf, pstack1, son_pstack;
    /* Pour un meme ptok_no, la pile d'analyse croit de facon non bornee ssi il existe
       un cycle vide dans l'automate LR(0). */
    /* Pour un meme ptok_no, la pile d'analyse croit (strictement) au cours d'une reduction
       ssi on fait une reduction A -> empty. */
    /* Soit pi une sous pile de pi' (pour le meme ptok_no) (|pi'| > |pi|) telles que pi:1 == pi':1.
       Soit pi''j les piles intermediaires :
       pi''0 = pi, pi''1 = (|pi|+1):pi', ..., pi''j = (|pi|+j):pi', ..., pi''n = pi'
       Soit si l'etat sommet de pile de la pile p''i, on a :
       s0 ->A1 s1 ->A2 s2 ->A3 ... ->An sn ou Ai =>+ empty or sn == s0, on a donc un cycle vide
       ds l'automate LR(0) et la pile croit donc indefiniment. */


    if (new_pstack (pstack, state, ptok_no, &pstack1))
    {
	/* On regarde si pstack n'a pas une sous_pstack avec meme state et meme ptok_no */
	conf = pstack1;

	while ((conf = XxYxZ_X (SXDG.parse_stacks, conf)) > 0
	       && XxYxZ_Z (SXDG.parse_stacks, conf) == ptok_no)
	{
	    if (XxYxZ_Y (SXDG.parse_stacks, conf) == state)
	    {
		pstack1 = 0;
		break;
	    }
	}
    }
    else
    {
	/* On regarde s'il y a un cycle sur "son". */
	/* L'appelant va mettre un lien "son" de pstack_orig vers pstack1. */
	son_pstack = pstack1;
	
	do {
	    if (son_pstack == pstack_orig || XxYxZ_Z (SXDG.parse_stacks, son_pstack) != ptok_no)
		break;
	} while ((son_pstack = SXDG.pstack_to_attr [son_pstack].son) != 0);

	if (son_pstack == pstack_orig)
	    /* Grammaire cyclique. */
	    pstack1 = 0;
    }

    if (pstack1 == 0)
    {
	if (!sxplocals.mode.is_silent)
	    sxerror (SXGET_TOKEN (sxplocals.ptok_no).source_index,
		     sxplocals.sxtables->err_titles [2][0],
		     "%sImplementation restriction,\n\
the current parser cannot handle cyclic grammars.",
		     sxplocals.sxtables->err_titles [2]+1);
	
	SXEXIT (0);
    }

    return pstack1;
}

static SXINT
parsact_DO (SXINT rule, SXINT rhs_lgth)
{
    SXINT action;

    if (sxplocals.mode.with_do_undo
	&& (action = SXDG.rule_to_action [rule]) < 0)
    {
	/* Effectue l'appel des parsact en look_ahead */
	sxpglobals.reduce = rule;
	sxpglobals.pspl = rhs_lgth - 1;
	sxplocals.atok_no = sxplocals.ptok_no;

	(*sxplocals.SXP_tables.parsact) (SXDO, -action);
    }
    else
	action = 0;

    return action;
}

static SXVOID
parsact_UNDO (SXINT rule, SXINT rhs_lgth, SXINT action)
{
    
    sxpglobals.reduce = rule;
    sxpglobals.pspl = rhs_lgth - 1;
    sxplocals.atok_no = sxplocals.ptok_no;

    (*sxplocals.SXP_tables.parsact) (SXUNDO, -action);
}


static SXBOOLEAN
check_reduce (SXINT pstack_orig, SXINT la_lgth, SXINT rule)
{
    SXINT		lhs, prdct_no, rhs_lgth, i, transition, state, pstack, nps, nc, rule_or_trans, action;
    struct pstack_to_attr	*pconf;
    SXBOOLEAN	is_checked = SXFALSE;;
    /* Les parsact sont codees en negatif, ca permet de les reconnaitre
       facilement tout en permettant de les creer dynamiquement au milieu
       de regles normales. */

    rhs_lgth = SXDG.rule_to_rhs_lgth [rule];

    action = parsact_DO (rule, rhs_lgth); /* Si <0, appel de SXDO */

    lhs = XxY_X (SXDG.P, rule);
    nps = 0;

    sxpglobals.xps -= rhs_lgth;
    pstack = pstack_orig;

    for (i = 1; i<= rhs_lgth; i++)
	pstack = XxYxZ_X (SXDG.parse_stacks, pstack);

    if (action < 0 && SXDG.has_been_updated)
	check_viable_prefix (sxpglobals.xps, &pstack);

    state = XxYxZ_Y (SXDG.parse_stacks, pstack);

    if ((transition = XxY_is_set (&SXDG.transitions, state, lhs)) > 0)
    {
	if ((state = SXDG.trans_to_next_state [transition]) == 0)
	    sxdp_add_new_state (transition, &state);

	if (++sxpglobals.xps > SXDG.lgt1)
	    parse_stack_oflw ();

	if ((nps = check_new_pstack (pstack_orig, pstack, state, sxplocals.ptok_no)) != 0)
	{
	    if (sxplocals.mode.with_parsprdct
		&& (prdct_no = XNT_TO_PRDCT (SXDG, lhs)) != -1)
		is_checked = (*sxplocals.SXP_tables.parsact) (SXPREDICATE, prdct_no);
	    else
		is_checked = SXTRUE;
	    
	    if (is_checked)
	    {
              if ((is_checked = solve_conflicts (nps, &la_lgth, &rule_or_trans, &nc)))
		{
		    pconf = SXDG.pstack_to_attr + nps;

		    pconf->rule_or_trans = rule;
		    pconf->brother = SXDG.pstack_to_attr [pstack_orig].son;
		    SXDG.pstack_to_attr [pstack_orig].son = nps;
		}
	    }

	    if (!is_checked)
		XxYxZ_erase (SXDG.parse_stacks, nps);
	}

	sxpglobals.xps--;
    }

    sxpglobals.xps += rhs_lgth;

    if (action < 0)
    {
	/* Defait les parsact appeles en look_ahead */
	parsact_UNDO (rule, rhs_lgth, action);

	if (SXDG.has_been_updated)
	    reactivate_path (sxpglobals.xps, pstack_orig);
    }

    return is_checked;
}


static SXINT
check_t_trans (SXINT pstack, SXINT la_lgth, SXINT transition)
{
    /* pstack1 est positionne' ssi la valeur retournee est 1 */
    /* On effectue la transition terminale depuis le sommet de pstack et on regarde si
       token [sxplocals.ptok_no] est un suivant possible. */
    SXINT		state, valid_trans_nb, pstack1, lahead, rule_or_trans, nc;
    struct pstack_to_attr	*pconf;
    SXBOOLEAN	last_t_trans, is_checked;

    /* On teste les predicats eventuels et on recupere les etats atteints. */
    SS_open (SXDG.next_state_stack);
    get_next_states (transition);
    lahead = XxY_Y (SXDG.transitions, transition);
    last_t_trans = (la_lgth == 1
		    || la_lgth == 0
		    || lahead == SXDG.eof_code
		    || sxgetbit (sxplocals.SXP_tables.PER_tset, -lahead) != 0);
    valid_trans_nb = 0;
    /* Les predicats ont ete traites, on lit le symbole suivant */
    sxplocals.ptok_no++;

    if (la_lgth > 1)
	la_lgth--;

    while (!SS_is_empty (SXDG.next_state_stack))
    {
	state = SS_pop (SXDG.next_state_stack);

	if (++sxpglobals.xps > SXDG.lgt1)
	    parse_stack_oflw ();

	/* check_new_pstack () est inutile sur les transitions terminales. */
	new_pstack (pstack, state, sxplocals.ptok_no, &pstack1);

	if (last_t_trans)
	    is_checked = SXTRUE;
	else
	    if (!(is_checked = solve_conflicts (pstack1, &la_lgth, &rule_or_trans, &nc)))
		XxYxZ_erase (SXDG.parse_stacks, pstack1);

	if (is_checked)
	{
	    valid_trans_nb++; 
	    pconf = SXDG.pstack_to_attr + pstack1;
	    pconf->rule_or_trans = -transition; /* < 0 */
	    pconf->brother = SXDG.pstack_to_attr [pstack].son;
	    SXDG.pstack_to_attr [pstack].son = pstack1;
	}

	sxpglobals.xps--;
    }

    sxplocals.ptok_no--;
    SS_close (SXDG.next_state_stack);

    return valid_trans_nb;
}

static SXVOID
erase_pstack (SXINT pstack)
{
    SXINT next_pstack;

    for (next_pstack = SXDG.pstack_to_attr [pstack].son;
	 next_pstack != 0;
	 next_pstack = SXDG.pstack_to_attr [next_pstack].brother)
	erase_pstack (next_pstack);

    XxYxZ_erase (SXDG.parse_stacks, pstack);
}

static SXVOID
erase_subpstack (SXINT pstack)
{
    SXINT prev_pstack;

    while ((prev_pstack = pstack) != 0)
    {
	pstack = XxYxZ_X (SXDG.parse_stacks, pstack);
	XxYxZ_erase (SXDG.parse_stacks, prev_pstack);
    }
}


static SXBOOLEAN
call_solve_conflicts (SXINT pstack_orig, SXINT pstack, SXINT *la_lgth)
{
    /* On ajuste la parse-stack pour la faire passer a pstack
       et on appelle solve_conflicts sans verification. */
    SXINT rule_or_trans, pstack1, rhs_lgth, rule, action;
    SXBOOLEAN	is_solved;
 /* SXINT state = XxYxZ_Y (SXDG.parse_stacks, pstack); */

    sxinitialise(rhs_lgth); /* pour faire taire "gcc -Wuninitialized" */
    if ((rule_or_trans = SXDG.pstack_to_attr [pstack].rule_or_trans) > 0)
    {
	/* C'est un reduce */
	rhs_lgth = SXDG.rule_to_rhs_lgth [rule_or_trans];
	action = parsact_DO (rule_or_trans, rhs_lgth);
	sxpglobals.xps -= rhs_lgth;
    }
    else
    {
	sxplocals.ptok_no++;
	action = 0;
    }

    if (++sxpglobals.xps > SXDG.lgt1)
	parse_stack_oflw ();

    if (action < 0 && SXDG.has_been_updated)
	reactivate_path (sxpglobals.xps, pstack);

    is_solved = solve_conflicts (pstack, la_lgth, &rule, &pstack1);

    sxpglobals.xps--;

    if (rule_or_trans > 0)
    {
	sxpglobals.xps += rhs_lgth;

	if (action < 0)
	{
	    parsact_UNDO (rule_or_trans, rhs_lgth, action);

	    if (SXDG.has_been_updated)
		reactivate_path (sxpglobals.xps, pstack_orig);
	}
    }
    else
	sxplocals.ptok_no--;

    return is_solved;
}



static SXBOOLEAN
solve_conflicts (SXINT parse_stack, SXINT *la_lgth, SXINT *rule_or_trans, SXINT *pstack1)
{
    /* pstack est la pile courante. */
    /* sxplocals.ptok_no est le look_ahead courant */
    /* On resoud les conflits eventuels en utilisant dans l'ordre :
       - du LR (k)
       - prio
       - du look-ahead non borne'
       - sinon (ambigu) resolution systeme
       */
    /* On ne detecte pas les erreurs de syntaxe en look-ahead, elles correspondent
       a la detection de branches mortes. */

    /* Si une pstack a ete resolue (pour un k donne'), elle l'est pour n'importe quel k': 
       - si k' <= k evident
       - si k' > k seule une erreur peut etre detectee en k'' (k < k''<= k') mais
       la resolution locale ne change pas. */

    SXINT		rule, prio, prio_reduce, xt, prio_shift, lahead, sup_lahead, valid_action_nb, pstack, state, item_occur, reduce_transition, shift_transition, shift_pstack, reduce_pstack, assoc, choosed_pstack, choosed_rule;
    SXINT		*pconf;
    SXBOOLEAN	is_in_look_ahead;
    struct pstack_to_attr	*pattr = SXDG.pstack_to_attr + parse_stack;

    sxinitialise(xt); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(assoc); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(choosed_pstack); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(choosed_rule); /* pour faire taire "gcc -Wuninitialized" */
    pstack = parse_stack; /* Si pstack1 == &(parse_stack) */

    if (pattr->la_lgth != 0)
    {
	/* Deja calcule' */
	*pstack1 = pattr->son;

	if (*pstack1 == 0)
	    /* et a e'choue' */
	    return SXFALSE;

	*rule_or_trans = SXDG.pstack_to_attr [*pstack1].rule_or_trans;

	if (pattr->la_lgth == -1
	    || (*la_lgth >= 0 && *la_lgth <= pattr->la_lgth) /* .. sur une distance superieure */)
	    return SXTRUE;

	/* On relance le tout sur la nouvelle longueur */
	/* pstack1 est unique car deja calcule'. */

	pattr->la_lgth = *la_lgth;

	if (*la_lgth > 0 && *rule_or_trans < 0)
	    --(*la_lgth);
	    
	if (call_solve_conflicts (parse_stack, *pstack1, la_lgth))
	    return SXTRUE;
	    
	erase_pstack (*pstack1);
	pattr->son = 0;
	return SXFALSE;
    }

    state = XxYxZ_Y (SXDG.parse_stacks, pstack);
    lahead = SXGET_TOKEN (sxplocals.ptok_no).lahead;
    reduce_transition = XxY_is_set (&SXDG.transitions, state, 0);
    shift_transition = XxY_is_set (&SXDG.transitions, state, -lahead);

    if (*la_lgth == 0)
    {
	/* LR (0) */
	valid_action_nb = 0;

	if (reduce_transition > 0)
	{
	    /* Il y a au moins une reduction */
	    XxY_Xforeach (SXDG.item_occurs, reduce_transition, item_occur)
	    {
		*rule_or_trans = XxY_X (SXDG.items, XxY_Y (SXDG.item_occurs, item_occur));
		valid_action_nb++;
	    }
	}

	if (valid_action_nb == 1 && shift_transition == 0)
	{
	    /* une seule reduction et pas de shift sur le terminal courant => LR (0) */
	    *pstack1 = 0;
	    return SXTRUE;
	}

	*la_lgth = 1;
    }

    is_in_look_ahead = SXDG.is_in_look_ahead;
    SXDG.is_in_look_ahead = *la_lgth != 0;

    if (shift_transition > 0)
	valid_action_nb = check_t_trans (pstack, *la_lgth, shift_transition);
    else
	valid_action_nb = 0;

    if (reduce_transition > 0)
    {
	/* Il y a au moins une reduction */
	XxY_Xforeach (SXDG.item_occurs, reduce_transition, item_occur)
	{
	    rule = XxY_X (SXDG.items, XxY_Y (SXDG.item_occurs, item_occur));

	    if (check_reduce (pstack, *la_lgth, rule))
		valid_action_nb++;
	}
    }

    pattr->la_lgth = *la_lgth;
    *pstack1 = (valid_action_nb == 0) ? 0 : pattr->son;

    if (valid_action_nb > 1)
    {
	/* Conflit */
	/* Attention, on peut avoir des conflits Shift/Shift !!!
	   ds le cas ou plusieurs predicats associes au meme terminal ont
	   repondu vrai. */
	/* On essaye une resolution par "prio". */
	if (SXDG.t_priorities != NULL)
	{
	    prio_reduce = 0;
	    prio_shift = 0;
	    shift_pstack = 0;
	    reduce_pstack = 0;

	    for (*pstack1 = pattr->son;
		 *pstack1 != 0;
		 *pstack1 = SXDG.pstack_to_attr [*pstack1].brother)
	    {
		if ((rule = SXDG.pstack_to_attr [*pstack1].rule_or_trans) > 0)
		{
		    /* Reduce */
		    if ((prio = SXDG.r_priorities [rule].value) == 0)
			break; 

		    if (prio > prio_reduce)
		    {
			prio_reduce = prio;
			reduce_pstack = *pstack1;
			assoc = SXDG.r_priorities [rule].assoc; /* au cas ou ... */
		    }
		    else if (prio == prio_reduce)
			reduce_pstack = 0;
		}
		else
		{
		    /* Shift */
		    xt = SXDG.state_to_in_symbol [XxYxZ_Y (SXDG.parse_stacks, *pstack1)];

		    if ((prio = SXDG.t_priorities [-xt].value) == 0)
			break; 

		    if (prio > prio_shift)
		    {
			prio_shift = prio;
			shift_pstack = *pstack1;
			assoc = SXDG.t_priorities [-xt].assoc; /* au cas ou ... */
		    }
		    else if (prio == prio_shift)
			shift_pstack = 0;
		}
	    }

	    if (*pstack1 == 0)
	    {
		/* Toutes les actions ont des "prio" */
		if (prio_shift > 0)
		{
		    if (prio_shift > prio_reduce)
		    {
			/* Priorite au[x] shift[s] */
			choosed_pstack = shift_pstack;
		    }
		    else
		    {
			if (prio_shift == prio_reduce)
			{
			    /* On prend l'associativite memorisee */
			    switch (assoc)
			    {
			    case NON_ASSOC:
				/* conflit non resolu! => erreur */
				choosed_pstack = 0;
				prio_reduce++; /* Pour les supprimer tous ! */ 
				prio_shift++; /* Pour les supprimer tous ! */ 
				break;

			    case LEFT_ASSOC:
				/* On choisit le[s] reduce[s] */
				choosed_pstack = reduce_pstack;
				break;

			    case RIGHT_ASSOC:
				/* On choisit le shift */
				choosed_pstack = shift_pstack;
				break;
			    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
			      sxtrap(ME,"unknown switch case #1");
#endif
			      break;
			    }
			}
			else
			    /* On choisit le[s] reduce[s] */
			    choosed_pstack = reduce_pstack;
		    }
		}
		else
		    choosed_pstack = reduce_pstack;

		if (choosed_pstack != 0)
		{
		    pconf = &(pattr->son);

		    while (*pconf != 0)
		    {
			if (SXDG.pstack_to_attr [*pconf].rule_or_trans > 0
			    && SXDG.pstack_to_attr [*pconf].son == 0)
			{
			    /* Reduce && incorrect */
			    erase_pstack (*pconf);
			    *pconf = SXDG.pstack_to_attr [*pconf].brother;
			}
			else
			    pconf = &(SXDG.pstack_to_attr [*pconf].brother);
		    }

		    valid_action_nb = 1;
		    *pstack1 = choosed_pstack;
		}
		else
		{
		    valid_action_nb = 0;
		    pconf = &(pattr->son);

		    while (*pconf != 0)
		    {
			if ((rule = SXDG.pstack_to_attr [*pconf].rule_or_trans) < 0)
			    xt = SXDG.state_to_in_symbol [XxYxZ_Y (SXDG.parse_stacks, *pconf)];

			if ((rule > 0 /* Reduce */ && SXDG.r_priorities [rule].value < prio_reduce)
			    || (rule < 0 /* Shift */ && SXDG.t_priorities [-xt].value < prio_shift))
			{
			    erase_pstack (*pconf);
			    *pconf = SXDG.pstack_to_attr [*pconf].brother;
			}
			else
			{
			    valid_action_nb++;
			    pconf = &(SXDG.pstack_to_attr [*pconf].brother);
			}
		    }
		}
	    }
	}

	if (valid_action_nb > 1)
	{
	    /* Ca n'a pas marche' */
	    /* On tente une resolution avec look-ahead non borne' */
	    /* Une autre variante pourrait etre interessante (et tres facile a implanter) :
	       - si la_lgth == 1, on essaye la_lgth = 2 (ca permet
	       de tester les predicats eventuels associes au token de
	       look-ahead
	       - sinon, on lit le source jusqu'a tomber sur un terminal cle'
	       en comptant le la_lgth correspondant et on lance une seule fois
	       la resolution non bornee sur cette valeur de la_lgth. Si ca ne
	       marche pas, => pseudo-ambiguite', si ca marche (une seule action)
	       les mauvais chemins se sont arretes au plus tot et les actions
	       du bon sont stockees, la suite sera donc rapide. Ca semble un
	       bon compromis entre acuite de la detection de l'ambiguite et
	       quantite de choses a stocker.
	       */
	    sup_lahead = -SXDG.eof_code; /* Le 13/04/07 prudence */

	    if (*la_lgth != -1)
	    {
		SXDG.Mtok_no = sxplocals.ptok_no + *la_lgth;

		do {
		    read_a_tok (SXDG.Mtok_no);
		    sup_lahead = SXGET_TOKEN (SXDG.Mtok_no).lahead;
		    SXDG.Mtok_no++; 
		} while (sup_lahead != -SXDG.eof_code
			 && sxgetbit (sxplocals.SXP_tables.PER_tset, sup_lahead) == 0);

		*la_lgth = -1;
	    }

	    valid_action_nb = 0;
	    pconf = &(pattr->son);
	    pattr->la_lgth = -1;

	    while (*pconf != 0)
	    {
		if (call_solve_conflicts (parse_stack, *pconf, la_lgth))
		{
		    valid_action_nb++;
		    *pstack1 = *pconf;
		    pconf = &(SXDG.pstack_to_attr [*pconf].brother);
		}
		else
		{
		    erase_pstack (*pconf);
		    *pconf = SXDG.pstack_to_attr [*pconf].brother;
		}
	    }

	    if (valid_action_nb > 1)
	    {
		/* Ambiguite */
		/* Tenir compte des grammaires cycliques... */

		if (SXDG.desambig != NULL)
		{
		    /* On passe la main a l'utilisateur */
		    if (SXDG.ambig_stack == NULL)
		    {
			SXDG.ambig_stack = (struct ambig_stack_elem*)
			    sxalloc ((SXDG.ambig_stack_size = valid_action_nb + 20) + 1,
				     sizeof (struct ambig_stack_elem));
		    }
		    else
			if (valid_action_nb > SXDG.ambig_stack_size)
			    SXDG.ambig_stack = (struct ambig_stack_elem*)
				sxrealloc (SXDG.ambig_stack,
					   (SXDG.ambig_stack_size = valid_action_nb + 20) + 1,
					   sizeof (struct ambig_stack_elem));

		    SXDG.ambig_stack_top = 0;
			
		    for (*pstack1 = pattr->son;
			 *pstack1 != 0;
			 *pstack1 = SXDG.pstack_to_attr [*pstack1].brother)
		    {
			SXDG.ambig_stack_top++;
			SXDG.ambig_stack [SXDG.ambig_stack_top].rule =
			    SXDG.pstack_to_attr [*pstack1].rule_or_trans;
		    }

		    (*SXDG.desambig) ();

		    SXDG.ambig_stack_top = 0;
		    valid_action_nb = 0;
		    pconf = &(pattr->son);

		    while (*pconf != 0)
		    {
			SXDG.ambig_stack_top++;

			if (SXDG.ambig_stack [SXDG.ambig_stack_top].rule == 0)
			{
			    /* Elimine' */
			    erase_pstack (*pconf);
			    *pconf = SXDG.pstack_to_attr [*pconf].brother;
			}
			else
			{
			    *pstack1 = *pconf;
			    pconf = &(SXDG.pstack_to_attr [*pconf].brother);
			    valid_action_nb++;
			}
		    }
		}
		else
		{
		    if (!sxplocals.mode.is_silent)
			sxerror (SXGET_TOKEN (sxplocals.ptok_no).source_index,
				 sxplocals.sxtables->err_titles [2][0],
				 "%sthis source text %s ambiguous.",
				 sxplocals.sxtables->err_titles [2]+1,
				 sup_lahead == -SXDG.eof_code ? "is" : "seems");
		}

		if (valid_action_nb > 1)
		{
		    /* Resolution systeme :
		       Priorite au shift sinon priorite a la regle de numero maximal. */
		    /* S'il y a plusieurs shift, celui associe' au predicat &true (s'il
		       existe) est e'limine'. */
		    choosed_pstack = 0;

		    for (*pstack1 = pattr->son;
			 *pstack1 != 0;
			 *pstack1 = SXDG.pstack_to_attr [*pstack1].brother)
		    {
			if ((rule = SXDG.pstack_to_attr [*pstack1].rule_or_trans) > 0)
			{	
			    if (choosed_pstack == 0 || (choosed_rule > 0 && rule > choosed_rule))
				{
				    choosed_pstack = *pstack1;
				    choosed_rule = rule;
				}
			}
			else
			{
			    if (choosed_pstack >= 0
				|| XT_TO_PRDCT (SXDG,
						SXDG.state_to_in_symbol [XxYxZ_Y (SXDG.parse_stacks,
									       *pstack1)]) == -1)
			    {
				/* C'est le premier ou le precedent est un reduce ou un
				   shift sur predicat &true. */
				choosed_pstack = *pstack1;
				choosed_rule = rule;
			    }
			}
		    }

		    pconf = &(pattr->son);

		    while (*pconf != 0)
		    {
			if (*pconf != choosed_pstack)
			{
			    erase_pstack (*pconf);
			    *pconf = SXDG.pstack_to_attr [*pconf].brother;
			}
			else
			    pconf = &(SXDG.pstack_to_attr [*pconf].brother);
		    }

		    *pstack1 = choosed_pstack;
		}
	    }
	}
    }

    SXDG.is_in_look_ahead = is_in_look_ahead;
    *rule_or_trans = (*pstack1 != 0) ? SXDG.pstack_to_attr [*pstack1].rule_or_trans : 0;

    return *pstack1 != 0;
}



static SXINT
reduce (SXINT rule, SXINT pstack_orig, SXINT pstack1)
{
    /* Si pstack1 == 0, on doit la calculer */
    SXINT		action, rhs_lgth, conf, pstack, lhs, prdct_no, state, transition;
    SXBOOLEAN	is_semact, is_checked;

    sxpglobals.reduce = rule;
    sxpglobals.pspl = (rhs_lgth = SXDG.rule_to_rhs_lgth [rule]) - 1;

    sxplocals.atok_no = sxplocals.ptok_no;

    /* Les parsact sont codees en negatif, ca permet de les reconnaitre
       facilement tout en permettant de les creer dynamiquement au milieu
       de regles normales. */

    action = SXDG.rule_to_action [rule];

    if ((is_semact = (action >= 0))) {
	if (sxplocals.mode.with_semact)
	    (*sxplocals.SXP_tables.semact) (SXACTION, action);
    }
    else
	if (sxplocals.mode.with_parsact)
	    (*sxplocals.SXP_tables.parsact) (SXACTION, -action);

    sxpglobals.xps -= sxpglobals.pspl;

    if (sxpglobals.xps > SXDG.lgt1)
	parse_stack_oflw ();

    sxpglobals.parse_stack [sxpglobals.xps].token.lahead =
	is_semact ? 0 /* nt */ : -1 /* action */ ;

    pstack = pstack_orig;

    while (rhs_lgth-- > 0)
    {
	conf = pstack;
	pstack = XxYxZ_X (SXDG.parse_stacks, pstack);
	XxYxZ_erase (SXDG.parse_stacks, conf);
    }

    if (pstack1 == 0)
    {
	if (SXDG.has_been_updated)
	    check_viable_prefix (sxpglobals.xps - 1, &pstack);

	/* On la calcule */
	lhs = XxY_X (SXDG.P, rule);

	state = XxYxZ_Y (SXDG.parse_stacks, pstack);

	if ((transition = XxY_is_set (&SXDG.transitions, state, lhs)) > 0)
	{
	    if ((state = SXDG.trans_to_next_state [transition]) == 0)
		sxdp_add_new_state (transition, &state);

	    if (sxplocals.mode.with_parsprdct
		&& (prdct_no = XNT_TO_PRDCT (SXDG, lhs)) != -1)
		is_checked = (*sxplocals.SXP_tables.parsact) (SXPREDICATE, prdct_no);
	    else
		is_checked = SXTRUE;
	    
	    if (is_checked)
	    {
		new_pstack (pstack, state, sxplocals.ptok_no, &pstack1);
		SXDG.pstack_to_attr [pstack1].rule_or_trans = rule;

		/* pstack_orig a disparu (sauf si pstack_orig == pstack) */
		/* De toutes les facons, le lien (vers pstack1) ne sert plus a rien. */
		SXDG.pstack_to_attr [pstack].son = 0; /* C'est + propre */
	    }
	}
    }
    else
    {
	if (SXDG.has_been_updated)
	    /* Il faut refabriquer une "bonne" parse_stack a partir de pstack1
	       et recreer ds l'automate les liens entre 2 etats successeurs qui ont
	       pu etre supprimes. */
	    reactivate_path (sxpglobals.xps, pstack1);
    }

    return pstack1;
}


static SXINT
shift (void)
{
    SXINT rule_or_trans, pstack, state, la_lgth;
    SXINT next_state, xt, trans;

    la_lgth = 0; /* LR (0) */

    while (solve_conflicts (pstack = SXDG.parse_stack, &la_lgth, &rule_or_trans, &SXDG.parse_stack))
    {
	if (rule_or_trans < 0)
	{
	    /* Shift (non precede' de reduce) entierement verifie'. */
	    return XxYxZ_Y (SXDG.parse_stacks, SXDG.parse_stack);
	}

	if (la_lgth == 0)
	{
	    /* Resolu en LR (0) (ou deja calcule')  */
	    SXDG.parse_stack = reduce (rule_or_trans, pstack, SXDG.parse_stack);
	}
	else
	{
	    /* On a une sequence (eventuellement vide) de reduce suivie d'un shift. */
	    
	    while (rule_or_trans > 0) {
		pstack = reduce (rule_or_trans, pstack, SXDG.parse_stack);
		SXDG.parse_stack = SXDG.pstack_to_attr [SXDG.parse_stack].son;

#if EBUG
		if (SXDG.parse_stack == 0)
		    sxtrap (ME, "shift");
#endif

		rule_or_trans = SXDG.pstack_to_attr [SXDG.parse_stack].rule_or_trans;
	    }

#if EBUG
		if (rule_or_trans == 0)
		    sxtrap (ME, "shift");
#endif

	    /* On termine sur un scan */
	    /* Attention, les reduces precedents ont pu effacer des etats ds l'automate et en
	       particulier la transition (etendue) qui mene de pstack a SXDG.parse_stack
	       (et des regles qui l'utiliserait ajoutees).
	       Il faut donc la recalculer eventuellement. */

	    if ((state = SXDG.trans_to_next_state [-rule_or_trans]) == 0)
		sxdp_add_new_state (-rule_or_trans, &state);

	    next_state = XxYxZ_Y (SXDG.parse_stacks, SXDG.parse_stack);

	    if (state < 0)
	    {
		xt = SXDG.state_to_in_symbol [next_state];
		trans = XxY_is_set (&SXDG.transitions, state, xt);

		if (trans > 0)
		    state = SXDG.trans_to_next_state [trans];
	    }

	    if (next_state != state)
		/* L'inegalite peut etre due a l'absence de /SXDO/SXUNDO/ ds les parsact :
		   le look-ahead a reussi avec une vieille grammaire (SXDG.parse_stack
		   a des etats du vieil automate) et on vient de recalculer avec le
		   nouvel automate. */
		sxtrap (ME, "shift");

	    return next_state;
	}
    } 

    return SXDG.parse_stack = 0;
}


static SXBOOLEAN
sxdp_parse_it (void)
{
    SXINT next_state;

    new_pstack ((SXINT)0, SXDG.init_state, sxplocals.ptok_no, &SXDG.parse_stack);

    if ((next_state = SXDG.trans_to_next_state [SXDG.init_transition]) == 0)
	sxdp_add_new_state (SXDG.init_transition, &next_state);

    new_pstack (SXDG.parse_stack, next_state, sxplocals.ptok_no, &SXDG.parse_stack);
    sxpglobals.xps = 2;

    do {
	read_a_tok (++sxplocals.ptok_no);
	
	if ((next_state = shift ()) == 0)
	    break;

	if (++sxpglobals.xps > SXDG.lgt1)
	    parse_stack_oflw ();

	sxpglobals.parse_stack [sxpglobals.xps].token = SXGET_TOKEN (sxplocals.ptok_no);

	if (SXDG.has_been_erased && (sxplocals.ptok_no & 0177) == 0)
	    GC_states ();
    } while (SXGET_TOKEN (sxplocals.ptok_no).lahead != -SXDG.eof_code);

    return next_state != 0;
}


SXBOOLEAN
sxdp_new_xt (SXINT t, SXINT prdct, SXINT *xt, SXINT prio, SXINT assoc)
{
    SXBOOLEAN is_new;

    if ((is_new = !XxY_set (&SXDG.xt, t, prdct, xt)))
    {
	if (SXDG.t_priorities != NULL)
	{
	    SXDG.t_priorities [*xt].value = prio;
	    SXDG.t_priorities [*xt].assoc = assoc;
	}
    }

    return is_new;
}


SXVOID
sxdp_delete_xt (SXINT xt)
{
    /* xt > 0 */
    XxY_erase (SXDG.xt, xt);
}

SXBOOLEAN
sxdp_new_xnt (SXINT nt, SXINT prdct, SXINT *xnt)
{
    return !XxY_set (&SXDG.xnt, nt, prdct, xnt);
}


SXVOID
sxdp_delete_xnt (SXINT xnt)
{
    /* xt > 0 */
    XxY_erase (SXDG.xnt, xnt);
}

SXBOOLEAN
sxdp_write (sxfiledesc_t file_descr /* file descriptor */ )
{
#define WRITE(p,l)	((bytes=(l))>0&&(write(file_descr, p, (size_t)bytes) == bytes))
  SXINT	bytes;  
  SXBOOLEAN is_prio = SXDG.r_priorities != NULL;

  return (sxlist_write (&(SXDG.states), file_descr)

	  && XxY_write (&SXDG.P, file_descr)
	  && XxY_write (&SXDG.rhs, file_descr)
	  && XxY_write (&SXDG.items, file_descr)
	  && XxY_write (&SXDG.transitions, file_descr)
	  && XxY_write (&SXDG.item_occurs, file_descr)
	  && XxY_write (&SXDG.transitionxnext_state, file_descr)
	  && XxY_write (&SXDG.xt, file_descr)
	  && XxY_write (&SXDG.xnt, file_descr)

	  && sxba_write (file_descr, SXDG.item_set)
	  && sxba_write (file_descr, SXDG.trans_has_prdct)
	  && sxba_write (file_descr, SXDG.is_state_linked)
	  && sxba_write (file_descr, SXDG.active_rule_set)

	  && WRITE (SXDG.rule_to_rhs_lgth, sizeof (SXINT) * (XxY_top (SXDG.P) + 1))
	  && WRITE (SXDG.rule_to_first_item, sizeof (SXINT) * (XxY_top (SXDG.P) + 1))
	  && WRITE (SXDG.rule_to_action, sizeof (SXINT) * (XxY_top (SXDG.P) + 1))
	  && WRITE (SXDG.state_to_action, sizeof (SXINT) * (sxlist_size (SXDG.states) + 1))
	  && WRITE (SXDG.state_to_in_symbol, sizeof (SXINT) * (sxlist_size (SXDG.states) + 1))
	  && WRITE (SXDG.state_to_used_lnk, sizeof (SXINT) * (sxlist_size (SXDG.states) + 1))
	  && WRITE (SXDG.item_to_next_item, sizeof (SXINT) * (XxY_top (SXDG.items) + 1))
	  && WRITE (SXDG.trans_to_next_state, sizeof (SXINT) * (XxY_top (SXDG.transitions) + 1))

	  && WRITE (&(SXDG.init_state), sizeof (SXINT))
	  && WRITE (&(SXDG.init_transition), sizeof (SXINT))
	  && WRITE (&(SXDG.super_start_symbol), sizeof (SXINT))
	  && WRITE (&(SXDG.start_symbol), sizeof (SXINT))
	  && WRITE (&(SXDG.super_rule), sizeof (SXINT))
	  && WRITE (&(SXDG.eof_code), sizeof (SXINT))

	  && WRITE (&is_prio, sizeof (SXBOOLEAN))
	  && (!is_prio || WRITE (SXDG.t_priorities, sizeof (struct priority) * (XxY_top (SXDG.xt) + 1)))
	  && (!is_prio || WRITE (SXDG.r_priorities, sizeof (struct priority) * (XxY_top (SXDG.P) + 1)))

	  );
}


SXBOOLEAN
sxdp_read (sxfiledesc_t file_descr /* file descriptor */)
{
#define READ(p,l)	((bytes=(l))>0&&(read (file_descr, p, (size_t) bytes) == bytes))
  SXINT	        bytes;
  SXBOOLEAN     is_prio;

  return (sxlist_read (&(SXDG.states), file_descr, "states", states_oflw, (sxoflw1_t) NULL, NULL)

	  && XxY_read (&SXDG.P, file_descr, "P", P_oflw, NULL)
	  && XxY_read (&SXDG.rhs, file_descr, "rhs", (sxoflw0_t) NULL, NULL)
	  && XxY_read (&SXDG.items, file_descr, "items", items_oflw, NULL)
	  && XxY_read (&SXDG.transitions, file_descr, "transitions", transitions_oflw, NULL)
	  && XxY_read (&SXDG.item_occurs, file_descr, "item_occurs", (sxoflw0_t) NULL, NULL)
	  && XxY_read (&SXDG.transitionxnext_state, file_descr, "transitionxnext_state", (sxoflw0_t) NULL, NULL)
	  && XxY_read (&SXDG.xt, file_descr, "xt", xt_oflw, NULL)
	  && XxY_read (&SXDG.xnt, file_descr, "xnt", (sxoflw0_t) NULL, NULL)

	  && sxba_read (file_descr, SXDG.item_set = sxba_calloc (XxY_size (SXDG.items) + 1))
	  && sxba_read (file_descr, SXDG.trans_has_prdct = sxba_calloc (XxY_size (SXDG.transitions) + 1))
	  && sxba_read (file_descr, SXDG.is_state_linked = sxba_calloc (sxlist_size (SXDG.states) + 1))
	  && sxba_read (file_descr, SXDG.active_rule_set = sxba_calloc (XxY_size (SXDG.P) + 1))

	  && READ (SXDG.rule_to_rhs_lgth = (SXINT*) sxalloc (XxY_size (SXDG.P) + 1, sizeof (SXINT)),
		   sizeof (SXINT) * (XxY_top (SXDG.P) + 1))
	  && READ (SXDG.rule_to_first_item = (SXINT*) sxalloc (XxY_size (SXDG.P) + 1, sizeof (SXINT)),
		   sizeof (SXINT) * (XxY_top (SXDG.P) + 1))
	  && READ (SXDG.rule_to_action = (SXINT*) sxalloc (XxY_size (SXDG.P) + 1, sizeof (SXINT)),
		   sizeof (SXINT) * (XxY_top (SXDG.P) + 1))
	  && READ (SXDG.state_to_action = (SXINT*) sxalloc (sxlist_size (SXDG.states) + 1, sizeof (SXINT)),
		   sizeof (SXINT) * (sxlist_size (SXDG.states) + 1))
	  && READ (SXDG.state_to_in_symbol = (SXINT*) sxalloc (sxlist_size (SXDG.states) + 1,
							     sizeof (SXINT)),
		   sizeof (SXINT) * (sxlist_size (SXDG.states) + 1))
	  && READ (SXDG.state_to_used_lnk = (SXINT*) sxalloc (sxlist_size (SXDG.states) + 1, sizeof (SXINT)),
		   sizeof (SXINT) * (sxlist_size (SXDG.states) + 1))
	  && READ (SXDG.item_to_next_item = (SXINT*) sxalloc (XxY_size (SXDG.items) + 1, sizeof (SXINT)),
		   sizeof (SXINT) * (XxY_top (SXDG.items) + 1))
	  && READ (SXDG.trans_to_next_state = (SXINT*) sxalloc (XxY_size (SXDG.transitions) + 1,
							      sizeof (SXINT)),
		   sizeof (SXINT) * (XxY_top (SXDG.transitions) + 1))

	  && READ (&(SXDG.init_state), sizeof (SXINT))
	  && READ (&(SXDG.init_transition), sizeof (SXINT))
	  && READ (&(SXDG.super_start_symbol), sizeof (SXINT))
	  && READ (&(SXDG.start_symbol), sizeof (SXINT))
	  && READ (&(SXDG.super_rule), sizeof (SXINT))
	  && READ (&(SXDG.eof_code), sizeof (SXINT))

	  && READ (&is_prio, sizeof (SXBOOLEAN))
	  && (!is_prio || READ (SXDG.t_priorities =
				(struct priority*) sxalloc (XxY_size (SXDG.xt) + 1,
							    sizeof (struct priority)),
				sizeof (struct priority) * (XxY_top (SXDG.xt) + 1)))
	  && (!is_prio || READ (SXDG.r_priorities =
				(struct priority*) sxalloc (XxY_size (SXDG.P) + 1,
							    sizeof (struct priority)),
				sizeof (struct priority) * (XxY_top (SXDG.P) + 1)))
	  );
}


SXVOID
sxdp_alloc (SXINT rule_nb)
{
    static SXINT	parse_stacks_foreach [] = {0, 0, 0, 0, 0, 0};
    
    if (SXDG.rule_to_rhs_lgth == NULL)
    {
	/* Allocation des structures pour la grammaire. */
	XxY_alloc (&SXDG.P, "P", rule_nb, 1, 1, 0, P_oflw, NULL);
	SXDG.rule_to_rhs_lgth = (SXINT*) sxalloc (XxY_size (SXDG.P) + 1, sizeof (SXINT));
	SXDG.rule_to_first_item = (SXINT*) sxalloc (XxY_size (SXDG.P) + 1, sizeof (SXINT));
	SXDG.rule_to_action = (SXINT*) sxalloc (XxY_size (SXDG.P) + 1, sizeof (SXINT));
	SXDG.active_rule_set = sxba_calloc (XxY_size (SXDG.P) + 1);
	
	XxY_alloc (&SXDG.rhs, "rhs", rule_nb*2, 1, 0, 0, (sxoflw0_t) NULL, NULL);

	XxY_alloc (&SXDG.items, "items", rule_nb*3, 1, 0, 1, items_oflw, NULL);
	SXDG.item_to_next_item = (SXINT*) sxalloc (XxY_size (SXDG.items) + 1, sizeof (SXINT));
	SXDG.item_set = sxba_calloc (XxY_size (SXDG.items) + 1);
	XxY_alloc (&SXDG.xt, "xt", rule_nb / 3, 1, 0, 0, xt_oflw, NULL);
	XxY_alloc (&SXDG.xnt, "xnt", rule_nb / 2, 1, 0, 0, (sxoflw0_t) NULL, NULL);
    }

    if (SXDG.state_to_action == NULL)
    {
	/* Allocation des structures pour l'automate. */
	sxlist_alloc (&SXDG.states, "states", rule_nb * 2, 1, 20, states_oflw, (sxoflw1_t) NULL, NULL);
	SXDG.state_to_action = (SXINT*) sxalloc (sxlist_size (SXDG.states) + 1, sizeof (SXINT));
	SXDG.state_to_in_symbol = (SXINT*) sxalloc (sxlist_size (SXDG.states) + 1, sizeof (SXINT));
	SXDG.state_to_used_lnk = (SXINT*) sxalloc (sxlist_size (SXDG.states) + 1, sizeof (SXINT));
	SXDG.is_state_linked = sxba_calloc (sxlist_size (SXDG.states) + 1);

	XxY_alloc (&SXDG.transitions, "transitions", sxlist_size (SXDG.states)*5, 1, 1, 1,
		   transitions_oflw, NULL);
	SXDG.trans_to_next_state = (SXINT*) sxalloc (XxY_size (SXDG.transitions) + 1, sizeof (SXINT));
	SXDG.trans_has_prdct = sxba_calloc (XxY_size (SXDG.transitions) + 1);

	XxY_alloc (&SXDG.transitionxnext_state, "transitionxnext_state",
		   XxY_size (SXDG.transitions), 1, 0, 1, (sxoflw0_t) NULL, NULL);

	XxY_alloc (&SXDG.item_occurs, "item_occurs", XxY_size (SXDG.transitions), 1, 1, 0,
		   (sxoflw0_t) NULL, NULL);
    }

    if (SXDG.pstack_to_attr == NULL)
    {
	XxYxZ_alloc (&SXDG.parse_stacks, "parse_stacks", sxlist_size (SXDG.states),
		     1, parse_stacks_foreach,
		     parse_stacks_oflw, NULL);
	SXDG.pstack_to_attr = (struct pstack_to_attr*) sxalloc (XxYxZ_size (SXDG.parse_stacks) + 1,
							     sizeof (struct pstack_to_attr));

	SXDG.prdct_stack = SS_alloc (20);
	SXDG.next_state_stack = SS_alloc (50);
	SXDG.item_stack = SS_alloc (100);

	SXDG.ambig_stack = NULL;
    }
}


SXVOID
sxdp_free (void)
{
    if (SXDG.rule_to_rhs_lgth != NULL)
    {
	if (!XxY_is_static (SXDG.P))
	{
	    sxfree (SXDG.rule_to_rhs_lgth), SXDG.rule_to_rhs_lgth = NULL;
	    sxfree (SXDG.rule_to_first_item), SXDG.rule_to_first_item = NULL;
	    sxfree (SXDG.rule_to_action), SXDG.rule_to_action = NULL;
	    sxfree (SXDG.active_rule_set), SXDG.active_rule_set = NULL;

	    if (SXDG.r_priorities != NULL)
		sxfree (SXDG.r_priorities), SXDG.r_priorities = NULL;
	}

	XxY_free (&SXDG.P);

	XxY_free (&SXDG.rhs);

	if (!XxY_is_static (SXDG.items))
	{
	    sxfree (SXDG.item_to_next_item), SXDG.item_to_next_item = NULL;
	    sxfree (SXDG.item_set), SXDG.item_set = NULL;
	}

	XxY_free (&SXDG.items);


	if (!XxY_is_static (SXDG.xt))
	    if (SXDG.t_priorities != NULL)
		sxfree (SXDG.t_priorities), SXDG.t_priorities = NULL;

	XxY_free (&SXDG.xt);

	XxY_free (&SXDG.xnt);  
    }

    if (SXDG.state_to_action != NULL)
    {
	if (!sxlist_is_static (SXDG.states))
	{
	    sxfree (SXDG.state_to_action), SXDG.state_to_action = NULL;
	    sxfree (SXDG.state_to_in_symbol), SXDG.state_to_in_symbol = NULL;
	    sxfree (SXDG.state_to_used_lnk), SXDG.state_to_used_lnk = NULL;
	    sxfree (SXDG.is_state_linked), SXDG.is_state_linked = NULL;
	}

	sxlist_free (&SXDG.states);

	if (!XxY_is_static (SXDG.transitions))
	{
	    sxfree (SXDG.trans_to_next_state), SXDG.trans_to_next_state = NULL;
	    sxfree (SXDG.trans_has_prdct), SXDG.trans_has_prdct = NULL;
	}

	XxY_free (&SXDG.transitions);

	XxY_free (&SXDG.transitionxnext_state);
	XxY_free (&SXDG.item_occurs);

    }

    if (SXDG.pstack_to_attr != NULL)
    {
	if (!XxY_is_static (SXDG.parse_stacks))
	    sxfree (SXDG.pstack_to_attr), SXDG.pstack_to_attr = NULL;

	XxYxZ_free (&SXDG.parse_stacks);

	SS_free (SXDG.prdct_stack);
	SS_free (SXDG.next_state_stack);
	SS_free (SXDG.item_stack);

	if (SXDG.ambig_stack != NULL)
	    sxfree (SXDG.ambig_stack), SXDG.ambig_stack = NULL;
    }
}


static SXVOID
sxdp_G0 (void)
{
    if (*language_name  /* Doit etre positionne' par le "main ()" de l'utilisateur. */ != SXNUL)
    {
	/* La grammaire initiale existe :
	   - soit sous forme de tables compilees (SXDG est partiellement- remplie
	   - soit dans un fichier que l'on va lire
	   */
	if (XxY_top (SXDG.P) == 0)
	{
	    /* Cas de la lecture de la grammaire dynamique initiale*/
	    sxfiledesc_t	file_descr /* file descriptor */ ;
	    char	file_name [128];

	    if ((file_descr = open (strcat (strcpy (file_name, language_name), ".dg"), 0)) < 0) {
		fprintf (sxstderr, "%s: cannot open ", ME);
		sxperror (file_name);
		sxexit (2);
	    }

	    if (!sxdp_read (file_descr))
	    {
		fprintf (sxstderr, "%s: cannot read ", ME);
		sxperror (file_name);
		sxexit (2);
	    }

	    close (file_descr);
	}
	else
	{
	    sxlist_reuse (&SXDG.states, "states", states_oflw, (sxoflw1_t) NULL, NULL);

	    XxY_reuse (&SXDG.P, "P", P_oflw, NULL);
	    XxY_reuse (&SXDG.rhs, "rhs", (sxoflw0_t) NULL, NULL);
	    XxY_reuse (&SXDG.items, "items", items_oflw, NULL);
	    XxY_reuse (&SXDG.transitions, "transitions", transitions_oflw, NULL);
	    XxY_reuse (&SXDG.item_occurs, "item_occurs", (sxoflw0_t) NULL, NULL);
	    XxY_reuse (&SXDG.transitionxnext_state, "transitionxnext_state", (sxoflw0_t) NULL, NULL);
	    XxY_reuse (&SXDG.xt, "xt", xt_oflw, NULL);
	    XxY_reuse (&SXDG.xnt, "xnt", (sxoflw0_t) NULL, NULL);
	}
    }

    sxdp_alloc ((SXINT)0		/* unused */);
}


SXINT
sxdynam_parser (SXINT what_to_do, struct sxtables *arg)
{
    switch (what_to_do) {
    case SXBEGIN:
	sxpglobals.parse_stack = (struct sxparstack*) sxalloc ((SXDG.lgt1 = 200) + 1, sizeof (struct sxparstack));
	break;

    case SXOPEN:
	sxdp_G0 ();
	sxplocals.sxtables = arg;
	sxplocals.SXP_tables = arg->SXP_tables;
	sxtkn_mngr (SXOPEN, 2);
	break;

    case SXINIT:
	sxtkn_mngr (SXINIT, 0);
	/* analyse normale */
	/* valeurs par defaut qui peut etre changee ds les
	   scan_act ou pars_act. */
	SXDG.token_filter = (struct sxtoken * (*) (SXINT)) NULL;
	SXDG.desambig = (SXINT (*) (void)) NULL;
	SXDG.is_in_look_ahead = SXFALSE;
	
	sxplocals.mode.mode = SXPARSER;
	sxplocals.mode.kind = SXWITH_RECOVERY;
	sxplocals.mode.local_errors_nb = 0;
	sxplocals.mode.global_errors_nb = 0;
	sxplocals.mode.look_back = 0; 
	sxplocals.mode.is_prefixe = SXFALSE;
	sxplocals.mode.is_silent = SXFALSE;
	sxplocals.mode.with_semact = SXTRUE;
	sxplocals.mode.with_parsact = SXTRUE;
	sxplocals.mode.with_parsprdct = SXTRUE;
	/* Don't do (and undo) parsing actions in look_ahead */
	sxplocals.mode.with_do_undo = SXFALSE;
	break;
			  
    case SXACTION:
	if (sxdp_parse_it ())
	{
	    /* Si appel recursif (parse_in_la ?) avec le meme SXDG. */
	    erase_subpstack (SXDG.parse_stack);
	    return SXTRUE;
	}    

	if (!sxplocals.mode.is_silent)
	    sxerror (SXGET_TOKEN (sxplocals.ptok_no).source_index, sxplocals.sxtables->err_titles [2][0], "%ssyntax error", sxplocals.sxtables->err_titles [2]+1);

	return SXFALSE;

    case SXFINAL:
	sxtkn_mngr (SXFINAL, 0);
	break;

    case SXCLOSE:
	sxdp_free ();
	sxtkn_mngr (SXCLOSE, 0);
	break;

    case SXEND:
	sxfree (sxpglobals.parse_stack), sxpglobals.parse_stack = NULL;
	break;

    default:
	fprintf (sxstderr, "The function \"sxdynam_parser\" is out of date with respect to its specification.\n");
	sxexit(1);
    }

    return SXTRUE;
}



