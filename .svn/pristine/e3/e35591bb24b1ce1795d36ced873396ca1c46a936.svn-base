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


VOID	sxba_print (s)
    register SXBA	s;

/* affiche sur la sortie standard la chaine de bits s; procedure de service */

{
    register int	i;

    for (i = 0; i < *s; i++)
	putchar (sxba_bit_is_set (s, i) ? '1' : '0');

    putchar ('\n');
}
