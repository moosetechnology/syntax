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
/* 20030512 10:59 (phd):	Adaptation � SGI (64 bits)		*/
/************************************************************************/
/* 06-10-94 10:01 (pb):		Bug dans XxYxZ_stat			*/
/************************************************************************/
/* 30-03-94 10:02 (pb):		Remplacement de strncpy par memcpy	*/
/************************************************************************/
/* 15-02-94 11:58 (pb):		Ajout de XxYxZ_cpy et utilisation des	*/
/*				"is_static"				*/
/************************************************************************/
/* 08-02-94 14:21 (pb):		Ajout de XxYxZ_to_c, XxYxZ_reuse	*/
/************************************************************************/
/* 07-02-94 17:17 (pb):		Ajout de XxYxZ_write, XxYxZ_read	*/
/************************************************************************/
/* 07-10-92 14:45 (pb):		Suppression des macros min et max	*/
/************************************************************************/
/* 04-02-92 10:50 (pb):		Modification de XxYxZ_HASH_FUN		*/
/************************************************************************/
/* 21-01-92 15:50 (pb):		La taille de table de hash est tj un nb	*/
/*				premier, il faut recalculer toutes les	*/
/*				chaines de collision sur debordement et	*/
/*				en particulier pouvoir executer "cmp"	*/
/*				sur 2 elements qcq, d'ou modif de 	*/
/*				"XxYxZ_scrmbl" qui stocke les val	*/
/************************************************************************/
/* 04-07-91 15:10 (pb):		Utilisation de "X_root_mngr"		*/
/************************************************************************/
/* 13-06-91 09:30 (pb):		Les liens des "trous" ont "X_80" pour	*/
/*				ajout de XxYxZ_foreach			*/
/*				Ajout de "has_free_buckets" et appel de	*/
/*				"gc" avant le doublement.		*/
/************************************************************************/
/* 10-06-91 10:10 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/



static char	ME [] = "XxYxZ_mngr";

#include "sxcommon.h"
#include "XxYxZ.h"
#include <memory.h>
#include <stdlib.h>

char WHAT_XXYXZ_MNGR[] = "@(#)SYNTAX - $Id: XxYxZ_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

extern void sxperror (char *);

extern void sxtrap (char *caller, char *message);
extern int  sxlast_bit (int nb);

static int		X_, Y_, Z_;
static XxYxZ_header	*HEADER_;
static int		KIND_;


static void XxYxZ_cpy (old, new, old_size)
    struct XxYxZ_elem	*old, *new;
    int			old_size;
{
    struct XxYxZ_elem *old_top = old + old_size;

    do {
	*new++ = *old++;
    } while (old <= old_top);
}

static void XxYxZ_assoc_oflw (old_size, new_size)
    int		old_size, new_size;
{
    /* La variable statique KIND_ est positionnee. */
    int	*old = HEADER_->lnk_hd [KIND_];

    if (((KIND_ < 3) && HEADER_->X_hd_is_static [KIND_])
	|| ((KIND_ >= 3) && (HEADER_->XxY_hd_is_static - 3) [KIND_])) {
	X_root_int_cpy (old,
			HEADER_->lnk_hd [KIND_] = (int*) sxalloc (new_size + 1, sizeof (int)),
			old_size);
    }
    else
	HEADER_->lnk_hd [KIND_] = (int*) sxrealloc (old, new_size + 1, sizeof (int));
}



static int XxYxZ_system_oflw (old_top)
    int old_top;
{
    /* Appele depuis X_root sur debordement (ou lock) de "hash_lnk". */
    /* On realloue "display" et les "lnk" des "foreach" (s'il y en a) */
    int kind, *old_lnk;

    if (HEADER_->is_static) {
	/* Premiere allocation dynamique */
	struct XxYxZ_elem *old_display = HEADER_->display;

	XxYxZ_cpy (old_display,
		   HEADER_->display = (struct XxYxZ_elem*) sxalloc (HEADER_->size + 1,
								    sizeof (struct X_elem)),
		   old_top);
    }
    else
	HEADER_->display = (struct XxYxZ_elem*) sxrealloc (HEADER_->display,
							   HEADER_->size + 1,
							   sizeof (struct XxYxZ_elem));
    
    if (HEADER_->suppress != NULL)
	for (kind = 0; kind < 6;kind++) {
	    if ((old_lnk = HEADER_->lnk [kind]) != NULL) {
		if (HEADER_->is_static) {
		    /* Premiere allocation dynamique */
		    X_root_int_cpy (old_lnk,
				    HEADER_->lnk [kind] = (int*) sxalloc (HEADER_->size + 1,
									  sizeof (int)),
				    old_top);
		}
		else
		    HEADER_->lnk [kind] = (int*) sxrealloc (old_lnk,
							    HEADER_->size + 1,
							    sizeof (int));
	    }
	}

    return 0;
}


static int XxYxZ_cmp (x)
    int x;
{
    /* Appele depuis X_root_s_set.
       "Compare" l'element courant se trouvant a l'indice x avec l'element que l'on
       recherche. Cette fonction retourne
          -1 si <
	  0  si ==
	  1  si >
    */
    register int		z;
    register struct XxYxZ_elem	*aelem = HEADER_->display + x;

    if ((z = aelem->X) >= X_) {
	if (z == X_) {
	    if ((z = aelem->Y) >= Y_) {
		if (z == Y_) {
		    if ((z = aelem->Z) >= Z_)
			return z == Z_ ? 0 : 1;

		    return -1;
		}

		return 1;
	    }

	    return -1;
	}

	return 1;
    }

    return -1;
}


static unsigned int XxYxZ_scrmbl (x)
    int x;
{
    /* Retourne la valeur du "scramble" de l'element d'indice x. */
    /* Positionne X_, Y_ et Z_, ce qui permet d'appeler "cmp". */
    register struct XxYxZ_elem	*aelem = HEADER_->display + x;

    X_ = aelem->X, Y_ = aelem->Y, Z_ = aelem->Z;
    return XxYxZ_HASH_FUN (X_, Y_, Z_);
}

static int XxYxZ_assign (x)
    int x;
{
    /* Positionne le triplet courant a l'indice x et traite les "foreach". */
    register struct XxYxZ_elem	*aelem = HEADER_->display + x;
    register int		kind;
    register int		*lnk;
    int				head;
    BOOLEAN			is_old;
    
    aelem->X = X_;
    aelem->Y = Y_;
    aelem->Z = Z_;

    if (HEADER_->suppress != NULL) {
	/* Il y a des "foreach". */
	for (kind = 0; kind < 6; kind++) {
	    /* Traitement du cas "associatif" */
	    if ((lnk = HEADER_->lnk [kind]) != NULL) {
		/* positionnement de la variable statique KIND_ si X[xY]_hd deborde */
		switch (KIND_ = kind) {
		case XYZ_X :
		    is_old = X_set (HEADER_->X_hd [XYZ_X], X_, &head);
		    break;
		    
		case XYZ_Y :
		    is_old = X_set (HEADER_->X_hd [XYZ_Y], Y_, &head);
		    break;
		    
		case XYZ_Z :
		    is_old = X_set (HEADER_->X_hd [XYZ_Z], Z_, &head);
		    break;
		    
		case XYZ_XY :
		    is_old = XxY_set ((HEADER_->XxY_hd - 3) [XYZ_XY], X_, Y_, &head);
		    break;
		    
		case XYZ_XZ :
		    is_old = XxY_set ((HEADER_->XxY_hd - 3) [XYZ_XZ], X_, Z_, &head);
		    break;
		    
		case XYZ_YZ :
		    is_old = XxY_set ((HEADER_->XxY_hd - 3) [XYZ_YZ], Y_, Z_, &head);
		    break;
		default: /* pour faire taire gcc -Wswitch-default */
		  sxinitialise(is_old); /* pour faire taire gcc -Wuninitialized */
#if EBUG
		  sxtrap(ME,"unknown switch case #1");
#endif
		  break;
		}
		
		if (is_old)
		    /* Ce n'est pas le premier X */
		    lnk [x] = HEADER_->lnk_hd [kind][head];
		else
		    lnk [x] = 0;
		
		HEADER_->lnk_hd [kind][head] = x;
	    }
	}
    }

    return 0;
}


static int XYZ_orig (x, kind)
    register int		x, kind;
{
    register struct XxYxZ_elem	*aelem = HEADER_->display + x;
    register int		indice;

    switch (kind) {
    case XYZ_X :
	indice = X_is_set (HEADER_->X_hd [XYZ_X], aelem->X);
	break;

    case XYZ_Y :
	indice = X_is_set (HEADER_->X_hd [XYZ_Y], aelem->Y);
	break;

    case XYZ_Z :
	indice = X_is_set (HEADER_->X_hd [XYZ_Z], aelem->Z);
	break;

    case XYZ_XY :
	indice = XxY_is_set ((HEADER_->XxY_hd - 3) [XYZ_XY], aelem->X, aelem->Y);
	break;

    case XYZ_XZ :
	indice = XxY_is_set ((HEADER_->XxY_hd - 3) [XYZ_XZ], aelem->X, aelem->Z);
	break;

    case XYZ_YZ :
	indice = XxY_is_set ((HEADER_->XxY_hd - 3) [XYZ_YZ], aelem->Y, aelem->Z);
	break;
    default: /* pour faire taire gcc -Wswitch-default */
      sxinitialise(indice); /* pour faire taire gcc -Wuninitialized */
#if EBUG
      sxtrap(ME,"unknown switch case #2");
#endif
      break;
    }

    if (indice == 0)
	sxtrap (ME, "XYZ_orig");

    return indice;
}


static int  XxYxZ_suppress (erased_lnk_hd)
    int			erased_lnk_hd;
{
    /* Supprime, vis-a-vis des "foreach" les elements de la liste erased_lnk_hd. */
    register int		*low, *high, *lnk;
    register int		x, y, orig, kind;
    register int		*hash_lnk = HEADER_->hash_lnk;

    /* On commence par marquer dans les "lnk" les elements que l'on supprimera. */
    x = erased_lnk_hd;

    do {
	for (kind = 0; kind < 6;kind++)
	    if ((lnk = HEADER_->lnk [kind]) != NULL)
		lnk [x] |= XxYxZ_80;
	
    } while ((x = (hash_lnk [x] & XxYxZ_7F)) != 0);
	     

    x = erased_lnk_hd;

    do {
	for (kind = 0; kind < 6; kind++) {
	    if ((lnk = HEADER_->lnk [kind]) != NULL && (lnk [x] & XxYxZ_80) != 0) {
		low = HEADER_->lnk_hd [kind] + (orig = XYZ_orig (x, kind));
	    
		while ((y = *low) != 0) {
		    if ((*(high = lnk + y) & XxYxZ_80) == 0)
			low = high;
		    else {
			*low = *high & XxYxZ_7F;
			*high = 0; /*supprime'*/
		    }
		}
	    
		if (low == HEADER_->lnk_hd [kind] + orig) {
		    if (kind < 3)
			X_erase (*(HEADER_->X_hd [kind]), orig);
		    else
			XxY_erase (*((HEADER_->XxY_hd - 3) [kind]), orig);
		}
	    }
	}
    } while ((x = (hash_lnk [x] & XxYxZ_7F)) != 0);

    return 0;
}

void	XxYxZ_alloc (header, name, init_elem_nb, average_list_nb_per_bucket,
		     average_XYZ_assoc, user_oflw, stat_file)
    XxYxZ_header	*header;
    char	*name;
    int		init_elem_nb, average_list_nb_per_bucket;
    int		*average_XYZ_assoc;
    int		(*user_oflw) ();
    FILE	*stat_file;
{
    /* Cette fonction "cree" une matrice creuse extensible de dimension 3 */
    /* De header "header", de nom "name", dont la taille initiale de la table de hash_code
       est un nombre premier et la longueur moyenne des chaines de collision est
       "average_list_nb_per_bucket". La taille de la structure est doublee chaque fois
       que cette longueur moyenne depasse la taille fixee, de plus la fonction utilisateur
       "oflw" est appelee.
       Si "average_XYZ_assoc [K(X|_)(Y|_)(Z|_)]" est non nul, on veut pouvoir
       parcourir tous les triplets (X, Y, Z) dont on connait 0, 1 ou 2 composants,
       par l'intermediaire de la macro XxYxZ_(X|_)(Y|_)(Z|_)foreach.
       Dans ce cas "average_XYZ_assoc []" est la longueur moyenne de la liste de collision
       liant entre eux les ensembles de triplets (X, Y, Z) ayant meme valeur de hash_code
       sur les composants connus.
       Si cette valeur moyenne est depassee, la structure est doublee.
       Les triplets sont ranges par ordre croissant. */
    register int	kind, lgth, size;
    BOOLEAN		has_foreach = FALSE;

    for (kind = 0; kind < 6; kind++) {
	if (average_XYZ_assoc [kind] != 0) {
	    has_foreach = TRUE;
	    break;
	}
    }

    X_root_alloc (header, name, init_elem_nb, average_list_nb_per_bucket, XxYxZ_system_oflw,
		  user_oflw, XxYxZ_cmp, XxYxZ_scrmbl, has_foreach ? XxYxZ_suppress : NULL,
		  XxYxZ_assign, stat_file);
    header->display = (struct XxYxZ_elem*) sxalloc (header->size + 1, sizeof (struct XxYxZ_elem));

    for (kind = 0; kind < 6; kind++) {
	if ((lgth = average_XYZ_assoc [kind]) != 0) {
	    has_foreach = TRUE;

	    if (kind < 3) {
		header->X_hd [kind] = (X_header*) sxalloc (1, sizeof (X_header));
		X_alloc (header->X_hd [kind], name, 1 + (1 << ((sxlast_bit (header->size) + 1) / 3)) /* racine cubique! */, lgth, XxYxZ_assoc_oflw, stat_file);
		size = X_size (*(header->X_hd [kind]));
		header->X_hd_is_static [kind] = FALSE;
	    }
	    else {
		(header->XxY_hd - 3) [kind] = (XxY_header*) sxalloc (1, sizeof (XxY_header));
		XxY_alloc ((header->XxY_hd - 3) [kind], name, 1 + (1 << ((sxlast_bit (header->size) + 1) * 2 / 3)) /* puissance 2/3 ! */, lgth, 0, 0, XxYxZ_assoc_oflw, stat_file);
		size = XxY_size (*((header->XxY_hd - 3) [kind]));
		(header->XxY_hd_is_static - 3) [kind] = FALSE;
	    }

	    header->lnk_hd [kind] = (int*) sxalloc (size + 1, sizeof (int));
	    header->lnk_hd [kind][0] = 0 /* Permet, si liste vide, de ne pas executer un "foreach". */;
	    header->lnk [kind] = (int*) sxalloc (header->size + 1, sizeof (int));
	    header->lnk [kind][0] = 0 /* Permet de quitter un "foreach" en positionnant la variable a 0 */;
	}
	else {
	    if (kind < 3) {
		header->X_hd [kind] = NULL;
		header->X_hd_is_static [kind] = FALSE;
	    }
	    else {
		(header->XxY_hd - 3) [kind] = NULL;
		(header->XxY_hd_is_static - 3) [kind] = FALSE; 
	      }

	    header->lnk_hd [kind] = header->lnk [kind] = NULL;
	}
    }
}



void	XxYxZ_clear (header)
    XxYxZ_header		*header;
{
    register X_header	*Xhd;
    register XxY_header	*XYhd;
    register int 	kind;

    if (header->top != 0) {
	X_root_clear (header);

	for (kind = 0; kind < 3; kind++) {
	    if ((Xhd = header->X_hd [kind]) != NULL)
		X_clear (Xhd);

	    if ((XYhd = header->XxY_hd [kind]) != NULL)
		XxY_clear (XYhd);
	}
    }
}



void	XxYxZ_stat (stat_file, header)
    FILE	*stat_file;
    XxYxZ_header		*header;
{
    register int	x, i, l, n;
    register X_header	*Xhd;
    register XxY_header	*XYhd;
    register int	*lnk_hd, *lnk;
    int			minl, maxl, meanl, kind;

    if (stat_file != NULL) {
	X_root_stat (stat_file, header);

	if (header->top != 0) {
	    for (kind = 0; kind < 3; kind++) {
		if ((Xhd = header->X_hd [kind]) != NULL) {
		    lnk_hd = header->lnk_hd [kind];
		    lnk = header->lnk [kind];
		    fprintf (stat_file, "X_statistics\n");
		    X_stat (stat_file, Xhd);
		    meanl = 0;
		    maxl = 0;
		    minl = XxYxZ_7F;
		    n = 0;
		    
		    for (x = X_top (*(Xhd)); x > 0;x--) {
			n++;
			l = 0;
			
			for (i = lnk_hd [x]; i != 0; i = lnk [i]) {
			    l++;
			}
			
			if (l < minl)
			    minl = l;

			if (l > maxl)
			    maxl = l;

			meanl += l;
		    }
		    
		    meanl /= n;
		    fprintf (stat_file, "X_minl = %d, X_maxl = %d, X_meanl = %d\n", minl, maxl, meanl);
		}
	    }
	    
	    for (kind = 3; kind < 6; kind++) {
		if ((XYhd = (header->XxY_hd - 3) [kind]) != NULL) {
		    lnk_hd = header->lnk_hd [kind];
		    lnk = header->lnk [kind];
		    fprintf (stat_file, "XY_statistics\n");
		    XxY_stat (stat_file, XYhd);
		    meanl = 0;
		    maxl = 0;
		    minl = XxYxZ_7F;
		    n = 0;
		    
		    for (x = XxY_top (*(XYhd)); x > 0;x--) {
			n++;
			l = 0;
			
			for (i = lnk_hd [x]; i != 0; i = lnk [i]) {
			    l++;
			}
			
			if (l < minl)
			    minl = l;

			if (l > maxl)
			    maxl = l;

			meanl += l;
		    }
		    
		    meanl /= n;
		    fprintf (stat_file, "XY_minl = %d, XY_maxl = %d, XY_meanl = %d\n", minl, maxl, meanl);
		}
	    }
	}
    }
}




void	XxYxZ_free (header)
    XxYxZ_header		*header;
{
    register int	kind;
    register X_header	*Xhd;
    register XxY_header	*XYhd;

    if (!header->is_static)
    {
	X_root_free (header);
	sxfree (header->display), header->display = NULL;
    }

    for (kind = 0; kind < 3; kind++) {
	if ((Xhd = header->X_hd [kind]) != NULL) {
	    if (!Xhd->is_static)
	    {
		X_free (Xhd);
		sxfree (header->lnk_hd [kind]), header->lnk_hd [kind] = NULL;

		if (!header->X_hd_is_static [kind])
		    sxfree (Xhd), header->X_hd [kind] = NULL;
	    }

	    if (!header->is_static)
		sxfree (header->lnk [kind]), header->lnk [kind] = NULL;
	}
    }

    for (kind = 3; kind < 6; kind++) {
	if ((XYhd = (header->XxY_hd - 3) [kind]) != NULL) {
	    if (!XYhd->is_static)
	    {
		XxY_free (XYhd);
		sxfree (header->lnk_hd [kind]), header->lnk_hd [kind] = NULL;

		if (!(header->XxY_hd_is_static - 3) [kind])
		    sxfree (XYhd), (header->XxY_hd - 3) [kind] = NULL;
	    }

	    if (!header->is_static)
		sxfree (header->lnk [kind]), header->lnk [kind] = NULL;
	}
    }
}



int	XxYxZ_is_set (header, X, Y, Z)
    register XxYxZ_header	*header;
    int			X, Y, Z;
{
    /* Si l'element X-Y-Z de la matrice creuse header existe, cette fonction
       retourne l'identifiant de cet element, sinon elle retourne 0 */

    register int z;

    X_ = X, Y_ = Y, Z_ = Z;

    if ((z = X_root_is_set (HEADER_ = header, XxYxZ_HASH_FUN (X_, Y_, Z_))) <= 0)
	return 0;
	
    return header->hash_lnk [z] & XxYxZ_80 ? 0 : z;
}



void	XxYxZ_gc (header)
    XxYxZ_header		*header;
{
    /* Cette procedure supprime physiquement les elements effaces
       par un "XxYxZ_erase" et permet la reutilisation des indices
       ainsi recuperes. */
    register int		x, erased_lnk_hd;
    register int		*hash_lnk, *alnk;

    if (header->has_free_buckets != 0 && (erased_lnk_hd = X_root_gc (header)) > 0) {
	/* Traitement des "foreach" des elements supprimes. */
	if ((HEADER_ = header)->suppress != NULL) /* Il y a des foreach. */
	    XxYxZ_suppress (erased_lnk_hd);

	hash_lnk = HEADER_->hash_lnk;
	
	/* On concatene erased_lnh_hd a free_buckets. */
	for ((alnk = &(HEADER_->free_buckets)); (x = (*alnk & XxYxZ_7F)) != 0; alnk = hash_lnk + x);
	
	*alnk = erased_lnk_hd | XxYxZ_80;
    }
}


BOOLEAN	XxYxZ_set (header, X, Y, Z, ref)
    XxYxZ_header	*header;
    int		X, Y, Z, *ref;
{
    /* Si l'element X-Y-Z de la matrice creuse header existe, cette fonction
       retourne TRUE et ref designe cet element, sinon elle cree un nouvel
       element, ref le designe et elle retourne FALSE */

    if (header->is_locked)
	sxtrap ("XxYxZ_set", header->name);

    X_ = X, Y_ = Y, Z_ = Z;

    return X_root_set (HEADER_ = header, XxYxZ_HASH_FUN (X_, Y_, Z_), ref);
}


void	XxYxZ_lock (header)
    XxYxZ_header		*header;
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure XxYxZ
	  - Interdit l'utilisation ulterieure de XxYxZ_set
	  - Appelle (si non NULL) la procedure utilisateur oflw
       L'utisateur vient peut etre d'appeler XxYxZ_gc. */
    register int kind;

    if (header->is_locked)
	return;

    HEADER_ = header;

    if (header->suppress != NULL)
	for (kind = 0; kind < 6;kind++) {
	    if (header->lnk [kind] != NULL) {
		/* positionnement de la variable statique KIND_ pour la reallocation de lnk_hd */
		if ((KIND_ = kind) < 3)
		    X_lock (header->X_hd [kind]);
		else
		    XxY_lock ((header->XxY_hd - 3) [kind]);
	    }
	}

    X_root_lock (header);
}


void	XxYxZ_unlock (header)
    XxYxZ_header		*header;
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure XxYxZ
	  - Interdit l'utilisation ulterieure de XxYxZ_set
	  - Appelle (si non NULL) la procedure utilisateur oflw
       L'utisateur vient peut etre d'appeler XxYxZ_gc. */
    register int kind;

    if (!header->is_locked)
	return;

    if (header->suppress != NULL)
	for (kind = 0; kind < 6;kind++) {
	    if (header->lnk [kind] != NULL) {
		if (kind < 3)
		    X_unlock (*(header->X_hd [kind]));
		else
		    XxY_unlock (*((header->XxY_hd - 3) [kind]));
	    }
	}

    header->is_locked = FALSE;
}





BOOLEAN XxYxZ_write (header, F_XxYxZ)
    XxYxZ_header	*header;
    int			F_XxYxZ /* file descriptor */;
{
    int		bytes, kind;
    int		*lnk_hd, *lnk;
    BOOLEAN 	is_X_hd, is_XxY_hd;
    X_header	*X_hd;
    XxY_header	*XxY_hd;

#define WRITE(p,l)	if ((bytes = (l)) > 0 && (write (F_XxYxZ, p, (size_t)bytes) != bytes)) return FALSE

    if (!X_root_write (header, F_XxYxZ))
	return FALSE;

    WRITE (header->display, sizeof (struct XxYxZ_elem) * (header->top + 1));

    for (kind = 0; kind < 6; kind++) {
	if (kind < 3) {
	    is_X_hd = (X_hd = header->X_hd [kind]) != NULL;

	    WRITE (&(is_X_hd), sizeof (BOOLEAN));

	    if (is_X_hd) {
		lnk_hd = header->lnk_hd [kind];
		lnk = header->lnk [kind];

		if (!X_write (X_hd, F_XxYxZ))
		    return FALSE;

		WRITE (lnk_hd, sizeof (int) * (X_hd->top + 1));
		WRITE (lnk, sizeof (int) * (header->top + 1));
	    }
	}
	else {
	    is_XxY_hd = (XxY_hd = header->XxY_hd [kind - 3]) != NULL;

	    WRITE (&(is_XxY_hd), sizeof (BOOLEAN));

	    if (is_XxY_hd) {
		lnk_hd = header->lnk_hd [kind];
		lnk = header->lnk [kind];

		if (!XxY_write (XxY_hd, F_XxYxZ))
		    return FALSE;

		WRITE (lnk_hd, sizeof (int) * (XxY_hd->top + 1));
		WRITE (lnk, sizeof (int) * (header->top + 1));
	    }
	}
    }

    return TRUE;
}

BOOLEAN XxYxZ_read (header, F_XxYxZ, name, user_oflw, stat_file)
    XxYxZ_header	*header;
    int			F_XxYxZ /* file descriptor */;
    char		*name;
    int			(*user_oflw) ();
    FILE		*stat_file;
{
    int		bytes, kind;
    int		*lnk_hd, *lnk;
    BOOLEAN 	is_X_hd, is_XxY_hd;
    X_header	*X_hd;
    XxY_header	*XxY_hd;

#define READ(p,l)	if ((bytes = (l)) > 0 && (read (F_XxYxZ, p, (size_t)bytes) != bytes)) return FALSE

    if (X_root_read (header, F_XxYxZ)) {
	header->display = (struct XxYxZ_elem*) sxalloc (header->size + 1,
							sizeof (struct XxYxZ_elem));
	READ (header->display, sizeof (struct XxYxZ_elem) * (header->top + 1));
	header->name = name;
	header->system_oflw = XxYxZ_system_oflw;
	header->user_oflw = user_oflw;
	header->cmp = XxYxZ_cmp;
	header->scrmbl = XxYxZ_scrmbl;
	header->suppress = XxYxZ_suppress;
	header->assign = XxYxZ_assign;
	header->stat_file = stat_file;

	for (kind = 0; kind < 6; kind++) {
	    if (kind < 3) {
		READ (&(is_X_hd), sizeof (BOOLEAN));

		if (is_X_hd) {
		    X_hd = header->X_hd [kind] = (X_header*) sxalloc (1, sizeof (X_header));

		    if (!X_read (X_hd, F_XxYxZ, name, XxYxZ_assoc_oflw, stat_file))
			return FALSE;

		    lnk_hd = header->lnk_hd [kind] = (int*) sxalloc (X_hd->size + 1, sizeof (int));
		    lnk = header->lnk [kind] = (int*) sxalloc (header->size + 1, sizeof (int));

		    READ (lnk_hd, sizeof (int) * (X_hd->top + 1));
		    READ (lnk, sizeof (int) * (header->top + 1));
		}
		else {
		    header->X_hd [kind] = NULL;
		    header->lnk_hd [kind] = NULL;
		    header->lnk [kind] = NULL;
		}

		header->X_hd_is_static [kind] = FALSE;
	    }
	    else {
		READ (&(is_XxY_hd), sizeof (BOOLEAN));

		if (is_XxY_hd) {
		    XxY_hd = (header->XxY_hd - 3) [kind] =
			(XxY_header*) sxalloc (1, sizeof (XxY_header));

		    if (!XxY_read (XxY_hd, F_XxYxZ, name, XxYxZ_assoc_oflw, stat_file))
			return FALSE;

		    lnk_hd = header->lnk_hd [kind] = (int*) sxalloc (XxY_hd->size + 1, sizeof (int));
		    lnk = header->lnk [kind] = (int*) sxalloc (header->size + 1, sizeof (int));

		    READ (lnk_hd, sizeof (int) * (XxY_hd->top + 1));
		    READ (lnk, sizeof (int) * (header->top + 1));
		}
		else {
		    (header->XxY_hd - 3) [kind] = NULL;
		    header->lnk_hd [kind] = NULL;
		    header->lnk [kind] = NULL;
		}

		(header->XxY_hd_is_static - 3) [kind] = FALSE;
	    }
	}

	return TRUE;
    }

    return FALSE;
}


void XxYxZ_array_to_c (header, F_XxYxZ, name)
    XxYxZ_header	*header;
    FILE		*F_XxYxZ /* named output stream */;
    char		*name; 
{
    int 	i, kind, size;
    char	sub_name [128];
    X_header	*X_hd;
    XxY_header	*XxY_hd;

    sxinitialise(X_hd);  /* pour faire taire "gcc -Wuninitialized" */
    fprintf (F_XxYxZ, "\n\nstatic struct XxYxZ_elem %s_display [%i] = {", name, header->size + 1);

    for (i = 0; i <= header->top; i++) {
	if ((i & 3) == 0)
	    fprintf (F_XxYxZ, "\n/* %i */ ", i);

	fprintf (F_XxYxZ, "{%i, %i, %i}, ",
		 header->display [i].X, header->display [i].Y, header->display [i].Z);
    } 

    fprintf (F_XxYxZ, "\n};\n");

    for (kind = 0; kind < 6; kind++) {
	if (kind < 3) {
	    if ((X_hd = header->X_hd [kind]) != NULL) {
	        if ((i = strlen (name)) > 120)
		    i = 120;

		memcpy (sub_name, name, (size_t)i);
		sprintf (sub_name + i, "s_X_hd_%i", kind);
		X_to_c (X_hd, F_XxYxZ, sub_name, TRUE /* is_static */);
		size = X_hd->size;
	    }
	    else
		size = 0;
	}
	else {
	    if ((XxY_hd = (header->XxY_hd - 3) [kind]) != NULL) {
	        if ((i = strlen (name)) > 118)
		    i = 118;

		memcpy (sub_name, name, (size_t)i);
		sprintf (sub_name + i, "_XxY_hd_%i", kind - 3);
		XxY_to_c (XxY_hd, F_XxYxZ, sub_name, TRUE /* is_static */);
		size = XxY_hd->size;
	    }
	    else
		size = 0;
	}
		
	if (size != 0) {
	    /* lnk_hd [kind] */
	    fprintf (F_XxYxZ, "\n\nstatic int %s_lnk_hd_%i [%i] = {", name, kind, X_hd->size + 1);

	    for (i = 0; i <= X_hd->top; i++) {
		if ((i & 7) == 0)
		    fprintf (F_XxYxZ, "\n/* %i */ ", i);

		fprintf (F_XxYxZ, "%i, ", header->lnk_hd [kind] [i]);
	    } 

	    fprintf (F_XxYxZ, "\n};\n");

	    /* lnk [kind] */
	    fprintf (F_XxYxZ, "\n\nstatic int %s_lnk_%i [%i] = {", name, kind, header->size + 1);

	    for (i = 0; i <= header->top; i++) {
		if ((i & 7) == 0)
		    fprintf (F_XxYxZ, "\n/* %i */ ", i);

		fprintf (F_XxYxZ, "%i, ", header->lnk [kind] [i]);
	    } 

	    fprintf (F_XxYxZ, "\n};\n");
	}
    }

    X_root_array_to_c (header, F_XxYxZ, name); /* hash_display, hash_lnk */
}

void XxYxZ_header_to_c (header, F_XxYxZ, name)
    XxYxZ_header	*header;
    FILE		*F_XxYxZ /* named output stream */;
    char		*name; 
{
    int 	kind;
    BOOLEAN	B [6];

    X_root_header_to_c (header, F_XxYxZ, name);
    fprintf (F_XxYxZ, "%s_display, /* display */\n", name);

    for (kind = 0; kind < 6; kind++)
	B [kind] = (header->lnk_hd [kind] != NULL);

    fprintf (F_XxYxZ, "{%s%s%s, ", B [0] ? "&" : "NULL", B [0] ? name : "", B [0] ? "_X_hd_0" : "");
    fprintf (F_XxYxZ, "%s%s%s, ", B [1] ? "&" : "NULL", B [1] ? name : "", B [1] ? "_X_hd_1" : "");
    fprintf (F_XxYxZ, "%s%s%s}, /* X_hd [3] */\n", B [2] ? "&" : "NULL", B [2] ? name : "", B [2] ? "_X_hd_2" : "");

    fprintf (F_XxYxZ, "{%s%s%s, ", B [3] ? "&" : "NULL", B [3] ? name : "", B [3] ? "_XxY_hd_0" : "");
    fprintf (F_XxYxZ, "%s%s%s, ", B [4] ? "&" : "NULL", B [4] ? name : "", B [4] ? "_XxY_hd_1" : "");
    fprintf (F_XxYxZ, "%s%s%s}, /* XxY_hd [3] */\n", B [5] ? "&" : "NULL", B [5] ? name : "", B [5] ? "_XxY_hd_2" : "");

    fprintf (F_XxYxZ, "{%s%s, ", B [0] ? name : "NULL", B [0] ? "_lnk_hd_0" : "");
    fprintf (F_XxYxZ, "%s%s, ", B [1] ? name : "NULL", B [1] ? "_lnk_hd_1" : "");
    fprintf (F_XxYxZ, "%s%s, ", B [2] ? name : "NULL", B [2] ? "_lnk_hd_2" : "");
    fprintf (F_XxYxZ, "%s%s, ", B [3] ? name : "NULL", B [3] ? "_lnk_hd_3" : "");
    fprintf (F_XxYxZ, "%s%s, ", B [4] ? name : "NULL", B [4] ? "_lnk_hd_4" : "");
    fprintf (F_XxYxZ, "%s%s}, /* lnk_hd [6] */\n", B [5] ? name : "NULL", B [5] ? "_lnk_hd_5" : "");

    fprintf (F_XxYxZ, "{%s%s, ", B [0] ? name : "NULL", B [0] ? "_lnk_0" : "");
    fprintf (F_XxYxZ, "%s%s, ", B [1] ? name : "NULL", B [1] ? "_lnk_1" : "");
    fprintf (F_XxYxZ, "%s%s, ", B [2] ? name : "NULL", B [2] ? "_lnk_2" : "");
    fprintf (F_XxYxZ, "%s%s, ", B [3] ? name : "NULL", B [3] ? "_lnk_3" : "");
    fprintf (F_XxYxZ, "%s%s, ", B [4] ? name : "NULL", B [4] ? "_lnk_4" : "");
    fprintf (F_XxYxZ, "%s%s}, /* lnk [6] */\n", B [5] ? name : "NULL", B [5] ? "_lnk_5" : "");

    fprintf (F_XxYxZ, "{TRUE, TRUE, TRUE}, /* X_is_static [3] */\n");
    fprintf (F_XxYxZ, "{TRUE, TRUE, TRUE} /* XxY_is_static [3] */\n");
    fprintf (F_XxYxZ, "}");
}

void XxYxZ_to_c (header, F_XxYxZ, name, is_static)
    XxYxZ_header	*header;
    FILE		*F_XxYxZ /* named output stream */;
    char		*name; 
    BOOLEAN		is_static;
{
    XxYxZ_array_to_c (header, F_XxYxZ, name);
    fprintf (F_XxYxZ, "\n\n%sXxYxZ_header %s =\n", is_static ? "static " : "", name);
    XxYxZ_header_to_c (header, F_XxYxZ, name);
    fprintf (F_XxYxZ, " /* End XxYxZ_header %s */;\n", name);
}



void XxYxZ_reuse (header, name, user_oflw, stat_file)
    XxYxZ_header	*header;
    char		*name;
    int			(*user_oflw) ();
    FILE		*stat_file;
{
    int kind;

    X_root_reuse (header, name, XxYxZ_system_oflw,
		  user_oflw, XxYxZ_cmp, XxYxZ_scrmbl, XxYxZ_suppress,
		  XxYxZ_assign, stat_file);

    for (kind = 0; kind < 6; kind++) {
        if (header->lnk [kind] != NULL) {
	    if (kind < 3)
	        X_reuse (header->X_hd [kind], name, XxYxZ_assoc_oflw, stat_file);
	    else
	        XxY_reuse ((header->XxY_hd - 3) [kind], name, XxYxZ_assoc_oflw, stat_file);
	  }
    }
}

