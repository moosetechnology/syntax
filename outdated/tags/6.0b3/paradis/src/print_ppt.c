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
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *                                                      *
   ******************************************************** */





/* Impression en clair des tables du paragrapheur */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 31-08-88 17:10 (bl):         Ajout de #ifdef MSDOS			*/
/************************************************************************/
/* 29-10-86 10:50 (pb):	Ajout de cette rubrique "modifications"	   	*/
/************************************************************************/

#define SX_DFN_EXT_VAR

#include "sxunix.h"

#include "PP_tables.h"
char WHAT_PARADISPRINT[] = "@(#)SYNTAX - $Id: print_ppt.c 1135 2008-03-05 15:32:38Z sagot $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE	*sxstdout, *sxstderr;
FILE	*sxtty;
#endif

extern BOOLEAN	paradis_read ();
extern VOID paradis_free ();
extern char	*ctime ();
static struct PP_ag_item	PP_ag;



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  SXINT		i;

  sxstdout = stdout;
  sxstderr = stderr;

  if (argc != 2) {
    printf ("usage: print_ppt language_name\n");
    return EXIT_FAILURE;
  }

  if (!paradis_read (&PP_ag, argv [1]))
    return EXIT_FAILURE;

  printf ("bnf_last_modif_time = %s\n", ctime (&PP_ag.bnf_modif_time));
  printf ("PP_indx_lgth = %d\n", PP_ag.PP_constants.PP_indx_lgth);
  printf ("PP_schema_lgth = %d\n", PP_ag.PP_constants.PP_schema_lgth);
  printf ("\n\ti\tPP_indx [i]\n");

  for (i = 1; i <= PP_ag.PP_constants.PP_indx_lgth; i++)
    printf ("\t%d\t%d\n", i, PP_ag.PP_indx [i]);

  printf ("\n\ti\tPP_schema [i].PP_codop\tPP_schema [i].PP_act\n");

  for (i = 1; i <= PP_ag.PP_constants.PP_schema_lgth; i++)
    printf ("\t%d\t%d\t\t\t%d\n", i, PP_ag.SXPP_schema [i].PP_codop, PP_ag.SXPP_schema [i].PP_act);

  paradis_free (&PP_ag); /* end print_ppt */


  return EXIT_SUCCESS;
}
