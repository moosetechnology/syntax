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

/* Cet analyseur utilise des tables non-de'terministes construites par le
   RLR ou le Left_Corner et produit une analyse unique.  Si le langage est
   de'terministe, c'est la bonne, si la phrase est ambigue, c'est l'une des
   analyses.  Peut e^tre modifie' pour trouver toutes les analyses. */

/* a ete realise a partir du parser deterministe de la version 5.0 */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Mar  7 Dec 1999 16:56(pb):	On part de la version de'terministe	*/
/************************************************************************/
/* 12-09-94 10:45 (pb):		VERSION PROVISOIRE : les post-actions	*/
/*				sont implantees ds ared->action et sont	*/
/*				>= 10000 et donc incompatibles avec des	*/
/*				actions semantiques sur la meme regle.	*/
/************************************************************************/
/* 10-08-94 16:17 (pb):		Prise en compte de 			*/
/*				sxplocals.mode.with_do_undo		*/
/************************************************************************/
/* 15-11-93 15:25 (pb):		L'appel de sxparser (ACTION, ...) 	*/
/*				depuis sxparse_in_la se fait par	*/
/*				 l'intermediaire de sxplocals.sxtables	*/
/************************************************************************/
/* 12-03-93 11:36 (pb):		errors_nb est separe en local_errors_nb	*/
/*				et global_errors_nb			*/
/************************************************************************/
/* 23-02-93 11:55 (pb):		Creation de la macro P_ACCESS() (unused)*/
/************************************************************************/
/* 22-02-93 11:55 (pb):		Suppression de la table "gotos"		*/
/*				Suppression des "adresses en pile"	*/
/*				Mfe, M0ref et Mref changent de		*/
/*				signification. Ajout de "final_state"	*/
/************************************************************************/
/* 05-02-93 09:10 (pb):		MARCHE ARRIERE, le champ token est de	*/
/*				nouveau un token (la modif precedente	*/
/*				impliquait look_back == 0)		*/
/************************************************************************/
/* 06-01-93 16:20 (pb):		Le champ token de la parse_stack devient*/
/*				un numero de token (toknb)		*/
/************************************************************************/
/* 24-09-92 14:50 (pb):		Adaptation au nouveau token_mngr	*/
/************************************************************************/
/* 08-04-92 11:40 (pb):		Utilisation de sxplocals.mode		*/
/************************************************************************/
/* 11-03-92 10:00 (pb):		Ajout de la fonction sxparse_in_la	*/
/************************************************************************/
/* 11-02-92 13:35 (pb):		token_mngr devient un module separe	*/
/************************************************************************/
/* 10-02-92 08:59 (pb):		Le point d'entree FINAL de sxtkn_mngr	*/
/*				est appele depuis FINAL du parser	*/
/************************************************************************/
/* 07-11-91 10:15 (pb):		Token_buf est initialise avec EOF	*/
/*				dans le nouveau point d'entree INIT et	*/
/*				non plus au debut de sxparse_it		*/
/************************************************************************/
/* 21-01-91 15:45 (pb):		Ajout de sxput_token			*/
/************************************************************************/
/* 05-10-89 10:21 (pb):		Bug dans la correction d'erreur :  	*/
/*				s2 [-1] pouvait etre negatif!		*/
/************************************************************************/
/* 21-06-89 10:39 (pb&phd):	On a contourne un bug du compilo apollo	*/
/*				dans execute_actions			*/
/************************************************************************/
/* 14-01-89 09:51 (pb):		Adaptation au RLR		  	*/
/************************************************************************/
/* 24-11-88 08:52 (pb):		Bug dans la correction d'erreur en  	*/
/*				presence de predicats			*/
/************************************************************************/
/* 16-11-88 10:01 (pb):		Oublie de la recopie de atok_no-1 lors 	*/
/*				du deplacement dans toks_buf		*/
/************************************************************************/
/* 09-06-88 09:22 (pb):		Suppression des macros NEXT_PTOK_NO et 	*/
/*				READ_P_TOKEN pour le compilo sm90 (SMX) */
/************************************************************************/
/* 03-06-88 10:10 (pb):		Modif de stack_bot		 	*/
/************************************************************************/
/* 15-04-88 11:40 (pb):		Introduction du token manager	 	*/
/************************************************************************/
/* 29-03-88 14:30 (pb):		Les nt vides prennent le source_index 	*/
/*				du look_ahead (non mais!)		*/
/************************************************************************/
/* 03-03-88 13:50 (pb):		Modification pour predicats sur nt	*/
/************************************************************************/
/* 11-01-88 14:54 (pb&phd):	Modification pour le compilateur uMEGA	*/
/************************************************************************/
/* 25-09-87 13:45 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/


static char	ME [] = "PARSER";

#include "sxunix.h"

char WHAT_SXDPARSER_NDT[] = "@(#)SYNTAX - $Id: sxdparser_ndt.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

/*   V A R I A B L E S   */

/* Variables which must be kept "local" to each recursive invocation of the
   parser are in the global variable "sxplocals", of type "struct sxplocals".
*/

static int		lgt1, lgt2, max_la_tok_no;
static SXP_SHORT	*la_ref_stack;

#if EBUG
int	    		nd_nb;
#endif

#define POP(t)		t[t[0]--]
#define PUSH(t,x)	((++t[0]>t[-1]) ? (SXP_SHORT*) sxrealloc (t-1, (t[-1] *= 2)+2, sizeof(SXP_SHORT))+1 : 0),t[t[0]]=x


static SXP_SHORT
sxP_access (abase, j)
    struct SXP_bases	*abase;
    SXP_SHORT		j;
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

static VOID	sxpsature ()
{
    sxpglobals.parse_stack = (struct sxparstack*) sxrealloc (sxpglobals.parse_stack,
							     (lgt1 *= 2) + 1,
							     sizeof (struct sxparstack));

}

#if 0
/* a mettre si on utilise le constructeur RLR */
int	ARC_traversal (ref, latok_no)
    register	int	ref;
    register	int	latok_no;
{
    register int	next;
    int		t_state;

    /* First scan in LookAhead */
    next = ref + sxplocals.SXP_tables.Mref /* next < 0 */;

    do {
	ref = sxP_access (sxplocals.SXP_tables.t_bases + (t_state = -next),
			sxget_token (++latok_no)->lahead);

	if (ref < -sxplocals.SXP_tables.Mred && ref >= -sxplocals.SXP_tables.Mprdct) {
	    /* Execution d'un predicat en look-ahead */
	    register struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts - ref;
	    int ptok_no;

	    ptok_no = sxplocals.ptok_no;
	    sxplocals.ptok_no = latok_no;

	    while (aprdct->prdct >= 0 /* User's predicate */  &&
		   (!sxplocals.mode.with_parsprdct ||
		    !(*sxplocals.SXP_tables.parsact) (PREDICATE, aprdct->prdct)))
		/* returning False */ 
		aprdct++;

	    ref = aprdct->action;
	    sxplocals.ptok_no = ptok_no;
	}
    } while ((next = ref + sxplocals.SXP_tables.Mref) < 0);

    /* LookAhead ends */
    if (ref == 0) {
	/* Error in LookAhead */
	(*sxplocals.SXP_tables.recovery) (ERROR, &t_state, latok_no);
	ref = t_state;
    }

    return ref;
}
#endif


static BOOLEAN
la_recognizer (xps, tok_no, ref)
   int		xps;
   SXP_SHORT	ref;
{
    struct SXP_reductions	*ared;
    struct SXP_prdct		*aprdct;
    SXP_SHORT			nt, prev_state, state, *pstack;
    int				lahead;
    BOOLEAN			ret_val;
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
	    return FALSE;

	if ((ared = sxplocals.SXP_tables.reductions + ref)->reduce == 0)
	    /* Halt */
	    return TRUE;

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
		return TRUE;
	    }

	    if (aprdct->prdct == 20000)
		return FALSE;
		    
	    aprdct++;
	    ref++;
	}
    }

    /* Predicates */
    while (aprdct->prdct >= 0 /* User's predicate */  &&
	   (!sxplocals.mode.with_parsprdct ||
	    !(*sxplocals.SXP_tables.parsact) (PREDICATE, aprdct->prdct)))
	/* returning False */
	aprdct++;

    return la_recognizer (xps, tok_no, aprdct->action);
}




static BOOLEAN	sxparse_it ()
{
    SXP_SHORT			ref, lahead, nt, state;
    struct SXP_reductions	*ared;
    struct SXP_prdct		*aprdct;
    BOOLEAN			is_semact, is_scan;
    int 			n;
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
    state = sxplocals.state = sxplocals.SXP_tables.init_state;

/* (*(sxplocals.SXP_tables.semact)) (INIT, lgt1); */

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
	    is_scan = TRUE;
	}
	else {
	    ref = -ref;
	    is_scan = FALSE;
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
		return FALSE;
	    }
	    
	    if ((sxpglobals.reduce = (ared = sxplocals.SXP_tables.reductions + ref)->reduce) == 0) {
		/* Halt */
		break;
	    }

	    /* Reduce */
	    sxpglobals.reduce = ared->reduce;
	    sxpglobals.pspl = ared->lgth;
	    is_semact = sxpglobals.reduce <= sxplocals.SXP_tables.P_nbpro && ared->action < 10000;

	    if (!is_semact && sxplocals.mode.with_do_undo && sxplocals.mode.with_parsact) {
		/* parsact a executer immediatement */
		/* Ca permet aux parsact de manipuler une structure // a stack en
		   utilisant les macros STACKtop(), STACKnewtop() et STACKreduce(). */
		if (ref > sxplocals.SXP_tables.Mrednt && !is_scan) {
		    /* Reduce dans les T_tables sans Scan */
		    sxpglobals.pspl--;
		    sxpglobals.xps--;
		    (*sxplocals.SXP_tables.parsact) (DO, ared->action);
		    sxpglobals.pspl++;
		    sxpglobals.xps++;
		}
		else
		    (*sxplocals.SXP_tables.parsact) (DO, ared->action);
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
		    sxplocals.SXP_tables.semact (ACTION, ared->action);
	    }
	    else
		if (sxplocals.mode.with_parsact)
		    sxplocals.SXP_tables.parsact (ACTION, ared->action);

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
			    return FALSE;
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
			!(*sxplocals.SXP_tables.parsact) (PREDICATE, aprdct->prdct)))
		    /* returning False */ 
		    aprdct++;

	    }

	    ref = aprdct->action;
	}
    }

    return TRUE;
}


static
error_message ()
{
    if (!sxplocals.mode.is_silent)
	sxput_error (SXGET_TOKEN (sxplocals.atok_no).source_index,
		     "%ssyntax error.",
		     sxplocals.sxtables->err_titles [2]);
}


BOOLEAN sxdparser_ndt (what_to_do, arg)
    int		what_to_do;
    struct sxtables	*arg;
{
    switch (what_to_do) {
    case BEGIN:

/* global initialization */

	lgt1 = 200;
	lgt2 = 50;

/* allocate arrays for normal analysis */

	sxpglobals.parse_stack = (struct sxparstack*) sxalloc (lgt1 + 1, sizeof (struct sxparstack));
	sxpglobals.xps = 0;
	sxpglobals.stack_bot = 0;
	la_ref_stack = (SXP_SHORT*) sxalloc (lgt2+2, sizeof (SXP_SHORT))+1;
	la_ref_stack [-1 /* size */] = lgt2;
	return TRUE;

    case OPEN:
	/* new language: new tables, new local parser variables */
	/* the global variable "sxplocals" must have been saved in case */
	/* of recursive invocation */

	/* test arg->magic for consistency */
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, ME);

/* prepare new local variables */

	sxplocals.sxtables = arg;
	sxplocals.SXP_tables = arg->SXP_tables;
	sxtkn_mngr (OPEN, 2);
	(*sxplocals.SXP_tables.recovery) (OPEN);
	return TRUE;

    case INIT:
	/* on initialise toks_buf avec "EOF" */

    {
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
	/* valeurs par defaut qui peut etre changee ds les scan_act ou pars_act. */
	sxplocals.mode.look_back = 0; 
	sxplocals.mode.mode = SXPARSER;
	sxplocals.mode.kind = SXWITH_RECOVERY;
	sxplocals.mode.local_errors_nb = 0;
	sxplocals.mode.global_errors_nb = 0;
	sxplocals.mode.is_prefixe = FALSE;
	sxplocals.mode.is_silent = FALSE;
	sxplocals.mode.with_semact = TRUE;
	sxplocals.mode.with_parsact = TRUE;
	sxplocals.mode.with_parsprdct = TRUE;
	sxplocals.mode.with_do_undo = FALSE;
	return TRUE;

    case ACTION:
	{
	    /* new file or portion of file */
	    /* arg is pointer to tables, but "sxplocals" should be allright */

	    SXP_SHORT	old_stack_bot = sxpglobals.stack_bot;
	    SXP_SHORT	reduce = sxpglobals.reduce;
	    SXP_SHORT	pspl = sxpglobals.pspl;
	    BOOLEAN	ret_val = TRUE;
	    int		lahead;

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
		    ret_val = FALSE;
		}
			
		if (lahead == TCUT)
		    sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;
	    } while (lahead != sxsvar.SXS_tables.S_termax);


#else
	    if (!sxparse_it ()) {
		error_message ();
		ret_val = FALSE;
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

    case FINAL:
	sxtkn_mngr (FINAL, 0);
	return TRUE;

    case CLOSE:
	/* end of language: free the local arrays */
	sxtkn_mngr (CLOSE, 0);
	(*sxplocals.SXP_tables.recovery) (CLOSE);
	return TRUE;

    case END:
	/* free everything */
	sxfree (la_ref_stack - 1), la_ref_stack = NULL;
	sxfree (sxpglobals.parse_stack), sxpglobals.parse_stack = NULL;
	return TRUE;

    default:
	fprintf (sxstderr, "The function \"sxdparser_ndt\" is out of date with respect to its specification.\n");
	abort ();
    }
}


