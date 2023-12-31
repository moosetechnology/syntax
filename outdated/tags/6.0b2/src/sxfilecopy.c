/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2003 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'�quipe ATOLL. (phd)                    *
   *                                                      *
   ******************************************************** */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030506 10:21 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 20030328 10:30 (phd):	Cr�ation				*/
/************************************************************************/


#include "sxunix.h"

char WHAT_SXFILECOPY[] = "@(#)SYNTAX - $Id: sxfilecopy.c 654 2007-06-26 10:42:18Z rlacroix $" WHAT_DEBUG;


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

BOOLEAN	sxfile_copy (char *in, char *out, long char_count, char *ME)
{
    register int	nbread;
    register long	cumul;
    register int	fin, fout;

/* intialise: */

    {
	struct stat	stat_buf;

	if ((fin = open (in, 0 + O_BINARY)) == -1 || fstat (fin, &stat_buf) != 0) {
	    fprintf (sxstderr, "%s: Unable to open (read) ", ME);
	    sxperror (in);
	    return FALSE;
	}

	if (stat_buf.st_size != char_count) {
	    fprintf (stderr, "%s: SEVERE ERROR: %lld bytes have been written\non %s instead of %ld.\n", ME, (long long)stat_buf.st_size, in, char_count);
	    close (fin);
	    return FALSE;
	}

	if ((fout = open (out, O_WRONLY + O_CREAT + O_TRUNC + O_BINARY, 0664)) == -1) {
	    fprintf (sxstderr, "%s: Unable to open (create) ", ME);
	    sxperror (out);
	    close (fin);
	    return FALSE;
	}
    }

/* copy: */

    {
/* On pourrait utiliser � char[256 * BUFSIZ] tampon �, mais cela peut �tre tr�s couteux, voire impossible */
	register char	*tampon;
	long	taille_tampon;

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
	    return TRUE;
	}

	fprintf (sxstderr, "%s: SEVERE ERROR: Only the %ld first bytes have been written\non %s (out of the %ld in %s).\n", ME, cumul, out, char_count, in);
	return FALSE;
    }
}
