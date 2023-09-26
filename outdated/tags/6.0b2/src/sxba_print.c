/* ********************************************************
   *							  *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *			and Universite d'Orleans	  *
   *							  *
   ******************************************************** */


/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */


#include "sxunix.h"

char WHAT_SXBAPRINT[] = "@(#)SYNTAX - $Id: sxba_print.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

VOID	sxba_print (s)
    register SXBA	s;

/* affiche sur la sortie standard la chaine de bits s; procedure de service */

{
    register int	i;

    for (i = 0; (SXBA_ELT)i < (*s); i++)
	putchar (sxba_bit_is_set (s, i) ? '1' : '0');

    putchar ('\n');
}
