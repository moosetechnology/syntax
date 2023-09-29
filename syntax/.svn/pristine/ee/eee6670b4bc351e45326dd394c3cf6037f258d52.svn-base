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
/* Permet de manipuler des ensembles de SXUINT et de leur assigner
   un identifiant (SXUINT) NON unique.
   Fonctionnalites semblables a XH.
*/
   
/*
   Un ensemble est identifie'e par un SXUINT obtenu par sxset_new_set_id
   Un element elem est ajoute' (s'il n'existe pas deja) par sxset_add_member
   Un ensemble de cardinal n occupe (n+2) * sizeof (SXUINT) octets

   Comparaison (sur 10000 listes aleatoires de taille max 200 prenant leurs
                elements sur [0..9999]):
                XH	XT	sxlist	sxsubset	sxSL	sxset	XxY
   Temps	1.36	15.16	2.76	2.25		1	1.53	25.84
   Place	1	4.12	3.02	3.10		1	0.99	5.15

   XxY designe l'implantation ou chaque element e de l'ensemble i est range' sous
   forme du couple (i, e)

   L'augmentation en temps par rapport au "packages list" est du au fait que chaque
   element ajoute' est teste' pour savoir s'il se trouve deja ds le sous-ensemble.

   Avantages :
      - Re'cupere effectivement la place occupe'e par les ensembles qui ont e'te'
        "released", sans changer les identifiants des ensembles valides.
      - "Clear" est fait en temps constant.

   Inconve'nients :
      - Ne supporte pas la construction simultane'e de plusieurs ensembles.

*/

#ifndef sxset_h
#define sxset_h

#include	<stdio.h>
#include        "sxalloc.h"


typedef struct sxset_header sxset_header;

struct sxset_header {
    char		*name;

    struct sxset_sets {
	SXUINT	card,
	                bot;
    }			*sets;

    SXUINT	*sparse,
                        *elems,
                        elems_size,
                        elems_top,
                        sets_size,
                        sets_top,
                        sparse_size,
                        erased_set_nb,
                        erased_elem_nb,
                        free_list,
                        current_set_id;

    void		(*sets_oflw) (sxset_header*, SXUINT, SXUINT),
    			(*elems_oflw) (sxset_header*);

    FILE		*stat_file;
} /* sxset_header */ ;

extern void		sxset_alloc ( sxset_header *header,
				      char *name,
				      SXUINT universe_size,
				      SXUINT sets_nb,
				      SXUINT average_elems_nb_per_set,
				      void (*sets_oflw) (sxset_header*, SXUINT, SXUINT),
				      void (*elems_oflw) (sxset_header*),
				      FILE *stat_file);

extern void		sxset_pack_sets (sxset_header *header);

extern void		sxset_pack_elems (sxset_header *header);

extern SXUINT	sxset_new_set_id (sxset_header *header);

extern SXBOOLEAN		sxset_member (sxset_header *header, SXUINT set_id, SXUINT elem);

extern void		sxset_free (sxset_header *header);

extern void		sxset_stat (sxset_header *header, FILE *stat_file);

/* header_ptr->current_set_id U= set_id */
extern void		sxset_copy (sxset_header *header, SXUINT set_id);

/* header_ptr->current_set_id == set_id */
extern SXBOOLEAN		sxset_compare (sxset_header *header, SXUINT set_id);

/* (h).current_set_id U= s */
#define sxset_or(h,s)	sxset_copy(h,s)

/* header_ptr->current_set_id &= set_id */
extern void		sxset_and (sxset_header *header, SXUINT set_id);

/* header_ptr->current_set_id -= set_id */
extern void		sxset_minus (sxset_header *header, SXUINT set_id);

/* new_set_id = ~set_id */
extern SXUINT	sxset_not (sxset_header *header, SXUINT set_id);

#define sxset_nil		(~((SXUINT) 0))
#define sxset_7F		(sxset_nil>>1)
#define sxset_80		(~sxset_7F)

#define sxset_current_set_id(h)	((h).current_set_id)

#define sxset_clear(h)		(h).current_set_id=(h).free_list=sxset_nil,	\
                                (h).erased_set_nb=(h).erased_elem_nb=		\
                                (h).elems_top=(h).sets_top=(SXUINT)0

#define sxset_card(h,s)		((h).sets[s].card)

#define sxset_delete_set(h,s)	((h).sets [s].bot|=sxset_80,(h).erased_set_nb++,\
				 (h).erased_elem_nb+=(h).sets [s].card)
#define sxset_is_set_deleted(h,s)						\
                                ((h).sets [s].bot&sxset_80)


#define sxset_sets_size(h)	((h).sets_size)
#define sxset_sets_top(h)	((h).sets_top)

#define sxset_elems_size(h)	((h).elems_size)
#define sxset_elems_top(h)	((h).elems_top)

/* set m with each member of s */
/* x is a working index, exit with n == 0 */
#define sxset_foreach(h,s,x,n,m)						\
				for (n=sxset_card(h,s),x=(h).sets[s].bot;	\
				     m=(h).elems[x], n > 0;		        \
				     n--,x++)


/* Could only be used in the active set */

extern void		sxset_add_member (sxset_header *header, SXUINT elem);

extern void		sxset_local_pack (sxset_header *header);

#define sxset_delete_member(h,m)						\
                                ((h).elems[(h).sparse[m]]|=sxset_80,(h).erased_elem_nb++)
#define sxset_is_member_deleted(h,m)						\
                                ((h).elems[(h).sparse[m]]&sxset_80)

#define sxset_pop(h)		(sxset_card(h,sxset_current_set_id(h))--,	\
                                (h).elems[--sxset_elems_top(h)])

#define sxset_raz(h)		((h).elems_top=(h).sets[(h).current_set_id].bot,\
				 sxset_card(h,(h).current_set_id)=0)

#endif
