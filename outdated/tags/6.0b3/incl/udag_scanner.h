#ifndef udag_scanner_h
#define udag_scanner_h

#include "varstr.h"

extern SXINT  dag_scanner             (SXINT what, struct sxtables *arg);
extern void   idag_source_processing  (SXINT /* SXEOF ca peut etre une macro ds les fichiers incluant udag_scanner.h */, SXBA *t2suffix_t_set, SXBA *mlstn2suffix_source_set, SXBA *mlstn2la_tset, SXBA *t2la_t_set);
extern SXINT  idag_p_q2pq             (SXINT p, SXINT q);
extern void   fill_idag_path          (void);
extern VARSTR sub_dag_to_comment      (VARSTR vstr, SXINT p, SXINT q);
extern VARSTR sub_dag_to_re           (VARSTR vstr, SXINT p, SXINT q);
extern SXINT  put_repair_tok          (SXINT t, char *t_str, char *comment_str, struct sxsource_coord *psource_index);
extern void   distribute_comment      (SXINT t);
extern void   idag_p_t_q2tok_no_stack (SXINT p, SXINT t, SXINT q);
extern void   fill_idag_q2pq_stack    (void);
/* nombre de tokens dans le source */
/*extern SXINT  get_toks_buf_size       (void); NON: on y accède désormais par idag.repair_toks_buf_top)*/

struct idag {
  SXINT          init_state, final_state, toks_buf_top, repair_toks_buf_top;

  SXINT
  *p2pq_hd /* Associe a chaque etat de depart p une liste de pq t.q. p2pq_hd[p] <= pq < p2pq_hd[p+1] */
    , *pq2q /* Associe a chaque pq un etat d'arrivee q */
    , *pq2tok_no /* Associe a chaque pq une liste de tok_no.  Deux cas soit tok_no=pq2tok_no[pq]
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
    , *pq2p /* permet, connaissant un pq de retrouver le p */
    ;

  SXBA
  source_set
  , orig_source_set /* sauvegarde temporaire de source_set pour le lexicaliseur, ds ce cas source_set est recode' */
  , *path /* path [i] = {j | i ->x j, x \in T^+} */
    ;
};

SX_GLOBAL_VAR struct idag idag;
/* Le DAG est lu ds le fichier source_file de nom source_file_name */
SX_GLOBAL_VAR FILE        *source_file;
SX_GLOBAL_VAR char        *source_file_name;
/* Nombre d'occurrences des terminaux du source (terminaux, pas formes-flechies) */
SX_GLOBAL_VAR SXINT       t_occur_nb;

/* C,a permet a l'utilisateur de specifier qq defauts si la forme-flechie n'est pas ds le dico */
SX_GLOBAL_VAR char        *ciu_name_ptr, *lciu_name_ptr, *digits_name_ptr;
#define CHECK_LOWER 1
#define CHECK_UPPER 2
SX_GLOBAL_VAR SXINT       default_set;

#define DAG_KIND    1
#define UDAG_KIND   2
#define SDAG_KIND   3
#define STRING_KIND 4
SX_GLOBAL_VAR SXINT       dag_kind;

/* Pour permettre des modifs dynamiques du dag, et pour conserver la propriete si (lpos, tok_no, rpos)
   on a lpos < rpos (par exemple traitement des erreurs), on maintient 2 tableaux dag_state2mlstn et mlstn2dag_state
   tels que la propriete precedente s'applique sur dag_state2mlstn[lpos] < dag_state2mlstn[rpos].
   Ce sont donc les milestones qui sont strictement croissants.
*/

/* Suppression des correspondances le 04/10/06 */
#define dag_state2mlstn(x)   (x)
#define mlstn2dag_state(x)   (x)

#endif /* udag_scanner_h */
