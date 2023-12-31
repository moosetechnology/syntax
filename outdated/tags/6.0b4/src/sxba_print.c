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

char WHAT_SXBAPRINT[] = "@(#)SYNTAX - $Id: sxba_print.c 940 2008-01-17 15:01:36Z rlacroix $" WHAT_DEBUG;

VOID	sxba_print (SXBA s)
        	  

/* affiche sur la sortie standard la chaine de bits s; procedure de service */

{
    SXBA_INDEX	i;

    for (i = 0; (SXBA_ELT)i < (*s); i++)
	putchar (sxba_bit_is_set (s, i) ? '1' : '0');

    putchar ('\n');
}
