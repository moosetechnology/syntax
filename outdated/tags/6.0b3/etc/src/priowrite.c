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
char WHAT_PRIOWRITE[] = "@(#)SYNTAX - $Id: priowrite.c 1215 2008-03-13 14:06:31Z rlacroix $" WHAT_DEBUG;



#define WRITE(f,p,l)		\
	if (bytes = (l), (write (f, p, bytes) != bytes))	\
		goto write_error


BOOLEAN		prio_write (SXINT t_priorities_size, 
			    SXINT r_priorities_size,
			    struct priority *t_priorities,
			    struct priority *r_priorities,
			    char *langname)
{
    SXINT	bytes;
    filedesc_t	/* file descriptor */ F_dt;
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
    return TRUE;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (ent_name);
    return FALSE;
}
