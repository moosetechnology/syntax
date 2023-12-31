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






static char	ME [] = "earley_parser";


/* is_lex==1 => On fait de la lexicalisation pour du earley
   Si toutes les autres options sont a 0 c'est le source global qui est utilise
   Si is_dynam_lex=1 on fait la lexicalisation par indice du source i en ne prenant en compte que le suffixe a_i ... a_n
   Si is_rfsa=1 on utilise l'automate rfsa pour calculer la lexicalisation par indice du source i
   Si de plus is_lfsa=1, on fait d'abord une passe gauche-droite par l'automate lfsa
   Si is_set_automaton=1 on calcule dynamiquement le set automaton :
   une premiere passe gauche-droite calcule les active sets
   une deuxieme passe droite-gauche (qui utilise le mirror set automaton), guide' par le resultat de la 1ere
   passe note les resultats definitifs.
   L'option de compilation is_lrsa est devenue inutile
*/  

/* Idee pour le traitement "simultane" de la semantique.

 Le point d'entre "action" traite simultanement toutes les regles
 Aik -> X Y ... Z
 action (prod_no, i, j_set, k)
 ou j_set contient tous les indices j de Zjk, dernier symbole de la rhs.
 La fonction action doit retourner la ou les regles retenues apres evaluation
 sous la forme d'un ensemble de p-uplets ou p = |rhs|-1.  Chaque p-uplet
 (i1, i2, ..., ip) est tel que Aik -> Xii1 Yi1i2 ... Zipk est une regle valide
 (bien entendu ip \in j_set). On peut peut etre ne noter que les non-terminaux.
 Avantage on traite simultanement O(n) regles dont on ne conserve eventuellement
 qu'une seule (et peut etre aucune si la "contrainte" n'est pas verifiee.)
*/


/* Une regle de la foret partagee issue de la production p = A -> w0 B1 ... Bl wl
   est un doublet (Aik, m) avec m = (... ((-p, j1), j2), ..., jl) ou les j1, ..., jl sont les
   index finals de B1, ..., Bl.
   Ex: Si on a la prod p = A -> B C qui a donne les 4 regles
   Aim -> Bil Clm
   Aim -> Bik Ckm
   Ajm -> Bjl Clm
   Ajm -> Bjk Ckm
   qui utilisent 8 symboles differents et 12 occur
   elles sont representees par
   (Aim, (-p,l))
   (Aim, (-p,k))
   (Ajm, (-p,l))
   (Ajm, (-p,k))
   4 symbols differents et 8 occur.
*/

/* Ordre ds l'execution des actions et predicats.

   On suppose que la grammaire est ;
      - non cyclique
      - epsilon-free (pour le moment)

   Condition :
      - un predicat associe au symbole X[i..j] ou une action associee a une regle
        dont X[i..j] est un symbole en RHS ne peuvent s'executer que s'il n'y a pas
	de reduction ulterieure produisant (en LHS) le symbole X[i..j].

   Theoreme :
      cette condition est verifiee si on impose les conditions suivantes :
      - les items de X[j] sont generes avant (et ont donc des identifiants inferieurs)
        a ceux de Y[l] si ;
	- j < l
	- ou j == l et X ->+ Y (chaine de reductions simples de X a Y).

   Demonstration :
      - soit (A -> ... X., h) de X[j]
      - soit (B -> ... Y., k) de Y[l]

      Par hypothese (A -> ... X., h) < (B -> ... Y., k) cad que la reduction B -> ... Y
      sera effectuee avant A -> ... X
      On suppose que cette sequence d'action est "lancee" apres le scan(/reduce) de ai.
      On montre que A[h..i] est different de Y[l..i].

      1) Si j < l
         on sait que h <= j => h < l et donc A[h..i] # Y[l..i]

      2) Si j < l
            - Si |... X| > 1 => h < j => h < l et donc A[h..i] # Y[l..i]
	    - Si |... X| == 1 => j == l == h et A -> ... X est une production simple et les A[j] sont
	      generes avant les X[j]. Si A[h..i] == Y[l..i] => A == Y et on a une contradiction
	      avec le fait que X[j] est genere avant Y[l]==A[j].
*/

#define SX_DFN_EXT_VAR2
#include "sxversion.h"
#include "sxunix.h"
#include "sxba_bag.h"
#include <math.h>

char WHAT_EARLEY_PARSER[] = "@(#)SYNTAX - $Id: earley_parser.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

SXUINT          maximum_input_size; /* Pour udag_scanner */
SXBOOLEAN       tmp_file_for_stdin; /* Pour read_a_re */
SXBOOLEAN       sxtty_is_stderr;

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
/* #define def_t2prod_item_set */
#define def_BVIDE
#define def_titem_set
#define def_rhs_nt2where
#define def_left_corner
#define def_nt2item_set
#define def_t2item_set
#define def_nt2order
#define def_order2nt
#if is_rcvr
#define def_nt2min_gen_lgth
#endif /* is_rcvr */
#if is_parser
#define def_lhs_nt2where
#if 0
#define def_prod2order
#define def_order2prod
/* Essai is_right_recursive va etre mis a 0 ds tous les cas */
#define def_prod_order2max
#define def_prod_order2min
#endif /* 0 */
#define def_empty_prod_item_set
#endif /* is_parser */

#include LC_TABLES_H
#endif /* LC_TABLES_H */

/* ESSAI */
#if is_right_recursive
#undef is_right_recursive
#endif /* is_right_recursive */

#define is_right_recursive 0

#ifdef MAKE_INSIDEG
/* Compilation de earley_parser avec l'option -DMAKE_INSIDEG
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
#include "varstr.h"
/* Pour avoir la definition et l'initialisation des variables (is_print_time, is_no_semantics, is_parse_tree_number, is_default_semantics) de rcg_sglbl.h faites ici */
#define SX_DFN_EXT_VAR_RCG
#include "earley.h"
#include "fsa.h"

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
#define PROD(i)         spf.insideG.prod2init_prod [i]
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
#define PROD(i)         i
#define NTSTRING(i)     ntstring [i]
#define TSTRING(i)      tstring [i]

#define RHS_NT2WHERE(i) rhs_nt2where [i]
#define LHS_NT2WHERE(i) lhs_nt2where [i]
             
#define	EMPTY_SET           BVIDE
#define	TITEM_SET           titem_set
#define	RC_TITEM_SET        rc_titem_set
#if is_epsilon
#define	EMPTY_PROD_ITEM_SET empty_prod_item_set
#else
#define	EMPTY_PROD_ITEM_SET NULL
#endif /* is_epsilon */
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

#include "XH.h"

extern void sxexit_timeout();

/* is_rcvr active le nouveau traitement d'erreur (cas is_dag)
   Il fait essentiellement du rattrapage "global"
   Si is_sa_rcvr il peut y avoir modification du dag source ce qui peut donc correspondre a de la correction
   ds earley (pas teste', messages d'erreur au niveau earley A FAIRE).
   Ds un premier temps, il n'est pas necessaire d'implanter cette nelle rcvr ds le cas lineaire ou sdag (puisqu'elle
   marche ds tous les cas...).  Il suffit de pretendre qu'un source lineaire ou un sdag est un dag !
*/

/* Guidage de la rcvr earley par la lexicalisation :
   Principe : on ne tente une recuperation d'erreur ds earley que si elle est compatible avec le
   traitement de la lexicalisation.
   Consequence : on ne se recuperera jamais sur des items non selectionnes par la lexicalisation.
   N'est-ce pas un peu dur car on ne pourra jamais se rattraper sur des phrases generees par
   des bouts de regles non selectionnees.

   Cas is_set_automaton :
   Si une erreur est detectee en SA, elle le sera en CF.
   Inversement une erreur en CF peut tres bien ne pas l'etre en SA.
     - il a marche' (pas d'erreur ou le traitement d'erreur a marche' -- Pb de son propre guidage par is_lex[2] --)
       on ne tente une recuperation d'erreur ds earley que si elle est compatible avec le traitement SA.  On n'essaye
       de se recuperer que ds le sur-langage regulier.  Un item earley de la recuperation est ajoute a la table
       dag_state seulement si cet item est un element de lex_compatible_item_sets [dag_state].
     - il n'a pas marche' (erreur et pas de is_dag_rcvr ou rcvr a echoue') => aucun filtrage

   Cas !is_set_automaton && is_lex[2] :
   Lors de la detection de la 1ere erreur, il faut "enlever" le filtrage lexical et TOUT recommencer car
   les etats deja traites ont ete prepares pour accepter des transitions non-terminales, mais certaines peuvent
   manquer!!
*/

/* Le 27/10/06 suppression de delta_n */
/* delta_n est un #define donne a la compilation qui donne le nombre maximal
   d'accroissement possible (par correction d'erreurs) de la chaine source. */
/* Meme ds le cas is_dag */
/* Ds les cas is_[dag_]rcvr, delta_n==0 => rattrapage global uniquement */

static SXINT          pass_number;

static SXINT          Tpq_repair_nb, Pij_repair_nb;
static SXINT          *repair_Tpq2tok_no;

static SXINT          working_rcvr_mlstn, rcvr_detection_table;
static SXBOOLEAN      rcvr_detection_table_cleared;

static SXBA           ilex_compatible_item_set;

#ifdef bnf21rcg_include_file
/* On analyse d'apres une grammaire bnf produite a partir d'une 1RCG */
#include bnf21rcg_include_file
#endif /* bnf21rcg_include_file */

#define PUSH(s,x)	(s)[++*(s)]=(x)
#define POP(s)		(s)[(*(s))--]
#define IS_EMPTY(s)	(*(s)==0)


#if is_rcvr
#define INSERTION     10
#define SUPPRESSION   10
#define CHANGEMENT    5
#define DEPLACEMENT   1


/* Format des textes des messages d'erreur... */
static char rcvr_start_point [] = "%sGlobal Earley recovery starts from here...";
static char rcvr_finish_point [] = "%s... until there, after a dummy transition on %s.";
static char rcvr_eof [] = "%sParsing stops on end of file.";
static char trivial_rcvr [] = "%sTrivial uncaught error recovery on end of file.";

static char sa_rcvr_insert_t [] = "%sA \"%s\" is inserted before \"%s\".";
static char sa_rcvr_start_point [] = "%sGlobal set automaton right to left recovery on \"%s\".";
static char sa_rcvr_resume_point [] = "%sParsing resumes on \"%s\".";


static struct rcvr_spec rcvr_spec_default = {INSERTION, SUPPRESSION, CHANGEMENT, DEPLACEMENT, MIXED_FORWARD_RANGE_WALK_KIND,
					     1 /* 1 seule chaine */, 1 /* 1 seule analyse de cette chaine */, TRY_MAX /* precision max */,
					     SXTRUE /* REPAIR */, SXTRUE /* PARSE */};
/* Les symboles d'une correction sont de la forme Xpq avec p et q negatifs
   Si -p ou -q <= final_mlstn, -p ou -q sont des  mlstn normaux
   Si -p ou -q > final_mlstn (et <= repair_mlstn_top) -p  ou -q sont des  mlstn normaux
   Ex: si la chaine "Jean aime Marie" est generee par "X aime Z T" qui couvre le source [2..3] de longueur 5
   (et si c'est la 1ere correction), on a
   Si X =>* Jean, Z =>* epsilon et T =>* Marie;
   final_mlstn = 6
   repair_mlstn_top = 8
   et X[-2..-7] aime[-7..-8] Z[-8..-8] T[-8..-3]
   avec les productions qui, a partir de X, Z et T generent "Jean", le vide et "Marie"
*/
   
static SXINT              repair_mlstn_top /* indice max atteint par les symboles impliques ds une correction */ ;
static SXBOOLEAN          perform_copy_local_repair_prod_stack /* pour savoir si on recopie ou non le résultat du
							      rcvr local dans la pile globale de rcvr */ ;

/*
  Le filtrage (lexicalisation) a pu enlever des items sur lesquels de la rcvr est possible
  Ce sont des items des nt2item_set [A], A \in rcvr_shift_NT_hd_set_mlstn et A>0
  On les met ds rcvr_non_kernel_item_set
  Ds earley_dag_glbl_rcvr (), on essaie d'abord de se recuperer sur rcvr_kernel_item_set, si ca echoue
  on essaie ensuite de se recuperer sur rcvr_non_kernel_item_set-rcvr_kernel_item_set
*/
#endif /* is_rcvr */

#ifdef DYNAMIC_BEAM_MEASURES
static SXLOGPROB* DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob;
static SXLOGPROB* DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob;
#endif /* DYNAMIC_BEAM_MEASURES */

#if MEASURES
void
MEASURE_time (what)
    SXINT what;
{
#include <sys/time.h>
#include <sys/resource.h>

#define TIME_INIT	0
#define TIME_FINAL	1

  static struct rusage prev_usage_2, next_usage_2;

  if (what == TIME_INIT) {
    getrusage (RUSAGE_SELF, &prev_usage_2);
    fputs ("MEASURES: TIME = START\n", stdout);
  }
  else {
    getrusage (RUSAGE_SELF, &next_usage_2);

    printf ("MEASURES: TIME = %i.%.2d\n", (next_usage_2.ru_utime.tv_sec - prev_usage_2.ru_utime.tv_sec), (next_usage_2.ru_utime.tv_usec  - prev_usage_2.ru_utime.tv_usec) / 10000);

    prev_usage_2.ru_utime.tv_sec = next_usage_2.ru_utime.tv_sec;
    prev_usage_2.ru_utime.tv_usec = next_usage_2.ru_utime.tv_usec;
  }
}
#endif /* MEASURES */

/* Ajoute' le 17/11/05 */
/* On peut faire suivre chaque terminal du source d'une chaine entre crochets qui est passee a la semantique */
/* Appele' depuis sxearley_main avant le scanner, permet a la semantique de remplir le nouveau champ
   for_semact.scanact avec la ft qui traite les [...] for_semact.scanact (tok_no, t) ds le cas is_dag
*/

#ifdef SEMANTICS
extern void SEMANTICS (void);
#endif /* SEMANTICS */
void
sxearley_set_for_semact (void)
{
#if is_rcvr
  /* valeurs par defaut */
  rcvr_spec = rcvr_spec_default;
#endif /* is_rcvr */

  for_semact.timeout_mngr=sxexit_timeout;
  for_semact.vtimeout_mngr=sxexit_timeout;

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

#if 0
void
sxearley_close_for_semact (void)
{
#ifdef SEMANTICS
  if (for_semact.sem_close) {
    (*for_semact.sem_close) ();
  }
#endif /* SEMANTICS */
}
#endif /* 0 */


#if has_Lex
static SXINT split_val;

#ifdef lrprod_list_include_file
/* Cet include construit par "rcg -1rcg -lrprod ../src/xtag3.lrprod_list.h L.rcg" permet de filtrer
   les guides, supertaggeurs, ... en supprimant par exemples les prod_gauches (issues de la partie
   gauche d'un arbre auxilliaire) qui n'ont pas de prod_droite associee. */
#include lrprod_list_include_file

static SXINT lrprod_filtered_nb;

static void
lrprod_filtering (main_item_set, MAIN_KIND, secondary_item_set, SECONDARY_KIND)
     SXBA main_item_set, secondary_item_set;
     SXUINT MAIN_KIND, SECONDARY_KIND;
{
  SXUINT head, v;
  SXINT assoc_item, body, item, prod, top;

  item = 3;

  while ((item = sxba_scan (main_item_set, item)) > 0) {
    prod = PROLIS (item);
    top = PROLON (prod+1);

    if ((head = left_right_prod_hd [prod]) && (head & MAIN_KIND)) {
      /* prod gauche provenant d'un arbre auxiliaire */
      body = head & EXTRACT_PROD_BODY;

      if (head & IS_SINGLE_PROD) {
	/* body est la prod droite unique associee a prod */
	assoc_item = PROLON (body);

	if (!SXBA_bit_is_set (secondary_item_set, assoc_item)) {
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  sxba_clear_range (main_item_set, item, top-item);
	  lrprod_filtered_nb++;
	}
      }
      else {
	while ((v = left_right_prod_list [body++]) && (v & SECONDARY_KIND)) {
	  assoc_item = PROLON (v & EXTRACT_PROD_BODY);
      
	  if (SXBA_bit_is_set (secondary_item_set, assoc_item)) {
	    /* Au moins un couple est bon, prod est donc valide */
	    break;
	  }
	}

	if (v == 0U) {
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  sxba_clear_range (main_item_set, item, top-item);
	  lrprod_filtered_nb++;
	}
      }
    }

    /* On "saute" prod */
    item = top-1;
  }
}
#endif /* lrprod_list_include_file */

static SXBA clause_val_set, prod_checked_set, prod_val_set, Lex_prod_item_set;

static SXBOOLEAN
check_Lex (prod)
     SXINT prod;
{
  SXINT     clause, bot, top, t;
  SXBOOLEAN ret_val;
  SXBA    Lex_set;
  
  if (SXBA_bit_is_reset_set (prod_checked_set, prod)) {
    /* C'est la 1ere fois qu'on trouve prod */
    clause = prod2clause [prod];  

    if (SXBA_bit_is_set (clause_has_Lex, clause)) {
      /* Il faut verifier les &Lex (qui sont differents) des clauses identiques */
      bot = clause2identical_disp [clause];
      top = clause2identical_disp [clause+1];

#if EBUG
      if (bot == top)
	/* Si on n'a pas pu verifier les Lex, il y a obligatoirement des clauses identiques */
	/* Si les clauses identiques ont les memes &Lex, ces &Lex ont deja ete verifies */
	sxtrap (ME, "check_Lex");
#endif

      t = 0;

      if (Lex_set = clause2Lex_set [clause]) {
	while ((t = sxba_scan (Lex_set, t)) > 0) {
	  if (!SXBA_bit_is_set (source_set, t))
	    break;
	}
      }

      if (t <= 0) {
	SXBA_1_bit (clause_val_set, clause);
	ret_val = SXTRUE;
      }
      else
	ret_val = SXFALSE;

      do {
	clause = clause2identical [bot];
	t = 0;
	
	if (Lex_set = clause2Lex_set [clause]) {
	  while ((t = sxba_scan (Lex_set, t)) > 0) {
	    if (!SXBA_bit_is_set (source_set, t))
	      break;
	  }
	}

	if (t <= 0)
	  ret_val = SXTRUE;
	else
	  clause2identical [bot] = 0;
      } while (++bot < top);

      if (ret_val)
	SXBA_1_bit (prod_val_set, prod);

      return ret_val;
    }

    SXBA_1_bit (prod_val_set, prod);
    return SXTRUE;
  }

  return SXBA_bit_is_set (prod_val_set, prod);
}

/* On supprime de source2valid_item_set les items de la prod principale representant des prod multiples
   (&Lex differents) dont tous les representants repondent faux */
/* must_check_identical_prod_set = {prod issues de clauses differentes / &Lex} */
static void
equiv_prod_Lex_filter (source2valid_item_set)
     SXBA source2valid_item_set;
{
  SXINT item, prod, top;

  item = 3;

  while ((item = sxba_scan (source2valid_item_set, item)) > 0) {
    prod = PROLIS (item);
    top = PROLON (prod+1);
      
    if (SXBA_bit_is_set (must_check_identical_prod_set, prod)
	/* Prod est le representant d'une classe d'equivalence de prod identiques sur lesquelles
	   il faut verifier les Lex */
	&& !check_Lex (prod)) {
	/* "supprimer" prod */
	/* On enleve les items de prod */
	sxba_clear_range (source2valid_item_set, item, top-item);
    }
      
    /* On "saute" prod */
    item = top-1;
  }
}


/* Remplit Lex_prod_item_set avec les items des prod dont les Lex sont compatibles avec le source */
static void
process_Lex ()
{
  SXINT i, t, item, prod, top;
  SXBA item_set, multiple_Lex_set;

  if (sxba_cardinal (source_set) > split_val) {
    sxba_empty (Lex_prod_item_set);
    t = 0;

    while ((t = sxba_0_lrscan (source_set, t)) > 0) {
      if (item_set = Lex2prod_item_set [t])
	sxba_or (Lex_prod_item_set, item_set);
    }

    /* Ici, Lex_prod_item_set contient les prod non selectionnables */
    sxba_not (Lex_prod_item_set); /* Ce qui n'est pas interdit est permis */
  }
  else {
    t = 0;

    while ((t = sxba_scan (source_set, t)) > 0) {
      /* Lex2prod_item_set [0] est initialise statiquement avec l'ensemble des items des prod n'ayant pas de &Lex() */
      /* Lex2prod_item_set [t] contient l'ensemble des items des prod ayant &Lex(t) */
      if (item_set = Lex2prod_item_set [t])
	sxba_or (Lex_prod_item_set, item_set);
    }

    /* On filtre en plus en tenant compte des &Lex multiples ds les prod */
    /* multiple_Lex_item_set, initialise ds les tables, contient les items des clauses ayant des &Lex multiples */
    sxba_and (multiple_Lex_item_set, Lex_prod_item_set);

    item = -1;

    while ((item = sxba_scan (multiple_Lex_item_set, item)) >= 0) {
      prod = PROLIS (item);
      top = PROLON (prod+1);

      if ((multiple_Lex_set = prod2multiple_Lex_set [prod]) && !sxba_is_subset (multiple_Lex_set, source_set)) {
	/* "supprimer" prod */
	/* On enleve les items de prod */
	sxba_clear_range (Lex_prod_item_set, item, top-item);
      }
      
      /* On "saute" prod */
      item = top-1;
    }
  }

#if is_epsilon
#ifdef MAKE_INSIDEG
  if (IS_EPSILON) 
#endif /* MAKE_INSIDEG */
    {
      /* Ajoute le 03/12/2002 */
      sxba_and (EMPTY_PROD_ITEM_SET, Lex_prod_item_set);

      item = 3;

      while ((item = sxba_scan (EMPTY_PROD_ITEM_SET, item)) > 0) {
	prod = PROLIS (item);
	top = PROLON (prod+1);
      
	if (SXBA_bit_is_set (must_check_identical_prod_set, prod)
	    /* Prod est le representant d'une classe d'equivalence de prod identiques (vides) sur lesquelles
	       il faut verifier les Lex */
	    && !check_Lex (prod)) {
	  /* "supprimer" prod */
	  /* On enleve les items de prod */
	  sxba_clear_range (EMPTY_PROD_ITEM_SET, item, top-item);
	}
      
	/* On "saute" prod */
	item = top-1;
      }
    }
#endif /* is_epsilon */
}

static void
fill_loop_clause_set ()
{
  SXINT clause, t;
  SXBA Lex_set;

  /* Le 10/6/2002 : changement, l'ensemble passe' au parseur RCG par earley2init_guide doit contenir (ce peut
     etre un sur-ensemble) l'ensemble des loop clauses qui NE SONT PAS verifiees par les &Lex.
     Si une clause est exclue par un &Lex, les clauses dont elle est issue (par -2var_form ou -1rcg) le sont
     aussi necessairement. Cette propriete facilite donc les "remontees" vers l'analyse par la grammaire d'origine. */
  clause = 0;

  while ((clause = sxba_scan_reset (loop_clause_set, clause)) > 0) {
    if (Lex_set = clause2Lex_set [clause]) {
      t = 0;

      while ((t = sxba_scan (Lex_set, t)) > 0) {
	if (!SXBA_bit_is_set (source_set, t)) {
	  SXBA_1_bit (loop_clause_set, clause);
	  break;
	}
      }
    }
  }
}
#else /* !has_Lex */
static SXBA loop_clause_set;
/* S'il n'y a pas de &Lex, toutes les boucles sont bonnes a priori ... */ 
#endif /* !has_Lex */

/* ********************************************************************************************************* */
/* visible par tout le monde parceque + pratique ... */
static SXBA    *i2rl_supertagger_item_set;
static SXBOOLEAN is_mlstn2non_lexicalized_look_ahead_t_set, is_mlstn2lexicalized_look_ahead_t_set, is_lex_compatible_item_sets;


/* C'est lexicalizer_mngr qui fait les alloc */
extern SXBA basic_item_set, basic_nt_set; 

#if is_lex
extern SXBA *mlstn2lex_la_tset;
extern SXBA *mlstn2la_tset;

extern SXBOOLEAN lexicalizer2basic_item_set (SXBOOLEAN is_mlstn2lex_la_tset, SXBOOLEAN is_mlstn2la_tset, SXBOOLEAN is_smallest_insideG);
static SXBA *mlstn2lexicalized_look_ahead_t_set; /* contiendra, pour chaque mlstn du source l'ensemble des
						    terminaux valides (au sens de la lexicalisation) qui ont une transition depuis ce mlstn */
#else /* is_lex */
static SXBA *mlstn2la_tset;
#endif /* is_lex */

static SXBA *mlstn2non_lexicalized_look_ahead_t_set; /* Meme chose mais sans tenir compte de la lexicalisation */
/* en particulier en cas d'erreur c'est mlstn2non_lexicalized_look_ahead_t_set qui sera utilise' */
static SXBA *mlstn2look_ahead_t_set; /* pointe sur mlstn2lexicalized ou mlstn2non_lexicalized suivant le cas */

/* Le source est stocke' ds les structures de... */
#include "udag_scanner.h"

static SXBA pq_t_set;

#if is_1la
static SXBA *mlstn2look_ahead_item_set;
#endif /* is_1la */

static SXBA mlstn_active_set, dead_ends, successful_p_set, beam_rcvr_resuming_states;

/* before_first_udag_trans() et after_last_trans() ont ete mis ds dag_scanner.c le 19/09/06 */


/* ********************************************************************************************************* */
/* Ici le source est soit un sdag, soit lineaire */

#if is_supertagging
/* A REVOIR */
static SXBA used_non_lex_prod_set;

/* item adresse un terminal, on est su^r que ce terminal a l'index i selectionne la prod PROLIS (item) */
/* Si i == 0, item designe une clause non lexicalisee (on fait un appel unique) */
static void
call_supertagger (item, i)
  SXINT item, i;
{
  SXINT t, bot, top, prod, clause;

  if (item <= 3)
    return;

  prod = PROLIS (item);

  if (i == 0
      && used_non_lex_prod_set
      && !SXBA_bit_is_reset_set (used_non_lex_prod_set, prod))
    /* On appelle une seule fois avec une clause non lexicalisee */
    return;

#if is_sdag
  /* Ds la foulee, on refabrique un sdag en sortie */
  if (i > 0 && (t = -LISPRO (item)) > 0)
    /* t == 0 => prod est non lexicalisee */
    SXBA_1_bit (glbl_out_source [i], t);
#endif /* is_sdag */

  clause = prod2clause [prod];
  bot = clause2identical_disp [clause];
  top = clause2identical_disp [clause+1];
  
#if has_Lex
  if (SXBA_bit_is_set (clause_has_Lex, clause)) {
    /* Le resultat des Lex de clause a ete memoize ds clause_val_set et clause2identical */
    if (SXBA_bit_is_set (clause_val_set, clause))
      supertagger (clause, i);

    do {
      if (clause = clause2identical [bot])
	supertagger (clause, i);
    } while (++bot < top);

    return;
  }
#endif /* has_Lex */

  /* Ici, les Lex eventuels ont deja ete verifies */
  supertagger (clause, i);
    
  if (bot < top) {
    do {
      supertagger (clause2identical [bot], i);
    } while (++bot < top);
  }
}
#endif /* is_supertagging */


#if is_supertagging
static SXINT	        *ff_ste;

/* Pour ressortir le sdag reduit */
static void
print_glbl_out_source ()
{
  SXINT tok, t, ste, t_nb;
  char *str;
  char mess [32];
  SXBOOLEAN is_first;
  SXBA line;
    
  if (is_print_time) {
    t_nb = 0;

    for (tok = 1; tok <= n; tok++) {
      t_nb += sxba_cardinal (glbl_out_source [tok]);
    }

    sprintf (mess, "\tOut_source[" SXILD "/" SXILD "]", n, t_nb);
    sxtime (SXACTION, mess);
  }

  for (tok = 1; tok <= n; tok++) {
    if ((ste = ff_ste [tok]) < 0)
      str = sxttext (sxplocals.sxtables, -ste);
    else
      str = sxstrget (ste);

    printf ("%s {", str);
    line = glbl_out_source [tok];
    t = 0;
    is_first = SXTRUE;

    while ((t = sxba_scan (line, t)) > 0) {
      if (is_first)
	is_first = SXFALSE;
      else
	fputs (" ", stdout);

      printf ("%s", sxttext (sxplocals.sxtables, t));
    }
	    
    fputs ("} ", stdout);       
  }

  fputs ("\n", stdout);
}
#endif /* is_supertagging */

/* Doivent etre connu du main */
SXINT
supertag_act (numact, tok_no)
     SXINT numact, tok_no;
{
#if is_supertagging
  switch (numact) {
  case 2: /* <FF> = %MOT ; 2 */ 
    if (ff_ste != NULL)
      ff_ste [tok_no] = SXGET_TOKEN (sxplocals.atok_no-1).string_table_entry;
    break;
  case 3: /* <FF> = <T> ; 3 */
    if (ff_ste != NULL)
      ff_ste [tok_no] = -SXGET_TOKEN (sxplocals.atok_no-1).lahead;
    break;
  case 4: /* <FF> = ; 4 */
    if (ff_ste != NULL)
      ff_ste [tok_no] = SXEMPTY_STE;
    break;
  }
#endif /* is_supertagging */

  return 1;
}

SXINT
alloc_ff_ste ()
{
#if is_supertagging
  sxprepare_for_possible_reset (ff_ste); /* semble inutile */
  ff_ste = (SXINT *) sxalloc (n+2, sizeof (SXINT));
#endif /* is_supertagging */

    return 1;
}

static void sxearley_raz (void);

#if is_supertagging && !is_recognizer
/* Cas ou` le supertagger est appele' directement depuis set_automaton_parser */
/* L'autre possibilite est que set_automaton_parser sert de guide au reconnaisseur earley
   qui lui appelle le supertagger */
SXBOOLEAN
sxearley_parse_it (void)
{
  SXINT i, item, prod;
  SXBA *lb2prod_item_set, supertagger_item_set, valid_suffix_item_set;

  sxearley_raz ();

#if MEASURES
  /* On est ds une campagne de mesures ... */
  printf ("MEASURES: There are " SXILD " input words which select " SXILD " terminal transitions.\n", last_tok_no, idag.ptq_nb);
#endif /* MEASURES */

#if has_Lex
  split_val = eof/2; /* Apres essais... */

  clause_val_set = sxba_calloc (last_clause+1);
  prod_checked_set = sxba_calloc (inputG_MAXPROD+1);
  prod_val_set = sxba_calloc (inputG_MAXPROD+1);

  process_Lex ();

  if (loop_clause_set)
    fill_loop_clause_set ();
#endif /* has_Lex */

  i2rl_supertagger_item_set = sxbm_calloc (n+1, inputG_MAXITEM+1);
  lb2prod_item_set = sxbm_calloc (n+1, inputG_MAXITEM+1);

  earley2init_supertagger (loop_clause_set);

  for (i = 1; i <= n; i++) {
    supertagger_item_set = i2rl_supertagger_item_set [i];

    item = 3;

    while ((item = sxba_scan (supertagger_item_set, item)) >= 0) {
	call_supertagger (item, i);
    }
  }

  /* On sort aussi les non-lexicalisees */
  valid_suffix_item_set = lb2prod_item_set [0];
  item = 3;

  while ((item = sxba_scan (valid_suffix_item_set, item)) > 0) {
    prod = PROLIS (item);

    if (item == PROLON (prod) && prod2t_nb [prod] == 0) {
      /* Les &Lex ont ete verifies */
	call_supertagger (item, 0);
    }

    item = PROLON (prod+1)-1;
  }

  sxbm_free (i2rl_supertagger_item_set);

  sxbm_free (lb2prod_item_set);

#if has_Lex
  sxfree (prod_checked_set), prod_checked_set = NULL;
  sxfree (prod_val_set), prod_val_set = NULL;
  sxfree (clause_val_set), clause_val_set = NULL;
#endif /* has_Lex */
    
  /* Pour ressortir le sdag reduit */
  print_glbl_out_source ();
  sxfree (ff_ste), ff_ste = NULL;

  if (is_print_time) {
    sxtime (TIME_FINAL, "\tDynamic Set Automaton");
  }
}

#else /* !(is_supertagging && !is_recognizer) */

static SXBOOLEAN      is_semact_fun, is_constraint_fun, is_prdct_fun, is_output_full_guide;
static SXBA	      *non_kernel_item_sets;

/* ATTENTION, il est possible que les probas calculees par nbest et le parseur beame' ne soient pas
   identiques dans le cas de recuperation d'erreur.
   En effet, les probas associees aux terminaux des chaines de correction ne sont pas identiques ds les 2 cas :
     nbest actuellement nulle
     parseur la proba min ds le source pour ce terminal
*/


/* unbeamed_first_pass déclenche une analyse en deux phases:
   1. reconnaisseur sans beam ni même calcul de probas puis reducer au cours duquel on calcule des probas right exactes, seulement sur des items grammaticalement valides
   2. reconnaisseur avec calcul des probas left et beam exact grâce à la phase 1, ces calculs n'étant effectués que sur des items "de la forêt" (qui n'est bien sûr pas construite encore) puis reducer standard */

#define recognizer_beam           1
#define reducer_beam              2
#define unbeamed_first_pass       4

#if beam_kind & unbeamed_first_pass
/* Dans ce cas, se succèdent une first_pass (1 et 2) et une second_pass (3 et 4):
  1. un recognizer normal, sans calcul de proba ni encore moins de beam : RECOGNIZER_KIND = 0
  2. un reducer appelé en mode "appel spécial" (argument booléen) qui calcule les probas right exactes (appel spécial non impacté par les #define)
  3. un recognizer "light" spécial non impacté par les #define; il calcule les probas left et beame tout le temps (? à vérifier)
  4. un reducer qui n'a plus besoin de beamer puisque le deuxième recognizer l'a fait (? à verifier) : REDUCER_KIND (0)
*/
# define RECOGNIZER_KIND (0) /* ni calculs de probas ni, a fortiori, beam */
# define REDUCER_KIND (0) /* le recognizer spécial (le deuxième recognizer) a tout beamé */
# define HAS_BEAM (2) /* beam en 4 étapes comme décrit ci-dessus */
#else /* beam_kind & unbeamed_first_pass */
# if beam_kind & reducer_beam
#   if beam_kind & recognizer_beam
#    define RECOGNIZER_KIND (2)
#    define REDUCER_KIND (2)
#    define HAS_BEAM (1) /* beam en 2 étapes (recognizer puis reducer) */
#   else /* beam_kind & recognizer_beam */
#    define RECOGNIZER_KIND (1) /* calculs seulement, pas de beam effectué */
#    define REDUCER_KIND (2)
#    define HAS_BEAM (1) /* beam en 2 étapes (recognizer puis reducer) */
#   endif /* beam_kind & recognizer_beam */
# else /* beam_kind & reducer_beam */
#   if beam_kind & recognizer_beam
#    define RECOGNIZER_KIND (2)
#    define REDUCER_KIND (0)
#    define HAS_BEAM (1) /* beam en 2 étapes (recognizer puis reducer) */
#   else /* beam_kind & recognizer_beam */
#    define RECOGNIZER_KIND (0)
#    define REDUCER_KIND (0)
#    define HAS_BEAM (0) /* pas de beam */
#   endif /* beam_kind & recognizer_beam */
# endif /* beam_kind & reducer_beam */
#endif /* beam_kind & unbeamed_first_pass */


#ifdef DYNAMIC_BEAM_MEASURES
#ifndef RECOGNIZER_KIND
#error DYNAMIC_BEAM_MEASURES requires RECOGNIZER_KIND to be equal to 2
#endif
#if RECOGNIZER_KIND != 2
#error DYNAMIC_BEAM_MEASURES requires RECOGNIZER_KIND to be equal to 2
#endif
#endif /* DYNAMIC_BEAM_MEASURES */

#if HAS_BEAM
#include <float.h>

#ifdef SXLOGPROB_IS_SXFLOAT
#define chouia    (1.0E-5)
static  SXFLOAT    d1_minus_d2;
static  SXBOOLEAN d1_less_d2;
#define cmp_SXLOGPROB(d1,d2)  ((d1_less_d2 = ((d1_minus_d2 = (d1)-(d2)) < 0)), \
			    (sxfabs(d1_minus_d2) <= chouia * sxfabs (d1)) ? 0 /* d1 == d2 */ : ((d1_less_d2) ? -1 /* d1 < d2 */: 1 /* d1 > d2 */) \
                           )
#else /* SXLOGPROB_IS_SXFLOAT */
#define cmp_SXLOGPROB(d1,d2) ((d1)-(d2))
#endif /* SXLOGPROB_IS_SXFLOAT */


#if defined(CONTROLLED_BEAM_H) || defined(BEAM_MEASURE)
static SXINT      *range2min_lgth;
static SXINT      controlled_beam_max_min_lgth, controlled_beam_max_final_state;

static SXINT
fill_range2min_lgth ()
{
  SXINT p, q, r, pp, pq, pr, qr, pr_lgth, old_pr_lgth, x;
  SXINT *pq_stack;
  SXBA pq_set;

  pq_stack = sxalloc (idag.max_range_value+1, sizeof (SXINT)), RAZ (pq_stack);
  pq_set = sxba_calloc (idag.max_range_value+1);

  range2min_lgth = sxcalloc (idag.max_range_value+1, sizeof (SXINT));

  for (p = idag.init_state; p <= idag.final_state; p++) {
    pp = p_q2range (p, p);
    PUSH (pq_stack, pp);
    SXBA_1_bit (pq_set, pp);
  }

  /* On simule la transition sur eof entre 0 et 1 */
  pq = p_q2range (0, 1);
  PUSH (pq_stack, pq);
  SXBA_1_bit (pq_set, pq);
  range2min_lgth [pq] = 1;

  x = 0;

  while (++x <= TOP (pq_stack)) {
    /* On parcourt la pile par valeur non decroissante de lgth */
    pq = pq_stack [x];
    SXBA_0_bit (pq_set, pq);
    p = range2p (pq);
    q = range2q (pq);
    pr_lgth = range2min_lgth [pq]+1;

    for (qr = idag.p2pq_hd [q]; qr < idag.p2pq_hd [q+1] && qr <= idag.last_pq; qr++) {
      r = idag.pq2q [qr];
      idag.p_q2range [p] [q] = pq;
      pr = p_q2range (p, r);

      old_pr_lgth = range2min_lgth [pr];

      if (old_pr_lgth == 0 || old_pr_lgth > pr_lgth) {
	range2min_lgth [pr] = pr_lgth;

	if (pr_lgth > controlled_beam_max_min_lgth)
	  controlled_beam_max_min_lgth = pr_lgth;

	if (SXBA_bit_is_reset_set (pq_set, pr))
	  PUSH (pq_stack, pr);
      }
    }
  }

  sxfree (pq_stack);
  sxfree (pq_set);
}
#endif /* CONTROLLED_BEAM_H || BEAM_MEASURE */

#ifdef CONTROLLED_BEAM_H
/* ESSAI avec des tables construites pour MICA (redb=1) */
#define CONTROLLED_BEAM_H_lgth
#define CONTROLLED_BEAM_H_global_table

/* Le controlled beam n'est reellement declanche que si le nombre de transitions de la phrase
   courante est superieure ou egale a terminal_transition_nb_trigger */
/* Sur MICA des essais ont montre que cette valeur est d'environ 290 */
#ifndef terminal_transition_nb_trigger
#define terminal_transition_nb_trigger 1
#endif /* terminal_transition_nb_trigger */

#if HAS_BEAM == 2
static SXBOOLEAN has_hit_a_valid_terminal_item;
#endif /* HAS_BEAM == 2 */

#include CONTROLLED_BEAM_H

/* On verifie si left_inside_logprob, la proba associee a (item_q, lgth (iq)) est compatible avec les
   probas controlees apprises sur corpus */
static SXBOOLEAN
check_controlled_beam_proof (SXINT item_q, SXINT iq, SXLOGPROB left_inside_logprob)
{
  SXINT     iq_lgth, prod, init_prod, init_item_q, couple;
  SXBOOLEAN ret_val = SXTRUE;

  if (item_q > 3) {
    /* OK sur la super-regle si on l'a atteinte */
    iq_lgth = range2min_lgth [iq];

    /* Si chaine trop grande entre i et q (non renseignee), elle a le benefice du doute */
    if (iq_lgth <= controlled_beam_max_min_lgth) {
#ifdef CONTROLLED_BEAM_H_lgth
      ret_val = cmp_SXLOGPROB (controlled_beam_lgth2logprob [iq_lgth], left_inside_logprob) <= 0;
#endif /* CONTROLLED_BEAM_H_lgth */

#ifdef CONTROLLED_BEAM_H_global_table
      if (ret_val) {
	/* On essaie d'etre + precis en utilisant la table globale */
#ifdef MAKE_INSIDEG
	prod = PROLIS (item_q);
	init_prod = spf.insideG.prod2init_prod [prod];
	init_item_q = prolon [init_prod] + item_q - PROLON (prod);
#else /* MAKE_INSIDEG */
	init_item_q = item_q;
#endif /* MAKE_INSIDEG */

	couple = XxY_is_set (&controlled_beam_global_table_hd, init_item_q, iq_lgth);

	ret_val = (couple == 0 /* non renseigne, benefice du doute */
		   ||
		   (couple <= controlled_beam_global_table_nb
		    && cmp_SXLOGPROB (controlled_beam_global_table2logprob [couple], left_inside_logprob) <= 0)
		   );
      }
#if LLOG
      else
	couple = 0;
#endif /* LLOG */
#endif /* CONTROLLED_BEAM_H_global_table */
    }
  }

#if LLOG
  if (item_q > 3) {
    printf ("check_controlled_beam_proof (item_q=" SXILD ", iq_lgth=" SXILD ", controlled_min_lgth=" SXILD ", left_inside_logprob=" SXLPD, item_q, iq_lgth, (SXINT)controlled_beam_max_min_lgth, SXLOGPROB_display (left_inside_logprob));
#ifdef CONTROLLED_BEAM_H_lgth
    printf (", controlled_lgth2logprob=" SXLPD, SXLOGPROB_display (controlled_beam_lgth2logprob [iq_lgth]));
#endif /* CONTROLLED_BEAM_H_lgth */
#ifdef CONTROLLED_BEAM_H_global_table
    printf ("(item_q, iq_lgth)=" SXILD ", controlled_left_inside_logprob=" SXLPD, couple, SXLOGPROB_display (controlled_beam_global_table2logprob [couple]));
#endif /* CONTROLLED_BEAM_H_global_table */
    printf (") %s\n", ret_val ? "SUCCEEDS" : "FAILS");
  }
#endif /* LLOG */
  
  return ret_val;
}
#endif /* CONTROLLED_BEAM_H */

#if HAS_BEAM == 2
static SXBOOLEAN saved_recognizer_scan_beam_value_is_set, saved_reducer_beam_value_is_set;
static SXBA      *i2valid_item_set;
#if is_rcvr
static SXBA      *i2valid_triple_set;
#endif /* is_rcvr */
#endif /* HAS_BEAM == 2 */
  
struct inside_logprob {
  SXLOGPROB left
#if REDUCER_KIND
  , right
#endif /* REDUCER_KIND */
    ;
};

struct logprob_struct {
  SXLOGPROB outside;
  struct inside_logprob inside;
  SXLOGPROB expected_total;
};


static XxY_header          ei_hd; /* ei_hd [A -> \alpha . \beta, range] */
static XxY_header          AxI_J_hd;
static struct logprob_struct *logprobs, nul_logprob; /* // a ei_hd */

#if REDUCER_KIND
static SXBA                ei_set, AxI_J_set;
#endif /* REDUCER_KIND */

static struct AxI_J_logprob_struct {
  SXLOGPROB inside;
#if REDUCER_KIND
  SXLOGPROB outside;
#endif /* REDUCER_KIND */
} *AxI_J_logprobs;


SXLOGPROB prod2logprobs [inputG_MAXPROD+1];

#ifdef MAKE_INSIDEG


static SXLOGPROB *nt2max_outside_logprob;

#if is_epsilon
static SXLOGPROB *nullable_nt2max_nullable_logprob;
#endif /* is_epsilon */

#else /* !MAKE_INSIDEG */


#if RECOGNIZER_KIND /* == 2 ? TODO */
static SXLOGPROB nt2max_outside_logprob [ntmax+1];
#else
static SXLOGPROB *nt2max_outside_logprob; /* pour des questions de visibilite a la compil */
#endif /* RECOGNIZER_KIND */

#if is_epsilon
static SXLOGPROB nullable_nt2max_nullable_logprob [ntmax+1];
#endif /* is_epsilon */

#endif /* !MAKE_INSIDEG */

static SXLOGPROB best_tree_logprob, min_acceptable_tree_logprob;

# if is_epsilon
/* Doit etre visible meme si RECOGNIZER_KIND == 0 */
static SXLOGPROB *item2nullable_max_prefix_logprob;
static SXLOGPROB **nt2nt_nullable_max_prefix_logprob;
# endif /* is_epsilon */

static	  SXLOGPROB *item2max_right_inside_logprob; /* visible tout le temps meme si pas alloue' */

#if RECOGNIZER_KIND
static	  SXLOGPROB **nt2nt_max_suffix_logprob;
#endif /* RECOGNIZER_KIND */

static SXINT                     ei, AxI_J;
static SXBOOLEAN                 is_AxI_J_set, is_ei_set;
static struct AxI_J_logprob_struct *max_logprob_ptr;
static struct logprob_struct       empty_ei_logprob;

#define ei2item(e)                   (XxY_X (ei_hd, e))
#define ei2rho(e)                    (XxY_Y (ei_hd, e))
#if LLOG
static void output_rcvr_rhs (SXINT litem, SXINT lmlstn, SXINT ritem, SXINT rmlstn);

static void
output_X (SXINT X)
{
  if (X < 0) {
    printf ("\"%s\"", TSTRING (-X));
  }
  else {
    printf ("<%s>", NTSTRING (X));
  }
}


static void
output_ei (SXINT cur_ei)
{
  SXINT     prod, rho, item_j, A, i, k, j, X, resume, item_k, triple;

  item_j = ei2item (cur_ei);
  prod = PROLIS (item_j);
  printf (SXILD ":", prod);

  A = LHS (prod);
  output_X (A);
  fputs (" = ", stdout);
  
  rho = ei2rho (cur_ei);
  i = rho2p (rho);
  item_k = PROLON (prod);
  k = i;
  j = rho2q (rho);

  output_rcvr_rhs (item_k, k, item_j, j);
 
  putchar ('\n');
}

static void
output_logprob_struct (struct logprob_struct *logprob_ptr)
{
#if REDUCER_KIND
  printf ("    logprob = {outside = " SXLPD ", inside.left = " SXLPD ", inside.right = " SXLPD ", expected_total = " SXLPD "}\n", SXLOGPROB_display (logprob_ptr->outside), SXLOGPROB_display (logprob_ptr->inside.left), SXLOGPROB_display (logprob_ptr->inside.right), SXLOGPROB_display (logprob_ptr->expected_total));
#else /* REDUCER_KIND */
  printf ("    logprob = {outside = " SXLPD ", inside.left = " SXLPD ", expected_total = " SXLPD "}\n", SXLOGPROB_display (logprob_ptr->outside), SXLOGPROB_display (logprob_ptr->inside.left), SXLOGPROB_display (logprob_ptr->expected_total));
#endif /* REDUCER_KIND */
}

static void
output_AxI_J_logprob_struct (struct AxI_J_logprob_struct *logprob_ptr)
{
#if REDUCER_KIND
  printf ("    AxI_J_logprob = {outside = " SXLPD ", inside = " SXLPD "}\n", SXLOGPROB_display (logprob_ptr->outside), SXLOGPROB_display (logprob_ptr->inside));
#else /* REDUCER_KIND */
  printf ("    AxI_J_logprob = {outside = " SXLPD "}\n", SXLOGPROB_display (logprob_ptr->outside));
#endif /* REDUCER_KIND */
}
#endif /* LLOG */

#if LLLOG
static SXBOOLEAN
make_ei (SXINT item_j, SXINT ij)
{
  is_ei_set = XxY_set (&ei_hd, item_j, ij, &ei);
  printf ("make_ei (item=" SXILD ", rho=" SXILD ") [ei=" SXILD " (%s)]: ", item_j, ij, ei, is_ei_set ? "old"  : "new");
  output_ei (ei);

  return is_ei_set;
}

#define ei2set(i,pq)                make_ei (i, pq)
#else /* LLLOG */
#define ei2set(i,pq)                 (is_ei_set = XxY_set (&ei_hd, i, pq, &ei))
#endif /* LLLOG */

#if LOG
#define ei2set_logprob_ptr(i,pq,ptr)   (ei2set (i, pq), ptr = &(logprobs [ei]), is_ei_set ? (struct logprob_struct*)NULL : (*ptr = empty_ei_logprob, ptr), is_ei_set)
#else /* LOG */
#define ei2set_logprob_ptr(i,pq,ptr)   (ei2set (i, pq), ptr = &(logprobs [ei]), is_ei_set)
#endif /* LOG */

#define get_ei_if_set(i,pq)          (ei = XxY_is_set (&ei_hd, i, pq))
#define ei2get_logprob_ptr(i,pq,ptr)   ((get_ei_if_set (i, pq)) ? ptr=&(logprobs [ei]) : (sxtrap (ME, "ei2get_logprob_ptr"), NULL))
#define prod2logprob(p)              (prod2logprobs [PROD (p)])

#define Axpq2set(A,pq)               (is_AxI_J_set = XxY_set (&AxI_J_hd, A, pq, &AxI_J))
#define Axpq2set_max_logprob_ptr(A,pq) (Axpq2set(A, pq)                            \
				      , max_logprob_ptr = &(AxI_J_logprobs [AxI_J])	\
				      , is_AxI_J_set			        \
				      )

#if LLOG
static void
output_Apq_logprobs (char *prefix, SXINT A, SXINT p, SXINT q, struct AxI_J_logprob_struct *max_logprob_ptr)
{
#if REDUCER_KIND
  printf ("%slogprobs (<%s>[" SXILD ".." SXILD "]) = {outside = " SXLPD ", inside = " SXLPD "}\n",
	  prefix, NTSTRING (A), p, q, SXLOGPROB_display (max_logprob_ptr->outside), SXLOGPROB_display (max_logprob_ptr->inside));
#else /* REDUCER_KIND */
  printf ("%slogprobs (<%s>[" SXILD ".." SXILD "]) = {inside = " SXLPD "}\n",
	  prefix, NTSTRING (A), p, q, SXLOGPROB_display (max_logprob_ptr->inside));
#endif /* REDUCER_KIND */
}
#endif /* LLOG */

#if LLOG
#define Axpq2get_max_logprob_ptr(A,pq)   ((AxI_J = XxY_is_set(&AxI_J_hd, A, pq))\
					, max_logprob_ptr = &(AxI_J_logprobs [AxI_J]) \
					, AxI_J ? 0 : sxtrap (ME, "Axpq2get_max_logprob_ptr") \
					, output_Apq_logprobs ("Get ", A, range2p (pq), range2q (pq), max_logprob_ptr) \
					)
#else /* LLOG */
#define Axpq2get_max_logprob_ptr(A,pq)   ((AxI_J = XxY_is_set(&AxI_J_hd, A, pq))\
                                          , max_logprob_ptr = &(AxI_J_logprobs [AxI_J])\
                                          , AxI_J ? 0 : sxtrap (ME, "Axpq2get_max_logprob_ptr")\
                                         )
#endif /* LLOG */


static void
ei_hd_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  logprobs = (struct logprob_struct *) sxrealloc (logprobs, new_size+1, sizeof (struct logprob_struct));
}


static void
AxI_J_hd_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  AxI_J_logprobs = (struct AxI_J_logprob_struct *) sxrealloc (AxI_J_logprobs, new_size+1, sizeof (struct AxI_J_logprob_struct));
}

#if HAS_BEAM == 2
#ifdef BEAM_MEASURE
static XxY_header controlled_beam_global_table, beam_measure_valid_table_hd;
static SXLOGPROB     *controlled_beam_global_table2logprob, *controlled_beam_min_lgth2logprob;

static void
controlled_beam_global_table_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  controlled_beam_global_table2logprob = (SXLOGPROB *) sxrealloc (controlled_beam_global_table2logprob, new_size+1, sizeof (SXLOGPROB));
}


static void
output_init_prod (FILE *outfile, SXINT prod)
{
  SXINT     A, top_item, bot_item, X;

  A = lhs [prod];

  fprintf (outfile, SXILD ":<%s> = ", prod, ntstring [A]);

  top_item = prolon [prod+1]-1;
  bot_item = prolon [prod];

  while (bot_item < top_item) {
    X = lispro [bot_item++];

    if (X < 0)
      fprintf (outfile, "\"%s\" ", tstring [-X]);
    else
      fprintf (outfile, "<%s> ", ntstring [X]);
  }

  fputs (";", outfile);
}

static void
controlled_beam_output ()
{
  SXINT lgth, top, couple, item, prod, x, prod_nb, top2;
  SXBA  init_prod_set;
  FILE  *outfile;

  if ((outfile = sxfopen (BEAM_MEASURE, "w")) == NULL) {
    fprintf (sxstderr, "%s: Cannot open (write) ", ME);
    sxperror (BEAM_MEASURE);
    return;
  }

  fprintf (outfile, "#define controlled_beam_max_min_lgth " SXILD "\n", controlled_beam_max_min_lgth);

  fputs ("#ifdef CONTROLLED_BEAM_H_lgth\n", outfile);

  fputs ("/* Function to be used for checking consistency between the definition of SXFLOAT \
   in the library version and in the compliled version of the tables below.\
   Returns SXTRUE if SXFLOAT is float, SXFALSE if it is double */\
SXINT\
get_controlled_beam_lgth2logprob_type (void) {\
  return SXFLOAT_TYPE_ID;\
}\n", stdout);

  fprintf (outfile, "static SXLOGPROB controlled_beam_lgth2logprob [" SXILD "] = {\n", controlled_beam_max_min_lgth+1);

  for (lgth = 0; lgth <= controlled_beam_max_min_lgth; lgth++)
    fprintf (outfile, "/* " SXILD " */ " SXLPD ",\n", lgth, SXLOGPROB_display (controlled_beam_min_lgth2logprob [lgth]));

  fputs ("};\n", outfile);

  fputs ("#endif /* CONTROLLED_BEAM_H_lgth */\n", outfile);

  fputs ("\n#ifdef CONTROLLED_BEAM_H_global_table\n", outfile);
  top = XxY_top (controlled_beam_global_table);
  fprintf (outfile, "#define controlled_beam_global_table_nb " SXILD "\n", top);
  top2 = XxY_top (beam_measure_valid_table_hd);

  for (x = 1; x <= top2; x++){
    item = XxY_X (beam_measure_valid_table_hd, x);
    lgth = XxY_X (beam_measure_valid_table_hd, x);

    /* On ajoute les items qui ont ete beam outes */
    XxY_set (&controlled_beam_global_table, item, lgth, &couple);
  }

  XxY_lock (&controlled_beam_global_table);
  XxY_to_c (&controlled_beam_global_table, outfile, "controlled_beam_global_table_hd", SXTRUE /* is_static */);

  fputs ("\n#if 0", outfile);

  init_prod_set = sxba_calloc (prodmax+1);

  for (x = top; x > 1; x--) {
    item = XxY_X (controlled_beam_global_table, x);
    prod = prolis [item];
    SXBA_1_bit (init_prod_set, prod);
  }

  prod_nb = sxba_cardinal (init_prod_set);

  fprintf (outfile, "\nVALID INITIAL GRAMMAR PRODUCTION'S LIST (" SXILD "/" SXILD ") with their LOGPROB's\n", prod_nb, prodmax);

  prod = 0;

  while ((prod = sxba_scan (init_prod_set, prod)) > 0) {
    fprintf (outfile, "[" SXILD ".." SXILD "]", prolon [prod], prolon [prod+1]-1);
    output_init_prod (outfile, prod);
    fprintf (outfile, " " SXLPD "\n", SXLOGPROB_display (prod2logprobs [prod]));
  }

#if 0
  if (prodmax-prod_nb > 0) {
    fprintf (outfile, "\nINITIAL GRAMMAR UNUSED PRODUCTION'S LIST (" SXILD ")\n", prodmax-prod_nb);
    prod = 0;

    while ((prod = sxba_0_lrscan (init_prod_set, prod)) > 0) {
      fprintf (outfile, "[" SXILD ".." SXILD "]", prolon [prod], prolon [prod+1]-1);
      output_init_prod (outfile, prod);
      fprintf (outfile, " " SXLPD "\n", SXLOGPROB_display (prod2logprobs [prod]));
    }
  }
#endif /* 0 */

  fputs ("#endif /* 0 */\n\n", outfile);
  
  fprintf (outfile, "static SXLOGPROB controlled_beam_global_table2logprob [" SXILD "] = {0,\n", top+1);

  for (couple = 1; couple <= top; couple++)
    fprintf (outfile, "/* " SXILD " = (" SXILD ", " SXILD ") */ " SXLPD ",\n", couple, XxY_X (controlled_beam_global_table, couple), XxY_Y (controlled_beam_global_table, couple), SXLOGPROB_display (controlled_beam_global_table2logprob [couple]));

  fputs ("};\n", outfile);
  fputs ("#endif /* CONTROLLED_BEAM_H_global_table */\n", outfile);

  sxfree (init_prod_set);

  if (is_print_time) {
    char mess [64];
      
    sprintf (mess, "\twriting %s:", BEAM_MEASURE);
    sxtime (TIME_FINAL, mess);
  }
}
#endif /* BEAM_MEASURE */
#endif /* HAS_BEAM == 2 */

#endif /* HAS_BEAM */

static SXBA T2_kernel_nt_set;

#if RECOGNIZER_KIND == 2
static SXBA *kernel_nt_sets;
static struct earley_table_companion {
  SXLOGPROB max_global_expected_logprob;
  SXLOGPROB dynamic_scan_beam_value;
  SXLOGPROB dynamic_shift_beam_value;
  SXLOGPROB dynamic_scan_beam_first_beamed_logprob;
  SXLOGPROB dynamic_shift_beam_first_beamed_logprob;
  SXINT beamrcvr_state;
  SXINT dynamic_beam_increment_nb;
  SXBOOLEAN did_scan_beam;
  SXBOOLEAN did_shift_beam;
  SXBOOLEAN no_more_beamrcvr_possible;
} *earley_table_companion;
#endif /* RECOGNIZER_KIND == 2 */


#if RECOGNIZER_KIND || (HAS_BEAM && is_rcvr)
static SXLOGPROB *nt2max_inside_logprob, *t2max_inside_logprob;
#if is_rcvr
static SXINT *nt2prod_that_generates_the_best_logprob_t_string;

static SXLOGPROB
get_max_inside_rcvr_logprob (SXINT left_item, SXINT right_item)
{
  SXLOGPROB max_inside_logprob = 0;
  SXINT  X;

  while (left_item < right_item) {
    X = LISPRO (left_item);

    if (X < 0)
      max_inside_logprob += t2max_inside_logprob [-X];
    else
      max_inside_logprob += nt2max_inside_logprob [X];

    left_item++;
  }

  return max_inside_logprob;
}
#endif /* is_rcvr */
#endif /* RECOGNIZER_KIND || (HAS_BEAM && is_rcvr) */

#if MEASURES || LOG
static SXBA used_prod_set;
extern void output_G (char *header, SXBA basic_item_set, SXBA basic_prod_set);
#endif /* MEASURES || LOG */

/* Connu meme si !is_rcvr */
static SXBOOLEAN               is_error_detected;




#ifdef MAKE_INSIDEG
static struct recognize_item {
  SXINT		**shift_NT_hd;
  SXINT		*items_stack;
  SXBA	        *index_sets;
} *RT;	
static char *RT_area; 
#else /* !MAKE_INSIDEG */
static struct recognize_item {
    SXINT	*shift_NT_hd [inputG_MAXNT + 1];
    SXINT	items_stack [inputG_MAXITEM + 1];
    SXBA	index_sets [inputG_MAXITEM + 1];
} *RT;	
#endif /* !MAKE_INSIDEG */

#if RECOGNIZER_KIND == 2
struct RT_logprobs {
  SXLOGPROB         max_global_logprob;
  SXINT         offset_in_global_logprobs;
};
static struct RT_global_logprobs {/* indexé par le numéro de table Earley */
  struct RT_logprobs *RT_logprobs; /* indexé par un index d'item */
  SXLOGPROB           *global_logprobs; /* à parcourir en parallèle avec l'index de début d'item et en partant de l'offset donné par RT_logprobs->offset_in_global_logprobs */
  SXINT            global_logprobs_allocated_size;
  SXINT            global_logprobs_top;
} *RT_global_logprobs;
static struct RT_logprobs *RT_global_logprobs_area;
#endif /* RECOGNIZER_KIND == 2 */

static SXBOOLEAN
is_in_reduce (bot, top, item)
     SXINT *bot, *top, item;
{
  while (--top >= bot) {
    if (*top == item) {
      return SXTRUE;
    }
  }

  return SXFALSE;
}

static SXBA             T2_look_ahead_t_set;
static SXINT		**T2_shift_NT_hd;
static SXINT		*T2_items_stack;
/* static SXINT		*T1_shift_state_stack, *T2_shift_state_stack; */
static SXBA             *T2_index_sets;
/* static SXINT		*T1_kernel_NT_stack, *T2_kernel_NT_stack; */
static SXBA		T2_non_kernel_item_set;
#if is_epsilon && is_rcvr
static SXBA		non_kernel_empty_item_set;
#endif /* is_epsilon && is_rcvr */

static SXBA		working_index_set;

static SXBA		T2_left_corner;
static SXBA		*RT_left_corner;

static bag_header	shift_bag;

static SXBA		*ntXindex_set;

static SXBA reduce_set, Bpq_set;
static SXINT *reduce_list, *Bpq_list;
#if HAS_BEAM == 1
static SXBA Bpq_list_set;
#endif /* HAS_BEAM == 1 */


#if is_parser
#include "SS.h"

/* Ajoute le Jeu 12 Avr 2001 14:28:26: La version "SXINT" a debordee sur l'analyse d'une phrase
   de 64 tokens avec la grammaire de L&H ft.bnf!! */
static	SXUINT	xprod;

static SXBA             *T2_backward_index_sets;

/* on peut faire de la recup de la forme 
   A -> \alpha [h] [k] \beta
   qui permet au vide de reconnaitre du source a l'interieur ou aux extremites d'une prod !!*/
#ifdef MAKE_INSIDEG
static SXINT		*rhs_stack;
#else /* !MAKE_INSIDEG */
static SXINT		rhs_stack [2*inputG_MAXRHSLGTH+3]; /* prod i j rhs_lgth*(p q) */
#endif /* !MAKE_INSIDEG */


#ifdef MAKE_INSIDEG
static struct parse_item {
  SXBA		*backward_index_sets;
  SXINT		**reduce_NT_hd;
} *PT;
static char *PT_area; 
#else /* !MAKE_INSIDEG */
static struct parse_item {
  SXBA		backward_index_sets [inputG_MAXITEM + 1];
  SXINT		*reduce_NT_hd [inputG_MAXNT + 1];
} *PT;
#endif /* !MAKE_INSIDEG */


static SXINT		**T2_reduce_NT_hd;

static bag_header	pt2_bag;

/* On peut diminuer la taille de shift_NT_stack : nb d'occurrences de NT en RHS */
#ifdef MAKE_INSIDEG
static struct PT2_item {
  SXINT		*shift_NT_stack;
  SXBA		*index_sets;
  SXBA		*backward_index_sets;
} *PT2, *PT2_save;
static char *PT2_area, *PT2_area_save; 
#else /* !MAKE_INSIDEG */
static struct PT2_item {
    SXINT		shift_NT_stack [inputG_MAXITEM - inputG_MAXPROD + 1];
    SXBA		index_sets [inputG_MAXITEM + 1];
    SXBA		backward_index_sets [inputG_MAXITEM + 1];
} *PT2, *PT2_save;
#endif /* !MAKE_INSIDEG */



static void 
alloc_PT2 (void)
{
#ifdef MAKE_INSIDEG
  {
    SXINT           sizeof_shift_NT_stack, sizeof_backward_index_sets, sizeof_index_sets, i;
    struct PT2_item *PT2i;
    char            *area_ptr;

    sizeof_shift_NT_stack = (MAXITEM-MAXPROD+1) * sizeof (SXINT);
    sizeof_backward_index_sets = (MAXITEM+1) * sizeof (SXBA);
    sizeof_index_sets = (MAXITEM+1) * sizeof (SXBA);
    PT2 = (struct PT2_item*) sxcalloc (1+n+1, sizeof (struct PT2_item));
    area_ptr = PT2_area = (char*) sxcalloc (1+n+1, sizeof_shift_NT_stack+sizeof_backward_index_sets+sizeof_index_sets);

    for (i = n+1; i >= 0; i--) {
      PT2i = &(PT2 [i]);
      PT2i->shift_NT_stack = (SXINT*) area_ptr;
      area_ptr += sizeof_shift_NT_stack;
      PT2i->index_sets = (SXBA*) area_ptr;
      area_ptr += sizeof_backward_index_sets;
      PT2i->backward_index_sets = (SXBA*) area_ptr;
      area_ptr += sizeof_index_sets;
    }
  }
#else /* !MAKE_INSIDEG */
  PT2 = (struct PT2_item*) sxcalloc (1+n+1, sizeof (struct PT2_item));
#endif /* !MAKE_INSIDEG */
}

static void
free_PT2 (void)
{
  if (PT2) sxfree (PT2), PT2 = NULL;

#ifdef MAKE_INSIDEG
  if (PT2_area) sxfree (PT2_area), PT2_area = NULL;
#endif /* MAKE_INSIDEG */
}

static void
free_PT2_save (void)
{
  if (PT2_save) sxfree (PT2_save), PT2_save = NULL;

#ifdef MAKE_INSIDEG
  if (PT2_area_save) sxfree (PT2_area_save), PT2_area_save = NULL;
#endif /* MAKE_INSIDEG */
}


static SXBA	RL_nt_set, prod_order_set;
static SXINT    *prod_order_stack;
static SXBA	*ap_sets;
static SXBA     *red_order_sets;

static void parse_item (SXINT item_k, SXINT *Aij_stack, SXBA i_set, SXINT i, SXINT k, SXINT j);
static void parse ();
#endif /* is_parser */

#if is_parser && is_rcvr
static struct parse_item empty_PT;
#endif /* is_parser && is_rcvr */


#if HAS_BEAM
static SXBOOLEAN set_forward_logprob (SXINT i, SXINT p, SXINT q, SXINT item_p, SXINT item_q, SXINT X, SXINT *pq_ptr, SXLOGPROB *X_inside_logprob_ptr);
#endif /* HAS_BEAM */

#if RECOGNIZER_KIND || (HAS_BEAM && is_rcvr)
static void fill_tnt_max_inside_logprobs (SXBOOLEAN fill_item2max_right_inside_logprob);
#endif /* RECOGNIZER_KIND || (HAS_BEAM && is_rcvr) */
#if is_parser || is_rcvr
static SXBOOLEAN IS_AND (SXBA lhs_bits_array, SXBA rhs_bits_array);
#endif /* is_parser || is_rcvr */

static SXBOOLEAN OR (SXBA lhs_bits_array, SXBA rhs_bits_array);

#if LOG
static void output_ni_prod (SXINT prod, SXBA lb_set, SXINT i, SXINT j);
static void output_table (SXINT p);
#endif /* LOG */

#if RECOGNIZER_KIND
static void set_non_kernel_item_logprobs_recognizer (SXINT p);
#endif /* RECOGNIZER_KIND */
#if REDUCER_KIND
static void set_non_kernel_item_logprobs_reducer (SXINT p);
#endif /* REDUCER_KIND */

#if LOG || LLOG || EBUG
static void output_rcvr_prod (SXINT litem, SXINT lmlstn, SXINT ritem, SXINT rmlstn);
#endif /* LOG || LLOG || EBUG */

#if HAS_BEAM == 1
/* i.e., #if RECOGNIZER_KIND */
static SXBOOLEAN beam_shift (SXINT A, SXINT p, SXINT q);
#endif /* HAS_BEAM == 1 */
static SXBOOLEAN shift (SXINT A, SXINT p, SXINT q);

static void      scan_reduce (SXINT p, SXINT q);
static void      set_T2 (SXINT q);
static void      process_non_kernel_filtering (SXINT q);
static void      process_non_kernel (SXINT q);

#if is_rcvr
static void clear_table (SXINT p);
static void copy_table (SXINT p, SXINT q);

static SXBA             rcvr_xcld_min_mlstn_set;
static SXBA             *rcvr_reduce_item_sets;

/* global recovery */
static VARSTR varstring;

/* Si une erreur est detectee, quand on reevalue une table, on ne le fait que sur les nouveaux items */
static struct recognize_item empty_RT;
static SXINT                   rcvr_on_super_rule_stack [16]; /* au plus 3 quintuplets */
static SXBOOLEAN               is_potential_orphan_rule, is_rcvr_can_be_cyclic, is_rcvr_on_site;

static SXBOOLEAN      is_during_error_rcvr_processing, is_during_reduce_rcvr_validation, has_hit_a_valid_kernel_terminal_item;
static SXBA	      *store_non_kernel_item_sets, T2_store_non_kernel_item_set;
static SXBOOLEAN      no_la_check;

static SXINT          *rcvr_reduce_item_stack;
static SXBA           rcvr_store_unfiltered_non_kernel_item_set;
static SXBA           rcvr_w2titem_set, rcvr_reduce_T2_non_kernel_item_set, rcvr_prod_set;
static SXBA           rcvr_w2item_set;
static SXBA           rcvr_non_kernel_item_set;

static SXBA           *rcvr_kernel_item_sets;

#if is_parser
/* recovery ds le cas reconnaisseur */
static XxY_header   glbl_dag_rcvr_resume_kitems;
static SXBA         glbl_dag_rcvr_resume_k_set, glbl_dag_rcvr_resume_item_set;
static XxYxZ_header glbl_dag_rcvr_start_kitems;
static SXINT        glbl_dag_rcvr_start_kitems_foreach [] = {0, 0, 1, 0, 0, 0}; /* Zforeach */
static SXBA         glbl_dag_rcvr_start_kitems_set;
static XxYxZ_header store_rcvr_string_hd;
static SXINT        store_rcvr_string_hd_foreach [] = {0, 0, 0, 0, 0, 1}; /* YZforeach */

#define get_rcvr_resume(j,it)   ((is_error_detected && XxY_is_allocated (glbl_dag_rcvr_resume_kitems)\
				  && SXBA_bit_is_set (glbl_dag_rcvr_resume_k_set, j) && SXBA_bit_is_set (glbl_dag_rcvr_resume_item_set, it))\
				 ? XxY_is_set (&glbl_dag_rcvr_resume_kitems, j, it) \
				 : 0)

/* Tentative pour ne conserver qu'une seule recuperation par point de reprise */
/* Echec, car il se peut qu'une recuperation se fasse sur des items produits par une tentative de recuperation precedente.
   Ces items peuvent faire partie d'une recup qui a reussie ou echoue' et qui a ou non le meme point de reprise que l'essai courant */
#endif /* is_parser */


#include "sxstack.h"

static SXINT        best_repair, best_distance;
static XxY_header XxY_gen_repair_hd;
static SXINT        *gen_tnt_repair_stack, *gen_valid_repair_string_stack, *repair_path_stack;
static XxY_header gen_tntXmaxlgth_repair_hd;
static SXINT        *gen_proto_stack; /* DSTACK */
static SXBA       gen_tntXmaxlgth_repair_set;
static SXINT        *local_repair_prod_stack, *glbl_repair_prod_stacks;
static SXBOOLEAN    gen_empty_repair_string;
static SXBA       repair_gen_nyp;
static struct     gen_repair2attr_struct {
  SXINT  i, delta_i, delta_j, anchor, distance;
  SXBA anchor_set;
}                 *gen_repair2attr;
static struct     gen_repair2attr2_struct {
  SXINT  nb, prod_stack;
}                 *gen_repair2attr2;
static bag_header repair_bag;

static SXBA       sub_dag_t_set, sub_dag_state_set, sub_dag_state_set2, *sub_dag_R, *sub_dag_R_plus;
static SXINT        sub_dag_delta_init;
static VARSTR     sub_dag_vstr;

static SXBOOLEAN generate ();

static void
XxY_gen_repair_hd_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  if (rcvr_spec.repair_kind != 1) {
    gen_repair2attr = (struct gen_repair2attr_struct*) sxrealloc (gen_repair2attr, new_size+1, sizeof (struct gen_repair2attr_struct));
    repair_gen_nyp = sxba_resize (repair_gen_nyp, new_size+1);
  }

  if (rcvr_spec.perform_repair_parse) {
    gen_repair2attr2 = (struct gen_repair2attr2_struct*) sxrealloc (gen_repair2attr2, new_size+1, sizeof (struct gen_repair2attr2_struct));
  }
}

static void
gen_tntXmaxlgth_repair_hd_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  gen_tntXmaxlgth_repair_set = sxba_resize (gen_tntXmaxlgth_repair_set, new_size+1);
}


/* suivant la valeur de kind, calcule un couple (m, n) de mlstn a partir du couple precedent (q, r)
   p est le mlstn sur lequel l'erreur est detectee.
   Pour l'instant on a soit m==p==n soit m<=p<n
   Si for_parsact.rcvr_range_walk, on appelle une fonction de l'utilisateur
   retourne SXFALSE ssi aucun nouveau range ne peut etre calcule'
*/
static SXINT delta;

static SXBOOLEAN
range_walk (p, q, r, m, n)
     SXINT p, q, r, *m, *n;
{
  SXINT      init_state, final_state;
  SXBOOLEAN    ret_val;

  init_state = idag.init_state;
  final_state = idag.final_state;

  if (q == init_state && r == final_state)
    return SXFALSE;

  if (for_semact.rcvr_range_walk)
    ret_val = (*for_semact.rcvr_range_walk) (p, q, r, m, n);
  else {
    if (q == 0 && r == 0) {
      /* initialisation [p..p] */
      *m = *n = p;
      delta = 0;
    }
    else {
      switch (rcvr_spec.range_walk_kind) {
      case FORWARD_RANGE_WALK_KIND :
	/*
	  priorite a ce qui a deja ete analyse', on saute plutot du source
	  [p..p] [p..p+1] ... [p..n]
	  [p-1..p+1] ... [p-1..n]
	  ...
	  [1..p+1] ... [1..n]
	*/
	if (r == final_state) {
	  *m = q-1;
	  *n = p+1;
	}
	else {
	  *m = q;
	  *n = r+1;
	}
    
	break;

      case BACKWARD_RANGE_WALK_KIND :
	/*
	  priorite au source non analyse', on remet en cause les prefixes corrects deja analyses 
	  [p..p]
	  [p..p+1] [p-1..p+1] ... [1..p+1]
	  ...
	  [p..n] ... [1..n]
	*/
	if (q == init_state || r == p) {
	  *m = p;
	  *n = r+1;
	}
	else {
	  *m = q-1;
	  *n = r;
	}

	break;

      case MIXED_FORWARD_RANGE_WALK_KIND :
	/*
	  On essaie toutes les combinaisons telles que m-n == delta
	  en partant (par valeurs decroissantes) de [p..p+delta] 
	  [p..p]
	  [p..p+1]
	  [p..p+2] [p-1..p+1]
	  ...
	  [p..p+delta] ... [p-delta+1..p+1]
	  ...
	*/
	if (r > p+1 && q > init_state) {
	  /* on reste ds l'etage courant */
	  *n = r-1;
	}
	else {
	  /* On a fini un etage */
	  delta++;

	  *n = p+delta;

	  if (*n >= final_state)
	    *n = final_state;
	}

	*m = *n - delta;

	break;

      case MIXED_BACKWARD_RANGE_WALK_KIND :
	/*
	  On essaie toutes les combinaisons telles que m-n == delta
	  en partant (par valeurs croissantes) de [p-delta..p] 
	  [p..p]
	  [p-1..p] [p..p+1] 
	  [p-2..p] [p-1..p+1] [p..p+2]
	  ...
	  [p-delta..p] ... [p..p+delta]
	  ...
	*/
	if (q < p && r < final_state) {
	  /* on reste ds l'etage courant */
	  *m = q+1;
	}
	else {
	  /* On a fini un etage */
	  delta++;

	  *m = p-delta;

	  if (*m < init_state)
	    *m = init_state;
	}

	*n = *m + delta;
	break;
      }
    }

    ret_val = SXTRUE;
  }
    
#if EBUG
  if (*m < init_state || *n > final_state)
    sxtrap (ME, "range_walk");
#endif /* EBUG */

  
  if (*m == p && rcvr_detection_table_cleared)
    /* p a ete raze' ... */
    /* ... on la restore */
    copy_table (p, working_rcvr_mlstn);

#if LOG
  printf ("**** range_walk = [" SXILD ".." SXILD "] ****\n", *m, *n);
#endif /* LOG */

  return ret_val;
}




/* copie (en sens inverse) ds glbl_repair_prod_stacks le contenu de local_repair_prod_stack, liste des productions utilisees pour generer
   la chaine courante.  L'identifiant de cette "analyse" sera stockee en // avec gen_valid_repair_string_stack */
static void
copy_local_repair_prod_stack (prefix)
     SXINT prefix;
{
  SXINT top, id, prev_id;
  SXINT *glbl_prod_stack;

  top = DTOP (local_repair_prod_stack);
  DCHECK (glbl_repair_prod_stacks, top+2);

  DSPUSH (glbl_repair_prod_stacks, 0); /* taille */
  id = DTOP (glbl_repair_prod_stacks);
  glbl_prod_stack = glbl_repair_prod_stacks + id;
  DTOP (glbl_repair_prod_stacks) += top+1;

  DSPUSH (glbl_prod_stack, 0); /* lien vers un autre arbre */

  while (top) {
    DSPUSH (glbl_prod_stack, local_repair_prod_stack [top]);
    top--;
  }

  prev_id = gen_repair2attr2 [prefix].prod_stack;
  glbl_prod_stack [1] = prev_id; /* Lien */
  gen_repair2attr2 [prefix].prod_stack = id;
}



#if HAS_BEAM
    /* Dans ce cas, on genere la chaine de proba max */
    /* Le travail est deja pre'mache' car nt2max_inside_logprob, t2max_inside_logprob et
       nt2prod_that_generates_the_best_logprob_t_string sont calcule; */
static void
generate_best_logprob_string (SXINT A, SXINT *prefix_ptr, SXINT *gen_lgth_ptr)
{
  SXINT item, X, prod = nt2prod_that_generates_the_best_logprob_t_string [A];

  if (rcvr_spec.perform_repair_parse) DPUSH (local_repair_prod_stack, prod);

  item = PROLON (prod);

  while ((X = LISPRO (item++)) != 0) {
    if (X < 0) {
      XxY_set (&XxY_gen_repair_hd, -X, *prefix_ptr, prefix_ptr);
      *gen_lgth_ptr += 1;
    }
    else
      generate_best_logprob_string (X, prefix_ptr, gen_lgth_ptr);
  }
}


static void
call_generate_best_logprob_string (SXINT litem, SXINT ritem, SXINT *gen_lgth_ptr)
{
  SXINT  X, new_prefix = 0;

  *gen_lgth_ptr = 0;

  while (litem < ritem) {
    X = LISPRO (litem);

    if (X < 0) {
      XxY_set (&XxY_gen_repair_hd, -X, new_prefix, &new_prefix);
      *gen_lgth_ptr += 1;
    }
    else {
      generate_best_logprob_string (X, &new_prefix, gen_lgth_ptr);
    }

    litem++;
  }

  if (new_prefix)
    DPUSH (gen_valid_repair_string_stack, new_prefix);
  else
    gen_empty_repair_string = SXTRUE;

  if (perform_copy_local_repair_prod_stack) {
    gen_repair2attr2 [new_prefix].prod_stack = 0; /* chaine corrigee unique */
    copy_local_repair_prod_stack (new_prefix);
  }

  if (rcvr_spec.perform_repair_parse)
    DTOP (local_repair_prod_stack) = 0;
}
#endif /* HAS_BEAM */

/* Nelle version /* 11/12/06
   Test de cycles grace a gen_tntXmaxlgth_repair_hd et gen_tntXmaxlgth_repair_set.  Les elements sont des couples
   (A, max_lgth) ou max_lgth est la taille maximale de la chaine terminale qui doit etre generee par A.
   Si on retombe sur (A, max_lgth) au cours du traitement d'un (A, max_lgth) => cycle (return SXFALSE) */
/* min_proto est la taille minimale des phrases generees par la protophrase contenue ds gen_proto_stack (on a min_proto <= lgth) */
static SXBOOLEAN
call_generate (bot_item, top_item, old_prefix, lgth, kind, min_proto)
     SXINT     bot_item, top_item, old_prefix, lgth, kind, min_proto;
{
  SXINT     X, gen_lgth, cur_item;
  SXBOOLEAN ret_val;

  ret_val = SXFALSE;

  if (top_item > bot_item) {
    /* On verifie que ca peut marcher */
    gen_lgth = min_proto;
    cur_item = top_item;

    while (--cur_item >= bot_item) {
      X = LISPRO (cur_item);

      if (X < 0)
	gen_lgth++;
      else
	gen_lgth += NT2MIN_GEN_LGTH (X);

      if (gen_lgth > lgth)
	return SXFALSE;
    }

    cur_item = top_item;

    DCHECK (gen_proto_stack, top_item-bot_item);

    while (--cur_item >= bot_item) {
      X = LISPRO (cur_item);

      if (X < 0)
	min_proto++;
      else
	min_proto += NT2MIN_GEN_LGTH (X);

      DSPUSH (gen_proto_stack, X);
    }

    ret_val = generate (old_prefix, lgth, kind, min_proto);

    /* Remise en etat */
    gen_proto_stack [0] -= (top_item-bot_item);
  }
  else {
    ret_val = generate (old_prefix, lgth, kind, min_proto);
  }

  return ret_val;
}

#define X80 (~(((SXUINT)~0)>>1))
#define X40 (X80>>1)

/* gen_proto_stack contient la protophrase a generer sur la longueur lgth si kind == 0 ou <= lgth si kind == 1 . 
   min_proto contient la taille min du genere' de cette protophrase.
   old_prefix est l'identifiant du prefixe de la chaine generee jusqu'ici */
static SXBOOLEAN
generate (old_prefix, lgth, kind, min_proto)
     SXINT     old_prefix, lgth, kind, min_proto;
{
  SXINT     X, X_max, new_prefix, item, prod, top_item, bot, top, x, size, parse_nb;
  SXBOOLEAN is_new, ret_val;

  
  if (IS_EMPTY (gen_proto_stack)) {
    /* fin d'une expansion */
    if (ret_val = (kind != 0 || lgth == 0)) {
      /* Le vide est une bonne chaine */
      /* On ne la met pas ds gen_valid_repair_string_stack car elle pourrait se calculer de plusieurs fac,ons */
      /* ... ou ne pas etre bonne (kind == 0 && lgth > 0) */
      if (old_prefix == 0)
	gen_empty_repair_string = SXTRUE;

      if (rcvr_spec.perform_repair_parse && perform_copy_local_repair_prod_stack) {
	copy_local_repair_prod_stack (old_prefix);
      }
    }
  }
  else {
    /* gen_proto_stack est non vide */
    ret_val = SXFALSE;
    X = DPOP (gen_proto_stack);

    if (X < 0) {
      /* terminal */
      if (lgth > 0) {
	/* A FAIRE : tenir compte de i = rcvr_spec.repair_parse_kind, nombre d'analyses par correction (0 => toutes ou i) */
	/* Ne pas utiliser is_new + chainer les copies ds copy_local_repair_prod_stack */
	/* Pour l'instant, on fait comme si rcvr_spec.repair_parse_kind == 1 */
	is_new = !XxY_set (&XxY_gen_repair_hd, -X, old_prefix, &new_prefix);

	if (/* (LE 29/12/06) IS_EMPTY (gen_proto_stack) || */min_proto == 1) {
	  /* Le fond de pile peut generer la chaine vide ... */
	  /* On note le nb de fois ou` la chaine "new_prefix" a ete generee */
	  if ((kind == 1 /* |chaines| <= lgth */ || lgth == 1)) {
	    /* On vient de trouver comment generer une chaine de la bonne longueur */
	    if (rcvr_spec.perform_repair_parse) {
	      if (is_new) {
		parse_nb = gen_repair2attr2 [new_prefix].nb = 1;
		gen_repair2attr2 [new_prefix].prod_stack = 0; /* init */
	      }
	      else {
		parse_nb = ++gen_repair2attr2 [new_prefix].nb;
	      }
	    }

	    if (is_new) {
	      /* ... pour la 1ere fois */
	      DPUSH (gen_valid_repair_string_stack, new_prefix);
	      ret_val = DTOP (gen_valid_repair_string_stack) == rcvr_spec.repair_kind /* nb max de chaines generees (ou 0 si toutes) */;
	    }

	    /* en // avec gen_valid_repair_string_stack */
	    if (rcvr_spec.perform_repair_parse && (rcvr_spec.repair_parse_kind == 0 /* toutes les analyses ... */
						   || parse_nb <= rcvr_spec.repair_parse_kind /* ... ou y'en manque */)) {
	      /* On appelle quand meme generate pour stocker les analyses de la chaine vide */
	      ret_val = generate (new_prefix, lgth-1, kind, min_proto-1);

#if EBUG
	      if (!ret_val)
		sxtrap (ME, "generate");
#endif /* EBUG */

#if 0
	      /* le 29/12/06 */
	      if (!IS_EMPTY (gen_proto_stack))
		call_generate (item /* bidon */, item /* bidon */, new_prefix, lgth-1, kind, min_proto-1);
	      else
		copy_local_repair_prod_stack (new_prefix);
#endif /* 0 */
	    }

	    ret_val = SXTRUE;
	  }
	}
	else
	  ret_val = generate (new_prefix, lgth-1, kind, min_proto-1);
      }
    }
    else {
      /* nt ou marqueur*/
      if (X & X40) {
	/* marqueur */
	X_max = X & ~X40;
	/* On vient de generer un X_max */
	/* On le dit */
	SXBA_0_bit (gen_tntXmaxlgth_repair_set, X_max);
	ret_val = generate (old_prefix, lgth, kind, min_proto);
	/* On remet en etat */
	SXBA_1_bit (gen_tntXmaxlgth_repair_set, X_max);
      }
      else {
	/* X est un nt */
	/* On regarde toutes les X-prods */
	min_proto -= NT2MIN_GEN_LGTH (X);

	XxY_set (&gen_tntXmaxlgth_repair_hd, X, lgth-min_proto /* taille max de X */, &X_max);

	if (SXBA_bit_is_reset_set (gen_tntXmaxlgth_repair_set, X_max)) {
	  /* C'est la 1ere fois qu'on traite X_max ds l'appel courant */
	  DPUSH (gen_proto_stack, X_max+X40); /* On le note */

	  for (top = NPG (X+1), bot = NPG (X); bot < top; bot++) {
	    prod = NUMPG (bot);

	    if (rcvr_spec.perform_repair_parse) DPUSH (local_repair_prod_stack, prod);

	    item = PROLON (prod);
	    top_item = PROLON (prod+1)-1;
      
	    if (call_generate (item, top_item, old_prefix, lgth, kind, min_proto)) {
	      ret_val = SXTRUE;
	    }

	    if (rcvr_spec.perform_repair_parse) DTOP (local_repair_prod_stack)--;

	    if (ret_val)
	      break;
	  }

	  /* On sort de la generation de X_max */
	  SXBA_0_bit (gen_tntXmaxlgth_repair_set, X_max);

#if EBUG
	  if (DPOP (gen_proto_stack) != X_max+X40)
	    sxtrap (ME, "generate");
#else /* !EBUG */
	  DPOP (gen_proto_stack);
#endif    /* !EBUG */   
	}
	/* else : cycle on retourne faux */
      }
    }

    DSPUSH (gen_proto_stack, X);
  }

  return ret_val;
}


/* si ai ... ap est la chaine generee stockee ds XxY_gen_repair_hd
   lrf est appele sur chaque symbole ds l'ordre lrf(a1), lrf (a2), ..., lrf (ap)
   rlf est appele sur chaque symbole ds l'ordre lrf(ap), lrf (ap-1), ..., lrf (a1)
*/
static void
walk_gen_string (prefix, lrf, rlf)
     SXINT prefix;
     void   (*lrf)(), (*rlf)();
{
  SXINT ai;

  if (prefix) {
    ai = XxY_X (XxY_gen_repair_hd, prefix);
    prefix = XxY_Y (XxY_gen_repair_hd, prefix);
    if (rlf) (*rlf) (ai, prefix == 0);
    walk_gen_string (prefix, lrf, rlf);
    if (lrf) (*lrf) (ai, prefix == 0);
  }
}


static SXINT          *repair_source_stack, *repair_terminal_index2i, *repair_terminal_index2j, *repair_Tpq, *repair_rhs_stack;
static XxY_header   repair_forms_hd;
static X_header     repair_Tpq_hd;
/* repair_Tpq2tok_no est en // mais independant des #if */
static XxY_header   repair_Pij_hd; /* Pij = (Aij, repair_Pij) */
static SXINT          *repair_Pij2gen_lgth; /* Si Pij =>* x alors |x| */
static XH_header    repair_iprod_stack_hd; /* Pij = rhs_lgth+2 Aij (Xkj ... Xpq  ... Xih) prod */
static SXINT          cur_error_string_lbound, cur_error_string_rbound /* lmlstn, rmlstn */, cur_repair_string_rbound /* lgth+1 de la chaine corrigee */;
static SXINT          repair_result_stack [inputG_MAXRHSLGTH+1];

static void
repair_Pij_hd_oflw (old_size, new_size)
     SXINT		old_size, new_size;
{
  repair_Pij2gen_lgth = (SXINT*) sxrealloc (repair_Pij2gen_lgth, new_size+1, sizeof (SXINT));
}


static void
repair_Tpq_hd_oflw (old_size, new_size)
     SXINT		old_size, new_size;
{
  repair_Tpq2tok_no = (SXINT*) sxrealloc (repair_Tpq2tok_no, new_size+1, sizeof (SXINT));
}



/* si a1 a2 ... ap est la chaine corrigee
   on a t = ai, t> 0 */
/* On fabrique le Tpq correspondant */
static SXINT
make_repair_terminal (t, i, comment_ste, psource_index)
     SXINT t, i, comment_ste;
     struct sxsource_coord *psource_index;
{
  SXINT            j, Tpq, single;
  struct sxtoken *ptok_buf;

  j = i+1;

  if (i == 1)
    /* debut de la chaine corrigee */
    /* On met le mlstn du debut de la chaine a corriger */
    i = cur_error_string_lbound;
  else
    /* delta courant */
    i += repair_mlstn_top-1;

  if (i > cur_error_string_rbound)
    i = cur_error_string_rbound;

  if (j == cur_repair_string_rbound)
    /* fin de la chaine corrigee */
    /* On met le mlstn de la fin de la chaine a corriger */
    j = cur_error_string_rbound;
  else
    j += repair_mlstn_top-1;

  if (j > cur_error_string_rbound)
    j = cur_error_string_rbound;

#ifdef MAKE_INSIDEG
  t = spf.insideG.t2init_t [t];
#endif /* MAKE_INSIDEG */

  if (!MAKE_Tij (Tpq, -t, i, j)) {
    /* On range directement ds la structure definitive */
    Tpq_repair_nb++;
    SXBA_1_bit (spf.outputG.Tpq_rcvr_set, Tpq);
  }
#if 0
  /* ... non, peut deja exister */
#if EBUG
  else
    /* Forcement nouveau */
    sxtrap (ME, "make_repair_terminal");
#endif /* EBUG */
#endif /* 0 */

  /* Il faut faire comme si c'etait un vrai terminal du source */
  /* dans les 2 cas, le resultat s'exploite par parser_Tpq2tok (Tpq) qui retourne un pointeur vers un token */
  X_set (&repair_Tpq_hd, Tpq, &single);
  repair_Tpq2tok_no [single] = put_repair_tok (t, inputG_tstring [t], comment_ste == SXEMPTY_STE ? NULL : sxstrget (comment_ste), psource_index);

  return -Tpq;
}



/* On instancie le contenu de rhs_stack */
/* Nelle version qui cree une foret */
/* retourne le Aij de la prod creee */
static SXINT
make_repair_prod (prod, i, j, rhs_stack)
     SXINT prod, i, j, *rhs_stack;
{
  SXINT bot_item, top_item, cur_item, A, Aij, Xpq, Pij, RHSij, lgth, init_prod, h, k;
  SXINT *cur_rhs_stack;

  A = LHS (prod);

#if MEASURES || LOG
  SXBA_1_bit (used_prod_set, prod);
#endif /* MEASURES || LOG */

#ifdef MAKE_INSIDEG
  A = spf.insideG.nt2init_nt [A];
  init_prod = spf.insideG.prod2init_prod [prod];
#else /* ifndef MAKE_INSIDEG */
  init_prod = prod;
#endif /* ifndef MAKE_INSIDEG */

  /* Les reajustements sur i et j ont deja ete faits */
  MAKE_Aij (Aij, A, i, j);

  if (i == j && Aij <= spf.outputG.maxnt && Aij > 1) {
    /* Ce nt vide a deja ete genere et n'est pas un nt de la reparation
       On ne va pas reinstancier (dupliquer) les prod qui menent au vide */
    /* Le 09/01/07 PB si c'est l'axiome !! */
    return Aij;
  }

  bot_item = PROLON (prod);
  top_item = PROLON (prod+1)-1;

  lgth = top_item-bot_item;
  XH_push (repair_iprod_stack_hd, lgth+2); /* Ca doit etre une vraie sxstack !! */
  XH_push (repair_iprod_stack_hd, Aij);

  if (bot_item < top_item) {
    cur_rhs_stack = rhs_stack+lgth;

    for (cur_item = bot_item; cur_item < top_item; cur_item++) {
      Xpq = *--cur_rhs_stack;
      XH_push (repair_iprod_stack_hd, Xpq);
    }
  }

  XH_push (repair_iprod_stack_hd, init_prod);

  if (!XH_set (&repair_iprod_stack_hd, &Pij)) {
    /* Nelle prod */
    /* Pour trouver les Pij qui ont Aij en lhs */
    XxY_set (&repair_Pij_hd, Aij, Pij, &RHSij);
    /* Attention repair_iprod_stack_hd peut contenir des Pij
       bidons qui ont prod==0 */
    repair_Pij2gen_lgth [Pij] = j-i;
    Pij_repair_nb++;

    /* (les occur de ses nt en rhs seront comptées plus tard) */

    if (bot_item < top_item && !is_rcvr_can_be_cyclic) {
      cur_rhs_stack = rhs_stack;

      for (cur_item = bot_item; cur_item < top_item; cur_item++) {
	Xpq = *cur_rhs_stack++;

	if (Xpq > 0) {
	  h = Aij2i (Xpq);
	  k = Aij2j (Xpq);
	}
	else {
	  h = Tij2i (-Xpq);
	  k = Tij2j (-Xpq);
	}

	if (h > 0 && k > h) {
	  /* pas en erreur et non vide */
	  if (h > i || k < j) {
	    /* ... et ne couvre pas le meme range que la lhs => prod non cyclique */
	    break;
	  }
	}
      }
      if (cur_item >= top_item)
	is_rcvr_can_be_cyclic = SXTRUE;
    }
  }
  
  return Aij;
}

#define MANUAL_DEBUG 0

#if MANUAL_DEBUG
static void
print_stack (SXINT *stack, char *stack_name)
{
  SXINT i, size = stack[0];

  printf ("%s [" SXILD "] = {", stack_name, size);

  for (i = 1; i < size; i++)
    printf (SXILD " ,", stack [i]);

  if (i == size)
    printf (SXILD, stack [i]);

  fputs ("}\n", stdout);
}
#endif /* MANUAL_DEBUG */

/* gen_tnt_repair_stack contient le suffixe d'une forme sentencielle (lhs au sommet)
   repair_source_stack contient le suffixe de la chaine source a reconnaitre (lhs au sommet)
   repair_Tpq contient, pour chaque element de repair_source_stack le Tpq associe'
   repair_terminal_index2[i,j] contient, pour chaque element de repair_source_stack la position (initiale|finale) qui lui correspond ds le source réel de départ
   prod_stack contient le suffixe de la liste des productions utilisees pour generer la chaine source courante
*/
static void
td_unique_parse (prod_stack)
     SXINT *prod_stack;
{
  SXINT     tnt, prod, bot_item, top_item, cur_item, bot_repair_rhs_stack, p, q, X, Aij, lgth;

#if MANUAL_DEBUG
  fputs ("******* Entering td_unique_parse (prod_stack) ********\n", stdout);
  print_stack (prod_stack, "prod_stack");
  print_stack (gen_tnt_repair_stack, "gen_tnt_repair_stack");
  print_stack (repair_source_stack, "repair_source_stack");
  print_stack (repair_rhs_stack, "repair_rhs_stack");
#endif /* MANUAL_DEBUG */

  if (DTOP (gen_tnt_repair_stack) == 0)
    return;

  tnt = DPOP (gen_tnt_repair_stack);

#if MANUAL_DEBUG
    printf ("tnt = " SXILD "\n", tnt);
#endif /* MANUAL_DEBUG */

  if (tnt < 0) {
#if EBUG || MANUAL_DEBUG
    /* repair_source_stack contient la séquence des terminaux constituant le source corrigé généré (le plus à gauche est au sommet)*/
    if (DTOP (repair_source_stack) == 0 || repair_source_stack [DTOP (repair_source_stack)] != -tnt)
      sxtrap (ME, "td_unique_parse");
#endif /* EBUG || MANUAL_DEBUG */
      
    /* scan OK */
    DPUSH (repair_rhs_stack, repair_Tpq [DTOP (repair_source_stack)]);

    /* On "enleve" t des piles */
    DTOP (repair_source_stack)--;
  }
  else {
    /* On a un nt en sommet de pile, on va l'etendre ... */
    prod = POP (prod_stack);

#if MANUAL_DEBUG
    printf ("prod = " SXILD "\n", tnt);
#endif /* MANUAL_DEBUG */

#if EBUG || MANUAL_DEBUG
    if (LHS (prod) != tnt)
      sxtrap (ME, "td_unique_parse");
#endif /* EBUG || MANUAL_DEBUG */

    p = repair_terminal_index2i [DTOP (repair_source_stack)];
    
    bot_repair_rhs_stack = DTOP (repair_rhs_stack)+1;
    
    cur_item = PROLON (prod);
    lgth = 0;
    
    while (X = LISPRO (cur_item++)) {
      lgth++;
      DPUSH (gen_tnt_repair_stack, X);
      td_unique_parse (prod_stack);
    }
    
    q = repair_terminal_index2j [DTOP (repair_source_stack)];

    Aij = make_repair_prod (prod, p, q, repair_rhs_stack+bot_repair_rhs_stack);
    DTOP (repair_rhs_stack) -= lgth;
    DPUSH (repair_rhs_stack, Aij);
  }
#if MANUAL_DEBUG
  fputs ("******* Leaving td_unique_parse (prod_stack) ********\n", stdout);
  print_stack (prod_stack, "prod_stack");
  print_stack (gen_tnt_repair_stack, "gen_tnt_repair_stack");
  print_stack (repair_source_stack, "repair_source_stack");
  print_stack (repair_rhs_stack, "repair_rhs_stack");
#endif /* MANUAL_DEBUG */
}


static void
unfold_gen (symb)
     SXINT symb;
{
  DPUSH (repair_source_stack, symb);
}

#if LOG
static void
print_repair_Pij (Pij)
     SXINT Pij;
{
  SXINT cur, bot, top, Aij, init_prod, A, i, j, p, Xpq, X, p_Xpq, q_Xpq;

  top = XH_X (repair_iprod_stack_hd, Pij+1)-1;
  init_prod = XH_list_elem (repair_iprod_stack_hd, top);

  if (init_prod > 0) {
    /* pas bidon */
    bot = XH_X (repair_iprod_stack_hd, Pij)+1;
    Aij = XH_list_elem (repair_iprod_stack_hd, bot);
    A = Aij2A (Aij);
    i = Aij2i (Aij);
    j = Aij2j (Aij);

    /* La vraie valeur de Pij ne sera connue que lors de la mise en foret */
    /* Attention, il faut utiliser tstring[] et ntstring[] et non TSTRING() et NTSTRING() */

    printf ("?/" SXILD ":<%s[" SXILD "::" SXILD "]>\t= ", init_prod, ntstring [A], i, j);

    for (cur = top-1; bot < cur; cur--) {
      Xpq = XH_list_elem (repair_iprod_stack_hd, cur);

      if (Xpq < 0) {
	Xpq = -Xpq;
	X = Tij2T (Xpq);
	p_Xpq = Tij2i (Xpq);
	q_Xpq = Tij2j (Xpq);
	printf ("\"%s\"[" SXILD "::" SXILD "] ", tstring [-X], p_Xpq, q_Xpq);
      }
      else {
	X = Aij2A (Xpq);
	p_Xpq = Aij2i (Xpq);
	q_Xpq = Aij2j (Xpq);

	if (Xpq <= spf.outputG.maxnt)
	  /* On peut avoir des Aii normaux en RHS de prod de la reparation */
	  printf ("<%s[" SXILD ".." SXILD "]> ", ntstring [X], p_Xpq, q_Xpq);
	else
	  printf ("<%s[" SXILD "::" SXILD "]> ", ntstring [X], p_Xpq, q_Xpq);
      }
    }

    fputs (";\n", stdout);
  }
}


/* Aij est negatif, il ne faut pas le confondre avec un "vrai" Aij */
static void
print_repair_parse_forest (Aij)
     SXINT Aij;
{
  SXINT Pij, i, j, p, q, RHSij, cur, bot, top, Xpq, X, p_Xpq, q_Xpq;

  /* Aij est special */
  p = XxY_X (repair_forms_hd, -Aij);
  i = p/MAXITEM;
  /* litem = p%MAXITEM */
  q = XxY_Y (repair_forms_hd, -Aij);
  j = q/MAXITEM;
  /* ritem = q%MAXITEM */

  fputs ("\n****** Instantiated initial sentential forms *******\n", stdout);

  XxY_Xforeach (repair_Pij_hd, Aij, RHSij) {
    Pij = XxY_Y (repair_Pij_hd, RHSij);

    bot = XH_X (repair_iprod_stack_hd, Pij)+1;
    top = XH_X (repair_iprod_stack_hd, Pij+1)-1;

    for (cur = top-1; bot < cur; cur--) {
      Xpq = XH_list_elem (repair_iprod_stack_hd, cur);

      /* Attention, il faut utiliser tstring[] et ntstring[] et non TSTRING() et NTSTRING() */
      if (Xpq < 0) {
	Xpq = -Xpq;
	X = Tij2T (Xpq);
	p_Xpq = Tij2i (Xpq);
	q_Xpq = Tij2j (Xpq);
	printf ("\"%s\"[" SXILD "::" SXILD "] ", tstring [-X], p_Xpq, q_Xpq);
      }
      else {  
	X = Aij2A (Xpq);
	p_Xpq = Aij2i (Xpq);
	q_Xpq = Aij2j (Xpq);

	if (Xpq <= spf.outputG.maxnt)
	  /* Il se peut que ce soit un bon (vide) */
	  printf ("<%s[" SXILD ".." SXILD "]> ", ntstring [X], p_Xpq, q_Xpq);
	else
	  printf ("<%s[" SXILD "::" SXILD "]> ", ntstring [X], p_Xpq, q_Xpq);
      }
    }
  }

  fputs ("\n****** Parse forest for the correcting string *******\n", stdout);
  
  for (Pij = 1; Pij < XH_top (repair_iprod_stack_hd); Pij++)
    print_repair_Pij (Pij);

  fputs ("****** End of the repair parse forest *******\n\n", stdout);
}
#endif /* LOG */



static SXINT
call_td_repair_parses (lmlstn, rmlstn, litem, ritem, top, xprod_stack)
     SXINT lmlstn, rmlstn, litem, ritem, top, xprod_stack;
{
  SXINT                   cur_item, gen_lgth, top_sf, X, t, x, index, size, repair_source_lgth, Aij, lstate, rstate, triple, ltok_no, ste, cur, RHSij, Pij, xbuf, Xpq, tok_no;
  SXINT                   *prod_stack;
  struct sxsource_coord source_index;

  size = DSIZE (repair_source_stack);

  walk_gen_string (top, NULL, unfold_gen);

  if (DSIZE (repair_source_stack) > size) {
    size = DSIZE (repair_source_stack);
    repair_terminal_index2i = (SXINT*) sxrealloc (repair_terminal_index2i, size+1, sizeof (SXINT));
    repair_terminal_index2j = (SXINT*) sxrealloc (repair_terminal_index2j, size+1, sizeof (SXINT));
    repair_Tpq = (SXINT*) sxrealloc (repair_Tpq, size+1, sizeof (SXINT));
  }

  repair_source_lgth = TOP (repair_source_stack);
    
  /* On recupere les commentaires associe's a la sous-chaine en erreur du source */
  if (lmlstn < rmlstn) {
    /* Le source est non vide */
    /* recup des commentaires */
    lstate = mlstn2dag_state (lmlstn);
    rstate = mlstn2dag_state (rmlstn);

    varstr_raz (varstring);
    varstring = sub_dag_to_comment (varstring, lstate, rstate);

    ste = sxstr2save (varstr_tostr (varstring), varstr_length (varstring));
  }
  else
    ste = SXEMPTY_STE;

  cur_error_string_lbound = lmlstn;
  cur_error_string_rbound = rmlstn;
  cur_repair_string_rbound = repair_source_lgth+1;

  /* Pour avoir une idee du source_index */
  if (lmlstn >= idag.final_state)
    tok_no = idag.pq2tok_no [idag.p2pq_hd [idag.final_state-1]],
      tok_no = (tok_no > 0) ? tok_no : idag.tok_no_stack [-tok_no+1],
      source_index = SXGET_TOKEN (tok_no).source_index;
  else
    tok_no = idag.pq2tok_no [idag.p2pq_hd [lmlstn]],
      tok_no = (tok_no > 0) ? tok_no : idag.tok_no_stack [-tok_no+1],
      source_index = SXGET_TOKEN (tok_no).source_index;

  index = 1;
  
  /* repair_terminal_index2i [i] est la position correspondant au (repair_source_lgth - i + 1)-ième terminal généré dans le source réel en erreur,
     et repair_terminal_index2i [0] donne la position dans le source réel de la fin de la section en erreur (attention au cas où la chaîne générée est vide) */
  for (x = repair_source_lgth; x > 0; x--) {
    SXINT Tpq = -(repair_Tpq [x] = make_repair_terminal (repair_source_stack [x], index, ste, &(source_index)));
    repair_terminal_index2i [x] = Tij2i (Tpq);
    repair_terminal_index2j [x] = Tij2j (Tpq);
    index++;
  }
  
  /* utilisation tordue de l'indice 0 : on y indique cur_error_string_[l,r]bound */
  repair_terminal_index2i [0] = cur_error_string_lbound;
  repair_terminal_index2j [0] = cur_error_string_rbound;

  DCHECK (gen_tnt_repair_stack, ritem-litem);

  /* On fabrique l'identifiant de la chaine corrigee instanciee.
     C'est un Aij special avec A = 0 et i = p*MAXITEM+litem et j = q*MAXITEM+ritem */
  XxY_set (&repair_forms_hd, lmlstn*MAXITEM+litem, rmlstn*MAXITEM+ritem, &Aij);
  Aij = -Aij; /* On le passe en neg, ce n'est pas un vrai Aij */

  while (xprod_stack) {
    prod_stack = glbl_repair_prod_stacks+xprod_stack;
    xprod_stack = prod_stack [1];

    DRAZ (gen_tnt_repair_stack);
    TOP (repair_source_stack) = repair_source_lgth;

    for (cur_item = litem; cur_item < ritem; cur_item++) {
      X = LISPRO (cur_item);
      DSPUSH (gen_tnt_repair_stack, X);
      DTOP (repair_rhs_stack) = 0;
      td_unique_parse (prod_stack);
      /* Ici repair_rhs_stack contient un seul symbole instancie' Xpq */
      /* si une foret d'analyse de la chaine corrigee est demandee, la chaine
	 en erreur elle meme se decouper de plusieurs fac,ons */
      /* PB : comment repercuter tout ca sur l'appelant ? */
      /* On suppose que le resultat d'un repair est (outre) repair_Pij_hd (et ses satellites)
	 la pile de piles repair_result_stack */
#if EBUG
      if (DTOP (repair_rhs_stack) != 1)
	sxtrap (ME, "call_td_repair_parses");
#endif /* EBUG */
      
      DSPUSH (repair_result_stack, repair_rhs_stack [1]);
    }

    top = TOP (repair_result_stack);

    XH_push (repair_iprod_stack_hd, top+2);
    XH_push (repair_iprod_stack_hd, Aij);

    for (cur = top; 1 <= cur; cur--) {
      XH_push (repair_iprod_stack_hd, repair_result_stack [cur]);
    }

    XH_push (repair_iprod_stack_hd, 0); /* Pas de prod */
    /* bidon, ne compte pas ds Pij_repair_nb */

#if 0
    /* Remplace' le 04/10/06 par le bloc ci-dessous */
    XH_set (&repair_iprod_stack_hd, &Pij);
#endif /* 0 */

    if (!XH_set (&repair_iprod_stack_hd, &Pij)) {
      /* Nelle prod */
      /* Pour trouver les Pij qui ont Aij en lhs */
      XxY_set (&repair_Pij_hd, Aij, Pij, &RHSij);
      /* Attention repair_iprod_stack_hd peut contenir des Pij
	 bidons qui ont prod==0 */
      repair_Pij2gen_lgth [Pij] = repair_source_lgth; /* !! */
      Pij_repair_nb++;

#if 0
      /* on va faire ce calcul de toutes façons dans put_in_shared_parse_forest ; le faire ici
	 semble ne faire que doubler la valeur de ...rhs_nb, et donc de le rendre faux */
      /* ... on peut compter les occur de ses nt en rhs */
      for (cur = top; 1 <= cur; cur--) {
	Xpq = repair_result_stack [cur];

	if (Xpq > 0)
	  /* xnt */
	  spf.outputG.Aij2rhs_nb [Xpq]++;
      }
#endif /* 0 */
    }

    TOP (repair_result_stack) = 0;
  }

  if (repair_source_lgth >= 2)
    repair_mlstn_top += repair_source_lgth-1;

#if LOG
  print_repair_parse_forest (Aij);
#endif /* !LOG */

  /* Le resultat est un id de la forme d'un Aij special qui definit des prods bidons
     Aij -> RHSij
     ou` les RHSij sont toutes les instanciations possibles de la chaine de symboles qui
     derivent ds la chaine corrigee instanciee */
  return Aij;
}



static void
print_gen_string (SXINT symb, SXBOOLEAN is_last_call)
{
  if (!is_last_call)
    putchar (' ');

  printf ("\"%s\"", TSTRING (symb));
}

static void
get_gen_string (SXINT symb, SXBOOLEAN is_last_call)
{
  if (!is_last_call)
    varstring = varstr_catchar (varstring, ' ');

  varstring = varstr_catenate (varstring, TSTRING (symb));
}

static void
gen_deplacement (SXINT sub_string)
{
  SXINT   symb, prefix, i, q, best_delta, top, trans, Y, delta, anchor, dij;
  SXINT   *t_stack, *t_stack_top;
  SXBA    anchor_set, set;
  SXBOOLEAN is_in_source;

  if (SXBA_bit_is_reset_set (repair_gen_nyp, sub_string)) {
    /* C'est la 1ere fois qu'on visite sub_string */
    if (sub_string) {
      /* prefixe des chaines corrigees non vide */
      prefix = XxY_Y (XxY_gen_repair_hd, sub_string);
      gen_deplacement (prefix);
      symb = XxY_X (XxY_gen_repair_hd, sub_string);
	
      /* a priori */
      gen_repair2attr [sub_string] = gen_repair2attr [prefix];
      gen_repair2attr [sub_string].i++; /* indice du debut de sub_string */

      anchor_set = gen_repair2attr [prefix].anchor_set;
      i = gen_repair2attr [prefix].i;
      q = TOP (repair_path_stack);
      best_delta = -1;

      /* Le symbole symb se trouve-t-il ds le source */
      for (top = q; top >= 1; top--) {
	if (!SXBA_bit_is_set (anchor_set, top)) {
	  trans = repair_path_stack [top];

	  /* trans est un pq */
	  t_stack = idag.t_stack + idag.pq2t_stack_hd [trans]; /* Pile des terminaux associes aux transitions pq */
	  t_stack_top = t_stack + *t_stack;
	  is_in_source = SXFALSE;

	  while (++t_stack <= t_stack_top) {
	    if (*t_stack == symb) {
	      is_in_source = SXTRUE;
	      break;
	    }
	  }

	  if (is_in_source) {
	    if ((delta = top-i) < 0)
	      delta = -delta;
	  
	    if (best_delta == -1 || delta < best_delta) {
	      best_delta = delta;
	      anchor = top;
	    }
	  }
	}
      }

      if (best_delta >= 0) {
	/* trouve' */
	if ((dij = gen_repair2attr [prefix].delta_i) == -1) {
	  /* ancrage : on passe de flottant a fige' */
	  /* distance nulle */
	  if (best_delta == 0)
	    gen_repair2attr [sub_string].delta_i = gen_repair2attr [sub_string].delta_j = 0;
	  else {
	    if (i < anchor) {
	      gen_repair2attr [sub_string].delta_i = best_delta;
	      gen_repair2attr [sub_string].delta_j = 0;
	    }
	    else {
	      gen_repair2attr [sub_string].delta_i = 0;
	      gen_repair2attr [sub_string].delta_j = best_delta;
	    }
	  }
	}
	else {
	  dij += i - (anchor+gen_repair2attr [prefix].delta_j);

	  if (dij < 0)
	    dij = -dij;

	  /* On ajoute le cout de ce deplacement */
	  gen_repair2attr [sub_string].distance += rcvr_spec.deplacement*dij;
	}
	  
	/* ne doit pas etre reutilise' ds ce parcourt */
	set = gen_repair2attr [sub_string].anchor_set = sxba_bag_get (&repair_bag, q+1);
	sxba_copy (set, anchor_set);
	SXBA_1_bit (set, anchor);
	gen_repair2attr [sub_string].anchor = anchor;
      }
      else
	gen_repair2attr [sub_string].anchor = 0;
    }
    else {
      /* debut de chaine */
      gen_repair2attr [sub_string].delta_i = gen_repair2attr [sub_string].delta_j = -1;
      gen_repair2attr [sub_string].i = 1; /* indice de la 1ere lettre de l'arbre des corrections */
      gen_repair2attr [sub_string].distance = 0; /* indice de la 1ere lettre de l'arbre des corrections */
      anchor_set = gen_repair2attr [sub_string].anchor_set = sxba_bag_get (&repair_bag, TOP (repair_path_stack)+1);
      sxba_empty (anchor_set);
      gen_repair2attr [sub_string].anchor = 0;
    }
  }
}


static SXINT
rcvr_gen_distance (sub_string, delta_i, delta_j, anchor_set)
     SXINT  sub_string, delta_i, delta_j;
     SXBA anchor_set;
{
  SXINT symb, prefix, distance, i, j, q;

  if (sub_string) {
    /* prefixe des chaines corrigees non vide */
    prefix = XxY_Y (XxY_gen_repair_hd, sub_string);
    distance = rcvr_gen_distance (prefix, delta_i, delta_j, anchor_set);

    if (best_distance == -1 || distance < best_distance) {
      i = gen_repair2attr [prefix].i;
      j = i+delta_i-delta_j; /* i des chaines corrigees courantes correspond a j dans le source */
      q = TOP (repair_path_stack);

      if (gen_repair2attr [sub_string].anchor == 0) {
	/* Le symbole courant n'est pas une ancre */

	if (j < 1 || j > q) {
	  /* Hors range */
	  /* On passe donc de j a i a j par insertion */
	  distance += rcvr_spec.insertion;
	}
	else {
	  if (SXBA_bit_is_set (anchor_set, j)) {
	    /* on tombe sur un symbole deplace' */
	    /* le symbole courant de la chaine corrigee est donc inse're'' */
	    distance += rcvr_spec.insertion;
	  }
	  else {
	    /* changement */
	    distance += rcvr_spec.changement;
	  }
	}
      }
      else {
	if (j >= 1 && j <= q && !SXBA_bit_is_set (anchor_set, j)) {
	  /* insertion dans le source (donc suppression ds la chaine corrigee) */
	  distance += rcvr_spec.suppression;
	}
      }
    }
  }
  else {
    /* debut de chaine */
    distance = 0;
  }

  return distance;
}


/* repair_path_stack contient un chemin du dag, donc une chaine source */
/* L'ensemble des corrections possibles se trouve ds (XxY_gen_repair_hd, gen_valid_repair_string_stack)
/* On calcule la distance entre ce chemin et l'ensemble des corrections possibles */
/* cad l'ensemble des modifs qu'il faut faire subire a la chaine source pour la transformer en une correction */
static void
rcvr_distance ()
{
  SXINT  x, top, delta_i, delta_j, distance, j, symb;
  SXBA anchor_set;

  sxba_empty (repair_gen_nyp);

  for (x = DTOP (gen_valid_repair_string_stack); x >= 1; x--) {
    top = gen_valid_repair_string_stack [x];
    gen_deplacement (top);
  }

  for (x = DTOP (gen_valid_repair_string_stack); x >= 1; x--) {
    /* Pour chaque chaine corrigee */
    top = gen_valid_repair_string_stack [x];
    delta_i = gen_repair2attr [top].delta_i;

    if (delta_i == -1) {
      /* flottante (aucun symbole commun) */
      delta_i = delta_j = 0;
    }
    else
      delta_j = gen_repair2attr [top].delta_j;
    
    anchor_set = gen_repair2attr [top].anchor_set;

    distance = rcvr_gen_distance (top, delta_i, delta_j, anchor_set);

    j = gen_repair2attr [top].i+delta_i-delta_j;

    for (symb = TOP (repair_path_stack); symb > j; symb--) {
      if (!SXBA_bit_is_set (anchor_set, symb)) {
	/* symbole sans correspondant dans la chaine corrigee */
	distance += rcvr_spec.suppression;
      }
    }

    if (best_distance == -1 || distance < best_distance) {
      best_distance = distance;
      best_repair = top;
    }
  }
}


/* genere ds path_stack les chemins du dag */
static SXBOOLEAN
rcvr_gen_dag_paths (SXINT ldag_state, SXINT rdag_state, void (*f)())
{
  SXINT   triple, pq_bot, pq_top;
  SXBOOLEAN ret_val;

  if (ldag_state >= rdag_state) {
    if (ret_val = (ldag_state == rdag_state))
      if (f) (*f) ();

    return ret_val;
  }

  ret_val = SXFALSE;

  /* Ds cette vision on met ds repair_path_stack des pq (si on a besoin de recuperer p a partir de pq il faudra rajouter ds idag une correspondance pq --> p) */
  pq_bot = idag.p2pq_hd [ldag_state];
  pq_top = idag.p2pq_hd [ldag_state+1];

  while (pq_bot < pq_top) {
    if (f) PUSH (repair_path_stack, pq_bot);

    if (rcvr_gen_dag_paths (idag.pq2q [pq_bot], rdag_state, f))
      ret_val = SXTRUE;

    if (f) TOP (repair_path_stack)--;

    pq_bot++;
  }

  return ret_val;
}


/* Le 28/09/05 */
/* Il y a eu de la recuperation d'erreur entre i et j ub */
/* On fabrique tous les Tij qui participent aux chemins terminaux allant de i a` j */
static void
xt_complete (SXINT lb, SXINT ub)
{
  SXINT   i, j, Tij, t, bot_pq, top_pq, q;
  SXINT   *t_stack, *t_stack_top;

  i = lb < 0 ? -lb : lb;
  j = ub < 0 ? -ub : ub;

  if (i == j) return;

  if (idag.path == NULL)
    fill_idag_path ();

  bot_pq = idag.p2pq_hd [i];
  top_pq = idag.p2pq_hd [i+1];

  while (bot_pq < top_pq) {
    q = idag.pq2q [bot_pq];

    if (q == j || SXBA_bit_is_set (idag.path [q], j)) {
      t_stack = idag.t_stack + idag.pq2t_stack_hd [bot_pq]; /* Pile des terminaux associes aux transitions pq */
      t_stack_top = t_stack + *t_stack;

      while (++t_stack <= t_stack_top) {
	t = *t_stack;

#ifdef MAKE_INSIDEG
	MAKE_Tij (Tij, -spf.insideG.t2init_t [t], i, q);
#else /* !MAKE_INSIDEG */
	MAKE_Tij (Tij, -t, i, q);
#endif /* !MAKE_INSIDEG */
      }

      if (SXBA_bit_is_reset_set (idag.path [0], q))
	/* Pour ne pas refaire 2 fois la meme chose */
	xt_complete (q, j);
    }

    bot_pq++;
  }
}


/* Une erreur a ete detectee ds la table mlstn
   Cette table contient un item left_item de la forme A -> \alpha .B \beta X \gamma.
   On a item == A -> \alpha B \beta .X \gamma et t \in First1 (X)
   et t est un terminal source du suffixe
   Pour chaque transition du suffixe de la forme p ->t q, on va mettre
   A -> \alpha B \beta .X \gamma dans la table Tp
   On suppose donc qu'il y a ds le source, entre mlstn et p une phrase de B \beta 
 */
/*
  litem = A -> \alpha . \beta \gamma
  ritem = A -> \alpha \beta . \gamma
  Le traitement d'erreur a "ajoute" ds le dag entre lmlstn et rmlstn une phrase de \beta
 */
/*
Ds le cas !is_dag
Soit l = rmlstn-lmlstn et ... litem \alpha ritem ... tq \alpha =>* x (si litem=ritem, \alpha=x=\varepsilon

        |    l=0    |            l>0
--------|-----------|---------------------------
 |x|=0  | no rcvr   |        suppression
--------|-----------|---------------------------
 |x|>0  | insertion |l<|x| | l=|x|     | l>|x|
                    |------|-----------|--------
                    |inser-|changement/|suppres-
                    |tion  |deplacement|sion
               
*/
/*
  Attention, il n'est pas possible, sans manip spe'ciale de 
     - changer le DAG source en lui incorporant les corrections et en renume'rotant ses e'tats (pour
       conserver la relation d'ordre) et de
     - re'utiliser la fore^t d'analyse (en se contentant d'y changer les Xpq en Xp'q')
  Ex :
  Soit la grammaire S -> A B A; A -> ; B -> b; et le source vide.  Apre`s correction on obtient la fore^t
  S[1..1] -> A[1..1] B[1::1] A[1..1]; A[1..1] -> ; B[1::1] -> "b"[1::1] ;
  Or, si on change le Dag d'entre'e en mettant une transition de 1 ->b 2, on aurait la fore^t
  S[1..2] -> A[1..1] B[1..2] A[2..2]; A[1..1] -> ; B[12] -> "b"[1..2] ; A[2..2] -> ;
  qui n'est pas isomorphe a` la pre'ce'dente.

  Si on veut absolument apre`s re'paration avoir la relation d'ordre sur les indices, il y a qq pistes :
     1) Une fois l'analyse et les re'parations effectue'es, on reconstruit un nouveau dag qui contient les
        corrections et on recommence l'analyse.
     2) Ds le reconnaisseur, sur toute re'paration (donc locale) on change le dag pour y incorporer
        les corrections (qui ne seront peut e^tre pas valide'es ulte'rieurement).  Ces changements peuvent
	conduire a de la reallocation des tables earley.  Cette modif du dag au fur et a` mesure assure que
	la suite de l'analyse (et donc la fore^t) tiendra compte des modifs.  Ces modifs peuvent se faire
	soit en changeant et en renumerotant tout le dag, soit en le changeant et en modifiant les dag2mlstn
	et mlstn2dag.
 */
static SXINT
earley_glbl_rcvr_message (litem, lmlstn, ritem, rmlstn, kind, store_rcvr_string)
     SXINT     litem, lmlstn, ritem, rmlstn, kind;
     SXBOOLEAN store_rcvr_string;
{
  SXINT                   cur_item, X, top, x, ldag_state, rdag_state, lgth, xbuf, lgth_tried, trans, xtok, triple, min_gen_lgth, gen_lgth, parse_id;
  struct sxsource_coord	lsource_index, rsource_index;
  char                  int_str [12];
  SXBOOLEAN               has_a_nt, really_store;
    
  ldag_state = mlstn2dag_state (lmlstn);
  rdag_state = mlstn2dag_state (rmlstn);

  varstr_raz (varstring);
  
  if (rcvr_spec.perform_repair) {
    if (!XxY_is_allocated (XxY_gen_repair_hd)) {
      sxprepare_for_possible_reset (XxY_gen_repair_hd.is_allocated);
      XxY_alloc (&XxY_gen_repair_hd, "XxY_gen_repair_hd", 100, 1, 0, 1, XxY_gen_repair_hd_oflw, NULL);

      if (rcvr_spec.repair_kind != 1) {
	gen_repair2attr = (struct gen_repair2attr_struct*) sxalloc (XxY_size (XxY_gen_repair_hd)+1, sizeof (struct gen_repair2attr_struct));
	repair_gen_nyp = sxba_calloc (XxY_size (XxY_gen_repair_hd)+1);
	repair_path_stack = (SXINT*) sxalloc (idag.final_state+rcvr_tok_no+1, sizeof (SXINT)), repair_path_stack [0] = 0;
	sxba_bag_alloc (&repair_bag, "repair_bag", (idag.final_state+rcvr_tok_no + 1) * SXNBLONGS (idag.final_state+rcvr_tok_no + 1), statistics);
      }

      DALLOC_STACK (gen_valid_repair_string_stack, 100);
      DALLOC_STACK (gen_tnt_repair_stack, 100);

      if (rcvr_spec.perform_repair_parse) {
	gen_repair2attr2 = (struct gen_repair2attr2_struct*) sxalloc (XxY_size (XxY_gen_repair_hd)+1, sizeof (struct gen_repair2attr2_struct));
	DALLOC_STACK (local_repair_prod_stack, 100);
	DALLOC_STACK (glbl_repair_prod_stacks, 1000);

	XxY_alloc (&repair_forms_hd, "repair_forms_hd", 50, 1, 0, 0, NULL, NULL);

	X_alloc (&repair_Tpq_hd, "repair_Tpq_hd", 100, 1, repair_Tpq_hd_oflw, NULL);
	sxprepare_for_possible_reset (repair_Tpq2tok_no);
	repair_Tpq2tok_no = (SXINT*) sxalloc (X_size (repair_Tpq_hd)+1, sizeof (SXINT)), repair_Tpq2tok_no [0] = 0;

	sxprepare_for_possible_reset (repair_iprod_stack_hd.is_allocated);
	XH_alloc (&repair_iprod_stack_hd, "repair_iprod_stack_hd", 100, 1, inputG_MAXRHSLGTH+3, NULL, NULL);
	XxY_alloc (&repair_Pij_hd, "repair_Pij_hd", 100, 1, 1, 0, repair_Pij_hd_oflw, NULL); /* Xforeach */
	repair_Pij2gen_lgth = (SXINT*) sxalloc (XxY_size (repair_Pij_hd)+1, sizeof (SXINT));

	DALLOC_STACK (repair_source_stack, 20);
	repair_terminal_index2i = (SXINT*) sxalloc (DSIZE (repair_source_stack)+1, sizeof (SXINT)); /* // a repair_source_stack */
	repair_terminal_index2j = (SXINT*) sxalloc (DSIZE (repair_source_stack)+1, sizeof (SXINT)); /* // a repair_source_stack */
	repair_Tpq = (SXINT*) sxalloc (DSIZE (repair_source_stack)+1, sizeof (SXINT)); /* // a repair_source_stack */
	DALLOC_STACK (repair_rhs_stack, 50);

	XxY_alloc (&gen_tntXmaxlgth_repair_hd, "gen_tntXmaxlgth_repair_hd", 50, 1, 0, 0, gen_tntXmaxlgth_repair_hd_oflw, NULL);
	gen_tntXmaxlgth_repair_set = sxba_calloc (XxY_size (gen_tntXmaxlgth_repair_hd)+1);
	DALLOC_STACK (gen_proto_stack, 100);
      }
    }
    else {
      XxY_clear (&XxY_gen_repair_hd);
      /* repair_gen_nyp is emptyied before usage */
      DRAZ (gen_valid_repair_string_stack);
      DRAZ (gen_tnt_repair_stack);

      if (rcvr_spec.repair_kind != 1) {
	repair_path_stack [0] = 0;
	sxba_bag_clear (&repair_bag);
      }
      
      if (rcvr_spec.perform_repair_parse) {
	DRAZ (local_repair_prod_stack);
	DRAZ (glbl_repair_prod_stacks);
	DRAZ (repair_source_stack);
	DRAZ (repair_rhs_stack);
	XxY_clear (&gen_tntXmaxlgth_repair_hd);
	DRAZ (gen_proto_stack);
      }
    }
      
    if (rcvr_spec.perform_repair_parse) {
      gen_repair2attr2 [0].prod_stack = gen_repair2attr2 [0].nb = 0; /* Pour prefixe vide */
    }
  }
    
  /* On definit la distance entre 2 chaines a1 ... ap et b1 ... bp comme suit :
     on fabrique un ensemble D de doublets (i, j) avec 1<=i<=p et 1<=j<=q de la fac,on suivante
     Initialement, les chaines sont flottantes delta_i = delta_j = -1
     Pour i variant de 1 a p, la 1ere fois que ai==bj, on fige les chaines :
        Si i==j on a delta_i=delta_j=0
	Si i<j on a delta_i=j-i et delta_j=0
	Si i>j on a delta_i=0 et delta_j=i-j
     Si les chaines sont figees, la distance dhk entre deux lettres ah et bk est dhk=|h+delta_i-(k+delta_j)|
     Le passage de flottant a fige' a un cout nul

     Un premier passage sur les 2 chaines les figes et calcule le cout des deplacements de lettres identiques:
     si d est le cout d'un deplacement sur une distance de 1, si les chaines sont figees et si ah==bk le cout est d*dhk
     On somme tous les couts de deplacements.

     puis apres on calcule les couts de d'insertion, suppression/changement a partir des couts unitaires i,s et c.

     Si les chaines sont flottantes apres la 1ere passe on met delta_i=delta_j=0

     soit j = i+delta_i-delta_j
        si j < 1 ou j > q => suppression
	sinon si bj est non deplace => changement
	      si bj est deplace => suppression

     les bk qui n'ont ete ni changes ni deplaces sont inseres

     Pour que les couts du passage de a a` b et de b a` a soient identiques, il faut que i==s
  */
	
  varstr_raz (varstring);
  varstring = varstr_catenate (varstring, "Global Earley ");

  if (litem == 1 && ritem == 2)
    varstring = varstr_catenate (varstring, "trivial ");
    
  varstring = varstr_catenate (varstring, "recovery ");

  if (rcvr_spec.perform_repair) {
    perform_copy_local_repair_prod_stack = store_rcvr_string && rcvr_spec.perform_repair && rcvr_spec.perform_repair_parse;

#if HAS_BEAM
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) {
      /* Dans ce cas, on genere la chaine de proba max */
      /* Le travail est deja pre'mache' car nt2max_inside_logprob, t2max_inside_logprob et
	 nt2prod_that_generates_the_best_logprob_t_string sont calcule; */
      gen_empty_repair_string = SXFALSE, TOP (gen_valid_repair_string_stack) = 0;
      call_generate_best_logprob_string (litem, ritem, &gen_lgth);
    }
    else
    /* Ici gen_valid_repair_string_stack contient la chaine corrigee */
#endif /* HAS_BEAM */
      {
	min_gen_lgth = 0;
	cur_item = ritem;
    
	while (--cur_item >= litem) {
	  X = LISPRO (cur_item);

	  if (X < 0)
	    min_gen_lgth++;
	  else
	    min_gen_lgth += NT2MIN_GEN_LGTH (X);
	}
  
	/* min_gen_lgth est la taille minimale des chaines generees par les symboles entre [litem..ritem[ */


	gen_lgth = -1;

	/* On essaie de generer les chaines de longueur lgth_tried */
	if ((lgth_tried = rmlstn-lmlstn) >= min_gen_lgth && (rcvr_spec.suppression == -1 || rcvr_spec.insertion == -1)) {
	  /* gen_proto_stack contient une proto-phrase inverse initialisee a vide */
	  gen_empty_repair_string = SXFALSE, TOP (gen_valid_repair_string_stack) = 0, DTOP (gen_proto_stack) = 0;

	  if (!call_generate (litem, ritem, 0 /* old_prefix */, lgth_tried /* lgth */, 0 /* kind => |chaines| == lgth */,
			      0 /* taille min des phrases generees par la proto-phrase de gen_proto_stack */)) {
	    if (lgth_tried == min_gen_lgth)
	      sxtrap (ME, "earley_glbl_rcvr_message");

	    /* Echec : On n'est pas sur de pouvoir generer une chaine de longueur donnee (strictement superieure au min) */
	    /* on va donc generer le min */
	    XxY_clear (&XxY_gen_repair_hd); /* il peut contenir des cochonneries de l'essai rate' */
	  }
	  else
	    gen_lgth = lgth_tried;
	}
    
	if (gen_lgth == -1) {
	  /* On genere la/les chaines de taille min */
	  /* gen_proto_stack contient une proto-phrase inverse initialisee a vide */
	  gen_empty_repair_string = SXFALSE, TOP (gen_valid_repair_string_stack) = 0, DTOP (gen_proto_stack) = 0;

	  if (!call_generate (litem, ritem, 0 /* old_prefix */, min_gen_lgth, 0 /* kind => |chaines| == lgth */,
			      0 /* taille min des phrases generees par la proto-phrase de gen_proto_stack */))
	    sxtrap (ME, "earley_glbl_rcvr_message");

	  gen_lgth = min_gen_lgth;
	}
      }

#if LOG
    fputs ("For ", stdout);
    output_rcvr_prod (litem, lmlstn, ritem, rmlstn);
    fputs (" The valid repair strings are:\n", stdout);

    for (x = TOP (gen_valid_repair_string_stack); x >= 1; x--) {
      top = gen_valid_repair_string_stack [x];
      walk_gen_string (top, print_gen_string, NULL);
      fputs ("\n", stdout);
    }

    if (gen_lgth == 0) {
      /* representation de la chaine vide */
      fputs ("\"\"\n", stdout);
    }
#endif /* LOG */
  }
  else
    gen_lgth = -1;

  if (lmlstn < rmlstn) {
    varstring = varstr_catenate (varstring, "replaces the former \"");
    varstring = sub_dag_to_re (varstring, ldag_state, rdag_state);
    varstring = varstr_catenate (varstring, "\" starting at line ");

    if (lmlstn == idag.final_state)
      xtok = idag.toks_buf_top; /* eof de fin */
    else {
      xtok= idag.pq2tok_no [idag.p2pq_hd [lmlstn]];

      if (xtok < 0)
	xtok = idag.tok_no_stack [-xtok+1];
    }

    lsource_index = SXGET_TOKEN (xtok).source_index;

    sprintf (int_str, SXILD, lsource_index.line);
    varstring = varstr_catenate (varstring, int_str);
    varstring = varstr_catenate (varstring, " column ");
    sprintf (int_str, SXILD, lsource_index.column);
    varstring = varstr_catenate (varstring, int_str);

    varstring = varstr_catenate (varstring, " by ");

    if (gen_empty_repair_string)
      varstring = varstr_catenate (varstring, "the empty string");
  }
  else
     varstring = varstr_catenate (varstring, "inserts ");

  if (gen_lgth == -1) {
    varstring = varstr_catenate (varstring, "a string");
    best_repair = -1;
  }
  else {
    /* On choisit LA chaine corrigee */
    if (gen_lgth == 0)
      /* Unique */
      best_repair = 0;
    else {
      if (rcvr_spec.repair_kind == 1) {
	/* On a genere' une seule chaine, c'est celle_la */
	best_repair = gen_valid_repair_string_stack [1];
      }
      else {
	if (rcvr_gen_dag_paths (ldag_state, rdag_state, NULL)) {
	  /* Il y a un chemin ds le source entre ldag_state et rdag_state */
	  best_distance = -1;

	  rcvr_gen_dag_paths (ldag_state, rdag_state, rcvr_distance);
	  /* best_distance et best_repair sont positionnes */

	  if (for_semact.rcvr)
	    best_repair = (*for_semact.rcvr) (/* ?? a voir */);
	}
	else {
	  /* pas de chemin ds le source entre lmlstn et rmlstn */
	  if (for_semact.rcvr)
	    best_repair = (*for_semact.rcvr) (/* ?? a voir */);
	  else
	    /* On prend la 1ere */
	    best_repair = gen_valid_repair_string_stack [1];
	}
      }
    }

    really_store = store_rcvr_string && !(rcvr_spec.perform_repair && rcvr_spec.perform_repair_parse);

    if (really_store && !XxYxZ_is_allocated (store_rcvr_string_hd)) {
      sxprepare_for_possible_reset (store_rcvr_string_hd.is_allocated);
      XxYxZ_alloc (&store_rcvr_string_hd, "store_rcvr_string_hd", XxYxZ_top (glbl_dag_rcvr_start_kitems)+1, 1,
		   store_rcvr_string_hd_foreach, NULL, NULL);
    }

    if (gen_lgth) {
      /* Pas la chaine vide */
      SXINT bot, ste, triple;

      varstring = varstr_catenate (varstring, "\"");

      if (really_store)
	bot = varstr_length (varstring);

      walk_gen_string (best_repair, get_gen_string, NULL);

      if (really_store) {
	ste = sxstr2save (varstring->first+bot, varstr_length (varstring)-bot);
	XxYxZ_set (&store_rcvr_string_hd, ste, lmlstn, rmlstn, &triple);
      }
    
      varstring = varstr_catenate (varstring, "\"");
    }
    else {
      if (really_store) {
	XxYxZ_set (&store_rcvr_string_hd, SXEMPTY_STE, lmlstn, rmlstn, &triple);
      }
    }
  }

  has_a_nt = SXFALSE;

  if (litem < ritem) {
    for (cur_item = litem; cur_item < ritem; cur_item++) {
      if (LISPRO (cur_item) > 0) {
	has_a_nt = SXTRUE;
	break;
      }
    }

    if (has_a_nt) {
      /* Y'a du nt ds le coup (sinon chaine terminale qui a genere elle-meme) */
      varstring = varstr_catenate (varstring, " generated by ");

      for (cur_item = litem; cur_item < ritem; cur_item++) {
	if (cur_item > litem)
	  varstring = varstr_catchar (varstring, ' ');

	X = LISPRO (cur_item);

	if (X < 0) {
	  varstring = varstr_catchar (varstring, '"');
	  varstring = varstr_catenate (varstring, TSTRING (-X));
	  varstring = varstr_catchar (varstring, '"');
	}
	else {
	  varstring = varstr_catchar (varstring, '<');
	  varstring = varstr_catenate (varstring, NTSTRING (X));
	  varstring = varstr_catchar (varstring, '>');
	}

	varstr_complete (varstring);
      }
    }
  }
  
  if (lmlstn == idag.final_state)
    xtok = idag.toks_buf_top; /* eof de fin */
  else {
    xtok= idag.pq2tok_no [idag.p2pq_hd [lmlstn]];

    if (xtok < 0)
      xtok = idag.tok_no_stack [-xtok+1];
  }

  rsource_index = SXGET_TOKEN (xtok).source_index;
  
  sxerror (rsource_index,
	   sxplocals.sxtables->err_titles [kind][0],
	   "%s%s.",
	   sxplocals.sxtables->err_titles [kind]+1,
	   varstr_tostr (varstring));
  
  /* Analyse de la chaine corrigee */
  if (perform_copy_local_repair_prod_stack && best_repair >= 0) {
    spf.outputG.has_repair = SXTRUE; /* spf.outputG sert de structure meme si !is_parse_forest */

    if (ritem > litem /* Le 09/01/07 Sinon suppression d'une chaine qui se fait sans analyse */
	/* && has_a_nt Enleve' le 28/02/06 pour avoir ces terminaux ds Tpq_rcvr_set */) {
      /* la chaine vide correspond a de la suppression (non decrite par des productions de la reparation) (parfois faux de'sormais?) */
      parse_id = call_td_repair_parses (lmlstn, rmlstn, litem, ritem, best_repair, gen_repair2attr2 [best_repair].prod_stack);
    }
  }
  else {
    parse_id = 0;

    if (store_rcvr_string) {
      xt_complete (lmlstn, rmlstn);
      sxba_empty (idag.path [0]);
    }
  }

  return parse_id; /* identifiant de la correction instanciee */
}


/* Nelle Nelle Nelle version le 24/10/06 */
/* ATTENTION la restriction du rattrapage impose que la recuperation se produise dans LA MEME production entre
   [A -> \alpha X [mlstn] \beta [next_mlstn] \gamma] */
/* left_item = [A -> \alpha X . \beta \gamma] est un item du kernel de T[mlstn].
   Il a pu etre mis par un shift (X, prev_mlst, mlstn) et ses "successeurs" ont donc fini par echouer.
   A-t-il tj un index_set non vide ? A VOIR
   item = [A -> \alpha X \beta . \gamma] est un item droit non reduce >= a left_item
*/
/* item ne peut plus etre ni en position reduce ni en position initiale */
static SXBOOLEAN
earley_glbl_rcvr_set (SXINT left_mlstn, SXINT left_item, SXINT right_mlstn, SXINT right_item)
{
  SXINT     X, **ptr2, cur_item, resume, triple; 
  SXBA      index_set, left_index_set, backward_index_set;
  SXBOOLEAN ret_val, has_cur_indexes, first_hit;

  X = LISPRO (right_item);

#if EBUG
  if (X == 0 || LISPRO (right_item-1) == 0)
    sxtrap (ME, "earley_glbl_rcvr_set");
#endif /* EBUG */
  
  /* Le 12/12/05 */
  /* L'utilisateur peut interdire la suppression */
  if (rcvr_spec.suppression == -1 && left_item == right_item && left_mlstn < right_mlstn)
    /* suppression de left_mlstn .. right_mlstn pour matcher la chaine vide entre left_item && right_item */
    return SXFALSE;

  /* Il peut ne pas y avoir de chemin ds le dag entre left_mlstn et right_mlstn */

#if LOG
  fputs ("trying ...\t", stdout);
  output_rcvr_prod (left_item, left_mlstn, right_item, right_mlstn);
#endif /* LOG */

  if (ret_val = ((X < 0 && SXBA_bit_is_set (rcvr_w2titem_set, right_item))
		 || (X > 0 && IS_AND (rcvr_w2titem_set, NT2ITEM_SET (X))))) {
    /* La rcvr a ete validee, on stocke les items ds les tables ... */  /* ici right_item est "bon" */
    left_index_set = RT [left_mlstn].index_sets [left_item];
    first_hit = (index_set = T2_index_sets [right_item]) == NULL;

#if HAS_BEAM == 1
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) {
      SXINT                 i;
      SXLOGPROB                inside_logprob = get_max_inside_rcvr_logprob (left_item, right_item);

      has_cur_indexes = SXFALSE;

      if (left_index_set == NULL) {
	/* left_item est le debut de la rhs !! */
	if (set_forward_logprob (left_mlstn, left_mlstn, right_mlstn, left_item, right_item, 0 /* rcvr */, NULL /* pq_ptr */, &inside_logprob)) {
	  if (index_set == NULL)
	    index_set = T2_index_sets [right_item] = sxba_bag_get (&shift_bag, right_mlstn+1);

	  has_cur_indexes = SXTRUE;
	  SXBA_1_bit (index_set, left_mlstn);
	}
      }
      else {
	/* OR (index_set, left_index_set); */
	i = -1;

	while ((i = sxba_scan (left_index_set, i)) >= 0) {
	  if (set_forward_logprob (i, left_mlstn, right_mlstn, left_item, right_item, 0 /* rcvr */, NULL /* pq_ptr */, &inside_logprob)) {
	    if (index_set == NULL)
	      index_set = T2_index_sets [right_item] = sxba_bag_get (&shift_bag, right_mlstn+1);

	    has_cur_indexes = SXTRUE;
	    SXBA_1_bit (index_set, i);
	  }
	}
      }
    }
    else
#endif /* HAS_BEAM == 1 */
      {
	if (index_set == NULL)
	  index_set = T2_index_sets [right_item] = sxba_bag_get (&shift_bag, right_mlstn+1);

	has_cur_indexes = SXTRUE;

	if (left_index_set == NULL) {
	  /* left_item est le debut de la rhs !! */
	  SXBA_1_bit (index_set, left_mlstn);
	}
	else
	  OR (index_set, left_index_set);
      }

    if (has_cur_indexes) {
      /* Le 22/09/06 */
      /* On est en rcvr, dans tous les cas (que T2_shift_NT_hd [X] soit NULL ou pas)
	 il faut recalculer T2_non_kernel_item_set qui cette fois sera non filtre'
	 par la lexicalisation
      */
      if (X > 0)
	/* pas de finesse avec left_corner !! */
	sxba_or (T2_non_kernel_item_set, NT2ITEM_SET (X));
      else
	X = 0;

      /* ret_val n'est positionne que s'il y a eu du changement */
      if (*(ptr2 = &(T2_shift_NT_hd [X])) == NULL) {
	/* c'est la 1ere fois qu'on rencontre X */
	/* rhs_nt2where [0] == 0 (pour les terminaux) */
	*ptr2 = &(T2_items_stack [RHS_NT2WHERE (X)]);
	*((*ptr2)++) = right_item;
      }
      else {
	if (first_hit)
	  *((*ptr2)++) = right_item;
	/* sinon, il s'y trouve deja (Le 04/10/05) */
      }
    }

    SXBA_1_bit (mlstn_active_set, right_mlstn);

    /* On marque la reussite ... */
    XxY_set (&glbl_dag_rcvr_resume_kitems, right_mlstn, right_item, &resume);
    /* Ajoute' le 20/09/10 pour accelerer le XxY_is_set (&glbl_dag_rcvr_resume_kitems) */
    SXBA_1_bit (glbl_dag_rcvr_resume_k_set, right_mlstn);
    SXBA_1_bit (glbl_dag_rcvr_resume_item_set, right_item);

#if EBUG
    if (triple = XxYxZ_is_set (&glbl_dag_rcvr_start_kitems, left_mlstn, left_item, resume))
      /* Juste pour savoir si ce cas est possible */
      sxtrap (ME, "earley_glbl_rcvr_set");
#endif /* EBUG */

    XxYxZ_set (&glbl_dag_rcvr_start_kitems, left_mlstn, left_item, resume, &triple);

#if LOG
    fputs ("\n", stdout);
    earley_glbl_rcvr_message (left_item, left_mlstn, right_item, right_mlstn, 1 /* warning */, SXFALSE);
#endif /* LOG */
  }

#if LOG
  printf (" ..., %s\n", ret_val ? "succeeds\n" : "fails");
#endif /* LOG */

  return ret_val;
}


/* item_p : A -> \alpha . \beta
   On simule une transition entre p et q sur "\beta" */
static SXBOOLEAN
earley_glbl_rcvr_reduce_pre (SXINT p, SXINT item_p, SXINT q, SXBA rcvr_w2titem_set)
{
  SXINT                 prod, A, i, item_q;
  SXBA                  index_set_p, save_T2_non_kernel_item_set;
  SXBOOLEAN             ret_val, has_reduce, shift_has_succeeded;

  /* Les "T2" sont positionnes */
  prod = PROLIS (item_p);
  A = LHS (prod);
  index_set_p = RT [p].index_sets [item_p];
  item_q = PROLON (prod+1)-1;

#if LOG
  fputs ("trying pre reduce ...\t", stdout);
  output_rcvr_prod (item_p, p, item_q, q);
  fputs ("\n", stdout);
#endif /* LOG */

  ret_val = SXFALSE;
  
  i = (index_set_p == NULL) ? p : sxba_scan (index_set_p, -1);

  while (i >= 0) {
    has_hit_a_valid_kernel_terminal_item = SXFALSE;
    save_T2_non_kernel_item_set = T2_non_kernel_item_set;
    T2_non_kernel_item_set = rcvr_reduce_T2_non_kernel_item_set;

#if HAS_BEAM == 1
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set)
      shift_has_succeeded = beam_shift (A, i, q);
    else
#endif /* HAS_BEAM == 1 */
      shift_has_succeeded = shift (A, i, q);
   
    if (shift_has_succeeded) {
      if (
	  TOP (reduce_list)
	  )
	scan_reduce (i, q);

      if (has_hit_a_valid_kernel_terminal_item /* detection d'un nouvel item scan valide du kernel ... */
	  || sxba_2op (NULL, SXBA_OP_IS, T2_non_kernel_item_set, SXBA_OP_AND, rcvr_w2titem_set) /* ... ou item scan valide non kernel */) {
	ret_val = SXTRUE;

	sxba_or (save_T2_non_kernel_item_set, rcvr_reduce_T2_non_kernel_item_set);

	DPUSH (rcvr_reduce_item_stack, -i);
      }
    }

    T2_non_kernel_item_set = save_T2_non_kernel_item_set;
    sxba_empty (rcvr_reduce_T2_non_kernel_item_set);

    i = (index_set_p == NULL) ? -1 : sxba_scan (index_set_p, i);
  }

  if (ret_val)
    DPUSH (rcvr_reduce_item_stack, item_p);

#if LOG
  printf (" ..., %s during pre validation\n", ret_val ? "succeeds" : "fails");
#endif /* LOG */

  return ret_val;
}


static void
earley_glbl_rcvr_reduce_post (SXINT p, SXINT q)
{
  SXINT                 item_p, prod, A, i, item_q, resume, triple;
  SXINT                 **ptr2;
  SXBA                  index_set_q, save_T2_non_kernel_item_set;
  SXBOOLEAN             has_reduce, shift_has_succeeded;

#if HAS_BEAM == 1
  SXINT                 iq;
  struct logprob_struct   *p_logprob_ptr, *q_logprob_ptr;
  SXBOOLEAN             exists;
  SXLOGPROB                old_logprob, new_left_inside_logprob, new_logprob, rcvr_inside_logprob, rcvr_logprob;
#endif /* HAS_BEAM == 1 */

  set_T2 (q); /* On met la bonne table */

  while (!IS_EMPTY (rcvr_reduce_item_stack)) {
    item_p = POP (rcvr_reduce_item_stack);
    prod = PROLIS (item_p);
    A = LHS (prod);
    item_q = PROLON (prod+1)-1;

#if LOG
    fputs ("Entering post reduce ...\t", stdout);
    output_rcvr_prod (item_p, p, item_q, q);
    fputs ("\n", stdout);
#endif /* LOG */

#if HAS_BEAM == 1
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) {
      /* On a besoin de le faire avant l'appel a scan_reduce () car on y a besoin des Aiq.
	 d'autre part, on est sur que ce qu'on construit est valide (la phase de validation rst passee) */
      rcvr_inside_logprob = get_max_inside_rcvr_logprob (item_p, item_q);
      rcvr_logprob = rcvr_inside_logprob + prod2logprob (prod);
    }
#endif /* HAS_BEAM == 1 */

    do {
      /* ... les index initiaux */
      i = POP (rcvr_reduce_item_stack);

      if (i > 0) {
	/* c'est un item */
	DPUSH (rcvr_reduce_item_stack, i);
	break;
      }

      i = -i;

#if HAS_BEAM == 1
      if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) {
	/* On a besoin de le faire avant l'appel a scan_reduce () car on y a besoin des Aiq.
	   d'autre part, on est sur que ce qu'on construit est valide (la phase de validation rst passee) */
	ei2get_logprob_ptr (item_p, p_q2pq (i, p), p_logprob_ptr);
	new_logprob = p_logprob_ptr->inside.left + rcvr_logprob;
	iq = p_q2pq (i, q);

	if (Axpq2set_max_logprob_ptr (A, iq)) {
	  /* existe */
	  if (cmp_SXLOGPROB (max_logprob_ptr->inside, new_logprob) < 0 /* < */) {
	    max_logprob_ptr->inside = new_logprob;
#if LLOG
	    output_Apq_logprobs ("Better ", A, i, q, max_logprob_ptr);
#endif /* LLOG */
	  }
	}
	else {
	  max_logprob_ptr->inside = new_logprob;
#if LLOG
	  output_Apq_logprobs ("New ", A, i, q, max_logprob_ptr);
#endif /* LLOG */
	}

	new_left_inside_logprob = p_logprob_ptr->inside.left + rcvr_inside_logprob;

	if (ei2set_logprob_ptr (item_q, iq, q_logprob_ptr)) {
# if RECOGNIZER_KIND == 2
	  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
	    old_logprob = q_logprob_ptr->expected_total;
	  else
# else /* RECOGNIZER_KIND == 2 */
	    old_logprob = q_logprob_ptr->inside.left;
# endif /* RECOGNIZER_KIND == 2 */

	  new_logprob = new_left_inside_logprob;

# if RECOGNIZER_KIND == 2
	  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
	    new_logprob += p_logprob_ptr->outside;
# endif /* RECOGNIZER_KIND == 2 */
	}

	if (!is_ei_set || cmp_SXLOGPROB (old_logprob, new_logprob) < 0 /* < */) {
	  /* On ne garde que la meilleure proba totale */
	  q_logprob_ptr->inside.left = new_left_inside_logprob;

# if RECOGNIZER_KIND == 2
	  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
	    q_logprob_ptr->outside = p_logprob_ptr->outside;
	    q_logprob_ptr->expected_total = new_logprob;
	  }
# endif /* RECOGNIZER_KIND == 2 */
	}
      }

      if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set)
	  shift_has_succeeded = beam_shift (A, i, q);
	else
#endif /* HAS_BEAM == 1 */
	  shift_has_succeeded = shift (A, i, q);
    
      if (shift_has_succeeded) {
	if (
	    TOP (reduce_list)
	    )
	  scan_reduce (i, q);

	/* Attention les [beam_]shift ont pu allouer T2_index_sets [item_q]
	 Il faut donc reaffecter index_set_q a chaque tour de boucle */
	index_set_q = T2_index_sets [item_q];

	if (index_set_q == NULL) {
	  /* C'est la 1ere fois qu'on voit item_q ds la table T [q] */
	  index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	  if (*(ptr2 = &(T2_reduce_NT_hd [A])) == NULL) {
	    /* c'est la 1ere fois qu'on fait un reduce vers A */
	    *ptr2 = &(T2_items_stack [LHS_NT2WHERE (A)]);
	  }

	  *((*ptr2)++) = item_q;
	}

	SXBA_1_bit (index_set_q, i);
      }
    } while (!IS_EMPTY (rcvr_reduce_item_stack));

#if EBUG
    if (index_set_q == NULL)
      sxtrap (ME, "earley_glbl_rcvr_reduce_post");
#endif /* EBUG */

    /* On marque la reussite ... */
    XxY_set (&glbl_dag_rcvr_resume_kitems, q, item_q, &resume);
    /* Ajoute' le 20/09/10 pour accelerer le XxY_is_set (&glbl_dag_rcvr_resume_kitems) */
    SXBA_1_bit (glbl_dag_rcvr_resume_item_set, item_q);

#if EBUG
    if (triple = XxYxZ_is_set (&glbl_dag_rcvr_start_kitems, p, item_p, resume))
      /* Juste pour savoir si ce cas est possible */
      sxtrap (ME, "earley_glbl_rcvr_set");
#endif /* EBUG */

    XxYxZ_set (&glbl_dag_rcvr_start_kitems, p, item_p, resume, &triple);

#if LOG
    earley_glbl_rcvr_message (item_p, p, item_q, q, 1 /* warning */, SXFALSE);
#endif /* LOG */

#if !is_parser
#if is_supertagging
    supertag (prod, sxba_scan (index_set_q, 0), q);
#endif /* is_supertagging */

#if is_guide
    /* Le guide peut etre produit soit par le reconnaisseur (ici) soit par RL_mreduction */
    /* On utilise valid_index_set_q (et non index_set_q) ca peut eviter certains appels (prod, i, j) deja
       effectues lors d'un appel precedent de shift() */
    output_guide (prod, index_set_q, p, q);
#endif /* is_guide */
#endif /* !is_parser */

#if LOG
    output_ni_prod (prod, index_set_q, p, q);
#endif /* LOG */

#if LOG
    fputs ("Leaving post reduce ...\t", stdout);
    output_rcvr_prod (item_p, p, item_q, q);
    fputs ("\n", stdout);
#endif /* LOG */
  }

  /* Ajoute' le 20/09/10 pour accelerer le XxY_is_set (&glbl_dag_rcvr_resume_kitems) */
  SXBA_1_bit (glbl_dag_rcvr_resume_k_set, q);
  SXBA_1_bit (mlstn_active_set, q);
}


/* Le 26/10/06 : Nelle Nelle version (introduction des TRY_MIN..TRY_MAX) */
/* Le 27/01/06 : Nelle version*/
/* rcvr ds le cas is_dag et !is_dag (is_sdag et normal) */
/* la borne (p=mlstn, q=next_mlstn) est impose' de l'exterieur par la strategie de rattrapage utilisee */
/* On ne traite pas les cas reduce [A -> \alpha ., i] car on a i <= p. Si i==p, le reduce B -> \beta . A \gamma
   a ete ou sera localement, si i < p, ce cas sera traite' par une autre borne */
static SXBOOLEAN        working_rcvr_mlstn_tables_must_be_cleared;
static SXINT
earley_glbl_rcvr (mlstn, next_mlstn)
     SXINT mlstn, next_mlstn;
{
  SXINT                   top_mlstn, next_dag_state, trans, X, Y, t, A, B, save_try_kind, top, x;
  SXINT                   item, prod, top_item, bot_item, cur_item, titem, prev_mlstn, prev_prod, bot_numpg, top_numpg;
  SXBA                  lex_compatible_item_set, *index_sets_mlstn, *index_sets_next_mlstn;
  SXBOOLEAN               local_ret_val = SXFALSE, call_process_non_kernel_mlstn;
  SXINT                   ret_val = 0, rcvr_kernel_nb;
  struct recognize_item	*RTq;
  SXINT		        *bot_ptr, *top_ptr, *bot_ptr2, *X_top_ptr;
  SXBA                  top_ptr2, source_t_set;
  static SXINT		**shift_NT_hd_mlstn, *items_stack_mlstn;
  static SXBA           rcvr_kernel_item_set;
  static SXBA           rcvr_reduce_item_set;

  if (mlstn == next_mlstn) {
    if (rcvr_spec.insertion == -1 /* pas d'insertion ... */)
      /* ... essai qui s'il marchait produirait necessairement de l'insertion, on l'abandonne donc a priori */
      return 0;

    /* Le 26/10/06 */
    /*
      Ds l'etat actuel, si on permet c,a, on va pouvoir faire cohabiter ds le meme etat des items qui vont se connecter
      de facon perverse : G =
    = $ <E> $ ;
     0 1   2 3
<E> = ( <E> ) ;
     4 5   6 7
<E> = <E> + <E> ;
     8   9 10  11
<E> = <E> * <E> ;
     12  1314  15
<E> = <E> <E> ;
     16  17  18
<E> = a ;
     1920

     source : \( + \)
             1  2 3  4

     Les corrections inserent en 2 et 3 respectivement "a | a \)" et "a | \( a" pour produire le DAG

             a             a
            ___           ___
       (   /   \    +    /   \   )
     1___2/     \2_____3/     \3___4
          \     /       \     /
          a\   /)       (\   /a
            \6/           \7/


     Si on rentre ce DAG comme source, 
             a             a
            ___           ___
       (   /   \    +    /   \   )
     1___2/     \4_____5/     \7___8
          \     /       \     /
          a\   /)       (\   /a
            \3/           \6/


     on obtient le genere' de gauche alors que le genere du apres corrections est celui de droite (les prods instanciees
     supplementaires correspondent a la connexion de la partie haute (resp. basse) de la partie gauche du DAG avec la
     partie basse (resp. haute) de la partie droite du DAG ce qui est bien sur errone'

1/5:<E[2..3]>   = "a"[2..3] ;                       12/5:<E[2::6]>  = "a"[2::6] ;
2/1:<E[1..4]>   = "("[1..2] <E[2..3]> ")"[3..4] ;    1/1:<E[1..2]>   = "("[1..2] <E[2::6]> ")"[6::2] ;
3/5:<E[2..4]>   = "a"[2..4] ;                       14/5:<E[2::2]>  = "a"[2::2] ;
4/5:<E[5..7]>   = "a"[5..7] ;                       13/5:<E[3::3]>  = "a"[3::3] ;
5/5:<E[6..7]>   = "a"[6..7] ;                       15/5:<E[7::3]>  = "a"[7::3] ;
6/2:<E[2..7]>   = <E[2..4]> "+"[4..5] <E[5..7]> ;    3/2:<E[2..3]>   = <E[2::2]> "+"[2..3] <E[3::3]> ;
7/1:<E[1..8]>   = "("[1..2] <E[2..7]> ")"[7..8] ;    5/1:<E[1..4]>   = "("[1..2] <E[2..3]> ")"[3..4] ;
8/1:<E[5..8]>   = "("[5..6] <E[6..7]> ")"[7..8] ;    6/1:<E[3..4]>   = "("[3::7] <E[7::3]> ")"[3..4] ;
9/2:<E[1..8]>   = <E[1..4]> "+"[4..5] <E[5..8]> ;    7/2:<E[1..4]>   = <E[1..2]> "+"[2..3] <E[3..4]> ;

                                                     2/2:<E[1..3]>   = <E[1..2]> "+"[2..3] <E[3::3]> ;
                                                     4/4:<E[1..3]>   = <E[1..2]> <E[2..3]> ;
                                                     8/2:<E[2..4]>   = <E[2::2]> "+"[2..3] <E[3..4]> ;
                                                     9/4:<E[1..4]>   = <E[1..2]> <E[2..4]> ;
                                                    10/4:<E[1..4]>  = <E[1..3]> <E[3..4]> ;
                                                    11/4:<E[2..4]>  = <E[2..3]> <E[3..4]> ;

    */
    /*
     Il faudrait en fait remplir non pas la table mlstn == next_mlstn mais une nelle table (au-dela de final_mlstn)
     ... mais PB des mlstn sequentiels
    */
    /*
      Pour quand meme faire un petit qqchose, j'essaie une correction unique
    */
    save_try_kind = rcvr_spec.try_kind;
    rcvr_spec.try_kind = TRY_MIN; /* C,a permet de ne pas essayer les autres des qu'une marche */
  }

  if (varstring == NULL) {
    sxprepare_for_possible_reset (varstring);
    varstring = varstr_alloc (128);
    
#if is_parser
    /* Cette structure memorise les couples (mlstn, item) sur lesquels se produisent de la
       recuperation globale */
    sxprepare_for_possible_reset (glbl_dag_rcvr_resume_kitems.is_allocated);
    XxY_alloc (&glbl_dag_rcvr_resume_kitems, "glbl_dag_rcvr_resume_kitems", MAXITEM+1, 1, 1, 0, NULL, NULL);
    glbl_dag_rcvr_resume_k_set = sxba_calloc (idag.final_state+1);
    glbl_dag_rcvr_resume_item_set = sxba_calloc (MAXITEM+1);
    XxYxZ_alloc (&glbl_dag_rcvr_start_kitems, "glbl_dag_rcvr_start_kitems", MAXITEM+1, 1,
		 glbl_dag_rcvr_start_kitems_foreach, NULL, NULL);
#endif /* is_parser */
  }

  /* Attention, des etats deja calcules et touches par le traitement d'erreur doivent etre recalcules */
  top_mlstn = idag.final_state+rcvr_tok_no;

  /* Le 24/10/06 */
  rcvr_kernel_item_set = rcvr_kernel_item_sets [mlstn];
  /* L'analyse normale (shift p ->X q) a note' ds rcvr_kernel_item_sets[q] les items kernel de la forme A -> \alpha X . Y \beta */
  /* Si Y > 0 on va essayer de se recuperer sur les items non reduce des productions de lc [Y] */
  /* Et sur les items A -> \alpha X Y \beta_1 . \beta_2 (y compris l'item reduce) */
  sxba_empty (rcvr_non_kernel_item_set);
  item = -1;

  while ((item = sxba_scan (rcvr_kernel_item_set, item)) >= 0) {
    if ((Y = LISPRO (item)) > 0)
      sxba_or (rcvr_non_kernel_item_set, NT2ITEM_SET (Y));
  }

  rcvr_reduce_item_set = rcvr_reduce_item_sets [mlstn];

#if is_epsilon
#ifdef MAKE_INSIDEG
  if (spf.insideG.is_eps) {
    sxba_minus (rcvr_non_kernel_item_set, EMPTY_PROD_ITEM_SET);
  }
#else /* !MAKE_INSIDEG */
  sxba_minus (rcvr_non_kernel_item_set, EMPTY_PROD_ITEM_SET);
#endif /* !MAKE_INSIDEG */
#endif /* is_epsilon */

  if (sxba_scan (rcvr_kernel_item_set, -1) == -1
      && sxba_scan (rcvr_reduce_item_set, -1) == -1 /* pas de reduce */
      ) {
    /* T[mlstn] est vide */
    if (mlstn == next_mlstn)
      rcvr_spec.try_kind = save_try_kind;

    return 0;
  }

  /* Le 12/12/05 */
  if (next_mlstn > mlstn) {
    /* On va remplir une table de travail toute neuve */
    clear_table (next_mlstn);
  }
#if EBUG
  else {
    if (next_mlstn < mlstn || mlstn != rcvr_detection_table)
      sxtrap (ME, "earley_glbl_rcvr"); 
    /* mlstn == next_mlstn == rcvr_detection_table
       Ds ce cas, s'il y a echec, il faut remettre T [mlstn] a son etat initial pour
       les tentatives ulterieures.
       T [mlstn] a deja ete sauve' ds earley_no_dag_rcvr par un copy_table qui doit etre complet (pas uniquement les pointeurs) */
  }
#endif /* EBUG */   

  source_t_set = mlstn2non_lexicalized_look_ahead_t_set [next_mlstn];
  sxba_empty (rcvr_w2item_set);

#if LOG
  printf ("look_ahead_set [" SXILD "] = {", next_mlstn);
  t = sxba_scan (source_t_set, 0);
  printf ("\"%s\"", TSTRING (t));

  while ((t = sxba_scan (source_t_set, t)) > 0)
    printf (", \"%s\"", TSTRING (t));
    
  fputs ("}\n", stdout);
#endif /* LOG */

  set_T2 (next_mlstn);
  index_sets_next_mlstn = T2_index_sets;

  /* Attention : il peut y avoir ds T2_non_kernel_item_set des items de la forme
     A -> \alpha .t \beta
     qui ne sont pas ds T2_shift_NT_hd [0] car t n'est pas une transition valide depuis dag_state.
     Il est difficile de les "recuperer" a cause de l'index_set qu'il faudrait leur associer : ds ce
     cas on pourrait avoir un index_set associe a un item initial !! */

  /* On fait la liste des items concernes par les terminaux de source_t_set */
  t = 0;

  while ((t = sxba_scan (source_t_set, t)) > 0) {
    sxba_or (rcvr_w2item_set, T2ITEM_SET (t));
  }

  sxba_copy (rcvr_w2titem_set, rcvr_w2item_set);
  /* rcvr_w2titem_set contient les items terminaux */
  sxba_and (rcvr_w2titem_set, TITEM_SET);

  /* On regarde en premier les (fermetures des) items non reduce de rcvr_kernel_item_set */
  /* A voir si l'absence de filtrage apporte qqchose ds le cas next_mlstn == mlstn ?? */
  if (next_mlstn > mlstn) {
    item = 3; /* Pas de recup sur 0: S' -> $ S $ */
    /* Sur un exemple j'ai eu de la recup sur 0: S' -> $ S [3] [4] $ qui a donc supprime' le dernier mot (entre 3 et 4)
       et de la recup S -> \alpha [3] [4] ; qui a supprime' ce dernier mot sur une autre prod */
    /* Finalement, le 09/01/07 on essaie item = 0; si item=3 a echoue' car ca peut etre le dernier espoir!! */
    /* 2/9/08 eh bien on préfère échouer plutôt que se rattraper sur la superrègle */
    while ((item = sxba_scan (rcvr_kernel_item_set, item)) >= 0) {
      prod = PROLIS (item);
      top_item = PROLON (prod+1)-1;
      
      if (item < top_item) {
	/* Non reduce */
	if (earley_glbl_rcvr_set (mlstn, item, next_mlstn, item))
	  local_ret_val = SXTRUE;
      }
    
      /* Il peut y avoir plusieurs items kernel de la meme prod ...
	 prudence, on ne traite que le + a gauche */
      item = top_item;
    }
  }


  if (!local_ret_val || rcvr_spec.try_kind >= TRY_LOW) {
    /* On va parcourir les items droits des prods non_kernel */
    item = 3;
    
    while ((item = sxba_scan (rcvr_non_kernel_item_set, item)) >= 0) {
      prod = PROLIS (item);

#if EBUG
      if (item != PROLON (prod))
	/* Non initial */
	sxtrap (ME, "earley_glbl_rcvr");
#endif /* EBUG */

      top_item = PROLON (prod+1)-1;
    
      for (cur_item = item+1; cur_item < top_item; cur_item++) {
	if (SXBA_bit_is_set (rcvr_kernel_item_set, cur_item))
	  /* les items du kernel ou les suivants ont deja ete examines ou le seront + tard */
	  break;

	if (index_sets_next_mlstn [cur_item] == NULL /* Le 28/09/05 cet item est nouveau */) {
	  if (earley_glbl_rcvr_set (mlstn, item, next_mlstn, cur_item)) {
	    local_ret_val = SXTRUE;

	    if (mlstn == next_mlstn)
	      /* Une correction unique (voir pave' du debut) */
	      break;
	  }
	}
      }

      item = top_item;
    }
  }

  call_process_non_kernel_mlstn = local_ret_val && (mlstn != next_mlstn);

  if (!local_ret_val || rcvr_spec.try_kind >= TRY_MEDIUM) {
    /* On va parcourir les items droits non reduce des items kernel */
    /* On ne traite pas les reduce a cause du set_T2, ils seront fait ds le bloc suivant */
    item = 3;

    while ((item = sxba_scan (rcvr_kernel_item_set, item)) >= 0) {
      prod = PROLIS (item);
      top_item = PROLON (prod+1)-1;

      for (cur_item = item+1; cur_item < top_item; cur_item++) {
	if (index_sets_next_mlstn [cur_item]/* index_set */ == NULL) {
	  if (earley_glbl_rcvr_set (mlstn, item, next_mlstn, cur_item)) {
	    local_ret_val = SXTRUE;

	    if (mlstn == next_mlstn)
	      /* Une correction unique (voir pave' du debut) */
	      break;
	  }
	}
      }

      item = top_item;
    }
  }

  if (!local_ret_val || rcvr_spec.try_kind >= TRY_HIGH) {
    /* On parcourt les items reduce du kernel ... */
    if (next_mlstn > mlstn) {
      /* On essaie de se recuperer sur les reduce */
      sxba_empty (rcvr_prod_set);
      item = 3; /* Pas de recup sur 0: S' -> $ S $ */

      while ((item = sxba_scan (rcvr_reduce_item_set, item)) >= 0) {
	/* item = [A -> \alpha ., i] \in T[mlstn] */
	prod = PROLIS (item);
	bot_item = PROLON (prod);

	if (item > bot_item /* pas prod vide */ && SXBA_bit_is_set (rcvr_w2item_set, item)) {
	  /* candidat */
	  SXBA_1_bit (rcvr_prod_set, prod); /* une seule tentative de recup reduce par prod */

	  if (index_sets_next_mlstn [item] == NULL) {
	    /* item jamais essaye' ds next_mlstn */
	    if (RT [working_rcvr_mlstn+1].index_sets [item] == NULL) {
	      /* item jamais essaye' ds les tables de travail */
	      if (!working_rcvr_mlstn_tables_must_be_cleared) {
		/* On travaille donc ds une table speciale qui est vide initialement,  A razer en fin */
		working_rcvr_mlstn_tables_must_be_cleared = SXTRUE;
		/* Il faut peut-etre faire un set_T2 specifique (on n'a pas besoin de tout!!) */
		set_T2 (working_rcvr_mlstn+1);
		/* petite correction, il faut prendre le look_ahead de next_mlstn */
		T2_look_ahead_t_set = mlstn2look_ahead_t_set [next_mlstn];
	      }

	      is_during_reduce_rcvr_validation = SXTRUE; /* On previent shift () ... */
	    
	      if (earley_glbl_rcvr_reduce_pre (mlstn, item, next_mlstn, rcvr_w2titem_set)) {
		/* DPUSH (rcvr_reduce_item_stack, item); */
	      }

	      is_during_reduce_rcvr_validation = SXFALSE;
	    }
	  }

	  item = PROLON (prod+1)-1; /* inutile de regarder les autres items de cette prod */
	}
      }
    }
  }

  rcvr_kernel_nb = 0;

  if ((!local_ret_val && IS_EMPTY (rcvr_reduce_item_stack) /* echec des tentatives precedentes */) || rcvr_spec.try_kind >= TRY_MAX) {
    /* ... et les items reduce des prods du kernel */

    /* meme si local_ret_val */
    /* On va parcourir les items droits des items non kernel */
    item = 3; /* Pas de recup sur 0: S' -> $ S $ */

    while ((item = sxba_scan (rcvr_kernel_item_set, item)) >= 0) {
      prod = PROLIS (item);

      if (SXBA_bit_is_reset_set (rcvr_prod_set, prod)) {
	top_item = PROLON (prod+1)-1;

	if (index_sets_next_mlstn [top_item]/* index_set */ == NULL) {
	  /* meme si local_ret_val */
	  /* On va parcourir les items reduce du kernel */
	  /* On essaie (apre`s abondon de l'essai precedent) de refaire de la rcvr sur les reduce */
	  /* Si [A -> \alpha . \beta, i] est ds la table mlstn et que aucune recup n'a ete possible sur
	     les items de \beta pour un next_mlstn donne' on avait decide' de ne rien faire sur
	     A -> \alpha \beta . car la recuperation effectuee pourra aussi bien se faire + tard sur les couples
	     d'items [B -> \gamma . A . \delta, j] des tables T[i].  C'est vrai, mais auquel cas, pour trouver
	     une chaine corrigee, on va devoir generer une chaine de \alpha qcq (disons la + courte) alors
	     que celle du source marche.  On va donc etre bien moins precis !! */
	  /* Pour eviter d'encombrer T[next_mlstn] avec des cochonneries venant d'essais avortes, les essais ont lieu
	     ds T[working_rcvr_mlstn+1] et si ca marche, on le refait pour de bon ds T[next_mlstn] */
	  /* On a atteint la position reduce de prod.
	     On regarde si cet item reduce peut etre suivi d'un terminal de source_t_set... */
	  if (SXBA_bit_is_set (rcvr_w2item_set, top_item)) {
	    /* ... candidat, */
	    /* Le 28/09/05 */
	    /* Il se peut que cet item se trouve deja ds les tables (et a donc rien donne'), si c'est le cas
	       ce n'est pas la peine de recommencer */
	    /* C'est l'index_set de next_mlstn qu'il faut prendre !! */
	    /* item jamais essaye' ds next_mlstn */
	    /* Le 04/10/05 Si top_ptr2 == NULL => nelle reduction */
	    /* Il faut simuler la reduction. */
	    /* On commence par chercher les terminaux et leurs mlstn d'origine */
	    /* Le 28/11/05 la recup ne depend pas de t */
	    /* Si cette recup echoue, elle peut laisser des scories indesirables */
	    if (RT [working_rcvr_mlstn+1].index_sets [top_item] == NULL) {
	      /* item jamais essaye' ds les tables de travail */
	      if (!working_rcvr_mlstn_tables_must_be_cleared) {
		/* On travaille donc ds une table speciale qui est vide initialement,  A razer en fin */
		working_rcvr_mlstn_tables_must_be_cleared = SXTRUE;
		/* Il faut peut-etre faire un set_T2 specifique (on n'a pas besoin de tout!!) */
		set_T2 (working_rcvr_mlstn+1);
		/* petite correction, il faut prendre le look_ahead de next_mlstn */
		T2_look_ahead_t_set = mlstn2look_ahead_t_set [next_mlstn];
	      }

	      is_during_reduce_rcvr_validation = SXTRUE; /* On previent shift () ... */
	    
	      if (earley_glbl_rcvr_reduce_pre (mlstn, item, next_mlstn, rcvr_w2titem_set)) {
		rcvr_kernel_nb++;
		/* DPUSH (rcvr_reduce_item_stack, item); */
	      }

	      is_during_reduce_rcvr_validation = SXFALSE;

	      if (mlstn == next_mlstn && rcvr_kernel_nb == 1 )
		/* Une correction unique (voir pave' du debut) */
		/* PB, ds le cas beam, elle sera peut etre invalidee */
		break;
	    }
	  }
	}

	item = top_item;
      }
    }
  }
  
  top = TOP (rcvr_reduce_item_stack);

  if (!local_ret_val && top == 0 && mlstn == 1 && next_mlstn == n) {
    if (SXBA_bit_is_set (rcvr_kernel_item_set, 1)
	&& index_sets_next_mlstn [2]/* index_set */ == NULL) {
      /* Toujours vrai ? */
      for (top_numpg = NPG (1+1), bot_numpg = NPG (1); bot_numpg < top_numpg; bot_numpg++) {
	/* On ne se rattrape que sur une regle definissant l'axiome */
	prod = NUMPG (bot_numpg);

	if (!SXBA_bit_is_set (rcvr_prod_set, prod)) {
	  top_item = PROLON (prod+1)-1;
	  bot_item = PROLON (prod);

	  if (bot_item < top_item) {
	    /* On essaie une non vide en premier */
	    SXBA_1_bit (rcvr_prod_set, prod); /* une seule tentative de recup reduce par prod */

	    if (!working_rcvr_mlstn_tables_must_be_cleared) {
	      /* On travaille donc ds une table speciale qui est vide initialement,  A razer en fin */
	      working_rcvr_mlstn_tables_must_be_cleared = SXTRUE;
	      /* Il faut peut-etre faire un set_T2 specifique (on n'a pas besoin de tout!!) */
	      set_T2 (working_rcvr_mlstn+1);
	      /* petite correction, il faut prendre le look_ahead de next_mlstn */
	      T2_look_ahead_t_set = mlstn2look_ahead_t_set [next_mlstn];
	    }

	    is_during_reduce_rcvr_validation = SXTRUE; /* On previent shift () ... */
	    
	    if (earley_glbl_rcvr_reduce_pre (1, bot_item, n, rcvr_w2titem_set)) {
	      /* DPUSH (rcvr_reduce_item_stack, bot_item); */
	      top = 1;
	    }

	    is_during_reduce_rcvr_validation = SXFALSE;

	    if (top == 1)
	      /* PB, ds le cas beam, elle sera peut etre invalidee */
	      break;
	  }
	}
      }

      if (top == 0) {
	for (top_numpg = NPG (1+1), bot_numpg = NPG (1); bot_numpg < top_numpg; bot_numpg++) {
	  /* On ne se rattrape que sur une regle definissant l'axiome */
	  prod = NUMPG (bot_numpg);

	  if (!SXBA_bit_is_set (rcvr_prod_set, prod)) {
	    /* ... vide */
	    bot_item = PROLON (prod);
	    SXBA_1_bit (rcvr_prod_set, prod); /* une seule tentative de recup reduce par prod */

	    if (!working_rcvr_mlstn_tables_must_be_cleared) {
	      /* On travaille donc ds une table speciale qui est vide initialement,  A razer en fin */
	      working_rcvr_mlstn_tables_must_be_cleared = SXTRUE;
	      /* Il faut peut-etre faire un set_T2 specifique (on n'a pas besoin de tout!!) */
	      set_T2 (working_rcvr_mlstn+1);
	      /* petite correction, il faut prendre le look_ahead de next_mlstn */
	      T2_look_ahead_t_set = mlstn2look_ahead_t_set [next_mlstn];
	    }

	    is_during_reduce_rcvr_validation = SXTRUE; /* On previent shift () ... */
	    
	    if (earley_glbl_rcvr_reduce_pre (1, bot_item, n, rcvr_w2titem_set)) {
	      /* DPUSH (rcvr_reduce_item_stack, bot_item); */
	      top = 1;
	    }

	    is_during_reduce_rcvr_validation = SXFALSE;

	    if (top == 1)
	      /* PB, ds le cas beam, elle sera peut etre invalidee */
	      break;
	  }
	}
      }

      /* ça n'a pas marché, si on est sur [1..n] on va tenter un rattrapage global dans la super-règle */
      if (top == 0) {
	local_ret_val = earley_glbl_rcvr_set (mlstn, 1, next_mlstn, 2);
	/* Normalement marche tout le temps, PB retourne une foret vide : axiome instancie' sans branches !! */
      }
    }
  }

  if (top > 0) {
    /* Ca a marche' on le met donc aussi ds les tables de next_mlstn */
    local_ret_val = SXTRUE;      
    earley_glbl_rcvr_reduce_post (mlstn, next_mlstn);
  }

  if (working_rcvr_mlstn_tables_must_be_cleared) {
    working_rcvr_mlstn_tables_must_be_cleared = SXFALSE;
    clear_table (working_rcvr_mlstn+1);
  }

  if (call_process_non_kernel_mlstn) {
    /* Le corps de ce if n'est fait qu'une fois */
    /* item, l'origine de la rcvr ne fait pas partie de la table T[mlstn], il faut donc l'y mettre
       (avec tous les items non kernel qui l'ont produits)
       on appelle donc process_non_kernel (mlstn) */
    no_la_check = SXTRUE;
    set_T2 (mlstn);
    no_la_check = SXFALSE;

    /* le process_non_kernel sur non_kernel_item_sets [next_mlstn] sera fait a l'exterieur */
    /* Il faut enlever les items terminaux, ils ne sont pas bons et ne doivent pas etre stockes */
    sxba_minus (rcvr_non_kernel_item_set, TITEM_SET);
    T2_non_kernel_item_set = rcvr_non_kernel_item_set;
    process_non_kernel_filtering (mlstn);

#if HAS_BEAM == 1
    /* i.e., #if RECOGNIZER_KIND */
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
      set_non_kernel_item_logprobs_recognizer (mlstn);
    else
      {
#if REDUCER_KIND
	if (reducer_beam_value_is_set)
	  set_non_kernel_item_logprobs_reducer (mlstn);
#else /* REDUCER_KIND */
	;
#endif /* REDUCER_KIND */
      }
#endif  /* HAS_BEAM == 1 */

    process_non_kernel (mlstn);
  }

  if (local_ret_val)
    ret_val = next_mlstn;

  if (mlstn == next_mlstn) {
    rcvr_spec.try_kind = save_try_kind;

    if (ret_val == 0) {
      /* Echec, On a pu mettre des cochonneries ds T [mlstn], on remet ds l'etat initial */
      copy_table (mlstn, working_rcvr_mlstn);
    }
  }

  return ret_val;
}




/* detection d'erreur ds la table T[p] (pas de transition terminale sur ap) */
/* Nouvelle version qui utilise range_walk (), la fonction qui permet des parcours
 differents de l'espace des ranges */
static SXINT
earley_no_dag_rcvr (p)
     SXINT p;
{
  SXINT     bot, rcvr_q, q, r, m, n;

#if LOG
  fputs ("\t*** Entering Error Recovery ***\n", stdout);
#endif /* LOG */

  if (rcvr_reduce_item_stack == NULL) {
    /* Duree de vie : reconnaisseur */
    sxprepare_for_possible_reset (rcvr_reduce_item_stack);
    DALLOC_STACK (rcvr_reduce_item_stack, 100);
    rcvr_non_kernel_item_set = sxba_calloc (MAXITEM+1);
    rcvr_w2item_set = sxba_calloc (MAXITEM+1);
    rcvr_prod_set = sxba_calloc (MAXPROD+1); /* Duree de vie : reconnaisseur */
    rcvr_w2titem_set = sxba_calloc (MAXITEM+1); /* Duree de vie : reconnaisseur */
    rcvr_reduce_T2_non_kernel_item_set = sxba_calloc (MAXITEM+1); /* Duree de vie : reconnaisseur */
  }

#ifdef MAKE_INSIDEG
  if (spf.insideG.t2item_set == NULL)
    fill_t2item_set (); /* Ds lexicalizer_mngr */

  if (spf.insideG.nt2min_gen_lgth == NULL)
    fill_nt2min_gen_lgth (); /* Ds lexicalizer_mngr */
#endif /* MAKE_INSIDEG */

#if HAS_BEAM == 1
  if ((recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) && nt2prod_that_generates_the_best_logprob_t_string == NULL) {
    /* pas calcule', on le fait */
    sxprepare_for_possible_reset (nt2prod_that_generates_the_best_logprob_t_string);
    nt2prod_that_generates_the_best_logprob_t_string = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT))
      , nt2prod_that_generates_the_best_logprob_t_string [0] = 0;
    fill_tnt_max_inside_logprobs (SXFALSE /* On ne remplit pas item2max_right_inside_logprob */);
  }
#endif /* HAS_BEAM == 1 */

#if is_1la
  if (!is_error_detected) {
    /* 1ere detection d'erreur */
    ilex_compatible_item_set = NULL;
    /* Recalcul du look-ahead au cas ou ... */
    fill_mlstn2look_ahead_item_set (SXFALSE);
  }
#endif /* is_1la

  /* On previent... */
  is_during_error_rcvr_processing = is_error_detected = SXTRUE;
  /* ... et on passe immediatement les ensembles de look-ahead au cas non lexicalise' */
  mlstn2look_ahead_t_set = mlstn2non_lexicalized_look_ahead_t_set;

  /* sauvegarde de p car T [p] peut etre raze' par une tentative [r]..[p] puis etre reutilisee par une
     autre tentative [p]..[s] */
  copy_table (working_rcvr_mlstn, p);
  rcvr_detection_table = p;
  rcvr_detection_table_cleared = SXFALSE;

  rcvr_q = q = r = 0;

  while (range_walk (p, q, r, &m, &n)) {
    /* Le test 
       if (m < n ||  rcvr_spec.insertion != -1)
       de l'insertion permise/deconseillee est faite ds earley_glbl_rcvr */
    if (!SXBA_bit_is_set (rcvr_xcld_min_mlstn_set, m)) {
      /* Borne inf de la signature non deja utilisee par une recup precedente */
      if (sxba_scan (mlstn2non_lexicalized_look_ahead_t_set [n], 0) > 0) {
	/* Test ajoute' le 15/11/06 car ds certain cas (source non passe' ds sxpipe et non traitement des mots inconnus)
	   il se peut que le look_ahead de n soit un mot inconnu, auquel cas il est inutile de tenter de se rattraper dessus !! */
	rcvr_q = earley_glbl_rcvr (m, n);

	if (rcvr_q) {
	  /* La recuperation sur la signature (m, n) a marche' ... */
	  /* On interdit d'autres recup sur [m+1..n] */
	  if (n > m)
	    sxba_range (rcvr_xcld_min_mlstn_set, m+1, n-m);

#if 0
	  if (m == n)
	    /* Pourquoi ?? */
	    SXBA_1_bit (rcvr_xcld_min_mlstn_set, n);
	  else
	    sxba_range (rcvr_xcld_min_mlstn_set, m+1, n-m);
#endif /* 0 */

	  break;
	}
      }
    }

    q = m;
    r = n;
  }

#if EBUG
  /* Normalement on doit tj se recuperer sur init -> <AXIOM> final !! */
  if (rcvr_q == 0)
    sxtrap (ME, "earley_dag_rcvr");
#endif
  
  /* On previent... */
  is_during_error_rcvr_processing = SXFALSE;

#if LOG
  fputs ("\t*** Leaving Error Recovery ***\n", stdout);
#endif /* LOG */

  return rcvr_q;
}
#endif /* is_rcvr */


#if is_rcvr || is_beamrcvr
static void
clear_table (SXINT p)
{
  if (p == rcvr_detection_table)
    /* On note si la table sur laquelle l'erreur courante a ete detectee a ete razee */
    /* car il faudra peut etre la restaurer a partir de la valeur sauvegardee ds T[working_rcvr_mlstn]
       par copy_table () */
    rcvr_detection_table_cleared = SXTRUE;

#ifdef MAKE_INSIDEG
 {
   SXINT                   i;
   struct recognize_item *RT_ptr;

   RT_ptr = RT+p;

   for (i = MAXNT; i >= 0; i--) {
     RT_ptr->shift_NT_hd [i] = NULL;
   }

   for (i = MAXITEM; i >= 0; i--) {
     RT_ptr->items_stack [i] = 0;
     RT_ptr->index_sets [i] = NULL; /* C'est un bag */
   }
 }
#else /* !MAKE_INSIDEG */
  RT [p] = empty_RT;
#endif /* !MAKE_INSIDEG */

#if is_rcvr
  sxba_empty (rcvr_reduce_item_sets [p]);
#endif /* is_rcvr */

  sxba_empty (RT_left_corner [p]);

#if is_rcvr
  sxba_empty (store_non_kernel_item_sets [p]);
#endif /* is_rcvr */

#if is_rcvr
  sxba_empty (rcvr_kernel_item_sets [p]);
#endif /* is_rcvr */

  sxba_empty (non_kernel_item_sets [p]);

#if HAS_BEAM == 1
  if (kernel_nt_sets) sxba_empty (kernel_nt_sets [p]);
#endif /* HAS_BEAM == 1 */

#if is_parser
#ifdef MAKE_INSIDEG
 {
   SXINT i;
   struct parse_item *PT_ptr;

   PT_ptr = PT+p;

   for (i = MAXITEM; i >= 0; i--) {
     PT_ptr->backward_index_sets [i] = NULL; /* C'est un bag */
   }

   for (i = MAXNT; i >= 0; i--) {
     PT_ptr->reduce_NT_hd [i] = NULL;
   }
 }
#else /* !MAKE_INSIDEG */
  PT [p] = empty_PT;
#endif /* !MAKE_INSIDEG */
#endif /* is_parser */
}
#endif /* is_rcvr || is_beamrcvr */

#if is_rcvr

/* T [p] = T [q] */
static void
copy_table (SXINT p, SXINT q)
{
  SXINT                 nt, item;
  SXINT                 *int_ptr;
  SXBA                  set, set2;
  struct recognize_item *RTp, *RTq;
  struct parse_item     *PTp, *PTq;

  RTp = RT+p;
  RTq = RT+q;

#ifdef MAKE_INSIDEG
  for (item = MAXITEM; item >= 0; item--) {
    RTp->items_stack [item] = RTq->items_stack [item];

    if (set = RTq->index_sets [item]) {
      set2 = RTp->index_sets [item] = sxba_bag_get (&shift_bag, SXBASIZE (set));
      sxba_copy (set2, set);
    }
    else {
      RTp->index_sets [item] = NULL;
    }
  }

  for (nt = MAXNT; nt >= 0; nt--) {
    if (int_ptr = RTq->shift_NT_hd [nt]) {
      RTp->shift_NT_hd [nt] = RTp->items_stack + (int_ptr - RTq->items_stack);
    }
    else
      RTp->shift_NT_hd [nt] = NULL;
  }
#else /* !MAKE_INSIDEG */
  RT [p] = RT [q];	

  /* Les RTp->index_sets [item] contiennent des SXBA alloues par des sxba_bag qui doivent etre recopies car les originaux
     ds q peuvent etre modifies */
  for (item = MAXITEM; item >= 0; item--) {
    if (set = RTp->index_sets [item]) {
      set2 = RTp->index_sets [item] = sxba_bag_get (&shift_bag, SXBASIZE (set));
      sxba_copy (set2, set);
    }
  }
#endif /* !MAKE_INSIDEG */

#if is_rcvr
  sxba_copy (rcvr_reduce_item_sets [p], rcvr_reduce_item_sets [q]);
#endif /* is_rcvr */

  sxba_copy (RT_left_corner [p], RT_left_corner [q]);

#if is_rcvr
  sxba_copy (store_non_kernel_item_sets [p], store_non_kernel_item_sets [q]);
#endif /* is_rcvr */

#if is_rcvr
  sxba_copy (rcvr_kernel_item_sets [p], rcvr_kernel_item_sets [q]);
#endif /* is_rcvr */

  sxba_copy (non_kernel_item_sets [p], non_kernel_item_sets [q]);

#if HAS_BEAM == 1
  if (kernel_nt_sets) sxba_copy (kernel_nt_sets [p], kernel_nt_sets [q]);
#endif /* HAS_BEAM == 1 */

#if is_parser
  PTp = PT+p;
  PTq = PT+q;

#ifdef MAKE_INSIDEG
  for (item = MAXITEM; item >= 0; item--) {
    if (set = PTq->backward_index_sets [item]) {
      set2 = PTp->backward_index_sets [item] = sxba_bag_get (&shift_bag, SXBASIZE (set));
      sxba_copy (set2, set);
    }
    else {
      PTp->backward_index_sets [item] = NULL;
    }
  }

  for (nt = MAXNT; nt >= 0; nt--) {
    if (int_ptr = PTq->reduce_NT_hd [nt]) {
      PTp->reduce_NT_hd [nt] = RTp->items_stack + (int_ptr - RTq->items_stack);
    }
    else
      PTp->reduce_NT_hd [nt] = NULL;
  }
#else /* !MAKE_INSIDEG */
  PT [p] = PT [q];	

  /* Les PTp->index_sets [item] contiennent des SXBA alloues par des sxba_bag qui doivent etre recopies car les originaux
     ds q peuvent etre modifies */
  for (item = MAXITEM; item >= 0; item--) {
    if (set = PTp->backward_index_sets [item]) {
      set2 = PTp->backward_index_sets [item] = sxba_bag_get (&shift_bag, SXBASIZE (set));
      sxba_copy (set2, set);
    }
  }
#endif /* !MAKE_INSIDEG */

#endif /* is_parser */
}
#endif /* is_rcvr */



#if HAS_BEAM == 1
static void
set_nt2max_prefix_suffix_logprob (SXINT Y, SXLOGPROB max_outside_logprob_of_Y)
{
  if (SXBA_bit_is_reset_set (T2_kernel_nt_set, Y)
      || cmp_SXLOGPROB ((nt2max_outside_logprob [Y]), max_outside_logprob_of_Y) < 0 /* < */) {
    nt2max_outside_logprob [Y] = max_outside_logprob_of_Y;

#if LLOG
    printf ("nt2max_outside_logprob [%s = " SXILD "] = " SXLPD "\n", NTSTRING (Y), Y, SXLOGPROB_display (nt2max_outside_logprob [Y]));
#endif /* LLOG */
  }
}
#endif /* HAS_BEAM == 1 */

#if LOG || LLOG
static void output_item (SXINT item_j, SXBA index_set_j, SXINT i, SXINT j);
#endif /* LOG || LLOG */

#if HAS_BEAM == 1
/*
  les logprobs associees a l'item_p [A -> \alpha . \beta X Y \gamma, i, p] sont connues par p_logprob_ptr
  On calcule celles de item_q [A -> \alpha \beta X . Y \gamma, i, q] si elles sont valides
  on a item_q > item_p et item_q - item_p = |\beta X |.  Si \beta est vide X est qcq sinon X > 0 et derive ds le vide.
  On calcule egalement les proba prefixe et suffixe de Y si Y>0 et si on est ds le cas recognizer_beam
 */
/* Le 21/09/10 Nouvelle version dans laquelle on ne propage plus individuellement les logprobs
   prefixe et suffixe mais uniquement leur somme rassemblee dans une proba outside */
static SXBOOLEAN
set_forward_logprob (SXINT i, SXINT p, SXINT q, SXINT item_p, SXINT item_q, SXINT X, SXINT *pq_ptr, SXLOGPROB *X_inside_logprob_ptr)
{
  SXINT                 pq, pqt, Y, ip, iq, ei_item_p;
  SXLOGPROB                old_logprob, new_logprob, X_inside_logprob, new_left_inside_logprob, right_inside_logprob, min_admissible_logprob, p_logprob_ptr_outside, item_q_prev_total_logprob, item_q_total_logprob;
  struct logprob_struct   *p_logprob_ptr, *q_logprob_ptr;
  SXBOOLEAN             bool;
  SXBOOLEAN             is_beam_proof = SXTRUE;

#if LLLOG
  printf ("*** Entering set_forward_logprob (i=" SXILD ", p=" SXILD ", q=" SXILD ", item_p=" SXILD ", item_q=" SXILD ", X=" SXILD ", X_inside_logprob=" SXLPD ") ***\n", i, p, q, item_p, item_q, X, SXLOGPROB_display (*X_inside_logprob_ptr));
#endif /* LLLOG */

#if EBUG
#if is_rcvr
    if (is_during_reduce_rcvr_validation)
      sxtrap (ME, "set_forward_logprob");
#endif /* is_rcvr */
#endif /* EBUG */

  if (X != 0 && (pq = *pq_ptr) == 0)
    *pq_ptr = pq = p_q2pq (p, q);

  if ((X_inside_logprob = *X_inside_logprob_ptr) == 0) {
    /* Attention, il est possible que X_inside_logprob == 0 et soit initialise' !! */
    if (X > 0) {
      if (item_q == item_p+1) {
	Axpq2get_max_logprob_ptr (X, pq);
	X_inside_logprob = max_logprob_ptr->inside;
      }
    }
    else {
      if (X < 0) {
	if (-X == SXEOF)
	  X_inside_logprob = 0;
	else {
	  X = -X;

#ifdef MAKE_INSIDEG
	  X = spf.insideG.t2init_t [X];
#endif /* MAKE_INSIDEG */

	  pqt = pq_t2pqt (pq, X);

#if EBUG
      if (pqt == 0)
	sxtrap (ME, "set_forward_logprob");
#endif /* EBUG */

	  X_inside_logprob = pqt2logprob (pqt);
	}
      }
# if is_rcvr
      else
	X_inside_logprob = get_max_inside_rcvr_logprob (item_p, item_q);
# endif /* is_rcvr */
    }
  }

  ip = p_q2pq (i, p);
  iq = p_q2pq (i, q);

  ei2get_logprob_ptr (item_p, ip, p_logprob_ptr);
  p_logprob_ptr_outside = p_logprob_ptr->outside; /* on en aura besoin plus tard, et une fois le ei2set (item_q,...) fait, on n'a aucune garantie qu'une réallocation n'ait pas eu lieu et que p_logprob_ptr soit encore utilisable */
  new_left_inside_logprob = p_logprob_ptr->inside.left + X_inside_logprob;
  item_q_total_logprob = new_left_inside_logprob;

# if RECOGNIZER_KIND == 2
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
    item_q_total_logprob += p_logprob_ptr_outside + item2max_right_inside_logprob [item_q];
    
    if (recognizer_shift_beam_value_is_set) {
      /* si item_q_total_logprob est trop pourri, on beame l'item q en positionnant is_beam_proof à SXFALSE */
      struct earley_table_companion* etc_ptr = earley_table_companion + q;
      min_admissible_logprob = etc_ptr->max_global_expected_logprob + etc_ptr->dynamic_shift_beam_value;
      
      if (!((is_beam_proof = (cmp_SXLOGPROB (item_q_total_logprob, min_admissible_logprob) >= 0 /* >= */)))) {
	struct earley_table_companion *earley_table_companion_q = earley_table_companion + q;
	if (!earley_table_companion_q->did_shift_beam) {
	  earley_table_companion_q->did_shift_beam = SXTRUE;
	  earley_table_companion_q->dynamic_shift_beam_first_beamed_logprob = item_q_total_logprob;
	}
	else
	  if (cmp_SXLOGPROB (earley_table_companion_q->dynamic_shift_beam_first_beamed_logprob, item_q_total_logprob) < 0)
	    earley_table_companion_q->dynamic_shift_beam_first_beamed_logprob = item_q_total_logprob;
      }
    }
  }
# endif /* RECOGNIZER_KIND == 2 */
  
#ifdef CONTROLLED_BEAM_H
#if is_rcvr
  if (!is_during_error_rcvr_processing) {
#endif /* is_rcvr */
    /* On regarde si l'inside.left proba associee a item_q, iq est compatible avec la proba predite */
    /* benefice du doute pendant une rcvr */
    if (is_beam_proof)
      is_beam_proof = check_controlled_beam_proof (item_q, iq, new_left_inside_logprob);
  }
#endif /* CONTROLLED_BEAM_H */

  if (is_beam_proof) {
    /* ici on est sûrs de vouloir rajouter item_q */
    bool = ei2set_logprob_ptr (item_q, iq, q_logprob_ptr);

    if (bool) {
      /* l'item [item_q, i, q] a deja ete vu */
      /* On ne tient pas compte de item2max_right_inside_logprob [item_q] qui est independant des i */
    
# if RECOGNIZER_KIND == 2
      if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
	item_q_prev_total_logprob = q_logprob_ptr->expected_total;
      else
# endif /* RECOGNIZER_KIND == 2 */
	item_q_prev_total_logprob = q_logprob_ptr->inside.left;
    }
  
    if (!bool || cmp_SXLOGPROB (item_q_prev_total_logprob, item_q_total_logprob) < 0 /* < */) {
      /* On ne garde que la meilleure proba totale */
      q_logprob_ptr->inside.left = new_left_inside_logprob;
    
# if RECOGNIZER_KIND == 2
      if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
	q_logprob_ptr->outside = p_logprob_ptr_outside;
	q_logprob_ptr->expected_total = item_q_total_logprob;
      
	if ((Y = LISPRO (item_q)) > 0)
	  set_nt2max_prefix_suffix_logprob (Y
					  , q_logprob_ptr->outside
					  + q_logprob_ptr->inside.left + item2max_right_inside_logprob [item_q+1]
					  );
      }
# endif /* RECOGNIZER_KIND == 2 */
    }
  }

#if LLLOG
  printf ("Forward transition from item_p=" SXILD " : ", item_p);
  output_item (item_p, NULL, i, p);
  printf (" to item_q=" SXILD " : ", item_q);
  output_item (item_q, NULL, i, q);
  printf (" is %s\n", is_beam_proof ? "kept" : "erased");
#endif /* LLLOG */

#if LLLOG
  fputs ("*** Leaving set_forward_logprob () ***\n", stdout);
#endif /* LLLOG */

  return is_beam_proof;
}
#endif /* HAS_BEAM == 1 */




#if RECOGNIZER_KIND || (HAS_BEAM && is_rcvr)
static void
fill_tnt_max_inside_logprobs (SXBOOLEAN fill_item2max_right_inside_logprob)
{
  /* pour chaque item A -> \alpha . \beta on calcule item2max_right_inside_logprob la proba max du suffixe \beta
     ATTENTION: item2max_right_inside_logprob ne contient pas la proba de la prod associee a l'item */
  /* Ca va permettre pour un item earley [A -> \alpha . \beta, i] de calculer une proba totale qui tient compte de \beta
     en en estimant la proba (optimiste) max */
  SXINT  pqt, t, prod, item, A, B, X, rhs_nt_nb, top, bot;
  SXINT  *nt_stack, *prod2rhs_nt_nb;
  SXBA   nt_set, t_set;
  SXLOGPROB *nt2prev_max_inside_logprob, *prod2max_inside_logprob, logprob, prev_logprob, new_logprob, delta_of_B;

	  
  sxprepare_for_possible_reset (nt2max_inside_logprob);
  nt2max_inside_logprob = (SXLOGPROB *) sxcalloc (MAXNT+1, sizeof (SXLOGPROB));
  t2max_inside_logprob = (SXLOGPROB *) sxcalloc (-MAXT+1, sizeof (SXLOGPROB));

  t_set = sxba_calloc (-MAXT+1);
  prod2max_inside_logprob = (SXLOGPROB *) sxcalloc (MAXPROD+1, sizeof (SXLOGPROB));
  nt_set = sxba_calloc (MAXNT+1);
  nt_stack = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), nt_stack [0] = 0;
  prod2rhs_nt_nb = (SXINT*) sxcalloc (MAXPROD+1, sizeof (SXINT));
  nt2prev_max_inside_logprob = (SXLOGPROB *) sxcalloc (MAXNT+1, sizeof (SXLOGPROB));

  /* On calcule t2max_inside_logprob */
  fill_idag_pqt2pq ();
	  
  for (pqt = MAX_pqt (); pqt >= 1; pqt--) {
    if (/* pq = */idag.pqt2pq [pqt]) {
      /* C'est un bon pqt (y'a des trous !!) */
      t = idag.t_stack [pqt];
      logprob = idag.pqt2logprob [pqt];

      /* attention, il se peut qu'on aie une proba de 1 associee a un terminal (log(t)==0), il faut distinguer ce cas d'une init */
      if (SXBA_bit_is_reset_set (t_set, t) || cmp_SXLOGPROB (t2max_inside_logprob [t], logprob) < 0 /* < */) {
	t2max_inside_logprob [t] = logprob;
#if LLOG
	printf ("t2max_inside_logprob [%s] = " SXLPD "\n",TSTRING (t), SXLOGPROB_display (logprob));
#endif /* LLOG */
      }
    }
  }

  sxfree (t_set);

  /* On calcule les logprobs des prod de la forme A -> T* */
  for (prod = 0; prod <= MAXPROD; prod++) {
    item = PROLON (prod);
    if (ilex_compatible_item_set == NULL || SXBA_bit_is_set (ilex_compatible_item_set, item)) {
      logprob = 0;
      rhs_nt_nb = 0;

      while ((X = LISPRO (item++)) != 0) {
	if (X > 0)
	  rhs_nt_nb++;
	else
	  logprob += t2max_inside_logprob [-X];
      }
	      
      /* On ne met pas la proba de prod ds prod2max_inside_logprob [prod] ... */
      prod2max_inside_logprob [prod] = logprob;
      /* ... mais on en tient compte pour nt2max_inside_logprob [LHS (prod)] */
      logprob += prod2logprob (prod);

      if (rhs_nt_nb == 0) {
	A = LHS (prod);

	if (nt2max_inside_logprob [A] == 0 || nt2max_inside_logprob [A] < logprob) {
	  nt2max_inside_logprob [A] = logprob;
#if is_rcvr
	  nt2prod_that_generates_the_best_logprob_t_string [A] = prod;
#endif /* is_rcvr */

	  if (SXBA_bit_is_reset_set (nt_set, A))
	    PUSH (nt_stack, A);
	}
      }
      else {
	prod2rhs_nt_nb [prod] = rhs_nt_nb;
      }
    }
  }

  while (!IS_EMPTY (nt_stack)) {
    B = POP (nt_stack);
    SXBA_0_bit (nt_set, B);
    prev_logprob = nt2prev_max_inside_logprob [B]; /* proba == 0 => 1ere utilisation de B en rhs */
    new_logprob = nt2max_inside_logprob [B];
    delta_of_B = new_logprob - prev_logprob;

    for (top = NPD (B+1)-1, bot = NPD (B); bot < top; bot++) {
      item = NUMPD (bot);

      if (ilex_compatible_item_set == NULL || SXBA_bit_is_set (ilex_compatible_item_set, item)) {
	prod = PROLIS (item);
	
	if (prod2rhs_nt_nb [prod] == 0) {
	  /* prod2max_inside_logprob [prod] a deja ete calcule' */
#if EBUG
	  if (prev_logprob == 0)
	    sxtrap (ME, "fill_tnt_max_inside_logprobs");
#endif /* EBUG */
	  
	  logprob = prod2max_inside_logprob [prod] += delta_of_B;
	  A = LHS (prod);
	  
	  if (nt2max_inside_logprob [A] == 0 || cmp_SXLOGPROB (nt2max_inside_logprob [A], logprob) < 0) {
	    if (SXBA_bit_is_reset_set (nt_set, A)) {
	      PUSH (nt_stack, A);
	      nt2prev_max_inside_logprob [A] = nt2max_inside_logprob [A];
	    }
	    
	    nt2max_inside_logprob [A] = logprob;
#if is_rcvr
	    nt2prod_that_generates_the_best_logprob_t_string [A] = prod;
#endif /* is_rcvr */
	  }
	}
	else {
	  /* Attention, la proba de B a pu s'ameliorer plusieurs fois avant que prod soit entierement calculee ... */
	  logprob = prod2max_inside_logprob [prod] += delta_of_B; /* ... d'ou le prev_logprob */
	  
	  if (prev_logprob == 0 && --prod2rhs_nt_nb [prod] == 0) {
	    /* On ne decompte que la ere fois qu'on trouve le/les B d'une prod */
	    /* 1ere eval de la rhs de prod */
	      
	    /* On ne met pas la proba de prod ds prod2max_inside_logprob [prod] ... */
	    /* ... mais on en tient compte pour nt2max_inside_logprob [LHS (prod)] */
	    logprob += prod2logprob (prod);

	    A = LHS (prod);

	    if (nt2max_inside_logprob [A] == 0 || cmp_SXLOGPROB (nt2max_inside_logprob [A], logprob) < 0) {
	      if (SXBA_bit_is_reset_set (nt_set, A)) {
		PUSH (nt_stack, A);
		nt2prev_max_inside_logprob [A] = nt2max_inside_logprob [A];
	      }

	      nt2max_inside_logprob [A] = logprob;
#if is_rcvr
	      nt2prod_that_generates_the_best_logprob_t_string [A] = prod;
#endif /* is_rcvr */
	    }
	  }
	}
      }
    }
  }

  sxfree (nt_stack);
  sxfree (nt_set);
  sxfree (prod2rhs_nt_nb);
  sxfree (nt2prev_max_inside_logprob);

#if RECOGNIZER_KIND == 2
  if ((recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) && fill_item2max_right_inside_logprob) {
    item2max_right_inside_logprob = (SXLOGPROB *) sxalloc (MAXITEM+1, sizeof (SXLOGPROB));

    item = MAXITEM + 1;
    logprob = 0;
    
    while (--item >= 0) {
      if (ilex_compatible_item_set == NULL || SXBA_bit_is_set (ilex_compatible_item_set, item)) {
	if ((X = LISPRO (item)) == 0)
	  logprob = 0;
	else {
	  if (X > 0)
	    logprob += nt2max_inside_logprob [X];
	  else
	    logprob += t2max_inside_logprob [-X];
	}
	
	item2max_right_inside_logprob [item] = logprob;
      }
    }
  
#if !is_rcvr
    /* On n'en aura pas besoin */
    sxfree (t2max_inside_logprob), t2max_inside_logprob = NULL;
    sxfree (nt2max_inside_logprob), nt2max_inside_logprob = NULL;
#endif /* !is_rcvr */
  }
#endif /* RECOGNIZER_KIND == 2 */

  sxfree (prod2max_inside_logprob);
}
#endif /* RECOGNIZER_KIND || (HAS_BEAM && is_rcvr) */


/* On ne peut pas utiliser
   sxba_2op (lhs_bits_array, SXBA_OP_COPY, op1_bits_array, SXBA_OP_AND, op2_bits_array)
   car les 3 operandes peuvent avoir des longueurs differentes !!
*/
static SXBOOLEAN
AND3 (SXBA lhs_bits_array, SXBA op1_bits_array, SXBA op2_bits_array)
{
  /* On suppose que lhs_bits_array est vide au-dela de op2_bits_array */
  SXBA	lhs_bits_ptr, op1_bits_ptr, op2_bits_ptr;
  SXINT	        slices_number = SXNBLONGS (SXBASIZE (op2_bits_array)), x;
  SXBOOLEAN	ret_val = SXFALSE;

#if EBUG
  sxbassert (*lhs_bits_array >= *op1_bits_array, "AND3 (|X|<|Y|)");
  sxbassert (*op1_bits_array >= *op2_bits_array, "AND3 (|Y|<|Z|)");
  sxbassert ((*(lhs_bits_array+SXNBLONGS (SXBASIZE (lhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (lhs_bits_array) - 1))) == 0UL, "AND3");
  sxbassert ((*(op1_bits_array+SXNBLONGS (SXBASIZE (op1_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (op1_bits_array) - 1))) == 0UL, "AND3");
  sxbassert ((*(op2_bits_array+SXNBLONGS (SXBASIZE (op2_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (op2_bits_array) - 1))) == 0UL, "AND3");
  for (x = SXNBLONGS (SXBASIZE (lhs_bits_array)); x > slices_number; x--)
    sxbassert (lhs_bits_array [x] == 0UL, "AND3 (X has a non empty suffix)");
#endif
    
  lhs_bits_ptr = lhs_bits_array + slices_number,
    op1_bits_ptr = op1_bits_array + slices_number,
    op2_bits_ptr = op2_bits_array + slices_number;

  while (slices_number-- > 0) {
    if ((*lhs_bits_ptr-- = (*op1_bits_ptr-- & *op2_bits_ptr--)))
      ret_val = SXTRUE;
  }

  return ret_val;
}



#if is_parser || is_rcvr
/* On ne peut utiliser
   sxba_2op (NULL, SXBA_OP_IS, lhs_bits_array, SXBA_OP_AND, rhs_bits_array)
   car les 2 operandes peuvent avoir des tailles differentes et l'intersection n'est
   testee que sur leurs prefixes communs
*/
static SXBOOLEAN
IS_AND (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
  SXBA	        lhs_bits_ptr, rhs_bits_ptr;
  SXINT	        slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
  SXINT	        lhs_slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));

#if EBUG
  sxbassert ((*(lhs_bits_array+lhs_slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (lhs_bits_array) - 1))) == 0, "IS_AND");
  sxbassert ((*(rhs_bits_array+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (rhs_bits_array) - 1))) == 0, "IS_AND");
#endif

  if (lhs_slices_number < slices_number)
    slices_number = lhs_slices_number;

  lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

  while (slices_number-- > 0) {
    if (*lhs_bits_ptr-- & *rhs_bits_ptr--)
      return SXTRUE;
  }

  return SXFALSE;
}
#endif /* is_parser || is_rcvr */


#if is_parser
/* bits_array1 or (bits_array2 & bits_array3) */
/* Retourne SXTRUE ssi bits_array1 a change' */
static SXBOOLEAN
OR_AND (SXBA bits_array1, SXBA bits_array2, SXBA bits_array3)
{
  SXBA	        bits_ptr1, bits_ptr2, bits_ptr3;
  SXINT	        slices_number = SXNBLONGS (SXBASIZE (bits_array2));
  SXBA_ELT	val1, val2;
  SXBOOLEAN	has_changed = SXFALSE;

#if EBUG
  sxbassert (*bits_array1 >= *bits_array2, "OR_AND (|X|<|Y|)");
  sxbassert (*bits_array2 <= *bits_array3, "OR_AND (|Y|>|Z|)");
  sxbassert ((*(bits_array1+SXNBLONGS (SXBASIZE (bits_array1))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array1) - 1))) == 0, "OR_AND");
  sxbassert ((*(bits_array2+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array2) - 1))) == 0, "OR_AND");
  sxbassert ((*(bits_array3+SXNBLONGS (SXBASIZE (bits_array3))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array3) - 1))) == 0, "OR_AND");
#endif

  bits_ptr1 = bits_array1 + slices_number, bits_ptr2 = bits_array2 + slices_number, bits_ptr3 = bits_array3 + slices_number;

  while (slices_number-- > 0) {
    if ((val1 = *bits_ptr1) != (val2 = ((*bits_ptr2--) & (*bits_ptr3--)))) {
      val2 |= val1;

      if (val1 != val2) {
	*bits_ptr1 = val2;
	has_changed = SXTRUE;
      }
    }

    bits_ptr1--;
  }

  return has_changed;
}


static SXBOOLEAN
OR_AND_MINUS (SXBA bits_array1, SXBA bits_array2, SXBA bits_array3, SXBA bits_array4)
{
  /* bits_array4 =  bits_array2 & bits_array3 - bits_array1
     bits_array1 |= bits_array4
     retourne vrai ssi bits_array4 est non vide */
  SXBA	        bits_ptr1, bits_ptr2, bits_ptr3, bits_ptr4;
  /* On change la taille! |bits_array4| == |bits_array2| */
  SXINT	        slices_number = SXNBLONGS (SXBASIZE (bits_array4) = SXBASIZE (bits_array2));
  SXBA_ELT	val;
  SXBOOLEAN	is_set = SXFALSE;

#if EBUG
  sxbassert (*bits_array1 >= *bits_array2, "OR_AND_MINUS (|X|<|Y|)");
  sxbassert (*bits_array3 >= *bits_array2, "OR_AND_MINUS (|Z|<|Y|)");
  sxbassert ((*(bits_array1+SXNBLONGS (SXBASIZE (bits_array1))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array1) - 1))) == 0, "OR_AND_MINUS");
  sxbassert ((*(bits_array2+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array2) - 1))) == 0, "OR_AND_MINUS");
  sxbassert ((*(bits_array3+SXNBLONGS (SXBASIZE (bits_array3))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array3) - 1))) == 0, "OR_AND_MINUS");
  /* Pas de verif sur bits_array4 qui sert d'ensemble de travail */
#endif

  bits_ptr1 = bits_array1 + slices_number,
    bits_ptr2 = bits_array2 + slices_number,
    bits_ptr3 = bits_array3 + slices_number,
    bits_ptr4 = bits_array4 + slices_number;

  while (slices_number-- > 0) {
    if ((val = *bits_ptr4-- = (*bits_ptr2--) & (*bits_ptr3--) & (~(*bits_ptr1)))) {
      is_set = SXTRUE;
      *bits_ptr1-- |= val;
    }
    else
      bits_ptr1--;
  }

  return is_set;
}

#endif	/* is_parser */

/* tailles differentes */
static void
COPY (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
  SXBA	        lhs_bits_ptr, rhs_bits_ptr;
  SXINT	        rhs_slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
  SXINT	        lhs_slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));
  SXBOOLEAN	ret_val = SXFALSE;
    
  lhs_bits_ptr = lhs_bits_array + lhs_slices_number, rhs_bits_ptr = rhs_bits_array + rhs_slices_number;

#if EBUG
  sxbassert (*lhs_bits_array >= *rhs_bits_array, "COPY (|X|<|Y|)");
  sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (lhs_bits_array) - 1))) == 0, "COPY");
  sxbassert ((*rhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (rhs_bits_array) - 1))) == 0, "COPY");
#endif

  lhs_slices_number -= rhs_slices_number;

  while (lhs_slices_number-- > 0)
    *lhs_bits_ptr-- = (SXBA_ELT) 0;

  while (rhs_slices_number-- > 0)
    *lhs_bits_ptr-- = *rhs_bits_ptr--;
}

/* Retourne vrai ssi la lhs change */
static SXBOOLEAN
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

  SXBA	              lhs_bits_ptr, rhs_bits_ptr;
  SXINT	              slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
  SXBA_ELT            lwork, work;
  SXBOOLEAN             ret_val = SXFALSE;
    
  lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

#if EBUG
  sxbassert (*lhs_bits_array >= *rhs_bits_array, "OR (|X|<|Y|)");
  sxbassert ((*(lhs_bits_array+SXNBLONGS (SXBASIZE (lhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (lhs_bits_array) - 1))) == 0, "OR");
  sxbassert ((*rhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (rhs_bits_array) - 1))) == 0, "OR");
#endif

  while (slices_number-- > 0) {
    work = ((*rhs_bits_ptr-- | (lwork = *lhs_bits_ptr)));

    if (lwork /* ancienne valeur */ != work /* nouvelle valeur */) {
      /* A change' */
      ret_val = SXTRUE;
      *lhs_bits_ptr = work;
    }

    lhs_bits_ptr--;
  }

  return ret_val;
}


#if (is_guide || is_full_guide || is_supertagging) && has_strlen
/* La simple 1_RCG a des &Strlen, ils ont ete traduits en predicats <A>&p ou p
   est la longueur attendue.  Il faut donc appeler le predicat earley_strlen () */

static SXINT
earley_strlen (pass_no, item_p, p, q, prdct_no)
     SXINT pass_no, item_p, p, q, prdct_no;
{
  return q-p == prdct_no;
}
#endif /* (is_guide || is_full_guide || is_supertagging) && has_strlen */


#if is_lex || has_Lex || is_guide || is_full_guide
/* Ensemble des items compatibles avec la lexicalisation et les &Lex pour l'index i du source */
static SXBA lex_compatible_item_set;
#endif /* is_lex || has_Lex || is_guide || is_full_guide */



#if !is_parser && is_supertagging
/* Cas de l'appel de supertagger depuis le reconnaisseur */
static SXBA *item2source_index_set;

static void
glbl_out_source_validation (nb, item, i, j)
     SXINT nb, item, i, j;
{
  /* Il reste nb+1 terminaux a gauche de item ds la prod LISPRO[(tem), le plus a droite
     a un index < j */
  /* Ils doivent tous etre >= i */
  SXINT k;
  SXBA source_index_set;

  /* Ici, on est su^r que multiple_t_item_set != NULL */
  item = sxba_1_rlscan (multiple_t_item_set, item);
  /* item pointe sur le terminal le + a droite du prefixe */
  source_index_set = item2source_index_set [item];
  /* Cet item a ete trouve sur tous les index de source_index_set */

  k = j;

  if ((k = sxba_1_rlscan (source_index_set, k)) > 0) {
    /* k < j */
    /* Il est bon (merci Earley).  Les t a sa gauche ont ete trouves a des indices corrects */
    if (nb > 0)
      glbl_out_source_validation (nb-1, item, i, k);

    call_supertagger (item, k);
    SXBA_0_bit (source_index_set, k);

    while ((k = sxba_1_rlscan (source_index_set, k)) >= i) {
      /* Les appels a glbl_out_source_validation (nb-1, item, k) ne serviraient a rien */
      call_supertagger (item, k);
      SXBA_0_bit (source_index_set, k);
    }
  }
}


static void
supertag (prod, i, j) 
     SXINT prod, i, j;
{
  /* Cette procedure valide les terminaux de prod rencontres entre i et j */
  SXINT item, t_nb, k;
  SXBA source_index_set;

  if ((t_nb = prod2t_nb [prod]) != 0) {
    if (t_nb < 0) {
      /* Un seul terminal ds prod */
      item = -t_nb;
      source_index_set = item2source_index_set [item];
      k = i-1;

      while ((k = sxba_scan_reset (source_index_set, k)) > 0) {
	/* i <= k < j */
	/* On valide */
	call_supertagger (item, k);
      }
    }
    else {
      glbl_out_source_validation (t_nb-1, PROLON (prod+1)-1, i, j);
    }
  }
  else
    call_supertagger (PROLON (prod), 0);
}
#endif /* !is_parser && is_supertagging */

#if is_make_oracle
/* Campagne de mesures */
#if is_parser
    /* Cas de l'oracle */
static SXBA *prod2lb_set;
#else /* !is_parser */
    /* cas de la mesure pour le guide (lexicaliseur) */
static SXBA *i2guide_item_set;
#endif /* !is_parser */
#endif /* is_make_oracle */

#if is_guide
#if is_make_oracle
/* On detourne output_guide de son role... */
static void
output_guide (prod, lb_set, i, j) 
     SXINT prod, i, j;
     SXBA lb_set;
{
  if (lb_set) {
    sxba_or (prod2lb_set [prod], lb_set);
  }
  else {
    SXBA_1_bit (prod2lb_set [prod], i);
  }
}
#else /* !is_make_oracle */
static void
output_guide (prod, lb_set, i, j) 
     SXINT prod, i, j;
     SXBA lb_set;
{
  /* Attention : la bnf ne contient ni les clauses multiples, ni les loops */
  /* Donc clause (qui est le representant de prod) peut ne pas se retrouver ds une analyse RCG
     car elle a pu ne pas etre validee par des &Lex eventuels associes */
  /* Attention : il y a des duplicats prod[i..j] */
  /* On pourra ajouter l'arg "local_pid" a chaque appel de earley2guide quand il y aura de la modularite */
  SXINT clause, bot, top;

  /* Ici, l'appel eventuel a check_Lex a deja ete fait.  On est donc su^r que prod est valide
     vis-a-vis des &Lex.  De plus clause_val_set et les clause2identical ont ete positionnes correctement */

  clause = prod2clause [prod];
  bot = clause2identical_disp [clause];
  top = clause2identical_disp [clause+1];

#if has_Lex
  if (SXBA_bit_is_set (clause_has_Lex, clause)) {
    /* Le resultat des Lex de clause a ete memoize ds clause_val_set et clause2identical */
    if (SXBA_bit_is_set (clause_val_set, clause))
      earley2guide (clause, lb_set, i, j);

    do {
      if (clause = clause2identical [bot])
	earley2guide (clause, lb_set, i, j);
    } while (++bot < top);

    return;
  }
#endif /* has_Lex */

  /* Ici, les Lex eventuels ont deja ete verifies */
  earley2guide (clause, lb_set, i, j);
    
  if (bot < top) {
    do {
      earley2guide (clause2identical [bot], lb_set, i, j);
    } while (++bot < top);
  }
}
#endif /* !is_make_oracle */
#endif /* is_guide */

#if is_full_guide      
/* static SXINT lb [rhs_maxnt+1], ub [rhs_maxnt+1]; */
/* Le 28/05/2003 pour y mettre aussi les terminaux... */
#ifdef MAKE_INSIDEG
static SXINT *lb, *ub;
#else /* !MAKE_INSIDEG */
static SXINT lb [inputG_MAXRHSLGTH+1], ub [inputG_MAXRHSLGTH+1];
#endif /* !MAKE_INSIDEG */

#if has_streq
static SXBOOLEAN
check_streq (clause)
     SXINT clause;
{
  SXINT x, *p, j, h;
  SXINT	*src1, *src2, *lim1;
  SXBOOLEAN is_Streqlen;

  if (x = clause2streq [clause]) {
    p = streq_list+x;

    while (j = *p++) {
      if (j > 0) {
	is_Streqlen = SXTRUE;
	h = *p++;
      }
      else {
	is_Streqlen = SXFALSE;
	j = -j;
	h = -*p++;
      }

      if (ub [j] - lb [j] != ub [h] - lb [h])
	break; /* Echec de la longueur */

      if (!is_Streqlen) {
#if is_sdag
	/* On ne sait pas (encore) faire la verif Streq ds le cas is_sdag */
	sxtrap (ME, "&Streq is not yet implemented on simple dags.");
#else /* !is_sdag */
	{
	  /* Predicat predefini qui verifie l'egalite des chaines source comprises entre
	     lb[j]..ub[j] et lb[h]+1..ub[h]-1
	     On sait deja que les tailles sont identiques
	  */
	  *src1 = glbl_source + lb [j], *src2 = glbl_source + lb [h], *lim1 = glbl_source + ub [j];

	  while (src1 < lim1) {
	    if (*src1++ != *src2++)
	      break; /* Echec sur la chaine */
	  }
	}
#endif /* !is_sdag */
      }
    }

    return j==0;
  }

  return SXTRUE;
}
#endif /* has_streq */


/* Nouvelle version */
static SXINT
output_full_guide (rhs_stack)
    SXINT *rhs_stack;
{
  SXINT i, k, prod, h, B, j, x, y, item, nbnt, bot, top, clause, top_item, bot_item, nbt;
  SXINT *plb, *pub, *p;

  prod = *rhs_stack++;
  top_item = PROLON (prod+1)-1;
  bot_item = PROLON (prod);
  k = rhs_stack [top_item-bot_item]; /* C,a peut etre i */
  i = *rhs_stack++;

  /* Faut-il reordonner la pile ? */
  if (x = prod2rhs_stack_map [prod]) {
    p = rhs_stack_map+x;
  }
  else {
    p = NULL;
    plb = &(lb [0]);
    pub = &(ub [0]);
  }

  clause = prod2clause [prod];
  bot = clause2identical_disp [clause];
  top = clause2identical_disp [clause+1];
  nbnt = prod2nbnt [prod];
  nbt = 0;

  lb [0] = i;
  ub [0] = k;

  h = i;

  while (bot_item < top_item) {
    B = LISPRO (bot_item++);
    j = h;
    h = *rhs_stack++;

    if (B > 0) {
      if (p) {
	y = *p++;
	lb [y] = j;
	ub [y] = h;
      }
      else {
	*++plb = j;
	*++pub = h;
      }
    }
    else {
      /* Ajoute le 28/05/2003 */
      ub [nbnt + ++nbt] = h;
    }
  }

#if has_Lex
  if (SXBA_bit_is_set (clause_has_Lex, clause)) {
    /* Le resultat des Lex de clause a ete memoize ds clause_val_set et clause2identical */
    if (SXBA_bit_is_set (clause_val_set, clause)) {
#if has_streq
      /* On verifie les &Streq[len] */
      if (check_streq (clause))
	earley2full_guide (clause, nbnt, &(lb [0]), &(ub [0]), nbt);
#else /* !has_streq */
      earley2full_guide (clause, nbnt, &(lb [0]), &(ub [0]), nbt);
#endif /* !has_streq */
    }

    do {
      if (clause = clause2identical [bot]) {
#if has_streq
	/* On verifie les &Streq[len] */
	if (check_streq (clause))
	  earley2full_guide (clause, nbnt, &(lb [0]), &(ub [0]), nbt);
#else /* !has_streq */
	earley2full_guide (clause, nbnt, &(lb [0]), &(ub [0]), nbt);
#endif /* !has_streq */
      }
    } while (++bot < top);

    return SXTRUE;
  }
#endif /* has_Lex */

  /* Ici, les Lex eventuels ont deja ete verifies */
#if has_streq
  /* On verifie les &Streq[len] */
  if (check_streq (clause))
    earley2full_guide (clause, nbnt, &(lb [0]), &(ub [0]), nbt);
#else /* !has_streq */
  earley2full_guide (clause, nbnt, &(lb [0]), &(ub [0]), nbt);
#endif /* !has_streq */

  if (bot < top) {
    do {
      clause = clause2identical [bot];
#if has_streq
      /* On verifie les &Streq[len] */
      if (check_streq (clause))
	earley2full_guide (clause, nbnt, &(lb [0]), &(ub [0]), nbt);
#else /* !has_streq */
      earley2full_guide (clause, nbnt, &(lb [0]), &(ub [0]), nbt);
#endif /* !has_streq */
    } while (++bot < top);
  }
 
  return SXTRUE;
}

#endif /* is_full_guide */


#if HAS_BEAM
/* beam demande' */
static SXBOOLEAN
fill_Axpq2max_logprob_ptr (SXINT item_q, SXINT A, SXINT i, SXINT q)
{
  SXINT                 iq;
  SXLOGPROB                complete_inside_logprob, *max_inside_logprob_ptr;
  SXBOOLEAN             ret_val = SXTRUE;
  struct logprob_struct   *iq_logprob_ptr;

  iq = p_q2pq (i, q);
  Axpq2set_max_logprob_ptr (A, iq);
  SXBA_1_bit (ntXindex_set [A], i);
  max_inside_logprob_ptr = &(max_logprob_ptr->inside);
  ei2set_logprob_ptr (item_q, iq, iq_logprob_ptr);
  complete_inside_logprob = iq_logprob_ptr->inside.left + prod2logprob (PROLIS (item_q)); /* proba associee a Aiq */

  if (!is_AxI_J_set) {
    /* C'est la 1ere fois qu'on tombe sur une reduction vers Aiq dans la table T[q] */
    /* On associe a <A, i> complete_inside_logprob */
    *max_inside_logprob_ptr = complete_inside_logprob;
#if LLOG
    output_Apq_logprobs ("New ", A, i, q, max_logprob_ptr);
#endif /* LLOG */
  }
  else {
    /* On associe a <A, i> le max des complete_inside_logprob */
    if (cmp_SXLOGPROB (*max_inside_logprob_ptr, complete_inside_logprob) < 0 /* < */) {
      *max_inside_logprob_ptr = complete_inside_logprob;
#if LLOG
      output_Apq_logprobs ("Better ", A, i, q, max_logprob_ptr);
#endif /* LLOG */
    }
    else
      ret_val = SXFALSE;
  }

  return ret_val;
}

static void
raz_AxI_J_and_ei_for_j_greater_than (SXINT j)
{
  SXINT Apq, pq, q, local_ei;

  for (Apq = XxY_top (AxI_J_hd); Apq > 0; Apq--) {
    pq = XxY_Y (AxI_J_hd, Apq);
    q = range2q (pq);
    if (q >= j)
      XxY_erase (AxI_J_hd, Apq);
  }
  for (local_ei = XxY_top (ei_hd); local_ei > 0; local_ei--) {
    pq = XxY_Y (ei_hd, local_ei);
    q = range2q (pq);
    if (q >= j)
      XxY_erase (ei_hd, local_ei);
  }
}
#endif /* HAS_BEAM */

#if HAS_BEAM == 1
/* i.e., #if RECOGNIZER_KIND */

/* Chaque table est execute'e 2 fois
   la 1ere passe calcule pour chaque item earley les proba prefixe et inside
   apres cette 1ere passe on elimine les items dont la proba prefixe est inferieure au
   seuil determine a partir de la meilleure proba et de la beam_value.
   On en deduit une liste des items valides que l'on epercute ds T2_non_kernel_item_set
   la 2eme passe est une passe normale
*/
/* Le 21/09/10 Nouvelle version dans laquelle on ne propage plus individuellement les logprobs
   prefixe et suffixe mais uniquement leur somme rassemblee dans une proba outside */
static void
set_non_kernel_item_logprobs_recognizer (SXINT p)
{
  /* Les nt ayant produits par la relation LEFT_CORNER< les items de T2_non_kernel_item_set
     sont ds T2_kernel_nt_set et leur proba ds nt2max_outside_proba.
     Repercute ces logprobs sur les items de T2_non_kernel_item_set */
  /* On commence par trier les logprobs */
  SXINT                 nt, item, null_left_item, pp, prod, A, Y;
  SXLOGPROB                nt_outside_logprob, outside_logprob, total_logprob, *nt_nullable_max_prefix_logprob, null_left_logprob, *nt_max_suffix_logprob;
  struct logprob_struct   *logprob_ptr;

  nt = 0;

  while ((nt = sxba_scan_reset (T2_kernel_nt_set, nt)) > 0) {
    nt_outside_logprob = nt2max_outside_logprob [nt];
#if is_epsilon
    nt_nullable_max_prefix_logprob = nt2nt_nullable_max_prefix_logprob [nt];
#endif /* is_epsilon */
    nt_max_suffix_logprob = nt2nt_max_suffix_logprob [nt];

    item = 0;

    while ((item = sxba_scan (NT2ITEM_SET (nt), item)) >= 0) {
      if (SXBA_bit_is_set (T2_non_kernel_item_set, item)) {
	prod = PROLIS (item);
	A = LHS (prod);
	outside_logprob = nt_outside_logprob + prod2logprob (prod);

#if is_epsilon
	if (A != nt)
	  outside_logprob += nt_nullable_max_prefix_logprob [A];

	null_left_logprob = item2nullable_max_prefix_logprob [item];
#else /* is_epsilon */
	null_left_logprob = 0;
#endif /* is_epsilon */

	if (A != nt) {
#if EBUG
	  if (nt_max_suffix_logprob [A] > 0)
	    sxtrap (ME, "set_non_kernel_item_logprobs_recognizer (found (A0,A1) whose nt2max_outside_logprob is > 0 (initilization value))");	    
#endif /* EBUG */
	  outside_logprob += nt_max_suffix_logprob [A];
	}

	total_logprob = outside_logprob + null_left_logprob + item2max_right_inside_logprob [item];

	pp = p_q2pq (p, p);

	ei2set_logprob_ptr (item, pp, logprob_ptr);

	if (!is_ei_set ||
	    cmp_SXLOGPROB (logprob_ptr->expected_total, total_logprob) < 0 /* < */) {
	    logprob_ptr->outside = outside_logprob;
	    logprob_ptr->inside.left = null_left_logprob;
	    logprob_ptr->expected_total = total_logprob;
	}

#if LLLOG
	printf ("Made and filled ei: (item=" SXILD ", pq=" SXILD ") [ei=" SXILD " (%s)] {outside = " SXLPD ", inside.left = " SXLPD ", expected_total = " SXLPD "}: ", item, pp, ei, is_ei_set ? "old"  : "new", SXLOGPROB_display (logprob_ptr->outside), SXLOGPROB_display (logprob_ptr->inside.left), SXLOGPROB_display (logprob_ptr->expected_total));
	output_ei (ei);
#endif /* LLLOG */
	
#if is_epsilon
	if (!is_ei_set) {
	  Y = LISPRO (item);

	  if (
#ifdef MAKE_INSIDEG
	  spf.insideG.bvide &&
#endif /* MAKE_INSIDEG */
	  Y > 0 && SXBA_bit_is_set (EMPTY_SET, Y)) {
	    if (!Axpq2set_max_logprob_ptr (Y, pp)) {
	      /* 1ere fois qu'on rencontre Yqq */
	      max_logprob_ptr->inside = nullable_nt2max_nullable_logprob [Y];
#if LLOG
	      output_Apq_logprobs ("New ", Y, p, p, max_logprob_ptr);
#endif /* LLOG */
	    }
	  }
	  else {
	    if (Y == 0) {
	      /* position reduce */
	      if (!Axpq2set_max_logprob_ptr (A, pp)) {
		/* 1ere fois qu'on rencontre A,p,p */
		max_logprob_ptr->inside = null_left_logprob + prod2logprob (prod);
#if LLOG
		output_Apq_logprobs ("New ", A, p, p, max_logprob_ptr);
#endif /* LLOG */
	      }
	    }
	  }
	}
#endif /* is_epsilon */
      }
    }
  }
}

#if REDUCER_KIND
static void
set_non_kernel_item_logprobs_reducer (SXINT p)
{
  /* Les nt ayant produits par la relation LEFT_CORNER les items de T2_non_kernel_item_set
     sont ds T2_kernel_nt_set et leur proba ds nt2max_outside_proba.
     Repercute ces logprobs sur les items de T2_non_kernel_item_set */
  /* On commence par trier les logprobs */
  SXINT                 item, prod, prev_prod, A, pp;
  SXLOGPROB                null_logprob;
  struct logprob_struct   *logprob_ptr;

  pp = p_q2pq (p, p);
  item = 3;

#if is_epsilon
  prev_prod = 0;
#endif /* is_epsilon */

  while ((item = sxba_scan (T2_non_kernel_item_set, item)) >= 0) {
    ei2set_logprob_ptr (item, pp, logprob_ptr);

#if is_epsilon
    if ((prod = PROLIS (item)) == prev_prod) {
      logprob_ptr->inside.left = null_logprob += nullable_nt2max_nullable_logprob [LISPRO (item-1)];
    }
    else {
      logprob_ptr->inside.left = null_logprob = 0;
      prev_prod = prod;
    }
#else /* is_epsilon */
    logprob_ptr->inside.left = 0;
#endif /* is_epsilon */
	  
#if LLLOG
    printf ("Made and filled ei: (item=" SXILD ", pq=" SXILD ") [ei=" SXILD " (%s)] {inside.left = " SXLPD "}: ", item, pp, ei, is_ei_set ? "old"  : "new", SXLOGPROB_display (logprob_ptr->inside.left));
	output_ei (ei);
#endif /* LLLOG */

#if is_epsilon
    if (LISPRO (item) == 0) {
      /* position reduce */
      A = LHS (PROLIS (item));

      if (!Axpq2set_max_logprob_ptr (A, pp)) {
	/* 1ere fois qu'on rencontre A,p,p */
	max_logprob_ptr->inside = nullable_nt2max_nullable_logprob [A];
#if LLOG
	output_Apq_logprobs ("New ", A, p, p, max_logprob_ptr);
#endif /* LLOG */
      }
    }
#endif /* is_epsilon */
  }
}
#endif /* REDUCER_KIND */
#endif /* HAS_BEAM == 1 */



#if LOG || LLOG || EBUG
static void
output_rcvr_rhs (SXINT litem, SXINT lmlstn, SXINT ritem, SXINT rmlstn)
{
  SXINT top_item, bot_item, prod, X;

  prod = PROLIS (litem);
  top_item = PROLON (prod+1)-1;
  bot_item = PROLON (prod);

  while (bot_item <= top_item) {
    if (bot_item == litem) {
      printf ("[" SXILD "] ", lmlstn);
    }

    if (bot_item == ritem) {
      printf ("[" SXILD "] ", rmlstn);
    }

    if (bot_item == top_item)
      break;

    X = LISPRO (bot_item);

    if (X < 0)
      printf ("\"%s\" ", TSTRING (-X));
    else
      printf ("<%s> ", NTSTRING (X));

    bot_item++;
  }

  fputs (";", stdout);
}

static void
output_rcvr_prod (SXINT litem, SXINT lmlstn, SXINT ritem, SXINT rmlstn)
{
  SXINT prod;

  prod = PROLIS (litem);
  printf (SXILD ":<%s> = ", prod, NTSTRING (LHS (prod)));
  output_rcvr_rhs (litem, lmlstn, ritem, rmlstn);
}

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

  printf (SXILD "\t", prod);

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

static void
output_ni_prod (SXINT prod, SXBA lb_set, SXINT i, SXINT j) 
{
  SXINT top_item, bot_item, lb, X;

  printf (SXILD ":<%s[", PROD (prod), NTSTRING (LHS (prod)));

  if (lb_set == NULL)
    printf (SXILD, i);
  else {
    fputs ("{", stdout);
    lb = sxba_scan (lb_set, -1);
    printf (SXILD, lb);

    while ((lb = sxba_scan (lb_set, lb)) >= 0) {
      printf (", " SXILD, lb);
    }

    fputs ("}", stdout);
  }
    
  printf (".." SXILD "]>\t= ", j);

  top_item = PROLON (prod+1)-1;
  bot_item = PROLON (prod);

  while (bot_item < top_item) {
    X = LISPRO (bot_item++);

    if (X < 0)
      printf ("\"%s\" ", TSTRING (-X));
    else
      printf ("<%s> ", NTSTRING (X));
  }

  fputs (";\n", stdout);
}

#if LOG || LLOG
static SXBOOLEAN is_in_output_table;

static void
output_item_without_nl (SXINT item_j, SXBA index_set_j, SXINT i, SXINT j)
{
  SXINT       prod, A, top_item, bot_item, X;
  SXBOOLEAN   is_init_item;
  static char *mark = "!!", *ptr;

#if HAS_BEAM
  SXLOGPROB                i_r, o_s;
  struct logprob_struct   *logprob_ptr;
#endif /* HAS_BEAM */

  prod = PROLIS (item_j);
  A = LHS (prod);

  printf ("[" SXILD ":<%s> = ", prod, NTSTRING (A));

  top_item = PROLON (prod+1)-1;
  bot_item = PROLON (prod);
  is_init_item = bot_item == item_j;

  while (bot_item < top_item) {
    if (bot_item == item_j)
      fputs (". ", stdout);

    X = LISPRO (bot_item++);

    if (X < 0)
      printf ("\"%s\" ", TSTRING (-X));
    else
      printf ("<%s> ", NTSTRING (X));
  }
    
  if (bot_item == item_j)
    fputs (". ", stdout);

  if (index_set_j)
    i = sxba_scan (index_set_j, -1);
  else {
    if (is_init_item)
      i = j;
  }

  if (i >= 0) {
    printf (", {" SXILD, i);

#if HAS_BEAM
    if ((recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) && (get_ei_if_set (item_j, p_q2pq (i, j)))) {
      /* On n'utilise pas ei2get_logprob_ptr (item_j, p_q2pq (i, j), logprob_ptr); car
	 l'item peut ne pas etre valide pour les logprobs */
      logprob_ptr = &(logprobs [ei]);
      i_r = 0;

      if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
	if (item2max_right_inside_logprob)
	  i_r = item2max_right_inside_logprob [item_j];
      }
#if REDUCER_KIND
      else {
	i_r = logprob_ptr->inside.right;
      }
#endif /* REDUCER_KIND */

      ptr = mark + strlen (mark);

#  if RECOGNIZER_KIND == 2
      if ((recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) && is_in_output_table && cmp_SXLOGPROB (logprob_ptr->expected_total, earley_table_companion [j].max_global_expected_logprob) > 0) {
	ptr = mark ;
	printf (" ----  max global expected logprob = " SXLPD, SXLOGPROB_display (earley_table_companion [j].max_global_expected_logprob));
      }
#  endif /* RECOGNIZER_KIND == 2 */

      printf (" [o=(u.b.)" SXLPD ", i.l=" SXLPD ", i.r(u.b.)=" SXLPD "]=" SXLPD "(p2p=" SXLPD ")%s", SXLOGPROB_display (logprob_ptr->outside), SXLOGPROB_display (logprob_ptr->inside.left), SXLOGPROB_display (i_r),
	      SXLOGPROB_display (logprob_ptr->expected_total), SXLOGPROB_display (prod2logprob (prod)), ptr);
    }
#endif /* HAS_BEAM */
  }

  if (index_set_j && i >= 0) {
    while ((i = sxba_scan (index_set_j, i)) > 0) {
      printf (", " SXILD, i);

#if HAS_BEAM
      if ((recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) && (get_ei_if_set (item_j, p_q2pq (i, j)))) {
	logprob_ptr = &(logprobs [ei]);

	if (pass_number == 1) {
	  if (item2max_right_inside_logprob)
	    i_r = item2max_right_inside_logprob [item_j];
	  else
	    i_r = 0;
	}
#if REDUCER_KIND
	else {
	  if (reducer_beam_value_is_set) {
	    i_r = logprob_ptr->inside.right;
	  }
	}
#endif /* REDUCER_KIND */

	ptr = mark + strlen (mark);

#  if RECOGNIZER_KIND == 2
	if ((recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) && is_in_output_table && cmp_SXLOGPROB (logprob_ptr->expected_total, earley_table_companion [j].max_global_expected_logprob + earley_table_companion [j].dynamic_scan_beam_value) < 0) {
	  ptr = mark ;
	  printf (" ++++  min admissible logprob = " SXLPD, SXLOGPROB_display (earley_table_companion [j].max_global_expected_logprob + earley_table_companion [j].dynamic_scan_beam_value));
	}
#  endif /* RECOGNIZER_KIND == 2 */

	if (reducer_beam_value_is_set)
	  printf (" [o=(u.b.)" SXLPD ", i.l=" SXLPD ", i.r(u.b.)=" SXLPD "]=" SXLPD "(p2p=" SXLPD ")%s", SXLOGPROB_display (logprob_ptr->outside), SXLOGPROB_display (logprob_ptr->inside.left), SXLOGPROB_display (i_r),
		  SXLOGPROB_display (logprob_ptr->expected_total), SXLOGPROB_display (prod2logprob (prod)), ptr);
	else
	  printf (" [o=(u.b.)" SXLPD ", i.l=" SXLPD "]=" SXLPD "(p2p=" SXLPD ")%s", SXLOGPROB_display (logprob_ptr->outside), SXLOGPROB_display (logprob_ptr->inside.left),
		  SXLOGPROB_display (logprob_ptr->expected_total), SXLOGPROB_display (prod2logprob (prod)), ptr);
      }
#endif /* HAS_BEAM */
    }
  }

  if (index_set_j || i >= 0) 
    fputs ("}", stdout);
  
  putchar (']');
}


static void
output_item (SXINT item_j, SXBA index_set_j, SXINT i, SXINT j)
{
  output_item_without_nl (item_j, index_set_j, i, j);
  putchar ('\n');
}

static void
output_table (SXINT q)
{
  struct recognize_item	*RTq, *RTp;
  SXINT			A, item_q, *top_ptr, *bot_ptr, p;
  SXBA                  index_set_q;

  is_in_output_table = SXTRUE;

  RTq = &(RT [q]);
#if RECOGNIZER_KIND == 2
  if ((recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) && earley_table_companion [q].beamrcvr_state)
    printf ("\n************************** T [" SXILD "] <BEAMRCVR (%s)> **************************\n", q, earley_table_companion [q].beamrcvr_state == 1 ? "local" : "global");
  else 
#endif /* RECOGNIZER_KIND == 2 */
    printf ("\n************************** T [" SXILD "] **************************\n", q);
  
#if RECOGNIZER_KIND == 2
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
    printf ("earley_table_companion [" SXILD "] . max_global_expected_logprob = " SXLPD "\n", q, SXLOGPROB_display (earley_table_companion [q].max_global_expected_logprob));
    printf ("earley_table_companion [" SXILD "] . dynamic_scan_beam_value = " SXLPD "\n", q, SXLOGPROB_display (earley_table_companion [q].dynamic_scan_beam_value));
    printf ("earley_table_companion [" SXILD "] . dynamic_shift_beam_value = " SXLPD "\n", q, SXLOGPROB_display (earley_table_companion [q].dynamic_shift_beam_value));
    printf ("earley_table_companion [" SXILD "] . dynamic_scan_beam_first_beamed_logprob = " SXLPD "\n", q, SXLOGPROB_display (earley_table_companion [q].dynamic_scan_beam_first_beamed_logprob));
    printf ("earley_table_companion [" SXILD "] . dynamic_shift_beam_first_beamed_logprob = " SXLPD "\n", q, SXLOGPROB_display (earley_table_companion [q].dynamic_shift_beam_first_beamed_logprob));
  }
#if LLOG
  if (q > 0) {
    SXINT *pq_stack = idag.q2pq_stack + idag.q2pq_stack_hd [q];
    SXINT *pq_ptr, p, t, pq, item_p;
    SXBA pq_t_set;
    SXINT best_t;
    SXLOGPROB best;

    for (pq_ptr = pq_stack + pq_stack [0]; pq_ptr > pq_stack; pq_ptr--) {
      pq = *pq_ptr;
      pq_t_set = idag.pq2t_set [pq];
      p = range2p (pq);
      RTp = &(RT [p]);
      top_ptr = RTp->shift_NT_hd [0];
      best_t = 0;
      best = SXLOGPROB_MIN;
      
      if (top_ptr) { /* sinon, table Earley T [q] vide */
	bot_ptr = &(RTp->items_stack [0]);
	
	while (--top_ptr >= bot_ptr) {
	  item_p = *top_ptr;
	  
	  if (item_p < 0) /* item_p a été viré précédemment */
	    continue;
	  
	  t = -LISPRO (item_p);
#ifdef MAKE_INSIDEG
	  t = spf.insideG.t2init_t [t];
#endif /* MAKE_INSIDEG */
	  
	  if (SXBA_bit_is_set (pq_t_set, t)) {
	    if (cmp_SXLOGPROB (best, pq_t2logprob (pq, t)) < 0) {
	      best = pq_t2logprob (pq, t);
	      best_t = t;
	    }
	  }
	}
      }
      if (best_t && (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)) {
	printf ("ITEM BEAM EXPLORE\tearley_table_companion [" SXILD "] . max_global_expected_logprob - earley_table_companion [" SXILD "] . max_global_expected_logprob = " SXLPD "\n", q, p, SXLOGPROB_display (earley_table_companion [q].max_global_expected_logprob - earley_table_companion [p].max_global_expected_logprob));
	printf ("ITEM BEAM EXPLORE\t\tBest terminal %s: pq_t2logprob = " SXLPD "\n", tstring [t] /* t a déjà été converti en t de la inputG ci-dessus, donc pas de TSTRING (t) */, SXLOGPROB_display (pq_t2logprob (pq, t)));
      }
    }
  }
#endif /* LLOG */
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
    printf ("min admissible logprob (= idem + beam_value) = " SXLPD "\n", SXLOGPROB_display (earley_table_companion [q].max_global_expected_logprob + earley_table_companion [q].dynamic_scan_beam_value));
#endif /* RECOGNIZER_KIND == 2 */
  fputs ("************************** SCAN ****************************\n", stdout);
  top_ptr = RTq->shift_NT_hd [0];
  bot_ptr = &(RTq->items_stack [0]);

  while (bot_ptr < top_ptr) {
    item_q = *bot_ptr++;

    if (item_q >= 0) {
      index_set_q = RTq->index_sets [item_q];
      output_item (item_q, index_set_q, q, q);
    }
  }

  fputs ("************************* SHIFT ****************************\n", stdout);
  for (A = 1; A <= MAXNT; A++) {
    top_ptr = RTq->shift_NT_hd [A];
    bot_ptr = &(RTq->items_stack [0]) + RHS_NT2WHERE (A);

    while (bot_ptr < top_ptr) {
      item_q = *bot_ptr++;

      if (item_q >= 0) {
	index_set_q = RTq->index_sets [item_q];
	output_item (item_q, index_set_q, q, q);
      }
    }
  }

#if is_parser
  fputs ("************************* REDUCE ***************************\n", stdout);
  for (A = 1; A <= MAXNT; A++) {
    if ((top_ptr = PT [q].reduce_NT_hd [A])) {
      bot_ptr = &(RTq->items_stack [LHS_NT2WHERE (A)]);

      while (bot_ptr < top_ptr) {
	item_q = *bot_ptr++;

	if (item_q >= 0) {
	  index_set_q = RTq->index_sets [item_q];
	  output_item (item_q, index_set_q, q, q);
	}
      }
    }
  }
#endif /* is_parser */
  printf ("********************** End T [" SXILD "] **************************\n", q);

  is_in_output_table = SXFALSE;
}
#endif /* LOG || LLOG */
#endif /* LOG || LLOG || EBUG */


#if REDUCER_KIND && EBUG
static void
check_logprob (struct logprob_struct *logprob_ptr, SXINT item, SXINT i, SXINT j)
{
  SXLOGPROB logprob = logprob_ptr->expected_total;

  if (cmp_SXLOGPROB (logprob, best_tree_logprob) > 0 /* > */) {
#if LLOG
    if (item > 0 && i > 0 && j > 0) {
      printf ("Error: found erroneous logprob (" SXLPD " > max_tree_logprob = " SXLPD ") on the following item:\n\t\t", SXLOGPROB_display (logprob), SXLOGPROB_display (best_tree_logprob));
      output_item (item, NULL, i, j);
    }
#endif /* LLOG */
    sxtrap (ME, "check_item_logprob (found logprob that is strictly better than that of the best tree)");
  }
}
#endif /* REDUCER_KIND && EBUG */


/* Nouvelle version du 16/05/2013 de [beam_]shift et scan_reduce
   qui ne tient pas compte de l'"ordre" des nt et qui peut donc s'utiliser dans tous les cas (cyclique, rcvr, insideG)
*/

static SXBOOLEAN
check_scans (SXINT p)
{
  SXINT *top_ptr, *bot_ptr, item_p, t;
  SXBA t_set = idag.p2t_set [p];
  struct recognize_item* RTp = &(RT [p]);
  SXBOOLEAN scan_check_successful = SXFALSE;

  top_ptr = RTp->shift_NT_hd [0];

  if (top_ptr) {
    bot_ptr = &(RTp->items_stack [0]);
    
    while (--top_ptr >= bot_ptr) {
      item_p = *top_ptr;

      if (item_p < 0) { /* item_p a été viré précédemment */
#if EBUG
        sxtrap (ME, "check_scans (found a negative item)"); // on ne voit pas pourquoi ça pourrait arriver...
#endif /* EBUG */
	continue;
      }
      
      t = -LISPRO (item_p);
#ifdef MAKE_INSIDEG
      t = spf.insideG.t2init_t [t];
#endif /* MAKE_INSIDEG */
      /* l'item item_p est de la forme A -> \alpha • t \beta */
      
      
      if (!SXBA_bit_is_set (t_set, t))
	*top_ptr = -item_p; /* on vire l'item, puisqu'il n'y a pas moyen de sortir de p par t */
      else
	scan_check_successful = SXTRUE;
    }
  }

  return scan_check_successful;
}


#if RECOGNIZER_KIND == 2
static SXBOOLEAN
fill_RT_global_logprobs (SXINT p)
{
  SXINT *top_ptr, *bot_ptr, item_p, t;
  struct RT_global_logprobs *RT_global_logprobs_p = &(RT_global_logprobs [p]);
  struct recognize_item *RTp = &(RT [p]);
  SXBA index_set_p, t_set = idag.p2t_set [p];
  SXLOGPROB local_max_logprob, local_logprob;
  struct RT_logprobs *tmp_RT_logprob_ptr;
  struct logprob_struct *p_logprob_ptr;
  SXBOOLEAN scan_check_successful = SXFALSE;
#if LLOG
  printf ("*** Entering fill_RT_global_logprobs (p = " SXILD ")\n", p);
#endif /* LLOG */

#if EBUG
  if (p == 0)
    sxtrap (ME, "fill_RT_global_logprobs (this function should never be called with p = 0)");
#endif /* EBUG */

  top_ptr = RTp->shift_NT_hd [0];

  if (top_ptr) {
    bot_ptr = &(RTp->items_stack [0]);

    while (--top_ptr >= bot_ptr) {
      item_p = *top_ptr;

      if (item_p < 0) /* item_p a été viré précédemment */
	continue;
    
      t = -LISPRO (item_p);
#ifdef MAKE_INSIDEG
      t = spf.insideG.t2init_t [t];
#endif /* MAKE_INSIDEG */
      /* l'item item_p est de la forme A -> \alpha • t \beta */

      if (SXBA_bit_is_set (t_set, t)) {
	/* il existe au moins une transition sortante de p par t */      
	index_set_p = RTp->index_sets [item_p];

	tmp_RT_logprob_ptr = &(RT_global_logprobs_p->RT_logprobs [top_ptr - bot_ptr]);

	if (index_set_p == NULL) { // il n'y a qu'une seule proba à stocker puisque qu'on est en tout début de partie droite
	
	  ei2get_logprob_ptr (item_p, p_q2pq (p, p), p_logprob_ptr);
	  local_max_logprob = p_logprob_ptr->outside;
#if EBUG
	  if (p_logprob_ptr->inside.left != 0)
	    sxtrap (ME, "fill_RT_global_logprobs (found a non-null left logprob for an item with the dot at the left of the right-hand side)");
#endif /* EBUG*/
	  local_max_logprob += item2max_right_inside_logprob [item_p+1]; /* pour que la proba soit complète il manque pq_t2logprob (pq, t), mais on n'a pas encore choisi un q donné; on rajoutera ça une fois arrivé sur q */
	  tmp_RT_logprob_ptr->offset_in_global_logprobs = -1;
	}
	else {
	  SXINT i = -1;

	  local_max_logprob = SXLOGPROB_MIN;

	  tmp_RT_logprob_ptr->offset_in_global_logprobs = RT_global_logprobs_p->global_logprobs_top;
	
	  while ((i = sxba_scan (index_set_p, i)) >= 0) {
	    ei2get_logprob_ptr (item_p, p_q2pq (i, p), p_logprob_ptr);
	    local_logprob = p_logprob_ptr->outside + p_logprob_ptr->inside.left;
	    local_logprob += item2max_right_inside_logprob [item_p+1]; /* pour que la proba soit complète il manque pq_t2logprob (pq, t), mais on n'a pas encore choisi un q donné; on rajoutera ça une fois arrivé sur q */

#if LLOG
	    printf ("ei: (item=" SXILD ", pq=" SXILD ") [ei=" SXILD " (%s)] {outside = " SXLPD ", inside.left = " SXLPD " (=> local_logprob = " SXLPD ")}: ", item_p, p_q2pq (i, p), ei, is_ei_set ? "old"  : "new", SXLOGPROB_display (p_logprob_ptr->outside), SXLOGPROB_display (p_logprob_ptr->inside.left), SXLOGPROB_display (local_logprob));
	output_ei (ei);
#endif /* LLOG */

	    RT_global_logprobs_p->global_logprobs [RT_global_logprobs_p->global_logprobs_top] = local_logprob;
	    if (++RT_global_logprobs_p->global_logprobs_top > RT_global_logprobs_p->global_logprobs_allocated_size) {
	      RT_global_logprobs_p->global_logprobs_allocated_size *= 2;
	      RT_global_logprobs_p->global_logprobs = (SXLOGPROB *) sxrealloc (RT_global_logprobs_p->global_logprobs, RT_global_logprobs_p->global_logprobs_allocated_size + 1, sizeof (SXLOGPROB));
	    }
	  
	    if (cmp_SXLOGPROB (local_logprob, local_max_logprob) > 0 /* > */)
	      local_max_logprob = local_logprob;
	  }
	}
	tmp_RT_logprob_ptr->max_global_logprob = local_max_logprob;
	scan_check_successful = SXTRUE;

#if LOG
	printf ("Keeping structurally admissible item (item_p=" SXILD ") : ", item_p);
	output_item (item_p, NULL, -1, p);
	printf ("        local_max_logprob = " SXLPD " (logprob to which pq_t2logprob (pq, t) is missing, q being non-specified here)\n", SXLOGPROB_display (local_max_logprob));
#endif /* LOG */	
      }
      else {
	*top_ptr = -item_p; /* on vire l'item, puisqu'il n'y a pas moyen de sortir de p par t */
#if LOG
	printf ("Discarding item for structural reasons (item_p=" SXILD ") : ", item_p);
	output_item (item_p, NULL, -1, p);
#endif /* LOG */
      }
    }
  }
#if LLOG
  printf ("*** Leaving fill_RT_global_logprobs (p = " SXILD "): %s ***\n", p, scan_check_successful ? "SXTRUE" : "SXFALSE");
#endif /* LLOG */

  return scan_check_successful;
}
#endif /* RECOGNIZER_KIND == 2 */


#if HAS_BEAM == 1
/* i.e., #if RECOGNIZER_KIND */
static SXBOOLEAN
beam_shift (SXINT A, SXINT p, SXINT q)
{
  /*
    Realise la transition Apq
    p < q
    Si A == 0 => transition terminale
  */
  SXINT			item_p, item_q;
  SXINT			**ptr2;
  SXINT 		X, B, Y, YY, prod, new_p, prdct_no, order, i, t;
  SXBA			index_set_p, index_set_q, cur_index_set_q, backward_index_set_q, index_set_B;
  struct recognize_item	*RTp;
  SXINT			*top_ptr, *bot_ptr, *t_top_ptr;
  SXBOOLEAN		ret_val, all_erased, has_cur_indexes, first_item_q;
  SXINT                 pq, iq;
  struct logprob_struct   *p_logprob_ptr;
  SXLOGPROB                Apq_inside_logprob, inside_logprob, max_logprob, new_logprob, p_logprob, local_max_logprob;
  SXLOGPROB                min_admissible_logprob;
#if RECOGNIZER_KIND == 2
  SXINT                 offset;
  SXINT                 ioffset;
  struct RT_global_logprobs *RT_global_logprobs_p;
  SXBOOLEAN             is_on_a_terminal_transition_for_the_first_time;
  struct RT_logprobs      *tmp_RT_logprob_ptr;
  SXINT                tmp_offset_in_global_logprobs;
  SXLOGPROB thrshld;
  SXLOGPROB local_pq_t_logprob, local_logprob;
  struct earley_table_companion *earley_table_companion_q;
#endif /* RECOGNIZER_KIND == 2 */

#if LLOG
  printf ("*** Entering beam_shift (A = %s (" SXILD "), p = " SXILD ", q = " SXILD ")\n", A > 0 ? NTSTRING (A) : "", A, p, q);
#endif /* LLOG */

  ret_val = SXFALSE;

  RTp = &(RT [p]);

  top_ptr = RTp->shift_NT_hd [A];
    
#if is_lex
  if (top_ptr == NULL && A > 0) {
    /* On peut se retrouver ici si l'on a fait une lexicalisation mais qu'on ne construit pas de grammaire inside. En
       effet, on peut avoir rajouté dans une table un item sur la seule foi des item_sets (de la grammaire initiale),
       alors que cet item ne pouvait être atteint depuis les symboles de partie droite des items de ladite table. Et du
       coup, on peut se retrouver à réduire l'un de ces mauvais items, et à chercher quel autre item avait bien pu
       "prédire" ledit mauvais item... et ne rien trouver! */
#if LLOG
    printf ("*** Leaving beam_shift (A = %s (" SXILD "), p = " SXILD ", q = " SXILD ") SXFALSE (lex)\n\n", A > 0 ? NTSTRING (A) : "", A, p, q);
#endif /* LLOG */
    return SXFALSE;
  }
#endif /* is_lex */

#if RECOGNIZER_KIND == 2
  if (top_ptr == NULL && idag.dag_kind == DAG_KIND && *(idag.q2pq_stack + idag.q2pq_stack_hd [q]) > 1) {
    /* On peut se retrouver ici si on examine une transition (vers q) en provenance de p, avec T [p] vidée par le beam.
       Cela n'est possible que sur un vrai dag, et sur un q auquel on peut arriver depuis plusieurs p distincts */
#if LLOG
    printf ("*** Leaving beam_shift (A = %s (" SXILD "), p = " SXILD ", q = " SXILD ") SXFALSE (DAG + fully beamed source table)\n\n", A > 0 ? NTSTRING (A) : "", A, p, q);
#endif /* LLOG */
    return SXFALSE;
  }

  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
    earley_table_companion_q = earley_table_companion + q;
    min_admissible_logprob = earley_table_companion_q->max_global_expected_logprob + earley_table_companion_q->dynamic_scan_beam_value;
  }
#endif /* RECOGNIZER_KIND == 2 */
  
#if EBUG
  if (top_ptr == NULL) 
    sxtrap (ME, "beam_shift");
#endif /* EBUG */

  bot_ptr = &(RTp->items_stack [0]);

  if (A > 0)
    bot_ptr += RHS_NT2WHERE (A);
#if EBUG && is_rcvr
  else {
    if (is_during_error_rcvr_processing)
      /* Pas de scan durant la rcvr */
      sxtrap (ME, "beam_shift");
  }
#endif /* EBUG && is_rcvr */

  pq = p_q2range(p, q);

  if (A > 0) {
#if is_rcvr
    if (!is_during_reduce_rcvr_validation)
      /* La inside proba de Apq a du etre calculee par la rcvr (voir earley_glbl_rcvr_reduce_pre) */
#endif /* is_rcvr */
      {
	Axpq2get_max_logprob_ptr (A, pq);
	Apq_inside_logprob = max_logprob_ptr->inside;
      }
  }

#if RECOGNIZER_KIND == 2
  offset = top_ptr - bot_ptr;
  RT_global_logprobs_p = &(RT_global_logprobs [p]);
#endif /* RECOGNIZER_KIND == 2 */

  while (--top_ptr >= bot_ptr) {
#if 0
    ret_val = SXTRUE; /* Le 21/11/06 */
#endif /* 0 */

#if RECOGNIZER_KIND == 2
    offset--;
#endif /* RECOGNIZER_KIND == 2 */
    
    item_p = *top_ptr;

    if (item_p < 0) {
      /* Le beaming a vire' cet item */
      *top_ptr = -item_p; /* On le remet pour un futur q'≠q tel que item_p permette à la fois p-t->q et p-t->q' */

      continue;
    }

    /* on ne peut arriver ici que sur un item_p permettant d'aller de p à q, sinon le continue ci-dessus se serait appliqué */
    
    if (A == 0) {
      t = -LISPRO (item_p);
      X = -t;

#ifdef MAKE_INSIDEG
      t = spf.insideG.t2init_t [t];
#endif /* MAKE_INSIDEG */

      if (!SXBA_bit_is_set (idag.pq2t_set [pq], t)) /* on n'a pas de transition p-t->q, donc on abandonne ce coup là */
	continue;
      
#if is_rcvr
      if (!is_during_reduce_rcvr_validation)
#endif /* is_rcvr */
	inside_logprob = (pq == idag.last_pq + 1 /* transition 0->1 */) ? 0 : pq_t2logprob (pq, t);
    }
    else {
      X = A;

#if is_rcvr
      if (!is_during_reduce_rcvr_validation)
#endif /* is_rcvr */
	inside_logprob = Apq_inside_logprob;
    }

    /* item_p = B -> \beta .X Y \gamma */

    item_q = item_p+1;
    /* item_q = B -> \beta X .Y \gamma */
    Y = LISPRO (item_q);

    /* On verifie le predicat de item_p */
#if is_prdct
    if (is_prdct_fun &&
	(prdct_no = prdct [item_p]) >= 0 &&
	!(*for_semact.prdct) (1 /* No de passe : reconnaisseur */, item_p, p, q, prdct_no))
      /* Echec du predicat */
      continue;
#endif /* is_prdct */
    
#if EBUG
    if (A > 0 && A != LISPRO (item_p))
      sxtrap (ME, "beam_shift");
#endif /* EBUG */

    index_set_p = RTp->index_sets [item_p];

#if LLLOG
    fputs ("Examining ", stdout);
    output_item (item_p, index_set_p, p, p);
#endif /* LLLOG */

#if is_parser
    new_p = p;
#endif /* is_parser */

#if RECOGNIZER_KIND == 2
    is_on_a_terminal_transition_for_the_first_time = (A == 0);
#endif /* RECOGNIZER_KIND == 2 */

    do {
      first_item_q = (index_set_q = T2_index_sets [item_q]) == NULL;

      if (Y == 0 && !first_item_q) {
	/* Pour connaitre les changements d'index_set sur les reduce */
	cur_index_set_q = working_index_set;
	*cur_index_set_q = *index_set_q; /* On force la meme longueur */
	sxba_empty (cur_index_set_q);
      }
      else
	cur_index_set_q = NULL;

#if is_rcvr
      if (is_during_reduce_rcvr_validation) {
	/* La copie est faite ds une dummy table */
	if (first_item_q)
	  cur_index_set_q = index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
	else {
	  if (Y != 0)
	    cur_index_set_q = index_set_q;
	}

	has_cur_indexes = SXTRUE;

	if (index_set_p == NULL)
	  SXBA_1_bit (cur_index_set_q, p);
	else
	  OR (cur_index_set_q, index_set_p);
      }
      else
#endif /* is_rcvr */
	{
	  has_cur_indexes = SXFALSE;
#if RECOGNIZER_KIND == 2
	  if (is_on_a_terminal_transition_for_the_first_time)
	    local_pq_t_logprob = pq_t2logprob (pq, t);

	  if ((recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
	      && is_on_a_terminal_transition_for_the_first_time) {
	    /* première phase du beam sur les transitions terminales: on va regarder que la meilleure desdites transitions n'est pas en dessous du seuil minimum. Si c'est le cas, on abandonne tout de suite sans rentrer dans le détail des différents -t-> */ 
	    tmp_RT_logprob_ptr =  &(RT_global_logprobs_p->RT_logprobs [offset]);
	    local_logprob = tmp_RT_logprob_ptr->max_global_logprob + local_pq_t_logprob;
	    
	    if (cmp_SXLOGPROB(local_logprob, min_admissible_logprob) < 0) {
	      /* le max est inférieur au min admissible: on beame, i.e., arrête d'explorer ce t là */
#if LLOG
	      printf ("Beaming item (item_q=" SXILD ") [" SXLPD " + " SXLPD " = " SXLPD " < " SXLPD "] : ", item_q, SXLOGPROB_display (tmp_RT_logprob_ptr->max_global_logprob), SXLOGPROB_display (local_pq_t_logprob), SXLOGPROB_display (tmp_RT_logprob_ptr->max_global_logprob + local_pq_t_logprob), SXLOGPROB_display (min_admissible_logprob));
	      output_item (item_q, NULL, -1, q);
#endif /* LLOG */
	      if (!earley_table_companion_q->did_scan_beam) { /* premier item à être beamé */
		earley_table_companion_q->did_scan_beam = SXTRUE;
		earley_table_companion_q->dynamic_scan_beam_first_beamed_logprob = local_logprob;
	      }
	      else
		if (cmp_SXLOGPROB (earley_table_companion_q->dynamic_scan_beam_first_beamed_logprob, local_logprob) < 0)
		  earley_table_companion_q->dynamic_scan_beam_first_beamed_logprob = local_logprob;
	      break;
	    }
#if LLOG
	    else {
	      printf ("Not beaming item (item_q=" SXILD ") [" SXLPD " + " SXLPD " = " SXLPD " >= " SXLPD "] : ", item_q, SXLOGPROB_display (tmp_RT_logprob_ptr->max_global_logprob), SXLOGPROB_display (local_pq_t_logprob), SXLOGPROB_display (local_logprob), SXLOGPROB_display (min_admissible_logprob));
	      output_item (item_q, NULL, -1, q);
	    }
#endif /* LLOG */
	  }
#endif /* RECOGNIZER_KIND == 2 */

	  if (index_set_p == NULL) {
	    /* le max est bon, sinon on aurait breaké ci-dessus. Or le max est ici le seul puisqu'il n'y a qu'un seul p! Donc il est bon */
	    if (set_forward_logprob (p, p, q, item_p, item_q, X, &pq, &inside_logprob)) {
	      if (first_item_q)
		cur_index_set_q = index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
	      else {
		if (Y != 0)
		  cur_index_set_q = index_set_q;
	      }
		
	      has_cur_indexes = SXTRUE;
	      SXBA_1_bit (cur_index_set_q, p);
	    }
	  }
	  else {
	    i = -1;
#if RECOGNIZER_KIND == 2
	    if ((recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
		&& is_on_a_terminal_transition_for_the_first_time) {
	      ioffset = 0;
	      tmp_offset_in_global_logprobs = tmp_RT_logprob_ptr->offset_in_global_logprobs;
	    }
#endif /* RECOGNIZER_KIND == 2 */
	    while ((i = sxba_scan (index_set_p, i)) >= 0) {
#if RECOGNIZER_KIND == 2
	      if ((recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
		  && is_on_a_terminal_transition_for_the_first_time) {
		SXLOGPROB local_logprob = RT_global_logprobs_p->global_logprobs [tmp_offset_in_global_logprobs + ioffset++] + local_pq_t_logprob;

		if (cmp_SXLOGPROB(local_logprob, min_admissible_logprob) < 0) {
		  /* le max est inférieur au min admissible: on beame, i.e., arrête d'explorer ce t là lorsqu'on vient du i courant */
#if LLOG
		  printf ("Beaming item (item_q=" SXILD ", i=" SXILD ") [" SXLPD " < " SXLPD "] : ", item_q, i, SXLOGPROB_display (local_logprob), SXLOGPROB_display (min_admissible_logprob));
		  output_item (item_q, NULL, i, q);
#endif /* LLOG */

		  if (!earley_table_companion_q->did_scan_beam) { /* premier item à être beamé */
		    earley_table_companion_q->did_scan_beam = SXTRUE;
		    earley_table_companion_q->dynamic_scan_beam_first_beamed_logprob = local_logprob;
		  }
		  else
		    if (cmp_SXLOGPROB (earley_table_companion_q->dynamic_scan_beam_first_beamed_logprob, local_logprob) < 0)
		      earley_table_companion_q->dynamic_scan_beam_first_beamed_logprob = local_logprob;

		  continue;
		}
#if LLOG
		else {
		  printf ("Not beaming item (item_q=" SXILD ", i=" SXILD ") [" SXLPD " >= " SXLPD "] : ", item_q, i, SXLOGPROB_display (local_logprob), SXLOGPROB_display (min_admissible_logprob));
		  output_item (item_q, NULL, -1, q);
		}
#endif /* LLOG */
	      }
#endif /* RECOGNIZER_KIND == 2 */
	      if (set_forward_logprob (i, p, q, item_p, item_q, X, &pq, &inside_logprob)) {
		if (!has_cur_indexes) {
		  /* 1ere fois qu'un i marche */
		  if (first_item_q)
		    cur_index_set_q = index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
		  else {
		    if (Y != 0)
		      cur_index_set_q = index_set_q;
		  }
		  
		  has_cur_indexes = SXTRUE;
		}
		
		SXBA_1_bit (cur_index_set_q, i);
	      }
	    }
	  }
	}

#if is_parser
      if (has_cur_indexes) {
	/* Pas de backward apres un terminal... */
	/* ... sauf ds le cas is_dag */
	/* ... ou is_rcvr (le 05/12/05) */
	/* Ajoute le 07/07/2003 */
#if is_rcvr
	/* Le 27/01/11 */
	if (!is_during_reduce_rcvr_validation)
#endif /* is_rcvr */
	  {
	    backward_index_set_q = T2_backward_index_sets [item_q];

	    if (backward_index_set_q == NULL)
	      backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	    SXBA_1_bit (backward_index_set_q, new_p);
	  }
      }
#endif /* is_parser */

      /* A cause des points de reprise de la rcvr il se peut que index_set_q soit initialement non vide
	 mais que les NT_hd le soient */
      if (has_cur_indexes) {
	ret_val = SXTRUE; /* Le 22/01/11 */

#if is_rcvr
	/* Le 11/10/06 */
	/* meme si ce n'est pas un bon item, on le note car on pourra tenter de la rcvr depuis la` */
	/* Ne pas le faire si on est en train de tester si une correction par reduce marche */
	if (!is_during_reduce_rcvr_validation)
	  {
	    /* point de reprise potentiel */
	    /* Le contexte gauche doit etre beam-proof */
	    if (Y == 0)
	      SXBA_1_bit (rcvr_reduce_item_sets [q], item_q);
	    else
	      SXBA_1_bit (rcvr_kernel_item_sets [q], item_q);
	  }
#endif /* is_rcvr */

	if (Y < 0) {
	  /* Verification du scan */
	  YY = -Y;

	  if (SXBA_bit_is_set (T2_look_ahead_t_set, YY)) {
	    /* YY est un bon scan */
	    YY = 0;
#if is_rcvr
	    has_hit_a_valid_kernel_terminal_item = SXTRUE;
#endif /* is_rcvr */

#if !is_parser && is_supertagging
	    SXBA_1_bit (item2source_index_set [item_q], q);
#if LOG
	    printf ("%s [" SXILD ", " SXILD "]\n", sxttext (sxplocals.sxtables, -Y), PROLIS (item_q), q);
#endif /* LOG */
#endif /* !is_parser && is_supertagging */
	  }
	  else {
	    /* echec du scan */
#if LLOG
	    fputs ("   future scan will prove impossible. Discarding item\n", stdout);
#endif /* LLOG */
	    break;
	  }
	}
	else {
	  if (Y == 0) {
	    prod = PROLIS (item_q);
	    B = LHS (prod);
	  }
	  else {
#if is_1LA
	    if (T2_look_ahead_item_set && !SXBA_bit_is_set (T2_look_ahead_item_set, item_q)) {
	      /* Echec du test du look-ahead */
	      break;
	    }
#endif /* is_1LA */

	    YY = Y;
	  }
	}

	if (first_item_q) {
	  /* C'est la 1ere fois qu'on voit item_q ds la table T [q] */
	  if (Y != 0) {
	    /* item_q n'est pas reduce */
	    if (*(ptr2 = &(T2_shift_NT_hd [YY])) == NULL) {
	      /* c'est la 1ere fois qu'on rencontre YY */
	      /* rhs_nt2where [0] == 0 (pour les terminaux) */
	      *ptr2 = &(T2_items_stack [RHS_NT2WHERE (YY)]);

	      if (Y > 0) {
		if (!SXBA_bit_is_set (T2_left_corner, Y)) {
		  /* 1ere fois qu'on regarde le left_corner de Y */
		  sxba_or (T2_left_corner, LEFT_CORNER (Y));
		  /* Le 05/10/05 PB:
		     Supposons Y>0 et EMPTY_SET(Y), T2_non_kernel_item_set va contenir nt2item_set [Y] et
		     supposons que ces items soient supprimes (filtrage) car, par exemple, le look-ahead
		     n'est pas bon.  La reduction "Y = ;" ne sera donc pas generee.  Ds le cas normal, c'est
		     pas grave car la mise en table des items issus de item_q finira par s'arreter. Sauf ds
		     le cas ou un rattrapage d'erreurs validera item_q ou item_q+1 ou ... Ds ce cas
		     on va donc se retrouver avec la production prolis[item_q] generee sans que
		     la/les Y-productions ne le soient !!
		     On essaie de faire qqchose ds process_non_kernel
		  */
		  sxba_or (T2_non_kernel_item_set, NT2ITEM_SET (Y));
		}
	      }
	    }
	  }
	  else {
	    /* item_q est reduce */
	    if (*(ptr2 = &(T2_reduce_NT_hd [B])) == NULL) {
	      /* c'est la 1ere fois qu'on fait un reduce vers B */
	      *ptr2 = &(T2_items_stack [LHS_NT2WHERE (B)]);
	    }
	  }

	  *((*ptr2)++) = item_q;
	}
      }

      YY = 0; /* Y== 0 ou ne derive pas ds le vide a priori */

      if (Y > 0 && has_cur_indexes) {
#if is_epsilon
	if (
#ifdef MAKE_INSIDEG
	    spf.insideG.bvide &&
#endif /* MAKE_INSIDEG */
	    SXBA_bit_is_set (EMPTY_SET, Y)) {
	  YY = Y; /* Y =>+ epsilon */

#if is_prdct
	  if (is_prdct_fun &&
	      (prdct_no = prdct [item_q]) != -1 &&
	      !(*for_semact.prdct) (1 /* No de passe : reconnaisseur */, item_q, q, q, prdct_no)) {
	    /* Le predicat a echoue */
	    break;
	  }
#endif /* is_prdct */

	  /* specifique au traitement des logprobs */
#if is_rcvr
	  if (!is_during_reduce_rcvr_validation)
	    /* La correction est lancee par un reduce qui a deja ete valide'
	       l'inside_logprob est calcule'e normalement (ce n'est pas nt2max_inside_logprob [Y]) */
#endif /* is_rcvr */
	    {
	      if (!Axpq2set_max_logprob_ptr (Y, p_q2pq (q, q))) {
		/* 1ere fois qu'on rencontre Yqq */
		max_logprob_ptr->inside = nullable_nt2max_nullable_logprob [Y];
#if LLOG
		output_Apq_logprobs ("New ", Y, q, q, max_logprob_ptr);
#endif /* LLOG */
	      }

	      inside_logprob += max_logprob_ptr->inside;
	    }

	  item_q++;
	  X = Y;
	  Y = LISPRO (item_q);

#if is_parser
	  new_p = q;
#endif /* is_parser */
	}
#endif /* is_epsilon */
      }
      
#if RECOGNIZER_KIND == 2
      is_on_a_terminal_transition_for_the_first_time = SXFALSE;
#endif /* RECOGNIZER_KIND == 2 */

    } while (YY != 0);

    if (Y == 0 && has_cur_indexes) {
      /* item_q est un item reduce ... */
      /* ... beam_proof */
      /* item_q = B -> \alpha A \beta .  et \beta =>* \epsilon */

      if (!first_item_q) {
	/* On y met les nouveaux i */
	sxba_or (index_set_q, cur_index_set_q);
	/* cur_index_set_q est non vide */
      }

#if MEASURES || LOG
      SXBA_1_bit (used_prod_set, prod);
#endif /* MEASURES || LOG */

#if is_parser
#if is_constraints
      if (is_constraint_fun && (prdct_no = constraints [prod]) != -1) {
	if (index_set_p == NULL) {
	  if (!(*for_semact.constraint) (1 /* No de passe : reconnaisseur */, prdct_no, prod, NULL, p, q))
	    continue;
	}
	else {
	  /* Il semble qu'on peut directement modifier index_set_p!! */
	  if (!(*for_semact.constraint) (1 /* No de passe : reconnaisseur */, prdct_no, prod, index_set_p, p, q))
	    continue;
	  /* ici index_set_p a pu etre touche */
	}
      }
#endif /* is_constraints */

#if is_supertagging
      if (i = sxba_scan (cur_index_set_q, 0) >= 0)
	supertag (prod, i, q);
#endif /* is_supertagging */

#if is_guide
      /* Le guide peut etre produit soit par le reconnaisseur (ici) soit par RL_mr */
      /* On utilise index_set_p (et non index_set_q) ca peut eviter certains appels (prod, i, j) deja
	 effectues lors d'un appel precedent de shift() */
      /* Le 13/01/06 on peut reutiliser index_set_q */
      output_guide (prod, cur_index_set_q, 0, q);
#if LOG
      output_ni_prod (prod, index_set_q, 0, q);
#endif /* LOG */
#endif /* is_guide */
#endif /* is_parser */

#if LOG && (is_parser || !is_guide)
      /* le cas is_guide est traite' ci dessus */
      /* On sort tous les index_set, pas que les nouveaux */
      output_ni_prod (prod, index_set_q, 0, q);
#endif /* LOG && (is_parser || !is_guide) */
      
      /* dans tous les cas on fait une reduction vers B */
      if (SXBA_bit_is_reset_set (reduce_set, B))
	PUSH (reduce_list, B); /* nouvelle */

#if is_rcvr
      if (is_during_reduce_rcvr_validation) {
	/* On ne tient pas compte des logprobs */
	index_set_B = ntXindex_set [B];

	if (index_set_p == NULL) {
	  /* item_q = B -> A .  */
	  SXBA_1_bit (index_set_B, p); /* peut deja etre positionné */

	  if (SXBA_bit_is_reset_set (Bpq_set, B))
	    PUSH (Bpq_list, B); /* DPUSH est inutile ds ce cas!! */
	}
	else {
	  /* item_q = B -> \alpha A \beta .
	     et \beta =>* \epsilon
	     et \alpha \not \equal \epsilon */
	  if (SXBA_bit_is_set (cur_index_set_q, p)) { /* \alpha derive ds le vide... */
	    if (SXBA_bit_is_reset_set (Bpq_set, B)) /* ... et Bpq est nouveau */
	      PUSH (Bpq_list, B); /* DPUSH est inutile ds ce cas!! */
	  }

	  OR (index_set_B, cur_index_set_q);
	}
      }
      else
#endif /* is_rcvr */
	{
	  if (index_set_p == NULL) {
	    /* item_q = B -> A .  */
	    SXBOOLEAN has_changed = fill_Axpq2max_logprob_ptr (item_q, B, p, q);
	    /* fill_Axpq2max_logprob_ptr a fait SXBA_1_bit (ntXindex_set [B], p) */
	    
	    if (SXBA_bit_is_reset_set (Bpq_set, B) /* 1er reduce vers Bpq */
		|| (has_changed /* 1er reduce vers Bpq ou meilleure proba */
		    && !SXBA_bit_is_set (Bpq_list_set, B) /* Bpq n'est pas ds Bpq_list */)) {
	      DPUSH (Bpq_list, B);
	      SXBA_1_bit (Bpq_list_set, B);
	    }
	  }
	  else {
	    /* item_q = B -> \alpha A \beta .
	       et \beta =>* \epsilon
	       et \alpha \not =>* \epsilon */
	    SXBOOLEAN has_changed;

	    i = -1;

	    /* OR (ntXindex_set [B], index_set_p); */
	    while ((i = sxba_scan (cur_index_set_q, i)) >= 0) {
	      if (i == p) {
		has_changed = fill_Axpq2max_logprob_ptr (item_q, B, i, q);
	    
		if (SXBA_bit_is_reset_set (Bpq_set, B) /* 1er reduce vers Bpq */
		    || (has_changed /* 1er reduce vers Bpq ou meilleure proba */
			&& !SXBA_bit_is_set (Bpq_list_set, B) /* Bpq n'est pas ds Bpq_list */)) {
		  DPUSH (Bpq_list, B);
		  SXBA_1_bit (Bpq_list_set, B);
		}
	      }
	      else
		fill_Axpq2max_logprob_ptr (item_q, B, i, q);
	    }
	  }
	}
    }
  } /* while (--top_ptr >= bot_ptr) */

#if LLOG
  printf ("*** Leaving beam_shift (A = %s (" SXILD "), p = " SXILD ", q = " SXILD ") %s\n\n", A > 0 ? NTSTRING (A) : "", A, p, q,
	  ret_val ? "SXTRUE" : "SXFALSE");
#endif /* LLOG */

  return ret_val;
}
#endif /* HAS_BEAM == 1 */

/* Ds le cas is_dag, p et q sont des milestones (et pas des etats du dag) */
/* dans tous les cas on a donc p <= q */
/* Changement le 17/07/2003
   Arg supplementaire qui indique s'il y a eu un reduce
   Retourne vrai ssi la table Tq a change' */
/* Le 21/11/06  Retourne vrai ssi il y a depuis p une transition sur A */
static SXBOOLEAN
shift (SXINT A, SXINT p, SXINT q)
{
  /*
    Realise la transition Apq
    p < q
    Si A == 0 => transition terminale
  */
  SXINT			item_p, item_q;
  SXINT			**ptr2;
  SXINT 		X, B, Y, YY, prod, new_p, prdct_no, order, i, t;
  SXBA			index_set_p, index_set_q, cur_index_set_q, backward_index_set_q, index_set_B, t_set;
  struct recognize_item	*RTp;
  SXINT			*top_ptr, *bot_ptr;
  SXBOOLEAN		ret_val, first_item_q;

#if LLOG
  printf ("*** Entering shift (A = %s (" SXILD "), p = " SXILD ", q = " SXILD ")\n", A > 0 ? NTSTRING (A) : "", A, p, q);
#endif /* LLOG */

  ret_val = SXFALSE;

  RTp = &(RT [p]);

  top_ptr = RTp->shift_NT_hd [A];
    
#if is_lex
  if (top_ptr == NULL && A > 0) {
    /* On peut se retrouver ici si l'on a fait une lexicalisation mais qu'on ne construit pas de grammaire inside. En
       effet, on peut avoir rajouté dans une table un item sur la seule foi des item_sets (de la grammaire initiale),
       alors que cet item ne pouvait être atteint depuis les symboles de partie droite des items de ladite table. Et du
       coup, on peut se retrouver à réduire l'un de ces mauvais items, et à chercher quel autre item avait bien pu
       "prédire" ledit mauvais item... et ne rien trouver! */
#if LLOG
    printf ("*** Leaving shift (A = %s (" SXILD "), p = " SXILD ", q = " SXILD ") SXFALSE (lex)\n\n", A > 0 ? NTSTRING (A) : "", A, p, q);
#endif /* LLOG */
    return SXFALSE;
  }
#endif /* is_lex */

#if EBUG
  if (top_ptr == NULL)
    sxtrap (ME, "shift");
#endif /* EBUG */

  bot_ptr = &(RTp->items_stack [0]);

  if (A > 0)
    bot_ptr += RHS_NT2WHERE (A);
#if EBUG && is_rcvr
  else {
    if (is_during_error_rcvr_processing)
      /* Pas de scan durant la rcvr */
      sxtrap (ME, "shift");
  }
#endif /* EBUG && is_rcvr */

  if (A == 0) {
    t_set = idag.pq2t_set [p_q2pq (p, q)];
  }
  
  while (--top_ptr >= bot_ptr) {
    ret_val = SXTRUE; /* Le 21/11/06 */
    item_p = *top_ptr;

    if (A == 0) {
      /* transition terminale, il faut verifier qu'elle conduit bien a q... */
      t = -LISPRO (item_p);

#ifdef MAKE_INSIDEG
      t = spf.insideG.t2init_t [t];
#endif /* MAKE_INSIDEG */

      if (!SXBA_bit_is_set (t_set, t))
	/* ... non */
	continue;
    }

    /* item_p = B -> \beta .X Y \gamma */
    item_q = item_p+1;
    /* item_q = B -> \beta X .Y \gamma */
    Y = LISPRO (item_q);

    /* On verifie le predicat de item_p */
#if is_prdct
    if (is_prdct_fun &&
	(prdct_no = prdct [item_p]) >= 0 &&
	!(*for_semact.prdct) (1 /* No de passe : reconnaisseur */, item_p, p, q, prdct_no))
      /* Echec du predicat */
      continue;
#endif /* is_prdct */

    X = A;
    
#if EBUG
    if (A > 0 && A != LISPRO (item_p))
      sxtrap (ME, "shift");
#endif /* EBUG */

    index_set_p = RTp->index_sets [item_p];

#if LLLOG
    fputs ("Examining ", stdout);
    output_item (item_p, index_set_p, p, p);
#endif /* LLLOG */

#if is_parser
    new_p = p;
#endif /* is_parser */

    do {
      first_item_q = (index_set_q = T2_index_sets [item_q]) == NULL;

      if (first_item_q)
	cur_index_set_q = index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
      else {
	if (Y == 0) {
	  /* Pour connaitre les changements d'index_set sur les reduce */
	  cur_index_set_q = working_index_set;
	  *cur_index_set_q = *index_set_q; /* On force la meme longueur */
	  sxba_empty (cur_index_set_q);
	}
	else
	  cur_index_set_q = index_set_q;
      }

      /* Dans tous les cas on remplit index_set_p */
      /* cas normal */
      /* meme si ce n'est pas un bon terminal, on le note car on pourra tenter de la rcvr depuis la` */
      /* pendant la rcvr
	 si is_during_reduce_rcvr_validation La copie est faite ds une dummy table 
	 sinon il faut le faire */
      if (index_set_p == NULL)
	SXBA_1_bit (cur_index_set_q, p);
      else
	OR (cur_index_set_q, index_set_p);


      /* A cause des points de reprise de la rcvr il se peut que index_set_q soit initialement non vide
	 mais que les NT_hd le soient */
#if is_rcvr
      /* Le 27/01/11 */
      if (!is_during_reduce_rcvr_validation)
#endif /* is_rcvr */
	{
#if is_rcvr
	  /* point de reprise potentiel */
	  /* Le contexte gauche doit etre beam-proof */
	  if (Y == 0)
	    SXBA_1_bit (rcvr_reduce_item_sets [q], item_q);
	  else
	    SXBA_1_bit (rcvr_kernel_item_sets [q], item_q);
#endif /* is_rcvr */

#if is_parser
	  /* Pas de backward apres un terminal... */
	  /* ... sauf ds le cas is_dag */
	  /* ... ou is_rcvr (le 05/12/05) */
	  /* Ajoute le 07/07/2003 */
	  backward_index_set_q = T2_backward_index_sets [item_q];

	  if (backward_index_set_q == NULL)
	    backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	  SXBA_1_bit (backward_index_set_q, new_p);
#endif /* is_parser */
	}

      if (Y < 0) {
	/* Verification du scan */
	YY = -Y;

	if (SXBA_bit_is_set (T2_look_ahead_t_set, YY)) {
	  /* YY est un bon scan */
	  YY = 0;
#if is_rcvr
	  has_hit_a_valid_kernel_terminal_item = SXTRUE;
#endif /* is_rcvr */

#if !is_parser && is_supertagging
	  SXBA_1_bit (item2source_index_set [item_q], q);
#if LOG
	  printf ("%s [" SXILD ", " SXILD "]\n", sxttext (sxplocals.sxtables, -Y), PROLIS (item_q), q);
#endif /* LOG */
#endif /* !is_parser && is_supertagging */
	}
	else {
	  /* echec du scan */
	  break;
	}
      }
      else {
	if (Y == 0) {
	  prod = PROLIS (item_q);
	  B = LHS (prod);
	}
	else {
#if is_1LA
	  if (T2_look_ahead_item_set && !SXBA_bit_is_set (T2_look_ahead_item_set, item_q)) {
	    /* Echec du test du look-ahead */
	    break;
	  }
#endif /* is_1LA */

	  YY = Y;
	}
      }

      if (first_item_q) {
	/* C'est la 1ere fois qu'on voit item_q ds la table T [q] */
	if (Y != 0) {
	  /* item_q n'est pas reduce */
	  if (*(ptr2 = &(T2_shift_NT_hd [YY])) == NULL) {
	    /* c'est la 1ere fois qu'on rencontre YY */
	    /* rhs_nt2where [0] == 0 (pour les terminaux) */
	    *ptr2 = &(T2_items_stack [RHS_NT2WHERE (YY)]);

	    if (Y > 0) {
	      if (!SXBA_bit_is_set (T2_left_corner, Y)) {
		/* 1ere fois qu'on regarde le left_corner de Y */
		sxba_or (T2_left_corner, LEFT_CORNER (Y));
		/* Le 05/10/05 PB:
		   Supposons Y>0 et EMPTY_SET(Y), T2_non_kernel_item_set va contenir nt2item_set [Y] et
		   supposons que ces items soient supprimes (filtrage) car, par exemple, le look-ahead
		   n'est pas bon.  La reduction "Y = ;" ne sera donc pas generee.  Ds le cas normal, c'est
		   pas grave car la mise en table des items issus de item_q finira par s'arreter. Sauf ds
		   le cas ou un rattrapage d'erreurs validera item_q ou item_q+1 ou ... Ds ce cas
		   on va donc se retrouver avec la production prolis[item_q] generee sans que
		   la/les Y-productions ne le soient !!
		   On essaie de faire qqchose ds process_non_kernel
		*/
		sxba_or (T2_non_kernel_item_set, NT2ITEM_SET (Y));
	      }
	    }
	  }
	}
	else {
	  /* item_q est reduce */
	  if (*(ptr2 = &(T2_reduce_NT_hd [B])) == NULL) {
	    /* c'est la 1ere fois qu'on fait un reduce vers B */
	    *ptr2 = &(T2_items_stack [LHS_NT2WHERE (B)]);
	  }
	}

	*((*ptr2)++) = item_q;
      }

      YY = 0; /* Y== 0 ou ne derive pas ds le vide a priori */

      if (Y > 0) {
#if is_epsilon
	if (
#ifdef MAKE_INSIDEG
	    spf.insideG.bvide &&
#endif /* MAKE_INSIDEG */
	    SXBA_bit_is_set (EMPTY_SET, Y)) {
	  YY = Y; /* Y =>+ epsilon */

#if is_prdct
	  if (is_prdct_fun &&
	      (prdct_no = prdct [item_q]) != -1 &&
	      !(*for_semact.prdct) (1 /* No de passe : reconnaisseur */, item_q, q, q, prdct_no)) {
	    /* Le predicat a echoue */
	    break;
	  }
#endif /* is_prdct */

	  item_q++;
	  X = Y;
	  Y = LISPRO (item_q);

#if is_parser
	  new_p = q;
#endif /* is_parser */
	}
#endif /* is_epsilon */
      }
    } while (YY != 0);

    if (Y == 0) {
      /* item_q est un item reduce ... */
      /* ... beam_proof */
      /* item_q = B -> \alpha A \beta .  et \beta =>* \epsilon */

      if (!first_item_q) {
	/* On y met les nouveaux i */
	sxba_or (index_set_q, cur_index_set_q);
	/* cur_index_set_q est non vide */
      }

#if MEASURES || LOG
      SXBA_1_bit (used_prod_set, prod);
#endif /* MEASURES || LOG */

#if is_parser
#if is_constraints
      if (is_constraint_fun && (prdct_no = constraints [prod]) != -1) {
	if (index_set_p == NULL) {
	  if (!(*for_semact.constraint) (1 /* No de passe : reconnaisseur */, prdct_no, prod, NULL, p, q))
	    continue;
	}
	else {
	  /* Il semble qu'on peut directement modifier index_set_p!! */
	  if (!(*for_semact.constraint) (1 /* No de passe : reconnaisseur */, prdct_no, prod, index_set_p, p, q))
	    continue;
	  /* ici index_set_p a pu etre touche */
	}
      }
#endif /* is_constraints */

#if is_supertagging
      if (i = sxba_scan (cur_index_set_q, 0) >= 0)
	supertag (prod, i, q);
#endif /* is_supertagging */

#if is_guide
      /* Le guide peut etre produit soit par le reconnaisseur (ici) soit par RL_mr */
      /* On utilise index_set_p (et non index_set_q) ca peut eviter certains appels (prod, i, j) deja
	 effectues lors d'un appel precedent de shift() */
      /* Le 13/01/06 on peut reutiliser index_set_q */
      output_guide (prod, cur_index_set_q, 0, q);
#if LOG
      output_ni_prod (prod, index_set_q, 0, q);
#endif /* LOG */
#endif /* is_guide */
#endif /* is_parser */

#if LOG && (is_parser || !is_guide)
      /* le cas is_guide est traite' ci dessus */
      /* On sort tous les index_set, pas que les nouveaux */
      output_ni_prod (prod, index_set_q, 0, q);
#endif /* LOG && (is_parser || !is_guide) */
      
      /* dans tous les cas on fait une reduction vers B */
      if (SXBA_bit_is_reset_set (reduce_set, B))
	PUSH (reduce_list, B); /* nouvelle */

      index_set_B = ntXindex_set [B];

      if (index_set_p == NULL) {
	/* item_q = B -> A .  */
	SXBA_1_bit (index_set_B, p); /* peut deja etre positionné */

	if (SXBA_bit_is_reset_set (Bpq_set, B))
	  PUSH (Bpq_list, B);
      }
      else {
	/* item_q = B -> \alpha A \beta .
	   et \beta =>* \epsilon
	   et \alpha \not \equal \epsilon */
	if (SXBA_bit_is_set (cur_index_set_q, p)) { /* \alpha derive ds le vide... */
	  if (SXBA_bit_is_reset_set (Bpq_set, B)) /* ... et Bpq est nouveau */
	    PUSH (Bpq_list, B);
	}

	OR (index_set_B, cur_index_set_q);
      }
    }
  } /* while (--top_ptr >= bot_ptr) */

#if LLOG
  printf ("*** Leaving shift (A = %s (" SXILD "), p = " SXILD ", q = " SXILD ") %s\n\n", A > 0 ? NTSTRING (A) : "", A, p, q,
	  ret_val ? "SXTRUE" : "SXFALSE");
#endif /* LLOG */

  return ret_val;
}



static void
scan_reduce (SXINT p, SXINT q)
{
  SXINT	      j, next_j, k, A, x, top;
  SXBA        index_set_A;
  /* ntXindex_set du niveau j sert de témoin, il ne peut donc etre remis a zéro qu'en fin de procédure */
  /* reduce_(set/list) contiennent l'(ensemble/liste) de tous les lhs nt impliqués ds un reduce depuis q */
  j = p;

  while (j >= 0) {
    /* reduce_list et reduce_set contiennent la liste complete des lhs nt.  Elle doit etre maj par shift ()
       car elle sert au raz de ntXindex_set pour chaque j en fin du while */
    if (j < p) {
      /* Si j == p, (appel initial) les Bpq... sont déjà positionnés */
      top = TOP (reduce_list);

      for (x = 1; x <= top; x++) {
	A = reduce_list [x];

	if (SXBA_bit_is_set (ntXindex_set [A], j)) {
	  /* ... oui pour Ajq */
	  PUSH (Bpq_list, A); /* initialisation, DPUSH est inutile ds le cas beam */
	  SXBA_1_bit (Bpq_set, A); /* on a deja mis Apq ds Bpq_list */

#if HAS_BEAM == 1
	  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set)
	    SXBA_1_bit (Bpq_list_set, A); /* Apq est ds la partie de Bpq_list non encore traitee */
#endif /* HAS_BEAM == 1 */
	}
      }
    }

    /* Ici, on traite les "shift" sur les lhs des reductions */
    for (x = 1; x <= TOP (Bpq_list) /* TOP et DTOP sont identiques */; x++) {
      /* Les shift () peuvent ajouter des nt aux Bpq */
      A = Bpq_list [x];

#if HAS_BEAM == 1
      if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set)
	SXBA_0_bit (Bpq_list_set, A); /* Apq est enlevé de la partie qui reste à traiter */
#endif /* HAS_BEAM == 1 */
	  
#if EBUG
      if (!SXBA_bit_is_set (ntXindex_set [A], j))
	sxtrap (ME, "scan_reduce");
#endif /* EBUG */
  
      /* ... oui pour Ajq */
#if HAS_BEAM == 1
	if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set)
	  beam_shift (A, j, q);
	else
#endif /* HAS_BEAM == 1 */
	  shift (A, j, q);

	/* Les appels de shift () ont pu mettre de nouveaux éléments ds Ppq_(set/stack) */
    }

    /* Ici le traitement du niveau j est terminé */ 
#if EBUG
#if HAS_BEAM == 1
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set)
      if (!sxba_is_empty (Bpq_list_set))
	sxtrap (ME, "scan_reduce");
#endif /* HAS_BEAM == 1 */
#endif /* EBUG */

    next_j = -1;
    top = TOP (reduce_list);
    RAZ (reduce_list); /* on la refabrique ds la boucle ci-dessous */
    RAZ (Bpq_list); /* RAZ et DRAZ sont identiques */

    for (x = 1; x <= top; x++) {
      A = reduce_list [x];
      SXBA_0_bit (Bpq_set, A); /* peut etre inutile */
      index_set_A = ntXindex_set [A];
      SXBA_0_bit (index_set_A, j); /* Parfois inutile */

      if ((k = sxba_1_rlscan (index_set_A, j)) < 0) {
	/* Plus d'indices pour A, on le vire */
	SXBA_0_bit (reduce_set, A);
      }
      else {
	/* Il existe des Akq avec k < j */
	PUSH (reduce_list, A);
	
	if (k > next_j)
	  next_j = k; /* Le + grand */
      }
    }

    j = next_j;
    /* Ici les Bpq sont vides, les reduce ne contiennent que des nt de bi <= j et les ntXindex_set de ces
       nt ne contiennent que des index <= j */
  }
	  
#if EBUG
  if (!sxba_is_empty (reduce_set) || TOP (reduce_list) != 0)
	sxtrap (ME, "scan_reduce");
#endif /* EBUG */
}


/* Le 22/09/06 Nelle version.  Sauf si no_la_check, T2_look_ahead_item_set
   est positionne' (meme si is_error_detected) car, apres de la rcvr
   on continue a traiter le (vrai) source !! */
/* q est un mlstn, positionne les variables "T2" qui referencent les tables q */
/* et Positionne ilex_compatible_item_set et T2_look_ahead_item_set */
static void
set_T2 (SXINT q)
{
  SXINT  p, x;
  SXBA t_set;
  struct recognize_item	*RTq;

  RTq = &(RT [q]);
  T2_non_kernel_item_set = non_kernel_item_sets [q];
  T2_shift_NT_hd = &(RTq->shift_NT_hd [0]);
  T2_items_stack = &(RTq->items_stack [0]);
  T2_index_sets = &(RTq->index_sets [0]);

  T2_left_corner = RT_left_corner [q];

  T2_look_ahead_t_set = mlstn2look_ahead_t_set [q];

#if HAS_BEAM == 1
  if (kernel_nt_sets) T2_kernel_nt_set = kernel_nt_sets [q];
#endif /* HAS_BEAM == 1 */

#if is_rcvr
  T2_store_non_kernel_item_set = store_non_kernel_item_sets [q];
#endif /* is_rcvr */

#if is_parser
  T2_backward_index_sets = &(PT [q].backward_index_sets [0]);
  T2_reduce_NT_hd = &(PT [q].reduce_NT_hd [0]);
#endif /* is_parser */

#if is_1la
  T2_look_ahead_item_set = mlstn2look_ahead_item_set [q];
#endif /* is_1la */

#if is_rcvr
  /* Correctifs a apporter en cas d'erreur */
  /* Le 08/07/05 */
  /* PB : 
     Supposons que le source est (cas reel) soit a =" _Uw - ;" soit b ="( _Uw | _uw) - ;"
     et qu'avec (a) dynamic_set_automaton () echoue mais marche avec (b)
     Dans le cas (b) la rcvr Earley sera donc bridee par le resultat de dynamic_set_automaton () et pourra donc rendre
     des resultats differents (et moins bon) que dans le cas (a) alors que le source de (b) est + general que celui de (a)
     (il le contient).
     Si une erreur est detectee on n'utilise plus le guidage
  */
  if (is_error_detected) {
    /* plus aucun filtrage apres detection d'une erreur */
    ilex_compatible_item_set = NULL;
  }

  if (no_la_check) {
    /* On a demande' l'inhibition du test du look-ahead ds process_non_kernel */
    T2_look_ahead_t_set = NULL;
  }
#endif /* is_rcvr */
}



/* On filtre T2_non_kernel_item_set */
static void
process_non_kernel_filtering (SXINT q)
{
  /* Le 05/10/05 (voir commentaire ds shift sur la fabrication de T2_non_kernel_item_set) */
  /* On genere les productions vides, meme si le look-ahead n'est pas correct */
  /* Le 07/11/05 on extrait les productions vides avant le guide car la rcvr peut "sortir" du guide */

#if is_rcvr
  if (ilex_compatible_item_set) {
    /* Il va y avoir du filtrage, on se souvient de la valeur de T2_non_kernel_item_set
       avant filtrage */
    sxba_copy (rcvr_store_unfiltered_non_kernel_item_set, T2_non_kernel_item_set);
  }
#endif /* is_rcvr */

#if is_epsilon && is_rcvr
  /* extraction des productions vides ... */
#ifdef MAKE_INSIDEG
  if (spf.insideG.is_eps) {
    sxba_copy (non_kernel_empty_item_set, T2_non_kernel_item_set);
    sxba_and (non_kernel_empty_item_set, EMPTY_PROD_ITEM_SET);
  }
#else /* !MAKE_INSIDEG */
  sxba_copy (non_kernel_empty_item_set, T2_non_kernel_item_set);
  sxba_and (non_kernel_empty_item_set, EMPTY_PROD_ITEM_SET);
#endif /* !MAKE_INSIDEG */
#endif /* is_epsilon && is_rcvr */

#if is_lex || has_Lex || is_guide || is_full_guide
  if (ilex_compatible_item_set) /* Peut etre NULL */
    sxba_and (T2_non_kernel_item_set, ilex_compatible_item_set);
#endif /* is_lex || has_Lex || is_guide || is_full_guide */

#if 0
  /* Semble faire double emploi avec ilex_compatible_item_set !! */
  if (T2_look_ahead_item_set) {
    sxba_and (T2_non_kernel_item_set, T2_look_ahead_item_set);
  }
#endif /* 0 */

#if is_epsilon && is_rcvr
  /* ... que l'on remet */
#ifdef MAKE_INSIDEG
  if (spf.insideG.is_eps) {
    sxba_or (T2_non_kernel_item_set, non_kernel_empty_item_set);
  }
#else /* !MAKE_INSIDEG */
  sxba_or (T2_non_kernel_item_set, non_kernel_empty_item_set);
#endif /* !MAKE_INSIDEG */
#endif /* is_epsilon && is_rcvr */

#if is_make_oracle && !is_parser
  sxba_copy (i2guide_item_set [q-1], T2_non_kernel_item_set);
#endif /* is_make_oracle && !is_parser */

#if is_rcvr
  if (is_error_detected)
    /* On ne [re]regarde que les nouveaux */
    sxba_minus (T2_non_kernel_item_set, T2_store_non_kernel_item_set);
#if 0
  /* Le 09/12/05 fait item par item ds process_non_kernel () */
  /* ... pour le coup d'apres */
  sxba_or (T2_store_non_kernel_item_set, T2_non_kernel_item_set);
#endif /* 0 */
#endif /* is_rcvr */
}

/* Le 25/10/06 Nelle version */
static void
process_non_kernel (SXINT q)
{
  /* On prepare la transition depuis q en stockant ds T2 les items non-kernel (scan ou nt_trans). */
  SXINT		**ptr2;
  SXINT		item, A, X, XX, prdct_no, next_item, prod;
  SXBA	        index_set;
  SXBOOLEAN       is_new_item;
#if is_parser
  SXBA	        backward_index_set;
#endif /* is_parser */

  item = -1;
    
  while ((item = sxba_scan_reset (T2_non_kernel_item_set, item)) >= 0) {
    /* item = A -> \alpha . X \beta et \alpha =>* \epsilon et aq \in FIRST1 (A -> \alpha . X \beta) */

#if is_rcvr
    /* Ici item va etre traite' (independamment des contextes d'appels particuliers eventuels)
       on le stocke pour etre su^r de ne pas recommencer la meme chose +tard */
    SXBA_1_bit (T2_store_non_kernel_item_set, item);
#endif /* is_rcvr */

    X = LISPRO (item);

#if is_epsilon
#if EBUG
    if (item == 0)
      sxtrap (ME, "process_non_kernel");
#endif /* EBUG */
    prod = PROLIS (item);
    A = LHS (prod);
#endif /* is_epsilon */

    if (X < 0 && T2_look_ahead_t_set /* test demande' */) {
      XX = -X;

      /* Verification du scan */
      /* Elle est peut etre inutile ? Il semblerait que non !! */
      if (!SXBA_bit_is_set (T2_look_ahead_t_set, XX))
	continue;
    }

    if (X < 0) {
      /* X == a_q */
#if is_prdct
      if (is_prdct_fun &&
	  (prdct_no = prdct [item]) != -1 &&
	  !(*for_semact.prdct) (1 /* No de passe : reconnaisseur */,
				item, q, q+1 /* inutilise */, prdct_no)) {
	/* Le predicat a echoue */
	continue;
      }
#endif /* is_prdct */

#if !is_parser && is_supertagging
      SXBA_1_bit (item2source_index_set [item], q);
#if LOG
      printf ("%s [" SXILD ", " SXILD "]\n", sxttext (sxplocals.sxtables, -X), PROLIS (item), q);
#endif /* LOG */
#endif /* !is_parser && is_supertagging */

      if (*(ptr2 = &(T2_shift_NT_hd [0])) == NULL) {
	/* c'est la 1ere fois qu'on rencontre un terminal */
	/* rhs_nt2where [0] == 0 (pour les terminaux) */
	*ptr2 = &(T2_items_stack [0]);
      }
    }
    else {
      /* X >= 0 */
      /* A -> \alpha . X \beta et \alpha =>* \epsilon et a_i \in FIRST1 (A -> \alpha . X \beta) */
      /* Cet item peut cependant ne pas etre valide si le predicat associe au a_i est faux.
	 Dans ce cas, il ne sera pas active par les sequences de reductions. */
#if is_epsilon
#if is_prdct
      if (
#ifdef MAKE_INSIDEG
	  spf.insideG.bvide &&
#endif /* MAKE_INSIDEG */
	  X > 0 &&
	  SXBA_bit_is_set (EMPTY_SET, X) &&
	  is_prdct_fun &&
	  (prdct_no = prdct [item]) != -1 &&
	  !(*for_semact.prdct) (1 /* No de passe : reconnaisseur */,
				item, q, q, prdct_no)) {
	/* Le predicat a echoue */
	/* On supprime les items de la meme production */
	next_item = item;

	do {
	  next_item++;

	  if (!SXBA_bit_is_set_reset (T2_non_kernel_item_set, next_item))
	    break;
	} while (LISPRO (next_item) != 0);

	continue;
      }
#endif /* is_prdct */

      if (X > 0) {
	/* C'est la 1ere fois qu'on trouve le nt X */
	if (*(ptr2 = &(T2_shift_NT_hd [X])) == NULL) {
	  /* c'est la 1ere fois qu'on est devant un X */
	  *ptr2 = &(T2_items_stack [RHS_NT2WHERE (X)]);
	}
      }
      else {
	/* Reduce */
	/* Ajoute' le 18/11/99 car T2_reduce_NT_hd n'est de'fini que si is_parser!! */
	/* item = A -> \alpha . et \alpha =>* \epsilon */
#if is_parser
#if is_constraints
	if (is_constraint_fun &&
	    (prdct_no = constraints [prod]) != -1 &&
	    !for_semact.constraint (1 /* No de passe : reconnaisseur */,
				    prdct_no, prod, NULL /* index_set */, q, q)) {
	  continue;
	}
#endif /* is_constraints */

	if (*(ptr2 = &(T2_reduce_NT_hd [A])) == NULL) {
	  /* c'est la 1ere fois qu'on fait un reduce vers A */
	  *ptr2 = &(T2_items_stack [LHS_NT2WHERE (A)]);
	}
#else /* !is_parser */
#if is_supertagging
	call_supertagger (PROLON (prod), 0);
#endif /* is_supertagging */

#if is_guide
	output_guide (prod, NULL, q, q);
#endif /* is_guide */
#endif /* !is_parser */

#if LOG
	output_ni_prod (prod, NULL, q, q);
#endif /* LOG */
      }
#else /* !is_epsilon */
      /* is_epsilon == SXFALSE */
      /* On est obligatoirement en debut de partie droite */
      /* X > 0 et T2_index_sets [item] == NULL */
      if (*(ptr2 = &(T2_shift_NT_hd [X])) == NULL) {
	/* c'est la 1ere fois qu'on est devant un X */
	*ptr2 = &(T2_items_stack [RHS_NT2WHERE (X)]);
      }
#endif /* !is_epsilon */
    }

    /* Ds le cas is_epsilon un item peut etre a la fois kernel et non-kernel */
    is_new_item = !is_in_reduce (&(T2_items_stack [(X == 0) ? LHS_NT2WHERE (A)
			                              : (X < 0 ? 0
				                               : RHS_NT2WHERE (X))]),
			   *ptr2, item);

    if (is_new_item)
      *((*ptr2)++) = item;

#if is_epsilon
    if (LISPRO (item-1)) {
      /* On n'est pas en debut de partie droite */
      /* Le symbole precedent est un nt */
      /* On positionne index_set et backward_index_set */
      /* Si index_set est non NULL, il peut etre modifie de fac,on safe, il n'est
	 pas commun avec une autre table. */
      /* On pourrait meme utiliser l'index_set de item-1 si non NULL */
      index_set = T2_index_sets [item];

      if (index_set == NULL)
	index_set = T2_index_sets [item] = sxba_bag_get (&shift_bag, q+1);

      SXBA_1_bit (index_set, q);
#if is_parser
      if ((backward_index_set = T2_backward_index_sets [item]) == NULL)
	backward_index_set = T2_backward_index_sets [item] = sxba_bag_get (&shift_bag, q+1);

      SXBA_1_bit (backward_index_set, q);
#endif /* is_parser */
    }
#endif /* is_epsilon */

  }
}



/* Adaptation a sdag et lex le 6/5/2002 */
/* Cette procedure complete le SXBA lex_compatible_item_set initialise (ds les tables)
   a l'ensemble des items des regles ne comportant aucun terminal */

/* l'idee est de construire pour chaque index i du source un ensemble lex_compatible_item_sets [i]
   qui ne prend en compte que les tokens du source > (ou >=!!) a i */
/* Il faut traiter le terminal source[i], meme si
   ce terminal a deja ete traite en source[j] (j>i) car les items qu'il induit ont pu ne pas etre valides a cause
   d'un terminal qui est apparu entre j et i, ces items pouvant maintenant etre valides */


/* **************************************************************************************** */




#if 0
static SXBA merge_table_item_set;

/* T [p] U= T [q] */
static void
merge_table (SXINT p, SXINT q)
{
  SXINT                 nt, item, delta;
  SXINT                 *topp_ptr, *topq_ptr, *botp_ptr, *botq_ptr;
  SXBA                  setp, setq;
  struct recognize_item *RTp, *RTq;
  struct parse_item     *PTp, *PTq;

#if EBUG
  if (p <= q)
    sxtrap (ME, "merge_table");
#endif /* EBUG */

#if LLOG
  printf ("\n****************** Entering merge_table (" SXILD ", " SXILD ") *****************\n", p, q);
  fputs ("\nBefore merging ...\n", stdout);
  output_table (p);
  output_table (q);
#endif /* LLOG */

  RTp = RT+p;
  RTq = RT+q;

#if is_parser
  PTp = PT+p;
  PTq = PT+q;
#endif /* is_parser */

  /* Je ne sais pas pourquoi copy_table fait une difference entre #ifdef MAKE_INSIDEG et #ifndef MAKE_INSIDEG !! */
  /* La`, je n'en fais pas */

  for (nt = MAXNT; nt >= 0; nt--) {
    topq_ptr = RTq->shift_NT_hd [nt];

    if (topq_ptr) {
      topp_ptr = RTp->shift_NT_hd [nt];

      botq_ptr = &(RTq->items_stack [0]);
      botp_ptr = &(RTp->items_stack [0]);

      if (nt > 0) {
	delta = RHS_NT2WHERE (nt);
	botp_ptr += delta;
	botq_ptr += delta;
      }

      if (topp_ptr == NULL) {
	while (botq_ptr < topq_ptr) {
	  item = *botp_ptr++ = *botq_ptr++;
	  
	  if ((setq = RTq->index_sets [item])) {
	    setp = sxba_bag_get (&shift_bag, p+1);
	    COPY (setp, setq);
	  }
	  else
	    setp = NULL;

	  RTp->index_sets [item] = setp;

#if is_parser
	  if ((setq = PTq->backward_index_sets [item])) {
	    setp = sxba_bag_get (&shift_bag, p+1);
	    COPY (setp, setq);
	  }
	  else
	    setp = NULL;

	  PTp->backward_index_sets [item] = setp;
#endif /* is_parser */
	} 
      }
      else {
	/* vrai merge des items, on passe par un item_set de travail */
	if (merge_table_item_set == NULL) {
	  sxprepare_for_possible_reset (merge_table_item_set);
	  merge_table_item_set = sxba_calloc (MAXITEM+1);
	}

	while (botp_ptr < topp_ptr) {
	  item = *botp_ptr++;
	  SXBA_1_bit (merge_table_item_set, item);
	}

	while (botq_ptr < topq_ptr) {
	  item = *botq_ptr++;

	  if (SXBA_bit_is_set (merge_table_item_set, item)) {
	    if ((setq = RTq->index_sets [item])) {
	      if ((setp = RTp->index_sets [item])) {
		OR (setp, setq);
	      }
	      else {
		setp = sxba_bag_get (&shift_bag, p+1);
		COPY (setp, setq);
	      }
	    }
	    else
	      setp = NULL;

	    RTp->index_sets [item] = setp;

#if is_parser
	    if ((setq = PTq->backward_index_sets [item])) {
	      if ((setp = PTp->backward_index_sets [item])) {
		OR (setp, setq);
	      }
	      else {
		setp = sxba_bag_get (&shift_bag, p+1);
		COPY (setp, setq);
	      }
	    }
	    else
	      setp = NULL;

	    PTp->backward_index_sets [item] = setp;
#endif /* is_parser */
	  }
	  else {
	    *botp_ptr++ = item;
	  
	    if ((setq = RTq->index_sets [item])) {
	      setp = sxba_bag_get (&shift_bag, p+1);
	      COPY (setp, setq);
	    }
	    else
	      setp = NULL;

	    RTp->index_sets [item] = setp;

#if is_parser
	    if ((setq = PTq->backward_index_sets [item])) {
	      setp = sxba_bag_get (&shift_bag, p+1);
	      COPY (setp, setq);
	    }
	    else
	      setp = NULL;

	    PTp->backward_index_sets [item] = setp;
#endif /* is_parser */
	  }
	} 

	sxba_empty (merge_table_item_set);
      }

      RTp->shift_NT_hd [nt] = botp_ptr;
    }
  }

#if is_rcvr
  OR (rcvr_reduce_item_sets [p], rcvr_reduce_item_sets [q]);
#endif /* is_rcvr */

  OR (RT_left_corner [p], RT_left_corner [q]);

#if is_rcvr
  OR (store_non_kernel_item_sets [p], store_non_kernel_item_sets [q]);
#endif /* is_rcvr */

#if is_rcvr
  OR (rcvr_kernel_item_sets [p], rcvr_kernel_item_sets [q]);
#endif /* is_rcvr */

  OR (non_kernel_item_sets [p], non_kernel_item_sets [q]);

#if RECOGNIZER_KIND == 2
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
    OR (kernel_nt_sets [p], kernel_nt_sets [q]);
#endif /* RECOGNIZER_KIND == 2 */

#if is_parser
  for (nt = MAXNT; nt > 0; nt--) {
    topq_ptr = PTq->reduce_NT_hd [nt];

    if (topq_ptr) {
      topp_ptr = PTp->reduce_NT_hd [nt];

      delta = LHS_NT2WHERE (nt);

      botq_ptr = &(RTq->items_stack [0]) + delta;
      botp_ptr = &(RTp->items_stack [0]) + delta;

      if (topp_ptr == NULL) {
	while (botq_ptr < topq_ptr) {
	  item = *botp_ptr++ = *botq_ptr++;
	  
	  if ((setq = RTq->index_sets [item])) {
	    setp = sxba_bag_get (&shift_bag, p+1);
	    COPY (setp, setq);
	  }
	  else
	    setp = NULL;

	  RTp->index_sets [item] = setp;

	  if ((setq = PTq->backward_index_sets [item])) {
	    setp = sxba_bag_get (&shift_bag, p+1);
	    COPY (setp, setq);
	  }
	  else
	    setp = NULL;

	  PTp->backward_index_sets [item] = setp;
	} 
      }
      else {
	/* vrai merge des items, on passe par un item_set de travail */
	if (merge_table_item_set == NULL) {
	  sxprepare_for_possible_reset (merge_table_item_set);
	  merge_table_item_set = sxba_calloc (MAXITEM+1);
	}

	while (botp_ptr < topp_ptr) {
	  item = *botp_ptr++;
	  SXBA_1_bit (merge_table_item_set, item);
	}

	while (botq_ptr < topq_ptr) {
	  item = *botq_ptr++;

	  if (SXBA_bit_is_set (merge_table_item_set, item)) {
	    if ((setq = RTq->index_sets [item])) {
	      if ((setp = RTp->index_sets [item])) {
		OR (setp, setq);
	      }
	      else {
		setp = sxba_bag_get (&shift_bag, p+1);
		COPY (setp, setq);
	      }
	    }
	    else
	      setp = NULL;

	    RTp->index_sets [item] = setp;

	    if ((setq = PTq->backward_index_sets [item])) {
	      if ((setp = PTp->backward_index_sets [item])) {
		OR (setp, setq);
	      }
	      else {
		setp = sxba_bag_get (&shift_bag, p+1);
		COPY (setp, setq);
	      }
	    }
	    else
	      setp = NULL;

	    PTp->backward_index_sets [item] = setp;
	  }
	  else {
	    *botp_ptr++ = item;
	  
	    if ((setq = RTq->index_sets [item])) {
	      setp = sxba_bag_get (&shift_bag, p+1);
	      COPY (setp, setq);
	    }
	    else
	      setp = NULL;

	    RTp->index_sets [item] = setp;

	    if ((setq = PTq->backward_index_sets [item])) {
	      setp = sxba_bag_get (&shift_bag, p+1);
	      COPY (setp, setq);
	    }
	    else
	      setp = NULL;

	    PTp->backward_index_sets [item] = setp;
	  }
	} 

	sxba_empty (merge_table_item_set);
      }

      PTp->reduce_NT_hd [nt] = botp_ptr;
    }
  }
#endif /* is_parser */

#if LLOG
  fputs ("\nAfter merging ...\n", stdout);
  output_table (p);
  printf ("\n****************** Leaving merge_table (" SXILD ", " SXILD ") *****************\n", p, q);
#endif /* LLOG */
}
#endif /* 0 */

#if RECOGNIZER_KIND == 2
static SXBOOLEAN
compute_recognizer_beam_threshold (SXINT q)
{
  SXINT *pq_stack = idag.q2pq_stack + idag.q2pq_stack_hd [q];
  SXINT *pq_ptr, *top_ptr, *bot_ptr, *t_stack, *t_stack_top;
  SXINT p, i, pq, t, item_p, pqprime, pqprimet;
  struct RT_global_logprobs *RT_global_logprobs_p;
  struct recognize_item *RTp;
  SXLOGPROB local_logprob, local_max_logprob = SXLOGPROB_MIN;
  SXBA pq_t_set;
  SXBOOLEAN q_is_reachable = SXFALSE, bool;
  SXLOGPROB transition_logprob, local_transition_logprob;

#if EBUG
  if (q == 0)
    sxtrap (ME, "compute_recognizer_beam_threshold (this function should never be called with q = 0)");
#endif /* EBUG */

  for (pq_ptr = pq_stack + pq_stack [0]; pq_ptr > pq_stack; pq_ptr--) {
    pq = *pq_ptr;
    pq_t_set = idag.pq2t_set [pq];
    p = range2p (pq);
    RT_global_logprobs_p = &(RT_global_logprobs [p]);
    RTp = &(RT [p]);
    top_ptr = RTp->shift_NT_hd [0];

    if (top_ptr) { /* sinon, table Earley T [p] vide */
      bot_ptr = &(RTp->items_stack [0]);

      while (--top_ptr >= bot_ptr) {
	item_p = *top_ptr;
	
	if (item_p < 0) /* item_p a été viré précédemment */
	  continue;

	t = -LISPRO (item_p);
#ifdef MAKE_INSIDEG
	t = spf.insideG.t2init_t [t];
#endif /* MAKE_INSIDEG */

	bool = SXBA_bit_is_set (pq_t_set, t);
	
	if (recognizer_beam_type	  
	    /* Le filtrage au niveau de q se fait sur la base du max des seuils globaux (toutes transitions sortant de p confondues, y compris vers ailleurs que q) des p tels qu'il existe une transition de p à q */
	    ||
	    /* Le filtrage au niveau de q se fait sur la base des seuils calculés sur chaque p tel qu'il existe une transition de p à q, seuil "q-spécifique" obtenu en prenant en compte seulement les transitions p->q ; on doit donc vérifier qu'il y a bien une transition p-t->q */
	    bool)
	  {
	    if (recognizer_beam_type) {
	      if (p == 0) {
		transition_logprob = 0;
	      } else {
		transition_logprob = SXLOGPROB_MIN;
		for (pqprime = idag.p2pq_hd [p]; pqprime < idag.p2pq_hd [p+1] && pqprime <= idag.last_pq; pqprime++) {
		  if ((pqprimet = pq_t2pqt (pqprime, t))) {
		    local_transition_logprob = pqt2logprob (pqprimet);
		    if (cmp_SXLOGPROB (local_transition_logprob, transition_logprob) > 0 /* > */)
		      transition_logprob = local_transition_logprob;
		  }
		}
		if (transition_logprob == SXLOGPROB_MIN)
		  continue;
	      }
	    } else
	      transition_logprob = pq_t2logprob (pq, t);

	    local_logprob = RT_global_logprobs_p->RT_logprobs [top_ptr - bot_ptr].max_global_logprob + transition_logprob;
#if LLLOG
	    fprintf (sxstdout, "MAX_GLOBAL_EXPECTED local_logprob = " SXLPD " + " SXLPD "\n", SXLOGPROB_display (RT_global_logprobs_p->RT_logprobs [top_ptr - bot_ptr].max_global_logprob), SXLOGPROB_display (transition_logprob));
#endif /* LLLOG */
	    
	    if (cmp_SXLOGPROB (local_logprob, local_max_logprob) > 0 /* > */) {
	      local_max_logprob = local_logprob;
#if LLLOG
	      fprintf (sxstdout, "  Updating local_max_logprob to " SXLPD "\n", SXLOGPROB_display (local_max_logprob));
#endif /* LLLOG */
	    }
	  }
	if (bool)
	  q_is_reachable = SXTRUE;
      }
    }
  }

  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
    earley_table_companion [q].max_global_expected_logprob = local_max_logprob;

  return q_is_reachable;
}
#endif /* RECOGNIZER_KIND == 2 */

static SXBOOLEAN
scan (SXINT q)
{
  SXINT *pq_ptr, *pq_stack = idag.q2pq_stack + idag.q2pq_stack_hd [q];
  SXINT p;
  SXBOOLEAN shift_has_succeeded, ret_val;

  ret_val = SXFALSE;
  
#if EBUG
  if (q == 0)
    sxtrap (ME, "scan (this function should never be called with q = 0)");
#endif /* EBUG */

  if (q == 1) {
#if HAS_BEAM == 1
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set)
      shift_has_succeeded = beam_shift (0, 0, 1);
    else
#endif /* HAS_BEAM == 1 */
      shift_has_succeeded = shift (0, 0, 1);
    
    ret_val = ret_val || shift_has_succeeded;
    
    if (shift_has_succeeded) {
      SXBA_1_bit (mlstn_active_set, 1);
      
      if (TOP (reduce_list))
	scan_reduce (0, 1);
    }
  } else {
    for (pq_ptr = pq_stack + pq_stack [0]; pq_ptr > pq_stack; pq_ptr--) {
      p = range2p (*pq_ptr);
#if HAS_BEAM == 1
      if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set)
	shift_has_succeeded = beam_shift (0, p, q);
      else
#endif /* HAS_BEAM == 1 */
	shift_has_succeeded = shift (0, p, q);

      ret_val = ret_val || shift_has_succeeded;
    
      if (shift_has_succeeded) {
	SXBA_1_bit (mlstn_active_set, q);
      
	if (TOP (reduce_list))
	  scan_reduce (p, q);
      }
    }
  }

  return ret_val;
}

static SXVOID
build_initial_earley_table ()
{
  struct logprob_struct   *pq_logprob_ptr;

  /* Initialisation : on met le kernel item */
  /* RT [0].items_stack [0] = 0; S' -> . $ S $ */
  RT [0].shift_NT_hd [0] = &(RT [0].items_stack [1]);

#if HAS_BEAM == 1

#if RECOGNIZER_KIND == 2

#if LLOG
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
    printf ("***************** BEAM_KIND = %i, SCAN_BEAM_VALUE = " SXLPD ", SHIFT_BEAM_VALUE = " SXLPD " *************\n", beam_kind, SXLOGPROB_display (earley_table_companion [0].dynamic_scan_beam_value), SXLOGPROB_display (earley_table_companion [0].dynamic_shift_beam_value));
#endif /* LLOG */

  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
    earley_table_companion [0].max_global_expected_logprob = 0;
    earley_table_companion [0].dynamic_scan_beam_value = recognizer_scan_beam_value;
    earley_table_companion [0].dynamic_shift_beam_value = recognizer_shift_beam_value;
    earley_table_companion [0].dynamic_beam_increment_nb = 0;
  }
#endif /* RECOGNIZER_KIND == 2 */
  
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) {
    ei2set_logprob_ptr (0 /* S' -> . $ S $ */, p_q2pq (0, 0), pq_logprob_ptr);
    pq_logprob_ptr->inside.left = 0;
  }

#if RECOGNIZER_KIND == 2
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
    pq_logprob_ptr->outside = 0; 
    pq_logprob_ptr->expected_total = item2max_right_inside_logprob [1];
    
#endif /* RECOGNIZER_KIND == 2 */
    
#if LLOG
    output_logprob_struct (pq_logprob_ptr);
#endif /* LLOG */
    
#if RECOGNIZER_KIND == 2
    {
      struct RT_global_logprobs *RT_global_logprobs_0 = &(RT_global_logprobs [0]);
      struct RT_logprobs *tmp_RT_logprob_ptr = &(RT_global_logprobs_0->RT_logprobs [0]);

      tmp_RT_logprob_ptr->max_global_logprob = item2max_right_inside_logprob [1];
      tmp_RT_logprob_ptr->offset_in_global_logprobs = -1;
    }
#endif /* RECOGNIZER_KIND == 2 */
  }
#endif /* HAS_BEAM == 1 */
 
  SXBA_1_bit (mlstn_active_set, 0);

#if LOG
  output_table (0);
#endif /* LOG */
}

static SXBOOLEAN
complete (SXINT q)
{
  SXBA                  index_set;
  SXBOOLEAN             has_non_kernel_items, scan_check_successful;

  /* 0 <= q <= final_pos+rcvr_tok_no,  0 <= q <= final_pos+rcvr_tok_no */
  /* Construit totalement les tables Tq.
     Si la table Tq est non vide, elle aura des items de la forme 
     A' -> \alpha' . t' \beta' \in mlstn2look_ahead_item_set[q] */

  /* Les predicats associes aux items terminaux (kernel et non-kernel) sont verifies. */

  /* ATTENTION si is_epsilon, un meme item peut etre a la fois kernel et non-kernel :
     Soit A -> \alpha . \beta kernel et B -> \gamma . C \delta kernel
     si C =>* A et \alpha =>* \epsilon alors  A -> \alpha . \beta est aussi non-kernel.
     Dans ce cas l'index_set associe a A -> \alpha' X . \beta n'est pas l'index_set de
     A -> \alpha'. X \beta mais une copie afin de permettre la modif ulterieure (ajout du
     numero i de la table locale). */

#if LLOG
  printf ("*** Entering complete (q = " SXILD ") ***\n", q);
#endif /* LLOG */

  has_non_kernel_items = (sxba_scan (T2_non_kernel_item_set, -1) >= 0);
  
  if (has_non_kernel_items) {
    process_non_kernel_filtering (q);

#if HAS_BEAM == 1
#if RECOGNIZER_KIND == 2
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
      set_non_kernel_item_logprobs_recognizer (q);
    else
#endif /* RECOGNIZER_KIND == 2 */
      {
#if REDUCER_KIND
	if (reducer_beam_value_is_set)
	  set_non_kernel_item_logprobs_reducer (q);
#else /* REDUCER_KIND */
	;
#endif /* REDUCER_KIND */
      }
#endif /* HAS_BEAM == 1 */

    process_non_kernel (q);
  }

#if RECOGNIZER_KIND == 2
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
    scan_check_successful = fill_RT_global_logprobs (q);
  else
#endif /* RECOGNIZER_KIND == 2 */
    scan_check_successful = check_scans (q);

#if is_rcvr
  if (!scan_check_successful && has_non_kernel_items) {

   /* Ici on est su^r qu'on est dans une impasse mais la detection courante
	 peut etre due a la lexicalisation, on essaie donc de "retarder
	 le traitement". Ex
	 source = "( a + a" --- il manque ")"
	 La lexicalisation a supprime' l'item  E -> . ( E ) puisqu'il n'y a pas de ")" ds le source
	 La detection a lieu la`
	 ( a + a
	 ^
	 Or, si on reactive cet item la detection aura lieu la`
	 ( a + a
	        ^
      */
      if (ilex_compatible_item_set) {
	/* y'a eu du filtrage on va essayer de recommencer sans filtrage */
	ilex_compatible_item_set = NULL;
	/* On recupere donc la valeur initiale ... */
	sxba_copy (T2_non_kernel_item_set, rcvr_store_unfiltered_non_kernel_item_set);
    
	process_non_kernel_filtering (q);

	if (!is_error_detected)
	  /* On ne [re]regarde que les nouveaux 
	     ca n'a pas ete fait ds process_non_kernel_filtering */
	  sxba_minus (T2_non_kernel_item_set, T2_store_non_kernel_item_set);

	process_non_kernel (q);

#if RECOGNIZER_KIND == 2
	if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
	  scan_check_successful = fill_RT_global_logprobs (q);
	else
#endif /* RECOGNIZER_KIND == 2 */
	  scan_check_successful = check_scans (q);
	
      }
  }
#endif /* is_rcvr */

#if LLOG
  printf ("*** Leaving complete (q = " SXILD "): %s ***\n", q, scan_check_successful ? "SXTRUE" : "SXFALSE");
#endif /* LLOG */

  return scan_check_successful;
}


static SXBOOLEAN
build_earley_table (SXINT q) /* remplace 'complete' */
{
  SXINT *pq_stack = idag.q2pq_stack + idag.q2pq_stack_hd [q], *pq_ptr, p;
  SXBOOLEAN ret_val = SXTRUE;
  
#if EBUG
  if (q == 0)
    sxtrap (ME, "build_earley_table (this function should never be called with q = 0)");
#endif /* EBUG */

  set_T2 (q);
  
#if RECOGNIZER_KIND == 2
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
    struct earley_table_companion *earley_table_companion_q = earley_table_companion + q;
    
#if is_beamrcvr
    if (earley_table_companion_q->beamrcvr_state == 0) { /* première exécution sans beamrcvr */
#endif /* is_beamrcvr */
#if LOG
      printf ("\n**** BUILDING EARLEY TABLE T [" SXILD "] ****\n", q);
#endif /* LOG */
      earley_table_companion_q->dynamic_beam_increment_nb = 0;
      earley_table_companion_q->dynamic_scan_beam_value = recognizer_scan_beam_value;
      earley_table_companion_q->dynamic_shift_beam_value = recognizer_shift_beam_value;
      ret_val = compute_recognizer_beam_threshold (q); /* positionne earley_table_companion_q->max_global_expected_proba
et renvoie un booléen qui dit si q est reachable */
#if is_beamrcvr
      earley_table_companion_q->no_more_beamrcvr_possible = SXFALSE;
    }
    else {
      SXBOOLEAN changed_at_least_one_beam_value = SXFALSE;
      if (earley_table_companion_q->beamrcvr_state == 2 /* redémarrage depuis le début */) {
	earley_table_companion_q->dynamic_beam_increment_nb++;
	ret_val = compute_recognizer_beam_threshold (q); /* positionne earley_table_companion_q->max_global_expected_proba
et renvoie un booléen qui dit si q est reachable */
      }
      if (scan_rcvr_beam_delta_is_set && earley_table_companion_q->did_scan_beam) { /* le scan_beam on a beamé lors du dernier essai */
	earley_table_companion_q->did_scan_beam = SXFALSE;
	earley_table_companion_q->did_shift_beam = SXFALSE;
	if (earley_table_companion_q->beamrcvr_state >= 1) {
	  if (earley_table_companion_q->beamrcvr_state == 2)
	    earley_table_companion_q->dynamic_scan_beam_value += scan_rcvr_beam_delta;
	  else if (earley_table_companion_q->beamrcvr_state == 1)
	    earley_table_companion_q->dynamic_scan_beam_value = earley_table_companion_q->dynamic_scan_beam_first_beamed_logprob - earley_table_companion_q->max_global_expected_logprob + scan_rcvr_beam_delta;
	  changed_at_least_one_beam_value = SXTRUE;
	}
      }
      if (shift_rcvr_beam_delta_is_set && earley_table_companion_q->did_shift_beam) { /* le shift_beam on a beamé lors du dernier essai (mais plus le scan_beam) */   
	earley_table_companion_q->did_shift_beam = SXFALSE;
	if (earley_table_companion_q->beamrcvr_state >= 1) {
	  if (earley_table_companion_q->beamrcvr_state == 2)
	    earley_table_companion_q->dynamic_shift_beam_value += shift_rcvr_beam_delta;
	  else if (earley_table_companion_q->beamrcvr_state == 1)
	    earley_table_companion_q->dynamic_shift_beam_value = earley_table_companion_q->dynamic_shift_beam_first_beamed_logprob - earley_table_companion_q->max_global_expected_logprob + shift_rcvr_beam_delta;
	  changed_at_least_one_beam_value = SXTRUE;
	}
      }
      if (cmp_SXLOGPROB (earley_table_companion_q->dynamic_scan_beam_value, earley_table_companion_q->dynamic_shift_beam_value) < 0 /* < */)
	earley_table_companion_q->dynamic_shift_beam_value = earley_table_companion_q->dynamic_scan_beam_value;
      if (earley_table_companion_q->beamrcvr_state == 1 && !changed_at_least_one_beam_value)
	/* Plus rien n'est beamé, on n'a plus rien à tenter côté beamrcvr local (ou on l'a pas demandé), il faudra passer par un beamrcvr global */
	ret_val = SXFALSE;
      earley_table_companion_q->no_more_beamrcvr_possible = !changed_at_least_one_beam_value;
#if LOG
      printf ("\n**** BUILDING EARLEY TABLE T [" SXILD "] <BEAMRCVR (%s:" SXILD ")> ****\n", q, earley_table_companion_q->beamrcvr_state == 2 ? "global" : "local", earley_table_companion_q->dynamic_beam_increment_nb);
#endif /* LOG */
    }
#endif /* is_beamrcvr */
  }

#if LOG
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
    printf ("earley_table_companion [" SXILD "].max_global_expected_logprob = " SXLPD "\n", q, SXLOGPROB_display (earley_table_companion [q].max_global_expected_logprob));
    for (pq_ptr = pq_stack + pq_stack [0]; pq_ptr > pq_stack; pq_ptr--) {
      p = range2p (*pq_ptr);
      printf ("  vs. predecessor " SXILD ", difference = " SXLPD "\n", p, SXLOGPROB_display (earley_table_companion [q].max_global_expected_logprob - earley_table_companion [p].max_global_expected_logprob));
    }
    printf ("earley_table_companion [" SXILD "].dynamic_scan_beam_value = " SXLPD "\n", q, SXLOGPROB_display (earley_table_companion [q].dynamic_scan_beam_value));
    printf ("earley_table_companion [" SXILD "].dynamic_shift_beam_value = " SXLPD "\n", q, SXLOGPROB_display (earley_table_companion [q].dynamic_shift_beam_value));
    printf ("earley_table_companion [" SXILD "].did_scan_beam = %i\n", q, earley_table_companion [q].did_scan_beam);
    printf ("earley_table_companion [" SXILD "].did_shift_beam = %i\n", q, earley_table_companion [q].did_shift_beam);
  }
#endif /* LOG */

#if EBUG
  if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
    if (cmp_SXLOGPROB (earley_table_companion [q].max_global_expected_logprob, 0) > 0
	|| cmp_SXLOGPROB (earley_table_companion [q].dynamic_shift_beam_value, 0) > 0
	|| cmp_SXLOGPROB (earley_table_companion [q].dynamic_scan_beam_value, 0) > 0
	)
      sxtrap (ME, "build_earley_table (no log prob should be > 0)");
  }
#endif /* EBUG */
#endif /* RECOGNIZER_KIND == 2 */

  if (ret_val)
    ret_val = scan (q); /* perform all scans landing in q */

  if (ret_val)
    ret_val = complete (q);

#if LOG
  output_table (q);
#endif /* LOG */

  return ret_val;
}


#if is_1la
/* Cette procedure remplit mlstn2look_ahead_item_set, le *SXBA qui a chaque mlstn associe l'ensemble des items
   A -> \alpha . \beta t.q. t \in First(\beta) et t est une transition valide'e par la lexicalisation depuis mlstn */
/* Si une erreur a ete detectee, le test du lokk_ahead est inhibee !! */
fill_mlstn2look_ahead_item_set (SXBOOLEAN is_first_time)
{
  SXINT  mlstn, t;
  SXBA look_ahead_item_set, look_ahead_t_set;

#ifdef MAKE_INSIDEG
  if (spf.insideg.t2item_set == NULL)
    fill_t2item_set (); /* Ds lexicalizer_mngr */
#endif /* MAKE_INSIDEG */

  for (mlstn = final_mlstn; mlstn >= init_mlstn; mlstn--) {
    look_ahead_item_set = mlstn2look_ahead_item_set [mlstn];
    if (!is_first_time) sxba_empty (look_ahead_item_set);

    /* Ds tous les cas mlstn2look_ahead_t_set est correct */
    look_ahead_t_set = mlstn2look_ahead_t_set [mlstn];

    t = 0;

    while ((t = sxba_scan (look_ahead_t_set, t)) >= 0)
      sxba_or (look_ahead_item_set, T2ITEM_SET (t));

    /* Filtrage par ilex_compatible_item_set dans tous les cas (et non par lex_compatible_item_sets [mlstn]
       ds le cas is_lex2 || is_set_automaton) car lex_compatible_item_sets [mlstn] ne contient que des items
       de la fermeture.  Par exemple la verification que l'item A -> \alpha . B \beta est correct
       vis_a_vis du look_ahead t, alors que B -> t va echouer car cet item n'est pas ds lex_compatible_item_sets [mlstn] */

    if (ilex_compatible_item_set)
      /* On prend en compte la lexicalisation */
      sxba_and (look_ahead_item_set, ilex_compatible_item_set);
  }
}
#endif /* is_1la */


/* On est ds le cas "is_dag", le source est un vrai dag */
/* Si l'on suppose qu'il y a en du guidage par un "dynamic set automaton", le traitement d'erreur au
  niveau du dsa se resume comme suit :
  - La correction a inseree des etats (et des transitions) ds le dag d'entree (rcvr_tok_no <= max_rcvr_tok_no)
  - Le traitement global s'est debrouille avec ca (attention le filtre lex_compatible_item_sets [dag_state]
    peut etre NULL)
  Pour le traitement des erreurs au niveau Earley :
  - Pas de correction locale (inutile ca a deja ete tente'/fait au niveau du dsa)
  - On se contente du traitement global qui ne modifie pas le dag

  D'autre part l'implantation earley suppose que les indices p, q des tables que l'on manipule sont
  tels que si [A -> \alpha . \beta, p] \in Tq on a p < q et qu'il y a un chemin ds le dag de p a` q

  Conclusion : les indices des tables sont des milestones
*/

#if is_rcvr
/* global recovery ds le cas is_dag */
/* Principe de la recuperation globale */
/*
  on est sur une table de l'analyseur earley d'indice mlstn/dag_state 
  Il n'existe pas ds le dag de transition sur t depuis dag_state et il n'existe pas
  ds la table d'item de la forme A -> \alpha .t \beta
  Pour tout item de la table de la forme [A -> \alpha . \beta t \gamma] on recherche ds le dag
  s'il existe une transition d1 ->t d2 t.q. le mlstn de d1 soit superieur a mlstn.
  Si c'est le cas, on met [A -> \alpha \beta .t \gamma] ds la table.
  Il peut ds le dag ne pas exister de chemin entre dag_state et d1.
  On suppose donc qu'entre dag_state et d1 il y a un chemin qui s'epelle comme une \beta-phrase.
 */


/* Le 15/12/05 */
/* Nelle version, on ne prend que la dead_end la + a droite et on utilise earley_no_dag_rcvr () */
static void
earley_dag_rcvr ()
{
  SXINT     dead_end, rcvr_q;

  /* On ne prend que la + a droite, les autres eventuelles seront examinees eventuellement + tard
     suivant le bon vouloir de range_walk () */
  dead_end = sxba_1_rlscan (dead_ends, idag.final_state+1+rcvr_tok_no+1);
  sxba_empty (dead_ends);

#if EBUG
  if (dead_end == -1)
    sxtrap (ME, "earley_dag_rcvr");
#endif /* EBUG */

  rcvr_q = earley_no_dag_rcvr (dead_end);
    
  if (rcvr_q)
    SXBA_1_bit (mlstn_active_set, rcvr_q);
}
#endif /* is_rcvr */


#if REDUCER_KIND

#ifdef BEAM_MEASURE
static void
put_a_valid_beam_measure (SXINT item_j, SXINT j)
{
  SXINT prod, init_prod, init_item_j, i, ij, ij_lgth, couple;
  SXBA  index_set_j;

  prod = PROLIS (item_j);

  if (item_j > PROLON (prod)) {
    /* On ne stocke pas les items initiaux */
#ifdef MAKE_INSIDEG
    init_prod = spf.insideG.prod2init_prod [prod];
    init_item_j = prolon [init_prod] + item_j - PROLON (prod);
#else /* MAKE_INSIDEG */
    init_item_j = item_j;
#endif /* MAKE_INSIDEG */
	
    index_set_j = PT2 [j].index_sets [item_j];
    i = -1;

    while ((i = sxba_scan (index_set_j, i)) >= 0 && i <= j) {
      ij = p_q2pq (i, j);
      ij_lgth = range2min_lgth [ij];

      XxY_set (&beam_measure_valid_table_hd, item_j, ij_lgth, &couple);
    }
  }
}


  /* Si BEAM_MEASURE, on stocke associe a chaque couple ([A -> \alpha . Y \beta], min_lgth (i, k))
     le min des logprobs inside.left de item_k.
     Il serait preferable de deplacer ce calcul apres le calcul des nbest car a ce moment on dispose
     exactement des items participants pour un n donne. */
static void
put_a_controlled_beam_measure (SXINT item, SXINT ij_lgth, SXLOGPROB logprob)
{
  SXINT couple;

  if (controlled_beam_min_lgth2logprob [ij_lgth] > logprob)
    controlled_beam_min_lgth2logprob [ij_lgth] = logprob;

  if (!XxY_set (&controlled_beam_global_table, item, ij_lgth, &couple)
      || logprob < controlled_beam_global_table2logprob [couple]) {
    controlled_beam_global_table2logprob [couple] = logprob;
  }
}
#endif /* BEAM_MEASURE */

/*
  les logprobs associees a l'item_j [A -> \alpha Y . \beta, i, j] sont connues par j_logprob_ptr
  On calcule celles de item_k [A -> \alpha . Y \beta, i, k] si elles sont valides
  On calcule egalement celle de Ykj si Y est un (bon) NT (Y > 0)
  Y == 0 est caracteristique d'une rcvr entre item_k et item_j
 */
/* Le 21/09/10 Nouvelle version dans laquelle on ne propage plus individuellement les logprobs
   prefixe et suffixe mais uniquement leur somme rassemblee dans une proba outside */
static SXINT
set_backward_logprob (SXINT i, SXINT k, SXINT j, SXINT item_k, SXINT item_j, SXINT Y, SXINT *kj_ptr)
{
  SXINT               ik, kj, k_triple, pqt, t, changed_k_logprob = 0;
  struct logprob_struct *j_logprob_ptr, *k_logprob_ptr;
  SXLOGPROB              Ykj_inside_logprob, Ykj_outside_logprob, total_logprob;
#if LLOG
  struct logprob_struct local_logprob;
#endif /* LLOG */

  if (Y != 0 && (kj = *kj_ptr) == 0)
    kj = *kj_ptr = p_q2pq (k, j);

  ei2get_logprob_ptr (item_j, p_q2pq (i, j), j_logprob_ptr);

  ik = p_q2pq (i, k);

  if (item_k == 0 || LISPRO (item_k-1) == 0) {
    /* item_k = A -> . \alpha */
#if EBUG
    if (i != k)
      sxtrap (ME, "set_backward_logprob");
#endif /* EBUG */

    ei2set_logprob_ptr (item_k, ik, k_logprob_ptr), k_triple = ei;

    if (!is_ei_set)
      /* 1ere fois qu'on tombe dessus */
      *k_logprob_ptr = nul_logprob;
  }
  else
    ei2get_logprob_ptr (item_k, ik, k_logprob_ptr), k_triple = ei;

#if LLLOG
  printf ("Made and filled ei: (item=" SXILD ", pq=" SXILD ") [ei=" SXILD " (%s)] {outside = " SXLPD ", inside.left = " SXLPD ", expected_total = " SXLPD "}: ", item_k, ik, ei, is_ei_set ? "old"  : "new", SXLOGPROB_display (k_logprob_ptr->outside), SXLOGPROB_display (k_logprob_ptr->inside.left), SXLOGPROB_display (k_logprob_ptr->expected_total));
	output_ei (ei);
#endif /* LLLOG */

#ifdef BEAM_MEASURE
  /* Si BEAM_MEASURE, on stocke associé a chaque couple ([A -> \alpha . Y \beta], min_lgth (i, k))
     le min des logprobs inside.left de item_k.
     Il serait preferable de deplacer ce calcul apres le calcul des nbest car a ce moment on dispose
     exactement des items participants pour un n donne. */
#if is_rcvr
  if (!is_error_detected)
    /* On ne prend pas de mesure si une erreur est detectee */
#endif /* is_rcvr */
    {
      SXINT prod, init_prod, init_item_k;

      prod = PROLIS (item_k);

      if (item_k > PROLON (prod)) {
	/* On ne met pas la proba des items initiaux qui est nulle */
#ifdef MAKE_INSIDEG
	init_prod = spf.insideG.prod2init_prod [prod];
	init_item_k = prolon [init_prod] + item_k - PROLON (prod);
#else /* MAKE_INSIDEG */
	init_item_k = item_k;
#endif /* MAKE_INSIDEG */

	put_a_controlled_beam_measure (init_item_k, range2min_lgth [ik], k_logprob_ptr->inside.left);
      }
    }
#endif /* BEAM_MEASURE */

  Ykj_outside_logprob = j_logprob_ptr->outside + k_logprob_ptr->inside.left + j_logprob_ptr->inside.right;

  if (Y > 0) {
    Axpq2get_max_logprob_ptr (Y, kj); /* macro qui positionne max_logprob_ptr et AxI_J */
    Ykj_inside_logprob = max_logprob_ptr->inside;
    /* Ykj_inside_logprob = j_logprob_ptr->inside.left - k_logprob_ptr->inside.left;
       est faux car la meilleure propa de j a pu se calculer a partir d'un autre k, disons k'.
       La proba Ykj_inside_logprob ainsi calculee peut donc etre surevaluee */
  }
  else if (Y < 0) {
    if (-Y == SXEOF)
      Ykj_inside_logprob = 0;
    else {
#ifdef MAKE_INSIDEG
      t = spf.insideG.t2init_t [-Y];
#else
      t = -Y;
#endif /* MAKE_INSIDEG */

      pqt = pq_t2pqt (kj, t);

#if EBUG
      if (pqt == 0)
	sxtrap (ME, "set_backward_logprob");
#endif /* EBUG */

      Ykj_inside_logprob = pqt2logprob (pqt);
    }
  }
  else /* Y = 0: on ne calcule pas de trucs pour Ykj, on vient d'un endroit en rcvr */
#if is_rcvr
    Ykj_inside_logprob = get_max_inside_rcvr_logprob (item_k, item_j);
#else /* is_rcvr */
    sxtrap (ME, "set_backward_logprob");
#endif /* is_rcvr */

  total_logprob = Ykj_outside_logprob + Ykj_inside_logprob;
  
  if (cmp_SXLOGPROB (total_logprob,
		  min_acceptable_tree_logprob) < 0) {
    changed_k_logprob = -1;
#if LLOG
    local_logprob = *k_logprob_ptr;
    k_logprob_ptr->outside = j_logprob_ptr->outside;
    k_logprob_ptr->inside.right = Ykj_inside_logprob + j_logprob_ptr->inside.right;
    k_logprob_ptr->expected_total = total_logprob;
    printf ("\t<< [" SXILD ".." SXILD "]: Beamed out logprob item since " SXLPD " < " SXLPD " ", i, k, 
	    SXLOGPROB_display (total_logprob),
	    SXLOGPROB_display (min_acceptable_tree_logprob));
    output_item (item_k, NULL, i, k);
    *k_logprob_ptr = local_logprob;
#endif /* LLOG */
  }
  else {
    if (SXBA_bit_is_set (ei_set, k_triple)) {
      if (cmp_SXLOGPROB (k_logprob_ptr->expected_total,
		      total_logprob) < 0 /* < */) {
	/* la nelle k_logprob est meilleure que la precedente */
#if LLOG
	printf ("\t<< [" SXILD ".." SXILD "]: This previous logprob item ", i, k);
	output_item (item_k, NULL, i, k);
#endif /* LLOG */
	k_logprob_ptr->outside = j_logprob_ptr->outside;
	k_logprob_ptr->inside.right = Ykj_inside_logprob + j_logprob_ptr->inside.right;
	k_logprob_ptr->expected_total = total_logprob;
	changed_k_logprob = 1;
#if LLOG
	fputs ("\t\tis replaced by ", stdout);
	output_item (item_k, NULL, i, k);
#endif /* LLOG */
      }

#if LLOG
      else {
	local_logprob = *k_logprob_ptr;

	printf ("\t<< [" SXILD ".." SXILD "]: This previous logprob item ", i, k);
	output_item (item_k, NULL, i, k);

	k_logprob_ptr->outside = j_logprob_ptr->outside;
	k_logprob_ptr->inside.right = Ykj_inside_logprob + j_logprob_ptr->inside.right;
	k_logprob_ptr->expected_total = total_logprob;

	fputs ("\t\tsubsumes ", stdout);
	output_item (item_k, NULL, i, k);

	*k_logprob_ptr = local_logprob;
      }
#endif /* LLOG */
    }
    else {
#ifdef DYNAMIC_BEAM_MEASURES
      if (reducer_beam_value_is_set
	  && (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)) {
	/* on stocke les logprobs venant du reconaisseur avant qu'elles ne soient écrasées (améliorées!) par le reducer */
	SXINT prod = PROLIS (item_k);
	if (item_k > PROLON (prod)) { /* sinon on est sur un item de la forme A -> . \alpha */
	  if (LISPRO (item_k - 1) < 0) {/* on est donc sur un item post-scan ( = de la forme A -> \alpha t . \beta ) */
	    if (recognizer_scan_beam_value_is_set
		&& (DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [k] == 0 
		    || cmp_SXLOGPROB (k_logprob_ptr->expected_total, DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [k]) < 0))
	      DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [k] = k_logprob_ptr->expected_total;
	  }
	  else {
	    if (recognizer_shift_beam_value_is_set
		&& (DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob [k] == 0 
		    || cmp_SXLOGPROB (k_logprob_ptr->expected_total, DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob [k]) < 0))
	      DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob [k] = k_logprob_ptr->expected_total;
	  }
	}
      }
#endif /* DYNAMIC_BEAM_MEASURES */
      /* C'est la 1ere fois qu'on tombe sur une k_logprob superieure au seuil, on la note */
      k_logprob_ptr->outside = j_logprob_ptr->outside;
      k_logprob_ptr->inside.right = Ykj_inside_logprob + j_logprob_ptr->inside.right;
      k_logprob_ptr->expected_total = total_logprob;
      SXBA_1_bit (ei_set, k_triple);
      changed_k_logprob = 1;
#if LLOG
      printf ("\t<< [" SXILD ".." SXILD "]: This brand new beam proof logprob item is set ", i, k);
      output_item (item_k, NULL, i, k);
#endif /* LLOG */
    }
  }

  if (changed_k_logprob >= 0 && Y > 0) {
    if (SXBA_bit_is_reset_set (AxI_J_set, AxI_J)) {
      max_logprob_ptr->outside = Ykj_outside_logprob;
    }
    else {
      if (cmp_SXLOGPROB (max_logprob_ptr->outside, Ykj_outside_logprob) < 0) {
	max_logprob_ptr->outside = Ykj_outside_logprob;
      }
      else
	changed_k_logprob += 2;
    }
  }

#if EBUG
  if (changed_k_logprob > 0)
    check_logprob (k_logprob_ptr, item_k, i, k);	      
#endif /* EBUG */

  return changed_k_logprob; /* -1 => k_logprob est beam out
			      0 => k_logprob est moins bon que celui du coup precedent et Ykj est nouveau ou remplace l'ancien
			      1 => k_logprob est garde' et Ykj est nouveau ou remplace l'ancien
			      2 => k_logprob est moins bon que celui du coup precedent et Ykj est moins bon que l'ancien
			      3 => k_logprob est garde' et Ykj est moins bon que l'ancien
			  */
}

static SXINT
set_reduce_item_logprob (SXINT item)
{
  SXINT                  ij, i, j;
  SXLOGPROB              outside_logprob, total_logprob;
  SXINT                  changed_j_logprob = 0;
  struct logprob_struct  *j_logprob_ptr;
#if LLOG
  struct logprob_struct local_logprob;
#endif /* LLOG */

  /* L'appelant vient d'appeler Axpq2get_max_logprob_ptr (A, ij); */
  ij = XxY_Y (AxI_J_hd, AxI_J);

#if EBUG || LOG || LLOG || DYNAMIC_BEAM_MEASURES
  i = range2p (ij);
  j = range2q (ij);
#endif /* EBUG || LLOG || DYNAMIC_BEAM_MEASURES */

#if LOG
  fprintf (sxstdout, "*** Entering set_reduce_item_logprob (" SXILD "): ", item);
  output_item (item, NULL, i, j);
#endif /* LOG */

#if EBUG
  if (!SXBA_bit_is_set (AxI_J_set, AxI_J))
    sxtrap (ME, "set_reduce_item_logprob");
#endif /* EBUG */

  if (item == 0 || LISPRO (item-1) == 0) {
    /* item = A -> . */
#if EBUG
    if (i != j)
      sxtrap (ME, "set_reduce_item_logprob");
#endif /* EBUG */

    ei2set_logprob_ptr (item, ij, j_logprob_ptr);

    if (!is_ei_set)
      /* 1ere fois qu'on tombe dessus */
      *j_logprob_ptr = nul_logprob;
  }
  else
    ei2get_logprob_ptr (item, ij, j_logprob_ptr);

  outside_logprob = max_logprob_ptr->outside + prod2logprob (PROLIS (item));
  total_logprob = outside_logprob + j_logprob_ptr->inside.left;
  
  if (SXBA_bit_is_set (ei_set, ei)) { /* on avait déjà réduit sur cet item */
    if (cmp_SXLOGPROB (j_logprob_ptr->outside, outside_logprob) < 0 /* < */) {
      j_logprob_ptr->outside = outside_logprob;
      j_logprob_ptr->expected_total = total_logprob;
      changed_j_logprob = 1;
    }
#if LLOG
    else {
      changed_j_logprob = -1;
      local_logprob = *j_logprob_ptr;

      printf ("\t<< [" SXILD ".." SXILD "]: This previous reduce logprob item ", i, j);
      output_item (item, NULL, i, j);

      j_logprob_ptr->outside = outside_logprob;
      j_logprob_ptr->expected_total = total_logprob;

      fputs ("\t\tsubsumes ", stdout);
      output_item (item, NULL, i, j);

      *j_logprob_ptr = local_logprob;
    }
#endif /* LLOG */    
  }
  else {
    if (cmp_SXLOGPROB (total_logprob, min_acceptable_tree_logprob) >= 0) {
#ifdef DYNAMIC_BEAM_MEASURES
      if (reducer_beam_value_is_set
	  && (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)) {
	/* on stocke les logprobs venant du reconaisseur avant qu'elles ne soient écrasées (améliorées!) par le reducer */
	SXINT prod = PROLIS (item);
	if (item > PROLON (prod)) { /* sinon on est sur un item de la forme A -> . \alpha */
	  if (LISPRO (item - 1) < 0) {/* on est donc sur un item post-scan ( = de la forme A -> \alpha t . \beta ) */
	    if (recognizer_scan_beam_value_is_set
		&& (DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [j] == 0 
		    || cmp_SXLOGPROB (j_logprob_ptr->expected_total, DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [j]) < 0))
	      DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [j] = j_logprob_ptr->expected_total;
	  }
	  else {
	    if (recognizer_shift_beam_value_is_set
		&& (DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob [j] == 0 
		    || cmp_SXLOGPROB (j_logprob_ptr->expected_total, DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob [j]) < 0))
	      DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob [j] = j_logprob_ptr->expected_total;
	  }
	}
      }
#endif /* DYNAMIC_BEAM_MEASURES */
      j_logprob_ptr->outside = outside_logprob;
      j_logprob_ptr->inside.right = 0;
      j_logprob_ptr->expected_total = total_logprob;
      changed_j_logprob = 1;
      SXBA_1_bit (ei_set, ei);
    }
    else {
      changed_j_logprob = -1;
#if LLOG
      local_logprob = *j_logprob_ptr;

      j_logprob_ptr->outside = outside_logprob;
      j_logprob_ptr->expected_total = total_logprob;

      printf ("Reduce item beamed out for indexes [" SXILD ".." SXILD "] since " SXLPD " < " SXLPD ": ", i, j,
	      SXLOGPROB_display (total_logprob),
	      SXLOGPROB_display (min_acceptable_tree_logprob));
      output_item (item, NULL, i, j);

      *j_logprob_ptr = local_logprob;
#endif /* LLOG */
    }
  }

#if LLLOG
  printf ("Made and filled ei: (item=" SXILD ", pq=" SXILD ") [ei=" SXILD " (%s)] {outside = " SXLPD ", inside.left = " SXLPD ", inside_right = " SXLPD ", expected_total = " SXLPD "}: ", item, ij, ei, is_ei_set ? "old"  : "new", SXLOGPROB_display (j_logprob_ptr->outside), SXLOGPROB_display (j_logprob_ptr->inside.left), SXLOGPROB_display (j_logprob_ptr->inside.left), SXLOGPROB_display (j_logprob_ptr->expected_total));
	output_ei (ei);
#endif /* LLLOG */

#if EBUG
  if (changed_j_logprob > 0) {
    check_logprob (j_logprob_ptr, item, i, j);	      
  }
#if LLOG
  else if (changed_j_logprob == 0) {
    sxtrap (ME, "set_reduce_item_logprob (failed to examine item)");
  }
#endif /* LLOG */
#endif /* EBUG */

#if LOG
  if (changed_j_logprob > 0) {
    printf ("Reduce item kept for indexes [" SXILD ".." SXILD "] since " SXLPD " >= " SXLPD ": ", i, j,
	    SXLOGPROB_display (j_logprob_ptr->expected_total),
	    SXLOGPROB_display (min_acceptable_tree_logprob));
    output_item (item, NULL, i, j);
  }
#endif /* LOG */

#if LOG
  fprintf (sxstdout, "*** Leaving set_reduce_item_logprob (" SXILD ") with exit value " SXILD "\n", item, changed_j_logprob);
#endif /* LOG */

  return changed_j_logprob;
}
#endif /* REDUCER_KIND */


#if HAS_BEAM == 2

/* Le 27/07/10 Il ne semble pas possible de beamer ds les tables valides et reduites*/

/* BEAM OUT
   ATTENTION :
   Soit ds T [i] les items 
   {i1 = [B -> \beta . A  t2 \gamma]
    i2 = [C -> \delta . A \lambda]
    i3 = [A -> . t1 ]
    i4 = [A -> . t1 t2]} tous valides
   et ds T [j=i+1] les items 
   {i1+1 = [B -> \beta A . t2 \gamma]
    i2+1 = [C -> \delta A . \lambda]
    i3+1 = [A -> t1 .]
    i4+1 = [A -> t1 . t2]} tous valides
   Supposons de plus que le meilleur contexte de beam pour A ds T [i] soit celui de i2 et soit tj le threshold pour T[j] obtenu sur i3+1 (l'item i4+1 est donc elimine').
   On suppose de plus que P(i1) < tj < P(i2).
   On voit que ds T [j] lors du traitement de la reduction A -> t1
   il n'est pas possible de supprimer i1+1 au pretexte que ds T [i] la proba de i1 P(i1) est < a tj car ca supprimerait
   le seul chemin valide.

   EN fait, on ne peut meme pas beamer sur les items du kernel
   Soit la grammaire P = {
                 r1: S -> t1 . A t3, -0.1
		 r2: S -> t1 . A,    -0.9
		 r3: A -> . t2 t3,   -0.4
		 r4: A -> . t2,      -0.6
		 }

		 On suppose que la logprob des terminaux est nulle. Soit le source t1 t2 t3.
		 On a les 2 derivations valides
                   r1         r4
		 S => t1 A t3 => t1 t2 t3 (p1+p4 = -0.7)
		 et
		   r2      r3
		 S => t1 A => t1 t2 t3    (p2+p3 = -1.3)

		 Si on fait du 1-best exact, la 1ere est donc choisie

		 Ensemble des tables valides reduites avec les logprobs estimees <prefixe, inside.left, inside.right, prod2logprob, outside>.
		 La meilleure proba pour A est -0.4 et pour S est -0.5
                          
   Soit T [1] = {
                 i0 = [S' -> $ . S $, 0],  <0, 0, -0.5, 0, 0> = -0.5
                 i1 = [S -> . t1 A t3, 1], <0, 0, -0.4, -0.1, 0> = -0.5
		 i2 = [S -> . t1 A, 1],    <0, 0, -0.4, -0.9, 0> = -1.3
		}
                          
   Soit T [2] = {
                 i'1 = [S -> t1 . A t3, 1], <0, 0, -0.4, -0.1, 0> = -0.5
		 i'2 = [S -> t1 . A, 1],    <0, 0, -0.4, -0.9, 0> = -1.3           **** BEAMED OUT ****
		 i'3 = [A -> . t2 t3, 2],   <0, 0, 0, -0.4, -0.1> = -0.5
		 i'4 = [A -> . t2, 2],      <0, 0, 0, -0.6, -0.1> = -0.7
		}

   Soit T [3] = {
                 i''1 = [S -> t1 A . t3, 1], <0, -0.6, 0, -0.1, 0> = -0.8           **** BEAMED OUT ****
		 i''2 = [S -> t1 A ., 1],    **** Non reduit ****
		 i''3 = [A -> t2 . t3, 2],   <0, 0, 0, -0.4, -0.1> = -0.5
		 i''4 = [A -> t2 ., 2],      <0, 0, 0, -0.6, -0.1> = -0.7           **** BEAMED OUT ****
		}

   Soit T [4] = {
                 i'''0 = [S' -> $ S . $],     <0, -0.7, 0, 0, 0> = -0.7              **** BEAMED OUT ****
                 i'''1 = [S -> t1 A t3 ., 1], <0, -0.6, 0, -0.1, 0> = -0.7           **** BEAMED OUT ****
                 i'''1' = [S -> t1 A . t3, 1], **** Non reduit ****
		 i'''2 = [S -> t1 A ., 1],    <0, -0.4, 0, -0.9, 0> = -1.3           **** BEAMED OUT ****
		 i'''3 = [A -> t2 t3 ., 2],   <0, 0, 0, -0.4, -0.1> = -0.5
		}

		Remarque : En T [3] l'item i''2 n'appartient pas aux tables reduites de meme qu'en T [4] l'item i'''1'
		S'il n'y avait aucun beam, les logprobs completes estimees seraient celles indiquees
		Maintenant, on fait du beam avec une plage nulle sur les items du kernel.

		Dans T [2] on a P(i'2)=-1.3 < P(i'1)=-0.5, l'item du kernel i'2 est donc supprime' de T [2], les autres items ainsi que leur proba sont inchangees.
		Dans T [3] on a P(i''4)=-0.7 < P(i''3)=-0.5, l'item du kernel i''4 est donc supprime'.  C'est aussi le seul item reduce qui produit l'item i''1
		   qui est donc aussi supprime'.
		Dans T [4] i'''1 est supprime' car i''1 l'a ete ds T [3], i'''2 l'est egalement car i'2 l'est ds T [2].  Finalement i'''0 l'est egalement car il
		   n'y a plus aucune reduction vers S.
		Il y a donc ECHEC de l'analyse !!

		Question peut-on ne tenir compte que de la proba prefixe+inside.left[+prod2logprob] ?? Non, pas + (l'exemple ci-dessus le montre e'galement)
*/

static SXBA              tbp_valid_item_set;

static SXBA              *il_A2index_set, *il_Ai2prod_set, il_A_set;
static SXINT             *il_A2status, *il_Ai2status, *il_A_stack, *il_Ai_stack;

#define il_nyp           0
#define il_true          -1
#define il_false         -2
#define il_in_progress   -3

#define il_Axi2Ai(A,i)   ((idag.final_state+1)*(A)+(i))

static SXINT                    *il_ei2status, *il_ei2status_stack;
#define il_itemxi2itemi(it,i)   ((idag.final_state+1)*(it)+(i))



static void il_set_Aij_inside_logprob (SXINT A, SXINT h, SXINT j);

/*
  Pour une LHS A on considere tous les items earley A_reduce A -> [i] \alpha [j] d'une table T [j].  Ces items sont classes par ordre croissant.
  On a A -> [i] \alpha [j] < A -> [i'] \alpha' [j] <=> i <= i'
  Pour evaluer les proba inside des Aij il faut trouver un ordre d'evaluation des logprobs inside.left des items earley A_reduce.
  Le pb vient du fait que A peut etre cyclique
  On evalue la proba inside.left des items earley A_reduce par valeur decroissante de i a partir de j.
  Pour un i donne', l'evaluation de A -> [i] \alpha [j] ne peut pas dependre de l'inside proba des Ai'j avec i' < i.
  Si elle depend d'Ai''j avec i'' > i, ces logprobs sont deja evaluees.
  Le probleme vient d'une eventuelle dependance viv-a-vis de l'inside proba de Aij lui-meme.
  Or, par construction (on a une bonne foret d'analyse), il existe au moins un item reduce earley A -> [i] \alpha' [j] dont le calcul de sa
  proba inside.left ne depend pas de Aij.  Une inside proba (partielle) peut donc etre calculee pour Aij.  Or les autres inside proba des Aij
  qui dependent de celles la ne peuvent pas etre meilleures.  Donc, pour un i donne', une premiere passe permet de calculer l'inside proba de Aij
  et une deuxieme passe (eventuelle) permet de calculer les inside.left proba des items earley qui dependent de Aij.
 */

static struct logprob_struct*
set_inside_left_logprob (SXINT item_j, SXINT i, SXINT j)
{
  SXINT                     ij, item_k, X, item_i, k, kj, t, ik, iti, prod, status, Xk, Xk_status;
  SXLOGPROB                    Xkj_inside_logprob, ikj_inside_logprob, ij_inside_logprob;
  SXBA                      index_set_j, backward_index_set_j;
  struct logprob_struct       *ij_logprob_ptr, *ik_logprob_ptr;
  SXBOOLEAN                 is_item_k_an_init_item, is_first;

  prod = PROLIS (item_j);
  item_i = PROLON (prod);
  iti = il_itemxi2itemi (item_j, i);
  status = il_ei2status [iti];

#if LOG
  printf ("\n******** Entering set_inside_left_logprob (item_j=" SXILD ", i=" SXILD ", j=" SXILD ") (STATUS=%s) ********\n", item_j, i, j, status==il_nyp ? "1st time" : (status==il_in_progress) ? "recursive" : "already processed");
  output_rcvr_prod (item_i, i, item_j, j);
  fputs ("\n", stdout);
#endif /* LOG */

  ij_logprob_ptr = (struct logprob_struct*)NULL;

  if (status != il_nyp && status != il_in_progress) {
    if (status > 0)
      /* OK */
      ij_logprob_ptr = &(logprobs [status]);
  }
  else {
    if (status == il_nyp) {
      /* 1ere fois qu'on examine [i] item_j [j] */
      /* sinon, appel recursif */
      il_ei2status [iti] = il_in_progress;
      PUSH (il_ei2status_stack, iti); /* pour RAZ */
    }

    ij = p_q2pq (i, j);
    item_k = item_j-1;
    X = LISPRO (item_k);

    if (item_j == 1 || X == 0) {
      is_first = SXFALSE;
      ij_inside_logprob = 0.;
    }
    else {
      /* A -> [i] \alpha X [j] \beta */
#if EBUG
      index_set_j = PT2_save [j].index_sets [item_j];

      if (!SXBA_bit_is_set (index_set_j, i))
	sxtrap (ME, "set_inside_left_logprob");
#endif /* EBUG */

      is_item_k_an_init_item = LISPRO (item_k-1) == 0;

      if (X > 0) {
	il_set_Aij_inside_logprob (X, i, j);
	/* Ici, on est sur que tous les Xkj (i <= k <= j) sont evalues */
      }

      backward_index_set_j = PT2_save [j].backward_index_sets [item_j];

      is_first = SXTRUE;
      k = i-1;

      while ((k = sxba_scan (backward_index_set_j, k)) >= 0) {
	if ((is_item_k_an_init_item && i == k)
	    ||
	    (!is_item_k_an_init_item && SXBA_bit_is_set (PT2_save [k].index_sets [item_k], i))
	    ) {
	  if (X > 0) {
	    Xk = il_Axi2Ai (X, k);
	    Xk_status = il_Ai2status [Xk];

#if EBUG
	    if (Xk_status <= 0)
	      sxtrap (ME, "set_inside_left_logprob");
#endif /* EBUG */

	    Xkj_inside_logprob = AxI_J_logprobs [Xk_status].inside;
#if LOG
	    printf ("Get Inside_Logprob (<%s>[" SXILD ":" SXILD "]) = " SXLPD "\n", NTSTRING (X), k, j, SXLOGPROB_display (Xkj_inside_logprob));
#endif /* LOG */
	  }
	  else {
	    t = -X;
#ifdef MAKE_INSIDEG
	    t = spf.insideG.t2init_t [t];
#endif /* MAKE_INSIDEG */
	    kj = p_q2pq (k, j);
	    Xkj_inside_logprob = pq_t2logprob (kj, t);
#if LOG
	    printf ("Get Inside_Logprob (\"%s\"[" SXILD ":" SXILD "]) = " SXLPD "\n", TSTRING (-X), k, j, SXLOGPROB_display (Xkj_inside_logprob));
#endif /* LOG */
	  }

	  if (k == j) {
	    /* item_k ij peut ne pas etre encore calcule' */
	    ik_logprob_ptr = set_inside_left_logprob (item_k, i, k);
	  }
	  else {
	    ei2get_logprob_ptr (item_k, p_q2pq (i, k), ik_logprob_ptr); /* Deja calculee, et beam-proof */
	  }

	  ikj_inside_logprob = ik_logprob_ptr->inside.left + Xkj_inside_logprob;

	  if (is_first || (ikj_inside_logprob >/* cmp_SXLOGPROB pas necessaire */ ij_inside_logprob)) {
	    is_first = SXFALSE;
	    ij_inside_logprob = ikj_inside_logprob;
	  }
	}
      }

#if EBUG
      if (is_first)
	sxtrap (ME, "set_inside_left_logprob");
#endif /* EBUG */
    }
    
    if (is_first)
      il_ei2status [iti] = il_false;
    else {
      ei2set_logprob_ptr (item_j, ij, ij_logprob_ptr);
      ij_logprob_ptr->inside.left = ij_inside_logprob;
      il_ei2status [iti] = ei;

#if LLLOG
      printf ("Made and filled ei: (item=" SXILD ", pq=" SXILD ") [ei=" SXILD " (%s)] {inside.left = " SXLPD "}: ", item_j, ij, ei, is_ei_set ? "old"  : "new", SXLOGPROB_display (j_logprob_ptr->inside.left));
	output_ei (ei);
#endif /* LLLOG */
    }
  }

#if LOG
  printf ("******** Leaving set_inside_left_logprob (item_j=" SXILD ", i=" SXILD ", j=" SXILD ")=" SXLPD " (%s) ********\n", item_j, i, j, ij_logprob_ptr ? SXLOGPROB_display (ij_logprob_ptr->inside.left) : 0., ij_logprob_ptr ? "SUCCEEDS" : "FAILS");
  output_rcvr_prod (item_i, i, item_j, j);
  fputs ("\n", stdout);
#endif /* LOG */

  return ij_logprob_ptr;
}


/* On positionne les inside logprobs des Aij pour tous les i t.q. h <= i <= j */
static void
il_set_Aij_inside_logprob (SXINT A, SXINT h, SXINT j)
{
  SXINT                     A_status, Aij_status, i, prod, item_j, ij, Ai, A0j, A0j_status;
  SXINT                     *A_status_ptr, *Aij_status_ptr, *A0j_status_ptr;
  SXBA                      index_set, prod_set;
  SXLOGPROB                    cur_Aij_inside_logprob;
  struct logprob_struct       *ij_logprob_ptr;

  A_status_ptr = &(il_A2status [A]);
  A_status = *A_status_ptr;

#if LOG
  printf ("******** Entering il_set_Aij_inside_logprob (A=" SXILD ", i=" SXILD ", j=" SXILD ") (A=<%s>) %s ********\n", A, h, j, NTSTRING (A),
	  A_status == il_nyp ? "FIRST CALL" : "NEXT CALL");
#endif /* LOG */

  if (A_status == il_nyp || A_status == il_in_progress) {
    /* Non Completement) Evalue */
    A0j = il_Axi2Ai (A, 0);
    A0j_status_ptr = &(il_Ai2status [A0j]);
    A0j_status = *A0j_status_ptr;

    if (A_status == il_nyp) {
      /* 1ere fois qu'on appelle l'evaluation de A */
      /* On change son statut pour les appels recursifs eventuels */
      *A_status_ptr = il_in_progress;
      *A0j_status_ptr = il_false; /* a priori */
    }

    index_set = il_A2index_set [A];
    i = j+1;

    /* Les i sont parcourus par index decroissants */
    while ((i = sxba_1_rlscan (index_set, i)) >= h) {
      ij = p_q2pq (i, j);
      Ai = il_Axi2Ai (A, i);
      Aij_status_ptr = &(il_Ai2status [Ai]);
      Aij_status = *Aij_status_ptr;

      if (Aij_status == il_nyp)
	/* Ici et maintenant on debute le traitement de Aij */
	*Aij_status_ptr = il_in_progress;

      prod_set = il_Ai2prod_set [Ai];
      prod = -1;

      while ((prod = sxba_scan_reset (prod_set, prod)) >= 0) {
	item_j = PROLON (prod+1)-1;
	
	if ((ij_logprob_ptr = set_inside_left_logprob (item_j, i, j))) {
	  /* On peut donc renseigner Aij  */
	  cur_Aij_inside_logprob = ij_logprob_ptr->inside.left + prod2logprob (prod);

	  if ((AxI_J = *Aij_status_ptr) == il_in_progress) {
	    /* 1ere fois */
	    Axpq2set_max_logprob_ptr (A, ij);
	    max_logprob_ptr->inside = cur_Aij_inside_logprob;
	    *Aij_status_ptr = AxI_J;
	    *A0j_status_ptr = il_true;
	  }
	  else {
	    max_logprob_ptr = &(AxI_J_logprobs [AxI_J]);

	    if (max_logprob_ptr->inside < cur_Aij_inside_logprob /* pas necessaire cmp_SXLOGPROB */)
	      max_logprob_ptr->inside = cur_Aij_inside_logprob;
	  }
	}
      }

      /* Ici on a termine' l'evaluation des Aij */
      if (Aij_status == il_nyp) {
	if (*Aij_status_ptr == il_in_progress)
	  *Aij_status_ptr = il_false; /* ... et aucun n'est beam proof */
      }
      
      SXBA_0_bit (index_set, i); /* Il peut etre deja  a zero du fait des appels recursifs */
    }

    if (i == -1) {
      /* Ici l'evaluation de tous les Aij est terminee */
      *A_status_ptr = *A0j_status_ptr;
    }
  }

#if LOG
  A_status = *A_status_ptr;
  printf ("******** Leaving il_set_Aij_inside_logprob (A=" SXILD ", i=" SXILD ", j=" SXILD ") (A=<%s>) %s ********\n", A, h, j, NTSTRING (A),
	  A_status == il_nyp ? "FIRST TIME" : (A_status == il_in_progress ? "UNCOMPLETED" : ((A_status == il_false ? "COMPLETED (FALSE)" : "COMPLETED (TRUE)"))));
#endif /* LOG */
}


#ifdef CONTROLLED_BEAM_H
struct sxba_stack {
  SXINT top, size;
  SXBA  *stack;
};

#define SXBA_STACK_ALLOC(hd,size)  (hd.stack = (SXBA *) sxalloc ((hd.size = size)+1, sizeof (SXBA)), hd.top = 0)
#define SXBA_STACK_REALLOC(hd)     (hd.stack = (SXBA *) sxrealloc (hd.stack, (hd.size *= 2)+1, sizeof (SXBA)))
#define SXBA_STACK_FREE(hd)        (sxfree (hd.stack), hd.stack = NULL);
#define SXBA_STACK_PUSH (hd,set)   (((++hd.top > hd.size) ? SXBA_STACK_REALLOC (hd) : NULL), hd.stack [hd.top] = set)
#define SXBA_STACK_POP (hd)        (hd.stack [--hd.top])
#define SXBA_STACK_IS_EMPTY (hd)   (hd.top == 0)
#define SXBA_STACK_TOP (hd)        (hd.top)

static SXBOOLEAN            must_perform_a_controlled_beam, has_hit_a_valid_terminal_item;

static struct logprob_struct* set_inside_left_controlled_logprob (SXINT item_j, SXINT i, SXINT j);

/* On positionne les inside logprobs des Aij pour tous les i t.q. h <= i <= j */
static void
il_set_Aij_inside_controlled_logprob (SXINT A, SXINT h, SXINT j)
{
  SXINT                     A_status, Aij_status, i, prod, item_j, ij, Ai, A0j, A0j_status;
  SXINT                     *A_status_ptr, *Aij_status_ptr, *A0j_status_ptr;
  SXBA                      index_set, prod_set;
  SXLOGPROB                    cur_Aij_inside_logprob;
  struct logprob_struct       *ij_logprob_ptr;

  A_status_ptr = &(il_A2status [A]);
  A_status = *A_status_ptr;

#if LOG
  printf ("******** Entering il_set_Aij_inside_controlled_logprob (A=" SXILD ", i=" SXILD ", j=" SXILD ") (A=<%s>) %s ********\n", A, h, j, NTSTRING (A),
	  A_status == il_nyp ? "FIRST CALL" : "NEXT CALL");
#endif /* LOG */

  if (A_status == il_nyp || A_status == il_in_progress) {
    /* Non (completement) Evalue */
    A0j = il_Axi2Ai (A, 0);
    A0j_status_ptr = &(il_Ai2status [A0j]);
    A0j_status = *A0j_status_ptr;

    if (A_status == il_nyp) {
      /* 1ere fois qu'on appelle l'evaluation de A */
      /* On change son statut pour les appels recursifs eventuels */
      *A_status_ptr = il_in_progress;
      *A0j_status_ptr = il_false; /* a priori */
    }

    index_set = il_A2index_set [A];
    i = j+1;

    /* Les i sont parcourus par index decroissants */
    while ((i = sxba_1_rlscan (index_set, i)) >= h) {
      ij = p_q2pq (i, j);
      Ai = il_Axi2Ai (A, i);
      Aij_status_ptr = &(il_Ai2status [Ai]);
      Aij_status = *Aij_status_ptr;

      if (Aij_status == il_nyp)
	/* Ici et maintenant on debute le traitement de Aij */
	*Aij_status_ptr = il_in_progress;

      prod_set = il_Ai2prod_set [Ai];
      prod = -1;

      while ((prod = sxba_scan_reset (prod_set, prod)) >= 0) {
	item_j = PROLON (prod+1)-1;
	
	if ((ij_logprob_ptr = set_inside_left_controlled_logprob (item_j, i, j))) {
	  /* On peut donc renseigner Aij  */
	  cur_Aij_inside_logprob = ij_logprob_ptr->inside.left + prod2logprob (prod);

	  if ((AxI_J = *Aij_status_ptr) == il_in_progress) {
	    /* 1ere fois */
	    Axpq2set_max_logprob_ptr (A, ij);
	    max_logprob_ptr->inside = cur_Aij_inside_logprob;
	    *Aij_status_ptr = AxI_J;
	    *A0j_status_ptr = il_true;
	  }
	  else {
	    max_logprob_ptr = &(AxI_J_logprobs [AxI_J]);

	    if (max_logprob_ptr->inside < cur_Aij_inside_logprob /* pas necessaire cmp_SXLOGPROB */)
	      max_logprob_ptr->inside = cur_Aij_inside_logprob;
	  }
	}
      }

      /* Ici on a termine' l'evaluation des Aij */
      if (Aij_status == il_nyp) {
	if (*Aij_status_ptr == il_in_progress)
	  *Aij_status_ptr = il_false; /* ... et aucun n'est beam proof */
      }
      
      SXBA_0_bit (index_set, i); /* Il peut etre deja  a zero du fait des appels recursifs */
    }

    if (i == -1) {
      /* Ici l'evaluation de tous les Aij est terminee */
      *A_status_ptr = *A0j_status_ptr;
    }
  }

#if LOG
  A_status = *A_status_ptr;
  printf ("******** Leaving il_set_Aij_inside_controlled_logprob (A=" SXILD ", i=" SXILD ", j=" SXILD ") (A=<%s>) %s ********\n", A, h, j, NTSTRING (A),
	  A_status == il_nyp ? "FIRST TIME" : (A_status == il_in_progress ? "UNCOMPLETED" : ((A_status == il_false ? "COMPLETED (FALSE)" : "COMPLETED (TRUE)"))));
#endif /* LOG */
}


static struct logprob_struct*
set_inside_left_controlled_logprob (SXINT item_j, SXINT i, SXINT j)
{
  SXINT                     ij, item_k, X, item_i, k, kj, t, ik, iti, prod, status, Xk, Xk_status;
  SXLOGPROB                    Xkj_inside_logprob, ikj_inside_logprob, ij_inside_logprob;
  SXBA                      index_set_j, index_set_k, backward_index_set_j, new_index_set_j, new_backward_index_set_j;
  struct logprob_struct       *ij_logprob_ptr, *ik_logprob_ptr;
  SXBOOLEAN                 is_item_k_an_init_item, is_first;

  prod = PROLIS (item_j);
  item_i = PROLON (prod);
  iti = il_itemxi2itemi (item_j, i);
  status = il_ei2status [iti];

#if LOG
  printf ("\n******** Entering set_inside_left_controlled_logprob (item_j=" SXILD ", i=" SXILD ", j=" SXILD ") (STATUS=%s) ********\n", item_j, i, j, status==il_nyp ? "1st time" : (status==il_in_progress) ? "recursive" : "already processed");
  output_rcvr_prod (item_i, i, item_j, j);
  fputs ("\n", stdout);
#endif /* LOG */

  ij_logprob_ptr = (struct logprob_struct*)NULL;

  if (status != il_nyp && status != il_in_progress) {
    if (status > 0)
      /* OK */
      ij_logprob_ptr = &(logprobs [status]);
  }
  else {
    if (status == il_nyp) {
      /* 1ere fois qu'on examine [i] item_j [j] */
      /* sinon, appel recursif */
      il_ei2status [iti] = il_in_progress;
      PUSH (il_ei2status_stack, iti); /* pour RAZ */
    }

    ij = p_q2pq (i, j);
    item_k = item_j-1;
    X = LISPRO (item_k);

    if (item_j == 1 || X == 0) {
      is_first = SXFALSE;
      ij_inside_logprob = 0.;

      if (must_perform_a_controlled_beam && item_j == 1) {
#if EBUG
	if (i != 0 || j != 1)
	  sxtrap (ME, "set_inside_left_controlled_logprob");
#endif /* EBUG */

	new_index_set_j = PT2 [1].index_sets [1] = sxba_bag_get (&pt2_bag, 1+1);
	new_backward_index_set_j = PT2 [1].backward_index_sets [1] = sxba_bag_get (&pt2_bag, 1+1);
	SXBA_1_bit (new_index_set_j, 0);
	SXBA_1_bit (new_backward_index_set_j, 0);
      }
    }
    else {
      /* A -> [i] \alpha X [j] \beta */
#if EBUG
      index_set_j = PT2_save [j].index_sets [item_j];

      if (!SXBA_bit_is_set (index_set_j, i))
	sxtrap (ME, "set_inside_left_controlled_logprob");
#endif /* EBUG */

      is_item_k_an_init_item = LISPRO (item_k-1) == 0;

      if (X > 0) {
	il_set_Aij_inside_controlled_logprob (X, i, j);
	/* Ici, on est sur que tous les Xkj (i <= k <= j) sont evalues */
      }

      backward_index_set_j = PT2_save [j].backward_index_sets [item_j];
      new_index_set_j = PT2 [j].index_sets [item_j];
      new_backward_index_set_j = PT2 [j].backward_index_sets [item_j];

      is_first = SXTRUE;
      k = i-1;

      while ((k = sxba_scan (backward_index_set_j, k)) >= 0) {
	if (is_item_k_an_init_item && i == k
	    || !is_item_k_an_init_item &&
	    (k == j && SXBA_bit_is_set (PT2_save [k].index_sets [item_k], i)
	     || k < j && (index_set_k = PT2 [k].index_sets [item_k] /* non vide */) && SXBA_bit_is_set (index_set_k, i))
	     ) {
	  if (X > 0) {
	    Xk = il_Axi2Ai (X, k);
	    Xk_status = il_Ai2status [Xk];

	    if (Xk_status > 0)
	      Xkj_inside_logprob = AxI_J_logprobs [Xk_status].inside;

#if LOG
	    if (Xk_status > 0)
	      printf ("Get Inside_Logprob (<%s>[" SXILD ":" SXILD "]) = " SXLPD "\n", NTSTRING (X), k, j, SXLOGPROB_display (Xkj_inside_logprob));
	    else
	      printf ("Get Inside_Logprob (<%s>[" SXILD ":" SXILD "]) = beamed out\n", NTSTRING (X), k, j);
#endif /* LOG */
	  }
	  else {
	    Xk_status = 1;
	    t = -X;
#ifdef MAKE_INSIDEG
	    t = spf.insideG.t2init_t [t];
#endif /* MAKE_INSIDEG */
	    kj = p_q2pq (k, j);
	    Xkj_inside_logprob = pq_t2logprob (kj, t);
#if LOG
	    printf ("Get Inside_Logprob (\"%s\"[" SXILD ":" SXILD "]) = " SXLPD "\n", TSTRING (-X), k, j, SXLOGPROB_display (Xkj_inside_logprob));
#endif /* LOG */
	  }

	  if (Xk_status > 0) {
	    if (k == j) {
	      /* item_k ij peut ne pas etre encore calcule' */
	      ik_logprob_ptr = set_inside_left_controlled_logprob (item_k, i, k);
	    }
	    else {
	      ei2get_logprob_ptr (item_k, p_q2pq (i, k), ik_logprob_ptr); /* Deja calculee, et beam-proof */
	    }

	    if (ik_logprob_ptr) {
	      ikj_inside_logprob = ik_logprob_ptr->inside.left + Xkj_inside_logprob;

	      if (must_perform_a_controlled_beam && check_controlled_beam_proof (item_j, ij, ikj_inside_logprob)) {
		/* beam_proof, le chemin ikj est donc controlle */
		if (new_index_set_j == NULL) {
		  new_index_set_j = PT2 [j].index_sets [item_j] = sxba_bag_get (&pt2_bag, j+1);
		  new_backward_index_set_j = PT2 [j].backward_index_sets [item_j] = sxba_bag_get (&pt2_bag, j+1);
		}

		if (is_first)
		  SXBA_1_bit (new_index_set_j, i);

		SXBA_1_bit (new_backward_index_set_j, k);

		if (is_first || (ikj_inside_logprob >/* cmp_SXLOGPROB pas necessaire */ ij_inside_logprob)) {
		  /* ... et pour l'instant, ;e meilleur */
		  is_first = SXFALSE;
		  ij_inside_logprob = ikj_inside_logprob;
		}
	      }
	    }
	  }
	}
      }
    }
    
    if (is_first)
      il_ei2status [iti] = il_false;
    else {
      ei2set_logprob_ptr (item_j, ij, ij_logprob_ptr);
      ij_logprob_ptr->inside.left = ij_inside_logprob;
      il_ei2status [iti] = ei;

#if LLLOG
      printf ("Made and filled ei: (item=" SXILD ", pq=" SXILD ") [ei=" SXILD " (%s)] {inside.left = " SXLPD "}: ", item_j, ij, ei, is_ei_set ? "old"  : "new", SXLOGPROB_display (j_logprob_ptr->inside.left));
	output_ei (ei);
#endif /* LLLOG */

      if (LISPRO (item_j) < 0)
	has_hit_a_valid_terminal_item = SXTRUE; /* La table T [j] est non vide (elle a un item scan) */
    }
  }

#if LOG
  printf ("******** Leaving set_inside_left_controlled_logprob (item_j=" SXILD ", i=" SXILD ", j=" SXILD ")=" SXLPD " (%s) ********\n", item_j, i, j, ij_logprob_ptr ? SXLOGPROB_display (ij_logprob_ptr->inside.left) : 0., ij_logprob_ptr ? "SUCCEEDS" : "FAILS");
  output_rcvr_prod (item_i, i, item_j, j);
  fputs ("\n", stdout);
#endif /* LOG */

  return ij_logprob_ptr;
}
#endif /* CONTROLLED_BEAM_H */



static void
prepare_inside_Akj_logprob (SXINT item_j, SXINT j)
{
  /* item_j = A -> \alpha [j] */
  SXINT prod, A, i, Ai;
  SXBA  index_set;

#if LOG
  printf ("Pushing reduce item " SXILD " ", item_j);
  output_item (item_j, PT2_save [j].index_sets [item_j], -1, j);
#endif /* LOG */

  prod = PROLIS (item_j);
  A = LHS (prod);

  if (SXBA_bit_is_reset_set (il_A_set, A)) {
    /* 1ere A_prod */
    PUSH (il_A_stack, A);
    Ai = il_Axi2Ai (A, 0);
    PUSH (il_Ai_stack, Ai);
  }

  if (PROLON (prod) == item_j) {
    /* A -> . */
    SXBA_1_bit (il_A2index_set [A], j);
    Ai = il_Axi2Ai (A, j);
    SXBA_1_bit (il_Ai2prod_set [Ai], prod);
    PUSH (il_Ai_stack, Ai);
  }
  else {
    index_set = PT2_save [j].index_sets [item_j];
    i = -1;

    while ((i = sxba_scan (index_set, i)) >= 0) {
      SXBA_1_bit (il_A2index_set [A], i);
      Ai = il_Axi2Ai (A, i);
      SXBA_1_bit (il_Ai2prod_set [Ai], prod);
      PUSH (il_Ai_stack, Ai);
    }
  }

#if 0
  item_j = PROLON (prod+1)-1; /* on saute prod */
#endif /* 0 */
}



static void
exact_inside_left_logprob_recognizer (void)
{
  SXINT                 A, i, j, item_j, prod, k, item_k, X, kj, k_logprob_changed, ij, x, j_logprob_changed, iti, Ai, pass_nb;
  SXINT                 *reduce_item_stack;
  SXBA                  valid_item_set, index_set_j, backward_index_set_j, index_set_k, Aij_set, new_index_set_j;
  SXBA                  *PT2j_index_set, *PT2j_backward_index_set, index_set, valid_triple_set;
  SXBOOLEAN             is_item_k_an_init_item, is_item_j_beam_proof;
  struct logprob_struct   *logprob_ptr;
#if is_rcvr
  SXINT                  resume, triple, kept_triple, top_item;
#endif /* is_rcvr */
  SXLOGPROB                 beta_max_inside_logprob, ij_inside_logprob, prod_logprob;
  struct logprob_struct    *ij_logprob_ptr, *ik_logprob_ptr;
  struct PT2_item	 *PT2j;

#if LOG
  fputs ("\n**** Entering exact_inside_left_logprob_recognizer ****\n", stdout);
#endif /* LOG */

#ifdef CONTROLLED_BEAM_H
  pass_nb = (idag.ptq_nb /* terminal_transition_nb */ >= terminal_transition_nb_trigger) ? 0 : 1;
#endif /* CONTROLLED_BEAM_H */

  PT2_save = PT2; /* exact_inside_left_logprob_recognizer () prend ses renseignements ds PT2_save ... */

#ifdef MAKE_INSIDEG
  PT2_area_save = PT2_area;
#endif /* MAKE_INSIDEG */

#ifdef CONTROLLED_BEAM_H
  if (pass_nb == 0)
    alloc_PT2 (); /* ... et ds ce cas on remplit PT2 */
#endif /* CONTROLLED_BEAM_H */
    
  /* Partie reconnaisseur */
  il_A2index_set = sxbm_calloc (MAXNT+1, idag.final_state+1);
  il_A2status = (SXINT *) sxcalloc (MAXNT+1, sizeof (SXINT));
  il_A_stack = (SXINT *) sxalloc (MAXNT+1, sizeof (SXINT)), il_A_stack [0] = 0; /* pour RAZ */
  il_A_set = sxba_calloc (MAXNT+1); /* pour RAZ */

  il_Ai2status = (SXINT *) sxcalloc ((MAXNT+1)*(idag.final_state+1)+1, sizeof (SXINT));
  il_Ai2prod_set = sxbm_calloc ((MAXNT+1)*(idag.final_state+1)+1, MAXPROD+1);
  il_Ai_stack = (SXINT *) sxalloc ((MAXNT+1)*(idag.final_state+1)+1, sizeof (SXINT)), il_Ai_stack [0] = 0; /* pour RAZ */

  il_ei2status = (SXINT *) sxcalloc ((MAXITEM+1)*(idag.final_state+1)+1, sizeof (SXINT));
  il_ei2status_stack = (SXINT *) sxalloc ((MAXITEM+1)*(idag.final_state+1)+1, sizeof (SXINT)), il_ei2status_stack [0] = 0;

  tbp_valid_item_set = sxba_calloc (MAXITEM+1);

#if is_rcvr
  if (is_error_detected && XxY_is_allocated (glbl_dag_rcvr_resume_kitems)) {
    if (nt2prod_that_generates_the_best_logprob_t_string == NULL) {
      /* pas calcule', on le fait */
      sxprepare_for_possible_reset (nt2prod_that_generates_the_best_logprob_t_string);
      nt2prod_that_generates_the_best_logprob_t_string = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT))
	, nt2prod_that_generates_the_best_logprob_t_string [0] = 0;
      fill_tnt_max_inside_logprobs (SXFALSE /* On ne remplit pas item2max_right_inside_logprob */);
    }
  }
#endif /* is_rcvr */

  /* initialisation des logprobs */
  ei2set_logprob_ptr (0 /* S' -> . $ S $ */, p_q2pq (0, 0), logprob_ptr);
  logprob_ptr->inside.left = 0;

#ifdef CONTROLLED_BEAM_H
  for (;;) {
    must_perform_a_controlled_beam = (++pass_nb == 1);

#if LOG
    printf ("\n**** Exact_inside_left_logprob_recognizer %s controlled beam ****\n",
	    must_perform_a_controlled_beam ? "with" : "without");
#endif /* LOG */
#endif /* CONTROLLED_BEAM_H */

    for (j = idag.init_state; j <= n+1; j++) {
#if LOG
      printf ("\n******************************* T [" SXILD "] *******************************\n", j);
#endif /* LOG */

      PT2j = &(PT2_save [j]);
      valid_item_set = i2valid_item_set [j];

      /* On cherche ds valid_item_set des items de la forme
	 A -> \alpha t . \beta
	 A -> \alpha [k] \beta [j] \gamma (de la rcvr)
	 A - > \alpha B . \beta
	 A - > \alpha .
      */

#ifdef CONTROLLED_BEAM_H
      has_hit_a_valid_terminal_item = SXFALSE;
#endif /* CONTROLLED_BEAM_H */

#if is_rcvr
      if (is_error_detected) {
	valid_triple_set = i2valid_triple_set [j];
	triple = -1;

	while ((triple = sxba_scan (valid_triple_set, triple)) > 0 ) {
	  /* C'est le bon ... */
	  k = XxYxZ_X (glbl_dag_rcvr_start_kitems, triple);
	  item_k = XxYxZ_Y (glbl_dag_rcvr_start_kitems, triple);
	  resume = XxYxZ_Z (glbl_dag_rcvr_start_kitems, triple);
	  item_j  = XxY_Y (glbl_dag_rcvr_resume_kitems, resume);

#if EBUG
	  if (j != XxY_X (glbl_dag_rcvr_resume_kitems, resume))
	    sxtrap (ME, "exact_inside_left_logprob_recognizer");
#endif /* EBUG */

	  /* A -> \alpha [k] \beta [j] \gamma */
	  beta_max_inside_logprob = get_max_inside_rcvr_logprob (item_k, item_j);
#if LOG
	  earley_glbl_rcvr_message (item_k, k, item_j, j, 1 /* warning */, SXFALSE);
#endif /* LOG */

	  X = LISPRO (item_j);

	  if (item_k == 0 || LISPRO (item_k-1) == 0) {
	    /* alpha est epsilon i == k */
	    /* A -> [k] \beta [j] \gamma */
	    ij = p_q2pq (k, j);
		
#ifdef CONTROLLED_BEAM_H
	    if (!must_perform_a_controlled_beam || check_controlled_beam_proof (item_j, ij, beta_max_inside_logprob))
#endif /* CONTROLLED_BEAM_H */
	      {
		if (!ei2set_logprob_ptr (item_j, ij, ij_logprob_ptr)
#ifdef CONTROLLED_BEAM_H
		    || !must_perform_a_controlled_beam /* ij_logprob_ptr a pu etre positionne' par la passe controlee qui a echouee */
#endif /* CONTROLLED_BEAM_H */
		    ) {
		  ij_logprob_ptr->inside.left = beta_max_inside_logprob;
		  iti = il_itemxi2itemi (item_j, k);
		  il_ei2status [iti] = ei;
		  PUSH (il_ei2status_stack, iti); /* pour RAZ */
#if LLOG
		  printf ("inside left logprob is " SXLPD " for the rcvr item (i=" SXILD ", item_k=" SXILD ", k=" SXILD ", item_j=" SXILD ", j=" SXILD ") ", SXLOGPROB_display (ij_logprob_ptr->inside.left), k, item_k, k, item_j, j);
		  output_item (item_j, NULL, k, j);
#endif /* LLOG */
		
#ifdef CONTROLLED_BEAM_H
		  if (must_perform_a_controlled_beam && LISPRO (item_j-1) != 0) {
		    /* \beta non vide */
		    new_index_set_j = PT2 [j].index_sets [item_j];
		
		    if (new_index_set_j == NULL)
		      new_index_set_j = PT2 [j].index_sets [item_j] = sxba_bag_get (&pt2_bag, j+1);

		    SXBA_1_bit (new_index_set_j, k);
		  }
#endif /* CONTROLLED_BEAM_H */

		  if (X == 0) {
		    prepare_inside_Akj_logprob (item_j, j);
		  }
#ifdef CONTROLLED_BEAM_H
		  else {
		    if (X < 0)
		      has_hit_a_valid_terminal_item = SXTRUE;
		  }
#endif /* CONTROLLED_BEAM_H */
		}
#if EBUG
		else
		  sxtrap (ME, "exact_inside_left_logprob_recognizer");
#endif /* EBUG */
	      }
	  }
	  else {
	    /* alpha est non vide */
	    index_set_k =  k == j ? PT2j->index_sets [item_k] : PT2 [k].index_sets [item_k];

	    if (index_set_k) {
	      index_set_j = PT2j->index_sets [item_j];
	      i = -1;

	      while ((i = sxba_scan (index_set_j, i)) >= 0 && i <= j) {
		if (SXBA_bit_is_set (index_set_k, i)) {
		  /* (item_k, i, k) est beam_proof */
		  ei2get_logprob_ptr (item_k, p_q2pq (i, k), ik_logprob_ptr);
		  ij_inside_logprob = ik_logprob_ptr->inside.left + beta_max_inside_logprob;
		  ij = p_q2pq (i, j);
		
#ifdef CONTROLLED_BEAM_H
		  if (!must_perform_a_controlled_beam || check_controlled_beam_proof (item_j, ij, ij_inside_logprob))
#endif /* CONTROLLED_BEAM_H */
		    {
		      if (!ei2set_logprob_ptr (item_j, ij, ij_logprob_ptr)
#ifdef CONTROLLED_BEAM_H
			  || !must_perform_a_controlled_beam /* ij_logprob_ptr a pu etre positionne' par la passe controlee qui a echouee */
#endif /* CONTROLLED_BEAM_H */
			  ) {
			ij_logprob_ptr->inside.left = ij_inside_logprob;
			iti = il_itemxi2itemi (item_j, i);
			il_ei2status [iti] = ei;
			PUSH (il_ei2status_stack, iti); /* pour RAZ */
#if LLOG
			printf ("inside left logprob is " SXLPD " for the rcvr item (i=" SXILD ", item_k=" SXILD ", k=" SXILD ", item_j=" SXILD ", j=" SXILD ") ", SXLOGPROB_display (ij_logprob_ptr->inside.left), i, item_k, k, item_j, j);
			output_item (item_j, NULL, i, j);
#endif /* LLOG */
		
#ifdef CONTROLLED_BEAM_H
			if (must_perform_a_controlled_beam) {
			  /* \beta non vide */
			  new_index_set_j = PT2 [j].index_sets [item_j];
		
			  if (new_index_set_j == NULL)
			    new_index_set_j = PT2 [j].index_sets [item_j] = sxba_bag_get (&pt2_bag, j+1);

			  SXBA_1_bit (new_index_set_j, i);
			}
#endif /* CONTROLLED_BEAM_H */

			if (X == 0) {
			  prepare_inside_Akj_logprob (item_j, j);
			}
#ifdef CONTROLLED_BEAM_H
			else {
			  if (X < 0)
			    has_hit_a_valid_terminal_item = SXTRUE;
			}
#endif /* CONTROLLED_BEAM_H */
		      }
#if EBUG
		      else
			sxtrap (ME, "exact_inside_left_logprob_recognizer");
#endif /* EBUG */
		    }
		}
	      }
	    }
	  }
	}
      }
#endif /* is_rcvr */

      item_j = 0;

      while ((item_j = sxba_scan (valid_item_set, item_j)) > 0) {
#if LOG
	output_item (item_j, PT2j->index_sets [item_j], -1, j);
#endif /* LOG */

	X = LISPRO (item_j);

	if (X == 0)
	  /* A -> \alpha . */
	  prepare_inside_Akj_logprob (item_j, j);
	else
	  SXBA_1_bit (tbp_valid_item_set, item_j);

#ifdef BEAM_MEASURE
	/* Le triplet (i, item_j, j) ou i \in [j].index_sets [item_j] est un item earley valide et reduit
	   on va noter (item_j, lgth(i, j)) pour la sortie finale des mesures */
#if is_rcvr
	if (!is_error_detected)
	  /* On ne prend pas de mesure si une erreur est detectee */
#endif /* is_rcvr */
	  put_a_valid_beam_measure (item_j, j);
#endif /* BEAM_MEASURE */
      }

      item_j = 0;

      while ((item_j = sxba_scan_reset (tbp_valid_item_set, item_j)) >= 0) {
	if (LISPRO (item_j-1) == 0) {
	  /* item_j = A -> . \beta */
#ifdef CONTROLLED_BEAM_H
	  if (must_perform_a_controlled_beam)
	    set_inside_left_controlled_logprob (item_j, j, j);
	  else
#endif /* CONTROLLED_BEAM_H */
	    set_inside_left_logprob (item_j, j, j);
	}
	else {
	  /* item_j = A -> \alpha X . \beta */
	  index_set_j = PT2j->index_sets [item_j];
	  i = -1;

	  while ((i = sxba_scan (index_set_j, i)) >= 0) {
#ifdef CONTROLLED_BEAM_H
	    if (must_perform_a_controlled_beam)
	      set_inside_left_controlled_logprob (item_j, i, j);
	    else
#endif /* CONTROLLED_BEAM_H */
	      set_inside_left_logprob (item_j, i, j);
	  }
	}
      }

      while (!IS_EMPTY (il_A_stack)) {
	A = POP (il_A_stack);
	SXBA_0_bit (il_A_set, A);
	il_A2status [A] = 0;
      }

      while (!IS_EMPTY (il_Ai_stack)) {
	Ai = POP (il_Ai_stack);
	il_Ai2status [Ai] = 0;

#if EBUG
	if (!sxba_is_empty (il_Ai2prod_set [Ai]))
	  sxtrap (ME, "exact_inside_left_logprob_recognizer");
#endif /* EBUG */
      }

      while (!IS_EMPTY (il_ei2status_stack))
	il_ei2status [POP (il_ei2status_stack)] = 0;

#ifdef CONTROLLED_BEAM_H
      if (must_perform_a_controlled_beam) {
	if (!has_hit_a_valid_terminal_item) {
	  /* Damned, une erreur a ete detectee ds cette phase sur la table T [j],
	     il faut recommencer sans faire de beam out. */
	  /* On remet en etat */
#if LLOG
	  printf ("Error detected in controlled beam during table T [" SXILD "], the inside left logprob process is restarted from the beginning without controlled beam.\n", j);
#endif /* LLOG */

	  /* on recommence a partir de PT2_save (sans rien stocker ds PT2) */
	  free_PT2 ();

	  PT2 = PT2_save;
#ifdef MAKE_INSIDEG
	  PT2_area = PT2_area_save;
#endif /* MAKE_INSIDEG */

	  break;
	}
      }
#endif /* CONTROLLED_BEAM_H */
    } /* for (j = idag.init_state; j <= n+1; j++)  */

#ifdef CONTROLLED_BEAM_H
    if (j > n+1)
      /* ca a marche' */
      break;
  } /* for (;;) */
#endif /* CONTROLLED_BEAM_H */

  sxfree (tbp_valid_item_set);

  sxbm_free (il_A2index_set), il_A2index_set = NULL;
  sxbm_free (il_Ai2prod_set), il_Ai2prod_set = NULL;
  sxfree (il_A2status), il_A2status = NULL;
  sxfree (il_Ai2status), il_Ai2status = NULL;
  sxfree (il_A_stack), il_A_stack = NULL;
  sxfree (il_A_set), il_A_set = NULL;
  sxfree (il_Ai_stack), il_Ai_stack = NULL;

  sxfree (il_ei2status), il_ei2status = NULL;
  sxfree (il_ei2status_stack), il_ei2status_stack = NULL;

#ifdef CONTROLLED_BEAM_H
  if (must_perform_a_controlled_beam) {
    /* Le resultat se trouve ds PT2 */
    free_PT2_save ();
  }
#endif /* CONTROLLED_BEAM_H */

#if LOG
  fputs ("\n**** Leaving exact_inside_left_logprob_recognizer ****\n", stdout);
#endif /* LOG */
}

#endif /* HAS_BEAM == 2 */

/* Cas is_dag */
static SXBOOLEAN
recognize (void)
{
  SXINT		i, j, p, q, r, s, first_rcvr_resuming_state, global_rcvr_resuming_state, last_global_rcvr_resuming_state = -1;
  SXBOOLEAN	is_in_LG, ret_val, used_beamrcvr = SXFALSE;
  SXBA		index_set, t_set, successful_p_set;
  SXINT         *sr_stack, *sr_ptr, global_beamrcvr_relaunches = 0, last_deadend_from_which_global_beamrcvr_was_launched = 0;

  pass_number = 1; /* reconnaisseur */

  mlstn_active_set = sxba_calloc (idag.final_state+1+1);
  successful_p_set = sxba_calloc (idag.final_state+1+1);
  dead_ends = sxba_calloc (idag.final_state+1+1);

  fill_states_in_all_paths_set();
  
  /* On commence par faire une transition sur le $ de [S' -> . $ S $] */
  build_initial_earley_table ();
  
  for (p = idag.init_state; p <= idag.final_state; p++) {
    if (SXBA_bit_is_set (successful_p_set, p) /* déjà construite successfully lors d'une précédente passe */
	|| build_earley_table (p) /* construite maintenant */
	) {
      /* p est accessible et on pourra aller au-delà de p */
      SXBA_1_bit (successful_p_set, p);
      last_global_rcvr_resuming_state = -1;
    }
    else {
      /* p n'est pas accessible ou alors il est accessible mais n'a pas de transition terminale valide (on ne pourra pas en ressortir)... */
      SXBA_1_bit (dead_ends, p);

#if is_beamrcvr
#if RECOGNIZER_KIND == 2
      if (SXBA_bit_is_set (idag.states_in_all_paths_set, p)) {
	// On va tenter de modifier temporairement le beam sur certains états pour réussir à continuer l'analyse
	if (can_try_recognizer_beamrcvr) {
	  // On va identifier des états de relance (<p), qui rempliront beam_rcvr_resuming_states, et qui sont définis
	  // comme suit:	  
	  // - ils ne sont séparés de p par aucun état "obligatoire" où la construction de la table earley a marché
	  //    (c'est un succès)
	  // - ils ont au moins un prédécesseur immédiat qui a marché
	  // Ils sont donc indépendants les uns des autres dans le DAG	  
	  // On va les marquer, repartir du plus petit d'entre eux, et modifier temporairement le(s) beam(s) sur chacun
	  // d'eux

	  // Cherchons déjà les états de relance
	  // Pour cela, cherchons tout d'abord l'état obligatoire <p le plus proche, qui est nécessairement un succès
	  q = sxba_1_rlscan (idag.states_in_all_paths_set, p);
	  if (q <= 0)
	    break; // pas de beam-rattrapage possible; pour l'instant on arrête de tenter de parser, mais il faudra faire en sorte que se déclenche le vrai rattrapage
#if EBUG
	  if (!SXBA_bit_is_set (successful_p_set, q))
	    sxtrap (ME, "recognize (could not find a valid predecessing mandatory state for initiating beam-related recovery)");
#endif /* EBUG */
	  // On cherche alors le premier des états d'où on va relancer le parsing et qui auront un beam modifié
	  if (beam_rcvr_resuming_states == NULL)
	    beam_rcvr_resuming_states = sxba_calloc (idag.final_state+2);
	  else
	    sxba_empty (beam_rcvr_resuming_states);
	  r = sxba_scan_reset (dead_ends, q);
	  clear_table (r);
	  if ((earley_table_companion [r].did_scan_beam && scan_rcvr_beam_delta_is_set)
	      || (earley_table_companion [r].did_shift_beam && shift_rcvr_beam_delta_is_set)) {
	    SXBA_1_bit (beam_rcvr_resuming_states, r);
	    first_rcvr_resuming_state = r;
	    earley_table_companion [r].beamrcvr_state = 1;
	  } else
	    first_rcvr_resuming_state = -1;
	  while (r < p) {
	    // On cherche ensuite les autres états d'où on va relancer le parsing et qui auront un beam modifié, états
	    // pour lesquels il faut vérifier qu'ils ont au moins un précedesseur immédiat qui est un succès
	    r = sxba_scan_reset (dead_ends, r);
	    clear_table (r);
	    sr_stack = idag.q2pq_stack + idag.q2pq_stack_hd [r];
	    for (sr_ptr = sr_stack + sr_stack [0]; sr_ptr > sr_stack; sr_ptr--) {
	      s = range2p (*sr_ptr);
	      if (SXBA_bit_is_set (successful_p_set, s)) { /* candidat état de relançage */
		if ((earley_table_companion [r].did_scan_beam && scan_rcvr_beam_delta_is_set)
		    || (earley_table_companion [r].did_shift_beam && shift_rcvr_beam_delta_is_set)) {
		  SXBA_1_bit (beam_rcvr_resuming_states, r);
		  if (first_rcvr_resuming_state == -1)
		    first_rcvr_resuming_state = r;
		  earley_table_companion [r].beamrcvr_state = 1;
		  break;
		}
	      }
	    }
	  }
	  if (first_rcvr_resuming_state > -1) {
	    p = first_rcvr_resuming_state - 1;
	    used_beamrcvr = SXTRUE;
	    raz_AxI_J_and_ei_for_j_greater_than (first_rcvr_resuming_state);
	    continue;
	  }
	  else {
	    SXINT i, j;
	    /* on n'a plus rien à tenter localement sur les états de relançage intelligents. On recommence donc depuis
	       le premier état r ayant beamé et supérieur à:
	       - 1 si on replante au même endroit que le coup d'avant, et en ce cas le beam doit être augmenté
	       - le même endroit que le coup d'avant + 1 si on plante plus loin que le précédent plantage
	    */
	    if (last_deadend_from_which_global_beamrcvr_was_launched == p) {
	      j = 1;
	    }
	    else {
	      j = last_deadend_from_which_global_beamrcvr_was_launched + 1;
	    }
	    for (r = j; r <= p; r++)
	      if (!earley_table_companion [r].no_more_beamrcvr_possible)
		break;
	    if (r > p) {
	      if (last_deadend_from_which_global_beamrcvr_was_launched == p) /* aucun beamrcvr possible sur les états de 1 à p inclus: inutile de tenter un global beamrcvr */
		break;
	      /* sinon, on force à retenter depuis le début et à utiliser un beam augmenté. On checke si du beamrcvr entre 1 et j-1 est possible (de j à p c'est déjà fait) */
	      for (r = 1; r < j; r++)
		if (!earley_table_companion [r].no_more_beamrcvr_possible)
		  break;
	      if (r >= j)
		break; /* y'avait vraiment pas moyen... */
	    }
	    global_rcvr_resuming_state = r;
	    /* à ce stade, on a:
	       - global_rcvr_resuming_state, première table que l'on va refaire
	       - on doit augmenter les beams des tables de global_rcvr_resuming_state à p */
	    /* on lance un global beamrcvr à partir de global_rcvr_resuming_state */
	    for (i = global_rcvr_resuming_state; i <= p; i++) {
	      clear_table (i);
	      earley_table_companion [i].beamrcvr_state = 2;
	      SXBA_0_bit (dead_ends, i);
	      SXBA_0_bit (mlstn_active_set, i);
	      SXBA_0_bit (successful_p_set, i);
	    }
	    p = global_rcvr_resuming_state - 1;
	    used_beamrcvr = SXTRUE;
	    raz_AxI_J_and_ei_for_j_greater_than (global_rcvr_resuming_state);
	    global_beamrcvr_relaunches++;
	    last_deadend_from_which_global_beamrcvr_was_launched = p; /* on se souvient de là où on a planté: si avec un
					       beam plus permissif b' on replante au même endroit, il faudra réaugmenter
					       le beam au-delà de b'; si on arrive à aller plus loin que p et qu'on
					       plante en p', il suffira, au moins dans un premier temps, d'utiliser le
					       beam b' entre seulement p+1 et p' pour voir ce qui se passe. Seulement si
					       alors on replante en p' il faudra recommencer du début avec un beam plus
					       grand que b' */
	    continue;
	  }
	} else
	  break;
      }

#else /* RECOGNIZER_KIND == 2 */
      // Sans recovery (et en attendant le retour du "vrai" recovery indépendant du beam-recovery ci-dessus, ne pas
      // réussir à construire une table p si p est un état "obligatoire" du DAG suffit à conclure à l'échec de l'analyse
      if (SXBA_bit_is_set (idag.states_in_all_paths_set, p))
	break;
#endif /* RECOGNIZER_KIND == 2 */
#else /* is_beamrcvr */
      // Sans recovery, ne pas réussir à construire une table p si p est un état "obligatoire" du DAG suffit à conclure
      // à l'échec de l'analyse
      if (SXBA_bit_is_set (idag.states_in_all_paths_set, p))
	break;
#endif /* is_beamrcvr */

      
#if 0
      if ((mlstn = sxba_scan_reset (mlstn_active_set, mlstn)) == -1) {
	/* ... et rien ds le suffixe n'a ete calcule. */
#if is_rcvr
	/* Erreur d'analyse, il faut faire du rattrapage */
	/* Les milestones n'ayant pas de descendents sont ds dead_ends  */
	earley_dag_rcvr ();
	/* si mlstn==-1, le traitement d'erreur n'a rien pu faire... */
	mlstn = sxba_scan_reset (mlstn_active_set, -1);
	/* On ne ressaie que ceux qui ont eu du nouveau */
#if LLOG
	printf ("--- Table " SXILD " after recovery --->", mlstn);
	output_table (mlstn);
	printf ("<--- Table " SXILD " after recovery ---\n\n", mlstn);
#endif /* LLOG */
#else /* !is_rcvr */
	/* ... adieu */
	break;
#endif /* !is_rcvr */
      }
#endif /* 0 */
    }
  }

  is_in_LG = sxba_bit_is_set (successful_p_set, idag.final_state) /* on est arrivés au bout de la phrase */
    && ((index_set = RT [idag.final_state].index_sets [2]) != NULL) /* et il y a un bon item de fin... */
    && SXBA_bit_is_set (index_set, 0); /* ...qui spanne bien depuis le début de la phrase */
    
  sxfree (mlstn_active_set), mlstn_active_set = NULL;
  sxfree (successful_p_set), successful_p_set = NULL;
  sxfree (dead_ends), dead_ends = NULL;
  if (beam_rcvr_resuming_states)
    sxfree (beam_rcvr_resuming_states), beam_rcvr_resuming_states = NULL;
  
#if 0
  if (merge_table_item_set)
    sxfree (merge_table_item_set), merge_table_item_set = NULL;
#endif /* 0 */

  if (!is_in_LG) {
#if is_rcvr
    /* Normalement, on ne passe jamais la ... */
    /* ... ben si !! */
    /* Le 11/07/05 : il semble que c,a  a ete corrige' et que de nouveau on n'y passe plus, a voir !! */
    /* Parsing stops on end of file */
    sxerror (sxsvar.sxlv.terminal_token.source_index,
	     sxplocals.sxtables->err_titles [2][0],
	     rcvr_eof /* "%sParsing stops on end of file." */,
	     sxplocals.sxtables->err_titles [2]+1);
#else /* !is_rcvr */
    /* Ca permet aussi de sortir : exit () de syntax avec le bon code */
    sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
	    "Syntax Error (recognize).",
	    sxplocals.sxtables->err_titles [2]);
#endif /* is_rcvr */
  }
    
  ret_val = is_in_LG;

#if is_rcvr
  if (is_error_detected)
    is_in_LG = SXFALSE;
#endif /* is_rcvr */  

  if (is_print_time) {
    char mess [128];
    //#if is_beamrcvr
    //    char tmpstr [32];
    //#endif /* is_beamrcvr */
  
#if HAS_BEAM
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) {
      struct logprob_struct *logprob_ptr;

      if (ret_val)
	ei2get_logprob_ptr (2, p_q2pq (0, idag.final_state), logprob_ptr);

#if is_beamrcvr
      /* todo: afficher si on a fait du beamrcvr, et lequel */
#endif /* is_beamrcvr */
      
      sprintf (mess, "\tDAG %sEarley Recognizer [%sBest = " SXLPD "], (%s) "
#if RECOGNIZER_KIND==2 && is_beamrcvr
SXILD
#endif /* RECOGNIZER_KIND==2 && is_beamrcvr */
, 
#ifdef CONTROLLED_BEAM_H
	       "controlled ",
#else /* CONTROLLED_BEAM_H */
	       "",
#endif /* CONTROLLED_BEAM_H */
#if RECOGNIZER_KIND
	       (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) ? "beamed " : "",
#else /* RECOGNIZER_KIND */
	       "",
# endif /* RECOGNIZER_KIND */
	       ret_val ? SXLOGPROB_display (logprob_ptr->inside.left) : 0,
	       is_in_LG ? "TRUE" : "FALSE"
#if RECOGNIZER_KIND==2 && is_beamrcvr
	       , earley_table_companion ? earley_table_companion [1].beamrcvr_state : 0
	       /* TODO: mettre plutôt un max_i (earley_table_companion [i].beamrcvr_state) */
#endif /* RECOGNIZER_KIND==2 && is_beamrcvr */
	       );
    }
    else
#endif /* HAS_BEAM */
      sprintf (mess, "\tDAG Earley Recognizer (%s)", is_in_LG ? "TRUE" : "FALSE");

    sxtime (TIME_FINAL, mess);
  }

  return ret_val;
}




#if is_parser

/******************************************************************************************

				     P A R S E R

******************************************************************************************/


/* cas is_parse_forest */
static SXINT      Axixj2Aij_hd_foreach [6] /* Pas de foreach */,
                Txixj2Tij_hd_foreach [6] =  {0, 1, 0, 0, 0, 0}/* Yforeach par Tij_iforeach */;


/******************************************************************************************
                                       U N F O L D I N G

  Procedures qui "deplient" spf.
  L'utilisateur peut demander de parcourir l'arbre no p en appelant 
  spf_dag2tree (p, spf.outputG.start_symbol, f);
  La fonction utilisateur f est appelee avec f (prod) ou prod est la production courante
  Les appels se font bas-haut gauche-droite
  Il a fallu appeler en premier spf_tree_count (&count);

Si le parseur earley a ete compile avec l'option -DSEMANTICS="unfold"
l'execution de ./L.out -pf -ptn
sort qqchose du type
(|N|=15, |T|=25, |P|=18, S=1, |outpuG|=45, TreeCount=4)
(La semantique normale, cad unfold(), a ete remplacee par le calcul du nombre d'arbres)
 tandis que l'execution de ./L.out -pf source
execute la semantique par defaut qui est l'impression de chacun des arbres de la foret

Parse tree #0
1:<NP[1..2]>    = "np"[1..2] ;
...

Parse tree #1
...

******************************************************************************************/
/* On vient de "reconnaitre" une Aij_prod, tous ses sous arbres sont evalue's */

#ifdef SEMANTICS
static void
unfold ()
{
  for_semact.sem_init = NULL;
  for_semact.sem_final = NULL;
  for_semact.sem_close = NULL;
  for_semact.semact = NULL;
  for_semact.parsact = NULL;
  for_semact.prdct = NULL;
  for_semact.constraint = NULL;
  for_semact.sem_pass = spf_unfold;
  for_semact.scanact = NULL;
}
#endif /* SEMANTICS */


/******************************************************************************************
                             E N D    O F    U N F O L D I N G
******************************************************************************************/


/******************************************************************************************
                  A T T R I B U T E    E V A L U A T I O N    o n    a    D A G
******************************************************************************************/
/*
  Principe : la valeur "val" d'un attribut "attr" associe a un noeud du DAG d'etiquette "Xpq" specifie par spf
  est rangee ds le XxY spf.dag.attr[attr] a l'indice (Xpq, val).  Il est possible de recuperer
  toutes les valeurs de attr associees a un Xpq.
  Si l'utilisateur veut calculer l'attribut a0 associe au noeud X0 qui depend des attributs
  (a1, X1), ..., (ak, Xk), il remplit attr_stack avec a0, a1, ..., ak et Xpq_stack avec X0, X1, ..., Xk
  et il appelle spf_dag_evaluate (k, f); pour toutes les combinaisons de valeurs d'attributs
  v1, ..., vk associees aux (a1, X1), ..., (ak, Xk), spf_dag_evaluate remplit val_stack[1], ..., val_stack[k]
  avec v1, ..., vk et appelle la fonction entiere (utilisateur) f, qui calcule (et retourne) v0 en
  fonction des v1, ..., vk.  Cette valeur sera rangee ds XxY spf.dag.attr[a0] a l'indice (X0, v0)
 */

/* Comment c,a se passe ?
   l'option de compilation du parseur earley -DSEMANTICS="ma_semantique" permet de specifier le point d'entree
   de la semantique de l'utilisateur.  La procedure ma_semantique() remplit for_semact.
   En particulier elle doit remplir for_semact.sem_pass qui est appele' apres la construction de la
   foret partagee.
   for_semact.sem_pass doit remplir 
    - attr_nb : nb d'attributs (reperes de 0 a attr_nb-1)
    - depend_nb : nb max de dependances des attributs
    - simple_history_hd_oflw : procedure appelee avec les arg (old_size, new_size) sur debordement.  Il faut
      reallouer la structure ds laquelle sont ranges les attributs (new_size+1)
    - pass_inherited (item_or_hook)
    - pass_derived (hook)
   de spf.dag
   Si pass_inherited retourne SXFALSE, le sous_arbre n'est pas visite'.
   Si pass_derived retourne SXFALSE, le sous_arbre est revisite'.

   Si l'utilisateur veut profiter de la gestion des attributs proposee, il doit appeler spf_dag_alloc ()
   et spf_dag_free () en fin de for_semact.sem_pass

   Il dispose de 2 types de parcourt spf_topological_top_down_walk () et spf_topological_bottom_up_walk ().
   Ces procedures peuvent etre appelees un nombre qcq de fois

   spf_topological_top_down_walk () parcourt le DAG de spf selon un ordre topologique :
   Les Aij-productions sont visitees ssi toutes les productions ou Aij
   intervient en rhs ont deja ete visitees.  
   Elle appelle (*spf.dag.pass_inherited) (hook_or_item)
   Si hook_or_item est un hook (hook_or_item >= maxitem) on s'apprete a visiter la Aij_prod
   spf.outputG.rhs [hook].LISPRO.(   S) hook_or_item est un item on visite le fils Xpq = spf.outputG.rhs [item].LISPRO () la 
   Aij_prod = spf.outputG.rhs [item].prolis.  
   On peut donc calculer (par spf_dag_evaluate) les attributs herites associes a Xpq (si tous leurs antecedents sont calcules);

   spf_topological_bottom_up_walk () parcourt le DAG de spf selon un ordre topologique :
   une Aij_production est visitee ssi tous les Xpq qui interviennent en rhs ont ete calcules.  
0_   Elle appelle (*spf.dag.pass_derived) (hook) ou hook est l'indice de la Aij_prod visitee
   (Aij_prod = spf.outputG.rhs [hook].lispro)  
   On peut donc calculer (par spf_dag_evaluate) les attributs synthetises associes a Aij (si tous leurs antecedents sont calcules);
   


   Elle doit allouer la structure ds laquelle sera rangee les valeurs des attributs (taille
   XxY_size (spf.dag.simple_history_hd)+1)

   for_semact.sem_pass doit appeler spf_walk (0);
   qui initialise le parcourt de la foret partagee.  pass_inherited et pass_derived sont appeles au bon moment.
   Pour evaluer un attribut a sur item qui depend de (b1, X1), ... (bk, Xk)
   la pass_inherited ou la pass_derived remplit spf.dag.attr_stack avec [a, b1, ..., bk], 
   spf.dag.Xpq_stack avec [X, X1, ..., Xk] et appelle spf_dag_evaluate (k, eval)
   eval() sera appele, autant de fois que necessaire.  A chaque appel, l'utilisateur trouvera ds
   spf.dag.history_stack la liste [pi, pi1, ..., pik].  Le resultat de l'evaluation sera range'
   en pi, les operandes provenant des indices pi1, ..., pik.

   Puis en fin spf_dag_free ()
   +free des attributs
 */



void
print_symb (out_file, symb, i, j)
     FILE  *out_file;
     SXINT   symb, i, j;
{
  SXINT ste;
  char *t;

  if (symb < 0) {
    t = tstring [-symb];

#if is_rcvr
    fprintf (out_file, "\"%s\"[", t);

    if (i <= 0) {
      if (i == 0)
	fputs ("*", out_file);
      else
	fprintf (out_file, SXILD, -i);
    }
    else
      fprintf (out_file, SXILD, i);

    if (i <= 0 || j <= 0 || i > idag.final_state || j > idag.final_state || i == j)
      fputs ("::", out_file);
    else
      fputs ("..", out_file);

    if (j <= 0) {
      if (j == 0)
	fputs ("*] ", out_file);
      else
	fprintf (out_file, SXILD "] ", -j);
    }
    else
      fprintf (out_file, SXILD "] ", j);
#else /* !is_rcvr */
    fprintf (out_file, "\"%s\"[" SXILD ".." SXILD "] ", t, i, j);
#endif /* !is_rcvr */
  }
  else {
#if is_rcvr
    fprintf (out_file, "<%s[", ntstring [symb]);

    if (i <= 0) {
      if (i == 0)
	fputs ("*", out_file);
      else
	fprintf (out_file, SXILD, -i);
    }
    else
      fprintf (out_file, SXILD, i);

    if (i <= 0 || j <= 0 || i > idag.final_state || j > idag.final_state)
      fputs ("::", out_file);
    else
      fputs ("..", out_file);

    if (j <= 0) {
      if (j == 0)
	fputs ("*]> ", out_file);
      else
	fprintf (out_file, SXILD "]> ", -j);
    }
    else
      fprintf (out_file, SXILD "]> ", j);
#else /* !is_rcvr */
    fprintf (out_file, "<%s[" SXILD ".." SXILD "]> ", ntstring [symb], i, j);
#endif /* !is_rcvr */
  }
}

/* La phrase
    he loves her
   1  2     3   4
   a ces milestones qq soit le type du source (dag, sdag ou lineaire).  On a donc
   <axiom[1..4]> = ...
   un nt vide <A>, entre loves et her s'ecrit <A[3..3]>
*/


static SXINT iprod_stack [inputG_MAXRHSLGTH+1/*compte*/+1/*lhs*/+1/*prod*/];
/* Nelle version le 14/09/06 */
/* utilise spf_put_a_iprod */
/* utilise spf_print_prod */
/* xt_complete a deja ete appele' */
static void
put_in_shared_parse_forest (rhs_stack)
     SXINT	*rhs_stack;
{
  SXINT	           prod, init_prod, h, i, j, k, A, Aij, bot_item, top_item, X, init_X, couple, bot_bot_item, *bot_rhs_stack;
  SXINT              *cur_iprod_top;
  SXBOOLEAN          has_error;

  prod = *rhs_stack++;

#if MEASURES || LOG
  SXBA_1_bit (used_prod_set, prod);
#endif /* MEASURES || LOG */

  top_item = PROLON (prod+1) - 1;
  bot_item = PROLON (prod);

  i = *rhs_stack++;
  j = *rhs_stack++;

#if EBUG
  if (i <= 0 || j <= 0)
    sxtrap (ME, "put_in_shared_parse_forest");
#endif /* EBUG */

  A = LHS (prod);

#ifdef MAKE_INSIDEG
  A = spf.insideG.nt2init_nt [A];
  init_prod = spf.insideG.prod2init_prod [prod];
#else /* ifndef MAKE_INSIDEG */
  init_prod = prod;
#endif /* ifndef MAKE_INSIDEG */

  cur_iprod_top = iprod_stack + (iprod_stack [0] = top_item-bot_item+2);
  *cur_iprod_top = init_prod;
  iprod_stack [1] = Aij = GET_Aij (A, i, j);

#if EBUG
  if (Aij == 0)
    sxtrap (ME, "put_in_shared_parse_forest");
#endif /* EBUG */

  /* separation complete du cas normal du cas ou y'a des erreurs */
#if is_rcvr
  /* Attention, meme ds le cas repair, la foret peut sembler cyclique a cause des bornes des ranges */
  /* Ex :sur <VP[4..7]> :
<PV[3..7]> 	= "prep"[3..4] <VP[4..7]> ;
<VP[4..7]> 	= <VP3[4..7]> ;
<VP3[4..7]> 	= <VERB[4::4]> <POSTVERB[4..7]> ;
<VERB[4::4]>    = "v" ;  // reparation
<POSTVERB[4..7]> = <VP2[4..7]> ;
<VP2[4..7]> 	= <VP[4..7]> ;
*/
  if (is_error_detected) {
    has_error = SXFALSE;
    bot_rhs_stack = rhs_stack;
    bot_bot_item = bot_item;

    while (bot_item < top_item) {
      X = LISPRO (bot_item++);
      h = *rhs_stack++;
      k = *rhs_stack++;

      if (h < -idag.final_state) {
	/* repair */
	has_error = SXTRUE;
	/* cas particulier ou k est lui-meme le symbole deja instancie' */
	Aij = k;
	X = 0;
      }
      else {
	if  (!is_rcvr_can_be_cyclic && (h < 0 || k < 0))
	  has_error = SXTRUE;
      }

      if (X != 0) {
	if (X < 0) {
	  /* Essai le 01/12/05, les terminaux du rattrapage contiennent (au moins) un indice <= 0 */
#ifdef MAKE_INSIDEG
	  X = -spf.insideG.t2init_t [-X];
#endif /* MAKE_INSIDEG */

	  MAKE_Tij (Aij, X, h, k);
   
	  if (h <= 0 || k <= 0)
	    SXBA_1_bit (spf.outputG.Tpq_rcvr_set, Aij);

	  Aij = -Aij;
	}
	else {
#ifdef MAKE_INSIDEG
	  X = spf.insideG.nt2init_nt [X];
#endif /* MAKE_INSIDEG */

	  if (h > 0 && k > 0) {
	    Aij = GET_Aij (X, h, k);

	    if (Aij == 0) {	  
	      sxtrap (ME, "put_in_shared_parse_forest");
	    }
	  }
	  else {
	    /* nt de l'error recovery */
	    MAKE_Aij (Aij, X, h, k);

#if EBUG
	    if (Aij <= spf.outputG.maxnt)
	      sxtrap (ME, "put_in_shared_parse_forest");
#endif /* EBUG */

	    /* ... qui peut meme exister si de la reparation est demandee, mais il n'est
	       pas defini par des xprod.  Ex 
	       A -> [i] B C [j] D ;
	       On avait B C =>+ x non vide
	       Et la correction revient a supprimer x
	       Tout se passe donc comme si on avait B C =>+ epsilon
	       On a B[-i::0] C[0::-j]
	    */

	    /* On notera ces nt non productifs +tard ds spf.outputG.non_productive_Aij_rcvr_set */
	  }
	}
      }

      *--cur_iprod_top = Aij;
    }

    if (has_error) {
      /* On regarde si cette production instanciee (qui contient une erreur) peut etre cyclique */
      while (bot_bot_item++ < top_item) {
	h = *bot_rhs_stack++;
	k = *bot_rhs_stack++;

	if (h > 0 && k > h) {
	  /* pas en erreur et non vide */
	  if (h > i || k < j) {
	    /* ... et ne couvre pas le meme range que la lhs => prod non cyclique */
	    break;
	  }
	}
      }

      if (bot_bot_item >= top_item)
	is_rcvr_can_be_cyclic = SXTRUE;
    }
  }
  else 
#endif /* is_rcvr */
    {
    while (bot_item < top_item) {
      X = LISPRO (bot_item++);
      h = *rhs_stack++;
      k = *rhs_stack++;

      if (X < 0) {
#ifdef MAKE_INSIDEG
	X = -spf.insideG.t2init_t [-X];
#endif /* MAKE_INSIDEG */
	MAKE_Tij (Aij, X, h, k);
	Aij = -Aij;
      }
      else {
#ifdef MAKE_INSIDEG
	X = spf.insideG.nt2init_nt [X];
#endif /* !MAKE_INSIDEG */
	Aij = GET_Aij (X, h, k);

	if (Aij == 0) {	  
	  sxtrap (ME, "put_in_shared_parse_forest");
	}
      }

      *--cur_iprod_top = Aij;
    }
  }

  spf_put_an_iprod (&(iprod_stack [0]));

  if (is_print_prod)
    spf_print_iprod (stdout, spf.outputG.maxprod, "", "\n");
}


#if is_epsilon || is_rcvr
  static SXBA *level_index_sets;
#endif /* is_epsilon || is_rcvr */

/* ATTENTION CHANGEMENT le 13 mars 2011
   Pour les segments corrige's [(k, item_k) .. (j, item_j)], k n'est plus mis ds le backward de (j, item_j)
   mais s'obtient par par un Zforeach des start_kitems.
   Les backward ne contiennent plus que les cas normaux */
static void
RL_mreduction_item (SXINT item_j, SXBA I, SXINT j, SXINT level)
{
  /* item_j = A -> \alpha Y . \gamma  et \alpha != \epsilon */
  SXINT				Y, bot_item, i, k, Z, new_k, item, item_k, order, t, cur_prod;
  SXBA			        backward_index_set, prev_index_set, index_set;
  SXBA			        /* tbp_set, */ ap_set;
  struct recognize_item	        *RTj;
  struct parse_item		*PTj;
  struct PT2_item		*PT2j;
  SXBOOLEAN                     is_ap_set_empty, is_item_k_valid, is_item_k_new_and_valid, is_Ykj_new_or_better, is_brand_new_ids, is_k_valid;

  SXBA			        ids, PT2j_backward_index_set, level_index_set;
  SXINT				*top_ptr, *bot_ptr;
#if is_rcvr
  SXINT                         triple, best_k, best_item_k, top_item, best_triple, resume_k, prev_k;
  SXBOOLEAN                     is_better;
#endif /* is_rcvr */
  SXINT                         k_logprob_changed; /* utilise pour faire de la factorisation avec le cas non beam */
#if REDUCER_KIND || HAS_BEAM == 2
  SXINT                         kj;
#endif /* REDUCER_KIND || HAS_BEAM == 2 */
#if is_rcvr || LLOG
  SXINT resume_j;
#endif /* is_rcvr || LLOG */

#if is_epsilon || is_rcvr
  level_index_set = level_index_sets [level];
#endif /* is_epsilon || is_rcvr */

  cur_prod = PROLIS (item_j);

#if is_rcvr
  resume_j = get_rcvr_resume ( j, item_j);
  top_item = PROLON (cur_prod+1)-1;
#else /* is_rcvr */
#if LLOG
  resume_j = 0;
#endif /* LLOG */
#endif /* is_rcvr */

#if LLOG
  printf ("\nEntering RL_mreduction_item (" SXILD ", <I>," SXILD "," SXILD ") %s:\n", item_j, j, level, resume_j ? "rcvr" : "");
  output_item (item_j, I, -1, j);
#endif /* LLOG */

  bot_item = PROLON (cur_prod);

#if is_rcvr
      if (resume_j) {
	/* (j, item_j) est la borne droite d'un segment de rattrapage */
	/* item_j est un item de la recuperation... */
	/* ...ca se complique */
	/* On a
	   item_j = A -> \alpha B \beta Y . \gamma
	   item_k = A -> \alpha B \beta . Y \gamma
	   mais la recuperation a ete lancee depuis l'item A -> \alpha B . \beta Y \gamma
	   qui est (peut-etre) different de item_k, il faut le retrouver...
	*/
	best_item_k = item_j+1;
	triple = 0;
#if EBUG
	prev_k = -1;
#endif /* EBUG */

	XxYxZ_Zforeach (glbl_dag_rcvr_start_kitems, resume_j, triple) {
	  k = XxYxZ_X (glbl_dag_rcvr_start_kitems, triple);
	  item_k = XxYxZ_Y (glbl_dag_rcvr_start_kitems, triple);
#if LLOG
	  fputs ("examining rcvr on ", stdout);
	  output_rcvr_prod (item_k, k, item_j, j);
	  fputs ("\n", stdout);
#endif /* LLOG */

#if EBUG
	  if (prev_k >= 0 && prev_k != k || item_k < bot_item || item_k > item_j)
	    /* Pour verifier qu'ils ont tous le meme k + segment coherent */
	    sxtrap (ME, "RL_mreduction_item");

	  prev_k = k;
#endif /* EBUG */

	  is_better = SXFALSE;

	  if (item_k < best_item_k) {
	    /* Je ne conserve que le + a gauche!! */
	    /* ... s'il est compatible avec les indes_set requis */
	    if (item_k == bot_item) {
	      if (SXBA_bit_is_set (I, k)) {
		/* il est bon */
		is_better = SXTRUE;
	      }
	    }
	    else {
#if HAS_BEAM == 2
	      if (reducer_beam_value_is_set && i2valid_item_set == NULL)
		/* 2eme passe du reduceur, on utilise l'index_set de la 1ere passe */
		prev_index_set = PT2_save [k].index_sets [item_k];
	      else
#endif /* HAS_BEAM == 2 */
		prev_index_set = RT [k].index_sets [item_k];

	      if (prev_index_set && IS_AND (prev_index_set, I)) {
		/* il est bon */
		is_better = SXTRUE;
	      }
	    }
	  }

	  if (is_better) {
	    best_triple = triple;
	    best_item_k = item_k;
	    best_k = k;
	  }
	}

	if (best_item_k <= item_j) {
	  /* il y a eu de la recuperation entre k et j compatible avec I */
#if LLOG
	  fputs ("Leftmost validated rcvr on ", stdout);
	  output_rcvr_prod (best_item_k, best_k, item_j, j);
	  fputs ("\n", stdout);
#endif /* LLOG */

	/* Ici :
	   item == 0 => aucune recuperation ne correspond a k (cas normal)
	   item != 0 => il y a eu de la recuperation entre k et j
	   si kept_triple == 0 => recup non impliquee ds le thread courant (on saute k)
	   si kept_triple != 0 => y'a de la recup validee sur new_item (pas cas normal)
	*/

	/* Attention : il est possible qu'un Aij soit fabrique par une A-prod qui a necessite du rcvr
	   et par une A-prod normale (ca peut meme etre la meme prod!!).  Il est clair que ds ce cas
	   on ne doit pas tenir compte de la rcvr.  PB, je ne sais pas comment l'eliminer/ne pas
	   sortir les messages... */

	/* Le 17/06/2010 En fait il semble que la question soit la suivante :
	   Soit l'item de la rcvr [A -> [i] \alpha [k] \beta [j] \gamma] avec de la rcvr entre [k] et [j]  ([k] se trouve en position new_item) et
	   Soit l'item normal [A -> [i] \alpha' \beta' [k] X [j] \gamma] t.q. \alpha \beta == \alpha' \beta' X (ici [k] se trouve en item_k == item_j-1)
	   Ces 2 items peuvent-ils etre simultanement valides apres la rcvr (sous quelles conditions) ou sinon lequel l'emporte (sous quelles conditions)  ?
	   Remarque : avant la rcvr, les items de la table T [k] ont tous ete construits.
	   Peut-on se recuperer sur un item normal (derive' du kernel) ? 
	   Si ce cas se produit ca ne peut se faire que si k==j et est donc en contradiction avec le fait
	   qu'une erreur a ete detectee ds la table e avec k<=e<=j.

	   La bonne table T [k] apres la rcvr ne devrait contenir que :
	       - Les items qui sont derives du kernel et aboutissent aux bases des items de la rcvr
	       - Les items qui sont derives des points de recuperation des items de la rcvr
	   Tous les autres sont des parasites !

	   Donc si l'item normal [A -> [i] \alpha' \beta' [k] X [j] \gamma] est derive du kernel, il doit
	   etre invalide', il est subsume' par celui de la rcvr et en particulier on ne doit pas produire
	   les regles qui generent les phrases de Xkj.

	   Cet item [A -> [i] \alpha' \beta' [k] X [j] \gamma] peut-il etre derive' d'un point de recuperation ?
	   Ds ce cas on doit avoir i==k==j avec \alpha' \beta' =>* \epsilon, ce qui est possible.  En revanche
	   cet item n'est pas atteint par une backward transition sur Xjj (incompatible avec le fait que
	   [A -> [i] \alpha' \beta' [k] X [j] \gamma] soit un item de la rcvr.  Cet item sera donc atteint
	   par une backward transition sur Xjj' (avec j' > j).

	   Bref, il semble donc que ce ne soit pas possible que item_k, k soit un item normal obtenu
	   par backward transition sur Xkj.  (Contrairement a ce qui est programmer).
	   Il faudrait donc pister ce cas une fois pour toute et trancher !!
	   Une piste pour avoir des textes avec beaucoup d'erreurs est d'utiliser du beam de 1 ds le reconnaisseur.
	*/
#if HAS_BEAM == 2
	  if (reducer_beam_value_is_set && i2valid_item_set == NULL)
	    /* On ne fait ca que sur le "vrai" passage */
#endif /* HAS_BEAM == 2 */
	    {
	      /* Attention : Si la recuperation se fait par transition sur l'axiome 1 ( 0: 0 -> $ 1 $), il n'y aura
		 pas de "production" reconnue par le parseur. Ds ce cas il faut trouver un moyen, ds la phase parser,
		 de sortir le message d'erreur disant qu'on n'a rien reconnu (A FAIRE) */
	      /* Le 11/08/04 pour l'instant, on le note !! */
	      if (item_j <= 2) {
		PUSH (rcvr_on_super_rule_stack, best_triple);
		PUSH (rcvr_on_super_rule_stack, best_item_k);
		PUSH (rcvr_on_super_rule_stack, best_k);
		PUSH (rcvr_on_super_rule_stack, item_j);
		PUSH (rcvr_on_super_rule_stack, j);
	      }
	    }

	  if (best_item_k == bot_item) {
	    /* ... on a atteint la pos initiale */
#if REDUCER_KIND
	    k_logprob_changed = reducer_beam_value_is_set ? (kj = 0, set_backward_logprob (best_k, best_k, j, best_item_k, item_j, 0, &kj)) : 1;

	    if (k_logprob_changed >= 0)
#endif /* REDUCER_KIND */
	      is_item_k_valid = SXTRUE;
	    /* plus de backward sur la rcvr */
	  }
	  else {
	    /* On est dans le cas A -> [i] \alpha X [k] \beta [j] \gamma */
	    /* best_item_k > bot_item */
#if HAS_BEAM == 2
	    if (reducer_beam_value_is_set && i2valid_item_set == NULL) {
	      /* 2eme passe du reduceur, on utilise l'index_set de la 1ere passe */
	      prev_index_set = PT2_save [best_k].index_sets [best_item_k];
	    }
	    else
#endif /* HAS_BEAM == 2 */
	      prev_index_set = RT [best_k].index_sets [best_item_k];

#if EBUG

	    if (prev_index_set == NULL)
	      sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */

	    /* Il se peut que k == j ds le cas de recuperation, meme si is_epsilon est faux */
	    if ((is_brand_new_ids = ((ids = PT2 [best_k].index_sets [best_item_k]) == NULL)))
	      ids = PT2 [best_k].index_sets [best_item_k] = sxba_bag_get (&pt2_bag, *prev_index_set /* Pour "OR_AND" le 19/10/05 a la place de k+1 */);
	  
#if REDUCER_KIND
	    if (reducer_beam_value_is_set) {
	      SXINT     i;

	      if (best_k == j) {
		sxba_empty (level_index_set);
		level_index_set [0] = j+1; /* On change la taille */
	      }

	      is_item_k_new_and_valid = is_item_k_valid = SXFALSE;
	      kj = p_q2pq (best_k, j);

	      i = -1;

	      while ((i = sxba_scan (I, i)) >= 0 && i <= best_k) {
		if (SXBA_bit_is_set (prev_index_set, i)) {
		  k_logprob_changed = set_backward_logprob (i, best_k, j, best_item_k, item_j, 0, &kj);

		  if (k_logprob_changed >= 0) {
		    /* Pas de Ykj en rcvr */
		    is_item_k_valid = SXTRUE;

		    if (SXBA_bit_is_reset_set (ids, i)) {
		      is_item_k_new_and_valid = SXTRUE;

		      if (best_k == j) {
			/* Pour le RL_mreduction_item recursif */
			SXBA_1_bit (level_index_set, i);
		      }
		    }
		  }
		}
	      }
	    } 
	    else /* ! reducer_beam_value_is_set */
#endif /* REDUCER_KIND */
	      {
		is_item_k_valid = SXTRUE; /* pas de beam, donc item_k et Ykj sont forcement valides */

		if (best_k == j) {
		  level_index_set [0] = j+1; /* On change la taille */
		  is_item_k_new_and_valid = OR_AND_MINUS (ids, prev_index_set, I, level_index_set);
		}
		else
		  is_item_k_new_and_valid = OR_AND (ids, prev_index_set, I);
	      }

	    if (best_k == j) {
	      if (is_item_k_new_and_valid) {
		RL_mreduction_item (best_item_k, level_index_set, j, level+1);
		/* plus de backward sur la rcvr */
	      }
	    }
	    else {
	      if (is_item_k_valid) {
		/* plus de backward sur la rcvr */
		if (is_item_k_new_and_valid) {
		  if (/* best_item_k > bot_item && best_k < j &&  toujours vrai ici */is_brand_new_ids) {
		    if (/* cas "X1 ... Xp [k] [j>k]" */ best_item_k == top_item 
			|| /* on est sur la super-règle dans un etat "$ AXIOME [k] [j>k] $" */ (best_item_k == 2 && item_j == 2)
			)
		      /* Le 20/11/06 Si on est ds le cas
			 242:<GN> = <ADJANTE___c> <ADJ_PREF___STAR> "nc" [5] [6] ; (j==6)
			 Il ne faut pas positionner PT2 [5].index_sets [<GN> = <ADJANTE___c> <ADJ_PREF___STAR> "nc" .]
		      */
		      /* Le 09/01/07 mais il faut quand meme continuer la reconnaissance de 242 et il faut que la lhs instanciee
			 soit de la forme <GN[i..6]> (et non <GN[i..5]>) */
		      /* Dans ce cas tordu, on restera en position reduce apre`s avoir recule' dans le source (!!!),
			 on doit le dire, en passant -new_item au lieu de new_item, 
			 car si au moins une re`gle etait dans cette position reduce de fac,on normale (sans rcvr),
			 et si on vient en premier sur la re`gle issue du rcvr, red_order_sets [k] ne sera pas positionné,
			 mais la re`gle "normale" ne fera plus rien */
		      PUSH (PT2 [best_k].shift_NT_stack, -best_item_k);
		    else
		      PUSH (PT2 [best_k].shift_NT_stack, best_item_k);
		  }
		}
	      }
	      else {
		if (is_brand_new_ids) {
		  /* Pour ne pas avoir d'index_set vide */
		  PT2 [best_k].index_sets [best_item_k] = NULL;
		}
	      }
	    }
	  }
	      
	  if (is_item_k_valid) {
#if !is_guide && !is_supertagging
	    /* Le 04/11/05 */
	    /* rcvr valide', on le note */
	    SXBA_1_bit (glbl_dag_rcvr_start_kitems_set, best_triple);
#endif /* !is_guide && !is_supertagging */
		
#if HAS_BEAM == 2
	    if (i2valid_triple_set) SXBA_1_bit (i2valid_triple_set [j], best_triple);
#endif /* HAS_BEAM == 2 */

#if LOG
	    earley_glbl_rcvr_message (best_item_k, best_k, item_j, j, 1 /* warning */, SXFALSE);
#endif /* LOG */
	  }

# if LLOG
	  if (is_item_k_valid) 
	    printf ("\t\tKept as rcvr item (triple=" SXILD ") ", best_triple);
	  else
	    fputs ("\t\tBeamed out rcvr logprob item for all i's ", stdout);

	  output_rcvr_prod (best_item_k, best_k, item_j, j);
	  fputs ("\n", stdout);
# endif /* LLOG */
	}
#if LLOG
	else {
	  printf ("\t\tUnvalidated rcvr item ");
	  output_rcvr_prod (best_item_k, best_k, item_j, j);
	  fputs ("\n", stdout);
	}
#endif /* LLOG */
      }
#endif /* is_rcvr */

  PTj = &(PT [j]);
		
#if HAS_BEAM == 2
  if (reducer_beam_value_is_set && i2valid_item_set == NULL)
    /* 2eme passe du reduceur, on utilise le backward de la 1ere passe */
    backward_index_set = PT2_save [j].backward_index_sets [item_j];
  else
#endif /* HAS_BEAM == 2 */
    backward_index_set = PTj->backward_index_sets [item_j];

#if EBUG
  if (backward_index_set == NULL
#if is_rcvr
      && resume_j == 0
      /* Il se peut que backward soit null s'il n'y a que de la rcvr depuis (j, item_j) */
#endif /* is_rcvr */
      )
    sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */

  if (backward_index_set) {
    PT2j = &(PT2 [j]);
    item_k = item_j - 1;
    Y = LISPRO (item_k);
    /* item_k = X -> \alpha . Y x \gamma */

    /* Le 10/07/2003
       Attention, ds le cas is_dag, item_j a pu etre ajoute directement par le traitement d'erreur sans qu'il y ait eu
       de reduction vers Y !! */
    if (Y > 0  /* Le 08/07/2003 */)
      ap_set = ap_sets [Y];
    else
      ap_set = NULL;

    is_ap_set_empty = SXTRUE;

    /* Ajoute' le 17/11/04 */
    PT2j_backward_index_set = PT2j->backward_index_sets [item_j];

    if (PT2j_backward_index_set == NULL)
      PT2j_backward_index_set = PT2j->backward_index_sets [item_j] = sxba_bag_get (&pt2_bag, j+1);
	    
    /* nelle version de la boucle sur les backward */
    k = -1;

    while ((k = sxba_scan (backward_index_set, k)) >= 0) {
#if LLOG
      printf ("\tTrying backward index " SXILD "\n", k);
#endif /* LLOG */

      /* Nelle version le 13/01/06 */
      if (item_k == bot_item) {
	if (!SXBA_bit_is_set (I, k)) {
	  /* Pas un bon k */
	  k_logprob_changed = -1;
	}
	else {
#if REDUCER_KIND
	  if (reducer_beam_value_is_set) {
	    kj = p_q2pq (k, j);
	    k_logprob_changed = set_backward_logprob (k, k, j, item_k, item_j, Y, &kj);
	    is_Ykj_new_or_better = (k_logprob_changed >= 0 && k_logprob_changed < 2);
	  }
	  else
#endif /* REDUCER_KIND */
	    k_logprob_changed = 0, is_Ykj_new_or_better = SXTRUE;

#if LLOG
	  if (k_logprob_changed >= 0)
	    printf ("\t\tkept ");
	  else
	    printf ("\t\tdiscarded ");

	  output_rcvr_prod (item_k, k, item_j, j);
	  fputs ("\n", stdout);
#endif /* LLOG */

	  if (k_logprob_changed >= 0) {
	    SXBA_1_bit (PT2j_backward_index_set, k);

#if HAS_BEAM == 2
	    if (i2valid_item_set) SXBA_1_bit (i2valid_item_set [k], item_k);
#endif /* HAS_BEAM == 2 */
	      
	    if (ap_set && is_Ykj_new_or_better && SXBA_bit_is_reset_set (ap_set, k)) {
	      is_ap_set_empty = SXFALSE;
	      SXBA_1_bit (RL_nt_set, Y);
	    }
	  }
	}
      }
      else {
	/* normalement RT [k].index_sets [item_k] == RT [k-nbt].index_sets [item_k] */
#if HAS_BEAM == 2
	if (reducer_beam_value_is_set && i2valid_item_set == NULL)
	  /* 2eme passe du reduceur, on utilise l'index_set de la 1ere passe */
	  prev_index_set = PT2_save [k].index_sets [item_k];
	else
#endif /* HAS_BEAM == 2 */
	  prev_index_set = RT [k].index_sets [item_k];
	
	if (prev_index_set == NULL || !IS_AND (prev_index_set, I)) {
	  /* C'est pas un bon k */
#if LLOG
	  printf ("\t\tdiscarded for all its indexes ");
	  output_rcvr_prod (item_k, k, item_j, j);
	  fputs ("\n", stdout);
#endif /* LLOG */
	}
	else {
	  /* C'est un bon k avec des bons index */
	  /* On ne doit pas le supprimer, il peut servir + tard. */
	  /* a cause de nbt on peut avoir ici item_k == bot_item */
	  if ((is_brand_new_ids = ((ids = PT2 [k].index_sets [item_k]) == NULL)))
	    ids = PT2 [k].index_sets [item_k] = sxba_bag_get (&pt2_bag, *prev_index_set /* Pour "OR_AND" le 19/10/05 a la place de k+1 */);
	  
#if REDUCER_KIND
	  if (reducer_beam_value_is_set) {
	    SXINT     i;

#if is_epsilon || is_rcvr
	    if (k == j) {
	      sxba_empty (level_index_set);
	      level_index_set [0] = j+1; /* On change la taille */
	    }
#endif /* is_epsilon || is_rcvr */

	    is_item_k_new_and_valid = is_Ykj_new_or_better = is_item_k_valid = SXFALSE;
	    kj = p_q2pq (k, j);

	    i = -1;

	    while ((i = sxba_scan (I, i)) >= 0 && i <= k) {
	      if (SXBA_bit_is_set (prev_index_set, i)) {
		k_logprob_changed = set_backward_logprob (i, k, j, item_k, item_j, Y, &kj);

		if (k_logprob_changed >= 0) {
		  is_item_k_valid = SXTRUE;

		  if (SXBA_bit_is_reset_set (ids, i)) {
		    is_item_k_new_and_valid = SXTRUE;

#if is_epsilon || is_rcvr
		    if (k == j) {
		      /* Pour le RL_mreduction_item recursif */
		      SXBA_1_bit (level_index_set, i);
		    }
#endif /* is_epsilon || is_rcvr */
		  }

		  if (k_logprob_changed < 2)
		    is_Ykj_new_or_better = SXTRUE;
		}

#if LLOG
		printf ("\t\t%s logprob item for i=" SXILD " ", (k_logprob_changed >= 0) ? "Keeping" : "Beaming", i);
		output_rcvr_prod (item_k, k, item_j, j);
		fputs ("\n", stdout);
#endif /* LLOG */
	      }
	    }
	  } 
	  else /* ! reducer_beam_value_is_set */
#endif /* REDUCER_KIND */
	    {
	      is_item_k_valid = is_Ykj_new_or_better = SXTRUE; /* pas de beam, donc item_k et Ykj sont forcement valides */

#if is_epsilon || is_rcvr
	      if (k == j) {
		level_index_set [0] = j+1; /* On change la taille */
		is_item_k_new_and_valid = OR_AND_MINUS (ids, prev_index_set, I, level_index_set);
	      }
	      else
#endif /* is_epsilon || is_rcvr */
		is_item_k_new_and_valid = OR_AND (ids, prev_index_set, I);

#if LLOG
	      {
		SXINT i = -1;

		while ((i = sxba_scan (I, i)) >= 0 && i <= k) {
		  if (SXBA_bit_is_set (prev_index_set, i)) {
		    printf ("\t\tkept for i=" SXILD " ", i);
		    output_rcvr_prod (item_k, k, item_j, j);
		    fputs ("\n", stdout);
		  }
		}
	      }
#endif /* LLOG */
	    }

#if is_epsilon || is_rcvr
	  if (k == j) {
	    if (is_item_k_new_and_valid) {
	      RL_mreduction_item (item_k, level_index_set, j, level+1);
	    }
	  }
	  /* On continue en sequence pour backward et Ykj */
#endif /* is_epsilon || is_rcvr */

	  if (is_item_k_valid /* Implique is_Ykj_valid */) {
	    SXBA_1_bit (PT2j_backward_index_set, k);

#if HAS_BEAM == 2
	    if (i2valid_item_set) SXBA_1_bit (i2valid_item_set [k], item_k);
#endif /* HAS_BEAM == 2 */

	    if (is_item_k_new_and_valid) {
	      if (item_k > bot_item && k < j && is_brand_new_ids)
		/* On pushe item_k pour la 1ere fois ds la table k du parseur */
		PUSH (PT2 [k].shift_NT_stack, item_k);
	    }
	      
	    if (ap_set && is_Ykj_new_or_better && SXBA_bit_is_reset_set (ap_set, k)) {
	      is_ap_set_empty = SXFALSE;
	      SXBA_1_bit (RL_nt_set, Y);
	    }
	  }
	  else {
	    if (is_brand_new_ids) {
	      /* Pour ne pas avoir d'index_set vide */
	      PT2 [k].index_sets [item_k] = NULL;
	    }
	  }
	}
      }

#if is_rcvr
      if (is_error_detected /* Y'a des erreurs ... */
	  && item_k == bot_item /* et on est en debut de partie droite ... */
	  && resume_j == 0 /* et pas de recup sur [k] Y [j] (sinon la recup [i][k] serait incluse dedans) ... */
	  && (resume_k = get_rcvr_resume (k, item_k)) > 0 /* et il y a de la recup sur [i][k] */
	  ) {
	/* On est donc ds le cas A -> [i][k] Y [j] \gamma
	   avec un segment vide de recuperation [i][k] */
	triple = 0;

	XxYxZ_Zforeach (glbl_dag_rcvr_start_kitems, resume_k, triple) {
	  i = XxYxZ_X (glbl_dag_rcvr_start_kitems, triple);
#if LLOG
	  fputs ("examining rcvr on ", stdout);
	  output_rcvr_prod (item_k, i, item_k, k);
	  fputs ("\n", stdout);
#endif /* LLOG */

#if EBUG	    
	  if (i == k || XxYxZ_Y (glbl_dag_rcvr_start_kitems, triple) != item_k)
	    sxtrap (ME , "RL_mreduction_item");
#endif /* EBUG */

	  if (SXBA_bit_is_set (I, i)) {
#if REDUCER_KIND
	    if (reducer_beam_value_is_set) {
	      kj = 0, k_logprob_changed = set_backward_logprob (i, k, k, item_k, item_k, 0, &kj);
	    }
	    else
#endif /* REDUCER_KIND */
	      k_logprob_changed = 0;
	  }
	  else
	    k_logprob_changed = -1;

# if LLOG
	  if (k_logprob_changed >= 0)
	    printf ("\t\tKept as rcvr item (triple=" SXILD ") ", triple);
	  else
	    printf ("\t\tExcluded as rcvr item for idex " SXILD " ", i);

	  output_rcvr_prod (item_k, i, item_k, k);
	  fputs ("\n", stdout);
# endif /* LLOG */

	  if (k_logprob_changed >= 0) {
#if HAS_BEAM == 2
	    if (i2valid_triple_set) SXBA_1_bit (i2valid_triple_set [j], triple);
#endif /* HAS_BEAM == 2 */

#if !is_guide && !is_supertagging
	    /* rcvr valide', on le note */
	    SXBA_1_bit (glbl_dag_rcvr_start_kitems_set, triple);
#endif /* !is_guide && !is_supertagging */

#if LOG
	    earley_glbl_rcvr_message (item_k, i, item_k, k, 1 /* warning */, SXFALSE);
#endif /* LOG */
	  }
	}
      }
#endif /* is_rcvr */
    } /* while ((k = sxba_scan (backward_index_set, k)) >= 0)  */

    if (ap_set  && !is_ap_set_empty /* garantit qu'au moins une des trans pour Y[..j] n'est pas du rcvr */
	/* && SXBA_bit_is_reset_set (RL_nt_set, Y) -- commenté Le 21/07/2003 */) {
      /* Le 22/08/05 le test SXBA_bit_is_reset_set (RL_nt_set, Y) est supprime' car on peut etre sur une
	 autre occur de Y que la 1ere */

      /* On récupère les productions instanciées définissant le non-terminal étudié (à savoir Y[k..j]),
	 où k est l'un des backwards parmi ceux qui ont été validés, et on les met dans prod_order_set pour
	 examen ultérieur. Ceci n'est nécessaire que si l'exécution courante de RL_mreduction_item a rajouté
	 de nouveaux k (backwards validés) dans l'ap_set  */

#if 0
      /* Le 25/03/10 */
      ap_set [0] = j+1; /* On change la taille!! */
#endif /* 0 */

      RTj = &(RT [j]);

      top_ptr = PTj->reduce_NT_hd [Y];

#if 0
      /* semble impossible depuis le 13 mars 2011 */
#if is_rcvr
      /* Le 22/06/04 */
      /* Si la transition Y a ete simulee par le rcvr, top_ptr peut etre vide!! */
      if (is_error_detected && top_ptr == NULL) {
	sxba_empty (ap_set);
	SXBA_0_bit (RL_nt_set, Y);

	return;
      }
#endif /* is_rcvr */
#endif /* 0 */

#if EBUG
      if (top_ptr == NULL)
	sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */

      bot_ptr = &(RTj->items_stack [LHS_NT2WHERE (Y)]);

      while (--top_ptr >= bot_ptr) {
	item = *top_ptr;

#if EBUG
	if (item < 0)
	  sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */

	cur_prod = PROLIS (item);

	if (SXBA_bit_is_reset_set (prod_order_set, cur_prod)) {
	  PUSH (prod_order_stack, cur_prod);
#if LLOG
	  fputs ("Pushing prod ", stdout);
	  output_prod (cur_prod);
#endif /* LLOG */
	}
      }
    }
  }

#if LLOG
  fputs ("Leaving RL_mreduction_item\n", stdout);
#endif /* LLOG */
}




static void
make_axiom ()
{
  SXINT   i, j, j_prime, k, resume1, resume2, Aij, triple, prev_triple, item_k;
  SXBA    backward_index_set;
#if is_rcvr
  if (SXTRUE) {
    if (!is_error_detected)
      MAKE_Aij (Aij, 1, 1, n+1);
    else {
      /* On va examiner la super regle */
      /* elle peut avoir plusieurs formes du type
	 0 -> $ [1] [j] S [k] [n+1] $ */

      /* Attention, ds make_axiom, glbl_dag_rcvr_start_kitems_set n'est pas disponible (ainsi que backward), on
	 simule donc a la main */
      /* 1. Calcul de k */
      j = 0; /* pas calcule' */
      resume2 = get_rcvr_resume (n+1, 2);
      
      if (resume2) {
	/* il y a du resume sur 
	   0 -> $  S [n+1] $ */
	triple = 0;

#if EBUG
	prev_triple = 0;
#endif /* EBUG */

	XxYxZ_Zforeach (glbl_dag_rcvr_start_kitems, resume2, triple) {
#if EBUG
	  if (prev_triple != 0)
	    sxtrap (ME, "make_axiom");

	  prev_triple = triple;
#endif /* EBUG */
	  k = XxYxZ_X (glbl_dag_rcvr_start_kitems, triple);
	  item_k = XxYxZ_Y (glbl_dag_rcvr_start_kitems, triple);

	  if (item_k == 2) {
	    /* cas  0 -> $  S [k] [n+1] $ */
	  }
	  else {
	    /* cas  0 -> $  [k] S [n+1] $ */
#if EBUG
	    if (item_k != 1 || k != 1)
	      sxtrap (ME, "make_axiom");
#endif /* EBUG */

	    k = n+1;
	    j = k;
	  }
	}
      }
      else {
	k = n+1;
      }

      if (j == 0) {
	/* 2. Calcul de j */
	backward_index_set = PT [k].backward_index_sets [2];
	/* dans un premier temps, on cherche à savoir s'il y a eu suppression de matériau 
	   à gauche du S qu'on va retenir */
	j = sxba_1_rlscan (backward_index_set, k+1);

	if (j > 1) {
	  /* il doit y avoir du rattrapage entre [1] et [j] */
	  if ((resume1 = get_rcvr_resume (j, 1)) > 0) {
	    triple = XxYxZ_is_set (&glbl_dag_rcvr_start_kitems, 1, 1 /* meme_item */, resume1);

#if EBUG
	    if (triple == 0)
	      sxtrap (ME, "make_axiom (the expected deletion between 1 and j doesn't correspond to a found rcvr)");
#endif /* EBUG */

	    /* erreur de type [1] [j] S */
	    /* On sait que j est sur l'item 1 */
#if EBUG
	    j_prime = sxba_scan (backward_index_set, -1);

	    if (j_prime != j)
	      sxtrap (ME, "make_axiom (two concurrent deletions before the axiom seem to have been found, which is impossible)");
#endif /* EBUG */

	    j = 1;
	  }
#if EBUG
	  else
	    sxtrap (ME, "make_axiom (the expected deletion between 1 and j doesn't correspond to a found rcvr)");
#endif /* EBUG */
	}
      }

      /* k et j devraient etre bons, meme s'il y a de la rcvr sur l'item 1 !! */
      MAKE_Aij (Aij, 1, j, k);
    }
  }
  else
#endif /* is_rcvr */
    MAKE_Aij (Aij, 1, 1, n+1);
  
  spf.outputG.start_symbol = Aij;
}


static void
RL_mreduction (SXBOOLEAN is_normal_pass)
{
  SXINT				i, j, A, init_A, A0j, Aij, t;
  SXBA			        ap_set, new_item_set;
  struct recognize_item	        *RTj;
  struct PT2_item		*PT2j;
  SXINT				*PT2j_shift_NT_stack, x, item, new_item, new_j, prod, X;
  SXBA			        *PT2j_index_set, *PT2j_backward_index_set, I, B, ids;
  SXBA			        index_set, new_index_set, red_order_set, wis;
  SXBOOLEAN			is_good, is_wis_empty;
#if is_rcvr
  SXBA                          tmp_rcvr_order_set;
#endif /* is_rcvr */
#if is_right_recursive
  SXINT				maxo, new_order, k_logprob_changed;
  SXBOOLEAN			must_loop;
#endif /* is_right_recursive */
#if REDUCER_KIND
  struct logprob_struct   *logprob_ptr;
  SXBA wis2 = NULL;
#if EBUG
  SXBA has_reduced_at_least_once_on_A, has_tried_to_reduce_on_A;
#endif /* EBUG */

  if (reducer_beam_value_is_set)
    wis2 = sxba_calloc (n+1+1);

#if EBUG
  has_tried_to_reduce_on_A = sxba_calloc (MAXNT+1);
  has_reduced_at_least_once_on_A = sxba_calloc (MAXNT+1);
#endif /* EBUG */
#endif /* REDUCER_KIND */

  pass_number = 2; /* reduceur */

  /* ESSAI !! */
  /* n += rcvr_tok_no-1; */
  n = idag.final_state-1; /* A FAIRE : remplacer les n+1 par idag.final_state */

  if (is_normal_pass && is_parse_forest)
    make_axiom (); /* Le 16/02/06 */

  RL_nt_set = sxba_calloc (MAXNT+1);
  prod_order_set = sxba_calloc (MAXPROD+1);
  prod_order_stack = (SXINT*) sxalloc (MAXPROD+1, sizeof (SXINT)), TOP (prod_order_stack) = 0;

  if (is_normal_pass && (is_parse_forest
#if is_guide || is_supertagging || EBUG
      || SXTRUE
#endif /* is_guide || is_supertagging || EBUG */
					)
      )
    new_item_set = sxba_calloc (MAXITEM+1);

  wis = sxba_calloc (n+1+1);
#if is_rcvr
  tmp_rcvr_order_set = is_error_detected ? sxba_calloc (MAXPROD+1) : NULL;
#endif /* is_rcvr */

#if is_epsilon || is_rcvr
  level_index_sets = sxbm_calloc (MAXRHSLGTH+1, n+1+1);
#endif /* is_epsilon || is_rcvr */

  index_set = PT2 [n+1].index_sets [2] = sxba_bag_get (&pt2_bag, n+1+1);
  SXBA_1_bit (index_set, 0); /* {0} */
  PUSH (PT2 [n+1].shift_NT_stack, 2); /* S' -> $ S . $ */

#if REDUCER_KIND
  if (reducer_beam_value_is_set) {
    ei_set = sxba_calloc (XxY_size (ei_hd) + 1);
    AxI_J_set = sxba_calloc (XxY_size (AxI_J_hd) + 1);
    ei2get_logprob_ptr (2 /* S' -> $ S . $ */, p_q2pq (0 /* S' -> . $ S $ */, idag.final_state), logprob_ptr);
    SXBA_1_bit (ei_set, ei);
    logprob_ptr->outside = logprob_ptr->inside.right = 0;
    /* logprob_ptr->prefix = logprob_ptr->inside.right = logprob_ptr->suffix = 0; */
    best_tree_logprob = logprob_ptr->inside.left;
    /* Il est possible de donner des valeurs de beam differentes entre le reconnaisseur et le reduceur */
    min_acceptable_tree_logprob = best_tree_logprob + reducer_beam_value;
#if LOG
    printf ("***************** min_acceptable_tree_logprob = " SXLPD " *****************\n", SXLOGPROB_display (min_acceptable_tree_logprob));
#endif /* LOG */
  }
#endif /* REDUCER_KIND */

#if HAS_BEAM == 2
#ifdef BEAM_MEASURE
  if (is_normal_pass) {
    SXINT old_size;

    if (idag.final_state > controlled_beam_max_final_state) {
      if (controlled_beam_min_lgth2logprob == NULL)
	controlled_beam_min_lgth2logprob = (SXLOGPROB *) sxcalloc (idag.final_state+1, sizeof (SXLOGPROB));
      else
	controlled_beam_min_lgth2logprob = (SXLOGPROB *) sxrecalloc (controlled_beam_min_lgth2logprob, controlled_beam_max_final_state+1, idag.final_state+1, sizeof (SXLOGPROB));

      controlled_beam_max_final_state = idag.final_state;
    }
  }
#endif /* BEAM_MEASURE */
#endif /* HAS_BEAM == 2 */

  for (j = n+1; j >= 0; j--) {
    PT2j = &(PT2 [j]);
    RTj = &(RT [j]);

    wis [0] = j+1; /* On change la taille!! */

    PT2j_index_set = &(PT2j->index_sets [0]);
    PT2j_backward_index_set = &(PT2j->backward_index_sets [0]);
    PT2j_shift_NT_stack = &(PT2j->shift_NT_stack [0]);

#if is_rcvr
    /* Ajoute' le 22/03/07 ?? */
    if (is_error_detected)
      sxba_empty (tmp_rcvr_order_set);
#endif /* is_rcvr */

    for (x = PT2j_shift_NT_stack [0]; x > 0; x--) {
      item = PT2j_shift_NT_stack [x];
      /* item = B -> \alpha A . X \beta */

#if EBUG
      if (item < 0 && !is_error_detected) {
	sxtrap (ME, "RL_mreduction (called with a negative item whereas no rcvr activated or no error detected by rcvr)");
      }
#endif /* EBUG */

#if is_rcvr
      if (item < 0) {
	/* on vient donc de reculer d'une position reduce en restant sur la position reduce à cause de rcvr */
	item = -item;
	prod = PROLIS (item);
	SXBA_1_bit (tmp_rcvr_order_set, prod);
      }
#endif /* is_rcvr */

#if HAS_BEAM == 2
      if (i2valid_item_set) SXBA_1_bit (i2valid_item_set [j], item);
#endif /* HAS_BEAM == 2 */

      I = PT2j_index_set [item];
#if 0
      /* Le 20/09/2002, je ne comprends plus ce test qui trappe sur un exemple... */
      /* A -> \alpha X1 X2 X3, X1=X2=X3 =>* \varepsilon
	 A -> \alpha X1 X2. X3 et A -> \alpha X1. X2 X3 sont ds shift_NT_stack et le
	 traitement de A -> \alpha X1 X2. X3 positionne le backward_index_set de 
	 A -> \alpha X1. X2 X3 */
#if EBUG
      if (I == NULL || PT2j_backward_index_set [item] != NULL)
	sxtrap (ME, "RL_mreduction");
#endif /* EBUG */
#endif /* 0 */

	RL_mreduction_item (item, I, j, 0);
    }

    red_order_set = red_order_sets [j];

    /* prod_order_set est positionne par RL_mreduction_item par le traitement des shifts. */
    if (!IS_EMPTY (prod_order_stack)) {
      do {
	prod = POP (prod_order_stack);
	SXBA_0_bit (prod_order_set, prod);

#if LLOG
	fputs ("Popping prod ", stdout);
	output_prod (prod);
#endif /* LLOG */

	item = PROLON (prod+1)-1;
	A = LHS (prod);
	ap_set = ap_sets [A]; /* prod est une Akj-prod avec k \in ap_set */

#if REDUCER_KIND
#if EBUG
	SXBA_1_bit (has_tried_to_reduce_on_A, A);
#endif /* EBUG */
#endif /* REDUCER_KIND */

	is_good = SXFALSE;
				
#if is_epsilon
	if (item == PROLON (prod)) {
	  /* item = A -> . */
	  if (SXBA_bit_is_set (ap_set, j)) {
	    /* Le 22/02/06, on ne fait TOUTE la suite que si le test est avere */
		
#if HAS_BEAM == 2
	    if (i2valid_item_set)
	      SXBA_1_bit (i2valid_item_set [j], item);
	    else
#endif /* HAS_BEAM == 2 */
	      {
		is_good = SXTRUE;

#if REDUCER_KIND
		if (reducer_beam_value_is_set) {
		  SXINT               j_logprob_changed;

		  Axpq2get_max_logprob_ptr (A, p_q2pq (j, j));
		  j_logprob_changed = set_reduce_item_logprob (item);

		  if (j_logprob_changed <= 0)
		    /* Echec car hors du beam, doit etre supprimee */
		    is_good = SXFALSE;
		}
#endif /* REDUCER_KIND */

		if (is_good) {
		  /* Le 08/02/06 */
		  /* Majorant du nb de Aij */ 
#ifdef MAKE_INSIDEG
		  init_A = spf.insideG.nt2init_nt [A];
#else /* !MAKE_INSIDEG */
		  init_A = A;
#endif /* !MAKE_INSIDEG */

		  MAKE_Aij (Aij, init_A, j, j);

#if LOG
		  output_ni_prod (prod, NULL, j, j);
#endif /* LOG */

#if is_supertagging
		  /* On appelle aussi supertagger sur clauses non-lexicalisees pour les comptabiliser */
		  call_supertagger (item, 0);
#endif /* is_supertagging */

#if is_guide
		  output_guide (prod, NULL, j, j);
#endif /* is_guide */
		}
	      }
	  }
	}
	else
#endif /* is_epsilon */
	  {
	    /* item = A -> \alpha X . */
#if HAS_BEAM == 2
	    if (reducer_beam_value_is_set && i2valid_item_set == NULL)
	      /* 2eme passe du reduceur, on utilise l'index_set de la 1ere passe */
	      index_set = PT2_save [j].index_sets [item];
	    else
#endif /* HAS_BEAM == 2 */
	      index_set = RTj->index_sets [item];

	    if (index_set) {
	      if ((new_index_set = PT2j->index_sets [item]) == NULL) {
		new_index_set = sxba_bag_get (&pt2_bag, j+1);
	      }

	      is_wis_empty = SXTRUE;

	      if (OR_AND_MINUS (new_index_set, index_set, ap_set, wis)) {
		/* calcule wis = (index_set & ap_set) - new_index_set
		   et new_index_set |= wis
		   retourne vrai ssi wis est non vide 
		   wis est l'ensemble des k valides pour prod et non encore traites */

		/* Il y a du nouveau */
#if REDUCER_KIND
		if (reducer_beam_value_is_set) {
		  SXINT  i, j_logprob_changed;
		    
		  i = -1;

		  while ((i = sxba_scan (wis, i)) >= 0) {
		    Axpq2get_max_logprob_ptr (A, p_q2pq (i, j));
		    j_logprob_changed = set_reduce_item_logprob (item);

		    if (j_logprob_changed <= 0) {
		      /* Echec car hors du beam, doit etre supprimee */
		      SXBA_0_bit (wis, i);
		      SXBA_0_bit (new_index_set, i);
		      SXBA_1_bit (wis2, i); /* pour éviter de les réexecuter juste après */
		    }
		    else {
		      is_wis_empty = SXFALSE;
		    }
		  }
		}
		else
#endif /* REDUCER_KIND */
		  is_wis_empty = SXFALSE;
	      }

#if REDUCER_KIND
	      if (reducer_beam_value_is_set) {
		SXINT               i, j_logprob_changed;

		i = -1;
		    
		while ((i = sxba_scan (ap_set, i)) >= 0) {
		  if (SXBA_bit_is_set (index_set, i)) {
		    /* Ce n'est pas new_index_set */
		    if (!SXBA_bit_is_set_reset(wis2, i) /* on vient de l'exécuter, inutile de le refaire */
			&& !SXBA_bit_is_set (wis, i)) {
		      /* On regarde les index deja presents qui ont a voir avec la choucroute, car les nouvelles proba
			 peuvent etre meilleures */
		      Axpq2get_max_logprob_ptr (A, p_q2pq (i, j));
		      j_logprob_changed = set_reduce_item_logprob (item);

		      if (j_logprob_changed > 0) {
			/* c'est le cas ici ... */
			SXBA_1_bit (wis, i); /* On va recommencer */
			is_wis_empty = SXFALSE;
#if LLOG
			printf ("  reexecution of this reducer item kept by beaming for indexes [" SXILD ".." SXILD "]: ", i, j);
			output_item (item, NULL, i, j);
#endif /* LLOG */
		      }
		    }
		  }
		}
	      }
#endif /* REDUCER_KIND */

	      if (!is_wis_empty) {
		/* Le 19/09/2002 : on [re]affecte new_index_set a PT2j->index_sets [item], ca
		   permet de ne pas avoir des index_set non NULL et vides */
		PT2j->index_sets [item] = new_index_set;

		/* MODIF: c'est wis qui decide l'arret des boucles */
		
#if HAS_BEAM == 2
		if (i2valid_item_set) SXBA_1_bit (i2valid_item_set [j], item);
#endif /* HAS_BEAM == 2 */

		RL_mreduction_item (item, wis, j, 0);
		is_good = SXTRUE;
	      }
#if is_rcvr
	      else {
		/* ici wis == NULL */
		/* Attention on peut etre dans "cas-tordu" :
		   On est avec A -> [i] \alpha [j] [k], j < k
		   Le cas A-> [i] \alpha [k] a deja ete traite quand j a valu k mais il n'a
		   pas ete conside're' comme un reduce (le traitement pour \alpha a deja ete memorise )
		*/
		if (is_error_detected && SXBA_bit_is_set (tmp_rcvr_order_set, prod)) {
		  if (IS_AND (index_set, ap_set)) {
		    is_good = SXTRUE;
		  }
		}
	      }
#endif /* is_rcvr */

	      if (is_good) {
		if (is_normal_pass && (is_parse_forest
#if is_guide || is_supertagging || EBUG
				       || SXTRUE
#endif /* is_guide || is_supertagging || EBUG */
				       )
		    )
		  SXBA_1_bit (new_item_set, item);
	      }
	    }
	  }

	if (is_good) {
	  SXBA_1_bit (red_order_set, prod);
#if REDUCER_KIND
#if EBUG
	  SXBA_1_bit (has_reduced_at_least_once_on_A, A);
#endif /* EBUG */
#endif /* REDUCER_KIND */
	}
      } while (!IS_EMPTY (prod_order_stack));		  

#if REDUCER_KIND
#if EBUG
      if (sxba_first_difference (has_tried_to_reduce_on_A, has_reduced_at_least_once_on_A) >= 0)
	sxtrap (ME, "RL_mreduction (could not reduce on at least one LHS symbol)");

      sxba_empty (has_reduced_at_least_once_on_A);
      sxba_empty (has_tried_to_reduce_on_A);
#endif /* EBUG */
#endif /* REDUCER_KIND */

      /* Le 28/08/2003 */
      if (is_normal_pass && (is_parse_forest
#if is_guide || is_supertagging || LOG
	  || SXTRUE
#endif /* is_guide || is_supertagging || EBUG */
					    )
	  ) {
	/* Ici, normalement, toutes les lhs instanciees des prod que l'on trouve sont reliees a l'axiome
	   => On est + precis que le reconnaisseur */
	/* Attention les RL_nt_set et les ap_sets doivent etre razes */
	/* Si la rhs des prod est ds T* => il y a eu un appel direct */
	new_item = 0;

	while ((new_item = sxba_scan_reset (new_item_set, new_item)) > 0) {
	  prod = PROLIS (new_item);
	  index_set = PT2 [j].index_sets [new_item];

	  /* Le 08/02/06 */
	  A = LHS (prod); 
#ifdef MAKE_INSIDEG
	  init_A = spf.insideG.nt2init_nt [A];
#else /* !MAKE_INSIDEG */
	  init_A = A;
#endif /* !MAKE_INSIDEG */

	  i = -1;

#if LOG
	  output_ni_prod (prod, index_set, 0, j);
#endif /* LOG */
	  
	  while ((i = sxba_scan (index_set, i)) >= 0) {
	    /* Majorant du nb de Aij */
	    MAKE_Aij (Aij, init_A, i, j);
	  }

#ifdef BEAM_MEASURE
#if is_rcvr
  if (!is_error_detected)
    /* On ne prend pas de mesure si une erreur est detectee */
#endif /* is_rcvr */
    {
      if (is_normal_pass) {
	/* On sort les mesures des reduces non vides */
	SXINT               item, init_prod, init_item, i, ij;
	struct logprob_struct *logprob_ptr;

#ifdef MAKE_INSIDEG
	init_prod = spf.insideG.prod2init_prod [prod];
#else /* MAKE_INSIDEG */
	init_prod = prod;
#endif /* MAKE_INSIDEG */

	init_item = prolon [init_prod+1] - 1;
	item = PROLON (prod+1)-1;

	i = -1;

	while ((i = sxba_scan (index_set, i)) >= 0) {
	  ij = p_q2pq (i, j);
	  ei2set_logprob_ptr (item, ij, logprob_ptr);

	  put_a_controlled_beam_measure (init_item, range2min_lgth [ij], logprob_ptr->inside.left);

#if LLLOG
	  printf ("Made and filled ei: (item=" SXILD ", pq=" SXILD ") [ei=" SXILD " (%s)] {inside.left = " SXLPD "}: ", item, ij, ei, is_ei_set ? "old"  : "new", SXLOGPROB_display (j_logprob_ptr->inside.left));
	output_ei (ei);
#endif /* LLLOG */
	}
      }
    }
#endif /* BEAM_MEASURE */

#if is_guide
	  output_guide (prod, index_set, 0, j);
#endif /* is_guide */

#if is_supertagging
	  /* On appelle aussi supertagger sur les clauses non-lexicalisees pour les comptabiliser */
	  if (prod2t_nb [prod] == 0)
	    call_supertagger (new_item, 0);
#endif /* is_supertagging */
	}
      }

      A = 0;
	
      while ((A = sxba_scan_reset (RL_nt_set, A)) > 0) {
	sxba_empty (ap_sets [A]);
      }
    }
  } /* End for (j = n+1; j >= 0; j--) */

#if HAS_BEAM == 2
#ifdef BEAM_MEASURE
  if (is_normal_pass) {
    sxfree (range2min_lgth), range2min_lgth = NULL;
  }
#endif /* BEAM_MEASURE */
#endif /* HAS_BEAM == 2 */
    
  sxfree (RL_nt_set), RL_nt_set = NULL;
  sxfree (prod_order_set), prod_order_set = NULL;
  sxfree (prod_order_stack), prod_order_stack = NULL;
  sxfree (wis);

#if REDUCER_KIND  
  if (wis2)
    sxfree (wis2);

#if EBUG
  sxfree (has_tried_to_reduce_on_A);
  sxfree (has_reduced_at_least_once_on_A);
#endif /* EBUG */
#endif /* REDUCER_KIND */

#if is_rcvr
  if (tmp_rcvr_order_set)
    sxfree (tmp_rcvr_order_set);
#endif /* is_rcvr */

#if REDUCER_KIND
  if (reducer_beam_value_is_set) {
    sxfree (ei_set), ei_set = NULL;
    sxfree (AxI_J_set), ei_set = NULL;
  }
#endif /* REDUCER_KIND */

#if is_epsilon || is_rcvr
  sxbm_free (level_index_sets), level_index_sets = NULL;
#endif /* is_epsilon || is_rcvr */

  if (is_normal_pass && (is_parse_forest
#if is_guide || is_supertagging || EBUG
      || SXTRUE
#endif /* is_guide || is_supertagging || EBUG */
					)
      )
    sxfree (new_item_set); 

  if (is_print_time) {
    char mess [64];

#if REDUCER_KIND
    if (reducer_beam_value_is_set)
      sprintf (mess, "\tBeamed Earley Reducer");
    else
#endif /* REDUCER_KIND */
      sprintf (mess, "\tEarley Reducer");

    sxtime (TIME_FINAL, mess);
  }
} /* RL_mreduction */


#if is_rcvr
static SXBA glbl_dag_rcvr_messages_set;
static SXINT  *rcvr_message2repair_parse_id;

#endif /* is_rcvr */
#endif /* is_parser */

/* Le 14/09/06 suppression de get_eof_val() */




/* Appele' depuis spf_fill_Tij2tok_no () */
/* Tpq provient de la rcvr */
SXINT
get_repair_Tpq2tok_no (SXINT Tpq)
{
#if is_rcvr
  return (repair_Tpq2tok_no) ? repair_Tpq2tok_no [X_is_set (&repair_Tpq_hd, Tpq)] : 0 /* terminal en erreur (mais pas de reparation) */;
#else
  sxtrap (ME, "get_repair_Tpq2tok_no (called whereas is_rcvr is not defined)");
  return 0;
#endif /* is_rcvr */
}


#if is_rcvr
static SXBA rcvr_Pij_set;

static SXINT
rcvr_bu_walk (prod)
     SXINT prod;
{
  SXBA_1_bit (rcvr_Pij_set, prod);

  return 1;
}

/* void local_fill_Tij2tok_no (){...} */
/* Remplace' le 14/02/08 par spf_fill_Tij2tok_no () qui peut appeler get_repair_Tpq2tok_no () */




static SXBOOLEAN    use_a_single_rcvr_string;
static XxY_header rcvr_out_range_memo;

/* A cause de la rcvr, on reconstruit un source fictif qui remplace le source entre lb et ub */
/* Ce source est (pre-)concatene aux commentaires des tokens commencant en ub */
/* Retourne le nb de terminaux de la chaine corrigee */
/* Pour un [lb..ub] donne' si cette ft est appelee plusieurs fois, les 2eme et suivantes, elle retourne un varstr vide (le 21/02/08) */
static SXBOOLEAN
is_rcvr_out_range_first_call (SXINT lb, SXINT ub)
{
  SXINT range;

  if (!XxY_is_allocated (rcvr_out_range_memo)) {
    sxprepare_for_possible_reset (rcvr_out_range_memo.is_allocated);
    XxY_alloc (&rcvr_out_range_memo, "rcvr_out_range_memo", idag.final_state+1 /* !! */, 1, 0, 0, NULL, NULL);
  }

  return !XxY_set (&rcvr_out_range_memo, lb, ub, &range);
}

VARSTR 
rcvr_out_range (VARSTR wvstr, SXINT lb, SXINT ub)
{
  SXINT          ldag_state, rdag_state, nb, triple, ste, t, init_t, xbuf, trans;
  SXINT          bot_pq, top_pq, tok_no, top_tok_no;
  SXINT          *tok_no_stack, *top_tok_no_stack;
  char           *str, *str2;
  SXBA           t_set;
  SXBOOLEAN        has_empty_ste;
  struct sxtoken *ptoken;

  varstr_raz (wvstr);

  if (is_rcvr_out_range_first_call (lb, ub)) {
    varstr_raz (varstring);

    if (lb < ub) {
      /* Le source est non vide */
      /* recup des commentaires */
      ldag_state = mlstn2dag_state (lb);
      rdag_state = mlstn2dag_state (ub);
    
      varstring = sub_dag_to_comment (varstring, ldag_state, rdag_state);
    }

    nb = 0;

    if (XxYxZ_is_allocated (store_rcvr_string_hd)) {
      /* On n'est pas ds le cas correction, on va donc prendre les terminaux de la rcvr ds store_rcvr_string_hd */
      /* maintenant, on recupere les corrections */
      has_empty_ste = SXFALSE;

      XxYxZ_YZforeach (store_rcvr_string_hd, lb, ub, triple) {
	ste = XxYxZ_X (store_rcvr_string_hd, triple);

	if (ste == SXEMPTY_STE)
	  has_empty_ste = SXTRUE;

	if (nb++ > 0)
	  break;
      }

      if (nb == 1 && has_empty_ste) {
	/* ... corrige' par la chaine vide.  On ne met que les commentaires !! */
	if (varstr_length (varstring)) {
	  wvstr = varstr_catenate (wvstr, varstr_tostr (varstring));
	}

	nb = 0;
      }

      if (nb > 0) {
	if (nb > 1 && !use_a_single_rcvr_string) {
	  if (has_empty_ste)
	    wvstr = varstr_catchar (wvstr, '[');
	  else
	    wvstr = varstr_catchar (wvstr, '(');
	}

	nb = 0;

	XxYxZ_YZforeach (store_rcvr_string_hd, lb, ub, triple) {
	  ste = XxYxZ_X (store_rcvr_string_hd, triple);

	  if (ste != SXEMPTY_STE) {
	    if (nb++ > 0)
	      wvstr = varstr_catenate (wvstr, " | ");
    
	    str = sxstrget (ste);
	    /* str est de la forme "t1 t2 ... tp"
	       on fabrique "comment _error__t1 comment _error__tp ... comment _error__tp"
	    */

	    do {
	      if (varstr_length (varstring)) {
		wvstr = varstr_catenate (wvstr, varstr_tostr (varstring));
		wvstr = varstr_catchar (wvstr, ' ');
	      }

	      wvstr = varstr_catenate (wvstr, "_error__");

	      str2 = strchr (str, ' ');

	      if (str2 == NULL)
		wvstr = varstr_catenate (wvstr, str);
	      else {
		wvstr = varstr_lcatenate (wvstr, str, str2-str);
		str = str2+1;
	      }
	    } while (str2);
	  }

	  if (use_a_single_rcvr_string && nb == 1)
	    break;
	}

	if (!use_a_single_rcvr_string && (nb > 1 || has_empty_ste)) {
	  if (has_empty_ste)
	    wvstr = varstr_catchar (wvstr, ']');
	  else
	    wvstr = varstr_catchar (wvstr, ')');

	  varstr_complete (wvstr);
	}
      }
    }
    else {
      /* Correction on est obligatoirement ds le cas  [lb::ub] de la suppression de symboles */
      /* On ne conserve donc que les commentaires associes a ces symboles */
      if (varstr_length (varstring)) {
	wvstr = varstr_catenate (wvstr, varstr_tostr (varstring));
      }
    }

    /* Le 18/02/08 c'est l'appelant qui sait quoi faire de wvstr */
  }

  return wvstr;
}
#else /* !is_rcvr */
/* Bidon car linke' avec spf_mngr */
VARSTR
rcvr_out_range (VARSTR vstr, SXINT lb, SXINT ub)
{
  return vstr;
}
#endif /* is_rcvr */




#ifdef has_output_nt
#define has_output_nt_or_output_only_special_terminals
#endif /* has_output_nt */

#ifndef has_output_nt_or_output_only_special_terminals
#ifdef output_only_special_terminals
#define has_output_nt_or_output_only_special_terminals
#endif /* output_only_special_terminals */
#endif /* has_output_nt_or_output_only_special_terminals */


/* Appele' depuis spf_mngr:spf_first_yield_bu_walk et ... */
SXINT
special_Apq_name (char *string, SXINT lgth)
{
  SXINT which_case = 0;

#ifdef has_output_nt
  if (lgth > 0 && string [lgth-1] == '!') {
    which_case = 1;

    if (lgth > 1 && string [lgth-2] == ':')
      which_case += 2;
    
    if (string [0] == ':')
      which_case += 4;
  }
#endif /* has_output_nt */

  return which_case;
}

SXINT
special_Tpq_name (char *string, SXINT lgth)
{
  SXINT which_case;

#ifdef output_only_special_terminals
  if (lgth > 1 && string [lgth-1] == '!')
    which_case = 1;
  else
    which_case = 0;
#else
  which_case = -1;
#endif /* output_only_special_terminals */
  
  return which_case;
}

#ifdef MAKE_INSIDEG
extern void idag_t_stack_restore ();
#endif /* MAKE_INSIDEG */

SXBOOLEAN
sxearley_parse_it (SXINT what)
{
  SXINT 	  t, nt, size, S1n;
  SXBOOLEAN ret_val = SXTRUE;
  SXBA    t_set;

  switch (what) {
  case SXOPEN: /* 1 fois pour toutes avant la 1ere phrase */
#if HAS_BEAM == 2
    saved_recognizer_scan_beam_value_is_set = recognizer_scan_beam_value_is_set;
    saved_reducer_beam_value_is_set = reducer_beam_value_is_set;
#endif /* HAS_BEAM == 2 */

#ifdef SEMANTICS
    if (is_no_semantics) {
      for_semact.sem_open = NULL;
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
	for_semact.sem_open = NULL;
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
    }


#if is_parser
    {
      is_semact_fun = for_semact.semact != NULL;

      /* Finalement le 20/09/06 on construit tj la foret partagee !! */
      is_parse_forest = SXTRUE;

#if is_full_guide
      /* is_print_prod = SXTRUE; ESSAI */
#ifdef MAKE_INSIDEG
      lb = (SXINT*) sxcalloc (MAXRHSLGTH+1, sizeof (SXINT));
      ub = (SXINT*) sxcalloc (MAXRHSLGTH+1, sizeof (SXINT));
#endif /* MAKE_INSIDEG */
      for_semact.sem_init = NULL;
      for_semact.sem_final = NULL;
      for_semact.sem_close = NULL;
      for_semact.semact = output_full_guide;
      for_semact.parsact = NULL;
      for_semact.prdct = NULL;
      for_semact.constraint = NULL;
      for_semact.sem_pass = NULL;
      for_semact.scanact = NULL;
#endif /* is_full_guide */
    }
#endif /* is_parser */
    /* else is_default_semantics, on execute la semantique specifiee par la grammaire. */

    if (for_semact.sem_open) {
      (*for_semact.sem_open) ();
    }
#endif /* SEMANTICS */

    break;

  case SXACTION:
    sxearley_raz ();

#ifdef bnf21rcg_include_file
    if (inputG_MAXITEM != rcg_itemmax) {
      fputs ("\nThe Earley/LC automaton and the RCG Lex structures are incompatible.\n", sxstderr);
      sxexit(1);
    }
#endif /* bnf21rcg_include_file */

#if MEASURES
    /* On est ds une campagne de mesures ... */
    printf ("MEASURES: There are " SXILD " input words which select " SXILD " terminal transitions.\n", last_tok_no, idag.ptq_nb);
#endif /* MEASURES */

    working_rcvr_mlstn = 1+n+delta_n+1;

#if MEASURES
    /* Pour la campagne de mesure */
    MEASURE_time (TIME_INIT);
#endif /* MEASURES */

    /* Traitement de la lexicalisation */
    is_mlstn2non_lexicalized_look_ahead_t_set = SXTRUE;
#if is_lex
    is_mlstn2lexicalized_look_ahead_t_set = SXTRUE;
#endif /* is_lex */


#if is_lex  
    /* Le comportement de lexicalizer2basic_item_set dépend de si on a compilé le lexicalizer_mngr avec MAKE_INSIDEG
       (impliqué par HUGE_CFG) ou non:
       
       MAKE_INSIDEG => insideG est construite. C'est la + petite grammaire reduite possible (ca depend des options du
       lexicalizer) construite. Dans ce cas basic_item_set est inutile (il est plein)
       !MAKE_INSIDEG => basic_item_set sert de filtre sur la inputG (insideG non construite)
	
       A priori si SXTRUE, l'analyse est + rapide et prend moins de place mais le rattrapage d'erreur eventuel ne se fera
       que sur cette grammaire restreinte et pourrait donc etre moins bon/pertinent.
    */

    if (lexicalizer2basic_item_set (is_mlstn2lexicalized_look_ahead_t_set, is_mlstn2non_lexicalized_look_ahead_t_set, /* is_smallest_insideG */ SXTRUE)) {
      if (spf.insideG.is_allocated) /* la insideG a été construite, car le lexicalizer_mngr a été compilé avec MAKE_INSIDEG */
	/* dans ce cas, le lexicaliseur nous a fourni une grammaire parfaite. Inutile de garder des structures detinées
	   à en filtrer des parties */
	sxfree (basic_item_set), basic_item_set = NULL; /*inutile */
      ilex_compatible_item_set = lex_compatible_item_set = basic_item_set;
      mlstn2look_ahead_t_set = mlstn2lexicalized_look_ahead_t_set = mlstn2lex_la_tset; /* global definie ds lexicalizer_mngr */
      mlstn2non_lexicalized_look_ahead_t_set = mlstn2la_tset;
#  if has_Lex
      /* On supprime aussi les items invalides dus aux &Lex */
      sxba_and (lex_compatible_item_set, Lex_prod_item_set);
      equiv_prod_Lex_filter (lex_compatible_item_set);
#  endif /* has_Lex */
    }
    else {	/* ... et rien ds le suffixe n'a ete calcule. */
      /* Echec du lexicalizer => le source n'est pas ds le langage */
      ret_val = SXFALSE;

      ilex_compatible_item_set = NULL;
      if (mlstn2lexicalized_look_ahead_t_set) sxbm_free (mlstn2lexicalized_look_ahead_t_set), mlstn2lexicalized_look_ahead_t_set = NULL;
      mlstn2look_ahead_t_set = mlstn2non_lexicalized_look_ahead_t_set = mlstn2la_tset;

#if !is_rcvr
      /* ... et pas de rattrapage, il est donc inutile de lancer l'analyse */
      /* Ca permet aussi de sortir : exit () de syntax avec le bon code */
      sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
	      "Syntax Error (sxearley_parse_it).",
	      sxplocals.sxtables->err_titles [2]);

#ifdef MAKE_INSIDEG
      spf_free_insideG (&spf.insideG);
#endif /* MAKE_INSIDEG */
      return ret_val;
#endif /* !is_rcvr */
    }
#else /* !is_lex */
    ilex_compatible_item_set = NULL;
    mlstn2la_tset = sxbm_calloc (1+n+1+2, inputG_SXEOF+1);
    idag_source_processing (SXEOF, NULL, NULL, mlstn2la_tset, NULL);
    mlstn2look_ahead_t_set = mlstn2non_lexicalized_look_ahead_t_set = mlstn2la_tset;
#  endif /* !is_lex */

    /* Ici, si ilex_compatible_item_set == NULL, soit la lexicalisation a echoue', soit elle n'a pas
       ete demande'e */


#if is_1la
      mlstn2look_ahead_item_set = sxbm_calloc (final_mlstn+1+1, MAXITEM+1);
      fill_mlstn2look_ahead_item_set (SXTRUE /* first_time */);
#endif /* is_1la */

    sxba_bag_alloc (&shift_bag, "shift_bag", (working_rcvr_mlstn+2) * MAXPROD * SXNBLONGS (working_rcvr_mlstn + 1), statistics);
#ifdef MAKE_INSIDEG
    {
      SXINT                   sizeof_shift_NT_hd, sizeof_items_stack, sizeof_index_sets, i;
      struct recognize_item *RTi;
      char                  *area_ptr;

      sizeof_shift_NT_hd = (MAXNT+1) * sizeof (SXINT *);
      sizeof_items_stack = (MAXITEM+1) * sizeof (SXINT);
      sizeof_index_sets = (MAXITEM+1) * sizeof (SXBA);
      RT = (struct recognize_item*) sxalloc (working_rcvr_mlstn+2, sizeof (struct recognize_item));
      area_ptr = RT_area = (char*) sxcalloc (working_rcvr_mlstn+2, sizeof_shift_NT_hd+sizeof_items_stack+sizeof_index_sets);

      for (i = 0; i <= working_rcvr_mlstn+1; i++) {
	RTi = &(RT [i]);
	RTi->shift_NT_hd = (SXINT**) area_ptr;
	area_ptr += sizeof_shift_NT_hd;
	RTi->items_stack = (SXINT*) area_ptr;
	area_ptr += sizeof_items_stack;
	RTi->index_sets = (SXBA*) area_ptr;
	area_ptr += sizeof_index_sets;
      }
    }
#else /* !MAKE_INSIDEG */
    RT = (struct recognize_item*) sxcalloc (working_rcvr_mlstn+2, sizeof (struct recognize_item));
#endif /* !MAKE_INSIDEG */
    RT_left_corner = sxbm_calloc (working_rcvr_mlstn+2, MAXNT+1);
    ntXindex_set = sxbm_calloc (MAXNT+1, working_rcvr_mlstn+2);
#if HAS_BEAM == 1
    {
      SXINT i;
      struct RT_global_logprobs *RT_global_logprobs_i;
      struct RT_logprobs *RT_logprobs_area_ptr;
      SXINT size = RHS_NT2WHERE(1); /* nombre d'items avec un point devant un terminal */

      RT_global_logprobs_area = (struct RT_logprobs*) sxalloc ((working_rcvr_mlstn+2)*size, sizeof (struct RT_logprobs));
      
      RT_global_logprobs = (struct RT_global_logprobs*) sxalloc (working_rcvr_mlstn+2, sizeof (struct RT_global_logprobs));

      RT_logprobs_area_ptr = RT_global_logprobs_area;
      for (i = 0; i <= working_rcvr_mlstn+1; i++) {
	RT_global_logprobs_i = &(RT_global_logprobs [i]);
	RT_global_logprobs_i->global_logprobs_top = 0;
	RT_global_logprobs_i->global_logprobs_allocated_size = size;
	RT_global_logprobs_i->global_logprobs = (SXLOGPROB *) sxalloc (size + 1, sizeof (SXLOGPROB));
	RT_global_logprobs_i->RT_logprobs = RT_logprobs_area_ptr;
	RT_logprobs_area_ptr += size;
      }
    }
#endif /* HAS_BEAM == 1 */
    
    fill_idag_q2pq_stack ();
    
#if is_rcvr
    rcvr_xcld_min_mlstn_set = sxba_calloc (working_rcvr_mlstn+2);
    store_non_kernel_item_sets = sxbm_calloc (working_rcvr_mlstn+2, MAXITEM+1);
    rcvr_kernel_item_sets = sxbm_calloc (working_rcvr_mlstn+2, MAXITEM+1);
    rcvr_reduce_item_sets = sxbm_calloc (working_rcvr_mlstn+2, MAXITEM+1);
#endif /* is_rcvr */

    non_kernel_item_sets = sxbm_calloc (working_rcvr_mlstn+2, MAXITEM+1);

#if RECOGNIZER_KIND == 2
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
      kernel_nt_sets = sxbm_calloc (working_rcvr_mlstn+2, MAXNT+1);
      earley_table_companion = (struct earley_table_companion*) sxcalloc (working_rcvr_mlstn+2, sizeof (struct earley_table_companion));
    }
#endif /* RECOGNIZER_KIND == 2 */

#if is_parser
#ifdef MAKE_INSIDEG
    {
      SXINT               sizeof_backward_index_sets, sizeof_reduce_NT_hd, i;
      struct parse_item *PTi;
      char              *area_ptr;

      sizeof_backward_index_sets = (MAXITEM+1) * sizeof (SXBA);
      sizeof_reduce_NT_hd = (MAXNT+1) * sizeof (SXINT*);
      PT = (struct parse_item*) sxcalloc (working_rcvr_mlstn+2, sizeof (struct parse_item));
      area_ptr = PT_area = (char*) sxcalloc (working_rcvr_mlstn+2, sizeof_backward_index_sets+sizeof_reduce_NT_hd);

      for (i = 0; i <= working_rcvr_mlstn+1; i++) {
	PTi = &(PT [i]);
	PTi->backward_index_sets = (SXBA*) area_ptr;
	area_ptr += sizeof_backward_index_sets;
	PTi->reduce_NT_hd = (SXINT**) area_ptr;
	area_ptr += sizeof_reduce_NT_hd;
      }
    }
#else /* !MAKE_INSIDEG */
    PT = (struct parse_item*) sxcalloc (working_rcvr_mlstn+2, sizeof (struct parse_item));
#endif /* !MAKE_INSIDEG */

#if is_full_guide
    /* for_semact.semact a pu etre ecrase si is_no_semantics ou is_parse_tree_number */
    is_output_full_guide = (for_semact.semact == output_full_guide);
#endif /* is_full_guide */
    is_constraint_fun = for_semact.constraint != NULL;
    is_prdct_fun = for_semact.prdct != NULL;
#endif /* is_parser */

#if (is_guide || is_full_guide || is_supertagging) && has_strlen
    /* La simple 1_RCG a des &Strlen, ils ont ete traduits en predicats <A>&p ou p
       est la longueur attendue.  Il faut donc appeler le predicat earley_strlen () */
    /* meme si on n'est pas ds le cas parseur */
    for_semact.prdct = earley_strlen;
    is_prdct_fun = SXTRUE;
#endif /* (is_guide || is_full_guide || is_supertagging) && has_strlen */

#if has_Lex
    clause_val_set = sxba_calloc (last_clause+1);
    prod_checked_set = sxba_calloc (MAXPROD+1);
    prod_val_set = sxba_calloc (MAXPROD+1);
#endif /* has_Lex */

#if is_supertagging
    used_non_lex_prod_set = sxba_calloc (MAXPROD+1);;
#endif /* is_supertagging */

#if is_make_oracle
    /* campagne de mesures */
#if is_parser
    /* Cas de l'oracle */
    prod2lb_set = sxbm_calloc (MAXPROD+1, n+1);
#else /* !is_parser */
    /* cas de la mesure pour le guide (lexicaliseur) */
    i2guide_item_set = sxbm_calloc (n+1, MAXITEM+1);
#endif /* !is_parser */
#endif /* is_make_oracle */

#if !is_parser && is_supertagging
    item2source_index_set = sxbm_calloc (MAXITEM+1, n+2);
#endif /* !is_parser && is_supertagging */

#if MEASURES
    /* Pour la campagne de mesures */
    MEASURE_time (TIME_FINAL);
#endif /* MEASURES */

    /* On ne lance pas recognize () si dynamic_set_automaton () a echoue' et s'il n'y a pas de traitement d'erreur ... */
    if (
#if is_rcvr
#ifndef HUGE_CFG
	SXTRUE ||
#endif /* HUGE_CFG */
#endif /* is_rcvr */
	ret_val) {
#if LOG
      fputs ("\n**** RECOGNIZER ****\n", stdout);
#endif /* LOG */

#if is_epsilon && is_rcvr
#ifdef MAKE_INSIDEG
      if (spf.insideG.is_eps) {
	non_kernel_empty_item_set = sxba_calloc (MAXITEM+1);
      }
#else /* !MAKE_INSIDEG */
      non_kernel_empty_item_set = sxba_calloc (MAXITEM+1);
#endif /* !MAKE_INSIDEG */
#endif /* is_epsilon && is_rcvr */

      working_index_set = sxba_bag_get (&shift_bag, n+1);

#if is_rcvr
      rcvr_store_unfiltered_non_kernel_item_set = sxba_calloc (MAXITEM+1);
      repair_mlstn_top = idag.final_state+1;
#endif /* is_rcvr */   

      reduce_set = sxba_calloc (MAXNT+1);
      Bpq_set = sxba_calloc (MAXNT+1);
      reduce_list = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), reduce_list [0] = 0;

#if HAS_BEAM == 1
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) {
      Bpq_list_set = sxba_calloc (MAXNT+1);
      DALLOC_STACK (Bpq_list, MAXNT+1); /* Ds ce cas le meme nt peut avoir plusieures occurrences
					   une (ou des) déjà traitée et une seule autre au plus à traiter
					   Les logprobs associées étant croissantes */
    }
    else
#endif /* HAS_BEAM == 1 */
      Bpq_list = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), Bpq_list [0] = 0;

#if MEASURES || LOG
      used_prod_set = sxba_calloc (MAXPROD+1);
#endif /* MEASURES || LOG */


#if HAS_BEAM
#if RECOGNIZER_KIND == 0
      /* Compile' sans beam durant lr reconnaisseur */
      recognizer_scan_beam_value_is_set = SXFALSE; /* prudence */
#endif /* RECOGNIZER_KIND == 0 */
#if REDUCER_KIND == 0
      /* Compile' sans beam durant lr reconnaisseur */
      reducer_beam_value_is_set = SXFALSE; /* prudence */
#endif /* REDUCER_KIND == 0 */

      if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) {
	XxY_alloc (&ei_hd, "ei_hd", MAXITEM*idag.final_state+1, 1, 0, 0, ei_hd_oflw, NULL);
	logprobs = (struct logprob_struct *) sxalloc (XxY_size (ei_hd)+1, sizeof (struct logprob_struct));

	XxY_alloc (&AxI_J_hd, "AxI_J_hd", MAXITEM*idag.final_state+1, 1, 0, 0, AxI_J_hd_oflw, NULL);
	AxI_J_logprobs = (struct AxI_J_logprob_struct *) sxalloc (XxY_size (AxI_J_hd)+1, sizeof (struct AxI_J_logprob_struct));

	fill_idag_pqt2logprob ();

#ifdef MAKE_INSIDEG
#if RECOGNIZER_KIND
	if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)
	  nt2max_outside_logprob = (SXLOGPROB*) sxalloc (MAXNT+1, sizeof (SXLOGPROB));
#endif /* RECOGNIZER_KIND */

#if is_epsilon
	nullable_nt2max_nullable_logprob = (SXLOGPROB*) sxalloc (MAXNT+1, sizeof (SXLOGPROB)), nullable_nt2max_nullable_logprob [0] = 0;
#endif /* is_epsilon */
#endif /* MAKE_INSIDEG */

#if is_epsilon
	if (EMPTY_SET) {
	  SXINT        X, Y, Y2, prod, reduce_item, item, item2;
	  SXLOGPROB    pr;
	  SXBA         nt2max_nullable_logprob_has_changed, prods_to_study;
	  SXBA         *nullable_nt2prods_whose_max_nullable_logprob_must_change_if_nt_max_nullable_logprob_has_changed;

	  nt2max_nullable_logprob_has_changed = sxba_calloc (MAXNT+1);
	  nullable_nt2prods_whose_max_nullable_logprob_must_change_if_nt_max_nullable_logprob_has_changed = sxbm_calloc (MAXNT+1, MAXPROD+1);
	    
	  X = 0;
	  while ((X = sxba_scan (EMPTY_SET, X)) > 0) {
	    nullable_nt2max_nullable_logprob [X] = SXLOGPROB_MIN;
	  }

	  item = 0;
	  while ((item = sxba_scan (EMPTY_PROD_ITEM_SET, item)) > 0) { /* pour chaque item qui peut dériver dans le vide */
	    prod = PROLIS (item);

	    X = LHS (prod);
	    Y = LISPRO (item); /* 0 si RHS vide, le premier symbole de RHS sinon */
	    if (Y == 0) { /* la prod de l'item item est à RHS vide */
	      /* initialisation: on connaît la logprob des prods à RHS vide, et on a donc pour le symbole de partie gauche
		 de ces prods une logprob meilleure que la valeur d'initialisation SXLOGPROB_MIN */
	      nullable_nt2max_nullable_logprob [X] = prod2logprob (prod);
	      SXBA_1_bit (nt2max_nullable_logprob_has_changed, X);
	    }
	    else {
	      item2 = item;
		
	      do {
		/* Y > 0, sinon ce ne serait pas une epsilon-prod */
		if (X == Y)
		  break; /* ainsi, sortir du do-while avec Y == 0 veut dire has_directly_recursive_rhs */
	      } while ((Y = LISPRO (++item2)) != 0);
		
	      if (Y == 0) { /* not has_directly_recursive_rhs */
		Y = LISPRO (item);
		do {
		  /* Y > 0, sinon ce ne serait pas une epsilon-prod */
		  SXBA_1_bit (nullable_nt2prods_whose_max_nullable_logprob_must_change_if_nt_max_nullable_logprob_has_changed [Y], prod);
		} while ((Y = LISPRO (++item)) != 0);
	      }
	    }	      
	  }

	  while ((Y = sxba_scan_reset (nt2max_nullable_logprob_has_changed, 0)) > 0) {
	    do {
	      prod = 0;
	      prods_to_study = nullable_nt2prods_whose_max_nullable_logprob_must_change_if_nt_max_nullable_logprob_has_changed [Y];
	      while ((prod = sxba_scan (prods_to_study, prod)) > 0) {
		X = LHS (prod);
		item = PROLON (prod);
		reduce_item = PROLON (prod+1)-1;
		pr = prod2logprob (prod);

		while (item < reduce_item) {
		  Y2 = LISPRO (item);
#if EBUG
		  if (Y2 == X)
		    sxtrap (ME, "error");
#endif /* EBUG */
		  pr += nullable_nt2max_nullable_logprob [Y2];		   
		  item++;
		}

		if (cmp_SXLOGPROB (nullable_nt2max_nullable_logprob [X], pr) < 0 /* < */) {
		  nullable_nt2max_nullable_logprob [X] = pr;
		  SXBA_1_bit (nt2max_nullable_logprob_has_changed, X);
		}
	      }
	    } while ((Y = sxba_scan_reset (nt2max_nullable_logprob_has_changed, Y)) > 0);
	  }
	  sxfree (nt2max_nullable_logprob_has_changed);
	  sxbm_free (nullable_nt2prods_whose_max_nullable_logprob_must_change_if_nt_max_nullable_logprob_has_changed);
	}

#endif /* is_epsilon */

#if RECOGNIZER_KIND == 2
	if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
	  SXINT  prod, item, X;
	  SXBA   empty_prefix_item_set;
	  SXLOGPROB logprob;

#if is_rcvr
	  nt2prod_that_generates_the_best_logprob_t_string = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT))
	    , nt2prod_that_generates_the_best_logprob_t_string [0] = 0;
#endif /* is_rcvr */

	  fill_tnt_max_inside_logprobs (SXTRUE /* On cree et on remplit item2max_right_inside_logprob */);

	  empty_prefix_item_set = sxba_calloc (MAXITEM+1);

#  if is_epsilon
	  item2nullable_max_prefix_logprob = (SXLOGPROB *) sxalloc (MAXITEM+1, sizeof (SXLOGPROB));
#  endif /* is_epsilon */

	  for (prod = 1; prod <= MAXPROD; prod++) {
	    item = PROLON (prod);

	    if (ilex_compatible_item_set == NULL || SXBA_bit_is_set (ilex_compatible_item_set, item)) {
#  if is_epsilon
	      item2nullable_max_prefix_logprob [item] = logprob = 0;
#  endif /* is_epsilon */
	      SXBA_1_bit (empty_prefix_item_set, item);

#  if is_epsilon
	      if (EMPTY_SET) {
		while ((X = LISPRO (item)) > 0 && SXBA_bit_is_set (EMPTY_SET, X)) {
		  item++;
		  //#  if is_epsilon
		  item2nullable_max_prefix_logprob [item] = logprob += nullable_nt2max_nullable_logprob [X];
		  //#  endif /* is_epsilon */
		  SXBA_1_bit (empty_prefix_item_set, item);
		}
	      }
#  endif /* is_epsilon */
	    }
	  }

	  {
	    /* pour chaque couple (A0, Ap) t.q.
	       r0   : A0   -> \alpha1 A1 \beta1
	       r1   : A1   -> \alpha2 A2 \beta2
	       ...
	       rp-1 : Ap-1 -> \alphap Ap \betap
	     
	       avec \alpha1 \alpha2 ... \alphap =>* \varepsilon (Ap est ds le coin gauche de A0)
	       on calcule MAX( P(\beta1 \beta2 ... \betap) + P(r0) + P(r1) + ... + P(rp-1)) la proba max que l'on range ds
	       nt2nt_max_suffix_logprob [A0] [Ap]
	       CETTE PROBA TIENT COMPTE DES PRODUCTIONS TRAVERSEES 
	    */

	    /* A VOIR : doit-on tenir compte des logprobs des productions traversees ? */

	    SXINT  A0, A1, A2, tbp_nb, bot, top;
	    SXLOGPROB *SXLOGPROB_ptr, *SXLOGPROB_ptr2, suffix_logprob, nullable_prefix_logprob, prefix_suffix_logprob;
	    SXBA   witness_A1, *witness;

#if LOG
  fputs ("\t*** Entering initialisation of nt2nt_max_suffix_logprob and nt2nt_nullable_max_prefix_logprob ***\n", stdout);
#endif /* LOG */

	    nt2nt_max_suffix_logprob = (SXLOGPROB**) sxalloc (MAXNT+1, sizeof (SXLOGPROB *));
	    nt2nt_max_suffix_logprob [0] = SXLOGPROB_ptr = (SXLOGPROB*) sxcalloc (MAXNT*MAXNT+1, sizeof (SXLOGPROB));

#if is_epsilon
	    nt2nt_nullable_max_prefix_logprob = (SXLOGPROB**) sxalloc (MAXNT+1, sizeof (SXLOGPROB *));
	    nt2nt_nullable_max_prefix_logprob [0] = SXLOGPROB_ptr2 = (SXLOGPROB*) sxcalloc (MAXNT*MAXNT+1, sizeof (SXLOGPROB));
#endif /* is_epsilon */
	  
	    for (A0 = 1; A0 <= MAXNT; A0++) {
	      nt2nt_max_suffix_logprob [A0] = SXLOGPROB_ptr;
	      for (A1 = 1; A1 <= MAXNT; A1++) {
		*(++SXLOGPROB_ptr) = 1; /* valeur d'initialisation, puisque ce n'est pas une logprob possible */
	      }
#if is_epsilon
	      nt2nt_nullable_max_prefix_logprob [A0] = SXLOGPROB_ptr2;
	      SXLOGPROB_ptr2 += MAXNT;
#endif /* is_epsilon */
	    }

	    witness = sxbm_calloc (MAXNT+1, MAXNT+1);

	    /* etape initiale */
	    /* Soit A0 -> \alpha1 A1 \beta1 avec \alpha1 =>* \varepsilon
	       on met P(\beta1) ds nt2nt_max_suffix_logprob [A0] [A1] */
	    tbp_nb = 0;
	    item = -1;

	    while ((item = sxba_scan (empty_prefix_item_set, item)) >= 0) {
	      if ((A1 = LISPRO (item)) > 0) {
		prod = PROLIS (item);
		A0 = LHS (prod);
#if is_epsilon
		nullable_prefix_logprob = item2nullable_max_prefix_logprob [item];
#else /* is_epsilon */
		nullable_prefix_logprob = 0;
#endif /* is_epsilon */

		suffix_logprob = item2max_right_inside_logprob [item+1] + prod2logprob (prod);

		prefix_suffix_logprob = 0;

#if is_epsilon
		SXLOGPROB_ptr2 = nt2nt_nullable_max_prefix_logprob [A0] + A1;
		prefix_suffix_logprob += *SXLOGPROB_ptr2;
#endif /* is_epsilon */

		SXLOGPROB_ptr = nt2nt_max_suffix_logprob [A0] + A1;
		prefix_suffix_logprob += *SXLOGPROB_ptr;

#if LLLOG
		output_item (item, NULL, -1, -1);
		printf ("<%s> LC <%s> (old_prefix + old_suffix = " SXLPD " + " SXLPD " = " SXLPD ", prefix + suffix = " SXLPD " + " SXLPD " = " SXLPD ")\n",
			NTSTRING (A1), NTSTRING (A0)
#if is_epsilon
			, SXLOGPROB_display (*SXLOGPROB_ptr2)
#else /* is_epsilon */
			, 0.
#endif /* is_epsilon */
			, SXLOGPROB_display (*SXLOGPROB_ptr)
			, SXLOGPROB_display (prefix_suffix_logprob)
			, SXLOGPROB_display (nullable_prefix_logprob)
			, SXLOGPROB_display (suffix_logprob)
			, SXLOGPROB_display (nullable_prefix_logprob + suffix_logprob)
			);
#endif /* LLLOG */
		if ((prefix_suffix_logprob == 1) /* c'est la première fois (1 est la logprob (impossible) d'initialisation) */
		    ||
		    (cmp_SXLOGPROB(prefix_suffix_logprob, nullable_prefix_logprob + suffix_logprob) < 0) /* c'est pas la première fois mais c'est mieux que précédemment, on met à jour */
		    ) {
#if is_epsilon
		  *SXLOGPROB_ptr2 = nullable_prefix_logprob;
#endif /* is_epsilon */
		  *SXLOGPROB_ptr = suffix_logprob;

		  if (SXBA_bit_is_reset_set (witness [A0], A1))
		    tbp_nb++;
		}
	      }
	    }


#if LOG
	    fputs ("\t*** Closure ***\n", stdout);
#endif /* LOG */

	    /* On ferme */
	    while (tbp_nb) {
	      for (A1 = 1; A1 <= MAXNT; A1++) {
		witness_A1 = witness [A1];
		A2 = 0;

		while ((A2 = sxba_scan_reset (witness_A1, A2)) > 0) {
		  tbp_nb--;
	    
		  for (top = NPD (A1+1)-1, bot = NPD (A1); bot < top; bot++) {
		    item = NUMPD (bot);

		    if (SXBA_bit_is_set (empty_prefix_item_set, item)) {
		      prod = PROLIS (item);
		      A0 = LHS (prod);
#if is_epsilon
		      nullable_prefix_logprob = item2nullable_max_prefix_logprob [item] + nt2nt_nullable_max_prefix_logprob [A1] [A2];
#else /* is_epsilon */
		      nullable_prefix_logprob = 0;
#endif /* is_epsilon */
		      suffix_logprob = item2max_right_inside_logprob [item+1] + nt2nt_max_suffix_logprob [A1] [A2] + prod2logprob (prod);

		      prefix_suffix_logprob = 0;

#if is_epsilon
		      SXLOGPROB_ptr2 = nt2nt_nullable_max_prefix_logprob [A0] + A2;
		      prefix_suffix_logprob += *SXLOGPROB_ptr2;
#endif /* is_epsilon */

		      SXLOGPROB_ptr = nt2nt_max_suffix_logprob [A0] + A2;
		      prefix_suffix_logprob += *SXLOGPROB_ptr;

#if LLLOG
		      printf ("<%s> LC+ <%s> LC <%s> (old_prefix + old_suffix = " SXLPD " + " SXLPD " = " SXLPD ", prefix + suffix = " SXLPD " + " SXLPD " = " SXLPD ")\n",
			      NTSTRING (A2), NTSTRING (A1), NTSTRING (A0)
#if is_epsilon
			      , SXLOGPROB_display (*SXLOGPROB_ptr2)
#else /* is_epsilon */
			      , SXLOGPROB_display (0)
#endif /* is_epsilon */
			      , SXLOGPROB_display (*SXLOGPROB_ptr)
			      , SXLOGPROB_display (prefix_suffix_logprob)
			      , SXLOGPROB_display (nullable_prefix_logprob)
			      , SXLOGPROB_display (suffix_logprob)
			      , SXLOGPROB_display (nullable_prefix_logprob + suffix_logprob)
			      );
#endif /* LLLOG */
		      if ((prefix_suffix_logprob == 1)
			  ||
			  (prefix_suffix_logprob < nullable_prefix_logprob + suffix_logprob)
			  ) {
#if is_epsilon
			*SXLOGPROB_ptr2 = nullable_prefix_logprob;
#endif /* is_epsilon */
			*SXLOGPROB_ptr = suffix_logprob;

			if (SXBA_bit_is_reset_set (witness [A0], A2))
			  tbp_nb++;
		      }
		    }
		  }
		}
	      }
	    }

	    sxbm_free (witness);

#if LOG
  fputs ("\t*** Leaving initialisation of nt2nt_max_suffix_logprob and nt2nt_nullable_max_prefix_logprob ***\n", stdout);
#endif /* LOG */
	  }

	  sxfree (empty_prefix_item_set);
	}
#endif /* RECOGNIZER_KIND == 2 */
      }

#if defined(CONTROLLED_BEAM_H) || defined(BEAM_MEASURE)
      fill_range2min_lgth ();
#endif /* CONTROLLED_BEAM_H || BEAM_MEASURE */

#endif /* HAS_BEAM */

#if HAS_BEAM == 2
      {
	reducer_beam_value_is_set = SXFALSE; /* pour n'executer que les bons trucs */
	recognizer_scan_beam_value_is_set = SXFALSE; /* pour n'executer que les bons trucs */

	ret_val = recognize ();

	recognizer_scan_beam_value_is_set = saved_recognizer_scan_beam_value_is_set;
	reducer_beam_value_is_set = saved_reducer_beam_value_is_set;
      }
#else /* HAS_BEAM == 2 */
      ret_val = recognize ();
#endif /* HAS_BEAM == 2 */


#if MEASURES || LOG
      if (ret_val)
	output_G ("recognizerG", NULL, used_prod_set); /* Ds lexicalizer_mngr !! */
#endif /* MEASURES || LOG */

      sxfree (reduce_set), reduce_set = NULL;
      sxfree (Bpq_set), Bpq_set = NULL;
      sxfree (reduce_list), reduce_list = NULL;

#if HAS_BEAM == 1
      if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) {
	sxfree (Bpq_list_set),Bpq_list_set = NULL; 
	DFREE_STACK (Bpq_list);
      }
      else
#endif /* HAS_BEAM == 1 */
	sxfree (Bpq_list), Bpq_list = NULL;

#if is_rcvr
      sxfree (rcvr_store_unfiltered_non_kernel_item_set), rcvr_store_unfiltered_non_kernel_item_set = NULL;

      if (rcvr_reduce_item_stack) {
	DFREE_STACK (rcvr_reduce_item_stack);
	sxfree (rcvr_non_kernel_item_set), rcvr_non_kernel_item_set = NULL;
	sxfree (rcvr_w2item_set), rcvr_w2item_set = NULL;
	sxfree (rcvr_prod_set), rcvr_prod_set = NULL;
	sxfree (rcvr_w2titem_set), rcvr_w2titem_set = NULL;
	sxfree (rcvr_reduce_T2_non_kernel_item_set), rcvr_reduce_T2_non_kernel_item_set = NULL;
      }
#endif /* is_rcvr */

#if is_epsilon && is_rcvr
      if (non_kernel_empty_item_set) sxfree (non_kernel_empty_item_set), non_kernel_empty_item_set = NULL;
#endif /* is_epsilon && is_rcvr */
    }

#if is_lex
    if (basic_item_set) sxfree (basic_item_set), basic_item_set = NULL;
    if (basic_nt_set) sxfree (basic_nt_set), ilex_compatible_item_set = lex_compatible_item_set = basic_nt_set = NULL;
#endif /* is_lex */

#if is_lex
    /* Attention, il se peut que mlstn2la_tset==mlstn2lex_la_tset (le 12/01/07) */
    if (mlstn2lex_la_tset && mlstn2lex_la_tset != mlstn2la_tset) {
      sxbm_free (mlstn2lex_la_tset), mlstn2lex_la_tset = NULL;
    }
#endif /* is_lex */
    if (mlstn2la_tset) sxbm_free (mlstn2la_tset), mlstn2la_tset = NULL;

    mlstn2lex_la_tset = NULL;
    mlstn2look_ahead_t_set = mlstn2lexicalized_look_ahead_t_set = NULL;
    mlstn2non_lexicalized_look_ahead_t_set = NULL;

#if has_Lex
    sxfree (prod_checked_set), prod_checked_set = NULL;
    sxfree (prod_val_set), prod_val_sGet = NULL;
#endif /* has_Lex */

#if RECOGNIZER_KIND == 2
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
#ifdef MAKE_INSIDEG
      if (nt2max_outside_logprob)
	sxfree (nt2max_outside_logprob), nt2max_outside_logprob = NULL;
#endif /* MAKE_INSIDEG */

#if is_epsilon
#ifdef MAKE_INSIDEG
      if (nullable_nt2max_nullable_logprob)
	sxfree (nullable_nt2max_nullable_logprob), nullable_nt2max_nullable_logprob = NULL;
#endif /* MAKE_INSIDEG */

      sxfree (item2nullable_max_prefix_logprob), item2nullable_max_prefix_logprob = NULL;
      sxfree (nt2nt_nullable_max_prefix_logprob [0]), sxfree (nt2nt_nullable_max_prefix_logprob), nt2nt_nullable_max_prefix_logprob = NULL;
# endif /* is_epsilon */

      if (nt2nt_max_suffix_logprob)
	sxfree (nt2nt_max_suffix_logprob [0]), sxfree (nt2nt_max_suffix_logprob), nt2nt_max_suffix_logprob = NULL;
    }
#endif /* RECOGNIZER_KIND == 2 */

#if HAS_BEAM == 1
    {
      SXINT i;
      
      sxfree (RT_global_logprobs_area), RT_global_logprobs_area = NULL;
      for (i = 0; i <= working_rcvr_mlstn+1; i++) {
	sxfree (RT_global_logprobs [i].global_logprobs);
      }
      sxfree (RT_global_logprobs), RT_global_logprobs = NULL;
    }
#endif /* HAS_BEAM == 1 */

#if is_parser
    if (ret_val) {
      if (sxerrmngr.nbmess [2] > 0)
	fprintf (sxstderr, "### WARNING: unexpected situation (errors are reported, although the recognizer worked successfuly). We go on\n");

      alloc_PT2 ();

      sxba_bag_alloc (&pt2_bag, "pt2_bag", (1+n + 1) * MAXPROD * SXNBLONGS (1+n + 1), statistics);
      ap_sets = sxbm_calloc (MAXNT+1, 1+n+1);
      red_order_sets = sxbm_calloc (1+n+1, MAXPROD+1);

#if is_rcvr
#if is_parser && !is_guide && !is_supertagging
      if (XxY_is_allocated (glbl_dag_rcvr_resume_kitems)) {
	sxprepare_for_possible_reset (glbl_dag_rcvr_start_kitems_set);
	glbl_dag_rcvr_start_kitems_set = sxba_calloc (XxYxZ_top (glbl_dag_rcvr_start_kitems)+1);
      }
#endif /* is_parser && !is_guide && !is_supertagging */
#endif /* is_rcvr */


#if HAS_BEAM == 2
      if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set || reducer_beam_value_is_set) {
	/* On ne lance cette evaluation que si l'une au moins des beam_value est non nulle */
#if LOG
	fputs ("\n**** REDUCER (top-down filter) ****\n", stdout);
#endif /* LOG */

	sxprepare_for_possible_reset (i2valid_item_set); /* semble inutile */
	i2valid_item_set = sxbm_calloc (n+1+1, MAXITEM+1);

#if is_rcvr
	if (is_error_detected) {
	  sxprepare_for_possible_reset (i2valid_triple_set);
	  i2valid_triple_set = sxbm_calloc (n+1+1, XxYxZ_top (glbl_dag_rcvr_start_kitems)+1);
	}
#endif /* is_rcvr */

	reducer_beam_value_is_set = SXFALSE; /* pour n'executer que les bons trucs de RL_mreduction */

	RL_mreduction (SXFALSE);

	/* On simule le reconnaisseur avec beam en utilisant i2valid_item_set */

	pass_number = 1;

	if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
	  fputs ("\n**** Beam filtering using expected logprobs in valid reduced tables is not implemented.  Exact inside left logprobs are computed instead ****\n", stdout);
	  recognizer_scan_beam_value_is_set = SXFALSE; /* prudence */
	}

	exact_inside_left_logprob_recognizer ();

	reducer_beam_value_is_set = saved_reducer_beam_value_is_set;
	sxbm_free (i2valid_item_set), i2valid_item_set = NULL;

#if is_rcvr
	if (i2valid_triple_set) sxbm_free (i2valid_triple_set), i2valid_triple_set = NULL;
#if is_parser && !is_guide && !is_supertagging
	if (is_error_detected && glbl_dag_rcvr_start_kitems_set) sxba_empty (glbl_dag_rcvr_start_kitems_set);
#endif /* is_parser && !is_guide && !is_supertagging */
#endif /* is_rcvr */

	PT2_save = PT2; /* Pour la seconde passe de RL_mreduction */

#ifdef MAKE_INSIDEG
	PT2_area_save = PT2_area;
#endif /* MAKE_INSIDEG */

	alloc_PT2 ();

	if (is_print_time) {
	  char mess [64];
	  struct logprob_struct *logprob_ptr;
	
	  ei2get_logprob_ptr (2, p_q2pq (0, idag.final_state), logprob_ptr);
	  sprintf (mess, "\tBest %s %sLogprob Computing = " SXLPD, recognizer_scan_beam_value_is_set ? "Estimated" : "Exact"
#ifdef CONTROLLED_BEAM_H
		   , must_perform_a_controlled_beam ? "Controlled " : ""
#else /* CONTROLLED_BEAM_H */
		   , ""
#endif /* CONTROLLED_BEAM_H */
		   , SXLOGPROB_display (logprob_ptr->inside.left));

	  sxtime (TIME_FINAL, mess);
	}
      }
#endif /* HAS_BEAM == 2 */

      /* ATTENTION CHANGEMENT :
	 Un symbole Aij ou i et j sont deux numeros de table signifie que :
	 Aij =>* ai ai+1 ... aj-1 */

      /* Modif du 08/02/06 */
      /* On cree des Aij, Tij meme si is_parse_forest n'est pas positionne' (on a donc des variables locales) */

      spf_allocate_Aij (MAXNT, SXEOF);

#if is_rcvr
      spf.outputG.Tpq_rcvr_set = sxba_calloc (XxYxZ_size (spf.outputG.Txixj2Tij_hd)+1);
#endif /* is_rcvr */

#if LOG
      fputs ("\n**** REDUCER (top-down filter) ****\n", stdout);
#endif /* LOG */

#ifdef DYNAMIC_BEAM_MEASURES
      if (reducer_beam_value_is_set) {
	if (recognizer_scan_beam_value_is_set)
	  DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob = (SXLOGPROB*) sxcalloc (idag.final_state+2, sizeof (SXLOGPROB));
	if (recognizer_shift_beam_value_is_set)
	  DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob = (SXLOGPROB*) sxcalloc (idag.final_state+2, sizeof (SXLOGPROB));
      }
#endif /* DYNAMIC_BEAM_MEASURES */

      RL_mreduction (SXTRUE);

#ifdef DYNAMIC_BEAM_MEASURES
      if (reducer_beam_value_is_set
	  && (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set)) {
	SXINT q;

	fputs ("\tq\tbeamrcvr_state\tdynamic_beam\tmax_expected_prob\tcur_worst_kept_prob\tpractical_beam\n", sxstderr);
	
	for (q = 1; q <= idag.final_state; q++) {
	  if (recognizer_scan_beam_value_is_set) {
	    if (DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [q] == 0)
	      fprintf (sxstderr, "SCAN\t" SXILD "\t-\t-\t" SXLPD "\t-\t-\n", q,
		       earley_table_companion [q].max_global_expected_logprob
		       );
	    else
	      fprintf (sxstderr, "SCAN\t" SXILD "\t" SXILD "\t" SXLPD "\t" SXLPD "\t" SXLPD "\t" SXLPD "\n", q,
		       earley_table_companion [q].beamrcvr_state, SXLOGPROB2proba (-earley_table_companion [q].dynamic_scan_beam_value),
		       earley_table_companion [q].max_global_expected_logprob,
		       DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [q],
		       cmp_SXLOGPROB (earley_table_companion [q].max_global_expected_logprob, DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [q]) == 0 ? 1 :
		       SXLOGPROB2proba (earley_table_companion [q].max_global_expected_logprob - DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [q])
		       );
	  }
	  if (recognizer_shift_beam_value_is_set) {
	    if (DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob [q] == 0)
	      fprintf (sxstderr, "SHIFT\t" SXILD "\t-\t-\t" SXLPD "\t-\t-\n", q,
		       earley_table_companion [q].max_global_expected_logprob
		       );
	    else
	      fprintf (sxstderr, "SHIFT\t" SXILD "\t" SXILD "\t" SXLPD "\t" SXLPD "\t" SXLPD "\t" SXLPD "\n", q,
		       earley_table_companion [q].beamrcvr_state, SXLOGPROB2proba (-earley_table_companion [q].dynamic_shift_beam_value),
		       earley_table_companion [q].max_global_expected_logprob,
		       DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob [q],
		       cmp_SXLOGPROB (earley_table_companion [q].max_global_expected_logprob, DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob [q]) == 0 ? 1 :
		       SXLOGPROB2proba (earley_table_companion [q].max_global_expected_logprob - DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob [q])
		       );
	  }
	}

	if (recognizer_scan_beam_value_is_set)
	  sxfree (DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob), DYNAMIC_BEAM_MEASURE_cur_worst_kept_scan_logprob = NULL;
	if (recognizer_shift_beam_value_is_set)
	  sxfree (DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob), DYNAMIC_BEAM_MEASURE_cur_worst_kept_shift_logprob = NULL;
      }
#endif /* DYNAMIC_BEAM_MEASURES */

#if HAS_BEAM == 2
      free_PT2_save ();
#endif /* HAS_BEAM == 2 */
    }
#endif /* is_parser */

#if is_make_oracle
    {
      /* campagne de mesures */
      SXINT oracle_nb, i, prod, item;
      SXBA set;

      oracle_nb = 0;

#if is_parser
      /* Cas de l'oracle */
      for (prod = 1; prod <= MAXPROD; prod++) {
	oracle_nb += sxba_cardinal (prod2lb_set [prod]);
      }

      sxbm_free (prod2lb_set);
#else /* !is_parser */
      /* cas de la mesure pour le guide (lexicaliseur) */

      for (i = 0; i <= n; i++) {
	set = i2guide_item_set [i];

	item = -1;

	while ((item = sxba_scan (set, item)) >= 0) {
	  prod = PROLIS (item);

	  if (item == PROLON (prod)) {
	    oracle_nb++;
	  }

	  item = PROLON (prod+1)-1;
	}
      }

      sxbm_free (i2guide_item_set);
#endif /* !is_parser */

      printf (SXILD " ", oracle_nb);
    }
#endif /* is_make_oracle */

#if RECOGNIZER_KIND == 2
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
      if (item2max_right_inside_logprob) {
	sxfree (item2max_right_inside_logprob), item2max_right_inside_logprob = NULL;
	XxY_free (&ei_hd);
	sxfree (logprobs), logprobs = NULL;
	XxY_free (&AxI_J_hd);
	sxfree (AxI_J_logprobs), AxI_J_logprobs = NULL;
      }
    }
#endif /* RECOGNIZER_KIND == 2 */

#if RECOGNIZER_KIND == 2 || (HAS_BEAM && is_rcvr)
    if (nt2max_inside_logprob) {
      sxfree (t2max_inside_logprob), t2max_inside_logprob = NULL;
      sxfree (nt2max_inside_logprob), nt2max_inside_logprob = NULL;
    }
#endif /* RECOGNIZER_KIND == 2 || (HAS_BEAM && is_rcvr) */

    sxba_bag_free (&shift_bag);
    sxbm_free (ntXindex_set);
#ifdef MAKE_INSIDEG
    sxfree (RT_area), RT_area = NULL; /* area */
#endif /* MAKE_INSIDEG */
    sxfree (RT), RT = NULL;
    sxbm_free (RT_left_corner);
#if is_rcvr
    sxfree (rcvr_xcld_min_mlstn_set), rcvr_xcld_min_mlstn_set = NULL;
    sxbm_free (rcvr_reduce_item_sets), rcvr_reduce_item_sets = NULL;
#endif /* is_rcvr */

#if is_rcvr
    sxbm_free (store_non_kernel_item_sets), store_non_kernel_item_sets = NULL;
#endif /* is_rcvr */

#if is_rcvr
    sxbm_free (rcvr_kernel_item_sets), rcvr_kernel_item_sets = NULL;
#endif /* is_rcvr */

    sxbm_free (non_kernel_item_sets), non_kernel_item_sets = NULL;

#if RECOGNIZER_KIND == 2
    if (recognizer_scan_beam_value_is_set || recognizer_shift_beam_value_is_set) {
      if (kernel_nt_sets) sxbm_free (kernel_nt_sets), kernel_nt_sets = NULL;
      if (earley_table_companion) sxfree (earley_table_companion), earley_table_companion = NULL;
    }
#endif /* RECOGNIZER_KIND == 2 */

#if is_supertagging
    sxfree (used_non_lex_prod_set), used_non_lex_prod_set = NULL;
#endif /* is_supertagging */

#if is_parser && !is_guide && !is_supertagging
    if (ret_val) {
      if (sxerrmngr.nbmess [2] > 0)
	fprintf (sxstderr, "### WARNING: unexpected situation (errors are reported, although the recognizer and the reducer worked successfuly). We go on\n");
      /* Le 08/02/06 positionnes meme si !is_parse_forest */
      spf_allocate_spf (MAXPROD);
      spf.outputG.maxxnt = spf.outputG.maxnt = XxYxZ_top (spf.outputG.Axixj2Aij_hd);

#if is_rcvr  
      if (is_error_detected) {
	glbl_dag_rcvr_messages_set = sxba_calloc (XxYxZ_top (glbl_dag_rcvr_start_kitems)+1);
	rcvr_message2repair_parse_id = (SXINT*) sxalloc (XxYxZ_top (glbl_dag_rcvr_start_kitems)+1, sizeof (SXINT));
      }
#endif /* is_rcvr */

#if LOG
      fputs ("\n**** PARSER ****\n", stdout);
#endif /* LOG */

#if MEASURES || LOG
      sxba_empty (used_prod_set);
#endif /* MEASURES || LOG */

#ifdef MAKE_INSIDEG
      rhs_stack = (SXINT*) sxalloc (2*MAXRHSLGTH+3, sizeof (SXINT)), rhs_stack [0] = 0;
#endif /* MAKE_INSIDEG */

      parse ();

#ifdef MAKE_INSIDEG
      sxfree (rhs_stack), rhs_stack = NULL;
#if is_full_guide
      sxfree (lb), lb = NULL;
      sxfree (ub), ub = NULL;
#endif /* is_full_guide */
#endif /* MAKE_INSIDEG */

      if (red_order_sets) {
	free_PT2 ();
	sxba_bag_free (&pt2_bag);
	sxbm_free (ap_sets), ap_sets = NULL;
	sxbm_free (red_order_sets), red_order_sets = NULL;
      }

#if MEASURES || LOG
      output_G ("goldG", NULL, used_prod_set); /* Ds lexicalizer_mngr !! */
#endif /* MEASURES || LOG */

#if LLOG
      {
	SXINT prod, old_prod, new_prod;

	new_prod = prod = 0;

	while ((prod = sxba_scan (used_prod_set, prod)) > 0) {
	  /* prod a ete selectionnee par la lexicalisation et a ete utilisee */
#ifdef MAKE_INSIDEG	  
	  old_prod = spf.insideG.prod2init_prod [prod];
#else /* MAKE_INSIDEG */
	  old_prod = prod;
#endif /* MAKE_INSIDEG */

	  printf (SXILD "/", ++new_prod);
	  output_prod (old_prod);
	}
      }
#endif /* LLOG */

#ifdef MAKE_INSIDEG
      /* Normalement, on n'en a + besoin !! BS: euh si, en cas -DLOG dans lfg_semact, par exemple*/
      spf_free_insideG (&spf.insideG);
#endif /* MAKE_INSIDEG */

      if (is_print_time) {
#if is_rcvr
	if (SXTRUE) {
	  if (is_error_detected)
	    sxtime (TIME_FINAL, "\tEarley Parser (FALSE)");
	  else
	    sxtime (TIME_FINAL, "\tEarley Parser (TRUE)");
	}
	else
#endif /* is_rcvr */
	  sxtime (TIME_FINAL, "\tEarley Parser (TRUE)");
      }

#if EBUG
#if is_rcvr
      if (is_error_detected && sxerrmngr.nbmess [2] == 0)
	/* Les erreurs ont disparues entre le reconnaisseur et le parseur !! */
	sxtrap (ME, "sxearley_parse_it");
#endif /* is_rcvr */
#endif /* EBUG */

      spf.outputG.is_error_detected = is_error_detected;

      /* Le 08/02/06 positionnes meme si !is_parse_forest */
      if (spf.outputG.maxprod == 0)
	/* L'axiome est un nt de la rcvr, on force maxnt a` 0!! */
	spf.outputG.maxnt = 0;

      spf.outputG.maxxnt = XxYxZ_top (spf.outputG.Axixj2Aij_hd);
      spf.outputG.maxt = -XxYxZ_top (spf.outputG.Txixj2Tij_hd); /* ca contient les xt eventuels !! */

      if (is_parse_forest) {
	/* On finalise la foret partagee */
	SXINT        Pij_repair, Aij, prod, i, j, top, Pij, init_prod, RHSij, bot2, top2, cur2, Xpq, size;
	struct rhs *prhs;
	struct lhs *plhs;

	/* Les nt en erreur sont au sommet de Axixj2Aij_hd */

	/* Le 06/02/06 on tient compte des "repair" eventuels */
	spf_reallocate_lhs_rhs (spf.outputG.maxprod+Pij_repair_nb+spf.outputG.maxxnt,
				spf.outputG.maxitem+Pij_repair_nb*(MAXRHSLGTH+1)+spf.outputG.maxprod+Pij_repair_nb+spf.outputG.maxxnt);
      
	spf.outputG.maxxprod = spf.outputG.maxprod;
	spf.outputG.Tpq_repair_nb = Tpq_repair_nb;
#if is_rcvr
	spf.outputG.repair_mlstn_top = repair_mlstn_top;
#endif /* is_rcvr */

	if (spf.outputG.maxxnt > spf.outputG.maxnt) {
	  spf.outputG.non_productive_Aij_rcvr_set = sxba_calloc (spf.outputG.maxxnt+1);

	  for (Aij = spf.outputG.maxnt+1; Aij <= spf.outputG.maxxnt; Aij++) {
	    i = Aij2i (Aij);
	    j = Aij2j (Aij);

	    if (i <= 0 || j <= 0)
	      SXBA_1_bit (spf.outputG.non_productive_Aij_rcvr_set, Aij);
	  }
	}
	
	Pij_repair = 0;

#if is_rcvr
	if (Pij_repair_nb) {
	  SXINT save_maxprod, *iprod_stack_ptr;

	  if (is_print_prod)
	    fputs ("********************************  X P R O D S  ********************************\n", stdout);

	  /* On met les prod instanciees de la reparation */
	  top = XxY_top (repair_Pij_hd);

	  spf.outputG.xPij2lgth = (SXINT *) sxalloc (Pij_repair_nb+1, sizeof (SXINT)), spf.outputG.xPij2lgth [0] = 0;

	  /* pour le restituer en fin */
	  save_maxprod = spf.outputG.maxprod;

	  for (Pij = 1; Pij <= top; Pij++) {
	    top2 = XH_X (repair_iprod_stack_hd, Pij+1)-1;
	    init_prod = XH_list_elem (repair_iprod_stack_hd, top2);

	    if (init_prod > 0) {
	      /* pas bidon */
	      bot2 = XH_X (repair_iprod_stack_hd, Pij);
	      Pij_repair++;
	      spf.outputG.xPij2lgth [Pij_repair] = repair_Pij2gen_lgth [Pij];
	      iprod_stack_ptr = &(XH_list_elem (repair_iprod_stack_hd, bot2));

	      spf_put_an_iprod (iprod_stack_ptr);

	      if (is_print_prod)
		spf_print_iprod (stdout, spf.outputG.maxprod, "", "\n");
	    }
	  }

	  spf.outputG.maxxprod = spf.outputG.maxprod;
	  spf.outputG.maxprod = save_maxprod;
	}
#endif /* is_rcvr */

	/* Remplissage des "hooks" qui permettent pour un nt donne' A
	   de trouver toutes les A-prods qui le definissent */
	spf_finalize ();

	/* Verrue de (moins en moins) provisoire */
#if is_rcvr
	/* Il n'est pas impossible que, ds le cas is_error_detected (independammant de is_potential_orphan_rule)
	   la spf soit devenue cyclique :
	   <A> = <B> \alpha ;
	   <B> = <A> ;
	   et la rcvr a vire' \alpha (\alpha[j..j] est OK).
	   Ds ce cas, il faudrait lancer un spf_topological_walk (spf.outputG.start_symbol, NULL, dummy_bu_walk);
	   ou dummy_bu_walk se contente de retourner 1 (C'est le nouveau parcourt bottom-up lui-meme qui vire les cycles) */
	/* pourrait aussi s'utiliser si spf.inputG.has_cycles */
	if (is_potential_orphan_rule
	    /* Dans certains cas, on a pu generer des prods inaccessibles (voir parse_item), on les vire manu avant de resoudre le pb !! */
	    /* Le nouveau spf_make_proper permet un appel direct */ 
	    || is_rcvr_can_be_cyclic
	    /*
	      Ds certains cas tordus (is_rcvr_can_be_cyclic), la foret peut etre un graphe cyclique, on vire les cycles
	      Ca serait bien que la presence de cycles eventuels soit mieux detectee que par is_rcvr_can_be_cyclic
	      et il faudrait que les messages soient sortis apres l'elimination des cycles
	    */
	    || spf.inputG.has_cycles /* Le 13/01/06 */
	    ) {
	  if (!spf_make_proper (spf.outputG.start_symbol)) // toute la forêt a disparu
	    sxtrap (ME, "sxparse_it (empty forest)");
	}
	else
#endif /* is_rcvr */  
	  /* Attention, si la grammaire est huge spf.inputG.has_cycles n'a pas ete positionne' et il n'a pas ete calcule'
	     ds insideG (faut-il le faire ?) sachant que le 1er appel a` la foret virera les cycles ? */
	  if (spf.inputG.has_cycles)
	    /* Le 13/01/06 */
	    /* je vire les cycles eventuels de la foret */
	    if (!spf_make_proper (spf.outputG.start_symbol)) // toute la forêt a disparu
	      sxtrap (ME, "sxparse_it (empty forest)");


	if (is_tagged_dag)
	  /* On imprime le source selectionne par la foret sous forme de udag */
	  spf_yield2dfa (SXTRUE /* udag */);
#if 0
	/* trop long pour être maintenu en toute situation dès que -DLOG */
#if LOG
	else {
	  /* A voir, la version precedente etait beaucoup trop longue */
	  puts ("\n\n ******* Tagged input sentence in minimal UDAG form:");
	  spf_yield2dfa (SXTRUE /* udag */);
	}
#endif /* LOG */
#endif /* 0 */
      }

#if MEASURES || LOG
      sxfree (used_prod_set), used_prod_set = NULL;
#endif /* MEASURES || LOG */

#if is_rcvr
      if (XH_is_allocated (repair_iprod_stack_hd)) {
	XxY_free (&repair_Pij_hd);
	sxfree (repair_Pij2gen_lgth), repair_Pij2gen_lgth = NULL;
	XH_free (&repair_iprod_stack_hd);
      }
#endif /* is_rcvr */

#ifdef MAKE_INSIDEG
      idag_t_stack_restore ();
#endif /* MAKE_INSIDEG */

      if (for_semact.sem_init != NULL)
	(*for_semact.sem_init) ();

      if (for_semact.sem_pass != NULL)
	(*for_semact.sem_pass) ();
    
      if (for_semact.sem_final != NULL)
	(*for_semact.sem_final) ();

      spf_free ();

#if is_rcvr
      if (repair_Tpq2tok_no) {
	X_free (&repair_Tpq_hd);
	sxfree (repair_Tpq2tok_no), repair_Tpq2tok_no = NULL;
      }

      if (XxY_is_allocated (rcvr_out_range_memo))
	XxY_free (&rcvr_out_range_memo);
#endif /* is_rcvr */
    }
    else {
      /* on n'a pas pu lancer l'analyse, faute de grammaire réduite non-vide (cas HUGE) et/ou faute de
	 recovery (cas !is_rcvr) */
      /* La forêt est donc vide */
      if (for_semact.sem_init != NULL)
	(*for_semact.sem_init) ();

      if (for_semact.sem_pass != NULL)
	(*for_semact.sem_pass) ();
    
      if (for_semact.sem_final != NULL)
	(*for_semact.sem_final) ();

    }
#endif /* is_parser && !is_guide && !is_supertagging */

#if HAS_BEAM && is_rcvr
    if (nt2prod_that_generates_the_best_logprob_t_string)
      sxfree (nt2prod_that_generates_the_best_logprob_t_string), nt2prod_that_generates_the_best_logprob_t_string = NULL;
#endif /* HAS_BEAM && is_rcvr */

#ifdef MAKE_INSIDEG
    spf_free_insideG (&spf.insideG);
#endif /* MAKE_INSIDEG */

#if has_Lex
    sxfree (clause_val_set), clause_val_set = NULL;
#endif /* has_Lex */

#if is_rcvr
    if (varstring) varstr_free (varstring), varstring = NULL;
#endif /* is_rcvr */

#if is_parser
    /* Au cas ou... */
#if is_rcvr
    if (XxY_is_allocated (glbl_dag_rcvr_resume_kitems)) {
      XxY_free (&glbl_dag_rcvr_resume_kitems);
      sxfree (glbl_dag_rcvr_resume_k_set), glbl_dag_rcvr_resume_k_set = NULL;
      sxfree (glbl_dag_rcvr_resume_item_set), glbl_dag_rcvr_resume_item_set = NULL;
      XxYxZ_free (&glbl_dag_rcvr_start_kitems);

      if (glbl_dag_rcvr_start_kitems_set) sxfree (glbl_dag_rcvr_start_kitems_set), glbl_dag_rcvr_start_kitems_set = NULL;
      if (XxYxZ_is_allocated (store_rcvr_string_hd)) XxYxZ_free (&store_rcvr_string_hd);
    }
#endif /* is_rcvr */

    if (red_order_sets) {
      free_PT2 ();
      sxba_bag_free (&pt2_bag);
      sxbm_free (ap_sets), ap_sets = NULL;
      sxbm_free (red_order_sets), red_order_sets = NULL;
    }
#endif /* is_parser */

#if is_parser
    if (PT) {
#ifdef MAKE_INSIDEG
      sxfree (PT_area), PT_area = NULL; /* area */
#endif /* MAKE_INSIDEG */
      sxfree (PT), PT = NULL;
    }
#endif /* is_parser */

#if !is_guide && !is_full_guide && !is_supertagging
    if (is_print_time)
      sxtime (TIME_FINAL, "\tDone");
#endif /* !is_guide && !is_full_guide && !is_supertagging */
    break;

  case SXCLOSE:
#ifdef SEMANTICS
    if (for_semact.sem_close) {
      (*for_semact.sem_close) ();
    }
#endif /* SEMANTICS */
    break;

  default:
    break;
  }

  return ret_val;
}
#endif /* !(is_supertagging && !is_recognizer) */

#if is_parser
/* Le 12/08/2002 */
/* L'"ordre" de sortie des prod instanciees n'est pas assure' */
/* Va environ 2 fois + vite que le + ancien */
static void
parse ()
{    
  SXINT     j, item_j, prod, nbt, l, *Aij_stack, repair_parse_id;
  SXBA    red_order_set;
  
  pass_number = 3; /* parseur */

  for (j = 0; j <= n+1; j++) {
    red_order_set = red_order_sets [j];
    /* red_order_set est positionne par le traitement des PT2. */
    /* il contient les A -> \alpha t. */
    prod = MAXPROD+1;

    while ((prod = sxba_1_rlscan (red_order_set, prod)) > 0) {
      /* Examine' du plus grand vers le + petit. */
#if 0
#ifdef MAKE_INSIDEG
      /* red_order_set contient directement une prod car order2prod n'est pas calcule' dynamiquement par lexicalizer_mngr */
      prod = prod_order;
#else /* !MAKE_INSIDEG */
      prod = order2prod [prod_order];
#endif /* !MAKE_INSIDEG */
#endif /* 0 */

      rhs_stack [0] = prod;
      item_j = PROLON (prod+1)-1;
      l = item_j-PROLON (prod);

      nbt = 0;

      Aij_stack = rhs_stack+2*(1+l);

      if (l == nbt)
	parse_item (item_j, Aij_stack, NULL, j-nbt, j, j);
      else
	parse_item (item_j, Aij_stack, PT2 [j-nbt].index_sets [item_j-nbt], 0, j, j);
    }
  }

#if is_rcvr  
  if (is_error_detected) {
    SXINT top;

    if (top = TOP (rcvr_on_super_rule_stack)) {
      /* De la recup implique la super regle */
      do {
	rcvr_message2repair_parse_id [rcvr_on_super_rule_stack [top-4]] =
	  earley_glbl_rcvr_message (rcvr_on_super_rule_stack [top-3],
				    rcvr_on_super_rule_stack [top-2],
				    rcvr_on_super_rule_stack [top-1],
				    rcvr_on_super_rule_stack [top],
				    2 /* error */,
				    is_parse_forest);
				  
	top -= 5;
      } while (top);
    }

    sxfree (glbl_dag_rcvr_messages_set), glbl_dag_rcvr_messages_set = NULL;
    sxfree (rcvr_message2repair_parse_id), rcvr_message2repair_parse_id = NULL;

    if (XxY_is_allocated (XxY_gen_repair_hd)) {
      XxY_free (&XxY_gen_repair_hd);

      if (rcvr_spec.repair_kind != 1) {
	sxfree (gen_repair2attr), gen_repair2attr = NULL;
	sxfree (repair_gen_nyp), repair_gen_nyp = NULL;
	sxfree (repair_path_stack), repair_path_stack = NULL;
	sxba_bag_free (&repair_bag);
      }

      DFREE_STACK (gen_valid_repair_string_stack);
      DFREE_STACK (gen_tnt_repair_stack);

      if (rcvr_spec.perform_repair_parse) {
	sxfree (gen_repair2attr2), gen_repair2attr2 = NULL;
	DFREE_STACK (local_repair_prod_stack);
	DFREE_STACK (glbl_repair_prod_stacks);
	XxY_free (&repair_forms_hd);
	DFREE_STACK (repair_source_stack);
	sxfree (repair_terminal_index2i), repair_terminal_index2i = NULL;
	sxfree (repair_terminal_index2j), repair_terminal_index2j = NULL;
	sxfree (repair_Tpq), repair_Tpq = NULL;
	DFREE_STACK (repair_rhs_stack);

	XxY_free (&gen_tntXmaxlgth_repair_hd);
	sxfree (gen_tntXmaxlgth_repair_set), gen_tntXmaxlgth_repair_set = NULL;
	DFREE_STACK (gen_proto_stack);
      }
    }
  }
#endif /* is_rcvr */
}


/* CAS is_parser */
/* Version ds laquelle chaque symbole Xpq contribue a 2 cellules ds Aij_stack pour p et q
   Ca permet des symboles instancies non consecutifs en cas de rcvr qui a supprime' du materiel
   source sur place (sans changer d'item) */
static void
parse_item (SXINT item_k, SXINT *Aij_stack, SXBA i_set, SXINT i, SXINT k, SXINT j)
{
  SXINT        prod, item_i, item_h, X, h, prdct_no, li, repair_parse_id;
  SXBA         backward_index_set, index_set;
#if is_rcvr
  SXINT        resume, triple, kept_triple, cur_item, A, symb_nb, *local_Aij_stack;
  SXINT        couple, RHSij, bot, top, cur, Xpq;
  SXBOOLEAN    has_rcvr, is_OK;
#endif /* is_rcvr */

  prod = PROLIS (item_k);
  item_i = PROLON (prod);
      
  if (i_set != NULL)
    i = sxba_scan (i_set, -1); /* le + a gauche */

#if is_rcvr
  resume = get_rcvr_resume (k, item_k);
#endif /* is_rcvr */

  if (item_i == item_k) {
    /* reduce */
    /* item_i = Aij -> . \alpha */
#if is_rcvr
    if (resume) {
      while (i >= 0) {
	triple = XxYxZ_is_set (&glbl_dag_rcvr_start_kitems, i, item_k, resume);

	is_OK = SXFALSE;

	if (triple && SXBA_bit_is_set (glbl_dag_rcvr_start_kitems_set, triple)) {
	  if (SXBA_bit_is_reset_set (glbl_dag_rcvr_messages_set, triple))
	    /* Un seul message par erreur (la meme recup sur \betaij peut se trouver impliquee ds plusieurs reduc
	       Ahk -> ... \betaij ... */
	    rcvr_message2repair_parse_id [triple] =
	      repair_parse_id =
	      earley_glbl_rcvr_message (item_i, i, item_i, k, 2 /* error */, is_parse_forest);
	  else
	    repair_parse_id = rcvr_message2repair_parse_id [triple];

	  /* Y-a-t'il qqchose a faire ds ce cas avec repair_parse_id ? */
	
	  *Aij_stack-- = j;
	  *Aij_stack-- = i;
	  /* Le 05/01/06 Les bornes du nt en lhs sont positives */
	
	  is_OK = SXTRUE;
	}
	else {
	  if (i == k) {
	    *Aij_stack-- = j; /* ajout */
	    *Aij_stack-- = k;
	  
	    is_OK = SXTRUE;
	  }
	}

	if (is_OK) {
	  put_in_shared_parse_forest (rhs_stack);

	  if (is_semact_fun)
	    (*for_semact.semact) (spf.outputG.maxprod);

	  Aij_stack += 2;
	}

	if (i_set == NULL)
	  break;

	i = sxba_scan (i_set, i);
      }
    }
    else
#endif /* is_rcvr */ 
      if ((i_set == NULL && i == k) ||
	  (i_set != NULL && SXBA_bit_is_set (i_set, k))) {
	*Aij_stack-- = j; /* ajout */
	*Aij_stack-- = k;

	put_in_shared_parse_forest (rhs_stack);

	if (is_semact_fun)
	  (*for_semact.semact) (spf.outputG.maxprod);
      }

    return;
  }

  /* non-terminal (cas !is_dag)... */
  /* ... ou terminal non-terminal (cas is_dag) */
  /* item_k = A -> \alpha X . \beta */
    
  if ((index_set = PT2 [k].index_sets [item_k]) &&
      (((i_set == NULL) && SXBA_bit_is_set (index_set, i)) || ((i_set != NULL) && IS_AND (index_set, i_set)))) {

#if is_rcvr
    if (resume) {
      triple = 0;

      XxYxZ_Zforeach (glbl_dag_rcvr_start_kitems, resume, triple) {
	if (SXBA_bit_is_set (glbl_dag_rcvr_start_kitems_set, triple)) {
	  break;
	}
      }

      if (triple) {
	h = XxYxZ_X (glbl_dag_rcvr_start_kitems, triple);

	if (h >= i) {
	  item_h = XxYxZ_Y (glbl_dag_rcvr_start_kitems, triple);

#if is_prdct
	  if ((!is_prdct_fun || (prdct_no = prdct [item_h]) == -1) ||
	      (*for_semact.prdct) (2 /* No de passe : analyseur */, item_h, h, k, prdct_no))
#endif  /* is_prdct */
	    {
	      /* Ici erreur confirmee */
	      if (SXBA_bit_is_reset_set (glbl_dag_rcvr_messages_set, triple))
		/* Un seul message par erreur (la meme recup sur \betaij peut se trouver impliquee ds plusieurs reduc
		   Ahk -> ... \betaij ... */
		rcvr_message2repair_parse_id [triple] =
		  repair_parse_id =
		  earley_glbl_rcvr_message (item_h, h, item_k, k, 2 /* error */, is_parse_forest);
	      else
		repair_parse_id = rcvr_message2repair_parse_id [triple];
	
	      symb_nb = item_k-item_h;
	      local_Aij_stack = Aij_stack;

	      if (symb_nb == 0 || repair_parse_id == 0) {
		if (symb_nb == 0) {
		  /* On a permis du rattrapage de la forme
		     A -> \alpha [h] [k] \beta */
		  /* comment instancier la prod A -> \alpha [h] [k] \beta pour qu'elle encode c,a ?? 
		     On ne met rien ds Aij_stack */
		}
		else {
		  *local_Aij_stack-- = -k; /* depart d'un rattrapage */

		  while (--symb_nb > 0) {
		    *local_Aij_stack-- = 0; /* valeur inconnue */
		    *local_Aij_stack-- = 0; /* valeur inconnue */
		  }

		  *local_Aij_stack-- = -h; /* fin d'un rattrapage */
		}
	    
		/* Ne pas toucher a Aij_stack */
		parse_item (item_h, local_Aij_stack, i_set, i, h, j);
	      }
	      else {
		/* "repair" a ete active' */
		XxY_Xforeach (repair_Pij_hd, repair_parse_id, couple) {
		  RHSij = XxY_Y (repair_Pij_hd, couple);

		  local_Aij_stack = Aij_stack;

		  bot = XH_X (repair_iprod_stack_hd, RHSij);
		  top = XH_X (repair_iprod_stack_hd, RHSij+1)-1;

		  for (cur = bot+2; cur < top; cur++) {
		    Xpq = XH_list_elem (repair_iprod_stack_hd, cur);
		    *local_Aij_stack-- = Xpq; /* special */
		    *local_Aij_stack-- = -(idag.final_state + 1); /* valeur speciale qui annonce la suivante */
		  }
		  
		  parse_item (item_h, local_Aij_stack, i_set, i, h, j);
		}
	      }
	    }
	}
	else
	  resume = 0;
      }
    }
#endif /* is_rcvr */

    backward_index_set = PT2 [k].backward_index_sets [item_k];

#if EBUG
    if (backward_index_set == NULL
#if is_rcvr
	&& resume == 0
#endif /* is_rcvr */
	)
      sxtrap (ME, "parse_item");
#endif

    if (backward_index_set) {
      item_h = item_k-1;
      h = i-1;

      while ((h = sxba_scan (backward_index_set, h)) > 0) {
#if is_prdct
	if ((!is_prdct_fun || (prdct_no = prdct [item_h]) == -1) ||
	    (*for_semact.prdct) (2 /* No de passe : analyseur */, item_h, h, k, prdct_no))
#endif  /* is_prdct */
	  {
	    Aij_stack [0] = k;
	    Aij_stack [-1] = h;
	    parse_item (item_h, Aij_stack-2, i_set, i, h, j);
	  }
      }
    }
  }
}
#endif /* is_parser */



static void
sxearley_raz (void)
{
#if HAS_BEAM == 2
  recognizer_scan_beam_value_is_set = saved_recognizer_scan_beam_value_is_set;
  reducer_beam_value_is_set = saved_reducer_beam_value_is_set;
#endif /* HAS_BEAM == 2 */

  is_error_detected = rcvr_detection_table_cleared = SXFALSE;
  Tpq_repair_nb = Pij_repair_nb = working_rcvr_mlstn = rcvr_detection_table = 0;
#if rcvr
  repair_mlstn_top = 0;
  perform_copy_local_repair_prod_stack = SXFALSE;
#ifndef MAKE_INSIDEG
  sxba_empty (rcvr_non_kernel_item_set);
#endif /* MAKE_INSIDEG */
#endif /* rcvr */
#if has_Lex
  split_val = 0;
#ifdef lrprod_list_include_file
  lrprod_filtered_nb = 0;
#endif /* lrprod_list_include_file */
  Lex_prod_item_set = Lex2prod_item_set_0;
#endif /* has_Lex */
  is_mlstn2non_lexicalized_look_ahead_t_set = is_mlstn2lexicalized_look_ahead_t_set = is_lex_compatible_item_sets = SXFALSE;
#if !(is_supertagging && !is_recognizer)
  is_semact_fun = is_constraint_fun = is_prdct_fun = is_output_full_guide = SXFALSE;
#if rcvr
  no_la_check = SXFALSE;
#ifndef MAKE_INSIDEG
  sxba_empty (rcvr_w2titem_set);
  sxba_empty (rcvr_store_unfiltered_non_kernel_item_set);
#endif /* MAKE_INSIDEG */
#endif /* rcvr */
#endif /* !(is_supertagging && !is_recognizer)*/
#if is_rcvr
  is_during_error_rcvr_processing = is_during_reduce_rcvr_validation = SXFALSE;
  rcvr_on_super_rule_stack [0] = 0;
  is_potential_orphan_rule = is_rcvr_can_be_cyclic = is_rcvr_on_site = SXFALSE;
  if (varstring)
    varstr_raz (varstring);
  if (sub_dag_vstr)
    varstr_raz (sub_dag_vstr);
#endif /* is_rcvr */

#ifndef MAKE_INSIDEG
  rhs_stack [0] = 0;
#if is_full_guide
  memset ((char*)lb , 0, (size_t) (sizeof (SXINT) * (inputG_MAXRHSLGTH + 1)));
  memset ((char*)ub , 0, (size_t) (sizeof (SXINT) * (inputG_MAXRHSLGTH + 1)));
#endif /* is_full_guide */
#if is_rcvr
  sxba_empty (rcvr_w2item_set);
  rcvr_reduce_item_stack [0] = 0;
#endif /* is_rcvr */
#endif /* !MAKE_INSIDEG */
  xprod = 0;
#if is_rcvr
  best_repair = best_distance = sub_dag_delta_init = 0;
  gen_empty_repair_string = SXFALSE;
  delta = 0;
  cur_error_string_lbound = cur_error_string_rbound /* lmlstn = rmlstn */ = cur_repair_string_rbound /* lgth+1 de la chaine corrigee */ = 0;
  repair_result_stack [0] = 0;
  working_rcvr_mlstn_tables_must_be_cleared = SXFALSE;
  use_a_single_rcvr_string = SXFALSE;
#endif /* is_rcvr */
  iprod_stack [0] = 0;
}


extern int sxearley_main (int argc, char *argv[]);


int
main (int argc, char *argv[])
{
  int ret_val;

#ifdef SXLOGPROB_IS_SXFLOAT
#if HAS_BEAM
  extern SXINT get_prod2logprobs_type (void);
  SXFLOAT_TYPE_CHECK (get_prod2logprobs_type);
#endif /* HAS_BEAM */
#endif /* SXLOGPROB_IS_SXFLOAT */

#if SXTTY_IS_SXSTDERR
  sxtty_is_stderr = SXTRUE;
#endif /* SXTTY_IS_STDERR */

#if HAS_BEAM
#ifdef BEAM_MEASURE
  XxY_alloc (&beam_measure_valid_table_hd, "beam_measure_valid_table_hd", 20000 /* !! tres dur a prevoir */+1, 1, 0, 0, NULL, NULL);
  XxY_alloc (&controlled_beam_global_table, "controlled_beam_global_table", 10000 /* !! tres dur a prevoir */+1, 1, 0, 0, controlled_beam_global_table_oflw, NULL);
  controlled_beam_global_table2logprob = (SXLOGPROB *) sxalloc (XxY_size (controlled_beam_global_table)+1, sizeof (SXLOGPROB));
#endif /* BEAM_MEASURE */

#ifdef CONTROLLED_BEAM_H_global_table
  SXFLOAT_TYPE_CHECK (get_controlled_beam_lgth2logprob_type);
  XxY_reuse (&controlled_beam_global_table_hd, "controlled_beam_global_table_hd", NULL, NULL);
#endif /* CONTROLLED_BEAM_H_global_table */

#endif /* HAS_BEAM */

  ret_val = sxearley_main (argc, argv);

#if HAS_BEAM
#ifdef BEAM_MEASURE
  controlled_beam_output ();

  XxY_free (&beam_measure_valid_table_hd);
  XxY_free (&controlled_beam_global_table);
  sxfree (controlled_beam_global_table2logprob), controlled_beam_global_table2logprob = NULL;
#endif /* BEAM_MEASURE */
#endif /* HAS_BEAM */

  return ret_val;
}

SXBOOLEAN        (*main_parser)(SXINT what) = sxearley_parse_it;
