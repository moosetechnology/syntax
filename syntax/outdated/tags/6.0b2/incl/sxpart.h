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
    int			*subsets_to_trans, *trans_to_subsets;
    int			sx, sl, sc;
    int			tx, tl, tc;
} sxpart_header;

/* Utilise' dans sxpart_seek */
extern int		sxpart_line_oflw (/* sxpart_header* h */);

/* A utiliser si le referentiel a augmente' */
extern void		sxpart_ref_oflw (/* sxpart_header* h, int new_ref_card */);

/* Initialise une zone de travail */
extern void		sxpart_alloc (/* sxpart_header* h,
					 int init_line_nb,
					 int ref_card */);

/* Libere une zone de travail */
extern void		sxpart_free (/* sxpart_header* h */);

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
extern SXBA		sxpart_doit (/* sxpart_header* h */);


/* Retourne l'identifiant (int) d'un nouveau sous_ensemble */
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
