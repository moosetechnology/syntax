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
/* 06-10-94 10:02 (pb):		Bug dans XxY_stat			*/
/************************************************************************/
/* 30-03-94 10:03 (pb):		Remplacement de strncpy par memcpy	*/
/************************************************************************/
/* 15-02-94 11:57 (pb):		Ajout de XxY_cpy et utilisation des	*/
/*				"is_static"				*/
/************************************************************************/
/* 08-02-94 14:22 (pb):		Ajout de XxY_to_c, XxY_reuse		*/
/************************************************************************/
/* 07-02-94 16:37 (pb):		Ajout de XxY_write, XxY_read		*/
/************************************************************************/
/* 15-04-93 13:30 (pb):		Modif de la table de sxnext_prime ()	*/
/*				65537 = 2**16 + 1 est premier. Si le	*/
/*				scramble est calcule' par x ^ (y << 16)	*/
/*				x et y < 2**16 et si y = x + 1, on a	*/
/*				x+2**16(x+1)=x(1+2**16)+2**16		*/
/*				=> tous les couples sont en collision	*/
/*				sur 2**16. On a pris 65539.		*/
/************************************************************************/
/* 04-12-92 11:10 (pb):		Bug ds XxY_to_SXBM sur les "is_erased"	*/
/************************************************************************/
/* 07-10-92 14:45 (pb):		Suppression des macros min et max	*/
/************************************************************************/
/* 06-02-92 11:40 (pb):		Modification de la taille des X_header	*/
/*				s'il existe des foreach			*/
/************************************************************************/
/* 04-02-92 10:50 (pb):		Modification de XxYxZ_HASH_FUN		*/
/************************************************************************/
/* 21-01-92 15:50 (pb):		La taille de table de hash est tj un nb	*/
/*				premier, il faut recalculer toutes les	*/
/*				chaines de collision sur debordement et	*/
/*				en particulier pouvoir executer "cmp"	*/
/*				sur 2 elements qcq, d'ou modif de 	*/
/*				"XxY_scrmbl" qui en plus stocke les val	*/
/************************************************************************/
/* 16-01-92 16:04 (pb):		Test de l'utilisation du "scramble"	*/
/*				pour ordonner les listes de collision	*/
/*				(le scramble est recalcule' sur chaque	*/
/*				element visite' et compare' au courant)	*/
/*				=> nette de'terioration des temps...	*/
/************************************************************************/
/* 08-07-91 16:22 (pb):		Utilisation de "X_root_mngr"		*/
/************************************************************************/
/* 13-06-91 10:20 (pb):		Les liens des "trous" ont "X_80" pour	*/
/*				ajout de XxY_foreach			*/
/*				Ajout de "has_free_buckets" et appel de	*/
/*				"gc" avant le doublement.		*/
/************************************************************************/
/* 12-06-91 11:55 (pb):		Bug dans la recuperation des "erase"	*/
/*				sur oflw. 				*/
/************************************************************************/
/* 12-09-90 16:50 (pb):		Ajout de "_gc", "_lock" et "_unlock" 	*/
/************************************************************************/
/* 12-09-90 14:20 (pb):		Recuperation des "erase" sur oflw. 	*/
/************************************************************************/
/* 04-09-90 09:41 (pb):		Ajout de "XxY_stat"		 	*/
/************************************************************************/
/* 26-06-90 10:53 (pb):		Ajout de "XxY_closure"		 	*/
/************************************************************************/
/* 19-06-90 11:56 (pb):		Ajout de "XxY_set_and_close" qui 	*/
/*				construit au fur et a mesure la		*/
/*				fermeture de la relation		*/
/************************************************************************/
/* 10-01-90 09:10 (pb):		Structure "associative"			*/
/************************************************************************/
/* 10-05-89 09:52 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/



static char	ME [] = "XxY_mngr";


#include "sxcommon.h"
#include "XxY.h"
#include <memory.h>

char WHAT_XXY_MNGR[] = "@(#)SYNTAX - $Id: XxY_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

extern void sxtrap (char *caller, char *message);
extern int  sxlast_bit (int nb);

static int		X_, Y_;
static XxY_header	*HEADER_;
static int		KIND_;


static void XxY_cpy (old, new, old_size)
    struct XxY_elem	*old, *new;
    int			old_size;
{
    struct XxY_elem *old_top = old + old_size;

    do {
	*new++ = *old++;
    } while (old <= old_top);
}

static void XxY_assoc_oflw (old_size, new_size)
    int		old_size, new_size;
{
    /* La variable statique KIND_ est positionnee. */
    int	*old = HEADER_->lnk_hd [KIND_];

    if (HEADER_->X_hd_is_static [KIND_]) {
	X_root_int_cpy (old,
			HEADER_->lnk_hd [KIND_] = (int*) sxalloc (new_size + 1, sizeof (int)),
			old_size);
    }
    else
	HEADER_->lnk_hd [KIND_] = (int*) sxrealloc (old, new_size + 1, sizeof (int));
}



static int XxY_system_oflw (old_top)
    int old_top;
{
    /* Appele depuis X_root sur debordement (ou lock) de "hash_lnk". */
    /* Suivant que la structure est statique ou non, On alloue ou realloue
       "display" et les "lnk" des "foreach" (s'il y en a) */
    int kind, *old_lnk;

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


static int XxY_cmp (x)
    int x;
{
    /* Appele depuis X_root_is_set.
       "Compare" l'element courant se trouvant a l'indice x avec l'element que l'on
       recherche. Cette fonction retourne
          -1 si <
	  0  si ==
	  1  si >
    */
    register int		z;
    register struct XxY_elem	*aelem = HEADER_->display + x;

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


static unsigned int XxY_scrmbl (x)
    int x;
{
    /* Retourne la valeur du "scramble" de l'element d'indice x. */
    /* Positionne X_ et Y_, ce qui permet d'appeler "cmp". */
    register struct XxY_elem	*aelem = HEADER_->display + x;

    X_ = aelem->X, Y_ = aelem->Y;
    return XxY_HASH_FUN (X_, Y_);
}

static int XxY_assign (x)
    int x;
{
    /* Positionne le doublet courant a l'indice x et traite les "foreach". */
    register struct XxY_elem	*aelem = HEADER_->display + x;
    register int		kind;
    register int		*lnk;
    int				head;
    
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


static int XY_orig (x, kind)
    register int		x, kind;
{
    register struct XxY_elem	*aelem = HEADER_->display + x;
    register int		indice;

    if ((indice = X_is_set (HEADER_->X_hd [kind], kind == XY_X ? aelem->X : aelem->Y)) == 0)
	sxtrap (ME, "XY_orig");

    return indice;
}


static int  XxY_suppress (erased_lnk_hd)
    int			erased_lnk_hd;
{
    /* Supprime, vis-a-vis des "foreach" les elements de la liste erased_lnk_hd. */
    register int		*low, *lnk;
    register int		x, y, orig;
    register int		*hash_lnk = HEADER_->hash_lnk;

    if ((lnk = HEADER_->lnk [XY_X]) != NULL) {
	x = erased_lnk_hd;

	do {
	    low = HEADER_->lnk_hd [XY_X] + (orig = XY_orig (x, XY_X));

	    while ((y = *low) != x)
		low = lnk + y;

	    if ((*low = lnk [x]) == 0 && low == HEADER_->lnk_hd [XY_X] + orig)
		X_erase (*(HEADER_->X_hd [XY_X]), orig);

	} while ((x = (hash_lnk [x] & XxY_7F)) != 0);
    }

    if ((lnk = HEADER_->lnk [XY_Y]) != NULL) {
	x = erased_lnk_hd;

	do {
	    low = HEADER_->lnk_hd [XY_Y] + (orig = XY_orig (x, XY_Y));

	    while ((y = *low) != x)
		low = lnk + y;

	    if ((*low = lnk [x]) == 0 && low == HEADER_->lnk_hd [XY_Y] + orig)
		X_erase (*(HEADER_->X_hd [XY_Y]), orig);

	} while ((x = (hash_lnk [x] & XxY_7F)) != 0);
    }

    return 0;
}

void	XxY_alloc (header, name, init_elem_nb, average_list_nb_per_bucket,
		     average_X_assoc, average_Y_assoc, user_oflw, stat_file)
    XxY_header	*header;
    char	*name;
    int		init_elem_nb, average_list_nb_per_bucket;
    int		average_X_assoc, average_Y_assoc;
    int		(*user_oflw) ();
    FILE	*stat_file;
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
    register int	lgth, has_foreach;

    sxinitialise(lgth); /* pour faire taire "gcc -Wuninitialized" */
    has_foreach = average_X_assoc + average_Y_assoc;

    X_root_alloc (header, name, init_elem_nb, average_list_nb_per_bucket, XxY_system_oflw,
		  user_oflw, XxY_cmp, XxY_scrmbl, has_foreach ? XxY_suppress : NULL,
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
	header->lnk_hd [XY_X] = (int*) sxalloc (X_size (*(header->X_hd [XY_X])) + 1, sizeof (int));
	header->lnk_hd [XY_X][0] = 0 /* Permet, si liste vide, de ne pas executer un "foreach". */;
	header->lnk [XY_X] = (int*) sxalloc (header->size + 1, sizeof (int));
	header->lnk [XY_X][0] = 0 /* Permet de quitter un "foreach" en positionnant la variable a 0 */;
    }
    else {
	header->X_hd [XY_X] = NULL;
	header->lnk_hd [XY_X] = header->lnk [XY_X] = NULL;
	header->X_hd_is_static [XY_X] = FALSE;
    }

    if (average_Y_assoc != 0) {
	header->X_hd [XY_Y] = (X_header*) sxalloc (1, sizeof (X_header));
	X_alloc (header->X_hd [XY_Y], name, lgth, average_Y_assoc, XxY_assoc_oflw, stat_file);
	header->lnk_hd [XY_Y] = (int*) sxalloc (X_size (*(header->X_hd [XY_Y])) + 1, sizeof (int));
	header->lnk_hd [XY_Y][0] = 0 /* Permet, si liste vide, de ne pas executer un "foreach". */;
	header->lnk [XY_Y] = (int*) sxalloc (header->size + 1, sizeof (int));
	header->lnk [XY_Y][0] = 0 /* Permet de quitter un "foreach" en positionnant la variable a 0 */;
    }
    else {
	header->X_hd [XY_Y] = NULL;
	header->lnk_hd [XY_Y] = header->lnk [XY_Y] = NULL;
	header->X_hd_is_static [XY_Y] = FALSE;
    }
}



void	XxY_clear (header)
    XxY_header		*header;
{
    register X_header	*Xhd;
    register int 	kind;

    if (header->top != 0) {
	X_root_clear (header);

	for (kind = 0; kind < 2; kind++) {
	    if ((Xhd = header->X_hd [kind]) != NULL)
		X_clear (Xhd);
	}
    }
}



void	XxY_stat (stat_file, header)
    FILE	*stat_file;
    XxY_header		*header;
{
    register int	x, i, l, n;
    register X_header	*Xhd;
    register int	*lnk_hd, *lnk;
    int			minl, maxl, meanl, kind;

    if (stat_file != NULL) {
	X_root_stat (stat_file, header);

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
		    fprintf (stat_file, "X_minl = %d, X_maxl = %d, X_meanl = %d\n", minl, maxl, meanl);
		}
	    }
	}
    }
}




void	XxY_free (header)
    XxY_header		*header;
{
    register int	kind;
    register X_header	*Xhd;

    if (!header->is_static)
    {
	X_root_free (header);
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



int	XxY_is_set (header, X, Y)
    register XxY_header	*header;
    int			X, Y;
{
    /* Si l'element X-Y de la matrice creuse header existe, cette fonction
       retourne l'identifiant de cet element, sinon elle retourne 0 */

    register int z;

    X_ = X, Y_ = Y;

    if ((z = X_root_is_set (HEADER_ = header, XxY_HASH_FUN (X_, Y_))) <= 0)
	return 0;
	
    return header->hash_lnk [z] & XxY_80 ? 0 : z;
}



void	XxY_gc (header)
    XxY_header		*header;
{
    /* Cette procedure supprime physiquement les elements effaces
       par un "XxY_erase" et permet la reutilisation des indices
       ainsi recuperes. */
    register int		x, erased_lnk_hd;
    register int		*hash_lnk, *alnk;

    if (header->has_free_buckets != 0 && (erased_lnk_hd = X_root_gc (header)) > 0) {
	/* Traitement des "foreach" des elements supprimes. */
	if ((HEADER_ = header)->suppress != NULL) /* Il y a des foreach. */
	    XxY_suppress (erased_lnk_hd);

	hash_lnk = HEADER_->hash_lnk;
	
	/* On concatene erased_lnh_hd a free_buckets. */
	for ((alnk = &(HEADER_->free_buckets)); (x = (*alnk & XxY_7F)) != 0; alnk = hash_lnk + x);
	
	*alnk = erased_lnk_hd | XxY_80;
    }
}


BOOLEAN	XxY_set (header, X, Y, ref)
    XxY_header	*header;
    int		X, Y, *ref;
{
    /* Si l'element X-Y de la matrice creuse header existe, cette fonction
       retourne TRUE et ref designe cet element, sinon elle cree un nouvel
       element, ref le designe et elle retourne FALSE */

    if (header->is_locked)
	sxtrap ("XxY_set", header->name);

    X_ = X, Y_ = Y;

    return X_root_set (HEADER_ = header, XxY_HASH_FUN (X_, Y_), ref);
}


void	XxY_lock (header)
    XxY_header		*header;
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure XxY
	  - Interdit l'utilisation ulterieure de XxY_set
	  - Appelle (si non NULL) la procedure utilisateur oflw
       L'utisateur vient peut etre d'appeler XxY_gc. */
    register int kind;

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

    X_root_lock (header);
}


void	XxY_unlock (header)
    XxY_header		*header;
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure XxY
	  - Interdit l'utilisation ulterieure de XxY_set
	  - Appelle (si non NULL) la procedure utilisateur oflw
       L'utisateur vient peut etre d'appeler XxY_gc. */
    register int kind;

    if (!header->is_locked)
	return;

    if (header->suppress != NULL)
	for (kind = 0; kind < 2;kind++) {
	    if (header->lnk [kind] != NULL) {
		X_unlock (*(header->X_hd [kind]));
	    }
	}

    header->is_locked = FALSE;
}


BOOLEAN	XxY_set_and_close (header, X, Y, ref)
    register XxY_header		*header;
    register int		X, Y, *ref;
{
    /* Si R est une relation binaire, header contient la fermeture transitive
       R+ de R.
       Complete, si necessaire, R+ par ajout de l'element (X, Y).
       Ref designe ce (nouvel) element.
       Retourne TRUE si ref existait deja, FALSE sinon. */
    register int	x, y, X1;

    if ((*ref = XxY_is_set (header, X, Y)))
	return TRUE;

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


void	XxY_to_SXBM (header, get_couple, couple_nb, index_hd, M)
    register XxY_header	*header;
    BOOLEAN	(*get_couple) ();
    int		couple_nb;
    register X_header	*index_hd;
    SXBA	** M;
{
    /* Si header est non NULL, il contient une relation binaire R.
       Sinon, on peut obtenir successivement les couples de cette relation
       par get_couple.
       couple_nb est le nombre de couples.
       Cette procedure stocke "au minimum" cette relation dans la matrice
       de bits carree M.
       index_hd est un X_header qui assure la correspondance entre
       les constituants des elements de R et les indices de M. */
    register int	x, n, old_n;
    int			X, Y, ind1, ind2;
    register SXBA	*bm;
    BOOLEAN 		OK;

    n = 2 * couple_nb + 1;

    if (*M == NULL) {
	X_alloc (index_hd, "index_hd", n, 2, NULL, NULL);
	bm = *M = sxbm_calloc (n, n);
    }
    else {
	X_clear (index_hd);
	/* On suppose que M est une matrice carree */
	bm = *M;
	old_n = BASIZE (bm [0]);
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
	    OK = TRUE;
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


BOOLEAN XxY_write (header, F_XxY)
    XxY_header	*header;
    int		F_XxY /* file descriptor */;
{
    int		bytes, kind;
    X_header	*X_hd;
    int		*lnk_hd, *lnk;
    BOOLEAN 	is_X_hd;

#define WRITE(p,l)	if ((bytes = (l)) > 0 && (write (F_XxY, p, (size_t)bytes) != bytes)) return FALSE

    if (!X_root_write (header, F_XxY))
	return FALSE;

    WRITE (header->display, sizeof (struct XxY_elem) * (header->top + 1));

    for (kind = 0; kind < 2; kind++) {
	is_X_hd = (X_hd = header->X_hd [kind]) != NULL;

	WRITE (&(is_X_hd), sizeof (BOOLEAN));

	if (is_X_hd) {
	    if (!X_write (X_hd, F_XxY))
		return FALSE;

	    lnk_hd = header->lnk_hd [kind];
	    lnk = header->lnk [kind];

	    WRITE (lnk_hd, sizeof (int) * (X_hd->top + 1));
	    WRITE (lnk, sizeof (int) * (header->top + 1));
	}
    }

    return TRUE;
}


BOOLEAN XxY_read (header, F_XxY, name, user_oflw, stat_file)
    XxY_header		*header;
    int			F_XxY /* file descriptor */;
    char		*name;
    int			(*user_oflw) ();
    FILE		*stat_file;
{
    int		bytes, kind;
    X_header	*X_hd;
    int		*lnk_hd, *lnk;
    BOOLEAN	is_X_hd	;

#define READ(p,l)	if ((bytes=(l))>0&&((read (F_XxY, p, (size_t)bytes)) != bytes)) return FALSE

    if (X_root_read (header, F_XxY)) {
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
	    READ (&(is_X_hd), sizeof (BOOLEAN));

	    if (is_X_hd) {
		X_hd = header->X_hd [kind] = (X_header*) sxalloc (1, sizeof (X_header));

		if (!X_read (X_hd, F_XxY, name, XxY_assoc_oflw, stat_file))
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

	return TRUE;
    }

    return FALSE;
}


void XxY_array_to_c (header, F_XxY, name)
    XxY_header		*header;
    FILE		*F_XxY /* named output stream */;
    char		*name; 
{
    int		i, kind;
    char	sub_name [128];
    X_header	*X_hd;

    fprintf (F_XxY, "\n\nstatic struct XxY_elem %s_display [%i] = {", name, header->size + 1);

    for (i = 0; i <= header->top; i++) {
	if ((i & 3) == 0)
	    fprintf (F_XxY, "\n/* %i */ ", i);

	fprintf (F_XxY, "{%i, %i}, ", header->display [i].X, header->display [i].Y);
    } 

    fprintf (F_XxY, "\n};\n");

    for (kind = 0; kind < 2; kind++) {
	if ((X_hd = header->X_hd [kind]) != NULL) {
	    if ((i = strlen (name)) > 120)
	        i = 120;

	    memcpy (sub_name, name, (size_t)i);
	    sprintf (sub_name + i, "_X_hd_%i", kind);
	    X_to_c (X_hd, F_XxY, sub_name, TRUE /* is_static */);

	    /* lnk_hd [kind] */
	    fprintf (F_XxY, "\n\nstatic int %s_lnk_hd_%i [%i] = {", name, kind, X_hd->size + 1);

	    for (i = 0; i <= X_hd->top; i++) {
		if ((i & 7) == 0)
		    fprintf (F_XxY, "\n/* %i */ ", i);

		fprintf (F_XxY, "%i, ", header->lnk_hd [kind] [i]);
	    } 

	    fprintf (F_XxY, "\n};\n");

	    /* lnk [kind] */
	    fprintf (F_XxY, "\n\nstatic int %s_lnk_%i [%i] = {", name, kind, header->size + 1);

	    for (i = 0; i <= header->top; i++) {
		if ((i & 7) == 0)
		    fprintf (F_XxY, "\n/* %i */ ", i);

		fprintf (F_XxY, "%i, ", header->lnk [kind] [i]);
	    } 

	    fprintf (F_XxY, "\n};\n");
	}
    }

    X_root_array_to_c (header, F_XxY, name); /* hash_display, hash_lnk */
}

void XxY_header_to_c (header, F_XxY, name)
    XxY_header		*header;
    FILE		*F_XxY /* named output stream */;
    char		*name; 
{
    int		kind;
    BOOLEAN	B [2];

    X_root_header_to_c (header, F_XxY, name);

    fprintf (F_XxY, "%s_display, /* display */\n", name);

    for (kind = 0; kind < 2; kind++)
	B [kind] = (header->lnk_hd [kind] != NULL);

    fprintf (F_XxY, "{%s%s%s, ", B [0] ? "&" : "NULL", B [0] ? name : "", B [0] ? "_X_hd_0" : "");
    fprintf (F_XxY, "%s%s%s}, /* X_hd [2] */\n", B [1] ? "&" : "NULL", B [1] ? name : "", B [1] ? "_X_hd_1" : "");

    fprintf (F_XxY, "{%s%s, ", B [0] ? name : "NULL", B [0] ? "_lnk_hd_0" : "");
    fprintf (F_XxY, "%s%s}, /* lnk_hd [2] */\n", B [1] ? name : "NULL", B [1] ? "_lnk_hd_1" : "");

    fprintf (F_XxY, "{%s%s, ", B [0] ? name : "NULL", B [0] ? "_lnk_0" : "");
    fprintf (F_XxY, "%s%s}, /* lnk [2] */\n", B [1] ? name : "NULL", B [1] ? "_lnk_1" : "");

    fprintf (F_XxY, "{TRUE, TRUE} /* X_hd_is_static [2] */\n");

    fprintf (F_XxY, "}");
}

void XxY_to_c (header, F_XxY, name, is_static)
    XxY_header		*header;
    FILE		*F_XxY /* named output stream */;
    char		*name; 
    BOOLEAN		is_static;
{
    XxY_array_to_c (header, F_XxY, name);
    fprintf (F_XxY, "\n\n%sXxY_header %s =\n", is_static ? "static " : "", name);
    XxY_header_to_c (header, F_XxY, name);
    fprintf (F_XxY, " /* End XxY_header %s */;\n", name);
}



void XxY_reuse (header, name, user_oflw, stat_file)
    XxY_header	*header;
    char	*name;
    int		(*user_oflw) ();
    FILE	*stat_file;
{
    int kind;

    X_root_reuse (header, name, XxY_system_oflw,
		  user_oflw, XxY_cmp, XxY_scrmbl, XxY_suppress,
		  XxY_assign, stat_file);

    for (kind = 0; kind < 2; kind++) {
        if (header->lnk [kind] != NULL)
	    X_reuse (header->X_hd [kind], name, XxY_assoc_oflw, stat_file);
    }
}

