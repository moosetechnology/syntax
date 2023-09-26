#ifndef nbest_h
#define nbest_h

#include "XxY.h"

extern void   nbest_open (int cur_contextual_proba_kind);
extern void   nbest_close ();

extern void   nbest_allocate (int nbest, int cur_contextual_proba_kind);
extern void   nbest_free ();
extern int    nbest_perform (int root_Aij, int h, int b, int cur_contextual_proba_kind, BOOLEAN do_filtering);
extern double nbest_get_weight (int root_Aij, int h, int cur_contextual_proba_kind);
extern int    nbest_get_Pij (int root_Aij, int h , int cur_contextual_proba_kind);
#if 0
/* A VOIR : permet de faire du nbest de l'exterieur !! */
extern void   nbest_filter_forest ();
extern void   nbest_print_forest (int Aij_or_item, int cur_contextual_proba_kind, int k);
#endif /* 0 */

extern double get_Tpq_logprob (int item, int cur_contextual_proba_kind);

#define PCFG         0
#define EPCFG        1
#define A_EPCFG      2
#define P_EPCFG      3
#define EP_EPCFG     4
#define EP_POS_EPCFG 5

double              min_logprob;
int                 *Pij2eprod;

extern int          logprobs_prod_nb; /* le nb de prod utilisees par la CFG qui a permis de construire les probas */
extern int          logprobs_nt_nb; /* le nb de nt utilises par la CFG qui a permis de construire les probas */
extern double       min_logprob_fs; /* Proba min des f_structures */

extern XxY_header   prodXval2eprod_hd, full_ctxt_hd, full_ctxtXeprod_hd, eprod_ctxtXeprod_hd, prod_ctxtXeprod_hd, A_ctxtXeprod_hd;
extern double       full_ctxtXeprod2logprobs [], eprod_ctxtXeprod2logprobs [], prod_ctxtXeprod2logprobs [],
                    A_ctxtXeprod2logprobs [], eprod2logprobs [], prod2logprobs [];

/* Pour la desambiguation lexicale */
extern XxY_header   ff_idXt2lex_id_hd, full_ctxtXlex_id_hd, eprod_ctxtXlex_id_hd, prod_ctxtXlex_id_hd, A_ctxtXlex_id_hd;
extern double       lex_id2logprobs [], full_ctxtXlex_id2logprobs [], eprod_ctxtXlex_id2logprobs [], prod_ctxtXlex_id2logprobs [],
                    A_ctxtXlex_id2logprobs [];

extern XxY_header   ctxtXsign_hd;
extern double       ctxtXsign2logprobs [];
extern XH_header    fs_signatures_hd;

#endif /* nbest_h */
