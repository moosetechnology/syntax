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





/* Constructeur d'analyseurs lexicaux de SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20020318 14:30 (phd):	"sxtmpfile" dans "lecl_run"		*/
/************************************************************************/
/* Lun 22 Nov 1999 14:24 (pb):	Ajout de l'option dico (les mots-cles	*/
/* 				seront recherches ds un dico)		*/
/************************************************************************/
/* Lun 25 Nov 1996 11:13 (pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 21-01-94 14:39 (pb):		Caractere change de "char" en "int"	*/
/************************************************************************/
/* 22-10-93 10:00 (pb):		Adaptation a OSF			*/
/************************************************************************/
/* 27-04-93 15:35 (pb):		Traitement du non-determinisme		*/
/************************************************************************/
/* 29-04-88 16:40 (pb):		Suppression de tout ce qui touche a la	*/
/*				Perfect Hashing Function		*/
/************************************************************************/
/* 28-03-88 16:40 (phd):	Utilisation de sxindex et sxrindex	*/
/************************************************************************/
/* 24-07-87 14:14 (phd):	Utilisation de "SXEXIT" partout		*/
/************************************************************************/
/* 20-11-87 15:15 (phd):	Modification du traitement de l'option	*/
/*				LANGUAGE_NAME pour permettre l'analyse	*/
/*				des eventuelles options restantes	*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Utilisation de "sxopentty"		*/
/************************************************************************/
/* 20-03-87 18:15 (phd):	Inclusion de "release.h"		*/
/************************************************************************/
/* 06-03-87 17:07 (pb):	Suppression des options ppt, noppt et check	*/
/************************************************************************/
/* 13-02-87 16:24 (phd):	Mode verbose plus agreable		*/
/************************************************************************/
/* 06-02-87 15:52 (pb):	Suppression des options "sem" et "nosem"  	*/
/************************************************************************/
/* 28-10-86 09:14 (pb):	Ajout de cette rubrique "modifications" 	*/
/************************************************************************/

#include "sxunix.h"
#include "varstr.h"
#include "lecl_ag.h"
char WHAT_LECLMAIN[] = "@(#)SYNTAX - $Id: lecl_main.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

char	by_mess [] = "the SYNTAX lexical constructor LECL";

#include "release.h"


/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout, *sxstderr;
FILE	*sxtty;
extern struct sxtables	lecl_tables;

/*---------------*/
/*    options    */
/*---------------*/

BOOLEAN		sxverbosep;
static char	ME [] = "lecl";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-sc, -source, -nsc, -nosource,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ob, -object, -nob, -noobject\n\
\t\t-tb, -table, -ntb, -notable\n\
\t\t-ls, -list, -nls, -nolist,\n\
\t\t-d, -dico, -nd, -nodico,\n\
\t\t-ot, -optimize, -not, -nooptimize,\n\
\t\t-d, -deterministic, -nd, -nondeterministic,\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define SOURCE		  1
#define VERBOSE		  2
#define OBJECT		  3
#define TABLE		  4
#define LIST		  5
#define OPTIMIZE	  6
#define NONDETERMINISTIC  7
#define DICO		  8
#define LANGUAGE_NAME	  9
#define LAST_OPTION	  LANGUAGE_NAME


static char	*option_tbl [] = {
    "",
    "sc", "source", "nsc", "nosource",
    "v", "verbose", "nv", "noverbose",
    "ob", "object", "nob", "noobject",
    "tb", "table", "ntb", "notable",
    "ls", "list", "nls", "nolist",
    "ot", "optimize", "not", "nooptimize",
    "d", "deterministic", "nd", "nondeterministic",
    "d", "dico", "nod", "nodico",
    "ln", "language_name",};

static int	option_kind [] = {
    UNKNOWN_ARG,
    SOURCE, SOURCE, -SOURCE, -SOURCE,
    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
    OBJECT, OBJECT, -OBJECT, -OBJECT,
    TABLE, TABLE, -TABLE, -TABLE,
    LIST, LIST, -LIST, -LIST,
    OPTIMIZE, OPTIMIZE, -OPTIMIZE, -OPTIMIZE,
    -NONDETERMINISTIC, -NONDETERMINISTIC, NONDETERMINISTIC, NONDETERMINISTIC,
    DICO, DICO, -DICO, -DICO, 
    LANGUAGE_NAME, LANGUAGE_NAME,};



static int	option_get_kind (arg)
    register char	*arg;
{
    register char	**opt;

    if (*arg++ != '-')
	return UNKNOWN_ARG;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}



static char	*option_get_text (kind)
    register int	kind;
{
    register int	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}



static VOID	language_name (path_name)
    char	*path_name;
{
    register char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    prgentname = (char*) sxalloc (strlen (p)+1, sizeof (char));

    if ((p = sxindex (strcpy (prgentname, p), '.')) != NULL) {
	*p = NUL;
    }
}




static	SXVOID lecl_run (pathname)
    register char	*pathname;
{
    register FILE	*infile;

    if (pathname == NULL) {
	register int	c;

	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	if ((infile = sxtmpfile ()) == NULL) {
	    fprintf (sxstderr, "%s: Unable to create ", ME);
	    sxperror ("temp file");
	    sxerrmngr.nbmess [2]++;
	    return;
	}

	while ((c = getchar ()) != EOF) {
	    putc (c, infile);
	}

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

    sxerr_mngr (BEGIN);
    syntax (ACTION, &lecl_tables);
    sxerr_mngr (END);
    fclose (infile);
    sxsrc_mngr (FINAL);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main(int argc, char *argv[])
{
    register int	argnum;

    sxstdout = stdout, sxstderr = stderr;

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (3);
    }

    sxopentty ();
    sxttycol1p = TRUE;

/* valeurs par defaut */

    options_set = OPTION (SOURCE) | OPTION (VERBOSE) | OPTION (OBJECT) | OPTION (OPTIMIZE);
    is_source = sxverbosep = is_object = is_optimize = is_dico = TRUE;
    is_check = is_ppt = is_table = is_list = is_non_deterministic_automaton = FALSE;

/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case SOURCE:
	    is_source = TRUE, options_set |= OPTION (SOURCE);
	    break;

	case -SOURCE:
	    is_source = is_object = is_table = is_list = FALSE, options_set &= noOPTION (SOURCE) & noOPTION (OBJECT) &
		 noOPTION (TABLE) & noOPTION (LIST);
	    break;

	case VERBOSE:
	    sxverbosep = TRUE, options_set |= OPTION (VERBOSE);
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE, options_set &= noOPTION (VERBOSE);
	    break;

	case OBJECT:
	    is_source = is_object = TRUE, options_set |= OPTION (SOURCE) | OPTION (OBJECT);
	    break;

	case -OBJECT:
	    is_object = FALSE, options_set &= noOPTION (OBJECT);
	    break;

	case TABLE:
	    is_source = is_table = TRUE, options_set |= OPTION (SOURCE) | OPTION (TABLE);
	    break;

	case -TABLE:
	    is_table = FALSE, options_set &= noOPTION (TABLE);
	    break;

	case LIST:
	    is_source = is_object = is_table = is_list = TRUE, options_set |= OPTION (SOURCE) | OPTION (OBJECT) | OPTION
		 (TABLE) | OPTION (LIST);
	    break;

	case -LIST:
	    is_list = FALSE, options_set &= noOPTION (LIST);
	    break;

	case OPTIMIZE:
	    is_optimize = TRUE, options_set |= OPTION (OPTIMIZE);
	    break;

	case -OPTIMIZE:
	    is_optimize = FALSE, options_set &= noOPTION (OPTIMIZE);
	    break;

	case DICO:
	    is_dico = TRUE, options_set |= OPTION (DICO);
	    break;

	case -DICO:
	    is_dico = FALSE, options_set &= noOPTION (DICO);
	    break;

	case -NONDETERMINISTIC:
	    is_non_deterministic_automaton = FALSE, options_set &= noOPTION (NONDETERMINISTIC);
	    break;

	case NONDETERMINISTIC:
	    is_non_deterministic_automaton = TRUE, options_set |= OPTION (NONDETERMINISTIC);
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

    sxstr_mngr (BEGIN);
    (*(lecl_tables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
    syntax (OPEN, &lecl_tables) /* Initialisation de SYNTAX */ ;

    if (options_set & OPTION (LANGUAGE_NAME)) {
	lecl_run (NULL);
    }
    else {
	do {
	    register int	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    sxstr_mngr (BEGIN) /* remise a vide de la string table */ ;
	    lecl_run (argv [argnum++]);
	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    syntax (CLOSE, &lecl_tables);
    (*(lecl_tables.analyzers.parser)) (END);
    sxstr_mngr (END);

    {
	int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	SXEXIT (severity);
    }

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



char	*options_text (line)
    register char	*line;
{
    register int	i;
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
