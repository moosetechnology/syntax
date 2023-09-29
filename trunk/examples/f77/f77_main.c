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

char WHAT_F77MAIN[] = "@(#)SYNTAX - $Id: f77_main.c 3538 2023-08-25 07:37:12Z garavel $";

SXBOOLEAN		is_ansi, is_json, is_pretty_printer, is_input_free_fortran;

/* On est dans un cas "mono-langage": */

extern struct sxtables	f77_tables;

extern SXVOID f77_src_mngr (SXINT what, FILE *infile, char *file_name);

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "f77";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ansi, -noansi,\n\
\t\t-json, -nojson,\n\
\t\t-iff, -input_free_fortran, -niff, -noinput_free_fortran.\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define ANSI 			2
#define JSON 			3
#define INPUT_FREE_FORTRAN      4

static char	*option_tbl [] = {"", "v", "verbose", "nv", "noverbose", "ansi", "noansi", "json", "nojson",
				  "iff", "input_free_fortran", "niff", "noinput_free_fortran"};

static SXINT	option_kind [] = {UNKNOWN_ARG, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE, ANSI, -ANSI, JSON, -JSON,
				  INPUT_FREE_FORTRAN, INPUT_FREE_FORTRAN, -INPUT_FREE_FORTRAN, -INPUT_FREE_FORTRAN};

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

    sxopentty ();

    /* valeurs par defaut */

    is_json = SXFALSE;
    is_pretty_printer = SXFALSE; /* always false for f77 */
    sxverbosep = SXFALSE;
    is_ansi = SXFALSE;
    is_input_free_fortran = SXFALSE;

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = SXTRUE;
	    break;

	case -VERBOSE:
	    sxverbosep = SXFALSE;
	    break;

	case ANSI:
	    is_ansi = SXTRUE;
	    break;

	case -ANSI:
	    is_ansi = SXFALSE;
	    break;

	case JSON:
	    is_json = SXTRUE;
	    break;

	case -JSON:
	    is_json = SXFALSE;
	    break;

	case INPUT_FREE_FORTRAN:
	    is_input_free_fortran = SXTRUE;
	    break;

	case -INPUT_FREE_FORTRAN:
	    is_input_free_fortran = SXFALSE;
	    break;

	case UNKNOWN_ARG:
	    goto run;

	default:
	    fprintf (sxstderr, Usage, "f77.out");
	    fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (SXINT) option_get_kind (argv [argnum]));
	    sxexit (3);
	}
    }

 run:
    syntax (SXINIT, &f77_tables, SXFALSE /* no includes */);

    if (argnum == argc) {
	/* Pas de fichier source, on lit sur stdin */
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	/* On devrait appeler syntax (SXBEGIN, &f77_tables...) ici, mais
	 * c'est impossible car f77_src_mngr() remplace sxsrc_mngr()
	 */ 
	sxerr_mngr (SXBEGIN);
	f77_src_mngr (SXINIT, stdin, "");

	syntax (SXACTION, &f77_tables);

	/* On devrait appeler syntax (SXEND, &f77_tables) ici, mais c'est
	 * impossible car f77_src_mngr() remplace sxsrc_mngr()
	 */
	f77_src_mngr (SXFINAL, NULL, NULL);
	sxerr_mngr (SXEND);

	/* clearerr (stdin); si utilisation repetee de "stdin" */
    } else {
	/* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
	for (; argnum < argc; argnum++) {
	    if (argv [argnum] [0] == '-' && argv [argnum] [1] == '\0') {
		if (sxverbosep) {
		    fputs ("\"stdin\":\n", sxtty);
		}

		/* On devrait appeler syntax (SXBEGIN, &f77_tables...) ici,
		 * mais c'est impossible car f77_src_mngr() remplace
		 * sxsrc_mngr()
	 	 */
		sxerr_mngr (SXBEGIN);
		f77_src_mngr (SXINIT, stdin, "");

		syntax (SXACTION, &f77_tables);

		/* On devrait appeler syntax (SXEND, &f77_tables) ici, mais
		 * c'est impossible car f77_src_mngr() remplace sxsrc_mngr()
	 	 */
		f77_src_mngr (SXFINAL, NULL, NULL);
		sxerr_mngr (SXEND);
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

		    /* On devrait appeler syntax (SXBEGIN, &f77_tables...) ici,
		     * mais c'est impossible car f77_src_mngr() remplace
		     * sxsrc_mngr()
	 	     */
		    sxerr_mngr (SXBEGIN);
		    f77_src_mngr (SXINIT, infile, argv [argnum]);

		    syntax (SXACTION, &f77_tables);

		    /* On devrait appeler syntax (SXEND, &f77_tables) ici,
		     * mais c'est impossible car f77_src_mngr() remplace
		     * sxsrc_mngr()
	 	     */
		    f77_src_mngr (SXFINAL, NULL, NULL);
		    sxerr_mngr (SXEND);

		    fclose (infile);
		}
	    }
	}
    }

    syntax (SXFINAL, &f77_tables, SXTRUE);

    sxexit (sxerr_max_severity ());
    return 0;
}

