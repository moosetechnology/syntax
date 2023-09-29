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
   *  Produit de l'équipe Langages et Traducteurs.        *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030520 14:12 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 20030520 14:12 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

static char	ME [] = "tables_c";




#include "sxunix.h"

#include "tables.h"
#include "sem_by.h"
char WHAT_TABLESINITTBL[] = "@(#)SYNTAX - $Id: init_tbl.c 1120 2008-02-28 15:53:59Z rlacroix $" WHAT_DEBUG;

#define nbt 4


BOOLEAN		init_tables (char *name)
{
    static char		discrepancy_message [] =
	 "%s: Be careful: %s.%s and %s.%s do not come from the same grammar of %s.\n";
    static char		bad_followers_message [] =
	 "%s: Implementation restriction: Followers_Number value was too large (%d) and has been replaced by %d.\n";
    SXINT	j, i;
    static char		*t_name [nbt] = {"bt", "st", "rt",};
    long	bnf_time [nbt];

    if (sxverbosep)
	fputs ("\tCompatibility Tests\n", sxtty);


/* vérification de la date des fichiers */

    bnf_time [0] = P.bnf_modif_time;
    bnf_time [1] = S.bnf_modif_time;
    bnf_time [2] = R.bnf_modif_time;

    switch (PC.sem_kind) {
    case sem_by_abstract_tree:
	bnf_time [3] = T.bnf_modif_time;
	t_name [3] = "att";
	break;

    case sem_by_paradis:
	bnf_time [3] = PP.bnf_modif_time;
	t_name [3] = "ppt";
	break;

    default:
	bnf_time [3] = 0 /* temps vide */ ;
	t_name [3] = NULL /* Inutile! */ ;
	break;
    }

    for (i = 0; i < nbt - 1; i++) {
	if (bnf_time [i] != 0) {
	    for (j = i + 1; j < nbt; j++) {
		if (bnf_time [i] != bnf_time [j] && bnf_time [j] != 0)
		    fprintf (sxstderr, discrepancy_message, ME, name, t_name [i], name, t_name [j], name);
	    }
	}
    }

    if (RC.P_followers_number > 5) {
	fprintf (sxstderr, bad_followers_message, ME, RC.P_followers_number, 5);
	RC.P_followers_number = 5;
    }

    return TRUE;
}
