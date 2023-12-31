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
/* Ven 15 Dec 2000 13:51 (pb):	Ajout du champ is_allocated et des 	*/
/*				macros correspondantes			*/
/************************************************************************/
/* 15-02-94 11:48 (pb):		Ajout de X_root_int_cpy	et traitement	*/
/*				de "is_static"				*/
/************************************************************************/
/* 08-02-94 14:19 (pb):		Ajout de X_root_to_c, X_root_reuse	*/
/************************************************************************/
/* 07-02-94 16:33 (pb):		Ajout de X_root_write, X_root_read	*/
/************************************************************************/
/* 13-11-92 14:15 (pb):		Bug ds "set" si "has_free_buckets" est	*/
/*				positionne bien qu'il n'y a rien a	*/
/*				recuperer ("gc" retourne 0).		*/
/************************************************************************/
/* 27-05-92 14:45 (pb):		Macro min et max en local		*/
/************************************************************************/
/* 06-02-92 11:20 (pb):		Bug ds "set" si utilisation de 		*/
/*				"suppress"				*/
/************************************************************************/
/* 24-01-92 16:25 (pb):		Bug ds "set" si utilisation de "gc"	*/
/************************************************************************/
/* 22-01-92 11:52 (pb):		Le "scramble" devient un unsigned int	*/
/************************************************************************/
/* 21-01-92 15:40 (pb):		Sur debordement, au lieu de doubler la	*/
/*				taille de la table de hash, on choisit	*/
/*				pour nouvelle taille un nombre premier	*/
/*				de valeur sensiblement double de la	*/
/*				precedente. La mise a jour des chaines	*/
/*				de collision doit etre un peu plus 	*/
/*				longue (il faut retrier tous les 	*/
/*				elements) mais finalement on y gagne	*/
/*				a la fois ds les "is_set" et les "set"	*/
/*					temps	lmin	lmax	lmoy	*/
/*				vieux => 12s	2	147	19	*/
/*				neuf  => 10s	6	34	19	*/
/************************************************************************/
/* 13-06-91 14:10 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/



static char	ME [] = "X_root_mngr";


#include "sxcommon.h"
#include "X_root.h"
#include <unistd.h>

char WHAT_X_ROOT_MNGR[] = "@(#)SYNTAX - $Id: X_root_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

extern int sxnext_prime (int germe);

#define max(x,y)	(((x) > (y)) ? (x) : (y))
#define min(x,y)	(((x) < (y)) ? (x) : (y))

static int	HASH_VAL_;

static void X_root_sat_mess (stat_file, header_name, old_size, new_size)
    FILE	*stat_file;
    char	*header_name;
    int		old_size, new_size;
{
    fprintf (stat_file, "%s: %s of size %d overflowed: reallocation with size %d.\n", ME, header_name, old_size, new_size);
}


void	X_root_int_cpy (old, new, old_size)
    int	*old, *new, old_size;
{
    int *old_top = old + old_size;

    do {
	*new++ = *old++;
    } while (old <= old_top);
}

void	X_root_alloc (header, name, init_elem_nb, average_list_nb_per_bucket, system_oflw,
		      user_oflw, cmp, scrmbl, suppress, assign, stat_file)
    X_root_header	*header;
    char		*name;
    int			init_elem_nb, average_list_nb_per_bucket;
    int			(*system_oflw) ();
    int			(*user_oflw) ();
    int			(*cmp) ();
    unsigned int	(*scrmbl) ();
    int			(*suppress) ();
    int			(*assign) ();
    FILE		*stat_file;
{
    /* Cette fonction alloue les structures communes aux (X | XxY | XxYxZ). */
    /* De header "header", de nom "name", dont la taille initiale de la table de hash_code
       est un nombre premier et la longueur moyenne des chaines de collision est
       "average_list_nb_per_bucket". La taille de la structure est doublee chaque fois
       que cette longueur moyenne depasse la taille fixee, de plus la fonction utilisateur
       "user_oflw" est appelee. */

    header->name = name;
    header->hash_size = sxnext_prime (init_elem_nb / average_list_nb_per_bucket);
    header->hash_display = (int*) sxcalloc ((unsigned long int)(header->hash_size), sizeof (int));
    init_elem_nb = header->hash_size * average_list_nb_per_bucket;
    header->hash_lnk = (int*) sxalloc (init_elem_nb + 1, sizeof (int));
    header->top = 0;
    header->size = init_elem_nb;
    header->is_locked = FALSE;
    header->free_buckets = X_root_80; /* liste des "erased" vide */
    header->has_free_buckets = 0;
    header->system_oflw = system_oflw;
    header->user_oflw = user_oflw;
    header->cmp = cmp;
    header->scrmbl = scrmbl;
    header->suppress = suppress;
    header->assign = assign;
    header->stat_file = stat_file;
    header->is_static = FALSE;
    header->is_allocated = TRUE;
}



void	X_root_clear (header)
    X_root_header		*header;
{
    register int 	*top, *bot;

    top = (bot = header->hash_display) + header->hash_size;
    
    while (--top >= bot)
	*top = 0;
    
    header->top = 0;
    header->free_buckets = X_root_80; /* liste des "erased" vide */
    header->has_free_buckets = 0;
    header->is_locked = FALSE;
}



void	X_root_stat (stat_file, header)
    FILE	*stat_file;
    X_root_header		*header;
{
    register int	x, i, l, n;
    int			minl, maxl, meanl, hash_size, size, top;

    hash_size = header->hash_size;
    size = header->size;

    if ((top = header->top) != 0) {
	n = 0;
	meanl = 0;
	maxl = 0;
	minl = size;

	for (x = hash_size - 1; x >= 0; x--) {
	    if ((i = header->hash_display [x]) != 0) {
		n++;
		l = 0;
		
		do {
		    l++;
		} while ((i = header->hash_lnk [i] & X_root_7F) != 0);
		
		minl = min (minl, l);
		maxl = max (maxl, l);
		meanl += l;
	    }
	}
	
	meanl /= n;
	fprintf (stat_file, "name = %s, hash_size = %d (%d%%), size = %d (%d%%), minl = %d, maxl = %d, meanl = %d\n", header->name, hash_size, (100*n)/hash_size, size, (100*top)/size, minl, maxl, meanl);
    }
    else
	fprintf (stat_file, "name = %s, hash_size = %d, size = %d is empty.\n", header->name, hash_size, size);
}



void	X_root_free (header)
    X_root_header		*header;
{
    if (header->stat_file != NULL)
	X_root_stat (header->stat_file, header);

    sxfree (header->hash_display), header->hash_display = NULL;
    sxfree (header->hash_lnk), header->hash_lnk = NULL;
    header->is_allocated = FALSE;
}



int	X_root_is_set (header, scramble)
    register X_root_header	*header;
    unsigned int		scramble;
{
    /* Si l'element de la matrice creuse header existe, cette fonction
       retourne l'identifiant de cet element.
       Sinon elle retourne l'element precedent :
          0 si c'est ds hash_display
	  negatif si c'est ds hash_lnk. */
    /* Attention, si >0 l'element retourne peut etre "erased". */
    /* utilise la fonction cmp qui retourne :
          -1 si <
	  0  si ==
	  1  si >
    */
    register int	x, z, prev = 0;
    register int	(*cmp)();
    register int	*hash_lnk;

    if ((x = header->hash_display [HASH_VAL_ = scramble % header->hash_size]) != 0) {
	cmp = header->cmp;
	hash_lnk = header->hash_lnk;

	do {
	    if ((z = (*cmp) (x)) < 0)
		break;

	    if (z == 0)
		return x;
	} while ((x = (hash_lnk [prev = x] & X_root_7F)) != 0);
    }

    return -prev;
}



int	X_root_gc (header)
    X_root_header		*header;
{
    /* Cette fonction supprime des hash_lnk les elements effaces par un "erase" et
       les lie ensemble.
       Elle retourne l'indice de la tete du lien. */
    register int		x, hash_value, erased_lnk_hd = 0;
    register int		*hash_display, *hash_lnk;
    register int		*low, *high;

    if (header->has_free_buckets != 0) {
	hash_lnk = header->hash_lnk;
	hash_display = header->hash_display;

	for (hash_value = header->hash_size - 1; hash_value >= 0; hash_value--) {
	    low = hash_display + hash_value;
	    
	    while ((x = *low) != 0) {
		if ((*(high = hash_lnk + x) & X_root_80) == 0)
		    low = high;
		else {
		    *low = *high & X_root_7F;
		    *high = erased_lnk_hd | X_root_80;
		    erased_lnk_hd = x;
		}
	    }
	}
	
	header->has_free_buckets = 0;
    }

    return erased_lnk_hd;
}


BOOLEAN	X_root_set (header, scramble, ref)
    X_root_header	*header;
    unsigned int	scramble;
    int			*ref;
{
    /* Si l'element X[-Y[-Z]] de la matrice creuse header existe, cette fonction
       retourne TRUE et ref designe cet element, sinon elle cree un nouvel
       element, ref le designe et elle retourne FALSE */

    register int	x, z;
    int	                old_hash_size, old_size, *old;
    BOOLEAN		overflow;

    sxinitialise(old_size); /* pour faire taire "gcc -Wuninitialized" */
    if ((x = X_root_is_set (header, scramble)) > 0) {
	register int	*alnk;

	if ((*(alnk = header->hash_lnk + (*ref = x)) & X_root_80) == 0)
	    return TRUE;

	/* erased */
	/* l'element qui vient d'etre reutilise est celui qui a ete supprime precedemment
	   on n'a donc rien a faire pour les "foreach". */
	*alnk &= X_root_7F; /* tant pis pour "has_free_buckets"! */
	return FALSE;
    }

/* Son predecesseur ds le chainage est -x. */

    overflow = FALSE;

/* Nouvel item d'indice z */
    if ((z = (header->free_buckets & X_root_7F)) != 0) {
	/* On utilise des "erase". */
	header->free_buckets = header->hash_lnk [z];
    }
    else if (header->top < header->size)
	z = ++(header->top);
    else if (header->has_free_buckets != 0) {
	/* On va utiliser les elements "erased".
	   Attention, le predecesseur ds le chainage (i.e. -x) peut etre
	   un "erased", si c'est le cas, le "gc" qui suit va le supprimer...
	   Dans ce cas, on recalcule le predecesseur. */
	register BOOLEAN	is_erased;

	is_erased = x < 0 ? (header->hash_lnk [-x] & X_root_80) != 0 : FALSE;

	if ((z = X_root_gc (header)) > 0) {
	    if (header->suppress != NULL) {
		/* il faut supprimer vis a vis des foreach les elements de la liste z. */
		/* Attention, suppress touche a HASH_VAL_ */
		int hv = HASH_VAL_;

		(*(header->suppress)) (z);
		HASH_VAL_ = hv;
	    }

	    header->free_buckets = header->hash_lnk [z];

	    if (is_erased)
		/* On recalcule x */
		x = X_root_is_set (header, scramble); /* negatif ou nul */
	}
	/* else has_free_buckets etait positionne bien qu'il n'y ait rien a recuperer.
	   (Cf. le debut de la fonction). */
    }
    else
	z = 0;

    if (z == 0) {
	/* La longueur moyenne d'une liste depasse la valeur specifiee.
	   Aucun element n'est recuperable.
	   On modifie la taille de la table de hash.
	   On prend un nombre premier a peu pres le double du precedent.
	   On double la taille du tableau contenant les chaines de collision.
	   Le ratio entre les deux reste a peu pres celui specifie par
	     average_list_nb_per_bucket. */

	overflow = TRUE;
	z = ++(header->top);
	header->hash_size = sxnext_prime (old_hash_size = header->hash_size);
	header->size = 2 * (old_size = header->size);

	if (header->is_static) {
	    old = header->hash_display;
	    X_root_int_cpy (old,
			    header->hash_display = (int*) sxalloc (header->hash_size,
								   sizeof (int)),
			    old_hash_size - 1);

	    old = header->hash_lnk;
	    X_root_int_cpy (old,
			    header->hash_lnk = (int*) sxalloc (header->size + 1, sizeof (int)),
			    old_size);
	}
	else {
	    header->hash_display = (int*) sxrealloc (header->hash_display,
						     header->hash_size,
						     sizeof (int));
	    header->hash_lnk = (int*) sxrealloc (header->hash_lnk, header->size + 1, sizeof (int));
	}

	/* Traitement par l'appelant du debordement. */
	(*(header->system_oflw)) (old_size);

	if (header->stat_file != NULL)
	    X_root_sat_mess (header->stat_file, header->name, old_size, header->size);
    }

    {
	register int *adisp;

	if (x == 0) {
	    header->hash_lnk [z] = *(adisp = header->hash_display + HASH_VAL_);
	    *adisp = z;
	}
	else {
	    header->hash_lnk [z] = *(adisp = header->hash_lnk - x) & X_root_7F;
	    *adisp = (*adisp & X_root_80) | z;
	}
    }

    /* met a l'indice z l'element X-[-Y[-Z]] */
    (*(header->assign)) (*ref = z);

    if (overflow) {
	/* Tous les elements sont ranges (meme le dernier). */
	/* On recalcule toutes les chaines de collision. */
	register int		hash_size;
	register int		*cur, *hash_lnk, *hash_display;
	register int		(*cmp)();
	register unsigned int	(*scrmbl)();

	hash_size = header->hash_size;
	hash_display = header->hash_display;
	cur = hash_display + hash_size;

	do {
	    *--cur = 0;
	} while (cur > hash_display);

	hash_lnk = header->hash_lnk;
	cmp = header->cmp;
	scrmbl = header->scrmbl;

	do {
	    if ((hash_lnk [z] & X_root_80) == 0) {
		/* Element n'appartenant pas a la liste libre */
		for (cur = hash_display + ((*scrmbl) (z) % hash_size);
		     (x = *cur) != 0 && (*cmp) (x) > 0;
		     cur = hash_lnk + x);

		hash_lnk [z] = x;
		*cur = z;
	    }
	} while (--z > 0);

	if (header->user_oflw != NULL)
	    (*(header->user_oflw)) (old_size, header->size);

	header->is_static = FALSE;
    }


    return FALSE;
}


void	X_root_lock (header)
    X_root_header	*header;
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure si elle n'est pas statique
	  - Interdit l'utilisation ulterieure de X_root_set
	  - Appelle (si non NULL) la procedure utilisateur user_oflw
       L'utisateur vient peut etre d'appeler X[xY[xZ]]_gc. */

    register int old_top;

    header->is_locked = TRUE;

    if (!header->is_static && header->size > (old_top = header->top)) {
	header->hash_lnk = (int*) sxrealloc (header->hash_lnk,
					     (header->size = old_top) + 1, sizeof (int));

	/* Traitement par l'appelant du "retrecissement". */
	
	(*(header->system_oflw)) (old_top);

	if (header->user_oflw != NULL)
	    (*(header->user_oflw)) (old_top, header->size);
    }
}


BOOLEAN X_root_write (header, F_X_root)
    X_root_header	*header;
    int			F_X_root /* file descriptor */;
{
    int	bytes;

#define WRITE(p,l)	if ((bytes = (l))>0 && (write (F_X_root, p, (size_t)bytes) != bytes)) return FALSE

    WRITE (&(header->hash_size), sizeof (int));
    WRITE (&(header->top), sizeof (int));
    WRITE (&(header->size), sizeof (int));
    WRITE (&(header->free_buckets), sizeof (int));
    WRITE (&(header->has_free_buckets), sizeof (int));
    WRITE (&(header->is_locked), sizeof (BOOLEAN));

    WRITE (header->hash_display, sizeof (int) * header->hash_size);
    WRITE (header->hash_lnk, sizeof (int) * (header->top + 1));

    return TRUE;
}


BOOLEAN X_root_read (header, F_X_root)
    X_root_header	*header;
    int			F_X_root /* file descriptor */;
{
    int	bytes;

#define READ(p,l)	if ((bytes = (l)) > 0 && (read (F_X_root, p, (size_t)bytes) != bytes)) return FALSE

    READ (&(header->hash_size), sizeof (int));
    READ (&(header->top), sizeof (int));
    READ (&(header->size), sizeof (int));
    READ (&(header->free_buckets), sizeof (int));
    READ (&(header->has_free_buckets), sizeof (int));
    READ (&(header->is_locked), sizeof (BOOLEAN));

    header->is_static = FALSE;

    header->hash_display = (int*) sxalloc (header->hash_size, sizeof (int));
    READ (header->hash_display, sizeof (int) * header->hash_size);

    header->hash_lnk = (int*) sxalloc (header->size + 1, sizeof (int));
    READ (header->hash_lnk, sizeof (int) * (header->top + 1));

    return TRUE;
}


void X_root_array_to_c (header, F_X, name)
    X_root_header	*header;
    FILE		*F_X /* named output stream */;
    char		*name; 
{
  int i, top;

  fprintf (F_X, "\n\nstatic int %s_hash_display [%i]", name, header->hash_size);

  top = header->hash_size - 1;

  while (header->hash_display [top] == 0)
    top--;

  if (top >= 0) {
    fputs (" = {", F_X);

    for (i = 0; i <= top; i++) {
      if ((i & 7) == 0)
	fprintf (F_X, "\n/* %i */ ", i);

      fprintf (F_X, "%i, ", header->hash_display [i]);
    }

    fputs ("\n}", F_X);
  }
  else
    fputs (" /* empty */", F_X);

  fprintf (F_X, ";\n\n\nstatic int %s_hash_lnk [%i]", name, header->size + 1);

  top = header->top;

  while (header->hash_lnk [top] == 0)
    top--;

  if (top >= 0) {
    fputs (" = {", F_X);

    for (i = 0; i <= top; i++) {
      if ((i & 7) == 0)
	fprintf (F_X, "\n/* %i */ ", i);

      fprintf (F_X, "%i, ", header->hash_lnk [i]);
    } 

    fputs ("\n}", F_X);
  }
  else
    fputs (" /* empty */", F_X);

  fputs (";\n", F_X);
}


void X_root_header_to_c (header, F_X, name)
    X_root_header	*header;
    FILE		*F_X /* named output stream */;
    char		*name; 
{
    fprintf (F_X, "{\"%s\", /* name */\n", name);
    fprintf (F_X, "%s_hash_display, /* hash_display */\n", name);
    fprintf (F_X, "%s_hash_lnk, /* hash_lnk */\n", name);
    fprintf (F_X, "%i, /* hash_size */\n", header->hash_size);
    fprintf (F_X, "%i, /* top */\n", header->top);
    fprintf (F_X, "%i, /* size */\n", header->size);

    if ((unsigned int)(header->free_buckets) == X_root_80) /* Le 10/03/2004 warning a la compil avec -2147483648 */
      fputs ("X_root_80, /* free_buckets */\n", F_X);
    else
      fprintf (F_X, "%i, /* free_buckets */\n", header->free_buckets);
    
    fprintf (F_X, "%i, /* has_free_buckets */\n", header->has_free_buckets);
    fputs ("NULL, /* (*system_oflw) () */\n", F_X);
    fputs ("NULL, /* (*user_oflw) () */\n", F_X);
    fputs ("NULL, /* (*cmp) () */\n", F_X);
    fputs ("NULL, /* (*scrmbl) () */\n", F_X);
    fputs ("NULL, /* (*suppress) () */\n", F_X);
    fputs ("NULL, /* (*assign) () */\n", F_X);
    fputs ("NULL, /* stat_file */\n", F_X);
    fprintf (F_X, "%i, /* is_locked */\n", header->is_locked ? 1 : 0);
    fputs ("TRUE, /* is_static */\n", F_X);
    fputs ("TRUE, /* is_allocated */\n", F_X);
}


void X_root_reuse (header, name, system_oflw, user_oflw, cmp, scrmbl, suppress, assign, stat_file)
    X_root_header	*header;
    char		*name;
    int			(*system_oflw) ();
    int			(*user_oflw) ();
    int			(*cmp) ();
    unsigned int	(*scrmbl) ();
    int			(*suppress) ();
    int			(*assign) ();
    FILE		*stat_file;
{
    header->name = name;
    header->system_oflw = system_oflw;
    header->user_oflw = user_oflw;
    header->cmp = cmp;
    header->scrmbl = scrmbl;
    header->suppress = suppress;
    header->assign = assign;
    header->stat_file = stat_file;
}


