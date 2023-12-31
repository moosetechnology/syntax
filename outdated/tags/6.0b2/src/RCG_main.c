/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1997 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe ATOLL.			  *
   *							  *
   ******************************************************** */





/* Programme principal pour l'analyse de textes decrits a l'aide
   d'une grammaire RCG */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040220 14:44 (phd):	#define SX_DFN_EXT_VAR			*/
/************************************************************************/
/* 20030319 10:48 (phd):	Utilisation de "sxtimestamp"		*/
/************************************************************************/
/* 20020318 14:30 (phd):	"sxtmpfile" dans "RCG_run"		*/
/************************************************************************/
/* Mer 15 Dec 1999 13:24 (pb):	Suppression de option "-generator_lgth"	*/
/************************************************************************/
/* 6 Nov 1997 10:23 (pb):	Ajout de cette rubrique "modifications" */
/************************************************************************/



#include "sxunix.h"

char WHAT_RCG_MAIN[] = "@(#)SYNTAX - $Id: RCG_main.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

#define SX_DFN_EXT_VAR

#include "rcg_sglbl.h"
#include "release.h"


#ifndef NO_SX
extern struct sxtables	RCG_tables;
#endif /* !NO_SX */

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "RCG_parser";
static char	Usage [] = "\
Usage:\t%s [options] files\n\
options=\n\
\t-v, -verbose, -nv, -noverbose,\n\
\t-ns, -no_semantics,\n\
\t-ds, -default_semantics,\n\
\t-ptn, -parse_tree_number,\n\
\t-t, -time, -nt, -no_time\n\
\t-, -stdin,\n\
\t-dl nnn, -debug_level nnn,\n\
\t-fl nnn, -forest_level nnn,(source=1, clause=2, lhs_prdct=4, lhs_clause=8, rhs=(16+4), n=32, supertagger=64)\n\
\t-g, -guiding,\n\
\t-fg, -full_guiding,\n\
\t-btn nnn, -best_tree_number nnn,\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define VERBOSE 	  1
#define NO_SEM		  2
#define DEFAULT_SEM	  3
#define PARSE_TREE_NUMBER 4
#define TIME		  5
#define STDIN	  	  6
#define DEBUG_LEVEL	  7
#define FOREST_LEVEL	  8
#define GUIDING		  9
#define FULL_GUIDING	  10
#define BEST_TREE_NUMBER  11
#define SOURCE_FILE	  12


static char	*option_tbl [] = {
    "",
    "v", "verbose", "nv", "noverbose",
    "ns", "no_semantics",
    "ds", "default_semantics",
    "ptn", "parse_tree_number",
    "t", "time", "nt", "no_time",
    "stdin",
    "dl", "debug_level",
    "fl", "forest_level",
    "g", "guiding",
    "fg", "full_guiding",
    "btn", "best_tree_number",};

static int	option_kind [] = {
    UNKNOWN_ARG,
    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
    NO_SEM, NO_SEM,
    DEFAULT_SEM, DEFAULT_SEM,
    PARSE_TREE_NUMBER, PARSE_TREE_NUMBER,
    TIME, TIME, -TIME, -TIME,
    STDIN,
    DEBUG_LEVEL, DEBUG_LEVEL,
    FOREST_LEVEL, FOREST_LEVEL,
    GUIDING, GUIDING,
    FULL_GUIDING, FULL_GUIDING,
    BEST_TREE_NUMBER, BEST_TREE_NUMBER,};



static int	option_get_kind (arg)
    register char	*arg;
{
    register char	**opt;

    if (*arg++ != '-')
	return SOURCE_FILE;

    if (*arg == NUL)
	return STDIN;

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


#ifdef NO_SX
#include "sxword.h"

extern BOOLEAN call_rcgparse_it ();
/* extern VOID    sxtime (); */
static FILE	*infile;
static char     *source_file_name;

#define ts_put(c)							\
(    (sxsvar.sxlv.ts_lgth+2 == sxsvar.sxlv_s.ts_lgth_use)			\
	? sxsvar.sxlv_s.token_string = (char *) sxrealloc (sxsvar.sxlv_s.	\
	    token_string, sxsvar.sxlv_s.ts_lgth_use *= 2, sizeof (char))	\
	: NULL,								\
     sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth ++] = c		\
)

#define ts_null()	(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth] = NUL)

BOOLEAN
rcgscan_it (t_names_ptr)
     sxword_header	*t_names_ptr;
{
  /* Appele' depuis RCG_parser pour simuler un scanner, les terminaux sont recherches ds le sxword_header t_names_ptr */
  int  c;
  char mess [32];

  if (is_print_time)
    sxtime (INIT,NULL);

  sxsvar.sxlv_s.token_string = (char*) sxalloc (sxsvar.sxlv_s.ts_lgth_use = 120, sizeof (char));
    
  sxsvar.sxlv.ts_lgth = 0;
  sxsvar.SXS_tables.S_termax = t_names_ptr->top-1;

  sxsvar.sxlv.terminal_token.source_index.file_name = source_file_name;
  sxsvar.sxlv.terminal_token.comment = NULL;

  sxsvar.sxlv.terminal_token.source_index.line = 1;
  sxsvar.sxlv.terminal_token.source_index.column = 1;

  /* On met EOF */
  sxsvar.sxlv.terminal_token.string_table_entry = EMPTY_STE; /* !! */	
  sxsvar.sxlv.terminal_token.lahead = sxsvar.SXS_tables.S_termax;
    
  SXGET_TOKEN (0) = sxsvar.sxlv.terminal_token;
    
  while ((c = getc (infile)) != EOF) {
    switch (c) {
    case ' ':
    case '\t':
    case '\n':
      if (sxsvar.sxlv.ts_lgth > 0) {
	if (sxsvar.sxlv.terminal_token.string_table_entry =
	    sxword_2retrieve (t_names_ptr, sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth))
	  sxsvar.sxlv.terminal_token.lahead = sxsvar.sxlv.terminal_token.string_table_entry-1;
	else
	  sxsvar.sxlv.terminal_token.lahead = 0; /* erreur */
    
	sxput_token (sxsvar.sxlv.terminal_token);

	sxsvar.sxlv.terminal_token.source_index.column += sxsvar.sxlv.ts_lgth;
	sxsvar.sxlv.ts_lgth = 0;
      }

      if (c == '\n') {
	sxsvar.sxlv.terminal_token.source_index.line++;
	sxsvar.sxlv.terminal_token.source_index.column = 1;
      }
      else
	sxsvar.sxlv.terminal_token.source_index.column++;

      break;
      
    default :
      ts_put (c);
      break;
    }
  }
      
  if (sxsvar.sxlv.ts_lgth > 0) {
    if (sxsvar.sxlv.terminal_token.string_table_entry =
	sxword_2retrieve (t_names_ptr, sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth))
      sxsvar.sxlv.terminal_token.lahead = sxsvar.sxlv.terminal_token.string_table_entry-1;
    else
      sxsvar.sxlv.terminal_token.lahead = 0; /* erreur */
    
    sxput_token (sxsvar.sxlv.terminal_token);

    sxsvar.sxlv.terminal_token.source_index.column += sxsvar.sxlv.ts_lgth;
    sxsvar.sxlv.ts_lgth = 0;
  }

  /* On met EOF */
  sxsvar.sxlv.terminal_token.string_table_entry = EMPTY_STE; /* !! */	
  sxsvar.sxlv.terminal_token.lahead = sxsvar.SXS_tables.S_termax;
    
  sxput_token (sxsvar.sxlv.terminal_token);
  
  sxfree (sxsvar.sxlv_s.token_string), sxsvar.sxlv_s.token_string = NULL;

  if (is_print_time) {
    sprintf (mess, "\tScanner[%i]", sxplocals.Mtok_no-1);
    sxtime (ACTION, mess);
  }

  return TRUE; /* inutile */
}

static	BOOLEAN
RCG_run (pathname)
    char	*pathname;
{
  int	c;
  BOOLEAN ret_val;

  if (pathname == NULL) {
    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }

    if ((infile = sxtmpfile ()) == NULL) {
      fprintf (sxstderr, "%s: Unable to create ", ME);
      sxperror ("temp file");
      return FALSE;
    }

    while ((c = getchar ()) != EOF) {
      putc (c, infile);
    }

    rewind (infile);
    source_file_name = "stdin";
  }
  else {
    if ((infile = sxfopen (pathname, "r")) == NULL) {
      fprintf (sxstderr, "%s: Cannot open (read) ", ME);
      sxperror (pathname);
      return FALSE;
    }
    else {
      if (sxverbosep) {
	fprintf (sxtty, "%s:\n", pathname);
      }

      source_file_name = pathname;
    }
  }

  sxtkn_mngr (OPEN, 2);
  sxtkn_mngr (INIT, 0);

  ret_val = call_rcgparse_it ();

  sxtkn_mngr (FINAL, 0);
  sxtkn_mngr (CLOSE, 0);

  fclose (infile);

  return ret_val;
}
#else /* !NO_SX */
static	RCG_run (pathname)
    char	*pathname;
{
  FILE	*infile;

  if (pathname == NULL) {
    int	c;

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

      sxsrc_mngr (INIT, infile, pathname);
    }
  }

  sxerr_mngr (BEGIN);
  syntax (ACTION, &RCG_tables);
  sxerr_mngr (END);
  sxsrc_mngr (FINAL);
  fclose (infile);
}
#endif /* !NO_SX */



/************************************************************************/
/* main function
/************************************************************************/
int main(int argc, char *argv[])
{
    int		argnum;
    BOOLEAN	in_options, is_source_file, is_stdin, is_generator_length;
    char	*str;

    sxstdout = stdout, sxstderr = stderr;

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (3);
    }

    sxopentty ();

/* valeurs par defaut */
    sxverbosep = TRUE;
    is_print_time = TRUE;
    is_no_semantics = FALSE;
    is_parse_tree_number = FALSE;
    is_default_semantics = TRUE; /* Semantique specifiee avec la grammaire */
    debug_level = 0;
    forest_level = 0; /* Ca permet d'utiliser un RCG_parser compile avec -DPARSE_FOREST avec un parser qcq */
#if 0
    forest_level = FL_n | FL_source | FL_clause | FL_rhs | FL_lhs_prdct | FL_lhs_clause; /* complet */
#endif
    is_guiding = is_full_guiding = FALSE;
    best_tree_number = 1;

/* Decodage des options */
    is_generator_length = FALSE;
    in_options = TRUE;
    is_source_file = FALSE;
    is_stdin = FALSE;
    argnum = 0;

    while (in_options && ++argnum < argc) {
	switch (option_get_kind (argv [argnum])) {
	case VERBOSE:
	    sxverbosep = TRUE;
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE;
	    break;

	case NO_SEM:
	    is_no_semantics = TRUE;
	    is_default_semantics = FALSE;
	    is_parse_tree_number = FALSE;
	    break;

	case DEFAULT_SEM:
	    is_no_semantics = FALSE;
	    is_default_semantics = TRUE;
	    is_parse_tree_number = FALSE;
	    break;

	case PARSE_TREE_NUMBER:
	    is_no_semantics = FALSE;
	    is_default_semantics = FALSE;
	    is_parse_tree_number = TRUE;
	    break;

	case TIME:
	    is_print_time = TRUE;
	    break;

	case -TIME:
	    is_print_time = FALSE;
	    break;

	case FOREST_LEVEL:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }

	    forest_level = (int) strtol (argv [argnum], &str, 0);

	    if (str == argv [argnum]) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }
	    
	    break;

	case GUIDING:
	    forest_level = FL_n | FL_clause | FL_lhs_prdct | FL_lhs_clause;
	    is_guiding = TRUE;
	    break;

	case FULL_GUIDING:
	    forest_level = FL_n | FL_clause | FL_rhs | FL_lhs_prdct | FL_lhs_clause;
	    is_full_guiding = TRUE;
	    break;

	case STDIN:
	    is_stdin = TRUE;
	    break;

	case DEBUG_LEVEL:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }

	    debug_level = (int) strtol (argv [argnum], &str, 0);

	    if (str == argv [argnum]) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }
	    
	    break;

	case SOURCE_FILE:
	    if (is_stdin) {
		is_stdin = FALSE;
	    }

	    is_source_file = TRUE;
	    in_options = FALSE;
	    break;

	case BEST_TREE_NUMBER:
	    if (++argnum >= argc) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }

	    best_tree_number = (int) strtol (argv [argnum], &str, 0);

	    if (str == argv [argnum]) {
		fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
			 ME, argv [argnum-1]);
		SXEXIT (3);
	    }
	    
	    break;

	case UNKNOWN_ARG:
	    fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
	    SXEXIT (3);
	}
    }

    if (!is_stdin && !is_source_file && !is_generator_length) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (3);
    }

    if (sxverbosep) {
	fprintf (sxtty, "%s\n", release_mess);
    }

#ifdef NO_SX
    /* Le source est traite' directement par RCG_parser */
    if (is_stdin) {
      RCG_run (NULL);
    }
    else {
      if (is_source_file) {
	do {
	  RCG_run (argv [argnum++]);
	} while (argnum < argc);
      }
    }
#else /* !NO_SX */
    /* Le source est traite' par [le scanneur] de SYNTAX */

    sxstr_mngr (BEGIN);

    (*(RCG_tables.analyzers.parser)) (BEGIN) /* Allocation des variables globales du parser */ ;
    syntax (OPEN, &RCG_tables) /* Initialisation de SYNTAX */ ;

    if (is_stdin) {
      RCG_run (NULL);
    }
    else {
      if (is_source_file) {
	do {
	  int	severity = sxerrmngr.nbmess [2];

	  sxerrmngr.nbmess [2] = 0;
	  sxstr_mngr (BEGIN) /* remise a vide de la string table */ ;
	  RCG_run (argv [argnum++]);
	  sxerrmngr.nbmess [2] += severity;
	} while (argnum < argc);
      }
      else {
	/* Cas du generateur (pas de source) */
	syntax (ACTION, &RCG_tables);
      }
    }

    syntax (CLOSE, &RCG_tables);
    (*(RCG_tables.analyzers.parser)) (END);

    sxstr_mngr (END);

    {
	int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
	    ;

	SXEXIT (severity);
    }
#endif /* !NO_SX */

  return EXIT_SUCCESS;
}


#if 0
/* Le 20/12/04 devient un module de la librairie  sxtm_mngr.c */
VOID
sxtime (what, str)
    int what;
    char *str;
{
  if (what == TIME_INIT) {
    sxtimestamp (INIT, NULL);
  }
  else {
    fputs (str, sxtty);

    if (is_print_time)
      sxtimestamp (ACTION, NULL);
    else
      putc ('\n', sxtty);
  }
}
#endif /* 0 */


VOID
sxvoid ()
/* procedure ne faisant rien */
{
}

BOOLEAN
sxbvoid ()
/* procedure ne faisant rien */
{
    return TRUE;
}

int
sxivoid ()
/* procedure ne faisant rien */
{
    return 1;
}
