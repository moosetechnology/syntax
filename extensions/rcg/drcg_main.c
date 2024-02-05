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

#define SX_DFN_EXT_VAR_RCG

#include "sxunix.h"
#include "put_edit.h"

#include "rcg.h"

/* includes et variable pour la manipulation de la date pour rcg_modif_time */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>

char WHAT_DRCGMAIN[] = "@(#)SYNTAX - $Id: drcg_main.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#include "sxversion.h"

extern struct sxtables	rcg_tables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "drcg";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-lo, -listing_output, -nlo, -no_listing_output\n\
\t\t-csd, -check_self_definition, -ncsd, -no_check_self_definition\n\
\t\t-cfg, -context_free_grammar, -ncfg, -no_context_free_grammar\n\
\t\t-cs, -check_set, -ncs, -no_check_set\n\
\t\t-do, -decrease_order, -io, -increase_order\n\
\t\t-OLTAG, -nOLTAG\n\
\t\t-1rcg, -1RCG, -no_1rcg, -no_1RCG\n\
\t\t-bnf, -BNF, -no_bnf, -no_BNF\n\
\t\t-s, -simple, -ns, -no_simple\n\
\t\t-p, -proper, -np, -no_proper\n\
\t\t-2v, -2var_form, -n2v, -no_2var_form\n\
\t\t-f, -factorize, -nf, -no_factorize\n\
\t\t-c, -combinatorial, -nc, -no_combinatorial\n\
\t\t-lfsa pathname, -LFSA pathname\n\
\t\t-rfsa pathname, -RFSA pathname\n\
\t\t-h nnn, -hvalue = nnn\n\
\t\t-lc, -left_corner, -lr0, -LR0\n\
\t\t-nfndfsa, -null_free_ndfsa, -no_nfndfsa, -no_null_free_ndfsa\n\
\t\t-fl, -full_lex, -nfl, -no_full_lex\n\
\t\t-ucp pathname, -useless_clause_pathname pathname\n\
\t\t-fl, -first_last, -no_fl, -no_first_last\n\
\t\t-la, -look_ahead, -no_la, -no_look_ahead\n\
\t\t-tag, -tagger, -no_tag, -no_tagger\n\
\t\t-sr, -shift_reduce, -no_sr, -no_shift_reduce\n\
\t\t-shallow /dev/null|pathname, -SHALLOW /dev/null|pathname\n\
\t\t-kl, -keep_lex, -no_kl, -no_keep_lex\n\
\t\t-ker, -keep_eps_rules, -no_ker, -no_keep_eps_rules\n\
\t\t-ln name, -language_name name\n\
\tThe \"-ln\" option is mandatory if no file is given.\n";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define VERBOSE 		((SXINT)1)
#define LISTING_OUTPUT		((SXINT)2)
#define CHECK_SELF_DEFINITION	((SXINT)3)
#define CONTEXT_FREE_GRAMMAR	((SXINT)4)
#define CHECK_SET		((SXINT)5)
#define DECREASE_ORDER 		((SXINT)6)
#define OLTAG	 		((SXINT)7)
#define ONE_RCG	 		((SXINT)8)
#define GEN_BNF			((SXINT)9)
#define SIMPLE			((SXINT)10)
#define PROPER			((SXINT)11)
#define TWO_VAR_FORM		((SXINT)12)
#define FACTORIZE		((SXINT)13)
#define COMBINATORIAL		((SXINT)14)
#define HVALUE		        ((SXINT)15)
#define LEFT_CORNER	        ((SXINT)16)
#define NFNDFSA                 ((SXINT)17)
#define FULL_LEX	        ((SXINT)18)
#define FIRST_LAST     	        ((SXINT)19)
#define LOOK_AHEAD	        ((SXINT)20)
#define TAGGER   	        ((SXINT)21)
#define SHIFT_REDUCE   	        ((SXINT)22)
#define SHALLOW   	        ((SXINT)23)
#define KEEP_LEX   	        ((SXINT)24)
#define KEEP_EPS_RULES 	        ((SXINT)25)
#define LANGUAGE_NAME 		((SXINT)26)
/* Ne pas depasser 31 !! */
/* Not in options_set */
#define USELESS_CLAUSE_PATHNAME ((SXINT)27)
#define LFSA		        ((SXINT)28)
#define RFSA		        ((SXINT)29)
#define LBNF		        ((SXINT)30)
#define RBNF		        ((SXINT)31)
#define LAST_OPTION		RBNF

static char	*option_tbl [] = {"",
				  "v", "verbose", "nv", "noverbose",
				  "lo", "listing_output", "nlo", "no_listing_output",
				  "csd", "check_self_definition", "ncsd", "no_check_self_definition",
				  "cfg", "context_free_grammar", "ncfg", "no_context_free_grammar",
				  "cs", "check_set", "ncs", "no_check_set",
				  "do", "decrease_order", "io", "increase_order",
				  "OLTAG", "nOLTAG",
				  "1rcg", "1RCG", "no_1rcg", "no_1RCG",
				  "bnf", "bnf", "no_bnf", "no_BNF",
				  "s", "simple", "ns", "no_simple",
				  "p", "proper", "np", "no_proper",
				  "2v", "2var_form", "n2v", "no_2var_form",
				  "f", "factorize", "nf", "no_factorize",
				  "c", "combinatorial", "nc", "no_combinatorial",
				  "h", "hvalue",
				  "lc", "left_corner", "lr0", "LR0",
				  "nfndfsa", "null_free_ndfsa", "no_nfndfsa", "no_null_free_ndfsa",
				  "fl", "full_lex", "nfl", "no_full_lex",
				  "fl", "first_last", "no_fl", "no_first_last",
				  "la", "look_ahead", "no_la", "no_look_ahead",
				  "tag", "tagger", "no_tag", "no_tagger",
				  "sr", "shift_reduce", "no_sr", "no_shift_reduce",
				  "shallow", "SHALLOW",
				  "kl", "keep_lex", "no_kl", "no_keep_lex",
				  "ker", "keep_eps_rules", "no_ker", "no_keep_eps_rules",
				  "ln", "language_name",
				  "ucp", "useless_clause_pathname",
				  "lfsa", "LFSA",
				  "rfsa", "RFSA",
				  "lbnf", "LBNF",
				  "rbnf", "RBNF",
};
static SXINT	option_kind [] = {UNKNOWN_ARG,
				  VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				  LISTING_OUTPUT, LISTING_OUTPUT, -LISTING_OUTPUT, -LISTING_OUTPUT,
				  CHECK_SELF_DEFINITION, CHECK_SELF_DEFINITION, -CHECK_SELF_DEFINITION, -CHECK_SELF_DEFINITION,
				  CONTEXT_FREE_GRAMMAR, CONTEXT_FREE_GRAMMAR, -CONTEXT_FREE_GRAMMAR, -CONTEXT_FREE_GRAMMAR,
				  CHECK_SET, CHECK_SET, -CHECK_SET, -CHECK_SET,
				  DECREASE_ORDER, DECREASE_ORDER, -DECREASE_ORDER, -DECREASE_ORDER,
				  OLTAG, -OLTAG,
				  ONE_RCG, ONE_RCG, -ONE_RCG, -ONE_RCG,
				  GEN_BNF, GEN_BNF, -GEN_BNF, -GEN_BNF,
				  SIMPLE, SIMPLE, -SIMPLE, -SIMPLE,
				  PROPER, PROPER, -PROPER, -PROPER,
				  TWO_VAR_FORM, TWO_VAR_FORM, -TWO_VAR_FORM, -TWO_VAR_FORM,
				  FACTORIZE, FACTORIZE, -FACTORIZE, -FACTORIZE,
				  COMBINATORIAL, COMBINATORIAL, -COMBINATORIAL, -COMBINATORIAL,
				  HVALUE, HVALUE,
				  LEFT_CORNER, LEFT_CORNER, -LEFT_CORNER, -LEFT_CORNER,
				  NFNDFSA, NFNDFSA, -NFNDFSA, -NFNDFSA,
				  FULL_LEX, FULL_LEX, -FULL_LEX, -FULL_LEX,
				  FIRST_LAST, FIRST_LAST, -FIRST_LAST, -FIRST_LAST,
				  LOOK_AHEAD, LOOK_AHEAD, -LOOK_AHEAD, -LOOK_AHEAD,
				  TAGGER, TAGGER, -TAGGER, -TAGGER,
				  SHIFT_REDUCE, SHIFT_REDUCE, -SHIFT_REDUCE, -SHIFT_REDUCE,
				  SHALLOW, SHALLOW,
				  KEEP_LEX, KEEP_LEX, -KEEP_LEX, -KEEP_LEX,
				  KEEP_EPS_RULES, KEEP_EPS_RULES, -KEEP_EPS_RULES, -KEEP_EPS_RULES,
				  LANGUAGE_NAME, LANGUAGE_NAME,
				  USELESS_CLAUSE_PATHNAME, USELESS_CLAUSE_PATHNAME,
				  LFSA, LFSA,
				  RFSA, RFSA,
				  LBNF, LBNF,
				  RBNF, RBNF,
};

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



static void	extract_language_name (char *path_name)
{
    char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    prgentname = (char*) sxalloc (strlen (p)+1, sizeof (char));

    if ((p = sxindex (strcpy (prgentname, p), '.')) != NULL) {
	*p = SXNUL;
    }
}



static void	drcg_run (char *pathname)
{
    extern void		drcg_pass (void);
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
	syntax (SXBEGIN, &rcg_tables, infile, "");
    }
    else if ((infile = fopen (pathname, "r")) == NULL) {
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

	extract_language_name (pathname);
	syntax (SXBEGIN, &rcg_tables, infile, pathname);
	stat (sxsrcmngr.source_coord.file_name, &buf);
    }

    if (sxverbosep) {
	fputs ("RCG part\n", sxtty);
    }

    syntax (SXACTION, &rcg_tables);

    if (!IS_ERROR) {
	if (sxverbosep) {
	    fputs ("Definite part\n", sxtty);
	}

	drcg_pass ();
    }

    syntax (SXEND, &rcg_tables);
    fclose (infile);

    if (prgentname != NULL)
      sxfree (prgentname), prgentname = NULL;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int	        argnum;
  char	*str;

  sxopentty ();

  by_mess = "the SYNTAX processor for Definite Range Concatenation Grammars";

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    sxexit (SXSEVERITIES);
  }

  /* valeurs par defaut */

  /* On avertit rcg_smp, rcg_gen_parser, ... que l'on traite une grammaire de type DRCG */
  is_drcg = true;

  options_set = OPTION (VERBOSE) | OPTION (CHECK_SELF_DEFINITION) | OPTION (DECREASE_ORDER) |
    OPTION (GEN_BNF) | OPTION (FACTORIZE) | OPTION (FIRST_LAST);
  sxverbosep = false;
  check_self_definition = true;
  check_set = false; /* Les clauses peuvent ne pas former un ensemble */
  decrease_order = true; /* controle le sens du parcourt des boucles for */
  is_gen_bnf = true; /* On genere un .bnf */

  is_listing_output = false;
  is_CF_parser = false;
  is_OLTAG = false; /* OLTAG == true : la RCG est produite a partir d'une TAG et la bnf produite est
		       un peu particuliere */
  is_1_RCG = false; /* 1rcg => On produit en sortie une 1-RCG qui decrit un sur-langage et qui servira de guide */
  is_simple = is_proper = false; /* On verifie si la RCG est simple ou propre */
  is_2var_form = false;
  is_factorize = true; /* Les clauses et predicats identiques sont reduits */
  is_combinatorial = false; /* ATTENTION : si is_combinatorial est "false" pour un module, la generation
			       du traitement des cycles non triviaux suppose que toute la grammaire -- les autres
			       modules -- sont aussi non-combinatoire, le test est effectue dynamiquement */
  lfsa_file = NULL;
  rfsa_file = NULL;
  h_value = 1;
  is_left_corner = true;
  used_nfndfsa = false;
  is_full_lex = false; /* La guiding RCG contient tous les &Lex */
  is_first_last = true; /* On genere les test pour first et last */
  is_look_ahead = false; /* On n'utilise pas de look-ahead ds le FSA */
  is_tagger = false; /* Le FSA ne contient plus que des transitions terminales (a l'exception des reduces
                        qui ont des &Lex associes).  L'analyse ne retient que l'indice ds le source des
			terminaux valides.  Peut servir pour faire du POS tagging ou un 1er niveau de guide */
  is_keep_lex = false; /* Si is_tagger.  Garde ds l'automate les productions qui ont du &Lex associe.
			  C,a permet d'etre +fin sur le tagging */
  is_keep_epsilon_rules = false; /* Si is_keep_lex == true.  Supprime de l'automate les productions vides
				    meme si elles ont du &Lex
				    C,a peut augmenter la vitesse */
  is_shift_reduce = false; /* Les actions shift-reduce du PDA sont codees par une transition unique. Le
			      langage du FSA est +proche de celui du PDA  si is_shift_reduce == true */
  is_shallow = false; /* cas (l|r)fsa on traite des clauses et non des LHS_prdct */
  shallow_file = NULL;
  lbnf_file = NULL;
  rbnf_file = NULL;

  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case VERBOSE:
      sxverbosep = true, options_set |= OPTION (VERBOSE);
      break;

    case -VERBOSE:
      sxverbosep = false, options_set &= noOPTION (VERBOSE);
      break;

    case LISTING_OUTPUT:
      is_listing_output = true, options_set |= OPTION (LISTING_OUTPUT);
      break;

    case -LISTING_OUTPUT:
      is_listing_output = false, options_set &= noOPTION (LISTING_OUTPUT);
      break;

    case CHECK_SELF_DEFINITION:
      check_self_definition = true, options_set |= OPTION (CHECK_SELF_DEFINITION);
      break;

    case -CHECK_SELF_DEFINITION:
      check_self_definition = false, options_set &= noOPTION (CHECK_SELF_DEFINITION);
      break;

    case CONTEXT_FREE_GRAMMAR:
      is_CF_parser = true, options_set |= OPTION (CONTEXT_FREE_GRAMMAR);
      break;

    case -CONTEXT_FREE_GRAMMAR:
      is_CF_parser = false, options_set &= noOPTION (CONTEXT_FREE_GRAMMAR);
      break;

    case CHECK_SET:
      check_set = true, options_set |= OPTION (CHECK_SET);
      break;

    case -CHECK_SET:
      check_set = false, options_set &= noOPTION (CHECK_SET);
      break;

    case DECREASE_ORDER:
      decrease_order = true, options_set |= OPTION (DECREASE_ORDER);
      break;

    case -DECREASE_ORDER:
      decrease_order = false, options_set &= noOPTION (DECREASE_ORDER);
      break;

    case OLTAG:
      is_OLTAG = true, options_set |= OPTION (OLTAG);
      break;

    case -OLTAG:
      is_OLTAG = false, options_set &= noOPTION (OLTAG);
      break;

    case ONE_RCG:
      is_1_RCG = true, options_set |= OPTION (ONE_RCG);
      break;

    case -ONE_RCG:
      is_1_RCG = false, options_set &= noOPTION (ONE_RCG);
      break;

    case GEN_BNF:
      is_gen_bnf = true, options_set |= OPTION (GEN_BNF);
      break;

    case -GEN_BNF:
      is_gen_bnf = false, options_set &= noOPTION (GEN_BNF);
      break;

    case SIMPLE:
      is_simple = true, options_set |= OPTION (GEN_BNF);
      break;

    case -SIMPLE:
      is_simple = false, options_set &= noOPTION (GEN_BNF);
      break;

    case PROPER:
      is_proper = true, options_set |= OPTION (GEN_BNF);
      break;

    case -PROPER:
      is_proper = false, options_set &= noOPTION (GEN_BNF);
      break;

    case TWO_VAR_FORM:
      is_2var_form = true, options_set |= OPTION (TWO_VAR_FORM);
      break;

    case -TWO_VAR_FORM:
      is_2var_form = false, options_set &= noOPTION (TWO_VAR_FORM);
      break;

    case FACTORIZE:
      is_factorize = true, options_set |= OPTION (TWO_VAR_FORM);
      break;

    case -FACTORIZE:
      is_factorize = false, options_set &= noOPTION (TWO_VAR_FORM);
      break;

    case COMBINATORIAL:
      is_combinatorial = true, options_set |= OPTION (COMBINATORIAL);
      break;

    case -COMBINATORIAL:
      is_combinatorial = false, options_set &= noOPTION (COMBINATORIAL);
      break;

    case LFSA:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (LFSA));
	sxexit (3);
      }
	      
      if ((lfsa_file = sxfopen (lfsa_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	sxexit (3);
      }

      break;

    case RFSA:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (RFSA));
	sxexit (3);
      }
	      
      if ((rfsa_file = sxfopen (rfsa_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	sxexit (3);
      }

      break;

    case HVALUE:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
	sxexit (3);
      }

      h_value = (SXINT) strtol (argv [argnum], &str, 0);

      if (str == argv [argnum]) {
	fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n", ME, argv [argnum-1]);
	sxexit (3);
      }
	    
      break;

    case LEFT_CORNER:
      is_left_corner = true, options_set |= OPTION (LEFT_CORNER);
      break;

    case -LEFT_CORNER:
      is_left_corner = false, options_set &= noOPTION (LEFT_CORNER);
      break;  

    case NFNDFSA:
      used_nfndfsa = true, options_set |= OPTION (NFNDFSA);
      break;

    case -NFNDFSA:
      used_nfndfsa = false, options_set &= noOPTION (NFNDFSA);
      break;

    case FULL_LEX:
      is_full_lex = true, options_set |= OPTION (FULL_LEX);
      break;

    case -FULL_LEX:
      is_full_lex = false, options_set &= noOPTION (FULL_LEX);
      break; 

    case USELESS_CLAUSE_PATHNAME:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n\t'' is assumed.\n", ME, option_get_text (USELESS_CLAUSE_PATHNAME));
	sxexit (3);
      }
	      
      if ((useless_clause_file = sxfopen (argv [argnum], "r")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	sxperror (argv [argnum]);
	sxexit (3);
      }

      break;

    case FIRST_LAST:
      is_first_last = true, options_set |= OPTION (FIRST_LAST);
      break;

    case -FIRST_LAST:
      is_first_last = false, options_set &= noOPTION (FIRST_LAST);
      break;

    case LOOK_AHEAD:
      is_look_ahead = true, options_set |= OPTION (LOOK_AHEAD);
      break;

    case -LOOK_AHEAD:
      is_look_ahead = false, options_set &= noOPTION (LOOK_AHEAD);
      break; 

    case TAGGER:
      is_tagger = true, options_set |= OPTION (TAGGER);
      break;

    case -TAGGER:
      is_tagger = false, options_set &= noOPTION (TAGGER);
      break; 

    case SHIFT_REDUCE:
      is_shift_reduce = true, options_set |= OPTION (SHIFT_REDUCE);
      break;

    case -SHIFT_REDUCE:
      is_shift_reduce = false, options_set &= noOPTION (SHIFT_REDUCE);
      break; 

    case SHALLOW:
      is_shallow = true, options_set |= OPTION (SHALLOW);

      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname (or %s) must follow the \"%s\" option;\n", ME, SX_DEV_NUL, option_get_text (LFSA));
	sxexit (3);
      }

      if (strncmp (shallow_file_name = argv [argnum], SX_DEV_NUL, strlen (SX_DEV_NUL)) != 0) {
	if ((shallow_file = sxfopen (argv [argnum], "r")) == NULL) {
	  fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	  sxperror (argv [argnum]);
	  sxexit (3);
	}
      }

      break;

    case KEEP_LEX:
      is_keep_lex = true, options_set |= OPTION (KEEP_LEX);
      break;

    case -KEEP_LEX:
      is_keep_lex = false, options_set &= noOPTION (KEEP_LEX);
      break; 

    case KEEP_EPS_RULES:
      is_keep_epsilon_rules = true, options_set |= OPTION (KEEP_EPS_RULES);
      break;

    case -KEEP_EPS_RULES:
      is_keep_epsilon_rules = false, options_set &= noOPTION (KEEP_EPS_RULES);
      break;

    case LBNF:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (LBNF));
	sxexit (3);
      }
	      
      if ((lbnf_file = sxfopen (lbnf_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	sxexit (3);
      }

      break;

    case RBNF:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (RBNF));
	sxexit (3);
      }
	      
      if ((rbnf_file = sxfopen (rbnf_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	sxexit (3);
      }

      break; 

    case LANGUAGE_NAME:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a name must follow the \"%s\" option;\n\t'' is assumed.\n", ME, option_get_text (
														 LANGUAGE_NAME));
	prgentname = (char*) sxalloc (1, sizeof (char));
	prgentname [0] = SXNUL;
      }
      else {
	prgentname = (char*) sxalloc (strlen (argv [argnum])+1, sizeof (char));
	strcpy (prgentname, argv [argnum]);
      }

      options_set = OPTION (LANGUAGE_NAME);
      break;

    case UNKNOWN_ARG:
      if (options_set & OPTION (LANGUAGE_NAME)) {
	fprintf (sxstderr, "%s: the \"%s\" option must not be used when naming files.\n", ME, option_get_text (
													       LANGUAGE_NAME));
	options_set &= noOPTION (LANGUAGE_NAME);
      }

      goto run;

    default:
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (SXINT) option_get_kind (argv [argnum]));
      sxexit (3);
    }
  }


  if (!(options_set & OPTION (LANGUAGE_NAME))) {
    fprintf (sxstderr, "%s: the \"%s\" option is mandatory when no file is named;\n\t'' is assumed\n", ME,
	     option_get_text (LANGUAGE_NAME));
    prgentname = (char*) sxalloc (1, sizeof (char));
    prgentname [0] = SXNUL;
  }

 run:
  if (sxverbosep) {
    fprintf (sxtty, "%s\n", release_mess);
  }

  syntax (SXINIT, &rcg_tables, false /* no includes */);

  if (options_set & OPTION (LANGUAGE_NAME)) {
    drcg_run ((char*)NULL);
  }
  else {
    do {
      int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      drcg_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (SXFINAL, &rcg_tables, true);

  sxexit (sxerr_max_severity ());
  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



char	*options_text (char *line)
{
    SXINT	i;
    bool	is_first = true;

    *line = SXNUL;

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = false;
	    else
		strcat (line, ", ");

	    strcat (line, option_get_text (i));

	    if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
}

