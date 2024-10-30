/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXTMPFILE[] = "@(#)SYNTAX - $Id: sxtmpfile.c 4478 2024-10-24 17:53:14Z garavel $" WHAT_DEBUG;

#ifndef sxnewfile
#ifdef WINNT /* No mkstemp */
FILE	*sxnewfile (char *name)
{
    /* warning: mktemp() is insecure and deprecated */
    return sxfopen (mktemp (name), "w+");
}
#else  /* #if WINNT *//* No mkstemp */
FILE	*sxnewfile (char *name)
{
    sxfiledesc_t  tmp_descriptor;
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
#endif /* #if WINNT *//* No mkstemp */
#endif


#ifndef sxtmpfile

FILE	*sxtmpfile (void)
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

FILE	*sxfopen (char *s, char *m)
{
    FILE	*f;

    if ((f = fopen (s, m)) != NULL) {
	setmode (fileno (f), O_BINARY);
    }

    return f;
}
#endif


#ifndef sxfreopen

FILE	*sxfreopen (char *s, char *m, FILE *f)
{
    if ((f = freopen (s, m, f)) != NULL) {
	setmode (fileno (f), O_BINARY);
    }

    return f;
}
#endif


#ifndef sxfdopen

FILE	*sxfdopen (SXINT d, char *m)
{
    if (d < 0) {
	return NULL;
    }

    setmode (d, O_BINARY);

    return fdopen (d, m);
}
#endif
