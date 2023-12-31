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
   *                                                      *
   ******************************************************** */




/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from parser.pl1         */
/*  on Thursday the second of January, 1986, at 11:32:44,    */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3.e PL1_C translator of INRIA,          */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 14:18 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* Jeu  4 Nov 1999 10:58(pb):	germe_to_gr_act peut etre null		*/
/************************************************************************/
/* Mer 3 Nov 1999 09:06(pb):	Bug ds deriv si grammaire conflictuelle	*/
/************************************************************************/
/* Mer 3 Nov 1999 09:06(pb):	Bug ds deriv si pas [LA]LR(1)		*/
/************************************************************************/
/* 13-09-94 10:23 (pb):		VERSION PROVISOIRE : les post-actions	*/
/*				sont implantees ds ared->action et sont	*/
/*				>= 10000 et donc incompatibles avec des	*/
/*				actions semantiques sur la meme regle.	*/
/************************************************************************/
/* 22-08-94 15:05 (pb):		Prise en compte de "with_do_undo"	*/
/************************************************************************/
/* 21-10-93 13:00 (phd&pb):	Adaptation a OSF			*/
/************************************************************************/
/* 25-03-93 14:19 (pb):		Suppression du parametre "stack" dans	*/
/*				"is_a_right_ctxt", "check_nt_ctxt" et	*/
/*				"deriv" (bug sur oflow).		*/
/************************************************************************/
/* 25-02-93 11:25 (pb) :	Les variables locales sont mises dans	*/
/*				sxplocals.rcvr				*/
/************************************************************************/
/* 22-02-93 15:00 (pb):		Suppression de la table "gotos"		*/
/*				Suppression des "adresses en pile"	*/
/*				Mfe, M0ref et Mref changent de		*/
/*				signification. Ajout de "final_state"	*/
/*				Utilisation des SXPER_trans_sets	*/
/************************************************************************/
/* 18-01-93 17:45 (pb):		Bug ds la version 4.0 de "deriv". 	*/
/************************************************************************/
/* 24-09-92 15:05 (pb):		Adaptation au nouveau token_mngr	*/
/************************************************************************/
/* 24-04-92 10:35 (pb):		Bug sur la valeur de atok_no retournee	*/
/*				lorsque un predicat est associe au	*/
/*				token precedent.			*/
/************************************************************************/
/* 08-04-92 10:35 (pb):		Utilisation du "mode" d'analyse		*/
/************************************************************************/
/* 18-03-92 11:20 (pb):		La zone "comment" des tokens inseres	*/
/*				n'etait pas remise a NULL lorsque les	*/
/*				commentaires etaient supprimes.		*/
/************************************************************************/
/* 11-09-91 11:20 (pb):		Amelioration ds deriv : les actions	*/
/*				reduce (avec champ test explicite et	*/
/*				sans scan) ne sont plus executees qu'une*/
/*				seule fois a un niveau donne.		*/
/************************************************************************/
/* 05-07-91 09:15 (pb):		Bug (grossier) detecte par GEC-ALSTHOM	*/
/*				La taille de la "parse_stack" pouvait	*/
/*				etre superieure a la taille des piles	*/
/*				locales paralleles			*/
/************************************************************************/
/* 05-10-89 11:15 (pb):		Prise en compte d'un appel recursif de	*/
/*				"recovery" par l'intermediaire du	*/
/*				"scanner" (sxget_token)			*/
/************************************************************************/
/* 17-03-89 14:00 (pb):		Adaptation au RLR			*/
/************************************************************************/
/* 24-11-88 09:40 (pb):		Suppression du dernier parametre de	*/
/*				P_access				*/
/*				is_a_right_ctxt rend FALSE si predicat	*/
/************************************************************************/
/* 03-06-88 10:19 (pb):		Bug pour STACKtop et STACKnew_top dans	*/
/*				le cas "End Of File"			*/
/************************************************************************/
/* 31-05-88 10:02 (pb):		Bug dans set_next_trans			*/
/************************************************************************/
/* 05-05-88 11:15 (pb):		Bug recuperation globale introduit sans */
/*				doute avec la 3.0			*/
/************************************************************************/
/* 15-04-88 14:12 (pb):		Adaptation au "token manager"		*/
/************************************************************************/
/* 29-03-88 18:00 (pb):		Adaptation aux sequences de predicats	*/
/************************************************************************/
/* 24-03-88 09:42 (pb):		Les prdcts rendent tous faux (prudence)	*/
/************************************************************************/
/* 23-03-88 09:08 (pb):		Bug dans set_[first/next]_trans		*/
/************************************************************************/
/* 04-03-88 10:05 (pb):		Modification pour predicats sur nt	*/
/************************************************************************/
/* 10-02-88 11:50 (pb):		Bug dans is_a_right_ctxt sur les prdcts	*/
/*				On n'execute les predicats que sur les	*/
/*				positions 0-1 comme en analyse normale	*/
/*				L'insertion d'un symbole unique est	*/
/*				conditionnee par la validation du	*/
/*				predicat associe			*/
/************************************************************************/
/* 21-01-88 14:50 (phd):	Suppression des macros "min" et "max",	*/
/*				remplacement "truncatate" -> "truncate"	*/
/************************************************************************/
/* 22-12-87 17:42 (phd):	Adaptation aux bits arrays		*/
/************************************************************************/
/* 04-12-87 10:35 (pb):		Adaptation a la V3			*/
/************************************************************************/
/* 10-11-87 10:50 (pb):		Possibilite de correction reduite si	*/
/*				non-partitionnement des predicats.	*/
/************************************************************************/
/* 04-11-87 15:10 (pb&phd):	Prise en compte de la possibilite de	*/
/*				non-partitionnement des predicats.	*/
/************************************************************************/
/* 05-10-87 09:48 (pb):		Message "Global Recovery" sorti avant	*/
/*				"parsing stops on EOF"			*/
/************************************************************************/
/* 29-07-87 15:06 (phd):	Suppression d'une affectation inutile	*/
/************************************************************************/
/* 02-07-87 10:00 (pb) :	Adaptation au nouveau recor		*/
/************************************************************************/
/* 15-06-87 09:02 (pb) :	Adaptation aux actions et predicats	*/
/*				+ ameliorations				*/
/************************************************************************/
/* 25-05-87 11:03 (phd) :	Ajout d'un message d'erreur		*/
/************************************************************************/
/* 19-05-87 17:49 (pb&phd) :	Utilisation des tables du scanner pour 	*/
/*				sortir le texte de "End Of File"	*/
/************************************************************************/
/* 03-05-87 08:55 (pb) :	Adaptation aux nouvelles tables        	*/
/************************************************************************/
/* 26-03-87 11:15 (pb) :	"END" change en "CLOSE"	          	*/
/************************************************************************/
/* 12-03-87 11:52 (pb) :	sxprecovery transforme en fonction   	*/
/************************************************************************/
/* 04-03-87 10:22 (pb) :	Bug dans l'insertion forcee	   	*/
/************************************************************************/
/* 27-02-87 10:40 (pb) :	Separation parser et error_recovery	*/
/************************************************************************/
/* 16-05-86 17:11 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/



# include "sxunix.h"
# include "SS.h"

#ifndef VARIANT_32
char WHAT_SXP_RECOVERY[] = "@(#)SYNTAX - $Id: sxp_rcvr.c 617 2007-06-11 08:24:34Z rlacroix $" WHAT_DEBUG;
#else
char WHAT_SXP_RECOVERY[] = "@(#)SYNTAX - $Id: sxp_rcvr.c 617 2007-06-11 08:24:34Z rlacroix $ SXP_RCVR_32" WHAT_DEBUG;
#endif
#define Commun 1
#define Propre 2
#define Defaut 3
#define Done   4

extern SXP_SHORT	sxP_access (), ARC_traversal ();

/*   V A R I A B L E S   */

static int	lgt1;


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

static char	*ttext (tables, tcode)
    struct sxtables	*tables;
    int		tcode;
{
    return (tcode == sxplocals.SXP_tables.P_tmax) ? sxplocals.SXP_tables.P_global_mess [sxplocals.SXP_tables.
	 P_followers_number + 2] : sxttext (tables, tcode);
}



static VOID	oflow ()

/* overflow of stack */

{
    lgt1 *= 2;
    sxplocals.rcvr.modified_set = sxba_resize (sxplocals.rcvr.modified_set, lgt1 + 1);
    sxplocals.rcvr.st_stack = (SXP_SHORT*) sxrealloc (sxplocals.rcvr.st_stack, lgt1 + 1, sizeof (SXP_SHORT));
    sxplocals.rcvr.stack = (SXP_SHORT*) sxrealloc (sxplocals.rcvr.stack, lgt1 + 1, sizeof (SXP_SHORT));
}




static BOOLEAN	set_next_item (vector, check, action, Max)
    struct SXP_item	*vector;
    register SXP_SHORT	*check, *action, Max;
{
    register struct SXP_item	*aitem;
    register SXP_SHORT	val;

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



static VOID	set_first_trans (abase, check, action, Max, next_action_kind)
    struct SXP_bases	*abase;
    SXP_SHORT	*check, *action, Max;
    int		*next_action_kind;
{
    SXP_SHORT	ref = abase->commun;

    if (ref <= sxplocals.SXP_tables.Mref /* codage direct */ ) {
	if ((*check = abase->propre) != 0) {
	    *action = ref;
	    *next_action_kind = Defaut;
	}
	else /* defaut */  {
	    *action = abase->defaut;
	    *next_action_kind = Done;
	}
    }
    else {
	*check = 0;
	set_next_item (sxplocals.SXP_tables.vector + ref, check, action, Max) /* always true (commun non vide) */ ;
	*next_action_kind = Commun;
    }
}



static BOOLEAN	set_next_trans (abase, check, action, Max, next_action_kind)
    register struct SXP_bases	*abase;
    SXP_SHORT	*check, *action, Max;
    int		*next_action_kind;
{
  switch (*next_action_kind) {
  case Commun:
    if (set_next_item (sxplocals.SXP_tables.vector + abase->commun, check, action, Max))
      return TRUE;

    if (abase->propre != 0 /* partie propre non vide */ ) {
      *next_action_kind = Propre;
      *check = 0;
      /* !!! pas de break ici : c'est voulu */

    case Propre:
       /* !!! ce "case" se trouve au milieu du "if" : c'est voulu */
      if (set_next_item (sxplocals.SXP_tables.vector + abase->propre, check, action, Max))
	return TRUE;
    }
    /* !!! pas de break ici : c'est voulu */
	
  case Defaut:
    *check = 0;
    *action = abase->defaut;
    *next_action_kind = Done;
    return TRUE;

  case Done:
    return FALSE;

  default:
    sxtrap ("sxp_rcvr", "set_next_trans");
    /*NOTREACHED*/
  }

  return FALSE; /*NOTREACHED*/
}


static BOOLEAN	reduce_already_seen (abase, check, action)
    register struct SXP_bases	*abase;
    SXP_SHORT	check, action;
{
    /* action ( >0) reference une action reduce qui n'est pas l'action par defaut
       du abase courant. On regarde si cette action n'a pas deja ete traitee. */

	SXP_SHORT		ref, test;
	int	next_action_kind;

	set_first_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind);

	do {
	    if (test == check)
		return FALSE;

	    if (ref == -action)
		return TRUE;
	} while (set_next_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind));

	return FALSE; /* garde-fou */
}

static int	get_tail (binf, bsup)
    int		binf, bsup;
{
    if (!sxplocals.rcvr.truncate_context)
	return bsup;

    if (binf == 0)
	binf = 1;

    for (; binf <= bsup; binf++) {
	sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + binf;

	if (sxgetbit (sxplocals.SXP_tables.PER_tset,
		      SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead))
	    return binf;
    }

    return bsup;
}



static SXVOID	restaure_stack (m, M)
    int		m, M;
{
    register int	i;

    for (i = m; i <= M; i++) {
	/* sauvegarde de la pile */
	sxplocals.rcvr.stack [i] = sxpglobals.parse_stack [i].state;
    }
}



static BOOLEAN	ARC_is_a_right_ctxt (head, tail, ref)
    register int		*head, tail;
    register SXP_SHORT		*ref;

/* Check if tokens contained in tokens[head..tail] are a valid right context of
   *ref. Assume that tail takes into account the occurrences of key-terminals.
   Predicates are supposed to return FALSE */


{
    register SXP_SHORT	next = *ref;

    for (; *head <= tail && (next += sxplocals.SXP_tables.Mref) < 0 ; (*head)++) {
	sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + *head;

	*ref = next = sxP_access (sxplocals.SXP_tables.t_bases - next,
				  SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead);

	if (next == 0 ||
	    (next < -sxplocals.SXP_tables.Mred && next >= -sxplocals.SXP_tables.Mprdct))
	    /* Error or Predicate */
	    return FALSE;
    }

    return TRUE;
}



static void undo ()
{
    struct SXP_reductions	*ared;
    int				old_xps, x;
    SXP_SHORT			ref_orig, ref;

    if (!SS_is_empty (sxplocals.rcvr.undo_stack)) {
	old_xps = sxpglobals.xps;
	x = SS_bot (sxplocals.rcvr.undo_stack);

	while (x < SS_top (sxplocals.rcvr.undo_stack)) {
	    ref_orig = ref  = SS_get (sxplocals.rcvr.undo_stack, x++);
	    sxpglobals.xps = SS_get (sxplocals.rcvr.undo_stack, x++);

	    if (ref < 0)
		ref = -ref;

	    ared = sxplocals.SXP_tables.reductions + ref;
	    sxpglobals.reduce = ared->reduce;
	    sxpglobals.pspl = ared->lgth; /* 0 */

	    if (ref > sxplocals.SXP_tables.Mrednt && ref_orig <= 0) {
		/* Reduce dans les T_tables sans Scan */
		sxpglobals.pspl--;
		sxpglobals.xps--;
	    }

	    (*sxplocals.SXP_tables.parsact) (UNDO, ared->action);
	}

	SS_clear (sxplocals.rcvr.undo_stack);
	sxpglobals.xps = old_xps;
    }


}



static BOOLEAN	REF_is_a_right_ctxt (bscan, bshift, head, tail, pxs, pstate, ref, look_ahead,
				     keep_stack)
    int		head, tail, *pxs;
    SXP_SHORT	*pstate, ref, look_ahead;
    BOOLEAN	bscan, bshift, keep_stack;

/* Check if tokens contained in tokens[head..tail] are a valid right context of
   stack. Assume that tail takes into account the occurrences of key-terminals.
   Predicates are tested only at places where they are supposed to return the same value as
   in normal analysis i.e. head==0 and head==1
   If keep_stack do not modify stack */
{
    SXP_SHORT			ref_orig, nt;
    int				xs = *pxs;
    struct SXP_reductions	*ared;
    SXP_SHORT			state = *pstate;
    BOOLEAN			is_rc = TRUE;

    while (head <= tail && is_rc) {
	while (!bshift) {
	    /* On sort de la boucle sur le shift suivant un scan */
	    if (ref < -sxplocals.SXP_tables.Mref) {
		int ARC_head = head + 1;
		SXP_SHORT ARC_ref = ref;

		if (ARC_head <= tail && !ARC_is_a_right_ctxt (&ARC_head, tail, &ARC_ref)) {
		    return FALSE;
		}
				   
		ref = ARC_ref;
				   
		if (ARC_head > tail) {
		    /* Verification complete */
		    if (ref < -sxplocals.SXP_tables.Mref) {
			/* mais on est reste dans l'ARC */
			ref = ARC_traversal (ref, sxplocals.atok_no + tail);
		    }
		}
	    }

	    if (ref == 0)
	    {
		/* Error */ 
		return FALSE;
	    }

	    ref_orig = ref;

	    if (ref > 0)
		/* Scan */ 
		bscan = TRUE;
	    else
		ref = -ref;

	    if ((state = ref - sxplocals.SXP_tables.Mprdct) > 0 /* Shift */ ) {
		if (++xs > lgt1)
		    oflow ();

		if (keep_stack
		    && SXBA_bit_is_reset_set (sxplocals.rcvr.modified_set, xs)) {
		    sxplocals.rcvr.st_stack [xs] = sxplocals.rcvr.stack [xs];
		}

		sxplocals.rcvr.stack [xs] = state;

		if (!bscan)
		    ref = sxP_access (sxplocals.SXP_tables.t_bases + state, look_ahead);
		else
		    bshift = TRUE; /* bscan && bshift == TRUE */
	    }
	    else if (ref > sxplocals.SXP_tables.Mred) {
		/* Predicates */
		if (sxplocals.mode.with_do_undo && sxplocals.mode.with_parsact)
		{
		    struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts + ref;
		    int old_xps;

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
		}
		else
		{
		    return FALSE;
		}
	    }
	    else {
		/* Reduce or Halt*/
		ared = sxplocals.SXP_tables.reductions + ref;

		if (ared->reduce == 0 /* Halt */ ) {
		    if (look_ahead == sxplocals.SXP_tables.P_tmax) {
			*pxs = xs, *pstate = state;
			return TRUE;
		    }

		    return FALSE;
		}

		if ((ared->reduce > sxplocals.SXP_tables.P_nbpro || ared->action >= 10000) &&
		    sxplocals.mode.with_do_undo &&
		    sxplocals.mode.with_parsact)
		{
		    /* parsact a executer immediatement */
		    /* Ca permet aux parsact de manipuler une structure // a stack en
		       utilisant les macros STACKtop(), STACKnewtop() et STACKreduce(). */
		    int		old_xps = sxpglobals.xps;
		    int		old_xs = xs;

		    sxpglobals.reduce = ared->reduce;
		    sxpglobals.pspl = ared->lgth; /* 0 */
		    sxpglobals.xps = xs;

		    if (ref > sxplocals.SXP_tables.Mrednt && ref_orig <= 0) {
			/* Reduce dans les T_tables sans Scan */
			sxpglobals.pspl--;
			sxpglobals.xps--;
		    }

		    (*sxplocals.SXP_tables.parsact) (DO, ared->action);

		    sxpglobals.xps = old_xps;

		    xs -= ared->lgth;

		    if (!keep_stack && xs < sxplocals.rcvr.min_xs)
			sxplocals.rcvr.min_xs = xs;

		    if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
			/* pas branchement direct */
			state = sxplocals.rcvr.stack [xs];
			ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, nt);
		    }

		    /* Appel recursif pour pouvoir faire UNDO. */
		    
		    is_rc = REF_is_a_right_ctxt (bscan, bshift, head, tail, &xs, &state, ref,
						 look_ahead, keep_stack);

		    if (!is_rc || keep_stack) {
			/* On fait UNDO immediatement */
			sxpglobals.reduce = ared->reduce;
			sxpglobals.pspl = ared->lgth; /* 0 */
			sxpglobals.xps = old_xs;

			if (ref > sxplocals.SXP_tables.Mrednt && ref_orig <= 0) {
			    /* Reduce dans les T_tables sans Scan */
			    sxpglobals.pspl--;
			    sxpglobals.xps--;
			}

			(*sxplocals.SXP_tables.parsact) (UNDO, ared->action);

			sxpglobals.xps = old_xps;
		    }
		    else
		    {
			/* On memorise les UNDO */
			SS_push (sxplocals.rcvr.undo_stack, ref_orig);
			SS_push (sxplocals.rcvr.undo_stack, old_xs);
		    }

		    if (is_rc) {
			*pxs = xs, *pstate = state;
			return TRUE;
		    }

		    return FALSE;
		}

		xs -= ared->lgth;

		if (!keep_stack && xs < sxplocals.rcvr.min_xs)
		    sxplocals.rcvr.min_xs = xs;

		if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
		    /* pas branchement direct */
		    state = sxplocals.rcvr.stack [xs];
		    ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, nt);
		}
	    }
	}

	/* bscan && bshift */

	if (++head <= tail) {
	    bscan = FALSE;
	    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + ++head;
	    look_ahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
	    ref = sxP_access (sxplocals.SXP_tables.t_bases + state, look_ahead);
	}
    }

    if (is_rc) {
	*pxs = xs, *pstate = state;
	return TRUE;
    }

    return FALSE;
}



static BOOLEAN	is_a_right_ctxt (head, tail, pxs, pstate, keep_stack)
    int		head, tail, *pxs;
    SXP_SHORT	*pstate;
    BOOLEAN	keep_stack;

/* Check if tokens contained in tokens[head..tail] are a valid right context of
   stack. Assume that tail takes into account the occurrences of key-terminals.
   Predicates are tested only at places where they are supposed to return the same value as
   in normal analysis i.e. head==0 and head==1
   If keep_stack do not modify stack */
{
    SXP_SHORT			ref, state, look_ahead;
    int				xs;
    BOOLEAN			is_rc;

    if (head > tail)
	return TRUE;

    xs = *pxs;
    state = *pstate;
    
    if (!keep_stack)
	sxplocals.rcvr.min_xs = xs;

    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + head;
    look_ahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
    ref = sxP_access (sxplocals.SXP_tables.t_bases + state, look_ahead);

    is_rc = REF_is_a_right_ctxt (FALSE, FALSE, head, tail, &xs, &state, ref, look_ahead,
				 keep_stack);

    if (keep_stack) {
	register int	elem = 0;

	while ((elem = sxba_scan_reset (sxplocals.rcvr.modified_set, elem)) > 0) {
	    sxplocals.rcvr.stack [elem] = sxplocals.rcvr.st_stack [elem];
	}
    }
    else
	*pxs = xs, *pstate = state;

    return is_rc;
}





/* Afin de ne pas boucler ds check_ctxt (transition vers un etat qui
   a deja ete visite par un check_ctxt ancetre du courant), on verifie
   que l'etat est nouveau, plusieurs possibilites :
   - trans_ref contient les etats interessants non encore visites par
     le chemin courant
   - trans_ref contient les etats interessants non encore visites

   1) est plus precis mais le nombre de chemins potentiels est exponentiels
   2) peut oublier des transitions mais le nombre de chemins a essayer est
      borne par le cardinal de trans_ref

   2a)trans_set est initialise a chaque appel principal
   2b)trans_set est initialise avant chaque parcour de pile

   On peut aussi limiter arbitrairement la longueur d'un chemin
   - taille fixe 3a)
   - taille dependant de la quantite depilee 3b)

   On essaye la 3b)
*/

static BOOLEAN	check_ctxt (head, tail, x, ref, trans_set, n)
    int		head, tail, *x, n;
    SXP_SHORT	ref;
    SXBA	trans_set;
{
    /* ref est une action valide depuis l'etat stack [x]
       On cherche une sequence de shifts qcq {T|NT}* suivie de t[head..tail]. */
    /* Si OK, la pile stack (et son sommet x) sont modifies, l'etat sommet
       est tel qu'il existe une transition terminale sur t [head].
       Sinon, la "stack" est inchangee. */

    SXP_SHORT		check, state, prev_state;
    int			next_action_kind;
    struct SXP_bases	*abase;
    BOOLEAN		ret_val;

#define MAX_PATH_LGTH	3

    state = (ref < 0) ? -ref : ref;

    if (state > sxplocals.SXP_tables.Mref /* ARC */
	|| (state -= sxplocals.SXP_tables.Mprdct) <= 0/* Error, Reduction or Predicates */ )
	/* On ne suit pas les chaines de reductions. Ces nouvelles reductions seront
	   examinees plus tard quand on sera au bon niveau de pile */
	/* User's predicate or &Else (assume always returning false) */
	    return FALSE;

    if ((--n + MAX_PATH_LGTH) >= 0 && SXBA_bit_is_set (trans_set, state)) {
	/* Shift */

	if (++*x > lgt1)
	    oflow ();

	prev_state = sxplocals.rcvr.stack [*x];
	sxplocals.rcvr.stack [*x] = state;
	abase = sxplocals.SXP_tables.t_bases + state;

	if (sxP_access (abase, SXGET_TOKEN (sxplocals.rcvr.TOK_0).lahead) > 0
	    /* scan sur le terminal cle */
	    && is_a_right_ctxt (head, tail, x, &state, TRUE))
	    ret_val = TRUE;
	else {
	    ret_val = FALSE;

	    set_first_trans (abase, &check, &ref,
			     sxplocals.SXP_tables.P_tmax, &next_action_kind);

	    do {
		if (check_ctxt (head, tail, x, ref, trans_set, n)) {
		    ret_val =  TRUE;
		    break;
		}
	    } while (set_next_trans (abase, &check, &ref,
				     sxplocals.SXP_tables.P_tmax, &next_action_kind));

	    if (!ret_val
		&& state + sxplocals.SXP_tables.Mprdct <= sxplocals.SXP_tables.M0ref) {
		/* Il y a des transitions non-terminales */
		abase = sxplocals.SXP_tables.nt_bases + state;
		set_first_trans (abase, &check, &ref,
			     sxplocals.SXP_tables.P_ntmax, &next_action_kind);

		do {
		    if (check_ctxt (head, tail, x, ref, trans_set, n)) {
			ret_val =  TRUE;
			break;
		    }
		} while (set_next_trans (abase, &check, &ref,
				     sxplocals.SXP_tables.P_ntmax, &next_action_kind));
	    }
	}

/* On remet les lieux dans l'etat initial */
#if 0
/* Si on choisit 1) */
	SXBA_1_bit (trans_set, REF);
#endif

	if (!ret_val) {
	    sxplocals.rcvr.stack [*x] = prev_state;
	    --*x;
	}

	return ret_val;
    }

    return FALSE;
}




static BOOLEAN	morgan (c1, c2)
    char	*c1, *c2;

/* rend vrai si c1 = c2 a une faute d'orthographe pres */

{
    register char	*tmax, *t1, *t2;
    int		l1, l2;
    char	d1, d2, e1, e2;

    switch ((l1 = strlen (c1)) - (l2 = strlen (c2))) {
    default:
	/* longueurs trop differentes */
	return FALSE;

    case 0:
	/* longueurs egales: on autorise un remplacement ou une interversion
	   de caracteres */
	tmax = c1 + l1, t1 = c1, t2 = c2;

	while (t1 < tmax && *t1 == *t2)
	    t1++, t2++;

	if (t1 == tmax)
	    /* egalite */
	    return TRUE;

	d1 = *t1++, d2 = *t2++ /* caracteres courants, differents */ ;
	e1 = *t1++, e2 = *t2++ /* caracteres suivants */ ;

	if (e1 != e2 /* caracteres suivants differents */
		     && (d1 != e2 || e1 != d2))
	    /* caracteres pas inverses */
	    return FALSE;


/* les restes des chaines doivent etre egaux */

	while (t1 < tmax && *t1 == *t2)
	    t1++, t2++;

	return t1 >= tmax /* >= pour le cas ou seuls les deux derniers caracteres sont differents */ ;

    case -1:
	tmax = c2 + l2, t1 = c2, t2 = c1;
	break;

    case 1:
	tmax = c1 + l1, t1 = c1, t2 = c2;
	break;
    }


/* longueurs egales a +- 1, t1 est la chaine la plus longue */
    /* il y a un caractere en trop dans la chaine la plus longue et ce doit
       etre la seule difference */

    while (t1 < tmax && *t1 == *t2)
	t1++, t2++;

    if (*t2 == NUL)
	/* egalite au dernier caractere pres */
	return TRUE;

    t1++ /* on saute un caractere dans la chaine la plus longue */ ;


/* les restes des chaines doivent etre egaux */

    while (t1 < tmax && *t1 == *t2)
	t1++, t2++;

    return t1 >= tmax;
}




static BOOLEAN match_a_tok (cur_mod, n, tok)
    int 	cur_mod, n, tok;
{
    /* Cette fonction regarde si le token "tok" match le nieme element de modele cur_mod */
    register int	*regle;
    register int	j;
    
    regle = sxplocals.SXP_tables.P_lregle [cur_mod];

    if (n > regle [0] || (j = regle [n]) == -1)
	return TRUE;

    if (j >= 0) {
	sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + j;

	return tok == SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
    }

    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + n - 1;

    if (sxplocals.sxtables->SXS_tables.check_keyword == NULL
	|| sxstrlen (j = SXGET_TOKEN (sxplocals.rcvr.TOK_i).string_table_entry) <= 2
	|| !sxkeywordp (sxplocals.sxtables, tok)
	|| !morgan (sxstrget (j), sxttext (sxplocals.sxtables, tok)))
	return FALSE;

    return TRUE;
}


static BOOLEAN search_a_rule (cur_mod, n, best_mod, ster)
    int 		*cur_mod, n, best_mod, *ster;
{
    /* Recherche un modele de priorite inferieure a cur_mod dont le prefixe verifie ster */

    register int 	mod, k, bot;
    register int	*regle;

    for (mod = *cur_mod + 1; mod < best_mod; mod++) {
	regle = sxplocals.SXP_tables.P_lregle [mod];
	k = regle [1];

	if (sxplocals.rcvr.is_correction_on_error_token ? k == 0 : (k != 0)) {
	    bot = sxplocals.rcvr.is_correction_on_error_token ? 1 : 0;

	    if ((k = regle [0]) > n)
		k = n;

	    for (; k > bot; k--) {
		if (!match_a_tok (mod, k, ster [k]))
		    break;
	    }

	    if (k == bot)
		break;
	}
    }

    return (*cur_mod = mod) < best_mod;
}


static BOOLEAN is_valid (cur_mod, n, ster)
    int cur_mod, n, *ster;
{
    /* Regarde si la regle de correction d'erreur cur_mod est valide par rapport
       aux don't delete et don't insert */
    register int	*regle;
    register int	j, k, lmod, head;

    regle = sxplocals.SXP_tables.P_lregle [cur_mod];

    if ((lmod = regle [0]) > n)
	lmod = n;

    head = sxplocals.SXP_tables.P_right_ctxt_head [cur_mod];
    sxba_fill (sxplocals.rcvr.to_be_checked);

    for (k = 1; k <= lmod; k++) {
	if ((j = regle [k]) >= 0) {
	    if (j >= head)
		break;

	    sxba_0_bit (sxplocals.rcvr.to_be_checked, j);
	}
	else if (sxgetbit (sxplocals.SXP_tables.P_no_insert, ster [k]))
	    return FALSE;
    }

    j = -1;

    while ((j = sxba_scan (sxplocals.rcvr.to_be_checked, j)) >= 0 && j < head) {
	sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + j;

	if (sxgetbit (sxplocals.SXP_tables.P_no_delete, SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead))
	    return FALSE;
    }

    return TRUE;
}


static SXVOID ARC_deriv (n, t_state, cur_mod)
    int	n, t_state, cur_mod;
{
    /* Les n tokens de ster valident cur_mod (et il n'existe pas de regle plus
       prioritaire que cur_mod qui les valident). On est dans l'etat t_state
       d'un ARC. */

    register struct SXP_bases	*abase = sxplocals.SXP_tables.t_bases + t_state;
    register int	k;
    SXP_SHORT		ref, test;
    int			next_action_kind, rule_no;

    set_first_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind);

    do {
	if (ref < -sxplocals.SXP_tables.Mred && ref >= -sxplocals.SXP_tables.Mprdct) {
	    /* On ne valide pas les terminaux etendus */
	    continue;
	}

	k = n;
	rule_no = cur_mod;
	sxplocals.rcvr.ARC_ster [++k] = test;

	if (match_a_tok (rule_no, k, test) ||
	    search_a_rule (&rule_no, k, sxplocals.rcvr.ARC_best.modele_no,
			   sxplocals.rcvr.ARC_ster))
	    do {
	    register int	*regle = sxplocals.SXP_tables.P_lregle [rule_no];
	    register int	lmod;
	    SXP_SHORT	next;
	    int head = regle [k + 1], tail, checked_lgth;

	    next = ref;

	    if (k < (lmod = regle [0])) {
		if (head >= sxplocals.SXP_tables.P_right_ctxt_head [rule_no]) {
		    if (!ARC_is_a_right_ctxt (&head, tail = get_tail (head, regle [lmod]), &next))
			continue;

		    checked_lgth = head - regle [k + 1];
		}
		else {
		    if ((next += sxplocals.SXP_tables.Mref) < 0) {
			/* LaScanShift */
			ARC_deriv (k, -next, rule_no);

			if (sxplocals.rcvr.ARC_best.modele_no == cur_mod)
			    return;
		    }
		    /* else sortie de l'ARC avant contexte de validation, on ne tient pas
		       compte de rule_no */
		    continue;
		}
	    }
	    else {
		tail = 0;
		checked_lgth = regle [lmod] - sxplocals.SXP_tables.P_right_ctxt_head [rule_no];
	    }

	    if (head > tail && is_valid (rule_no, k, sxplocals.rcvr.ARC_ster)) {
		/* On privilegie les modeles "completement" verifies et valides
		   a l'interieur de l'ARC */
		sxplocals.rcvr.ARC_best.modele_no = rule_no;
		sxplocals.rcvr.ARC_best.ref = next;
		sxplocals.rcvr.ARC_best.lgth = k;

		if (rule_no == cur_mod)
		    return;

		break;
	    }
	    else if (checked_lgth > sxplocals.rcvr.ARC_a_la_rigueur.checked_lgth ||
		     (checked_lgth == sxplocals.rcvr.ARC_a_la_rigueur.checked_lgth &&
		     rule_no < sxplocals.rcvr.ARC_a_la_rigueur.modele_no)) {
		/* Sinon la portion du contexte de validation verifiee est preponderante */
		sxplocals.rcvr.ARC_a_la_rigueur.modele_no = rule_no;
		sxplocals.rcvr.ARC_a_la_rigueur.ref = next;
		sxplocals.rcvr.ARC_a_la_rigueur.lgth = k;
		sxplocals.rcvr.ARC_a_la_rigueur.checked_lgth = checked_lgth;
	    }
	} while (search_a_rule (&rule_no, k, sxplocals.rcvr.ARC_best.modele_no,
				sxplocals.rcvr.ARC_ster));
    } while (set_next_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind) &&
	     ref != 0);
}


static VOID	deriv ();

static VOID new_ref (k, n, xstd, ref, test, t_state, rule_no)
    int		k, n, xstd, rule_no;
    SXP_SHORT	ref, test, t_state;
{
    struct SXP_reductions	*ared;
    SXP_SHORT			state, ref_orig;
    struct SXP_bases		*abase = sxplocals.SXP_tables.t_bases + t_state;
    SXP_SHORT			j, prev_state;
    BOOLEAN			scan;

    j = xstd;

 NEW_REF:    
    if (ref < -sxplocals.SXP_tables.Mref) {
	/* On rentre dans un ARC, comme il decrit un surlangage, il ne peut pas
	   etre utilise pour valider une correction. On l'utilise pour retourner
	   une action syntaxique (shift ou reduce) -- elle peut etre incorrecte --
	   qui sera elle meme utilisee pour une vraie validation */

	int	i, cur_mod;
	BOOLEAN	store_is_correction_on_error_token;

	for (i = 1; i <= n; i++)
	    sxplocals.rcvr.ARC_ster [i] = sxplocals.rcvr.ster [i];

	sxplocals.rcvr.ARC_ster [i] = test;
	sxplocals.rcvr.ARC_a_la_rigueur.modele_no =
	    sxplocals.rcvr.ARC_best.modele_no = sxplocals.SXP_tables.P_min_prio_0 + 1;
	sxplocals.rcvr.ARC_a_la_rigueur.checked_lgth = 0;
	store_is_correction_on_error_token = sxplocals.rcvr.is_correction_on_error_token;
	sxplocals.rcvr.is_correction_on_error_token = TRUE;
	cur_mod = rule_no;

	if (!match_a_tok (cur_mod, i, test) &&
	    !search_a_rule (&cur_mod, i, sxplocals.rcvr.ARC_best.modele_no,
			    sxplocals.rcvr.ARC_ster))
	    return;

	ARC_deriv (i, -ref - sxplocals.SXP_tables.Mref, cur_mod);
	sxplocals.rcvr.is_correction_on_error_token = store_is_correction_on_error_token;

	if (sxplocals.rcvr.ARC_best.modele_no > sxplocals.SXP_tables.P_min_prio_0 &&
	    sxplocals.rcvr.ARC_a_la_rigueur.modele_no <= sxplocals.SXP_tables.P_min_prio_0) {
	    sxplocals.rcvr.ARC_best.modele_no = sxplocals.rcvr.ARC_a_la_rigueur.modele_no;
	    sxplocals.rcvr.ARC_best.ref = sxplocals.rcvr.ARC_a_la_rigueur.ref;
	    sxplocals.rcvr.ARC_best.lgth = sxplocals.rcvr.ARC_a_la_rigueur.lgth;
	}

	if (sxplocals.rcvr.ARC_best.modele_no <= sxplocals.SXP_tables.P_min_prio_0) {
	    if (sxplocals.rcvr.ARC_best.ref + sxplocals.SXP_tables.Mref >= 0)
		ref = sxplocals.rcvr.ARC_best.ref;
	    else {
		ref = ARC_traversal (sxplocals.rcvr.ARC_best.ref,
				     sxplocals.atok_no + sxplocals.SXP_tables.P_lregle [sxplocals.rcvr.ARC_best.modele_no] [sxplocals.rcvr.ARC_best.lgth]);
	    }
	}
	else {
	    /* La correction a echouee => rattrapage global en LookAhead */
	    ref = sxplocals.SXP_tables.germe_to_gr_act [t_state];
	}
    }

    ref_orig = ref;

    if ( (scan = (ref > 0))	/* scan */ ) {
	if (n == 1 && sxplocals.rcvr.is_correction_on_error_token)
	    sxba_1_bit (sxplocals.rcvr.vt_set, test);

	sxplocals.rcvr.ster [++k] = test;
    }
    else
	ref = -ref;

    if ((state = ref - sxplocals.SXP_tables.Mprdct) > 0 /* Shift */ ) {
	if (++j > lgt1)
	    oflow ();

	prev_state = sxplocals.rcvr.stack [j];
	sxplocals.rcvr.stack [j] = state;

	deriv (k > n, k, j, state, rule_no);

	sxplocals.rcvr.stack [j] = prev_state;
    }
    else if (ref > sxplocals.SXP_tables.Mred) {
	/* predicates */ 
	if (sxplocals.mode.with_do_undo && sxplocals.mode.with_parsact)
	{
	    struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts + ref;
	    int old_xps;

	    /* Au cas ou le predicat est associe' a un nt et il veut re'fe'rencer
	       une structure // a` stack. Il faut utiliser STACKtop () */
	    old_xps = sxpglobals.xps;
	    sxpglobals.xps = j; 

	    while (aprdct->prdct >= 0 /* User's predicate */  &&
		   (!sxplocals.mode.with_parsprdct ||
		    !(*sxplocals.SXP_tables.parsact) (PREDICATE, aprdct->prdct)))
		/* returning False */ 
		aprdct++;

	    sxpglobals.xps = old_xps;
	    ref = aprdct->action;

	    goto NEW_REF;
	}
	else
	    if (n == 1 && sxplocals.rcvr.is_correction_on_error_token)
		/* On ne valide pas les terminaux etendus */
		sxplocals.rcvr.has_prdct = TRUE;
    }
    else if (ref > 0		/* Reduce or Halt */ ) {
	ared = sxplocals.SXP_tables.reductions + ref;

	if (ared->reduce == 0	/* Halt */ ) {
	    int cur_mod;

	    if (n == 1 && sxplocals.rcvr.is_correction_on_error_token)
		sxba_1_bit (sxplocals.rcvr.vt_set, test); /* test == sxplocals.SXP_tables.P_tmax*/

	    for (k++; k <= sxplocals.SXP_tables.P_maxlregle; k++) {
		sxplocals.rcvr.ster [k] = sxplocals.SXP_tables.P_tmax;
	    }

	    /* deriv suppose que rule_no est verifie jusqu'a n-1, ce n'est pas le
	       cas ici => appel de search_a_rule */
	    cur_mod = rule_no - 1;

	    if (search_a_rule (&cur_mod, sxplocals.SXP_tables.P_maxlregle,
			       sxplocals.rcvr.nomod, sxplocals.rcvr.ster))
		deriv (TRUE, sxplocals.SXP_tables.P_maxlregle, j - ared->lgth, 0, cur_mod);
	}
	else			/* Reduce */
	    if (test == 0 || scan || !reduce_already_seen (abase, test, ref)) {
		register int	nt, new_ref_state;

		if ((ared->reduce > sxplocals.SXP_tables.P_nbpro || ared->action >= 10000) &&
		    sxplocals.mode.with_do_undo &&
		    sxplocals.mode.with_parsact)
		{
		    /* parsact a executer immediatement */
		    /* Ca permet aux parsact de manipuler une structure // a stack en
		       utilisant les macros STACKtop(), STACKnewtop() et STACKreduce(). */
		    int		old_xps = sxpglobals.xps;
		    int		old_xs = j;

		    sxpglobals.reduce = ared->reduce;
		    sxpglobals.pspl = ared->lgth; /* 0 */
		    sxpglobals.xps = old_xs;

		    if (ref > sxplocals.SXP_tables.Mrednt && ref_orig <= 0) {
			/* Reduce dans les T_tables sans Scan */
			sxpglobals.pspl--;
			sxpglobals.xps--;
		    }

		    (*sxplocals.SXP_tables.parsact) (DO, ared->action);

		    sxpglobals.xps = old_xps;

		    j -= ared->lgth;

		    new_ref_state = sxplocals.rcvr.stack [j];

		    if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
			/* pas branchement direct */
			ref = sxP_access (sxplocals.SXP_tables.nt_bases + new_ref_state, nt);
		    }

		    /* Appel recursif pour pouvoir faire UNDO. */
		    new_ref (k, n, j, ref, test, t_state, rule_no);

		    /* On fait UNDO */
		    sxpglobals.reduce = ared->reduce;
		    sxpglobals.pspl = ared->lgth; /* 0 */
		    sxpglobals.xps = old_xs;

		    if (ref > sxplocals.SXP_tables.Mrednt && ref_orig <= 0) {
			/* Reduce dans les T_tables sans Scan */
			sxpglobals.pspl--;
			sxpglobals.xps--;
		    }

		    (*sxplocals.SXP_tables.parsact) (UNDO, ared->action);

		    sxpglobals.xps = old_xps;

		    return;
		}

		j -= ared->lgth;
		
		new_ref_state = sxplocals.rcvr.stack [j];
		    
		if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
		    /* pas branchement direct */
		    ref = sxP_access (sxplocals.SXP_tables.nt_bases + new_ref_state, nt);
		}

		goto NEW_REF;
	    }
	/* else reduction deja examinee, on ne recommence pas... */
    }
}



static VOID	deriv (string_has_grown, n, xstd, t_state, rule_no)
    int		n, xstd, rule_no;
    SXP_SHORT	t_state;
    BOOLEAN	string_has_grown;

/* fabrique les corrections locales; place ses resultats dans "nomod" (numero
   du modele) et "lter" (nouveau flot de tokens) */
/* Suppose que t_state designe une T_table */

{
    if (string_has_grown) {
	BOOLEAN must_return = TRUE;

	if (match_a_tok (rule_no, n, sxplocals.rcvr.ster [n]) ||
	    search_a_rule (&rule_no, n, sxplocals.rcvr.nomod, sxplocals.rcvr.ster))
	    do {
	    register int	*regle = sxplocals.SXP_tables.P_lregle [rule_no];
	    register int	head, lmod;
	    register int 	*best_str, i;

	    if (n < (lmod = regle [0])) {
		if ((head = regle [n + 1]) >= sxplocals.SXP_tables.P_right_ctxt_head [rule_no]) {
		    if (!is_a_right_ctxt (head, get_tail (head, regle [lmod]),
					  &xstd, &t_state, TRUE))
			continue;
		}
		else {
		    must_return = FALSE;
		    break;
		}
	    }

	    /* Here the model rule_no has been checked */
	    if (is_valid (rule_no, n, sxplocals.rcvr.ster)) {
		sxplocals.rcvr.nomod = rule_no;
		best_str = sxplocals.rcvr.lter;
	    }
	    else if (rule_no < sxplocals.rcvr.a_la_rigueur.modele_no) {
		sxplocals.rcvr.a_la_rigueur.modele_no = rule_no;
		best_str = sxplocals.rcvr.a_la_rigueur.correction;
	    }
	    else return;

	    for (i = 1; i <= n; i++) {
		best_str [i] = sxplocals.rcvr.ster [i];
	    }

	    for (; i <= lmod; i++) {
		sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + regle [i];
		best_str [i] = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
	    }

	    return;
	} while (search_a_rule (&rule_no, n, sxplocals.rcvr.nomod, sxplocals.rcvr.ster));

	if (must_return)
	    return;
    }

    /* Here the string ster [1:n] matches the prefix of length n of model rule_no */

    {
	register struct SXP_bases	*abase = sxplocals.SXP_tables.t_bases + t_state;
	SXP_SHORT			ref, test;
	int				next_action_kind;

	set_first_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind);

	do {
	    new_ref (n, n, xstd, ref, test, t_state, rule_no);
	} while (set_next_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax,
				 &next_action_kind));
    }
}



static BOOLEAN	ARC_is_a_correction (t_state)
    SXP_SHORT	t_state;

/* rend vrai si on peut trouver une correction locale */

{
    sxplocals.rcvr.ARC_ster [1] = (SXGET_TOKEN (sxplocals.rcvr.TOK_0)).lahead;
    sxplocals.rcvr.ARC_a_la_rigueur.modele_no = sxplocals.rcvr.ARC_best.modele_no = sxplocals.SXP_tables.P_min_prio_0 + 1;
    sxplocals.rcvr.ARC_a_la_rigueur.checked_lgth = 0;
    sxplocals.rcvr.is_correction_on_error_token = TRUE;
    ARC_deriv (1, t_state, 1);

    if (sxplocals.rcvr.ARC_best.modele_no > sxplocals.SXP_tables.P_min_prio_0) {
	if (sxplocals.rcvr.ARC_a_la_rigueur.modele_no > sxplocals.SXP_tables.P_min_prio_0)
	    return FALSE;

	sxplocals.rcvr.ARC_best.modele_no = sxplocals.rcvr.ARC_a_la_rigueur.modele_no;
	sxplocals.rcvr.ARC_best.ref = sxplocals.rcvr.ARC_a_la_rigueur.ref;
	sxplocals.rcvr.ARC_best.lgth = sxplocals.rcvr.ARC_a_la_rigueur.lgth;
    }

    return TRUE;
}



static BOOLEAN	is_a_correction (force_correction_on_error_token, xs, state)
    BOOLEAN	force_correction_on_error_token;
    int		xs;
    SXP_SHORT	state;

/* rend vrai si on peut trouver une correction locale */
/* state est un t_state */

{
    register int	i, l;
    int		xs2 = xs;
    SXP_SHORT	state2 = state;

    sxplocals.rcvr.a_la_rigueur.modele_no = sxplocals.SXP_tables.P_nbcart + 1;
    sxba_empty (sxplocals.rcvr.vt_set);
    sxplocals.rcvr.has_prdct = FALSE;

    if (!force_correction_on_error_token && !is_a_right_ctxt (0, 0, &xs2, &state2, FALSE))
    {
	/* Prdct on previous token */
	undo ();
	return FALSE;
    }

    sxplocals.rcvr.ster [1] = (SXGET_TOKEN (sxplocals.rcvr.TOK_0)).lahead;
    sxplocals.rcvr.nomod = sxplocals.SXP_tables.P_min_prio_0 + 1;
    sxplocals.rcvr.is_correction_on_error_token = TRUE;
    deriv (TRUE, 1, xs2, state2, 1);

    if (sxplocals.rcvr.nomod > sxplocals.SXP_tables.P_min_prio_0)
	sxplocals.rcvr.nomod = sxplocals.SXP_tables.P_nbcart + 1;

    if (!force_correction_on_error_token) {
	restaure_stack (sxplocals.rcvr.min_xs + 1, xs);
	undo ();
    }


/* On regarde s'il existe un modele plus prioritaire que nomod pouvant
   corriger le terminal precedant le terminal en erreur. */

    if (!force_correction_on_error_token &&
	sxplocals.SXP_tables.P_max_prio_X < sxplocals.rcvr.nomod) {
	sxplocals.rcvr.is_correction_on_error_token = FALSE;
	deriv (FALSE, 0, xs, state, sxplocals.SXP_tables.P_max_prio_X);
    }

    if (sxplocals.rcvr.nomod > sxplocals.SXP_tables.P_nbcart) {
	if (sxplocals.rcvr.a_la_rigueur.modele_no > sxplocals.SXP_tables.P_nbcart)
	    return FALSE;

	l = sxplocals.SXP_tables.P_lregle [sxplocals.rcvr.nomod = sxplocals.rcvr.a_la_rigueur.modele_no] [0];

	for (i = 1; i <= l; i++)
	    sxplocals.rcvr.lter [i] = sxplocals.rcvr.a_la_rigueur.correction [i];
    }

    undo ();

    return TRUE;
}




/* E R R O R   R E C O V E R Y */

static BOOLEAN	ARC_recovery (at_state, latok_no)
    int	*at_state, latok_no;
{
    /* Recuperation d'erreur en LookAhead
       - Pas de message
       - Ne pas aller au-dela de la limite d'un ARC
       - Ne pas modifier la token_string
       - Ne pas tenter de correction sur le token precedent le token en erreur

       Rend (dans at_state) une "ref" (une des actions syntaxiques choisis par l'ARC) */

/* L'appel a sxget_token doit s'effectuer avant tout positionnement de variables statiques
   car un appel recursif par l'intermediaire d'une action du scanner appele par
   sxget_token. */
/* accumulation de tokens en avance */
    sxget_token (latok_no + sxplocals.SXP_tables.P_nmax);

    sxplocals.rcvr.TOK_0 = latok_no - 1;

    if (ARC_is_a_correction (*at_state)) {
	if (sxplocals.rcvr.ARC_best.ref + sxplocals.SXP_tables.Mref >= 0)
	    *at_state = sxplocals.rcvr.ARC_best.ref;
	else
	    *at_state = ARC_traversal (sxplocals.rcvr.ARC_best.ref, latok_no + sxplocals.SXP_tables.P_lregle [sxplocals.rcvr.ARC_best.modele_no] [sxplocals.rcvr.ARC_best.lgth] - 1);
    }
    else {
	/* La correction a echouee => rattrapage global en LookAhead */
	*at_state = sxplocals.SXP_tables.germe_to_gr_act [*at_state];

	if (sxplocals.mode.kind == SXWITH_CORRECTION)
	    /* Seule une tentative de correction a ete demandee. */
	    return FALSE;
    }

    return TRUE;
}

static BOOLEAN	recovery ()
{
    char				*msg_params [5];
    register int			i, j, x, l;
    register int			*regle;
    register struct SXP_local_mess	*local_mess;
    int					head, tail, im, k, ll, vt_set_card, x1, y, y1, xps, next_action_kind, xs2;
    SXP_SHORT				check, state, ref;
    BOOLEAN				is_warning, no_correction_on_previous_token;
    struct sxtoken			*p;
    struct sxsource_coord		source_index;

    if ((no_correction_on_previous_token = (sxplocals.ptok_no == sxplocals.atok_no))) {
	/* On suppose qu'on n'est jamais en debut de tokens_buf! */
	sxplocals.atok_no--;
    }

/* accumulation de tokens en avance */
/* L'appel a sxget_token doit s'effectuer avant tout positionnement de variables statiques
   car un appel recursif par l'intermediaire d'une action du scanner appele par
   sxget_token. */
    sxget_token (sxplocals.atok_no + sxplocals.SXP_tables.P_nmax);


/* L'erreur sera marquee (listing) a l'endroit de la detection */
    source_index = SXGET_TOKEN (sxplocals.ptok_no).source_index;
    is_warning = TRUE;
    sxplocals.rcvr.TOK_0 = sxplocals.atok_no;
    state = sxplocals.state;
    xps = sxpglobals.xps;

    while (lgt1 <= xps)
	oflow ();

    restaure_stack (sxpglobals.stack_bot + 1, xps);

    if (!is_a_correction (no_correction_on_previous_token, xps, state)) {
	/* correction a echouee */
	if (sxplocals.mode.kind == SXWITH_CORRECTION)
	    /* Seule une tentative de correction a ete demandee. */
	    return FALSE;

	goto riennevaplus;
    }


/* OK ca a marche */

    im = 0;
    regle = sxplocals.SXP_tables.P_lregle [sxplocals.rcvr.nomod];
    local_mess = sxplocals.SXP_tables.SXP_local_mess + sxplocals.rcvr.nomod;
    ll = regle [0];


/* preparation du message d'erreur */

    for (j = ll; j > 0; j--) {
	if (regle [j] > im)
	    im = regle [j];
    }

    for (j = local_mess->P_param_no - 1; j >= 0; j--) {
	register struct sxtoken		*tok;
	register int ate;

	i = local_mess->P_param_ref [j];

	if (i < 0) {
	  /* X or S */
	  msg_params [j] = ttext (sxplocals.sxtables, sxplocals.rcvr.lter [-i]);
	}
	else {
	  tok = (sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + i, &SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	  msg_params [j] = ((ate = tok->string_table_entry) == EMPTY_STE || ate == ERROR_STE
			    ? ttext (sxplocals.sxtables, tok->lahead)
			    : sxstrget (ate));
	}
#if 0
	/* Le 13/05/03 */
	tok = (sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + i,
	       &SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	msg_params [j] = i < 0 /* X or S */
			       ? ttext (sxplocals.sxtables, sxplocals.rcvr.lter [-i])
			       : ((ate = tok->string_table_entry) == EMPTY_STE || ate == ERROR_STE
				  ? ttext (sxplocals.sxtables, tok->lahead)
				  : sxstrget (ate));
#endif
    }

    /* On doit retrouver NULL ds le token, meme si les commentaires
       ne sont pas gardes. */
    for (y1 = 1; y1 <= ll; y1++)
	sxplocals.rcvr.com [y1] = NULL;

    if (!sxsvar.SXS_tables.S_are_comments_erased) {

/* deplacement des commentaires */

	x = im;

	for (y = ll; regle [y] == x && x >= 0; y--) {
	    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + x--;
	    sxplocals.rcvr.com [y] = SXGET_TOKEN (sxplocals.rcvr.TOK_i).comment;
	}

	if (ll < ++y)
	    y = ll;

	y1 = 1;

	for (x1 = 0; x1 <= x && y1 < y; x1++) {
	    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + x1;
	    sxplocals.rcvr.com [y1++] = SXGET_TOKEN (sxplocals.rcvr.TOK_i).comment;
	}

	for (x1 = x1; x1 <= x; x1++) {
	    register struct sxtoken	*tok;

	    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + x1;
	    tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));

	    if (sxplocals.rcvr.com [y] == NULL)
		sxplocals.rcvr.com [y] = tok->comment;
	    else if (tok->comment != NULL) {
		/* concatenation des commentaires */
		char	*tcom = tok->comment;

		sxplocals.rcvr.com [y] = sxrealloc (sxplocals.rcvr.com [y], strlen (sxplocals.rcvr.com [y]) + strlen (tcom) + 1, sizeof (char));
		strcat (sxplocals.rcvr.com [y], tcom);
		sxfree (tcom);
	    }
	}
    }


/* deplacement des source_index
   principe:
   si un token se retrouve dans la chaine corrigee, il conserve son
   source_index, cela dans tous les cas (echange, duplication, ..)
   s'il correspond a un remplacement, il prend le source_index du
   token qu'il remplace
   s'il correspond a une insertion, il prend le source_index du token
   qui le suit.
*/

    x = 0;
    j = sxplocals.SXP_tables.P_right_ctxt_head [sxplocals.rcvr.nomod];

    for (l = 1, p = sxplocals.rcvr.toks; (i = regle [l]) < j; l++, p++) {
	if (i < 0) {
	    p->lahead = sxplocals.rcvr.lter [l];
	    p->string_table_entry = ERROR_STE /* error value */ ;
	    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + x;
	    p->source_index = SXGET_TOKEN (sxplocals.rcvr.TOK_i).source_index;

	    if (sxgenericp (sxplocals.sxtables, sxplocals.rcvr.lter [l]))
		is_warning = FALSE;
	}
	else {
	    *p = (sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + i, SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	    x = i + 1;
	}

	p->comment = sxplocals.rcvr.com [l];
	/* et voila */
    }

exit_of_er_re:
    sxtknmdf (sxplocals.rcvr.toks, l - 1, sxplocals.atok_no, j);
    sxplocals.ptok_no = sxplocals.atok_no += (no_correction_on_previous_token ? 1 : 0);

/* "Warning: "machin" is replaced by chose." ou similaire */

    if (!sxplocals.mode.is_silent)
	sxput_error (source_index, local_mess->P_string, is_warning ?
		     sxplocals.sxtables->err_titles [1] :
		     sxplocals.sxtables->err_titles [2],
		     msg_params [0],
		     msg_params [1],
		     msg_params [2],
		     msg_params [3],
		     msg_params [4]);

    return TRUE;


/* la correction locale a echoue */

riennevaplus:
    if ((vt_set_card = sxba_cardinal (sxplocals.rcvr.vt_set)) == 1
	&& !sxplocals.rcvr.has_prdct
	&& (k = sxba_scan (sxplocals.rcvr.vt_set, 0)) != sxplocals.SXP_tables.P_tmax) {
	/* un seul suivant valide different de EOF ==> insertion forcee */
	/* Pas de predicats dans le coup */
	local_mess = sxplocals.SXP_tables.SXP_local_mess + sxplocals.SXP_tables.P_nbcart + 1;

	/* preparation du message */
	
	for (j = local_mess->P_param_no - 1; j >= 0; j--) {
	    register struct sxtoken	*tok;
	    register int ate;

	    i = local_mess->P_param_ref [j];
	    tok = (sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + i,
		   &SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	    msg_params [j] = i < 0 /* assume %0 */
				   ? ttext (sxplocals.sxtables, k)
				   : ((ate = tok->string_table_entry) == EMPTY_STE || ate == ERROR_STE
				      ? ttext (sxplocals.sxtables, tok->lahead)
				      : sxstrget (ate));
	}


/* insertion du nouveau token, avec source_index et comment */

	sxplocals.rcvr.toks [0] = (SXGET_TOKEN (sxplocals.rcvr.TOK_0));
	p = sxplocals.rcvr.toks + 1;
	p->lahead = k;
	p->string_table_entry = ERROR_STE /* error value */ ;
	p->comment = NULL;
	p->source_index = source_index;
	l = 3, j = 1;

	if (sxgenericp (sxplocals.sxtables, k))
	    is_warning = FALSE;

	goto exit_of_er_re;
    }


/* rattrapage global */

/* message sur les suivants attendus */

    if (((unsigned int) (vt_set_card)) <= (unsigned)sxplocals.SXP_tables.P_followers_number /* a < x <= b */  && !sxplocals.rcvr.has_prdct) {
	i = 0;
	j = 0;

	while ((j = sxba_scan (sxplocals.rcvr.vt_set, j)) > 0) {
	    msg_params [i++] = ttext (sxplocals.sxtables, j);
	}


/* "Error: "machin" is expected." */

	if (!sxplocals.mode.is_silent)
	    sxput_error (source_index,
			 sxplocals.SXP_tables.P_global_mess [vt_set_card - 1],
			 sxplocals.sxtables->err_titles [2],
			 msg_params [0],
			 msg_params [1],
			 msg_params [2],
			 msg_params [3],
			 msg_params [4]);
    }


/* "Error: Global Recovery." */

    if (!sxplocals.mode.is_silent)
	sxput_error (source_index,
		     sxplocals.SXP_tables.P_global_mess [sxplocals.SXP_tables.P_followers_number],
		     sxplocals.sxtables->err_titles [2]);

/* on y va */
/* On utilise parse_stack car les actions semantiques ne connaissent qu'elle. */

    for (;;) {
	sxget_token (sxplocals.atok_no + sxplocals.SXP_tables.P_validation_length);

/* avaler un token */
	sxplocals.rcvr.TOK_0 = ++sxplocals.atok_no;
	p = &(SXGET_TOKEN (sxplocals.rcvr.TOK_0));

	if (p->lahead == sxplocals.SXP_tables.P_tmax) {
	    /* on a trouve la fin du fichier */
	    /* "Error: Parsing stops on End of File." */
	    if (!sxplocals.mode.is_silent)
		sxput_error (p->source_index,
			     sxplocals.SXP_tables.P_global_mess
			        [sxplocals.SXP_tables.P_followers_number + 3],
			     sxplocals.sxtables->err_titles [2]);
	    /* On simule une reduction entre top = sxpglobals.xps - 1 (l'ancien sommet de
	       pile est non significatif en tant que token) et new_top = sxpglobals.stack_bot + 1
	       pour les actions */
	    sxpglobals.xps-- /* pour STACKtop */;
	    sxpglobals.pspl = sxpglobals.xps - sxpglobals.stack_bot - 1 /* pour STACKnew_top */ ;
	    (*(sxplocals.SXP_tables.semact)) (ERROR, 1);
	    sxplocals.ptok_no = sxplocals.atok_no;
	    return FALSE;
	}

	if ((i = sxgetbit (sxplocals.SXP_tables.PER_tset, p->lahead)) > 0) {
	    /* terminal courant == terminal cle */
	    /* on regarde si les P_validation_length terminaux suivants
	       sont des suivants valides, puis sinon on "pop" la pile et
	       on recommence; si on tombe au fond de la pile on avance
	       d'un token */
	    SXBA trans = sxplocals.SXP_tables.PER_trans_sets != NULL
		? sxplocals.SXP_tables.PER_trans_sets [i]
		    : NULL;

	    head = 0;
	    tail = get_tail (0, sxplocals.SXP_tables.P_validation_length - 1);

	    for (xps = sxpglobals.xps; xps > sxpglobals.stack_bot; xps--) {
		/* pop the stack and check the right context */
		register struct SXP_bases	*abase;
		BOOLEAN			is_checked;

		ref = state = sxpglobals.parse_stack [xs2 = xps].state;
		abase = sxplocals.SXP_tables.t_bases + state;

		if (sxP_access (abase, p->lahead) > 0 /* scan sur le terminal cle */
		    && is_a_right_ctxt (head, tail, &xs2, &ref, TRUE))
		    break;

		if (trans != NULL && SXBA_bit_is_set (trans, state)) {
		    /* state est un candidat */

#if 0
		    sxba_copy (sxplocals.rcvr.trans_set, trans); */
#endif

		    /* On regarde les transitions terminales... */
		    set_first_trans (abase, &check, &ref,
				     sxplocals.SXP_tables.P_tmax, &next_action_kind);

		    while ((!(is_checked = check_ctxt (head, tail, &xs2, ref, trans, sxpglobals.xps - xps)))
			     && set_next_trans (abase, &check, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind));

		    if (is_checked)
			break;

		    if (state <= sxplocals.SXP_tables.M0ref - sxplocals.SXP_tables.Mprdct) {
			/* Il y a des transitions non-terminales */
			abase = sxplocals.SXP_tables.nt_bases + state;
			set_first_trans (abase, &check, &ref,
					 sxplocals.SXP_tables.P_ntmax, &next_action_kind);

			while ((!(is_checked = check_ctxt (head, tail, &xs2, ref, trans, sxpglobals.xps - xps)))
				 && set_next_trans (abase, &check, &ref, sxplocals.SXP_tables.P_ntmax, &next_action_kind)); 

			if (is_checked)
			    break;
		    }
		}
	    }

	    if (xps > sxpglobals.stack_bot)
		break;
	}
    }

    /* Les "nouveaux" etats sont dans stack [xps+1 : xs2]
       Les etats a remplacer sont dans parse_stack [xps+1 : sxpglobals.xps]
       i.e. sxpglobals.xps - xps anciens etats sont remplaces par xs2 - xps nouveaux. */
       
#if 1
    /* Essai le 18/10/04 */
    /* L'ancienne version plantait ds un atc car on essayait un "add_list_node" sur un noeud liste
       qui n'existait pas et correspondait au terminal qui se trouvait en xps */
    /* La modif ci-dessous a l'air de marcher mais je ne sais pas vraiment pourquoi !! */
    sxpglobals.pspl = sxpglobals.xps - xps;
    (*(sxplocals.SXP_tables.semact)) (ERROR, xs2 - xps);

    /* reajustement de la pile */
    while (xps <= xs2) {
	sxpglobals.parse_stack [xps].state = sxplocals.rcvr.stack [xps];
	sxpglobals.parse_stack [xps].token.lahead = 0 /* simulation of a NT */ ;
	xps++;
    }
#else
    sxpglobals.pspl = sxpglobals.xps - xps - 1 /* pour STACKnewtop() eventuel */ ;
    (*(sxplocals.SXP_tables.semact)) (ERROR, xs2 - xps);

    /* reajustement de la pile */
    while (++xps <= xs2) {
	sxpglobals.parse_stack [xps].state = sxplocals.rcvr.stack [xps];
	sxpglobals.parse_stack [xps].token.lahead = 0 /* simulation of a NT */ ;
    }
#endif /* 0 */

    sxpglobals.xps = xs2;
    sxplocals.state = sxplocals.rcvr.stack [xs2];

    {
	int	ate;

	if ((i = sxplocals.SXP_tables.P_validation_length) > 5) {
	    i = 5;
	}

	while (i-- > 0) {
	    p = (sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + i,
		 &SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	    msg_params [i] = (ate = p->string_table_entry) == EMPTY_STE || ate == ERROR_STE
		? ttext (sxplocals.sxtables, p->lahead)
		: sxstrget (ate);
	}


/* "Warning: Parsing resumes on "machin"." */

	if (!sxplocals.mode.is_silent)
	    sxput_error ((SXGET_TOKEN (sxplocals.rcvr.TOK_0)).source_index,
			 sxplocals.SXP_tables.P_global_mess
			    [sxplocals.SXP_tables.P_followers_number + 1],
			 sxplocals.sxtables->err_titles [1],
			 msg_params [0],
			 msg_params [1],
			 msg_params [2],
			 msg_params [3],
			 msg_params [4]);
    }

    sxplocals.ptok_no = sxplocals.atok_no;
    return TRUE;
}




BOOLEAN		sxprecovery (what_to_do, at_state, latok_no)
    int		what_to_do, *at_state, latok_no;
{
    switch (what_to_do) {
    case OPEN:
	/* Allocation will be done at first call. */
	break;

    case ACTION:
    case ERROR:
	sxplocals.rcvr.truncate_context = TRUE;

	if (sxplocals.rcvr.stack == NULL) {
	    int   sizofpts = sxplocals.SXP_tables.P_sizofpts;

	    /* Tant pis, on ne partage pas stack, st_stack et modified_set
	       entre differents langages... */
	    /* Il faudrait pouvoir appeler le point d'entree END pour les desallouer
	       en fin de process or sxprecovery est lie' aux tables du parser
	       or le point d'entree END du parser est independant de ces tables. */
	    sxplocals.rcvr.stack = (SXP_SHORT*) sxalloc ((lgt1 = 200) + 1, sizeof (SXP_SHORT));
	    sxplocals.rcvr.st_stack = (SXP_SHORT*) sxalloc (lgt1 + 1, sizeof (SXP_SHORT));
	    sxplocals.rcvr.modified_set = sxba_calloc (lgt1 + 1);
	    
	    sxplocals.rcvr.com = (char**) sxalloc (sizofpts + 1, sizeof (char*));
	    sxplocals.rcvr.ARC_ster = (int*) sxalloc (sizofpts + 1, sizeof (int));
	    sxplocals.rcvr.ster = (int*) sxalloc (sizofpts + 1, sizeof (int));
	    sxplocals.rcvr.lter = (int*) sxalloc (sizofpts + 1, sizeof (int));
	    sxplocals.rcvr.toks = (struct sxtoken*) sxalloc (sizofpts + 1, sizeof (struct sxtoken));
	    sxplocals.rcvr.a_la_rigueur.correction = (int*) sxalloc (sizofpts + 1, sizeof (int));
	    sxplocals.rcvr.vt_set = sxba_calloc (sxplocals.SXP_tables.P_tmax + 1);
#if 0
	    sxplocals.rcvr.trans_set = sxba_calloc (sxplocals.SXP_tables.Mref - sxplocals.SXP_tables.Mprdct + 1);
#endif
	    sxplocals.rcvr.to_be_checked = sxba_calloc (sxplocals.SXP_tables.P_nmax + 1);
	    sxplocals.rcvr.mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.mxvec;
	    sxplocals.rcvr.Mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.Mxvec;
	    sxplocals.rcvr.undo_stack = SS_alloc (50);
	}

	return (what_to_do == ACTION) ? recovery () : ARC_recovery (at_state, latok_no);

    case CLOSE:
	if (sxplocals.rcvr.stack != NULL) {
	    sxfree (sxplocals.rcvr.stack), sxplocals.rcvr.stack = NULL;
	    sxfree (sxplocals.rcvr.st_stack);
	    sxfree (sxplocals.rcvr.modified_set);

	    sxfree (sxplocals.rcvr.com);
	    sxfree (sxplocals.rcvr.ARC_ster);
	    sxfree (sxplocals.rcvr.ster);
	    sxfree (sxplocals.rcvr.lter);
	    sxfree (sxplocals.rcvr.toks);
	    sxfree (sxplocals.rcvr.a_la_rigueur.correction);
	    sxfree (sxplocals.rcvr.vt_set);
#if 0
	    sxfree (sxplocals.rcvr.trans_set);
#endif
	    sxfree (sxplocals.rcvr.to_be_checked);
	    SS_free (sxplocals.rcvr.undo_stack);
	}

	break;

    default:
	fprintf (sxstderr, "The function \"sxprecovery\" is out of date with respect to its specification.\n");
	abort ();
    }

    return TRUE;
}
