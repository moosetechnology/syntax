/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

#include "sxunix.h"
#include "D_tables.h"
char WHAT_PRIOREAD[] = "@(#)SYNTAX - $Id: prioread.c 1215 2008-03-13 14:06:31Z rlacroix $" WHAT_DEBUG;



#define READ(f,p,l)		\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error


BOOLEAN		prio_read (SXINT t_priorities_size, 
			   SXINT r_priorities_size,
			   struct priority **t_priorities,
			   struct priority **r_priorities,
			   char *langname)
{
    SXINT	bytes;
    filedesc_t	/* file descriptor */ F_dt;
    SXINT		dt_version;
    char	ent_name [128];
    static char		ME [] = "prio_read";


/* allocation et lecture des tables de prio */

    *t_priorities = (struct priority*) sxcalloc (t_priorities_size, sizeof (struct priority));
    *r_priorities = (struct priority*) sxcalloc (r_priorities_size, sizeof (struct priority));

    if ((F_dt = open (strcat (strcpy (ent_name, langname), ".dt"), 0+O_BINARY)) < 0) {
	/* No message: this is normal */
	return FALSE;
    }

    READ (F_dt, &(dt_version), sizeof (SXINT));

    if (dt_version != dt_num_version) {
	fprintf (sxstderr, "tables format has changed : please use the new prio\n");
	return (FALSE);
    }

    READ (F_dt, *t_priorities, t_priorities_size * sizeof (struct priority));
    READ (F_dt, *r_priorities, r_priorities_size * sizeof (struct priority));
    close (F_dt);
    return TRUE;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
    sxperror (ent_name);
    close (F_dt);
    return FALSE;
}
