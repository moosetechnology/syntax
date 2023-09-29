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


/* Utilise's ds read_a_re */
#define RE2NFA              (SXINT)1
#define RE2EFNFA            (SXINT)2
#define RE2DFA              (SXINT)4
#define RE_IS_A_DAG         (SXINT)8
#define RE_USE_COMMENTS    (SXINT)16
#define RE_INCREASE_ORDER  (SXINT)32

extern int re_reader (char *pathname_or_string, BOOLEAN from_file, int (*re_process)(void));
#ifdef sxu2_h
/* Sinon struct sxtables est inconnu */
extern int read_a_re (void (*prelude)(BOOLEAN, SXINT, SXINT, SXINT, SXINT), 
			void (*store)(SXINT, struct sxtoken **, SXINT, SXINT), 
			void (*postlude)(SXINT), 
			SXINT what_to_do);
#endif /* sxu2_h */

/* Utilise's ds fsa_mngr */
/* Renumerotation "raisonnable" des e'tats d'un FSA (Si DAG on a p ->t q => p < q) */
extern void fsa_normalize (SXINT init_state, 
			   SXINT final_state, 
			   SXINT eof_ste,
			   void (*fsa_forward_trans)(SXINT ,void (*fill_old_state2max_path_lgth)(SXINT, SXINT, SXINT)), 
			   void (*fsa_fill_trans)(SXINT , SXINT, SXINT));

/* transforme un NFA (avec des transitions vides ou non) en un dfa qui peut etre minimal */
extern void nfa2dfa (SXINT init_state,
		     SXINT final_state,
		     SXINT eof_ste,
		     BOOLEAN (*empty_trans)(SXINT, SXBA), 
		     void (*nfa_extract_trans)(SXINT, void (*nfa_fill_trans)(SXINT, SXINT, SXINT) ), 
		     void (*dfa_fill_trans)(SXINT, SXINT, SXINT, BOOLEAN), 
		     void (*mindfa_fill_trans)(SXINT, SXINT, SXINT), 
		     BOOLEAN to_be_normalized);

/* Transforme un dfa en sa version minimale */
extern void dfa_minimize (SXINT cur_dfa_init_state, 
			  SXINT cur_dfa_final_state, 
			  SXINT eof_ste, 
			  void (*cur_dfa_extract_trans)(SXINT, 
							void (*dfa_fill_dfa_state2pred_stack) (SXINT dfa_state, 
											       SXINT t, 
											       SXINT next_dfa_state)), 
			  void (*mindfa_fill_trans)(SXINT ,SXINT ,SXINT), 
			  BOOLEAN to_be_normalized);

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
  BOOLEAN from_left_to_right;

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

  BOOLEAN    is_static;

  struct sxdfa_private private;
};

struct sxdfa_struct {
  SXINT   init_state, last_state, number_of_out_trans, final_state_nb, max_path_lgth, max_arity, max_t_val;
  char    *name;
  FILE    *stats;

  SXINT *states /* [0..last_state] */ /* C'est une DSTACK */,
    *next_state_list /* [0..trans_nb+last_state] */ /* C'est une DSTACK */,
    *trans_list /* [0..trans_list [0] */;

  BOOLEAN is_static, is_a_dag;

  struct sxdfa_private private;
};

extern void        word_tree_alloc (struct word_tree_struct *word_tree_ptr, char *name, SXINT word_nb, SXINT word_lgth, SXINT Xforeach, SXINT Yforeach,
				    BOOLEAN from_left_to_right, BOOLEAN with_path2id, void (*oflw) (SXINT, SXINT), FILE *stats);
extern void        word_tree_oflw (struct word_tree_struct *word_tree_ptr, SXINT old_size, SXINT new_size);
extern SXINT       word_tree_add_a_word (struct word_tree_struct *word_tree_ptr, SXINT *input_stack, SXINT id);
extern SXINT       word_tree_add_a_string (struct word_tree_struct *word_tree_ptr, char *string, SXINT string_lgth, SXINT id);
extern SXINT       word_tree_add (struct word_tree_struct *word_tree_ptr, SXUINT (*get_next_symb) (struct sxdfa_private*), SXINT lgth, SXINT id);
extern SXINT       word_tree_seek_a_string (struct word_tree_struct *word_tree_ptr, char *kw, SXINT *kwl);
extern void        word_tree2sxdfa (struct word_tree_struct *word_tree_ptr, struct sxdfa_struct *sxdfa_ptr, char *name, FILE *stats, BOOLEAN to_be_minimized);
extern void        word_tree_free (struct word_tree_struct *word_tree_ptr);

/* utilisation des sxdfa_struct */
extern void        nfa2sxdfa (SXINT init_state, 
	   SXINT cur_final_state, 
	   SXINT eof_ste, 
	   SXINT trans_nb, 
	   BOOLEAN (*empty_trans)(SXINT, SXBA), 
	   void (*nfa_extract_trans)(SXINT, void (*sxnfa_fill_trans) (SXINT nfa_state, SXINT t, SXINT next_nfa_state)), 
	   struct sxdfa_struct *sxdfa_ptr, 
	   BOOLEAN to_be_minimized, 
	   BOOLEAN to_be_normalized);
extern void        sxdfa_minimize (struct sxdfa_struct *sxdfa_ptr, BOOLEAN to_be_normalized);
extern void        sxdfa_normalize (struct sxdfa_struct *sxdfa_ptr);

extern void        sxdfa_alloc (struct sxdfa_struct *sxdfa_ptr, SXINT state_nb, SXINT trans_nb, char *name, FILE *stats);
extern void        sxdfa_tree2min_dag (struct sxdfa_struct *sxdfa_ptr); /* en entree io_dag est un arbre, en sortie, il devient un dag minimal */
extern void        sxdfa_extract_trans (struct sxdfa_struct *sxdfa_ptr,
					SXINT state,
					BOOLEAN (*f)(struct sxdfa_struct *, SXINT, SXINT, SXINT));
extern SXINT       sxdfa_seek_a_string (struct sxdfa_struct *sxdfa_ptr, char *kw, SXINT *kwl);
extern SXINT       sxdfa_seek_a_word (struct sxdfa_struct *sxdfa_ptr, SXINT *input_stack);
extern SXINT       sxdfa_seek (struct sxdfa_struct *sxdfa_ptr, SXUINT (*get_next_symb) (struct sxdfa_private *), SXINT *input_lgth);
extern void        sxdfa_free (struct sxdfa_struct *sxdfa_ptr);
extern void        sxdfa2c (struct sxdfa_struct *sxdfa_ptr, FILE *file, char *name, BOOLEAN is_static);
#ifdef varstr_h
extern VARSTR	   sxdfadag2re (VARSTR varstr_ptr, struct sxdfa_struct *sxdfa_ptr, char *(*get_trans_name) (SXINT));
#endif /* varstr_h */




struct sxdfa_packed_struct {
  SXUINT   init_state, last_state, number_of_out_trans, final_state_nb, max_path_lgth, max_arity, max_t_val;
  SXUINT   base_shift, stop_mask, t_mask, id_mask;
  char    *name;
  FILE    *stats;

  SXUINT  *delta;

  BOOLEAN is_static, is_a_dag;

  struct sxdfa_private private;
};


extern void        sxdfa2sxdfa_packed (struct sxdfa_struct *sxdfa_ptr, struct sxdfa_packed_struct *sxdfa_packed_ptr, char *name, FILE *stats); 
extern SXINT       sxdfa_packed_seek_a_string (struct sxdfa_packed_struct *sxdfa_packed_ptr, char *kw, SXINT *kwl);
extern SXINT       sxdfa_packed_seek_a_word (struct sxdfa_packed_struct *sxdfa_packed_ptr, SXINT *input_stack);
extern SXINT       sxdfa_packed_seek (struct sxdfa_packed_struct *sxdfa_packed_ptr,
				      SXUINT (*get_next_symb) (struct sxdfa_private *),
				      SXINT *input_lgth);
extern void        sxdfa_packed2c (struct sxdfa_packed_struct *sxdfa_packed_ptr, FILE *file, char *name, BOOLEAN is_static);
extern void        sxdfa_packed_free (struct sxdfa_packed_struct *sxdfa_packed_ptr); 



struct sxdfa_comb {
  SXUINT               max;
  SXUINT               init_base;
  SXUINT               class_mask, stop_mask;
  SXUINT 	       base_shift;
  unsigned char        *char2class;
  SXUINT               *comb_vector;

  BOOLEAN              is_static, is_a_dag;

  struct sxdfa_private private;
};


extern void        sxdfa2comb_vector (struct sxdfa_struct *sxdfa_ptr, SXINT optim_kind, SXINT comb_vector_threshold, struct sxdfa_comb *sxdfa_comb_ptr);
extern void        sxdfa_comb2c (struct sxdfa_comb *sxdfa_comb_ptr, FILE *file, char *dico_name, BOOLEAN is_static);
extern SXINT       sxdfa_comb_seek_a_string (struct sxdfa_comb *sxdfa_comb_ptr, char *kw, SXINT *kwl);
extern SXINT       sxdfa_comb_seek (struct sxdfa_comb *sxdfa_comb_ptr,
				    SXUINT (*get_next_symb) (struct sxdfa_private *),
				    SXINT *input_lgth);
extern void        sxdfa_comb_free (struct sxdfa_comb *sxdfa_comb_ptr);


#include "SXpost.h"
#endif /* fsa_h */

/*
 * Local Variables:
 * mode: C
 * version-control: yes
 * End:
 */
