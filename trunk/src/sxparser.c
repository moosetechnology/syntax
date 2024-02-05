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

static char	ME [] = "PARSER";

#include "sxversion.h"
#include "sxunix.h"

#ifndef VARIANT_32
char WHAT_SXPARSER[] = "@(#)SYNTAX - $Id: sxparser.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;
#endif

/*   V A R I A B L E S   */

/* Variables which must be kept "local" to each recursive invocation of the
   parser are in the global variable "sxplocals", of type "struct sxplocals".
*/

static SXINT	lgt1, lgt2;
static SXP_SHORT	*stack /* lgt1 */;
static bool	bscan;
static struct gstack {
    SXINT		top, bot;
    SXP_SHORT	*stack /* lgt2 */;
    SXP_SHORT	state;
} ga, gb;

SXINT sxP_get_parse_stack_size (void)
{
    return lgt1;
}

SXP_SHORT		sxP_access (struct SXP_bases *abase, SXINT j)
{
    struct SXP_item	*ajvector, *ajrefvector;
    SXP_SHORT	ref = abase->commun;

    if (ref <= sxplocals.SXP_tables.Mref /* codage direct */ ) {
	if (abase->propre == j /* checked */ )
	    return ref;
	else
	    return abase->defaut;
    }

    if ((ajrefvector = (ajvector = sxplocals.SXP_tables.vector + j) + ref)->check == j)
	/* dans commun */
	return ajrefvector->action;

    if ((ref = abase->propre) != 0 /* propre est non vide */  &&
	(ajrefvector = ajvector + ref)->check == j)
	/* dans propre */
	return ajrefvector->action;

    return abase->defaut;
}

#if BUG
static void print_tok (SXINT tok_no, SXINT xps, struct sxtoken *pt)
{
  fprintf (sxstdout,
	   "%s\t\ttoken [%ld(%ld, %ld)], (xps = %ld) = (%ld, \"%s\")\n" /* & */
	   "\t\t\tste = (%ld, \"%s\")\n" /* & */
	   "\t\t\tsource_index = (%s, %ld, %ld)\n" /* & */
	   "\t\t\tcomment = \"%s\"\n",
	   ME,
	   (SXINT) tok_no,
	   (SXINT) SXTOKEN_PAGE(tok_no),
	   (SXINT) SXTOKEN_INDX(tok_no),
	   xps,
	   pt->lahead,
	   sxttext (sxsvar.sxtables, pt->lahead),
	   pt->string_table_entry,
	   pt->string_table_entry > 1 ? sxstrget (pt->string_table_entry) :
	      (pt->string_table_entry == 1 ? "SXEMPTY_STE" : "SXERROR_STE"),
	   pt->source_index.file_name,
	   (SXUINT) pt->source_index.line,
	   (SXUINT) pt->source_index.column,
	   (pt->comment != NULL) ? pt->comment : "(NULL)");
}

static void print_reduce (SXINT red_no)
{
  fprintf (sxstdout,
	   "%s\t\treduction_no = %ld\n",
	   ME, (SXINT) red_no);
}
#endif

static void	execute_actions (struct gstack *s1)
{
    /* Caracteristique de s1 :
       - elle contient exactement un Scan
       - elle ne contient pas de Scan en LookAhead
       - elle se termine par un Shift
       Apres execution le token du sommet de la parse_stack est donc non significatif */

    SXP_SHORT	x, state, ref;
    struct SXP_reductions	*ared;

    for (x = s1->bot /* bottom */  + 1; x <= s1->top; x++) {
	if ((ref = s1->stack [x]) > 0) {
	    /* Scan */
	    sxpglobals.parse_stack [sxpglobals.xps].token = SXGET_TOKEN (sxplocals.atok_no);

#if BUG
	    print_tok (sxplocals.atok_no, sxpglobals.xps, &(SXGET_TOKEN (sxplocals.atok_no)));
#endif
	    
	    sxplocals.atok_no++;
	}
	else {
	    /* NoScan */
	    ref = -ref;
	}

	if ((state = ref - sxplocals.SXP_tables.Mprdct) > 0 /* Shift */ ) {
	    sxpglobals.parse_stack [++sxpglobals.xps].state = state;
	}
	else {
	    /* Reduce or Halt */
	    if ((sxpglobals.reduce =
		  (ared = sxplocals.SXP_tables.reductions + ref)->reduce) != 0 /* Reduce */ ) {
	        bool is_semact = 
		   sxpglobals.reduce <= sxplocals.SXP_tables.P_nbpro && sxis_semantic_action (ared->action);

#if BUG
		print_reduce (sxpglobals.reduce);
#endif

	        sxpglobals.pspl = ared->lgth;

	        if (ref > sxplocals.SXP_tables.Mrednt && s1->stack [x] <= 0) {
		    /* Reduce dans les T_tables sans Scan */
		    if (sxpglobals.pspl-- == 0) {
		        /* production vide */
		        sxpglobals.parse_stack [sxpglobals.xps].token.source_index =
			    SXGET_TOKEN (sxplocals.atok_no).source_index;
                    }
		    sxpglobals.xps--;
	        }

	        if (is_semact) {
		    if (sxplocals.mode.with_semact)
		        (*sxplocals.SXP_tables.semact) (SXACTION, ared->action);
	        }
                else {
		    if (sxplocals.mode.with_parsact)
		        (*sxplocals.SXP_tables.parsact) (SXACTION, ared->action);
	        }

	        sxpglobals.xps -= sxpglobals.pspl;
	        sxpglobals.parse_stack [sxpglobals.xps].token.lahead =
		    is_semact ? 0 /* nt */  : -1 /* action */ ;
            }
	}
    }

    if (sxplocals.mode.look_back != 0)
	/* Ces tokens deviennent inaccessibles. */
	sxplocals.left_border = sxplocals.atok_no - sxplocals.mode.look_back;

    s1->top = s1->bot /* top = bottom, empty the stack */ ;
}


static SXINT undo_actions (struct gstack *s1, SXINT xs)
{
    /* On doit "defaire" les actions eventuelles de s1, en ordre inverse de
       leur execution en assurant la coherence des indices de pile d'analyse. */

    SXP_SHORT	x, ref;
    struct SXP_reductions	*ared;

    for (x = s1->top; x > s1->bot; x--) {
	if ((ref = s1->stack [x]) != 0) {
	    if (ref > 0)
		/* Scan undo */
		/* --sxplocals.ptok_no */;
	    else
		/* NoScan */
		ref = -ref;

	    if ((ref - sxplocals.SXP_tables.Mprdct) > 0 /* Shift */ )
		/* pop */
		--xs;
	    else {
		/* Reduce */
		ared = sxplocals.SXP_tables.reductions + ref;
		xs += ared->lgth;

		if (ared->reduce > sxplocals.SXP_tables.P_nbpro || sxis_syntactic_action (ared->action)) {
		    SXINT old_xps = sxpglobals.xps;

		    sxpglobals.reduce = ared->reduce;
		    sxpglobals.pspl = ared->lgth; /* 0 */
		    sxpglobals.xps = xs;

		    if (ref > sxplocals.SXP_tables.Mrednt && s1->stack [x] <= 0) {
			/* Reduce dans les T_tables sans Scan */
			sxpglobals.pspl--;
			sxpglobals.xps--;
		    }

		    (*sxplocals.SXP_tables.parsact) (SXUNDO, ared->action);

		    sxpglobals.xps = old_xps;
		}
	    }
	}
    }

    s1->top = s1->bot /* top = bottom, empty the stack */ ;

    return xs;
}



static void	sxpsature (SXINT nbt)

/* overflow of tables for normal analysis */
    

{
    switch (nbt) {
    case 1:
	lgt1 *= 2;
	sxpglobals.parse_stack = (struct sxparstack*) sxrealloc (sxpglobals.parse_stack, lgt1 + 1,
								 sizeof (struct sxparstack));
	stack = (SXP_SHORT*) sxrealloc (stack, lgt1 + 1, sizeof (SXP_SHORT));
	break;

    case 2:
	lgt2 *= 2;
	ga.stack = (SXP_SHORT*) sxrealloc (ga.stack, lgt2 + 1, sizeof (SXP_SHORT));
	gb.stack = (SXP_SHORT*) sxrealloc (gb.stack, lgt2 + 1, sizeof (SXP_SHORT));
	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap(ME,"unknown switch case #1");
#endif
      break;
    }
}

SXP_SHORT	ARC_traversal (SXP_SHORT ref, SXINT latok_no)
{
    SXP_SHORT	next;
    SXP_SHORT		t_state;

    /* First scan in LookAhead */
    next = ref + sxplocals.SXP_tables.Mref /* next < 0 */;

    do {
	ref = sxP_access (sxplocals.SXP_tables.t_bases + (t_state = -next),
			sxget_token (++latok_no)->lahead);

	if (ref < -sxplocals.SXP_tables.Mred && ref >= -sxplocals.SXP_tables.Mprdct) {
	    /* Execution d'un predicat en look-ahead */
	    struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts - ref;
	    SXINT ptok_no;

	    ptok_no = sxplocals.ptok_no;
	    sxplocals.ptok_no = latok_no;

	    while (aprdct->prdct >= 0 /* User's predicate */  &&
		   (!sxplocals.mode.with_parsprdct ||
		    !(*sxplocals.SXP_tables.parsact) (SXPREDICATE, aprdct->prdct)))
		/* returning False */ 
		aprdct++;

	    ref = aprdct->action;
	    sxplocals.ptok_no = ptok_no;
	}
    } while ((next = ref + sxplocals.SXP_tables.Mref) < 0);

    /* LookAhead ends */
    if (ref == 0) {
	/* Error in LookAhead */
	(*sxplocals.SXP_tables.recovery) (SXERROR, &t_state, latok_no);
	ref = t_state;
    }

    return ref;
}


/*
Afin de permettre des corrections d'erreur sur l'unite' lexicale
(t0) precedent celle (t1) sur laquelle une erreur a ete detectee et
cela sans avoir a defaire le resultat des actions indument executees,
l'execution des actions (syntaxiques et semantiques) est "retardee"
par rapport a l'analyse syntaxique proprement dite.  Cet effet est
obtenu en utilisant deux piles s1 et s2.  s1 est la pile des actions
en retard et s2 est la pile des actions courantes.

Les actions de s1 sont executees lorsque l'instruction qui vient
d'etre empilee dans s2 est un scan ( t1 * () ...  ou t1 * red p ...).
On est alors certain que le terminal t1, teste par cette instruction,
est correct -- aucune erreur ne sera detectee ici--, le terminal t0 ne
sera donc jamais modifie par une correction d'erreur eventuelle et les
actions associees contenues dans s1 peuvent donc s'executer.

On echange les piles s1 et s2 (s1 doit etre vide) lorsque la derniere
instruction empilee dans s2 est le premier shift suivant le scan de
s2.  On est sur qu'entre ces deux instructions:
  - aucun test explicite du terminal scanne ne s'est produit
   - il n'y a aucune parsact

Apres echange, l'etat atteint par l'instruction shift est stocke en
s2[-1], ce sera l'etat utilise par la recuperation d'erreur.  Une pile
si contient donc exactement une et une seule instruction scan, se
termine par une instruction shift et l'instruction de fond de pile est
une Action de l'etat si[-1].  Le terminal associe a la pile s2 est
celui qui a ete lu par l'instruction scan de la pile s1.

Cependant, afin de "rapprocher" les parsact eventuelles des predicats
pouvant les utiliser, on execute la pile en retard a la vue d'un
terminal etendu (le terminal associe est le symbol courant du source).
La verification n'etant pas complete, si une erreur est detectee par
les predicats associes, la correction correspondante ne peut toucher a
t0, il faut donc le signaler a l'error_recovery.  Lors de la detection
d'erreur, si la pile en retard s1 "contient" un predicat &i, elle est
egalement executee et aucune correction d'erreur ne sera tentee sur
t0.  La raison en est que nous ne sommes pas sur que l'execution
passee de &i, son execution au cours du rattrapage d'erreur et sa
future execution apres rattrapage donnent le meme resultat.
*/

static bool	sxparse_it (void)
{
    SXP_SHORT			ref;
    SXINT                       lahead;
    SXP_SHORT                   state;
    SXINT			xs;
    struct SXP_reductions	*ared;
    struct gstack 	*s1 = &ga, *s2 = &gb;

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

/* ******************************************************************** */

/* initializations */

    xs = sxpglobals.xps = sxpglobals.stack_bot;
    stack [xs] = 0;
    sxpglobals.parse_stack [xs].state = 0;
    lahead = sxplocals.SXP_tables.P_tmax;
    sxplocals.state = sxplocals.SXP_tables.init_state;
    state = (SXP_SHORT) sxplocals.state;

/* (*(sxplocals.SXP_tables.semact)) (SXINIT, lgt1); */

restart:
    ref = sxP_access (sxplocals.SXP_tables.t_bases + (s2->state = state), lahead);
    s1->top = s1->bot /* top = bottom, empty the stacks */ ;
    s2->top = s2->bot;
    bscan = false;


/* Swap de s1 et s2 : premiere etiquette apres scan
   Evaluation de s1 : premier test explicite */

    for (;;) {
	if (ref < -sxplocals.SXP_tables.Mref)
	    ref = ARC_traversal (ref, sxplocals.ptok_no);

	/* memorisation de l'action courante */
	if (++s2->top > lgt2) {
	    sxpsature ((SXINT)2);
	}

	s2->stack [s2->top] = ref;

	if (ref > 0 /* scan */ ) {
	    SXINT n;

	    if (s1->bot != s1->top)
		execute_actions (s1);

	    n = ++sxplocals.ptok_no;

	    while (n > sxplocals.Mtok_no)
		(*(sxplocals.SXP_tables.scanit)) ();

	    lahead = SXGET_TOKEN (n).lahead;
	    bscan = true;
	}
	else
	    ref = -ref;

	if ((state = ref - sxplocals.SXP_tables.Mprdct) > 0 /* Shift */ ) {
	    if (++xs > lgt1)
		sxpsature ((SXINT)1);

	    if (state <= sxplocals.SXP_tables.M0ref)
		stack [xs] = state;
	    /* else empile 0 */

	    if (bscan) {
		struct gstack	*pp;

		bscan = false;
		/* swap s1 <-> s2, s1 doit etre vide */
		pp = s1, s1 = s2, s2 = pp;
		s2->state = state;
	    }

	    ref = sxP_access (sxplocals.SXP_tables.t_bases + state, (SXINT)lahead);
	}
	else if (ref <= sxplocals.SXP_tables.Mred /* Reduce, Error or Halt */ ) {
	    if (ref == 0 /* error recovery */ ) {
	      if (sxplocals.mode.is_prefixe && state == (SXP_SHORT) sxplocals.SXP_tables.final_state)
		    return true;

		sxplocals.mode.local_errors_nb++;

		if (sxplocals.mode.kind == SXWITHOUT_ERROR)
		    return false;

		if (sxplocals.mode.with_do_undo && sxplocals.mode.with_parsact)
		{
		    /* Ds le cas SXDO/SXUNDO, on essaie une correction sur le token
		       precedent. */
		    /* Il faut "defaire" les parsact eventuelles executees ds s2 et s1. */
		    if (s2->bot != s2->top)
			xs = undo_actions (s2, xs);

		    if (s1->bot != s1->top)
			xs = undo_actions (s1, xs);
		}
		else
		{
		    if (s1->bot != s1->top && s1->state < 0) /* Predicat dans s1 non vide */
			/* On ne touche pas au token precedent */
			execute_actions (s1);
		}
		
		sxplocals.state = (SXINT) ((sxplocals.ptok_no == sxplocals.atok_no) ? labs ((SXINT) s2->state) : s1->state);

		if ((*sxplocals.SXP_tables.recovery) (SXACTION)) {
		    /* Succes */
		    SXINT	i;
		    
		    xs = sxpglobals.xps;
		    
		    for (i = sxpglobals.stack_bot + 1; i <= xs; i++) {
			/* stack [stack_bot] == 0 */
			/* restauration de la pile */
			stack [i] = sxpglobals.parse_stack [i].state;
		    }
		    
		    lahead = SXGET_TOKEN (sxplocals.ptok_no).lahead;
		    state = (SXP_SHORT) sxplocals.state;
		    goto restart;
		}
		
		/* Failure */
		return false;
	    }

/* Reduce or Halt */

	    if ((ared = sxplocals.SXP_tables.reductions + ref)->reduce == 0 /* Halt */ ) {
		if (s1->bot != s1->top)
		    /* On vide s1 */
		    execute_actions (s1);

		if (s2->bot != s2->top) {
		    /* puis s2 */
		    execute_actions (s2);
		}

		break;
	    }

	    {
		/* Reduce */
		SXP_SHORT	nt;

#if BUG
		print_reduce (ared->reduce);
#endif

		if ((ared->reduce > sxplocals.SXP_tables.P_nbpro || sxis_syntactic_action (ared->action)) &&
		    sxplocals.mode.with_do_undo &&
		    sxplocals.mode.with_parsact)
		{
		    /* parsact a executer immediatement */
		    /* Ca permet aux parsact de manipuler une structure // a stack en
		       utilisant les macros SXSTACKtop(), SXSTACKnewtop() et SXSTACKreduce(). */
		    SXINT old_xps = sxpglobals.xps;

		    sxpglobals.reduce = ared->reduce;
		    sxpglobals.pspl = ared->lgth; /* 0 */
		    sxpglobals.xps = xs;

		    if (ref > sxplocals.SXP_tables.Mrednt && s2->stack [s2->top] <= 0) {
			/* Reduce dans les T_tables sans Scan */
			sxpglobals.pspl--;
			sxpglobals.xps--;
		    }

		    (*sxplocals.SXP_tables.parsact) (SXDO, ared->action);

		    sxpglobals.xps = old_xps;
		}

		xs -= ared->lgth;

		if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
		    /* pas branchement direct */
		    state = stack [xs];
		    ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, (SXINT)nt); 
		}
	    }
	}
	else {
	    /* Predicates */
	    struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts + ref;
	    SXINT old_xps;

	    if (!sxplocals.mode.with_do_undo && s1->bot != s1->top)
		execute_actions (s1);

	    --s2->top /* On ecrase le predicat */ ;

	    /* Au cas ou le predicat est associe' a un nt et il veut re'fe'rencer
	       une structure // a` stack. Il faut utiliser SXSTACKtop () */
	    old_xps = sxpglobals.xps;
	    sxpglobals.xps = xs; 

	    while (aprdct->prdct >= 0 /* User's predicate */  &&
		   (!sxplocals.mode.with_parsprdct ||
		    !(*sxplocals.SXP_tables.parsact) (SXPREDICATE, aprdct->prdct)))
		/* returning False */ 
		aprdct++;

	    sxpglobals.xps = old_xps;

	    ref = aprdct->action;

	    if (s2->state > 0)
		s2->state = -s2->state /* On note la presence de predicats ds s2 */;
	}
    }

    sxtkn_mngr (SXFINAL, 0);
    return true;
}



bool sxparser (SXINT	what_to_do, struct sxtables *arg)
{
    switch (what_to_do) {
    case SXBEGIN:

/* global initialization */

	lgt1 = 200;
	lgt2 = 50;

/* allocate arrays for normal analysis */

	sxpglobals.parse_stack = (struct sxparstack*) sxalloc (lgt1 + 1,
							       sizeof (struct sxparstack));
	stack = (SXP_SHORT*) sxalloc (lgt1 + 1, sizeof (SXP_SHORT));
	ga.stack = (SXP_SHORT*) sxalloc (lgt2 + 1, sizeof (SXP_SHORT));
	gb.stack = (SXP_SHORT*) sxalloc (lgt2 + 1, sizeof (SXP_SHORT));
	sxpglobals.xps = 0;
	sxpglobals.stack_bot = 0;
	ga.top = ga.bot = gb.top = gb.bot = 1; /* top, bottom  = 1; */ 
	break;

    case SXOPEN:
	/* new language: new tables, new local parser variables */
	/* the global variable "sxplocals" must have been saved in case */
	/* of recursive invocation */

	/* test arg->magic for consistency */
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, ME);

/* prepare new local variables */

	sxplocals.sxtables = arg;
	sxplocals.SXP_tables = arg->SXP_tables;
	sxtkn_mngr (SXOPEN, 2);
	(*sxplocals.SXP_tables.recovery) (SXOPEN);
	break;

    case SXINIT:
	/* on initialise toks_buf avec "EOF" */

    {
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
	/* valeurs par defaut qui peut etre changee ds les
	   scan_act ou pars_act. */
	sxplocals.mode.look_back = 1; 
	sxplocals.mode.mode = SXPARSER;
	sxplocals.mode.kind = SXWITH_RECOVERY;
	sxplocals.mode.local_errors_nb = 0;
	sxplocals.mode.global_errors_nb = 0;
	sxplocals.mode.is_prefixe = false;
	sxplocals.mode.is_silent = false;
	sxplocals.mode.with_semact = true;
	sxplocals.mode.with_parsact = true;
	sxplocals.mode.with_parsprdct = true;
	sxplocals.mode.with_do_undo = false;
	break;

    case SXACTION:
	{
	    /* new file or portion of file */
	    /* arg is pointer to tables, but "sxplocals" should be allright */

	    SXINT       old_stack_bot = sxpglobals.stack_bot;
	    SXP_SHORT	old_ga_state = ga.state, old_gb_state = gb.state;
	    SXINT		old_ga_bot = ga.bot, old_ga_top = ga.top, old_gb_bot = gb.bot, old_gb_top = gb.top;
	    SXINT	reduce = sxpglobals.reduce;
	    SXINT	pspl = sxpglobals.pspl;
	    bool	old_bscan = bscan;
	    bool	ret_val;

	    if ((sxpglobals.stack_bot = sxpglobals.xps + 5) > lgt1)
		sxpsature ((SXINT)1);

	    ga.bot = ga.top;
	    gb.bot = gb.top;

	    ret_val = sxparse_it ();

	    sxpglobals.reduce = reduce;
	    sxpglobals.pspl = pspl;
	    sxpglobals.xps = sxpglobals.stack_bot - 5;
	    sxpglobals.stack_bot = old_stack_bot;
	    bscan = old_bscan;
	    ga.bot = old_ga_bot;
	    ga.top = old_ga_top;
	    ga.state = old_ga_state;
	    gb.bot = old_gb_bot;
	    gb.top = old_gb_top;
	    gb.state = old_gb_state;
	    return ret_val;
	}

    case SXFINAL:
	sxtkn_mngr (SXFINAL, 0);
	break;

    case SXCLOSE:
	/* end of language: free the local arrays */
	sxtkn_mngr (SXCLOSE, 0);
	(*sxplocals.SXP_tables.recovery) (SXCLOSE);
	break;

    case SXEND:
	/* free everything */
	sxfree (gb.stack), gb.stack = NULL;
	sxfree (ga.stack), ga.stack = NULL;
	sxfree (stack), stack = NULL;
	sxfree (sxpglobals.parse_stack), sxpglobals.parse_stack = NULL;
	break;

    default:
	fprintf (sxstderr, "The function \"sxparser\" is out of date with respect to its specification because \"what_to_do\" has value %ld = 0x%lx\n", (SXINT) what_to_do, (SXINT) what_to_do);
	sxexit(1);
	    /*NOTREACHED*/
    }

    return true;
}


bool sxparse_in_la (SXINT ep_la, SXINT Ttok_no, SXINT *Htok_no, struct sxparse_mode *mode_ptr)
{
    /* Appel recursif du parser pour verifier si le sous-langage defini par
       le point d'entree ep_la contient un prefixe du source. */
    /* Le token caracteristique du point d'entree est memorise en Ttok_no */
    /* L'indice du dernier token analyse par cet appel est range en Htok_no */
    SXINT		atok_no, ptok_no, old_left_border;
    bool			ret_val;
    struct sxtoken		old_tt, *ptt;
    struct sxparse_mode		old_mode;

    /* On memorise qq petites choses */
    atok_no = sxplocals.atok_no;
    ptok_no = sxplocals.ptok_no;
    old_mode = sxplocals.mode;
    old_left_border = sxplocals.left_border;
    sxplocals.mode = *mode_ptr;
    /* On ne touche pas a left-border, une recuperation de place peut donc survenir
       pendant le look-ahead, mais ds ce cas il recuperera de la place avant l'indice
       de depart. */
    old_tt = *(ptt = &(SXGET_TOKEN (Ttok_no)));
    
    /* On change le token courant par le point d'entree */
    ptt->lahead = ep_la;
    ptt->string_table_entry = SXEMPTY_STE;
    /* On pointe sur le token precedent (normalement non acce'de') */
    sxplocals.atok_no = sxplocals.ptok_no = Ttok_no - 1;
    
    /* appel recursif */
    ret_val = (*(sxplocals.sxtables->analyzers.parser)) (SXACTION, sxplocals.sxtables);
    
    /* dernier token lu par l'appel precedent. */
    *Htok_no = sxplocals.ptok_no;

    /* On remet en place */
    *ptt = old_tt;
    sxplocals.atok_no = atok_no;
    sxplocals.ptok_no = ptok_no;
    mode_ptr->local_errors_nb = sxplocals.mode.local_errors_nb;
    mode_ptr->global_errors_nb = sxplocals.mode.global_errors_nb;
    /* L'appelant decide (et le fait) si le nombre d'erreur doit
       etre cumule'. */
    sxplocals.mode = old_mode;
    sxplocals.left_border = old_left_border;

    return ret_val;
}
