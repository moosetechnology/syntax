/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */

/****************************************************************/
/*                                                              */
/*                                                              */
/*  This program has been translated from get_column_number.pl1 */
/*  on Thursday the twenty-fourth of April, 1986, at 13:29:14,  */
/*  on the Multics system at INRIA,                             */
/*  by the release 3.3g PL1_C translator of INRIA,              */
/*         developped by the "Langages et Traducteurs" team,    */
/*         using the SYNTAX (*), FNC and Paradis systems.       */
/*                                                              */
/*                                                              */
/****************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                          */
/****************************************************************/
#include "sxcommon.h"
char WHAT_LECLGETCOLNB[] = "@(#)SYNTAX - $Id: get_col_nb.c 1114 2008-02-27 15:13:51Z rlacroix $" WHAT_DEBUG;

SXINT	get_column_number (char *string, SXINT index)
{
    /* Cette procedure retourne la colonne d'affichage du caractere d'index 
   "index" de la chaine "string". On suppose que les positions de tabulation
   sont tous les "tabs" (1, tabs+1, 2*tabs+1, ...) et que le debut de la chaine
   s'affiche en colonne 1 */

#define tabs 8
#define BS 8
#define HT 9
    char	*s, *x;
    SXINT		col;

    x = string + (index - 1);
    col = 0;

    for (s = string; s < x; s++)
	if (*s == HT)
	    col += tabs - (col % tabs);
	else if (*s == BS)
	    col--;
	else
	    col++;

    return col;
}
