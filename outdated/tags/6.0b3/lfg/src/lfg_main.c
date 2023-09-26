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
#include "release.h"
char WHAT_LFGMAIN[] = "@(#)SYNTAX - $Id: lfg_main.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE *sxstdout, *sxstderr;
#endif

FILE              *bnf_file, *vocabulary_file;
char              *bnf_file_name, *vocabulary_file_name, *path_name, *prgent_name, *language_name;
SXUINT lfg_time;

extern struct sxtables	lfg_tables;


/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "lfg";
static char	Usage [] = "\
Usage:\t%s [options] [source_file]\n\
options=\
\t--help,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-bnf pathname,\n\
\t\t-vbl pathname, -vocabulary pathname\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define HELP 		        ((SXINT)1)
#define VERBOSE 		((SXINT)2)
#define BNF		        ((SXINT)3)
#define VOCABULARY	        ((SXINT)4)
#define LAST_OPTION		VOCABULARY

static char	*option_tbl [] = {"",
				  "-help",
				  "v", "verbose", "nv", "noverbose",
				  "bnf",
				  "vbl", "vocabulary",
};

static SXINT	option_kind [] = {UNKNOWN_ARG,
				  HELP,
				  VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				  BNF,
				  VOCABULARY, VOCABULARY,
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
	*p = NUL;
	return p+1;
    }
	
    return (char*) NULL;
}

/* includes et variable pour la manipulation de la date pour modif_time */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>


static VOID	lfg_run (char *pathname)
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
	sxsrc_mngr (INIT, infile, "");
	lfg_time = time (0) /* stdin => now */ ;
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
	lfg_time = buf.st_mtime;
    }

    sxerr_mngr (BEGIN);
    syntax (ACTION, &lfg_tables);
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
  int argnum;
  SXINT kind;

  sxstdout = stdout;
  sxstderr = stderr;

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
  bnf_file = NULL;
  vocabulary_file = NULL;

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

    case BNF:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (BNF));
	SXEXIT (3);
      }
	      
      if ((bnf_file = sxfopen (bnf_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break;

    case VOCABULARY:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (VOCABULARY));
	SXEXIT (3);
      }
	      
      if ((vocabulary_file = sxfopen (vocabulary_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

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

  
  if (sxverbosep) {
    fprintf (sxtty, "%s\n", release_mess);
  }

  sxstr_mngr (BEGIN);
  (*(lfg_tables.analyzers.parser)) (BEGIN);
  syntax (OPEN, &lfg_tables);

  if (argnum == argc /* stdin (sans -) */ ||
      strcmp (argv [argnum], "-") == 0) {
    lfg_run ((char*)NULL);
  }
  else {
    /* argv [argnum] est le source */
    lfg_run (path_name = argv [argnum]);
  }

  syntax (CLOSE, &lfg_tables);
  (*(lfg_tables.analyzers.parser)) (END);
  sxstr_mngr (END);

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
      ;

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

