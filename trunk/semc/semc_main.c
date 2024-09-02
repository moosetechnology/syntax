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

/************************************************************************/
/*           Constructeur pour attributs synthetises : SEMC	        */
/************************************************************************/

#define SX_DFN_EXT_VAR_SEMC
#define SX_DFN_EXT_VAR_BNF /* pour definir les variables globales de BNF (cf bnf_vars.h) */

#include "sxversion.h"
#include "sxunix.h"
#include "B_tables.h"
#include "semc_vars.h"
#include "put_edit.h"

char WHAT_SEMC_MAIN[] = "@(#)SYNTAX - $Id: semc_main.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

/* These include files for date/time manipulation: */
#include <sys/types.h>
#include <sys/stat.h>

char	by_mess [] = "the SYNTAX attribute processor SEMC";

SXINT     (*more_scan_act) (SXINT code, SXINT act_no);

extern void	no_tables (void);
extern void	bnf_lo (void);
extern bool	tab_sem (void);
extern void	semc_lo (void);
extern void	semc_free (void);
extern SXTABLES	bnf_tables;

/*---------------*/
/*    options    */
/*---------------*/

bool		is_check;
static char	ME [] = "semc";
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

#define UNKNOWN_ARG 		((SXINT)0)
#define SOURCE 			((SXINT)1)
#define VERBOSE 		((SXINT)2)
#define LIST 			((SXINT)3)
#define TABLE_LENGTH	 	((SXINT)4)
#define CHECK	 		((SXINT)5)
#define LANGUAGE_NAME		((SXINT)6)
#define SEM_OUT		        ((SXINT)7)
#define DEFAULT		        ((SXINT)8)
#define MAX_RIGHT_HAND_SIZE 	((SXINT)9)
#define LAST_OPTION	        MAX_RIGHT_HAND_SIZE

static char	*option_tbl [] = {"", "sc", "source", "nsc", "nosource", "v", "verbose", "nv", "noverbose", "ls", "list",
     "nls", "nolist", "tbl", "table_length", "ck", "check", "ln", "language_name", "so", "sem_out", "df", "default",
     "rhs", "max_right_hand_side",};
static SXINT	option_kind [] = {UNKNOWN_ARG, SOURCE, SOURCE, -SOURCE, -SOURCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LIST, LIST, -LIST, -LIST, TABLE_LENGTH, TABLE_LENGTH, CHECK, CHECK, LANGUAGE_NAME, LANGUAGE_NAME, SEM_OUT, SEM_OUT,
     DEFAULT, DEFAULT, MAX_RIGHT_HAND_SIZE, MAX_RIGHT_HAND_SIZE,};




/*-------------------------------------------------------------------*/

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




/*-------------------------------------------------------------------*/

static char	*option_get_text (SXINT kind)
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}




/*-------------------------------------------------------------------*/

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






/*-------------------------------------------------------------------*/

static void	bnf_run (char *pathname)
{
    FILE	*infile;

    if (pathname == NULL) {
	int	c;

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

	if (sxverbosep) fprintf (sxtty, "%s:\n", pathname);
	extract_language_name (pathname);
	syntax (SXBEGIN, &bnf_tables, infile, pathname);
	stat (sxsrcmngr.source_coord.file_name, &buf);
	bnf_ag.bnf_modif_time = buf.st_mtime;
    }

    syntax (SXACTION, &bnf_tables);

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

	semc_lo ();
	semc_free ();
    }

    syntax (SXEND, &bnf_tables);
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

  sxopentty ();

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    sxexit (3);
  }

  /* valeurs par defaut */

  options_set = OPTION (SOURCE) | OPTION (VERBOSE) | OPTION (LIST);
  is_source = is_list = is_default = true;
  is_check = is_sem_out = false;
  tbl_lgth = 100;
  max_RHS = 10;


  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case SOURCE:
      is_source = true, options_set |= OPTION (SOURCE);
      break;

    case -SOURCE:
      is_source = false, options_set &= noOPTION (SOURCE);
      break;

    case VERBOSE:
      sxverbosep = true, options_set |= OPTION (VERBOSE);
      break;

    case -VERBOSE:
      sxverbosep = false, options_set &= noOPTION (VERBOSE);
      break;

    case LIST:
      is_list = true, options_set |= OPTION (LIST);
      break;

    case -LIST:
      is_list = false, options_set &= noOPTION (LIST);
      break;

    case SEM_OUT:
      is_sem_out = true, options_set |= OPTION (SEM_OUT);
      break;

    case -SEM_OUT:
      is_sem_out = false, options_set &= noOPTION (SEM_OUT);
      break;

    case DEFAULT:
      is_default = true, options_set |= OPTION (DEFAULT);
      break;

    case -DEFAULT:
      is_default = false, options_set &= noOPTION (DEFAULT);
      break;

    case TABLE_LENGTH:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &tbl_lgth))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-tbl_lgth\", value %ld is retained.\n", ME,
		 (SXINT) tbl_lgth);
      else {
	options_set |= OPTION (TABLE_LENGTH);
      }

      break;

    case MAX_RIGHT_HAND_SIZE:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &max_RHS))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-rhs\", value %ld is retained.\n", ME, (SXINT) max_RHS);
      else {
	options_set |= OPTION (MAX_RIGHT_HAND_SIZE);
      }

      break;

    case CHECK:
      is_check = true, options_set |= OPTION (CHECK);
      break;

    case LANGUAGE_NAME:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a name must follow the \"%s\" option;\n\t'' is assumed.\n", ME, option_get_text (
														 LANGUAGE_NAME));
	prgentname [0] = SXNUL;
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
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (SXINT) option_get_kind (argv [argnum]));
      sxexit (3);
    }
  }

  if (!(options_set & OPTION (LANGUAGE_NAME))) {
    fprintf (sxstderr, "%s: the \"%s\" option is mandatory when no file is named;\n\t'' is assumed\n", ME,
	     option_get_text (LANGUAGE_NAME));
    prgentname = NULL;
  }

 run:
  if (sxverbosep) {
    setbuf (stdout, NULL);
    fprintf (sxtty, "\n%s\n", release_mess);
  }

  syntax (SXINIT, &bnf_tables, false /* no includes */);

  if (options_set & OPTION (LANGUAGE_NAME)) {
    bnf_run ((char*)NULL);
  }
  else {
    do {
      int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      sxstr_mngr (SXCLEAR) /* remise a vide de la string table */ ;
      bnf_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (SXFINAL, &bnf_tables, true);

  sxexit (sxerr_max_severity ());
  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

/*-------------------------------------------------------------------*/

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

	    if (i == MAX_RIGHT_HAND_SIZE)
		sprintf (line + strlen (line), "=%ld", (SXINT) max_RHS);
	}

    return line;
}

