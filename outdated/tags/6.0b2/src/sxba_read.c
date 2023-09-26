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
/* 20041209 11:40 (phd):	Correction de type pour 64 bits		*/
/************************************************************************/
/* 20030505 11:05 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 15-02-94 18:19 (pb):		Ne depend plus de "sxunix.h"		*/
/************************************************************************/
/* 09-04-90 11:30 (phd):	Reecriture				*/
/************************************************************************/
/* 10-05-88 10:15 (pb):		Creation				*/
/************************************************************************/



#include "sxcommon.h"
#include "sxba.h"
#include <unistd.h>

char WHAT_SXBA_READ[] = "@(#)SYNTAX - $Id: sxba_read.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;


BOOLEAN
sxba_read (int file, SXBA bits_array)

/*
 * "sxba_read" reads the already allocated "bits_array" on "file".
 * "file" must be the result of "open".
 */

{
    SXBA_ELT	bits_number, bytes_number;

    if (read (file, &bits_number, sizeof (SXBA_ELT)) != sizeof (SXBA_ELT) || bits_number != BASIZE (bits_array)) {
	return FALSE;
    }

    bytes_number = NBLONGS (bits_number) * sizeof (SXBA_ELT);
    return (SXBA_ELT)(read (file, bits_array + 1, bytes_number)) == bytes_number;
}
