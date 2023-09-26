/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.	  *
   *							  *
   ******************************************************** */





/* Constructeur pour la semantique par arbres abstraits de SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030527 15:57 (phd):	#define SX_DFN_EXT_VAR			*/
/************************************************************************/
/* 20030520 10:37 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 21-01-94 14:26 (pb):		Caractere change de "char" en "int"	*/
/************************************************************************/
/* 20020318 14:30 (phd):	"sxtmpfile" dans "bnf_run"		*/
/************************************************************************/
/* 22-10-93 10:00 (pb):		Adaptation a OSF			*/
/************************************************************************/
/* 20-05-92 15:40 (phd):	Declaration de "mktemp"			*/
/************************************************************************/
/* 18-06-90 17:11 (pb):		Ajout de l'option "max_line_length"	*/
/*				pour genat_c, genat_pascal		*/
/************************************************************************/
/* 11-03-88 14:26 (pb):		Suppression de #include <sys/timeb.h>	*/
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
/* 23-06-87 13:55 (pb):		Options "-c" et "-pascal"		*/
/************************************************************************/
/* 27-04-87 16:19 (phd):	Modification de OPT_NB			*/
/************************************************************************/
/* 20-03-87 18:15 (phd):	Inclusion de "release.h"		*/
/************************************************************************/
/* 13-02-87 14:43 (phd):	Mode verbose plus agreable		*/
/************************************************************************/
/* 26-11-86 09:45 (phd&pb) :	Lecture sur stdin (voir bnf)	 	*/
/************************************************************************/
/* 13-11-86 14:00 (pb) :	Changement dans la grammaire:	 	*/
/*			   	suppression du %DOLLAR de fin	 	*/
/*			   	Listing appele apres la semantique	*/
/*			   	pour la sortie des numeros d'actions	*/
/*			   	Suppression de l'option stl	   	*/
/*			   	Ajout de l'option max_RHS	   	*/
/************************************************************************/
/* 31-10-86 10:38 (pb) :	Listing appele depuis smp		*/
/************************************************************************/
/* 24-10-86 10:18 (phd):	Amelioration des messages d'erreur	*/
/************************************************************************/
/* 22-10-86 09:20 (bl):	   	Modification pour la date.   		*/
/************************************************************************/
/* 21-10-86 18:02 (phd):	Unbufferisation de sxtty		*/
/************************************************************************/
/* 08-10-86 17:00 (phd):	Mise en forme pour semat.		*/
/************************************************************************/
/* 11-09-86 17:30 (phd):	Modifications de detail.		*/
/************************************************************************/
/* 28-05-86 21:00 (bl) :	Decodage des options			*/
/************************************************************************/
/* 16-05-86 17:00 (phd):	Modification des appels au "src_mngr"	*/
/************************************************************************/
/* 15-05-86 13:03 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 15-05-86 13:03 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#define SX_DFN_EXT_VAR	/* Pour définir les variables globales */
#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"
char WHAT_SEMATMAIN[] = "@(#)SYNTAX - $Id: semat_main.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;

/* These include files for date/time manipulation: */
#include <sys/types.h>
#include <sys/stat.h>

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};


char	by_mess [] = "the SYNTAX grammar & abstract tree processor SEMAT";

#include "release.h"


extern VOID	no_tables (), bnf_lo ();
extern BOOLEAN	semat_sem ();
extern VOID     semat_lo ();
extern struct sxtables	bnf_tables;

/*---------------*/
/*    options    */
/*---------------*/

BOOLEAN		is_check, is_c, is_pascal;
int		max_line_length;
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

#define UNKNOWN_ARG 		0
#define SOURCE 			1
#define VERBOSE 		2
#define LIST 			3
#define MAX_RIGHT_HAND_SIDE 	4
#define CHECK	 		5
#define C	 		6
#define PASCAL	 		7
#define MAX_LINE_LENGTH		8
#define LANGUAGE_NAME		9
#define LAST_OPTION		LANGUAGE_NAME

static char	*option_tbl [] = {"", "sc", "source", "nsc", "nosource", "v", "verbose", "nv", "noverbose", "ls", "list",
     "nls", "nolist", "rhs", "max_right_hand_side", "ck", "check", "c", "pascal", "ll", "max_line_length", "ln", "language_name",};
static int	option_kind [] = {UNKNOWN_ARG, SOURCE, SOURCE, -SOURCE, -SOURCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LIST, LIST, -LIST, -LIST, MAX_RIGHT_HAND_SIDE, MAX_RIGHT_HAND_SIDE, CHECK, CHECK, C, PASCAL, MAX_LINE_LENGTH, MAX_LINE_LENGTH, LANGUAGE_NAME,
     LANGUAGE_NAME,};



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




static VOID	bnf_run (pathname)
    register char	*pathname;
{
    register FILE	*infile;

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

	    language_name (pathname);
	    sxsrc_mngr (INIT, infile, pathname);
	    stat (sxsrcmngr.source_coord.file_name, &buf);
	    bnf_ag.bnf_modif_time = buf.st_mtime;
	}
    }

    sxerr_mngr (BEGIN);
    syntax (ACTION, &bnf_tables);
    sxsrc_mngr (FINAL);

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
    sxerr_mngr (END);
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
  is_source = sxverbosep = is_list = TRUE;
  is_check = FALSE;
  is_c = TRUE;
  is_pascal = FALSE;
  max_RHS = -1; /* Le 16/4/2002, par defaut, pas de verif de la longueur des RHS */
  max_line_length = 128;


  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case SOURCE:
      is_source = TRUE, options_set |= OPTION (SOURCE);
      break;

    case -SOURCE:
      is_list = is_source = FALSE, options_set &= noOPTION (SOURCE) & noOPTION (LIST);
      break;

    case VERBOSE:
      sxverbosep = TRUE, options_set |= OPTION (VERBOSE);
      break;

    case -VERBOSE:
      sxverbosep = FALSE, options_set &= noOPTION (VERBOSE);
      break;

    case LIST:
      is_list = is_source = TRUE, options_set |= OPTION (LIST) | OPTION (SOURCE);
      break;

    case -LIST:
      is_list = FALSE, options_set &= noOPTION (LIST);
      break;

    case MAX_RIGHT_HAND_SIDE:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &max_RHS))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-rhs\", %d is retained.\n", ME, max_RHS);
      else {
	options_set |= OPTION (MAX_RIGHT_HAND_SIDE);
      }

      break;

    case CHECK:
      is_check = TRUE, options_set |= OPTION (CHECK);
      break;

    case C:
      is_c = TRUE, is_pascal = FALSE, options_set |= OPTION (C), options_set &= noOPTION (PASCAL);
      break;

    case PASCAL:
      is_pascal = TRUE, is_c = FALSE, options_set |= OPTION (PASCAL), options_set &= noOPTION (C);
      break;

    case MAX_LINE_LENGTH:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &max_line_length))
	fprintf (sxstderr, "%s: a numeric argument must follow \"lls\", %d is retained.\n", ME, max_line_length);
      else {
	options_set |= OPTION (MAX_LINE_LENGTH);
      }

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
    setbuf (stdout, NULL);
    fprintf (sxtty, "%s\n", release_mess);
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

	    if (i == MAX_RIGHT_HAND_SIDE) {
		sprintf (line + strlen (line), "=%d", max_RHS);
	    }
	    else if (i == MAX_LINE_LENGTH) {
		sprintf (line + strlen (line), "=%d", max_line_length);
	    }
	    else if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
}



VOID	sxvoid ()
/* procedure ne faisant rien */
{
}
