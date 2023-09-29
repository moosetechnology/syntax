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
/* 16-09-93 16:35 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#ifndef lint
#define WHAT	"@(#)L_main.c	- SYNTAX [unix] - Jeu 3 Mar 1994 11:55:37"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

#endif

static char	ME [] = "L_main";

#include "sxunix.h"


/* On lit a priori sur stdin, et cetera */

#ifdef unix
FILE	*sxstdout = {stdout}, *sxstderr = {stderr};
#else
FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
#endif

FILE	*sxtty;
BOOLEAN		sxverbosep = {TRUE};

/* On est dans un cas "mono-langage": */

extern struct sxtables	sxtables;

static char	Usage [] = "\
Usage:\t%s files...\n";


main (argc, argv)
    int		argc;
    char	*argv [];
{
#ifndef unix
    if (sxstdout == NULL) {
	sxstdout = stdout;
    }
    if (sxstderr == NULL) {
	sxstderr = stderr;
    }
#endif

#ifdef BUG
    /* Suppress bufferisation, in order to have proper	 */
    /* messages when something goes wrong...		 */

    setbuf (stdout, NULL);

#endif

    sxopentty ();
    sxstr_mngr (BEGIN) /* Creation de la table des chaines */ ;
    (*(sxtables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
    syntax (OPEN, &sxtables) /* Initialisation de SYNTAX (mono-langage) */ ;

    if (argc == 1) {
	/* Pas d'argument: on lit sur stdin */
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	sxsrc_mngr (INIT, stdin, "");
	sxerr_mngr (BEGIN);
	syntax (ACTION, &sxtables);
	sxsrc_mngr (FINAL);
	sxerr_mngr (END);
	/* clearerr (stdin); si utilisation repetee de "stdin" */
    }
    else {
	/* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
	register int	argnum;

	for (argnum = 1; argnum < argc; argnum++)
	    if (argv [argnum] [0] == '-' && argv [argnum] [1] == '\0') {
		if (sxverbosep) {
		    fputs ("\"stdin\":\n", sxtty);
		}

		sxsrc_mngr (INIT, stdin, "");
		sxerr_mngr (BEGIN);
		syntax (ACTION, &sxtables);
		sxsrc_mngr (FINAL);
		sxerr_mngr (END);
	    }
	    else {
		register FILE	*infile;

		if ((infile = fopen (argv [argnum], "r")) == NULL) {
		    fprintf (sxstderr, "%s: Cannot open (read) ", argv [0]);
		    sxperror (argv [argnum]);
		    sxerrmngr.nbmess [SEVERITIES - 1]++;
		}
		else {
		    if (sxverbosep) {
			fprintf (sxtty, "%s:\n", argv [argnum]);
		    }

		    sxsrc_mngr (INIT, infile, argv [argnum]);
		    sxerr_mngr (BEGIN);
		    syntax (ACTION, &sxtables);
		    sxsrc_mngr (FINAL);
		    sxerr_mngr (END);
		    fclose (infile);
		}
	    }
    }

    syntax (CLOSE, &sxtables);
    (*(sxtables.analyzers.parser)) (END) /* Inutile puisque le process va etre termine */ ;
    sxstr_mngr (END) /* Inutile puisque le process va etre termine */ ;

    {
	register int	severity;

	for (severity = SEVERITIES - 1;
	     severity > 0 && sxerrmngr.nbmess [severity] == 0;
	     severity--) {
	    /* null; */
	}

	SXEXIT (severity);
    }
}



VOID	sxvoid ()
/* procedure ne faisant rien */
{
}
