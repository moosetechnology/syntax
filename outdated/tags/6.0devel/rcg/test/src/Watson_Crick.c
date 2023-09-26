/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 
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


static char	ME [] = "Watson_Crick.c";

#include "sxunix.h"
#include "rcg_glbl.h"
char WHAT_RCGTESTWATSON[] = "@(#)SYNTAX - $Id: Watson_Crick.c 1404 2008-06-24 13:05:34Z rlacroix $" WHAT_DEBUG;

static SXINT Watson_Crick [5][5]= {
    /*	0	A	C	G	U	*/	         
/* 0 */{0,	0,	0,	0,	0},
/* A */{0,	0,	0,	0,	2},
/* C */{0,	0,	0,	4,	0},
/* G */{0,	0,	5,	0,	1},
/* U */{0,	3,	0,	0,	0}
};


SXBOOLEAN
_wc (rho0, ilb, iub)
    SXINT *rho0, *ilb, *iub;
{
    return Watson_Crick [SOURCE(iub[0])][SOURCE(iub[1])];
}
