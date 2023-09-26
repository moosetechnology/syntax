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
/* 06-10-94 10:08 (pb):		Bug dans XxY_stat			*/
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


static char	what [] = "@(#)XxY_mngr.c	- SYNTAX [unix] - Jeu 6 Oct 1994 10:08:38";
static char	ME [] = "XxY_mngr";


#include "XxY.h"

static int		X_, Y_;
static XxY_header	*HEADER_;
static int		**oflw_ptr;


static void XxY_assoc_oflw (old_size, new_size)
    int		old_size, new_size;
{
    *oflw_ptr = (int*) sxrealloc (*oflw_ptr, new_size + 1, sizeof (int));
}



static void XxY_system_oflw (old_size)
    int old_size;
{
    /* Appele depuis X_root sur debordement de "hash_lnk". */
    /* On realloue "display" et les "lnk" des "foreach" (s'il y en a) */
    register int kind;

    HEADER_->display = (struct XxY_elem*) sxrealloc (HEADER_->display, HEADER_->size + 1, sizeof (struct XxY_elem));
    
    if (HEADER_->suppress != NULL)
	for (kind = 0; kind < 2;kind++) {
	    if (HEADER_->lnk [kind] != NULL)
		HEADER_->lnk [kind] = (int*) sxrealloc (HEADER_->lnk [kind], HEADER_->size + 1, sizeof (int));
	}
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

static void XxY_assign (x)
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
		/* positionnement de la variable statique oflw_ptr si X_hd deborde */
		oflw_ptr = &(HEADER_->lnk_hd [kind]);
		
		if (X_set (HEADER_->X_hd [kind], kind == XY_X ? X_ : Y_, &head))
		    /* Ce n'est pas le premier X */
		    lnk [x] = HEADER_->lnk_hd [kind][head];
		else
		    lnk [x] = 0;
		
		HEADER_->lnk_hd [kind][head] = x;
	    }
	}
    }
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


static void  XxY_suppress (erased_lnk_hd)
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
    register int	kind, lgth, size, has_foreach;

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
	X_alloc (header->X_hd [XY_X], name, lgth, average_X_assoc, XxY_assoc_oflw, NULL);
	header->lnk_hd [XY_X] = (int*) sxalloc (X_size (*(header->X_hd [XY_X])) + 1, sizeof (int));
	header->lnk_hd [XY_X][0] = 0 /* Permet, si liste vide, de ne pas executer un "foreach". */;
	header->lnk [XY_X] = (int*) sxalloc (header->size + 1, sizeof (int));
	header->lnk [XY_X][0] = 0 /* Permet de quitter un "foreach" en positionnant la variable a 0 */;
    }
    else {
	header->X_hd [XY_X] = NULL;
	header->lnk_hd [XY_X] = header->lnk [XY_X] = NULL;
    }

    if (average_Y_assoc != 0) {
	header->X_hd [XY_Y] = (X_header*) sxalloc (1, sizeof (X_header));
	X_alloc (header->X_hd [XY_Y], name, lgth, average_Y_assoc, XxY_assoc_oflw, NULL);
	header->lnk_hd [XY_Y] = (int*) sxalloc (X_size (*(header->X_hd [XY_Y])) + 1, sizeof (int));
	header->lnk_hd [XY_Y][0] = 0 /* Permet, si liste vide, de ne pas executer un "foreach". */;
	header->lnk [XY_Y] = (int*) sxalloc (header->size + 1, sizeof (int));
	header->lnk [XY_Y][0] = 0 /* Permet de quitter un "foreach" en positionnant la variable a 0 */;
    }
    else {
	header->X_hd [XY_Y] = NULL;
	header->lnk_hd [XY_Y] = header->lnk [XY_Y] = NULL;
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
    register XxY_header	*XYhd;
    register int	*lnk_hd, *lnk;
    int			minl, maxl, meanl, top, kind;

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

    X_root_free (header);
    sxfree (header->display), header->display = NULL;

    for (kind = 0; kind < 2; kind++) {
	if ((Xhd = header->X_hd [kind]) != NULL) {
	    X_free (Xhd);
	    sxfree (header->lnk_hd [kind]), header->lnk_hd [kind] = NULL;
	    sxfree (header->lnk [kind]), header->lnk [kind] = NULL;
	    sxfree (Xhd), header->X_hd [kind] = NULL;
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

    if (header->suppress != NULL)
	for (kind = 0; kind < 2;kind++) {
	    if (header->lnk [kind] != NULL) {
		/* positionnement de la variable statique oflw_ptr pour la reallocation de lnk_hd */
		oflw_ptr = &(header->lnk_hd [kind]);
		X_lock (header->X_hd [kind]);
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

    if (*ref = XxY_is_set (header, X, Y))
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
	else if (OK = !XxY_is_erased (*header, x)) {
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

