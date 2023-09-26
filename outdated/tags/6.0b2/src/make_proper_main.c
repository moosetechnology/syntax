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





/* Main module for "make_proper" a SYNTAX module which outputs a CFG in BNF format
   From both an input CFG (%include LC_TABLES_H) which has been produced by an
   execution of bnf -huge L.bnf > L_huge.h (L_huge.h and LC_TABLES_H are synonyms)
   and a source text which is a list of terminal symbols.  This list is converted into
   T' a subset of T (option -exclude).  The proper subgrammar whose set of terminals is T'
   is printed on stdout using the SYNTAX library module lexicalizer_mngr.c */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Dec 21 2006 10:30 (pb):	Ajout de cette rubrique "modifications" */
/************************************************************************/

static char	ME [] = "make_proper_main";

#define SX_DFN_EXT_VAR

#include "sxunix.h"
char WHAT_MAKE_PROPER_MAIN[] = "@(#)SYNTAX - $Id: make_proper_main.c 602 2007-05-31 07:18:28Z sagot $" WHAT_DEBUG;

#ifdef LC_TABLES_H
/* On compile les tables "left_corner" ... */
/* ... uniquement celles dont on va avoir besoin ds lexicalizer_mngr */

#define def_lispro
#define def_prolis
#define def_prolon
#define def_lhs
#define def_BVIDE
#define def_npg
#define def_numpg
#define def_npd
#define def_numpd
#define def_ntstring
#define def_tstring

#define def_multiple_t_item_set
#define def_inflected_form_names

#include LC_TABLES_H
#endif /* LC_TABLES_H */

/* Pour avoir la definition et l'initialisation de spf faite ici par sxspf.h */
#define SX_DFN_EXT_VAR2
#include "earley.h"
#include "sxspf.h"
#include "sxword.h"
#include "varstr.h"

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

extern struct sxtables  sxtables;
extern SXBA            basic_item_set; 
static VARSTR	       vstr;

/*---------------*/
/*    options    */
/*---------------*/


BOOLEAN	        sxverbosep;
static BOOLEAN	is_help, is_exclude, is_prod_mapping, is_terminal_mapping, is_non_terminal_mapping;

#define SXEOF   (-tmax)

static char	Usage [] = "\
Usage:\t%s [options] [file]\n\
options=\t--help,\n\
\t\t-v, -verbose,\n\
\t\t-e, -exclude,\n\
\t\t-pm, -prod_mapping,\n\
\t\t-tm, -terminal_mapping,\n\
\t\t-nm, -non_terminal_mapping,\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define HELP	          1
#define VERBOSE		  2
#define EXCLUDE		  3
#define PROD_MAPPING      4
#define TERMINAL_MAPPING  5
#define NON_TERMINAL_MAPPING  6
#define SOURCE_FILE	  7
#define STDIN	          8


static char	*option_tbl [] = {
    "",
    "-help",  
    "v", "verbose",
    "e", "exclude",
    "pm", "prod_mapping",
    "tm", "terminal_mapping",
    "nm", "non_terminal_mapping",
};

static int	option_kind [] = {
    UNKNOWN_ARG,
    HELP,
    VERBOSE, VERBOSE,
    EXCLUDE, EXCLUDE,
    PROD_MAPPING, PROD_MAPPING,
    TERMINAL_MAPPING, TERMINAL_MAPPING,
    NON_TERMINAL_MAPPING, NON_TERMINAL_MAPPING,
};



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



static void
make_proper_run (pathname)
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

      sxsrc_mngr (INIT, infile, pathname);
    }
  }

  sxerr_mngr (BEGIN);
  syntax (ACTION, &sxtables);
  sxerr_mngr (END);
  fclose (infile);
  sxsrc_mngr (FINAL);
}



/************************************************************************/
/* main function
/************************************************************************/
int main(int argc, char *argv[])
{
  int		argnum;
  BOOLEAN	is_source_file, is_stdin;
  char          *source_file;

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }
  if (sxstderr == NULL) {
    sxstderr = stderr;
  }
  
#ifdef BUG
  /* Suppress bufferisation, in order to have proper	 */
  /* messages when something goes wrong...		 */
  setbuf (stdout, NULL);
#endif
  
  sxopentty ();

  /* valeurs par defaut */
  sxverbosep = FALSE;
  is_help = FALSE;
  is_exclude = FALSE;
  is_prod_mapping = FALSE;
  is_terminal_mapping = FALSE;
  is_non_terminal_mapping = FALSE;
  is_stdin = TRUE;
  is_source_file = FALSE;
  
  argnum = 0;

  while (++argnum < argc) {
    switch (option_get_kind (argv [argnum])) {
    case HELP:
      is_help = TRUE;
      break;

    case VERBOSE:
      sxverbosep = TRUE;
      break;

    case EXCLUDE:
      is_exclude = TRUE;

    case PROD_MAPPING:
      is_prod_mapping = TRUE;
      break;

    case TERMINAL_MAPPING:
      is_terminal_mapping = TRUE;
      break;

    case NON_TERMINAL_MAPPING:
      is_non_terminal_mapping = TRUE;
      break;

    case STDIN:
      is_stdin = TRUE;
      break;

    case SOURCE_FILE:
      is_stdin = FALSE;
      is_source_file = TRUE;
      source_file = argv [argnum];
      break;

    case UNKNOWN_ARG:
      fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
      fprintf (sxstderr, Usage, ME);
      SXEXIT (3);
    }
  }

  if (!is_stdin && !is_source_file || is_help) {
    fprintf (sxstderr, Usage, ME);
    SXEXIT (3);
  }


  sxstr_mngr (BEGIN);
  (*(sxtables.analyzers.parser)) (BEGIN, &sxtables) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &sxtables) /* Initialisation de SYNTAX */ ;

  make_proper_run (is_stdin ? NULL : source_file);

  syntax (CLOSE, &sxtables);
  (*(sxtables.analyzers.parser)) (END, &sxtables);
  sxstr_mngr (END);

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
      ;

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

/* Utilisees ds lexicalizer_mngr */
static void
output_nt (int nt)
{
  printf ("<%s> ", spf.inputG.ntstring [nt]);
}


static void
output_t (int t)
{
  varstr_raz (vstr);
  vstr = varstr_quote (vstr, spf.inputG.tstring [t]);
  printf ("%s ", varstr_tostr (vstr));
}



void
output_prod (int prod)
{
  int        item, X;

  output_nt (spf.inputG.lhs [prod]);

  fputs ("\t= ", stdout);

  item = spf.inputG.prolon [prod];

  while ((X = spf.inputG.lispro [item++]) != 0) {
    if (X > 0)
      output_nt (X);
    else
      output_t (-X);
  }

  fputs (";\n", stdout);
}

/* "Semantique" de make_proper */
SXVOID
make_proper_semact (what, arg)
    int		what, arg;
{
  static int            word_pos, glbl_source_size;
  struct sxtoken	*tok;
  int                   t_code, new_prod;
  char                  *word;

  switch (what) {
  case OPEN:
    glbl_source_size = 100;
    glbl_source_area = glbl_source = (int*) sxalloc (glbl_source_size+1, sizeof (int));
    source_set = sxba_calloc (SXEOF+1);
    break;

  case INIT:
    sxword_reuse (&inflected_form_names, "inflected_form_names", sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
    word_pos = 0;
    break;

  case ACTION:
    if (arg == 0)
	return;

#if EBUG
    if (arg != 1)
      sxtrap (ME, "make_proper_semact");
#endif /* EBUG */

    if (++word_pos > glbl_source_size) {
      glbl_source_size *= 2;
      glbl_source_area = glbl_source = (int*) sxrealloc (glbl_source, glbl_source_size+1, sizeof (int));
    }

    tok = &(STACKtoken (STACKtop ()));
    word = sxstrget (tok->string_table_entry);
    t_code = sxword_retrieve (&inflected_form_names, word);

    if (t_code <= 1) {
      sxput_error (tok->source_index,
		   "%s: The unknown terminal symbol \"%s\" is ignored.",
		   sxsvar.sxtables->err_titles [1]);
      t_code = 0;
    }
    else {
      t_code--;
      SXBA_1_bit (source_set, t_code);
    }

    glbl_source [word_pos] = t_code;

    break;

  case SEMPASS:
    n = word_pos;
    init_mlstn = 1;
    final_mlstn = n+1;

    /*  On prepare l'appel de lexicalizer_mngr */
    if (is_exclude) {
      sxba_not (source_set); 
      SXBA_0_bit (source_set, 0);
    }
    else
      SXBA_1_bit (source_set, SXEOF);

    vstr = varstr_alloc (32); /* Pour output_t () !! */
      
    /* Attention, lexicalizer_mngr doit etre compile' avec l'option -DMAKE_PROPER */
    if (lexicalizer2basic_item_set (FALSE, FALSE)) {
      /* On sort la grammaire reduite contenue ds spf.insideG */
      int               new_prod, prod, i, X;
      extern char	*ctime ();
      long	        date_time;
    
      date_time = time (0);

      printf ("\
*   *****************************************************************************************\n\
*    This proper sub-grammar for the language \"%s\" in BNF format has been generated\n\
*    on %s\
*    by the SYNTAX(*) make_proper processor\n\
*   *****************************************************************************************\n\
*    (*) SYNTAX is a trademark of INRIA.\n\
*   *****************************************************************************************\n\n\n",
	      LANGUAGE_NAME, ctime (&date_time));

      for (new_prod = 1; new_prod <= spf.insideG.maxprod; new_prod++) {
	prod = spf.insideG.prod2init_prod [new_prod];

	if (is_prod_mapping)
	  printf ("*[P%i %i]\n", new_prod, prod);

	output_prod (prod);
      }

      if (is_non_terminal_mapping) {
	fputs ("\n* *********************** NON-TERMINAL MAPPING [new_nt_code -> old_nt_code] ***********************", stdout);

	for (i = 1; i <= spf.insideG.maxnt; i++) {
	  X = spf.insideG.nt2init_nt [i];
	  printf ("\n*[N%i %i] ", i, X);
	  output_nt (X);
	}

	fputs ("\n", stdout);
      }

      if (is_terminal_mapping) {
	fputs ("\n* *********************** TERMINAL MAPPING [new_t_code -> old_t_code] ***********************", stdout);

	for (i = 1; i < -spf.insideG.maxt; i++) {
	  X = spf.insideG.t2init_t [i];
	  printf ("\n*[T%i %i] ", i, X);
	  output_t (X);
	}

	fputs ("\n", stdout);
      }

      spf_free_insideG (&spf.insideG);
    }
    else {
      sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
	      "%sNo proper sub-grammar for the language \"%s\" can be constructed from the input terminal sub-set.",
	      sxplocals.sxtables->err_titles [2],
	      LANGUAGE_NAME);
    }

    varstr_free (vstr), vstr = NULL;

#if 0
    /* Attention, lexicalizer_mngr doit etre compile' avec l'option -DMAKE_PROPER */
    if (lexicalizer2basic_item_set (FALSE, FALSE)) {
      /* On sort la grammaire reduite */
      int               item, prod, i, X, id;
      extern char	*ctime ();
      long	        date_time;
      sxword_header     t_names, nt_names;
      int               *new_t2old_t, *new_nt2old_nt, top_t_names, top_nt_names;
    
      date_time = time (0);

      printf ("\
*   *****************************************************************************************\n\
*    This proper sub-grammar for the language \"%s\" in BNF format has been generated\n\
*    on %s\
*    by the SYNTAX(*) make_proper processor\n\
*   *****************************************************************************************\n\
*    (*) SYNTAX is a trademark of INRIA.\n\
*   *****************************************************************************************\n\n\n",
	      LANGUAGE_NAME, ctime (&date_time));

      vstr = varstr_alloc (32);

      if (is_terminal_mapping) {
	new_t2old_t = (int*) sxalloc (-spf.insideG.maxt+1, sizeof (int)), new_t2old_t [0] = 0;
	sxword_alloc (&t_names, "t_names", -spf.insideG.maxt+1, 1, 32, sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL /* resize */, sxcont_free, NULL, NULL);
	top_t_names = 1;
      }

      if (is_non_terminal_mapping) {
	new_nt2old_nt = (int*) sxalloc (spf.insideG.maxnt+1, sizeof (int)), new_nt2old_nt [0] = 0;
	sxword_alloc (&nt_names, "nt_names", spf.inputG.maxnt+1, 1, 32, sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL /* resize */, sxcont_free, NULL, NULL);
	top_nt_names = 1;
      }

      new_prod = 0;
      item = 3;

      while ((item = sxba_scan (basic_item_set, item)) > 0) {
	prod = spf.inputG.prolis [item];

	if (is_prod_mapping)
	  printf ("*[P%i %i]\n", ++new_prod, prod);

	output_prod (prod);

	if (is_non_terminal_mapping) {
	  X = spf.inputG.lhs [prod];
	  id = sxword_save (&nt_names, spf.inputG.ntstring [X]);
	  
	  if (id > top_nt_names) {
	    /* Nouveau */
	    top_nt_names = id;
	    new_nt2old_nt [id-1] = X;
	  }
	}

	if (is_terminal_mapping || is_non_terminal_mapping) {
	  while ((X = spf.inputG.lispro [item]) != 0) {
	    if (X > 0) {
	      if (is_non_terminal_mapping) {
		id = sxword_save (&nt_names, spf.inputG.ntstring [X]);

		if (id > top_nt_names) {
		  /* Nouveau */
		  top_nt_names = id;
		  new_nt2old_nt [id-1] = X;
		}
	      }
	    }
	    else {
	      if (is_terminal_mapping) {
		X = -X;
		id = sxword_save (&t_names, spf.inputG.tstring [X]);

		if (id > top_t_names) {
		  /* Nouveau */
		  top_t_names = id;
		  new_t2old_t [id-1] = X;
		}
	      }
	    }

	    item++;
	  }
	}
	else
	  item = spf.inputG.prolon [prod+1]-1;
      }

      if (is_non_terminal_mapping) {
	fputs ("\n* *********************** NON-TERMINAL MAPPING [new_nt_code -> old_nt_code] ***********************", stdout);

	for (i = 1; i < top_nt_names; i++) {
	  X = new_nt2old_nt [i];
	  printf ("\n*[N%i %i] ", i, X);
	  output_nt (X);
	}

	fputs ("\n", stdout);

	sxword_free (&nt_names);
	sxfree (new_nt2old_nt);
      }

      if (is_terminal_mapping) {
	fputs ("\n* *********************** TERMINAL MAPPING [new_t_code -> old_t_code] ***********************", stdout);

	for (i = 1; i < top_t_names; i++) {
	  X = new_t2old_t [i];
	  printf ("\n*[T%i %i] ", i, X);
	  output_t (X);
	}

	fputs ("\n", stdout);

	sxword_free (&t_names);
	sxfree (new_t2old_t);
      }

      varstr_free (vstr), vstr = NULL;
      sxfree (basic_item_set), basic_item_set = NULL;
    }
    else {
      sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
	      "%sNo proper sub-grammar for the language \"%s\" can be constructed from the input terminal sub-set.",
	      sxplocals.sxtables->err_titles [2],
	      LANGUAGE_NAME);
    }
#endif /* 0 */

    break;

  case FINAL:
    break;

  case CLOSE:
    sxfree (glbl_source_area), glbl_source_area = glbl_source = NULL;
    sxfree (source_set), source_set = source_top = NULL;
    break;

  default:
    fputs ("The function \"make_proper_semact\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
  }
}


SXVOID
make_proper_scanact (code, act_no)
    int		code;
    int		act_no;
{
  switch (code) {
  case OPEN:
  case CLOSE:
  case INIT:
  case FINAL:
    return;

  case ACTION:
    switch (act_no) {
      register SHORT	c;

    case 1: /* \nnn => char */
      {
	int	val;
	char	c, *s, *t;

	t = s = sxsvar.lv_s.token_string + sxsvar.lv.mark.index;

	for (val = *s++ - '0'; (c = *s++) != NUL; ) {
	  val = (val << 3) + c - '0';
	}

	*t = val;
	sxsvar.lv.ts_lgth = sxsvar.lv.mark.index + 1;
	sxsvar.lv.mark.index = -1;
      }

      return;
    }

  default:
    fputs ("The function \"make_proper_scanact\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
  }
}


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
}

int
sxivoid ()
/* procedure ne faisant rien */
{
}

/* Appelees ds sxspf_mngr */
struct sxtoken* parser_Tpq2tok (int Tpq)
{
  return NULL;
}

void fill_Tij2tok_no ()
{
}
