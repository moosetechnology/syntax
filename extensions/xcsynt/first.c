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

#if 0
static char	ME [] = "first";
#endif

#include "P_tables.h"
#include "csynt_optim.h"

char WHAT_XCSYNTFIRST[] = "@(#)SYNTAX - $Id: first.c 3652 2023-12-24 09:43:15Z garavel $" WHAT_DEBUG;

#include "SS.h"
#include "RLR.h"


static SXINT	*fit; /* FirstInitDisplay */
/* Structure contenant les ensembles d'items a la earley */
 /* Earley's Item Set List */
static SXINT		*EISL_disp, *EISL_stack, EISL_disp_top, EISL_stack_top;
/* ensemble d'items a la earley */

static SXBA		EI_set, nt_set;

/* Un item a la earley est un doublet EI = (i, d) ou
      - i est un item LR(0)
      - d est l'identifiant un ensemble d'items a la earley (EarleyState) */
/* Les EI sont implantes par des couples (i, d) ou
      - (i, d) est element de IxES_hd */

static XxY_header	IxES_hd;

static XxY_header	ESxT_hd;
static SXINT		*ES_T_to_ES;

static SXINT		LA_top;

static XH_header	*stack_hd;

static bool		(*parse_completed)(SXINT, SXINT, bool);

static void	oflw_IxES (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    EI_set = sxba_resize (EI_set, new_size + 1);
}

static void	oflw_ESxT (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    ES_T_to_ES = (SXINT*) sxrealloc (ES_T_to_ES, new_size + 1, sizeof (SXINT));
}

static void first_alloc (void)
{
    fit = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (SXINT));
    EISL_stack = (SXINT*) sxalloc (xac2 + 1, sizeof (SXINT));
    EISL_stack [0] = xac2;
    EISL_stack_top = 1;
    EISL_disp = (SXINT*) sxalloc (xac2 + 1, sizeof (SXINT));
    EISL_disp [0] = xac2;
    EISL_disp [1] = EISL_stack_top;
    EISL_disp_top = 1;
    XxY_alloc (&IxES_hd, "IxES", xac2, 2, 0, 0, oflw_IxES, statistics_file);
    EI_set = sxba_calloc (XxY_size (IxES_hd) + 1);
    XxY_alloc (&ESxT_hd, "ESxT", xac2, 2, 0, 0, oflw_ESxT, statistics_file);
    ES_T_to_ES = (SXINT*) sxalloc (XxY_size (ESxT_hd) + 1, sizeof (SXINT));
    nt_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1);
}


void first_free (void)
{
    if (EI_set != NULL) {
	sxfree (EISL_stack);
	sxfree (EISL_disp);
	XxY_free (&IxES_hd);
	sxfree (EI_set), EI_set = NULL;
	sxfree (fit);
	XxY_free (&ESxT_hd);
	sxfree (ES_T_to_ES);
	sxfree (nt_set);
    }
}


	
static void	ei_put (SXINT item, SXINT d)
{
    SXINT id;

    XxY_set (&IxES_hd, item, d, &id);

    if (SXBA_bit_is_reset_set (EI_set, id)) {
	EISL_stack [EISL_stack_top] = id;

	if (++EISL_stack_top > EISL_stack [0])
	    EISL_stack = (SXINT*) sxrealloc (EISL_stack, (EISL_stack [0] *= 2) + 1, sizeof (SXINT));
    }
}



static SXINT put_nt (SXINT nt)
{
    SXINT y, lim;

    if (SXBA_bit_is_reset_set (nt_set, nt))
	for (lim = bnf_ag.WS_NTMAX [nt + 1].npg, y = bnf_ag.WS_NTMAX [nt].npg; y < lim; y++) {
	    ei_put (bnf_ag.WS_NBPRO [bnf_ag.WS_NBPRO [y].numpg].prolon, EISL_disp_top);
	}

    return EISL_disp_top;
}



static SXINT closure (SXINT nt_init)
{
    SXINT	id, x, item, y;
    SXINT			nt, xnt, lim, d, etat_init;
    bool		is_final = false;
    
    etat_init = fit [nt_init];

    for (x = EISL_disp [EISL_disp_top]; x < EISL_stack_top; x++) {
	id = EISL_stack [x];
	item = XxY_X (IxES_hd, id);
	
	if ((xnt = bnf_ag.WS_INDPRO [item].lispro) > 0) {
	    /* item : lhs -> alpha . xnt beta */
	    if (SXBA_bit_is_set (bnf_ag.BVIDE, xnt))
		ei_put (item + 1, XxY_Y (IxES_hd, id));
	    
	    put_nt (XNT_TO_NT_CODE (xnt));
	}
	else if (xnt == 0) {
	    /* item : xnt -> gamma . */
	    xnt = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc;
	    d = XxY_Y (IxES_hd, id);
	    nt = XNT_TO_NT_CODE (xnt);
	    
	    if (nt == nt_init && (d == etat_init || d == -etat_init))
		/* l'etat initial peut aussi etre final! */
		is_final = true;
	    
	    /* On regarde les (A -> alpha . xnt beta, d1) de Id (s'il y en a) */
	    /* Attention, d peut etre egal a EISL_disp_top.
	       Dans ce cas la recherche d'item de la forme ". xnt" peut ne pas
	       etre complete au moment ou elle est faite (un ". xnt" pouvant
	       apparaitre plus tard)
	       Pour traiter ce cas, lors de l'examen d'un item
	       [A -> gamma .B delta, d], si B =>+ epsilon, on ajoute
	       [A -> gamma B .delta, d] (avant de fermer B) */
	    
	    if (d != EISL_disp_top) {
		lim = EISL_disp [d + 1];
		
		for (y = EISL_disp [d]; y < lim; y++) {
		    id = EISL_stack [y];
		    item = XxY_X (IxES_hd, id);
		    
		    if (xnt == bnf_ag.WS_INDPRO [item].lispro) {
			ei_put (item + 1, XxY_Y (IxES_hd, id));
		    }
		}
	    }
	}
    }
    
    if ((x = EISL_disp_top) >= EISL_disp [0])
	EISL_disp = (SXINT*) sxrealloc (EISL_disp, (EISL_disp [0] *= 2) + 1, sizeof (SXINT));

    EISL_disp [++EISL_disp_top] = EISL_stack_top;
    return is_final ? -x : x;
}



static SXINT trans (SXINT nt_init, SXINT earley_state, SXINT t)
{
    SXINT	x, item, id;
    SXINT			indice, lim;
    bool		is_trans;

    if (!XxY_set (&ESxT_hd, earley_state, t, &indice)) {
	is_trans = false;
	lim = EISL_disp [earley_state + 1];
	x = EISL_disp [earley_state];
	sxba_empty (EI_set);
	
	while (x < lim) {
	    id = EISL_stack [x];
	    item = XxY_X (IxES_hd, id);
	    
	    if (bnf_ag.WS_INDPRO [item].lispro == t) {
		ei_put (item + 1, XxY_Y (IxES_hd, id));
		is_trans = true;
	    }
	    
	    x++;
	}

	ES_T_to_ES [indice] = is_trans ? (sxba_empty (nt_set), closure (nt_init)) : 0;
    }

    return ES_T_to_ES [indice];
}




static SXINT first_init (SXINT xnt)
{
    SXINT	nt;

    nt = XNT_TO_NT_CODE (xnt);

    if (fit [nt] == 0) {
	sxba_empty (EI_set);
	sxba_empty (nt_set);
	fit [nt] = put_nt (nt);
	/* fil [nt] peut etre utilise dans closure pour savoir si l'etat est final. */
	fit [nt] = closure (nt);
	/* Etat final code en negatif. */
    }

    return fit [nt];
}

static bool prefixe (SXINT item, SXINT t_ref)
{
    SXINT	ES, xtnt, next_t, t_ref_init;

    if ((xtnt = bnf_ag.WS_INDPRO [item].lispro) == 0 || t_ref == LA_top)
	return parse_completed (item, t_ref, false);

    next_t = XH_list_elem (*stack_hd, t_ref);
    
    if (xtnt < 0) {
	if (xtnt == next_t && prefixe (item + 1, t_ref + 1))
	    return true;
    }
    else {
	t_ref_init = t_ref;
	ES = first_init (xtnt);
	
	do {
	    if (ES < 0) {
		/* Etat final, une phrase de xtnt vient d'etre reconnue, on progresse
		   dans item. */
		if (prefixe (item + 1, t_ref))
		    return true;
		
		if (next_t == 0)
		    return false;
		
		ES = -ES;
	    }
	    
	    if (next_t == 0)
		/* Reconnaissance d'un prefixe strict de xtnt */
		return parse_completed (item, t_ref_init, true);
	    
	    ES = trans (xtnt, ES, next_t);
	    next_t = ++t_ref == LA_top ? 0 : XH_list_elem (*stack_hd, t_ref);
	} while (ES != 0);
    }
    
    
    return false;
}

bool earley_parse (SXINT LA_init, SXINT LA_final, XH_header *hd, SXINT item, bool (*F)(SXINT, SXINT, bool))
{
    stack_hd = hd;
    LA_top = LA_final;
    parse_completed = F;

    if (EI_set == NULL)
	first_alloc ();

    return prefixe (item, LA_init);
}
