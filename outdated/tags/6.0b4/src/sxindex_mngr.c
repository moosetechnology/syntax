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

char WHAT_SXINDEX_MNGR[] = "@(#)SYNTAX - $Id: sxindex_mngr.c 949 2008-01-22 10:20:32Z rlacroix $" WHAT_DEBUG;

void sxindex_alloc (sxindex_header *header,
		    SXINT line_nb,
		    SXINT column_nb,
		    void (*user_oflw) (sxindex_header *header, SXINT old_line_nb, SXINT old_size))
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
    header->set = (SXINT*) sxalloc (header->size, sizeof (SXINT));
    header->oflw = user_oflw;
    header->index = 0;
    header->free = -1;
}


SXINT sxindex_seek (sxindex_header *header)
{
    SXINT x, old_line_nb, old_size;

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
	    header->set = (SXINT*) sxrealloc (header->set, header->size *= 2, sizeof (SXINT));
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
static SXINT	bytes;


BOOLEAN sxindex_write (sxindex_header *header, filedesc_t file_descr)
{
    return
	WRITE (&header->size, sizeof (SXINT))
	&& WRITE (&header->index, sizeof (SXINT))
	&& WRITE (&header->free, sizeof (SXINT))
	&& WRITE (&header->mask, sizeof (SXINT))
	&& WRITE (&header->shift, sizeof (SXINT))
	&& WRITE (&header->line_nb, sizeof (SXINT))
	&& WRITE (&header->column_nb, sizeof (SXINT))
	&& WRITE (header->set, sizeof (SXINT)*header->index)
	;
}

BOOLEAN sxindex_read (sxindex_header *header,
		      filedesc_t file_descr,
		      void (*user_oflw) (sxindex_header *header, SXINT old_line_nb, SXINT old_size))
{
  BOOLEAN b_ret = 
	READ (&header->size, sizeof (SXINT))
	&& READ (&header->index, sizeof (SXINT))
	&& READ (&header->free, sizeof (SXINT))
	&& READ (&header->mask, sizeof (SXINT))
	&& READ (&header->shift, sizeof (SXINT))
	&& READ (&header->line_nb, sizeof (SXINT))
	&& READ (&header->column_nb, sizeof (SXINT))
	&& READ (header->set = (SXINT*) sxalloc (header->size, sizeof (SXINT)),
		 sizeof (SXINT)*header->index);
  if ( b_ret ) {
    header->oflw = user_oflw;
  }
  return b_ret;
}

static void
sxindex_out_tab_int (FILE *file, char *name1, char *name2, SXINT *tab, SXINT size, SXINT top)
{
    SXINT i;

    fprintf (file, "\n\nstatic SXINT %s%s [%ld] = {", name1, name2, (long)size);

    for (i = 0; i < top; i++) {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %ld */ ", (long)i);

	fprintf (file, "%ld, ", (long)tab [i]);
    } 

    fprintf (file, "\n} /* End %s%s */;\n", name1, name2);
}


void sxindex_array_to_c (sxindex_header *header, FILE *file, char *name)
{
    sxindex_out_tab_int (file, name, "_set", header->set, header->size, header->index);
}


void sxindex_header_to_c (sxindex_header *header, FILE *file, char *name)
{
    fprintf (file, "{ %s_set /* set */,\n", name);
    fprintf (file, "%ld /* size */, %ld /* index */,\n", (long)header->size, (long)header->index);
    fprintf (file, "%ld /* mask */, %ld /* shift */,\n", (long)header->mask, (long)header->shift);
    fprintf (file, "%ld /* line_nb */, %ld /* column_nb */,\n", (long)header->line_nb, (long)header->column_nb);
    fprintf (file, "%ld /* free */,\n", (long)header->free);
    fprintf (file, "NULL /* (*oflw) () */,\n");
    fprintf (file, "1 /* is_static */\n");
    fprintf (file, "}");
}



void sxindex_to_c (sxindex_header *header, FILE *file, char *name, BOOLEAN is_static)
{
    sxindex_array_to_c (header, file, name);
    fprintf (file, "\n\n%ssxindex_header %s =\n", is_static ? "static " : "", name);
    sxindex_header_to_c (header, file, name);
    fprintf (file, ";\n");
}



void sxindex_reuse (sxindex_header *header,
		    void (*user_oflw) (sxindex_header *header, SXINT old_line_nb, SXINT old_size))
{
    header->oflw = user_oflw;
}



