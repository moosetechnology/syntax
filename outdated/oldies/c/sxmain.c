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

char WHAT_SXMAIN[] = "@(#)sxmain.d.c	- SYNTAX [unix] - 11 Janvier 1988";

#include "sxunix.h"

/* On est dans un cas "mono-langage": */

extern SXTABLES	sxtables;



int main (int argc, char *argv [])
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
    sxstr_mngr (SXBEGIN) /* Creation de la table des chaines */ ;
    (*(sxtables.SX_parser)) (SXBEGIN, NULL /* dummy */) /* Allocation des variables globales du parser */ ;
    syntax (SXOPEN, &sxtables) /* Initialisation de SYNTAX (mono-langage) */ ;

    if (argc == 1) {
	/* Pas d'argument: on lit sur stdin */
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	sxsrc_mngr (SXINIT, stdin, "");
	sxerr_mngr (SXBEGIN);
	syntax (SXACTION, &sxtables);
	sxsrc_mngr (SXFINAL);
	sxerr_mngr (SXEND);
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

		sxsrc_mngr (SXINIT, stdin, "");
		sxerr_mngr (SXBEGIN);
		syntax (SXACTION, &sxtables);
		sxsrc_mngr (SXFINAL);
		sxerr_mngr (SXEND);
	    }
	    else {
		register FILE	*infile;

		if ((infile = fopen (argv [argnum], "r")) == NULL) {
		    fprintf (sxstderr, "%s: Cannot open (read) ", argv [0]);
		    sxperror (argv [argnum]);
		    sxerrmngr.nbmess [SXSEVERITIES - 1]++;
		}
		else {
		    if (sxverbosep) {
			fprintf (sxtty, "%s:\n", argv [argnum]);
		    }

		    sxsrc_mngr (SXINIT, infile, argv [argnum]);
		    sxerr_mngr (SXBEGIN);
		    syntax (SXACTION, &sxtables);
		    sxsrc_mngr (SXFINAL);
		    sxerr_mngr (SXEND);
		    fclose (infile);
		}
	    }
    }

    syntax (SXCLOSE, &sxtables);
    (*(sxtables.SX_parser)) (SXEND, NULL /* dummy */) /* Inutile puisque le process va etre termine */ ;
    sxstr_mngr (SXEND) /* Inutile puisque le process va etre termine */ ;

    {
	register int	severity;

	for (severity = SXSEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--) {
	    /* null; */
	}

	sxexit (severity);
    }
}


