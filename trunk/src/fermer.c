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

/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"

char WHAT_FERMER[] = "@(#)SYNTAX - $Id: fermer.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

/* Comme sxba_or mais |lhs_bits_array| <= |rhs_bits_array| */
static SXBA
OR (SXBA lhs_bits_array, SXBA rhs_bits_array)
/*
 * "sxba_or" puts into its first argument the result of the bitwise
 * "OR" of its two arguments.  It returns its (modified) first
 * argument.
 */
{
  SXBA	lhs_bits_ptr, rhs_bits_ptr;
  SXBA_ELT  lhs_size = SXBASIZE (lhs_bits_array), rhs_size = SXBASIZE (rhs_bits_array);
  SXBA_ELT  slices_number = SXNBLONGS (lhs_size);

  lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

  sxbassert (lhs_size <= rhs_size, "or (unequal sizes)");
  sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (lhs_size - 1))) == 0, "or");
  sxbassert ((*(rhs_bits_array+SXNBLONGS (rhs_size)) & ((~((SXBA_ELT)0)) << 1 << MOD (rhs_size - 1))) == 0, "or");

  if (lhs_size < rhs_size) {
    slices_number--;
    *lhs_bits_ptr-- |= (*rhs_bits_ptr--) & ~((~((SXBA_ELT)0)) << 1 << MOD (lhs_size - 1));
  }

  while (slices_number-- > 0) {
    *lhs_bits_ptr-- |= *rhs_bits_ptr--;
  }

  return lhs_bits_array;
}

/* comme sxba_first_difference avec |bits_array_1| <= |bits_array_2| */
static SXINT
FIRST_DIFFERENCE (SXBA bits_array_1, SXBA bits_array_2)
/*
 * "sxba_first_difference" returns the index of the first bit which is
 * set in one of its arguments and reset in the other, or -1 if its
 * arguments hold the same bits.
 */
{
  SXBA_ELT    size1 = SXBASIZE (bits_array_1), size2 = SXBASIZE (bits_array_2);
  SXBA_ELT    slices_number;
  SXBA	    start_1 = bits_array_1;
  SXBA_ELT oper2;

  slices_number = SXNBLONGS (size1);

  sxbassert (size1 <= size2, "first_difference (unequal sizes)");
  sxbassert ((*(bits_array_1+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array_1) - 1))) == 0, "first_difference");
  sxbassert ((*(bits_array_2+SXNBLONGS (size2)) & ((~((SXBA_ELT)0)) << 1 << MOD (size2 - 1))) == 0, "first_difference");

  while (slices_number-- > 0) {
    oper2 = *++bits_array_2;

    if (size1 < size2 && slices_number == 0) {
      /* derniere tranche */
      oper2 &= ~((~((SXBA_ELT)0)) << 1 << MOD (size1 - 1));
    }

    if (*++bits_array_1 != oper2) {
      SXBA_ELT	xor [2];

      xor [0] = SXBITS_PER_LONG /* no need to be finer */ ;
      xor [1] = *bits_array_1 ^ oper2;
      return MUL (bits_array_1 - start_1 - 1) + sxba_scan (xor, -1);
    }
  }

  return -1;
}


void	fermer (SXBA *R, SXBA_INDEX taille)
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

/* Le 16/01/02 Bug ds le calcul de "lignes_pleines" */
{
    SXBA	*bm;
    SXBA_INDEX	*zeroes;
    struct tl {
	SXBA	        line;
	SXBA_INDEX      i;
    }	        *tl /* 0:taille-1 */ ;
    SXBA_INDEX  i, j;
    SXBA_INDEX  zero_i, zero_j;
    struct tl	*axl, *axtl;
    SXBA	ligne_i, ligne_j;
    SXBA	/* taille */ col_non_vides, lignes_pleines, lignes_non_vides, une_ligne_pleine;
    bool	is_ligne_j_pleine;

    bm = sxbm_calloc (4, taille);
    une_ligne_pleine = bm [0];
    col_non_vides = bm [1];
    lignes_non_vides = bm [2];
    lignes_pleines = bm [3];
    tl = (struct tl*) sxalloc (taille, sizeof (struct tl));
    zeroes = (SXBA_INDEX*) sxalloc (taille, sizeof (SXBA_INDEX));
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
		OR (col_non_vides, ligne_i);
		(++axtl)->line = ligne_i;
		axtl->i = i;
	    }
	}
    }


/* On regarde s'il y a des lignes pleines */

    axl = tl;
    sxba_copy (une_ligne_pleine, col_non_vides);

    while (axl <= axtl) {
	if (FIRST_DIFFERENCE (une_ligne_pleine, axl->line) == -1) {
	    SXBA_1_bit (lignes_pleines, axl->i);
	    *axl = *axtl--;
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

		if (is_ligne_j_pleine || FIRST_DIFFERENCE (une_ligne_pleine, ligne_i) == -1 /* i est pleine */ ) {
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

#include "sxstack.h"
/* Comme fermer mais + rapide (au moins sur un exemple !!) */
/* R [0..taille-1, 0..taille-1] contient la relation */
void	fermer2 (SXBA *R, SXBA_INDEX taille)
{
  SXBA_INDEX A, B, **tilde_stacks, *area_ptr;
  SXBA_INDEX *tilde_stack, *tilde_stack_top;
  SXBA_INDEX *tbp_stack, *glbl_ptr;
  SXBA    tbp_set, line_A;

  tbp_set = sxba_calloc (taille);
  tbp_stack = (SXBA_INDEX*) sxalloc (taille, sizeof (SXBA_INDEX)), tbp_stack [0] = 0;

  /* Contiendra la relation transposee de R sous-forme de piles : tilde_stacks [B] = [A_1, A_2, ..., A_p] <=> (A_i, B) \in R */
  tilde_stacks = (SXBA_INDEX**) sxcalloc (taille, sizeof (SXBA_INDEX*)); /* calloc pour tester la non initialisation */
  glbl_ptr = area_ptr = (SXBA_INDEX*) sxalloc (taille*taille, sizeof (SXBA_INDEX));

  /* transposition de la relation initiale et initialisation des "tilde_stack" */
  for (A = 0; A < taille; A++) {
    line_A = R [A];

    if ((B = sxba_scan (line_A, -1)) >= 0) {
      /* non vide */
      SXBA_1_bit (tbp_set, A);
      PUSH (tbp_stack, A);

      do {
	/* (A, B) est un element de la relation */
	/* On note les predecesseurs immediats de B */
	if ((tilde_stack = tilde_stacks [B]) == NULL) {
	  /* non initialise'e, on le fait */
	  tilde_stack = tilde_stacks [B] = glbl_ptr;
	  *glbl_ptr = 0; /* pile vide */
	  glbl_ptr += taille;
	}

	PUSH (tilde_stack, A);
      } while ((B = sxba_scan (line_A, B)) >= 0);
    }
  }

  while (!IS_EMPTY (tbp_stack)) {
    /* A a change' depuis le coup precedent ... */
    A = POP (tbp_stack);
    SXBA_0_bit (tbp_set, A);
    line_A = R [A];
    /* ... Les changements sont ds stacks [A] ... */
    /* ... on repercute ces changements sur les nt B t.q. B => A */
    
    if (tilde_stack = tilde_stacks [A], tilde_stack) {
      /* A a des predecesseurs */
      tilde_stack_top = tilde_stack + TOP (tilde_stack);

      while (++tilde_stack <= tilde_stack_top) {
	B = *tilde_stack;

	if (sxba_or_with_test (R [B], line_A)) {
	  /* R [B] a change' ... */
	  if (SXBA_bit_is_reset_set (tbp_set, B))
	    /* ... et n'est pas ds tbp_stack, on le note */
	    PUSH (tbp_stack, B);
	}
      }
    }
  }

  sxfree (tbp_set);
  sxfree (tbp_stack);
  sxfree (tilde_stacks);
  sxfree (area_ptr);
}
