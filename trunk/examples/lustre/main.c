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

char WHAT_LUSTRE[] = "@(#)SYNTAX - $Id: main.c 4166 2024-08-19 09:00:49Z garavel $";

/* Exemple simple d'utilisation de SYNTAX */

#include "sxunix.h"

/* On est dans un cas "mono-langage" */

extern SXTABLES	sxtables;

int main (int argc, char *argv[])
{
    syntax (SXINIT, &sxtables, false);

    if (argc == 1) {
	/* Pas d'argument: on lit sur stdin */
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	syntax (SXBEGIN, &sxtables, stdin, "");
	syntax (SXACTION, &sxtables);
	syntax (SXEND, &sxtables);
    } else {
	/* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
	int argnum;

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

		if ((infile = fopen (argv [argnum], "r")) == NULL) {
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
    return 0;
}

