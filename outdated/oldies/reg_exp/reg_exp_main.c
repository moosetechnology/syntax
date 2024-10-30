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
/* 11-04-94 17:46 (pb):	Ajout de cette rubrique "modifications" 	*/
/************************************************************************/


char WHAT_REGEXP_MAIN[] = "@(#)reg_exp_main.c\t- SYNTAX [unix] - Jeu 26 Mai 1994 11:17:34";

#include "sxunix.h"
#include "sxdynam_scanner.h"


static struct sxdynam_scanner	re_header;


/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "reg_exp";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-sc, -source, -nsc, -nosource,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ob, -object, -nob, -noobject\n\
\t\t-tb, -table, -ntb, -notable\n\
\t\t-ls, -list, -nls, -nolist,\n\
\t\t-ot, -optimize, -not, -nooptimize,\n\
\t\t-d, -deterministic, -nd, -nondeterministic,\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define SOURCE		  1
#define VERBOSE		  2
#define OBJECT		  3
#define TABLE		  4
#define LIST		  5
#define OPTIMIZE	  6
#define NONDETERMINISTIC	  7
#define LANGUAGE_NAME	  8
#define LAST_OPTION	  LANGUAGE_NAME

#if 0
static char	*option_tbl [] = {
    "",
    "sc", "source", "nsc", "nosource",
    "v", "verbose", "nv", "noverbose",
    "ob", "object", "nob", "noobject",
    "tb", "table", "ntb", "notable",
    "ls", "list", "nls", "nolist",
    "ot", "optimize", "not", "nooptimize",
    "d", "deterministic", "nd", "nondeterministic",				      
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
    LANGUAGE_NAME, LANGUAGE_NAME,};


static int	option_get_kind (char *arg)
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



static char	*option_get_text (int kind)
{
    register int	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}



static	language_name (char *path_name, char *lang_name)
{
    register char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    if ((p = sxindex (strcpy (lang_name, p), '.')) != NULL) {
	*p = SXNUL;
    }

    if (strlen (lang_name) > 10) {
	lang_name [10] = SXNUL;
    }
}



static FILE *
sxstr_to_file (char *source_file, int source_lgth)
{
    char	*last;
    FILE	*infile;
    char	pathname [32];

    /* "w+": Truncate or create for update (reading and writing) */
    infile = sxtmpfile ();

    if (infile != NULL)
    {
	last = source_file + source_lgth;

	while (source_file < last)
	    putc (*source_file++, infile);

	unlink (pathname) /* vanishes at once, dies when closed */ ;
	rewind (infile);
    }

    return infile;
}
#endif



static FILE *
stdin_to_file (void)
{
    FILE	*infile;
    char	pathname [32];
    int		c;

    infile = sxtmpfile ();

    if (infile != NULL)
    {
	while ((c = getchar ()) != EOF)
	    putc (c, infile);

	unlink (pathname)		/* vanishes at once, dies when closed */ ;
	rewind (infile);
    }

    return infile;
}

int main (int argc, char *argv[])
{
    FILE	*infile;
    // int		ste;

    if (sxstdout == NULL) {
	sxstdout = stdout;
    }
    if (sxstderr == NULL) {
	sxstderr = stderr;
    }

    setbuf (sxstdout, NULL);

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	sxexit (3);
    }

    SXDS = &(re_header);

    if ((infile = fopen (argv [1], "r")) != NULL)
    {
	sxstr_mngr (SXBEGIN);
	sxerr_mngr (SXBEGIN);

	sxre_alloc ();

	/* Si la description lexicale est stockee ds une chaine str (plutot que ds un fichier), 
	   appeler sxre_doit (NULL, str) */
	sxre_doit (infile, argv [1]);

	sxerr_mngr (SXEND);
	sxstr_mngr (SXEND);

	fclose (infile);

	if (!SXDS->is_smp_error && (infile = stdin_to_file ()) != NULL)
	{
	    SXDS->scanact = NULL; /* Pas d'action lexicale */
	    SXDS->recovery = NULL; /* Pour l'instant */
	    SXDS->desambig = NULL; /* Pour l'instant */

	    sxstr_mngr (SXBEGIN);

	    sxdynam_scanner (SXOPEN);
	    
	    if (SXDS->scanact != NULL)
		(*(SXDS->scanact)) (SXOPEN, SXANY_INT, SXANY_INT);

	    sxsrc_mngr (SXINIT, infile, "<stdin>");
	    sxerr_mngr (SXBEGIN);

	    sxdynam_scanner (SXINIT);
	    
	    if (SXDS->scanact != NULL)
		(*(SXDS->scanact)) (SXINIT, SXANY_INT, SXANY_INT);

	    fprintf (sxstdout, "Let's go...\n");

	    while (sxsrcmngr.current_char != EOF)
	    {
		sxds_scanit ();

		if (SXDS->dslv [0].ts_lgth != 0)
		    fprintf (sxstdout, "%s\n", SXDS->dslv [0].token_string);
	    }

	    if (SXDS->scanact != NULL)
		(*(SXDS->scanact)) (SXFINAL, SXANY_INT, SXANY_INT);

	    sxdynam_scanner (SXFINAL);

	    sxerr_mngr (SXEND);
	    fclose (infile);
	    sxsrc_mngr (SXFINAL);

	    if (SXDS->scanact != NULL)
		(*(SXDS->scanact)) (SXCLOSE, SXANY_INT, SXANY_INT);

	    sxdynam_scanner (SXCLOSE);

	    sxstr_mngr (SXEND);
	}

	sxre_free ();
    }

    sxexit (0);
}
