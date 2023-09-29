/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 2005 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe ATOLL.			  *
   *							  *
   ******************************************************** */





/* Main module for a dag unfolder (dag->udag) */


#define SX_DFN_EXT_VAR

#include "sxunix.h"
#include "sxstack.h"
#include "varstr.h"
#include "fsa.h"
#include "XxYxZ.h"
#include "XxY.h"

#define WHAT	"@(#)dag2udag_main.c\t- SYNTAX [unix] - 23 mars 2004"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

#if LOG
static char mess [128];
static SXINT total_trans_nb;
#endif

void             (*main_parser)(void); /* NULL pas de earley ou autre */

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

/*---------------*/
/*    options    */
/*---------------*/

extern BOOLEAN		sxverbosep;
#endif

BOOLEAN		output_udag;
static BOOLEAN  is_help;
static char	ME [] = "dag2udag_main";
static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\t-v, -verbose, -nv, -noverbose,\n\
\t-d, -dag, -u, -udag,\n\
\t--help,\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define VERBOSE 	  1
#define UNFOLD	          2
#define HELP	          3
#define SOURCE_FILE	  4
#define LAST_OPTION	  SOURCE_FILE


static char	*option_tbl [] = {
    "",
    "v", "verbose", "nv", "noverbose",
    "d", "dag", "u", "udag",
    "-help",
};

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
    -UNFOLD, -UNFOLD, UNFOLD, UNFOLD,
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
  BOOLEAN      has_epsilon_trans;
} dfa, *cur_fsa;


#if 0
static SXINT        dfa_final_state;
static XxY_header XxY_out_trans;
static SXINT        *XxY_out_trans2next_state;
#endif /* 0 */
static SXINT        fsa_trans_nb;




/* Cette procedure est appelee depuis read_a_re a` la fin de la 1ere passe sur l'arbre abstrait */
/* Les arguments sont 
   - TRUE => OK
   - ste des transitions vers l'etat final
   - le nb de noeuds de cet arbre et 
   - le nb d'operande (dont eof + 1 bidon de fin) */
/* Peut servir a faire des alloc + ciblees */
static void
prelude_re (BOOLEAN is_OK, SXINT eof_ste, SXINT node_nb, SXINT operand_nb, SXINT fsa_kind)
{
  SXINT *fsa_foreach;
#if EBUG
  char *name;
#endif
    SXINT foreach [6] = {1, 0, 0, 0, 0, 0};

    sxuse (operand_nb);
  if (is_OK) {
#if LOG
    sxtime (INIT, NULL);
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
}

/* Cette procedure est appelee depuis read_a_re sur chaque transition de l'automate produit */
/* L'etat initial est 1 */
/* l'etat final unique est atteint par transition sur le terminal bidon "<EOF>" de ste eof_ste et vaut -f */
/* Tous les etats de 1 a f sont utilises */
/* ptok_ptr contient {NULL, ptok1, ...ptokm, NULL} */
/* ste est commun a tous les ptok de ptok_ptr */
/* ptoki est un pointeur vers un token courant (dans le noeud de l'arbre abstrait) */
static void
store_re (SXINT state, struct sxtoken **ptok_ptr, SXINT ste, SXINT next_state)
{
  SXINT            triple;

  if (ptok_ptr == NULL && ste == 0) {
    /* Transition epsilon */
    cur_fsa->has_epsilon_trans = TRUE;
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

    XxYxZ_set (&(cur_fsa->fsa_hd), state, ste, next_state, &triple);
#if EBUG
    printf ("%ld\t\"%s\"\t\t%ld\n", (long)state, sxstrget (ste), (long)next_state);
#endif
  }
}


static void
raz_re (void)
{
  if (XxYxZ_is_allocated (cur_fsa->fsa_hd)) {
    XxYxZ_free (&(cur_fsa->fsa_hd));
  }
}


#if 0
static void
dfa_fill_trans (SXINT dfa_state, SXINT t, SXINT new_dfa_state, SXINT is_final)
{
  SXINT id;

#if EBUG
  printf ("%i\t\"%s\"\t\t%i\n", dfa_state, sxstrget (t), new_dfa_state, is_final ? "(final)" : "");
#endif

#if LOG
  total_trans_nb++;

  if (is_final)
    final_state = new_dfa_state;
#endif /* LOG */

  /* On stocke le DFA pour la minimisation */
  XxY_set (&XxY_out_trans, dfa_state, t, &id);
  XxY_out_trans2next_state [id] = new_dfa_state;

  if (is_final)
    dfa_final_state = new_dfa_state;
}

static void
dfa_extract_trans (SXINT dfa_state, void (*output_trans) (SXINT, SXINT, SXINT))
{
  SXINT stateXt;

  XxY_Xforeach (XxY_out_trans, dfa_state, stateXt)
    (*output_trans) (dfa_state, XxY_Y (XxY_out_trans, stateXt), XxY_out_trans2next_state [stateXt]);
}
#endif /* 0 */

static void
DFA_extract_trans (SXINT dfa_state, void (*output_trans) (SXINT, SXINT, SXINT))
{
  SXINT triple;

  XxYxZ_Xforeach (cur_fsa->fsa_hd, dfa_state, triple)
    (*output_trans) (dfa_state, XxYxZ_Y (cur_fsa->fsa_hd, triple), XxYxZ_Z (cur_fsa->fsa_hd, triple));
}

#if 0
static void
XxY_out_trans_oflw (SXINT old_size, SXINT new_size)
{
  sxuse (old_size);
  XxY_out_trans2next_state = (SXINT*) sxrealloc (XxY_out_trans2next_state, new_size+1, sizeof (SXINT));
}
#endif /* 0 */

static XxYxZ_header minDFA_hd;
static SXINT          minDFA_foreach [] = {1 /* X pour minDFA_extract_trans */, 0, 0, 0, 1 /* XZ pour minDFA_edges */, 0};
static SXINT          minDFA_final_state;
static BOOLEAN        is_a_dag;

static void
minDFA_alloc (void)
{
  is_a_dag = TRUE; /* A priori */
  XxYxZ_alloc (&minDFA_hd, "minDFA_hd", fsa_trans_nb+1, 1, minDFA_foreach, NULL, NULL);
}

static void
minDFA_fill_trans (SXINT dfa_state, SXINT t, SXINT next_dfa_state)
{
  SXINT triple;

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
      printf ("%ld\t%s\t%ld\n", (long)dfa_state, sxstrget (t), (long)next_dfa_state);
    }
  }
  else {
#if LOG
    total_trans_nb++;
#endif /* LOG */
    
    if (XxYxZ_is_allocated (minDFA_hd)) {
      if (dfa_state != next_dfa_state)
	XxYxZ_set (&minDFA_hd, dfa_state, t, next_dfa_state, &triple);
      
      if (next_dfa_state < dfa_state)
	is_a_dag = FALSE;
    }
  }
}


static VARSTR w_vstr;
#if 0
static char   *meta_symb [6] = {"(", ")*", "(", ")+", "(", ")?"}; /* Pour le nouveau fsa2re */

static SXINT
minDFA_edges (SXINT i, SXINT j, char **string)
{
  SXINT    triple, t, prev_t, ste;

  prev_t = 0;

  XxYxZ_XZforeach (minDFA_hd, i, j, triple) {
    t = XxYxZ_Y (minDFA_hd, triple);

    if (prev_t == 0)
      prev_t = t;
    else {
      if (prev_t > 0) {
	varstr_raz (w_vstr);
	w_vstr = varstr_lcatenate (w_vstr, "(", 1);
	w_vstr = varstr_catenate (w_vstr, sxstrget (prev_t));
	prev_t = -1;
      }
      
      w_vstr = varstr_lcatenate (w_vstr, "|", 1);
      w_vstr = varstr_catenate (w_vstr, sxstrget (t));
    }
  }

  if (prev_t > 0) {
    ste = prev_t;
  }
  else {
    w_vstr = varstr_lcatenate (w_vstr, ")", 1);
    ste = sxstr2save (varstr_tostr (w_vstr), varstr_length (w_vstr));
  }

  *string = sxstrget (ste);

  return 0;
}

static void
minDFA2RE (void)
{
  SXINT          triple, state, next_state;
  SXBA         *minDFA_R, *minDFA_R_plus;
  VARSTR       vstr;

  minDFA_R = sxbm_calloc (minDFA_final_state+1, minDFA_final_state+1);
  minDFA_R_plus = sxbm_calloc (minDFA_final_state+1, minDFA_final_state+1);

  for (triple = XxYxZ_top (minDFA_hd); triple >= 1; triple--) {
    state = XxYxZ_X (minDFA_hd, triple);
    next_state = XxYxZ_Z (minDFA_hd, triple);
    SXBA_1_bit (minDFA_R_plus [state], next_state);
    SXBA_1_bit (minDFA_R [state], next_state);
  }

  fermer2 (minDFA_R_plus, minDFA_final_state+1);
    
  vstr = varstr_alloc (512);
  w_vstr = varstr_alloc (128);
  vstr = fsa2re (vstr, minDFA_R, minDFA_R_plus, minDFA_final_state, 1, minDFA_final_state, minDFA_edges, &(meta_symb [0]));

  printf ("%s\n", varstr_tostr (vstr));

  sxbm_free (minDFA_R);
  sxbm_free (minDFA_R_plus);
  varstr_free (vstr), vstr = NULL;
  varstr_free (w_vstr), w_vstr = NULL;
}
#endif /* 0 */


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
static void
postlude_re (SXINT fsa_kind)
{
  sxuse (fsa_kind);
  fsa_trans_nb = cur_fsa->transition_nb = XxYxZ_top (cur_fsa->fsa_hd);

#if LOG
  sprintf (mess, "RE2DFA: state_nb = %ld", (long)cur_fsa->final_state);
  total_trans_nb = 0;
  sxtime (ACTION, mess);
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

  dfa_minimize (1, cur_fsa->final_state, cur_fsa->eof_ste, DFA_extract_trans, minDFA_fill_trans, TRUE /* to_be_normalized */);  // c'est lui qui sort le udag si output_udag

  if (output_udag)
    puts ("##DAG END");

#if LOG
  sprintf (mess, "DFA2min_DFA: state_nb = %ld, total_trans_nb = %ld", (long)minDFA_final_state, (long)total_trans_nb);
  total_trans_nb = 0;
  sxtime (ACTION, mess);
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
  
  raz_re ();
}




/* C'est la fonction qui appelle read_a_re. Le resultat de la lecture de la re est traite' par les fonctions prelude_re, store_re et postlude_re */
static int
fsa_processor (void)
{
  int severity;

  severity = read_a_re (prelude_re,
			store_re, 
			postlude_re,
			RE2DFA|RE_USE_COMMENTS|RE_INCREASE_ORDER /* what_to_do */);

  return severity;
}

/************************************************************************/
/* main function                                                        */
/************************************************************************/
int
main (int argc, char *argv[])
{
  int           argnum;
  BOOLEAN	in_options, is_source_file, is_stdin;

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
  output_udag = is_stdin = TRUE;
  is_help = FALSE;
  argnum = 0;

  is_source_file = FALSE;

  if (argc > 1) {
    /* Decodage des options */
    in_options = TRUE;

    while (in_options && ++argnum < argc) {
      switch (option_get_kind (argv [argnum])) {
      case VERBOSE:
	sxverbosep = TRUE;
	break;

      case -VERBOSE:
	sxverbosep = FALSE;
	break;

      case UNFOLD:
	output_udag = TRUE;
	break;

      case -UNFOLD:
	output_udag = FALSE;
	break;

      case HELP:
	is_help = TRUE;
	break;

      case SOURCE_FILE:
	if (is_stdin) {
	  is_stdin = FALSE;
	}

	is_source_file = TRUE;
	in_options = FALSE;
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

    sxstr_mngr (BEGIN);
    sxerr_mngr (BEGIN);

    if (is_stdin)
      severity = re_reader (NULL, TRUE, fsa_processor);
    else
      severity = re_reader (argv [argnum], TRUE, fsa_processor);

    sxstr_mngr (END);
    sxerr_mngr (END);
    
    SXEXIT (severity);
  }
  return 0;
}

SXINT
sxivoid (void)
/* procedure ne faisant rien */
{
  return 1;
}
