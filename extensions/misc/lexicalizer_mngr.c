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

#include "sxversion.h"
#include "sxunix.h"

char WHAT_LEXICALIZER_MNGR[] = "@(#)SYNTAX - $Id: lexicalizer_mngr.c 4305 2024-09-16 16:27:23Z garavel $" WHAT_DEBUG;

static char	ME [] = "lexicalizer_mngr";

#if EBUG
static SXINT INDEX_VAL;
#define CHECK_INDEX(size,x)   (INDEX_VAL=x, (INDEX_VAL >= 0 || INDEX_VAL<size) ? INDEX_VAL : sxtrap (ME, "under or overflow"))
#else /* !EBUG */
#define CHECK_INDEX(size,x)   x
#endif /* !EBUG */

#ifdef LC_TABLES_H
/* On compile les tables "left_corner" ... */
/* ... uniquement celles dont on peut avoir besoin */
/* Elles contiennent entre autre : */
/* ------------------------------------------------------------------------ */
/* left_corner = {(A, B) | A =>* \alpha B \beta =>* B \beta} */
/* nt2item_set [A] = {B -> \alpha . \beta | B left_corner A et \alpha =>* \epsilon} */
/* t2item_set [t] = {A -> \alpha . \beta | t \in First1(\beta Follow1(A))} */
/* BVIDE = {A | A =>+ \epsilon} */
/* is_fully_empty = {A | si A =>+ x alors x = \epsilon} (nt ne derivant que ds le vide) */
/* t2prod_item_set [t] = { A -> \alpha . \beta | \alpha \beta == \gamma t \delta} */
/* titem_set = { A -> \alpha . t \beta } */
/* multiple_t_item_set = { A -> \alpha . t1 \beta | \alpha \beta = \gamma t2 \delta} */
/* rc_titem_set = { A -> \alpha t1 . \beta } */
/* nt2lc_item_set [A] = nt2item_set [A] */
/* nt2rc_item_set [A] = {B -> \alpha . \beta | B right_corner A et \beta =>* \epsilon} */
/* valid_suffixes = {A -> \alpha . \beta | \beta =>* \epsilon} */
/* valid_prefixes = {A -> \alpha . \beta | \alpha =>* \epsilon} */
/* suffix_titem_set = {A -> \alpha . t \beta | \beta =>* \epsilon} */
/* prefix_titem_set = {A -> \alpha . t \beta | \alpha =>* \epsilon} */
/* empty_prod_item_set = {A -> \alpha . \beta | \alpha \beta =>* \epsilon} */
/* ------------------------------------------------------------------------ */

/* On prend les valeurs initiales dans spf.inputG (compilees ds l'appelant earley par exemple) */

#define def_t2prod_item_set

/* Ds le cas #define HUGE_CFG */
#define def_t2prod_hd

#ifdef is_check_rhs_sequences
#define def_segment
/* definit segment et segment_list
   segment est // a lispro.  On a si i = segment [item] :
   i == -1 => fin d'un segment
   i == 0 => 
*/
#endif /* is_check_rhs_sequences */


/* Si ifndef HUGE_CFG */
#define def_multiple_t_item_set
/* Si ifdef HUGE_CFG */
#define def_multiple_t_prod_set

#if is_set_automaton
#define def_titem_set
#define def_rc_titem_set
#define def_nt2rc_item_set
#define def_nt2lc_item_set
#define def_empty_prod_item_set
#endif /* is_set_automaton */


#include LC_TABLES_H
#endif /* LC_TABLES_H */

#ifdef MAKE_INSIDEG
/* Compilation de lexicalizer_mngr avec l'option -DMAKE_INSIDEG
   La grammaire peut etre huge ou pas, mais elle sera traitee comme si elle etait huge */
#else /* !MAKE_INSIDEG */
/* Si la grammaire est "huge", on force la fabrication d'unr nelle grammaire filtree par le source */
#  ifdef HUGE_CFG
#  define MAKE_INSIDEG
#  endif /* HUGE_CFG */
#endif /* !MAKE_INSIDEG */

/* On peut forcer MULTI_ANCHOR en compilant avec la directive -DMULTI_ANCHOR */
/* Ds le cas "ifdef HUGE_CFG" on ne definit pas MULTI_ANCHOR pour 2 raisons
   - la sous-grammaire reduite peut etre vide (et l'analyseur ne peut donc pas s'executer)
   - la rcvr eventuelle ne pourra pas s'appuyer sur les prod a ancres multiples qui auront disparues
*/
#ifndef MULTI_ANCHOR
#  ifdef MAKE_PROPER
#  define MULTI_ANCHOR
#  else /* ifndef MAKE_PROPER */
#    ifndef HUGE_CFG
#    define MULTI_ANCHOR
#    endif /* ifndef HUGE_CFG */
#  endif /* ifndef MAKE_PROPER */
#endif /* ifndef MULTI_ANCHOR */


#ifdef MAKE_PROPER
#define no_sxspf_function_declarations
#include "sxspf.h"
extern bool is_print_time;// = false;
static SXINT TIME_FINAL = 0;
static SXINT n = 0;
#else
#include "earley.h" /* qui inclut sxspf.h */
#endif /* MAKE_PROPER */
#include "udag_scanner.h"

/* Pour acceder aux constantes initiales des tables statiques */
#define inputG_MAXITEM     itemmax
#define inputG_MAXPROD     prodmax
#define inputG_MAXNT       ntmax
#define inputG_MAXT        (tmax)
#define inputG_SXEOF       (-tmax)

/* Pour acceder soit a insideG soit aux tables statiques */
#ifdef MAKE_INSIDEG

#define SXEOF           (-spf.insideG.maxt)
#define MAXNT           spf.insideG.maxnt
#define MAXT            spf.insideG.maxt
#define MAXITEM         spf.insideG.maxitem
#define MAXPROD         spf.insideG.maxprod
#define MAXRHSLGTH      spf.insideG.maxrhs
#define MAXRHSNT        spf.insideG.maxrhsnt
#define IS_EPSILON      spf.insideG.is_eps
#define IS_RIGHT_RECURSIVE      (spf.insideG.right_recursive_set != NULL)
#define IS_CYCLIC       spf.insideG.is_cycle

#define LISPRO(i)       spf.insideG.lispro [i]
#define PROLIS(i)       spf.insideG.prolis [i]
#define PROLON(i)       spf.insideG.prolon [i]
#define LHS(i)          spf.insideG.lhs [i]
#define NPG(i)          spf.insideG.npg [i]
#define NUMPG(i)        spf.insideG.numpg [i]
#define NPD(i)          spf.insideG.npd [i]
#define NUMPD(i)        spf.insideG.numpd [i]
#define TPD(i)          spf.insideG.tpd [i]
#define TNUMPD(i)       spf.insideG.tnumpd [i]
#define PROD(i)         spf.insideG.prod2init_prod [i]
#define NT(i)           spf.insideG.nt2init_nt [i]
#define T(i)            spf.insideG.t2init_t [i]
#define NTSTRING(i)     ntstring [spf.insideG.nt2init_nt [i]]
#define TSTRING(i)      tstring [spf.insideG.t2init_t [i]]

#define RHS_NT2WHERE(i) spf.insideG.rhs_nt2where [i]
#define LHS_NT2WHERE(i) spf.insideG.lhs_nt2where [i]
             
#define	EMPTY_SET           spf.insideG.bvide
#define	TITEM_SET           spf.insideG.titem_set
#define	RC_TITEM_SET        spf.insideG.rc_titem_set
#define	EMPTY_PROD_ITEM_SET spf.insideG.empty_prod_item_set
#define	LEFT_CORNER(i)      spf.insideG.left_corner [i]
#define	NT2ITEM_SET(i)      spf.insideG.nt2item_set [i]
#define	RIGHT_CORNER(i)     spf.insideG.right_corner [i]
#define	NT2RC_ITEM_SET(i)   spf.insideG.nt2rc_item_set [i]
#define	VALID_PREFIXES      spf.insideG.valid_prefixes
#define	VALID_SUFFIXES      spf.insideG.valid_suffixes
#define	T2PROD_ITEM_SET(i)  spf.insideG.t2prod_item_set [i]
#define	T2ITEM_SET(i)       spf.insideG.t2item_set [i]
#define	NT2MIN_GEN_LGTH(i)  spf.insideG.nt2min_gen_lgth [i]

#else /* MAKE_INSIDEG */

#define SXEOF           (-tmax)
#define MAXNT           ntmax
#define MAXT            tmax
#define MAXITEM         itemmax
#define MAXPROD         prodmax
#define MAXRHSLGTH      rhs_lgth
#define MAXRHSNT        rhs_maxnt
#define IS_EPSILON      is_epsilon
#define IS_RIGHT_RECURSIVE      is_right_recursive
#define IS_CYCLIC       is_cyclic


#define LISPRO(i)       spf.inputG.lispro [i]
#define PROLIS(i)       spf.inputG.prolis [i]
#define PROLON(i)       spf.inputG.prolon [i]
#define LHS(i)          spf.inputG.lhs [i]
#define NPG(i)          spf.inputG.npg [i]
#define NUMPG(i)        spf.inputG.numpg [i]
#define NPD(i)          spf.inputG.npd [i]
#define NUMPD(i)        spf.inputG.numpd [i]
#define PROD(i)         i
#define NT(i)           i
#define T(i)            i
#define NTSTRING(i)     spf.inputG.ntstring [i]
#define TSTRING(i)      spf.inputG.tstring [i]
#define	EMPTY_SET       spf.inputG.BVIDE
             
#define	TITEM_SET           titem_set
#define	RC_TITEM_SET        rc_titem_set
#if is_epsilon
#define	EMPTY_PROD_ITEM_SET empty_prod_item_set
#else
#define	EMPTY_PROD_ITEM_SET NULL
#endif /* is_epsilon */
#define	LEFT_CORNER(i)      left_corner [i]
#define	NT2ITEM_SET(i)      nt2lc_item_set [i]
#define	RIGHT_CORNER(i)     right_corner [i]
#define	NT2RC_ITEM_SET(i)   nt2rc_item_set [i]
#define	VALID_PREFIXES      valid_prefixes
#define	VALID_SUFFIXES      valid_suffixes
#define	T2PROD_ITEM_SET(i)  t2prod_item_set [i]
#define	T2ITEM_SET(i)       t2item_set [i]
#define	NT2MIN_GEN_LGTH(i)  nt2min_gen_lgth [i]

#endif /* MAKE_INSIDEG */

#include "sxstack.h"
  
static SXBA    *mlstn2suffix_source_set;
static SXBA    *t2suffix_t_set;
#ifndef MAKE_INSIDEG
static SXBA    lex_source_set;
#endif /* !MAKE_INSIDEG */

/* Ds le cas HUGE_CFG, les calculs initiaux se font ds basic_prod_stack */
/* Pas un tableau statique pour gagner de la place !! */
#ifdef HUGE_CFG
static SXINT     *basic_prod_stack;
/* static SXBA    unlexicalized_prod_set; est accessible ds les tables statiques si def_t2prod_hd */
#else /* ifndef HUGE_CFG */
static SXBA    unlexicalized_item_set;
#endif /* ifndef HUGE_CFG */

/* C'est lexicalizer_mngr qui fait les alloc */
/* Ces variables externes sont accessibes depuis earley */
SXBA           basic_item_set, basic_nt_set; 
SXBA           *mlstn2lex_la_tset;
SXBA           *mlstn2la_tset;

#if is_set_automaton
static bool is_mlstn2lex_la_tset_for_dsa;
#endif /* is_set_automaton */

/* la == look-ahead, lb == look-back */
static SXBA *t2la_t_set;



#if LOG
extern void output_nt (SXINT nt);
extern void output_t (SXINT t);
#endif /* LOG */
#if LOG || EBUG
extern void output_prod (SXINT prod);
#endif /* LOG || EBUG */

#ifdef MAKE_INSIDEG
#ifndef MAKE_PROPER

/* left_corner = {(A, B) | A =>* \alpha B \beta =>* B \beta} */
/* right_corner = {(A, B) | A =>* \alpha B \beta =>* \alpha B} */
/* On en profite pour regarder si la grammaire est recursive droite et cyclique */
/* On a recursive droite en utilisant right_corner avant la fermeture reflexive */
/* Elle ne peut etre cyclique que si la grammaire inputG l'est et si un terminal est a la fois recursif gauche et droit */

static void LC_construction (struct insideG *insideG_ptr)
{
  SXINT 	  prod, item, bot_item, A, B, X, bot, top, lim, i;
  SXBA	  line, init_line, line_A;

  insideG_ptr->left_corner = sxbm_calloc (insideG_ptr->maxnt+1, insideG_ptr->maxnt+1);
  insideG_ptr->nt2item_set = sxbm_calloc (insideG_ptr->maxnt+1, insideG_ptr->maxitem+1);

  for (A = 1; A <= insideG_ptr->maxnt; A++) {
    line_A = insideG_ptr->left_corner [A];
    bot = insideG_ptr->npg [A];
    top = insideG_ptr->npg [A+1];

    while (bot < top) {
      prod = insideG_ptr->numpg [bot++];
      bot_item = item = insideG_ptr->prolon [prod];

      while ((B = insideG_ptr->lispro [item++]) > 0) {
	SXBA_1_bit (line_A, B);

	if (insideG_ptr->bvide == NULL || !SXBA_bit_is_set (insideG_ptr->bvide, B))
	  break;
      }
    }
  }

 fermer2 (insideG_ptr->left_corner, insideG_ptr->maxnt+1);

  /* La fermeture fermer2 est considerablement +rapide que la procedure fermer de la
     librairie SYNTAX sur un exemple ou l'on a S => A_1 ... => A_10000
  fermer (insideG_ptr->left_corner, insideG_ptr->maxnt+1);
  */

  /* Fermeture reflexive et calcul de left_recursive_set */
  for (A = 1; A <= insideG_ptr->maxnt; A++) {
    line = insideG_ptr->left_corner [A];

    if (SXBA_bit_is_set (line, A)) {
      if (insideG_ptr->left_recursive_set == NULL)
	insideG_ptr->left_recursive_set = sxba_calloc (insideG_ptr->maxnt+1);

      SXBA_1_bit (insideG_ptr->left_recursive_set, A);
    }
      
    SXBA_1_bit (insideG_ptr->left_corner [A], A);
  }

  /* On remplit nt2item_set [A] = {B -> \alpha . \beta | B left_corner* A et \alpha =>* \epsilon} */
  for (A = 1; A <= insideG_ptr->maxnt; A++) {
    line = insideG_ptr->left_corner [A];
    init_line = insideG_ptr->nt2item_set [A];

    B = 0;

    while ((B = sxba_scan (line, B)) > 0) {
      for (lim = insideG_ptr->npg [B+1], i = insideG_ptr->npg [B]; i < lim; i++) {
	item = insideG_ptr->prolon [insideG_ptr->numpg [i]];

	do {
	  SXBA_1_bit (init_line, item);
	  X = insideG_ptr->lispro [item++];
	} while (X > 0 && insideG_ptr->bvide && SXBA_bit_is_set (insideG_ptr->bvide, X));
      }
    }
  }
}


void RC_construction ()
{
  SXINT 	  prod, item, bot_item, A, B, X, bot, top, lim, i;
  SXBA	  line, init_line, line_A;

  if (spf.insideG.right_corner == NULL) {
    /* prudence */
    spf.insideG.right_corner = sxbm_calloc (spf.insideG.maxnt+1, spf.insideG.maxnt+1);
    spf.insideG.nt2rc_item_set = sxbm_calloc (spf.insideG.maxnt+1, spf.insideG.maxitem+1);

    for (A = 1; A <= spf.insideG.maxnt; A++) {
      line_A = spf.insideG.right_corner [A];
      bot = spf.insideG.npg [A];
      top = spf.insideG.npg [A+1];

      while (bot < top) {
	prod = spf.insideG.numpg [bot++];
	bot_item = spf.insideG.prolon [prod];
	item = spf.insideG.prolon [prod+1]-1;

	while (item > bot_item) {
	  if ((B = spf.insideG.lispro [--item]) > 0) {
	    SXBA_1_bit (line_A, B);

	    if (spf.insideG.bvide == NULL || !SXBA_bit_is_set (spf.insideG.bvide, B))
	      break;
	  }
	  else
	    break;
	}
      }
    }

    fermer2 (spf.insideG.right_corner, spf.insideG.maxnt+1);

    /* La fermeture fermer2 est considerablement +rapide que la procedure fermer de la
       librairie SYNTAX sur un exemple ou l'on a S => A_1 ... => A_10000
       fermer (spf.insideG.left_corner, spf.insideG.maxnt+1);
    */

    /* Fermeture reflexive et calcul de right_recursive_set */
    for (A = 1; A <= spf.insideG.maxnt; A++) {
      line = spf.insideG.right_corner [A];

      if (SXBA_bit_is_set (line, A)) {
	if (spf.insideG.right_recursive_set == NULL)
	  spf.insideG.right_recursive_set = sxba_calloc (spf.insideG.maxnt+1);
	
	SXBA_1_bit (spf.insideG.right_recursive_set, A);
      }
      
      SXBA_1_bit (spf.insideG.right_corner [A], A);
    }

    /* On remplit nt2rc_item_set [A] = {B -> \alpha . \beta | B right_corner A et \beta =>* \epsilon} */
    for (A = 1; A <= spf.insideG.maxnt; A++) {
      line = spf.insideG.right_corner [A];
      init_line = spf.insideG.nt2rc_item_set [A];

      B = 0;

      while ((B = sxba_scan (line, B)) > 0) {
	for (lim = spf.insideG.npg [B+1], i = spf.insideG.npg [B]; i < lim; i++) {
	  prod = spf.insideG.numpg [i];
	  bot = spf.insideG.prolon [prod];
	  item = spf.insideG.prolon [prod+1]-1;

	  do {
	    SXBA_1_bit (init_line, item);
	  } while (spf.insideG.bvide && item > bot && (X = spf.insideG.lispro [--item]) > 0 && SXBA_bit_is_set (spf.insideG.bvide, X));
	}
      }
    }
  }
}

/* remplit spf.insideG.is_cycle */

void cycle_detection ()
{  
  SXINT  nbnt, nbv, nblr, A, bot, top, prod, bot_item, item, X;
  SXBA *cyclic, lr_recursive_set;

  lr_recursive_set = sxba_calloc (spf.insideG.maxnt+1);

  if (sxba_2op (lr_recursive_set, SXBA_OP_COPY, spf.insideG.left_recursive_set, SXBA_OP_AND, spf.insideG.right_recursive_set)) {
    /* lr_recursive_set contient les nt qui sont a la fois recursifs gauches et droits, ce sont les candidats pour les cycles */
    cyclic = sxbm_calloc (spf.insideG.maxnt+1, spf.insideG.maxnt+1);

    A = 0;

    while (!spf.insideG.is_cycle && (A = sxba_scan (lr_recursive_set, A)) > 0) {
      bot = spf.insideG.npg [A];
      top = spf.insideG.npg [A+1];

      while (!spf.insideG.is_cycle && bot < top) {
	prod = spf.insideG.numpg [bot];
	bot_item = item = spf.insideG.prolon [prod];
	X = spf.insideG.lispro [item];

	/* On regarde si c'est une bonne rhs */
	if (X > 0) {
	  nbnt = nbv = nblr = 0;

	  do {
	    nbnt++;

	    if (spf.insideG.bvide && SXBA_bit_is_set (spf.insideG.bvide, X))
	      nbv++;

	    if (SXBA_bit_is_set (lr_recursive_set, X))
	      nblr++;

	  } while ((X = spf.insideG.lispro [++item]) > 0);

	  if (X == 0 && nblr > 0 && nbv >= nbnt-1) {
	    /* C'est une bonne rhs */
	    while ((X = spf.insideG.lispro [bot_item++]) > 0) {
	      if (SXBA_bit_is_set (lr_recursive_set, X)) {
		if (nbv == nbnt /* tout derive ds le vide */
		    || (spf.insideG.bvide == NULL || !SXBA_bit_is_set (spf.insideG.bvide, X)) /* C'est celle qui ne derive pas ds le vide */) {
		  if (X == A) {
		    spf.insideG.is_cycle = true;
		    break;
		  }

		  SXBA_1_bit (cyclic [A], X);
		}
	      }
	    }
	  }
	}

	bot++;
      }
    }

    if (!spf.insideG.is_cycle) {
      /* On peut peut etre faire le fermer manuel de LC_construction!! */
      fermer2 (cyclic, spf.insideG.maxnt+1);

      A = 0;
      
      while ((A = sxba_scan (lr_recursive_set, A)) > 0) {
	if (SXBA_bit_is_set (cyclic [A], A)) {
	  spf.insideG.is_cycle = true;
	  break;
	}
      }
    }

    sxbm_free (cyclic);
  }

  sxfree (lr_recursive_set);
}




/* Multiplication de matrices booleennes */
/* P U= Q x R */
/* P [0:l, 0:n], Q [0:l, 0:m], R [0:m, 0:n] */

static SXBA *bmm (SXBA *P, SXBA *Q, SXBA *R, SXINT l /*, SXINT m, SXINT n */)
{
  SXINT	i, j;
  SXBA	P_line, Q_line;

  for (i = 0; i < l; i++) {
    P_line = P [i];
    Q_line = Q [i];

    j = -1;

    while ((j = sxba_scan (Q_line, j)) >= 0)
      sxba_or (P_line, R [j]);
  }

  return P;
}

/* On calcule D (Debutant) : D = {(A, t) | A -> \alpha t \beta et \alpha =>* \epsilon}
   FIRST = LC* D avec LC = Left-Corner */

static void first_construction (SXBA *FIRST)
{
  SXINT  prod, item, A, X;
  SXBA *D;

  D = sxbm_calloc (spf.insideG.maxnt+1, SXEOF+1);

  for (prod = 0; prod <= spf.insideG.maxprod; prod++) {
    A = spf.insideG.lhs [prod];
    item = spf.insideG.prolon [prod];

    while ((X = spf.insideG.lispro [item++]) != 0) {
      if (X < 0) {
	X = -X;
	SXBA_1_bit (D [A], X);
	break;
      }

      if (spf.insideG.bvide == NULL || !SXBA_bit_is_set (spf.insideG.bvide, X))
	break;
    }
  }

  bmm (FIRST, spf.insideG.left_corner, D, spf.insideG.maxnt);
  sxbm_free (D);
}

static void follow_construction (SXBA *FOLLOW, SXBA *FIRST)
{
  /* On calcule RC~ (Transposee de Right-Corner) : RC~ = {(B, A) | A -> \alpha B \beta et \beta =>* \epsilon}
     On calcule NNV (NT Voisin) : NNV = {(B, C) | U -> \alpha B \beta C \gamma et \beta =>* \epsilon}
     On calcule NTV (T Voisin) : NTV = {(A, t) | U -> \alpha B \beta t \gamma et \beta =>* \epsilon}
     FOLLOW = RC* NNV FIRST U RC* NTV */

  SXINT	prod, A, B, C, t, item, next_item, nt;
  SXBA	*RC_tilde, *NNV, *NTV, line;

  if (spf.insideG.right_corner == NULL)
    RC_construction ();

  RC_tilde = sxbm_calloc (spf.insideG.maxnt+1, spf.insideG.maxnt+1);

  for (A = 0; A <= spf.insideG.maxnt; A++) {
    line = spf.insideG.right_corner [A];

    B = -1;

    while ((B = sxba_scan (line, B)) >= 0) {
      SXBA_1_bit (RC_tilde [B], A);
    }
  }

  NNV = sxbm_calloc (spf.insideG.maxnt+1, spf.insideG.maxnt+1);
  NTV = sxbm_calloc (spf.insideG.maxnt+1, SXEOF+1);

  for (prod = 0; prod <= spf.insideG.maxprod; prod++) {
    A = spf.insideG.lhs [prod];
    item = spf.insideG.prolon [prod];

    while ((B = spf.insideG.lispro [item++]) != 0) {
      if (B > 0) {
	next_item = item;

	while ((C = spf.insideG.lispro [next_item++]) != 0) {
	  if (C > 0) {
	    SXBA_1_bit (NNV [B], C);

	    if (spf.insideG.bvide == NULL || !SXBA_bit_is_set (spf.insideG.bvide, C))
	      break;
	  }
	  else {
	    t = -C;
	    SXBA_1_bit (NTV [B], t);
	    break;
	  }
	}
      }
    }
  }

  bmm (NTV, NNV, FIRST, spf.insideG.maxnt /*, spf.insideG.maxnt, -spf.insideG.maxt */); /* NTV U= NNV FIRST */
  bmm (FOLLOW, RC_tilde, NTV, spf.insideG.maxnt /*, spf.insideG.maxnt, -spf.insideG.maxt */); /* FOLLOW = RC_tilde* NTV */

  sxbm_free (RC_tilde);
  sxbm_free (NNV);
  sxbm_free (NTV);
}


/* Calcul de item2t_set
   item2t_set [A -> \alpha . \beta] = FIRST (\beta) FOLLOW (A).
*/

static void item2t_set_construction (SXBA *item2t_set, SXBA *FOLLOW, SXBA *FIRST)
{
  SXINT	prod, A, B, left, right, item; 
  SXBA	line;

  for (prod = 0; prod <= spf.insideG.maxprod; prod++) {
    A = spf.insideG.lhs [prod];
    right = prod == 0 ? 2 : spf.insideG.prolon [prod+1]-1;
    left = spf.insideG.prolon [prod];

    for (item = right; item >= left; item--) {
      B = spf.insideG.lispro [item];
      line = item2t_set [item];

      if (B == 0)
	sxba_or (line, FOLLOW [A]);
      else {
	if (B < 0) {
	  B = -B;
	  SXBA_1_bit (item2t_set [item], B);
	}
	else {
	  sxba_or (item2t_set [item], FIRST [B]);

	  if (spf.insideG.bvide && SXBA_bit_is_set (spf.insideG.bvide, B))
	    sxba_or (item2t_set [item], item2t_set [item+1]);
	}
      }
    }
  }

  SXBA_1_bit (item2t_set [0], SXEOF); /* eof est un look-ahead de S' -> . eof S eof */
  SXBA_1_bit (item2t_set [3], SXEOF); /* eof est un look-ahead de S' -> eof S eof . */
}


/* La rcvr d'Earley appelera cette proc si besoin est */
/* t2item_set [t] = {A -> \alpha . \beta | t \in First1(\beta Follow1(A))} */

void fill_t2item_set ()
{
  SXINT  item, t;
  SXBA line;
  SXBA *first, *follow, *item2t_set;

  first = sxbm_calloc (spf.insideG.maxnt+1, SXEOF+1);
  follow = sxbm_calloc (spf.insideG.maxnt+1, SXEOF+1);
  item2t_set = sxbm_calloc (spf.insideG.maxitem+1, -spf.insideG.maxt+1);
  spf.insideG.t2item_set = sxbm_calloc (-spf.insideG.maxt+1, spf.insideG.maxitem+1);

  first_construction (first),
  follow_construction (follow, first);
  /* item2t_set [A -> \alpha . \beta] = FIRST (\beta) FOLLOW (A). */
  item2t_set_construction (item2t_set, follow, first);

  for (item = 0; item <= spf.insideG.maxitem; item++) {
    line = item2t_set [item];
    t = 0;

    while ((t = sxba_scan (line, t)) > 0)
      SXBA_1_bit (spf.insideG.t2item_set [t], item);
  }

  /* Le terminal Any, de code 0 regroupe tous les terminaux sauf eof */
  for (t = SXEOF-1; t > 0; t--)
    sxba_or (spf.insideG.t2item_set [0], spf.insideG.t2item_set [t]);

  sxbm_free (first);
  sxbm_free (follow);
  sxbm_free (item2t_set);
}


static void call_fill_nt2min_gen_lgth ()
{
  /* nt2t_set [A] = {t | A =>+ x t y} */
  SXINT  item, prod, A, B, X, bot, top, cur_min, cur_nt_min, cur_prod_min, bot2, top2;
  SXBA nt_set, prod_set;
  SXINT  *nt2cur_min, *prod2cur_min;

  nt_set = sxba_calloc (spf.insideG.maxnt+1);
  prod2cur_min = (SXINT *) sxcalloc (spf.insideG.maxprod+1, sizeof (SXINT));
  prod_set = sxba_calloc (spf.insideG.maxprod+1);


  for (A = 1; A <= spf.insideG.maxnt; A++) {
    if (spf.insideG.bvide == NULL || !SXBA_bit_is_set (spf.insideG.bvide, A)) {
      /* On parcourt les nt_prods */
      bot = spf.insideG.npg [A];
      top = spf.insideG.npg [A+1];
      cur_nt_min = -1;

      while (bot < top) {
	prod = spf.insideG.numpg [bot];
	cur_prod_min = 0;
	item = spf.insideG.prolon [prod];

	while (X = spf.insideG.lispro [item]) {
	  if (X < 0 || spf.insideG.bvide == NULL || !SXBA_bit_is_set (spf.insideG.bvide, X))
	    cur_prod_min++;

	  item++;
	}

	prod2cur_min [prod] = cur_prod_min;

	if (cur_nt_min == -1 || cur_prod_min < cur_nt_min)
	  cur_nt_min = cur_prod_min;

	bot++;
      }
	      
      if (cur_nt_min > 0) {
	SXBA_1_bit (nt_set, A); /* taille a change' */
	spf.insideG.nt2min_gen_lgth [A] = cur_nt_min; /* au moins */
      }
    }
  }

  /* nt_set contient les nt dont la taille min a change */
  while ((B = sxba_scan_reset (nt_set, 0)) > 0) {
    do {
      bot = spf.insideG.npd [B];

      while (item = spf.insideG.numpd [bot]) {
	prod = spf.insideG.prolis [item];
	/* changement qui impacte prod ... */
	SXBA_1_bit (prod_set, prod);
	bot++;
      }
    } while ((B = sxba_scan_reset (nt_set, B)) > 0);

    prod = 0;

    while ((prod = sxba_scan_reset (prod_set, prod)) > 0) {
      item = spf.insideG.prolon [prod];
      cur_min = 0;

      while (X = spf.insideG.lispro [item]) {
	if (X < 0)
	  cur_min++;
	else
	  cur_min += spf.insideG.nt2min_gen_lgth [X];

	item++;
      }

      if (cur_min > prod2cur_min [prod]) {
	/* nelle taille de prod qui peut impacter A */
	prod2cur_min [prod] = cur_min;
	A = spf.insideG.lhs [prod];
	SXBA_1_bit (nt_set, A);
      }
    }

    A = 0;

    while ((A = sxba_scan_reset (nt_set, A)) > 0) {
      bot = spf.insideG.npg [A];
      top = spf.insideG.npg [A+1];

      cur_min = -1;

      while (bot < top) {
	prod = spf.insideG.numpg [bot];

	if (cur_min == -1 || prod2cur_min [prod] < cur_min)
	  cur_min = prod2cur_min [prod];

	bot++;
      }

      if (cur_min > spf.insideG.nt2min_gen_lgth [A]) {
	/* effectivement le min de A a change' */
	spf.insideG.nt2min_gen_lgth [A] = cur_min;
	SXBA_1_bit (nt_set, A);
      }
    }
  }

  spf.insideG.nt2min_gen_lgth [0] = spf.insideG.nt2min_gen_lgth [1]+2 /* $ $ */;

  sxfree (nt_set);
  sxfree (prod2cur_min);
  sxfree (prod_set);
}

/* La rcvr d'Earley appelera cette proc si besoin est */
/* nt2min_gen_lgth [A] = l, l=min(x), A =>+ x et l=|x| */

void fill_nt2min_gen_lgth ()
{
  if (spf.insideG.nt2min_gen_lgth == NULL) {
    /* Pas deja fait */
    spf.insideG.nt2min_gen_lgth = (SXINT *) sxcalloc (spf.insideG.maxnt+1, sizeof (SXINT));

    call_fill_nt2min_gen_lgth ();
  }
}


/* La nouvelle grammaire reduite vient d'etre mise ds insideG, 
   Les codes des terminaux ont change
   On se souvient des anciens ds idag.orig_t_stack (si c'est le 1er coup)
   ... et on met les nouveaux ds idag.t_stack */

static void idag_t_stack_recode (SXINT *old_t2new_t)
{
  SXINT   pq, y, lgth, new_t, max_pq;
  SXINT   *source_t_stack;
  bool multiple;

  if (idag.orig_t_stack == NULL) {
    multiple = false;
    idag.orig_t_stack = idag.t_stack;
    idag.orig_source_set = idag.source_set;
  }
  else {
    /* Appel multiple de idag_t_stack_recode */
    multiple = true;
    sxfree (idag.source_set), idag.source_set = NULL;
  }

  source_t_stack = idag.t_stack;
  DALLOC_STACK (idag.t_stack, DTOP (idag.orig_t_stack));
  idag.source_set = sxba_calloc (SXEOF+1);

  max_pq = idag.p2pq_hd [idag.final_state];

  for (pq = 1; pq < max_pq; pq++) {
    y = idag.pq2t_stack_hd [pq];
    lgth = source_t_stack [y];
    DSPUSH (idag.t_stack, lgth);

    while (lgth-- > 0) {
      new_t = old_t2new_t [source_t_stack [++y]];

      if (new_t < 0)
	/* Au cas ou ... */
	new_t = -new_t;

      DSPUSH (idag.t_stack, new_t);
      SXBA_1_bit (idag.source_set, new_t);
    }
  }

  if (multiple)
    /* On se debarasse de l'intermediaire */
    DFREE_STACK (source_t_stack);
}

void idag_t_stack_restore ()
{
  DFREE_STACK (idag.t_stack);
  idag.t_stack = idag.orig_t_stack, idag.orig_t_stack = NULL;
  sxfree (idag.source_set);
  idag.source_set = idag.orig_source_set, idag.orig_source_set = NULL;
}

#endif /* !MAKE_PROPER */
#endif /* MAKE_INSIDEG */


#ifndef MAKE_PROPER
/* Ds ce cas c'est check_multiple_anchor_occur_pre () qui a ete appele' */
#  ifndef MULTI_ANCHOR

static bool check_multiple_anchor_occur_post (SXINT *multi_anchor_erased_prod_stack)
{
  SXINT nb, item, prod, bot, top, t;

#    if LOG
  fputs ("****** Entering the multiple anchor occurrence post test ******\n", stdout);
#    endif /* LOG */

  nb = 0;

  while (!IS_EMPTY (multi_anchor_erased_prod_stack)) {
    nb++;
    prod = DPOP (multi_anchor_erased_prod_stack);
    bot = PROLON (prod);
    top = PROLON (prod+1);
    sxba_clear_range (basic_item_set, bot, top-bot); /* On elimine prod */
#    if LLLOG
    t = -LISPRO (item);
#      ifdef MAKE_INSIDEG
    t = spf.insideG.t2init_t [t];
    prod = spf.insideG.prod2init_prod [prod];
#      endif /* MAKE_INSIDEG */

    printf ("The multiple anchor occurrence post test fails for \"%s\" in production ", spf.inputG.tstring [t]);
    output_prod (prod);
#    endif /* LLLOG */
  }

#    if MEASURES || LOG
  printf ("%sThe multiple anchor occurrence post test erases %i productions.\n",
#      if MEASURES
	  "MEASURES: "
#      else /* LOG */
	  ""
#      endif /* LOG */
	  , nb
	  );
#    endif /* MEASURES || LOG */

#    if LOG
  fputs ("****** Leaving the multiple anchor occurrence post test ******\n", stdout);
#    endif /* LOG */

  return nb > 0;
}
#  endif /* ifndef MULTI_ANCHOR */
#endif /* ifndef MAKE_PROPER */




#ifdef is_check_rhs_sequences

static void crs_dag_or_nodag_source_processing ()
{  
  SXINT  t1, t2;
  SXBA *store_mlstn2la_tset, t_set;

#if LOG
      fputs ("****** Entering crs_dag_or_nodag_source_processing () ******\n", stdout);
#endif /* LOG */

  /* On construit t2suffix_t_set */
  t2suffix_t_set = sxbm_calloc (SXEOF+1, SXEOF+1);
  mlstn2suffix_source_set = sxbm_calloc (idag.final_state+1, SXEOF+1);

  /* On construit t2la_t_set en utilisant mlstn2la_tset */
  store_mlstn2la_tset = mlstn2la_tset; /* Prudence */
  mlstn2la_tset = sxbm_calloc (1+n+1+2, SXEOF+1);
  t2la_t_set = sxbm_calloc (SXEOF+1, SXEOF+1);

  /* On remplit t2suffix_t_set (en utilisant mlstn2suffix_source_set) */
  /* mlstn2la_tset sera rempli + tard !! */
  idag_source_processing (SXEOF, t2suffix_t_set, mlstn2suffix_source_set, mlstn2la_tset, t2la_t_set);

  /* t2la_t_set [SXEOF] est l'ensemble des terminaux debutant le source !! */
  sxba_copy (t2la_t_set [SXEOF], mlstn2la_tset [1]);

  /* t2suffix_t_set [SXEOF] est l'ensemble des terminaux du source !! */
  sxba_copy (t2suffix_t_set [SXEOF], mlstn2suffix_source_set [1]);

  sxbm_free (mlstn2la_tset);
  mlstn2la_tset = store_mlstn2la_tset;

#if is_epsilon
  sxbm_free (mlstn2suffix_source_set), mlstn2suffix_source_set = NULL;
#endif /* is_epsilon */

#if LOG
  {
    SXINT  cur_t, t, old_t, old_t2;
    SXBA suffix_t_set, la_t_set;
     
    fputs ("\n", stdout);

    cur_t = 0;

    while ((cur_t = sxba_scan (idag.source_set, cur_t)) > 0 /* && cur_t < SXEOF */) {
#ifdef MAKE_INSIDEG
      old_t = spf.insideG.t2init_t [cur_t];
#else /* ifndef MAKE_INSIDEG */
      old_t = cur_t;
#endif /* ifndef MAKE_INSIDEG */

      suffix_t_set = t2suffix_t_set [cur_t];

      printf ("suffix_t_set (\"%s\") = {", spf.inputG.tstring [old_t]);

      t = sxba_scan (suffix_t_set, 0);

      if (t > 0) {
#ifdef MAKE_INSIDEG
	old_t2 = spf.insideG.t2init_t [t];
#else /* ifndef MAKE_INSIDEG */
	old_t2 = t;
#endif /* ifndef MAKE_INSIDEG */
	printf ("\"%s\"", spf.inputG.tstring [old_t2]);
	  
	while ((t = sxba_scan (suffix_t_set, t)) > 0) {
#ifdef MAKE_INSIDEG
	  old_t2 = spf.insideG.t2init_t [t];
#else /* ifndef MAKE_INSIDEG */
	  old_t2 = t;
#endif /* ifndef MAKE_INSIDEG */
	  printf (", \"%s\"", spf.inputG.tstring [old_t2]);
	}
      }

      fputs ("}\n", stdout);

      la_t_set = t2la_t_set [cur_t];
      printf ("look_ahead_t_set (\"%s\") = {", spf.inputG.tstring [old_t]);

      t = sxba_scan (la_t_set, 0);

      if (t > 0) {
#ifdef MAKE_INSIDEG
	old_t2 = spf.insideG.t2init_t [t];
#else /* ifndef MAKE_INSIDEG */
	old_t2 = t;
#endif /* ifndef MAKE_INSIDEG */
	printf ("\"%s\"", spf.inputG.tstring [old_t2]);
	  
	while ((t = sxba_scan (la_t_set, t)) > 0) {
#ifdef MAKE_INSIDEG
	  old_t2 = spf.insideG.t2init_t [t];
#else /* ifndef MAKE_INSIDEG */
	  old_t2 = t;
#endif /* ifndef MAKE_INSIDEG */
	  printf (", \"%s\"", spf.inputG.tstring [old_t2]);
	}
      }

      fputs ("}\n", stdout);
    }
     
    fputs ("\n", stdout);
  }
#endif /* LOG */

#if LOG
  fputs ("****** Leaving crs_dag_or_nodag_source_processing () ******\n", stdout);
#endif /* LOG */
}



/* On verifie l'ordre des ancres multiples */

static bool check_multiple_anchor_sequence (SXINT *multi_anchor_prod_stack)
{
  SXINT     prev_t, prev_item, nb, bot_item, X, old_prev_t, prod, item, t, new_prod_nb;
  bool is_axiom_kept;

#    if LOG
  fputs ("****** Entering the multiple anchor sequence test ******\n", stdout);
#    endif /* LOG */

  nb = 0;

  while (!IS_EMPTY (multi_anchor_prod_stack)) {
    prod = DPOP (multi_anchor_prod_stack);
    prev_t = 0;
    prev_item = 0;
    bot_item = item = PROLON (prod);

    while ((X = LISPRO (item)) != 0) {
      if (X < 0) {
	t = -X;

	if (prev_t) {
	  if (prev_item == item-1) {
	    if (!SXBA_bit_is_set (t2la_t_set [prev_t], t))
	      break;
	  }
	  else {
	    if (!SXBA_bit_is_set (t2suffix_t_set [prev_t], t))
	      break;
	  }

	  prev_t = t;
	  prev_item = item;
	}
      }

      item++;
    }

    if (X) {
      /* On elimine prod */
      sxba_clear_range (basic_item_set, bot_item, PROLON (prod+1)-bot_item); /* On elimine prod */
      nb++;
#    if LOG
#      ifdef MAKE_INSIDEG
      t = spf.insideG.t2init_t [t];
      old_prev_t = spf.insideG.t2init_t [prev_t];
      prod = spf.insideG.prod2init_prod [prod];
#      else /* MAKE_INSIDEG */
      old_prev_t = prev_t;
#      endif /* MAKE_INSIDEG */

      printf ("The multiple anchor sequence test fails for \"%s\" and \"%s\" in production ", spf.inputG.tstring [old_prev_t], spf.inputG.tstring [t]);
      output_prod (prod);
#    endif /* LOG */
    }
  }

#    if MEASURES || LOG
  printf ("%sThe multiple anchor sequence test erases %i productions.\n",
#      if MEASURES
	  "MEASURES: "
#      else /* LOG */
	  ""
#      endif /* LOG */
	  , nb);
#    endif /* MEASURES || LOG */

#    if LOG
  fputs ("****** Leaving the multiple anchor sequence test ******\n", stdout);
#    endif /* LOG */

  return nb > 0; /* Il a supprime' des prods */
}


static  SXINT     axiom_prod_nb;
static  SXBA    *nt2first_packed, *nt2last_packed;
static  SXBA    w_t_set, F_set;
static  SXBA    *nt2look_ahead_set, *nt2first_set;
static  SXBA    *nt2cl_nt_set, *nt2cr_nt_set; /* symetriques de left_corner et right_corner */
static  SXBA    left_item_set;
static  SXBA    right_item_set;
static  SXBA    *nt2suffix_set;
static  SXBA    *nt2anla_t_set; /* AncestorNeighbourLookAhead */
static  SXBA    w2_t_set, w2_nt_set;
static  SXINT     *titem2packed_titem, *packed_titem2titem;
static  SXINT     packed_titem_nb;
#if LOG || EBUG
static  SXBA    first_or_last_lhs_nt_set;
#endif /* LOG || EBUG */

#ifdef def_segment
static SXINT      *crs_segment, *crs_segment_list;
static SXBA     crs_valid_segment_set;
#endif /* def_segment */
  

/* Remplit t_set et nt_set avec les voisins gauches de A */

static void left_neighbour (SXINT A, SXBA t_set, SXBA nt_set)
{
  SXINT bot, item, X;

  bot = NPD (A);

  while (item = NUMPD (bot++)) {
    if (SXBA_bit_is_set (basic_item_set, item)) {
#if is_epsilon
      while (--item >= 0 /* La super-regle n'a pas 0 a l'item -1 !! */ && (X = LISPRO (item))) {
	if (X < 0) {
	  /* Si X == eof (celui du debut), t2la_t_set [eof] est l'ensemble des terminaux debutant le source */
	  X = -X;
	  SXBA_1_bit (t_set, X);
	  break;
	}
	else {
	  SXBA_1_bit (nt_set, X);

	  if (EMPTY_SET && !SXBA_bit_is_set (EMPTY_SET, X))
	    /* X \not =>+ vide*/
	    break;
	}
      }
#else /* !is_epsilon */
      if (--item >= 0 /* La super-regle n'a pas 0 a l'item -1 !! */ && (X = LISPRO (item))) {
	if (X < 0) {
	  /* Si X == eof (celui du debut), t2la_t_set [eof] est l'ensemble des terminaux debutant le source */
	  X = -X;
	  SXBA_1_bit (t_set, X);
	}
	else {
	  SXBA_1_bit (nt_set, X);;
	}
      }
#endif /* !is_epsilon */
    }
  }
}

/* Remplit t_set et nt_set avec les voisins droits de A */

static void right_neighbour (SXINT A, SXBA t_set, SXBA nt_set)
{
  SXINT bot, item, X;

  bot = NPD (A);

  while (item = NUMPD (bot++)) {
    if (SXBA_bit_is_set (basic_item_set, item)) {
#if is_epsilon
      while ((X = LISPRO (++item)) != 0) {
	if (X < 0) {
	  /* Si X == eof (celui de la fin), t2la_t_set [eof] est l'ensemble des terminaux debutant le source */
	  X = -X;
	  SXBA_1_bit (t_set, X);
	  break;
	}
	else {
	  SXBA_1_bit (nt_set, X);

	  if (EMPTY_SET && !SXBA_bit_is_set (EMPTY_SET, X))
	    /* X \not =>+ vide*/
	    break;
	}
      }
#else /* !is_epsilon */
      if ((X = LISPRO (item+1)) != 0) {
	if (X < 0) {
	  /* Si X == eof (celui du debut), t2la_t_set [eof] est l'ensemble des terminaux debutant le source */
	  X = -X;
	  SXBA_1_bit (t_set, X);
	}
	else {
	  SXBA_1_bit (nt_set, X);;
	}
      }
#endif /* !is_epsilon */
    }
  }
}


static void crs_erase_prod (SXINT prod
#if LLOG
		, char *string, SXINT A, SXINT B
#endif /* LLOG */
		)
{
  SXINT bot_item, top_item, item;

  bot_item = PROLON (prod);
  top_item = PROLON (prod+1);
		    
  sxba_clear_range (basic_item_set, bot_item, top_item-bot_item);
  /* On nettoie aussi left et right-item_set */
  sxba_scan_reset (left_item_set, bot_item-1);
  item = sxba_1_rlscan (right_item_set, top_item-1);
  SXBA_0_bit (right_item_set, item);

  if (LHS (prod) == 1)
    axiom_prod_nb--;

#if LLOG
  {
    SXINT old_A, old_B, old_prod;

#ifdef MAKE_INSIDEG
    old_A = (A > 0) ? spf.insideG.nt2init_nt [A] : -spf.insideG.t2init_t [-A];
    old_B = (B > 0) ? spf.insideG.nt2init_nt [B] : -spf.insideG.t2init_t [-B];
#else /* !MAKE_INSIDEG */
    old_A = A;
    old_B = B;
#endif /* MAKE_INSIDEG */

    if (old_A > 0)
      printf ("%s of <%s> and ", string, spf.inputG.ntstring [old_A]);
    else
      printf ("%s of \"%s\" and ", string, spf.inputG.tstring [-old_A]);

    if (old_B > 0)
      printf ("<%s>", spf.inputG.ntstring [old_B]);
    else
      printf ("\"%s\"", spf.inputG.tstring [-old_B]);

    fputs (" w.r.t. the source text erases ", stdout);

#ifdef MAKE_INSIDEG
    old_prod = spf.insideG.prod2init_prod [prod];
#else /* !MAKE_INSIDEG */
    old_prod = prod;
#endif /* !MAKE_INSIDEG */

    output_prod (old_prod);
  }
#endif /* LLOG */
}

/* Soit p : A -> \alpha X1 \beta X2 \gamma une prod.
   Pour tout symbole X de N U T, on definit gen(X) = {t | X =>* x t y, t \in T}
   Si pour tout (t1, t2) \in gen(X1) X gen(X2), on a t2 \not \in t2suffix_t_set [t1] (aucun des terminaux
   generes par X2 ne suit, dans le source, t1) alors la production p est eliminee
*/
/* Pour chaque nt A on calcule nt2first[A] et nt2last[A]
   nt2first[A] = {t1| A =>+ t1 x t2}
   nt2last[A] = {t2| A =>+ t1 x t2}
*/
/* basic_item_set et basic_nt_set sont des SXBA globaux (les analyseurs comme earley peuvent y acceder) */
/* Elimine ds basic_item_set les productions eliminees par ce test */
/* Utilise basic_nt_set (mais n'y touche pas, c'est un make_a_next_reduced_grammar() ulterieur qui le repositionnera) */
/* La sous-grammaire selectionnee par basic_item_set est reduite */
/* l'ordre des ancres multiples a deja ete verifie' */
/* Retourne le nombre de prod eliminees */
/* Nelle version, insideG a (peut-etre) ete construite */
/* Nelle nelle version, first et last referencent des titems packe's qui ne sont construits que la 1ere fois*/

static SXINT check_rhs_sequences (bool *is_axiom_kept)
{
  SXINT             erased_prod_nb = 0; 
  bool         succeeded;

#if LOG
  fputs ("\n*** Entering check_rhs_sequences ***\n", stdout);
#endif /* LOG */

  if (nt2first_packed == NULL) {
    /* calcul de first et last mais le resultat est un ensemble d'items terminaux "packes" */
    SXINT     item, prod, A, X, top_item, bot, old_A, bot_item, left_item, init_left_item;
    SXINT     packed_titem, cur_packed_titem;
    SXINT     *seg_stack;
    SXINT     *nt_first_stack, *nt_last_stack;
    SXBA    nt_first_packed, nt_last_packed, item_first_set, item_last_set, nt_first_set, nt_last_set;
    SXBA    working_item_set;
#if EBUG
    SXBA    non_empty_first_set, non_empty_last_set;
#endif /* EBUG */

    w_t_set = sxba_calloc (SXEOF+1);
    F_set = sxba_calloc (SXEOF+1);

    working_item_set = sxba_calloc (MAXITEM+1);
    sxba_2op (working_item_set, SXBA_OP_COPY, TITEM_SET, SXBA_OP_AND, basic_item_set);
    packed_titem_nb = sxba_cardinal (working_item_set);
    sxfree (working_item_set);

    titem2packed_titem = (SXINT*) sxcalloc (MAXITEM+1, sizeof (SXINT));
    packed_titem2titem = (SXINT*) sxalloc (packed_titem_nb+1, sizeof (SXINT));
    nt2first_packed = sxbm_calloc (MAXNT+1, packed_titem_nb+1);
    nt2last_packed = sxbm_calloc (MAXNT+1, packed_titem_nb+1);

    nt_first_stack = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), nt_first_stack [0] = 0;
    nt_first_set = sxba_calloc (MAXNT+1);
    nt_last_stack = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), nt_last_stack [0] = 0;
    nt_last_set = sxba_calloc (MAXNT+1);
    item_first_set = sxba_calloc (MAXITEM+1);
    item_last_set = sxba_calloc (MAXITEM+1);
#if EBUG
    non_empty_first_set = sxba_calloc (MAXNT+1);
    non_empty_last_set = sxba_calloc (MAXNT+1);
#endif /* EBUG */
#if LOG || EBUG
    first_or_last_lhs_nt_set = sxba_calloc (MAXNT+1);
#endif /* LOG || EBUG */

    /* la symetrique de left_corner */
    nt2cl_nt_set = sxbm_calloc (MAXNT+1, MAXNT+1);
    left_item_set = sxba_calloc (MAXITEM+1);
    /* la symetrique de right_corner */
    nt2cr_nt_set = sxbm_calloc (MAXNT+1, MAXNT+1);
    right_item_set = sxba_calloc (MAXITEM+1);

    /* les look-ahead des voisins des ancetres */
    nt2anla_t_set = sxbm_calloc (MAXNT+1, SXEOF+1);
    w2_t_set = sxba_calloc (SXEOF+1);
    w2_nt_set = sxba_calloc (MAXNT+1);

#ifdef def_segment
    crs_segment = (SXINT*) sxcalloc (MAXITEM+1, sizeof (SXINT));
    crs_segment_list = (SXINT*) sxcalloc (sizeof (segment_list)/sizeof (SXINT), sizeof (SXINT));
    crs_valid_segment_set = sxba_calloc (sizeof (segment_list)/sizeof (SXINT));
#endif /* def_segment */

    /* calcul des first et last */
    /* On parcourt les prod */
    axiom_prod_nb = 0;
    packed_titem = 0;
    item = 3;

    while ((item = sxba_scan (basic_item_set, item)) > 0) {
#ifdef def_segment
      bot_item = item;
#endif /* def_segment */
      prod = PROLIS (item);
      /* Selectionnee */
      A = LHS (prod);

      if (A == 1)
	axiom_prod_nb++;

      nt_first_packed = nt2first_packed [A];
      nt_last_packed = nt2last_packed [A];
#if LOG || EBUG
      /* La sous-grammaire selectionnee par basic_item_set est reduite.  Il suffit donc de noter
	 les nt en LHS pour avoir les actifs */
      SXBA_1_bit (first_or_last_lhs_nt_set, A);
#endif /* LOG || EBUG */

      while ((X = LISPRO (item)) != 0) {
	if (X > 0) {
	  /* Toute modif sur X doit se repercuter sur A */
	  SXBA_1_bit (item_first_set, item);
	  SXBA_1_bit (nt2cl_nt_set [X], A); /* X est ds le coin gauche de A */

#if is_epsilon
	  if (EMPTY_SET && !SXBA_bit_is_set (EMPTY_SET, X))
	    /* X \not =>+ vide*/
	    break;
#else /* !is_epsilon */
	  break;
#endif /* is_epsilon */
	}
	else {
	  X = -X;

	  if ((cur_packed_titem = titem2packed_titem [item]) == 0) {
	    /* 1ere fois */
	    cur_packed_titem = titem2packed_titem [item] = ++packed_titem;
	    packed_titem2titem [packed_titem] = item;
	  }

	  if (SXBA_bit_is_reset_set (nt_first_packed, cur_packed_titem)) {
	    if (SXBA_bit_is_reset_set (nt_first_set, A)) {
#if EBUG
	      SXBA_1_bit (non_empty_first_set, A); /* non vide */
#endif /* EBUG */
	      PUSH (nt_first_stack, A);
	    }
	  }

	  break;
	}

	item++;
      }

      if (X != 0)
	SXBA_1_bit (left_item_set, item); /* item non nullable le + a gauche */

#ifdef def_segment
      left_item = (X != 0) ? item : 0;
#endif /* def_segment */
	
      item = top_item = PROLON (prod+1)-1;

      while ((X = LISPRO (--item)) != 0) {
	if (X > 0) {
	  /* Toute modif sur X doit se repercuter sur A */
	  SXBA_1_bit (item_last_set, item);
	  SXBA_1_bit (nt2cr_nt_set [X], A); /* X est ds le coin droit de A */

#if is_epsilon
	  if (EMPTY_SET && !SXBA_bit_is_set (EMPTY_SET, X))
	    /* X \not =>+ vide*/
	    break;
#else /* !is_epsilon */
	  break;
#endif /* is_epsilon */
	}
	else {
	  X = -X;

	  if ((cur_packed_titem = titem2packed_titem [item]) == 0) {
	    /* 1ere fois */
	    cur_packed_titem = titem2packed_titem [item] = ++packed_titem;
	    packed_titem2titem [packed_titem] = item;
	  }

	  if (SXBA_bit_is_reset_set (nt_last_packed, cur_packed_titem)) {
	    if (SXBA_bit_is_reset_set (nt_last_set, A)) {
#if EBUG
	      SXBA_1_bit (non_empty_last_set, A); /* non vide */
#endif /* EBUG */
	      PUSH (nt_last_stack, A);
	    }
	  }

	  break;
	}
      }

      if (X != 0)
	SXBA_1_bit (right_item_set, item); /* item non nullable le + a droite */

#ifdef def_segment
      /* On remplit crs_segment et crs_segment_list */
      if (X != 0 && left_item < item && left_item > 0) {
	/* Il y a au moins un segment dans prod ... */
	/* ... et item est la fin d'un */
	crs_segment [item] = -1;
	/* Si def_segment on a insideG */
	init_left_item = spf.inputG.prolon [PROD (prod)]+left_item-bot_item;

	do {
	  seg_stack = crs_segment_list+(crs_segment [left_item] = segment [init_left_item]);
	  PUSH (seg_stack, left_item); /* On empile l'item gauche du segment */
	  
	  while ((X = segment [++init_left_item]) == 0)
	    left_item++;

	} while (++left_item < item);
      }
#endif /* def_segment */

      item = top_item;
    }

    while (!IS_EMPTY (nt_first_stack)) {
      X = POP (nt_first_stack);
      SXBA_0_bit (nt_first_set, X);

      bot = NPD (X);

      while (item = NUMPD (bot++)) {
	if (SXBA_bit_is_set (item_first_set, item)) {
	  /* item est concerne' par les changements sur X */
	  prod = PROLIS (item);
	  A = LHS (prod);

	  /* On regarde si ces changements se repercutent sur la lhs ... */
	  if (sxba_or_with_test (nt2first_packed [A], nt2first_packed [X])) {
	    /* ... oui ... */
	    if (SXBA_bit_is_reset_set (nt_first_set, A)) {
	      /* ... et A ne se trouve plus en pile, on l'y remet */
#if EBUG
	      SXBA_1_bit (non_empty_first_set, A); /* non vide */
#endif /* EBUG */
	      PUSH (nt_first_stack, A);
	    }
	  }
	}
      }
    }

    while (!IS_EMPTY (nt_last_stack)) {
      X = POP (nt_last_stack);
      SXBA_0_bit (nt_last_set, X);

      bot = NPD (X);

      while (item = NUMPD (bot++)) {
	if (SXBA_bit_is_set (item_last_set, item)) {
	  /* item est concerne' par les changements sur X */
	  prod = PROLIS (item);
	  A = LHS (prod);

	  /* On regarde si ces changements se repercutent sur la lhs ... */
	  if (sxba_or_with_test (nt2last_packed [A], nt2last_packed [X])) {
	    /* ... oui ... */
	    if (SXBA_bit_is_reset_set (nt_last_set, A)) {
	      /* ... et A ne se trouve plus en pile */
#if EBUG
	      SXBA_1_bit (non_empty_last_set, A); /* non vide */
#endif /* EBUG */
	      PUSH (nt_last_stack, A);
	    }
	  }
	}
      }
    }

#if EBUG
    if (sxba_first_difference (non_empty_first_set, non_empty_last_set) != -1)
      sxtrap (ME, "check_rhs_sequences");
#endif /* EBUG */

    sxfree (nt_first_stack);
    sxfree (nt_first_set);
    sxfree (nt_last_stack);
    sxfree (nt_last_set);
    sxfree (item_first_set);
    sxfree (item_last_set);

    /* calcul de la fermeture reflexive du transpose' de left-corner */
    fermer2 (nt2cl_nt_set, MAXNT+1);
    /* calcul de la fermeture reflexive du transpose' de right-corner */
    fermer2 (nt2cr_nt_set, MAXNT+1);

#if EBUG
    {
      bool is_OK = true;
      /* Comme la grammaire selectionnee par basic_item_set est reduite ... */
      /* ... si un nt est non vide, son first et last doivent l'etre egalement */
      A = 0;

      while ((A = sxba_scan (first_or_last_lhs_nt_set, A)) > 0) {
#if is_epsilon
	if (EMPTY_SET && SXBA_bit_is_set (EMPTY_SET, A))
	  /* A =>+ vide */
	  continue;
#endif /* is_epsilon */

	/* A ne derive pas ds le vide et a ete calcule'*/
	if (!SXBA_bit_is_set (non_empty_first_set, A) || !SXBA_bit_is_set (non_empty_last_set, A)) {
	  /* nt2first|last [A] est vide (A n'est pas en lhs) */
	  is_OK = false;
#ifdef MAKE_INSIDEG
	  old_A = spf.insideG.nt2init_nt [A];
#else /* !MAKE_INSIDEG */
	  old_A = A;
#endif /* !MAKE_INSIDEG */

	  if (!SXBA_bit_is_set (non_empty_first_set, A))
	    printf ("SYSTEM ERROR: first(<%s>) should not be empty.\n", spf.inputG.ntstring [old_A]);

	  if (!SXBA_bit_is_set (non_empty_last_set, A))
	    printf ("SYSTEM ERROR: last(<%s>) should not be empty.\n", spf.inputG.ntstring [old_A]);
	}
      }

      if (!is_OK)
	sxtrap (ME, "check_rhs_sequences");

      sxfree (non_empty_first_set);
      sxfree (non_empty_last_set);
    }
#endif /* EBUG */

    nt2first_set = sxbm_calloc (MAXNT+1, SXEOF+1);
    nt2look_ahead_set = sxbm_calloc (MAXNT+1, SXEOF+1);
    nt2suffix_set = sxbm_calloc (MAXNT+1, SXEOF+1);
  }
  else {
    sxba_and (left_item_set, basic_item_set);
    sxba_and (right_item_set, basic_item_set);
#ifdef def_segment
    sxba_empty (crs_valid_segment_set);
#endif /* def_segment */
  }
  
  /* Ici nt2first_packed et nt2last_packed sont positionnes */
  /* Ainsi que le mapping packed_titem2titem */

  /* Recalcules a chaque fois car ils sont filtres par basic_item_set */

  /* Remplit nt2first_set avec les terminaux qui peuvent debuter nt */
  {
    SXINT     A, t, packed_titem, titem;

    SXBA    first_set, first_packed;

    A = 0;

    while ((A = sxba_scan (basic_nt_set, A)) > 0) {
      first_set = nt2first_set [A];
      sxba_empty (first_set);
      first_packed = nt2first_packed [A];

      packed_titem = 0;

      while ((packed_titem = sxba_scan (first_packed, packed_titem)) > 0) {
	titem = packed_titem2titem [packed_titem];

	if (SXBA_bit_is_set (basic_item_set, titem)) {
	  t = -LISPRO (titem);
	  SXBA_1_bit (first_set, t);
	}
      }
    }
  }


  /* Remplit nt2look_ahead_set et nt2suffix_set avec les terminaux qui peuvent suivre immediatement ou non les items terminaux de nt2last_packed */
  {
    SXINT     A, t, packed_titem, titem;
    SXBA    look_ahead_set, suffix_set, last_packed;
    bool is_first_t;

    A = 0;

    while ((A = sxba_scan (basic_nt_set, A)) > 0) {
      look_ahead_set = nt2look_ahead_set [A];
      suffix_set = nt2suffix_set [A];
      last_packed = nt2last_packed [A];

      sxba_empty (w_t_set);
      is_first_t = true;
      packed_titem = 0;

      while ((packed_titem = sxba_scan (last_packed, packed_titem)) > 0) {
	titem = packed_titem2titem [packed_titem];

	if (SXBA_bit_is_set (basic_item_set, titem)) {
	  t = -LISPRO (titem);

	  if (SXBA_bit_is_reset_set (w_t_set, t)) {
	    if (is_first_t) {
	      is_first_t = false;
	      sxba_copy (look_ahead_set, t2la_t_set [t]);
	      sxba_copy (suffix_set, t2suffix_t_set [t]);
	    }
	    else {
	      sxba_or (look_ahead_set, t2la_t_set [t]);
	      sxba_or (suffix_set, t2suffix_t_set [t]);
	    }
	  }
	}
      }

#if 0
      /* Le 02/03/07.  Pour etre +precis, comme la relation "se trouve avant" << n'est pas transitive, Si Af = first[A] et Al=last[A], nt2suffix_set [A] note' A<<
	 se calcule par A<< = Al<< \cap Af<< (ce sont a la fois des successeurs des debutants et des terminateurs de A) */
      /* On pourrait mesurer l'efficacite de cette mesure car c'est refait (partiellement) lors des calculs */
      first_set = nt2first_set [A];
      t = sxba_scan (first_set, 0);
      sxba_copy (w_t_set, t2suffix_t_set [t]);

      while ((t = sxba_scan (first_set, t)) > 0) {
	sxba_or (w_t_set, t2suffix_t_set [t]);
      }

      sxba_and (suffix_set, w_t_set);
      /* Il n'est pas necessaire de faire sxba_and (look_ahead_set, w_t_set); */
#endif /* 0 */
    }
  }


#if LLOG
  /* On imprime first, last, look_ahead et suffix */
  {
    SXINT     init_A, A, t, init_t, packed_titem, titem;
    SXBA    nt_first_set, nt_last_packed, nt_follow, nt_suffix;
    bool is_empty, is_first_time;

    A = 0;

    while ((A = sxba_scan (first_or_last_lhs_nt_set, A)) > 0) {
      nt_first_set = nt2first_set [A];
      nt_last_packed = nt2last_packed [A];
      nt_follow = nt2look_ahead_set [A];
      nt_suffix = nt2suffix_set [A];

      if (sxba_scan (nt_first_set, 0) > 0
	  || sxba_scan (nt_last_packed, 0) > 0
	  || sxba_scan (nt_follow, 0) > 0
	  || sxba_scan (nt_suffix, 0) > 0
	  ) {

#if is_epsilon
	is_empty = EMPTY_SET && sxba_bit_is_set (EMPTY_SET, A);
#else /* !is_epsilon */
	is_empty = false;
#endif /* !is_epsilon */

#ifdef MAKE_INSIDEG
	init_A = spf.insideG.nt2init_nt [A];
#else /* !MAKE_INSIDEG */
	init_A = A;
#endif /* !MAKE_INSIDEG */

	printf ("<%s>[%i/%i is %sempty]\n", spf.inputG.ntstring [init_A], A, init_A, is_empty ? "" : "non ");
	  
	fputs ("\tfirst = {", stdout);
	is_first_time = true;
	t = 0;
	  
	while ((t = sxba_scan (nt_first_set, t)) > 0) {
#ifdef MAKE_INSIDEG
	  init_t = spf.insideG.t2init_t [t];
#else /* !MAKE_INSIDEG */
	  init_t = t;
#endif /* !MAKE_INSIDEG */

	  if (is_first_time) {
	    is_first_time = false;
	    printf ("\"%s\"", spf.inputG.tstring [init_t]);
	  }
	  else
	    printf (", \"%s\"", spf.inputG.tstring [init_t]);
	}

	fputs ("}\n", stdout);

	fputs ("\tlast = {", stdout);

	sxba_empty (w_t_set);
	is_first_time = true;
	packed_titem = 0;
	  
	while ((packed_titem = sxba_scan (nt_last_packed, packed_titem)) > 0) {
	  titem = packed_titem2titem [packed_titem];

	  if (SXBA_bit_is_set (basic_item_set, titem)) {
	    t = -LISPRO (titem);

	    if (SXBA_bit_is_reset_set (w_t_set, t)) {
#ifdef MAKE_INSIDEG
	      init_t = spf.insideG.t2init_t [t];
#else /* !MAKE_INSIDEG */
	      init_t = t;
#endif /* !MAKE_INSIDEG */

	      if (is_first_time) {
		is_first_time = false;
		printf ("\"%s\"", spf.inputG.tstring [init_t]);
	      }
	      else
		printf (", \"%s\"", spf.inputG.tstring [init_t]);
	    }
	  }
	}

	fputs ("}\n", stdout);

	fputs ("\tlook_ahead = {", stdout);
	is_first_time = true;
	t = 0;

	while ((t = sxba_scan (nt_follow, t)) > 0) {
#ifdef MAKE_INSIDEG
	  init_t = spf.insideG.t2init_t [t];
#else /* !MAKE_INSIDEG */
	  init_t = t;
#endif /* !MAKE_INSIDEG */
	  if (is_first_time) {
	    is_first_time = false;
	    printf ("\"%s\"", spf.inputG.tstring [init_t]);
	  }
	  else
	    printf (", \"%s\"", spf.inputG.tstring [init_t]);
	}

	fputs ("}\n", stdout);


	fputs ("\tsuffix = {", stdout);
	t = sxba_scan (nt_suffix, 0);
	is_first_time = true;
	t = 0;
	  
	while ((t = sxba_scan (nt_suffix, t)) > 0) {
#ifdef MAKE_INSIDEG
	  init_t = spf.insideG.t2init_t [t];
#else /* !MAKE_INSIDEG */
	  init_t = t;
#endif /* !MAKE_INSIDEG */
	  if (is_first_time) {
	    is_first_time = false;
	    printf ("\"%s\"", spf.inputG.tstring [init_t]);
	  }
	  else
	    printf (", \"%s\"", spf.inputG.tstring [init_t]);
	}

	fputs ("}\n", stdout);
      }
    }
  }
#endif /* LLOG */


  {
   /* Nouvelle version du 02/03/07 */
    /* ... et du 09/03/07 qui peut utiliser def_segment */
   /* pour chaque prod p on isole successivement de gauche a droite dans sa rhs les sous-chaines de taille au moins 2 limitees par des symboles non nullable
      On appelle ca un segment
      p : U -> .... A X_1 ... X_p B ...
      on a donc A et B ne derivant pas ds le vide et X_1 ... X_p vide ou derivant ds le vide
      On va verifier que ce que l'on connait (first, last, follow, ...) sur les A X_1 ... X_p B est compatible avec le source.

      On commence par calculer un ensemble F qui va evoluer (diminuer) au fur et a mesure du traitement de la RHS et qui contient l'ensemble des terminaux
      qui "peuvent" suivre (au sens large) compte tenu du source et de tout ce qu'on a deja vu le point ou il est [re]calcule'.
      On ecrit a < b si le terminal a precede immediatement b ds le source et a << b si le terminal a precede b ds le source.
      On generalise ca a des ensembles de terminaux.
      Le gros PB avec ces 2 relations est qu'elles ne sont pas transitives :
      a < b et b < c   N'IMPLIQUE PAS a << c (source "bcab")
      a << b et b << c N'IMPLIQUE PAS a << c (source "bcab")
      Donc si on veut trouver l'ensemble C des terminaux qui peut suivre B qui lui-meme suit A, pour etre le + precis possible, il faut calculer :
      on a B = A<< et C = B<< \cap B (A<< designe les suivants de A).  On ne retient des suivants de B que ceux qui sont aussi des suivants de A.

      initialement F=T

      Pour chaque nouveau segment A X_1 ... X_p B 
      Soit Ala = A< et Asuf = A<<
      Soit F = Asuf \cap F

      On va d'abord verifier (sans tenir compte de X_1 ... X_p) si B peut suivre immediatement A

      Soit w = Ala \cap F
      Si w == NULL => on vire p
      Sinon on compare avec Bf = first [B]
        Si w \cap Bf != NULL => on continue
	i = 0;
	#if is_epsilon
	Si F \cap Bf == NULL => on vire p

	Attention, entre un tl de last [A] et un tf de first [b], il y a les X_i, mais aussi des Y_j qui terminent A et
	des Z_k qui commencent B.  Or les Y et Z sont inconnus bien qu'ils soient peut etre pertinents => aucune verif sur les X_i

	Sinon on va utiliser les X_1 ... X_p pour voir si leurs firsts (quand ils ne derivent pas ds vide) conviennent
	Foreach 1 <= i <= p
	  On calcule Xif = first [X_i] \cap Ala
	  Si Xif != NULL
	    On regarde s'il est compatible avec B
	    On calcule Xil = last [X_i]
	    que l'on filtre (ce doit etre des suivants de Xif compatibles avec F)
	    Xil = Xil \cap (Xif<< \cup Xif) \cap F (on peut ne prendre que F pour simplifier le calcul)
	    Si Xil != NULL
	       Si Xil<< \cap Bf \cap F != NULL
	         => OK : X_i peut suivre immediatement A et peut preceder B (on ne fait rien sur les X_{i+1} ... X_p)
		         on peut utiliser Xil< au lieu de Xil<< si i==p
	         F = Xil<<\cap F; Non, car c'est Xj, j>i qui peut etre bon et ici on ne le teste pas (ne le faire que si i==p)
		 break;

        Fin foreach
	#endif is_epsilon

	Si i==0 || i > p => echec, on vire p
   */
    SXINT     A, B, item, right_item, prod, top_item, X, Xi, Xj, bot_item, prev_item, inside_erased_prod_nb, i, j;
    SXINT     seg_id, top;
    SXINT     *seg_stack;
    SXBA    Ala_set, Asuf_set;

   /* On parcourt les prod */
    inside_erased_prod_nb = 0;
    item = 3;

    /* Nelle version, on utilise left_item_set et right_item_set */
    while ((item = sxba_scan (left_item_set, item)) > 0) {
#ifdef def_segment
      if ((seg_id = crs_segment [item]) <= 0) continue; /* Il n'y a pas de segment ds cette prod */
      prod = PROLIS (item);
#else /* ifndef def_segment */
      prod = PROLIS (item);
      right_item = sxba_scan (right_item_set, item);

      if (right_item <= 0 || prod != PROLIS (right_item)) continue; /* Il n'y a pas de segment ds cette prod */
#endif /* ifndef def_segment */

      /* prod contient au moins 2 symboles non nullables de basic => on l'examine */
      A = LISPRO (item);
      sxba_copy (F_set, (A > 0) ? nt2suffix_set [A] : t2suffix_t_set [-A]); /* Valeur initiale :successeurs de la frontiere gauche du segment le + a gauche */
      prev_item = item;

      while ((X = LISPRO (++item)) != 0) {
#if is_epsilon
	if (X > 0 && EMPTY_SET && SXBA_bit_is_set (EMPTY_SET, X))
	  NULL;
	else
#endif /* is_epsilon */
	  {
	    /* ici X < 0 ou ne derive pas ds le vide ... */
	    /* ... et c'est la fin d'un segment */
	    /* Pour chaque nouveau segment A X_1 ... X_p B */
	    /* ici, entre prev_item/A et item/X, il y a un segment */
#ifdef def_segment
	    /* seg_id est celui de A (et il est >0) */
	    if (!SXBA_bit_is_reset_set (crs_valid_segment_set, seg_id)) {
	      /* segment deja examine' (et OK!!) */
	      /* On le replace ds son ctxt gauche */
	      /* Asuf = A<< */
	      /* On fait evoluer F_set avant de continuer ... */
	      if (!sxba_2op (F_set, SXBA_OP_COPY, F_set, SXBA_OP_AND, (X > 0) ? nt2suffix_set [X] : t2suffix_t_set [-X])) {
		/* ... vide */
		crs_erase_prod (prod
#if LLOG
				, "Sequence error detection", A, X
#endif /* LLOG */
				);
		inside_erased_prod_nb++;
		break;
	      }

	      seg_id = crs_segment [item];

	      if (seg_id <= 0)
		break; /* Y'a plus de segments ds prod */
	    
	      A = X;
	      prev_item = item;

	      continue;
	    }
#endif /* def_segment */

	    B = X;
	    /* On regarde si le segment, independamment de son occurrence ds prod, est valide */
	      
	    /* Dans tous les cas, les look-ahead de B doivent aussi etre des successeurs de A */
	    /* Soit Asuf = A<< */
	    Asuf_set = (A > 0) ? nt2suffix_set [A] : t2suffix_t_set [-A];
	    succeeded = sxba_2op (NULL, SXBA_OP_IS, Asuf_set, SXBA_OP_AND, (B > 0) ? nt2look_ahead_set [B] : t2la_t_set [-B]);
	    
	    if (succeeded) {
	      /* Soit Ala = A< */
	      Ala_set = (A > 0) ? nt2look_ahead_set [A] : t2la_t_set [-A];
	      
	      /* On va d'abord regarder si B peut suivre immediatement A */
	      /* if (B > 0) => Au moins un debutant de B doit etre un suivant immediat des terminateurs de A */
	      /* if (B < 0) => B est un suivant immediat des terminateurs de A */
	      succeeded = (B > 0) ? sxba_2op (NULL, SXBA_OP_IS, Ala_set, SXBA_OP_AND, nt2first_set [B]) : SXBA_bit_is_set (Ala_set, -B);

#if is_epsilon
	      if (!succeeded && item > prev_item+1) {
		/* Il y a des Xi et les debutants de B ne sont pas des look-ahead immediats des terminateurs de A */
		/* Pour que p soit valide if faut donc qu'il existe un couple (Xi, Xj), i <= j tel que
		   - les debutants de Xi sont ds les look-ahead immediats des terminateurs de A et
		   - les debutants de B sont ds les look-ahead immediats des terminateurs de Xj
		   (autrement dit il existe une deriv ds laquelle A et Xi sont adjacents ainsi que Xj et B)
		   Il faut de + verifier que ces ensembles sont bien des suffixes.  Attention, on ne peut pas modifier F_set car on
		   n'est pas su^r que ce sont bien Xi et Xj qui vont etre utilises et pas Xi' ou Xj' avec i<i'<j'<j */
		if ((B > 0) ? sxba_2op (w2_t_set, SXBA_OP_COPY, Asuf_set, SXBA_OP_AND, nt2first_set [B]) : SXBA_bit_is_set (Asuf_set, -B)) {
		  /* Les debutants de B peuvent suivre A */
		  for (i = prev_item+1; i < item; i++) {
		    Xi = LISPRO (i);

		    if (sxba_2op (NULL, SXBA_OP_IS, Ala_set, SXBA_OP_AND, nt2first_set [Xi])) {
		      /* OK pour Xi et c'est le + a gauche */
		      for (j = item-1; j >= i; j--) {
			Xj = LISPRO (j);
			
			if (sxba_2op (NULL, SXBA_OP_IS, nt2look_ahead_set [Xj], SXBA_OP_AND, w2_t_set)) {
			  /* OK pour Xj et c'est le + a droite */
			  succeeded = true;
			  break;
			}
		      }

		      break;
		    }
		  }
		}
	      }
#endif /* is_epsilon */
	    }

	    if (!succeeded) {
	      /* Ici echec du segment independamment de son environment */
#ifdef def_segment
	      /* On va repercuter c,a sur toutes les prod qui contiennent ce segment */
	      seg_stack = crs_segment_list+seg_id;

	      for (top = *seg_stack; top > 0; top--) {
		i = seg_stack [top];

		if (SXBA_bit_is_set (basic_item_set, i)) {
		crs_erase_prod (PROLIS (i)
#if LLOG
				, "Sequence error detection", A, B
#endif /* LLOG */
				);
		  inside_erased_prod_nb++;
		}
	      }
#else /* ifndef def_segment */
		crs_erase_prod (prod
#if LLOG
				, "Sequence error detection", A, B
#endif /* LLOG */
				);
	      inside_erased_prod_nb++;
#endif /* ifndef def_segment */

	      break;
	    }

	    /* Ici on va terminer les verifs sur prod ... */
	    /* ... et calculer F_set pour le segment suivant a droite */
	    if (!((B > 0) ? sxba_2op (NULL, SXBA_OP_IS, F_set, SXBA_OP_AND, nt2first_set [B]) : SXBA_bit_is_set (F_set, -B))
		|| !(sxba_2op (F_set, SXBA_OP_COPY, F_set, SXBA_OP_AND, (B > 0) ? nt2suffix_set [B] : t2suffix_t_set [-B]))) {
		crs_erase_prod (prod
#if LLOG
				, "Sequence error detection", A, B
#endif /* LOG */
				);
		inside_erased_prod_nb++;
		break;
	    }

#ifdef def_segment
	    seg_id = crs_segment [item];

	    if (seg_id <= 0)
	      break; /* Y'a plus de segments ds prod */
#endif /* def_segment */

	    A = B;
	    prev_item = item;
	  }
      }
    }

#if MEASURES || LOG
    printf ("%sThe check_rhs_sequences procedure in inside mode erases %i productions.\n",
#if MEASURES
	    "MEASURES: ",
#else /* LOG */
	    "",
#endif /* LOG */
	    inside_erased_prod_nb);
#endif /* MEASURES || LOG */

    erased_prod_nb += inside_erased_prod_nb;
  }
    
  {
    /* On verifie que pour chaque A-regle p telle que p = A -> \alpha B \beta, \alpha =>* \eps et B non nullable
       B est tel que
       - soit les debutants de B sont dans les look-ahead des voisins gauches des ancetres de A
       - soit les debutants de B sont dans les suffixes des voisins gauches des ancetres de A
    */
    /* la == look-ahead, n == left-neighbour et a == ancestor */
    /* nt2anla_t_set contient soit les 1_look ahead soit les look_ahead (de taille 1) a une distance k qcq des voisins gauches des ancetres */
    /* aln = ancestor left_neighbour */
    /* C'est la composition de l'inverse de la relation coin gauche avec la relation left_neighbour */
    SXINT  predecessor_erased_prod_nb, item, prod, X, A, B, item2, t, bot_item, top_item;
    SXBA cl_nt_set, anla_t_set, look_ahead;

    /* calcul pour chaque nt des look_aheads des voisins gauches de ses ancetres */
#if is_epsilon
    sxba_empty (w2_t_set); /* Il peut ne pas etre vide car utilise' par inside */
#endif /* is_epsilon */
    A = 0;

    while ((A = sxba_scan (basic_nt_set, A)) > 0) {
      cl_nt_set = nt2cl_nt_set [A];
      anla_t_set = NULL;

      if (!SXBA_bit_is_set (cl_nt_set, A))
	/* On force A est un ancetre de A */
	left_neighbour (A, w2_t_set, w2_nt_set);

      X = 0;

      while ((X = sxba_scan (cl_nt_set, X)) > 0) {
	/* X est un ancetre de A */
	left_neighbour (X, w2_t_set, w2_nt_set);
      }

      t = 0;

      while ((t = sxba_scan_reset (w2_t_set, t)) > 0) {
	/* t est un voisin gauche d'un ancetre de A */
#if is_epsilon
	look_ahead = t2suffix_t_set [t];
#else /* !is_epsilon */
	look_ahead = t2la_t_set [t];
#endif /* !is_epsilon */

	if (anla_t_set == NULL) {
	  anla_t_set = nt2anla_t_set [A];
	  sxba_copy (anla_t_set, look_ahead);
	}
	else
	  sxba_or (anla_t_set, look_ahead);
      }

      X = 0;

      while ((X = sxba_scan_reset (w2_nt_set, X)) > 0) {
	/* X est un nt voisin gauche d'un ancetre de A */
#if is_epsilon
	look_ahead = nt2suffix_set [X];
#else /* !is_epsilon */
	look_ahead = nt2look_ahead_set [X];
#endif /* !is_epsilon */

	if (anla_t_set == NULL) {
	  anla_t_set = nt2anla_t_set [A];
	  sxba_copy (anla_t_set, look_ahead);
	}
	else
	  sxba_or (anla_t_set, look_ahead);
      }
    }

    /* Soit une prod p A -> \alpha B \beta avec \alpha =>* \eps et B non nullable
       Si first [B] ou (last [B]) n'est pas ds nt2anla_t_set [A]
       alors on peut eliminer p */

    predecessor_erased_prod_nb = 0;
    item = 3;

    while ((item = sxba_scan (left_item_set, item)) > 0) {
      /* item = A -> \alpha .B \beta avec \alpha =>* \eps et B non-nullable */
      B = LISPRO (item);
      prod = PROLIS (item);
      A = LHS (prod);
      succeeded = (B > 0) ? sxba_2op (NULL, SXBA_OP_IS, nt2anla_t_set [A], SXBA_OP_AND, nt2first_set [B]) : SXBA_bit_is_set (nt2anla_t_set [A], -B);
	
#if is_epsilon
      if (succeeded && B > 0) {
	/* On utilise nt2look_ahead_set [B] a la place de nt2last_set [B] qui n'est pas calcule' */
	succeeded = sxba_2op (NULL, SXBA_OP_IS, nt2anla_t_set [A], SXBA_OP_AND, nt2look_ahead_set [B]);
      }
#endif /* !is_epsilon */

      if (!succeeded) {
	/* Echec */
	crs_erase_prod (prod
#if LLOG
			, "Ancestor's left neighbours look-aheads", A, B
#endif /* LLOG */
			);
	predecessor_erased_prod_nb++;
      }
    }

#if MEASURES || LOG
    printf ("%sThe check_rhs_sequences procedure in left-corner neighbours mode erases %i productions.\n",
#if MEASURES
	    "MEASURES: ",
#else /* LOG */
	    "",
#endif /* LOG */
	    predecessor_erased_prod_nb);
#endif /* MEASURES || LOG */

    erased_prod_nb += predecessor_erased_prod_nb;
  }

  {
    /* On verifie que pour chaque A-regle p telle que p = A -> \alpha B \beta, \beta =>* \eps et B non nullable
       B est tel que des look-ahead de B sont des debutants des voisins droits dea ancetres de A
    */
    /* lb == look-back, rn == right-neighbour et a == ancestor */
    SXINT  successor_erased_prod_nb, item2, bot_item, top_item, t, item, prod, X, A, B;
    SXBA cr_nt_set, anla_t_set;

    /* calcul pour chaque nt des debutants des voisins droits de ses ancetres */ 
    /* Pour chaque nt A on calcule ses ancetres (c'est la transposee de la relation right-corner).  Soit X un tel ancetre.
       Pour chaque X on calcule ses voisins droits Y.  pour chaque Y on calcule ses debutants qui doivent etre des look_ahead de B. */
    A = 0;

    while ((A = sxba_scan (basic_nt_set, A)) > 0) {
      cr_nt_set = nt2cr_nt_set [A];
      anla_t_set = nt2anla_t_set [A];
      sxba_empty (anla_t_set);

      if (!SXBA_bit_is_set (cr_nt_set, A))
	/* On force A est un ancetre de A */
	right_neighbour (A, anla_t_set, w2_nt_set);

      X = 0;

      while ((X = sxba_scan (cr_nt_set, X)) > 0) {
	/* X est un ancetre de A */
	right_neighbour (X, anla_t_set, w2_nt_set);
      }

      X = 0;

      while ((X = sxba_scan_reset (w2_nt_set, X)) > 0) {
	/* X est un nt voisin droit d'un ancetre de A */
	sxba_or (anla_t_set, nt2first_set [X]);
      }
    }

    /* Soit une prod p A -> \alpha B \beta avec \beta =>* \eps et B non nullable */

    successor_erased_prod_nb = 0;
    item = 3;

    while ((item = sxba_scan (right_item_set, item)) > 0) {
      B = LISPRO (item);
      prod = PROLIS (item);
      A = LHS (prod);
	
#if is_epsilon
      succeeded = sxba_2op (NULL, SXBA_OP_IS, nt2anla_t_set [A], SXBA_OP_AND, (B > 0) ? nt2suffix_set [B] : t2suffix_t_set [-B]);
#else /* !is_epsilon */
      succeeded = sxba_2op (NULL, SXBA_OP_IS, nt2anla_t_set [A], SXBA_OP_AND, (B > 0) ? nt2look_ahead_set [B] : t2la_t_set [-B]);
#endif /* !is_epsilon */

      if (!succeeded) {
	/* prod doit etre eleminee */
	crs_erase_prod (prod
#if LLOG
			, "Ancestor's right neighbours look-aheads", A, B
#endif /* LLOG */
			);
	successor_erased_prod_nb++;
      }
    }

#if MEASURES || LOG
    printf ("%sThe check_rhs_sequences procedure in right-corner neighbours mode erases %i productions.\n",
#if MEASURES
	    "MEASURES: ",
#else /* LOG */
	    "",
#endif /* LOG */
	    successor_erased_prod_nb);
#endif /* MEASURES || LOG */

    erased_prod_nb += successor_erased_prod_nb;
  }

  *is_axiom_kept = (axiom_prod_nb > 0);

#if LOG
  fputs ("*** Leaving check_rhs_sequences ***\n", stdout);
#endif /* LOG */

  return erased_prod_nb;
}
#endif /* is_check_rhs_sequences */

#ifdef MAKE_INSIDEG
/* Alloues et positionnes ds make_a_first_reduced_grammar () et utilises/liberes ds make_insideG () */
static SXINT *old_nt2lhs_nb, *old_nt2rhs_nb, *old_t2rhs_nb;
#endif /* MAKE_INSIDEG */

/* NRG = NextReducedGrammar */
#ifndef NRG
#if !defined MULTI_ANCHOR || defined is_set_automaton || defined is_check_rhs_sequences
#define NRG 1
#endif /* !define MULTI_ANCHOR || defined is_set_automaton || defined is_check_rhs_sequences */
#endif /* NRG */

#if NRG==1
/* Non realloues a chaque appel de make_a_next_reduced_grammar () */
static SXINT             *nrg_working_nt_stack, *nrg_store_npd, *nrg_store_numpd, *nrg_store_npg, *nrg_store_numpg, *nrg_prod2rhs_nt_nb, *nrg_init_tprod_stack;
static SXBA            nrg_working_nt_set, nrg_tgen_nt_set, nrg_nyp_nt_set, nrg_nyp_nt_set2;
#endif /* NRG */


/* Cette fonction prend ds basic_item_set une "sous-grammaire" de inputG et la reduit sur place */
/* Si #ifdef MAKE_INSIDEG, on en profite pour remplir
  spf.insideG.maxprod
  spf.insideG.maxt
  spf.insideG.maxnt
  spf.insideG.maxitem
  spf.insideG.maxrhs
  spf.insideG.maxrhsnt
  spf.insideG.sizeofnumpd
  spf.insideG.sizeoftnumpd
  spf.insideG.is_eps
*/
  
/* basic_item_set et basic_nt_set sont des SXBA globaux (les analyseurs comme earley peuvent y acceder) */
/* Filtre basic_item_set pour ne laisser que la + grande sous-grammaire reduite ... */
/* ... et, alloue/remplit basic_nt_set avec les nt de cette sous-grammaire */
/* Retourne le nb de prod de la grammaire reduite */
/* On ne conserve que les prods accessibles depuis le super axiome 0 ... */
/* ... qui peuvent generer des chaines terminales de t_set^* */
/* Nelle version le 09/02/07 depend de HUGE_CFG */

static SXINT make_a_first_reduced_grammar ()
{
  SXINT             item, nb, X, prod, A, bot, top, bot_item, cur_item, unlexicalized_prod_nb;
  SXINT             new_prod_nb = 0;
  SXBA              tgen_nt_set, working_nt_set, basic_t_set; /* Allocation dynamique ds le tas (plante en static !!) */
  SXINT             *working_nt_stack;
  SXINT             *prod2rhs_nt_nb;
  SXINT             rhsnt_nb, rhst_nb, rhs_nb;

#if LOG
  fputs ("****** Entering make_a_first_reduced_grammar () ******\n", stdout);
#endif /* LOG */

  spf.insideG.maxprod = spf.insideG.maxt = spf.insideG.maxnt = spf.insideG.maxitem = 0;
  spf.insideG.maxrhs = spf.insideG.maxrhsnt = spf.insideG.sizeofnumpd = spf.insideG.sizeoftnumpd = 0;
  spf.insideG.is_eps = false;

  /* calcul des nt qui peuvent deriver ds t_set^* */
  tgen_nt_set = sxba_calloc (inputG_MAXNT+1);
  working_nt_set = sxba_calloc (inputG_MAXNT+1);
  basic_t_set = sxba_calloc (inputG_SXEOF+1);

  working_nt_stack = (SXINT*) sxalloc (inputG_MAXNT+1, sizeof (SXINT)), RAZ (working_nt_stack);

  /* On cree basic_nt_set, l'ensemble des nt de la grammaire reduite */
  sxprepare_for_possible_reset (basic_nt_set);
  basic_nt_set = sxba_calloc (inputG_MAXNT+1);

  /* basic_prod_stack/basic_item_set ne contiennent que des ref aux prod lexicalisees selectionnees par le source */
  /* Attention, on va rater S =>+ \eps */

  /* prod2rhs_nt_nb doit etre initialiser a 0 */
  prod2rhs_nt_nb = (SXINT*) sxcalloc (inputG_MAXPROD+1, sizeof (SXINT));
   
#ifdef HUGE_CFG
  while (!IS_EMPTY (basic_prod_stack))
#else /* ifndef HUGE_CFG */
    item = 3; /* On saute [S' -> $ S $] ... */

  while ((item = sxba_scan (basic_item_set, item)) >= 0)
#endif /* ifndef HUGE_CFG */
    {

#ifdef HUGE_CFG
      prod = POP (basic_prod_stack);
      if (prod == 0) continue; /* Super-regle */
      item = spf.inputG.prolon [prod];
#else /* ifndef HUGE_CFG */
      prod = spf.inputG.prolis [item];
#endif /* ifndef HUGE_CFG */

      A = spf.inputG.lhs [prod];
      nb = 0;


      while ((X = spf.inputG.lispro [item]) != 0) {
	if (X > 0
#if is_epsilon
	    && !SXBA_bit_is_set (spf.inputG.BVIDE, X)
	    /* Si X =>+ vide, on ne repere pas ces X en RHS */
#endif /* is_epsilon */
	    ) {
	  nb++;
	}

	item++;
      }

      if (nb == 0) {
#if is_epsilon
	if (!SXBA_bit_is_set (spf.inputG.BVIDE, A))
	  /* Si A =>+ vide, on ne met pas A ds tgen_nt_set */
#endif /* is_epsilon */
	  {
	    if (SXBA_bit_is_reset_set (working_nt_set, A)) /* A genere une chaine terminale */
	      PUSH (working_nt_stack, A);

#if NRG==1
	    /* On memorise pour make_a_next_reduced_grammar () la pile des prod dont la rhs est ds T^+ */
	    if (nrg_init_tprod_stack == NULL) {
	      sxprepare_for_possible_reset (nrg_init_tprod_stack);
	      DALLOC_STACK (nrg_init_tprod_stack, 50);
	    }

	    DPUSH (nrg_init_tprod_stack, prod);
#endif /* NRG */
	  }
#if LLOG
	fputs ("This production generates a terminal string:", stdout);
	output_prod (prod);
#endif /* LLOG */

	nb = -1; /* traitee */
      }

      prod2rhs_nt_nb [prod] = nb; /* nb de nt de la rhs sur lesquels on ne sait rien */
    }

  /* Ici working_nt_stack contient une liste des nt A tq A=>+ x, x \in t_set^+ (non vide) et A (par ailleurs) ne derive pas ds le vide */
  /* tgen_nt_set garde working_nt_stack */
  while (!IS_EMPTY (working_nt_stack)) {
    X = POP (working_nt_stack);
    /* X genere une chaine terminale ... */
    SXBA_1_bit (tgen_nt_set, X);

#if LLOG
    printf ("Seeking for RHS occurrences of <%s>\n", spf.inputG.ntstring [X]);
#endif /* LLOG */

    bot = spf.inputG.npd [X];

    while (item = spf.inputG.numpd [bot]) {
      prod = spf.inputG.prolis [item];
      /* ... et l'on a item/prod A -> \alpha .X \beta */

      if (prod > 0) {
	/* On est su^r que prod n'est pas la super-re`gle */
	A = spf.inputG.lhs [prod];

#if is_epsilon
	if (!SXBA_bit_is_set (spf.inputG.BVIDE, A))
	  /* Si A =>+ vide, on ne met pas A ds tgen_nt_set */
#endif /* is_epsilon */
	  {
	    if (!SXBA_bit_is_set (working_nt_set, A)) {
	      /* On ne sait encore rien sur A */
	      nb = prod2rhs_nt_nb [prod];
#if EBUG
	      if (nb < 0)
		/* A =>+ T^+ */
		sxtrap (ME, "make_a_first_reduced_grammar");
#endif /* EBUG */

	      if (nb > 0 /* lexicalisee et non finalisee */ || 
#ifdef HUGE_CFG
		  unlexicalized_prod_set && SXBA_bit_is_set (unlexicalized_prod_set, prod)
#else  /* ifndef HUGE_CFG */
		  SXBA_bit_is_set (unlexicalized_item_set, item)
#endif /* ifndef HUGE_CFG */
		  ) {
		if (nb == 0) {
		  /* prod jamais vue (et non lexicalisee */
		  cur_item = spf.inputG.prolon [prod];

		  while ((X = spf.inputG.lispro [cur_item]) != 0) {
#if EBUG
		    if (X < 0) 
		      sxtrap (ME, "make_a_first_reduced_grammar");
#endif /* EBUG */

		    if (!SXBA_bit_is_set (tgen_nt_set, X)
#if is_epsilon
			&&!SXBA_bit_is_set (spf.inputG.BVIDE, X)
			/* Si X =>+ vide, on ne le compte pas */
#endif /* is_epsilon */
			)
		      nb++;

		    cur_item++;  
		  }
		}
		else {
		  nb--;
		}

		if (nb == 0) {
		  if (SXBA_bit_is_reset_set (working_nt_set, A))
		    PUSH (working_nt_stack, A);
#if LLOG
		  fputs ("This production generates a non empty terminal string:", stdout);
		  output_prod (prod);
#endif /* LLOG */
	       
		  nb = -1;
		}

		prod2rhs_nt_nb [prod] = nb;
	      }
	    }
	  }
      }

      bot++;
    }
  }

#if is_epsilon
  if (SXBA_bit_is_set (spf.inputG.BVIDE, 1))
    /* Si eps est ds le langage on dit que l'axiome genere une chaine terminale (vide) */
    SXBA_1_bit (tgen_nt_set, 1);
#endif /* is_epsilon */

  /* Ici tgen_nt_set contient l'ensemble des nt A tq A=>+ x, x \in t_set^* */
  if (SXBA_bit_is_set (tgen_nt_set, 1)) {
    /* On a donc S =>+ x, x \in t_set^* */
    /* On vire les productions qui ne generent pas des chaines terminales ou qui sont inaccessibles depuis l'axiome */
#ifdef MAKE_INSIDEG
    /* Alloues/positionnes ici ds make_a_first_reduced_grammar () et utilises/liberes ds make_insideG () */
    sxprepare_for_possible_reset (old_nt2lhs_nb);
    old_nt2lhs_nb = (SXINT*) sxcalloc (inputG_MAXNT+1, sizeof (SXINT)), old_nt2lhs_nb [0] = 0;
    sxprepare_for_possible_reset (old_nt2rhs_nb);
    old_nt2rhs_nb = (SXINT*) sxcalloc (inputG_MAXNT+1, sizeof (SXINT)), old_nt2rhs_nb [0] = 0;
    sxprepare_for_possible_reset (old_t2rhs_nb);
    old_t2rhs_nb = (SXINT*) sxcalloc (inputG_SXEOF+1, sizeof (SXINT));
    /* Attention, du fait de la forme de inputG, old_t2rhs_nb [0] et old_t2rhs_nb [inputG_SXEOF] sont initialises a 1 */
    old_t2rhs_nb [0] = old_t2rhs_nb [inputG_SXEOF] = 1; /* Le eof a l'item 0 !! */
#endif /* MAKE_INSIDEG */

    /* On reremplit tout */
#ifdef HUGE_CFG
    /* On utilise basic_prod_stack */
    /* TOP (basic_prod_stack) = 0; elle est vide */
    PUSH (basic_prod_stack, 0); /* la super-regle */
    /* l'indice 2 sera pour une S-prod */
#else /* ifndef HUGE_CFG */
    /* On utilise basic_item_set */
    sxba_empty (basic_item_set);
    sxba_range (basic_item_set, 0, 4); /* Les items de la super-regle */
#endif /* ifndef HUGE_CFG */

    /* l'ensemble des nt de la grammaire reduite */
    SXBA_1_bit (basic_nt_set, 1);

    sxba_empty (working_nt_set);
    SXBA_1_bit (working_nt_set, 1); /* l'axiome */
    PUSH (working_nt_stack, 1);

#if MEASURES || LOG
    unlexicalized_prod_nb = 0;
#endif /* MEASURES || LOG */

    spf.insideG.maxnt = 1;

    while (!IS_EMPTY (working_nt_stack)) {
      A = POP (working_nt_stack);
      /* Ici on a S =>* A =>+ x */

      bot = spf.inputG.npg [A];
      top = spf.inputG.npg [A+1];

      while (bot < top) {
	prod = spf.inputG.numpg [bot];
	bot_item = spf.inputG.prolon [prod];
	nb = prod2rhs_nt_nb [prod];

	if (nb != 0 /* examinee en bu, on va la considerer car elle peut etre bonne */ || 
#ifdef HUGE_CFG
	    unlexicalized_prod_set && SXBA_bit_is_set (unlexicalized_prod_set, prod)
#else  /* ifndef HUGE_CFG */
	    SXBA_bit_is_set (unlexicalized_item_set, bot_item)
#endif /* ifndef HUGE_CFG */
	    ) {
	  item = bot_item;

	  while ((X = spf.inputG.lispro [item++]) != 0) {
	    if (X > 0 &&
#if is_epsilon
		!SXBA_bit_is_set (spf.inputG.BVIDE, X) &&
#endif /* is_epsilon */
		!SXBA_bit_is_set (tgen_nt_set, X) /* ne derive pas ds un x */
		) {
	      /* ... prod doit donc etre rejetee */
	      break;
	    }
	  }

	  if (X == 0) {
	    /* OK RHS =>* x */
#if LLOG
	    fputs ("This production belongs to the reduced grammar:", stdout);
	    output_prod (prod);
#endif /* LLOG */

	    new_prod_nb++;

#ifdef HUGE_CFG
	    /* On utilise basic_prod_stack */
	    PUSH (basic_prod_stack, prod);
#else /* ifndef HUGE_CFG */
	    /* On utilise basic_item_set */
	    SXBA_1_bit (basic_item_set, bot_item);
#endif /* ifndef HUGE_CFG */

	    rhsnt_nb = rhst_nb = 0;
	    item = bot_item;

	    while ((X = spf.inputG.lispro [item]) != 0) {
	      if (X > 0) {
		rhsnt_nb++;
#ifdef MAKE_INSIDEG
		old_nt2rhs_nb [X]++;
#endif /* MAKE_INSIDEG */

		if (SXBA_bit_is_reset_set (working_nt_set, X)) {
		  PUSH (working_nt_stack, X);
		  spf.insideG.maxnt++;
		  /* l'ensemble des nt de la grammaire reduite */
		  SXBA_1_bit (basic_nt_set, X);
		}
	      }
	      else {
		rhst_nb++;
		X = -X;

		if (SXBA_bit_is_reset_set (basic_t_set, X)) {
#ifndef MAKE_INSIDEG
		  /* c'est le make_a_*first*_reduced_grammar : lex_source_set contient déjà EOF mais rien d'autre */
		  SXBA_1_bit (lex_source_set, X);
#endif /* ndef MAKE_INSIDEG */		  
		  spf.insideG.maxt--;
		}

#ifdef MAKE_INSIDEG
		old_t2rhs_nb [X]++;
#endif /* MAKE_INSIDEG */
	      }

	      item++;
#ifndef HUGE_CFG
	      SXBA_1_bit (basic_item_set, item);
#endif /* ifndef HUGE_CFG */
	    }

#ifdef MAKE_INSIDEG
	    old_nt2lhs_nb [A]++;
#endif /* MAKE_INSIDEG */

#if MEASURES || LOG
	    if (rhst_nb == 0) unlexicalized_prod_nb++;
#endif /* MEASURES || LOG */

	    spf.insideG.maxprod++;
	    rhs_nb = rhsnt_nb+rhst_nb;
	    spf.insideG.maxitem += rhs_nb+1;
	    spf.insideG.sizeofnumpd += rhsnt_nb;
	    spf.insideG.sizeoftnumpd += rhst_nb;
	      
	    if (rhsnt_nb > spf.insideG.maxrhsnt)
	      spf.insideG.maxrhsnt = rhsnt_nb;

	    if (rhs_nb > spf.insideG.maxrhs)
	      spf.insideG.maxrhs = rhs_nb;

	    if (rhs_nb == 0)
	      spf.insideG.is_eps = true;
	  }
	}

	bot++;
      }
    }

    /* Qq petites rectifications ... */
    spf.insideG.maxt--; /* eof */
    spf.insideG.maxitem += 3; /* La super-prod */
    spf.insideG.sizeofnumpd += spf.insideG.maxnt+2;
    spf.insideG.sizeoftnumpd += 2 /* les 2 eof */+1/* index 0 */+1/* 0 bidon de fin */;
  }

  sxfree (prod2rhs_nt_nb);
  sxfree (tgen_nt_set);
  sxfree (working_nt_set);
  sxfree (basic_t_set);
  sxfree (working_nt_stack);

#ifndef MAKE_PROPER
#if LLOG
  {
    SXINT  new_prod, t, cur, top;

    /* On sort la nouvelle grammaire filtree par le source */
    fputs ("\n********** Current reduced sub-grammar **********\n", stdout);

    if (new_prod_nb == 0)
      fputs ("\n********** WARNING: This grammar is not reduced **********\n", stdout);

    new_prod = 0;

#ifdef HUGE_CFG
    top = TOP (basic_prod_stack);

    for (cur = 1; cur <= top; cur++)
#else /* ifndef HUGE_CFG */
      item = 3; /* On saute [S' -> $ S $] ... */

    while ((item = sxba_scan (basic_item_set, item)) >= 0)
#endif /* ifndef HUGE_CFG */
      {
#ifdef HUGE_CFG
	prod = basic_prod_stack [cur];
	if (prod == 0) continue; /* Super-regle */
	item = spf.inputG.prolon [prod];
#else /* ifndef HUGE_CFG */
	prod = spf.inputG.prolis [item];
#endif /* ifndef HUGE_CFG */
	new_prod++;
	printf ("%i/", new_prod);
	output_prod (prod);
#ifndef HUGE_CFG
	item = spf.inputG.prolon [prod+1]-1;
#endif /* ifndef HUGE_CFG */
      }

    fputs ("********** End of Current reduced sub-grammar **********\n", stdout);
  }
#endif /* LLOG */
#endif /* !MAKE_PROPER */

#if EBUG
  if (new_prod_nb) {
    /* On verifie que la grammaire est reduite */
    SXINT            top, cur, item, prod, A, X, nb;
    SXINT            t_nb, rhs_t_nb, nt_nb, rhs_nt_nb, lhs_nt_nb, prod_nb, tgen_prod_nb, tgen_nt_nb;
    SXBA           t_set, nt_set, rhs_nt_set, lhs_nt_set, rhs_t_set, prod_set, tgen_prod_set, nyp_nt_set, nyp_nt_set2; /* Allocation dynamique ds le tas (plante en static !!) */
    SXINT            *working_nt_stack, *store_npd, *store_numpd, *store_npg, *store_numpg, *prod2rhs_nt_nb;

    t_set = sxba_calloc (inputG_SXEOF+1);
    rhs_t_set = sxba_calloc (inputG_SXEOF+1);

    nt_set = sxba_calloc (inputG_MAXNT+1);
    rhs_nt_set = sxba_calloc (inputG_MAXNT+1);
    lhs_nt_set = sxba_calloc (inputG_MAXNT+1);
    nyp_nt_set = sxba_calloc (inputG_MAXNT+1);
    nyp_nt_set2 = sxba_calloc (inputG_MAXNT+1);

    prod_set = sxba_calloc (inputG_MAXPROD+1);
    tgen_prod_set = sxba_calloc (inputG_MAXPROD+1);

    working_nt_stack = (SXINT*) sxalloc (inputG_MAXNT+1, sizeof (SXINT));
    store_npd = (SXINT*) sxalloc (inputG_MAXNT+1, sizeof (SXINT));
    store_numpd = (SXINT*) sxalloc (inputG_MAXITEM+1 /* Moins precis que sizeof(numpd)/sizeof(*numpd) mais disponible si numpd non "compile'" en local */, sizeof (SXINT));
    store_npg = (SXINT*) sxalloc (inputG_MAXNT+1, sizeof (SXINT));
    store_numpg = (SXINT*) sxalloc (inputG_MAXPROD+inputG_MAXNT+1, sizeof (SXINT));
    prod2rhs_nt_nb = (SXINT*) sxalloc (inputG_MAXPROD+1, sizeof (SXINT));

#ifdef HUGE_CFG
    top = TOP (basic_prod_stack);

    for (cur = 1; cur <= top; cur++)
#else /* ifndef HUGE_CFG */
    item = -1;

    while ((item = sxba_scan (basic_item_set, item)) >= 0)
#endif /* ifndef HUGE_CFG */
      {
#ifdef HUGE_CFG
	prod = basic_prod_stack [cur];
	item = spf.inputG.prolon [prod];
#else /* ifndef HUGE_CFG */
	prod = spf.inputG.prolis [item];
#endif /* ifndef HUGE_CFG */
	SXBA_1_bit (prod_set, prod);
	A = spf.inputG.lhs [prod];

	if (A > 0) {
	  SXBA_1_bit (nt_set, A);
	
	  if (SXBA_bit_is_reset_set (nyp_nt_set2, A))
	    /* 1ere A-prod */
	    store_numpg [store_npg [A] = spf.inputG.npg [A+1]-1+A] = 0; /* On pointe vers le 0 de fin de liste (liste vide) */

	  store_numpg [--store_npg [A]] = prod; /* On ajoute prod a la liste */
	}

	nb = 0;

	while ((X = spf.inputG.lispro [item]) != 0) {
	  if (X > 0) {
	    nb++;
	    SXBA_1_bit (nt_set, X);

	    if (SXBA_bit_is_reset_set (nyp_nt_set, X))
	      store_numpd [store_npd [X] = spf.inputG.npd [X+1]-1] = 0; /* On pointe vers le 0 de fin de liste (liste vide) */

	    store_numpd [--store_npd [X]] = item; /* On ajoute item a la liste */
	  }
	  else {
	    X = -X;

	    SXBA_1_bit (t_set, X);
	  }

	  item++;
	}

	prod2rhs_nt_nb [prod] = nb;
      }

    if (!SXBA_bit_is_set (nt_set, 1))
      sxtrap (ME, "make_a_first_reduced_grammar (NO AXIOM)");

    SXBA_1_bit (rhs_t_set, inputG_SXEOF); /* Eof de la super-regle */
    SXBA_1_bit (rhs_nt_set, 1); /* Axiome ds la super-regle */
    
    working_nt_stack [0] = 0;
    PUSH (working_nt_stack, 1);

    while (!IS_EMPTY (working_nt_stack)) {
      A = POP (working_nt_stack);
      SXBA_1_bit (lhs_nt_set, A);

      bot = store_npg [A];

      while ((prod = store_numpg [bot]) != 0) {
	item = spf.inputG.prolon [prod];

	while ((X = spf.inputG.lispro [item]) != 0) {
	  if (X > 0) {
	    if (SXBA_bit_is_reset_set (rhs_nt_set, X))
	      PUSH (working_nt_stack, X);
	  }
	  else {
	    X = -X;
	    SXBA_1_bit (rhs_t_set, X);
	  }

	  item++;
	}

	bot++;
      }
    }

    t_nb = sxba_cardinal (t_set);
    rhs_t_nb = sxba_cardinal (rhs_t_set);
    nt_nb = sxba_cardinal (nt_set);
    rhs_nt_nb = sxba_cardinal (rhs_nt_set);
    lhs_nt_nb = sxba_cardinal (lhs_nt_set);

    if (t_nb != rhs_t_nb || nt_nb != rhs_nt_nb || nt_nb != lhs_nt_nb)
      sxtrap (ME, "make_a_first_reduced_grammar (TOP-DOWN CHECK FAILED)");
    
    /* On verifie que chaque nt genere une chaine terminale */
    sxba_empty (lhs_nt_set);
    prod = 0;

    while ((prod = sxba_scan (prod_set, prod)) > 0) {
      item = spf.inputG.prolon [prod];

      while ((X = spf.inputG.lispro [item]) != 0) {
	if (X > 0)
	  break;

	item++;
      }

      if (X == 0) {
	/* prod genere une chaine terminale */
	SXBA_1_bit (tgen_prod_set, prod);
	A = spf.inputG.lhs [prod];
	/* A genere une chaine terminale */

	if (SXBA_bit_is_reset_set (lhs_nt_set, A))
	  PUSH (working_nt_stack, A);
      }
    }

    while (!IS_EMPTY (working_nt_stack)) {
      X = POP (working_nt_stack);

      bot = store_npd [X];

      while ((item = store_numpd [bot]) != 0) {
	prod = spf.inputG.prolis [item];

	if (--prod2rhs_nt_nb [prod] == 0) {
	  SXBA_1_bit (tgen_prod_set, prod);
	  A = spf.inputG.lhs [prod];
	  /* A genere une chaine terminale */

	  if (A > 0 && SXBA_bit_is_reset_set (lhs_nt_set, A))
	    PUSH (working_nt_stack, A);
	}

	bot++;
      }
    }

    prod_nb = sxba_cardinal (prod_set);
    tgen_prod_nb = sxba_cardinal (tgen_prod_set);
    tgen_nt_nb = sxba_cardinal (lhs_nt_set);

    if (prod_nb != tgen_prod_nb || nt_nb != tgen_nt_nb) {
      if (prod_nb != tgen_prod_nb) {
	prod = sxba_first_difference (prod_set, tgen_prod_set);
	fputs ("This production should generate a terminal string:", stdout);
	output_prod (prod);
      }
      sxtrap (ME, "make_a_first_reduced_grammar (BOTTOM-UP CHECK FAILED)");
    }

    sxfree (t_set);
    sxfree (rhs_t_set);
    sxfree (nt_set);
    sxfree (rhs_nt_set);
    sxfree (lhs_nt_set);
    sxfree (nyp_nt_set);
    sxfree (nyp_nt_set2);
    sxfree (prod_set);
    sxfree (tgen_prod_set);
    sxfree (working_nt_stack);
    sxfree (store_npd);
    sxfree (store_numpd);
    sxfree (store_npg);
    sxfree (store_numpg);
    sxfree (prod2rhs_nt_nb);
  }
#endif /* EBUG */


#if MEASURES || LOG
  printf ("%sFirst Reduced Grammar = (|N| = %i, |T| = %i, |P| = %i(%i), |G| = %i)\n",
#if MEASURES
	  "MEASURES: "
#else /* LOG */
	  ""
#endif /* LOG */
	  , spf.insideG.maxnt
	  , -spf.insideG.maxt-1
	  , spf.insideG.maxprod
	  , unlexicalized_prod_nb
	  , spf.insideG.maxitem /* traditionnellement, c'est l'indice du 0 de la derniere regle !! */
	  );
#endif /* MEASURES || LOG */

#if LOG
  fputs ("****** Leaving make_a_first_reduced_grammar () ******\n", stdout);
#endif /* LOG */

  return new_prod_nb;
}


#if is_cycle_free
/* *************************************************************************************************
                                  C Y C L E   E L I M I N A T I O N
   ************************************************************************************************* */
/* Si la sous-grammaire cyclique n'est pas epsilon-free, il est impossible (ds le cas general) de la transformer
   en une grammaire non cyclique uniquement en supprimant des productions.
   Exemple :
   S -> A
   A -> A X
   A -> a
   X -> eps
   X -> b

   sur le source "a b"

   En fait c'est aussi vrai sur une epsilon-free :
   S -> A B
   A -> B
   A -> a
   B -> A
   B -> b

   On a L = {aa, ab, ba, bb}
   Si on supprime A -> B, on a L = {aa, ab}
   Si on supprime B -> A, on a L = {ab, bb}
*/

/* On ne peut donc pas rendre une grammaire cycle-free en se contentant de supprimer des prods !! */

/* On est apres un insideG eventuel */
/* basic_item_set contient une grammaire reduite */
/* On supprime les cycles s'il y en a (retourne true) */

/* On regarde si la grammaire est cyclique */

static bool cycle_check ()
{
  SXINT     prod, A, pA, item, X, pX, Y, pY, pB, repr;
  SXBA    pA_cycle_set, cycle_set, repr_set, line;
  bool is_cycle;
  SXINT     pA_nb;
  SXINT     *pA2A, *A2pA, *pA2min_pA;
  SXBA    *cyclic_sets;
#if is_epsilon
  SXINT     cur_item, Z, pZ
  bool X_vide, Y_vide;
  SXBA    pA_cycle_set;
#endif /* is_epsilon */
#if LOG
  SXINT     *simple_prod_stack;
#endif /* LOG */

#if LOG
  fputs ("****** Entering cycle_check () ******\n", stdout);
#endif /* LOG */

  pA2A = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT));
  A2pA = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT));

  A = 0;
  pA_nb = 0;

  while ((A = sxba_scan (basic_nt_set, A)) > 0) {
    pA_nb++;
    pA2A [pA_nb] = A;
    A2pA [A] = pA_nb;
  }

  /* cyclic_sets [pA] = {pB | A -> \alpha B \beta, \alpha\beta =>* vide} */
  cyclic_sets = sxbm_calloc (pA_nb+1, pA_nb+1);
  pA2min_pA = (SXINT*) sxcalloc (pA_nb+1, sizeof (SXINT));

#if LOG
  simple_prod_stack = (SXINT*) sxalloc (MAXPROD+1, sizeof (SXINT)), simple_prod_stack [0] = 0;
#endif /* LOG */

  item = 3;

  while ((item = sxba_scan (basic_item_set, item)) > 0) {
    prod = PROLIS (item);
    X = LISPRO (item);

    if (X > 0) {
      /* rhs non vide et commencant par un nt */
      if ((Y = LISPRO (item+1)) == 0) {
	/* 1 seul nt en rhs => candidat */
	pX = A2pA [X];
	A = LHS (PROLIS (item));
	pA = A2pA [A];
	SXBA_1_bit (cyclic_sets [pA], pX);
#if LOG
	PUSH (simple_prod_stack, prod);
#endif /* LOG */
      }
#if is_epsilon
      else {
	if (EMPTY_SET && Y > 0) {
	  /* Au moins 2 nt en rhs A -> X Y ... */
	  X_vide = SXBA_bit_is_set (EMPTY_SET, X);
	  Y_vide = SXBA_bit_is_set (EMPTY_SET.BVIDE, Y);

	  if (X_vide || Y_vide) {
	    cur_item = PROLON (prod+1)-1;
	    item++;

	    while (--cur_item > item) {
	      Z = LISPRO (cur_item);

	      if (Z < 0 || !SXBA_bit_is_set (EMPTY_SET, Z))
		break;
	    }

	    if (Z < 0 /* un terminal */
		|| (!X_vide || !Y_vide) && cur_item > item /* Au moins 2 nt non vides */
		)
	      break;
      
	    A = LHS (prod);
	    pA = A2pA [A];
	    pA_cycle_set = cyclic_sets [pA];

	    if (cur_item == item) {
	      /* le suffixe derive ds le vide */
	      if (!X_vide) {
		/* C'est X */
#if LOG
		PUSH (simple_prod_stack, prod);
#endif /* LOG */
		pX = A2pA [X];
		SXBA_1_bit (pA_cycle_set, pX);
	      }
	      else {
		if (!Y_vide) {
		  /* C'est Y */
#if LOG
		  PUSH (simple_prod_stack, prod);
#endif /* LOG */
		  pY = A2pA [Y];
		  SXBA_1_bit (pA_cycle_set, pY);
		}
		else {
		  /* toute la rhs est OK */
#if LOG
		  PUSH (simple_prod_stack, prod);
#endif /* LOG */
		  pX = A2pA [X];
		  SXBA_1_bit (pA_cycle_set, pX);
		  pY = A2pA [Y];
		  SXBA_1_bit (pA_cycle_set, pY);
		  pZ = A2pA [Z];
		  SXBA_1_bit (pA_cycle_set, pZ);

		  cur_item++; /* repere Z */

		  while ((Z = LISPRO (++cur_item)) != 0) {
		    pZ = A2pA [Z];
		    SXBA_1_bit (pA_cycle_set, pZ);
		  }
		}
	      }
	    }
	    else {
	      /* A -> X Y ... Z ... */
	      /* ... et Z ne derive pas ds le vide.
		 c'est l'unique candidat si X Y ... derive ds le vide */
	      if (X_vide && Y_vide) {
		while (++item < cur_item) {
		  X = LISPRO (item);

		  if (X < 0 || !SXBA_bit_is_set (EMPTY_SET, X))
		    break;
		}

		if (item == cur_item) {
		  /* C'est Z */
#if LOG
		  PUSH (simple_prod_stack, prod);
#endif /* LOG */
		  pZ = A2pA [Z];
		  SXBA_1_bit (pA_cycle_set, pZ);
		}
	      }
	    }
	  }
	}
      }
#endif /* is_epsilon */
    }

    item = PROLON (prod+1)-1;
  }

  fermer2 (cyclic_sets, pA_nb+1);

  cycle_set = cyclic_sets [0];
  is_cycle = false;

  for (pA = 1; pA <= pA_nb; pA++) {
    line = cyclic_sets [pA];

    if (SXBA_bit_is_set (line, pA) /* cycle autour de A ... */
	&& pA2min_pA [pA] == 0 /* ... jamais vu */) {
      /* ...et c'est le +petit du cycle du cycle */
      is_cycle = true;

#if LOG
      fputs ("**** There are cycles around: ", stdout);
#endif /* LOG */

      pB = 0;

      while ((pB = sxba_scan (line, pB)) > 0) {
	/* A =>+ B ... */
	if (SXBA_bit_is_set (cyclic_sets [pB], pA)) {
	  /* ... B =>+ A */
#if LOG
	  output_nt (NT (pA2A [pB]));
#endif /* LOG */

	  SXBA_1_bit (line, pB);
	  pA2min_pA [pB] = pA;
	}
	else
	  /* pas ds le cycle */
	  SXBA_0_bit (line, pB);
      }

#if LOG
      fputs ("\n", stdout);
#endif /* LOG */

      sxba_or (cycle_set, line);
    }
  }

#if LOG
  /* On examine maintenant les prod stockees ds simple_prod_stack */
  while (!IS_EMPTY (simple_prod_stack)) {
    prod = POP (simple_prod_stack);
    A = LHS (prod);
    pA = A2pA [A];
    repr = pA2min_pA [pA];

    if (repr) {
      /* La lhs fait partie d'un cycle */
      /* il faut regarder s'il y a un nt en rhs qui fait partie du meme cycle */
      item = PROLON (prod);

      while ((X = LISPRO (item++)) != 0) {
	/* prod A -> X */
	pX = A2pA [X];

	if (repr == pA2min_pA [pX]) {
	  fputs ("This production is cyclic: ", stdout);
	  output_prod (PROD (prod));

	  break;
	}
      }
    }
  }
#endif /* LOG */

  if (is_cycle) {
    sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
	    "%sThe lexical filtering results in a reduced cyclic sub-grammar.",
	    sxplocals.sxtables->err_titles [2]+1);
    sxexit (2);
  }

  sxbm_free (cyclic_sets), cyclic_sets = NULL;
  sxfree (pA2A), pA2A = NULL;
  sxfree (A2pA), A2pA = NULL;
  sxfree (pA2min_pA), pA2min_pA = NULL;
#if LOG
  sxfree (simple_prod_stack), simple_prod_stack = NULL;
#endif /* LOG */

#if LOG
  fputs ("****** Leaving cycle_check () ******\n", stdout);
#endif /* LOG */

  return is_cycle;
}
/* *************************************************************************************************
                                  C Y C L E   E L I M I N A T I O N  (end)
   ************************************************************************************************* */
#endif /* is_cycle_free */



#if LLOG

static void print_current_sub_grammar ()
{
  SXINT  B, new_prod, old_prod, t, old_t, old_A, item, prod;
  SXBA *nt2gen_t_set, cur_max_t_set, nt_set;

  /* On sort la nouvelle grammaire filtree par le source */
  fputs ("\n********** Current sub-grammar **********\n", stdout);

  item = 3;
  new_prod = 0;

  while ((item = sxba_scan (basic_item_set, item)) > 0) {
    prod = PROLIS (item);
    new_prod++;
    printf ("%i/", new_prod);

#ifdef MAKE_INSIDEG
    old_prod = spf.insideG.prod2init_prod [prod];
#else /* !MAKE_INSIDEG */
    old_prod = prod;
#endif /* !MAKE_INSIDEG */

    output_prod (old_prod);

    item = PROLON (prod+1)-1;
  }

  fputs ("********** End of Current sub-grammar **********\n", stdout);
}
#endif /* LLOG */

#if EBUG

static void check_reduced_sub_grammar ()
{
  /* On verifie que la grammaire est reduite */
  SXINT            top, cur, item, prod, A, X, nb, bot;
  SXINT            t_nb, rhs_t_nb, nt_nb, rhs_nt_nb, lhs_nt_nb, prod_nb, tgen_prod_nb, tgen_nt_nb;
  SXBA           t_set, nt_set, rhs_nt_set, lhs_nt_set, rhs_t_set, prod_set, tgen_prod_set, nyp_nt_set, nyp_nt_set2;
  SXINT            *working_nt_stack, *store_npd, *store_numpd, *store_npg, *store_numpg, *prod2rhs_nt_nb;
      
  fputs ("\n********** Entering check_reduced_sub-grammar **********\n", stdout);

  t_set = sxba_calloc (SXEOF+1);
  nt_set = sxba_calloc (MAXNT+1);
  rhs_nt_set = sxba_calloc (MAXNT+1);
  lhs_nt_set = sxba_calloc (MAXNT+1);
  rhs_t_set = sxba_calloc (SXEOF+1);
  prod_set = sxba_calloc (MAXPROD+1);
  tgen_prod_set = sxba_calloc (MAXPROD+1);
  nyp_nt_set = sxba_calloc (MAXNT+1);
  nyp_nt_set2 = sxba_calloc (MAXNT+1);

  working_nt_stack = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), working_nt_stack [0] = 0;
  store_npd = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT));
  store_numpd = (SXINT*) sxalloc (MAXITEM+1, sizeof (SXINT));
  store_npg = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT));
  store_numpg = (SXINT*) sxalloc (MAXPROD+MAXNT+1, sizeof (SXINT));
  prod2rhs_nt_nb = (SXINT*) sxalloc (MAXPROD+1, sizeof (SXINT));

  item = -1;

  while ((item = sxba_scan (basic_item_set, item)) >= 0) {
    prod = PROLIS (item);
    SXBA_1_bit (prod_set, prod);
    A = LHS (prod);

    if (A > 0) {
      SXBA_1_bit (nt_set, A);
	
      if (SXBA_bit_is_reset_set (nyp_nt_set2, A))
	/* 1ere A-prod */
	store_numpg [store_npg [A] = NPG (A+1)-1+A] = 0; /* On pointe vers le 0 de fin de liste (liste vide) */

      store_numpg [--store_npg [A]] = prod; /* On ajoute prod a la liste */
    }

    nb = 0;

    while ((X = LISPRO (item)) != 0) {
      if (X > 0) {
	nb++;
	SXBA_1_bit (nt_set, X);

	if (SXBA_bit_is_reset_set (nyp_nt_set, X))
	  store_numpd [store_npd [X] = NPD (X+1)-1] = 0; /* On pointe vers le 0 de fin de liste (liste vide) */

	store_numpd [--store_npd [X]] = item; /* On ajoute item a la liste */
      }
      else {
	X = -X;

	SXBA_1_bit (t_set, X);
      }

      item++;
    }

    prod2rhs_nt_nb [prod] = nb;
  }

  if (!SXBA_bit_is_set (nt_set, 1))
    sxtrap (ME, "check_reduced_sub_grammar (NO AXIOM)");

  SXBA_1_bit (rhs_t_set, SXEOF); /* Eof de la super-regle */
  SXBA_1_bit (rhs_nt_set, 1); /* Axiome ds la super-regle */
    
  working_nt_stack [0] = 0;
  PUSH (working_nt_stack, 1);

  while (!IS_EMPTY (working_nt_stack)) {
    A = POP (working_nt_stack);
    SXBA_1_bit (lhs_nt_set, A);

    bot = store_npg [A];

    while ((prod = store_numpg [bot]) != 0) {
      item = PROLON (prod);

      while ((X = LISPRO (item)) != 0) {
	if (X > 0) {
	  if (SXBA_bit_is_reset_set (rhs_nt_set, X))
	    PUSH (working_nt_stack, X);
	}
	else {
	  X = -X;
	  SXBA_1_bit (rhs_t_set, X);
	}

	item++;
      }

      bot++;
    }
  }

  t_nb = sxba_cardinal (t_set);
  rhs_t_nb = sxba_cardinal (rhs_t_set);
  nt_nb = sxba_cardinal (nt_set);
  rhs_nt_nb = sxba_cardinal (rhs_nt_set);
  lhs_nt_nb = sxba_cardinal (lhs_nt_set);

  if (t_nb != rhs_t_nb || nt_nb != rhs_nt_nb || nt_nb != lhs_nt_nb)
    sxtrap (ME, "check_reduced_sub_grammar (TOP-DOWN CHECK FAILED)");
    
  /* On verifie que chaque nt genere une chaine terminale */
  sxba_empty (lhs_nt_set);
  prod = 0;

  while ((prod = sxba_scan (prod_set, prod)) > 0) {
    item = PROLON (prod);

    while ((X = LISPRO (item)) != 0) {
      if (X > 0)
	break;

      item++;
    }

    if (X == 0) {
      /* prod genere une chaine terminale */
      SXBA_1_bit (tgen_prod_set, prod);
      A = LHS (prod);
      /* A genere une chaine terminale */

      if (SXBA_bit_is_reset_set (lhs_nt_set, A))
	PUSH (working_nt_stack, A);
    }
  }

  while (!IS_EMPTY (working_nt_stack)) {
    X = POP (working_nt_stack);

    bot = store_npd [X];

    while ((item = store_numpd [bot]) != 0) {
      prod = PROLIS (item);

      if (--prod2rhs_nt_nb [prod] == 0) {
	SXBA_1_bit (tgen_prod_set, prod);
	A = LHS (prod);
	/* A genere une chaine terminale */

	if (A > 0 && SXBA_bit_is_reset_set (lhs_nt_set, A))
	  PUSH (working_nt_stack, A);
      }

      bot++;
    }
  }

  prod_nb = sxba_cardinal (prod_set);
  tgen_prod_nb = sxba_cardinal (tgen_prod_set);
  tgen_nt_nb = sxba_cardinal (lhs_nt_set);

  if (prod_nb != tgen_prod_nb || nt_nb != tgen_nt_nb) {
    if (prod_nb != tgen_prod_nb) {
      prod = sxba_first_difference (prod_set, tgen_prod_set);
      fputs ("This production should generate a terminal string:", stdout);
      output_prod (prod);
    }
    sxtrap (ME, "check_reduced_sub_grammar (BOTTOM-UP CHECK FAILED)");
  }

  sxfree (t_set);
  sxfree (nt_set);
  sxfree (rhs_nt_set);
  sxfree (lhs_nt_set);
  sxfree (rhs_t_set);
  sxfree (prod_set);
  sxfree (tgen_prod_set);
  sxfree (nyp_nt_set);
  sxfree (nyp_nt_set2);
  sxfree (working_nt_stack);
  sxfree (store_npd);
  sxfree (store_numpd);
  sxfree (store_npg);
  sxfree (store_numpg);
  sxfree (prod2rhs_nt_nb);
      
  fputs ("\n********** Leaving check_reduced_sub_grammar **********\n", stdout);
}
#endif /* EBUG */


#if NRG==1
#ifdef MAKE_INSIDEG
#define NRG_NPD(X)   NPD (X)
#define NRG_NUMPD(X) NUMPD (X)
#define NRG_NPG(X)   NPG (X)
#define NRG_NUMPG(X) NUMPG (X)
#else /* ifndef MAKE_INSIDEG */
#define NRG_NPD(X)   nrg_store_npd [X]
#define NRG_NUMPD(X) nrg_store_numpd [X]
#define NRG_NPG(X)   nrg_store_npg [X]
#define NRG_NUMPG(X) nrg_store_numpg [X]
#endif /* ifndef MAKE_INSIDEG */

static void nrg_allocate ()
{
  sxprepare_for_possible_reset (nrg_working_nt_set);
  nrg_working_nt_set = sxba_calloc (MAXNT+1);
  nrg_tgen_nt_set = sxba_calloc (MAXNT+1);
  nrg_working_nt_stack = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT));
#ifndef MAKE_INSIDEG
  nrg_nyp_nt_set = sxba_calloc (MAXNT+1);
  nrg_store_npd =  (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT));
  nrg_store_numpd = (SXINT*) sxalloc (MAXITEM+1, sizeof (SXINT));
  nrg_nyp_nt_set2 = sxba_calloc (MAXNT+1);
  nrg_store_npg =  (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT));
  nrg_store_numpg = (SXINT*) sxalloc (MAXPROD+MAXNT+1, sizeof (SXINT)); /* Les listes se terminent par des zeros */
#endif  /* MAKE_INSIDEG */
}

static void nrg_free ()
{
  sxfree (nrg_working_nt_set), nrg_working_nt_set = NULL;
  sxfree (nrg_tgen_nt_set), nrg_tgen_nt_set = NULL;
  sxfree (nrg_working_nt_stack), nrg_working_nt_stack = NULL;
#ifndef MAKE_INSIDEG
  sxfree (nrg_nyp_nt_set), nrg_nyp_nt_set = NULL;
  sxfree (nrg_store_npd), nrg_store_npd = NULL;
  sxfree (nrg_store_numpd), nrg_store_numpd = NULL;
  sxfree (nrg_nyp_nt_set2), nrg_nyp_nt_set2 = NULL;
  sxfree (nrg_store_npg), nrg_store_npg = NULL;
  sxfree (nrg_store_numpg), nrg_store_numpg = NULL;
#endif  /* MAKE_INSIDEG */
}


/* Analogue a make_a_first_reduced_grammar excepte' que basic_item_set peut referencer des item de insideG */
/* basic_item_set et basic_nt_set sont des SXBA globaux (les analyseurs comme earley peuvent y acceder) */
/* Filtre basic_item_set pour ne laisser que la + grande sous-grammaire reduite ... */
/* ... et, reremplit basic_nt_set avec les nt de cette sous-grammaire */
/* Retourne new_prod_nb */

static SXINT make_a_next_reduced_grammar ()
{
  SXINT             item, nb, X, prod, A, bot, top, bot_item, new_prod_nb = 0, old_X, old_prod, cur, cur_item, nrg_init_tprod_stack_top;
  bool         ret_val;
#if MEASURES || LOG
  SXINT             nt_nb = 0, t_nb = 0, item_nb = 0, rhst_nb, unlexicalized_prod_nb = 0;
  SXBA            t_set;
#endif /* MEASURES || LOG */

#if LOG
  fputs ("****** Entering make_a_next_reduced_grammar () ******\n", stdout);
#endif /* LOG */

  if (nrg_working_nt_set == NULL) {
    nrg_allocate ();

#ifndef MAKE_INSIDEG
    /* La 1ere fois, on fabrique les nrg_store  */
    /* Sinon on va utiliser ceux de insideG */
    item = 3; /* On saute [S' -> $ S $] ... */

    while ((item = sxba_scan (basic_item_set, item)) >= 0) {
      prod = PROLIS (item);
      A = LHS (prod);
	
      if (SXBA_bit_is_reset_set (nrg_nyp_nt_set2, A))
	/* 1ere A-prod */
	nrg_store_numpg [nrg_store_npg [A] = NPG (A+1)-1+A] = 0; /* On pointe vers le 0 de fin de liste (liste vide) */

      nrg_store_numpg [--nrg_store_npg [A]] = prod; /* On ajoute prod a la liste */

      while ((X = LISPRO (item)) != 0) {
	if (X > 0
#if is_epsilon
	    && EMPTY_SET && !SXBA_bit_is_set (EMPTY_SET, X)
	    /* Si X =>+ vide, on ne repere pas ces X en RHS */
#endif /* is_epsilon */
	    ) {
	  if (SXBA_bit_is_reset_set (nrg_nyp_nt_set, X))
	    nrg_store_numpd [nrg_store_npd [X] = NPD (X+1)-1] = 0; /* On pointe vers le 0 de fin de liste (liste vide) */

	  nrg_store_numpd [--nrg_store_npd [X]] = item; /* On ajoute item a la liste */
	}

	item++;
      }
    }
#endif  /* MAKE_INSIDEG */
  }
  else {
    sxba_empty (nrg_working_nt_set);
    sxba_empty (nrg_tgen_nt_set);
  }

   /* calcul des nt qui peuvent deriver ds t_set^* */
   RAZ (nrg_working_nt_stack);
   nrg_prod2rhs_nt_nb = (SXINT*) sxcalloc (MAXPROD+1, sizeof (SXINT));

   if (nrg_init_tprod_stack) {
     nrg_init_tprod_stack_top = TOP (nrg_init_tprod_stack);

     for (cur = 1; cur <= nrg_init_tprod_stack_top; cur++) {
       prod = nrg_init_tprod_stack [cur];
       item = PROLON (prod);

       if (SXBA_bit_is_set (basic_item_set, item)) {
	 A = LHS (prod);
	 
	 if (SXBA_bit_is_reset_set (nrg_working_nt_set, A))
	   PUSH (nrg_working_nt_stack, A);

	 nrg_prod2rhs_nt_nb [prod] = -1; 
#if LLOG
	 fputs ("This production generates a non empty terminal string:", stdout);
#ifdef MAKE_INSIDEG
	 prod = spf.insideG.prod2init_prod [prod];
#endif /* ifdef MAKE_INSIDEG */
	 output_prod (prod);
#endif /* LLOG */
       }
     }
   }

  /* Ici working_nt_stack contient une liste des nt A tq A=>+ x, x \in t_set^+ (non vide) et A (par ailleurs) ne derive pas ds le vide */
  /* tgen_nt_set garde working_nt_stack */
  while (!IS_EMPTY (nrg_working_nt_stack)) {
    X = POP (nrg_working_nt_stack);
    /* X genere une chaine terminale ... */
    SXBA_1_bit (nrg_tgen_nt_set, X);

#if LLOG
#  ifdef MAKE_INSIDEG
    old_X = spf.insideG.nt2init_nt [X];
#  else /* ifndef MAKE_INSIDEG */
    old_X = X;
#  endif /* ifndef MAKE_INSIDEG */
    printf ("Seeking for RHS occurrences of <%s>\n", spf.inputG.ntstring [old_X]);
#endif /* LLOG */

#ifndef MAKE_INSIDEG
    if (SXBA_bit_is_set (nrg_nyp_nt_set, X)) {
#endif /* !MAKE_INSIDEG */
      bot = NRG_NPD (X);

      while (item = NRG_NUMPD (bot)) {
	if (SXBA_bit_is_set (basic_item_set, item)) {
	  prod = PROLIS (item);
	  /* ... et l'on a item/prod A -> \alpha .X \beta */

	  if (prod > 0) {
	    /* On est su^r que prod n'est pas la super-re`gle */
	    A = LHS (prod);

#if is_epsilon
	    if (EMPTY_SET && !SXBA_bit_is_set (EMPTY_SET, A))
	      /* Si A =>+ vide, on ne met pas A ds tgen_nt_set */
#endif /* is_epsilon */
	      {
		if (!SXBA_bit_is_set (nrg_working_nt_set, A)) {
		  /* On ne sait encore rien sur A */
		  nb = nrg_prod2rhs_nt_nb [prod];
#if EBUG
		  if (nb < 0)
		    /* A =>+ T^+ */
		    sxtrap (ME, "make_a_next_reduced_grammar");
#endif /* EBUG */

		  if (nb == 0) {
		    /* prod jamais vue (et non lexicalisee */
		    cur_item = PROLON (prod);

		    while ((X = LISPRO (cur_item)) != 0) {
		      if (X > 0) {
			if (!SXBA_bit_is_set (nrg_tgen_nt_set, X)
#if is_epsilon
			    && EMPTY_SET && !SXBA_bit_is_set (EMPTY_SET, X)
			    /* Si X =>+ vide, on ne le compte pas */
#endif /* is_epsilon */
			    )
			  nb++;
		      }

		      cur_item++;  
		    }
		  }
		  else {
		    nb--;
		  }

		  if (nb == 0) {
		    if (SXBA_bit_is_reset_set (nrg_working_nt_set, A))
		      PUSH (nrg_working_nt_stack, A);
#if LLOG
		    fputs ("This production generates a non empty terminal string:", stdout);
#  ifdef MAKE_INSIDEG
		    old_prod = spf.insideG.prod2init_prod [prod];
#  else /* ifndef MAKE_INSIDEG */
		    old_prod = prod;
#  endif /* ifndef MAKE_INSIDEG */
		    output_prod (old_prod);
#endif /* LLOG */
	       
		    nb = -1;
		  }

		  nrg_prod2rhs_nt_nb [prod] = nb;
		}
	      }
	  }
	}

	bot++;
      }
#ifndef MAKE_INSIDEG
    }
#endif /* !MAKE_INSIDEG */
  }

#if is_epsilon
  if (EMPTY_SET && SXBA_bit_is_set (EMPTY_SET, 1))
    /* Si eps est ds le langage on dit que l'axiome genere une chaine terminale (vide) */
    SXBA_1_bit (nrg_tgen_nt_set, 1);
#endif /* is_epsilon */

   /* Ici nrg_tgen_nt_set contient l'ensemble des nt A tq A=>+ x, x \in t_set^* */
  if (SXBA_bit_is_set (nrg_tgen_nt_set, 1)) {
    /* On a donc S =>+ x, x \in t_set^* */
    /* On vire les productions qui ne generent pas des chaines terminales ou qui sont inaccessibles depuis l'axiome */
    SXBA new_basic_item_set;

    ret_val = true; /* a priori */


#if MEASURES || LOG
    t_set = sxba_calloc (SXEOF+1);
    /* nt_nb = 0; init */
    t_nb = 1; /* eof */
    item_nb = 4; /* super-regle */
#endif /* MEASURES || LOG */

    /* On reremplit tout */
    new_basic_item_set = sxba_calloc (MAXITEM+1);
    sxba_empty (basic_nt_set);
#ifndef MAKE_INSIDEG
    sxba_empty (lex_source_set);
    SXBA_1_bit (lex_source_set, SXEOF);
#endif /* ndef MAKE_INSIDEG */		  

    sxba_range (new_basic_item_set, 0, 4); /* Les items de la super-regle */
    SXBA_1_bit (basic_nt_set, 1); /* l'axiome */
    PUSH (nrg_working_nt_stack, 1);

    while (!IS_EMPTY (nrg_working_nt_stack)) {
      A = POP (nrg_working_nt_stack);
#if MEASURES || LOG
      nt_nb++;
#endif /* MEASURES || LOG */

      /* Ici on a S =>* A =>+ x */
      bot = NRG_NPG (A);

#ifdef MAKE_INSIDEG
      top = spf.insideG.npg [A+1];

      while (bot < top)
#else /* ifndef MAKE_INSIDEG */
      while (prod = NRG_NUMPG (bot))
#endif  /* MAKE_INSIDEG */
	{
#ifdef MAKE_INSIDEG
	  prod = spf.insideG.numpg [bot];
#else /* ifndef MAKE_INSIDEG */
#endif  /* MAKE_INSIDEG */

	  bot_item = item = PROLON (prod);

	  if (SXBA_bit_is_set (basic_item_set, item)) {
	    while ((X = LISPRO (item++)) != 0) {
	      if (X > 0 /* nt */
		  && !SXBA_bit_is_set (nrg_tgen_nt_set, X) /* ne derive pas ds un x */
#if is_epsilon
		  && EMPTY_SET && !SXBA_bit_is_set (EMPTY_SET, X)
		  /* Si X =>+ vide, on ne le compte pas */
#endif /* is_epsilon */
		  ) {
		/* ... prod doit donc etre rejetee */
		break;
	      }
	    }

	    if (X == 0) {
	      /* RHS =>* x */
#if MEASURES || LOG
	      item_nb++;
	      rhst_nb = 0;
#endif /* MEASURES || LOG */
	      new_prod_nb++;
	      item = bot_item;
	      SXBA_1_bit (new_basic_item_set, item);

	      while ((X = LISPRO (item)) != 0) {
		if (X > 0) {
		  if (SXBA_bit_is_reset_set (basic_nt_set, X)) {
		    PUSH (nrg_working_nt_stack, X);
		  }
		}
		else {
		  X = -X; /* ne sert à rien si ni ndef MAKE_INSIDEG, ni MEASURES, ni LOG, mais bon
			     tant pis */
#ifndef MAKE_INSIDEG
		  SXBA_1_bit (lex_source_set, X);
#endif /* ndef MAKE_INSIDEG */		  
#if MEASURES || LOG
		  rhst_nb++;

		  if (SXBA_bit_is_reset_set (t_set, X)) {
#ifndef MAKE_INSIDEG
		    SXBA_1_bit (lex_source_set, X);
#endif /* ndef MAKE_INSIDEG */	
		    t_nb++;
		  }
#endif /* MEASURES || LOG */
		}

#if MEASURES || LOG
		item_nb++;
#endif /* MEASURES || LOG */
		item++;
		SXBA_1_bit (new_basic_item_set, item);
	      }

#if MEASURES || LOG
	      if (rhst_nb == 0) unlexicalized_prod_nb++;
#endif /* MEASURES || LOG */
	    }
	  }
	 
	  bot++;
	}
    }

#if MEASURES || LOG
    sxfree (t_set);
#endif /* MEASURES || LOG */

    sxfree (basic_item_set);
    basic_item_set = new_basic_item_set;
  }
  else {
    ret_val = false;

#if LOG
    fputs ("\n********** WARNING: This grammar is not reduced **********\n", stdout);
#endif /* LOG */
  }

#if LLOG
  print_current_sub_grammar ();
#endif /* LLOG */

#if EBUG
  if (ret_val)
    check_reduced_sub_grammar ();
#endif /* EBUG */


#if MEASURES || LOG
  printf ("%sNext Reduced Grammar = (|N| = %i, |T| = %i, |P| = %i(%i), |G| = %i)\n",
#if MEASURES
	  "MEASURES: "
#else /* LOG */
	  ""
#endif /* LOG */
	  , nt_nb
	  , t_nb-1 /* On enleve eof */
	  , new_prod_nb
	  , unlexicalized_prod_nb
	  , item_nb-1 /* traditionnellement, c'est l'indice do 0 de la derniere regle !! */
	  );
#endif /* MEASURES || LOG */

#if LOG
  fputs ("****** Leaving make_a_next_reduced_grammar () ******\n", stdout);
#endif /* LOG */

  sxfree (nrg_prod2rhs_nt_nb), nrg_prod2rhs_nt_nb = NULL;

  return new_prod_nb;
}
#endif /* NRG==1 */



#if MEASURES || LOG
/* Appelee aussi depuis earley_parser */

void output_G (char *header, SXBA basic_item_set, SXBA basic_prod_set)
{
  SXINT  old_prod, t_nb = 0, nt_nb = 0, prod_nb = 0, item_nb = 0;
  SXINT  item, prod, item_or_prod, X;
  SXBA t_set, nt_set;

#if LOG
  fputs ("****** Entering output_G () ******\n", stdout);
#endif /* LOG */

  nt_set = sxba_calloc (MAXNT+1);
  t_set = sxba_calloc (SXEOF+1);

  item_or_prod = basic_item_set ? 3 : 0;

  while ((item_or_prod = sxba_scan (basic_item_set ? basic_item_set : basic_prod_set, item_or_prod)) >= 0) {
    if (basic_item_set) {
      item = item_or_prod;
      prod = PROLIS (item);
    }
    else {
      prod = item_or_prod;
      item = PROLON (prod);
    }

    prod_nb++;
    item_nb++;

    X = LHS (prod);
	
    if (SXBA_bit_is_reset_set (nt_set, X))
      nt_nb++;

#if LLOG
#ifdef MAKE_INSIDEG
    old_prod = spf.insideG.prod2init_prod [prod];
#else /* ifndef MAKE_INSIDEG */
    old_prod = prod;
#endif /* ifndef MAKE_INSIDEG */
    printf ("%i/", prod_nb-1);
    output_prod (old_prod);
#endif /* LLOG */

    while ((X = LISPRO (item)) != 0) {
      if (X > 0) {
	if (SXBA_bit_is_reset_set (nt_set, X))
	  nt_nb++;
      }
      else {
	X = -X;

	if (SXBA_bit_is_reset_set (t_set, X))
	  t_nb++;
      }

      item_nb++;
      item++;
    }

    if (basic_item_set) {
      item_or_prod = item;
    }
  }
  
  sxfree (nt_set);
  sxfree (t_set);

  printf ("%s%s = (|N| = %i, |T| = %i, |P| = %i, |G| = %i)\n",
#if MEASURES
	  "MEASURES: "
#else /* LOG */
	  ""
#endif /* LOG */
	  , header
	  , nt_nb
	  , t_nb
	  , prod_nb
	  , item_nb /* traditionnellement, c'est l'indice do 0 de la derniere regle !! */);

#if LOG
  fputs ("****** Leaving output_G () ******\n", stdout);
#endif /* LOG */
}
#else /* MEASURES || LOG */

void output_G (char *header, SXBA basic_item_set, SXBA basic_prod_set)
{
  fputs ("****** Entering and immediately leaving dummy output_G () ******\n", stdout);
}
#endif /* MEASURES || LOG */


#if is_set_automaton
static SXBA
  source_t_set /* SXEOF+1 */
  , int_set /* MAXNT+1 */
  , nt_set /* MAXNT+1 */
  , new_rl_control_set /* MAXITEM+1 */
  , closure_set /* MAXITEM+1 */
  , or_and_t_set /* SXEOF+1 */
  , working_titem_set /* MAXITEM+1 */
  ;

static SXINT
  *nt_stack /* MAXNT+1 */
  ;

/* "parseur" fonde' sur un set-automaton (la structure de controle est un ensemble et pas une pile)
   C'est un automate fini
   Analyse en temps lineaire */
/* Il peut etre utilise comme shallow parseur (option -Dis_shallow=1) et/ou comme supertaggeur
   (option -Dis_supertagging=1) ou comme "lexicaliseur" (option -Dis_lex=1) */

static SXBA           rl_valid_item_set_i, lr_valid_item_set_i, cur_titem_set, new_basic_item_set;
static SXBA           *mlstn2lexicalized_look_ahead_t_set;

/* On utilise la relation left_corner (et right_corner) */
/* valid_prefixes avec nt2lc_item_set [1] */
/* A chaque etape, si c'est la 1ere fois qu'on voit un A, on ajoute
   nt2lc_item_set [A] */

/* left_corner = {(A, B) | A =>* \alpha B \beta =>* B \beta} */
/* nt2lc_item_set [A] = {B -> \alpha . \beta | B left_corner A et \alpha =>* \epsilon} */

/* right_corner = {(A, B) | A =>* \alpha B \beta =>* \alpha B} */
/* nt2rc_item_set [A] = {B -> \alpha . \beta | B right_corner A et \beta =>* \epsilon} */

/* valeurs initiales ds les tables */
/* titem_set = {A -> \alpha .t \beta | A -> \alpha t \beta \in P} */
/* rc_titem_set = {A -> \alpha t . \beta | A -> \alpha t \beta \in P} */


/* NOUVELLE VERSION
   On fait un passage gauche droite qui note pour chaque position du source l'active set calcule.
   Puis, par un passage droite gauche, on ne conserve ds chaque active set que les items connectes
   a l'item final [S' -> $ S. $]
   C'est +precis que set_automaton_parser () => on conserve
   -Dis_lrsa est donc sans effet...
 */

/* Structure intermediaire qui a chaque terminal t qui intervient ds une transition du dag depuis (ou vers)
   un noeud, associe l'ensemble des triplets (lpos, tok_no, rpos).  Bien sur on a lahead(tok_no)==t */
static SXBA           next_mlstn_set;
static SXBA           active_set, control_set, rl_active_set, rl_control_set;
static SXBA           *active_sets, *control_sets, *rl_active_sets, *rl_control_sets;



/* On vient de reconnaitre U (item = A -> \alpha U . \beta) */
/* Changement le 09/10/06 Si \beta == \varepsilon, on met item ds active_set sinon on le met ds closure_set
   il pourra comme c,a etre filtre'par la lexicalization */

static void close_reduce (SXINT item
#if LLOG
	      , SXINT mlstn
#endif /* LLOG */
	      )
{
  SXINT X, prod, bot, A;

  X = LISPRO (item);

  if (IS_EPSILON) {
    while (X != 0) {
      SXBA_1_bit (closure_set, item);

      if (X < 0) {
	return;
      }

      if (SXBA_bit_is_reset_set (int_set, X)) {
	/* C'est la 1ere fois qu'on ferme X au niveau i */
	sxba_or (closure_set, NT2ITEM_SET (X));
      }

      if (!SXBA_bit_is_set (EMPTY_SET, X))
	return;

      item++;
      SXBA_1_bit (closure_set, item);
      X = LISPRO (item);
    }
  }
  else { /* !is_epsilon */
    SXBA_1_bit (closure_set, item);

    if (X != 0) {
      if (X > 0 && SXBA_bit_is_reset_set (int_set, X)) {
	/* C'est la 1ere fois qu'on ferme X au niveau i */
	sxba_or (closure_set, NT2ITEM_SET (X));
      }

      return;
    }
  } /* !is_epsilon */

  /* On a reconnu prod */
  /* Elle couvre une sous-chaine non vide */
  SXBA_1_bit (active_set, item);
  prod = PROLIS (item);

#if LLOG
  {
    SXINT old_prod;

#ifdef MAKE_INSIDEG
    old_prod = spf.insideG.prod2init_prod [prod];
#else /* ifndef MAKE_INSIDEG */
    old_prod = prod;
#endif /* ifndef MAKE_INSIDEG */

    printf ("Pass LR [?..%i] recognizes production ", mlstn);
    output_prod (old_prod);
  }
#endif /* LLOG */

  A = LHS (prod);

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    /* premiere reduction vers A */
    PUSH (nt_stack, A);
  }
}

/* On vient de reconnaitre U (item = A -> \alpha . U \beta) */

static void rl_close_reduce (SXINT item
#if LLOG
		 , SXINT mlstn
#endif /* LLOG */
		 )
{
  SXINT X, prod, A, bot;

  SXBA_1_bit (closure_set, item);
  bot = PROLON (prod = PROLIS (item));

  if (IS_EPSILON) {
    while (item > bot) {
      X = LISPRO (item-1);

      if (X < 0) {
	return;
      }

      if (SXBA_bit_is_reset_set (int_set, X)) {
	/* C'est la 1ere fois qu'on ferme X au niveau i */
	sxba_or (closure_set, NT2RC_ITEM_SET (X));
      }

      if (!SXBA_bit_is_set (EMPTY_SET, X))
	return;

      item--;
      SXBA_1_bit (closure_set, item);
    }
  }
  else { /* !is_epsilon */
    if (item > bot) {
      X = LISPRO (item-1);

      if (X > 0 && SXBA_bit_is_reset_set (int_set, X)) {
	/* C'est la 1ere fois qu'on ferme X au niveau i */
	sxba_or (closure_set, NT2RC_ITEM_SET (X));
      }

      return;
    }
  } /* !is_epsilon */

  /* On a reconnu prod */
  /* Elle couvre une sous-chaine non vide ... */
  /* ...et a ete validee par la passe gauche droite */

#if LLOG
  {
    SXINT old_prod;

#ifdef MAKE_INSIDEG
    old_prod = spf.insideG.prod2init_prod [prod];
#else /* ifndef MAKE_INSIDEG */
    old_prod = prod;
#endif /* ifndef MAKE_INSIDEG */

    printf ("Pass RL [%i..?] recognizes production ", mlstn);
    output_prod (old_prod);
  }
#endif /* LLOG */

  if (!SXBA_bit_is_set (new_basic_item_set, bot))
    sxba_range (new_basic_item_set, bot, PROLON (prod+1)-bot);

  A = LHS (prod);

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    PUSH (nt_stack, A);
  }
}


/* Nouvelles versions (12/02/08) qui utilisent idag */
/* remplit les control et active sets des etats du dynamic set automaton atteints depuis mlstn 
   de gauche a droite */
/* Nelle version du 05/10/06 qui marche pour is_dag ou !is_dag */

static bool sa_scanner (SXINT mlstn)
{
  SXINT     item, t, next_mlstn, bot_pq, top_pq, trans;
  SXINT     *t_stack, *top_t_stack;
  bool   has_forward_ttrans = false;
  SXBA      next_active_set;

  /* active_set = active_sets [mlstn]; */
  sxba_2op (cur_titem_set, SXBA_OP_COPY, active_set, SXBA_OP_AND, TITEM_SET);
  /* cur_titem_set est donc la partie de l'active set qui ne contient que des titems de la forme
     A -> \alpha .t \beta */
  item = MAXITEM+1;

  while ((item = sxba_1_rlscan (cur_titem_set, item)) >= 0) {
    SXBA_0_bit (cur_titem_set, item);
    SXBA_1_bit (cur_titem_set, item+1); /* On pointe derriere les terminaux */
    t = -LISPRO (item);
    SXBA_1_bit (source_t_set, t);
  }

  bot_pq = idag.p2pq_hd [mlstn];
  top_pq = idag.p2pq_hd [mlstn+1];

  while (bot_pq < top_pq) {
    next_mlstn = idag.pq2q [bot_pq];
    sxba_or (control_sets [next_mlstn], control_set);
    next_active_set = active_sets [next_mlstn];

    t_stack = idag.t_stack + idag.pq2t_stack_hd [bot_pq];
    top_t_stack = t_stack +*t_stack;

    while (++t_stack <= top_t_stack) {
      t = *t_stack;

      if (SXBA_bit_is_set (source_t_set, t)) {
	has_forward_ttrans = true; /* Au moins un des next_active_set non vide */
	sxba_2op (next_active_set, SXBA_OP_OR, cur_titem_set, SXBA_OP_AND, T2PROD_ITEM_SET (t));
      }
    }

    bot_pq++;
  }

  /* Clear */
  sxba_empty (source_t_set);

  return has_forward_ttrans;
}

/* remplit les control et active sets des etats du dynamic set automaton atteints depuis mlstn 
 de droite a gauche */
/* Nelle version du 05/10/06 qui marche pour is_dag ou !is_dag */

static bool sa_rlscanner (SXINT mlstn)
{
  SXINT     item, prev_item, t, prev_mlstn, pq;
  SXINT     *t_stack, *top_t_stack, *pq_stack, *top_pq_stack;
  bool   has_backward_ttrans = false;
  SXBA      prev_active_set;

  /* rl_active_set = rl_active_sets [mlstn]; */
  sxba_2op (cur_titem_set, SXBA_OP_COPY, rl_active_set, SXBA_OP_AND, RC_TITEM_SET);
  /* cur_titem_set est la partie de l'active set qui ne contient que des titems de la forme
     A -> \alpha t. \beta */
  item = -1;

  while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
    prev_item = item-1;
    SXBA_1_bit (cur_titem_set, prev_item); /* On pointe devant les terminaux */
    t = -LISPRO (prev_item);
    SXBA_1_bit (source_t_set, t);
  }

  pq_stack = idag.q2pq_stack + idag.q2pq_stack_hd [mlstn];
  top_pq_stack = pq_stack + *pq_stack;

  while (++pq_stack <= top_pq_stack) {
    pq = *pq_stack;
    prev_mlstn = range2p (pq);

    sxba_or (rl_control_sets [prev_mlstn], new_rl_control_set);
    prev_active_set = rl_active_sets [prev_mlstn];

    t_stack = idag.t_stack + idag.pq2t_stack_hd [pq];
    top_t_stack = t_stack +*t_stack;

    while (++t_stack <= top_t_stack) {
      t = *t_stack;

      if (SXBA_bit_is_set (source_t_set, t)) {
	has_backward_ttrans = true; /* Au moins un des next_active_set non vide */

	/* Ca servira ds recognize () */
	if (mlstn2lexicalized_look_ahead_t_set)
	  SXBA_1_bit (mlstn2lexicalized_look_ahead_t_set [prev_mlstn], t);

	sxba_2op (prev_active_set, SXBA_OP_OR, cur_titem_set, SXBA_OP_AND, T2PROD_ITEM_SET (t));
      }
    }
  }

  sxba_empty (source_t_set);

  return has_backward_ttrans;
}




#if 0
/* Anciennes versions */
/* remplit les control et active sets des etats du dynamic set automaton atteints depuis mlstn 
 de gauche a droite */
/* Nelle version du 05/10/06 qui marche pour is_dag ou !is_dag */

static bool sa_scanner (SXINT mlstn)
{
  SXINT     dag_state, trans, item, t, next_mlstn, Y, next_item;
  bool   has_forward_ttrans = false;
  SXBA      next_active_set;

  /* active_set = active_sets [mlstn]; */
  sxba_2op (cur_titem_set, SXBA_OP_COPY, active_set, SXBA_OP_AND, TITEM_SET);
  /* cur_titem_set est donc la partie de l'active set qui ne contient que des titems de la forme
     A -> \alpha .t \beta */

#if is_dag
  /* On en extrait l'ensemble de ces terminaux */
  item = MAXITEM+1;

  while ((item = sxba_1_rlscan (cur_titem_set, item)) >= 0) {
    SXBA_0_bit (cur_titem_set, item);
    SXBA_1_bit (cur_titem_set, item+1); /* On pointe derriere les terminaux */
    t = -LISPRO (item);
    SXBA_1_bit (source_t_set, t);
  }

  dag_state = mlstn;

  XxYxZ_Xforeach (dag_hd, dag_state, trans) {
    /* attention le DAG peut etre non-deterministe par rapport aux t */
    Y = XxYxZ_Y (dag_hd, trans);

    if (
#if is_sdag
	sxba_2op (or_and_t_set, SXBA_OP_COPY, glbl_source [Y], SXBA_OP_AND, source_t_set)
#else /* !is_sdag */
	t = glbl_source [Y], SXBA_bit_is_set (source_t_set, t)
#endif /* !is_sdag */
	) {
      has_forward_ttrans = true;
      next_mlstn = XxYxZ_Z (dag_hd, trans);

      if (SXBA_bit_is_reset_set (next_mlstn_set, next_mlstn))
	sxba_or (control_sets [next_mlstn], control_set);

      next_active_set = active_sets [next_mlstn];

#if is_sdag
      t = -1;

      while ((t = sxba_scan (or_and_t_set, t)) >= 0) {
	sxba_2op (next_active_set, SXBA_OP_OR, cur_titem_set, SXBA_OP_AND, T2PROD_ITEM_SET (t));
      }
#else /* !is_sdag */
      sxba_2op (next_active_set, SXBA_OP_OR, cur_titem_set, SXBA_OP_AND, T2PROD_ITEM_SET (t));
#endif /* !is_sdag */
    }		  
  }

  /* Clear */
  if (has_forward_ttrans)
    sxba_empty (next_mlstn_set);

  sxba_empty (source_t_set);

#else /* !is_dag */
  next_mlstn = mlstn+1;
  next_active_set = active_sets [next_mlstn];

#if is_sdag
  glbl_source_i = glbl_source [mlstn];
#endif /* is_sdag */
  item = -1;

  while ((item = sxba_scan (cur_titem_set, item)) >= 0) {
    t = -LISPRO (item);

    if (
#if is_sdag
	SXBA_bit_is_set (glbl_source_i, t)
#else /* !is_sdag */
	glbl_source [mlstn] == t
#endif /* !is_sdag */
	) {
      has_forward_ttrans = true;
      next_item = item+1;
      SXBA_1_bit (next_active_set, next_item); /* A -> \alpha a_i . \beta */
    }
  }

  if (has_forward_ttrans) 
    sxba_or (control_sets [next_mlstn], control_set);
#endif /* !is_dag */

  return has_forward_ttrans;
}

/* remplit les control et active sets des etats du dynamic set automaton atteints depuis mlstn 
 de droite a gauche */
/* Nelle version du 05/10/06 qui marche pour is_dag ou !is_dag */

static bool sa_rlscanner (SXINT mlstn)
{
  SXINT     trans, item, prev_item, t, prev_mlstn, Y;
  bool has_backward_ttrans = false;
  SXBA    prev_active_set, look_ahead_t_set;

  /* rl_active_set = rl_active_sets [mlstn]; */
  sxba_2op (cur_titem_set, SXBA_OP_COPY, rl_active_set, SXBA_OP_AND, RC_TITEM_SET);
  /* cur_titem_set est la partie de l'active set qui ne contient que des titems de la forme
     A -> \alpha t. \beta */

#if is_dag
  /* On en extrait l'ensemble de ces terminaux */
  item = -1;

  while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
    prev_item = item-1;
    SXBA_1_bit (cur_titem_set, prev_item); /* On pointe devant les terminaux */
    t = -LISPRO (prev_item);
    SXBA_1_bit (source_t_set, t);
  }

  /* Extractions des transitions du DAG depuis mlstn */
  XxYxZ_Zforeach (dag_hd, mlstn, trans) {
    /* attention le DAG peut etre non-deterministe par rapport aux t */
    Y = XxYxZ_Y (dag_hd, trans);
    prev_mlstn = XxYxZ_X (dag_hd, trans);

    if (
#if is_sdag
	sxba_2op (or_and_t_set, SXBA_OP_COPY, glbl_source [Y], SXBA_OP_AND, source_t_set)
#else /* !is_sdag */
	t = glbl_source [Y], SXBA_bit_is_set (source_t_set, t)
#endif /* !is_sdag */
	) {
      has_backward_ttrans = true;

      if (SXBA_bit_is_reset_set (next_mlstn_set, prev_mlstn))
	sxba_or (rl_control_sets [prev_mlstn], new_rl_control_set);

      prev_active_set = rl_active_sets [prev_mlstn];

#if is_sdag
      /* Ca servira ds recognize () */
      if (mlstn2lexicalized_look_ahead_t_set)
	sxba_or (mlstn2lexicalized_look_ahead_t_set [prev_mlstn], or_and_t_set);

      t = -1;

      while ((t = sxba_scan (or_and_t_set, t)) >= 0) {
	sxba_2op (working_titem_set, SXBA_OP_OR, cur_titem_set, SXBA_OP_AND, T2PROD_ITEM_SET (t));
      }
#else /* !is_sdag */
      sxba_2op (working_titem_set, SXBA_OP_OR, cur_titem_set, SXBA_OP_AND, T2PROD_ITEM_SET (t));

      /* Ca servira ds recognize () */
      if (mlstn2lexicalized_look_ahead_t_set)
	SXBA_1_bit (mlstn2lexicalized_look_ahead_t_set [prev_mlstn], t);
#endif /* !is_sdag */

      sxba_or (prev_active_set, working_titem_set);
    }		  
  }

  /* Clear */
  if (has_backward_ttrans)
    sxba_empty (next_mlstn_set);

  sxba_empty (source_t_set);
  sxba_empty (cur_titem_set);
#else /* !is_dag */
  prev_mlstn = mlstn-1;
  prev_active_set = rl_active_sets [prev_mlstn];

  /* Ca servira ds recognize () */
  if (mlstn2lexicalized_look_ahead_t_set)
    look_ahead_t_set = mlstn2lexicalized_look_ahead_t_set [prev_mlstn];

#if is_sdag
  glbl_source_i = glbl_source [prev_mlstn];
#endif /* is_sdag */

  item = -1;

  while ((item = sxba_scan (cur_titem_set, item)) >= 0) {
    prev_item = item-1;
    t = -LISPRO (prev_item);

    if (
#if is_sdag
	SXBA_bit_is_set (glbl_source_i, t)
#else /* !is_sdag */
	glbl_source [prev_mlstn] == t
#endif /* !is_sdag */
	) {
      has_backward_ttrans = true;
      SXBA_1_bit (prev_active_set, prev_item); /* A -> \alpha . a_i \beta */

      if (mlstn2lexicalized_look_ahead_t_set)
	SXBA_1_bit (look_ahead_t_set, t);
    }
  }

  if (has_backward_ttrans) {
    sxba_or (rl_control_sets [prev_mlstn], new_rl_control_set);
  }
#endif /* !is_dag */

  return has_backward_ttrans;
}
#endif /* 0 */



static bool lr_pass_succeeds;


/* Ici on est ds le cas is_dag ou !is_dag */
/* Si l'appelant veut associer a chaque dag_state l'ensemble des terminaux selectionnes par le
   dynamic_set_automaton, il passe mlstn2lex_la_tset non vide qui sera rempli */

static bool dynamic_set_automaton ()
{
  SXINT  t, item, item_1, X, i, A, bot, top, prod, mlstn; 
  SXBA supertagger_item_set;
  SXINT  *store_npd, *store_numpd;

#ifdef MAKE_INSIDEG
  SXBA w_set1;
#else /* !MAKE_INSIDEG */
  SXBA_CST_ALLOC (w_set1, inputG_MAXITEM+1);
#endif /* !MAKE_INSIDEG */

#if LOG
  fputs ("****** Entering dynamic_set_automaton () ******\n", stdout);
#endif /* LOG */

#ifdef MAKE_INSIDEG
  w_set1 = sxba_calloc (spf.insideG.maxitem+1);
  /* On utilise les npd/numpd de insideG */
  store_npd = spf.insideG.npd;
  store_numpd = spf.insideG.numpd;
#else /* ifndef MAKE_INSIDEG */
  {
    /* On les reconstruits a partir de inputG+basic_item_set */
    SXBA nyp_nt_set;

    nyp_nt_set = sxba_calloc (inputG_MAXNT+1);
    store_npd = (SXINT*) sxalloc (inputG_MAXNT+1, sizeof (SXINT));
    store_numpd = (SXINT*) sxalloc (inputG_MAXITEM+1, sizeof (SXINT));

    item = -1; /* On traite aussi [S' -> $ S $] ... */

    while ((item = sxba_scan (basic_item_set, item)) >= 0) {
      while ((X = LISPRO (item)) != 0) {
	if (X > 0) {
	  if (SXBA_bit_is_reset_set (nyp_nt_set, X))
	    store_numpd [store_npd [X] = NPD (X+1)-1] = 0; /* On pointe vers le 0 de fin de liste (liste vide) */
	      
	  store_numpd [--store_npd [X]] = item; /* On ajoute item a la liste */
	}

	item++;
      }
    }

    sxfree (nyp_nt_set);
  }
#endif /* ifndef MAKE_INSIDEG */

  if (is_mlstn2lex_la_tset_for_dsa) {
    mlstn2lexicalized_look_ahead_t_set = sxbm_calloc (1+n+1+2, SXEOF+1);
  }

  source_t_set = sxba_calloc (SXEOF+1);
  int_set = sxba_calloc (MAXNT+1);
  nt_set = sxba_calloc (MAXNT+1);
  new_rl_control_set = sxba_calloc (MAXITEM+1);
  closure_set = sxba_calloc (MAXITEM+1);
  or_and_t_set = sxba_calloc (SXEOF+1);
  working_titem_set = sxba_calloc (MAXITEM+1);

  nt_stack = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), nt_stack [0] = 0;

  cur_titem_set = &(w_set1 [0]);
  active_sets = sxbm_calloc (idag.final_state+1, MAXITEM+1);
  control_sets = sxbm_calloc (idag.final_state+1+max_rcvr_tok_no, MAXITEM+1); /* +max_rcvr_tok_no car rl_control_sets==control_sets */
  next_mlstn_set = sxba_calloc (idag.final_state+1+max_rcvr_tok_no);

  /* On part des items initiaux valides par la lexicalisation et connectes a S' -> $ . S $ */
  control_set = control_sets [idag.init_state]; /* transition sur le eof initial!! */
  active_set = active_sets [idag.init_state];
  /* Simulation du close-reduce */
  sxba_copy (control_set, NT2ITEM_SET (1));
  SXBA_1_bit (control_set, 1); /* Le 23/06/2003, j'ajoute [S' -> $ . S $] */

  sxba_2op (active_set, SXBA_OP_COPY, control_set, SXBA_OP_AND, basic_item_set);

  if (sa_scanner (idag.init_state)) {
    /* Il y a des transitions terminales depuis l'etat initial */
    /* Sinon, pas de traitement d'erreur ds la phase gauche-droite, on se contente de l'abandonner */
    for (mlstn = idag.init_state+1; mlstn <= idag.final_state; mlstn++) {
      control_set = control_sets [mlstn];
      active_set = active_sets [mlstn];
      sxba_empty (closure_set);

      item = -1;

      /* active_set = {A -> \alpha t. \beta | ? ->t mlstn} */
      while ((item = sxba_scan (active_set, item)) > 0) {
	/* close_reduce () met uniquement ds active_set les items reduce */
	close_reduce (item
#if LLOG
		      , mlstn
#endif /* LLOG */
		      );
      }

      if (!IS_EMPTY (nt_stack)) {
	do {
	  A = POP (nt_stack);
	  /* On cherche toutes les occurrences de A en RHS telle que B -> \alpha . A \beta soit ds control_set */
	  bot = store_npd [A];

	  while (item = store_numpd [bot++]/* B -> \alpha . A  \beta */) {
	    if (SXBA_bit_is_set (control_set, item)) {
	      close_reduce (item+1
#if LLOG
			    , mlstn
#endif /* LLOG */
			    );
	    }
	  }
	} while (!IS_EMPTY (nt_stack));

	sxba_empty (nt_set);
      }

      /* On change de niveau */
      sxba_empty (int_set);

      /* On ajoute les items de la fermeture filtres par la lexicalisation */
      sxba_2op (active_set, SXBA_OP_OR, closure_set, SXBA_OP_AND, basic_item_set);

      /* On fabrique le nouveau control set... */
      sxba_or (control_set, active_set);
      /* ...qui est utilise ds sa_dag_scanner */

      if (mlstn == idag.final_state || !sa_scanner (mlstn)) {
	/* Il n'y a pas de transitions terminales depuis l'etat courant */
	/* pas de traitement d'erreur ds la phase gauche-droite, on se contente de l'abandonner */
	/* Le 03/05/04 non, il ne faut pas abandonner Ex :
	   ... (ou alors | ou_alors) ...
	   Si "ou" est une ff inconnue il faut quand meme continuer, c,a pourra marcher par "ou_alors" */ 
	/* break; */
      }
    }
  }

  lr_pass_succeeds = SXBA_bit_is_set (active_sets [idag.final_state], 2); /* [S' -> $ S. $] \in active_set */

  /* Si la passe gauche-droite a echouee et s'il n'y a pas de traitement d'erreur, c'est fini... */
  mlstn = 0; /* echec a priori */

  if (lr_pass_succeeds) {
    /* active_set == active_sets [idag.final_state] */
    /* active_set ne contient pas des "prod_set" */
    new_basic_item_set = sxba_calloc (MAXITEM+1);

    /* Le 13/02/08 Il faut appeler fill_idag_q2pq_stack () car sa_rlscanner () a besoin des tables backward de idag */
    fill_idag_q2pq_stack ();

    if (IS_EPSILON) {
      /* On y met les items des productions vides pertinentes (sinon rien) */
      sxba_2op (new_basic_item_set, SXBA_OP_COPY, active_set, SXBA_OP_AND, EMPTY_PROD_ITEM_SET);
    }
 
    /* On y met les items de S' -> $ S $ */
    sxba_range (new_basic_item_set, 0, 4);

    /* Analyse par le mirror dynamic set automaton dont les active sets seront filtres par ceux de l'analyse gauche-droite */
    rl_active_sets = sxbm_calloc (idag.final_state+1+max_rcvr_tok_no, MAXITEM+1);
    rl_active_set = rl_active_sets [idag.final_state];

    /* On ne conserve que les items connectes a l'item final [S' -> $ S. $] */
    /* Ca permet de supprimer de ne conserver que les prods dont on a atteint les items reduce */
    sxba_2op (rl_active_set, SXBA_OP_COPY, NT2RC_ITEM_SET (1), SXBA_OP_AND, active_sets [idag.final_state]);
    SXBA_1_bit (rl_active_set, 2); /* ... et on remet [S' -> $ S. $] */

    /* On "recycle" les control_sets (on n'a pas besoin de idag.final_state) */
    rl_control_sets = control_sets;

    for (mlstn = idag.init_state; mlstn < idag.final_state; mlstn++)
      sxba_empty (rl_control_sets [mlstn]);

    sxba_copy (new_rl_control_set, rl_active_set); /* pour sa_rlscanner */

    if (mlstn2lexicalized_look_ahead_t_set) {
      SXBA_1_bit (mlstn2lexicalized_look_ahead_t_set [0], SXEOF);
      SXBA_1_bit (mlstn2lexicalized_look_ahead_t_set [idag.final_state], SXEOF);
    }

    if (sa_rlscanner (idag.final_state)) {
      /* Les active_set et control_set des mlstns atteints par scan arriere des terminaux finals du source
	 ont ete initialises.  Ils pourront se completer + tard s'ils ont d'autres transitions sortantes */
      for (mlstn = idag.final_state-1; mlstn >= idag.init_state; mlstn--) {
	rl_control_set = rl_control_sets [mlstn];
	rl_active_set = rl_active_sets [mlstn];
	sxba_empty (closure_set);

	/* Le but est de remplir new_basic_item_set */
	item = -1;

	/* Tous les items de rl_active_set sont de la forme
	   A -> \alpha .t \beta, avec mlstn ->t ?? */
	while ((item = sxba_scan (rl_active_set, item)) >= 0) {
	  /* Contrairement a close_reduce(), rl_close_reduce() ne touche pas a rl_active_set */
	  rl_close_reduce (item
#if LLOG
			   , mlstn
#endif /* LLOG */
			   );
	}

	if (!IS_EMPTY (nt_stack)) {
	  do {
	    A = POP (nt_stack);
	    /* On cherche toutes les occurrences de A en RHS telle que B -> \alpha A . \beta soit ds rl_control_set */
	    bot = store_npd [A];

	    while (item = store_numpd [bot++]/* B -> \alpha . A  \beta */) {
	      item_1 = item+1;

	      if (SXBA_bit_is_set (rl_control_set, item_1)) {
		rl_close_reduce (item
#if LLOG
				 , mlstn
#endif /* LLOG */
				 );
	      }
	    }
	  } while (!IS_EMPTY (nt_stack));

	  sxba_empty (nt_set);
	}

	/* ici closure_set est rempli ainsi que new_basic_item_set avec les "reduce" non vides du miroir */

	/* On change de niveau */
	sxba_empty (int_set);
	/* On filtre par le resultat de la passe gauche-droite */
	sxba_2op (rl_active_set, SXBA_OP_OR, closure_set, SXBA_OP_AND, active_sets [mlstn]);

	if (IS_EPSILON) {
	  /* On ajoute les productions vides des fermetures */
	  /* Si A est implique' ds closure_set et si A -> \alpha \beta avec \alpha \beta =>* \varepsilon alors A -> \alpha . \beta \in closure_set */
	  sxba_2op (new_basic_item_set, SXBA_OP_OR, closure_set, SXBA_OP_AND, EMPTY_PROD_ITEM_SET);
	} /* is_epsilon */

	  /* On fabrique le nouveau control set... */
	sxba_2op (new_rl_control_set, SXBA_OP_COPY, rl_control_set, SXBA_OP_OR, rl_active_set);
	/* ...qui est utilise ds sa_rlscanner */

	/* scan arriere */
	sa_rlscanner (mlstn); /* il peut echouer, mais d'autres trans vers "prev_mlstn" peuvent marcher */
      }
	
      if (SXBA_bit_is_set (rl_active_set, 1)) {
	/* rl_active_set est celui calcule' pour idag.init_state */
	/* [S' -> $ .S $] est atteint => C'est OK */
	mlstn = 1; /* OK */
      }
    }
    else
      mlstn = 0; /* echec */

    sxbm_free (rl_active_sets), rl_active_sets = NULL;

    if (mlstn > 0) {
      sxfree (basic_item_set);
      basic_item_set = new_basic_item_set;
    } else
      sxfree (new_basic_item_set);
  }

  sxbm_free (active_sets), active_sets = NULL;
  sxbm_free (control_sets), control_sets = NULL;
  sxfree (next_mlstn_set), next_mlstn_set = NULL;

#ifdef MAKE_INSIDEG
  sxfree (w_set1);
#else /* ifndef MAKE_INSIDEG */
  sxfree (store_npd);
  sxfree (store_numpd);
#endif /* ifndef MAKE_INSIDEG */

  sxfree (source_t_set), source_t_set = NULL;
  sxfree (int_set), int_set = NULL;
  sxfree (nt_set), nt_set = NULL;
  sxfree (new_rl_control_set), new_rl_control_set = NULL;
  sxfree (closure_set), closure_set = NULL;
  sxfree (or_and_t_set), or_and_t_set = NULL;
  sxfree (working_titem_set), working_titem_set = NULL;
  sxfree (nt_stack), nt_stack = NULL;

  if (mlstn == 0 && mlstn2lexicalized_look_ahead_t_set) sxbm_free (mlstn2lexicalized_look_ahead_t_set), mlstn2lexicalized_look_ahead_t_set = NULL;

#if MEASURES || LOG
  output_G ("Dynamic Set Automaton Grammar", basic_item_set, NULL);
#endif /* MEASURES || LOG */

  if (is_print_time) {
    char string [64];

    sprintf (string, "\t\tDynamic Set Automaton (%s)", (mlstn > 0) ? "TRUE" : "FALSE");

    sxtime (TIME_FINAL, string);
  }

#if LOG
  fputs ("****** Leaving dynamic_set_automaton () ******\n", stdout);
#endif /* LOG */

  return mlstn > 0;
}
#endif /* is_set_automaton */


#ifdef MAKE_INSIDEG
/* Remplit spf.insideG en utilisant la sous-grammaire reduite de basic_item_set */

static void make_insideG (SXINT *old_prod2new_prod)
{
  SXINT new_t, new_nt, bot, prod, new_prod, A, X, new_X, item, new_item, left_item, rhs_size, top_basic_prod_stack, cur_basic_prod_stack;
  SXINT *prolon_ptr, *lhs_ptr, *prolis_ptr, *lispro_ptr;
  SXINT *old_nt2new_nt, *prod2nb_symb, *new_nt_stack, *old_t2new_t;
#if MEASURES || LOG
  SXINT rhst_nb, unlexicalized_prod_nb = 0;
#endif /* MEASURES || LOG */

#if LOG
  fputs ("****** Entering make_insideG () ******\n", stdout);
#endif /* LOG */

  spf.insideG.modif_time = time (0);
  /* Deja positionnes ds make_a_first_reduced_grammar ()
  spf.insideG.maxprod
  spf.insideG.maxt
  spf.insideG.maxnt
  spf.insideG.maxitem
  spf.insideG.maxrhs
  spf.insideG.maxrhsnt
  spf.insideG.sizeofnumpd
  spf.insideG.sizeoftnumpd
  spf.insideG.is_eps
  */

  spf_allocate_insideG (&spf.insideG, 
			false,
#if is_set_automaton
			IS_DYNAMIC_SET_AUTOMATON
#else /* !is_set_automaton */
			false
#endif /* !is_set_automaton */
			);

  old_t2new_t = (SXINT*) sxcalloc (inputG_SXEOF+1, sizeof (SXINT));
  new_nt_stack = (SXINT*) sxalloc (spf.insideG.maxnt+1, sizeof (SXINT)), RAZ (new_nt_stack);
  old_nt2new_nt = (SXINT*) sxcalloc (inputG_MAXNT+1, sizeof (SXINT));
  prod2nb_symb = (SXINT*) sxcalloc (spf.insideG.maxprod+1, sizeof (SXINT));

  prolon_ptr = &(spf.insideG.prolon [0]);
  lhs_ptr = &(spf.insideG.lhs [0]);
  prolis_ptr = &(spf.insideG.prolis [0]);
  lispro_ptr = &(spf.insideG.lispro [0]);

  /* On met la super regle */
  if (old_prod2new_prod) old_prod2new_prod [0] = 0;
  *prolon_ptr++ = 0;
  *lhs_ptr++ = 0;
  *lispro_ptr++ = spf.insideG.maxt;
  old_t2new_t [inputG_SXEOF] = SXEOF;
  spf.insideG.t2init_t [SXEOF] = inputG_SXEOF;
  SXBA_1_bit (spf.insideG.titem_set, 0);
  *prolis_ptr++ = 0;
  *lispro_ptr++ = 1;
  *prolis_ptr++ = 0;
  *lispro_ptr++ = spf.insideG.maxt;
  SXBA_1_bit (spf.insideG.titem_set, 2);
  *prolis_ptr++ = 0;
  *lispro_ptr++ = 0;
  *prolis_ptr++ = 0;

  *prolon_ptr++ = 4;

  /* c'est parti ... */
  new_nt = new_t = new_prod = 0; 

  {
    /* gestion de l'occurrence de l'axiome ds la super-regle */
    X = 1; 
    old_nt2new_nt [X] = new_X = ++new_nt;
    spf.insideG.nt2init_nt [new_X] = X;
    /* initialement, on pointe vers le debut de sa propre tranche */
    spf.insideG.npg [new_X+1] = spf.insideG.npg [new_X] += old_nt2lhs_nb [X];
    spf.insideG.npd [new_X+1] = spf.insideG.npd [new_X] += old_nt2rhs_nb [X]+1 /* le 0 de fin de liste */;
    spf.insideG.numpd [--spf.insideG.npd [new_X]] = 0 /* le 0 de fin de liste */;
    spf.insideG.numpd [--spf.insideG.npd [new_X]] = 1; /* S' -> $ . S $ */
    spf.insideG.rhs_nt2where [new_X]++;
  }
  
  new_item = 3;

#ifdef HUGE_CFG
  top_basic_prod_stack = TOP (basic_prod_stack);
  /* Attention, on suppose que a l'indice 1 on a 0 (la super-prod) et a l'indice 2 on a une S-prod */

  for (cur_basic_prod_stack = 2; cur_basic_prod_stack <= top_basic_prod_stack; cur_basic_prod_stack++)
#else /* ifndef HUGE_CFG */
  item = 3;

  while ((item = sxba_scan (basic_item_set, item)) > 0)
#endif /* ifndef HUGE_CFG */
    {
#ifdef HUGE_CFG
      prod = basic_prod_stack [cur_basic_prod_stack];
      item = spf.inputG.prolon [prod];
#else /* ifndef HUGE_CFG */
      prod = spf.inputG.prolis [item];
#endif /* ifndef HUGE_CFG */

      new_item++;
      spf.insideG.prod2init_prod [++new_prod] = prod;
      if (old_prod2new_prod) old_prod2new_prod [prod] = new_prod;

      X = spf.inputG.lhs [prod];
      new_X = old_nt2new_nt [X];
#if is_set_automaton
      if (IS_DYNAMIC_SET_AUTOMATON) {
	/* Pour remplir spf.insideG.t2prod_item_set */
	rhs_size = spf.inputG.prolon [prod+1]-item;
	left_item = new_item;
      }
#endif /* is_set_automaton */

      if (new_X == 0) {
	old_nt2new_nt [X] = new_X = ++new_nt;
	spf.insideG.nt2init_nt [new_X] = X;
	/* initialement, on pointe vers le debut de sa propre tranche */
	spf.insideG.npg [new_X+1] = spf.insideG.npg [new_X] += old_nt2lhs_nb [X];
	spf.insideG.npd [new_X+1] = spf.insideG.npd [new_X] += old_nt2rhs_nb [X]+1 /* le 0 de fin de liste */;
	spf.insideG.numpd [--spf.insideG.npd [new_X]] = 0 /* le 0 de fin de liste */;
      }

      spf.insideG.numpg [--spf.insideG.npg [new_X]] = new_prod;
      
      *lhs_ptr++ = new_X;
      spf.insideG.lhs_nt2where [new_X]++;

#if is_epsilon
      if (spf.insideG.is_eps && (spf.inputG.lispro [item] == 0)) {
	/* On initialise BVIDE et empty_prod_item_set */
	if (SXBA_bit_is_reset_set (spf.insideG.bvide, new_X))
	  PUSH (new_nt_stack, new_X);

	SXBA_1_bit (spf.insideG.empty_prod_item_set, new_item);
      }
#endif /* is_epsilon */

#if MEASURES || LOG
      rhst_nb = 0;
#endif /* MEASURES || LOG */

      while (X = spf.inputG.lispro [item]) {
	if (X > 0) {
	  new_X = old_nt2new_nt [X];

	  if (new_X == 0) {
	    old_nt2new_nt [X] = new_X = ++new_nt;
	    spf.insideG.nt2init_nt [new_X] = X;
	    /* initialement, on pointe vers le debut de sa propre tranche */
	    spf.insideG.npg [new_X+1] = spf.insideG.npg [new_X] += old_nt2lhs_nb [X];
	    spf.insideG.npd [new_X+1] = spf.insideG.npd [new_X] += old_nt2rhs_nb [X]+1 /* le 0 de fin de liste */;
	    spf.insideG.numpd [--spf.insideG.npd [new_X]] = 0 /* le 0 de fin de liste */;
	  }

	  spf.insideG.numpd [--spf.insideG.npd [new_X]] = new_item;
	  spf.insideG.rhs_nt2where [new_X]++;
	}
	else {
#if MEASURES || LOG
	  rhst_nb++;
#endif /* MEASURES || LOG */

	  new_X = old_t2new_t [-X];

	  if (new_X == 0) {
	    old_t2new_t [-X] = new_X = ++new_t;
	    spf.insideG.t2init_t [new_X] = -X;
	    spf.insideG.tpd [new_X+1] = spf.insideG.tpd [new_X] += old_t2rhs_nb [-X];
	    SXBA_1_bit (idag.source_set, new_X);
	  }

	  spf.insideG.tnumpd [--spf.insideG.tpd [new_X]] = new_item;
	  spf.insideG.rhs_nt2where [0]++;
	  SXBA_1_bit (spf.insideG.titem_set, new_item);

#if is_set_automaton
	  if (IS_DYNAMIC_SET_AUTOMATON) {
	    X = new_item+1;
	    SXBA_1_bit (spf.insideG.rc_titem_set, X);

	    /* Tant pis s'il y a plusieurs occur du m terminal ds la meme prod */
	    sxba_range (spf.insideG.t2prod_item_set [new_X], left_item, rhs_size);
	  }
#endif /* is_set_automaton */

	  new_X = -new_X;
	}

	prod2nb_symb [new_prod]++;

	*lispro_ptr++ = new_X;
	*prolis_ptr++ = new_prod;

	item++;
	new_item++;
      }

#if MEASURES || LOG
      if (rhst_nb == 0) unlexicalized_prod_nb++;
#endif /* MEASURES || LOG */
      
      *lispro_ptr++ = 0;
      *prolis_ptr++ = new_prod;

      *prolon_ptr++ = new_item+1;
    } 
    
#if EBUG
  if (new_nt != MAXNT || new_t != SXEOF-1 || new_prod != MAXPROD || new_item != MAXITEM)
    sxtrap (ME, "make_insideG");
#endif /* EBUG */

  spf.insideG.numpd [spf.insideG.sizeofnumpd-1] = 0; /* bidon */

#if EBUG
  if (spf.insideG.npd [MAXNT+1]+1 != spf.insideG.sizeofnumpd)
    sxtrap (ME, "make_insideG");
#endif /* EBUG */

  /* On finalise tpd, tnumpd */
  X = spf.insideG.tpd [SXEOF+1] = spf.insideG.tpd [SXEOF] + 2 /* Les 2 eof */;

#if EBUG
  if (X+1 != spf.insideG.sizeoftnumpd)
    sxtrap (ME, "make_insideG");
#endif /* EBUG */

  spf.insideG.tnumpd [X] = 0; /* bidon */
  spf.insideG.tnumpd [X-1] = 2; /* S' -> eof S . eof */
  spf.insideG.tnumpd [X-2] = 0; /* S' -> . eof S eof */

  spf.insideG.t2init_t [SXEOF] = inputG_SXEOF; /* valeur positive */

#if is_epsilon
  if (spf.insideG.is_eps) {
    /* On remplit BVIDE et empty_prod_item_set */
    while (!IS_EMPTY (new_nt_stack)) {
      A = POP (new_nt_stack);
      bot = spf.insideG.npd [A];

      while (item = spf.insideG.numpd [bot++]) {
	prod = spf.insideG.prolis [item];

	if (--prod2nb_symb [prod] == 0) {
	  A = spf.insideG.lhs [prod];

	  if (SXBA_bit_is_reset_set (spf.insideG.bvide, A))
	    PUSH (new_nt_stack, A);

	  item = spf.insideG.prolon [prod];
	  sxba_range (spf.insideG.empty_prod_item_set, item, spf.insideG.prolon [prod+1]-item);
	}
      }
    }
  }
#endif /* is_epsilon */

  sxfree (new_nt_stack);
  sxfree (old_nt2new_nt);
  sxfree (prod2nb_symb);

#ifndef MAKE_PROPER
  /* calcule les relations "left_corner", "nt2item_set" et "left_recursive_set" sur cette nouvelle grammaire */
  LC_construction (&spf.insideG);

  /* Les relations "right_corner", "nt2rc_item_set" et "right_recursive_set" seront calculees a la demande par appel explicite de RC_construction () */
#if is_set_automaton
  if (IS_DYNAMIC_SET_AUTOMATON) {
    RC_construction ();
  }
#endif /* is_set_automaton */

  {
    /* Calculs de rhs_nt2where et lhs_nt2where. */
    /* rhs et lhs decoupent le range des items :
       les items terminaux, les items non-terminaux et les items reduce. */
    /* Le NT de code 0 (S') n'apparait pas en RHS. 0 est utilise pour les terminaux. */
    /* rhs_nt2where [0] = 0
       rhs_nt2where [1] = nb d'occurrences des terminaux en rhs
       si A \in NT et A > 1
       rhs_nt2where [A] = rhs_nt2where [A-1]+nb d'occurrences du NT A-1 en RHS. */
    /* lhs_nt2where [0] = rhs_nt2where [ntmax]+nb d'occurrences du NT ntmax en RHS
       si A \in NT
       lhs_nt2where [A] = lhs_nt2where [A-1]+nb d'occurrences du NT A-1 en RHS. */
    SXINT nb, nt, v;

    nb = spf.insideG.rhs_nt2where [0];
    spf.insideG.rhs_nt2where [0] = 0; /* Les terminaux commencent en 0 */

    for (nt = 1; nt <= spf.insideG.maxnt; nt++) {
      v = spf.insideG.rhs_nt2where [nt];
      spf.insideG.rhs_nt2where [nt] = nb;
      nb += v;
    }

    for (nt = 0; nt <= spf.insideG.maxnt; nt++) {
      v = spf.insideG.lhs_nt2where [nt];
      spf.insideG.lhs_nt2where [nt] = nb;
      nb += v;
    }
  }
    

#if 0
  /* PB ds le cas HUGE, nt2order, ..., prod_order2min ne sont pas generes
     Il faudrait donc ici reprendre les calculs faits ds LC1.c
     1er essai, on va modifier Earley pour lui faire executer les reductions ds une file */
  {
    /* On cherche un ordre statique d'execution des reductions */
    /* On a la proprie'te' :
       old_nt < old_nt' <=> new_nt < new_nt' avec old_nt = spf.insideG.nt2init_nt [new_nt] et old_nt' = spf.insideG.nt2init_nt [new_nt'] */
    /* Ce calcul va reutiliser les tableaux statiques nt2order et order2nt */
    SXINT old_order, new_order, old_nt, new_nt, old_prod, new_prod;

    spf.insideG.nt2order = (SXINT*) sxalloc (spf.insideG.maxnt+1, sizeof (SXINT)), spf.insideG.nt2order [0] = 0;
    spf.insideG.order2nt = (SXINT*) sxalloc (spf.insideG.maxnt+1, sizeof (SXINT)), spf.insideG.order2nt [0] = 0;

    new_order = 0;

    for (old_order = 1; old_order <= inputG_MAXNT; old_order++) {
      old_nt = order2nt [old_order];

      if (new_nt = old_nt2new_nt [old_nt]) {
	new_order++;
	spf.insideG.nt2order [new_nt] = new_order;
	spf.insideG.order2nt [new_order] = new_nt;
      }
    }

    /* On cherche un ordre statique d'execution des actions semantiques */
    spf.insideG.order2prod = (SXINT*) sxcalloc (spf.insideG.maxprod+1, sizeof (SXINT)), spf.insideG.order2prod [0] = 0;
    spf.insideG.prod2order = (SXINT*) sxcalloc (spf.insideG.maxprod+1, sizeof (SXINT)), spf.insideG.prod2order [0] = 0;
    spf.insideG.prod_order2min = (SXINT*) sxcalloc (spf.insideG.maxprod+1, sizeof (SXINT)), spf.insideG.prod_order2min [0] = 0;
    spf.insideG.prod_order2max = (SXINT*) sxcalloc (spf.insideG.maxprod+1, sizeof (SXINT)), spf.insideG.prod_order2max [0] = 0;

    new_order = 0;

    for (old_order = 1; old_order <= inputG_MAXPROD; old_order++) {
      old_prod = order2prod [old_order];

      if (new_prod = old_prod2new_prod [old_prod]) {
	new_order++;
	spf.insideG.prod2order [new_prod] = new_order;
	spf.insideG.order2prod [new_order] = new_prod;
	old_order2new_order [old_order] = new_order;
	new_order2old_order [new_order] = old_order;
      }
    }

    for (new_order = 1; new_order <= spf.insideG.maxprod; new_order++) {
      old_order = new_order2old_order [new_order];
      spf.insideG.prod_order2min [new_order] = old_order2new_order [prod_order2min [old_order]];
      spf.insideG.prod_order2max [new_order] = old_order2new_order [prod_order2max [old_order]];
    }
  }
#endif /* 0 */
#endif /* MAKE_PROPER */

#ifdef MAKE_INSIDEG
#ifndef MAKE_PROPER
  idag_t_stack_recode (old_t2new_t /* Passage de inputG a insideG */);
#endif /* MAKE_PROPER */
#endif /* MAKE_INSIDEG */
  
  sxfree (old_t2new_t);

  sxfree (old_nt2lhs_nb), old_nt2lhs_nb = NULL;
  sxfree (old_nt2rhs_nb), old_nt2rhs_nb = NULL;
  sxfree (old_t2rhs_nb), old_t2rhs_nb = NULL;

#if MEASURES || LOG
  printf ("%sinsideG = (|N| = %i, |T| = %i, |P| = %i(%i), |G| = %i)\n",
#if MEASURES
	  "MEASURES: "
#else /* LOG */
	  ""
#endif /* LOG */
	  , MAXNT
	  , SXEOF-1
	  , MAXPROD
	  , unlexicalized_prod_nb
	  , MAXITEM);
#endif /* MEASURES || LOG */

#if LOG
  fputs ("****** Leaving make_insideG () ******\n", stdout);
#endif /* LOG */
}

/* Remplit lex_insideG en utilisant spf.insideG et basic_item_set */

static void make_smallest_insideG ()
{
  SXINT            new_t, new_nt, bot, prod, new_prod, A, new_A, X, new_X, item, new_item;
  SXINT            *prolon_ptr, *lhs_ptr, *prolis_ptr, *lispro_ptr;
  SXINT            *old_nt2new_nt, *prod2nb_symb, *new_nt_stack, *old_t2new_t;
  SXINT            rhsnt_nb, rhst_nb, rhs_nb;
  SXINT            *new_nt2lhs_nb, *new_nt2rhs_nb, *new_t2rhs_nb;
  bool        not_identical = false;
#if MEASURES || LOG
  SXINT            unlexicalized_prod_nb = 0;
#endif /* MEASURES || LOG */
  struct insideG lex_insideG;

#if LOG
  fputs ("****** Entering make_smallest_insideG () ******\n", stdout);
#endif /* LOG */

  /* On commence par copier spf.insideG ds lex_insideG */
  lex_insideG = spf.insideG;

  /* Source_set et glbl_source sont relatif a lex_insideG */
  /* Ils seront changes en fin si necessaire */
  
  /* On raze spf.insideG */
  memset (&spf.insideG, 0, sizeof (struct insideG));

  spf.insideG.modif_time = time (0);

  new_nt2lhs_nb = (SXINT*) sxcalloc (lex_insideG.maxnt+1, sizeof (SXINT));
  new_nt2rhs_nb = (SXINT*) sxcalloc (lex_insideG.maxnt+1, sizeof (SXINT)), new_nt2rhs_nb [1] = 1 /* l'axiome ds la super-regle */;
  new_t2rhs_nb = (SXINT*) sxcalloc (-lex_insideG.maxt+1, sizeof (SXINT));

  old_t2new_t = (SXINT*) sxcalloc (-lex_insideG.maxt+1, sizeof (SXINT));
  new_t = 0;
  old_nt2new_nt = (SXINT*) sxcalloc (lex_insideG.maxnt+1, sizeof (SXINT));
  old_nt2new_nt [1] = new_nt = spf.insideG.maxnt = 1; /* L'axiome a le code 1 */
  spf.insideG.sizeofnumpd = 1; /* occur de l'axiome ds la super-regle */
  /* Attention, il se peut que dans les insideG la lhs de la 1ere regle ne soit pas l'axiome */

  /* traditionnellement, spf.insideG.maxitem est l'indice du 0 de la derniere regle !! */
  spf.insideG.maxitem = item = 3; /* On saute la super-regle */

  while ((item = sxba_scan (basic_item_set, item)) > 0) {
    /* Nelle regle */
    A = lex_insideG.lhs [lex_insideG.prolis [item]];

    if ((new_A = old_nt2new_nt [A]) == 0)
      new_A = old_nt2new_nt [A] = ++spf.insideG.maxnt; /* nouvel nt */

    new_nt2lhs_nb [new_A]++;

    rhsnt_nb = rhst_nb = 0;

    while ((X = lex_insideG.lispro [item]) != 0) {
      if (X > 0) {
	rhsnt_nb++;

	if ((new_X = old_nt2new_nt [X]) == 0)
	  new_X = old_nt2new_nt [X] = ++spf.insideG.maxnt; /* nouvel nt */

	new_nt2rhs_nb [new_X]++;
      }
      else {
	rhst_nb++;

	if ((new_X = old_t2new_t [-X]) == 0) {
	  new_X = old_t2new_t [-X] = ++spf.insideG.maxt; /* nouveau t */

	  if (new_X != -X)
	    not_identical = true;
	}

	new_t2rhs_nb [new_X]++;
      }

      item++;
    }

    spf.insideG.maxprod++;
    rhs_nb = rhsnt_nb+rhst_nb;
    spf.insideG.maxitem += rhs_nb+1;
    spf.insideG.sizeofnumpd += rhsnt_nb;
    spf.insideG.sizeoftnumpd += rhst_nb;

    if (rhs_nb == 0)
      spf.insideG.is_eps = true;

    if (rhs_nb > spf.insideG.maxrhs)
      spf.insideG.maxrhs = rhs_nb;

    if (rhsnt_nb > spf.insideG.maxrhsnt)
      spf.insideG.maxrhsnt = rhsnt_nb;
  }

  /* Qq petites rectifications ... */
  spf.insideG.maxt = -spf.insideG.maxt/* <0 */ -1/* eof */;

  if (spf.insideG.maxt != lex_insideG.maxt)
    not_identical = true;

  old_t2new_t [-lex_insideG.maxt] = -SXEOF; /* en neg, Il a ete trouve' */
    
  spf.insideG.sizeofnumpd += spf.insideG.maxnt+2;
  spf.insideG.sizeoftnumpd += 2 /* les 2 eof */ +2/* les 2 zeros bidons de debut et de fin */;
  new_t2rhs_nb [SXEOF] += 2; /* Les deux $ de la super-regle */

  /* Ici toutes les cstes de spf.insideG sont positionnees */

  /* On appelle l'allocation generale */
  spf_allocate_insideG (&spf.insideG, false, false);

  new_nt_stack = (SXINT*) sxalloc (spf.insideG.maxnt+1, sizeof (SXINT)), RAZ (new_nt_stack);
  prod2nb_symb = (SXINT*) sxcalloc (spf.insideG.maxprod+1, sizeof (SXINT));

  prolon_ptr = &(spf.insideG.prolon [0]);
  lhs_ptr = &(spf.insideG.lhs [0]);
  prolis_ptr = &(spf.insideG.prolis [0]);
  lispro_ptr = &(spf.insideG.lispro [0]);

  /* On met la super regle */
  *prolon_ptr++ = 0;
  *lhs_ptr++ = 0;
  *lispro_ptr++ = spf.insideG.maxt;
  spf.insideG.t2init_t [SXEOF] = inputG_SXEOF;
  SXBA_1_bit (spf.insideG.titem_set, 0);
  *prolis_ptr++ = 0;
  *lispro_ptr++ = 1;
  *prolis_ptr++ = 0;
  *lispro_ptr++ = spf.insideG.maxt;
  SXBA_1_bit (spf.insideG.titem_set, 2);
  *prolis_ptr++ = 0;
  *lispro_ptr++ = 0;
  *prolis_ptr++ = 0;

  *prolon_ptr++ = 4;

  /* c'est parti ... */
  new_prod = 0; 

  {
    /* gestion de l'occurrence de l'axiome ds la super-regle */
    new_X = X = 1;
    old_nt2new_nt [X] = -new_X; /* passe' en <0 a la 1ere rencontre */
    spf.insideG.nt2init_nt [new_X] = X;
    /* initialement, on pointe vers le debut de sa propre tranche */
    spf.insideG.npg [new_X+1] = spf.insideG.npg [new_X] += new_nt2lhs_nb [new_X];
    spf.insideG.npd [new_X+1] = spf.insideG.npd [new_X] += new_nt2rhs_nb [new_X]+1 /* le 0 de fin de liste */;
    spf.insideG.numpd [--spf.insideG.npd [new_X]] = 0 /* le 0 de fin de liste */;
    spf.insideG.numpd [--spf.insideG.npd [new_X]] = 1; /* S' -> $ . S $ */
    spf.insideG.rhs_nt2where [new_X]++;
  }
  
  new_item = item = 3;

  while ((item = sxba_scan (basic_item_set, item)) > 0) {
    prod = lex_insideG.prolis [item];

    new_item++;
    spf.insideG.prod2init_prod [++new_prod] = lex_insideG.prod2init_prod [prod];

    X = lex_insideG.lhs [prod];
    new_X = old_nt2new_nt [X];

    if (new_X > 0) {
      old_nt2new_nt [X] = -new_X; /* passe' en <0 a la 1ere rencontre */
      spf.insideG.nt2init_nt [new_X] = lex_insideG.nt2init_nt [X];
      /* initialement, on pointe vers le debut de sa propre tranche */
      spf.insideG.npg [new_X+1] = spf.insideG.npg [new_X] += new_nt2lhs_nb [new_X];
      spf.insideG.npd [new_X+1] = spf.insideG.npd [new_X] += new_nt2rhs_nb [new_X]+1 /* le 0 de fin de liste */;
      spf.insideG.numpd [--spf.insideG.npd [new_X]] = 0 /* le 0 de fin de liste */;
    }
    else
      new_X = -new_X;

    spf.insideG.numpg [--spf.insideG.npg [new_X]] = new_prod;
      
    *lhs_ptr++ = new_X;
    spf.insideG.lhs_nt2where [new_X]++;

#if is_epsilon
    if (spf.insideG.is_eps && (lex_insideG.lispro [item] == 0)) {
      /* On initialise BVIDE et empty_prod_item_set */
      if (SXBA_bit_is_reset_set (spf.insideG.bvide, new_X))
	PUSH (new_nt_stack, new_X);

      SXBA_1_bit (spf.insideG.empty_prod_item_set, new_item);
    }
#endif /* is_epsilon */

#if MEASURES || LOG
    rhst_nb = 0;
#endif /* MEASURES || LOG */

    while (X = lex_insideG.lispro [item]) {
      if (X > 0) {
	new_X = old_nt2new_nt [X];

	if (new_X > 0) {
	  old_nt2new_nt [X] = -new_X; /* passe' en <0 a la 1ere rencontre */
	  spf.insideG.nt2init_nt [new_X] = lex_insideG.nt2init_nt [X];
	  /* initialement, on pointe vers le debut de sa propre tranche */
	  spf.insideG.npg [new_X+1] = spf.insideG.npg [new_X] += new_nt2lhs_nb [new_X];
	  spf.insideG.npd [new_X+1] = spf.insideG.npd [new_X] += new_nt2rhs_nb [new_X]+1 /* le 0 de fin de liste */;
	  spf.insideG.numpd [--spf.insideG.npd [new_X]] = 0 /* le 0 de fin de liste */;
	}
	else
	  new_X = -new_X;

	spf.insideG.numpd [--spf.insideG.npd [new_X]] = new_item;
	spf.insideG.rhs_nt2where [new_X]++;
      }
      else {
#if MEASURES || LOG
	rhst_nb++;
#endif /* MEASURES || LOG */

	new_X = old_t2new_t [-X];

	if (new_X > 0) {
	  old_t2new_t [-X] = -new_X;
	  spf.insideG.t2init_t [new_X] = lex_insideG.t2init_t [-X];
	  spf.insideG.tpd [new_X+1] = spf.insideG.tpd [new_X] += new_t2rhs_nb [new_X];
	  SXBA_1_bit (idag.source_set, new_X);
	}
	else
	  new_X = -new_X;

	spf.insideG.tnumpd [--spf.insideG.tpd [new_X]] = new_item;
	spf.insideG.rhs_nt2where [0]++;
	SXBA_1_bit (spf.insideG.titem_set, new_item);
	new_X = -new_X;
      }

      prod2nb_symb [new_prod]++;

      *lispro_ptr++ = new_X;
      *prolis_ptr++ = new_prod;

      item++;
      new_item++;
    }

#if MEASURES || LOG
    if (rhst_nb == 0) unlexicalized_prod_nb++;
#endif /* MEASURES || LOG */
      
    *lispro_ptr++ = 0;
    *prolis_ptr++ = new_prod;

    *prolon_ptr++ = new_item+1;
  } 
    
#if EBUG
  if (new_prod != spf.insideG.maxprod || new_item != spf.insideG.maxitem)
    sxtrap (ME, "make_smallest_insideG");
#endif /* EBUG */

  spf.insideG.numpd [spf.insideG.sizeofnumpd-1] = 0; /* bidon */

#if EBUG
  if (spf.insideG.npd [spf.insideG.maxnt+1]+1 != spf.insideG.sizeofnumpd)
    sxtrap (ME, "make_smallest_insideG");
#endif /* EBUG */

  /* On finalise tpd, tnumpd */
  X = spf.insideG.tpd [SXEOF+1] = spf.insideG.tpd [SXEOF] + 2 /* Les 2 eof */;

#if EBUG
  if (X+1 != spf.insideG.sizeoftnumpd)
    sxtrap (ME, "make_smallest_insideG");
#endif /* EBUG */

  spf.insideG.tnumpd [X] = 0; /* bidon */
  spf.insideG.tnumpd [X-1] = 2; /* S' -> eof S . eof */
  spf.insideG.tnumpd [X-2] = 0; /* S' -> . eof S eof */

#if is_epsilon
  if (spf.insideG.is_eps) {
    /* On remplit BVIDE et empty_prod_item_set */
    while (!IS_EMPTY (new_nt_stack)) {
      A = POP (new_nt_stack);
      bot = spf.insideG.npd [A];

      while (item = spf.insideG.numpd [bot++]) {
	prod = spf.insideG.prolis [item];

	if (--prod2nb_symb [prod] == 0) {
	  A = spf.insideG.lhs [prod];

	  if (SXBA_bit_is_reset_set (spf.insideG.bvide, A))
	    PUSH (new_nt_stack, A);

	  item = spf.insideG.prolon [prod];
	  sxba_range (spf.insideG.empty_prod_item_set, item, spf.insideG.prolon [prod+1]-item);
	}
      }
    }
  }
#endif /* is_epsilon */

  {
    /* Calculs de rhs_nt2where et lhs_nt2where. */
    /* rhs et lhs decoupent le range des items :
       les items terminaux, les items non-terminaux et les items reduce. */
    /* Le NT de code 0 (S') n'apparait pas en RHS. 0 est utilise pour les terminaux. */
    /* rhs_nt2where [0] = 0
       rhs_nt2where [1] = nb d'occurrences des terminaux en rhs
       si A \in NT et A > 1
       rhs_nt2where [A] = rhs_nt2where [A-1]+nb d'occurrences du NT A-1 en RHS. */
    /* lhs_nt2where [0] = rhs_nt2where [ntmax]+nb d'occurrences du NT ntmax en RHS
       si A \in NT
       lhs_nt2where [A] = lhs_nt2where [A-1]+nb d'occurrences du NT A-1 en RHS. */
    SXINT nb, nt, v;

    nb = spf.insideG.rhs_nt2where [0];
    spf.insideG.rhs_nt2where [0] = 0; /* Les terminaux commencent en 0 */

    for (nt = 1; nt <= spf.insideG.maxnt; nt++) {
      v = spf.insideG.rhs_nt2where [nt];
      spf.insideG.rhs_nt2where [nt] = nb;
      nb += v;
    }

    for (nt = 0; nt <= spf.insideG.maxnt; nt++) {
      v = spf.insideG.lhs_nt2where [nt];
      spf.insideG.lhs_nt2where [nt] = nb;
      nb += v;
    }
  }
  
  /* Pour l'instant on se debarrasse de lex_insideG */
  spf_free_insideG (&lex_insideG);

  sxfree (new_nt_stack);
  sxfree (old_nt2new_nt);
  sxfree (prod2nb_symb);

  sxfree (new_nt2lhs_nb), new_nt2lhs_nb = NULL;
  sxfree (new_nt2rhs_nb), new_nt2rhs_nb = NULL;
  sxfree (new_t2rhs_nb), new_t2rhs_nb = NULL;

#if is_set_automaton
  if (mlstn2lexicalized_look_ahead_t_set && not_identical) {
    /* Il faut l'adapter a smallest_insideG */
    SXINT  i, old_t, new_t;
    SXBA *mlstn2lexicalized_look_ahead_new_t_set, old_set, new_set;

    mlstn2lexicalized_look_ahead_new_t_set = sxbm_calloc (1+n+1+2, SXEOF+1);

    for (i = n+1+2; i >= 0; i--) {
      old_set = mlstn2lexicalized_look_ahead_t_set [i];
      new_set = mlstn2lexicalized_look_ahead_new_t_set [i];

      old_t = 0;

      while ((old_t = sxba_scan (old_set, old_t)) > 0) {
	if (new_t = -old_t2new_t [old_t])
	  SXBA_1_bit (new_set, new_t);
      }
    }

    sxbm_free (mlstn2lexicalized_look_ahead_t_set);
    mlstn2lexicalized_look_ahead_t_set = mlstn2lexicalized_look_ahead_new_t_set;
  }
#endif /* is_set_automaton */

#ifdef MAKE_INSIDEG
#ifndef MAKE_PROPER
  idag_t_stack_recode (old_t2new_t);
#endif /* MAKE_PROPER */
#endif /* MAKE_INSIDEG */

  sxfree (old_t2new_t);

  /* calcule les relations "left_corner", "nt2item_set" et "left_recursive_set" sur cette nouvelle grammaire */
  LC_construction (&spf.insideG);

#if LLOG
  sxfree (basic_item_set);
  sxprepare_for_possible_reset (basic_item_set);
  basic_item_set = sxba_calloc (MAXITEM+1);
  sxba_fill (basic_item_set);
  print_current_sub_grammar ();
#endif /* LLOG */

#if EBUG
#if LLOG==0  
  sxfree (basic_item_set);
  sxprepare_for_possible_reset (basic_item_set);
  basic_item_set = sxba_calloc (MAXITEM+1);
  sxba_fill (basic_item_set);
#endif /* LLOG==0 */
  check_reduced_sub_grammar ();
#endif /* EBUG */

#if MEASURES || LOG
  printf ("%sspf.insideG = (|N| = %i, |T| = %i, |P| = %i(%i), |G| = %i)\n",
#if MEASURES
	  "MEASURES: "
#else /* LOG */
	  ""
#endif /* LOG */
	  , spf.insideG.maxnt
	  , SXEOF-1
	  , spf.insideG.maxprod
	  , unlexicalized_prod_nb
	  , spf.insideG.maxitem);
#endif /* MEASURES || LOG */

#if LOG
  fputs ("****** Leaving make_smallest_insideG () ******\n", stdout);
#endif /* LOG */
}
#endif /* MAKE_INSIDEG */


#ifdef HUGE_CFG

static bool unvalid_multiple_anchor_occur_prod_pre (SXINT prod)
{
  SXINT             item, t, cur, X;
  bool         unvalid = false;

  /* prod est multiple */
  item = spf.inputG.prolon [prod];

  while ((X = spf.inputG.lispro [item]) != 0) {
    if (X < 0) {
      t = -X;

      if (!SXBA_bit_is_set (idag.source_set, t)) {
	/* t n'est pas ds le source */
	/* prod doit etre eliminee */
	unvalid = true;
	break;
      }
    }

    item++;
  }

  return unvalid;
}
#else /* !HUGE_CFG */

static bool check_multiple_anchor_occur_pre_with_basic_item_set (SXINT **map_stack, SXINT **maep_stack)
{
  SXINT             item, t, prod, top, bot;
  SXINT             prev_prod, nb = 0;
  SXINT             *multi_anchor_prod_stack, *multi_anchor_erased_prod_stack;
  SXBA_CST_ALLOC (local_multiple_t_item_set, inputG_MAXITEM+1);

#if LOG
  fputs ("****** Entering the multiple anchor occurrence pre test ******\n", stdout);
#endif /* LOG */

#ifdef is_check_rhs_sequences
  if (IS_CHECK_RHS_SEQUENCES) multi_anchor_prod_stack = NULL;
#endif /* is_check_rhs_sequences */
#ifndef MULTI_ANCHOR
  multi_anchor_erased_prod_stack = NULL;
#endif /* ifndef MULTI_ANCHOR */

  /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
  if (sxba_2op (local_multiple_t_item_set, SXBA_OP_COPY, multiple_t_item_set, SXBA_OP_AND, basic_item_set)) {
    /* ... qui restent selectionnees */
    item = -1;
    prev_prod = 0;

    while ((item = sxba_scan (local_multiple_t_item_set, item)) >= 0) {
      t = -spf.inputG.lispro [item];
      prod = spf.inputG.prolis [item];

#ifdef is_check_rhs_sequences
      if (IS_CHECK_RHS_SEQUENCES && prod != prev_prod) {
	/* On vient de changer de prod a ancre multiple ... */
	if (prev_prod > 0) {
	  /* ... et il y a une precedente, non sautee */
	  /* On s'en souvient pour tester l'ordre des terminaux + tard */
	  if (multi_anchor_prod_stack == NULL)
	    DALLOC_STACK (multi_anchor_prod_stack, 50);

	  DPUSH (multi_anchor_prod_stack, prod);
	}

	prev_prod = prod;
      }
#endif /* is_check_rhs_sequences */

      if (!SXBA_bit_is_set (idag.source_set, t)) {
	/* La prod de item doit etre eliminee */
	top = spf.inputG.prolon [prod+1];
#ifdef MULTI_ANCHOR
	/* ... tout de suite */
	/* On enleve les items de prod */
	bot = spf.inputG.prolon [prod];
	sxba_clear_range (basic_item_set, bot, top-bot);
	nb++;
#if LLLOG
	printf ("The multiple anchor occurrence pre test fails for \"%s\" in production ", spf.inputG.tstring [t]);
	output_prod (prod);
#endif /* LLLOG */
#else /* ifndef MULTI_ANCHOR */
	/* ... plus tard, on la note */
	if (multi_anchor_erased_prod_stack == NULL)
	  DALLOC_STACK (multi_anchor_erased_prod_stack, 50);

	DPUSH (multi_anchor_erased_prod_stack, prod);
#endif /* ifndef MULTI_ANCHOR */

#ifdef is_check_rhs_sequences
	if (IS_CHECK_RHS_SEQUENCES) prev_prod = -prod; /* sautee */
#endif /* is_check_rhs_sequences */
	item = top-1; /* On "saute" prod */
      }
    }

#if MEASURES || LOG
    if (nb > 0)
      printf ("%sThe multiple anchor occurrence pre test erases %i productions.\n",
#if MEASURES
	      "MEASURES: "
#else /* LOG */
	      ""
#endif /* LOG */
	      , nb);
#endif /* MEASURES || LOG */
  }

#ifdef is_check_rhs_sequences
  if (IS_CHECK_RHS_SEQUENCES && multi_anchor_prod_stack) *map_stack = multi_anchor_prod_stack;
#endif /* is_check_rhs_sequences */
#ifndef MULTI_ANCHOR
  if (multi_anchor_erased_prod_stack) *maep_stack = multi_anchor_erased_prod_stack;
#endif /* ifndef MULTI_ANCHOR */

#if LOG
  fputs ("****** Leaving the multiple anchor occurrence pre test ******\n", stdout);
#endif /* LOG */

  return nb > 0;
}
#endif /* HUGE_CFG */


/* Calcule une sous-grammaire reduite de la grammaire d'origine dont l'ensemble des symboles terminaux
   est l'ensemble des terminaux du texte source.
   Si ce module est compile' avec l'option -DMAKE_INSIDEG, cette sous-grammaire reduite (si elle existe)
   est stockee ds spf.insideG.  Elle peut etre utilisee par les (parseurs) appelants comme si c'etait
   la grammaire initiale stockee ds spf.inputG.
   Si ce module n'est pas compile' avec l'option -DMAKE_INSIDEG, la sous-grammaire reduite n'est pas construite
   explicitement, c'est l'ensemble d'items (de la grammaire d'origine) du SXBA  basic_item_set.
   Ds le cas MAKE_INSIDEG, basic_item_set est complet et fait reference non pas a spf.inputG mais a spf.insideG.
   Si on veut que l'ordre des terminaux du (DAG) source soit (partiellement) pris en compte ds l'obtention de
   cette sous-grammaire reduite, il faut compiler ce module avec l'option -Dis_check_rhs_sequences. Si une RHS
   contient la sequence de symboles "A X1 ... Xp B" avec X1 ... Xp =>* \varepsilon et A et B non vides, on verifie
     - que au moins un debutant de X1 ... Xp B est un suivant immediat des terminateurs de A
     - et que au moins un debutant de B est un suivant non forcement immediat des terminateurs de A

   La compilation avec l'option -DMAKE_PROPER est utilisee pour fabriquer le processeur "make_proper" qui
   sort explicitement la sous-grammaire sous la forme d'une BNF.
*/
/* On tient compte de l'ordre des terminaux ds les prod a ancres multiples */
/* l'ordre des terminaux est donne' par t2suffix_t_set (si t1 \in t2suffix_t_set [t] <=> il y a dans le source un t1 a droite de t, et accessible depuis t) */
/* Soit P' (l'ensemble des productions de) la sous-grammaire reduite telle que pour tout x = a_1 ... a_n \in L(P'), on a x \in t_set^*
   et telle que si S =>* \alpha A \beta =>p \alpha \sigma \beta =>* x et si \sigma = \sigma_1 t_1 \sigma_2 t_2 \sigma_3, alors t_2 suit t_1 dans le source.
   On a basic_item_set = {A -> \alpha . \beta | p = A\alpha \beta et p \in P'} */

/* Si lex_source_set est non NULL il contiendra le sous-ensemble de source_set conserve' par la lexicalisation */
/* Nouvelle version du 26/01/07 */

static bool t_set2basic_item_set ()
{
  SXINT     bot, top, prod, item, t, new_prod_nb, unlexicalized_prod_nb; 
  bool is_axiom_kept;
  SXINT     *multi_anchor_erased_prod_stack = NULL, *multi_anchor_prod_stack = NULL;

#if LOG
  fputs ("****** Entering t_set2basic_item_set () ******\n", stdout);
#endif /* LOG */

#if MEASURES || LOG
#ifdef HUGE_CFG
  unlexicalized_prod_nb = sxba_cardinal (unlexicalized_prod_set);
#else /* ifndef HUGE_CFG */
  unlexicalized_item_set = t2prod_item_set [0];
  unlexicalized_prod_nb = 0;
  item = 3;

  while ((item = sxba_scan (unlexicalized_item_set, item)) > 0) {
    unlexicalized_prod_nb++;
    item = spf.inputG.prolon [spf.inputG.prolis [item]+1]-1;
  }
#endif /* ifndef HUGE_CFG */

  printf ("%sinputG = (|N| = %i, |T| = %i, |P| = %i(%i), |G| = %i)\n",
#if MEASURES
	  "MEASURES: "
#else /* LOG */
	  ""
#endif /* LOG */
	  , inputG_MAXNT
	  , inputG_SXEOF-1
	  , inputG_MAXPROD
	  , unlexicalized_prod_nb
	  , inputG_MAXITEM);
#endif /* MEASURES || LOG */

#ifdef HUGE_CFG
  {
    SXBA basic_prod_set, first_time_prod_set;

#if MEASURES || LOG
    SXINT  nb = 0;
#endif /* MEASURES || LOG */

    basic_prod_stack = (SXINT*) sxalloc (inputG_MAXPROD+1/* On peut pusher la super-regle */+1, sizeof (SXINT)), RAZ (basic_prod_stack);

    if (multiple_t_prod_set) {
      basic_prod_set = sxba_calloc (inputG_MAXPROD+1/* On peut pusher la super-regle */+1);
      first_time_prod_set = sxba_calloc (inputG_MAXPROD+1/* On peut pusher la super-regle */+1);
    }

    t = 0;

    while ((t = sxba_scan (idag.source_set, t)) > 0) {
      bot = t2prod_hd [t];
      top = t2prod_hd [t+1];

      while (bot < top) {
	prod = t2prod_list [bot];
	/* prod est ancree sur t */

	if (multiple_t_prod_set && SXBA_bit_is_set (multiple_t_prod_set, prod)) {
	  /* Le 28/02/08 On memoize unvalid_multiple_anchor_occur_prod_pre (prod)  qui va etre appele'
	     p fois si sa RHS a p contient p symboles terminaux differents */
	  if (!SXBA_bit_is_set (basic_prod_set, prod)) {
	    /* C'est la 1ere fois qu'on voit prod ou prod a deja ete examine' et il est mauvais et elimine' */
	    if (SXBA_bit_is_reset_set (first_time_prod_set, prod)) {
	      /* C'est la 1ere fois qu'on voit prod */
	      if (unvalid_multiple_anchor_occur_prod_pre (prod)) {
		/* prod a ancre multiple invalide */
		/* On l'elimine .... */
#ifdef MULTI_ANCHOR
		/* ... tout de suite */
#if MEASURES || LOG
		nb++;
#if LLLOG
		printf ("The multiple anchor occurrence pre test fails for \"%s\" in production ", spf.inputG.tstring [t]);
		output_prod (prod);
#endif /* LLLOG */
#endif /* MEASURES || LOG */
#else /* ifndef MULTI_ANCHOR */
		/* ... plus tard ... */
		/* ... on la conserve ... */
		SXBA_1_bit (basic_prod_set, prod);
		PUSH (basic_prod_stack, prod);

		/* ... et on la note aussi pour + tard */
		if (multi_anchor_erased_prod_stack == NULL)
		  DALLOC_STACK (multi_anchor_erased_prod_stack, 50);

		DPUSH (multi_anchor_erased_prod_stack, prod);
#endif /* ifndef MULTI_ANCHOR */
	      }
	      else
		{
		/* On la garde */
		SXBA_1_bit (basic_prod_set, prod);
		PUSH (basic_prod_stack, prod);

#ifdef is_check_rhs_sequences
		if (IS_CHECK_RHS_SEQUENCES) {
		  /* prod a ancres multiples conservee */
		  /* On s'en souvient pour tester l'ordre des terminaux + tard */
		  if (multi_anchor_prod_stack == NULL)
		    DALLOC_STACK (multi_anchor_prod_stack, 50);

		  DPUSH (multi_anchor_prod_stack, prod);
		}
#endif /* is_check_rhs_sequences */
	      }
	    }
	  }
	}
	else {
	  PUSH (basic_prod_stack, prod);
	}

	bot++;
      }
    }

    if (multiple_t_prod_set) {
      sxfree (basic_prod_set);
      sxfree (first_time_prod_set);
    }

#if MEASURES || LOG
    if (nb > 0)
      printf ("%sThe multiple anchor occurrence pre test erases %i productions.\n",
#if MEASURES
	      "MEASURES: "
#else /* LOG */
	      ""
#endif /* LOG */
	      , nb);
#endif /* MEASURES || LOG */
  }
#else /* ifndef HUGE_CFG */
  sxprepare_for_possible_reset (basic_item_set);
  basic_item_set = sxba_calloc (inputG_MAXITEM+1);

  /* t2prod_item_set [0] = {A -> \alpha . \beta | \alpha \beta \in NT*} */
  /* sxba_copy (basic_item_set, t2prod_item_set [0]); */
  unlexicalized_item_set = t2prod_item_set [0];
  /* t2prod_item_set [0] = {A -> \alpha . \beta | \alpha \beta \in NT*} */

  t = 0;

  while ((t = sxba_scan (idag.source_set, t)) > 0)
    sxba_or (basic_item_set, t2prod_item_set [t]);

  if (multiple_t_item_set) {
    /* Il y a des productions a ancre multiples ... */
    /* ... on regarde si toutes ces ancres sont ds le source */
    /* Si p = A -> ... t1 ... t2 ...
       et si t1 \in source_set, les items de p sont dans basic_item_set.
       En revanche, si t2 \not \in source_set, ils doivent etre supprimes */
    /* L'ordre de ces ancres ne sera exploite' que si is_check_rhs_sequences */
    check_multiple_anchor_occur_pre_with_basic_item_set (&multi_anchor_prod_stack, &multi_anchor_erased_prod_stack);
  }
#endif /* ifndef HUGE_CFG */

  if (new_prod_nb = make_a_first_reduced_grammar ()) {
    /* Grammaire reduite non vide */
    SXINT top, i, new_prod;
    SXINT *old_prod2new_prod = NULL;

    /* basic_nt_set a ete alloue'/rempli par make_a_first_reduced_grammar */
#ifdef MAKE_INSIDEG
    /* Si MAKE_INSIDEG, on remplit spf.insideG avec cette 1ere grammaire reduite */
    /* Ca permet[tra] a la rcvr eventuelle de travailler sur une sous-grammaire + grande
       qui permet entre autre les intervertions, ... */

#  ifndef MAKE_PROPER
#    if NRG==1
    if (nrg_init_tprod_stack)
      old_prod2new_prod = (SXINT*) sxcalloc (spf.inputG.maxprod+1, sizeof (SXINT));
#    endif /* if NRG==1 */

#    ifdef is_check_rhs_sequences
    if (IS_CHECK_RHS_SEQUENCES && old_prod2new_prod == NULL && multi_anchor_prod_stack)
      old_prod2new_prod = (SXINT*) sxcalloc (spf.inputG.maxprod+1, sizeof (SXINT));
#    endif /* is_check_rhs_sequences */

#    ifndef MULTI_ANCHOR
    if (old_prod2new_prod == NULL && multi_anchor_erased_prod_stack)
      old_prod2new_prod = (SXINT*) sxcalloc (spf.inputG.maxprod+1, sizeof (SXINT));
#    endif /* MULTI_ANCHOR */
#  endif /* MAKE_PROPER */

    make_insideG (old_prod2new_prod);

#  ifdef HUGE_CFG
    sxfree (basic_prod_stack), basic_prod_stack = NULL;
#  endif /* HUGE_CFG */

#  ifndef MAKE_PROPER
#    if NRG==1
    if (nrg_init_tprod_stack) {
      top = DTOP (nrg_init_tprod_stack);
      nrg_init_tprod_stack [0] = 0;

      for (i = 1; i <= top; i++) {
	if (new_prod = old_prod2new_prod [nrg_init_tprod_stack [i]])
	  DSPUSH (nrg_init_tprod_stack, new_prod);
      }
    }
#    endif /* if NRG==1 */
#  endif /* MAKE_PROPER */

#  ifndef MAKE_PROPER
#    ifdef is_check_rhs_sequences
    if (IS_CHECK_RHS_SEQUENCES && multi_anchor_prod_stack) {
      top = DTOP (multi_anchor_prod_stack);
      multi_anchor_prod_stack [0] = 0;

      for (i = 1; i <= top; i++) {
	if (new_prod = old_prod2new_prod [multi_anchor_prod_stack [i]])
	  DSPUSH (multi_anchor_prod_stack, new_prod);
      }
    }
#    endif /* is_check_rhs_sequences */
#  endif /* MAKE_PROPER */

#  ifndef MAKE_PROPER
#    ifndef MULTI_ANCHOR
    if (multi_anchor_erased_prod_stack) {
      top = DTOP (multi_anchor_erased_prod_stack);
      multi_anchor_erased_prod_stack [0] = 0;

      for (i = 1; i <= top; i++) {
	if (new_prod = old_prod2new_prod [multi_anchor_erased_prod_stack [i]])
	  DSPUSH (multi_anchor_erased_prod_stack, new_prod);
      }
    }
#    endif /* MULTI_ANCHOR */
#  endif /* MAKE_PROPER */

    if (old_prod2new_prod) sxfree (old_prod2new_prod);

    if (basic_item_set) sxfree (basic_item_set);
    sxprepare_for_possible_reset (basic_item_set);
    basic_item_set = sxba_calloc (spf.insideG.maxitem+1);
    sxba_fill (basic_item_set); /* Ici, tout est bon */

    if (basic_nt_set) sxfree (basic_nt_set);
    sxprepare_for_possible_reset (basic_nt_set);
    basic_nt_set = sxba_calloc (spf.insideG.maxnt+1);
    sxba_fill (basic_nt_set); /* Ici, tout est bon */

#  if EBUG
    if (new_prod_nb != spf.insideG.maxprod)
      sxtrap (ME, "t_set2basic_item_set");
#  endif /* EBUG */
#endif /* ifdef MAKE_INSIDEG */

    if (is_print_time) {
      if (new_prod_nb)
	sxtime (TIME_FINAL, "\t\tBasic (TRUE)");
      else
	sxtime (TIME_FINAL, "\t\tBasic (FALSE)");
    }

#if is_cycle_free
    cycle_check ();

    if (is_print_time)
      sxtime (TIME_FINAL, "\t\tCycle Free");
#endif /* is_cycle_free */

    is_axiom_kept = true;

#ifndef MAKE_PROPER

#  ifndef MULTI_ANCHOR
    /* Ca n'a pas ete fait + tot, on le fait ici */
    if (is_axiom_kept && multi_anchor_erased_prod_stack) {
      /* Il y a des productions a ancre multiples a eliminer */
      if (check_multiple_anchor_occur_post (multi_anchor_erased_prod_stack)) {
	/* y'a eu des suppressions */
	/* Comme ca check_rhs_sequences recoit une grammaire reduite en entree */
	if ((new_prod_nb = make_a_next_reduced_grammar ()) == 0)
	  is_axiom_kept = false;
      }
    }
#  endif /* ifndef MULTI_ANCHOR */

#if 0
    /* ESSAI : est appele' apres check_rhs_sequences */
#if is_set_automaton
    if (IS_DYNAMIC_SET_AUTOMATON && is_axiom_kept && dynamic_set_automaton ())
      /* dynamic_set_automaton a marche, retourne-t-il automatiquement une grammaire reduite ?? */
      /* Non Ex :
	 S -> X \alpha X
	 X -> \eps
	 [X -> .] est selectionne' en gauche_droite mais supposons que la reconnaissance de S -> X \alpha X echoue ds \alpha
	 Quand on attaque la passe droite-gauche [X -> .] est ds le rc de S et comme [X -> .] a ete selectionne' en gauche droite
	 la prod X -> \eps est validee (alors qu'elle est inaccessible) */
      /* dynamic_set_automaton () englobe-t-il check_rhs_sequences () ?
	 Non, il peut y avoir des prods qui seront supprimees par l'un sans l'etre par l'autre */
      new_prod_nb = make_a_next_reduced_grammar ();
#endif /* is_set_automaton */
#endif /* 0 */

#  ifdef is_check_rhs_sequences
    /* Appel unique de dag_or_nodag_source_processing */
    if (IS_CHECK_RHS_SEQUENCES && is_axiom_kept) {
      SXINT i;

      crs_dag_or_nodag_source_processing ();

      if (multi_anchor_prod_stack) {
	/* On verifie l'ordre des ancres multiples */
	if (check_multiple_anchor_sequence (multi_anchor_prod_stack)) {
	  /* Y'a eu des prods supprimees */
	  /* Comme ca check_rhs_sequences () recoit une grammaire reduite en entree */
	  if ((new_prod_nb = make_a_next_reduced_grammar ()) == 0)
	    is_axiom_kept = false;
	}
      }

      /* IS_CHECK_RHS_SEQUENCES == 0  => Rien
	 IS_CHECK_RHS_SEQUENCES >= 1  => On boucle IS_CHECK_RHS_SEQUENCES fois
	 IS_CHECK_RHS_SEQUENCES == -1 => On boucle jusqu'au point fixe */
      for (i = 1;
	   (i <= IS_CHECK_RHS_SEQUENCES || IS_CHECK_RHS_SEQUENCES == -1) && is_axiom_kept && check_rhs_sequences (&is_axiom_kept);
	   i++) {
	/* basic_item_set a ete elague */
	if (is_axiom_kept) {
	  /* ... et non vide */
	  new_prod_nb = make_a_next_reduced_grammar ();

#    if EBUG
	  if (new_prod_nb == 0)
	    sxtrap (ME, "t_set2basic_item_set");
#    endif /* EBUG */
	}
      }

      /* A SORTIR de check_rhs_sequences () */
      sxbm_free (nt2first_packed), nt2first_packed = NULL;
      sxbm_free (nt2first_set), nt2first_set = NULL;
      sxbm_free (nt2last_packed), nt2last_packed = NULL;
      sxbm_free (nt2look_ahead_set), nt2look_ahead_set = NULL;
      sxbm_free (nt2cl_nt_set), nt2cl_nt_set = NULL;
      sxbm_free (nt2cr_nt_set), nt2cr_nt_set = NULL;
      sxfree (left_item_set), left_item_set = NULL;
      sxfree (right_item_set), right_item_set = NULL;
      sxbm_free (nt2suffix_set), nt2suffix_set = NULL;
      sxfree (w_t_set), w_t_set = NULL;
      sxfree (F_set), F_set = NULL;
      sxfree (titem2packed_titem), titem2packed_titem = NULL;
      sxfree (packed_titem2titem), packed_titem2titem = NULL;
      sxbm_free (nt2anla_t_set), nt2anla_t_set = NULL;
      sxfree (w2_t_set), w2_t_set = NULL;
      sxfree (w2_nt_set), w2_nt_set = NULL;
#if LOG || EBUG
      sxfree (first_or_last_lhs_nt_set), first_or_last_lhs_nt_set = NULL;
#endif /* LOG || EBUG */
#ifdef def_segment
      sxfree (crs_segment), crs_segment = NULL;
      sxfree (crs_segment_list), crs_segment_list = NULL;
      sxfree (crs_valid_segment_set), crs_valid_segment_set = NULL;
#endif /* def_segment */

      if (t2suffix_t_set) sxfree (t2suffix_t_set), t2suffix_t_set = NULL;
      if (t2la_t_set) sxbm_free (t2la_t_set), t2la_t_set = NULL;

      if (is_print_time) {
	if (is_axiom_kept)
	  sxtime (TIME_FINAL, "\t\tCheck RHS Sequences (TRUE)");
	else
	  sxtime (TIME_FINAL, "\t\tCheck RHS Sequences (FALSE)");
      }
    }
#  endif /* is_check_rhs_sequences */

#if is_set_automaton
    if (IS_DYNAMIC_SET_AUTOMATON && is_axiom_kept && dynamic_set_automaton ())
      /* dynamic_set_automaton a marche, retourne-t-il automatiquement une grammaire reduite ?? */
      /* Non Ex :
	 S -> X \alpha X
	 X -> \eps
	 [X -> .] est selectionne' en gauche_droite mais supposons que la reconnaissance de S -> X \alpha X echoue ds \alpha
	 Quand on attaque la passe droite-gauche [X -> .] est ds le rc de S et comme [X -> .] a ete selectionne' en gauche droite
	 la prod X -> \eps est validee (alors qu'elle est inaccessible) */
      /* dynamic_set_automaton () englobe-t-il check_rhs_sequences () ?
	 Non, il peut y avoir des prods qui seront supprimees par l'un sans l'etre par l'autre */
      new_prod_nb = make_a_next_reduced_grammar ();
#endif /* is_set_automaton */

#  if NRG==1
    if (nrg_working_nt_set)
      nrg_free ();
#  endif /* NRG==1 */
#endif /* ifndef MAKE_PROPER */
  }
  else {
    is_axiom_kept = false;

#ifdef HUGE_CFG
    sxfree (basic_prod_stack), basic_prod_stack = NULL;
#endif /* HUGE_CFG */
  }

#ifndef MAKE_PROPER
#  if NRG==1
  if (nrg_init_tprod_stack)
    DFREE_STACK (nrg_init_tprod_stack);
#  endif /* NRG==1 */
#endif /* ifndef MAKE_PROPER */
  
  if (multi_anchor_erased_prod_stack) DFREE_STACK (multi_anchor_erased_prod_stack);
#ifdef is_check_rhs_sequences
  if (IS_CHECK_RHS_SEQUENCES && multi_anchor_prod_stack) DFREE_STACK (multi_anchor_prod_stack);
#endif /* is_check_rhs_sequences */

#ifndef MAKE_PROPER
  if (!is_axiom_kept) {
#  ifdef HUGE_CFG
    if (!spf.insideG.is_allocated) {
      /* Ici on est ds le petrin, car, aucune sous-grammaire reduite n'a pu etre extraite sur aucun sous-ensemble (meme vide) du source */
      /* et on n'a pas les tables pour faire l'analyse (earley) sur la "grosse" grammaire */
      /* On peut
	 - Obliger la "grosse" grammaire a deriver ds le vide (on conserve au moins les prod t.q. S =>+ \varepsilon)
	 - avoir un systeme de regles/terminaux bidons qui marcheraient toujours
      */
      sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
	      "%sNo reduced sub-grammar can be constructed from the input text.",
	      sxplocals.sxtables->err_titles [2]+1);
#if 0
      sxexit (2);
#endif /* 0 */
    }

    /* On va faire l'analyse sur insideG complete, mais bien sur il y aura des erreurs de syntaxe */
    /* On laisse is_axiom_kept a false */
#  else  /* ifndef HUGE_CFG */
    if (spf.insideG.is_allocated) {
      /* On est ici ds le cas ou il y a eu MAKE_INSIDEG demande' explicitement sur une "petite" grammaire
	 l'analyseur a du lui aussi etre compile' avec -DMAKE_INSIDEG, on va faire l'analyse avec l'insideG complet */
      /* On laisse is_axiom_kept a false */
    }
    else {
#    ifdef MAKE_INSIDEG
      /* Ici compilation explicite avec -DMAKE_INSIDEG */
      /* Il faut faire l'analyse avec la petite grammaire complete non filtree par basic_item_set */
      /* PB : l'analyseur est compile' avec aussi avec -DMAKE_INSIDEG et les macros LISPRO, ...
	 referencent donc insideG et non la petite grammaire !! */
      /* On peut peut etre recopier inputG ds insideG (sans dire qu'elle est allouee) */
      /* A FAIRE */
#    else /* ifndef MAKE_INSIDEG */
      /* Ici on va faire l'analyse avec la petite grammaire complete non filtree par basic_item_set */
#    endif /* ifndef MAKE_INSIDEG */
    }
  
#  endif /* ifndef HUGE_CFG */
  }
#else /* !MAKE_PROPER */
  /* make_proper_main utilise insideG */
  if (basic_item_set) sxfree (basic_item_set), basic_item_set = NULL;
#endif /* MAKE_PROPER */

#if LOG
  fputs ("****** Leaving t_set2basic_item_set () ******\n", stdout);
#endif /* LOG */

  return is_axiom_kept;
}

/* remplit basic_prod_set */
/* basic_prod_set= {A -> \alpha . \beta | \alpha \beta \in NT^* ou
                                          \alpha \beta == \gamma t \delta et t==a_i ou` a_i est un terminal du source
                   }
   De plus si \alpha \beta == \gamma t1 \delta t2 \sigma alors il existe dans le source un chemin de la forme eof ... t1 ... t2... eof
*/
/* t2suffix_t_set [t1] = {t | existe un chemin du source de la forme "eof ... t1 ... t ... eof" */
/* autrement dit, t peut suivre t1 (uniquement si la grammaire a des prod a terminaux multiples, sinon NULL) */
/* mlstn2la_tset [i] = {t | \exists j s.t. i ->t j dans le dag} */
/* mlstn2lex_la_tset [i] = {t | \exists j s.t. i ->t j dans le dag et t a ete retenu par la lexicalisation} */

bool lexicalizer2basic_item_set (bool is_mlstn2lex_la_tset, bool is_mlstn2la_tset, bool is_smallest_insideG)
{
  SXINT     mlstn;
  bool ret_val;

  if (is_print_time) {
    fputs ("\tLexicalizer\n", sxtty);
  }

  /* Modif car intialisation partielle interdite avec le -W ci-dessous */
  /* warning: missing field 'outputG' initializer [-Wmissing-field-initializers] */
  spf.inputG = spf_inputG; // L'initialisation est faite ici (sans vérification!!)

#if is_set_automaton
  if (IS_DYNAMIC_SET_AUTOMATON) is_mlstn2lex_la_tset_for_dsa = is_mlstn2lex_la_tset;
#endif /* is_set_automaton */

#ifndef MAKE_INSIDEG
  if (is_mlstn2la_tset) {
    lex_source_set = sxba_calloc (SXEOF+1);
    SXBA_1_bit (lex_source_set, SXEOF);
  }
#endif /* !MAKE_INSIDEG */

  ret_val = t_set2basic_item_set ();

#ifndef MAKE_PROPER
#  ifdef MAKE_INSIDEG
  if (ret_val && is_smallest_insideG && (IS_CHECK_RHS_SEQUENCES != 0 || IS_DYNAMIC_SET_AUTOMATON) && !sxba_is_full (basic_item_set)) {
    /* Ici tout a marche' et une insideG est rempli */
    /* ... et basic_item_set est restreint  */
    /* && il est demande' de l'incorporer dans un nouvel insideG */
    make_smallest_insideG ();
  }
#  endif /* MAKE_INSIDEG */
#endif /* !MAKE_PROPER */


  /* Il semble que ds tous les cas (MAKE_INSIDEG ou non) on puisse appeler dag_or_nodag_source_processing () */
  /* Ds le cas MAKE_INSIDEG source_set et glbl_source ont ete recode's */
  if (
#ifdef HUGE_CFG
      ret_val &&
#endif /* HUGE_CFG */
      is_mlstn2la_tset) {
    mlstn2la_tset = sxbm_calloc (1+n+1+2, SXEOF+1); /* Rempli meme si ret_val == false ds le cas ifndef MAKE_INSIDEG */

    idag_source_processing (SXEOF, t2suffix_t_set, mlstn2suffix_source_set, mlstn2la_tset, t2la_t_set);

    if (ret_val && is_mlstn2lex_la_tset) {
#ifdef MAKE_INSIDEG
#if is_set_automaton
      if (IS_DYNAMIC_SET_AUTOMATON)
	mlstn2lex_la_tset = mlstn2lexicalized_look_ahead_t_set ? mlstn2lexicalized_look_ahead_t_set : mlstn2la_tset; /* prudence */
      else
	mlstn2lex_la_tset = mlstn2la_tset;
#else /* !is_set_automaton */
      /* Ils sont identiques !! attention au free */
      mlstn2lex_la_tset = mlstn2la_tset;
#endif /* !is_set_automaton */
#else /* !MAKE_INSIDEG */
#if is_set_automaton
      if (IS_DYNAMIC_SET_AUTOMATON) {
	mlstn2lex_la_tset = mlstn2lexicalized_look_ahead_t_set ? mlstn2lexicalized_look_ahead_t_set : mlstn2la_tset; /* prudence */
      }
      else {
	/* lex_source_set contient l'ensemble des t du source qui n'ont pas ete elimines par la lexicalisation */
	mlstn2lex_la_tset = sxbm_calloc (1+n+1+2, SXEOF+1);

	for (mlstn = 0; mlstn <= idag.final_state; mlstn++) {
	  sxba_2op (mlstn2lex_la_tset [mlstn], SXBA_OP_COPY, mlstn2la_tset [mlstn], SXBA_OP_AND, lex_source_set);
	}
      }
#else /* is_set_automaton */
      /* lex_source_set contient l'ensemble des t du source qui n'ont pas ete elimines par la lexicalisation */
      mlstn2lex_la_tset = sxbm_calloc (1+n+1+2, SXEOF+1);

      for (mlstn = 0; mlstn <= idag.final_state; mlstn++) {
	sxba_2op (mlstn2lex_la_tset [mlstn], SXBA_OP_COPY, mlstn2la_tset [mlstn], SXBA_OP_AND, lex_source_set);
      }
#endif /* is_set_automaton */
#endif /* !MAKE_INSIDEG */
    }

#ifndef MAKE_INSIDEG
    if (lex_source_set) sxfree (lex_source_set), lex_source_set = NULL;
#endif /* !MAKE_INSIDEG */
  }

  if (!ret_val) {
    if (basic_item_set) sxfree (basic_item_set), basic_item_set = NULL;
    if (basic_nt_set) sxfree (basic_nt_set), basic_nt_set = NULL;
  }

  if (is_print_time) {
    if (ret_val)
      sxtime (TIME_FINAL, "\tEnd Lexicalizer (TRUE)");
    else
      sxtime (TIME_FINAL, "\tEnd Lexicalizer (FALSE)");
  }

  return ret_val;
}


#if 0
/* --------------------------------------------------------------------------------------------------- */
/* Ci-dessous on trouve les sources des anciennes versions */
#if is_lex2

static bool t_set2index_item_set (SXBA basic_item_set,
		      SXBA t_set,
		      SXBA result_item_set)
{
  SXINT t;

  /* t2prod_item_set [0] = {A -> \alpha . \beta | \alpha \beta \in NT*} */
  sxba_copy (result_item_set, T2PROD_ITEM_SET (0));

  t = 0;

  while ((t = sxba_scan (t_set, t)) >= 0)
    sxba_or (result_item_set, T2PROD_ITEM_SET (t));

  return sxba_2op (result_item_set, SXBA_OP_COPY, result_item_set, SXBA_OP_AND, basic_item_set);
}



/* A chaque mlstn i du source, on affecte ds indexed_item_sets l'ensemble des items qu'il selectionne : */
/* indexed_item_sets [i] = {A -> \alpha . \beta | \alpha \beta \in NT^* ou
                                                  \alpha \beta = \gamma t \delta et t==a_i ou "eof ... a_i ... t ... eof"
                                                  est un chemin du source
                           } \cap basic_prod_set
*/
/* mlstn2suffix_source_set [i]= { t | a_i=t ou x=a_1 ... a_i ... t ... a_n} */
/* t2suffix_t_set [t1] = {t | existe un chemin du source de la forme "eof ... t1 ... t ... eof" */
/* autrement dit, t peut suivre t1 (uniquement si la grammaire a des prod a terminaux multiples, sinon NULL) */

bool lexicalizer2indexed_item_sets (bool is_mlstn2lex_la_tset,
			       bool is_mlstn2la_tset)
{
  bool ret_val;

#ifndef MAKE_INSIDEG
  if (is_mlstn2la_tset) {
    lex_source_set = sxba_calloc (SXEOF+1);
    SXBA_1_bit (lex_source_set, SXEOF);
  }
#endif /* !MAKE_INSIDEG */

  ret_val = t_set2basic_item_set ();

  if (ret_val) {
    indexed_item_sets = sxbm_calloc (idag.final_state+1, MAXITEM+1);
    sxba_copy (indexed_item_sets [0], basic_item_set);
    sxba_copy (indexed_item_sets [idag.init_state], basic_item_set); /* C'est le meme */

    for (mlstn = idag.init_state+1; mlstn <= idag.final_state; mlstn++) {
      if (!t_set2index_item_set (basic_item_set,
				 mlstn2suffix_source_set [mlstn],
				 indexed_item_sets [mlstn])) {
	ret_val = false;
	break;
      }
    
      if (mlstn2lex_la_tset)
	sxba_2op (mlstn2lex_la_tset [mlstn], SXBA_OP_COPY, mlstn2la_tset [mlstn], SXBA_OP_AND, lex_source_set);
    }

    SXBA_1_bit (indexed_item_sets [idag.final_state], 2); /* On ajoute S' -> $ <S> . $ */
  }

  if (is_mlstn2la_tset) {
    if (
#ifdef MAKE_INSIDEG
	ret_val
#else /* !MAKE_INSIDEG */
	true
#endif /* !MAKE_INSIDEG */
	) {
      mlstn2la_tset = sxbm_calloc (1+n+1+2, SXEOF+1); /* Rempli meme si ret_val == false ds le cas ifndef MAKE_INSIDEG */

      idag_source_processing (SXEOF, t2suffix_t_set, mlstn2suffix_source_set, mlstn2la_tset, t2la_t_set);

#ifdef MAKE_INSIDEG
      if (ret_val && is_mlstn2lex_la_tset) {
	/* Ils sont identiques !! attention au free */
	mlstn2lex_la_tset = mlstn2la_tset;
      }
#else /* !MAKE_INSIDEG */
      if (ret_val && is_mlstn2lex_la_tset) {
	/* lex_source_set contient l'ensemble des t du source qui n'ont pas ete elimines par la lexicalisation */
	mlstn2lex_la_tset = sxbm_calloc (1+n+1+2, SXEOF+1);

	for (mlstn = 0; mlstn <= idag.final_state; mlstn++) {
	  sxba_2op (mlstn2lex_la_tset [mlstn], SXBA_OP_COPY, mlstn2la_tset [mlstn], SXBA_OP_AND, lex_source_set);
	}
      }

      sxfree (lex_source_set), lex_source_set = NULL;
#endif /* !MAKE_INSIDEG */
    }
  }

  if (basic_item_set) sxfree (basic_item_set), basic_item_set = NULL;
  if (basic_nt_set) sxfree (basic_nt_set), basic_nt_set = NULL;

  if (is_print_time) {
    if (ret_val)
      sxtime (TIME_FINAL, "\tIndex Lexicalizer (TRUE)");
    else
      sxtime (TIME_FINAL, "\tIndex Lexicalizer (FALSE)");
  }

  return ret_val;
}
#endif /* is_lex2 */


/* Adaptation + haut */
#if is_set_automaton
static SXBA     *i2rl_supertagger_item_set; /* Si l'appelant veut du supertagging!! */


static SXBA
  source_t_set /* SXEOF+1 */,
  int_set /* MAXNT+1 */,
  nt_set /* MAXNT+1 */,
  new_rl_control_set /* MAXITEM+1 */,
  closure_set /* MAXITEM+1 */,
  or_and_t_set /* SXEOF+1 */,
  working_titem_set /* MAXITEM+1 */,
  ;

static SXINT
  *nt_stack /* MAXNT+1 */
  ;

/* "parseur" fonde' sur un set-automaton (la structure de controle est un ensemble et pas une pile)
   C'est un automate fini
   Analyse en temps lineaire */
/* Il peut etre utilise comme shallow parseur (option -Dis_shallow=1) et/ou comme supertaggeur
   (option -Dis_supertagging=1) ou comme "lexicaliseur" (option -Dis_lex=1) */

static SXBA           rl_valid_item_set_i, lr_valid_item_set_i, cur_titem_set, ilex_compatible_item_set;
static SXBA           *mlstn2lexicalized_look_ahead_t_set;

/* On utilise la relation left_corner (et right_corner) */
/* valid_prefixes avec nt2lc_item_set [1] */
/* A chaque etape, si c'est la 1ere fois qu'on voit un A, on ajoute
   nt2lc_item_set [A] */

/* left_corner = {(A, B) | A =>* \alpha B \beta =>* B \beta} */
/* nt2lc_item_set [A] = {B -> \alpha . \beta | B left_corner A et \alpha =>* \epsilon} */

/* right_corner = {(A, B) | A =>* \alpha B \beta =>* \alpha B} */
/* nt2rc_item_set [A] = {B -> \alpha . \beta | B right_corner A et \beta =>* \epsilon} */

/* valeurs initiales ds les tables */
/* titem_set = {A -> \alpha .t \beta | A -> \alpha t \beta \in P} */
/* rc_titem_set = {A -> \alpha t . \beta | A -> \alpha t \beta \in P} */


/* NOUVELLE VERSION
   On fait un passage gauche droite qui note pour chaque position du source l'active set calcule.
   Puis, par un passage droite gauche, on ne conserve ds chaque active set que les items connectes
   a l'item final [S' -> $ S. $]
   C'est +precis que set_automaton_parser () => on conserve
   -Dis_lrsa est donc sans effet...
 */

/* Structure intermediaire qui a chaque terminal t qui intervient ds une transition du dag depuis (ou vers)
   un noeud, associe l'ensemble des triplets (lpos, tok_no, rpos).  Bien sur on a lahead(tok_no)==t */
static SXBA           next_mlstn_set;
static SXBA           active_set, control_set, rl_active_set, rl_control_set;
static SXBA           *active_sets, *control_sets, *rl_active_sets, *rl_control_sets;



/* On vient de reconnaitre U (item = A -> \alpha U . \beta) */
/* Changement le 09/10/06 Si \beta == \varepsilon, on met item ds active_set sinon on le met ds closure_set
   il pourra comme c,a etre filtre'par la lexicalization */

static void close_reduce (item)
     SXINT item;
{
  SXINT X, prod, bot, A;

  X = LISPRO (item);

  if (IS_EPSILON) {
    while (X != 0) {
      SXBA_1_bit (closure_set, item);

      if (X < 0) {
	return;
      }

      if (SXBA_bit_is_reset_set (int_set, X)) {
	/* C'est la 1ere fois qu'on ferme X au niveau i */
	sxba_or (closure_set, nt2lc_item_set [X]);
      }

      if (!SXBA_bit_is_set (EMPTY_SET, X))
	return;

      item++;
      SXBA_1_bit (closure_set, item);
      X = LISPRO (item);
    }
  } else { /* !is_epsilon */
    SXBA_1_bit (closure_set, item);

    if (X != 0) {
      if (X > 0 && SXBA_bit_is_reset_set (int_set, X)) {
	/* C'est la 1ere fois qu'on ferme X au niveau i */
	sxba_or (closure_set, nt2lc_item_set [X]);
      }

      return;
    }
  } /* !is_epsilon */

  /* On a reconnu prod */
  /* Elle couvre une sous-chaine non vide */
  SXBA_1_bit (active_set, item);
  prod = PROLIS (item);

  A = LHS (prod);

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    /* premiere reduction vers A */
    PUSH (nt_stack, A);
  }
}

/* On vient de reconnaitre U (item = A -> \alpha . U \beta) */

static void rl_close_reduce (SXINT item)
{
  SXINT X, prod, A, bot;

  SXBA_1_bit (closure_set, item);
  bot = PROLON (prod = PROLIS (item));

  if (IS_EPSILON) {
    while (item > bot) {
      X = LISPRO (item-1);

      if (X < 0) {
	return;
      }

      if (SXBA_bit_is_reset_set (int_set, X)) {
	/* C'est la 1ere fois qu'on ferme X au niveau i */
	sxba_or (closure_set, NT2RC_ITEM_SET (X));
      }

      if (!SXBA_bit_is_set (EMPTY_SET, X))
	return;

      item--;
      SXBA_1_bit (closure_set, item);
    }
  }
  else { /* !is_epsilon */
    if (item > bot) {
      X = LISPRO (item-1);

      if (X > 0 && SXBA_bit_is_reset_set (int_set, X)) {
	/* C'est la 1ere fois qu'on ferme X au niveau i */
	sxba_or (closure_set, NT2RC_ITEM_SET (X));
      }

      return;
    }
  } /* !is_epsilon */

  /* On a reconnu prod */
  /* Elle couvre une sous-chaine non vide ... */
  /* ...et a ete validee par la passe gauche droite */
  if (!SXBA_bit_is_set (ilex_compatible_item_set, bot))
    sxba_range (ilex_compatible_item_set, bot, PROLON (prod+1)-bot);

  A = LHS (prod);

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    PUSH (nt_stack, A);
  }
}


/* remplit les control et active sets des etats du dynamic set automaton atteints depuis mlstn 
 de gauche a droite */
/* Nelle version du 05/10/06 qui marche pour is_dag ou !is_dag */

static bool sa_scanner (SXINT mlstn)
{
  SXINT     dag_state, trans, item, t, next_mlstn, Y, next_item;
  bool has_forward_ttrans = false;
  SXBA    next_active_set;

  /* active_set = active_sets [mlstn]; */
  sxba_2op (cur_titem_set, SXBA_OP_COPY, active_set, SXBA_OP_AND, TITEM_SET);
  /* cur_titem_set est donc la partie de l'active set qui ne contient que des titems de la forme
     A -> \alpha .t \beta */

#if is_dag
  /* On en extrait l'ensemble de ces terminaux */
  item = MAXITEM+1;

  while ((item = sxba_1_rlscan (cur_titem_set, item)) >= 0) {
    SXBA_0_bit (cur_titem_set, item);
    SXBA_1_bit (cur_titem_set, item+1); /* On pointe derriere les terminaux */
    t = -LISPRO (item);
    SXBA_1_bit (source_t_set, t);
  }

  dag_state = mlstn;

  XxYxZ_Xforeach (dag_hd, dag_state, trans) {
    /* attention le DAG peut etre non-deterministe par rapport aux t */
    Y = XxYxZ_Y (dag_hd, trans);

    if (
#if is_sdag
	sxba_2op (or_and_t_set, SXBA_OP_COPY, glbl_source [Y], SXBA_OP_AND, source_t_set)
#else /* !is_sdag */
	t = toks_buf [Y].lahead, SXBA_bit_is_set (source_t_set, t)
#endif /* !is_sdag */
	) {
      has_forward_ttrans = true;
      next_mlstn = XxYxZ_Z (dag_hd, trans);

      if (SXBA_bit_is_reset_set (next_mlstn_set, next_mlstn))
	sxba_or (control_sets [next_mlstn], control_set);

      next_active_set = active_sets [next_mlstn];

#if is_sdag
      t = -1;

      while ((t = sxba_scan (or_and_t_set, t)) >= 0) {
	sxba_2op (next_active_set, SXBA_OP_OR, cur_titem_set, SXBA_OP_AND, T2PROD_ITEM_SET (t));
      }
#else /* !is_sdag */
      sxba_2op (next_active_set, SXBA_OP_OR, cur_titem_set, SXBA_OP_AND, T2PROD_ITEM_SET (t));
#endif /* !is_sdag */
    }		  
  }

  /* Clear */
  if (has_forward_ttrans)
    sxba_empty (next_mlstn_set);

  sxba_empty (source_t_set);

#else /* !is_dag */
  next_mlstn = mlstn+1;
  next_active_set = active_sets [next_mlstn];

#if is_sdag
  glbl_source_i = glbl_source [mlstn];
#endif /* is_sdag */
  item = -1;

  while ((item = sxba_scan (cur_titem_set, item)) >= 0) {
    t = -LISPRO (item);

    if (
#if is_sdag
	SXBA_bit_is_set (glbl_source_i, t)
#else /* !is_sdag */
	glbl_source [mlstn] == t
#endif /* !is_sdag */
	) {
      has_forward_ttrans = true;
      next_item = item+1;
      SXBA_1_bit (next_active_set, next_item); /* A -> \alpha a_i . \beta */
    }
  }

  if (has_forward_ttrans) 
    sxba_or (control_sets [next_mlstn], control_set);
#endif /* !is_dag */

  return has_forward_ttrans;
}

/* remplit les control et active sets des etats du dynamic set automaton atteints depuis mlstn 
 de droite a gauche */
/* Nelle version du 05/10/06 qui marche pour is_dag ou !is_dag */

static bool sa_rlscanner (SXINT mlstn)
{
  SXINT     trans, item, prev_item, t, prev_mlstn, Y;
  bool has_backward_ttrans = false;
  SXBA    prev_active_set, look_ahead_t_set, rl_supertagger_item_set;

  /* rl_active_set = rl_active_sets [mlstn]; */
  sxba_2op (cur_titem_set, SXBA_OP_COPY, rl_active_set, SXBA_OP_AND, RC_TITEM_SET);
  /* cur_titem_set est la partie de l'active set qui ne contient que des titems de la forme
     A -> \alpha t. \beta */

#if is_dag
  /* On en extrait l'ensemble de ces terminaux */
  item = -1;

  while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
    prev_item = item-1;
    SXBA_1_bit (cur_titem_set, prev_item); /* On pointe devant les terminaux */
    t = -LISPRO (prev_item);
    SXBA_1_bit (source_t_set, t);
  }

  /* Extractions des transitions du DAG depuis mlstn */
  XxYxZ_Zforeach (dag_hd, mlstn, trans) {
    /* attention le DAG peut etre non-deterministe par rapport aux t */
    Y = XxYxZ_Y (dag_hd, trans);
    prev_mlstn = XxYxZ_X (dag_hd, trans);

    if (
#if is_sdag
	sxba_2op (or_and_t_set, SXBA_OP_COPY, glbl_source [Y], SXBA_OP_AND, source_t_set)
#else /* !is_sdag */
	t = toks_buf [Y].lahead, SXBA_bit_is_set (source_t_set, t)
#endif /* !is_sdag */
	) {
      has_backward_ttrans = true;

      if (SXBA_bit_is_reset_set (next_mlstn_set, prev_mlstn))
	sxba_or (rl_control_sets [prev_mlstn], new_rl_control_set);

      prev_active_set = rl_active_sets [prev_mlstn];

#if is_sdag
      /* Ca servira ds recognize () */
      if (mlstn2lexicalized_look_ahead_t_set)
	sxba_or (mlstn2lexicalized_look_ahead_t_set [prev_mlstn], or_and_t_set);

      t = -1;

      while ((t = sxba_scan (or_and_t_set, t)) >= 0) {
	sxba_2op (working_titem_set, SXBA_OP_OR, cur_titem_set, SXBA_OP_AND, T2PROD_ITEM_SET (t));
      }
#else /* !is_sdag */
      sxba_2op (working_titem_set, SXBA_OP_OR, cur_titem_set, SXBA_OP_AND, T2PROD_ITEM_SET (t));

      /* Ca servira ds recognize () */
      if (mlstn2lexicalized_look_ahead_t_set)
	SXBA_1_bit (mlstn2lexicalized_look_ahead_t_set [prev_mlstn], t);
#endif /* !is_sdag */

      sxba_or (prev_active_set, working_titem_set);

      if (i2rl_supertagger_item_set)
	/* PB : que note-t'on ds le cas d'un supertaggeur ? */
	/* Pour l'instant, je choisis tok_no */
	sxba_or (i2rl_supertagger_item_set [Y], working_titem_set);
    }		  
  }

  /* Clear */
  if (has_backward_ttrans)
    sxba_empty (next_mlstn_set);

  sxba_empty (source_t_set);
  sxba_empty (cur_titem_set);
#else /* !is_dag */
  prev_mlstn = mlstn-1;
  prev_active_set = rl_active_sets [prev_mlstn];

  /* Ca servira ds recognize () */
  if (mlstn2lexicalized_look_ahead_t_set)
    look_ahead_t_set = mlstn2lexicalized_look_ahead_t_set [prev_mlstn];

  if (i2rl_supertagger_item_set)
    /* PB : que note-t'on ds le cas d'un supertaggeur ? */
    /* Pour l'instant, je choisis tok_no */
    rl_supertagger_item_set = i2rl_supertagger_item_set [prev_mlstn];

#if is_sdag
  glbl_source_i = glbl_source [prev_mlstn];
#endif /* is_sdag */

  item = -1;

  while ((item = sxba_scan (cur_titem_set, item)) >= 0) {
    prev_item = item-1;
    t = -LISPRO (prev_item);

    if (
#if is_sdag
	SXBA_bit_is_set (glbl_source_i, t)
#else /* !is_sdag */
	glbl_source [prev_mlstn] == t
#endif /* !is_sdag */
	) {
      has_backward_ttrans = true;
      SXBA_1_bit (prev_active_set, prev_item); /* A -> \alpha . a_i \beta */

      if (mlstn2lexicalized_look_ahead_t_set)
	SXBA_1_bit (look_ahead_t_set, t);

      if (i2rl_supertagger_item_set)
	/* PB : que note-t'on ds le cas d'un supertaggeur ? */
	/* Pour l'instant, je choisis tok_no */
	sxba_1_bit (rl_supertagger_item_set, prev_item);
    }
  }

  if (has_backward_ttrans) {
    sxba_or (rl_control_sets [prev_mlstn], new_rl_control_set);
  }
#endif /* !is_dag */

  return has_backward_ttrans;
}




static bool lr_pass_succeeds;
static SXBA    source2valid_item_set;

/* Ici on est ds le cas is_dag ou !is_dag */
/* Si l'appelant veut associer a chaque dag_state l'ensemble des terminaux selectionnes par le
   dynamic_set_automaton, il passe mlstn2lex_la_tset non vide qui sera rempli */

bool dynamic_set_automaton (SXBA *i2supertagger_item_set,
		       bool is_mlstn2lex_la_tset,
		       bool is_mlstn2la_tset)
{
  SXINT  t, item, item_1, X, i, A, bot, top, prod, mlstn; 
  SXBA supertagger_item_set;

#if is_lex2 || is_lex
  SXINT nt;
  SXINT store_npd [inputG_MAXNT+1];

#ifdef MAKE_INSIDEG
  SXINT  *store_numpd;
#else /* MAKE_INSIDEG */
  SXINT  store_numpd [sizeof(numpd)/sizeof(*numpd)];
#endif /* MAKE_INSIDEG */
#else /* !(is_lex2 || is_lex) */
  SXINT  *store_npd, *store_numpd;
#endif /* !(is_lex2 || is_lex) */

#ifdef MAKE_INSIDEG
  SXBA w_set1, w_set2;
#else /* !MAKE_INSIDEG */
  SXBA_CST_ALLOC (w_set2, inputG_MAXITEM+1);
  SXBA_CST_ALLOC (w_set1, inputG_MAXITEM+1);
#endif /* !MAKE_INSIDEG */

#if is_lex2
  mlstn = (SXINT) lexicalizer2indexed_item_sets (false, is_mlstn2la_tset);
#else /* !is_lex2 */
#  if is_lex
#    ifdef MAKE_INSIDEG
  w_set2 = sxba_calloc (inputG_MAXITEM+1);
#    else /* !MAKE_INSIDEG */
  sxba_empty (w_set2);
#    endif /* !MAKE_INSIDEG */
  mlstn = (SXINT) lexicalizer2basic_item_set (false, is_mlstn2la_tset);
#  else /* !is_lex */
  mlstn = 0;

  if (is_mlstn2la_tset) {
    mlstn2la_tset = sxbm_calloc (1+n+1+2, inputG_SXEOF+1);
    /* Il faut le calculer */
    idag_source_processing (SXEOF, t2suffix_t_set, mlstn2suffix_source_set, mlstn2la_tset, t2la_t_set);
  }
#  endif /* !is_lex */
#endif /* !is_lex2 */

#ifdef MAKE_INSIDEG
  store_numpd = (SXINT*) sxalloc (spf.insideG.sizeofnumpd, sizeof (SXINT));
#endif /* MAKE_INSIDEG */

  if (mlstn) {
#if is_lex2
    source2valid_item_set = indexed_item_sets [idag.init_state];
#else /* !is_lex2 */
    source2valid_item_set = basic_index_set;
#endif /* !is_lex2 */

    if (is_mlstn2lex_la_tset)
      mlstn2lexicalized_look_ahead_t_set = mlstn2lex_la_tset = sxbm_calloc (1+n+1+2, inputG_SXEOF+1);

    source_t_set = sxba_calloc (SXEOF+1);
    int_set = sxba_calloc (MAXNT+1);
    nt_set = sxba_calloc (MAXNT+1);
    new_rl_control_set = sxba_calloc (MAXITEM+1);
    closure_set = sxba_calloc (MAXITEM+1);
    or_and_t_set = sxba_calloc (SXEOF+1);
    working_titem_set = sxba_calloc (MAXITEM+1);

    nt_stack = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), nt_stack [0] = 0;

    i2rl_supertagger_item_set = i2supertagger_item_set;

#ifdef MAKE_INSIDEG
    w_set1 = sxba_calloc (spf.insideG.maxitem+1);
#endif /* MAKE_INSIDEG */

    cur_titem_set = &(w_set1 [0]);
    active_sets = sxbm_calloc (idag.final_state+1, MAXITEM+1);
    control_sets = sxbm_calloc (idag.final_state+1+max_rcvr_tok_no, MAXITEM+1); /* +max_rcvr_tok_no car rl_control_sets==control_sets */
    next_mlstn_set = sxba_calloc (idag.final_state+1+max_rcvr_tok_no);

    /* On ne touche pas aux tables statiques */
#if is_lex2 || is_lex
    /* On fabrique les nouveaux npd/numpd en tenant compte de source2valid_item_set */
    /* Pour remettre en l'etat... */
    for (nt = 0; nt <= MAXNT; nt++) {
      store_numpd [store_npd [nt] = NPD (nt+1)-1] = 0; /* On pointe vers le 0 de fin de liste (liste vide) */
    }

    item = -1;

    while ((item = sxba_scan (source2valid_item_set, item)) >= 0) {
      if ((X = LISPRO (item)) > 0) {
	store_numpd [--store_npd [X]] = item; /* On ajoute item a la liste */
      }
    }
#else /* !(is_lex2 || is_lex) */
    /* On utilise les tables statiques */
    store_npd = &(NPD (0));
    store_numpd = &(NUMPD (0));
#endif /* !(is_lex2 || is_lex) */

    /* On part des items initiaux valides par la lexicalisation et connectes a S' -> $ . S $ */
    control_set = control_sets [idag.init_state]; /* transition sur le eof initial!! */
    active_set = active_sets [idag.init_state];
    /* Simulation du close-reduce */
    sxba_copy (control_set, nt2lc_item_set [1]);
    SXBA_1_bit (control_set, 1); /* Le 23/06/2003, j'ajoute [S' -> $ . S $] */

#if is_lex || is_lex2
    sxba_2op (active_set, SXBA_OP_COPY, control_set, SXBA_OP_AND, source2valid_item_set);
#else /* !(is_lex || is_lex2) */
    sxba_copy (active_set, control_set);
#endif /* !(is_lex || is_lex2) */

    if (sa_scanner (idag.init_state)) {
      /* Il y a des transitions terminales depuis l'etat initial */
      /* Sinon, pas de traitement d'erreur ds la phase gauche-droite, on se contente de l'abandonner */
      for (mlstn = idag.init_state+1; mlstn <= idag.final_state; mlstn++) {
	control_set = control_sets [mlstn];
	active_set = active_sets [mlstn];
	sxba_empty (closure_set);

	item = -1;

	/* active_set = {A -> \alpha t. \beta | ? ->t mlstn} */
	while ((item = sxba_scan (active_set, item)) > 0) {
	  /* close_reduce () met uniquement ds active_set les items reduce */
	  close_reduce (item);
	}

	if (!IS_EMPTY (nt_stack)) {
	  do {
	    A = POP (nt_stack);
	    /* On cherche toutes les occurrences de A en RHS telle que B -> \alpha . A \beta soit ds control_set */
	    bot = store_npd [A];

	    while (item = store_numpd [bot++]/* B -> \alpha . A  \beta */) {
	      if (SXBA_bit_is_set (control_set, item)) {
		close_reduce (item+1);
	      }
	    }
	  } while (!IS_EMPTY (nt_stack));

	  sxba_empty (nt_set);
	}

	/* On change de niveau */
	sxba_empty (int_set);

	/* On ajoute les items de la fermeture filtres par la lexicalisation */
#if is_lex2
	sxba_2op (active_set, SXBA_OP_OR, closure_set, SXBA_OP_AND, indexed_item_sets [mlstn]);
#else /* !is_lex2 */
#if is_lex
	sxba_2op (active_set, SXBA_OP_OR, closure_set, SXBA_OP_AND, source2valid_item_set);
#else /* !is_lex */
	sxba_or (active_set, closure_set);
#endif /* !is_lex */
#endif /* !is_lex2 */

	/* On fabrique le nouveau control set... */
	sxba_or (control_set, active_set);
	/* ...qui est utilise ds sa_dag_scanner */

	if (mlstn == idag.final_state || !sa_scanner (mlstn)) {
	  /* Il n'y a pas de transitions terminales depuis l'etat courant */
	  /* pas de traitement d'erreur ds la phase gauche-droite, on se contente de l'abandonner */
	  /* Le 03/05/04 non, il ne faut pas abandonner Ex :
	     ... (ou alors | ou_alors) ...
	     Si "ou" est une ff inconnue il faut quand meme continuer, c,a pourra marcher par "ou_alors" */ 
	  /* break; */
	}
      }
    }

    lr_pass_succeeds = SXBA_bit_is_set (active_sets [idag.final_state], 2); /* [S' -> $ S. $] \in active_set */

    /* Si la passe gauche-droite a echouee et s'il n'y a pas de traitement d'erreur, c'est fini... */
    mlstn = 0; /* echec a priori */

    if (lr_pass_succeeds ) {
      /* active_set == active_sets [idag.final_state] */
      /* active_set ne contient pas des "prod_set" */
      set_automaton_item_sets = sxbm_calloc (idag.final_state+1, MAXITEM+1);

      if (IS_EPSILON) {
	/* On y met les items des productions vides pertinentes (sinon rien) */
	sxba_2op (set_automaton_item_sets [idag.final_state], SXBA_OP_COPY, EMPTY_PROD_ITEM_SET, SXBA_OP_AND, active_set);
      } /* is_epsilon */
 
      /* ... et les items de S' -> $ S $ */
      sxba_range (set_automaton_item_sets [idag.final_state], 0, 4);

      /* Analyse par le mirror dynamic set automaton dont les active sets seront filtres par ceux de l'analyse gauche-droite */
      rl_active_sets = sxbm_calloc (idag.final_state+1+max_rcvr_tok_no, MAXITEM+1);
      rl_active_set = rl_active_sets [idag.final_state];

      /* On ne conserve que les items connectes a l'item final [S' -> $ S. $] */
      /* Ca permet de supprimer de ne conserver que les prods dont on a atteint les items reduce */
      sxba_2op (rl_active_set, SXBA_OP_COPY, NT2RC_ITEM_SET (1), SXBA_OP_AND, active_sets [idag.final_state]);
      SXBA_1_bit (rl_active_set, 2); /* ... et on remet [S' -> $ S. $] */

      /* On "recycle" les control_sets (on n'a pas besoin de idag.final_state) */
      rl_control_sets = control_sets;

      for (mlstn = idag.init_state; mlstn < idag.final_state; mlstn++)
	sxba_empty (rl_control_sets [mlstn]);

      sxba_copy (new_rl_control_set, rl_active_set); /* pour sa_rlscanner */

      if (mlstn2lex_la_tset) {
	SXBA_1_bit (mlstn2lex_la_tset [0], SXEOF);
	SXBA_1_bit (mlstn2lex_la_tset [idag.final_state], SXEOF);
      }

      if (sa_rlscanner (idag.final_state)) {
	/* Les active_set et control_set des mlstns atteints par scan arriere des terminaux finals du source
	   ont ete initialises.  Ils pourront se completer + tard s'ils ont d'autres transitions sortantes */
	for (mlstn = idag.final_state-1; mlstn >= idag.init_state; mlstn--) {
	  rl_control_set = rl_control_sets [mlstn];
	  rl_active_set = rl_active_sets [mlstn];
	  sxba_empty (closure_set);

	  /* Le but est de remplir ilex_compatible_item_set */
	  ilex_compatible_item_set = set_automaton_item_sets [mlstn]; /* vide */

	  item = -1;

	  /* Tous les items de rl_active_set sont de la forme
	     A -> \alpha .t \beta, avec mlstn ->t ?? */
	  while ((item = sxba_scan (rl_active_set, item)) >= 0) {
	    /* Contrairement a close_reduce(), rl_close_reduce() ne touche pas a rl_active_set */
	    rl_close_reduce (item);
	  }

	  if (!IS_EMPTY (nt_stack)) {
	    do {
	      A = POP (nt_stack);
	      /* On cherche toutes les occurrences de A en RHS telle que B -> \alpha A . \beta soit ds rl_control_set */
	      bot = store_npd [A];

	      while (item = store_numpd [bot++]/* B -> \alpha . A  \beta */) {
		item_1 = item+1;

		if (SXBA_bit_is_set (rl_control_set, item_1)) {
		  rl_close_reduce (item);
		}
	      }
	    } while (!IS_EMPTY (nt_stack));

	    sxba_empty (nt_set);
	  }

	  /* ici closure_set est rempli ainsi que ilex_compatible_item_set avec les "reduce" non vides du miroir */

	  /* On change de niveau */
	  sxba_empty (int_set);
	  /* On filtre par le resultat de la passe gauche-droite */
	  sxba_2op (rl_active_set, SXBA_OP_OR, closure_set, SXBA_OP_AND, active_sets [mlstn]);

	  if (IS_EPSILON) {
	    /* On ajoute les productions vides des fermetures */
	    /* Si A est implique' ds closure_set et si A -> \alpha \beta avec \alpha \beta =>* \varepsilon alors A -> \alpha . \beta \in closure_set */
	    sxba_2op (ilex_compatible_item_set, SXBA_OP_OR, closure_set, SXBA_OP_AND, EMPTY_PROD_ITEM_SET);
	  } /* is_epsilon */

	  /* On fabrique le nouveau control set... */
	  sxba_2op (new_rl_control_set, SXBA_OP_COPY, rl_control_set, SXBA_OP_OR, rl_active_set);
	  /* ...qui est utilise ds sa_rlscanner */

	  /* scan arriere */
	  sa_rlscanner (mlstn); /* il peut echouer, mais d'autres trans vers "prev_mlstn" peuvent marcher */
	}
	
	if (SXBA_bit_is_set (rl_active_set, 1)) {
	  /* rl_active_set est celui calcule' pour idag.init_state */
	  /* [S' -> $ .S $] est atteint => C'est OK */
	  mlstn = 1; /* OK */
	}
      }
      else
	mlstn = 0; /* echec */

      sxbm_free (rl_active_sets), rl_active_sets = NULL;
    }

    sxbm_free (active_sets), active_sets = NULL;
    sxbm_free (control_sets), control_sets = NULL;
    sxfree (next_mlstn_set), next_mlstn_set = NULL;

#ifdef MAKE_INSIDEG
    sxfree (w_set1);
#endif /* MAKE_INSIDEG */
    sxfree (source_t_set), source_t_set = NULL;
    sxfree (int_set), int_set = NULL;
    sxfree (nt_set), nt_set = NULL;
    sxfree (new_rl_control_set), new_rl_control_set = NULL;
    sxfree (closure_set), closure_set = NULL;
    sxfree (or_and_t_set), or_and_t_set = NULL;
    sxfree (working_titem_set), working_titem_set = NULL;
    sxfree (nt_stack), nt_stack = NULL;
  }

#if is_lex
#ifdef MAKE_INSIDEG
  sxfree (w_set2);
#endif /* MAKE_INSIDEG */
#endif /* is_lex  */

#ifdef MAKE_INSIDEG
  sxfree (store_numpd);
#endif /* MAKE_INSIDEG */

#if is_lex2
  sxbm_free (indexed_item_sets), indexed_item_sets = NULL;
#else /* !is_lex2 */
#if is_lex
  sxfree (basic_item_set), basic_item_set = NULL;
  sxfree (basic_nt_set), basic_nt_set = NULL;
#endif /* is_lex */  
#endif /* is_lex2 */

  if (is_print_time) {
    char string [64];

    sprintf (string, "\tDynamic Set Automaton for %ss (%s)",
#if is_dag
			"DAG"
#else /* !is_dag */
#if is_sdag
			"SDAG"
#else /* !is_sdag */
#endif /* !is_sdag */
			"string"
#endif /* !is_dag */
	     , (mlstn > 0) ? "true" : "false");

    sxtime (TIME_FINAL, string);
  }

  return mlstn > 0;
}
#endif /* is_set_automaton */

/* Ci-dessous on a le traitement d'erreurs qui a ete enleve' au niveau lexical ... */
/* ... et les sources des anciennes versions */
/* --------------------------------------------------------------------------------------------------- */
/* Ci-dessous on a le traitement d'erreurs qui a ete enleve' au niveau lexical */
/* Pour le remettre il faudrait reconstruire entierement le dag et ses satellites a la sortie */

#include "XxY.h"
static XxY_header     glbl_dag_rcvr_txtrans;

#if is_dag
#if is_sa_rcvr

static bool sa_dag_state_rl_rcvr (SXINT mlstn)
{
  SXINT dag_state, item, t, new_dag_state, trans, rule_no, triple, prev_mlstn, prev_trans, prev_tok; 
  bool ret_val;
  SXBA rcvr_t_set;
#define last_rule_no 3
  SXINT rule_kind [last_rule_no+1] = {0, 1, 2, 3};

  if (rcvr_tok_no >= max_rcvr_tok_no)
    return false;

  rcvr_tok_no++;

  dag_state = mlstn2dag_state [mlstn];

  /* On calcule l'ensemble des terminaux compatibles avec les items de rl_active_sets [dag_state] */
  sxba_2op (cur_titem_set, SXBA_OP_OR, rl_active_sets [dag_state], SXBA_OP_AND, rc_titem_set);

  rcvr_t_set = glbl_source [last_tok_no+rcvr_tok_no];

  ret_val = false;
  item = -1;

  while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
    t = -lispro [item-1];
    SXBA_1_bit (rcvr_t_set, t);
    ret_val = true;
  }

  if (!ret_val)
    return false; 

  new_dag_state = final_pos+rcvr_tok_no;

  for (rule_no = 1; rule_no <= last_rule_no; rule_no++) {
    switch (rule_kind [rule_no]) {
    case 1: /* 0 X 1 2 */
      /* Insertion */
      /* Extractions des transitions du DAG depuis dag_state */
      XxYxZ_Zforeach (dag_hd, dag_state, trans) {
	XxYxZ_set (&dag_hd, XxYxZ_X (dag_hd, trans), XxYxZ_Y (dag_hd, trans), new_dag_state, &triple);
	/*
	  On laisse les transitions, elles peuvent etre "utilisees" par du rattrapage global !!
	  XxYxZ_erase (dag_hd, trans);
	*/
      }
      
      break;

    case 2: /* 0 2 3 */
      /* Suppression */
      XxYxZ_Zforeach (dag_hd, dag_state, trans) {
	prev_mlstn = XxYxZ_X (dag_hd, trans);
	
	XxYxZ_Zforeach (dag_hd, prev_mlstn, prev_trans) {
	  /* On pourrait peut etre verifier qu'on est a intersection non vide avec rcvr_t_set */
	  prev_tok = XxYxZ_Y (dag_hd, prev_trans);

	  if (prev_tok > last_tok_no) {
	    /* On retombe sur une transition ajoutee par le traitement d'erreur... */
	    /* ...Dois-je faire un traitement particulier ? oui */
	    if (!sxba_is_subset (glbl_source [prev_tok], rcvr_t_set))
	      /* Ca ne marchera pas */
	      continue;
	  }
	  else {
#if is_sdag
	    if (!sxba_is_subset (glbl_source [prev_tok], rcvr_t_set))
	      /* Ca ne marchera pas */
	      continue;
#else /* !is_sdag */
	    t = toks_buf [Y].lahead;

	    if (!SXBA_bit_is_set (rcvr_t_set, t))
	      /* Ca ne marchera pas */
	      continue;
#endif /* !is_sdag */
	  }

	  XxYxZ_set (&dag_hd, XxYxZ_X (dag_hd, prev_trans), prev_tok, dag_state, &triple);
	}

	/* XxYxZ_erase (dag_hd, trans); */
      }

      break;

    case 3: /* 0 X 2 */
      /* Remplacement */
      XxYxZ_Zforeach (dag_hd, dag_state, trans) {
	XxYxZ_set (&dag_hd, XxYxZ_X (dag_hd, trans), last_tok_no+rcvr_tok_no, new_dag_state, &triple);
	/* XxYxZ_erase (dag_hd, trans); */
      }
      
      break;

    default:
      break;
    }
  }

  XxYxZ_set (&dag_hd, new_dag_state, last_tok_no+rcvr_tok_no, dag_state, &triple);
	
  XxYxZ_Xforeach (dag_hd, dag_state, trans) {
    sxerror (toks_buf [XxYxZ_Y (dag_hd, trans)].source_index,
	     sxplocals.sxtables->err_titles [1][0],
	     sa_rcvr_insert_t /* "%sA \"%s\" is inserted before \"%s\"." */,
	     sxplocals.sxtables->err_titles [1]+1,
	     "word",
	     sxstrget (toks_buf [XxYxZ_Y (dag_hd, trans)].string_table_entry));
    break;
  }

  if (dag_state == new_init_pos)
    /* On change la racine du dag */
    new_init_pos = new_dag_state;

  return true;
}


/* Principe de la recuperation globale */
/*
  Cas lr (le cas miroir rl est symetrique)
  on est ds un etat mlstn/dag_state 
  Il n'existe pas ds le dag de transition sur t depuis dag_state et il n'existe pas
  ds active_set d'item de la forme A -> \alpha .t \beta
  Pour tout item ds active_set de la forme [A -> \alpha . \beta t \gamma] on recherche ds le dag
  s'il existe une transition d1 ->t d2 t.q. le mlstn de d1 soit superieur a mlstn.
  Si c'est le cas, on met [A -> \alpha \beta .t \gamma] ds active_sets [d1].
  Il peut ds le dag ne pas exister de chemin entre dag_state et d1.
  On suppose donc qu'entre dag_state et d1 il y a un chemin qui s'epelle comme une \beta-phrase.
 */

static bool glbl_sa_dag_state_rl_rcvr (SXINT mlstn)
{
  SXINT dag_state, item, left_item, bot_item, Y, t, trans, next_mlstn, it, next_dag_state, txtrans, right_trans; 
  bool ret_val;
  SXBA active_set, control_set, glbl_source_i;

  dag_state = mlstn2dag_state [mlstn];

  active_set = rl_active_sets [dag_state];
  control_set = rl_control_sets [dag_state];

  if (!XxY_is_allocated (glbl_dag_rcvr_txtrans)) {
    sxprepare_for_possible_reset (glbl_dag_rcvr_txtrans.is_allocated);
    XxY_alloc (&glbl_dag_rcvr_txtrans, "glbl_dag_rcvr_txtrans", 2*sxba_cardinal (idag.source_set), 1, 1, 0, NULL, NULL);

    /* On ne fait du rattrapage global que sur les "vrais" terminaux du source... */
    for (trans = 1; trans <= top_dag_trans; trans++) {
      Y = XxYxZ_Y (dag_hd, trans);
#if is_sdag
      glbl_source_i = glbl_source [Y];
      t = 0;

      while ((t = sxba_scan (glbl_source_i, t)) > 0) {
	XxY_set (&glbl_dag_rcvr_txtrans, t, trans, &txtrans);
      }
#else /* !is_sdag */
      t = toks_buf [Y].lahead;
      XxY_set (&glbl_dag_rcvr_txtrans, t, trans, &txtrans);
#endif /* !is_sdag */
    }
  }

  /* Attention, des etats deja calcules  et touches par le traitement d'erreur doivent etre recalcules */
  ret_val = false;

  for (trans = 1; trans <= top_dag_trans; trans++) {
    next_dag_state = XxYxZ_Z (dag_hd, trans);
    next_mlstn = dag_state2mlstn [next_dag_state];
	      
    if (next_mlstn < mlstn) {
      ret_val = true;
      Y = XxYxZ_Y (dag_hd, trans);
#if is_sdag
      sxba_or (source_t_set, glbl_source [Y]);
#else /* !is_sdag */
      t = toks_buf [Y].lahead;
      SXBA_1_bit (source_t_set, t);
#endif /* !is_sdag */
    }
  }

  if (ret_val) {
    ret_val = false;
    item = -1;

    while ((item = sxba_scan (active_set, item)) >= 0) {
      bot_item = prolon [prolis [item]];

      for (left_item = item-2; left_item >= bot_item; left_item--) {
	if ((t = -lispro [left_item]) > 0 && SXBA_bit_is_set (source_t_set, t)) {
	  XxY_Xforeach (glbl_dag_rcvr_txtrans, t, txtrans) {
	    trans = XxY_Y (glbl_dag_rcvr_txtrans, txtrans);
	    next_dag_state = XxYxZ_Z (dag_hd, trans);
	    next_mlstn = dag_state2mlstn [next_dag_state];

	    if (next_mlstn < mlstn) {
	      /* Il peut ne pas y avoir de chemin ds le dag entre next_dag_state et dag_state */
	      if (SXBA_bit_is_set (mlstn_active_set, next_dag_state)
		  && SXBA_bit_is_set_reset (mlstn_already_processed, next_mlstn)) {
		/* non vide... */
		/* ... et non (deja) filtre' */
		sxba_and (rl_active_sets [next_dag_state], titem_set);
	      }

	      SXBA_1_bit (rl_active_sets [next_dag_state], left_item+1);
	      SXBA_1_bit (mlstn_active_set, next_dag_state);
	      sxba_or (rl_control_sets [next_dag_state], control_set);

	      if (!ret_val) {
		/* 1er coup */
		XxYxZ_Xforeach (dag_hd, dag_state, right_trans) {
		  sxerror (toks_buf [XxYxZ_Y (dag_hd, right_trans)].source_index,
			   sxplocals.sxtables->err_titles [2][0],
			   sa_rcvr_start_point /* "%sGlobal set automaton right to left recovery on \"%s\"." */,
			   sxplocals.sxtables->err_titles [2]+1,
			   sxstrget (toks_buf [XxYxZ_Y (dag_hd, right_trans)].string_table_entry));
		  break;
		}
	      }
    
	      sxerror (toks_buf [XxYxZ_Y (dag_hd, trans)].source_index,
		       sxplocals.sxtables->err_titles [2][0],
		       sa_rcvr_resume_point /* "%sParsing resumes on \"%s\"." */,
		       sxplocals.sxtables->err_titles [2]+1,
		       sxstrget (toks_buf [XxYxZ_Y (dag_hd, trans)].string_table_entry));

	      ret_val = true;
	    }
	  }
	}
      }
    }
  }
	  
  sxba_empty (source_t_set);

  return ret_val;
}

static SXINT sa_dag_rcvr (SXINT bot_mlstn)
{
  SXINT prev_rcvr_tok_no, dead_end, rcvr_mlstn, dag_state, mlstn; 

  /* L'erreur a ete detectee sur bot_mlstn */
  /* Les etats du sa associes a bot_mlstn sont vides, ainsi que les etats associes aux mlstn m < bot_mlstn */

  /* dead_ends contient les milestones sans predecesseurs */
  prev_rcvr_tok_no = rcvr_tok_no;

  rcvr_mlstn = dead_end = 0;

  while ((dead_end = sxba_scan (dead_ends, dead_end)) > 0) {
    if (sa_dag_state_rl_rcvr (dead_end)) {
      /* On note le plus grand milestone qui a marche' */
      rcvr_mlstn = dead_end;
    }
  }

  /* Il faut eviter de boucler sur des insertions non bornees */
  /* Le nombre d'insertion est limite par max_rcvr_tok_no, est-ce suffisant ? */

  if (rcvr_mlstn == 0) {
    /* La correction a echouee, on essaie du traitement + global */
    /* Est-ce que ca vaut le coup si on est en train de guider du Earley car il faudra recommencer ? */  
    /* Oui : si le traitement d'erreur en SA a marche', les items ajoute's servirons de guide et permettront
       de  filtrer les items ajoute's par le traitement d'erreur earley */
    /* Inversement, si le traitement d'erreur a echoue en SA, peut-il reussir en earley ? */
    /* Non si la recup d'erreur SA definit un sur-langage de la recup earley */
    /* principe : Si A -> \alpha \beta . \gamma est ds rl_active_sets [dead_end], si
       t \in Last(\alpha), si (i ->t j) \in dag et si dag_state2mlstn[j] est non encore calcule
       alors on met A -> \alpha . \beta \gamma ds rl_active_sets [dag_state2mlstn[j]] */
    while ((dead_end = sxba_scan (dead_ends, dead_end)) > 0) {
      if (glbl_sa_dag_state_rl_rcvr (dead_end)) {
	rcvr_mlstn = dead_end;
      }
    }
  }

  if (rcvr_mlstn > 0) {
    /* Le traitement d'erreur a marche... */
    /* ... On va repartir (de droite a gauche) sur le milestone rcvr_mlstn */
    /* Attention on peut avoir m1 < rcvr_mlstn < m2 et m1 dependant de m2. */
    /* cependant, comme la recup d'erreur ajoute des transitions, les ensembles d'items
       sont croissants.  On peut donc se contenter de toucher aux active sets en ne
       conservant que les transitions terminales */
    for (mlstn = bot_mlstn+1; mlstn < rcvr_mlstn; mlstn++) {
      dag_state = mlstn2dag_state [mlstn];

      if (SXBA_bit_is_set (mlstn_active_set, dag_state)
	  && SXBA_bit_is_set_reset (mlstn_already_processed, mlstn)) {
	/* non vide... */
	/* ... et non (deja) filtre' */
	/* On ne conserve que les items de la forme A -> \alpha . t \beta */
	/* pour les futurs rl_close_reduce() */
	sxba_and (rl_active_sets [dag_state], titem_set);
      }
    }
  }
  /*
    else
    La ca merde vraiment... 
  */

  /* Il faut remettre en etat les structures du dag... */
  /* ... En particulier les nouveaux milestones doivent etre tries */
  if (prev_rcvr_tok_no < rcvr_tok_no) {
    /* On a cree rcvr_tok_no-prev_rcvr_tok_no nouveaux etats au dag state 
       (final_pos+prev_rcvr_tok_no+1, ..., final_pos+rcvr_tok_no) */
    SXINT new_mlstn, pos, trans, X, Z; 
    SXBA tbp, ap, new_mlstn_active_set, new_mlstn_already_processed;
    SXINT *new_dag_state2mlstn, *new_mlstn2dag_state;
    bool done;

    tbp = sxba_calloc (final_pos+rcvr_tok_no+1);
    ap = sxba_calloc (final_pos+rcvr_tok_no+1);
    new_dag_state2mlstn = (SXINT*) sxalloc (final_pos+1+rcvr_tok_no, sizeof (SXINT));
    new_mlstn2dag_state = (SXINT*) sxalloc (final_pos+1+rcvr_tok_no, sizeof (SXINT));
    new_dag_state2mlstn [0] = new_mlstn2dag_state [0] = 0;

    /* Attention, il faut partir de la [nouvelle] racine du dag */
    SXBA_1_bit (tbp, new_init_pos);
    done = false;
    new_mlstn = 0;

    while (!done) {
      done = true;

      pos = sxba_scan (tbp, 0);

      XxYxZ_Zforeach (dag_hd, pos, trans) {
	X = XxYxZ_X (dag_hd, trans);

	if (!SXBA_bit_is_set (ap, X))
	  break;
      }

      if (trans == 0) {
	/* Tous les predecesseurs de pos sont calcules */
	new_mlstn2dag_state [++new_mlstn] = pos;
	new_dag_state2mlstn [pos] = new_mlstn;
	SXBA_0_bit (tbp, pos);
	SXBA_1_bit (ap, pos);

	XxYxZ_Xforeach (dag_hd, pos, trans) {
	  Z = XxYxZ_Z (dag_hd, trans);
	  SXBA_1_bit (tbp, Z);
	  done = false;
	}
      }
    }

    sxfree (tbp);
    sxfree (ap);

    /* On va repartir d'ici */
    rcvr_mlstn = new_dag_state2mlstn [mlstn2dag_state [rcvr_mlstn]];
    sxba_empty (dead_ends);

    new_mlstn_already_processed = sxba_calloc (idag.final_state+1+rcvr_tok_no);
    mlstn = 0;

    while ((mlstn = sxba_scan (mlstn_already_processed, mlstn)) > 0) {
      new_mlstn = new_dag_state2mlstn [mlstn2dag_state [mlstn]];
      SXBA_1_bit (new_mlstn_already_processed, new_mlstn);
    }

    sxfree (mlstn_already_processed);
    mlstn_already_processed = new_mlstn_already_processed;

    sxfree (dag_state2mlstn);
    sxfree (mlstn2dag_state);

    dag_state2mlstn = new_dag_state2mlstn;
    mlstn2dag_state = new_mlstn2dag_state;
  }

  /* la passe gauche droite a pu quand m marcher, tant pis... */
  lr_pass_succeeds = false;

  if (rcvr_mlstn) {
    dag_state = mlstn2dag_state [rcvr_mlstn];
    /* On fabrique le nouveau control set... */
    sxba_2op (new_rl_control_set, SXBA_OP_COPY, rl_control_sets [dag_state], SXBA_OP_OR, rl_active_sets [dag_state]);
    /* ...qui est utilise ds sa_dag_rlscanner */
    sa_dag_rlscanner (dag_state);
    SXBA_1_bit (mlstn_already_processed, rcvr_mlstn);
  }

  return rcvr_mlstn;
}

if (XxY_is_allocated (glbl_dag_rcvr_txtrans)) XxY_free (&glbl_dag_rcvr_txtrans);
#endif /* is_sa_rcvr */
#endif /* is_dag */


#if is_set_automaton || is_rcvr
static SXBA_CST_ALLOC (source_t_set, eof+1);
#endif /* is_set_automaton || is_rcvr */

#if is_dag


#if is_lex
/* Cette procedure remplit source2valid_item_set avec l'ensemble des items (tous les items d'une production)
   qui peuvent etre selectionnees en ne tenant compte que de la lexicalisation due globalement au source.
   (Elle ne realise donc pas un decoupage du source par index, contrairement a dynamic_lexicalizer).
   Elle tient compte des ancres multiples et de leur ordre d'apparition dans les productions multiples. */
/* Nouvelle version le 16/08/06 A TESTER
   Changement ds le traitement des ancres multiples
   Les productions selectionnes doivent etre accessibles depuis l'axiome */

#if 0

static void basic_lexicalizer (SXBA source2valid_item_set)
{
  SXINT     t, item, prev_prod, prev_t, prod, bot, top, i, j; 
  SXINT     mlstn, next_mlstn, dag_state, trans, Y, Z;
  SXBA    glbl_source_i, mlstn_glbl_source_set, next_mlstn_glbl_source_set, valid_item_set;
  SXBA    *t2right_t_set, *mlstn2glbl_source_set;

  if (multiple_t_item_set) {
    /* t2right_t_set [t1] = {t | existe un chemin du source de la forme "eof ... t1 ... t ... eof" */
    /* autrement dit, t peut suivre t1 */
    t2right_t_set = sxbm_calloc (eof+1, eof+1);
    mlstn2glbl_source_set = sxbm_calloc (idag.final_state+2, eof+1);

    SXBA_1_bit (mlstn2glbl_source_set [idag.final_state+1], eof);

    for (mlstn = idag.final_state; mlstn >= idag.init_state; mlstn--) {
      mlstn_glbl_source_set = mlstn2source_set [mlstn];

      dag_state = mlstn2dag_state [mlstn];

      XxYxZ_Xforeach (dag_hd, dag_state, trans) {
	Y = XxYxZ_Y (dag_hd, trans);
	next_mlstn = dag_state2mlstn [XxYxZ_Z (dag_hd, trans)];
	next_mlstn_glbl_source_set = mlstn2glbl_source_set [next_mlstn];

#if is_sdag
	trans_source_set = glbl_source [Y];

	t = -1;

	while ((t = sxba_scan (trans_source_set, t)) >= 0) {
	  sxba_or (t2right_t_set [t], next_mlstn_glbl_source_set);
	}

	sxba_or (mlstn_glbl_source_set, next_mlstn_glbl_source_set);
	sxba_or (mlstn_glbl_source_set, trans_source_set);
#else /* !is_sdag */
	t = toks_buf [Y].lahead;
	sxba_or (t2right_t_set [t], next_mlstn_glbl_source_set);
	sxba_or (mlstn_glbl_source_set, next_mlstn_glbl_source_set);
	SXBA_1_bit (mlstn_glbl_source_set, t);
#endif /* !is_sdag */
      }
    }
  
    sxbm_free (mlstn2glbl_source_set);
  }

  valid_item_set = sxba_calloc (itemmax+1);

  /* On calcule les items valides du fait du source */
  if (sxba_cardinal (idag.source_set) > split_val) {
    t = 0;

    while ((t = sxba_0_lrscan (idag.source_set, t)) > 0) {
      sxba_or (valid_item_set, t2prod_item_set [t]);
    }

    sxba_not (valid_item_set); /* Ce qui n'est pas interdit est permis */

#if has_Lex
    /* On supprime aussi les items invalides dus aux &Lex */
    sxba_and (valid_item_set, Lex_prod_item_set);
    equiv_prod_Lex_filter (valid_item_set);
#endif /* has_Lex */

    if (multiple_t_item_set) {
      /* On verifie en plus l'ordre des ancres multiples */
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      sxba_and (multiple_t_item_set, valid_item_set);

      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (multiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (prev_prod != prod) {
	  /* 1ere ancre de prod */
	  prev_prod = prod;
	  prev_t = t;
	}
	else {
	  /* ancres suivantes */
	  if (!SXBA_bit_is_set (t2right_t_set [prev_t], t)) {
	    /* echec, t ne suit pas prev_t ds le source */
	    bot = prolon [prod];
	    SXBA_0_bit (valid_item_set, bot); /* ca suffit */
	    /* On "saute" prod */
	    item = prolon [prod+1]-1;
	  }
	  else
	    prev_t = t;
	}
      }
    }
  }
  else {
    sxba_copy (valid_item_set, t2prod_item_set [0]);
    t = 0;

    while ((t = sxba_scan (idag.source_set, t)) >= 0) {
      sxba_or (valid_item_set, t2prod_item_set [t]);
    }

#if has_Lex
    /* On supprime aussi les items invalides dus aux &Lex */
    sxba_and (valid_item_set, Lex_prod_item_set);
    equiv_prod_Lex_filter (valid_item_set);
#endif /* has_Lex */

    if (multiple_t_item_set) {
      /* On verifie en plus l'ordre des ancres multiples */
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      sxba_and (multiple_t_item_set, valid_item_set);

      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (multiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (!SXBA_bit_is_set (idag.source_set, t)) {
	  /* prod est a ancre multiples, mais toutes ses ancres ne sont pas ds le source */
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  bot = prolon [prod];
	  SXBA_0_bit (valid_item_set, bot); /* ca suffit */
	  /* On "saute" prod */
	  item = prolon [prod+1]-1;
	}
	else {
	  /* On verifie l'ordre */
	  if (prev_prod != prod) {
	    /* 1ere ancre de prod */
	    prev_prod = prod;
	    prev_t = t;
	  }
	  else {
	    /* ancres suivantes */
	    if (!SXBA_bit_is_set (t2right_t_set [prev_t], t)) {
	      /* echec, t ne suit pas prev_t ds le source */
	      bot = prolon [prod];
	      SXBA_0_bit (valid_item_set, bot); /* ca suffit */
	      /* On "saute" prod */
	      item = prolon [prod+1]-1;
	    }
	    else
	      prev_t = t;
	  }
	}
      }
    }
  }

  if (multiple_t_item_set) {
    sxbm_free (t2right_t_set);
  }

  /* On ne conserve que les prod accessibles depuis l'axiome */
  nt_set = sxba_calloc (ntmax + 1);
  nt_stack = (SXINT*) sxalloc (ntmax + 1, sizeof (SXINT)), nt_stack [0] = 0;

  SXBA_1_bit (nt_set, 1);
  PUSH (nt_stack, 1);

  while (!IS_EMPTY (nt_stack)) {
    A = POP (nt_stack);

    bot = npg [A];
    top = npg [A+1];

    while (bot < top) {
      prod = numpg [bot];
      item = prolon [prod];

      if (SXBA_bit_is_set (valid_item_set, item)) {
	SXBA_1_bit (source2valid_item_set, item);

	while ((X = lispro [item]) != 0) {
	  if (X > 0 && SXBA_bit_is_reset_set (nt_set, X))
	    PUSH (nt_stack, X);

	  item++;
	  SXBA_1_bit (source2valid_item_set, item);
	}
      }

      bot++;
    }
  }

  sxfree (nt_set);
  sxfree (nt_stack);
  sxfree (valid_item_set);
}
#endif /* 0 */

static void basic_lexicalizer (SXBA source2valid_item_set)
{
  SXINT t, item, prev_prod, prod, bot, top, i, j; 
  SXINT mlstn, dag_state, trans, Y, Z, flip_flop;
  SXBA *t2hd_mlstn_set, path, left_mlstn_set, next_mlstn_set, right_mlstn_set, *wset, glbl_source_i;
  bool is_empty;

  if (multiple_t_item_set) {
    t2hd_mlstn_set = sxbm_calloc (eof+1, idag.final_state+1);
    mlstn_path = sxbm_calloc (idag.final_state+1, idag.final_state+1);
    wset = sxbm_calloc (2, idag.final_state+1);

    /* on calcule t2hd_mlstn_set, ca sert a verifier l'ordre des ancres multiples... */
    /* ...et mlstn_path [i] qui est l'ensemble des milestones j suivant strictement i (i < j) et connectes a i (i ->x j, x \in T^+) */

    for (mlstn = idag.final_state; mlstn >= idag.init_state; mlstn--) {
      dag_state = mlstn2dag_state [mlstn];
      path = mlstn_path [mlstn];

      XxYxZ_Xforeach (dag_hd, dag_state, trans) {
	Y = XxYxZ_Y (dag_hd, trans);
#if is_sdag
	glbl_source_i = glbl_source [Y];
	t = 0;

	while ((t = sxba_scan (glbl_source_i, t)) > 0) {
	  SXBA_1_bit (t2hd_mlstn_set [t], mlstn);
	}
#else /* !is_sdag */
	t = toks_buf [Y].lahead;
	SXBA_1_bit (t2hd_mlstn_set [t], mlstn);
#endif /* !is_sdag */
	Z = dag_state2mlstn [XxYxZ_Z (dag_hd, trans)];
	SXBA_1_bit (path, Z);
	/* On ferme au fur et a mesure (c'est un DAG et ses noeuds sont ordonnes) */
	sxba_or (path, mlstn_path [Z]);
      }
    }
  }

  /* On calcule les items valides du fait du source */
  if (sxba_cardinal (idag.source_set) > split_val) {
    t = 0;

    while ((t = sxba_0_lrscan (idag.source_set, t)) > 0) {
      sxba_or (source2valid_item_set, t2prod_item_set [t]);
    }

    sxba_not (source2valid_item_set); /* Ce qui n'est pas interdit est permis */

#if has_Lex
    /* On supprime aussi les items invalides dus aux &Lex */
    sxba_and (source2valid_item_set, Lex_prod_item_set);
    equiv_prod_Lex_filter (source2valid_item_set);
#endif /* has_Lex */

    if (multiple_t_item_set) {
      /* On verifie en plus l'ordre des ancres multiples */
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      sxba_and (multiple_t_item_set, source2valid_item_set);

      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (multiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (prev_prod != prod) {
	  /* 1ere ancre de prod */
	  flip_flop = 0;
	  left_mlstn_set = t2hd_mlstn_set [t]; /* ancres gauches */
	  prev_prod = prod;
	  next_mlstn_set = wset [flip_flop++ % 2];
	  sxba_empty (next_mlstn_set);
	}
	else {
	  /* ancres suivantes */
	  right_mlstn_set = t2hd_mlstn_set [t];

	  is_empty = true;
	  i = -1;

	  while ((i = sxba_scan (left_mlstn_set, i)) >= 0) {
	    if (sxba_or_and (next_mlstn_set, mlstn_path [i], right_mlstn_set))
	      is_empty = false;
	  }

	  if (is_empty) {
	    /* echec */
	    bot = prolon [prod];
	    top = prolon [prod+1];
	    sxba_clear_range (source2valid_item_set, bot, top-bot);
	    /* On "saute" prod */
	    item = top-1;
	  }
	  else {
	    left_mlstn_set = next_mlstn_set;
	    next_mlstn_set = wset [flip_flop++ % 2];
	    sxba_empty (next_mlstn_set);
	  }
	}
      }
    }
  }
  else {
    sxba_copy (source2valid_item_set, t2prod_item_set [0]);
    t = 0;

    while ((t = sxba_scan (idag.source_set, t)) >= 0) {
      sxba_or (source2valid_item_set, t2prod_item_set [t]);
    }

#if has_Lex
    /* On supprime aussi les items invalides dus aux &Lex */
    sxba_and (source2valid_item_set, Lex_prod_item_set);
    equiv_prod_Lex_filter (source2valid_item_set);
#endif /* has_Lex */

    if (multiple_t_item_set) {
      /* On verifie en plus l'ordre des ancres multiples */
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      sxba_and (multiple_t_item_set, source2valid_item_set);

      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (multiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (!SXBA_bit_is_set (idag.source_set, t)) {
	  /* prod est a ancre multiples, mais toutes ses ancres ne sont pas ds le source */
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  bot = prolon [prod];
	  top = prolon [prod+1];
	  sxba_clear_range (source2valid_item_set, bot, top-bot);

	  /* On "saute" prod */
	  item = top-1;
	}
	else {
	  /* On verifie l'ordre */
	  if (prev_prod != prod) {
	    /* 1ere ancre de prod */
	    flip_flop = 0;
	    left_mlstn_set = t2hd_mlstn_set [t]; /* ancres gauches */
	    prev_prod = prod;
	    next_mlstn_set = wset [flip_flop++ % 2];
	    sxba_empty (next_mlstn_set);
	  }
	  else {
	    /* ancres suivantes */
	    right_mlstn_set = t2hd_mlstn_set [t];

	    is_empty = true;
	    i = -1;

	    while ((i = sxba_scan (left_mlstn_set, i)) >= 0) {
	      if (sxba_or_and (next_mlstn_set, mlstn_path [i], right_mlstn_set))
		is_empty = false;
	    }

	    if (is_empty) {
	      /* echec */
	      bot = prolon [prod];
	      top = prolon [prod+1];
	      sxba_clear_range (source2valid_item_set, bot, top-bot);
	      /* On "saute" prod */
	      item = top-1;
	    }
	    else {
	      left_mlstn_set = next_mlstn_set;
	      next_mlstn_set = wset [flip_flop++ % 2];
	      sxba_empty (next_mlstn_set);
	    }
	  }
	}
      }
    }
  }

  if (multiple_t_item_set) {
    sxbm_free (t2hd_mlstn_set);
    sxbm_free (wset);
  }
}
#endif /* is_lex */

#if is_lex2
#if 0

/* INUTILISEE POUR LE MOMENT */
/* Cette procedure remplit ub2item_set en n'utilisant que la lexicalisation */
/* ub2item_set [i] = ensemble des items des prod qui peuvent se terminer au milestone i */
/* Elle prend en compte l'ordre des ancres multiples */

static void ub_lexicalizer (SXBA *ub2item_set)
{  
  SXINT i, j, item, t, prod, prev_prod, bot, top, Y, Z;
  SXBA imultiple_t_item_set, item_set;
  SXBA_CST_ALLOC (working_set, itemmax+1); 
  SXINT mlstn, dag_state, trans, flip_flop;
  SXBA *t2hd_mlstn_set, path, left_mlstn_set, next_mlstn_set, right_mlstn_set, *wset, glbl_source_i, *source_t_sets;
  SXBA source_t_set, source_t_set_Z;
  bool is_empty;

  if (multiple_t_item_set) {
    source_t_sets = sxbm_calloc (idag.final_state+1, eof+1);
    imultiple_t_item_set = &(working_set [0]);
    t2hd_mlstn_set = sxbm_calloc (eof+1, idag.final_state+1);
    wset = sxbm_calloc (2, idag.final_state+1);

    if (mlstn_path == NULL) {
      sxprepare_for_possible_reset (mlstn_path);
      mlstn_path = sxbm_calloc (idag.final_state+1, idag.final_state+1);

      /* on calcule  mlstn_path [i] qui est l'ensemble des milestones j 
	 suivant strictement i (i < j) et connectes a i (i ->x j, x \in T^+) */
      for (mlstn = idag.final_state; mlstn >= idag.init_state; mlstn--) {
	dag_state = mlstn2dag_state [mlstn];
	path = mlstn_path [mlstn];

	XxYxZ_Xforeach (dag_hd, dag_state, trans) {
	  Z = dag_state2mlstn [XxYxZ_Z (dag_hd, trans)];
	  SXBA_1_bit (path, Z);
	  /* On ferme au fur et a mesure (c'est un DAG et ses noeuds sont ordonnes) */
	  sxba_or (path, mlstn_path [Z]);
	}
      }
    }
  }

  /* On remplit l'indice idag.final_state avec le calcul de base
  basic_lexicalizer (ub2item_set [idag.final_state]);
  ... inutile */

  /* L'indice idag.init_state est remplit avec les prod non lexicalisees... */
  /* On ne peut pas utiliser les prod vides, car, vu la facon dont on
     procede, les non lexicalisees non vides ne seraient jamais trouvees!! */
  sxba_copy (ub2item_set [idag.init_state], t2prod_item_set [0]);
  /* ... filtrees par le resultat de basic_lexicalizer
  sxba_and (ub2item_set [idag.init_state], ub2item_set [idag.final_state]);
  ... inutile */

  for (mlstn = idag.init_state; mlstn <= idag.final_state; mlstn++) {
    dag_state = mlstn2dag_state [mlstn];

    if (multiple_t_item_set)
      source_t_set = source_t_sets [mlstn];

    /* On calcule les transitions depuis mlstn */
    XxYxZ_Xforeach (dag_hd, dag_state, trans) {
      Y = XxYxZ_Y (dag_hd, trans);
      Z = dag_state2mlstn [XxYxZ_Z (dag_hd, trans)];

      if (multiple_t_item_set) {
	source_t_set_Z = source_t_sets [Z];
	sxba_or (source_t_set_Z, source_t_set);
      }

#if is_sdag
      glbl_source_i = glbl_source [Y];

      if (multiple_t_item_set) {
	sxba_or (source_t_set_Z, glbl_source_i);
      }

      t = 0;

      while ((t = sxba_scan (glbl_source_i, t)) > 0) {
	sxba_or (ub2item_set [Z], t2prod_item_set [t]);

	if (multiple_t_item_set) {
	  SXBA_1_bit (source_t_set_Z, t);
	  SXBA_1_bit (t2hd_mlstn_set [t], Z);
	}
      }
#else /* !is_sdag */
      t = toks_buf [Y].lahead;
      sxba_or (ub2item_set [Z], t2prod_item_set [t]);

      if (multiple_t_item_set) {
	SXBA_1_bit (source_t_set_Z, t);
	SXBA_1_bit (t2hd_mlstn_set [t], Z);
      }
#endif /* !is_sdag */
    }
    
    item_set = ub2item_set [mlstn];
    /* ... filtrees par le resultat de basic_lexicalizer
    sxba_and (item_set, ub2item_set [idag.final_state]);
    ... inutile */ 

    /* Une prod lexicalisee contenant t1, t2, ..., tp est reconnue en mlstn, seulement si l'on a
       init_mlst ->x1t1 m1 ->x2t2 m2 ... ->xptp mp ->x mlstn
       les xi \in T^*
     */ 

    if (multiple_t_item_set) {
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      source_t_set = source_t_sets [mlstn];

      sxba_copy (imultiple_t_item_set, multiple_t_item_set);
      sxba_and (imultiple_t_item_set, item_set);
      /* On filtre en plus en tenant compte des terminaux multiples ds les prod */
      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (imultiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (prev_prod != prod && prev_prod != 0) {
	  /* Il faut verifier que l'un des milestones de left_mlstn_set est mlstn */
	  if (!SXBA_bit_is_set (left_mlstn_set, mlstn)) {
	    bot = prolon [prev_prod];
	    top = prolon [prev_prod+1];
	    sxba_clear_range (item_set, bot, top-bot);
	  }
	}

	if (!SXBA_bit_is_set (source_t_set, t)) {
	  /* prod est a ancres multiples, mais toutes ses ancres ne sont pas ds l'ensemble prefixe */
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  bot = prolon [prod];
	  top = prolon [prod+1];
	  sxba_clear_range (item_set, bot, top-bot);

	  /* On "saute" prod */
	  item = top-1;
	  prev_prod = 0;
	}
	else {
	  /* On verifie l'ordre */
	  /* Attention, ici les ancres peuvent provenir de chemins differents */
	  if (prev_prod != prod) {
	    /* 1ere ancre de prod */
	    flip_flop = 0;
	    left_mlstn_set = t2hd_mlstn_set [t]; /* ancres gauches */
	    /* Attention, les milestones de left_mlstn_set peuvent ne pas e^tre connectees a mlstn */
	    prev_prod = prod;
	    next_mlstn_set = wset [flip_flop++ % 2];
	    sxba_empty (next_mlstn_set);
	  }
	  else {
	    /* ancres suivantes */
	    right_mlstn_set = t2hd_mlstn_set [t];

	    is_empty = true;
	    i = -1;

	    while ((i = sxba_scan (left_mlstn_set, i)) >= 0) {
	      if (sxba_or_and (next_mlstn_set, mlstn_path [i], right_mlstn_set))
		is_empty = false;
	    }

	    if (is_empty) {
	      /* echec */
	      bot = prolon [prod];
	      top = prolon [prod+1];
	      sxba_clear_range (source2valid_item_set, bot, top-bot);
	      /* On "saute" prod */
	      item = top-1;
	      prev_prod = 0;
	    }
	    else {
	      /* next_mlstn_set contient les milestones mi t.q.
		 ->x1t1 m1 ->x2t2 m2 ... ->xiti mi
	      */
	      left_mlstn_set = next_mlstn_set;
	      next_mlstn_set = wset [flip_flop++ % 2];
	      sxba_empty (next_mlstn_set);
	    }
	  }
	}
      }
    }
  }

  /* On propage les item_sets le long du DAG... */
  /* ...Si une prod est reconnue en mlstn, elle est aussi reconnue sur tous les milestones accessibles depuis mlstn. */
  for (mlstn = idag.init_state; mlstn <= idag.final_state; mlstn++) {
    item_set = ub2item_set [mlstn];
    dag_state = mlstn2dag_state [mlstn];

    /* On calcule les transitions depuis mlstn */
    XxYxZ_Xforeach (dag_hd, dag_state, trans) {
      sxba_or (ub2item_set [dag_state2mlstn [XxYxZ_Z (dag_hd, trans)]], item_set);
    }
  }
  
  /* Normalement, ici, on a 
     ub2item_set [idag.final_state] == basic_lexicalizer ();
  */

#if is_epsilon
  /* En fait, l'indice idag.init_state est rempli avec les prod vides */
  /* C'est mieux qu'avec les non lexicalisees */
  sxba_copy (ub2item_set [idag.init_state], empty_prod_item_set);
  /* ... filtrees par ub2item_set [idag.final_state]...
  sxba_and (ub2item_set [idag.init_state], ub2item_set [idag.final_state]);
  ... inutile */
#else /* !is_epsilon */
  sxba_empty (ub2item_set [idag.init_state]);
#endif /* !is_epsilon */
  
  if (multiple_t_item_set) {
    sxbm_free (source_t_sets);
    sxbm_free (t2hd_mlstn_set);
    sxbm_free (wset);
  }

#if 0
#if has_Lex
#ifdef lrprod_list_include_file
    /* On exploite les "lrprod" */
    /* une prod gauche g ne peut se terminer en i que s'il existe une prod droite d associee qui se termine en j, j >= i
       Si l est la taille minimale des phrases de d on a j == i+l.  Ici on prend l==0 */
    for (i = idag.init_state; i <= idag.final_state; i++) {
      lrprod_filtering (ub2item_set [i], IS_LEFT_PROD, ub2item_set [i], IS_RIGHT_PROD);
    }
#endif /* lrprod_list_include_file */
#endif /* has_Lex */
#endif /* 0 */
}
#endif /* 0 */

/* Cette procedure remplit lb2item_set en n'utilisant que la lexicalisation */
/* lb2item_set [i] = ensemble des items des prod qui peuvent commencer a l'indice i */
/* Elle prend en compte l'ordre des ancres multiples */
/* Celle-la` est un peu + rapide */

static void lb_lexicalizer (SXBA *lb2item_set)
{  
  SXINT i, j, item, t, prod, prev_prod, bot, top, X, Y, Z;
  SXBA glbl_source_i, imultiple_t_item_set, item_set;
  SXBA_CST_ALLOC (working_set, itemmax+1);
  SXINT mlstn, dag_state, trans, flip_flop;
  SXBA *t2hd_mlstn_set, path, left_mlstn_set, next_mlstn_set, right_mlstn_set, *wset, *source_t_sets;
  SXBA source_t_set, source_t_set_X;
  bool is_empty;

  if (multiple_t_item_set) {
    source_t_sets = sxbm_calloc (idag.final_state+1, eof+1);
    imultiple_t_item_set = &(working_set [0]);
    t2hd_mlstn_set = sxbm_calloc (eof+1, idag.final_state+1);
    wset = sxbm_calloc (2, idag.final_state+1);

    if (mlstn_path == NULL) {
      sxprepare_for_possible_reset (mlstn_path);
      mlstn_path = sxbm_calloc (idag.final_state+1, idag.final_state+1);

      /* on calcule  mlstn_path [i] qui est l'ensemble des milestones j 
	 suivant strictement i (i < j) et connectes a i (i ->x j, x \in T^+) */
      for (mlstn = idag.final_state; mlstn >= idag.init_state; mlstn--) {
	dag_state = mlstn2dag_state [mlstn];
	path = mlstn_path [mlstn];

	XxYxZ_Xforeach (dag_hd, dag_state, trans) {
	  Z = dag_state2mlstn [XxYxZ_Z (dag_hd, trans)];
	  SXBA_1_bit (path, Z);
	  /* On ferme au fur et a mesure (c'est un DAG et ses noeuds sont ordonnes) */
	  sxba_or (path, mlstn_path [Z]);
	}
      }
    }
  }

  /* On remplit l'indice idag.init_state avec le calcul de base
  basic_lexicalizer (lb2item_set [idag.init_state]);
  ... inutile */

  /* L'indice idag.final_state est remplit avec les prod non lexicalisees... */
  /* On ne peut pas utiliser les prod vides, car, vu la facon dont on
     procede, les non lexicalisees non vides ne seraient jamais trouvees!! */
  sxba_copy (lb2item_set [idag.final_state], t2prod_item_set [0]);
  /* ... filtrees par le resultat de basic_lexicalizer
  sxba_and (lb2item_set [idag.final_state], lb2item_set [idag.init_state]);
  ... inutile */

  for (mlstn = idag.final_state; mlstn >= idag.init_state; mlstn--) {
    dag_state = mlstn2dag_state [mlstn];

    if (multiple_t_item_set)
      source_t_set = source_t_sets [mlstn];

    XxYxZ_Zforeach (dag_hd, dag_state, trans) {
      Y = XxYxZ_Y (dag_hd, trans);
      X = dag_state2mlstn [XxYxZ_X (dag_hd, trans)];

      if (multiple_t_item_set) {
	source_t_set_X = source_t_sets [X];
	sxba_or (source_t_set_X, source_t_set);
      }

#if is_sdag
      glbl_source_i = glbl_source [Y];

      if (multiple_t_item_set) {
	sxba_or (source_t_set_X, glbl_source_i);
      }

      t = 0;

      while ((t = sxba_scan (glbl_source_i, t)) > 0) {
	sxba_or (lb2item_set [X], t2prod_item_set [t]);

	if (multiple_t_item_set) {
	  SXBA_1_bit (source_t_set_X, t);
	  SXBA_1_bit (t2hd_mlstn_set [t], X);
	}
      }
#else /* !is_sdag */
      t = toks_buf [Y].lahead;
      sxba_or (lb2item_set [X], t2prod_item_set [t]);

      if (multiple_t_item_set) {
	SXBA_1_bit (source_t_set_X, t);
	SXBA_1_bit (t2hd_mlstn_set [t], X);
      }
#endif /* !is_sdag */
    }
    
    item_set = lb2item_set [mlstn];
    /* ... filtrees par le resultat de basic_lexicalizer
    sxba_and (item_set, lb2item_set [idag.init_state]);
    ... inutile */ 

    /* Une prod lexicalisee contenant t1, t2, ..., tp est reconnue en mlstn, seulement si l'on a
       mlstn ->x1t1 m1 ->x2t2 m2 ... ->xptp mp ->x idag.final_state
       les xi \in T^*
     */ 

    if (multiple_t_item_set) {
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      source_t_set = source_t_sets [mlstn];

      sxba_copy (imultiple_t_item_set, multiple_t_item_set);
      sxba_and (imultiple_t_item_set, item_set);
      /* On filtre en plus en tenant compte des terminaux multiples ds les prod */
      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (imultiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (prev_prod != prod && prev_prod != 0) {
	  /* Il faut verifier que l'un des milestones de left_mlstn_set est mlstn */
	  if (!SXBA_bit_is_set (left_mlstn_set, mlstn)) {
	    bot = prolon [prev_prod];
	    top = prolon [prev_prod+1];
	    sxba_clear_range (item_set, bot, top-bot);
	  }
	}

	if (!SXBA_bit_is_set (source_t_set, t)) {
	  /* prod est a ancre multiples, mais toutes ses ancres ne sont pas ds l'ensemble suffixe */
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  bot = prolon [prod];
	  top = prolon [prod+1];
	  sxba_clear_range (item_set, bot, top-bot);

	  /* On "saute" prod */
	  item = top-1;
	  prev_prod = 0;
	}
	else {
	  /* On verifie l'ordre */
	  if (prev_prod != prod) {
	    /* 1ere ancre de prod */
	    flip_flop = 0;
	    left_mlstn_set = t2hd_mlstn_set [t]; /* ancres gauches */
	    prev_prod = prod;
	    next_mlstn_set = wset [flip_flop++ % 2];
	    sxba_empty (next_mlstn_set);
	  }
	  else {
	    /* ancres suivantes */
	    right_mlstn_set = t2hd_mlstn_set [t];

	    is_empty = true;
	    i = -1;

	    while ((i = sxba_scan (left_mlstn_set, i)) >= 0) {
	      if (sxba_or_and (next_mlstn_set, mlstn_path [i], right_mlstn_set))
		is_empty = false;
	    }

	    if (is_empty) {
	      /* echec */
	      bot = prolon [prod];
	      top = prolon [prod+1];
	      sxba_clear_range (item_set, bot, top-bot);
	      /* On "saute" prod */
	      item = top-1;
	      prev_prod = 0;
	    }
	    else {
	      left_mlstn_set = next_mlstn_set;
	      next_mlstn_set = wset [flip_flop++ % 2];
	      sxba_empty (next_mlstn_set);
	    }
	  }
	}
      }
    }
  }

  /* On propage les item_sets le long du DAG en backward... */
  /* ...Si une prod est reconnue en mlstn, elle est aussi reconnue sur tous les milestones conduisant a mlstn. */
  for (mlstn = idag.final_state; mlstn >= idag.init_state; mlstn--) {
    item_set = lb2item_set [mlstn];
    dag_state = mlstn2dag_state [mlstn];

    /* On calcule les transitions conduisant a mlstn */
    XxYxZ_Zforeach (dag_hd, dag_state, trans) {
      sxba_or (lb2item_set [dag_state2mlstn [XxYxZ_X (dag_hd, trans)]], item_set);
    }
  }
  
  /* Normalement, ici, on a 
     lb2item_set [idag.init_state] == basic_lexicalizer ();
  */

#if is_epsilon
  /* En fait, l'indice idag.final_state est rempli avec les prod vides */
  /* C'est mieux qu'avec les non lexicalisees */
  sxba_copy (lb2item_set [idag.final_state], empty_prod_item_set);
  /* ... filtrees par lb2item_set [idag.init_state]...
  sxba_and (lb2item_set [idag.final_state], lb2item_set [idag.init_state]);
  ... inutile */
#else /* !is_epsilon */
  sxba_empty (lb2item_set [idag.final_state]);
#endif /* !is_epsilon */

  if (multiple_t_item_set) {
    sxbm_free (source_t_sets);
    sxbm_free (t2hd_mlstn_set);
    sxbm_free (wset);
  }

#if 0
#if has_Lex
#ifdef lrprod_list_include_file
    /* On exploite les "lrprod" */
    /* une prod gauche g ne peut commencer en i que s'il existe une prod droite d associee qui commence en j, j >= i
       Si l est la taille minimale des phrases de g on a j == i+l.  Ici on prend l==0 */
    for (i = idag.final_state; i >= idag.init_state; i--) {
      lrprod_filtering (lb2item_set [i], IS_LEFT_PROD, lb2item_set [i], IS_RIGHT_PROD);
    }
#endif /* lrprod_list_include_file */
#endif /* has_Lex */
#endif /* 0 */
}
#endif /* is_lex2 */

#if is_set_automaton
/* "parseur" fonde' sur un set-automaton (la structure de controle est un ensemble et pas une pile)
   C'est un automate fini
   Analyse en temps lineaire */
/* Il peut etre utilise comme shallow parseur (option -Dis_shallow=1) et/ou comme supertaggeur
   (option -Dis_supertagging=1) ou comme "lexicaliseur" (option -Dis_lex=1) */

static SXBA rl_valid_item_set_i, lr_valid_item_set_i, cur_titem_set;
static SXBA_CST_ALLOC (valid_suffix_item_set, itemmax+1);
static SXBA_CST_ALLOC (static_w_set1, itemmax+1); /* Pour rl_valid_item_set_i */
static SXBA_CST_ALLOC (static_w_set2, itemmax+1); /* Pour lr_valid_item_set_i */
static SXBA_CST_ALLOC (int_set, ntmax+1);
static SXBA_CST_ALLOC (nt_set, ntmax+1);
static SXINT nt_stack [ntmax+1];

/* On utilise la relation left_corner (et right_corner) */
/* valid_prefixes avec nt2lc_item_set [1] */
/* A chaque etape, si c'est la 1ere fois qu'on voit un A, on ajoute
   nt2lc_item_set [A] */

/* left_corner = {(A, B) | A =>* \alpha B \beta =>* B \beta} */
/* nt2lc_item_set [A] = {B -> \alpha . \beta | B left_corner A et \alpha =>* \epsilon} */

/* right_corner = {(A, B) | A =>* \alpha B \beta =>* \alpha B} */
/* nt2rc_item_set [A] = {B -> \alpha . \beta | B right_corner A et \beta =>* \epsilon} */

/* valeurs initiales ds les tables */
/* titem_set = {A -> \alpha .t \beta | A -> \alpha t \beta \in P} */
/* rc_titem_set = {A -> \alpha t . \beta | A -> \alpha t \beta \in P} */


/* NOUVELLE VERSION
   On fait un passage gauche droite qui note pour chaque position du source l'active set calcule.
   Puis, par un passage droite gauche, on ne conserve ds chaque active set que les items connectes
   a l'item final [S' -> $ S. $]
   C'est +precis que set_automaton_parser () => on conserve
   -Dis_lrsa est donc sans effet...
 */

/* Structure intermediaire qui a chaque terminal t qui intervient ds une transition du dag depuis (ou vers)
   un noeud, associe l'ensemble des triplets (lpos, tok_no, rpos).  Bien sur on a lahead(tok_no)==t */
static SXBA_CST_ALLOC (new_rl_control_set, itemmax+1);
static SXBA_CST_ALLOC (closure_set, itemmax+1);
static SXBA *t2dag_trans_set;
static SXBA next_mlstn_set, mlstn_already_processed;
static SXBA active_set, control_set, rl_active_set, rl_control_set, mlstn_active_set;
static SXBA *active_sets, *control_sets, *rl_active_sets, *rl_control_sets;
static XxY_header glbl_dag_rcvr_txtrans;

/* On vient de reconnaitre U (item = A -> \alpha U . \beta) */

static void close_reduce (SXINT item)
{
  SXINT X, prod, bot, A;

  SXBA_1_bit (closure_set, item);
  X = lispro [item];

#if is_epsilon
  while (X != 0) {
    if (X < 0) {
      return;
    }

    if (SXBA_bit_is_reset_set (int_set, X)) {
      /* C'est la 1ere fois qu'on ferme X au niveau i */
      sxba_or (closure_set, nt2lc_item_set [X]);
    }

    if (!SXBA_bit_is_set (BVIDE, X))
      return;

    item++;
    SXBA_1_bit (closure_set, item);
    X = lispro [item];
  }
#else /* !is_epsilon */
  if (X != 0) {
    if (X > 0 && SXBA_bit_is_reset_set (int_set, X)) {
      /* C'est la 1ere fois qu'on ferme X au niveau i */
      sxba_or (closure_set, nt2lc_item_set [X]);
    }

    return;
  }
#endif /* !is_epsilon */

  /* On a reconnu prod */
  /* Elle couvre une sous-chaine non vide */
  prod = prolis [item];

#if has_Lex
  if (prod2t_nb [prod] == 0
      && SXBA_bit_is_set (must_check_identical_prod_set, prod)
      /* Prod non lexicalisee (les autres ont ete verifiees) representante d'une classe d'equivalence de prod identiques
	 sur lesquelles il faut verifier les Lex */
      && !check_Lex (prod))
    /* Echec de la verification */
    return;
#endif /* has_Lex */

  A = lhs [prod];

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    /* premiere reduction vers A */
    PUSH (nt_stack, A);
  }
}

/* On vient de reconnaitre U (item = A -> \alpha . U \beta) */

static void rl_close_reduce (SXINT item)
{
  SXINT X, prod, A, bot;

  SXBA_1_bit (closure_set, item);
  bot = prolon [prod = prolis [item]];

#if is_epsilon
  while (item > bot) {
    X = lispro [item-1];

    if (X < 0) {
      return;
    }

    if (SXBA_bit_is_reset_set (int_set, X)) {
      /* C'est la 1ere fois qu'on ferme X au niveau i */
      sxba_or (closure_set, nt2rc_item_set [X]);
    }

    if (!SXBA_bit_is_set (BVIDE, X))
      return;

    item--;
    SXBA_1_bit (closure_set, item);
  }
#else /* !is_epsilon */
  if (item > bot) {
    X = lispro [item-1];

    if (X > 0 && SXBA_bit_is_reset_set (int_set, X)) {
      /* C'est la 1ere fois qu'on ferme X au niveau i */
      sxba_or (closure_set, nt2rc_item_set [X]);
    }

    return;
  }
#endif /* !is_epsilon */

  /* On a reconnu prod */
  /* Elle couvre une sous-chaine non vide ... */
  /* ...et a ete validee par la passe gauche droite */
  if (!SXBA_bit_is_set (ilex_compatible_item_set, bot))
    sxba_range (ilex_compatible_item_set, bot, prolon [prod+1]-bot);

  A = lhs [prod];

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    PUSH (nt_stack, A);
  }
}

/* remplit les control et active sets des etats du dynamic set automaton atteints depuis mlstn 
 de gauche a droite */

static bool sa_dag_scanner (SXINT mlstn)
{
  SXINT dag_state, trans, item, t, next_mlstn, prod, Y;
  bool has_forward_ttrans = false;
  SXBA glbl_source_i, trans_set;

  /* active_set = active_sets [mlstn]; */
  /* Ici cur_titem_set est vide */
  sxba_or_and (cur_titem_set, active_set, titem_set);
  /* cur_titem_set est la partie de l'active set qui ne contient que des titems de la forme
     A -> \alpha .t \beta */

  dag_state = mlstn2dag_state [mlstn];

  XxYxZ_Xforeach (dag_hd, dag_state, trans) {
    /* attention le DAG peut etre non-deterministe par rapport aux t */
    Y = XxYxZ_Y (dag_hd, trans);
#if is_sdag
    glbl_source_i = glbl_source [Y];
    t = 0;

    while ((t = sxba_scan (glbl_source_i, t)) > 0) {
      SXBA_1_bit (source_t_set, t);
      SXBA_1_bit (t2dag_trans_set [t], trans);
    }
#else /* !is_sdag */
    t = toks_buf [Y].lahead;
    SXBA_1_bit (source_t_set, t);
    SXBA_1_bit (t2dag_trans_set [t], trans);
#endif /* !is_sdag */		  
  }

  /* Extraction des titems pour la phase scanner */
  item = -1;

  while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
    t = -lispro [item];
    
    if (SXBA_bit_is_set (source_t_set, t)) {
#if has_Lex
      prod = prolis [item];

      if (SXBA_bit_is_set (must_check_identical_prod_set, prod)
	  /* Prod est le representant d'une classe d'equivalence de prod identiques sur lesquelles
	     il faut verifier les Lex */
	  && !check_Lex (prod))
	continue;
#endif /* has_Lex */
      trans_set = t2dag_trans_set [t];
      trans = -1;

      while ((trans = sxba_scan (trans_set, trans)) >= 0) {
	has_forward_ttrans = true;
	next_mlstn = dag_state2mlstn [XxYxZ_Z (dag_hd, trans)];
	SXBA_1_bit (active_sets [next_mlstn], item+1);

	if (SXBA_bit_is_reset_set (next_mlstn_set, next_mlstn))
	  sxba_or (control_sets [next_mlstn], control_set);
      }
    }
  }

  /* Clear */
  t = 0;

  while ((t = sxba_scan_reset (source_t_set, t)) > 0) {
    sxba_empty (t2dag_trans_set [t]);
  }

  if (has_forward_ttrans)
    sxba_empty (next_mlstn_set);

  return has_forward_ttrans;
}

/* remplit les control et active sets des etats du dynamic set automaton atteints depuis mlstn 
 de droite a gauche */

static bool sa_dag_rlscanner (SXINT dag_state)
{
  SXINT trans, item, t, next_dag_state, prod, Y;
  bool has_backward_ttrans = false;
  SXBA glbl_source_i, trans_set;

  /* active_set = active_sets [mlstn]; */
  /* Ici cur_titem_set est vide */
  sxba_or_and (cur_titem_set, rl_active_set, rc_titem_set);
  /* cur_titem_set est la partie de l'active set qui ne contient que des titems de la forme
     A -> \alpha t. \beta */

  /* Extractions des transitions du DAG depuis dag_state */
  XxYxZ_Zforeach (dag_hd, dag_state, trans) {
    /* attention le DAG peut etre non-deterministe par rapport aux t */
    Y = XxYxZ_Y (dag_hd, trans);
#if is_sdag
    glbl_source_i = glbl_source [Y];
    t = 0;

    while ((t = sxba_scan (glbl_source_i, t)) > 0) {
      SXBA_1_bit (source_t_set, t);
      SXBA_1_bit (t2dag_trans_set [t], trans);
    }
#else /* !is_sdag */
    t = toks_buf [Y].lahead;
    SXBA_1_bit (source_t_set, t);
    SXBA_1_bit (t2dag_trans_set [t], trans);
#endif /* !is_sdag */		  
  }

  /* Extraction des titems pour la phase scanner */
  item = -1;

  while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
    t = -lispro [item-1];
    
    if (SXBA_bit_is_set (source_t_set, t)) {
      trans_set = t2dag_trans_set [t];
      trans = -1;

      while ((trans = sxba_scan (trans_set, trans)) >= 0) {
	has_backward_ttrans = true;
	/* kernel non vide */
	next_dag_state = XxYxZ_X (dag_hd, trans);
	SXBA_1_bit (mlstn_active_set, next_dag_state);
	SXBA_1_bit (rl_active_sets [next_dag_state], item-1);

	/* Ca servira ds recognize () */
	if (mlstn2lexicalized_look_ahead_t_set)
	  SXBA_1_bit (mlstn2lexicalized_look_ahead_t_set [next_dag_state], t);

#if is_supertagging && !is_recognizer
	/* PB : que note-t'on ds le cas d'un supertaggeur ? */
	/* Pour l'instant, je choisis tok_no */
	SXBA_1_bit (i2rl_supertagger_item_set [XxYxZ_Y (dag_hd, trans)], item-1);
#endif /* is_supertagging && !is_recognizer */

	if (SXBA_bit_is_reset_set (next_mlstn_set, next_dag_state))
	  sxba_or (rl_control_sets [next_dag_state], new_rl_control_set);
      }
    }
  }

  /* Clear */
  t = 0;

  while ((t = sxba_scan_reset (source_t_set, t)) > 0) {
    sxba_empty (t2dag_trans_set [t]);
  }

  if (has_backward_ttrans)
    sxba_empty (next_mlstn_set);

  return has_backward_ttrans;
}

static bool lr_pass_succeeds;
static SXBA source2valid_item_set;

#if is_sa_rcvr

static bool sa_dag_state_rl_rcvr (SXINT mlstn)
{
  SXINT dag_state, item, t, new_dag_state, trans, rule_no, triple, prev_mlstn, prev_trans, prev_tok; 
  bool ret_val;
  SXBA rcvr_t_set;
#define last_rule_no 3
  SXINT rule_kind [last_rule_no+1] = {0, 1, 2, 3};

  if (rcvr_tok_no >= max_rcvr_tok_no)
    return false;

  rcvr_tok_no++;

  dag_state = mlstn2dag_state [mlstn];

  /* On calcule l'ensemble des terminaux compatibles avec les items de rl_active_sets [dag_state] */
  sxba_or_and (cur_titem_set, rl_active_sets [dag_state], rc_titem_set);

  rcvr_t_set = glbl_source [last_tok_no+rcvr_tok_no];

  ret_val = false;
  item = -1;

  while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
    t = -lispro [item-1];
    SXBA_1_bit (rcvr_t_set, t);
    ret_val = true;
  }

  if (!ret_val)
    return false; 

  new_dag_state = final_pos+rcvr_tok_no;

  for (rule_no = 1; rule_no <= last_rule_no; rule_no++) {
    switch (rule_kind [rule_no]) {
    case 1: /* 0 X 1 2 */
      /* Insertion */
      /* Extractions des transitions du DAG depuis dag_state */
      XxYxZ_Zforeach (dag_hd, dag_state, trans) {
	XxYxZ_set (&dag_hd, XxYxZ_X (dag_hd, trans), XxYxZ_Y (dag_hd, trans), new_dag_state, &triple);
	/*
	  On laisse les transitions, elles peuvent etre "utilisees" par du rattrapage global !!
	  XxYxZ_erase (dag_hd, trans);
	*/
      }
      
      break;

    case 2: /* 0 2 3 */
      /* Suppression */
      XxYxZ_Zforeach (dag_hd, dag_state, trans) {
	prev_mlstn = XxYxZ_X (dag_hd, trans);
	
	XxYxZ_Zforeach (dag_hd, prev_mlstn, prev_trans) {
	  /* On pourrait peut etre verifier qu'on est a intersection non vide avec rcvr_t_set */
	  prev_tok = XxYxZ_Y (dag_hd, prev_trans);

	  if (prev_tok > last_tok_no) {
	    /* On retombe sur une transition ajoutee par le traitement d'erreur... */
	    /* ...Dois-je faire un traitement particulier ? oui */
	    if (!sxba_is_subset (glbl_source [prev_tok], rcvr_t_set))
	      /* Ca ne marchera pas */
	      continue;
	  }
	  else {
#if is_sdag
	    if (!sxba_is_subset (glbl_source [prev_tok], rcvr_t_set))
	      /* Ca ne marchera pas */
	      continue;
#else /* !is_sdag */
	    t = toks_buf [Y].lahead;

	    if (!SXBA_bit_is_set (rcvr_t_set, t))
	      /* Ca ne marchera pas */
	      continue;
#endif /* !is_sdag */
	  }

	  XxYxZ_set (&dag_hd, XxYxZ_X (dag_hd, prev_trans), prev_tok, dag_state, &triple);
	}

	/* XxYxZ_erase (dag_hd, trans); */
      }

      break;

    case 3: /* 0 X 2 */
      /* Remplacement */
      XxYxZ_Zforeach (dag_hd, dag_state, trans) {
	XxYxZ_set (&dag_hd, XxYxZ_X (dag_hd, trans), last_tok_no+rcvr_tok_no, new_dag_state, &triple);
	/* XxYxZ_erase (dag_hd, trans); */
      }
      
      break;

    default:
      break;
    }
  }

  XxYxZ_set (&dag_hd, new_dag_state, last_tok_no+rcvr_tok_no, dag_state, &triple);
	
  XxYxZ_Xforeach (dag_hd, dag_state, trans) {
    sxerror (toks_buf [XxYxZ_Y (dag_hd, trans)].source_index,
	     sxplocals.sxtables->err_titles [1][0],
	     sa_rcvr_insert_t /* "%sA \"%s\" is inserted before \"%s\"." */,
	     sxplocals.sxtables->err_titles [1]+1,
	     "word",
	     sxstrget (toks_buf [XxYxZ_Y (dag_hd, trans)].string_table_entry));
    break;
  }

  if (dag_state == new_init_pos)
    /* On change la racine du dag */
    new_init_pos = new_dag_state;

  return true;
}


/* Principe de la recuperation globale */
/*
  Cas lr (le cas miroir rl est symetrique)
  on est ds un etat mlstn/dag_state 
  Il n'existe pas ds le dag de transition sur t depuis dag_state et il n'existe pas
  ds active_set d'item de la forme A -> \alpha .t \beta
  Pour tout item ds active_set de la forme [A -> \alpha . \beta t \gamma] on recherche ds le dag
  s'il existe une transition d1 ->t d2 t.q. le mlstn de d1 soit superieur a mlstn.
  Si c'est le cas, on met [A -> \alpha \beta .t \gamma] ds active_sets [d1].
  Il peut ds le dag ne pas exister de chemin entre dag_state et d1.
  On suppose donc qu'entre dag_state et d1 il y a un chemin qui s'epelle comme une \beta-phrase.
 */

static bool glbl_sa_dag_state_rl_rcvr (SXINT mlstn)
{
  SXINT dag_state, item, left_item, bot_item, Y, t, trans, next_mlstn, it, next_dag_state, txtrans, right_trans; 
  bool ret_val;
  SXBA active_set, control_set, glbl_source_i;

  dag_state = mlstn2dag_state [mlstn];

  active_set = rl_active_sets [dag_state];
  control_set = rl_control_sets [dag_state];

  if (!XxY_is_allocated (glbl_dag_rcvr_txtrans)) {
    sxprepare_for_possible_reset (glbl_dag_rcvr_txtrans.is_allocated);
    XxY_alloc (&glbl_dag_rcvr_txtrans, "glbl_dag_rcvr_txtrans", 2*sxba_cardinal (idag.source_set), 1, 1, 0, NULL, NULL);

    /* On ne fait du rattrapage global que sur les "vrais" terminaux du source... */
    for (trans = 1; trans <= top_dag_trans; trans++) {
      Y = XxYxZ_Y (dag_hd, trans);
#if is_sdag
      glbl_source_i = glbl_source [Y];
      t = 0;

      while ((t = sxba_scan (glbl_source_i, t)) > 0) {
	XxY_set (&glbl_dag_rcvr_txtrans, t, trans, &txtrans);
      }
#else /* !is_sdag */
      t = toks_buf [Y].lahead;
      XxY_set (&glbl_dag_rcvr_txtrans, t, trans, &txtrans);
#endif /* !is_sdag */
    }
  }

  /* Attention, des etats deja calcules  et touches par le traitement d'erreur doivent etre recalcules */
  ret_val = false;

  for (trans = 1; trans <= top_dag_trans; trans++) {
    next_dag_state = XxYxZ_Z (dag_hd, trans);
    next_mlstn = dag_state2mlstn [next_dag_state];
	      
    if (next_mlstn < mlstn) {
      ret_val = true;
      Y = XxYxZ_Y (dag_hd, trans);
#if is_sdag
      sxba_or (source_t_set, glbl_source [Y]);
#else /* !is_sdag */
      t = toks_buf [Y].lahead;
      SXBA_1_bit (source_t_set, t);
#endif /* !is_sdag */
    }
  }

  if (ret_val) {
    ret_val = false;
    item = -1;

    while ((item = sxba_scan (active_set, item)) >= 0) {
      bot_item = prolon [prolis [item]];

      for (left_item = item-2; left_item >= bot_item; left_item--) {
	if ((t = -lispro [left_item]) > 0 && SXBA_bit_is_set (source_t_set, t)) {
	  XxY_Xforeach (glbl_dag_rcvr_txtrans, t, txtrans) {
	    trans = XxY_Y (glbl_dag_rcvr_txtrans, txtrans);
	    next_dag_state = XxYxZ_Z (dag_hd, trans);
	    next_mlstn = dag_state2mlstn [next_dag_state];

	    if (next_mlstn < mlstn) {
	      /* Il peut ne pas y avoir de chemin ds le dag entre next_dag_state et dag_state */
	      if (SXBA_bit_is_set (mlstn_active_set, next_dag_state)
		  && SXBA_bit_is_set_reset (mlstn_already_processed, next_mlstn)) {
		/* non vide... */
		/* ... et non (deja) filtre' */
		sxba_and (rl_active_sets [next_dag_state], titem_set);
	      }

	      SXBA_1_bit (rl_active_sets [next_dag_state], left_item+1);
	      SXBA_1_bit (mlstn_active_set, next_dag_state);
	      sxba_or (rl_control_sets [next_dag_state], control_set);

	      if (!ret_val) {
		/* 1er coup */
		XxYxZ_Xforeach (dag_hd, dag_state, right_trans) {
		  sxerror (toks_buf [XxYxZ_Y (dag_hd, right_trans)].source_index,
			   sxplocals.sxtables->err_titles [2][0],
			   sa_rcvr_start_point /* "%sGlobal set automaton right to left recovery on \"%s\"." */,
			   sxplocals.sxtables->err_titles [2]+1,
			   sxstrget (toks_buf [XxYxZ_Y (dag_hd, right_trans)].string_table_entry));
		  break;
		}
	      }
    
	      sxerror (toks_buf [XxYxZ_Y (dag_hd, trans)].source_index,
		       sxplocals.sxtables->err_titles [2][0],
		       sa_rcvr_resume_point /* "%sParsing resumes on \"%s\"." */,
		       sxplocals.sxtables->err_titles [2]+1,
		       sxstrget (toks_buf [XxYxZ_Y (dag_hd, trans)].string_table_entry));

	      ret_val = true;
	    }
	  }
	}
      }
    }
  }
	  
  sxba_empty (source_t_set);

  return ret_val;
}

static SXINT sa_dag_rcvr (SXINT bot_mlstn)
{
  SXINT prev_rcvr_tok_no, dead_end, rcvr_mlstn, dag_state, mlstn; 

  /* L'erreur a ete detectee sur bot_mlstn */
  /* Les etats du sa associes a bot_mlstn sont vides, ainsi que les etats associes aux mlstn m < bot_mlstn */

  /* dead_ends contient les milestones sans predecesseurs */
  prev_rcvr_tok_no = rcvr_tok_no;

  rcvr_mlstn = dead_end = 0;

  while ((dead_end = sxba_scan (dead_ends, dead_end)) > 0) {
    if (sa_dag_state_rl_rcvr (dead_end)) {
      /* On note le plus grand milestone qui a marche' */
      rcvr_mlstn = dead_end;
    }
  }

  /* Il faut eviter de boucler sur des insertions non bornees */
  /* Le nombre d'insertion est limite par max_rcvr_tok_no, est-ce suffisant ? */

  if (rcvr_mlstn == 0) {
    /* La correction a echouee, on essaie du traitement + global */
    /* Est-ce que ca vaut le coup si on est en train de guider du Earley car il faudra recommencer ? */  
    /* Oui : si le traitement d'erreur en SA a marche', les items ajoute's servirons de guide et permettront
       de  filtrer les items ajoute's par le traitement d'erreur earley */
    /* Inversement, si le traitement d'erreur a echoue en SA, peut-il reussir en earley ? */
    /* Non si la recup d'erreur SA definit un sur-langage de la recup earley */
    /* principe : Si A -> \alpha \beta . \gamma est ds rl_active_sets [dead_end], si
       t \in Last(\alpha), si (i ->t j) \in dag et si dag_state2mlstn[j] est non encore calcule
       alors on met A -> \alpha . \beta \gamma ds rl_active_sets [dag_state2mlstn[j]] */
    while ((dead_end = sxba_scan (dead_ends, dead_end)) > 0) {
      if (glbl_sa_dag_state_rl_rcvr (dead_end)) {
	rcvr_mlstn = dead_end;
      }
    }
  }

  if (rcvr_mlstn > 0) {
    /* Le traitement d'erreur a marche... */
    /* ... On va repartir (de droite a gauche) sur le milestone rcvr_mlstn */
    /* Attention on peut avoir m1 < rcvr_mlstn < m2 et m1 dependant de m2. */
    /* cependant, comme la recup d'erreur ajoute des transitions, les ensembles d'items
       sont croissants.  On peut donc se contenter de toucher aux active sets en ne
       conservant que les transitions terminales */
    for (mlstn = bot_mlstn+1; mlstn < rcvr_mlstn; mlstn++) {
      dag_state = mlstn2dag_state [mlstn];

      if (SXBA_bit_is_set (mlstn_active_set, dag_state)
	  && SXBA_bit_is_set_reset (mlstn_already_processed, mlstn)) {
	/* non vide... */
	/* ... et non (deja) filtre' */
	/* On ne conserve que les items de la forme A -> \alpha . t \beta */
	/* pour les futurs rl_close_reduce() */
	sxba_and (rl_active_sets [dag_state], titem_set);
      }
    }
  }
  /*
    else
    La ca merde vraiment... 
  */

  /* Il faut remettre en etat les structures du dag... */
  /* ... En particulier les nouveaux milestones doivent etre tries */
  if (prev_rcvr_tok_no < rcvr_tok_no) {
    /* On a cree rcvr_tok_no-prev_rcvr_tok_no nouveaux etats au dag state 
       (final_pos+prev_rcvr_tok_no+1, ..., final_pos+rcvr_tok_no) */
    SXINT new_mlstn, pos, trans, X, Z; 
    SXBA tbp, ap, new_mlstn_active_set, new_mlstn_already_processed;
    SXINT *new_dag_state2mlstn, *new_mlstn2dag_state;
    bool done;

    tbp = sxba_calloc (final_pos+rcvr_tok_no+1);
    ap = sxba_calloc (final_pos+rcvr_tok_no+1);
    new_dag_state2mlstn = (SXINT*) sxalloc (final_pos+1+rcvr_tok_no, sizeof (SXINT));
    new_mlstn2dag_state = (SXINT*) sxalloc (final_pos+1+rcvr_tok_no, sizeof (SXINT));
    new_dag_state2mlstn [0] = new_mlstn2dag_state [0] = 0;

    /* Attention, il faut partir de la [nouvelle] racine du dag */
    SXBA_1_bit (tbp, new_init_pos);
    done = false;
    new_mlstn = 0;

    while (!done) {
      done = true;

      pos = sxba_scan (tbp, 0);

      XxYxZ_Zforeach (dag_hd, pos, trans) {
	X = XxYxZ_X (dag_hd, trans);

	if (!SXBA_bit_is_set (ap, X))
	  break;
      }

      if (trans == 0) {
	/* Tous les predecesseurs de pos sont calcules */
	new_mlstn2dag_state [++new_mlstn] = pos;
	new_dag_state2mlstn [pos] = new_mlstn;
	SXBA_0_bit (tbp, pos);
	SXBA_1_bit (ap, pos);

	XxYxZ_Xforeach (dag_hd, pos, trans) {
	  Z = XxYxZ_Z (dag_hd, trans);
	  SXBA_1_bit (tbp, Z);
	  done = false;
	}
      }
    }

    sxfree (tbp);
    sxfree (ap);

    /* On va repartir d'ici */
    rcvr_mlstn = new_dag_state2mlstn [mlstn2dag_state [rcvr_mlstn]];
    sxba_empty (dead_ends);

    new_mlstn_already_processed = sxba_calloc (idag.final_state+1+rcvr_tok_no);
    mlstn = 0;

    while ((mlstn = sxba_scan (mlstn_already_processed, mlstn)) > 0) {
      new_mlstn = new_dag_state2mlstn [mlstn2dag_state [mlstn]];
      SXBA_1_bit (new_mlstn_already_processed, new_mlstn);
    }

    sxfree (mlstn_already_processed);
    mlstn_already_processed = new_mlstn_already_processed;

    sxfree (dag_state2mlstn);
    sxfree (mlstn2dag_state);

    dag_state2mlstn = new_dag_state2mlstn;
    mlstn2dag_state = new_mlstn2dag_state;

    /* ne pas utiliser top_dag_trans car dag_hd a pu changer... */
    t2dag_trans_set = sxbm_resize (t2dag_trans_set, eof+1, eof+1, XxYxZ_top (dag_hd)+1); // INQUIETANT
  }

  /* la passe gauche droite a pu quand m marcher, tant pis... */
  lr_pass_succeeds = false;

  if (rcvr_mlstn) {
    dag_state = mlstn2dag_state [rcvr_mlstn];
    /* On fabrique le nouveau control set... */
    sxba_copy (new_rl_control_set, rl_control_sets [dag_state]);
    sxba_or (new_rl_control_set, rl_active_sets [dag_state]);
    /* ...qui est utilise ds sa_dag_rlscanner */
    sa_dag_rlscanner (dag_state);
    SXBA_1_bit (mlstn_already_processed, rcvr_mlstn);
  }

  return rcvr_mlstn;
}
#endif /* is_sa_rcvr */

static bool dynamic_set_automaton (SXBA *lb2prod_item_set)
{
  SXINT t, item, item_1, X, i, A, bot, top, prod, mlstn, pos, dag_state; 
  SXBA supertagger_item_set;
  SXBA_CST_ALLOC (w_set2, itemmax+1);
  SXBA_CST_ALLOC (w_set1, itemmax+1);
#if is_lex2
  SXBA *lb2item_set;
#endif /* is_lex2 */
#if is_lex2 || is_lex
  SXINT nt;
  SXINT store_npd [ntmax+1];
  static SXINT store_numpd [sizeof(numpd)/sizeof(*numpd)];
#else /* !(is_lex2 || is_lex) */
  SXINT *store_npd, *store_numpd;
#endif /* !(is_lex2 || is_lex) */

  cur_titem_set = &(w_set1 [0]);
  active_sets = sxbm_calloc (idag.final_state+1, itemmax+1);
  control_sets = sxbm_calloc (idag.final_state+1+max_rcvr_tok_no, itemmax+1); /* +max_rcvr_tok_no car rl_control_sets==control_sets */
  t2dag_trans_set = sxbm_calloc (eof+1, top_dag_trans+1);
  next_mlstn_set = sxba_calloc (final_pos+1+max_rcvr_tok_no);

  mlstn2lexicalized_look_ahead_t_set = sxbm_calloc (final_pos+1+max_rcvr_tok_no, eof+1);

#if is_lex2
  lb2item_set = sxbm_calloc (idag.final_state+1, itemmax+1);
  lb_lexicalizer (lb2item_set);
  source2valid_item_set = lb2item_set [idag.init_state];
#else /* !is_lex2 */
#if is_lex
  sxba_empty (source2valid_item_set = &(w_set2 [0]));
  basic_lexicalizer (source2valid_item_set);
#endif /* is_lex */
#endif /* !is_lex2 */

#if is_lex2 || is_lex
  /* On fabrique les nouveaux npd/numpd en tenant compte de source2valid_item_set */
  /* Pour remettre en l'etat... */
  for (nt = 1; nt <= ntmax; nt++) {
    store_npd [nt] = npd [nt+1]-1; /* On pointe vers le 0 de fin de liste (liste vide) */
  }

  item = -1;

  while ((item = sxba_scan (source2valid_item_set, item)) >= 0) {
    if ((X = lispro [item]) > 0) {
      store_numpd [--store_npd [X]] = item; /* On ajoute item a la liste */
    }
  }
#else /* !(is_lex2 || is_lex) */
  /* On utilise les tables statiques */
  store_npd = &(npd [0]);
  store_numpd = &(numpd [0]);
#endif /* !(is_lex2 || is_lex) */

#if 0
  /* Le 13/08/2003, je laisse les tables statiques inchangees a cause de la recup d'erreur eventuelle
     (un peu moins efficace ds le cas is_lex) */
#if is_lex2
  /* On filtre les tables statiques */
  sxba_and (nt2lc_item_set [1], source2valid_item_set);
  /* dans le cas is_lex2 le filtrage se fera dynamiquement pour chaque indice */
#else /* !is_lex2 */
#if is_lex
  for (X = 1; X <= ntmax; X++)
    sxba_and (nt2lc_item_set [X], source2valid_item_set);
#endif /* is_lex */
#endif /* !is_lex2 */
#endif /* 0 */

  /* On part des items initiaux valides par la lexicalisation et connectes a S' -> $ . S $ */
  control_set = control_sets [idag.init_state]; /* transition sur le eof initial!! */
  active_set = active_sets [idag.init_state];
  /* Simulation du close-reduce */
  sxba_copy (control_set, nt2lc_item_set [1]);

  SXBA_1_bit (control_set, 1); /* Le 23/06/2003, j'ajoute [S' -> $ . S $] */
  sxba_copy (active_set, control_set);
#if is_lex || is_lex2
  sxba_and (active_set, source2valid_item_set);
#endif /* is_lex || is_lex2 */

  /* ATTENTION : Lors de la passe gauche droite, on a mlstn==dag_state */

  if (sa_dag_scanner (idag.init_state)) {
    /* Il y a des transitions terminales depuis l'etat initial */
    /* Sinon, pas de traitement d'erreur ds la phase gauche-droite, on se contente de l'abandonner */
    for (mlstn = idag.init_state+1; mlstn <= idag.final_state; mlstn++) {
      control_set = control_sets [mlstn];
      /* active_set = {A -> \alpha t. \beta | ? ->t mlstn} */
      active_set = active_sets [mlstn];
      /* Ici closure_set est vide */

      item = -1;

      while ((item = sxba_scan (active_set, item)) > 0) {
	/* close_reduce () ne touche pas a active_set */
	close_reduce (item);
      }

      if (!IS_EMPTY (nt_stack)) {
	do {
	  A = POP (nt_stack);
	  /* On cherche toutes les occurrences de A en RHS telle que B -> \alpha . A \beta soit ds control_set */
	  bot = store_npd [A];

	  while (item = store_numpd [bot++]/* B -> \alpha . A  \beta */) {
	    if (SXBA_bit_is_set (control_set, item)) {
	      close_reduce (item+1);
	    }
	  }
	} while (!IS_EMPTY (nt_stack));

	sxba_empty (nt_set);
      }

      /* On change de niveau */
      sxba_empty (int_set);
#if is_lex2
      /* On filtre par le resultat de lb_lexicalizer */
      sxba_or_and (active_set, closure_set, lb2item_set [mlstn]);
#else /* !is_lex2 */
#if is_lex
      /* Le filtrage par source2valid_item_set a deja ete fait ds nt2lc_item_set [A] (non, depuis le 13/08/2003) */
      sxba_or_and (active_set, closure_set, source2valid_item_set);
#else /* !is_lex */
      sxba_or (active_set, closure_set);
#endif /* !is_lex */
#endif /* !is_lex2 */
      sxba_empty (closure_set);

      /* On fabrique le nouveau control set... */
      sxba_or (control_set, active_set);
    /* ...qui est utilise ds sa_dag_scanner */

      if (mlstn == idag.final_state || !sa_dag_scanner (mlstn)) {
	/* Il n'y a pas de transitions terminales depuis l'etat courant */
	/* pas de traitement d'erreur ds la phase gauche-droite, on se contente de l'abandonner */
	/* Le 03/05/04 non, il ne faut pas abandonner Ex :
	   ... (ou alors | ou_alors) ...
	   Si "ou" est une ff inconnue il faut quand meme continuer, c,a pourra marcher par "ou_alors" */ 
	/* break; */
      }
    }
  }

  lr_pass_succeeds = SXBA_bit_is_set (active_sets [idag.final_state], 2); /* [S' -> $ S. $] \in active_set */

  /* Si la passe gauche-droite a echouee et s'il n'y a pas de traitement d'erreur, c'est fini... */
  mlstn = 0; /* echec a priori */

  if (
#if is_sa_rcvr
      true ||
#endif /* is_sa_rcvr */
      lr_pass_succeeds ) {

#if is_epsilon
    /* Les lb sont des dag_states */
    sxba_or_and (lb2prod_item_set [final_pos], empty_prod_item_set, active_set);
#endif /* is_epsilon */

    /* Analyse par le mirror dynamic set automaton dont les active sets seront filtres par ceux de l'analyse gauche-droite */
    /* mlstn_already_processed et dead_ends sont les seuls ensembles indexes par des milestones, tous les autres le sont
       par des dag_state */
    mlstn_already_processed = sxba_calloc (idag.final_state+1);
    dead_ends = sxba_calloc (idag.final_state+1+max_rcvr_tok_no);

    mlstn_active_set = sxba_calloc (final_pos+1+max_rcvr_tok_no);
    rl_active_sets = sxbm_calloc (final_pos+1+max_rcvr_tok_no, itemmax+1);
    rl_active_set = rl_active_sets [final_pos];

    /* On ne conserve que les items connectes a l'item final [S' -> $ S. $] */
    if (lr_pass_succeeds) {
      sxba_or_and (rl_active_set, nt2rc_item_set [1], active_sets [final_pos]);
    }
    else {
       /* Le 13/08/2003, si la passe gauche droite a echouee, pas de filtrage ... */
      sxba_copy (rl_active_set, nt2rc_item_set [1]);
#if 0
#if is_lex2 || is_lex
      sxba_or_and (rl_active_set, nt2rc_item_set [1], source2valid_item_set);
#else /* !(is_lex2 || is_lex) */
      sxba_copy (rl_active_set, nt2rc_item_set [1]);
#endif /* !(is_lex2 || is_lex) */
#endif /* 0 */
    }
    
    SXBA_1_bit (rl_active_set, 2); /* Le 23/06/2003, j'ajoute [S' -> $ S. $] */

    /* On "recycle" les control_sets */
    rl_control_sets = control_sets;

    for (pos = 1; pos <= final_pos; pos++)
      sxba_empty (rl_control_sets [pos]);

    sxba_copy (new_rl_control_set, rl_active_set); /* pour sa_dag_rlscanner */

    if (!sa_dag_rlscanner (final_pos)) {
      SXBA_1_bit (dead_ends, idag.final_state);
    }
    else
      SXBA_1_bit (mlstn_already_processed, idag.final_state);

    mlstn = idag.final_state; /* c'est bon initialement */

    while ((mlstn = sxba_0_rlscan (mlstn_already_processed, mlstn)) >= 0) {
      /* On decroit jusqu'a mlstn==0 pour capturer les erreurs eventuelles a l'origine */
      dag_state = mlstn2dag_state [mlstn];

      if (!SXBA_bit_is_set (mlstn_active_set, dag_state)) {
	if (sxba_1_rlscan (mlstn_active_set, dag_state) == -1) {
	  if (dag_state == 0 && SXBA_bit_is_set (rl_active_set, 1)) {
	    /* rl_active_set est celui calcule' par dag_state==init_pos */
	    /* [S' -> $ .S $] est atteint => C'est OK */
	    mlstn = 1;
	    /* Analyse correcte avec rattrapage d'erreur eventuel */
	    break;
	  }

#if is_sa_rcvr
	  /* Erreur d'analyse, il faut faire du rattrapage */
	  /* Les milestones n'ayant pas de descendents sont ds dead_ends  */
	  mlstn = sa_dag_rcvr (mlstn);
	  /* si mlstn==0, le traitement d'erreur n'a rien pu faire... */
#else /* !is_sa_rcvr */
	  mlstn = 0;
#endif /* !is_sa_rcvr */
	}
	else
	  /* Le 27/08/2003 l'analyse peut se poursuivre sur des mlstn + a gauche... */
	  continue;

	if (mlstn == 0) {
	  /* Soit il n'y a pas de traitement d'erreur au niveau du dynamic set automaton soit il a
	     completement echoue.  Que fait-on pour les lb2prod_item_set ? */
	  /* conserve-t-on ceux qui ont deja ete calcules ? */
	  break;
	}
      }

      rl_control_set = rl_control_sets [dag_state];
      rl_active_set = rl_active_sets [dag_state];
      sxba_empty (closure_set);

      /* PB c'est quoi l'indice de lb2prod_item_set ??? */
      ilex_compatible_item_set = lb2prod_item_set [dag_state]; /* vide */

      /* Normalement, tous les items de rl_active_set sont de la forme
	 A -> \alpha .t \beta, avec dag_state ->t ?? */
      item = -1;

      while ((item = sxba_scan (rl_active_set, item)) >= 0) {
	/* rl_close_reduce() ne touche pas a rl_active_set */
	rl_close_reduce (item);
      }

      if (!IS_EMPTY (nt_stack)) {
	do {
	  A = POP (nt_stack);
	  /* On cherche toutes les occurrences de A en RHS telle que B -> \alpha A . \beta soit ds rl_control_set */
	  bot = store_npd [A];

	  while (item = store_numpd [bot++]/* B -> \alpha . A  \beta */) {
	    item_1 = item+1;

	    if (SXBA_bit_is_set (rl_control_set, item_1)) {
	      rl_close_reduce (item);
	    }
	  }
	} while (!IS_EMPTY (nt_stack));

	sxba_empty (nt_set);
      }

      /* On change de niveau */
      sxba_empty (int_set);
      
      if (lr_pass_succeeds)
	/* On filtre par le resultat de la passe gauche-droite */
	sxba_or_and (rl_active_set, closure_set, active_sets [dag_state]);
      else
	/* ...sinon pas de fitrage (on est en rcvr) ... */
	sxba_or (rl_active_set, closure_set);

#if is_epsilon
      /* On ajoute les productions vides des fermetures */
      sxba_or_and (ilex_compatible_item_set, rl_active_set, empty_prod_item_set);
#endif /* is_epsilon */

      /* On fabrique le nouveau control set... */
      sxba_copy (new_rl_control_set, rl_control_set);
      sxba_or (new_rl_control_set, rl_active_set);
      /* ...qui est utilise ds sa_dag_rlscanner */

      /* scan arriere */
      if (dag_state == new_init_pos || !sa_dag_rlscanner (dag_state))
	/* Pour l'error recovery */
	SXBA_1_bit (dead_ends, mlstn);
      else
	SXBA_1_bit (mlstn_already_processed, mlstn);
    }

    sxbm_free (rl_active_sets), rl_active_sets = NULL;
    sxfree (mlstn_active_set), mlstn_active_set = NULL;
    sxfree (dead_ends), dead_ends = NULL;
    sxfree (mlstn_already_processed), mlstn_already_processed = NULL;
  }

  if (mlstn == 0) {
    /* Erreur (non recuperable) */
    /* Le 13/03/2003 : changement de strategie, plus aucun filtre, on laisse earley se debrouiller ... */
    /* Le 04/06/04 : on laisse earley se debrouiller meme si is_sa_rcvr n'est pas positionne' */

    /* On "RAZ", NE PAS TOUCHER A lb2prod_item_set [0] !! */
    for (dag_state = init_pos; dag_state <= final_pos; dag_state++) {
      lb2prod_item_set [dag_state] = NULL;
    }

#if 0
#if is_lex2
    /* On prend le resultat de lb_lexicalizer */
    for (dag_state = init_pos; dag_state <= final_pos; dag_state++) {
      sxba_copy (lb2prod_item_set [dag_state], lb2item_set [dag_state2mlstn [mlstn]]);
    }
#else /* !is_lex2 */
#if is_lex
    /* On prend le resultat de basic_lexicalizer */
    SXBA item_set = lb2prod_item_set [init_pos];

    sxba_copy (item_set, source2valid_item_set);

    for (dag_state = init_pos+1; dag_state <= final_pos; dag_state++) {
      lb2prod_item_set [dag_state] = item_set;
    }
    
#else /* !is_lex */
    /* On "RAZ", NE PAS TOUCHER A lb2prod_item_set [0] !! */
    for (dag_state = init_pos; dag_state <= final_pos; dag_state++) {
      lb2prod_item_set [dag_state] = NULL;
    }
#endif /* !is_lex */
#endif /* is_lex2 */
#endif /* 0 */

    sxbm_free (mlstn2lexicalized_look_ahead_t_set), mlstn2lexicalized_look_ahead_t_set = NULL;
  }

  sxbm_free (active_sets), active_sets = NULL;
  sxbm_free (control_sets), control_sets = NULL;
  sxbm_free (t2dag_trans_set), t2dag_trans_set = NULL;
  sxfree (next_mlstn_set), next_mlstn_set = NULL;
  if (XxY_is_allocated (glbl_dag_rcvr_txtrans)) XxY_free (&glbl_dag_rcvr_txtrans);
#if is_lex2
  sxbm_free (lb2item_set);
#endif /* is_lex2 */

  return mlstn > 0;
}
#endif /* is_set_automaton */


#else /* !is_dag */
#if is_lex
/* Cette procedure remplit source2valid_item_set avec l'ensemble des items (tous les items d'une production)
   qui peuvent etre selectionnees en ne tenant compte que de la lexicalisation due globalement au source.
   (Elle ne realise donc pas un decoupage du source par index, contrairement a dynamic_lexicalizer).
   Elle tient compte des ancres multiples et de leur ordre d'apparition dans les productions multiples. */
/* Nouvelle version le 16/08/06 A TESTER
   Changement ds le traitement des ancres multiples
   Les productions selectionnes doivent etre accessibles depuis l'axiome */
#if 0

static void basic_lexicalizer (SXBA source2valid_item_set)
{
  SXINT     t, item, prev_prod, prev_t, prod, bot, top, i, j; 
  SXINT     mlstn, next_mlstn, dag_state, trans, Y, Z;
  SXBA    glbl_source_i, mlstn_glbl_source_set, next_mlstn_glbl_source_set, valid_item_set;
  SXBA    *t2right_t_set, glbl_source_set;

  if (multiple_t_item_set) {
    /* t2right_t_set [t1] = {t | existe un chemin du source de la forme "eof ... t1 ... t ... eof" */
    /* autrement dit, t peut suivre t1 */
    t2right_t_set = sxbm_calloc (eof+1, eof+1);
    glbl_source_set = sxba_calloc (eof+1);

    SXBA_1_bit (glbl_source_set, eof);

    for (mlstn = n; mlstn >= 1; mlstn--) {
#if is_sdag
      glbl_source_i = glbl_source [mlstn];

      t = -1;

      while ((t = sxba_scan (glbl_source_i, t)) >= 0) {
	sxba_or (t2right_t_set [t], glbl_source_set);
      }

      sxba_or (glbl_source_set, glbl_source_i);
#else /* !is_sdag */
      t = glbl_source [mlstn];
      sxba_or (t2right_t_set [t], glbl_source_set);
      SXBA_1_bit (glbl_source_set, t);
#endif /* !is_sdag */
    }
  
    sxfree (glbl_source_set);
  }

  valid_item_set = sxba_calloc (itemmax+1);

  /* On calcule les items valides du fait du source */
  if (sxba_cardinal (idag.source_set) > split_val) {
    t = 0;

    while ((t = sxba_0_lrscan (idag.source_set, t)) > 0) {
      sxba_or (valid_item_set, t2prod_item_set [t]);
    }

    sxba_not (valid_item_set); /* Ce qui n'est pas interdit est permis */

#if has_Lex
    /* On supprime aussi les items invalides dus aux &Lex */
    sxba_and (valid_item_set, Lex_prod_item_set);
    equiv_prod_Lex_filter (valid_item_set);
#endif /* has_Lex */

    if (multiple_t_item_set) {
      /* On verifie en plus l'ordre des ancres multiples */
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      sxba_and (multiple_t_item_set, valid_item_set);

      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (multiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (prev_prod != prod) {
	  /* 1ere ancre de prod */
	  prev_prod = prod;
	  prev_t = t;
	}
	else {
	  /* ancres suivantes */
	  if (!SXBA_bit_is_set (t2right_t_set [prev_t], t)) {
	    /* echec, t ne suit pas prev_t ds le source */
	    bot = prolon [prod];
	    SXBA_0_bit (valid_item_set, bot); /* ca suffit */
	    /* On "saute" prod */
	    item = prolon [prod+1]-1;
	  }
	  else
	    prev_t = t;
	}
      }
    }
  }
  else {
    sxba_copy (valid_item_set, t2prod_item_set [0]);
    t = 0;

    while ((t = sxba_scan (idag.source_set, t)) >= 0) {
      sxba_or (valid_item_set, t2prod_item_set [t]);
    }

#if has_Lex
    /* On supprime aussi les items invalides dus aux &Lex */
    sxba_and (valid_item_set, Lex_prod_item_set);
    equiv_prod_Lex_filter (valid_item_set);
#endif /* has_Lex */

    if (multiple_t_item_set) {
      /* On verifie en plus l'ordre des ancres multiples */
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      sxba_and (multiple_t_item_set, valid_item_set);

      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (multiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (!SXBA_bit_is_set (idag.source_set, t)) {
	  /* prod est a ancre multiples, mais toutes ses ancres ne sont pas ds le source */
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  bot = prolon [prod];
	  SXBA_0_bit (valid_item_set, bot); /* ca suffit */
	  /* On "saute" prod */
	  item = prolon [prod+1]-1;
	}
	else {
	  /* On verifie l'ordre */
	  if (prev_prod != prod) {
	    /* 1ere ancre de prod */
	    prev_prod = prod;
	    prev_t = t;
	  }
	  else {
	    /* ancres suivantes */
	    if (!SXBA_bit_is_set (t2right_t_set [prev_t], t)) {
	      /* echec, t ne suit pas prev_t ds le source */
	      bot = prolon [prod];
	      SXBA_0_bit (valid_item_set, bot); /* ca suffit */
	      /* On "saute" prod */
	      item = prolon [prod+1]-1;
	    }
	    else
	      prev_t = t;
	  }
	}
      }
    }
  }

  if (multiple_t_item_set) {
    sxbm_free (t2right_t_set);
  }

  /* On ne conserve que les prod accessibles depuis l'axiome */
  nt_set = sxba_calloc (ntmax + 1);
  nt_stack = (SXINT*) sxalloc (ntmax + 1, sizeof (SXINT)), nt_stack [0] = 0;

  SXBA_1_bit (nt_set, 1);
  PUSH (nt_stack, 1);

  while (!IS_EMPTY (nt_stack)) {
    A = POP (nt_stack);

    bot = npg [A];
    top = npg [A+1];

    while (bot < top) {
      prod = numpg [bot];
      item = prolon [prod];

      if (SXBA_bit_is_set (valid_item_set, item)) {
	SXBA_1_bit (source2valid_item_set, item);

	while ((X = lispro [item]) != 0) {
	  if (X > 0 && SXBA_bit_is_reset_set (nt_set, X))
	    PUSH (nt_stack, X);

	  item++;
	  SXBA_1_bit (source2valid_item_set, item);
	}
      }

      bot++;
    }
  }

  sxfree (nt_set);
  sxfree (nt_stack);
  sxfree (valid_item_set);
}
#endif /* 0 */

static void basic_lexicalizer (SXBA source2valid_item_set)
{
  SXINT t, item, prev_prod, prod, bot, top, i, j;
  SXBA *t2i_set, glbl_source_i;


  if (multiple_t_item_set) {
    t2i_set = sxbm_calloc (eof+1, n+1);

    /* on calcule t2i_set avec glbl_source */
    /* Ca sert a verifier l'ordre des ancres multiples */

    for (i = 1; i <= n; i++) {
#if is_sdag
      glbl_source_i = glbl_source [i];

      t = 0;

      while ((t = sxba_scan (glbl_source_i, t)) > 0) {
	SXBA_1_bit (t2i_set [t], i);
      }
#else /* !is_sdag */
      SXBA_1_bit (t2i_set [glbl_source [i]], i);
#endif /* !is_sdag */
    }
  }

  /* On calcule les items valides du fait du source */
  if (sxba_cardinal (idag.source_set) > split_val) {
    t = 0;

    while ((t = sxba_0_lrscan (idag.source_set, t)) > 0) {
      sxba_or (source2valid_item_set, t2prod_item_set [t]);
    }

    sxba_not (source2valid_item_set); /* Ce qui n'est pas interdit est permis */

#if has_Lex
    /* On supprime aussi les items invalides dus aux &Lex */
    sxba_and (source2valid_item_set, Lex_prod_item_set);
    equiv_prod_Lex_filter (source2valid_item_set);
#endif /* has_Lex */

    if (multiple_t_item_set) {
      /* On verifie en plus l'ordre des ancres multiples */
      /* Le test de l'ordre conduit a un set_automaton un tout petit peu + rapide */
      sxba_and (multiple_t_item_set, source2valid_item_set);

      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (multiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (prev_prod != prod) {
	  /* 1ere ancre de prod */
	  prev_prod = prod;
	  i = -1;
	}

	if ((i = sxba_scan (t2i_set [t], i)) < 0) {
	  /* echec */
	  bot = prolon [prod];
	  top = prolon [prod+1];
	  sxba_clear_range (source2valid_item_set, bot, top-bot);
	  /* On "saute" prod */
	  item = top-1;
	}
      }
    }
  }
  else {
    sxba_copy (source2valid_item_set, t2prod_item_set [0]);
    t = 0;

    while ((t = sxba_scan (idag.source_set, t)) >= 0) {
      sxba_or (source2valid_item_set, t2prod_item_set [t]);
    }

#if has_Lex
    sxba_and (source2valid_item_set, Lex_prod_item_set);
    equiv_prod_Lex_filter (source2valid_item_set);
#endif /* has_Lex */

    if (multiple_t_item_set) {
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      sxba_and (multiple_t_item_set, source2valid_item_set);
      /* On filtre en plus en tenant compte des terminaux multiples ds les prod */

      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (multiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (!SXBA_bit_is_set (idag.source_set, t)) {
	  /* prod est a ancre multiples, mais toutes ses ancres ne sont pas ds le source */
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  bot = prolon [prod];
	  top = prolon [prod+1];
	  sxba_clear_range (source2valid_item_set, bot, top-bot);

	  /* On "saute" prod */
	  item = top-1;
	}
	else {
	  /* On verifie l'ordre */
	  if (prev_prod != prod) {
	    /* 1ere ancre de prod */
	    prev_prod = prod;
	    j = -1;
	  }

	  if ((j = sxba_scan (t2i_set [t], j)) < 0) {
	    /* echec */
	    bot = prolon [prod];
	    top = prolon [prod+1];
	    sxba_clear_range (source2valid_item_set, bot, top-bot);
	    /* On "saute" prod */
	    item = top-1;
	  }
	}
      }
    }
  }

  if (multiple_t_item_set)
    sxbm_free (t2i_set);

#if 0
  /* TEST : avec xtag3 sur les 42253 phrases de wsj-1MW.sdag, 7495 seulement filtrent (par lrprod)
     7495 productions.  Donc 1 phrase sur 6 "beneficie" de ce filtre et c,a seulement sur une seule
     production qui est eliminee!!  Ce test vaut-il le coup ds basic_lexicalizer ?
     Reponse non, je l'enleve pour le moment
   */
#if has_Lex
#ifdef lrprod_list_include_file
  lrprod_filtering (source2valid_item_set, IS_LEFT_PROD+IS_RIGHT_PROD, source2valid_item_set, IS_LEFT_PROD+IS_RIGHT_PROD);
#endif /* lrprod_list_include_file */
#endif /* has_Lex */
#endif /* 0 */
}

#if is_lex2
/* Cette procedure remplit ub2item_set en n'utilisant que la lexicalisation */
/* ub2item_set [i] = ensemble des items des prod qui peuvent se terminer a l'indice i */
/* Elle prend en compte l'ordre des ancres multiples */

static void ub_lexicalizer (SXBA *ub2item_set)
{  
  SXINT i, j, item, t, prod, prev_prod, bot, top;
  SXBA glbl_source_i, imultiple_t_item_set, item_set, *t2i_set;
  SXBA_CST_ALLOC (source_t_set, eof+1);
  SXBA_CST_ALLOC (working_set, itemmax+1);

  sxba_empty (source_t_set);

  if (multiple_t_item_set) {
    imultiple_t_item_set = &(working_set [0]);
    t2i_set = sxbm_calloc (eof+1, n+1);
  }

  /* On remplit l'indice n avec le calcul de base */
  basic_lexicalizer (ub2item_set [n]);

  /* L'indice 0 est remplit avec les prod non lexicalisees... */
  /* On ne peut pas utiliser les prod vides, car, vu la facon dont on
     procede, les non lexicalisees non vides ne seraient jamais trouvees!! */
  sxba_copy (ub2item_set [0], t2prod_item_set [0]);
  /* ... filtrees par le resultat de basic_lexicalizer */
  sxba_and (ub2item_set [0], ub2item_set [n]);
#if 0 && has_Lex
  /* ...dont les &Lex sont valides */
  sxba_and (ub2item_set [0], Lex_prod_item_set);
#endif /* 0 && has_Lex */

  /* Le 19/05/2003 changement de "i <= n" en "i < n" */
  for (i = 1; i < n; i++) {
    item_set = ub2item_set [i]; /* vide */
#if is_sdag
    glbl_source_i = glbl_source [i];
    t = 0;

    while ((t = sxba_scan (glbl_source_i, t)) > 0) {
#if 0
      /* ATTENTION : on ne peut pas tenir compte de l'ordre des ancres. Si une prod contient
	 t et t' ds cet ordre et que t soit trouve' en k et t' en j, j < k, prod, a l'indice
	 j ne devrait pas etre selectionne.  Cependant si t est retrouve plus tard
	 a l'indice i, i < j, prod doit etre alors selectionne, il ne le sera cependant pas
	 car t n'est pas nouveau... */
      if (SXBA_bit_is_reset_set (source_t_set, t))
	/* Nouveau t */
	sxba_or (item_set, t2prod_item_set [t]);
#endif /* 0 */

      SXBA_1_bit (source_t_set, t);
      sxba_or (item_set, t2prod_item_set [t]); /* meme si t est ds le suffixe (pour tester l'ordre) */

      if (multiple_t_item_set)
	SXBA_1_bit (t2i_set [t], i);
    }
#else /* !is_sdag */
    t = glbl_source [i];
    SXBA_1_bit (source_t_set, t);
    sxba_copy (item_set, t2prod_item_set [t]);

    if (multiple_t_item_set)
      SXBA_1_bit (t2i_set [t], i);
#endif /* !is_sdag */

    /* ... filtrees par le resultat de basic_lexicalizer */
    sxba_and (item_set, ub2item_set [n]);

#if 0 && has_Lex
    /* On assure la compatibilite des locaux avec les Lex */
    sxba_and (item_set, Lex_prod_item_set);
#endif /* 0 && has_Lex */

    if (multiple_t_item_set) {
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      sxba_copy (imultiple_t_item_set, multiple_t_item_set);
      sxba_and (imultiple_t_item_set, item_set);
      /* On filtre en plus en tenant compte des terminaux multiples ds les prod */
      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (imultiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (!SXBA_bit_is_set (source_t_set, t)) {
	  /* prod est a ancre multiples, mais toutes ses ancres ne sont pas ds le suffixe */
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  bot = prolon [prod];
	  top = prolon [prod+1];
	  sxba_clear_range (item_set, bot, top-bot);

	  /* On "saute" prod */
	  item = top-1;
	}
	else {
	  /* On verifie l'ordre */
	  if (prev_prod != prod) {
	    /* 1ere ancre de prod */
	    prev_prod = prod;
	    j = -1;
	  }

	  if ((j = sxba_scan (t2i_set [t], j)) < 0) {
	    /* echec */
	    bot = prolon [prod];
	    top = prolon [prod+1];
	    sxba_clear_range (item_set, bot, top-bot);
	    /* On "saute" prod */
	    item = top-1;
	  }
	}
      }
    }

    /* On fusionne avec les indices inferieurs */
    sxba_or (item_set, ub2item_set [i-1]);
  }

#if is_epsilon
  /* En fait, l'indice 0 est rempli avec les prod vides */
  /* C'est mieux qu'avec les non lexicalisees */
  sxba_copy (ub2item_set [0], empty_prod_item_set);
  /* ... filtrees par le resultat de basic_lexicalizer */
  sxba_and (ub2item_set [0], ub2item_set [n]);
#if 0 && has_Lex
  /* On supprime aussi les items invalides dus aux &Lex */
  sxba_and (ub2item_set [0], Lex_prod_item_set);
#endif /* 0 && has_Lex */
#endif /* is_epsilon */

  if (multiple_t_item_set)
    sxbm_free (t2i_set);

#if 0
#if has_Lex
#ifdef lrprod_list_include_file
    /* On exploite les "lrprod" */
    /* une prod gauche g ne peut se terminer en i que s'il existe une prod droite d associee qui se termine en j, j >= i
       Si l est la taille minimale des phrases de d on a j == i+l.  Ici on prend l==0 */
    for (i = 0; i <= n; i++) {
      lrprod_filtering (ub2item_set [i], IS_LEFT_PROD, ub2item_set [i], IS_RIGHT_PROD);
    }
#endif /* lrprod_list_include_file */
#endif /* has_Lex */
#endif /* 0 */
}

/* Cette procedure remplit lb2item_set en n'utilisant que la lexicalisation */
/* lb2item_set [i] = ensemble des items des prod qui peuvent commencer a l'indice i */
/* Elle prend en compte l'ordre des ancres multiples */
/* Celle-la` est un peu + rapide */

static void lb_lexicalizer (SXBA *lb2item_set)
{  
  SXINT i, j, item, t, prod, prev_prod, bot, top;
  SXBA glbl_source_i, imultiple_t_item_set, item_set, *t2i_set;
  SXBA_CST_ALLOC (source_t_set, eof+1);
  SXBA_CST_ALLOC (working_set, itemmax+1);

  sxba_empty (source_t_set);

  if (multiple_t_item_set) {
    imultiple_t_item_set = &(working_set [0]);
    t2i_set = sxbm_calloc (eof+1, n+1);
  }

  /* On remplit l'indice 0 avec le calcul de base */
  basic_lexicalizer (lb2item_set [0]);

  /* L'indice n est remplit avec les prod non lexicalisees... */
  /* On ne peut pas utiliser les prod vides, car, vu la facon dont on
     procede, les non lexicalisees non vides ne seraient jamais trouvees!! */
  sxba_copy (lb2item_set [n], t2prod_item_set [0]);
  /* ... filtrees par le resultat de basic_lexicalizer */
  sxba_and (lb2item_set [n], lb2item_set [0]);
#if 0 && has_Lex
  /* ...dont les &Lex sont valides */
  sxba_and (lb2item_set [n], Lex_prod_item_set);
#endif /* 0 && has_Lex */

  /* Le 19/05/2003 changement de "i > 0" en "i > 1" */
  /* lb2item_set [0] est initialise' par basic */
  for (i = n; i > 1; i--) {
    item_set = lb2item_set [i-1]; /* vide */
#if is_sdag
    glbl_source_i = glbl_source [i];
    t = 0;

    while ((t = sxba_scan (glbl_source_i, t)) > 0) {
#if 0
      /* ATTENTION : on ne peut pas tenir compte de l'ordre des ancres. Si une prod contient
	 t et t' ds cet ordre et que t soit trouve' en k et t' en j, j < k, prod, a l'indice
	 j ne devrait pas etre selectionne.  Cependant si t est retrouve plus tard
	 a l'indice i, i < j, prod doit etre alors selectionne, il ne le sera cependant pas
	 car t n'est pas nouveau... */
      if (SXBA_bit_is_reset_set (source_t_set, t))
	/* Nouveau t */
	sxba_or (item_set, t2prod_item_set [t]);
#endif /* 0 */

      SXBA_1_bit (source_t_set, t);
      sxba_or (item_set, t2prod_item_set [t]); /* meme si t est ds le suffixe (pour tester l'ordre) */

      if (multiple_t_item_set)
	SXBA_1_bit (t2i_set [t], i);
    }
#else /* !is_sdag */
    t = glbl_source [i];
    SXBA_1_bit (source_t_set, t);
    sxba_copy (item_set, t2prod_item_set [t]);

    if (multiple_t_item_set)
      SXBA_1_bit (t2i_set [t], i);
#endif /* !is_sdag */

    /* ... filtrees par le resultat de basic_lexicalizer */
    sxba_and (item_set, lb2item_set [0]);
#if 0 && has_Lex
    /* On assure la compatibilite des locaux avec les Lex */
    sxba_and (item_set, Lex_prod_item_set);
#endif /* 0 && has_Lex */

    if (multiple_t_item_set) {
      /* multiple_t_item_set, initialise ds les tables, contient les t_items multiples */
      sxba_copy (imultiple_t_item_set, multiple_t_item_set);
      sxba_and (imultiple_t_item_set, item_set);
      /* On filtre en plus en tenant compte des terminaux multiples ds les prod */
      prev_prod = 0;
      item = -1;

      while ((item = sxba_scan (imultiple_t_item_set, item)) >= 0) {
	t = -lispro [item];
	prod = prolis [item];

	if (!SXBA_bit_is_set (source_t_set, t)) {
	  /* prod est a ancre multiples, mais toutes ses ancres ne sont pas ds le suffixe */
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  bot = prolon [prod];
	  top = prolon [prod+1];
	  sxba_clear_range (item_set, bot, top-bot);

	  /* On "saute" prod */
	  item = top-1;
	}
	else {
	  /* On verifie l'ordre */
	  if (prev_prod != prod) {
	    /* 1ere ancre de prod */
	    prev_prod = prod;
	    j = -1;
	  }

	  if ((j = sxba_scan (t2i_set [t], j)) < 0) {
	    /* echec */
	    bot = prolon [prod];
	    top = prolon [prod+1];
	    sxba_clear_range (item_set, bot, top-bot);
	    /* On "saute" prod */
	    item = top-1;
	  }
	}
      }
    }

    /* On fusionne avec les indices superieurs */
    sxba_or (item_set, lb2item_set [i]);
  }

#if is_epsilon
  /* En fait, l'indice n est rempli avec les prod vides */
  /* C'est mieux qu'avec les non lexicalisees */
  sxba_copy (lb2item_set [n], empty_prod_item_set);
  /* ... filtrees par le resultat de basic_lexicalizer */
  sxba_and (lb2item_set [n], lb2item_set [0]);
#endif /* is_epsilon */

  if (multiple_t_item_set)
    sxbm_free (t2i_set);

#if 0
#if has_Lex
#ifdef lrprod_list_include_file
    /* On exploite les "lrprod" */
    /* une prod gauche g ne peut commencer en i que s'il existe une prod droite d associee qui commence en j, j >= i
       Si l est la taille minimale des phrases de g on a j == i+l.  Ici on prend l==0 */
    for (i = n; i > 0; i--) {
      lrprod_filtering (lb2item_set [i], IS_LEFT_PROD, lb2item_set [i], IS_RIGHT_PROD);
    }
#endif /* lrprod_list_include_file */
#endif /* has_Lex */
#endif
}
#if 0
/* ESSAI */
/* Un peu + long */

static void ub_lexicalizer (SXBA *ub2item_set)
{
  SXINT t, i, j, prod, prev_prod, item, bot, top;
  SXBA *new_t_sets, *t2i_set, i_set, item_set, new_t_set, imultiple_t_item_set, glbl_source_i;
  SXBA_CST_ALLOC (prefix_source_set, eof+1);
  SXBA_CST_ALLOC (working_set, itemmax+1);

  sxba_empty (prefix_source_set);

  if (multiple_t_item_set) {
    imultiple_t_item_set = &(working_set [0]);
    t2i_set = sxbm_calloc (eof+1, n+1);
  }

  if (n >= 1) {
    /* On remplit l'indice n avec basic */
    basic_lexicalizer (ub2item_set [n]);

    /* Une passe gauche droite sur le source cumule les prefixes... */
    new_t_sets = sxbm_calloc (n+1, eof+1);

    for (i = 1; i <= n; i++) {
      new_t_set = new_t_sets [i];
      glbl_source_i = glbl_source [i];
      sxba_copy (new_t_set, glbl_source_i);
      sxba_minus (new_t_set, prefix_source_set);
      sxba_or (prefix_source_set, glbl_source_i);

      if (multiple_t_item_set) {    
	t = 0;

	while ((t = sxba_scan (glbl_source_i, t)) > 0) {
	  SXBA_1_bit (t2i_set [t], i);
	}
      }
    }

    for (i = n-1; i > 0; i--) {
      /* on cherche les nouveaux terminaux entre les niveaux i et i+1 */
      item_set = ub2item_set [i]; /* vide */
      sxba_copy (item_set, ub2item_set [i+1]);

      new_t_set = new_t_sets [i+1];

      t = 0;

      while ((t = sxba_scan (new_t_set, t)) > 0) {
	/* t est nouveau */
	sxba_minus (item_set, t2prod_item_set [t]);
      }

      /* On verifie l'ordre des ancres multiples */
      if (multiple_t_item_set) {
	sxba_copy (imultiple_t_item_set, multiple_t_item_set);
	sxba_and (imultiple_t_item_set, item_set);
	/* On filtre en plus en tenant compte des terminaux multiples ds les prod */
	prev_prod = 0;
	item = -1;

	while ((item = sxba_scan (imultiple_t_item_set, item)) >= 0) {
	  t = -lispro [item];
	  i_set = t2i_set [t];
	  prod = prolis [item];

	  if (prev_prod != prod) {
	    /* 1ere ancre de prod */
	    prev_prod = prod;
	    j = -1;
	  }

	  if ((j = sxba_scan (i_set, j)) < 0 || j > i) {
	    /* echec */
	    bot = prolon [prod];
	    top = prolon [prod+1];
	    sxba_clear_range (item_set, bot, top-bot);
	    /* On "saute" prod */
	    item = top-1;
	  }
	}
      }
    }

    sxbm_free (new_t_sets);
  }

#if is_epsilon
  /* L'indice 0 est rempli avec les prod vides */
  /* C'est mieux qu'avec les non lexicalisees... */
  /* ... sxba_copy (ub2item_set [0], t2prod_item_set [0]); */
  sxba_copy (ub2item_set [0], empty_prod_item_set);
#if has_Lex
  /* On supprime aussi les items invalides dus aux &Lex */
  sxba_and (ub2item_set [0], Lex_prod_item_set);
#endif /* has_Lex */
#endif /* is_epsilon */
}

static void lb_lexicalizer (SXBA *lb2item_set)
{
  SXINT t, i, j, prod, prev_prod, item, bot, top;
  SXBA *new_t_sets, *t2i_set, i_set, item_set, new_t_set, imultiple_t_item_set, glbl_source_i;
  SXBA_CST_ALLOC (prefix_source_set, eof+1);
  SXBA_CST_ALLOC (working_set, itemmax+1);

  sxba_empty (prefix_source_set);

  if (multiple_t_item_set) {
    imultiple_t_item_set = &(working_set [0]);
    t2i_set = sxbm_calloc (eof+1, n+1);
  }

  if (n >= 1) {
    /* On remplit l'indice 0 avec basic */
    basic_lexicalizer (lb2item_set [0]);

    /* Une passe droite gauche sur le source cumule les suffixes... */
    new_t_sets = sxbm_calloc (n+1, eof+1);

    for (i = n; i >= 1; i--) {
#if is_sdag
      new_t_set = new_t_sets [i];
      glbl_source_i = glbl_source [i];
      sxba_copy (new_t_set, glbl_source_i);
      sxba_minus (new_t_set, prefix_source_set);
      sxba_or (prefix_source_set, glbl_source_i);
#else /* !is_sdag */
      t = glbl_source [i];

      if (SXBA_bit_is_reset_set (prefix_source_set, t)) {
	SXBA_1_bit (new_t_sets [i], t);
      }
#endif /* !is_sdag */

      if (multiple_t_item_set) { 
#if is_sdag   
	t = 0;

	while ((t = sxba_scan (glbl_source_i, t)) > 0) {
	  SXBA_1_bit (t2i_set [t], i);
	}
#else /* !is_sdag */
	SXBA_1_bit (t2i_set [t], i);
#endif /* !is_sdag */
      }
    }

    for (i = 1; i < n; i++) {
      /* on parcourt les terminaux du niveau i qui ne sont pas ds le suffixe */
      item_set = lb2item_set [i];
      sxba_copy (item_set, lb2item_set [i-1]);

      new_t_set = new_t_sets [i];

      t = 0;

      while ((t = sxba_scan (new_t_set, t)) > 0) {
	/* t est nouveau */
	sxba_minus (item_set, t2prod_item_set [t]);
      }

      /* On verifie l'ordre des ancres multiples */
      if (multiple_t_item_set) {
	sxba_copy (imultiple_t_item_set, multiple_t_item_set);
	sxba_and (imultiple_t_item_set, item_set);
	/* On filtre en plus en tenant compte des terminaux multiples ds les prod */
	prev_prod = 0;
	item = -1;

	while ((item = sxba_scan (imultiple_t_item_set, item)) >= 0) {
	  t = -lispro [item];
	  i_set = t2i_set [t];
	  prod = prolis [item];

	  if (prev_prod != prod) {
	    /* 1ere ancre de prod */
	    prev_prod = prod;
	    j = i;
	  }

	  if ((j = sxba_scan (i_set, j)) < 0) {
	    /* echec */
	    bot = prolon [prod];
	    top = prolon [prod+1];
	    sxba_clear_range (item_set, bot, top-bot);
	    /* On "saute" prod */
	    item = top-1;
	  }
	}
      }
    }

    sxbm_free (new_t_sets);
  }

#if is_epsilon
  /* L'indice n est rempli avec les prod vides */
  /* C'est mieux qu'avec les non lexicalisees... */
  /* ... sxba_copy (lb2item_set [n], t2prod_item_set [0]); */
  sxba_copy (lb2item_set [n], empty_prod_item_set);
#if has_Lex
  /* On supprime aussi les items invalides dus aux &Lex */
  sxba_and (lb2item_set [n], Lex_prod_item_set);
#endif /* has_Lex */
#endif /* is_epsilon */
}
#endif /* 0 */
#endif /* is_lex2 */
#endif /* is_lex */

#if is_set_automaton
/* "parseur" fonde' sur un set-automaton (la structure de controle est un ensemble et pas une pile)
   C'est un automate fini
   Analyse en temps lineaire */
/* Il peut etre utilise comme shallow parseur (option -Dis_shallow=1) et/ou comme supertaggeur
   (option -Dis_supertagging=1) ou comme "lexicaliseur" (option -Dis_lex=1) */

/* ESSAI : si OK le faire partout ou c'est possible
static SXBA_CST_ALLOC (set, size);
 */

static SXBA rl_valid_item_set_i, lr_valid_item_set_i, cur_titem_set;
static SXBA_CST_ALLOC (valid_suffix_item_set, itemmax+1);
static SXBA_CST_ALLOC (static_w_set1, itemmax+1); /* Pour rl_valid_item_set_i */
static SXBA_CST_ALLOC (static_w_set2, itemmax+1); /* Pour lr_valid_item_set_i */
static SXBA_CST_ALLOC (int_set, ntmax+1);
static SXBA_CST_ALLOC (nt_set, ntmax+1);
static SXINT nt_stack [ntmax+1];

/* Ici, on essaie d'etre + malin */
/* On utilise la relation left_corner (et right_corner) */
/* valid_prefixes avec nt2lc_item_set [1] */
/* A chaque etape, si c'est la 1ere fois qu'on voit un A, on ajoute
   nt2lc_item_set [A] */

/* left_corner = {(A, B) | A =>* \alpha B \beta =>* B \beta} */
/* nt2lc_item_set [A] = {B -> \alpha . \beta | B left_corner A et \alpha =>* \epsilon} */

/* right_corner = {(A, B) | A =>* \alpha B \beta =>* \alpha B} */
/* nt2rc_item_set [A] = {B -> \alpha . \beta | B right_corner A et \beta =>* \epsilon} */

/* valeurs initiales ds les tables */
/* titem_set = {A -> \alpha .t \beta | A -> \alpha t \beta \in P} */
/* rc_titem_set = {A -> \alpha t . \beta | A -> \alpha t \beta \in P} */

#if 0
/* Ancienne version des set automata */
#if is_lrsa
static SXBA lr_valid_item_set_i;
static SXBA_CST_ALLOC (valid_prefix_item_set, itemmax+1);
#if is_supertagging
SXBA *i2lr_supertagger_item_set;
#endif /* is_supertagging */

/* On vient de reconnaitre U (item = A -> \alpha U . \beta) */

static void set_automaton_parser_valid_prefix_item_set (item)
     SXINT item;
{
  SXINT X, prod, bot, A;

#if 0
  /* Le 21/05/2003 : on prend lr_valid_item_set_i plutot que valid_prefix_item_set */
  /* Les reductions qui ne couvrent pas la chaine vide ne dependent que du control set
     de l'etat precedent. */
  SXBA_1_bit (valid_prefix_item_set, item);
#endif /* 0 */
  SXBA_1_bit (lr_valid_item_set_i, item);
  X = lispro [item];

#if is_epsilon
  while (X != 0) {
    if (X < 0) {
#if 0
      /* Le 21/05/2003 : cur_titem_set sera extrait de lr_valid_item_set_i */
      SXBA_1_bit (cur_titem_set, item);
#endif /* 0 */
      return;
    }

    if (SXBA_bit_is_reset_set (int_set, X)) {
      /* C'est la 1ere fois qu'on ferme X au niveau i */
#if is_lex2
      sxba_or_and (lr_valid_item_set_i, rl_valid_item_set_i, nt2lc_item_set [X]);
#else /* !is_lex2 */
      sxba_or (lr_valid_item_set_i, nt2lc_item_set [X]);
#endif /* !is_lex2 */
    }

    if (!SXBA_bit_is_set (BVIDE, X))
      return;

    item++;
#if 0
    SXBA_1_bit (valid_prefix_item_set, item);
#endif /* 0 */
    SXBA_1_bit (lr_valid_item_set_i, item);
    X = lispro [item];
  }
#else /* !is_epsilon */
  if (X != 0) {
    /*
      Le 21/05/2003 : cur_titem_set sera extrait de lr_valid_item_set_i
      if (X < 0) {
      SXBA_1_bit (cur_titem_set, item);
      }
      else { */
    if (X > 0) {
      if (SXBA_bit_is_reset_set (int_set, X)) {
	/* C'est la 1ere fois qu'on ferme X au niveau i */
#if is_lex2
	sxba_or_and (lr_valid_item_set_i, rl_valid_item_set_i, nt2lc_item_set [X]);
#else /* !is_lex2 */
	sxba_or (lr_valid_item_set_i, nt2lc_item_set [X]);
#endif /* !is_lex2 */
      }
    }

    return;
  }
#endif /* !is_epsilon */

  /* On a reconnu prod */
  /* Elle couvre une sous-chaine non vide */
  prod = prolis [item];
#if has_Lex
  if (prod2t_nb [prod] == 0
      && SXBA_bit_is_set (must_check_identical_prod_set, prod)
      /* Prod non lexicalisee (les autres ont ete verifiees) representante d'une classe d'equivalence de prod identiques
	 sur lesquelles il faut verifier les Lex */
      && !check_Lex (prod))
    /* Echec de la verification */
    return;
#endif /* has_Lex */
  bot = prolon [prod];
  A = lhs [prod];

  if (!SXBA_bit_is_set (ilex_compatible_item_set, bot))
    sxba_range (ilex_compatible_item_set, bot, prolon [prod+1]-bot);

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    PUSH (nt_stack, A);
  }
}
#endif /* is_lrsa */

/* On vient de reconnaitre U (item = A -> \alpha . U \beta) */

static void set_automaton_parser_valid_suffix_item_set (SXINT item)
{
  SXINT X, prod, A, bot;
#if 0
  /* Le 21/05/2003 : on prend rl_valid_item_set_i plutot que valid_suffix_item_set */
  /* Les reductions qui ne couvrent pas la chaine vide ne dependent que du control set
     de l'etat precedent. */
  SXBA_1_bit (valid_suffix_item_set, item);
#endif /* 0 */
  SXBA_1_bit (rl_valid_item_set_i, item);
  bot = prolon [prod = prolis [item]];

#if is_epsilon
  while (item > bot) {
    X = lispro [item-1];

    if (X < 0) {
#if 0
      /* Le 21/05/2003 : cur_titem_set sera extrait de rl_valid_item_set_i */
      SXBA_1_bit (cur_titem_set, item);
#endif /* 0 */
      return;
    }

    if (SXBA_bit_is_reset_set (int_set, X)) {
      /* C'est la 1ere fois qu'on ferme X au niveau i */
#if is_lrsa || is_lex2
	sxba_or_and (rl_valid_item_set_i, lr_valid_item_set_i, nt2rc_item_set [X]);
#else /* !(is_lrsa || is_lex2) */
	sxba_or (rl_valid_item_set_i, nt2rc_item_set [X]);
#endif /* !(is_lrsa || is_lex2) */
    }

    if (!SXBA_bit_is_set (BVIDE, X))
      return;

    item--;
#if 0
    SXBA_1_bit (valid_suffix_item_set, item);
#endif /* 0 */
    SXBA_1_bit (rl_valid_item_set_i, item);
  }
#else /* !is_epsilon */
  if (item > bot) {
    X = lispro [item-1];

    /*
      Le 21/05/2003 : cur_titem_set sera extrait de lr_valid_item_set_i
    if (X < 0) {
      SXBA_1_bit (cur_titem_set, item);
    }
    else { */
    if (X > 0) {
      if (SXBA_bit_is_reset_set (int_set, X)) {
	/* C'est la 1ere fois qu'on ferme X au niveau i */
#if is_lrsa || is_lex2
	sxba_or_and (rl_valid_item_set_i, lr_valid_item_set_i, nt2rc_item_set [X]);
#else /* !(is_lrsa || is_lex2) */
	sxba_or (rl_valid_item_set_i, nt2rc_item_set [X]);
#endif /* !(is_lrsa || is_lex2) */
      }
    }

    return;
  }
#endif /* !is_epsilon */

  /* On a reconnu prod */
  /* Elle couvre une sous-chaine non vide */
#if has_Lex
  if (prod2t_nb [prod] == 0
      && SXBA_bit_is_set (must_check_identical_prod_set, prod)
      /* Prod non lexicalisee (les autres ont ete verifiees) representante d'une classe d'equivalence de prod identiques
	 sur lesquelles il faut verifier les Lex */
      && !check_Lex (prod))
    /* Echec de la verification */
    return;
#endif /* has_Lex */

  A = lhs [prod];

  if (!SXBA_bit_is_set (ilex_compatible_item_set, bot))
    sxba_range (ilex_compatible_item_set, bot, prolon [prod+1]-bot);

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    PUSH (nt_stack, A);
  }
}

static void set_automaton_parser (SXBA *lb2prod_item_set)
{
  SXINT t, item, item_1, X, i, A, bot, top, prod; 
  SXBA supertagger_item_set, glbl_source_i, source2valid_item_set;
  SXBA_CST_ALLOC (w_set2, itemmax+1);
  SXBA_CST_ALLOC (w_set1, itemmax+1);
#if is_lrsa || is_lex2
  SXBA *ub2prod_item_set;
#endif /* is_lrsa || is_lex2 */
#if is_lrsa && is_lex2
  SXBA *lb2item_set;
#endif /* is_lrsa && is_lex2 */
#if is_lex2 || is_lex
  SXINT nt;
  SXINT store_npd [ntmax+1];
  static SXINT store_numpd [sizeof(numpd)/sizeof(*numpd)];
#else /* !(is_lex2 || is_lex) */
  SXINT *store_npd, *store_numpd;
#endif /* !(is_lex2 || is_lex) */

#if 0
  /* ESSAI */
  /* TEST : avec xtag3 sur les 42253 phrases de wsj-1MW.sdag, 41706 seulement filtrent (par lrprod)
     572224 productions indexees.
     Donc, en moyenne 13.7 prod par phrases, cad 1/2 prod par position ds le source en moyenne => pas terrible.
     Ce test vaut-il le coup ds set_automaton_parser ?
     Reponse non, il n'est pas incorpore'.
     De plus il a l'air couteux en temps
   */
  SXBA *lb, *ub;

  ub = sxbm_calloc (n+1, itemmax+1);
  ub_lexicalizer (ub);
  lb = sxbm_calloc (n+1, itemmax+1);
  lb_lexicalizer (lb);
  
  /* Si une prod droite commence en i, il doit y avoir une prod gauche qui se termine en i */
  for (i = n; i > 0; i--) {
    lrprod_filtering (lb [i], IS_RIGHT_PROD, ub [i], IS_LEFT_PROD);
  }
  if (lrprod_filtered_nb)
    fprintf (sxstderr, "lrprod_filtered_nb = %i ", lrprod_filtered_nb);

  sxbm_free (ub);
  sxbm_free (lb);
  /* FIN ESSAI */
#endif

  cur_titem_set = &(w_set1 [0]);

#if is_lrsa || is_lex2
  ub2prod_item_set = sxbm_calloc (n+1, itemmax+1);
#endif /* is_lrsa || is_lex2 */

  /* Pour remettre en l'etat... */
  for (nt = 1; nt <= ntmax; nt++) {
    store_npd [nt] = npd [nt];
  }

#if is_lrsa
#if is_lex2
  lb2item_set = sxbm_calloc (n+1, itemmax+1);
  lb_lexicalizer (lb2item_set);
  source2valid_item_set = lb2item_set [0];
#else /* !is_lex2 */
  sxba_empty (source2valid_item_set = &(w_set2 [0]));
  basic_lexicalizer (source2valid_item_set);
#endif /* !is_lex2 */

  item = -1;

  while ((item = sxba_scan (source2valid_item_set, item)) >= 0) {
    if ((X = lispro [item]) > 0) {
      numpd [--npd [X]] = item;
    }
  }

#if is_lex2
  /* On filtre les tables statiques */
  sxba_and (nt2lc_item_set [1], source2valid_item_set);
  /* dans le cas is_lex2 le filtrage se fera dynamiquement pour chaque indice */
#else /* !is_lex2 */
  for (X = 1; X <= ntmax; X++)
    sxba_and (nt2lc_item_set [X], source2valid_item_set);
#endif /* !is_lex2 */

  /* On part des items initiaux valides par la lexicalisation et connectes a S' -> $ . S $ */
  sxba_copy (valid_prefix_item_set, nt2lc_item_set [1]);

  lr_valid_item_set_i = &(static_w_set2 [0]);

  sxba_copy (lr_valid_item_set_i, valid_prefix_item_set);

  sxba_copy (cur_titem_set, valid_prefix_item_set);
  sxba_and (cur_titem_set, titem_set);

  /* On prepare le nouveau source2valid_item_set */
  source2valid_item_set = &(w_set2 [0]);

#if is_epsilon
  /* Le 21/05/2003 : on ne considere que les items connectes a S' -> $ . S $ */
  sxba_or_and (ub2prod_item_set [0], empty_prod_item_set, valid_prefix_item_set);
  /* sxba_copy (ub2prod_item_set [0], empty_prod_item_set); */
  sxba_copy (source2valid_item_set, ub2prod_item_set [0]);
#else /* !is_epsilon */
  sxba_empty (source2valid_item_set);
#endif /* !is_epsilon */

  /* valid_prefix_item_set est le control set.
     lr_valid_item_set_i est l'active set.
     cur_titem_set est la partie de l'active set qui ne contient que des titems. */

  for (i = 1; i <= n; i++) {
    ilex_compatible_item_set = ub2prod_item_set [i]; /* vide */
#if 0
    /* Le 21/05/2003 : deplace' en fin de boucle et modifie' */
#if is_epsilon
    sxba_copy (ilex_compatible_item_set, empty_prod_item_set);
#endif /* is_epsilon */
#endif /* 0 */

#if is_lex2
    rl_valid_item_set_i = lb2item_set [i];
#endif /* is_lex2 */

#if is_supertagging && !is_recognizer
    supertagger_item_set = i2lr_supertagger_item_set [i];
#endif /* is_supertagging && !is_recognizer */

#if is_sdag
    glbl_source_i = glbl_source [i];
#endif /* is_sdag */
    /* Le 21/05/2003
       item = itemmax;
    
       while ((item = sxba_1_rlscan (cur_titem_set, item)) >= 0) {
       SXBA_0_bit (cur_titem_set, item);
    */

    item = -1;

    while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
      t = -lispro [item];

      if (
#if is_sdag
	  SXBA_bit_is_set (glbl_source_i, t)
#else /* !is_sdag */
	  glbl_source [i] == t
#endif /* !is_sdag */
	) {
	/* Bon terminal */
#if has_Lex
	prod = prolis [item];

	if (SXBA_bit_is_set (must_check_identical_prod_set, prod)
	    /* Prod est le representant d'une classe d'equivalence de prod identiques sur lesquelles
	       il faut verifier les Lex */
	    && !check_Lex (prod))
	    continue;
#endif /* has_Lex */
#if is_supertagging && !is_recognizer
	SXBA_1_bit (supertagger_item_set, item);
#endif /* is_supertagging && !is_recognizer */

	set_automaton_parser_valid_prefix_item_set (item+1);
      }
    }

    if (!IS_EMPTY (nt_stack)) {
      do {
	A = POP (nt_stack);

	/* On cherche toutes les occurrences de A en RHS telle que B -> \alpha . A \beta soit ds valid_prefix_item_set */
	bot = npd [A];

	while (item = numpd [bot++]/* B -> \alpha . A  \beta */) {
	  if (SXBA_bit_is_set (valid_prefix_item_set, item)) {
	    set_automaton_parser_valid_prefix_item_set (item+1);
	  }
	}
      } while (!IS_EMPTY (nt_stack));

      sxba_empty (nt_set);
    }

    /* On change de niveau */
    sxba_empty (int_set);
    /* lr_valid_item_set_i est l'active set */
    /* On fabrique le nouveau control set */
    sxba_or (valid_prefix_item_set, lr_valid_item_set_i);
    /* Ici cur_titem_set est vide */
    sxba_or_and (cur_titem_set, lr_valid_item_set_i, titem_set);
#if is_epsilon
    /* On ajoute les productions vides des fermetures */
    sxba_or_and (ilex_compatible_item_set, lr_valid_item_set_i, empty_prod_item_set);
#endif /* is_epsilon */
    sxba_empty (lr_valid_item_set_i);

    sxba_or (source2valid_item_set, ilex_compatible_item_set);
  }

  /* On remet en etat */
  for (nt = 1; nt <= ntmax; nt++)
    npd [nt] = store_npd [nt];
#else /* !is_lrsa */
#if is_lex2
  ub_lexicalizer (ub2prod_item_set);
  source2valid_item_set = ub2prod_item_set [n];
#else /* !is_lex2 */
  /* Cas de la passe gauche droite minimale */
  basic_lexicalizer (source2valid_item_set = &(w_set2 [0]));
#endif /* !is_lex2 */
#endif /* !is_lrsa */

  /* On fait maintenant un passage droite gauche en utilisant la relation right_corner */

  /* On fabrique/complete les npd/numpd en tenant compte de source2valid_item_set */

  item = -1;

  while ((item = sxba_scan (source2valid_item_set, item)) >= 0) {
    if ((X = lispro [item]) > 0) {
      numpd [--npd [X]] = item;
    }
  }

#if is_lrsa || is_lex2
  sxba_and (nt2rc_item_set [1], source2valid_item_set);
  /* Ici, le filtrage des autres se fera dynamiquement pour chaque indice */
#else /* !(is_lrsa || is_lex2) */
  /* On filtre les tables statiques */
  for (X = 1; X <= ntmax; X++)
    sxba_and (nt2rc_item_set [X], source2valid_item_set);
#endif /* !(is_lrsa || is_lex2) */

  /* sxba_and (rc_titem_set, source2valid_item_set); */

#if is_lrsa && is_lex2
  sxbm_free (lb2item_set);
#endif /* is_lrsa && is_lex2 */

  rl_valid_item_set_i = &(static_w_set1 [0]);

  /* On part des items reduce atteints par la passe gauche-droite eventuelle et connectes a S' -> $ S . $ */
  sxba_copy (valid_suffix_item_set, nt2rc_item_set [1]);

  sxba_copy (cur_titem_set, valid_suffix_item_set);
  sxba_and (cur_titem_set, rc_titem_set);

#if is_epsilon
  /* Le 21/05/2003 : on ne considere que les items connectes a S' -> $ S . $ */
  sxba_or_and (lb2prod_item_set [n], empty_prod_item_set, valid_suffix_item_set);
  /* sxba_copy (lb2prod_item_set [n], empty_prod_item_set); */
#endif /* is_epsilon */

  /* valid_suffix_item_set est le control set.
     rl_valid_item_set_i est l'active set.
     cur_titem_set est la partie de l'active set qui ne contient que des titems. */

  for (i = n; i > 0; i--) {
    ilex_compatible_item_set = lb2prod_item_set [i-1]; /* vide */
#if 0
    /* Le 21/05/2003 : deplace' en fin de boucle et modifie' */
#if is_epsilon
    sxba_copy (ilex_compatible_item_set, empty_prod_item_set);
#endif /* is_epsilon */
#endif /* 0 */

    /* Contient les items reduce de la forme A -> \alpha . \beta, \beta =>* \epsilon pour le niveau i */
    /* utilise comme filtre */
#if is_lrsa || is_lex2
    lr_valid_item_set_i = ub2prod_item_set [i-1];
#endif /* is_lrsa || is_lex2 */

#if is_supertagging && !is_recognizer
    supertagger_item_set = i2rl_supertagger_item_set [i];
#endif /* is_supertagging && !is_recognizer */

#if is_sdag
    glbl_source_i = glbl_source [i];
#endif /* is_sdag */
    item = -1;

    while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
      t = -lispro [item-1];

      if (
#if is_sdag
	  SXBA_bit_is_set (glbl_source_i, t)
#else /* !is_sdag */
	  glbl_source [i] == t
#endif /* !is_sdag */
	) {
	/* Bon terminal */
#if has_Lex
	prod = prolis [item];

	if (SXBA_bit_is_set (must_check_identical_prod_set, prod)
	    /* Prod est le representant d'une classe d'equivalence de prod identiques sur lesquelles
	       il faut verifier les Lex */
	    && !check_Lex (prod))
	    continue;
#endif /* has_Lex */
#if is_supertagging && !is_recognizer
	item_1 = item-1;
	SXBA_1_bit (supertagger_item_set, item_1);
#endif /* is_supertagging && !is_recognizer */

	set_automaton_parser_valid_suffix_item_set (item-1);
      }
    }

    if (!IS_EMPTY (nt_stack)) {
      do {
	A = POP (nt_stack);

	/* On cherche toutes les occurrences de A en RHS telle que B -> \alpha A . \beta soit ds valid_suffix_item_set */
	bot = npd [A];

	while (item = numpd [bot++]/* B -> \alpha . A  \beta */) {
	  item_1 = item+1;

	  if (SXBA_bit_is_set (valid_suffix_item_set, item_1)) {
	    set_automaton_parser_valid_suffix_item_set (item);
	  }
	}
      } while (!IS_EMPTY (nt_stack));

      sxba_empty (nt_set);
    }

    /* On change de niveau */
    sxba_empty (int_set);
    /* rl_valid_item_set_i est l'active set */
    /* On fabrique le nouveau control set */
    sxba_or (valid_suffix_item_set, rl_valid_item_set_i);

    /* Ici cur_titem_set est vide */
    sxba_or_and (cur_titem_set, rl_valid_item_set_i, rc_titem_set);
#if is_epsilon
    /* On ajoute les productions vides des fermetures */
    sxba_or_and (ilex_compatible_item_set, rl_valid_item_set_i, empty_prod_item_set);
#endif /* is_epsilon */
    sxba_empty (rl_valid_item_set_i);
  }

#if is_lrsa || is_lex2
  sxbm_free (ub2prod_item_set);
#endif /* is_lrsa || is_lex2 */

  /* Prudence on remet npd pour des utilisations futures... */
  for (nt = 1; nt <= ntmax; nt++)
    npd [nt] = store_npd [nt];
}
#endif /* 0 */


/* NOUVELLE VERSION
   On fait un passage gauche droite qui note pour chaque position du source l'active set calcule.
   Puis, par un passage droite gauche, on ne conserve ds chaque active set que les items connectes
   a l'item final [S' -> $ S. $]
   C'est +precis que set_automaton_parser () => on conserve
   -Dis_lrsa est donc sans effet...
 */
static SXBA_CST_ALLOC (control_set, itemmax+1);
static SXBA active_set, rl_active_set, closure_set;

/* On vient de reconnaitre U (item = A -> \alpha U . \beta) */

static void close_reduce (SXINT item)
{
  SXINT X, prod, bot, A;

  SXBA_1_bit (active_set, item);
  X = lispro [item];

#if is_epsilon
  while (X != 0) {
    if (X < 0) {
      return;
    }

    if (SXBA_bit_is_reset_set (int_set, X)) {
      /* C'est la 1ere fois qu'on ferme X au niveau i */
      sxba_or (active_set, nt2lc_item_set [X]);
    }

    if (!SXBA_bit_is_set (BVIDE, X))
      return;

    item++;
    SXBA_1_bit (active_set, item);
    X = lispro [item];
  }
#else /* !is_epsilon */
  if (X != 0) {
    if (X > 0 && SXBA_bit_is_reset_set (int_set, X)) {
      /* C'est la 1ere fois qu'on ferme X au niveau i */
      sxba_or (active_set, nt2lc_item_set [X]);
    }

    return;
  }
#endif /* !is_epsilon */

  /* On a reconnu prod */
  /* Elle couvre une sous-chaine non vide */
  prod = prolis [item];

#if has_Lex
  if (prod2t_nb [prod] == 0
      && SXBA_bit_is_set (must_check_identical_prod_set, prod)
      /* Prod non lexicalisee (les autres ont ete verifiees) representante d'une classe d'equivalence de prod identiques
	 sur lesquelles il faut verifier les Lex */
      && !check_Lex (prod))
    /* Echec de la verification */
    return;
#endif /* has_Lex */

  A = lhs [prod];

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    /* premiere reduction vers A */
    PUSH (nt_stack, A);
  }
}

/* On vient de reconnaitre U (item = A -> \alpha . U \beta) */

static void rl_close_reduce (SXINT item)
{
  SXINT X, prod, A, bot;

  SXBA_1_bit (rl_active_set, item);
  bot = prolon [prod = prolis [item]];

#if is_epsilon
  while (item > bot) {
    X = lispro [item-1];

    if (X < 0) {
      return;
    }

    if (SXBA_bit_is_reset_set (int_set, X)) {
      /* C'est la 1ere fois qu'on ferme X au niveau i */
      sxba_or (rl_active_set, nt2rc_item_set [X]);
    }

    if (!SXBA_bit_is_set (BVIDE, X))
      return;

    item--;
    SXBA_1_bit (rl_active_set, item);
  }
#else /* !is_epsilon */
  if (item > bot) {
    X = lispro [item-1];

    if (X > 0 && SXBA_bit_is_reset_set (int_set, X)) {
      /* C'est la 1ere fois qu'on ferme X au niveau i */
      sxba_or (rl_active_set, nt2rc_item_set [X]);
    }

    return;
  }
#endif /* !is_epsilon */

  /* On a reconnu prod */
  /* Elle couvre une sous-chaine non vide ... */
  /* ...et a ete validee par la passe gauche droite */
  if (!SXBA_bit_is_set (ilex_compatible_item_set, bot))
    sxba_range (ilex_compatible_item_set, bot, prolon [prod+1]-bot);

  A = lhs [prod];

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    PUSH (nt_stack, A);
  }
}


static bool dynamic_set_automaton (SXBA *lb2prod_item_set)
{
  SXINT t, item, item_1, X, i, A, bot, top, prod; 
  SXBA supertagger_item_set, glbl_source_i, source2valid_item_set;
  SXBA_CST_ALLOC (w_set3, itemmax+1);
  SXBA_CST_ALLOC (w_set2, itemmax+1);
  SXBA_CST_ALLOC (w_set1, itemmax+1);
  SXBA *active_sets;
#if is_lex2
  SXBA *lb2item_set;
  SXBA_CST_ALLOC (w_set4, itemmax+1);
#endif /* is_lex2 */
#if is_lex2 || is_lex
  SXINT nt;
  SXINT store_npd [ntmax+1];
  static SXINT store_numpd [sizeof(numpd)/sizeof(*numpd)];
#else /* !(is_lex2 || is_lex) */
  SXINT *store_npd, *store_numpd;
#endif /* !(is_lex2 || is_lex) */

  cur_titem_set = &(w_set1 [0]);
  active_sets = sxbm_calloc (n+1, itemmax+1);

#if is_lex2
  lb2item_set = sxbm_calloc (n+1, itemmax+1);
  lb_lexicalizer (lb2item_set);
  source2valid_item_set = lb2item_set [0];
#else /* !is_lex2 */
  sxba_empty (source2valid_item_set = &(w_set2 [0]));
  basic_lexicalizer (source2valid_item_set);
#endif /* !is_lex2 */

#if is_lex2 || is_lex
  /* On fabrique/complete les npd/numpd en tenant compte de source2valid_item_set */
  /* Pour remettre en l'etat... */
  for (nt = 1; nt <= ntmax; nt++) {
    store_npd [nt] = npd [nt+1]-1; /* On pointe vers le 0 de fin de liste (liste vide) */
  }

  item = -1;

  while ((item = sxba_scan (source2valid_item_set, item)) >= 0) {
    if ((X = lispro [item]) > 0) {
      store_numpd [--store_npd [X]] = item; /* On ajoute item a la liste */
    }
  }
#else /* !(is_lex2 || is_lex) */
  /* On utilise les tables statiques */
  store_npd = &(npd [0]);
  store_numpd = &(numpd [0]);
#endif /* !(is_lex2 || is_lex) */

#if is_lex2
  /* On filtre les tables statiques */
  sxba_and (nt2lc_item_set [1], source2valid_item_set);
  /* dans le cas is_lex2 le filtrage se fera dynamiquement pour chaque indice */
  sxba_empty (closure_set = &(w_set4 [0]));
#else /* !is_lex2 */
  for (X = 1; X <= ntmax; X++)
    sxba_and (nt2lc_item_set [X], source2valid_item_set);
#endif /* !is_lex2 */

  /* On part des items initiaux valides par la lexicalisation et connectes a S' -> $ . S $ */
  sxba_copy (control_set, nt2lc_item_set [1]);
  active_set = active_sets [0];
  sxba_copy (active_set, control_set);
  /* cur_titem_set est la partie de l'active set qui ne contient que des titems. */
  sxba_copy (cur_titem_set, active_set);
  sxba_and (cur_titem_set, titem_set);

  for (i = 1; i <= n; i++) {
    active_set = active_sets [i];

#if is_sdag
    glbl_source_i = glbl_source [i];
#endif /* is_sdag */

    item = -1;

    while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
      t = -lispro [item];

      if (
#if is_sdag
	  SXBA_bit_is_set (glbl_source_i, t)
#else /* !is_sdag */
	  glbl_source [i] == t
#endif /* !is_sdag */
	) {
	/* Bon terminal */
#if has_Lex
	prod = prolis [item];

	if (SXBA_bit_is_set (must_check_identical_prod_set, prod)
	    /* Prod est le representant d'une classe d'equivalence de prod identiques sur lesquelles
	       il faut verifier les Lex */
	    && !check_Lex (prod))
	    continue;
#endif /* has_Lex */

	close_reduce (item+1);
      }
    }

    if (!IS_EMPTY (nt_stack)) {
      do {
	A = POP (nt_stack);

	/* On cherche toutes les occurrences de A en RHS telle que B -> \alpha . A \beta soit ds control_set */
	bot = store_npd [A];

	while (item = store_numpd [bot++]/* B -> \alpha . A  \beta */) {
	  if (SXBA_bit_is_set (control_set, item)) {
	    close_reduce (item+1);
	  }
	}
      } while (!IS_EMPTY (nt_stack));

      sxba_empty (nt_set);
    }

    /* On change de niveau */
    sxba_empty (int_set);
#if is_lex2
    /* On filtre par le resultat de lb_lexicalizer */
    sxba_and (active_set, lb2item_set [i]);
#endif /* is_lex2 */
    /* On fabrique le nouveau control set... */
    sxba_or (control_set, active_set);
    /* ... et le nouveau cur_titem_set */
    /* Ici cur_titem_set est vide */
    sxba_or_and (cur_titem_set, active_set, titem_set);
  }

  /* On filtre les active_sets de droite a gauche */
  /* active_set == active_sets [n] */
  /* On ne conserve que les items connectes a l'item final [S' -> $ S. $] */
  sxba_and (active_set, nt2rc_item_set [1]);
  sxba_copy (control_set, active_set);

  sxba_copy (cur_titem_set, active_set);
  sxba_and (cur_titem_set, rc_titem_set);

#if is_epsilon
  sxba_or_and (lb2prod_item_set [n], empty_prod_item_set, active_set);
#endif /* is_epsilon */

  rl_active_set = &(w_set3 [0]);

  for (i = n; i > 0; i--) {
    sxba_empty (rl_active_set);
    ilex_compatible_item_set = lb2prod_item_set [i-1]; /* vide */

#if is_supertagging && !is_recognizer
    supertagger_item_set = i2rl_supertagger_item_set [i];
#endif /* is_supertagging && !is_recognizer */

    /* Normalement, tous les items de cur_titem_set sont de la forme
       A -> \alpha t. \beta, avec t==a_i */
    item = -1;

    while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
      item_1 = item-1;
#if EBUG
      t = -lispro [item_1];

      if (
#if is_sdag
	  !SXBA_bit_is_set (glbl_source [i], t)
#else /* !is_sdag */
	  glbl_source [i] != t
#endif /* !is_sdag */
	)
	sxtrap (ME, "set_automaton_parser");
#endif /* EBUG */

#if is_supertagging && !is_recognizer
	SXBA_1_bit (supertagger_item_set, item_1);
#endif /* is_supertagging && !is_recognizer */

      rl_close_reduce (item_1);
    }

    if (!IS_EMPTY (nt_stack)) {
      do {
	A = POP (nt_stack);

	/* On cherche toutes les occurrences de A en RHS telle que B -> \alpha A . \beta soit ds control_set */
	bot = store_npd [A];

	while (item = store_numpd [bot++]/* B -> \alpha . A  \beta */) {
	  item_1 = item+1;

	  if (SXBA_bit_is_set (control_set, item_1)) {
	    rl_close_reduce (item);
	  }
	}
      } while (!IS_EMPTY (nt_stack));

      sxba_empty (nt_set);
    }

    /* On change de niveau */
    sxba_empty (int_set);
    /* On filtre par la passe gauche-droite */
    sxba_and (rl_active_set, active_sets [i-1]);
    /* On fabrique le nouveau control set */
    sxba_or (control_set, rl_active_set);

    /* Ici cur_titem_set est vide */
    sxba_or_and (cur_titem_set, rl_active_set, rc_titem_set);
#if is_epsilon
    /* On ajoute les productions vides des fermetures */
    sxba_or_and (ilex_compatible_item_set, rl_active_set, empty_prod_item_set);
#endif /* is_epsilon */
  }

  sxbm_free (active_sets);
#if is_lex2
  sxbm_free (lb2item_set);
#endif /* is_lex2 */

  return true;
}
#endif /* is_set_automaton */
#endif /* !is_dag */


#if is_rfsa
#define PID 1 /* Pour l'instant */
#include "lrfsa.h"

struct fsaG        *rfsaGs [PID];

static void copy_out_source2source (struct fsaG **fsaGs)
{
  /* Une passe vient de re'ussir et a construit un nouveau source ds glbl_out_source */
  /* On fabrique le nouveau source pour la passe suivante par recopie */
  SXINT  pid, tok;
  SXBA out_line;
  SXBA set, fsa_valid_prod_set;

  sxba_empty (idag.source_set); /* On refabrique source_set */

  for (tok = 1; tok <= n; tok++) {
    out_line = glbl_out_source [tok];
    sxba_copy (glbl_source [tok], out_line);
    sxba_or (idag.source_set, out_line);
    sxba_empty (out_line);
  }
    
  /* SXBA_1_bit (glbl_source [0], 0), SXBA_1_bit (glbl_source [n+1], 0); deja fait */
}

static void lfsa_lexicalize (struct fsaG *G)
{
  SXINT	i, t, prod, prev_prod, item;
  SXBA  *t2i_set, glbl_source_i;

  if (!G->shallow) G->fsa_valid_prod_set = sxba_calloc (G->prod_nb+1);

  G->Lex = sxba_calloc (G->prod_nb+1);

  if (sxba_cardinal (idag.source_set) > split_val) {
    t = 0;

    while ((t = sxba_0_lrscan (idag.source_set, t)) > 0) {
      sxba_or (G->Lex, (G->t2prod_set) [t]);
    }

    /* Ici, G->Lex contient les prod non selectionnables */

    sxba_not (G->Lex); /* Ce qui n'est pas interdit est permis */
  }
  else {
    sxba_copy (G->Lex, G->t2prod_set [0]);

    t = 0;

    while ((t = sxba_scan (idag.source_set, t)) >= 0) {
      sxba_or (G->Lex, (G->t2prod_set) [t]);
    }

    if (multiple_t_item_set) {
      /* On filtre en plus en tenant compte des terminaux multiples ds les prod */
      item = -1;

      while ((item = sxba_scan (multiple_t_item_set, item)) >= 0) {
	t = -lispro [item];

	if (!SXBA_bit_is_set (idag.source_set, t)) {
	  /* "supprimer" prod */
	  prod = prolis [item];
	  SXBA_0_bit (G->Lex, prod);
	  /* On "saute" prod */
	  item = prolon [prod+1]-1;
	}
      }
    }
  }

  SXBA_1_bit (G->Lex, 0); /* Valide les transitions vides */

  /* On verifie en plus l'ordre des ancres multiples */
  /* Le test de l'ordre conduit a un set_automaton un tout petit peu + rapide */
  if (multiple_t_item_set) {
    /* on calcule t2i_set avec glbl_source */
    t2i_set = sxbm_calloc (eof+1, n+1);

    for (i = 1; i <= n; i++) {
#if is_sdag
      glbl_source_i = glbl_source [i];

      t = 0;

      while ((t = sxba_scan (glbl_source_i, t)) > 0) {
	SXBA_1_bit (t2i_set [t], i);
      }
#else /* !is_sdag */
      t = glbl_source [i];
      SXBA_1_bit (t2i_set [t], i);
#endif /* !is_sdag */
    }

    prev_prod = 0;
    item = -1;

    while ((item = sxba_scan (multiple_t_item_set, item)) >= 0) {
      t = -lispro [item];
      prod = prolis [item];

      if (prev_prod != prod) {
	/* 1ere ancre de prod */
	prev_prod = prod;
	i = -1;
      }

      if ((i = sxba_scan (t2i_set [t], i)) < 0) {
	/* echec */
	SXBA_0_bit (G->Lex, prod);
	/* On "saute" prod */
	item = prolon [prod+1]-1;
      }
    }

    sxbm_free (t2i_set);
  }
}

#if is_lfsa
struct fsaG        *lfsaGs [PID];

static bool call_lfsa ()
{
  SXINT         i, tnb;
  bool     ret_val, pid;
  char	      str [164];
  struct fsaG *fsaG;

  _lfsa_if (); /* Appel de l'interface lfsa qui remplit lfsaGs [0] */
    
  fsaG = lfsaGs [0];

  fsaG->prod2lub = sxbm_calloc (fsaG->prod_nb+1, n+1);

  lfsa_lexicalize (fsaG);

  ret_val = (*(fsaG->process_fsa)) (fsaG->init_state, 0);

  if (ret_val) {
    /* La passe lfsa a "reduit" le source, c'est celui la` qu'on va utiliser ds la suite */
    copy_out_source2source (lfsaGs);
  }

  if (is_print_time) {
    tnb = 0;

    for (i = 1; i <= n; i++)
#if is_sdag
      tnb += sxba_cardinal (glbl_source [i]);
#else /* !is_sdag */
      tnb += 1;
#endif /* !is_sdag */

#if EBUG2
    sprintf (str, "\tLFSA[t=%i/c=%i/ic=%i] (%s)",
	     tnb,
	     fsaG->fsa_valid_prod_set ? sxba_cardinal (fsaG->fsa_valid_prod_set) : 0,
	     fsaG->instantiated_prod_nb,
	     ret_val ? "true" : "false");
#else
    sprintf (str, "\tLFSA[t=%i] (%s)",
	     tnb,
	     ret_val ? "true" : "false");
#endif

    sxtime (TIME_FINAL, str);
  }
    
  (*(fsaG->final_fsa)) ();

  return ret_val;
}

/* La passe lfsa est terminee avec succes */

static void rfsa_lexicalize (struct fsaG *G)
{
  SXBA        lfsa_valid_prod_set;

  if (!G->shallow) G->fsa_valid_prod_set = sxba_calloc (G->prod_nb+1);

  /* On utilise le fsa_valid_prod_set de lfsa ou son Lex a la rigueur */
  lfsa_valid_prod_set = lfsaGs [G->pid]->fsa_valid_prod_set;

  if (lfsa_valid_prod_set == NULL) {
    lfsa_valid_prod_set = lfsaGs [G->pid]->Lex;
  }

#if EBUG
  if (lfsa_valid_prod_set == NULL)
    sxtrap (ME, "rfsa_lexicalize");
#endif

  SXBA_1_bit (lfsa_valid_prod_set, 0); /* Valide les transitions vides */
  G->Lex = lfsa_valid_prod_set;
  /* Attention G->Lex du rfsa ne contient pas les loop_prods alors que le
     G->Lex du lfsa les contenait!! */
}
#endif /* is_lfsa */



static bool call_rfsa ()
{
  SXINT     i, tnb, pid;
  bool ret_val;
  char	  str [164];
  SXBA    set;
  struct fsaG *fsaG;

  _rfsa_if (); /* Appel de l'interface rfsa qui remplit rfsaGs [0] */
    
  fsaG = rfsaGs [0];

  fsaG->prod2lub = sxbm_calloc (fsaG->prod_nb+1, n+1);

#if is_lfsa
  rfsa_lexicalize (fsaG);
#else
  lfsa_lexicalize (fsaG);
#endif

  ret_val = (*(fsaG->process_fsa)) (fsaG->init_state, n);

  if (ret_val) {
    /* La passe rfsa a "reduit" le source, c'est celui la` qu'on va utiliser ds la suite */
    copy_out_source2source (rfsaGs);
  }

  if (is_print_time) {
    tnb = 0;

    for (i = 1; i <= n; i++)
#if is_sdag
      tnb += sxba_cardinal (glbl_source [i]);
#else /* !is_sdag */
      tnb += 1;
#endif /* !is_sdag */

#if EBUG2
    sprintf (str, "\tRFSA[t=%i/c=%i/ic=%i] (%s)",
	     tnb,
	     fsaG->fsa_valid_prod_set ? sxba_cardinal (fsaG->fsa_valid_prod_set) : 0,
	     fsaG->instantiated_prod_nb,
	     ret_val ? "true" : "false");
#else
    sprintf (str, "\tRFSA[t=%i] (%s)",
	     tnb,
	     ret_val ? "true" : "false");
#endif

    sxtime (TIME_FINAL, str);
  }
    
  (*(fsaG->final_fsa)) ();

  return ret_val;
}


#if is_lfsa
/* On force la propriete lb <= ub */
/* On remplit lex_compatible_item_sets */

static void lub_washer ()
{
  SXINT           pid, prod, lb, bot, l;
  SXBA		valid_prod_set, *prod2lb, *prod2ub, lb_set;

  for (pid = 0; pid < PID; pid++) {
    valid_prod_set = rfsaGs [pid]->fsa_valid_prod_set;
    prod2lb = rfsaGs [pid]->prod2lub;
    prod2ub = lfsaGs [pid]->prod2lub;

    prod = 0;

    while ((prod = sxba_scan (valid_prod_set, prod)) > 0) {
      bot = prolon [prod];
      l = prolon [prod+1]-bot;
      lb_set = prod2lb [prod];

      lb = sxba_1_rlscan (prod2ub [prod], n+1)+1; /* max lb */
      /* Tous les index de
	 lb_set doivent etre inferieurs ou egaux a ub
	 ub_set doivent etre superieurs ou egaux a lb */

      while ((lb = sxba_1_rlscan (lb_set, lb)) >= 0) {
	/* prod commence a l'indice lb */
#if has_Lex
	/* On assure la compatibilite des locaux avec les Lex */
	if (SXBA_bit_is_set (Lex_prod_item_set, bot))
#endif /* has_Lex */
	  sxba_range (lex_compatible_item_sets [lb], bot, l);
      }
    }
  }
}
#else /* !is_lfsa */
/* On remplit lex_compatible_item_sets */

static void lub_washer ()
{
  SXINT           pid, prod, lb, bot, l;
  SXBA		valid_prod_set, *prod2lb, lb_set;

  for (pid = 0; pid < PID; pid++) {
    valid_prod_set = rfsaGs [pid]->fsa_valid_prod_set;
    prod2lb = rfsaGs [pid]->prod2lub;

    prod = 0;

    while ((prod = sxba_scan (valid_prod_set, prod)) > 0) {
      bot = prolon [prod];
      l = prolon [prod+1]-bot;
      lb_set = prod2lb [prod];

      lb = -1;

      while ((lb = sxba_scan (lb_set, lb)) >= 0) {
	/* prod commence a l'indice lb */
#if has_Lex
	/* On assure la compatibilite des locaux avec les Lex */
	if (SXBA_bit_is_set (Lex_prod_item_set, bot))
#endif /* has_Lex */
	  sxba_range (lex_compatible_item_sets [lb], bot, l);
      }
    }
  }
}
#endif /* !is_lfsa */
#else /* !is_rfsa */
#if is_dynam_lex
static SXBA cur_titem_set, nt_set, valid_suffix_item_set;
static SXINT *nt_stack;

/* Traitement de droite a gauche */
/* item = A -> \alpha . X \beta */

static void dynam_lex_valid_suffix_item_set (SXINT item)
{
  SXINT bot, X, prod, A;

  SXBA_1_bit (valid_suffix_item_set, item);
  bot = prolon [prod = prolis [item]];

#if is_epsilon
  while (--item >= bot) {
    X = lispro [item];

    if (X < 0) {
      SXBA_1_bit (cur_titem_set, item);
      break;
    }

    if (!SXBA_bit_is_set (BVIDE, X))
      break;

    SXBA_1_bit (valid_suffix_item_set, item);
  }

  if (item >= bot)
    return;
#else /* !is_epsilon */
  if (item > bot) {
    if ((X = lispro [--item]) < 0)
      SXBA_1_bit (cur_titem_set, item);

    return;
  }
#endif /* !is_epsilon */

  /* On a reconnu prod */
  A = lhs [prod];

  if (!SXBA_bit_is_set (ilex_compatible_item_set, bot))
    sxba_range (ilex_compatible_item_set, bot, prolon [prod+1]-bot);

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    PUSH (nt_stack, A);
  }
}

#if is_lrdynam_lex
static SXBA valid_prefix_item_set;
/* Traitement de gauche a droite */
/* item = A -> \alpha X . \beta */

static void dynam_lex_valid_prefix_item_set (SXINT item)
{
  SXINT X, prod, A, bot;

  SXBA_1_bit (valid_prefix_item_set, item);
  X = lispro [item];

#if is_epsilon
  while (X != 0) {
    if (X < 0) {
      SXBA_1_bit (cur_titem_set, item);
      return;
    }

    if (!SXBA_bit_is_set (BVIDE, X))
      return;
    
    item++;
    SXBA_1_bit (valid_prefix_item_set, item);
    X = lispro [item];
  }
#else /* !is_epsilon */
  if (X != 0) {
    if (X < 0)
      SXBA_1_bit (cur_titem_set, item);

    return;
  }
#endif /* !is_epsilon */

  /* On a reconnu prod */
  prod = prolis [item];
  A = lhs [prod];
  bot = prolon [prod];

  if (!SXBA_bit_is_set (ilex_compatible_item_set, bot))
    sxba_range (ilex_compatible_item_set, bot, prolon [prod+1]-bot);

  if (SXBA_bit_is_reset_set (nt_set, A)) {
    PUSH (nt_stack, A);
  }
}
#endif /* is_lrdynam_lex */


/* valeurs initiales ds les tables */
/* valid_suffixes = {A -> \alpha . \beta | \beta =>* \epsilon} */
/* suffix_titem_set = {A -> \alpha . t \beta | \beta =>* \epsilon} */
/* empty_prod_item_set = {A -> \alpha . \beta | \alpha \beta =>* \epsilon} */

static void dynam_lexicalizer (SXBA *lb2prod_item_set)
{
  SXINT t, item, item_1, X, i, A, bot, top, prod; 
  SXBA supertagger_item_set, glbl_source_i, source2valid_item_set; 
  SXBA_CST_ALLOC (w_set2, itemmax+1);
  SXBA_CST_ALLOC (w_set1, itemmax+1); 
#if is_lrdynam_lex || is_lex2
  SXBA *ub2prod_item_set;
#endif /* is_lrdynam_lex || is_lex2 */

#if is_lrdynam_lex && is_lex2
  SXBA *lb2item_set;
#endif /* is_lrdynam_lex && is_lex2 */

  SXINT nt;
  SXINT store_npd [ntmax+1];

  SXBA_CST_ALLOC (w_set4, itemmax+1);
  SXBA_CST_ALLOC (w_set3, ntmax+1);
  SXINT w_stack [ntmax+1];
  sxba_empty (nt_set = &(w_set3 [0]));
  nt_stack = &(w_stack [0]), nt_stack [0] = 0;

  sxba_empty (cur_titem_set = &(w_set2 [0]));

#if is_lrdynam_lex || is_lex2
  ub2prod_item_set = sxbm_calloc (n+1, itemmax+1);
#endif /* is_lrdynam_lex || is_lex2 */
  
  /* Pour remettre en etat */
  for (nt = 1; nt <= ntmax; nt++)
    store_npd [nt] = npd [nt];

  /* On calcule les items valides du fait du source */
#if is_lrdynam_lex
  /* La passe gauche/droite est faite par un automate */

  /* valid_prefixes = {A -> \alpha . \beta | \alpha =>* \epsilon} */
  /* prefix_titem_set = {A -> \alpha . t \beta | \alpha =>* \epsilon} */
  /* prefix_titem_set = valid_prefixes \cap titem_set */
#if is_lex2
  lb2item_set = sxbm_calloc (n+1, itemmax+1);
  lb_lexicalizer (lb2item_set);
  source2valid_item_set = lb2item_set [0];
  sxba_empty (valid_prefix_item_set = &(w_set1 [0]));
#else /* !is_lex2 */
  sxba_empty (source2valid_item_set = &(w_set4 [0]));
  basic_lexicalizer (source2valid_item_set);
  sxba_and (prefix_titem_set, source2valid_item_set);
  sxba_and (valid_prefixes, source2valid_item_set);
  valid_prefix_item_set = &(valid_prefixes [0]);
#endif /* !is_lex2 */
  /* On fabrique les npd/numpd en tenant compte de source2valid_item_set */
  item = -1;

  while ((item = sxba_scan (source2valid_item_set, item)) >= 0) {
    if ((X = lispro [item]) > 0) {
      numpd [--npd [X]] = item;
    }
  }

  /* On prepare le nouveau source2valid_item_set */
  source2valid_item_set = &(w_set4 [0]);

#if is_epsilon
  sxba_copy (ub2prod_item_set [0], empty_prod_item_set);
  sxba_copy (source2valid_item_set, empty_prod_item_set);
#else /* !is_epsilon */
  sxba_empty (source2valid_item_set);
#endif /* !is_epsilon */

  for (i = 1; i <= n; i++) {
    ilex_compatible_item_set = ub2prod_item_set [i]; /* vide */
#if is_epsilon
    sxba_copy (ilex_compatible_item_set, empty_prod_item_set);
#endif /* is_epsilon */

#if is_lex2
    sxba_or_and (valid_prefix_item_set, valid_prefixes, lb2item_set [i-1]);
    sxba_or_and (cur_titem_set, prefix_titem_set, lb2item_set [i-1]);
#else /* !is_lex2 */
    sxba_or (cur_titem_set, prefix_titem_set);
#endif /* !is_lex2 */

#if is_sdag
    glbl_source_i = glbl_source [i];
#endif /* is_sdag */
    /* Il faut faire un scan droite/gauche a cause de dynam_lex_valid_prefix_item_set
       qui peut positionner cur_titem_set */
    item = itemmax;

    while ((item = sxba_1_rlscan (cur_titem_set, item)) >= 0) {
      SXBA_0_bit (cur_titem_set, item);
      t = -lispro [item];

      if (
#if is_sdag
	  SXBA_bit_is_set (glbl_source_i, t)
#else /* !is_sdag */
	  glbl_source [i] == t
#endif /* !is_sdag */
	) {
	/* Bon terminal */
	dynam_lex_valid_prefix_item_set (item+1);
      }
    }

    if (!IS_EMPTY (nt_stack)) {
      do {
	A = POP (nt_stack);

	/* On cherche toutes les occurrences de A en RHS telle que B -> \alpha A . \beta soit ds valid_prefix_item_set */
	bot = npd [A];

	while (item = numpd [bot++]/* B -> \alpha . A  \beta */) {
	  if (SXBA_bit_is_set (valid_prefix_item_set, item)) {
	    /* On recommence, meme si valid_prefix_item_set [item] est deja positionne pour le traitement local */
	    dynam_lex_valid_prefix_item_set (item+1);
	  }
	}
      } while (!IS_EMPTY (nt_stack));

      sxba_empty (nt_set);
    }
    
    sxba_or (source2valid_item_set, ilex_compatible_item_set);
  }

  sxba_empty (cur_titem_set);

  for (nt = 1; nt <= ntmax; nt++)
    npd [nt] = store_npd [nt];

#if is_lex2
  sxbm_free (lb2item_set);
#else /* !is_lex2 */
  sxba_and (suffix_titem_set, source2valid_item_set);
#endif /* !is_lex2 */

#else /* !is_lrdynam_lex */

#if is_lex2
  ub_lexicalizer (ub2prod_item_set);
  source2valid_item_set = ub2prod_item_set [n];
#else /* !is_lex2 */
  basic_lexicalizer (source2valid_item_set = &(w_set4 [0]));
#endif /* !is_lex2 */
#endif /* !is_lrdynam_lex */

#if !(is_lex2 || is_lrdynam_lex)
  sxba_and (suffix_titem_set, source2valid_item_set);
#endif /* !is_lex2 */

  sxba_and (valid_suffixes, source2valid_item_set);
  valid_suffix_item_set = &(valid_suffixes [0]);

  /* On fabrique/complete les npd/numpd en tenant compte de source2valid_item_set */
  item = -1;

  while ((item = sxba_scan (source2valid_item_set, item)) >= 0) {
    if ((X = lispro [item]) > 0) {
      numpd [--npd [X]] = item;
    }
  }

  /* On part des items reduce atteints par la passe gauche-droite eventuelle et connectes a S' -> $ S . $ */
  /*
  sxba_copy (valid_suffix_item_set, nt2rc_item_set [1]);
  sxba_and (valid_suffix_item_set, source2valid_item_set);
  */


#if is_epsilon
  sxba_copy (lex_compatible_item_sets [n], empty_prod_item_set);
#endif /* is_epsilon */

  for (i = n; i > 0; i--) {
    ilex_compatible_item_set = lb2prod_item_set [i-1]; /* vide */
#if is_epsilon
    sxba_copy (ilex_compatible_item_set, empty_prod_item_set);
#endif /* is_epsilon */

#if is_lrdynam_lex || is_lex2
    /* sxba_or_and (valid_suffix_item_set, valid_suffixes, ub2item_set [i]); */
    sxba_or_and (cur_titem_set, suffix_titem_set, ub2prod_item_set [i]);
#else /* !(is_lrdynam_lex || is_lex2) */
    /* Si A -> \alpha B. \beta, a ete trouve' le coup d'avant, il faut ajouter les
       terminaux du coin droit de B.  Ici, on ajoute le sur-ensemble suffix_titem_set */
    sxba_or (cur_titem_set, suffix_titem_set);
#endif /* !(is_lrdynam_lex || is_lex2) */

#if is_sdag
    glbl_source_i = glbl_source [i];
#endif /* is_sdag */
    item = -1;
      
    while ((item = sxba_scan_reset (cur_titem_set, item)) >= 0) {
      t = -lispro [item];

      if (
#if is_sdag
	  SXBA_bit_is_set (glbl_source_i, t)
#else /* !is_sdag */
	  glbl_source [i] == t
#endif /* !is_sdag */
	) {
	/* Bon terminal */
	dynam_lex_valid_suffix_item_set (item);
      }
    }

    if (!IS_EMPTY (nt_stack)) {
      do {
	A = POP (nt_stack);

	/* On cherche toutes les occurrences de A en RHS telle que B -> \alpha A . \beta soit ds valid_suffix_item_set */
	bot = npd [A];

	while (item = numpd [bot++]/* B -> \alpha . A  \beta */) {
	  item_1 = item+1;

	  if (SXBA_bit_is_set (valid_suffix_item_set, item_1)) {
	    /* On recommence, meme si valid_suffix_item_set [item] est deja positionne pour le traitement local */
	    dynam_lex_valid_suffix_item_set (item);
	  }
	}
      } while (!IS_EMPTY (nt_stack));

      sxba_empty (nt_set);
    }
  }

#if is_lrdynam_lex || is_lex2
  sxbm_free (ub2prod_item_set);
#endif /* is_lrdynam_lex || is_lex2 */

  /* Prudence... */
  for (nt = 1; nt <= ntmax; nt++)
    npd [nt] = store_npd [nt];
}
#endif /* is_dynam_lex */
#endif /* !is_rfsa */

/* --------------------------------------------------------------------------------------------------- */
/* --------------------------- DEBUT POUBELLE -------------------------------------------------------- */
#define is_check_generate 1
#if is_check_generate
/* On supprime des prod qui generent des chaines qui n'ont pas une longueur compatible avec le source */
/* Ca marche (surtout si epsilon-free), mais pas retenu pour l'instant */

static SXINT  *proto_stack;
static SXBA valid_proto_prod_set;


/* On suppose qu'on a spf.insideG.nt2min_gen_lgth */

static void generate (SXINT min_proto, SXINT lgth)
{
  /* min_proto est la taille min generee par le fond de la pile */
  /* et lgth est la taille min qu'il reste a generer */
  /* On doit avoir min_proto <= lgth */
  SXINT X, prod, bot, top, item, base_item, nb, min_gen_lgth;

  if (IS_EMPTY (proto_stack))
    return;

  X = DPOP (proto_stack);

  if (X < 0) {
    /* t */
    generate (min_proto-1, lgth-1);
    return;
  }

  /* nt */
  min_proto -= spf.insideG.nt2min_gen_lgth [X];
  bot = NPG (X);
  top = NPG (X+1);

  while (bot < top) {
    prod = NUMPG (bot);

    if (!SXBA_bit_is_set (valid_proto_prod_set, prod)) {
      item = base_item = PROLON (prod);
      nb = 0;
      min_gen_lgth = 0;

      while ((X = LISPRO (item)) != 0) {
	if (X < 0)
	  min_gen_lgth++;
	else
	  min_gen_lgth += spf.insideG.nt2min_gen_lgth [X];

	item++;
	nb++;
      }

      if (min_proto+min_gen_lgth <= lgth) {
	SXBA_1_bit (valid_proto_prod_set, prod);

	if (nb > 0) {
	  /* On peut empiler */
	  DCHECK (proto_stack, nb);
	  item = base_item;

	  while ((X = LISPRO (item)) != 0) {
	    DSPUSH (proto_stack, X);
	    item++;
	  }

	  generate (min_proto+min_gen_lgth, lgth);
	}
      }
    }

    bot++;
  }
}

static bool check_generate ()
{
#if LOG
  fputs ("****** Entering check_generate () ******\n", stdout);
#endif /* LOG */
  spf.insideG.nt2min_gen_lgth = (SXINT*) sxcalloc (MAXNT+1, sizeof (SXINT));

  call_fill_nt2min_gen_lgth ();

  DALLOC_STACK (proto_stack, 100);
  valid_proto_prod_set = sxba_calloc (MAXPROD+1);

  DSPUSH (proto_stack, 1); /* axiome */
  generate (spf.insideG.nt2min_gen_lgth [1], n-1);

#    if MEASURES || LOG
  printf ("%sThe check_generate test validates %i productions.\n",
#      if MEASURES
	  "MEASURES: "
#      else /* LOG */
	  ""
#      endif /* LOG */
	  , sxba_cardinal (valid_proto_prod_set));
#    endif /* MEASURES || LOG */

  sxfree (valid_proto_prod_set), valid_proto_prod_set = NULL;
  DFREE_STACK (proto_stack);

#if LOG
  fputs ("****** Leaving check_generate () ******\n", stdout);
#endif /* LOG */

  return true; /* On sera + fin +tard */
}
    
if (is_axiom_kept && (is_axiom_kept = check_generate ())) {
}
#endif /* is_check_generate */

/* *************************************************************************************************
                                  C Y C L E   E L I M I N A T I O N
   ************************************************************************************************* */
/* Ne marche pas (voir commentaire devant cycle_check () */
/* On ne peut donc pas rendre une grammaire cycle-free en se contentant de supprimer des prods !! */
/* On ne lance donc cycle_elimination () que si la grammaire est epsilon-free */

/* Dans ce cas soit C un cycle et P_c les productions (simples) de ce cycle.
   Soit E un point d'entree de ce cycle (\exists A -> \alpha E \beta \not\in P_c et S =>* A)
   On supprime toutes les prod de P_c de la forme "U -> E" qui atteignent le point d'entre'e E.
*/ 
/* Cette procedure transforme la grammaire cyclique prod_hd en une grammaire equivalente sans cycle */
/* repr_set contient les representants (nt) des classes d'equivalence de la relation cycle */
/* Si A est un representant, la classe d'equivalence de representant A est ds cyclic_sets [A] */
static SXINT  pA_nb;
static SXINT  *pA2A, *A2pA, *simple_prod_stack, *pA2min_pA;
static SXBA *cyclic_sets;

static void cycle_elimination ()
{
  SXINT           pA, A, bot, top, prod, bot_item, item, X, pX, repr;
  SXINT           *nt2equiv,*nt_stack;
  SXBA          line, repr_set, nt_set;

#if LOG
  fputs ("****** Entering cycle_elimination () ******\n", stdout);
#endif /* LOG */

  /* On recherche un point d'entree principal par cycle.
     Ce sera le representant de ce cycle */
  repr_set = sxba_calloc (pA_nb+1);
  nt2equiv = (SXINT*) sxcalloc (pA_nb+1, sizeof (SXINT));

  if (SXBA_bit_is_set (cyclic_sets [0], 1)) {
    /* Cycle autour de l'axiome, c'est lui le representant */
    SXBA_1_bit (repr_set, 1);
    line = cyclic_sets [1];
    pA = 0;

    while ((pA = sxba_scan (line, pA)) > 0) {
      nt2equiv [pA] = 1; /* axiome */
    }
  }

  nt_stack = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), nt_stack [0] = 0;
  nt_set = sxba_calloc (MAXNT+1);

  /* On regarde les nt accessibles depuis l'axiome */
  /* ... et on cherche les points d'entree principaux */
  PUSH (nt_stack, 1);
  SXBA_1_bit (nt_set, 1);

  while (!IS_EMPTY (nt_stack)) {
    A = POP (nt_stack);
    bot = NPG (A);
    top = NPG (A+1)-1;
   
    while (bot < top) {
      prod = NUMPD (bot);
      bot++;
      bot_item = PROLON (prod);

      if (SXBA_bit_is_set (basic_item_set, bot_item)) {
	item = bot_item;
	
	while ((X = LISPRO (item++)) != 0) {
	  if (X > 0) {
	    pX = A2pA [X];

	    if (SXBA_bit_is_set (cyclic_sets [0], pX)) {
	      /* nt cyclique */
	      if (nt2equiv [pX] == 0) {
		/* 1ere fois qu'on le trouve, c'est le representant de son cycle */
		SXBA_1_bit (repr_set, pX);
		line = cyclic_sets [pA2min_pA [pX]/* nt de code min du cycle */];
		pA = 0;

		while ((pA = sxba_scan (line, pA)) > 0) {
		  nt2equiv [pA] = pX;
		}
	      }
	    }

	    if (SXBA_bit_is_reset_set (nt_set, X))
	      PUSH (nt_stack, X);
	  }
	}
      }
    }
  }

  /* Ici repr_set contient les points d'entree principaux */

  /* On examine maintenant les prod simples stockees ds simple_prod_stack */
  while (!IS_EMPTY (simple_prod_stack)) {
    prod = POP (simple_prod_stack);
    A = LHS (prod);
    item = PROLON (prod);
    X = LISPRO (item);
    /* prod A -> X */
    pA = A2pA [A];
    pX = A2pA [X];

    if ((repr = nt2equiv [pA]) == nt2equiv [pX] /* elle est dans un cycle de representant repr ... */
	&& repr == X /* ... et sa RHS est l'entree de ce cycle => on supprime donc prod de basic */) {
	sxba_clear_range (basic_item_set, item, PROLON (prod+1)-item);
#if LOG
	fputs ("This cyclic production is erased: ", stdout);
	output_prod (PROD (prod));
#endif /* LOG */
    }
  }

  sxfree (nt2equiv);
  sxfree (repr_set);
  sxfree (nt_stack);
  sxfree (nt_set);

#if LOG
  fputs ("****** Leaving cycle_elimination () ******\n", stdout);
#endif /* LOG */
}
/* *************************************************************************************************
                                  C Y C L E   E L I M I N A T I O N  (end)
   ************************************************************************************************* */

/* ----------------------------- FIN POUBELLE -------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------------- */


#endif /* 0 */
