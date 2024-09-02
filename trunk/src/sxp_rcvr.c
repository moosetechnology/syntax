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

#include "sxversion.h"
#include "sxunix.h"
#include "SS.h"

#ifndef VARIANT_32
char WHAT_SXP_RECOVERY[] = "@(#)SYNTAX - $Id: sxp_rcvr.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;
#endif

/*
 * Note: a simpler version of this module once existed. It performed a
 * simplified form of parser recovery. The source code of this module can be
 * found in outdated/deleted/src/sxp_srcvr.c
 */

#define Commun 1
#define Propre 2
#define Defaut 3
#define Done   4

#define CYCLE_DETECTION 2
/* set CYCLE_DETECTION to 0 to remove the ad-hoc code added to prevent 
 * infinite loops in error recovery ; set CYCLE_DETECTION to 1 to use only
 * the simple algorithm for infinite-loop prevention
 */

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

static char	*ttext (SXTABLES *tables, SXINT tcode)
{
    return (tcode == sxplocals.SXP_tables.P_tmax) ? sxplocals.SXP_tables.P_global_mess [sxplocals.SXP_tables.
	 P_followers_number + 2] : sxttext (tables, tcode);
}



static void	oflow (void)

/* overflow of stack */

{
    lgt1 *= 2;
    sxplocals.rcvr.modified_set = sxba_resize (sxplocals.rcvr.modified_set, lgt1 + 1);
    sxplocals.rcvr.st_stack = (SXP_SHORT*) sxrealloc (sxplocals.rcvr.st_stack, lgt1 + 1, sizeof (SXP_SHORT));
    sxplocals.rcvr.stack = (SXP_SHORT*) sxrealloc (sxplocals.rcvr.stack, lgt1 + 1, sizeof (SXP_SHORT));
}

static bool	set_next_item (
			       struct SXP_item *vector,
			       SXP_SHORT *check, 
			       SXP_SHORT *action,
			       SXINT Max)
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
	    return true;
	}

    return false;
}



static void	set_first_trans (
				 struct SXP_bases *abase,
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



static bool	set_next_trans (
				struct SXP_bases *abase,
				SXP_SHORT *check, 
				SXP_SHORT *action,
				SXINT Max,
				SXINT *next_action_kind)
{
  switch (*next_action_kind) {
  case Commun:
    if (set_next_item (sxplocals.SXP_tables.vector + abase->commun, check, action, Max))
      return true;

    if (abase->propre != 0 /* partie propre non vide */ ) {
      *next_action_kind = Propre;
      *check = 0;
      /* !!! pas de break ici : c'est voulu */

    case Propre:
       /* !!! ce "case" se trouve au milieu du "if" : c'est voulu */
      if (set_next_item (sxplocals.SXP_tables.vector + abase->propre, check, action, Max))
	return true;
    }
    /* !!! pas de break ici : c'est voulu */
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
    __attribute__ ((fallthrough));
#endif
	
  case Defaut:
    *check = 0;
    *action = abase->defaut;
    *next_action_kind = Done;
    return true;

  case Done:
    return false;

  default:
    sxtrap ("sxp_rcvr", "set_next_trans");
    /*NOTREACHED*/
  }

  return false; /*NOTREACHED*/
}


static bool	reduce_already_seen (struct SXP_bases *abase, SXP_SHORT check, SXP_SHORT action)
{
    /* action ( >0) reference une action reduce qui n'est pas l'action par defaut
       du abase courant. On regarde si cette action n'a pas deja ete traitee. */

	SXP_SHORT		ref, test;
	SXINT	next_action_kind;

	set_first_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind);

	do {
	    if (test == check)
		return false;

	    if (ref == -action)
		return true;
	} while (set_next_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind));

	return false; /* garde-fou */
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



static void	restaure_stack (SXINT m, SXINT M)
{
    SXINT	i;

    for (i = m; i <= M; i++) {
	/* sauvegarde de la pile */
	sxplocals.rcvr.stack [i] = sxpglobals.parse_stack [i].state;
    }
}



static bool	ARC_is_a_right_ctxt (SXINT *head, SXINT tail, SXP_SHORT *ref)

/* Check if tokens contained in tokens[head..tail] are a valid right context of
   *ref. Assume that tail takes into account the occurrences of key-terminals.
   Predicates are supposed to return false */


{
    SXP_SHORT	next = *ref;

    for (; *head <= tail && (next += sxplocals.SXP_tables.Mref) < 0 ; (*head)++) {
	sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + *head;

	*ref = next = sxP_access (sxplocals.SXP_tables.t_bases - next,
				  SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead);

	if (next == 0 ||
	    (next < -sxplocals.SXP_tables.Mred && next >= -sxplocals.SXP_tables.Mprdct))
	    /* Error or Predicate */
	    return false;
    }

    return true;
}



static void undo (void)
{
    struct SXP_reductions	*ared;
    SXINT				old_xps, x;
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

	    (*sxplocals.SXP_tables.P_parsact) (SXUNDO, ared->action);
	}

	SS_clear (sxplocals.rcvr.undo_stack);
	sxpglobals.xps = old_xps;
    }
}



static bool	REF_is_a_right_ctxt (bool bscan, 
				     bool bshift, 
				     SXINT head, 
				     SXINT tail, 
				     SXINT *pxs, 
				     SXP_SHORT *pstate, 
				     SXP_SHORT ref, 
				     SXP_SHORT look_ahead,
				     bool keep_stack)

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
    bool			is_rc = true;
#if CYCLE_DETECTION >= 1
    bool			cycle_detected = false;
#if CYCLE_DETECTION == 2
    SXP_SHORT			backup_ref, backup_ref_orig;
#endif

    ref_orig = 0;
#if CYCLE_DETECTION == 2
    backup_ref = ref;
    backup_ref_orig = 0;
#endif
#endif

    while (head <= tail && is_rc) {
	while (!bshift) {
	    /* On sort de la boucle sur le shift suivant un scan */
	    if (ref < -sxplocals.SXP_tables.Mref) {
		SXINT ARC_head = head + 1;
		SXP_SHORT ARC_ref = ref;

		if (ARC_head <= tail && !ARC_is_a_right_ctxt (&ARC_head, tail, &ARC_ref)) {
		    return false;
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

	    if (ref == 0) {
		/* Error */ 
		return false;
	    }

#if CYCLE_DETECTION >= 1
	    if ((sxplocals.SXP_tables.P_parsact != NULL)
               /*
                * On est dans le cas ou il y a des actions syntaxiques
                * (c'est-a-dire, des predicats &1, &2, etc. dans la
                * grammaire SEMC ou CSYNT) ; nous avons constate que, 
                * dans ce cas, le rattrapage d'erreurs syntaxiques pouvait
                * entrer dans une boucle infinie : on evite ce probleme en
                * implantant une heuristique simple
                */
		&& ((ref_orig == ref)
               /*
                * La condition (ref_orig == ref) n'est jamais vraie lors de
                * la premiere iteration, puisque ref_orig a ete initialisee
                * a 0 et qu'on sait ici que ref != 0
                */
#if CYCLE_DETECTION == 2
		|| ((backup_ref == ref_orig) && (backup_ref_orig == ref))
		/*
		 * Wendelin Serwe : la recuperation d'erreur peut entrer dans
		 * une boucle infinie si les valeurs de ref et ref_orig
		 * alternent indefiniment ; on detecte cette situation grace
		 * aux variables auxiliaires backup_ref et backup_ref_orig
                 */
#endif
	       )) {
               if (cycle_detected) {
                  /* Lors de l'iteration precedente, on avait deja l'egalite
                   * ref_orig == ref ; on suppose que le code est entre' dans
                   * une boucle infinie dont on force l'interruption ; mais
                   * idealement, il faudrait traiter le probleme a la racine
                   */
                  return false;
               } else {
                  cycle_detected = true;
               }
	    }
#if CYCLE_DETECTION == 2
            backup_ref = ref;
            backup_ref_orig = ref_orig;
#endif
#endif

	    ref_orig = ref;

	    if (ref > 0)
		/* Scan */ 
		bscan = true;
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
		    bshift = true; /* bscan && bshift == true */
	    }
	    else if (ref > sxplocals.SXP_tables.Mred) {
		/* Predicates */
		if (sxplocals.mode.with_do_undo && sxplocals.mode.with_parsact)
		{
		    struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts + ref;
		    SXINT old_xps;

		    /* Au cas ou le predicat est associe' a un nt et il veut re'fe'rencer
		       une structure // a` stack. Il faut utiliser SXSTACKtop () */
		    old_xps = sxpglobals.xps;
		    sxpglobals.xps = xs; 

		    while (aprdct->prdct >= 0 /* User's predicate */  &&
			   (!sxplocals.mode.with_parsprdct ||
			    !(*sxplocals.SXP_tables.P_parsact) (SXPREDICATE, aprdct->prdct)))
			/* returning False */ 
			aprdct++;

		    sxpglobals.xps = old_xps;

		    ref = aprdct->action;
		}
		else
		{
		    return false;
		}
	    }
	    else {
		/* Reduce or Halt*/
		ared = sxplocals.SXP_tables.reductions + ref;

		if (ared->reduce == 0 /* Halt */ ) {
		    if (look_ahead == sxplocals.SXP_tables.P_tmax) {
			*pxs = xs, *pstate = state;
			return true;
		    }

		    return false;
		}

		if ((ared->reduce > sxplocals.SXP_tables.P_nbpro || sxis_syntactic_action (ared->action)) &&
		    sxplocals.mode.with_do_undo &&
		    sxplocals.mode.with_parsact)
		{
		    /* parsact a executer immediatement */
		    /* Ca permet aux parsact de manipuler une structure // a stack en
		       utilisant les macros SXSTACKtop(), SXSTACKnewtop() et SXSTACKreduce(). */
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

		    (*sxplocals.SXP_tables.P_parsact) (SXDO, ared->action);

		    sxpglobals.xps = old_xps;

		    xs -= ared->lgth;

		    if (!keep_stack && xs < sxplocals.rcvr.min_xs)
			sxplocals.rcvr.min_xs = xs;

		    if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
			/* pas branchement direct */
			state = sxplocals.rcvr.stack [xs];
			ref = sxP_access (sxplocals.SXP_tables.nt_bases + state, nt);
		    }

		    /* Appel recursif pour pouvoir faire SXUNDO. */
		    
		    is_rc = REF_is_a_right_ctxt (bscan, bshift, head, tail, &xs, &state, ref,
						 look_ahead, keep_stack);

		    if (!is_rc || keep_stack) {
			/* On fait SXUNDO immediatement */
			sxpglobals.reduce = ared->reduce;
			sxpglobals.pspl = ared->lgth; /* 0 */
			sxpglobals.xps = old_xs;

			if (ref > sxplocals.SXP_tables.Mrednt && ref_orig <= 0) {
			    /* Reduce dans les T_tables sans Scan */
			    sxpglobals.pspl--;
			    sxpglobals.xps--;
			}

			(*sxplocals.SXP_tables.P_parsact) (SXUNDO, ared->action);

			sxpglobals.xps = old_xps;
		    }
		    else
		    {
			/* On memorise les SXUNDO */
			SS_push (sxplocals.rcvr.undo_stack, ref_orig);
			SS_push (sxplocals.rcvr.undo_stack, old_xs);
		    }

		    if (is_rc) {
			*pxs = xs, *pstate = state;
			return true;
		    }

		    return false;
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
	    bscan = false;
	    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + head;
	    look_ahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
	    ref = sxP_access (sxplocals.SXP_tables.t_bases + state, look_ahead);
	}
    }

    if (is_rc) {
	*pxs = xs, *pstate = state;
	return true;
    }

    return false;
}



static bool	is_a_right_ctxt (SXINT head, SXINT tail, SXINT *pxs, SXP_SHORT *pstate, bool keep_stack)

/* Check if tokens contained in tokens[head..tail] are a valid right context of
   stack. Assume that tail takes into account the occurrences of key-terminals.
   Predicates are tested only at places where they are supposed to return the same value as
   in normal analysis i.e. head==0 and head==1
   If keep_stack do not modify stack */
{
    SXP_SHORT			ref, state, look_ahead;
    SXINT				xs;
    bool			is_rc;

    if (head > tail)
	return true;

    xs = *pxs;
    state = *pstate;
    
    if (!keep_stack)
	sxplocals.rcvr.min_xs = xs;

    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + head;
    look_ahead = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
    ref = sxP_access (sxplocals.SXP_tables.t_bases + state, look_ahead);

    is_rc = REF_is_a_right_ctxt (false, false, head, tail, &xs, &state, ref, look_ahead,
				 keep_stack);

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

static bool	check_ctxt (SXINT head, SXINT tail, SXINT *x, SXP_SHORT ref, SXBA trans_set, SXINT n)
{
    /* ref est une action valide depuis l'etat stack [x]
       On cherche une sequence de shifts qcq {T|NT}* suivie de t[head..tail]. */
    /* Si OK, la pile stack (et son sommet x) sont modifies, l'etat sommet
       est tel qu'il existe une transition terminale sur t [head].
       Sinon, la "stack" est inchangee. */

    SXP_SHORT		check, state, prev_state;
    SXINT			next_action_kind;
    struct SXP_bases	*abase;
    bool		ret_val;

#define MAX_PATH_LGTH	0

    state = (ref < 0) ? -ref : ref;

    if (state > sxplocals.SXP_tables.Mref /* ARC */
	|| (state -= sxplocals.SXP_tables.Mprdct) <= 0/* Error, Reduction or Predicates */ )
	/* On ne suit pas les chaines de reductions. Ces nouvelles reductions seront
	   examinees plus tard quand on sera au bon niveau de pile */
	/* User's predicate or &Else (assume always returning false) */
	    return false;

    if ((--n + MAX_PATH_LGTH) >= 0 && SXBA_bit_is_set_reset (trans_set, state)) {
	/* Shift */

	if (++*x > lgt1)
	    oflow ();

	prev_state = sxplocals.rcvr.stack [*x];
	sxplocals.rcvr.stack [*x] = state;
	abase = sxplocals.SXP_tables.t_bases + state;

	if (sxP_access (abase, SXGET_TOKEN (sxplocals.rcvr.TOK_0).lahead) > 0
	    /* scan sur le terminal cle */
	    && is_a_right_ctxt (head, tail, x, &state, true))
	    ret_val = true;
	else {
	    ret_val = false;

	    set_first_trans (abase, &check, &ref,
			     sxplocals.SXP_tables.P_tmax, &next_action_kind);

	    do {
		if (check_ctxt (head, tail, x, ref, trans_set, n)) {
		    ret_val =  true;
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
			ret_val =  true;
			break;
		    }
		} while (set_next_trans (abase, &check, &ref,
				     sxplocals.SXP_tables.P_ntmax, &next_action_kind));
	    }
	}

/* On remet les lieux dans l'etat initial */

	if (!ret_val) {
	    sxplocals.rcvr.stack [*x] = prev_state;
	    --*x;
	}

	return ret_val;
    }

    return false;
}

static bool	morgan (char *c1, char *c2)

/* rend vrai si c1 = c2 a une faute d'orthographe pres */

{
    char	*tmax, *t1, *t2;
    SXINT		l1, l2;
    char	d1, d2, e1, e2;

    switch ((l1 = strlen (c1)) - (l2 = strlen (c2))) {
    default:
	/* longueurs trop differentes */
	return false;

    case 0:
	/* longueurs egales: on autorise un remplacement ou une interversion
	   de caracteres */
	tmax = c1 + l1, t1 = c1, t2 = c2;

	while (t1 < tmax && *t1 == *t2)
	    t1++, t2++;

	if (t1 == tmax)
	    /* egalite */
	    return true;

	d1 = *t1++, d2 = *t2++ /* caracteres courants, differents */ ;
	e1 = *t1++, e2 = *t2++ /* caracteres suivants */ ;

	if (e1 != e2 /* caracteres suivants differents */
		     && (d1 != e2 || e1 != d2))
	    /* caracteres pas inverses */
	    return false;


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

    if (*t2 == SXNUL)
	/* egalite au dernier caractere pres */
	return true;

    t1++ /* on saute un caractere dans la chaine la plus longue */ ;


/* les restes des chaines doivent etre egaux */

    while (t1 < tmax && *t1 == *t2)
	t1++, t2++;

    return t1 >= tmax;
}



static bool match_a_tok (SXINT cur_mod, SXINT n, SXINT tok)
{
    /* Cette fonction regarde si le token "tok" match le nieme element de modele cur_mod */
    SXINT	*regle;
    SXINT	j;

    if (cur_mod > sxplocals.rcvr.glbl_best_mod)
        sxtrap ("sxp_rcvr", "match_a_tok() unexpected model priority");
    
    regle = sxplocals.SXP_tables.P_lregle [cur_mod];

    if (n > regle [0] || (j = regle [n]) == -1)
	return true;

    if (j >= 0) {
	sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + j;

	return tok == SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
    }

    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + n - 1;

    if (sxplocals.sxtables->SXS_tables.S_check_keyword == NULL
	|| sxstrlen (j = SXGET_TOKEN (sxplocals.rcvr.TOK_i).string_table_entry) <= 2
	|| !sxkeywordp (sxplocals.sxtables, tok)
	|| !morgan (sxstrget (j), sxttext (sxplocals.sxtables, tok)))
	return false;

    return true;
}


static bool search_a_rule (SXINT *cur_mod, SXINT n, SXINT best_mod, SXINT *ster)
{
    SXINT 	mod, k, bot;
    SXINT	*regle;

    /* Recherche un modele mod tel que l'on a cur_mod < mod <= best_mod dont le prefixe verifie ster */

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


static bool is_valid (SXINT cur_mod, SXINT n, SXINT *ster)
{
    /* Regarde si la regle de correction d'erreur cur_mod est valide par rapport
       aux don't delete et don't insert */
    SXINT	*regle;
    SXINT	j, k, lmod, head;

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
	    return false;
    }

    j = -1;

    while ((j = sxba_scan (sxplocals.rcvr.to_be_checked, j)) >= 0 && j < head) {
	sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + j;

	if (sxgetbit (sxplocals.SXP_tables.P_no_delete, SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead))
	    return false;
    }

    return true;
}


static void ARC_deriv (SXINT n, SXINT t_state, SXINT cur_mod)
{
    /* Les n tokens de ster valident cur_mod (et il n'existe pas de regle plus
       prioritaire que cur_mod qui les valident). On est dans l'etat t_state
       d'un ARC. */

    struct SXP_bases	*abase = sxplocals.SXP_tables.t_bases + t_state;
    SXINT	k;
    SXP_SHORT		ref, test;
    SXINT			next_action_kind, rule_no;

    set_first_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind);

    do {
	if (ref < -sxplocals.SXP_tables.Mred && ref >= -sxplocals.SXP_tables.Mprdct) {
	    /* On ne valide pas les terminaux etendus */
	    continue;
	}

	k = n;
	rule_no = cur_mod;
	sxplocals.rcvr.ARC_ster [++k] = test;

	if (match_a_tok (rule_no, k, (SXINT)test) ||
	    search_a_rule (&rule_no, k, sxplocals.rcvr.ARC_best.modele_no,
			   sxplocals.rcvr.ARC_ster))
	    do {
	    SXINT	*regle = sxplocals.SXP_tables.P_lregle [rule_no];
	    SXINT	lmod;
	    SXP_SHORT	next;
	    SXINT head = regle [k + 1], tail, checked_lgth;

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
			ARC_deriv (k, (SXINT)-next, rule_no);

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


static void	deriv (bool string_has_grown, SXINT n, SXINT xstd, SXP_SHORT t_state, SXINT rule_no);

static void new_ref (SXINT k, SXINT n, SXINT xstd, SXP_SHORT ref, SXP_SHORT test, SXP_SHORT t_state, SXINT rule_no)
{
    struct SXP_reductions	*ared;
    SXP_SHORT			state, ref_orig;
    struct SXP_bases		*abase;
    SXINT                       j;
    SXP_SHORT			prev_state;
    bool			scan, never_scanned;

    abase = sxplocals.SXP_tables.t_bases + t_state;
    j = xstd;
    never_scanned = true;

  for (;;) {
    if (ref < -sxplocals.SXP_tables.Mref) {
	/* On rentre dans un ARC, comme il decrit un surlangage, il ne peut pas
	   etre utilise pour valider une correction. On l'utilise pour retourner
	   une action syntaxique (shift ou reduce) -- elle peut etre incorrecte --
	   qui sera elle meme utilisee pour une vraie validation */

	SXINT	i, cur_mod;
	bool	store_is_correction_on_error_token;

	for (i = 1; i <= n; i++)
	    sxplocals.rcvr.ARC_ster [i] = sxplocals.rcvr.ster [i];

	sxplocals.rcvr.ARC_ster [i] = test;
	sxplocals.rcvr.ARC_a_la_rigueur.modele_no =
	    sxplocals.rcvr.ARC_best.modele_no = sxplocals.SXP_tables.P_min_prio_0 + 1;
	sxplocals.rcvr.ARC_a_la_rigueur.checked_lgth = 0;
	store_is_correction_on_error_token = sxplocals.rcvr.is_correction_on_error_token;
	sxplocals.rcvr.is_correction_on_error_token = true;
	cur_mod = rule_no;

	if (!match_a_tok (cur_mod, i, (SXINT)test) &&
	    !search_a_rule (&cur_mod, i, sxplocals.rcvr.ARC_best.modele_no,
			    sxplocals.rcvr.ARC_ster))
	    break;

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
      never_scanned = false;

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

	/* Le 10/07/07, modifié le 7 aout (le test "(scan)" devient "!(never_scanned && test)" */
	/* deriv (k > n, k, j, state, rule_no); est remplace' par ... */

	if (never_scanned && test) {
	  new_ref (k, n, (SXINT)j, sxP_access (sxplocals.SXP_tables.t_bases + state, test), test, t_state, rule_no);
	}
	else {
	  deriv ((bool) (k > n), k, j, state, rule_no);
	}
	/* Le 10/07/07 */

	sxplocals.rcvr.stack [j] = prev_state;
    }
    else if (ref > sxplocals.SXP_tables.Mred) {
	/* predicates */ 
	if (sxplocals.mode.with_do_undo && sxplocals.mode.with_parsact)
	{
	    struct SXP_prdct	*aprdct = sxplocals.SXP_tables.prdcts + ref;
	    SXINT old_xps;

	    /* Au cas ou le predicat est associe' a un nt et il veut re'fe'rencer
	       une structure // a` stack. Il faut utiliser SXSTACKtop () */
	    old_xps = sxpglobals.xps;
	    sxpglobals.xps = j; 

	    while (aprdct->prdct >= 0 /* User's predicate */  &&
		   (!sxplocals.mode.with_parsprdct ||
		    !(*sxplocals.SXP_tables.P_parsact) (SXPREDICATE, aprdct->prdct)))
		/* returning False */ 
		aprdct++;

	    sxpglobals.xps = old_xps;
	    ref = aprdct->action;

	    continue;
	}
	else
	    if (n == 1 && sxplocals.rcvr.is_correction_on_error_token)
		/* On ne valide pas les terminaux etendus */
		sxplocals.rcvr.has_prdct = true;
    }
    else if (ref > 0		/* Reduce or Halt */ ) {
	ared = sxplocals.SXP_tables.reductions + ref;

	if (ared->reduce == 0	/* Halt */ ) {
	    SXINT cur_mod;

	    if (n == 1 && sxplocals.rcvr.is_correction_on_error_token)
		sxba_1_bit (sxplocals.rcvr.vt_set, test); /* test == sxplocals.SXP_tables.P_tmax*/

	    for (k++; k <= sxplocals.SXP_tables.P_maxlregle; k++) {
		sxplocals.rcvr.ster [k] = sxplocals.SXP_tables.P_tmax;
	    }

	    /* deriv suppose que rule_no est verifie jusqu'a n-1, ce n'est pas le
	       cas ici => appel de search_a_rule */
	    cur_mod = rule_no - 1;

	    if (search_a_rule (&cur_mod, sxplocals.SXP_tables.P_maxlregle,
			       sxplocals.rcvr.glbl_best_mod, sxplocals.rcvr.ster))
		deriv (true, sxplocals.SXP_tables.P_maxlregle, j - ared->lgth, 0, cur_mod);
	}
	else			/* Reduce */
	    if (test == 0 || scan || !reduce_already_seen (abase, test, ref)) {
		SXINT	nt, new_ref_state;

		if ((ared->reduce > sxplocals.SXP_tables.P_nbpro || sxis_syntactic_action (ared->action)) &&
		    sxplocals.mode.with_do_undo &&
		    sxplocals.mode.with_parsact)
		{
		    /* parsact a executer immediatement */
		    /* Ca permet aux parsact de manipuler une structure // a stack en
		       utilisant les macros SXSTACKtop(), SXSTACKnewtop() et SXSTACKreduce(). */
		    SXINT		old_xps = sxpglobals.xps;
		    SXINT		old_xs = j;

		    sxpglobals.reduce = ared->reduce;
		    sxpglobals.pspl = ared->lgth; /* 0 */
		    sxpglobals.xps = old_xs;

		    if (ref > sxplocals.SXP_tables.Mrednt && ref_orig <= 0) {
			/* Reduce dans les T_tables sans Scan */
			sxpglobals.pspl--;
			sxpglobals.xps--;
		    }

		    (*sxplocals.SXP_tables.P_parsact) (SXDO, ared->action);

		    sxpglobals.xps = old_xps;

		    j -= ared->lgth;

		    new_ref_state = sxplocals.rcvr.stack [j];

		    if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
			/* pas branchement direct */
			ref = sxP_access (sxplocals.SXP_tables.nt_bases + new_ref_state, nt);
		    }

		    /* Appel recursif pour pouvoir faire SXUNDO. */
		    new_ref (k, n, (SXINT)j, ref, test, t_state, rule_no);

		    /* On fait SXUNDO */
		    sxpglobals.reduce = ared->reduce;
		    sxpglobals.pspl = ared->lgth; /* 0 */
		    sxpglobals.xps = old_xs;

		    if (ref > sxplocals.SXP_tables.Mrednt && ref_orig <= 0) {
			/* Reduce dans les T_tables sans Scan */
			sxpglobals.pspl--;
			sxpglobals.xps--;
		    }

		    (*sxplocals.SXP_tables.P_parsact) (SXUNDO, ared->action);

		    sxpglobals.xps = old_xps;

		    break;
		}

		j -= ared->lgth;
		
		new_ref_state = sxplocals.rcvr.stack [j];
		    
		if ((nt = (ref = ared->lhs) - sxplocals.SXP_tables.Mref) > 0) {
		    /* pas branchement direct */
		    ref = sxP_access (sxplocals.SXP_tables.nt_bases + new_ref_state, nt);
		}

		continue;
	    }
	/* else reduction deja examinee, on ne recommence pas... */
    }
    break;
  }
}

/* Les terminaux t1 et t2 de code interne t1code et t2code sont tous les 2 des candidats valides pour une correction.
 * Si ti est générique stri repère le caractère du nom de ce générique situé après le '%'
 * Si ti est non-générique, stri repère le premier caractère de son nom
 * t1 est "meilleur" que t2 ssi on a
 *    - t1 est non-générique et t2 est générique
 *    - ou, étant tous les 2 génériques ou tous les 2 non-génériques on a str1 < str2 (par ordre alphabétique)
*/

static bool better_candidate (SXINT t1code, SXINT t2code)
{
  bool  is_gen1, is_gen2;
  char  *str1, *str2, c1, c2;
  SXINT l1, l2, l;
  
  is_gen1 = sxgenericp (sxplocals.sxtables, t1code);
  is_gen2 = sxgenericp (sxplocals.sxtables, t2code);

  if (!is_gen1 && is_gen2) return true;
  if (is_gen1 && !is_gen2) return false;

  str1 = sxttext (sxplocals.sxtables, t1code);
  str2 = sxttext (sxplocals.sxtables, t2code);
  if (is_gen1) str1++;
  if (is_gen2) str2++;
  l1 = strlen (str1);
  l2 = strlen (str2);
  l = l1 < l2 ? l1 : l2;

  while (l-- > 0) {
    c1 = *str1++;
    c2 = *str2++;
    if (c1 < c2) return true;
    if (c1 > c2) return false;
  }

  /* préfixes identiques */
  return l1 < l2;
}


static bool check_best_ster (SXINT n, SXINT *best_str)
{
    SXINT i, best, cur;

    /* Pour essayer de pallier des fluctuations dans la correction d'erreur dues
     * a de "petites" modifications dans la grammaire, qui sont genantes pour
     * les tests de regression, on ne garde, pour un modele donne', que la
     * correction qui met en jeu les terminaux ayant des codes minimaux
     */

    for (i = 1; i <= n; i++) {
        best = best_str [i];
        cur = sxplocals.rcvr.ster [i];
        if (cur != best) {
	  /* cur et best sont dans le même modèle, s'ils sont différents, ils correspondent forcément à un "S" ou un "X" */
	  return better_candidate (cur, best); 
        }
    }

  /* Identiques */
  return false;
}


static void	deriv (bool string_has_grown, SXINT n, SXINT xstd, SXP_SHORT t_state, SXINT rule_no)

/* fabrique les corrections locales; place ses resultats dans "nomod" (numero
   du modele) et "lter" (nouveau flot de tokens) */
/* Suppose que t_state designe une T_table */
/* rcvr.nomod est plus prioritaire que a_la_rigueur.modele_no */

{
    if (rule_no > sxplocals.rcvr.glbl_best_mod) { 
        /* Si un modele de correction est valide' par une branche du sous-arbre
         * des derivations calculees par cet appel de deriv(), sa priorite' ne
         * pourra etre meilleure que celle de rule_no
         */
        return; /* garde-fou */
    }   

    if (string_has_grown) {
	bool must_return = true;
        bool not_kept;
	SXINT	*regle;
	SXINT	head, lmod;
	SXINT 	*best_str, i;
        SXINT     *pnomod;
        bool valid;

	if (match_a_tok (rule_no, n, sxplocals.rcvr.ster [n]) ||
	  search_a_rule (&rule_no, n, sxplocals.rcvr.glbl_best_mod, sxplocals.rcvr.ster)) {
	  do {
	    /* rule_no <= glbl_best_mod */
	    regle = sxplocals.SXP_tables.P_lregle [rule_no];

	    if (n < (lmod = regle [0])) {
	      if ((head = regle [n + 1]) >= sxplocals.SXP_tables.P_right_ctxt_head [rule_no]) {
		if (!is_a_right_ctxt (head, get_tail (head, regle [lmod]),
				      &xstd, &t_state, true))
		  continue; /* Echec pour rule_no */
	      }
	      else {
		/* rule_no partiellement verifie', on poursuit sur le  meme chemin */
		must_return = false;
		break;
	      }
	    }

	    /* Here the model rule_no has been checked */
	    valid = is_valid (rule_no, n, sxplocals.rcvr.ster);
	    if (valid) {
	      /* candidat principal */
	      pnomod = &sxplocals.rcvr.nomod;
	      best_str = sxplocals.rcvr.lter;
	      /* On inhibe "a_la_rigueur" */
	      sxplocals.rcvr.a_la_rigueur.modele_no = 0;
	    }
	    else {
	      /* candidat "a_la_rigueur" (vis-a-vis des Dont_delete/Dont_insert) */
	      pnomod = &sxplocals.rcvr.a_la_rigueur.modele_no;
	      best_str = sxplocals.rcvr.a_la_rigueur.correction;
	    }

	    /* Ici, si *pnomod == 0 => La correction courante est une "a_la_rigueur" et elle ne sera pas
	       memorisee (il y a deja un principal de stocke') dont le rule_no est sxplocals.rcvr.glbl_best_mod */
	    
	    not_kept = (*pnomod == 0) || ((rule_no == *pnomod) && !check_best_ster (n, best_str));
	    	    
	    /* not_kept => on a deja mieux en magasin... */
	    /* ... et on ne trouvera pas mieux en continuant */
	    
	    if (!not_kept) {
	      /* Ici, cette correction doit etre gardee, elle est meilleure que la precedente dans sa categorie */
	      *pnomod = rule_no;

	      if (valid)
		/* sxplocals.rcvr.glbl_best_mod est un modele principal qui a passe' le test is_valid() */
		/* Si aucun modele n'a ete valide' et si un "a_la_rigueur" a ete trouve', on continue a explorer les
		   moins prioritaires a la recherche d'un valide.
		   En revanche, si un valide est trouve', il serait inutile de rechercher des "a_la_rigueur". */
		sxplocals.rcvr.glbl_best_mod = rule_no;

	      for (i = 1; i <= n; i++) {
		best_str [i] = sxplocals.rcvr.ster [i];
	      }

	      for (; i <= lmod; i++) {
		sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + regle [i];
		best_str [i] = SXGET_TOKEN (sxplocals.rcvr.TOK_i).lahead;
	      }
            }

	    return; /* on est sur une feuille, on ne trouvera pas mieux en continuant */
	  } while (search_a_rule (&rule_no, n, sxplocals.rcvr.glbl_best_mod, sxplocals.rcvr.ster));
	}

	if (must_return)
	    return;
    }

    /* Here the string ster [1:n] matches the prefix of length n of model rule_no */

    {
	struct SXP_bases	*abase = sxplocals.SXP_tables.t_bases + t_state;
	SXP_SHORT			ref, test;
	SXINT				next_action_kind;

	set_first_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax, &next_action_kind);

	do {
	    new_ref (n, n, xstd, ref, test, t_state, rule_no);
	} while (set_next_trans (abase, &test, &ref, sxplocals.SXP_tables.P_tmax,
				 &next_action_kind));
    }
}


static bool	ARC_is_a_correction (SXINT t_state)

/* rend vrai si on peut trouver une correction locale */

{
    sxplocals.rcvr.ARC_ster [1] = (SXGET_TOKEN (sxplocals.rcvr.TOK_0)).lahead;
    sxplocals.rcvr.ARC_a_la_rigueur.modele_no = sxplocals.rcvr.ARC_best.modele_no = sxplocals.SXP_tables.P_min_prio_0 + 1;
    sxplocals.rcvr.ARC_a_la_rigueur.checked_lgth = 0;
    sxplocals.rcvr.is_correction_on_error_token = true;
    ARC_deriv ((SXINT)1, t_state, (SXINT)1);

    if (sxplocals.rcvr.ARC_best.modele_no > sxplocals.SXP_tables.P_min_prio_0) {
	if (sxplocals.rcvr.ARC_a_la_rigueur.modele_no > sxplocals.SXP_tables.P_min_prio_0)
	    return false;

	sxplocals.rcvr.ARC_best.modele_no = sxplocals.rcvr.ARC_a_la_rigueur.modele_no;
	sxplocals.rcvr.ARC_best.ref = sxplocals.rcvr.ARC_a_la_rigueur.ref;
	sxplocals.rcvr.ARC_best.lgth = sxplocals.rcvr.ARC_a_la_rigueur.lgth;
    }

    return true;
}



static bool	is_a_correction (bool no_correction_on_previous_token, SXINT xs, SXP_SHORT state)

/* rend vrai si on peut trouver une correction locale */
/* state est un t_state */

{
    SXINT	i, l;
    SXINT		xs2 = xs;
    SXP_SHORT	state2 = state;

    sxplocals.rcvr.a_la_rigueur.modele_no = sxplocals.SXP_tables.P_nbcart + 1;
    sxba_empty (sxplocals.rcvr.vt_set);
    sxplocals.rcvr.has_prdct = false;

    if (!no_correction_on_previous_token && !is_a_right_ctxt ((SXINT)0, (SXINT)0, &xs2, &state2, false))
    {
	/* Prdct on previous token */
	undo ();
	return false;
    }

    /* Ici, apres l'appel a is_a_right_ctxt(), on peut appeler deriv() */

    sxplocals.rcvr.ster [1] = (SXGET_TOKEN (sxplocals.rcvr.TOK_0)).lahead; /* TOK_0 est correct */
    sxplocals.rcvr.nomod = sxplocals.SXP_tables.P_min_prio_0 + 1;
    sxplocals.rcvr.glbl_best_mod = sxplocals.rcvr.nomod;
    sxplocals.rcvr.is_correction_on_error_token = true; /* La correction n'implique que des modeles de la forme 0 X ... */
    deriv (true, (SXINT)1, xs2, state2, (SXINT)1);

    if (sxplocals.rcvr.nomod > sxplocals.SXP_tables.P_min_prio_0) {
        /* Echec de la correction avec les modeles de la forme "0 ..." */
        /* "a_la_rigueur" peut être rempli */
	sxplocals.rcvr.nomod = sxplocals.SXP_tables.P_nbcart + 1;
        sxplocals.rcvr.glbl_best_mod = sxplocals.rcvr.nomod; 
    }

    if (!no_correction_on_previous_token) {
	restaure_stack (sxplocals.rcvr.min_xs + 1, xs);
	undo ();
    }


/* On regarde s'il existe un modele plus prioritaire que nomod pouvant
   corriger le terminal precedant le terminal en erreur. */

    if (!no_correction_on_previous_token /* stack est correcte */ &&
	sxplocals.SXP_tables.P_max_prio_X < sxplocals.rcvr.nomod) {
	sxplocals.rcvr.is_correction_on_error_token = false; /* La correction implique tous les modeles */
	deriv (false, (SXINT)0, xs, state, sxplocals.SXP_tables.P_max_prio_X);
    }

    if (sxplocals.rcvr.nomod > sxplocals.SXP_tables.P_nbcart) {
	if (sxplocals.rcvr.a_la_rigueur.modele_no > sxplocals.SXP_tables.P_nbcart)
	    return false;

	l = sxplocals.SXP_tables.P_lregle [sxplocals.rcvr.nomod = sxplocals.rcvr.a_la_rigueur.modele_no] [0];

	for (i = 1; i <= l; i++)
	    sxplocals.rcvr.lter [i] = sxplocals.rcvr.a_la_rigueur.correction [i];
    }

    undo ();

    return true;
}



/* E R R O R   R E C O V E R Y */

static bool	ARC_recovery (SXINT *at_state, SXINT latok_no)
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
	    return false;
    }

    return true;
}

static bool	recovery (void)
{
    char				*msg_params [5];
    SXINT			i, j, x, l;
    SXINT			*regle;
    struct SXP_local_mess	*local_mess;
    SXINT					head, tail, im, k, ll, vt_set_card, x1, y, y1, xps, next_action_kind, xs2;
    SXP_SHORT				check, state, ref;
    bool				is_warning, no_correction_on_previous_token;
    struct sxtoken			*p;
    struct sxsource_coord		source_index;
    SXBA                        trans;

    no_correction_on_previous_token = (sxplocals.state == 0); /* Essai */
    /* ptok_no repère le symbole en erreur (le "1" des modèles), atok_no le "previous_token"  (le "0" des modèles) */

#if 0
    if ((no_correction_on_previous_token = (sxplocals.ptok_no == sxplocals.atok_no))) {
	/* On suppose qu'on n'est jamais en debut de tokens_buf! */
	sxplocals.atok_no--;
    }
#endif

/* accumulation de tokens en avance */
/* L'appel a sxget_token doit s'effectuer avant tout positionnement de variables statiques
   car un appel recursif par l'intermediaire d'une action du scanner appele par
   sxget_token. */
    sxget_token (sxplocals.atok_no + sxplocals.SXP_tables.P_nmax);


/* L'erreur sera marquee (listing) a l'endroit de la detection */
    source_index = SXGET_TOKEN (sxplocals.ptok_no).source_index;
    is_warning = true;
    sxplocals.rcvr.TOK_0 = sxplocals.atok_no;
    state = (SXP_SHORT) sxplocals.state;
    xps = sxpglobals.xps;

    while (lgt1 <= xps)
	oflow ();

    restaure_stack (sxpglobals.stack_bot + 1, xps);

    if (!is_a_correction (no_correction_on_previous_token, xps, state)) {
	/* La correction a échoué */
	 sxplocals.mode.global_errors_nb++; /* Décompte du nombre d'erreurs qui n'ont pas été corrigées */

	if (sxplocals.mode.kind == SXWITH_CORRECTION)
	    /* Seule une tentative de correction a ete demandee. */
	    return false;

	goto riennevaplus;
    }

    sxplocals.mode.local_errors_nb++; /* Décompte du nombre de corrections (qui ont marché) */

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
	struct sxtoken		*tok;
	SXINT ate;

	i = local_mess->P_param_ref [j];

	if (i < 0) {
	  /* X or S */
	  msg_params [j] = ttext (sxplocals.sxtables, sxplocals.rcvr.lter [-i]);
	}
	else {
	  tok = (sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + i, &SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	  msg_params [j] = ((ate = tok->string_table_entry) == SXEMPTY_STE || ate == SXERROR_STE
			    ? ttext (sxplocals.sxtables, tok->lahead)
			    : sxstrget (ate));
	}
	if (msg_params [j] == NULL) {
	  /* le parametre courant est la chaine vide, ce qui ne correspond pas
	   * a une correction raisonnable : du coup, on considere la tentative
	   * de correction comme un echec */
	  goto riennevaplus;
	}
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

	for ( /* x1 = x1 */ ; x1 <= x; x1++) {
	    struct sxtoken	*tok;

	    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + x1;
	    tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));
 
	    if (sxplocals.rcvr.com [y] == NULL)
		sxplocals.rcvr.com [y] = tok->comment;
	    else if (tok->comment != NULL) {
		/* concatenation des commentaires */
		char	*tcom = tok->comment;

		sxplocals.rcvr.com [y] = sx_alloc_and_copy ( strlen (sxplocals.rcvr.com [y]) + strlen (tcom) + 1,
							     sizeof (char),
							     sxplocals.rcvr.com [y],
							     strlen (sxplocals.rcvr.com [y]) );

		strcat ( sxplocals.rcvr.com [y], tcom );

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
	    p->string_table_entry = SXERROR_STE /* error value */ ;
	    sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + x;
	    p->source_index = SXGET_TOKEN (sxplocals.rcvr.TOK_i).source_index;

	    if (sxgenericp (sxplocals.sxtables, sxplocals.rcvr.lter [l]))
		is_warning = false;
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
	sxerror (source_index, 
		 is_warning ?
		 sxplocals.sxtables->err_titles [1][0] :
		 sxplocals.sxtables->err_titles [2][0],
		 local_mess->P_string,
		 is_warning ?
		 sxplocals.sxtables->err_titles [1]+1 :
		 sxplocals.sxtables->err_titles [2]+1,
		 msg_params [0],
		 msg_params [1],
		 msg_params [2],
		 msg_params [3],
		 msg_params [4]);

    return true;


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
	    struct sxtoken	*tok;
	    SXINT ate;

	    i = local_mess->P_param_ref [j];
	    tok = (sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + i,
		   &SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	    msg_params [j] = i < 0 /* assume %0 */
				   ? ttext (sxplocals.sxtables, k)
				   : ((ate = tok->string_table_entry) == SXEMPTY_STE || ate == SXERROR_STE
				      ? ttext (sxplocals.sxtables, tok->lahead)
				      : sxstrget (ate));
	}


/* insertion du nouveau token, avec source_index et comment */

	sxplocals.rcvr.toks [0] = (SXGET_TOKEN (sxplocals.rcvr.TOK_0));
	p = sxplocals.rcvr.toks + 1;
	p->lahead = k;
	p->string_table_entry = SXERROR_STE /* error value */ ;
	p->comment = NULL;
	p->source_index = source_index;
	l = 3, j = 1;

	if (sxgenericp (sxplocals.sxtables, k))
	    is_warning = false;

	goto exit_of_er_re;
    }


/* rattrapage global */

    if (vt_set_card == 0) {
      /*
       * problem in recovery (the set of valid followers --- possible 
       * continuations for the current (valid) prefix --- is empty)
       */
      sxerror (source_index,
	       sxplocals.sxtables->err_titles [2][0],
	       "error: impossible recovery from syntax error",
	       sxplocals.sxtables->err_titles [2]+1);
      return false;
    }

/* message sur les suivants attendus */

    if (((SXUINT) (vt_set_card)) <= (SXUINT)sxplocals.SXP_tables.P_followers_number /* a < x <= b */  && !sxplocals.rcvr.has_prdct) {
	i = 0;
	j = 0;

	while ((j = sxba_scan (sxplocals.rcvr.vt_set, j)) > 0) {
	    msg_params [i++] = ttext (sxplocals.sxtables, j);
	}


/* "Error: "machin" is expected." */

	if (!sxplocals.mode.is_silent)
	    sxerror (source_index,
		     sxplocals.sxtables->err_titles [2][0],
		     sxplocals.SXP_tables.P_global_mess [vt_set_card - 1],
		     sxplocals.sxtables->err_titles [2]+1,
		     msg_params [0],
		     msg_params [1],
		     msg_params [2],
		     msg_params [3],
		     msg_params [4]);
    }


/* "Error: Global Recovery." */

    if (!sxplocals.mode.is_silent)
	sxerror (source_index,
		 sxplocals.sxtables->err_titles [2][0],
		 sxplocals.SXP_tables.P_global_mess [sxplocals.SXP_tables.P_followers_number],
		 sxplocals.sxtables->err_titles [2]+1);

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
		sxerror (p->source_index,
			 sxplocals.sxtables->err_titles [2][0],
			 sxplocals.SXP_tables.P_global_mess
			 [sxplocals.SXP_tables.P_followers_number + 3],
			 sxplocals.sxtables->err_titles [2]+1);
	    /* On simule une reduction entre top = sxpglobals.xps - 1 (l'ancien sommet de
	       pile est non significatif en tant que token) et new_top = sxpglobals.stack_bot + 1
	       pour les actions */
	    sxpglobals.xps-- /* pour SXSTACKtop */;
	    sxpglobals.pspl = sxpglobals.xps - sxpglobals.stack_bot - 1 /* pour STACKnew_top */ ;
	    (*(sxplocals.SXP_tables.P_semact)) (SXERROR, 1, NULL);
	    sxplocals.ptok_no = sxplocals.atok_no;
	    return false;
	}

	if ((i = sxgetbit (sxplocals.SXP_tables.PER_tset, p->lahead)) > 0) {
	    /* terminal courant == terminal cle */
	    /* on regarde si les P_validation_length terminaux suivants
	       sont des suivants valides, puis sinon on "pop" la pile et
	       on recommence; si on tombe au fond de la pile on avance
	       d'un token */
	    if (sxplocals.SXP_tables.PER_trans_sets != NULL)
	      trans = sxba_copy (sxplocals.rcvr.PER_trans_sets [i], sxplocals.SXP_tables.PER_trans_sets [i]);
	    else
	      trans = NULL;

	    head = 0;
	    tail = get_tail ((SXINT)0, sxplocals.SXP_tables.P_validation_length - 1);

	    for (xps = sxpglobals.xps; xps > sxpglobals.stack_bot; xps--) {
		/* pop the stack and check the right context */
		struct SXP_bases	*abase;
		bool			is_checked;

		ref = state = sxpglobals.parse_stack [xs2 = xps].state;
		abase = sxplocals.SXP_tables.t_bases + state;

		if (sxP_access (abase, p->lahead) > 0 /* scan sur le terminal cle */
		    && is_a_right_ctxt (head, tail, &xs2, &ref, true))
		    break;

		if (trans != NULL && SXBA_bit_is_set (trans, state)) {
		    /* state est un candidat */

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
    (*(sxplocals.SXP_tables.P_semact)) (SXERROR, xs2 - xps, NULL);

    /* reajustement de la pile */
    while (xps <= xs2) {
	sxpglobals.parse_stack [xps].state = sxplocals.rcvr.stack [xps];
	sxpglobals.parse_stack [xps].token.lahead = 0 /* simulation of a NT */ ;
	xps++;
    }
#else
    sxpglobals.pspl = sxpglobals.xps - xps - 1 /* pour SXSTACKnewtop() eventuel */ ;
    (*(sxplocals.SXP_tables.P_semact)) (SXERROR, xs2 - xps);

    /* reajustement de la pile */
    while (++xps <= xs2) {
	sxpglobals.parse_stack [xps].state = sxplocals.rcvr.stack [xps];
	sxpglobals.parse_stack [xps].token.lahead = 0 /* simulation of a NT */ ;
    }
#endif /* 0 */

    sxpglobals.xps = xs2;
    sxplocals.state = (SXINT) sxplocals.rcvr.stack [xs2];

    {
	SXINT	ate;

	if ((i = sxplocals.SXP_tables.P_validation_length) > 5) {
	    i = 5;
	}

	while (i-- > 0) {
	    p = (sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + i,
		 &SXGET_TOKEN (sxplocals.rcvr.TOK_i));
	    msg_params [i] = (ate = p->string_table_entry) == SXEMPTY_STE || ate == SXERROR_STE
		? ttext (sxplocals.sxtables, p->lahead)
		: sxstrget (ate);
	}


/* "Warning: Parsing resumes on "machin"." */

	if (!sxplocals.mode.is_silent)
	    sxerror ((SXGET_TOKEN (sxplocals.rcvr.TOK_0)).source_index,
		     sxplocals.sxtables->err_titles [1][0],
		     sxplocals.SXP_tables.P_global_mess
		     [sxplocals.SXP_tables.P_followers_number + 1],
		     sxplocals.sxtables->err_titles [1]+1,
		     msg_params [0],
		     msg_params [1],
		     msg_params [2],
		     msg_params [3],
		     msg_params [4]);
    }

    sxplocals.ptok_no = sxplocals.atok_no;
    return true;
}



bool		sxprecovery (SXINT what, SXINT *at_state, SXINT latok_no)
{
    SXINT i;

    switch (what) {
    case SXOPEN:
	/* Allocation will be done at first call. */
	break;

    case SXACTION:
    case SXERROR:
	sxplocals.rcvr.truncate_context = true;

	if (sxplocals.rcvr.stack == NULL) {
	    SXINT   sizofpts = sxplocals.SXP_tables.P_sizofpts;

	    /* Tant pis, on ne partage pas stack, st_stack et modified_set
	       entre differents langages... */
	    /* Il faudrait pouvoir appeler le point d'entree SXEND pour les desallouer
	       en fin de process or sxprecovery est lie' aux tables du parser
	       or le point d'entree SXEND du parser est independant de ces tables. */
	    sxplocals.rcvr.stack = (SXP_SHORT*) sxalloc ((lgt1 = 200) + 1, sizeof (SXP_SHORT));
	    sxplocals.rcvr.st_stack = (SXP_SHORT*) sxalloc (lgt1 + 1, sizeof (SXP_SHORT));
	    sxplocals.rcvr.modified_set = sxba_calloc (lgt1 + 1);
	    
	    sxplocals.rcvr.com = (char**) sxalloc (sizofpts + 1, sizeof (char*));
	    sxplocals.rcvr.ARC_ster = (SXINT*) sxalloc (sizofpts + 1, sizeof (SXINT));
	    sxplocals.rcvr.ster = (SXINT*) sxalloc (sizofpts + 1, sizeof (SXINT));
	    sxplocals.rcvr.lter = (SXINT*) sxalloc (sizofpts + 1, sizeof (SXINT));
	    sxplocals.rcvr.toks = (struct sxtoken*) sxalloc (sizofpts + 1, sizeof (struct sxtoken));
	    sxplocals.rcvr.a_la_rigueur.correction = (SXINT*) sxalloc (sizofpts + 1, sizeof (SXINT));
	    sxplocals.rcvr.vt_set = sxba_calloc (sxplocals.SXP_tables.P_tmax + 1);
	    sxplocals.rcvr.to_be_checked = sxba_calloc (sxplocals.SXP_tables.P_nmax + 1);
	    sxplocals.rcvr.mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.mxvec;
	    sxplocals.rcvr.Mvector = sxplocals.SXP_tables.vector + sxplocals.SXP_tables.Mxvec;
	    sxplocals.rcvr.undo_stack = SS_alloc (50);
	    sxplocals.rcvr.PER_trans_sets = (SXBA*) sxalloc (sxplocals.SXP_tables.PER_tset [0] + 1, sizeof (SXBA));
	    for (i = sxplocals.SXP_tables.PER_tset [0]; i > 0; i--)
	      sxplocals.rcvr.PER_trans_sets [i] = (SXBA) sxba_calloc (sxplocals.SXP_tables.PER_trans_sets [i][0]);
	}

	return (what == SXACTION) ? recovery () : ARC_recovery (at_state, latok_no);

    case SXCLOSE:
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
	    sxfree (sxplocals.rcvr.to_be_checked);
	    SS_free (sxplocals.rcvr.undo_stack);
	    for (i = sxplocals.SXP_tables.PER_tset [0]; i > 0; i--)
	      sxfree (sxplocals.rcvr.PER_trans_sets [i]);
	    sxfree (sxplocals.rcvr.PER_trans_sets), sxplocals.rcvr.PER_trans_sets = NULL;
	}

	break;

    default:
	fprintf (sxstderr, "The function \"sxprecovery\" is out of date with respect to its specification.\n");
	sxexit(1);
    }

    return true;
}
