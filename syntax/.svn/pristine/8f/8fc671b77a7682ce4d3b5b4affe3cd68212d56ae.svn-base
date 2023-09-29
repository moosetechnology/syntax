#ifndef lfg_semact_h
#define lfg_semact_h

#include "SXante.h"

SX_GLOBAL_VAR int               Xpq_top;
SX_GLOBAL_VAR int               *fs_id_stack, *fs_id2dynweight;
SX_GLOBAL_VAR SXBA              fs_id_set; /* pour print_f_structure () et uwfs2ufs_id () et ... */
SX_GLOBAL_VAR XH_header         XH_fs_hd, XH_ufs_hd, XH_cyclic_fs_hd, XH_cyclic_ufs_hd;
SX_GLOBAL_VAR int               empty_ufs_id;
SX_GLOBAL_VAR XH_header         dynam_pred_vals; /* Quadruplets : lexeme_id, sous_cat1, sous_cat2, priority */
SX_GLOBAL_VAR XxY_header        heads;
SX_GLOBAL_VAR int               *heads_stack; /* Une DSTACK le 21/06/05 */
SX_GLOBAL_VAR char              *input_sentence_string;
SX_GLOBAL_VAR int               *Pij2disp, *fs_id_Pij_dstack, Pij_cur_bot;
SX_GLOBAL_VAR double            *fs_id_Pij_wstack;

SX_GLOBAL_VAR SXBOOLEAN           is_print_f_structure;
SX_GLOBAL_VAR SXBOOLEAN           is_partial  /* SXTRUE <=> Aucune f_structure a la racine, on a recupere ce qu'on a pu ... */;
SX_GLOBAL_VAR SXBOOLEAN           is_relaxed_run /* positionne si le 1er passage a echoue'.  On en refait un sans la verif de la coherence */;
SX_GLOBAL_VAR SXBOOLEAN           is_consistent /* !is_consistent <=> toutes les f_struct sont incoherentes */;
SX_GLOBAL_VAR SXBOOLEAN           with_ranking;
SX_GLOBAL_VAR SXBOOLEAN           relaxed_run_allowed, is_unique_parse, is_print_output_dag, is_ranking;
SX_GLOBAL_VAR SXBOOLEAN           is_print_final_f_structure_number, is_proba_ranking, is_nbest_ranking;
SX_GLOBAL_VAR int               max_fs_nb, fs_nb_threshold;
SX_GLOBAL_VAR int               old_fs_id_Pij_dstack_size;

#define include_qq_valeurs_declarations 1
#ifdef lfg_disamb_h
#include lfg_disamb_h
#endif /* lfg_disamb_h */
#undef include_qq_valeurs_declarations

#define LOCAL_ARG_SET_SIZE      NBLONGS(MAX_FIELD_ID+1)
SX_GLOBAL_VAR SXBOOLEAN           set_fs_id_signature (int fs_id, int level, SXBOOLEAN (*f)());

SX_GLOBAL_VAR int               seek_prefix_Aij (char* A, int Pij);
SX_GLOBAL_VAR int               seek_Aij (char* A, int Pij);

SX_GLOBAL_VAR int               walk_aij (int aij_val, int (*f) ());
SX_GLOBAL_VAR SXBOOLEAN           fs_is_set (int fs_id, int field_id, int **ret_val);
SX_GLOBAL_VAR int               atom_id2local_atom_id (int atom_id);
SX_GLOBAL_VAR int               get_atomic_field_val (int fs_id, int field_id);
SX_GLOBAL_VAR int               get_fs_id_atom_val (int field);
#ifdef ESSAI
SX_GLOBAL_VAR int               get_ufs_atom_val (int val, SXBOOLEAN *is_shared, SXBA local_atom_id_set); /* Pour output_semact !! */
SX_GLOBAL_VAR SXBA              left_local_atom_id_set;
SX_GLOBAL_VAR X_header          atom_id2local_atom_id_hd;
#endif /* ESSAI */

#endif /* lfg_semact_h */
