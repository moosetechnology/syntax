/* **********************************************************************
   *									*
   *									*
   * Copyright (c) 1986 by Institut National de Recherche               *
   *			en Informatique et en Automatique		*
   *									*
   *									*
   ********************************************************************** */




/* **********************************************************************
   *							  		*
   *	 Produit de l'equipe Langages et Traducteurs.	            	*
   *							  		*
   ********************************************************************** */



/*   L E C L _ A G . H   */


/************************** C O N S T A N T S *******************************/


#define xactmin 		(-3)
#define prdct_true_code 	(1)
#define prdct_false_code 	(2)

#define NONDETER	64
#define MIXTE   	32
#define LA      	16
#define PRDCT    	8
#define ACT      	4
#define SHIFT    	2
#define REDUCE   	1

#define KW_NOT_		01
#define KW_EXCLUDE_	02
#define KW_LIST_	04

#define Shift_Reduce   1
#define Reduce_Shift   2
#define Reduce_Reduce  4

/* Le 24/01/2003 changement pour les structures internes des constructeurs 
typedef unsigned short transition_matrix_item;
*/
typedef unsigned int transition_matrix_item;

/*************************** E X T E R N S ***********************************/

int	lgt [5];

char	**err_titles,
	*prgentname;

long    options_set;

BOOLEAN	are_comments_defined,
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

int	abbrev_size,
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

VARSTR	check_kw;

/****************************** T Y P E S ************************************/

struct abbrev_item {
   struct lecl_node *subtree_ptr;
   int pront, prdct_no;
   BOOLEAN is_empty, is_used;
   };


struct action_or_prdct_code_item {
   transition_matrix_item	stmt;
   int 			        action_or_prdct_no, param;
   BOOLEAN			is_system;
   char				kind;
   };



struct ers_disp_item {
   struct lecl_node *subtree_ptr;
   int prolon, reduc, pexcl, pkw, component_no, master_token, post_action;
   struct sxsource_coord lex_name_source_index;
   BOOLEAN restricted_context, is_a_user_defined_context, is_unbounded_context, is_keywords_spec;
   char /* bit(3) */ kw_kind;
   char /* bit(3) */ priority_kind;
   };


struct ers_item {
   int lispro, prdct_no, prolis;
   struct sxsource_coord liserindx;
   BOOLEAN is_erased;
   };


struct esc_to_sc_prdct_item {
   int simple_class, predicate;
   };


struct exclude_item {
   int name, component;
   struct sxsource_coord source_index;
   };


struct fsa_item {
   int item_ref, transition_ref, seed_state, shift_state, else_prdct, lnk, prod_no;
   char state_kind /* bit (6) */;
   BOOLEAN del, scan;
   };


struct fsa_trans_item {
   int		cc_ref, next_state_no;
   BOOLEAN	is_del, is_scan;
   };


struct item_attributes {
   int origine_index, ctxt_no;
   char /* bit(2) */ kind;
   };


struct keywords_item {
   int string_table_entry, t_code;
   };


struct lr_marker {
   int index;
   struct item_attributes attributes;
   };


struct synonym_item {
   int string_table_entry;
   struct sxsource_coord source_index;
   };



/*****************************************************************************/


/*	L G T [ 1 ]	*/
   struct lr_marker *substate;
   struct lr_marker *state;

/*	L G T [ 2 ]	*/
   struct fsa_item *fsa;

/*	L G T [ 3 ]	*/
   struct lr_marker *item;

/*	L G T [ 4 ]	*/
   SXBA /* smax */ *compound_classes;
   struct fsa_trans_item *fsa_trans;


/* ************************************************************************ */

/*	A B B R E V _ S I Z E	*/
   struct abbrev_item *abbrev /* -abbrev_size:-1 */ ;

/*	A B B R E V _ S I Z E : C L A S S _ S I Z E	*/
   int *abbrev_or_class_to_ate /* -abbrev_size:class_size */ ;

/*	A C T I O N _ S I Z E : P R E D I C A T E _ S I Z E	*/
   int *action_or_prdct_to_ate /* -action_size:predicate_size */ ;

/*      P R E D I C A T E _ S I Z E	*/
   int *prdct_to_ers /* 0:predicate_size */ ;

/*      S M A X    */
   SXBA /* smax */ class_set ;

/*      M A X _ R E     */
   SXBA /* max_re */ is_re_generated ;

/*      C M A X   -   S M A X    */
   SXBA /* smax */ *kod /* 0..cmax */ ;

/*    C L A S S _ S I Z E   */
   int *not_declared /* class_size */ ;
   SXBA /* char_max */ *class_to_char_set /* 0:class_size */ ;
   SXBA /* class_size */ is_class_used_in_re ;

/*	E R S _ S I Z E		*/
   struct ers_item *ers /* 1:ers_size */ ;

/*	E R S _ S I Z E   -   M A X _ R E _ L G T H		*/
   SXBA /* max_re_lgth */ *suivant /* 1:ers_size */ ;

/*	T O K E N _ S I Z E		*/
   struct ers_disp_item *ers_disp /* 1:token_size */ ;
   struct keywords_item *keywords /* 1:token_size */ ;

/*	T O K E N _ S I Z E   -   T E R M A X		*/
   SXBA /* termax */ *RE_to_T /* 1:token_size */ ;

/*	E X C L U D E _ S I Z E	*/
   struct exclude_item *exclude /* 1:exclude_size */ ;

/*	K E Y W O R D _ S I Z E	*/
   int *keyword /* 1:keyword_size */ ;

/*           S M A X   -   C H A R _ M A X          */
   SXBA /* char_max */ *sc_to_char_set /* 1:smax */ ;
   SXBA /* char_max */ *new_sc_to_char_set /* 1:smax */ ;

/*    M A X _ N O D E _ N O _ I N _ R E   -   M A X _ R E _ L G T H   */
   SXBA /* max_re_lgth */ *tfirst /* 1:max_node_no_in_re */ ;
   SXBA /* max_re_lgth */ *tnext /* 1:max_node_no_in_re */ ;

/*   T E R M A X   */
   int *t_to_ate /* 0:termax */ ;
   SXBA /* termax */ is_a_keyword /* init "0"b */ ;
   SXBA /* termax */ t_is_defined_by_a_token /* init "0"b */ ;
   SXBA /* termax */ *follow ;
   SXBA /* termax */ is_a_generic_terminal ;

/*   A T E _ T O _ T _ T O P   */
   int *ate_to_t /* 0:ate_to_t_top */ ;

/*   T A R G E T _ C O L L A T E _ L E N G T H   */
   int *classe /* 0:target_collate_length - 1 */ ;
   int *target_code /* 0:target_collate_length - 1 */ /* init ((target_collate_length) 1) */ ;
   char *target_collate /* 0:target_collate_length - 1 */ /* init (collate ()) */ ;

/*   C H A R _ M A X   */
   int *simple_classe /* 0:char_max */ ;

/*   C T X T _ S I Z E   -   M A X _ R E   */
   SXBA /* max_re */ *ctxt /* 1:ctxt_size */ ;

/*   L A S T _ S T A T E _ N O : S M A X   */
   transition_matrix_item **transition_matrix /* 1:last_state_no, 1:Smax */ ;

/*   X P R O D   */
   struct action_or_prdct_code_item *action_or_prdct_code /* 1:xprod */ ;

/*   S Y N O N Y M _ L I S T _ N O   */
   int *synonym_list /* 1:synonym_list_no */ ;

/*   S Y N O N Y M _ N O   */
   struct synonym_item *synonym /* 1:synonym_no */ ;

/*   A T E _ T O _ T O K E N _ N O _ T O P   */
   int *ate_to_token_no /* 1:ate_to_token_no_top */ ;

/*   S M A X : X P R D C T _ T O _ A T E   */
   int **sc_prdct_to_esc /* 1:smax, 0:xprdct_to_ate */ ;
   struct esc_to_sc_prdct_item *esc_to_sc_prdct /* 1:smax*xprdct_to_ate */ ;


/* end lecl_ag.h */

