/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1988 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *							  *
   ******************************************************** */





/* Lecture de tableaux de bits pour SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 09-04-90 11:30 (phd):	Reecriture				*/
/************************************************************************/
/* 10-05-88 10:15 (pb):		Creation				*/
/************************************************************************/

#ifndef lint
char	what_sxba_read [] = "@(#)sxba_read.c\t- SYNTAX [unix] - 9 Avril 1990";
#endif


#include "sxunix.h"
#include "sxba.h"


BOOLEAN		sxba_read (file, bits_array)
    int		file;
    register SXBA	bits_array;

/*
 * "sxba_read" reads the already allocated "bits_array" on "file".
 * "file" must be the result of "open".
 */

{
    int		bits_number, bytes_number;

    if (read (file, &bits_number, sizeof (SXBA_ELT)) != sizeof (SXBA_ELT) || bits_number != BASIZE (bits_array)) {
	return FALSE;
    }

    bytes_number = NBLONGS (bits_number) * sizeof (SXBA_ELT);
    return read (file, bits_array + 1, bytes_number) == bytes_number;
}
