/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030527 15:57 (phd):	#define SX_DFN_EXT_VAR			*/
/************************************************************************/
/* 20030512 13:07 (phd):	Adaptation � SGI (64 bits)		*/
/************************************************************************/
/* 10-04-90 14:30 (phd):	Suppression des "typedef" de bl		*/
/*				Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#define SX_DFN_EXT_VAR	/* Pour d�finir les variables globales */
#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"
char WHAT_PRINTBT[] = "@(#)SYNTAX - $Id: print_bt.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;

extern SXVOID bnf_dump ();

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  char	*name;

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }
  if (sxstderr == NULL) {
    sxstderr = stderr;
  }

  name = argv [1];
  printf ("GRAMMAIRE:%s\n", name);

  if (bnf_read (&bnf_ag, name)) {
    bnf_dump ();
    bnf_free (&bnf_ag);
  }

  return EXIT_SUCCESS;
}
