/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (bl)  *
   *							  *
   ******************************************************** */





/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030526 15:49 (phd):	Utilisation de SX_GLOBAL_VAR		*/
/************************************************************************/
/* 10-04-90 14:30 (phd):	Suppression des "typedef" de bl		*/
/*				Ajout du copyright			*/
/************************************************************************/


/*------------------------------*/
/*          bnf_vars.h          */
/*------------------------------*/
/*				*/
/*  VARIABLES for BNF's TABLES  */
/*				*/
/*------------------------------*/

typedef SXBA *SXBM;
SX_GLOBAL_VAR	struct bnf_ag_item	bnf_ag;
SX_GLOBAL_VAR	long	options_set;
SX_GLOBAL_VAR	BOOLEAN		is_list, is_source, is_proper, is_predicate, is_huge;
SX_GLOBAL_VAR	int	max_RHS;
SX_GLOBAL_VAR	int	EOF_ste, M_t, M_nt, indpro_size;
SX_GLOBAL_VAR	char	*prgentname;
SX_GLOBAL_VAR	int	*proper [4];
SX_GLOBAL_VAR	struct sxtables		*sxtab_ptr;

#define		ws_tbl_size	bnf_ag.WS_TBL_SIZE
#define		ws_nbpro	bnf_ag.WS_NBPRO
#define		ws_indpro	bnf_ag.WS_INDPRO
#define		xt_to_t_prdct	bnf_ag.XT_TO_T_PRDCT
#define		xnt_to_nt_prdct	bnf_ag.XNT_TO_NT_PRDCT
#define		ws_ntmax	bnf_ag.WS_NTMAX
#define		adr		bnf_ag.ADR
#define		tpd		bnf_ag.TPD
#define		numpd		bnf_ag.NUMPD
#define		tnumpd		bnf_ag.TNUMPD
#define		rule_to_reduce	bnf_ag.RULE_TO_REDUCE
#define		is_a_generic	bnf_ag.IS_A_GENERIC_TERMINAL
#define		bvide		bnf_ag.BVIDE
#define		nullable	bnf_ag.NULLABLE
#define		follow		bnf_ag.FOLLOW
#define		t_string	bnf_ag.T_STRING
#define		nt_string	bnf_ag.NT_STRING

#define		XNT_TO_NT(code) (((code)-bnf_ag.WS_TBL_SIZE.ntmax>0) ? \
				 bnf_ag.XNT_TO_NT_PRDCT[(code)-bnf_ag.WS_TBL_SIZE.ntmax].v_code : \
				 (code))
#define		XT_TO_T(code)	((0<bnf_ag.WS_TBL_SIZE.tmax-(code)) ? \
				 -bnf_ag.XT_TO_T_PRDCT[bnf_ag.WS_TBL_SIZE.tmax-(code)].v_code : \
				 (code))
#define		LGPROD(r)	(ws_nbpro [r + 1].prolon - ws_nbpro [r].prolon - 1)
#define 	IS_A_PARSACT(nt)	((nt) <= bnf_ag.WS_TBL_SIZE.ntmax &&\
					 bnf_ag.WS_NBPRO [bnf_ag.WS_NTMAX [nt].npg].numpg >\
					 bnf_ag.WS_TBL_SIZE.nbpro)
/* pas un xnt en LHS d'une production ajoutee */ 

#define		W	 	ws_tbl_size
#define 	WN	 	ws_nbpro
#define 	WI	 	ws_indpro
#define 	WX	 	ws_ntmax
#define 	VS	 	VISITED->token

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */

