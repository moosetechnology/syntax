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
#ifndef udag_scanner_h
#define udag_scanner_h
#include "SXante.h"
#include "X.h"

#include <setjmp.h>
SX_GLOBAL_VAR jmp_buf environment_before_current_sentence;
SX_GLOBAL_VAR void (*free_after_long_jmp) (void);

#ifdef sxu2_h
extern SXINT  dag_scanner             (SXINT what, struct sxtables *arg);
extern SXINT  put_repair_tok          (SXINT t, char *t_str, char *comment_str, struct sxsource_coord *psource_index);
#endif /* sxu2_h */

extern void   idag_source_processing  (SXINT /* SXEOF ca peut etre une macro ds les fichiers incluant udag_scanner.h */, SXBA *t2suffix_t_set, SXBA *mlstn2suffix_source_set, SXBA *mlstn2la_tset, SXBA *t2la_t_set);
extern SXINT  idag_pq_t2pqt           (SXINT pq, SXINT t);
extern void   fill_idag_pqt2pq        (void);
extern void   fill_idag_path          (void);
extern void   fill_idag_pqt2proba     (void);

#ifdef varstr_h
extern VARSTR sub_dag_to_comment      (VARSTR vstr, SXINT p, SXINT q);
extern VARSTR sub_dag_to_re           (VARSTR vstr, SXINT p, SXINT q);
#endif /* varstr_h */

extern void   distribute_comment      (SXINT t);
extern void   idag_p_t_q2tok_no_stack (SXINT **tok_no_stack_ptr, SXINT p, SXINT t, SXINT q);
extern void   fill_idag_q2pq_stack    (void);
#if 0
/* obsolete, remplace' par range2p */
extern void   fill_idag_pq2p          (void);
#endif /* 0 */
extern void   fill_more_idag_infos    (SXBOOLEAN is_qstack_needed, SXBOOLEAN is_pstack_needed, SXBOOLEAN is_inside_t_needed/* A COMPLETER : , SXBOOLEAN is_StrLen_needed */);
/* nombre de tokens dans le source */
/*extern SXINT  get_toks_buf_size       (void); NON: on y accède désormais par idag.repair_toks_buf_top)*/
extern char   **idag_get_t2string     (void); /* Retourne le pointeur vers t2string */
extern SXINT  *idag_p_l2q_SS_stack    (SXINT *SS_stack, SXINT p, SXINT l);

#define p_q2pq(p,q)                   (idag.p_q2range [p][q])
#define p_q2range(p,q)                (idag.p_q2range [p][q])
#define range2p(r)                    (idag.range2p [r])
#define range2q(r)                    (idag.range2q [r])
#define pqt2pq(pqt)                   (idag.pqt2pq [pqt])
#define pqt2p(pqt)                    (idag.range2p [pqt2pq (pqt)])
#define pqt2q(pqt)                    (idag.range2q [pqt2pq (pqt)])
#define pqt2t(pqt)                    (idag.t_stack [pqt])
#define pq_t2pqt(pq,t)                (idag_pq_t2pqt (pq, t))
#define p_q_t2pqt(p,q,t)              (pq_t2pqt (p_q2range (p, q), t))
#define MAX_pqt()                     (idag.pqt2pq [0])
#define pqt2proba(pqt)                (idag.pqt2proba [pqt])
#define pq_t2proba(pq,t)              (idag.pqt2proba [pq_t2pqt (pq, t)])
#define p_q_t2proba(p,q,t)            (idag.pqt2proba [pq_t2pqt (p_q2range (p, q), t)])

#define MAKE_AN_IDAG_RANGE(p,q)       p_q2range (p, q)

#if 0
/* obsolete */
#if defined (__GNUC__) && !EBUG
#define idag_p_q2pq(p,q)              ({                                                                         \
                                         SXINT bi = p, bs = q;                                                   \
                                         SXINT *pq_base_ptr;                                                     \
                                         ((bs <= bi || bs > *(pq_base_ptr = idag.pq_base2pq+idag.p2pq_base [bi]))\
                                            ? 0                                                                  \
					    : pq_base_ptr [bs-bi]);                                              \
				      })
#define MAKE_AN_IDAG_RANGE(p,q)       ({                                                                         \
                                         SXINT bi = p, bs = q, pq;                                               \
                                         (((pq = idag_p_q2pq (bi, bs)) == 0)                                     \
                                           ? X_is_set (&(idag.ranges), (bi<<idag.logn)+bs)+idag.last_pq          \
                                           : pq);                                                                \
                                      })
#else /* defined (__GNUC__) && !EBUG */
extern SXINT idag_p_q2pq (SXINT p, SXINT q);
extern SXINT MAKE_AN_IDAG_RANGE (SXINT p, SXINT q);
#endif /* defined (__GNUC__) && !EBUG */
#endif /* 0 */


struct idag {
  SXINT          init_state, final_state, toks_buf_top, repair_toks_buf_top, eof_code, dag_kind, ptq_nb,
    source_set_cardinal, logn, last_pq, max_range_value;

  SXINT
    *p2pq_base /* Voir la macro ... */
    , *pq_base2pq /* ... idag_p_q2pq(p,q) */
   , *p2pq_hd /* Associe a chaque etat de depart p une liste de pq t.q. p2pq_hd[p] <= pq < p2pq_hd[p+1] */
    , *pq2q /* Associe a chaque pq un etat d'arrivee q */
    , *pq2tok_no /* Associe a chaque pq une liste de tok_no.  Deux cas. Soit tok_no = pq2tok_no[pq]
		  Si tok_no > 0 la liste est un singleton qui ne contient que tok_no
		  Si tok_no < 0 alors tok_no_stack[-tok_no] est une stack des tok_no */
    , *tok_no_stack /* Voir ci_dessus */ 
    , *pq2t_stack_hd /* Associe a chaque pq la stack des terminaux qui permettent une transition de p a q. x=pq2t_stack_hd[pq] est un index dans t_stack ... */
    , *t_stack /* ... stack=t_stack+x est une stack qui contient la liste des terminaux qui permettent d'aller de p a q */
    , *orig_t_stack /* // a` t_stack, contient (eventuellement) les codes des terminaux d'origine alors que t_stack contient ceux de la "insideG" */
    , *tok_no2t_stack_hd /* Associe a chaque tok_no la stack des terminaux associes a ce tok_no. x=tok_no2t_stack_hd[tok_no] est un index dans local_t_stack ... */
    , *local_t_stack /* local_t_stack+x est la stack qui contient la liste des terminaux associes a un tok_no */
    , *q2pq_stack_hd /* q2pq_stack_hd [q] est un index ds q2pq_stack qui repere ... */
    , *q2pq_stack /* ... la pile des predecesseurs de q (en terme de pq).  Non renseigne' en standard, se construit en appelant fill_idag_q2pq_stack () */
#if 0
    , *pq2p /* permet, connaissant un pq de retrouver le p .  Non renseigne' en standard, se construit en appelant fill_idag_pq2p () */
#endif /* 0 */
    , *pqt2pq /* permet, connaissant un pqt (attention, ce n'est pas un Tij, mais un index dans idag.t_stack) de retrouver le pq. Non renseigne' en standard, se construit en appelant fill_idag_pqt2pq () */
    , *p2ff_ste /* dans le cas SDAG, permet de récupérer le ste de la ff (premier élément d'une ligne de sdag) */
    , **p_q2range /* contrairement à un pq, un range n'est pas forcément une transition ; tout pq est un range, un range non transition est > à idag.last_pq */
    , *range2p /* permet connaissant un range [p..q] de retrouver p, renseigne' en standard.  Plus general que l'ancien pq2p */
    , *range2q /* permet connaissant un range [p..q] de retrouver q, renseigne' en standard, subsume pq2q */
    , *tok_no2semlex_ste /* BS (pour débug sxpipe, dont pê inutile...) associe à un tok_no son éventuel semlex_ste */
    ;

  /* Non renseigne' en standard, se construit en appelant fill_more_idag_infos () */
  SXINT
  ***pt2q_disp /* *(pt2q_disp [p]+t) est une stack de q */
  , ***tq2p_disp /* *(tq2p_disp [q]+t) est une stack de p */
  ;

  SXBA
  source_set
    , orig_source_set /* sauvegarde temporaire de source_set pour le lexicaliseur, ds ce cas source_set est recode' */
    , *path /* path [i] = {j | i ->x j, x \in T^+} */
    , *range2inside_t_set /* ensemble des inside_t qui se trouvent entre p et q */
    , q_sets [2] /* sets de travail pour calculer les q qui se trouvent a la distance l d'un p */
    ;

  X_header 
  global_t2inside_t
    , ranges
    ;

  double
  *pqt2proba /* proba max de l'ensemble des transitions p --t--> q */
  ;
};


#ifndef sxu2_h
/* Le DAG est lu ds le fichier source_file de nom source_file_name */
extern FILE        *source_file;
extern char        *source_file_name;
#endif /* sxu2_h */

/* C,a permet a l'utilisateur de specifier qq defauts si la forme-flechie n'est pas ds le dico */
SX_GLOBAL_VAR char        *ciu_name_ptr, *lciu_name_ptr, *digits_name_ptr;
#define CHECK_LOWER 1
#define CHECK_UPPER 2
SX_GLOBAL_VAR SXINT       default_set;

#define DAG_KIND    1
#define UDAG_KIND   2
#define SDAG_KIND   4
#define STRING_KIND 8

SX_GLOBAL_VAR struct idag idag;
SX_GLOBAL_VAR SXINT       dag_kind SX_INIT_VAL(DAG_KIND);

/* Pour permettre des modifs dynamiques du dag, et pour conserver la propriete si (lpos, tok_no, rpos)
   on a lpos < rpos (par exemple traitement des erreurs), on maintient 2 tableaux dag_state2mlstn et mlstn2dag_state
   tels que la propriete precedente s'applique sur dag_state2mlstn[lpos] < dag_state2mlstn[rpos].
   Ce sont donc les milestones qui sont strictement croissants.
*/

/* Suppression des correspondances le 04/10/06 */
#define dag_state2mlstn(x)   (x)
#define mlstn2dag_state(x)   (x)

#endif /* udag_scanner_h */
