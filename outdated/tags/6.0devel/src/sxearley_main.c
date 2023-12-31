/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/

/* Main module for an Earley-like parser built by csynt_lc, the left-corner SYNTAX constructor */





#include "sxversion.h"
#include "sxunix.h"
#include "earley.h"
#include "udag_scanner.h"
#include <float.h>
#include <math.h>

char WHAT_SXEARLEY_MAIN[] = "@(#)SYNTAX - $Id: sxearley_main.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

static char	ME [] = "sxearley_main";

extern SXUINT          maximum_input_size; /* Pour udag_scanner definie ds earley_parser */

#if 0
#ifdef dummy_tables
/* Il y a des tables SYNTAX ds le coup */
extern struct sxtables	dummy_tables;
static struct sxtables	*dummy_tables_ptr = {&dummy_tables};
#else /* dummy_tables */
static struct sxtables	*dummy_tables_ptr = {NULL};
#endif /* dummy_tables */
#endif /* 0 */


/*---------------*/
/*    options    */
/*---------------*/

extern SXBOOLEAN tmp_file_for_stdin; /* Ds earley_parser.c */
extern SXBOOLEAN sxtty_is_stderr;

static SXBOOLEAN  is_help;
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
\t\t-scanb recognizer_scan_beam_value=real, -recognizer_scan_beam recognizer_scan_beam_value=real\n\
\t\t-scanb2 alternative_recognizer_scan_beam_value=real, -alternative_recognizer_scan_beam alternative_recognizer_scan_beam_value=real\n\
\t\t-shiftb recognizer_shift_beam_value=real, -recognizer_shift_beam recognizer_shift_beam_value=real\n\
\t\t-scanrcvr scan_rcvr_beam_delta=real, -recognizer_scan_rcvr_beam_delta scan_rcvr_beam_delta=real\n\
\t\t-shiftrcvr shift_rcvr_beam_delta=real, -recognizer_shift_rcvr_beam_delta shift_rcvr_beam_delta=real\n\
\t\t-redb reducer_beam_value=real, -reducer_beam reducer_beam_value=real\n\
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
#define RECOGNIZER_SCAN_BEAM_VALUE    25
#define ALT_RECOGNIZER_SCAN_BEAM_VALUE    26
#define RECOGNIZER_SHIFT_BEAM_VALUE    27
#define REDUCER_BEAM_VALUE    28
#define SCAN_RCVR_BEAM_VALUE  29
#define SHIFT_RCVR_BEAM_VALUE  30
#define MAX_INPUT_SIZE    31
#define TMP_FILE_FOR_STDIN                       32
#define SOURCE_FILE	  33


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
    "scanb", "recognizer_scan_beam",
    "scanb2", "alternative_recognizer_scan_beam",
    "shiftb", "recognizer_shift_beam",
    "redb", "reducer_beam",
    "scanrcvr", "recognizer_scan_rcvr_beam_delta",
    "shiftrcvr", "recognizer_shift_rcvr_beam_delta",
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
    RECOGNIZER_SCAN_BEAM_VALUE, RECOGNIZER_SCAN_BEAM_VALUE,
    ALT_RECOGNIZER_SCAN_BEAM_VALUE, ALT_RECOGNIZER_SCAN_BEAM_VALUE,
    RECOGNIZER_SHIFT_BEAM_VALUE, RECOGNIZER_SHIFT_BEAM_VALUE,
    REDUCER_BEAM_VALUE, REDUCER_BEAM_VALUE,
    SCAN_RCVR_BEAM_VALUE, SCAN_RCVR_BEAM_VALUE,
    SHIFT_RCVR_BEAM_VALUE, SHIFT_RCVR_BEAM_VALUE,    
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

    source_file_name = "<stdin>";

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
      sxsrc_mngr (SXINIT, infile, source_file_name);
    } else
      sxsrc_mngr (SXINIT, stdin, source_file_name);
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
    SXSHORT c;

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
  SXBOOLEAN	in_options, is_source_file, is_stdin;
  SXFLOAT       tmp_value;

  sxinitialise (is_source_file); /* pour faire taire gcc -Wuninitialized */

  if (sxtty_is_stderr) {
    if (sxstderr != NULL)
       sxtty = sxstderr;
    else
       sxtty = stderr;
  }
  sxopentty ();

  /* valeurs par defaut */
  sxverbosep = SXFALSE;
  is_print_time = SXTRUE;
  is_print_prod = SXFALSE;
  is_no_semantics = SXFALSE;
  is_parse_tree_number = SXFALSE;
  is_default_semantics = SXTRUE; /* Semantique specifiee avec la grammaire */
  is_help = SXFALSE;
  is_parse_forest = SXFALSE;
  tmp_file_for_stdin = SXFALSE;

  is_stdin = SXTRUE;

  dag_kind = 0;

  default_set = 0; /* Peut contenir CHECK_UPPER|CHECK_LOWER */
  ciu_name_ptr = "__ANY__"; /* Chaine de caractere par defaut (elle doit figurer ds le dico) des noms propres inconnus */
  lciu_name_ptr = "__ANY__"; /* Chaine de caractere par defaut (elle doit figurer ds le dico) des mots minuscules inconnus */
  digits_name_ptr = NULL; /* Chaine de caractere par defaut (elle doit figurer ds le dico) des nombres ecrits en chiffres */

  time_out = 0;
  vtime_out_s = vtime_out_us = 0L;
  is_tagged_dag = SXFALSE;
  is_tagged_sdag = SXFALSE;

  recognizer_scan_beam_value_is_set = SXFALSE;
  recognizer_beam_type = SXFALSE;
  recognizer_scan_beam_value = SXLOGPROB_MIN;
  recognizer_shift_beam_value = SXLOGPROB_MIN;
  recognizer_shift_beam_value_is_set = SXFALSE;
  reducer_beam_value = SXLOGPROB_MIN;
  reducer_beam_value_is_set = SXFALSE;
  scan_rcvr_beam_delta_is_set = SXFALSE;
  shift_rcvr_beam_delta_is_set = SXFALSE;
  
  /* Options du lexicalizer */
  IS_CHECK_RHS_SEQUENCES = 0; /* -1 => pt fixe, 0 => rien, i>0 => i fois */
  IS_DYNAMIC_SET_AUTOMATON = SXFALSE;
  maximum_input_size = ~((SXUINT) 0); /* unbounded */

  /* On fait un 1er passage pour reperer NO_SEM ou PARSE_TREE_NUMBER car ces options modifient sxearley_open_for_semact */
  if (argc > 1) {
    /* Decodage des options */
    argnum = 0;

    while (++argnum < argc) {
      switch (option_get_kind (argv [argnum])) {
      case NO_SEM:
	is_no_semantics = SXTRUE;
	is_default_semantics = SXFALSE;
	is_parse_tree_number = SXFALSE;
	break;

      case PARSE_TREE_NUMBER:
	is_no_semantics = SXFALSE;
	is_default_semantics = SXFALSE;
	is_parse_tree_number = SXTRUE;
	is_parse_forest = SXTRUE;
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
    is_source_file = SXFALSE;

    /* Decodage des options */
    in_options = SXTRUE;
    argnum = 0;

    while (in_options && ++argnum < argc) {
      switch (option_get_kind (argv [argnum])) {
      case VERBOSE:
	sxverbosep = SXTRUE;
	break;

      case -VERBOSE:
	sxverbosep = SXFALSE;
	break;

      case NO_SEM:
	is_no_semantics = SXTRUE;
	is_default_semantics = SXFALSE;
	is_parse_tree_number = SXFALSE;
	break;

      case DEFAULT_SEM:
	is_no_semantics = SXFALSE;
	is_default_semantics = SXTRUE;
	is_parse_tree_number = SXFALSE;
	break;

      case PARSE_TREE_NUMBER:
	is_no_semantics = SXFALSE;
	is_default_semantics = SXFALSE;
	is_parse_tree_number = SXTRUE;
	is_parse_forest = SXTRUE;
	break;

      case TIME:
	is_print_time = SXTRUE;
	break;

      case -TIME:
	is_print_time = SXFALSE;
	break;

      case PRINT_PROD:
	is_print_prod = SXTRUE;
	break;

      case HELP:
	is_help = SXTRUE;
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
	  SXEXIT (6);
	}

	ciu_name_ptr = argv [argnum];
	break;

      case LOWER_CASE_INITIAL_UNKNOWN_WORD:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a word must follow the \"%s\" option;\n", ME, option_get_text (LOWER_CASE_INITIAL_UNKNOWN_WORD));
	  SXEXIT (6);
	}

	lciu_name_ptr = argv [argnum];
	break;

      case DIGIT_SEQUENCE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a word must follow the \"%s\" option;\n", ME, option_get_text (DIGIT_SEQUENCE));
	  SXEXIT (6);
	}

	digits_name_ptr = argv [argnum];
	break;

      case STDIN:
	is_stdin = SXTRUE;
	break;

      case PARSE_FOREST:
	is_parse_forest = SXTRUE;
	break;

      case TIME_OUT:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (TIME_OUT));
	  SXEXIT (6);
	}

	time_out = atoi (argv [argnum]);
	
	break;

      case VIRTUAL_TIME_OUT:
	{
	  SXINT i;
	  char digit, *suffix;
	  long pos;

	  if (++argnum >= argc) {
	    fprintf (sxstderr, "%s: a number (digits[.digits]) must follow the \"%s\" option;\n", ME, option_get_text (VIRTUAL_TIME_OUT));
	    SXEXIT (6);
	  }

	  /* long int strtol(const char *nptr, char **endptr, SXINT base); */
	  vtime_out_s = strtol (argv [argnum], &suffix, 10);

	  if (*suffix == '.') {
	    pos = 100000L;
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
	    SXEXIT (6);
	  }
	}
	
	break;

      case TAGGED_DAG:
	is_tagged_dag = SXTRUE;
	break;

      case TAGGED_SDAG:
	is_tagged_sdag = SXTRUE;
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
	  SXEXIT (6);
	}

	IS_CHECK_RHS_SEQUENCES = atoi (argv [argnum]);
	break;

      case DYNAMIC_SET_AUTOMATON:
	IS_DYNAMIC_SET_AUTOMATON = SXTRUE;
	break;

      case RECOGNIZER_SCAN_BEAM_VALUE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a real greater or equal than 1 must follow the \"%s\" option;\n", ME, option_get_text (RECOGNIZER_SCAN_BEAM_VALUE));
	  SXEXIT (6);
	}

	tmp_value = sxatof (argv [argnum]);

	if (tmp_value < 1) {
	  recognizer_scan_beam_value = SXLOGPROB_MIN;
	  recognizer_scan_beam_value_is_set = SXFALSE;
	}
	else {
	  recognizer_scan_beam_value = -proba2SXLOGPROB (tmp_value);
	  recognizer_scan_beam_value_is_set = SXTRUE;
	}

	break;

      case ALT_RECOGNIZER_SCAN_BEAM_VALUE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a real greater or equal than 1 must follow the \"%s\" option;\n", ME, option_get_text (ALT_RECOGNIZER_SCAN_BEAM_VALUE));
	  SXEXIT (6);
	}

	tmp_value = sxatof (argv [argnum]);

	if (tmp_value < 1) {
	  recognizer_scan_beam_value = SXLOGPROB_MIN;
	  recognizer_scan_beam_value_is_set = SXFALSE;
	}
	else {
	  recognizer_scan_beam_value = -proba2SXLOGPROB (tmp_value);
	  recognizer_scan_beam_value_is_set = SXTRUE;
	  recognizer_beam_type = SXTRUE;
	}

	break;

      case RECOGNIZER_SHIFT_BEAM_VALUE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a real greater or equal than 1 must follow the \"%s\" option;\n", ME, option_get_text (RECOGNIZER_SHIFT_BEAM_VALUE));
	  SXEXIT (6);
	}

	tmp_value = sxatof (argv [argnum]);

	if (tmp_value < 1) {
	  recognizer_shift_beam_value = SXLOGPROB_MIN;
	  recognizer_shift_beam_value_is_set = SXFALSE;
	}
	else {
	  recognizer_shift_beam_value = -proba2SXLOGPROB (tmp_value);
	  recognizer_shift_beam_value_is_set = SXTRUE;
	}

	break;

      case REDUCER_BEAM_VALUE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a real greater or equal than 1 must follow the \"%s\" option;\n", ME, option_get_text (REDUCER_BEAM_VALUE));
	  SXEXIT (6);
	}

	tmp_value = sxatof (argv [argnum]);

	if (tmp_value < 1) {
	  reducer_beam_value = SXLOGPROB_MIN;
	  reducer_beam_value_is_set = SXFALSE;
	}
	else {
	  reducer_beam_value = -proba2SXLOGPROB (tmp_value);
	  reducer_beam_value_is_set = SXTRUE;
	}

	break;

      case SCAN_RCVR_BEAM_VALUE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a real greater or equal than 1 must follow the \"%s\" option;\n", ME, option_get_text (SCAN_RCVR_BEAM_VALUE));
	  SXEXIT (6);
	}

	tmp_value = sxatof (argv [argnum]);

	if (tmp_value >= 1) {
	  scan_rcvr_beam_delta = -proba2SXLOGPROB (tmp_value);
	  scan_rcvr_beam_delta_is_set = SXTRUE;
	}

	break;

      case SHIFT_RCVR_BEAM_VALUE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: a real greater or equal than 1 must follow the \"%s\" option;\n", ME, option_get_text (SHIFT_RCVR_BEAM_VALUE));
	  SXEXIT (6);
	}

	tmp_value = sxatof (argv [argnum]);

	if (tmp_value >= 1) {
	  shift_rcvr_beam_delta = -proba2SXLOGPROB (tmp_value);
	  shift_rcvr_beam_delta_is_set = SXTRUE;
	}

	break;

      case MAX_INPUT_SIZE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (MAX_INPUT_SIZE));
	  SXEXIT (6);
	}

	maximum_input_size = (SXUINT) atol (argv [argnum]);
	break;

      case TMP_FILE_FOR_STDIN:
	tmp_file_for_stdin = SXTRUE;
	break;

      case SOURCE_FILE:
	if (is_stdin) {
	  is_stdin = SXFALSE;
	}

	is_source_file = SXTRUE;
	in_options = SXFALSE;
	break;

      case UNKNOWN_ARG:
	if (!for_semact.process_args || !for_semact.process_args (&argnum, argc, argv)) {
	  fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
	  fprintf (sxstderr, Usage, (for_semact.ME) ? (*for_semact.ME)() : ME, (for_semact.string_args) ? (*for_semact.string_args)() : "");
	  SXEXIT (3);
	}
	
	/* arguments pris par la semantique */
	break;
      default:
	sxtrap (ME, "main");
      }
    }
  }

  can_try_recognizer_beamrcvr = (recognizer_scan_beam_value_is_set && scan_rcvr_beam_delta_is_set)
	    || (recognizer_shift_beam_value_is_set && shift_rcvr_beam_delta_is_set);

  if ((!is_stdin && !is_source_file) || is_help) {
    fprintf (sxstderr, Usage, (for_semact.ME) ? (*for_semact.ME)() : ME, (for_semact.string_args) ? (*for_semact.string_args)() : "");
    SXEXIT (3);
  }

  if (is_stdin) {
    earley_run (NULL);
  }
  else {
    do {
      earley_run (argv [argnum++]);
    } while (argnum < argc);
  }

  SXEXIT (sxerr_max_severity ());
  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}


