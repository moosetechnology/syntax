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


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 09-08-94 13:41 	(pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#define WHAT	"@(#)xtag_main.c\t- SYNTAX [unix] - Mer 4 Jan 1995 10:14:22"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};


#include "sxunix.h"
#include "xtag.h"

/* includes et variable pour la manipulation de la date pour xtag_modif_time */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>


char	by_mess [] = "the SYNTAX grammar processor XTAG2LIG";

#include "release.h"

extern void xtag_second_pass ();

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {stdout}, *sxstderr = {stderr};
FILE	*sxtty;

extern struct sxtables	xtag_tables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "xtag2lig";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-nf, -normal_form, -nnf, -no_normal_form\n\
\t\t-tes, -test_empty_stack, -ntes, -no_test_empty_stack\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define NORMAL_FORM		2
#define TEST_EMPTY_STACK	3
#define LANGUAGE_NAME 		4
#define LAST_OPTION		LANGUAGE_NAME

static char	*option_tbl [] = {"",
				      "v", "verbose", "nv", "noverbose",
				      "nf", "normal_form", "nnf", "no_normal_form",
				      "tes", "test_empty_stack", "ntes", "no_test_empty_stack",
				      "ln", "language_name",};
static int	option_kind [] = {UNKNOWN_ARG,
				      VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				      NORMAL_FORM, NORMAL_FORM, -NORMAL_FORM, -NORMAL_FORM,
				      TEST_EMPTY_STACK, TEST_EMPTY_STACK, -TEST_EMPTY_STACK, -TEST_EMPTY_STACK,
				      LANGUAGE_NAME, LANGUAGE_NAME,};

static int	option_get_kind (arg)
    register char	*arg;
{
    register char	**opt;

    if (*arg++ != '-')
	return UNKNOWN_ARG;

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



static	language_name (path_name, lang_name)
    char	*path_name, *lang_name;
{
    register char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    if ((p = sxindex (strcpy (lang_name, p), '.')) != NULL) {
	*p = SXNUL;
    }

    if (strlen (lang_name) > 20) {
	fprintf (sxstderr, "%s: Warning, the language name \"%s\" is too long and truncated to ", ME, lang_name);
	lang_name [20] = SXNUL;
	fprintf (sxstderr, "\"%s\".\n", lang_name);
	sxerrmngr.nbmess [1]++;
    }
}



static SXVOID	xtag_run (pathname)
    register char	*pathname;
{
    register FILE	*infile;

    if (pathname == NULL) {
	char	pathname [32];
	register int	c;

	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	if ((infile = fopen (mktemp (strcpy (pathname, "/tmp/tagXXXXXX")), "w")) == NULL) {
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
	sxsrc_mngr (SXINIT, infile, "");
    }
    else if ((infile = fopen (pathname, "r")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	sxperror (pathname);
	sxerrmngr.nbmess [2]++;
	return;
    }
    else {
	struct stat	buf /* date/time info */ ;

	if (sxverbosep) {
	    fprintf (sxtty, "%s:\n", pathname);
	}

	language_name (pathname, prgentname);
	sxsrc_mngr (SXINIT, infile, pathname);
	stat (sxsrcmngr.source_coord.file_name, &buf);
    }

    sxerr_mngr (SXBEGIN);
    syntax (SXACTION, &xtag_tables);
    sxsrc_mngr (SXFINAL);
    sxerr_mngr (SXEND);
    fclose (infile);
}



main (argc, argv)
    int		argc;
    char	*argv [];
{
    register int	argnum;

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	sxexit (SXSEVERITIES);
    }

    sxopentty ();

/* valeurs par defaut */

    options_set = OPTION (VERBOSE) | OPTION(NORMAL_FORM) | OPTION(TEST_EMPTY_STACK);
    sxverbosep = SXTRUE;
    is_normal_form = SXTRUE;
    is_test_empty_stack = SXTRUE;


/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = SXTRUE, options_set |= OPTION (VERBOSE);
	    break;

	case -VERBOSE:
	    sxverbosep = SXFALSE, options_set &= noOPTION (VERBOSE);
	    break;

	case NORMAL_FORM:
	    is_normal_form = SXTRUE, options_set |= OPTION (NORMAL_FORM);
	    break;

	case -NORMAL_FORM:
	    is_normal_form = SXFALSE, options_set &= noOPTION (NORMAL_FORM);
	    break;

	case TEST_EMPTY_STACK:
	    is_test_empty_stack = SXTRUE, options_set |= OPTION (TEST_EMPTY_STACK);
	    break;

	case -TEST_EMPTY_STACK:
	    is_test_empty_stack = SXFALSE, options_set &= noOPTION (TEST_EMPTY_STACK);
	    break;

	case LANGUAGE_NAME:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: a name must follow the \"%s\" option;\n\t'' is assumed.\n", ME, option_get_text (
		     LANGUAGE_NAME));
		prgentname [0] = SXNUL;
	    }
	    else {
		strcpy (prgentname, argv [argnum]);
	    }

	    options_set = OPTION (LANGUAGE_NAME);
	    break;

	case UNKNOWN_ARG:
	    if (options_set & OPTION (LANGUAGE_NAME)) {
		fprintf (sxstderr, "%s: the \"%s\" option must not be used when naming files.\n", ME, option_get_text (
		     LANGUAGE_NAME));
		options_set &= noOPTION (LANGUAGE_NAME);
	    }

	    goto run;

	default:
	    fprintf (sxstderr, "%s: internal error %d while decoding options.\n", ME, option_get_kind (argv [argnum]));
	    sxexit (3);
	}
    }

    if (!(options_set & OPTION (LANGUAGE_NAME))) {
	fprintf (sxstderr, "%s: the \"%s\" option is mandatory when no file is named;\n\t'' is assumed\n", ME,
	     option_get_text (LANGUAGE_NAME));
	prgentname [0] = SXNUL;
    }

run:
    if (sxverbosep) {
	fprintf (sxtty, "%s\n", release_mess);
    }

    sxstr_mngr (SXBEGIN);
    (*(xtag_tables.analyzers.parser)) (SXBEGIN);
    syntax (SXOPEN, &xtag_tables);

    if (options_set & OPTION (LANGUAGE_NAME)) {
	xtag_run (NULL);
    }
    else {
	do {
	    register int	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    xtag_run (argv [argnum++]);

	    if (argnum < argc)
		  /* On libere l'arbre abstrait */
	      sxatc (SXCLOSE, &xtag_tables);

	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    syntax (SXCLOSE, &xtag_tables);
    (*(xtag_tables.analyzers.parser)) (SXEND);

    xtag_second_pass ();

    sxstr_mngr (SXEND);

    {
	register int	severity;

	for (severity = SXSEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	sxexit (severity);
    }
}



char	*options_text (line)
    register char	*line;
{
    register int	i;
    SXBOOLEAN	is_first = SXTRUE;

    *line = SXNUL;

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = SXFALSE;
	    else
		strcat (line, ", ");

	    strcat (line, option_get_text (i));

	    if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
}



SXVOID	sxvoid ()
/* procedure ne faisant rien */
{
}
