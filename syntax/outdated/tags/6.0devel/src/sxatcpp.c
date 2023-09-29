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
#include "sxunix.h"

char WHAT_SXATCPP[] = "@(#)SYNTAX - $Id: sxatcpp.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

/* Gestion dans le tas : */

static SXINT	stack_size;
struct tree_node_stack {
    struct sxnode_pp	*head, *tail;
};
static struct tree_node_stack	*tree_node_stack /* 1:stack_size */ ;

#define allocate_node() (node == area_end ? allocate_area () : ++node)

#ifndef NBNODES
# define NBNODES 128
#endif

#ifdef SXRESIZE
  /* Increase area_size, to minimize number of allocations */
# define NEW_AREA_SIZE (area_size += NBNODES)
#else
  /* Do not grow area_size, as it cannot be reallocated */
# define NEW_AREA_SIZE (area_size = NBNODES)
#endif

struct sxatcpp_area {
    struct sxatcpp_area	*next;
    struct sxnode_pp	zone [1];
};
static long	area_size;
static struct sxatcpp_area	*area;
static struct sxnode_pp	*area_end, *node;



static struct sxnode_pp	*allocate_area (void)
{
    struct sxatcpp_area	*old, *new;

    old = area;
    area = new = (struct sxatcpp_area*) sxcalloc (1, sizeof (struct sxatcpp_area) + NEW_AREA_SIZE * sizeof (struct sxnode_pp));
    new->next = old;
    area_end = (node = new->zone) + area_size;
    return node;
}



static SXVOID	free_areas (void)
{
    struct sxatcpp_area	*prev, *next;
    next = area;

    while ((prev = next) != NULL) {
	next = next->next;
	sxfree (prev);
    }
}



static struct sxnode_pp	*father_ptr, *prv_ptr;
static char	*com_ref;




#define cons_com(c1,c2) (c1==NULL?c2:(c2==NULL?c1:string_cat(c1,c2)))

static char	*string_cat (char *s1, char *s2)
{
    char	*t1, *t2;
    size_t	l1;

    s1 = (char*) sxrealloc (s1, (l1 = strlen (s1)) + strlen (s2) + 1, sizeof (char));
    t1 = s1 + l1, t2 = s2;

    while ((*t1++ = *t2++) != SXNUL) {
	/* null; */
    }

    sxfree (s2);
    return s1;
}



static SXVOID	initialize (void)
{
    stack_size = 128;
    area_size = 0;
    area = NULL;
    node = area_end = NULL;
    father_ptr = NULL;
    tree_node_stack = (struct tree_node_stack*) sxalloc (stack_size + 1, sizeof (struct tree_node_stack));
}



static SXVOID	finalize (void)
{
/* le commentaire eventuel se trouvant en fin de  */
/* programme devient le post_comment de la racine */
    father_ptr->post_comment = cons_com (father_ptr->post_comment, sxsvar.sxlv.terminal_token.comment);

#ifdef SXRESIZE
    if (area != NULL) {
	SXRESIZE (area, 1, sizeof (struct sxatcpp_area) + (area_size = (area_end = node) - area->zone) * sizeof (struct sxnode_pp));
    }
#endif

    sxfree (tree_node_stack);
}



static SXVOID	action (void)
{
    SXINT	top, new_top, xs;
    struct sxtoken	*token;
    struct sxnode_pp	*brother, *son, *father;
    SXBOOLEAN	is_first = SXTRUE;

    top = SXSTACKtop ();
    new_top = SXSTACKnewtop ();

    if (top >= stack_size)
	tree_node_stack = (struct tree_node_stack*) sxrealloc (tree_node_stack, (stack_size *= 2) + 1, sizeof (struct
	     tree_node_stack));

    son = father = allocate_node ();
    father->name = SXSTACKreduce ();

/* Initialisation: on met a priori le source_index du look_ahead */

    father->terminal_token.source_index = SXGET_TOKEN (sxplocals.atok_no).source_index;
    com_ref = NULL;
    prv_ptr = NULL;

    for (xs = new_top; xs <= top; xs++) {
	token = &(SXSTACKtoken (xs));

	if (token->lahead >= 0 /* nt or t */ ) {
	    if (is_first)
		father->terminal_token.source_index = token->source_index;

	    if (token->lahead == 0) {
		/* non-terminal */
		brother = son->next = tree_node_stack [xs].head;
		son = tree_node_stack [xs].tail;
	    }
	    else if (sxgenericp (sxplocals.sxtables, token->lahead)) {
		son = son->next = brother = allocate_node ();
		brother->terminal_token = *token;
		brother->name = -token->lahead;
	    }
	    else
		brother = NULL;

	    if (brother != NULL) {
		brother->terminal_token.comment = cons_com (com_ref, brother->terminal_token.comment);
		com_ref = NULL;
	    }
	    else if (is_first)
		father->terminal_token.comment = token->comment;
	    else if (prv_ptr != NULL)
		prv_ptr->post_comment = cons_com (prv_ptr->post_comment, token->comment);
	    else
		com_ref = cons_com (com_ref, token->comment);

	    prv_ptr = brother;
	    is_first = SXFALSE;
	}
    }

    father->post_comment = com_ref;
    tree_node_stack [new_top].head = father_ptr = father;
    tree_node_stack [new_top].tail = son;
}



static SXVOID	error (void)
{
    father_ptr = allocate_node ();
    father_ptr->name = 0 /* error node */ ;
    tree_node_stack [SXSTACKnewtop ()].head = tree_node_stack [SXSTACKnewtop ()].tail = father_ptr;
}



SXVOID	sxatcpp (SXINT sxatcpp_what, struct sxtables *arg /* or action number */)
{
    switch (sxatcpp_what) {
    case SXOPEN:
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, "atcpp");

    case SXCLOSE:
	sxppp (sxatcpp_what, arg);
	break;

    case SXINIT:
	initialize ();
	break;

    case SXFINAL:
	finalize ();
	break;

    case SXACTION:
	if ((long) arg != 0) {	/* 0 when simple reduction */
	    action ();
	}
	break;

    case SXSEMPASS:
        if (sxerrmngr.nbmess [2] != 0)
	  break;

	if (sxverbosep) {
	    fputs ("Pretty Printer ", sxtty);
	}

	/* dans le cas ou sxppp() est appelee avec un premier parametre valant
        * SXACTION, le deuxieme parametre, formellement declare avec le type
        * "(struct sxtables *)", doit etre de type "(struct sxnode_pp *)" en
        * realite : l'interieur de la fonction sxppp() contient un cast dans
        * ce cas */
	sxppp (SXACTION, (struct sxtables *) father_ptr);
	free_areas ();
	break;

    case SXERROR:
	error ();
	break;

    default:
	fprintf (sxstderr, "The function \"sxatcpp\" is out of date with respect to its specification.\n");
	sxexit(1);
    }
}
