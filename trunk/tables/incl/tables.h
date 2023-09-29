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

/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR_TABLES : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_TABLES  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_TABLES /* Inutile de le faire plusieurs fois */
#ifdef SX_DFN_EXT_VAR_TABLES
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#define SX_GLOBAL_VAR_TABLES        /*rien*/
#define SX_INIT_VAL_TABLES(v)       = v
#else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#define SX_GLOBAL_VAR_TABLES        extern
#define SX_INIT_VAL_TABLES(v)       /*rien*/
#endif /* #ifndef SX_DFN_EXT_VAR_TABLES */
#endif /* #ifndef SX_GLOBAL_VAR_TABLES */

#define slice_length	25
#define slice_length2	12

#include "S_tables.h"
#include "P_tables.h"
#include "R_tables.h"
#include "PP_tables.h"
#include "T_tables.h"

SX_GLOBAL_VAR_TABLES struct lecl_tables_s	S;
SX_GLOBAL_VAR_TABLES struct parser_ag_item	P;
SX_GLOBAL_VAR_TABLES struct R_tables_s	R;
SX_GLOBAL_VAR_TABLES struct PP_ag_item	PP;
SX_GLOBAL_VAR_TABLES struct T_ag_item	T;

#define PC P.constants
#define PR P.reductions
#define PT P.t_bases
#define PNT P.nt_bases
#define PV P.vector
#define PGO P.gotos
#define PPR P.prdcts
#define PGA P.germe_to_gr_act

#define SC S.S_tbl_size
#define SM S.S_transition_matrix
#define SP S.SXS_action_or_prdct_code
#define SK S.S_check_kw
#define SA S.SXS_adrp

#define RC R.R_tbl_size
#define SL R.SXS_local_mess
#define SG R.S_global_mess
#define PL R.SXP_local_mess
#define PG R.P_global_mess

#define PPC PP.PP_constants
#define PPS PP.SXPP_schema

#define TC T.T_constants
#define TN T.SXT_node_info

SX_GLOBAL_VAR_TABLES SXBOOLEAN	is_syno;
