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
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *							  *
   ******************************************************** */



/* Convertisseur de sources YACC en sources YAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20020318 14:30 (phd):	"sxtmpfile" dans "bnf_run"		*/
/************************************************************************/
/* 22-10-93 10:00 (pb):		Adaptation a OSF			*/
/************************************************************************/
/* 20-05-92 16:10 (phd):	Declaration de options_text		*/
/*				Correction de l'ecriture de la date	*/
/*				Correction d'un "char" en "int"		*/
/************************************************************************/
/* 20-05-92 15:40 (phd):	Declaration de "mktemp"			*/
/************************************************************************/
/* 18-12-87 15:00 (pb):		Warning si "nom-du-langage" trop long	*/
/************************************************************************/
/* 24-07-87 14:14 (phd):	Utilisation de "SXEXIT" partout		*/
/************************************************************************/
/* 20-11-87 15:15 (phd):	Modification du traitement de l'option	*/
/*				LANGUAGE_NAME pour permettre l'analyse	*/
/*				des eventuelles options restantes	*/
/************************************************************************/
/* 01-09-87 15:46 (phd):	Renommage des ysx_tables		*/
/************************************************************************/
/* 28-08-87 17:27 (phd):	Ajout de "ysx_list_out"			*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Utilisation de "sxopentty"		*/
/************************************************************************/
/* 27-04-87 11:00 (phd):	Creation a partir de semact_main	*/
/************************************************************************/

#include "sxunix.h"
#include "release.h"
#include "ysx_vars.h"
#include "put_edit.h"
char WHAT_YSXMAIN[] = "@(#)SYNTAX - $Id: ysx_main.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;
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

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB	((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define SOURCE 			1
#define VERBOSE 		2
#define LANGUAGE_NAME		3
#define LAST_OPTION		LANGUAGE_NAME

static char	*option_tbl [] = {"", "sc", "source", "nsc", "nosource", "v", "verbose", "nv", "noverbose", "ln",
     "language_name",};
static int	option_kind [] = {UNKNOWN_ARG, SOURCE, SOURCE, -SOURCE, -SOURCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LANGUAGE_NAME, LANGUAGE_NAME,};



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



static char	*options_text (line)
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

	    if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
}



static VOID	language_name (path_name)
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

#include "varstr.h"

static VOID	ysx_list_out ()
{
    register FILE	*listing;
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
    put_edit_nnl (9, "Listing Of:");
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
	long	date_time;

	put_edit_nnl (9, "Generated on:");
	date_time = time (0);
	put_edit (25, ctime (&date_time));
    }

    put_edit_nl (2);
    sxlisting_output (listing);
}



static VOID	ysx_run (pathname)
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
    }
    else if ((infile = fopen (pathname, "r")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	sxperror (pathname);
	sxerrmngr.nbmess [2]++;
	return;
    }
    else {
	if (sxverbosep) fprintf (sxtty, "%s:\n", pathname);
	language_name (pathname);
	sxsrc_mngr (INIT, infile, pathname);
    }

    sxerr_mngr (BEGIN);
    if (sxverbosep) fputs ("Source Analysis\n", sxtty);
    syntax (ACTION, &ysx_tables);
    sxsrc_mngr (FINAL);

    if (is_source) {
	if (sxverbosep) fputs ("Listing Output\n", sxtty);
	ysx_list_out ();
    }

    sxerr_mngr (END);
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

  options_set = OPTION (SOURCE) | OPTION (VERBOSE);
  is_source = sxverbosep = TRUE;


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

    case LANGUAGE_NAME:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a name must follow the \"%s\" option;\n\t'' is assumed.\n", ME, option_get_text (
														 LANGUAGE_NAME));
	prgentname = (char*) sxalloc (1, sizeof (char));
	prgentname [0] = NUL;
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
      fprintf (sxstderr, "%s: internal error %d while decoding options.\n", ME, option_get_kind (argv [argnum]));
      SXEXIT (3);
    }
  }

  if (!(options_set & OPTION (LANGUAGE_NAME))) {
    fprintf (sxstderr, "%s: the \"%s\" option is mandatory when no file is named;\n\t'' is assumed\n", ME,
	     option_get_text (LANGUAGE_NAME));
    prgentname = (char*) sxalloc (1, sizeof (char));
    prgentname [0] = NUL;
  }

 run:
  if (sxverbosep) {
    fprintf (sxtty, "%s\n", release_mess);
  }

  sxstr_mngr (BEGIN);
  (*(ysx_tables.analyzers.parser)) (BEGIN);
  syntax (OPEN, &ysx_tables);

  if (options_set & OPTION (LANGUAGE_NAME)) {
    ysx_run (NULL);
  }
  else {
    do {
      register int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      ysx_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (CLOSE, &ysx_tables);
  (*(ysx_tables.analyzers.parser)) (END);
  sxstr_mngr (END);

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--) {
    }

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
