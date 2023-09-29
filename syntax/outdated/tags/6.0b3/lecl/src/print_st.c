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
#define SX_DFN_EXT_VAR_LECL
#define SX_DFN_EXT_VAR

/* Le 24/01/2003
   Pour avoir les bonnes macros pour STMT, KEEP, SCAN, CODOP, NEXT ... */
#define SXS_MAX (SHRT_MAX+1)
#include "sxunix.h"
#include "sxsstmt.h"
#include "S_tables.h"
char WHAT_LECLPRINTST[] = "@(#)SYNTAX - $Id: print_st.c 1135 2008-03-05 15:32:38Z sagot $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE	*sxstdout, *sxstderr;
FILE	*sxtty;
BOOLEAN  sxverbosep;
#endif

extern BOOLEAN	lecl_read (struct lecl_tables_s *lecl_tables_ptr, char *langname);
extern VOID	lecl_free (struct lecl_tables_s *lecl_tables_ptr);
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

  sxstdout = stdout;
  sxstderr = stderr;

  if (argc != 2) {
    printf ("usage: print_st language_name\n");
    SXEXIT (2);
  }

  if (!lecl_read (&lecl_tables, argv [1]))
    SXEXIT (1);

  printf ("bnf_last_modif_time = %s\n", ctime (&lecl_tables.bnf_modif_time));
  printf ("constructor_kind = %ld\n", (long)lecl_tables.S_tbl_size.S_constructor_kind);
  printf ("last_char_code = %ld\n", (long)lecl_tables.S_tbl_size.S_last_char_code);
  printf ("last_simple_class_no = %ld\n", (long)lecl_tables.S_tbl_size.S_last_simple_class_no);
  printf ("last_state_no = %ld\n", (long)lecl_tables.S_tbl_size.S_last_state_no);
  printf ("last_static_ste = %ld\n", (long)lecl_tables.S_tbl_size.S_last_static_ste);
  printf ("terlis_lgth = %ld\n", (long)lecl_tables.S_tbl_size.S_terlis_lgth);
  printf ("check_kw_lgth = %ld\n", (long)lecl_tables.S_tbl_size.S_check_kw_lgth);
  printf ("termax = %ld\n", (long)lecl_tables.S_tbl_size.S_termax);
  printf ("xprod = %ld\n", (long)lecl_tables.S_tbl_size.S_xprod);
  printf ("counters_size = %ld\n", (long)lecl_tables.S_tbl_size.S_counters_size);
  printf ("are_comments_erased = %d\n", lecl_tables.S_tbl_size.S_are_comments_erased);
  printf ("is_user_action_or_prdct = %d\n", lecl_tables.S_tbl_size.S_is_user_action_or_prdct);
  printf ("\nchar_to_simple_class\n");

  for (i = 0; i <= lecl_tables.S_tbl_size.S_last_char_code; i++)
    if (lecl_tables.S_char_to_simple_class [i] != 1)
      printf ("   %s\t%ld\n", CHAR_TO_STRING (i), (long)lecl_tables.S_char_to_simple_class [i]);

  printf ("\ntransition_matrix\n");

  for (i = 1; i <= lecl_tables.S_tbl_size.S_last_state_no; i++) {
    printf ("STATE %ld\n", (long)i);
    printf ("\tclasse\t(keep\tscan\tcodop\tnext\n");
    tm = *(lecl_tables.S_transition_matrix + i);

    for (j = 1; j <= lecl_tables.S_tbl_size.S_last_simple_class_no; j++) {
      tm++;

      if (CODOP (*tm) != Error)
	printf ("\t%ld\t(%c\t%c\t%lu\t%lu)\n", (long)j, (!KEEP (*tm) && SCAN (*tm)) ? '-' : ' ', (SCAN (*tm)) ? '*' : ' ',
		(SXUINT)CODOP (*tm), (SXUINT)NEXT (*tm));
    }
  }

  printf ("\naction_or_prdct_code\n");
  printf ("x\t(keep\tscan\tcodop\tnext)\tno\tparam\tis_system\tkind\n");

  for (i = 1; i <= lecl_tables.S_tbl_size.S_xprod; i++) {
    ap = &(lecl_tables.SXS_action_or_prdct_code [i]);
    printf ("%ld\t(%c\t%c\t%lu\t%lu)\t%ld\t%ld\t%d\t\t%d\n", (long)i, (!KEEP (ap->stmt) && SCAN (ap->stmt)) ? '-' : ' ', (SCAN (
														     ap->stmt)) ? '*' : ' ', CODOP (ap->stmt), (SXUINT)NEXT (ap->stmt), (long)ap->action_or_prdct_no, (long)ap->param, ap->is_system,
	    (int)ap->kind);
  }

  printf ("\nadrp\n");
  printf ("x\txter\tsyno\tlgth\n");

  for (i = 0; i <= lecl_tables.S_tbl_size.S_last_static_ste; i++) {
    ad = &(lecl_tables.SXS_adrp [i]);
    printf ("%ld\t%ld\t%ld\t%ld\n", (long)i, (long)ad->xterlis, (long)ad->syno_lnk, (long)ad->lgth);
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
