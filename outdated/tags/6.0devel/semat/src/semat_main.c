/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/









/* Constructeur pour la semantique par arbres abstraits de SYNTAX */


#define SX_DFN_EXT_VAR_BNF /* pour definir les variables globales de BNF (cf bnf_vars.h) */

#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"
#include "put_edit.h"

char WHAT_SEMATMAIN[] = "@(#)SYNTAX - $Id: semat_main.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

/* These include files for date/time manipulation: */
#include <sys/types.h>
#include <sys/stat.h>


char	by_mess [] = "the SYNTAX grammar & abstract tree processor SEMAT";

#include "sxversion.h"

extern  SXINT	semat_scan_act (SXINT code, SXINT act_no);
SXINT	(*more_scan_act) (SXINT code, SXINT act_no) = {semat_scan_act};

extern SXVOID	no_tables (void), bnf_lo (void);
extern SXBOOLEAN	semat_sem (void);
extern SXVOID     semat_lo (void);
extern struct sxtables	bnf_tables;

/*---------------*/
/*    options    */
/*---------------*/

SXBOOLEAN		is_check, is_c, is_pascal;
SXINT		max_line_length;
static char	ME [] = "semat";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-sc, -source, -nsc, -nosource,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ls, -list, -nls, -nolist,\n\
\t\t-rhs nnn, -max_right_hand_side nnn,\n\
\t\t-ck, -check\n\
\t\t(-c | -pascal),\n\
\t\t-ll nnn, -max_line_length nnn,\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define SOURCE 			((SXINT)1)
#define VERBOSE 		((SXINT)2)
#define LIST 			((SXINT)3)
#define MAX_RIGHT_HAND_SIDE 	((SXINT)4)
#define CHECK	 		((SXINT)5)
#define C	 		((SXINT)6)
#define PASCAL	 		((SXINT)7)
#define MAX_LINE_LENGTH		((SXINT)8)
#define LANGUAGE_NAME		((SXINT)9)
#define LAST_OPTION		LANGUAGE_NAME

static char	*option_tbl [] = {"", "sc", "source", "nsc", "nosource", "v", "verbose", "nv", "noverbose", "ls", "list",
     "nls", "nolist", "rhs", "max_right_hand_side", "ck", "check", "c", "pascal", "ll", "max_line_length", "ln", "language_name",};
static SXINT	option_kind [] = {UNKNOWN_ARG, SOURCE, SOURCE, -SOURCE, -SOURCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LIST, LIST, -LIST, -LIST, MAX_RIGHT_HAND_SIDE, MAX_RIGHT_HAND_SIDE, CHECK, CHECK, C, PASCAL, MAX_LINE_LENGTH, MAX_LINE_LENGTH, LANGUAGE_NAME,
     LANGUAGE_NAME,};



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



static SXVOID	extract_language_name (char *path_name)
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




static SXVOID	bnf_run (char *pathname)
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
	syntax (SXBEGIN, &bnf_tables, infile, "");
	bnf_ag.bnf_modif_time = time (0) /* stdin => now */ ;
    }
    else {
	if ((infile = sxfopen (pathname, "r")) == NULL) {
	    fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	    sxperror (pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}
	else {
	    struct stat		buf /* date/time info */ ;

	    if (sxverbosep) {
		fprintf (sxtty, "%s:\n", pathname);
	    }

	    extract_language_name (pathname);
	    syntax (SXBEGIN, &bnf_tables, infile, pathname);
	    stat (sxsrcmngr.source_coord.file_name, &buf);
	    bnf_ag.bnf_modif_time = buf.st_mtime;
	}
    }

    syntax (SXACTION, &bnf_tables);

    if (!is_proper || IS_ERROR)
	no_tables ();
    else {
	if (semat_sem ()) {
	    bnf_write (&bnf_ag, prgentname);
	}
	else {
	    sxerrmngr.nbmess [2]++;
	}
    }

    bnf_lo ();
    semat_lo ();
    syntax (SXEND, &bnf_tables);
    fclose (infile);
    bnf_free (&bnf_ag);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}


/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int	argnum;

  sxopentty ();

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    SXEXIT (3);
  }

  /* valeurs par defaut */

  options_set = OPTION (SOURCE) | OPTION (VERBOSE) | OPTION (LIST);
  sxverbosep = SXFALSE;
  is_source = is_list = SXTRUE;
  is_check = SXFALSE;
  is_c = SXTRUE;
  is_pascal = SXFALSE;
  max_RHS = -1; /* Le 16/4/2002, par defaut, pas de verif de la longueur des RHS */
  max_line_length = 128;


  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case SOURCE:
      is_source = SXTRUE, options_set |= OPTION (SOURCE);
      break;

    case -SOURCE:
      is_list = is_source = SXFALSE, options_set &= noOPTION (SOURCE) & noOPTION (LIST);
      break;

    case VERBOSE:
      sxverbosep = SXTRUE, options_set |= OPTION (VERBOSE);
      break;

    case -VERBOSE:
      sxverbosep = SXFALSE, options_set &= noOPTION (VERBOSE);
      break;

    case LIST:
      is_list = is_source = SXTRUE, options_set |= OPTION (LIST) | OPTION (SOURCE);
      break;

    case -LIST:
      is_list = SXFALSE, options_set &= noOPTION (LIST);
      break;

    case MAX_RIGHT_HAND_SIDE:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &max_RHS))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-rhs\", %ld is retained.\n", ME, (long)max_RHS);
      else {
	options_set |= OPTION (MAX_RIGHT_HAND_SIDE);
      }

      break;

    case CHECK:
      is_check = SXTRUE, options_set |= OPTION (CHECK);
      break;

    case C:
      is_c = SXTRUE, is_pascal = SXFALSE, options_set |= OPTION (C), options_set &= noOPTION (PASCAL);
      break;

    case PASCAL:
      is_pascal = SXTRUE, is_c = SXFALSE, options_set |= OPTION (PASCAL), options_set &= noOPTION (C);
      break;

    case MAX_LINE_LENGTH:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &max_line_length))
	fprintf (sxstderr, "%s: a numeric argument must follow \"lls\", %ld is retained.\n", ME, (long)max_line_length);
      else {
	options_set |= OPTION (MAX_LINE_LENGTH);
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
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (long)option_get_kind (argv [argnum]));
      SXEXIT (3);
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

  syntax (SXINIT, &bnf_tables, SXFALSE /* no includes */ );

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

  syntax (SXFINAL, &bnf_tables, SXTRUE);

  SXEXIT (sxerr_max_severity ());
  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



char	*options_text (char *line)
{
    SXINT	i;
    SXBOOLEAN	is_first = SXTRUE;

    *line = SXNUL;

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = SXFALSE;
	    else
		strcat (line, ", ");

	    strcat (line, option_get_text (i));

	    if (i == MAX_RIGHT_HAND_SIDE) {
		sprintf (line + strlen (line), "=%ld", (long)max_RHS);
	    }
	    else if (i == MAX_LINE_LENGTH) {
		sprintf (line + strlen (line), "=%ld", (long)max_line_length);
	    }
	    else if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
}

