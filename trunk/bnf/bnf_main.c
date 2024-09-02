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

#define SX_DFN_EXT_VAR_BNF /* Pour definir variables globales du module BNF */

#include "sxversion.h"
#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"
#include "put_edit.h"

/* includes et variable pour la manipulation de la date pour bnf_modif_time */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>

char WHAT_BNFMAIN[] = "@(#)SYNTAX - $Id: bnf_main.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

char	by_mess [] = "the SYNTAX grammar processor BNF";

#include "sxversion.h"

SXINT	(*more_scan_act) (SXINT code, SXINT act_no);


extern void	no_tables (void), bnf_lo (void);
extern SXTABLES	bnf_tables;

extern void bnf_huge (struct bnf_ag_item *B, char *langname);
extern void bnf_terminal (struct bnf_ag_item *B, char *langname);

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "bnf";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-sc, -source, -nsc, -nosource,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ls, -list, -nls, -nolist,\n\
\t\t-rhs nnn, -max_right_hand_side nnn,\n\
\t\t-ln name, -language_name name\n\
\t\t-h, -huge\n\
\t\t-t, -terminal\n\
\t\t-col1\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	((SXINT) 1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define SOURCE 			((SXINT)1)
#define VERBOSE 		((SXINT)2)
#define LIST 			((SXINT)3)
#define MAX_RIGHT_HAND_SIDE 	((SXINT)4)
#define LANGUAGE_NAME		((SXINT)5)
#define HUGE		        ((SXINT)6)
#define TERMINAL	        ((SXINT)7)
#define COL_1                   ((SXINT)8)
#define LAST_OPTION		COL_1

static char	*option_tbl [] = {"", "sc", "source", "nsc", "nosource", "v", "verbose", "nv", "noverbose", "ls", "list",
     "nls", "nolist", "rhs", "max_right_hand_side", "ln", "language_name", "h", "huge", "t", "terminal","col1", };
static SXINT	option_kind [] = {UNKNOWN_ARG, SOURCE, SOURCE, -SOURCE, -SOURCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LIST, LIST, -LIST, -LIST, MAX_RIGHT_HAND_SIDE, MAX_RIGHT_HAND_SIDE, LANGUAGE_NAME, LANGUAGE_NAME, HUGE, HUGE, TERMINAL, TERMINAL,COL_1};



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



static void	bnf_run (char *pathname)
{
    FILE	*infile;

    if (pathname == NULL) {
        int c;

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
	syntax (SXBEGIN, &bnf_tables, infile, "");
	bnf_ag.bnf_modif_time = time (0) /* stdin => now */ ;
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

	extract_language_name (pathname);
	syntax (SXBEGIN, &bnf_tables, infile, pathname);
	stat (sxsrcmngr.source_coord.file_name, &buf);
	bnf_ag.bnf_modif_time = buf.st_mtime;
    }

    syntax (SXACTION, &bnf_tables);

    if (!is_proper || IS_ERROR)
	no_tables ();
    else {
      if (is_huge)
	bnf_huge (&bnf_ag, prgentname);
      else {
	if (is_terminal)
	  bnf_terminal (&bnf_ag, prgentname);
	  
	bnf_write (&bnf_ag, prgentname);
      }
    }

    bnf_lo ();
    syntax (SXEND, &bnf_tables);
    fclose (infile);
    bnf_free (&bnf_ag);

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

#if EBUG
    fprintf (sxstdout, "\n(bnf_)main : Magic numbers (bnf_tables = %lu, local value = %lu)\n", bnf_tables.magic, SXMAGIC_NUMBER);
#endif

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	sxexit (SXSEVERITIES);
    }

/* valeurs par defaut */

    options_set = OPTION (SOURCE) | OPTION (VERBOSE) | OPTION (LIST);
    is_source = is_list = true;
    max_RHS = -1; /* Le 16/4/2002, par defaut, pas de verif de la longueur des RHS */
    is_huge = is_terminal = is_col_1 = false;


/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case SOURCE:
	    is_source = true, options_set |= OPTION (SOURCE);
	    break;

	case -SOURCE:
	    is_list = is_source = false, options_set &= noOPTION (LIST) & noOPTION (SOURCE);
	    break;

	case VERBOSE:
	    sxverbosep = true, options_set |= OPTION (VERBOSE);
	    break;

	case -VERBOSE:
	    sxverbosep = false, options_set &= noOPTION (VERBOSE);
	    break;

	case LIST:
	    is_list = is_source = true, options_set |= OPTION (LIST) | OPTION (LIST);
	    break;

	case -LIST:
	    is_list = false, options_set &= noOPTION (LIST);
	    break;

	case MAX_RIGHT_HAND_SIDE:
	    if (++argnum >= argc || !sxnumarg (argv [argnum], &max_RHS))
		fprintf (sxstderr, "%s: a numeric argument must follow the \"%s\" option;\n\tvalue %ld is retained.\n", ME
		     , option_get_text (MAX_RIGHT_HAND_SIDE), (SXINT) max_RHS);
	    else {
		options_set |= OPTION (MAX_RIGHT_HAND_SIDE);
	    }

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

	    options_set = OPTION (LANGUAGE_NAME);
	    break;

	case HUGE:
	    is_huge = true, options_set |= OPTION (HUGE);
	    is_terminal = false;
	    break;

	case TERMINAL:
	    is_terminal = true, options_set |= OPTION (TERMINAL);
	    is_huge = false;
	    break;

	case COL_1:
	    is_col_1 = true;
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
	    sxexit (3);
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
	fprintf (sxtty, "%s\n", release_mess);
    }

    syntax (SXINIT, &bnf_tables, false /* no includes */);

    if (options_set & OPTION (LANGUAGE_NAME)) {
	bnf_run ((char*)NULL);
    }
    else {
	do {
	    int	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    bnf_run (argv [argnum++]);
	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    syntax (SXFINAL, &bnf_tables, true);

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

	    if (i == MAX_RIGHT_HAND_SIDE) {
		sprintf (line + strlen (line), "=%ld", (SXINT) max_RHS);
	    }
	    else if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
}

