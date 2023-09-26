/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 2007 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe ALPAGE.                    (pb) *
   *							  *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 08-01-08 13:30 (pb):	Ajout de cette rubrique "modifications"	        */
/************************************************************************/


#define SX_GLOBAL_VAR
#include "sxunix.h"

char WHAT_SXMAIN[] = "@(#)SYNTAX - $Id: amlgm_cmpd.c 527 2007-05-09 14:02:47Z boullier $" WHAT_DEBUG;

static char ME [] = "amlgm_cmpd.c";

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE	*sxstdout, *sxstderr;
BOOLEAN sxverbosep
#endif


extern struct sxtables	sxtables;


BOOLEAN           is_static;
char              *LANGUAGE_NAME;



/*---------------*/
/*    options    */
/*---------------*/

static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\n\
\t\t-e, -extern,\n\
\t\t--h, --help,\n\
";

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define EXTERN 		        ((SXINT)1)
#define HELP 		        ((SXINT)2)

static char	*option_tbl [] = {"", "e", "extern", "-h", "-help"};
static SXINT	option_kind [] = {UNKNOWN_ARG, EXTERN, EXTERN, HELP, HELP};



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


#if 0
static char	*option_get_text (SXINT kind)
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}
#endif

static	void
amlgm_cmpd_run (char *pathname)
{
  FILE	 *infile;
    
  sxstr_mngr (BEGIN) /* Creation de la table des chaines */ ;
  (*(sxtables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &sxtables) /* Initialisation de SYNTAX (mono-langage) */ ;

  if (pathname == NULL) {
    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }

    LANGUAGE_NAME = "<stdin>";

    sxsrc_mngr (INIT, stdin, "");
    sxerr_mngr (BEGIN);
    syntax (ACTION, &sxtables);
    sxsrc_mngr (FINAL);
    sxerr_mngr (END);
    /* clearerr (stdin); si utilisation repetee de "stdin" */
  }
  else {
    if ((infile = sxfopen (pathname, "r")) == NULL) {
      fprintf (sxstderr, "%s: Cannot open (read) ", ME);
      sxperror (pathname);
      sxerrmngr.nbmess [2]++;
      return;
    }

    if (sxverbosep) {
      fprintf (sxtty, "%s:\n", pathname);
    }
    
    LANGUAGE_NAME = pathname;

    sxsrc_mngr (INIT, infile, pathname);
    sxerr_mngr (BEGIN);
    syntax (ACTION, &sxtables);
    sxsrc_mngr (FINAL);
    sxerr_mngr (END);
    fclose (infile);
  }

  syntax (CLOSE, &sxtables);
  (*(sxtables.analyzers.parser)) (END) /* Inutile puisque le process va etre termine */ ;
  sxstr_mngr (END) /* Inutile puisque le process va etre termine */ ;
}


int main (int argc, char *argv[])
{
    sxstdout = stdout;
    sxstderr = stderr;
    sxverbosep = TRUE;

#if BUG
    /* Suppress bufferisation, in order to have proper	 */
    /* messages when something goes wrong...		 */
    setbuf (stdout, NULL);
#endif

    sxopentty ();

    /* Valeurs par defaut */
    is_static = TRUE;

    {
      /* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
      int	argnum;
      BOOLEAN   has_unknown_arg = FALSE;

      for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case EXTERN:
	  is_static = FALSE;
	  break;

	case HELP:
	  fprintf (sxstderr, "%s", Usage);
	  return EXIT_SUCCESS;	  

	case UNKNOWN_ARG:
	  has_unknown_arg = TRUE;

	  if (argnum+1 == argc && argv [argnum] [0] == '-' && argv [argnum] [1] == '\0') {
	    amlgm_cmpd_run (NULL);
	  }
	  else {
	    if (argv [argnum] [0] == '-')
	      fprintf (sxstderr, "A file name cannot start with an \"-\": ignored.\n");
	    else
	      amlgm_cmpd_run (argv [argnum]);
	  }

	  break;
	default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
	  sxtrap("make_a_dico/amlgm_cmpd_main","unknown switch case #1");
#endif
	  break;
	}
      }

      if (!has_unknown_arg)
	amlgm_cmpd_run (NULL);
    }

    return EXIT_SUCCESS;
}



VOID	sxvoid (void)
/* procedure ne faisant rien */
{
}

BOOLEAN	sxbvoid (void)
/* procedure ne faisant rien */
{
    return 0;
}
