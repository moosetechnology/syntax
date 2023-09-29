#define ppt_num_version 2

struct PP_constants {
	int PP_indx_lgth, PP_schema_lgth;
	};

struct PP_ag_item {
	long		bnf_modif_time;

	struct PP_constants	PP_constants;

	int		*PP_indx;		/* 1:PP_indx_lgth */

	struct SXPP_schema	*SXPP_schema;	/* 1:PP_schema_lgth */
	};

extern BOOLEAN paradis_write(struct PP_ag_item *PP_ag, char *langname);
extern VOID    paradis_free (struct PP_ag_item *PP_ag);
