/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *                                                      *
   ******************************************************** */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030423 13:00 (phd):	Améliorations, notamment pour récursion	*/
/************************************************************************/
/* 20030423 11:30 (phd):	Bogue dans la gestion du nom du fichier	*/
/*				inclus (H. Garavel, David Champelovier)	*/
/************************************************************************/
/* 21-10-93 13:00 (phd&pb):	Adaptation a OSF			*/
/************************************************************************/
/* 19-07-90 12:00 (pb):		Bug dans le (pointeur sur le) nom de	*/
/*				l'include utilise dans la structure	*/
/*				sxsrcmngr.source_coord.source_file	*/
/*				(Detecte par:Xavier Pandolfi LIFIA-IMAG)*/
/************************************************************************/
/* 03-12-87 13:50 (phd):	Correction d'un == (merci lint!)	*/
/************************************************************************/
/* 01-10-87 11:55 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"

char WHAT_SXINCL_MNGR[] = "@(#)SYNTAX - $Id: sxincl_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

struct incl {
    FILE	*infile;
    struct sxsrcmngr	srcmngr;
    unsigned int pathste;
};
static struct incl	*incls;
static int	xincls, incls_size;



static BOOLEAN	my_push_incl (char *pathname, BOOLEAN disallow_recursion)
{
    register int	x;
    register struct incl	*ai;
    register unsigned int	ste;

    ste = sxstrsave (pathname);

    if (disallow_recursion)
	for (x = xincls - 1; x >= 0; x--)
	    if (incls [x].pathste == ste)
		return FALSE /* appel recursif */ ;

    if (xincls >= incls_size)
	incls = (struct incl*) sxrealloc (incls, incls_size += 5, sizeof (struct incl));

    ai = incls + xincls++;
    ai->srcmngr = sxsrcmngr;
    ai->pathste = ste;

    if ((ai->infile = sxfopen (pathname, "r")) == NULL) {
	xincls--;
	return FALSE;
    }
    
    sxsrc_mngr (INIT, ai->infile, sxstrget (ste));
    sxnext_char () /* Lecture du premier caractere de l'include */;
    return TRUE;
}



BOOLEAN		sxpush_incl (char *pathname)
{
    return my_push_incl (pathname, TRUE);
}



BOOLEAN		sxpush_recincl (char *pathname)
{
    return my_push_incl (pathname, FALSE);
}



BOOLEAN		sxpop_incl ()
{
    register struct incl	*ai;

    if (xincls <= 0)
	return FALSE;

    ai = incls + --xincls;
    sxsrc_mngr (FINAL);
    sxsrcmngr = ai->srcmngr;
    sxfclose (ai->infile);
    return TRUE;
}



VOID	sxincl_mngr (sxincl_mngr_what)
    int		sxincl_mngr_what;
{
    switch (sxincl_mngr_what) {
    case OPEN:
	incls = (struct incl*) sxalloc (incls_size = 5, sizeof (struct incl));
	break;

    case INIT:
	xincls = 0;
	break;

    case FINAL:
	while (xincls > 0)
	    sxfclose (incls [--xincls].infile);
	break;

    case CLOSE:
	sxfree (incls);
	break;
    default: /* pour faire taire gcc -Switch-default */
#if EBUG
      sxtrap("sxincl_mngr","unknown switch case #1");
#endif
      break;
    }
}
