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
#ifndef fsa_h
#define fsa_h
#include "SXante.h"
#ifndef sxstack_h
/* Pour struct sxdfa_struct */
#include "sxstack.h"
#endif
#ifndef XxY_h
#include "XxY.h"
#endif
#ifndef sxba_h
#include "sxba.h"
#endif
#ifndef XH_h
#include "XH.h"
#endif


#if 0
#define ESSAI_INVERSE_MAPPING
#endif /* 0 */


#ifdef ESSAI_INVERSE_MAPPING
struct inverse_mapping {
  SXINT     init_state, final_state;
  SXINT     *new_state2old_state_set_id;
  XH_header XH_state_set;
};
#endif /* ESSAI_INVERSE_MAPPING */


/* Utilise's ds read_a_re */
#define RE2NFA              (SXINT)1
#define RE2EFNFA            (SXINT)2
#define RE2DFA              (SXINT)4
#define RE_IS_A_DAG         (SXINT)8
#define RE_USE_COMMENTS    (SXINT)16
#define RE_INCREASE_ORDER  (SXINT)32


#define dico_base2char_stack_ptr(cur_dico,base) (cur_dico->base2stack ? \
						    (   cur_dico->base2stack [base] ?	\
						        cur_dico->char_stack_list + cur_dico->base2stack [base] : \
						        (unsigned char *) 1 \
						    ) : NULL)


extern SXINT re_reader (char *pathname_or_string, bool from_file, SXINT (*re_process)(void));
#ifdef sxu2_h
/* Sinon struct sxtables est inconnu */
extern SXINT read_a_re (void (*prelude)(bool, SXINT, SXINT, SXINT, SXINT), 
			void (*store)(SXINT, struct sxtoken **, struct sxtoken **, SXINT, SXINT), 
			SXINT (*postlude)(SXINT), 
			SXINT what_to_do);
#endif /* sxu2_h */

/* Utilise's ds fsa_mngr */
/* Renumerotation "raisonnable" des e'tats d'un FSA (Si DAG on a p ->t q => p < q) */
extern void fsa_normalize (SXINT init_state, 
			   SXINT final_state, 
			   SXINT eof_ste,
			   void (*fsa_forward_trans)(SXINT ,void (*fill_old_state2max_path_lgth)(SXINT, SXINT, SXINT)), 
			   void (*fsa_fill_trans)(SXINT , SXINT, SXINT)
#ifdef ESSAI_INVERSE_MAPPING
			   , SXINT *new_state2old_state
#endif /* ESSAI_INVERSE_MAPPING */
			   );

/* transforme un NFA (avec des transitions vides ou non) en un dfa qui peut etre minimal */
extern void nfa2dfa (SXINT init_state,
		     SXINT final_state,
		     SXINT max_state, /* voir commentaire dans fsa_mngr.c */
		     SXINT eof_ste,
		     bool (*empty_trans)(SXINT, SXBA), 
		     void (*nfa_extract_trans)(SXINT, void (*nfa_fill_trans)(SXINT, SXINT, SXINT) ), 
		     void (*dfa_fill_trans)(SXINT, SXINT, SXINT, bool), 
		     void (*mindfa_fill_trans)(SXINT, SXINT, SXINT), 
		     bool to_be_normalized
#ifdef ESSAI_INVERSE_MAPPING
		     , struct inverse_mapping *inverse_mapping
#endif /* ESSAI_INVERSE_MAPPING */
		     );

/* Transforme un dfa en sa version minimale */
extern void dfa_minimize (SXINT cur_dfa_init_state, 
			  SXINT cur_dfa_final_state, 
			  SXINT eof_ste, 
			  void (*cur_dfa_extract_trans)(SXINT, 
							void (*dfa_fill_dfa_state2pred_stack) (SXINT dfa_state, 
											       SXINT t, 
											       SXINT next_dfa_state)), 
			  void (*mindfa_fill_trans)(SXINT ,SXINT ,SXINT), 
			  bool to_be_normalized
#ifdef ESSAI_INVERSE_MAPPING
			  , struct inverse_mapping *inverse_mapping
#endif /* ESSAI_INVERSE_MAPPING */
			  );

#ifdef varstr_h
extern VARSTR	fsa_to_re (VARSTR varstr_ptr,
			   SXBA *R,
			   SXBA *R_plus,
			   SXINT size,
			   SXINT initial_s,
			   SXINT final_s,
			   SXINT (*get_name_refs) (SXBA_INDEX, SXBA_INDEX, char **));
extern VARSTR	fsa2re (VARSTR varstr_ptr,
			SXBA *R,
			SXBA *R_plus,
			SXINT size,
			SXINT initial_s,
			SXINT final_s,
			SXINT (*get_name_refs) (SXBA_INDEX, SXBA_INDEX, char **), 
			char *op [6]);
extern VARSTR	dag2re (VARSTR varstr_ptr, 
			SXINT dag_init_state, 
			SXINT dag_final_state, 
			SXINT eof_ste, 
			void (*dag_extract_trans) (SXINT, void (*fill_dag_hd) (SXINT p, SXINT t, SXINT q)), 
			char *(*get_trans_name) (SXINT));
#endif /* varstr_h */


/* Essai pour fabriquer + rapidement des dicos qui puissent etre utilises par sxspell
   (mais acces beaucoup + long) */
struct sxdfa_private {
  bool from_left_to_right;

  union {
    SXINT          *SXINT_ptr;
    char           *char_ptr;
  } ptr;
};


struct word_tree_struct {
  SXINT      root, max_final_state_nb, max_path_lgth, max_trans;

  char       *name;
  FILE       *stats;

  XxY_header paths;
  
  SXBA       final_path_set;
  SXINT      *path2id;

  bool    is_static;

  struct sxdfa_private private;
};

struct sxdfa_struct {
  SXINT   init_state, last_state, number_of_out_trans, final_state_nb, max_path_lgth, max_arity, max_t_val;
  char    *name;
  FILE    *stats;

  SXINT *states /* [0..last_state] */ /* C'est une DSTACK */,
    *next_state_list /* [0..trans_nb+last_state] */ /* C'est une DSTACK */,
    *trans_list /* [0..trans_list [0] */;

  bool is_static, is_a_dag;

  struct sxdfa_private private;
};

extern void        word_tree_alloc (struct word_tree_struct *word_tree_ptr, char *name, SXINT word_nb, SXINT word_lgth, SXINT Xforeach, SXINT Yforeach,
				    bool from_left_to_right, bool with_path2id, void (*oflw) (SXINT, SXINT) /* i.e., sxoflw0_t */, FILE *stats);
extern void        word_tree_oflw (struct word_tree_struct *word_tree_ptr, SXINT old_size, SXINT new_size);
extern SXINT       word_tree_add_a_word (struct word_tree_struct *word_tree_ptr, SXINT *input_stack, SXINT id);
extern SXINT       word_tree_add_a_string (struct word_tree_struct *word_tree_ptr, char *string, SXINT string_lgth, SXINT id);
extern SXINT       word_tree_add (struct word_tree_struct *word_tree_ptr, SXUINT (*get_next_symb) (struct sxdfa_private*), SXINT lgth, SXINT id);
extern SXINT       word_tree_seek_a_string (struct word_tree_struct *word_tree_ptr, char *kw, SXINT *kwl);
extern void        word_tree2sxdfa (struct word_tree_struct *word_tree_ptr, struct sxdfa_struct *sxdfa_ptr, char *name, FILE *stats, bool to_be_minimized);
extern void        word_tree_free (struct word_tree_struct *word_tree_ptr);

/* utilisation des sxdfa_struct */
extern void        nfa2sxdfa (SXINT init_state, 
	   SXINT cur_final_state, 
	   SXINT cur_max_state, 
	   SXINT eof_ste, 
	   SXINT trans_nb, 
	   bool (*empty_trans)(SXINT, SXBA), 
	   void (*nfa_extract_trans)(SXINT, void (*sxnfa_fill_trans) (SXINT nfa_state, SXINT t, SXINT next_nfa_state)), 
	   struct sxdfa_struct *sxdfa_ptr, 
	   bool to_be_minimized, 
	   bool to_be_normalized);
extern void        sxdfa_minimize (struct sxdfa_struct *sxdfa_ptr, bool to_be_normalized);
extern void        sxdfa_normalize (struct sxdfa_struct *sxdfa_ptr);

extern void        sxdfa_alloc (struct sxdfa_struct *sxdfa_ptr, SXINT state_nb, SXINT trans_nb, char *name, FILE *stats);
extern void        sxdfa_tree2min_dag (struct sxdfa_struct *sxdfa_ptr); /* en entree io_dag est un arbre, en sortie, il devient un dag minimal */
extern void        sxdfa_extract_trans (struct sxdfa_struct *sxdfa_ptr,
					SXINT state,
					bool (*f)(struct sxdfa_struct *, SXINT, SXINT, SXINT));
extern SXINT       sxdfa_seek_a_string (struct sxdfa_struct *sxdfa_ptr, char *kw, SXINT *kwl);
extern SXINT       sxdfa_seek_a_word (struct sxdfa_struct *sxdfa_ptr, SXINT *input_stack);
extern SXINT       sxdfa_seek (struct sxdfa_struct *sxdfa_ptr, SXUINT (*get_next_symb) (struct sxdfa_private *), SXINT *input_lgth);
extern void        sxdfa_free (struct sxdfa_struct *sxdfa_ptr);
extern void        sxdfa2c (struct sxdfa_struct *sxdfa_ptr, FILE *file, char *name, bool is_static);
#ifdef varstr_h
extern VARSTR	   sxdfadag2re (VARSTR varstr_ptr, struct sxdfa_struct *sxdfa_ptr, char *(*get_trans_name) (SXINT));
#endif /* varstr_h */




struct sxdfa_packed_struct {
  SXUINT   init_state, last_state, number_of_out_trans, final_state_nb, max_path_lgth, max_arity, max_t_val;
  SXUINT   base_shift, stop_mask, t_mask, id_mask;
  char    *name;
  FILE    *stats;

  SXUINT  *delta;

  bool is_static, is_a_dag;

  struct sxdfa_private private;
};


extern void        sxdfa2sxdfa_packed (struct sxdfa_struct *sxdfa_ptr, struct sxdfa_packed_struct *sxdfa_packed_ptr, char *name, FILE *stats); 
extern SXINT       sxdfa_packed_seek_a_string (struct sxdfa_packed_struct *sxdfa_packed_ptr, char *kw, SXINT *kwl);
extern SXINT       sxdfa_packed_seek_a_word (struct sxdfa_packed_struct *sxdfa_packed_ptr, SXINT *input_stack);
extern SXINT       sxdfa_packed_seek (struct sxdfa_packed_struct *sxdfa_packed_ptr,
				      SXUINT (*get_next_symb) (struct sxdfa_private *),
				      SXINT *input_lgth);
extern void        sxdfa_packed2c (struct sxdfa_packed_struct *sxdfa_packed_ptr, FILE *file, char *name, bool is_static);
extern void        sxdfa_packed_free (struct sxdfa_packed_struct *sxdfa_packed_ptr); 



struct sxdfa_comb {
  SXUINT               max;
  SXUINT               init_base;
  SXUINT               class_mask, stop_mask;
  SXUINT 	       base_shift;
  unsigned char        *char2class;
  SXUINT               *comb_vector;

  bool              is_static, is_a_dag;

  SXINT                *base2stack;
  unsigned char        *char_stack_list;
  SXINT                char_stack_list_top;

  char                 *name; /* à remplir par l'utilisateur */
  SXINT                id; /* à remplir par l'utilisateur */

  struct sxdfa_private private;
};


extern void        sxdfa2comb_vector (struct sxdfa_struct *sxdfa_ptr, SXINT optim_kind, SXINT comb_vector_threshold, struct sxdfa_comb *sxdfa_comb_ptr);
extern void        sxdfa_comb2c (struct sxdfa_comb *sxdfa_comb_ptr, FILE *file, char *dico_name, bool is_static);
extern SXINT       sxdfa_comb_seek_a_string (struct sxdfa_comb *sxdfa_comb_ptr, char *kw, SXINT *kwl);
extern SXINT       sxdfa_comb_seek (struct sxdfa_comb *sxdfa_comb_ptr,
				    SXUINT (*get_next_symb) (struct sxdfa_private *),
				    SXINT *input_lgth);
extern void        sxdfa_comb_free (struct sxdfa_comb *sxdfa_comb_ptr);



extern bool nfa_intersection (SXINT nfa1_init_state, 
				   SXINT nfa1_final_state, 
				   SXINT nfa1_max_state,  /* en général, == 0 ; si > 0, on est dans le cas DAG (pas de trans
							     sortant de final_state) ; en contrepartie, on a le droit d'avoir
							     final_state != max_state, et on a le droit d'avoir des transitions
							     vers final_state qui ne sont pas sur eof_ste */
				   SXINT nfa1_eof_ste,
				   SXINT nfa2_init_state, 
				   SXINT nfa2_final_state, 
				   SXINT nfa2_max_state,
				   SXINT nfa2_eof_ste, 
				   bool (*nfa1_empty_trans)(SXINT, SXBA),  
				   bool (*nfa2_empty_trans)(SXINT, SXBA), 
				   void (*nfa1_extract_non_eps_trans)(SXINT, void (*)(SXINT, SXINT, SXINT)),
				   void (*nfa2_extract_non_eps_trans)(SXINT, void (*)(SXINT, SXINT, SXINT)), 
				   void (*nfa_intersection_fill_trans)(SXINT, SXINT, SXINT, bool)
#ifdef ESSAI_INVERSE_MAPPING
				   , struct inverse_mapping *inverse_mapping
#endif /* ESSAI_INVERSE_MAPPING */
				   );

#include "SXpost.h"
#endif /* fsa_h */

