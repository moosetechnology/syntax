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

char WHAT_TABLESOUT[] = "@(#)SYNTAX - $Id: out.c 3650 2023-12-23 07:32:10Z garavel $" WHAT_DEBUG;


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
out_ext_int (char *nom)
{
  if ( !strcmp ( nom, "SEMACT" ) ) {

    /* CAS SPECIAL 'SEMACT' ->

       Il existe des cas ou cette fonction doit generer des declarations externes
       qui sont des macros de preprocesseur. Par exemple dans les constructeurs
       yax et ysx, elle genere les declarations externes des macros SEMACT et
       SCANACT. Nous essayons de gerer le cas ou ces 2 macros ont la meme valeur.
       Dans ce cas, il ne faut generer qu'une seule declaration externe. C'est le
       but de la macro SCANACT_AND_SEMACT_ARE_IDENTICAL : si le developpeur veut
       donner la meme valeur aux 2 macros SCANACT et SEMACT, alors il doit
       positionner la macro SCANACT_AND_SEMACT_ARE_IDENTICAL a 1 afin qu'on ne
       genere qu'une seule declaration externe.

       Ce mecanisme n'est pas dangereux, voici toutes les combinaisons possibles :

       - si SCANACT et SEMACT sont differents :

        - et si SCANACT_AND_SEMACT_ARE_IDENTICAL n'est pas positionne
          alors on generera 2 declarations externes (1 pour SCANACT et 1 pour SEMACT)
        - et si SCANACT_AND_SEMACT_ARE_IDENTICAL est positionne
          alors on ne generera qu'1 declaration externe, et on verra apparaitre
          un warning "missing declaration"

       - si SCANACT et SEMACT sont identiques :

        - et si SCANACT_AND_SEMACT_ARE_IDENTICAL n'est pas positionne
          alors on generera 2 declarations externes, et on verra apparaitre un
          warning "redundant redeclaration"
        - et si SCANACT_AND_SEMACT_ARE_IDENTICAL est positionne
          alors on ne generera qu'1 declaration externe

    */

    printf ("#ifndef SCANACT_AND_SEMACT_ARE_IDENTICAL\n");
    printf ("extern SXINT SEMACT();\n");
    printf ("#endif");
  } else {
    printf ("extern SXINT %s();\n", nom);
  }
}

void
out_ext_int_newstyle (char *nom)
{
  if ( ( strlen ( nom ) >= 6 ) && !strncmp ( nom, "SEMACT", 6 ) ) {
    /* special case 'SEMACT' */
    printf ("#ifndef SCANACT_AND_SEMACT_ARE_IDENTICAL\n");
    printf ("extern SXINT %s;\n",nom);
    printf ("#endif\n");
  } else {
    printf ("extern SXINT %s;\n", nom);
  }
}

void
out_ext_BOOLEAN (char *nom)
{
    printf ("extern bool %s();\n", nom);
}

void
out_ext_BOOLEAN_newstyle (char *nom)
{
    printf ("extern bool %s;\n", nom);
}


void
out_char (char *nom, char *elt_m1)
{
    printf ("static char %s[]={%s\n", nom, elt_m1);
}
