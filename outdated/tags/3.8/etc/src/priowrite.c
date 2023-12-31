/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)prio_write.c\t- SYNTAX [unix] - 31 Mai 1988";

#include "sxunix.h"
#include "D_tables.h"



#define WRITE(f,p,l)		\
	if (bytes = (l), (write (f, p, bytes) != bytes))	\
		goto write_error


BOOLEAN		prio_write (t_priorities_size, r_priorities_size, t_priorities, r_priorities, langname)
    int		t_priorities_size, r_priorities_size;
    struct priority	*t_priorities, *r_priorities;
    char	*langname;
{
    register int	bytes;
    register int	/* file descriptor */ F_dt;
    int		dt_version;
    char	ent_name [128];
    static char		ME [] = "prio_write";


/* sortie des tables de prio */

    if ((F_dt = creat (strcat (strcpy (ent_name, langname), ".dt"), 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	goto file_error;
    }

    dt_version = dt_num_version;
    /* numero de version des tables */

    WRITE (F_dt, &(dt_version), sizeof (int));
    WRITE (F_dt, t_priorities, t_priorities_size * sizeof (struct priority));
    WRITE (F_dt, r_priorities, r_priorities_size * sizeof (struct priority));
    close (F_dt);
    return TRUE;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (ent_name);
    return FALSE;
}
