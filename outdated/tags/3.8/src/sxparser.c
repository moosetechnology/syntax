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
/* 05-02-93 09:10 (pb):		MARCHE ARRIERE, le champ token est de	*/
/*				nouveau un token (la modif precedente	*/
/*				impliquait look_back == 0)		*/
/************************************************************************/
/* 06-01-93 16:20 (pb):		Le champ token de la parse_stack devient*/
/*				un numero de token (toknb)		*/
/************************************************************************/
/* 24-09-92 14:50 (pb):		Adaptation au nouveau token_mngr	*/
/************************************************************************/
/* 30-03-92 11:40 (pb):		Utilisation de sxplocals.mode		*/
/************************************************************************/
/* 11-03-92 10:00 (pb):		Ajout de la fonction sxparse_in_la	*/
/************************************************************************/
/* 11-02-92 13:35 (pb):		token_mngr devient un module separe	*/
/************************************************************************/
/* 10-02-92 08:59 (pb):		Le point d'entree FINAL de sxtkn_mngr	*/
/*				est appele depuis FINAL du parser	*/
/************************************************************************/
/* 29-10-91 10:15 (pb):		Token_buf est initialise avec EOF	*/
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


#ifndef lint
char	what_sxparser [] = "@(#)sxparser.c\t- SYNTAX [unix] -  05 Fevrier 1993";
#endif

static char	ME [] = "PARSER";

#include "sxunix.h"

#define abs(x)	((x) < 0 ? -(x) : (x))

/*   V A R I A B L E S   */

/* Variables which must be kept "local" to each recursive invocation of the
   parser are in the global variable "sxplocals", of type "struct sxplocals".
*/

static int	lgt1, lgt2;
static SXSHORT	*stack /* lgt1 */ , *ga, *gb /* lgt2 */ ;
static BOOLEAN	bscan;



SXSHORT		P_access (abase, j)
    register struct SXP_bases	*abase;
    register SXSHORT	j;
{
    register struct SXP_item	*ajvector, *ajrefvector;
    register SXSHORT	ref = abase->commun;

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



static VOID	execute_actions (s1)
    register SXSHORT	*s1;
{
    /* Caracteristique de s1 :
       - elle contient exactement un Scan
       - elle ne contient pas de Scan en LookAhead
       - elle se termine par un Shift
       Apres execution le token du sommet de la parse_stack est donc non significatif */

    register SXSHORT	x, ref;
    register struct SXP_reductions	*ared;

    for (x = s1 [1] /* bottom */  + 1; x <= s1 [0]; x++) {
	if ((ref = s1 [x]) > 0) {
	    /* Scan */
	    sxpglobals.parse_stack [sxpglobals.xps].token = SXGET_TOKEN (sxplocals.atok_no);
	    sxplocals.atok_no++;
	}
	else {
	    /* NoScan */
	    ref = -ref;
	}

	if (ref > sxplocals.SXP_tables.Mprdct /* Shift */ ) {
	    sxpglobals.parse_stack [++sxpglobals.xps].state = (ref > sxplocals.SXP_tables.Mfe) ? ((ref -= sxplocals.
		 SXP_tables.M0ref) <= 0 ? 0 : ref + sxplocals.SXP_tables.Mref) : sxplocals.SXP_tables.gotos [ref].state;
	}
	else
	     /* Reduce or Halt */
	     if ((sxpglobals.reduce = (ared = sxplocals.SXP_tables.reductions + ref)->reduce) != 0 /* Reduce */ ) {
	    register BOOLEAN	is_semact = sxpglobals.reduce <= sxplocals.SXP_tables.P_nbpro;

	    sxpglobals.pspl = ared->lgth;

	    if (ref > sxplocals.SXP_tables.Mrednt && s1 [x] <= 0) {
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
	}
    }

    if (sxplocals.mode.look_back != 0)
	/* Ces tokens deviennent inaccessibles. */
	sxplocals.left_border = sxplocals.atok_no - sxplocals.mode.look_back;

    s1 [0] = s1 [1] /* top = bottom, empty the stack */ ;
}



static VOID	sxpsature (nbt)
    int		nbt;

/* overflow of tables for normal analysis */
    

{
    switch (nbt) {
    case 1:
	lgt1 *= 2;
	sxpglobals.parse_stack = (struct sxparstack*) sxrealloc (sxpglobals.parse_stack, lgt1 + 1, sizeof (struct
	     sxparstack));
	stack = (SXSHORT*) sxrealloc (stack, lgt1 + 1, sizeof (SXSHORT));
	break;

    case 2:
	lgt2 *= 2;
	ga = (SXSHORT*) sxrealloc (ga - 1, lgt2 + 2, sizeof (SXSHORT)) + 1;
	gb = (SXSHORT*) sxrealloc (gb - 1, lgt2 + 2, sizeof (SXSHORT)) + 1;
	break;
    }
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


static BOOLEAN	sxparse_it ()
{
    register SXSHORT	ref, lahead, state;
    register int	xs;
    register struct SXP_reductions	*ared;
    register SXSHORT *s1 = ga, *s2 = gb;

/*

          !scan                                              scan
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
					       | Shift: empiler ref+Mref-M0ref, goto ref-M0ref
                                               |
                                        M0ref  -
					       | Shift: empiler 0, goto ref-Mfe
                                               |
                                          Mfe  -
                                               | Shift: acces Floyd_Evans [ref]
                                               |
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
    state = sxplocals.state = sxplocals.SXP_tables.init_state;

/* (*(sxplocals.SXP_tables.semact)) (INIT, lgt1); */

restart:
    ref = P_access (sxplocals.SXP_tables.t_bases + (s2 [-1] = state), lahead);
    s1 [0] = s1 [1] /* top = bottom, empty the stacks */ ;
    s2 [0] = s2 [1];
    bscan = FALSE;


/* Swap de s1 et s2 : premiere etiquette apres scan
   Evaluation de s1 : premier test explicite */

    for (;;) {
	/* memorisation de l'action courante */
	if (++s2 [0] > lgt2) {
	    register BOOLEAN	s1_is_ga = s1 == ga;

	    sxpsature (2);

	    if (s1_is_ga)
		s1 = ga, s2 = gb;
	    else
		s1 = gb, s2 = ga;
	}

	s2 [s2 [0]] = ref;

	if (ref > 0 /* scan */ ) {
	    register int n;

	    if (s1 [1] != s1 [0])
		execute_actions (s1);

	    n = ++sxplocals.ptok_no;

	    while (n > sxplocals.Mtok_no)
		(*(sxplocals.SXP_tables.scanit)) ();

	    lahead = SXGET_TOKEN (n).lahead;
	    bscan = TRUE;
	}
	else
	    ref = -ref;

	if (ref > sxplocals.SXP_tables.Mprdct /* Shift */ ) {
	    if (++xs > lgt1)
		sxpsature (1);

	    if ((state = ref - sxplocals.SXP_tables.Mfe) > 0 /* direct */ ) {
		if ((ref -= sxplocals.SXP_tables.M0ref) > 0 /* etat == etiq */ ) {
		    stack [xs] = sxplocals.SXP_tables.Mref + (state = ref);
		}
		/* else empile 0 */ 
	    }
	    else {
		register struct SXP_goto	*afe = sxplocals.SXP_tables.gotos + ref;

		stack [xs] = afe->state;
		state = afe->next;
	    }

	    if (bscan) {
		register SXSHORT	*pp;

		bscan = FALSE;
		/* swap s1 <-> s2, s1 doit etre vide */
		pp = s1, s1 = s2, s2 = pp;
		s2 [-1] = state;
	    }

	    ref = P_access (sxplocals.SXP_tables.t_bases + state, lahead);
	}
	else if (ref <= sxplocals.SXP_tables.Mred /* Reduce, Error or Halt */ ) {
	    if (ref == 0 /* error recovery */ ) {
		if (sxplocals.mode.is_prefixe && state == sxplocals.final_state)
		    return TRUE;

		sxplocals.mode.errors_nb++;

		if (sxplocals.mode.kind == SXWITHOUT_ERROR)
		    return FALSE;

		if (s1 [1] != s1 [0] && s1 [-1] < 0) /* Predicat dans s1 non vide */
		    execute_actions (s1);
		
		sxplocals.state = (sxplocals.ptok_no == sxplocals.atok_no) ? abs (s2 [-1]) : s1 [-1];
		
		if ((*sxplocals.SXP_tables.recovery) (ACTION)) {
		    /* Succes */
		    register int	i;
		    
		    xs = sxpglobals.xps;
		    
		    for (i = sxpglobals.stack_bot + 1; i <= xs; i++) {
			/* stack [stack_bot] == 0 */
			/* restauration de la pile */
			stack [i] = sxpglobals.parse_stack [i].state;
		    }
		    
		    lahead = SXGET_TOKEN (sxplocals.ptok_no).lahead;
		    state = sxplocals.state;
		    goto restart;
		}
		
		/* Failure */
		return FALSE;
	    }
	    
/* Reduce or Halt */

	    if ((ared = sxplocals.SXP_tables.reductions + ref)->reduce == 0 /* Halt */ ) {
		if (s1 [1] != s1 [0])
		    /* On vide s1 */
		    execute_actions (s1);

		if (s2 [1] != s2 [0]) {
		    /* puis s2 */
		    execute_actions (s2);
		}

		break;
	    }

	    {
		/* Reduce */
		register SXSHORT	nt;

		xs -= ared->lgth;

		if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0 /* pas branchement direct */  && (state = (
		     ref = stack [xs]) - sxplocals.SXP_tables.Mref) > 0
		    /* pas adresse de branchement en pile */
								       ) {
		    ref = P_access (sxplocals.SXP_tables.nt_bases + state, nt);
		}
	    }
	}
	else {
	    /* Predicates */
	    register struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts + ref;

	    if (s1 [1] != s1 [0])
		execute_actions (s1);

	    while (aprdct->prdct >= 0 /* User's predicate */  &&
		   (!sxplocals.mode.with_parsprdct || !(*sxplocals.SXP_tables.parsact) (PREDICATE, aprdct->
		 prdct /* returning False */ )))
		aprdct++;

	    ref = aprdct->action;
	    --s2 [0] /* On ecrase le predicat */ ;

	    if (s2 [-1] > 0)
		s2 [-1] = -s2 [-1] /* On note la presence de predicats ds s2 */;
	}
    }

    sxtkn_mngr (FINAL, 0);
    return TRUE;
}




BOOLEAN sxparser (what_to_do, arg)
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
	stack = (SXSHORT*) sxalloc (lgt1 + 1, sizeof (SXSHORT));
	ga = (SXSHORT*) sxalloc (lgt2 + 2, sizeof (SXSHORT)) + 1;
	gb = (SXSHORT*) sxalloc (lgt2 + 2, sizeof (SXSHORT)) + 1;
	sxpglobals.xps = 0;
	sxpglobals.stack_bot = 0;
	ga [0] = gb [0] = ga [1] = gb [1] /* top, bottom */  = 1;
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
	/* valeurs par defaut qui peut etre changee ds les
	   scan_act ou pars_act. */
	sxplocals.mode.look_back = 1; 
	sxplocals.mode.kind = SXWITH_RECOVERY;
	sxplocals.mode.errors_nb = 0;
	sxplocals.mode.is_prefixe = FALSE;
	sxplocals.mode.is_silent = FALSE;
	sxplocals.mode.with_semact = TRUE;
	sxplocals.mode.with_parsact = TRUE;
	sxplocals.mode.with_parsprdct = TRUE;
	return TRUE;

    case ACTION:
	{
	    /* new file or portion of file */
	    /* arg is pointer to tables, but "sxplocals" should be allright */

	    register SXSHORT	old_stack_bot = sxpglobals.stack_bot, old_ga_bot = ga [1], old_ga_top = ga [0],
		 old_ga_state = ga [-1], old_gb_bot = gb [1], old_gb_top = gb [0], old_gb_state = gb [-1];
	    register SXSHORT	reduce = sxpglobals.reduce;
	    register SXSHORT	pspl = sxpglobals.pspl;
	    register BOOLEAN	old_bscan = bscan;
	    BOOLEAN		ret_val;

	    if ((sxpglobals.stack_bot = sxpglobals.xps + 5) > lgt1)
		sxpsature (1);

	    ga [1] = ga [0];
	    gb [1] = gb [0];

	    ret_val = sxparse_it ();

	    sxpglobals.reduce = reduce;
	    sxpglobals.pspl = pspl;
	    sxpglobals.xps = sxpglobals.stack_bot - 5;
	    sxpglobals.stack_bot = old_stack_bot;
	    bscan = old_bscan;
	    ga [1] = old_ga_bot;
	    ga [0] = old_ga_top;
	    ga [-1] = old_ga_state;
	    gb [1] = old_gb_bot;
	    gb [0] = old_gb_top;
	    gb [-1] = old_gb_state;
	    return ret_val;
	}

    case FINAL:
	sxtkn_mngr (FINAL, 0);
	return TRUE;

    case CLOSE:
	/* end of language: free the local arrays */
	sxtkn_mngr (CLOSE, 0);
	return TRUE;

    case END:
	/* free everything */
	sxfree (gb - 1), gb = NULL;
	sxfree (ga - 1), ga = NULL;
	sxfree (stack), stack = NULL;
	sxfree (sxpglobals.parse_stack), sxpglobals.parse_stack = NULL;
	(*sxplocals.SXP_tables.recovery) (CLOSE);
	return TRUE;

    default:
	fprintf (sxstderr, "The function \"sxparser\" is out of date with respect to its specification.\n");
	abort ();
    }
}


BOOLEAN sxparse_in_la (ep_la, Ttok_no, Htok_no, mode_ptr)
    int	ep_la, Ttok_no, *Htok_no;
    struct sxparse_mode		*mode_ptr;
{
    /* Appel recursif du parser pour verifier si le sous-langage defini par
       le point d'entree ep_la contient un prefixe du source. */
    /* Le token caracteristique du point d'entree est memorise en Ttok_no */
    /* L'indice du dernier token analyse par cet appel est range en Htok_no */
    int		atok_no, ptok_no, old_left_border,
                old_la, old_ste, ref, state;
    BOOLEAN			ret_val;
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
    ptt->string_table_entry = EMPTY_STE;
    /* On pointe sur le token precedent (normalement non acce'de') */
    sxplocals.atok_no = sxplocals.ptok_no = Ttok_no - 1;
    
    ref = P_access (sxplocals.SXP_tables.t_bases + sxplocals.SXP_tables.init_state,
		    sxplocals.SXP_tables.P_tmax); /* ref = GOTO (init_state, eof) */
    /* ref est > 0 (scan) */

    if ((state = ref - sxplocals.SXP_tables.Mfe) > 0 /* direct */ ) {
	if ((ref -= sxplocals.SXP_tables.M0ref) > 0 /* etat == etiq */ ) {
	    state = ref;
	}
    }
    else {
	state = sxplocals.SXP_tables.gotos [ref].next;
    }

    /* state = GOTO (init_state, eof) */
    ref = P_access (sxplocals.SXP_tables.nt_bases + state, 1 /* axiome */);

    if (ref < 0)
	ref = -ref;

    if ((state = ref - sxplocals.SXP_tables.Mfe) > 0 /* direct */ ) {
	if ((ref -= sxplocals.SXP_tables.M0ref) > 0 /* etat == etiq */ ) {
	    state = ref;
	}
    }
    else {
	state = sxplocals.SXP_tables.gotos [ref].next;
    }

    /* state = GOTO (init_state, "eof axiome") */
    sxplocals.final_state = state;

    /* appel recursif */
    ret_val = sxparser (ACTION, sxplocals.sxtables);
    
    /* dernier token lu par l'appel precedent. */
    *Htok_no = sxplocals.ptok_no;

    /* On remet en place */
    *ptt = old_tt;
    sxplocals.atok_no = atok_no;
    sxplocals.ptok_no = ptok_no;
    mode_ptr->errors_nb = sxplocals.mode.errors_nb;
    /* L'appelant decide (et le fait) si le nombre d'erreur doit
       etre cumule'. */
    sxplocals.mode = old_mode;
    sxplocals.left_border = old_left_border;

    return ret_val;
}
