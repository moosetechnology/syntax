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

/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR_XBNF : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_XBNF  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_XBNF /* Inutile de le faire plusieurs fois */
#  ifdef SX_DFN_EXT_VAR_XBNF
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#    define SX_GLOBAL_VAR_XBNF	/*rien*/
#    define SX_INIT_VAL_XBNF(v)	= v
#  else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#    define SX_GLOBAL_VAR_XBNF	extern
#    define SX_INIT_VAL_XBNF(v)	/*rien*/
#  endif /* #ifndef SX_DFN_EXT_VAR_XBNF */
#endif /* #ifndef SX_GLOBAL_VAR_XBNF */

typedef SXBA *SXBM;
SX_GLOBAL_VAR_XBNF struct bnf_ag_item	bnf_ag;
SX_GLOBAL_VAR_XBNF long	options_set;
SX_GLOBAL_VAR_XBNF BOOLEAN		is_list, is_source, is_proper, is_predicate;
SX_GLOBAL_VAR_XBNF SXINT	max_RHS;
SX_GLOBAL_VAR_XBNF SXINT	EOF_ste, M_t, M_nt, indpro_size;
SX_GLOBAL_VAR_XBNF char	*prgentname;
SX_GLOBAL_VAR_XBNF SXINT	*proper [4];
SX_GLOBAL_VAR_XBNF struct sxtables		*sxtab_ptr;

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
#define		XT_TO_PRDCT(code)	(((code)<bnf_ag.WS_TBL_SIZE.tmax)\
               ? bnf_ag.XT_TO_T_PRDCT[bnf_ag.WS_TBL_SIZE.tmax-(code)].prdct_no : -1)

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

