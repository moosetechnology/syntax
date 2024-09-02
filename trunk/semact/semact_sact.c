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
#include "sem_by.h"
#include "B_tables.h"
#include "bnf_vars.h"

char WHAT_SEMACT_SACT[] = "@(#)SYNTAX - $Id: semact_sact.c 4183 2024-08-26 17:07:35Z garavel $" WHAT_DEBUG;

static SXINT	*user_actions;
static SXINT	act_lgth, rule_no;


static void	gripe (void)
{
    fputs ("\nA function of \"semact\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}


void semact_semact (SXINT code, SXINT numact, SXTABLES *arg)
{
    (void) arg;
    switch (code) {
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
    case SXSEMPASS:
    case SXERROR:
	/* Work is done either in bnf's scanner actions or in semact's main */
	return;

    case SXACTION:
	switch (numact) {
	case 1:
	    user_actions [rule_no] = atol (sxstrget (SXSTACKtoken (SXSTACKtop ()).string_table_entry));
	    return;
	case 0:
	    return;
	default:
#if EBUG
	  sxtrap("semact_sact","unknown switch case #1");
#endif
	  break;
	}
        /* FALLTHROUGH */ /* added by W. Serwe, confirmed by P. Boullier */ 

    default:
	gripe ();
    }
}



extern SXTABLES	sxtables;



bool		semact_sem (void)
{
    SXINT	i;
    SXINT	act;

    if (W.nbpro != rule_no) {
	sxtmsg (sxsrcmngr.source_coord.file_name, "%sInternal inconsistency %ld-%ld;\n\tactions will not be produced.\n",
	     sxtables.err_titles [2]+1, W.nbpro, rule_no);
	return false;
    }

    for (i = 1; i <= rule_no; i++) {
	if ((act = user_actions [i]) != 0) {
	    WN [i].action = act;
	    WN [i].bprosimpl = false;
	}
    }

    W.sem_kind = sem_by_action;
    return true;
}



bool semact_scan_act (SXINT code, SXINT act_no)
{
    static struct {
	       struct sxsvar	sxsvar;
	       struct sxplocals		sxplocals;
	   }	bnf, semact;

    switch (code) {
    case SXOPEN:
	user_actions = (SXINT *) sxalloc (act_lgth = 128, sizeof (SXINT));
	bnf.sxsvar = sxsvar;
	bnf.sxplocals = sxplocals;
	syntax (SXOPEN, &sxtables);
	semact.sxsvar = sxsvar;
	semact.sxplocals = sxplocals;
	sxsvar = bnf.sxsvar;
	sxplocals = bnf.sxplocals;
	break;

    case SXCLOSE:
	bnf.sxsvar = sxsvar;
	bnf.sxplocals = sxplocals;
	sxsvar = semact.sxsvar;
	sxplocals = semact.sxplocals;
	syntax (SXCLOSE, &sxtables);
	sxsvar = bnf.sxsvar;
	sxplocals = bnf.sxplocals;
	sxfree (user_actions);
	break;

    case SXINIT:
	rule_no = 0;
	break;

    case SXFINAL:
	break;

    case SXACTION:
	switch (act_no) {
	case 1:
	    if (act_lgth == ++rule_no) {
		user_actions = (SXINT *) sxrealloc (user_actions, act_lgth *= 2, sizeof (SXINT));
	    }

	    user_actions [rule_no] = 0;
	    bnf.sxsvar = sxsvar;
	    bnf.sxplocals = sxplocals;
	    sxsvar = semact.sxsvar;
	    sxplocals = semact.sxplocals;
	    sxsrcpush (EOF, ";", sxsrcmngr.source_coord);
/* Ca permet de commencer l'analyse des actions (apres le get_next_char() de la partie
   SXINIT du scanner) avec ";" comme previous char et le caractere suivant le ';' comme
   caractere courant. */
	    sxsyntax (SXACTION, &sxtables);
	    semact.sxsvar = sxsvar;
	    semact.sxplocals = sxplocals;
	    sxsvar = bnf.sxsvar;
	    sxplocals = bnf.sxplocals;
	    return SXANY_BOOL;

	default:
	    break;
	}
        /* FALLTHROUGH */ /* added by W. Serwe, confirmed by P. Boullier */

    default:
	gripe ();
	    /*NOTREACHED*/
    }

    return SXANY_BOOL;
}



