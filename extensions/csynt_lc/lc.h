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
/*  L C . H  */

#include "varstr.h"
#include "sxunix.h"
#include "put_edit.h"
#include "B_tables.h"
#include "XxYxZ.h"


/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR_CSYNT_LC : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_CSYNT_LC  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_CSYNT_LC /* Inutile de le faire plusieurs fois */
#  ifdef SX_DFN_EXT_VAR_CSYNT_LC
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#    define SX_GLOBAL_VAR_CSYNT_LC	/*rien*/
#    define SX_INIT_VAL_CSYNT_LC(v)	= v
#  else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#    define SX_GLOBAL_VAR_CSYNT_LC	extern
#    define SX_INIT_VAL_CSYNT_LC(v)	/*rien*/
#  endif /* #ifndef SX_DFN_EXT_VAR_CSYNT_LC */
#endif /* #ifndef SX_GLOBAL_VAR_CSYNT_LC */

#define min(x,y) (((x)<=(y))?(x):(y))
#define max(x,y) (((x)>=(y))?(x):(y))

#define PUSH(s,x)	(s)[++*(s)]=(x)
#define POP(s)		(s)[(*(s))--]
#define IS_EMPTY(s)	(*(s)==0)

#define	LGPROD(r)	(bnf_ag.WS_NBPRO [r + 1].prolon - bnf_ag.WS_NBPRO [r].prolon - 1)
#define PROD_TO_RED(x)	(x > bnf_ag.WS_TBL_SIZE.actpro ? bnf_ag.RULE_TO_REDUCE [x - bnf_ag.WS_TBL_SIZE.actpro] : x)


/* le parametre xt est POSITIF */
#define XT_TO_T_CODE(xt) ((xt+bnf_ag.WS_TBL_SIZE.tmax<=0) ? xt : bnf_ag.XT_TO_T_PRDCT[xt+bnf_ag.WS_TBL_SIZE.tmax].v_code)
#define XT_TO_PRDCT_CODE(xt) ((xt+bnf_ag.WS_TBL_SIZE.tmax<=0) ? -1 : bnf_ag.XT_TO_T_PRDCT[xt+bnf_ag.WS_TBL_SIZE.tmax].prdct_no)

#define XNT_TO_NT_CODE(xnt) ((xnt-bnf_ag.WS_TBL_SIZE.ntmax<=0) ? xnt : bnf_ag.XNT_TO_NT_PRDCT[xnt-bnf_ag.WS_TBL_SIZE.ntmax].v_code)
#define XNT_TO_PRDCT_CODE(xnt) ((xnt-bnf_ag.WS_TBL_SIZE.ntmax<=0) ? -1 : bnf_ag.XNT_TO_NT_PRDCT[xnt-bnf_ag.WS_TBL_SIZE.ntmax].prdct_no)

#define get_t_string(t)  (bnf_ag.T_STRING + bnf_ag.ADR [t])
#define get_nt_string(nt)  (bnf_ag.NT_STRING + bnf_ag.ADR [nt])
#define get_nt_length(nt) (bnf_ag.ADR [nt+1] - bnf_ag.ADR [nt] - 1)

SX_GLOBAL_VAR_CSYNT_LC struct bnf_ag_item	bnf_ag;

SX_GLOBAL_VAR_CSYNT_LC bool is_automaton,
        is_listing_opened,
        is_LC,
        is_ELC,
        is_PLR,
        print_time,
	sxttycol1p;

SX_GLOBAL_VAR_CSYNT_LC VARSTR  vstr;

SX_GLOBAL_VAR_CSYNT_LC FILE	*listing;

#define TIME_INIT	0
#define TIME_FINAL	1

#if defined(SXVERSION) && EBUG
#define statistics	stdout
#else
#define statistics	NULL
#endif

/*   E N D    L C . H  */





