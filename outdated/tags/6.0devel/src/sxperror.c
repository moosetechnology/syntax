/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/









#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXPERROR[] = "@(#)SYNTAX - $Id: sxperror.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;


#ifdef __CYGWIN__
#include <errno.h>
#else
#if defined(unix) || defined(__APPLE_CC__) || defined(__hpux) || defined(__NetBSD__)
# include <errno.h>
  /* extern SXINT	errno; */
# if defined(unix) && !defined(linux) || defined(__hpux)
extern SXINT	sys_nerr;
# endif
#else /* of ifdef unix */
#endif /* of ifdef unix */
#endif /* of idef __CYGWIN__ */

SXVOID	sxperror (char *string)
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
