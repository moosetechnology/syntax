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
#include "sxnd.h"

char WHAT_SXNDTOKEN_MNGR[] = "@(#)SYNTAX - $Id: sxndtoken_mngr.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

#if EBUG
# define stdout_or_NULL	stdout
#else
# define stdout_or_NULL	NULL
#endif


/* Module sxmilstn_mngr.c		*/

SXVOID sxmilstn_oflw (sxindex_header *index_header_ptr, SXINT old_line_nb, SXINT old_size)
{
    SXINT new_line_nb = sxindex_line_nb (*index_header_ptr);
    SXINT new_column_nb = sxindex_column_nb (*index_header_ptr);
    SXINT new_size = sxindex_size (*index_header_ptr);

    sxuse(old_size); /* pour faire taire gcc -Wunused */
    sxndtkn.milestones.buf = (struct sxmilstn_elem**)
	sxrealloc (sxndtkn.milestones.buf, new_line_nb, sizeof (struct sxmilstn_elem*));

    while (--new_line_nb >= old_line_nb) {
	sxndtkn.milestones.buf [new_line_nb] = (struct sxmilstn_elem*)
	    sxalloc (new_column_nb, sizeof (struct sxmilstn_elem));
    }

    sxndtkn.milestones.set = sxba_resize (sxndtkn.milestones.set, new_size);
}

SXVOID sxmilstn_alloc (sxmilstn_header *header, SXINT size)
{
    SXINT i, l;

    sxindex_alloc (&header->indexes, 1, size, sxmilstn_oflw);
    i = sxindex_line_nb (header->indexes);
    header->buf = (struct sxmilstn_elem**) sxalloc (i, sizeof (struct sxmilstn_elem*));

    l = sxindex_column_nb (header->indexes);

    while (--i >= 0) {
	header->buf [i] = (struct sxmilstn_elem*) sxalloc (l, sizeof (struct sxmilstn_elem));
    }

    l = sxindex_size (header->indexes);
    header->set = sxba_calloc (l);
}

SXVOID sxmilstn_free (sxmilstn_header *header)
{
    SXINT i = sxindex_line_nb (header->indexes);

    while (--i >= 0) {
	sxfree (header->buf [i]);
    }

    sxfree (header->set);
    sxfree (header->buf);
    sxindex_free (header->indexes);
}

/* Fin module sxmilstn_mngr.c		*/




struct sxmilstn_elem	*milestone_new (struct sxmilstn_elem *prev_milstn_ptr, SXINT my_milestone, SXINT kind)
{
    SXINT		new_milstn = sxmilstn_seek (sxndtkn.milestones);
    struct sxmilstn_elem
	*new_milstn_ptr = &sxmilstn_access (sxndtkn.milestones, new_milstn);

    new_milstn_ptr->my_index = new_milstn;
    new_milstn_ptr->my_milestone = my_milestone;
    new_milstn_ptr->next_for_scanner = 0;
    new_milstn_ptr->prev_milstn = prev_milstn_ptr;
    new_milstn_ptr->kind = kind;

    if (prev_milstn_ptr != NULL) {
	if ((new_milstn_ptr->next_milstn = prev_milstn_ptr->next_milstn) != NULL)
	    new_milstn_ptr->next_milstn->prev_milstn = new_milstn_ptr;

	prev_milstn_ptr->next_milstn = new_milstn_ptr;
    }
    else
	new_milstn_ptr->next_milstn = NULL;

    return new_milstn_ptr;
}


SXVOID	sxndtkn_put (SXBA set)
{
    SXINT			tde, old_milestone, x = -1;
    SXBOOLEAN		is_kept, is_first = SXTRUE, is_check_complete = SXFALSE;
    struct sxndlv	*p;

/* Il est possible que plusieurs reduces donnent exactement le meme token :
   Ex : (@1 | @2) t
   Si milestone initial et terminal_token identiques, il serait bien de ne
   le stocker qu'une fois...
   A FAIRE. */

    while ((x = sxba_scan_reset (set, x)) >= 0) {
	p = sxndsvar.ndlv + x;
	old_milestone = p->milestone;
	is_kept = SXTRUE;

	if (p->terminal_token.lahead > 0) {
	    sxput_token (p->terminal_token);

	    if (is_first) {
		sxndtkn.milstn_head = milestone_new (sxndtkn.milstn_head,
						     ++sxndtkn.last_milestone_no,
						     (SXINT)MS_BEGIN); 
		p->milestone = sxndtkn.milstn_head->my_index;

		if (p->terminal_token.lahead == sxndsvar.SXS_tables.S_termax) {
		    /* On suppose que tous les scanners ont reconnus eof */
		    is_first = SXFALSE; /* Il doit disparaitre */
		}
	    }

	    XxYxZ_set (&sxndtkn.dag,
		       old_milestone,
		       sxplocals.Mtok_no,
		       sxndtkn.milstn_head->my_index,
		       &tde);
	}
	else if (p->terminal_token.comment != NULL)
	    /* On suppose les commentaires (non nuls) tous differents */
	    ndlv_clear (p, SXTRUE);

	if (p->terminal_token.lahead > 0 || p->terminal_token.comment == NULL) {
	    if (is_first) {
		is_first = SXFALSE;

		if (sxndsvar.index_header.set)
		  /* Le 09/05/3003 */
		  ndlv_clear (p, SXFALSE);
	    }
	    else {
		is_kept = SXFALSE;

		if (sxndsvar.index_header.set)
		  /* Le 09/05/3003 */
		  sxindex_release (sxndsvar.index_header, x);
	    }
	}

	/* Si un scanner a disparu (sxindex_release ()) ou son milestone initial
	   a change, et s'il n'existe plus ds sxndsvar.active_ndlv_set, alors
	   il devient MS_COMPLETE */
	/* la verification complete ne peut se faire que lorsque tous les
	   active_ndlv_set sont connus, donc hors de la boucle principale */
	if (!is_kept || old_milestone != p->milestone) {
	    is_check_complete = SXTRUE;
	    SXBA_1_bit (sxndtkn.milestones.set, old_milestone);
	}
    }

    if (is_check_complete) {
	x = -1;
	
	while ((x = sxba_scan (sxndsvar.active_ndlv_set, x)) >= 0) {
	    old_milestone = sxndsvar.ndlv [x].milestone;
	    SXBA_bit_is_set_reset (sxndtkn.milestones.set, old_milestone);
	}
	
	if (sxndsvar.top_incl != NULL) {
	    /* Il y a des includes ds le coup. */
	    /* Il peut y avoir des scanners actifs en attente. */
	    struct ndincl_elem	*top = sxndsvar.top_incl;
	    SXBA		active_ndlv_set;

	    do {
		active_ndlv_set = top->ndlsets [0];
		x = -1;
	
		while ((x = sxba_scan (active_ndlv_set, x)) >= 0) {
		    old_milestone = sxndsvar.ndlv [x].milestone;
		    SXBA_bit_is_set_reset (sxndtkn.milestones.set, old_milestone);
		}
	    } while ((top = top->prev) != NULL);
	}

	x = -1;
	
	while ((x = sxba_scan_reset (sxndtkn.milestones.set, x)) >= 0) {
	    (&sxmilstn_access (sxndtkn.milestones, x))->kind |= MS_COMPLETE ;
	}
    }
}


SXVOID	sxndtkn_mngr (SXINT sxndtkn_mngr_what, SXINT size)
{
    SXINT 		tde, prev_mlstn;
    static SXINT tokens_dag_foreach [] = {1, 0, 1, 0, 0, 0};

    switch (sxndtkn_mngr_what) {
    case SXOPEN:
	XxYxZ_alloc (&sxndtkn.dag, "tokens_dag", size * (SXTOKEN_AND + 1),
		     1, tokens_dag_foreach, NULL, stdout_or_NULL); 
	sxmilstn_alloc (&sxndtkn.milestones, (SXINT)50);
	break;

    case SXINIT:
	/* Le token Mtok_no (eof) a ete fabrique' et parse_stack.for_scanner.next_hd contient
	   le numero (normalement 1), du parser initial qui contient l'action scan sur
	   le eof initial. */
	sxmilstn_clear (sxndtkn.milestones);

	/* Le milestone initial est initialise' comme suit */
	parse_stack.milstn_tail = 
	    sxndtkn.milstn_head = milestone_new ((struct sxmilstn_elem *)NULL,
						 sxndtkn.last_milestone_no = 0,
						 (SXINT)MS_WAIT | (SXINT)MS_COMPLETE | (SXINT)MS_BEGIN);
	sxndtkn.milstn_head->next_for_scanner = *parse_stack.for_scanner.next_hd;

	prev_mlstn = sxndtkn.milstn_head->my_index;

	sxndtkn.milstn_current = sxndtkn.milstn_head =
	    milestone_new (sxndtkn.milstn_head, ++sxndtkn.last_milestone_no, (SXINT)MS_BEGIN);

	XxYxZ_clear (&sxndtkn.dag);
	XxYxZ_set (&sxndtkn.dag,
		   prev_mlstn /* 0 */,
		   sxplocals.Mtok_no /* 0 */,
		   sxndtkn.milstn_head->my_index /* 1 */,
		   &tde);

	sxndsvar.ndlv [sxndsvar.current_ndlv->my_index].milestone = sxndtkn.milstn_head->my_index;

	break;

    case SXFINAL:
	break;

    case SXCLOSE:
	sxmilstn_free (&sxndtkn.milestones);
	XxYxZ_free (&sxndtkn.dag);
	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxndtoken_mngr","unknown switch case #1");
#endif
      break;
    }
}

