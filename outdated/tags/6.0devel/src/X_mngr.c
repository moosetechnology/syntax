/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/










#include "sxversion.h"
#include "sxcommon.h"
#include "X.h"

char WHAT_X_MNGR[] = "@(#)SYNTAX - $Id: X_mngr.c 2271 2020-03-09 18:55:38Z garavel $" WHAT_DEBUG;

extern void sxtrap (char *caller, char *message);

static SXINT		X_;
static X_header		*HEADER_;


static void X_cpy (struct X_elem *old, struct X_elem *new, SXINT old_size)
{
    struct X_elem *old_top = old + old_size;

    do {
	*new++ = *old++;
    } while (old <= old_top);
}

static void X_system_oflw (SXINT old_top)
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
}


static SXINT X_cmp (SXINT x)
{
    /* Appele depuis X_root_s_set.
       "Compare" l'element courant se trouvant a l'indice x avec l'element que l'on
       recherche. Cette fonction retourne
          -1 si <
	  0  si ==
	  1  si >
    */
    SXINT		z;

    if ((z = HEADER_->display [x].X) >= X_)
	return z == X_ ? 0 : 1;

    return -1;
}


static SXUINT X_scrmbl (SXINT x)
{
    /* Retourne la valeur du "scramble" de l'element d'indice x. */
    /* Positionne X_, ce qui permet d'appeler "cmp". */
    return X_HASH_FUN (X_ = HEADER_->display [x].X);
}

static SXINT X_assign (SXINT x)
{
    /* Positionne l'element courant a l'indice x. */
    return HEADER_->display [x].X = X_;
}


void	X_alloc (X_header *header, 
		 char *name, 
		 SXINT init_elem_nb, 
		 SXINT average_list_nb_per_bucket, 
		 void (*user_oflw) (SXINT, SXINT), 
		 FILE *stat_file)
{
    /* Cette fonction "cree" une table d'entiers extensible */
    /* De header "header", de nom "name", dont la taille initiale de la table de hash_code
       est un nombre premier et la longueur moyenne des chaines
       de collision est "average_list_nb_per_bucket".
       La taille de la structure est doublee chaque fois
       que cette longueur moyenne depasse la taille fixee, de plus la fonction utilisateur
       "oflw" est appelee. */
    X_root_alloc ((X_root_header*)header, name, init_elem_nb, average_list_nb_per_bucket, X_system_oflw,
		  user_oflw, X_cmp, X_scrmbl, (SXINT (*) (SXINT)) NULL, X_assign, stat_file);
    header->display = (struct X_elem*) sxalloc (header->size + 1, sizeof (struct X_elem));
}



void	X_clear (X_header *header)
{
    if (header->top != 0)
	X_root_clear ((X_root_header*)header);
}



void	X_stat (FILE *stat_file, X_header *header)
{
    if (stat_file != NULL)
	X_root_stat (stat_file, (X_root_header*)header);
}




void	X_free (X_header *header)
{
    if (!header->is_static)
    {
	X_root_free ((X_root_header*)header);
	sxfree (header->display), header->display = NULL;
    }
}



SXINT	X_is_set (X_header *header, SXINT X)
{
    /* Si l'element X de header existe, cette fonction
       retourne l'indice de cet element sinon 0. */
    SXINT z;

    X_ = X;

    if ((z = X_root_is_set ((X_root_header*)(HEADER_ = header), X_HASH_FUN (X_))) <= 0)
	return 0;
	
    return header->hash_lnk [z] & X_80 ? 0 : z;
}



void	X_gc (X_header *header)
{
    /* Cette procedure supprime physiquement les elements effaces
       par un "X_erase" et permet la reutilisation des indices
       ainsi recuperes. */
    SXINT		x, erased_lnk_hd;
    SXINT		*hash_lnk, *alnk;

    if (header->has_free_buckets != 0 && (erased_lnk_hd = X_root_gc ((X_root_header*)header)) > 0) {
	hash_lnk = HEADER_->hash_lnk;
	
	/* On concatene erased_lnh_hd a free_buckets. */
	for ((alnk = &(HEADER_->free_buckets)); (x = (*alnk & X_7F)) != 0; alnk = hash_lnk + x);
	
	*alnk = erased_lnk_hd | X_80;
    }
}


SXBOOLEAN	X_set (X_header *header, SXINT X, SXINT *ref)
{
    /* Si l'element X de header existe, cette fonction
       retourne SXTRUE et ref designe cet element, sinon elle cree un nouvel
       element, ref le designe et elle retourne SXFALSE */

    if (header->is_locked)
	sxtrap ("X_set", header->name);

    X_ = X;

    return X_root_set ((X_root_header*)(HEADER_ = header), X_HASH_FUN (X_), ref);
}


void	X_lock (X_header *header)
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure X
	  - Interdit l'utilisation ulterieure de X_set
	  - Appelle (si non NULL) la procedure utilisateur oflw
       L'utisateur vient peut etre d'appeler X_gc. */
    if (header->is_locked)
	return;

    X_root_lock ((X_root_header*)(HEADER_ = header));
}

SXBOOLEAN X_write (X_header *header, sxfiledesc_t F_X /* file descriptor */)
{
    size_t	bytes;

#define WRITE(p,l)	if ((bytes = (l)) > 0 && ((size_t)write (F_X, p, bytes) != bytes)) return SXFALSE

    if (X_root_write ((X_root_header*)header, F_X)) {
	WRITE (header->display, sizeof (struct X_elem) * (header->top + 1));

	return SXTRUE;
    }

    return SXFALSE;
}


SXBOOLEAN X_read (X_header *header, sxfiledesc_t F_X /* file descriptor */, char *name, void (*user_oflw) (SXINT, SXINT), FILE *stat_file)
{
    size_t	bytes;

#define READ(p,l)	if ((bytes = (l)) > 0 && ((size_t)read (F_X, p, bytes) != bytes)) return SXFALSE

    if (X_root_read ((X_root_header*)header, F_X)) {
	header->display = (struct X_elem*) sxalloc (header->size + 1, sizeof (struct X_elem));
	READ (header->display, sizeof (struct X_elem) * (header->top + 1));
	header->name = name;
	header->system_oflw = X_system_oflw;
	header->user_oflw = user_oflw;
	header->cmp = X_cmp;
	header->scrmbl = X_scrmbl;
	header->suppress = (SXINT (*) (SXINT)) NULL;
	header->assign = X_assign;
	header->stat_file = stat_file;

	return SXTRUE;
    }

    return SXFALSE;
}

void X_array_to_c (X_header *header, FILE *F_X /* named output stream */, char *name)
{
    SXINT i;

    fprintf (F_X, "\n\nstatic struct X_elem %s_display [%ld] = {", name, (long)header->size + 1);

    for (i = 0; i <= header->top; i++) {
	if ((i & 7) == 0)
	    fprintf (F_X, "\n/* %ld */ ", (long)i);

	fprintf (F_X, "{%ld}, ", (long)header->display [i].X);
    } 

    fprintf (F_X, "\n};\n");

    X_root_array_to_c ((X_root_header*)header, F_X, name); /* hash_display, hash_lnk */
}


void X_header_to_c (X_header *header, FILE *F_X /* named output stream */, char *name)
{
    X_root_header_to_c ((X_root_header*)header, F_X, name);
    
    fprintf (F_X, "%s_display /* display */\n", name);

    fprintf (F_X, "}");
}


void X_to_c (X_header *header, FILE *F_X /* named output stream */, char *name, SXBOOLEAN is_static)
{
    X_array_to_c (header, F_X, name);
    fprintf (F_X, "\n\n%sX_header %s =\n", is_static ? "static " : "", name);
    X_header_to_c (header, F_X, name);
    fprintf (F_X, " /* End X_header %s */;\n", name);
}



void X_reuse (X_header *header, 
	      char *name, 
	      void (*user_oflw) (SXINT, SXINT), 
	      FILE *stat_file)
{
    X_root_reuse ((X_root_header*)header, name, X_system_oflw,
		  user_oflw, X_cmp, X_scrmbl, (SXINT (*) (SXINT)) NULL,
		  X_assign, stat_file);
}

