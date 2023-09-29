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
#include "sxunix.h"
#include "B_tables.h"

char WHAT_SEMC_LO[] = "@(#)SYNTAX - $Id: semc_lo.c 3599 2023-09-18 12:57:19Z garavel $" WHAT_DEBUG;

#include "semc_vars.h"
#include "varstr.h"
#include "put_edit.h"

extern SXVOID       bnf_lo (void);


extern FILE	*listing;
static char	line [132], linei [132], line_minus [132];
static VARSTR	vstr_ptr;

#define get_at_string(at)  sxstrget(attr_to_ste [at])
#define get_nt_string(nt)  sxstrget(nt_to_ste [nt])


/* --------------------------------------------------------- */

/*
|-----------------------------|
| L I S T I N G   O U T P U T |
|-----------------------------|*/

static SXBOOLEAN	less_nt (SXINT i, SXINT j)
{
    char	*k1, *k2;


/* les symboles non terminaux de nt_string sont des chaines C differentes
   les unes des autres */

    k1 = get_nt_string (i);
    k2 = get_nt_string (j);

    while (*k1 == *k2)
	k1++, k2++;

    return *k1 < *k2;
}




/* less_nt */

/* --------------------------------------------------------- */

static SXBOOLEAN	less_at (SXINT i, SXINT j)
{
    char	*k1, *k2;


/* les attributs sont des chaines C differentes les unes des autres */

    k1 = get_at_string (i);
    k2 = get_at_string (j);

    while (*k1 == *k2)
	k1++, k2++;

    return *k1 < *k2;
}




/* less_at */

/* --------------------------------------------------------- */

static SXINT	*sort (SXBOOLEAN (*compare)(SXINT, SXINT), SXINT n)
{
    SXINT	i;
    SXINT	*code /* 1:n */ ;

    code = (SXINT*) sxalloc (n + 1, sizeof (SXINT));

    for (i = n; i >= 1; i--)
	code [i] = i;

    sort_by_tree (code, 1, n, compare);
    return code;
}




/* sort */

/* --------------------------------------------------------- */

static char	*strrevcpy (char *destination, char *source, SXINT size)
/* destination = reverse (source) || blancs
   on suppose que &destination != &source */

{
    char	*d, *d1;

    *(d = destination + size) = SXNUL;
    d1 = destination + strlen (source);

    while (--d >= d1)
	*d = ' ';

    d1 = source;

    while (*d1)
	*d-- = *d1++;

    return destination;
}




/* strrevcpy */

/* --------------------------------------------------------- */

static SXVOID	print_nt_at (void)
{
    SXINT	i, j, k;
    SXINT		table_slice_size, max_slice_no, slice_no, bi, bs;
    SXINT		*sort_nt, *sort_at;
    char	vs;

    vstr_ptr = varstr_alloc (128);

/* tri des non-terminaux et des attributs */

    sort_nt = sort (less_nt, W.ntmax);
    sort_at = sort (less_at, (SXINT)max_attr);
    table_slice_size = (128 - M_nt) >> 1;
    max_slice_no = (max_attr - 1) / table_slice_size + 1;

    for (slice_no = 1; slice_no <= max_slice_no; slice_no++) {
	put_edit_apnnl ("\t\tN O N - T E R M I N A L S       A T T R I B U T E S");

	if (slice_no < max_slice_no)
	    put_edit_apnnl ((slice_no == 1) ? "\t( T O   B E   C O N T I N U E D )" : "\t( C O N T I N U E D )");
	else if (slice_no > 1)
	    put_edit_apnnl ("\t( E N D )");

	put_edit_nl (3);
	bi = table_slice_size * (slice_no - 1) + 1;

	if ((bs = table_slice_size * slice_no) > (SXINT)max_attr) {
	    bs = max_attr;
	}

	{
	    char	**ren /* bi:bs *//* M_at */ ;

	    ren = (char**) sxalloc ((SXINT) (bs - bi + 1), sizeof (char*));

	    for (i = bi; i <= bs; i++) {
		ren [i-bi] = (char*) sxalloc ((SXINT) (M_at + 1), sizeof (char));
		strrevcpy (ren [i-bi], varstr_tostr (varstr_cat_cstring (varstr_raz (vstr_ptr), get_at_string (sort_at [i])))
		     , M_at);
	    }

	    line [i = M_nt + 2 * (bs - bi + 1 + 1)] = SXNUL;
	    line_minus [i] = SXNUL;

	    while (--i >= 0) {
		line [i] = ' ';
		line_minus [i] = '-';
	    }

	    line [M_nt + 1] = '|';

	    for (i = M_at - 1; i >= 0; i--) {
		k = M_nt + 1;

		for (j = bi; j <= bs; j++) {
		    line [k += 2] = ren [j-bi] [i];
		}

		put_edit_ap (line);
	    }

	    put_edit_ap (line_minus);

	    for (i = 1; i <= W.ntmax; i++) {
		for (j = bi; j <= bs; j++) {
		    vs = attr_nt [sort_at [j]] [sort_nt [i]];
		    linei [j - bi] = (vs == ' ') ? '.' : vs;
		}
		/* LINTED : this cast from long to int is needed by printf() */
		sprintf (line, "%-*s | ", (int)M_nt, varstr_tostr (varstr_cat_cstring (varstr_raz (vstr_ptr), get_nt_string (
		     sort_nt [i]))));
		k = M_nt + 1;

		for (j = bi; j <= bs; j++) {
		    line [k += 2] = linei [j - bi];
		}

		put_edit_ap (line);
	    }

	    for (i = bs - bi; i >= 0; i--)
		sxfree (ren [i]);

	    sxfree (ren);
	}
    }

    sxfree (sort_at);
    sxfree (sort_nt);
}




/* print_nt_at */

/* --------------------------------------------------------- */

SXVOID	semc_lo (void)
{

    if (is_source && is_list) {
	bnf_lo ();

	if (!IS_ERROR) {
	    put_edit_nl (1);
	    put_edit_ap ("Some information about your attribute grammar:");
	    put_edit_ap ("----------------------------------------------");
	    put_edit_nl (1);
	    put_edit_apnnl ("# of productions");
	    put_edit_nb (30, W.nbpro);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of non-terminals");
	    put_edit_nb (30, W.ntmax);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of terminals");
	    put_edit_nb (30, -W.tmax);
	    put_edit_nl (1);
	    put_edit_apnnl ("Size of the grammar");
	    put_edit_nb (30, W.indpro - W.nbpro);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of attributes");
	    put_edit_nb (30, (SXINT)max_attr);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of attribute definitions");
	    put_edit_nb (30, nb_sem_rules);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of identities");
	    put_edit_nb (30, nb_identities);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of default identities");
	    put_edit_nb (30, nb_defaults);
	    put_edit_nl (1);

	    if (is_sem_out) {
		put_edit_nl (2);
		print_nt_at ();
	    } /* is_sem_out */
	} /* if !error... */
    } /* is_source && is_list */
}
/* semc_lo */

/* --------------------------------------------------------- */
