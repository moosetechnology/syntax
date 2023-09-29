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

/*   L E C L _ A G . H   */


/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR_LECL : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_LECL  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_LECL /* Inutile de le faire plusieurs fois */
#ifdef SX_DFN_EXT_VAR_LECL
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#define SX_GLOBAL_VAR_LECL	/*rien*/
#define SX_INIT_VAL_LECL(v)	= v
#else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#define SX_GLOBAL_VAR_LECL	extern
#define SX_INIT_VAL_LECL(v)	/*rien*/
#endif /* #ifndef SX_DFN_EXT_VAR_LECL */
#endif /* #ifndef SX_GLOBAL_VAR_LECL */


/************************** C O N S T A N T S *******************************/


#define xactmin 		((SXINT)-3)
#define prdct_true_code 	((SXINT)1)
#define prdct_false_code 	((SXINT)2)

#define NONDETER	((SXINT)64)
#define MIXTE   	((SXINT)32)
#define LA      	((SXINT)16)
#define PRDCT    	((SXINT)8)
#define ACT      	((SXINT)4)
#define SHIFT    	((SXINT)2)
#define REDUCE   	((SXINT)1)

#define KW_NOT_		((SXINT)01)
#define KW_EXCLUDE_	((SXINT)02)
#define KW_LIST_	((SXINT)04)

#define Shift_Reduce   ((SXINT)1)
#define Reduce_Shift   ((SXINT)2)
#define Reduce_Reduce  ((SXINT)4)

/* Le 24/01/2003 changement pour les structures internes des constructeurs 
typedef unsigned short transition_matrix_item;
*/
typedef SXUINT transition_matrix_item;

/*************************** E X T E R N S ***********************************/

SX_GLOBAL_VAR_LECL SXINT	lgt [5];

SX_GLOBAL_VAR_LECL char	**err_titles,
	*prgentname;

SX_GLOBAL_VAR_LECL SXINT    options_set;

SX_GLOBAL_VAR_LECL SXBOOLEAN	are_comments_defined,
	are_comments_erased,
	fsa_done,
	is_1la,
	is_4_char_max,
	is_check,
	is_fsa_construction_needed,
	is_list,
	is_object,
	is_optimize,
        is_dico,
        is_non_deterministic_automaton,
	is_ppt,
	is_predicate_code,
	is_sem,
	is_source,
	is_synonym_list,
	is_table,
	st_done,
	has_nondeterminism,
	sxttycol1p;

SX_GLOBAL_VAR_LECL SXINT	abbrev_size,
	action_size,
	ate_to_token_no_top,
	ate_to_t_top,
	byte_length,
	char_max,
	class_size,
	cmax,
	comments_re_no,
	counters_size,
	ctxt_size,
	current_abbrev_no,
	current_re_no,
	current_token_no,
	ers_size,
	exclude_size,
	keyword_size,
	false_ate,
	include_re_no,
	last_esc,
	last_state_no,
	last_static_ste,
	max_bucket_no,
	max_la,
	max_node_no_in_re,
	max_re,
	max_re_lgth,
	max_t_code_name_lgth,
	nbndef,
	post_action_no,
	predicate_size,
	release_code,
	Smax,
	smax,
	synonym_list_no,
	synonym_no,	
	syno_kw_string_lgth,
	target_collate_length,
	terlis_lgth,
	termax,
	token_size,
	true_ate,
	word_length,
	xactmax,
	xnondetermax,
	xeq,
	xexclude,
	xkw,
	xfsa2,
	xfsa_t,
	xlis,
	xnd,
	xprdct_to_ate,
	xprod,
	x_ctxt;

SX_GLOBAL_VAR_LECL VARSTR	check_kw;

/****************************** T Y P E S ************************************/

struct abbrev_item {
   struct lecl_node *subtree_ptr;
   SXINT pront, prdct_no;
   SXBOOLEAN is_empty, is_used;
   };


struct action_or_prdct_code_item {
   transition_matrix_item	stmt;
   SXINT 			        action_or_prdct_no, param;
   SXBOOLEAN			is_system;
   char				kind;
   };



struct ers_disp_item {
   struct lecl_node *subtree_ptr;
   SXINT prolon, reduc, pexcl, pkw, component_no, master_token, post_action;
   struct sxsource_coord lex_name_source_index;
   SXBOOLEAN restricted_context, is_a_user_defined_context, is_unbounded_context, is_keywords_spec;
   char /* bit(3) */ kw_kind;
   char /* bit(3) */ priority_kind;
   };


struct ers_item {
   SXINT lispro, prdct_no, prolis;
   struct sxsource_coord liserindx;
   SXBOOLEAN is_erased;
   };


struct esc_to_sc_prdct_item {
   SXINT simple_class, predicate;
   };


struct exclude_item {
   SXINT name, component;
   struct sxsource_coord source_index;
   };


struct fsa_item {
   SXINT item_ref, transition_ref, seed_state, shift_state, else_prdct, lnk, prod_no;
   char state_kind /* bit (7) */;
   SXBOOLEAN del, scan;
   };


struct fsa_trans_item {
   SXINT		cc_ref, next_state_no;
   SXBOOLEAN	is_del, is_scan;
   };


struct item_attributes {
   SXINT origine_index, ctxt_no;
   char /* bit(2) */ kind;
   };


struct keywords_item {
   SXINT string_table_entry, t_code;
   };


struct lr_marker {
   SXINT index;
   struct item_attributes attributes;
   };


struct synonym_item {
   SXINT string_table_entry;
   struct sxsource_coord source_index;
   };


/*   L E C L _ C O N F L I C T _ R E C O R D  */

struct detection_record {
    struct detection_record	*next;
    SXINT		index;
};
struct detection_item {
    SXINT		orig;
    struct detection_record	*detection_ptr;
};
struct conflict_record {
    struct conflict_record	*lnk;
    SXINT		seed_state, detection_state, choice, origs_number;
    char	kind /* bit (5) */ ;
    struct detection_item	*detection_items;
};

/*    E N D   L E C L _ C O N F L I C T _ R E C O R D   */


/*****************************************************************************/


/*	L G T [ 1 ]	*/
SX_GLOBAL_VAR_LECL    struct lr_marker *substate;
SX_GLOBAL_VAR_LECL    struct lr_marker *state;

/*	L G T [ 2 ]	*/
SX_GLOBAL_VAR_LECL    struct fsa_item *fsa;

/*	L G T [ 3 ]	*/
SX_GLOBAL_VAR_LECL    struct lr_marker *item;

/*	L G T [ 4 ]	*/
SX_GLOBAL_VAR_LECL    SXBA /* smax */ *compound_classes;
SX_GLOBAL_VAR_LECL    struct fsa_trans_item *fsa_trans;


/* ************************************************************************ */

/*	A B B R E V _ S I Z E	*/
SX_GLOBAL_VAR_LECL    struct abbrev_item *abbrev /* -abbrev_size:-1 */ ;

/*	A B B R E V _ S I Z E : C L A S S _ S I Z E	*/
SX_GLOBAL_VAR_LECL    SXINT *abbrev_or_class_to_ate /* -abbrev_size:class_size */ ;

/*	A C T I O N _ S I Z E : P R E D I C A T E _ S I Z E	*/
SX_GLOBAL_VAR_LECL    SXINT *action_or_prdct_to_ate /* -action_size:predicate_size */ ;

/*      P R E D I C A T E _ S I Z E	*/
SX_GLOBAL_VAR_LECL    SXINT *prdct_to_ers /* 0:predicate_size */ ;

/*      S M A X    */
SX_GLOBAL_VAR_LECL    SXBA /* smax */ class_set ;

/*      M A X _ R E     */
SX_GLOBAL_VAR_LECL    SXBA /* max_re */ is_re_generated ;

/*      C M A X   -   S M A X    */
SX_GLOBAL_VAR_LECL    SXBA /* smax */ *kod /* 0..cmax */ ;

/*    C L A S S _ S I Z E   */
SX_GLOBAL_VAR_LECL    SXINT *not_declared /* class_size */ ;
SX_GLOBAL_VAR_LECL    SXBA /* char_max */ *class_to_char_set /* 0:class_size */ ;
SX_GLOBAL_VAR_LECL    SXBA /* class_size */ is_class_used_in_re ;

/*	E R S _ S I Z E		*/
SX_GLOBAL_VAR_LECL    struct ers_item *ers /* 1:ers_size */ ;

/*	E R S _ S I Z E   -   M A X _ R E _ L G T H		*/
SX_GLOBAL_VAR_LECL    SXBA /* max_re_lgth */ *suivant /* 1:ers_size */ ;

/*	T O K E N _ S I Z E		*/
SX_GLOBAL_VAR_LECL    struct ers_disp_item *ers_disp /* 1:token_size */ ;
SX_GLOBAL_VAR_LECL    struct keywords_item *keywords /* 1:token_size */ ;

/*	T O K E N _ S I Z E   -   T E R M A X		*/
SX_GLOBAL_VAR_LECL    SXBA /* termax */ *RE_to_T /* 1:token_size */ ;

/*	E X C L U D E _ S I Z E	*/
SX_GLOBAL_VAR_LECL    struct exclude_item *exclude /* 1:exclude_size */ ;

/*	K E Y W O R D _ S I Z E	*/
SX_GLOBAL_VAR_LECL    SXINT *keyword /* 1:keyword_size */ ;

/*           S M A X   -   C H A R _ M A X          */
SX_GLOBAL_VAR_LECL    SXBA /* char_max */ *sc_to_char_set /* 1:smax */ ;
SX_GLOBAL_VAR_LECL    SXBA /* char_max */ *new_sc_to_char_set /* 1:smax */ ;

/*    M A X _ N O D E _ N O _ I N _ R E   -   M A X _ R E _ L G T H   */
SX_GLOBAL_VAR_LECL    SXBA /* max_re_lgth */ *tfirst /* 1:max_node_no_in_re */ ;
SX_GLOBAL_VAR_LECL    SXBA /* max_re_lgth */ *tnext /* 1:max_node_no_in_re */ ;

/*   T E R M A X   */
SX_GLOBAL_VAR_LECL    SXINT *t_to_ate /* 0:termax */ ;
SX_GLOBAL_VAR_LECL    SXBA /* termax */ is_a_keyword /* init "0"b */ ;
SX_GLOBAL_VAR_LECL    SXBA /* termax */ t_is_defined_by_a_token /* init "0"b */ ;
SX_GLOBAL_VAR_LECL    SXBA /* termax */ *follow ;
SX_GLOBAL_VAR_LECL    SXBA /* termax */ is_a_generic_terminal ;
SX_GLOBAL_VAR_LECL    SXBA /* termax */ zombies_tset ;

/*   A T E _ T O _ T _ T O P   */
SX_GLOBAL_VAR_LECL    SXINT *ate_to_t /* 0:ate_to_t_top */ ;

/*   T A R G E T _ C O L L A T E _ L E N G T H   */
SX_GLOBAL_VAR_LECL    SXINT *classe /* 0:target_collate_length - 1 */ ;
SX_GLOBAL_VAR_LECL    SXINT *target_code /* 0:target_collate_length - 1 */ /* init ((target_collate_length) 1) */ ;
SX_GLOBAL_VAR_LECL    SXINT *target_collate /* 0:target_collate_length - 1 */ /* init (collate ()) */ ;

/*   C H A R _ M A X   */
SX_GLOBAL_VAR_LECL    SXINT *simple_classe /* 0:char_max */ ;

/*   C T X T _ S I Z E   -   M A X _ R E   */
SX_GLOBAL_VAR_LECL    SXBA /* max_re */ *ctxt /* 1:ctxt_size */ ;

/*   L A S T _ S T A T E _ N O : S M A X   */
SX_GLOBAL_VAR_LECL    transition_matrix_item **transition_matrix /* 1:last_state_no, 1:Smax */ ;

/*   X P R O D   */
SX_GLOBAL_VAR_LECL    struct action_or_prdct_code_item *action_or_prdct_code /* 1:xprod */ ;

/*   S Y N O N Y M _ L I S T _ N O   */
SX_GLOBAL_VAR_LECL    SXINT *synonym_list /* 1:synonym_list_no */ ;

/*   S Y N O N Y M _ N O   */
SX_GLOBAL_VAR_LECL    struct synonym_item *synonym /* 1:synonym_no */ ;

/*   A T E _ T O _ T O K E N _ N O _ T O P   */
SX_GLOBAL_VAR_LECL    SXINT *ate_to_token_no /* 1:ate_to_token_no_top */ ;

/*   S M A X : X P R D C T _ T O _ A T E   */
SX_GLOBAL_VAR_LECL    SXINT **sc_prdct_to_esc /* 1:smax, 0:xprdct_to_ate */ ;
SX_GLOBAL_VAR_LECL    struct esc_to_sc_prdct_item *esc_to_sc_prdct /* 1:smax*xprdct_to_ate */ ;


/* end lecl_ag.h */

