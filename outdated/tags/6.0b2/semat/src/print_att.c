

#include "sxunix.h"
#include "T_tables.h"
char WHAT_SEMATPRINTATT[] = "@(#)SYNTAX - $Id: print_att.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

extern BOOLEAN	semat_read ();
extern VOID	semat_free ();
static struct T_ag_item		T_ag;



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int		i;

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }
  if (sxstderr == NULL) {
    sxstderr = stderr;
  }

  if (argc != 2) {
    printf ("usage: print_att language_name\n");
    return 1;
  }

  if (!semat_read (&T_ag, argv [1]))
    return 1;

  printf ("bnf_last_modif_time = %s\n", ctime (&T_ag.bnf_modif_time));
  printf ("T_nbpro = %d\n", T_ag.T_constants.T_nbpro);
  printf ("T_nbnod = %d\n", T_ag.T_constants.T_nbnod);
  printf ("T_stack_schema_size = %d\n", T_ag.T_constants.T_stack_schema_size);
  printf ("T_node_name_string_lgth = %d\n", T_ag.T_constants.T_node_name_string_lgth);
  printf ("T_ter_to_node_name_size = %d\n", T_ag.T_constants.T_ter_to_node_name_size);
  printf ("\n\ti\tT_node_info [i].T_node_name\tT_node_info [i].T_ss_indx\n");

  for (i = 1; i <= T_ag.T_constants.T_nbpro + 1; i++)
    printf ("\t%d\t\t\t%d\t\t\t\t%d\n", i, T_ag.SXT_node_info [i].T_node_name, T_ag.SXT_node_info [i].T_ss_indx);

  printf ("\n\ti\tT_stack_schema [i]\n");

  for (i = 1; i <= T_ag.T_constants.T_stack_schema_size; i++)
    printf ("\t%d\t\t%d\n", i, T_ag.T_stack_schema [i]);

  printf ("\n\tnode_code\tnode_name\n");

  for (i = 1; i <= T_ag.T_constants.T_nbnod; i++)
    printf ("\t%d\t\t%s\n", i, T_ag.T_node_name_string + T_ag.T_nns_indx [i]);

  printf ("\n\ti\tT_ter_to_node_name [i]\n");

  for (i = 1; i <= T_ag.T_constants.T_ter_to_node_name_size; i++)
    printf ("\t%d\t%d\n", i, T_ag.T_ter_to_node_name [i]);

  semat_free (&T_ag);

  return EXIT_SUCCESS;
}
