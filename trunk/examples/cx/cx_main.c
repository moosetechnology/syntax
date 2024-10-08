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

#include "sxversion.h"
#include "sxunix.h"
#include "put_edit.h" /* cette inclusion est necessaire pour que les variables
			 _SXPE_* soient bien definies. Ces variables sont utilisees
			 par le fichier cx_act.c */

char WHAT_CXMAIN[] = "@(#)SYNTAX - $Id: cx_main.c 4173 2024-08-26 11:56:20Z garavel $";

char	by_mess [] = "the SYNTAX cross referencer CX";

extern SXINT	line_length;
static char	ME [] = "cx";

char	*prgentname;

/* On est dans un cas "mono-langage": */

extern SXTABLES	sxtables;

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
static SXINT	option_kind [] = {UNKNOWN_ARG, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LINE_LENGTH, LINE_LENGTH};



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



static char	*option_get_text (SXINT kind)
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}



static void	language_name_cx (char *path_name)
{
    char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    prgentname = (char*) sxalloc (strlen (p)+1, sizeof (char));

    if ((p = sxindex (strcpy (prgentname, p), '.')) != NULL) {
	*p = SXNUL;
    }
}



static void	cx_run (char *pathname)
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
	syntax (SXBEGIN, &sxtables, infile, "");
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

	    language_name_cx (pathname);
	    syntax (SXBEGIN, &sxtables, infile, pathname);
	}
    }

    syntax (SXACTION, &sxtables);
    syntax (SXEND, &sxtables);
    fclose (infile);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}



int main (int argc, char *argv[])
{
    int		argnum;

    sxopentty ();

/* valeurs par defaut */

    line_length = 124;

/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = true;
	    break;

	case -VERBOSE:
	    sxverbosep = false;
	    break;

	case LINE_LENGTH:
	    if (++argnum >= argc || !sxnumarg (argv [argnum], &line_length))
		fprintf (sxstderr, "%s: a numeric argument must follow the \"%s\" option;\n\tvalue %ld is retained.\n", ME
		     , option_get_text (LINE_LENGTH), (SXINT) line_length);

	    break;

	case UNKNOWN_ARG:
	    goto run;

	default:
	    fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (SXINT) option_get_kind (argv [argnum]));
	    sxexit (3);
	}
    }

run:
    if (sxverbosep) {
	fprintf (sxtty, "%s\n", release_mess);
    }

    syntax (SXINIT, &sxtables, false /* no includes */);

    if (argnum >= argc) {
	cx_run (NULL);
    }
    else {
	do {
	    int	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    sxstr_mngr (SXCLEAR) /* remise a vide de la string table */ ;
	    cx_run (argv [argnum++]);
	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    syntax (SXFINAL, &sxtables, true);

    sxexit (sxerr_max_severity ());
    return 0;
}

