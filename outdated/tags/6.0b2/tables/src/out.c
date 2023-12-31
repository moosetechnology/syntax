/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'�quipe Langages et Traducteurs.        *
   *							  *
   ******************************************************** */





/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030522 17:06 (phd):	Ajout d'un param�tre � out_struct,	*/
/*	out_char, out_int, out_tab, out_tab_int : l'�l�ment -1 �ventuel	*/
/************************************************************************/

#include "sxunix.h"
#include "sem_by.h"

#include "tables.h"
char WHAT_TABLESOUT[] = "@(#)SYNTAX - $Id: out.c 653 2007-06-22 15:06:10Z rlacroix $" WHAT_DEBUG;



SXVOID
out_struct (nom, elt_m1)
    char	*nom, *elt_m1;
{
    printf ("static struct %s %s[]={%s\n", nom, nom, elt_m1);
}


SXVOID
out_end_struct ()
{
    puts ("};");
}


SXVOID
out_short (nom)
    char	*nom;
{
    printf ("static /*short*/ int %s[]={\n", nom);
}


SXVOID
out_int (nom, elt_m1)
    char	*nom, *elt_m1;
{
    printf ("static int %s[]={%s\n", nom, elt_m1);
}


SXVOID
out_bit (nom, chb)
    char	*nom;
    register SXBA	chb;
{
    register int	rang, lg;

    out_int (nom, "");
    printf ("%d", lg = sxba_cardinal (chb));

    if (lg != 0) {
	for (rang = sxba_scan (chb, 0); rang != -1; rang = sxba_scan (chb, rang)) {
	    printf (",%d", rang);
	}
    }

    out_end_struct ();
}


SXVOID
out_bitc (nom, chb)
    char	*nom;
    register SXBA	chb;
{
    register int	rang, lg;

    out_int (nom, "");
    printf ("%d", lg = sxba_cardinal (chb));

    if (lg != 0) {
	for (rang = sxba_scan (chb, 0); rang != -1; rang = sxba_scan (chb, rang)) {
	    printf (",%d", rang - 1);
	}
    }

    out_end_struct ();
}



static SXVOID	out_tab (typ, nom, tab, deb, fin, elt_m1)
    char	*typ;
    char	*nom, *elt_m1;
    int		tab [];
    int		deb, fin;
{
    /* static typ nom []={...}; */
    register int	j, i;
    register int	d, nbt;

    printf ("static %s %s[]={%s\n", typ, nom, elt_m1);
    nbt = (fin - deb + 1) / slice_length;
    d = deb;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%d,", tab [j]);
	}

	putchar (NEWLINE);
    }

    for (j = d; j <= fin; j++) {
	printf ("%d,", tab [j]);
    }

    out_end_struct ();
}


SXVOID
out_tab_int (nom, tab, deb, fin, elt_m1)
    char	*nom, *elt_m1;
    int		tab [];
    int		deb, fin;
{
    out_tab ("int", nom, tab, deb, fin, elt_m1);
}


SXVOID
out_ext_int (nom)
    char	*nom;
{
  if ( !strcmp ( nom, "SEMACT" ) ) {
    /* special case 'SEMACT' */
    printf ("#ifndef SCANACT_AND_SEMACT_ARE_IDENTICAL\n");
    printf ("extern int SEMACT();\n");
    printf ("#endif");
  } else {
    printf ("extern int %s();\n", nom);
  }
}


SXVOID
out_ext_BOOLEAN (nom)
    char	*nom;
{
    printf ("extern SXBOOLEAN %s();\n", nom);
}


SXVOID
out_char (nom, elt_m1)
    char	*nom, *elt_m1;
{
    printf ("static char %s[]={%s\n", nom, elt_m1);
}
