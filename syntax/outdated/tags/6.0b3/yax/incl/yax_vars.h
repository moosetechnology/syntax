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
SX_GLOBAL_VAR_YAX BOOLEAN		is_err;

/* options de yax */

SX_GLOBAL_VAR_YAX BOOLEAN		is_sem_stats;
SX_GLOBAL_VAR_YAX SXINT	tbl_lgth;
SX_GLOBAL_VAR_YAX struct sxtoken	*terminal_token;
SX_GLOBAL_VAR_YAX SXINT	*nt_to_ste /* [1:ntmax] */ ;
SX_GLOBAL_VAR_YAX SXINT	*t_to_ste /* [1:-tmax] */ ;
SX_GLOBAL_VAR_YAX SXINT	*attr_to_ste /* [-NBTERM:tbl_lgth] */ ;
SX_GLOBAL_VAR_YAX char	**attr_nt /* [1:tbl_lgth] char [ntmax] */ ;
SX_GLOBAL_VAR_YAX SXINT	*ste_to_tnt /* [1:lst] */ ;
SX_GLOBAL_VAR_YAX SXINT	*ste_to_attr /* [1:lst] */ ;


