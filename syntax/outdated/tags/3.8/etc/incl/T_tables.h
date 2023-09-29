#define att_num_version 2

struct T_constants {
	int	T_nbpro,
		T_nbnod,
		T_stack_schema_size,
		T_node_name_string_lgth,
		T_ter_to_node_name_size;
	};

struct T_ag_item {
	long		bnf_modif_time;

	struct T_constants	T_constants;

	struct SXT_node_info	*SXT_node_info;	/* 1:T_nbpro+1 */

	int		*T_stack_schema,	/* 1:T_stack_schema_size */
			*T_nns_indx,	/* 0:T_nbnod+1 */
			*T_ter_to_node_name;/* 0:T_ter_to_node_name_size */

	char		*T_node_name_string;
	};
