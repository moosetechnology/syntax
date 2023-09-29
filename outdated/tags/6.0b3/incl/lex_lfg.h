#ifndef lfg_lex_h
#define lfg_lex_h

#include "SXante.h"
#include "sxba.h"

#include "XH.h"
#include "X.h"

#define SCOPE static

#ifdef def_amlgm_list
struct ac_list {
  SXINT hd, next;
};
#else /* !def_amlgm_list */
#ifdef def_cmpnd_list
struct ac_list {
  SXINT hd, next;
};
#endif /* def_cmpnd_list */
#endif /* !def_amlgm_list */

#ifdef def_attr_vals
struct attr_val {
  SXINT attr, kind, val, next;
};
#endif /* def_attr_vals */

#ifdef def_pred_vals
struct pred_val {
  SXINT lexeme, sous_cat1, sous_cat2;
};
#endif /* def_pred_vals */

#ifdef def_if_id2t_list
struct if_id2t_list {
  SXINT t, list;
};
#endif /* def_if_id2t_list */

#ifdef def_structure_args_list
struct structure_args_list {
  SXINT lexeme, struct_id, priority;
};
#endif /* def_structure_args_list */


#ifdef def_amlgm_list
SCOPE        struct ac_list amlgm_list[];
#endif /* def_amlgm_list */

#ifdef def_cmpnd_list
SCOPE        struct ac_list cmpnd_list[];
#endif /* def_cmpnd_list */

#ifdef def_attr_vals
SCOPE        struct attr_val attr_vals[];
#endif /* def_attr_vals */

#ifdef def_pred_vals
SCOPE        struct pred_val pred_vals[];
#endif /* def_pred_vals */

#ifdef def_if_id2t_list
SCOPE        struct if_id2t_list if_id2t_list[];
#endif /* def_if_id2t_list */

#ifdef def_structure_args_list
SCOPE        struct structure_args_list structure_args_list[];
#endif /* def_structure_args_list */

#if USE_A_DICO

#ifdef def_dico_cmpnd
SCOPE        struct dico dico_cmpnd;
#endif /* def_dico_cmpnd */

#ifdef def_dico_t
SCOPE        struct dico dico_t;
#endif /* def_dico_t */

#ifdef def_dico_if
SCOPE        struct dico dico_if;
#endif /* def_dico_if */
#else /* !USE_A_DICO */

#ifdef def_amalgam_or_compound_component_names
SCOPE        sxword_header amalgam_or_compound_component_names;
#endif /* def_amalgam_or_compound_component_names */

#ifdef def_t_names
SCOPE        sxword_header t_names;
#endif /* def_t_names */

#ifdef def_inflected_form_names
SCOPE        sxword_header inflected_form_names;
#endif /* def_inflected_form_names */
#endif /* !USE_A_DICO */


#ifdef def_inflected_form2display
SCOPE        SXINT inflected_form2display[];
#endif /* def_inflected_form2display */
#if 0

#ifdef def_dico_cmpndcomb_vector
SCOPE        SXUINT dico_cmpndcomb_vector[];
#endif /* def_dico_cmpndcomb_vector */

#ifdef def_dico_cmpndchar2class
SCOPE        unsigned char dico_cmpndchar2class[];
#endif /* def_dico_cmpndchar2class */
#endif

#ifdef def_amalgam_id_set
SCOPE        SXBA_ELT amalgam_id_set[];
#endif /* def_amalgam_id_set */

#ifdef def_compound_component_id_set
SCOPE        SXBA_ELT compound_component_id_set[];
#endif /* def_compound_component_id_set */

#ifdef def_if_id_set
SCOPE        SXBA_ELT if_id_set[];
#endif /* def_if_id_set */

#ifdef def_initial_compound_component_id_set
SCOPE        SXBA_ELT initial_compound_component_id_set[];
#endif /* def_initial_compound_component_id_set */

#ifdef def_final_compound_component_id_set
SCOPE        SXBA_ELT final_compound_component_id_set[];
#endif /* def_final_compound_component_id_set */

#ifdef def_XH_cc_list
SCOPE        XH_header XH_cc_list;
#endif /* def_XH_cc_list */

#ifdef def_ac_id2string
SCOPE        char *ac_id2string[];
#endif /* def_ac_id2string */

#ifdef def_amlgm_component_stack
SCOPE        SXINT amlgm_component_stack[];
#endif /* def_amlgm_component_stack */

#ifdef def_ac_id2amlgm_list
SCOPE        SXINT ac_id2amlgm_list[];
#endif /* def_ac_id2amlgm_list */

#ifdef def_cc2cmpnd_list
SCOPE        SXINT cc2cmpnd_list[];
#endif /* def_cc2cmpnd_list */

#ifdef def_amalgam_list
SCOPE        XH_header amalgam_list;
#endif /* def_amalgam_list */

#ifdef def_amalgam_names
SCOPE        char *amalgam_names[];
#endif /* def_amalgam_names */

#ifdef def_X_amalgam_if_id_hd
SCOPE        X_header X_amalgam_if_id_hd;
#endif /* def_X_amalgam_if_id_hd */
#if 0

#ifdef def_dico_tcomb_vector
SCOPE        SXUINT dico_tcomb_vector[];
#endif /* def_dico_tcomb_vector */

#ifdef def_dico_tchar2class
SCOPE        unsigned char dico_tchar2class[];
#endif /* def_dico_tchar2class */

#ifdef def_dico_ifcomb_vector
SCOPE        SXUINT dico_ifcomb_vector[];
#endif /* def_dico_ifcomb_vector */

#ifdef def_dico_ifchar2class
SCOPE        unsigned char dico_ifchar2class[];
#endif /* def_dico_ifchar2class */
#endif

#ifdef def_if_id2tok_list_id
SCOPE        SXINT if_id2tok_list_id[];
#endif /* def_if_id2tok_list_id */

#ifdef def_tok_disp
SCOPE        SXINT tok_disp[];
#endif /* def_tok_disp */

#ifdef def_tok_list
SCOPE        SXINT tok_list[];
#endif /* def_tok_list */

#ifdef def_tok_str
SCOPE        char *tok_str[];
#endif /* def_tok_str */

#ifdef def_lex_atom_hd
SCOPE        SXINT lex_atom_hd[];
#endif /* def_lex_atom_hd */

#ifdef def_lex_atom_list
SCOPE        SXINT lex_atom_list[];
#endif /* def_lex_atom_list */

#ifdef def_lex_adjunct_hd
SCOPE        SXINT lex_adjunct_hd[];
#endif /* def_lex_adjunct_hd */

#ifdef def_lex_adjunct_list
SCOPE        SXINT lex_adjunct_list[];
#endif /* def_lex_adjunct_list */

#ifdef def_sous_cat
SCOPE        SXINT sous_cat[];
#endif /* def_sous_cat */

#ifdef def_sous_cat_list
SCOPE        SXINT sous_cat_list[];
#endif /* def_sous_cat_list */

#ifdef def_compound_field
SCOPE        SXINT compound_field[];
#endif /* def_compound_field */

#ifdef def_compound_field_list
SCOPE        SXINT compound_field_list[];
#endif /* def_compound_field_list */

#ifdef def_argument_set
SCOPE        SXBA_ELT argument_set[];
#endif /* def_argument_set */

#ifdef def_args_ref_list
SCOPE        SXINT args_ref_list[];
#endif /* def_args_ref_list */

#ifdef def_field_id2atom_field_id
SCOPE        SXINT *field_id2atom_field_id[];
#endif /* def_field_id2atom_field_id */

#ifdef def_field_id2kind
SCOPE        unsigned char field_id2kind[];
#endif /* def_field_id2kind */

#ifdef def_field_id2string
SCOPE        char *field_id2string[];
#endif /* def_field_id2string */

#ifdef def_field_id2pass_nb
SCOPE        SXINT field_id2pass_nb[];
#endif /* def_field_id2pass_nb */

#ifdef def_atom_id2string
SCOPE        char *atom_id2string[];
#endif /* def_atom_id2string */

#ifdef def_lexeme_id2string
SCOPE        char *lexeme_id2string[];
#endif /* def_lexeme_id2string */

#endif /* lfg_lex_h */
