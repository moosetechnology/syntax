/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2005 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'�quipe ATOLL.  (phd)                   *
   *                                                      *
   ******************************************************** */




/* Gestion des erreurs dans l'utilisation des cha�nes de bits */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20050119 15:34 (phd) :	Cr�ation (phd)				*/
/************************************************************************/

# include	"sxunix.h"

char WHAT_SXBATRAP[] = "@(#)SYNTAX - $Id: sxbatrap.c 940 2008-01-17 15:01:36Z rlacroix $" WHAT_DEBUG;


VOID	sxbatrap (char *caller)
{
#if 0
  char commande [128];/* Certains SE, comme Digital Unix, n'ont pas snprintf */
#endif

    fprintf (sxstderr, "\nERROR detected in parameters during \"SXBA_%s\".\n", caller);
#if 0
    /* Le 17/06/05 : abandon car c,a m.... ds les grandes largeurs !! */
    fprintf (sxstderr, "Trying to give control to the debugger...\n");
    sprintf (commande, "(gdb --pid=%d 0>%s 1>%s 2>%s)2>%s", getpid (), SX_DEV_TTY, SX_DEV_TTY, SX_DEV_TTY, SX_DEV_NUL);
    system (commande);
    fprintf (sxstderr, "Sorry it did not work. Exiting...\n");
#endif /* 0 */
    sxexit (SEVERITIES);
}
