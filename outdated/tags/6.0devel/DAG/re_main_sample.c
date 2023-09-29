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
/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 





/* Exemple d'utilisation de read_a_re et fsa_mngr */




#include "sxunix.h"

char WHAT_SXMAIN[] = "@(#)SYNTAX - $Id: re_main_sample.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

static char ME[] = "re_main";

#include "sxstack.h"
#include "varstr.h"
#include "fsa.h"
#include "XxYxZ.h"
#include "XxY.h"

/* On est dans un cas "mono-langage": */

static char mess [128];

static struct fa {
  SXINT          fsa_kind, init_state, final_state, sigma_card, eof_ste, transition_nb;
  XxYxZ_header fsa_hd;
  SXBOOLEAN      has_epsilon_trans;
} nfa, efnfa, dfa, *cur_fsa;


static SXINT        dfa_final_state;
static XxY_header XxY_out_trans;
static SXINT        *XxY_out_trans2next_state;
static SXINT        fsa_trans_nb;

#define WHAT_TO_DO     (RE2NFA|RE2EFNFA|RE2DFA|RE_USE_COMMENTS|RE_INCREASE_ORDER)




/* Cette procedure est appelee depuis read_a_re a` la fin de la 1ere passe sur l'arbre abstrait */
/* Les arguments sont 
   - SXTRUE => OK
   - ste des transitions vers l'etat final
   - le nb de noeuds de cet arbre et 
   - le nb d'operande (dont eof + 1 bidon de fin) */
/* Peut servir a faire des alloc + ciblees */
static void
prelude_re (SXINT is_OK, SXINT eof_ste, SXINT node_nb, SXINT operand_nb, SXINT fsa_kind)
{
  SXINT *fsa_foreach;
#if EBUG
  char *name;
#endif

  if (is_OK) {
#if LOG
    sxtime (SXINIT, NULL);
#endif /* LOG */

    /* L'ER est correcte (en particulier c'est un DAG si on a demande' un DAG) */
    /* fsa_kind == RE2NFA || kind == RE2EFNFA || kind == RE2DFA */
    switch (fsa_kind) {
    case RE2NFA :
      /* L'automate ds nfsa_hd est non-deterministe avec des transitions vides */
      {
	SXINT foreach [6] = {1, 0, 0, 1, 0, 0};
#if EBUG
	name = "RE2NFA";
#endif
	fsa_foreach = foreach;
	cur_fsa = &nfa;
      }
      break;

    case RE2EFNFA :
      /* L'automate ds nfsa_hd est non-deterministe sans transitions vides */
      {
	SXINT foreach [6] = {1, 0, 0, 0, 0, 0};
#if EBUG
	name = "RE2EFNFA";
#endif
	fsa_foreach = foreach;
	cur_fsa = &efnfa;
      }
      break;

    case RE2DFA :
      /* L'automate ds nfsa_hd est deterministe (mais non minimal) ... */
      /* ... sauf ds le cas d'un DAG */
      {
	SXINT foreach [6] = {1, 0, 0, 0, 0, 0};
#if EBUG
	name = "RE2DFA";
#endif
	fsa_foreach = foreach;
	cur_fsa = &dfa;
      }
    break;
    }

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
    cur_fsa->has_epsilon_trans = SXTRUE;
    XxYxZ_set (&(cur_fsa->fsa_hd), state, 0, next_state, &triple);
#if EBUG
    printf ("%i\t\"<EPSILON>\"\t\t%i\n", state, next_state);
#endif
  }
  else {
    /* Pour l'instant on n'exploite pas ptok_ptr qui contient {NULL, ptok1, ...ptokm, NULL}
       ou chaque ptoki repere un sxtoken dans l'arbre abstrait dont le string_table_entry est ste */
    if (ste == cur_fsa->eof_ste)
      cur_fsa->final_state = next_state;

    XxYxZ_set (&(cur_fsa->fsa_hd), state, ste, next_state, &triple);
#if EBUG
    printf ("%i\t\"%s\"\t\t%i\n", state, sxstrget (ste), next_state);
#endif
  }
}


static void
raz_re ()
{
  if (XxYxZ_is_allocated (cur_fsa->fsa_hd)) {
    XxYxZ_free (&(cur_fsa->fsa_hd));
  }
}


/* On met ds next_nfa_state_set les etats atteint par transition vide depuis nfa_state */
static SXBOOLEAN
nfa_empty_trans (SXINT nfa_state, SXBA next_nfa_state_set)
{
  SXINT     triple, next_nfa_state;
  SXBOOLEAN ret_val = SXFALSE;

  XxYxZ_XYforeach (cur_fsa->fsa_hd, nfa_state, 0, triple) {
    ret_val = SXTRUE;
    next_nfa_state = XxYxZ_Z (cur_fsa->fsa_hd, triple);
    SXBA_1_bit (next_nfa_state_set, next_nfa_state);
  }

  return ret_val;
}


/* appel f avec les transitions (t, next_state, t non vide) depuis nfa_state */
static void
nfa_extract_trans (SXINT nfa_state, void (*f)())
{
  SXINT triple, t, next_nfa_state;

  XxYxZ_Xforeach (cur_fsa->fsa_hd, nfa_state, triple) {
    t = XxYxZ_Y (cur_fsa->fsa_hd, triple);

    if (t != 0) {
      next_nfa_state = XxYxZ_Z (cur_fsa->fsa_hd, triple);
      (*f)(nfa_state, t, next_nfa_state);
    }
  }
}

static SXINT final_state, total_trans_nb;

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
dfa_extract_trans (SXINT dfa_state, void (*output_trans) ())
{
  SXINT stateXt;

  XxY_Xforeach (XxY_out_trans, dfa_state, stateXt)
    (*output_trans) (dfa_state, XxY_Y (XxY_out_trans, stateXt), XxY_out_trans2next_state [stateXt]);
}


static void
DFA_extract_trans (SXINT dfa_state, void (*output_trans) ())
{
  SXINT triple;

  XxYxZ_Xforeach (cur_fsa->fsa_hd, dfa_state, triple)
    (*output_trans) (dfa_state, XxYxZ_Y (cur_fsa->fsa_hd, triple), XxYxZ_Z (cur_fsa->fsa_hd, triple));
}

static void
XxY_out_trans_oflw (SXINT old_size, SXINT new_size)
{
  XxY_out_trans2next_state = (SXINT*) sxrealloc (XxY_out_trans2next_state, new_size+1, sizeof (SXINT));
}

static XxYxZ_header minDFA_hd;
static SXINT          minDFA_foreach [] = {1 /* X pour minDFA_extract_trans */, 0, 0, 0, 1 /* XZ pour minDFA_edges */, 0};
static SXINT          minDFA_final_state;
static SXBOOLEAN        is_a_dag;

static void
minDFA_alloc ()
{
  is_a_dag = SXTRUE; /* A priori */
  XxYxZ_alloc (&minDFA_hd, "minDFA_hd", fsa_trans_nb+1, 1, minDFA_foreach, NULL, NULL);
}

static void
minDFA_fill_trans (SXINT dfa_state, SXINT t, SXINT next_dfa_state)
{
  SXINT triple;

#if EBUG
  printf ("%i\t\"%s\"\t\t%i\n", dfa_state, sxstrget (t), next_dfa_state, t == cur_fsa->eof_ste ? "(final)" : "");
#endif

#if LOG
  total_trans_nb++;

  if (t == cur_fsa->eof_ste)
    final_state = next_dfa_state;
#endif /* LOG */

  if (XxYxZ_is_allocated (minDFA_hd)) {
    XxYxZ_set (&minDFA_hd, dfa_state, t, next_dfa_state, &triple);

    if (next_dfa_state <= dfa_state)
      is_a_dag = SXFALSE;

    if (t == cur_fsa->eof_ste)
     minDFA_final_state = next_dfa_state;
  }
}


static VARSTR w_vstr;
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
minDFA2RE ()
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


static void
minDFA_extract_trans (SXINT dfa_state, void (*output_trans) ())
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
  /* fsa_kind == RE2NFA || kind == RE2EFNFA || kind == RE2DFA */

  fsa_trans_nb = cur_fsa->transition_nb = XxYxZ_top (cur_fsa->fsa_hd);

  if (fsa_kind & (RE2NFA|RE2EFNFA)) {
    XxY_alloc (&XxY_out_trans, "XxY_out_trans", cur_fsa->final_state+1, 1, 1, 0, XxY_out_trans_oflw, NULL);
    XxY_out_trans2next_state = (SXINT*) sxalloc (XxY_size (XxY_out_trans)+1, sizeof (SXINT));
  }

  switch (fsa_kind) {
  case RE2NFA :
    /* L'automate ds nfsa_hd est non-deterministe avec des transitions vides */
    /* On le transforme en automate deterministe */
#if LOG
    sprintf (mess, "RE2NFA: state_nb = %i", cur_fsa->final_state);
    sxtime (SXACTION, mess);
    total_trans_nb = 0;
#endif
#if EBUG
    fputs ("NFA2DFA\n", stdout);
#endif

    nfa2dfa (cur_fsa->init_state, cur_fsa->final_state, cur_fsa->eof_ste, cur_fsa->has_epsilon_trans ? nfa_empty_trans : NULL, nfa_extract_trans,
	     dfa_fill_trans, NULL /* pas de min */, SXFALSE /* ... et donc to_be_normalized est sans objet */);
#if LOG
    sprintf (mess, "NFA2DFA: state_nb = %i, total_trans_nb = %i", final_state, total_trans_nb);
    total_trans_nb = 0;
    sxtime (SXACTION, mess);
#endif

#if EBUG
    fputs ("DFA2min_DFA\n", stdout);
#endif
    fsa_trans_nb = XxY_top (XxY_out_trans);
    minDFA_alloc ();
    dfa_minimize (1, dfa_final_state, cur_fsa->eof_ste, dfa_extract_trans, minDFA_fill_trans, SXTRUE /* to_be_normalized */);
#if LOG
    sprintf (mess, "DFA2min_DFA: state_nb = %i, total_trans_nb = %i", final_state, total_trans_nb);
    total_trans_nb = 0;
    sxtime (SXACTION, mess);
#endif

    break;

  case RE2EFNFA :
    /* L'automate ds nfsa_hd est non-deterministe sans transitions vides */
#if LOG
    sprintf (mess, "RE2EFNFA: state_nb = %i", cur_fsa->final_state);
    sxtime (SXACTION, mess);
    total_trans_nb = 0;
#endif
#if EBUG
    fputs ("EFNFA2DFA\n", stdout);
#endif

    nfa2dfa (cur_fsa->init_state, cur_fsa->final_state, cur_fsa->eof_ste, NULL, nfa_extract_trans, dfa_fill_trans, NULL /* pas de min */, SXFALSE /* ... et donc to_be_normalized est sans objet */);
#if LOG
    sprintf (mess, "EFNFA2DFA: state_nb = %i, total_trans_nb = %i", final_state, total_trans_nb);
    total_trans_nb = 0;
    sxtime (SXACTION, mess);
#endif

#if EBUG
    fputs ("EFDFA2min_DFA\n", stdout);
#endif
    fsa_trans_nb = XxY_top (XxY_out_trans);
    minDFA_alloc ();
    dfa_minimize (1, dfa_final_state, cur_fsa->eof_ste, dfa_extract_trans, minDFA_fill_trans, SXTRUE /* to_be_normalized */);
#if LOG
    sprintf (mess, "EFDFA2min_DFA: state_nb = %i, total_trans_nb = %i", final_state, total_trans_nb);
    total_trans_nb = 0;
    sxtime (SXACTION, mess);
#endif

    break;

  case RE2DFA :
    /* L'automate ds nfsa_hd est deterministe (mais non minimal) ... */
    /* ... sauf ds le cas d'un DAG */
#if LOG
    sprintf (mess, "RE2DFA: state_nb = %i", cur_fsa->final_state);
    total_trans_nb = 0;
    sxtime (SXACTION, mess);
#endif
#if EBUG
    fputs ("DFA2min_DFA\n", stdout);
#endif
    /* fsa_trans_nb est celui de cur_fsa */
    minDFA_alloc ();
    dfa_minimize (1, cur_fsa->final_state, cur_fsa->eof_ste, DFA_extract_trans, minDFA_fill_trans, SXTRUE /* to_be_normalized */);
#if LOG
    sprintf (mess, "DFA2min_DFA: state_nb = %i, total_trans_nb = %i", final_state, total_trans_nb);
    total_trans_nb = 0;
    sxtime (SXACTION, mess);
#endif

    break;
  }

  minDFA2RE ();

  if (is_a_dag) {
    /* Si le minDFA est un DAG on peut (aussi) sortir l'ER par dag2re () */
    w_vstr = varstr_alloc (512);
    w_vstr = dag2re (w_vstr, 1, minDFA_final_state, cur_fsa->eof_ste, minDFA_extract_trans, get_trans_name);
    printf ("min DAG to RE:\n%s\n", varstr_tostr (w_vstr));
    varstr_free (w_vstr), w_vstr = NULL;
  }

  XxYxZ_free (&minDFA_hd);

  if (fsa_kind & (RE2NFA|RE2EFNFA)) {
    XxY_free (&XxY_out_trans);
    sxfree (XxY_out_trans2next_state), XxY_out_trans2next_state = NULL;
  }
  
  raz_re ();
}

/* C'est la fonction qui appelle read_a_re. Le resultat de la lecture de la re est traite' par les fonctions prelude_re, store_re et postlude_re */
static SXINT
fsa_processor ()
{
  SXINT severity;

  severity = read_a_re (prelude_re,
			store_re, 
			postlude_re,
			WHAT_TO_DO);

  return severity;
}



int
main(int argc, char *argv[])
{
  sxopentty ();

  if (argc == 1) {
    /* Pas d'argument: on lit sur stdin */
    re_reader (NULL, SXTRUE, fsa_processor);
  }
  else {
    /* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
    int	argnum;

    for (argnum = 1; argnum < argc; argnum++)
      if (argv [argnum] [0] == '-' && argv [argnum] [1] == '\0') {
	re_reader (NULL, SXTRUE, fsa_processor);
      }
      else {
	re_reader (argv [argnum], SXTRUE, fsa_processor);
      }
  }

  SXEXIT (sxerr_max_severity ());

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

