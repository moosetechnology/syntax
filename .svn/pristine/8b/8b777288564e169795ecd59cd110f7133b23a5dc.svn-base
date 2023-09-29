/*  L E C L _ T A B L E S	 */


/* Le 24/01/2003 changement pour les structures internes des constructeurs 
#define st_num_version 7
typedef unsigned short transition_matrix_s;
*/
#define st_num_version 8
typedef SXUINT transition_matrix_s;

struct S_tbl_size_s {
	SXINT	S_constructor_kind, 
		S_last_char_code,
		S_last_simple_class_no,
		S_last_state_no,
		S_last_static_ste,
		S_terlis_lgth,
		S_check_kw_lgth,
		S_termax,
		S_xprod, 
		S_counters_size;
	
	BOOLEAN S_are_comments_erased,
		S_is_user_action_or_prdct,
		S_is_non_deterministic;
	};

struct action_or_prdct_code_s {
	transition_matrix_s    stmt;

	SXINT	action_or_prdct_no,
		param;

	BOOLEAN	is_system;
	char	kind;
	};

struct S_adrp_s {
	SXINT	xterlis,
		syno_lnk,
		lgth;
	};

struct lecl_tables_s {
	long					bnf_modif_time;

	struct S_tbl_size_s			S_tbl_size;

	unsigned char				*S_char_to_simple_class;

	transition_matrix_s	   		**S_transition_matrix;

	struct action_or_prdct_code_s		*SXS_action_or_prdct_code;

	struct S_adrp_s				*SXS_adrp;

	char					*S_terlis;

	char					*S_check_kw;

	SXBA				 	S_is_a_generic_terminal,
						S_is_a_keyword;

	};

