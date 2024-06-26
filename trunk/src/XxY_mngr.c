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

static char	ME [] = "XxY_mngr";

#include "sxversion.h"
#include "sxcommon.h"
#include "XxY.h"
#include <memory.h>

char WHAT_XXY_MNGR[] = "@(#)SYNTAX - $Id: XxY_mngr.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

extern void sxtrap (char *caller, char *message);
extern SXINT  sxlast_bit (SXINT nb);

static SXINT		X_, Y_;
static XxY_header	*HEADER_;
static SXINT		KIND_;


static void XxY_cpy (struct XxY_elem *old, struct XxY_elem *new, SXINT old_size)
{
    struct XxY_elem *old_top = old + old_size;

    do {
	*new++ = *old++;
    } while (old <= old_top);
}

static void XxY_assoc_oflw (SXINT old_size, SXINT new_size)
{
    /* La variable statique KIND_ est positionnee. */
    SXINT	*old = HEADER_->lnk_hd [KIND_];

    if (HEADER_->X_hd_is_static [KIND_]) {
	X_root_int_cpy (old,
			HEADER_->lnk_hd [KIND_] = (SXINT*) sxalloc (new_size + 1, sizeof (SXINT)),
			old_size);
    }
    else
	HEADER_->lnk_hd [KIND_] = (SXINT*) sxrealloc (old, new_size + 1, sizeof (SXINT));
}



static void XxY_system_oflw (SXINT old_top)
{
    /* Appele depuis X_root sur debordement (ou lock) de "hash_lnk". */
    /* Suivant que la structure est statique ou non, On alloue ou realloue
       "display" et les "lnk" des "foreach" (s'il y en a) */
    SXINT kind, *old_lnk;

    if (HEADER_->is_static) {
	/* Premiere allocation dynamique */
	struct XxY_elem *old_display = HEADER_->display;

	XxY_cpy (old_display,
		 HEADER_->display = (struct XxY_elem*) sxalloc (HEADER_->size + 1,
								sizeof (struct XxY_elem)),
		 old_top);
    }
    else
	HEADER_->display = (struct XxY_elem*) sxrealloc (HEADER_->display,
							 HEADER_->size + 1,
							 sizeof (struct XxY_elem));
    
    if (HEADER_->suppress != NULL)
	for (kind = 0; kind < 2;kind++) {
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


static SXINT XxY_cmp (SXINT x)
{
    /* Appele depuis X_root_is_set.
       "Compare" l'element courant se trouvant a l'indice x avec l'element que l'on
       recherche. Cette fonction retourne
          -1 si <
	  0  si ==
	  1  si >
    */
    SXINT		z;
    struct XxY_elem	*aelem = HEADER_->display + x;

    if ((z = aelem->X) >= X_) {
	if (z == X_) {
	    if ((z = aelem->Y) >= Y_)
		return z == Y_ ? 0 : 1;

	    return -1;
	}

	return 1;
    }

    return -1;
}


static SXUINT XxY_scrmbl (SXINT x)
{
    /* Retourne la valeur du "scramble" de l'element d'indice x. */
    /* Positionne X_ et Y_, ce qui permet d'appeler "cmp". */
    struct XxY_elem	*aelem = HEADER_->display + x;

    X_ = aelem->X, Y_ = aelem->Y;
    return XxY_HASH_FUN (X_, Y_);
}

static SXINT XxY_assign (SXINT x)
{
    /* Positionne le doublet courant a l'indice x et traite les "foreach". */
    struct XxY_elem	*aelem = HEADER_->display + x;
    SXINT		kind;
    SXINT		*lnk;
    SXINT				head;
    
    aelem->X = X_;
    aelem->Y = Y_;

    if (HEADER_->suppress != NULL) {
	/* Il y a des "foreach". */
	for (kind = 0; kind < 2; kind++) {
	    /* Traitement du cas "associatif" */
	    if ((lnk = HEADER_->lnk [kind]) != NULL) {
		/* positionnement de la variable statique KIND_ si X_hd deborde */
		if (X_set (HEADER_->X_hd [KIND_ = kind], kind == XY_X ? X_ : Y_, &head))
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


static SXINT XY_orig (SXINT x, SXINT kind)
{
    struct XxY_elem	*aelem = HEADER_->display + x;
    SXINT		indice;

    if ((indice = X_is_set (HEADER_->X_hd [kind], kind == XY_X ? aelem->X : aelem->Y)) == 0)
	sxtrap (ME, "XY_orig");

    return indice;
}


static SXINT  XxY_suppress (SXINT erased_lnk_hd)
{
    /* Supprime, vis-a-vis des "foreach" les elements de la liste erased_lnk_hd. */
    SXINT		*low, *lnk;
    SXINT		x, y, orig;
    SXINT		*hash_lnk = HEADER_->hash_lnk;

    if ((lnk = HEADER_->lnk [XY_X]) != NULL) {
	x = erased_lnk_hd;

	do {
	    low = HEADER_->lnk_hd [XY_X] + (orig = XY_orig (x, (SXINT)XY_X));

	    while ((y = *low) != x)
		low = lnk + y;

	    if ((*low = lnk [x]) == 0 && low == HEADER_->lnk_hd [XY_X] + orig)
		X_erase (*(HEADER_->X_hd [XY_X]), orig);

	} while ((x = (hash_lnk [x] & XxY_7F)) != 0);
    }

    if ((lnk = HEADER_->lnk [XY_Y]) != NULL) {
	x = erased_lnk_hd;

	do {
	    low = HEADER_->lnk_hd [XY_Y] + (orig = XY_orig (x, (SXINT)XY_Y));

	    while ((y = *low) != x)
		low = lnk + y;

	    if ((*low = lnk [x]) == 0 && low == HEADER_->lnk_hd [XY_Y] + orig)
		X_erase (*(HEADER_->X_hd [XY_Y]), orig);

	} while ((x = (hash_lnk [x] & XxY_7F)) != 0);
    }

    return 0;
}

void	XxY_alloc (XxY_header *header, char *name, SXINT init_elem_nb, SXINT average_list_nb_per_bucket, SXINT average_X_assoc, SXINT average_Y_assoc, sxoflw0_t user_oflw, FILE *stat_file)
{
    /* Cette fonction "cree" une matrice creuse extensible de dimension 2 */
    /* De header "header", de nom "name", dont la taille initiale de la table de hash_code
       est un nombre premier et la longueur moyenne des chaines de collision est
       "average_list_nb_per_bucket". La taille de la structure est doublee chaque fois
       que cette longueur moyenne depasse la taille fixee, de plus la fonction utilisateur
       "oflw" est appelee.
       Si "average_X_assoc" (resp. "average_y_assoc") est non nul, on veut pouvoir
       parcourir tous les couples (X, Y) ayant un X (resp. Y) donne, par l'intermediaire
       de la macro XxY_Xforeach (resp. XxY_Yforeach). Dans ce cas "average_X_assoc"
       (resp. "average_y_assoc") est la longueur moyenne de la liste de collision
       liant entre eux les ensembles de couples (X, Y) ayant meme valeur de hash_code
       sur X (resp. Y). Si cette valeur moyenne est depassee, la structure est doublee.
       Pour un X (resp. Y) donne, on obtient les Y (resp. X) correspondants par ordre
       croissant. */
    SXINT	lgth, has_foreach;

    sxinitialise(lgth); /* pour faire taire "gcc -Wuninitialized" */
    has_foreach = average_X_assoc + average_Y_assoc;

    X_root_alloc ((X_root_header*)header, name, init_elem_nb, average_list_nb_per_bucket, XxY_system_oflw,
		  user_oflw, XxY_cmp, XxY_scrmbl,
		 has_foreach ? XxY_suppress : (SXINT (*) (SXINT)) NULL,
		  XxY_assign, stat_file);
    header->display = (struct XxY_elem*) sxalloc (header->size + 1, sizeof (struct XxY_elem));

    if (has_foreach)
	/* On suppose que la relation est pleine a 1/16 (environ 7%), l'estimation du
	   nombre de lignes (ou de colonnes) est donc de 4 fois la racine carree. */
	/* 4 fois la racine carree (gromo sodo) */
	lgth = 1 + (1 << ((sxlast_bit (header->size) + 1 + 4) / 2));

    if (average_X_assoc != 0) {
	header->X_hd [XY_X] = (X_header*) sxalloc (1, sizeof (X_header));
	X_alloc (header->X_hd [XY_X], name, lgth, average_X_assoc, XxY_assoc_oflw, stat_file);
	header->lnk_hd [XY_X] = (SXINT*) sxalloc (X_size (*(header->X_hd [XY_X])) + 1, sizeof (SXINT));
	header->lnk_hd [XY_X][0] = 0 /* Permet, si liste vide, de ne pas executer un "foreach". */;
	header->lnk [XY_X] = (SXINT*) sxalloc (header->size + 1, sizeof (SXINT));
	header->lnk [XY_X][0] = 0 /* Permet de quitter un "foreach" en positionnant la variable a 0 */;
	header->X_hd_is_static [XY_X] = false;
    }
    else {
	header->X_hd [XY_X] = NULL;
	header->lnk_hd [XY_X] = header->lnk [XY_X] = NULL;
	header->X_hd_is_static [XY_X] = false;
    }

    if (average_Y_assoc != 0) {
	header->X_hd [XY_Y] = (X_header*) sxalloc (1, sizeof (X_header));
	X_alloc (header->X_hd [XY_Y], name, lgth, average_Y_assoc, XxY_assoc_oflw, stat_file);
	header->lnk_hd [XY_Y] = (SXINT*) sxalloc (X_size (*(header->X_hd [XY_Y])) + 1, sizeof (SXINT));
	header->lnk_hd [XY_Y][0] = 0 /* Permet, si liste vide, de ne pas executer un "foreach". */;
	header->lnk [XY_Y] = (SXINT*) sxalloc (header->size + 1, sizeof (SXINT));
	header->lnk [XY_Y][0] = 0 /* Permet de quitter un "foreach" en positionnant la variable a 0 */;
	header->X_hd_is_static [XY_Y] = false;
    }
    else {
	header->X_hd [XY_Y] = NULL;
	header->lnk_hd [XY_Y] = header->lnk [XY_Y] = NULL;
	header->X_hd_is_static [XY_Y] = false;
    }
}



void	XxY_clear (XxY_header *header)
{
    X_header	*Xhd;
    SXINT 	kind;

    if (header->top != 0) {
	X_root_clear ((X_root_header*)header);

	for (kind = 0; kind < 2; kind++) {
	    if ((Xhd = header->X_hd [kind]) != NULL)
		X_clear (Xhd);
	}
    }
}



void	XxY_stat (FILE *stat_file, XxY_header *header)
{
    SXINT	x, i, l, n;
    X_header	*Xhd;
    SXINT	*lnk_hd, *lnk;
    SXINT			minl, maxl, meanl, kind;

    if (stat_file != NULL) {
	X_root_stat (stat_file, (X_root_header*)header);

	if (header->top != 0) {
	    for (kind = 0; kind < 2; kind++) {
		if ((Xhd = header->X_hd [kind]) != NULL) {
		    lnk_hd = header->lnk_hd [kind];
		    lnk = header->lnk [kind];
		    fprintf (stat_file, "X_statistics\n");
		    X_stat (stat_file, Xhd);
		    meanl = 0;
		    maxl = 0;
		    minl = XxY_7F;
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
	}
    }
}




void	XxY_free (XxY_header *header)
{
    SXINT	kind;
    X_header	*Xhd;

    if (!header->is_static)
    {
	X_root_free ((X_root_header*)header);
	sxfree (header->display), header->display = NULL;
    }

    for (kind = 0; kind < 2; kind++) {
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
}



SXINT	XxY_is_set (XxY_header *header, SXINT X, SXINT Y)
{
    /* Si l'element X-Y de la matrice creuse header existe, cette fonction
       retourne l'identifiant de cet element, sinon elle retourne 0 */

    SXINT z;

    X_ = X, Y_ = Y;

    if ((z = X_root_is_set ((X_root_header*)(HEADER_ = header), XxY_HASH_FUN (X_, Y_))) <= 0)
	return 0;
	
    return header->hash_lnk [z] & XxY_80 ? 0 : z;
}



void	XxY_gc (XxY_header *header)
{
    /* Cette procedure supprime physiquement les elements effaces
       par un "XxY_erase" et permet la reutilisation des indices
       ainsi recuperes. */
    SXINT		x, erased_lnk_hd;
    SXINT		*hash_lnk, *alnk;

    if (header->has_free_buckets != 0 && (erased_lnk_hd = X_root_gc ((X_root_header*)header)) > 0) {
	/* Traitement des "foreach" des elements supprimes. */
	if ((HEADER_ = header)->suppress != NULL) /* Il y a des foreach. */
	    XxY_suppress (erased_lnk_hd);

	hash_lnk = HEADER_->hash_lnk;
	
	/* On concatene erased_lnh_hd a free_buckets. */
	for ((alnk = &(HEADER_->free_buckets)); (x = (*alnk & XxY_7F)) != 0; alnk = hash_lnk + x);
	
	*alnk = erased_lnk_hd | XxY_80;
    }
}


bool	XxY_set (XxY_header *header, SXINT X, SXINT Y, SXINT *ref)
{
    /* Si l'element X-Y de la matrice creuse header existe, cette fonction
       retourne true et ref designe cet element, sinon elle cree un nouvel
       element, ref le designe et elle retourne false */

    if (header->is_locked)
	sxtrap ("XxY_set", header->name);

    X_ = X, Y_ = Y;

    return X_root_set ((X_root_header*)(HEADER_ = header), XxY_HASH_FUN (X_, Y_), ref);
}


void	XxY_lock (XxY_header *header)
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure XxY
	  - Interdit l'utilisation ulterieure de XxY_set
	  - Appelle (si non NULL) la procedure utilisateur oflw
       L'utisateur vient peut etre d'appeler XxY_gc. */
    SXINT kind;

    if (header->is_locked)
	return;

    HEADER_ = header;

    if (header->suppress != NULL)
	for (kind = 0; kind < 2;kind++) {
	    if (header->lnk [kind] != NULL) {
		/* positionnement de la variable statique KIND_ pour
		   la reallocation de lnk_hd */
		X_lock (header->X_hd [KIND_ = kind]);
	    }
	}

    X_root_lock ((X_root_header*)header);
}


void	XxY_unlock (XxY_header *header)
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure XxY
	  - Interdit l'utilisation ulterieure de XxY_set
	  - Appelle (si non NULL) la procedure utilisateur oflw
       L'utisateur vient peut etre d'appeler XxY_gc. */
    SXINT kind;

    if (!header->is_locked)
	return;

    if (header->suppress != NULL)
	for (kind = 0; kind < 2;kind++) {
	    if (header->lnk [kind] != NULL) {
	      X_unlock (*(header->X_hd [kind]));
	    }
	}

    header->is_locked = false;
}


bool	XxY_set_and_close (XxY_header *header, SXINT X, SXINT Y, SXINT *ref)
{
    /* Si R est une relation binaire, header contient la fermeture transitive
       R+ de R.
       Complete, si necessaire, R+ par ajout de l'element (X, Y).
       Ref designe ce (nouvel) element.
       Retourne true si ref existait deja, false sinon. */
    SXINT	x, y, X1;

    if ((*ref = XxY_is_set (header, X, Y)))
	return true;

    if (X != Y) {
	XxY_Yforeach (*header, X, x) {
	    X1 = XxY_X (*header, x);

	    XxY_Xforeach (*header, Y, y) {
		XxY_set (header, X1, XxY_Y (*header, y), ref);
	    }

	    XxY_set (header, X1, Y, ref);
	}

	XxY_Xforeach (*header, Y, y) {
	    XxY_set (header, X, XxY_Y (*header, y), ref);
	}
    }

    return XxY_set (header, X, Y, ref);
}


void	XxY_to_SXBM (XxY_header *header, bool (*get_couple) (SXINT*, SXINT*, SXINT), SXINT couple_nb, X_header *index_hd, SXBA **M)
{
    /* Si header est non NULL, il contient une relation binaire R.
       Sinon, on peut obtenir successivement les couples de cette relation
       par get_couple.
       couple_nb est le nombre de couples.
       Cette procedure stocke "au minimum" cette relation dans la matrice
       de bits carree M.
       index_hd est un X_header qui assure la correspondance entre
       les constituants des elements de R et les indices de M. */
    SXINT	x, n, old_n;
    SXINT			X, Y, ind1, ind2;
    SXBA	*bm;
    bool 		OK;

    n = 2 * couple_nb + 1;

    if (*M == NULL) {
	X_alloc (index_hd, "index_hd", n, 2, (sxoflw0_t) NULL, NULL);
	bm = *M = sxbm_calloc (n, n);
    }
    else {
	X_clear (index_hd);
	/* On suppose que M est une matrice carree */
	bm = *M;
	old_n = (SXINT) SXBASIZE (bm [0]);
	X = (n < old_n) ? n : old_n;

	for (x = 1; x < X; x++)
	    sxba_empty (bm [x]);
	
	if (n > old_n)
	    bm = *M = sxbm_resize (bm, old_n, n, n);
    }

    /* On initialise M avec R */
    /* On remplit index_hd */

    for (x = 1; x <= couple_nb; x++) {
	if (get_couple != NULL) {
	    OK = true;
	    get_couple (&X, &Y, x);
	}
	else if ((OK = !XxY_is_erased (*header, x))) {
	    X = XxY_X (*header, x);
	    Y = XxY_Y (*header, x);
	}

	if (OK) {
	    X_set (index_hd, X, &ind1);
	    X_set (index_hd, Y, &ind2);
	    SXBA_1_bit (bm [ind1], ind2);
	}
    }
}


bool XxY_write (XxY_header *header, sxfiledesc_t F_XxY /* file descriptor */)
{
    size_t      bytes;
    SXINT	kind;
    X_header	*X_hd;
    SXINT	*lnk_hd, *lnk;
    bool 	is_X_hd;

#define WRITE(p,l)	if ((bytes = (l)) > 0 && ((size_t)write (F_XxY, p, bytes) != bytes)) return false

    if (!X_root_write ((X_root_header*)header, F_XxY))
	return false;

    WRITE (header->display, sizeof (struct XxY_elem) * (header->top + 1));

    for (kind = 0; kind < 2; kind++) {
	is_X_hd = (X_hd = header->X_hd [kind]) != NULL;

	WRITE (&(is_X_hd), sizeof (bool));

	if (is_X_hd) {
	    if (!X_write (X_hd, F_XxY))
		return false;

	    lnk_hd = header->lnk_hd [kind];
	    lnk = header->lnk [kind];

	    WRITE (lnk_hd, sizeof (SXINT) * (X_hd->top + 1));
	    WRITE (lnk, sizeof (SXINT) * (header->top + 1));
	}
    }

    return true;
}


bool XxY_read (XxY_header *header, sxfiledesc_t F_XxY /* file descriptor */, char *name, void (*user_oflw) (SXINT, SXINT), FILE *stat_file)
{
    size_t       bytes;
    SXINT        kind;
    X_header	*X_hd;
    SXINT		*lnk_hd, *lnk;
    bool	is_X_hd	;

#define READ(p,l)	if ((bytes=(l))>0&&(((size_t)read (F_XxY, p, bytes)) != bytes)) return false

    if (X_root_read ((X_root_header*)header, F_XxY)) {
	header->display = (struct XxY_elem*) sxalloc (header->size + 1, sizeof (struct XxY_elem));
	READ (header->display, sizeof (struct XxY_elem) * (header->top + 1));
	header->name = name;
	header->system_oflw = XxY_system_oflw;
	header->user_oflw = user_oflw;
	header->cmp = XxY_cmp;
	header->scrmbl = XxY_scrmbl;
	header->suppress = XxY_suppress;
	header->assign = XxY_assign;
	header->stat_file = stat_file;

	for (kind = 0; kind < 2; kind++) {
	    READ (&(is_X_hd), sizeof (bool));

	    if (is_X_hd) {
		X_hd = header->X_hd [kind] = (X_header*) sxalloc (1, sizeof (X_header));

		if (!X_read (X_hd, F_XxY, name, XxY_assoc_oflw, stat_file))
		    return false;

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

	    header->X_hd_is_static [kind] = false;
	}

	return true;
    }

    return false;
}


void XxY_array_to_c (XxY_header *header, FILE *F_XxY /* named output stream */, char *name)
{
    SXUINT	i;
    SXINT       i_counter, kind;
    char	sub_name [128];
    X_header	*X_hd;

    fprintf (F_XxY, "\n\nstatic struct XxY_elem %s_display [%ld] = {", name, (SXINT) header->size + 1);

    for (i_counter = 0; i_counter <= header->top; i_counter++) {
	if ((i_counter & 3) == 0)
	    fprintf (F_XxY, "\n/* %ld */ ", (SXINT) i_counter);

	fprintf (F_XxY, "{%ld, %ld}, ", (SXINT) header->display [i_counter].X, (SXINT) header->display [i_counter].Y);
    } 

    fprintf (F_XxY, "\n};\n");

    for (kind = 0; kind < 2; kind++) {
	if ((X_hd = header->X_hd [kind]) != NULL) {
	    if ((i = strlen (name)) > 120)
	        i = 120;

	    memcpy (sub_name, name, (size_t)i);
	    sprintf (sub_name + i, "_X_hd_%ld", (SXINT) kind);
	    X_to_c (X_hd, F_XxY, sub_name, true /* is_static */);

	    /* lnk_hd [kind] */
	    fprintf (F_XxY, "\n\nstatic SXINT %s_lnk_hd_%ld [%ld] = {", name, (SXINT) kind, (SXINT) X_hd->size + 1);

	    for (i_counter = 0; i_counter <= X_hd->top; i_counter++) {
		if ((i_counter & 7) == 0)
		    fprintf (F_XxY, "\n/* %ld */ ", (SXINT) i_counter);

		fprintf (F_XxY, "%ld, ", (SXINT) header->lnk_hd [kind] [i_counter]);
	    } 

	    fprintf (F_XxY, "\n};\n");

	    /* lnk [kind] */
	    fprintf (F_XxY, "\n\nstatic SXINT %s_lnk_%ld [%ld] = {", name, (SXINT) kind, (SXINT) header->size + 1);

	    for (i_counter = 0; i_counter <= header->top; i_counter++) {
		if ((i_counter & 7) == 0)
		    fprintf (F_XxY, "\n/* %ld */ ", (SXINT) i_counter);

		fprintf (F_XxY, "%ld, ", (SXINT) header->lnk [kind] [i_counter]);
	    } 

	    fprintf (F_XxY, "\n};\n");
	}
    }

    X_root_array_to_c ((X_root_header*)header, F_XxY, name); /* hash_display, hash_lnk */
}

void XxY_header_to_c (XxY_header *header, FILE *F_XxY /* named output stream */, char *name)
{
    SXINT		kind;
    bool	B [2];

    X_root_header_to_c ((X_root_header*)header, F_XxY, name);

    fprintf (F_XxY, "%s_display, /* display */\n", name);

    for (kind = 0; kind < 2; kind++)
	B [kind] = (header->lnk_hd [kind] != NULL);

    fprintf (F_XxY, "{%s%s%s, ", B [0] ? "&" : "NULL", B [0] ? name : "", B [0] ? "_X_hd_0" : "");
    fprintf (F_XxY, "%s%s%s}, /* X_hd [2] */\n", B [1] ? "&" : "NULL", B [1] ? name : "", B [1] ? "_X_hd_1" : "");

    fprintf (F_XxY, "{%s%s, ", B [0] ? name : "NULL", B [0] ? "_lnk_hd_0" : "");
    fprintf (F_XxY, "%s%s}, /* lnk_hd [2] */\n", B [1] ? name : "NULL", B [1] ? "_lnk_hd_1" : "");

    fprintf (F_XxY, "{%s%s, ", B [0] ? name : "NULL", B [0] ? "_lnk_0" : "");
    fprintf (F_XxY, "%s%s}, /* lnk [2] */\n", B [1] ? name : "NULL", B [1] ? "_lnk_1" : "");

    fprintf (F_XxY, "{true, true} /* X_hd_is_static [2] */\n");

    fprintf (F_XxY, "}");
}

void XxY_to_c (XxY_header *header, FILE *F_XxY /* named output stream */, char *name, bool is_static)
{
    XxY_array_to_c (header, F_XxY, name);
    fprintf (F_XxY, "\n\n%sXxY_header %s =\n", is_static ? "static " : "", name);
    XxY_header_to_c (header, F_XxY, name);
    fprintf (F_XxY, " /* End XxY_header %s */;\n", name);
}



void XxY_reuse (XxY_header *header, char *name, void (*user_oflw) (SXINT, SXINT), FILE *stat_file)
{
    SXINT kind;

    X_root_reuse ((X_root_header*)header, name, XxY_system_oflw,
		  user_oflw, XxY_cmp, XxY_scrmbl, XxY_suppress,
		  XxY_assign, stat_file);

    for (kind = 0; kind < 2; kind++) {
        if (header->lnk [kind] != NULL)
	    X_reuse (header->X_hd [kind], name, XxY_assoc_oflw, stat_file);
    }
}

