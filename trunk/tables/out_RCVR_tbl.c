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

static char	ME [] = "out_RCVR_tbl";

#include "sxversion.h"
#include "sxunix.h"
#include "tables.h"
#include "out.h"
#include "sxba.h"

char WHAT_TABLESOUTRCVRTBL[] = "@(#)SYNTAX - $Id: out_RCVR_tbl.c 3650 2023-12-23 07:32:10Z garavel $" WHAT_DEBUG;

#define Commun 1
#define Propre 2
#define Defaut 3
#define Done   4

static     SXBA	*TNT_trans_plus, *T_trans, t_line_0, t_line;


static bool	set_next_item (SXINT base, SXINT *check, SXINT *action, SXINT Max)
{
    struct P_item	*aitem;

    base -= PC.deltavec;

    if (base + *check < 0)
	*check = -base;

    if (base + Max > PC.Mvec)
	Max = PC.Mvec - base;

    for (aitem = P.vector + base + ++*check; *check <= Max; aitem++, ++*check)
	if (aitem->check == *check) {
	    *action = aitem->action;
	    return true;
	}
    *action = 0;
    return false;
}

static void	set_first_trans (struct P_base *abase,
				 SXINT *check,
				 SXINT *action, 
				 SXINT Max, 
				 SXINT *next_action_kind)
{
    SXINT			ref = abase->commun;

    if (ref <= PC.Mref /* codage direct */ ) {
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
	set_next_item (ref, check, action, Max) /* always true (commun non vide) */ ;
	*next_action_kind = Commun;
    }
}

static bool	set_next_trans (struct P_base *abase,
				SXINT *check, 
				SXINT *action, 
				SXINT Max, 
				SXINT *next_action_kind)
{
    switch (*next_action_kind) {
    case Commun:
	if (set_next_item (abase->commun, check, action, Max))
	    return true;

	if (abase->propre != 0 /* partie propre non vide */ ) {
	    *next_action_kind = Propre;
	    *check = 0;
	    /* !!! pas de break ici : c'est voulu */
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	    __attribute__ ((fallthrough));
#endif

    case Propre:
	    /* !!! ce "case" se trouve au milieu du "if" : c'est voulu */
	    if (set_next_item (abase->propre, check, action, Max))
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

    default:
	sxtrap ("out_RCVR_tbl", "set_next_trans");
	  /*NOTREACHED*/
	return false;

    case Done:
	return false;
    }
}

void set_NT_trans (SXINT x, SXINT ref)
{
    if (ref > 0)
	sxtrap (ME, "out_rcvr_trans");
    
    ref = -ref;
    
    if (ref > PC.Mref)
	sxtrap (ME, "set_NT_trans");
    
    if ((ref -= PC.Mprdct) > 0) {
	SXBA_1_bit (TNT_trans_plus [ref], x);
    }
}


void set_T_trans (SXINT x, SXINT ref)
{
    if (ref > PC.Mref)
	sxtrap (ME, "set_T_trans");
    
    if ((ref -= PC.Mprdct) > 0) {
	SXBA_1_bit (TNT_trans_plus [ref], x);
    }
}


void out_rcvr_trans (void)
{
    /* On construit la BM T_trans qui a chaque terminal cle t associe l'ensemble
       des ref (a Mprdct pres) (chaque ref identifie un couple q = (etat, etq)) tel
       que l'on a "q ->*X q' ->t" cad il existe entre etat et etat' une
       sequence (eventuellement vide) de transitions (terminales ou non-terminales)
       et il existe depuis etq' une transition sur t. */
       
    /* Les Mref-M0ref premiers index de rcvr_nt_states referencent des etats
       tels que state==etq, les suivants sont paralleles aux elements de gotos. */


    SXINT			x, t, nt, nt_top; 
    SXINT			ref, next_action_kind;
    struct P_base	*abase;

    nt_top = PC.Mref - PC.Mprdct;
    TNT_trans_plus = sxbm_calloc (nt_top + 1, nt_top + 1);
    T_trans = sxbm_calloc (PC.tmax + 1, nt_top + 1);

    for (x = 1; x <= nt_top; x++) {
	if (x <= PC.M0ref - PC.Mprdct) {
	    abase = P.nt_bases + x;
	    set_first_trans (abase, &nt, &ref, PC.ntmax, &next_action_kind);
		
	    do {
		set_NT_trans (x, ref);
	    } while (set_next_trans (abase, &nt, &ref, PC.ntmax, &next_action_kind));
	}
	

	abase = P.t_bases + x;
	set_first_trans (abase, &t, &ref, PC.tmax, &next_action_kind);
	
	do {
	    set_T_trans (x, ref);

	    if (ref > 0 && SXBA_bit_is_set (R.PER_tset, t))
		SXBA_1_bit (T_trans [t], x);
	} while (set_next_trans (abase, &t, &ref, PC.tmax, &next_action_kind));
    }

    /* Fermeture transitive */
    fermer (TNT_trans_plus, nt_top + 1);

    t = 0;

    while ((t = sxba_scan (R.PER_tset, t)) > 0) {
	t_line = T_trans [t];
	t_line_0 = T_trans [0];
	sxba_copy (t_line_0, t_line);
	x = 0;

	while ((x = sxba_scan (t_line_0, x)) > 0) {
	    sxba_or (t_line, TNT_trans_plus [x]);
	}
    }

    t = 0;

    while ((t = sxba_scan (R.PER_tset, t)) > 0) {
	char	str [20];
	/* out_SXBA ("SXPBA_kt", t, T_trans [t]); le 2 avril 2002 */
	sprintf (str, "%ld", (SXINT) t);
	sxba_to_c (T_trans [t], stdout, "SXPBA_kt", str, true /* static */);
    }

    puts ("static SXBA SXPBM_trans[]={NULL,");
    t = 0;

    while ((t = sxba_scan (R.PER_tset, t)) > 0) {
	printf ("&SXPBA_kt%ld[0],\n", (SXINT) t);
    }

    out_end_struct ();

    sxbm_free (T_trans);
    sxbm_free (TNT_trans_plus);
}
