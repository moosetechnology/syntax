/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *							  *
   ******************************************************** */




/* paragrapheur de Ada */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040112 14:10 (phd):	Adaptation à G5 (faux 64 bits)		*/
/************************************************************************/
/* 20020328 13:30 (phd):	Utilisation de sxfile_copy		*/
/************************************************************************/
/* 20020320 17:03 (phd):	Utilisation de SX_DEV_NUL		*/
/************************************************************************/
/* 20020318 17:30 (phd):	Utilisation de sxnewfile		*/
/************************************************************************/
/* 19991029 16:25 (phd):	O_TRUNC dans file_copy...		*/
/************************************************************************/
/* 19991029 17:25 (phd):	Écritures plus musclées dans file_copy	*/
/************************************************************************/
/* 06-10-99 18:45 (phd):	Déplacement d'includes de file_copy	*/
/************************************************************************/
/* Lun 25 Nov 1996 10:15(pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 22-10-93 10:00 (pb):		Adaptation a OSF			*/
/************************************************************************/
/* 21-04-88 16:12 (phd):	Test du nombre d'octets dans file_copy	*/
/************************************************************************/
/* 11-03-88 14:50 (pb):		Bug dans struct save_lv			*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Utilisation de "sxopentty"		*/
/************************************************************************/
/* 25-05-87 18:02 (phd):	Ajout de l'entree SEMPASS	   	*/
/************************************************************************/
/* 25-05-87 12:40 (phd):	Ajout d'un message d'erreur	   	*/
/************************************************************************/
/* 02-03-87 18:10 (phd):	Creation a partir de ppc_main.c	   	*/
/************************************************************************/

#define SX_DFN_EXT_VAR	/* Nécessaire sur G5... */
#include "sxunix.h"
#include "ppada_td.h"
char WHAT_PPADAMAIN[] = "@(#)SYNTAX - $Id: ppada_main.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;



/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout, *sxstderr;
FILE	*sxtty;
extern struct sxtables	ppada_args_tables, ppada_tables;

/*---------------*/
/*    options    */
/*---------------*/

BOOLEAN		sxverbosep, ppada_verbose;
static int	ppada_file_argnum, ppada_output;
static CASE	ppada_id_case;
static BOOLEAN	ppada_id_dark;
static char	ME [] = "ppada";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\n\
\t-kwlc, -key_words_lower_cased,\n\
\t-kwuc, -key_words_upper_cased,\n\
\t-kwci, -key_words_capitalised_initial,\n\
\t-kwdk, -key_words_darkened, -nkwdk, -nokey_words_darkened,\n\
\t-idlc, -identifiers_lower_cased,\n\
\t-iduc, -identifiers_upper_cased,\n\
\t-idci, -identifiers_capitalised_initial,\n\
\t-nidm, -noidentifier_modification,\n\
\t-iddk, -identifiers_darkened, -niddk, -noidentifiers_darkened,\n\
\t-v, -verbose, -nv, -noverbose,\n\
\t-ll nnn, -line_length nnn,\n\
\t-tabs, -notabs,\n\
\t-every nnn,\n\
\t-mm nnn, -max_margin nnn,\n\
\t-sm, -shift_margin, -nsm, -no_shift_margin,\n\
\t-of PATH, -output_file PATH, -nof, -nooutput_file,\n\
\t-d PATH, -directory PATH.\n";



static VOID	default_options ()
{
    sxverbosep = FALSE;
    ppada_verbose = TRUE;
    ppada_file_argnum = 0 /* input is stdin */ ;
    ppada_output = ERROR_STE /* output is input */ ;
    ppada_id_case = NO_SPECIAL_CASE /* How should identifiers be written */ ;
    ppada_id_dark = FALSE /* identifiers are not artificially darkened */ ;
    sxppvariables.kw_case = NO_SPECIAL_CASE /* How should keywords be written */ ;
    sxppvariables.terminal_case = NULL /* NO_SPECIAL_CASE */ ;
    sxppvariables.kw_dark = FALSE /* keywords are not artificially darkened */ ;
    sxppvariables.terminal_dark = NULL /* Same as kw_dark, but for each type of terminal */ ;
    sxppvariables.no_tabs = FALSE /* optimize spaces into tabs */ ;
    sxppvariables.block_margin = FALSE /* preserve structure when deeply nested */ ;
    sxppvariables.line_length = 122 /* What it says */ ;
    sxppvariables.max_margin = 60 /* Do not indent lines further than that */ ;
    sxppvariables.tabs_interval = 8 /* number of columns between two tab positions */ ;
}



static VOID	option_file (dir_name)
    char	*dir_name;
{
    register FILE	*in_file;
    char	in_name [300];

    if (dir_name != NULL && (in_file = sxfopen (strcat (strcpy (in_name, dir_name), "/.ppadarc"), "r")) != NULL) {
	sxsrc_mngr (INIT, in_file, in_name);
	syntax (ACTION, &ppada_args_tables);
	sxsrc_mngr (FINAL);
	fclose (in_file);
    }
}



static BOOLEAN	in_path_permitted = TRUE;



static VOID	option_dir (dir)
    char	*dir;
{
    register BOOLEAN	ipp;
    struct {
	struct lv	scan_lv;
	struct sxplocals	pars_lv;
	struct sxsrcmngr	src_lv;
    }	save_lv;

    ipp = in_path_permitted;
    in_path_permitted = FALSE;
    save_lv.scan_lv = sxsvar.lv;
    save_lv.pars_lv = sxplocals;
    save_lv.src_lv = sxsrcmngr;
    (*ppada_tables.analyzers.parser) (OPEN, &ppada_args_tables);
    option_file (dir);
    (*ppada_tables.analyzers.parser) (CLOSE, &ppada_args_tables);
    sxsrcmngr = save_lv.src_lv;
    sxplocals = save_lv.pars_lv;
    sxsvar.lv = save_lv.scan_lv;
    in_path_permitted = ipp;
}



static VOID	decode_options (argc, argv)
    int		argc;
    register char	*argv [];
{
    register int	argn;
    register FILE	*in_file;
    extern char		*getenv ();

    syntax (OPEN, &ppada_args_tables);
    in_path_permitted = FALSE;
    option_file (getenv ("HOME"));
    option_file (".");
    in_path_permitted = TRUE;
    sxsrc_mngr (INIT, in_file = fopen (SX_DEV_NUL, "r"), "command line args");
    sxsrcmngr.current_char = EOF;

    for (argn = argc - 1; argn > 0; argn--) {
	sxsrcmngr.source_coord.line = argn + 1;
	sxsrcmngr.source_coord.column = 1;
	sxsrcpush (NUL, argv [argn], sxsrcmngr.source_coord);
	sxsrcpush (NUL, "\n", sxsrcmngr.source_coord);
    }

    syntax (ACTION, &ppada_args_tables);
    sxsrc_mngr (FINAL);
    in_file != NULL && fclose (in_file);
    syntax (CLOSE, &ppada_args_tables);
}



static VOID	register_options ()
{
    register int	terminal;

    sxverbosep = ppada_verbose;

    if (ppada_id_case != NO_SPECIAL_CASE) {
	sxppvariables.terminal_case = sxcalloc (ADA_EOF, sizeof (CASE));
	sxppvariables.terminal_case [IDENTIFIER] = ppada_id_case;

	if (sxppvariables.kw_case != NO_SPECIAL_CASE) {
	    for (terminal = 1; terminal < ADA_EOF; terminal++) {
		if (sxkeywordp (&ppada_tables, terminal)) {
		    sxppvariables.terminal_case [terminal] = sxppvariables.kw_case;
		}
	    }
	}
    }

    if (ppada_id_dark) {
	sxppvariables.terminal_dark = sxcalloc (ADA_EOF, sizeof (BOOLEAN));
	sxppvariables.terminal_dark [IDENTIFIER] = TRUE;

	if (sxppvariables.kw_dark) {
	    for (terminal = 1; terminal < ADA_EOF; terminal++) {
		if (sxkeywordp (&ppada_tables, terminal)) {
		    sxppvariables.terminal_dark [terminal] = TRUE;
		}
	    }
	}
    }
}




static VOID	ppada_run (in_ste, out_ste)
    int		in_ste, out_ste;
{
    register FILE	*infile;
    register char	*in_name;
    char	temp [20];

    switch (out_ste) {
    case EMPTY_STE:
	sxstdout = stdout;
	break;

    case ERROR_STE:
	out_ste = in_ste;

    default:
	if ((sxstdout = sxnewfile (strcpy (temp, "/tmp/ppadaXXXXXX"))) == NULL) {
	    fprintf (sxstderr, "%s: Unable to open (write) ", ME);
	    sxperror (temp);
	    sxerrmngr.nbmess [2]++;
	    return;
	}
    }

    if (*(in_name = sxstrget (in_ste)) == NUL) {
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	sxsrc_mngr (INIT, infile = stdin, "");
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

	sxsrc_mngr (INIT, infile, in_name = strcpy (sxalloc (sxstrlen (in_ste) + 1, sizeof (char)), in_name));
    }

    syntax (ACTION, &ppada_tables);

    if (infile != stdin) {
	fclose (infile);
	sxfree (in_name);
    }

    if (sxstdout != stdout) {
	fclose (sxstdout);

	if (sxppvariables.is_error || sxerrmngr.nbmess [2] != 0 || sxerrmngr.nbmess [1] != 0 || !sxfile_copy (temp, sxstrget (out_ste), sxppvariables.char_count, ME)) {
	    fprintf (sxstderr, "%s: The paragraphed version of %s is in %s.\n", ME, sxstrget (in_ste), temp);
	    (sxerrmngr.nbmess [2])++;
	}
	else {
	    unlink (temp);
	}
    }

    sxsrc_mngr (FINAL);
}



VOID	ppada_args_semact (what, which)
    int		what;
    int		which;
{
    switch (what) {
    case ERROR:
	fprintf (sxstderr, Usage, ME);
	SXEXIT (3);

    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
    case SEMPASS:
	break;

    case ACTION:
	switch (which) {
	case 0:
	    break;

	case 1:
	    /* <ppargs> = <-args>	 			*/
	    if (in_path_permitted) {
		ppada_output = EMPTY_STE;
	    }

	    break;

	case 2:
	    /* <ppargs> = <-args> <paths>			*/
	    if (in_path_permitted) {
		ppada_file_argnum = STACKtoken (STACKtop ()).source_index.line - 1;
	    }
	    else
		sxput_error (STACKtoken (STACKtop ()).source_index,
		     "%sA file name can only appear as a command line argument.", ppada_args_tables.err_titles [2]);

	    break;

	case 3:
	    /* <-arg> = -verbose				*/
	    ppada_verbose = TRUE;
	    break;

	case 4:
	    /* <-arg> = -noverbose				*/
	    ppada_verbose = FALSE;
	    break;

	case 5:
	    /* <-arg> = -output_file <path>			*/
	    ppada_output = STACKtoken (STACKtop ()).string_table_entry;
	    break;

	case 6:
	    /* <-arg> = -nooutput_file				*/
	    ppada_output = EMPTY_STE;
	    break;

	case 8:
	    /* <-arg>	= -key_words_upper_cased		*/
	    sxppvariables.kw_case = UPPER_CASE;
	    break;

	case 9:
	    /* <-arg>	= -key_words_lower_cased		*/
	    sxppvariables.kw_case = NO_SPECIAL_CASE;
	    break;

	case 10:
	    /* <-arg>	= -key_words_capitalised_initial	*/
	    sxppvariables.kw_case = CAPITALISED_INITIAL;
	    break;

	case 11:
	    /* <-arg>	= -key_words_darkened			*/
	    sxppvariables.kw_dark = TRUE;
	    break;

	case 12:
	    /* <-arg>	= -nokey_words_darkened			*/
	    sxppvariables.kw_dark = FALSE;
	    break;

	case 13:
	    /* <-arg>	= -line_length %NUMBER			*/
	    sxppvariables.line_length = atol (sxstrget (STACKtoken (STACKtop ()).string_table_entry));
	    break;

	case 14:
	    /* <-arg>	= -tabs					*/
	    sxppvariables.no_tabs = FALSE;
	    break;

	case 15:
	    /* <-arg>	= -notabs				*/
	    sxppvariables.no_tabs = TRUE;
	    break;

	case 16:
	    /* <-arg>	= -every %NUMBER			*/
	    sxppvariables.tabs_interval = atol (sxstrget (STACKtoken (STACKtop ()).string_table_entry));
	    break;

	case 17:
	    /* <-arg>	= -max_margin %NUMBER			*/
	    sxppvariables.max_margin = atol (sxstrget (STACKtoken (STACKtop ()).string_table_entry));
	    break;

	case 18:
	    /* <-arg>	= -shift_margin				*/
	    sxppvariables.block_margin = FALSE;
	    break;

	case 19:
	    /* <-arg>	= -noshift_margin			*/
	    sxppvariables.block_margin = TRUE;
	    break;

	case 20:
	    /* <-arg>	= -directory <path>			*/
	    option_dir (sxstrget (STACKtoken (STACKtop ()).string_table_entry));
	    break;

	case 21:
	    /* <-arg>	= -identifiers_upper_cased		*/
	    ppada_id_case = UPPER_CASE;
	    break;

	case 22:
	    /* <-arg>	= -identifiers_lower_cased		*/
	    ppada_id_case = LOWER_CASE;
	    break;

	case 23:
	    /* <-arg>	= -identifiers_capitalised_initial	*/
	    ppada_id_case = CAPITALISED_INITIAL;
	    break;

	case 24:
	    /* <-arg>	= -noidentifier_modification		*/
	    ppada_id_case = NO_SPECIAL_CASE;
	    break;

	case 25:
	    /* <-arg>	= -identifiers_darkened			*/
	    ppada_id_dark = TRUE;
	    break;

	case 26:
	    /* <-arg>	= -noidentifiers_darkened		*/
	    ppada_id_dark = FALSE;
	    break;

	default:
	    sxput_error (STACKtoken (STACKnewtop ()).source_index, "%sNot yet implemented.\nExecution aborted.",
		 ppada_args_tables.err_titles [2]);
	    SXEXIT (3);
	}

	break;

    default:
	fputs ("The function \"ppada_args_semact\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}



main (argc, argv)
    int		argc;
    char	*argv [];
{
    sxstdout = stdout, sxstderr = stderr;
    sxopentty ();
    default_options ();
    sxerr_mngr (BEGIN);
    sxstr_mngr (BEGIN);
    (*ppada_tables.analyzers.parser) (BEGIN);
    ppada_scan_act (BEGIN);
    decode_options (argc, argv);


/* On y va! */

    if (sxerrmngr.nbmess [1] == 0 && sxerrmngr.nbmess [2] == 0) {
	register_options ();
	syntax (OPEN, &ppada_tables);

	if (ppada_file_argnum == 0) {
	    ppada_run (ERROR_STE, ppada_output);
	}
	else
	    do {
		register int	warnings, errors;

		warnings = sxerrmngr.nbmess [1], sxerrmngr.nbmess [1] = 0, errors = sxerrmngr.nbmess [2], sxerrmngr.
		     nbmess [2] = 0;
		ppada_run (sxstrsave (argv [ppada_file_argnum++]), ppada_output);
		sxerrmngr.nbmess [1] += warnings, sxerrmngr.nbmess [2] += errors;
	    } while (ppada_file_argnum < argc);

	syntax (CLOSE, &ppada_tables);
    }
    else
	ppada_args_semact (ERROR, 0);

    ppada_scan_act (END);
    (*ppada_tables.analyzers.parser) (END);
    sxstr_mngr (END);
    sxerr_mngr (END);

    if (sxppvariables.terminal_case != NULL) {
	sxfree (sxppvariables.terminal_case), sxppvariables.terminal_case = NULL;
    }

    if (sxppvariables.terminal_dark != NULL) {
	sxfree (sxppvariables.terminal_dark), sxppvariables.terminal_dark = NULL;
    }

    {
	register int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	SXEXIT (severity);
    }
}
