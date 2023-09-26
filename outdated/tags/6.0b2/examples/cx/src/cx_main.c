/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1985 by Institut National de Recherche *
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
/* 20030515 14:43 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 20020318 14:30 (phd):	"sxtmpfile" dans "cx_run"		*/
/************************************************************************/
/* 20020318 14:30 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
char WHAT_CXMAIN[] = "@(#)SYNTAX - $Id: cx_main.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

char	by_mess [] = "the SYNTAX cross referencer CX";

#include "release.h"


extern int	line_length;
BOOLEAN		sxverbosep;
static char	ME [] = "cx";

char	*prgentname;

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout, *sxstderr;
FILE	*sxtty;

/* On est dans un cas "mono-langage": */

extern struct sxtables	cx_tables;


/*---------------*/
/*    options    */
/*---------------*/

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define LINE_LENGTH	 	2
#define LAST_OPTION		LINE_LENGTH

static char	*option_tbl [] = {"", "v", "verbose", "nv", "noverbose", "ll", "line_length"};
static int	option_kind [] = {UNKNOWN_ARG, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LINE_LENGTH, LINE_LENGTH};



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




static SXVOID	language_name (path_name)
    char	*path_name;
{
    register char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    prgentname = (char*) sxalloc (strlen (p)+1, sizeof (char));

    if ((p = sxindex (strcpy (prgentname, p), '.')) != NULL) {
	*p = NUL;
    }
}



static SXVOID	cx_run (pathname)
    register char	*pathname;
{
    register FILE	*infile;

    if (pathname == NULL) {
	register int	c;

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

	    language_name (pathname);
	    sxsrc_mngr (INIT, infile, pathname);
	}
    }

    sxerr_mngr (BEGIN);
    syntax (ACTION, &cx_tables);
    sxerr_mngr (END);
    fclose (infile);
    sxsrc_mngr (FINAL);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}




main (argc, argv)
    int		argc;
    char	*argv [];
{
    int		argnum;

    sxstdout = stdout, sxstderr = stderr;

/* valeurs par defaut */

    sxverbosep = FALSE;
    line_length = 124;

/* let's go */

    sxopentty ();

/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = TRUE;
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE;
	    break;

	case LINE_LENGTH:
	    if (++argnum >= argc || !sxnumarg (argv [argnum], &line_length))
		fprintf (sxstderr, "%s: a numeric argument must follow the \"%s\" option;\n\tvalue %d is retained.\n", ME
		     , option_get_text (LINE_LENGTH), line_length);

	    break;

	case UNKNOWN_ARG:
	    goto run;

	default:
	    fprintf (sxstderr, "%s: internal error %d while decoding options.\n", ME, option_get_kind (argv [argnum]));
	    SXEXIT (3);
	}
    }

run:
    if (sxverbosep) {
	fprintf (sxtty, "%s\n", release_mess);
    }

    sxstr_mngr (BEGIN);
    (*(cx_tables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
    syntax (OPEN, &cx_tables) /* Initialisation de SYNTAX */ ;

    if (argnum >= argc) {
	cx_run (NULL);
    }
    else {
	do {
	    register int	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    sxstr_mngr (BEGIN) /* remise a vide de la string table */ ;
	    cx_run (argv [argnum++]);
	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    syntax (CLOSE, &cx_tables);
    (*(cx_tables.analyzers.parser)) (END) /* Inutile puisque le process va etre termine */ ;
    sxstr_mngr (END) /* Inutile puisque le process va etre termine */ ;

    {
	register int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	SXEXIT (severity);
    }
}



VOID	sxvoid ()
/* procedure ne faisant rien */
{
}
