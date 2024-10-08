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
#include "D_tables.h"

char WHAT_PRIOWRITE[] = "@(#)SYNTAX - $Id: priowrite.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#define WRITE(f,p,l)		\
	if (bytes = (l), (write (f, p, bytes) != bytes))	\
		goto write_error


bool		prio_write (SXINT t_priorities_size, 
			    SXINT r_priorities_size,
			    struct priority *t_priorities,
			    struct priority *r_priorities,
			    char *langname)
{
    SXINT	bytes;
    sxfiledesc_t	/* file descriptor */ F_dt;
    SXINT		dt_version;
    char	ent_name [128];
    static char		ME [] = "prio_write";


/* sortie des tables de prio */

    if ((F_dt = open (strcat (strcpy (ent_name, langname), ".dt"), O_WRONLY+O_CREAT+O_BINARY, 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	goto file_error;
    }

    dt_version = dt_num_version;
    /* numero de version des tables */

    WRITE (F_dt, &(dt_version), sizeof (SXINT));
    WRITE (F_dt, t_priorities, t_priorities_size * sizeof (struct priority));
    WRITE (F_dt, r_priorities, r_priorities_size * sizeof (struct priority));
    close (F_dt);
    return true;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (ent_name);
    return false;
}
