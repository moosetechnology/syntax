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
/* 20040213 14:51 (phd):	#if WINNT pour TTY&CON			*/
/************************************************************************/
/* 20020320 17:03 (phd):	Externalisation de SX_DEV_{TTY,NUL}	*/
/************************************************************************/
/* 20000922 08:14 (phd):	Adaptation � MINGW			*/
/************************************************************************/
/* 19991020 13:05 (phd):	Utilisation de O_TEXT (pour Cygwin)	*/
/************************************************************************/
/* 31-07-87 16:02 (phd):	Creation				*/
/************************************************************************/

#include "sxunix.h"

char WHAT_SXOPENTTY[] = "@(#)SYNTAX - $Id: sxopentty.c 620 2007-06-11 12:40:19Z rlacroix $" WHAT_DEBUG;

VOID sxopentty ()
{
  if ((sxtty = fopen (SX_DEV_TTY, "a")) == NULL || !isatty (fileno (sxtty))) {
    sxtty = fopen (SX_DEV_NUL, "w") ; 
    /* if (sxtty==NULL) sxverbosep=FALSE; */
  }
  else {
#if O_TEXT
    setmode (fileno (sxtty), O_TEXT);
#endif
    setbuf (sxtty, NULL);
  }
}
