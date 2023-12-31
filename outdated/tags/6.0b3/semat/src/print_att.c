/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) ???? by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */

#define SX_DFN_EXT_VAR


#include "sxunix.h"
#include "T_tables.h"
char WHAT_SEMATPRINTATT[] = "@(#)SYNTAX - $Id: print_att.c 1135 2008-03-05 15:32:38Z sagot $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE	*sxstdout, *sxstderr, *sxtty;
#endif

extern BOOLEAN	semat_read (struct T_ag_item *T_ag, char *langname);
extern VOID	semat_free (struct T_ag_item *T_ag);
static struct T_ag_item		T_ag;



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  SXINT		i;

  sxstdout = stdout;
  sxstderr = stderr;

  if (argc != 2) {
    printf ("usage: print_att language_name\n");
    return 1;
  }

  if (!semat_read (&T_ag, argv [1]))
    return 1;

  printf ("bnf_last_modif_time = %s\n", ctime (&T_ag.bnf_modif_time));
  printf ("T_nbpro = %ld\n", (long)T_ag.T_constants.T_nbpro);
  printf ("T_nbnod = %ld\n", (long)T_ag.T_constants.T_nbnod);
  printf ("T_stack_schema_size = %ld\n", (long)T_ag.T_constants.T_stack_schema_size);
  printf ("T_node_name_string_lgth = %ld\n", (long)T_ag.T_constants.T_node_name_string_lgth);
  printf ("T_ter_to_node_name_size = %ld\n", (long)T_ag.T_constants.T_ter_to_node_name_size);
  printf ("\n\ti\tT_node_info [i].T_node_name\tT_node_info [i].T_ss_indx\n");

  for (i = 1; i <= T_ag.T_constants.T_nbpro + 1; i++)
    printf ("\t%ld\t\t\t%ld\t\t\t\t%ld\n", (long)i, (long)T_ag.SXT_node_info [i].T_node_name, (long)T_ag.SXT_node_info [i].T_ss_indx);

  printf ("\n\ti\tT_stack_schema [i]\n");

  for (i = 1; i <= T_ag.T_constants.T_stack_schema_size; i++)
    printf ("\t%ld\t\t%ld\n", (long)i, (long)T_ag.T_stack_schema [i]);

  printf ("\n\tnode_code\tnode_name\n");

  for (i = 1; i <= T_ag.T_constants.T_nbnod; i++)
    printf ("\t%ld\t\t%s\n", (long)i, T_ag.T_node_name_string + T_ag.T_nns_indx [i]);

  printf ("\n\ti\tT_ter_to_node_name [i]\n");

  for (i = 1; i <= T_ag.T_constants.T_ter_to_node_name_size; i++)
    printf ("\t%ld\t%ld\n", (long)i, (long)T_ag.T_ter_to_node_name [i]);

  semat_free (&T_ag);

  return EXIT_SUCCESS;
}
