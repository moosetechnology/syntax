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
/* 20040112 14:10 (phd):	Adaptation � G5 (faux 64 bits)		*/
/************************************************************************/
/* 12 Fev 2003 11:0 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#define SX_DFN_EXT_VAR	/* N�cessaire sur G5... */
#include "sxunix.h"
#include "release.h"
char WHAT_XBNF2LEXPRDCTMAIN[] = "@(#)SYNTAX - $Id: xbnf2lex_prdct_main.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;

/* includes et variable pour la manipulation de la date pour bnf_modif_time */


char	by_mess [] = "the SYNTAX grammar pre-processor xbnf2lex_prdct";

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};

extern struct sxtables	xbnf_tables;

char	*prgentname, *processorname;
long	options_set;
BOOLEAN	is_list, is_source, is_proper, is_predicate;
int	max_RHS;


/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "xbnf2lex_prdct";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-sc, -source, -nsc, -nosource,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ls, -list, -nls, -nolist,\n\
\t\t-rhs nnn, -max_right_hand_side nnn,\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define SOURCE 			1
#define VERBOSE 		2
#define LIST 			3
#define MAX_RIGHT_HAND_SIDE 	4
#define LANGUAGE_NAME		5
#define LAST_OPTION		LANGUAGE_NAME

static char	*option_tbl [] = {"", "sc", "source", "nsc", "nosource", "v", "verbose", "nv", "noverbose", "ls", "list",
     "nls", "nolist", "rhs", "max_right_hand_side", "ln", "language_name",};
static int	option_kind [] = {UNKNOWN_ARG, SOURCE, SOURCE, -SOURCE, -SOURCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LIST, LIST, -LIST, -LIST, MAX_RIGHT_HAND_SIDE, MAX_RIGHT_HAND_SIDE, LANGUAGE_NAME, LANGUAGE_NAME,};



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
	processorname = p+1;
    }
}



static VOID	xbnf2lex_prdct_run (pathname)
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
    else if ((infile = sxfopen (pathname, "r")) == NULL) {
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

    sxerr_mngr (BEGIN);
    syntax (ACTION, &xbnf_tables);
    sxsrc_mngr (FINAL);
    sxerr_mngr (END);
    fclose (infile);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
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

    options_set = OPTION (SOURCE) | OPTION (VERBOSE) | OPTION (LIST);
    is_source = sxverbosep = is_list = TRUE;
    max_RHS = -1; /* Le 16/4/2002, par defaut, pas de verif de la longueur des RHS */


/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case SOURCE:
	    is_source = TRUE, options_set |= OPTION (SOURCE);
	    break;

	case -SOURCE:
	    is_list = is_source = FALSE, options_set &= noOPTION (LIST) & noOPTION (SOURCE);
	    break;

	case VERBOSE:
	    sxverbosep = TRUE, options_set |= OPTION (VERBOSE);
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE, options_set &= noOPTION (VERBOSE);
	    break;

	case LIST:
	    is_list = is_source = TRUE, options_set |= OPTION (LIST) | OPTION (LIST);
	    break;

	case -LIST:
	    is_list = FALSE, options_set &= noOPTION (LIST);
	    break;

	case MAX_RIGHT_HAND_SIDE:
	    if (++argnum >= argc || !sxnumarg (argv [argnum], &max_RHS))
		fprintf (sxstderr, "%s: a numeric argument must follow the \"%s\" option;\n\tvalue %d is retained.\n", ME
		     , option_get_text (MAX_RIGHT_HAND_SIDE), max_RHS);
	    else {
		options_set |= OPTION (MAX_RIGHT_HAND_SIDE);
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
	prgentname = (char*) sxalloc (1, sizeof (char));
	prgentname [0] = NUL;
    }

run:
    if (sxverbosep) {
	fprintf (sxtty, "%s\n", release_mess);
    }

    sxstr_mngr (BEGIN);
    (*(xbnf_tables.analyzers.parser)) (BEGIN);
    syntax (OPEN, &xbnf_tables);

    if (options_set & OPTION (LANGUAGE_NAME)) {
	xbnf2lex_prdct_run (NULL);
    }
    else {
	do {
	    register int	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    xbnf2lex_prdct_run (argv [argnum++]);
	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    syntax (CLOSE, &xbnf_tables);
    (*(xbnf_tables.analyzers.parser)) (END);
    sxstr_mngr (END);

    {
	int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	SXEXIT (severity);
    }

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
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

	    if (i == MAX_RIGHT_HAND_SIDE) {
		sprintf (line + strlen (line), "=%d", max_RHS);
	    }
	    else if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
}



VOID	sxvoid ()
/* procedure ne faisant rien */
{
}
