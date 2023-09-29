/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)prio_read.c\t- SYNTAX [unix] - 3 Avril 1987";

#include "sxunix.h"
#include "D_tables.h"



#define READ(f,p,l)		\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error


BOOLEAN		prio_read (t_priorities_size, r_priorities_size, t_priorities, r_priorities, langname)
    int		t_priorities_size, r_priorities_size;
    struct priority	**t_priorities, **r_priorities;
    char	*langname;
{
    register int	bytes;
    register int	/* file descriptor */ F_dt;
    int		dt_version;
    char	ent_name [128];
    static char		ME [] = "prio_read";


/* allocation et lecture des tables de prio */

    *t_priorities = (struct priority*) sxcalloc (t_priorities_size, sizeof (struct priority));
    *r_priorities = (struct priority*) sxcalloc (r_priorities_size, sizeof (struct priority));

    if ((F_dt = open (strcat (strcpy (ent_name, langname), ".dt"), 0)) < 0) {
	/* No message: this is normal */
	return FALSE;
    }

    READ (F_dt, &(dt_version), sizeof (int));

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
