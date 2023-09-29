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

#ifndef lint
char	what_sxincl_mngr [] = "@(#)sxincl_mngr.c	- SYNTAX [unix] - Jeu 21 Oct 1993";
#endif

#include "sxunix.h"


struct incl {
    FILE	*infile;
    char	*pathname;
    struct sxsrcmngr	srcmngr;
};
static struct incl	*incls;
static int	xincls, incls_size;



BOOLEAN		sxpush_incl (pathname)
    char	*pathname;
{
    register int	x;
    register struct incl	*ai;

    if (xincls >= incls_size)
	incls = (struct incl*) sxrealloc (incls, incls_size += 5, sizeof (struct incl));

    ai = incls + xincls;
    ai->srcmngr = sxsrcmngr;
    ai->infile = fopen (pathname, "r");
    ai->pathname = strcpy ((char*) sxalloc (strlen (pathname) + 1, sizeof (char)), pathname);
    xincls++;

    for (x = xincls - 2; x >= 0; x--)
	if (strcmp (incls [x].pathname, pathname) == 0) {
	    ai->infile = NULL;
	    return FALSE /* appel recursif */ ;
	}

    if (ai->infile == NULL)
	return FALSE;

    sxsrc_mngr (INIT, ai->infile, ai->pathname);
    sxnext_char () /* Lecture du premier caractere de l'include */;
    return TRUE;
}



BOOLEAN		sxpop_incl ()
{
    register struct incl	*ai;

    if (xincls <= 0)
	return FALSE;

    ai = incls + --xincls;

/* Tue Apr 15 20:09:24 MEST 2003
   Changement introduit par David Champelovier pour resoudre une bogue
   qui se produit lorsqu'un fichier inclus comporte une erreur syntaxique
   (voir /common/Syntax/notes/bug_2003a). La ligne ci-dessous a ete
   commentee */
/*    sxfree (ai->pathname);*/

    if (ai->infile != NULL) {
	fclose (ai->infile);
	sxsrc_mngr (FINAL);
    }

    sxsrcmngr = ai->srcmngr;
    return TRUE;
}



VOID	sxincl_mngr (what)
    int		what;
{
    /* Handle errors */
    switch (what) {
    case OPEN:
	incls = (struct incl*) sxalloc (incls_size = 5, sizeof (struct incl));
	break;

    case INIT:
	xincls = 0;
	break;

    case FINAL:
	while (xincls > 0)
	    sxfree (incls [--xincls].pathname);
	break;

    case CLOSE:
	sxfree (incls);
	break;
    }
}
