#ifndef XT_h
#define XT_h

/* Fonctionnalites similaires au package "XH".

   Avantages :
      - Partage les prefixes des listes
      - Recupere effectivement la place occupee par les listes qui ont ete
        "erased", sans changer les identifiants des listes actives.
      - Supporte la construction simultanee de plusieurs listes.

   Inconvenients :
      - Plus lent que "XH" (environ 4 fois)
      - Plus gourmand en place pour un taux de partage global inferieur ou
        egal a trois.
      - Le calcul de la longueur d'une liste est long dans l'implantation
        actuelle.
*/

#include	<stdio.h>
#include	"X.h"
#include	"XxY.h"
#include	"sxba.h"

typedef struct {
    XxY_header		tree;
    X_header		list;
    SXBA		unreachable_set;
    SXINT			unreachable_set_size;
    FILE		*stat_file;
} XT_header;

extern void			XT_alloc (XT_header *header, 
					  char *name, 
					  SXINT init_elem_nb, 
					  SXINT average_list_nb_per_bucket, 
					  SXINT average_list_size, 
					  void (*oflw) (SXINT, SXINT), 
					  FILE *stat_file);
extern void			XT_free (XT_header *header);
extern BOOLEAN			XT_set (XT_header *header, SXINT list, SXINT *result);
extern SXINT			XT_list_lgth (XT_header *header, SXINT list);
extern void			XT_stat (FILE *stat_file, XT_header *header);


#define XT_erase(h,l)		X_erase((h).list,l)
#define XT_clear(h)		(XxY_clear (&((h).tree)), X_clear (&((h).list)))
#define XT_push(h,l,x,r)	XxY_set (&((h).tree), l, x, r)
    /* On ajoute l'element x a la sous-liste l.
       La liste vide est representee par un entier negatif ou nul. */
#define XT_is_set(h,l)		(X_is_set (&((h).list, l)))

#define XT_size(h)		X_size ((h).list)
#define XT_top(h)		X_top ((h).list)
#define XT_list_size(h)		XxY_size ((h).tree)
#define XT_list_top(h)		XxY_top ((h).tree)
#define XT_lock(h)		(XxY_lock (&((h).tree)), X_lock (&((h).list)))
#define XT_unlock(h)		(XxY_unlock ((h).tree), X_unlock ((h).list) )
#define XT_list_elem(h,x)	XxY_Y ((h).tree, x)
#define XT_foreach(h,x)		for (x=X_X((h).list,x);x>0;x=XxY_X((h).tree,x))

#endif
