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
#include "sem_by.h"
#include "tables.h"

char WHAT_TABLESOUT[] = "@(#)SYNTAX - $Id: out.c 4024 2024-06-07 07:55:42Z garavel $" WHAT_DEBUG;


void
out_struct (char *nom, char *elt_m1)
{
    printf ("static struct %s %s[]={%s\n", nom, nom, elt_m1);
}


void
out_end_struct (void)
{
    puts ("};");
}


void
out_short (char *nom)
{
    printf ("static /*short*/ SXINT %s[]={\n", nom);
}


void
out_int (char *nom, char *elt_m1)
{
    printf ("static SXINT %s[]={%s\n", nom, elt_m1);
}


void
out_bit (char *nom, SXBA chb)
{
    SXINT	rang, lg;

    out_int (nom, "");
    printf ("%ld", (SXINT) (lg = sxba_cardinal (chb)));

    if (lg != 0) {
	for (rang = sxba_scan (chb, 0); rang != -1; rang = sxba_scan (chb, rang)) {
	    printf (",%ld", (SXINT) rang);
	}
    }

    out_end_struct ();
}


void
out_bitc (char *nom, SXBA chb)
{
    SXINT	rang, lg;

    out_int (nom, "");
    printf ("%ld", (SXINT) (lg = sxba_cardinal (chb)));

    if (lg != 0) {
	for (rang = sxba_scan (chb, 0); rang != -1; rang = sxba_scan (chb, rang)) {
	    printf (",%ld", (SXINT) rang - 1);
	}
    }

    out_end_struct ();
}



static void	out_tab (char *typ, char *nom, SXINT tab[], SXINT deb, SXINT fin, char *elt_m1)
{
    /* static typ nom []={...}; */
    SXINT	j, i;
    SXINT	d, nbt;

    printf ("static %s %s[]={%s\n", typ, nom, elt_m1);
    nbt = (fin - deb + 1) / slice_length;
    d = deb;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%ld,", (SXINT) tab [j]);
	}

	putchar (SXNEWLINE);
    }

    for (j = d; j <= fin; j++) {
	printf ("%ld,", (SXINT) tab [j]);
    }

    out_end_struct ();
}


void
out_tab_int (char *nom, SXINT tab[], SXINT deb, SXINT fin, char *elt_m1)
{
    out_tab ("SXINT", nom, tab, deb, fin, elt_m1);
}

void
out_extern_int (char *nom)
{
    /* assert nom != "SEMACT" */
    printf ("extern SXINT %s;\n", nom);
}

void
out_extern_bool (char *nom)
{
    printf ("extern bool %s;\n", nom);
}


void
out_char (char *nom, char *elt_m1)
{
    printf ("static char %s[]={%s\n", nom, elt_m1);
}
