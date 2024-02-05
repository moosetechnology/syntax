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

char WHAT_PRIOREAD[] = "@(#)SYNTAX - $Id: prioread.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;


#define READ(f,p,l)		\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error


bool		prio_read (SXINT t_priorities_size, 
			   SXINT r_priorities_size,
			   struct priority **t_priorities,
			   struct priority **r_priorities,
			   char *langname)
{
    SXINT	bytes;
    sxfiledesc_t	/* file descriptor */ F_dt;
    SXINT		dt_version;
    char	ent_name [128];
    static char		ME [] = "prio_read";


/* allocation et lecture des tables de prio */

    *t_priorities = (struct priority*) sxcalloc (t_priorities_size, sizeof (struct priority));
    *r_priorities = (struct priority*) sxcalloc (r_priorities_size, sizeof (struct priority));

    if ((F_dt = open (strcat (strcpy (ent_name, langname), ".dt"), 0+O_BINARY)) < 0) {
	/* No message: this is normal */
	return false;
    }

    READ (F_dt, &(dt_version), sizeof (SXINT));

    if (dt_version != dt_num_version) {
	fprintf (sxstderr, "tables format has changed : please use the new prio\n");
	return (false);
    }

    READ (F_dt, *t_priorities, t_priorities_size * sizeof (struct priority));
    READ (F_dt, *r_priorities, r_priorities_size * sizeof (struct priority));
    close (F_dt);
    return true;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
    sxperror (ent_name);
    close (F_dt);
    return false;
}
