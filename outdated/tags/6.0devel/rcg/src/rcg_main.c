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







#define SX_DFN_EXT_VAR_RCG

#include "sxunix.h"
#include "put_edit.h"

#include "rcg.h"

/* includes et variable pour la manipulation de la date pour rcg_modif_time */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
char WHAT_RCGMAIN[] = "@(#)SYNTAX - $Id: rcg_main.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

#include "sxversion.h"

extern struct sxtables	rcg_tables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "rcg";
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
\t\t-flx, -full_lex, -nflx, -no_full_lex\n\
\t\t-ucp pathname, -useless_clause_pathname pathname\n\
\t\t-fl, -first_last, -no_fl, -no_first_last\n\
\t\t-la, -look_ahead, -no_la, -no_look_ahead\n\
\t\t-tag, -tagger, -no_tag, -no_tagger\n\
\t\t-sr, -shift_reduce, -no_sr, -no_shift_reduce\n\
\t\t-shallow /dev/null|pathname, -SHALLOW /dev/null|pathname\n\
\t\t-kl, -keep_lex, -no_kl, -no_keep_lex\n\
\t\t-ker, -keep_eps_rules, -no_ker, -no_keep_eps_rules\n\
\t\t-cic, -check_instantiable_clause, -no_cic, -no_check_instantiable_clause\n\
\t\t-lbnf pathname, -LBNF pathname\n\
\t\t-rbnf pathname, -RBNF pathname\n\
\t\t-Lex pathname\n\
\t\t-lrprod pathname\n\
\t\t-t pathname, -terminal pathname\n\
\t\t-col1\n\
\t\t-tig pathname\n\
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
#define CHECK_INSTANTIABLE_CLAUSE        ((SXINT)26)
#define LANGUAGE_NAME 		((SXINT)27)
/* Ne pas depasser 31 !! */
/* Not in options_set */
#define USELESS_CLAUSE_PATHNAME ((SXINT)28)
#define LFSA		        ((SXINT)29)
#define RFSA		        ((SXINT)30)
#define LBNF		        ((SXINT)31)
#define RBNF		        ((SXINT)32)
#define LEX		        ((SXINT)33)
#define LRPROD                  ((SXINT)34)
#define TERMINAL                ((SXINT)35)
#define COL1                    ((SXINT)36)
#define TIG                     ((SXINT)37)
#define LAST_OPTION		TIG

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
				  "flx", "full_lex", "nflx", "no_full_lex",
				  "fl", "first_last", "no_fl", "no_first_last",
				  "la", "look_ahead", "no_la", "no_look_ahead",
				  "tag", "tagger", "no_tag", "no_tagger",
				  "sr", "shift_reduce", "no_sr", "no_shift_reduce",
				  "shallow", "SHALLOW",
				  "kl", "keep_lex", "no_kl", "no_keep_lex",
				  "ker", "keep_eps_rules", "no_ker", "no_keep_eps_rules",
				  "cic", "check_instantiable_clause", "no_cic", "no_check_instantiable_clause",
				  "ln", "language_name",
				  "ucp", "useless_clause_pathname",
				  "lfsa", "LFSA",
				  "rfsa", "RFSA",
				  "lbnf", "LBNF",
				  "rbnf", "RBNF",
				  "Lex",
				  "lrprod",
				  "t", "terminal",
				  "col1",
				  "tig",
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
				  CHECK_INSTANTIABLE_CLAUSE, CHECK_INSTANTIABLE_CLAUSE, -CHECK_INSTANTIABLE_CLAUSE, -CHECK_INSTANTIABLE_CLAUSE,
				  LANGUAGE_NAME, LANGUAGE_NAME,
				  USELESS_CLAUSE_PATHNAME, USELESS_CLAUSE_PATHNAME,
				  LFSA, LFSA,
				  RFSA, RFSA,
				  LBNF, LBNF,
				  RBNF, RBNF,
				  LEX,
				  LRPROD,
				  TERMINAL, TERMINAL,
				  COL1,
				  TIG,
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



static	char *
extract_language_name (char *path_name)
{
    char	*p;

    if ((p = sxrindex (path_name, '/')) == NULL)
	p = path_name;
    else
	p++;

    prgentname = (char*) sxalloc (strlen (p)+1, sizeof (char));

    if ((p = sxindex (strcpy (prgentname, p), '.')) != NULL) {
	*p = SXNUL;
	return p+1;
    }
	
    return (char*) NULL;
}



static SXVOID	rcg_run (char *pathname)
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
	syntax (SXBEGIN, &rcg_tables, infile, "");
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

	suffixname = extract_language_name (pathname);
	syntax (SXBEGIN, &rcg_tables, infile, pathname);
	stat (sxsrcmngr.source_coord.file_name, &buf);
    }

    syntax (SXACTION, &rcg_tables);
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
  int		argnum;
  char	*str;

  sxopentty ();

  by_mess = "the SYNTAX processor for Range Concatenation Grammars";

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    SXEXIT (SXSEVERITIES);
  }

  /* valeurs par defaut */

  options_set = OPTION (VERBOSE) | OPTION (CHECK_SELF_DEFINITION) | OPTION (DECREASE_ORDER) |
    OPTION (GEN_BNF) | OPTION (FACTORIZE) | OPTION (FIRST_LAST);
  sxverbosep = SXFALSE;
  check_self_definition = SXTRUE;
  check_set = SXFALSE; /* Les clauses peuvent ne pas former un ensemble */
  decrease_order = SXTRUE; /* controle le sens du parcourt des boucles for */
  is_gen_bnf = SXFALSE; /* Le 26/05/04 Par defaut, on ne genere plus de .bnf */

  is_listing_output = SXFALSE;
  is_CF_parser = SXFALSE;
  is_OLTAG = SXFALSE; /* OLTAG == SXTRUE : la RCG est produite a partir d'une TAG et la bnf produite est
		       un peu particuliere */
  is_1_RCG = SXFALSE; /* 1rcg => On produit en sortie une 1-RCG qui decrit un sur-langage et qui servira de guide */
  is_simple = is_proper = SXFALSE; /* On verifie si la RCG est simple ou propre */
  is_2var_form = SXFALSE;
  is_factorize = SXTRUE; /* Les clauses et predicats identiques sont reduits */
  is_combinatorial = SXFALSE; /* ATTENTION : si is_combinatorial est "SXFALSE" pour un module, la generation
			       du traitement des cycles non triviaux suppose que toute la grammaire -- les autres
			       modules -- sont aussi non-combinatoire, le test est effectue dynamiquement */
  lfsa_file = NULL;
  rfsa_file = NULL;
  h_value = 1;
  is_left_corner = SXTRUE;
  used_nfndfsa = SXFALSE;
  is_full_lex = SXFALSE; /* La guiding RCG contient tous les &Lex */
  is_first_last = SXTRUE; /* On genere les test pour first et last */
  is_look_ahead = SXFALSE; /* On n'utilise pas de look-ahead ds le FSA */
  is_tagger = SXFALSE; /* Le FSA ne contient plus que des transitions terminales (a l'exception des reduces
                        qui ont des &Lex associes).  L'analyse ne retient que l'indice ds le source des
			terminaux valides.  Peut servir pour faire du POS tagging ou un 1er niveau de guide */
  is_keep_lex = SXFALSE; /* Si is_tagger.  Garde ds l'automate les productions qui ont du &Lex associe.
			  C,a permet d'etre +fin sur le tagging */
  is_keep_epsilon_rules = SXFALSE; /* Si is_keep_lex == SXTRUE.  Supprime de l'automate les productions vides
				    meme si elles ont du &Lex
				    C,a peut augmenter la vitesse */
  is_shift_reduce = SXFALSE; /* Les actions shift-reduce du PDA sont codees par une transition unique. Le
			      langage du FSA est +proche de celui du PDA  si is_shift_reduce == SXTRUE */
  is_shallow = SXFALSE; /* cas (l|r)fsa on traite des clauses et non des LHS_prdct */
  shallow_file = NULL;
  lbnf_file = NULL;
  rbnf_file = NULL;
  Lex_file = NULL;
  lrprod_file = NULL;
  tig_file = NULL;
  check_instantiable_clause = SXTRUE; /* SXTRUE => on detecte des erreurs sur les clauses non instantiables
				       SXFALSE => ces clauses sont considerees comme &false (), et on genere le code */

  terminal_file_name = NULL;
  terminal_file = NULL; /* Doit-on coder les terminaux comme des ident C et sortir leur liste */
  is_col1 = SXFALSE; /* Les terminaux ne sont pas generes en colonne 1 (voir make_a_dico) */

  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case VERBOSE:
      sxverbosep = SXTRUE, options_set |= OPTION (VERBOSE);
      break;

    case -VERBOSE:
      sxverbosep = SXFALSE, options_set &= noOPTION (VERBOSE);
      break;

    case LISTING_OUTPUT:
      is_listing_output = SXTRUE, options_set |= OPTION (LISTING_OUTPUT);
      break;

    case -LISTING_OUTPUT:
      is_listing_output = SXFALSE, options_set &= noOPTION (LISTING_OUTPUT);
      break;

    case CHECK_SELF_DEFINITION:
      check_self_definition = SXTRUE, options_set |= OPTION (CHECK_SELF_DEFINITION);
      break;

    case -CHECK_SELF_DEFINITION:
      check_self_definition = SXFALSE, options_set &= noOPTION (CHECK_SELF_DEFINITION);
      break;

    case CONTEXT_FREE_GRAMMAR:
      is_CF_parser = SXTRUE, options_set |= OPTION (CONTEXT_FREE_GRAMMAR);
      break;

    case -CONTEXT_FREE_GRAMMAR:
      is_CF_parser = SXFALSE, options_set &= noOPTION (CONTEXT_FREE_GRAMMAR);
      break;

    case CHECK_SET:
      check_set = SXTRUE, options_set |= OPTION (CHECK_SET);
      break;

    case -CHECK_SET:
      check_set = SXFALSE, options_set &= noOPTION (CHECK_SET);
      break;

    case DECREASE_ORDER:
      decrease_order = SXTRUE, options_set |= OPTION (DECREASE_ORDER);
      break;

    case -DECREASE_ORDER:
      decrease_order = SXFALSE, options_set &= noOPTION (DECREASE_ORDER);
      break;

    case OLTAG:
      is_OLTAG = SXTRUE, options_set |= OPTION (OLTAG);
      break;

    case -OLTAG:
      is_OLTAG = SXFALSE, options_set &= noOPTION (OLTAG);
      break;

    case ONE_RCG:
      is_1_RCG = SXTRUE, options_set |= OPTION (ONE_RCG);
      break;

    case -ONE_RCG:
      is_1_RCG = SXFALSE, options_set &= noOPTION (ONE_RCG);
      break;

    case GEN_BNF:
      is_gen_bnf = SXTRUE, options_set |= OPTION (GEN_BNF);
      break;

    case -GEN_BNF:
      is_gen_bnf = SXFALSE, options_set &= noOPTION (GEN_BNF);
      break;

    case SIMPLE:
      is_simple = SXTRUE, options_set |= OPTION (GEN_BNF);
      break;

    case -SIMPLE:
      is_simple = SXFALSE, options_set &= noOPTION (GEN_BNF);
      break;

    case PROPER:
      is_proper = SXTRUE, options_set |= OPTION (GEN_BNF);
      break;

    case -PROPER:
      is_proper = SXFALSE, options_set &= noOPTION (GEN_BNF);
      break;

    case TWO_VAR_FORM:
      is_2var_form = SXTRUE, options_set |= OPTION (TWO_VAR_FORM);
      break;

    case -TWO_VAR_FORM:
      is_2var_form = SXFALSE, options_set &= noOPTION (TWO_VAR_FORM);
      break;

    case FACTORIZE:
      is_factorize = SXTRUE, options_set |= OPTION (FACTORIZE);
      break;

    case -FACTORIZE:
      is_factorize = SXFALSE, options_set &= noOPTION (FACTORIZE);
      break;

    case COMBINATORIAL:
      is_combinatorial = SXTRUE, options_set |= OPTION (COMBINATORIAL);
      break;

    case -COMBINATORIAL:
      is_combinatorial = SXFALSE, options_set &= noOPTION (COMBINATORIAL);
      break;

    case LFSA:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (
												   LFSA));
	SXEXIT (3);
      }
	      
      if ((lfsa_file = sxfopen (lfsa_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break;

    case RFSA:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (
												   RFSA));
	SXEXIT (3);
      }
	      
      if ((rfsa_file = sxfopen (rfsa_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break;

    case HVALUE:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
		 ME, argv [argnum-1]);
	SXEXIT (3);
      }

      h_value = (SXINT) strtol (argv [argnum], &str, 0);

      if (str == argv [argnum]) {
	fprintf (sxstderr, "%s: the option \"%s\" must be followed by an integer number.\n",
		 ME, argv [argnum-1]);
	SXEXIT (3);
      }
	    
      break;

    case LEFT_CORNER:
      is_left_corner = SXTRUE, options_set |= OPTION (LEFT_CORNER);
      break;

    case -LEFT_CORNER:
      is_left_corner = SXFALSE, options_set &= noOPTION (LEFT_CORNER);
      break;   

    case NFNDFSA:
      used_nfndfsa = SXTRUE, options_set |= OPTION (NFNDFSA);
      break;

    case -NFNDFSA:
      used_nfndfsa = SXFALSE, options_set &= noOPTION (NFNDFSA);
      break; 

    case FULL_LEX:
      is_full_lex = SXTRUE, options_set |= OPTION (FULL_LEX);
      break;

    case -FULL_LEX:
      is_full_lex = SXFALSE, options_set &= noOPTION (FULL_LEX);
      break;  

    case USELESS_CLAUSE_PATHNAME:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (
												   USELESS_CLAUSE_PATHNAME));
	SXEXIT (3);
      }
	      
      if ((useless_clause_file = sxfopen (argv [argnum], "r")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break; 

    case FIRST_LAST:
      is_first_last = SXTRUE, options_set |= OPTION (FIRST_LAST);
      break;

    case -FIRST_LAST:
      is_first_last = SXFALSE, options_set &= noOPTION (FIRST_LAST);
      break; 

    case LOOK_AHEAD:
      is_look_ahead = SXTRUE, options_set |= OPTION (LOOK_AHEAD);
      break;

    case -LOOK_AHEAD:
      is_look_ahead = SXFALSE, options_set &= noOPTION (LOOK_AHEAD);
      break; 

    case TAGGER:
      is_tagger = SXTRUE, options_set |= OPTION (TAGGER);
      break;

    case -TAGGER:
      is_tagger = SXFALSE, options_set &= noOPTION (TAGGER);
      break; 

    case SHIFT_REDUCE:
      is_shift_reduce = SXTRUE, options_set |= OPTION (SHIFT_REDUCE);
      break;

    case -SHIFT_REDUCE:
      is_shift_reduce = SXFALSE, options_set &= noOPTION (SHIFT_REDUCE);
      break;  

    case SHALLOW:
      is_shallow = SXTRUE, options_set |= OPTION (SHALLOW);

      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname (or %s) must follow the \"%s\" option;\n", ME, SX_DEV_NUL, option_get_text (
														       LFSA));
	SXEXIT (3);
      }

      if (strncmp (shallow_file_name = argv [argnum], SX_DEV_NUL, strlen (SX_DEV_NUL)) != 0) {
	if ((shallow_file = sxfopen (argv [argnum], "r")) == NULL) {
	  fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	  sxperror (argv [argnum]);
	  SXEXIT (3);
	}
      }

      break;

    case KEEP_LEX:
      is_keep_lex = SXTRUE, options_set |= OPTION (KEEP_LEX);
      break;

    case -KEEP_LEX:
      is_keep_lex = SXFALSE, options_set &= noOPTION (KEEP_LEX);
      break; 

    case KEEP_EPS_RULES:
      is_keep_epsilon_rules = SXTRUE, options_set |= OPTION (KEEP_EPS_RULES);
      break;

    case -KEEP_EPS_RULES:
      is_keep_epsilon_rules = SXFALSE, options_set &= noOPTION (KEEP_EPS_RULES);
      break;

    case LBNF:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (
												   LBNF));
	SXEXIT (3);
      }
	      
      if ((lbnf_file = sxfopen (lbnf_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break;

    case RBNF:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (
												   RBNF));
	SXEXIT (3);
      }
	      
      if ((rbnf_file = sxfopen (rbnf_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break; 

    case LEX:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (
												   LEX));
	SXEXIT (3);
      }
	      
      if ((Lex_file = sxfopen (Lex_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break; 

    case LRPROD:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (
												   LEX));
	SXEXIT (3);
      }
	      
      if ((lrprod_file = sxfopen (lrprod_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break; 

    case TIG:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (TIG));
	SXEXIT (3);
      }
	      
      if ((tig_file = sxfopen (tig_file_name = argv [argnum], "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
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

      options_set |= OPTION (LANGUAGE_NAME);
      break;

    case CHECK_INSTANTIABLE_CLAUSE:
      check_instantiable_clause = SXTRUE, options_set |= OPTION (CHECK_INSTANTIABLE_CLAUSE);
      break;

    case -CHECK_INSTANTIABLE_CLAUSE:
      check_instantiable_clause = SXFALSE, options_set &= noOPTION (CHECK_INSTANTIABLE_CLAUSE);
      break;

    case TERMINAL:
      if (++argnum >= argc) {
	fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (TERMINAL));
	SXEXIT (3);
      }
	      
      if ((terminal_file = sxfopen (terminal_file_name = argv [argnum], "a")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) file in append mode ", ME);
	sxperror (argv [argnum]);
	SXEXIT (3);
      }

      break;

    case COL1:
      is_col1 = SXTRUE;
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

  if (is_combinatorial && (is_simple || is_proper)) {
    fprintf (sxstderr, "%s: incompatible \"-combinatorial\" option.\n", ME);
    SXEXIT (3);
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

  syntax (SXINIT, &rcg_tables, SXFALSE /* no includes */);

  if (options_set & OPTION (LANGUAGE_NAME)) {
    rcg_run ((char*)NULL);
  }
  else {
    do {
      int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      rcg_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (SXFINAL, &rcg_tables, SXTRUE);

  SXEXIT (sxerr_max_severity ());
  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



char	*options_text (char *line)
{
    SXINT	i;
    SXBOOLEAN	is_first = SXTRUE;

    *line = SXNUL;

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = SXFALSE;
	    else
		strcat (line, ", ");

	    strcat (line, option_get_text (i));

	    if (i == LANGUAGE_NAME) {
		strcat (strcat (line, " "), prgentname);
	    }
	}

    return line;
}

