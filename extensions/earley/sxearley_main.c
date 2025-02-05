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

/* Main module for an Earley-like parser built by csynt_lc, the left-corner SYNTAX constructor */

#include "sxversion.h"
#include "sxunix.h"
#include "earley.h"
#include "udag_scanner.h"
#include <float.h>
#include <math.h>

char WHAT_SXEARLEY_MAIN[] = "@(#)SYNTAX - $Id: sxearley_main.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

static char	ME [] = "sxearley_main";

extern SXUINT          maximum_input_size; /* Pour udag_scanner definie ds earley_parser */

#if 0
#ifdef dummy_tables
/* Il y a des tables SYNTAX ds le coup */
extern SXTABLES	dummy_tables;
static SXTABLES	*dummy_tables_ptr = {&dummy_tables};
#else /* dummy_tables */
static SXTABLES	*dummy_tables_ptr = {NULL};
#endif /* dummy_tables */
#endif /* 0 */


/*---------------*/
/*    options    */
/*---------------*/

extern bool tmp_file_for_stdin; /* Ds earley_parser.c */
extern bool sxtty_is_stderr;

static bool  is_help;
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-v, -verbose, -nv, -noverbose,\n\
\t\t-ns, -no_semantics,\n\
\t\t-ds, -default_semantics,\n\
\t\t-ptn, -parse_tree_number,\n\
\t\t-t, -time, -nt, -no_time\n\
\t\t-, -stdin,\n\
\t\t--help,\n\
\t\t-tl, -try_lower,\n\
\t\t-tu, -try_upper,\n\
\t\t-Uw capitalized_initial_unknown_word,\n\
\t\t-uw lower_case_initial_unknown_word,\n\
\t\t-digits dico_word,\n\
\t\t-pf, -parse_forest,\n\
\t\t-pp, -print_prod.\n\
\t\t-to max_time(s)=dd, -time_out max_time(s)=dd,\n\
\t\t-vto max_time(s)=dd[.dd], -virtual_time_out max_time(s)=dd[.dd],\n\
\t\t-td, -tagged_dag,\n\
\t\t-pos, -tagged_sdag,\n\
\t\t-dag,\n\
\t\t-udag,\n\
\t\t-sdag,\n\
\t\t-string,\n\
\t\t-crs type, -check_rhs_sequences type\n\
\t\t-dsa, -dynamic_set_automaton\n\
\t\t-b beam_value=double, -beam beam_value=double\n\
\t\t-mis size, -max_input_size size,\n\
\t\t-tmp, -tmp_file_for_stdin,\n\
%s\
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
#define PRINT_PROD	  7
#define HELP	          8
#define TRY_LOWER         9
#define TRY_UPPER         10
#define CAPITALIZED_INITIAL_UNKNOWN_WORD         11
#define LOWER_CASE_INITIAL_UNKNOWN_WORD          12
#define DIGIT_SEQUENCE 	  13
#define PARSE_FOREST	  14
#define TIME_OUT 	  15
#define VIRTUAL_TIME_OUT  16
#define TAGGED_DAG        17
#define TAGGED_SDAG       18
#define DAG_INPUT         19
#define UDAG_INPUT        20
#define SDAG_INPUT        21
#define STRING_INPUT      22
#define CHECK_RHS_SEQUENCES                      23
#define DYNAMIC_SET_AUTOMATON                    24
#define BEAM_VALUE    25
#define MAX_INPUT_SIZE    26
#define TMP_FILE_FOR_STDIN                       27
#define SOURCE_FILE	  28


static char	*option_tbl [] = {
    "",
    "v", "verbose", "nv", "noverbose",
    "ns", "no_semantics",
    "ds", "default_semantics",
    "ptn", "parse_tree_number",
    "t", "time", "nt", "no_time",
    "stdin",
    "pp", "print_prod",
    "-help",
    "tl", "try_lower",
    "tu", "try_upper",
    "Uw",
    "uw",
    "digits",
    "pf", "parse_forest",
    "to", "time_out",
    "vto", "virtual_time_out",
    "td", "tagged_dag",
    "pos", "tagged_sdag",
    "dag",
    "udag",
    "sdag",
    "string",
    "crs", "check_rhs_sequences",
    "dsa", "dynamic_set_automaton",
    "b", "beam",
    "mis", "max_input_size",
    "tmp", "tmp_file_for_stdin",
};

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
    NO_SEM, NO_SEM,
    DEFAULT_SEM, DEFAULT_SEM,
    PARSE_TREE_NUMBER, PARSE_TREE_NUMBER,
    TIME, TIME, -TIME, -TIME,
    STDIN,
    PRINT_PROD, PRINT_PROD,
    HELP,
    TRY_LOWER, TRY_LOWER,
    TRY_UPPER, TRY_UPPER,
    CAPITALIZED_INITIAL_UNKNOWN_WORD,
    LOWER_CASE_INITIAL_UNKNOWN_WORD,
    DIGIT_SEQUENCE,
    PARSE_FOREST, PARSE_FOREST,
    TIME_OUT, TIME_OUT,
    VIRTUAL_TIME_OUT, VIRTUAL_TIME_OUT,
    TAGGED_DAG, TAGGED_DAG,
    TAGGED_SDAG, TAGGED_SDAG,
    DAG_INPUT,
    UDAG_INPUT,
    SDAG_INPUT,
    STRING_INPUT,
    CHECK_RHS_SEQUENCES, CHECK_RHS_SEQUENCES,
    DYNAMIC_SET_AUTOMATON, DYNAMIC_SET_AUTOMATON,
    BEAM_VALUE, BEAM_VALUE,
    MAX_INPUT_SIZE, MAX_INPUT_SIZE,
    TMP_FILE_FOR_STDIN, TMP_FILE_FOR_STDIN,
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

static void
earley_run (char *pathname)
{
#if 0
  SXINT severity;
#endif /* 0 */
  FILE	*infile = NULL;

  if (pathname == NULL) {
    int	c; /* pas SXINT */

    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }

    if (tmp_file_for_stdin) {
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
    } else
      sxsrc_mngr (SXINIT, stdin, "");

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

    /* si le dag_kind n'est pas sp�cifi� par -dag ou -udag, on tente une d�tection automatique */
    if (dag_kind == 0) {
      c = sxlafirst_char();       // (*sxsrcmngr.infile)._IO_buf_base[0];
      sxlaback(1);                // on revient en arri�re apr�s avoir avanc� d'un pas de lookahead pour r�cup�rer c

      if ((c < '0' || c > '9') && c != '#' /*c'est probablement un dag normal*/)
	dag_kind = DAG_KIND;
      else
	dag_kind = UDAG_KIND;
    }
  }

  dag_scanner (SXBEGIN, NULL /* dummy_tables_ptr */);
  dag_scanner (SXOPEN, NULL);
  dag_scanner (SXINIT, NULL);

#if 0
  severity = 
#endif /* 0 */
    dag_scanner (SXACTION, NULL);

  dag_scanner (SXFINAL, NULL);
  sxsrc_mngr (SXFINAL);
  dag_scanner (SXCLOSE, NULL);
  dag_scanner (SXEND, NULL);

  if (infile)
    fclose (infile);
#if 0
  return severity;
#endif /* 0 */
}



/************************************************************************/
/* main function */
/************************************************************************/
int
sxearley_main (int argc, char *argv[])
{
  int		argnum;
  bool	in_options, is_source_file, is_stdin;

  sxinitialise (is_source_file); /* pour faire taire gcc -Wuninitialized */

  if (sxtty_is_stderr) {
    if (sxstderr != NULL)
       sxtty = sxstderr;
    else
       sxtty = stderr;
  }
  sxopentty ();

  /* valeurs par defaut */
  sxverbosep = false;
  is_print_time = true;
  is_print_prod = false;
  is_no_semantics = false;
  is_parse_tree_number = false;
  is_default_semantics = true; /* Semantique specifiee avec la grammaire */
  is_help = false;
  is_parse_forest = false;
  tmp_file_for_stdin = false;

  is_stdin = true;

  dag_kind = 0;

  default_set = 0; /* Peut contenir CHECK_UPPER|CHECK_LOWER */
  ciu_name_ptr = "__ANY__"; /* Chaine de caractere par defaut (elle doit figurer ds le dico) des noms propres inconnus */
  lciu_name_ptr = "__ANY__"; /* Chaine de caractere par defaut (elle doit figurer ds le dico) des mots minuscules inconnus */
  digits_name_ptr = NULL; /* Chaine de caractere par defaut (elle doit figurer ds le dico) des nombres ecrits en chiffres */

  time_out = 0;
  vtime_out_s = vtime_out_us = 0;
  is_tagged_dag = false;
  is_tagged_sdag = false;

  beam_value_is_set = false;
  beam_value = -DBL_MAX;

  /* Options du lexicalizer */
  IS_CHECK_RHS_SEQUENCES = 0; /* -1 => pt fixe, 0 => rien, i>0 => i fois */
  IS_DYNAMIC_SET_AUTOMATON = false;
  maximum_input_size = ~((SXUINT) 0); /* unbounded */

  /* On fait un 1er passage pour reperer NO_SEM ou PARSE_TREE_NUMBER car ces options modifient sxearley_open_for_semact */
  if (argc > 1) {
    /* Decodage des options */
    argnum = 0;

    while (++argnum < argc) {
      switch (option_get_kind (argv [argnum])) {
      case NO_SEM:
	is_no_semantics = true;
	is_default_semantics = false;
	is_parse_tree_number = false;
	break;

      case PARSE_TREE_NUMBER:
	is_no_semantics = false;
	is_default_semantics = false;
	is_parse_tree_number = true;
	is_parse_forest = true;
	break;

      default:
	break;
      }
    }
  }

  /* Cette procedure de l'analyseur earley appelle eventuellement une partie initialisation de la semantique qui
     peut initialiser des fonctions ds for_semact */
  sxearley_set_for_semact ();

  if (argc > 1) {
    is_source_file = false;

    /* Decodage des options */
    in_options = true;
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
	is_parse_forest = true;
	break;

      case TIME:
	is_print_time = true;
	break;

      case -TIME:
	is_print_time = false;
	break;

      case PRINT_PROD:
	is_print_prod = true;
	break;

      case HELP:
	is_help = true;
	break;

      case TRY_LOWER:
	default_set |= CHECK_LOWER;
	break;

      case TRY_UPPER:
	default_set |= CHECK_UPPER;
	break;

      case CAPITALIZED_INITIAL_UNKNOWN_WORD:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a word must follow the \"%s\" option;\n", ME, option_get_text (CAPITALIZED_INITIAL_UNKNOWN_WORD));
	  sxexit (6);
	}

	ciu_name_ptr = argv [argnum];
	break;

      case LOWER_CASE_INITIAL_UNKNOWN_WORD:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a word must follow the \"%s\" option;\n", ME, option_get_text (LOWER_CASE_INITIAL_UNKNOWN_WORD));
	  sxexit (6);
	}

	lciu_name_ptr = argv [argnum];
	break;

      case DIGIT_SEQUENCE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a word must follow the \"%s\" option;\n", ME, option_get_text (DIGIT_SEQUENCE));
	  sxexit (6);
	}

	digits_name_ptr = argv [argnum];
	break;

      case STDIN:
	is_stdin = true;
	break;

      case PARSE_FOREST:
	is_parse_forest = true;
	break;

      case TIME_OUT:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (TIME_OUT));
	  sxexit (6);
	}

	time_out = atoi (argv [argnum]);
	
	break;

      case VIRTUAL_TIME_OUT:
	{
	  SXINT i;
	  char digit, *suffix;
	  SXINT pos;

	  if (++argnum >= argc) {
	    fprintf (sxstderr, "%s: a number (digits[.digits]) must follow the \"%s\" option;\n", ME, option_get_text (VIRTUAL_TIME_OUT));
	    sxexit (6);
	  }

	  vtime_out_s = strtol (argv [argnum], &suffix, 10);

	  if (*suffix == '.') {
	    pos = 100000;
	    i = 0;

	    while (++i) {
	      digit = suffix [i];

	      if (digit < '0' || digit > '9') {
		suffix += i;
		break;
	      }

	      vtime_out_us += (digit - '0') * pos;
	      pos /= 10;
	    }
	  }

	  if (*suffix != SXNUL) {
	    fprintf (sxstderr, "%s: \"%s\" must be a number (digits[.digits]);\n", ME, argv [argnum]);
	    sxexit (6);
	  }
	}
	
	break;

      case TAGGED_DAG:
	is_tagged_dag = true;
	break;

      case TAGGED_SDAG:
	is_tagged_sdag = true;
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

      case CHECK_RHS_SEQUENCES:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (CHECK_RHS_SEQUENCES));
	  sxexit (6);
	}

	IS_CHECK_RHS_SEQUENCES = atoi (argv [argnum]);
	break;

      case DYNAMIC_SET_AUTOMATON:
	IS_DYNAMIC_SET_AUTOMATON = true;
	break;

      case BEAM_VALUE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a double greater or equal than 1 must follow the \"%s\" option;\n", ME, option_get_text (BEAM_VALUE));
	  sxexit (6);
	}

	beam_value = atof (argv [argnum]);

	if (beam_value < 1) {
	  beam_value = -DBL_MAX;
	  beam_value_is_set = false;
	}
	else {
	  beam_value = -log10 (beam_value);
	  beam_value_is_set = true;
	}

	break;

      case MAX_INPUT_SIZE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (MAX_INPUT_SIZE));
	  sxexit (6);
	}

	maximum_input_size = (SXUINT) atol (argv [argnum]);
	break;

      case TMP_FILE_FOR_STDIN:
	tmp_file_for_stdin = true;
	break;

      case SOURCE_FILE:
	if (is_stdin) {
	  is_stdin = false;
	}

	is_source_file = true;
	in_options = false;
	break;

      case UNKNOWN_ARG:
	if (!for_semact.process_args || !for_semact.process_args (&argnum, argc, argv)) {
	  fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
	  fprintf (sxstderr, Usage, (for_semact.ME) ? (*for_semact.ME)() : ME, (for_semact.string_args) ? (*for_semact.string_args)() : "");
	  sxexit (3);
	}
	
	/* arguments pris par la semantique */
	break;
      default:
	sxtrap (ME, "main");
      }
    }
  }

  if ((!is_stdin && !is_source_file) || is_help) {
    fprintf (sxstderr, Usage, (for_semact.ME) ? (*for_semact.ME)() : ME, (for_semact.string_args) ? (*for_semact.string_args)() : "");
    sxexit (3);
  }

  if (is_stdin) {
    earley_run (NULL);
  }
  else {
    do {
      earley_run (argv [argnum++]);
    } while (argnum < argc);
  }

  sxexit (sxerr_max_severity ());
  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}


