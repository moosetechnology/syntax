/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */

#include "sxunix.h"
#include <stdio.h>
#include "R_tables.h"
char WHAT_RECORPRINTRT[] = "@(#)SYNTAX - $Id: print_rt.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

extern BOOLEAN	recor_read ();
extern VOID	recor_free ();
static struct R_tables_s	R_tables;



static char	*copy (s1, s2, l)
    char	*s1, *s2;
    int		l;
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
  int		i, j, x;
  char	mess [132];

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }
  if (sxstderr == NULL) {
    sxstderr = stderr;
  }

  if (argc != 2) {
    printf ("usage: print_rt language_name\n");
    exit (2);
  }

  if (!recor_read (&R_tables, argv [1]))
    exit (1);

  printf ("bnf_last_modif_time = %s\n", ctime (&R_tables.bnf_modif_time));
  printf ("S_nbcart = %d\n", R_tables.R_tbl_size.S_nbcart);
  printf ("S_nmax = %d\n", R_tables.R_tbl_size.S_nmax);
  printf ("S_maxlregle = %d\n", R_tables.R_tbl_size.S_maxlregle);
  printf ("S_maxparam = %d\n", R_tables.R_tbl_size.S_maxparam);
  printf ("S_lstring_mess = %d\n", R_tables.R_tbl_size.S_lstring_mess);
  printf ("S_last_char_code = %d\n", R_tables.R_tbl_size.S_last_char_code);
  printf ("P_nbcart = %d\n", R_tables.R_tbl_size.P_nbcart);
  printf ("P_maxlregle = %d\n", R_tables.R_tbl_size.P_maxlregle);
  printf ("P_maxparam = %d\n", R_tables.R_tbl_size.P_maxparam);
  printf ("P_lstring_mess = %d\n", R_tables.R_tbl_size.P_lstring_mess);
  printf ("P_tmax = %d\n", R_tables.R_tbl_size.P_tmax);
  printf ("P_nmax = %d\n", R_tables.R_tbl_size.P_nmax);
  printf ("P_validation_length = %d\n", R_tables.R_tbl_size.P_validation_length);
  printf ("P_followers_number = %d\n", R_tables.R_tbl_size.P_followers_number);
  printf ("P_sizeofpts = %d\n", R_tables.R_tbl_size.P_sizeofpts);
  printf ("P_max_prio_X = %d\n", R_tables.R_tbl_size.P_max_prio_X);
  printf ("P_min_prio_0 = %d\n", R_tables.R_tbl_size.P_min_prio_0);
  printf ("E_nb_kind = %d\n", R_tables.R_tbl_size.E_nb_kind);
  printf ("E_maxltitles = %d\n", R_tables.R_tbl_size.E_maxltitles);
  printf ("E_labstract = %d\n", R_tables.R_tbl_size.E_labstract);
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
	printf ("%d ", x);
    }

    printf ("\t; ");

    for (j = 0; j <= R_tables.SXS_local_mess [i].param_no; j++) {
      if (j != 0) {
	x = R_tables.SXS_local_mess [i].string_ref [j].param;

	if (x >= 0) {
	  printf (" $%d ", x);
	}
	else {
	  printf (" %%%d ", -x - 1);
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
    printf ("\nRight_Ctxt_Head = %d\t; ", R_tables.P_right_ctxt_head [i]);

    for (j = 1; j <= R_tables.P_lregle [i] [0]; j++) {
      x = R_tables.P_lregle [i] [j];

      if (x == -1)
	printf ("X ");
      else if (x == -2)
	printf ("S ");
      else
	printf ("%d ", x);
    }

    printf ("\t; ");

    for (j = 0; j <= R_tables.SXP_local_mess [i].param_no; j++) {
      if (j != 0) {
	x = R_tables.SXP_local_mess [i].string_ref [j].param;

	if (x >= 0) {
	  printf (" $%d ", x);
	}
	else {
	  printf (" %%%d ", -x - 1);
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
	printf (" $%d ", x);
      }
      else {
	printf (" %%%d ", -x - 1);
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
  exit (0);

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
