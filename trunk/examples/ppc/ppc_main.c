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

/* paragrapheur de C */

#include "sxunix.h"

char WHAT_PPCMAIN[] = "@(#)SYNTAX - $Id: ppc_main.c 3331 2023-06-04 20:24:13Z garavel $";

/* Les tables engendrees par SYNTAX */
extern struct sxtables	ppc_args_tables;

extern SXVOID ppc_scan_act (SXINT code, SXINT act_no);

#include "ppc1_td.h"
#include "ppc2_td.h"
#include "ppc3_td.h"

extern struct sxtables	ppc1_tables, ppc2_tables, ppc3_tables;

/*---------------*/
/*    options    */
/*---------------*/

struct sxtables		*ppc_tables;
SXINT	ppc_TYPE;
SXBOOLEAN		ppc_verbose;
static SXINT	ppc_file_argnum, ppc_output;
static char	ME [] = "ppc";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\n\
\t-style nnn,\n\
\t-kwlc, -key_words_lower_cased,\n\
\t-kwuc, -key_words_upper_cased,\n\
\t-kwci, -key_words_capitalised_initial,\n\
\t-kwdk, -key_words_darkened, -nkwdk, -nokey_words_darkened,\n\
\t-v, -verbose, -nv, -noverbose,\n\
\t-ll nnn, -line_length nnn,\n\
\t-tabs, -notabs,\n\
\t-every nnn,\n\
\t-mm nnn, -max_margin nnn,\n\
\t-sm, -shift_margin, -nsm, -no_shift_margin,\n\
\t-of PATH, -output_file PATH, -nof, -nooutput_file,\n\
\t-d PATH, -directory PATH,\n\
\t-T NAME, -TYPE NAME.\n";



static SXVOID	default_options (void)
{
    ppc_tables = &ppc1_tables;
    ppc_TYPE = TYPE1;
    sxverbosep = SXFALSE;
    ppc_verbose = SXFALSE;
    ppc_file_argnum = 0 /* input is stdin */ ;
    ppc_output = SXERROR_STE /* output is input */ ;
    sxppvariables.kw_case = SXNO_SPECIAL_CASE /* How should keywords be written */ ;
    sxppvariables.terminal_case = NULL /* SXNO_SPECIAL_CASE */ ;
    sxppvariables.kw_dark = SXFALSE /* keywords are not artificially darkened */ ;
    sxppvariables.terminal_dark = NULL /* Same as kw_dark, but for each type of terminal */ ;
    sxppvariables.no_tabs = SXFALSE /* optimize spaces into tabs */ ;
    sxppvariables.block_margin = SXFALSE /* preserve structure when deeply nested */ ;
    sxppvariables.line_length = 122 /* What it says */ ;
    sxppvariables.max_margin = 60 /* Do not indent lines further than that */ ;
    sxppvariables.tabs_interval = 8 /* number of columns between two tab positions */ ;
}



static SXVOID	option_file (char *dir_name)
{
    FILE	*in_file;
    char	in_name [300];

    if (dir_name != NULL && (in_file = sxfopen (strcat (strcpy (in_name, dir_name), "/.ppcrc"), "r")) != NULL) {
	sxsrc_mngr (SXINIT, in_file, in_name);
	syntax (SXACTION, &ppc_args_tables);
	sxsrc_mngr (SXFINAL);
	fclose (in_file);
    }
}



static SXBOOLEAN	in_path_permitted = SXTRUE;



static SXVOID	option_dir (char *dir)
{
    SXBOOLEAN	ipp;
    struct {
	struct sxlv	scan_lv;
	struct sxplocals	pars_lv;
	struct sxsrcmngr	src_lv;
    }	save_lv;

    ipp = in_path_permitted;
    in_path_permitted = SXFALSE;
    save_lv.scan_lv = sxsvar.sxlv;
    save_lv.pars_lv = sxplocals;
    save_lv.src_lv = sxsrcmngr;
    (*ppc_args_tables.analyzers.parser) (SXOPEN, &ppc_args_tables);
    option_file (dir);
    (*ppc_args_tables.analyzers.parser) (SXCLOSE, &ppc_args_tables);
    sxsrcmngr = save_lv.src_lv;
    sxplocals = save_lv.pars_lv;
    sxsvar.sxlv = save_lv.scan_lv;
    in_path_permitted = ipp;
}



static SXVOID	decode_options (int argc, char *argv[])
{
    int 	argn;
    FILE	*in_file;

    in_path_permitted = SXFALSE;
    option_file (getenv ("HOME"));
    option_file (".");
    in_path_permitted = SXTRUE;
    in_file = fopen (SX_DEV_NUL, "r");
    sxsrc_mngr (SXINIT, in_file, "command line args");
    sxsrcmngr.current_char = EOF;

    for (argn = argc - 1; argn > 0; argn--) {
	sxsrcmngr.source_coord.line = argn + 1;
	sxsrcmngr.source_coord.column = 1;
	sxsrcpush (SXNUL, argv [argn], sxsrcmngr.source_coord);
	sxsrcpush (SXNUL, "\n", sxsrcmngr.source_coord);
    }

    syntax (SXACTION, &ppc_args_tables);
    sxsrc_mngr (SXFINAL);
    if (in_file != NULL)
      fclose (in_file);
}




static SXVOID	ppc_run (SXINT in_ste, SXINT out_ste)
{
    FILE	*infile;
    char	*in_name;
    char	temp [20];

    switch (out_ste) {
    case SXEMPTY_STE:
	sxstdout = stdout;
	break;

    case SXERROR_STE:
	out_ste = in_ste;
	/* FALLTHROUGH */

    default:
	if ((sxstdout = sxnewfile (strcpy (temp, "/tmp/ppcXXXXXX"))) == NULL) {
	    fprintf (sxstderr, "%s: Unable to open (write) ", ME);
	    sxperror (temp);
	    sxerrmngr.nbmess [2]++;
	    return;
	}
    }

    if (*(in_name = sxstrget (in_ste)) == SXNUL) {
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	infile = stdin;
	sxsrc_mngr (SXINIT, infile, "");
    }
    else if ((infile = sxfopen (in_name, "r")) == NULL) {
	fprintf (sxstderr, "%s: Unable to open (read) ", ME);
	sxperror (in_name);
	sxerrmngr.nbmess [2]++;
	return;
    }
    else {
	if (sxverbosep) {
	    fprintf (sxtty, "%s:\n", in_name);
	}

	in_name = strcpy (sxalloc (sxstrlen (in_ste) + 1, sizeof (char)), in_name);
	sxsrc_mngr (SXINIT, infile, in_name);
    }

    syntax (SXACTION, ppc_tables);

    if (infile != stdin) {
	fclose (infile);
	sxfree (in_name);
    }

    if (sxstdout != stdout) {
	fclose (sxstdout);

	if (sxerrmngr.nbmess [2] != 0)
	    fprintf (sxstderr, "%s: The paragraphed version of %s could not be built, because global recovery was required.\n", ME, sxstrget (in_ste));
	else if (sxppvariables.is_error || sxerrmngr.nbmess [1] != 0 || !sxfile_copy (temp, sxstrget (out_ste), sxppvariables.char_count, ME)) {
	    fprintf (sxstderr, "%s: The paragraphed version of %s is in %s.\n", ME, sxstrget (in_ste), temp);
	    (sxerrmngr.nbmess [2])++;
	}
	else {
	    unlink (temp);
	}
    }

    sxsrc_mngr (SXFINAL);
}

static SXVOID	user_args_error (char *message)
{
    sxerror (SXSTACKtoken (SXSTACKnewtop ()).source_index,
	     ppc_args_tables.err_titles [2][0],
	     "%s%s %s not yet implemented.\nExecution aborted.",
	     ppc_args_tables.err_titles [2]+1,
	     message,
	     sxstrget (SXSTACKtoken (SXSTACKtop ()).string_table_entry));
    sxexit (3);
}



SXVOID	ppc_args_semact (SXINT what, SXINT which)
{
    switch (what) {
    case SXERROR:
	fprintf (sxstderr, Usage, ME);
	sxexit (3);

    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
    case SXSEMPASS:
	break;

    case SXACTION:
	switch (which) {
	case 0:
	    break;

	case 1:
	    /* <ppargs> = <-args>	 			*/
	    if (in_path_permitted) {
		ppc_output = SXEMPTY_STE;
	    }

	    break;

	case 2:
	    /* <ppargs> = <-args> <paths>			*/
	    if (in_path_permitted) {
		ppc_file_argnum = SXSTACKtoken (SXSTACKtop ()).source_index.line - 1;
	    }
	    else
		sxerror (SXSTACKtoken (SXSTACKtop ()).source_index,
			 ppc_args_tables.err_titles [2][0],
			 "%sA file name can only appear as a command line argument.",
			 ppc_args_tables.err_titles [2]+1);

	    break;

	case 3:
	    /* <-arg> = -verbose				*/
	    ppc_verbose = SXTRUE;
	    break;

	case 4:
	    /* <-arg> = -noverbose				*/
	    ppc_verbose = SXFALSE;
	    break;

	case 5:
	    /* <-arg> = -output_file <path>			*/
	    ppc_output = SXSTACKtoken (SXSTACKtop ()).string_table_entry;
	    break;

	case 6:
	    /* <-arg> = -nooutput_file				*/
	    ppc_output = SXEMPTY_STE;
	    break;

	case 7:
	    /* <-arg> = -TYPE %IDENTIFIER			*/
	    ppc_scan_act (SXACTION, 0);
	    break;

	case 8:
	    /* <-arg>	= -key_words_upper_cased		*/
	    sxppvariables.kw_case = SXUPPER_CASE;
	    break;

	case 9:
	    /* <-arg>	= -key_words_lower_cased		*/
	    sxppvariables.kw_case = SXNO_SPECIAL_CASE;
	    break;

	case 10:
	    /* <-arg>	= -key_words_capitalised_initial	*/
	    sxppvariables.kw_case = SXCAPITALISED_INITIAL;
	    break;

	case 11:
	    /* <-arg>	= -key_words_darkened			*/
	    sxppvariables.kw_dark = SXTRUE;
	    break;

	case 12:
	    /* <-arg>	= -nokey_words_darkened			*/
	    sxppvariables.kw_dark = SXFALSE;
	    break;

	case 13:
	    /* <-arg>	= -line_length %NUMBER			*/
	    sxppvariables.line_length = atol (sxstrget (SXSTACKtoken (SXSTACKtop ()).string_table_entry));
	    break;

	case 14:
	    /* <-arg>	= -tabs					*/
	    sxppvariables.no_tabs = SXFALSE;
	    break;

	case 15:
	    /* <-arg>	= -notabs				*/
	    sxppvariables.no_tabs = SXTRUE;
	    break;

	case 16:
	    /* <-arg>	= -every %NUMBER			*/
	    sxppvariables.tabs_interval = atol (sxstrget (SXSTACKtoken (SXSTACKtop ()).string_table_entry));
	    break;

	case 17:
	    /* <-arg>	= -max_margin %NUMBER			*/
	    sxppvariables.max_margin = atol (sxstrget (SXSTACKtoken (SXSTACKtop ()).string_table_entry));
	    break;

	case 18:
	    /* <-arg>	= -shift_margin				*/
	    sxppvariables.block_margin = SXFALSE;
	    break;

	case 19:
	    /* <-arg>	= -noshift_margin			*/
	    sxppvariables.block_margin = SXTRUE;
	    break;

	case 20:
	    /* <-arg>	= -directory <path>			*/
	    option_dir (sxstrget (SXSTACKtoken (SXSTACKtop ()).string_table_entry));
	    break;

	case 21:
	    /* <-arg>	= -style %NUMBER			*/
	    switch (atol (sxstrget (SXSTACKtoken (SXSTACKtop ()).string_table_entry))) {
	    case 1:
		ppc_tables = &ppc1_tables;
		ppc_TYPE = TYPE1;
		break;

	    case 2:
		ppc_tables = &ppc2_tables;
		ppc_TYPE = TYPE2;
		break;

	    case 3:
		ppc_tables = &ppc3_tables;
		ppc_TYPE = TYPE3;
		break;

	    default:
		user_args_error ("Style");
	    }

	    break;

	default:
	    user_args_error ("Option");
	}

	break;

    default:
	fputs ("The function \"ppc_args_semact\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}



int main (int argc, char *argv[])
{
    sxopentty ();
    default_options ();
    sxerr_mngr (SXBEGIN);

    /* INRIA/CONVECS: this example is quite complicated because Syntax is
     * used to parse both the options on the command-line and the C file.
     * Things would be much simpler if the former task was done using 
     * the getopt() function. Even if using Syntax to parse command-line
     * options, the complexity could be reduced by first analyzing these
     * options, then closing Syntax entirely, then re-opening Syntax from
     * scratch to analyze the C file. The increase in cost would be
     * probably negligible. Most of the current complexity comes from the
     * desire not to close Syntax entirely, but reusing its data structures
     * as much as possible.
     */

    syntax (SXINIT, &ppc_args_tables, SXFALSE /* no includes */);

    ppc_scan_act (SXBEGIN, 0);
    decode_options (argc, argv);
    syntax (SXCLOSE, &ppc_args_tables);

    if (sxerrmngr.nbmess [1] != 0 || sxerrmngr.nbmess [2] != 0) {
	ppc_args_semact (SXERROR, 0);
	/* NOTREACHED */
    }

    sxverbosep = ppc_verbose;
    syntax (SXOPEN, ppc_tables);

    if (ppc_file_argnum == 0) {
        ppc_run (SXERROR_STE, ppc_output);
    } else {
        do {
	    int	warnings, errors;

	    warnings = sxerrmngr.nbmess [1];
            sxerrmngr.nbmess [1] = 0;
            errors = sxerrmngr.nbmess [2];
            sxerrmngr.nbmess [2] = 0;
	    ppc_run (sxstrsave (argv [ppc_file_argnum++]), ppc_output);
	    sxerrmngr.nbmess [1] += warnings;
            sxerrmngr.nbmess [2] += errors;
        } while (ppc_file_argnum < (SXINT)argc);
    }

    syntax (SXFINAL, ppc_tables, SXTRUE);

    ppc_scan_act (SXEND, 0);
    sxerr_mngr (SXEND);

    sxexit (sxerr_max_severity ());
    return 0;
}

