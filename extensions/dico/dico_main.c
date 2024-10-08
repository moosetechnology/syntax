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

/* Exemple simple d'utilisation de SYNTAX */

#include "sxversion.h"
#include "sxunix.h"
#include "sxdico.h"

char WHAT_DICOMAIN[] = "@(#)SYNTAX - $Id: dico_main.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

SXINT             optim_kind, process_kind, print_on_sxtty;
char            *dico_name, *prgentname;
bool         static_kind, extern_kind;

/*---------------*/
/*    options    */
/*---------------*/
static char	ME [] = "dico";
static char	Usage [] = "\
Usage:\t%s [options] [source_file]\n\
options=\
\t--help,\n\
\t\t-v, -verbose, -nv (default), -noverbose,\n\
\t\t-p, -partiel,\n\
\t\t-t, -total (default),\n\
\t\t-ti, -time,\n\
\t\t-sp, -space (default),\n\
\t\t-pb, -prefix_bound,\n\
\t\t-sb, -suffix_bound,\n\
\t\t-n dico_name, -name dico_name,\n\
\t\t-s, -static (default),\n\
\t\t-e, -extern,\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define HELP 		        ((SXINT)1)
#define VERBOSE 		((SXINT)2)
#define PARTIEL_opt             ((SXINT)3)
#define TOTAL_opt               ((SXINT)4)
#define TIME_opt                ((SXINT)5)
#define SPACE_opt               ((SXINT)6)
#define PREFIX_BOUND_opt        ((SXINT)7)
#define SUFFIX_BOUND_opt        ((SXINT)8)
#define DICO_NAME               ((SXINT)9)
#define STATIC_opt              ((SXINT)10)
#define EXTERN_opt              ((SXINT)11)
#define LAST_OPTION		EXTERN_opt

static char	*option_tbl [] = {"",
				  "-help",
				  "v", "verbose", "nv", "noverbose",
				  "p", "partiel",
				  "t", "total",
				  "ti", "time",
				  "sp", "space",
				  "pb", "prefix_bound",
				  "sb", "suffix_bound",
				  "n", "name",
                                  "s", "static",
                                  "e", "extern",
};

static SXINT	option_kind [] = {UNKNOWN_ARG,
				  HELP,
				  VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				  PARTIEL_opt, PARTIEL_opt,
				  TOTAL_opt, TOTAL_opt,
				  TIME_opt, TIME_opt,
				  SPACE_opt, SPACE_opt,
				  PREFIX_BOUND_opt, PREFIX_BOUND_opt,
				  SUFFIX_BOUND_opt, SUFFIX_BOUND_opt,
				  DICO_NAME, DICO_NAME,
				  STATIC_opt, STATIC_opt,
				  EXTERN_opt, EXTERN_opt,
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


static char	*option_get_text (SXINT kind)
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}


/* On est dans un cas "mono-langage": */

extern SXTABLES	sxtables;

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


/************************************************************************/
/* main function */
/************************************************************************/
int main(int argc, char *argv[])
{
  int	argnum;
  SXINT kind;

  sxopentty ();

  /* valeurs par defaut */
  optim_kind = SPACE; /* priorite a l'espace */
  process_kind = TOTAL; /* Les mots sont ranges en entier ds le dico */
  sxverbosep = false; /* Les phases du traitement restent silencieuses */
  print_on_sxtty = 0; /* Pas d'animation au cours de la construction du dico */
  static_kind = true; /* Le dico est static */
  extern_kind = false; /* Le dico est externe */

  /* Decodage des options */
  for (argnum = 1; argnum < argc; argnum++) {
    switch (kind = option_get_kind (argv [argnum])) {
    case HELP:
      fprintf (sxstderr, Usage, ME);
      return EXIT_SUCCESS;

    case VERBOSE:
      sxverbosep = true;
      print_on_sxtty = 1;
      break;

    case -VERBOSE:
      sxverbosep = false;
      break;

    case PARTIEL_opt:
      process_kind = PARTIEL;
      break;

    case TOTAL_opt:
      process_kind = TOTAL;
      break;

    case TIME_opt:
      optim_kind = TIME;
      break;

    case SPACE_opt:
      optim_kind = SPACE;
      break;

    case PREFIX_BOUND_opt:
      optim_kind = PREFIX_BOUND;
      break;

    case SUFFIX_BOUND_opt:
      optim_kind = SUFFIX_BOUND;
      break;

    case DICO_NAME:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a dictionnary name must follow the \"%s\" option;\n", ME, option_get_text (DICO_NAME));
	sxexit (3);
      }
	     
      dico_name = argv [argnum];

      break;

    case STATIC_opt:
      static_kind = true;
      extern_kind = false;
      break;

    case EXTERN_opt:
      extern_kind = true;
      static_kind = false;
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

  syntax (SXINIT, &sxtables, false /* no includes */);

  if (argnum == argc /* stdin (sans -) */ ||
      strcmp (argv [argnum], "-") == 0) {
    /* on lit sur stdin */
    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }

    prgentname = (char*) sxalloc (6, sizeof (char));
    strcpy (prgentname, "stdin");
	
    syntax (SXBEGIN, &sxtables, stdin, "");
    syntax (SXACTION, &sxtables);
    syntax (SXEND, &sxtables);
  }
  else {
    /* Tous les arguments sont des noms de fichiers */
    while (argnum < argc) {
      FILE	*infile;

      if ((infile = sxfopen (argv [argnum], "r")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (read) ", argv [0]);
	sxperror (argv [argnum]);
	sxerrmngr.nbmess [SXSEVERITIES - 1]++;
      }
      else {
	if (sxverbosep) {
	  fprintf (sxtty, "%s:\n", argv [argnum]);
	}

	extract_language_name (argv [argnum]);
	syntax (SXBEGIN, &sxtables, infile, argv [argnum]);
	syntax (SXACTION, &sxtables);
	syntax (SXEND, &sxtables);
	fclose (infile);
      }

      argnum++;
    }
  }

  syntax (SXFINAL, &sxtables, true);

  if (prgentname != NULL)
    sxfree (prgentname), prgentname = NULL;

  sxexit (sxerr_max_severity ());
  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

