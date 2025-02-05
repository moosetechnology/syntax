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


/* Permet de manipuler les sous-ensembles de la partie [0:n] des entiers naturels.
   Le re'fe'rentiel peut e^tre (tous) les entiers relatifs si l'on filtre par le X_mngr.
   La valeur de n peut changer dynamiquement.
   
   Fonctionnalites similaires (pour des ensembles) aux packages "XH", "XT" et sxlist.

   Un subset est identifie' par un entier positif ou nul.
   Un subset d'un referentiel de cardinal n occupe a peu pres
   (5 + (n+SXBITS_PER_LONG-1)/SXBITS_PER_LONG) cellules.

   Comparaison (sur 10000 ensembles aleatoires de taille max 200 prenant leurs
                elements sur [0..499]):
                XH		XT		sxlist		sxsubset
   Temps	1.47		8.22		3.71		1
   Place	4.82		19.57		9.85		1

   Avantages :
      - Re'cupere effectivement la place occupe'e par les subsets qui ont e'te'
        "released", sans changer les identifiants des subsets actifs.
      - Supporte la construction simultane'e de plusieurs subsets.
      - Chaque sous-ensemble peut e^tre manipule' par les SXBA

   Inconve'nients :
      - Chaque subset occupe une place fixe qui ne de'pend que du
        cardinal du re'fe'rentiel

   Bug :
      - He'rite' des SXBA, les ensembles calcule's par sxba_not, sxba_fill posent des
        problemes lorsque leur taille augmente...

   Une implantation avec des sxindex matriciels a ete essayee et s'est revelee plus 
   mauvaise.
*/

#include "sxversion.h"
#include "sxcommon.h"
#include "sxsubset.h"
#include "sxalloc.h"
#include <memory.h>
#include <unistd.h>
#include <string.h>

char WHAT_SXSUBSET_MNGR[] = "@(#)SYNTAX - $Id: sxsubset_mngr.c 4488 2024-10-26 08:32:33Z garavel $" WHAT_DEBUG;

static sxsubset_header *_sxsubset_header; /* Pour oflw */

extern SXINT sxnext_prime (SXINT germe);

static void
sxsubset_subset_oflw (sxindex_header *subset_hd, SXINT old_line_nb, SXINT old_size)
{
    SXINT new_size = sxindex_size (_sxsubset_header->subset_hd);
    SXINT cardinal = sxsubset_card (*_sxsubset_header);
    SXINT *p, *q, i, x;
    SXUINT scrmbl;

    sxuse(old_line_nb); /* pour faire taire gcc -Wunused */
    sxuse(subset_hd); /* pour faire taire gcc -Wunused */
    _sxsubset_header->hash_size = sxnext_prime (_sxsubset_header->hash_size);

    if (sxindex_is_static (_sxsubset_header->subset_hd))
    {
	SXBA	*old_subsets = _sxsubset_header->subsets;
	char *old = (char*) _sxsubset_header->display;

	_sxsubset_header->hash_tbl = (SXINT*) sxalloc (_sxsubset_header->hash_size, sizeof (SXINT));
	
	_sxsubset_header->subsets = sxbm_calloc (new_size, cardinal);

	for (i = 0; i < old_size; i++)
	    sxba_copy (_sxsubset_header->subsets [i], old_subsets [i]);

	memcpy ((char*)(_sxsubset_header->display = (struct subset_display*)
			sxalloc (new_size,
				 sizeof (struct subset_display))),
		old,
		old_size * sizeof (struct subset_display));
    }
    else
    {
	_sxsubset_header->hash_tbl = (SXINT*) sxrealloc (_sxsubset_header->hash_tbl,
						       _sxsubset_header->hash_size,
						       sizeof (SXINT));

	_sxsubset_header->subsets = sxbm_resize (_sxsubset_header->subsets,
						 old_size,
						 new_size,
						 cardinal);
						 
	_sxsubset_header->display = (struct subset_display*)
	    sxrealloc (_sxsubset_header->display,
		       new_size,
		       sizeof (struct subset_display));

    }

    p = _sxsubset_header->hash_tbl + _sxsubset_header->hash_size;

    while (--p >= _sxsubset_header->hash_tbl)
	*p = -1;

    sxindex_foreach (_sxsubset_header->subset_hd, i)
    {
	if (*(q = &(_sxsubset_header->display [i].lnk)) != -2)
	{
	    /* C'est un vrai sous-ensemble */
	    scrmbl = _sxsubset_header->display [i].scrmbl;

	    for (p = _sxsubset_header->hash_tbl + (scrmbl % _sxsubset_header->hash_size);
		 (x = *p) >= 0 && _sxsubset_header->display [x].scrmbl <= scrmbl;
		 p = &(_sxsubset_header->display [x].lnk));

	    *q = x;
	    *p = i;
	}
    }

    if (_sxsubset_header->subset_oflw != NULL)
	(*_sxsubset_header->subset_oflw) (old_size, new_size);
}


void
sxsubset_elem_oflw (sxsubset_header *h, SXINT new_cardinal)
{
    SXINT size = sxindex_size (h->subset_hd);
    SXINT old_cardinal = sxsubset_card (*h);

    if (sxindex_is_static (h->subset_hd)) {
	SXINT	slices = SXNBLONGS (old_cardinal);
	SXBA	*old_subsets = h->subsets, s1, s2;
	SXINT	x, i;

	h->subsets = sxbm_calloc (size, new_cardinal);

	sxindex_foreach (h->subset_hd, x)
	{
	    s1 = old_subsets [x];
	    s2 = h->subsets [x];

	    for (i = slices; i > 0; i--)
		s2 [i] = s1 [i];
	}
    }
    else
	h->subsets = sxbm_resize (h->subsets,
				  size,
				  size,
				  new_cardinal);

    if (h->elem_oflw != NULL)
	(*h->elem_oflw) (old_cardinal, new_cardinal);
}

void
sxsubset_alloc (sxsubset_header *h,
		SXINT subset_nb,
		SXINT cardinal,
		SXINT (*user_subset_oflw) (SXINT, SXINT),
		SXINT (*user_elem_oflw) (SXINT, SXINT),
		FILE *stat_file)
{
    SXINT size, *p;

    sxindex_alloc (&(h->subset_hd), 0, subset_nb, sxsubset_subset_oflw);
    size = sxindex_size (h->subset_hd);

    h->hash_size = sxnext_prime (subset_nb);
    h->hash_tbl = (SXINT*) sxalloc (h->hash_size, sizeof (SXINT));

    p = h->hash_tbl + h->hash_size;

    while (--p >= h->hash_tbl)
	*p = -1;

    h->subsets = sxbm_calloc (size, cardinal);

    h->display = (struct subset_display*) sxalloc (size,
						   sizeof (struct subset_display));

    h->subset_oflw = user_subset_oflw;
    h->elem_oflw = user_elem_oflw;

    h->stat_file = stat_file;
}

void
sxsubset_stat (FILE *stat_file, sxsubset_header *h)
{
    SXINT total_sizeof, used_sizeof;
    SXINT size = sxindex_size (h->subset_hd);
    SXINT top = sxindex_top (h->subset_hd);

    total_sizeof = used_sizeof = sizeof (sxsubset_header) +
	    sizeof (SXINT) * (h->hash_size) ;

    total_sizeof += sxindex_total_sizeof (h->subset_hd) +
	size * (SXBA_total_sizeof (h->subsets [0]) + sizeof (SXBA*)) +
	    size * sizeof (struct subset_display);

    used_sizeof += sxindex_used_sizeof (h->subset_hd) +
	top * (SXBA_used_sizeof (h->subsets [0]) + sizeof (SXBA*)) +
	    top * sizeof (struct subset_display);

    fprintf (stat_file, "sxsubset_header: Total_sizeof (byte) = %ld (%ld%%)\n",
	     (SXINT) total_sizeof, (SXINT) (100*used_sizeof)/total_sizeof);
}

void
sxsubset_free (sxsubset_header *h)
{
    if (h->stat_file != NULL)
	sxsubset_stat (h->stat_file, h);
    
    if (!sxsubset_is_static (*h))
    {
	sxfree (h->display), h->display = NULL;
	sxfree (h->hash_tbl), h->hash_tbl = NULL;
	sxbm_free (h->subsets), h->subsets = NULL;
	sxindex_free (h->subset_hd);
    }
}

 
SXINT
sxsubset_seek (sxsubset_header *h)
{
    /* Retourne l'identifiant entier d'un nouvel ensemble manipulable par les
       SXBA via sxsubset_to_sxba */
    SXINT			subset_nb;

    _sxsubset_header = h; /* Pour les oflw */

    h->display [subset_nb = sxindex_seek (&(h->subset_hd))].lnk = -2; /* Ce n'est (pas encore)
								      un sous_ensemble */

    return subset_nb;
}


bool
sxsubset_unset (sxsubset_header *h, SXINT subset_nb)
{
    /* Le sous-ensemble "subset_nb" redevient un simple SXBA.
       Retourne true ssi il figure ds le pool des sous-ensembles. */
    SXINT x, *p;

    for (p = h->hash_tbl + (h->display [subset_nb].scrmbl % h->hash_size);
	 (x = *p) >= 0;
	 p = &(h->display [x].lnk))
    {
	if (x == subset_nb)
	{
	    *p = h->display [subset_nb].lnk;
	    h->display [subset_nb].lnk = -2 /* Ce n'est plus un sous_ensemble */;
	    break;
	}
    }

    return x == subset_nb;
}


SXINT
sxsubset_set (sxsubset_header *h, SXINT subset_nb)
{
    /* subset_nb est un identifiant (entier positif ou nul) d'un ensemble,
       retourne par sxsubset_seek,
       manipule par les SXBA (via sxsubset_to_sxba) que l'on veut entrer ds la structure.
       Le resultat est l'identifiant de ce sous-ensemble ds la structure.
       Si resultat == subset_nb => c'est un nouveau, sinon il existait deja et on peut
       donc le liberer via sxsubset_erase(h,subset). */
    SXBA		subset = h->subsets [subset_nb], ss;
    SXINT			nb_slices = SXNBLONGS (sxsubset_card (*h));
    SXUINT	scr, scrmbl = 0;
    SXINT			i, x, *p;

    for (i = nb_slices; i > 0; i--)
	scrmbl ^= subset [i];

    for (p = h->hash_tbl + (scrmbl % h->hash_size);
	 (x = *p) >= 0 && (scr = h->display [x].scrmbl) <= scrmbl;
	 p = &(h->display [x].lnk))
    {
	if (scr == scrmbl)
	{
	    ss = h->subsets [x];

	    for (i = nb_slices; i > 0; i--)
		if (ss [i] != subset [i])
		    break;

	    if (i == 0)
		return x;
	}
    }

    h->display [subset_nb].lnk = x;
    h->display [subset_nb].scrmbl = scrmbl;

    return *p = subset_nb;
}

   

#define WRITE(p,l)	((bytes=(l))>0&&(write(file_descr, p, (size_t)bytes) == bytes))
#define READ(p,l)	((bytes=(l))>0&&(read (file_descr, p, (size_t)bytes) == bytes))
static SXINT	bytes;

bool
sxsubset_write (sxsubset_header *header, sxfiledesc_t file_descr)
{
    SXINT cardinal = SXBASIZE (header->subsets [0]);

    return
	sxindex_write (&(header->subset_hd), file_descr)
	&& WRITE (&(header->hash_size), sizeof (SXINT))
	&& WRITE (&cardinal, sizeof (SXINT))
	&& sxbm_write (file_descr, header->subsets, sxindex_size (header->subset_hd))
	&& WRITE (header->hash_tbl, sizeof (SXINT) * (header->hash_size))
	&& WRITE (header->display, sizeof (struct subset_display) *
		  sxindex_size (header->subset_hd))
	;
}


bool
sxsubset_read (sxsubset_header *header,
	       sxfiledesc_t file_descr,
	       char *name,
	       SXINT (*subset_oflw) (SXINT, SXINT), 
	       SXINT (*elem_oflw) (SXINT, SXINT),
	       FILE *stat_file)
{
    SXINT cardinal;

    bool b_ret =
	sxindex_read (&(header->subset_hd), file_descr, (sxoflw2_t) NULL)
	&& READ (&(header->hash_size), sizeof (SXINT))
	&& READ (&cardinal, sizeof (SXINT))
	&& sxbm_read (file_descr,
		      header->subsets = sxbm_calloc (sxindex_size (header->subset_hd), cardinal),
		      sxindex_size (header->subset_hd))
	&& READ (header->hash_tbl = (SXINT*) sxalloc (header->hash_size, sizeof (SXINT)),
		 sizeof (SXINT) * (header->hash_size))
	&& READ (header->display =
		 (struct subset_display*) sxalloc (sxindex_size (header->subset_hd),
						   sizeof (struct subset_display)),
		 sizeof (struct subset_display) * sxindex_size (header->subset_hd));
    sxuse(name); /* pour faire taire gcc -Wunused */
    if ( b_ret ) {
      header->subset_oflw = subset_oflw;
      header->elem_oflw = elem_oflw;
      header->stat_file = stat_file;
    }
    return b_ret;
}

static sxsubset_header	*_p_hd;
static FILE             *_file;


static void
print_display (SXINT x)
{
    if (sxindex_is_released (_p_hd->subset_hd, x))
	fprintf (_file, "{0, 0}, ");
    else
	fprintf (_file, "{%ld, %ld}, ",
	 (SXINT) _p_hd->display [x].lnk,    
	 (SXINT) _p_hd->display [x].scrmbl);
}


#define SXSUBSET_STR1_SIZE_	128
#define SXSUBSET_STR2_SIZE_	14
static char			sxsubset_str_ [SXSUBSET_STR1_SIZE_ + SXSUBSET_STR2_SIZE_];
static SXINT			sxsubset_str_lgth_;
#define sxsubset_init_str(s)	(((sxsubset_str_lgth_ = strlen (s)) > SXSUBSET_STR1_SIZE_)	\
                                   ? sxsubset_str_lgth_ = SXSUBSET_STR1_SIZE_ : 0,		\
                                memcpy (sxsubset_str_, s, (size_t)sxsubset_str_lgth_))
#define sxsubset_cat_str(s,n)	memcpy (sxsubset_str_ + sxsubset_str_lgth_, s, n)

static void
sxsubset_out_tab_int (FILE *file, char *name, SXINT *tab, SXINT size)
{
    SXINT i;

    fprintf (file, "\n\nstatic SXINT %s [%ld] = {", name, (SXINT) size);

    for (i = 0; i < size; i++) {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %ld */ ", (SXINT) i);

	fprintf (file, "%ld, ", (SXINT) tab [i]);
    } 

    fprintf (file, "\n} /* End %s */;\n", name);
}


static void
sxsubset_out_tab_struct (FILE *file,
			 char *struct_name,
			 char *name,
			 void (*print) (SXINT),
			 SXINT size, SXINT top)
{
    SXINT i;

    fprintf (file, "\n\nstatic %s %s [%ld] = {", struct_name, name, (SXINT) size);

    for (i = 0; i < top; i++) {
	if ((i & 3) == 0)
	    fprintf (file, "\n/* %ld */ ", (SXINT) i);

	print (i);
    } 

    fprintf (file, "\n} /* End %s %s */;\n", struct_name, name);
}

void
sxsubset_array_to_c (sxsubset_header *header, FILE *file, char *name)
{
    _p_hd = header;
    _file = file;

    sxsubset_init_str (name);
    sxsubset_cat_str ("_subset_hd", 11);
    sxindex_array_to_c (&(header->subset_hd), file, sxsubset_str_);

    sxbm_to_c (file, header->subsets, sxindex_size (header->subset_hd), name,
	       "_subsets", true);

    sxsubset_cat_str ("_hash_tbl", 10);
    sxsubset_out_tab_int (file, sxsubset_str_, header->hash_tbl, header->hash_size);

    sxsubset_cat_str ("_display", 9);
    sxsubset_out_tab_struct (file, "struct subset_display", sxsubset_str_, print_display,
		    sxindex_size (header->subset_hd), sxindex_top (header->subset_hd));
}


void
sxsubset_header_to_c (sxsubset_header *header, FILE *file, char *name)
{
    fprintf (file, "{\n");

    sxsubset_init_str (name);
    sxsubset_cat_str ("_subset_hd", 11);
    sxindex_header_to_c (&(header->subset_hd), file, sxsubset_str_);
    fprintf (file, " /* End subset_hd */,\n");

    fprintf (file, "%s_subsets /* subsets */,\n", name);

    fprintf (file, "%s_hash_tbl /* hash_tbl */,\n", name);

    fprintf (file, "%s_display /* display */,\n", name);

    fprintf (file, "%ld /* hash_size */,\n", (SXINT) header->hash_size);

    fprintf (file, "NULL /* (*subset_oflw) () */,\n");

    fprintf (file, "NULL /* (*elem_oflw) () */\n");

    fprintf (file, "NULL /* stat_file */\n");

    fprintf (file, "}");
}



void
sxsubset_to_c (sxsubset_header *header, FILE *file, char *name, bool is_static)
{
    sxsubset_array_to_c (header, file, name);
    fprintf (file, "\n\n%ssxsubset_header %s =\n", is_static ? "static " : "", name);
    sxsubset_header_to_c (header, file, name);
    fprintf (file, ";\n");
}



void
sxsubset_reuse (sxsubset_header *header,
		SXINT (*user_subset_oflw) (SXINT, SXINT), 
		SXINT (*user_elem_oflw) (SXINT, SXINT),
		FILE *stat_file)
{
    header->subset_oflw = user_subset_oflw;
    header->elem_oflw = user_elem_oflw;
    header->stat_file = stat_file;
}



