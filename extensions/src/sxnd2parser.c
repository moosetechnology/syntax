/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/
/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 

static char	ME [] = "ND2PARSER";

# include <stdio.h>
#include "sxversion.h"
#include "sxunix.h"
# include "sxba.h"
# include "XxY.h"
# include "XxYxZ.h"
# include "SS.h"
# include "sxnd.h"

char WHAT_SXND2PARSER[] = "@(#)SYNTAX - $Id: sxnd2parser.c 2955 2023-03-30 14:20:45Z garavel $" WHAT_DEBUG;

#if EBUG
# define stdout_or_NULL	stdout
#else
# define stdout_or_NULL	NULL
#endif

/************************************************************************/
/*				ESSAI					*/
/*									*/
/* Un parser est un triplet (state, ref, tok_no)			*/
/************************************************************************/

/************************************************************************/
/* It is the nondeterministic version of the SYNTAX parser		*/
/* This algorithm is borrowed from Jan Rekers' thesis			*/
/************************************************************************/

#define PUSH(T,x)	T [x] = T [0], T [0] = x
#define POP(T,x)	x = T [0], T [0] = T [x], T [x] = 0


static SXINT	*rhs_stack, *symbol_stack, *symb_to_rule_hd, *symb_to_rule_list;
static SXBA	symbol_set, hook_symbol_set;


#define Commun 1
#define Propre 2
#define Defaut 3
#define Done   4

SXVOID sxnd2parser_GC ()
{
    /* Attention, le GC peut etre appele' meme s'il reste des places vides. */
    SXINT				i, x, father, son, xarea, index;
    SXBOOLEAN			is_in_sons = SXFALSE;
    struct sxmilstn_elem	*pm;

    if (parse_stack.GC_area == NULL)
	parse_stack.GC_area =
	    (SXINT*) sxalloc ((parse_stack.GC_area_size = XxYxZ_size (parse_stack.parsers)) + 1,
			    sizeof (SXINT));
    else if (parse_stack.GC_area_size < XxYxZ_size (parse_stack.parsers)) {
	parse_stack.GC_area =
	    (SXINT*) sxrealloc (parse_stack.GC_area,
			      (parse_stack.GC_area_size = XxYxZ_size (parse_stack.parsers)) + 1,
			      sizeof (SXINT));
    }

    xarea = 0;
    sxba_fill (parse_stack.free_parsers_set);

    if (parse_stack.rcvr.is_up && (pm = parse_stack.rcvr.milstn_head) != NULL) {
	/* GC () appele depuis le rattrapage d'erreur. */
	do {
	    for (father = pm->next_for_scanner;
		 father > 0;
		 father = parse_stack.parser_to_attr [father].next_for_scanner) {
		if (SXBA_bit_is_set_reset (parse_stack.free_parsers_set, father)) {
		    parse_stack.GC_area [++xarea] = father;
		}
	    }
	} while ((pm = pm->next_milstn) != NULL);
    }

    if ((pm = parse_stack.milstn_head) != NULL) {
	do {
	    for (father = pm->next_for_scanner;
		 father > 0;
		 father = parse_stack.parser_to_attr [father].next_for_scanner) {
		if (SXBA_bit_is_set_reset (parse_stack.free_parsers_set, father)) {
		    parse_stack.GC_area [++xarea] = father;
		}
	    }
	} while ((pm = pm->next_milstn) != NULL);
    }
    
    for (i = 0; i <= for_scanner_mask; i++) {
	for (father = parse_stack.for_scanner.previouses [i];
	     father > 0;
	     father = parse_stack.parser_to_attr [father].next_for_scanner) {
	    if (SXBA_bit_is_set_reset (parse_stack.free_parsers_set, father)) {
		parse_stack.GC_area [++xarea] = father;
	    }
	}
    }
	
    for (x = parse_stack.for_reducer.top; x > 0; x--) {
	if ((father = parse_stack.for_reducer.triples [x].parser) > 0
	    /* Si father est <0 (shift/reduce), le "vrai" parser sera atteint par ailleurs. */
	    && SXBA_bit_is_set_reset (parse_stack.free_parsers_set, father)) {
	    parse_stack.GC_area [++xarea] = father;
	}
    }

    do {
	son = parse_stack.GC_area [xarea--];

	while ((son = parse_stack.parser_to_attr [father = son].son) > 0) {
	    if (!SXBA_bit_is_set_reset (parse_stack.free_parsers_set, son))
		break;
	}

	if (son < 0) {
	    if (!is_in_sons) {
		is_in_sons = SXTRUE;
		sxba_fill (parse_stack.erasable_sons);
	    }

	    son = -son;
	    
	    if (SXBA_bit_is_set_reset (parse_stack.free_parsers_set, son)) {
		parse_stack.GC_area [++xarea] = son;
	    }

	    XxY_Xforeach (parse_stack.sons, father, x) {
		SXBA_0_bit (parse_stack.erasable_sons, x);
		son = XxY_Y (parse_stack.sons, x);

		if (SXBA_bit_is_set_reset (parse_stack.free_parsers_set, son)) {
		    parse_stack.GC_area [++xarea] = son;
		}
	    }
	}
    } while (xarea > 0);

    x = XxY_top (parse_stack.sons) + 1;

    if (is_in_sons) {
	while ((x = sxba_1_rlscan (parse_stack.erasable_sons, x)) > 0)
	    XxY_erase (parse_stack.sons, x);
    }
    else
	while (--x > 0)
	    XxY_erase (parse_stack.sons, x); 

    parse_stack.hole_nb = XxYxZ_size (parse_stack.parsers) - (x = XxYxZ_top (parse_stack.parsers));
    x++;

    while ((x = sxba_1_rlscan (parse_stack.free_parsers_set, x)) > 0) {
	XxYxZ_erase (parse_stack.parsers, x);
	parse_stack.hole_nb++;
    }
}



static  SXVOID parsers_oflw (old_size, new_size)
    SXINT		old_size, new_size;
{
    parse_stack.hole_nb = new_size - old_size;

    parse_stack.parser_to_attr =
	(struct parsers_attr*) sxrealloc (parse_stack.parser_to_attr,
					  new_size + 1,
					  sizeof (struct parsers_attr));
			
    parse_stack.parsers_set = sxbm_resize (parse_stack.parsers_set,
					   5,
					   5,
					   new_size + 1);
    parse_stack.free_parsers_set = parse_stack.parsers_set [4];
}

static  SXVOID sons_oflw (old_size, new_size)
    SXINT		old_size, new_size;
{
    parse_stack.erasable_sons = sxba_resize (parse_stack.erasable_sons, new_size + 1);
    parse_stack.current_links = sxba_resize (parse_stack.current_links, new_size + 1);
    parse_stack.used_links = sxba_resize (parse_stack.used_links, new_size + 1);
    parse_stack.transitions = (SXINT*) sxrealloc (parse_stack.transitions, new_size + 1, sizeof (SXINT));
}

/*

!scan (lascan si < -Mref)                                    scan
------------^-------------\/-----------------------------------^---------------------------------
                          0         Mrednt          Mred    Mprdct              Mfe M0ref  Mref
--------------------------|------------|--------------|-------|------------------|----|------|--->
                           \___________/
                          trans sur nt
                           \__________________________/\______/\_________________________________
                                    reduction           prdct              shift


                                               ^
                                               | acces vector [ref]
                                         Mref  -
					       | Shift: goto t_bases[ref-Mprdct]
                                               |	empiler 0
                                        M0ref  -
					       | Shift: acces [n]t_bases[ref-Mprdct]
                                               |	(etat!=etq)
                                          Mfe  -
                                               | Shift: acces [n]t_bases[ref-Mprdct]
                                               |	(etat==etq)
                                       Mprdct  -
                                               | Prdct: acces Predicates [-ref]
                                               |
                                         Mred  -
                                               | Reduce: acces Reductions [ref]
                                               |
                                       Mrednt  -
                                               | Reduce: acces Reductions [ref]
                                               | (sur transition non terminale)
                                               |
                                            0  - Error
                                              ref


*/

static SXBOOLEAN	set_next_item (vector, check, action, Max)
    struct SXP_item	*vector;
    SXINT			*check, *action, Max;
{
    struct SXP_item	*aitem;
    SXINT			val;

    if ((val = sxplocals.rcvr.mvector - vector) > *check)
	*check = val - 1;

    if ((val = sxplocals.rcvr.Mvector - vector) < Max)
	*check = val;

    for (aitem = vector + ++*check; *check <= Max; aitem++, ++*check)
	if (aitem->check == *check) {
	    *action = aitem->action;
	    return SXTRUE;
	}

    return SXFALSE;
}



SXVOID	set2_first_trans (abase, check, action, Max, next_action_kind)
    struct SXP_bases	*abase;
    SXINT			*check, *action, Max;
    SXINT			*next_action_kind;
{
    SXINT	ref = (SXINT) abase->commun;

    if (ref <= sxplocals.SXP_tables.Mref /* codage direct */ ) {
	if ((*check = (SXINT) abase->propre) != 0) {
	    *action = ref;
	    *next_action_kind = Defaut;
	}
	else /* defaut */  {
	    *action = (SXINT) abase->defaut;
	    *next_action_kind = Done;
	}
    }
    else {
	*check = 0;
	set_next_item (sxplocals.SXP_tables.vector + ref, check, action, Max);
	/* always true (commun non vide) */ 
	*next_action_kind = Commun;
    }
}



SXBOOLEAN	set2_next_trans (abase, check, action, Max, next_action_kind)
    struct SXP_bases	*abase;
    SXINT			*check, *action, Max;
    SXINT			*next_action_kind;
{
    switch (*next_action_kind) {
    case Commun:
	if (set_next_item (sxplocals.SXP_tables.vector + abase->commun, check, action, Max))
	    return SXTRUE;

	if (abase->propre != 0 /* partie propre non vide */ ) {
	    *next_action_kind = Propre;
	    *check = 0;

    case Propre:
	    if (set_next_item (sxplocals.SXP_tables.vector + abase->propre, check, action, Max))
		return SXTRUE;
	}

    case Defaut:
	*check = 0;
	*action = (SXINT) abase->defaut;
	*next_action_kind = Done;
	return SXTRUE;

    default:
	sxtrap ("sxnd2parser", "set2_next_trans");
	  /*NOTREACHED*/

    case Done:
	return SXFALSE;
    }
}



SXINT NDP_access (abase, j)
    struct SXP_bases	*abase;
    SXINT			j;
{
    struct SXP_item	*ajvector, *ajrefvector;
    SXINT			ref = (SXINT) abase->commun;

    if (ref <= sxplocals.SXP_tables.Mref /* codage direct */ ) {
	if ((SXINT) abase->propre == j /* checked */ )
	    return ref;
	else
	    return (SXINT) abase->defaut;
    }

    if ((ajrefvector = (ajvector = sxplocals.SXP_tables.vector + j) + ref)->check == j)
	/* dans commun */
	return (SXINT) ajrefvector->action;

    if ((ref = abase->propre) != 0 /* propre est non vide */ 
	&& (ajrefvector = ajvector + ref)->check == j)
	/* dans propre */
	return (SXINT) ajrefvector->action;

    return (SXINT) abase->defaut;
}



static SXINT	ARC_traversal (ref, latok_no)
    SXINT	ref, latok_no;
{
    SXINT next, first_t_state, t_state;

    /* First scan in LookAhead */
    first_t_state = -(next = ref + sxplocals.SXP_tables.Mref /* next < 0 */);

    if (sxplocals.mode.mode != SXPARSER)
	/* On utilise le non determinisme... */
	ref = 0;
    else do {
	ref = NDP_access (sxplocals.SXP_tables.t_bases + (t_state = -next),
			  sxget_token (++latok_no)->lahead);
	
	if (ref < -sxplocals.SXP_tables.Mred && ref >= -sxplocals.SXP_tables.Mprdct) {
	    /* Execution d'un predicat en look-ahead en mode parser */
	    /* Il n'y a pas de non determinisme ds les ARCs generes. */
	    if (sxplocals.mode.with_parsprdct) {
		struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts - ref;
		SXINT ptok_no;
		
		ptok_no = sxplocals.ptok_no;
		sxplocals.ptok_no = latok_no;
		
		while (aprdct->prdct >= 0 /* User's predicate */  &&
		       (!(*sxplocals.SXP_tables.parsact) (SXPREDICATE, aprdct->
							  prdct /* returning False */ )))
		    aprdct++;
		
		ref = aprdct->action;
		sxplocals.ptok_no = ptok_no;
	    }
	    else {
		/* On ne doit pas executer les predicats, ce choix multiple sera traite
		   comme du non determinisme par l'appelant. */
		break;
	    }
	}
    } while ((next = ref + sxplocals.SXP_tables.Mref) < 0);
    
    /* LookAhead ends */
    if (ref == 0) {
	/* Error in LookAhead */
	/* ref is a sequence of non-deterministic actions. */
	ref = sxplocals.SXP_tables.germe_to_gr_act [first_t_state];
    }

    return ref;
}



static SXBOOLEAN new_symbol (p, k, i, j, symbol)
    SXINT p, k, i, j, *symbol;
{
    SXINT		kind, range;

    if (k > 0) {
	/* Suffixe sharing. */
	XxY_set (&parse_stack.suffixes, p, k, &kind);
	kind += sxplocals.SXP_tables.P_xnbpro;
    }
    else
	kind = p;

    if (i + 1 == j)
	range = -i;
    else
	XxY_set (&parse_stack.ranges, i, j, &range);

    return !XxY_set (&parse_stack.symbols, kind, range, symbol);
}



static void  set_unit_rule (lhs, rhs_symb)
    SXINT		lhs, rhs_symb;
{
    /* Possible ambiguity between different rules (with identical lhs). */
    /* Such a rule is coded with a negative rhs non-terminal. */
    SXINT rhs, rule_no;

    XxY_set (&parse_stack.sf_rhs, rhs_symb, 0, &rhs);
    XxY_set (&parse_stack.sf_rule, lhs, -rhs, &rule_no);
}  

static void  set_rule (lhs, rhs)
    SXINT			lhs, rhs;
{
    SXINT	reduce, rule_no;

    if ((reduce = XxY_X (parse_stack.symbols, lhs) < 0 ? -lhs : lhs)
	> sxplocals.SXP_tables.P_nbpro) {
	if (sxplocals.mode.with_parsact) {
	    /* On doit executer les parsact en cours d'analyse, croisons les doigts */
#if 0
	    /* A FAIRE : donner les moyens aux actions non deterministes d'acceder
	       a leur environnement. */
	    (*sxplocals.SXP_tables.parsact) (SXACTION, parse_stack.semact [reduce]);
#endif
	}
	else {
	    parse_stack.parsact_seen = SXTRUE; /* postponed */
	}
    }

    XxY_set (&parse_stack.sf_rule, lhs, rhs, &rule_no);
}




static SXINT build_a_rule (parser, index, rhs)
    SXINT parser, index, rhs;
{
    SXINT symb;
    
    if (new_symbol (parse_stack.ared->reduce,
		    index,
		    parse_stack.parser_to_attr [parser].level,
		    parse_stack.current_level,
		    &symb))
#if EBUG
	if (index == 0)
	    sxtrap (ME, "build_a_rule")
#endif
		;
    
    set_rule (symb, rhs);
    return symb;
}


static SXVOID  set_start_symbol (symbol)
    SXINT		symbol;
{
    SXINT		symb, rhs, rule_no;
    /* Halt */
    /* Several "final" parse_stack elem could occur. */

    if (parse_stack.start_symbol == 0) {
	new_symbol (0, 0, 0, parse_stack.current_level + 1, &symb);
	parse_stack.start_symbol = symb;
    }

    XxY_set (&parse_stack.sf_rhs, 0, 0, &rhs);
    XxY_set (&parse_stack.sf_rhs, symbol, rhs, &rhs);
    XxY_set (&parse_stack.sf_rhs, 0, rhs, &rhs);

    XxY_set (&parse_stack.sf_rule, parse_stack.start_symbol, rhs, &rule_no);
}



SXINT mreds_push (SXINT parser, SXINT ref)
{
    SXINT			x, i;
    SXINT			*ai;
    struct triple	*top, *cur;

    if ((x = ++parse_stack.for_reducer.top) > parse_stack.for_reducer.size)		
	parse_stack.for_reducer.triples =						
	    (struct triple*) sxrealloc (parse_stack.for_reducer.triples,		
					  (parse_stack.for_reducer.size *= 2) + 1,		
					  sizeof (struct triple));				
    
    top = parse_stack.for_reducer.triples + x;

    /* Il peut y avoir des couples (parser, ref) multiples identiques.
       Ca ne peut se produire que dans le cas ou mreds_push est appele' sur un shift-reduce
       (dans les autres cas, mreds_push n'est appele' que sur un nouveau parser).
       Ex C -> alpha, C -> beta, A -> gamma C. Si le depilage sur alpha et beta conduit
       ds le meme parser p et si l'action est : ref = Shift(C)-Reduce(A -> gamma C), le couple
       (p, ref) sera empile 2 fois. */
    
    if (sxplocals.mode.mode == SXPARSER) {
	/* pour parse_forest () */
	ai = parse_stack.for_reducer.triples_hd + ref;

	if (parse_stack.for_reducer.refs [ref] == 0) {
	    PUSH (parse_stack.for_reducer.refs, ref);
	    parse_stack.for_reducer.shift_reduce_symb [ref] = 0;
	    *ai = 0;
	}

	top->link = *ai;
	*ai = x;
    }
	
    top->parser = parser;

    return parse_stack.for_reducer.top;
}




SXINT seek_parser (son, state, ref, symbol)
    SXINT		son, state, ref, symbol;
{
    /* Si ref est negatif, il s'agit d'une transition non-terminale */
    SXINT				x, father, local_link, index, son_state;
    struct  parsers_attr	*pattr;
    SXBOOLEAN			is_an_old_father, is_active_son;

    /* On note les etats qui ont (au moins) un fils actif. */
    /* goto (son, symbol) = father et symbol =>+ epsilon. */
    /* Ca permet de ne pas oublier do_limited_reductions. */
    
    is_active_son = (ref < 0 /* nt_trans */
		     && parse_stack.parser_to_attr [son].milestone
		     == parse_stack.current_milestone);

    is_an_old_father = XxYxZ_set (&parse_stack.parsers,
				  state,
				  ref,
				  parse_stack.current_token,
				  &father);

    pattr = &(parse_stack.parser_to_attr [father]);

    if (is_an_old_father) {
	/* Add a link from father to son if it does not already exists. */
	if ((x = pattr->son) == son || -x == son) {
	    if (sxplocals.mode.mode == SXPARSER && pattr->symbol != symbol)
		set_unit_rule (pattr->symbol, symbol);
	}
	else {
	    if (x > 0)
		pattr->son = -x;
	    
	    if (XxY_set (&parse_stack.sons, father, son, &local_link)) {
		if (sxplocals.mode.mode == SXPARSER
		    && parse_stack.transitions [local_link] != symbol)
		    set_unit_rule (parse_stack.transitions [local_link], symbol);

		if (is_active_son)
		   pattr->for_do_limited = SXTRUE; 
	    }
	    else {
		parse_stack.transitions [local_link] = symbol;
		
		/* "father" is active and it already exists; furthermore it already has
		   a child which is not "son". */
		/* If "father" (or one of its ancestors) has a reduce action, this action
		   must take "link" as a pop path if relevant.
		   To be sure that such a case is always processed, it will be examined
		   when "father" has an active ancestor or when "father" has already been
		   processed. */
		if (ref < 0  && pattr->is_already_processed) {
		    /* old active parser and new link between father and son */
		    parse_stack.is_new_links = SXTRUE;
		    pattr->for_do_limited = SXTRUE;
		    SXBA_1_bit (parse_stack.current_links, local_link);
		}
	    }
	}
    }
    else {
	pattr->milestone = parse_stack.current_milestone;

	/* add a link from father to son */
	pattr->son = son;
	pattr->level = parse_stack.current_level;
	pattr->symbol = symbol;
	pattr->for_do_limited = is_active_son;
	pattr->is_already_processed = SXFALSE;

	if (ref > 0) {
	    pattr->next_for_scanner = *parse_stack.for_scanner.next_hd;
	    *parse_stack.for_scanner.next_hd = father;
	}
	else {
	    mreds_push (father, -ref);
	}

	if (--parse_stack.hole_nb == 0)
	    sxnd2parser_GC ();
    }

    return father;
}

static SXVOID create_parser (son, state, ref, symbol)
    SXINT		son, state, ref, symbol;
{
    /* ref est non nul. */
    SXINT				father, index;
    struct SXP_prdct		*aprdct;

    if (ref < -sxplocals.SXP_tables.Mref)
	ref = ARC_traversal (ref, sxplocals.ptok_no);
	
    if (ref > 0 /* scan */ ) {
	father = seek_parser (son, state, ref, symbol);
    }
    else {
	ref = -ref;
	
#if EBUG
	if (ref > sxplocals.SXP_tables.Mprdct /* Shift */)
	    sxtrap (ME, "unexpected shift action.");
#endif
	
	if (ref <= sxplocals.SXP_tables.Mred /* reduce/halt */) {
	    if (sxplocals.SXP_tables.reductions [ref].reduce == 0) { /* Halt */
		if (sxplocals.mode.mode == SXPARSER)
		    set_start_symbol (symbol);

		parse_stack.halt_hit = SXTRUE;
	    }
	    else { /* Reduce */
		father = seek_parser (son, state, -ref, symbol);
	    }
	}
	else { /* Predicates or non-determinism */
	    if ((aprdct = sxplocals.SXP_tables.prdcts + ref)->prdct >= 20000) {
		/* non-determinism */
		for (;;) {
		    create_parser (son, state, aprdct->action, symbol);

		    if (aprdct->prdct == 20000)
			break;

		    aprdct++;
		}
	    }
	    else { /* Predicates */
		if (sxplocals.mode.mode == SXPARSER && sxplocals.mode.with_parsprdct
		    || parse_stack.rcvr.with_parsprdct) {
		    /* Configuration ou on doit executer les predicats. */
		    while (aprdct->prdct >= 0 /* User's predicate */  &&
			   (!(*sxplocals.SXP_tables.parsact) (SXPREDICATE, aprdct->
							      prdct /* returning False */ )))
			aprdct++;
	
		    create_parser (son, state, aprdct->action, symbol);
		}
		else {
		    /* Traites comme le non determinisme... */
		    if (sxplocals.mode.mode == SXPARSER)
			/* La foret partagee devra etre parcourue (meme si c'est un arbre)
			   pour executer les pars(act|prdct) et y eliminer des branches. */
			parse_stack.parsprdct_seen = SXTRUE;

		    for (;;) {
			if (aprdct->action)
			    create_parser (son, state, aprdct->action, symbol);

			if (aprdct->prdct < 0)
			    break;

			aprdct++;
		    }
		}
	    }
	}
    }
}



static SXVOID do_reductions ();

static SXVOID do_pops (p, l)
    SXINT		p, l;
{
    /* We want to perform a reduce_action A -> alpha on p. */
    /* l = length (alpha). */
    /* p is a parse_stack_elem. */
    SXINT			x, son, father, next_son, first_son;
    SXBA		fathers_set, sons_set;
    SXBOOLEAN		is_in_set;

    is_in_set = SXFALSE;
    son = p;

    while (l-- > 0) {
	father = son;

	if (!is_in_set) {
	    if ((son = parse_stack.parser_to_attr [father].son) < 0) {
		sons_set = (l & 01) ?
		    parse_stack.parsers_set [1] : parse_stack.parsers_set [0];

		XxY_Xforeach (parse_stack.sons, father, x) {
		    next_son = XxY_Y (parse_stack.sons, x);
		    SXBA_1_bit (sons_set, next_son);
		}

		son = -son;
		is_in_set = SXTRUE;
	    }
	}
	else {
	    is_in_set = SXFALSE;

	    if (l & 01) {
		fathers_set = parse_stack.parsers_set [0];
		sons_set = parse_stack.parsers_set [1];
	    }
	    else {
		fathers_set = parse_stack.parsers_set [1];
		sons_set = parse_stack.parsers_set [0];
	    }

	    if ((son = parse_stack.parser_to_attr [father].son) < 0) {
		is_in_set = SXTRUE;
		son = -son;

		XxY_Xforeach (parse_stack.sons, father, x) {
		    next_son = XxY_Y (parse_stack.sons, x);
		    SXBA_1_bit (sons_set, next_son);
		}
	    }

	    father = 0;
	    
	    while ((father = sxba_scan_reset (fathers_set, father)) > 0) {
		if ((first_son = parse_stack.parser_to_attr [father].son) < 0) {
		    is_in_set = SXTRUE;
		    first_son = -first_son;
		    
		    XxY_Xforeach (parse_stack.sons, father, x) {
			if ((next_son = XxY_Y (parse_stack.sons, x)) != son)
			    SXBA_1_bit (sons_set, next_son);
		    }
		}

		if (first_son != son) {
		    SXBA_1_bit (sons_set, first_son);
		    is_in_set = SXTRUE;
		}
	    }
	}
    }

    do_reductions (son, is_in_set);
}


static SXVOID seek_paths ();

SXVOID sxndsubparse_a_token (parser, ref, symbol, do_shift_reduce)
    SXINT		parser, ref, symbol;
    SXBOOLEAN	do_shift_reduce;
{
    SXINT			state, lgth, rhs, t_trans, next_action_kind, dum_parser;
    SXINT			*pref;
    struct SXP_bases	*abase;

    /* do_shift_reduce ne doit etre vrai que dans le cas scan-reduce, sinon il y a
       des possibilites d'appels recursifs infinis dans le cas ou un shift-reduce +
       pop ramene ds le meme etat avec la meme ref. Ce cas peut se produire car le
       traitement des erreurs globales peut induire des cycles (non vides) dans parsers
       Ex : A -> t A et etat [A -> t . A] ds p et goto (p, t) = p et
       goto (p, A) = shift-reduce (p->tA) */

#if EBUG
    parse_stack.shift_trans_nb++;
#endif    
    
    if (ref <= sxplocals.SXP_tables.Mred) {
	/* Shift|scan-Reduce */
	if (do_shift_reduce) {
	    /* les variables statiques doivent etre OK on le traite on the fly. */
	    do_pops (parser,
		     lgth = (parse_stack.ared = sxplocals.SXP_tables.reductions + ref)->lgth);
	    
	    if (sxplocals.mode.mode == SXPARSER) {
		XxY_set (&parse_stack.sf_rhs, symbol, 0, &rhs);
		/* On repositionne parse_stack.ared qui a pu etre reaffecte' par un
		   appel recursif de do_pops. */
		parse_stack.ared = sxplocals.SXP_tables.reductions + ref;
		seek_paths (parser, rhs, SXFALSE, lgth);
	    }
	}
	else {
	    if (!XxYxZ_set (&parse_stack.shift_reduce, parser, ref,
			    parse_stack.current_token, &dum_parser))
	      mreds_push (-dum_parser, ref);
	    
	    if (sxplocals.mode.mode == SXPARSER) {
		if (*(pref = parse_stack.for_reducer.shift_reduce_symb + ref) == 0) {
		    /* Pour parse_forest */
		    *pref = symbol;
		}
		else if (*pref != symbol)
		    set_unit_rule (*pref, symbol);
	    }
	}
    }
    else {
	state = ref - sxplocals.SXP_tables.Mprdct;
	abase = sxplocals.SXP_tables.t_bases + state;	
	
	if (sxplocals.mode.mode != SXGENERATOR
	    || parse_stack.current_token != parse_stack.delta_generator) {
	    /* SXPARSER or SXRECOGNIZER */
	    if ((ref = NDP_access (abase, parse_stack.current_lahead)) != 0)
		create_parser (parser, state, ref, symbol);
	}
	else {
	    /* sxplocals.mode.mode == SXGENERATOR && parse_stack.current_token == 0 */
	    /* Le token sur lequel on fait la generation n'a pas encore ete determine'
	       par le contexte. */
	    set2_first_trans (abase, &t_trans, &ref,
			     (SXINT) sxplocals.SXP_tables.P_tmax, &next_action_kind);
	    
	    while (ref != 0) {
		/* Dans le cas SXGENERATOR, current_token n'est pas un numero de token
		   (on traite un X, il n'y a pas de token correspondant) mais directement
		   le look_ahead (en negatif). On suppose que l'appelant peut se debrouiller
		   avec ca... */
		/* Si le defaut est un reduce, t_trans == 0, les bons terminaux seront
		   determines plus tard. */
		parse_stack.current_token = parse_stack.delta_generator - t_trans;
		parse_stack.current_lahead = t_trans;

		create_parser (parser, state, ref, symbol);
		
		if (!set2_next_trans (abase, &t_trans, &ref,
				     sxplocals.SXP_tables.P_tmax, &next_action_kind))
		    break;
	    }
	}
    }
}
    
    
static SXVOID do_reductions (son, is_in_set)
    SXINT		son;
    SXBOOLEAN	is_in_set;
{
    /* We want to perform goto (son, A) where A is the lhs of the
       reduced production. */
    SXINT				ref, state, next_son, symb, lhs; 

    /* Dynamic construction of the mapping:
          rule_no -> action_no
       MUST BE STATIC IN A FUTURE RELEASE */

    parse_stack.semact [parse_stack.ared->reduce] = parse_stack.ared->action;
    next_son = 0;

    do {
	if (sxplocals.mode.mode == SXPARSER)
	    new_symbol (parse_stack.ared->reduce,
			0,
			parse_stack.parser_to_attr [son].level,
			parse_stack.current_level,
			&symb);
	else
	    symb = 0;

	if ((lhs = (ref = parse_stack.ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
	    /* pas branchement direct */
	    state = XxYxZ_X (parse_stack.parsers, son);

#if EBUG
	    if (state > sxplocals.SXP_tables.M0ref - sxplocals.SXP_tables.Mprdct)
		sxtrap (ME, "do_reductions");
#endif
	    ref = NDP_access (sxplocals.SXP_tables.nt_bases + state, lhs);
	}

	sxndsubparse_a_token (son, -ref, symb, SXFALSE);
    } while (is_in_set
	     && (son = next_son = sxba_scan_reset (parse_stack.parsers_set [0], next_son)) > 0);
}



static SXVOID do_limited_reductions (p, l)
    SXINT p, l;
{
    /* Traque les reduces qui passent par certaines transitions stockees dans used_links. */
    /* called with l > 0 */
    SXINT	x, father, son, first_father;
    SXBOOLEAN		is_link, is_sons;
    SXBA	fathers_set, sons_set, fathers_link_set, sons_link_set;
    SXBA		s;

    /* All parsers_set are supposed to be empty. */
    x = l & 01;
    fathers_set = parse_stack.parsers_set [x];
    fathers_link_set = parse_stack.parsers_set [x+2];
    x = (x + 1) & 01;
    sons_set = parse_stack.parsers_set [x];
    sons_link_set = parse_stack.parsers_set [x+2];
    SXBA_1_bit (fathers_set, p);

    while (--l >= 0) {
	father = 0;
	
	while ((father = sxba_scan_reset (fathers_set, father)) > 0) {
	    is_link = SXBA_bit_is_set_reset (fathers_link_set, father);
	    
	    if ((is_sons = (son = parse_stack.parser_to_attr [father].son) < 0))
		son = -son;
	    
	    if (l > 0 || is_link)
		SXBA_1_bit (sons_set, son);

	    if (l > 0 && is_link)
		SXBA_1_bit (sons_link_set, son);
	    
	    if (is_sons) {
		XxY_Xforeach (parse_stack.sons, father, x) {
		    son = XxY_Y (parse_stack.sons, x);

		    if (l > 0 || is_link || SXBA_bit_is_set (parse_stack.used_links, x))
			SXBA_1_bit (sons_set, son);

		    if (l > 0 && (is_link || SXBA_bit_is_set (parse_stack.used_links, x)))
			SXBA_1_bit (sons_link_set, son);
		}
	    }
	}

	/* fathers_set and fathers_link_set are empty */

	if (l > 0) {
	    s = fathers_set, fathers_set = sons_set, sons_set = s;
	    s = fathers_link_set, fathers_link_set = sons_link_set, sons_link_set = s;
	}
    }
    /* sons_link_set is empty */
	
    if ((son = sxba_scan_reset (sons_set, 0)) > 0) {
	do_reductions (son, sxba_scan (sons_set, son) > 0);
    }
}


static SXVOID seek_paths (p, p_trans, is_in_set, l)
    SXINT		p, p_trans, l;
    SXBOOLEAN	is_in_set;
{
    SXINT		x, son, father, first_son;
    SXBA	fathers_set, sons_set, s;
    SXINT		*ptr, *son_rhs_ptr;
    SXINT		son_i, father_i, father_rhs_hd, rhs, first_son_rhs_hd, son_rhs_hd;
    SXBOOLEAN	is_sons, is_first_time, is_in_sons_set;

#if EBUG
    if (p_trans != 0)
	parse_stack.red_trans_nb += 1 + (is_in_set
					 ? sxba_cardinal (parse_stack.parsers_set [1])
					 : 0);
#endif    

    if (l == 0) {
	build_a_rule (p, 0, p_trans);

	if (is_in_set) {
	    son = 0;
	    sons_set = parse_stack.parsers_set [1];

	    while ((son = sxba_scan_reset (sons_set, son)) > 0) {
		build_a_rule (son, 0, p_trans);
	    }
	}

	return;
    }
    
    fathers_set = parse_stack.parsers_set [0];
    father_i = 0;

    sons_set = parse_stack.parsers_set [1];
    son_i = 1;

    if (is_in_set) {
	son = 0;

	while ((son = sxba_scan (sons_set, son)) > 0)
	    parse_stack.parser_to_attr [son].rhs_i [son_i] = p_trans;
    }

    first_son = p;
    first_son_rhs_hd = p_trans;

    /* lorsqu'on a de'pile' de la bonne longueur, les "fils" sont correctement
       positionnes. */
    while (l-- > 0) {
	/* Les fils du coup precedent deviennent les peres du coup suivant. */
	s = fathers_set, fathers_set = sons_set, sons_set = s;
	x = father_i, father_i = son_i, son_i = x;

	father = first_son;

	if (first_son_rhs_hd < 0)
	    /* C'est un NT et pas une RHS. */
	    XxY_set (&parse_stack.sf_rhs, -first_son_rhs_hd, 0, &father_rhs_hd);
	else
	    father_rhs_hd = first_son_rhs_hd;

	first_son_rhs_hd = 0;
	is_first_time = SXTRUE;
	is_in_sons_set = SXFALSE;

	for (;;) {
#if EBUG
	    parse_stack.red_trans_nb++;    
#endif    

	    if ((is_sons = (son = parse_stack.parser_to_attr [father].son) < 0))
		son = -son;
	    
	    if (is_first_time)
		first_son = son;
	
	    XxY_set (&parse_stack.sf_rhs,
		     parse_stack.parser_to_attr [father].symbol,
		     father_rhs_hd,
		     &rhs);

	    if (first_son == son)
		son_rhs_ptr = &first_son_rhs_hd;
	    else {
		is_in_sons_set = SXTRUE;
		son_rhs_ptr = &(parse_stack.parser_to_attr [son].rhs_i [son_i]);

		if (SXBA_bit_is_reset_set (sons_set, son))
		    *son_rhs_ptr = 0;
	    }
	    
	    if (*son_rhs_ptr == 0)
		*son_rhs_ptr = rhs;	
	    else {
		if (*son_rhs_ptr > 0) {
		    *son_rhs_ptr = -build_a_rule (son, l, *son_rhs_ptr);
		}
		
		set_rule (-*son_rhs_ptr, rhs);
	    }

	    if (is_sons) {
		XxY_Xforeach (parse_stack.sons, father, x) {
#if EBUG
		    parse_stack.red_trans_nb++;    
#endif    
		    son = XxY_Y (parse_stack.sons, x);
		    XxY_set (&parse_stack.sf_rhs,
			     parse_stack.transitions [x],
			     father_rhs_hd,
			     &rhs);

		    if (first_son == son)
			son_rhs_ptr = &first_son_rhs_hd;
		    else {
			is_in_sons_set = SXTRUE;
			son_rhs_ptr = &(parse_stack.parser_to_attr [son].rhs_i [son_i]);

			if (SXBA_bit_is_reset_set (sons_set, son))
			    *son_rhs_ptr = 0;
		    }
		    
		    if (*son_rhs_ptr == 0)
			*son_rhs_ptr = rhs;	
		    else {
			if (*son_rhs_ptr > 0) {
			    *son_rhs_ptr = -build_a_rule (son, l, *son_rhs_ptr);
			}
			
			set_rule (-*son_rhs_ptr, rhs);
		    }
		}
	    }

	    if (is_first_time) {
		is_first_time = SXFALSE;
		father = 0;
	    }

	    if (is_in_set && (father = sxba_scan_reset (fathers_set, father)) > 0) {
		if ((father_rhs_hd = parse_stack.parser_to_attr [father].rhs_i [father_i]) < 0)
		   /* C'est un NT et pas une RHS. */
		    XxY_set (&parse_stack.sf_rhs, -father_rhs_hd, 0, &father_rhs_hd); 
	    }
	    else
		break;
	}

	is_in_set = is_in_sons_set;
    }

    if (first_son_rhs_hd > 0) {
	build_a_rule (first_son, 0, first_son_rhs_hd);
    }

    if (is_in_set) {
	son = 0;

	while ((son = sxba_scan_reset (sons_set, son)) > 0) {
	    if ((son_rhs_hd = parse_stack.parser_to_attr [son].rhs_i [son_i]) > 0) {
		build_a_rule (son, 0, son_rhs_hd);
	    }
	}
    }
}


static SXVOID parse_forest ()
{
    /* The construction of the parse forest is not performed within the
       recognizer in order to avoid the multiple definition of the same
       sub-string with different productions. This may arise when a new
       link is added between previous parsers, triggering a future call
       to do_limited_reductions and adds a path such that a node with a
       single father becomes multi_fathered and hence produces an inter
       mediate non-terminal which describes a path which has already be
       en processed previously. One solution is to have a two step proc
       ess: first, the recognizer which set all the pathes and the non-
       terminals which are the names of the transitions and second, the
       creation of the parse forest (ie the grammar rules) which uses
       the previous transitions and pathes. */
    SXINT 		ref;
    SXINT			p, x, parser, symbol, rhs;
    SXBOOLEAN		is_set;
    SXBA		parser_set;
    struct triple	*triple;

    while ((ref = parse_stack.for_reducer.refs [0]) > 0) {
	parse_stack.for_reducer.refs [0] = parse_stack.for_reducer.refs [ref];
	parse_stack.for_reducer.refs [ref] = 0;
	triple = parse_stack.for_reducer.triples + parse_stack.for_reducer.triples_hd [ref];

	if ((parser = triple->parser) < 0)
	    parser = XxYxZ_X (parse_stack.shift_reduce, -parser);

	if ((x = triple->link) > 0) {
	    is_set = SXTRUE;
	    parser_set = parse_stack.parsers_set [1];

	    do {
		triple = parse_stack.for_reducer.triples + x;

		if ((p = triple->parser) < 0)
		    p = XxYxZ_X (parse_stack.shift_reduce, -p);

		SXBA_1_bit (parser_set, p);
	    } while ((x = triple->link) > 0);
	}
	else
	   is_set = SXFALSE; 

	if ((symbol = parse_stack.for_reducer.shift_reduce_symb [ref]) == 0)
	    rhs = 0;
	else
	    XxY_set (&parse_stack.sf_rhs, symbol, 0, &rhs);

	seek_paths (parser, rhs, is_set,
			(parse_stack.ared = sxplocals.SXP_tables.reductions + ref)->lgth);
    }
}


SXVOID reducer ()
{
    /* new_top est non nul et designe la tete de la liste qui parcourt les
       active parsers. */
    SXINT		p, ref, lgth, x, bot, cur_top, xsr;

    bot = 1;
    
    while ((cur_top = parse_stack.for_reducer.top) >= bot) {
	for (x = bot; x <= cur_top; x++) {
	    if ((xsr = parse_stack.for_reducer.triples [x].parser) > 0) {
		p = xsr;
		ref = -XxYxZ_Y (parse_stack.parsers, xsr);
	    }
	    else {
		p = XxYxZ_X (parse_stack.shift_reduce, -xsr);
		ref = XxYxZ_Y (parse_stack.shift_reduce, -xsr);
	    }

	    if (sxplocals.mode.mode == SXGENERATOR) {
		parse_stack.current_token = xsr > 0
		    ? XxYxZ_Z (parse_stack.parsers, xsr)
			: XxYxZ_Z (parse_stack.shift_reduce, -xsr);
		parse_stack.current_lahead = (parse_stack.current_token <= 0)
		    ? parse_stack.delta_generator - parse_stack.current_token
			: SXGET_TOKEN (parse_stack.current_token).lahead;
	    }

	    parse_stack.parser_to_attr [p].is_already_processed = SXTRUE;
	    do_pops (p, (parse_stack.ared = sxplocals.SXP_tables.reductions + ref)->lgth);
	}
	
	if (parse_stack.is_new_links) {
	    do {
		parse_stack.is_new_links = SXFALSE;
		/* Les seuls candidats a un do_limited_reductions sont :
		   - les tetes de liens de "used_links"
		   - les parsers ayant un descendant actif. */
		sxba_copy (parse_stack.used_links, parse_stack.current_links);
		sxba_empty (parse_stack.current_links);

		for (x = 1; x <= cur_top; x++) {
		    if ((xsr = parse_stack.for_reducer.triples [x].parser) > 0) {
			p = xsr;
			ref = -XxYxZ_Y (parse_stack.parsers, xsr);
		    }
		    else {
			p = XxYxZ_X (parse_stack.shift_reduce, -xsr);
			ref = XxYxZ_Y (parse_stack.shift_reduce, -xsr);
		    }

		    if (parse_stack.parser_to_attr [p].for_do_limited) {
			if ((lgth = (parse_stack.ared =
				    sxplocals.SXP_tables.reductions + ref)->lgth) > 0) {
			    if (sxplocals.mode.mode == SXGENERATOR) {
				parse_stack.current_token = xsr > 0
				    ? XxYxZ_Z (parse_stack.parsers, xsr)
					: XxYxZ_Z (parse_stack.shift_reduce, -xsr);
				parse_stack.current_lahead = (parse_stack.current_token <= 0)
				    ? parse_stack.delta_generator - parse_stack.current_token
					: SXGET_TOKEN (parse_stack.current_token).lahead;
			    }
			
			    do_limited_reductions (p, lgth);
			}
		    }
		}
	    } while (parse_stack.is_new_links);
	}
	
	bot = cur_top + 1;
    }

    if (sxplocals.mode.mode == SXPARSER)
	parse_forest ();
    
    if (XxYxZ_top (parse_stack.shift_reduce) > 0)
	XxYxZ_clear (&parse_stack.shift_reduce);

    parse_stack.for_reducer.top = 0;
}



static SXVOID sxndparse_a_token (symbol)	
    SXINT symbol;
{
    SXINT			parser;
    
    *parse_stack.for_scanner.next_hd = 0;

    for (parser = *parse_stack.for_scanner.current_hd;
	 parser > 0;
	 parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
	sxndsubparse_a_token (parser,
			      XxYxZ_Y (parse_stack.parsers, parser),
			      symbol,
			      SXTRUE);
    }

    if (parse_stack.for_reducer.top > 0)
	reducer ();
}


static SXVOID grammar_new_rule (lhs)
    SXINT lhs;
{
    if (++parse_stack.rule_top > parse_stack.rule_size) {
	parse_stack.rule = (struct rule*) sxrealloc (parse_stack.rule,
					      (parse_stack.rule_size *= 2) + 1,
					      sizeof (struct rule));
	symb_to_rule_list = (SXINT*) sxrealloc (symb_to_rule_list,
					 parse_stack.rule_size + 1,
					 sizeof (SXINT));
    }

    parse_stack.rule [parse_stack.rule_top].hd = parse_stack.grammar_top + 1;
    parse_stack.rule [parse_stack.rule_top].lgth = 0;

    if (lhs != 0) {
	symb_to_rule_list [parse_stack.rule_top] = symb_to_rule_hd [lhs];
	symb_to_rule_hd [lhs] = parse_stack.rule_top;
    }
}

static SXVOID grammar_put (symbol)
	SXINT symbol;
{
    if (++parse_stack.grammar_top > parse_stack.grammar_size) {
	parse_stack.grammar = (SXINT*) sxrealloc (parse_stack.grammar,
					      (parse_stack.grammar_size *= 2) + 1,
					      sizeof (SXINT));
    }

    parse_stack.rule [parse_stack.rule_top].lgth++;
    parse_stack.grammar [parse_stack.grammar_top] = symbol;
}


static SXVOID expand_rule (tnt)
    SXINT tnt;
{
    /* "rhs_stack" is an SS_stack and contains a list of symbols which designates
       a prefixe of a rule. */
    /* tnt is the most rhs symbol */
    /* Appends the "expansion" of tnt. */
    SXINT item, old_top, x, rhs, rule_no;

    old_top = SS_top (rhs_stack);

    if (tnt > 0 &&
	XxY_X (parse_stack.symbols, tnt) > sxplocals.SXP_tables.P_xnbpro) {

	XxY_Xforeach (parse_stack.sf_rule, tnt, rule_no) {
	    rhs = XxY_Y (parse_stack.sf_rule, rule_no);
	    tnt = 0;

	    while (rhs > 0) {
		tnt = XxY_X (parse_stack.sf_rhs, rhs);
		rhs = XxY_Y (parse_stack.sf_rhs, rhs);

		if (rhs > 0
		    || tnt == 0 /* eof de fin ou error (fin de rhs) */) {
		    SS_push (rhs_stack, tnt);

		    if (tnt > 0 && SXBA_bit_is_reset_set (symbol_set, tnt)) {
			PUSH (symbol_stack, tnt);
		    }
		}
	    }

	    expand_rule (tnt);
	    SS_top (rhs_stack) = old_top;
	}
    }
    else {
	x = SS_bot (rhs_stack);
	grammar_new_rule (SS_get (rhs_stack, x));

	while (x < old_top) {
	    grammar_put (SS_get (rhs_stack, x));
	    x++;
	}

	if (tnt != 0) {
	    grammar_put (tnt);

	    if (tnt > 0 && SXBA_bit_is_reset_set (symbol_set, tnt)) {
		PUSH (symbol_stack, tnt);
	    }
	}
    }
}


static SXVOID hook_construction (symbol)
    SXINT symbol;
{
    SXINT rule, lhs, rhs, tnt;
    SXINT	*aelem, *lim;

    PUSH (symbol_stack, symbol);
    sxba_empty (symbol_set);
    SXBA_1_bit (symbol_set, symbol);
    parse_stack.is_ambiguous = SXTRUE;
    grammar_new_rule (0); /* On ne touche pas a symb_to_rule */
    grammar_put (symbol);

    do {
	POP (symbol_stack, lhs);

	if ((rule = symb_to_rule_hd [lhs]) < parse_stack.hook_rule) {
	    do {
		grammar_put (rule);
		rule = symb_to_rule_list [rule];
	    } while (rule > 0);

	    XxY_Xforeach (parse_stack.sf_rule, lhs, rule) {
		if ((rhs = XxY_Y (parse_stack.sf_rule, rule)) < 0) {
		    /* unit production */
		    tnt = XxY_X (parse_stack.sf_rhs, -rhs);
		    
		    if (SXBA_bit_is_reset_set (symbol_set, tnt)) {
			PUSH (symbol_stack, tnt);
		    }
		}
	    }
	}
	else {
	    /* Already processed as a hook. */
	    aelem = parse_stack.grammar + parse_stack.rule [rule].hd;
	    lim = aelem + parse_stack.rule [rule].lgth;

	    while (++aelem < lim) {
		grammar_put (*aelem);
	    }
	}
    } while (symbol_stack [0] > 0);
	
    symb_to_rule_hd [symbol] = parse_stack.rule_top;
}



static SXVOID unused_rule_elimination ()
{
    SXINT		lhs_symbol, rule, item, tnt, lim, symbol, pos, rhs, nb;
    SXINT		*aelem;

    parse_stack.rule_size = XxY_top (parse_stack.sf_rule) + 1;

    symb_to_rule_hd =
	(SXINT*) sxcalloc (XxY_top (parse_stack.symbols) + 1, sizeof (SXINT));

    symb_to_rule_list =
	(SXINT*) sxalloc (parse_stack.rule_size + 1, sizeof (SXINT));

    parse_stack.rule =
	(struct rule*) sxalloc (parse_stack.rule_size + 1, sizeof (struct rule));
    parse_stack.rule_top = 0;

    parse_stack.grammar =
	(SXINT*) sxalloc ((parse_stack.grammar_size = 4 * parse_stack.rule_size) + 1,
			sizeof (SXINT));
    parse_stack.grammar_top = 0;


    /* "root" creation */
    grammar_new_rule (0);
    grammar_put (0);
    grammar_put (parse_stack.start_symbol);
    parse_stack.root = parse_stack.rule_top; 

    rhs_stack = SS_alloc (20);
    symbol_stack = (SXINT*) sxcalloc (XxY_top (parse_stack.symbols) + 1, sizeof (SXINT));
    symbol_stack [0] = -1;
    symbol_set = sxba_calloc (XxY_top (parse_stack.symbols) + 1);
    hook_symbol_set = sxba_calloc (XxY_top (parse_stack.symbols) + 1);

    PUSH (symbol_stack, parse_stack.start_symbol);
    SXBA_1_bit (symbol_set, parse_stack.start_symbol);

    do {
	POP (symbol_stack, lhs_symbol);
	nb = 0;

	XxY_Xforeach (parse_stack.sf_rule, lhs_symbol, rule) {
	    nb++;

	    if ((rhs = XxY_Y (parse_stack.sf_rule, rule)) < 0) {
		/* unit production */
		tnt = XxY_X (parse_stack.sf_rhs, -rhs);

		if (SXBA_bit_is_reset_set (symbol_set, tnt)) {
		    PUSH (symbol_stack, tnt);
		}
	    }
	    else {
		SS_clear (rhs_stack);
		SS_push (rhs_stack, lhs_symbol);
		tnt = 0;

		while (rhs > 0) {
		    tnt = XxY_X (parse_stack.sf_rhs, rhs);
		    rhs = XxY_Y (parse_stack.sf_rhs, rhs);

		    if (rhs > 0
			|| tnt == 0 /* eof de fin ou error (fin de rhs) */) {
			SS_push (rhs_stack, tnt);
			
			if (tnt > 0 && SXBA_bit_is_reset_set (symbol_set, tnt)) {
			    PUSH (symbol_stack, tnt);
			}
		    }
		}

		expand_rule (tnt);
	    }
	}

	if (nb > 1)
	    SXBA_1_bit (hook_symbol_set, lhs_symbol);
    } while (symbol_stack [0] > 0);
	
    parse_stack.hook_rule = parse_stack.rule_top + 1;
    parse_stack.is_ambiguous = SXFALSE;

    symbol = 0;
    
    while ((symbol = sxba_scan (hook_symbol_set, symbol)) > 0) {
	hook_construction (symbol);
    }
    
    sxfree (symbol_stack);
    SS_free (rhs_stack);
    sxfree (symbol_set);
    sxfree (hook_symbol_set);

    parse_stack.positions = (SXINT*) sxalloc (parse_stack.grammar_top + 1,
					    sizeof (SXINT));

    for (rule = 1;
	 rule < parse_stack.hook_rule;
	 rule++) {
	item = parse_stack.rule [rule].hd;
	aelem = parse_stack.grammar + item;
	lim = item + parse_stack.rule [rule].lgth;
	parse_stack.positions [item] = pos = 0;

	while (++item < lim) {
	    parse_stack.positions [item] = ++pos;
	    aelem++;

	    if (*aelem > 0)
		*aelem = symb_to_rule_hd [*aelem];
	}
    }

    sxfree (symb_to_rule_hd);
    sxfree (symb_to_rule_list);

    parse_stack.start_symbol =
	parse_stack.grammar [parse_stack.rule [parse_stack.root].hd + 1];
}



#ifdef LDBG
static SXVOID output_symbol (symbol)
    SXINT symbol;
{
    SXINT reduce, range, i, j, k;
    char c;

    reduce = XxY_X (parse_stack.symbols, symbol);

    if ((k = reduce - sxplocals.SXP_tables.P_xnbpro) > 0) {
	reduce = XxY_X (parse_stack.suffixes, k);
	k = XxY_Y (parse_stack.suffixes, k);
    }
    else
	k = 0;

    range = XxY_Y (parse_stack.symbols, symbol);

    if (range < 0) {
	i = -range;
	j = i + 1;
    }
    else {
	i = XxY_X (parse_stack.ranges, range);
	j = XxY_Y (parse_stack.ranges, range);
    }

    /* Si le scanner est non-deterministe, les bornes [a..b] de range designent des
       milestones donc des reperes dans le texte source et en fait un segment du source. */
    c = sxplocals.sxtables->SXS_tables.S_is_non_deterministic ? ']' : '[' ;

    if (k == 0)
	printf ("<%i, [%i .. %i%c> ",
		reduce,
		i,
		j,
		c);
    else
	printf ("<%i, %i, [%i .. %i%c> ",
		reduce,
		k,
		i,
		j,
		c);
    
}



static SXVOID output_shared_forest ()
{
    SXINT		rule, lhs, rhs, symbol;
    struct sxtoken	*atok;

    fputs ("*\tUnprocessed shared parse forest denotation.\n\n", stdout);

    if (parse_stack.start_symbol == 0) {
	fputs ("*\tThere is no start symbol in this shared parse forest denotation.\n\n", stdout);
/*	return; */
    }
    else {
	fputs ("*\tThe start symbol is ", stdout);
	output_symbol (parse_stack.start_symbol);
	fputs (".\n\n", stdout);
    }

    for (rule = 1; rule <= XxY_top (parse_stack.sf_rule); rule++) {
	lhs = XxY_X (parse_stack.sf_rule, rule);
	rhs = XxY_Y (parse_stack.sf_rule, rule);
	
	if (rhs < 0) {
	    fputs ("*\tunit rule\n", stdout);
	    rhs = -rhs;
	}
	
	printf ("%i\t: ", rule);
	output_symbol (lhs);
	fputs ("\t= ", stdout);
	
	while (rhs > 0) {
	    symbol = XxY_X (parse_stack.sf_rhs, rhs);
	    
	    if (symbol > 0)
		output_symbol (symbol);
	    else if (symbol == 0) {
		/* Eof ou Error */
		if (XxY_X (parse_stack.symbols, lhs) == 0)
		    /* Initial EoF */
		    printf ("\"%s\" ", sxttext (sxplocals.sxtables, SXGET_TOKEN (0).lahead));
		else
		    fputs ("Error ", stdout);
	    }
	    else {
		symbol = -symbol;
		atok = &(SXGET_TOKEN (symbol));
		
		if (sxgenericp (sxplocals.sxtables, atok->lahead)
		    && atok->string_table_entry != SXERROR_STE)
		    printf ("\"%s\" ", sxstrget (atok->string_table_entry));
		else 
		    printf ("\"%s\" ", sxttext (sxplocals.sxtables, atok->lahead));
	    }
	    
	    rhs = XxY_Y (parse_stack.sf_rhs, rhs)   ;
	}
	
	fputs (";\n", stdout);
    }
    
    fputs ("\n\n", stdout);
}
#endif

    
    
#if EBUG
static SXVOID output_rule (rule)
    SXINT rule;
{
    SXINT item, symbol, range, lim, i, j, reduce;
    struct sxtoken	*atok;

    if (rule == parse_stack.root)
	printf ("%i\t: <start_symbol, [0 .. %i]>\t= <%i> ;\n",
		rule,
		parse_stack.current_level + 1,
		parse_stack.start_symbol);
    else {
	item = parse_stack.rule [rule].hd;
	symbol = parse_stack.grammar [item];
	reduce = XxY_X (parse_stack.symbols, symbol);
	range = XxY_Y (parse_stack.symbols, symbol);

	if (range < 0) {
	    i = -range;
	    j = i + 1;
	}
	else {
	    i = XxY_X (parse_stack.ranges, range);
	    j = XxY_Y (parse_stack.ranges, range);
	}

	if (i != j) {
	    if (!sxplocals.sxtables->SXS_tables.S_is_non_deterministic)
		j--;

	    printf ("%i\t: <%s %i, [%i .. %i]>\t= ",
		    rule,
		    rule >= parse_stack.hook_rule ? "hook" : "reduce",
		    reduce,
		    i,
		    j);
	}
	else
	    printf ("%i\t: <reduce %i, void (before %i)>\t= ",
		    rule,
		    reduce,
		    i);
	
	lim = item + parse_stack.rule [rule].lgth;
	
	while (++item < lim) {
	    symbol = parse_stack.grammar [item];
	    
	    if (symbol > 0) {
		printf ("<%i> ", symbol);
	    }
	    else if (symbol == 0) {
		if (rule < parse_stack.hook_rule) {
		    /* Eof ou Error */
		    if (reduce == 0)
			/* Initial EoF */
			printf ("\"%s\" ", sxttext (sxplocals.sxtables, SXGET_TOKEN (0).lahead));
		    else
			fputs ("Error ", stdout);
		}
	    }
	    else {
		symbol = -symbol;
		atok = &(SXGET_TOKEN (symbol));
		
		if (sxgenericp (sxplocals.sxtables, atok->lahead)
				&& atok->string_table_entry != SXERROR_STE)
		    printf ("\"%s\" ", sxstrget (atok->string_table_entry));
		else 
		    printf ("\"%s\" ", sxttext (sxplocals.sxtables, atok->lahead));
	    }
	}
	
	fputs (";\n", stdout);
    }
}


static SXVOID output_grammar ()
{
    SXINT		rule;

    printf ("*\t(|N| = %i, |T| = %i, |P| = %i, |G| = %i)\n\n",
	  parse_stack.G.N,  
	  parse_stack.G.T,  
	  parse_stack.G.P,  
	  parse_stack.G.G);

    printf ("*\t%s's root is defined at rule #%i.\n\n",
	    parse_stack.is_ambiguous ? "DAG" : "TREE",
	    parse_stack.root);

    for (rule = 1; rule <= parse_stack.rule_top; rule++)
	if (parse_stack.rule [rule].lgth > 0)
	    output_rule (rule);
}



#endif


static SXVOID sxndparse_it ()
{
    SXINT current_symbol, ms, mtsa, sa;

#if EBUG
    sxtime (SXINIT, "Let's go...\n");
#endif    

    do {
	do {
	    /* cas du scanner deterministe */
	    current_symbol = -sxplocals.ptok_no;
	    /* read next token */
	    parse_stack.current_level = ++sxplocals.ptok_no;
	    
	    while (sxplocals.ptok_no > sxplocals.Mtok_no)
		(*(sxplocals.SXP_tables.scanit)) ();
	    
	    parse_stack.current_token = sxplocals.ptok_no;
	    parse_stack.current_milestone = sxplocals.ptok_no;
	    parse_stack.current_lahead = SXGET_TOKEN (sxplocals.ptok_no).lahead;
	    
	    parse_stack.for_scanner.current_hd = parse_stack.for_scanner.next_hd;
	    parse_stack.for_scanner.top = FS_INCR (parse_stack.for_scanner.top);
	    parse_stack.for_scanner.next_hd =
		parse_stack.for_scanner.previouses + parse_stack.for_scanner.top;

	    sxndparse_a_token (current_symbol);

	} while (*parse_stack.for_scanner.next_hd > 0);
	
	/* if (parse_stack.halt_hit) => EOF, normal case */
	/* Else, syntax error on sxplocals.ptok_no */
	/* for_scanner [for_scanner_top] designates the t_trans on the previous
	   (sxplocals.ptok_no - 1) non error token. */
	/* After Recovery, for_scanner_top, next_for_scanner and ptok_no should be correct */
    } while (!parse_stack.halt_hit && (*sxplocals.SXP_tables.recovery) (SXACTION));
}




static SXVOID dag_set (index, succ_kind, new_kind)
    SXINT index, succ_kind, new_kind;
{
    /* Si tous les successeurs de chaque predecesseur complet de index
       contient le kind "succ_kind", alors le kind de ce predecesseur est 
       mis (|=) a new_kind */
    SXINT				x, z, prev_ms_nb, next_ms_nb, mtsa, sa, tok_no;
    struct sxmilstn_elem 	*prev_ms_ptr, *next_ms_ptr;

    XxYxZ_Zforeach (sxndtkn.dag, index, z) {
	prev_ms_nb = XxYxZ_X (sxndtkn.dag, z);
	prev_ms_ptr = &sxmilstn_access (sxndtkn.milestones, prev_ms_nb);
	
	if (((prev_ms_ptr->kind & new_kind) == 0) && (prev_ms_ptr->kind & MS_COMPLETE)) {
	    /* un candidat (pas encore traite') */
	    XxYxZ_Xforeach (sxndtkn.dag, prev_ms_nb, x) {
		next_ms_nb = XxYxZ_Z (sxndtkn.dag, x);
		next_ms_ptr = &sxmilstn_access (sxndtkn.milestones, next_ms_nb);
		
		if (!(next_ms_ptr->kind & succ_kind))
		    break;
	    }
	    
	    if (x == 0) {
		prev_ms_ptr->kind |= new_kind;

		if (new_kind == MS_DEAD) {
		    /* prev_ms_nb est supprime des structures */
		    dag_set (prev_ms_nb, MS_SLEEP | MS_DEAD, MS_DEAD);

		    XxYxZ_Xforeach (sxndtkn.dag, prev_ms_nb, x) {
			/* On supprime tout ce qui depend de prev_ms_nb. */
			XxYxZ_erase (sxndtkn.dag, x);
		    }

		    /* enlever prev_ms_nb de la liste commencant en parse_stack.milstn_head */

		    if (prev_ms_ptr->prev_milstn == NULL) {
			parse_stack.milstn_head = prev_ms_ptr->next_milstn;
			parse_stack.milstn_head->prev_milstn = NULL;
		    }
		    else {
			prev_ms_ptr->prev_milstn->next_milstn = prev_ms_ptr->next_milstn;
			prev_ms_ptr->next_milstn->prev_milstn = prev_ms_ptr->prev_milstn;
		    }
		    
		    /* On suppose qu'on ne supprime jamais ni current ni head */
		    if (prev_ms_ptr == sxndtkn.milstn_current
			|| prev_ms_ptr == sxndtkn.milstn_head)
			sxtrap (ME, "dag_set");

		    if (prev_ms_nb != sxndtkn.eof_milestone)
			/* On ne supprime pas le milestone eof */
			sxmilstn_release (sxndtkn.milestones, prev_ms_nb);
		}
		else if (new_kind == MS_EXHAUSTED) {
		    dag_set (prev_ms_nb, MS_EXHAUSTED, MS_SLEEP);
		}
		else /* new_kind == MS_SLEEP */ {
		    dag_set (prev_ms_nb, MS_SLEEP | MS_DEAD, MS_DEAD);
		}
	    }
	}
    }
}



/*
  Un milestone est MS_BEGIN a sa naissance (le scanner vient de calculer
  toutes ses aretes entrantes)

  Un milestone est MS_COMPLETE quand toutes ses aretes sortantes ont ete
  calculees (il a disparu des variables locales du scanner)

  Un milestone est MS_DEAD (et recuperable) quand tous ses successeurs sont MS_DEAD | MS_SLEEP

  Un milestone est MS_SLEEP quand tous ses successeurs sont MS_EXHAUSTED

  Un milestone est MS_EXHAUSTED quand il est MS_COMPLETE et quand tous ses successeurs
  sont calcules (MS_WAIT | MS_EMPTY)

  Un milestone est MS_WAIT quand il est calcule (par sxndparser ()) et non vide

  Un milestone est MS_EMPTY quand il est calcule (par sxndparser ()) et vide
*/


static SXVOID sxnd2parse_it ()
{
    SXINT				x, y, z, prev_ms_nb, next_ms_nb, parser;
    struct sxmilstn_elem 	*prev_ms_ptr, *next_ms_ptr;
    SXBOOLEAN			is_wait;

    /* Si milstn_current != NULL, il pointe vers la bonne milestone
       sinon, on appelle le scanner.
       si milstn_current->my_index == eof_milestone c'est fini. */

    
    for (;;) {
	if (sxndtkn.milstn_current == NULL) {
	    next_ms_ptr = sxndtkn.milstn_head;

	    while (next_ms_ptr->next_milstn == NULL)
		sxndscan_it ();

	    sxndtkn.milstn_current = next_ms_ptr->next_milstn;
	}

	/* On va construire le parser associe a sxndtkn.milstn_current */
	/* Du fait du traitement d'erreur, il a pu deja etre examine', on
	   reinitialise donc son kind */
	sxndtkn.milstn_current->kind &= MS_COMPLETE + MS_BEGIN;
	is_wait = SXFALSE;

	XxYxZ_Zforeach (sxndtkn.dag, sxndtkn.milstn_current->my_index, z) {
	    prev_ms_nb = XxYxZ_X (sxndtkn.dag, z);
	    prev_ms_ptr = &sxmilstn_access (sxndtkn.milestones, prev_ms_nb);

	    if ((prev_ms_ptr->kind & (MS_WAIT + MS_EMPTY)) == 0) {
		/* Les predecesseurs sont obligatoirement deja calcules. */
		sxtrap (ME, "sxnd2parse_it");
	    }

	    if (prev_ms_ptr->kind & MS_WAIT) {
		is_wait = SXTRUE;
	    }
	}

	if (is_wait) {
	    next_ms_nb = sxndtkn.milstn_current->my_index;

	    /* Le calcul du milestone courant se fait toujours dans
	       parse_stack.for_scanner.next_hd qui doit etre a 0. */
	    
	    XxYxZ_Zforeach (sxndtkn.dag, next_ms_nb, z) {
		prev_ms_nb = XxYxZ_X (sxndtkn.dag, z);
		prev_ms_ptr = &sxmilstn_access (sxndtkn.milestones, prev_ms_nb);
		
		if (prev_ms_ptr->kind & MS_WAIT) {
		    /* cas du scanner non deterministe */
		    parse_stack.current_milestone = next_ms_nb;
		    parse_stack.current_level = prev_ms_nb; /* A VOIR */
		    parse_stack.current_token = XxYxZ_Y (sxndtkn.dag, z);
		    parse_stack.current_lahead = SXGET_TOKEN (parse_stack.current_token).lahead;
		    
		    for (parser = prev_ms_ptr->next_for_scanner;
			 parser > 0;
			 parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
			sxndsubparse_a_token (parser,
					      XxYxZ_Y (parse_stack.parsers, parser),
					      -XxYxZ_Z (parse_stack.parsers, parser) /* symbol */,
					      SXTRUE);
		    }
		}
	    }
	    
	    if (parse_stack.for_reducer.top > 0)
		reducer ();
	    
	    if (*parse_stack.for_scanner.next_hd) {
		/* Non vide */
		sxndtkn.milstn_current->next_for_scanner = *parse_stack.for_scanner.next_hd;
		*parse_stack.for_scanner.next_hd = 0;
		sxndtkn.milstn_current->kind |= MS_WAIT;
	    
		/* On regarde si certains des predecesseurs de milstn_current
		   peuvent devenir MS_EXHAUSTED */
		dag_set (next_ms_nb, MS_WAIT | MS_EMPTY, MS_EXHAUSTED);
	    }
	    else if (parse_stack.start_symbol == 0) {
		for (prev_ms_ptr = parse_stack.milstn_head;
		     prev_ms_ptr != sxndtkn.milstn_current;
		     prev_ms_ptr = prev_ms_ptr->next_milstn) {
		    if ((prev_ms_ptr->kind & (MS_WAIT + MS_COMPLETE)) == MS_WAIT)
			/* calcule (non vide) et pas completement exploite */
			break;
		}
		
		if (prev_ms_ptr == sxndtkn.milstn_current) {
		    if (!(*sxplocals.SXP_tables.recovery) (SXACTION))
			break;

		    /* Tout a ete repositionne correctement */
		    /* milstn_current repere le precedent. */
		}
		else
		   sxndtkn.milstn_current->kind |= MS_EMPTY; 
	    }
	}
	else {
	    /* Tous les predecesseurs sont vides */
	    sxndtkn.milstn_current->kind |= MS_EMPTY;
	}

	if (sxndtkn.milstn_current->my_index == sxndtkn.eof_milestone)
	    break;

	sxndtkn.milstn_current = sxndtkn.milstn_current->next_milstn;
    }
}



static SXBOOLEAN sxnddesambig_it ()
{
#if EBUG
    sxtime (SXACTION, "Scanning, Non-Deterministic Parsing & Parse Forest Sharing done in");
    fprintf (sxtty, "Source text: %ld tokens, %ld shift-transitions, %ld reduce-steps.\n",
	     (SXINT) sxplocals.Mtok_no - 1, (SXINT) parse_stack.shift_trans_nb, (SXINT) parse_stack.red_trans_nb);
#endif    

#ifdef LDBG
    output_shared_forest ();
    sxtime (SXACTION, "Shared Parse Forest Printing done in");
#endif

    if (parse_stack.start_symbol == 0) {
	parse_stack.root = 0;
	parse_stack.positions = NULL;
	parse_stack.rule = NULL;
	parse_stack.grammar = NULL;
#if EBUG
	fputs ("*\tThere is no start symbol in this shared parse forest denotation.\n\n", stdout);
#endif

    }
    else {
	unused_rule_elimination ();
	parse_stack.G.G = parse_stack.grammar_top;
	parse_stack.G.N = parse_stack.G.P = parse_stack.rule_top;
	parse_stack.G.T = parse_stack.current_level;

#if EBUG
	sxtime (SXACTION, "Unused Sub-Tree Erasing done in");
	output_grammar ();
	sxtime (SXACTION, "Shared Parse Forest Printing done in");
#endif
    }

    if (sxplocals.mode.global_errors_nb == 0) {
	if (parse_stack.root != 0
	    && parse_stack.is_ambiguous
	    && (/* parse_stack.parsact_seen ||*/ parse_stack.parsprdct_seen)) {
/* Doit-t-on executer les pars(act|prdct) si (pour un texte donne)
   la foret partagee est un arbre ?
   (le but des pars(act|prdct) est de produire un arbre, les erreurs eventuelles
   seront detectee au cours des phases ulterieures).

   Si oui, doit-t-on executer les parsact s'il n'y a pas de parsprdct ?
*/
	    SXINT		item;
	    SXBOOLEAN	failed;

	    /* sxndtw.(pass_inherited |
	               pass_derived |
		       cycle_processing |
		       open_hook |
		       close_hook |
		       nodes_oflw)
	       on deja ete positionnes par l'appel de sxndtw_open ()
	       depuis (*sxplocals.SXP_tables.parsact) (SXINIT)
	    */
	    sxndtw_init ();

	    failed = sxndrtw_walk ();

	    if (failed) {
		/* La foret a disparue..., les predicats y sont alles un peu fort. */
		parse_stack.start_symbol = 0;
	    }
	    else {
		sxndtw_check_grammar ();
	    }

#if EBUG
	    sxtime (SXACTION, "Action & Predicate Phase done in");
	    fputs ("\n\n*\tAfter \"Action & Predicate\" phase.\n\n", stdout);
	    output_grammar ();
	    sxtime (SXACTION, "Parse Tree Printing done in");
#endif

	    sxndtw_final ();
	    sxndtw_close ();
	}

	if (parse_stack.start_symbol != 0
	    && parse_stack.is_ambiguous) {
	    (*sxplocals.SXP_tables.desambig) (SXOPEN);
	    (*sxplocals.SXP_tables.desambig) (SXINIT);
	    (*sxplocals.SXP_tables.desambig) (SXACTION);

#if EBUG
	    sxtime (SXACTION, "Desambiguation Phase done in");
	    fputs ("\n\n*\tAfter desambiguation phase.\n\n", stdout);
	    output_grammar ();
	    sxtime (SXACTION, "Parse Tree Printing done in");
#endif

	    if (parse_stack.is_ambiguous)
		parse_stack.start_symbol = 0;
	    
	    (*sxplocals.SXP_tables.desambig) (SXFINAL);
	    (*sxplocals.SXP_tables.desambig) (SXCLOSE);
	}
    }
    else
	parse_stack.start_symbol = 0;

    if (parse_stack.start_symbol > 0)
	/* Here, simulate on the resulting tree the semantic calls as if the parser
	   was deterministic. */
	sxndtw_sem_calls ();

    return parse_stack.start_symbol > 0;
}


SXBOOLEAN sxndparser (what_to_do, arg)
    SXINT		what_to_do;
    struct sxtables	*arg;
{
    SXINT i, p, link;
    static SXINT	parsers_foreach [] = {0, 0, 0, 0, 0, 0};
    static SXINT	shift_reduce_foreach [] = {0, 0, 0, 0, 0, 0};

    switch (what_to_do) {
    case SXBEGIN:
	return SXTRUE;

    case SXOPEN:
	/* new language: new tables, new local parser variables */
	/* the global variable "sxplocals" must have been saved in case */
	/* of recursive invocation */

	/* test arg->magic for consistency */
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, ME);

	/* Should be computed statically... */
	parse_stack.semact = (SXINT*) sxalloc (arg->SXP_tables.P_xnbpro + 1, sizeof (SXINT));

	XxY_alloc (&parse_stack.sons, "sons", 128, 1, 1, 0, sons_oflw, stdout_or_NULL);
	
	parse_stack.erasable_sons = sxba_calloc (XxY_size (parse_stack.sons) + 1);
	parse_stack.current_links = sxba_calloc (XxY_size (parse_stack.sons) + 1);
	parse_stack.used_links = sxba_calloc (XxY_size (parse_stack.sons) + 1);
	parse_stack.transitions = (SXINT*) sxalloc (XxY_size (parse_stack.sons) + 1, sizeof (SXINT));

/* Qq mesures sur une grammaire peu ambigue de C ont donnees, pour 1000 tokens analyses
   les tailles suivantes :
       suffixes		0
       ranges		900
       symbols		2400
       sf_rhs		3400
       sf_rule		2400

   On initialise avec ces valeurs...
*/
	XxY_alloc (&parse_stack.sf_rhs, "sf_rhs", 3400, 1, 0, 0, NULL, stdout_or_NULL);
	XxY_alloc (&parse_stack.sf_rule, "sf_rule", 2400, 1, 1, 0, NULL, stdout_or_NULL);
	
	XxY_alloc (&parse_stack.suffixes, "suffixes", 128, 1, 0, 0, NULL, stdout_or_NULL);
	XxY_alloc (&parse_stack.ranges, "ranges", 900, 1, 0, 0, NULL, stdout_or_NULL);
	XxY_alloc (&parse_stack.symbols, "symbols", 2400, 1, 0, 0, NULL, stdout_or_NULL);
	parse_stack.for_reducer.triples =
	    (struct triple*) sxalloc ((i = 50) + 1, sizeof (struct triple));
	parse_stack.for_reducer.size = i; /* size */
	parse_stack.for_reducer.top  = 0; /* top */

	/* parse_stack.for_reducer.refs peut etre utilise soit avec des reduces
	   (cas normal), soit avec des etats LR (error recovery). */
	i = arg->SXP_tables.Mref - arg->SXP_tables.Mprdct;

	if (arg->SXP_tables.Mred > i)
	    i = arg->SXP_tables.Mred;

	parse_stack.for_reducer.refs = (SXINT*) sxcalloc (i + 1, sizeof (SXINT));
	parse_stack.for_reducer.refs [0] = -1;
	parse_stack.for_reducer.triples_hd = (SXINT*) sxalloc (arg->SXP_tables.Mred + 1,
							    sizeof (SXINT));
	parse_stack.for_reducer.shift_reduce_symb = (SXINT*) sxcalloc (arg->SXP_tables.Mred + 1,
							 sizeof (SXINT));

	XxYxZ_alloc (&parse_stack.parsers, "parsers", 256, 1, parsers_foreach,
		     parsers_oflw, stdout_or_NULL); 

	i = parse_stack.hole_nb = XxYxZ_size (parse_stack.parsers);
	i++;
	parse_stack.parser_to_attr =
	    (struct parsers_attr*) sxalloc (i, sizeof (struct parsers_attr));

	parse_stack.parsers_set = sxbm_calloc (5, i);
	parse_stack.free_parsers_set = parse_stack.parsers_set [4];

	parse_stack.GC_area = NULL;

	XxYxZ_alloc (&parse_stack.shift_reduce, "shift_reduce", 31, 1, shift_reduce_foreach,
		     NULL, stdout_or_NULL); 



/* prepare new local variables */

	sxplocals.sxtables = arg;
	sxplocals.SXP_tables = arg->SXP_tables;
	sxtkn_mngr (SXOPEN, 2);
	(*sxplocals.SXP_tables.recovery) (SXOPEN);
	return SXTRUE;

    case SXINIT:
    {
	/* Le token 0 est "EOF" */
	struct sxtoken tok;

	sxtkn_mngr (SXINIT, 0);
	/* terminal_token EOF */
	tok.lahead = sxplocals.SXP_tables.P_tmax;
	tok.string_table_entry = SXEMPTY_STE;
	tok.source_index = sxsrcmngr.source_coord;
	tok.comment = NULL;
	SXGET_TOKEN (0) = tok;
    }
    
	/* analyse normale */
	/* valeurs par defaut qui peuvent etre changees ds les scan_act ou pars_act. */
	sxplocals.mode.look_back = 0; /* All tokens are kept. */
	sxplocals.mode.mode = SXPARSER;
	sxplocals.mode.kind = SXWITH_RECOVERY | SXWITH_CORRECTION;
	sxplocals.mode.local_errors_nb = 0;
	sxplocals.mode.global_errors_nb = 0;
	sxplocals.mode.is_prefixe = SXFALSE;
	sxplocals.mode.is_silent = SXFALSE;
	sxplocals.mode.with_semact = SXTRUE;
	sxplocals.mode.with_parsact = SXFALSE;
	sxplocals.mode.with_parsprdct = SXFALSE;

#if EBUG
	parse_stack.shift_trans_nb = 0;
	parse_stack.red_trans_nb = 0;
#endif    

	parse_stack.start_symbol = 0;
	parse_stack.parsact_seen = SXFALSE;
	parse_stack.parsprdct_seen = SXFALSE;
	
	parse_stack.for_scanner.top = for_scanner_mask; /* pourquoi pas ! */

	for (i = 0; i <= for_scanner_mask; i++) {
	    parse_stack.for_scanner.previouses [i] = 0;
	}

	parse_stack.for_scanner.next_hd =
	    parse_stack.for_scanner.previouses + parse_stack.for_scanner.top;

	parse_stack.current_level = 0;
	parse_stack.current_milestone = 0;
	parse_stack.current_token = 0;
	parse_stack.current_lahead = sxplocals.SXP_tables.P_tmax;

	create_parser (0 /* son */,
		       sxplocals.SXP_tables.init_state,
		       NDP_access (sxplocals.SXP_tables.t_bases + sxplocals.SXP_tables.init_state,
				   parse_stack.current_lahead) /* ref */,
		       0 /* symb */ );

	parse_stack.halt_hit = SXFALSE;
	parse_stack.rcvr.is_up = SXFALSE;
	parse_stack.milstn_head = NULL;
	parse_stack.delta_generator = 0;

	(*sxplocals.SXP_tables.recovery) (SXINIT);
	return SXTRUE;
			  
    case SXACTION:
	sxndparse_it ();

	if (sxplocals.mode.mode == SXPARSER)
	    sxnddesambig_it ();

	return SXTRUE;

    case SXFINAL:
	(*sxplocals.SXP_tables.recovery) (SXFINAL);
	sxtkn_mngr (SXFINAL, 0);
	return SXTRUE;

    case SXCLOSE:
	/* end of language: free the local arrays */
	if (parse_stack.positions != NULL)
	    sxfree (parse_stack.positions);

	sxfree (parse_stack.semact);
	sxfree (parse_stack.parsers_set);
	sxfree (parse_stack.transitions);
	XxY_free (&parse_stack.sons);
	XxY_free (&parse_stack.suffixes);
	XxY_free (&parse_stack.ranges);
	XxY_free (&parse_stack.symbols);
	XxY_free (&parse_stack.sf_rhs);
	XxY_free (&parse_stack.sf_rule);

	if (parse_stack.rule != NULL) {
	    sxfree (parse_stack.rule);
	    sxfree (parse_stack.grammar);
	}

	sxfree (parse_stack.erasable_sons);
	sxfree (parse_stack.current_links);
	sxfree (parse_stack.used_links);

	sxfree (parse_stack.for_reducer.triples);
	sxfree (parse_stack.for_reducer.refs);
	sxfree (parse_stack.for_reducer.triples_hd);
	sxfree (parse_stack.for_reducer.shift_reduce_symb);

	XxYxZ_free (&parse_stack.parsers);
	sxfree (parse_stack.parser_to_attr);

	if (parse_stack.GC_area != NULL)
	    sxfree (parse_stack.GC_area), parse_stack.GC_area = NULL;

	XxYxZ_free (&parse_stack.shift_reduce);
	
	sxtkn_mngr (SXCLOSE, 0);
	return SXTRUE;

    case SXEND:
	(*sxplocals.SXP_tables.recovery) (SXCLOSE);
	return SXTRUE;

    default:
	fprintf (sxstderr, "The function \"sxndparser\" is out of date with respect to its specification.\n");
	sxexit(1);
    }
}



SXBOOLEAN sxnd22parser (what_to_do, arg)
    SXINT		what_to_do;
    struct sxtables	*arg;
{
    SXINT i, p, link;

    switch (what_to_do) {
    case SXBEGIN:
	sxndparser (SXBEGIN, arg);
	return SXTRUE;

    case SXOPEN:
	sxndparser (SXOPEN, arg);
	sxndtkn_mngr (SXOPEN, 2);
	return SXTRUE;

    case SXINIT:
	sxndparser (SXINIT, arg);
	/* parse_stack.for_scanner.next_hd contient les actions shifts sur le eof initial */
	sxndtkn_mngr (SXINIT, 0);
	/* tail pointe sur le successeur du milestone initial. */
	parse_stack.milstn_head = sxndtkn.milstn_head;
	parse_stack.milstn_head->next_for_scanner = *parse_stack.for_scanner.next_hd;
	*parse_stack.for_scanner.next_hd = 0;
	return SXTRUE;
			  
    case SXACTION:
	sxnd2parse_it ();

	if (sxplocals.mode.mode == SXPARSER)
	    sxnddesambig_it ();

	return SXTRUE;

    case SXFINAL:
	sxndparser (SXFINAL, arg);
	sxndtkn_mngr (SXFINAL, 0);
	return SXTRUE;

    case SXCLOSE:
	sxndparser (SXCLOSE, arg);
	sxndtkn_mngr (SXCLOSE, 0);
	return SXTRUE;

    case SXEND:
	sxndparser (SXEND, arg);
	return SXTRUE;

    default:
	fprintf (sxstderr, "The function \"sxnd22parser\" is out of date with respect to its specification.\n");
	sxexit(1);
    }
}





SXBOOLEAN sxnd2parse_in_la (ep_la, Ttok_no, Htok_no, mode_ptr)
    SXINT	ep_la, Ttok_no, *Htok_no;
    struct sxparse_mode		*mode_ptr;
{
    /* Appel recursif du parser pour verifier si le sous-langage defini par
       le point d'entree ep_la contient un prefixe du source. */
    /* Le token caracteristique du point d'entree est memorise en Ttok_no */
    /* L'indice du dernier token analyse par cet appel est range en Htok_no */
    SXINT			i, current_token, current_lahead, current_level, p, link;
    SXBOOLEAN		ret_val;
    struct sxtoken	old_tt, *ptt;
    struct sxparse_mode	old_mode;
    SXINT			*cfs, *nfs, *fs;

    /* On memorise qq petites choses */
    old_mode = sxplocals.mode;
    current_token = parse_stack.current_token;
    current_lahead = parse_stack.current_lahead;
    current_level = parse_stack.current_level;

    sxplocals.mode = *mode_ptr;
    old_tt = *(ptt = &(SXGET_TOKEN (Ttok_no)));
    
    /* On change le token courant par le point d'entree */
    ptt->lahead = ep_la;
    ptt->string_table_entry = SXEMPTY_STE;
    /* On pointe sur le token precedent (normalement non acce'de') */
    Ttok_no--;
    
    /* On va travailler en bascule sur les deux for_parser suivants. */
    /* Faux si recursif a plus de 2 niveaux, ds ce cas memoriser les for_scanner
       en local (attention a GC ()). */
    cfs = parse_stack.for_scanner.current_hd;
    nfs = parse_stack.for_scanner.next_hd;
    parse_stack.for_scanner.current_hd = parse_stack.for_scanner.previouses
	+ (i = FS_INCR (parse_stack.for_scanner.top));
    parse_stack.for_scanner.next_hd = parse_stack.for_scanner.previouses + FS_INCR (i);
    *parse_stack.for_scanner.next_hd = 0;
			
    /* appel recursif */
    /* create a stack node p with state START_STATE(grammar) */
    parse_stack.current_milestone = parse_stack.current_token = Ttok_no; /* ? */
    parse_stack.current_lahead = sxplocals.SXP_tables.P_tmax;

    create_parser (0 /* son */,
		   sxplocals.SXP_tables.init_state,
		   NDP_access (sxplocals.SXP_tables.t_bases + sxplocals.SXP_tables.init_state,
			       parse_stack.current_lahead) /* ref */,
		   0 /* symb */ );

    do {
	parse_stack.current_milestone = parse_stack.current_token = ++Ttok_no;
	parse_stack.current_lahead = sxget_token (Ttok_no)->lahead;
	fs = parse_stack.for_scanner.next_hd;
	parse_stack.for_scanner.current_hd = parse_stack.for_scanner.next_hd;
	parse_stack.for_scanner.next_hd = fs;

	sxndparse_a_token (0);

	ret_val = *parse_stack.for_scanner.next_hd > 0
	    && XxYxZ_X (parse_stack.parsers, *parse_stack.for_scanner.next_hd)
		== sxplocals.SXP_tables.final_state;
	/* ret_val <==> on a atteint l'etat final */
    } while (!ret_val
	     && *parse_stack.for_scanner.next_hd > 0
	     && parse_stack.current_lahead != sxplocals.SXP_tables.P_tmax);
    
    *parse_stack.for_scanner.current_hd = 0;
    *parse_stack.for_scanner.next_hd = 0;
    parse_stack.for_scanner.next_hd = nfs;
    parse_stack.for_scanner.current_hd = cfs;

    /* dernier token lu par l'appel precedent. */
    *Htok_no = Ttok_no;

    /* On remet en place */
    *ptt = old_tt;
    parse_stack.current_lahead = current_lahead;
    parse_stack.current_token = current_token;
    parse_stack.current_level = current_level;
    mode_ptr->local_errors_nb = sxplocals.mode.local_errors_nb;
    mode_ptr->global_errors_nb = sxplocals.mode.global_errors_nb;
    /* L'appelant decide (et le fait) si le nombre d'erreur doit
       etre cumule'. */
    sxplocals.mode = old_mode;

    return ret_val;
}
