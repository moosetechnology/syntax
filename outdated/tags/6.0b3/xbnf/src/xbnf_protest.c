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


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030512 13:07 (phd):	Adaptation � SGI (64 bits)		*/
/************************************************************************/
/* Lun 25 Nov 1996 10:20(pb):	Adaptation a LINUX			*/
/************************************************************************/
/* Jeu 14 Nov 1996 17:28 (pb):	Dans "xbnf", les "post-actions" sont 	*/
/*				significatives pour tester si les	*/
/*				productions forment un ensemble		*/
/************************************************************************/
/* 25-09-96 13:39 (pb):		F_empty est remodifie			*/
/************************************************************************/
/* Jul 4 1996 15:26 (pb):	Modif du test les productions forment	*/
/*				t-elles un ensemble ?			*/
/************************************************************************/
/* 05-08-94 15:40 (pb):		bvide (xnt) <==> bvide (nt)		*/
/************************************************************************/
/* 04-08-94 14:28 (pb):		Les predicats ne sont plus des objets	*/
/*				de 1ere classe				*/
/************************************************************************/
/* 05-07-94 18:58 (pb):		Les predicats sont des terminaux 	*/
/*				"derivant" ds le vide.			*/
/************************************************************************/
/* 05-05-92 15:05 (pb):		Les tests d'ambiguites sont deplaces	*/
/*				vers csynt pour traiter + facilement	*/
/*				le cas du non-determinisme.		*/
/************************************************************************/
/* 01-10-90 11:25 (pb):		Ajout d'un test (cs d'ambiguite)	*/
/************************************************************************/
/* 10-04-90 14:30 (phd):	Suppression des "typedef" de bl		*/
/************************************************************************/
/* 12-02-88 17:25 (pb):		Adaptation aux XNT + qq modifs		*/
/************************************************************************/
/* 10-12-86 16:05 (phd):	Correction de error_same_prod		*/
/************************************************************************/
/* 09-12-86 17:08 (phd):	Optimisations				*/
/************************************************************************/
/* 09-12-86 17:00 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
#include "sxba.h"
#include "B_tables.h"
#include "xbnf_vars.h"
#include "varstr.h"
char WHAT_XBNFPROTEST[] = "@(#)SYNTAX - $Id: xbnf_protest.c 1122 2008-02-28 15:55:03Z rlacroix $" WHAT_DEBUG;


/*--------------------------------------*/
/*					*/
/*	P R O P E R T Y    T E S T	*/
/*					*/
/*--------------------------------------*/


/*------------*/
/* procedures */
/*------------*/


static VARSTR	vstr;

static BOOLEAN	prod_equal;

static BOOLEAN
is_prod_equal (SXINT prod1, SXINT prod2)
{
    SXINT l1, l2, xl1, xl2, i, s1, s2;

    l1 = WN [prod1 + 1].prolon - (xl1 = WN [prod1].prolon);
    l2 = WN [prod2 + 1].prolon - (xl2 = WN [prod2].prolon);

    if (l1 != l2)
	return FALSE;

    for (i = l1-2; i >= 0; i--) {
	s1 = WI [xl1 + i].lispro;
	s2 = WI [xl2 + i].lispro;

	if (s1 != s2)
	    return FALSE;
    }

    return (bnf_ag.RULE_TO_REDUCE != NULL) ? (bnf_ag.RULE_TO_REDUCE [prod1] == bnf_ag.RULE_TO_REDUCE [prod2]) : TRUE;
}

static BOOLEAN
is_prod_less_equal (SXINT prod1, SXINT prod2)
{
    SXINT l1, l2, xl1, xl2, i, s1, s2;
    SXINT	post_action1, post_action2;

    l1 = WN [prod1 + 1].prolon - (xl1 = WN [prod1].prolon);
    l2 = WN [prod2 + 1].prolon - (xl2 = WN [prod2].prolon);

    if (l1 != l2)
	return l1 < l2;

    for (i = l1-2; i >= 0; i--) {
	s1 = WI [xl1 + i].lispro;
	s2 = WI [xl2 + i].lispro;

	if (s1 != s2)
	    return s1 < s2;
    }
    
    if (bnf_ag.RULE_TO_REDUCE != NULL /* Le 17/03/2003 */) {
      post_action1 = bnf_ag.RULE_TO_REDUCE [prod1];
      post_action2 = bnf_ag.RULE_TO_REDUCE [prod2];

      if (post_action1 != post_action2)
	return post_action1 < post_action2;
    }

    return prod_equal = TRUE; /* egalite */
}



static VOID	error_same_prod (SXINT *sorted, SXINT n)
{
    /* sorted [1:n] contient des numeros de productions identiques et consecutifs */
    SXINT		prod1, prod2, x;
    char	string [12];
    BOOLEAN	eq;
		
    is_proper = FALSE;
    prod2 = sorted [x = 1];
		
    while (x < n) {
	prod1 = prod2;
	prod2 = sorted [++x];

	if (is_prod_equal (prod1, prod2)) {
	    varstr_catenate (vstr, "\t\t\t");
	    sprintf (string, "%ld", (long)prod1);
	    varstr_catenate (vstr, string);

	    do
	    {
		prod1 = prod2;
		eq = (++x <= n && is_prod_equal (prod1, prod2 = sorted [x]));
		sprintf (string, eq ? ", %ld" : " and %ld\n", (long)prod1);
		varstr_catenate (vstr, string);
	    } while (eq);

	    sxtmsg (sxsrcmngr.source_coord.file_name,
		    "%sThe following productions:\n\
%s\
\t\t\tare identical.", sxtab_ptr->err_titles [2], varstr_tostr (vstr));

	    varstr_raz (vstr);
	}
    }
}


#if 0
static VOID
error_same_prod (SXINT x1, SXINT x2)
{
    sxtmsg (sxsrcmngr.source_coord.file_name,
	    "%sProductions %d and %d are identical.",
	    sxtab_ptr->err_titles [2],
	    x1,
	    x2);
}
#endif


static SXINT	F_empty (SXINT x_rule)
{
    return LGPROD (x_rule);
}


#if 0
/* Le 25/9/96, on revient a l'ancienne definition */
static SXINT	F_empty (SXINT x_rule)
{
    SXINT size = 0, x_lispro, lim = WN [x_rule + 1].prolon - 1, item;

    for (x_lispro = WN [x_rule].prolon; x_lispro < lim; x_lispro++) {
	item = WI [x_lispro].lispro;

	if (item > 0 || XT_TO_PRDCT (item) == -1)
	    size++;
    }

    return size;
}
#endif

static SXINT	get_nt_set_string (SXINT *nt_set, SXINT *null_rhs, SXINT *rhs_size);
static SXINT	get_nt_set_t_or_empty_string (SXINT *nt_set, 
					      SXINT *rhs_size, 
					      SXINT *null_rhs, 
					      SXINT (*F)(SXINT))
{
    /* This function computes the set of all non terminal symbols */
    /* which can generate an empty string or a terminal_string    */
    /* depending on		  				  */
    /* rhs_size(x_prod) = length of the RHS of x_prod        	  */
    SXINT	pred_null_rhs = 0, x_rule;

    for (x_rule = 1; x_rule <= W.xnbpro; x_rule++) {
	if ((rhs_size [x_rule] = F (x_rule)) == 0) {
	    null_rhs [pred_null_rhs] = x_rule;
	    pred_null_rhs = x_rule;
	}
    }

    null_rhs [pred_null_rhs] = W.xnbpro + 1;
    return get_nt_set_string (nt_set, null_rhs, rhs_size);
}



static SXINT F_t (SXINT x_rule)
{
    SXINT size = 0, x_lispro, lim = WN [x_rule + 1].prolon - 1;

    for (x_lispro = WN [x_rule].prolon; x_lispro < lim; x_lispro++)
	if (WI [x_lispro].lispro > 0)
	    size++;

    return size;
}


static SXINT	get_nt_set_string (SXINT *nt_set, SXINT *null_rhs, SXINT *rhs_size)
/* procedure auxiliaire pour les deux precedentes		 */
{
    SXINT	nb_nt_set = 0;
    SXINT	pred_nt_set = 0;
    SXINT	x_rule = null_rhs [0];
    SXINT	i;

    nt_set [0] = W.xntmax + 1;

    while (x_rule <= W.xnbpro) {
	SXINT	nt = WN [x_rule].reduc;

	x_rule = null_rhs [x_rule];

	if (nt_set [nt] == 0) {
	    nt_set [nt] = nt_set [pred_nt_set];
	    nt_set [pred_nt_set] = nt;
	    pred_nt_set = nt;
	    nb_nt_set++;

	    for (i = WX [nt].npd; i < WX [nt + 1].npd; i++) {
		SXINT	nx_rule = WI [numpd [i]].prolis;

		if ((nx_rule > 0) && (0 == --rhs_size [nx_rule])) {
		    null_rhs [nx_rule] = x_rule;
		    x_rule = nx_rule;
		}
	    }
	}
    }

    nt_set [pred_nt_set] = W.xntmax + 1;
    return nb_nt_set;
}


SXVOID
property_test (void)
{
    is_proper = TRUE, proper [1] = NULL, proper [2] = NULL, proper [3] = NULL;

    {
	/* IS  THE  PRODUCTION  LIST  A  SET ? */
	SXINT		lim, n, nt, x1;
	SXINT		*sorted;

	sorted = (SXINT*) sxalloc (W.xnbpro+1, sizeof (SXINT));
	vstr = varstr_alloc (32);

	for (nt = 1; nt <= W.ntmax; nt++) {
	    if ((lim = WX [nt + 1].npg) - (x1 = WX [nt].npg) > 1) {
		n = 0;

		do {
		    sorted [++n] = WN [x1].numpg;
		} while (++x1 < lim);

		prod_equal = FALSE;
		sort_by_tree (sorted, 1, n, is_prod_less_equal);

		/* Dans la version "xbnf", les "post-actions" sont significatives.
		   Elles sont stockees ds bnf_ag.RULE_TO_REDUCE. */
		if (prod_equal)
		    error_same_prod (sorted, n);
	    }
	}

	sxfree (sorted);
	varstr_free (vstr), vstr = NULL;
    }


#if 0
    {
	/* IS  THE  PRODUCTION  LIST  A  SET ? */
	SXINT	x2, x1, nt;

	for (nt = 1; nt <= W.ntmax; nt++) {
	    SXINT		lim1;

	    for (lim1 = WX [nt + 1].npg - 1, x1 = WX [nt].npg; x1 < lim1; x1++) {
		SXINT	xp1, xl1, l;

		xp1 = WN [x1].numpg;
		xl1 = WN [xp1].prolon;
		l = WN [xp1 + 1].prolon - xl1;

		for (x2 = x1 + 1; x2 <= lim1; x2++) {
		    SXINT		xp2, xl2;

		    xp2 = WN [x2].numpg;
		    xl2 = WN [xp2].prolon;

		    if (l == WN [xp2 + 1].prolon - xl2) {
			BOOLEAN		is_same_prod = TRUE;
			SXINT	i;

			for (i = l - 2; i >= 0 && (is_same_prod = WI [xl1 + i].lispro == WI [xl2 + i].lispro); i--)
			    ;

			if (is_same_prod) {
			    error_same_prod (xp1, xp2);
			    is_proper = FALSE;
			}
		    }
		}
	    }
	}
    }
#endif

    {
	/* NON  PRODUCTIVE  NON  TERMINAL  SYMBOLS  TEST */
	SXINT	un_used;
	SXINT	*use, *null_rhs, *rhs_size;

	use = (SXINT*) /* 0:W.ntmax */ sxcalloc (W.xntmax + 1, sizeof (SXINT));
	null_rhs = (SXINT*) /* 0:W.xnbpro */ sxcalloc (W.xnbpro + 1, sizeof (SXINT));
	rhs_size = (SXINT*) /* 1:W.xnbpro */ sxalloc (W.xnbpro + 1, sizeof (SXINT));

	if ((un_used = W.ntmax - get_nt_set_t_or_empty_string (use, rhs_size, null_rhs, F_t)) > 0) {
	    SXINT	i, j;

	    is_proper = FALSE;
	    proper [1] = (SXINT*) /* 0:un_used */ sxalloc (un_used + 1, sizeof (SXINT));
	    proper [1] [j = 0] = un_used;

	    for (i = 1; i <= W.ntmax; i++)
		if (use [i] == 0)
		    proper [1] [++j] = i;
	}

	sxfree (use);
	sxfree (null_rhs);
	sxfree (rhs_size);
    }

    {
	/* INACCESSIBLE  NON  TERMINAL  SYMBOLS  TEST */
	SXINT	*accessible;
	SXINT	i, un_accessible, next_accessible;

	accessible = (SXINT*) /* 0:W.ntmax */ sxcalloc (W.ntmax + 1, sizeof (SXINT));

/* the axiom is accessible */

	accessible [0] = 1;
	next_accessible = 1;
	accessible [1] = W.ntmax + 1;
	un_accessible = W.ntmax - 1;

	while (next_accessible <= W.ntmax) {
	    SXINT		accessed, lim;

	    next_accessible = accessible [accessed = next_accessible];

	    for (lim = WX [accessed + 1].npg, i = WX [accessed].npg; i < lim; i++) {
		SXINT	x_lispro;
		SXINT	x_rule;
		SXINT	lim_x_lispro;

		x_rule = WN [i].numpg;
		lim_x_lispro = WN [x_rule + 1].prolon - 2;

		for (x_lispro = WN [x_rule].prolon; x_lispro <= lim_x_lispro; x_lispro++) {
		    SXINT		code;

		    if ((code = WI [x_lispro].lispro) > 0 && accessible [code = XNT_TO_NT (code)] == 0) {
			un_accessible--;
			accessible [code] = next_accessible;
			next_accessible = code;
		    }
		}
	    }
	}

	if (un_accessible > 0) {
	    SXINT	property_test_i, j;

	    is_proper = FALSE;
	    proper [2] = (SXINT*) /* 0:un_accessible.ntmax */ sxalloc (un_accessible + 1, sizeof (SXINT));
	    proper [2] [j = 0] = un_accessible;

	    for (property_test_i = 1; property_test_i <= W.ntmax; property_test_i++)
		if (accessible [property_test_i] == 0)
		    proper [2] [++j] = property_test_i;
	}

	sxfree (accessible);
    }

    {
	/* bvide */
	/* bvide[nt]=TRUE iff the non terminal symbol nt can generate the empty string */
	SXINT	nt, xnt;
	SXINT	*nt_set_empty_string, *null_rhs;
	SXINT		*rhs_size;

	nt_set_empty_string = (SXINT*) /* 0:W.xntmax */ sxcalloc (W.xntmax + 1, sizeof (SXINT));
	null_rhs = (SXINT*) /* 0:W.xnbpro */ sxcalloc (W.xnbpro + 1, sizeof (SXINT));
	rhs_size = (SXINT*) /* 1:W.xnbpro */ sxalloc (W.xnbpro + 1, sizeof (SXINT));
	get_nt_set_t_or_empty_string (nt_set_empty_string, rhs_size, null_rhs, F_empty);

	if ((nt = nt_set_empty_string [0]) <= W.xntmax) {
	    /* On parcourt les NT derivant dans le vide. */
	    do {
		SXBA_1_bit (bvide, nt);
	    } while ((nt = nt_set_empty_string [nt]) <= W.xntmax);

	    for (xnt = W.xntmax; xnt > W.ntmax; xnt--) {
		nt = XNT_TO_NT (xnt);

		if (SXBA_bit_is_set (bvide, nt))
		    SXBA_1_bit (bvide, xnt);
	    }
	}

	sxfree (nt_set_empty_string);
	sxfree (null_rhs);
	sxfree (rhs_size);
    }

    if (!is_proper)
	fprintf (sxstderr, "\tGrammar not proper, see diagnoses in listing.\n");
}
