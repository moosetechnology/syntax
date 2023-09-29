/*----------------*/
/*    B_tables    */
/*----------------*/

/*
  Version 6 :	Suppression des champs lgprod et tnbt de WS_NBPRO
                Ajout de actpro
		Ajout de RULE_TO_REDUCE, FIRST, NULLABLE
*/
#define bt_num_version 6


struct ws_tbl_size_s {
    int           nbpro, actpro, xnbpro, tmax, xtmax, ntmax, xntmax, maxnpd, maxtpd,
                  indpro, t_string_length, nt_string_length, sem_kind;
};

struct ws_nbpro_s {
    int           prolon, reduc, numpg, action;
    BOOLEAN       bprosimpl;
};

struct ws_indpro_s {
    int           lispro, prolis;
};

struct xv_to_v_prdct_s {
    int           v_code, prdct_no;
};

struct ws_ntmax_s {
    int           npg, npd;
};

struct bnf_ag_item {
    long          bnf_modif_time;
    struct ws_tbl_size_s WS_TBL_SIZE;
    struct ws_nbpro_s	*WS_NBPRO;			/* 0:xnbpro+1		 */
    struct ws_indpro_s	*WS_INDPRO;			/* 0:indpro		 */
    struct xv_to_v_prdct_s	*XT_TO_T_PRDCT;		/* 0:xtmax+tmax	 	 */
    struct xv_to_v_prdct_s	*XNT_TO_NT_PRDCT;	/* 0:xntmax-ntmax	 */
    struct ws_ntmax_s	*WS_NTMAX;			/* 0:xntmax+1		 */
    int          *ADR;					/* -xtmax-1:xntmax+1	 */
    int          *TPD;					/* -xtmax-1:-1		 */
    int          *NUMPD;				/* 0:maxnpd		 */
    int          *TNUMPD;				/* 0:maxtpd		 */
    int          *RULE_TO_REDUCE;			/* 0:xnbpro-actpro	 */
    SXBA          IS_A_GENERIC_TERMINAL;		/* -tmax		 */
    SXBA          BVIDE;				/* xntmax		 */
    SXBA          NULLABLE;				/* indpro		 */
    SXBA         *FIRST;				/* 1:ntmax,1:xtmax	 */
    SXBA         *FOLLOW;				/* 1:-tmax,1:-tmax	 */
    char         *T_STRING;				/* t_string_length	 */
    char         *NT_STRING;				/* nt_string_length	 */
};


extern BOOLEAN bnf_read (struct bnf_ag_item *B, char *langname);
extern BOOLEAN bnf_write (struct bnf_ag_item *B, char *langname);
extern SXVOID  bnf_free (struct bnf_ag_item *B);
