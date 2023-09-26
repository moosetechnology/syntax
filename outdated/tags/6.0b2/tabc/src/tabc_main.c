/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
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
/*           Constructeur pour attributs synthetises : TABACT	        */
/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20070117 10:00 (pb):	        Adaptation a la 6.0		        */
/************************************************************************/
/* 20020318 14:30 (phd):	"sxtmpfile" dans "bnf_run"		*/
/************************************************************************/
/* 22-10-93 10:00 (pb):		Adaptation a OSF			*/
/************************************************************************/
/* 20-05-92 15:40 (phd):	Declaration de "mktemp"			*/
/*				Correction d'un "char" en "int"		*/
/************************************************************************/
/* 24-08-88 17:20 (bl):	ajout du #ifdef MSDOS				*/
/************************************************************************/
/* 06-06-88 14:20 (pb):	BITSTR => SXBA					*/
/************************************************************************/
/* 14-03-88 08:43 (pb):		Suppression de #include <sys/timeb.h>	*/
/************************************************************************/
/* 18-12-87 15:00 (pb):		Warning si "nom-du-langage" trop long	*/
/************************************************************************/
/* 24-07-87 14:14 (phd):	Utilisation de "SXEXIT" partout		*/
/************************************************************************/
/* 20-11-87 15:15 (phd):	Modification du traitement de l'option	*/
/*				LANGUAGE_NAME pour permettre l'analyse	*/
/*				des eventuelles options restantes	*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Utilisation de "sxopentty"		*/
/************************************************************************/
/* 20-03-87 18:15 (phd):	Inclusion de "release.h"		*/
/************************************************************************/
/* 13-02-87 14:43 (phd):	Mode verbose plus agreable		*/
/************************************************************************/
/* 29-10-86 12:00 (bl):	Demarrage a partir de semat_main.c de phd	*/
/************************************************************************/

#define SX_DFN_EXT_VAR

#include "sxunix.h"
#include "B_tables.h"
#ifdef	MSDOS
#include "tbc_vars.h"
#else
#include "tabc_vars.h"
#endif
char WHAT_TABCMAIN[] = "@(#)SYNTAX - $Id: tabc_main.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;

/* These include files for date/time manipulation: */
#include <sys/types.h>
#include <sys/stat.h>


char	by_mess [] = "the SYNTAX attribute processor TABC";

#include "release.h"

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};

#if 0
FILE	*sxstdout, *sxstderr;
FILE	*sxtty;
#endif /* 0 */

extern VOID	no_tables ();
extern VOID	bnf_lo ();
extern BOOLEAN	tab_sem ();
extern VOID	tabc_lo ();
extern VOID	tabc_free ();
extern struct sxtables	bnf_tables;

/*---------------*/
/*    options    */
/*---------------*/

BOOLEAN		is_check;
static char	ME [] = "tabc";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-sc, -source, -nsc, -nosource,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ls, -list, -nls, -nolist,\n\
\t\t-so, -sem_out, -nso, -nosem_out\n\
\t\t-df, -default, -ndf, -nodefault\n\
\t\t-rhs nnn, -max_right_hand_side nnn,\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB	((sizeof (option_tbl) / sizeof (*option_tbl)) - 1)

#define UNKNOWN_ARG 		0
#define SOURCE 			1
#define VERBOSE 		2
#define LIST 			3
#define TABLE_LENGTH	 	4
#define CHECK	 		5
#define LANGUAGE_NAME		6
#define SEM_OUT		        7
#define DEFAULT		        8
#define MAX_RIGHT_HAND_SIZE 	9
#define LAST_OPTION	        MAX_RIGHT_HAND_SIZE

static char	*option_tbl [] = {"", "sc", "source", "nsc", "nosource", "v", "verbose", "nv", "noverbose", "ls", "list",
     "nls", "nolist", "tbl", "table_length", "ck", "check", "ln", "language_name", "so", "sem_out", "df", "default",
     "rhs", "max_right_hand_side",};
static int	option_kind [] = {UNKNOWN_ARG, SOURCE, SOURCE, -SOURCE, -SOURCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LIST, LIST, -LIST, -LIST, TABLE_LENGTH, TABLE_LENGTH, CHECK, CHECK, LANGUAGE_NAME, LANGUAGE_NAME, SEM_OUT, SEM_OUT,
     DEFAULT, DEFAULT, MAX_RIGHT_HAND_SIZE, MAX_RIGHT_HAND_SIZE,};




/*-------------------------------------------------------------------*/

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




/*-------------------------------------------------------------------*/

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




/*-------------------------------------------------------------------*/

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


#if 0
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
#endif /* 0 */




/*-------------------------------------------------------------------*/

static VOID	bnf_run (pathname)
    register char	*pathname;
{
    register FILE	*infile;

    if (pathname == NULL) {
	register int	c;

	if (sxverbosep) fputs ("\"stdin\":\n", sxtty);

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

	if (sxverbosep) fprintf (sxtty, "%s:\n", pathname);
	language_name (pathname);
#if 0
	language_name (pathname, prgentname);
#endif /* 0 */
	sxsrc_mngr (INIT, infile, pathname);
	stat (sxsrcmngr.source_coord.file_name, &buf);
	bnf_ag.bnf_modif_time = buf.st_mtime;
    }

    sxerr_mngr (BEGIN);
    syntax (ACTION, &bnf_tables);
    sxsrc_mngr (FINAL);

    if (!is_proper || IS_ERROR) {
	no_tables ();
	bnf_lo ();
    }
    else {
	if (tab_sem ()) {
	    bnf_write (&bnf_ag, prgentname);
	}
	else {
	    sxerrmngr.nbmess [2]++;
	}

	tabc_lo ();
	tabc_free ();
    }

    sxerr_mngr (END);
    fclose (infile);
    bnf_free (&bnf_ag);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}




/*-------------------------------------------------------------------*/
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
    SXEXIT (3);
  }

  sxopentty ();

  /* valeurs par defaut */

  options_set = OPTION (SOURCE) | OPTION (VERBOSE) | OPTION (LIST);
  is_source = sxverbosep = is_list = is_default = TRUE;
  is_check = is_sem_out = FALSE;
  tbl_lgth = 100;
  max_RHS = 10;


  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case SOURCE:
      is_source = TRUE, options_set |= OPTION (SOURCE);
      break;

    case -SOURCE:
      is_source = FALSE, options_set &= noOPTION (SOURCE);
      break;

    case VERBOSE:
      sxverbosep = TRUE, options_set |= OPTION (VERBOSE);
      break;

    case -VERBOSE:
      sxverbosep = FALSE, options_set &= noOPTION (VERBOSE);
      break;

    case LIST:
      is_list = TRUE, options_set |= OPTION (LIST);
      break;

    case -LIST:
      is_list = FALSE, options_set &= noOPTION (LIST);
      break;

    case SEM_OUT:
      is_sem_out = TRUE, options_set |= OPTION (SEM_OUT);
      break;

    case -SEM_OUT:
      is_sem_out = FALSE, options_set &= noOPTION (SEM_OUT);
      break;

    case DEFAULT:
      is_default = TRUE, options_set |= OPTION (DEFAULT);
      break;

    case -DEFAULT:
      is_default = FALSE, options_set &= noOPTION (DEFAULT);
      break;

    case TABLE_LENGTH:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &tbl_lgth))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-tbl_lgth\", value %d is retained.\n", ME,
		 tbl_lgth);
      else {
	options_set |= OPTION (TABLE_LENGTH);
      }

      break;

    case MAX_RIGHT_HAND_SIZE:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &max_RHS))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-rhs\", value %d is retained.\n", ME, max_RHS);
      else {
	options_set |= OPTION (MAX_RIGHT_HAND_SIZE);
      }

      break;

    case CHECK:
      is_check = TRUE, options_set |= OPTION (CHECK);
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
    setbuf (stdout, NULL);
    fprintf (sxtty, "\n%s\n", release_mess);
  }

  sxstr_mngr (BEGIN);
  (*(bnf_tables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &bnf_tables) /* Initialisation de SYNTAX */ ;

  if (options_set & OPTION (LANGUAGE_NAME)) {
    bnf_run (NULL);
  }
  else {
    do {
      register int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      sxstr_mngr (BEGIN) /* remise a vide de la string table */ ;
      bnf_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (CLOSE, &bnf_tables);
  (*(bnf_tables.analyzers.parser)) (END);
  sxstr_mngr (END);

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
      ;

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}




/*-------------------------------------------------------------------*/

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

	    if (i == MAX_RIGHT_HAND_SIZE)
		sprintf (line + strlen (line), "=%d", max_RHS);
	}

    return line;
}




/*-------------------------------------------------------------------*/

VOID	sxvoid ()
/* procedure ne faisant rien */
{
}
