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

char WHAT_SXSCRMBL[] = "@(#)SYNTAX - $Id: sxscrmbl.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

#ifndef SXBITS_PER_LONG
#define SXBITS_PER_LONG	32
#endif


#if SXBITS_PER_LONG==64
unsigned long int
sxscrmbl (string, length)
    char		*string;
    unsigned long int	length;
{
    unsigned long int	scramble;
    char		*last;

    switch (scramble = length) {
    case 0:
	return 0;

    default:
	last = string + length - 1;
	scramble += (unsigned long int) *last-- << 48;
	scramble += (unsigned long int) *last-- << 56;
	scramble += (unsigned long int) *last-- << 52;
	scramble += (unsigned long int) *last-- << 36;
	scramble += (unsigned long int) *last << 44;

	last = string + length/2 + 1;
	scramble += (unsigned long int) *last-- << 60;
	scramble += (unsigned long int) *last-- << 40;
	scramble += (unsigned long int) *last-- << 32;
	scramble += (unsigned long int) *last-- << 16;
	scramble += (unsigned long int) *last << 24;

	goto k5;

    case 15:
	scramble += (unsigned long int) *string++ << 48;

    case 14:
	scramble += (unsigned long int) *string++ << 56;

    case 13:
	scramble += (unsigned long int) *string++ << 52;

    case 12:
	scramble += (unsigned long int) *string++ << 36;

    case 11:
	scramble += (unsigned long int) *string++ << 44;

    case 10:
	scramble += (unsigned long int) *string++ << 60;

    case 9:
	scramble += (unsigned long int) *string++ << 40;

    case 8:
	scramble += (unsigned long int) *string++ << 32;

    case 7:
	scramble += (unsigned long int) *string++ << 16;

    case 6:
	scramble += (unsigned long int) *string++ << 24;

    case 5:
    k5:
	scramble += (unsigned long int) *string++ << 20;

    case 4:
	scramble += (unsigned long int) *string++ << 4;

    case 3:
	scramble += (unsigned long int) *string++ << 12;

    case 2:
	scramble += (unsigned long int) *string++ << 28;

    case 1:
	return (scramble + ((unsigned long int) *string << 8));
    }
}
#else
unsigned long int
sxscrmbl (string, length)
    char		*string;
    unsigned long int	length;
{
    unsigned long int	scramble;
    char		*last;

    switch (scramble = length) {
    case 0:
	return 0;

    default:
	last = string + length - 1;
	scramble += (unsigned long int) *last-- << 16;
	scramble += (unsigned long int) *last << 24;

	last = string + length/2;
	scramble += (unsigned long int) *last-- << 20;
	scramble += (unsigned long int) *last << 4;
	goto k3;

    case 7:
	scramble += (unsigned long int) *string++ << 16;

    case 6:
	scramble += (unsigned long int) *string++ << 24;

    case 5:
	scramble += (unsigned long int) *string++ << 20;

    case 4:
	scramble += (unsigned long int) *string++ << 4;

    case 3:
    k3:
	scramble += (unsigned long int) *string++ << 12;

    case 2:
	scramble += (unsigned long int) *string++ << 28;

    case 1:
	return (scramble + ((unsigned long int) *string << 8));
    }
}
#endif

