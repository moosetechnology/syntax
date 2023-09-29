/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/
/*   G E T _ A _ P A R T I T I O N   */








#include "sxunix.h"
char WHAT_LECLGETAPART[] = "@(#)SYNTAX - $Id: get_a_part.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;


struct equiv_class_item {
    SXINT	repr, eq_lnk;
};




SXVOID get_a_partition (SXBOOLEAN (*is_compatible)(SXINT, SXINT, SXINT), 
		      SXINT (*get_next_state)(SXINT, SXINT), 
		      SXBA is_used, 
		      SXINT state_no, 
		      SXINT tmax, 
		      struct equiv_class_item *equiv_class, 
		      SXINT *partition_no)
{
    /* Cette procedure partitionne un ensemble d'etats (automate) en classes
   d'equivalences. L'automate est represente sous forme matricielle de
   dimension state_no * tmax ou state_no est le nombre de lignes (etats),
   tmax est le nombre de colonnes (symboles)

   On construit un vecteur equiv_class de taille 0:state_no (attention, 0 est
   utilise comme memoire de manoeuvre) ou chaque element est un doublet (repr,
   eq_lnk). Pour un etat s, repr(s) est le representant de la classe 
   d'equivalence a laquelle appartient s. Soit s' = eq_lnk(s)
   si s'>0 alors s' est l'etat suivant (s'>s) de la meme classe d'equivalence
   si s=0 alors s est le dernier etat de cette classe d'equivalence.
   Partition_no est le nombre de classes d'equivalences calculees.

   La connaissance d'une action de l'automate est obtenue par l'intermediaire
   de deux fonctions:

   get_next_state (etat, symbole) retourne l'etat suivant (>0) si l'action
   est une transition vers un autre etat de l'automate sinon 0

   is_compatible (statei, statej, symbole) est vrai ssi les actions dans les
   etats statei et statej sur symbole sont compatibles (0-indistinguables)

   Deux etats sont 0_indistinguables ssi les actions correspondant a un
   symbole sont 0_indistinguables.

   Methode
   On partitionne tout d'abord l'ensemble des etats en sous_ensembles dont les
   elements sont 0_indistinguables. Puis on raffine ces partitions.
   Deux etats sont k+1_indistinguables ssi leurs actions shift correspondantes
   referencent des etats k_indistinguables. Les classes d'equivalences sont
   obtenues lorsque la partition est stable */

    

    SXINT	i, j, k;
    SXINT		*partitions, *sano /* 1:state_no */ ;
    /* donne le nb d'actions shift dans l'etat i */
    SXINT		**shift_action /* 1:state_no, 1:tmax */ ;
    SXBOOLEAN	is_a_new_partition;
    SXINT	*shifti, *shiftj;
    SXINT		l, xp1, xp2, prev, new_part;


    partitions = (SXINT*) sxalloc ((SXINT) (state_no + 1), sizeof (SXINT));
    sano = (SXINT*) sxalloc ((SXINT) (state_no + 1), sizeof (SXINT));
    shift_action = (SXINT**) sxalloc ((SXINT) (state_no + 1), sizeof (SXINT*));

    for (i = 1; i <= state_no; i++)
	shift_action [i] = (SXINT*) sxcalloc ((SXINT) (tmax + 1), sizeof (SXINT));


/* Construction de shift_action */

    i = 0;

    while ((i = sxba_scan (is_used, i)) > 0) {
	shifti = shift_action [i];
	l = 0;
	equiv_class [i].repr = i;

	for (j = 1; j <= tmax; j++) {
	    if ((k = get_next_state (i, j)) > 0) {
		shifti [++l] = k;
	    }
	}

	sano [i] = l;
    }


/* Partitionnement des etats en sous_ensembles dont les elements sont
   0_indistinguables */

    xp2 = 1;
    *partition_no = 0;

    i = 0;
    while ((i = sxba_scan (is_used, i)) > 0) {
	if (equiv_class [i].repr == i) {
	    (*partition_no)++;
	    prev = i;
	    l = sano [i];
	    j = i;

	    while ((j = sxba_scan (is_used, j)) > 0) {
		if (equiv_class [j].repr == j && sano [j] == l) {
		    for (k = 1; k <= tmax && is_compatible (i, j, k); k++)
			;

		    if (k > tmax) {
			equiv_class [j].repr = i;
			equiv_class [prev].eq_lnk = j;
			prev = j;
		    }
		}
	    }

	    if (prev != i) {
		/* classe d'equivalence multiple, on la stocke dans partitions */
		partitions [xp2++] = i;
	    }
	}
    }

    is_a_new_partition = SXTRUE;

    while (is_a_new_partition) {
	is_a_new_partition = SXFALSE;

	for (xp1 = 1; xp1 < xp2; xp1++) {
	    i = partitions [xp1];
	    equiv_class [0].eq_lnk = 0;
	    new_part = 0;
	    shifti = shift_action [i];
	    k = i;

	    for (j = equiv_class [k].eq_lnk; j != 0; j = equiv_class [k].eq_lnk) {
		/* On regarde si les etats i et j sont toujours indistinguables */
		shiftj = shift_action [j];

		for (l = 1; l <= sano [i]; l++) {
		    if (equiv_class [shifti [l]].repr != equiv_class [shiftj [l]].repr) {
			equiv_class [k].eq_lnk = equiv_class [j].eq_lnk;
			equiv_class [j].eq_lnk = 0;
			equiv_class [new_part].eq_lnk = j;
			new_part = j;
			equiv_class [j].repr = equiv_class [0].eq_lnk;
			/* nouveau representant */
			goto next;
		    }
		}

		k = j;
next:		;
	    }

	    new_part = equiv_class [0].eq_lnk;

	    if (new_part != 0) {
		/* une nouvelle partition a ete creee */
		(*partition_no)++;
		is_a_new_partition = SXTRUE;

		if (equiv_class [new_part].eq_lnk != 0) {
		    /* Cette partition a au moins deux elements, on la stocke */
		    if (equiv_class [i].eq_lnk == 0)
			/* la partition i n'a plus qu'un element, on reutilise la place
   dans partitions */
			partitions [xp1--] = new_part;
		    else
			partitions [xp2++] = new_part;
		}
	    }
	}
    }

    for (i = state_no; i > 0; i--)
	sxfree (shift_action [i]);

    sxfree (shift_action);
    sxfree (sano);
    sxfree (partitions);
}
