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
char WHAT_TABLESOUT[] = "@(#)SYNTAX - $Id: out.c 1120 2008-02-28 15:53:59Z rlacroix $" WHAT_DEBUG;



SXVOID
out_struct (char *nom, char *elt_m1)
{
    printf ("static struct %s %s[]={%s\n", nom, nom, elt_m1);
}


SXVOID
out_end_struct (void)
{
    puts ("};");
}


SXVOID
out_short (char *nom)
{
    printf ("static /*short*/ SXINT %s[]={\n", nom);
}


SXVOID
out_int (char *nom, char *elt_m1)
{
    printf ("static SXINT %s[]={%s\n", nom, elt_m1);
}


SXVOID
out_bit (char *nom, SXBA chb)
{
    SXINT	rang, lg;

    out_int (nom, "");
    printf ("%ld", (long)(lg = sxba_cardinal (chb)));

    if (lg != 0) {
	for (rang = sxba_scan (chb, 0); rang != -1; rang = sxba_scan (chb, rang)) {
	    printf (",%ld", (long)rang);
	}
    }

    out_end_struct ();
}


SXVOID
out_bitc (char *nom, SXBA chb)
{
    SXINT	rang, lg;

    out_int (nom, "");
    printf ("%ld", (long)(lg = sxba_cardinal (chb)));

    if (lg != 0) {
	for (rang = sxba_scan (chb, 0); rang != -1; rang = sxba_scan (chb, rang)) {
	    printf (",%ld", (long)rang - 1);
	}
    }

    out_end_struct ();
}



static SXVOID	out_tab (char *typ, char *nom, SXINT tab[], SXINT deb, SXINT fin, char *elt_m1)
{
    /* static typ nom []={...}; */
    SXINT	j, i;
    SXINT	d, nbt;

    printf ("static %s %s[]={%s\n", typ, nom, elt_m1);
    nbt = (fin - deb + 1) / slice_length;
    d = deb;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%ld,", (long)tab [j]);
	}

	putchar (NEWLINE);
    }

    for (j = d; j <= fin; j++) {
	printf ("%ld,", (long)tab [j]);
    }

    out_end_struct ();
}


SXVOID
out_tab_int (char *nom, SXINT tab[], SXINT deb, SXINT fin, char *elt_m1)
{
    out_tab ("SXINT", nom, tab, deb, fin, elt_m1);
}


SXVOID
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

SXVOID
out_ext_int_newstyle (char *nom)
{
  if ( ( strlen ( nom ) >= 6 ) && !strncmp ( nom, "SEMACT", 6 ) ) {
    /* special case 'SEMACT' */
    printf ("#ifndef SCANACT_AND_SEMACT_ARE_IDENTICAL\n");
    printf ("extern SXINT %s;\n",nom);
    printf ("#endif");
  } else {
    printf ("extern SXINT %s;\n", nom);
  }
}

SXVOID
out_ext_BOOLEAN (char *nom)
{
    printf ("extern SXBOOLEAN %s();\n", nom);
}

SXVOID
out_ext_BOOLEAN_newstyle (char *nom)
{
    printf ("extern SXBOOLEAN %s;\n", nom);
}


SXVOID
out_char (char *nom, char *elt_m1)
{
    printf ("static char %s[]={%s\n", nom, elt_m1);
}
