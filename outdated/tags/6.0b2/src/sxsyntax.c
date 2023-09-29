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
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *                                                      *
   ******************************************************** */




/* Module generique d'appel a SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 27-01-93 18:06 (pb) :	Appel de "INIT" du scanner et du parser	*/
/*				avant le "INIT" des scanact et parsact	*/
/*				afin de changer les valeurs par defaut.	*/
/************************************************************************/
/* 06-11-91 18:06 (pb) :	Ajout des points d'entree INIT et FINAL	*/
/*				au parser.				*/
/************************************************************************/
/* 25-05-87 11:03 (phd) :	Ajout d'un message d'erreur		*/
/************************************************************************/
/* 20-05-86 14:35 (phd):	L'appel des actions du scanner est	*/
/*				conditionne par son utilite		*/
/************************************************************************/
/* 16-05-86 17:00 (phd):	Nouvelle version des enchainements	*/
/*				d'appels, suite a discussion avec pb	*/
/************************************************************************/
/* 15-05-86 13:05 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 15-05-86 13:05 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "sxunix.h"

char WHAT_SXSYNTAX[] = "@(#)SYNTAX - $Id: sxsyntax.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

VOID	syntax (syntax_what, tables)
    int		syntax_what;
    register struct sxtables	*tables;
{
    switch (syntax_what) {
    case OPEN:
	(*(tables->analyzers.scanner)) (OPEN, tables);
	(*(tables->analyzers.parser)) (OPEN, tables);

	if (tables->SXS_tables.S_is_user_action_or_prdct)
	    (*(tables->SXS_tables.scanact)) (OPEN, tables);

	if (tables->SXP_tables.parsact != NULL)
	    (*(tables->SXP_tables.parsact)) (OPEN, tables);

	(*(tables->SXP_tables.semact)) (OPEN, tables);
	break;

    case CLOSE:
	(*(tables->SXP_tables.semact)) (CLOSE, tables);

	if (tables->SXP_tables.parsact != NULL)
	    (*(tables->SXP_tables.parsact)) (CLOSE, tables);

	if (tables->SXS_tables.S_is_user_action_or_prdct)
	    (*(tables->SXS_tables.scanact)) (CLOSE, tables);

	(*(tables->analyzers.parser)) (CLOSE, tables);
	(*(tables->analyzers.scanner)) (CLOSE, tables);
	break;

    case ACTION:
	(*(tables->analyzers.scanner)) (INIT, tables);
	(*(tables->analyzers.parser)) (INIT, tables);

	if (tables->SXS_tables.S_is_user_action_or_prdct)
	    (*(tables->SXS_tables.scanact)) (INIT, tables);

	if (tables->SXP_tables.parsact != NULL)
	    (*(tables->SXP_tables.parsact)) (INIT, tables);

	(*(tables->SXP_tables.semact)) (INIT, tables);
	(*(tables->analyzers.parser)) (ACTION, tables);
	(*(tables->SXP_tables.semact)) (FINAL, tables);

	if (tables->SXP_tables.parsact != NULL)
	    (*(tables->SXP_tables.parsact)) (FINAL, tables);

	if (tables->SXS_tables.S_is_user_action_or_prdct)
	    (*(tables->SXS_tables.scanact)) (FINAL, tables);

	(*(tables->analyzers.parser)) (FINAL, tables);
	(*(tables->analyzers.scanner)) (FINAL, tables);
	(*(tables->SXP_tables.semact)) (SEMPASS, tables);
	break;

    default:
	fprintf (sxstderr, "The function \"syntax\" is out of date with respect to its specification.\n");
	abort ();
    }
}
