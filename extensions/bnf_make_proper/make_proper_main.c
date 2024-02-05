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

/* Main module for "make_proper" a SYNTAX module which outputs a CFG in BNF format
   From both an input CFG (%include LC_TABLES_H) which has been produced by an
   execution of bnf -huge L.bnf > L_huge.h (L_huge.h and LC_TABLES_H are synonyms)
   and a source text which is a list of terminal symbols.  This list is converted into
   T' a subset of T (option -exclude).  The proper subgrammar whose set of terminals is T'
   is printed on stdout using the SYNTAX library module lexicalizer_mngr.c */

static char	ME [] = "make_proper_main";

#include "sxversion.h"
#include "sxunix.h"

char WHAT_MAKE_PROPER_MAIN[] = "@(#)SYNTAX - $Id: make_proper_main.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

//#ifdef LC_TABLES_H
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
#define def_sxdfa_comb_inflected_form
/* Attention depuis le 17/01/08 le dictionnaire des terminaux est sous la forme d'un sxdfa_comb (voir "fsa.h") et
   non plus sous la forme d'un sxword */

#include LC_TABLES_H
//#endif /* LC_TABLES_H */

/* Pour avoir la definition et l'initialisation de spf faite ici par sxspf.h et celle de idag faite par udag_scanner.h */
#define SX_DFN_EXT_VAR2
#include "sxspf.h"
#include "varstr.h"
#include "udag_scanner.h"

SXINT n;
bool is_print_time;

VARSTR   concat_ff_tstr (VARSTR vstr, char* ff, int Tpq) {return NULL;}
struct sxtoken *tok_no2tok (int tok_no) {return NULL;}
int      get_SEMLEX_lahead () {return 0;}
void     local_fill_Tij2tok_no () {}
void fill_Tij2tok_no () {}


extern struct sxtables  sxtables;
extern SXBA            basic_item_set; 
static VARSTR	       vstr;

/*---------------*/
/*    options    */
/*---------------*/


static bool	is_help, is_exclude, is_prod_mapping, is_terminal_mapping, is_non_terminal_mapping;

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

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    HELP,
    VERBOSE, VERBOSE,
    EXCLUDE, EXCLUDE,
    PROD_MAPPING, PROD_MAPPING,
    TERMINAL_MAPPING, TERMINAL_MAPPING,
    NON_TERMINAL_MAPPING, NON_TERMINAL_MAPPING,
};



static SXINT	option_get_kind (arg)
    char	*arg;
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



static void
make_proper_run (pathname)
    char	*pathname;
{
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
    syntax (SXBEGIN, &sxtables, infile, "");
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

      syntax (SXBEGIN, &sxtables, infile, pathname);
    }
  }

  syntax (SXACTION, &sxtables);
  syntax (SXEND, &sxtables);
  fclose (infile);
}



/************************************************************************/
/* main function
/************************************************************************/
int main(int argc, char *argv[])
{
  SXINT		argnum;
  bool	is_source_file, is_stdin;
  char          *source_file;

  n = 0;

  sxopentty ();

  /* valeurs par defaut */
  is_help = false;
  is_exclude = false;
  is_prod_mapping = false;
  is_terminal_mapping = false;
  is_non_terminal_mapping = false;
  is_stdin = true;
  is_source_file = false;
  
  argnum = 0;

  while (++argnum < argc) {
    switch (option_get_kind (argv [argnum])) {
    case HELP:
      is_help = true;
      break;

    case VERBOSE:
      sxverbosep = true;
      break;

    case EXCLUDE:
      is_exclude = true;

    case PROD_MAPPING:
      is_prod_mapping = true;
      break;

    case TERMINAL_MAPPING:
      is_terminal_mapping = true;
      break;

    case NON_TERMINAL_MAPPING:
      is_non_terminal_mapping = true;
      break;

    case STDIN:
      is_stdin = true;
      break;

    case SOURCE_FILE:
      is_stdin = false;
      is_source_file = true;
      source_file = argv [argnum];
      break;

    case UNKNOWN_ARG:
      fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
      fprintf (sxstderr, Usage, ME);
      sxexit (3);
    }
  }

  if (!is_stdin && !is_source_file || is_help) {
    fprintf (sxstderr, Usage, ME);
    sxexit (3);
  }


  syntax (SXINIT, &sxtables, false /* no includes */);

  make_proper_run (is_stdin ? NULL : source_file);

  syntax (SXFINAL, &sxtables, true);

  sxexit (sxerr_max_severity ());
  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

/* Utilisees ds lexicalizer_mngr */
static void
output_nt (SXINT nt)
{
  printf ("<%s> ", spf.inputG.ntstring [nt]);
}


static void
output_t (SXINT t)
{
  varstr_raz (vstr);
  vstr = varstr_quote (vstr, spf.inputG.tstring [t]);
  printf ("%s ", varstr_tostr (vstr));
}



void
output_prod (SXINT prod)
{
  SXINT        item, X;

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
void
make_proper_semact (what, arg)
    SXINT		what, arg;
{
  static SXINT          word_pos;//, glbl_source_size;
  struct sxtoken	*tok;
  SXINT                 t_code, new_prod, word_lgth;
  char                  *word;

  switch (what) {
  case SXOPEN:
#if 0
    glbl_source_size = 100;
    glbl_source_area = glbl_source = (SXINT*) sxalloc (glbl_source_size+1, sizeof (SXINT));
#endif /* 0 */
    idag.source_set = sxba_calloc (SXEOF+1);
    break;

  case SXINIT:
#if 0
    sxword_reuse (&inflected_form_names, "inflected_form_names", sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
#endif /* 0 */
    word_pos = 0;
    break;

  case SXACTION:
    if (arg == 0)
	return;

#if EBUG
    if (arg != 1)
      sxtrap (ME, "make_proper_semact");
#endif /* EBUG */

#if 0
    if (++word_pos > glbl_source_size) {
      glbl_source_size *= 2;
      glbl_source_area = glbl_source = (SXINT*) sxrealloc (glbl_source, glbl_source_size+1, sizeof (SXINT));
    }
#endif /* 0 */

    tok = &(SXSTACKtoken (SXSTACKtop ()));
    word = sxstrget (tok->string_table_entry);
    word_lgth = strlen (word);

    t_code = sxdfa_comb_seek_a_string (&sxdfa_comb_inflected_form, word, &word_lgth);

    if (word_lgth != 0)
      t_code = 0;

    if (t_code == 0)
      sxerror (tok->source_index,
	       sxsvar.sxtables->err_titles [1][0],
	       "%s: The unknown terminal symbol \"%s\" is ignored.",
	       sxsvar.sxtables->err_titles [1]+1,
	       word);
    else {
      SXBA_1_bit (idag.source_set, t_code);
#if 0
      glbl_source [word_pos] = t_code;
#endif /* 0 */
    }

#if 0
    t_code = sxword_retrieve (&inflected_form_names, word);

    if (t_code <= 1) {
      sxerror (tok->source_index,
	       sxsvar.sxtables->err_titles [1][0],
	       "%s: The unknown terminal symbol \"%s\" is ignored.",
	       sxsvar.sxtables->err_titles [1]+1);
      t_code = 0;
    }
    else {
      t_code--;
      SXBA_1_bit (source_set, t_code);
    }

    glbl_source [word_pos] = t_code;
#endif /* 0 */

    break;

  case SXSEMPASS:
    n = word_pos;
#if 0
    init_mlstn = 1;
    final_mlstn = n+1;
#endif /* 0 */

    /*  On prepare l'appel de lexicalizer_mngr */
    if (is_exclude) {
      sxba_not (idag.source_set); 
      SXBA_0_bit (idag.source_set, 0);
    }
    else
      SXBA_1_bit (idag.source_set, SXEOF);

    vstr = varstr_alloc (32); /* Pour output_t () !! */
      
    /* Attention, lexicalizer_mngr doit etre compile' avec l'option -DMAKE_PROPER */
    if (lexicalizer2basic_item_set (false, false)) {
      /* On sort la grammaire reduite contenue ds spf.insideG */
      SXINT               new_prod, prod, i, X;
    
      printf ("\
*   *****************************************************************************************\n\
*    This proper sub-grammar for the language \"%s\" in BNF format has been generated\n\
*    by the SYNTAX(*) make_proper processor\n\
*   *****************************************************************************************\n\
*    (*) SYNTAX is a trademark of INRIA.\n\
*   *****************************************************************************************\n\n\n",
	      LANGUAGE_NAME);

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
	      sxplocals.sxtables->err_titles [2]+1,
	      LANGUAGE_NAME);
    }

    varstr_free (vstr), vstr = NULL;

#if 0
    /* Attention, lexicalizer_mngr doit etre compile' avec l'option -DMAKE_PROPER */
    if (lexicalizer2basic_item_set (false, false)) {
      /* On sort la grammaire reduite */
      SXINT               item, prod, i, X, id;
      extern char	*ctime ();
      sxword_header     t_names, nt_names;
      SXINT               *new_t2old_t, *new_nt2old_nt, top_t_names, top_nt_names;
    
      printf ("\
*   *****************************************************************************************\n\
*    This proper sub-grammar for the language \"%s\" in BNF format has been generated\n\
*    by the SYNTAX(*) make_proper processor\n\
*   *****************************************************************************************\n\
*    (*) SYNTAX is a trademark of INRIA.\n\
*   *****************************************************************************************\n\n\n",
	      LANGUAGE_NAME);

      vstr = varstr_alloc (32);

      if (is_terminal_mapping) {
	new_t2old_t = (SXINT*) sxalloc (-spf.insideG.maxt+1, sizeof (SXINT)), new_t2old_t [0] = 0;
	sxword_alloc (&t_names, "t_names", -spf.insideG.maxt+1, 1, 32, sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL /* resize */, sxcont_free, NULL, NULL);
	top_t_names = 1;
      }

      if (is_non_terminal_mapping) {
	new_nt2old_nt = (SXINT*) sxalloc (spf.insideG.maxnt+1, sizeof (SXINT)), new_nt2old_nt [0] = 0;
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
	      sxplocals.sxtables->err_titles [2]+1,
	      LANGUAGE_NAME);
    }
#endif /* 0 */

    break;

  case SXFINAL:
    break;

  case SXCLOSE:
#if 0
    sxfree (glbl_source_area), glbl_source_area = glbl_source = NULL;
#endif /* 0 */
    sxfree (idag.source_set), idag.source_set /*= source_top*/ = NULL;
    break;

  default:
    fputs ("The function \"make_proper_semact\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
  }
}


void
make_proper_scanact (code, act_no)
    SXINT		code;
    SXINT		act_no;
{
  switch (code) {
  case SXOPEN:
  case SXCLOSE:
  case SXINIT:
  case SXFINAL:
    return;

  case SXACTION:
    switch (act_no) {
      short	c;

    case 1: /* \nnn => char */
      {
	SXINT	val;
	char	c, *s, *t;

	t = s = sxsvar.sxlv_s.token_string + sxsvar.sxlv.mark.index;

	for (val = *s++ - '0'; (c = *s++) != SXNUL; ) {
	  val = (val << 3) + c - '0';
	}

	*t = val;
	sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index + 1;
	sxsvar.sxlv.mark.index = -1;
      }

      return;
    }

  default:
    fputs ("The function \"make_proper_scanact\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
  }
}

/* Appelees ds sxspf_mngr */
void idag_source_processing  (SXINT i, SXBA *t2suffix_t_set, SXBA *mlstn2suffix_source_set, SXBA *mlstn2la_tset, SXBA *t2la_t_set){}
SXINT get_repair_Tpq2tok_no (SXINT Tpq) {return 0;}
void   idag_p_t_q2tok_no_stack (SXINT **tok_no_stack_ptr, SXINT p, SXINT t, SXINT q) {};
VARSTR  rcvr_out_range    (VARSTR vstr, SXINT lb, SXINT ub) {return NULL;}
SXINT   special_Apq_name  (char *string, SXINT lgth) {return 0;}
SXINT   special_Tpq_name  (char *string, SXINT lgth) {return 0;}
VARSTR sub_dag_to_comment      (VARSTR vstr, SXINT p, SXINT q) {return NULL;}
void sxearley_open_for_semact (void) {};
SXINT  dag_scanner             (SXINT what, struct sxtables *arg) {return 1;};
