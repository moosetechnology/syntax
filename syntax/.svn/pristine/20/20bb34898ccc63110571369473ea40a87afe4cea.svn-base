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

#ifndef lint
#ifdef	BUG
#define WHAT	"@(#)sxmain.d.c	- SYNTAX [unix] - 11 Janvier 1988"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

#else
#define WHAT	"@(#)sxmain.c	- SYNTAX [unix] - 11 Janvier 1988"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

#endif
#endif


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

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--) {
	    /* null; */
	}

	SXEXIT (severity);
    }
}



VOID	sxvoid ()
/* procedure ne faisant rien */
{
}
