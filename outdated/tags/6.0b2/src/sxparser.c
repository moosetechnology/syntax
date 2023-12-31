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
/* 20030512 10:59 (phd):	Adaptation � SGI (64 bits)		*/
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
/* 10 Jul 2000 16:55(pb):	Bug ds les ga, gb, les indices ne	*/
/*				peuvent pas etre des short		*/
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

#ifndef VARIANT_32
char WHAT_SXPARSER[] = "@(#)SYNTAX - $Id: sxparser.c 617 2007-06-11 08:24:34Z rlacroix $" WHAT_DEBUG;
#else
char WHAT_SXPARSER[] = "@(#)SYNTAX - $Id: sxparser.c 617 2007-06-11 08:24:34Z rlacroix $ SXPARSER_32" WHAT_DEBUG;
#endif
#if 0 /* d�j� d�fini par stdlib.h, d�sormais inclus via sxu2.h */
#define abs(x)	((x) < 0 ? -(x) : (x))
#endif /* 0 */

/*   V A R I A B L E S   */

/* Variables which must be kept "local" to each recursive invocation of the
   parser are in the global variable "sxplocals", of type "struct sxplocals".
*/

static int	lgt1, lgt2;
static SXP_SHORT	*stack /* lgt1 */;
static BOOLEAN	bscan;
static struct gstack {
    int		top, bot;
    SXP_SHORT	*stack /* lgt2 */;
    SXP_SHORT	state;
} ga, gb;



SXP_SHORT		sxP_access (abase, j)
    register struct SXP_bases	*abase;
    register SXP_SHORT	j;
{
    register struct SXP_item	*ajvector, *ajrefvector;
    register SXP_SHORT	ref = abase->commun;

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

#if 0
/* semble ne rien gagner... */
#define sxP_ACCESS(abase,j)									\
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

static VOID	execute_actions (s1)
    register struct gstack	*s1;
{
    /* Caracteristique de s1 :
       - elle contient exactement un Scan
       - elle ne contient pas de Scan en LookAhead
       - elle se termine par un Shift
       Apres execution le token du sommet de la parse_stack est donc non significatif */

    register SXP_SHORT	x, state, ref;
    register struct SXP_reductions	*ared;

    for (x = s1->bot /* bottom */  + 1; x <= s1->top; x++) {
	if ((ref = s1->stack [x]) > 0) {
	    /* Scan */
	    sxpglobals.parse_stack [sxpglobals.xps].token = SXGET_TOKEN (sxplocals.atok_no);
	    sxplocals.atok_no++;
	}
	else {
	    /* NoScan */
	    ref = -ref;
	}

	if ((state = ref - sxplocals.SXP_tables.Mprdct) > 0 /* Shift */ ) {
	    sxpglobals.parse_stack [++sxpglobals.xps].state = state;
	}
	else
	     /* Reduce or Halt */
	     if ((sxpglobals.reduce =
		  (ared = sxplocals.SXP_tables.reductions + ref)->reduce) != 0 /* Reduce */ ) {
	    register BOOLEAN	is_semact = 
		sxpglobals.reduce <= sxplocals.SXP_tables.P_nbpro && ared->action < 10000;

	    sxpglobals.pspl = ared->lgth;

	    if (ref > sxplocals.SXP_tables.Mrednt && s1->stack [x] <= 0) {
		/* Reduce dans les T_tables sans Scan */

		if (sxpglobals.pspl-- == 0)
		    /* production vide */
		    sxpglobals.parse_stack [sxpglobals.xps].token.source_index =
			SXGET_TOKEN (sxplocals.atok_no).source_index;

		sxpglobals.xps--;
	    }

	    if (is_semact) {
		if (sxplocals.mode.with_semact)
		    (*sxplocals.SXP_tables.semact) (ACTION, ared->action);
	    }
	    else
		if (sxplocals.mode.with_parsact)
		    (*sxplocals.SXP_tables.parsact) (ACTION, ared->action);

	    sxpglobals.xps -= sxpglobals.pspl;
	    sxpglobals.parse_stack [sxpglobals.xps].token.lahead =
		is_semact ? 0 /* nt */  : -1 /* action */ ;
	}
    }

    if (sxplocals.mode.look_back != 0)
	/* Ces tokens deviennent inaccessibles. */
	sxplocals.left_border = sxplocals.atok_no - sxplocals.mode.look_back;

    s1->top = s1->bot /* top = bottom, empty the stack */ ;
}


static int undo_actions (s1, xs)
    register struct gstack	*s1;
    int				xs;
{
    /* On doit "defaire" les actions eventuelles de s1, en ordre inverse de
       leur execution en assurant la coherence des indices de pile d'analyse. */

    register SXP_SHORT	x, ref;
    register struct SXP_reductions	*ared;

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

		if (ared->reduce > sxplocals.SXP_tables.P_nbpro || ared->action >= 10000) {
		    int old_xps = sxpglobals.xps;

		    sxpglobals.reduce = ared->reduce;
		    sxpglobals.pspl = ared->lgth; /* 0 */
		    sxpglobals.xps = xs;

		    if (ref > sxplocals.SXP_tables.Mrednt && s1->stack [x] <= 0) {
			/* Reduce dans les T_tables sans Scan */
			sxpglobals.pspl--;
			sxpglobals.xps--;
		    }

		    (*sxplocals.SXP_tables.parsact) (UNDO, ared->action);

		    sxpglobals.xps = old_xps;
		}
	    }
	}
    }

    s1->top = s1->bot /* top = bottom, empty the stack */ ;

    return xs;
}



static VOID	sxpsature (nbt)
    int		nbt;

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

SXP_SHORT	ARC_traversal (ref, latok_no)
    register	SXP_SHORT	ref;
    register	int	latok_no;
{
    register SXP_SHORT	next;
    SXP_SHORT		t_state;

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
    SXP_SHORT			ref, lahead, state;
    int				xs;
    struct SXP_reductions	*ared;
    register struct gstack 	*s1 = &ga, *s2 = &gb;
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

    xs = sxpglobals.xps = sxpglobals.stack_bot;
    stack [xs] = 0;
    sxpglobals.parse_stack [xs].state = 0;
    lahead = sxplocals.SXP_tables.P_tmax;
    state = sxplocals.state = sxplocals.SXP_tables.init_state;

/* (*(sxplocals.SXP_tables.semact)) (INIT, lgt1); */

restart:
    ref = sxP_access (sxplocals.SXP_tables.t_bases + (s2->state = state), lahead);
    s1->top = s1->bot /* top = bottom, empty the stacks */ ;
    s2->top = s2->bot;
    bscan = FALSE;


/* Swap de s1 et s2 : premiere etiquette apres scan
   Evaluation de s1 : premier test explicite */

    for (;;) {
	if (ref < -sxplocals.SXP_tables.Mref)
	    ref = ARC_traversal (ref, sxplocals.ptok_no);

	/* memorisation de l'action courante */
	if (++s2->top > lgt2) {
	    sxpsature (2);
	}

	s2->stack [s2->top] = ref;

	if (ref > 0 /* scan */ ) {
	    int n;

	    if (s1->bot != s1->top)
		execute_actions (s1);

	    n = ++sxplocals.ptok_no;

	    while (n > sxplocals.Mtok_no)
		(*(sxplocals.SXP_tables.scanit)) ();

	    lahead = SXGET_TOKEN (n).lahead;
	    bscan = TRUE;
	}
	else
	    ref = -ref;

	if ((state = ref - sxplocals.SXP_tables.Mprdct) > 0 /* Shift */ ) {
	    if (++xs > lgt1)
		sxpsature (1);

	    if (state <= sxplocals.SXP_tables.M0ref)
		stack [xs] = state;
	    /* else empile 0 */

	    if (bscan) {
		struct gstack	*pp;

		bscan = FALSE;
		/* swap s1 <-> s2, s1 doit etre vide */
		pp = s1, s1 = s2, s2 = pp;
		s2->state = state;
	    }

	    ref = sxP_access (sxplocals.SXP_tables.t_bases + state, lahead);
#if 0
	    abase = sxplocals.SXP_tables.t_bases + state;
	    ref = P_ACCESS (abase, lahead);
#endif
	}
	else if (ref <= sxplocals.SXP_tables.Mred /* Reduce, Error or Halt */ ) {
	    if (ref == 0 /* error recovery */ ) {
		if (sxplocals.mode.is_prefixe && state == sxplocals.SXP_tables.final_state)
		    return TRUE;

		sxplocals.mode.local_errors_nb++;

		if (sxplocals.mode.kind == SXWITHOUT_ERROR)
		    return FALSE;

		if (sxplocals.mode.with_do_undo && sxplocals.mode.with_parsact)
		{
		    /* Ds le cas DO/UNDO, on essaie une correction sur le token
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
		
		sxplocals.state = (sxplocals.ptok_no == sxplocals.atok_no) ? abs (s2->state) : s1->state;

		if ((*sxplocals.SXP_tables.recovery) (ACTION)) {
		    /* Succes */
		    int	i;
		    
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

		if ((ared->reduce > sxplocals.SXP_tables.P_nbpro || ared->action >= 10000) &&
		    sxplocals.mode.with_do_undo &&
		    sxplocals.mode.with_parsact)
		{
		    /* parsact a executer immediatement */
		    /* Ca permet aux parsact de manipuler une structure // a stack en
		       utilisant les macros STACKtop(), STACKnewtop() et STACKreduce(). */
		    int old_xps = sxpglobals.xps;

		    sxpglobals.reduce = ared->reduce;
		    sxpglobals.pspl = ared->lgth; /* 0 */
		    sxpglobals.xps = xs;

		    if (ref > sxplocals.SXP_tables.Mrednt && s2->stack [s2->top] <= 0) {
			/* Reduce dans les T_tables sans Scan */
			sxpglobals.pspl--;
			sxpglobals.xps--;
		    }

		    (*sxplocals.SXP_tables.parsact) (DO, ared->action);

		    sxpglobals.xps = old_xps;
		}

		xs -= ared->lgth;

		if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
		    /* pas branchement direct */
		    state = stack [xs];
		    ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, nt); 
#if 0
		    abase = sxplocals.SXP_tables.nt_bases + state;
		    ref = sxP_ACCESS (abase, nt);
#endif
		}
	    }
	}
	else {
	    /* Predicates */
	    struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts + ref;
	    int old_xps;

	    if (!sxplocals.mode.with_do_undo && s1->bot != s1->top)
		execute_actions (s1);

	    --s2->top /* On ecrase le predicat */ ;

	    /* Au cas ou le predicat est associe' a un nt et il veut re'fe'rencer
	       une structure // a` stack. Il faut utiliser STACKtop () */
	    old_xps = sxpglobals.xps;
	    sxpglobals.xps = xs; 

	    while (aprdct->prdct >= 0 /* User's predicate */  &&
		   (!sxplocals.mode.with_parsprdct ||
		    !(*sxplocals.SXP_tables.parsact) (PREDICATE, aprdct->prdct)))
		/* returning False */ 
		aprdct++;

	    sxpglobals.xps = old_xps;

	    ref = aprdct->action;

	    if (s2->state > 0)
		s2->state = -s2->state /* On note la presence de predicats ds s2 */;
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

	sxpglobals.parse_stack = (struct sxparstack*) sxalloc (lgt1 + 1,
							       sizeof (struct sxparstack));
	stack = (SXP_SHORT*) sxalloc (lgt1 + 1, sizeof (SXP_SHORT));
	ga.stack = (SXP_SHORT*) sxalloc (lgt2 + 1, sizeof (SXP_SHORT));
	gb.stack = (SXP_SHORT*) sxalloc (lgt2 + 1, sizeof (SXP_SHORT));
	sxpglobals.xps = 0;
	sxpglobals.stack_bot = 0;
	ga.top = ga.bot = gb.top = gb.top = 1; /* top, bottom  = 1; */ 
	break;

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
	break;

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
	break;

    case ACTION:
	{
	    /* new file or portion of file */
	    /* arg is pointer to tables, but "sxplocals" should be allright */

	    SXP_SHORT	old_stack_bot = sxpglobals.stack_bot, old_ga_state = ga.state, old_gb_state = gb.state;
	    int		old_ga_bot = ga.bot, old_ga_top = ga.top, old_gb_bot = gb.bot, old_gb_top = gb.top;
	    SXP_SHORT	reduce = sxpglobals.reduce;
	    SXP_SHORT	pspl = sxpglobals.pspl;
	    BOOLEAN	old_bscan = bscan;
	    BOOLEAN	ret_val;

	    if ((sxpglobals.stack_bot = sxpglobals.xps + 5) > lgt1)
		sxpsature (1);

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

    case FINAL:
	sxtkn_mngr (FINAL, 0);
	break;

    case CLOSE:
	/* end of language: free the local arrays */
	sxtkn_mngr (CLOSE, 0);
	(*sxplocals.SXP_tables.recovery) (CLOSE);
	break;

    case END:
	/* free everything */
	sxfree (gb.stack), gb.stack = NULL;
	sxfree (ga.stack), ga.stack = NULL;
	sxfree (stack), stack = NULL;
	sxfree (sxpglobals.parse_stack), sxpglobals.parse_stack = NULL;
	break;

    default:
	fprintf (sxstderr, "The function \"sxparser\" is out of date with respect to its specification.\n");
	abort ();
	    /*NOTREACHED*/
    }

    return TRUE;
}


BOOLEAN sxparse_in_la (ep_la, Ttok_no, Htok_no, mode_ptr)
    int	ep_la, Ttok_no, *Htok_no;
    struct sxparse_mode		*mode_ptr;
{
    /* Appel recursif du parser pour verifier si le sous-langage defini par
       le point d'entree ep_la contient un prefixe du source. */
    /* Le token caracteristique du point d'entree est memorise en Ttok_no */
    /* L'indice du dernier token analyse par cet appel est range en Htok_no */
    int		atok_no, ptok_no, old_left_border;
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
    
    /* appel recursif */
    ret_val = (*(sxplocals.sxtables->analyzers.parser)) (ACTION, sxplocals.sxtables);
    
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
