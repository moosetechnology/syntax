/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1996 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe ATOLL.			  *
   *							  *
   ******************************************************** */





/* Constructeur d'analyseurs lexicaux de SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030319 10:48 (phd):	Utilisation de "sxtimestamp"		*/
/************************************************************************/
/* Aug 20 1996 14:17 (pb):	Ajout de cette rubrique "modifications" */
/************************************************************************/


#include "sxunix.h"
#include "semact.h"
char WHAT_SXEARLEYMAIN[] = "@(#)SYNTAX - $Id: sxearley_main.c 1075 2008-02-19 14:56:56Z rlacroix $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE	*sxstdout, *sxstderr;
FILE	*sxtty;
#endif

extern struct sxtables	earley_tables;

/*---------------*/
/*    options    */
/*---------------*/

BOOLEAN		sxverbosep = TRUE;
static char	ME [] = "earley_parser";
static char	Usage [] = "\
Usage:\t%s [options] files\n\
options=\t-ns, -no_semantics,\n\
\t\t-ds, -default_semantics,\n\
\t\t-ptn, -parse_tree_number,\n\
\t\t-t, -time, -nt, -no_time\n\
\t\t-, -stdin,\n\
\t\t-pp, -print_prod.\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define NO_SEM		  1
#define DEFAULT_SEM	  2
#define PARSE_TREE_NUMBER 3
#define TIME		  4
#define STDIN	  	  5
#define PRINT_PROD	  6
#define SOURCE_FILE	  7


static char	*option_tbl [] = {
    "",
    "ns", "no_semantics",
    "ds", "default_semantics",
    "ptn", "parse_tree_number",
    "t", "time", "nt", "no_time",
    "stdin",
    "pp", "print_prod",};

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    NO_SEM, NO_SEM,
    DEFAULT_SEM, DEFAULT_SEM,
    PARSE_TREE_NUMBER, PARSE_TREE_NUMBER,
    TIME, TIME, -TIME, -TIME,
    STDIN,
    PRINT_PROD, PRINT_PROD,};



static SXINT	option_get_kind (arg)
    char	*arg;
{
    char	**opt;

    if (*arg++ != '-')
	return SOURCE_FILE;

    if (*arg == NUL)
	return STDIN;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}



static char	*option_get_text (kind)
    SXINT	kind;
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}



static	earley_run (pathname)
    char	*pathname;
{
    extern char 	*mktemp ();
    FILE	*infile;

    if (pathname == NULL) {
	char	pathname [32];
	SXINT	c;

	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	if ((infile = fopen (mktemp (strcpy (pathname, "/tmp/earleyXXXXXX")), "w")) == NULL) {
	    fprintf (sxstderr, "%s: Unable to create temp file:\t", ME);
lost:	    sxperror (pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}

	while ((c = getchar ()) != EOF) {
	    putc (c, infile);
	}

	if ((infile = freopen (pathname, "r", infile)) == NULL) {
	    fprintf (sxstderr, "%s: Unable to reopen temp file:\t", ME);
	    goto lost;
	}

	unlink (pathname) /* vanishes at once, dies when closed */ ;
	rewind (infile);
	sxsrc_mngr (INIT, infile, "");
    }
    else {
	if ((infile = fopen (pathname, "r")) == NULL) {
	    fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	    sxperror (pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}
	else {
	    if (sxverbosep) {
		fprintf (sxtty, "%s:\n", pathname);
	    }

	    sxsrc_mngr (INIT, infile, pathname);
	}
    }

    sxerr_mngr (BEGIN);
    syntax (ACTION, &earley_tables);
    sxerr_mngr (END);
    fclose (infile);
    sxsrc_mngr (FINAL);
}




main (argc, argv)
    SXINT		argc;
    char	*argv [];
{
    SXINT		argnum;
    BOOLEAN	in_options, is_source_file, is_stdin;

    sxstdout = stdout;
    sxstderr = stderr;

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (3);
    }

    sxopentty ();

/* valeurs par defaut */

    is_print_time = TRUE;
    is_print_prod = FALSE;
    is_no_semantics = FALSE;
    is_parse_tree_number = FALSE;
    is_default_semantics = TRUE; /* Semantique specifiee avec la grammaire */

/* Decodage des options */

    in_options = TRUE;
    is_source_file = FALSE;
    is_stdin = FALSE;
    argnum = 0;

    while (in_options && ++argnum < argc) {
	switch (option_get_kind (argv [argnum])) {
	case NO_SEM:
	    is_no_semantics = TRUE;
	    is_default_semantics = FALSE;
	    is_parse_tree_number = FALSE;
	    break;

	case DEFAULT_SEM:
	    is_no_semantics = FALSE;
	    is_default_semantics = TRUE;
	    is_parse_tree_number = FALSE;
	    break;

	case PARSE_TREE_NUMBER:
	    is_no_semantics = FALSE;
	    is_default_semantics = FALSE;
	    is_parse_tree_number = TRUE;
	    break;

	case TIME:
	    is_print_time = TRUE;
	    break;

	case -TIME:
	    is_print_time = FALSE;
	    break;

	case PRINT_PROD:
	    is_print_prod = TRUE;
	    break;

	case STDIN:
	    is_stdin = TRUE;
	    break;

	case SOURCE_FILE:
	    if (is_stdin) {
		is_stdin = FALSE;
	    }

	    is_source_file = TRUE;
	    in_options = FALSE;
	    break;

	case UNKNOWN_ARG:
	    fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
	    SXEXIT (3);
	}
    }

    if (!is_stdin && !is_source_file) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (3);
    }


    sxstr_mngr (BEGIN);

    (*(earley_tables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
    syntax (OPEN, &earley_tables) /* Initialisation de SYNTAX */ ;

    if (is_stdin) {
	earley_run (NULL);
    }
    else {
	do {
	    SXINT	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    sxstr_mngr (BEGIN) /* remise a vide de la string table */ ;
	    earley_run (argv [argnum++]);
	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    syntax (CLOSE, &earley_tables);
    (*(earley_tables.analyzers.parser)) (END);

    sxstr_mngr (END);

    {
	SXINT	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	SXEXIT (severity);
    }
}


VOID
sxtime (what, str)
    SXINT what;
    char *str;
{
  if (what == TIME_INIT) {
    sxtimestamp (INIT, NULL);
  }
  else {
    fputs (str, sxtty);

    if (is_print_time)
      sxtimestamp (ACTION, NULL);
    else
      putc ('\n', sxtty);
  }
}


VOID
sxvoid ()
/* procedure ne faisant rien */
{
}

BOOLEAN
sxbvoid ()
/* procedure ne faisant rien */
{
}
