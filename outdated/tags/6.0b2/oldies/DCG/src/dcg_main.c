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
/* Lun 23 Dec 1996 16:22 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#define WHAT	"@(#)dcg_main.c\t- SYNTAX [unix] - Lun 23 Dec 1996 16:22:35"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};


#include "sxunix.h"
#include "dcg.h"

/* includes et variable pour la manipulation de la date pour dcg_modif_time */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>


char	by_mess [] = "the SYNTAX grammar processor DCG";

#include "release.h"


/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

extern struct sxtables	dcg_tables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "dcg";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-nf, -normal_form, -nnf, -no_normal_form\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define NORMAL_FORM		2
#define LANGUAGE_NAME 		3
#define LAST_OPTION		LANGUAGE_NAME

static char	*option_tbl [] = {"",
				      "v", "verbose", "nv", "noverbose",
				      "nf", "normal_form", "nnf", "no_normal_form",
				      "ln", "language_name",};
static int	option_kind [] = {UNKNOWN_ARG,
				      VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				      NORMAL_FORM, NORMAL_FORM, -NORMAL_FORM, -NORMAL_FORM,
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
	*p = NUL;
    }

    if (strlen (lang_name) > 10) {
	fprintf (sxstderr, "%s: Warning, the language name \"%s\" is too long and truncated to ", ME, lang_name);
	lang_name [10] = NUL;
	fprintf (sxstderr, "\"%s\".\n", lang_name);
	sxerrmngr.nbmess [1]++;
    }
}



static VOID	dcg_run (pathname)
    register char	*pathname;
{
    register FILE	*infile;

    if (pathname == NULL) {
	char	pathname [32];
	register int	c;

	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	if ((infile = sxfopen (mktemp (strcpy (pathname, "/tmp/dcgXXXXXX")), "w")) == NULL) {
	    fprintf (sxstderr, "%s: Unable to create temp file:\t", ME);
lost:	    sxperror (pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}

	while ((c = getchar ()) != EOF) {
	    putc (c, infile);
	}

	if ((infile = sxfreopen (pathname, "r", infile)) == NULL) {
	    fprintf (sxstderr, "%s: Unable to reopen temp file:\t", ME);
	    goto lost;
	}

	unlink (pathname) /* vanishes at once, dies when closed */ ;
	rewind (infile);
	sxsrc_mngr (INIT, infile, "");
    }
    else if ((infile = sxfopen (pathname, "r")) == NULL) {
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
	sxsrc_mngr (INIT, infile, pathname);
	stat (sxsrcmngr.source_coord.file_name, &buf);
    }

    sxerr_mngr (BEGIN);
    syntax (ACTION, &dcg_tables);
    sxsrc_mngr (FINAL);
    sxerr_mngr (END);
    fclose (infile);
}



main (argc, argv)
    int		argc;
    char	*argv [];
{
    int	argnum;

    if (sxstdout == NULL) {
	sxstdout = stdout;
    }
    if (sxstderr == NULL) {
	sxstderr = stderr;
    }

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (SEVERITIES);
    }

    sxopentty ();

/* valeurs par defaut */

    options_set = OPTION (VERBOSE) | OPTION(NORMAL_FORM);
    sxverbosep = TRUE;
    is_normal_form = TRUE;

/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = TRUE, options_set |= OPTION (VERBOSE);
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE, options_set &= noOPTION (VERBOSE);
	    break;

	case NORMAL_FORM:
	    is_normal_form = TRUE, options_set |= OPTION (NORMAL_FORM);
	    break;

	case -NORMAL_FORM:
	    is_normal_form = FALSE, options_set &= noOPTION (NORMAL_FORM);
	    break;

	case LANGUAGE_NAME:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: a name must follow the \"%s\" option;\n\t'' is assumed.\n", ME, option_get_text (
		     LANGUAGE_NAME));
		prgentname [0] = NUL;
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
	    SXEXIT (3);
	}
    }

    if (!(options_set & OPTION (LANGUAGE_NAME))) {
	fprintf (sxstderr, "%s: the \"%s\" option is mandatory when no file is named;\n\t'' is assumed\n", ME,
	     option_get_text (LANGUAGE_NAME));
	prgentname [0] = NUL;
    }

run:
    if (sxverbosep) {
	fprintf (sxtty, "%s\n", release_mess);
    }

    sxstr_mngr (BEGIN);
    (*(dcg_tables.analyzers.parser)) (BEGIN);
    syntax (OPEN, &dcg_tables);

    if (options_set & OPTION (LANGUAGE_NAME)) {
	dcg_run (NULL);
    }
    else {
	do {
	    register int	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    dcg_run (argv [argnum++]);
	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    syntax (CLOSE, &dcg_tables);
    (*(dcg_tables.analyzers.parser)) (END);
    sxstr_mngr (END);

    {
	register int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	SXEXIT (severity);
    }
}



char	*options_text (line)
    register char	*line;
{
    register int	i;
    BOOLEAN	is_first = TRUE;

    *line = NUL;

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = FALSE;
	    else
		strcat (line, ", ");

	    strcat (line, option_get_text (i));

	    if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
}



VOID	sxvoid ()
/* procedure ne faisant rien */
{
}
