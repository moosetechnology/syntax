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
/* 26-03-92 14:26 (pb):	Nouvelle adresse pour email			*/
/************************************************************************/
/* 09-04-90 18:15 (pb):	Creation					*/
/************************************************************************/

#ifndef lint
char	what_sxtrap [] = "@(#)sxtrap.c\t- SYNTAX [unix] - 26 Mars 1992";
#endif




#include "sxunix.h"


VOID	sxtrap (caller, message)
    char	*caller, *message;
{
    fprintf (sxstderr, "\nInternal constructor ERROR in \"%s\" during \"%s\".\n", caller, message);
    fprintf (sxstderr, "Please send informations (grammar, constructor options, ...) to\n\tPierre.Boullier@inria.fr\n");
    sxexit (SEVERITIES);
}
