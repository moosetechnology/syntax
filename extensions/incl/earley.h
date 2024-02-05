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

#ifndef earley_h
#define earley_h
#include "SXante.h"

/* Le handleur positionne cette variable s'il est active' */
SX_GLOBAL_VAR bool is_virtual_timeout_signal_raised;

/* EARLEY.h */
/* Le 11/08/2003, changement de nom de semact.h en earley.h */
/* Interface entre les analyseurs a la Earley et les actions semantiques. */

extern void             sxearley_set_for_semact (void);
extern bool          sxearley_parse_it (SXINT what);
extern void             print_symb (FILE *out_file, SXINT symb, SXINT i, SXINT j);
/* permet ds tous les cas a la semantique d'acceder au token associe' a un Tpq */
/* extern struct sxtoken   *parser_Tpq2tok (SXINT Tpq);
   NON: maintenant, on accède au(x) token(s) d'un Tpq de la façon suivante:
     1. on va regarder spf.outputG.Tpq2tok_no [Tpq] ; si c'est positif, c'est le tok_no unique du Tpq ; sinon... 
     2. c'est négatif, et c'est l'opposée de la clé de la liste des tok_no de Tpq dans le XH nommé spf.outputG.Tpq2XH_tok_no 
   Ensuite, le passage de tok_no à tok est assuré par tok_no2tok (int tok_no) */


#include        "rcg_sglbl.h"

SX_GLOBAL_VAR SXINT	      n /* nb de token du source */;

SX_GLOBAL_VAR bool	      is_print_prod, is_parse_forest, is_tagged_dag, is_tagged_sdag, IS_DYNAMIC_SET_AUTOMATON;
SX_GLOBAL_VAR SXINT	      IS_CHECK_RHS_SEQUENCES;
SX_GLOBAL_VAR int             time_out;
SX_GLOBAL_VAR SXINT           vtime_out_s, vtime_out_us;
SX_GLOBAL_VAR double          beam_value;
SX_GLOBAL_VAR bool	      beam_value_is_set;

SX_GLOBAL_VAR struct mem_signatures   parser_mem_signatures;

#if defined(SXVERSION) && ( EBUG || LOG || LLOG )
#define statistics	(stdout)
#else /* !(EBUG || LOG || LLOG) */
/* Ne pas utiliser NULL!! */
#define statistics	(0)
#endif /* !(EBUG || LOG || LLOG) */

/* Le 04/10/06, changement */
#define rcvr_tok_no       0
#define max_rcvr_tok_no   0
#define delta_n           0


/* delta_n est un #define donne a la compilation qui donne le nombre maximal
   d'accroissement possible (par correction d'erreurs) de la chaine source. */
/* Meme ds le cas is_dag */
/* Ds les cas is_[dag_]rcvr, delta_n==0 => rattrapage global uniquement */
#ifndef delta_n
# define delta_n 0
#endif /* delta_n */

/* permet de selectionner une strategie systeme de parcours des couples de milestones sur lesquels
   la rcvr est essayee successivement.  L'utilisateur doit remplir ds l'appel de SEMANTICS() le champ
   rcvr_spec.range_walk_kind avec l'une de ces valeurs.
   Si ca ne lui convient pas il peut faire son propre parcours en emplissant for_semact.rcvr_range_walk
   avec sa propre fonction
*/
#define FORWARD_RANGE_WALK_KIND        1
#define BACKWARD_RANGE_WALK_KIND       2
#define MIXED_FORWARD_RANGE_WALK_KIND  3
#define MIXED_BACKWARD_RANGE_WALK_KIND 4

#define GEN_ALL_REPAIR_STRING          0

#define GEN_ALL_REPAIR_PARSE           0

#define TRY_MIN                        0
#define TRY_LOW                        1
#define TRY_MEDIUM                     2
#define TRY_HIGH                       3
#define TRY_MAX                        4

/* Cette structure est remplie par l'appel de SEMANTICS () */
SX_GLOBAL_VAR struct rcvr_spec {
  SXINT insertion; /* -1 => pas d'insertion */
  SXINT suppression;
  SXINT changement;
  SXINT deplacement;
  SXINT range_walk_kind;
  SXINT repair_kind; /* GEN_ALL_REPAIR_STRING ou nb */
  SXINT repair_parse_kind; /* GEN_ALL_REPAIR_PARSE ou nb */
  SXINT try_kind; /* [TRY_MIN..TRY_MAX] */
  bool perform_repair, perform_repair_parse;
} rcvr_spec;

#if defined(WINNT) && !defined(ushort)
typedef unsigned short	ushort;
#endif

#include "sxspf.h"

#if defined __GNUC__
#pragma GCC system_header
/* attention, ceci désactive les warnings jusqu'à la fin du fichier courant (seulement dans le cas GCC) */
#elif defined __SUNPRO_CC
#pragma disable_warn
#elif defined _MSC_VER
#pragma warning(push, 1)
#endif
SX_GLOBAL_VAR struct for_semact {
  void    (*sem_open) (),
    (*sem_init) (),
    (*sem_final) (),
    (*sem_close) (void),
    (*scanact) (SXINT, SXINT, char *), /* Le 17/11/05 pour traiter les [|...|] */
    (*oflw) (SXINT, SXINT),
    (*timeout_mngr) (int), /* BS 26/04/06 */
    (*vtimeout_mngr) (int);

  SXINT      (*prdct) (SXINT, SXINT, SXINT, SXINT, SXINT),
    (*parsact) (),
    (*semact) (),
    (*constraint) (SXINT, SXINT, SXINT, SXBA, SXINT, SXINT),
    (*sem_pass) (),
    (*rcvr) (),
    (*rcvr_range_walk) (SXINT, SXINT, SXINT, SXINT*, SXINT*); /* c'est l'utilisateur qui calcule les range de la rcvr */

  char        *(*string_args) (void), /* Le 18/01/06 retourne l'utilisation des args de la semantique */
    *(*ME) (void); /* nom de la semantique */

  bool     (*process_args) (int*, int, char **); /* Le 18/01/06 pour traiter les args de la semantique */

  SXINT         pass_nb;

  struct mem_signatures mem_signatures;
} for_semact;
#if defined __SUNPRO_CC
#pragma enable_warn
#elif defined _MSC_VER
#pragma warning(pop)
#endif

#endif /* earley_h */
