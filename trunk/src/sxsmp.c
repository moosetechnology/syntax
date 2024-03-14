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

#define SXNODE struct sxnode_header_s


#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXSMP[] = "@(#)SYNTAX - $Id: sxsmp.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

void	sxsmp (SXNODE *root, void (*pass_inherited) (void), void (*pass_derived) (void))
{
    SXNODE	*node_ptr;
    bool	done;

    if (root == NULL)
	return;

    done = false;
    sxnext_visit.normal = true;

    if ((SXVISITED = root->son) != NULL) {
	SXVISIT_KIND = SXINHERITED;
    }
    else {
	SXVISITED = root;
	SXVISIT_KIND = SXDERIVED;
    }

    do {
	if (SXVISIT_KIND == SXINHERITED) {
	    (*pass_inherited) ();

	    if ((node_ptr = SXVISITED->son) != NULL) {
		SXVISITED = node_ptr;
		SXLEFT = NULL;
	    }
	    else {
		SXVISIT_KIND = SXDERIVED;
		SXLAST_ELEM = NULL;
	    }
	}
	else {
	    (*pass_derived) ();

	    if (SXVISITED == root) {
		done = true;
	    }
	    else if ((node_ptr = SXVISITED->brother) != NULL) {
		SXVISIT_KIND = SXINHERITED;
		SXLEFT = SXVISITED;
		SXVISITED = node_ptr;
	    }
	    else {
		SXLAST_ELEM = SXVISITED;
		SXVISITED = SXVISITED->father;
	    }
	}

	if (!sxnext_visit.normal) {
	    done = false;
	    sxnext_visit.normal = true;
	    SXVISITED = sxnext_visit.visited;

	    if ((SXVISIT_KIND = sxnext_visit.visit_kind) == SXINHERITED) {
		SXLEFT = sxbrother (SXVISITED, SXVISITED->position - 1);
	    }
	    else {
		SXLAST_ELEM = sxson (SXVISITED, SXVISITED->degree);
	    }
	}
    } while (!done);
}
