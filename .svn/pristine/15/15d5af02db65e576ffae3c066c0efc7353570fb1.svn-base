/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1992 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 26-11-92 11:56 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/


static char	what [] = "@(#)sxsymbol_table_mngr.c	- SYNTAX [unix] - 25 Janvier 1993";
static char	ME [] = "sxsymbol_table_mngr";

#include "sxsymbol_table.h"


static struct sxsymbol_table_hd	*stp_;

static void scoped_entries_oflw (old_size, new_size)
    int old_size, new_size;
{
    if (stp_->indexes != NULL)
	stp_->indexes = (int*) sxrealloc (stp_->indexes, new_size + 1, sizeof (int));    

    if (stp_->oflw != NULL)
	(*stp_->oflw) (old_size, new_size);
}

static void nested_access_oflw (header, old_size, new_size)
    struct sxsymbol_table_hd	*header;
    int old_size, new_size;
{
    int	*top, *access;

    header->nested_access = (int*)
	sxrealloc (header->nested_access,
		   (header->nested_access_size = new_size) + 1,
		   sizeof (int));

    top = header->nested_access + new_size;
    access = header->nested_access + old_size;

    while (++access <= top)
	*access = 0;
}

void sxsymbol_table_alloc (header, nested_size, scoped_size, user_oflw)
    struct sxsymbol_table_hd	*header;
    int				nested_size, scoped_size;
    void			(*user_oflw) ();
{
    XxY_alloc (&(header->scoped_access), "scoped_access", scoped_size, 3, 0, 1, scoped_entries_oflw, NULL);

    if ((header->nested_access_size = nested_size) > 0) {
	header->nested_access = (int*)
	    sxcalloc (nested_size + 1, sizeof (int));
	header->indexes = (int*)
	    sxalloc (XxY_size (header->scoped_access) + 1, sizeof (int));    
	header->indexes [0] = 0; /* For unknown entries in scoped_access */
    }
    else {
	header->nested_access = header->indexes = NULL;
    }

    header->oflw = user_oflw;
}

void sxsymbol_table_free (header)
    struct sxsymbol_table_hd	*header;
{
    XxY_free (&(header->scoped_access));

    if (header->nested_access != NULL) {
	sxfree (header->nested_access), header->nested_access = NULL;
	sxfree (header->indexes);
    }
}


BOOLEAN sxsymbol_table_put (header, name, scope_nb, index) 
    struct sxsymbol_table_hd	*header;
    int 			name, scope_nb;
    int				*index;
{
    /* In all cases sets *index with a unique integer for (name, scope_nb).
       returns FALSE iff (name, scope_nb) already exists. */
    int		*p;

    stp_ = header;

    if (XxY_set (&(header->scoped_access), name, scope_nb, index))
	return FALSE;

    if (header->nested_access != NULL) {
	if (name > header->nested_access_size)
	    nested_access_oflw (header, header->nested_access_size, name + header->nested_access_size);

	header->indexes [*index] = *(p = header->nested_access + name);
	*p = *index;
    }

    return TRUE;
}


void sxsymbol_table_erase (header, index, all)
    struct sxsymbol_table_hd	*header;
    int 			index;
    BOOLEAN			all;
{
    /* If all is TRUE, recovers space. */
    int name;

    name = XxY_X (header->scoped_access, index);
    
    if (header->nested_access != NULL && header->nested_access [name] == index)
	header->nested_access [name] = header->indexes [index];
    /* else already erased by a previous call! */
    
    if (all)
	XxY_erase (header->scoped_access, index);
}



void sxsymbol_table_close (header, scope_nb, all)
    struct sxsymbol_table_hd	*header;
    int 			scope_nb;
    BOOLEAN			all;
{
    int index;

    XxY_Yforeach (header->scoped_access, scope_nb, index) {
	sxsymbol_table_erase (header, index, all);
    }
}
