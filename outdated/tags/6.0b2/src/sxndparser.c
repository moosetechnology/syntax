/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                	     (PB)			  *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030506 17:08 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 20030319 10:48 (phd):	Utilisation de "sxtimestamp"		*/
/************************************************************************/
/* Mer 11 Fev 2003 11:25(pb):	Bug ds le traitement des XNT ds le cas  */
/*                              du left-corner                          */
/************************************************************************/
/* Mer 5 Fev 2003 15:45(pb):	si les tables sont compilees avec       */
/*                              l'option -Dsxndprecovery=sxbvoid        */
/*				Ca permet de ne pas faire de recovery   */
/************************************************************************/
/* Mer 10 Nov 1999 16:38(pb):	Bug si l'automate sous-jacent est	*/
/*				left-corner, on peut trouver des	*/
/*				transitions non-terminales 		*/
/*				non-deterministes 			*/
/************************************************************************/
/* 30-05-95 16:47 (pb):		Appel de GC avec le parser que l'on 	*/
/*				vient de creer en parametre.		*/
/************************************************************************/
/* 30-05-95 14:40 (pb):		Bug ds do_limited qu'on pouvait quitter	*/
/*				avec un parser_sets non vide.		*/
/************************************************************************/
/* 26-04-95 15:29 (pb):		On remplace "symbol" ds les parser par	*/
/*				"reduce". Leur nb reste borne par niveau*/
/************************************************************************/
/* 24-04-95 15:33 (pb):		Bug ds parse_forest			*/
/************************************************************************/
/* 21-04-95 10:51 (pb):		Suppression des "unit_rules" qui ne 	*/
/*				marchent pas ds les cas tordus. 2 trans	*/
/* 				differentes peuvent etre labellees l'une*/
/*				par A et B et l'autre seulement par A.	*/
/*				Cette trans n'est pas equiv. a la 1ere.	*/
/*				On ajoute "symbol" aux parsers		*/
/************************************************************************/
/* 19-04-95 11;47 (pb):		Bug ds unused_rule_elimination, refonte */
/*				du traitement des hooks.		*/
/************************************************************************/
/* 01-03-95 16:00 (pb):		Ajout de "item_hd" ds "rule" et de	*/
/*				"lhs_rule" et "next_item" ds grammar	*/
/************************************************************************/
/* 21-02-95 10:14 (pb):		Ajout de "third_field" ds "for_parsact"	*/
/*				ca permet d'ajouter des info semantiques*/
/*				au codage des symboles de la foret	*/				
/************************************************************************/
/* 15-02-95 15:54 (pb):		Bug dans "single_reduce" sur "ptriple"	*/
/************************************************************************/
/* 02-02-95 16:32 (pb):		Dans le cas shift-reduce le 2eme param	*/
/*				de action_new_top est -xtriple.		*/
/************************************************************************/
/* 01-02-95 17:02 (pb):		sxndsubparse_a_token prend un arg en +	*/
/*				qui est une ft				*/
/************************************************************************/
/* 23-11-94 15:27 (pb):		Ajout de "is_valid" pour parse_forest	*/
/*				red. qui conduisent a une impasse	*/
/************************************************************************/
/* 22-11-94 14:05 (pb):		Ajout de "re_do" afin de permettre aux	*/
/*				parsact d'agir sur le parser		*/
/************************************************************************/
/* 10-10-94 14:04 (pb):		Suppression du champ "for_do_limited"	*/
/************************************************************************/
/* 12-09-94 10:54 (pb):		VERSION PROVISOIRE : les post-actions	*/
/*				sont implantees ds ared->action et sont	*/
/*				>= 10000 et donc incompatibles avec des	*/
/*				actions semantiques sur la meme regle.	*/
/************************************************************************/
/* 19-08-94 14:38 (pb):		Les "codes" des sequences non		*/
/*				deterministes sont superieures a 20000	*/
/*				(et non plus a 10000)			*/
/************************************************************************/
/* 11-06-93 16:25 (pb):		Analyse non deterministe sur un dag de	*/
/*				tokens					*/
/************************************************************************/
/* 09-04-93 11:50 (pb):		Creation du XxY_header shared_forest	*/
/*				Suppression du XH output_grammar	*/
/************************************************************************/
/* 23-03-93 15:18 (pb):		Traitement d'erreur ds un ARC: analyse	*/
/*				non deterministe depuis le germe.	*/
/************************************************************************/
/* 01-03-93 15:28 (pb):		Appel de desambig via les P_tables	*/
/************************************************************************/
/* 22-02-93 15:40 (pb):		Suppression de la table "gotos"		*/
/*				Suppression des "adresses en pile"	*/
/*				Mfe, M0ref et Mref changent de		*/
/*				signification. Ajout de "final_state"	*/
/************************************************************************/
/* 29-12-92 15:40 (pb):		Bug ds GC, ds "shifter ()", le parser	*/
/*				courant, sommet de "for_scanner" etait	*/
/*				considere comme recuperable		*/
/*				immediatement				*/
/************************************************************************/
/* 28-12-92 14:45 (pb):		Bug ds GC, certains fils pouvaient etre	*/
/*				libres et neanmoins l'element (x, fils)	*/
/*				appartenir encore a "parse_stack.sons"	*/
/************************************************************************/
/* 19-11-92 18:05 (pb):		active_reduces est change en pile	*/
/*				pour corriger un bug avec do_limited	*/
/*				Les reductions des nouveaux parser crees*/
/*				par appel a do_limited devaient passer	*/
/*				par les used_links.			*/
/************************************************************************/
/* 28-09-92 11:30 (pb):		Les terminaux, dans la foret partagee	*/
/*				produite sont codes par des entiers	*/
/*				negatifs dont la valeur absolue est le	*/
/*				numero du token.			*/
/************************************************************************/
/* 20-05-92 16:55 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/


static char	ME [] = "NDPARSER";

#include        "sxcommon.h"
# include <stdio.h>
# include "sxnd.h"

#ifndef VARIANT_32
char WHAT_SXNDPARSER[] = "@(#)SYNTAX - $Id: sxndparser.c 620 2007-06-11 12:40:19Z rlacroix $" WHAT_DEBUG;
#else
char WHAT_SXNDPARSER[] = "@(#)SYNTAX - $Id: sxndparser.c 620 2007-06-11 12:40:19Z rlacroix $ SXNDPARSER_32" WHAT_DEBUG;
#endif
#if EBUG
# define stdout_or_NULL	stdout
#else
# define stdout_or_NULL	NULL
#endif

extern SXVOID	output_repair_list ();
extern SXVOID	output_repair_string ();


/************************************************************************/
/* It is the nondeterministic version of the SYNTAX parser		*/
/* This algorithm is borrowed from Jan Rekers' thesis			*/
/************************************************************************/

#define PUSH(T,x)	T [x] = T [0], T [0] = x
#define POP(T,x)	x = T [0], T [0] = T [x], T [x] = 0


static int	*rhs_stack, *symbol_stack, *symb_to_rule_hd, *symb_to_rule_list /*, *equiv, *equiv_list */;
static SXBA	hook_symbol_set;

#define Commun 1
#define Propre 2
#define Defaut 3
#define Done   4

VOID sxndparser_GC (int current_parser)
{
    /* Si current_parser > 0, il vient d'etre cree et il ne figure nulle part ailleurs, on
       le conserve. */
    /* Attention, le GC peut etre appele' meme s'il reste des places vides. */
    int				i, x, father, son, xarea, xmscan;
    BOOLEAN			is_in_sons = FALSE;
    struct sxmilstn_elem	*pm;

    if (parse_stack.GC_area == NULL)
	parse_stack.GC_area =
	    (int*) sxalloc ((parse_stack.GC_area_size = XxYxZ_size (parse_stack.parsers)) + 1,
			    sizeof (int));
    else if (parse_stack.GC_area_size < XxYxZ_size (parse_stack.parsers)) {
	parse_stack.GC_area =
	    (int*) sxrealloc (parse_stack.GC_area,
			      (parse_stack.GC_area_size = XxYxZ_size (parse_stack.parsers)) + 1,
			      sizeof (int));
    }

    xarea = 0;
    sxba_fill (parse_stack.free_parsers_set);

    if (current_parser > 0) {
        SXBA_0_bit (parse_stack.free_parsers_set, current_parser);
        parse_stack.GC_area [++xarea] = current_parser;
    }

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

    if ((pm = parse_stack.milstn_tail) != NULL) {
	do {
	    for (father = pm->next_for_scanner;
		 father > 0;
		 father = parse_stack.parser_to_attr [father].next_for_scanner) {
		if (SXBA_bit_is_set_reset (parse_stack.free_parsers_set, father)) {
		    parse_stack.GC_area [++xarea] = father;
		}
	    }
	} while ((pm = pm->next_milstn) != sxndtkn.milstn_current);
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
		is_in_sons = TRUE;
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
	if (parse_stack.parser_to_attr [x].scan_ref < 0) {
	    XxY_Xforeach (parse_stack.mscans, x, xmscan)
		XxY_erase (parse_stack.mscans, xmscan);
	}

	XxYxZ_erase (parse_stack.parsers, x);
	parse_stack.hole_nb++;
    }

    if (parse_stack.for_parsact.GC != NULL)
	(*parse_stack.for_parsact.GC) ();
}



static  SXVOID parsers_oflw (old_size, new_size)
    int		old_size, new_size;
{
    parse_stack.hole_nb = new_size - old_size;

    /* sizeof (struct parsers_attr) == 2**n */
    parse_stack.parser_to_attr =
	(struct parsers_attr*) sxrealloc (parse_stack.parser_to_attr,
					  new_size + 1,
					  sizeof (struct parsers_attr));
			
    parse_stack.parsers_set = sxbm_resize (parse_stack.parsers_set,
					   5,
					   5,
					   new_size + 1);
    parse_stack.free_parsers_set = parse_stack.parsers_set [4];

    if (parse_stack.for_parsact.parsers_oflw != NULL)
	(*parse_stack.for_parsact.parsers_oflw) (old_size, new_size);
}

static  SXVOID sons_oflw (old_size, new_size)
    int		old_size, new_size;
{
    parse_stack.erasable_sons = sxba_resize (parse_stack.erasable_sons, new_size + 1);
    parse_stack.current_links = sxba_resize (parse_stack.current_links, new_size + 1);
    parse_stack.used_links = sxba_resize (parse_stack.used_links, new_size + 1);
    parse_stack.transitions = (int*) sxrealloc (parse_stack.transitions, new_size + 1, sizeof (int));

    if (parse_stack.for_parsact.sons_oflw != NULL)
	(*parse_stack.for_parsact.sons_oflw) (old_size, new_size);
}

static  SXVOID symbols_oflw (old_size, new_size)
    int		old_size, new_size;
{
    if (parse_stack.for_parsact.symbols_oflw != NULL)
	(*parse_stack.for_parsact.symbols_oflw) (old_size, new_size);
}


#if 0
/* Le 20/12/04 devient un module de la librairie  sxtm_mngr.c */
#if EBUG
static VOID sxtime (what, str)
    int what;
    char *str;
{
  if (what == INIT) {
    sxtimestamp (INIT, NULL);
    fputs (str, sxtty);
  }
  else {
    sxtimestamp (ACTION, str);
  }
}
#endif
#endif /* 0 */


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

static BOOLEAN	set_next_item (vector, check, action, Max)
    struct SXP_item	*vector;
    int			*check, *action, Max;
{
    struct SXP_item	*aitem;
    int			val;

    if ((val = sxplocals.rcvr.mvector - vector) > *check)
	*check = val - 1;

    if ((val = sxplocals.rcvr.Mvector - vector) < Max)
	*check = val;

    for (aitem = vector + ++*check; *check <= Max; aitem++, ++*check)
	if (aitem->check == *check) {
	    *action = aitem->action;
	    return TRUE;
	}

    return FALSE;
}



SXVOID	set_first_trans (abase, check, action, Max, next_action_kind)
    struct SXP_bases	*abase;
    int			*check, *action, Max;
    int			*next_action_kind;
{
    int	ref = (int) abase->commun;

    if (ref <= sxplocals.SXP_tables.Mref /* codage direct */ ) {
	if ((*check = (int) abase->propre) != 0) {
	    *action = ref;
	    *next_action_kind = Defaut;
	}
	else /* defaut */  {
	    *action = (int) abase->defaut;
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



BOOLEAN	set_next_trans (abase, check, action, Max, next_action_kind)
    struct SXP_bases	*abase;
    int			*check, *action, Max;
    int			*next_action_kind;
{
    switch (*next_action_kind) {
    case Commun:
	if (set_next_item (sxplocals.SXP_tables.vector + abase->commun, check, action, Max))
	    return TRUE;

	if (abase->propre != 0 /* partie propre non vide */ ) {
	    *next_action_kind = Propre;
	    *check = 0;

    case Propre:
	    if (set_next_item (sxplocals.SXP_tables.vector + abase->propre, check, action, Max))
		return TRUE;
	}

    case Defaut:
	*check = 0;
	*action = (int) abase->defaut;
	*next_action_kind = Done;
	return TRUE;

    default:
	sxtrap ("sxndparser", "set_next_trans");
	  /*NOTREACHED*/

    case Done:
	return FALSE;
    }
}



int NDP_access (abase, j)
    struct SXP_bases	*abase;
    int			j;
{
    struct SXP_item	*ajvector, *ajrefvector;
    int			ref = (int) abase->commun;

    if (ref <= sxplocals.SXP_tables.Mref /* codage direct */ ) {
	if ((int) abase->propre == j /* checked */ )
	    return ref;
	else
	    return (int) abase->defaut;
    }

    if ((ajrefvector = (ajvector = sxplocals.SXP_tables.vector + j) + ref)->check == j)
	/* dans commun */
	return (int) ajrefvector->action;

    if ((ref = abase->propre) != 0 /* propre est non vide */ 
	&& (ajrefvector = ajvector + ref)->check == j)
	/* dans propre */
	return (int) ajrefvector->action;

    return (int) abase->defaut;
}



/* static */ BOOLEAN
new_symbol (reduce, k, i, j, symbol)
    int reduce, k, i, j, *symbol;
{
    int		kind, range;

    if (k > 0) {
	/* Suffixe sharing. */
	XxY_set (&parse_stack.suffixes, reduce, k, &kind);
	kind += sxplocals.SXP_tables.P_xnbpro;
    }
    else
	kind = reduce;

#if EBUG
	if (!sxplocals.sxtables->SXS_tables.S_is_non_deterministic && j < i)
	    sxtrap (ME, "new_symbol");
#endif

    if (i + 1 == j)
	range = -i;
    else
	XxY_set (&parse_stack.ranges, i, j, &range);

    return !XxY_set (&parse_stack.symbols, kind, range, symbol);
}



#if 0
static int  set_unit_rule (lhs, rhs_symb)
    int		lhs, rhs_symb;
{
    /* Possible ambiguity between different rules (with identical lhs). */
    int rule_no;

    XxY_set (&parse_stack.unit_rules, lhs, rhs_symb, &rule_no);
    parse_stack.is_unit_rules = TRUE;

    return rule_no;
}  
#endif

/* static */ void
set_rule (lhs, rhs)
    int		lhs, rhs;
{
    int		rule_no;

#if EBUG
    if (lhs < 0)
	sxtrap (ME, "set_rule");
#endif 

#if 0
    {
      int	reduce = XxY_X (parse_stack.symbols, lhs);

      if (reduce > sxplocals.SXP_tables.P_nbpro || (act = parse_stack.red2act [reduce]) >= 10000) {
	if (sxplocals.mode.with_parsact) {
	      /* On doit executer les parsact en cours d'analyse, croisons les doigts */

	      /* A FAIRE : donner les moyens aux actions non deterministes d'acceder
		 a leur environnement. */
	  (*sxplocals.SXP_tables.parsact) (ACTION, act);
	}
	else {
	  parse_stack.parsact_seen = TRUE; /* postponed */
	}
      }
    }
    
#endif

    XxY_set (&parse_stack.sf_rule, lhs, rhs, &rule_no);
}




static int build_a_rule (parser, build_a_rule_index, rhs)
    int parser, build_a_rule_index, rhs;
{
    int symb;
    SXBOOLEAN bool;
    bool = (new_symbol (parse_stack.ared->reduce,
			build_a_rule_index,
			parse_stack.parser_to_attr [parser].milestone,
			parse_stack.current_milestone,
			&symb));
#if EBUG
    if (bool && build_a_rule_index == 0)
      sxtrap (ME, "build_a_rule");
#endif

    set_rule (symb, rhs);
    return symb;
}


/* static */ SXVOID
set_start_symbol (symbol)
    int		symbol;
{
    int		symb, rhs, rule_no;
    /* Halt */
    /* Several "final" parse_stack elem could occur. */

    if (parse_stack.start_symbol == 0) {
	new_symbol (0, 0, 0, parse_stack.current_milestone + 1, &symb);
	parse_stack.start_symbol = symb;
    }

    XxY_set (&parse_stack.sf_rhs, 0, 0, &rhs);
    XxY_set (&parse_stack.sf_rhs, symbol, rhs, &rhs);
    XxY_set (&parse_stack.sf_rhs, 0, rhs, &rhs);

    XxY_set (&parse_stack.sf_rule, parse_stack.start_symbol, rhs, &rule_no);
}



static SXVOID clear_triples ()
{
    int *pref, ref;

    if ((ref = parse_stack.for_reducer.refs [0]) > 0) {
	parse_stack.for_reducer.refs [0] = -1;

	do {
	    ref = *(pref = parse_stack.for_reducer.refs + ref);
	    *pref = 0;
	} while (ref > 0);
    }

    parse_stack.for_reducer.triples_hd [0] = 0; /* cas ref == 0 */
}



int mreds_push (int parser, int ref, int tok_no, BOOLEAN is_new_triple, BOOLEAN is_shift_reduce, int symbol)
    /* Param "is_shift_reduce" ajoute' le 06/10/94 pour les pars[act|prdct] */
    /* Param "symbol" ajoute' le 21/4/95 */
{
    /* Si is_new_triple, l'appelant sait que le triplet est nouveau. */
    int			x;
    int			*ai;
    struct triple	*top, *cur;

    /* Il peut y avoir des couples (parser, ref) multiples identiques.
       Ca ne peut se produire que dans le cas ou mreds_push est appele' sur un shift-reduce
       (dans les autres cas, mreds_push n'est appele' que sur un nouveau parser).
       Ex C -> alpha, C -> beta, A -> gamma C. Si le depilage sur alpha et beta conduit
       ds le meme parser p et si l'action est : ref = Shift(C)-Reduce(A -> gamma C), le couple
       (p, ref) sera empile 2 fois. */
    
    ai = parse_stack.for_reducer.triples_hd + ref;
    
    /* Le cas ref == 0 correspond a une analyse "LR(0)" pendant le rattrapage global
       d'erreur, is_new_triple doit etre vrai. */

#if EBUG
    if (ref == 0 && !is_new_triple)
	sxtrap (ME, "mreds_push");
#endif

    if (ref > 0 && parse_stack.for_reducer.refs [ref] == 0) { 
	PUSH (parse_stack.for_reducer.refs, ref);
	*ai = 0;
    }
    else if (!is_new_triple) {
	x = *ai;
	
	do {
	    cur = parse_stack.for_reducer.triples + x;
	    
	    if (cur->parser == parser && cur->tok_no == tok_no && cur->symbol == symbol) {
#if EBUG
		if (cur->is_shift_reduce != is_shift_reduce)
		    sxtrap (ME, "mreds_push");
#endif		
		
		return x;
	    }
	} while ((x = cur->link) != 0);
    }
    
    if (++parse_stack.for_reducer.top > parse_stack.for_reducer.size)		
	parse_stack.for_reducer.triples =						
	    (struct triple*) sxrealloc (parse_stack.for_reducer.triples,		
					  (parse_stack.for_reducer.size *= 2) + 1,		
					  sizeof (struct triple));				
    
    top = parse_stack.for_reducer.triples + parse_stack.for_reducer.top;

    top->link = *ai;
    *ai = parse_stack.for_reducer.top;
    
    top->parser = parser;
    top->ref = ref;
    top->tok_no = tok_no;
    top->symbol = symbol;
    top->re_do = 0;
    top->is_shift_reduce = is_shift_reduce;
    /* positionne ssi au moins une reduction ne conduit pas a une impasse. */
    top->is_valid = FALSE;

    return parse_stack.for_reducer.top;
}


int new_parser (son, state, symbol)
    int		son, state, symbol;
{
    /* Cree le parser "father" (s'il n'existe pas deja), a partir de
       state, parse_stack.current_token et "reduce"
       et le lie vers son fils "son".
       Si la transition est non-terminale, reduce est le no de production ayant provoque
       cette transition, sinon reduce == 0 (le terminal est deja pris en compte ds state).
       Retourne father. */
    int				x, father;
    struct  parsers_attr	*pattr;
    BOOLEAN			is_an_old_father;

    /* On note les etats qui ont (au moins) un fils actif. */
    /* goto (son, symbol) = father et symbol =>+ epsilon. */
    /* Ca permet de ne pas oublier do_limited_pops. */
    
    is_an_old_father = XxYxZ_set (&parse_stack.parsers,
				  state,
				  parse_stack.current_token,
				  symbol <= 0 ? 0 : parse_stack.ared->reduce,
				  /* symbol, */
				  &father);

    pattr = &(parse_stack.parser_to_attr [father]);

    if (is_an_old_father) {
#if EBUG
	if (pattr->milestone != parse_stack.current_milestone)
	    sxtrap (ME, "new_parser");
#endif

	/* Add a link from father to son if it does not already exists. */
	if ((x = pattr->son) == son || -x == son) {
	    /* son est le fils principal de father */
	    /* Il semble que ce ne soit pas possible que la meme reduction caracterisee par symbol
	       soit "declanchee" depuis 2 parsers differents et "arrive" sur le meme parser son. */
	    /* Mais il est possible de "refaire" ce qui a ete fait (les parsact on pu forcer une
	       reevaluation. */	    
	    /* if (sxplocals.mode.mode == SXPARSER && pattr->symbol != symbol)
		set_unit_rule (pattr->symbol, symbol); */
#if EBUG
	if (sxplocals.mode.mode == SXPARSER && pattr->symbol != symbol)
	    sxtrap (ME, "new_parser");
#endif
	parse_stack.local_link = 0;
	}
	else {
	    if (x > 0)
		pattr->son = -x;
	    
	    if (XxY_set (&parse_stack.sons, father, son, &parse_stack.local_link)) {
		/* son est un fils secondaire de father */
		/* if (sxplocals.mode.mode == SXPARSER &&
		    parse_stack.transitions [parse_stack.local_link] != symbol)
		    set_unit_rule (parse_stack.transitions [parse_stack.local_link], symbol); */
#if EBUG
	      if (sxplocals.mode.mode == SXPARSER &&
		  parse_stack.transitions [parse_stack.local_link] != symbol)
		  sxtrap (ME, "new_parser");
#endif

	    }
	    else {
		parse_stack.transitions [parse_stack.local_link] = symbol;
		
		/* "father" is active and it already exists; furthermore it already has
		   a child which is not "son". */
		/* If "father" (or one of its ancestors) has a reduce action, this action
		   must take "link" as a pop path if relevant.
		   To be sure that such a case is always processed, it will be examined
		   when "father" has an active ancestor or when "father" has already been
		   processed. */
		if (pattr->is_already_processed) {
		    /* is_already_processed is an "only if" condition. */
		    /* old active parser and new link between father and son */
		    parse_stack.is_new_links = TRUE;
		    SXBA_1_bit (parse_stack.current_links, parse_stack.local_link);
		}
	    }
	}
    }
    else {
	/* add a link from father to son */
        parse_stack.local_link = 0;
	pattr->son = son;
	pattr->milestone = parse_stack.current_milestone;
	pattr->symbol = symbol;
	pattr->is_already_processed = FALSE;
	pattr->scan_ref = 0;	/* permet de savoir si ce parser a deja ete trouve avec une
				   ref correspondant a un scan. */

	if (--parse_stack.hole_nb == 0)
	    sxndparser_GC (father);
    }

    return father;
}




static BOOLEAN call_predicate (current_token, prdct)
    int current_token, prdct;
{
    BOOLEAN 	ret_value;
    int		ptok_no = sxplocals.ptok_no;

    sxplocals.ptok_no = current_token;
    ret_value = (*sxplocals.SXP_tables.parsact) (PREDICATE, prdct);
    sxplocals.ptok_no = ptok_no;
    
    return ret_value;
}


static int	ARC_traversal (ref)
    int	ref;
{
    /* Pour le moment (a faire plus tard)
       les ARC ne sont pas utilises (on utilise le non-determinisme) dans les cas suivants :
       - Le flot de token n'est pas lineaire (scanner non deterministe ou analyse d'une
       correction d'erreur multiple avec un scanner deterministe).
       - Pendant le traitement d'erreur (prudence)
       - Si SXGENERATOR
       */
    int next, current_token;

    next = ref + sxplocals.SXP_tables.Mref /* next < 0 */; /* Le 16/04/07 */

    if (parse_stack.ARC.level == 0) {
	/* 1er appel de ARC_traversal */
	parse_stack.ARC.current_token = parse_stack.current_token;
	parse_stack.ARC.init_t_state = -next;

	if (parse_stack.is_token_dag ||
	    parse_stack.rcvr.is_up ||
	    sxplocals.mode.mode == SXGENERATOR)
	    /* On utilise le non determinisme... */
	    return sxplocals.SXP_tables.germe_to_gr_act [parse_stack.ARC.init_t_state];
    }

    parse_stack.ARC.level++;
    current_token = parse_stack.ARC.current_token;

    do {
	ref = NDP_access (sxplocals.SXP_tables.t_bases - next,
			  sxget_token (++parse_stack.ARC.current_token)->lahead);
	    
	if (ref < -sxplocals.SXP_tables.Mred && ref >= -sxplocals.SXP_tables.Mprdct) {
	    /* Il n'y a pas de non determinisme ds les ARCs generes. */
	    /* Execution d'un predicat en look-ahead en mode parser */
	    struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts - ref;
		
	    for (;;) {
		if (aprdct->action != 0
		    && (!sxplocals.mode.with_parsprdct
			|| ( aprdct->prdct >= 0 /* User's predicate */
			&& call_predicate (parse_stack.ARC.current_token, aprdct->prdct)))) {
		    ref = ARC_traversal (aprdct->action);

		    if (ref != 0)
			return ref;
		}
		    
		if (aprdct->prdct < 0) {
		    return aprdct->action;
		}
		    
		aprdct++;
	    }
	}

	if ((next = ref + sxplocals.SXP_tables.Mref) >= 0) {
	    /* OK, fin de l'ARC */
	    return ref;
	}
    } while (next < 0);

    parse_stack.ARC.current_token = current_token;
    parse_stack.ARC.level--;

    
    if (parse_stack.ARC.level == 0 && ref == 0) {
	/* Error in LookAhead  */
	ref = sxplocals.SXP_tables.germe_to_gr_act [parse_stack.ARC.init_t_state];
    }
    
    return ref;
}




static void create_ref (father, ref)
    int		father, ref;
{
    /* ref est non nul. */
    int				xmscan;
    struct SXP_prdct		*aprdct;
    struct parsers_attr		*pattr;
    BOOLEAN			with_parsprdct;

    if (ref < -sxplocals.SXP_tables.Mref)
	ref = ARC_traversal (ref);

    if (ref > 0			/* scan */ ) {
	pattr = parse_stack.parser_to_attr + father;

	if (pattr->scan_ref == 0) {
	    /* 1er scan */
	    pattr->scan_ref = ref;
	    pattr->next_for_scanner = *parse_stack.for_scanner.next_hd;
	    *parse_stack.for_scanner.next_hd = father;
	}
	else if (pattr->scan_ref != ref && pattr->scan_ref != -ref) {
	    /* Plusieurs scan_ref differents sont possibles
	       ds le cas de predicats differents  vrai simultanement (cas de non-determinisme)
	       associes au meme terminal.
	       Le non-determinisme (normal) produisant au plus un shift (cas du conflit
	       Shift/Reduce) et un ou des reduces (traites par mreds_push). */
	    XxY_set (&parse_stack.mscans, father, ref, &xmscan);
	    
	    if (pattr->scan_ref > 0)
		pattr->scan_ref = -pattr->scan_ref;
	}
    }
    else {
	ref = -ref;
	
#if EBUG
	if (ref > sxplocals.SXP_tables.Mprdct /* Shift */)
	    sxtrap (ME, "unexpected shift action.");
#endif
	
	if (ref <= sxplocals.SXP_tables.Mred /* reduce|halt */) {
#if 0
	    if (sxplocals.SXP_tables.reductions [ref].reduce == 0) { /* Halt */
		parse_stack.halt_hit = TRUE;
	    }
	    else
#endif
	    {		/* Reduce */
		if (sxplocals.SXP_tables.reductions [ref].reduce == 0)
		    /* Halt */
		    parse_stack.halt_hit = TRUE;

		mreds_push (father, ref, parse_stack.current_token,
			    FALSE, FALSE /* not is_shift_reduce */, 0);
	    }
	}
	else {			/* Predicates or non-determinism */
	    if ((aprdct = sxplocals.SXP_tables.prdcts + ref)->prdct >= 20000) {
		/* non-determinism */
		for (;;) {
		    if (aprdct->action)
			create_ref (father, aprdct->action);
		    
		    if (aprdct->prdct == 20000)
			break;
		    
		    aprdct++;
		}
	    }
	    else {		/* Predicats associes a un symbole TERMINAL */
              with_parsprdct = (parse_stack.rcvr.is_up && parse_stack.rcvr.with_parsprdct)
		    || (!parse_stack.rcvr.is_up && sxplocals.mode.with_parsprdct);
		/* Si !with_parsprdct, on suppose qu'ils retournent vrai (ils seront
		   uniquement executes en phase de desambiguation. */
		/* Le parser origine de la XT transition n'est pas encore calcule'
		   ce sera bot tq :
		   XxYxZ_set (&parse_stack.parsers,
		   state,
		   parse_stack.current_token,
		   symbol
		   &bot);
		   Quoi noter (ds parse_stack.for_parsact) pour permettre leur
		   exe'cution???
		   */
		for (;;) {
		    if (aprdct->action != 0
			&& (!with_parsprdct
			    || (aprdct->prdct >= 0 /* User's predicate */
			        && (*sxplocals.SXP_tables.parsact) (PREDICATE, -aprdct->prdct-1)))) {
		      /* Le 11/02/2003 les predicats des XT sont passes en < 0 (-prdct-1)
			 car  parse_stack.current_token est le XT alors que dans le cas XNT, c'est 
			 le look_ahead */
			create_ref (father, aprdct->action);
		    }
		    
		    if (aprdct->prdct < 0) {
		      /* Ajoute le 10/02/2003 a 18:15 */
		      if (aprdct->action)
		        create_ref (father, aprdct->action);

		      break;
		    }
		    
		    aprdct++;
		}
	    }
	}
    }
}


BOOLEAN
sxndsubparse_a_token (parser, ref, symbol, f)
    int		parser, ref, symbol;
    int		(*f) ();
{
    int			father, state, t_trans, next_action_kind, xtriple;
    struct SXP_bases	*abase;
    BOOLEAN		ret_val = FALSE;

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
	xtriple = mreds_push (parser, ref, parse_stack.current_token,
			FALSE, TRUE /* is_shift_reduce */, symbol);

#if 0
/* 21/4/95 */
	if (sxplocals.mode.mode == SXPARSER) {
	    /* Pour parse_forest */
	    int	*pct;

	    if (*(pct = &(parse_stack.for_reducer.triples [xtriple].symbol)) == 0)
		*pct = symbol;
	    else if (*pct != symbol)
		set_unit_rule (*pct, symbol);
	}
#endif
	if (f != NULL)
	    /* Modif du 02/02/95, on passe -xtriple au lieu de -ref */
	    (*f) (parser, -xtriple, symbol);

	ret_val = TRUE;
    }
    else {
	state = ref - sxplocals.SXP_tables.Mprdct;
	abase = sxplocals.SXP_tables.t_bases + state;	
	
	if (sxplocals.mode.mode != SXGENERATOR
	    || parse_stack.current_token != parse_stack.delta_generator) {
	    /* SXPARSER or SXRECOGNIZER */
	    if ((ref = NDP_access (abase, parse_stack.current_lahead)) != 0) {
		father = new_parser (parser, state, symbol);
		create_ref (father, ref);

#if 0
		if (parse_stack.halt_hit && sxplocals.mode.mode == SXPARSER)
		    set_start_symbol (symbol);
#endif

		if (f != NULL)
		    (*f) (parser, father, symbol);

		ret_val = TRUE;
	    }
	}
	else {
	    /* sxplocals.mode.mode == SXGENERATOR && parse_stack.current_lahead == 0 */
	    /* Le token sur lequel on fait la generation n'a pas encore ete determine'
	       par le contexte. */
	    set_first_trans (abase, &t_trans, &ref,
			     (int) sxplocals.SXP_tables.P_tmax, &next_action_kind);
	    
	    while (ref != 0) {
		/* Dans le cas SXGENERATOR, current_token n'est pas un numero de token
		   (on traite un X, il n'y a pas de token correspondant) mais directement
		   le look_ahead (en negatif). On suppose que l'appelant peut se debrouiller
		   avec ca... */
		/* Si le defaut est un reduce, t_trans == 0, les bons terminaux seront
		   determines plus tard. */
		parse_stack.current_token = parse_stack.delta_generator - t_trans;
		parse_stack.current_lahead = t_trans;

		father = new_parser (parser, state, symbol);
		create_ref (father, ref);
		
		ret_val = TRUE;

		if (f != NULL)
		    (*f) (parser, father, symbol);

		if (!set_next_trans (abase, &t_trans, &ref,
				     sxplocals.SXP_tables.P_tmax, &next_action_kind))
		    break;
	    }
	}
    }

    return ret_val;
}
    
 

static BOOLEAN do_reductions (son, is_in_set)
    int		son;
    BOOLEAN	is_in_set;
{
    /* We want to perform goto (son, A) where A is the lhs of the
       reduced production. */
    /* Retourne vrai si au moins une des reductions est validee par le contexte. */
    int			ref, state, next_son = 0, symb, lhs; 
    BOOLEAN		ret_val = FALSE;
    struct SXP_prdct	*aprdct, *aprdct2;
    BOOLEAN 		with_parsprdct;

    do {
	if (sxplocals.mode.mode == SXPARSER)
	    new_symbol (parse_stack.ared->reduce,
			0,
			parse_stack.parser_to_attr [son].milestone,
			parse_stack.current_milestone,
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

	ref = -ref;

	if (parse_stack.for_parsact.action_bot != NULL)
	    (*parse_stack.for_parsact.action_bot) (son);

	if (ref > sxplocals.SXP_tables.Mred && ref <= sxplocals.SXP_tables.Mprdct)
	{
	    /* Extended non-terminal (Predicates) */
	    aprdct = sxplocals.SXP_tables.prdcts + ref;

	    if (aprdct->prdct >= 20000) {
		/* Non-determinisme apres une transition non-terminale, on est obligatoirement avec
		   un automate sous-jacent left-corner */
		for (;;) {
		    if (aprdct->action != 0 &&
			sxndsubparse_a_token (son, -aprdct->action, symb, parse_stack.for_parsact.action_new_top))
			    ret_val = TRUE;

		    if (aprdct->prdct == 20000)
			break;
		    
		    aprdct++;
		}
	    }
	    else {		/* Predicats associes a un symbole NON-TERMINAL */
	        with_parsprdct = (parse_stack.rcvr.is_up && parse_stack.rcvr.with_parsprdct)
		    || (!parse_stack.rcvr.is_up && sxplocals.mode.with_parsprdct);
		/* Si !with_parsprdct, on suppose qu'ils retournent vrai (ils seront
		   uniquement executes en phase de desambiguation. */
		for (;;) {
		    if (aprdct->action != 0
			&& (!with_parsprdct
			    || (aprdct->prdct >= 0 /* User's predicate */
			        && (*sxplocals.SXP_tables.parsact) (PREDICATE, aprdct->prdct)))) {
#if EBUG
			if (aprdct->action > 0) /* scan illegal ici! */
			    sxtrap (ME, "do_reductions");
#endif

			/* Le 11/02/2003 : bug.  Une transition sur du xnt peut conduire a du
			   non-determinisme ds le cas left-corner */

			ref = -aprdct->action;

			if (ref > sxplocals.SXP_tables.Mred && ref <= sxplocals.SXP_tables.Mprdct) {
			  /* Non-determinisme ou predicats */
			  aprdct2 = sxplocals.SXP_tables.prdcts + ref;

			  if (aprdct2->prdct >= 20000) {
			    /* Non-determinisme apres une transition non-terminale etendue, on est obligatoirement avec
			       un automate sous-jacent left-corner */
			    for (;;) {
			      if (aprdct2->action != 0 &&
				  sxndsubparse_a_token (son, -aprdct2->action, symb, parse_stack.for_parsact.action_new_top))
				ret_val = TRUE;

			      if (aprdct2->prdct == 20000)
				break;
		    
			      aprdct2++;
			    }
			  }
			  else {
			    /* On est ds le cas nt&i &j non traite' */
			    sxtrap (ME, "do_reductions");
			  }
			}
			else {
			  if (sxndsubparse_a_token (son, ref, symb, parse_stack.for_parsact.action_new_top))
			    ret_val = TRUE;
			}
		    }
		    
		    if (aprdct->prdct < 0) {
		        /* defaut */
		        ref = -aprdct->action;

			if (ref > 0) {
			  /* Il y a une action sur le defaut */
			  if (ref > sxplocals.SXP_tables.Mred && ref <= sxplocals.SXP_tables.Mprdct) {
			    /* Non-determinisme ou predicats */
			    aprdct2 = sxplocals.SXP_tables.prdcts + ref;

			    if (aprdct2->prdct >= 20000) {
			      /* Non-determinisme apres une transition non-terminale etendue, on est obligatoirement avec
				 un automate sous-jacent left-corner */
			      for (;;) {
				if (aprdct2->action != 0 &&
				    sxndsubparse_a_token (son, -aprdct2->action, symb, parse_stack.for_parsact.action_new_top))
				  ret_val = TRUE;

				if (aprdct2->prdct == 20000)
				  break;
		    
				aprdct2++;
			      }
			    }
			    else {
			      /* pas de predicats apres le defaut */
			      sxtrap (ME, "do_reductions");
			    }
			  }
			  else {
			    if (sxndsubparse_a_token (son, ref, symb, parse_stack.for_parsact.action_new_top))
			      ret_val = TRUE;
			  }
			}

			break;
		    }
		    
		    aprdct++;
		}
	    }
	}
	else {
	    if (sxndsubparse_a_token (son, ref, symb, parse_stack.for_parsact.action_new_top))
		ret_val = TRUE;

	}
    } while (is_in_set
	     && (son = next_son = sxba_scan_reset (parse_stack.parsers_set [0], next_son)) > 0);

    return ret_val;
}


static BOOLEAN do_pops (p, l)
    int		p, l;
{
    /* We want to perform a reduce_action A -> alpha on p. */
    /* l = length (alpha). */
    /* p is a parse_stack_elem. */
    int			son, father, next_son, first_son;
    SXBA		fathers_set, sons_set;
    BOOLEAN		is_in_set;
    
    is_in_set = FALSE;
    son = p;
    
    while (l-- > 0) {
	father = son;

	if (!is_in_set) {
	    if ((son = parse_stack.parser_to_attr [father].son) < 0) {
		sons_set = (l & 01) ?
		    parse_stack.parsers_set [1] : parse_stack.parsers_set [0];

		son = -son;
		is_in_set = TRUE;

		XxY_Xforeach (parse_stack.sons, father, parse_stack.local_link) {
		    next_son = XxY_Y (parse_stack.sons, parse_stack.local_link);
		    SXBA_1_bit (sons_set, next_son);

		    if (parse_stack.for_parsact.action_pop != NULL)
			(*parse_stack.for_parsact.action_pop) (l, next_son, father);
		}
	    }

	    if (parse_stack.for_parsact.action_pop != NULL)
	    {
		parse_stack.local_link = 0;
		(*parse_stack.for_parsact.action_pop) (l, son, father);
	    }
	}
	else {
	    is_in_set = FALSE;

	    if (l & 01) {
		fathers_set = parse_stack.parsers_set [0];
		sons_set = parse_stack.parsers_set [1];
	    }
	    else {
		fathers_set = parse_stack.parsers_set [1];
		sons_set = parse_stack.parsers_set [0];
	    }

	    if ((son = parse_stack.parser_to_attr [father].son) < 0) {
		is_in_set = TRUE;
		son = -son;

		XxY_Xforeach (parse_stack.sons, father, parse_stack.local_link) {
		    next_son = XxY_Y (parse_stack.sons, parse_stack.local_link);
		    SXBA_1_bit (sons_set, next_son);

		    if (parse_stack.for_parsact.action_pop != NULL)
			(*parse_stack.for_parsact.action_pop) (l, next_son, father);
		}
	    }

	    if (parse_stack.for_parsact.action_pop != NULL)
	    {
		parse_stack.local_link = 0;
		(*parse_stack.for_parsact.action_pop) (l, son, father);
	    }

	    father = 0;
	    
	    while ((father = sxba_scan_reset (fathers_set, father)) > 0) {
		if ((first_son = parse_stack.parser_to_attr [father].son) < 0) {
		    is_in_set = TRUE;
		    first_son = -first_son;
		    
		    XxY_Xforeach (parse_stack.sons, father, parse_stack.local_link) {
			next_son = XxY_Y (parse_stack.sons, parse_stack.local_link);

			if (next_son != son)
			    SXBA_1_bit (sons_set, next_son);

			if (parse_stack.for_parsact.action_pop != NULL)
			    (*parse_stack.for_parsact.action_pop) (l, next_son, father);
		    }
		}

		if (first_son != son) {
		    SXBA_1_bit (sons_set, first_son);
		    is_in_set = TRUE;
		}

		if (parse_stack.for_parsact.action_pop != NULL)
		{
		    parse_stack.local_link = 0;
		    (*parse_stack.for_parsact.action_pop) (l, first_son, father);
		}
	    }
	}
    }

    return do_reductions (son, is_in_set);
}


static SXVOID seek_paths ();

   


/* Amelioration eventuelle :
   - Noter pour chaque appel de do_pops l'ensemble des parsers actifs atteint lors des
     depilages
   - Noter ds seek_parser l'ensemble des father extremite de nouveaux liens
   - Ne lancer un do_limited_pops que si un parser actif existe ds la descendance
     de l'origine.
*/
static BOOLEAN do_limited_pops (p, l)
    int p, l;
{
    /* Traque les reduces qui passent par certaines transitions stockees dans used_links. */
    /* called with l > 0 */
    register int	x, father, first_son, son;
    BOOLEAN		is_link, is_sons, no_son, ret_val;
    register SXBA	fathers_set, sons_set, fathers_link_set, sons_link_set;
    SXBA		s;

    /* All parsers_set are supposed to be empty. */
    /* l >= 1 */
    x = l & 01;
    fathers_set = parse_stack.parsers_set [x+2];
    fathers_link_set = parse_stack.parsers_set [x];
    x = (x + 1) & 01;
    sons_set = parse_stack.parsers_set [x+2];
    sons_link_set = parse_stack.parsers_set [x];
    SXBA_1_bit (fathers_set, p);

    while (--l >= 0) {
	father = 0;
	no_son = TRUE;

	while ((father = sxba_scan_reset (fathers_set, father)) > 0) {
	    is_link = SXBA_bit_is_set_reset (fathers_link_set, father);
	    
	    if ((is_sons = (first_son = parse_stack.parser_to_attr [father].son) < 0))
		first_son = -first_son;

	    if (l > 0 &&
		(is_link ||
		 parse_stack.parser_to_attr [first_son].milestone ==
		 parse_stack.current_milestone)) {
		/* Les liens pour do_limited_pops ne peuvent partir que des
		   fathers actifs. */
		no_son = FALSE;
		SXBA_1_bit (sons_set, first_son);
	    }

	    if (is_link)
		SXBA_1_bit (sons_link_set, first_son);
	    
	    if (parse_stack.for_parsact.action_pop != NULL)
	    {
		parse_stack.local_link = 0;
		(*parse_stack.for_parsact.action_pop) (l, first_son, father);
	    }

	    if (is_sons) {
		XxY_Xforeach (parse_stack.sons, father, parse_stack.local_link) {
		    son = XxY_Y (parse_stack.sons, parse_stack.local_link);

		    if (is_link || SXBA_bit_is_set (parse_stack.used_links, parse_stack.local_link)) {
			SXBA_1_bit (sons_link_set, son);

			if (l > 0) {
			  no_son = FALSE;
			  SXBA_1_bit (sons_set, son);
		        }
		    }
		    else if (l > 0 &&
			(parse_stack.parser_to_attr [son].milestone ==
			 parse_stack.current_milestone)) {
			/* Les liens pour do_limited_pops ne peuvent partir que des
			   fathers actifs. */
			no_son = FALSE;
			SXBA_1_bit (sons_set, son);
		    }

		    if (parse_stack.for_parsact.action_pop != NULL)
			(*parse_stack.for_parsact.action_pop) (l, son, father);
		}
	    }
	}

	/* fathers_set and fathers_link_set are empty */

	if (l > 0) {
	    if (no_son)
		return FALSE;

	    s = fathers_set, fathers_set = sons_set, sons_set = s;
	    s = fathers_link_set, fathers_link_set = sons_link_set, sons_link_set = s;
	}
    }
    /* sons_set is empty */
	
    ret_val = FALSE;

    if ((son = sxba_scan_reset (sons_link_set, 0)) > 0) {
	ret_val |= do_reductions (son, sxba_scan (sons_link_set, son) > 0);
    }

    return ret_val;
}


static SXVOID seek_paths (p, p_trans, is_in_set, l)
    int		p, p_trans, l;
    BOOLEAN	is_in_set;
{
    int		x, son, father, first_son, major_son;
    SXBA	fathers_set, sons_set, s;
    int		*son_rhs_ptr;
    int		son_i, father_i, father_rhs_hd, rhs, first_son_rhs_hd, son_rhs_hd;
    BOOLEAN	is_sons, is_first_time, is_in_sons_set;

    fathers_set = parse_stack.parsers_set [0];
    father_i = 0;

    sons_set = parse_stack.parsers_set [1];
    son_i = 1;

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
	is_first_time = TRUE;
	is_in_sons_set = FALSE;

	for (;;) {
#if EBUG
	    parse_stack.red_trans_nb++;    
#endif    

	    if ((is_sons = (major_son = parse_stack.parser_to_attr [father].son) < 0))
		major_son = -major_son;
	    
	    if (is_first_time)
		first_son = major_son;
	
	    XxY_set (&parse_stack.sf_rhs,
		     parse_stack.parser_to_attr [father].symbol,
		     father_rhs_hd,
		     &rhs);

	    if (first_son == major_son)
		son_rhs_ptr = &first_son_rhs_hd;
	    else {
		is_in_sons_set = TRUE;
		son_rhs_ptr = &(parse_stack.parser_to_attr [major_son].rhs_i [son_i]);

		if (SXBA_bit_is_reset_set (sons_set, major_son))
		    *son_rhs_ptr = 0;
	    }
	    
	    if (*son_rhs_ptr == 0)
		*son_rhs_ptr = rhs;	
	    else {
		if (*son_rhs_ptr > 0) {
		    *son_rhs_ptr = -build_a_rule (major_son, l, *son_rhs_ptr);
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
			is_in_sons_set = TRUE;
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
		is_first_time = FALSE;
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
       to do_limited_pops and adds a path such that a node with a
       single father becomes multi_fathered and hence produces an inter
       mediate non-terminal which describes a path which has already be
       en processed previously. One solution is to have a two step proc
       ess: first, the recognizer which set all the pathes and the non-
       terminals which are the names of the transitions and second, the
       creation of the parse forest (ie the grammar rules) which uses
       the previous transitions and pathes. */
    int 		ref;
    int			p, parser, rhs;
    SXBA		parser_set;
    struct triple	*triple;

    while ((ref = parse_stack.for_reducer.refs [0]) > 0) {
	parse_stack.for_reducer.refs [0] = parse_stack.for_reducer.refs [ref];
	parse_stack.for_reducer.refs [ref] = 0;

	triple = parse_stack.for_reducer.triples + parse_stack.for_reducer.triples_hd [ref];
	parse_stack.ared = sxplocals.SXP_tables.reductions + ref;

	if (parse_stack.ared->reduce == 0) {
	    /* Halt */
	    /* Normalement chaque parser a un fils unique (son == 2) */
	    /* Le 09/05/2003 Ce n'est pas vrai ds le cas d'un scanner nd (les look-ahead du eof
	       du debut peuvent etre differents), j'enleve donc le test 
	       || parse_stack.parser_to_attr [triple->parser].son <= 0 */
	  do
	  {
#if EBUG
	      if (/* triple->link > 0 || */ !triple->is_valid || !parse_stack.halt_hit ||
		  triple->parser == 0 /* || parse_stack.parser_to_attr [triple->parser].son <= 0 */)
		  sxtrap (ME, "parse_forest");
#endif

	      set_start_symbol (parse_stack.parser_to_attr [triple->parser].symbol);

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
#if EBUG
			    parse_stack.red_trans_nb += 1;

			    if (triple->symbol == 0)
				sxtrap (ME, "parse_forest");
#endif	    
	    
			    XxY_set (&parse_stack.sf_rhs, triple->symbol, 0, &rhs);
			}
			else
			    rhs = 0;
			
			build_a_rule (triple->parser, 0, rhs);
		    }

		    triple = parse_stack.for_reducer.triples + triple->link;
		} while (triple > parse_stack.for_reducer.triples);  
	    }
	    else
	    {
		int	*prhs, main_rhs;

		parser = 0;
		parser_set = NULL;

		do
		{
		    if (triple->is_valid)
		    {
			if (triple->is_shift_reduce)
			{
#if EBUG
			    parse_stack.red_trans_nb += 1;

			    if (triple->symbol == 0)
				sxtrap (ME, "parse_forest");
#endif	    
	    
			    XxY_set (&parse_stack.sf_rhs, triple->symbol, 0, &rhs);
			}
			else
			    rhs = 0;
			
			if (parser == 0)
			{
			    parser = triple->parser;
			    main_rhs = rhs;
			}
			else
			{
			    p = triple->parser;

			    if (p == parser)
			    {
				prhs = &main_rhs;
			    }
			    else
			    {
				if (parser_set == NULL)
				    parser_set = parse_stack.parsers_set [1];

				prhs = &(parse_stack.parser_to_attr [p].rhs_i [1]);
			    }

			    if (p != parser && SXBA_bit_is_reset_set (parser_set, p))
				*prhs = rhs;
			    else
			    {
				/* meme parser initial! */
				if (*prhs > 0)
				{
				    /* vrai rhs => on en fait un NT */
				    *prhs = -build_a_rule (p, parse_stack.ared->lgth, *prhs);
				}

				set_rule (-*prhs, rhs);
			    }
			}
		    }

		    triple = parse_stack.for_reducer.triples + triple->link;
		} while (triple > parse_stack.for_reducer.triples);  

		if (parser > 0)
		    seek_paths (parser, main_rhs, parser_set != NULL, parse_stack.ared->lgth);
	    }
	}
    }

    parse_stack.for_reducer.triples_hd [0] = 0; /* cas ref == 0 */
}

#if 0
static SXVOID parse_forest ()
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
    int 		ref;
    int			p, x, parser, symbol, rhs;
    SXBA		parser_set;
    struct triple	*triple;

    while ((ref = parse_stack.for_reducer.refs [0]) > 0) {
	parse_stack.for_reducer.refs [0] = parse_stack.for_reducer.refs [ref];
	parse_stack.for_reducer.refs [ref] = 0;

	triple = parse_stack.for_reducer.triples + parse_stack.for_reducer.triples_hd [ref];

	if (sxplocals.SXP_tables.reductions [ref].reduce == 0) {
	    /* Halt */
	    /* Normalement chaque parser a un fils unique (son == 2) */
	  do
	  {
#if EBUG
	      if (/* triple->link > 0 || */ !triple->is_valid || !parse_stack.halt_hit ||
		  triple->parser == 0 || parse_stack.parser_to_attr [triple->parser].son <= 0)
		  sxtrap (ME, "parse_forest");
#endif

	      set_start_symbol (parse_stack.parser_to_attr [triple->parser].symbol);

	      triple = parse_stack.for_reducer.triples + triple->link;
	  } while (triple > parse_stack.for_reducer.triples);
	}
	else {
	    parser = 0;
	    parser_set = NULL;

	    if (triple->is_valid) {
		parser = triple->parser;

		if (triple->is_shift_reduce) {
#if EBUG
		    parse_stack.red_trans_nb += 1;

		    if (triple->symbol == 0)
			sxtrap (ME, "parse_forest");
#endif	    
	    
		    XxY_set (&parse_stack.sf_rhs, triple->symbol, 0, &rhs);
		}
		else
		    rhs = 0;
	    }

	    if ((x = triple->link) > 0) {
		do {
		    triple = parse_stack.for_reducer.triples + x;

		    if (triple->is_valid) {
			if (parser == 0) {
			    parser = triple->parser;

			    if (triple->is_shift_reduce) {
#if EBUG
				parse_stack.red_trans_nb += 1;

				if (triple->symbol == 0)
				    sxtrap (ME, "parse_forest");
#endif	    
	    
				XxY_set (&parse_stack.sf_rhs, triple->symbol, 0, &rhs);
			    }
			    else
				rhs = 0;
			}
			else {
			    if (parser_set == NULL)
				parser_set = parse_stack.parsers_set [1];

			    p = triple->parser;
			    SXBA_1_bit (parser_set, p);

			    if (triple->is_shift_reduce) {
#if EBUG
				parse_stack.red_trans_nb += 1;

				if (triple->symbol == 0)
				    sxtrap (ME, "parse_forest");
#endif	    
	    
				XxY_set (&parse_stack.sf_rhs, triple->symbol, 0,
					 &(parse_stack.parser_to_attr [p].rhs_i [1]));
			    }
			    else
				parse_stack.parser_to_attr [p].rhs_i [1] = 0;
			}
		    }
		} while ((x = triple->link) > 0);
	    }

	    if (parser > 0)
		seek_paths (parser, rhs, parser_set != NULL,
			    (parse_stack.ared = sxplocals.SXP_tables.reductions + ref)->lgth);
	}
    }

    parse_stack.for_reducer.triples_hd [0] = 0; /* cas ref == 0 */
}
#endif


static void
single_reduce (xtriple, is_limited)
    int		xtriple;
    BOOLEAN	is_limited;
{
    struct triple	*ptriple;
    int		parser, ref, lgth;
    BOOLEAN     pops_val;

    ptriple = parse_stack.for_reducer.triples + xtriple;
    ref = ptriple->ref;
    parse_stack.ared = sxplocals.SXP_tables.reductions + ref;

    if (parse_stack.ared->reduce == 0)
    {
	 /* halt */
	ptriple->is_valid = TRUE; /* A priori */
    }
    else
    {
	parser = ptriple->parser;
	lgth = parse_stack.ared->lgth;

	if (!is_limited ||
	    (lgth > 0 &&
	    (!ptriple->is_shift_reduce ||
	     parse_stack.parser_to_attr [parser].milestone ==
	     parse_stack.current_milestone)) /* top actif */) {
	    /* reducer () peut etre appele sur des tokens multiples (meme si
	       sxplocals.mode.mode != SXGENERATOR), on repositionne donc
	       current_token et current_lahead dans tous les cas. */
	    parse_stack.current_token = ptriple->tok_no;
	    parse_stack.current_lahead = (parse_stack.current_token <= 0)
		? parse_stack.delta_generator - parse_stack.current_token
		    : SXGET_TOKEN (parse_stack.current_token).lahead;

	    if (!is_limited)
		parse_stack.parser_to_attr [parser].is_already_processed = TRUE;

	    if (parse_stack.for_parsact.action_top != NULL)
		(*parse_stack.for_parsact.action_top) (xtriple);

	    /* ptriple peut etre invalide' par une reallocation => on positionne is_valid en 2 coups */
	    pops_val = ((is_limited)
			? do_limited_pops (parser, lgth)
			: do_pops (parser, lgth));

	    parse_stack.for_reducer.triples [xtriple].is_valid |= pops_val;
	}
    }
}


SXVOID reducer ()
{
    /* new_top est non nul et designe la tete de la liste qui parcourt les
       active parsers. */
    int			x, cur_top, bot;
    SXBA		swap;

    bot = 1;
    
    while ((cur_top = parse_stack.for_reducer.top) >= bot) {
	for (x = bot; x <= cur_top; x++) {
	    single_reduce (x, FALSE);
	}
	
	if (parse_stack.is_new_links) {
	    do {
		/* Les seuls candidats a un do_limited_pops sont :
		   - les tetes de liens de "used_links"
		   - les parsers ayant un descendant actif. */
		parse_stack.is_new_links = FALSE;

		swap = parse_stack.used_links;
		parse_stack.used_links = parse_stack.current_links;
		sxba_empty (parse_stack.current_links = swap);

		for (x = 1; x <= cur_top; x++) {
		    single_reduce (x, TRUE);
		}
	    } while (parse_stack.is_new_links);
	}

	if ((x = parse_stack.for_reducer.re_do) != -1) {
	    /* Les actions semantiques forcent la reevaluation de la liste "re_do". */
	    do {
		parse_stack.for_reducer.re_do = parse_stack.for_reducer.triples [x].re_do;
		parse_stack.for_reducer.triples [x].re_do = 0;
		single_reduce (x, FALSE);
	    } while ((x = parse_stack.for_reducer.re_do) != -1);
	}

	bot = cur_top + 1;
    }

    if (parse_stack.for_parsact.action_final != NULL) {
      /* Modif du 16/04/07 */
      for (x = 1; x <= cur_top; x++) 
	(*parse_stack.for_parsact.action_final) (x);
#if 0
	(*parse_stack.for_parsact.action_final) (x);
#endif /* 0 */
    }

    if (sxplocals.mode.mode == SXPARSER)
	parse_forest ();
    else
	clear_triples ();
    
    parse_stack.for_reducer.top = 0;
}



static SXVOID grammar_new_rule (lhs)
    int lhs;
{
    struct rule	*prule;

    if (parse_stack.rule_top > parse_stack.rule_size) {
	parse_stack.rule = (struct rule*) sxrealloc (parse_stack.rule,
					      (parse_stack.rule_size *= 2) + 1,
					      sizeof (struct rule));
	symb_to_rule_list = (int*) sxrealloc (symb_to_rule_list,
					 parse_stack.rule_size + 1,
					 sizeof (int));
    }

    prule = parse_stack.rule + parse_stack.rule_top;
    prule->hd = parse_stack.grammar_top + 1;
    prule->lgth = prule->item_hd = 0;

    if (lhs != 0) {
	symb_to_rule_list [parse_stack.rule_top] = symb_to_rule_hd [lhs];
	symb_to_rule_hd [lhs] = parse_stack.rule_top;
    }
}

static SXVOID grammar_put (symbol)
	int symbol;
{
    struct grammar	*pgram;

    if (++parse_stack.grammar_top > parse_stack.grammar_size) {
	parse_stack.grammar = (struct grammar*) sxrealloc (parse_stack.grammar,
					      (parse_stack.grammar_size *= 2) + 1,
					      sizeof (struct grammar));
    }

    parse_stack.rule [parse_stack.rule_top].lgth++;

    pgram = parse_stack.grammar + parse_stack.grammar_top;
    pgram->rhs_symb = symbol;
    pgram->lhs_rule = parse_stack.rule_top;
}



static SXVOID
push_new_nt (symbol)
    int symbol;
{
    /* symbol != 0 */

    if (symbol <= 0) {
	symbol = -symbol;
	SXBA_1_bit (parse_stack.useful_tset, symbol);
    }
    else {
#if 0
	int eq;

	if (parse_stack.is_unit_rules && (eq = equiv [symbol]) != 0)
	{
	    if (SXBA_bit_is_reset_set (hook_symbol_set, eq))
	    {
		for (symbol = eq; symbol > 0; symbol = equiv_list [symbol])
		{
		    if (SXBA_bit_is_reset_set (symbol_set, symbol))
			PUSH (symbol_stack, symbol);
		}
	    }
	}
	else
#endif
	{
	    if (SXBA_bit_is_reset_set (parse_stack.useful_ntset, symbol)) {
		PUSH (symbol_stack, symbol);
	    }
	}
    }
}



static SXVOID expand_rule (tnt)
    int tnt;
{
    /* "rhs_stack" is an SS_stack and contains a list of symbols which designates
       a prefixe of a rule. */
    /* tnt is the most rhs symbol */
    /* Appends the "expansion" of tnt. */
    int old_top, x, rhs, rule_no;

    old_top = SS_top (rhs_stack);

    if (tnt > 0 &&
	XxY_X (parse_stack.symbols, tnt) > sxplocals.SXP_tables.P_xnbpro) {
	/* NT decrivant le partage d'un suffixe */
	XxY_Xforeach (parse_stack.sf_rule, tnt, rule_no) {
	    rhs = XxY_Y (parse_stack.sf_rule, rule_no);
	    tnt = 0;

	    while (rhs > 0) {
		tnt = XxY_X (parse_stack.sf_rhs, rhs);
		rhs = XxY_Y (parse_stack.sf_rhs, rhs);

		if (rhs > 0
		    || tnt == 0 /* eof de fin ou error (fin de rhs) */) {
		    SS_push (rhs_stack, tnt);

		    if (tnt != 0)
			push_new_nt (tnt);
		}
	    }

	    expand_rule (tnt);
	    SS_top (rhs_stack) = old_top;
	}
    }
    else {
	parse_stack.rule_top++;

	if (parse_stack.grammar != NULL) {
	    x = SS_bot (rhs_stack);
	    grammar_new_rule (SS_get (rhs_stack, x));

	    while (x < old_top) {
		grammar_put (SS_get (rhs_stack, x));
		x++;
	    }

	    if (tnt != 0)
		grammar_put (tnt);
	}
	else
	    SS_push (rhs_stack, tnt);

	if (tnt != 0)
	    push_new_nt (tnt);

	if (parse_stack.for_parsact.post_td_action != NULL)
	    (*parse_stack.for_parsact.post_td_action) (rhs_stack);

	SS_top (rhs_stack) = old_top;
    }
}


static SXVOID hook_construction (symbol)
    int symbol;
{
    int 		rule, old_rule_top, old_grammar_top;

    old_rule_top = parse_stack.rule_top;
    old_grammar_top = parse_stack.grammar_top;
    parse_stack.rule_top++;
    grammar_new_rule (0); /* On ne touche pas a symb_to_rule */
    grammar_put (symbol);

#if 0
    do
    {
#endif
	rule = symb_to_rule_hd [symbol];

#if EBUG
	if (rule >= parse_stack.hook_rule)
	    sxtrap (ME, "hook_construction");
#endif
	while (rule > 0)
	{
	    grammar_put (rule);

	    parse_stack.grammar [parse_stack.grammar_top].next_item = parse_stack.rule [rule].item_hd;
	    parse_stack.rule [rule].item_hd = parse_stack.grammar_top;

	    rule = symb_to_rule_list [rule];
	}
#if 0
    } while ((symbol = equiv_list [symbol]) > 0);
#endif

    if (parse_stack.grammar_top > old_grammar_top + 2)
	/* vrai hook, on le valide. */
	symb_to_rule_hd [symbol] = parse_stack.rule_top;
    else {
	/* Singleton, on l'utilise directement. */
	symb_to_rule_hd [symbol] = rule = parse_stack.grammar [parse_stack.grammar_top].rhs_symb;
	/* On remet en place */
	parse_stack.rule [rule].item_hd = parse_stack.grammar [parse_stack.grammar_top].next_item;
	parse_stack.rule_top = old_rule_top;
	parse_stack.grammar_top = old_grammar_top;
    }
}

#if 0
static SXVOID hook_construction (symbol)
    int symbol;
{
    int 		rule, lhs, rhs, tnt, old_rule_top, old_grammar_top;
    struct grammar	*aelem, *lim;

    PUSH (symbol_stack, symbol);
    sxba_empty (symbol_set);
    SXBA_1_bit (symbol_set, symbol);
    old_rule_top = parse_stack.rule_top;
    old_grammar_top = parse_stack.grammar_top;
    parse_stack.rule_top++;
    grammar_new_rule (0); /* On ne touche pas a symb_to_rule */
    grammar_put (symbol);

    do {
	POP (symbol_stack, lhs);
	rule = symb_to_rule_hd [lhs];

	if (rule < parse_stack.hook_rule) {
	    /* Il peut ne rester que des "unit". */
	    while (rule > 0) {
		grammar_put (rule);

		parse_stack.grammar [parse_stack.grammar_top].next_item = parse_stack.rule [rule].item_hd;
		parse_stack.rule [rule].item_hd = parse_stack.grammar_top;

		rule = symb_to_rule_list [rule];
	    }

	    XxY_Xforeach (parse_stack.unit_rules, lhs, rule) {
		/* unit production */
		tnt = XxY_Y (parse_stack.unit_rules, rule);
		    
		if (SXBA_bit_is_reset_set (symbol_set, tnt)) {
		    PUSH (symbol_stack, tnt);
		}
	    }
	}
	else {
	    /* Already processed as a hook. */
	    aelem = parse_stack.grammar + parse_stack.rule [rule].hd;
	    lim = aelem + parse_stack.rule [rule].lgth;

	    while (++aelem < lim) {
	        rule = aelem->rhs_symb;
		grammar_put (rule);
		parse_stack.grammar [parse_stack.grammar_top].next_item = parse_stack.rule [rule].item_hd;
		parse_stack.rule [rule].item_hd = parse_stack.grammar_top;
	    }
	}
    } while (symbol_stack [0] > 0);
	
    if (parse_stack.grammar_top > old_grammar_top + 2)
	/* vrai hook, on le valide. */
	symb_to_rule_hd [symbol] = parse_stack.rule_top;
    else {
	/* Singleton, on l'utilise directement. */
	symb_to_rule_hd [symbol] = rule = parse_stack.grammar [parse_stack.grammar_top].rhs_symb;
	/* On remet en place */
	parse_stack.rule [rule].item_hd = parse_stack.grammar [parse_stack.grammar_top].next_item;
	parse_stack.rule_top = old_rule_top;
	parse_stack.grammar_top = old_grammar_top;
    }
}
#endif


static SXVOID unused_rule_elimination ()
{
    int			lhs_symbol, rule, item, tnt, lim, symbol, rhs, rule_top;
    struct grammar	*pgram;
    struct rule		*prule;

    if (parse_stack.for_parsact.post_td_init == NULL) {
	parse_stack.rule_size = XxY_top (parse_stack.sf_rule)
	    /* + XxY_top (parse_stack.unit_rules) */ + 1;

	symb_to_rule_hd = (int*) sxcalloc ((unsigned long int)XxY_top (parse_stack.symbols) + 1, sizeof (int));

	symb_to_rule_list = (int*) sxalloc (parse_stack.rule_size + 1, sizeof (int));

	parse_stack.rule = (struct rule*) sxalloc (parse_stack.rule_size + 1, sizeof (struct rule));
	/* parse_stack.rule_top = 0; */
	parse_stack.rule [0].hd = parse_stack.rule [0].lgth = parse_stack.rule [0].item_hd = 0;

	parse_stack.grammar =
	    (struct grammar*) sxalloc ((parse_stack.grammar_size = 4 * parse_stack.rule_size) + 1,
				       sizeof (struct grammar));
	/* parse_stack.grammar_top = 0; */
	parse_stack.grammar [0].rhs_symb = parse_stack.grammar [0].lhs_rule = parse_stack.grammar [0].next_item = 0;

	/* "root" creation */
	parse_stack.rule_top++;
	grammar_new_rule (0);
	grammar_put (0);
	grammar_put (parse_stack.start_symbol);
	parse_stack.root = parse_stack.rule_top;
	hook_symbol_set = sxba_calloc (XxY_top (parse_stack.symbols) + 1);
    }
    else
	/* Ce sont les "parsact" qui se chargent du stockage de la grammaire */
	(*parse_stack.for_parsact.post_td_init) ();

    rhs_stack = SS_alloc (20);
    symbol_stack = (int*) sxcalloc ((unsigned long int)XxY_top (parse_stack.symbols) + 1, sizeof (int));
    symbol_stack [0] = -1;
    parse_stack.useful_ntset = sxba_calloc (XxY_top (parse_stack.symbols) + 1);
    parse_stack.useful_tset = sxba_calloc (sxplocals.Mtok_no + 1);

    /* lhs_symbol est la LHS de plusieurs regles. C'est un hook a moins que
       lhs_symbol soit lui-meme en RHS d'une unit-rule. Dans ce cas cette definition
       multiple sera integree au cours du calcul associe a l'exploitation de cette
       unit-rule. */
    
#if 0
    if (parse_stack.is_unit_rules)
    {
	/* On construit les classes d'équivalence */
	/* On suppose que si un nt est atteint depuis l'axiome, tous les elements de
	   la classe sont atteints. */
	SXBA	subset = sxba_calloc (XxY_top (parse_stack.symbols) + 1);
	int	*substack = (int*) sxcalloc (XxY_top (parse_stack.symbols) + 1, sizeof (int));
	substack [0] = -1;

	equiv = (int*) sxcalloc (XxY_top (parse_stack.symbols) + 1, sizeof (int));
	equiv_list = (int*) sxalloc (XxY_top (parse_stack.symbols) + 1, sizeof (int));
	{
	  int	unit_rule;

	  for (unit_rule = 1; unit_rule <= XxY_top (parse_stack.unit_rules); unit_rule++) {
	    lhs_symbol = XxY_X (parse_stack.unit_rules, unit_rule);
	    SXBA_1_bit (symbol_set, lhs_symbol);
	    tnt = XxY_Y (parse_stack.unit_rules, unit_rule);
	    SXBA_1_bit (symbol_set, tnt);
	  }
	}

	symbol = 0;

	while((symbol = sxba_scan (symbol_set, symbol)) > 0)
	{
	    int		eq, prev_symbol;

	    SXBA_1_bit (subset, symbol);
	    PUSH (substack, symbol);

	    do
	    {
		POP (substack, symbol);
		SXBA_0_bit (symbol_set, symbol);

		XxY_Xforeach (parse_stack.unit_rules, symbol, rule)
		{
		    tnt = XxY_Y (parse_stack.unit_rules, rule);

		    if (SXBA_bit_is_reset_set (subset, tnt))
		      PUSH (substack, tnt);
		}

		XxY_Yforeach (parse_stack.unit_rules, symbol, rule)
		{
		    lhs_symbol = XxY_X (parse_stack.unit_rules, rule);

		    if (SXBA_bit_is_reset_set (subset, lhs_symbol))
		      PUSH (substack, lhs_symbol);
		}
		
	    }  while (substack [0] > 0);

	    symbol = eq = sxba_scan_reset (subset, 0);
	    equiv [eq] = eq;

	    while((symbol = sxba_scan_reset (subset, prev_symbol = symbol)) > 0)
	    {
		equiv_list [prev_symbol] = symbol;
		equiv [symbol] = eq;
	    }

	    equiv_list [prev_symbol] = 0;
	}

	sxfree (substack);
	sxfree (subset);
    }
#endif

    push_new_nt (parse_stack.start_symbol);

    do {
	POP (symbol_stack, lhs_symbol);
	rule_top = parse_stack.rule_top;

	XxY_Xforeach (parse_stack.sf_rule, lhs_symbol, rule) {
	    rhs = XxY_Y (parse_stack.sf_rule, rule);
	    SS_clear (rhs_stack);
	    SS_push (rhs_stack, lhs_symbol);
	    tnt = 0;

	    while (rhs > 0) {
		tnt = XxY_X (parse_stack.sf_rhs, rhs);
		rhs = XxY_Y (parse_stack.sf_rhs, rhs);

		if (rhs > 0
		    || tnt == 0 /* eof de fin ou error (fin de rhs) */) {
		    SS_push (rhs_stack, tnt);
			
		    if (tnt != 0)
			push_new_nt (tnt);
		}
	    }

	    expand_rule (tnt);
	}

	if (parse_stack.for_parsact.post_td_action == NULL) {
	    if (parse_stack.rule_top - rule_top > 1
		/* && (!parse_stack.is_unit_rules || equiv [lhs_symbol] == 0) */)
	    {
#if 0
		/* regle multiple qui n'est pas element d'une classe d'equivalence */
		equiv_list [lhs_symbol] = 0;
		equiv [lhs_symbol] = lhs_symbol;
#endif
		SXBA_1_bit (hook_symbol_set, lhs_symbol);
	    }
	}
    } while (symbol_stack [0] > 0);
	
    sxfree (symbol_stack);
    SS_free (rhs_stack);

    if (parse_stack.for_parsact.post_td_final == NULL) {
	/* Seule la presence de "hook" est caracteristique de l'ambiguite. Il y a deux facons
	   d'en obtenir :
	   - unit productions validees (remontant jusqu'a l'axiome) : cas de deux regles differentes
	   A -> alpha et A -> beta (meme LHS) ou alpha et beta derivent dans la meme sous-chaine
	   source
	   - ou une seule regle A -> alpha telle que alpha derive de plus d'une fac,on dans la
	   meme sous-chaine source. */

	parse_stack.hook_rule = parse_stack.rule_top + 1;

	symbol = sxba_scan (hook_symbol_set, 0);
    
	if (symbol > 0)
	{
	    do
	    {
		hook_construction (symbol);
	    } while ((symbol = sxba_scan (hook_symbol_set, symbol)) > 0);

	    parse_stack.is_ambiguous = parse_stack.rule_top >= parse_stack.hook_rule;
	}
	else
	    parse_stack.is_ambiguous = FALSE;

#if 0
	if (parse_stack.is_unit_rules)
	{
	    sxfree (equiv);
	    sxfree (equiv_list);
	}
#endif

	sxfree (hook_symbol_set);

	for (rule = 1; rule < parse_stack.hook_rule; rule++) {
	    prule = parse_stack.rule + rule;
	    item = prule->hd;
	    pgram = parse_stack.grammar + item;
	    lim = item + prule->lgth;

	    while (++item < lim)
	    {
		pgram++;

		if (pgram->rhs_symb > 0)
		{
		    pgram->rhs_symb = symb_to_rule_hd [pgram->rhs_symb];
		    prule = parse_stack.rule + pgram->rhs_symb;
		    pgram->next_item = prule->item_hd;
		    prule->item_hd = item;
		}
	    }
	}

	sxfree (symb_to_rule_hd);
	sxfree (symb_to_rule_list);

	parse_stack.start_symbol =
	    parse_stack.grammar [parse_stack.rule [parse_stack.root].hd + 1].rhs_symb;

	/* Ca permet aux "traitement semantique" de supprimer des regles par :
	   CTRL_PUSH (parse_stack.erased, rule_no)
	   puis d'appeler
	   sxndparse_pack_forest ()
	   */
	CTRL_ALLOC (parse_stack.erased, parse_stack.rule_top + 1);
	CTRL_ALLOC (parse_stack.reached, parse_stack.rule_top + 1);
    }
    else
	(*parse_stack.for_parsact.post_td_final) (parse_stack.start_symbol);

}



#ifdef LDBG
static SXVOID output_symbol (symbol)
    int symbol;
{
    int				reduce, range, i, j, k, kind;

    kind = XxY_X (parse_stack.symbols, symbol);

    if (kind > sxplocals.SXP_tables.P_xnbpro) {
	kind -= sxplocals.SXP_tables.P_xnbpro;
	reduce = XxY_X (parse_stack.suffixes, kind);
	k = XxY_Y (parse_stack.suffixes, kind);
    }
    else {
	reduce = kind;
	k = 0;
    }

    range = XxY_Y (parse_stack.symbols, symbol);

    if (range < 0) {
	i = -range;
	j = i + 1;
    }
    else {
	i = XxY_X (parse_stack.ranges, range);
	j = XxY_Y (parse_stack.ranges, range);
    }

    /* dans tous les cas (scanner [non-]deterministe), les bornes [a..b] de range designent des
       milestones donc des reperes dans le texte source et en fait un segment du source. */
    if (k == 0)
	printf ("<%i, [%i .. %i]> ",
		reduce,
		i,
		j);
    else
	printf ("<%i, %i, [%i .. %i]> ",
		reduce,
		k,
		i,
		j);
}



static SXVOID output_shared_forest ()
{
    int		rule, unit_rule, lhs, rhs, symbol;
    struct sxtoken	*atok;

    fputs ("\n*\tUnprocessed shared parse forest denotation.\n\n", stdout);

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
	
	printf ("%s%i\t: ", XxY_is_erased (parse_stack.sf_rule, rule) ? "*" : "", rule);
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
		    && atok->string_table_entry != ERROR_STE)
		    printf ("\"%s\"(%d) ", sxstrget (atok->string_table_entry), symbol);
		else 
		    printf ("\"%s\"(%d) ", sxttext (sxplocals.sxtables, atok->lahead), symbol);
	    }
	    
	    rhs = XxY_Y (parse_stack.sf_rhs, rhs);
	}
	
	fputs (";\n", stdout);
    }
    
#if 0
    if (parse_stack.is_unit_rules) {
	fputs ("*\tunit rules\n", stdout);
	rule--;

	for (unit_rule = 1; unit_rule <= XxY_top (parse_stack.unit_rules); unit_rule++) {
	    lhs = XxY_X (parse_stack.unit_rules, unit_rule);
	    printf ("%s%i\t: ", XxY_is_erased (parse_stack.unit_rules, unit_rule) ? "*" : "",
		    rule + unit_rule);
	    output_symbol (lhs);
	    fputs ("\t= ", stdout);
	    symbol = XxY_Y (parse_stack.unit_rules, unit_rule);
	    output_symbol (symbol);
	    fputs (";\n", stdout);
	}
    }
#endif

    fputs ("\n\n", stdout);
}
#endif

    
    
#if EBUG
static SXVOID output_rule (rule)
    int rule;
{
    int			item, symbol, range, lim, i, j, reduce;
    char		*third_field;
    struct sxtoken	*atok;

    if (rule == parse_stack.root)
	printf ("%i\t: <start_symbol, [0 .. %i]>\t= <%i> ;\n",
		rule,
		parse_stack.current_milestone + 1,
		parse_stack.start_symbol);
    else {
	item = parse_stack.rule [rule].hd;
	symbol = parse_stack.grammar [item].rhs_symb;

	if (parse_stack.for_parsact.third_field != NULL)
	{
	    (*parse_stack.for_parsact.third_field) (&symbol, &third_field);
	}
	else
	    third_field = NULL;

	reduce = XxY_X (parse_stack.symbols, symbol);

	if (reduce > sxplocals.SXP_tables.P_xnbpro)
	    reduce -= sxplocals.SXP_tables.P_xnbpro;

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
	    printf ("%i\t: <%s %i, [%i .. %i]%s%s>\t= ",
		    rule,
		    rule >= parse_stack.hook_rule ? "hook" : "reduce",
		    reduce,
		    i,
		    j,
		    third_field == NULL ? "" : ", ",
		    third_field == NULL ? "" : third_field);
	}
	else
	    printf ("%i\t: <reduce %i, void at milestone #%i%s%s>\t= ",
		    rule,
		    reduce,
		    i,
		    third_field == NULL ? "" : ", ",
		    third_field == NULL ? "" : third_field);
	
	lim = item + parse_stack.rule [rule].lgth;
	
	while (++item < lim) {
	    symbol = parse_stack.grammar [item].rhs_symb;
	    
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
				&& atok->string_table_entry != ERROR_STE)
		    printf ("\"%s\"(%d) ", sxstrget (atok->string_table_entry), symbol);
		else 
		    printf ("\"%s\"(%d) ", sxttext (sxplocals.sxtables, atok->lahead), symbol);
	    }
	}
	
	fputs (";\n", stdout);
    }
}


static SXVOID output_grammar ()
{
    int		rule;

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

static SXVOID do_put_error ()
{
    int 	i, j, k, tok_no, error_tok_no;
    BOOLEAN	output_all;

    for (i = 0; i <= parse_stack.rcvr.repair_list_top; i = j) {
	/* On repere l'ensemble des corrections [i..j[ relatives a la meme erreur. */
	j = i;
	error_tok_no = parse_stack.rcvr.repair_list [i].error_tok_no;

	while (++j <= parse_stack.rcvr.repair_list_top
	       && parse_stack.rcvr.repair_list [j].error_tok_no == error_tok_no);
	
	/* On regarde si au moins un token de ce groupement a ete utilise. */
	output_all = TRUE;

	for (k = i; k < j; k++) {
	    tok_no = parse_stack.rcvr.repair_list [k].repair_tok_no;

	    if (SXBA_bit_is_set (parse_stack.useful_tset, tok_no)) {
		output_all = FALSE;
		break;
	    }
	}

	/* Si aucun n'est atteint, on les sort tous, sinon on ne sort que les atteints. */

	for (k = i; k < j; k++) {
	    tok_no = parse_stack.rcvr.repair_list [k].repair_tok_no;

	    if (output_all || SXBA_bit_is_set (parse_stack.useful_tset, tok_no))
		output_repair_string (k);
	}
    }
}


static VOID sxndparse_it ()
{
    int parser, scan_ref, tok_no, xmscan;

#if EBUG
    sxtime (INIT, "Let's go...\n");
#endif    

    for (;;) {
	do {
	    /* cas du scanner deterministe */
	    /* read next token */
	    if (++sxplocals.ptok_no > sxplocals.Mtok_no) {
		do {
		    (*(sxplocals.SXP_tables.scanit)) ();
		} while (sxplocals.ptok_no > sxplocals.Mtok_no);
	    }

	    parse_stack.current_token = parse_stack.current_milestone = sxplocals.ptok_no;
	    parse_stack.current_lahead = SXGET_TOKEN (sxplocals.ptok_no).lahead;
	    
	    parse_stack.for_scanner.current_hd = parse_stack.for_scanner.next_hd;
	    parse_stack.for_scanner.top = FS_INCR (parse_stack.for_scanner.top);
	    parse_stack.for_scanner.next_hd =
		parse_stack.for_scanner.previouses + parse_stack.for_scanner.top;

	    *parse_stack.for_scanner.next_hd = 0;
	    
	    for (parser = *parse_stack.for_scanner.current_hd;
		 parser > 0;
		 parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
		scan_ref = parse_stack.parser_to_attr [parser].scan_ref;
		tok_no = -XxYxZ_Y (parse_stack.parsers, parser);

		if (scan_ref < 0) {
		    XxY_Xforeach (parse_stack.mscans, parser, xmscan) {
			sxndsubparse_a_token (parser, XxY_Y (parse_stack.mscans, xmscan), tok_no, NULL);
		    }

		    scan_ref = -scan_ref;
		}

		sxndsubparse_a_token (parser, scan_ref, tok_no, NULL);
	    }
	    
	    if (parse_stack.for_reducer.top > 0)
		reducer ();
	} while (*parse_stack.for_scanner.next_hd > 0);
	
	/* if (parse_stack.halt_hit) => EOF, normal case */
	/* Else, syntax error on sxplocals.ptok_no */
	/* for_scanner [for_scanner_top] designates the t_trans on the previous
	   (sxplocals.ptok_no - 1) non error token. */
	/* After Recovery, for_scanner_top, next_for_scanner and ptok_no should be correct */

	if (parse_stack.halt_hit || !((*sxplocals.SXP_tables.recovery) (ACTION)))
	    break;

	if (parse_stack.milstn_tail != NULL) {
	    int 			bxyz, fxyz, prev_node, cur_node,
	                                prev_token, prev_parser, next_parser;
	    int				*pnfs;
	    struct sxmilstn_elem	*cur_ms_ptr;

	    /* Cas de corrections MULTIPLES qui sont stockees ds sxndtkn.dag */
	    /* parse_stack.milstn_tail n'est pas touche pour GC. */
	    parse_stack.is_token_dag = TRUE; /* Pour ARC_traversal */
	    cur_ms_ptr = parse_stack.milstn_tail;

	    while (cur_ms_ptr->next_milstn != NULL) {
		*parse_stack.for_scanner.next_hd = 0;
		parse_stack.current_milestone = cur_ms_ptr->my_milestone;
		cur_node = cur_ms_ptr->my_index;

		XxYxZ_Xforeach (sxndtkn.dag, cur_node, fxyz) {
		    parse_stack.current_token = XxYxZ_Y (sxndtkn.dag, fxyz);
		    parse_stack.current_lahead = SXGET_TOKEN (parse_stack.current_token).lahead;

		    XxYxZ_Zforeach (sxndtkn.dag, cur_node, bxyz) {
			prev_node = XxYxZ_X (sxndtkn.dag, bxyz);
			prev_token = XxYxZ_Y (sxndtkn.dag, bxyz);
		    
			for (parser = parse_stack.rcvr.milstn_elems [prev_node].next_for_scanner;
			     parser > 0;
			     parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
			    /* prev_node peut avoir des transitions qui ne conduisent pas
			       a cur_node. */
			    if (XxYxZ_Y (parse_stack.parsers, parser) == prev_token) {
				scan_ref = parse_stack.parser_to_attr [parser].scan_ref;

				if (scan_ref < 0) {
				    XxY_Xforeach (parse_stack.mscans, parser, xmscan) {
					sxndsubparse_a_token (parser,
							      XxY_Y (parse_stack.mscans, xmscan),
							      -prev_token /* symbol */,
							      NULL);
				    }

				    scan_ref = -scan_ref;
				}

				sxndsubparse_a_token (parser, scan_ref, -prev_token /* symbol */, NULL);
			    }
			}
		    }
		}
		
		if (parse_stack.for_reducer.top > 0)
		    reducer ();
		
		if (*parse_stack.for_scanner.next_hd) {
		    /* Non vide */
		    cur_ms_ptr->next_for_scanner = *parse_stack.for_scanner.next_hd;
		}

		/* Le parsing doit valider les memes chaines que l'error recovery!
		   Cependant le token_dag a ete prolonge artificiellement pour avoir
		   un seul noeud de sortie, il est donc possible que certains chemins
		   echouent. On verifiera que le noeud de sortie est non vide. */

		cur_ms_ptr = cur_ms_ptr->next_milstn;
	    }

	    /* On fabrique next_hd */
	    *parse_stack.for_scanner.current_hd = 0; /* simple */
	    *parse_stack.for_scanner.next_hd = 0;
	    prev_parser = 0;
	    cur_node = cur_ms_ptr->my_index;

	    XxYxZ_Zforeach (sxndtkn.dag, cur_node, bxyz) {
		prev_node = XxYxZ_X (sxndtkn.dag, bxyz);
		prev_token = XxYxZ_Y (sxndtkn.dag, bxyz);
		parser = *(pnfs = &(parse_stack.rcvr.milstn_elems [prev_node].next_for_scanner));

		while (parser > 0) {
		    if (XxYxZ_Y (parse_stack.parsers, parser) == prev_token) {
			if (prev_parser == 0)
			   *parse_stack.for_scanner.next_hd = parser;
			else
			    parse_stack.parser_to_attr [prev_parser].next_for_scanner = parser;

			next_parser =
			    parse_stack.parser_to_attr [parser].next_for_scanner;

			/* un parser ne doit appartenir qu'a une seule liste
			   (prev_node peut etre trouve plusieurs fois). */
			*pnfs = next_parser;
			
			parse_stack.parser_to_attr [prev_parser = parser].next_for_scanner = 0;
			parser = next_parser;
		    }
		    else {
			parser =
			    *(pnfs = &(parse_stack.parser_to_attr [parser].next_for_scanner));
		    }
		}
	    }
	    
#if EBUG
	    /* Le parsing doit valider au moins une chaine de l'error recovery!. */
	    if (*parse_stack.for_scanner.next_hd == 0)
		sxtrap (ME, "sxndparse_it");
#endif

	    parse_stack.milstn_tail = NULL;
	    /* ptok_no doit etre correct */
	    parse_stack.is_token_dag = FALSE;
	}
    }
}




static BOOLEAN dag_set (dag_set_index, succ_kind, new_kind)
    int dag_set_index, succ_kind, new_kind;
{
    /* Si tous les successeurs de chaque predecesseur (complet) de dag_set_index
       contient un composant de "succ_kind", alors le kind de ce predecesseur est 
       mis (|=) a new_kind */
    /* Retourne TRUE ssi au moins un milestone est mis a MS_DEAD. */
    int				x, z, prev_ms_nb, next_ms_nb;
    struct sxmilstn_elem 	*prev_ms_ptr, *next_ms_ptr, *cur_ms_ptr;
    BOOLEAN			has_dead = FALSE;

    XxYxZ_Zforeach (sxndtkn.dag, dag_set_index, z) {
	prev_ms_nb = XxYxZ_X (sxndtkn.dag, z);
	prev_ms_ptr = &sxmilstn_access (sxndtkn.milestones, prev_ms_nb);
	
#if EBUG
	if ((prev_ms_ptr->kind & MS_COMPLETE) == 0)
	    sxtrap (ME, "dag_set");
#endif

	if ((prev_ms_ptr->kind & new_kind) == 0) {
	    /* un candidat (pas encore traite') */
	    XxYxZ_Xforeach (sxndtkn.dag, prev_ms_nb, x) {
		next_ms_nb = XxYxZ_Z (sxndtkn.dag, x);
		next_ms_ptr = &sxmilstn_access (sxndtkn.milestones, next_ms_nb);
		
		if ((next_ms_ptr->kind & succ_kind) == 0)
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

		    has_dead = TRUE;
		}
		else if (new_kind == MS_EXHAUSTED) {
		    has_dead = has_dead || dag_set (prev_ms_nb, MS_EXHAUSTED, MS_SLEEP);
		}
		else /* new_kind == MS_SLEEP */ {
		    has_dead = has_dead || dag_set (prev_ms_nb, MS_SLEEP | MS_DEAD, MS_DEAD);
		}
	    }
	}
    }

    if (new_kind == MS_SLEEP && has_dead) {
	/* On a positionne' des MS_DEAD, on les supprime effectivement. */
	cur_ms_ptr = &sxmilstn_access (sxndtkn.milestones, dag_set_index);

	while (cur_ms_ptr != parse_stack.milstn_tail) {
	    /* On "remonte" vers la queue des milestones actifs, mais, pour des
	       raisons techniques, on ne supprime pas le milestone associe' au
	       eof de tete. */
	    if ((cur_ms_ptr->kind & MS_DEAD) != 0) {
		cur_ms_ptr->next_milstn->prev_milstn = cur_ms_ptr->prev_milstn;

		if (cur_ms_ptr->prev_milstn != NULL)
		    cur_ms_ptr->prev_milstn->next_milstn = cur_ms_ptr->next_milstn;

		cur_ms_ptr = cur_ms_ptr->prev_milstn;
	    }
	    else
		cur_ms_ptr = cur_ms_ptr->prev_milstn;
	}

	has_dead = FALSE;
    }

    return has_dead;
}



/*
  Un milestone est MS_BEGIN a sa naissance (le scanner vient de calculer
  toutes ses aretes entrantes)

  Un milestone est MS_COMPLETE quand toutes ses aretes sortantes ont ete
  calculees (il a disparu des variables locales du scanner).

  Un milestone ne peut se calculer que lorsqu'il est MS_COMPLETE et quand tous ses
  predecesseurs sont calcules.

  Un milestone est MS_WAIT quand il est calcule (par sxndparser ()) et non vide

  Un milestone est MS_EMPTY quand il est calcule (par sxndparser ()) et vide

  Un milestone est MS_EXHAUSTED quand il est calcule' et quand tous ses successeurs
  sont calcules (MS_WAIT | MS_EMPTY)

  Un milestone est MS_SLEEP quand tous ses successeurs sont MS_EXHAUSTED

  Un milestone est MS_DEAD (et recuperable) quand tous ses successeurs sont MS_DEAD | MS_SLEEP
*/


static BOOLEAN sxnd2_error_detected ()
{
    int				xyz, next_ms_nb, next_tok_no, parser;
    struct sxmilstn_elem 	*ms_ptr, *next_ms_ptr;

    for (ms_ptr = parse_stack.milstn_tail;
	 ms_ptr != sxndtkn.milstn_current;
	 ms_ptr = ms_ptr->next_milstn) {
	if (ms_ptr->kind & MS_WAIT) {
	    /* calcule (non vide) */
	    XxYxZ_Xforeach (sxndtkn.dag, ms_ptr->my_index, xyz) {
		/* On regarde ses successeurs */
		next_tok_no = XxYxZ_Y (sxndtkn.dag, xyz);
		next_ms_nb = XxYxZ_Z (sxndtkn.dag, xyz);
		next_ms_ptr = &(sxmilstn_access (sxndtkn.milestones, next_ms_nb));

		for (parser = ms_ptr->next_for_scanner;
		     parser > 0;
		     parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
		    if (XxYxZ_Y (parse_stack.parsers, parser) == next_tok_no) {
			/* transition non vide */
			if ((next_ms_ptr->kind & (MS_WAIT + MS_EMPTY)) == 0)
			    /* pas calcule'e */
			    return FALSE; /* Pas d'erreur. */
		    }
		}
	    } 
	}
    }

    return TRUE; /* Detection d'une erreur */
}


static VOID sxnd2parse_it ()
{
    int				x, z, prev_ms_nb, cur_ms_nb, parser, prev_token, scan_ref, xmscan;
    struct sxmilstn_elem 	*prev_ms_ptr;
    BOOLEAN			is_wait;

    for (;;) {
#if EBUG
	if (sxndtkn.milstn_current == NULL)
	    sxtrap (ME, "sxnd2parse_it");
#endif

	while ((sxndtkn.milstn_current->kind & MS_COMPLETE) == 0)
	    (*(sxplocals.SXP_tables.scanit)) ();

	/* On va construire le parser associe a sxndtkn.milstn_current */

	is_wait = FALSE;

#if 0
	/* Le 07/05/2003 my_index est change' en my_milestone!! */
	XxYxZ_Zforeach (sxndtkn.dag, sxndtkn.milstn_current->my_index, z)
#endif

	XxYxZ_Zforeach (sxndtkn.dag, sxndtkn.milstn_current->my_milestone, z) {
	    prev_ms_nb = XxYxZ_X (sxndtkn.dag, z);
	    prev_ms_ptr = &sxmilstn_access (sxndtkn.milestones, prev_ms_nb);

	    if ((prev_ms_ptr->kind & (MS_WAIT + MS_EMPTY)) == 0) {
		/* Les predecesseurs sont obligatoirement deja calcules. */
		sxtrap (ME, "sxnd2parse_it");
	    }

	    if (prev_ms_ptr->kind & MS_WAIT) {
		is_wait = TRUE;
		break;
	    }
	}

	if (is_wait) {
	    parse_stack.current_milestone = cur_ms_nb = sxndtkn.milstn_current->my_milestone;

	    /* Le calcul du milestone courant se fait toujours dans
	       parse_stack.for_scanner.next_hd qui doit etre a 0. */
	    
	    XxYxZ_Xforeach (sxndtkn.dag, cur_ms_nb, x) {
		parse_stack.current_token = XxYxZ_Y (sxndtkn.dag, x);
		parse_stack.current_lahead = SXGET_TOKEN (parse_stack.current_token).lahead;

		XxYxZ_Zforeach (sxndtkn.dag, cur_ms_nb, z) {
		    prev_ms_nb = XxYxZ_X (sxndtkn.dag, z);
		    prev_ms_ptr = &sxmilstn_access (sxndtkn.milestones, prev_ms_nb);
		    prev_token = XxYxZ_Y (sxndtkn.dag, z);
		    
		    if (prev_ms_ptr->kind & MS_WAIT) {
			for (parser = prev_ms_ptr->next_for_scanner;
			     parser > 0;
			     parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
			    /* prev_ms_nb peut avoir des transitions qui ne conduisent pas
			       a cur_ms_nb. */
			    if (XxYxZ_Y (parse_stack.parsers, parser) == prev_token) {
				scan_ref = parse_stack.parser_to_attr [parser].scan_ref;

				if (scan_ref < 0) {
				    XxY_Xforeach (parse_stack.mscans, parser, xmscan) {
					sxndsubparse_a_token (parser,
							      XxY_Y (parse_stack.mscans, xmscan),
							      -prev_token /* symbol */,
							      NULL);
				    }

				    scan_ref = -scan_ref;
				}

				sxndsubparse_a_token (parser, scan_ref, -prev_token /* symbol */, NULL);
			    }
			}
		    }
		}
	    }
	    
	    if (parse_stack.for_reducer.top > 0)
		reducer ();
	    
	    sxndtkn.milstn_current->next_for_scanner = *parse_stack.for_scanner.next_hd;

	    if (*parse_stack.for_scanner.next_hd) {
		/* Non vide */
		*parse_stack.for_scanner.next_hd = 0;
		sxndtkn.milstn_current->kind |= MS_WAIT;
	    
		/* On regarde si certains des predecesseurs de milstn_current
		   peuvent devenir MS_EXHAUSTED */
		dag_set (cur_ms_nb, MS_WAIT | MS_EMPTY, MS_EXHAUSTED);
		sxndtkn.milstn_current = sxndtkn.milstn_current->next_milstn;
	    }
	    else if (!parse_stack.halt_hit) {
		/* On detecte une erreur ssi tous les milestones non vides deja traites
		   sont tels que tous leurs successeurs sont deja traite's. */
		sxndtkn.milstn_current->kind |= MS_EMPTY; /* Pour sxnd2_error_detected */

		if (sxnd2_error_detected ()) {
		    if (!(*sxplocals.SXP_tables.recovery) (ACTION))
			break;

		    /* Tout a ete repositionne correctement */
		    /* milstn_current est correct. */
		}
		else
		   sxndtkn.milstn_current = sxndtkn.milstn_current->next_milstn; 
	    }
	    else
		break; /* OK, fin de l'analyse */
	}
	else {
	    /* Tous les predecesseurs sont vides */
	    sxndtkn.milstn_current->kind |= MS_EMPTY;
	    sxndtkn.milstn_current = sxndtkn.milstn_current->next_milstn;
	}
    }
}


static BOOLEAN sxnddesambig_it ()
{
    extern int sxvoid ();

#if EBUG
    sxtime (FINAL, "\nScanning, Non-Deterministic Parsing & Parse Forest Sharing done in");
    fprintf (sxtty, "Source text: %d tokens, %d shift-transitions, %d reduce-steps.\n",
	     sxplocals.Mtok_no - 1, parse_stack.shift_trans_nb, parse_stack.red_trans_nb);
#endif    

#ifdef LDBG
    output_shared_forest ();
    sxtime (FINAL, "Shared Parse Forest Printing done in");
#endif

    if (parse_stack.start_symbol == 0) {
	if (parse_stack.rcvr.repair_list != NULL)
	    output_repair_list ();

	parse_stack.root = 0;
	parse_stack.rule = NULL;
	parse_stack.grammar = NULL;
#if EBUG
	fputs ("*\tThere is no start symbol in this shared parse forest denotation.\n\n", stdout);
#endif

    }
    else {
	/* Si les fonctions top_down (for_parsact.post_td_(init, action, final)) sont positionnees
	   unused_rule_elimination les appelle directement et ne CONSTRUIT PAS parse_stack.grammar. */
	unused_rule_elimination ();

	parse_stack.G.G = parse_stack.grammar_top;
	parse_stack.G.N = parse_stack.G.P = parse_stack.rule_top;
	parse_stack.G.T = sxplocals.Mtok_no - 1;

	if (parse_stack.rcvr.repair_list != NULL)
	    do_put_error ();

#if EBUG
	sxtime (FINAL, "Unused Sub-Tree Erasing done in");

	if (parse_stack.grammar != NULL) {
	    output_grammar ();
	    sxtime (FINAL, "Shared Parse Forest Printing done in");
	}
#endif
    }

    if (sxplocals.mode.with_semact) {
	/* Ici, il faut que "grammar" soit construite... */
	sxplocals.mode.mode = SXDESAMBIG;

	if (sxplocals.mode.global_errors_nb == 0) {
	    if (parse_stack.root != 0) {
		if (parse_stack.for_parsact.post != NULL ||
		    /* parse_stack.parsact_seen ||*/ parse_stack.parsprdct_seen) {
		    BOOLEAN	failed;

		    if (parse_stack.for_parsact.post != NULL) {
			if (parse_stack.for_parsact.post () == 0)
			    /* La foret a disparue..., les predicats y sont alles un peu fort. */
			    parse_stack.start_symbol = 0;
		    }
		    else {
			/* Doit-t-on executer les pars(act|prdct) si (pour un texte donne)
			   la foret partagee est un arbre ?
			   (le but des pars(act|prdct) est de produire un arbre, les erreurs eventuelles
			   seront detectee au cours des phases ulterieures).
			   
			   Si oui, doit-t-on executer les parsact s'il n'y a pas de parsprdct ?
			   */

			/* sxndtw.(pass_inherited |
			   pass_derived |
			   cycle_processing |
			   open_hook |
			   close_hook |
			   nodes_oflw)
			   on deja ete positionnes par l'appel de sxndtw_open ()
			   depuis (*sxplocals.SXP_tables.parsact) (INIT)
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

			sxndtw_final ();
			sxndtw_close ();
		    }
	  
#if EBUG
		    sxtime (FINAL, "Action & Predicate Phase done in");
		    fputs ("\n\n*\tAfter \"Action & Predicate\" phase.\n\n", stdout);
		    output_grammar ();
		    sxtime (FINAL, "Parse Tree Printing done in");
#endif

		}

		if (parse_stack.start_symbol != 0
		    && parse_stack.is_ambiguous
		    && sxplocals.SXP_tables.desambig != sxvoid) {
		    (*sxplocals.SXP_tables.desambig) (OPEN);
		    (*sxplocals.SXP_tables.desambig) (INIT);
		    (*sxplocals.SXP_tables.desambig) (ACTION);

#if EBUG
		    sxtime (FINAL, "Desambiguation Phase done in");
		    fputs ("\n\n*\tAfter desambiguation phase.\n\n", stdout);
		    output_grammar ();
		    sxtime (FINAL, "Parse Tree Printing done in");
#endif
	    
		    (*sxplocals.SXP_tables.desambig) (FINAL);
		    (*sxplocals.SXP_tables.desambig) (CLOSE);
		}
	    }
      
	    if (parse_stack.is_ambiguous)
		parse_stack.start_symbol = 0;
	}
	else
	    parse_stack.start_symbol = 0;

	if (parse_stack.start_symbol > 0
	    && sxplocals.SXP_tables.semact != sxvoid)
	    /* Here, simulate on the resulting tree the semantic calls as if the parser
	       was deterministic. */
	    sxndtw_sem_calls ();
    }

    return parse_stack.start_symbol > 0;
}




void
sxndparse_erase_hook_item (item)
    int item;
{
    /* Appele depuis la "semantique" qui a decide de supprimer l'element d'indice
       "item" d'un hook. */
    /* On le remplace par le plus a droite. */
    /* Attention : les liens "item_hd", "next_item" ne sont pas touches, ce qui oblige
       quand on les parcourt a tester le "symbole" courant. */
    struct grammar	*pgram = parse_stack.grammar + item, *prgram;
    struct rule 	*prule = parse_stack.rule + pgram->lhs_rule;
    int			right_item = prule->hd + prule->lgth - 1;

    /* On fait "disparaitre" l'element */
    if (item == right_item)
    {
	/* C'est le plus a droite */
	pgram->rhs_symb = 0;
    }
    else
    {
	/* On deplace le plus a droite */
	prgram = parse_stack.grammar + right_item;
	pgram->rhs_symb = prgram->rhs_symb;
	prgram->rhs_symb = 0;
	pgram->next_item = prgram->next_item;
	prgram->next_item = item;
    }

    prule->lgth--;
}

BOOLEAN
sxndparse_clean_forest ()
{
    /* L'ensemble les regles supprimees se trouve ds "erased". */

    /* Calcule la (nouvelle) taille de la foret. */
    /* Un symbole est accessible ssi il existe au moins un chemin
       entre la racine et une feuille qui le traverse. */
    /* Les hooks singletons sont elimines. */
    /* Les regles inutiles (n'appartenant a aucun chemin actif) sont eliminees. */
    /* positionne is_ambiguous si la foret n'est pas un arbre */
    /* Positionne "reached' l'ensemble des symboles accessibles. */

    /* Retourne FALSE ssi la foret a disparue. */

    int			lhs_rule_no, rhs_rule_no, rule_lgth, item, next_item, lim; 
    struct rule		*plhs;

    if (!CTRL_IS_EMPTY (parse_stack.erased))
    {
	/* On commence par "remonter" les regles inaccessibles */
	CTRL_FOREACH (parse_stack.erased, rhs_rule_no)
	{
	    for (item = parse_stack.rule [rhs_rule_no].item_hd;
		 item != 0;
		 item = next_item)
	    {
		next_item = parse_stack.grammar [item].next_item;

		if (parse_stack.grammar [item].rhs_symb == rhs_rule_no)
		{
		    lhs_rule_no = parse_stack.grammar [item].lhs_rule;
		    plhs = parse_stack.rule + lhs_rule_no;

		    if (plhs->lgth > 0)
		    {
			if (lhs_rule_no >= parse_stack.hook_rule)
			{
			    /* C'est un hook */
			    sxndparse_erase_hook_item (item);

			    if (plhs->lgth == 1)
				/* Le hook disparait */
				CTRL_PUSH (parse_stack.erased, lhs_rule_no);
			}
			else
			    /* la regle disparait */
			    CTRL_PUSH (parse_stack.erased, lhs_rule_no);
		    }
		}
	    }

	    parse_stack.rule [rhs_rule_no].lgth = 0;
	    parse_stack.rule [rhs_rule_no].item_hd = 0;
	}
    }

    parse_stack.G.T = sxplocals.Mtok_no - 1;
    parse_stack.G.G = parse_stack.G.N = parse_stack.G.P = 0;

    /* On note les regles accessibles depuis l'axiome */

    parse_stack.is_ambiguous = FALSE;

    if (parse_stack.rule [parse_stack.root].lgth == 0)
	/* Ce n'est meme plus un arbre d'analyse */
	return FALSE;

    CTRL_CLEAR (parse_stack.reached);
    CTRL_PUSH (parse_stack.reached, parse_stack.root);

    CTRL_FOREACH (parse_stack.reached, lhs_rule_no)
    {
	rule_lgth = parse_stack.rule [lhs_rule_no].lgth; /* > 0 */

#if EBUG
	if (rule_lgth == 0)
	    sxtrap (ME, "sxndparse_clean_forest");
#endif

	parse_stack.G.G += rule_lgth;
	parse_stack.G.N++;
	parse_stack.G.P++;
	item = parse_stack.rule [lhs_rule_no].hd;
	lim = item + rule_lgth;

	while (++item < lim)
	{
	    if ((rhs_rule_no = parse_stack.grammar [item].rhs_symb) > 0)
		/* Non terminal */
		CTRL_PUSH (parse_stack.reached, rhs_rule_no);
	}
    }

    /* On incorpore les hook qui sont devenus des singletons */
    parse_stack.rule_top = parse_stack.hook_rule;
    lhs_rule_no = parse_stack.hook_rule - 1;

    while ((lhs_rule_no = sxba_scan (CTRL2SXBA (parse_stack.reached), lhs_rule_no)) > 0)
    {
	if (parse_stack.rule [lhs_rule_no].lgth == 2)
	{
	    /* singleton */
	    item = parse_stack.rule [lhs_rule_no].hd + 1;
	    rhs_rule_no = parse_stack.grammar [item].rhs_symb;
	    parse_stack.grammar [item].rhs_symb = 0;
	    
	    for (item = parse_stack.rule [lhs_rule_no].item_hd;
		 item != 0;
		 item = next_item)
	    {
		next_item = parse_stack.grammar [item].next_item;

		if (parse_stack.grammar [item].rhs_symb == lhs_rule_no)
		{
		    parse_stack.grammar [item].rhs_symb = rhs_rule_no;
		    parse_stack.grammar [item].next_item = parse_stack.rule [rhs_rule_no].item_hd;
		    parse_stack.rule [rhs_rule_no].item_hd = item;
		}
	    }

	    parse_stack.rule [lhs_rule_no].item_hd = 0;
	    parse_stack.rule [lhs_rule_no].lgth = 0;

	    parse_stack.G.G -= 2;
	    parse_stack.G.N--;
	    parse_stack.G.P--;
	    SXBA_0_bit (CTRL2SXBA (parse_stack.reached), lhs_rule_no);
	}
	else
	{
	    parse_stack.rule_top = lhs_rule_no;
	    parse_stack.is_ambiguous = TRUE;
	}
    }

    lhs_rule_no = 0;

    while ((lhs_rule_no = sxba_0_lrscan (CTRL2SXBA (parse_stack.reached), lhs_rule_no)) > 0)
    {
	parse_stack.rule [lhs_rule_no].item_hd = 0;
	parse_stack.rule [lhs_rule_no].lgth = 0;
    }

    return TRUE;
}


void
sxndparse_pack_forest ()
{
    /* L'ensemble les regles accessibles se trouve ds "reached". */
    /* Recree une nouvelle foret partagee dont tous les noeuds sont accessibles. */

    int			item, lim, rhs_symb, old_rule_no, new_rule_no, new_grammar_top;
    struct rule		*por, *pnr, *new_rule;
    struct grammar	*pog, *png, *new_grammar;

    /* On regenere la nouvelle grammaire */
    /* Ca pourrait se faire sur place... */
    if (sxba_0_lrscan (CTRL2SXBA (parse_stack.reached), 0) < 0)
	/* Il n'y a rien a supprimer... */
	return;

    pnr = new_rule = (struct rule*) sxalloc (parse_stack.G.P + 1, sizeof (struct rule));
    png = new_grammar = (struct grammar*) sxalloc (parse_stack.G.G + 1, sizeof (struct grammar));

    old_rule_no = 0;
    new_rule_no = 0;

    while ((old_rule_no = sxba_scan (CTRL2SXBA (parse_stack.reached), old_rule_no)) > 0)
    {
	parse_stack.rule [old_rule_no].item_hd = ++new_rule_no; /* non utilise */
	new_rule [new_rule_no].item_hd = 0;
    }
        
    if (parse_stack.is_ambiguous)
    {
	/* Nouvelle adresse du 1er hook non vide */
	old_rule_no = sxba_scan (CTRL2SXBA (parse_stack.reached), parse_stack.hook_rule - 1);
	parse_stack.hook_rule = parse_stack.rule [old_rule_no].item_hd;
    }
    else
	parse_stack.hook_rule = new_rule_no + 1;

    old_rule_no = 0;
    new_rule_no = 0;
    new_grammar_top = 0;
    /* parse_stack.rule;
       parse_stack.grammar; */

    while ((old_rule_no = sxba_scan (CTRL2SXBA (parse_stack.reached), old_rule_no)) > 0)
    {
	por = parse_stack.rule + old_rule_no;
	pog = parse_stack.grammar + por->hd;

	new_rule_no++, pnr++;
	new_grammar_top++, png++;

#if EBUG
	if (por->lgth == 0)
	    sxtrap (ME, "sxndparse_pack_forest");
#endif

	pnr->lgth = por->lgth;
	pnr->hd = new_grammar_top;

	item = por->hd;
	png->rhs_symb = pog->rhs_symb;
	png->lhs_rule = new_rule_no;

	lim = item + por->lgth;

	while (++item < lim)
	{
	    pog++;
	    new_grammar_top++;
	    png++;

	    rhs_symb = pog->rhs_symb;

	    if (rhs_symb > 0)
	    {
		rhs_symb = parse_stack.rule [rhs_symb].item_hd;
		png->next_item = new_rule [rhs_symb].item_hd;
		new_rule [rhs_symb].item_hd = new_grammar_top;
	    }

	    png->rhs_symb = rhs_symb;
	    png->lhs_rule = new_rule_no;
	}
    }

    sxfree (parse_stack.rule), parse_stack.rule = new_rule;
    sxfree (parse_stack.grammar), parse_stack.grammar = new_grammar;

    parse_stack.rule_size = parse_stack.rule_top = new_rule_no;
    parse_stack.grammar_size = parse_stack.grammar_top = new_grammar_top;

    CTRL_RESIZE (parse_stack.reached, parse_stack.rule_size + 1);
    CTRL_RESIZE (parse_stack.erased, parse_stack.rule_size + 1);

    CTRL_CLEAR (parse_stack.reached);
    CTRL_CLEAR (parse_stack.erased);
}



BOOLEAN sxndparser (what_to_do, arg)
    int		what_to_do;
    struct sxtables	*arg;
{
    int i;
    static int	parsers_foreach [] = {0, 0, 0, 0, 0, 0};

    switch (what_to_do) {
    case BEGIN:
	break;

    case OPEN:
	/* new language: new tables, new local parser variables */
	/* the global variable "sxplocals" must have been saved in case */
	/* of recursive invocation */

	/* test arg->magic for consistency */
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, ME);

	XxY_alloc (&parse_stack.sons, "sons", 128, 1, 1, 0, sons_oflw, stdout_or_NULL);
	
	parse_stack.erasable_sons = sxba_calloc (XxY_size (parse_stack.sons) + 1);
	parse_stack.current_links = sxba_calloc (XxY_size (parse_stack.sons) + 1);
	parse_stack.used_links = sxba_calloc (XxY_size (parse_stack.sons) + 1);
	parse_stack.transitions = (int*) sxalloc (XxY_size (parse_stack.sons) + 1, sizeof (int));

/* Qq mesures sur une grammaire peu ambigüe de C ont donné, pour 1000 tokens analysés,
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

#if 0
	XxY_alloc (&parse_stack.unit_rules, "unit_rules", 500, 1, 1, 1, NULL, stdout_or_NULL);
#endif
	
	XxY_alloc (&parse_stack.suffixes, "suffixes", 128, 1, 0, 0, NULL, stdout_or_NULL);
	XxY_alloc (&parse_stack.ranges, "ranges", 900, 1, 0, 0, NULL, stdout_or_NULL);
	XxY_alloc (&parse_stack.symbols, "symbols", 2400, 1, 0, 0, symbols_oflw, stdout_or_NULL);
	parse_stack.for_reducer.triples =
	    (struct triple*) sxalloc ((i = 50) + 1, sizeof (struct triple));
	parse_stack.for_reducer.size = i; /* size */
	parse_stack.for_reducer.top  = 0; /* top */

	/* parse_stack.for_reducer.refs peut etre utilise soit avec des reduces
	   (cas normal), soit avec des etats LR (error recovery). */
	i = arg->SXP_tables.Mref - arg->SXP_tables.Mprdct;

	if (arg->SXP_tables.Mred > i)
	    i = arg->SXP_tables.Mred;

	parse_stack.for_reducer.refs = (int*) sxcalloc ((unsigned long int)i + 1, sizeof (int));
	parse_stack.for_reducer.refs [0] = -1;
	parse_stack.for_reducer.triples_hd = (int*) sxalloc (arg->SXP_tables.Mred + 1,
							     sizeof (int));

	XxY_alloc (&parse_stack.mscans, "mscans", 30, 1, 1, 0, NULL, stdout_or_NULL); 

	XxYxZ_alloc (&parse_stack.parsers, "parsers", 256, 1, parsers_foreach,
		     parsers_oflw, stdout_or_NULL); 

	i = parse_stack.hole_nb = XxY_size (parse_stack.parsers);
	i++;
	parse_stack.parser_to_attr =
	    (struct parsers_attr*) sxalloc (i, sizeof (struct parsers_attr));

	parse_stack.parsers_set = sxbm_calloc (5, i);
	parse_stack.free_parsers_set = parse_stack.parsers_set [4];

	parse_stack.GC_area = NULL;

    {
	/* Devrait etre statique!!! */
	int			ref;
	struct SXP_reductions	*ared;

	parse_stack.red2act = (int*) sxalloc (arg->SXP_tables.P_xnbpro + 1, sizeof (int));
	ared = arg->SXP_tables.reductions;

	for (ref = 1; ref <= arg->SXP_tables.Mred; ref++) {
	  ared++;
	  parse_stack.red2act [ared->reduce] = ared->action;
        }
    }

/*
   parse_stack est globale et les initialisations suivantes sont faites automatiquement.

	parse_stack.for_parsact.action_top = NULL;
	parse_stack.for_parsact.action_pop = NULL;
	parse_stack.for_parsact.action_bot = NULL;
	parse_stack.for_parsact.action_new_top = NULL;
	parse_stack.for_parsact.action_final = NULL;
	parse_stack.for_parsact.GC = NULL;
	parse_stack.for_parsact.post = NULL;
	parse_stack.for_parsact.third_field = NULL;
	parse_stack.for_parsact.parsers_oflw = NULL;
	parse_stack.for_parsact.sons_oflw = NULL;
	parse_stack.for_parsact.symbols_oflw = NULL;
*/

/* prepare new local variables */

	sxplocals.sxtables = arg;
	sxplocals.SXP_tables = arg->SXP_tables;
	sxtkn_mngr (OPEN, 2);
	(*sxplocals.SXP_tables.recovery) (OPEN);
	break;

    case INIT:
    {
	/* Le token 0 est "EOF" */
	struct sxtoken tok;

	sxtkn_mngr (INIT, 0);
	/* terminal_token EOF */
	tok.lahead = sxplocals.SXP_tables.P_tmax;
	tok.string_table_entry = EMPTY_STE;
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
	sxplocals.mode.is_prefixe = FALSE;
	sxplocals.mode.is_silent = FALSE;
	sxplocals.mode.with_semact = FALSE;
	sxplocals.mode.with_parsact = TRUE;
	sxplocals.mode.with_parsprdct = TRUE;
	sxplocals.mode.with_do_undo = FALSE;

#if EBUG
	parse_stack.shift_trans_nb = 0;
	parse_stack.red_trans_nb = 0;
#endif    

	parse_stack.start_symbol = 0;
	parse_stack.parsact_seen = FALSE;
	parse_stack.parsprdct_seen = FALSE;
	
	parse_stack.ARC.level = 0;
	parse_stack.is_token_dag = FALSE;
	/* parse_stack.is_unit_rules = FALSE; */

	parse_stack.for_reducer.re_do = -1; /* liste des re_do vide */

	parse_stack.for_scanner.top = for_scanner_mask; /* pourquoi pas ! */

	for (i = 0; i <= for_scanner_mask; i++) {
	    parse_stack.for_scanner.previouses [i] = 0;
	}

	parse_stack.for_scanner.next_hd =
	    parse_stack.for_scanner.previouses + parse_stack.for_scanner.top;

	parse_stack.current_milestone = 0;
	parse_stack.current_token = 0;
	parse_stack.current_lahead = sxplocals.SXP_tables.P_tmax;

	create_ref (new_parser (0 /* son */,
				sxplocals.SXP_tables.init_state,
				0 /* symb */ ),
		    NDP_access (sxplocals.SXP_tables.t_bases + sxplocals.SXP_tables.init_state,
				parse_stack.current_lahead) /* ref */);

	parse_stack.halt_hit = FALSE;
	parse_stack.rcvr.is_up = FALSE;
	parse_stack.milstn_tail = NULL;
	parse_stack.delta_generator = 0;

	(*sxplocals.SXP_tables.recovery) (INIT);
	break;
			  
    case ACTION:
	sxndparse_it ();

	if (sxplocals.mode.mode == SXPARSER)
	    sxnddesambig_it ();

	break;

    case FINAL:
	(*sxplocals.SXP_tables.recovery) (FINAL);
	sxtkn_mngr (FINAL, 0);
	break;

    case CLOSE:
	/* end of language: free the local arrays */
	sxfree (parse_stack.parsers_set);
	sxfree (parse_stack.transitions);
	XxY_free (&parse_stack.sons);
	XxY_free (&parse_stack.suffixes);
	XxY_free (&parse_stack.ranges);
	XxY_free (&parse_stack.symbols);
	XxY_free (&parse_stack.sf_rhs);
	XxY_free (&parse_stack.sf_rule);
	/* XxY_free (&parse_stack.unit_rules); */

	if (parse_stack.useful_ntset != NULL) {
	    sxfree (parse_stack.useful_ntset), parse_stack.useful_ntset = NULL;
	    sxfree (parse_stack.useful_tset);
	}

	if (parse_stack.rule != NULL) {
	    sxfree (parse_stack.rule);
	    sxfree (parse_stack.grammar);
	    CTRL_FREE (parse_stack.erased);
	    CTRL_FREE (parse_stack.reached);
	}

	sxfree (parse_stack.erasable_sons);
	sxfree (parse_stack.current_links);
	sxfree (parse_stack.used_links);

	sxfree (parse_stack.for_reducer.triples);
	sxfree (parse_stack.for_reducer.refs);
	sxfree (parse_stack.for_reducer.triples_hd);

	XxY_free (&parse_stack.mscans);

	XxY_free (&parse_stack.parsers);
	sxfree (parse_stack.parser_to_attr);

	if (parse_stack.GC_area != NULL)
	    sxfree (parse_stack.GC_area), parse_stack.GC_area = NULL;

	sxfree (parse_stack.red2act);

	sxtkn_mngr (CLOSE, 0);
	break;

    case END:
	(*sxplocals.SXP_tables.recovery) (CLOSE);
	break;

    default:
	fprintf (sxstderr, "The function \"sxndparser\" is out of date with respect to its specification.\n");
	abort ();
	    /*NOTREACHED*/
    }

    return TRUE;
}



BOOLEAN sxnd2parser (what_to_do, arg)
    int		what_to_do;
    struct sxtables	*arg;
{
    switch (what_to_do) {
    case BEGIN:
	sxndparser (BEGIN, arg);
	break;

    case OPEN:
	sxndparser (OPEN, arg);
	sxndtkn_mngr (OPEN, 2);
	break;

    case INIT:
	sxndparser (INIT, arg);
	/* parse_stack.for_scanner.next_hd contient les actions shifts sur le eof initial */
	sxndtkn_mngr (INIT, 0);
	*parse_stack.for_scanner.next_hd = 0;
	parse_stack.is_token_dag = TRUE;
	break;
			  
    case ACTION:
	sxnd2parse_it ();

	if (sxplocals.mode.mode == SXPARSER)
	    sxnddesambig_it ();

	break;

    case FINAL:
	sxndparser (FINAL, arg);
	sxndtkn_mngr (FINAL, 0);
	break;

    case CLOSE:
	sxndparser (CLOSE, arg);
	sxndtkn_mngr (CLOSE, 0);
	break;

    case END:
	sxndparser (END, arg);
	break;

    default:
	fprintf (sxstderr, "The function \"sxnd2parser\" is out of date with respect to its specification.\n");
	abort ();
	    /*NOTREACHED*/
    }

    return TRUE;
}





BOOLEAN sxndparse_in_la (ep_la, Ttok_no, Htok_no, mode_ptr)
    int	ep_la, Ttok_no, *Htok_no;
    struct sxparse_mode		*mode_ptr;
{
    /* Appel recursif du parser pour verifier si le sous-langage defini par
       le point d'entree ep_la contient un prefixe du source. */
    /* Le token caracteristique du point d'entree est memorise en Ttok_no */
    /* L'indice du dernier token analyse par cet appel est range en Htok_no */
    int			i, current_token, current_lahead, current_milestone, parser, scan_ref, xmscan;
    BOOLEAN		ret_val;
    struct sxtoken	old_tt, *ptt;
    struct sxparse_mode	old_mode;
    int			*cfs, *nfs, *fs;

    /* On memorise qq petites choses */
    old_mode = sxplocals.mode;
    current_token = parse_stack.current_token;
    current_lahead = parse_stack.current_lahead;
    current_milestone = parse_stack.current_milestone;

    sxplocals.mode = *mode_ptr;
    old_tt = *(ptt = &(SXGET_TOKEN (Ttok_no)));
    
    /* On change le token courant par le point d'entree */
    ptt->lahead = ep_la;
    ptt->string_table_entry = EMPTY_STE;
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

    create_ref (new_parser (0 /* son */,
			    sxplocals.SXP_tables.init_state,
			    0 /* symb */ ),
		NDP_access (sxplocals.SXP_tables.t_bases + sxplocals.SXP_tables.init_state,
			    parse_stack.current_lahead) /* ref */);

    do {
	parse_stack.current_milestone = parse_stack.current_token = ++Ttok_no;
	parse_stack.current_lahead = sxget_token (Ttok_no)->lahead;
	fs = parse_stack.for_scanner.next_hd;
	parse_stack.for_scanner.current_hd = parse_stack.for_scanner.next_hd;
	parse_stack.for_scanner.next_hd = fs;

	*parse_stack.for_scanner.next_hd = 0;
	
	for (parser = *parse_stack.for_scanner.current_hd;
	     parser > 0;
	     parser = parse_stack.parser_to_attr [parser].next_for_scanner) {
	    scan_ref = parse_stack.parser_to_attr [parser].scan_ref;

	    if (scan_ref < 0) {
		XxY_Xforeach (parse_stack.mscans, parser, xmscan) {
		    sxndsubparse_a_token (parser,
					  XxY_Y (parse_stack.mscans, xmscan),
					  0 /* symbol */,
					  NULL);
		}

		scan_ref = -scan_ref;
	    }

	    sxndsubparse_a_token (parser, scan_ref, 0 /* symbol */, NULL);
	}
	
	if (parse_stack.for_reducer.top > 0)
	    reducer ();
	
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
    parse_stack.current_milestone = current_milestone;
    mode_ptr->local_errors_nb = sxplocals.mode.local_errors_nb;
    mode_ptr->global_errors_nb = sxplocals.mode.global_errors_nb;
    /* L'appelant decide (et le fait) si le nombre d'erreur doit
       etre cumule'. */
    sxplocals.mode = old_mode;

    return ret_val;
}
