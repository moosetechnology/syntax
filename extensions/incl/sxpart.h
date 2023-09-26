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
/* 			CALCUL DE LA PARTITION MINIMALE				*/

/* C'est l'ensemble des mintermes non vides a l'exclusion du minterme nul. 	*/

#ifndef sxpart_h
#define	sxpart_h

#include "sxba.h"

typedef struct {
    SXBA		*subsets;
    SXBA		*trans;
    SXBA		input;
    SXBA		output;
    SXINT			*subsets_to_trans, *trans_to_subsets;
    SXINT			sx, sl, sc;
    SXINT			tx, tl, tc;
} sxpart_header;

/* Utilise' dans sxpart_seek */
extern SXINT		sxpart_line_oflw (sxpart_header *h);

/* A utiliser si le referentiel a augmente' */
extern void		sxpart_ref_oflw (sxpart_header *h, SXINT new_size);

/* Initialise une zone de travail */
extern void		sxpart_alloc (sxpart_header *h, 
				      SXINT line_nb, 
				      SXINT column_nb);

/* Libere une zone de travail */
extern void		sxpart_free (sxpart_header* header);

/* Calcule la partition: les ensembles de de'part doivent etre initialise's
   (Chaque appel a sxpart_seek(h) retourne un nouvel identifiant i qui est transforme'
    en SXBA par sxpart_to_subset(h,i)). Attention, a cause des reallocations eventuelles
   un SXBA peut-e^tre invalide' par un appel de sxpart_seek, en revanche, l'identifiant
   retourne' par sxpart_seek est toujours valide (jusqu'au prochain sxpart_clear).
   Les identifiants de ces ensembles de de'part doivent figurer dans le SXBA input
   obtenu par sxpart_to_input.
   Retourne un SXBA qui contient les identifiants des e'le'ments de la partition re'sultat.
   De plus, pour chacun de ces e'le'ments, il est possible de connai^tre (via sxpart_to_orig)
   le minterme associe' (sous-ensemble de input). */
extern SXBA		sxpart_doit (sxpart_header* h);


/* Retourne l'identifiant (SXINT) d'un nouveau sous_ensemble */
#define sxpart_seek(h)		((h).sx == (h).sl ? sxpart_line_oflw (&(h)) : (h).sx++)

/* Associe a l'identifiant i (retourne' par sxpart_seek) un SXBA.
   Attention une utilisation ulte'rieure de sxpart_seek peut invalider
   les sous_ensembles (SXBA) actifs, cependant, leurs identifiants restent
   valides. */
#define sxpart_to_subset(h,i)	((h).subsets [i])

/* Retourne le SXBA qui contiendra l'ensemble des identifiants des sous-ensembles
   dont on veut calculer la partition minimale. */
#define sxpart_to_input(h)	((h).input)

/* Si i est (l'identifiant d'un) e'le'ment d'une partition, les e'le'ments de cet
   SXBA repre'sentent le(s composants non comple'mente's du) minterme associe'
   (sous-ensemble de input "responsable" de i). */
#define sxpart_to_orig(h,i)	((h).trans [(h).subsets_to_trans [i]])

/* Nettoie la structure h */
#define sxpart_clear(h)		((h).sx = 0)

#endif
