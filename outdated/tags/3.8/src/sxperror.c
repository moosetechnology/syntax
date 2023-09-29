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




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 17-12-86 12:10 (phd):	Creation				*/
/************************************************************************/

#ifndef lint
char	what_sxperror [] = "@(#)sxperror.c\t- SYNTAX [unix] - 17 Decembre 1986";
#endif

#include "sxunix.h"
#include <errno.h>

#ifndef __STDC__

/* ici, on n'est pas en ANSI C et on suppose que la fonction strerror() manque 
   et que les variables sys_nerr et sys_errlist[] existent : on definit la 
   fonction strerror a partir de ces variables */

static char *strerror (errnum)
     int errnum;
{
  extern char *sys_errlist[];
  extern int sys_nerr;

  if (errnum > 0 && errnum <= sys_nerr)
    return sys_errlist[errnum];
  else
    return NULL;
}

#else

#include <string.h>

/* ici, on est en ANSI C et string.h contient la declaration de strerror() */

#endif


VOID	sxperror (string)
    char	*string;
{
    char *sys_errmsg;

    sys_errmsg = strerror (errno);
    fprintf (sxstderr, "%s: %s.\n", string, (sys_errmsg == NULL) ? "Unknown error" : sys_errmsg);
}

