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

bool xtag_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    case SXOPEN:
	return SXANY_BOOL;

    case SXCLOSE:
	return SXANY_BOOL;

    case SXINIT:
	return SXANY_BOOL;

    case SXFINAL:
	return SXANY_BOOL;

    case SXACTION:
	switch (act_no) {
	case 1:
	  if (--sxsvar.sxlv_s.counters [1] == 0)
	    sxsvar.sxlv_s.counters [0] = 0;
	  return SXANY_BOOL;
	}

    default:
	gripe ();
    }
}

