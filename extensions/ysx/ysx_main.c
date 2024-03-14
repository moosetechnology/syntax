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

/* Convertisseur de sources YACC en sources YAX */

#define SX_DFN_EXT_VAR_YSX /* pour definir les variables globales de YSX (cf ysx_vars.h) */

#include "sxunix.h"
#include "sxversion.h"
#include "ysx_vars.h"
#include "put_edit.h"

char WHAT_YSXMAIN[] = "@(#)SYNTAX - $Id: ysx_main.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

extern struct sxtables	ysx_tables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "ysx";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-sc, -source, -nsc, -nosource,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	((SXINT) 1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB	((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define SOURCE 			((SXINT)1)
#define VERBOSE 		((SXINT)2)
#define LANGUAGE_NAME		((SXINT)3)
#define LAST_OPTION		LANGUAGE_NAME

static char	*option_tbl [] = {"", "sc", "source", "nsc", "nosource", "v", "verbose", "nv", "noverbose", "ln",
     "language_name",};
static SXINT	option_kind [] = {UNKNOWN_ARG, SOURCE, SOURCE, -SOURCE, -SOURCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
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



static char	*options_text (char *line)
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

	    if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
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

#include "varstr.h"

static void	ysx_list_out (void)
{
    FILE	*listing;
    VARSTR vstr;
    char	*lst_name;

    vstr = varstr_alloc (32);

    vstr = varstr_catenate (vstr, prgentname);
    vstr = varstr_catenate (vstr, ".ys.l");
    lst_name = varstr_tostr (vstr);

    if ((listing = sxfopen (lst_name, "w")) == NULL) {
      fprintf (sxstderr, "%s: cannot open (create) ", ME);
      sxperror (lst_name);
      return;
    }

    varstr_free (vstr);

    put_edit_initialize (listing);
    put_edit_nnl (9, "Listing of:");
    put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
    put_edit_nnl (9, "By:");
    put_edit (25, "The SYNTAX translator from YACC to YAX specifications, YSX");
    put_edit (25, release_mess);

    {
	char	line [132];

	put_edit_nnl (9, "Options:");
	put_edit (25, options_text (line));
    }

    {
	time_t date_time;

	put_edit_nnl (9, "Generated on:");
	date_time = time (0);
	put_edit (25, ctime (&date_time));
    }

    put_edit_nl (2);
    sxlisting_output (listing);
}



static void	ysx_run (char *pathname)
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
	syntax (SXBEGIN, &ysx_tables, infile, "");
    }
    else if ((infile = fopen (pathname, "r")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	sxperror (pathname);
	sxerrmngr.nbmess [2]++;
	return;
    }
    else {
	if (sxverbosep) fprintf (sxtty, "%s:\n", pathname);
	extract_language_name (pathname);
	syntax (SXBEGIN, &ysx_tables, infile, pathname);
    }

    if (sxverbosep) fputs ("Source Analysis\n", sxtty);
    syntax (SXACTION, &ysx_tables);

    if (is_source) {
	if (sxverbosep) fputs ("Listing Output\n", sxtty);
	ysx_list_out ();
    }

    syntax (SXEND, &ysx_tables);
    fclose (infile);

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
    sxexit (3);
  }

  /* valeurs par defaut */

  options_set = OPTION (SOURCE) | OPTION (VERBOSE);
  sxverbosep = false;
  is_source = true;


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

  syntax (SXINIT, &ysx_tables, false /* no includes */);

  if (options_set & OPTION (LANGUAGE_NAME)) {
    ysx_run ((char*)NULL);
  }
  else {
    do {
      int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      ysx_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (SXFINAL, &ysx_tables, true);

  sxexit (sxerr_max_severity ());

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
