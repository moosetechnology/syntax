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
/* 20030512 10:59 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 15-02-94 11:57 (pb):		Ajout de X_cpy et utilisation des	*/
/*				"is_static"				*/
/************************************************************************/
/* 08-02-94 14:20 (pb):		Ajout de X_to_c, X_reuse		*/
/************************************************************************/
/* 07-02-94 16:35 (pb):		Ajout de X_write, X_read		*/
/************************************************************************/
/* 21-01-92 15:50 (pb):		La taille de table de hash est tj un nb	*/
/*				premier, il faut recalculer toutes les	*/
/*				chaines de collision sur debordement et	*/
/*				en particulier pouvoir executer "cmp"	*/
/*				sur 2 elements qcq, d'ou modif de 	*/
/*				"X_scrmbl" qui en plus stocke les val	*/
/************************************************************************/
/* 08-07-91 16:22 (pb):		Utilisation de "X_root_mngr"		*/
/************************************************************************/
/* 13-06-91 10:45 (pb):		Les liens des "trous" ont "X_80" pour	*/
/*				ajout de X_foreach			*/
/*				Ajout de "has_free_buckets" et appel de	*/
/*				"gc" avant le doublement.		*/
/************************************************************************/
/* 13-09-90 10:50 (pb):		Ajout de "_gc", "_lock" et "_unlock" 	*/
/************************************************************************/
/* 04-09-90 10:35 (pb):		Ajout de "X_stat"		 	*/
/************************************************************************/
/* 26-06-90 09:23 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/



#include "sxcommon.h"
#include "X.h"

char WHAT_X_MNGR[] = "@(#)SYNTAX - $Id: X_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

extern void sxtrap (char *caller, char *message);

static int		X_;
static X_header		*HEADER_;


static void X_cpy (old, new, old_size)
    struct X_elem	*old, *new;
    int			old_size;
{
    struct X_elem *old_top = old + old_size;

    do {
	*new++ = *old++;
    } while (old <= old_top);
}

static int X_system_oflw (old_top)
    int old_top;
{
    /* Appele depuis X_root sur debordement (ou lock) de "hash_lnk". */

    if (HEADER_->is_static) {
	/* Premiere allocation dynamique */
	struct X_elem *old_display = HEADER_->display;

	X_cpy (old_display,
	       HEADER_->display = (struct X_elem*) sxalloc (HEADER_->size + 1,
							    sizeof (struct X_elem)),
	       old_top);
    }
    else
	/* On realloue "display". */
	HEADER_->display = (struct X_elem*) sxrealloc (HEADER_->display,
						       HEADER_->size + 1,
						       sizeof (struct X_elem));
    return 0;
}


static int X_cmp (x)
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

    if ((z = HEADER_->display [x].X) >= X_)
	return z == X_ ? 0 : 1;

    return -1;
}


static unsigned int X_scrmbl (x)
    int x;
{
    /* Retourne la valeur du "scramble" de l'element d'indice x. */
    /* Positionne X_, ce qui permet d'appeler "cmp". */
    return X_HASH_FUN (X_ = HEADER_->display [x].X);
}

static int X_assign (x)
    int x;
{
    /* Positionne l'element courant a l'indice x. */
    return HEADER_->display [x].X = X_;
}


void	X_alloc (header, name, init_elem_nb, average_list_nb_per_bucket, user_oflw, stat_file)
    X_header	*header;
    char	*name;
    int		init_elem_nb, average_list_nb_per_bucket;
    int		(*user_oflw) ();
    FILE	*stat_file;
{
    /* Cette fonction "cree" une table d'entiers extensible */
    /* De header "header", de nom "name", dont la taille initiale de la table de hash_code
       est un nombre premier et la longueur moyenne des chaines
       de collision est "average_list_nb_per_bucket".
       La taille de la structure est doublee chaque fois
       que cette longueur moyenne depasse la taille fixee, de plus la fonction utilisateur
       "oflw" est appelee. */
    X_root_alloc (header, name, init_elem_nb, average_list_nb_per_bucket, X_system_oflw,
		  user_oflw, X_cmp, X_scrmbl, NULL, X_assign, stat_file);
    header->display = (struct X_elem*) sxalloc (header->size + 1, sizeof (struct X_elem));
}



void	X_clear (header)
    X_header		*header;
{
    if (header->top != 0)
	X_root_clear (header);
}



void	X_stat (stat_file, header)
    FILE	*stat_file;
    X_header		*header;
{
    if (stat_file != NULL)
	X_root_stat (stat_file, header);
}




void	X_free (header)
    X_header		*header;
{
    if (!header->is_static)
    {
	X_root_free (header);
	sxfree (header->display), header->display = NULL;
    }
}



int	X_is_set (header, X)
    register X_header	*header;
    int			X;
{
    /* Si l'element X de header existe, cette fonction
       retourne l'indice de cet element sinon 0. */
    register int z;

    X_ = X;

    if ((z = X_root_is_set (HEADER_ = header, X_HASH_FUN (X_))) <= 0)
	return 0;
	
    return header->hash_lnk [z] & X_80 ? 0 : z;
}



void	X_gc (header)
    X_header		*header;
{
    /* Cette procedure supprime physiquement les elements effaces
       par un "X_erase" et permet la reutilisation des indices
       ainsi recuperes. */
    register int		x, erased_lnk_hd;
    register int		*hash_lnk, *alnk;

    if (header->has_free_buckets != 0 && (erased_lnk_hd = X_root_gc (header)) > 0) {
	hash_lnk = HEADER_->hash_lnk;
	
	/* On concatene erased_lnh_hd a free_buckets. */
	for ((alnk = &(HEADER_->free_buckets)); (x = (*alnk & X_7F)) != 0; alnk = hash_lnk + x);
	
	*alnk = erased_lnk_hd | X_80;
    }
}


BOOLEAN	X_set (header, X, ref)
    X_header	*header;
    int		X, *ref;
{
    /* Si l'element X de header existe, cette fonction
       retourne TRUE et ref designe cet element, sinon elle cree un nouvel
       element, ref le designe et elle retourne FALSE */

    if (header->is_locked)
	sxtrap ("X_set", header->name);

    X_ = X;

    return X_root_set (HEADER_ = header, X_HASH_FUN (X_), ref);
}


void	X_lock (header)
    X_header		*header;
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure X
	  - Interdit l'utilisation ulterieure de X_set
	  - Appelle (si non NULL) la procedure utilisateur oflw
       L'utisateur vient peut etre d'appeler X_gc. */
    if (header->is_locked)
	return;

    X_root_lock (HEADER_ = header);
}

BOOLEAN X_write (header, F_X)
    X_header	*header;
    int		F_X /* file descriptor */;
{
    int	bytes;

#define WRITE(p,l)	if ((bytes = (l)) > 0 && (write (F_X, p, (size_t)bytes) != bytes)) return FALSE

    if (X_root_write (header, F_X)) {
	WRITE (header->display, sizeof (struct X_elem) * (header->top + 1));

	return TRUE;
    }

    return FALSE;
}


BOOLEAN X_read (header, F_X, name, user_oflw, stat_file)
    X_header		*header;
    int			F_X /* file descriptor */;
    char		*name;
    int			(*user_oflw) ();
    FILE		*stat_file;
{
    int	bytes;

#define READ(p,l)	if ((bytes = (l)) > 0 && (read (F_X, p, (size_t)bytes) != bytes)) return FALSE

    if (X_root_read (header, F_X)) {
	header->display = (struct X_elem*) sxalloc (header->size + 1, sizeof (struct X_elem));
	READ (header->display, sizeof (struct X_elem) * (header->top + 1));
	header->name = name;
	header->system_oflw = X_system_oflw;
	header->user_oflw = user_oflw;
	header->cmp = X_cmp;
	header->scrmbl = X_scrmbl;
	header->suppress = NULL;
	header->assign = X_assign;
	header->stat_file = stat_file;

	return TRUE;
    }

    return FALSE;
}

void X_array_to_c (header, F_X, name)
    X_header		*header;
    FILE		*F_X /* named output stream */;
    char		*name; 
{
    int i;

    fprintf (F_X, "\n\nstatic struct X_elem %s_display [%i] = {", name, header->size + 1);

    for (i = 0; i <= header->top; i++) {
	if ((i & 7) == 0)
	    fprintf (F_X, "\n/* %i */ ", i);

	fprintf (F_X, "{%i}, ", header->display [i].X);
    } 

    fprintf (F_X, "\n};\n");

    X_root_array_to_c (header, F_X, name); /* hash_display, hash_lnk */
}


void X_header_to_c (header, F_X, name)
    X_header		*header;
    FILE		*F_X /* named output stream */;
    char		*name; 
{
    X_root_header_to_c (header, F_X, name);
    
    fprintf (F_X, "%s_display /* display */\n", name);

    fprintf (F_X, "}");
}


void X_to_c (header, F_X, name, is_static)
    X_header		*header;
    FILE		*F_X /* named output stream */;
    char		*name; 
    BOOLEAN		is_static;
{
    X_array_to_c (header, F_X, name);
    fprintf (F_X, "\n\n%sX_header %s =\n", is_static ? "static " : "", name);
    X_header_to_c (header, F_X, name);
    fprintf (F_X, " /* End X_header %s */;\n", name);
}



void X_reuse (header, name, user_oflw, stat_file)
    X_header	*header;
    char	*name;
    int		(*user_oflw) ();
    FILE	*stat_file;
{
    X_root_reuse (header, name, X_system_oflw,
		  user_oflw, X_cmp, X_scrmbl, NULL,
		  X_assign, stat_file);
}

