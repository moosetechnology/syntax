/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/










#include "sxversion.h"
#include "sxcommon.h"

char WHAT_SXSET_MNGR[] = "@(#)SYNTAX - $Id: sxset_mngr.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

#if EBUG
static char	ME [] = "sxset_mngr";
#endif


#include	"sxset.h"


void
sxset_alloc (sxset_header *header,
	     char *name,
	     SXUINT universe_size,
	     SXUINT sets_nb,
	     SXUINT average_elems_nb_per_set,
	     void (*sets_oflw) (sxset_header*, SXUINT, SXUINT),
	     void (*elems_oflw) (sxset_header*),
	     FILE *stat_file)
{
    /* Cette fonction "cree" un adresseur extensible d'ensembles de SXUINT */

    header->name = name;
    header->sparse = (SXUINT*) sxalloc (header->sparse_size = universe_size,
					       sizeof (SXUINT));
    header->sets = (struct sxset_sets*) sxalloc (header->sets_size = sets_nb,
					     sizeof (struct sxset_sets));
    header->elems =
	(SXUINT*) sxalloc (header->elems_size = sets_nb * average_elems_nb_per_set,
				  sizeof (SXUINT));

    sxset_clear (*header);

    header->sets_oflw = sets_oflw;
    header->elems_oflw = elems_oflw;
    header->stat_file = stat_file;
}


void
sxset_stat (sxset_header *header, FILE *stat_file)
{
    SXUINT	total_size, total_top, sets_top = 0;
    SXUINT       x;

    for (x = header->free_list; x != sxset_nil; x = header->sets [x&sxset_7F].bot)
	sets_top++;

    total_size = sizeof (sxset_header) + sizeof (struct sxset_sets) * header->sets_size
	+ sizeof (SXUINT) * header->elems_size;
    total_top = sizeof (sxset_header) + sizeof (struct sxset_sets) * (header->sets_top - sets_top)
	+ sizeof (SXUINT) * header->elems_top;

    fprintf (stat_file, "name = %s, Total_size (byte) = %ld (%ld%%)\n",
		 header->name, (long)total_size, (long)(100*total_top)/total_size);
}
    
    
    
void
sxset_free (sxset_header *header)
{
    if (header->stat_file != NULL)
	sxset_stat (header, header->stat_file);
    
    sxfree (header->sparse), header->sparse = NULL;
    sxfree (header->sets), header->sets = NULL;
    sxfree (header->elems), header->elems = NULL;
}




SXBOOLEAN
sxset_compare (sxset_header *header, SXUINT set_id)
{
    /* Compare set_id with header->current_set_id */
    SXUINT	x, n, elem;

    if (sxset_card (*header, header->current_set_id) != sxset_card (*header, set_id))
	return SXFALSE;

    sxset_foreach (*header, set_id, x, n, elem) {
	if (!sxset_member (header, header->current_set_id, elem))
	    return SXFALSE;
    }

    return SXTRUE;
}




void
sxset_pack_sets (sxset_header *header)
{
    SXUINT	x;

    if (header->erased_set_nb > 0) {
	for (x = 0; x < header->sets_top; x++) {
	    if ((header->sets [x].bot & sxset_80) && ((header->sets [x].card & sxset_80) == 0)) {
		/* Erased set not already in the free_list */
		header->sets [x].card |= sxset_80; /* Erased and belongs to the free_list */
		header->sets [x].bot = header->free_list;
		header->free_list = (x | sxset_80);
	    }
	}

	header->erased_set_nb = 0;
    }
}


static void
sxset_reactivate (sxset_header *header, SXUINT set_id)
{
    /* Fill sparse with "set_id". */
    SXUINT	x, n, elem;

    sxset_foreach (*header, set_id, x, n, elem) {
	header->sparse [elem] = x;
    }

    header->current_set_id = set_id;
}


void
sxset_local_pack (sxset_header *header)
{
    /* Pack the current active set */
    SXUINT	x, n, elem, *to, *from, *bot;

    to = from = bot = header->elems + header->sets [header->current_set_id].bot;

    sxset_foreach (*header, header->current_set_id, x, n, elem) {
	if (!sxset_is_member_deleted (*header, elem)){
	    if (to != from) {
		header->sparse [elem] = from - header->elems; /* Reactivation */
		*from = elem;
	    }

	    to++;
	}

	from++;
    }

    if (to != from) {
	header->sets [header->current_set_id].card = to - bot;
	header->elems_top = to - header->elems;
    }
}


void
sxset_pack_elems (sxset_header *header)
{
    /* Pack both sets and elems */
    SXUINT	x, *new_elems, *bot, *new_bot, *top, *new_top;

    if (header->erased_set_nb > 0 || header->erased_elem_nb > 0) {
	new_bot = new_top = 
	    new_elems = (SXUINT *) sxalloc (header->elems_size, sizeof (SXUINT));
	
	for (x = 0; x < header->sets_top; x++) {
	    if ((header->sets [x].bot & sxset_80) == 0) {
		/* Not erased set, copied */
		/* Elems are marked */
		bot = header->elems + (header->sets [x].bot & sxset_7F);
		top = bot + header->sets [x].card;
		header->sets [x].bot = new_top - new_elems;

		while (bot < top) {
		    if ((*bot & sxset_80) == 0) {
			*new_top++ = *bot;
		    }

		    bot++;
		}

		header->sets [x].card = new_top - new_bot;
		new_bot = new_top;
	    }
	    else
		if ((header->sets [x].card & sxset_80) == 0) {
		    /* Erased set, not in the free_list */
		    header->sets [x].card |= sxset_80; /* Erased and belongs to the free_list */
		    header->sets [x].bot = header->free_list;
		    header->free_list = (x | sxset_80);
		}
	}

	sxfree (header->elems);
	header->elems = new_elems;
	header->elems_top = new_top - new_elems;
	header->erased_elem_nb = 0;
	header->erased_set_nb = 0;

	/* Who knows! */
	sxset_reactivate (header, header->current_set_id);
    }
}

				 

SXUINT
sxset_new_set_id (sxset_header *header)
{
    SXUINT	old_size;

    if (header->sets_top >= (old_size = header->sets_size)) {
	if (header->free_list == sxset_nil) {
	    if (header->erased_set_nb < (old_size >> 2)) {
		/* Less than 25% of erased set, reallocation. */
		header->sets = (struct sxset_sets*) sxrealloc (header->sets,
							       header->sets_size *= 2,
							       sizeof (struct sxset_sets));

		if (header->sets_oflw != NULL)
		    (*header->sets_oflw) (header, old_size, header->sets_size);

		header->current_set_id = header->sets_top;
	    }
	    else {
		sxset_pack_sets (header);
		header->current_set_id = header->free_list & sxset_7F;
		header->free_list = header->sets [header->current_set_id].bot;
	    }
	}
	else {
	    header->current_set_id = header->free_list & sxset_7F;
	    header->free_list = header->sets [header->current_set_id].bot;
	}
    }
    else {
	header->current_set_id = header->sets_top++;
    }
	    
    header->sets [header->current_set_id].card = 0;
    header->sets [header->current_set_id].bot = header->elems_top;

    return header->current_set_id;
}

#if EBUG
extern void sxtrap (char *caller, char *message);
#endif

SXBOOLEAN
sxset_member (sxset_header *header, SXUINT set_id, SXUINT elem)
{
    SXUINT	val;

#if EBUG
    /* may only be used while "set_id" is active */
    if (header->current_set_id != set_id)
	sxtrap (ME, "sxset_member");
#endif

    if (elem >= header->sparse_size)
	return SXFALSE;

    val = header->sparse [elem];

    return val >= header->sets [set_id].bot &&
	val < header->elems_top &&
	header->elems [val] == elem;
}

static void
sxset_elems_oflw (sxset_header *header)
{
    /* header->elems_top == header->elems_size */

    if (header->erased_elem_nb < (header->elems_size >> 2)) {
	/* Less than 25% of erased elems, reallocation. */
	header->elems = (SXUINT*) sxrealloc (header->elems,
						    header->elems_size *= 2,
						    sizeof (SXUINT));

	if (header->sets_oflw != NULL)
	    (*header->sets_oflw) (header,
				  header->elems_top /* old_size */,
				  header->elems_size /* new_size */);
		
    }
    else
	sxset_pack_elems (header);
}


void
sxset_add_member (sxset_header *header, SXUINT elem)
{
    SXUINT	val;
    SXBOOLEAN		not_a_member;

    if (elem >= header->sparse_size) {
	header->sparse_size *= 2;

	if (elem >= header->sparse_size)
	    header->sparse_size = elem + 1;

	header->sparse = (SXUINT*) sxrealloc (header->sparse,
						     header->sparse_size,
						     sizeof (SXUINT));
	not_a_member = SXTRUE;
    }
    else {
	val = header->sparse [elem];
	not_a_member = (val < header->sets [header->current_set_id].bot ||
			val >= header->elems_top ||
			header->elems [val] != elem);
    }

    if (not_a_member) {
	header->sparse [elem] = header->elems_top;
	header->sets [header->current_set_id].card++;
	header->elems [header->elems_top++] = elem;

	if (header->elems_top == header->elems_size)
	    sxset_elems_oflw (header);
    }
}



void
sxset_copy (sxset_header *header, SXUINT set_id)
{
    /* Copy (add or union) set_id in header->current_set_id */
    /* Assume that current_set_id is correct and has been set by sxset_new_set_id */
    SXUINT	x, n, elem;

    sxset_foreach (*header, set_id, x, n, elem) {
	sxset_add_member (header, elem);
    }
}

void
sxset_and (sxset_header *header, SXUINT set_id)
{
    /* header->current_set_id = header->current_set_id & set_id */
    /* Assume header->current_set_id has no erased elem. */
    SXUINT	x, n1, n2, elem, val, *to, *from, *bot;

    to = bot = header->elems + header->sets [header->current_set_id].bot;
    n1 = sxset_card (*header, header->current_set_id);

    if (n1 > 0 && sxset_card (*header, set_id)) {
	sxset_foreach (*header, set_id, x, n2, elem) {
	    val = header->sparse [elem];

	    if (val < header->elems_top) {
		from = header->elems + val;

		if (from >= bot && *from == elem) {
		    /* is in the intersection */
		    *from |= sxset_80; /* marked */
		}
	    }
	}

	from = bot;

	while (n1 > 0) {
	    if (*from & sxset_80) {
		*to = (*from & sxset_7F);
		to++;
	    }
	
	    from++;
	    n1--;
	}	
    }

    header->sets [header->current_set_id].card = to - bot;
    header->elems_top = to - header->elems;
}



void
sxset_minus (sxset_header *header, SXUINT set_id)
{
    /* header->current_set_id = header->current_set_id - set_id */
    SXUINT	x, n1, n2, elem, val, *to, *from, *bot;

    n1 = sxset_card (*header, header->current_set_id);

    if (n1 && sxset_card (*header, set_id)) {
	/* Non empty */
	bot = header->elems + header->sets [header->current_set_id].bot;

	sxset_foreach (*header, set_id, x, n2, elem) {
	    val = header->sparse [elem];

	    if (val < header->elems_top) {
		from = header->elems + val;

		if (from >= bot && *from == elem) {
		    /* is in the intersection */
		    *from |= sxset_80; /* marked */
		}
	    }
	}

	to = from = bot;

	while (n1 > 0) {
	    if ((*from & sxset_80) == 0) {
		*to = (*from & sxset_7F);
		to++;
	    }
	
	    from++;
	    n1--;
	}	

	header->sets [header->current_set_id].card = to - bot;
	header->elems_top = to - header->elems;
    }
}

SXUINT
sxset_not (sxset_header *header, SXUINT set_id)
{
    /* header->current_set_id = ~set_id */
    SXUINT	x, elem;

    sxset_new_set_id (header); /* Result == header->current_set_id */
    sxset_reactivate (header, set_id);

    for (x = 0; x < header->sparse_size; x++) {
	elem = header->sparse [x];

	if (!sxset_member (header, set_id, elem)) {
	    header->sets [header->current_set_id].card++;
	    header->elems [header->elems_top++] = elem;
	
	    if (header->elems_top == header->elems_size)
		sxset_elems_oflw (header);
	}
    }

    sxset_reactivate (header, header->current_set_id);

    return header->current_set_id;
}

