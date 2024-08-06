/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1994 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ChLoE.			  *
   *                                                      *
   ******************************************************** */

/* Execute les actions et predicats syntaxiques de la grammaire wcw3.lig. */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 23-08-94 13:40 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)wcw3_parsact.c	- SYNTAX [unix] - Mar 23 Aou 1994 13:40:43";

static char	ME [] = "wcw3_parsact";

#include "sxunix.h"
#include "wcw3_tdef.h"

static int	middle;

bool wcw3_parsact (int which, struct sxtables	*arg)
{
    int act_no;

    switch (which) {
    case SXOPEN:
    case SXCLOSE:
	return SXANY_BOOL;

    case SXINIT:
	/* On lit tout le texte source */
	do {
	    (*(sxplocals.SXP_tables.P_scan_it)) ();
	} while (SXGET_TOKEN (sxplocals.Mtok_no).lahead != sxplocals.SXP_tables.P_tmax);

	/* Mtok_no doit etre pair */
	if (sxplocals.Mtok_no & 01)
	{
	    /* On insere 'c' en (Mtok_no + 1) / 2 */
	    struct sxtoken	new_token;

	    middle = (sxplocals.Mtok_no + 1) >> 1;
	    new_token = SXGET_TOKEN (middle);
	    new_token.lahead = c_code;
	    sxtknmdf (&new_token, 1, sxplocals.atok_no, 0);

	    sxput_error (new_token.source_index,
			 "%s\"c\" is inserted before \"%s\".",
			 sxplocals.sxtables->err_titles [2],
			 new_token.lahead == a_code ? "a" : "b");
	}
	else
	    middle = sxplocals.Mtok_no >> 1;

	/* Le milieu doit etre un 'c' */
	if (SXGET_TOKEN (middle).lahead != c_code)
	{
	   sxput_error (SXGET_TOKEN (middle).source_index,
			 "%s\"%s\" is replaced by \"c\".",
			 sxplocals.sxtables->err_titles [2],
			 SXGET_TOKEN (middle).lahead == a_code ? "a" : "b"); 
	   SXGET_TOKEN (middle).lahead = c_code;
	}

	return SXANY_BOOL;

    case SXFINAL:
	return SXANY_BOOL;

    case SXACTION:
    case SXDO:
    case SXUNDO:
	break;

    case SXPREDICATE:
	act_no = (int) arg;

	if (act_no != 0)
	    break;

	return sxplocals.ptok_no == middle;

    default:
	fputs ("The function \"wcw3_parsact\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }

    return SXANY_BOOL;
}

