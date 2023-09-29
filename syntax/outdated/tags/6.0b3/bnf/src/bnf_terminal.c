/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 14-10-99 16:40 (phd) :	Utilisation de O_BINARY (pour Windows)	*/
/************************************************************************/
/* 14-10-99 16:39 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"

char WHAT_BNFTERMINAL[] = "@(#)SYNTAX - $Id: bnf_terminal.c 815 2007-10-15 11:38:13Z rlacroix $" WHAT_DEBUG;

static void
output_header (SXINT bnf_modif_time, char *langname)
{
  SXINT cur_time;

  cur_time = time (0);  /* now */

  printf ("\
/* ***************************************************************************************\n\
\tThis terminal symbol list has been extracted on %s\
\tby the SYNTAX(*) BNF processor running with the \"-terminal\" option\n\
\tfrom the CFG \"%s\" in BNF form last modified on %s\
******************************************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
****************************************************************************************** */\n\n\n",
	  ctime (&cur_time),
	  (langname == NULL) ? "stdin" : langname,
	  ctime (&bnf_modif_time));
}



static void
escape_printf (char *str)
{
  while (*str != NUL) {
    if (*str == '\\' || *str == '"')
      printf ("\\%c",*str);
    else
      printf ("%c",*str);

    str++;
  }
}


#define get_t_string(t)		(bnf_ag.T_STRING + bnf_ag.ADR [t])

VOID
bnf_terminal (struct bnf_ag_item *B, char *langname)
{
  SXINT    t;

  if (sxverbosep)
    fputs ("   Terminal Symbol List Output\n", sxtty);

  output_header (B->bnf_modif_time, langname);

  for (t = 1; t < -W.tmax; t++) {
    printf ("/* %ld */ \"", t);
    escape_printf (get_t_string (-t));
    fputs ("\"\n", stdout);
  }
}
