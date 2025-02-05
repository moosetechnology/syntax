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

char WHAT_LFGLEXMAIN[] = "@(#)SYNTAX - $Id: lfg_lex_main.c 4203 2024-08-29 13:45:46Z garavel $" WHAT_DEBUG;

bool           use_a_dico, ignore_warnings, make_proper_will_be_used;
char              *path_name, *prgent_name;
SXUINT lfg_lex_time;

extern SXTABLES	sxtables;


/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "lfg_lex";
static char	Usage [] = "\
Usage:\t%s [options] [source_file]\n\
options=\
\t--help,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-dico, -nodico (sxword_mngr),\n\
\t\t-ignore_warnings, -iw,\n\
\t\t-make_proper, -mp,\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define HELP 		        1
#define VERBOSE 		2
#define DICO		        3
#define IGNORE_WARNINGS		4
#define MAKE_PROPER             5
#define LAST_OPTION		MAKE_PROPER

static char	*option_tbl [] = {"",
				  "-help",
				  "v", "verbose", "nv", "noverbose",
				  "dico", "nodico",
				  "ignore_warnings", "iw",
				  "make_proper", "mp",
};

static SXINT	option_kind [] = {UNKNOWN_ARG,
				  HELP,
				  VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				  DICO, -DICO,
				  IGNORE_WARNINGS, IGNORE_WARNINGS,
				  MAKE_PROPER, MAKE_PROPER,
};

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



#if 0
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
#endif /* 0 */



static	char *
suffixname (char *pathname)
{
    char	*p;

    if ((p = sxrindex (pathname, '/')) == NULL)
	p = pathname;
    else
	p++;

    prgent_name = (char*) sxalloc (strlen (p)+1, sizeof (char));

    if ((p = sxindex (strcpy (prgent_name, p), '.')) != NULL) {
	*p = SXNUL;
	return p+1;
    }
	
    return (char*) NULL;
}

/* includes et variable pour la manipulation de la date pour modif_time */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>


static void	lfg_run (char *pathname)
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
	lfg_lex_time = time (0) /* stdin => now */ ;
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

	language_name = suffixname (pathname);
	syntax (SXBEGIN, &sxtables, infile, pathname);
	stat (sxsrcmngr.source_coord.file_name, &buf);
	lfg_lex_time = buf.st_mtime;
    }

    syntax (SXACTION, &sxtables);
    syntax (SXEND, &sxtables);
    fclose (infile);

    if (prgent_name != NULL)
      sxfree (prgent_name), prgent_name = NULL;
}

/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int argnum;
  SXINT kind;

  sxopentty ();

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    sxexit (SXSEVERITIES);
  }

  /* valeurs par defaut */
  sxverbosep = false;
  use_a_dico = true;

  /* Decodage des options */
  for (argnum = 1; argnum < argc; argnum++) {
    switch (kind = option_get_kind (argv [argnum])) {
    case HELP:
      fprintf (sxstderr, Usage, ME);
      return EXIT_SUCCESS;

    case VERBOSE:
      sxverbosep = true;
      break;

    case -VERBOSE:
      sxverbosep = false;
      break;

    case DICO:
      use_a_dico = true;
      break;

    case -DICO:
      use_a_dico = false;
      break;

    case IGNORE_WARNINGS:
      ignore_warnings = true;
      break;

    case MAKE_PROPER:
      make_proper_will_be_used = true;
      break;

    case UNKNOWN_ARG:
      if (argv [argnum] [0] == '-' && strlen (argv [argnum]) > 1) {
	/* -blabla => erreur */
	fprintf (sxstderr, "%s: invalid source pathname \"%s\".\n", ME, argv [argnum]);
	sxexit (3);
      }
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #1");
#endif
      break;
    }

    if (kind == UNKNOWN_ARG)
      break;
  }

  
   if (sxverbosep) {
     fprintf (sxtty, "%s\n", release_mess);
  }
   /*  if (sxverbosep) {
       fprintf (sxtty, "Release %s\n", SXVERSION);
       } */

  syntax (SXINIT, &sxtables, false /* no includes */);

  if (argnum == argc /* stdin (sans -) */ ||
      strcmp (argv [argnum], "-") == 0) {
    lfg_run ((char*)NULL);
  }
  else {
    /* argv [argnum] est le source */
    lfg_run (path_name = argv [argnum]);
  }

  syntax (SXFINAL, &sxtables, true);

  {
    int	severity;

    severity = sxerr_max_severity ();

    if (ignore_warnings && severity == 1)
      severity = 0;

    sxexit (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

