/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */


#define NODE struct node_header_s


#include "sxunix.h"

char WHAT_SXSMP[] = "@(#)SYNTAX - $Id: sxsmp.c 866 2007-11-06 08:58:15Z rlacroix $" WHAT_DEBUG;




VOID	sxsmp (NODE *root, VOID (*pass_inherited) (void), VOID (*pass_derived) (void))
{
    NODE	*node_ptr;
    BOOLEAN	done;

    if (root == NULL)
	return;

    done = FALSE;
    sxnext_visit.normal = TRUE;

    if ((VISITED = root->son) != NULL) {
	VISIT_KIND = INHERITED;
    }
    else {
	VISITED = root;
	VISIT_KIND = DERIVED;
    }

    do {
	if (VISIT_KIND == INHERITED) {
	    (*pass_inherited) ();

	    if ((node_ptr = VISITED->son) != NULL) {
		VISITED = node_ptr;
		LEFT = NULL;
	    }
	    else {
		VISIT_KIND = DERIVED;
		LAST_ELEM = NULL;
	    }
	}
	else {
	    (*pass_derived) ();

	    if (VISITED == root) {
		done = TRUE;
	    }
	    else if ((node_ptr = VISITED->brother) != NULL) {
		VISIT_KIND = INHERITED;
		LEFT = VISITED;
		VISITED = node_ptr;
	    }
	    else {
		LAST_ELEM = VISITED;
		VISITED = VISITED->father;
	    }
	}

	if (!sxnext_visit.normal) {
	    done = FALSE;
	    sxnext_visit.normal = TRUE;
	    VISITED = sxnext_visit.visited;

	    if ((VISIT_KIND = sxnext_visit.visit_kind) == INHERITED) {
		LEFT = sxbrother (VISITED, VISITED->position - 1);
	    }
	    else {
		LAST_ELEM = sxson (VISITED, VISITED->degree);
	    }
	}
    } while (!done);
}
