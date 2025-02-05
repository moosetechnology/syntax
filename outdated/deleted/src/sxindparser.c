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

static char	ME [] = "INDPARSER";

/************************************************************************/
/* It is the incremental nondeterministic version of the SYNTAX parser	*/
/* This algorithm is borrowed from Jan Rekers' thesis			*/
/************************************************************************/

/*
  The parse_stack is a directed graph whose nodes are called parse_stack_elem
  Each parse_stack_elem is a couple (state, token_nb) where state is an element
  of the underlying PDA and token_nb the indexes of the current_token at the
  moment where this parse_stack_elem was built.
  The active_parsers are a set of parse_stack_elem.
  The parse_stack is a set of couples (parse_stack_elem, parse_stack_elem).
*/

#include "sxversion.h"
#include "sxcommon.h"
# include <stdio.h>
# include "sxindparser.h"
# include "B_tables.h"

char WHAT_SXIND_PARSER[] = "@(#)SYNTAX - $Id: sxindparser.c 4389 2024-10-07 08:38:08Z garavel $" WHAT_DEBUG;


struct rhs {
    SXINT		symbol, next;
};

struct  parse_stack_elem {
    SXINT		state, red_ref, shift_ref, son, level, symbol;
};

struct G {
    SXINT		N, T, P, G;
};

struct red_parsers {
    SXINT parser, lnk;
};


static struct parse_stack {
    struct parse_stack_elem	*graph;

    struct red_parsers		*red_parsers_list;

    XxY_header			sons,
                                suffixes,
                                ranges,
                                symbols;

    XH_header			output_grammar;

    SXBA			*parsers_set;

    SXINT	                        *parsers_rhs0,
    	                        *parsers_rhs1;

    SXBA			current_links,
                                used_links,
                                free_parsers_set,
                                erasable_sons,
                                active_sons;

    SXINT				*active_states,
                                *active_parsers,
                                *GC_area,
                                *red_stack,
                                *red_parsers_hd,
                                *for_shifter,
                                *for_scanner,
                                *for_actor;

    SXINT				start_symbol,
                                graph_size,
	                        graph_top,
	                        rhs_size,
	                        rhs_top,
	                        red_parsers_list_top,
	                        red_parsers_list_size,
	                        free_parser,
	                        current_level,
                                current_token;

    SXINT			        *transitions,
	                        red_no,
	                        current_symbol;

    struct rhs			*rhs;

    struct G			G;

    bool			is_new_links;
} parse_stack;


static  void sons_oflw (old_size, new_size)
    SXINT		old_size, new_size;
{
    parse_stack.erasable_sons = sxba_resize (parse_stack.erasable_sons, new_size + 1);
    parse_stack.current_links = sxba_resize (parse_stack.current_links, new_size + 1);
    parse_stack.used_links = sxba_resize (parse_stack.used_links, new_size + 1);
    parse_stack.transitions = (SXINT*) sxrealloc (parse_stack.transitions, new_size + 1, sizeof (SXINT));
}


static void GC (p)
    SXINT p;
{
    SXINT	x, father, son, lim;
    SXBA	free_parsers_set, erasable_sons;
    bool		is_in_sons = false;

    sxba_fill (erasable_sons = parse_stack.erasable_sons);
    sxba_fill (free_parsers_set = parse_stack.free_parsers_set);

    SS_push (parse_stack.GC_area, p);
    SXBA_0_bit (free_parsers_set, p);
    
    for (lim = SS_top (parse_stack.for_scanner), x = SS_bot (parse_stack.for_scanner);
	 x < lim;
	 x++) {
	father = SS_get (parse_stack.for_scanner, x);

	if (SXBA_bit_is_set_reset (free_parsers_set, father))
	    SS_push (parse_stack.GC_area, father);
    }

    x = 0;

    while ((x = parse_stack.for_shifter [x]) > 0) {
	father = parse_stack.active_parsers [x];

	if (SXBA_bit_is_set_reset (free_parsers_set, father))
	    SS_push (parse_stack.GC_area, father);
    }

    /* for_actor is always a subset of active_states. */ 
    x = 0;

    while ((x = parse_stack.active_states [x]) > 0) {
	father = parse_stack.active_parsers [x];

	if (SXBA_bit_is_set_reset (free_parsers_set, father))
	    SS_push (parse_stack.GC_area, father);
    }

    do {
	son = SS_pop (parse_stack.GC_area);

	while ((son = parse_stack.graph [father = son].son) > 0) {
	    if (!SXBA_bit_is_set_reset (free_parsers_set, son))
		break;
	}

	if (son < 0) {
	    is_in_sons = true;
	    son = -son;
	    
	    if (SXBA_bit_is_set_reset (free_parsers_set, son))
		SS_push (parse_stack.GC_area, son);

	    XxY_Xforeach (parse_stack.sons, father, x) {
		SXBA_0_bit (erasable_sons, x);
		son = XxY_Y (parse_stack.sons, x);

		if (SXBA_bit_is_set_reset (free_parsers_set, son))
		    SS_push (parse_stack.GC_area, son);
	    }
	}
    } while (!SS_is_empty (parse_stack.GC_area));

    if (is_in_sons) {
	x = XxY_top (parse_stack.sons) + 1;

	while ((x = sxba_1_rlscan (erasable_sons, x)) > 0)
	    XxY_erase (parse_stack.sons, x);
    }

    parse_stack.free_parser =
	parse_stack.graph_top =
	    ((x = sxba_0_rlscan (free_parsers_set, parse_stack.graph_size + 1)) < 0) ? 0 : x;
}



static bool new_symbol (p, k, i, j, symbol)
    SXINT p, k, i, j, *symbol;
{
    SXINT kind, range;

    if (k > 0) {
	/* Suffixe sharing. */
	XxY_set (&parse_stack.suffixes, p, k, &kind);
	kind += sxplocals.SXP_tables.P_nbpro;
    }
    else
	kind = p;

    if (p >= 0)
	XxY_set (&parse_stack.ranges, i, j, &range);
    else
	range = j;

    return XxY_set (&parse_stack.symbols, kind, range, symbol);
}


static void output_symbol (symbol)
    SXINT symbol;
{
    /* Assume symbol > 0 => not a terminal string. */
    SXINT kind, range, i, j;

    if (symbol < 0) {
	symbol = -symbol;

	if (symbol <= sxplocals.SXP_tables.P_tmax)
	    printf ("\"%s\" ", sxttext (sxsvar.sxtables, symbol));
	else 
	    printf ("\"%s\" ", sxstrget (symbol - sxplocals.SXP_tables.P_tmax));
		     
    }
    else {
	kind = XxY_X (parse_stack.symbols, symbol);
	range = XxY_Y (parse_stack.symbols, symbol);
	
	if (kind < 0)
	    printf("<%i, %i> ", kind, range);
	else {
	    i = XxY_X (parse_stack.ranges, range);
	    j = XxY_Y (parse_stack.ranges, range);
	    
	    if (kind > sxplocals.SXP_tables.P_nbpro) {
		kind -= sxplocals.SXP_tables.P_nbpro;
		printf ("<%i, %i, %i, %i> ", XxY_X (parse_stack.suffixes, kind),
			 XxY_Y (parse_stack.suffixes, kind), i, j);
	    }
	    else
		printf ("<%i, %i, %i> ", kind, i, j);
	}
    }
}


static void output_grammar_rule (rule_no)
    SXINT rule_no;
{
    SXINT x, lim;

    lim = XH_X (parse_stack.output_grammar, rule_no + 1);
    x = XH_X (parse_stack.output_grammar, rule_no);
    output_symbol (XH_list_elem (parse_stack.output_grammar, x));
    printf ("\t= ");

    while (++x < lim) {
	output_symbol (XH_list_elem (parse_stack.output_grammar, x));
    }

    printf (";\n");
}


static void  set_t_rule (symbol)
    SXINT	symbol;
{
    SXINT		kind, range, t, rule_no;

    kind = XxY_X (parse_stack.symbols, symbol);
    range = XxY_Y (parse_stack.symbols, symbol);
    t = (sxgenericp (sxsvar.sxtables, -kind)) ?
	  -(SXGET_TOKEN (range).string_table_entry + sxplocals.SXP_tables.P_tmax) :
	  kind;
    XH_push (parse_stack.output_grammar, symbol);
    XH_push (parse_stack.output_grammar, t);
    XH_set (&parse_stack.output_grammar, &rule_no);
}  


static void  set_unit_rule (lhs, rhs)
    SXINT		lhs, rhs;
{
    SXINT rule_no;

    XH_push (parse_stack.output_grammar, lhs);
    XH_push (parse_stack.output_grammar, rhs);
    XH_set (&parse_stack.output_grammar, &rule_no);
}  


static void  set_rule (lhs, rhs)
    SXINT			lhs, rhs;
{
    SXINT				rule_no;

    XH_push (parse_stack.output_grammar, lhs);

    if (rhs < 0)
	XH_push (parse_stack.output_grammar, -rhs);
    else {
	while (rhs > 0) {
	    XH_push (parse_stack.output_grammar, parse_stack.rhs [rhs].symbol);
	    rhs = parse_stack.rhs [rhs].next;
	}
    }

    XH_set (&parse_stack.output_grammar, &rule_no);
}



static void push_rp (ref, p)
    SXINT ref, p;
{
    SXINT		q;
    struct red_parsers	*ared_parsers;

    if (parse_stack.red_stack [ref] == 0) {
	PUSH (parse_stack.red_stack, ref);
	parse_stack.red_parsers_hd [ref] = -p;
    }
    else {
	if (++parse_stack.red_parsers_list_top >= parse_stack.red_parsers_list_size)
	    parse_stack.red_parsers_list = (struct red_parsers*)
		sxrealloc (parse_stack.red_parsers_list,
			   (parse_stack.red_parsers_list_size *= 2) + 1,
			   sizeof (struct red_parsers));

	ared_parsers = parse_stack.red_parsers_list + parse_stack.red_parsers_list_top;

	if ((q = parse_stack.red_parsers_hd [ref]) < 0) {
	    ared_parsers->parser = -q;
	    ared_parsers->lnk = 0;
	    q = parse_stack.red_parsers_hd [ref] = parse_stack.red_parsers_list_top;
	    ared_parsers++, parse_stack.red_parsers_list_top++;
	}

	ared_parsers->parser = p;
	ared_parsers->lnk = q;
	parse_stack.red_parsers_hd [ref] = parse_stack.red_parsers_list_top;
    }
}



static void  set_start_symbol (symbol)
    SXINT		symbol;
{
    SXINT		symb;
    /* Halt */
    /* Several "final" parse_stack elem could occur since they are not
       set in "active_parsers". */

    if (parse_stack.start_symbol == 0) {
	new_symbol (0, 0, 0, parse_stack.current_level, &symb);
	parse_stack.start_symbol = symb;
    }
	
    set_unit_rule (parse_stack.start_symbol, symbol);
}



static SXINT add_active_parser (SXINT son, SXINT state, SXINT symbol)
{
    /* Create a parser named "father" whose state is "state"
     and link it to "son". */
    /* The transition from "son" to "father" is performed on "symbol".
       "symbol" is a pointer to a 4-tuple (p, k, i, j) s.t.
          - p>0 is an input grammar rule number
	    p<0 is the opposite of the code of aterminal symbol
	  - k is non negative integer. The couple (p,k) designate an item
	    if p = A -> X0 X1   Xk  Xl, (p,k) is the item A -> X0 X1   .Xk  Xl
	  - i and j are positive integers which are token numbers s.t.
	    i <= j and Xk...Xl =>* ai+1 ... aj */


    SXINT			x, father, ref;
    SXINT					link, t_state, lim, red_no, son_state;
    struct SXP_prdct		*aprdct;
    struct parse_stack_elem	*agraph;

    /* Already known as an active parser or as a for_shifter? */
    if (parse_stack.active_states [state] != 0  ||
	parse_stack.for_shifter [state] != 0) {
	agraph = &(parse_stack.graph [father = parse_stack.active_parsers [state]]);

	/* Add a link from father to son if it does not already exists. */
	if ((x = agraph->son) == son || -x == son) {
	    if (agraph->symbol != symbol)
		set_unit_rule (agraph->symbol, symbol);

	    return 0;
	}

	if (x > 0)
	    agraph->son = -x;
		
	if (XxY_set (&parse_stack.sons, father, son, &link)) {
	    if (parse_stack.transitions [link] != symbol)
		set_unit_rule (parse_stack.transitions [link], symbol);

	    return 0;
	}
	    
	parse_stack.transitions [link] = symbol;

	/* "father" is active and it already exists; furthermore it already has
	   a child which is not "son". */
	/* If "father" (or one of its ancestors) has a reduce action, this action
	   must take "link" as a pop path if relevant.
	   To be sure that such a case is always processed, it will be examined
	   when "father" has an active ancestor or when "father" has already been
	   processed. */
	return (parse_stack.active_states [state] != 0 &&
	    (parse_stack.for_actor [state] == 0 ||
	     SXBA_bit_is_set (parse_stack.active_sons, state))) ? link : 0;
    }

    /* create a stack node father with state "state" */

    /* Seeking a new parse graph elem whose id is father. */
    if (parse_stack.graph_top < parse_stack.graph_size)
	father = ++parse_stack.graph_top;
    else    
	if (parse_stack.free_parser > 0 &&
	    (parse_stack.free_parser = sxba_1_rlscan (parse_stack.free_parsers_set, parse_stack.free_parser)) > 0)
	    father =  parse_stack.free_parser;
    else {
	GC (son);

	if (parse_stack.graph_top < parse_stack.graph_size)
	    father = ++parse_stack.graph_top;
	else    
	    if (parse_stack.free_parser > 0 &&
		(parse_stack.free_parser = sxba_1_rlscan (parse_stack.free_parsers_set, parse_stack.free_parser)) > 0)
		father =  parse_stack.free_parser;
	    else {
		parse_stack.graph = (struct parse_stack_elem*)
		    sxrealloc (parse_stack.graph,
			       (parse_stack.graph_size *= 2) + 1,
			       sizeof (struct parse_stack_elem));

		parse_stack.free_parsers_set = sxba_resize (parse_stack.free_parsers_set,
						    parse_stack.graph_size + 1);
		parse_stack.parsers_set = sxbm_resize (parse_stack.parsers_set,
						       4,
						       4,
						       parse_stack.graph_size + 1);
		parse_stack.parsers_rhs0 = (SXINT*) sxrealloc (parse_stack.parsers_rhs0,
							     parse_stack.graph_size + 1,
							     sizeof (SXINT));
		parse_stack.parsers_rhs1 = (SXINT*) sxrealloc (parse_stack.parsers_rhs1,
							     parse_stack.graph_size + 1,
							     sizeof (SXINT));
		father =  ++parse_stack.graph_top;
	    }
    }

    agraph = &(parse_stack.graph [father]);
    agraph->state = state;
    agraph->red_ref = 0;
    agraph->shift_ref = 0;
    /* add a link from father to son */
    agraph->son = son;
    agraph->level = parse_stack.current_level;
    agraph->symbol = symbol;
    
    if (state > lr0_automaton_state_nb) {
	PUSH (parse_stack.active_states, state);
	PUSH (parse_stack.for_actor, state);
	parse_stack.active_parsers [state] = father;

	if (parse_stack.active_states [son_state = parse_stack.graph [son].state] != 0)
	    SXBA_1_bit (parse_stack.active_sons, son_state);

	return 0;
    }
    else {
	struct Q0	*aQ0 = Q0 + state;

	if (aQ0->t_trans_nb != 0 &&
	    (ref = XxY_is_set (&Q0xV_hd, state, -parse_stack.current_token)) > 0) {
	    if (parse_stack.current_token == sxplocals.SXP_tables.P_tmax) {
		/* Halt */
		set_start_symbol (symbol);
	    }
	    else {
		agraph->shift_ref = Q0xV_to_Q0 [ref];
		PUSH (parse_stack.for_shifter, state);
		parse_stack.active_parsers [state] = father;
	    }
	}
    
	if ((lim = aQ0->red_trans_nb) != 0) {
	    ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb;
	    red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
	    PUSH (parse_stack.active_states, state);
	    PUSH (parse_stack.for_actor, state);
	    parse_stack.active_parsers [state] = father;
	    push_rp (red_no, father); /* for parse_forest () */

	    if (parse_stack.active_states [son_state = parse_stack.graph [son].state] != 0)
		SXBA_1_bit (parse_stack.active_sons, son_state);

	    if (lim == 1) {
		agraph->red_ref = red_no;
	    }
	    else {
		agraph->red_ref = -state;
		lim += ref;

		while (++ref < lim) {
		    red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis; 
		    push_rp (red_no, father); /* for parse_forest () */
		}
	    }
	}
    }

    return 0;
}
    

static SXINT push_rhs (SXINT symbol, SXINT list)
{
    /* Adds symbol before list. */

    if ((++parse_stack.rhs_top + 1) > parse_stack.rhs_size)
	parse_stack.rhs = (struct rhs*) sxrealloc (parse_stack.rhs, (parse_stack.rhs_size *= 2) + 1, sizeof (struct rhs));

    if (list < 0) {
	parse_stack.rhs [parse_stack.rhs_top].symbol = -list;
	parse_stack.rhs [list = parse_stack.rhs_top].next = 0;
	++parse_stack.rhs_top;
    }

    parse_stack.rhs [parse_stack.rhs_top].symbol = symbol;
    parse_stack.rhs [parse_stack.rhs_top].next = list;
    return parse_stack.rhs_top;
}


static void do_reductions (son, is_in_set)
    SXINT		son;
    bool	is_in_set;
{
    /* We want to perform a  reduce_action A -> alpha on p. */
    /* l = length(alpha). */
    /* p is a parse_stack_elem and link an index in parse_stack.sons
       which is characteristic of a given transition. */
    /* FORALL q for which a path of length l from p to q through
       link exists SXDO
          REDUCER (q, GOTO (state (q), A)). */
    SXINT					symb, link, next; 
    struct parse_stack_elem		*agraph;
    bool				is_first_time;

    is_first_time = true;

    do {
	agraph = &(parse_stack.graph [son]);
	new_symbol (parse_stack.red_no,
		    0,
		    agraph->level,
		    parse_stack.current_level,
		    &symb);

	/* in-lining of reducer () */
	if ((next = Q0xV_to_Q0 [XxY_is_set (&Q0xV_hd, agraph->state, bnf_ag.WS_NBPRO [parse_stack.red_no].reduc)]) < 0) {
	    /* Shift and Reduce */
	    next = lr0_automaton_state_nb + bnf_ag.WS_INDPRO [-next].prolis;
	}

	/* We create an active parser even when next designates a
	   shift (on the LHS non-terminal) and reduce
	   (i.e next > lr0_automaton_state_nb. */
	/* If exists q in active_parsers with state(q)==state then */
	if ((link = add_active_parser (son, next, symb)) > 0) {
	    /* if there is no direct link from father to son yet then */
	    /* add a link from father to son */
	    /* forall r in (active_states - for_actor) do */
	    parse_stack.is_new_links = true;
	    SXBA_1_bit (parse_stack.current_links, link);
	}
	
	if (is_first_time) {
	    if (!is_in_set)
		break;

	    is_first_time = false;
	    son = 0;
	}
    } while ((son = sxba_scan_reset (parse_stack.parsers_set [0], son)) > 0);
}



static void do_pops (p, l)
    SXINT		p, l;
{
    /* We want to perform a  reduce_action A -> alpha on p. */
    /* l = length(alpha). */
    /* p is a parse_stack_elem and link an index in parse_stack.sons
       which is characteristic of a given transition. */
    /* FORALL q for which a path of length l from p to q through
       link exists SXDO
          REDUCER (q, GOTO (state (q), A)). */
    SXINT			x, son, father, next_son, first_son;
    SXBA		fathers_set, sons_set;
    bool		is_in_set;

    is_in_set = false;
    son = p;

    while (l-- > 0) {
	father = son;

	if (!is_in_set) {
	    if ((son = parse_stack.graph [father].son) < 0) {
		sons_set = (l & 01) ?
		    parse_stack.parsers_set [1] : parse_stack.parsers_set [0];

		XxY_Xforeach (parse_stack.sons, father, x) {
		    next_son = XxY_Y (parse_stack.sons, x);
		    SXBA_1_bit (sons_set, next_son);
		}

		son = -son;
		is_in_set = true;
	    }
	}
	else {
	    is_in_set = false;

	    if (l & 01) {
		fathers_set = parse_stack.parsers_set [0];
		sons_set = parse_stack.parsers_set [1];
	    }
	    else {
		fathers_set = parse_stack.parsers_set [1];
		sons_set = parse_stack.parsers_set [0];
	    }

	    if ((son = parse_stack.graph [father].son) < 0) {
		is_in_set = true;
		son = -son;

		XxY_Xforeach (parse_stack.sons, father, x) {
		    next_son = XxY_Y (parse_stack.sons, x);
		    SXBA_1_bit (sons_set, next_son);
		}
	    }

	    father = 0;
	    
	    while ((father = sxba_scan_reset (fathers_set, father)) > 0) {
		if ((first_son = parse_stack.graph [father].son) < 0) {
		    is_in_set = true;
		    first_son = -first_son;
		    
		    XxY_Xforeach (parse_stack.sons, father, x) {
			if ((next_son = XxY_Y (parse_stack.sons, x)) != son)
			    SXBA_1_bit (sons_set, next_son);
		    }
		}

		if (first_son != son) {
		    SXBA_1_bit (sons_set, first_son);
		    is_in_set = true;
		}
	    }
	}
    }

    do_reductions (son, is_in_set);
}



static void do_limited_reductions (p, l)
    SXINT p, l;
{
    /* We want to perform a  reduce_action A -> alpha on p which uses link. */
    /* l = length(alpha). */
    /* p is a parse_stack_elem and link an index in parse_stack.sons
       which is characteristic of a given transition. */
    /* FORALL q for which a path of length l from p to q through
       link exists SXDO
       REDUCER (q, GOTO (state (q), A)). */
    /* called with l > 0 */
    SXINT	x, father, son, first_father;
    bool		is_no_link, is_sons;
    SXBA	fathers_set, sons_set, fathers_no_link_set, sons_no_link_set;
    SXBA		s;

    x = l & 01;
    fathers_set = parse_stack.parsers_set [x];
    fathers_no_link_set = parse_stack.parsers_set [x+2];
    x = (x + 1) & 01;
    sons_set = parse_stack.parsers_set [x];
    sons_no_link_set = parse_stack.parsers_set [x+2];
    SXBA_1_bit (fathers_set, p);
    SXBA_1_bit (fathers_no_link_set, p);

    for (;;) {
	father = 0;
	
	while ((father = sxba_scan_reset (fathers_set, father)) > 0) {
	    is_no_link = SXBA_bit_is_set_reset (fathers_no_link_set, father);
	    
	    if ((is_sons = (son = parse_stack.graph [father].son) < 0))
		son = -son;
	    
	    SXBA_1_bit (sons_set, son);

	    if (is_no_link)
		SXBA_1_bit (sons_no_link_set, son);
	    
	    if (is_sons) {
		XxY_Xforeach (parse_stack.sons, father, x) {
		    son = XxY_Y (parse_stack.sons, x);
		    SXBA_1_bit (sons_set, son);

		    if (is_no_link && !SXBA_bit_is_set (parse_stack.used_links, x))
			SXBA_1_bit (sons_no_link_set, son);
		}
	    }
	}

	if (--l == 0)
	    break;

	s = fathers_set, fathers_set = sons_set, sons_set = s;
	s = fathers_no_link_set, fathers_no_link_set = sons_no_link_set, sons_no_link_set = s;
    }
	
    sxba_minus (sons_set, sons_no_link_set);

    if ((son = sxba_scan_reset (sons_set, 0)) > 0) {
	do_reductions (son, sxba_scan (sons_set, son) > 0);
    }
}


static void seek_paths (p, p_trans, is_in_set, l)
    SXINT		p, p_trans, l;
    bool	is_in_set;
{
    SXINT		x, son, father, symb, first_son;
    SXBA	fathers_set, sons_set, s;
    SXINT		*fathers_rhs_hd, *sons_rhs_hd, *ptr, *son_rhs_ptr;
    SXINT		father_rhs_hd, rhs, first_son_rhs_hd, son_rhs_hd;
    bool	is_sons, is_first_time, is_in_sons_set;

    if (l == 0 && !is_in_set) {
	new_symbol (parse_stack.red_no,
		    0,
		    parse_stack.graph [p].level,
		    parse_stack.current_level,
		    &symb);
	set_rule (symb, p_trans);
	return;
    }
    
    fathers_set = parse_stack.parsers_set [1];
    fathers_rhs_hd = parse_stack.parsers_rhs1;
    sons_set = parse_stack.parsers_set [0];
    sons_rhs_hd = parse_stack.parsers_rhs0;

    first_son = p;
    first_son_rhs_hd = p_trans;

    while (l-- > 0) {
	father = first_son;
	father_rhs_hd = first_son_rhs_hd;
	first_son_rhs_hd = 0;
	is_first_time = true;

	for (;;) {
	    if ((is_sons = (son = parse_stack.graph [father].son) < 0))
		son = -son;
	    
	    if (is_first_time)
		first_son = son;

	    rhs = push_rhs (parse_stack.graph [father].symbol, father_rhs_hd);

	    if (first_son == son)
		son_rhs_ptr = &first_son_rhs_hd;
	    else {
		is_in_sons_set = true;
		son_rhs_ptr = sons_rhs_hd + son;

		if (SXBA_bit_is_reset_set (sons_set, son))
		    *son_rhs_ptr = 0;
	    }
	    
	    if (*son_rhs_ptr == 0)
		*son_rhs_ptr = rhs;	
	    else {
		if (*son_rhs_ptr > 0) {
		    new_symbol (parse_stack.red_no,
				l,
				parse_stack.graph [son].level,
				parse_stack.current_level,
				&symb);
		    set_rule (symb, *son_rhs_ptr);
		    *son_rhs_ptr = -symb;
		}
		
		set_rule (-*son_rhs_ptr, rhs);
	    }

	    if (is_sons) {
		XxY_Xforeach (parse_stack.sons, father, x) {
		    son = XxY_Y (parse_stack.sons, x);
		    son_rhs_ptr = sons_rhs_hd + son;
		    rhs = push_rhs (parse_stack.transitions [x], father_rhs_hd);

		    if (first_son == son)
			son_rhs_ptr = &first_son_rhs_hd;
		    else {
			is_in_sons_set = true;
			son_rhs_ptr = sons_rhs_hd + son;

			if (SXBA_bit_is_reset_set (sons_set, son))
			    *son_rhs_ptr = 0;
		    }
		    
		    if (*son_rhs_ptr == 0)
			*son_rhs_ptr = rhs;	
		    else {
			if (*son_rhs_ptr > 0) {
			    new_symbol (parse_stack.red_no,
					l,
					parse_stack.graph [son].level,
					parse_stack.current_level,
					&symb);
			    set_rule (symb, *son_rhs_ptr);
			    *son_rhs_ptr = -symb;
			}
			
			set_rule (-*son_rhs_ptr, rhs);
		    }
		}
	    }

	    if (is_first_time) {
		is_first_time = false;
		father = 0;
	    }

	    if (is_in_set && (father = sxba_scan_reset (fathers_set, father)) > 0) {
		father_rhs_hd = fathers_rhs_hd [father];
	    }
	    else
		break;
	}

	is_in_set = is_in_sons_set;

	if (l > 0) {
	    s = fathers_set, fathers_set = sons_set, sons_set = s;
	    ptr = fathers_rhs_hd, fathers_rhs_hd = sons_rhs_hd, sons_rhs_hd = ptr;
	}
    }

    if (first_son_rhs_hd > 0) {
	new_symbol (parse_stack.red_no,
		    0,
		    parse_stack.graph [first_son].level,
		    parse_stack.current_level,
		    &symb);
	set_rule (symb, first_son_rhs_hd);
    }

    if (is_in_set) {
	son = 0;

	while ((son = sxba_scan_reset (sons_set, son)) > 0) {
	    if ((son_rhs_hd = sons_rhs_hd [son]) > 0) {
		new_symbol (parse_stack.red_no,
			    0,
			    parse_stack.graph [son].level,
			    parse_stack.current_level,
			    &symb);
		set_rule (symb, son_rhs_hd);
	    }
	}
    }

    parse_stack.rhs_top = 0;
}


static void parse_forest ()
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
    SXINT			p, ref, x;

    while (parse_stack.active_states [0] > 0) {
	POP (parse_stack.active_states, p);
	p = parse_stack.active_parsers [p];

	if ((ref = parse_stack.graph [p].red_ref) == 0) {
	    /* Shift and reduce parser */
	    /* Always a one to one mapping between parsers and ref. */
	    parse_stack.red_no = bnf_ag.WS_INDPRO [parse_stack.graph [p].state].prolis;
	    seek_paths (p, 0, false, LGPROD (parse_stack.red_no));

	}
	else  {
	    parse_stack.graph [p].red_ref = 0;
	}
    }

    while (parse_stack.red_stack [0] > 0) {
	POP (parse_stack.red_stack, ref);
	
	if ((x = parse_stack.red_parsers_hd [ref]) < 0) {
	    parse_stack.red_no = ref;
	    seek_paths (-x, 0, false, LGPROD (parse_stack.red_no));
	}
	else {
	    do {
		p = parse_stack.red_parsers_list [x].parser;
		
		if ((x = parse_stack.red_parsers_list [x].lnk) != 0)
		    SXBA_1_bit (parse_stack.parsers_set [1], p);
	    } while (x != 0);
	    
	    parse_stack.red_no = ref;
	    seek_paths (p, 0, true, LGPROD (parse_stack.red_no));
	}
    }
    
    parse_stack.red_parsers_list_top = 0;
}


static bool shifter ()
{
    SXINT		p, q, ref, n, son, lgth;

    /* active_parsers == phi */
    /* for_actor == phi */

    if (parse_stack.for_shifter [0] < 0)
	return false;

    do {
	POP (parse_stack.for_shifter, p);
	SS_push (parse_stack.for_scanner, parse_stack.active_parsers [p]);
    } while (parse_stack.for_shifter [0] > 0);

    parse_stack.current_level = sxplocals.ptok_no;

    new_symbol (-parse_stack.current_token,
		0,
		parse_stack.current_level - 1,
		parse_stack.current_level,
		&parse_stack.current_symbol);

    if (parse_stack.current_token != sxplocals.SXP_tables.P_tmax)
	set_t_rule (parse_stack.current_symbol);

    /* read next token */
    n = ++sxplocals.ptok_no;

    while (n > sxplocals.Mtok_no)
	(*(sxplocals.SXP_tables.P_scan_it)) ();

    parse_stack.current_token = SXGET_TOKEN (n).lahead;
    /* for_scanner = for_shifter */
    /* for_shifter = phi */

    do {
	p = SS_pop (parse_stack.for_scanner);
	ref = parse_stack.graph [p].shift_ref;
	parse_stack.graph [p].shift_ref = 0;

	if (ref < 0 /* scan and reduce */) {
	    /* in-lining */
	    do_pops (p,
		     lgth = -ref - bnf_ag.WS_NBPRO [parse_stack.red_no = bnf_ag.WS_INDPRO [-ref].prolis].prolon);
	    seek_paths (p, push_rhs (parse_stack.current_symbol, 0), false, lgth);
	}
	else
	    add_active_parser (p, ref, parse_stack.current_symbol);
    } while (!SS_is_empty (parse_stack.for_scanner));
    
    return true;
}



static void unused_rule_elimination (start_symbol)
    SXINT start_symbol;
{
    /* Let R be the relation : x R y <=> x -> alpha y beta in P.
       A rule p is kept iff p = A -> alpha and A in {y | s R+ y}. */

    SXBA	*R_plus, line;
    SXINT		size, lim, rule_no, lim1, x, symbol;

    size = XxY_top (parse_stack.symbols) + 1;
    R_plus = sxbm_calloc (size, size);

    for (lim = XH_top (parse_stack.output_grammar),
	 rule_no = 1;
	 rule_no < lim;
	 rule_no++) {
	lim1 = XH_X (parse_stack.output_grammar, rule_no + 1);
	x  = XH_X (parse_stack.output_grammar, rule_no);
	line = R_plus [XH_list_elem (parse_stack.output_grammar, x)];

	while (++x < lim1) {
	    if ((symbol = XH_list_elem (parse_stack.output_grammar, x)) > 0)
		SXBA_1_bit (line, symbol);
	}
    }

    fermer (R_plus, size);

    line = R_plus [start_symbol];
    SXBA_1_bit (line, start_symbol);

    parse_stack.G.N = sxba_cardinal (line);
    parse_stack.G.T = parse_stack.current_level;
    parse_stack.G.G = parse_stack.G.P = 0;

    for (rule_no = 1;
	 rule_no < lim;
	 rule_no++) {
	x = XH_X (parse_stack.output_grammar, rule_no);
	symbol = XH_list_elem (parse_stack.output_grammar, x);

	if (SXBA_bit_is_set (line, symbol)) {
	    ++parse_stack.G.P;
	    parse_stack.G.G += XH_X (parse_stack.output_grammar, rule_no + 1) - x;
	}
	else
	    XH_erase (parse_stack.output_grammar, rule_no);
    }

    sxbm_free (R_plus);
}



static void output_grammar (start_symbol)
    SXINT start_symbol;
{
    SXINT lim, rule_no;

    printf ("*\t(|N| = %i, |T| = %i, |P| = %i, |G| = %i)\n\n",
	  parse_stack.G.N,  
	  parse_stack.G.T,  
	  parse_stack.G.P,  
	  parse_stack.G.G);

    if (start_symbol > 0) {
	fputs ("<start_symbol> \t= ", stdout);
	output_symbol (start_symbol);
	putchar (';');
	putchar ('\n');
    }
    else
	fputs ("*\tThere is no start symbol in this parse forest.\n\n", stdout);

    for (lim = XH_top (parse_stack.output_grammar),
	 rule_no = 1;
	 rule_no < lim;
	 rule_no++) {
	if (!XH_is_erased (parse_stack.output_grammar, rule_no)) {
	   output_grammar_rule (rule_no); 
	}
    }
}



static bool sxparse_it ()
{
    SXINT				y, p, ref, lgth, lim;
    struct parse_stack_elem	*agraph;
    struct SXP_prdct		*aprdct;
    
    parse_stack.start_symbol = 0;
    parse_stack.current_token = sxplocals.SXP_tables.P_tmax;

    /* create a stack node p with state START_STATE(grammar) */
    p = parse_stack.graph_top = 1;
    agraph = &(parse_stack.graph [p]);
    agraph->state = 1; /* why not! */
    agraph->red_ref = 0;
    agraph->shift_ref = 1;

    /* no son */
    agraph->son = 0;
    agraph->level = 0;
    /* agraph->symbol = ... */
    PUSH (parse_stack.for_shifter, agraph->state);
    parse_stack.active_parsers [agraph->state] = p;
    parse_stack.rhs_top = 0;
    shifter ();

    /* in_lining of parseword () */
    do {
	/* for_actor == active_parsers */
	/* for_shifter == phi */
	if (parse_stack.for_actor [0] != -1) {
	    /* while for_actor <> phi  do */
	    do {
		/* remove a parser p from for_actor */
		POP (parse_stack.for_actor, p);
		p = parse_stack.active_parsers [p];

		if ((ref = parse_stack.graph [p].red_ref) == 0) {
		    /* Shift and reduce parser */
		    ref = parse_stack.graph [p].state - lr0_automaton_state_nb;
		}

		if (ref > 0) {
		    parse_stack.red_no = ref;
		    do_pops (p, LGPROD (parse_stack.red_no));
		}
		else {
		    struct Q0	*aQ0 = Q0 - ref;

		    lim = aQ0->red_trans_nb + (ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);

		    do {
			parse_stack.red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
			do_pops (p, LGPROD (parse_stack.red_no));
		    } while (++ref < lim);
		}
	    } while (parse_stack.for_actor [0] != -1);

	    while (parse_stack.is_new_links) {
		parse_stack.is_new_links = false;
		sxba_copy (parse_stack.used_links, parse_stack.current_links);
		sxba_empty (parse_stack.current_links);

		/* for_actor == active_parsers */
		p = 0;

		while ((p = parse_stack.active_states [p]) > 0)
		    PUSH (parse_stack.for_actor, p);

		do {
		    /* remove a parser p from for_actor */
		    POP (parse_stack.for_actor, p);
		    p = parse_stack.active_parsers [p];
		
		    if ((ref = parse_stack.graph [p].red_ref) == 0) {
			/* Shift and reduce parser */
			ref = parse_stack.graph [p].state - lr0_automaton_state_nb;;
		    }
		    
		    if (ref > 0) {
			if ((lgth = LGPROD (ref)) > 0) {
			    parse_stack.red_no = ref;
			    do_limited_reductions (p, lgth);
			}
		    }
		    else {
			struct Q0	*aQ0 = Q0 - ref;

			lim = aQ0->red_trans_nb + (ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);

			do {
			    parse_stack.red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;

			    if ((lgth = LGPROD (parse_stack.red_no)) > 0)
				do_limited_reductions (p, lgth);
			} while (++ref < lim);
		    }
		} while (parse_stack.for_actor [0] != -1);
	    }

	    parse_forest ();
	    sxba_empty (parse_stack.active_sons);
	}
    } while (shifter ());

    if (parse_stack.start_symbol == 0)
	sxerror (SXGET_TOKEN (sxplocals.ptok_no).source_index,
		 sxplocals.sxtables->err_titles [2][0],
		 "%sSyntax error.",
		 sxplocals.sxtables->err_titles [2]+1);
    else
	unused_rule_elimination (parse_stack.start_symbol);

    output_grammar (parse_stack.start_symbol);

    return parse_stack.start_symbol > 0;
}


bool sxindparser (what_to_do, arg)
    SXINT		what_to_do;
    SXTABLES	*arg;
{
    switch (what_to_do) {
    case SXBEGIN:
	return true;

    case SXOPEN:
	/* new language: new tables, new local parser variables */
	/* the global variable "sxplocals" must have been saved in case */
	/* of recursive invocation */

	/* test arg->magic for consistency */
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, ME);

	/* internal grammar form reading. */
	/* Assume that the name of the language has been given in the "record" spec!! */
	if (!bnf_read (&bnf_ag, arg->err_titles [0] + 1))
	    sxexit (2);

	/* LR (0) automaton construction. */
	LR0 ();
	XH_lock (&nucleus_hd);

	lr0_automaton_state_nb = xac2 - 1;

	parse_stack.graph = (struct parse_stack_elem*) sxalloc ((parse_stack.graph_size = 128) + 1,
							sizeof (struct parse_stack_elem));
	parse_stack.graph_top = 0;
	parse_stack.free_parsers_set = sxba_calloc (parse_stack.graph_size + 1);
	sxba_fill (parse_stack.free_parsers_set);
	parse_stack.free_parser = 0;
	parse_stack.parsers_set = sxbm_calloc (4, parse_stack.graph_size + 1);
	parse_stack.parsers_rhs0 = (SXINT*) sxalloc (parse_stack.graph_size + 1, sizeof (SXINT));
	parse_stack.parsers_rhs1 = (SXINT*) sxalloc (parse_stack.graph_size + 1, sizeof (SXINT));

        {
	    SXINT SIZE = lr0_automaton_state_nb + bnf_ag.WS_TBL_SIZE.xnbpro + 1;

	    parse_stack.active_states = (SXINT*) sxcalloc (SIZE, sizeof (SXINT));
	    parse_stack.active_states [0] = -1;
	    parse_stack.for_actor = (SXINT*) sxcalloc (SIZE, sizeof (SXINT));
	    parse_stack.for_actor [0] = -1;
	    parse_stack.for_shifter = (SXINT*) sxcalloc (SIZE, sizeof (SXINT));
	    parse_stack.for_shifter [0] = -1;
	    parse_stack.active_parsers = (SXINT*) sxalloc (SIZE, sizeof (SXINT));
	    parse_stack.active_sons = sxba_calloc (SIZE);
	}

        {
	    SXINT SIZE = arg->SXP_tables.Mred + 1;

	    parse_stack.red_stack = (SXINT*) sxcalloc (SIZE, sizeof (SXINT));
	    parse_stack.red_stack [0] = -1;
	    parse_stack.red_parsers_hd = (SXINT*) sxalloc (SIZE, sizeof (SXINT));
	    parse_stack.red_parsers_list = (struct red_parsers*) sxalloc (parse_stack.red_parsers_list_size = SIZE, sizeof (struct red_parsers));
	    parse_stack.red_parsers_list_top = 0;
	}

	parse_stack.for_scanner = SS_alloc (20);
	parse_stack.GC_area = SS_alloc (20);

	XxY_alloc (&parse_stack.sons, "sons", 128, 1, 1, 0, sons_oflw, NULL);
	
	parse_stack.erasable_sons = sxba_calloc (XxY_size (parse_stack.sons) + 1);
	parse_stack.current_links = sxba_calloc (XxY_size (parse_stack.sons) + 1);
	parse_stack.used_links = sxba_calloc (XxY_size (parse_stack.sons) + 1);
	parse_stack.transitions = (SXINT*) sxalloc (XxY_size (parse_stack.sons) + 1, sizeof (SXINT));


	XxY_alloc (&parse_stack.suffixes, "suffixes", 128, 1, 0, 0, NULL, NULL);
	XxY_alloc (&parse_stack.ranges, "ranges", 128, 1, 0, 0, NULL, NULL);
	XxY_alloc (&parse_stack.symbols, "symbols", 128, 1, 0, 0, NULL, NULL);

	XH_alloc (&parse_stack.output_grammar, "output_grammar", 128, 1, 4, NULL, NULL);

	parse_stack.rhs = (struct rhs*) sxalloc ((parse_stack.rhs_size = 50) + 1, sizeof (struct rhs));

/* prepare new local variables */

	sxplocals.sxtables = arg;
	sxplocals.SXP_tables = arg->SXP_tables;
	sxtkn_mngr (SXOPEN, NULL, sxplocals.SXP_tables.P_sizofpts * 8);
	(*sxplocals.SXP_tables.P_recovery) (SXOPEN, NULL /* dummy */, (SXINT) 0 /* dummy */);
	return true;

    case SXINIT:
	/* on initialise toks_buf avec "EOF" */

    {
	struct sxtoken tok;

	/* terminal_token EOF */
	tok.lahead = sxplocals.SXP_tables.P_tmax;
	tok.string_table_entry = SXEMPTY_STE;
	tok.source_index = sxsrcmngr.source_coord;
	tok.comment = NULL;
	sxtkn_mngr (SXINIT, &tok, 0);
    }
    
	/* analyse normale */
	/* valeurs par defaut qui peut etre changee ds les
	   scan_act ou pars_act. */
	sxplocals.mode.look_back = 1; 
	sxplocals.mode.kind = SXWITH_RECOVERY;
	sxplocals.mode.errors_nb = 0;
	sxplocals.mode.is_prefixe = false;
	sxplocals.mode.is_silent = false;
	sxplocals.mode.with_semact = true;
	sxplocals.mode.with_parsact = true;
	sxplocals.mode.with_parsprdct = true;
	return true;

    case SXACTION:
	return sxparse_it ();

    case SXFINAL:
	sxtkn_mngr (SXFINAL, NULL, 0);
	return true;

    case SXCLOSE:
	/* end of language: free the local arrays */
	sxfree (parse_stack.graph);
	sxfree (parse_stack.free_parsers_set);
	sxfree (parse_stack.parsers_set);
	sxfree (parse_stack.active_sons);
 	sxfree (parse_stack.active_states);
	sxfree (parse_stack.for_shifter);
	sxfree (parse_stack.active_parsers);
 	sxfree (parse_stack.for_actor);
	SS_free (parse_stack.for_scanner);
	SS_free (parse_stack.GC_area);
	sxfree (parse_stack.red_stack);
	sxfree (parse_stack.red_parsers_hd);
	sxfree (parse_stack.red_parsers_list);
	sxfree (parse_stack.transitions);
	XxY_free (&parse_stack.sons);
	XxY_free (&parse_stack.suffixes);
	XxY_free (&parse_stack.ranges);
	XxY_free (&parse_stack.symbols);
	XH_free (&parse_stack.output_grammar);
	sxfree (parse_stack.erasable_sons);
	sxfree (parse_stack.current_links);
	sxfree (parse_stack.used_links);
	sxfree (parse_stack.rhs);
	sxfree (parse_stack.parsers_rhs0);
	sxfree (parse_stack.parsers_rhs1);

	sxtkn_mngr (SXCLOSE, NULL, 0);
	bnf_free (&bnf_ag);
	return true;

    case SXEND:
	(*sxplocals.SXP_tables.P_recovery) (SXCLOSE, NULL /* dummy */, (SXINT) 0 /* dummy */);
	return true;

    default:
	fprintf (sxstderr, "The function \"sxindparser\" is out of date with respect to its specification.\n");
	sxexit(1);
    }
}

