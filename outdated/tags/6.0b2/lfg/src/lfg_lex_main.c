/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 2004 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe ATOLL.                 	  *
   *							  *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* mardi 13 janvier 2004 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#define SX_DFN_EXT_VAR

#include "sxunix.h"
/* #include "release.h" */
char WHAT_LFGLEXMAIN[] = "@(#)SYNTAX - $Id: lfg_lex_main.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;


/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
BOOLEAN           use_a_dico, ignore_warnings, make_proper_will_be_used;
char              *path_name, *prgent_name, *language_name;
unsigned long int lfg_lex_time;

extern struct sxtables	lfg_lex_tables;


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

static int	option_kind [] = {UNKNOWN_ARG,
				  HELP,
				  VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				  DICO, -DICO,
				  IGNORE_WARNINGS, IGNORE_WARNINGS,
				  MAKE_PROPER, MAKE_PROPER,
};

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



#if 0
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
#endif /* 0 */



static	char *
suffixname ()
{
    char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    prgent_name = (char*) sxalloc (strlen (p)+1, sizeof (char));

    if ((p = sxindex (strcpy (prgent_name, p), '.')) != NULL) {
	*p = NUL;
	return p+1;
    }
	
    return (char*) NULL;
}

/* includes et variable pour la manipulation de la date pour modif_time */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>


static VOID	lfg_run (pathname)
    char	*pathname;
{
    FILE	*infile;

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
	sxsrc_mngr (INIT, infile, pathname);
	stat (sxsrcmngr.source_coord.file_name, &buf);
	lfg_lex_time = buf.st_mtime;
    }

    sxerr_mngr (BEGIN);
    syntax (ACTION, &lfg_lex_tables);
    sxsrc_mngr (FINAL);
    sxerr_mngr (END);
    fclose (infile);

    if (prgent_name != NULL)
      sxfree (prgent_name), prgent_name = NULL;
}

/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int	argnum, kind;

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }

  if (sxstderr == NULL) {
    sxstderr = stderr;
  }

#ifdef BUG
  /* Suppress bufferisation, in order to have proper	 */
  /* messages when something goes wrong...		 */
  setbuf (stdout, NULL);
#endif

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    SXEXIT (SEVERITIES);
  }

  sxopentty ();

  /* valeurs par defaut */
  sxverbosep = TRUE;
  use_a_dico = TRUE;

  /* Decodage des options */
  for (argnum = 1; argnum < argc; argnum++) {
    switch (kind = option_get_kind (argv [argnum])) {
    case HELP:
      fprintf (sxstderr, Usage, ME);
      return EXIT_SUCCESS;

    case VERBOSE:
      sxverbosep = TRUE;
      break;

    case -VERBOSE:
      sxverbosep = FALSE;
      break;

    case DICO:
      use_a_dico = TRUE;
      break;

    case -DICO:
      use_a_dico = FALSE;
      break;

    case IGNORE_WARNINGS:
      ignore_warnings = TRUE;
      break;

    case MAKE_PROPER:
      make_proper_will_be_used = TRUE;
      break;

    case UNKNOWN_ARG:
      if (argv [argnum] [0] == '-' && strlen (argv [argnum]) > 1) {
	/* -blabla => erreur */
	fprintf (sxstderr, "%s: invalid source pathname \"%s\".\n", ME, argv [argnum]);
	SXEXIT (3);
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

  
/*   if (sxverbosep) { */
/*     fprintf (sxtty, "%s\n", release_mess); */
/*   } */
  if (sxverbosep) {
    fprintf (sxtty, "Release %s\n", SXVERSION);
  }

  sxstr_mngr (BEGIN);
  (*(lfg_lex_tables.analyzers.parser)) (BEGIN);
  syntax (OPEN, &lfg_lex_tables);

  if (argnum == argc /* stdin (sans -) */ ||
      strcmp (argv [argnum], "-") == 0) {
    lfg_run (NULL);
  }
  else {
    /* argv [argnum] est le source */
    lfg_run (path_name = argv [argnum]);
  }

  syntax (CLOSE, &lfg_lex_tables);
  (*(lfg_lex_tables.analyzers.parser)) (END);
  sxstr_mngr (END);

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
      ;

    if (ignore_warnings && severity == 1)
      severity = 0;

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

