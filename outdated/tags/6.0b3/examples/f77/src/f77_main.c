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
/* 20000623 17:07 (phd&pb):	Initialisation dynamique de sxstd???	*/
/************************************************************************/
/* 19-03-92 09:58 (pb):		Ajout de l'option is_ansi		*/
/************************************************************************/
/* 18-02-92 13:03 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#define SX_DFN_EXT_VAR

#include "sxunix.h"
char WHAT_F77MAIN[] = "@(#)SYNTAX - $Id: f77_main.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE  *sxstdout, *sxstderr;
FILE  *sxtty;
BOOLEAN sxverbosep;
#endif

BOOLEAN		is_ansi, is_pretty_printer;

/* On est dans un cas "mono-langage": */

extern struct sxtables	f77_tables;

extern VOID f77_src_mngr (SXINT what, FILE *infile, char *file_name);

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "f77";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ansi, -noansi.\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define ANSI 			2

static char	*option_tbl [] = {"", "v", "verbose", "nv", "noverbose", "ansi", "noansi",};
static SXINT	option_kind [] = {UNKNOWN_ARG, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     ANSI, -ANSI};



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






int main (int argc, char *argv[])
{
    int	argnum;

    sxstdout = stdout;
    sxstderr = stderr;

    sxuse (Usage);
#ifdef BUG
    /* Suppress bufferisation, in order to have proper	 */
    /* messages when something goes wrong...		 */

    setbuf (stdout, NULL);

#endif

/* valeurs par defaut */

    is_pretty_printer = FALSE;
    sxverbosep = TRUE;
    is_ansi = FALSE;

    sxopentty ();

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = TRUE;
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE;
	    break;

	case ANSI:
	    is_ansi = TRUE;
	    break;

	case -ANSI:
	    is_ansi = FALSE;
	    break;

	case UNKNOWN_ARG:
	    goto run;

	default:
	    fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (long)option_get_kind (argv [argnum]));
	    SXEXIT (3);
	}
    }


 run:
    sxstr_mngr (BEGIN) /* Creation de la table des chaines */ ;
    (*(f77_tables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
    syntax (OPEN, &f77_tables) /* Initialisation de SYNTAX (mono-langage) */ ;

    if (argnum == argc) {
	/* Pas de fichier source, on lit sur stdin */
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	sxerr_mngr (BEGIN);
	f77_src_mngr (INIT, stdin, "");
	syntax (ACTION, &f77_tables);
	f77_src_mngr (FINAL, NULL, NULL);
	sxerr_mngr (END);
	/* clearerr (stdin); si utilisation repetee de "stdin" */
    }
    else {
	/* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
	for (; argnum < argc; argnum++) {
	    if (argv [argnum] [0] == '-' && argv [argnum] [1] == '\0') {
		if (sxverbosep) {
		    fputs ("\"stdin\":\n", sxtty);
		}

		sxerr_mngr (BEGIN);
		f77_src_mngr (INIT, stdin, "");
		syntax (ACTION, &f77_tables);
		f77_src_mngr (FINAL, NULL, NULL);
		sxerr_mngr (END);
	    }
	    else {
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

		    sxerr_mngr (BEGIN);
		    f77_src_mngr (INIT, infile, argv [argnum]);
		    syntax (ACTION, &f77_tables);
		    f77_src_mngr (FINAL, NULL, NULL);
		    sxerr_mngr (END);
		    fclose (infile);
		}
	    }
	}
    }

    syntax (CLOSE, &f77_tables);
    (*(f77_tables.analyzers.parser)) (END) /* Inutile puisque le process va etre termine */ ;
    sxstr_mngr (END) /* Inutile puisque le process va etre termine */ ;

    {
	int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--) {
	    /* null; */
	}

	SXEXIT (severity);
    }
    return 0;
}



VOID	sxvoid (void)
/* procedure ne faisant rien */
{
}

