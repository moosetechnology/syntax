/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *							  *
   ******************************************************** */





/* Exemple simple d'utilisation de SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20060314 16:55 (pb):	Le dico peut etre statique ou externe		*/
/************************************************************************/
/* 20040112 14:10 (phd):	Adaptation à G5 (faux 64 bits)		*/
/************************************************************************/
/* 20030515 14:43 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 20000623 17:07 (phd&pb):	Initialisation dynamique de sxstd???	*/
/*				sur UNIX aussi				*/
/************************************************************************/
/* 11-01-88 14:44 (phd&pb):	Initialisation dynamique de sxstd???	*/
/*				sur tout systeme autre que UNIX		*/
/************************************************************************/
/* 24-07-87 14:14 (phd):	Utilisation de "SXEXIT" partout		*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Utilisation de "sxopentty"		*/
/************************************************************************/
/* 13-02-87 14:43 (phd):	Mode verbose plus agreable		*/
/************************************************************************/
/* 17-10-86 15:21 (phd):	Appels a sxerr_mmngr			*/
/************************************************************************/
/* 23-10-86 13:28 (phd):	Amelioration des messages d'erreur	*/
/************************************************************************/
/* 17-10-86 17:20 (phd):	Ajout de sorties en mode verbeux	*/
/************************************************************************/
/* 17-10-86 15:13 (phd):	Ajout du fichier sxtty			*/
/************************************************************************/
/* 22-05-86 12:10 (phd):	Modification des appels au "str_mngr"	*/
/*				"unbufferisation" de "stdout" si "BUG"	*/
/************************************************************************/
/* 16-05-86 17:00 (phd):	Modification des appels au "src_mngr"	*/
/************************************************************************/
/* 15-05-86 13:03 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 15-05-86 13:03 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#define SX_DFN_EXT_VAR	/* Nécessaire sur G5... */
#include "sxunix.h"
#include "sxdico.h"
char WHAT_DICOMAIN[] = "@(#)SYNTAX - $Id: dico_main.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE	*sxstdout, *sxstderr;
#endif

SXINT             optim_kind, process_kind, print_on_sxtty;
char            *dico_name, *prgentname;
BOOLEAN         static_kind, extern_kind;

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

extern struct sxtables	sxtables;

static SXVOID	language_name (char *path_name)
{
    char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    prgentname = (char*) sxalloc (strlen (p)+1, sizeof (char));

    if ((p = sxindex (strcpy (prgentname, p), '.')) != NULL) {
	*p = NUL;
    }
}


/************************************************************************/
/* main function */
/************************************************************************/
int main(int argc, char *argv[])
{
  int	argnum;
  SXINT kind;

  sxstdout = stdout;
  sxstderr = stderr;

#ifdef BUG
  /* Suppress bufferisation, in order to have proper	 */
  /* messages when something goes wrong...		 */

  setbuf (stdout, NULL);

#endif


  /* valeurs par defaut */
  optim_kind = SPACE; /* priorite a l'espace */
  process_kind = TOTAL; /* Les mots sont ranges en entier ds le dico */
  sxverbosep = FALSE; /* Les phases du traitement restent silencieuses */
  print_on_sxtty = 0; /* Pas d'animation au cours de la construction du dico */
  static_kind = TRUE; /* Le dico est static */
  extern_kind = FALSE; /* Le dico est externe */

  /* Decodage des options */
  for (argnum = 1; argnum < argc; argnum++) {
    switch (kind = option_get_kind (argv [argnum])) {
    case HELP:
      fprintf (sxstderr, Usage, ME);
      return EXIT_SUCCESS;

    case VERBOSE:
      sxverbosep = TRUE;
      print_on_sxtty = 1;
      break;

    case -VERBOSE:
      sxverbosep = FALSE;
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
	SXEXIT (3);
      }
	     
      dico_name = argv [argnum];

      break;

    case STATIC_opt:
      static_kind = TRUE;
      extern_kind = FALSE;
      break;

    case EXTERN_opt:
      extern_kind = TRUE;
      static_kind = FALSE;
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

  sxopentty ();
  sxstr_mngr (BEGIN) /* Creation de la table des chaines */ ;
  (*(sxtables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &sxtables) /* Initialisation de SYNTAX (mono-langage) */ ;

  if (argnum == argc /* stdin (sans -) */ ||
      strcmp (argv [argnum], "-") == 0) {
    /* on lit sur stdin */
    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }

    prgentname = (char*) sxalloc (6, sizeof (char));
    strcpy (prgentname, "stdin");
	
    sxsrc_mngr (INIT, stdin, "");
    sxerr_mngr (BEGIN);
    syntax (ACTION, &sxtables);
    sxsrc_mngr (FINAL);
    sxerr_mngr (END);
  }
  else {
    /* Tous les arguments sont des noms de fichiers */
    while (argnum < argc) {
      FILE	*infile;

      if ((infile = sxfopen (argv [argnum], "r")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (read) ", argv [0]);
	sxperror (argv [argnum]);
	sxerrmngr.nbmess [SEVERITIES - 1]++;
      }
      else {
	if (sxverbosep) {
	  fprintf (sxtty, "%s:\n", argv [argnum]);
	}

	language_name (argv [argnum]);
	sxsrc_mngr (INIT, infile, argv [argnum]);
	sxerr_mngr (BEGIN);
	syntax (ACTION, &sxtables);
	sxsrc_mngr (FINAL);
	sxerr_mngr (END);
	fclose (infile);
      }

      argnum++;
    }
  }

  syntax (CLOSE, &sxtables);
  (*(sxtables.analyzers.parser)) (END) /* Inutile puisque le process va etre termine */ ;
  sxstr_mngr (END) /* Inutile puisque le process va etre termine */ ;

  if (prgentname != NULL)
    sxfree (prgentname), prgentname = NULL;

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--) {
      /* null; */
    }

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



VOID	sxvoid (void)
/* procedure ne faisant rien */
{
}
