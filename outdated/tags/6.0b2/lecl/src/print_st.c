/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1984 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */

/* Le 24/01/2003
   Pour avoir les bonnes macros pour STMT, KEEP, SCAN, CODOP, NEXT ... */
#define SXS_MAX (SHRT_MAX+1)
#include "sxunix.h"
#include "sxsstmt.h"
#include "S_tables.h"
char WHAT_LECLPRINTST[] = "@(#)SYNTAX - $Id: print_st.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

BOOLEAN  sxverbosep;
extern BOOLEAN	lecl_read ();
extern VOID	lecl_free ();
static transition_matrix_s	*tm;
static struct action_or_prdct_code_s	*ap;
static struct S_adrp_s	*ad;
static struct lecl_tables_s	lecl_tables;



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int		i, j;

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }
  if (sxstderr == NULL) {
    sxstderr = stderr;
  }

  if (argc != 2) {
    printf ("usage: print_st language_name\n");
    SXEXIT (2);
  }

  if (!lecl_read (&lecl_tables, argv [1]))
    SXEXIT (1);

  printf ("bnf_last_modif_time = %s\n", ctime (&lecl_tables.bnf_modif_time));
  printf ("constructor_kind = %d\n", lecl_tables.S_tbl_size.S_constructor_kind);
  printf ("last_char_code = %d\n", lecl_tables.S_tbl_size.S_last_char_code);
  printf ("last_simple_class_no = %d\n", lecl_tables.S_tbl_size.S_last_simple_class_no);
  printf ("last_state_no = %d\n", lecl_tables.S_tbl_size.S_last_state_no);
  printf ("last_static_ste = %d\n", lecl_tables.S_tbl_size.S_last_static_ste);
  printf ("terlis_lgth = %d\n", lecl_tables.S_tbl_size.S_terlis_lgth);
  printf ("check_kw_lgth = %d\n", lecl_tables.S_tbl_size.S_check_kw_lgth);
  printf ("termax = %d\n", lecl_tables.S_tbl_size.S_termax);
  printf ("xprod = %d\n", lecl_tables.S_tbl_size.S_xprod);
  printf ("counters_size = %d\n", lecl_tables.S_tbl_size.S_counters_size);
  printf ("are_comments_erased = %d\n", lecl_tables.S_tbl_size.S_are_comments_erased);
  printf ("is_user_action_or_prdct = %d\n", lecl_tables.S_tbl_size.S_is_user_action_or_prdct);
  printf ("\nchar_to_simple_class\n");

  for (i = 0; i <= lecl_tables.S_tbl_size.S_last_char_code; i++)
    if (lecl_tables.S_char_to_simple_class [i] != 1)
      printf ("   %s\t%d\n", CHAR_TO_STRING (i), (int) lecl_tables.S_char_to_simple_class [i]);

  printf ("\ntransition_matrix\n");

  for (i = 1; i <= lecl_tables.S_tbl_size.S_last_state_no; i++) {
    printf ("STATE %d\n", i);
    printf ("\tclasse\t(keep\tscan\tcodop\tnext\n");
    tm = *(lecl_tables.S_transition_matrix + i);

    for (j = 1; j <= lecl_tables.S_tbl_size.S_last_simple_class_no; j++) {
      tm++;

      if (CODOP (*tm) != Error)
	printf ("\t%d\t(%c\t%c\t%d\t%d)\n", j, (!KEEP (*tm) && SCAN (*tm)) ? '-' : ' ', (SCAN (*tm)) ? '*' : ' ',
		CODOP (*tm), NEXT (*tm));
    }
  }

  printf ("\naction_or_prdct_code\n");
  printf ("x\t(keep\tscan\tcodop\tnext)\tno\tparam\tis_system\tkind\n");

  for (i = 1; i <= lecl_tables.S_tbl_size.S_xprod; i++) {
    ap = &(lecl_tables.SXS_action_or_prdct_code [i]);
    printf ("%d\t(%c\t%c\t%d\t%d)\t%d\t%d\t%d\t\t%d\n", i, (!KEEP (ap->stmt) && SCAN (ap->stmt)) ? '-' : ' ', (SCAN (
														     ap->stmt)) ? '*' : ' ', CODOP (ap->stmt), NEXT (ap->stmt), ap->action_or_prdct_no, ap->param, ap->is_system,
	    ap->kind);
  }

  printf ("\nadrp\n");
  printf ("x\txter\tsyno\tlgth\n");

  for (i = 0; i <= lecl_tables.S_tbl_size.S_last_static_ste; i++) {
    ad = &(lecl_tables.SXS_adrp [i]);
    printf ("%d\t%d\t%d\t%d\n", i, ad->xterlis, ad->syno_lnk, ad->lgth);
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
  SXEXIT (0);

  return EXIT_SUCCESS;
}
