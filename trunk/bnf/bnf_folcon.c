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

/* Follow Construction */

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "B_tables.h"
#include "bnf_vars.h"

char WHAT_BNFFOLCON[] = "@(#)SYNTAX - $Id: bnf_folcon.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#if 0
#define INFINI 0x7FFFFFFF

static SXINT	*s_levels, *d_levels;
static SXBA	*nt_follow, *X_follow;
static SXINT	xtmax, tmax, ntmax;



static SXINT	nt_debutant (nt, set, level)
    SXINT	nt;
    SXBA	set;
    SXINT		level;
{
    SXBA	line;
    SXINT	l, i, x, item;
    SXINT	*cur_level;
    SXINT		min_level, lim;
    bool	is_computed;

    cur_level = d_levels + nt;

    if ((i = *cur_level) > 0) {
	/* En cours de calcul */
	return i;
    }

    line = bnf_ag.FIRST [nt];

    if (i < 0) {
	sxba_or (set, line);
	return -1 /* c'est calcule */ ;
    }

    *cur_level = level /* on lance le calcul de debutant (nt) au niveau level */ ;
    is_computed = true;
    min_level = INFINI;
    lim = WX [nt + 1].npg;

    for (l = WX [nt].npg; l < lim; l++) {
	x = WN [WN [l].numpg].prolon;

	while ((item = WI [x++].lispro) > 0) {
	    /* occurrence d'un non terminal */
	    i = nt_debutant (item = XNT_TO_NT (item), line, level + 1);

	    if (i > 0) {
		is_computed = false;

		if (i < min_level)
		    min_level = i;
	    }

	    if (!SXBA_bit_is_set (bvide, item))
		break;
	}

	if (item < 0)
	    /* occurrence d'un terminal */
	    SXBA_1_bit (line, -item);
    }

    sxba_or (set, line);

    if (is_computed || min_level == level) {
	return *cur_level = -1 /* calcule */ ;
    }

    *cur_level = 0 /* non calcule */ ;
    return min_level;
}



static SXINT	nt_suivant (nt, set, level)
    SXINT	nt;
    SXBA	set;
    SXINT		level;
{
    SXBA	line;
    SXINT	l, i;
    SXINT	*cur_level;
    SXINT		min_level, lim;
    bool	is_computed;

    cur_level = s_levels + nt;

    if ((i = *cur_level) > 0) {
	/* En cours de calcul */
	return i;
    }

    line = nt_follow [nt];

    if (i < 0) {
	sxba_or (set, line);
	return -1 /* c'est calcule */ ;
    }

    *cur_level = level /* on lance le calcul de follow (nt) au niveau level (>0) */ ;
    is_computed = true;
    min_level = INFINI;
    lim = WX [nt + 1].npd;

    for (l = WX [nt].npd; l < lim; l++) {
	i = suivant (numpd [l], line, level + 1);

	if (i > 0) {
	    is_computed = false;

	    if (i < min_level)
		min_level = i;
	}
    }

    sxba_or (set, line);

    if (is_computed || min_level == level) {
	return *cur_level = -1 /* calcule */ ;
    }

    *cur_level = 0 /* non calcule */ ;
    return min_level;
}



static SXINT	suivant (x, set, level)
    SXINT	x;
    SXBA	set;
    SXINT		level;
{
    SXINT	item;

    while ((item = WI [++x].lispro) > 0) {
	/* occurrence d'un non terminal */
	nt_debutant (item = XNT_TO_NT (item), set, 1);

	if (!SXBA_bit_is_set (bvide, item))
	    return -1;
    }

    if (item == 0) {
	/* occurrence en fin de production */
	if ((item = WN [WI [x].prolis].reduc) != 0) {
	    return nt_suivant (XNT_TO_NT (item), set, level + 1);
	}
    }
    else
	/* occurrence d'un terminal */
	SXBA_1_bit (set, -item);

    return -1;
}
#endif


static SXBA*
bmm (SXBA *P, SXBA *Q, SXBA *R, SXINT l)
{
    /* Multiplication de matrices booleennes */
    /* P U= Q x R */
    /* P [l, n], Q [l, m], R [m, n] */

    SXBA	P_line, Q_line;
    SXINT		i, j;

    for (i = 0; i < l; i++)
    {
	P_line = P [i];
	Q_line = Q [i];

	j = -1;

	while ((j = sxba_scan (Q_line, j)) >= 0)
	    sxba_or (P_line, R [j]);
    }

    return P;
}

void
follow_construction (void)
{

    /* Calcul de FIRST, FOLLOW et NULLABLE */

    /* On calcule LC (Left-Corner) : LC = {(A, B) | A -> \alpha B \beta et \alpha =>* \epsilon}
       On calcule D (Debutant) : D = {(A, t) | A -> \alpha t \beta et \alpha =>* \epsilon}
       FIRST = LC* D */

    /* On calcule RC (Right-Corner) : RC = {(B, A) | A -> \alpha B \beta et \beta =>* \epsilon}
       On calcule NNV (NT Voisin) : NNV = {(B, C) | U -> \alpha B \beta C \gamma et \beta =>* \epsilon}
       On calcule NTV (T Voisin) : NTV = {(A, t) | U -> \alpha B \beta t \gamma et \beta =>* \epsilon}
       NT_FOLLOW = RC* NNV FIRST U RC* NTV

       On calcule L (Last) : D = {(t, A) | A -> \alpha t \beta et \beta =>* \epsilon}
       On calcule TNV (NT Voisin) : TNV = {(t, C) | U -> \alpha t \beta C \gamma et \beta =>* \epsilon}
       On calcule follow (valeur initiale) : FOLLOW = {(t1, t) | U -> \alpha t1 \beta t \gamma et \beta =>* \epsilon}
       
       follow U= L NT_FOLLOW U TNV FIRST
 */
    
    SXINT		limt, limxt, limnt, limxpro;
    SXINT		xprod, A, B, C, xt, t, t1, item, next_item, xtnt, nt;
    SXBA	line;

    SXBA	*LC, *D, *FIRST;
    SXBA	*RC, *NNV, *NTV, *NT_FOLLOW, *TNV, *L;

    limxt = W.xtmax + 1;
    limt = -W.tmax + 1;
    limnt = W.ntmax + 1;
    limxpro = W.xnbpro + 1;

    LC = sxbm_calloc (limnt, limnt);
    D = sxbm_calloc (limnt, limxt);
    bnf_ag.FIRST = sxbm_calloc (limnt, limxt);

    RC = sxbm_calloc (limnt, limnt);
    NNV = sxbm_calloc (limnt, limnt);
    NTV = sxbm_calloc (limnt, limt);
    NT_FOLLOW = sxbm_calloc (limnt, limt);

    L = sxbm_calloc (limt, limnt);
    TNV = sxbm_calloc (limt, limnt);
    follow = sxbm_calloc (limt, limt);

    for (xprod = 0; xprod < limxpro; xprod++)
    {
	A = WN [xprod].reduc;
	A = XNT_TO_NT (A);

	item = WN [xprod].prolon;

	while ((xtnt = WI [item++].lispro) != 0)
	{
	    if (xtnt < 0)
	    {
		t = -xtnt;
		SXBA_1_bit (D [A], t);
		break;
	    }

	    B = XNT_TO_NT (xtnt);
	    SXBA_1_bit (LC [A], B);

	    if (!SXBA_bit_is_set (bnf_ag.BVIDE, B))
		break;
	}

	item = WN [xprod+1].prolon-1;

	while ((xtnt = WI [--item].lispro) != 0)
	{
	    if (xtnt < 0)
	    {
		t = -XT_TO_T (xtnt);
		SXBA_1_bit (L [t], A);
		break;
	    }

	    
	    B = XNT_TO_NT (xtnt);
	    SXBA_1_bit (RC [B], A);

	    if (!SXBA_bit_is_set (bnf_ag.BVIDE, B))
		break;
	}

	item = WN [xprod].prolon;

	while ((xtnt = WI [item++].lispro) != 0)
	{
	    next_item = item;

	    if (xtnt > 0)
	    {
		B = XNT_TO_NT (xtnt);

		while ((xtnt = WI [next_item++].lispro) != 0)
		{
		    if (xtnt > 0)
		    {
			C = XNT_TO_NT (xtnt);
			SXBA_1_bit (NNV [B], C);

			if (!SXBA_bit_is_set (bnf_ag.BVIDE, C))
			    break;
		    }
		    else
		    {
			t = -XT_TO_T (xtnt);
			SXBA_1_bit (NTV [B], t);
			break;
		    }
		}
	    }
	    else
	    {
		t1 = -XT_TO_T (xtnt);

		while ((xtnt = WI [next_item++].lispro) != 0)
		{
		    if (xtnt > 0)
		    {
			C = XNT_TO_NT (xtnt);
			SXBA_1_bit (TNV [t1], C);

			if (!SXBA_bit_is_set (bnf_ag.BVIDE, C))
			    break;
		    }
		    else
		    {
			t = -XT_TO_T (xtnt);
			SXBA_1_bit (follow [t1], t);
			break;
		    }
		}
	    }
	}
    }

    fermer (LC, limnt);

    for (nt = 1; nt < limnt; nt++)
	SXBA_1_bit (LC [nt], nt);

    bmm (bnf_ag.FIRST, LC, D, limnt /*, limnt, limxt */);

    if (limt == limxt)
	FIRST = bnf_ag.FIRST;
    else {
	/* FIRST (sur les terminaux non etendus) est utilise dans le calcul de FOLLOW
	   on passe de bnf_ag.FIRST a FIRST */
	FIRST = sxbm_calloc (limnt, limxt);

	for (nt = 1; nt < limnt; nt++) {
	    line = FIRST [nt];

	    sxba_copy (line, bnf_ag.FIRST [nt]);

	    xt = limt-1;

	    while ((xt = sxba_scan_reset (line, xt)) > 0) {
		t = -XT_TO_T (-xt);
		SXBA_1_bit (line, t);
	    }

	    line [0] = (SXBA_ELT) limt; /* On change la longueur!! */
	}
    }

    fermer (RC, limnt);

    for (nt = 1; nt < limnt; nt++)
	SXBA_1_bit (RC [nt], nt);

    bmm (NTV, NNV, FIRST, limnt/* , limnt, limxt */); /* NTV U= NNV LC* D */
    bmm (NT_FOLLOW, RC, NTV, limnt/* , limnt, limxt */); /* RC* NTV */
    bmm (follow, L, NT_FOLLOW, limt/* , limnt, limt */);
    bmm (follow, TNV, FIRST, limt/* , limnt, limt */); 

    SXBA_1_bit (follow [-W.tmax], -W.tmax) /* EOF follows EOF */ ;

    if (limt != limxt)
	sxbm_free (FIRST);

    sxbm_free (LC);
    sxbm_free (D);
    sxbm_free (RC);
    sxbm_free (NNV);
    sxbm_free (NTV);
    sxbm_free (NT_FOLLOW);
    sxbm_free (TNV);
    sxbm_free (L);

#if 0
    SXINT	i, head, tail;


    tmax = -W.tmax + 1;
    xtmax = W.xtmax + 1;
    ntmax = W.ntmax + 1;
    X_follow = sxbm_calloc (xtmax, xtmax);
    s_levels = (SXINT*) sxcalloc (ntmax, sizeof (SXINT));
    d_levels = (SXINT*) sxcalloc (ntmax, sizeof (SXINT));
    nt_follow = sxbm_calloc (ntmax, xtmax);
    bnf_ag.FIRST = sxbm_calloc (ntmax, xtmax);

    for (i = 1; i < xtmax; i++) {
	SXINT	lim = tpd [-i - 1];
	SXBA	foli = X_follow [i];

	for (head = tpd [-i]; head < lim; head++) {
	    suivant (tnumpd [head], foli, 0);
	}
    }

    SXBA_1_bit (X_follow [-W.tmax], -W.tmax) /* EOF follows EOF */ ;

    if (xtmax == tmax)
	follow = X_follow;
    else {
	/* FOLLOW est utilise par lecl, on restreint son espace a tmax. */

	for (i = tmax; i < xtmax;i++) {
	    sxba_or (X_follow [-XT_TO_T (-i)], X_follow [i]);
	}

	for (i = 1; i < tmax; i++) {
	    SXBA	foli = X_follow [i];
	    head = tmax - 1;

	    while ((head = sxba_scan (foli, head)) > 0) {
		item = -XT_TO_T (-head);
		SXBA_1_bit (foli, item);
	    }
	}

	follow = sxbm_resize (X_follow, xtmax, tmax, tmax);
    }

    /* On termine le calcul de FIRST qui peut ne pas etre complet. */

    for (i = 1; i < ntmax; i++) {
	if (d_levels [i] == 0)
	    nt_debutant (i, bnf_ag.FIRST [i], 0);
    }

    sxbm_free (nt_follow);
    sxfree (d_levels);
    sxfree (s_levels);
#endif


    /* Calcul de NULLABLE */
    
    nullable = sxba_calloc (bnf_ag.WS_TBL_SIZE.indpro + 1);
    
    for (xprod = 0; xprod < limxpro; xprod++) {
	item = bnf_ag.WS_NBPRO [xprod + 1].prolon - 1;
	SXBA_1_bit (nullable, item);
	
	while ((xtnt = bnf_ag.WS_INDPRO [--item].lispro) > 0 && SXBA_bit_is_set (bnf_ag.BVIDE, xtnt))
	    SXBA_1_bit (nullable, item);
    }
}
