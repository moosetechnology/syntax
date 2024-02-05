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

/* Le 21/6/2002 : ATTENTION, il semble que LC1 doive s'utiliser avec xbnf RULE_TO_REDUCE
   doit contenir des post_actions, or avec bnf, il peut contenir des duplicats de regles
   dus au traitement des xnt.
*/

/*
   Constructeur d'automates de la classe Left-Corner utilisables avec un analyseur
   a la Earley.
   Look-Ahead de 1 token de type SLR:
      LA (A -> \alpha . \beta) = 1:First1(\beta) Follow1(A)
*/
/* Si l'option "-plr" est positionnee on active, en plus du constructeur left-corner un
   constructeur d'automates de la classe Predictive LR utilisables avec un analyseur
   a la Earley.
   PLR = (Q, V, \delta, p0, F)
       - V = N U T
       - Q = {S' -> } U {A -> \alpha | A -> \alpha \beta \in P' et \alpha != \epsilon}
       - p0 = S' -> 
       - \forall A -> \alpha \in Q :
          closure (A -> \alpha) = {[B -> \beta . \gamma] | [B -> \beta . \gamma] \in
	                                                   closure ([A -> \alpha . \beta])}
       - B -> \beta X \in \delta (A -> \alpha, X) <=> [B -> \beta .X \gamma] \in closure (A -> \alpha)
       - F = {A -> \alpha | [A -> \alpha .] \in closure (A -> \alpha)}

   Look-Ahead de 1 token de type SLR:
      LA (A -> \alpha) = 1:First1([A -> \alpha . \beta]) Follow1(A)
      
   Apres divers tests d'analyse utilisant differentes formes de l'automate PLR et differents
   codages du reconnaisseur correspondant, les resultats (temps d'analtyse) ont toujours ete plus mauvais
   qu'avec le constructeur LC. Je decide donc de ne pas implanter de constructeur PLR.
   Je ne regarde meme pas l'automate Common-Prefix, ca doit etre encore pire...

*/



static char	ME [] = "LC (1)";

#include "sxversion.h"
#include "lc.h"
#include "bstr.h"
#include "sxstack.h"
#include "XH.h"
#include "sxmatrix2vector.h"
char WHAT_LC1[] = "@(#)SYNTAX - $Id: LC1.c 3652 2023-12-24 09:43:15Z garavel $" WHAT_DEBUG;

extern struct bstr	*options_text (struct bstr *bstr);

static SXINT	limt, limxt, limnt, limpro, limitem, my_eof;
static SXINT	*lhs, *lispro, *prolis, *prdct, *prolon, *semact;

static SXBA	*FOLLOW, *FIRST, *item2t_set, *t2item_set, *nt2item_set, *left_corner, bvide, *nt2rc_item_set, *right_corner;
static SXBA	*right_recursive, *cyclic;
static SXINT	*rrnt2nt, *ccnt2nt, *order2prod, *prod2order, *prod_order2min, *prod_order2max,
                *item2nbt, *item2nbnt;
static SXBA	is_mutual_right_recursive_prod;
static SXINT	*prod2nbnt;
static SXBA	*cycle_nb2item_set;
static SXINT	*item2cycle_nb;
static SXINT	rrnt, ccnt, rhs_maxnt, rhs_lgth;
static bool	is_epsilon;
static SXINT	*tnb_list, **tnb_list_disp; /* Implantation de prodXnt2nbt */
static SXINT	*nt2order, *order2nt;
static SXINT	*rhs_nt2where, *lhs_nt2where;

/* Specifique PLR */
static SXINT 		lf_empty_string, q0, q1, q2, q3, qf;
static SXINT		*item2lf, *item2Q, *q2prod;
static XxY_header	lf_hd, Q_hd;
static SXBA		*Q2item_set, *Q2t_set, *kq2q_set, *nkq2q_set, *int2q_set, *innt2q_set, *outt2q_set;

static SXBA	*KQ2item_set, *NKQ2item_set, *outnt2q_set;
static SXBA	**KQT, **KQNT, **NKQT, **NKQNT;
static SXBA	*KQxT, *KQxNT, *NKQxT, *NKQxNT;
static SXBA	Q_is_shift;
static SXINT	*tkq, *ntkq, *tnkq, *ntnkq, *qXv, *delta;
static SXINT	xdelta, xqXv;

/* Par raison de compatibilite, on sort des parser_tables (presque) vides. */
#include "P_tables.h"
static struct parser_ag_item	parser_ag;

/* Retourne vrai si la lhs change */
static bool
OR (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
  /* La memoisation de OR par
     if (XxY_set (&include_sets_hd, rhs_bits_array, lhs_bits_array, &dum))
     return;
     sur ARN_186 a donne' :
     - nombre d'appels de OR : 		185206
     - nombre d'appels deja memoise : 	 99190
     - nombre d'inclusion non memoise :	  7508
     - AUGMENTATION du temps d'execution total de 0.76s a 3.27s

     Il est donc beaucoup plus couteux de memoiser que de calculer des OR inutiles. */

  SXBA	lhs_bits_ptr, rhs_bits_ptr;
  SXINT	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
  SXBA_ELT            lwork, work;
  bool             ret_val = false;

#if EBUG
  sxbassert (*lhs_bits_array >= *rhs_bits_array, "OR (|X|<|Y|)");
  sxbassert ((*(lhs_bits_array+SXNBLONGS (SXBASIZE (lhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (lhs_bits_array) - 1))) == 0, "OR");
  sxbassert ((*(rhs_bits_array+SXNBLONGS (SXBASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (rhs_bits_array) - 1))) == 0, "OR");
#endif
    
  lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

  while (slices_number-- > 0)
    {
      work = (*rhs_bits_ptr-- | (lwork = *lhs_bits_ptr));

      if (lwork /* ancienne valeur */ != work /* nouvelle valeur */)
	/* A change' */
	ret_val = true;

      *lhs_bits_ptr-- = work;
    }

  return ret_val;
}


static void
out_header (void)
{
  printf ("\
/* ********************************************************************************\n\
    These left-corner tables for the language \"%s\" has been generated\n\
    by the SYNTAX(*) csynt_lc processor\n\
   ********************************************************************************\n\
    (*) SYNTAX is a trademark of INRIA.\n\
   ******************************************************************************** */\n\n",
	  language_name);
}

#define out_define(str,val)	printf("#define %s\t%ld\n",str,val)
#define out_define_long(str,val)	printf("#define %s\t%ld\n",str,val)

static void
out_ifdef (char *name)
{
  printf ("\n\n#ifdef def_%s\n", name);
}

static void

out_endif (char *name)
{
  printf ("#endif /* def_%s */\n", name);
}


static void
output_elc_tables (void)
{


  SXINT tmax, ntmax, max_state, state, item, prod, X, nt, l, i;
  SXINT cur, top, x, bot2, top2, ndstate, next_state, next_state2;
  SXINT *ip, *t_intarea, *knt_intarea, *nknt_intarea, *state2item, *item2state;
  char	*str_ptr;
  SXBA item_set, next_state_set, *prod2la_set;
  SXBA *nt2next_state_set, nt_set, *t2next_state_set, t_set;

  SXINT           *eps_red_disp, *trans_disp, *teq2state, *state2tstate, *nteq2state, *state2ntstate;
  XH_header     nd_trans, eps_reds, teq_hd, nteq_hd;
  SXBA          next_tstate_set, next_ntstate_set;
  bool       has_t_trans = false, has_nt_trans = false, has_tequiv = false, has_ntequiv = false, has_eps_red = false;

  struct matrix t_shift, knt_shift, nknt_shift;
  struct vector t_vector, knt_vector, nknt_vector;

  sxinitialise(next_state); /* pour faire taire gcc -Wuninitialized */
  out_header ();

  puts ("\n");

  /* Le 24/06/04 pour verifier que si on utilise csynt_lc sur une "bnf" produite par lfg
     la date de bnf est posterieure a la date de creation des structures lfg */
  out_define_long ("bnf_modif_time", bnf_ag.bnf_modif_time);

  ntmax = bnf_ag.WS_TBL_SIZE.ntmax;
  out_define ("ntmax", ntmax);
  tmax = -bnf_ag.WS_TBL_SIZE.tmax;
  out_define ("tmax", tmax);
  out_define ("nbpro", bnf_ag.WS_TBL_SIZE.nbpro);
  out_define ("is_epsilon", (SXINT) is_epsilon);

  puts ("\n\nstatic SXINT lispro [] = {");

  for (prod = 0; prod < limpro; prod++) {
    item = prolon [prod];

    printf ("/* %ld [%ld..%ld] */\t", (SXINT) prod, (SXINT) item, (SXINT) prolon [prod+1]-1);

    while ((X = lispro [item++]) != 0) {
      printf ("%ld, ", X);
    }

    puts ("0,");
  }

  puts ("};");

  puts ("\n\nstatic SXINT prolis [] = {");

  for (prod = 0; prod < limpro; prod++) {
    item = prolon [prod];

    while ((X = lispro [item++]) != 0) {
      printf ("%ld, ", (SXINT) prod);
    }

    printf ("%ld,\n", (SXINT) prod);
  }

  puts ("};");


  puts ("\n\nstatic SXINT lhs [] = {");

  for (prod = 0; prod < limpro; prod++) {
    printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) lhs [prod]);
  }

  puts ("};");

  puts ("\n\nstatic SXINT semact [] = {");

  for (prod = 0; prod < limpro; prod++) {
    printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) semact [prod]);
  }

  puts ("};");

  puts ("\n\nstatic char *ntstring [] = {\"\",");

  for (nt = 1; nt < limnt; nt++) {
    /* On supprime les '<' et '>' des nt */
    str_ptr = get_nt_string (nt);
    l = strlen (str_ptr);
    str_ptr [l-1] = SXNUL;
    printf ("/* %ld */\t\"%s\",\n", (SXINT) nt, str_ptr+1);
    str_ptr [l-1] = '>';
  }

  puts ("};");

  state2item = (SXINT*) sxalloc (limitem, sizeof (SXINT));
  item2state = (SXINT*) sxalloc (limitem, sizeof (SXINT));

  item2state [0] = 0;
  item2state [1] = 1;
  item2state [2] = max_state = 2;
  item2state [3] = 0;

  state2item [0] = 0;
  state2item [1] = 1;
  state2item [2] = 2;


  for (prod = 1; prod < limpro; prod++) {
    item = prolon [prod];
    item2state [item] = 0;

    if (lispro [item] != 0) {
      do {
	item2state [++item] = ++max_state;
	state2item [max_state] = item;
      } while (lispro [item] != 0);
    }
  }

  puts ("\n");
  out_define ("max_state", max_state);

  puts ("\n\nstatic SXINT state2item [] = {0,");

  for (state = 1; state <= max_state; state++) {
    printf ("/* %ld */\t%ld,\n", (SXINT) state, (SXINT) state2item [state]);
  }

  puts ("};");

  /* On a : */
  /* left_corner = {(A, B) | A =>* \alpha B \beta =>* B \beta} */
  /* nt2item_set [A] = {B -> \alpha . \beta | B left_corner A et \alpha =>* \epsilon} */
  /* t2item_set [t] = {A -> \alpha . \beta | t \in First1(\beta Follow1(A))} */

  /* Pour chaque etat d'item kernel nt, on cherche une equivalence entre le sous-etat
     des transitions terminales ou non_terminales */
  next_tstate_set = sxba_calloc (limitem);
  XH_alloc (&teq_hd, "teq_hd", max_state, 1, 1+limt/4, NULL, statistics);
  teq2state = (SXINT*) sxalloc (max_state+1, sizeof (SXINT));
  state2tstate = (SXINT*) sxcalloc (max_state+1, sizeof (SXINT));

  next_ntstate_set = sxba_calloc (limitem);
  XH_alloc (&nteq_hd, "teq_hd", max_state, 1, 1+limt/4, NULL, statistics);
  nteq2state = (SXINT*) sxalloc (max_state+1, sizeof (SXINT));
  state2ntstate = (SXINT*) sxcalloc (max_state+1, sizeof (SXINT));

  for (state = 1; state <= max_state; state++) {
    item = state2item [state];
    X = lispro [item];

    if (X > 0) {
      /* Cas general, l'item kernel est de la forme A -> \alpha .X \beta avec ou` X est un nt */
      item_set = nt2item_set [X];

      item = -1;

      while ((item = sxba_scan (item_set, item)) >= 0) {
	X = lispro [item];

	if (X != 0)
	  next_state = item2state [item+1];
	      
	if (X < 0) {
	  SXBA_1_bit (next_tstate_set, next_state);
	  has_t_trans = true;
	}
	else {
	  if (X > 0) {
	    SXBA_1_bit (next_ntstate_set, next_state);
	    has_nt_trans = true;
	  }
	}
      }

      if (has_t_trans) {
	has_t_trans = false;

	next_state = -1;

	while ((next_state = sxba_scan_reset (next_tstate_set, next_state)) >= 0) {
	  XH_push (teq_hd, next_state);
	}

	if (XH_set (&teq_hd, &X)) {
	  /* existe deja */
	  state2tstate [state] = teq2state [X];
	  has_tequiv = true;
	}
	else {
	  /* Nouveau */
	  teq2state [X] = state;
	  state2tstate [state] = state; /* representant */
	}
      }

      if (has_nt_trans) {
	has_nt_trans = false;

	next_state = -1;

	while ((next_state = sxba_scan_reset (next_ntstate_set, next_state)) >= 0) {
	  XH_push (nteq_hd, next_state);
	}

	if (XH_set (&nteq_hd, &X)) {
	  /* existe deja */
	  state2ntstate [state] = nteq2state [X];
	  has_ntequiv = true;
	}
	else {
	  /* Nouveau */
	  nteq2state [X] = state;
	  state2ntstate [state] = state; /* representant */
	}
      }
    }
  }

  sxfree (next_tstate_set);
  XH_free (&teq_hd);
  sxfree (teq2state);

  sxfree (next_ntstate_set);
  XH_free (&nteq_hd);
  sxfree (nteq2state);

  /* Ici, pour un etat p ayant des transitions terminales non kernel si q = state2tstate [p] on a
     si p == q => c'est le representant
     si p != q => p et q sont equivalents (pour les  transitions terminales) et q est le representant */

  if (has_tequiv) {
    puts ("\n\nstatic SXINT state2tstate [] = {0,");

    for (state = 1; state <= max_state; state++) {
      printf ("/* %ld */\t%ld,\n", (SXINT) state, (SXINT) state2tstate [state]);
    }

    puts ("};");
  }
  else
    puts ("\n\nstatic SXINT *state2tstate;\n");

  if (has_ntequiv) {
    puts ("\n\nstatic SXINT state2ntstate [] = {0,");

    for (state = 1; state <= max_state; state++) {
      printf ("/* %ld */\t%ld,\n", (SXINT) state, (SXINT) state2ntstate [state]);
    }

    puts ("};");
  }
  else
    puts ("\n\nstatic SXINT *state2ntstate;\n");

  t_shift.last_line = max_state;
  t_shift.last_col = my_eof; /* le code de eof est eof */
  t_shift.matrix = (SXINT**) sxalloc (max_state+1, sizeof (SXINT*));
  ip = t_intarea = (SXINT*) sxalloc ((max_state+1)*(limt), sizeof (SXINT));

  for (i = 0; i <= max_state; i++) {
    t_shift.matrix [i] = ip;
    ip += limt;
  }

  t_shift.signature = sxbm_calloc (max_state+1, limt);

  knt_shift.last_line = max_state;
  knt_shift.last_col = ntmax;
  knt_shift.matrix = (SXINT**) sxalloc (max_state+1, sizeof (SXINT*));
  ip = knt_intarea = (SXINT*) sxalloc ((max_state+1)*(limnt), sizeof (SXINT));

  for (i = 0; i <= max_state; i++) {
    knt_shift.matrix [i] = ip;
    ip += limnt;
  }

  knt_shift.signature = sxbm_calloc (max_state+1, limnt);

  nknt_shift.last_line = max_state;
  nknt_shift.last_col = ntmax;
  nknt_shift.matrix = (SXINT**) sxalloc (max_state+1, sizeof (SXINT*));
  ip = nknt_intarea = (SXINT*) sxalloc ((max_state+1)*(limnt), sizeof (SXINT));

  for (i = 0; i <= max_state; i++) {
    nknt_shift.matrix [i] = ip;
    ip += limnt;
  }

  nknt_shift.signature = sxbm_calloc (max_state+1, limnt);

  nt2next_state_set = sxbm_calloc (limnt, limitem);
  t2next_state_set = sxbm_calloc (limt,  limitem);
  nt_set = sxba_calloc (limnt); 
  t_set = sxba_calloc (limt);

  eps_red_disp = (SXINT*) sxalloc (state+1, sizeof (SXINT));
  XH_alloc (&nd_trans, "nd_trans", limitem, 1, 2, NULL, statistics);

  if (is_epsilon) {
    XH_alloc (&eps_reds, "eps_reds", limpro, 1, 2, NULL, statistics);
  }

  for (state = 1; state <= max_state; state++) {
    item = state2item [state];
    X = lispro [item];

    if (X == 0)
      /* item reduce du kernel (unique) */
      /* On ne fait rien */
      {}
    else {
      if (X < 0) {
	/* item t-shift du kernel (unique) */
	/* On a state ->X state+1 */
	X = -X;
	SXBA_1_bit (t_shift.signature [state], X);
	t_shift.matrix [state] [X] = item2state [item+1];
      }
      else {
	/* Cas general, l'item kernel est de la forme A -> \alpha .X \beta avec ou` X est un nt */
	item_set = nt2item_set [X];
	/* On partitionne ces items par les symboles qu'ils reperent */

	/* Les transitions nt du kernel sont deterministes */
	next_state = item2state [item+1];
	SXBA_1_bit (knt_shift.signature [state], X);
	knt_shift.matrix [state] [X] = next_state;

	has_tequiv = (state2tstate [state] == state);
	has_ntequiv = (state2ntstate [state] == state);

	item = -1;

	while ((item = sxba_scan (item_set, item)) >= 0) {
	  X = lispro [item];

	  if (X != 0)
	    next_state = item2state [item+1];
	      
	  if (X < 0) {
	    if (has_tequiv) {
	      X = -X;
	      SXBA_1_bit (t2next_state_set [X], next_state);
	      SXBA_1_bit (t_set, X);
	    }
	  }
	  else {
	    if (X == 0) {
	      XH_push (eps_reds, prolis [item]);
	      has_eps_red = true;
	    }
	    else {
	      if (has_ntequiv) {
		SXBA_1_bit (nt2next_state_set [X], next_state);
		SXBA_1_bit (nt_set, X);
	      }
	    }
	  }
	}

	if (has_eps_red) {
	  has_eps_red = false;
	  XH_set (&eps_reds, eps_red_disp+state);
	}
	else
	  eps_red_disp [state] = 0;

	X = -1;

	while ((X = sxba_scan_reset (t_set, X)) >= 0) {
	  SXBA_1_bit (t_shift.signature [state], X);

	  next_state = sxba_scan_reset (next_state_set = t2next_state_set [X], -1);

	  if ((next_state2 = sxba_scan_reset (next_state_set, next_state)) > 0) {
	    /* La transition sur X est non-deterministe */
	    XH_push (nd_trans, next_state);
	    XH_push (nd_trans, next_state2);

	    while ((next_state2 = sxba_scan_reset (next_state_set, next_state2)) > 0) {
	      XH_push (nd_trans, next_state2);
	    }

	    XH_set (&nd_trans, &ndstate);
	    t_shift.matrix [state] [X] = ndstate + max_state;/* codage des trans non deterministes */
	  }
	  else {
	    /* La transition sur X est deterministe */
	    t_shift.matrix [state] [X] = next_state;
	  }
	}

	X = 0;

	while ((X = sxba_scan_reset (nt_set, X)) > 0) {
	  SXBA_1_bit (nknt_shift.signature [state], X);

	  next_state = sxba_scan_reset (next_state_set = nt2next_state_set [X], -1);

	  if ((next_state2 = sxba_scan_reset (next_state_set, next_state)) > 0) {
	    /* La transition sur X est non-deterministe */
	    XH_push (nd_trans, next_state);
	    XH_push (nd_trans, next_state2);

	    while ((next_state2 = sxba_scan_reset (next_state_set, next_state2)) > 0) {
	      XH_push (nd_trans, next_state2);
	    }

	    XH_set (&nd_trans, &ndstate);
	    nknt_shift.matrix [state] [X] = ndstate + max_state;/* codage des trans non deterministes */
	  }
	  else {
	    /* La transition sur X est deterministe */
	    nknt_shift.matrix [state] [X] = next_state;
	  }
	}
      }
    }
  }

  sxfree (state2tstate);
  sxfree (state2ntstate);

  matrix2vector (&t_shift, &t_vector, 25);
  vector2c (&t_vector, stdout, "t_shift", true /* static */);

  sxfree (t_shift.matrix);
  sxfree (t_intarea);
  sxfree (t_shift.signature);
  vector_free (&t_vector);

  matrix2vector (&knt_shift, &knt_vector, 25);
  vector2c (&knt_vector, stdout, "knt_shift", true /* static */);

  sxfree (knt_shift.matrix);
  sxfree (knt_intarea);
  sxfree (knt_shift.signature);;
  vector_free (&knt_vector);

  matrix2vector (&nknt_shift, &nknt_vector, 25);
  vector2c (&nknt_vector, stdout, "nknt_shift", true /* static */);

  sxfree (nknt_shift.matrix);
  sxfree (nknt_intarea);
  sxfree (nknt_shift.signature);;
  vector_free (&nknt_vector);

  puts ("\n\nstatic SXINT trans_list [] = {0,");

  cur = 1;
  top = XH_top (nd_trans);

  trans_disp = (SXINT*) sxalloc (top, sizeof (SXINT));

  for (x = 1; x < top; x++) {
    printf ("/* %ld */ ", (SXINT) x);
    trans_disp [x] = cur;

    bot2 = XH_X (nd_trans, x);
    top2 = XH_X (nd_trans, x+1);

    do {
      printf ("%ld, ", (SXINT) XH_list_elem (nd_trans, bot2));
      cur++;
    } while (++bot2 < top2);

    puts ("");
  }

  puts ("};\n");

  puts ("static SXINT trans_disp [] = {0,");

  for (x = 1; x < top; x++) {
    printf ("/* %ld */ %ld,\n", (SXINT) x, (SXINT) trans_disp [x]);
  }

  printf ("%ld};\n", (SXINT) cur);

  sxfree (trans_disp);

  if (is_epsilon) {
    puts ("\nstatic SXINT eps_red_list [] = {0,");

    cur = 1;

    for (state = 1; state <= max_state; state++) {
      x = eps_red_disp [state];
      eps_red_disp [state] = cur;

      if (x) {
	printf ("/* %ld */ ", (SXINT) state);

	bot2 = XH_X (eps_reds, x);
	top2 = XH_X (eps_reds, x+1);

	do {
	  printf ("%ld, ", (SXINT) XH_list_elem (eps_reds, bot2));
	  cur++;
	} while (++bot2 < top2);

	puts ("");
      }
    }

    puts ("};\n");

    puts ("static SXINT eps_red_disp [] = {0,");

    for (state = 1; state <= max_state; state++) {
      printf ("/* %ld */ %ld,\n", (SXINT) state, (SXINT) eps_red_disp [state]);
    }

    puts ("};\n");
  }


  sxfree (state2item);
  sxfree (item2state);

  sxbm_free (nt2next_state_set);
  sxbm_free (t2next_state_set);
  sxfree (nt_set);
  sxfree (t_set);

  sxfree (eps_red_disp);

  XH_free (&nd_trans);

  if (is_epsilon)
    XH_free (&eps_reds);

  prod2la_set = sxbm_calloc (limpro, limt);

  for (prod = 1; prod < limpro; prod++)
    sxba_copy (prod2la_set [prod], item2t_set [prolon [prod+1]-1]);

  puts ("\n#if is_1LA");
  sxbm3_to_c (stdout, prod2la_set, limpro, "prod2la_set", "", true /* is_static */);
  puts ("#endif /* is_1LA */");

  sxbm_free (prod2la_set);
}

static SXBA *nt2gen_t_set;

/* nt2gen_t_set [A] = {t | A =>+ x1 t x2} */
static void
fill_nt2gen_t_set (void)
{
  SXINT  prod, item, A, B, X, bot, top;
  SXINT  *nt_stack;
  SXBA nt_set, gen_t_set;

  nt_set = sxba_calloc (limnt);
  nt_stack = (SXINT*) sxalloc (limnt, sizeof (SXINT)), nt_stack [0] = 0;

  for (prod = 1; prod < limpro; prod++) {
    item = prolon [prod];
    A = lhs [prod];
    gen_t_set = nt2gen_t_set [A];

    while ((X = lispro [item++]) != 0) {
      if (X < 0) {
	X = -X;
	SXBA_1_bit (gen_t_set, X);

	if (SXBA_bit_is_reset_set (nt_set, A))
	  PUSH (nt_stack, A);
      }
    }
  }

  while (!IS_EMPTY (nt_stack)) {
    A = POP (nt_stack);
    SXBA_0_bit (nt_set, A);

    if (A > 0) {
      bot = bnf_ag.WS_NTMAX [A].npd;
      top = bnf_ag.WS_NTMAX [A+1].npd;
      gen_t_set = nt2gen_t_set [A];

      while (bot < top) {
	item = bnf_ag.NUMPD [bot];
	B = lhs [prolis [item]]; /* On est su^r que prod n'est pas la super-re`gle */

	if (sxba_or_with_test (nt2gen_t_set [B], gen_t_set)) {
	  /* B a change' ... */
	  if (SXBA_bit_is_reset_set (nt_set, B)) {
	    /* ... et n'est pas/plus ds la pile */
	    PUSH (nt_stack, B);
	  }
	}

	bot++;
      }
    }
  }

  sxfree (nt_set);
  sxfree (nt_stack);
}


/* pour CRS : calcul et sortie des segments */
static SXINT *segment_hd2nb;

static void
segment_hd_oflw (SXINT old_size, SXINT new_size)
{
  sxuse(old_size); /* pour faire taire gcc -Wunused */
  segment_hd2nb = (SXINT*) sxrealloc (segment_hd2nb, new_size+1, sizeof (SXINT));
}

void output_segments (void)
{
  SXINT       prod, bnf_huge_item_2, head_item, bnf_huge_X, seg_id, top, bot, segment_hd_top, nb, i;
  SXINT       *ptr;
  SXINT       *segment, *segment_list, *segment_hd2id;
  XH_header segment_hd;

  XH_alloc (&segment_hd, "segment_hd", bnf_ag.WS_TBL_SIZE.indpro-2*bnf_ag.WS_TBL_SIZE.nbpro+1, 1, 5, segment_hd_oflw, NULL);
  segment_hd2nb = (SXINT*) sxalloc (XH_size (segment_hd)+1, sizeof (SXINT));
  segment_hd2nb [0] = 0;

  segment = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.indpro+1, sizeof (SXINT));

  /* On isole les segments */
  /* traitement manuel de la super-regle S' -> $ S $ */
  if (SXBA_bit_is_set (bnf_ag.BVIDE, 1)) {
    /* eps est ds L */
    XH_push (segment_hd, bnf_ag.WS_TBL_SIZE.tmax);
    XH_push (segment_hd, 1);
    XH_push (segment_hd, bnf_ag.WS_TBL_SIZE.tmax);
    XH_set (&segment_hd, &seg_id);
    segment_hd2nb [seg_id] = 2; /* le 0 de fin */
    segment_hd2nb [0] += 2; /* le 0 de fin */
    segment [0] = seg_id;
    segment [2] = -1; /* peut etre ecrase' + tard */
      
  }
  else {
    XH_push (segment_hd, bnf_ag.WS_TBL_SIZE.tmax);
    XH_push (segment_hd, 1);
    XH_set (&segment_hd, &seg_id);
    segment_hd2nb [seg_id] = 2; /* le 0 de fin */
    segment_hd2nb [0] += 2; /* le 0 de fin */
    segment [0] = seg_id;

    XH_push (segment_hd, 1);
    XH_push (segment_hd, bnf_ag.WS_TBL_SIZE.tmax);
    XH_set (&segment_hd, &seg_id);
    segment_hd2nb [seg_id] = 2; /* le 0 de fin */
    segment_hd2nb [0] += 2; /* le 0 de fin */
    segment [1] = seg_id;
    segment [2] = -1; /* peut etre ecrase' + tard */
  }

  for (prod = 1; prod <= bnf_ag.WS_TBL_SIZE.nbpro; prod++) {
    bnf_huge_item_2 = bnf_ag.WS_NBPRO [prod].prolon;
    head_item = 0;

    while ((bnf_huge_X = bnf_ag.WS_INDPRO [bnf_huge_item_2].lispro) != 0) {
      if (bnf_huge_X < 0 || !SXBA_bit_is_set (bnf_ag.BVIDE, bnf_huge_X)) {
	if (head_item) {
	  /* On vient d'isoler un segment */
	  /* On calcule son identifiant */
	  for (i = head_item; i <= bnf_huge_item_2; i++) {
	    XH_push (segment_hd, bnf_ag.WS_INDPRO [i].lispro);
	  }

	  if (XH_set (&segment_hd, &seg_id)) {
	    segment_hd2nb [seg_id]++;
	    segment_hd2nb [0]++;
	  }
	  else {
	    segment_hd2nb [seg_id] = 2; /* le 0 de fin */
	    segment_hd2nb [0] += 2; /* le 0 de fin */
	  }

	  segment [head_item] = seg_id;
	  segment [bnf_huge_item_2] = -1; /* peut etre ecrase' + tard */
	}
	  
	head_item = bnf_huge_item_2;
      }

      bnf_huge_item_2++;
    }
  }

  segment_list = (SXINT*) sxalloc (segment_hd2nb [0]+1, sizeof (SXINT));
  segment_list [0] = 0;

  segment_hd_top = XH_top (segment_hd);

  segment_hd2id = (SXINT*) sxalloc (segment_hd_top+1, sizeof (SXINT));
  segment_hd2id [0] = 0;
	      
  top = 1;

  for (seg_id = 1; seg_id < segment_hd_top; seg_id++) {
    segment_hd2id [seg_id] = top;
    segment_list [top] = 0; /* nb d'elem ds la pile */
    top += segment_hd2nb [seg_id];
  }
    
  for (prod = 0; prod <= bnf_ag.WS_TBL_SIZE.nbpro; prod++) {
    if (prod == 0)
      bnf_huge_item_2 = 0;
    else
      bnf_huge_item_2 = bnf_ag.WS_NBPRO [prod].prolon;

    head_item = 0;

    while (bnf_huge_item_2 == 0 || bnf_ag.WS_INDPRO [bnf_huge_item_2].lispro != 0) {
      seg_id = segment [bnf_huge_item_2];

      if (seg_id > 0) {
	bot = segment [bnf_huge_item_2] = segment_hd2id [seg_id]; /* indice vers le bas de la pile */
	segment_list [bot + ++segment_list [bot]] = bnf_huge_item_2; /* On push bnf_huge_item_2 ds cette "pile" */
      }

      bnf_huge_item_2++;
    }
  }

  out_ifdef ("segment");
  printf ("static SXINT segment [%ld] = {\n", (SXINT) bnf_ag.WS_TBL_SIZE.indpro+1);
  for (prod = 0; prod <= bnf_ag.WS_TBL_SIZE.nbpro; prod++) {
    printf ("/* prod=%ld */", (SXINT) prod);

    if (prod == 0)
      bnf_huge_item_2 = 0;
    else
      bnf_huge_item_2 = bnf_ag.WS_NBPRO [prod].prolon;

    while (bnf_huge_item_2 == 0 || bnf_ag.WS_INDPRO [bnf_huge_item_2].lispro != 0) {
      printf (" %ld,", (SXINT) segment [bnf_huge_item_2]);
      bnf_huge_item_2++;
    }

    fputs (" 0,\n", stdout);
  }
  fputs ("};\n", stdout);

    
  printf ("static SXINT segment_list [%ld] = {0,\n", (SXINT) segment_hd2nb [0]+1);

  for (seg_id = 1; seg_id < segment_hd_top; seg_id++) {
    bot = segment_hd2id [seg_id];
    ptr = segment_list+bot;
    nb = *ptr;
    printf ("/* seg_id=%ld */ %ld,", (SXINT) seg_id, (SXINT) nb);

    while (nb-- > 0) {
      printf (" %ld,", (SXINT) *++ptr);
    }

    fputs ("\n", stdout);
  }
  fputs ("};\n", stdout);

  out_endif ("segment");

  XH_free (&segment_hd);
  sxfree (segment_hd2nb), segment_hd2nb = NULL;
  sxfree (segment);
  sxfree (segment_list);
  sxfree (segment_hd2id);
}






static void
output_tables (void)
{
    SXINT		prod, item, X, x, y, n, *p, nt, t, q, l;
    bool	is_prdct;
    VARSTR	output_tables_vstr;
    char	*str_ptr;

    /* Ds le cas RULE_TO_REDUCE, on verifie que c'est bien xbnf qui a ete utilise */
    if (bnf_ag.RULE_TO_REDUCE != NULL && bnf_ag.RULE_TO_REDUCE [0] != 0) {
      fputs ("\n\tYou must use the xbnf constructor, see you later...\n", sxtty);
      sxexit (3);
    }

    output_tables_vstr = varstr_alloc (limitem);

    out_header ();

#if 0
    puts ("\n#include \"sxunix.h\"\n");
#endif /* 0 */

    /* Le 24/06/04 pour verifier que si on utilise csynt_lc sur une "bnf" produite par lfg
       la date de bnf est posterieure a la date de creation des structures lfg */
    out_define_long ("bnf_modif_time", bnf_ag.bnf_modif_time);

    out_define ("ntmax", bnf_ag.WS_TBL_SIZE.ntmax);
    printf ("#define tmax\t(%ld)\n", (SXINT) bnf_ag.WS_TBL_SIZE.tmax);
    out_define ("itemmax", bnf_ag.WS_TBL_SIZE.indpro);
    out_define ("prodmax", bnf_ag.WS_TBL_SIZE.xnbpro);
    out_define ("initial_state", (SXINT) 0);
    out_define ("rhs_lgth", rhs_lgth);
    out_define ("rhs_maxnt", rhs_maxnt);
    out_define ("is_right_recursive", (SXINT) (rrnt > 0));
    out_define ("is_cyclic", (SXINT) (ccnt > 0));
    out_define ("is_epsilon", (SXINT) is_epsilon);
    out_define ("is_constraints", (SXINT) (bnf_ag.RULE_TO_REDUCE != NULL));

    is_prdct = (bnf_ag.WS_TBL_SIZE.xtmax > my_eof) || (bnf_ag.WS_TBL_SIZE.xntmax > bnf_ag.WS_TBL_SIZE.ntmax);
    out_define ("is_prdct", (SXINT) is_prdct);

    out_ifdef ("lispro");
    puts ("static SXINT lispro [] = {");

    for (prod = 0; prod < limpro; prod++) {
	item = prolon [prod];

	printf ("/* %ld [%ld..%ld] */\t", (SXINT) prod, (SXINT) item, (SXINT) prolon [prod+1]-1);

	while ((X = lispro [item++]) != 0) {
	    printf ("%ld, ", (SXINT) X);
	}

	puts ("0,");
    }

    puts ("};");
    out_endif ("lispro");

    out_ifdef ("prolis");
    puts ("static SXINT prolis [] = {");

    for (prod = 0; prod < limpro; prod++) {
	item = prolon [prod];

	while ((X = lispro [item++]) != 0) {
	    printf ("%ld, ", (SXINT) prod);
	}

	printf ("%ld,\n", (SXINT) prod);
    }

    puts ("};");
    out_endif ("prolis");


    out_ifdef ("prolon");
    puts ("static SXINT prolon [] = {");

    for (prod = 0; prod <= limpro; prod++) {
	printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) prolon [prod]);
    }

    puts ("};");
    out_endif ("prolon");


    out_ifdef ("lhs");
    puts ("static SXINT lhs [] = {");

    for (prod = 0; prod < limpro; prod++) {
	printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) lhs [prod]);
    }

    puts ("};");
    out_endif ("lhs");

    out_ifdef ("semact");
    puts ("static SXINT semact [] = {");

    for (prod = 0; prod < limpro; prod++) {
	printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) semact [prod]);
    }

    puts ("};");
    out_endif ("semact");


    nt2gen_t_set = sxbm_calloc (limnt, limt);
    fill_nt2gen_t_set ();
    out_ifdef ("nt2gen_t_set");
    sxbm3_to_c (stdout, nt2gen_t_set, limnt, "nt2gen_t_set", "", true /* is_static */);
    out_endif ("nt2gen_t_set");
    sxfree (nt2gen_t_set), nt2gen_t_set = NULL;


    out_ifdef ("nt2order");
    puts ("static SXINT nt2order [] = {0,");

    for (nt = 1; nt < limnt; nt++) {
	printf ("/* %ld = %s */\t%ld,\n", (SXINT) nt, get_nt_string (nt), (SXINT) nt2order [nt]);
    }

    puts ("};");
    out_endif ("nt2order");


    out_ifdef ("order2nt");
    puts ("static SXINT order2nt [] = {0,");

    for (nt = 1; nt < limnt; nt++) {
	printf ("/* %ld */\t%ld /* %s */,\n", (SXINT) nt, (SXINT) order2nt [nt], get_nt_string (nt));
    }

    puts ("};");
    out_endif ("order2nt");


    out_ifdef ("rhs_nt2where");
    puts ("static SXINT rhs_nt2where [] = {");

    printf ("/* %ld */\t%ld /* %s */,\n", (SXINT) 0, (SXINT) rhs_nt2where [0], "terminals");

    for (nt = 1; nt < limnt; nt++) {
	printf ("/* %ld */\t%ld /* %s */,\n", (SXINT) nt, (SXINT) rhs_nt2where [nt], get_nt_string (nt));
    }

    puts ("};");
    out_endif ("rhs_nt2where");


    out_ifdef ("lhs_nt2where");
    puts ("static SXINT lhs_nt2where [] = {");

    printf ("/* %ld */\t%ld /* %s */,\n", (SXINT) 0, (SXINT) lhs_nt2where [0], "<Super Start Symbol>");

    for (nt = 1; nt < limnt; nt++) {
	printf ("/* %ld */\t%ld /* %s */,\n", (SXINT) nt, (SXINT) lhs_nt2where [nt], get_nt_string (nt));
    }

    puts ("};");
    out_endif ("lhs_nt2where");


    out_ifdef ("t2item_set");
    sxbm3_to_c (stdout, t2item_set, limt, "t2item_set", "", true /* is_static */);
    out_endif ("t2item_set");

    out_ifdef ("left_corner");
    sxbm3_to_c (stdout, left_corner, limnt, "left_corner", "", true /* is_static */);
    out_endif ("left_corner");

    out_ifdef ("nt2item_set");
    sxbm3_to_c (stdout, nt2item_set, limnt, "nt2item_set", "", true /* is_static */);
    out_endif ("nt2item_set");

    if (bnf_ag.RULE_TO_REDUCE != NULL) {
      out_ifdef ("constraints");
      puts ("\n\nstatic SXINT constraints [] = {\n/* 0 */ -1,");

      for (prod = 1; prod < limpro; prod++) {
	printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) bnf_ag.RULE_TO_REDUCE [prod]);
      }

      puts ("};");
      out_endif ("constraints");
    }

    if (is_epsilon) {
      out_ifdef ("BVIDE");
      sxba2c (bvide, stdout, "BVIDE", "", true /* is_static */, output_tables_vstr);
      out_endif ("BVIDE");
    }

#if 0
    sxba2c (bnf_ag.NULLABLE, stdout, "NULLABLE", "", true /* is_static */, output_tables_vstr);
#endif

    if (is_prdct) {
      out_ifdef ("prdct");
      /* S'il y a des predicats, on permet leur execution, meme si on n'est pas ds le cas is_parser */
	puts ("static SXINT prdct [] = {");

	for (prod = 0; prod < limpro; prod++) {
	    printf ("/* %ld */\t", (SXINT) prod);

	    item = prolon [prod];

	    while (lispro [item] != 0) {
		printf ("%ld, ", (SXINT) prdct [item]);
		item++;
	    }

	    puts ("-1,");
	}

	puts ("};");
      out_endif ("prdct");
    }

#if 0
    puts ("\n#if is_parser");
#endif /* 0 */

    out_ifdef ("item2nbnt");
    puts ("static SXINT item2nbnt [] = {");

    for (prod = 0; prod < limpro; prod++) {
	printf ("/* %ld */\t", (SXINT) prod);

	item = prolon [prod];

	while ((X = lispro [item]) != 0) {
	    printf ("%ld, ", (SXINT) item2nbnt [item]);
	    item++;
	}

	printf ("%ld,\n", (SXINT) item2nbnt [item]);
    }

    puts ("};");
    out_endif ("item2nbnt");


    out_ifdef ("item2nbt");
    puts ("static SXINT item2nbt [] = {");

    for (prod = 0; prod < limpro; prod++) {
	printf ("/* %ld */\t", (SXINT) prod);

	item = prolon [prod];

	while ((X = lispro [item]) != 0) {
	    printf ("%ld, ", (SXINT) item2nbt [item]);
	    item++;
	}

	printf ("%ld,\n", (SXINT) item2nbt [item]);
    }

    puts ("};");
    out_endif ("item2nbt");


    out_ifdef ("prod2nbnt");
    puts ("static SXINT prod2nbnt [] = {");

    for (prod = 0; prod < limpro; prod++) {
	printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) prod2nbnt [prod]);
    }

    puts ("};");
    out_endif ("prod2nbnt");


    out_ifdef ("prodXnt2nbt");
    puts ("static SXINT prodXnt2nbt [prodmax+1] [rhs_maxnt+1] = {");

    for (prod = 0; prod < limpro; prod++) {
	printf ("/* %ld */\t{", (SXINT) prod);
	p = tnb_list_disp [prod];

	for (n = 0; n < rhs_maxnt; n++){
	    printf ("%ld, ", (SXINT) p [n]);
	}

	printf ("%ld},\n", (SXINT) p [n]);
    }

    puts ("};");
    out_endif ("prodXnt2nbt");


    out_ifdef ("prod2order");
    puts ("static SXINT prod2order [] = {");

    for (prod = 0; prod < limpro; prod++) {
	printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) prod2order [prod]);
    }

    puts ("};");
    out_endif ("prod2order");


    out_ifdef ("order2prod");
    puts ("static SXINT order2prod [] = {");

    for (prod = 0; prod < limpro; prod++) {
	printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) order2prod [prod]);
    }

    puts ("};");
    out_endif ("order2prod");


    if (rrnt > 0) {
      out_ifdef ("prod_order2min");
      puts ("static SXINT prod_order2min [] = {");

      for (prod = 0; prod < limpro; prod++) {
	printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) prod_order2min [prod]);
      }

      puts ("};");
      out_endif ("prod_order2min");


      out_ifdef ("prod_order2max");
      puts ("static SXINT prod_order2max [] = {");

      for (prod = 0; prod < limpro; prod++) {
	printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) prod_order2max [prod]);
      }

      puts ("};");
      out_endif ("prod_order2max");
#if 0
      sxba2c (is_mutual_right_recursive_prod, stdout, "is_right_recursive_prod", "",
	      true		/* is_static */, output_tables_vstr);
#endif

      if (ccnt > 0) {
#if 0
	sxbm2c (stdout, cycle_nb2item_set, ccnt+1, "cycle_nb2item_set", "", true /* is_static */, output_tables_vstr);
#endif
	out_ifdef ("item2cycle_nb");
	puts ("static SXINT item2cycle_nb [] = {");

	for (prod = 0; prod < limpro; prod++) {
	  printf ("/* %ld */\t", (SXINT) prod);

	  item = prolon [prod];

	  while ((X = lispro [item]) != 0) {
	    printf ("%ld, ", (SXINT) item2cycle_nb [item]);
	    item++;
	  }

	  printf ("%ld,\n", (SXINT) item2cycle_nb [item]);
	}

	puts ("};");
	out_endif ("item2cycle_nb");
      }
    }

#if 0
    puts ("\n#endif /* is_parser */");
#endif /* 0 */


    out_ifdef ("tstring");
    puts ("static char *tstring [] = {\"\",");

    for (t = 1; t < limt; t++) {
      str_ptr = get_t_string (-t);
      printf ("/* %ld */\t\"", (SXINT) t);
      while (*str_ptr) {
	if (*str_ptr == '"' || *str_ptr == '\\')
	  printf ("\\");
	printf ("%c", *str_ptr);
	str_ptr++;
      }
      printf ("\",\n");
    }

    puts ("};");
    out_endif ("tstring");


    out_ifdef ("ntstring");
    puts ("static char *ntstring [] = {\"\",");

    for (nt = 1; nt < limnt; nt++) {
	/* On supprime les '<' et '>' des nt */
	str_ptr = get_nt_string (nt);
	l = strlen (str_ptr);
	str_ptr [l-1] = SXNUL;
	printf ("/* %ld */\t\"%s\",\n", (SXINT) nt, str_ptr+1);
	str_ptr [l-1] = '>';
    }

    puts ("};");
    out_endif ("ntstring");

    {
      /* Traitement de la lexicalisation */
      SXINT i, j, output_tables_item, output_tables_X, Y, t_nb;
      SXINT *prod2t_nb;
      SXBA *t2prod_item_set, lex_compatible_item_set, prod_item_set, multiple_t_item_set;
      bool multiple_t = false;

#if 0
      puts ("\n#if is_lex || is_set_automaton");
#endif /* 0 */

      prod2t_nb = (SXINT *) sxalloc (limpro, sizeof (SXINT));
      t2prod_item_set = sxbm_calloc (limt, limitem);
      lex_compatible_item_set = t2prod_item_set [0];
      multiple_t_item_set = sxba_calloc (limitem);

      for (prod = 0; prod < limpro; prod++) {
	i = output_tables_item = prolon [prod];
	j = 0;
	Y = 0;
	t_nb = 0;

	do {
	  output_tables_X = lispro [i];

	  if (output_tables_X < 0) {
	    prod_item_set = t2prod_item_set [-output_tables_X];
	    j = output_tables_item;

	    do {
	      SXBA_1_bit (prod_item_set, j);
	    } while (lispro [j++] != 0);

	    if (Y == 0) {
	      Y = output_tables_X;
	      t_nb = -i;
	    }
	    else {
	      if (Y != output_tables_X)
		multiple_t = true;

	      if (t_nb < 0)
		t_nb = 2;
	      else
		t_nb++;
	    }
	  }

	  i++;
	} while (output_tables_X != 0);

	prod2t_nb [prod] = t_nb;

	if (j == 0) {
	  j = output_tables_item;

	  do {
	    SXBA_1_bit (lex_compatible_item_set, j);
	  } while (lispro [j++] != 0);
	}

	if (multiple_t) {
	  multiple_t = false;
	  j = output_tables_item;

	  do {
	    if ((output_tables_X = lispro [j]) < 0)
	      SXBA_1_bit (multiple_t_item_set, j);

	    j++;
	  } while (output_tables_X != 0);
	}
      }

      out_ifdef ("t2prod_item_set");
      /* Inutile d'appeler sxbm3_to_c */
      sxbm2c (stdout, t2prod_item_set, limt, "t2prod_item_set", "", true /* is_static */, output_tables_vstr);
      out_endif ("t2prod_item_set");

#if 0
      /* Enleve le 15/10/2002 */
      puts ("\nstatic SXBA lex_compatible_item_set = t2prod_item_set [0];");
#endif /* 0 */

#if 0
      puts ("\n#endif /* is_lex || is_set_automaton */");
#endif /* 0 */

#if 0
      /* Enleve le 8/10/2002 */
      puts ("\n#if is_lex2 || is_supertagging");
#endif /* 0 */

      out_ifdef ("multiple_t_item_set");

      if (sxba_is_empty (multiple_t_item_set))
	puts ("static SXBA multiple_t_item_set;");
      else
	sxba2c (multiple_t_item_set, stdout, "multiple_t_item_set", "", true /* is_static */, output_tables_vstr);

      out_endif ("multiple_t_item_set");

#if 0
      puts ("\n#endif /* is_lex2 || is_supertagging */");
#endif /* 0 */

      sxfree (multiple_t_item_set);
      sxbm_free (t2prod_item_set);

      {
	SXBA titem_set, rc_titem_set;

	titem_set = sxba_calloc (limitem);
	rc_titem_set = sxba_calloc (limitem);

	for (output_tables_item = 0; output_tables_item < limitem; output_tables_item++) {
	  if (lispro [output_tables_item] < 0) {
	    SXBA_1_bit (titem_set, output_tables_item);
	    SXBA_1_bit (rc_titem_set, output_tables_item+1);
	  }
	}

	
	out_ifdef ("titem_set");
	sxba2c (titem_set, stdout, "titem_set", "", true /* is_static */, output_tables_vstr);
	out_endif ("titem_set");


#if 0
	puts ("\n#if is_set_automaton || is_rcvr");
	sxba2c (titem_set, stdout, "titem_set", "", true /* is_static */, output_tables_vstr);
	puts ("\n#endif /* is_set_automaton || is_rcvr */\n");
#endif /* 0 */


	{
	  /* nt2t_set [A] = {t | A =>+ x t y} */
	  SXINT  output_tables_item_2, output_tables_prod, A, B, output_tables_X_2, bot, top, cur_min, cur_nt_min, cur_prod_min, bot2, top2;
	  SXBA *nt2t_set, nt_set, prod_set;
	  SXINT  *nt_stack, *nt2cur_min, *prod2cur_min;

	  nt2t_set = sxbm_calloc (limnt, limt);
	  nt_stack = (SXINT *) sxalloc (limnt, sizeof (SXINT)), nt_stack [0] = 0;
	  nt_set = sxba_calloc (limnt);

	  output_tables_item_2 = 3;

	  while ((output_tables_item_2 = sxba_scan (titem_set, output_tables_item_2)) > 0) {
	    output_tables_X_2 = -lispro [output_tables_item_2];
	    output_tables_prod = prolis [output_tables_item_2];
	    A = lhs [output_tables_prod];

	    SXBA_1_bit (nt2t_set [A], output_tables_X_2);
	  }

	  for (A = 1; A < limnt; A++) {
	    PUSH (nt_stack, A);
	    SXBA_1_bit (nt_set, A);
	  }

	  while (!IS_EMPTY (nt_stack)) {
	    B = POP (nt_stack);
	    SXBA_0_bit (nt_set, B);

	    bot = bnf_ag.WS_NTMAX [B].npd;
	    top = bnf_ag.WS_NTMAX [B+1].npd;

	    while (bot < top) {
	      output_tables_item_2 = bnf_ag.NUMPD [bot];
	      output_tables_prod = prolis [output_tables_item_2];
	      A = lhs [output_tables_prod];

	      if (OR (nt2t_set [A], nt2t_set [B])) {
		if (SXBA_bit_is_reset_set (nt_set, A))
		  PUSH (nt_stack, A);
	      }
      
	      bot++;
	    }
	  }

	  SXBA_1_bit (nt2t_set [0], my_eof);

#if 0
	  puts ("\n#if is_rcvr");
#endif /* 0 */

	  out_ifdef ("nt2t_set");
	  sxbm2c (stdout, nt2t_set, limnt, "nt2t_set", "", true /* is_static */, output_tables_vstr);
	  out_endif ("nt2t_set");

	  sxbm_free (nt2t_set);
	  sxfree (nt_stack);

	  nt2cur_min = (SXINT *) sxcalloc (limnt, sizeof (SXINT));
	  prod2cur_min = (SXINT *) sxcalloc (limpro, sizeof (SXINT));
	  prod_set = sxba_calloc (limpro);


	  for (A = 1; A < limnt; A++) {

	    if (!SXBA_bit_is_set (bvide, A)) {
	      /* On parcourt les nt_prods */
	      bot = bnf_ag.WS_NTMAX [A].npg;
	      top = bnf_ag.WS_NTMAX [A+1].npg;
	      cur_nt_min = -1;

	      while (bot < top) {
		output_tables_prod = bnf_ag.WS_NBPRO [bot].numpg;
		cur_prod_min = 0;
		bot2 = prolon [output_tables_prod];
		top2 = prolon [output_tables_prod+1]-1;

		for (output_tables_item_2 = bot2; output_tables_item_2 < top2; output_tables_item_2++) {
		  output_tables_X_2 = lispro [output_tables_item_2];

		  if (output_tables_X_2 < 0 || !SXBA_bit_is_set (bvide, output_tables_X_2))
		    cur_prod_min++;
		}

		prod2cur_min [output_tables_prod] = cur_prod_min;

		if (cur_nt_min == -1 || cur_prod_min < cur_nt_min)
		  cur_nt_min = cur_prod_min;

		bot++;
	      }
	      
	      if (cur_nt_min > 0) {
		SXBA_1_bit (nt_set, A); /* taille a change' */
		nt2cur_min [A] = cur_nt_min; /* au moins */
	      }
	    }
	  }

	  /* nt_set contient les nt dont la taille min a change */
	  while ((B = sxba_scan_reset (nt_set, 0)) > 0) {
	    do {
	      bot = bnf_ag.WS_NTMAX [B].npd;
	      top = bnf_ag.WS_NTMAX [B+1].npd;

	      while (bot < top) {
		output_tables_item_2 = bnf_ag.NUMPD [bot];
		output_tables_prod = prolis [output_tables_item_2];
		/* changement qui impacte output_tables_prod ... */
		SXBA_1_bit (prod_set, output_tables_prod);
		bot++;
	      }
	    } while ((B = sxba_scan_reset (nt_set, B)) > 0);

	    output_tables_prod = 0;

	    while ((output_tables_prod = sxba_scan_reset (prod_set, output_tables_prod)) > 0) {
	      bot = prolon [output_tables_prod];
	      top = prolon [output_tables_prod+1]-1;
	      cur_min = 0;

	      for (output_tables_item_2 = bot; output_tables_item_2 < top; output_tables_item_2++) {
		output_tables_X_2 = lispro [output_tables_item_2];

		if (output_tables_X_2 < 0)
		  cur_min++;
		else {
		  cur_min += nt2cur_min [output_tables_X_2];
		}
	      }

	      if (cur_min > prod2cur_min [output_tables_prod]) {
		/* nelle taille de output_tables_prod qui peut impacter A */
		prod2cur_min [output_tables_prod] = cur_min;
		A = lhs [output_tables_prod];
		SXBA_1_bit (nt_set, A);
	      }
	    }

	    A = 0;

	    while ((A = sxba_scan_reset (nt_set, A)) > 0) {
	      bot = bnf_ag.WS_NTMAX [A].npg;
	      top = bnf_ag.WS_NTMAX [A+1].npg;

	      cur_min = -1;

	      while (bot < top) {
		output_tables_prod = bnf_ag.WS_NBPRO [bot].numpg;

		if (cur_min == -1 || prod2cur_min [output_tables_prod] < cur_min)
		  cur_min = prod2cur_min [output_tables_prod];

		bot++;
	      }

	      if (cur_min > nt2cur_min [A]) {
		/* effectivement le min de A a change' */
		nt2cur_min [A] = cur_min;
		SXBA_1_bit (nt_set, A);
	      }
	    }
	  }

	  nt2cur_min [0] = nt2cur_min [1]+2 /* $ $ */;
	
	  out_ifdef ("nt2min_gen_lgth");
	  /* nt2min_gen_lgth [A] = l, l=min_x A =>+ x et l=|x| */
	  printf ("static SXINT nt2min_gen_lgth [%ld] = {\n", (SXINT) limnt);

	  for (A = 0; A < limnt; A++) {
	    printf ("/* %ld */\t%ld,\n", (SXINT) A, (SXINT) nt2cur_min [A]);
	  }

	  puts ("};");
	  out_endif ("nt2min_gen_lgth");

#if 0
	  puts ("\n#endif /* is_rcvr */");
#endif /* 0 */

	  sxfree (nt_set);
	  sxfree (nt2cur_min);
	  sxfree (prod2cur_min);
	  sxfree (prod_set);
	}

#if 0
	puts ("\n#if is_set_automaton");
#endif /* 0 */

	out_ifdef ("prod2t_nb");
	puts ("static SXINT prod2t_nb [] = {/* 0 */ 2,");

	for (prod = 1; prod < limpro; prod++) {
	  printf ("/* %ld */\t%ld,\n", (SXINT) prod, (SXINT) prod2t_nb [prod]);
	}

	puts ("};");
	out_endif ("prod2t_nb");

	sxfree (prod2t_nb);


	out_ifdef ("rc_titem_set");
	sxba2c (rc_titem_set, stdout, "rc_titem_set", "", true /* is_static */, output_tables_vstr);
	out_endif ("rc_titem_set");


	sxfree (titem_set);
	sxfree (rc_titem_set);

	out_ifdef ("nt2lc_item_set");
	/* ATTENTION: ne pas utiliser sxbm3_to_c car ces SXBM seront modifies dynamiquement.  Il n'est
	   donc pas possible de "partager" les ensembles egaux */
	sxbm2_to_c (stdout, nt2item_set, limnt, "nt2lc_item_set", "", true /* is_static */);
	out_endif ("nt2lc_item_set");


	out_ifdef ("nt2rc_item_set");
	sxbm2_to_c (stdout, nt2rc_item_set, limnt, "nt2rc_item_set", "", true /* is_static */);
	out_endif ("nt2rc_item_set");
      }

#if 0
      puts ("\n#endif /* is_set_automaton */");
#endif /* 0 */

      {
	SXINT output_tables_prod_2, bot, top, output_tables_item_3, output_tables_X_3;
	/* valid_suffixes = {A -> \alpha . \beta | \beta =>* \epsilon} */
	/* valid_prefixes = {A -> \alpha . \beta | \alpha =>* \epsilon} */
	/* init_titem_set = {A -> \alpha . t \beta | \beta =>* \epsilon} */
	/* prefix_titem_set = {A -> \alpha . t \beta | \alpha =>* \epsilon} */
	/* empty_prod_item_set = {A -> \alpha . \beta | \alpha \beta =>* \epsilon} */
	SXBA valid_suffixes, valid_prefixes, init_titem_set, prefix_titem_set, empty_prod_item_set;
	SXINT *npd;
	bool is_empty;

	valid_suffixes = sxba_calloc (limitem);
	valid_prefixes = sxba_calloc (limitem);
	init_titem_set = sxba_calloc (limitem);
	prefix_titem_set = sxba_calloc (limitem);
	empty_prod_item_set = sxba_calloc (limitem);

	npd = (SXINT*) sxcalloc (limnt, sizeof (SXINT)), npd [0] = limnt;

	for (output_tables_prod_2 = 0; output_tables_prod_2 < limpro; output_tables_prod_2++) {
	  bot = prolon [output_tables_prod_2];
	  output_tables_item_3 = top = prolon [output_tables_prod_2+1]-1;

	  is_empty = true;

	  while (output_tables_item_3 >= bot) {
	    if (is_empty)
	      SXBA_1_bit (valid_suffixes, output_tables_item_3);

	    if (--output_tables_item_3 >= bot) {
	      output_tables_X_3 = lispro [output_tables_item_3];

	      if (output_tables_X_3 < 0) {
		if (is_empty)
		  SXBA_1_bit (init_titem_set, output_tables_item_3);

		is_empty = false;
	      }
	      else {
		npd [output_tables_X_3]++;
		npd [0]++;

		if (is_empty && !SXBA_bit_is_set (bvide, output_tables_X_3))
		  is_empty = false;
	      }
	    }
	  }

	  if (is_empty) {
	    for (output_tables_item_3 = bot; output_tables_item_3 <= top; output_tables_item_3++) {
	      SXBA_1_bit (empty_prod_item_set, output_tables_item_3);
	      SXBA_1_bit (valid_prefixes, output_tables_item_3);
	    }
	  }
	  else {
	    for (output_tables_item_3 = bot; output_tables_item_3 < top; output_tables_item_3++) {
	      SXBA_1_bit (valid_prefixes, output_tables_item_3);
	      output_tables_X_3 = lispro [output_tables_item_3];

	      if (output_tables_X_3 < 0) {
		SXBA_1_bit (prefix_titem_set, output_tables_item_3);
		break;
	      }

	      if (!SXBA_bit_is_set (bvide, output_tables_X_3))
		break;
	    }
	  }
	}

#if 0
	puts ("\n#if is_dynam_lex");
#endif /* 0 */
	
	out_ifdef ("valid_suffixes");
	sxba2c (valid_suffixes, stdout, "valid_suffixes", "", true /* is_static */, output_tables_vstr);
	out_endif ("valid_suffixes");

	out_ifdef ("valid_prefixes");
	sxba2c (valid_prefixes, stdout, "valid_prefixes", "", true /* is_static */, output_tables_vstr);
	out_endif ("valid_prefixes");

	out_ifdef ("suffix_titem_set");
	sxba2c (init_titem_set, stdout, "suffix_titem_set", "", true /* is_static */, output_tables_vstr);
	out_endif ("suffix_titem_set");

	out_ifdef ("prefix_titem_set");
	sxba2c (prefix_titem_set, stdout, "prefix_titem_set", "", true /* is_static */, output_tables_vstr);
	out_endif ("prefix_titem_set");
#if 0
	puts ("\n#endif /* is_dynam_lex */");
#endif /* 0 */

	{
	  SXINT  lim, output_tables_i;
	  SXBA is_fully_empty;

	  out_ifdef ("npg");
	  puts ("static SXINT npg [] = {0,");

	  for (output_tables_X_3 = 1; output_tables_X_3 <= limnt; output_tables_X_3++) {
	    printf ("/* %ld */ %ld,\n", (SXINT) output_tables_X_3, (SXINT) bnf_ag.WS_NTMAX [output_tables_X_3].npg);
	  }

	  puts ("};");
	  out_endif ("npg");


	  out_ifdef ("numpg");
	  puts ("static SXINT numpg [] = {0,");

	  for (output_tables_X_3 = 1; output_tables_X_3 < limnt; output_tables_X_3++) {
	    printf ("/* %ld */ ", (SXINT) output_tables_X_3);

	    for (lim = bnf_ag.WS_NTMAX [output_tables_X_3 + 1].npg, output_tables_i = bnf_ag.WS_NTMAX [output_tables_X_3].npg; output_tables_i < lim; output_tables_i++) {
	      printf ("%ld, ", (SXINT) bnf_ag.WS_NBPRO [output_tables_i].numpg);
	    }

	    fputc ('\n', stdout);
	  }

	  puts ("};");
	  out_endif ("numpg");

	  if (is_epsilon) {
	    is_fully_empty = sxba_calloc (limnt);

	    for (output_tables_X_3 = 1; output_tables_X_3 < limnt; output_tables_X_3++) {
	      if (SXBA_bit_is_set (bvide, output_tables_X_3) && sxba_is_subset (nt2item_set [output_tables_X_3], empty_prod_item_set))
		SXBA_1_bit (is_fully_empty, output_tables_X_3);
	    }

	    out_ifdef ("is_fully_empty");
	    sxba2c (is_fully_empty, stdout, "is_fully_empty", "", true /* is_static */, output_tables_vstr);
	    out_endif ("is_fully_empty");

	    sxfree (is_fully_empty);
	  }

	  out_ifdef ("numpd");
	  puts ("static SXINT numpd [] = {0,");

	  for (output_tables_X_3 = 1; output_tables_X_3 < limnt; output_tables_X_3++) {
	    printf ("/* %ld */ ", (SXINT) output_tables_X_3);

	    for (lim = bnf_ag.WS_NTMAX [output_tables_X_3 + 1].npd, output_tables_i = bnf_ag.WS_NTMAX [output_tables_X_3].npd; output_tables_i < lim; output_tables_i++) {
	      printf ("%ld, ", (SXINT) bnf_ag.NUMPD [output_tables_i]);
	    }

	    puts ("0,");
	  }

	  puts ("};");
	  out_endif ("numpd");

	  /* ATTENTION, changement ds npd le 27/09/06, il redevient normal
	     (excepte' que les listes numpd se terminent par un zero) */
	  npd [0] = 1;

	  out_ifdef ("npd");
	  puts ("static SXINT npd [] = {0,");

	  for (output_tables_X_3 = 1; output_tables_X_3 < limnt; output_tables_X_3++) {
	    printf ("/* %ld */ %ld,\n", (SXINT) output_tables_X_3, (SXINT) npd [0]);
	    npd [0] += bnf_ag.WS_NTMAX [output_tables_X_3+1].npd - bnf_ag.WS_NTMAX [output_tables_X_3].npd + 1;
	  }
	
	  printf ("/* %ld (top) */ %ld,\n", (SXINT) output_tables_X_3, (SXINT) npd [0]);

	  puts ("};");
	  out_endif ("npd");
	}

	if (is_epsilon) {
	  out_ifdef ("bvide");
	  sxba2c (bvide, stdout, "bvide", "", true /* is_static */, output_tables_vstr);
	  out_endif ("bvide");
	}

	if (is_epsilon) {
	  out_ifdef ("empty_prod_item_set");
	  sxba2c (empty_prod_item_set, stdout, "empty_prod_item_set", "", true /* is_static */, output_tables_vstr);
	  out_endif ("empty_prod_item_set");
	}

#if 0
	/* ESSAI d'un shallow parseur grossier */

	puts ("\n#if is_shallow");
	
	sxba2c (valid_suffixes, stdout, "valid_suffixes", "", true /* is_static */, output_tables_vstr);
	sxba2c (valid_prefixes, stdout, "valid_prefixes", "", true /* is_static */, output_tables_vstr);
	sxba2c (init_titem_set, stdout, "suffix_titem_set", "", true /* is_static */, output_tables_vstr);
	sxba2c (prefix_titem_set, stdout, "prefix_titem_set", "", true /* is_static */, output_tables_vstr);

	printf ("\n\nstatic SXINT numpd [%i];\n", npd [0]+1);
	npd [0] = 0;

	puts ("\n\nstatic SXINT npd [] = {0,");

	for (output_tables_X_3 = 1; output_tables_X_3 < limnt; output_tables_X_3++) {
	  printf ("/* %i */ %i,\n", output_tables_X_3, npd [0]+= npd [output_tables_X_3]+1);
	}

	puts ("};");

	if (is_epsilon) {
	  sxba2c (bvide, stdout, "bvide", "", true /* is_static */, output_tables_vstr);
	}

	puts ("\n\nstatic char *tstring [] = {\"\",");

	for (t = 1; t < limt; t++) {
	  printf ("/* %i */\t\"%s\",\n", t, get_t_string (-t));
	}

	puts ("};");

	puts ("\n\nstatic char *ntstring [] = {\"\",");

	for (nt = 1; nt < limnt; nt++) {
	  /* On supprime les '<' et '>' des nt */
	  str_ptr = get_nt_string (nt);
	  l = strlen (str_ptr);
	  str_ptr [l-1] = SXNUL;
	  printf ("/* %i */\t\"%s\",\n", nt, str_ptr+1);
	  str_ptr [l-1] = '>';
	}

	puts ("};");
	puts ("\n#endif /* is_shallow */");
#endif /* 0 */


	sxfree (valid_suffixes);
	sxfree (valid_prefixes);
	sxfree (init_titem_set);
	sxfree (prefix_titem_set);
	sxfree (empty_prod_item_set);
	sxfree (npd);
      }
    }

    if (is_PLR) {
	puts ("\n#if is_PLR");
	out_define ("q0", (SXINT) q0);
	out_define ("q2", (SXINT) q2);
	out_define ("q3", (SXINT) q3);
	out_define ("qmax", (SXINT) qf);

	puts ("\n\nstatic SXINT q2prod [] = {");

	for (q = q0; q <= qf; q++) {
	    printf ("/* %ld */\t%ld,\n", (SXINT) q, (SXINT) q2prod [q]);
	}

	puts ("};");

	sxbm2c (stdout, kq2q_set, qf+1, "kq2q_set", "", true /* is_static */, output_tables_vstr);

	sxbm2c (stdout, nkq2q_set, qf+1, "nkq2q_set", "", true /* is_static */, output_tables_vstr);

	sxbm2c (stdout, int2q_set, limt, "int2q_set", "", true /* is_static */, output_tables_vstr);

	sxbm2c (stdout, innt2q_set, limnt, "innt2q_set", "", true /* is_static */, output_tables_vstr);

	sxbm2c (stdout, outt2q_set, limt, "outt2q_set", "", true /* is_static */, output_tables_vstr);

	sxbm2c (stdout, outnt2q_set, limnt, "outnt2q_set", "", true /* is_static */, output_tables_vstr);

	sxba2c (Q_is_shift, stdout, "Q_is_shift", "", true /* is_static */, output_tables_vstr);

	puts ("\n\nstatic SXINT delta [] = {");

	for (x = 0; x <= xdelta; x++) {
	    printf ("%ld, ", (SXINT) delta [x]);

	    if (x == xdelta || delta [x+1] == -1)
		printf ("/* %ld */\n", (SXINT) x);
	}

	puts ("};");

	puts ("\n\nstatic SXINT *qXv [] = {");

	for (x = 0; x < xqXv; x++) {
	    if ((y = qXv [x]) >= 0)
		printf ("/* %ld */ &(delta [%ld]),\n", (SXINT) x, (SXINT) y);
	    else
		puts ("NULL,");
	}

	puts ("};");

	puts ("\n\nstatic SXINT **tkq [] = {");

	for (q = q0; q <= qf; q++) {
	    if ((y = tkq [q]) >= 0)
		printf ("/* %ld */ &(qXv [%ld]),\n", (SXINT) q, (SXINT) y);
	    else
		puts ("NULL,");
	}

	puts ("};");

	puts ("\n\nstatic SXINT **ntkq [] = {");

	for (q = q0; q <= qf; q++) {
	    if ((y = ntkq [q]) >= 0)
		printf ("/* %ld */ &(qXv [%ld]),\n", (SXINT) q, (SXINT) y);
	    else
		puts ("NULL,");
	}

	puts ("};");

	puts ("\n\nstatic SXINT **tnkq [] = {");

	for (q = q0; q <= qf; q++) {
	    if ((y = tnkq [q]) >= 0)
		printf ("/* %ld */ &(qXv [%ld]),\n", (SXINT) q, (SXINT) y);
	    else
		puts ("NULL,");
	}

	puts ("};");

	puts ("\n\nstatic SXINT **ntnkq [] = {");

	for (q = q0; q <= qf; q++) {
	    if ((y = ntnkq [q]) >= 0)
		printf ("/* %ld */ &(qXv [%ld]),\n", (SXINT) q, (SXINT) y);
	    else
		puts ("NULL,");
	}

	puts ("};");

	puts ("\n#endif");
    }

#if 0
    /* Modif le 26/09/06 */
    /* Les tables sont incluses ds les parseurs */
    puts ("\n\n#ifndef no_sxparser_earley\n");
    puts ("#include PARSER_h\n");
    puts ("#endif  /* no_sxparser_earley */\n");
#endif /* 0 */

#if 0
    /* Modif le 12/09/06 */
    /* La compil des tables doit donc avoir une option d type
		-DPARSER_h=\"my_parser.h\"	\
		ou` my_parser.h peut (pour l'instant) etre sxparser_earley_lc.h ou CYK_parser.h
    */
    
    puts ("\n\n#ifndef no_sxparser_earley\n");
    puts ("#include \"sxparser_earley_lc.h\"\n");
    puts ("#endif  /* no_sxparser_earley */\n");
    puts ("\n#if is_shallow");
    puts ("\n\n#include \"sxcrude_shallow_parser.h\"");
    puts ("\n#else /* !is_shallow */");
    puts ("\n\n#include \"sxparser_earley_lc.h\"");
    puts ("\n#endif /* !is_shallow */\n");
#endif /* 0 */

    output_segments ();

    varstr_free (output_tables_vstr);
}	


static SXBA*
bmm (SXBA *P, SXBA *Q, SXBA *R, SXINT l/* , m, n */)
{
    /* Multiplication de matrices booleennes */
    /* P U= Q x R */
    /* P [l, n], Q [l, m], R [m, n] */

    SXBA	P_line, Q_line;
    SXINT		i, j;

    for (i = 0; i < l; i++)
    {
	P_line = P [i];
	Q_line = Q [i];

	j = -1;

	while ((j = sxba_scan (Q_line, j)) >= 0)
	    sxba_or (P_line, R [j]);
    }

    return P;
}

static void
follow_construction (void)
{
    /* On calcule RC (Right-Corner) : RC = {(B, A) | A -> \alpha B \beta et \beta =>* \epsilon}
       On calcule NNV (NT Voisin) : NNV = {(B, C) | U -> \alpha B \beta C \gamma et \beta =>* \epsilon}
       On calcule NTV (T Voisin) : NTV = {(A, t) | U -> \alpha B \beta t \gamma et \beta =>* \epsilon}
       FOLLOW = RC* NNV FIRST U RC* NTV */

    SXINT		xprod, A, B, C, t, item, next_item, nt;

    SXBA	*RC, *NNV, *NTV;

    RC = sxbm_calloc (limnt, limnt);
    NNV = sxbm_calloc (limnt, limnt);
    NTV = sxbm_calloc (limnt, limt);

    for (xprod = 0; xprod < limpro; xprod++)
    {
	A = lhs [xprod];

	item = prolon [xprod+1]-1;

	while ((B = lispro [--item]) > 0)
	{
	    SXBA_1_bit (RC [B], A);

	    if (!SXBA_bit_is_set (bvide, B))
		break;
	}

	item = prolon [xprod];

	while ((B = lispro [item++]) != 0)
	{
	    if (B > 0)
	    {
		next_item = item;

		while ((C = lispro [next_item++]) != 0)
		{
		    if (C > 0)
		    {
			SXBA_1_bit (NNV [B], C);

			if (!SXBA_bit_is_set (bvide, C))
			    break;
		    }
		    else
		    {
			t = -C;
			SXBA_1_bit (NTV [B], t);
			break;
		    }
		}
	    }
	}
    }

    fermer (RC, limnt);

    for (nt = 1; nt < limnt; nt++)
	SXBA_1_bit (RC [nt], nt);

    bmm (NTV, NNV, FIRST, limnt/* , limnt, limxt */); /* NTV U= NNV FIRST */
    bmm (FOLLOW, RC, NTV, limnt/* , limnt, limxt */); /* FOLLOW = RC* NTV */

    sxbm_free (RC);
    sxbm_free (NNV);
    sxbm_free (NTV);
}



static char     star_71 [] = "***********************************************************************";

static SXINT	*xt_to_ate, *xnt_to_ate;
static VARSTR	wvstr;
static char	small_string [12];


static void
alloc_alpha_table (void)
{
    xt_to_ate = (SXINT *) sxcalloc (bnf_ag.WS_TBL_SIZE.xtmax + 1, sizeof (SXINT));
    xnt_to_ate = (SXINT *) sxcalloc (bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (SXINT));
    sxstr_mngr (SXBEGIN);
    wvstr = varstr_alloc (32);
}


static void
free_alpha_table (void)
{
    if (xt_to_ate != NULL) {
	sxfree (xt_to_ate), xt_to_ate = NULL;
	sxfree (xnt_to_ate), xnt_to_ate = NULL;
	sxstr_mngr (SXEND);
	varstr_free (wvstr);
    }
}



bool		open_listing (SXINT kind, char *open_listing_language_name, char *open_listing_ME, char *component_name)
{
    struct bstr	*bstr;

    sxuse(kind); /* pour faire taire gcc -Wunused */
    if (is_listing_opened)
	return true;

    {
	char	listing_name [32];

	if ((listing = fopen (strcat (strcpy (listing_name, open_listing_language_name), component_name), "w")) == NULL) {
	    if (sxverbosep && !sxttycol1p) {
		fputc ('\n', sxtty);
		sxttycol1p = true;
	    }

	    fprintf (sxstderr, "%s: cannot open (create) ", open_listing_ME);
	    sxperror (listing_name);
	    return false;
	}
    }

    is_listing_opened = true;
    put_edit_initialize (listing);
    put_edit_nnl (9, "Listing of:");
    put_edit (25, open_listing_language_name);
    put_edit_nnl (9, "By:");
    put_edit (25, open_listing_ME);
    put_edit (25, release_mess);
    put_edit_nnl (9, "Options:");
    bstr = bstr_alloc (128, 25, 79);
    put_edit (25, bstr_tostr (options_text (/* kind, */bstr)));
    bstr_free (bstr);
    put_edit_nnl (9, "Generated on:");

    {
      time_t date_time;

      date_time = time (0);
      put_edit (25, ctime (&date_time));
    }

    put_edit_nl (3);

    if (xt_to_ate == NULL) {
	alloc_alpha_table ();
    }

    return true;
}


static char*
xt_to_str (SXINT code)
{
    /* code est negatif */
    SXINT	ate, xt_to_str_prdct;

    if (xt_to_ate == NULL) {
	alloc_alpha_table ();
    }

    if ((ate = xt_to_ate [-code]) == 0) {
	xt_to_str_prdct = XT_TO_PRDCT_CODE (-code);
	code = -XT_TO_T_CODE (-code);

	varstr_raz (wvstr);

	if ((ate = xt_to_ate [-code]) == 0) {
	    /* On ne requote qu'une fois. */
	    if (SXBA_bit_is_set (bnf_ag.IS_A_GENERIC_TERMINAL, -code)) {
		varstr_catenate (wvstr, get_t_string (code));
	    }
	    else {
		varstr_quote (wvstr, get_t_string (code));
	    }

	    ate = sxstrsave (varstr_tostr (wvstr));
	}
	else
	    varstr_catenate (wvstr, sxstrget (ate));

	if (xt_to_str_prdct >= 0) {
	    sprintf (small_string, " &%ld", (SXINT) xt_to_str_prdct);
	    varstr_catenate (wvstr, small_string);
	}

	ate = sxstrsave (varstr_tostr (wvstr));
    }

    return sxstrget (ate);
}


static char*
xnt_to_str (SXINT code)
{
    /* code est negatif */
    SXINT	ate, xtn_to_str_prdct;

    if (xt_to_ate == NULL) {
	alloc_alpha_table ();
    }

    if ((ate = xnt_to_ate [code]) == 0) {
	xtn_to_str_prdct = XNT_TO_PRDCT_CODE (code);
	code = XNT_TO_NT_CODE (code);

	varstr_raz (wvstr);

	varstr_catenate (wvstr, get_nt_string (code));

	if (xtn_to_str_prdct >= 0) {
	    sprintf (small_string, " &%ld", (SXINT) xtn_to_str_prdct);
	    varstr_catenate (wvstr, small_string);
	}

	ate = sxstrsave (varstr_tostr (wvstr));
    }

    return sxstrget (ate);
}


static void
soritem (SXINT x, SXINT d, SXINT f, char *c1, char *s2)
{
    char	car;
    SXINT	i, c;
    SXINT		noprod, addeb, adfin, xegal, j, l;
    char	tail [64], *t, *string;
    bool	is_first_time;

    noprod = prolis [x];
    addeb = prolon [noprod];
    adfin = prolon [noprod + 1] - 2;

    if (*c1 != ' ') {
	put_edit_nnl (d, c1);
	c = d + 1;
    }
    else
	c = d;

    put_edit_fnb (c, 4, noprod);

    if ((i = PROD_TO_RED (noprod)) != noprod) {
	put_edit_apnnl ("(");
	put_edit_apnb (i);
	put_edit_apnnl (")");
    }

    put_edit_apnnl (":");
    c = put_edit_get_col () + 1;

    if (noprod > 0) {
	SXINT	nt;

	nt = lhs [noprod];
	put_edit_nnl (c, get_nt_string (nt));
	c += get_nt_length (nt) + 1;
    }

    if (c > 24)  {
	put_edit_nnl (f, c1);

	if (*s2 != SXNUL) {
	    t = tail;

	    while ((car = *s2++) != SXNUL && car != '\n') {
		*t++ = car;
	    }

	    if (car == SXNUL) {
		s2--;
	    }

	    *t = SXNUL;
	    put_edit_nnl (f + 1, tail);
	}

	put_edit_nl (1);
	put_edit_nnl (d, c1);
    }

    c = 24;
    put_edit_nnl (c, "=");
    xegal = c += 2;

    for (i = addeb; i <= adfin; i++) {
	if (i == x) {
	    put_edit_nnl (c, ".");
	    c += 2;
	}

	j = bnf_ag.WS_INDPRO [i].lispro;
	string = (j>0) ? xnt_to_str (j) : xt_to_str (j);
	l = strlen (string) + 1;
	c += l;

	if (c >= f) {
	    put_edit_nnl (f, c1);

	    if (*s2 != SXNUL) {
		t = tail;

		while ((car = *s2++) != SXNUL && car != '\n') {
		    *t++ = car;
		}

		if (car == SXNUL) {
		    s2--;
		}

		*t = SXNUL;
		put_edit_nnl (f + 1, tail);
	    }

	    put_edit_nl (1);
	    put_edit_nnl (d, c1);
	    c = xegal + l;
	}

	put_edit_nnl (c - l, string);
    }

    if (x > adfin)
	put_edit_nnl (c, ".");

    if (*c1 != SXNUL)
	put_edit_nnl (f, c1);

    is_first_time = true;

    while (*s2 != SXNUL) {
	if (is_first_time) {
	    is_first_time = false;
	}
	else if (*c1 != SXNUL) {
	    put_edit_nnl (d, c1);
	    put_edit_nnl (f, c1);
	}

	t = tail;

	while ((car = *s2++) != SXNUL && car != '\n') {
	    *t++ = car;
	}

	if (car == SXNUL) {
	    s2--;
	}

	*t = SXNUL;
	put_edit_nnl (f + 1, tail);
    }

    put_edit_nl (1); /* end soritem */
}



static char*
laset (VARSTR varstring, SXINT maxi, SXBA look_ahead_set)
{
    SXINT	t, col, l;
    bool		is_first = true;
    char	*t_string;

    varstr_raz (varstring);
    varstr_catenate (varstring, " {");
    col = 3;
    t = 0;

    while ((t = sxba_scan (look_ahead_set, t)) > 0) {
	if (is_first)
	    is_first = false;
	else {
	    varstr_catenate (varstring, ", ");
	    col += 2;
	}

	t_string = xt_to_str (-t);
	col += (l = strlen (t_string));

	if (col > maxi) {
	    varstr_catenate (varstring, "\n ");
	    col = l + 2;
	}

	varstr_catenate (varstring, t_string);
    }

    return varstr_tostr (varstr_catenate (varstring, "}"));
}



static void
item2t_set_construction (void)
{
    SXINT		xprod, A, B, left, right, item; 
    SXBA	line;

    for (xprod = 0; xprod <= bnf_ag.WS_TBL_SIZE.nbpro; xprod++) {
	A = lhs [xprod];
	right = xprod == 0 ? 2 : prolon [xprod+1]-1;
	left = prolon [xprod];

	for (item = right; item >= left; item--)
	{
	    B = lispro [item];
	    line = item2t_set [item];

	    if (B == 0)
		sxba_or (line, FOLLOW [A]);
	    else
	    {
		if (B < 0)
		{
		    B = -B;
		    SXBA_1_bit (item2t_set [item], B);
		}
		else
		{
		    sxba_or (item2t_set [item], FIRST [B]);

		    if (SXBA_bit_is_set (bvide, B))
			sxba_or (item2t_set [item], item2t_set [item+1]);
		}
	    }
	}
    }

    SXBA_1_bit (item2t_set [3], my_eof); /* eof est un look-ahead de S' -> eof S eof . */
}


static bool
is_less_equal_rr (SXINT j, SXINT i)
{
    SXINT U, T;

    if (i == j)
	return true;

    U = rrnt2nt [i];
    T = rrnt2nt [j];

    return SXBA_bit_is_set (right_recursive [T], U);
}

static bool
is_less_equal_cc (SXINT j, SXINT i)
{
    SXINT U, T;

    if (i == j)
	return true;

    U = ccnt2nt [i];
    T = ccnt2nt [j];

    return SXBA_bit_is_set (cyclic [T], U);
}

static SXBA	*cycles;

static bool
is_less_equal_cycles (SXINT j, SXINT i)
{
    SXBA	line_i = cycles [i];
    SXINT		n;

    n = sxba_first_difference (cycles [j], line_i);

    return n < 0 || SXBA_bit_is_set (line_i, n);
}

static void
red_static_order (void)
{
    /* On recherche s'il existe un ordre total d'execution des reductions qui est statique */
    /* Le pb est le suivant : une reduction se decompose en 2 phases, le pop et la transition
       nt sur la LHS. Supposons qu'a un moment on a note ds une table G[A][j] l'ensemble des transitions
       Aji a effectuer. Cette table est exploitee par j decroissants. Existe t'il un ordre (statique)
       sur les nt tel que, pour un j donne si on traite G[A][j], on est sur que ce traitement ne
       positionnera pas l'element G[B][j] ou B est un nt qui precede A et qui a donc deja ete examine
       (et non traite car G[B][j] n'etait pas positionne a ce moment la).
       Si A -> \alpha B \beta et \alpha \beta =>* \epsilon, alors B doit etre examine avant A.
       Si la grammaire n'est pas cyclique, il existe un ordre total.
       (Il peut exister un ordre total, meme si la grammaire est cyclique --- cas de A -> A qui
        ne gene pas pour trouver l'ordre.)
       Si la grammaire n'a pas de productions simples (etendues par les derivations ds le vide), l'ordre
       est quelconque. */
    SXBA	*ntXnt, line, po_line;
    SXINT		*nt_stack, *nt2nb, *sorted, *porder2nt;
    SXINT		order, prod, item, X, A, B, po_A, po_B, po, nbnt;


    nt2nb = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
    nt_stack = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
    ntXnt = sxbm_calloc (limnt, limnt);

    for (prod = 1; prod < limpro; prod++)
    {
	if ((nbnt = prod2nbnt [prod]) > 0) {
	    item = prolon [prod];

	    if (lispro [item+nbnt] == 0) {
		/* Que des nt en rhs */
		A = lhs [prod];

		while ((X = lispro [item++]) != 0) {
		    if (X != A && /* On ne note pas les cycles triviaux */
			SXBA_bit_is_set (bnf_ag.NULLABLE, item) &&
			SXBA_bit_is_reset_set (ntXnt [X], A)) {
			    nt2nb [A]++;
		    }

		    if (!SXBA_bit_is_set (bvide, X))
			break;
		}
	    }
	}
    }

    order = limnt-1;

    for (A = 1; A < limnt; A++) {
	if (nt2nb [A] == 0) {
	    nt2order [A] = order;
	    order2nt [order] = A;
	    order--;
	    PUSH (nt_stack, A);
	}
    }

    if (order > 0) {
	while (!IS_EMPTY (nt_stack)) {
	    B = POP (nt_stack);
	    line = ntXnt [B];

	    A = 0;

	    while ((A = sxba_scan (line, A)) > 0) {
		if (--nt2nb [A] == 0) {
		    nt2order [A] = order;
		    order2nt [order] = A;
		    order--;
		    PUSH (nt_stack, A);
		}
	    }
	}

	if (order > 0) {
	    /* La grammaire a des cycles non-triviaux */
	    /* A l'interieur d'un cycle, l'ordre n'a pas d'importance, il y
	       aura un test dynamique. */
	    cycles = sxbm_calloc (order+1, order+1);
	    sorted = (SXINT*) sxalloc (order+1, sizeof (SXINT));
	    porder2nt = (SXINT*) sxalloc (order+1, sizeof (SXINT));

	    /* On donne un ordre provisoire */
	    po_A = 1;

	    for (A = 1; A < limnt; A++) {
		if (nt2nb [A] != 0) {
		    nt2order [A] = po_A;
		    porder2nt [po_A] = A;
		    po_A++;
		}
	    }

	    for (po_B = 1; po_B <= order; po_B++) {
		po_line = cycles [po_B];
		B = porder2nt [po_B];
		line = ntXnt [B];

		A = 0;

		while ((A = sxba_scan (line, A)) > 0) {
		    po_A = nt2order [A];
		    SXBA_1_bit (po_line, po_A);
		}
	    }

	    fermer (cycles, order+1);

	    for (po = 1; po <= order; po++)
		sorted [po] = po;

	    if (order > 1)
		/* Les + grands sont les cycles de plus haut niveau */
		sort_by_tree (sorted, 1, order, is_less_equal_cycles);

	    do {
		A = porder2nt [sorted [order]];
		nt2order [A] = order;
		order2nt [order] = A;
	    } while (--order > 0);

	    sxbm_free (cycles);
	    sxfree (sorted);
	    sxfree (porder2nt);
	}
    }

    sxbm_free (ntXnt);
    sxfree (nt_stack);
    sxfree (nt2nb);
}

static void
sem_static_order (void)
{
    /* On recherche s'il existe un ordre total d'execution des actions semantiques qui est statique */
    /* Soient ;
       p = A -> \alpha B \beta
       q = B -> \gamma
       Pour un j donne et des i' et i tq i'<= i <= j, on a a l'execution
       p = Ai'j -> \alphai'i Bij \betajj
       q = Bij -> \gammaij
       Si \beta =>* \epsilon alors si toutes les B_productions telle que q s'executent avant p,
       on est sur que la RHS de p est "connue" avant l'execution de p. */
    /* Si des productions sont mutuellement recursives droites, il n'y a pas d'ordre total. */
    /* Si la grammaire est mutuellement recursive droite, dans le cas general il n'est pas
       possible de calculer un ordre d'execution statique qui puisse s'appliquer a tous les
       cas dynamiques ex:
       I = A -> \alpha B
       II = B -> A
       III = A -> t1
       IV = B -> t2
       l'ordre d'execution des prod I et II depend de la reconnaissance de t1 ou t2. */

    SXINT		*nt_stack, *lhs_nb, *prod_nb;
    SXBA	line, prod_set, rhs_set;
    SXBA	*rhs2prod;
    SXINT		gorder, prod, item, A, B;

    prod_set = sxba_calloc (limpro);
    rhs_set = sxba_calloc (limnt);
    lhs_nb = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
    prod_nb = (SXINT*) sxcalloc (limpro, sizeof (SXINT));
    rhs2prod = sxbm_calloc (limnt, limpro);

    gorder = limpro-1;

    for (prod = 1; prod < limpro; prod++)
    {
	item = prolon [prod+1] - 2;

	if ((B = lispro [item]) <= 0)
	{
	    /* item = A -> \alpha t. ou item = A -> \epsilon */
	    order2prod [gorder] = prod;
	    prod2order [prod] = gorder;
#if 0
	    prod_order2min [gorder] = 0;
	    prod_order2max [gorder] = 0;
#endif
	    gorder--;
	}
	else
	{
	    SXBA_1_bit (prod_set, prod);
	    A = lhs [prod];
	    lhs_nb [A]++;
	    
	    do {
		/* item = A -> \alpha B . \beta et \beta =>* \epsilon */
		if (SXBA_bit_is_reset_set (rhs2prod [B], prod)) {
		    prod_nb [prod]++;
		    SXBA_1_bit (rhs_set, B);
		}
	    } while (SXBA_bit_is_set (bvide, B) && (B = lispro [--item]) > 0);
	}
    }

    if (gorder > 0) {
	nt_stack = (SXINT*) sxcalloc (limnt, sizeof (SXINT));

	B = 0;

	while ((B = sxba_scan (rhs_set, B)) > 0) {
	    /* B apparait en RHS */
	    if (lhs_nb [B] == 0) {
		/* B est evalue */
		PUSH (nt_stack, B);
	    }
	}

	while (!IS_EMPTY (nt_stack)) {
	    B = POP (nt_stack);
	    line = rhs2prod [B];

	    prod = 0;

	    while ((prod = sxba_scan (line, prod)) > 0) {
		if (--prod_nb [prod] == 0) {
		    /* prod est evalue */
		    SXBA_0_bit (prod_set, prod);
		    order2prod [gorder] = prod;
		    prod2order [prod] = gorder;
#if 0
		    prod_order2min [gorder] = 0;
		    prod_order2max [gorder] = 0;
#endif
		    gorder--;

		    A = lhs [prod];

		    if (--lhs_nb [A] == 0) {
			/* A est evalue */
			PUSH (nt_stack, A);
		    }
		}
	    }
	}

	if (gorder > 0)
	{
	    /* La grammaire est recursive droite :
	       on a A =>+ \alpha A */
	    /* Les productions impliquees sont un sous-ensemble de prod_set */

	    /* calcul de right_recursive */
	    SXBA	*RR, rr_item_set;
	    SXBA	sem_static_order_line;
	    SXINT		*nt2rrnt, *sorted;
	    SXINT		U, V, i, j, card;
	    SXINT		order_min, order_max;

	    right_recursive = sxbm_calloc (limnt, limnt);
	    nt2rrnt = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
	    rrnt2nt = (SXINT*) sxalloc (limnt, sizeof (SXINT));

	    rr_item_set = sxba_calloc (limitem);

	    prod = 0;

	    while ((prod = sxba_scan (prod_set, prod)) > 0) {
		A = lhs [prod];
		item = prolon [prod+1]-1;
		sem_static_order_line = right_recursive [A];

		while ((B = lispro [--item]) > 0)
		{
		    SXBA_1_bit (rr_item_set, item); /* On note les items candidats */
		    SXBA_1_bit (sem_static_order_line, B);

		    if (!SXBA_bit_is_set (bvide, B))
			break;
		}
	    }

	    fermer (right_recursive, limnt); /* A est recursif droit (i.e, A =>+ \alpha A) <=>
						SXBA_1_bit (right_recursive, A) */

	    /* On definit sur les NT une relation d'equivalence :
	       A equiv B <=> A right_recursive B right_recursive A
	       et on en deduit une partition des NT recursifs droits.
	       Chaque sous-ensemble de la partition est identifie par un entier i (de 1<=i<=rrnt) et
	       l'element caracteristique est rrnt2nt [i]. */

	    /* rrnt = 0; statique */

	    for (A = 1; A < limnt; A++)
	    {
		if (nt2rrnt [A] == 0)
		{
		    sem_static_order_line = right_recursive [A];

		    if (SXBA_bit_is_set (sem_static_order_line, A))
		    {
			/* A est recursif droit */
			nt2rrnt [A] = ++rrnt;
			rrnt2nt [rrnt] = A;

			B = 0;

			while ((B = sxba_scan (sem_static_order_line, B)) > 0)
			{
			    if (SXBA_bit_is_set (right_recursive [B], A)) {
				/* A et B sont equivalents */
				nt2rrnt [B] = rrnt;
			    }
			}
		    }
		}
	    }

#if EBUG
	    if (rrnt == 0)
		sxtrap (ME, "sem_static_order");
#endif


	    /* La grammaire est recursive droite */
	    /* Un item U -> \alpha .V \beta ne fait partie des ensembles d'items
	       impliques dans les recursivites droites (pour la classe d'equivalence A) ssi
	       A right_recursive U et V right_recursive A. */

	    RR = sxbm_calloc (rrnt+1, limpro);

	    item = 0;

	    while ((item = sxba_scan_reset (rr_item_set, item)) > 0)
	    {
		V = lispro [item];

		if ((i = nt2rrnt [V]) > 0) {
		    U = lhs [prolis [item]];

		    if (i == nt2rrnt [U]) {
			prod = prolis [item];
			SXBA_1_bit (RR [i], prod);
			SXBA_1_bit (rr_item_set, item); /* pour cyclic_grammar */
		    }
		}
	    }

	    sorted = (SXINT*) sxalloc (rrnt+1, sizeof (SXINT));

	    for (i = 1; i <= rrnt; i++)
		sorted [i] = i;

	    if (rrnt > 1) {
		/* Il faut "classer" les ensembles de la partition. */

		/* Si i != j, la partition j (j.) depend de la partition i (i.) ssi on a :
		   U -> \alpha .V \beta \in i.
		   Z -> \gamma .T \delta \in j. et
		   T ->+ \lambda U */

		/* En fait, on peut utiliser les elements caracteristiques */

		sort_by_tree (sorted, 1, rrnt, is_less_equal_rr);
	    }

	    sxbm_free (right_recursive);

	    for (j = rrnt; j > 0; j--) {
		i = sorted [j];
		A = rrnt2nt [i];

		/* On genere les productions de la partition "i" ds un ordre quelconque */
		sem_static_order_line = RR [i];
		card = sxba_cardinal (sem_static_order_line);
		order_max = gorder;
		order_min = gorder-card+1;

		prod = 0;

		while ((prod = sxba_scan (sem_static_order_line, prod)) > 0) {
		    SXBA_0_bit (prod_set, prod);
		    SXBA_1_bit (is_mutual_right_recursive_prod, prod);
		    prod_nb [prod] = 0; /* Evaluation "forcee" */

		    order2prod [gorder] = prod;
		    prod2order [prod] = gorder;
		    prod_order2max [gorder] = order_max;
		    prod_order2min [gorder] = order_min;
		    gorder--;

		    A = lhs [prod];

		    if (--lhs_nb [A] == 0) {
			/* A est evalue */
			PUSH (nt_stack, A);
		    }
		}

		while (!IS_EMPTY (nt_stack)) {
		    B = POP (nt_stack);
		    sem_static_order_line = rhs2prod [B];

		    prod = 0;

		    while ((prod = sxba_scan (sem_static_order_line, prod)) > 0) {
			if (SXBA_bit_is_set (prod_set, prod) && --prod_nb [prod] == 0) {
			    /* prod est evalue */
			    SXBA_0_bit (prod_set, prod);
			    order2prod [gorder] = prod;
			    prod2order [prod] = gorder;
#if 0
			    prod_order2min [gorder] = 0;
			    prod_order2max [gorder] = 0;
#endif
			    gorder--;

			    A = lhs [prod];

			    if (--lhs_nb [A] == 0) {
				/* A est evalue */
				PUSH (nt_stack, A);
			    }
			}
		    }
		}
	    }

	    sxbm_free (RR);
	    sxfree (nt2rrnt);
	    sxfree (sorted);

	    sxfree (rr_item_set);
	}

	sxfree (nt_stack);
    }

#if EBUG
    if (gorder != 0)
	sxtrap (ME, "sem_static_order");
#endif

    sxbm_free (rhs2prod);
    sxfree (prod_nb);
    sxfree (lhs_nb);
    sxfree (rhs_set);
    sxfree (prod_set);
}


static void
cyclic_grammar (void)
{
    /* Cette procedure extrait et ordonne les cycles maximaux. */
    /* calcul de cyclic */
    /* rrnt contient le nb de nt impliques dans des recursivites droites et
       rrnt2nt en est la liste */
    SXINT		prod, item, prev_item, i, A, B, U, V, X, nbnt;

    SXBA	line;
    SXBA	cc_item_set;
    SXINT		*nt2ccnt, *sorted;

    cyclic = sxbm_calloc (limnt, limnt);
    nt2ccnt = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
    ccnt2nt = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
    cc_item_set = sxba_calloc (limitem);

    for (prod = 1; prod < limpro; prod++)
    {
	if ((nbnt = prod2nbnt [prod]) > 0) {
	    item = prolon [prod];

	    if (lispro [item+nbnt] == 0) {
		/* Que des nt en rhs */
		A = lhs [prod];

		while ((X = lispro [prev_item = item++]) != 0) {
		    if (SXBA_bit_is_set (bnf_ag.NULLABLE, item)) {
			SXBA_1_bit (cyclic [A], X);
			SXBA_1_bit (cc_item_set, prev_item);
		    }

		    if (!SXBA_bit_is_set (bvide, X))
			break;
		}
	    }
	}
    }

    fermer (cyclic, limnt);	/* A est cyclique (i.e, A =>+ A) <=>
				   SXBA_1_bit (cyclic, A) */

    /* ccnt = 0; *//* statique */

    /* Modif le Mar 10 Avr 2001 11:00:54 */
    /* On n'utilise plus les partitions rrnt, ca semble faux ... */
    for (A = 1; A < limnt; A++)
    {
	if (nt2ccnt [A] == 0)
	{
	    line = cyclic [A];

	    if (SXBA_bit_is_set (line, A))
	    {
		nt2ccnt [A] = ++ccnt;
		ccnt2nt [ccnt] = A;

		B = 0;

		while ((B = sxba_scan (line, B)) > 0)
		{
		    if (SXBA_bit_is_set (cyclic [B], A)){
			/* A et B sont equivalents */
			nt2ccnt [B] = ccnt;
		    }
		}
	    }
	}
    }

    /* La grammaire est cyclique */
    /* Un item U -> \alpha .V \beta ne fait partie des ensembles d'items
       impliques dans les cycles (pour la classe d'equivalence A) ssi
       A cyclic U et V cyclic A et \alpha \beta =>* \epsilon. */

    sorted = (SXINT*) sxalloc (ccnt+1, sizeof (SXINT));

    for (i = 1; i <= ccnt; i++)
	sorted [i] = i;

    if (ccnt > 1) {
	/* Il faut "classer" les ensembles de la partition. */

	/* Si i != j, la partition j (j.) depend de la partition i (i.) ssi on a :
	   U -> \alpha .V \beta \in i.
	   Z -> \gamma .T \delta \in j. et
	   T ->+ U */

	/* En fait, on peut utiliser les elements caracteristiques */

	sort_by_tree (sorted, 1, ccnt, is_less_equal_cc);
	/* Les cycles prioritaires (de plus haut niveau) ont les numeros les plus grands. */
    }

    cycle_nb2item_set = sxbm_calloc (ccnt+1, limitem);
    item2cycle_nb = (SXINT*) sxcalloc (limitem, sizeof (SXINT));

    item = 0;

    while ((item = sxba_scan (cc_item_set, item)) > 0)
    {
	V = lispro [item];

	if ((i = nt2ccnt [V]) > 0) {
	    U = lhs [prolis [item]];

	    if (i == nt2ccnt [U]) {
		i = sorted [i];
		SXBA_1_bit (cycle_nb2item_set [i], item);
		item2cycle_nb [item] = i;
	    }
	}
    }

    sxfree (sorted);
    sxbm_free (cyclic);
    sxfree (nt2ccnt);
    sxfree (ccnt2nt);
    sxfree (cc_item_set);
}

static void
LC_construction (void)
{
  SXINT 	xprod, item, A, B, X, lim, i;
  SXBA	line, init_line;

  for (xprod = 1; xprod <= bnf_ag.WS_TBL_SIZE.nbpro; xprod++) {
    item = prolon [xprod];
    A = lhs [xprod];
    line = left_corner [A];

    while ((B = lispro [item++]) > 0)
      {
	SXBA_1_bit (line, B);

	if (!SXBA_bit_is_set (bvide, B))
	  break;
      }
  }

  fermer (left_corner, limnt);

  for (A = 1; A < limnt; A++)
    SXBA_1_bit (left_corner [A], A);
    
#if 0
  /* Abandon, c'est +lent avec une grosse grammaire (nt > 2500) */
    
  /* nt2local_prefixes [A] = {(A, A -> \alpha . \beta) | \alpha =>* \epsilon} */
  {
    SXBA	*nt2local_prefixes;

    nt2local_prefixes = sxbm_calloc (limnt, limitem);

    for (B = 1; B < limnt; B++) {
      line = nt2local_prefixes [B];

      for (lim = bnf_ag.WS_NTMAX [B + 1].npg, i = bnf_ag.WS_NTMAX [B].npg; i < lim; i++) {
	item = prolon [bnf_ag.WS_NBPRO [i].numpg];

	do {
	  SXBA_1_bit (line, item);
	  X = lispro [item++];
	} while (X > 0 && SXBA_bit_is_set (bvide, X));
      }
    }

    /* On remplit nt2item_set [A] = {B -> \alpha . \beta | B left_corner* A et \alpha =>* \epsilon} */
    /* i.e. left_corner nt2local_prefixes */
    for (A = 1; A < limnt; A++) {
      line = left_corner [A];
      init_line = nt2item_set [A];

      B = 0;

      while ((B = sxba_scan (line, B)) > 0) {
	sxba_or (init_line, nt2local_prefixes [B]);
      }
    }

    sxbm_free (nt2local_prefixes);
  }
#endif /* 0 */

  /* On remplit nt2item_set [A] = {B -> \alpha . \beta | B left_corner* A et \alpha =>* \epsilon} */
  for (A = 1; A < limnt; A++) {
    line = left_corner [A];
    init_line = nt2item_set [A];

    B = 0;

    while ((B = sxba_scan (line, B)) > 0) {
      for (lim = bnf_ag.WS_NTMAX [B + 1].npg, i = bnf_ag.WS_NTMAX [B].npg; i < lim; i++) {
	item = prolon [bnf_ag.WS_NBPRO [i].numpg];

	do {
	  SXBA_1_bit (init_line, item);
	  X = lispro [item++];
	} while (X > 0 && SXBA_bit_is_set (bvide, X));
      }
    }
  }
}

/* right_corner = {(A, B) | A =>* \alpha B \beta =>* \alpha B} */
static void
RC_construction (void)
{
  SXINT 	xprod, item, A, B, X, bot, lim, i;
  SXBA	line, init_line;

  for (xprod = 1; xprod <= bnf_ag.WS_TBL_SIZE.nbpro; xprod++) {
    bot = prolon [xprod];
    item = prolon [xprod+1]-1;
    A = lhs [xprod];
    line = right_corner [A];

    while (item > bot) {
      if ((B = lispro [--item]) > 0) {
	SXBA_1_bit (line, B);

	if (!SXBA_bit_is_set (bvide, B))
	  break;
      }
    }
  }

  fermer (right_corner, limnt);

  for (A = 1; A < limnt; A++)
    SXBA_1_bit (right_corner [A], A);
    
#if 0
  /* Abandon, c'est +lent avec une grosse grammaire (nt > 2500) */
  {
    SXBA	*nt2local_suffixes;

    /* nt2local_suffixes [A] = {(A, A -> \alpha . \beta) | \beta =>* \epsilon} */
    nt2local_suffixes = sxbm_calloc (limnt, limitem);

    for (B = 1; B < limnt; B++) {
      line = nt2local_suffixes [B];

      for (lim = bnf_ag.WS_NTMAX [B + 1].npg, i = bnf_ag.WS_NTMAX [B].npg; i < lim; i++) {
	xprod = bnf_ag.WS_NBPRO [i].numpg;
	bot = prolon [xprod];
	item = prolon [xprod+1]-1;

	do {
	  SXBA_1_bit (line, item);
	} while (item > bot && (X = lispro [--item]) > 0 && SXBA_bit_is_set (bvide, X));
      }
    }

    /* On remplit nt2rc_item_set [A] = {B -> \alpha . \beta | B right_corner A et \beta =>* \epsilon} */
    /* i.e. right_corner nt2local_suffixes */
    for (A = 1; A < limnt; A++) {
      line = right_corner [A];
      init_line = nt2rc_item_set [A];

      B = 0;

      while ((B = sxba_scan (line, B)) > 0) {
	sxba_or (init_line, nt2local_suffixes [B]);
      }
    }

    sxbm_free (nt2local_suffixes);
  }
#endif /* 0 */

  /* On remplit nt2rc_item_set [A] = {B -> \alpha . \beta | B right_corner A et \beta =>* \epsilon} */
  for (A = 1; A < limnt; A++) {
    line = right_corner [A];
    init_line = nt2rc_item_set [A];

    B = 0;

    while ((B = sxba_scan (line, B)) > 0) {

      for (lim = bnf_ag.WS_NTMAX [B + 1].npg, i = bnf_ag.WS_NTMAX [B].npg; i < lim; i++) {
	xprod = bnf_ag.WS_NBPRO [i].numpg;
	bot = prolon [xprod];
	item = prolon [xprod+1]-1;

	do {
	  SXBA_1_bit (init_line, item);
	} while (item > bot && (X = lispro [--item]) > 0 && SXBA_bit_is_set (bvide, X));
      }
    }
  }
}


static void
listing_output (void)

{
    char 	str [64];
    char		s1 [12];
    SXINT		q, i, item, nt, z, x;
    SXBA	line;

    if (!is_listing_opened)
	open_listing ((SXINT)1, language_name, "LC1", ".la.l");

    if (is_listing_opened) {
	if (sxverbosep)
	    fputs ("\tAutomaton Listing Output ... ", sxtty);

	put_edit (1, "\n*****        S T A T I C - N T - O R D E R       *****");

	for (i = limnt-1; i > 0; i--) {
	    nt = order2nt [i];
	    put_edit_fnb (8, 4, nt);
	    put_edit (16, get_nt_string (nt));
	}

	if (rrnt > 0) {
	    put_edit (1, "\n*****        S T A T I C - E V A L U A T I O N - O R D E R       *****");

	    i = limpro;

	    while (--i) {
		item = prolon [order2prod [i]+1]-1;
		sprintf (str, " [i = %ld, min_i = %ld, max_i = %ld]", (SXINT) i, (SXINT) prod_order2min [i], (SXINT) prod_order2max [i]);
		soritem (item, (SXINT)1, (SXINT)71, "", str);
	    }

	    if (ccnt > 0) {
		put_edit (1, "\n*****        C Y C L E S       *****");

		for (i = ccnt; i > 0; i--)
		{
		    put_edit_nl (2);
		    line = cycle_nb2item_set [i];
		    item = 0;

		    while ((item = sxba_scan (line, item)) > 0)
			soritem (item, (SXINT)1, (SXINT)71, "", "");
		}
	    }
	    else {
		put_edit (1, "\n\n***** This grammar is not cyclic *****");
	    }
	}
	else {
	    put_edit (1, "\n\n***** This grammar is not right-recursive *****");
	}
	    
	put_edit (1, "\n*****        1 - L O O K - A H E A D       *****");

	for (item = 1; item < limitem; item++) {
	    soritem (item, (SXINT)1, (SXINT)71, "", laset (vstr, (SXINT)60, item2t_set [item]));
	}

	if (is_PLR) {
	    put_edit (1, "*****        P L R (1)    A U T O M A T O N       *****");

	    for (q = q0; q <= qf; q++) {
		put_edit_nl (6);
		put_edit_nnl (1, "STATE ");
		put_edit_apnb (q);

		if (q != q0) {
		    SXINT	n, state;

		    sxinitialise(state);
		    put_edit_nnl (31, "Transition on ");
		    z = XxY_Y (Q_hd, q);
		    z = XxY_Y (lf_hd, z);
		    put_edit_apnnl ((z > 0) ? get_nt_string (z) : xt_to_str (z));
		    put_edit_apnnl (" from ");
		    n = 0;
		
		    for (x = q0; x <= qf; x++) {
			if (SXBA_bit_is_set (kq2q_set [x], q) || SXBA_bit_is_set (nkq2q_set [x], q)) {
			    if (++n > 1)
				break;

			    state = x;
			}
		    }

		    if (n == 1) {
			put_edit_apnnl ("state ");
			put_edit_apnb (state);
			put_edit_nl (1);
		    }
		    else {
			put_edit_ap ("states:");
			put_edit_nb (11, state);
			n = 0;

			for (x = q0; x <= qf; x++) {
			    if (SXBA_bit_is_set (kq2q_set [x], q) || SXBA_bit_is_set (nkq2q_set [x], q)) {
				if (++n > 1) {
				    put_edit_apnnl (", ");
				    state = x;

				    if (put_edit_get_col () >= 81)
					put_edit_nb (11, state);
				    else
					put_edit_apnb (state);
				}
			    }
			}
		    }
		}

		put_edit (1, star_71);
		/* On sort les items du kernel */
		item = -1;

		while ((item = sxba_scan (Q2item_set [q], item)) >= 0) {
		    if (item2Q [item] == q) {
			if (lispro [item] == 0) {
			    soritem (item, (SXINT)1, (SXINT)71, "*", laset (vstr, (SXINT)60, item2t_set [item]));
			}
			else {
			    sprintf (s1, " ---> %ld", (SXINT) item2Q [item+1]);
			    soritem (item, (SXINT)1, (SXINT)71, "*", s1);
			}
		    }
		}

		put_edit (1, star_71);

		/* On sort les items non-kernel */
		item = -1;

		while ((item = sxba_scan (Q2item_set [q], item)) >= 0) {
		    if (item2Q [item] != q) {
			if (lispro [item] == 0) {
			    soritem (item, (SXINT)1, (SXINT)71, "*", laset (vstr, (SXINT)60, item2t_set [item]));
			}
			else {
			    sprintf (s1, " ---> %ld", (SXINT) item2Q [item+1]);
			    soritem (item, (SXINT)1, (SXINT)71, "*", s1);
			}
		    }
		}

		put_edit (1, star_71);
	    }
	}

	free_alpha_table ();

	if (sxverbosep)
	    sxtime (SXACTION, "done");
    }
}


static void
new_grammar (void)
{
    /* On reecrit lhs et lispro */
    SXINT prod, item, A, X;

    limxt = bnf_ag.WS_TBL_SIZE.xtmax + 1;
    my_eof = -bnf_ag.WS_TBL_SIZE.tmax;
    limt = my_eof + 1;
    limnt = bnf_ag.WS_TBL_SIZE.ntmax + 1;
    limpro = bnf_ag.WS_TBL_SIZE.xnbpro + 1;
    limitem = bnf_ag.WS_TBL_SIZE.indpro + 1;

    lhs = (SXINT*) sxalloc (limpro, sizeof (SXINT));
    prolon = (SXINT*) sxalloc (limpro+1, sizeof (SXINT));
    semact = (SXINT*) sxalloc (limpro, sizeof (SXINT));

    lispro = (SXINT*) sxalloc (limitem, sizeof (SXINT));
    prolis = (SXINT*) sxalloc (limitem, sizeof (SXINT));
    prdct = (SXINT*) sxalloc (limitem, sizeof (SXINT));

    bvide = sxba_calloc (limnt);
    sxba_substr (bvide, bnf_ag.BVIDE, 0, 0, limnt);

    /* On change la regle supplementaire en : S' -> $ S $ */
    bnf_ag.WS_INDPRO [0].lispro = bnf_ag.WS_TBL_SIZE.tmax;
    bnf_ag.WS_NBPRO [0].prolon = 0;;

    for (prod = 0; prod < limpro; prod++) {
	A = bnf_ag.WS_NBPRO [prod].reduc;
	lhs [prod] = XNT_TO_NT_CODE (A);
	prolon [prod] = item = bnf_ag.WS_NBPRO [prod].prolon;
	semact [prod] = bnf_ag.WS_NBPRO [prod].action;

	while ((X = bnf_ag.WS_INDPRO [item].lispro) != 0) {
	    prolis [item] = prod;
	    prdct [item] = X > 0 ? XNT_TO_PRDCT_CODE (X) : XT_TO_PRDCT_CODE (-X);
	    lispro [item++] = X > 0 ? XNT_TO_NT_CODE (X) : -XT_TO_T_CODE (-X);
	}

	prdct [item] = -1;
	prolis [item] = prod;
	lispro [item] = 0;
    }

    prolon [limpro] = bnf_ag.WS_NBPRO [limpro].prolon;
}

static void
grammar_constants (void)
{
    SXINT prod, item, tnb, ntnb, Y, Ynb;
    SXINT	*l, **d;
    /* calcul de item2nbt */
    item2nbt = (SXINT*) sxcalloc (limitem, sizeof (SXINT));
    /* nb de nt a gauche du point */
    item2nbnt = (SXINT*) sxcalloc (limitem, sizeof (SXINT));
    prod2nbnt = (SXINT*) sxcalloc (limpro, sizeof (SXINT));

    is_epsilon = sxba_scan (bvide, 0) > 0;

    /* rhs_maxnt = 0;  static */
    /* rhs_lgth = 0;  static */

    for (prod = 0; prod < limpro; prod++)
    {
	item = prolon [prod] ;
	/* item2nbt [item] == 0 (calloc) */

	tnb = 0;
	ntnb = 0;
	Ynb = 0;
	
	while ((Y = lispro [item]) != 0)
	{
	    Ynb++;
	    item2nbnt [item] = ntnb;

	    if (Y < 0)
		tnb++;
	    else {
		ntnb++;
		tnb = 0;
	    }

	    item++;
	    item2nbt [item] = tnb;
	}

	prod2nbnt [prod] = item2nbnt [item] = ntnb;

	if (ntnb > rhs_maxnt)
	    rhs_maxnt = ntnb;

	if (Ynb > rhs_lgth)
	    rhs_lgth = Ynb;
    }

    /* Associe a une production et a son pieme non terminal en rhs
       le nombre de terminaux separant le p-1 eme nt du p eme.
       si p == 0, c'est le nombre de terminaux entre le dernier nt et la fin de la rhs. */
    /* SXINT prodXnt2nbt [prodmax+1][rhs_maxnt+1]; */

    l = tnb_list = (SXINT*) sxcalloc (limpro * (rhs_maxnt+1), sizeof (SXINT));
    d = tnb_list_disp = (SXINT**) sxalloc (limpro, sizeof (SXINT*));

    for (prod = 0; prod < limpro; prod++) {
	*d++ = l;
	l += rhs_maxnt+1;
    }

    
    for (prod = 0; prod < limpro; prod++)
    {
	item = prolon [prod] ;
	/* item2nbt [item] == 0 (calloc) */

	tnb = 0;
	ntnb = 0;
	l = tnb_list_disp [prod];
	
	while ((Y = lispro [item]) != 0)
	{
	    if (Y < 0)
		tnb++;
	    else {
		ntnb++;
		l [ntnb] = tnb;
		tnb = 0;
	    }

	    item++;
	}

	l [0] = tnb;
    }
    
}




static void
left_factoring (void)
{
    /* On remplit lf_hd et item2lf */
    SXINT	prod, item, lf, X, A, q, wall;
    	
    lf_empty_string = 0;
    q0 = 0;
    SXBA_1_bit (Q2item_set [q0], 0 /* [S' -> . $ S $] */);
    SXBA_1_bit (NKQ2item_set [q0], 0 /* [S' -> . $ S $] */);
    item2Q [0] = q0;

    for (prod = 0; prod < limpro; prod++) {
	A = lhs [prod];
	lf = lf_empty_string;
	item = prolon [prod];
	X = lispro [item];
	wall = prolon [prod+1]-1;

	while (++item <= wall) {
	    XxY_set (&lf_hd, lf, X, &lf);
	    XxY_set (&Q_hd, A, lf, &q);
	    item2Q [item] = q;

	    SXBA_1_bit (KQ2item_set [q], item);
	    SXBA_1_bit (Q2item_set [q], item);

	    if ((X = lispro [item]) > 0) {
		sxba_or (NKQ2item_set [q], nt2item_set [X]);
		sxba_or (Q2item_set [q], nt2item_set [X]);
	    }
	}
    }

    q1 = item2Q [1 /* [S' -> $ . S $] */];
    q2 = item2Q [2 /* [S' -> $ S . $] */];
    q3 = item2Q [3]; /* S' -> eof S eof . */
    qf = XxY_top (Q_hd);
}


static void
fill_delta (void)
{
    SXINT		q, r, X, t, nt, item, citem;
    SXBA	item_set, t_set, q_set;
    SXINT		k_item, nk_item;
    SXBA	k_item_set, nk_item_set, *subset;
    SXINT		old_xqXv;
    bool	is_empty;

    SXBA_1_bit (Q_is_shift, q0);

    for (q = q0; q <= qf;q++) {
	/* kernel */
	k_item_set = KQ2item_set [q];
	k_item = -1;

	while ((k_item = sxba_scan (k_item_set, k_item)) >= 0) {
	    if ((X = lispro [k_item]) != 0) {
		SXBA_1_bit (Q_is_shift, q);

		if (X > 0)
		    SXBA_1_bit (outnt2q_set [X], q);
		else
		    SXBA_1_bit (outnt2q_set [0], q);

		r = item2Q [k_item+1];
		subset = (X > 0) ? KQNT [q] : (X = -X, KQT [q]);
		SXBA_1_bit (subset [X], r);
	    }
	}

	/* non-kernel */
	nk_item_set = NKQ2item_set [q];
	nk_item = -1;

	while ((nk_item = sxba_scan (nk_item_set, nk_item)) >= 0) {
	    if ((X = lispro [nk_item]) != 0) {
		if (X > 0)
		    SXBA_1_bit (outnt2q_set [X], q);
		else
		    SXBA_1_bit (outnt2q_set [0], q);

		r = item2Q [nk_item+1];
		subset = (X > 0) ? NKQNT [q] : (X = -X, NKQT [q]);
		SXBA_1_bit (subset [X], r);
	    }
	}
    }

    xdelta = -1;
    xqXv = 0;

    for (q = q0; q <= qf; q++) {
	subset = KQT [q];
	old_xqXv = xqXv;
	is_empty = true;

	for (t = 0; t <= my_eof; t++) {
	    q_set = subset [t];

	    if (sxba_is_empty (q_set))
		qXv [xqXv++] = -1;
	    else {
		is_empty = false;
		delta [++xdelta] = -1;

		r = -1;

		while ((r = sxba_scan (q_set, r)) >= 0)
		    delta [++xdelta] = r;

		qXv [xqXv++] = xdelta;
	    }
	}

	if (is_empty) {
	    xqXv = old_xqXv;
	    tkq [q] = -1;
	}
	else {
	    tkq [q] = old_xqXv;
	}
    }

    for (q = q0; q <= qf; q++) {
	subset = KQNT [q];
	old_xqXv = xqXv;
	is_empty = true;

	for (nt = 0; nt < limnt; nt++) {
	    q_set = subset [nt];

	    if (sxba_is_empty (q_set))
		qXv [xqXv++] = -1;
	    else {
		is_empty = false;
		delta [++xdelta] = -1;

		r = -1;

		while ((r = sxba_scan (q_set, r)) >= 0)
		    delta [++xdelta] = r;

		qXv [xqXv++] = xdelta;
	    }
	}

	if (is_empty) {
	    xqXv = old_xqXv;
	    ntkq [q] = -1;
	}
	else {
	    ntkq [q] = old_xqXv;
	}
    }

    for (q = q0; q <= qf; q++) {
	subset = NKQT [q];
	old_xqXv = xqXv;
	is_empty = true;

	for (t = 0; t <= my_eof; t++) {
	    q_set = subset [t];

	    if (sxba_is_empty (q_set))
		qXv [xqXv++] = -1;
	    else {
		is_empty = false;
		delta [++xdelta] = -1;

		r = -1;

		while ((r = sxba_scan (q_set, r)) >= 0)
		    delta [++xdelta] = r;

		qXv [xqXv++] = xdelta;
	    }
	}

	if (is_empty) {
	    xqXv = old_xqXv;
	    tnkq [q] = -1;
	}
	else {
	    tnkq [q] = old_xqXv;
	}
    }

    for (q = q0; q <= qf; q++) {
	subset = NKQNT [q];
	old_xqXv = xqXv;
	is_empty = true;

	for (nt = 0; nt < limnt; nt++) {
	    q_set = subset [nt];

	    if (sxba_is_empty (q_set))
		qXv [xqXv++] = -1;
	    else {
		is_empty = false;
		delta [++xdelta] = -1;

		r = -1;

		while ((r = sxba_scan (q_set, r)) >= 0)
		    delta [++xdelta] = r;

		qXv [xqXv++] = xdelta;
	    }
	}

	if (is_empty) {
	    xqXv = old_xqXv;
	    ntnkq [q] = -1;
	}
	else {
	    ntnkq [q] = old_xqXv;
	}
    }

    /* On considere la transition de q0 vers q1 comme etant non-kernel */
    SXBA_1_bit (nkq2q_set [q0], q1);
    SXBA_1_bit (Q2t_set [q0], my_eof);

    for (item = 1; item < limitem; item++) {
	q = item2Q [item];

	if (q > 0) {
	    /* item du kernel */
#if EBUG
	    if (item == 0 || lispro [item-1] == 0)
		sxtrap (ME, "fill_delta");
#endif

	    X = lispro [item-1];

	    if (X < 0)
		SXBA_1_bit (int2q_set [-X], q);
	    else
		/* Ensemble des etats dont la in_transition est terminale ou X */
		SXBA_1_bit (innt2q_set [X], q);

	    sxba_or (Q2t_set [q], item2t_set [item]);

	    if (lispro [item] == 0)
		/* item reduce */
		q2prod [q] = prolis [item];

	    item_set = Q2item_set [q];
	    q_set = nkq2q_set [q];
	    citem = -1;

	    while ((citem = sxba_scan (item_set, citem)) >= 0) {
		if (lispro [citem] != 0) {
		    /* item shift */
		    r = item2Q [citem+1];

		    /* FAUX : un item A -> \alpha . \beta peut etre a la fois kernel et non_kernel
		       si \alpha != \epsilon et \alpha =>+ \epsilon */
		    if (citem == item)
			SXBA_1_bit (kq2q_set [q], r);
		    else
			SXBA_1_bit (q_set, r);
		}
	    }
	}
    }

    for (q = q0; q <= qf; q++) {
	t_set = Q2t_set [q];
	t = 0;

	while ((t = sxba_scan (t_set, t)) > 0)
	    SXBA_1_bit (outt2q_set [t], q);
    }

    SXBA_1_bit (outt2q_set [my_eof], q3); /* eof est un look-ahead de S' -> eof S eof . */

    /* Le terminal Any, de code 0 regroupe tous les terminaux sauf eof */
    for (t = my_eof-1; t > 0; t--) {
	sxba_or (outt2q_set [0], outt2q_set [t]);
    }
}


SXINT
LC1 (void)
{
    SXBA	line;
    SXINT		nt, xt, t, X, item, prod, nb, v;

    if (sxverbosep) {
	fprintf (sxtty, "%s\n", ME);

	if (print_time)
	    sxtime (SXINIT, NULL);
    }

    if (sxverbosep)
	fputs ("\tLC (1) Automaton ... ", sxtty);

    /* On reecrit la grammaire */
    new_grammar ();

    /* On parcourt la grammaire pour en deduire qq constantes. */
    grammar_constants ();

    /* On reduit FIRST */
    FIRST = bnf_ag.FIRST;

    if (limxt > limt)
    {
	for (nt = 1; nt < limnt; nt++)
	{
	    line = FIRST [nt];

	    xt = limt-1;

	    while ((xt = sxba_scan_reset (line, xt)) > 0) {
		t = XT_TO_T_CODE (xt);
		SXBA_1_bit (line, t);
	    }

	    line [0] = (SXBA_ELT) limt; /* On change la longueur */
	}

    }

    /* calcul de FOLLOW */
    FOLLOW = sxbm_calloc (limnt, limt);
    follow_construction ();


    /* Calcul de item2t_set
       item2t_set (A -> \alpha . \beta) = FIRST (\beta) FOLLOW (A).
    */

    item2t_set = sxbm_calloc (limitem, limt);
    item2t_set_construction ();
    t2item_set = sxbm_calloc (limt, limitem);

    for (item = 0; item < limitem; item++) {
	line = item2t_set [item];

	t = 0;

	while ((t = sxba_scan (line, t)) > 0)
	    SXBA_1_bit (t2item_set [t], item);
    }

    /* Le terminal Any, de code 0 regroupe tous les terminaux sauf eof */
    for (t = my_eof-1; t > 0; t--)
	sxba_or (t2item_set [0], t2item_set [t]);


    sxbm_free (FOLLOW);

    /* Calculs de rhs_nt2where et lhs_nt2where. */
    /* rhs et lhs decoupent le range des items :
       les items terminaux, les items non-terminaux et les les items reduce. */
    /* Le NT de code 0 (S') n'apparait pas en RHS. 0 est utilise pour les terminaux. */
    /* rhs_nt2where [0] = 0
       rhs_nt2where [1] = nb d'occurrences des terminaux en rhs
       si A \in NT et A > 1
       rhs_nt2where [A] = rhs_nt2where [A-1]+nb d'occurrences du NT A-1 en RHS. */
    /* lhs_nt2where [0] = rhs_nt2where [ntmax]+nb d'occurrences du NT ntmax en RHS
       si A \in NT
       lhs_nt2where [A] = lhs_nt2where [A-1]+nb d'occurrences du NT A-1 en RHS. */
    rhs_nt2where = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
    lhs_nt2where = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
	
    for (prod = 0; prod < limpro; prod++) {
	for (item = prolon [prod]; (X = lispro [item]) != 0; item++) {
	    if (X < 0) {
		X = 0;
	    }

	    rhs_nt2where [X]++;
	}

	lhs_nt2where [lhs [prod]]++;
    }

    nb = rhs_nt2where [0];
    rhs_nt2where [0] = 0; /* Les terminaux commencent en 0 */

    for (nt = 1; nt < limnt; nt++) {
	v = rhs_nt2where [nt];
	rhs_nt2where [nt] = nb;
	nb += v;
    }

    for (nt = 0; nt < limnt; nt++) {
	v = lhs_nt2where [nt];
	lhs_nt2where [nt] = nb;
	nb += v;
    }


    /* On cherche un ordre statique d'execution des reductions */
    nt2order = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
    order2nt = (SXINT*) sxcalloc (limnt, sizeof (SXINT));
    red_static_order ();

    /* On cherche un ordre statique d'execution des actions semantiques */
    order2prod = (SXINT*) sxcalloc (limpro, sizeof (SXINT));
    prod2order = (SXINT*) sxcalloc (limpro, sizeof (SXINT));
    prod_order2min = (SXINT*) sxcalloc (limpro, sizeof (SXINT));
    prod_order2max = (SXINT*) sxcalloc (limpro, sizeof (SXINT));
    is_mutual_right_recursive_prod = sxba_calloc (limpro);
    sem_static_order ();

    if (rrnt > 0)
	cyclic_grammar ();

    if (rrnt2nt != NULL)
	sxfree (rrnt2nt);

    /* calcul de left_corner */
    nt2item_set = sxbm_calloc (limnt, limitem);
    left_corner = sxbm_calloc (limnt, limnt);

    LC_construction ();

    /* calcul de right_corner */
    nt2rc_item_set = sxbm_calloc (limnt, limitem);
    right_corner = sxbm_calloc (limnt, limnt);

    RC_construction ();

    if (sxverbosep)
	sxtime (SXACTION, "done");

    if (is_PLR) {
	/* Calcul de l'automate PLR */
	SXINT q;

	if (sxverbosep)
	    fputs ("\tPLR (1) Automaton ... ", sxtty);

	item2lf = (SXINT*) sxcalloc (limitem, sizeof (SXINT));
	item2Q = (SXINT*) sxcalloc (limitem, sizeof (SXINT));
	Q2item_set = sxbm_calloc (limitem, limitem);
	Q2t_set = sxbm_calloc (limitem, limt);

	KQ2item_set = sxbm_calloc (limitem, limitem);
	NKQ2item_set = sxbm_calloc (limitem, limitem);

	XxY_alloc (&lf_hd, "lf", limitem, 1, 0, 0, NULL,
#if EBUG
		   sxtty
#else
		   NULL
#endif
		   );

	XxY_alloc (&Q_hd, "Q", limitem, 1, 0, 0, NULL,
#if EBUG
		   sxtty
#else
		   NULL
#endif
		   );

	left_factoring ();

	/* q2q_set [q] = {A -> \alpha X0 X1 ... Xl | A -> \alpha = q et X1 ... Xl =>* \epsilon}
	   t2q_set [t] = {A -> \alpha t X1 ... Xl | X1 ... Xl =>* \epsilon} U
	   { A -> \alpha | [A -> \alpha .] \in P et t \in FOLLOW1 [A]}
	   nt2q_set [nt] = {A -> \alpha nt X1 ... Xl | X1 ... Xl =>* \epsilon}
	   q2prod [q] = s'il contient un item reduce p alors p sinon 0
	   */

	q2prod = (SXINT*) sxcalloc (qf+1, sizeof (SXINT));
	kq2q_set = sxbm_calloc (qf+1, qf+1);
	nkq2q_set = sxbm_calloc (qf+1, qf+1);
	int2q_set = sxbm_calloc (limt, qf+1);
	innt2q_set = sxbm_calloc (limnt, qf+1);
	outt2q_set = sxbm_calloc (limt, qf+1);

	KQT = (SXBA **) sxcalloc (qf+1, sizeof (SXBA*));
	KQNT = (SXBA **) sxcalloc (qf+1, sizeof (SXBA*));
	NKQT = (SXBA **) sxcalloc (qf+1, sizeof (SXBA*));
	NKQNT = (SXBA **) sxcalloc (qf+1, sizeof (SXBA*));

	KQxT = sxbm_calloc ((qf+1)*limt, qf+1);
	KQxNT = sxbm_calloc ((qf+1)*limnt, qf+1);
	NKQxT = sxbm_calloc ((qf+1)*limt, qf+1);
	NKQxNT = sxbm_calloc ((qf+1)*limnt, qf+1);

	for (q = q0; q <= qf; q++) {
	    KQT [q] = &(KQxT [q*limt]);
	    KQNT [q] = &(KQxNT [q*limnt]);
	    NKQT [q] = &(NKQxT [q*limt]);
	    NKQNT [q] = &(NKQxNT [q*limnt]);
	}

	tkq = (SXINT*) sxcalloc (qf+1, sizeof (SXINT));
	ntkq = (SXINT*) sxcalloc (qf+1, sizeof (SXINT));
	tnkq = (SXINT*) sxcalloc (qf+1, sizeof (SXINT));
	ntnkq = (SXINT*) sxcalloc (qf+1, sizeof (SXINT));

	qXv = (SXINT*) sxcalloc (2*(qf+1)*(limt+limnt), sizeof (SXINT));
	delta = (SXINT*) sxcalloc (2*(qf+1)*(limt+limnt)*(qf+1), sizeof (SXINT));

	outnt2q_set = sxbm_calloc (limnt, qf+1);
	Q_is_shift = sxba_calloc (qf+1);

	fill_delta ();

	if (sxverbosep)
	    sxtime (SXACTION, "done");
    }

    if (sxverbosep && ccnt > 0)
	fputs ("\t\tThis grammar is cyclic\n", sxtty);

    if (is_automaton)
	listing_output ();

    if (sxverbosep)
	    fputs ("\tTables Output ... ", sxtty);

    if (is_ELC)
      output_elc_tables ();
    else
      output_tables ();

    if (sxverbosep)
	sxtime (SXACTION, "done");

    sxbm_free (item2t_set);
    sxbm_free (t2item_set);
    
    sxfree (order2prod);
    sxfree (prod2order);
    sxfree (prod_order2min);
    sxfree (prod_order2max);
    sxfree (is_mutual_right_recursive_prod);

    sxfree (item2nbt);
    sxfree (item2nbnt);
    sxfree (prod2nbnt);
    sxbm_free (nt2item_set);
    sxbm_free (left_corner);
    sxbm_free (nt2rc_item_set);
    sxbm_free (right_corner);
    sxfree (tnb_list);
    sxfree (tnb_list_disp);
    sxfree (order2nt);
    sxfree (nt2order);

    sxfree (rhs_nt2where);
    sxfree (lhs_nt2where);

    sxfree (lhs);
    sxfree (lispro);
    sxfree (prolis);
    sxfree (prdct);
    sxfree (prolon);
    sxfree (semact);

    sxfree (bvide);

    if (ccnt > 0) {
	sxbm_free (cycle_nb2item_set);
	sxfree (item2cycle_nb);
    }

    if (is_PLR) {
	sxfree (item2lf);
	sxfree (item2Q);
	sxbm_free (Q2item_set);
	sxbm_free (Q2t_set);
	XxY_free (&lf_hd);
	XxY_free (&Q_hd);
	sxfree (q2prod);
	sxbm_free (kq2q_set);
	sxbm_free (nkq2q_set);
	sxbm_free (int2q_set);
	sxbm_free (innt2q_set);
	sxbm_free (outt2q_set);
	
	sxbm_free (KQ2item_set);
	sxbm_free (NKQ2item_set);
	sxfree (KQT);
	sxfree (KQNT);
	sxfree (NKQT);
	sxfree (NKQNT);
	sxbm_free (KQxT);
	sxbm_free (KQxNT);
	sxbm_free (NKQxT);
	sxbm_free (NKQxNT);
	sxfree (tkq);
	sxfree (ntkq);
	sxfree (tnkq);
	sxfree (ntnkq);
	sxfree (qXv);
	sxfree (delta);

	sxbm_free (outnt2q_set);
	sxfree (Q_is_shift);
    }

    if (is_listing_opened) {
	put_edit_finalize ();
	is_listing_opened = false;
    }

    {
        extern bool	parser_write (struct parser_ag_item *parser_ag_ptr, char *langname);

	parser_ag.bnf_modif_time = bnf_ag.bnf_modif_time;
	/* Hack: on utilise nd_degree pour indiquer a "tables_c" qu'il doit lire les "pat"
	   (tables produites par lig2bnf). nd_degree >= 0 => non-determinisme. */
	parser_ag.constants.nd_degree = 0;
	parser_write (&parser_ag, language_name);
    }

#if 0
    final_state = 3; /* 0: ? = 0 EOF 1 <AXIOME> 2 EOF 3 ; */
#endif

    return 0;
}

