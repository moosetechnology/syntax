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

/* Exemple simple d'utilisation de SYNTAX */

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXMAIN[] = "@(#)SYNTAX - $Id: sxmain.c 3948 2024-05-02 07:51:23Z garavel $" WHAT_DEBUG;

/* On est dans un cas "mono-langage": */

extern struct sxtables	sxtables;

int main(int argc, char *argv[])
{
    syntax (SXINIT, &sxtables, false /* no include manager */);

    if (argc == 1) {
	/* Pas d'argument: on lit sur stdin */
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	syntax (SXBEGIN, &sxtables, stdin, "");
	syntax (SXACTION, &sxtables);
	syntax (SXEND, &sxtables);
	/* clearerr (stdin); si utilisation repetee de "stdin" */
    } else {
	/* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
	int	argnum;

	for (argnum = 1; argnum < argc; argnum++)
	    if (argv [argnum] [0] == '-' && argv [argnum] [1] == '\0') {
		if (sxverbosep) {
		    fputs ("\"stdin\":\n", sxtty);
		}

		syntax (SXBEGIN, &sxtables, stdin, "");
		syntax (SXACTION, &sxtables);
		syntax (SXEND, &sxtables);
	    } else {
		FILE	*infile;

		if ((infile = sxfopen (argv [argnum], "r")) == NULL) {
		    fprintf (sxstderr, "%s: Cannot open (read) ", argv [0]);
		    sxperror (argv [argnum]);
		    sxerrmngr.nbmess [SXSEVERITIES - 1]++;
		} else {
		    if (sxverbosep) {
			fprintf (sxtty, "%s:\n", argv [argnum]);
		    }

		    syntax (SXBEGIN, &sxtables, infile, argv [argnum]);
		    syntax (SXACTION, &sxtables);
		    syntax (SXEND, &sxtables);
		    fclose (infile);
		}
	    }
    }

    syntax (SXFINAL, &sxtables, true);

    sxexit (sxerr_max_severity ());

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

