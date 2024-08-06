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

/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 

/* Programme principal pour l'analyse de textes decrits a l'aide
   d'une grammaire RCG.  Le source est un "guide" produit par
   une execution d'une 1-RCG */

#include "sxversion.h"
#include "sxunix.h"

char WHAT_RCG_GUIDED[] = "@(#)SYNTAX - $Id: RCG_guided_main.c 4147 2024-08-02 10:32:28Z garavel $" WHAT_DEBUG;

#include "rcg_sglbl.h"
#include <stdlib.h>

/* void sxtime (); */

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

static SXINT	option_kind [] = {
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



static SXINT	option_get_kind (char	*arg)
{
    char	**opt;

    if (*arg++ != '-')
	return SOURCE_FILE;

    if (*arg == SXNUL)
	return STDIN;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}

#if 0

static char	*option_get_text (SXINT	kind)
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}

#endif

static struct {
    struct sxsvar	sxsvar;
    struct sxplocals	sxplocals;
}	GUIDE, RCG;


static	void RCG_run (char *pathname)
{
    FILE	*infile;

    if (pathname == NULL) {
	char	tmp_pathname [32];
	SXINT	c;

	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	if ((infile = sxfdopen (mkstemp (strcpy (tmp_pathname, "/tmp/RCGXXXXXX")), "w")) == NULL) {
	    fprintf (sxstderr, "%s: Unable to create temp file:\t", ME);
lost:	    sxperror (tmp_pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}

	while ((c = getchar ()) != EOF) {
	    putc (c, infile);
	}

	if ((infile = freopen (tmp_pathname, "r", infile)) == NULL) {
	    fprintf (sxstderr, "%s: Unable to reopen temp file:\t", ME);
	    goto lost;
	}

	unlink (tmp_pathname) /* vanishes at once, dies when closed */ ;
	rewind (infile);
	syntax (SXBEGIN, &guide_tables, infile, "");
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

	    syntax (SXBEGIN, &guide_tables, infile, pathname);
	}
    }

    if (is_print_time)
	 sxtime (SXINIT, (char *) NULL);

    syntax (SXACTION, &guide_tables);

    GUIDE.sxsvar = sxsvar;
    GUIDE.sxplocals = sxplocals;

    if (is_print_time)
	sxtime (SXACTION, "\tReading guide");

    sxsvar = RCG.sxsvar;
    sxplocals = RCG.sxplocals;

    syntax (SXACTION, &RCG_tables);

    RCG.sxsvar = sxsvar;
    RCG.sxplocals = sxplocals;

    sxsvar = GUIDE.sxsvar;
    sxplocals = GUIDE.sxplocals;

    syntax (SXEND, &guide_tables);
    fclose (infile);
}


/************************************************************************/
/* main function */
/************************************************************************/

int main(int argc, char *argv[])
{
    SXINT	argnum;
    bool	in_options, is_source_file, is_stdin, is_generator_length;
    char	*str;

    sxopentty ();

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	sxexit (3);
    }

/* valeurs par defaut */
    sxverbosep = false;
    is_print_time = true;
    is_no_semantics = false;
    is_parse_tree_number = false;
    is_default_semantics = true; /* Semantique specifiee avec la grammaire */
    debug_level = 0;
    forest_level = 0; /* Ca permet d'utiliser un RCG_parser compile avec -DPARSE_FOREST avec un parser qcq */
#if 0
    forest_level = FL_n | FL_source | FL_clause | FL_rhs | FL_lhs_prdct | FL_lhs_clause; /* complet */
#endif
    is_guiding = is_full_guiding = false;
    best_tree_number = 1;

/* Decodage des options */
    is_generator_length = false;
    in_options = true;
    is_source_file = false;
    is_stdin = false;
    argnum = 0;

    while (in_options && ++argnum < argc) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = true;
	    break;

	case -VERBOSE:
	    sxverbosep = false;
	    break;

	case NO_SEM:
	    is_no_semantics = true;
	    is_default_semantics = false;
	    is_parse_tree_number = false;
	    break;

	case DEFAULT_SEM:
	    is_no_semantics = false;
	    is_default_semantics = true;
	    is_parse_tree_number = false;
	    break;

	case PARSE_TREE_NUMBER:
	    is_no_semantics = false;
	    is_default_semantics = false;
	    is_parse_tree_number = true;
	    break;

	case TIME:
	    is_print_time = true;
	    break;

	case -TIME:
	    is_print_time = false;
	    break;

	case FOREST_LEVEL:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
		sxexit (3);
	    }

	    forest_level = (SXINT) strtol (argv [argnum], &str, 0);

	    if (str == argv [argnum]) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
		sxexit (3);
	    }
	    
	    break;

	case GUIDING:
	    forest_level = FL_n | FL_clause | FL_lhs_prdct | FL_lhs_clause;
	    is_guiding = true;
	    break;

	case FULL_GUIDING:
	    forest_level = FL_n | FL_clause | FL_rhs | FL_lhs_prdct | FL_lhs_clause;
	    is_full_guiding = true;
	    break;

	case STDIN:
	    is_stdin = true;
	    break;

	case DEBUG_LEVEL:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
		sxexit (3);
	    }

	    debug_level = (SXINT) strtol (argv [argnum], &str, 0);

	    if (str == argv [argnum]) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
		sxexit (3);
	    }
	    
	    break;

	case SOURCE_FILE:
	    if (is_stdin) {
		is_stdin = false;
	    }

	    is_source_file = true;
	    in_options = false;
	    break;

	case BEST_TREE_NUMBER:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
		sxexit (3);
	    }

	    best_tree_number = (SXINT) strtol (argv [argnum], &str, 0);

	    if (str == argv [argnum]) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
		sxexit (3);
	    }
	    
	    break;

	case UNKNOWN_ARG:
	    fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
	    sxexit (3);
	}
    }

    if (!is_stdin && !is_source_file && !is_generator_length) {
	fprintf (sxstderr, Usage, ME);
	sxexit (3);
    }

    if (sxverbosep) {
	fprintf (sxtty, "%s\n", release_mess);
    }

    syntax (SXOPEN, &RCG_tables) /* Initialisation de SYNTAX */ ;
    RCG.sxsvar = sxsvar;
    RCG.sxplocals = sxplocals;

    syntax (SXINIT, &guide_tables, false /* no includes */);

    if (is_stdin) {
	RCG_run (NULL);
    }
    else {
	if (is_source_file) {
	    do {
		SXINT	severity = sxerrmngr.nbmess [2];

		sxerrmngr.nbmess [2] = 0;
		sxstr_mngr (SXCLEAR) /* remise a vide de la string table */ ;
		RCG_run (argv [argnum++]);
		sxerrmngr.nbmess [2] += severity;
	    } while (argnum < argc);
	}
	else {
	    /* Cas du generateur (pas de source) */
	    syntax (SXACTION, &guide_tables);
	}
    }

    syntax (SXFINAL, &guide_tables, false); /* sxstr_mngr (SXEND) called later */

    sxsvar = RCG.sxsvar;
    sxplocals = RCG.sxplocals;

    syntax (SXCLOSE, &RCG_tables);

    sxstr_mngr (SXEND);

    sxexit (sxerr_max_severity ());

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}


