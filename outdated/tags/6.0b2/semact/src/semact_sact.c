/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.        *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040212 11:15 (phd):	Modif pour calmer le compilo HPUX	*/
/************************************************************************/
/* 20030520 10:37 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 25-10-93 11:30 (pb):	Oubli du & ds sxsyntax (ACTION, &semact_tables)	*/
/*			Merci IBM...					*/
/************************************************************************/
/* 23-12-91 15:45 (pb):	Ajout du sxsrcpush malencontreusement oublie	*/
/*			le 19-12					*/
/************************************************************************/
/* 19-12-91 11:00 (pb):	Suite a la creation du point d'entree INIT du	*/
/*			parser (version 3.7a), l'appel d'un autre parser*/
/*			doit se faire par sxsyntax (ACTION, ...)	*/
/************************************************************************/
/* 25-05-87 18:02 (phd):	Ajout de l'entree SEMPASS	   	*/
/************************************************************************/
/* 25-05-87 16:18 (phd):	Point d'entree ERROR			*/
/************************************************************************/
/* 18-05-87 15:05 (pb&phd):	Adaptation au bnf nouveau		*/
/************************************************************************/
/* 16-03-87 08:55 (pb):		Ajout de @2				*/
/************************************************************************/
/* 23-12-86 17:36 (phd):	Correction du cas CLOSE			*/
/************************************************************************/
/* 22-12-86 11:21 (phd):	Renommage semact_parsact=>semact_semact	*/
/************************************************************************/
/* 10-12-86 18:10 (phd):	Amelioration d'un message d'erreur	*/
/************************************************************************/
/* 20-11-86 10:40 (pb):	Suppression de l'arret sur "$" en colonne 1   	*/
/************************************************************************/
/* 09-10-86 09:49 (phd):	Introduction de la semantique de semact	*/
/************************************************************************/
/* 08-10-86 09:43 (phd):	Introduction des actions de semact	*/
/************************************************************************/
/* 07-10-86 13:47 (phd):	Adaptation des actions scanner a semact	*/
/************************************************************************/
/* 12-09-86 10:56 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 12-09-86 10:55 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
#include "sem_by.h"
#include "B_tables.h"
#include "bnf_vars.h"
char WHAT_SEMACT_SACT[] = "@(#)SYNTAX - $Id: semact_sact.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;


static long	*user_actions;
static int	act_lgth, rule_no;



static VOID	gripe ()
{
    fputs ("\nA function of \"semact\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
}


SXVOID
semact_semact (code, numact)
    int		code;
    int		numact;
{

    switch (code) {
    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
    case SEMPASS:
    case ERROR:
	/* Work is done either in bnf's scanner actions or in semact's main */
	return;

    case ACTION:
	switch (numact) {
	case 1:
	    user_actions [rule_no] = atol (sxstrget (STACKtoken (STACKtop ()).string_table_entry));
	case 0:
	    return;
	default:
#if EBUG
	  sxtrap("semact_sact","unknown switch case #1");
#endif
	  break;
	}

    default:
	gripe ();
    }
}



extern struct sxtables	semact_tables;



BOOLEAN		semact_sem ()
{
    register int	i;
    register long	act;

    if (W.nbpro != rule_no) {
	sxtmsg (sxsrcmngr.source_coord.file_name, "%sInternal inconsistency %d-%d;\n\tactions will not be produced.\n",
	     semact_tables.err_titles [2], W.nbpro, rule_no);
	return FALSE;
    }

    for (i = 1; i <= rule_no; i++) {
	if ((act = user_actions [i]) != 0) {
	    WN [i].action = act;
	    WN [i].bprosimpl = FALSE;
	}
    }

    W.sem_kind = sem_by_action;
    return TRUE;
}



static int	semact_scan_act (code, act_no)
    int		code;
    int		act_no;
{
    static struct {
	       struct sxsvar	sxsvar;
	       struct sxplocals		sxplocals;
	   }	bnf, semact;

    switch (code) {
    case OPEN:
	user_actions = (long*) sxalloc (act_lgth = 128, sizeof (long));
	bnf.sxsvar = sxsvar;
	bnf.sxplocals = sxplocals;
	syntax (OPEN, &semact_tables);
	semact.sxsvar = sxsvar;
	semact.sxplocals = sxplocals;
	sxsvar = bnf.sxsvar;
	sxplocals = bnf.sxplocals;
	break;

    case CLOSE:
	bnf.sxsvar = sxsvar;
	bnf.sxplocals = sxplocals;
	sxsvar = semact.sxsvar;
	sxplocals = semact.sxplocals;
	syntax (CLOSE, &semact_tables);
	sxsvar = bnf.sxsvar;
	sxplocals = bnf.sxplocals;
	sxfree (user_actions);
	break;

    case INIT:
	rule_no = 0;

    case FINAL:
	break;

    case ACTION:
	switch (act_no) {
	default:
	    break;

	case 1:
	    if (act_lgth == ++rule_no) {
		user_actions = (long*) sxrealloc (user_actions, act_lgth *= 2, sizeof (long));
	    }

	    user_actions [rule_no] = 0;
	    bnf.sxsvar = sxsvar;
	    bnf.sxplocals = sxplocals;
	    sxsvar = semact.sxsvar;
	    sxplocals = semact.sxplocals;
	    sxsrcpush (EOF, ";", sxsrcmngr.source_coord);
/* Ca permet de commencer l'analyse des actions (apres le get_next_char() de la partie
   INIT du scanner) avec ";" comme previous char et le caractere suivant le ';' comme
   caractere courant. */
	    sxsyntax (ACTION, &semact_tables);
/*
	    (*(semact_tables.analyzers.scanner)) (INIT, &semact_tables);

	    if (semact_tables.SXS_tables.S_is_user_action_or_prdct)
		(*(semact_tables.SXS_tables.scanact)) (INIT, &semact_tables);

	    (*(semact_tables.analyzers.parser)) (ACTION, &semact_tables);

	    if (semact_tables.SXS_tables.S_is_user_action_or_prdct)
		(*(semact_tables.SXS_tables.scanact)) (FINAL, &semact_tables);

	    (*(semact_tables.analyzers.scanner)) (FINAL, &semact_tables);
*/
	    semact.sxsvar = sxsvar;
	    semact.sxplocals = sxplocals;
	    sxsvar = bnf.sxsvar;
	    sxplocals = bnf.sxplocals;
	    return 0;
	}

    default:
	gripe ();
	    /*NOTREACHED*/
    }

    return 0;
}



int	(*more_scan_act) () = {semact_scan_act};
