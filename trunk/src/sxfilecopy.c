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

char WHAT_SXFILECOPY[] = "@(#)SYNTAX - $Id: sxfilecopy.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

bool	sxfile_copy (char *in, char *out, SXINT char_count, char *ME)
{
    SXINT	nbread;
    SXINT	cumul;
    sxfiledesc_t	fin, fout;

/* intialise: */

    {
	struct stat	stat_buf;

	if ((fin = open (in, 0 + O_BINARY)) == -1 || fstat (fin, &stat_buf) != 0) {
	    fprintf (sxstderr, "%s: Unable to open (read) ", ME);
	    sxperror (in);
	    return false;
	}

	if (stat_buf.st_size != (off_t) char_count) {
#if defined (_WIN32) && ! defined (_WIN64)
           /* cas de win32 : le type off_t occupe 4 octets */
           fprintf (stderr, "%s: SEVERE ERROR: %ld bytes have been written\non %s instead of %ld.\n", ME, (SXINT) stat_buf.st_size, in, char_count);
#else
	    fprintf (stderr, "%s: SEVERE ERROR: %lld bytes have been written\non %s instead of %ld.\n", ME, (long long)stat_buf.st_size, in, char_count);
#endif
	    close (fin);
	    return false;
	}

	if ((fout = open (out, O_WRONLY + O_CREAT + O_TRUNC + O_BINARY, 0664)) == -1) {
	    fprintf (sxstderr, "%s: Unable to open (create) ", ME);
	    sxperror (out);
	    close (fin);
	    return false;
	}
    }

/* copy: */

    {
/* On pourrait utiliser � char[256 * BUFSIZ] tampon �, mais cela peut �tre tr�s couteux, voire impossible */
	char	*tampon;
	SXINT	taille_tampon;

	tampon = sxalloc (taille_tampon = 256 * BUFSIZ, sizeof (*tampon));
	cumul = 0;

	while ((nbread = read (fin, tampon, taille_tampon * sizeof (*tampon))) > 0) {
	    if (write (fout, tampon, (size_t) nbread) != nbread) {
		nbread = -1;
		break;
	    }

	    cumul += nbread;
	}

	sxfree (tampon);
    }

/* finalise: */

    {
	close (fin);
	close (fout);

	if (nbread == 0) {
	    return true;
	}

	fprintf (sxstderr, "%s: SEVERE ERROR: Only the %ld first bytes have been written\non %s (out of the %ld in %s).\n", ME, cumul, out, char_count, in);
	return false;
    }
}
