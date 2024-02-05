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

#include "sxversion.h"
#include "sxunix.h"
#include <stdarg.h>

char WHAT_SXINCL_MNGR[] = "@(#)SYNTAX - $Id: sxincl_mngr.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

/*---------------------------------------------------------------------------*/

static bool   sxname_separation = false;

/*
 * mettre sxname_separation a true permet de ranger les pathnames des
 * fichiers inclus dans une table differente de sxstrmngr, ce qui permet
 * de donner aux fichiers inclus des numeros contigus a partir de zero, 
 * valeur qui designe le fichier principal; si sxname_separation vaut false
 * on n'a aucune garantie sur ces numeros
 */

/*---------------------------------------------------------------------------*/

/* pile stockant les fichiers actuellement ouverts */

struct incl {
    FILE	*infile;
    struct sxsrcmngr	srcmngr;
    SXINT ste;
    /* ste est l'indice de la chaine contenant le pathname du fichier ;
     * cette chaine est contenue soit dans la table sxstrmngr si
     * sxname_separation vaut false, soit dans la table incl_pathnames
     * si sxname_separation vaut true ; dans ce dernier cas, les valeurs
     * de ste sont contigues car incl_pathnames ne contient que des noms
     * de fichiers (plus SXERROR_STE et SXEMPTY_STE) mais pas de lexemes
     */
};
static struct incl	*incls;
static SXINT	incl_depth = 0;
static SXINT    incls_size;

/*---------------------------------------------------------------------------*/

/*
 * table stockant tous les fichiers inclus ou qui l'ont ete; seulement
 * definie si sxname_separation vaut true
 */
static sxstrmngr_t *incl_pathnames = NULL; /* par precaution */

/*
 * sxown_incl_pathnames vaut true si incl_pathnames est allouee a
 * l'interieur de ce module et false si incl_pathnames vaut l'adresse
 * d'une table fournie par l'appelant et allouee a l'exterieur de ce module
 */
static bool sxown_incl_pathnames;

/*
 * nombre d'elements inseres par defaut dans incl_pathnames, vraisemblablement
 * deux (SXERROR_STE et SXEMPTY_STE); seulement defini si sxname_separation
 * vaut true
 */
static SXINT incl_pathnames_initial_size;

/*---------------------------------------------------------------------------*/

/*
 * table stockant le niveau de profondeur d'inclusion pour les fichiers inclus ;
 * comme indice, on reutilise celui de la table des noms des fichiers ; pour
 * avoir un tableau de taille limite au nombre de fichiers, ce tableau est
 * seulement defini si sxname_separation vaut true
 */
static SXINT *incl_depths = NULL;

/* taille courante de incl_depths[] */
static SXINT incl_depths_size = 0;

/*---------------------------------------------------------------------------*/

static bool my_push_incl (char *pathname,
                               bool disallow_multiple_inclusions,
                               bool disallow_recursive_inclusions)
{
    FILE	*file;
    SXINT	x;
    struct incl	*ai;
    SXINT	nb_includes, ste;

    if ((file = sxfopen (pathname, "r")) == NULL) {
        return false;
    }

    nb_includes = 0; /* to avoid Gcc "-Wmaybe-uninitialized" warnings */ 
    if (sxname_separation == false) {
      ste = sxstrsave (pathname);
    } else {
      nb_includes = sxstr_size (incl_pathnames);
      ste = sxstr_save (incl_pathnames, pathname);
    }

    if (disallow_recursive_inclusions) {
      /* note : on ne detecte pas l'inclusion du fichier principal, car il n'est
       * stocke sur la pile incls[] */
      for (x = incl_depth - 1; x >= 0; x--) {
	 if (incls [x].ste == ste) {
	    fclose (file);
	    return false; /* appel recursif */
	 }
      }
    }

    if (disallow_multiple_inclusions) {
      if (sxname_separation == false) {
        fprintf (sxstderr, "sxincl_mngr: sxpush_uniqincl() requires SXSEPARATE\n");
        sxexit (1);
      } else {
        if (sxstr_size (incl_pathnames) == nb_includes) {
          /* le nom de fichier pathname etait deja present dans la table avant
           * l'appel de my_push_incl() : on ne fait pas l'inclusion */
	  return true; /* fichier pathname deja inclus auparavant */
        }
      } 
    }

    if (incl_depth >= incls_size)
	incls = (struct incl*) sxrealloc (incls, incls_size += 5, sizeof (struct incl));

    ai = incls + incl_depth++;
    ai->srcmngr = sxsrcmngr;
    ai->infile = file;
    ai->ste = ste;
    if (sxname_separation == false) {
      sxsrc_mngr (SXINIT, ai->infile, sxstrget (ste));
    } else {
	sxsrc_mngr (SXINIT, ai->infile, sxstr_get (incl_pathnames, ste));
	if (ste >= incl_depths_size) {
	    /* agrandissement de incl_depths[] */
	    incl_depths = (SXINT *) sxrealloc (incl_depths, incl_depths_size + SXHASH_LENGTH, sizeof (SXINT));
	    if (incl_depths == NULL) {
		fprintf (sxstderr, "sxincl_mngr: not enough memory for storing include depths\n");
		sxexit (1);
	    }
	    memset (incl_depths + incl_depths_size, -1, SXHASH_LENGTH * sizeof (SXINT));
	    incl_depths_size = incl_depths_size + SXHASH_LENGTH;
	}
	if (incl_depths[ste] < incl_depth) {
	    incl_depths[ste] = incl_depth;
	}
    }

    sxnext_char () /* Lecture du premier caractere de l'include */;
    return true;
}

/*---------------------------------------------------------------------------*/

bool		sxpush_incl (char *pathname)
{
    return my_push_incl (pathname, false, true);
}

/*---------------------------------------------------------------------------*/

bool		sxpush_recincl (char *pathname)
{
    return my_push_incl (pathname, false, false);
}

/*---------------------------------------------------------------------------*/

bool		sxpush_uniqincl (char *pathname)
{
    return my_push_incl (pathname, true, true);
}

/*---------------------------------------------------------------------------*/

bool		sxpop_incl (void)
{
    struct incl	*ai;

    if (incl_depth <= 0)
	return false;

    ai = incls + --incl_depth;
    sxsrc_mngr (SXFINAL);
    sxsrcmngr = ai->srcmngr;
    sxfclose (ai->infile);

    return true;
}

/*---------------------------------------------------------------------------*/

SXINT sxincl_depth (void)
{
    return incl_depth;
}

/*---------------------------------------------------------------------------*/

SXINT sxincl_size (void)
{
    if (sxname_separation == false)
        return 0; /* resultat non defini */ 
    else {
        /* on renvoie une valeur numerotee a partir de zero */
        return sxstr_size (incl_pathnames) - incl_pathnames_initial_size;
    }
}

/*---------------------------------------------------------------------------*/

char *sxincl_get (SXINT incl_index)
{
    if (sxname_separation == false)
        return sxstrget (incl_index);
    else {
        /* on recoit une valeur incl_index numerotee a partir de zero */
        return sxstr_get (incl_pathnames, incl_index + incl_pathnames_initial_size);
    }
}

/*---------------------------------------------------------------------------*/

SXINT sxincl_retrieve (char *pathname)
{
    SXINT ste;

    if (sxname_separation == false) {
        ste = sxstrretrieve (pathname);
        if (ste == SXERROR_STE)
           return -1;
        return ste;
    } else {
        ste = sxstr_retrieve (incl_pathnames, pathname);
        if (ste == SXERROR_STE)
           return -1;
        /* sinon, on renvoie une valeur numerotee a partir de zero */
        return ste - incl_pathnames_initial_size;
    }
}

/*---------------------------------------------------------------------------*/

SXINT sxincl_max_depth (void)
{
    SXINT size, i, max_depth;

    /* renvoie la profondeur maximale des fichiers inclus */

    if (incl_depths == NULL) {
	return (0);
    }

    size = sxincl_size () + incl_pathnames_initial_size;
    max_depth = 0;
    for (i = incl_pathnames_initial_size; i < size; i++) {
	if (incl_depths[i] > max_depth) {
	    max_depth = incl_depths[i];
	}
    }
    return (max_depth);
}

/*---------------------------------------------------------------------------*/

SXINT sxincl_get_depth (SXINT incl_index)
{
    /* renvoie la profondeur du fichier inclus numero incl_index */

    if (incl_depths == NULL) {
        return (0);
    }

    return (incl_depths[incl_index + incl_pathnames_initial_size]);
}

/*---------------------------------------------------------------------------*/

void sxincl_depend_but (FILE *f, SXINT order, SXINT excluded_index)
{
    SXINT size, i, max_depth, depth;
    
    /* affichage de tous les noms des fichiers inclus, sauf de celui qui
     * porte le numero "excluded_index" dans l'ordre specifie */

    size = sxincl_size ();
    switch (order) {
    case sxincl_order_reading:
	for (i = 0; i < size; i++) {
	    if (i != excluded_index) {
		fprintf (f, "%s\n", sxincl_get (i));
	    }
	}
	break;

    case sxincl_order_inverse_reading:
	for (i = size; i >= 0; i--) {
	    if (i != excluded_index) {
		fprintf (f, "%s\n", sxincl_get (i));
	    }
	}
	break;

    case sxincl_order_increasing_depth:
	if (sxname_separation == false) {
	    fprintf (sxstderr, "sxincl_mngr: unsupported printing order\n");
	    break;
	}
	max_depth = sxincl_max_depth ();
	for (depth = 0; depth <= max_depth; depth++) {
	    for (i = 0; i < size; i++) {
		if ((i != excluded_index) &&
		    (sxincl_get_depth (i) == depth)) {
		    fprintf (f, "%s\n", sxincl_get (i));
		}
	    }
	}
	break;

    case sxincl_order_decreasing_depth:
	if (sxname_separation == false) {
	    fprintf (sxstderr, "sxincl_mngr: unsupported printing order\n");
	    break;
	}
	for (depth = sxincl_max_depth (); depth >= 0; depth--) {
	    for (i = 0; i < size; i++) {
		if ((i != excluded_index) &&
		    (sxincl_get_depth (i) == depth)) {
		    fprintf (f, "%s\n", sxincl_get (i));
		}
	    }
	}
	break;

    default:
	fprintf (sxstderr, "sxincl_mngr: unsupported printing order\n");
	break;
    }
}

/*---------------------------------------------------------------------------*/

void sxincl_depend (FILE *f, SXINT order)
{
    /* appel de sxincl_depend_but avec un index invalide */
    sxincl_depend_but (f, order, -1);
}

/*---------------------------------------------------------------------------*/

void	sxincl_mngr (SXINT sxincl_mngr_what, ...)
{
    va_list ap;
    char *principal_pathname;
    SXINT ste;
     
    switch (sxincl_mngr_what) {
    case SXOPEN:
	incls = (struct incl*) sxalloc (incls_size = 5, sizeof (struct incl));
	break;

    case SXINIT:
	incl_depth = 0;
	sxname_separation = false;
	break;

    case SXSEPARATE:
	/* n'a d'effet qui si appele apres sxincl_mngr (SXINIT) */
	/* SXSEPARATE doit etre suivi d'un pointeur vers une table sxstrmngr_t
	 * (eventuellement NULL) et d'un pointeur vers une chaine de caracteres
	 * donnant le nom du fichier principal.
	 * SXSEPARATE force Syntax a stocker les noms de fichiers inclus
	 * (y compris le nom du fichier principal) dans la table separee
	 * incl_pathnames, au lieu de les melanger avec les lexemes
	 */
        sxname_separation = true;
	va_start (ap, sxincl_mngr_what);
        incl_pathnames = va_arg (ap, sxstrmngr_t*);
        if (incl_pathnames != NULL) {
	    /* l'utilisateur a fourni l'adresse d'une table allouee par lui */
	    sxown_incl_pathnames = false;
	} else {
	    /* creation de la table incl_pathnames */
	    incl_pathnames = (sxstrmngr_t *) sxalloc (1, sizeof (sxstrmngr_t));
	    if (incl_pathnames == NULL) {
		fprintf (sxstderr, "sxincl_mngr: not enough memory for storing include pathnames\n");
		sxexit (1);
	    }
	    /* initialisation de champs testes par sxstr_mngr() pour savoir si
	     * la table est vide */
	    incl_pathnames->head = NULL;
	    incl_pathnames->top = 1; /* n'importe quelle valeur non nulle */
            sxstr_mngr (SXOPEN, incl_pathnames);
	    sxown_incl_pathnames = true;
	}
	/* on mesure la taille initiale de cette table, que l'on definit
	 * comme le nombre de valeurs inutiles stockees par defaut dans
	 * incl_pathnames lors de sa creation (vraisemblablement 2,
	 * c'est-a-dire, SXERROR_STE et SXEMPTY_STE) */
	incl_pathnames_initial_size = sxstr_size (incl_pathnames);
	/* recuperation du nom de fichier principal */
	principal_pathname = va_arg (ap, char*);
	if (principal_pathname [0] == '\0') {
	    /* principal_pathname est la chaine vide qui, par defaut, fait
	     * deja partie de la table incl_pathnames (SXEMPTY_STE) */
	     incl_pathnames_initial_size -= 1;
	     ste = SXEMPTY_STE;
	} else {
	     /* on stocke le nom du fichier principal passe en parametre */
	     ste = sxstr_save (incl_pathnames, principal_pathname);
	}
	/* allocation et initialisation de incl_depths */
	if (incl_depths == NULL) {
	    incl_depths = (SXINT *) sxalloc (SXHASH_LENGTH, sizeof (SXINT));
	    if (incl_depths == NULL) {
		fprintf (sxstderr, "sxincl_mngr: not enough memory for storing include depths\n");
		sxexit (1);
	    }
	    incl_depths_size = SXHASH_LENGTH;
	    memset (incl_depths, -1, SXHASH_LENGTH * sizeof (SXINT));
	    incl_depths[ste] = 0;
	}
	va_end (ap);
	break;

    case SXFINAL:
	while (incl_depth > 0)
	    sxfclose (incls [--incl_depth].infile);
	break;

    case SXCLOSE:
	sxfree (incls);
	if ((sxname_separation == true) && (sxown_incl_pathnames == true)) {
	    /* on ne purge et detruit la table incl_pathnames que si celle-
	     * ci n'a pas ete allouee directement par l'utilisateur
	     */
	    sxstr_mngr (SXCLOSE, incl_pathnames);
	    sxfree (incl_pathnames);
	}
	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxincl_mngr","unknown switch case #1");
#endif
      break;
    }
}
