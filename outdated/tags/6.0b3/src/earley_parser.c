/* ********************************************************
 *                                                      *
 *                                                      *
 * Copyright (c) 1996 by Institut National de Recherche *
 *                    en Informatique et en Automatique *
 *                                                      *
 *                                                      *
 ******************************************************** */




/* ********************************************************
 *                                                        *
 *     Produit de l'equipe ATOLL.			  *
 *                                                        *
 ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Lun 19 sep 2002 	        Suppression des Xpack                   */
/*                              Pour p = A -> X1 ... Xp                 */
/*                              Pour une instanciation Aij, la stack    */
/*                               contient                               */
/*                              [p|i|i1|...|ip] avec ip=j, (p>=0)       */
/*                              et ik=borne sup du range couvert par Xk */
/************************************************************************/
/* Lun 5 mai 2002 	        Adaptation a is_sdag et is_lex          */
/************************************************************************/
/* Jeu 23 Jan 1997 15:07	Bug ds RL_mreduction () ds le traitement*/
/*				des is_right_recursive			*/
/************************************************************************/
/* Ven 10 Jan 1997 15:22	Bug ds RL_mreduction () ds le traitement*/
/*				des is_right_recursive			*/
/************************************************************************/
/* Jul 23 1996 11:08:		Ajout de cette rubrique "modifications"	*/
/************************************************************************/




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

#include "sxunix.h"
#include "sxba_bag.h"

char WHAT_EARLEY_PARSER[] = "@(#)SYNTAX - $Id: earley_parser.c 1164 2008-03-07 15:31:55Z sagot $" WHAT_DEBUG;

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
#define def_prod2order
#define def_order2prod
#define def_prod_order2max
#define def_prod_order2min
#define def_empty_prod_item_set
#if !is_dag
#define def_item2nbt
#endif /* !is_dag */
#endif /* is_parser */

#include LC_TABLES_H
#endif /* LC_TABLES_H */

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
#define SX_DFN_EXT_VAR2
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
#define TPD(i)          tpd [i]
#define TNUMPD(i)       tnumpd [i]
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

static SXINT          Tpq_repair_nb, Pij_repair_nb;
static SXINT          *repair_Tpq2tok_no;

static SXINT          working_rcvr_mlstn, rcvr_detection_table;
static BOOLEAN      rcvr_detection_table_cleared;

static SXBA         ilex_compatible_item_set;

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
					     TRUE /* REPAIR */, TRUE /* PARSE */};
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
static BOOLEAN          perform_copy_local_repair_prod_stack /* pour savoir si on recopie ou non le résultat du
							      rcvr local dans la pile globale de rcvr */ ;

/*
  Le filtrage (lexicalisation) a pu enlever des items sur lesquels de la rcvr est possible
  Ce sont des items des nt2item_set [A], A \in rcvr_shift_NT_hd_set_mlstn et A>0
  On les met ds rcvr_non_kernel_item_set
  Ds earley_dag_glbl_rcvr (), on essaie d'abord de se recuperer sur rcvr_kernel_item_set, si ca echoue
  on essaie ensuite de se recuperer sur rcvr_non_kernel_item_set-rcvr_kernel_item_set
*/
#ifdef MAKE_INSIDEG
static SXBA rcvr_non_kernel_item_set;
#else /* !MAKE_INSIDEG */
static SXBA_CST_ALLOC (rcvr_non_kernel_item_set, itemmax+1);
#endif /* !MAKE_INSIDEG */

static SXBA global_rcvr_valid_left_mlstn_set;

#endif /* is_rcvr */


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
void
sxearley_init_for_semact ()
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



#if is_rcvr
static SXBA             rcvr_xcld_min_mlstn_set;
static SXBA             *rcvr_reduce_item_sets;
#endif /* is_rcvr */



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

static SXBA clause_val_set, prod_checked_set, prod_val_set, Lex_prod_item_set = Lex2prod_item_set_0;

static BOOLEAN
check_Lex (prod)
     SXINT prod;
{
  SXINT     clause, bot, top, t;
  BOOLEAN ret_val;
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
	ret_val = TRUE;
      }
      else
	ret_val = FALSE;

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
	  ret_val = TRUE;
	else
	  clause2identical [bot] = 0;
      } while (++bot < top);

      if (ret_val)
	SXBA_1_bit (prod_val_set, prod);

      return ret_val;
    }

    SXBA_1_bit (prod_val_set, prod);
    return TRUE;
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
static BOOLEAN is_mlstn2non_lexicalized_look_ahead_t_set, is_mlstn2lexicalized_look_ahead_t_set, is_lex_compatible_item_sets;


/* C'est lexicalizer_mngr qui fait les alloc */
extern SXBA basic_item_set, basic_nt_set; 

#if is_lex
extern SXBA *mlstn2lex_la_tset;
extern SXBA *mlstn2la_tset;

extern BOOLEAN lexicalizer2basic_item_set (BOOLEAN is_mlstn2lex_la_tset, BOOLEAN is_mlstn2la_tset, BOOLEAN is_smallest_insideG);
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

static SXBA q_set, mlstn_active_set, dead_ends;

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
  BOOLEAN is_first;
  SXBA line;
    
  if (is_print_time) {
    t_nb = 0;

    for (tok = 1; tok <= n; tok++) {
      t_nb += sxba_cardinal (glbl_out_source [tok]);
    }

    sprintf (mess, "\tOut_source[%ld/%ld]", n, t_nb);
    sxtime (ACTION, mess);
  }

  for (tok = 1; tok <= n; tok++) {
    if ((ste = ff_ste [tok]) < 0)
      str = sxttext (sxplocals.sxtables, -ste);
    else
      str = sxstrget (ste);

    printf ("%s {", str);
    line = glbl_out_source [tok];
    t = 0;
    is_first = TRUE;

    while ((t = sxba_scan (line, t)) > 0) {
      if (is_first)
	is_first = FALSE;
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
      ff_ste [tok_no] = EMPTY_STE;
    break;
  }
#endif /* is_supertagging */

  return 1;
}

SXINT
alloc_ff_ste ()
{
#if is_supertagging
    ff_ste = (SXINT *) sxalloc (n+2, sizeof (SXINT));
#endif /* is_supertagging */

    return 1;
}

#if is_supertagging && !is_recognizer
/* Cas ou` le supertagger est appele' directement depuis set_automaton_parser */
/* L'autre possibilite est que set_automaton_parser sert de guide au reconnaisseur earley
   qui lui appelle le supertagger */
BOOLEAN
sxearley_parse_it ()
{
  SXINT i, item, prod;
  SXBA *lb2prod_item_set, supertagger_item_set, valid_suffix_item_set;

#if MEASURES
  /* On est ds une campagne de mesures ... */
  printf ("MEASURES: There are %ld input words which select %ld terminal symbols.\n", last_tok_no, t_occur_nb);
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

static BOOLEAN		is_semact_fun, is_constraint_fun, is_prdct_fun, is_output_full_guide;


/* Si une erreur est detectee, quand on reevalue une table, on ne le fait que sur les nouveaux items */
#if is_rcvr
static SXBA	      *store_non_kernel_item_sets, T2_store_non_kernel_item_set;
static BOOLEAN        no_la_check;

#ifdef MAKE_INSIDEG
static SXBA           rcvr_w2titem_set;
static SXBA           rcvr_store_unfiltered_non_kernel_item_set;
#else /* !MAKE_INSIDEG */
static SXBA_CST_ALLOC (rcvr_w2titem_set, itemmax+1); 
static SXBA_CST_ALLOC (rcvr_store_unfiltered_non_kernel_item_set, itemmax+1); 
#endif /* !MAKE_INSIDEG */

static SXBA           *rcvr_kernel_item_sets;
#endif /* is_rcvr */

static SXBA	*non_kernel_item_sets;

#ifdef MAKE_INSIDEG
static struct recognize_item {
  SXINT		**shift_NT_hd;
  SXINT		*items_stack;
  SXBA	        *index_sets;
} *RT;	
static char *RT_area; 
#else /* !MAKE_INSIDEG */
static struct recognize_item {
    SXINT		*shift_NT_hd [inputG_MAXNT + 1];
    SXINT		items_stack [inputG_MAXITEM + 1];
    SXBA	index_sets [inputG_MAXITEM + 1];
} *RT;	
#endif /* !MAKE_INSIDEG */


/* Utilise' ds dag_scanner */
char *
t2string (t)
     SXINT t;
{
  /* Le lexicaliseur n'est pas passe' */
  return tstring [t];
}

/* Connu meme si !is_rcvr */
static BOOLEAN               is_error_detected;

#if is_rcvr
static BOOLEAN               is_during_error_rcvr_processing, is_during_reduce_rcvr_validation;
static struct recognize_item empty_RT;
static SXINT                   rcvr_on_super_rule_stack [16]; /* au plus 3 quintuplets */
static BOOLEAN               is_potential_orphan_rule, is_rcvr_can_be_cyclic, is_rcvr_on_site;
#endif /* is_rcvr */

static BOOLEAN
is_in_reduce (bot, top, item)
     SXINT *bot, *top, item;
{
  while (--top >= bot) {
    if (*top == item) {
      return TRUE;
    }
  }

  return FALSE;
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
#if !is_parser && is_guide || LOG
static SXBA		working_index_set;
#endif /* !is_parser && is_guide || LOG */

static SXBA		T2_left_corner;
static SXBA		*RT_left_corner;

static bag_header	shift_bag;

static SXBA		*ntXindex_set, lhs_order_set;

#if is_cyclic || is_rcvr
#ifdef MAKE_INSIDEG
static SXINT              *cyclic_stack;
#else /* !MAKE_INSIDEG */
static SXINT		cyclic_stack [inputG_MAXNT+1];
#endif /* !MAKE_INSIDEG */
#endif /* is_cyclic || is_rcvr */

#ifdef MAKE_INSIDEG
static SXBA             *nt_hd;
#else /* !MAKE_INSIDEG */
static SXBA		nt_hd [inputG_MAXNT+1];
#endif /* !MAKE_INSIDEG */


/* On ne peut pas utiliser
   sxba_2op (lhs_bits_array, SXBA_OP_COPY, op1_bits_array, SXBA_OP_AND, op2_bits_array)
   car les 3 operandes peuvent avoir des longueurs differentes !!
*/
static BOOLEAN
AND3 (SXBA lhs_bits_array, SXBA op1_bits_array, SXBA op2_bits_array)
{
  /* On suppose que lhs_bits_array est vide au-dela de op2_bits_array */
  SXBA	lhs_bits_ptr, op1_bits_ptr, op2_bits_ptr;
  SXINT	        slices_number = NBLONGS (BASIZE (op2_bits_array)), x;
  BOOLEAN	ret_val = FALSE;

#if EBUG
  sxbassert (*lhs_bits_array >= *op1_bits_array, "AND3 (|X|<|Y|)");
  sxbassert (*op1_bits_array >= *op2_bits_array, "AND3 (|Y|<|Z|)");
  sxbassert ((*(lhs_bits_array+NBLONGS (BASIZE (lhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs_bits_array) - 1))) == 0, "AND3");
  sxbassert ((*(op1_bits_array+NBLONGS (BASIZE (op1_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (op1_bits_array) - 1))) == 0, "AND3");
  sxbassert ((*(op2_bits_array+NBLONGS (BASIZE (op2_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (op2_bits_array) - 1))) == 0, "AND3");
  for (x = NBLONGS (BASIZE (lhs_bits_array)); x > slices_number; x--)
    sxbassert (lhs_bits_array [x] == 0UL, "AND3 (X has a non empty suffix)");
#endif
    
  lhs_bits_ptr = lhs_bits_array + slices_number,
    op1_bits_ptr = op1_bits_array + slices_number,
    op2_bits_ptr = op2_bits_array + slices_number;

  while (slices_number-- > 0) {
    if (*lhs_bits_ptr-- = (*op1_bits_ptr-- & *op2_bits_ptr--))
      ret_val = TRUE;
  }

  return ret_val;
}


#if is_parser

#include "SS.h"

static SXINT		max_tree_nb;

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
} *PT2;
static char *PT2_area; 
#else /* !MAKE_INSIDEG */
static struct PT2_item {
    SXINT			shift_NT_stack [inputG_MAXITEM - inputG_MAXPROD + 1];
    SXBA		index_sets [inputG_MAXITEM + 1];
    SXBA		backward_index_sets [inputG_MAXITEM + 1];
} *PT2;
#endif /* !MAKE_INSIDEG */


static SXBA	RL_nt_set, prod_order_set;
static SXBA	*ap_sets;
static SXBA     *red_order_sets;

static void parse_item ();
static void parse ();
#endif /* is_parser */

#if is_parser && is_rcvr
static struct parse_item empty_PT;
#endif /* is_parser && is_rcvr */

#if is_rcvr
static void
clear_table (p)
     SXINT p;
{
  if (p == rcvr_detection_table)
    /* On note si la table sur laquelle l'erreur courante a ete detectee a ete razee */
    /* car il faudra peut etre la restaurer a partir de la valeur sauvegardee ds T[working_rcvr_mlstn]
       par copy_table () */
    rcvr_detection_table_cleared = TRUE;

#ifdef MAKE_INSIDEG
 {
   SXINT                   i;
   struct recognize_item *RT_ptr;

   RT_ptr = RT+p;

   for (i = MAXNT; i >= 0; i--) {
     RT_ptr->shift_NT_hd [i] = NULL; /* C'est un bag */
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
     PT_ptr->reduce_NT_hd [i] = NULL; /* C'est un bag */
   }
 }
#else /* !MAKE_INSIDEG */
  PT [p] = empty_PT;
#endif /* !MAKE_INSIDEG */
#endif /* is_parser */
}

/* T [p] = T [q] */
static void
copy_table (p, q)
     SXINT p, q;
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
      set2 = RTp->index_sets [item] = sxba_bag_get (&shift_bag, BASIZE (set));
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
      set2 = RTp->index_sets [item] = sxba_bag_get (&shift_bag, BASIZE (set));
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

#if is_parser
  PTp = PT+p;
  PTq = PT+q;

#ifdef MAKE_INSIDEG
  for (item = MAXITEM; item >= 0; item--) {
    if (set = PTq->backward_index_sets [item]) {
      set2 = PTp->backward_index_sets [item] = sxba_bag_get (&shift_bag, BASIZE (set));
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
      set2 = PTp->backward_index_sets [item] = sxba_bag_get (&shift_bag, BASIZE (set));
      sxba_copy (set2, set);
    }
  }
#endif /* !MAKE_INSIDEG */

#endif /* is_parser */
}
#endif /* is_rcvr */


#if is_parser || is_rcvr
/* On ne peut utiliser
   sxba_2op (NULL, SXBA_OP_IS, lhs_bits_array, SXBA_OP_AND, rhs_bits_array)
   car les 2 operandes peuvent avoir des tailles differentes et l'intersection n'est
   testee que sur leurs prefixes communs
*/
static BOOLEAN
IS_AND (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
  SXBA	        lhs_bits_ptr, rhs_bits_ptr;
  SXINT	        slices_number = NBLONGS (BASIZE (rhs_bits_array));
  SXINT	        lhs_slices_number = NBLONGS (BASIZE (lhs_bits_array));

#if EBUG
  sxbassert ((*(lhs_bits_array+lhs_slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs_bits_array) - 1))) == 0, "IS_AND");
  sxbassert ((*(rhs_bits_array+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs_bits_array) - 1))) == 0, "IS_AND");
#endif

  if (lhs_slices_number < slices_number)
    slices_number = lhs_slices_number;

  lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

  while (slices_number-- > 0) {
    if (*lhs_bits_ptr-- & *rhs_bits_ptr--)
      return TRUE;
  }

  return FALSE;
}
#endif /* is_parser || is_rcvr */


#if is_parser
/* bits_array1 or (bits_array2 & bits_array3) */
/* Retourne TRUE ssi bits_array1 a change' */
static BOOLEAN
OR_AND (SXBA bits_array1, SXBA bits_array2, SXBA bits_array3)
{
  SXBA	        bits_ptr1, bits_ptr2, bits_ptr3;
  SXINT	        slices_number = NBLONGS (BASIZE (bits_array2));
  SXBA_ELT	val1, val2;
  BOOLEAN	has_changed = FALSE;

#if EBUG
  sxbassert (*bits_array1 >= *bits_array2, "OR_AND (|X|<|Y|)");
  sxbassert (*bits_array2 <= *bits_array3, "OR_AND (|Y|>|Z|)");
  sxbassert ((*(bits_array1+NBLONGS (BASIZE (bits_array1))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array1) - 1))) == 0, "OR_AND");
  sxbassert ((*(bits_array2+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array2) - 1))) == 0, "OR_AND");
  sxbassert ((*(bits_array3+NBLONGS (BASIZE (bits_array3))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array3) - 1))) == 0, "OR_AND");
#endif

  bits_ptr1 = bits_array1 + slices_number, bits_ptr2 = bits_array2 + slices_number, bits_ptr3 = bits_array3 + slices_number;

  while (slices_number-- > 0) {
    if ((val1 = *bits_ptr1) != (val2 = ((*bits_ptr2--) & (*bits_ptr3--)))) {
      val2 |= val1;

      if (val1 != val2) {
	*bits_ptr1 = val2;
	has_changed = TRUE;
      }
    }

    bits_ptr1--;
  }

  return has_changed;
}


static BOOLEAN
OR_AND_MINUS (SXBA bits_array1, SXBA bits_array2, SXBA bits_array3, SXBA bits_array4)
{
  /* bits_array4 =  bits_array2 & bits_array3 - bits_array1
     bits_array1 |= bits_array4
     retourne vrai ssi bits_array4 est non vide */
  SXBA	        bits_ptr1, bits_ptr2, bits_ptr3, bits_ptr4;
  /* On change la taille! |bits_array4| == |bits_array2| */
  SXINT	        slices_number = NBLONGS (BASIZE (bits_array4) = BASIZE (bits_array2));
  SXBA_ELT	val;
  BOOLEAN	is_set = FALSE;

#if EBUG
  sxbassert (*bits_array1 >= *bits_array2, "OR_AND_MINUS (|X|<|Y|)");
  sxbassert (*bits_array3 >= *bits_array2, "OR_AND_MINUS (|Z|<|Y|)");
  sxbassert ((*(bits_array1+NBLONGS (BASIZE (bits_array1))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array1) - 1))) == 0, "OR_AND_MINUS");
  sxbassert ((*(bits_array2+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array2) - 1))) == 0, "OR_AND_MINUS");
  sxbassert ((*(bits_array3+NBLONGS (BASIZE (bits_array3))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array3) - 1))) == 0, "OR_AND_MINUS");
  /* Pas de verif sur bits_array4 qui sert d'ensemble de travail */
#endif

  bits_ptr1 = bits_array1 + slices_number,
    bits_ptr2 = bits_array2 + slices_number,
    bits_ptr3 = bits_array3 + slices_number,
    bits_ptr4 = bits_array4 + slices_number;

  while (slices_number-- > 0) {
    if ((val = *bits_ptr4-- = (*bits_ptr2--) & (*bits_ptr3--) & (~(*bits_ptr1)))) {
      is_set = TRUE;
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
  SXINT	        rhs_slices_number = NBLONGS (BASIZE (rhs_bits_array));
  SXINT	        lhs_slices_number = NBLONGS (BASIZE (lhs_bits_array));
  BOOLEAN	ret_val = FALSE;
    
  lhs_bits_ptr = lhs_bits_array + lhs_slices_number, rhs_bits_ptr = rhs_bits_array + rhs_slices_number;

#if EBUG
  sxbassert (*lhs_bits_array >= *rhs_bits_array, "COPY (|X|<|Y|)");
  sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs_bits_array) - 1))) == 0, "COPY");
  sxbassert ((*rhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs_bits_array) - 1))) == 0, "COPY");
#endif

  lhs_slices_number -= rhs_slices_number;

  while (lhs_slices_number-- > 0)
    *lhs_bits_ptr-- = (SXBA_ELT) 0;

  while (rhs_slices_number-- > 0)
    *lhs_bits_ptr-- = *rhs_bits_ptr--;
}

/* Retourne vrai ssi la lhs change */
static BOOLEAN
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
  SXINT	              slices_number = NBLONGS (BASIZE (rhs_bits_array));
  SXBA_ELT            lwork, work;
  BOOLEAN             ret_val = FALSE;
    
  lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

#if EBUG
  sxbassert (*lhs_bits_array >= *rhs_bits_array, "OR (|X|<|Y|)");
  sxbassert ((*(lhs_bits_array+NBLONGS (BASIZE (lhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs_bits_array) - 1))) == 0, "OR");
  sxbassert ((*rhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs_bits_array) - 1))) == 0, "OR");
#endif

  while (slices_number-- > 0) {
    work = (*rhs_bits_ptr-- | (lwork = *lhs_bits_ptr));

    if (lwork /* ancienne valeur */ != work /* nouvelle valeur */) {
      /* A change' */
      ret_val = TRUE;
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
static BOOLEAN
check_streq (clause)
     SXINT clause;
{
  SXINT x, *p, j, h;
  SXINT	*src1, *src2, *lim1;
  BOOLEAN is_Streqlen;

  if (x = clause2streq [clause]) {
    p = streq_list+x;

    while (j = *p++) {
      if (j > 0) {
	is_Streqlen = TRUE;
	h = *p++;
      }
      else {
	is_Streqlen = FALSE;
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

  return TRUE;
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

    return TRUE;
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
 
  return TRUE;
}

#endif /* is_full_guide */

#if MEASURES || LOG
static SXBA used_prod_set;
extern void output_G (char *header, SXBA basic_item_set, SXBA basic_prod_set);
#endif /* MEASURES || LOG */

#if LOG
static void output_ni_prod (SXINT prod, SXBA lb_set, SXINT i, SXINT j);
#if LLOG
static void output_table (SXINT p);
#endif /* LLOG */
#endif /* LOG */



/* Ds le cas is_dag, p et q sont des milestones (et pas des etats du dag) */
/* dans tous les cas on a donc p <= q */
/* Changement le 17/07/2003
   Arg supplementaire qui indique s'il y a eu un reduce
   Retourne vrai ssi la table Tq a change' */
/* Le 21/11/06  Retourne vrai ssi il y a depuis p une transition sur A */
static BOOLEAN
shift (A, p, q, has_reduce, from_cyclic_stack)
    SXINT A, p, q;
    BOOLEAN *has_reduce, from_cyclic_stack;
{
  /*
    Realise la transition Apq
    p < q
    Si A == 0 => transition terminale
  */
  SXINT			item_p, item_q;
  SXINT			**ptr2;
  SXINT 			X, B, Y, YY, prod, new_p, prdct_no, order, i, t;
  SXBA			index_set_p, index_set_q, prev_index_set_q, backward_index_set_q;
  struct recognize_item	*RTp;
  SXINT			*top_ptr, *bot_ptr;
  BOOLEAN		ret_val;

  ret_val = *has_reduce = FALSE;

  RTp = &(RT [p]);

  top_ptr = RTp->shift_NT_hd [A];
    
#if is_lex
  if (top_ptr == NULL && A > 0)
    /* La lexicalisation a pu invalider la "preparation" de transitions nt */
    return FALSE;
#endif /* is_lex */

#if EBUG
  if (top_ptr == NULL)
    sxtrap (ME, "shift");
#endif

  bot_ptr = &(RTp->items_stack [0]);

  if (A > 0)
    bot_ptr += RHS_NT2WHERE (A);

  while (--top_ptr >= bot_ptr) {
    ret_val = TRUE; /* Le 21/11/06 */
    item_p = *top_ptr;

    if (A == 0) {
      /* transition terminale, il faut verifier qu'elle conduit bien a q... */
      t = -LISPRO (item_p);

      if (!SXBA_bit_is_set (pq_t_set, t))
	/* ... non */
	continue;
    }

    /* item_p = B -> \beta .X Y \gamma */

    item_q = item_p+1;
    /* item_q = B -> \beta X .Y \gamma */
    Y = LISPRO (item_q);

    /* On verifie le look_ahead de item_q... */
#if is_1LA
#if is_rcvr
    /* Le 11/10/06 */
    /* meme si ce n'est pas un bon item pour le look_ahead, on le note car on pourra tenter de la rcvr depuis la` */
    /* Ne pas le faire si on est en train de tester si une correction par reduce marche */
    if (!is_during_reduce_rcvr_validation && Y != 0 /* Pas sur les reduce */)
      SXBA_1_bit (rcvr_kernel_item_sets [q], item_q); /* nelle version */
    /* Il faut aussi remplir les index_set si echec du look_ahead A FAIRE */
#endif /* is_rcvr */

    if (T2_look_ahead_item_set && !SXBA_bit_is_set (T2_look_ahead_item_set, item_q))
      /* Echec du test du look-ahead */
      continue;
#endif /* is_1LA */

    /* ... et le predicat de item_p */
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
#endif

    index_set_p = RTp->index_sets [item_p];
    index_set_q = T2_index_sets [item_q];

#if !is_parser && is_guide || LOG
    /* Le 13/01/06 */
    /* Pour connaitre les changements */
    if (index_set_q == NULL)
      prev_index_set_q = NULL;
    else {
      prev_index_set_q = working_index_set;
      *prev_index_set_q = *index_set_q; /* On force la meme longueur */
      sxba_copy (prev_index_set_q, index_set_q);
    }
#endif /* !is_parser && is_guide || LOG */

#if is_parser
    new_p = p;
#endif /* is_parser */

#if is_epsilon
    while (Y != 0) {
      /* modif du 6/5/2002 */
#if is_rcvr
      /* Le 11/10/06 */
      /* meme si ce n'est pas un bon item, on le note car on pourra tenter de la rcvr depuis la` */
      /* Ne pas le faire si on est en train de tester si une correction par reduce marche */
      if (!is_during_reduce_rcvr_validation)
	SXBA_1_bit (rcvr_kernel_item_sets [q], item_q);
#endif /* is_rcvr */

      if (Y < 0) {
	/* Verification du scan */
	YY = -Y;

	if (SXBA_bit_is_set (T2_look_ahead_t_set, YY))
	  /* YY est un bon look_ahead */
	  YY = 0;
	else {
	  /* echec du look-ahead */
#if is_rcvr
	  /* Le 11/10/06 */
	  /* meme si ce n'est pas un bon terminal, on le note car on pourra tenter de la rcvr depuis la` */
	  /* Ne pas le faire si on est en train de tester si une correction par reduce marche */
	  if (!is_during_reduce_rcvr_validation) {
	    if (index_set_q == NULL)
	      index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	    if (index_set_p == NULL)
	      SXBA_1_bit (index_set_q, p);
	    else
	      OR (index_set_q, index_set_p);

#if is_parser
	    backward_index_set_q = T2_backward_index_sets [item_q];

	    if (backward_index_set_q == NULL)
	      backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	    SXBA_1_bit (backward_index_set_q, new_p);
#endif /* is_parser */
	  }
#endif /* is_rcvr */

	  break;
	}

#if !is_parser && is_supertagging
	SXBA_1_bit (item2source_index_set [item_q], q);
#if LOG
	printf ("%s [%ld, %ld]\n", sxttext (sxplocals.sxtables, -Y), PROLIS (item_q), q);
#endif
#endif /* !is_parser && is_supertagging */
      }
      else
	YY = Y;

      /* fin modif du 6/5/2002 */

#if 0
      /* Le 17/07/2003 */
      ret_val = TRUE; /* On va ajouter qqchose a Tq */
#endif /* 0 */

      if (index_set_q == NULL) {
	if (*(ptr2 = &(T2_shift_NT_hd [YY])) == NULL) {
	  /* c'est la 1ere fois qu'on rencontre YY */
	  /* rhs_nt2where [0] == 0 (pour les terminaux) */
	  *ptr2 = &(T2_items_stack [RHS_NT2WHERE (YY)]);

	  if (Y > 0 && !SXBA_bit_is_set (T2_left_corner, Y)) {
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

	*((*ptr2)++) = item_q;

	if (X > 0) {
	  index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	  if (index_set_p == NULL)
	    SXBA_1_bit (index_set_q, p);
	  else
	    COPY (index_set_q, index_set_p);

#if is_parser
#if EBUG
	  if (T2_backward_index_sets [item_q] != NULL)
	    sxtrap (ME, "shift");
#endif
	  backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
	  SXBA_1_bit (backward_index_set_q, new_p);
#endif /* is_parser */
	}
	else {
	  /* item_q = B -> \alpha a . Y \beta */
#if EBUG
	  if (A > 0)
	    sxtrap (ME, "shift");
#endif

	  index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	  if (index_set_p == NULL) {
	    /* \alpha == \epsilon */
	    SXBA_1_bit (index_set_q, p);
	  }
	  else
	    /* On ne reutilise pas index_set_p */
	    COPY (index_set_q, index_set_p);

#if is_parser
	  /* Le 05/12/05 on met des backward sur les terminaux de !is_dag ds le cas is_rcvr pour de la rcvr sur les terminaux */
	  /* Ajoute le 07/07/2003 */
#if EBUG
	  if (T2_backward_index_sets [item_q] != NULL)
	    sxtrap (ME, "shift");
#endif
	  backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
	  SXBA_1_bit (backward_index_set_q, new_p);
#endif /* is_parser */
	}
      }
      else {
	/* index_set_q != NULL */
	/* X peut etre <0 en cas de "reexecution" (cas d'un appel depuis le traitement d'erreurs). */
		
	if (index_set_p == NULL)
	  SXBA_1_bit (index_set_q, p);
	else
	  OR (index_set_q, index_set_p);

#if is_parser
	/* Le 05/12/05 on met des backward sur les terminaux de !is_dag ds le cas is_rcvr pour de la rcvr sur les terminaux */
	/* Ajoute le 07/07/2003 */
#if EBUG
	if (T2_backward_index_sets [item_q] == NULL)
	  sxtrap (ME, "shift");
#endif

	backward_index_set_q = T2_backward_index_sets [item_q];
	SXBA_1_bit (backward_index_set_q, new_p);
#endif /* is_parser */
      }

      if (
#ifdef MAKE_INSIDEG
	  spf.insideG.bvide &&
#endif /* MAKE_INSIDEG */
	  Y > 0 && SXBA_bit_is_set (EMPTY_SET, Y)) {
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

#if is_1LA
#if is_rcvr
	/* Le 11/10/06 */
	/* meme si ce n'est pas un bon item pour le look_ahead, on le note car on pourra tenter de la rcvr depuis la` */
	/* Ne pas le faire si on est en train de tester si une correction par reduce marche */
	if (!is_during_reduce_rcvr_validation && Y != 0 /* Pas sur les reduce */)
	  SXBA_1_bit (rcvr_kernel_item_sets [q], item_q); /* nelle version */
	/* Il faut aussi remplir les index_set si echec du look_ahead A FAIRE */
#endif /* is_rcvr */

	if (T2_look_ahead_item_set && !SXBA_bit_is_set (T2_look_ahead_item_set, item_q))
	  /* Echec du test du look-ahead */
	  continue;
#endif /* is_1LA */

#if is_parser
	new_p = q;
#endif /* is_parser */

	index_set_q = T2_index_sets [item_q];

#if !is_parser && is_guide || LOG
	/* Le 13/01/06 */
	/* Pour connaitre les changements */
	if (index_set_q == NULL)
	  prev_index_set_q = NULL;
	else {
	  prev_index_set_q = working_index_set;
	  *prev_index_set_q = *index_set_q; /* On force la meme longueur */
	  sxba_copy (prev_index_set_q, index_set_q);
	}
#endif /* !is_parser && is_guide || LOG */
      }
      else
	break;
    }

    if (Y != 0)
      continue;
#else /* !is_epsilon */
    /* is_epsilon = FALSE */
    if (Y != 0) {
#if is_rcvr
      /* Le 11/10/06 */
      /* meme si ce n'est pas un bon item, on le note car on pourra tenter de la rcvr depuis la` */
      /* Ne pas le faire si on est en train de tester si une correction par reduce marche */
      if (!is_during_reduce_rcvr_validation)
	SXBA_1_bit (rcvr_kernel_item_sets [q], item_q);
#endif /* is_rcvr */

      if (Y < 0) {
      /* Verification du scan */
	YY = -Y;

	if (SXBA_bit_is_set (T2_look_ahead_t_set, YY))
	  /* YY est un bon look_ahead */
	  YY = 0;
	else {
	  /* echec du look-ahead */
#if is_rcvr
	  /* Le 11/10/06 */
	  /* meme si ce n'est pas un bon terminal, on le note car on pourra tenter de la rcvr depuis la` */
	  /* Ne pas le faire si on est en train de tester si une correction par reduce marche */
	  if (!is_during_reduce_rcvr_validation) {
	    if (index_set_q == NULL)
	      index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	    if (index_set_p == NULL)
	      SXBA_1_bit (index_set_q, p);
	    else
	      OR (index_set_q, index_set_p);

#if is_parser
	    backward_index_set_q = T2_backward_index_sets [item_q];

	    if (backward_index_set_q == NULL)
	      backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	    SXBA_1_bit (backward_index_set_q, new_p);
#endif /* is_parser */
	  }
#endif /* is_rcvr */

	  /* on examine la transition suivante... */
	  continue;
	}

#if !is_parser && is_supertagging
	SXBA_1_bit (item2source_index_set [item_q], q);
#if LOG
	printf ("%s [%ld, %ld]\n", sxttext (sxplocals.sxtables, -Y), PROLIS (item_q), q);
#endif
#endif /* !is_parser && is_supertagging */
      }
      else
	YY = Y;

      /* fin modif du 6/5/2002 */

#if 0
      /* Le 17/07/2003 */
      ret_val = TRUE; /* On va ajouter qqchose a Tq */
#endif /* 0 */

      if (index_set_q == NULL) {
	/* C'est la 1ere fois qu'on rencontre item_q */
	if (*(ptr2 = &(T2_shift_NT_hd [YY])) == NULL) {
	  /* c'est la 1ere fois qu'on rencontre YY */
	  /* rhs_nt2where [0] == 0 (pour les terminaux) */
	  *ptr2 = &(T2_items_stack [RHS_NT2WHERE (YY)]);

	  if (Y > 0 && !SXBA_bit_is_set (T2_left_corner, Y)) {
	    sxba_or (T2_left_corner, LEFT_CORNER (Y));
	    sxba_or (T2_non_kernel_item_set, NT2ITEM_SET (Y));
	  }
	}

	*((*ptr2)++) = item_q;

	if (A > 0) {
	  /* Apres un NT, on cree toujours un nouvel index_set */
	  index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	  if (index_set_p == NULL)
	    SXBA_1_bit (index_set_q, p);
	  else
	    COPY (index_set_q, index_set_p);

#if is_parser
#if EBUG
	  if (T2_backward_index_sets [item_q] != NULL)
	    sxtrap (ME, "shift");
#endif

	  backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
	  SXBA_1_bit (backward_index_set_q, p);
#endif /* is_parser */
	}
	else {
	  /* shift sur un terminal */
	  index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	  if (index_set_p == NULL) {
	    SXBA_1_bit (index_set_q, p);
	  }
	  else
	    /* On ne reutilise pas index_set_p */
	    COPY (index_set_q, index_set_p);

#if is_parser
	  /* Pas de backward apres un terminal... */
	  /* ... sauf ds le cas is_dag */
	  /* ... ou is_rcvr (le 05/12/05) */
	  /* Ajoute le 07/07/2003 */
#if EBUG
	  if (T2_backward_index_sets [item_q] != NULL)
	    sxtrap (ME, "shift");
#endif
	  backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
	  SXBA_1_bit (backward_index_set_q, p);
#endif /* is_parser */
	}
      }
      else {
	/* index_set_q != NULL */
	/* A peut etre <0 en cas de "reexecution" (cas d'un appel depuis le traitement d'erreurs). */

	if (index_set_p == NULL)
	  SXBA_1_bit (index_set_q, p);
	else
	  OR (index_set_q, index_set_p);

#if is_parser
#if EBUG
	if (T2_backward_index_sets [item_q] == NULL)
	  sxtrap (ME, "shift");
#endif

	backward_index_set_q = T2_backward_index_sets [item_q];
	SXBA_1_bit (backward_index_set_q, p);
#endif /* is_parser */
      }

      continue;
    }
#endif /* !is_epsilon */

    /* Y == 0 */
    /* item_q = B -> \alpha A \beta .  et \beta =>* \epsilon */
    prod = PROLIS (item_q);
    B = LHS (prod);

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

    /* On ne stocke pas les reduces ds le cas du reconnaisseur. */
    /* Pas de backward apres un terminal... sauf ds le cas is_dag */
    if (index_set_q == NULL) {
      if (*(ptr2 = &(T2_reduce_NT_hd [B])) == NULL) {
	/* c'est la 1ere fois qu'on fait un reduce vers B */
	*ptr2 = &(T2_items_stack [LHS_NT2WHERE (B)]);
      }

#if is_rcvr
      /* Ne pas le faire si on est en train de tester si une correction par reduce marche */
      if (!is_during_reduce_rcvr_validation)
	SXBA_1_bit (rcvr_reduce_item_sets [q], item_q);
#endif /* is_rcvr */

      *((*ptr2)++) = item_q;
		
      if (X > 0) {
	index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	if (index_set_p == NULL)
	  SXBA_1_bit (index_set_q, p);
	else
	  COPY (index_set_q, index_set_p);

#if EBUG
	if (T2_backward_index_sets [item_q] != NULL)
	  sxtrap (ME, "shift");
#endif

	backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
	SXBA_1_bit (backward_index_set_q, new_p);
      }
      else {
	index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

	if (index_set_p == NULL) {
	  SXBA_1_bit (index_set_q, p);
	}
	else
	  /* On ne reutilise pas index_set_p */
	  COPY (index_set_q, index_set_p);

	/* Ajoute le 07/07/2003 */
#if EBUG
	if (T2_backward_index_sets [item_q] != NULL)
	  sxtrap (ME, "shift");
#endif
	backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
	SXBA_1_bit (backward_index_set_q, new_p);
      }
    }
    else {
      /* index_set_q != NULL */
      /* X peut etre <=0 en cas de "reexecution" (cas d'un appel depuis le traitement d'erreurs). */
      if (index_set_p == NULL)
	SXBA_1_bit (index_set_q, p);
      else
	OR (index_set_q, index_set_p);

      /* Ajoute le 07/07/2003 */
      backward_index_set_q = T2_backward_index_sets [item_q];
#if EBUG
      if (backward_index_set_q == NULL)
	sxtrap (ME, "shift");
#endif
      SXBA_1_bit (backward_index_set_q, new_p);
    }
#else /* !is_parser */

#if is_supertagging
    if (index_set_p == NULL) {
      supertag (prod, p, q);
    }
    else {
      supertag (prod, sxba_scan (index_set_p, 0), q);
    }
#endif /* is_supertagging */

#if is_guide
    /* Le guide peut etre produit soit par le reconnaisseur (ici) soit par RL_mreduction */
    /* On utilise index_set_p (et non index_set_q) ca peut eviter certains appels (prod, i, j) deja
       effectues lors d'un appel precedent de shift() */
    /* Le 13/01/06 on peut reutiliser index_set_q */
    if (prev_index_set_q) {
      /* on ne passe que les nouveaux index_set_q - prev_index_set_q */
      *prev_index_set_q = *index_set_q; /* prudence sur les tailles */

      if (sxba_2op (prev_index_set_q, SXBA_OP_COPY, index_set_q, SXBA_OP_MINUS, prev_index_set_q)) {
	output_guide (prod, prev_index_set_q, p, q);
#if LOG
	output_ni_prod (prod, prev_index_set_p, p, q);
#endif /* LOG */
      }
    }
    else {
      /* tout est nouveau ds index_set_q */
      output_guide (prod, index_set_p, p, q);
#if LOG
      output_ni_prod (prod, index_set_p, p, q);
#endif /* LOG */
    }
#endif /* is_guide */
#endif /* !is_parser */

#if LOG && (is_parser || !is_guide)
    /* le cas is_guide est traite' ci dessus (il touche a prev_index_set_q) */
    if (prev_index_set_q) {
      /* on ne passe que les nouveaux index_set_q - prev_index_set_q */
      *prev_index_set_q = *index_set_q; /* prudence sur les tailles */

      if (sxba_2op (prev_index_set_q, SXBA_OP_COPY, index_set_q, SXBA_OP_MINUS, prev_index_set_q))
	output_ni_prod (prod, prev_index_set_q, p, q);
    }
    else
      /* tout est nouveau ds index_set_q */
      output_ni_prod (prod, index_set_p, p, q);
#endif /* LOG && (is_parser || !is_guide) */

#ifdef MAKE_INSIDEG
    /* nt2order, ... n'ont pas ete calcules dynamiquement, on essaie de se debrouiller autrement */
    /* On examine les nt par ordre decroissant */
    /* Si B < A (ou A < 0), B n'a pas encore ete examine' => pas de PB */
    /* Si B > A > 0, B a deja ete examine'.
          Si p \in index_set_p
             Si p \in ntXindex_set [B] => Bpq a deja ete examine' (ou le sera grace a cyclic_stack)
             Si p \not \in ntXindex_set [B] => Bpq n'a pas encore  ete examine' => on le met ds cyclic_stack
    */
    if (index_set_p == NULL) {
      /* item_q = B -> A .  */
#if is_cyclic || is_rcvr
      if (A > 0 /* pas ds le cas scan */
	  && (B > A /* pas ds le bon ordre */ || from_cyclic_stack /* appel depuis cyclic_stack */)
	  && !SXBA_bit_is_set (ntXindex_set [B], p) /* Bp est nouveau */
	  /* Si A provient de cyclic_stack, tous les shift(p,q) suivants en proviennent aussi */
	  ) {
	/* son ordre de passage a deja ete examine, on force son re-examen */
	PUSH (cyclic_stack, B);
      }
#endif /* is_cyclic || is_rcvr */

      SXBA_1_bit (ntXindex_set [B], p);
    }
    else {
      /* item_q = B -> \alpha A \beta .
	 et \beta =>* \epsilon
	 et \alpha \not =>* \epsilon */
#if is_cyclic || is_rcvr
      /* Ajout le 05/12/05 */
      if (A > 0
	  && (B > A /* pas ds le bon ordre */ || from_cyclic_stack /* appel depuis cyclic_stack */)
	  && SXBA_bit_is_set (index_set_p, p)
	  /* Si A provient de cyclic_stack, tous les shift(p,q) suivants en proviennent aussi */
	  && !SXBA_bit_is_set (ntXindex_set [B], p) /* Bp est nouveau */
	  ) {
	PUSH (cyclic_stack, B);
      }
#endif /* is_cyclic || is_rcvr */

      OR (ntXindex_set [B], index_set_p);
    }

    /* On met ds tous les cas car c,a sert aussi a faire les bons "clear" a la fin de scan_reduce */
    SXBA_1_bit (lhs_order_set, B);
#else /* !MAKE_INSIDEG */
    /* modif du 6/5/2002 */
    order = nt2order [B];

    if (index_set_p == NULL) {
      /* item_q = B -> A .  */
#if is_cyclic || is_rcvr
      if (A > 0 /* pas ds le cas scan */
	  && !SXBA_bit_is_set (ntXindex_set [B], p) /* p est nouveau */
	  /* Si A provient de cyclic_stack, tous les shift(p,q) suivants en proviennent aussi */
	  && (order >= nt2order [A] /* pas ds le bon ordre */ || from_cyclic_stack /* provient de cyclic_stack */)
#if !is_cyclic && is_rcvr
	  && is_error_detected
#endif /* !is_cyclic && is_rcvr */   
	  ) {
	/* On est ds le cas cyclique, Bp est nouveau et son ordre de passage
	   a deja ete examine, on force son re-examen */
	PUSH (cyclic_stack, B);
      }
#endif /* is_cyclic || is_rcvr */
      SXBA_1_bit (ntXindex_set [B], p);
    }
    else {
      /* item_q = B -> \alpha A \beta .
	 et \beta =>* \epsilon
	 et \alpha \not =>* \epsilon */
#if is_rcvr
      /* Ajout le 05/12/05 */
      if (is_error_detected && A > 0
	  && SXBA_bit_is_set (index_set_p, p)
	  /* Si A provient de cyclic_stack, tous les shift(p,q) suivants en proviennent aussi */
	  && (order >= nt2order [A] /* pas ds le bon ordre */ || from_cyclic_stack /* provient de cyclic_stack */)
	  && !SXBA_bit_is_set (ntXindex_set [B], p))
	PUSH (cyclic_stack, B);
#endif /* is_rcvr */    

      OR (ntXindex_set [B], index_set_p);
    }

    /* On met ds tous les cas car c,a sert aussi a faire les bons "clear" a la fin de scan_reduce */
    SXBA_1_bit (lhs_order_set, order);
#endif /* !MAKE_INSIDEG */

    *has_reduce = TRUE;		/* reduce */
  }

  return ret_val;
}




/* modif du 6/5/2002 */
/* Le 21/11/06 passe de fonction booleenne a procedure */
#ifdef MAKE_INSIDEG
/* Nelle version du 06/12/06 a adapter au cas normal si concluante !! */
static void
scan_reduce (p, q)
    SXINT 	p, q;
{
  SXINT	      j, next_j, k, A, next_A;
  SXBA        index_set_A;
  BOOLEAN     has_reduce;

  j = p;
  A = sxba_1_rlscan (lhs_order_set, MAXNT+1);

  while (j >= 0) {
    do {
      if (SXBA_bit_is_set (ntXindex_set [A], j))
	/* ... oui pour Ajq */
	shift (A, j, q, &has_reduce, FALSE /* from_cyclic_stack */);
    } while ((A = sxba_1_rlscan (lhs_order_set, A)) > 0);

#if is_cyclic || is_rcvr
    /* Ici, on traite les lhs des reductions  qui n'ont pas suivies l'ordre decroissant du code des nt */
    while (!IS_EMPTY (cyclic_stack)) {
      A = POP (cyclic_stack);
	  
#if EBUG
      if (!SXBA_bit_is_set (ntXindex_set [A], j))
	sxtrap (ME, "scan_reduce");
#endif /* EBUG */
  
      /* ... oui pour Ajq */
      shift (A, j, q, &has_reduce, TRUE /* from_cyclic_stack */);
    }
#endif /* is_cyclic || is_rcvr */

    /* Ici le traitement du niveau j est termine', lhs_order_set est complet */
    next_j = -1;
    A = 0;

    while ((A = sxba_scan (lhs_order_set, A)) > 0) {
      index_set_A = ntXindex_set [A];
      SXBA_0_bit (index_set_A, j); /* Parfois inutile */

      if ((k = sxba_1_rlscan (index_set_A, j)) < 0) {
	/* Plus d'indices pour A, on le vire */
	SXBA_0_bit (lhs_order_set, A);
      }
      else {
	/* Il existe des Akq avec k < j */
	next_A = A;
	
	if (k > next_j)
	  next_j = k; /* Le + grand */
      }
    }

    j = next_j;
    A = next_A; /* Le + grand */
  }
}
#else /* !MAKE_INSIDEG */
static void
scan_reduce (p, q)
    SXINT 	p, q;
{
  SXBA_ELT    filtre;
  SXINT	      j, indice, order, A;
  BOOLEAN     has_reduce;

  filtre = 1 << MOD (p);
  indice = DIV (p) + 1;

  j = p;

  for (;;) {
    order = MAXNT+1;

    while ((order = sxba_1_rlscan (lhs_order_set, order)) > 0) {
      A = order2nt [order];

      /* if (SXBA_bit_is_set_reset (ntXindex_set [A], j)) */
      if (nt_hd [A] == NULL)
	/* Les appels a shift ont pu rajouter des A */
	nt_hd [A] = ntXindex_set [A] + indice;

      if (*(nt_hd [A]) & filtre) {
	shift (A, j, q, &has_reduce, FALSE /* from_cyclic_stack */);
      }
    }

#if is_cyclic || is_rcvr
    /* Ici, on traite les (nouveaux) shift qui n'ont pas suivi l'ordre statique d'evaluation */
    while (!IS_EMPTY (cyclic_stack)) {
      A = POP (cyclic_stack);

      if (nt_hd [A] == NULL)
	/* Les appels a shift ont pu rajouter des A */
	nt_hd [A] = ntXindex_set [A] + indice;
	    
      shift (A, j, q, &has_reduce, TRUE /* from_cyclic_stack */);
    }
#endif /* is_cyclic || is_rcvr */

    j--;
	
    if ((filtre >>= 1) == 0) {
      order = 0;

      while ((order = sxba_scan (lhs_order_set, order)) > 0) {
	A = order2nt [order];

	if (nt_hd [A] != NULL)
	  /* nt_hd [A] peut ne pas etre positionne' si A a ete mis ds lhs_order_set (par shift),
	     nt2order[A] > order courant, pas cyclique et j == k*32 */
	  *((nt_hd [A])--) = 0;

	if (j < 0) {
	  SXBA_0_bit (lhs_order_set, order);
	  nt_hd [A] = NULL;
	}
      }

      if (j < 0)
	break;

      filtre = 1 << MOD (j);
      indice--;
    }
  }
}
#endif /* !MAKE_INSIDEG */


/* Le 22/09/06 Nelle version.  Sauf si no_la_check, T2_look_ahead_item_set
   est positionne' (meme si is_error_detected) car, apres de la rcvr
   on continue a traiter le (vrai) source !! */
/* q est un mlstn, positionne les variables "T2" qui referencent les tables q */
/* et Positionne ilex_compatible_item_set et T2_look_ahead_item_set */
static void
set_T2 (q)
     SXINT q;
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
process_non_kernel_filtering (q)
     SXINT q;
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
process_non_kernel (q)
    SXINT q;
{
  /* On prepare la transition depuis q en stockant ds T2 les items non-kernel (scan ou nt_trans). */
  SXINT		**ptr2;
  SXINT		item, A, X, XX, prdct_no, next_item, prod;
  SXBA	        index_set;
  BOOLEAN       is_new_item;
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
      printf ("%s [%ld, %ld]\n", sxttext (sxplocals.sxtables, -X), PROLIS (item), q);
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
      /* is_epsilon == FALSE */
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

#if LLOG
  output_table (q);
#endif /* LLOG */
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
#if LOG || EBUG
static void
output_rcvr_prod (litem, lmlstn, ritem, rmlstn)
     SXINT litem, lmlstn, ritem, rmlstn;
{
  SXINT top_item, bot_item, prod, X;

  prod = PROLIS (litem);
  printf ("%ld:<%s> = ", prod, NTSTRING (LHS (prod)));

  top_item = PROLON (prod+1)-1;
  bot_item = PROLON (prod);

  while (bot_item <= top_item) {
    if (bot_item == litem) {
      printf ("[%ld] ", lmlstn);
    }

    if (bot_item == ritem) {
      printf ("[%ld] ", rmlstn);
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

  printf ("%ld\t", prod);

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

  printf ("%ld:<%s[", PROD (prod), NTSTRING (LHS (prod)));

  if (lb_set == NULL)
    printf ("%ld", i);
  else {
    fputs ("{", stdout);
    lb = sxba_scan (lb_set, -1);
    printf ("%ld", lb);

    while ((lb = sxba_scan (lb_set, lb)) >= 0) {
      printf (", %ld", lb);
    }

    fputs ("}", stdout);
  }
    
  printf ("..%ld]>\t= ", j);

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

#if LLOG
static void
output_item (SXINT item_p, SXBA index_set_p)
{
  SXINT prod, A, top_item, bot_item, i, X;

  prod = PROLIS (item_p);
  A = LHS (prod);

  printf ("[%ld:<%s> = ", prod, NTSTRING (A));

  top_item = PROLON (prod+1)-1;
  bot_item = PROLON (prod);

  while (bot_item < top_item) {
    if (bot_item == item_p)
      fputs (". ", stdout);

    X = LISPRO (bot_item++);

    if (X < 0)
      printf ("\"%s\" ", TSTRING (-X));
    else
      printf ("<%s> ", NTSTRING (X));
  }
    
  if (bot_item == item_p)
    fputs (". ", stdout);

  if (index_set_p) {
    i = sxba_scan (index_set_p,-1);
    printf (", {%ld", i);

    while ((i = sxba_scan (index_set_p, i)) > 0)
      printf (", %ld", i);

    fputs ("}", stdout);
  }
  
  fputs ("]\n", stdout);
}

static void
output_table (SXINT p)
{
  struct recognize_item	*RTp;
  SXINT			A, item_p, *top_ptr, *bot_ptr;
  SXBA                  index_set_p;

  RTp = &(RT [p]);
  printf ("\n************************** T [%ld] **************************\n", p);
  fputs ("************************** SCAN ****************************\n", stdout);
  top_ptr = RTp->shift_NT_hd [0];
  bot_ptr = &(RTp->items_stack [0]);

  while (bot_ptr < top_ptr) {
    item_p = *bot_ptr++;
    index_set_p = RTp->index_sets [item_p];
    output_item (item_p, index_set_p);
  }

  fputs ("************************* SHIFT ****************************\n", stdout);
  for (A = 1; A <= MAXNT; A++) {
    top_ptr = RTp->shift_NT_hd [A];
    bot_ptr = &(RTp->items_stack [0]) + RHS_NT2WHERE (A);

    while (bot_ptr < top_ptr) {
      item_p = *bot_ptr++;
      index_set_p = RTp->index_sets [item_p];
      output_item (item_p, index_set_p);
    }
  }

#if is_parser
  fputs ("************************* REDUCE ***************************\n", stdout);
  for (A = 1; A <= MAXNT; A++) {
    if (top_ptr = PT [p].reduce_NT_hd [A]) {
      bot_ptr = &(RTp->items_stack [LHS_NT2WHERE (A)]);

      while (bot_ptr < top_ptr) {
	item_p = *bot_ptr++;
	index_set_p = RTp->index_sets [item_p];
	output_item (item_p, index_set_p);
      }
    }
  }
#endif /* is_parser */
  printf ("********************** End T [%ld] **************************\n", p);
}
#endif /* LLOG */
#endif /* LOG || EBUG */




#if is_rcvr
/* global recovery */
static VARSTR varstring;

#ifdef MAKE_INSIDEG
static SXBA   rcvr_w2item_set;
#else /* !MAKE_INSIDEG */
static SXBA_CST_ALLOC (rcvr_w2item_set, itemmax+1);
#endif /* !MAKE_INSIDEG */

#if is_parser
/* recovery ds le cas reconnaisseur */
static XxY_header   glbl_dag_rcvr_resume_kitems;
static XxYxZ_header glbl_dag_rcvr_start_kitems;
static SXINT          glbl_dag_rcvr_start_kitems_foreach [] = {0, 0, 0, 0, 1, 0}; /* XZforeach */
static SXBA         glbl_dag_rcvr_start_kitems_set;
static XxYxZ_header store_rcvr_string_hd;
static SXINT          store_rcvr_string_hd_foreach [] = {0, 0, 0, 0, 0, 1}; /* YZforeach */

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
static BOOLEAN    gen_empty_repair_string;
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

static BOOLEAN generate ();

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
   retourne FALSE ssi aucun nouveau range ne peut etre calcule'
*/
static BOOLEAN
range_walk (p, q, r, m, n)
     SXINT p, q, r, *m, *n;
{
  static SXINT delta;
  SXINT      init_state, final_state;
  BOOLEAN    ret_val;

  init_state = idag.init_state;
  final_state = idag.final_state;

  if (q == init_state && r == final_state)
    return FALSE;

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

    ret_val = TRUE;
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
  printf ("**** range_walk = [%ld..%ld] ****\n", *m, *n);
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


/* Nelle version /* 11/12/06
   Test de cycles grace a gen_tntXmaxlgth_repair_hd et gen_tntXmaxlgth_repair_set.  Les elements sont des couples
   (A, max_lgth) ou max_lgth est la taille maximale de la chaine terminale qui doit etre generee par A.
   Si on retombe sur (A, max_lgth) au cours du traitement d'un (A, max_lgth) => cycle (return FALSE) */
/* min_proto est la taille minimale des phrases generees par la protophrase contenue ds gen_proto_stack (on a min_proto <= lgth) */
static BOOLEAN
call_generate (bot_item, top_item, old_prefix, lgth, kind, min_proto)
     SXINT     bot_item, top_item, old_prefix, lgth, kind, min_proto;
{
  SXINT     X, gen_lgth, cur_item;
  BOOLEAN ret_val;

  ret_val = FALSE;

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
	return FALSE;
    }

    cur_item = top_item;

    DCHECK (gen_proto_stack, top_item-cur_item);

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
static BOOLEAN
generate (old_prefix, lgth, kind, min_proto)
     SXINT     old_prefix, lgth, kind, min_proto;
{
  SXINT     X, X_max, new_prefix, item, prod, top_item, bot, top, x, size, parse_nb;
  BOOLEAN is_new, ret_val;

  
  if (IS_EMPTY (gen_proto_stack)) {
    /* fin d'une expansion */
    if (ret_val = (kind != 0 || lgth == 0)) {
      /* Le vide est une bonne chaine */
      /* On ne la met pas ds gen_valid_repair_string_stack car elle pourrait se calculer de plusieurs fac,ons */
      /* ... ou ne pas etre bonne (kind == 0 && lgth > 0) */
      if (old_prefix == 0)
	gen_empty_repair_string = TRUE;

      if (rcvr_spec.perform_repair_parse && perform_copy_local_repair_prod_stack) {
	copy_local_repair_prod_stack (old_prefix);
      }
    }
  }
  else {
    /* gen_proto_stack est non vide */
    ret_val = FALSE;
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

	    ret_val = TRUE;
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
	      ret_val = TRUE;
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


/* si ai ... ap est la chaine generee stckee ds XxY_gen_repair_hd
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
  repair_Tpq2tok_no [single] = put_repair_tok (t, inputG_tstring [t], comment_ste == EMPTY_STE ? NULL : sxstrget (comment_ste), psource_index);

  return -Tpq;
}

/* On instancie le contenu de rhs_stack */
/* Nelle version qui cree une foret */
/* retourne le Aij de la prod creee */
static SXINT
make_repair_prod (prod, i, j, rhs_stack)
     SXINT prod, i, j, *rhs_stack;
{
  SXINT bot_item, top_item, cur_item, A, Aij, Xpq, Pij, RHSij, lgth, init_prod;
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

    /* ... on peut compter les occur de ses nt en rhs */
    if (bot_item < top_item) {
      cur_rhs_stack = rhs_stack;

      for (cur_item = bot_item; cur_item < top_item; cur_item++) {
	Xpq = *cur_rhs_stack++;

#if 0 /* sera fait plus tard */
	if (Xpq > 0)
	  /* xnt */
	  spf.outputG.Aij2rhs_nb [Xpq]++;
#endif /* 0 */
      }
    }
  }
  
  return Aij;
}


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

  if (DTOP (gen_tnt_repair_stack) == 0)
    return;

  tnt = DPOP (gen_tnt_repair_stack);

  if (tnt < 0) {
#if EBUG
    /* repair_source_stack contient la séquence des terminaux constituant le source corrigé généré (le plus à gauche est au sommet)*/
    if (DTOP (repair_source_stack) == 0 || repair_source_stack [DTOP (repair_source_stack)] != -tnt)
      sxtrap (ME, "td_unique_parse");
#endif /* EBUG */
      
    /* scan OK */
    DPUSH (repair_rhs_stack, repair_Tpq [DTOP (repair_source_stack)]);

    /* On "enleve" t des piles */
    DTOP (repair_source_stack)--;
  }
  else {
    /* On a un nt en sommet de pile, on va l'etendre ... */
    prod = POP (prod_stack);

#if EBUG
    if (LHS (prod) != tnt)
      sxtrap (ME, "td_unique_parse");
#endif /* EBUG */

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

    printf ("?/%ld:<%s[%ld::%ld]>\t= ", init_prod, ntstring [A], i, j);

    for (cur = top-1; bot < cur; cur--) {
      Xpq = XH_list_elem (repair_iprod_stack_hd, cur);

      if (Xpq < 0) {
	Xpq = -Xpq;
	X = Tij2T (Xpq);
	p_Xpq = Tij2i (Xpq);
	q_Xpq = Tij2j (Xpq);
	printf ("\"%s\"[%ld::%ld] ", tstring [-X], p_Xpq, q_Xpq);
      }
      else {
	X = Aij2A (Xpq);
	p_Xpq = Aij2i (Xpq);
	q_Xpq = Aij2j (Xpq);

	if (Xpq <= spf.outputG.maxnt)
	  /* On peut avoir des Aii normaux en RHS de prod de la reparation */
	  printf ("<%s[%ld..%ld]> ", ntstring [X], p_Xpq, q_Xpq);
	else
	  printf ("<%s[%ld::%ld]> ", ntstring [X], p_Xpq, q_Xpq);
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
	printf ("\"%s\"[%ld::%ld] ", tstring [-X], p_Xpq, q_Xpq);
      }
      else {  
	X = Aij2A (Xpq);
	p_Xpq = Aij2i (Xpq);
	q_Xpq = Aij2j (Xpq);

	if (Xpq <= spf.outputG.maxnt)
	  /* Il se peut que ce soit un bon (vide) */
	  printf ("<%s[%ld..%ld]> ", ntstring [X], p_Xpq, q_Xpq);
	else
	  printf ("<%s[%ld::%ld]> ", ntstring [X], p_Xpq, q_Xpq);
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
    ste = EMPTY_STE;

  cur_error_string_lbound = lmlstn;
  cur_error_string_rbound = rmlstn;
  cur_repair_string_rbound = repair_source_lgth+1;

  /* Pour avoir une idee du source_index */
  tok_no = idag.pq2tok_no [idag.p2pq_hd [lmlstn]], tok_no = (tok_no > 0) ? tok_no : idag.tok_no_stack [-tok_no+1], source_index = SXGET_TOKEN (tok_no).source_index;

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

#if 0 /* on va faire ce calcul de toutes façons dans put_in_shared_parse_forest ; le faire ici
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
print_gen_string (SXINT symb, BOOLEAN is_last_call)
{
  if (!is_last_call)
    putchar (' ');

  printf ("\"%s\"", TSTRING (symb));
}

static void
get_gen_string (SXINT symb, BOOLEAN is_last_call)
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
  BOOLEAN is_in_source;

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
	  is_in_source = FALSE;

	  while (++t_stack <= t_stack_top) {
	    if (*t_stack == symb) {
	      is_in_source = TRUE;
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
static BOOLEAN
rcvr_gen_dag_paths (SXINT ldag_state, SXINT rdag_state, void (*f)())
{
  SXINT   triple, pq_bot, pq_top;
  BOOLEAN ret_val;

  if (ldag_state >= rdag_state) {
    if (ret_val = (ldag_state == rdag_state))
      if (f) (*f) ();

    return ret_val;
  }

  ret_val = FALSE;

  /* Ds cette vision on met ds repair_path_stack des pq (si on a besoin de recuperer p a partir de pq il faudra rajouter ds idag une correspondance pq --> p) */
  pq_bot = idag.p2pq_hd [ldag_state];
  pq_top = idag.p2pq_hd [ldag_state+1];

  while (pq_bot < pq_top) {
    if (f) PUSH (repair_path_stack, pq_bot);

    if (rcvr_gen_dag_paths (idag.pq2q [pq_bot], rdag_state, f))
      ret_val = TRUE;

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
     BOOLEAN store_rcvr_string;
{
  SXINT                   cur_item, X, top, x, ldag_state, rdag_state, lgth, xbuf, lgth_tried, trans, xtok, triple, min_gen_lgth, gen_lgth, parse_id;
  struct sxsource_coord	lsource_index, rsource_index;
  char                  int_str [12];
  BOOLEAN               has_a_nt, really_store;
    
  ldag_state = mlstn2dag_state (lmlstn);
  rdag_state = mlstn2dag_state (rmlstn);

  varstr_raz (varstring);
  
  if (rcvr_spec.perform_repair) {
    if (!XxY_is_allocated (XxY_gen_repair_hd)) {
      XxY_alloc (&XxY_gen_repair_hd, "XxY_gen_repair_hd", 100, 1, 0, 1, XxY_gen_repair_hd_oflw, NULL);

      if (rcvr_spec.repair_kind != 1) {
	gen_repair2attr = (struct gen_repair2attr_struct*) sxalloc (XxY_size (XxY_gen_repair_hd)+1, sizeof (struct gen_repair2attr_struct));
	repair_gen_nyp = sxba_calloc (XxY_size (XxY_gen_repair_hd)+1);
	repair_path_stack = (SXINT*) sxalloc (idag.final_state+rcvr_tok_no+1, sizeof (SXINT)), repair_path_stack [0] = 0;
	sxba_bag_alloc (&repair_bag, "repair_bag", (idag.final_state+rcvr_tok_no + 1) * NBLONGS (idag.final_state+rcvr_tok_no + 1), statistics);
      }

      DALLOC_STACK (gen_valid_repair_string_stack, 100);
      DALLOC_STACK (gen_tnt_repair_stack, 100);

      if (rcvr_spec.perform_repair_parse) {
	gen_repair2attr2 = (struct gen_repair2attr2_struct*) sxalloc (XxY_size (XxY_gen_repair_hd)+1, sizeof (struct gen_repair2attr2_struct));
	DALLOC_STACK (local_repair_prod_stack, 100);
	DALLOC_STACK (glbl_repair_prod_stacks, 1000);

	XxY_alloc (&repair_forms_hd, "repair_forms_hd", 50, 1, 0, 0, NULL, NULL);
	X_alloc (&repair_Tpq_hd, "repair_Tpq_hd", 100, 1, repair_Tpq_hd_oflw, NULL);
	repair_Tpq2tok_no = (SXINT*) sxalloc (X_size (repair_Tpq_hd)+1, sizeof (SXINT)), repair_Tpq2tok_no [0] = 0;
	XxY_alloc (&repair_Pij_hd, "repair_Pij_hd", 100, 1, 1, 0, repair_Pij_hd_oflw, NULL); /* Xforeach */
	repair_Pij2gen_lgth = (SXINT*) sxalloc (XxY_size (repair_Pij_hd)+1, sizeof (SXINT));
	XH_alloc (&repair_iprod_stack_hd, "repair_iprod_stack_hd", 100, 1, inputG_MAXRHSLGTH+3, NULL, NULL);

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
    perform_copy_local_repair_prod_stack = store_rcvr_string && rcvr_spec.perform_repair && rcvr_spec.perform_repair_parse;

    /* On essaie de generer les chaines de longueur lgth_tried */
    if ((lgth_tried = rmlstn-lmlstn) >= min_gen_lgth && (rcvr_spec.suppression == -1 || rcvr_spec.insertion == -1)) {
      /* gen_proto_stack contient une proto-phrase inverse initialisee a vide */
      gen_empty_repair_string = FALSE, TOP (gen_valid_repair_string_stack) = 0, DTOP (gen_proto_stack) = 0;

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
      gen_empty_repair_string = FALSE, TOP (gen_valid_repair_string_stack) = 0, DTOP (gen_proto_stack) = 0;

      if (!call_generate (litem, ritem, 0 /* old_prefix */, min_gen_lgth, 0 /* kind => |chaines| == lgth */,
			  0 /* taille min des phrases generees par la proto-phrase de gen_proto_stack */))
	sxtrap (ME, "earley_glbl_rcvr_message");

      gen_lgth = min_gen_lgth;
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

    sprintf (int_str, "%ld", lsource_index.line);
    varstring = varstr_catenate (varstring, int_str);
    varstring = varstr_catenate (varstring, " column ");
    sprintf (int_str, "%ld", lsource_index.column);
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
	XxYxZ_set (&store_rcvr_string_hd, EMPTY_STE, lmlstn, rmlstn, &triple);
      }
    }
  }

  has_a_nt = FALSE;

  if (litem < ritem) {
    for (cur_item = litem; cur_item < ritem; cur_item++) {
      if (LISPRO (cur_item) > 0) {
	has_a_nt = TRUE;
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
  
  sxput_error (rsource_index,
	       "%s%s.",
	       sxplocals.sxtables->err_titles [kind],
	       varstr_tostr (varstring));

  /* Analyse de la chaine corrigee */
  if (perform_copy_local_repair_prod_stack && best_repair >= 0) {
    spf.outputG.has_repair = TRUE; /* spf.outputG sert de structure meme si !is_parse_forest */

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
static BOOLEAN
earley_glbl_rcvr_set (SXINT left_mlstn, SXINT left_item, SXINT right_mlstn, SXINT right_item)
{
  SXINT     X, **ptr2, cur_item, resume, triple; 
  SXBA    index_set, left_index_set, backward_index_set;
  BOOLEAN ret_val;

  X = LISPRO (right_item);

#if EBUG
  if (X == 0 || LISPRO (right_item-1) == 0)
    sxtrap (ME, "earley_glbl_rcvr_set");
#endif /* EBUG */
  
  /* Le 12/12/05 */
  /* L'utilisateur peut interdire la suppression */
  if (rcvr_spec.suppression == -1 && left_item == right_item && left_mlstn < right_mlstn)
    /* suppression de left_mlstn .. right_mlstn pour matcher la chaine vide entre left_item && right_item */
    return FALSE;

  /* Il peut ne pas y avoir de chemin ds le dag entre left_mlstn et right_mlstn */

#if LOG
  fputs ("trying ...\t", stdout);
  output_rcvr_prod (left_item, left_mlstn, right_item, right_mlstn);
#endif /* LOG */

  if (ret_val = ((X < 0 && SXBA_bit_is_set (rcvr_w2titem_set, right_item))
		 || (X > 0 && IS_AND (rcvr_w2titem_set, NT2ITEM_SET (X))))) {
    /* La rcvr a ete validee, on stocke les items ds les tables ... */  /* ici right_item est "bon" */
    index_set = T2_index_sets [right_item];

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
      if (index_set == NULL)
	*((*ptr2)++) = right_item;
      /* sinon, il s'y trouve deja (Le 04/10/05) */
    }

    if (index_set == NULL) {
      /* on n'est pas en debut de partie droite, il faut creer un index_set... */
      index_set = T2_index_sets [right_item] = sxba_bag_get (&shift_bag, right_mlstn+1);
#if is_parser
      /* ... et un backward */
      backward_index_set = T2_backward_index_sets [right_item] = sxba_bag_get (&shift_bag, right_mlstn+1);
#endif /* is_parser */
    }
#if is_parser
    else {
      /* \alpha \in NT+ et state a deja ete fabrique par "shift ()". */
      backward_index_set = T2_backward_index_sets [right_item]; 
    }
#endif /* is_parser */
      
    left_index_set = RT [left_mlstn].index_sets [left_item];

    if (left_index_set == NULL) {
      /* left_item est le debut de la rhs !! */
      SXBA_1_bit (index_set, left_mlstn);
    }
    else
      OR (index_set, left_index_set);

#if is_parser
    SXBA_1_bit (backward_index_set, left_mlstn);
#endif /* is_parser */

    SXBA_1_bit (mlstn_active_set, right_mlstn);

    /* On marque la reussite ... */
    XxY_set (&glbl_dag_rcvr_resume_kitems, right_mlstn, right_item, &resume);

#if EBUG
    if (triple = XxYxZ_is_set (&glbl_dag_rcvr_start_kitems, left_mlstn, left_item, resume))
      /* Juste pour savoir si ce cas est possible */
      sxtrap (ME, "earley_glbl_rcvr_set");
#endif /* EBUG */

    XxYxZ_set (&glbl_dag_rcvr_start_kitems, left_mlstn, left_item, resume, &triple);

#if LOG
    fputs ("\n", stdout);
    earley_glbl_rcvr_message (left_item, left_mlstn, right_item, right_mlstn, 1 /* warning */, FALSE);
#endif /* LOG */
  }

#if LOG
  printf (" ..., %s\n", ret_val ? "succeeds\n" : "fails");
#endif /* LOG */

  return ret_val;
}


/* item_p : A -> \alpha . \beta
   On simule une transition entre p et q sur "\beta" */
static BOOLEAN
earley_glbl_rcvr_reduce_pre (SXINT p, SXINT item_p, SXINT q, SXBA rcvr_w2titem_set)
{
  SXINT                   prod, A, order;
  SXBA                  index_set_p;
  BOOLEAN               ret_val;

  /* Les "T2" sont positionnes */
  prod = PROLIS (item_p);
  A = LHS (prod);
  index_set_p = RT [p].index_sets [item_p];

#if !is_parser
#if is_supertagging
  if (!is_during_reduce_rcvr_validation) {
    if (index_set_p == NULL) {
      supertag (prod, p, q);
    }
    else {
      supertag (prod, sxba_scan (index_set_p, 0), q);
    }
  }
#endif /* is_supertagging */

#if is_guide
  if (!is_during_reduce_rcvr_validation) {
    /* Le guide peut etre produit soit par le reconnaisseur (ici) soit par RL_mreduction */
    /* On utilise index_set_p (et non index_set_q) ca peut eviter certains appels (prod, i, j) deja
       effectues lors d'un appel precedent de shift() */
    output_guide (prod, index_set_p, p, q);
  }
#endif /* is_guide */
#endif /* !is_parser */

#ifdef MAKE_INSIDEG
  if (index_set_p == NULL) {
    /* item_q = A -> B .  */
    SXBA_1_bit (ntXindex_set [A], p);
  }
  else
    /* item_q = A -> \alpha \beta . */
    OR (ntXindex_set [A], index_set_p);

  SXBA_1_bit (lhs_order_set, A);
#else /* !MAKE_INSIDEG */
  /* modif du 6/5/2002 */
  order = nt2order [A];

  if (index_set_p == NULL) {
    /* item_q = A -> B .  */
    SXBA_1_bit (ntXindex_set [A], p);
  }
  else
    /* item_q = A -> \alpha \beta . */
    OR (ntXindex_set [A], index_set_p);

  SXBA_1_bit (lhs_order_set, order);
#endif /* !MAKE_INSIDEG */

  scan_reduce (p, q);

  ret_val = rcvr_w2titem_set == NULL
    || T2_shift_NT_hd [0] != NULL
    || sxba_2op (NULL, SXBA_OP_IS, T2_non_kernel_item_set, SXBA_OP_AND, rcvr_w2titem_set);

#if LOG
  if (/* !ret_val || rcvr_w2titem_set == NULL */ !ret_val || is_during_reduce_rcvr_validation /* Le 09/01/07 ?? */) {
    fputs ("trying reduce ...\t", stdout);
    output_rcvr_prod (item_p, p, PROLON (prod+1)-1, q);
    printf (" ..., %s\n", ret_val ? "succeeds" : "fails");
  }

  if (ret_val && !is_during_reduce_rcvr_validation) {
    output_ni_prod (prod, index_set_p, p, q);
  }
#endif /* LOG */

  return ret_val;
}

  /* On ne stocke pas les reduces ds le cas du reconnaisseur. */
  /* Pas de backward apres un terminal... sauf ds le cas is_dag */
static void
earley_glbl_rcvr_reduce_post (p, item_p, q)
     SXINT p, item_p, q;
{
  SXINT  prod, A, item_q, **ptr2, resume, triple;
  SXBA index_set_p, index_set_q, backward_index_set_q;

  prod = PROLIS (item_p);
  A = LHS (prod);
  index_set_p = RT [p].index_sets [item_p];
  item_q = PROLON (prod+1)-1;
  index_set_q = T2_index_sets [item_q];

  if (index_set_q == NULL) {
    if (*(ptr2 = &(T2_reduce_NT_hd [A])) == NULL) {
      /* c'est la 1ere fois qu'on fait un reduce vers A */
      *ptr2 = &(T2_items_stack [LHS_NT2WHERE (A)]);
    }

    *((*ptr2)++) = item_q;
		
    index_set_q = T2_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);

    if (index_set_p == NULL)
      SXBA_1_bit (index_set_q, p);
    else
      /* On ne reutilise pas index_set_p */
      COPY (index_set_q, index_set_p);

#if EBUG
    if (T2_backward_index_sets [item_q] != NULL)
      sxtrap (ME, "earley_glbl_rcvr_shift_reduce_post");
#endif
    backward_index_set_q = T2_backward_index_sets [item_q] = sxba_bag_get (&shift_bag, q+1);
  }
  else {
    if (index_set_p == NULL)
      SXBA_1_bit (index_set_q, p);
    else
      OR (index_set_q, index_set_p);

    backward_index_set_q = T2_backward_index_sets [item_q];

#if EBUG
    if (backward_index_set_q == NULL)
      sxtrap (ME, "earley_glbl_rcvr_shift_reduce_post");
#endif
  }

  SXBA_1_bit (backward_index_set_q, p);

  SXBA_1_bit (mlstn_active_set, q);

  /* On marque la reussite ... */
  XxY_set (&glbl_dag_rcvr_resume_kitems, q, item_q, &resume);

#if EBUG
  if (triple = XxYxZ_is_set (&glbl_dag_rcvr_start_kitems, p, item_p, resume))
    /* Juste pour savoir si ce cas est possible */
    sxtrap (ME, "earley_glbl_rcvr_set");
#endif /* EBUG */

  XxYxZ_set (&glbl_dag_rcvr_start_kitems, p, item_p, resume, &triple);

#if LOG
  earley_glbl_rcvr_message (item_p, p, item_q, q, 1 /* warning */, FALSE);
#endif /* LOG */
}



#ifdef MAKE_INSIDEG
static SXINT            *rcvr_reduce_item_stack;
#else /* !MAKE_INSIDEG */
static SXINT            rcvr_reduce_item_stack[itemmax+1];
#endif /* !MAKE_INSIDEG */

/* Le 26/10/06 : Nelle Nelle version (introduction des TRY_MIN..TRY_MAX) */
/* Le 27/01/06 : Nelle version*/
/* rcvr ds le cas is_dag et !is_dag (is_sdag et normal) */
/* la borne (p=mlstn, q=next_mlstn) est impose' de l'exterieur par la strategie de rattrapage utilisee */
/* On ne traite pas les cas reduce [A -> \alpha ., i] car on a i <= p. Si i==p, le reduce B -> \beta . A \gamma
   a ete ou sera localement, si i < p, ce cas sera traite' par une autre borne */
static SXINT
earley_glbl_rcvr (mlstn, next_mlstn)
     SXINT mlstn, next_mlstn;
{
  SXINT                   top_mlstn, next_dag_state, trans, X, Y, t, A, B, save_try_kind, flip_flop;
  SXINT                   item, prod, top_item, bot_item, cur_item, titem, prev_mlstn, prev_prod;
  SXBA                  lex_compatible_item_set, *index_sets_mlstn, *index_sets_next_mlstn;
  BOOLEAN               local_ret_val = FALSE, call_process_non_kernel_mlstn;
  SXINT                   ret_val = 0;
  struct recognize_item	*RTq;
  SXINT		        *bot_ptr, *top_ptr, *bot_ptr2, *X_top_ptr;
  SXBA                  top_ptr2, source_t_set;
  static SXINT		**shift_NT_hd_mlstn, *items_stack_mlstn;
  static SXBA           rcvr_kernel_item_set;
  static SXBA           rcvr_reduce_item_set;
  static BOOLEAN        working_rcvr_mlstn_tables_must_be_cleared;

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
    varstring = varstr_alloc (128);
    
#if is_parser
    /* Cette structure memorise les couples (mlstn, item) sur lesquels se produisent de la
       recuperation globale */
    XxY_alloc (&glbl_dag_rcvr_resume_kitems, "glbl_dag_rcvr_resume_kitems", MAXITEM+1, 1, 1, 0, NULL, NULL);
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
  printf ("look_ahead_set [%ld] = {", next_mlstn);
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
#if 0
    item = 0; /* Pas de recup sur 0: S' -> . $ S $ */
#endif /* 0 */
    item = 3; /* Pas de recup sur 0: S' -> $ S $ */
    /* Sur un exemple j'ai eu de la recup sur 0: S' -> $ S [3] [4] $ qui a donc supprime' le dernier mot (entre 3 et 4)
       et de la recup S -> \alpha [3] [4] ; qui a supprime' ce dernier mot sur une autre prod */
    /* Finalement, le 09/01/07 on essaie item = 0; si item=3 a echoue' car ca peut etre le dernier espoir!! */

    for (flip_flop = 0; flip_flop < 2; flip_flop++) {
      while ((item = sxba_scan (rcvr_kernel_item_set, item)) >= 0 && (flip_flop == 0 || item < 3)) {
	prod = PROLIS (item);
	top_item = PROLON (prod+1)-1;

	if (item < top_item) {
	  /* Non reduce */
	  if (earley_glbl_rcvr_set (mlstn, item, next_mlstn, item))
	    local_ret_val = TRUE;
	}

	/* Il peut y avoir plusieurs items kernel de la meme prod ...
	   prudence, on ne traite que le + a gauche */
	item = top_item;
      }

      if (!local_ret_val && flip_flop == 0) {
	/* On regarde si on peut se rattraper sur la super_regle */
	item = 0;
      }
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
	    local_ret_val = TRUE;

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
#if 0
    item = 0; /* Pas de recup sur 0: S' -> . $ S $ */
#endif /* 0 */
    item = 3;

    for (flip_flop = 0; flip_flop < 2; flip_flop++) {
      while ((item = sxba_scan (rcvr_kernel_item_set, item)) >= 0) {
	prod = PROLIS (item);
	top_item = PROLON (prod+1)-1;

	for (cur_item = item+1; cur_item < top_item; cur_item++) {
	  if (index_sets_next_mlstn [cur_item]/* index_set */ == NULL) {
	    if (earley_glbl_rcvr_set (mlstn, item, next_mlstn, cur_item)) {
	      local_ret_val = TRUE;

	      if (mlstn == next_mlstn)
		/* Une correction unique (voir pave' du debut) */
		break;
	    }
	  }
	}

	item = top_item;
      }

      if (!local_ret_val && flip_flop == 0) {
	/* On regarde si on peut se rattraper sur la super_regle */
	item = 0;
      }
    }
  }

  if (!local_ret_val || rcvr_spec.try_kind >= TRY_HIGH) {
    /* On parcourt les items reduce du kernel ... */
    if (next_mlstn > mlstn) {
      /* On essaie de se recuperer sur les reduce */
      item = 3; /* Pas de recup sur 0: S' -> $ S $ */

      while ((item = sxba_scan (rcvr_reduce_item_set, item)) >= 0) {
	/* item = [A -> \alpha ., i] \in T[mlstn] */
	prod = PROLIS (item);
	bot_item = PROLON (prod);

	if (item > bot_item /* pas prod vide */ && SXBA_bit_is_set (rcvr_w2item_set, item)) {
	  /* candidat */
	  if (index_sets_next_mlstn [item] == NULL) {
	    /* item jamais essaye' ds next_mlstn */
	    if (RT [working_rcvr_mlstn+1].index_sets [item] == NULL) {
	      /* item jamais essaye' ds les tables de travail */
	      if (!working_rcvr_mlstn_tables_must_be_cleared) {
		/* On travaille donc ds une table speciale qui est vide initialement,  A razer en fin */
		working_rcvr_mlstn_tables_must_be_cleared = TRUE;
		/* Il faut peut-etre faire un set_T2 specifique (on n'a pas besoin de tout!!) */
		set_T2 (working_rcvr_mlstn+1);
		/* petite correction, il faut prendre le look_ahead de next_mlstn */
		T2_look_ahead_t_set = mlstn2look_ahead_t_set [next_mlstn];
	      }

	      is_during_reduce_rcvr_validation = TRUE; /* On previent shift () ... */
	    
	      if (earley_glbl_rcvr_reduce_pre (mlstn, item, next_mlstn, rcvr_w2titem_set)) {
		local_ret_val = TRUE;
		PUSH (rcvr_reduce_item_stack, item);
	      }

	      is_during_reduce_rcvr_validation = FALSE;

	      if (mlstn == next_mlstn && local_ret_val )
		/* Une correction unique (voir pave' du debut) */
		break;
	    }
	  }
	}
      }
    }
  }

  if (!local_ret_val || rcvr_spec.try_kind >= TRY_MAX) {
    /* ... et les items reduce des prods du kernel */

    /* meme si local_ret_val */
    /* On va parcourir les items droits des items non kernel */
    item = 3; /* Pas de recup sur 0: S' -> $ S $ */

    while ((item = sxba_scan (rcvr_kernel_item_set, item)) >= 0) {
      prod = PROLIS (item);
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
	      working_rcvr_mlstn_tables_must_be_cleared = TRUE;
	      /* Il faut peut-etre faire un set_T2 specifique (on n'a pas besoin de tout!!) */
	      set_T2 (working_rcvr_mlstn+1);
	      /* petite correction, il faut prendre le look_ahead de next_mlstn */
	      T2_look_ahead_t_set = mlstn2look_ahead_t_set [next_mlstn];
	    }

	    is_during_reduce_rcvr_validation = TRUE; /* On previent shift () ... */
	    
	    if (earley_glbl_rcvr_reduce_pre (mlstn, item, next_mlstn, rcvr_w2titem_set)) {
	      local_ret_val = TRUE;
	      PUSH (rcvr_reduce_item_stack, item);
	    }

	    is_during_reduce_rcvr_validation = FALSE;

	    if (mlstn == next_mlstn && local_ret_val )
	      /* Une correction unique (voir pave' du debut) */
	      break;
	  }
	}
      }

      item = top_item;
    }
  }

  if (!IS_EMPTY (rcvr_reduce_item_stack)) {
    /* Ca a marche' on le met donc aussi ds les tables de next_mlstn */
    set_T2 (next_mlstn);

    while (!IS_EMPTY (rcvr_reduce_item_stack)) {
      item = POP (rcvr_reduce_item_stack);
      earley_glbl_rcvr_reduce_pre (mlstn, item, next_mlstn, NULL);

#if is_parser
      earley_glbl_rcvr_reduce_post (mlstn, item, next_mlstn);
#endif /* is_parser */
    }
  }

  if (working_rcvr_mlstn_tables_must_be_cleared) {
    working_rcvr_mlstn_tables_must_be_cleared = FALSE;
    clear_table (working_rcvr_mlstn+1);
  }

  if (call_process_non_kernel_mlstn) {
    /* Le corps de ce if n'est fait qu'une fois */
    /* item, l'origine de la rcvr ne fait pas partie de la table T[mlstn], il faut donc l'y mettre
       (avec tous les items non kernel qui l'ont produits)
       on appelle donc process_non_kernel (mlstn) */
    no_la_check = TRUE;
    set_T2 (mlstn);
    no_la_check = FALSE;

    /* le process_non_kernel sur non_kernel_item_sets [next_mlstn] sera fait a l'exterieur */
    /* Il faut enlever les items terminaux, ils ne sont pas bons et ne doivent pas etre stockes */
    sxba_minus (rcvr_non_kernel_item_set, TITEM_SET);
    T2_non_kernel_item_set = rcvr_non_kernel_item_set;
    process_non_kernel_filtering (mlstn);
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

#ifdef MAKE_INSIDEG
  if (spf.insideG.t2item_set == NULL)
    fill_t2item_set (); /* Ds lexicalizer_mngr */

  if (spf.insideG.nt2min_gen_lgth == NULL)
    fill_nt2min_gen_lgth (); /* Ds lexicalizer_mngr */

  /* Le 05/12/06 Allocation dynamique des "SXBA_CST_ALLOC" si MAKE_INSIDEG */
  if (rcvr_non_kernel_item_set == 0) {
    rcvr_non_kernel_item_set = sxba_calloc (MAXITEM+1); /* Duree de vie : reconnaisseur */
    rcvr_w2titem_set = sxba_calloc (MAXITEM+1); /* Duree de vie : reconnaisseur */
    rcvr_w2item_set = sxba_calloc (MAXITEM+1); /* Duree de vie : reconnaisseur */
    rcvr_reduce_item_stack = (SXINT*) sxalloc (MAXITEM+1, sizeof (SXINT)), rcvr_reduce_item_stack [0] = 0; /* Duree de vie : reconnaisseur */
  }
#endif /* MAKE_INSIDEG */

#if is_1la
  if (!is_error_detected) {
    /* 1ere detection d'erreur */
    ilex_compatible_item_set = NULL;
    /* Recalcul du look-ahead au cas ou ... */
    fill_mlstn2look_ahead_item_set (FALSE);
  }
#endif /* is_1la

  /* On previent... */
  is_during_error_rcvr_processing = is_error_detected = TRUE;
  /* ... et on passe immediatement les ensembles de look-ahead au cas non lexicalise' */
  mlstn2look_ahead_t_set = mlstn2non_lexicalized_look_ahead_t_set;

  /* sauvegarde de p car T [p] peut etre raze' par une tentative [r]..[p] puis etre reutilisee par une
     autre tentative [p]..[s] */
  copy_table (working_rcvr_mlstn, p);
  rcvr_detection_table = p;
  rcvr_detection_table_cleared = FALSE;

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
  is_during_error_rcvr_processing = FALSE;

#if LOG
  fputs ("\t*** Leaving Error Recovery ***\n", stdout);
#endif /* LOG */

  return rcvr_q;
}

#endif /* is_rcvr */


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
  printf ("\n****************** Entering merge_table (%ld, %ld) *****************\n", p, q);
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
	if (merge_table_item_set == NULL)
	  merge_table_item_set = sxba_calloc (MAXITEM+1);

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
	if (merge_table_item_set == NULL)
	  merge_table_item_set = sxba_calloc (MAXITEM+1);

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
  printf ("\n****************** Leaving merge_table (%ld, %ld) *****************\n", p, q);
#endif /* LLOG */
}

static BOOLEAN
complete (dag_statep, mlstnp)
     SXINT dag_statep, mlstnp;
{
  SXINT	                dag_stateq, mlstnq, Y, t, trans, trans2;
  SXINT                 bot_pq, top_pq;
  SXINT                 *t_stack, *t_stack_top;
  BOOLEAN               has_reduce, has_changed = FALSE, has_locally_changed, eps_trans_to_final_state, trans_to_final_state;
  SXBA                  index_set;

  /* dag_statep est un dag_state et mlstnp est son milestone associe' */
  /* 0 <= dag_statep <= final_pos+rcvr_tok_no,  0 <= mlstnp <= final_pos+rcvr_tok_no */
  /* Construit (partiellement) les tables Tmlstnq accessibles depuis mlstnp (mlstnq > Tmlstnp).
     Complete la table Tmlstnp dans le cas de productions vides.
     La table Tmlstnp est non vide (elle a des items de la forme A -> \alpha . t \beta)
     Si la table Tmlstnq est non vide, elle aura des items de la forme 
     A' -> \alpha' . t' \beta' \in mlstn2look_ahead_item_set[mlstnq] */

  /* Le look-ahead est verifie pour tous les state de T1_state_set. */
  /* Les predicats associes aux items terminaux (kernel et non-kernel) sont egalement verifies. */
  /* Le look_ahead ai+1 n'est pas verifie. */

  /* ATTENTION si is_epsilon, un meme item peut etre a la fois kernel et non-kernel :
     Soit A -> \alpha . \beta kernel et B -> \gamma . C \delta kernel
     si C =>* A et \alpha =>* \epsilon alors  A -> \alpha . \beta est aussi non-kernel.
     Dans ce cas l'index_set associe a A -> \alpha' X . \beta n'est pas l'index_set de
     A -> \alpha'. X \beta mais une copie afin de permettre la modif ulterieure (ajout du
     numero i de la table locale). */
  T2_non_kernel_item_set = non_kernel_item_sets [mlstnp];

  if (sxba_scan (T2_non_kernel_item_set, -1) >= 0) {
    set_T2 (mlstnp);

    process_non_kernel_filtering (mlstnp);
    process_non_kernel (mlstnp);

    /* Debut essai du 25/09/06 (A TESTER) */
    if (RT [mlstnp].shift_NT_hd [0] == NULL) {
      /* Ici on est su^r qu'il y a une erreur mais la detection courante
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
#if is_rcvr
      if (ilex_compatible_item_set) {
	/* y'a eu du filtrage on va essayer de recommencer sans filtrage */
	ilex_compatible_item_set = NULL;
	/* On recupere donc la valeur initiale ... */
	sxba_copy (T2_non_kernel_item_set, rcvr_store_unfiltered_non_kernel_item_set);
    
	process_non_kernel_filtering (mlstnp);

	if (!is_error_detected)
	  /* On ne [re]regarde que les nouveaux 
	     ca n'a pas ete fait ds process_non_kernel_filtering */
	  sxba_minus (T2_non_kernel_item_set, T2_store_non_kernel_item_set);

	process_non_kernel (mlstnp);
    
	if (RT [mlstnp].shift_NT_hd [0] == NULL)
	  /* Echec */
	  return FALSE;
      }
      else
#endif /* is_rcvr */
	return FALSE;
    }
    /* fIN essai du 25/09/06*/
  }
  else {
#if LLOG
    output_table (mlstnp);
#endif /* LLOG */

    if (RT [mlstnp].shift_NT_hd [0] == NULL)
      return FALSE;
  }

  /* mlstnp est non vide et a des transitions terminales correctes */
  /* Il y a des items de la forme : A -> \alpha . ai \beta */
  if (mlstnp == 0) {
    /* Le traitement d'erreur du SA a pu creer une nouvelle racine au DAG!! */
    mlstnq = idag.init_state;

    set_T2 (mlstnq);

    SXBA_1_bit (pq_t_set, SXEOF);

    if (shift (0, mlstnp, mlstnq, &has_reduce, FALSE /* from_cyclic_stack */)) {
      has_changed = TRUE;
      SXBA_1_bit (mlstn_active_set, mlstnq);
      
      if (has_reduce)
	scan_reduce (mlstnp, mlstnq);
    }
  }
  else {
    /* Le 08/07/05 */
    if (mlstnp == idag.final_state+rcvr_tok_no) {
      /* On est sur l'etat final du dag et il semble que la transition sur le eof final n'y figure pas !! */
      /* On regarde si l'item [0: S' -> $ S . $, 0] est ds la table mlstnp */
      return ((index_set = RT [mlstnp].index_sets [2]) != NULL) && SXBA_bit_is_set (index_set, 0);
    }

    bot_pq = idag.p2pq_hd [dag_statep];
    top_pq = idag.p2pq_hd [dag_statep+1];

    while (bot_pq < top_pq) {
      mlstnq = dag_stateq = idag.pq2q [bot_pq];
      eps_trans_to_final_state = FALSE;
      t_stack = idag.t_stack + idag.pq2t_stack_hd [bot_pq]; /* Pile des terminaux associes aux transitions pq */
      t_stack_top = t_stack + *t_stack;

      sxba_empty (pq_t_set);
    
      while (++t_stack <= t_stack_top) {
	t = *t_stack;
	SXBA_1_bit (pq_t_set, t);

	if (t == inputG_SXEOF) {
	  eps_trans_to_final_state = TRUE;
#if EBUG
	  if (dag_stateq != idag.final_state)
	    sxtrap (ME, "complete");
#endif /* EBUG */
	}
      }

      /* Modif le 18/01/08 ou je deplace (a priori) la "}" qui terminait ici le "if (SXBA_bit_is_reset_set (q_set, dag_stateq)) {" en fin de XxYxZ_Xforeach !! */
      if (eps_trans_to_final_state) {
	/* On merge l'etat mlstnp avec l'etat mlstnq */
	merge_table (mlstnq, mlstnp);
	/* A voir comment repasser de mlstnq a mlstnp lors des phases suivantes ? est-ce suffisant ? */
      }

      /* Ds tous les cas */
      if (sxba_2op (pq_t_set, SXBA_OP_COPY, pq_t_set, SXBA_OP_AND, mlstn2look_ahead_t_set [mlstnp])) {
	/* Si pq_t_set est non vide */
	set_T2 (mlstnq);

	if (shift (0, mlstnp, mlstnq, &has_reduce, FALSE /* from_cyclic_stack */)) {
	  has_changed = TRUE;
	  SXBA_1_bit (mlstn_active_set, mlstnq);
	
	  if (has_reduce)
	    scan_reduce (mlstnp, mlstnq);
	}
      }

      bot_pq++;
    }
  }

  return has_changed; /* TRUE <=> Y'a eu au moins une transition terminale depuis mlstnp (vers une autre table) */
}



#if is_1la
/* Cette procedure remplit mlstn2look_ahead_item_set, le *SXBA qui a chaque mlstn associe l'ensemble des items
   A -> \alpha . \beta t.q. t \in First(\beta) et t est une transition valide'e par la lexicalisation depuis mlstn */
/* Si une erreur a ete detectee, le test du lokk_ahead est inhibee !! */
fill_mlstn2look_ahead_item_set (BOOLEAN is_first_time)
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

/* Cas is_dag */
static BOOLEAN
recognize ()
{
  SXINT		i, j, mlstn, dag_state;
  BOOLEAN	is_in_LG, ret_val;
  SXBA		index_set, t_set;

  pq_t_set = sxba_calloc (SXEOF+1);

  q_set = sxba_calloc (idag.final_state+1+1);

  mlstn_active_set = sxba_calloc (idag.final_state+1+1);
  dead_ends = sxba_calloc (idag.final_state+1+1);

  /* Initialisation : on met le kernel item */
  /* RT [0].items_stack [0] = 0; S' -> . $ S $ */
  RT [0].shift_NT_hd [0] = &(RT [0].items_stack [1]);

  /* On commence par faire une transition sur le $ de [S' -> . $ S $] */
  mlstn = 0;

  do {
    /* On a traite des transitions vers mlstn, il est (peut etre) non vide... */
    dag_state = mlstn2dag_state (mlstn);

    if (complete (dag_state, mlstn)) {
      /* mlstn a des transitions valides */
      mlstn = sxba_scan_reset (mlstn_active_set, mlstn);
    }
    else {
      /* mlstn n'a pas de transition terminale valide... */
      SXBA_1_bit (dead_ends, mlstn);
      
      if ((mlstn = sxba_scan_reset (mlstn_active_set, mlstn)) == -1) {
	/* ... et rien ds le suffixe n'a ete calcule. */
#if is_rcvr
	/* Erreur d'analyse, il faut faire du rattrapage */
	/* Les milestones n'ayant pas de descendents sont ds dead_ends  */
	earley_dag_rcvr ();
	/* si mlstn==-1, le traitement d'erreur n'a rien pu faire... */
	mlstn = sxba_scan_reset (mlstn_active_set, -1);
	/* On ne ressaie que ceux qui ont eu du nouveau */
#else /* !is_rcvr */
	/* ... adieu */
	break;
#endif /* !is_rcvr */
      }
    }
  } while (mlstn >= 0);

  is_in_LG = ((index_set = RT [idag.final_state+rcvr_tok_no].index_sets [2]) != NULL) && SXBA_bit_is_set (index_set, 0);
    
  sxfree (q_set);
  sxfree (mlstn_active_set);
  sxfree (dead_ends);
  sxfree (pq_t_set), pq_t_set = NULL;

  if (merge_table_item_set)
    sxfree (merge_table_item_set), merge_table_item_set = NULL;

  if (!is_in_LG) {
#if is_rcvr
    /* Normalement, on ne passe jamais la ... */
    /* ... ben si !! */
    /* Le 11/07/05 : il semble que c,a  a ete corrige' et que de nouveau on n'y passe plus, a voir !! */
    /* Parsing stops on end of file */
    sxput_error (sxsvar.sxlv.terminal_token.source_index,
		 rcvr_eof /* "%sParsing stops on end of file." */,
		 sxplocals.sxtables->err_titles [2]);
#else /* !is_rcvr */
    /* Ca permet aussi de sortir : exit () de syntax avec le bon code */
    sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
	    "%sSyntax Error.",
	    sxplocals.sxtables->err_titles [2]);
#endif /* is_rcvr */
  }
    
  ret_val = is_in_LG;

#if is_rcvr
  if (is_error_detected)
    is_in_LG = FALSE;
#endif /* is_rcvr */  

  if (is_print_time) {
    if (is_in_LG)
      sxtime (TIME_FINAL, "\tDAG Earley Recognizer (TRUE)");
    else
      sxtime (TIME_FINAL, "\tDAG Earley Recognizer (FALSE)");
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
   Si pass_inherited retourne FALSE, le sous_arbre n'est pas visite'.
   Si pass_derived retourne FALSE, le sous_arbre est revisite'.

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
   Elle appelle (*spf.dag.pass_derived) (hook) ou hook est l'indice de la Aij_prod visitee
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
	fprintf (out_file, "%ld", -i);
    }
    else
      fprintf (out_file, "%ld", i);

    if (i <= 0 || j <= 0 || i > idag.final_state || j > idag.final_state || i == j)
      fputs ("::", out_file);
    else
      fputs ("..", out_file);

    if (j <= 0) {
      if (j == 0)
	fputs ("*] ", out_file);
      else
	fprintf (out_file, "%ld] ", -j);
    }
    else
      fprintf (out_file, "%ld] ", j);
#else /* !is_rcvr */
    fprintf (out_file, "\"%s\"[%ld..%ld] ", t, i, j);
#endif /* !is_rcvr */
  }
  else {
#if is_rcvr
    fprintf (out_file, "<%s[", ntstring [symb]);

    if (i <= 0) {
      if (i == 0)
	fputs ("*", out_file);
      else
	fprintf (out_file, "%ld", -i);
    }
    else
      fprintf (out_file, "%ld", i);

    if (i <= 0 || j <= 0 || i > idag.final_state || j > idag.final_state)
      fputs ("::", out_file);
    else
      fputs ("..", out_file);

    if (j <= 0) {
      if (j == 0)
	fputs ("*]> ", out_file);
      else
	fprintf (out_file, "%ld]> ", -j);
    }
    else
      fprintf (out_file, "%ld]> ", j);
#else /* !is_rcvr */
    fprintf (out_file, "<%s[%ld..%ld]> ", ntstring [symb], i, j);
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
  BOOLEAN          has_error;

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
    has_error = FALSE;
    bot_rhs_stack = rhs_stack;
    bot_bot_item = bot_item;

    while (bot_item < top_item) {
      X = LISPRO (bot_item++);
      h = *rhs_stack++;
      k = *rhs_stack++;

      if (h < -idag.final_state) {
	/* repair */
	has_error = TRUE;
	/* cas particulier ou k est lui-meme le symbole deja instancie' */
	Aij = k;
	X = 0;
      }
      else {
	if  (!is_rcvr_can_be_cyclic && (h < 0 || k < 0))
	  has_error = TRUE;
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

      if (bot_bot_item > top_item)
	is_rcvr_can_be_cyclic = TRUE;
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



static void
RL_mreduction_item (item_j, I, j)
     SXINT 	item_j, j;
     SXBA	I;
{
  /* item_j = A -> \alpha . \gamma  et \alpha != \epsilon */
  SXINT				Y, bot_item, i, k, Z, new_k, nbt, new_item, item_k, item, order, new_item_k, t, cur_prod;
  SXBA			        backward_index_set, prev_index_set, index_set;
  SXBA			        /* tbp_set, */ ap_set;
  struct recognize_item	        *RTj;
  struct parse_item		*PTj;
  struct PT2_item		*PT2j;
  BOOLEAN                       is_ap_set_empty, is_rcvr_trans;

  SXBA			        ids, nbis;
  SXINT				*top_ptr, *bot_ptr;
#if is_rcvr
  SXINT                         resume, triple, kept_triple, top_item;
  BOOLEAN                       is_item_j_a_glbl_rcvr_kernel_item;
#endif /* is_rcvr */

  cur_prod = PROLIS (item_j);

#if LLOG
  fprintf(stdout,"RL_mreduction_item(%ld,,%ld):\t", item_j, j);
  output_item (item_j, I);
#endif /* LLOG */

#if is_rcvr
  is_item_j_a_glbl_rcvr_kernel_item = is_error_detected && XxY_is_allocated (glbl_dag_rcvr_resume_kitems) &&
    ((resume = XxY_is_set (&glbl_dag_rcvr_resume_kitems, j, item_j)) > 0);

  top_item = PROLON (cur_prod+1)-1;
#endif /* is_rcvr */

  bot_item = PROLON (cur_prod);

  PTj = &(PT [j]);
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

  nbis = PT2j->backward_index_sets [item_j];

  is_ap_set_empty = TRUE;

  if (item_k == bot_item
#if is_rcvr
      && !is_item_j_a_glbl_rcvr_kernel_item
#endif /* is_rcvr */
      ) {
    /* \alpha = \epsilon */
    if (ap_set /* Le 15/07/2003 */) {
      /* Le 20/10/05 Il se peut que |ap_set| < |I| suite au changement de taille au cours d'un appel pre'ce'dent de RL_mreduction_item
	 (voir la fin de la procedure)
	 On pourrait plutot ne pas changer de taille (a voir) */
      if (*I > *ap_set)
	*ap_set = *I;
    }

    /* On peut avoir de la rcvr de type A -> [i] [k] Y ... */
    /* Le backward sur Y contient k alors que I contient i !! */
#if is_rcvr
    if (is_error_detected) {
      /* backward peut etre different de index_set !! */
      if (nbis == NULL)
	/* 1er coup */
	nbis = sxba_bag_get (&pt2_bag, j+1);
    }
    else
#endif /* is_rcvr */
      nbis = PT2j->index_sets [item_j]; /* backward et index_sets sont et resteront identiques */

    PT2j->backward_index_sets [item_j] = nbis;

#if is_rcvr
    if (is_error_detected) {
      backward_index_set = PTj->backward_index_sets [item_j];

#if EBUG
      if (backward_index_set == NULL)
	sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */

      /* attention, on peut etre ds le cas A -> [i] [k] Y [j]
	 et le ap_set [Y] doit contenir k et pas i */

      k = -1;

      while ((k = sxba_scan (backward_index_set, k)) >= 0) {
	is_item_j_a_glbl_rcvr_kernel_item /* bien qu'il concerne ici item_k... */ = XxY_is_allocated (glbl_dag_rcvr_resume_kitems) &&
	  ((resume = XxY_is_set (&glbl_dag_rcvr_resume_kitems, k, bot_item)) > 0);

	if (is_item_j_a_glbl_rcvr_kernel_item) {
	  i = -1;

	  while ((i = sxba_scan (I, i)) >= 0) {
	    if ((triple = XxYxZ_is_set (&glbl_dag_rcvr_start_kitems, i, bot_item, resume)) > 0) {
#if EBUG
	      if (i >= k)
		sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */

#if is_parser && !is_guide && !is_supertagging
	      /* rcvr valide', on le note */
	      SXBA_1_bit (glbl_dag_rcvr_start_kitems_set, triple);
#endif /* is_parser && !is_guide && !is_supertagging */

#if LOG
	      earley_glbl_rcvr_message (bot_item, i, bot_item, k, 1 /* warning */, FALSE);
#endif /* LOG */

	      SXBA_1_bit (nbis, k);

	      if (ap_set) {
		/* On traite le [k] Y [j] de A -> [i] [k] Y [j] */
		if (SXBA_bit_is_reset_set (ap_set, k)) {
		  /* A change' */
		  is_ap_set_empty = FALSE;
		  SXBA_1_bit (RL_nt_set, Y);
		}
	      }
	    }
	    else {
	      if (i == k) {
		SXBA_1_bit (nbis, i);

		if (ap_set) {
		  if (SXBA_bit_is_reset_set (ap_set, k)) {
		    /* A change' */
		    is_ap_set_empty = FALSE;
		    SXBA_1_bit (RL_nt_set, Y);
		  }
		}
	      }
	    }
	  }
	}
	else {
	  if (SXBA_bit_is_set (I, k)) {
	    SXBA_1_bit (nbis, k);

	    if (ap_set) {
	      if (SXBA_bit_is_reset_set (ap_set, k)) {
		/* A change' */
		is_ap_set_empty = FALSE;
		SXBA_1_bit (RL_nt_set, Y);
	      }
	    }
	  }
	}
      }
    }
    else
#endif /* is_rcvr */
      /* Pas d'erreur, cas normal */
      if (ap_set) {
	if (OR (ap_set, I)) {
	  /* A change' */
	  is_ap_set_empty = FALSE;
	  SXBA_1_bit (RL_nt_set, Y);
	}
      }
  }
  else {
    nbt = 0; /* Le 13/02/08 */
    backward_index_set = PTj->backward_index_sets [item_j];

#if EBUG
    if (backward_index_set == NULL)
      sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */
      
    /* Ajoute' le 17/11/04 */
    if (nbis == NULL)
      nbis = PT2j->backward_index_sets [item_j] = sxba_bag_get (&pt2_bag, j+1);
	    
    /* nelle version de la boucle sur les backward */
    k = -1;

    while ((k = sxba_scan (backward_index_set, k)) >= 0) {
#if LLOG
      fprintf(stdout,"\t<< %ld : ",k);
      output_item(item_k,RT [k].index_sets [item_k]);
#endif /* LLOG */

      is_rcvr_trans = FALSE;

#if is_rcvr
      /* Ds la cas d'erreur sur des symboles multiples "X1 X2 ... Xp", il faut aussi essayer du traitement
	 normal sur Xp avec le meme k */
      if (is_item_j_a_glbl_rcvr_kernel_item) {
	/* item_j est un item de la recuperation... */
	/* ...ca se complique */
	/* On a
	   item_j = A -> \alpha B \beta Y . \gamma
	   item_k = A -> \alpha B \beta . Y \gamma
	   mais la recuperation a ete lancee depuis l'item A -> \alpha B . \beta Y \gamma
	   qui est (peut-etre) different de item_k, il faut le retrouver...
	*/
	new_item = item_j+1;
	kept_triple = 0;
	item = 0;

	XxYxZ_XZforeach (glbl_dag_rcvr_start_kitems, k, resume, triple) {
#if 0
	  /* C'est faux, il peut y en avoir plusieurs !! */
#if EBUG
	  if (item != 0)
	    /* au plus un ... */
	    sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */
#endif /* 0 */

	  item = XxYxZ_Y (glbl_dag_rcvr_start_kitems, triple);

	  if (item < new_item) {
	    /* Je ne conserve que le + a gauche!! */
	    /* ... s'il marche */
	    if (item == bot_item) {
	      if (SXBA_bit_is_set (I, k)) {
		/* il est bon */
		new_item = item;
		kept_triple = triple;
	      }
	    }
	    else {
	      prev_index_set = RT [k].index_sets [item];

	      if (prev_index_set && IS_AND (prev_index_set, I)) {
		/* il est bon */
		new_item = item;
		kept_triple = triple;
	      }
	    }
	  }
	}

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
	if (item != 0) {
	  /* il y a eu de la recuperation entre k et j */
	  if (kept_triple /* une recup d'erreur a ete validee */
#if 0 /* if 0 car un premier passage peut être destiné à échouer, puis le même rattrapage peut être
	 re-réclamé à un endroit où il est nécessaire et où il marchera */
	      && SXBA_bit_is_reset_set (global_rcvr_valid_left_mlstn_set, k) /* On ne valide qu'une seule recup par mlstn */
#endif /* 0 */
	      ) {
#if EBUG
	    if (new_item > item_j)
	      sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */

#if is_parser && !is_guide && !is_supertagging
	    /* Le 04/11/05 */
	    /* rcvr valide', on le note */
	    SXBA_1_bit (glbl_dag_rcvr_start_kitems_set, kept_triple);
#endif /* is_parser && !is_guide && !is_supertagging */

#if LOG
	    earley_glbl_rcvr_message (new_item, k, item_j, j, 1 /* warning */, FALSE);
#endif /* LOG */

	    /* Attention : Si la recuperation se fait par transition sur l'axiome 1 ( 0: 0 -> $ 1 $), il n'y aura
	       pas de "production" reconnue par le parseur. Ds ce cas il faut trouver un moyen, ds la phase parser,
	       de sortir le message d'erreur disant qu'on n'a rien reconnu (A FAIRE) */
	    /* Le 11/08/04 pour l'instant, on le note !! */
	    if (item_j <= 2) {
	      PUSH (rcvr_on_super_rule_stack, kept_triple);
	      PUSH (rcvr_on_super_rule_stack, new_item);
	      PUSH (rcvr_on_super_rule_stack, k);
	      PUSH (rcvr_on_super_rule_stack, item_j);
	      PUSH (rcvr_on_super_rule_stack, j);
	    }

#if 0
	    /* Le 13/02/08 */
#if !is_dag
	    nbt = (is_error_detected) ? 0 : item2nbt [new_item];
#endif /* !is_dag */
#endif /* 0 */

	    if (new_item == bot_item+nbt) {
	      /* ... on a atteint la pos initiale */
	      SXBA_1_bit (nbis, k);
	      continue; /* prochain k */
	    }

	    /* ... sinon, on continue en sequence */
	    prev_index_set = RT [k].index_sets [new_item];

	    new_item -= nbt;
	    /* new_item > bot_item */

	    is_rcvr_trans = TRUE;
	  }
	  else {
	    /* recup non impliquee ds le thread courant (on saute k) */
	    continue;
	  }
	}
      }
#endif /* is_rcvr */
	
      /* Nelle version le 13/01/06 */
      if (!is_rcvr_trans) {
	new_item = item_k - nbt;

	if (new_item == bot_item) {
	  SXINT h = k-nbt;
	      
	  if (!SXBA_bit_is_set (I, h))
	    /* Pas un bon k */
	    continue;
	}
	else {
	  /* normalement RT [k].index_sets [item_k] == RT [k-nbt].index_sets [new_item] */
	  prev_index_set = RT [k].index_sets [item_k];

#if EBUG
	if (prev_index_set == NULL)
	  sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */	

	if (!IS_AND (prev_index_set, I))
	  /* C'est pas un bon k */
	  continue;
	}
      }

      /* C'est un bon k */
      /* On ne doit pas le supprimer, il peut servir + tard. */
      SXBA_1_bit (nbis, k);

      if (ap_set && !is_rcvr_trans /* && is_new || !SXBA_bit_is_set (ap_set, k) le 08/07/2003 */) {
	if (SXBA_bit_is_reset_set (ap_set, k)) {
	  is_ap_set_empty = FALSE;
	  SXBA_1_bit (RL_nt_set, Y);
	}
      }

      /* a cause de nbt on peut avoir ici new_item == bot_item */
      if (new_item > bot_item) {
	/* new_item pointe derriere un nt... */
	/* ... sauf peut etre ds le cas is_dag */
	new_k = k-nbt;

#if 0
	/* Le 13/02/08 */
#if !is_dag
	/* Essai le 29/11/05 */
	if (nbt) {
	  /* On saute les terminaux */
	  prev_index_set = RT [new_k].index_sets [new_item];

#if EBUG
	  if (prev_index_set == NULL|| !IS_AND (prev_index_set, I))
	    sxtrap (ME, "RL_mreduction_item");
#endif /* EBUG */
	}
#endif /* !is_dag */
#endif /* 0 */	

#if is_epsilon || is_rcvr
	/* Il se peut que new_k == j ds le cas de recuperation, meme si is_epsilon==0 */
	if (new_k == j) {
	  if ((ids = PT2j->index_sets [new_item]) == NULL)
	    ids = PT2j->index_sets [new_item] = sxba_bag_get (&pt2_bag, j+1);

	  if (OR_AND (ids, prev_index_set, I)) {
	    /* Il y a du nouveau */
	    RL_mreduction_item (new_item, ids, j);
	  }
	}
	else
#endif /* is_epsilon || is_rcvr */
	  {
#if is_rcvr
	    if (is_rcvr_trans && (/* cas "X1 ... Xp [k] [j>k]" */ new_item == top_item 
		 || /* on est sur la super-règle dans un etat "$ AXIOME [k] [j>k] $" */ (new_item == 2 && item_j == 2))
		) {
	      /* Le 20/11/06 Si on est ds le cas
		 242:<GN> = <ADJANTE___c> <ADJ_PREF___STAR> "nc" [5] [6] ; (j==6)
		 Il ne faut pas positionner PT2 [5].index_sets [<GN> = <ADJANTE___c> <ADJ_PREF___STAR> "nc" .]
	      */
	      /* Le 09/01/07 mais il faut quand meme continuer la reconnaissance de 242 et il faut que la lhs instanciee
		 soit de la forme <GN[i..6]> (et non <GN[i..5]>) */
	      if ((ids = PT2 [new_k].index_sets [new_item]) == NULL) {
		ids = PT2 [new_k].index_sets [new_item] = sxba_bag_get (&pt2_bag, *prev_index_set /* Pour "OR_AND" le 19/10/05 a la place de new_k+1 */);
		/* Dans ce cas tordu, on restera en position reduce apre`s avoir recule' dans le source (!!!),
		   on doit le dire, en passant new_item au lieu de new_item, 
		   car si au moins une re`gle sera dans cette position reduce de fac,on normale (sans rcvr),
		   et si on vient en premier sur la re`gle issue du rcvr, red_order_sets [new_k] ne sera pas positionné,
		   mais la re`gle "normale" ne fera plus rien */
		PUSH (PT2 [new_k].shift_NT_stack, -new_item);
	      }

	      OR_AND (ids, prev_index_set, I);
	    }
	    else
#endif /* is_rcvr */
	      {
		if ((ids = PT2 [new_k].index_sets [new_item]) == NULL) {
		  ids = PT2 [new_k].index_sets [new_item] = sxba_bag_get (&pt2_bag, *prev_index_set /* Pour "OR_AND" le 19/10/05 a la place de new_k+1 */);
		  PUSH (PT2 [new_k].shift_NT_stack, new_item);
		}
	    
		OR_AND (ids, prev_index_set, I);
	      }
	  }
      }

#if is_supertagging
      /* A VOIR DS LE CAS is_dag... et is_rcvr... */
      if (nbt) {
	for (new_k = k-nbt, new_item_k = new_item; new_k < k; new_k++, new_item_k++) {
	  /* On est en train d'examiner la reduction prod = PROLIS (new_item_k).
	     Ses &Lex ont ete verifies ds recognize () */
	  call_supertagger (new_item_k, new_k);
	}
      }
#endif /* is_supertagging */
    }
  }

  if (ap_set  && !is_ap_set_empty /* garantit qu'au moins une des trans pour Y[..j] n'est pas du rcvr */
      /* && SXBA_bit_is_reset_set (RL_nt_set, Y) /* Le 21/07/2003 */) {
    /* Le 22/08/05 le test SXBA_bit_is_reset_set (RL_nt_set, Y) est supprime' car on peut etre sur une
       autre occur de Y que la 1ere */

    /* On récupère les productions instanciées définissant le non-terminal étudié (à savoir Y[k..j]),
     où k est l'un des backwards parmi ceux qui ont été validés, et on les met dans prod_order_set pour
    examen ultérieur. Ceci n'est nécessaire que si l'exécution courante de RL_mreduction_item a rajouté
    de nouveaux k (backwards validés) dans l'ap_set  */

    ap_set [0] = j+1; /* On change la taille!! */

    RTj = &(RT [j]);

    top_ptr = PTj->reduce_NT_hd [Y];
#if is_rcvr
    /* Le 22/06/04 */
    /* Si la transition Y a ete simulee par le rcvr, top_ptr peut etre vide!! */
    if (is_error_detected && top_ptr == NULL)
      return;
#endif /* is_rcvr */

#if EBUG
    if (top_ptr == NULL)
      sxtrap (ME, "RL_mreduction_item");
#endif
    bot_ptr = &(RTj->items_stack [LHS_NT2WHERE (Y)]);

    while (--top_ptr >= bot_ptr) {
      item = *top_ptr;
#if EBUG
      if (item < 0)
	sxtrap (ME, "RL_mreduction_item");
#endif

      cur_prod = PROLIS (item);

#ifdef MAKE_INSIDEG
      /* On y met directement une prod car prod2order n'est pas calcule' dynamiquement par lexicalizer_mngr */
      SXBA_1_bit (prod_order_set, cur_prod);
#else /* !MAKE_INSIDEG */
      order = prod2order [cur_prod];
      SXBA_1_bit (prod_order_set, order);
#endif /* !MAKE_INSIDEG */
    }
  }
}


static void
make_axiom ()
{
  SXINT     i, j, j_prime, k, resume, Aij;
  SXBA    backward_index_set;
  BOOLEAN is_1_a_glbl_rcvr_kernel_item, is_2_a_glbl_rcvr_kernel_item, triple;

#if is_rcvr
  if (TRUE) {
    if (!is_error_detected)
      MAKE_Aij (Aij, 1, 1, n+1);
    else {
      /* On va examiner la super regle */
      /* elle peut avoir plusieurs formes du type
	 0 -> $ [1] [j] S [k] [n+1] $ */

      /* 1. Calcul de k */
      backward_index_set = PT [n+1].backward_index_sets [2];
      k = sxba_1_rlscan (backward_index_set, n+1);

      is_2_a_glbl_rcvr_kernel_item = XxY_is_allocated (glbl_dag_rcvr_resume_kitems) &&
	((resume = XxY_is_set (&glbl_dag_rcvr_resume_kitems, n+1, 2)) > 0);
      
      if (is_2_a_glbl_rcvr_kernel_item) {
	triple = XxYxZ_is_set (&glbl_dag_rcvr_start_kitems, k, 2 /* meme_item */, resume);
	if (triple) { /* si erreur de type S [k<n+1] [n+1] */
	  /* on n'a pas franchi S, on est sur une erreur (on est toujours sur l'item 2): k != n+1 */
	  j = sxba_scan (backward_index_set, 0);
	  if (j < k) {
	    /* on (suppose qu'on) ne peut pas avoir à la fois "0 -> $ ... S [j] [n+1]" $ et "0 -> $ ... S [k] [n+1] $"  */
	    /* donc j ne correspond qu'à l'item 1 (il est avant S, et jamais après)
	       donc S[j..n+1] "existe", et aucun autre S[j<x<k,n+1] n'existe
	       mais aussi par ailleurs S[j..k] "existe", et on veut le virer pour ne conserver que S[j..n+1], ce qui
	       impose d'inhiber le rattrapage [j] S [k] [n+1] ;
	    */
	    SXBA_0_bit (backward_index_set, k);
	    XxYxZ_erase (glbl_dag_rcvr_start_kitems, triple);
	    k = n+1;
	  }
	  else {
	    /* supposons que ce backward unique ne correspond qu'à l'item 2 */
	    /* k est correct, il faudra trouver j */
	  }
	} 
	else
	  k = n+1;
      }
      else
	k = n+1;


      /* 2. Calcul de j */
      backward_index_set = PT [k].backward_index_sets [2];
      /* dans un premier temps, on cherche à savoir s'il y a eu suppression de matériau 
	 à gauche du S qu'on va retenir */
      j = sxba_1_rlscan (backward_index_set, k);

      is_1_a_glbl_rcvr_kernel_item = (j > 1);

      if (is_1_a_glbl_rcvr_kernel_item) {
	resume = XxY_is_set (&glbl_dag_rcvr_resume_kitems, j, 1);
	triple = XxYxZ_is_set (&glbl_dag_rcvr_start_kitems, 1, 1 /* meme_item */, resume);
#if EBUG
	if (triple == 0) {
	  sxtrap (ME, "make_axiom (the expected deletion between 1 and j doesn't correspond to a found rcvr)");
	}
#endif /* EBUG */
	/* erreur de type [1] [j] S */
	/* On sait que j est sur l'item 1 */
	j_prime = sxba_scan (backward_index_set, k);
#if EBUG
	if (j_prime < j && j_prime > 1) {
	  sxtrap (ME, "make_axiom (two concurrent deletions before the axiom seem to have been found, which is impossible)");
	}
#endif /* EBUG */
	if (j_prime == 1) {
	  SXBA_0_bit (backward_index_set, 1);
	  XxYxZ_erase (glbl_dag_rcvr_start_kitems, triple);
	  j = 1;
	}
	else {
	  /* j_prime == j, et c'est le bon j */
	}
      }
      else
	j = 1;
      
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
RL_mreduction ()
{
  SXINT				i, j, A, init_A, order, tnb, A0j, Aij, t;
  SXBA			        ap_set, new_item_set;
  struct parse_item		*PTj;
  struct recognize_item	        *RTj;
  struct PT2_item		*PT2j;

  SXINT				*PT2j_shift_NT_stack, x, item, new_item, new_j, prod, X;
  SXBA			        *PT2j_index_set, *PT2j_backward_index_set, I, B, ids;
  SXBA			        index_set, new_index_set, red_order_set, wis;
#if is_rcvr
  SXBA                          tmp_rcvr_order_set;
#endif /* is_rcvr */
  BOOLEAN			is_good;
#if is_right_recursive
  SXINT				maxo, new_order;
  BOOLEAN			must_loop;
#endif /* is_right_recursive */

  /* ESSAI !! */
  n += rcvr_tok_no-1;

  make_axiom (); /* Le 16/02/06 */

  RL_nt_set = sxba_calloc (MAXNT+1);
  prod_order_set = sxba_calloc (MAXPROD+1);

  if (is_parse_forest
#if is_guide || is_supertagging || EBUG
      || TRUE
#endif /* is_guide || is_supertagging || EBUG */
      )
    new_item_set = sxba_calloc(MAXITEM+1);

  wis = sxba_calloc (n+1+1);
#if is_rcvr
  tmp_rcvr_order_set = is_error_detected ? sxba_calloc (MAXPROD+1) : NULL;
#endif /* is_rcvr */
	
  index_set = PT2 [n+1].index_sets [2] = sxba_bag_get (&pt2_bag, n+1+1);
  SXBA_1_bit (index_set, 0); /* {0} */

  PUSH (PT2 [n+1].shift_NT_stack, 2); /* S' -> $ S . $ */

  for (j = n+1; j >= 0; j--) {
    PT2j = &(PT2 [j]);
    PTj = &(PT [j]);
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
#ifndef MAKE_INSIDEG
	prod = prod2order [prod];
#endif /* !MAKE_INSIDEG */
	SXBA_1_bit (tmp_rcvr_order_set, prod);
      }
#endif /* is_rcvr */

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
#endif
#endif /* 0 */

      RL_mreduction_item (item, I, j);
    }

#if is_right_recursive
    must_loop = FALSE;
#endif /* is_right_recursive */
    red_order_set = red_order_sets [j];

    /* prod_order_set est positionne par RL_mreduction_item par le traitement des shifts. */
    if ((order = sxba_scan_reset (prod_order_set, 0)) > 0) {
      /* Boucle infinie ca permet des break et de reexaminer les cas ou l'ordre d'execution des prod est non garanti */
#ifdef MAKE_INSIDEG
      for (;;) {
#else /* !MAKE_INSIDEG */
#if is_rcvr
      for (;;) {
#endif /* is_rcvr */
#endif /* !MAKE_INSIDEG */
	do {
#ifdef MAKE_INSIDEG
	  /* prod_order_set contient directement une prod car order2prod n'est pas calcule' dynamiquement par lexicalizer_mngr */
	  prod = order;
#else /* !MAKE_INSIDEG */
	  prod = order2prod [order];
#endif /* !MAKE_INSIDEG */

	  item = PROLON (prod+1)-1;
	  A = LHS (prod);
	  tnb = 0;

#if 0
	  /* Le 13/02/08 */
#if is_dag
	  tnb = 0;
#else /* !is_dag */
#if is_rcvr
	  tnb = (is_error_detected) ? 0 : item2nbt [item];
#else /* !is_rcvr */
	  tnb = item2nbt [item];
#endif /* !is_rcvr */
#endif /* !is_dag */
#endif /* 0 */

	  ap_set = ap_sets [A]; /* prod est une Akj-prod avec k \in ap_set */

	  is_good = FALSE;
				
	  if (tnb == 0) {
#if is_epsilon
	    if (item == PROLON (prod)) {
	      /* item = A -> . */
	      if (SXBA_bit_is_set (ap_set, j)) {
		/* Le 22/02/06, on ne fait TOUTE la suite que si le test est avere */
		is_good = TRUE;

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
	    else
#endif /* is_epsilon */
	      {
		/* item = A -> \alpha X . */
		index_set = RTj->index_sets [item];

		if ((new_index_set = PT2j->index_sets [item]) == NULL) {
		  new_index_set = sxba_bag_get (&pt2_bag, j+1);
		}

		if (OR_AND_MINUS (new_index_set, index_set, ap_set, wis)) {
		  /* calcule wis = (index_set & ap_set) - new_index_set
		     et new_index_set |= wis
		     retourne vrai ssi wis est non vide 
		     wis est l'ensemble des k valides pour prod et non encore traites */

		  /* Il y a du nouveau */
		  /* Le 19/09/2002 : on [re]affecte new_index_set a PT2j->index_sets [item], ca
		     permet de ne pas avoir des index_set non NULL et vides */
		  PT2j->index_sets [item] = new_index_set;

		  /* MODIF: c'est wis qui decide l'arret des boucles */
		  RL_mreduction_item (item, wis, j);
		  is_good = TRUE;

		  if (is_parse_forest
#if is_guide || is_supertagging || EBUG
		      || TRUE
#endif /* is_guide || is_supertagging || EBUG */
		      )
		    SXBA_1_bit (new_item_set, item);
		}
	      }
	  }
	  else {
#if is_supertagging
	    for (new_j = j-tnb, new_item = item-tnb; new_j < j; new_j++, new_item++) {
	      /* On est en train d'examiner la reduction prod = PROLIS (new_item).
		 Ses &Lex ont ete verifies ds recognize () */
	      call_supertagger (new_item, new_j);
	    }
#endif /* is_supertagging */

	    new_j = j - tnb;

	    if (PROLON (prod) == (new_item = item - tnb)) {
	      /* Que des t en rhs */
	      if (SXBA_bit_is_set (ap_set, new_j))
		is_good = TRUE;

	      /* Le 08/02/06 */
	      /* Majorant du nb de Aij */ 
#ifdef MAKE_INSIDEG
	      init_A = spf.insideG.nt2init_nt [A];
#else /* !MAKE_INSIDEG */
	      init_A = A;
#endif /* !MAKE_INSIDEG */

	      MAKE_Aij (Aij, init_A, new_j, j);

#if LOG
	      output_ni_prod (prod, NULL, new_j, j);
#endif /* LOG */

#if is_guide
	      output_guide (prod, NULL, new_j, j);
#endif /* is_guide */
	    }
	    else {
	      /* new_item = A -> \alpha B . t1 ... tnbt */
	      index_set = RT [new_j].index_sets [new_item];

	      if (IS_AND (index_set, ap_set)) {
		if ((ids = PT2 [new_j].index_sets [new_item]) == NULL) {
		  ids = PT2 [new_j].index_sets [new_item] = sxba_bag_get (&pt2_bag, new_j+1);
		  PUSH (PT2 [new_j].shift_NT_stack, new_item);
		}

		if (OR_AND (ids, index_set, ap_set)) {
		  is_good = TRUE;

		  if (is_parse_forest
#if is_guide || is_supertagging || EBUG
		      || TRUE
#endif /* is_guide || is_supertagging || EBUG */
		      )
		    SXBA_1_bit (new_item_set, new_item);
		}
	      }
	    }
	  }

	  if (is_good)
	    SXBA_1_bit (red_order_set, order);

	  /* Le bloc ci_dessous semble valide ds tous les cas (independant de MAKE_INSIDEG)
	     Il semble aussi possible de le supprimer purement et simplement et de reboucler
	     s'il y a des prods (non traitees) ds prod_order_set */
#if is_right_recursive
	  /* Un cycle est fini ssi aucune production de ce cycle n'a marche.
	     sinon, il faut recommencer le cycle et reexecuter les productions
	     meme celles qui n'ont pas marche le coup precedent. */

	  if ((maxo = prod_order2max [order]) != 0) {
	    /* production recursive droite */
#if 1
	    /* sera peut-etre reexecutee */
	    SXBA_1_bit (prod_order_set, order);
#endif /* 1 */

	    if (is_good) {
	      /* La sequence sera reexecutee */
	      must_loop = TRUE;
	      /* Le 19/08/05 deplace la */
	      /* SXBA_1_bit (prod_order_set, order); */
	    }

	    new_order = sxba_scan (prod_order_set, order);

	    if ((new_order > maxo || new_order == -1)) {
	      /* Sortie de cycle */
	      if (must_loop) {
		/* On recommence */
		order = prod_order2min [order]-1; /* ordre du debut de boucle-1 */
		must_loop = FALSE;
	      }
	      else {
		/* Le 22/08/05 */
		/* On enleve ceux qui ont ete ajoutes */
		new_order = prod_order2min [order];

		do {
		  if (prod_order2max [new_order] == maxo)
		    /* ds le cycle */
		    SXBA_0_bit (prod_order_set, new_order);
		} while ((new_order = sxba_scan (prod_order_set, new_order)) <= maxo && new_order != -1);
	      }
	    }
	  }
#endif /* is_right_recursive */

#if is_rcvr
	  /* Si on vient de rcvr-cas-tordu, on veut positionner à 1 l'offset order de red_order_sets [j],
	     et stocker l'item courant dans new_item_set, même si !is_good */
	  if (is_error_detected && SXBA_bit_is_set (tmp_rcvr_order_set, order)) {
	    SXBA_1_bit (red_order_set, order);

	    if (
#if is_guide || is_supertagging || EBUG
		TRUE
#else /* is_guide || is_supertagging || EBUG */
		is_parse_forest
#endif /* is_guide || is_supertagging || EBUG */
		)
	      SXBA_1_bit (new_item_set, item);
	  }
#endif /* is_rcvr */

	} while ((order = sxba_scan_reset (prod_order_set, order)) > 0);
 
#ifdef MAKE_INSIDEG
	if ((order = sxba_scan_reset (prod_order_set, 0)) < 0)
	  break;

	/* On reexamine tout */
      }
#else /* !MAKE_INSIDEG */
#if is_rcvr
	/* ESSAI le 19/08/05. On pourrait etre + fin */
	/* Le traitement d'erreur a pu mettre ds prod_order_set une reduction dont l'ordre d'execution
	   devrait preceder le courant */
	if (!is_error_detected || (order = sxba_scan_reset (prod_order_set, 0)) < 0)
	  break;

	/* On reexamine tout */
      }
#endif /* is_rcvr */
#endif /* !MAKE_INSIDEG */

#if is_right_recursive
      sxba_empty (prod_order_set);
#endif /* is_right_recursive */
		  
      /* Le 28/08/2003 */
      if (is_parse_forest
#if is_guide || is_supertagging || LOG
	  || TRUE
#endif /* is_guide || is_supertagging || EBUG */
	  ) {
	/* Ici, normalement, toutes les lhs instanciees des prod que l'on trouve sont reliees a l'axiome
	   => On est + precis que le reconnaisseur */
	/* Attention les RL_nt_set et les ap_sets doivent etre razes */
	/* Si la rhs des prod est ds T* => il y a eu un appel direct */
	new_item = 0;

	while ((new_item = sxba_scan_reset (new_item_set, new_item)) > 0) {
	  prod = PROLIS (new_item);
	  item = PROLON (prod+1)-1;
	  tnb = item-new_item;
	  index_set = PT2 [j-tnb].index_sets [new_item];

	  /* Le 08/02/06 */
	  A = LHS (prod); 
#ifdef MAKE_INSIDEG
	  init_A = spf.insideG.nt2init_nt [A];
#else /* !MAKE_INSIDEG */
	  init_A = A;
#endif /* !MAKE_INSIDEG */

	  i = -1;

	  while ((i = sxba_scan (index_set, i)) >= 0) {
	    /* Majorant du nb de Aij */
	    MAKE_Aij (Aij, init_A, i, j);
	  }

#if LOG
	  output_ni_prod (prod, index_set, 0, j);
#endif /* LOG */

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
  }
    
  sxfree (RL_nt_set);
  sxfree (prod_order_set);
  sxfree (wis);
#if is_rcvr
  if (tmp_rcvr_order_set)
    sxfree(tmp_rcvr_order_set);
#endif /* is_rcvr */

  if (is_parse_forest
#if is_guide || is_supertagging || EBUG
      || TRUE
#endif /* is_guide || is_supertagging || EBUG */
      )
    sxfree (new_item_set);
}


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




static BOOLEAN    use_a_single_rcvr_string;
static XxY_header rcvr_out_range_memo;

/* A cause de la rcvr, on reconstruit un source fictif qui remplace le source entre lb et ub */
/* Ce source est (pre-)concatene aux commentaires des tokens commencant en ub */
/* Retourne le nb de terminaux de la chaine corrigee */
/* Pour un [lb..ub] donne' si cette ft est appelee plusieurs fois, les 2eme et suivantes, elle retourne un varstr vide (le 21/02/08) */
static BOOLEAN
is_rcvr_out_range_first_call (SXINT lb, SXINT ub)
{
  SXINT range;

  if (!XxY_is_allocated (rcvr_out_range_memo)) {
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
  BOOLEAN        has_empty_ste;
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
      has_empty_ste = FALSE;

      XxYxZ_YZforeach (store_rcvr_string_hd, lb, ub, triple) {
	ste = XxYxZ_X (store_rcvr_string_hd, triple);

	if (ste == EMPTY_STE)
	  has_empty_ste = TRUE;

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

	  if (ste != EMPTY_STE) {
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

#if 0
    /* Le 18/02/08 c'est l'appelant qui sait quoi faire de wvstr */
    if (varstr_length (wvstr)) {
      /* Ici wvstr contient la chaine corrigee complete avec les commentaires de la chaine erronee qu'elle remplace */
      /* On va "incorporer" cette chaine aux commentaires des tokens qui commencent en rdag_state */
      /* Si nb == 0, wvstr ne contient que des commentaires */

      if (rdag_state == idag.final_state) {
	/* rdag_state est l'etat final du DAG */
	/* On change le comment du eof final */
	ptoken = &(SXGET_TOKEN (idag.toks_buf_top));

	if (ptoken->comment) {
	  varstr_raz (varstring);
	  varstring = varstr_catenate (varstring, varstr_tostr (wvstr));

	  if (nb) {
	    varstring = varstr_catchar (varstring, ' ');
	    varstring = varstr_catenate (varstring, ptoken->comment);
	  }
	  else {
	    varstr_pop (varstring, 1); /* On enleve le "}" de fin */
	    varstring = varstr_catchar (varstring, ' '); /* on le remplace par un blanc */
	    varstring = varstr_catenate (varstring, ptoken->comment+1); /* On enleve le '{' de debut du comment */
	  }

	  ste = sxstr2save (varstr_tostr (varstring), varstr_length (varstring));
	}
	else
	  ste = sxstr2save (varstr_tostr (wvstr), varstr_length (wvstr));

	ptoken->comment = sxstrget (ste);
      }
      else {
	bot_pq = idag.p2pq_hd [rdag_state];
	top_pq = idag.p2pq_hd [rdag_state+1];

	while (bot_pq < top_pq) {
	  tok_no = idag.pq2tok_no [bot_pq];

	  if (tok_no < 0) {
	    tok_no_stack = idag.tok_no_stack - tok_no;
	    top_tok_no_stack = tok_no_stack + *tok_no_stack;
	    tok_no_stack++;
	  }
	  else {
	    /* truc */
	    top_tok_no_stack = tok_no_stack = &tok_no;
	  }

	  while (tok_no_stack <= top_tok_no_stack) {
	    tok_no = *tok_no_stack++;
	    ptoken = &(SXGET_TOKEN (tok_no));

	    if (ptoken->comment) {
	      varstr_raz (varstring);
	      varstring = varstr_catenate (varstring, varstr_tostr (wvstr));

	      if (nb) {
		varstring = varstr_catchar (varstring, ' ');
		varstring = varstr_catenate (varstring, ptoken->comment);
	      }
	      else {
		varstr_pop (varstring, 1); /* On enleve le "}" de fin */
		varstring = varstr_catchar (varstring, ' '); /* on le remplace par un blanc */
		varstring = varstr_catenate (varstring, ptoken->comment+1); /* On enleve le '{' de debut du comment */
	      }

	      ste = sxstr2save (varstr_tostr (varstring), varstr_length (varstring));
	    }
	    else
	      ste = sxstr2save (varstr_tostr (wvstr), varstr_length (wvstr));

	    ptoken->comment = sxstrget (ste);
	  }

	  bot_pq++;
	}
      }
    }
#endif /* 0 */
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
  if (lgth > 0 && string [lgth-1] == '!')
    which_case = 1;
  else
    which_case = 0;
#else
  which_case = -1;
#endif /* output_only_special_terminals */
  
  return which_case;
}

BOOLEAN
sxearley_parse_it ()
{
  SXINT 	  t, nt, size, S1n;
  BOOLEAN ret_val = TRUE, insideG_kind;
  SXBA    t_set;

#ifdef bnf21rcg_include_file
  if (inputG_MAXITEM != rcg_itemmax) {
    fputs ("\nThe Earley/LC automaton and the RCG Lex structures are incompatible.\n", sxstderr);
    sxexit(1);
  }
#endif /* bnf21rcg_include_file */

#if MEASURES
  /* On est ds une campagne de mesures ... */
  printf ("MEASURES: There are %ld input words which select %ld terminal symbols.\n", last_tok_no, t_occur_nb);
#endif /* MEASURES */

  working_rcvr_mlstn = 1+n+delta_n+1;

#if MEASURES
  /* Pour la campagne de mesure */
  MEASURE_time (TIME_INIT);
#endif /* MEASURES */

  /* Traitement de la lexicalisation */
  is_mlstn2non_lexicalized_look_ahead_t_set = TRUE;
#if is_lex
  is_mlstn2lexicalized_look_ahead_t_set = TRUE;
#endif /* is_lex */


#if is_lex  
  /* Le dernier arg insideG_kind a ete ajoute le 20/04/07
     Si lexicalizer2basic_item_set est TRUE, il indique :
        TRUE  => Que insideG est la + petite grammaire reduite possible (ca depend des options du lexicalizer) construite
                 Dans ce cas basic_item_set est inutile (il est plein)
        FALSE => insideG est la 1ere construite et basic_item_set la filtre
	
	A priori si TRUE, l'analyse est + rapide et prend moins de place mais le rattrapage d'erreur eventuel ne se fera
	  que sur cette grammaire restreinte et pourrait donc etre moins bon/pertinent.
  */
  insideG_kind = TRUE; /* Faut-il en faire une option de compil ou un arg de l'appel de earley ? */

  if (lexicalizer2basic_item_set (is_mlstn2lexicalized_look_ahead_t_set, is_mlstn2non_lexicalized_look_ahead_t_set, insideG_kind)) {
    if (insideG_kind) sxfree (basic_item_set), basic_item_set = NULL; /*inutile */
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
    ret_val = FALSE;

#if !is_rcvr
    /* ... et pas de rattrapage, il est donc inutile de lancer l'analyse */
    /* Ca permet aussi de sortir : exit () de syntax avec le bon code */
    sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
	    "%sSyntax Error.",
	    sxplocals.sxtables->err_titles [2]);
#endif /* !is_rcvr */

    ilex_compatible_item_set = NULL;
    if (mlstn2lexicalized_look_ahead_t_set) sxbm_free (mlstn2lexicalized_look_ahead_t_set), mlstn2lexicalized_look_ahead_t_set = NULL;
    mlstn2lexicalized_look_ahead_t_set = NULL; /* mlstn2lex_la_tset, global definie ds lexicalizer_mngr vaut NULL */
    mlstn2look_ahead_t_set = mlstn2non_lexicalized_look_ahead_t_set = mlstn2la_tset;
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
  fill_mlstn2look_ahead_item_set (TRUE /* first_time */);
#endif /* is_1la */

#if is_parser
  {
    is_semact_fun = for_semact.semact != NULL;

    /* Finalement le 20/09/06 on construit tj la foret partagee !! */
    is_parse_forest = TRUE;

#if is_full_guide
    /* is_print_prod = TRUE; ESSAI */
#ifdef MAKE_INSIDEG
    lb = (SXINT*) sxcalloc ( MAXRHSLGTH+1, sizeof (SXINT));
    ub = (SXINT*) sxcalloc ( MAXRHSLGTH+1, sizeof (SXINT));
#endif /* MAKE_INSIDEG */
    for_semact.sem_init = NULL;
    for_semact.sem_final = NULL;
    for_semact.semact = output_full_guide;
    for_semact.parsact = NULL;
    for_semact.prdct = NULL;
    for_semact.constraint = NULL;
    for_semact.sem_pass = NULL;
    for_semact.scanact = NULL;
#endif /* is_full_guide */

  }
#endif /* is_parser */

  sxba_bag_alloc (&shift_bag, "shift_bag", (working_rcvr_mlstn+2) * MAXPROD * NBLONGS (working_rcvr_mlstn + 1), statistics);
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
  ntXindex_set = sxbm_calloc (MAXNT+1, working_rcvr_mlstn+2), lhs_order_set = sxba_calloc (MAXNT+1);

#if is_rcvr
  rcvr_xcld_min_mlstn_set = sxba_calloc (working_rcvr_mlstn+2);
  store_non_kernel_item_sets = sxbm_calloc (working_rcvr_mlstn+2, MAXITEM+1);
  rcvr_kernel_item_sets = sxbm_calloc (working_rcvr_mlstn+2, MAXITEM+1);
  rcvr_reduce_item_sets = sxbm_calloc (working_rcvr_mlstn+2, MAXITEM+1);
#endif /* is_rcvr */

  non_kernel_item_sets = sxbm_calloc (working_rcvr_mlstn+2, MAXITEM+1);

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

  if (is_no_semantics) {
    for_semact.sem_init = NULL;
    for_semact.sem_final = NULL;
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
      for_semact.semact = NULL;
      for_semact.parsact = NULL;
      for_semact.prdct = NULL;
      for_semact.constraint = NULL;
      for_semact.sem_pass = spf_print_tree_count;
      for_semact.scanact = NULL;
    }
  }

  /* else is_default_semantics, on execute la semantique specifiee par la grammaire. */

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
  is_prdct_fun = TRUE;
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

  /* On lance ne lance pas recognize () si dynamic_set_automaton () a echoue' et s'il n'y a pas de traitement d'erreur ... */
  if (
#if is_rcvr
      TRUE ||
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

#if !is_parser && is_guide || LOG
    working_index_set = sxba_calloc (n+1);
#endif /* !is_parser && is_guide || LOG */

#if is_rcvr
#ifdef MAKE_INSIDEG
    rcvr_store_unfiltered_non_kernel_item_set = sxba_calloc (MAXITEM+1);
#endif /* MAKE_INSIDEG */
    repair_mlstn_top = idag.final_state+1;
#endif /* is_rcvr */   

#if is_cyclic || is_rcvr
#ifdef MAKE_INSIDEG
    cyclic_stack = (SXINT*) sxalloc (MAXNT+1, sizeof (SXINT)), cyclic_stack [0] = 0;
#endif /* MAKE_INSIDEG */  
#endif /* is_cyclic || is_rcvr */

#ifdef MAKE_INSIDEG
    nt_hd =  (SXBA*) sxcalloc (MAXNT+1, sizeof (SXBA));
#endif /* MAKE_INSIDEG */ 

#if MEASURES || LOG
    used_prod_set = sxba_calloc (MAXPROD+1);
#endif /* MEASURES || LOG */

    ret_val = recognize ();

#if MEASURES || LOG
    output_G ("recognizerG", NULL, used_prod_set); /* Ds lexicalizer_mngr !! */
#endif /* MEASURES || LOG */

#if is_cyclic || is_rcvr
#ifdef MAKE_INSIDEG
    sxfree (cyclic_stack), cyclic_stack = NULL;
#endif /* MAKE_INSIDEG */ 
#endif /* is_cyclic || is_rcvr */

#ifdef MAKE_INSIDEG
    sxfree (nt_hd), nt_hd = NULL;
#endif /* MAKE_INSIDEG */ 

#if is_rcvr
#ifdef MAKE_INSIDEG
    sxfree (rcvr_store_unfiltered_non_kernel_item_set), rcvr_store_unfiltered_non_kernel_item_set = NULL;
    if (rcvr_non_kernel_item_set) sxfree (rcvr_non_kernel_item_set), rcvr_non_kernel_item_set = NULL;
    if (rcvr_w2titem_set) sxfree (rcvr_w2titem_set), rcvr_w2titem_set = NULL;
    if (rcvr_w2item_set) sxfree (rcvr_w2item_set), rcvr_w2item_set = NULL;
    if (rcvr_reduce_item_stack) sxfree (rcvr_reduce_item_stack), rcvr_reduce_item_stack = NULL;
#endif /* MAKE_INSIDEG */
#endif /* is_rcvr */    

#if !is_parser && is_guide || LOG
    sxfree (working_index_set), working_index_set = NULL;
#endif /* !is_parser && is_guide || LOG */

#if is_epsilon && is_rcvr
    if (non_kernel_empty_item_set) sxfree (non_kernel_empty_item_set), non_kernel_empty_item_set = NULL;
#endif /* is_epsilon && is_rcvr */
  }


#if is_lex
  if (basic_item_set) sxfree (basic_item_set), basic_item_set = NULL;
  if (basic_nt_set) sxfree (basic_nt_set), basic_nt_set = NULL;
#endif /* is_lex */

#if is_lex
  /* Attention, il se peut que mlstn2la_tset==mlstn2lex_la_tset (le 12/01/07) */
  if (mlstn2lex_la_tset && mlstn2lex_la_tset != mlstn2la_tset) {
    sxbm_free (mlstn2lex_la_tset), mlstn2lex_la_tset = NULL;
  }
#endif /* is_lex */
  if (mlstn2la_tset) sxbm_free (mlstn2la_tset), mlstn2la_tset = NULL;

#if has_Lex
  sxfree (prod_checked_set), prod_checked_set = NULL;
  sxfree (prod_val_set), prod_val_set = NULL;
#endif /* has_Lex */

#if is_parser
  if (ret_val && sxerrmngr.nbmess [2] == 0) {
#ifdef MAKE_INSIDEG
    {
      SXINT             sizeof_shift_NT_stack, sizeof_backward_index_sets, sizeof_index_sets, i;
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
    sxba_bag_alloc (&pt2_bag, "pt2_bag", (1+n + 1) * MAXPROD * NBLONGS (1+n + 1), statistics);
    ap_sets = sxbm_calloc (MAXNT+1, 1+n+1);
    red_order_sets = sxbm_calloc (1+n+1, MAXPROD+1);

    /* ATTENTION CHANGEMENT :
       Un symbole Aij ou i et j sont deux numeros de table signifie que :
       Aij =>* ai ai+1 ... aj-1 */

    /* Modif du 08/02/06 */
    /* On cree des Aij, Tij meme si is_parse_forest n'est pas positionne' (on a donc des variables locales) */

    spf_allocate_Aij (MAXNT, SXEOF);

#if is_rcvr
    spf.outputG.Tpq_rcvr_set = sxba_calloc (XxYxZ_size (spf.outputG.Txixj2Tij_hd)+1);
#endif /* is_rcvr */

#if is_rcvr
    if (is_error_detected)
      global_rcvr_valid_left_mlstn_set = sxba_calloc (working_rcvr_mlstn+2);

#if is_parser && !is_guide && !is_supertagging
    if (XxY_is_allocated (glbl_dag_rcvr_resume_kitems))
      glbl_dag_rcvr_start_kitems_set = sxba_calloc (XxYxZ_top (glbl_dag_rcvr_start_kitems)+1);
#endif /* is_parser && !is_guide && !is_supertagging */
#endif /* is_rcvr */

#if LOG
    fputs ("\n**** REDUCER (top-down filter) ****\n", stdout);
#endif /* LOG */

    RL_mreduction ();

#if is_rcvr
    if (global_rcvr_valid_left_mlstn_set) sxfree (global_rcvr_valid_left_mlstn_set), global_rcvr_valid_left_mlstn_set = NULL;
#endif /* is_rcvr */

    if (is_print_time)
      sxtime (TIME_FINAL, "\tEarley Reducer");
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

    printf ("%ld ", oracle_nb);
  }
#endif /* is_make_oracle */

  sxba_bag_free (&shift_bag);
  sxbm_free (ntXindex_set), sxfree (lhs_order_set), lhs_order_set = NULL;
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
  sxbm_free (store_non_kernel_item_sets);
#endif /* is_rcvr */

#if is_rcvr
  sxbm_free (rcvr_kernel_item_sets);
#endif /* is_rcvr */

  sxbm_free (non_kernel_item_sets);

#if is_supertagging
  sxfree (used_non_lex_prod_set), used_non_lex_prod_set = NULL;
#endif /* is_supertagging */

#if is_parser && !is_guide && !is_supertagging
  if (ret_val && sxerrmngr.nbmess [2] == 0) {
    /* Le 08/02/06 positionnes meme si !is_parse_forest */
    spf_allocate_spf (MAXPROD);
    spf.outputG.maxxnt = spf.outputG.maxnt = XxYxZ_top (spf.outputG.Axixj2Aij_hd);
	
    if (for_semact.sem_init != NULL)
      (*for_semact.sem_init) ();

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
#ifdef MAKE_INSIDEG
      sxfree (PT2_area), PT2_area = NULL; /* area */
#endif /* MAKE_INSIDEG */
      sxfree (PT2), PT2 = NULL;
      sxba_bag_free (&pt2_bag);
      sxbm_free (ap_sets);
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

	printf ("%ld/", ++new_prod);
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
      if (TRUE) {
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
	spf_make_proper (spf.outputG.start_symbol);
      }
      else
#endif /* is_rcvr */  
	/* Attention, si la grammaire est huge spf.inputG.has_cycles n'a pas ete positionne' et il n'a pas ete calcule'
	   ds insideG (faut-il le faire ?) sachant que le 1er appel a` la foret virera les cycles ? */
	if (spf.inputG.has_cycles)
	  /* Le 13/01/06 */
	  /* je vire les cycles eventuels de la foret */
	  spf_make_proper (spf.outputG.start_symbol);


      if (is_tagged_dag)
	/* On imprime le source selectionne par la foret sous forme de udag */
	spf_yield2dfa (TRUE /* udag */);
#if LOG
      else {
	/* A voir, la version precedente etait beaucoup trop longue */
	puts ("\n\n ******* Tagged input sentence in minimal UDAG form:");
	spf_yield2dfa (TRUE /* udag */);
      }
#endif /* LOG */
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

    if (for_semact.sem_pass != NULL) {
      (*for_semact.sem_pass) ();
    }

    spf_free ();

#if is_rcvr
    if (repair_Tpq2tok_no) {
      X_free (&repair_Tpq_hd);
      sxfree (repair_Tpq2tok_no), repair_Tpq2tok_no = NULL;
    }

    if (XxY_is_allocated (rcvr_out_range_memo))
      XxY_free (&rcvr_out_range_memo);
#endif /* is_rcvr */
	
    if (for_semact.sem_final != NULL)
      (*for_semact.sem_final) ();
  }
#endif /* is_parser && !is_guide && !is_supertagging */

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
    XxYxZ_free (&glbl_dag_rcvr_start_kitems);
    if (glbl_dag_rcvr_start_kitems_set) sxfree (glbl_dag_rcvr_start_kitems_set), glbl_dag_rcvr_start_kitems_set = NULL;
    if (XxYxZ_is_allocated (store_rcvr_string_hd)) XxYxZ_free (&store_rcvr_string_hd);
  }
#endif /* is_rcvr */

  if (red_order_sets) {
#ifdef MAKE_INSIDEG
    sxfree (PT2_area), PT2_area = NULL; /* area */
#endif /* MAKE_INSIDEG */
    sxfree (PT2), PT2 = NULL;
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
  SXINT     j, item_j, prod, prod_order, nbt, l, *Aij_stack, repair_parse_id;
  SXBA    red_order_set;

  for (j = 0; j <= n+1; j++) {
    red_order_set = red_order_sets [j];
    /* red_order_set est positionne par le traitement des PT2. */
    /* il contient les A -> \alpha t. */
    prod_order = MAXPROD+1;

    while ((prod_order = sxba_1_rlscan (red_order_set, prod_order)) > 0) {
      /* Examine' du plus grand vers le + petit. */
#ifdef MAKE_INSIDEG
      /* red_order_set contient directement une prod car order2prod n'est pas calcule' dynamiquement par lexicalizer_mngr */
      prod = prod_order;
#else /* !MAKE_INSIDEG */
      prod = order2prod [prod_order];
#endif /* !MAKE_INSIDEG */

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
parse_item (item_k, Aij_stack, i_set, i, k, j)
     SXINT item_k, *Aij_stack, i, k, j;
     SXBA i_set;
{
  SXINT        prod, item_i, item_h, X, h, prdct_no, li, repair_parse_id;
  SXBA       backward_index_set, index_set;
#if is_rcvr
  SXINT        resume, triple, kept_triple, cur_item, A, symb_nb, *local_Aij_stack;
  SXINT        couple, RHSij, bot, top, cur, Xpq;
  BOOLEAN    is_item_k_a_glbl_rcvr_kernel_item, has_rcvr, is_OK;
#endif /* is_rcvr */

  prod = PROLIS (item_k);
  item_i = PROLON (prod);

#if is_rcvr
  is_item_k_a_glbl_rcvr_kernel_item = XxY_is_allocated (glbl_dag_rcvr_resume_kitems) &&
    ((resume = XxY_is_set (&glbl_dag_rcvr_resume_kitems, k, item_k)) > 0);
#endif /* is_rcvr */

  if (item_i == item_k) {
    /* reduce */
    /* item_i = Aij -> . \alpha */
#if is_rcvr
    is_item_k_a_glbl_rcvr_kernel_item = XxY_is_allocated (glbl_dag_rcvr_resume_kitems) &&
      ((resume = XxY_is_set (&glbl_dag_rcvr_resume_kitems, k, item_k)) > 0);

    if (is_item_k_a_glbl_rcvr_kernel_item) {
      if (i_set != NULL)
	i = sxba_scan (i_set, -1);

      while (i >= 0) {
	triple = XxYxZ_is_set (&glbl_dag_rcvr_start_kitems, i, item_k, resume);

	is_OK = FALSE;

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
	
	  is_OK = TRUE;
	}
	else {
	  if (i == k) {
	    *Aij_stack-- = j; /* ajout */
	    *Aij_stack-- = k;
	  
	    is_OK = TRUE;
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
      if (i_set == NULL && i == k ||
	  i_set != NULL && SXBA_bit_is_set (i_set, k)) {
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
      ((i_set == NULL) && SXBA_bit_is_set (index_set, i) || (i_set != NULL) && IS_AND (index_set, i_set))) {
    backward_index_set = PT2 [k].backward_index_sets [item_k];

#if EBUG
    if (backward_index_set == NULL)
      sxtrap (ME, "parse_item");
#endif

    item_h = item_k-1;
    h = -1;

    while ((h = sxba_scan (backward_index_set, h)) > 0) {
      if (h >= i) {
#if is_prdct
	if ((!is_prdct_fun || (prdct_no = prdct [item_h]) == -1) ||
	    (*for_semact.prdct) (2 /* No de passe : analyseur */, item_h, h, k, prdct_no))
#endif  /* is_prdct */
	  {
#if is_rcvr
	    if (is_item_k_a_glbl_rcvr_kernel_item) {
	      item_h = item_k+1;
	      kept_triple = 0;
	      cur_item = 0;

	      /* Le couple (k, item_k) a ete ajoute par le traitement d'erreur... */ 
	      /* ... il faut trouver a quoi correspond h */
	      XxYxZ_XZforeach (glbl_dag_rcvr_start_kitems, h, resume, triple) {
		cur_item = XxYxZ_Y (glbl_dag_rcvr_start_kitems, triple);

		if (SXBA_bit_is_set (glbl_dag_rcvr_start_kitems_set, triple)) {
		  if (cur_item < item_h) {
		    /* Je ne conserve que le + a gauche!! */
		    item_h = cur_item;
		    kept_triple = triple;
		  }
		}
	      }

	      if (cur_item == 0) {
		/* Pas de rcvr, traitement normal, on remet item_h... */
		item_h = item_k-1;
		Aij_stack [0] = k;
		Aij_stack [-1] = h;
		parse_item (item_h, Aij_stack-2, i_set, i, h, j);
	      }
	      else {
#if EBUG
		if (item_h > item_k)
		  /* Il semble que ce cas soit exclus par RL_mreduction_item */
		  sxtrap (ME, "parse_item");
#endif /* EBUG */
  
		/* rcvr... */
		/* ... on ne s'occupe donc pas de earley_dag_glbl_rcvr_triples */
		/* On pourra le virer si les tests montrent que c'est inutile */

		/* Ici erreur confirmee */
		if (SXBA_bit_is_reset_set (glbl_dag_rcvr_messages_set, kept_triple))
		  /* Un seul message par erreur (la meme recup sur \betaij peut se trouver impliquee ds plusieurs reduc
		     Ahk -> ... \betaij ... */
		  rcvr_message2repair_parse_id [kept_triple] =
		    repair_parse_id =
		    earley_glbl_rcvr_message (item_h, h, item_k, k, 2 /* error */, is_parse_forest);
		else
		  repair_parse_id = rcvr_message2repair_parse_id [kept_triple];
	
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
		    
		continue; /* Pas de traitement normal le 27/10/05 */
	      }
	    }
	    else {
	      /* traitement normal */
	      Aij_stack [0] = k;
	      Aij_stack [-1] = h;
	      parse_item (item_h, Aij_stack-2, i_set, i, h, j);
	    }
#else /* !is_rcvr */
	    Aij_stack [0] = k;
	    Aij_stack [-1] = h;
	    parse_item (item_h, Aij_stack-2, i_set, i, h, j);
#endif /* !is_rcvr */
	  }
      }
    }
  }
}
#endif /* is_parser */

