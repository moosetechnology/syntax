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
#ifndef nbest_h
#define nbest_h

#include "XxY.h"
#include "XH.h"

#ifndef SX_GLOBAL_VAR_NBEST
#define SX_GLOBAL_VAR_NBEST extern
#endif

SX_GLOBAL_VAR_NBEST void   nbest_open (SXINT cur_contextual_proba_kind);
SX_GLOBAL_VAR_NBEST void   nbest_close (void);

SX_GLOBAL_VAR_NBEST void   nbest_allocate (SXINT nbest, SXINT cur_contextual_proba_kind);
SX_GLOBAL_VAR_NBEST void   nbest_free (void);
SX_GLOBAL_VAR_NBEST SXINT    nbest_perform (SXINT root_Aij, SXINT h, SXINT b, SXINT cur_contextual_proba_kind, SXBOOLEAN do_filtering);
SX_GLOBAL_VAR_NBEST double nbest_get_weight (SXINT root_Aij, SXINT h, SXINT cur_contextual_proba_kind);
SX_GLOBAL_VAR_NBEST SXINT    nbest_get_Pij (SXINT root_Aij, SXINT h , SXINT cur_contextual_proba_kind);
#if 0
/* A VOIR : permet de faire du nbest de l'exterieur !! */
SX_GLOBAL_VAR_NBEST void   nbest_filter_forest ();
SX_GLOBAL_VAR_NBEST void   nbest_print_forest (SXINT Aij_or_item, SXINT cur_contextual_proba_kind, SXINT k);
#endif /* 0 */

SX_GLOBAL_VAR_NBEST double get_Tpq_logprob (SXINT item, SXINT cur_contextual_proba_kind);
SX_GLOBAL_VAR_NBEST void   nbest_seek_kth_best_tree (SXINT Aij_or_item, SXINT k, void (*f) (SXINT Pij, double weight));

#define PCFG         0
#define EPCFG        1
#define A_EPCFG      2
#define P_EPCFG      3
#define EP_EPCFG     4
#define EP_POS_EPCFG 5

SX_GLOBAL_VAR_NBEST SXINT               nbest;

SX_GLOBAL_VAR_NBEST double              min_logprob;
SX_GLOBAL_VAR_NBEST SXINT               *Pij2eprod;

SX_GLOBAL_VAR SXINT          logprobs_prod_nb; /* le nb de prod utilisees par la CFG qui a permis de construire les probas */
SX_GLOBAL_VAR SXINT          logprobs_nt_nb; /* le nb de nt utilises par la CFG qui a permis de construire les probas */
SX_GLOBAL_VAR double       min_logprob_fs; /* Proba min des f_structures */

SX_GLOBAL_VAR XxY_header   prodXval2eprod_hd, full_ctxt_hd, full_ctxtXeprod_hd, eprod_ctxtXeprod_hd, prod_ctxtXeprod_hd, A_ctxtXeprod_hd;
SX_GLOBAL_VAR double       full_ctxtXeprod2logprobs [], eprod_ctxtXeprod2logprobs [], prod_ctxtXeprod2logprobs [],
                    A_ctxtXeprod2logprobs [], eprod2logprobs [], prod2logprobs [];

/* Pour la desambiguation lexicale */
SX_GLOBAL_VAR XxY_header   ff_idXt2lex_id_hd, full_ctxtXlex_id_hd, eprod_ctxtXlex_id_hd, prod_ctxtXlex_id_hd, A_ctxtXlex_id_hd;
SX_GLOBAL_VAR double       lex_id2logprobs [], full_ctxtXlex_id2logprobs [], eprod_ctxtXlex_id2logprobs [], prod_ctxtXlex_id2logprobs [],
                    A_ctxtXlex_id2logprobs [];

SX_GLOBAL_VAR XxY_header   ctxtXsign_hd;
SX_GLOBAL_VAR double       ctxtXsign2logprobs [];
SX_GLOBAL_VAR XH_header    fs_signatures_hd;

/* Pour de la semantique qui a besoin des resultats de nbest */
SX_GLOBAL_VAR_NBEST XxY_header        dedication_hd;

struct dedication {
  double total_weight;
  SXINT    PijK, signature, k, next;
};

SX_GLOBAL_VAR_NBEST struct dedication *dedication_list;
SX_GLOBAL_VAR_NBEST XH_header         signatures_hd;
#endif /* nbest_h */
