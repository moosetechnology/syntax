/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1996 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe ATOLL.			  *
   *							  *
   ******************************************************** */





/* Main module for an Earley-like parser built by csynt_lc, the left-corner SYNTAX constructor */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20060426 16:25 (bs)          Exploitation de la capture de timeout   */
/************************************************************************/
/* 20060118 13:15 (pb):	        Marche avec is_dag et !is_dag		*/
/*                              ... et les options de la semantique sont*/
/*                              traitees par la semantique              */
/************************************************************************/
/* 20030319 10:48 (phd):	Utilisation de "sxtimestamp"		*/
/************************************************************************/
/* 6 mai 2002 10:00 (pb):	Traitement des "sdag"                   */
/************************************************************************/
/* Aug 20 1996 14:17 (pb):	Ajout de cette rubrique "modifications" */
/************************************************************************/



#define SX_DFN_EXT_VAR

#include "sxunix.h"
#include "earley.h"
#include "udag_scanner.h"

char WHAT_SXEARLEY_MAIN[] = "@(#)SYNTAX - $Id: sxearley_main.c 1130 2008-03-05 12:11:19Z syntax $" WHAT_DEBUG;

static char	ME [] = "earley_main";

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

#ifdef dummy_tables
/* Il y a des tables SYNTAX ds le coup */
extern struct sxtables	dummy_tables;
static struct sxtables	*dummy_tables_ptr = {&dummy_tables};
#else /* dummy_tables */
static struct sxtables	*dummy_tables_ptr = {NULL};
#endif /* dummy_tables */

BOOLEAN        (*main_parser)(void) = sxearley_parse_it;

/*---------------*/
/*    options    */
/*---------------*/


BOOLEAN		sxverbosep;

static BOOLEAN  is_help;
static SXINT    time_out;
static long     vtime_out_s, vtime_out_us;
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
\t\t-mis size, -max_input_size size,\n\
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
#define MAX_INPUT_SIZE    25
#define SOURCE_FILE	  26


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
    "mis", "max_input_size",
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
    MAX_INPUT_SIZE, MAX_INPUT_SIZE,
};



static SXINT
option_get_kind (char *arg)
{
  char	**opt;

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
    sxsrc_mngr (INIT, infile, "");

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

      sxsrc_mngr (INIT, infile, pathname);
    }
    
    source_file_name = pathname;
  }

  source_file = infile;
    
  if (time_out) {
    sxcaught_timeout (time_out,for_semact.timeout_mngr);
  }

  if (vtime_out_s+vtime_out_us) {
    sxcaught_virtual_timeout (vtime_out_s, vtime_out_us, for_semact.vtimeout_mngr);
  }

  {
    SHORT c;

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

  dag_scanner (BEGIN, dummy_tables_ptr);
  dag_scanner (OPEN, NULL);
  dag_scanner (INIT, NULL);

  severity = dag_scanner (ACTION, NULL);

  dag_scanner (FINAL, NULL);
  sxsrc_mngr (FINAL);
  dag_scanner (CLOSE, NULL);
  dag_scanner (END, NULL);

  fclose (infile);
#if 0
  return severity;
#endif /* 0 */
}



/************************************************************************/
/* main function
/************************************************************************/
int
main (int argc, char *argv[])
{
  SXINT		argnum;
  BOOLEAN	in_options, is_source_file, is_stdin;
  SXINT           more_argc;
  char          **more_argv;

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }
  if (sxstderr == NULL) {
    sxstderr = stderr;
  }
  
#if BUG
  /* Suppress bufferisation, in order to have proper	 */
  /* messages when something goes wrong...		 */
  setbuf (stdout, NULL);
#endif
  
  sxopentty ();

  /* valeurs par defaut */
  sxverbosep = TRUE;
  is_print_time = TRUE;
  is_print_prod = FALSE;
  is_no_semantics = FALSE;
  is_parse_tree_number = FALSE;
  is_default_semantics = TRUE; /* Semantique specifiee avec la grammaire */
  is_help = FALSE;
  is_parse_forest = FALSE;

  is_stdin = TRUE;

  dag_kind = 0;

  default_set = 0; /* Peut contenir CHECK_UPPER|CHECK_LOWER */
  ciu_name_ptr = NULL; /* Chaine de caractere par defaut (elle doit figurer ds le dico) des noms propres inconnus */
  lciu_name_ptr = NULL; /* Chaine de caractere par defaut (elle doit figurer ds le dico) des mots minuscules inconnus */
  digits_name_ptr = NULL; /* Chaine de caractere par defaut (elle doit figurer ds le dico) des nombres ecrits en chiffres */

  time_out = 0;
  vtime_out_s = vtime_out_us = 0L;
  is_tagged_dag = FALSE;
  is_tagged_sdag = FALSE;

  /* Options du lexicalizer */
  IS_CHECK_RHS_SEQUENCES = 0; /* -1 => pt fixe, 0 => rien, i>0 => i fois */
  IS_DYNAMIC_SET_AUTOMATON = FALSE;
  maximum_input_size = ~0; /* unbounded */

  /* On fait un 1er passage pour reperer NO_SEM ou PARSE_TREE_NUMBER car ces options modifient sxearley_init_for_semact */
  if (argc > 1) {
    /* Decodage des options */
    argnum = 0;

    while (++argnum < argc) {
      switch (option_get_kind (argv [argnum])) {
      case NO_SEM:
	is_no_semantics = TRUE;
	is_default_semantics = FALSE;
	is_parse_tree_number = FALSE;
	break;

      case PARSE_TREE_NUMBER:
	is_no_semantics = FALSE;
	is_default_semantics = FALSE;
	is_parse_tree_number = TRUE;
	is_parse_forest = TRUE;
	break;

      default:
	break;
      }
    }
  }

  /* Cette procedure de l'analyseur earley appelle eventuellement une partie initialisation de la semantique qui
     peut initialiser des fonctions ds for_semact */
  sxearley_init_for_semact ();

  if (argc > 1) {
    is_source_file = FALSE;

    /* Decodage des options */
    in_options = TRUE;
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
	is_parse_forest = TRUE;
	break;

      case TIME:
	is_print_time = TRUE;
	break;

      case -TIME:
	is_print_time = FALSE;
	break;

      case PRINT_PROD:
	is_print_prod = TRUE;
	break;

      case HELP:
	is_help = TRUE;
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
	is_stdin = TRUE;
	break;

      case PARSE_FOREST:
	is_parse_forest = TRUE;
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

	  if (*suffix != NUL) {
	    fprintf (sxstderr, "%s: \"%s\" must be a number (digits[.digits]);\n", ME, argv [argnum]);
	    SXEXIT (6);
	  }
	}
	
	break;

      case TAGGED_DAG:
	is_tagged_dag = TRUE;
	break;

      case TAGGED_SDAG:
	is_tagged_sdag = TRUE;
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
	IS_DYNAMIC_SET_AUTOMATON = TRUE;
	break;

      case MAX_INPUT_SIZE:
	if (++argnum >= argc) {
	  fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (MAX_INPUT_SIZE));
	  SXEXIT (6);
	}

	maximum_input_size = atoi (argv [argnum]);
	break;

      case SOURCE_FILE:
	if (is_stdin) {
	  is_stdin = FALSE;
	}

	is_source_file = TRUE;
	in_options = FALSE;
	break;

      case UNKNOWN_ARG:
	if (!for_semact.process_args || !for_semact.process_args (&argnum, argc, argv)) {
	  fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
	  fprintf (sxstderr, Usage, (for_semact.ME) ? (*for_semact.ME)() : ME, (for_semact.string_args) ? (*for_semact.string_args)() : "");
	  SXEXIT (3);
	}
	
	/* arguments pris par la semantique */
	break;
      }
    }
  }

  if (!is_stdin && !is_source_file || is_help) {
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

  {
    SXINT	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
      ;

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



#if 0
SXINT
get_SEMLEX_lahead (void)
{
  return SEMLEX_lahead;
}

#if is_dag
/* Le source est un vrai dag */
/* sxearley_parser() a ete mis ds sxparser_earley_lc.h */
#else /* !is_dag */
/* cas !is_dag && (is_sdag || !is_sdag) */
/* Cette fonction, appelee depuis l'error repair de earley fabrique un token
   "genere" par l'error repair, comme s'il provenait du source */

SXINT
get_toks_buf_size (void)
{
  return sxplocals.Mtok_no;
}

#if 0
struct sxtoken* tok_no2tok (tok_no)
     SXINT tok_no;
{  
  return  &SXGET_TOKEN (tok_no);
}
#endif

SXINT put_repair_tok (SXINT t,
		      char  *t_str,
		      char  *comment_str,
		      struct sxsource_coord *psource_index)
{
  struct sxtoken token;

  token.comment = comment_str;
  token.string_table_entry = sxstr2save (t_str, strlen (t_str));
  token.lahead = t;
  token.source_index = *psource_index;

  sxput_token (token);

  return sxplocals.Mtok_no;
}


#if 0
static SXBA *glbl_source_area;
#endif /* 0 */

#if is_sdag

static VOID	gripe ()
{
    fputs ("\nA function of \"sdag\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}


/* Le 17/11/05 les grammaires des sources doivent avoir la forme :
<axiom> = <S> ;
<T> = "..." ;
...
<T> = "..." ;
<TL> = ;
<TL> = <TL1> ;
<TL> = <TL1> %SEMLEX ; 5
<TL1> = <TL> <F> ; 1
<F> = <T> ;
<F> = %MOT ;
<FF> = %MOT ; 2
<FF> = <T> ; 3
<FF> = ; 4
<S> = <S> <L> ;
<S> = ;
<L> = <FF> ( <TL> ) ;
*/

SXINT
sdag_semact (code, numact)
    SXINT		code;
    SXINT		numact;
{
  static SXINT	tok_no, tok, t_nb, tmax;
  SXINT		lahead, top, single;
  BOOLEAN	ret_val, oversized;
  char		mess [32];

  switch (code) {
  case OPEN:
  case CLOSE:
  case SEMPASS:
  case ERROR:
    return 0;
  case INIT:
    if (is_print_time)
      sxtime (INIT, NULL);

    t_nb = 0;
	
    n = 0;
    tok_no = 1;

    do {
      (*(sxplocals.SXP_tables.scanit)) ();
      lahead = SXGET_TOKEN (sxplocals.Mtok_no).lahead;

      if (lahead == sxsvar.SXS_tables.S_termax-1) {
	/* C'est le ")" qui marque la fin la fin de l'association forme flechie -> terminaux */
	n++;
      }
	    
    } while (lahead != sxsvar.SXS_tables.S_termax);

    /* Le 17/11/05 */
    /* independance vis-a-vis de la specif d'un sdag */
    /* eof = get_eof_val (); /* pris ds les tables earley fini */
    eof = -spf.inputG.maxt; /* Le 14/09/06 */
    tmax = eof-1;

    alloc_ff_ste (); /* bidon si !is_supertagging */

    top = (n+2)*2;

    /* Le 9/1/02 ESSAI.  Le code de eof est passe a 0. 
       Ca permet de faire marcher les &First et &Last car eof est code' par 0 ds les first_%i_%i */
    /* Pour earley on revient a eof... */
    /* Finalement, on met les deux... */
    /* Ben non, car eof == 0 => pour le dernier token on recupere ds item_set_i2 tout t2item_set [0]
       qui est initialise' avec tous les items.  Donc aucun filtrage (C,a peut ne pas gagner de temps
       mais ce n'est pas faux --- si le main est une RCG, c'est correct */
    glbl_source_area = glbl_source = sxbm_calloc (top, eof+1);
    SXBA_1_bit (glbl_source [0], eof);
    /* SXBA_1_bit (glbl_source [0], 0); */
    SXBA_1_bit (glbl_source [n+1], eof);
    /* SXBA_1_bit (glbl_source [n+1], 0); */

    glbl_out_source = glbl_source+n+2;
	
    source_set = sxba_calloc (eof+1);
    /* On ajoute eof comme terminal rencontre' */
    SXBA_1_bit (source_set, eof);

    source_top = source_set + NBLONGS (BASIZE (source_set)); /* pointe vers la derniere "tranche" */

    /* Le 29/11/05 */
    X_alloc (&tok_noXtok, "tok_noXtok", sxplocals.Mtok_no+9, 1, NULL /* ne deborde jamais */, NULL);
    tok_noXtok2xbuf = (SXINT*) sxalloc (X_size (tok_noXtok), sizeof (SXINT));
    tok_noXtok2xbuf [0] = 0; /* c'est + propre */

    X_set (&tok_noXtok, SDAG_PACK(0, eof), &single); /* Le eof du debut */
    tok_noXtok2xbuf [single] = 0;
    /* bidon pour "ff", "(" et ")" */
    X_set (&tok_noXtok, SDAG_PACK (0, SDAG_FF), &single);
    tok_noXtok2xbuf [single] = 0;
    X_set (&tok_noXtok, SDAG_PACK (0, SDAG_LP), &single);
    tok_noXtok2xbuf [single] = 0;
    X_set (&tok_noXtok, SDAG_PACK (0, SDAG_RP), &single);
    tok_noXtok2xbuf [single] = 0;

    X_set (&tok_noXtok, SDAG_PACK(n+1,eof), &single); /* Le eof de fin */
    tok_noXtok2xbuf [single] = sxplocals.Mtok_no;
    /* bidon pour "ff", "(" et ")" */
    X_set (&tok_noXtok, SDAG_PACK (n+1, SDAG_FF), &single);
    tok_noXtok2xbuf [single] = sxplocals.Mtok_no;
    X_set (&tok_noXtok, SDAG_PACK (n+1, SDAG_LP), &single);
    tok_noXtok2xbuf [single] = sxplocals.Mtok_no;
    X_set (&tok_noXtok, SDAG_PACK (n+1, SDAG_RP), &single);
    tok_noXtok2xbuf [single] = sxplocals.Mtok_no;
    
    return 0;
  case FINAL:
    /* glbl_source est rempli, on peut lancer l'analyse */
    oversized = ((SXUINT) t_nb > maximum_input_size);
    
    if (is_print_time) {
      sprintf (mess, "\tScanner[%i/%i%s]", n, t_nb, oversized ? "(oversized)" : "");
      sxtime (ACTION, mess);
    }

    if (oversized)
      /* Le source depasse la taille maximale autorisee (le 19/04/07) */
      SXEXIT (7);

    /* Ajoute' le 26/09/06 !! */
    init_mlstn = 1;
    final_mlstn = n+1;

    ret_val = sxearley_parse_it ();

    sxbm_free (glbl_source_area), glbl_source_area = glbl_source = NULL;
    sxfree (source_set), source_set = NULL;

    X_free (&tok_noXtok);
    sxfree (tok_noXtok2xbuf), tok_noXtok2xbuf = NULL;

    return 0;

  case ACTION:
    switch (numact) {
    case 1:			/* <TL1> = <TL> <F> ; 1 */
      /* Le code de <T> est SXGET_TOKEN (sxplocals.atok_no-1).lahead */
      /* Il est associe a l'index tok_no */
      tok = SXGET_TOKEN (sxplocals.atok_no-1).lahead;

      if (SXBA_bit_is_reset_set (glbl_source [tok_no], tok)) {
	t_nb++;
      
	/* Le 29/11/05 il faut pouvoir faire des put_error sur tok.  On fait donc une correspondance
	   tok_no X tok --> xbuf (pour SXGET_TOKEN) */
	X_set (&tok_noXtok, SDAG_PACK (tok_no, tok), &single);
	tok_noXtok2xbuf [single] = sxplocals.atok_no-1;

	if (tok <= tmax)
	  SXBA_1_bit (source_set, tok);
      }
      else
	sxput_error (SXGET_TOKEN (sxplocals.atok_no-1).source_index,
		     "%sDuplicate terminal symbol: skipped",
		     sxplocals.sxtables->err_titles [1]);

      if (SXGET_TOKEN (sxplocals.atok_no).lahead == sxsvar.SXS_tables.S_termax-1) {
	/* Pour reperer la ")" */
	X_set (&tok_noXtok, SDAG_PACK (tok_no, SDAG_RP), &single);
	tok_noXtok2xbuf [single] = sxplocals.atok_no;

	tok_no++;
      }
	    
      return 0;

    case 2: /* <FF> = %MOT ; 2 */ 
    case 3: /* <FF> = <T> ; 3 */
    case 4: /* <FF> = ; 4 */
      supertag_act (numact, tok_no); /* Ds earley, bidon si on n'est pas ds le cas is_supertagging */

      /* Pour reperer la forme flechie */
      X_set (&tok_noXtok, SDAG_PACK (tok_no, SDAG_FF), &single);
      tok_noXtok2xbuf [single] = sxplocals.atok_no-1;

      /* Pour reperer la "(" */
      X_set (&tok_noXtok, SDAG_PACK (tok_no, SDAG_LP), &single);
      tok_noXtok2xbuf [single] = sxplocals.atok_no;

      if (SXGET_TOKEN (sxplocals.atok_no+1).lahead == sxsvar.SXS_tables.S_termax-1) {
	/* On est ds le cas <FF> { } */
	/* Mot inconnu, on met T */
	for (tok = 1; tok <= tmax; tok++) {
	  SXBA_1_bit (glbl_source [tok_no], tok), t_nb++;
	  SXBA_1_bit (source_set, tok);
	}
      
	/* Pour reperer la ")" */
	X_set (&tok_noXtok, SDAG_PACK (tok_no, SDAG_RP), &single);
	tok_noXtok2xbuf [single] = sxplocals.atok_no+1;
	  
	tok_no++;
      }

      return 0;

    case 5: /* <TL> = <TL1> %SEMLEX ; 5 */ /* Le 17/11/05 */
      /* On est ds le cas ff ( ... t [|...|]  ... ) */
      /* Le code du t est ds tok et l'indice de ff est ds tok_no */
      if (for_semact.scanact) {
	(*for_semact.scanact) (tok_no, tok, sxstrget (SXGET_TOKEN (sxplocals.atok_no-1).string_table_entry));
      }

      /* bidouille qui par l'intermediaire de get_SEMLEX_lahead permet de recuperer le code interne du terminal
	 %SEMLEX s'il y en a eu ds le source.  On peut donc savoir, pour un terminal donne' si le terminal
	 qui le suit est un %SEMLEX.  Utilise' lorsqu'on veut ressortir le source filtre' */
      if (SEMLEX_lahead == 0)
	/* 1ere fois */
	SEMLEX_lahead = SXGET_TOKEN (sxplocals.atok_no-1).lahead;

      if (SXGET_TOKEN (sxplocals.atok_no).lahead == sxsvar.SXS_tables.S_termax-1) {
	/* Pour reperer la ")" */
	X_set (&tok_noXtok, SDAG_PACK (tok_no, SDAG_RP), &single);
	tok_noXtok2xbuf [single] = sxplocals.atok_no;

	tok_no++;
      }
	    
      return 0;

    case 0:
      return 0;

    default:
      break;
    }

  default:
    gripe ();
  }
}

#else /* !is_sdag */
#if 0
static SXINT *glbl_source_area;
#endif /* 0 */

static BOOLEAN
call_sxparse_it (void)
{
    SXINT         tok;
    BOOLEAN	ret_val, oversized;

    oversized = ((SXUINT) n > maximum_input_size);

    if (is_print_time) {
      sprintf (mess, "\tScanner[%i%s]", n, oversized ? "(oversized)" : "");
      sxtime (ACTION, mess);
    }

    if (oversized)
      /* Le source depasse la taille maximale autorisee (le 19/04/07) */
      SXEXIT (7);

    glbl_source_area = glbl_source = (SXINT*) sxalloc (sxplocals.Mtok_no+1, sizeof (SXINT));
    source_set = sxba_calloc (eof+1);

    /* On ajoute eof comme terminal rencontre' */
    glbl_source [sxplocals.Mtok_no] = glbl_source [0] = eof;
    SXBA_1_bit (source_set, eof);

    source_top = source_set + NBLONGS (BASIZE (source_set)); /* pointe vers la derniere "tranche" */

    for (n = 1; n < sxplocals.Mtok_no; n++) {
	glbl_source [n] = tok = SXGET_TOKEN (n).lahead;
	SXBA_1_bit (source_set, tok);
    }

    n = sxplocals.Mtok_no-1;

    /* Ajoute' le 26/09/06 !! */
    init_mlstn = 1;
    final_mlstn = n+1;

    ret_val = sxearley_parse_it ();

    sxfree (glbl_source_area), glbl_source_area = glbl_source = NULL;
    sxfree (source_set), source_set = source_top = NULL;

#if 0
    if (is_print_time)
	sxtime (ACTION, "\tDone");
#endif /* 0 */

    return ret_val;
}


BOOLEAN
sxearley_parser (SXINT what_to_do, struct sxtables *arg)
{
  char mess [32];

  switch (what_to_do) {
  case BEGIN:
    return TRUE;

  case OPEN:

    sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, "sxearley_parser");

    sxplocals.sxtables = arg;
    sxplocals.SXP_tables = arg->SXP_tables;
    sxtkn_mngr (OPEN, 2);

#if 0
    (*sxplocals.SXP_tables.recovery) (OPEN);
#endif /* 0 */

    /* eof = get_eof_val (); fini */
    eof = -spf.inputG.maxt; /* Le 14/09/06 */

    return TRUE;

  case INIT:
    {
      struct sxtoken tok;

      sxtkn_mngr (INIT, 0);

      /* terminal_token EOF */
      tok.lahead = eof;
      tok.string_table_entry = EMPTY_STE;
      tok.source_index = sxsrcmngr.source_coord;
      tok.comment = NULL;
      /* Le token 0 est "EOF" */
      SXGET_TOKEN (0) = tok;
    }
    
#if 0
    /* analyse normale */
    /* valeurs par defaut qui peuvent etre changees ds les scan_act ou pars_act. */
    sxplocals.mode.look_back = 0; /* All tokens are kept. */
    sxplocals.mode.mode = SXPARSER;
    sxplocals.mode.kind = SXWITH_RECOVERY | SXWITH_CORRECTION;
    sxplocals.mode.local_errors_nb = 0;
    sxplocals.mode.global_errors_nb = 0;
    sxplocals.mode.is_prefixe = FALSE;
    sxplocals.mode.is_silent = FALSE;
    sxplocals.mode.with_semact = TRUE;
    sxplocals.mode.with_parsact = TRUE;
    sxplocals.mode.with_parsprdct = TRUE;
    sxplocals.mode.with_do_undo = FALSE;
#endif

    return TRUE;

  case ACTION:
    {
      SXINT	lahead;
      BOOLEAN	ret_val, store_print_time;

      if (is_print_time)
	sxtime (INIT, NULL);
	
#if TCUT
      /* L'option de compilation -DTCUT=t_code permet de couper l'analyse d'un source
	 en morceaux separes par t_code */
      ret_val = TRUE;
      store_print_time = is_print_time;
      is_print_time = FALSE;

      do {
	do {
	  (*(sxplocals.SXP_tables.scanit)) ();
	  lahead = SXGET_TOKEN (sxplocals.Mtok_no).lahead;
	} while (lahead != -tmax && lahead != TCUT);

	n = sxplocals.Mtok_no - 1;

	if (lahead == TCUT) {
	  SXGET_TOKEN (sxplocals.Mtok_no).lahead = -tmax /* c'est-�-dire EOF */;
	}

	ret_val &= call_sxparse_it ();

	if (lahead == TCUT)
	  sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;
      } while (lahead != -tmax);

      if (store_print_time) {
	is_print_time = TRUE;
	sxtime (ACTION, "\tTotal parse time");
      }
#else /* !TCUT */
      do {
	(*(sxplocals.SXP_tables.scanit)) ();
      } while (SXGET_TOKEN (sxplocals.Mtok_no).lahead != eof);

      n = sxplocals.Mtok_no - 1;

      ret_val = call_sxparse_it ();
#endif /* !TCUT */

      return ret_val;
    }

  case FINAL:
    sxtkn_mngr (FINAL, 0);
    return TRUE;

  case CLOSE:
    /* end of language: free the local arrays */
    sxtkn_mngr (CLOSE, 0);

#if 0
    (*sxplocals.SXP_tables.recovery) (CLOSE);
#endif /* 0 */

    return TRUE;

  case END:
    /* free everything */
    return TRUE;

  default:
    fprintf (sxstderr, "The function \"sxearley_parser\" is out of date with respect to its specification.\n");
    sxexit(1);
  }
}

#endif /* !is_sdag */

/* On a une transition sur t entre les mlstn p et q
   On retourne le tok_no tq toks_buf[tok_no] soit le token
   de cette transition */
/* Cette fonction est appelee depuis  spf_fill_Tij2tok_no du sxspf_mngr */
/* Il y en a une autre version ds le cas is_dag ds dag_scanner.c */
void
fill_Tij2tok_no (SXINT Tpq, SXINT maxTpq)
{
  SXINT t, p, q, result, tok_no = 0;

  t = -Tij2T (Tpq);
  /* Si t == 0 => mot inconnu */

#if EBUG
  if (t < 0)
    sxtrap (ME, "fill_Tij2tok_no");
#endif /* EBUG */

  p = Tij2i (Tpq);
  q = Tij2j (Tpq);

#if EBUG
  if (q != p+1)
    sxtrap (ME, "fill_Tij2tok_no");
#endif /* EBUG */

#if is_sdag
  tok_no = tok_noXtok2xbuf [X_is_set (&tok_noXtok, SDAG_PACK (p, t))]; /* Il faut utiliser SXGET_TOKEN () */
#else
  tok_no = p;
#endif /* !is_sdag */

  spf.outputG.Tij2tok_no [Tpq] = tok_no;

#if LOG
  {
    struct sxtoken *ptok1, *ptok2, *ptok3;

    ptok1 = &(SXGET_TOKEN (tok_no));

#if is_sdag
    ptok2 = &(SXGET_TOKEN (tok_noXtok2xbuf [X_is_set (&tok_noXtok, SDAG_PACK (p, SDAG_FF))])); /* la ff */
    ptok3 = &(SXGET_TOKEN (tok_no+1)); /* %SEMLEX eventuel */
    printf ("tok_no=%i, (local_comment=%s, source_index=[%i, %i], ff=%s, semlex=[|%s|])\n", tok_no,
	    ptok1->comment == NULL ? "" : ptok1->comment, ptok1->source_index.line, ptok1->source_index.column,
	    sxstrget (ptok2->string_table_entry), 
	    (ptok3->lahead == get_SEMLEX_lahead ()) ? sxstrget (ptok3->string_table_entry) : ""
	    );
#else
    printf ("tok_no=%i, (local_comment=%s, source_index=[%i, %i])\n", tok_no,
	    ptok1->comment == NULL ? "" : ptok1->comment, ptok1->source_index.line, ptok1->source_index.column
	    );
#endif /* !is_sdag */
  }
#endif /* LOG */
}


/* Le 18/12/06
   dag_or_nodag_source_processing ()
   a ete deplace ds lexicalizer_mngr.c
*/
#endif /* !is_dag */

#endif /* 0 */

/* Le 20/12/04 sxtime devient un module de la librairie  sxtm_mngr.c */

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

SXINT
sxivoid ()
/* procedure ne faisant rien */
{
  return 1;
}
