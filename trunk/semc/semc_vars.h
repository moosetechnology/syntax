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
   variable, selon la valeur donnée à SX_GLOBAL_VAR_SEMC : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_SEMC  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_SEMC /* Inutile de le faire plusieurs fois */
#ifdef SX_DFN_EXT_VAR_SEMC
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#define SX_GLOBAL_VAR_SEMC	/*rien*/
#define SX_INIT_VAL_SEMC(v)	= v
#else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#define SX_GLOBAL_VAR_SEMC	extern
#define SX_INIT_VAL_SEMC(v)	/*rien*/
#endif /* #ifndef SX_DFN_EXT_VAR_SEMC */
#endif /* #ifndef SX_GLOBAL_VAR_SEMC */

#include "bnf_vars.h"

SX_GLOBAL_VAR_SEMC SXINT     max_attr;
SX_GLOBAL_VAR_SEMC SXINT	nt_pg, prolnb, maxlis, xnomod, ximplic, xprod, nb_definitions, nb_sem_rules, nb_defaults, nb_identities,
     M_at;
SX_GLOBAL_VAR_SEMC bool		is_ident, is_bigbez, bident, is_err, is_empty;

/* options de semc */

SX_GLOBAL_VAR_SEMC bool		is_sem_out, is_default;
SX_GLOBAL_VAR_SEMC SXINT	tbl_lgth;
SX_GLOBAL_VAR_SEMC char	*modele, *mod_ident, *a_repeter;
SX_GLOBAL_VAR_SEMC struct sxtoken	rule_token, terminal_token;
SX_GLOBAL_VAR_SEMC SXINT	/* 1:ntmax		  */ *nt_to_ste;
SX_GLOBAL_VAR_SEMC SXINT	/* 1:-tmax		  */ *t_to_ste;
SX_GLOBAL_VAR_SEMC SXINT	/* -NBTERM:tbl_lgth	  */ *attr_to_ste;
SX_GLOBAL_VAR_SEMC SXINT	/* 1:tbl_lgth		  */ *defini;
SX_GLOBAL_VAR_SEMC SXINT	/* 1:tbl_lgth		  */ *attr_lgt;
SX_GLOBAL_VAR_SEMC SXINT	/* 1:tbl_lgth		  */ *type_attr;
SX_GLOBAL_VAR_SEMC char	/* 1:tbl_lgth char[ntmax] */ **attr_nt;

struct ste_elem {
    SXINT		code;
    short	nature;
    struct ste_elem	*next_elem;
    SXINT		stentry;
};

SX_GLOBAL_VAR_SEMC struct ste_elem		/* 1:ntmax-tmax+tbl_lgth */ *ste_to;

