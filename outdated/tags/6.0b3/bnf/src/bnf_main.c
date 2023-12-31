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


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030526 15:49 (phd):	#define SX_DFN_EXT_VAR			*/
/************************************************************************/
/* 20020318 14:30 (phd):	"sxtmpfile" dans "bnf_run"		*/
/************************************************************************/
/* Lun 25 Nov 1996 10:15(pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 21-01-94 14:05 (pb):		Caractere passe de "char" a "SXINT"	*/
/************************************************************************/
/* 22-10-93 10:00 (pb):		Adaptation a OSF			*/
/************************************************************************/
/* 18-12-87 15:00 (pb):		Warning si "nom-du-langage" trop long	*/
/************************************************************************/
/* 24-07-87 14:14 (phd):	Utilisation de "SXEXIT" partout		*/
/************************************************************************/
/* 20-11-87 15:15 (phd):	Modification du traitement de l'option	*/
/*				LANGUAGE_NAME pour permettre l'analyse	*/
/*				des eventuelles options restantes	*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Utilisation de "sxopentty"		*/
/************************************************************************/
/* 27-04-87 14:17 (phd):	Modification de detail			*/
/************************************************************************/
/* 20-03-87 18:15 (phd):	Inclusion de "release.h"		*/
/************************************************************************/
/* 13-02-87 14:43 (phd):	Mode verbose plus agreable		*/
/************************************************************************/
/* 19-12-86 08:57 (pb) :	Messages "by" et "release" en externe 	*/
/************************************************************************/
/* 25-11-86 18:10 (phd) :	Amelioration de l'ecriture des options 	*/
/************************************************************************/
/* 25-11-86 17:10 (phd) :	Utilisation d'un fichier temporaire 	*/
/*				lors de la lecture sur stdin.		*/
/************************************************************************/
/* 13-11-86 12:00 (pb) :	Changement dans la grammaire:	 	*/
/*			   	suppression du %DOLLAR de fin	 	*/
/*			   	Listing appele apres la semantique	*/
/*			   	pour la sortie des numeros d'actions	*/
/*			   	Suppression de l'option stl	   	*/
/*			   	Ajout de l'option max_RHS	   	*/
/************************************************************************/
/* 31-10-86 10:17 (pb) :	Listing appele depuis smp		*/
/************************************************************************/
/* 21-10-86 18:02 (phd) :	Unbufferisation de sxtty		*/
/************************************************************************/
/* 15-10-86 20:00 (bl) :	Modification pour la date.   		*/
/************************************************************************/
/* 08-10-86 17:00 (phd):	Mise en forme pour semact.		*/
/************************************************************************/
/* 11-09-86 17:30 (phd):	Modifications de detail.		*/
/************************************************************************/
/* 28-05-86 21:00 (bl) :	Decodage des options			*/
/************************************************************************/
/* 16-05-86 17:00 (phd):	Modification des appels au "src_mngr"	*/
/************************************************************************/
/* 15-05-86 13:03 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 15-05-86 13:03 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#define SX_DFN_EXT_VAR_BNF /* Pour definir variables globales du module BNF */

#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"

/* includes et variable pour la manipulation de la date pour bnf_modif_time */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
char WHAT_BNFMAIN[] = "@(#)SYNTAX - $Id: bnf_main.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

char	by_mess [] = "the SYNTAX grammar processor BNF";

#include "release.h"

SXINT	(*more_scan_act) (SXINT code, SXINT act_no);


extern VOID	no_tables (void), bnf_lo (void);
extern struct sxtables	bnf_tables;

extern VOID bnf_huge (struct bnf_ag_item *B, char *langname);
extern VOID bnf_terminal (struct bnf_ag_item *B, char *langname);

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "bnf";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-sc, -source, -nsc, -nosource,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ls, -list, -nls, -nolist,\n\
\t\t-rhs nnn, -max_right_hand_side nnn,\n\
\t\t-ln name, -language_name name\n\
\t\t-h, -huge\n\
\t\t-t, -terminal\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define SOURCE 			((SXINT)1)
#define VERBOSE 		((SXINT)2)
#define LIST 			((SXINT)3)
#define MAX_RIGHT_HAND_SIDE 	((SXINT)4)
#define LANGUAGE_NAME		((SXINT)5)
#define HUGE		        ((SXINT)6)
#define TERMINAL	        ((SXINT)7)
#define LAST_OPTION		TERMINAL

static char	*option_tbl [] = {"", "sc", "source", "nsc", "nosource", "v", "verbose", "nv", "noverbose", "ls", "list",
     "nls", "nolist", "rhs", "max_right_hand_side", "ln", "language_name", "h", "huge", "t", "terminal",};
static SXINT	option_kind [] = {UNKNOWN_ARG, SOURCE, SOURCE, -SOURCE, -SOURCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     LIST, LIST, -LIST, -LIST, MAX_RIGHT_HAND_SIDE, MAX_RIGHT_HAND_SIDE, LANGUAGE_NAME, LANGUAGE_NAME, HUGE, HUGE, TERMINAL, TERMINAL,};



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



static char	*option_get_text (SXINT kind)
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}



static SXVOID	language_name (char *path_name)
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



static VOID	bnf_run (char *pathname)
{
    FILE	*infile;

    if (pathname == NULL) {
        int c;

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
	bnf_ag.bnf_modif_time = time (0) /* stdin => now */ ;
    }
    else if ((infile = sxfopen (pathname, "r")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	sxperror (pathname);
	sxerrmngr.nbmess [2]++;
	return;
    }
    else {
	struct stat	buf /* date/time info */ ;

	if (sxverbosep) {
	    fprintf (sxtty, "%s:\n", pathname);
	}

	language_name (pathname);
	sxsrc_mngr (INIT, infile, pathname);
	stat (sxsrcmngr.source_coord.file_name, &buf);
	bnf_ag.bnf_modif_time = buf.st_mtime;
    }

    sxerr_mngr (BEGIN);
    syntax (ACTION, &bnf_tables);
    sxsrc_mngr (FINAL);

    if (!is_proper || IS_ERROR)
	no_tables ();
    else {
      if (is_huge)
	bnf_huge (&bnf_ag, prgentname);
      else {
	if (is_terminal)
	  bnf_terminal (&bnf_ag, prgentname);
	  
	bnf_write (&bnf_ag, prgentname);
      }
    }

    bnf_lo ();
    sxerr_mngr (END);
    fclose (infile);
    bnf_free (&bnf_ag);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main(int argc, char *argv[])
{
    int 	argnum;

    if (sxstdout == NULL) {
	sxstdout = stdout;
    }
    if (sxstderr == NULL) {
	sxstderr = stderr;
    }

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (SEVERITIES);
    }

    sxopentty ();

/* valeurs par defaut */

    options_set = OPTION (SOURCE) | OPTION (VERBOSE) | OPTION (LIST);
    is_source = sxverbosep = is_list = TRUE;
    max_RHS = -1; /* Le 16/4/2002, par defaut, pas de verif de la longueur des RHS */
    is_huge = is_terminal = FALSE;


/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case SOURCE:
	    is_source = TRUE, options_set |= OPTION (SOURCE);
	    break;

	case -SOURCE:
	    is_list = is_source = FALSE, options_set &= noOPTION (LIST) & noOPTION (SOURCE);
	    break;

	case VERBOSE:
	    sxverbosep = TRUE, options_set |= OPTION (VERBOSE);
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE, options_set &= noOPTION (VERBOSE);
	    break;

	case LIST:
	    is_list = is_source = TRUE, options_set |= OPTION (LIST) | OPTION (LIST);
	    break;

	case -LIST:
	    is_list = FALSE, options_set &= noOPTION (LIST);
	    break;

	case MAX_RIGHT_HAND_SIDE:
	    if (++argnum >= argc || !sxnumarg (argv [argnum], &max_RHS))
		fprintf (sxstderr, "%s: a numeric argument must follow the \"%s\" option;\n\tvalue %ld is retained.\n", ME
		     , option_get_text (MAX_RIGHT_HAND_SIDE), (long)max_RHS);
	    else {
		options_set |= OPTION (MAX_RIGHT_HAND_SIDE);
	    }

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

	    options_set = OPTION (LANGUAGE_NAME);
	    break;

	case HUGE:
	    is_huge = TRUE, options_set |= OPTION (HUGE);
	    is_terminal = FALSE;
	    break;

	case TERMINAL:
	    is_terminal = TRUE, options_set |= OPTION (TERMINAL);
	    is_huge = FALSE;
	    break;

	case UNKNOWN_ARG:
	    if (options_set & OPTION (LANGUAGE_NAME)) {
		fprintf (sxstderr, "%s: the \"%s\" option must not be used when naming files.\n", ME, option_get_text (
		     LANGUAGE_NAME));
		options_set &= noOPTION (LANGUAGE_NAME);
	    }

	    goto run;

	default:
	    fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (long)option_get_kind (argv [argnum]));
	    SXEXIT (3);
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
	fprintf (sxtty, "%s\n", release_mess);
    }

    sxstr_mngr (BEGIN);
    (*(bnf_tables.analyzers.parser)) (BEGIN);
    syntax (OPEN, &bnf_tables);

    if (options_set & OPTION (LANGUAGE_NAME)) {
	bnf_run ((char*)NULL);
    }
    else {
	do {
	    int	severity = sxerrmngr.nbmess [2];

	    sxerrmngr.nbmess [2] = 0;
	    bnf_run (argv [argnum++]);
	    sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
    }

    syntax (CLOSE, &bnf_tables);
    (*(bnf_tables.analyzers.parser)) (END);
    sxstr_mngr (END);

    {
	int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	SXEXIT (severity);
    }

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



char	*options_text (char *line)
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

	    if (i == MAX_RIGHT_HAND_SIDE) {
		sprintf (line + strlen (line), "=%ld", (long)max_RHS);
	    }
	    else if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
}



VOID	sxvoid (void)
/* procedure ne faisant rien */
{
}
