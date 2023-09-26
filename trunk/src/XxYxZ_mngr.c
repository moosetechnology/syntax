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

static char	ME [] = "XxYxZ_mngr";

#include "sxversion.h"
#include "sxcommon.h"
#include "XxYxZ.h"
#include <memory.h>
#include <stdlib.h>

char WHAT_XXYXZ_MNGR[] = "@(#)SYNTAX - $Id: XxYxZ_mngr.c 3264 2023-05-16 08:29:29Z garavel $" WHAT_DEBUG;

extern void sxtrap (char *caller, char *message);
extern SXINT  sxlast_bit (SXINT nb);

static SXINT		X_, Y_, Z_;
static XxYxZ_header	*HEADER_;
static SXINT		KIND_;


static void XxYxZ_cpy (struct XxYxZ_elem *old, struct XxYxZ_elem *new, SXINT old_size)
{
    struct XxYxZ_elem *old_top = old + old_size;

    do {
	*new++ = *old++;
    } while (old <= old_top);
}

static void XxYxZ_assoc_oflw (SXINT old_size, SXINT new_size)
{
    /* La variable statique KIND_ est positionnee. */
    SXINT	*old = HEADER_->lnk_hd [KIND_];

    if (((KIND_ < 3) && HEADER_->X_hd_is_static [KIND_])
	|| ((KIND_ >= 3) && (HEADER_->XxY_hd_is_static - 3) [KIND_])) {
	X_root_int_cpy (old,
			HEADER_->lnk_hd [KIND_] = (SXINT*) sxalloc (new_size + 1, sizeof (SXINT)),
			old_size);
    }
    else
	HEADER_->lnk_hd [KIND_] = (SXINT*) sxrealloc (old, new_size + 1, sizeof (SXINT));
}



static void XxYxZ_system_oflw (SXINT old_top)
{
    /* Appele depuis X_root sur debordement (ou lock) de "hash_lnk". */
    /* On realloue "display" et les "lnk" des "foreach" (s'il y en a) */
    SXINT kind, *old_lnk;

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
				    HEADER_->lnk [kind] = (SXINT*) sxalloc (HEADER_->size + 1,
									  sizeof (SXINT)),
				    old_top);
		}
		else
		    HEADER_->lnk [kind] = (SXINT*) sxrealloc (old_lnk,
							    HEADER_->size + 1,
							    sizeof (SXINT));
	    }
	}
}


static SXINT XxYxZ_cmp (SXINT x)
{
    /* Appele depuis X_root_s_set.
       "Compare" l'element courant se trouvant a l'indice x avec l'element que l'on
       recherche. Cette fonction retourne
          -1 si <
	  0  si ==
	  1  si >
    */
    SXINT		z;
    struct XxYxZ_elem	*aelem = HEADER_->display + x;

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


static SXUINT XxYxZ_scrmbl (SXINT x)
{
    /* Retourne la valeur du "scramble" de l'element d'indice x. */
    /* Positionne X_, Y_ et Z_, ce qui permet d'appeler "cmp". */
    struct XxYxZ_elem	*aelem = HEADER_->display + x;

    X_ = aelem->X, Y_ = aelem->Y, Z_ = aelem->Z;
    return XxYxZ_HASH_FUN (X_, Y_, Z_);
}

static SXINT XxYxZ_assign (SXINT x)
{
    /* Positionne le triplet courant a l'indice x et traite les "foreach". */
    struct XxYxZ_elem	*aelem = HEADER_->display + x;
    SXINT		kind;
    SXINT		*lnk;
    SXINT				head;
    SXBOOLEAN			is_old;
    
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


static SXINT XYZ_orig (SXINT x, SXINT kind)
{
    struct XxYxZ_elem	*aelem = HEADER_->display + x;
    SXINT		indice;

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


static SXINT  XxYxZ_suppress (SXINT erased_lnk_hd)
{
    /* Supprime, vis-a-vis des "foreach" les elements de la liste erased_lnk_hd. */
    SXINT		*low, *high, *lnk;
    SXINT		x, y, orig, kind;
    SXINT		*hash_lnk = HEADER_->hash_lnk;

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

void	XxYxZ_alloc (XxYxZ_header *header, 
		     char *name, 
		     SXINT init_elem_nb, 
		     SXINT average_list_nb_per_bucket, 
		     SXINT *average_XYZ_assoc, 
		     void (*user_oflw) (SXINT, SXINT), 
		     FILE *stat_file)
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
    SXINT	kind, lgth, size;
    SXBOOLEAN		has_foreach = SXFALSE;

    for (kind = 0; kind < 6; kind++) {
	if (average_XYZ_assoc [kind] != 0) {
	    has_foreach = SXTRUE;
	    break;
	}
    }

    X_root_alloc ((X_root_header*)header, name, init_elem_nb, average_list_nb_per_bucket, XxYxZ_system_oflw,
		  user_oflw, XxYxZ_cmp, XxYxZ_scrmbl, has_foreach ? XxYxZ_suppress : (SXINT (*) (SXINT)) NULL,
		  XxYxZ_assign, stat_file);
    header->display = (struct XxYxZ_elem*) sxalloc (header->size + 1, sizeof (struct XxYxZ_elem));

    for (kind = 0; kind < 6; kind++) {
	if ((lgth = average_XYZ_assoc [kind]) != 0) {
	    has_foreach = SXTRUE;

	    if (kind < 3) {
		header->X_hd [kind] = (X_header*) sxalloc (1, sizeof (X_header));
		X_alloc (header->X_hd [kind], name, 1 + (1 << ((sxlast_bit (header->size) + 1) / 3)) /* racine cubique! */, lgth, XxYxZ_assoc_oflw, stat_file);
		size = X_size (*(header->X_hd [kind]));
		header->X_hd_is_static [kind] = SXFALSE;
	    }
	    else {
		(header->XxY_hd - 3) [kind] = (XxY_header*) sxalloc (1, sizeof (XxY_header));
		XxY_alloc ((header->XxY_hd - 3) [kind], name, 1 + (1 << ((sxlast_bit (header->size) + 1) * 2 / 3)) /* puissance 2/3 ! */, lgth, 0, 0, XxYxZ_assoc_oflw, stat_file);
		size = XxY_size (*((header->XxY_hd - 3) [kind]));
		(header->XxY_hd_is_static - 3) [kind] = SXFALSE;
	    }

	    header->lnk_hd [kind] = (SXINT*) sxalloc (size + 1, sizeof (SXINT));
	    header->lnk_hd [kind][0] = 0 /* Permet, si liste vide, de ne pas executer un "foreach". */;
	    header->lnk [kind] = (SXINT*) sxalloc (header->size + 1, sizeof (SXINT));
	    header->lnk [kind][0] = 0 /* Permet de quitter un "foreach" en positionnant la variable a 0 */;
	}
	else {
	    if (kind < 3) {
		header->X_hd [kind] = NULL;
		header->X_hd_is_static [kind] = SXFALSE;
	    }
	    else {
		(header->XxY_hd - 3) [kind] = NULL;
		(header->XxY_hd_is_static - 3) [kind] = SXFALSE; 
	      }

	    header->lnk_hd [kind] = header->lnk [kind] = NULL;
	}
    }
}



void	XxYxZ_clear (XxYxZ_header *header)
{
    X_header	*Xhd;
    XxY_header	*XYhd;
    SXINT 	kind;

    if (header->top != 0) {
	X_root_clear ((X_root_header*)header);

	for (kind = 0; kind < 3; kind++) {
	    if ((Xhd = header->X_hd [kind]) != NULL)
		X_clear (Xhd);

	    if ((XYhd = header->XxY_hd [kind]) != NULL)
		XxY_clear (XYhd);
	}
    }
}



void	XxYxZ_stat (FILE *stat_file, XxYxZ_header *header)
{
    SXINT	x, i, l, n;
    X_header	*Xhd;
    XxY_header	*XYhd;
    SXINT	*lnk_hd, *lnk;
    SXINT			minl, maxl, meanl, kind;

    if (stat_file != NULL) {
	X_root_stat (stat_file, (X_root_header*)header);

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
		    fprintf (stat_file, "X_minl = %ld, X_maxl = %ld, X_meanl = %ld\n", (SXINT) minl, (SXINT) maxl, (SXINT) meanl);
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
		    fprintf (stat_file, "XY_minl = %ld, XY_maxl = %ld, XY_meanl = %ld\n", (SXINT) minl, (SXINT) maxl, (SXINT) meanl);
		}
	    }
	}
    }
}




void	XxYxZ_free (XxYxZ_header *header)
{
    SXINT	kind;
    X_header	*Xhd;
    XxY_header	*XYhd;

    if (!header->is_static)
    {
	X_root_free ((X_root_header*)header);
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



SXINT	XxYxZ_is_set (XxYxZ_header *header, SXINT X, SXINT Y, SXINT Z)
{
    /* Si l'element X-Y-Z de la matrice creuse header existe, cette fonction
       retourne l'identifiant de cet element, sinon elle retourne 0 */

    SXINT z;

    X_ = X, Y_ = Y, Z_ = Z;

    if ((z = X_root_is_set ((X_root_header*)(HEADER_ = header), XxYxZ_HASH_FUN (X_, Y_, Z_))) <= 0)
	return 0;
	
    return header->hash_lnk [z] & XxYxZ_80 ? 0 : z;
}



void	XxYxZ_gc (XxYxZ_header *header)
{
    /* Cette procedure supprime physiquement les elements effaces
       par un "XxYxZ_erase" et permet la reutilisation des indices
       ainsi recuperes. */
    SXINT		x, erased_lnk_hd;
    SXINT		*hash_lnk, *alnk;

    if (header->has_free_buckets != 0 && (erased_lnk_hd = X_root_gc ((X_root_header*)header)) > 0) {
	/* Traitement des "foreach" des elements supprimes. */
	if ((HEADER_ = header)->suppress != NULL) /* Il y a des foreach. */
	    XxYxZ_suppress (erased_lnk_hd);

	hash_lnk = HEADER_->hash_lnk;
	
	/* On concatene erased_lnh_hd a free_buckets. */
	for ((alnk = &(HEADER_->free_buckets)); (x = (*alnk & XxYxZ_7F)) != 0; alnk = hash_lnk + x);
	
	*alnk = erased_lnk_hd | XxYxZ_80;
    }
}


SXBOOLEAN	XxYxZ_set (XxYxZ_header *header, SXINT X, SXINT Y, SXINT Z, SXINT *ref)
{
    /* Si l'element X-Y-Z de la matrice creuse header existe, cette fonction
       retourne SXTRUE et ref designe cet element, sinon elle cree un nouvel
       element, ref le designe et elle retourne SXFALSE */

    if (header->is_locked)
	sxtrap ("XxYxZ_set", header->name);

    X_ = X, Y_ = Y, Z_ = Z;

    return X_root_set ((X_root_header*)(HEADER_ = header), XxYxZ_HASH_FUN (X_, Y_, Z_), ref);
}


void	XxYxZ_lock (XxYxZ_header *header)
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure XxYxZ
	  - Interdit l'utilisation ulterieure de XxYxZ_set
	  - Appelle (si non NULL) la procedure utilisateur oflw
       L'utisateur vient peut etre d'appeler XxYxZ_gc. */
    SXINT kind;

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

    X_root_lock ((X_root_header*)header);
}


void	XxYxZ_unlock (XxYxZ_header *header)
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure XxYxZ
	  - Interdit l'utilisation ulterieure de XxYxZ_set
	  - Appelle (si non NULL) la procedure utilisateur oflw
       L'utisateur vient peut etre d'appeler XxYxZ_gc. */
    SXINT kind;

    if (!header->is_locked)
	return;

    if (header->suppress != NULL)
	for (kind = 0; kind < 6;kind++) {
	    if (header->lnk [kind] != NULL) {
		if (kind < 3)
		  X_unlock (*(header->X_hd [kind]));
		else
		    XxY_unlock ((header->XxY_hd - 3) [kind]);
	    }
	}

    header->is_locked = SXFALSE;
}





SXBOOLEAN XxYxZ_write (XxYxZ_header *header, sxfiledesc_t F_XxYxZ /* file descriptor */)
{
    size_t      bytes;
    SXINT	kind;
    SXINT	*lnk_hd, *lnk;
    SXBOOLEAN 	is_X_hd, is_XxY_hd;
    X_header	*X_hd;
    XxY_header	*XxY_hd;

#define WRITE(p,l)	if ((bytes = (l)) > 0 && ((size_t)write (F_XxYxZ, p, bytes) != bytes)) return SXFALSE

    if (!X_root_write ((X_root_header*)header, F_XxYxZ))
	return SXFALSE;

    WRITE (header->display, sizeof (struct XxYxZ_elem) * (header->top + 1));

    for (kind = 0; kind < 6; kind++) {
	if (kind < 3) {
	    is_X_hd = (X_hd = header->X_hd [kind]) != NULL;

	    WRITE (&(is_X_hd), sizeof (SXBOOLEAN));

	    if (is_X_hd) {
		lnk_hd = header->lnk_hd [kind];
		lnk = header->lnk [kind];

		if (!X_write (X_hd, F_XxYxZ))
		    return SXFALSE;

		WRITE (lnk_hd, sizeof (SXINT) * (X_hd->top + 1));
		WRITE (lnk, sizeof (SXINT) * (header->top + 1));
	    }
	}
	else {
	    is_XxY_hd = (XxY_hd = header->XxY_hd [kind - 3]) != NULL;

	    WRITE (&(is_XxY_hd), sizeof (SXBOOLEAN));

	    if (is_XxY_hd) {
		lnk_hd = header->lnk_hd [kind];
		lnk = header->lnk [kind];

		if (!XxY_write (XxY_hd, F_XxYxZ))
		    return SXFALSE;

		WRITE (lnk_hd, sizeof (SXINT) * (XxY_hd->top + 1));
		WRITE (lnk, sizeof (SXINT) * (header->top + 1));
	    }
	}
    }

    return SXTRUE;
}

SXBOOLEAN XxYxZ_read (XxYxZ_header *header, 
		    sxfiledesc_t F_XxYxZ /* file descriptor */, 
		    char *name, 
		    void (*user_oflw) (SXINT, SXINT), 
		    FILE *stat_file)
{
    size_t      bytes;
    SXINT	kind;
    SXINT	*lnk_hd, *lnk;
    SXBOOLEAN 	is_X_hd, is_XxY_hd;
    X_header	*X_hd;
    XxY_header	*XxY_hd;

#define READ(p,l)	if ((bytes = (l)) > 0 && ((size_t)read (F_XxYxZ, p, bytes) != bytes)) return SXFALSE

    if (X_root_read ((X_root_header*)header, F_XxYxZ)) {
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
		READ (&(is_X_hd), sizeof (SXBOOLEAN));

		if (is_X_hd) {
		    X_hd = header->X_hd [kind] = (X_header*) sxalloc (1, sizeof (X_header));

		    if (!X_read (X_hd, F_XxYxZ, name, XxYxZ_assoc_oflw, stat_file))
			return SXFALSE;

		    lnk_hd = header->lnk_hd [kind] = (SXINT*) sxalloc (X_hd->size + 1, sizeof (SXINT));
		    lnk = header->lnk [kind] = (SXINT*) sxalloc (header->size + 1, sizeof (SXINT));

		    READ (lnk_hd, sizeof (SXINT) * (X_hd->top + 1));
		    READ (lnk, sizeof (SXINT) * (header->top + 1));
		}
		else {
		    header->X_hd [kind] = NULL;
		    header->lnk_hd [kind] = NULL;
		    header->lnk [kind] = NULL;
		}

		header->X_hd_is_static [kind] = SXFALSE;
	    }
	    else {
		READ (&(is_XxY_hd), sizeof (SXBOOLEAN));

		if (is_XxY_hd) {
		    XxY_hd = (header->XxY_hd - 3) [kind] =
			(XxY_header*) sxalloc (1, sizeof (XxY_header));

		    if (!XxY_read (XxY_hd, F_XxYxZ, name, XxYxZ_assoc_oflw, stat_file))
			return SXFALSE;

		    lnk_hd = header->lnk_hd [kind] = (SXINT*) sxalloc (XxY_hd->size + 1, sizeof (SXINT));
		    lnk = header->lnk [kind] = (SXINT*) sxalloc (header->size + 1, sizeof (SXINT));

		    READ (lnk_hd, sizeof (SXINT) * (XxY_hd->top + 1));
		    READ (lnk, sizeof (SXINT) * (header->top + 1));
		}
		else {
		    (header->XxY_hd - 3) [kind] = NULL;
		    header->lnk_hd [kind] = NULL;
		    header->lnk [kind] = NULL;
		}

		(header->XxY_hd_is_static - 3) [kind] = SXFALSE;
	    }
	}

	return SXTRUE;
    }

    return SXFALSE;
}


void XxYxZ_array_to_c (XxYxZ_header *header, FILE *F_XxYxZ /* named output stream */, char *name)
{
    SXUINT       i;
    SXINT 	i_counter, kind, size;
    char	sub_name [128];
    X_header	*X_hd;
    XxY_header	*XxY_hd;

    sxinitialise(X_hd);  /* pour faire taire "gcc -Wuninitialized" */
    fprintf (F_XxYxZ, "\n\nstatic struct XxYxZ_elem %s_display [%ld] = {", name, (SXINT) header->size + 1);

    for (i_counter = 0; i_counter <= header->top; i_counter++) {
	if ((i_counter & 3) == 0)
	    fprintf (F_XxYxZ, "\n/* %ld */ ", (SXINT) i_counter);

	fprintf (F_XxYxZ, "{%ld, %ld, %ld}, ",
		 (SXINT) header->display [i_counter].X, (SXINT) header->display [i_counter].Y, (SXINT) header->display [i_counter].Z);
    } 

    fprintf (F_XxYxZ, "\n};\n");

    for (kind = 0; kind < 6; kind++) {
	if (kind < 3) {
	    if ((X_hd = header->X_hd [kind]) != NULL) {
	        if ((i = strlen (name)) > 120)
		    i = 120;

		memcpy (sub_name, name, (size_t)i);
		sprintf (sub_name + i, "s_X_hd_%ld", (SXINT) kind);
		X_to_c (X_hd, F_XxYxZ, sub_name, SXTRUE /* is_static */);
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
		sprintf (sub_name + i, "_XxY_hd_%ld", (SXINT) kind - 3);
		XxY_to_c (XxY_hd, F_XxYxZ, sub_name, SXTRUE /* is_static */);
		size = XxY_hd->size;
	    }
	    else
		size = 0;
	}
		
	if (size != 0) {
	    /* lnk_hd [kind] */
	    fprintf (F_XxYxZ, "\n\nstatic SXINT %s_lnk_hd_%ld [%ld] = {", name, (SXINT) kind, (SXINT) X_hd->size + 1);

	    for (i_counter = 0; i_counter <= X_hd->top; i_counter++) {
		if ((i_counter & 7) == 0)
		    fprintf (F_XxYxZ, "\n/* %ld */ ", (SXINT) i_counter);

		fprintf (F_XxYxZ, "%ld, ", (SXINT) header->lnk_hd [kind] [i_counter]);
	    } 

	    fprintf (F_XxYxZ, "\n};\n");

	    /* lnk [kind] */
	    fprintf (F_XxYxZ, "\n\nstatic SXINT %s_lnk_%ld [%ld] = {", name, (SXINT) kind, (SXINT) header->size + 1);

	    for (i_counter = 0; i_counter <= header->top; i_counter++) {
		if ((i_counter & 7) == 0)
		    fprintf (F_XxYxZ, "\n/* %ld */ ", (SXINT) i_counter);

		fprintf (F_XxYxZ, "%ld, ", (SXINT) header->lnk [kind] [i_counter]);
	    } 

	    fprintf (F_XxYxZ, "\n};\n");
	}
    }

    X_root_array_to_c ((X_root_header*)header, F_XxYxZ, name); /* hash_display, hash_lnk */
}

void XxYxZ_header_to_c (XxYxZ_header *header, FILE *F_XxYxZ /* named output stream */, char *name)
{
    SXINT 	kind;
    SXBOOLEAN	B [6];

    X_root_header_to_c ((X_root_header*)header, F_XxYxZ, name);
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

    fprintf (F_XxYxZ, "{SXTRUE, SXTRUE, SXTRUE}, /* X_is_static [3] */\n");
    fprintf (F_XxYxZ, "{SXTRUE, SXTRUE, SXTRUE} /* XxY_is_static [3] */\n");
    fprintf (F_XxYxZ, "}");
}

void XxYxZ_to_c (XxYxZ_header *header, FILE *F_XxYxZ /* named output stream */, char *name, SXBOOLEAN is_static)
{
    XxYxZ_array_to_c (header, F_XxYxZ, name);
    fprintf (F_XxYxZ, "\n\n%sXxYxZ_header %s =\n", is_static ? "static " : "", name);
    XxYxZ_header_to_c (header, F_XxYxZ, name);
    fprintf (F_XxYxZ, " /* End XxYxZ_header %s */;\n", name);
}



void XxYxZ_reuse (XxYxZ_header *header, 
		  char *name, 
		  void (*user_oflw) (SXINT, SXINT), 
		  FILE *stat_file)
{
    SXINT kind;

    X_root_reuse ((X_root_header*)header, name, XxYxZ_system_oflw,
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

