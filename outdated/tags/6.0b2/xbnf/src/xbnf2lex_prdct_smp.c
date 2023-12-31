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
/* 12 Fev 2003 11:0 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
char WHAT_XBNF2LEXPRDCTSMP[] = "@(#)SYNTAX - $Id: xbnf2lex_prdct_smp.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;


extern	VOID xbnf2lex_prdct (/* struct node_header_s *adam */);

struct sxtables *sxtab_ptr;


VOID
xbnf2lexprdct_smp (what, sxtables_ptr)
    int		what;
    struct sxtables	*sxtables_ptr;
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
	if (sxerrmngr.nbmess[2]>0) {
	    return;
	}


/*---------*/
/* actions */
/*---------*/

	if (sxverbosep)
	    fputs ("   xbnf2lex_prdct\n", sxtty);

	xbnf2lex_prdct (sxatcvar.atc_lv.abstract_tree_root);
	break;

    default:
	fputs ("The function \"xbnf2lex_prdct_smp\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
