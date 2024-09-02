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

/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 

/* Programme principal pour l'analyse de textes decrits a l'aide
   d'une grammaire RCG */

#include "sxversion.h"
#include "sxunix.h"

#define SX_DFN_EXT_VAR2
#include "udag_scanner.h"

char WHAT_RCG_MAIN[] = "@(#)SYNTAX - $Id: RCG_main.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

#include "rcg_sglbl.h"

SXUINT          maximum_input_size; /* Pour udag_scanner */
bool       tmp_file_for_stdin; /* Pour read_a_re */

#ifndef NO_SX
/* On utilise udag_scanner pour lire le source */
#ifdef dummy_tables
/* Il y a des tables SYNTAX ds le coup */
extern SXTABLES	dummy_tables;
static SXTABLES	*dummy_tables_ptr = {&dummy_tables};
#else /* dummy_tables */
static SXTABLES	*dummy_tables_ptr = {NULL};
#endif /* dummy_tables */
#endif /* !NO_SX */

extern bool call_rcg_parse_it (void);
bool        (*main_parser)(void) = call_rcg_parse_it;

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
\t-btn nnn, -best_tree_number nnn,\n\
\t-dag,\n\
\t-udag,\n\
\t-sdag,\n\
\t-string,\n\
\t-dy file_name, -dag_yield file_name,\n\
";

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
#define DAG_INPUT         12
#define UDAG_INPUT        13
#define SDAG_INPUT        14
#define STRING_INPUT      15
#define DAG_YIELD         16
#define SOURCE_FILE	  17


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
    "btn", "best_tree_number",
    "dag",
    "udag",
    "sdag",
    "string",
    "dy", "dag_yield",
};

static SXINT	option_kind [] = {
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
    BEST_TREE_NUMBER, BEST_TREE_NUMBER,
    DAG_INPUT,
    UDAG_INPUT,
    SDAG_INPUT,
    STRING_INPUT,
    DAG_YIELD, DAG_YIELD,
};



static SXINT
option_get_kind (char *arg)
{
  char	**opt;

  if (*arg++ != '-')
    return SOURCE_FILE;

  if (*arg == SXNUL)
    return STDIN;

  for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
    if (strcmp (*opt, arg) == 0 /* egalite */ )
      break;
  }

  return option_kind [opt - option_tbl];
}



static char*
option_get_text (SXINT kind)
{
  SXINT	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}


#ifdef NO_SX
#include "sxword.h"

extern bool call_rcgparse_it ();
/* extern void    sxtime (); */
static FILE	*infile;
static char     *source_file_name;

#define ts_put(c)							\
(    (sxsvar.sxlv.ts_lgth+2 == sxsvar.sxlv_s.ts_lgth_use)			\
	? sxsvar.sxlv_s.token_string = (char *) sxrealloc (sxsvar.sxlv_s.	\
	    token_string, sxsvar.sxlv_s.ts_lgth_use *= 2, sizeof (char))	\
	: NULL,								\
     sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth ++] = c		\
)

#define ts_null()	(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth] = SXNUL)

bool
rcgscan_it (sxword_header *t_names_ptr)
{
  /* Appele' depuis RCG_parser pour simuler un scanner, les terminaux sont recherches ds le sxword_header t_names_ptr */
  SXINT  c;
  char mess [32];

  if (is_print_time)
    sxtime (SXINIT,NULL);

  sxsvar.sxlv_s.token_string = (char*) sxalloc (sxsvar.sxlv_s.ts_lgth_use = 120, sizeof (char));
    
  sxsvar.sxlv.ts_lgth = 0;
  sxsvar.SXS_tables.S_termax = t_names_ptr->top-1;

  sxsvar.sxlv.terminal_token.source_index.file_name = source_file_name;
  sxsvar.sxlv.terminal_token.comment = NULL;

  sxsvar.sxlv.terminal_token.source_index.line = 1;
  sxsvar.sxlv.terminal_token.source_index.column = 1;

  /* On met EOF */
  sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE; /* !! */	
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
  sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE; /* !! */	
  sxsvar.sxlv.terminal_token.lahead = sxsvar.SXS_tables.S_termax;
    
  sxput_token (sxsvar.sxlv.terminal_token);
  
  sxfree (sxsvar.sxlv_s.token_string), sxsvar.sxlv_s.token_string = NULL;

  if (is_print_time) {
    sprintf (mess, "\tScanner[%i]", sxplocals.Mtok_no-1);
    sxtime (SXACTION, mess);
  }

  return true; /* inutile */
}

static	bool
RCG_run (char *pathname)
{
  SXINT	c;
  bool ret_val;

  if (pathname == NULL) {
    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }

    if ((infile = sxtmpfile ()) == NULL) {
      fprintf (sxstderr, "%s: Unable to create ", ME);
      sxperror ("temp file");
      return false;
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
      return false;
    }
    else {
      if (sxverbosep) {
	fprintf (sxtty, "%s:\n", pathname);
      }

      source_file_name = pathname;
    }
  }

  sxtkn_mngr (SXOPEN, 2);
  sxtkn_mngr (SXINIT, 0);

  ret_val = call_rcgparse_it ();

  sxtkn_mngr (SXFINAL, 0);
  sxtkn_mngr (SXCLOSE, 0);

  fclose (infile);

  return ret_val;
}
#else /* !NO_SX */
static	void
RCG_run (char *pathname)
{
  SXINT severity;
  FILE	*infile;

  if (pathname == NULL) {
    SXINT	c;

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
    sxsrc_mngr (SXINIT, infile, "");

    source_file_name = "<stdin>";
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

      sxsrc_mngr (SXINIT, infile, pathname);
    }
    
    source_file_name = pathname;
  }

  source_file = infile;
    
  {
    short c;

    /* si le dag_kind n'est pas spécifié par -dag ou -udag, on tente une détection automatique */
    if (dag_kind == 0) {
      c = sxlafirst_char();       // (*sxsrcmngr.infile)._IO_buf_base[0];
      sxlaback(1);                // on revient en arrière après avoir avancé d'un pas de lookahead pour récupérer c

      if ((c < '0' || c > '9') && c != '#' /*c'est probablement un dag normal*/)
	dag_kind = DAG_KIND;
      else
	dag_kind = UDAG_KIND;
    }
  }

  dag_scanner (SXBEGIN, dummy_tables_ptr);
  dag_scanner (SXOPEN, NULL);
  dag_scanner (SXINIT, NULL);

  severity = dag_scanner (SXACTION, NULL);
  (void) severity;  /* ajoute par Hubert */

  dag_scanner (SXFINAL, NULL);
  sxsrc_mngr (SXFINAL);
  dag_scanner (SXCLOSE, NULL);
  dag_scanner (SXEND, NULL);

  fclose (infile);
}
#endif /* !NO_SX */



/************************************************************************/
/* main function */
/************************************************************************/
int
main (int argc, char *argv[])
{
  SXINT		argnum;
  bool	in_options, is_source_file, is_stdin, is_generator_length;
  char	*str;

  sxopentty ();

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    sxexit (3);
  }

  /* valeurs par defaut */
  is_print_time = true;
  is_no_semantics = false;
  is_parse_tree_number = false;
  is_default_semantics = true; /* Semantique specifiee avec la grammaire */
  debug_level = 0;
  forest_level = 0; /* Ca permet d'utiliser un RCG_parser compile avec -DPARSE_FOREST avec un parser qcq */

  is_guiding = is_full_guiding = false;
  best_tree_number = 1;
  maximum_input_size = ~0; /* unbounded */

  is_generator_length = false;
  in_options = true;
  is_source_file = false;
  is_stdin = false;
  dag_yield_file_name = NULL;
  dag_yield_file = NULL;

  /* Decodage des options */
  argnum = 0;

  while (in_options && ++argnum < argc) {
    switch (option_get_kind (argv [argnum])) {
    case VERBOSE:
      sxverbosep = true;
      break;

    case -VERBOSE:
      sxverbosep = false;
      break;

    case NO_SEM:
      is_no_semantics = true;
      is_default_semantics = false;
      is_parse_tree_number = false;
      break;

    case DEFAULT_SEM:
      is_no_semantics = false;
      is_default_semantics = true;
      is_parse_tree_number = false;
      break;

    case PARSE_TREE_NUMBER:
      is_no_semantics = false;
      is_default_semantics = false;
      is_parse_tree_number = true;
      break;

    case TIME:
      is_print_time = true;
      break;

    case -TIME:
      is_print_time = false;
      break;

    case FOREST_LEVEL:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
	sxexit (3);
      }

      forest_level = (SXINT) strtol (argv [argnum], &str, 0);

      if (str == argv [argnum]) {
	fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
	sxexit (3);
      }
	    
      break;

    case GUIDING:
      forest_level = FL_n | FL_clause | FL_lhs_prdct | FL_lhs_clause;
      is_guiding = true;
      break;

    case FULL_GUIDING:
      forest_level = FL_n | FL_clause | FL_rhs | FL_lhs_prdct | FL_lhs_clause;
      is_full_guiding = true;
      break;

    case STDIN:
      is_stdin = true;
      break;

    case DEBUG_LEVEL:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
	sxexit (3);
      }

      debug_level = (SXINT) strtol (argv [argnum], &str, 0);

      if (str == argv [argnum]) {
	fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
	sxexit (3);
      }
	    
      break;

    case SOURCE_FILE:
      if (is_stdin) {
	is_stdin = false;
      }
      is_source_file = true;
      in_options = false;
      break;

    case BEST_TREE_NUMBER:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
	sxexit (3);
      }

      best_tree_number = (SXINT) strtol (argv [argnum], &str, 0);

      if (str == argv [argnum]) {
	fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
	sxexit (3);
      }
	    
      break;

    case DAG_INPUT:
      dag_kind = DAG_KIND;
      break;

    case UDAG_INPUT:
      dag_kind = UDAG_KIND;
      break;

    case SDAG_INPUT:
      dag_kind = SDAG_KIND;
      break;

    case STRING_INPUT:
      dag_kind = STRING_KIND;
      break;

    case DAG_YIELD:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (DAG_YIELD));
	sxexit (3);
      }
	      
      if ((dag_yield_file = sxfopen (dag_yield_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) file in write mode ", ME);
	sxperror (argv [argnum]);
	sxexit (3);
      }

      break;

    case UNKNOWN_ARG:
      fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
      sxexit (3);
    }
  }

  if (!is_stdin && !is_source_file && !is_generator_length) {
    fprintf (sxstderr, Usage, ME);
    sxexit (3);
  }

  if (sxverbosep) {
    fprintf (sxtty, "%s\n", release_mess);
  }

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
  sxexit (sxerr_max_severity ());

  return EXIT_SUCCESS;
}

