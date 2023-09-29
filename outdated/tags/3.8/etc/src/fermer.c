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

static char	what [] = "@(#)fermer.c\t- SYNTAX [unix] - 31 Mai 1988";




/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/

#include "sxunix.h"
#include "sxba.h"



fermer (R, taille)
    SXBA	*R;
    int		taille;
/* Cette procedure realise la fermeture transitive d'une relation			*/
/* binaire @ sur un ensembe E (|E| = taille).						*/
/* La relation est representee par une "BitsMatrix" R de				*/
/* dimension R [(0:taille-1), (0:taille-1)]						*/
/* i @ j <=> R [i, j]									*/
/* Le resultat est calcule dans R							*/

/* Methode:										*/
/* Une ligne i vide est vide si quelque soit j different de i on a !(i @ j)		*/
/* Une ligne vide est calculee								*/
/* Les references a des lignes vides n'ajoutent aucun couple a la fermeture		*/
/* "zeroes[i]" contient a tout instant pour chaque ligne i l'index j (ou -1)		*/
/* de l'element le plus a droite de la ligne i 						*/
/* "tl" est une structure qui contient a tout moment les lignes non encore		*/
/* traitees (lignes dont le resultat peut encore evoluer)				*/
/* Une ligne est pleine si elle est egale a l'union des lignes a traiter		*/
/* Une ligne pleine est calculee							*/
/* Toute reference a une ligne pleine est pleine					*/
/* On examine les colonnes j de 0 a taille-1						*/
/* On boucle tant qu'il y a encore des lignes a traiter					*/
/* 											*/
/* Les colonnes de 0 a j-1 ont deja ete examinees					*/
/* si i @ j alors ligne_i = ligne_i U ligne_j						*/
/* si zeroes[i]==j alors la ligne i ne contient plus que des zeros au-dela de		*/
/* j et est donc calculee								*/
/* si ligne_i est pleine elle est calculee						*/
{
    SXBA	*bm;
    int		*zeroes;
    struct tl {
	SXBA	line;
	int	i;
    }	*tl /* 0:taille-1 */ ;
    register int	i, j, zero_i, zero_j;
    register struct tl	*axl, *axtl;
    register SXBA	ligne_i, ligne_j;
    SXBA	/* taille */ col_non_vides, lignes_pleines, lignes_non_vides, une_ligne_pleine;
    BOOLEAN	is_ligne_j_pleine;


    bm = sxbm_calloc (4, taille);
    une_ligne_pleine = bm [0];
    col_non_vides = bm [1];
    lignes_non_vides = bm [2];
    lignes_pleines = bm [3];
    tl = (struct tl*) sxalloc (taille, sizeof (struct tl));
    zeroes = (int*) sxalloc (taille, sizeof (int));
    axtl = tl - 1;


/* Pretraitement sur R */

/* Pour chaque ligne i non vide on memorise
   dans tl le couple (ligne_i, i)
   SXBA_1_bit (lignes_non_vides, i)
   zeroes [i], index du 1 le plus a droite */

    for (i = 0; i < taille; i++) {
	if ((j = zeroes [i] = sxba_1_rlscan ((ligne_i = R [i]), taille)) >= 0) {
	    /* il y a qqc dans la ligne i */
	    if (j != i || j != sxba_1_lrscan (ligne_i, -1)) {
		/* non uniquement reflexive */
		SXBA_1_bit (lignes_non_vides, i);
		sxba_or (col_non_vides, ligne_i);
		(++axtl)->line = ligne_i;
		axtl->i = i;
	    }
	}
    }


/* On regarde s'il y a des lignes pleines */

    axl = tl;
    sxba_or (une_ligne_pleine, col_non_vides);

    while (axl <= axtl) {
	if (sxba_first_difference (une_ligne_pleine, axl->line) == -1) {
	    *axl = *axtl--;
	    SXBA_1_bit (lignes_pleines, axl->i);
	}
	else
	    axl++;
    }


/* On ne s'interesse qu'aux colonnes referencant des lignes non vides */

    sxba_and (col_non_vides, lignes_non_vides);
    j = -1;

    while ((j = sxba_scan (col_non_vides, j)) >= 0) {
	/* Il y a qqc dans la colonne j et la ligne j est non vide */
	if ((axl = tl) > axtl)
	    /* En fait tout est deja calcule */
	    break;


/* Il reste des lignes non traitees. */

	ligne_j = R [j];
	zero_j = zeroes [j];
	is_ligne_j_pleine = SXBA_bit_is_set (lignes_pleines, j);

	while (axl <= axtl) {
	    if (SXBA_bit_is_set (ligne_i = axl->line, j) /* i @ j */  && (i = axl->i) != j /* i # j */ ) {
		sxba_or (ligne_i, ligne_j);

		if (zero_j > (zero_i = zeroes [i])) {
		    zeroes [i] = zero_i = zero_j;
		}

		if (zero_i == j /* il ne reste que des zeros */ ) {
		    /* La ligne i est donc calculee */
		    *axl = *axtl--;
		    continue;
		}

		if (is_ligne_j_pleine || sxba_first_difference (une_ligne_pleine, ligne_i) == -1 /* i est pleine */ ) {
		    SXBA_1_bit (lignes_pleines, i);
		    *axl = *axtl--;
		    continue;
		}
	    }

	    axl++;
	}
    }

    sxfree (zeroes);
    sxfree (tl);
    sxbm_free (bm);
}
