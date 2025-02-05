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

/* Constructeur d'analyseurs lexicaux de SYNTAX */

#define SX_DFN_EXT_VAR_LECL

#include "sxversion.h"
#include "sxunix.h"
#include "varstr.h"
#include "lecl_ag.h"
#include "put_edit.h"

char WHAT_LECLMAIN[] = "@(#)SYNTAX - $Id: lecl_main.c 4180 2024-08-26 15:26:45Z garavel $" WHAT_DEBUG;

char	by_mess [] = "the SYNTAX lexical constructor LECL";

extern SXTABLES	sxtables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "lecl";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-sc, -source, -nsc, -nosource,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ob, -object, -nob, -noobject\n\
\t\t-tb, -table, -ntb, -notable\n\
\t\t-ls, -list, -nls, -nolist,\n\
\t\t-d, -dico, -nd, -nodico,\n\
\t\t-ot, -optimize, -not, -nooptimize,\n\
\t\t-d, -deterministic, -nd, -nondeterministic,\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	((SXINT) 1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  ((SXINT)0)
#define SOURCE		  ((SXINT)1)
#define VERBOSE		  ((SXINT)2)
#define OBJECT		  ((SXINT)3)
#define TABLE		  ((SXINT)4)
#define LIST		  ((SXINT)5)
#define OPTIMIZE	  ((SXINT)6)
#define NONDETERMINISTIC  ((SXINT)7)
#define DICO		  ((SXINT)8)
#define LANGUAGE_NAME	  ((SXINT)9)
#define LAST_OPTION	  LANGUAGE_NAME


static char	*option_tbl [] = {
    "",
    "sc", "source", "nsc", "nosource",
    "v", "verbose", "nv", "noverbose",
    "ob", "object", "nob", "noobject",
    "tb", "table", "ntb", "notable",
    "ls", "list", "nls", "nolist",
    "ot", "optimize", "not", "nooptimize",
    "d", "deterministic", "nd", "nondeterministic",
    "d", "dico", "nod", "nodico",
    "ln", "language_name",};

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    SOURCE, SOURCE, -SOURCE, -SOURCE,
    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
    OBJECT, OBJECT, -OBJECT, -OBJECT,
    TABLE, TABLE, -TABLE, -TABLE,
    LIST, LIST, -LIST, -LIST,
    OPTIMIZE, OPTIMIZE, -OPTIMIZE, -OPTIMIZE,
    -NONDETERMINISTIC, -NONDETERMINISTIC, NONDETERMINISTIC, NONDETERMINISTIC,
    DICO, DICO, -DICO, -DICO, 
    LANGUAGE_NAME, LANGUAGE_NAME,};



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



static void	extract_language_name (char *path_name)
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



static	void lecl_run (char *pathname)
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

	    extract_language_name (pathname);
	    syntax (SXBEGIN, &sxtables, infile, pathname);
	}
    }

    syntax (SXACTION, &sxtables);
    syntax (SXEND, &sxtables);
    fclose (infile);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main(int argc, char *argv[])
{
    int 	argnum;

    sxopentty ();

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	sxexit (3);
    }

    sxttycol1p = true;

/* valeurs par defaut */

    options_set = OPTION (SOURCE) | OPTION (VERBOSE) | OPTION (OBJECT) | OPTION (OPTIMIZE);
    is_source = is_object = is_optimize = is_dico = true;
    is_check = is_ppt = is_table = is_list = is_non_deterministic_automaton = false;

/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case SOURCE:
	    is_source = true, options_set |= OPTION (SOURCE);
	    break;

	case -SOURCE:
	    is_source = is_object = is_table = is_list = false, options_set &= noOPTION (SOURCE) & noOPTION (OBJECT) &
		 noOPTION (TABLE) & noOPTION (LIST);
	    break;

	case VERBOSE:
	    sxverbosep = true, options_set |= OPTION (VERBOSE);
	    break;

	case -VERBOSE:
	    sxverbosep = false, options_set &= noOPTION (VERBOSE);
	    break;

	case OBJECT:
	    is_source = is_object = true, options_set |= OPTION (SOURCE) | OPTION (OBJECT);
	    break;

	case -OBJECT:
	    is_object = false, options_set &= noOPTION (OBJECT);
	    break;

	case TABLE:
	    is_source = is_table = true, options_set |= OPTION (SOURCE) | OPTION (TABLE);
	    break;

	case -TABLE:
	    is_table = false, options_set &= noOPTION (TABLE);
	    break;

	case LIST:
	    is_source = is_object = is_table = is_list = true, options_set |= OPTION (SOURCE) | OPTION (OBJECT) | OPTION
		 (TABLE) | OPTION (LIST);
	    break;

	case -LIST:
	    is_list = false, options_set &= noOPTION (LIST);
	    break;

	case OPTIMIZE:
	    is_optimize = true, options_set |= OPTION (OPTIMIZE);
	    break;

	case -OPTIMIZE:
	    is_optimize = false, options_set &= noOPTION (OPTIMIZE);
	    break;

	case DICO:
	    is_dico = true, options_set |= OPTION (DICO);
	    break;

	case -DICO:
	    is_dico = false, options_set &= noOPTION (DICO);
	    break;

	case -NONDETERMINISTIC:
	    is_non_deterministic_automaton = false, options_set &= noOPTION (NONDETERMINISTIC);
	    break;

	case NONDETERMINISTIC:
	    is_non_deterministic_automaton = true, options_set |= OPTION (NONDETERMINISTIC);
	    break;

	case LANGUAGE_NAME:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: a name must follow the \"%s\" option;\n\t'' is assumed.\n", ME, option_get_text (
		     LANGUAGE_NAME));
		prgentname = (char*) sxalloc (1, sizeof (char));
		prgentname [0] = SXNUL;
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
	    fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (SXINT) option_get_kind (argv [argnum]));
	    break;
	}
    }

    if (!(options_set & OPTION (LANGUAGE_NAME))) {
	fprintf (sxstderr, "%s: the \"%s\" option is mandatory when no file is named;\n\t'' is assumed\n", ME,
	     option_get_text (LANGUAGE_NAME));
	prgentname = (char*) sxalloc (1, sizeof (char));
	prgentname [0] = SXNUL;
    }

run:
    if (sxverbosep) {
	setbuf (stdout, NULL);
	fprintf (sxtty, "%s\n", release_mess);
    }

    syntax (SXINIT, &sxtables, false /* no includes */);

    if (options_set & OPTION (LANGUAGE_NAME)) {
	lecl_run ((char*)NULL);
    }
    else {
	do {
	    int	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    sxstr_mngr (SXCLEAR) /* remise a vide de la string table */ ;
	    lecl_run (argv [argnum++]);
	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    syntax (SXFINAL, &sxtables, true);

    sxexit (sxerr_max_severity ());

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



char	*options_text (char *line)
{
    SXINT	i;
    bool	is_first = true;

    *line = SXNUL;

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = false;
	    else
		strcat (line, ", ");

	    strcat (line, option_get_text (i));
	}

    return line;
}

