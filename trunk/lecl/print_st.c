/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

#define SX_DFN_EXT_VAR_LECL

/* Le 24/01/2003
   Pour avoir les bonnes macros pour STMT, KEEP, SCAN, CODOP, NEXT ... */
#define SXS_MAX (SHRT_MAX+1)

#include "sxversion.h"
#include "sxunix.h"
#include "sxsstmt.h"
#include "S_tables.h"

char WHAT_LECLPRINTST[] = "@(#)SYNTAX - $Id: print_st.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

extern bool	lecl_read (struct lecl_tables_s *lecl_tables_ptr, char *langname);
extern void	lecl_free (struct lecl_tables_s *lecl_tables_ptr);
static transition_matrix_s	*tm;
static struct action_or_prdct_code_s	*ap;
static struct S_adrp_s	*ad;
static struct lecl_tables_s	lecl_tables;

/************************************************************************/
/* main function */
/************************************************************************/

int main (int argc, char *argv[])
{
  SXINT		i, j;

  sxopentty ();

  if (argc != 2) {
    printf ("usage: print_st language_name\n");
    sxexit (2);
  }

  if (!lecl_read (&lecl_tables, argv [1]))
    sxexit (1);

  printf ("bnf_last_modif_time = %s\n", ctime (&lecl_tables.bnf_modif_time));
  printf ("constructor_kind = %ld\n", (SXINT) lecl_tables.S_tbl_size.S_constructor_kind);
  printf ("last_char_code = %ld\n", (SXINT) lecl_tables.S_tbl_size.S_last_char_code);
  printf ("last_simple_class_no = %ld\n", (SXINT) lecl_tables.S_tbl_size.S_last_simple_class_no);
  printf ("last_state_no = %ld\n", (SXINT) lecl_tables.S_tbl_size.S_last_state_no);
  printf ("last_static_ste = %ld\n", (SXINT) lecl_tables.S_tbl_size.S_last_static_ste);
  printf ("terlis_lgth = %ld\n", (SXINT) lecl_tables.S_tbl_size.S_terlis_lgth);
  printf ("check_kw_lgth = %ld\n", (SXINT) lecl_tables.S_tbl_size.S_check_kw_lgth);
  printf ("termax = %ld\n", (SXINT) lecl_tables.S_tbl_size.S_termax);
  printf ("xprod = %ld\n", (SXINT) lecl_tables.S_tbl_size.S_xprod);
  printf ("counters_size = %ld\n", (SXINT) lecl_tables.S_tbl_size.S_counters_size);
  printf ("are_comments_erased = %d\n", lecl_tables.S_tbl_size.S_are_comments_erased);
  printf ("is_user_action_or_prdct = %d\n", lecl_tables.S_tbl_size.S_is_user_action_or_prdct);
  printf ("\nchar_to_simple_class\n");

  for (i = 0; i <= lecl_tables.S_tbl_size.S_last_char_code; i++)
    if (lecl_tables.S_char_to_simple_class [i] != 1)
      printf ("   %s\t%ld\n", SXCHAR_TO_STRING (i), (SXINT) lecl_tables.S_char_to_simple_class [i]);

  printf ("\ntransition_matrix\n");

  for (i = 1; i <= lecl_tables.S_tbl_size.S_last_state_no; i++) {
    printf ("STATE %ld\n", (SXINT) i);
    printf ("\tclasse\t(keep\tscan\tcodop\tnext\n");
    tm = *(lecl_tables.S_transition_matrix + i);

    for (j = 1; j <= lecl_tables.S_tbl_size.S_last_simple_class_no; j++) {
      tm++;

      if (CODOP (*tm) != Error)
	printf ("\t%ld\t(%c\t%c\t%lu\t%lu)\n", (SXINT) j, (!KEEP (*tm) && SCAN (*tm)) ? '-' : ' ', (SCAN (*tm)) ? '*' : ' ',
		(SXUINT)CODOP (*tm), (SXUINT)NEXT (*tm));
    }
  }

  printf ("\naction_or_prdct_code\n");
  printf ("x\t(keep\tscan\tcodop\tnext)\tno\tparam\tis_system\tkind\n");

  for (i = 1; i <= lecl_tables.S_tbl_size.S_xprod; i++) {
    ap = &(lecl_tables.SXS_action_or_prdct_code [i]);
    printf ("%ld\t(%c\t%c\t%lu\t%lu)\t%ld\t%ld\t%d\t\t%d\n", (SXINT) i, (!KEEP (ap->stmt) && SCAN (ap->stmt)) ? '-' : ' ', (SCAN (
														     ap->stmt)) ? '*' : ' ', CODOP (ap->stmt), (SXUINT)NEXT (ap->stmt), (SXINT) ap->action_or_prdct_no, (SXINT) ap->param, ap->is_system,
	    (int)ap->kind);
  }

  printf ("\nadrp\n");
  printf ("x\txter\tsyno\tlgth\n");

  for (i = 0; i <= lecl_tables.S_tbl_size.S_last_static_ste; i++) {
    ad = &(lecl_tables.SXS_adrp [i]);
    printf ("%ld\t%ld\t%ld\t%ld\n", (SXINT) i, (SXINT) ad->xterlis, (SXINT) ad->syno_lnk, (SXINT) ad->lgth);
  }

  printf ("\nterlis\n");
  printf ("\"%s\"\n", lecl_tables.S_terlis);
  printf ("\ncheck_kw\n");

  if (lecl_tables.S_check_kw != NULL)
    printf ("%s\n", lecl_tables.S_check_kw);
  else
    printf ("%s\n", "There is no keyword.");

  printf ("\nis_a_generic_terminal\n");
  sxba_print (lecl_tables.S_is_a_generic_terminal);
  printf ("\nis_a_keyword\n");
  sxba_print (lecl_tables.S_is_a_keyword);
  lecl_free (&lecl_tables);
  sxexit (0);

  return EXIT_SUCCESS;
}
