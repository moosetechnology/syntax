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

#include "sxversion.h"
#include "sxunix.h"
#include <stdio.h>
#include "R_tables.h"

char WHAT_RECORPRINTRT[] = "@(#)SYNTAX - $Id: print_rt.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

extern bool	recor_read (struct R_tables_s *R_tables, char *langname);
extern void	recor_free (struct R_tables_s *R_tables);
static struct R_tables_s	R_tables;

static char	*copy (char *s1, char *s2, SXINT l)
{
    strncpy (s1, s2, l);
    *(s1 + l) = '\0';
    return s1;
    /* end copy */
}

/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  SXINT		i, j, x;
  char	mess [132];

  sxopentty ();

  if (argc != 2) {
    printf ("usage: print_rt language_name\n");
    sxexit (2);
  }

  if (!recor_read (&R_tables, argv [1]))
    sxexit (1);

  printf ("bnf_last_modif_time = %s\n", ctime (&R_tables.bnf_modif_time));
  printf ("S_nbcart = %ld\n", (SXINT) R_tables.R_tbl_size.S_nbcart);
  printf ("S_nmax = %ld\n", (SXINT) R_tables.R_tbl_size.S_nmax);

  printf ("S_maxlregle = %ld\n", (SXINT) R_tables.R_tbl_size.S_maxlregle);
  printf ("S_maxparam = %ld\n", (SXINT) R_tables.R_tbl_size.S_maxparam);
  printf ("S_lstring_mess = %ld\n", (SXINT) R_tables.R_tbl_size.S_lstring_mess);
  printf ("S_last_char_code = %ld\n", (SXINT) R_tables.R_tbl_size.S_last_char_code);
  printf ("P_nbcart = %ld\n", (SXINT) R_tables.R_tbl_size.P_nbcart);
  printf ("P_maxlregle = %ld\n", (SXINT) R_tables.R_tbl_size.P_maxlregle);
  printf ("P_maxparam = %ld\n", (SXINT) R_tables.R_tbl_size.P_maxparam);
  printf ("P_lstring_mess = %ld\n", (SXINT) R_tables.R_tbl_size.P_lstring_mess);
  printf ("P_tmax = %ld\n", (SXINT) R_tables.R_tbl_size.P_tmax);
  printf ("P_nmax = %ld\n", (SXINT) R_tables.R_tbl_size.P_nmax);
  printf ("P_validation_length = %ld\n", (SXINT) R_tables.R_tbl_size.P_validation_length);
  printf ("P_followers_number = %ld\n", (SXINT) R_tables.R_tbl_size.P_followers_number);
  printf ("P_sizeofpts = %ld\n", (SXINT) R_tables.R_tbl_size.P_sizeofpts);
  printf ("P_max_prio_X = %ld\n", (SXINT) R_tables.R_tbl_size.P_max_prio_X);
  printf ("P_min_prio_0 = %ld\n", (SXINT) R_tables.R_tbl_size.P_min_prio_0);
  printf ("E_nb_kind = %ld\n", (SXINT) R_tables.R_tbl_size.E_nb_kind);
  printf ("E_maxltitles = %ld\n", (SXINT) R_tables.R_tbl_size.E_maxltitles);
  printf ("E_labstract = %ld\n", (SXINT) R_tables.R_tbl_size.E_labstract);
  printf ("\nTitles\n");

  for (i = 1; i <= R_tables.R_tbl_size.E_nb_kind; i++) {
    printf ("\t%s\n", R_tables.E_titles [i]);
  }

  printf ("\nScanner\n");
  printf ("\tLocal");

  for (i = 1; i <= R_tables.R_tbl_size.S_nbcart; i++) {
    printf ("\n\t\t");

    for (j = 1; j <= R_tables.S_lregle [i] [0]; j++) {
      x = R_tables.S_lregle [i] [j];

      if (x == -1)
	printf ("X ");
      else if (x == -2)
	printf ("S ");
      else
	printf ("%ld ", (SXINT) x);
    }

    printf ("\t; ");

    for (j = 0; j <= R_tables.SXS_local_mess [i].param_no; j++) {
      if (j != 0) {
	x = R_tables.SXS_local_mess [i].string_ref [j].param;

	if (x >= 0) {
	  printf (" $%ld ", (SXINT) x);
	}
	else {
	  printf (" %%%ld ", (SXINT) -x - 1);
	}
      }

      copy (mess, R_tables.S_string_mess + R_tables.SXS_local_mess [i].string_ref [j].index, R_tables.SXS_local_mess [i
	    ].string_ref [j].length);
      printf ("%s", mess);
    }
  }

  printf ("\n\n\t\tNo_Delete =\n\t\t");
  sxba_print (R_tables.S_no_delete);
  printf ("\t\tNo_Insert =\n\t\t");
  sxba_print (R_tables.S_no_insert);
  printf ("\n\tGlobal\n");
  copy (mess, R_tables.S_string_mess + R_tables.S_global_mess [1].index, R_tables.S_global_mess [1].length);
  printf ("\t\tDetection\t: %s\n", mess);
  copy (mess, R_tables.S_string_mess + R_tables.S_global_mess [2].index, R_tables.S_global_mess [2].length);
  printf ("\t\tKeyword\t: %s\n", mess);
  copy (mess, R_tables.S_string_mess + R_tables.S_global_mess [3].index, R_tables.S_global_mess [3].length);
  printf ("\t\tEol\t: %s\n", mess);
  copy (mess, R_tables.S_string_mess + R_tables.S_global_mess [4].index, R_tables.S_global_mess [4].length);
  printf ("\t\tEof\t: %s\n", mess);
  copy (mess, R_tables.S_string_mess + R_tables.S_global_mess [5].index, R_tables.S_global_mess [5].length);
  printf ("\t\tHalt\t: %s\n", mess);
  printf ("\nParser\n");
  printf ("\tLocal");

  for (i = 1; i <= R_tables.R_tbl_size.P_nbcart; i++) {
    printf ("\nRight_Ctxt_Head = %ld\t; ", (SXINT) R_tables.P_right_ctxt_head [i]);

    for (j = 1; j <= R_tables.P_lregle [i] [0]; j++) {
      x = R_tables.P_lregle [i] [j];

      if (x == -1)
	printf ("X ");
      else if (x == -2)
	printf ("S ");
      else
	printf ("%ld ", (SXINT) x);
    }

    printf ("\t; ");

    for (j = 0; j <= R_tables.SXP_local_mess [i].param_no; j++) {
      if (j != 0) {
	x = R_tables.SXP_local_mess [i].string_ref [j].param;

	if (x >= 0) {
	  printf (" $%ld ", (SXINT) x);
	}
	else {
	  printf (" %%%ld ", (SXINT) -x - 1);
	}
      }

      copy (mess, R_tables.P_string_mess + R_tables.SXP_local_mess [i].string_ref [j].index, R_tables.SXP_local_mess [i
	    ].string_ref [j].length);
      printf ("%s", mess);
    }
  }

  printf ("\n\n\t\tNo_Delete =\n\t\t");
  sxba_print (R_tables.P_no_delete);
  printf ("\t\tNo_Insert =\n\t\t");
  sxba_print (R_tables.P_no_insert);
  printf ("\n\tForced_Insertion\n\t\t");

  for (j = 0; j <= R_tables.SXP_local_mess [R_tables.R_tbl_size.P_nbcart + 1].param_no; j++) {
    if (j != 0) {
      x = R_tables.SXP_local_mess [R_tables.R_tbl_size.P_nbcart + 1].string_ref [j].param;

      if (x >= 0) {
	printf (" $%ld ", (SXINT) x);
      }
      else {
	printf (" %%%ld ", (SXINT) -x - 1);
      }
    }

    copy (mess, R_tables.P_string_mess + R_tables.SXP_local_mess [R_tables.R_tbl_size.P_nbcart + 1].string_ref [j].
	  index, R_tables.SXP_local_mess [R_tables.R_tbl_size.P_nbcart + 1].string_ref [j].length);
    printf ("%s", mess);
  }

  printf ("\n\n\tGlobal\n");
  printf ("\t\tKey_Terminals =\n\t\t");
  sxba_print (R_tables.PER_tset);
  copy (mess, R_tables.P_string_mess + R_tables.P_global_mess [1].index, R_tables.P_global_mess [1].length);
  printf ("\t\tFollowers\t: %s\n", mess);
  copy (mess, R_tables.P_string_mess + R_tables.P_global_mess [2].index, R_tables.P_global_mess [2].length);
  printf ("\t\tDetection\t:%s\n", mess);
  copy (mess, R_tables.P_string_mess + R_tables.P_global_mess [3].index, R_tables.P_global_mess [3].length);
  printf ("\t\tRestarting\t:%s\n", mess);
  copy (mess, R_tables.P_string_mess + R_tables.P_global_mess [4].index, R_tables.P_global_mess [4].length);
  printf ("\t\tHalt\t:%s\n", mess);
  printf ("\nAbstract\n");
  printf ("\t%s\n", R_tables.E_abstract);
  recor_free (&R_tables);
  sxexit (0);

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
