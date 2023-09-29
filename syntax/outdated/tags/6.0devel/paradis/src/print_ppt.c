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
/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 










/* Impression en clair des tables du paragrapheur */




#include "sxunix.h"

#include "PP_tables.h"
char WHAT_PARADISPRINT[] = "@(#)SYNTAX - $Id: print_ppt.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

extern SXBOOLEAN	paradis_read (struct PP_ag_item *PP_ag, char *langname);
static struct PP_ag_item	PP_ag;



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  SXINT		i;

  sxopentty ();

  if (argc != 2) {
    printf ("usage: print_ppt language_name\n");
    return EXIT_FAILURE;
  }

  if (!paradis_read (&PP_ag, argv [1]))
    return EXIT_FAILURE;

  printf ("bnf_last_modif_time = %s\n", ctime (&PP_ag.bnf_modif_time));
  printf ("PP_indx_lgth = %ld\n", (long)PP_ag.PP_constants.PP_indx_lgth);
  printf ("PP_schema_lgth = %ld\n", (long)PP_ag.PP_constants.PP_schema_lgth);
  printf ("\n\ti\tPP_indx [i]\n");

  for (i = 1; i <= PP_ag.PP_constants.PP_indx_lgth; i++)
    printf ("\t%ld\t%ld\n", (long)i, (long)PP_ag.PP_indx [i]);

  printf ("\n\ti\tPP_schema [i].PP_codop\tPP_schema [i].PP_act\n");

  for (i = 1; i <= PP_ag.PP_constants.PP_schema_lgth; i++)
    printf ("\t%ld\t%ld\t\t\t%ld\n", (long)i, (long)PP_ag.SXPP_schema [i].PP_codop, (long)PP_ag.SXPP_schema [i].PP_act);

  paradis_free (&PP_ag); /* end print_ppt */


  return EXIT_SUCCESS;
}
