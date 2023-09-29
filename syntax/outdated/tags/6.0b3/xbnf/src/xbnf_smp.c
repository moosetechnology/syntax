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
/* 20030512 13:07 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 10-04-90 14:30 (phd):	Suppression des "typedef" de bl		*/
/*				Ajout du copyright			*/
/************************************************************************/

#include "sxunix.h"
#include "B_tables.h"
#include "xbnf_vars.h"
char WHAT_XBNFSMP[] = "@(#)SYNTAX - $Id: xbnf_smp.c 1122 2008-02-28 15:55:03Z rlacroix $" WHAT_DEBUG;


extern	SXVOID symbol_table_processing (NODE *);
extern	SXVOID property_test (void);
extern	SXVOID follow_construction (void);


SXVOID
sxxbnfsmp (SXINT what, struct sxtables *sxtables_ptr)
{
    switch (what) {
    case OPEN:
	sxatcvar.atc_lv.node_size = sizeof (NODE);
	sxtab_ptr = sxtables_ptr;
	break;

    case CLOSE:
	break;

    case ACTION:

/*-----------------*/
/* initializations */
/*-----------------*/

	ws_nbpro = (struct ws_nbpro_s*) NULL;
	ws_indpro = (struct ws_indpro_s*) NULL;
	ws_ntmax = (struct ws_ntmax_s*) NULL;
	adr = tpd = numpd = tnumpd = (SXINT*) NULL;
	t_string = nt_string = (char*) NULL;
	bvide = (SXBA) NULL;
	follow = (SXBM) NULL;

	if (IS_ERROR) {
	    return;
	}


/* ws_tbl_size(W) initialize */

	W.sem_kind = 0;
	WN = (struct ws_nbpro_s*) NULL;
	EOF_ste = sxstrsave ("END OF FILE");


/*---------*/
/* actions */
/*---------*/

	if (sxverbosep)
	    fputs ("   Symbol Table Processing\n", sxtty);

	symbol_table_processing (sxatcvar.atc_lv.abstract_tree_root);

	if (sxverbosep)
	    fputs ("   Property Test\n", sxtty);

	property_test ();

	if (sxverbosep)
	    fputs ("   Follow Construction\n", sxtty);

	follow_construction ();
	break;

    default:
	fputs ("The function \"sxxbnfsmp\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
