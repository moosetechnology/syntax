/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/
#include "bnf_vars.h"

/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR_YAX : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_YAX  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_YAX /* Inutile de le faire plusieurs fois */
#  ifdef SX_DFN_EXT_VAR_YAX
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#    define SX_GLOBAL_VAR_YAX	/*rien*/
#    define SX_INIT_VAL_YAX(v)	= v
#  else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#    define SX_GLOBAL_VAR_YAX	extern
#    define SX_INIT_VAL_YAX(v)	/*rien*/
#  endif /* #ifndef SX_DFN_EXT_VAR_YAX */
#endif /* #ifndef SX_GLOBAL_VAR_YAX */

SX_GLOBAL_VAR_YAX SXINT	max_attr, nb_definitions, nb_sem_rules, nb_occurrences;
SX_GLOBAL_VAR_YAX SXBOOLEAN		is_err;

/* options de yax */

SX_GLOBAL_VAR_YAX SXBOOLEAN		is_sem_stats;
SX_GLOBAL_VAR_YAX SXINT	tbl_lgth;
SX_GLOBAL_VAR_YAX struct sxtoken	*terminal_token;
SX_GLOBAL_VAR_YAX SXINT	*nt_to_ste /* [1:ntmax] */ ;
SX_GLOBAL_VAR_YAX SXINT	*t_to_ste /* [1:-tmax] */ ;
SX_GLOBAL_VAR_YAX SXINT	*attr_to_ste /* [-NBTERM:tbl_lgth] */ ;
SX_GLOBAL_VAR_YAX char	**attr_nt /* [1:tbl_lgth] char [ntmax] */ ;
SX_GLOBAL_VAR_YAX SXINT	*ste_to_tnt /* [1:lst] */ ;
SX_GLOBAL_VAR_YAX SXINT	*ste_to_attr /* [1:lst] */ ;


