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

struct sxlist_to_attr {
    SXINT next_list, scrmbl, list_lgth, first_elem;
};

struct sxlist_bucket {
    SXINT next, elem;
};

typedef struct {
    X_header		hd;
    SXINT			*hd_to_list;

    sxindex_header	list_hd;
    struct sxlist_to_attr
	                *list_to_attr;

    sxindex_header	elem_hd;
    struct sxlist_bucket
	                *buckets;

    SXINT		(*list_oflw) (SXINT, SXINT),
                        (*elem_oflw) (SXINT, SXINT);

    FILE		*stat_file;
} sxlist_header;

#ifndef SX_GLOBAL_VAR_LIST_HEADER
#define SX_GLOBAL_VAR_LIST_HEADER extern
#endif

SX_GLOBAL_VAR_LIST_HEADER sxlist_header		*_sxlist_header; /* Pour oflw */

extern void			sxlist_alloc (sxlist_header *h,
					      char *name,
					      SXINT init_elem_nb,
					      SXINT average_list_nb_per_bucket,
					      SXINT average_list_size,
					      SXINT (*user_list_oflw) (SXINT, SXINT), 
					      SXINT (*user_elem_oflw) (SXINT, SXINT),
					      FILE *stat_file);
extern void			sxlist_free (sxlist_header*);
extern BOOLEAN			sxlist_set (sxlist_header *h,
					    SXINT scrmbl,
					    SXINT *elem_stack,
					    SXINT *result);
    /* Positionne l'identifiant unique result_ptr de la liste contenue ds SS_stack dont
       le scramble est scrmbl. Retourne TRUE ssi la liste est nouvelle. */
extern void			sxlist_erase (sxlist_header *h, SXINT l);
extern BOOLEAN			sxlist_write (sxlist_header *header, filedesc_t file_descr);
extern BOOLEAN			sxlist_read (sxlist_header *header,
					     filedesc_t file_descr,
					     char *name,
					     SXINT (*list_oflw) (SXINT, SXINT),
					     SXINT (*elem_oflw) (SXINT, SXINT),
					     FILE *stat_file);
extern void			sxlist_array_to_c (sxlist_header *header, FILE *file, char *name);
extern void			sxlist_header_to_c (sxlist_header *header, FILE *file, char *name);
extern void                     sxlist_to_c (
					     sxlist_header *header, 
					     FILE *file, 
					     char *name, 
					     BOOLEAN is_static );
extern void			sxlist_reuse (sxlist_header *header,
					      char *name,
					      SXINT (*user_list_oflw) (SXINT, SXINT),
					      SXINT (*user_elem_oflw) (SXINT, SXINT),
					      FILE *stat_file);
extern void			 sxlist_stat (FILE *stat_file, sxlist_header *header);


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

#endif
