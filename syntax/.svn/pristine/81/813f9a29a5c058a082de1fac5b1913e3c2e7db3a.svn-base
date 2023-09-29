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


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* mercredi 7 avril 2004 (pb):	Ajout du param check_instantiable_clause*/
/************************************************************************/
/* 20020318 14:30 (phd):	"sxtmpfile" dans "srcg_run"		*/
/************************************************************************/
/* Ven  1 Jun 2001 10:38 (pb):	Ajout du param combinatorial.  	 	*/
/************************************************************************/
/* Jeu 26 Oct 2000 18:31 (pb):	Ajout du param factorize.  	 	*/
/************************************************************************/
/* Mar 19 Sep 2000 13:54 (pb):	Ajout du param 2var_form		*/
/*				Permet (si possible) de generer une srcg*/
/*				avec 2 variables par argument	 	*/
/************************************************************************/
/* Lun 26 Jun 2000 15:02 (pb):	Ajout des params 1srcg et bnf		*/
/*				Permet de generer une 1-srcg		*/
/*				Permet de (ne pas) generer une bnf	*/
/************************************************************************/
/* Jeu  6 Avr 2000 13:49 (pb):	Ajout du param OLTAG			*/
/*				Permet de generer une BNF qui tient	*/
/*				compte que la SRCG provient d'une TAG	*/
/************************************************************************/
/* 19991209 18:46 (phd):	Adaptation à la version Windows NT	*/
/************************************************************************/
/* Lun 11 Mai 1998 14:00 (pb):	Ajout du param decrease/increase_order	*/
/*				Permet de controler le sns de parcourt	*/
/*				des boucles for				*/
/************************************************************************/
/* Ven 13 Fev 1998 13:51 (pb):	Ajout du param CONTEXT_FREE_GRAMMAR	*/
/*				Permet de generer une BNF qui est un	*/
/*				sur-langage + raisonnable que T^*	*/
/************************************************************************/
/* Mar  3 Fev 1998 16:58 (pb):	Ajout du param CHECK_SELF_DEFINITION	*/
/*				Permet de ne pas tester la def d'un 	*/
/*				prdct en ft de sa negation => 		*/
/*				potentiellement dangereux		*/
/************************************************************************/
/* Jeu 18 Sep 1997 17:37 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxversion.h"
#include "sxunix.h"

#define SX_DFN_EXT_VAR_SRCG
#include "srcg.h"

/* includes et variable pour la manipulation de la date pour srcg_modif_time */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
char WHAT_SRCGMAIN[] = "@(#)SYNTAX - $Id: srcg_main.c 1112 2008-02-26 12:54:01Z rlacroix $" WHAT_DEBUG;

char	by_mess [] = "the SYNTAX processor for Synchronous Range Concatenation Grammars";

extern struct sxtables	srcg_tables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "srcg_main";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-pf pathname, -parse_file pathname\n\
\t\t-sf pathname, -synchro_file pathname\n\
\t\t-d, -direct\n\
\t\t-r, -reverse\n\
\tThe \"-ln\" option is mandatory if no file is given.\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define VERBOSE 		((SXINT)1)
#define PARSE_FILE              ((SXINT)2)
#define SYNCHRO_FILE            ((SXINT)3)
#define DIRECT_OPTION           ((SXINT)4)
#define REVERSE_OPTION          ((SXINT)5)
#define SOURCE_FILE             ((SXINT)6)
#define LAST_OPTION		SOURCE_FILE

static char	*option_tbl [] = {"",
				  "v", "verbose", "nv", "noverbose",
				  "pf", "parse_file",
				  "sf", "synchro_file",
				  "d", "direct",
				  "r", "reverse",
};
static SXINT	option_kind [] = {UNKNOWN_ARG,
				  VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				  PARSE_FILE, PARSE_FILE,
				  SYNCHRO_FILE, SYNCHRO_FILE,
				  DIRECT_OPTION, DIRECT_OPTION,
				  REVERSE_OPTION, REVERSE_OPTION,
};

static SXINT
option_get_kind (char *arg)
{
  char	**opt;

  if (*arg++ != '-')
    return SOURCE_FILE;

  for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
    if (strcmp (*opt, arg) == 0 /* egalite */ )
      break;
  }

  return option_kind [opt - option_tbl];
}



static char
*option_get_text (SXINT kind)
{
  SXINT	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}



static	char*
extract_language_name (char *path_name)
{
  char	*p;

  if ((p = sxrindex (path_name, '/')) == NULL)
    p = path_name;
  else
    p++;

  prgentname = (char*) sxalloc (strlen (p)+1, sizeof (char));

  if ((p = sxindex (strcpy (prgentname, p), '.')) != NULL) {
    *p = SXNUL;
    return p+1;
  }
	
  return (char*) NULL;
}



static void
srcg_run (char *pathname)
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
    syntax (SXBEGIN, &srcg_tables, infile, "");
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

    suffixname = extract_language_name (pathname);
    syntax (SXBEGIN, &srcg_tables, infile, pathname);
    stat (sxsrcmngr.source_coord.file_name, &buf);
  }

  syntax (SXACTION, &srcg_tables);
  syntax (SXEND, &srcg_tables);
  fclose (infile);

  if (prgentname != NULL)
    sxfree (prgentname), prgentname = NULL;
}



/************************************************************************/
/* main function */
/************************************************************************/
int
main (int argc, char *argv[])
{
  int	  argnum;

  SXBOOLEAN is_source_file = SXFALSE;

  sxopentty ();

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    SXEXIT (SXSEVERITIES);
  }

  /* valeurs par defaut */
  options_set = OPTION (VERBOSE) | OPTION (DIRECT_OPTION);
  sxverbosep = SXTRUE;
  /* La parse RCG correspond aux args de gauche du profile */
  srcg_kind = DIRECT;
  parse_file = NULL;
  synchro_file = NULL;

  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case VERBOSE:
      sxverbosep = SXTRUE, options_set |= OPTION (VERBOSE);
      break;

    case -VERBOSE:
      sxverbosep = SXFALSE, options_set &= noOPTION (VERBOSE);
      break;

    case DIRECT_OPTION:
      srcg_kind = DIRECT, options_set &= noOPTION (REVERSE_OPTION), options_set |= OPTION (DIRECT_OPTION);
      break;

    case REVERSE_OPTION:
      srcg_kind = REVERSE, options_set &= noOPTION (DIRECT_OPTION), options_set |= OPTION (REVERSE_OPTION);
      break;

    case PARSE_FILE:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (PARSE_FILE));
	SXEXIT (3);
      }
	      
      if ((parse_file = sxfopen (parse_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break;

    case SYNCHRO_FILE:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (SYNCHRO_FILE));
	SXEXIT (3);
      }
	      
      if ((synchro_file = sxfopen (synchro_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break;

    case SOURCE_FILE:
      is_source_file = SXTRUE;
      break;

    case UNKNOWN_ARG:
    default:
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (long)option_get_kind (argv [argnum]));
      SXEXIT (3);
    }

    if (is_source_file)
      break;
  }

  if (parse_file == NULL || synchro_file == NULL) {
    fprintf (sxstderr, "%s: The parse_file and synchro_file options are mandatory.n", ME);
    SXEXIT (3);
  }

  if (sxverbosep) {
    fprintf (sxtty, "%s\n", release_mess);
  }

  syntax (SXINIT, &srcg_tables, SXFALSE /* no includes */);

  if (argnum == argc) {
    srcg_run ((char*)NULL);
  }
  else {
    do {
      SXINT	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      srcg_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (SXFINAL, &srcg_tables, SXTRUE);

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
    }

  return line;
}
