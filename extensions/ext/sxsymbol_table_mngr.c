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
#include "sxcommon.h"
#include "sxsymbol_table.h"

char WHAT_SXSYMBOL_TABLE_MNGR[] = "@(#)SYNTAX - $Id: sxsymbol_table_mngr.c 3917 2024-04-23 08:33:57Z garavel $" WHAT_DEBUG;


static struct sxsymbol_table_hd	*stp_;

static void scoped_entries_oflw (SXINT old_size, SXINT new_size)
{
    if (stp_->indexes != NULL)
	stp_->indexes = (SXINT*) sxrealloc (stp_->indexes, new_size + 1, sizeof (SXINT));    

    if (stp_->oflw != NULL)
	(*stp_->oflw) (old_size, new_size);
}

static void nested_access_oflw (struct sxsymbol_table_hd *header, SXINT old_size, SXINT new_size)
{
    SXINT	*top, *nested_access_oflw_access;

    header->nested_access = (SXINT*)
	sxrealloc (header->nested_access,
		   (header->nested_access_size = new_size) + 1,
		   sizeof (SXINT));

    top = header->nested_access + new_size;
    nested_access_oflw_access = header->nested_access + old_size;

    while (++nested_access_oflw_access <= top)
	*nested_access_oflw_access = 0;
}

void sxsymbol_table_alloc (struct sxsymbol_table_hd *header, SXINT nested_size, SXINT scoped_size, void (*user_oflw) (SXINT, SXINT))
{
    XxY_alloc (&(header->scoped_access), "scoped_access", scoped_size, 3, 0, 1, scoped_entries_oflw, NULL);

    if ((header->nested_access_size = nested_size) > 0) {
	header->nested_access = (SXINT*)
	    sxcalloc ((SXUINT)nested_size + 1, sizeof (SXINT));
	header->indexes = (SXINT*)
	    sxalloc (XxY_size (header->scoped_access) + 1, sizeof (SXINT));    
	header->indexes [0] = 0; /* For unknown entries in scoped_access */
    }
    else {
	header->nested_access = header->indexes = NULL;
    }

    header->oflw = user_oflw;
}

void sxsymbol_table_free (struct sxsymbol_table_hd *header)
{
    XxY_free (&(header->scoped_access));

    if (header->nested_access != NULL) {
	sxfree (header->nested_access), header->nested_access = NULL;
	sxfree (header->indexes);
    }
}


bool sxsymbol_table_put (struct sxsymbol_table_hd *header, SXINT name, SXINT scope_nb, SXINT *sxsymbol_table_put_index)
{
    /* In all cases sets *index with a unique integer for (name, scope_nb).
       returns false iff (name, scope_nb) already exists. */
    SXINT		*p;

    stp_ = header;

    if (XxY_set (&(header->scoped_access), name, scope_nb, sxsymbol_table_put_index))
	return false;

    if (header->nested_access != NULL) {
	if (name > header->nested_access_size)
	    nested_access_oflw (header, header->nested_access_size, name + header->nested_access_size);

	header->indexes [*sxsymbol_table_put_index] = *(p = header->nested_access + name);
	*p = *sxsymbol_table_put_index;
    }

    return true;
}


void sxsymbol_table_erase (struct sxsymbol_table_hd *header, SXINT sxsymbol_table_erase_index, bool all)
{
    /* If all is true, recovers space. */
    SXINT name;

    name = XxY_X (header->scoped_access, sxsymbol_table_erase_index);
    
    if (header->nested_access != NULL && header->nested_access [name] == sxsymbol_table_erase_index)
	header->nested_access [name] = header->indexes [sxsymbol_table_erase_index];
    /* else already erased by a previous call! */
    
    if (all)
	XxY_erase (header->scoped_access, sxsymbol_table_erase_index);
}



void sxsymbol_table_close (struct sxsymbol_table_hd *header, SXINT scope_nb, bool all)
{
    SXINT sxsymbol_table_close_index;

    XxY_Yforeach (header->scoped_access, scope_nb, sxsymbol_table_close_index) {
	sxsymbol_table_erase (header, sxsymbol_table_close_index, all);
    }
}
