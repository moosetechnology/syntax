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

static char	ME [] = "X_root_mngr";

#include "sxversion.h"
#include "sxcommon.h"
#include "X_root.h"
#include <unistd.h>

char WHAT_X_ROOT_MNGR[] = "@(#)SYNTAX - $Id: X_root_mngr.c 2947 2023-03-29 17:06:41Z garavel $" WHAT_DEBUG;

extern SXINT sxnext_prime (SXINT germe);

#define max(x,y)	(((x) > (y)) ? (x) : (y))
#define min(x,y)	(((x) < (y)) ? (x) : (y))

static SXUINT	HASH_VAL_;

static void X_root_sat_mess (FILE *stat_file, char *header_name, SXINT old_size, SXINT new_size)
{
    fprintf (stat_file, "%s: %s of size %ld overflowed: reallocation with size %ld.\n", ME, header_name, (SXINT) old_size, (SXINT) new_size);
}


void	X_root_int_cpy (SXINT *old, SXINT *new, SXINT old_size)
{
    SXINT *old_top = old + old_size;

    do {
	*new++ = *old++;
    } while (old <= old_top);
}


void	X_root_alloc (X_root_header *header,
		      char *name, 
		      SXINT init_elem_nb, 
		      SXINT average_list_nb_per_bucket, 
		      void (*system_oflw) (SXINT), 
		      void (*user_oflw) (SXINT, SXINT),
		      SXINT (*cmp) (SXINT), 
		      SXUINT (*scrmbl) (SXINT), 
		      SXINT (*suppress) (SXINT), 
		      SXINT (*assign) (SXINT), 
		      FILE *stat_file)
{
    /* Cette fonction alloue les structures communes aux (X | XxY | XxYxZ). */
    /* De header "header", de nom "name", dont la taille initiale de la table de hash_code
       est un nombre premier et la longueur moyenne des chaines de collision est
       "average_list_nb_per_bucket". La taille de la structure est doublee chaque fois
       que cette longueur moyenne depasse la taille fixee, de plus la fonction utilisateur
       "user_oflw" est appelee. */

    header->name = name;
    header->hash_size = sxnext_prime (init_elem_nb / average_list_nb_per_bucket);
    header->hash_display = (SXINT*) sxcalloc ((SXUINT)(header->hash_size), sizeof (SXINT));
    init_elem_nb = header->hash_size * average_list_nb_per_bucket;
    header->hash_lnk = (SXINT*) sxalloc (init_elem_nb + 1, sizeof (SXINT));
    header->top = 0;
    header->size = init_elem_nb;
    header->is_locked = SXFALSE;
    header->free_buckets = X_root_80; /* liste des "erased" vide */
    header->has_free_buckets = 0;
    header->system_oflw = system_oflw;
    header->user_oflw = user_oflw;
    header->cmp = cmp;
    header->scrmbl = scrmbl;
    header->suppress = suppress;
    header->assign = assign;
    header->stat_file = stat_file;
    header->is_static = SXFALSE;
    header->is_allocated = SXTRUE;
}



void	X_root_clear (X_root_header *header)
{
    SXINT 	*top, *bot;

    top = (bot = header->hash_display) + header->hash_size;
    
    while (--top >= bot)
	*top = 0;
    
    header->top = 0;
    header->free_buckets = X_root_80; /* liste des "erased" vide */
    header->has_free_buckets = 0;
    header->is_locked = SXFALSE;
}



void	X_root_stat (FILE *stat_file, X_root_header *header)
{
    SXINT	x, i, l, n;
    SXINT			minl, maxl, meanl, hash_size, size, top;

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
	fprintf (stat_file, "name = %s, hash_size = %ld (%ld%%), size = %ld (%ld%%), minl = %ld, maxl = %ld, meanl = %ld\n", header->name, (SXINT) hash_size, (SXINT) (100*n)/hash_size, (SXINT) size, (SXINT) (100*top)/size, (SXINT) minl, (SXINT) maxl, (SXINT) meanl);
    }
    else
	fprintf (stat_file, "name = %s, hash_size = %ld, size = %ld is empty.\n", header->name, (SXINT) hash_size, (SXINT) size);
}



void	X_root_free (X_root_header *header)
{
    if (header->stat_file != NULL)
	X_root_stat (header->stat_file, header);

    sxfree (header->hash_display), header->hash_display = NULL;
    sxfree (header->hash_lnk), header->hash_lnk = NULL;
    header->is_allocated = SXFALSE;
}



SXINT	X_root_is_set (X_root_header *header, SXUINT scramble)
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
    SXINT	x, z, prev = 0;
    SXINT	(*cmp)(SXINT);
    SXINT	*hash_lnk;

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



SXINT	X_root_gc (X_root_header *header)
{
    /* Cette fonction supprime des hash_lnk les elements effaces par un "erase" et
       les lie ensemble.
       Elle retourne l'indice de la tete du lien. */
    SXINT		x, hash_value, erased_lnk_hd = 0;
    SXINT		*hash_display, *hash_lnk;
    SXINT		*low, *high;

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


SXBOOLEAN	X_root_set (X_root_header *header, SXUINT scramble, SXINT *ref)
{
    /* Si l'element X[-Y[-Z]] de la matrice creuse header existe, cette fonction
       retourne SXTRUE et ref designe cet element, sinon elle cree un nouvel
       element, ref le designe et elle retourne SXFALSE */

    SXINT	x, z;
    SXINT	                old_hash_size, old_size, *old;
    SXBOOLEAN		overflow;

    sxinitialise(old_size); /* pour faire taire "gcc -Wuninitialized" */
    if ((x = X_root_is_set (header, scramble)) > 0) {
	SXINT	*alnk;

	if ((*(alnk = header->hash_lnk + (*ref = x)) & X_root_80) == 0)
	    return SXTRUE;

	/* erased */
	/* l'element qui vient d'etre reutilise est celui qui a ete supprime precedemment
	   on n'a donc rien a faire pour les "foreach". */
	*alnk &= X_root_7F; /* tant pis pour "has_free_buckets"! */
	return SXFALSE;
    }

/* Son predecesseur ds le chainage est -x. */

    overflow = SXFALSE;

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
	SXBOOLEAN	is_erased;

	is_erased = x < 0 ? (header->hash_lnk [-x] & X_root_80) != 0 : SXFALSE;

	if ((z = X_root_gc (header)) > 0) {
	    if (header->suppress != NULL) {
		/* il faut supprimer vis a vis des foreach les elements de la liste z. */
		/* Attention, suppress touche a HASH_VAL_ */
		SXUINT hv = HASH_VAL_;

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

	overflow = SXTRUE;
	z = ++(header->top);
	header->hash_size = sxnext_prime (old_hash_size = header->hash_size);
	header->size = 2 * (old_size = header->size);

	if (header->is_static) {
	    old = header->hash_display;
	    X_root_int_cpy (old,
			    header->hash_display = (SXINT*) sxalloc (header->hash_size,
								   sizeof (SXINT)),
			    old_hash_size - 1);

	    old = header->hash_lnk;
	    X_root_int_cpy (old,
			    header->hash_lnk = (SXINT*) sxalloc (header->size + 1, sizeof (SXINT)),
			    old_size);
	}
	else {
	    header->hash_display = (SXINT*) sxrealloc (header->hash_display,
						     header->hash_size,
						     sizeof (SXINT));
	    header->hash_lnk = (SXINT*) sxrealloc (header->hash_lnk, header->size + 1, sizeof (SXINT));
	}

	/* Traitement par l'appelant du debordement. */
	(*(header->system_oflw)) (old_size);

	if (header->stat_file != NULL)
	    X_root_sat_mess (header->stat_file, header->name, old_size, header->size);
    }

    {
	SXINT *adisp;

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
	SXINT		hash_size;
	SXINT		*cur, *hash_lnk, *hash_display;
	SXINT		(*cmp)(SXINT);
	SXUINT	        (*scrmbl)(SXINT);

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

	header->is_static = SXFALSE;
    }


    return SXFALSE;
}


void	X_root_lock (X_root_header *header)
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure si elle n'est pas statique
	  - Interdit l'utilisation ulterieure de X_root_set
	  - Appelle (si non NULL) la procedure utilisateur user_oflw
       L'utisateur vient peut etre d'appeler X[xY[xZ]]_gc. */

    SXINT old_top;

    header->is_locked = SXTRUE;

    if (!header->is_static && header->size > (old_top = header->top)) {
	header->hash_lnk = (SXINT*) sxrealloc (header->hash_lnk,
					     (header->size = old_top) + 1, sizeof (SXINT));

	/* Traitement par l'appelant du "retrecissement". */
	
	(*(header->system_oflw)) (old_top);

	if (header->user_oflw != NULL)
	    (*(header->user_oflw)) (old_top, header->size);
    }
}


SXBOOLEAN X_root_write (X_root_header *header, sxfiledesc_t F_X_root /* file descriptor */)
{
    size_t	bytes;

#define WRITE(p,l)	if ((bytes = (l))>0 && ((size_t)write (F_X_root, p, bytes) != bytes)) return SXFALSE

    WRITE (&(header->hash_size), sizeof (SXINT));
    WRITE (&(header->top), sizeof (SXINT));
    WRITE (&(header->size), sizeof (SXINT));
    WRITE (&(header->free_buckets), sizeof (SXINT));
    WRITE (&(header->has_free_buckets), sizeof (SXINT));
    WRITE (&(header->is_locked), sizeof (SXBOOLEAN));

    WRITE (header->hash_display, sizeof (SXINT) * header->hash_size);
    WRITE (header->hash_lnk, sizeof (SXINT) * (header->top + 1));

    return SXTRUE;
}


SXBOOLEAN X_root_read (X_root_header *header, sxfiledesc_t F_X_root /* file descriptor */)
{
    size_t	bytes;

#define READ(p,l)	if ((bytes = (l)) > 0 && ((size_t)read (F_X_root, p, bytes) != bytes)) return SXFALSE

    READ (&(header->hash_size), sizeof (SXINT));
    READ (&(header->top), sizeof (SXINT));
    READ (&(header->size), sizeof (SXINT));
    READ (&(header->free_buckets), sizeof (SXINT));
    READ (&(header->has_free_buckets), sizeof (SXINT));
    READ (&(header->is_locked), sizeof (SXBOOLEAN));

    header->is_static = SXFALSE;

    header->hash_display = (SXINT*) sxalloc (header->hash_size, sizeof (SXINT));
    READ (header->hash_display, sizeof (SXINT) * header->hash_size);

    header->hash_lnk = (SXINT*) sxalloc (header->size + 1, sizeof (SXINT));
    READ (header->hash_lnk, sizeof (SXINT) * (header->top + 1));

    return SXTRUE;
}


void X_root_array_to_c (X_root_header *header, FILE *F_X /* named output stream */, char *name)
{
  SXINT i, top;


  top = header->hash_size - 1;

  while (header->hash_display [top] == 0)
    top--;

  if (top >= 0) {
    fprintf (F_X, "\n\nstatic SXINT %s_hash_display [%ld] = {", name, (SXINT) header->hash_size);

    for (i = 0; i <= top; i++) {
      if ((i & 7) == 0)
	fprintf (F_X, "\n/* %ld */ ", (SXINT) i);

      fprintf (F_X, "%ld, ", (SXINT) header->hash_display [i]);
    }

    fputs ("\n}", F_X);
  }
  else {
    fprintf (F_X, "\n\nstatic SXINT %s_hash_display [1] /* empty */;\n", name);
  }

  fprintf (F_X, ";\n\n\nstatic SXINT %s_hash_lnk [%ld]", name, (SXINT) header->size + 1);

  top = header->top;

  while (header->hash_lnk [top] == 0)
    top--;

  if (top >= 0) {
    fputs (" = {", F_X);

    for (i = 0; i <= top; i++) {
      if ((i & 7) == 0)
	fprintf (F_X, "\n/* %ld */ ", (SXINT) i);

      fprintf (F_X, "%ld, ", (SXINT) header->hash_lnk [i]);
    } 

    fputs ("\n}", F_X);
  }
  else
    fputs (" /* empty */", F_X);

  fputs (";\n", F_X);
}


void X_root_header_to_c (X_root_header *header, FILE *F_X /* named output stream */, char *name)
{
    fprintf (F_X, "{\"%s\", /* name */\n", name);
    fprintf (F_X, "%s_hash_display, /* hash_display */\n", name);
    fprintf (F_X, "%s_hash_lnk, /* hash_lnk */\n", name);
    fprintf (F_X, "%ld, /* hash_size */\n", (SXINT) header->hash_size);
    fprintf (F_X, "%ld, /* top */\n", (SXINT) header->top);
    fprintf (F_X, "%ld, /* size */\n", (SXINT) header->size);

    if ((header->free_buckets) == X_root_80) /* Le 10/03/2004 warning a la compil avec -2147483648 */
      fputs ("X_root_80, /* free_buckets */\n", F_X);
    else
      fprintf (F_X, "%ld, /* free_buckets */\n", (SXINT) header->free_buckets);
    
    fprintf (F_X, "%ld, /* has_free_buckets */\n", (SXINT) header->has_free_buckets);
    fputs ("NULL, /* (*system_oflw) () */\n", F_X);
    fputs ("NULL, /* (*user_oflw) () */\n", F_X);
    fputs ("NULL, /* (*cmp) () */\n", F_X);
    fputs ("NULL, /* (*scrmbl) () */\n", F_X);
    fputs ("NULL, /* (*suppress) () */\n", F_X);
    fputs ("NULL, /* (*assign) () */\n", F_X);
    fputs ("NULL, /* stat_file */\n", F_X);
    fprintf (F_X, "%ld, /* is_locked */\n", header->is_locked ? (SXINT) 1 : (SXINT) 0);
    fputs ("SXTRUE, /* is_static */\n", F_X);
    fputs ("SXTRUE, /* is_allocated */\n", F_X);
}


void X_root_reuse (X_root_header *header, char *name, void (*system_oflw) (SXINT), void (*user_oflw) (SXINT, SXINT), SXINT (*cmp) (SXINT), SXUINT (*scrmbl) (SXINT), SXINT (*suppress) (SXINT), SXINT (*assign) (SXINT), FILE *stat_file)
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


