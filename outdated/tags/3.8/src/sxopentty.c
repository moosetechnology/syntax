/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *                                                      *
   ******************************************************** */




/* Ouverture du fichier "sxtty" pour le mode "verbose" */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 31-07-87 16:02 (phd):	Creation				*/
/************************************************************************/

#ifndef lint
char	what_sxopentty [] = "@(#)sxopentty.c\t- SYNTAX [unix] - 31 Juillet 1987";
#endif


#include "sxunix.h"



VOID sxopentty ()
{
    sxtty = fopen ("/dev/tty", "a");
    if (sxtty != NULL && isatty (fileno (sxtty)))
	setbuf (sxtty, NULL);
    else {
	sxtty = fopen ("/dev/null", "w");
        if (sxtty == NULL) {
		/* ceci peut arriver sous Windows si /dev/null n'existe pas */
		sxtty = stderr;
        }
    }
}

