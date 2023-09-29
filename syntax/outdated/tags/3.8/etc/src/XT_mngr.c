/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1989 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 02-10-92 13:20 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/


static char	what [] = "@(#)XT_mngr.c	- SYNTAX [unix] - 2 Octobre 1992";
static char	ME [] = "XT_mngr";


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
	

#include	"XxY.h"
#include	"X.h"
#include	"XT.h"



void	XT_alloc (header, name, init_elem_nb, average_list_nb_per_bucket, average_list_size, oflw, stat_file)
    XT_header	*header;
    char	*name;
    int		init_elem_nb, average_list_nb_per_bucket, average_list_size;
    int		(*oflw) ();
    FILE	*stat_file;
{
    /* Cette fonction "cree" un adresseur extensible de listes d'entiers
       qui partagent leurs prefixes */

    XxY_alloc (&(header->tree),
	       name,
	       init_elem_nb * average_list_size,
	       average_list_nb_per_bucket,
	       0,
	       0,
	       NULL,
	       stat_file);

    X_alloc (&(header->list),
	       name,
	       init_elem_nb,
	       average_list_nb_per_bucket,
	       oflw,
	       stat_file);

    header->unreachable_set_size = 0;
}

    
void	XT_free (header)
    XT_header	*header;
{
    if (header->unreachable_set_size != 0)
	sxfree (header->unreachable_set), header->unreachable_set = NULL;

    X_free (&(header->list));
    XxY_free (&(header->tree));
}



BOOLEAN	XT_set (header, list, result)
    XT_header	*header;
    int		list, *result;
{
    int i, x;

    if (X_top (header->list) >= X_size (header->list) &&
	(header->list.free_buckets & X_root_7F) == 0 &&
	header->list.has_free_buckets) {
	/* Ca va (peut_etre) deborder et on va utiliser des listes "erased"
	   on en profite, avant quelles ne disparaissent pour "recuperer"
	   la place utilisee pour stocker les prefixes. */
	i = XxY_top (header->tree);

	if (header->unreachable_set_size == 0)
	    header->unreachable_set = sxba_calloc ((header->unreachable_set_size = i) + 1);
	else if (i > header->unreachable_set_size)
	    header->unreachable_set = sxba_resize (header->unreachable_set,
						   (header->unreachable_set_size = i) + 1);
    
	sxba_fill (header->unreachable_set);
	
	for (i = X_top (header->list) - 1; i > 0; i--) {
	    if (X_is_erased (header->list, i))
		for (x = X_X (header->list, i);
		     x > 0 && SXBA_bit_is_set_reset (header->unreachable_set, x);
		     x = XxY_X (header->tree, x));
	}
	
	i = 0;
	
	while ((i = sxba_scan (header->unreachable_set, i)) > 0)
	    XxY_erase (header->tree, i);
    }

    return X_set (&(header->list), list, result);
}


int	XT_list_lgth (header, list)
    XT_header	*header;
    int		list;
{
    int lgth = 0;

    XT_foreach (*header, list)
	lgth++;

    return lgth;
}


