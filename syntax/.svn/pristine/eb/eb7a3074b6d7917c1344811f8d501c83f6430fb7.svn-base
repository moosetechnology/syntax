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




/* Main module for a dag unfolder (dag->udag) */



#include "sxversion.h"
#include "sxunix.h"
#include "sxstack.h"
#include "varstr.h"
#include "fsa.h"
#include "XxYxZ.h"
#include "XxY.h"
#include "udag_scanner.h"

#define WHAT	"@(#)dag2udag_main.c\t- SYNTAX [unix] - 23 mars 2004"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

#if LOG
static char mess [128];
static SXINT total_trans_nb;
#endif

static XxYxZ_header input_transitions_hd;
static SXINT *input_transition_stack;

void             (*main_parser)(void); /* NULL pas de earley ou autre */

VARSTR cur_input_vstr; /* unused */
SXINT SEMLEX_lahead;

SXBOOLEAN		output_udag;
static SXBOOLEAN  is_help;
SXBOOLEAN tmp_file_for_stdin;
static char	ME [] = "dag2udag_main";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-v, -verbose, -nv, -noverbose,\n\
\t-d, -dag, -u, -udag,\n\
\t\t-tmp, -tmp_file_for_stdin,\n\
\t--help,\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define VERBOSE 	  1
#define UNFOLD	          2
#define HELP	          3
#define TMP_FILE_FOR_STDIN 4
#define SOURCE_FILE	  5
#define LAST_OPTION	  SOURCE_FILE


static char	*option_tbl [] = {
    "",
    "v", "verbose", "nv", "noverbose",
    "d", "dag", "u", "udag",
    "tmp", "tmp_file_for_stdin",
    "-help",
};

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
    -UNFOLD, -UNFOLD, UNFOLD, UNFOLD,
    TMP_FILE_FOR_STDIN, TMP_FILE_FOR_STDIN,
    HELP,
};

static SXINT	option_get_kind (char *arg)
{
  char	**opt;

  if (*arg++ != '-')
    return SOURCE_FILE;

  for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
    if (strcmp (*opt, arg) == 0 /* egalite */ )
      break;
  }

  return option_kind [opt - option_tbl];
}


#if 0
static char	*option_get_text (SXINT kind)
{
  SXINT	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}
#endif /* 0 */



/* On est dans un cas "mono-langage": */

static struct fa {
  SXINT          fsa_kind, init_state, final_state, sigma_card, eof_ste, transition_nb;
  XxYxZ_header fsa_hd;
  SXBOOLEAN      has_epsilon_trans;
} dfa, *cur_fsa;


static SXINT        fsa_trans_nb;




/* Cette procedure est appelee depuis read_a_re à la fin de la 1ere passe sur l'arbre abstrait */
/* Les arguments sont 
   - SXTRUE => OK
   - ste des transitions vers l'etat final
   - le nb de noeuds de cet arbre et 
   - le nb d'operande (dont eof + 1 bidon de fin) */
/* Peut servir a faire des alloc + ciblees */
static void
prelude_re (SXBOOLEAN is_OK, SXINT eof_ste, SXINT node_nb, SXINT operand_nb, SXINT fsa_kind)
{
  SXINT *fsa_foreach;
#if EBUG
  char *name;
#endif
  SXINT foreach [6] = {1, 0, 0, 0, 0, 0};
  
  sxuse (operand_nb);
  if (is_OK) {
#if LOG
    sxtime (SXINIT, NULL);
#endif /* LOG */

#if EBUG
    name = "RE2DFA";
#endif
    fsa_foreach = foreach;
    cur_fsa = &dfa;

    cur_fsa->fsa_kind = fsa_kind;
    cur_fsa->eof_ste = eof_ste;
    cur_fsa->init_state = 1;
    XxYxZ_alloc (&(cur_fsa->fsa_hd), "nfsa_hd", node_nb+1, 1, fsa_foreach, NULL /* pas de re_oflw */, NULL); 

#if EBUG
    printf ("\n%s\n", name);
#endif
    
  }
  else
    /* PB ds la lecture de l'ER (pas un dag, ...) */
    SXEXIT (2);

  sxtkn_mngr (SXINIT, 0);
}

/* Cette procedure est appelee depuis read_a_re sur chaque transition de l'automate produit */
/* L'etat initial est 1 */
/* l'etat final unique est atteint par transition sur le terminal bidon "<EOF>" de ste eof_ste et vaut -f */
/* Tous les etats de 1 a f sont utilises */
/* ptok_ptr contient {NULL, ptok1, ...ptokm, NULL} */
/* ste est commun a tous les ptok de ptok_ptr */
/* ptoki est un pointeur vers un token courant (dans le noeud de l'arbre abstrait) */
static void
store_re (SXINT state, struct sxtoken **ptok_ptr, struct sxtoken **semlex_ptok_ptr, SXINT ste, SXINT next_state)
{
  SXINT            triple, triple2, comment_ste, semlex_ste;
  struct sxtoken *ptok, *semlex_ptok;

  sxuse (semlex_ptok_ptr);

  if (ptok_ptr == NULL && ste == 0) {
    /* Transition epsilon */
    cur_fsa->has_epsilon_trans = SXTRUE;
    XxYxZ_set (&(cur_fsa->fsa_hd), state, 0, next_state, &triple);
#if EBUG
    printf ("%ld\t\"<EPSILON>\"\t\t%ld\n", (long)state, (long)next_state);
#endif
  }
  else {
    /* Pour l'instant on n'exploite pas ptok_ptr qui contient {NULL, ptok1, ...ptokm, NULL}
       ou chaque ptoki repere un sxtoken dans l'arbre abstrait dont le string_table_entry est ste */
    if (ste == cur_fsa->eof_ste)
      cur_fsa->final_state = next_state;

    if ((ptok = *++ptok_ptr) != NULL) {
      do {
	if (ptok->comment) 
	  comment_ste = sxstrsave (ptok->comment);
	else
	  comment_ste = 0;

	if (semlex_ptok_ptr != NULL) {
	  semlex_ptok = *++semlex_ptok_ptr;
	  if (semlex_ptok)
	    semlex_ste = semlex_ptok->string_table_entry;
	  else 
	    semlex_ste = 0;
	}
	else
	  semlex_ste = 0;

	if (!XxYxZ_set (&input_transitions_hd, comment_ste, ste, semlex_ste, &triple)) {
	  DPUSH (input_transition_stack, triple);
	  XxYxZ_set (&(cur_fsa->fsa_hd), state, triple, next_state, &triple2);
#if EBUG
	  printf ("%ld\t%s \"%s\" %s\t\t%ld\n", (long)state, sxstrget (comment_ste), sxstrget (ste), sxstrget (semlex_ste), (long)next_state);
#endif
	} else {
	  SXINT idx;

	  for (idx = DTOP (input_transition_stack); idx > 0; idx--) {
	    if (input_transition_stack [idx] == triple)
	      break;
	  }
	  if (idx == 0) {
	    DPUSH (input_transition_stack, triple);
	    XxYxZ_set (&(cur_fsa->fsa_hd), state, triple, next_state, &triple2);
#if EBUG
	    printf ("%ld\t%s \"%s\" %s\t\t%ld\n", (long)state, sxstrget (comment_ste), sxstrget (ste), sxstrget (semlex_ste), (long)next_state);
#endif
	  }	  
	}
      } while ((ptok = *++ptok_ptr) != NULL);
    }
    DRAZ (input_transition_stack);
  }
}


static void
raz_re (void)
{
  if (XxYxZ_is_allocated (cur_fsa->fsa_hd)) {
    XxYxZ_free (&(cur_fsa->fsa_hd));
  }
}



static void
DFA_extract_trans (SXINT dfa_state, void (*output_trans) (SXINT, SXINT, SXINT))
{
  SXINT triple;

  XxYxZ_Xforeach (cur_fsa->fsa_hd, dfa_state, triple)
    (*output_trans) (dfa_state, XxYxZ_Y (cur_fsa->fsa_hd, triple), XxYxZ_Z (cur_fsa->fsa_hd, triple));
}


static XxYxZ_header minDFA_hd;
static SXINT          minDFA_foreach [] = {1 /* X pour minDFA_extract_trans */, 0, 0, 0, 1 /* XZ pour minDFA_edges */, 0};
static SXINT          minDFA_final_state;
static SXBOOLEAN        is_a_dag;

static void
minDFA_alloc (void)
{
  is_a_dag = SXTRUE; /* A priori */
  XxYxZ_alloc (&minDFA_hd, "minDFA_hd", fsa_trans_nb+1, 1, minDFA_foreach, NULL, NULL);
}

static void
minDFA_fill_trans (SXINT dfa_state, SXINT triple, SXINT next_dfa_state)
{
  SXINT semlex_ste;

  if (next_dfa_state == cur_fsa->final_state)
    minDFA_final_state = --next_dfa_state;

  if (output_udag
#if EBUG
      || 1
#endif /* EBUG */
      ) {
    if (dfa_state < next_dfa_state) {
#if LOG
      total_trans_nb++;
#endif /* LOG */
      semlex_ste = XxYxZ_Z (input_transitions_hd, triple);
      printf ("%ld\t%s %s %s%s%s\t%ld\n", (long)dfa_state, 
	      sxstrget (XxYxZ_X (input_transitions_hd, triple)), 
	      sxstrget (XxYxZ_Y (input_transitions_hd, triple)),
	      semlex_ste ? "[|": "",
	      sxstrget (semlex_ste),
	      semlex_ste ? "|]": "",
	      (long)next_dfa_state);
    }
  }
  else {
#if LOG
    total_trans_nb++;
#endif /* LOG */
    
    if (XxYxZ_is_allocated (minDFA_hd)) {
      if (dfa_state != next_dfa_state)
	XxYxZ_set (&minDFA_hd, dfa_state, triple, next_dfa_state, &triple);
      
      if (next_dfa_state < dfa_state)
	is_a_dag = SXFALSE;
    }
  }
}


static VARSTR w_vstr;

static void
minDFA_extract_trans (SXINT dfa_state, void (*output_trans) (SXINT, SXINT, SXINT))
{
  SXINT triple;

  XxYxZ_Xforeach (minDFA_hd, dfa_state, triple)
    (*output_trans) (dfa_state, XxYxZ_Y (minDFA_hd, triple), XxYxZ_Z (minDFA_hd, triple));
}

static char*
get_trans_name (SXINT t)
{
  return sxstrget (t);
}


/* Cette fonction est appelee après les appels à store_[u]re(). */
/* Elle utilise l'automate lu */
static SXINT
postlude_re (SXINT fsa_kind)
{
  sxuse (fsa_kind);
  fsa_trans_nb = cur_fsa->transition_nb = XxYxZ_top (cur_fsa->fsa_hd);

#if LOG
  sprintf (mess, "RE2DFA: state_nb = %ld", (long)cur_fsa->final_state);
  total_trans_nb = 0;
  sxtime (SXACTION, mess);
#endif

#if EBUG
  fputs ("DFA2min_DFA\n", stdout);
#endif
  /* fsa_trans_nb est celui de cur_fsa */
  if (!output_udag)
    minDFA_alloc ();
#if !EBUG
  else
#endif /* !EBUG */
    puts ("##DAG BEGIN");

  dfa_minimize (1, cur_fsa->final_state, cur_fsa->transition_nb, DFA_extract_trans, minDFA_fill_trans, SXTRUE /* to_be_normalized */
#ifdef ESSAI_INVERSE_MAPPING
		     , NULL
#endif /* ESSAI_INVERSE_MAPPING */
		);  // c'est lui qui sort le udag si output_udag

  if (output_udag)
    puts ("##DAG END");

#if LOG
  sprintf (mess, "DFA2min_DFA: state_nb = %ld, total_trans_nb = %ld", (long)minDFA_final_state, (long)total_trans_nb);
  total_trans_nb = 0;
  sxtime (SXACTION, mess);
#endif

  //  minDFA2RE ();

  if (!output_udag) {
    if (is_a_dag) {
      /* Si le minDFA est un DAG on peut (aussi) sortir l'ER par dag2re () */
      w_vstr = varstr_alloc (512);
      w_vstr = dag2re (w_vstr, 1, minDFA_final_state, cur_fsa->eof_ste, minDFA_extract_trans, get_trans_name);
      printf ("%s\n", varstr_tostr (w_vstr));
      varstr_free (w_vstr), w_vstr = NULL;
    } else
      sxtrap (ME, "postlude_re: can't make a DAG out of a non-DAGable RE (which is strange: the input was a DAG to be minimized...)");

    XxYxZ_free (&minDFA_hd);
  }
  
  XxYxZ_clear (&input_transitions_hd);
  raz_re ();

  return 0;
}




/* C'est la fonction qui appelle read_a_re. Le resultat de la lecture de la re est traite' par les fonctions prelude_re, store_re et postlude_re */
static SXINT
fsa_processor (void)
{
  int severity;

  severity = read_a_re (prelude_re,
			store_re, 
			postlude_re,
			RE2DFA|RE_IS_A_DAG|RE_INCREASE_ORDER /* what_to_do */); /* On ne met pas "|RE_USE_COMMENTS" car on va utiliser les tokens ds store_re */

  return severity;
}

/************************************************************************/
/* main function                                                        */
/************************************************************************/
int
main (int argc, char *argv[])
{
  int           argnum;
  SXBOOLEAN	in_options, is_source_file, is_stdin;

  sxopentty ();

  /* valeurs par defaut */
  output_udag = is_stdin = SXTRUE;
  is_help = SXFALSE;
  argnum = 0;

  is_source_file = SXFALSE;
  tmp_file_for_stdin = SXFALSE;

  if (argc > 1) {
    /* Decodage des options */
    in_options = SXTRUE;

    while (in_options && ++argnum < argc) {
      switch (option_get_kind (argv [argnum])) {
      case VERBOSE:
	sxverbosep = SXTRUE;
	break;

      case -VERBOSE:
	sxverbosep = SXFALSE;
	break;

      case UNFOLD:
	output_udag = SXTRUE;
	break;

      case -UNFOLD:
	output_udag = SXFALSE;
	break;

      case HELP:
	is_help = SXTRUE;
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
	fprintf (sxstderr, "%s: unknown option \"%s\".\n", ME, argv [argnum]);
	SXEXIT (3);
      default:
#if EBUG
	sxtrap ("dag2udag_main.c","unknown switch case");
#endif
	break;
      }
    }
  }

  if ((!is_stdin && !is_source_file) || is_help || (argnum < argc - 1)) {
    fprintf (sxstderr, Usage, ME);
    SXEXIT (3);
  }

  {
    int	severity;
    SXINT empty_foreach [6] = {0, 0, 0, 0, 0, 0};

    sxstr_mngr (SXBEGIN);
    sxerr_mngr (SXBEGIN);

    XxYxZ_alloc (&input_transitions_hd, "input_transitions_hd", 100, 1, empty_foreach, NULL, NULL);
    DALLOC_STACK (input_transition_stack, 10);

    if (is_stdin)
      severity = re_reader (NULL, SXTRUE, fsa_processor);
    else
      severity = re_reader (argv [argnum], SXTRUE, fsa_processor);

    XxYxZ_free (&input_transitions_hd);
    DFREE_STACK (input_transition_stack);

    sxstr_mngr (SXEND);
    sxerr_mngr (SXEND);
    
    SXEXIT (severity);
  }
  return 0;
}

