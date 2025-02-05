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



/*
  Analyseur a la CYK qui accepte en entree 
    - les tables LC (comme earley) d'une CFG generale
    - un source qui peut etre sous forme lineaire, de SDAG ou de DAG (traite par dag_scanner)
      Ce source peut etre utilise par un pre-traitement de lexicalisation dont l'effet est de
      ne considerer qu'une sous-grammaire, partie de la CFG initiale "concernee" par ce source.
      G est cette grammaire (filtree ou non).


  Si la CFG n'a pas de production vide
    - pour chaque mlstn i on a Tii = {A -> . \alpha | A -> \alpha \in G}
    - pour chaque range ij de taille |ij| = 1, 2, ..., n on calcule Tij
      Tij = {A -> \alpha . \beta | A -> \alpha \beta \in G, \alpha =>* x,
                                   x \in T^+, i =>x j}
      ou i =>x j signifie qu'il existe ds le source un chemin qui s'epelle x
      entre les etats i et j

      Tij (initialement vide) se calcule comme suit
      pour tout k t.q. i<=k<j, on calcule Tij a partir de Tik et de Tkj
      
      Operation tshift
      S'il existe ds le source entre k et j une transition sur le terminal t (k =>t j)
         et s'il y a ds Tik un item de la forme A -> \alpha . t \beta, alors
	 on ajoute A -> \alpha t . \beta a Tij

      Operation reduce_shift
      Si B -> \beta . \in Tkj et A -> \alpha .B \delta \in Tik, alors
         on ajoute A -> \alpha B . \delta a Tij

      +traitement des (chaines de) productions unitaires
      Une chaine de productions unitaires est un ensemble de productions de la forme
      A_0 -> A_1, A_1 -> A_2, ..., A_p-1 -> A_p
      Si on {C -> . A_0 \alpha, A_0 -> . A_1, ..., A_p-1 -> . A_p} \include Tii et si
      A_p -> \beta . \in Tij alors, l'operation reduce_shift precedente a ajoute
      A_p-1 -> A_p . a Tij.  Il faut maintenant simuler la reduction A_p-1 -> A_p
      qui va ajouter A_p-2 -> A_p-1 . a Tij, ..., puis finalement C -> A_0 . \alpha
      a Tij.  On obtient ce traitement en appelent reduce_shift sur Tii, Tij tant
      que cette operation modifie Tij


  Si la CFG a des productions vides (is_epsilon est positionne)
    - pour chaque mlstn i on a Tii = {A -> \alpha . \beta | A -> \alpha \beta \in G et \alpha =>* \varepsilon}
    - pour chaque range ij de taille |ij| = 1, 2, ..., n on calcule Tij
      Tij = {A -> \alpha . \beta | A -> \alpha \beta \in G, \alpha =>* x,
                                   x \in T^+, i =>x j}
      ou i =>x j signifie qu'il existe ds le source un chemin qui s'epelle x
      entre les etats i et j

      Tij (initialement vide) se calcule comme suit
      pour tout k t.q. i<=k<j, on calcule Tij a partir de Tik et de Tkj
      
      Operation tshift
      S'il existe ds le source entre k et j une transition sur le terminal t (k =>t j)
         et s'il y a ds Tik un item de la forme A -> \alpha . t \beta, alors
	 on ajoute A -> \alpha t . \beta a Tij

      Operation reduce_shift
      Si on a un item B-reduce dans Tkj (B -> \beta . \gamma \in Tkj, \gamma =>* \varepsilon)
         et si on a un B-item ds Tik (A -> \alpha .B \delta \in Tik), alors
         on ajoute A -> \alpha B \delta1 . \delta2 a Tij pour \delta1 \delta2 == \delta
	 et \delta1 =>* \varepsilon

      +traitement des (chaines de) productions unitaires
      Ds le cas d'une grammaire a productions vides, Une chaine de productions unitaires
      est un ensemble de productions de la forme
      A_0 -> \pi_1 A_1 \sigma_1, A_1 -> \pi_2 A_2 \sigma_2, ..., A_p-1 -> \pi_p A_p \sigma_p
      ou` les \pi_i et \sigma_i peuvent deriver ds le vide
      Si on {C -> \pi_0. A0 \alpha, A0 -> \pi_1 . A1 \sigma_1, ..., Ap-1 -> \pi_p . Ap \sigma_p} \include Tii et si
      A_p -> \beta . \gamma \in Tij alors, l'operation reduce_shift precedente a ajoute
      Ap-1 -> \pi_p A_p . \sigma_p a Tij.  Il faut maintenant simuler la reduction Ap-1 -> \pi_p A_p \sigma_p
      qui va ajouter Ap-2 -> \pi_p-1 A_p-1 . \sigma_p-1 a Tij, ..., puis finalement C -> \pi_0 A0 . \alpha
      a Tij.  On obtient ce traitement en appelent reduce_shift sur Tii, Tij tant
      que cette operation modifie Tij
*/


#include "sxversion.h"
#include "sxunix.h"

char WHAT_CYKPARSER[] = "@(#)SYNTAX - $Id: CYK_parser.c 4148 2024-08-02 10:43:56Z garavel $" WHAT_DEBUG;

static char	ME [] = "CYK_parser";



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
#define def_lispro
#define def_prolis
#define def_prolon
#define def_lhs
#define def_npd
#define def_numpd
#define def_npg
#define def_numpg
#define def_ntstring
#define def_tstring
#define def_multiple_t_item_set
#define def_valid_prefixes
#define def_valid_suffixes
#define def_BVIDE
#define def_titem_set
#if is_parser
#define def_empty_prod_item_set
#endif /* is_parser */

#include LC_TABLES_H
#endif /* LC_TABLES_H */

#ifdef MAKE_INSIDEG
/* Compilation de CYK_parser avec l'option -DMAKE_INSIDEG
   La grammaire peut etre huge ou pas, mais elle sera traitee comme si elle etait huge */
/* Ds ce cas, on force is_lex */
#  ifndef is_lex
#  undef is_lex
#  endif /* is_lex */
#else /* !MAKE_INSIDEG */
/* Si la grammaire est "huge", on force la fabrication d'unr nelle grammaire filtree par le source */
#  ifdef HUGE_CFG
#    ifndef MAKE_INSIDEG
#    define MAKE_INSIDEG
#    endif /* MAKE_INSIDEG */
#  endif /* HUGE_CFG */
#endif /* !MAKE_INSIDEG */


/* Pour avoir la definition et l'initialisation de spf faite ici par sxspf.h */
#define SX_DFN_EXT_VAR2
#include "earley.h"

/* Attention, outputG (Aij, Tpq, ...) doivent etre calcules en ft des inputG (et non de insideG qui disparait
   apres la construction de outputG */
/* Pour acceder aux valeurs initiales des tables statiques */
#define inputG_SXEOF       (-tmax)
#define inputG_MAXITEM     itemmax
#define inputG_MAXNT       ntmax
#define inputG_MAXPROD     prodmax
#define inputG_MAXRHSLGTH  rhs_lgth
#define inputG_tstring     tstring
#define inputG_ntstring    ntstring

/* Pour acceder soit a insideG soit aux tables statiques */
#ifdef MAKE_INSIDEG

#include "sxspf.h"

/* A VOIR : Si la lexicalization de base a echouee (rare !!), pour que ca marche il faut un truc de la forme
   (pareil ds le lexicalizer_mngr)
#define SXEOF             spf.insideG.is_allocated ? (-spf.insideG.maxt) : (-tmax)
...
#define LISPRO(i)       spf.insideG.is_allocated ? spf.insideG.lispro [i] : lispro [i]
*/

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

#else /* !MAKE_INSIDEG */

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

#define LISPRO(i)       lispro [i]
#define PROLIS(i)       prolis [i]
#define PROLON(i)       prolon [i]
#define LHS(i)          lhs [i]
#define NPG(i)          npg [i]
#define NUMPG(i)        numpg [i]
#define NPD(i)          npd [i]
#define NUMPD(i)        numpd [i]
#define TPD(i)          tpd [i]
#define TNUMPD(i)       tnumpd [i]
#define NTSTRING(i)     ntstring [i]
#define TSTRING(i)      tstring [i]

#define RHS_NT2WHERE(i) rhs_nt2where [i]
#define LHS_NT2WHERE(i) lhs_nt2where [i]
             
#define	EMPTY_SET           BVIDE
#define	TITEM_SET           titem_set
#define	RC_TITEM_SET        rc_titem_set
#define	EMPTY_PROD_ITEM_SET empty_prod_item_set
#define	LEFT_CORNER(i)      left_corner [i]
#define	NT2ITEM_SET(i)      nt2item_set [i]
#define	RIGHT_CORNER(i)     right_corner [i]
#define	NT2RC_ITEM_SET(i)   nt2rc_item_set [i]
#define	VALID_PREFIXES      valid_prefixes
#define	VALID_SUFFIXES      valid_suffixes
#define	T2PROD_ITEM_SET(i)  t2prod_item_set [i]
#define	T2ITEM_SET(i)       t2item_set [i]
#define	NT2MIN_GEN_LGTH(i)  nt2min_gen_lgth [i]

#endif /* !MAKE_INSIDEG */

#include "sxstack.h"
#include "XxY.h"
#include "XxYxZ.h"


#if is_dag
#include "udag_scanner.h"
#endif /* is_dag */


static SXBA         working_item_set, working_Tij_set_1, working_Tij_set_2, working_nt_set;
static SXINT          *working_nt_stack;
static SXBA         *CYK_ij2t_set, *CYK_Tij;
static SXINT          **CYK_ij, *CYK_ij_area;


#if is_parser
/* Structures et variables utilisees par l'analyseur (pas le reconnaisseur) CYK */
static SXINT          CYK_item_hd_foreach [6] =  {0, 0, 0, 1, 0, 0} /* XYforeach */,
                    CYK_reduce_item_hd_foreach [6] =  {0, 0, 0, 0, 1, 0} /* XZforeach */;
static SXINT          spf_prod_nb;
static SXINT          *CYK_npd, *nt2reduce_item_set_id, *CYK_numpd;
static SXINT          *CYK_rhs_stack; /* DSTACK */
static XxYxZ_header CYK_item_hd;
static XxYxZ_header CYK_reduce_item_hd;


#if is_epsilon
static void spf_empty_nt_in_spf (SXINT A, SXINT i);

static void
put_empty_prod_in_spf (SXINT prod, SXINT i)
{
  SXINT     bot, top, lgth, Aii, X, Xii;
  bool is_new_Xii;

  bot = prolon [prod];
  top = prolon [prod+1]-1;
  lgth = top-bot;
  Aii = DTOP (CYK_rhs_stack);

  while (--top >= bot) {
    X = lispro [top];

    is_new_Xii = !MAKE_Aij (Xii, X, i, i);
    DPUSH (CYK_rhs_stack, Xii);

    if (is_new_Xii)
      spf_empty_nt_in_spf (X, i);
  }
    
  DPUSH (CYK_rhs_stack, prod);
  spf_put_an_iprod (CYK_rhs_stack);

  if (is_print_prod)
    spf_print_iprod (stdout, spf.outputG.maxprod, "", "\n");

  DTOP (CYK_rhs_stack) -= lgth+1;
}

/* On genere maintenant les productions vides */
static void
spf_empty_nt_in_spf (SXINT A, SXINT i)
{
  SXINT bot, top, prod, item;

  bot = npg [A];
  top = npg [A+1];

  while (bot < top) {
    prod = numpg [bot];
    item = prolon [prod];

    /* EMPTY_PROD_ITEM_SET = {A -> \alpha . \beta | \alpha \beta =>* \epsilon} */
    if (SXBA_bit_is_set (empty_prod_item_set, item)) {
      /* trouvee */
      put_empty_prod_in_spf (prod, i);
    }

    bot++;
  }
}
#endif /* is_epsilon */


/* item = A -> [i] \alpha [j] . \beta */
static void
spf_td_extraction (SXINT i, SXINT j, SXINT item)
{
  SXINT     ij, reduce_item, X, prod, triple, triple2, k, kj, Aij; 
  bool is_new_Aij;

  /* On s'occupe du prefixe */
  X = LISPRO (item-1);

#ifdef MAKE_INSIDEG
  if (X > 0)
    X = spf.insideG.nt2init_nt [X];
  else {
    if (X < 0)
      X = -spf.insideG.t2init_t [-X];
  }
#endif /* ifdef MAKE_INSIDEG */

  if (X == 0) {
    /* reconnue en entier */
    prod = PROLIS (item);
#ifdef MAKE_INSIDEG
    prod = spf.insideG.prod2init_prod [prod];
#endif /* ifdef MAKE_INSIDEG */
    DPUSH (CYK_rhs_stack, prod);
    spf_put_an_iprod (CYK_rhs_stack);

    if (is_print_prod)
      spf_print_iprod (stdout, spf.outputG.maxprod, "", "\n");

    DPOP (CYK_rhs_stack);
  }
  else {
#if is_epsilon
    if (i == j) {
      /* On est ds le cas (particulier) i == j et \alpha =>+ \varepsilon */
#if EBUG
      if (X < 0)
	sxtrap (ME, "spf_td_extraction");
#endif /* EBUG */  
	
      is_new_Aij = !MAKE_Aij (Aij, X, j, j);
      DPUSH (CYK_rhs_stack, Aij);   

      if (is_new_Aij)
	spf_empty_nt_in_spf (X, j);

      spf_td_extraction (i, j, item-1);
      DPOP (CYK_rhs_stack); 
    }
    else {
#endif /* is_epsilon */
      ij = CYK_ij [i] [j];

      XxYxZ_XYforeach (CYK_item_hd, item, ij, triple) {
	k = XxYxZ_Z (CYK_item_hd, triple);

	if (X < 0) {
	  /* terminal entre k et j */
	  MAKE_Tij (Aij, X, k, j);
	  DPUSH (CYK_rhs_stack, -Aij);
	}
	else {
	  /* nt entre k et j ... */
	  /* on met la lhs */
	  is_new_Aij = !MAKE_Aij (Aij, X, k, j);
	  DPUSH (CYK_rhs_stack, Aij);

	  if (is_new_Aij) {
	    /* ... dont les definitions sont donnees par */
#if is_epsilon
	    if (k == j) {
	      spf_empty_nt_in_spf (X, j);
	    }
	    else {
#endif /* is_epsilon */
	      kj = CYK_ij [k] [j];

	      XxYxZ_XZforeach (CYK_reduce_item_hd, X, kj, triple2) {
		reduce_item = XxYxZ_Y (CYK_reduce_item_hd, triple2);
		/* reduce_item ==  <S> -> \alpha .  */
		spf_td_extraction (k, j, reduce_item);
	      }
#if is_epsilon
	    }
#endif /* is_epsilon */
	  }
	}

	spf_td_extraction (i, k, item-1);
	DPOP (CYK_rhs_stack);
      }
#if is_epsilon
    }
#endif /* is_epsilon */

  }
}



/* L'analyse est terminee, on extrait la spf */
/* ij est le code ds CYK_ij du range qui couvre le source */
static void
CYK_extract_spf ()
{
  SXINT triple, reduce_item, ij;

  /* On fabrique l'axiome instancie', il doit avoir le code 1 */
  MAKE_Aij (spf.outputG.start_symbol, 1 /* axiome */, init_mlstn, final_mlstn);
  /* On met la lhs instanciee  */
  DPUSH (CYK_rhs_stack, spf.outputG.start_symbol);

#if is_epsilon
  /* verrue si le source est vide */
  if (init_mlstn == final_mlstn) {
    spf_empty_nt_in_spf (spf.outputG.start_symbol, init_mlstn);
  }
  else {
#endif /* is_epsilon */
    ij = CYK_ij [init_mlstn] [final_mlstn];

    XxYxZ_XZforeach (CYK_reduce_item_hd, spf.outputG.start_symbol, ij, triple) {
      reduce_item = XxYxZ_Y (CYK_reduce_item_hd, triple);
      /* reduce_item ==  <S> -> \alpha .  */
      spf_td_extraction (init_mlstn, final_mlstn, reduce_item);
    }
#if is_epsilon
  }
#endif /* is_epsilon */

  DPOP (CYK_rhs_stack);
}
#endif /* is_parser */

#if LOG
static void
print_xitem (SXINT item, SXINT i, SXINT j, SXINT k, char *prefix, char *suffix)
{
  SXINT top_item, bot_item, X, prod;

  prod = PROLIS (item);
#ifdef MAKE_INSIDEG
  prod = spf.insideG.prod2init_prod [prod];
#endif /* MAKE_INSIDEG */
  printf ("%s%i:<%s>\t= [%i] ", prefix, prod, ntstring [lhs [prod]], i);

  top_item = prolon [prod+1]-1;
  bot_item = prolon [prod];

  while (bot_item < top_item) {
    if (bot_item+1 == item)
      printf ("[%i] ", k);

    if (bot_item == item) {
      printf ("[%i] . ", j);
    }

    X = lispro [bot_item++];

    if (X < 0)
      printf ("\"%s\" ", tstring [-X]);
    else
      printf ("<%s> ", ntstring [X]);
  }

  if (bot_item == item) {
    printf ("[%i] . ", j);
  }

  printf (";%s", suffix);
}


static void
print_item (SXINT item, char *prefix, char *suffix)
{
  SXINT top_item, bot_item, X, prod;

  prod = PROLIS (item);
#ifdef MAKE_INSIDEG
  prod = spf.insideG.prod2init_prod [prod];
#endif /* MAKE_INSIDEG */
  printf ("%s%i:<%s>\t= ", prefix, prod, ntstring [lhs [prod]]);

  top_item = prolon [prod+1]-1;
  bot_item = prolon [prod];

  while (bot_item < top_item) {
    if (bot_item == item)
      fputs (". ", stdout);

    X = LISPRO (bot_item++);

    if (X < 0)
      printf ("\"%s\" ", tstring [-X]);
    else
      printf ("<%s> ", ntstring [X]);
  }
    
  if (bot_item == item)
    fputs (". ", stdout);

  printf (";%s", suffix);
}



static void
print_Tij (SXINT i, SXINT j)
{
  SXINT  ij, item;
  SXBA Tij;

  ij = CYK_ij [i] [j];
  Tij = CYK_Tij [ij];

  printf ("\n\
*********** T[%i][%i] = {%s", i, j, Tij ? "\n" : " NULL ");
  
  if (Tij) {
    item = -1;

    while ((item = sxba_scan (Tij, item)) >= 0)
      print_item (item, "", "\n");
  }

  fputs ("} ***********\n", stdout);
}
#endif /* LOG */


/* On remplit Tii = { A -> \alpha . \beta | A -> \alpha . \beta \in valid_item_set et \alpha =>* epsilon} */
static void
CYK_emptyshift (SXBA Tii, SXBA valid_item_set)
{
  /* VALID_PREFIXES = {A -> \alpha . \beta | \alpha =>* \epsilon} */
  if (valid_item_set)
    sxba_2op (Tii, SXBA_OP_COPY, VALID_PREFIXES, SXBA_OP_AND, valid_item_set);
  else
    sxba_copy (Tii, VALID_PREFIXES);
}

/* S'il y a entre k et j une transition terminale sur t et si ds Tik il y a un item de la forme A -> \alpha .t \beta, alors
   on ajoute A -> \alpha t .\beta a Tij */
/* Retourne 1 si Tij a ete modifie', 3 si on a mis (en +) qqchose ds new_reduce_Tij et 0 sinon */
static SXINT
CYK_tshift (SXBA Tij, SXBA Tik, SXBA kj_t_set, SXBA new_reduce_Tij
#if is_parser || LLOG
	    , SXINT i, SXINT j, SXINT k
#endif /* is_parser || LLOG */
)
{
  SXINT     item, next_item, t, it, next_it, X;
  SXINT     ret_val;
#if is_parser
  SXINT     CYK_item, dummy, ij;
#endif /* is_parser */

#if LLOG
  bool is_new;

  printf ("\n\
*********** CYK_tshift (T[%i][%i] = T[%i][%i] + t[%i][%i]) = {\n", i, j, i, k, k, j);
#endif /* LLOG */

  /* titem_set = { A -> \alpha . t \beta } */
  sxba_2op (working_item_set, SXBA_OP_COPY, Tik, SXBA_OP_AND, TITEM_SET);

  ret_val = 0;
  item = 3; /* S' -> $ S $ . */

  while ((item = sxba_scan (working_item_set, item)) > 0) {
    t = -LISPRO (item);

    if (SXBA_bit_is_set (kj_t_set, t)) {
      next_item = item+1;

#if LLOG
      print_item (next_item, "", " = ");
      print_item (item, "", "");
      printf (" + \"%s\" [%i..%i] ;%s\n", tstring [t], k, j, !SXBA_bit_is_set (Tij, next_item) ? "(new)" : "");
#endif /* LLOG */

      /* On met A -> \alpha t . \beta ds Tij */
      if (SXBA_bit_is_reset_set (Tij, next_item)) {
	ret_val |= 1;

	/* VALID_SUFFIXES = {A -> \alpha . \beta | \beta =>* \epsilon} */
	if (new_reduce_Tij && SXBA_bit_is_set (VALID_SUFFIXES, next_item)) {
	  SXBA_1_bit (new_reduce_Tij, next_item);
	  ret_val = 3;
	}

#if is_epsilon
	it = next_item;
	  
	/* Si \beta = \beta1 \beta2 et si \beta1 =>+ \varepsilon, on y met aussi
	   A -> \alpha B \beta1 . \beta2 */
	while ((X = LISPRO (it)) > 0 && EMPTY_SET && SXBA_bit_is_set (EMPTY_SET, X)) {
	  next_it = it+1;

#if LLOG
	  print_item (next_it, "", " = ");
	  print_item (it, "", "");
#ifdef MAKE_INSIDEG
	  X = spf.insideG.nt2init_nt [X];
#endif /* MAKE_INSIDEG */
	  printf (" + <%s> [%i..%i] ;%s\n", ntstring [X], j, j, !SXBA_bit_is_set (Tij, next_it) ? " (new)" : "");
#endif /* LLOG */

	  if (SXBA_bit_is_reset_set (Tij, next_it)) {
	    ret_val |= 1;

	    /* VALID_SUFFIXES = {A -> \alpha . \beta | \beta =>* \epsilon} */
	    if (new_reduce_Tij && SXBA_bit_is_set (VALID_SUFFIXES, next_it)) {
	      SXBA_1_bit (new_reduce_Tij, next_item);
	      ret_val = 3;
	    }
	  }

	  it++;
	}
#endif /* is_epsilon */
      }

#if is_parser
      ij = CYK_ij [i] [j];

      if (!XxYxZ_set (&CYK_item_hd, next_item, ij, k, &CYK_item)) {
	ret_val |= 1;
#if LLOG
	is_new = true;
#endif /* LLOG */
      }
#if LLOG
      else
	is_new = false;

      print_xitem (next_item, i, j, k, "parse\t", is_new ? " (new)\n" : "\n");
#endif /* LLOG */

#if is_epsilon
	/* Si \beta = \beta1 X \beta2 et si \beta1 X =>+ \varepsilon, tous les items
	   de la forme A -> \alpha B \beta1 X . \beta2 
	   vont referencer le nt instancie' Xjj	*/
	it = next_item;

	while ((X = LISPRO (it++)) > 0 && EMPTY_SET && SXBA_bit_is_set (EMPTY_SET, X)) {
	  if (!XxYxZ_set (&CYK_item_hd, it, ij, j, &CYK_item)) {
	    ret_val |= 1;
#if LLOG
	    is_new = true;
#endif /* LLOG */
	  }
#if LLOG
	  else
	    is_new = false;

	  print_xitem (next_item, i, j, j, "parse\t", is_new ? " (new)\n" : "\n");
#ifdef MAKE_INSIDEG
	  X = spf.insideG.nt2init_nt [X];
#endif /* MAKE_INSIDEG */
	  printf ("\t<%s> [%i..%i] = {\n", ntstring [X], j, j);
#endif /* LLOG */
	}
#endif /* is_epsilon */
#endif /* is_parser */
    }
  }

#if LLOG
  printf ("} %s***********\n", ret_val ? "(new) " : "");
#endif /* LLOG */

  return ret_val;
}

/* S'il y a ds Tkj un item de la forme B -> \alpha .\beta avec \beta =>* \varepsilon et si ds 
   Tik il y a un item de la forme A -> \gamma .B \delta, alors on ajoute
   A -> \gamma B .\delta a Tij */
/* Retourne 1 si Tij a ete modifie', 3 si on a mis (en +) qqchose ds new_reduce_Tij et 0 sinon */
static SXINT
CYK_ntshift (SXBA Tij, SXBA Tik, SXBA Tkj, SXBA new_reduce_Tij
#if is_parser || LLOG
	     , SXINT i, SXINT j, SXINT k
#endif /* is_parser || LLOG */
)
{
  SXINT     item, next_item, prod, B, old_B, it, next_it, X;
  SXINT     ret_val;
  SXBA    local_working_item_set;
#if is_parser
  SXINT     CYK_item, dummy, x, id, reduce_item, CYK_reduce_item, ij;
#endif /* is_parser */
#if LLOG
  SXINT     bot, top;
  bool is_new;

  printf ("\n\
*********** CYK_ntshift (T[%i][%i] = T[%i][%i] + T[%i][%i]) = {\n", i, j, i, k, k, j);
#endif /* LLOG */

  ret_val = 0;

  if (new_reduce_Tij && Tij != Tkj) {
    /* On peut travailler directement ds Tkj qui est en fait un ensemble de travail */
    local_working_item_set = Tkj;
  }
  else {
    sxba_copy (working_item_set, Tkj);
    local_working_item_set = working_item_set;
  }

  /* VALID_SUFFIXES = {A -> \alpha . \beta | \beta =>* \epsilon} */
  if (sxba_2op (local_working_item_set, SXBA_OP_IS, local_working_item_set, SXBA_OP_AND, VALID_SUFFIXES)) {
    item = 0;

    while ((item = sxba_scan_reset (local_working_item_set, item)) > 0) {
      prod = PROLIS (item);
      B = LHS (prod);

      if (SXBA_bit_is_reset_set (working_nt_set, B))
	PUSH (working_nt_stack, B);

      /* On "saute" prod */
      item = PROLON (prod+1)-1;

#if is_parser
      /* On ne note que A -> \alpha \beta . */
      CYK_numpd [--CYK_npd [B]] = item;
      nt2reduce_item_set_id [B] = 0;
#endif /* is_parser */
    }

    /* Ici on a isole' ds working_nt_set, working_nt_stack les B-reduce de Tkj
       Les items reduce correspondants sont ds numpd */

    /* On selectionne les items non-terminaux (ou finals) de Tik */
    sxba_2op (working_item_set, SXBA_OP_COPY, Tik, SXBA_OP_MINUS, TITEM_SET);

    item = 0;

    while ((item = sxba_scan (working_item_set, item)) > 0) {
      B = LISPRO (item);

      if (B > 0 && SXBA_bit_is_set (working_nt_set, B)) {
	/* item = A -> \alpha . B \beta, \alpha =>* x et i =>x k */
	next_item = item+1;

#if LLOG
	print_item (next_item, "", " = ");
	print_item (item, "", "");
#ifdef MAKE_INSIDEG
	old_B = spf.insideG.nt2init_nt [B];
#else /* ifndef MAKE_INSIDEG */	
	old_B = B;
#endif /* ifndef MAKE_INSIDEG */
	printf (" + <%s> [%i..%i] ;%s\n", ntstring [old_B], k, j, !SXBA_bit_is_set (Tij, next_item) ? " (new)" : "");
#endif /* LLOG */
	
	/* On met A -> \alpha B . \beta ds Tij */
	if (SXBA_bit_is_reset_set (Tij, next_item)) {
	  ret_val |= 1;

	  /* VALID_SUFFIXES = {A -> \alpha . \beta | \beta =>* \epsilon} */
	  if (new_reduce_Tij && SXBA_bit_is_set (VALID_SUFFIXES, next_item)) {
	    SXBA_1_bit (new_reduce_Tij, next_item);
	    ret_val = 3;
	  }
	  
#if is_epsilon
	  it = next_item;
	  
	  /* Si \beta = \beta1 \beta2 et si \beta1 =>+ \varepsilon, on y met aussi
	     A -> \alpha B \beta1 . \beta2 */
	  while ((X = LISPRO (it)) > 0 && EMPTY_SET && SXBA_bit_is_set (EMPTY_SET, X)) {
	    next_it = it+1;

#if LLOG
	    print_item (next_it, "", " = ");
	    print_item (it, "", "");
#ifdef MAKE_INSIDEG
	    X = spf.insideG.nt2init_nt [X];
#endif /* ifdef MAKE_INSIDEG */
	    printf (" + <%s> [%i..%i] ;%s\n", ntstring [X], j, j, !SXBA_bit_is_set (Tij, next_it) ? " (new)" : "");
#endif /* LLOG */

	    if (SXBA_bit_is_reset_set (Tij, next_it)) {
	      ret_val |= 1;

	      /* VALID_SUFFIXES = {A -> \alpha . \beta | \beta =>* \epsilon} */
	      if (new_reduce_Tij && SXBA_bit_is_set (VALID_SUFFIXES, next_it)) {
		SXBA_1_bit (new_reduce_Tij, next_item);
		ret_val = 3;
	      }
	    }

	    it++;
	  }
#endif /* is_epsilon */
	}

#if is_parser
	/* Pour l'extraction de la spf, on stocke les items de la forme
	   A -> [i] \alpha [k] B [j] . \beta */
	/* En fait, a partir
	   de l'item Earley  [A -> \alpha B . \beta] de la table T[j]
	   on peut retrouver les renseignements suivants :
	   A -> [i] \alpha [k] B [j] . \beta
	   Bkj -> \gamma_1 .
	      ...
           Bkj -> \gamma_p .
	*/
	ij = CYK_ij [i] [j];

	if (!XxYxZ_set (&CYK_item_hd, next_item, ij, k, &CYK_item)) {
	  ret_val |= 1;
#if LLOG
	  is_new = true;
#endif /* LLOG */
	}
#if LLOG
	else
	  is_new = false;

	print_xitem (next_item, next_item <= 3 ? 0 : i, j, k, "parse\t", is_new ? " (new)\n" : "\n");
	printf ("\t<%s> [%i..%i] = {\n", ntstring [old_B], k, j);
#endif /* LLOG */

	/* ... et les Bkj-prods associees */
	x = CYK_npd [B];
	  
	while (reduce_item = CYK_numpd [x++]) {
#if LLOG
	  is_new = !
#endif /* LLOG */
	    XxYxZ_set (&CYK_reduce_item_hd, B, reduce_item, CYK_ij [k] [j], &CYK_reduce_item);
#if LLOG
	  print_item (reduce_item, "\t\t", is_new ? " (new)\n" : "\n");
#endif /* LLOG */
	}

#if LLOG
	fputs ("\t}\n", stdout);
#endif /* LLOG */

#if is_epsilon
	/* Si \beta = \beta1 X \beta2 et si \beta1 X =>+ \varepsilon, tous les items
	   de la forme A -> \alpha B \beta1 X . \beta2 
	   vont referencer le nt instancie' Xjj	*/
	it = next_item;

	while ((X = LISPRO (it++)) > 0 && EMPTY_SET && SXBA_bit_is_set (EMPTY_SET, X)) {
	  if (!XxYxZ_set (&CYK_item_hd, it, ij, j, &CYK_item)) {
	    ret_val |= 1;
#if LLOG
	    is_new = true;
#endif /* LLOG */
	  }
#if LLOG
	  else
	    is_new = false;

	  print_xitem (next_item, i, j, j, "parse\t", is_new ? " (new)\n" : "\n");
#ifdef MAKE_INSIDEG
	  X = spf.insideG.nt2init_nt [X];
#endif /* ifdef MAKE_INSIDEG */
	  printf ("\t<%s> [%i..%i] = {\n", ntstring [X], j, j);
#endif /* LLOG */
	}
#endif /* is_epsilon */
#endif /* is_parser */
      }
    }

    /* On nettoie */
    while (!IS_EMPTY (working_nt_stack)) {
      B = POP (working_nt_stack);
      SXBA_0_bit (working_nt_set, B);
    
#if is_parser
      CYK_npd [B] = NPD (B+1)-1; /* remise en etat */
#endif /* is_parser */
    }
  }

#if LLOG
  printf ("} %s***********\n", ret_val ? "(new) " : "");
#endif /* LLOG */

  return ret_val;
}


static void
fill_CYK_ij ()
{
#if is_dag
  SXINT mlstn, dag_state, trans, Y, next_mlstn, t;

  for (mlstn = final_mlstn-1; mlstn >= init_mlstn; mlstn--) {
    dag_state = mlstn2dag_state (mlstn);

    XxYxZ_Xforeach (dag_hd, dag_state, trans) {
      Y = XxYxZ_Y (dag_hd, trans);
      next_mlstn = dag_state2mlstn (XxYxZ_Z (dag_hd, trans));
      CYK_ij [mlstn] [next_mlstn] = trans /* ij */;
#if is_sdag
      CYK_ij2t_set [trans] = glbl_source [Y];
#else /* !is_sdag */
      t = toks_buf [Y].lahead;
      SXBA_1_bit (CYK_ij2t_set [trans], t);
#endif /* !is_sdag */
    }
  }
#else /* !is_dag */
  SXINT mlstn, t;

  for (mlstn = n; mlstn > 0; mlstn--) {
    CYK_ij [mlstn] [mlstn+1] = mlstn /* ij */;
#if is_sdag
    CYK_ij2t_set [mlstn] = glbl_source [mlstn];
#else /* !is_sdag */
    t = glbl_source [mlstn];
    SXBA_1_bit (CYK_ij2t_set [mlstn], t);
#endif /* !is_sdag */
  }
#endif /* !is_dag */
}


/* C'est lexicalizer_mngr qui fait les alloc */
extern SXBA basic_item_set, basic_nt_set; 

#if is_lex
extern bool lexicalizer2basic_item_set (bool is_mlstn2lex_la_tset, bool is_mlstn2la_tset);
static SXBA *mlstn2lexicalized_look_ahead_t_set; /* contiendra, pour chaque mlstn du source l'ensemble des
						    terminaux valides (au sens de la lexicalisation) qui ont une transition depuis ce mlstn */
#endif /* is_lex */

/* On alloue les structures */
bool
sxearley_parse_it ()
{
  SXINT     size, t_size, i, j, k, ii, jj, nn, ij, ik, kj, max_span, max_i, span, CYK_ij_t_top, nt, CYK_ij_top, val;
  SXBA    Tij, Tik, Tkj, T11, T22, Tnn, Tii, Tjj, swap, new_reduce_Tij_1, new_reduce_Tij_2;
  bool is_empty, is_in_L, R1, R2;
#if is_lex2 || is_set_automaton
  SXBA *indexed_item_sets;
#endif /* is_lex2 || is_set_automaton */

  /* Modif car intialisation partielle interdite avec le -W ci-dessous */
  /* warning: missing field 'outputG' initializer [-Wmissing-field-initializers] */
  spf.inputG = spf_inputG;

  /* Traitement de la lexicalisation */
#  if is_lex
    is_in_L = lexicalizer2basic_item_set (false, false);
#  else /* !is_lex */
    is_in_L = true;
#  endif /* !is_lex */

    if (is_in_L) {
      /* La lexicalisation a marche' (ou pas de lexicalisation) */
      working_item_set = sxba_calloc (MAXITEM+1);
      working_Tij_set_1 = sxba_calloc (MAXITEM+1);
      working_Tij_set_2 = sxba_calloc (MAXITEM+1);
      working_nt_set = sxba_calloc (MAXNT+1);
      working_nt_stack = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), working_nt_stack [0] = 0;

#if is_parser
      CYK_npd = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT));
      CYK_numpd = (SXINT*) sxalloc (MAXITEM+1, sizeof (SXINT));
      nt2reduce_item_set_id = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT));
      
      /* Pour utiliser CYK_numpd sur la grammaire filtree */
      for (nt = 1; nt <= ntmax; nt++) {
	CYK_npd [nt] = NPD (nt+1)-1;
      }
#endif /* is_parser */

      size = (final_mlstn+1)*(final_mlstn+1);

      {
	SXINT *bot_area_ptr, **bot_ptr, **top_ptr;

	bot_area_ptr = CYK_ij_area = (SXINT*) sxcalloc (size, sizeof (SXINT));
	bot_ptr = CYK_ij = (SXINT**) sxalloc (final_mlstn+1, sizeof (SXINT*));

	top_ptr = CYK_ij+final_mlstn+1;
  
	while (bot_ptr < top_ptr) {
	  *bot_ptr++ = bot_area_ptr;
	  bot_area_ptr += final_mlstn+1;
	}
      }

#if is_dag
      /* Le nb de span des terminaux */
      CYK_ij_t_top = XxY_top (dag_hd);
      t_size = CYK_ij_t_top+1;
#else /* !is_dag */
      /* Le nb de span des terminaux */
      CYK_ij_t_top = n;
      t_size = final_mlstn+1;
#endif /* !is_dag */

#if is_sdag
      /* On va reutiliser glbl_source */
      CYK_ij2t_set = (SXBA *) sxalloc (t_size, sizeof (SXBA));
#else /* !is_sdag */
      CYK_ij2t_set = sxbm_calloc (t_size, SXEOF+1);
#endif /* !is_sdag */

      size = final_mlstn + ((final_mlstn-1)*final_mlstn)/2 + 1;
      CYK_Tij = sxbm_calloc (size, itemmax+1);

      /* derniere valeur de "ij" */
      CYK_ij_top = CYK_ij_t_top;

      /* Initialisation des tables avec les ij du source */
      fill_CYK_ij ();

      /* Phase d'initialisation des Tij */
      CYK_ij [init_mlstn] [init_mlstn] = ii = ++CYK_ij_top;
      T11 = CYK_Tij [ii];

      CYK_emptyshift (T11, basic_item_set);

      /* On enleve <S'> -> . $ <S> $ */
      SXBA_0_bit (T11, 0);
  
      if (final_mlstn > init_mlstn) {
	CYK_ij [final_mlstn] [final_mlstn] = nn = ++CYK_ij_top;
	Tnn = CYK_Tij [nn];
	sxba_copy (Tnn, T11);
	/* On ajoute S' -> $ <S> . $ */
	SXBA_1_bit (Tnn, 2);

	if (init_mlstn+1 < final_mlstn) {
	  CYK_ij [init_mlstn+1] [init_mlstn+1] = ii = ++CYK_ij_top;
	  T22 = CYK_Tij [ii];
	  sxba_copy (T22, T11);
	}
      }

      /* On ajoute S' -> $ . <S> $ */
      SXBA_1_bit (T11, 1);

#if is_epsilon
      if (n==0 /* source_vide */ && EMPTY_SET && SXBA_bit_is_set (EMPTY_SET, 1))
	/* On ajoute S' -> $ <S> . $ */
	SXBA_1_bit (T11, 2);
#endif /* is_epsilon */

      /* Phase d'initialisation sur les productions vides */
      /* Elle initialise aussi les items (valides) de la forme A -> . \alpha */
      for (i = init_mlstn+2; i < final_mlstn; i++) {
	CYK_ij [i] [i] = ii = ++CYK_ij_top;

	/* Tous les intermediaires sont identiques */
	CYK_Tij [ii] = T22; /* finesse !! */
      }
#if LOG
      for (i = init_mlstn; i <= final_mlstn; i++) {
	ii = CYK_ij [i] [i];
	print_Tij (i, i);
      }
#endif /* LOG */

      if (is_print_time)
	sxtime (TIME_FINAL, "\tCYK Initialization");

#if is_parser
      XxYxZ_alloc (&CYK_item_hd, "CYK_item_hd", n*itemmax+1, 1, CYK_item_hd_foreach, NULL, NULL);
      XxYxZ_alloc (&CYK_reduce_item_hd, "CYK_reduce_item_hd", n*prodmax+1, 1, CYK_reduce_item_hd_foreach, NULL, NULL);
#endif /* is_parser */

      max_span = final_mlstn-init_mlstn;

      for (span = 1; span <= max_span; span++) {
	/* length of span */
	max_i = final_mlstn-span;

	for (i = init_mlstn; i <= max_i; i++) {
	  /* start of span */
	  j = i+span; /* end of span */

	  if ((ij = CYK_ij [i] [j]) == 0)
	    CYK_ij [i] [j] = ij = ++CYK_ij_top;

	  Tij = CYK_Tij [ij];
	  is_empty = true;

	  /* On traite Tij = Tij-1 Tj-1j
	     puis Tij = Tij-2 Tj-2j
	     ...
	     puis Tij = Tii+1 Ti+1j
	     et enfin Tij = Tii + Tij */
	  new_reduce_Tij_1 = working_Tij_set_1;
	  R1 = false;

	  for (k = j-1; i <= k; k--) {
	    if ((ik = CYK_ij [i] [k]) && (kj = CYK_ij [k] [j])) {
	      if (Tik = CYK_Tij [ik]) {
		if (i == k) {
		  ii = ik;
		  Tii = Tik;
		}

		if (kj <= CYK_ij_t_top &&
		    (val = CYK_tshift (Tij, Tik, CYK_ij2t_set [kj], i == k ? new_reduce_Tij_1 : NULL
#if is_parser || LLOG
				       , i, j, k
#endif /* is_parser || LLOG */
				       ))) {
		  is_empty = false;

		  if (i == k && (val & 2)) {
		    /* Tij = Tii + tij 
		       a donne de nouveaux reduces ds new_reduce_Tij_1 */
		    R1 = true;
		    ii = ik;
		    Tii = Tik;
		  }
		}

		if (Tkj = CYK_Tij [kj]) {
		  /* On shifte dans Tij les B items de Tik s'il y a une B-prod complete ds Tkj */
		  if ((val = CYK_ntshift (Tij, Tik, Tkj, i == k ? new_reduce_Tij_1 : NULL
#if is_parser || LLOG
					  , i, j, k
#endif /* is_parser || LLOG */
					  ))) {
		    is_empty = false;

		    if (i == k && (val & 2)) {
		      /* Tij = Tii + Tij 
			 a donne de nouveaux reduces ds new_reduce_Tij_1 */
		      R1 = true;
		      ii = ik;
		      Tii = Tik;
		    }
		  }
		}
	      }
	    }
	  }

	  /* ... puis les chaines de productions unitaires par point fixe */

	  if (R1) {
	    new_reduce_Tij_2 = working_Tij_set_2;

	    /* new_reduce_Tij_1 est non vide et new_reduce_Tij_2 est vide */
	    while (2 & CYK_ntshift (Tij, Tii, new_reduce_Tij_1, new_reduce_Tij_2
#if is_parser || LLOG
				    , i, j, i
#endif /* is_parser || LLOG */
				    )) {
	      /* Resultat ds new_reduce_Tij_2 (non vide) mais new_reduce_Tij_1 a ete raze' */
	      swap = new_reduce_Tij_1;
	      new_reduce_Tij_1 = new_reduce_Tij_2;
	      new_reduce_Tij_2 = swap;
	    }
	  }

	  if (is_empty)
	    CYK_Tij [ij] = NULL;

#if LOG
	  print_Tij (i, j);
#endif /* LOG */
	}
      }

      is_in_L = ((ij = CYK_ij [init_mlstn] [final_mlstn])
		 && CYK_Tij [ij] /* pas vire' */
		 && SXBA_bit_is_set (CYK_Tij [ij], 2 /* S' -> $ S .$ */));

#if LOG
      printf ("\nRecognition %s\n", is_in_L ? "succeeds" : "fails");
#endif /* LOG */

      if (is_print_time) {
	if (is_in_L)
	  sxtime (TIME_FINAL, "\tCYK Recognizer (true)");
	else
	  sxtime (TIME_FINAL, "\tCYK Recognizer (false)");
      }


#if is_parser
      if (is_in_L) {
	spf_allocate_Aij (MAXNT, SXEOF);
	spf_allocate_spf (MAXPROD);
	DALLOC_STACK (CYK_rhs_stack, 1000);

	CYK_extract_spf ();

	spf.outputG.maxxnt = spf.outputG.maxnt = XxYxZ_top (spf.outputG.Axixj2Aij_hd);
	spf.outputG.maxxprod = spf.outputG.maxprod;
    
	spf_finalize (); 
	
	if (spf.inputG.has_cycles)
	  /* Le 13/01/06 */
	  /* je vire les cycles eventuels de la foret */
	  spf_make_proper (spf.outputG.start_symbol);

	/* else is_default_semantics, on execute la semantique specifiee par la grammaire. */
	if (for_semact.sem_pass != NULL) {
	  (*for_semact.sem_pass) ();
	}

	DFREE_STACK (CYK_rhs_stack);
	spf_free ();
      }

      XxYxZ_free (&CYK_item_hd);
      XxYxZ_free (&CYK_reduce_item_hd);

      sxfree (CYK_npd), CYK_npd = NULL;
      sxfree (CYK_numpd), CYK_numpd = NULL;
      sxfree (nt2reduce_item_set_id), nt2reduce_item_set_id = NULL;
#endif /* is_parser */

      sxfree (CYK_ij_area), CYK_ij_area = NULL;
      sxfree (CYK_ij), CYK_ij = NULL;
#if is_sdag
      sxfree (CYK_ij2t_set), CYK_ij2t_set = NULL;
#else /* !is_sdag */
      sxbm_free (CYK_ij2t_set), CYK_ij2t_set = NULL;
#endif /* !is_sdag */
      sxbm_free (CYK_Tij), CYK_Tij = NULL;

      sxfree (working_item_set), working_item_set = NULL;
      sxfree (working_Tij_set_1), working_Tij_set_1 = NULL;
      sxfree (working_Tij_set_2), working_Tij_set_2 = NULL;
      sxfree (working_nt_set), working_nt_set = NULL;
      sxfree (working_nt_stack), working_nt_stack = NULL;
    }

#if is_lex
  if (basic_item_set) sxfree (basic_item_set), basic_item_set = NULL;
  if (basic_nt_set) sxfree (basic_nt_set), basic_nt_set = NULL;
#endif /* is_lex && !is_lex2 && !is_set_automaton */

  if (is_print_time) {
    if (is_in_L)
      sxtime (TIME_FINAL, "\tCYK Parser (true)");
    else
      sxtime (TIME_FINAL, "\tCYK Parser (false)");
  }

  return is_in_L;
}



/* Ajoute' le 17/11/05 */
/* On peut faire suivre chaque terminal du source d'une chaine entre crochets qui est passee a la semantique */
/* Appele' depuis sxearley_main avant le scanner, permet a la semantique de remplir le nouveau champ
   for_semact.scanact avec la ft qui traite les [...] for_semact.scanact (tok_no, t) ds le cas is_dag
*/
void
sxearley_open_for_semact ()
{
  extern void sxexit_timeout();

  for_semact.timeout_mngr=sxexit_timeout;
  for_semact.vtimeout_mngr=sxexit_timeout;

  if (is_no_semantics) {
    for_semact.sem_init = NULL;
    for_semact.sem_final = NULL;
    for_semact.sem_close = NULL;
    for_semact.semact = NULL;
    for_semact.parsact = NULL;
    for_semact.prdct = NULL;
    for_semact.constraint = NULL;
    for_semact.sem_pass = NULL;
    for_semact.scanact = NULL;
  }
  else {
    if (is_parse_tree_number) {
      for_semact.sem_init = NULL;
      for_semact.sem_final = NULL;
      for_semact.sem_close = NULL;
      for_semact.semact = NULL;
      for_semact.parsact = NULL;
      for_semact.prdct = NULL;
      for_semact.constraint = NULL;
      for_semact.sem_pass = spf_print_tree_count;
      for_semact.scanact = NULL;
    }
    else {
#ifdef SEMANTICS
      /* Le 29/08/2003 */
      /* l'option de compilation -DSEMANTICS="ma_semantique" permet de specifier le point d'entree
	 de la semantique de l'utilisateur.  La procedure ma_semantique() remplit for_semact. */
      /* Le 23/02/06 appel de SEMANTICS meme si !is_default_semantics.  C,a permet a la semantique
	 de lire ses options.  On peut donc utiliser un appel normal en ajoutant -ns ou -ptn sans
	 changer les autres args (lus par la semantique) et qui deviendraient donc ille'gaux en
	 absence de semantique */
      SEMANTICS ();
      /* Les options d'analyse -no_semantics, -default_semantics, -parse_tree_number et -parse_forest
	 permettent d'influer la dessus... */
      /* Si -parse_forest l'analyse earley construit une foret partagee (ds spf) qui peut etre exploitee par
	 for_semact.sem_pass */
#endif /* SEMANTICS */
    }
  }
}

/* Ca permet d'eviter que la compil de l'utilisateur de'pende de is_dag */
/* permet d'acceder au "token" source a partir Tpq */
#if is_dag
#define Tpq2tok(Tpq)  toks_buf [spf.outputG.Tij2tok_no [Tpq]]
#else /* !is_dag */
#define Tpq2tok(Tpq)  SXGET_TOKEN (spf.outputG.Tij2tok_no [Tpq])
#endif /* !is_dag */

/* Ca permet ds tous les cas a la semantique d'acceder au token associe' a un Tpq */
struct sxtoken*
parser_Tpq2tok (SXINT Tpq)
{
  if (spf.outputG.Tij2tok_no == NULL)
    spf_fill_Tij2tok_no ();

  return &(Tpq2tok (Tpq));
}

/* Pour le lexicalizer ou la semantique */
void
output_nt (SXINT nt)
{
  printf ("<%s> ", spf.inputG.ntstring [nt]);
}


void
output_t (SXINT t)
{
  printf ("\"%s\" ", spf.inputG.tstring [t]);
}



void
output_prod (SXINT prod)
{
  SXINT        item, X;

  printf ("%i\t", prod);

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
