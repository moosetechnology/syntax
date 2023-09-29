/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2000 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   * Produit de l'équipe ATOLL (Langages et Traducteurs). *
   *                                                      *
   ******************************************************** */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20050125 17:36 (phd):	Tentative de rationalisation (1ère)	*/
/************************************************************************/
/* 20040213 14:51 (phd):	#if WINNT pour WHAT			*/
/************************************************************************/
/* 20040209 15:03 (phd):	Adaptation à HPUX			*/
/************************************************************************/
/* 20030505 14:18 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 20000905 18:14 (phd):	Adaptation à MINGW			*/
/************************************************************************/
/* 12-10-99 18:05 (phd):	Utilisation de <errno.h>		*/
/************************************************************************/
/* 06-10-99 18:25 (phd):	Utilisation de strerror			*/
/************************************************************************/
/* 17-12-86 12:10 (phd):	Creation				*/
/************************************************************************/

#include "sxunix.h"

char WHAT_SXPERROR[] = "@(#)SYNTAX - $Id: sxperror.c 1033 2008-02-14 13:20:30Z rlacroix $" WHAT_DEBUG;


#ifdef __CYGWIN__
#include <errno.h>
#else
#if defined(unix) || defined(__APPLE_CC__) || defined(__hpux)
# include <errno.h>
  /* extern SXINT	errno; */
# if defined(unix) && !defined(linux) || defined(__hpux)
extern SXINT	sys_nerr;
# endif
#else /* of ifdef unix */
#if 0 /* inclus par sxunix */
# include <stdlib.h>
#endif /* 0 */
#endif /* of ifdef unix */
#endif /* of idef __CYGWIN__ */

VOID	sxperror (char *string)
{
#if defined(linux) || defined(__SVR4) || defined(__hpux) || defined(__APPLE_CC__)
# define ERRORSTRING	strerror (errno)
  /* extern char		*strerror (); */
#else
# define ERRORSTRING	((errno >= sys_nerr) ? "Unknown error" : (sys_errlist [errno]))
# ifdef unix
#  ifndef _CYGWIN_TYPES_H
  extern char		*sys_errlist [];
#  endif /* _CYGWIN_TYPES_H */
# endif /* of ifdef unix */
#endif

    fprintf (sxstderr, "%s: %s.\n", string, ERRORSTRING);
}
