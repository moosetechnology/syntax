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
/* 20030506 10:21 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 05-04-94 11:25 (pb):		Memorisation des index ds un tableau	*/
/*				d'entiers (+ de 20 fois + rapide)	*/
/************************************************************************/
/* 15-02-94 15:17 (pb):		Ajout des lecture et ecritures		*/
/************************************************************************/
/* 07-06-93 11:15 (pb):		Ajout "old_line_nb" et "old_size" au	*/
/*				params de oflw				*/
/************************************************************************/
/* 26-11-92 09:36 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/




/* This package allows the handling of indexes for vectors or matrices.
   Vector indexes are integers and Matrix indexes are couples of integers
   ( V [i] or M [i] [j]).
   Brand new indexes can be seeked or already used indexes can be released.
   The user can manage the (concurrent) arrays (V or M), since he is warns when an
   overflow occurs.
   In the Matrix case, when an overflow occurs, the number of line is increased. */


#include "sxcommon.h"
#include "sxindex.h"
#include <stdio.h>
#include <unistd.h>

char WHAT_SXINDEX_MNGR[] = "@(#)SYNTAX - $Id: sxindex_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

void sxindex_alloc (header, line_nb, column_nb, user_oflw)
    sxindex_header	*header;
    int			line_nb, column_nb;
    void		(*user_oflw) ();
{
    header->shift = 0;

    if ((header->line_nb = line_nb) == 0) {
	header->mask = -1;
	header->column_nb = column_nb;
	header->line_nb = 1;
    }
    else {
	column_nb--;

	while (column_nb != 0) {
	   header->shift++;
	   column_nb >>= 1;
	}

	header->column_nb = 1 << header->shift;
	header->mask = header->column_nb - 1;
	/* Si 2**(p-1) < column_nb <= 2**p
	   alors header->column_nb == 2**p, header->shift == p et header->mask == 2**p - 1 */
    }

    header->size = header->line_nb * header->column_nb;
    header->set = (int*) sxalloc (header->size, sizeof (int));
    header->oflw = user_oflw;
    header->index = 0;
    header->free = -1;
}


int sxindex_seek (header)
    sxindex_header	*header;
{
    int x, old_line_nb, old_size;

    if (header->index == header->size) {
	if ((x = header->free) == -1) {
	    /* Tous les index sont occupes. */
	    old_line_nb = header->line_nb;

	    if (header->mask != -1)
		/* Matrice */
		header->line_nb *= 2;
	    else
		header->column_nb *= 2;

	    old_size = header->size;
	    header->set = (int*) sxrealloc (header->set, header->size *= 2, sizeof (int));
	    x = header->index++;

	    if (header->oflw != NULL)
		(*header->oflw) (header, old_line_nb, old_size);
	}
	else
	    header->free = header->set [x];
    }
    else
	x = header->index++;

    header->set [x] = -2;	/* Occupe' */
    return x;
}


#define WRITE(p,l)	((bytes=(l))>0&&(write(file_descr, p, (size_t) bytes) == bytes))
#define READ(p,l)	((bytes=(l))>0&&(read (file_descr, p, (size_t) bytes) == bytes))
static int	bytes;


BOOLEAN sxindex_write (header, file_descr)
    sxindex_header	*header;
    int			file_descr;
{
    return
	WRITE (&header->size, sizeof (int))
	&& WRITE (&header->index, sizeof (int))
	&& WRITE (&header->free, sizeof (int))
	&& WRITE (&header->mask, sizeof (int))
	&& WRITE (&header->shift, sizeof (int))
	&& WRITE (&header->line_nb, sizeof (int))
	&& WRITE (&header->column_nb, sizeof (int))
	&& WRITE (header->set, sizeof (int)*header->index)
	;
}


BOOLEAN sxindex_read (header, file_descr, user_oflw)
    sxindex_header	*header;
    int			file_descr;
    void		(*user_oflw) ();
{
  BOOLEAN b_ret = 
	READ (&header->size, sizeof (int))
	&& READ (&header->index, sizeof (int))
	&& READ (&header->free, sizeof (int))
	&& READ (&header->mask, sizeof (int))
	&& READ (&header->shift, sizeof (int))
	&& READ (&header->line_nb, sizeof (int))
	&& READ (&header->column_nb, sizeof (int))
	&& READ (header->set = (int*) sxalloc (header->size, sizeof (int)),
		 sizeof (int)*header->index);
  if ( b_ret ) {
    header->oflw = user_oflw;
  }
  return b_ret;
}

static void
sxindex_out_tab_int (file, name1, name2, tab, size, top)
    FILE	*file;
    char	*name1, *name2;
    int		*tab, size, top;
{
    int i;

    fprintf (file, "\n\nstatic int %s%s [%i] = {", name1, name2, size);

    for (i = 0; i < top; i++) {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %i */ ", i);

	fprintf (file, "%i, ", tab [i]);
    } 

    fprintf (file, "\n} /* End %s%s */;\n", name1, name2);
}


void sxindex_array_to_c (header, file, name)
    sxindex_header	*header;
    FILE		*file;
    char		*name;
{
    sxindex_out_tab_int (file, name, "_set", header->set, header->size, header->index);
}


void sxindex_header_to_c (header, file, name)
    sxindex_header	*header;
    FILE		*file;
    char		*name;
{
    fprintf (file, "{ %s_set /* set */,\n", name);
    fprintf (file, "%i /* size */, %i /* index */,\n", header->size, header->index);
    fprintf (file, "%i /* mask */, %i /* shift */,\n", header->mask, header->shift);
    fprintf (file, "%i /* line_nb */, %i /* column_nb */,\n", header->line_nb, header->column_nb);
    fprintf (file, "%i /* free */,\n", header->free);
    fprintf (file, "NULL /* (*oflw) () */,\n");
    fprintf (file, "1 /* is_static */\n");
    fprintf (file, "}");
}



void sxindex_to_c (header, file, name, is_static)
    sxindex_header	*header;
    FILE		*file;
    char		*name;
    BOOLEAN		is_static;
{
    sxindex_array_to_c (header, file, name);
    fprintf (file, "\n\n%ssxindex_header %s =\n", is_static ? "static " : "", name);
    sxindex_header_to_c (header, file, name);
    fprintf (file, ";\n");
}



void sxindex_reuse (header, user_oflw)
    sxindex_header	*header;
    void		(*user_oflw) ();
{
    header->oflw = user_oflw;
}



