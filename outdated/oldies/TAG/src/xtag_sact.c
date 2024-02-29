/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.	  *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 09-08-94 (pb):	Creation					*/
/************************************************************************/

char WHAT[] = "@(#)xtag_sact.c\t- SYNTAX [unix] - Mer 28 Dec 1994 13:55:26";

#include "sxunix.h"
#include "xtag.h"


static void
gripe ()
{
    fputs ("\nThe function \"xtag_scan_act\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
}



void	(*more_scan_act) ();



xtag_scan_act (code, act_no)
    int		code;
    int		act_no;
{
    switch (code) {
    case SXOPEN:
	return;

    case SXCLOSE:
	return;

    case SXINIT:
	return;

    case SXFINAL:
	return;

    case SXACTION:
	switch (act_no) {
	case 1:
	  if (--sxsvar.sxlv_s.counters [1] == 0)
	    sxsvar.sxlv_s.counters [0] = 0;

	  return;

	}

    default:
	gripe ();
    }
}
