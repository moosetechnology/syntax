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

/* This package allows the handling of indexes for vectors or matrices.
   Vector indexes are integers and Matrix indexes are couples of integers
   ( V [i] or M [i] [j]).
   Brand new indexes can be seeked or already used indexes can be released.
   The user can manage the (concurrent) arrays (V or M), since he is warns when an
   overflow occurs.
   In the Matrix case, when an overflow occurs, the number of line is increased. */

#include "sxversion.h"
#include "sxcommon.h"
#include "sxindex.h"
#include <stdio.h>
#include <unistd.h>

char WHAT_SXINDEX_MNGR[] = "@(#)SYNTAX - $Id: sxindex_mngr.c 2947 2023-03-29 17:06:41Z garavel $" WHAT_DEBUG;

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


SXBOOLEAN sxindex_write (sxindex_header *header, sxfiledesc_t file_descr)
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

SXBOOLEAN sxindex_read (sxindex_header *header,
		      sxfiledesc_t file_descr,
		      void (*user_oflw) (sxindex_header *header, SXINT old_line_nb, SXINT old_size))
{
  SXBOOLEAN b_ret = 
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

    fprintf (file, "\n\nstatic SXINT %s%s [%ld] = {", name1, name2, (SXINT) size);

    for (i = 0; i < top; i++) {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %ld */ ", (SXINT) i);

	fprintf (file, "%ld, ", (SXINT) tab [i]);
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
    fprintf (file, "%ld /* size */, %ld /* index */,\n", (SXINT) header->size, (SXINT) header->index);
    fprintf (file, "%ld /* mask */, %ld /* shift */,\n", (SXINT) header->mask, (SXINT) header->shift);
    fprintf (file, "%ld /* line_nb */, %ld /* column_nb */,\n", (SXINT) header->line_nb, (SXINT) header->column_nb);
    fprintf (file, "%ld /* free */,\n", (SXINT) header->free);
    fprintf (file, "NULL /* (*oflw) () */,\n");
    fprintf (file, "1 /* is_static */\n");
    fprintf (file, "}");
}



void sxindex_to_c (sxindex_header *header, FILE *file, char *name, SXBOOLEAN is_static)
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



