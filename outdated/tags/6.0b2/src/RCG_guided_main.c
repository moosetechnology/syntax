/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1997 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe ATOLL.			  *
   *							  *
   ******************************************************** */





/* Programme principal pour l'analyse de textes decrits a l'aide
   d'une grammaire RCG.  Le source est un "guide" produit par
   une execution d'une 1-RCG */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030319 10:48 (phd):	Utilisation de "sxtimestamp"		*/
/************************************************************************/
/* Mer 15 Dec 1999 13:24 (pb):	Suppression de option "-generator_lgth"	*/
/************************************************************************/
/* 6 Nov 1997 10:23 (pb):	Ajout de cette rubrique "modifications" */
/************************************************************************/


#include "sxunix.h"

char WHAT_RCG_GUIDED[] = "@(#)SYNTAX - $Id: RCG_guided_main.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;


#include "rcg_sglbl.h"
#include "release.h"

/* VOID sxtime (); */

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout, *sxstderr;
FILE	*sxtty;
extern struct sxtables	guide_tables;
extern struct sxtables	RCG_tables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "RCG_parser";
static char	Usage [] = "\
Usage:\t%s [options] files\n\
options=\n\
\t-v, -verbose, -nv, -noverbose,\n\
\t-ns, -no_semantics,\n\
\t-ds, -default_semantics,\n\
\t-ptn, -parse_tree_number,\n\
\t-t, -time, -nt, -no_time\n\
\t-, -stdin,\n\
\t-dl nnn, -debug_level nnn,\n\
\t-fl nnn, -forest_level nnn,(source=1, clause=2, lhs_prdct=4, lhs_clause=8, rhs=(16+4), n=32, supertagger=64)\n\
\t-g, -guiding,\n\
\t-fg, -full_guiding,\n\
\t-btn nnn, -best_tree_number nnn,\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define VERBOSE 	  1
#define NO_SEM		  2
#define DEFAULT_SEM	  3
#define PARSE_TREE_NUMBER 4
#define TIME		  5
#define STDIN	  	  6
#define DEBUG_LEVEL	  7
#define FOREST_LEVEL	  8
#define GUIDING		  9
#define FULL_GUIDING	  10
#define BEST_TREE_NUMBER  11
#define SOURCE_FILE	  12


static char	*option_tbl [] = {
    "",
    "v", "verbose", "nv", "noverbose",
    "ns", "no_semantics",
    "ds", "default_semantics",
    "ptn", "parse_tree_number",
    "t", "time", "nt", "no_time",
    "stdin",
    "dl", "debug_level",
    "fl", "forest_level",
    "g", "guiding",
    "fg", "full_guiding",
    "btn", "best_tree_number",
};

static int	option_kind [] = {
    UNKNOWN_ARG,
    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
    NO_SEM, NO_SEM,
    DEFAULT_SEM, DEFAULT_SEM,
    PARSE_TREE_NUMBER, PARSE_TREE_NUMBER,
    TIME, TIME, -TIME, -TIME,
    STDIN,
    DEBUG_LEVEL, DEBUG_LEVEL,
    FOREST_LEVEL, FOREST_LEVEL,
    GUIDING, GUIDING,
    FULL_GUIDING, FULL_GUIDING,
    BEST_TREE_NUMBER, BEST_TREE_NUMBER,
};



static int	option_get_kind (arg)
    register char	*arg;
{
    register char	**opt;

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
    register int	kind;
{
    register int	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}

static struct {
    struct sxsvar	sxsvar;
    struct sxplocals	sxplocals;
}	GUIDE, RCG;


static	RCG_run (pathname)
    register char	*pathname;
{
    extern char 	*mktemp ();
    register FILE	*infile;

    if (pathname == NULL) {
	char	pathname [32];
	register int	c;

	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	if ((infile = sxfopen (mktemp (strcpy (pathname, "/tmp/RCGXXXXXX")), "w")) == NULL) {
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
	if ((infile = sxfopen (pathname, "r")) == NULL) {
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

    if (is_print_time)
	sxtime (INIT);

    sxerr_mngr (BEGIN);
    syntax (ACTION, &guide_tables);

    GUIDE.sxsvar = sxsvar;
    GUIDE.sxplocals = sxplocals;

    if (is_print_time)
	sxtime (ACTION, "\tReading guide");

    sxsvar = RCG.sxsvar;
    sxplocals = RCG.sxplocals;

    syntax (ACTION, &RCG_tables);

    RCG.sxsvar = sxsvar;
    RCG.sxplocals = sxplocals;

    sxsvar = GUIDE.sxsvar;
    sxplocals = GUIDE.sxplocals;

    sxerr_mngr (END);
    fclose (infile);
    sxsrc_mngr (FINAL);
}



/************************************************************************/
/* main function
/************************************************************************/
int main(int argc, char *argv[])
{
    int		argnum;
    BOOLEAN	in_options, is_source_file, is_stdin, is_generator_length;
    char	*str;

    sxstdout = stdout, sxstderr = stderr;

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (3);
    }

    sxopentty ();

/* valeurs par defaut */
    sxverbosep = TRUE;
    is_print_time = TRUE;
    is_no_semantics = FALSE;
    is_parse_tree_number = FALSE;
    is_default_semantics = TRUE; /* Semantique specifiee avec la grammaire */
    debug_level = 0;
    forest_level = 0; /* Ca permet d'utiliser un RCG_parser compile avec -DPARSE_FOREST avec un parser qcq */
#if 0
    forest_level = FL_n | FL_source | FL_clause | FL_rhs | FL_lhs_prdct | FL_lhs_clause; /* complet */
#endif
    is_guiding = is_full_guiding = FALSE;
    best_tree_number = 1;

/* Decodage des options */
    is_generator_length = FALSE;
    in_options = TRUE;
    is_source_file = FALSE;
    is_stdin = FALSE;
    argnum = 0;

    while (in_options && ++argnum < argc) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = TRUE;
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE;
	    break;

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

	case FOREST_LEVEL:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }

	    forest_level = (int) strtol (argv [argnum], &str, 0);

	    if (str == argv [argnum]) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }
	    
	    break;

	case GUIDING:
	    forest_level = FL_n | FL_clause | FL_lhs_prdct | FL_lhs_clause;
	    is_guiding = TRUE;
	    break;

	case FULL_GUIDING:
	    forest_level = FL_n | FL_clause | FL_rhs | FL_lhs_prdct | FL_lhs_clause;
	    is_full_guiding = TRUE;
	    break;

	case STDIN:
	    is_stdin = TRUE;
	    break;

	case DEBUG_LEVEL:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }

	    debug_level = (int) strtol (argv [argnum], &str, 0);

	    if (str == argv [argnum]) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }
	    
	    break;

	case SOURCE_FILE:
	    if (is_stdin) {
		is_stdin = FALSE;
	    }

	    is_source_file = TRUE;
	    in_options = FALSE;
	    break;

	case BEST_TREE_NUMBER:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }

	    best_tree_number = (int) strtol (argv [argnum], &str, 0);

	    if (str == argv [argnum]) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }
	    
	    break;

	case UNKNOWN_ARG:
	    fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
	    SXEXIT (3);
	}
    }

    if (!is_stdin && !is_source_file && !is_generator_length) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (3);
    }

    if (sxverbosep) {
	fprintf (sxtty, "%s\n", release_mess);
    }

    sxstr_mngr (BEGIN);

    (*(guide_tables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;

    syntax (OPEN, &RCG_tables) /* Initialisation de SYNTAX */ ;
    RCG.sxsvar = sxsvar;
    RCG.sxplocals = sxplocals;

    syntax (OPEN, &guide_tables) /* Initialisation de SYNTAX */ ;

    if (is_stdin) {
	RCG_run (NULL);
    }
    else {
	if (is_source_file) {
	    do {
		register int	severity = sxerrmngr.nbmess [2];

		sxerrmngr.nbmess [2] = 0;
		sxstr_mngr (BEGIN) /* remise a vide de la string table */ ;
		RCG_run (argv [argnum++]);
		sxerrmngr.nbmess [2] += severity;
	    } while (argnum < argc);
	}
	else {
	    /* Cas du generateur (pas de source) */
	    syntax (ACTION, &guide_tables);
	}
    }

    syntax (CLOSE, &guide_tables);

    sxsvar = RCG.sxsvar;
    sxplocals = RCG.sxplocals;

    syntax (CLOSE, &RCG_tables);

    (*(guide_tables.analyzers.parser)) (END);

    sxstr_mngr (END);

    {
	int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	SXEXIT (severity);
    }

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}


#if 0
/* Le 20/12/04 devient un module de la librairie  sxtm_mngr.c */
VOID
sxtime (what, str)
    int what;
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
#endif /* 0 */


VOID
sxvoid ()
/* procedure ne faisant rien */
{
}

BOOLEAN
sxbvoid ()
/* procedure ne faisant rien */
{
    return TRUE;
}

int
sxivoid ()
/* procedure ne faisant rien */
{
    return 1;
}


