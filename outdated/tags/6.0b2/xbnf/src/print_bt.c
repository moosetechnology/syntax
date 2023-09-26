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
/* 20030512 13:07 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 10-04-90 14:30 (phd):	Suppression des "typedef" de bl		*/
/*				Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
#include "B_tables.h"
#include "xbnf_vars.h"
char WHAT_XBNFPRINTBT[] = "@(#)SYNTAX - $Id: print_bt.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

extern SXVOID xbnf_dump ();

FILE	*sxstdout = {NULL}, *sxstderr = {NULL}, *sxtty;



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
    xbnf_dump ();
    bnf_free (&bnf_ag);
  }

  return EXIT_SUCCESS;
}
