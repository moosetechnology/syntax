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

/* Non deterministic tree walker */

static char ME [] = "Non deterministic tree walker";

#include "sxversion.h"
#include "sxcommon.h"
# include <stdio.h>
# include "sxnd.h"

char WHAT_SXNDTREE_WALKER[] = "@(#)SYNTAX - $Id: sxndtreewalker.c 4143 2024-08-02 08:50:12Z garavel $" WHAT_DEBUG;

static SXINT	lgt1;


SXINT sxndtw_is_cycle (SXINT node)
{
    /* If there is a cycle along the path designated by node this function returns
       the prefix of this path ending at the beginning of the cycle else it returns 0. */
    SXINT	item = sxndtw_node_to_item (node);

    while ((node = sxndtw_node_to_father (node)) > 0) {
	if (item == sxndtw_node_to_item (node))
	    break;
    }

    return node;
}


static void paths_oflw (SXINT old_size, SXINT new_size)
{

    sxndtw.is_erased = sxba_resize (sxndtw.is_erased, new_size + 1);

    if (sxndtw.nodes_oflw != NULL)
	(*sxndtw.nodes_oflw) (old_size, new_size); 
}


void	sxndtw_open (SXINT    (*pass_inherited) (void),
		     SXINT    (*pass_derived) (void),
		     bool  (*cycle_processing) (void),
		     void     (*open_hook) (void), 
		     void     (*close_hook) (void), 
		     void     (*nodes_oflw) (SXINT, SXINT) )
{
    sxuse(cycle_processing); /* pour faire taire gcc -Wunused */
    sxndtw.pass_inherited = pass_inherited;
    sxndtw.pass_derived = pass_derived;
    sxndtw.open_hook = open_hook;
    sxndtw.close_hook = close_hook;
    sxndtw.nodes_oflw = nodes_oflw;
}


void	sxndtw_init (void)
{
    SXINT item;

    if (parse_stack.root > 0) {
	XxY_alloc (&sxndtw.paths,
		   "paths",
		   parse_stack.grammar_top,
		   1,
		   0,
		   0,
		   paths_oflw,
		   NULL);
	sxndtw.is_erased = sxba_calloc (XxY_size (sxndtw.paths) + 1);
	sxndtw.reached_symbol = sxba_calloc (parse_stack.rule_top + 1);
	sxndtw.already_traversed = sxba_calloc (parse_stack.rule_top + 1);
	
	if (sxndtw.nodes_oflw != NULL)
	    /* initialisation */
	    (*sxndtw.nodes_oflw) (0, XxY_size (sxndtw.paths) + 1);
	
	(void) sxndtw_node_to_son (0, 0, NDFATHER);
	sxndtw.root = NDFATHER;
#if 0
	/* Le 09/05/2003 on a change' par ci-dessus */
	NDFATHER = sxndtw.root = 0;
#endif
	NDFATHER_SYMBOL = parse_stack.root;
	NDFATHER_REDNO = 0;
	item = sxndtw_symb_to_lhsitem (NDFATHER_SYMBOL) + 1;
	(void) sxndtw_node_to_son (NDFATHER, item, NDVISITED);
	NDVISITED_POSITION = 1;
	NDVISITED_SYMBOL = sxndtw_item_to_symb (item);
    }
}


void	sxndtw_final (void)
{
    if (parse_stack.root > 0) {
	XxY_free (&sxndtw.paths);
	sxfree (sxndtw.is_erased);
	sxfree (sxndtw.reached_symbol);
	sxfree (sxndtw.already_traversed);
	
	if (sxndtw.nodes_oflw != NULL)
	    /* initialisation */
	    (*sxndtw.nodes_oflw) (XxY_size (sxndtw.paths) + 1, 0);
    }
}


void	sxndtw_close (void)
{
    sxndtw.pass_inherited = NULL;
    sxndtw.pass_derived = NULL;
    sxndtw.open_hook = NULL;
    sxndtw.close_hook = NULL;
    sxndtw.nodes_oflw = NULL;
}


void	sxndtw_node_erase (SXINT n)
{
    /* Indique que le sous-arbre "n" est supprime' et "recupere" les
       noeuds INTERNES de ce sous_arbre. */
    SXINT arity, son, i, symbol, item;

    if ((symbol = sxndtw_node_to_symb (n)) > 0) {
	item = sxndtw_symb_to_lhsitem (symbol);
	
	for (arity = sxndtw_symb_to_arity (symbol), i = 1; i <= arity; i++) {
	    item++;
	    /* On ne descend que sur les calcules, ca permet entre autre
	       d'eviter les cycles. */
	    son = XxY_is_set (&sxndtw.paths, n, item);
	    
	    if (son > 0 && !sxndtw_is_node_erased (son))
		sxndtw_node_erase (son);
	    
	    XxY_erase (sxndtw.paths, son);
	}
    }
}



void	sxndtw_walk (void)
{
  SXINT p, arity, item, lhs_item;

  if (parse_stack.root == 0)
    return;

#if 0
  /* Le 30/04/2003 */
  sxndtw.pv.node = sxndtw.root;
  (void) sxndtw_node_to_son (sxndtw.root,
		      sxndtw_symb_to_lhsitem (sxndtw_node_to_symb (sxndtw.root)) + 1,
		      sxndtw.nv.node);
#endif /* 0 */
  NDPREV_NODE = NDVISITED;
  (void) sxndtw_node_to_son (NDVISITED,
		      sxndtw_symb_to_lhsitem (sxndtw_node_to_symb (NDVISITED)) + 1,
		      sxndtw.nv.node);

  sxndtw.nv.kind = SXINHERITED;

  while ((NDVISITED = sxndtw.nv.node) != sxndtw.root) {
    item = sxndtw_node_to_item (NDVISITED);
    NDVISITED_SYMBOL = sxndtw_item_to_symb (item);
    NDVISITED_POSITION = sxndtw_item_to_pos (item);
    NDVISITED_ARITY = sxndtw_node_to_arity (NDVISITED);
    NDFATHER = sxndtw_node_to_father (NDVISITED);

    if (NDFATHER == sxndtw.root) {
      NDFATHER_SYMBOL = parse_stack.root;
      NDFATHER_REDNO = 0;
    }
    else {
      NDFATHER_SYMBOL = sxndtw_item_to_symb (sxndtw_node_to_item (NDFATHER));
      NDFATHER_REDNO = sxndtw_symb_to_ref (NDFATHER_SYMBOL);
    }

    if (sxndtw.nv.kind == SXINHERITED) {
      if (NDVISITED_SYMBOL <= 0 /* terminal */ ||
	  NDVISITED_ARITY == 0 /* no son */) {
	sxndtw.nv.node = NDVISITED;
	sxndtw.nv.kind = SXDERIVED;
      }
      else {
	/* It exists some sons. */
	lhs_item = sxndtw_symb_to_lhsitem (NDVISITED_SYMBOL);

	for (p = 1; p <= NDVISITED_ARITY; p++) {
	  (void) sxndtw_node_to_son (NDVISITED, lhs_item + p, sxndtw.nv.node);

	  if (!sxndtw_is_node_erased (sxndtw.nv.node))
	    break;
	}

	if (p > NDVISITED_ARITY) {
	  sxndtw.nv.node = NDVISITED;
	  sxndtw.nv.kind = SXDERIVED;
	}
      }

      (*sxndtw.pass_inherited) ();
    }
    else {
      arity = sxndtw_node_to_arity (NDFATHER);

      for (p = NDVISITED_POSITION + 1; p <= arity; p++) {
	++item;
	(void) sxndtw_node_to_son (NDFATHER, item, sxndtw.nv.node);

	if (!sxndtw_is_node_erased (sxndtw.nv.node))
	  break;
      }

      if (p > arity)
	sxndtw.nv.node = NDFATHER;
      else
	sxndtw.nv.kind = SXINHERITED;		

      (*sxndtw.pass_derived) ();
      
      if (NDVISITED_SYMBOL > 0)
	SXBA_1_bit (sxndtw.already_traversed, NDVISITED_SYMBOL);
    }
    
    NDPREV_NODE = NDVISITED;
  }
}



static bool sxndtw_reached_symbol (SXINT node, SXINT symbol)
{
    /* Calcule sxndtw.reached_symbol, l'ensemble des symboles qui sont atteints
       (il existe au moins un chemin) depuis la racine. */
    SXINT		i, arity, son_nb, son, item;
    bool	is_a_subtree;

    if (SXBA_bit_is_reset_set (sxndtw.reached_symbol, symbol)) {
	if ((arity = sxndtw_symb_to_arity (symbol)) == 0)
	    return true;
	
	item = sxndtw_symb_to_lhsitem (symbol);
	is_a_subtree = true;
	
	if (symbol >= parse_stack.hook_rule) {
	    /* visited node is the root of a hook. */
	    son_nb = 0;
	    
	    for (i = 1; i <= arity; i++) {
		item++;
		(void) sxndtw_node_to_son (node, item, son);
		
		if (!sxndtw_is_node_erased (son)) {
		    /* Non erased sub-tree. */
		    son_nb++;
		    
		    if (!sxndtw_reached_symbol (son, sxndtw_item_to_symb (item)))
			is_a_subtree = false;
		}
	    }
	    
	    return son_nb == 1 && is_a_subtree; /* false => ambigu */
	}
	else {
	    for (i = 1; i <= arity; i++) {
		item++;

		if ((symbol = sxndtw_item_to_symb (item)) > 0) {
		    (void) sxndtw_node_to_son (node, item, son);
		
		    if (!sxndtw_reached_symbol (son, symbol))
			is_a_subtree = false;
		}
	    }
	    
	    return is_a_subtree;
	}
    }

    return false; /* ambigu */
}


void sxndtw_check_grammar (void)
{
    /* Calcule la (nouvelle) taille de la foret. */
    /* Calcule reached_symbol l'ensemble des symboles accessibles. */
    /* Un symbole est accessible ssi il existe au moins un chemin dynamique
       qui le traverse. */
    /* Les hooks singletons sont elimines. */
    /* Les regles inutiles (n'appartenant a aucun chemin actif) sont mis a zero. */
    /* positionne is_ambiguous si la foret n'est pas un arbre */
    /* Les "erase" ont ete remontes jusqu'aux hooks (ou la racine) */
    SXINT		symbol, son_nb, arity, lhsitem, item, i, son_symb, rule, lim, single;

    sxinitialise(single); /* pour faire taire "gcc -Wuninitialized" */
    parse_stack.is_ambiguous = !sxndtw_reached_symbol (sxndtw.root, parse_stack.root);

    for (symbol = parse_stack.rule_top;
	 symbol >= parse_stack.hook_rule;
	 symbol--) {
	if (SXBA_bit_is_set (sxndtw.reached_symbol, symbol)) {
	    son_nb = 0;
	    arity = sxndtw_symb_to_arity (symbol);
	    lhsitem = item = sxndtw_symb_to_lhsitem (symbol);
	    
	    for (i = 1; i <= arity; i++) {
		item++;
		son_symb = sxndtw_item_to_symb (item);
		
		if (SXBA_bit_is_set (sxndtw.reached_symbol, son_symb)) {
		    son_nb++;
		    single = son_symb;
		}
		else
		    sxndtw_item_erase (item);
	    }

	    if (son_nb == 1) {
		SXBA_0_bit (sxndtw.reached_symbol, symbol);
		sxndtw_item_to_symb (lhsitem) = single;
	    }
	}
    }

    parse_stack.G.T = sxplocals.Mtok_no - 1;
    parse_stack.G.G = parse_stack.G.N = parse_stack.G.P = 0;

    for (rule = 1;
	 rule <= parse_stack.rule_top;
	 rule++) {
	if (SXBA_bit_is_set (sxndtw.reached_symbol, rule)) {
	    parse_stack.G.P++;
	    parse_stack.G.G++;
	    item = parse_stack.rule [rule].hd;
	    lim = item + parse_stack.rule [rule].lgth;

	    while (++item < lim) {
		parse_stack.G.G++;
		symbol = sxndtw_item_to_symb (item);

		if (symbol > 0) {
		    parse_stack.G.N++;

		    if (symbol >= parse_stack.hook_rule
			&& !SXBA_bit_is_set (sxndtw.reached_symbol, symbol))
			/* hook singleton */
			parse_stack.grammar [item] =
			    parse_stack.grammar [sxndtw_symb_to_lhsitem (symbol)];
		}
	    }
	}
	else
	    parse_stack.rule [rule].lgth = 0;  
    }
}


static void sxndtw_execute_actions (SXINT symbol)
{
    SXINT i, arity, item, red_no, new_top, action;

    if (symbol <= 0) {
	if (sxpglobals.xps >= lgt1)
	    sxpglobals.parse_stack = (struct sxparstack*)
		sxrealloc (sxpglobals.parse_stack, (lgt1 *= 2) + 1, sizeof (struct sxparstack));
	
	sxplocals.atok_no = -symbol;
	sxpglobals.parse_stack [++sxpglobals.xps].token = SXGET_TOKEN (sxplocals.atok_no);
    }
    else {
	arity = sxndtw_symb_to_arity (symbol);

	if (symbol >= parse_stack.hook_rule) {
	    /* " node" is the root of a hook. */
	    /* Should not occur */
	    sxtrap (ME, "sxndtw_execute_actions");
	}
	else {
	    if (arity == 0) {
		new_top = sxpglobals.xps;

		if (new_top++ >= lgt1)
		    sxpglobals.parse_stack = (struct sxparstack*)
			sxrealloc (sxpglobals.parse_stack, (lgt1 *= 2) + 1,
				   sizeof (struct sxparstack));

		sxpglobals.pspl = -1;		
	    }
	    else {
		item = sxndtw_symb_to_lhsitem (symbol);

		for (i = 1; i <= arity; i++) {
		    item++;
		    sxndtw_execute_actions (sxndtw_item_to_symb (item));
		}

		sxpglobals.pspl = arity - 1;
		new_top = sxpglobals.xps - sxpglobals.pspl;
	    }

	    red_no = sxndtw_symb_to_red_no (symbol);
	    action = sxndtw_symb_to_action (symbol);
	    sxplocals.atok_no++; /* repere le look_ahead */

	    if ((sxpglobals.reduce = red_no) > 0 &&
		red_no <= sxplocals.SXP_tables.P_nbpro &&
		sxis_semantic_action (action))
		sxplocals.SXP_tables.P_semact (SXACTION, action, NULL); 

	    sxpglobals.parse_stack [sxpglobals.xps = new_top].token.lahead = 0 /* nt */;
	}
    }
}



bool sxndtw_sem_calls (void)
{
    /* Assume grammar is a tree. */
    /* Simulate the calls to the semantics as if the grammar was deterministic. */
    /* Must also work if the parse-forest was initially a tree. */

    /* init */
    sxpglobals.parse_stack =
	(struct sxparstack*) sxalloc ((lgt1 = 200) + 1,
				      sizeof (struct sxparstack));
    sxpglobals.xps = 0;
    /* sxplocals.atok_no = 0; First EOF */

    /* action */
    sxndtw_execute_actions (parse_stack.start_symbol);

    /* final */
    sxfree (sxpglobals.parse_stack), sxpglobals.parse_stack = NULL;
    return true;
}


bool	sxndrtw_walk (void)
{
    /* recursive walk */
    /* if sxndtw.pass_derived () returns a negative SXINT, the subtree is revisited. */
    /* Returns false iff the walk must go on. */
    SXINT		father, father_symbol, visited, visited_symbol, visited_pos;
    SXINT		visited_arity, hook_pos, hook, hook_arity, son, son_pos, son_item, lhs_item, hook_item, hook_symbol, father_redno, sxndrtw_walk_what;
    bool	quit;

    father = NDFATHER;
    father_symbol = NDFATHER_SYMBOL;
    father_redno = NDFATHER_REDNO = sxndtw_symb_to_red_no (father_symbol);
    visited = NDVISITED;
    visited_symbol = NDVISITED_SYMBOL;
    visited_pos = NDVISITED_POSITION;
    visited_arity = NDVISITED_ARITY = sxndtw_symb_to_arity (visited_symbol);

    if (visited_symbol > 0) {
	if (SXBA_bit_is_set (sxndtw.reached_symbol, visited_symbol)) {
	    if (sxndtw.cycle_processing != NULL)
		(*sxndtw.cycle_processing) ();

	    return true; /* Pour l'instant */
	}

	SXBA_1_bit (sxndtw.reached_symbol, visited_symbol);
    }

    do {
	sxndrtw_walk_what = 0;

	if (sxndtw.pass_inherited != NULL && (*sxndtw.pass_inherited) () > 0)
	    quit = true;
	else {
	    NDFATHER = visited;
	    NDFATHER_SYMBOL = visited_symbol;
	    lhs_item = sxndtw_symb_to_lhsitem (visited_symbol);
	    
	    if (visited_symbol >= parse_stack.hook_rule /* a_hook */) {
		/* A hook is kept iff at least one of its son is kept. */
		quit = true;
		
		for (hook_pos = 1; hook_pos <= visited_arity; hook_pos++) {
		    hook_item = lhs_item + (NDVISITED_POSITION = hook_pos);
		    NDVISITED_SYMBOL = hook_symbol = sxndtw_item_to_symb (hook_item);
		    NDVISITED = sxndtw_node_to_son (visited, hook_item, hook);
		    NDVISITED_ARITY = hook_arity = sxndtw_symb_to_arity (hook_symbol);
		    /* Assume not a hook */
		    
		    if (sxndtw.open_hook != NULL)
			(*sxndtw.open_hook) ();
		    
		    NDFATHER = hook;
		    NDFATHER_SYMBOL = hook_symbol;
		    hook_item = sxndtw_symb_to_lhsitem (hook_symbol);
		    
		    for (son_pos = 1; son_pos <= hook_arity; son_pos++) {
			son_item = hook_item + (NDVISITED_POSITION = son_pos);
			NDVISITED_SYMBOL = sxndtw_item_to_symb (son_item);
			NDVISITED = sxndtw_node_to_son (hook, son_item, son);
			
			if (sxndrtw_walk ()) {
			    SXBA_1_bit (sxndtw.is_erased, hook); /* pour close_hook () */
			    break;
			}
		    }
		    
		    NDFATHER = visited;
		    NDFATHER_SYMBOL = visited_symbol;
		    NDVISITED = hook;
		    NDVISITED_SYMBOL = hook_symbol;
		    NDVISITED_POSITION = hook_pos;
		    NDVISITED_ARITY = hook_arity;
		    
		    if (sxndtw.close_hook != NULL)
			(*sxndtw.close_hook) ();
		    
		    if (son_pos > hook_arity)
			quit = false;
		    else
			sxndtw_node_erase (hook);
		}
	    }
	    else {
		quit = false; /* si visited_arity == 0 */

		for (son_pos = 1; son_pos <= visited_arity; son_pos++) {
		    son_item = lhs_item + (NDVISITED_POSITION = son_pos);
		    NDVISITED_SYMBOL = sxndtw_item_to_symb (son_item);
		    NDVISITED = sxndtw_node_to_son (visited, son_item, son);
		    
		    if ((quit = sxndrtw_walk ()))
			break;
		}
	    }
	    
	    NDFATHER = father;
	    NDFATHER_SYMBOL = father_symbol;
	    NDFATHER_REDNO = father_redno;
	    NDVISITED = visited;
	    NDVISITED_SYMBOL = visited_symbol;
	    NDVISITED_POSITION = visited_pos;
	    NDVISITED_ARITY = visited_arity;
	    
	    if (!quit) {
		if (sxndtw.pass_derived != NULL) {
		    if ((sxndrtw_walk_what = (*sxndtw.pass_derived) ()) > 0)
			quit = true;
		    /* Si sxndrtw_walk_what < 0 => on reboucle sur le sous arbre */
		}
	    }
	}
    } while (sxndrtw_walk_what < 0);
    
    if (visited_symbol > 0)
	SXBA_0_bit (sxndtw.reached_symbol, visited_symbol);

    return quit;
}
