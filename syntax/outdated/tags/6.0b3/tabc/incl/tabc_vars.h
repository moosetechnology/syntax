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
SX_GLOBAL_VAR_TABC BOOLEAN		is_ident, is_bigbez, bident, is_err, is_empty;

/* options de tabact */

SX_GLOBAL_VAR_TABC BOOLEAN		is_sem_out, is_default;
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
