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

#define SX_DFN_EXT_VAR_XBNF
#define SX_DFN_EXT_VAR

#include "sxunix.h"
#include "B_tables.h"
#include "xbnf_vars.h"
char WHAT_XBNFPRINTBT[] = "@(#)SYNTAX - $Id: print_bt.c 1135 2008-03-05 15:32:38Z sagot $" WHAT_DEBUG;

extern SXVOID xbnf_dump (void);

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
FILE	*sxstdout, *sxstderr, *sxtty;
#endif



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  char	*name;

  sxuse(argc); /* pour faire taire gcc -Wunused */
  sxstdout = stdout;
  sxstderr = stderr;

  name = argv [1];
  printf ("GRAMMAIRE:%s\n", name);

  if (bnf_read (&bnf_ag, name)) {
    xbnf_dump ();
    bnf_free (&bnf_ag);
  }

  return EXIT_SUCCESS;
}
