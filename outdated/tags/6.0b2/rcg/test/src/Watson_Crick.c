/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1997 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 25 Fev 1999 14:06:361 (PB)	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

static char	ME [] = "Watson_Crick.c";

#include "sxunix.h"
#include "rcg_glbl.h"
char WHAT_RCGTESTWATSON[] = "@(#)SYNTAX - $Id: Watson_Crick.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

static int Watson_Crick [5][5]= {
    /*	0	A	C	G	U	*/	         
/* 0 */{0,	0,	0,	0,	0},
/* A */{0,	0,	0,	0,	2},
/* C */{0,	0,	0,	4,	0},
/* G */{0,	0,	5,	0,	1},
/* U */{0,	3,	0,	0,	0}
};


BOOLEAN
_wc (rho0, ilb, iub)
    int *rho0, *ilb, *iub;
{
    return Watson_Crick [SOURCE(iub[0])][SOURCE(iub[1])];
}
