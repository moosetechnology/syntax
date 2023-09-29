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












/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR_TABC : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_TABC  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_TABC /* Inutile de le faire plusieurs fois */
#  ifdef SX_DFN_EXT_VAR_TABC
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#    define SX_GLOBAL_VAR_TABC	/*rien*/
#    define SX_INIT_VAL_TABC(v)	= v
#  else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#    define SX_GLOBAL_VAR_TABC	extern
#    define SX_INIT_VAL_TABC(v)	/*rien*/
#  endif /* #ifndef SX_DFN_EXT_VAR_TABC */
#endif /* #ifndef SX_GLOBAL_VAR_TABC */

#include "bnf_vars.h"

SX_GLOBAL_VAR_TABC SXINT     max_attr;
SX_GLOBAL_VAR_TABC SXINT	nt_pg, prolnb, maxlis, xnomod, ximplic, xprod, nb_definitions, nb_sem_rules, nb_defaults, nb_identities,
     M_at;
SX_GLOBAL_VAR_TABC SXBOOLEAN		is_ident, is_bigbez, bident, is_err, is_empty;

/* options de tabact */

SX_GLOBAL_VAR_TABC SXBOOLEAN		is_sem_out, is_default;
SX_GLOBAL_VAR_TABC SXINT	tbl_lgth;
SX_GLOBAL_VAR_TABC char	*modele, *mod_ident, *a_repeter;
SX_GLOBAL_VAR_TABC struct sxtoken	rule_token, terminal_token;
SX_GLOBAL_VAR_TABC SXINT	/* 1:ntmax		  */ *nt_to_ste;
SX_GLOBAL_VAR_TABC SXINT	/* 1:-tmax		  */ *t_to_ste;
SX_GLOBAL_VAR_TABC SXINT	/* -NBTERM:tbl_lgth	  */ *attr_to_ste;
SX_GLOBAL_VAR_TABC SXINT	/* 1:tbl_lgth		  */ *defini;
SX_GLOBAL_VAR_TABC SXINT	/* 1:tbl_lgth		  */ *attr_lgt;
SX_GLOBAL_VAR_TABC SXINT	/* 1:tbl_lgth		  */ *type_attr;
SX_GLOBAL_VAR_TABC char	/* 1:tbl_lgth char[ntmax] */ **attr_nt;
struct ste_elem {
    SXINT		code;
    short	nature;
    struct ste_elem	*next_elem;
    SXINT		stentry;
};
SX_GLOBAL_VAR_TABC struct ste_elem		/* 1:ntmax-tmax+tbl_lgth */ *ste_to;
