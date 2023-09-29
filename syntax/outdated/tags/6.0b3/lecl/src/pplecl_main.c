/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.	  *
   *							  *
   ******************************************************** */





/* paragrapheur de LECL */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040112 14:10 (phd):	Adaptation à G5 (faux 64 bits)		*/
/************************************************************************/
/* 20020328 13:30 (phd):	Utilisation de sxfile_copy		*/
/************************************************************************/
/* 20020318 17:30 (phd):	Utilisation de sxnewfile		*/
/************************************************************************/
/* 06-10-99 18:45 (phd):	Déplacement d'includes de file_copy	*/
/************************************************************************/
/* Lun 25 Nov 1996 11:13 (pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 22-10-93 10:00 (pb):		Adaptation a OSF			*/
/************************************************************************/
/* 21-04-88 16:12 (phd):	Test du nombre d'octets dans file_copy	*/
/************************************************************************/
/* 28-03-88 16:40 (phd):	Utilisation de sxindex et sxrindex	*/
/************************************************************************/
/* 24-07-87 14:14 (phd):	Utilisation de "SXEXIT" partout		*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Utilisation de "sxopentty"		*/
/************************************************************************/
/* 06-03-87 16:48 (pb):	Suppression des options: nkwuc, nkwlc, nkwci,	*/
/*			mm, sm et nsm					*/
/************************************************************************/
/* 13-02-87 14:43 (phd):	Mode verbose plus agreable		*/
/************************************************************************/
/* 16-01-87 12:11 (pb):	Utilisation de pplecl_td.h	 	   	*/
/************************************************************************/
/* 07-01-87 11:59 (pb):	Bugs dans les options		 	   	*/
/************************************************************************/
/* 06-11-86 11:51 (pb):	Ajout de cette rubrique "modifications"	   	*/
/************************************************************************/

#define SX_DFN_EXT_VAR_LECL

#define SX_DFN_EXT_VAR	/* Nécessaire sur G5... */
#include "sxunix.h"
char WHAT_PPLECLMAIN[] = "@(#)SYNTAX - $Id: pplecl_main.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
FILE *sxstdout, *sxstderr, *sxtty;
BOOLEAN sxverbosep;
#endif

extern struct sxtables	pp_lecl_tables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "pplecl";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define KWUC 			1
#define KWLC 			2
#define KWCI 			3
#define KWDK 			4
#define VERBOSE 		5
#define LINE_LENGTH	 	6
#define TABS			7
#define EVERY			8
#define LAST_OPTION		EVERY

static char	*option_tbl [] = {"", "kwuc", "keywords_in_upper_case", "kwlc", "keywords_in_lower_case", "kwci",
     "keywords_capitalised_initial", "kwdk", "keywords_in_dark", "nkwdk", "nokeywords_in_dark", "v", "verbose", "nv",
     "noverbose", "ll", "line_length", "tabs", "notabs", "every"};
static SXINT	option_kind [] = {UNKNOWN_ARG, KWUC, KWUC, KWLC, KWLC, KWCI, KWCI, KWDK, KWDK, -KWDK, -KWDK, VERBOSE,
     VERBOSE, -VERBOSE, -VERBOSE, LINE_LENGTH, LINE_LENGTH, TABS, -TABS, EVERY};



static SXINT	option_get_kind (char *arg)
{
    char	**opt;

    if (*arg++ != '-')
	return UNKNOWN_ARG;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}



static VOID	pplecl_run (char *pathname)
{
    FILE	*infile;
    char	un_nom [20];

    if (pathname == NULL) {
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	sxsrc_mngr (INIT, infile = stdin, "");
	sxstdout = stdout;
    }
    else {
	if ((infile = sxfopen (pathname, "r")) == NULL) {
	    fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	    sxperror (pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}
	else {
	    strcpy (un_nom, "ppleclXXXXXX");

	    if ((sxstdout = sxnewfile (un_nom)) == NULL) {
		fprintf (sxstderr, "%s: Cannot open (write) ", ME);
		sxperror (un_nom);
		sxerrmngr.nbmess [2]++;
		return;
	    }

	    if (sxverbosep) {
		fprintf (sxtty, "%s:\n", pathname);
	    }

	    sxsrc_mngr (INIT, infile, pathname);
	}
    }

    syntax (ACTION, &pp_lecl_tables);

    if (infile != stdin) {
	fclose (sxstdout);
	fclose (infile);

	if (sxppvariables.is_error || !sxfile_copy (un_nom, pathname, sxppvariables.char_count, ME)) {
	    fprintf (sxstderr, "%s: Le resultat du paragraphage se trouve dans %s.\n", ME, un_nom);
	    (sxerrmngr.nbmess [2])++;
	}
	else {
	    unlink (un_nom);
	}
    }

    sxsrc_mngr (FINAL);
}



/************************************************************************/
/* main function */
/************************************************************************/
int main(int argc, char *argv[])
{
  int	argnum;
  SXINT	val;

  sxstdout = stdout;
  sxstderr = stderr;

  sxopentty ();

  /* valeurs par defaut */

  sxverbosep = TRUE;
  sxppvariables.kw_case = CAPITALISED_INITIAL /* How should keywords be written */ ;
  sxppvariables.terminal_case = NULL /* Same as kw_case, but for each type of terminal */ ;
  sxppvariables.kw_dark = FALSE /* keywords are not artificially darkened */ ;
  sxppvariables.terminal_dark = NULL /* Same as kw_dark, but for each type of terminal */ ;
  sxppvariables.no_tabs = FALSE /* optimize spaces into tabs */ ;
  sxppvariables.block_margin = FALSE /* preserve structure when deeply nested */ ;
  sxppvariables.line_length = 79 /* What it says */ ;
  sxppvariables.max_margin = 60 /* Do not indent lines further than that */ ;
  sxppvariables.tabs_interval = 8 /* number of columns between two tab positions */ ;


  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case KWUC:
      sxppvariables.kw_case = UPPER_CASE;
      break;

    case KWLC:
      sxppvariables.kw_case = LOWER_CASE;
      break;

    case KWCI:
      sxppvariables.kw_case = CAPITALISED_INITIAL;
      break;

    case KWDK:
      sxppvariables.kw_dark = TRUE;
      break;

    case -KWDK:
      sxppvariables.kw_dark = FALSE;
      break;

    case VERBOSE:
      sxverbosep = TRUE;
      break;

    case -VERBOSE:
      sxverbosep = FALSE;
      break;

    case LINE_LENGTH:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &val))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-ll\", %ld value is retained.\n", ME,
		 sxppvariables.line_length);
      else {
	sxppvariables.line_length = val;
      }

      break;

    case TABS:
      sxppvariables.no_tabs = FALSE;
      break;

    case -TABS:
      sxppvariables.no_tabs = TRUE;
      break;

    case EVERY:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &val))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-every\", %ld value is retained.\n", ME,
		 sxppvariables.tabs_interval);
      else {
	sxppvariables.tabs_interval = val;
      }

      break;

    case UNKNOWN_ARG:
      goto run;

    default:
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (long)option_get_kind (argv [argnum]));
      SXEXIT (3);
    }
  }

 run:
  if (sxverbosep) {
    setbuf (stdout, NULL);
  }

  sxstr_mngr (BEGIN);
  (*(pp_lecl_tables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &pp_lecl_tables) /* Initialisation de SYNTAX */ ;

  if (argnum == argc) {
    pplecl_run ((char*)NULL);
  }
  else {
    do {
      int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      pplecl_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (CLOSE, &pp_lecl_tables);
  (*(pp_lecl_tables.analyzers.parser)) (END);
  sxstr_mngr (END);

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
      ;

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



VOID	sxvoid (void)
/* procedure ne faisant rien */
{
}
