/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.	  *
   *							  *
   ******************************************************** */



/************************************************************************/
/*          Constructeur pour grammaires attribuees a la YACC 	        */
/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030527 15:57 (phd):	#define SX_DFN_EXT_VAR			*/
/************************************************************************/
/* 20020318 14:30 (phd):	"sxtmpfile" dans "bnf_run"		*/
/************************************************************************/
/* 21-01-94 14:30 (pb):		Caractere change de "char" en "SXINT"	*/
/************************************************************************/
/* 06-06-88 14:20 (pb):	BITSTR => SXBA					*/
/************************************************************************/
/* 22-04-88 09:43 (phd):	Utilisation de sxindex et sxrindex	*/
/************************************************************************/
/* 20-04-88 16:46 (phd):	Modifications des options		*/
/************************************************************************/
/* 11-03-88 14:40 (pb):		#include <sys/timeb.h>			*/
/************************************************************************/
/* 18-12-87 15:00 (pb):		Warning si "nom-du-langage" trop long	*/
/************************************************************************/
/* 24-07-87 14:14 (phd):	Utilisation de "SXEXIT" partout		*/
/************************************************************************/
/* 20-11-87 15:15 (phd):	Modification du traitement de l'option	*/
/*				LANGUAGE_NAME pour permettre l'analyse	*/
/*				des eventuelles options restantes	*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Utilisation de "sxopentty"		*/
/************************************************************************/
/* 15-04-87 16:32 (phd):	Ajout de bnf_lo() avant yax_lo()	*/
/************************************************************************/

#define SX_DFN_EXT_VAR_BNF /* pour definir les variables globales de BNF (cf bnf_vars.h) */
#define SX_DFN_EXT_VAR_YAX /* pour definir les variables globales de YAX (cf yax_vars.h) */

#include "sxunix.h"
#include "B_tables.h"
#include "yax_vars.h"

/* These include files for date/time manipulation: */
#include <sys/types.h>
#include <sys/stat.h>
char WHAT_YAXMAIN[] = "@(#)SYNTAX - $Id: yax_main.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
FILe *sxstdout, *sxstderr;
#endif

char	by_mess [] = "the SYNTAX attribute processor YAX";

#include "release.h"

SXINT (*more_scan_act) (SXINT code, SXINT act_no) ;

extern VOID	no_tables (void);
extern VOID	bnf_lo (void);
extern BOOLEAN	yax_sem (void);
extern VOID	yax_lo (void);
extern VOID	yax_free (void);
extern struct sxtables	bnf_tables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "yax";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-sc, -source, -nsc, -nosource,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ls, -list, -nls, -nolist,\n\
\t\t-st, -stats, -nst, -nostats\n\
\t\t-rhs nnn, -max_right_hand_side nnn,\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB	((sizeof (option_tbl) / sizeof (*option_tbl)) - 1)

#define UNKNOWN_ARG 		((SXINT)0)
#define SOURCE 			((SXINT)1)
#define VERBOSE 		((SXINT)2)
#define LIST 			((SXINT)3)
#define LANGUAGE_NAME		((SXINT)4)
#define SEM_STATS	        ((SXINT)5)
#define MAX_RIGHT_HAND_SIZE 	((SXINT)6)
#define LAST_OPTION	        MAX_RIGHT_HAND_SIZE

static char	*option_tbl [] = {"", "sc", "source", "nsc", "nosource", "v", "verbose", "nv", "noverbose", "ls", "list",
     "nls", "nolist", "ln", "language_name", "st", "stats", "rhs", "max_right_hand_side",};
static SXINT	option_kind [] = {UNKNOWN_ARG, SOURCE, SOURCE, -SOURCE, -SOURCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LIST, LIST, -LIST, -LIST, LANGUAGE_NAME, LANGUAGE_NAME, SEM_STATS, SEM_STATS, MAX_RIGHT_HAND_SIZE,
     MAX_RIGHT_HAND_SIZE,};




/*-------------------------------------------------------------------*/

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




/*-------------------------------------------------------------------*/

static char	*option_get_text (SXINT kind)
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}




/*-------------------------------------------------------------------*/

static SXVOID	language_name (char *path_name)
{
    char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    prgentname = (char*) sxalloc (strlen (p)+1, sizeof (char));

    if ((p = sxindex (strcpy (prgentname, p), '.')) != NULL) {
	*p = NUL;
    }
}




/*-------------------------------------------------------------------*/

static VOID	bnf_run (char *pathname)
{
    FILE	*infile;

    if (pathname == NULL) {
	int	c;

	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	if ((infile = sxtmpfile ()) == NULL) {
	    fprintf (sxstderr, "%s: Unable to create ", ME);
	    sxperror ("temp file");
	    sxerrmngr.nbmess [2]++;
	    return;
	}

	while ((c = getchar ()) != EOF) {
	    putc (c, infile);
	}

	rewind (infile);
	sxsrc_mngr (INIT, infile, "");
	bnf_ag.bnf_modif_time = time (0) /* stdin => now */ ;
    }
    else {
	if ((infile = sxfopen (pathname, "r")) == NULL) {
	    fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	    sxperror (pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}
	else {
	    struct stat		buf /* date/time info */ ;

	    if (sxverbosep) {
		fprintf (sxtty, "%s:\n", pathname);
	    }

	    language_name (pathname);
	    sxsrc_mngr (INIT, infile, pathname);
	    stat (sxsrcmngr.source_coord.file_name, &buf);
	    bnf_ag.bnf_modif_time = (long) buf.st_mtime;
	}
    }

    sxerr_mngr (BEGIN);
    syntax (ACTION, &bnf_tables);
    sxsrc_mngr (FINAL);

    if (!is_proper || IS_ERROR) {
	no_tables ();
	bnf_lo ();
    }
    else {
	if (yax_sem ()) {
	    bnf_write (&bnf_ag, prgentname);
	}
	else {
	    sxerrmngr.nbmess [2]++;
	}

	bnf_lo ();
	yax_lo ();
	yax_free ();
    }

    sxerr_mngr (END);
    fclose (infile);
    bnf_free (&bnf_ag);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}




/*-------------------------------------------------------------------*/

/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int	argnum;

  sxstdout = stdout;
  sxstderr = stderr;

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    SXEXIT (3);
  }

  sxopentty ();

  /* valeurs par defaut */

  options_set = OPTION (SOURCE) | OPTION (VERBOSE) | OPTION (LIST) | OPTION (SEM_STATS);
  is_source = sxverbosep = is_list = is_sem_stats = TRUE;
  tbl_lgth = 10;
  max_RHS = -1; /* Le 16/4/2002, par defaut, pas de verif de la longueur des RHS */


  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case SOURCE:
      is_source = TRUE, options_set |= OPTION (SOURCE);
      break;

    case -SOURCE:
      is_source = FALSE, options_set &= noOPTION (SOURCE);
      break;

    case VERBOSE:
      sxverbosep = TRUE, options_set |= OPTION (VERBOSE);
      break;

    case -VERBOSE:
      sxverbosep = FALSE, options_set &= noOPTION (VERBOSE);
      break;

    case LIST:
      is_list = TRUE, options_set |= OPTION (LIST);
      break;

    case -LIST:
      is_list = FALSE, options_set &= noOPTION (LIST);
      break;

    case SEM_STATS:
      is_sem_stats = TRUE, options_set |= OPTION (SEM_STATS);
      break;

    case -SEM_STATS:
      is_sem_stats = FALSE, options_set &= noOPTION (SEM_STATS);
      break;

    case MAX_RIGHT_HAND_SIZE:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &max_RHS))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-rhs\", value %ld is retained.\n", ME, (long)max_RHS);
      else {
	options_set |= OPTION (MAX_RIGHT_HAND_SIZE);
      }

      break;

    case LANGUAGE_NAME:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a name must follow the \"%s\" option;\n\t'' is assumed.\n", ME, option_get_text (
														 LANGUAGE_NAME));
	prgentname = (char*) sxalloc (1, sizeof (char));
	prgentname [0] = NUL;
      }
      else {
	prgentname = (char*) sxalloc (strlen (argv [argnum])+1, sizeof (char));
	strcpy (prgentname, argv [argnum]);
      }

      options_set |= OPTION (LANGUAGE_NAME);
      break;

    case UNKNOWN_ARG:
      if (options_set & OPTION (LANGUAGE_NAME)) {
	fprintf (sxstderr, "%s: the \"%s\" option must not be used when naming files.\n", ME, option_get_text (
													       LANGUAGE_NAME));
	options_set &= noOPTION (LANGUAGE_NAME);
      }

      goto run;

    default:
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (long)option_get_kind (argv [argnum]));
      SXEXIT (3);
    }
  }

  if (!(options_set & OPTION (LANGUAGE_NAME))) {
    fprintf (sxstderr, "%s: the \"%s\" option is mandatory when no file is named;\n\t'' is assumed\n", ME,
	     option_get_text (LANGUAGE_NAME));
    prgentname = (char*) sxalloc (1, sizeof (char));
    prgentname [0] = NUL;
  }

 run:
  if (sxverbosep) {
    fprintf (sxtty, "%s\n", release_mess);
  }

  sxstr_mngr (BEGIN);
  (*(bnf_tables.analyzers.parser)) (BEGIN);
  syntax (OPEN, &bnf_tables);

  if (options_set & OPTION (LANGUAGE_NAME)) {
    bnf_run ((char*)NULL);
  }
  else {
    do {
      int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      sxstr_mngr (BEGIN) /* remise a vide de la string table */ ;
      bnf_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (CLOSE, &bnf_tables);
  (*(bnf_tables.analyzers.parser)) (END);
  sxstr_mngr (END);

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
      ;

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}




/*-------------------------------------------------------------------*/

char	*options_text (char *line)
{
    SXINT	i;
    BOOLEAN	is_first = TRUE;

    *line = NUL;

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = FALSE;
	    else
		strcat (line, ", ");

	    strcat (line, option_get_text (i));

	    if (i == MAX_RIGHT_HAND_SIZE)
		sprintf (line + strlen (line), "=%ld", (long)max_RHS);
	}

    return line;
}




/*-------------------------------------------------------------------*/

VOID	sxvoid (void)
/* procedure ne faisant rien */
{
}
