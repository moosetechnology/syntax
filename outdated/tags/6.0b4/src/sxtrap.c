/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1990 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *                                                      *
   ******************************************************** */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 09-04-90 18:15 (pb):	Creation					*/
/************************************************************************/

#include "sxunix.h"

char WHAT_SXTRAP[] = "@(#)SYNTAX - $Id: sxtrap.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

VOID	sxtrap (caller, message)
    char	*caller, *message;
{
    fprintf (sxstderr, "\nInternal system ERROR in \"%s\" during \"%s\".\n", caller, message);
    fprintf (sxstderr, "Please send informations (grammar, constructor options, ...) to\n\tPierre.Boullier@inria.fr\n");
    sxexit (SEVERITIES);
}
