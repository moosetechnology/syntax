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
/* 13-09-94 10:26 (pb):		VERSION PROVISOIRE : les post-actions	*/
/*				sont implantees ds ared->action et sont	*/
/*				>= 10000 et donc incompatibles avec des	*/
/*				actions semantiques sur la meme regle.	*/
/************************************************************************/
/* 24-08-94 11:30 (pb):		Prise en compte de "with_do_undo"	*/
/************************************************************************/
/* 21-10-93 13:00 (phd&pb):	Adaptation a "strings.h" OSF		*/
/************************************************************************/
/* 25-02-93 11:25 (pb) :	Les variables locales sont mises dans	*/
/*				sxplocals.rcvr				*/
/************************************************************************/
/* 25-03-93 14:19 (pb):		Suppression du parametre "stack" dans	*/
/*				"is_a_right_ctxt" et "check_nt_ctxt"	*/
/*				(bug sur oflow).			*/
/************************************************************************/
/* 22-02-93 15:00 (pb):		Suppression de la table "gotos"		*/
/*				Suppression des "adresses en pile"	*/
/*				Mfe, M0ref et Mref changent de		*/
/*				signification. Ajout de "final_state"	*/
/*				Utilisation des SXPER_trans_sets	*/
/************************************************************************/
/* 24-09-92 15:55 (pb):		Adaptation au nouveau token_mngr	*/
/************************************************************************/
/* 24-04-92 10:35 (pb):		Bug sur la valeur de atok_no retournee	*/
/*				lorsque un predicat est associe au	*/
/*				token precedent.			*/
/************************************************************************/
/* 08-04-92 16:08 (pb):		Utilisation du "mode" d'analyse		*/
/************************************************************************/
/* 24-11-88 09:40 (pb):		Suppression du dernier parametre de	*/
/*				P_access				*/
/*				is_a_right_ctxt rend FALSE si predicat	*/
/************************************************************************/
/* 03-06-88 10:19 (pb):		Bug pour STACKtop et STACKnew_top dans	*/
/*				le cas "End Of File"			*/
/************************************************************************/
/* 05-05-88 11:15 (pb):		Bug recuperation globale introduit sans */
/*				doute avec la 3.0			*/
/************************************************************************/
/* 27-04-88 11:45 (pb):		Adaptation au "token manager"		*/
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


#include "sxunix.h"
#include "SS.h"

#ifndef VARIANT_32
char WHAT_SXP_SRECOVERY[] = "@(#)SYNTAX - $Id: sxp_srcvr.c 1031 2008-02-14 10:05:15Z rlacroix $" WHAT_DEBUG;
#else
char WHAT_SXP_SRECOVERY[] = "@(#)SYNTAX - $Id: sxp_srcvr.c 1031 2008-02-14 10:05:15Z rlacroix $ SXP_SRCVR_32" WHAT_DEBUG;
#endif
#define Commun 1
#define Propre 2
#define Defaut 3
#define Done   4

extern SXP_SHORT	sxP_access (struct SXP_bases *abase, SXINT j), ARC_traversal (SXP_SHORT ref, SXINT latok_no);

/*   V A R I A B L E S   */

static SXINT	lgt1;


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


static char	*ttext (struct sxtables *tables, SXINT tcode)
{
    return (tcode == sxplocals.SXP_tables.P_tmax) ? sxplocals.SXP_tables.P_global_mess [sxplocals.SXP_tables.
	 P_followers_number + 2] : sxttext (tables, tcode);
}



static VOID	oflow (void)

/* overflow of stack */

{
    lgt1 *= 2;
    sxplocals.rcvr.modified_set = sxba_resize (sxplocals.rcvr.modified_set, lgt1 + 1);
    sxplocals.rcvr.st_stack = (SXP_SHORT*) sxrealloc (sxplocals.rcvr.st_stack, lgt1 + 1, sizeof (SXP_SHORT));
    sxplocals.rcvr.stack = (SXP_SHORT*) sxrealloc (sxplocals.rcvr.stack, lgt1 + 1, sizeof (SXP_SHORT));
}




static BOOLEAN	set_next_item (struct SXP_item *vector, SXP_SHORT *check, SXP_SHORT *action, SXINT Max)
{
    struct SXP_item	*aitem;
    SXP_SHORT	val;

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



static VOID	set_first_trans (struct SXP_bases *abase, 
				 SXP_SHORT *check, 
				 SXP_SHORT *action, 
				 SXINT Max, 
				 SXINT *next_action_kind)
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



static BOOLEAN	set_next_trans (struct SXP_bases *abase, 
				SXP_SHORT *check, 
				SXP_SHORT *action, 
				SXINT Max, 
				SXINT *next_action_kind)
{
  switch (*next_action_kind) {
  case Commun:
    if (set_next_item (sxplocals.SXP_tables.vector + abase->commun, check, action, (SXINT)Max))
      return TRUE;

    if (abase->propre != 0 /* partie propre non vide */ ) {
      *next_action_kind = Propre;
      *check = 0;
      /* !!! pas de break ici : c'est voulu */

    case Propre:
       /* !!! ce "case" se trouve au milieu du "if" : c'est voulu */
      if (set_next_item (sxplocals.SXP_tables.vector + abase->propre, check, action, (SXINT)Max))
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



static SXINT	get_tail (SXINT binf, SXINT bsup)
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



static SXVOID	restaure_stack (SXINT m, SXINT M)
{
    SXINT	i;

    for (i = m; i <= M; i++) {
	/* sauvegarde de la pile */
	sxplocals.rcvr.stack [i] = sxpglobals.parse_stack [i].state;
    }
}


static BOOLEAN	ARC_is_a_right_ctxt (SXINT *head, SXINT tail, SXP_SHORT *ref)

/* Check if tokens contained in tokens[head..tail] are a valid right context of
   *ref. Assume that tail takes into account the occurrences of key-terminals.
   Predicates are supposed to return FALSE */


{
    SXP_SHORT	next = *ref;

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


static BOOLEAN	REF_is_a_right_ctxt (SXINT head, 
				     SXINT tail, 
				     SXINT *pxs, 
				     SXP_SHORT *pstate, 
				     SXP_SHORT ref, 
				     SXP_SHORT look_ahead, 
				     BOOLEAN keep_stack)

/* Check if tokens contained in tokens[head..tail] are a valid right context of
   stack. Assume that tail takes into account the occurrences of key-terminals.
   Predicates are tested only at places where they are supposed to return the same value as
   in normal analysis i.e. head==0 and head==1
   If keep_stack do not modify stack */


{
    SXP_SHORT			ref_orig, nt;
    SXINT				xs = *pxs;
    struct SXP_reductions	*ared;
    SXP_SHORT			state = *pstate;
    BOOLEAN			bscan, done = FALSE, is_rc;

    sxinitialise(is_rc); /* pour faire taire gcc -Wuninitialized */
    while (!done) {
	bscan = FALSE;

	while (!bscan) {
	    if (ref < -sxplocals.SXP_tables.Mref) {
		SXINT ARC_head = head + 1;
		SXP_SHORT ARC_ref = ref;

		if (ARC_head <= tail && !ARC_is_a_right_ctxt (&ARC_head, tail, &ARC_ref)) {
		    done = TRUE, is_rc = FALSE;
		    break;
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
		done = TRUE, is_rc = FALSE;
		break;
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
	    }
	    else if (ref > sxplocals.SXP_tables.Mred) {
		/* Predicates */
		if (sxplocals.mode.with_do_undo && sxplocals.mode.with_parsact)
		{
		    struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts + ref;
		    SXINT old_xps;

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
		    done = TRUE, is_rc = FALSE;
		    break;
		}
	    }
	    else {
		/* Reduce or Halt*/
		ared = sxplocals.SXP_tables.reductions + ref;

		if (ared->reduce == 0 /* Halt */ ) {
		    done = TRUE;
		    is_rc = (look_ahead == sxplocals.SXP_tables.P_tmax);
		    break;
		}

		if ((ared->reduce > sxplocals.SXP_tables.P_nbpro || ared->action >= 10000) &&
		    sxplocals.mode.with_do_undo &&
		    sxplocals.mode.with_parsact)
		{
		    /* parsact a executer immediatement */
		    /* Ca permet aux parsact de manipuler une structure // a stack en
		       utilisant les macros STACKtop(), STACKnewtop() et STACKreduce(). */
		    SXINT		old_xps = sxpglobals.xps;
		    SXINT		old_xs = xs;

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

		    state = sxplocals.rcvr.stack [xs];

		    if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
			/* pas branchement direct */
			ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, nt);
		    }

		    /* Appel recursif pour pouvoir faire UNDO. */
		    if (bscan) {
			if (++head > tail) {
			    done = TRUE, is_rc = TRUE;
			    break;
			}

			sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + head;
			look_ahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
		    }

		    is_rc = REF_is_a_right_ctxt (head, tail, &xs, &state, ref,
						 look_ahead, keep_stack);

		    if (keep_stack) {
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

		    done = TRUE;
		    break;
		}

		xs -= ared->lgth;

		if (!keep_stack && xs < sxplocals.rcvr.min_xs)
		    sxplocals.rcvr.min_xs = xs;

		state = sxplocals.rcvr.stack [xs];

		if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
		    /* pas branchement direct */
		    ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, nt);
		}
	    }
	}

	if (!done) {
	    if (++head > tail)
		done = TRUE, is_rc = TRUE;
	    else {
		sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + head;
		look_ahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
		ref = sxP_access (sxplocals.SXP_tables.t_bases + state, look_ahead);
	    }
	}
    }

    if (!keep_stack) 
	*pxs = xs, *pstate = state;

    return is_rc;
}

static BOOLEAN	is_a_right_ctxt (SXINT head, SXINT tail, SXINT *pxs, SXP_SHORT *pstate, BOOLEAN keep_stack)

/* Check if tokens contained in tokens[head..tail] are a valid right context of
   stack. Assume that tail takes into account the occurrences of key-terminals.
   Predicates are tested only at places where they are supposed to return the same value as
   in normal analysis i.e. head==0 and head==1
   If keep_stack do not modify stack */


{
    SXP_SHORT			ref;
    SXINT				xs = *pxs;
    SXP_SHORT			state = *pstate, look_ahead;
    BOOLEAN			is_rc;

    if (head > tail)
	return TRUE;

    if (!keep_stack)
	sxplocals.rcvr.min_xs = xs;

    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + head;
    look_ahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
    ref = sxP_access (sxplocals.SXP_tables.t_bases + state, look_ahead);

    is_rc = REF_is_a_right_ctxt (head, tail, &xs, &state, ref, look_ahead, keep_stack);


    if (keep_stack) {
	SXINT	elem = 0;

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

static BOOLEAN	check_ctxt (SXINT head, SXINT tail, SXINT *x, SXP_SHORT ref, SXBA trans_set, SXINT n)
{
    /* ref est une action valide depuis l'etat stack [x]
       On cherche une sequence de shifts qcq {T|NT}* suivie de t[head..tail]. */
    /* Si OK, la pile stack (et son sommet x) sont modifies, l'etat sommet
       est tel qu'il existe une transition terminale sur t [head].
       Sinon, la "stack" est inchangee. */

    SXP_SHORT		check, state, prev_state;
    SXINT			next_action_kind;
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




static VOID set_vt_set (SXP_SHORT *stack, SXINT xstd, SXP_SHORT state1);

static VOID new_ref (SXP_SHORT *stack, SXINT xstd, SXP_SHORT ref, SXP_SHORT test)
{
    struct SXP_reductions	*ared;
    SXINT                       j = xstd;
    SXP_SHORT			prev_state, nt, state;

    for (;;) {
	if (ref == 0) {
	    /* error */ 
	    return;
	}

	if (ref < -sxplocals.SXP_tables.Mref /* ARC ... tant pis, on valide */
	    || ref > 0		/* scan */ ) {
	    sxba_1_bit (sxplocals.rcvr.vt_set, test);
	    return;
	}

	ref = -ref;

	if (ref > sxplocals.SXP_tables.Mprdct /* Shift */ ) {
	    if (++j > lgt1)
		oflow ();

	    prev_state = stack [j];
	    stack [j] = ref -= sxplocals.SXP_tables.Mprdct;

	    set_vt_set (stack, j, ref);

	    stack [j] = prev_state;
	    return;
	}
	
	if (ref > sxplocals.SXP_tables.Mred /* predicates */ ) {
	    if (sxplocals.mode.with_do_undo && sxplocals.mode.with_parsact)
	    {
		/* On valide les terminaux etendus */
		struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts + ref;
		SXINT old_xps;
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
	    }
	    else {
		/* On ne valide pas les terminaux etendus */	    
		sxplocals.rcvr.has_prdct = TRUE;
		return;
	    }
	}
	else {
	    /* Reduce or Halt */
	    ared = sxplocals.SXP_tables.reductions + ref;

	    if (ared->reduce == 0 /* Halt */ ) {
		sxba_1_bit (sxplocals.rcvr.vt_set, sxplocals.SXP_tables.P_tmax);
		return;
	    }

	    if ((ared->reduce > sxplocals.SXP_tables.P_nbpro || ared->action >= 10000) &&
		sxplocals.mode.with_do_undo &&
		sxplocals.mode.with_parsact)
	    {
		/* parsact a executer immediatement */
		/* Ca permet aux parsact de manipuler une structure // a stack en
		   utilisant les macros STACKtop(), STACKnewtop() et STACKreduce(). */
		SXINT		old_xps = sxpglobals.xps;
		SXINT		old_xs = j;

		sxpglobals.reduce = ared->reduce;
		sxpglobals.pspl = ared->lgth; /* 0 */
		sxpglobals.xps = old_xs;

		if (ref > sxplocals.SXP_tables.Mrednt) {
		    /* Reduce dans les T_tables sans Scan */
		    sxpglobals.pspl--;
		    sxpglobals.xps--;
		}

		(*sxplocals.SXP_tables.parsact) (DO, ared->action);

		sxpglobals.xps = old_xps;

		j -= ared->lgth;

		state = sxplocals.rcvr.stack [j];

		if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
		    /* pas branchement direct */
		    ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, nt);
		}

		/* Appel recursif pour pouvoir faire UNDO. */
		new_ref (stack, j, ref, test);

		/* On fait UNDO */
		sxpglobals.reduce = ared->reduce;
		sxpglobals.pspl = ared->lgth; /* 0 */
		sxpglobals.xps = old_xs;

		if (ref > sxplocals.SXP_tables.Mrednt) {
		    /* Reduce dans les T_tables sans Scan */
		    sxpglobals.pspl--;
		    sxpglobals.xps--;
		}

		(*sxplocals.SXP_tables.parsact) (UNDO, ared->action);

		sxpglobals.xps = old_xps;

		return;
	    }

	    j -= ared->lgth;
	    state = stack [j];

	    if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
		/* pas branchement direct */
		ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, nt);
	    }
	}
    }
}


static VOID	set_vt_set (SXP_SHORT *stack, SXINT xstd, SXP_SHORT state1)

/* remplit vt_set */

{
    struct SXP_bases	*abase = sxplocals.SXP_tables.t_bases + state1;
    SXP_SHORT 	ref, test;
    SXINT		next_action_kind;

    set_first_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind);

    do {
	new_ref (stack, xstd, ref, test);
    } while (set_next_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind));
}




/* E R R O R   R E C O V E R Y */


static BOOLEAN	ARC_recovery (SXINT *at_state, SXINT latok_no)
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
    /* rattrapage global en LookAhead */
    *at_state = sxplocals.SXP_tables.germe_to_gr_act [*at_state];
    return TRUE;
}

static BOOLEAN	recovery (void)
{
    char				*msg_params [5];
    SXINT			i, j, l;
    struct SXP_local_mess	*local_mess;
    SXINT					head, tail, k, vt_set_card, xps, next_action_kind, xs2;
    SXP_SHORT				check, state, ref;
    BOOLEAN				is_warning, no_correction_on_previous_token;
    struct sxtoken			*p;
    struct sxsource_coord		source_index;

    if ((no_correction_on_previous_token = (sxplocals.ptok_no == sxplocals.atok_no))) {
	/* On suppose qu'on n'est jamais en debut de tokens_buf! */
	sxplocals.atok_no--;
    }

/* L'erreur sera marquee (listing) a l'endroit de la detection */
    source_index = SXGET_TOKEN (sxplocals.ptok_no).source_index;

    is_warning = TRUE;
    state = sxplocals.state;
    xs2 = sxpglobals.xps;

    restaure_stack (sxpglobals.stack_bot + 1, xs2);

    sxba_empty (sxplocals.rcvr.vt_set);
    sxplocals.rcvr.has_prdct = FALSE;

    if (no_correction_on_previous_token
	|| is_a_right_ctxt ((SXINT)0, (SXINT)0, &xs2, &state, FALSE)) 
	set_vt_set (sxplocals.rcvr.stack, xs2, state);

    if ((vt_set_card = sxba_cardinal (sxplocals.rcvr.vt_set)) == 1
	&& !sxplocals.rcvr.has_prdct
	&& (k = sxba_scan (sxplocals.rcvr.vt_set, 0)) != sxplocals.SXP_tables.P_tmax) {
	/* un seul suivant valide different de EOF ==> insertion forcee */
	/* Pas de predicats dans le coup */
	local_mess = sxplocals.SXP_tables.SXP_local_mess + sxplocals.SXP_tables.P_nbcart + 1;

	/* preparation du message */
	
	for (j = local_mess->P_param_no - 1; j >= 0; j--) {
	    struct sxtoken	*tok;
	    SXINT ate;

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
    }


/* rattrapage global */

/* message sur les suivants attendus */

    if (((SXUINT) (vt_set_card)) <= (SXUINT) sxplocals.SXP_tables.P_followers_number /* a < x <= b */  && !sxplocals.rcvr.has_prdct) {
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
	    tail = get_tail ((SXINT)0, sxplocals.SXP_tables.P_validation_length - 1);

	    for (xps = sxpglobals.xps; xps > sxpglobals.stack_bot; xps--) {
		/* pop the stack and check the right context */
		struct SXP_bases	*abase;
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

		    while ((!(is_checked = check_ctxt (head, tail, &xs2, ref,
						       trans, sxpglobals.xps - xps)))
			   && set_next_trans (abase, &check, &ref,
					     sxplocals.SXP_tables.P_tmax, &next_action_kind));

		    if (is_checked)
			break;

		    if (state <= sxplocals.SXP_tables.M0ref - sxplocals.SXP_tables.Mprdct) {
			/* Il y a des transitions non-terminales */
			abase = sxplocals.SXP_tables.nt_bases + state;
			set_first_trans (abase, &check, &ref,
					 sxplocals.SXP_tables.P_ntmax, &next_action_kind);

			while ((!(is_checked = check_ctxt (head, tail, &xs2, ref,
							   trans, sxpglobals.xps - xps)))
			       &&set_next_trans (abase, &check, &ref, sxplocals.SXP_tables.P_ntmax,
						 &next_action_kind)); 

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
       
    sxpglobals.pspl = sxpglobals.xps - xps - 1 /* pour STACKnewtop() eventuel */ ;
    (*(sxplocals.SXP_tables.semact)) (ERROR, xs2 - xps);

    /* reajustement de la pile */
    while (++xps <= xs2) {
	sxpglobals.parse_stack [xps].state = sxplocals.rcvr.stack [xps];
	sxpglobals.parse_stack [xps].token.lahead = 0 /* simulation of a NT */ ;
    }

    sxpglobals.xps = xs2;
    sxplocals.state = sxplocals.rcvr.stack [xs2];

    {
	SXINT	ate;

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




BOOLEAN		sxpsrecovery (SXINT what_to_do, SXINT *at_state, SXINT latok_no)
{
    switch (what_to_do) {
    case OPEN:
	/* Allocation will be done at first call. */
	break;

    case ACTION:
    case ERROR:
	if (sxplocals.mode.kind == SXWITH_CORRECTION)
	    /* Seule une tentative de correction a ete demandee. */
	    return FALSE;

	sxplocals.rcvr.truncate_context = TRUE;

	if (sxplocals.rcvr.stack == NULL) {
	    SXINT   sizofpts = sxplocals.SXP_tables.P_sizofpts;

	    /* Tant pis, on ne partage pas stack, st_stack et modified_set
	       entre differents langages... */
	    /* Il faudrait pouvoir appeler le point d'entree END pour les desallouer
	       en fin de process or sxprecovery est lie' aux tables du parser
	       or le point d'entree END du parser est independant de ces tables. */
	    sxplocals.rcvr.stack = (SXP_SHORT*) sxalloc ((lgt1 = 200) + 1, sizeof (SXP_SHORT));
	    sxplocals.rcvr.st_stack = (SXP_SHORT*) sxalloc (lgt1 + 1, sizeof (SXP_SHORT));
	    sxplocals.rcvr.modified_set = sxba_calloc (lgt1 + 1);
	    
	    sxplocals.rcvr.ster = (SXINT*) sxalloc (sizofpts + 1, sizeof (SXINT));
	    sxplocals.rcvr.lter = (SXINT*) sxalloc (sizofpts + 1, sizeof (SXINT));
	    sxplocals.rcvr.vt_set = sxba_calloc (sxplocals.SXP_tables.P_tmax + 1);
#if 0
	    sxplocals.rcvr.trans_set = sxba_calloc (sxplocals.SXP_tables.Mref - sxplocals.SXP_tables.Mprdct + 1);
#endif
	    sxplocals.rcvr.mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.mxvec;
	    sxplocals.rcvr.Mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.Mxvec;
	}

	return (what_to_do == ACTION) ? recovery () : ARC_recovery (at_state, latok_no);

    case CLOSE:
	if (sxplocals.rcvr.stack != NULL) {
	    sxfree (sxplocals.rcvr.stack), sxplocals.rcvr.stack = NULL;
	    sxfree (sxplocals.rcvr.st_stack);
	    sxfree (sxplocals.rcvr.modified_set);

	    sxfree (sxplocals.rcvr.ster);
	    sxfree (sxplocals.rcvr.lter);
	    sxfree (sxplocals.rcvr.vt_set);
#if 0
	    sxfree (sxplocals.rcvr.trans_set);
#endif
	}

	break;

    default:
	fprintf (sxstderr, "The function \"sxpsrecovery\" is out of date with respect to its specification.\n");
	sxexit(1);
    }

    return TRUE;
}
