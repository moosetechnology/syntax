/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.	  *
   *							  *
   ******************************************************** */





/* Constructeur d'analyseurs lexicaux dynamiques de SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030428 14:52 (pb&phd):	Suppression de _SXBA_TARGET_SIZE	*/
/************************************************************************/
/* 31-05-94 11:55 (pb):	Ajout de cette rubrique "modifications" 	*/
/************************************************************************/

#define SX_DFN_EXT_VAR_LECL

#include "sxdynam_scanner.h"
char WHAT_LECLDMAIN[] = "@(#)SYNTAX - $Id: lecld_main.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

char	by_mess [] = "the SYNTAX dynamic lexical constructor LECLD";

#include "release.h"

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
FILE *sxstdout, *sxstderr, *sxtty;
BOOLEAN sxverbosep;
#endif

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "lecld";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

static long	options_set;


static char	*prgentname;

static struct sxdynam_scanner	re_header;

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	((SXINT)0)
#define VERBOSE 	((SXINT)1)
#define LANGUAGE_NAME	((SXINT)2)
#define LAST_OPTION	  LANGUAGE_NAME


static char	*option_tbl [] = {
    "",
    "v", "verbose", "nv", "noverbose",
    "ln", "language_name",};

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
    LANGUAGE_NAME, LANGUAGE_NAME,};



static SXINT	option_get_kind (arg)
    char	*arg;
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



static char	*option_get_text (kind)
    SXINT	kind;
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}



static VOID	language_name (path_name)
    char	*path_name;
{
    char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    prgentname = (char*) sxalloc (strlen (p)+1, sizeof (char));

    if ((p = sxindex (strcpy (prgentname, p), '.')) != NULL) {
	*p = NUL;
    }
}


extern char 	*mktemp ();

static SXVOID	lecld_run (pathname)
    char	*pathname;
{

    FILE	*infile;

    if (pathname == NULL) {
	char	pathname [32];
	int	c;

	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	if ((infile = sxfopen (mktemp (strcpy (pathname, "/tmp/lecldXXXXXX")), "w")) == NULL) {
	    fprintf (sxstderr, "%s: Unable to create temp file:\t", ME);
lost:	    sxperror (pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}

	while ((c = getchar ()) != EOF) {
	    putc (c, infile);
	}

	if ((infile = sxfreopen (pathname, "r", infile)) == NULL) {
	    fprintf (sxstderr, "%s: Unable to reopen temp file:\t", ME);
	    goto lost;
	}

	unlink (pathname) /* vanishes at once, dies when closed */ ;
	rewind (infile);
	sxsrc_mngr (INIT, infile, "");
    }
    else {
	if ((infile = sxfopen (pathname, "r")) == NULL) {
	    fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	    sxperror (pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}
	else {
	    if (sxverbosep) {
		fprintf (sxtty, "%s:\n", pathname);
	    }

	    language_name (pathname);
	    sxsrc_mngr (INIT, infile, pathname);
	}
    }

    sxstr_mngr (BEGIN);
    sxerr_mngr (BEGIN);

    sxre_alloc ();
    sxre_doit (infile, pathname == NULL ? "" : pathname);
    fclose (infile);

    if (!SXDS->is_smp_error)
    {
#include "B_tables.h"

	SXINT		t_code, re_no;
	filedesc_t	file_descr /* file descriptor */ ;
	char	file_name [128];

	struct bnf_ag_item	bnf_ag;
	
	if (!bnf_read (&bnf_ag, prgentname))
	    sxexit ();

	SXDS->scanact = NULL;	/* Pas d'action lexicale */
	SXDS->recovery = NULL;	/* Pour l'instant */
	SXDS->desambig = NULL;	/* Pour l'instant */

	sxdynam_scanner (OPEN);
	    
	if (SXDS->scanact != NULL)
	    (*(SXDS->scanact)) (OPEN);

	sxdynam_scanner (INIT);
	    
	if (SXDS->scanact != NULL)
	    (*(SXDS->scanact)) (INIT);

	/* Traitement de eof */
	t_code = -bnf_ag.WS_TBL_SIZE.tmax;
	sxre_process_eof (t_code);

	while (--t_code >= 1)
	{
	    re_no = sxre_string_to_re (bnf_ag.T_STRING + bnf_ag.ADR [-t_code],
				       t_code,
				       SXBA_bit_is_set (bnf_ag.IS_A_GENERIC_TERMINAL, t_code));

	    if (re_no == ERROR_STE)
		sxexit ();
	}

	/* ".ds" means ".dynamic_scanner" */
	if ((file_descr = creat (strcat (strcpy (file_name, prgentname), ".ds"), 0666)) < 0) {
	    fprintf (sxstderr, "%s: cannot create ", ME);
	    sxperror (file_name);
	    sxexit ();
	}

	if (!sxre_write (file_descr)) {
	    fprintf (sxstderr, "%s: cannot write ", ME);
	    sxperror (file_name);
	    sxexit ();
	}

	close (file_descr);

	if (SXDS->scanact != NULL)
	    (*(SXDS->scanact)) (FINAL);

	sxdynam_scanner (FINAL);

	if (SXDS->scanact != NULL)
	    (*(SXDS->scanact)) (CLOSE);

	sxdynam_scanner (CLOSE);
	bnf_free (&bnf_ag);
    }

    sxre_free ();

    sxerr_mngr (END);
    sxstr_mngr (END);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main(int argc, char *argv[])
{
    int 	argnum;

    sxstdout = stdout;
    sxstderr = stderr;

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (3);
    }

    sxopentty ();

/* valeurs par defaut */

    options_set = OPTION (VERBOSE);
    sxverbosep = TRUE;

/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = TRUE, options_set |= OPTION (VERBOSE);
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE, options_set &= noOPTION (VERBOSE);
	    break;

	case LANGUAGE_NAME:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: a name must follow the \"%s\" option;\n\t'' is assumed.\n", ME, option_get_text (
		     LANGUAGE_NAME));
		prgentname = (char*) sxalloc (1, sizeof (char));
		prgentname [0] = NUL;
	    }
	    else {
	        prgentname = (char*) sxalloc (strlen (argv [argnum])+1, sizeof (char));
		strcpy (prgentname, argv [argnum]);
	    }

	    options_set |= OPTION (LANGUAGE_NAME);
	    break;

	case UNKNOWN_ARG:
	    if (options_set & OPTION (LANGUAGE_NAME)) {
		fprintf (sxstderr, "%s: the \"%s\" option must not be used when naming files.\n", ME, option_get_text (
		     LANGUAGE_NAME));
		options_set &= noOPTION (LANGUAGE_NAME);
	    }

	    goto run;

	default:
	    fprintf (sxstderr, "%s: internal error %d while decoding options.\n", ME, option_get_kind (argv [argnum]));
	    break;
	}
    }

    if (!(options_set & OPTION (LANGUAGE_NAME))) {
	fprintf (sxstderr, "%s: the \"%s\" option is mandatory when no file is named;\n\t'' is assumed\n", ME,
	     option_get_text (LANGUAGE_NAME));
	prgentname = (char*) sxalloc (1, sizeof (char));
	prgentname [0] = NUL;
    }

run:
    if (sxverbosep) {
	setbuf (stdout, NULL);
	fprintf (sxtty, "%s\n", release_mess);
    }

    SXDS = &(re_header);

    if (options_set & OPTION (LANGUAGE_NAME)) {
	lecld_run ((char*)NULL);
    }
    else {
	do {
	    int	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    lecld_run (argv [argnum++]);
	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    {
	int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	SXEXIT (severity);
    }

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



char	*options_text (line)
    char	*line;
{
    SXINT	i;
    BOOLEAN	is_first = TRUE;

    *line = NUL;

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = FALSE;
	    else
		strcat (line, ", ");

	    strcat (line, option_get_text (i));
	}

    return line;
}



VOID	sxvoid ()
/* procedure ne faisant rien */
{
}
