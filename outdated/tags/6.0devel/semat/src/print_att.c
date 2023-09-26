/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/







#include "sxunix.h"
#include "T_tables.h"
char WHAT_SEMATPRINTATT[] = "@(#)SYNTAX - $Id: print_att.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

extern SXBOOLEAN	semat_read (struct T_ag_item *T_ag, char *langname);
extern SXVOID	semat_free (struct T_ag_item *T_ag);
static struct T_ag_item		T_ag;



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  SXINT		i;

  sxopentty ();

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
