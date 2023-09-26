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


#include "bnf_vars.h"

int	nt_pg, prolnb, maxlis, xnomod, ximplic, max_attr, xprod, nb_definitions, nb_sem_rules, nb_defaults, nb_identities,
     M_at;
BOOLEAN		is_ident, is_bigbez, bident, is_err, is_empty;

/* options de tabact */

BOOLEAN		is_sem_out, is_default;
int	tbl_lgth;
char	*modele, *mod_ident, *a_repeter;
struct sxtoken	rule_token, terminal_token;
int	/* 1:ntmax		  */ *nt_to_ste;
int	/* 1:-tmax		  */ *t_to_ste;
int	/* -NBTERM:tbl_lgth	  */ *attr_to_ste;
int	/* 1:tbl_lgth		  */ *defini;
int	/* 1:tbl_lgth		  */ *attr_lgt;
int	/* 1:tbl_lgth		  */ *type_attr;
char	/* 1:tbl_lgth char[ntmax] */ **attr_nt;
struct ste_elem {
    int		code;
    short	nature;
    struct ste_elem	*next_elem;
    int		stentry;
};
struct ste_elem		/* 1:ntmax-tmax+tbl_lgth */ *ste_to;
