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
   *  Produit de l'equipe ChLoE (PB)			  *
   *                                                      *
   ******************************************************** */


/* Procedure commune a str_mngr et word_mngr. */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 01-04-94 10:00 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include        "sxcommon.h"
#include	"sxscrmbl.h"

char WHAT_SXSCRMBL[] = "@(#)SYNTAX - $Id: sxscrmbl.c 1042 2008-02-15 08:40:28Z rlacroix $" WHAT_DEBUG;

#ifndef SXBITS_PER_LONG
#define SXBITS_PER_LONG	32
#endif


#if SXBITS_PER_LONG==64
SXUINT
sxscrmbl (char *string, SXUINT length)
{
    SXUINT	scramble;
    char		*last;

    switch (scramble = length) {
    case 0:
	return 0;

    default:
	last = string + length - 1;
	scramble += (SXUINT) *last-- << 48;
	scramble += (SXUINT) *last-- << 56;
	scramble += (SXUINT) *last-- << 52;
	scramble += (SXUINT) *last-- << 36;
	scramble += (SXUINT) *last << 44;

	last = string + length/2 + 1;
	scramble += (SXUINT) *last-- << 60;
	scramble += (SXUINT) *last-- << 40;
	scramble += (SXUINT) *last-- << 32;
	scramble += (SXUINT) *last-- << 16;
	scramble += (SXUINT) *last << 24;

	goto k5;

    case 15:
	scramble += (SXUINT) *string++ << 48;

    case 14:
	scramble += (SXUINT) *string++ << 56;

    case 13:
	scramble += (SXUINT) *string++ << 52;

    case 12:
	scramble += (SXUINT) *string++ << 36;

    case 11:
	scramble += (SXUINT) *string++ << 44;

    case 10:
	scramble += (SXUINT) *string++ << 60;

    case 9:
	scramble += (SXUINT) *string++ << 40;

    case 8:
	scramble += (SXUINT) *string++ << 32;

    case 7:
	scramble += (SXUINT) *string++ << 16;

    case 6:
	scramble += (SXUINT) *string++ << 24;

    case 5:
    k5:
	scramble += (SXUINT) *string++ << 20;

    case 4:
	scramble += (SXUINT) *string++ << 4;

    case 3:
	scramble += (SXUINT) *string++ << 12;

    case 2:
	scramble += (SXUINT) *string++ << 28;

    case 1:
	return (scramble + ((SXUINT) *string << 8));
    }
}
#else
SXUINT
sxscrmbl (char *string, SXUINT length)
{
    SXUINT	scramble;
    char		*last;

    switch (scramble = length) {
    case 0:
	return 0;

    default:
	last = string + length - 1;
	scramble += (SXUINT) *last-- << 16;
	scramble += (SXUINT) *last << 24;

	last = string + length/2;
	scramble += (SXUINT) *last-- << 20;
	scramble += (SXUINT) *last << 4;
	goto k3;

    case 7:
	scramble += (SXUINT) *string++ << 16;

    case 6:
	scramble += (SXUINT) *string++ << 24;

    case 5:
	scramble += (SXUINT) *string++ << 20;

    case 4:
	scramble += (SXUINT) *string++ << 4;

    case 3:
    k3:
	scramble += (SXUINT) *string++ << 12;

    case 2:
	scramble += (SXUINT) *string++ << 28;

    case 1:
	return (scramble + ((SXUINT) *string << 8));
    }
}
#endif

