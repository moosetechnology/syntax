#ifndef sxlist_h
#define sxlist_h
/* Fonctionnalites similaires aux packages "XH" et "XT".

   Une liste est identifie'e par un entier positif ou nul.
   Une liste de longueur n occupe a peu pres (8.03 + 2.03n) cellules.

   Comparaison (sur 10000 listes aleatoires de taille max 200 prenant leurs
                elements sur [0..499]):
                XH		XT		sxlist
   Temps	1		6		3
   Place	1		4		2

   Avantages :
      - Recupere effectivement la place occupee par les listes qui ont ete
        "erased", sans changer les identifiants des listes actives.
      - Supporte la construction simultanee de plusieurs listes.

   Inconvenients :
      - Ne partage pas les prefixes des listes
      - Plus gourmand en place et temps que XH.
*/

#include	<stdio.h>
#include	"X.h"
#include	"sxindex.h"

extern void			sxlist_alloc (); /* (header_ptr,
						    name,
						    init_elem_nb,
						    average_list_nb_per_bucket,
						    average_list_size,
						    list_oflw,
						    elem_oflw,
						    stat_file) */
extern void			sxlist_free (); /* (header_ptr) */
extern BOOLEAN			sxlist_set (); /* (header_ptr,
						  scrmbl,
						  SS_stack,
						  result_ptr) */
    /* Positionne l'identifiant unique result_ptr de la liste contenue ds SS_stack dont
       le scramble est scrmbl. Retourne TRUE ssi la liste est nouvelle. */
extern void			sxlist_erase (); /* (header_ptr, list) */
extern BOOLEAN			sxlist_write (); /* (header_ptr, file_descr) */
extern BOOLEAN			sxlist_read (); /* (header_ptr,
						    file_descr,
						    name,
						    list_oflw,
						    elem_oflw,
						    stat_file) */
extern void			sxlist_array_to_c (); /* (header_ptr, file, name) */
extern void			sxlist_header_to_c (); /* (header_ptr, file, name) */
extern void			sxlist_to_c (); /* (header_ptr,
						   file,
						   name,
						   is_static) */
extern void			sxlist_reuse (); /* (header_ptr,
						    name,
						    list_oflw,
						    elem_oflw,
						    stat_file) */
extern void			 sxlist_stat (); /* (stat_file, header_ptr) */


#define sxlist_lgth(h,l)	((h).list_to_attr [l].list_lgth)
#define sxlist_clear(h)		(X_clear (&((h).hd)),		\
				 sxindex_clear ((h).list_hd),	\
				 sxindex_clear ((h).elem_hd))
#define sxlist_size(h)		sxindex_size ((h).list_hd)
    /* Nombre maximal de listes */
#define sxlist_elem_size(h)	sxindex_size ((h).elem_hd)
    /* Nombre maximal d'elements cumules. */
#define sxlist_lock(h)		X_lock (&((h).hd)))
#define sxlist_unlock(h)	X_unlock ((h).hd)

#define sxlist_first_bucket(h,l)					\
	                        ((h).list_to_attr[l].first_elem)
#define sxlist_next_bucket(h,b)	((h).buckets[b].next)

#define sxlist_foreach_list(h,l)					\
	                        sxindex_foreach((h).list_hd,l)
#define sxlist_foreach_elem(h,l,b)					\
	                        for (b=sxlist_first_bucket(h,l);	\
	                        b>0;					\
	                        b=sxlist_next_bucket(h,b))
#define sxlist_elem(h,b)	((h).buckets[b].elem)
#define sxlist_is_static(h)	sxindex_is_static ((h).list_hd)
#define sxlist_elem_is_static(h)					\
	                        sxindex_is_static ((h).elem_hd)


struct sxlist_to_attr {
    int next_list, scrmbl, list_lgth, first_elem;
};

struct sxlist_bucket {
    int next, elem;
};

typedef struct {
    X_header		hd;
    int			*hd_to_list;

    sxindex_header	list_hd;
    struct sxlist_to_attr
	                *list_to_attr;

    sxindex_header	elem_hd;
    struct sxlist_bucket
	                *buckets;

    int			(*list_oflw) (),
                        (*elem_oflw) ();

    FILE		*stat_file;
} sxlist_header;

sxlist_header		*_sxlist_header; /* Pour oflw */

#endif
