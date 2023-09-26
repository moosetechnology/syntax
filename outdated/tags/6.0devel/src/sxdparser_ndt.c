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
/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 





/* Cet analyseur utilise des tables non-de'terministes construites par le
   RLR ou le Left_Corner et produit une analyse unique.  Si le langage est
   de'terministe, c'est la bonne, si la phrase est ambigue, c'est l'une des
   analyses.  Peut e^tre modifie' pour trouver toutes les analyses. */

/* a ete realise a partir du parser deterministe de la version 5.0 */



static char	ME [] = "PARSER";

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXDPARSER_NDT[] = "@(#)SYNTAX - $Id: sxdparser_ndt.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

/*   V A R I A B L E S   */

/* Variables which must be kept "local" to each recursive invocation of the
   parser are in the global variable "sxplocals", of type "struct sxplocals".
*/

static SXINT		lgt1, lgt2, max_la_tok_no;
static SXP_SHORT	*la_ref_stack;

#if EBUG
SXINT	    		nd_nb;
#endif

#define POP(t)		t[t[0]--]
#define PUSH(t,x)	((++t[0]>t[-1]) ? (SXP_SHORT*) sxrealloc (t-1, (t[-1] *= 2)+2, sizeof(SXP_SHORT))+1 : 0),t[t[0]]=x


static SXP_SHORT
sxP_access (struct SXP_bases *abase, SXP_SHORT j)
{
    struct SXP_item	*ajvector, *ajrefvector;
    SXP_SHORT		ref = abase->commun;

    if (ref <= sxplocals.SXP_tables.Mref /* codage direct */ ) {
	if (abase->propre == j /* checked */ )
	    return ref;
	else
	    return abase->defaut;
    }

    if ((ajrefvector = (ajvector = sxplocals.SXP_tables.vector + j) + ref)->check == j)
	/* dans commun */
	return ajrefvector->action;

    if ((ref = abase->propre) != 0 /* propre est non vide */  && (ajrefvector = ajvector + ref)->check == j)
	/* dans propre */
	return ajrefvector->action;

    return abase->defaut;
}

#if 0
/* semble ne rien gagner... */
#define P_ACCESS(abase,j)									\
     (abase->commun <= sxplocals.SXP_tables.Mref /* codage direct */ )				\
        ? ((abase->propre == j /* checked */ )							\
            ? abase->commun									\
	    : abase->defaut)									\
	: (((ajrefvector = (ajvector = sxplocals.SXP_tables.vector + j)				\
	     + abase->commun)->check == j)							\
	    ? ajrefvector->action								\
	    : ((abase->propre != 0 /* propre est non vide */					\
		&& (ajrefvector = ajvector + abase->propre)->check == j)			\
	       ? ajrefvector->action								\
	       : abase->defaut))
#endif

static SXVOID	sxpsature (void)
{
    sxpglobals.parse_stack = (struct sxparstack*) sxrealloc (sxpglobals.parse_stack,
							     (lgt1 *= 2) + 1,
							     sizeof (struct sxparstack));

}

#if 0
/* a mettre si on utilise le constructeur RLR */
SXINT	ARC_traversal (ref, latok_no)
    SXINT	ref;
    SXINT	latok_no;
{
    SXINT	next;
    SXINT		t_state;

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
#endif


static SXBOOLEAN
la_recognizer (SXINT xps, SXINT tok_no, SXP_SHORT ref)
{
    struct SXP_reductions	*ared;
    struct SXP_prdct		*aprdct;
    SXP_SHORT			nt, prev_state, state;
    SXINT			*pstack, lahead;
    SXBOOLEAN			ret_val;
#if 0
    struct SXP_bases		*abase;
    struct SXP_item		*ajvector, *ajrefvector; /* pour P_ACCESS() */
#endif

#if 0
    if (ref < -sxplocals.SXP_tables.Mref)
	ref = ARC_traversal (ref, tok_no);
#endif

    if (ref > 0) {
	/* scan */
	tok_no++;

	if (tok_no > max_la_tok_no)
	     max_la_tok_no = tok_no;

	while (tok_no > sxplocals.Mtok_no)
	    (*(sxplocals.SXP_tables.scanit)) ();
    }
    else
	ref = -ref;

    lahead = SXGET_TOKEN (tok_no).lahead;
  
    if ((state = ref - sxplocals.SXP_tables.Mprdct) > 0) {
	/* Shift */
	if (++xps > lgt1)
	    sxpsature ();

	pstack = &(sxpglobals.parse_stack [xps].state);
	prev_state = *pstack;

	if (state <= sxplocals.SXP_tables.M0ref)
	    *pstack = state;
	/* else empile 0 */

	ref = sxP_access (sxplocals.SXP_tables.t_bases + state, lahead);
#if 0
	abase = sxplocals.SXP_tables.t_bases + state;
	ref = P_ACCESS (abase, lahead);
#endif

	ret_val = la_recognizer (xps, tok_no, ref);
	*pstack = prev_state;

	return ret_val;
    }

    if (ref <= sxplocals.SXP_tables.Mred /* Reduce, Error or Halt */ ) {
	if (ref == 0)
	    /* error */
	    return SXFALSE;

	if ((ared = sxplocals.SXP_tables.reductions + ref)->reduce == 0)
	    /* Halt */
	    return SXTRUE;

	/* Reduce */
	xps -= ared->lgth;

	if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
	    /* pas branchement direct */
	    state = sxpglobals.parse_stack [xps].state;
	    ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, nt); 
#if 0
	    abase = sxplocals.SXP_tables.nt_bases + state;
	    ref = P_ACCESS (abase, nt);
#endif
	}

	return la_recognizer (xps, tok_no, ref);
    }

    /* Predicates or non-determinism */
    aprdct = sxplocals.SXP_tables.prdcts + ref;

    if (aprdct->prdct >= 20000) {
	/* non-determinism */
	/* On est en look_ahead */
	for (;;) {
#if EBUG
	    nd_nb++;
#endif
	    if (aprdct->action && la_recognizer (xps, tok_no, aprdct->action)){
		PUSH (la_ref_stack, ref);
		return SXTRUE;
	    }

	    if (aprdct->prdct == 20000)
		return SXFALSE;
		    
	    aprdct++;
	    ref++;
	}
    }

    /* Predicates */
    while (aprdct->prdct >= 0 /* User's predicate */  &&
	   (!sxplocals.mode.with_parsprdct ||
	    !(*sxplocals.SXP_tables.parsact) (SXPREDICATE, aprdct->prdct)))
	/* returning False */
	aprdct++;

    return la_recognizer (xps, tok_no, aprdct->action);
}




static SXBOOLEAN	sxparse_it (void)
{
    SXP_SHORT			ref, lahead, nt, state;
    struct SXP_reductions	*ared;
    struct SXP_prdct		*aprdct;
    SXBOOLEAN			is_semact, is_scan;
    SXINT 			n;
#if 0
    struct SXP_bases		*abase;
    struct SXP_item		*ajvector, *ajrefvector; /* pour P_ACCESS() */
#endif


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

    sxpglobals.xps = sxpglobals.stack_bot;
    sxpglobals.parse_stack [sxpglobals.xps].state = 0;
    la_ref_stack [0 /* top */] = 0;
    max_la_tok_no = 0;
    lahead = sxplocals.SXP_tables.P_tmax;
    sxplocals.state = sxplocals.SXP_tables.init_state;
    state = (SXP_SHORT) sxplocals.state;

/* (*(sxplocals.SXP_tables.semact)) (SXINIT, lgt1); */

restart:
    ref = sxP_access (sxplocals.SXP_tables.t_bases + state, lahead);

    for (;;) {
#if 0
	if (ref < -sxplocals.SXP_tables.Mref)
	    ref = ARC_traversal (ref, sxplocals.ptok_no);
#endif

	if (ref > 0 /* scan */ ) {
	    if (sxplocals.mode.look_back != 0)
		/* Ces tokens deviennent inaccessibles. */
		sxplocals.left_border = sxplocals.atok_no - sxplocals.mode.look_back;

	    sxpglobals.parse_stack [sxpglobals.xps].token = SXGET_TOKEN (sxplocals.atok_no);
	    sxplocals.atok_no++;

	    n = ++sxplocals.ptok_no;

	    while (n > sxplocals.Mtok_no)
		(*(sxplocals.SXP_tables.scanit)) ();

	    lahead = SXGET_TOKEN (n).lahead;
	    is_scan = SXTRUE;
	}
	else {
	    ref = -ref;
	    is_scan = SXFALSE;
	}

	if ((state = ref - sxplocals.SXP_tables.Mprdct) > 0) {
	    /* Shift */
	    if (++sxpglobals.xps > lgt1)
		sxpsature ();

	    if (state <= sxplocals.SXP_tables.M0ref)
		sxpglobals.parse_stack [sxpglobals.xps].state = state;
	    /* else empile 0 */

	    ref = sxP_access (sxplocals.SXP_tables.t_bases + state, lahead);
#if 0
	    abase = sxplocals.SXP_tables.t_bases + state;
	    ref = P_ACCESS (abase, lahead);
#endif
	}
	else if (ref <= sxplocals.SXP_tables.Mred) {
	    /* Reduce, Error or Halt */
	    if (ref == 0) {
		/* Error : no error recovery */
		return SXFALSE;
	    }
	    
	    if ((sxpglobals.reduce = (ared = sxplocals.SXP_tables.reductions + ref)->reduce) == 0) {
		/* Halt */
		break;
	    }

	    /* Reduce */
	    sxpglobals.reduce = ared->reduce;
	    sxpglobals.pspl = ared->lgth;
	    is_semact = sxpglobals.reduce <= sxplocals.SXP_tables.P_nbpro && sxis_semantic_action (ared->action);

	    if (!is_semact && sxplocals.mode.with_do_undo && sxplocals.mode.with_parsact) {
		/* parsact a executer immediatement */
		/* Ca permet aux parsact de manipuler une structure // a stack en
		   utilisant les macros SXSTACKtop(), SXSTACKnewtop() et SXSTACKreduce(). */
		if (ref > sxplocals.SXP_tables.Mrednt && !is_scan) {
		    /* Reduce dans les T_tables sans Scan */
		    sxpglobals.pspl--;
		    sxpglobals.xps--;
		    (*sxplocals.SXP_tables.parsact) (SXDO, ared->action);
		    sxpglobals.pspl++;
		    sxpglobals.xps++;
		}
		else
		    (*sxplocals.SXP_tables.parsact) (SXDO, ared->action);
	    }

	    if (ref > sxplocals.SXP_tables.Mrednt && !is_scan) {
		/* Reduce dans les T_tables sans Scan */

		if (sxpglobals.pspl-- == 0)
		    /* production vide */
		    sxpglobals.parse_stack [sxpglobals.xps].token.source_index = SXGET_TOKEN (sxplocals.atok_no).source_index;

		sxpglobals.xps--;
	    }

	    if (is_semact) {
		if (sxplocals.mode.with_semact)
		    sxplocals.SXP_tables.semact (SXACTION, ared->action);
	    }
	    else
		if (sxplocals.mode.with_parsact)
		    sxplocals.SXP_tables.parsact (SXACTION, ared->action);

	    sxpglobals.xps -= sxpglobals.pspl;
	    sxpglobals.parse_stack [sxpglobals.xps].token.lahead = is_semact ? 0 /* nt */  : -1 /* action */ ;

	    if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
		/* pas branchement direct */
		state = sxpglobals.parse_stack [sxpglobals.xps].state;
		ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, nt); 
#if 0
		abase = sxplocals.SXP_tables.nt_bases + state;
		ref = P_ACCESS (abase, nt);
#endif
	    }
	}
	else {
	    /* Predicates or non-determinism */
	    aprdct = sxplocals.SXP_tables.prdcts + ref;

	    if (aprdct->prdct >= 20000) {
		/* non-determinism */
		if (la_ref_stack [0] == 0) {
		    /* On ne connait pas le chemin de look_ahead, on lance le calcul */
		    for (;;) {
#if EBUG
			nd_nb++;
#endif
			if (aprdct->action && la_recognizer (sxpglobals.xps, sxplocals.ptok_no, aprdct->action))
			    break;

			if (aprdct->prdct == 20000) {
			    /* echec total => error */
			    sxplocals.atok_no = max_la_tok_no;
			    return SXFALSE;
			}
		    
			aprdct++;
		    }
		}
		else {
		    aprdct = sxplocals.SXP_tables.prdcts + POP (la_ref_stack);
		}
	    }
	    else {
		/* Predicates */
		while (aprdct->prdct >= 0 /* User's predicate */  &&
		       (!sxplocals.mode.with_parsprdct ||
			!(*sxplocals.SXP_tables.parsact) (SXPREDICATE, aprdct->prdct)))
		    /* returning False */ 
		    aprdct++;

	    }

	    ref = aprdct->action;
	}
    }

    return SXTRUE;
}


static
error_message (void)
{
    if (!sxplocals.mode.is_silent)
	sxerror (SXGET_TOKEN (sxplocals.atok_no).source_index,
		 sxplocals.sxtables->err_titles [2][0],
		 "%ssyntax error.",
		 sxplocals.sxtables->err_titles [2]+1);
}


SXBOOLEAN sxdparser_ndt (SXINT what_to_do, struct sxtables *arg)
{
    switch (what_to_do) {
    case SXBEGIN:

/* global initialization */

	lgt1 = 200;
	lgt2 = 50;

/* allocate arrays for normal analysis */

	sxpglobals.parse_stack = (struct sxparstack*) sxalloc (lgt1 + 1, sizeof (struct sxparstack));
	sxpglobals.xps = 0;
	sxpglobals.stack_bot = 0;
	la_ref_stack = (SXP_SHORT*) sxalloc (lgt2+2, sizeof (SXP_SHORT))+1;
	la_ref_stack [-1 /* size */] = lgt2;
	return SXTRUE;

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
	return SXTRUE;

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
	/* valeurs par defaut qui peut etre changee ds les scan_act ou pars_act. */
	sxplocals.mode.look_back = 0; 
	sxplocals.mode.mode = SXPARSER;
	sxplocals.mode.kind = SXWITH_RECOVERY;
	sxplocals.mode.local_errors_nb = 0;
	sxplocals.mode.global_errors_nb = 0;
	sxplocals.mode.is_prefixe = SXFALSE;
	sxplocals.mode.is_silent = SXFALSE;
	sxplocals.mode.with_semact = SXTRUE;
	sxplocals.mode.with_parsact = SXTRUE;
	sxplocals.mode.with_parsprdct = SXTRUE;
	sxplocals.mode.with_do_undo = SXFALSE;
	return SXTRUE;

    case SXACTION:
	{
	    /* new file or portion of file */
	    /* arg is pointer to tables, but "sxplocals" should be allright */

	    SXP_SHORT	old_stack_bot = sxpglobals.stack_bot;
	    SXP_SHORT	reduce = sxpglobals.reduce;
	    SXP_SHORT	pspl = sxpglobals.pspl;
	    SXBOOLEAN	ret_val = SXTRUE;
	    SXINT		lahead;

	    if ((sxpglobals.stack_bot = sxpglobals.xps + 5) > lgt1)
		sxpsature ();

#if TCUT
	    /* L'option de compilation -DTCUT=t_code permet de couper l'analyse d'un source
	       en morceaux separes par t_code */
	    do {

		do {
		    (*(sxplocals.SXP_tables.scanit)) ();
		    lahead = SXGET_TOKEN (sxplocals.Mtok_no).lahead;
		} while (lahead != sxsvar.SXS_tables.S_termax && lahead != TCUT);

		if (lahead == TCUT)
		    SXGET_TOKEN (sxplocals.Mtok_no).lahead = sxsvar.SXS_tables.S_termax;

		if (!sxparse_it ()) {
		    error_message ();
		    ret_val = SXFALSE;
		}
			
		if (lahead == TCUT)
		    sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;
	    } while (lahead != sxsvar.SXS_tables.S_termax);


#else
	    if (!sxparse_it ()) {
		error_message ();
		ret_val = SXFALSE;
	    }
#endif

#if EBUG
	    fprintf (sxstdout, "Non determinism degree = %i.\n", nd_nb);
#endif

	    sxpglobals.reduce = reduce;
	    sxpglobals.pspl = pspl;
	    sxpglobals.xps = sxpglobals.stack_bot - 5;
	    sxpglobals.stack_bot = old_stack_bot;
	    return ret_val;
	}

    case SXFINAL:
	sxtkn_mngr (SXFINAL, 0);
	return SXTRUE;

    case SXCLOSE:
	/* end of language: free the local arrays */
	sxtkn_mngr (SXCLOSE, 0);
	(*sxplocals.SXP_tables.recovery) (SXCLOSE);
	return SXTRUE;

    case SXEND:
	/* free everything */
	sxfree (la_ref_stack - 1), la_ref_stack = NULL;
	sxfree (sxpglobals.parse_stack), sxpglobals.parse_stack = NULL;
	return SXTRUE;

    default:
	fprintf (sxstderr, "The function \"sxdparser_ndt\" is out of date with respect to its specification.\n");
	sxexit(1);
    }
}


