/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1990 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *                                                      *
   ******************************************************** */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030506 10:21 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 20020318 17:30 (phd):	"sxnewfile"				*/
/************************************************************************/
/* 14-10-99 14:10 (phd):	"sxfopen", "sxfreopen"			*/
/************************************************************************/
/* 20-05-92 15:40 (phd):	Declaration de "mktemp"			*/
/************************************************************************/
/* 16-08-90 12:30 (phd):	Creation				*/
/************************************************************************/


#include "sxunix.h"

char WHAT_SXTMPFILE[] = "@(#)SYNTAX - $Id: sxtmpfile.c 636 2007-06-18 13:14:07Z rlacroix $" WHAT_DEBUG;

#ifndef sxnewfile
#ifdef WINNT /* No mktemp */
FILE	*sxnewfile (name)
    char	*name;
{

    return sxfopen (mktemp (name), "w+");
}
#else  /* #if WINNT *//* No mktemp */
FILE	*sxnewfile (name)
    char	*name;
{
    int		tmp_descriptor;
    FILE	*tmp_file;

    if (name != NULL) {
	if ((tmp_descriptor = mkstemp (name)) != -1) {
	    if ((tmp_file = sxfdopen (tmp_descriptor, "w+")) != NULL)
		return tmp_file;
	    close (tmp_descriptor);
	}

	name [0] = SXNUL;
    }

    return NULL;
}
#endif /* #if WINNT *//* No mktemp */
#endif


#ifndef sxtmpfile

FILE	*sxtmpfile ()
{
    static char		tmp_seed [] = "/tmp/sxXXXXXX";
    char	tmp_path [sizeof tmp_seed];
    FILE	*tmp_file;

    if ((tmp_file = sxnewfile (strcpy (tmp_path, tmp_seed))) != NULL)
	unlink (tmp_path);

    return tmp_file;
}
#endif


#ifndef sxfopen

FILE	*sxfopen (s, m)
    char	*s;
    char	*m;
{
    FILE	*f;

    if ((f = fopen (s, m)) != NULL) {
	setmode (fileno (f), O_BINARY);
    }

    return f;
}
#endif


#ifndef sxfreopen

FILE	*sxfreopen (s, m, f)
    char	*s;
    char	*m;
    FILE	*f;
{
    if ((f = freopen (s, m, f)) != NULL) {
	setmode (fileno (f), O_BINARY);
    }

    return f;
}
#endif


#ifndef sxfdopen

FILE	*sxfdopen (d, m)
    int		d;
    char	*m;
{
    if (d < 0) {
	return NULL;
    }

    setmode (d, O_BINARY);

    return fdopen (d, m);
}
#endif
