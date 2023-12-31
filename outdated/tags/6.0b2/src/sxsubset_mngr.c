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
   *     Produit de l'equipe ChLoE.     		  *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030512 10:59 (phd):	Adaptation � SGI (64 bits)		*/
/************************************************************************/
/* 20030312 13:32 (phd):	READ(cardinal...) => READ(&cardinal...)	*/
/************************************************************************/
/* 30-03-94 10:23 (pb):		Remplacement de strncpy par memcpy	*/
/************************************************************************/
/* 21-03-94 17:18 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/



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

#include        "sxcommon.h"
#include	"sxsubset.h"
#include	"sxalloc.h"
#include	<memory.h>
#include        <unistd.h>
#include        <string.h>

char WHAT_SXSUBSET_MNGR[] = "@(#)SYNTAX - $Id: sxsubset_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

extern int sxnext_prime (int germe);

static void
sxsubset_subset_oflw (subset_hd, old_line_nb, old_size)
    sxindex_header	*subset_hd;
    int			old_line_nb, old_size;
{
    int new_size = sxindex_size (_sxsubset_header->subset_hd);
    int cardinal = sxsubset_card (*_sxsubset_header);
    int *p, *q, i, x;
    unsigned int scrmbl;

    _sxsubset_header->hash_size = sxnext_prime (_sxsubset_header->hash_size);

    if (sxindex_is_static (_sxsubset_header->subset_hd))
    {
	SXBA	*old_subsets = _sxsubset_header->subsets;
	char *old = (char*) _sxsubset_header->display;

	_sxsubset_header->hash_tbl = (int*) sxalloc (_sxsubset_header->hash_size, sizeof (int));
	
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
	_sxsubset_header->hash_tbl = (int*) sxrealloc (_sxsubset_header->hash_tbl,
						       _sxsubset_header->hash_size,
						       sizeof (int));

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
sxsubset_elem_oflw (h, new_cardinal)
    sxsubset_header	*h;
    int			new_cardinal;
{
    int size = sxindex_size (h->subset_hd);
    int old_cardinal = sxsubset_card (*h);

    if (sxindex_is_static (h->subset_hd)) {
	int	slices = NBLONGS (old_cardinal);
	SXBA	*old_subsets = h->subsets, s1, s2;
	int	x, i;

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
sxsubset_alloc (h,
		subset_nb,
		cardinal,
		user_subset_oflw,
		user_elem_oflw,
		stat_file)
    sxsubset_header	*h;
    int 		subset_nb, cardinal;
    int			(*user_subset_oflw) (), (*user_elem_oflw) ();
    FILE		*stat_file;
{
    int size, *p;

    sxindex_alloc (&(h->subset_hd), 0, subset_nb, sxsubset_subset_oflw);
    size = sxindex_size (h->subset_hd);

    h->hash_size = sxnext_prime (subset_nb);
    h->hash_tbl = (int*) sxalloc (h->hash_size, sizeof (int));

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
sxsubset_stat (stat_file, h)
    FILE	*stat_file;
    sxsubset_header	*h;
{
    int total_sizeof, used_sizeof;
    int size = sxindex_size (h->subset_hd);
    int top = sxindex_top (h->subset_hd);

    total_sizeof = used_sizeof = sizeof (sxsubset_header) +
	    sizeof (int) * (h->hash_size) ;

    total_sizeof += sxindex_total_sizeof (h->subset_hd) +
	size * (SXBA_total_sizeof (h->subsets [0]) + sizeof (SXBA*)) +
	    size * sizeof (struct subset_display);

    used_sizeof += sxindex_used_sizeof (h->subset_hd) +
	top * (SXBA_used_sizeof (h->subsets [0]) + sizeof (SXBA*)) +
	    top * sizeof (struct subset_display);

    fprintf (stat_file, "sxsubset_header: Total_sizeof (byte) = %d (%d%%)\n",
	     total_sizeof, (100*used_sizeof)/total_sizeof);
}

void
sxsubset_free (h)
   sxsubset_header	*h;
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

 
int
sxsubset_seek (h)
    sxsubset_header	*h;
{
    /* Retourne l'identifiant entier d'un nouvel ensemble manipulable par les
       SXBA via sxsubset_to_sxba */
    int			subset_nb;

    _sxsubset_header = h; /* Pour les oflw */

    h->display [subset_nb = sxindex_seek (&(h->subset_hd))].lnk = -2; /* Ce n'est (pas encore)
								      un sous_ensemble */

    return subset_nb;
}


BOOLEAN
sxsubset_unset (h, subset_nb)
    sxsubset_header	*h;
    int			subset_nb;
{
    /* Le sous-ensemble "subset_nb" redevient un simple SXBA.
       Retourne TRUE ssi il figure ds le pool des sous-ensembles. */
    int x, *p;

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


int
sxsubset_set (h, subset_nb)
    sxsubset_header	*h;
    int			subset_nb;
{
    /* subset_nb est un identifiant (entier positif ou nul) d'un ensemble,
       retourne par sxsubset_seek,
       manipule par les SXBA (via sxsubset_to_sxba) que l'on veut entrer ds la structure.
       Le resultat est l'identifiant de ce sous-ensemble ds la structure.
       Si resultat == subset_nb => c'est un nouveau, sinon il existait deja et on peut
       donc le liberer via sxsubset_erase(h,subset). */
    SXBA		subset = h->subsets [subset_nb], ss;
    int			nb_slices = NBLONGS (sxsubset_card (*h));
    unsigned int	scr, scrmbl = 0;
    int			i, x, *p;

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
static int	bytes;

BOOLEAN
sxsubset_write (header, file_descr)
    sxsubset_header	*header;
    int			file_descr;
{
    int cardinal = BASIZE (header->subsets [0]);

    return
	sxindex_write (&(header->subset_hd), file_descr)
	&& WRITE (&(header->hash_size), sizeof (int))
	&& WRITE (&cardinal, sizeof (int))
	&& sxbm_write (file_descr, header->subsets, sxindex_size (header->subset_hd))
	&& WRITE (header->hash_tbl, sizeof (int) * (header->hash_size))
	&& WRITE (header->display, sizeof (struct subset_display) *
		  sxindex_size (header->subset_hd))
	;
}


BOOLEAN
sxsubset_read (header, file_descr, name, subset_oflw, elem_oflw, stat_file)
    sxsubset_header	*header;
    int			file_descr;
    char		*name;
    int			(*subset_oflw) (), (*elem_oflw) ();
    FILE		*stat_file;
{
    int cardinal;

    BOOLEAN b_ret =
	sxindex_read (&(header->subset_hd), file_descr)
	&& READ (&(header->hash_size), sizeof (int))
	&& READ (&cardinal, sizeof (int))
	&& sxbm_read (file_descr,
		      header->subsets = sxbm_calloc (sxindex_size (header->subset_hd), cardinal),
		      sxindex_size (header->subset_hd))
	&& READ (header->hash_tbl = (int*) sxalloc (header->hash_size, sizeof (int)),
		 sizeof (int) * (header->hash_size))
	&& READ (header->display =
		 (struct subset_display*) sxalloc (sxindex_size (header->subset_hd),
						   sizeof (struct subset_display)),
		 sizeof (struct subset_display) * sxindex_size (header->subset_hd));
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
print_display (x)
    int x;
{
    if (sxindex_is_released (_p_hd->subset_hd, x))
	fprintf (_file, "{0, 0}, ");
    else
	fprintf (_file, "{%i, %i}, ",
	 _p_hd->display [x].lnk,    
	 _p_hd->display [x].scrmbl);
}


#define SXSUBSET_STR1_SIZE_	128
#define SXSUBSET_STR2_SIZE_	14
static char			sxsubset_str_ [SXSUBSET_STR1_SIZE_ + SXSUBSET_STR2_SIZE_];
static int			sxsubset_str_lgth_;
#define sxsubset_init_str(s)	(((sxsubset_str_lgth_ = strlen (s)) > SXSUBSET_STR1_SIZE_)	\
                                   ? sxsubset_str_lgth_ = SXSUBSET_STR1_SIZE_ : 0,		\
                                memcpy (sxsubset_str_, s, (size_t)sxsubset_str_lgth_))
#define sxsubset_cat_str(s,n)	memcpy (sxsubset_str_ + sxsubset_str_lgth_, s, n)

static void
sxsubset_out_tab_int (file, name, tab, size)
    FILE	*file;
    char	*name;
    int		*tab, size;
{
    int i;

    fprintf (file, "\n\nstatic int %s [%i] = {", name, size);

    for (i = 0; i < size; i++) {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %i */ ", i);

	fprintf (file, "%i, ", tab [i]);
    } 

    fprintf (file, "\n} /* End %s */;\n", name);
}


static void
sxsubset_out_tab_struct (file, struct_name, name, print, size, top)
    FILE	*file;
    char	*struct_name, *name;
    void	(*print) ();
    int		size, top;
{
    int i;

    fprintf (file, "\n\nstatic %s %s [%i] = {", struct_name, name, size);

    for (i = 0; i < top; i++) {
	if ((i & 3) == 0)
	    fprintf (file, "\n/* %i */ ", i);

	print (i);
    } 

    fprintf (file, "\n} /* End %s %s */;\n", struct_name, name);
}

void
sxsubset_array_to_c (header, file, name)
    sxsubset_header	*header;
    FILE		*file;
    char		*name;
{
    _p_hd = header;
    _file = file;

    sxsubset_init_str (name);
    sxsubset_cat_str ("_subset_hd", 11);
    sxindex_array_to_c (&(header->subset_hd), file, sxsubset_str_);

    sxbm_to_c (file, header->subsets, sxindex_size (header->subset_hd), name,
	       "_subsets", TRUE);

    sxsubset_cat_str ("_hash_tbl", 10);
    sxsubset_out_tab_int (file, sxsubset_str_, header->hash_tbl, header->hash_size);

    sxsubset_cat_str ("_display", 9);
    sxsubset_out_tab_struct (file, "struct subset_display", sxsubset_str_, print_display,
		    sxindex_size (header->subset_hd), sxindex_top (header->subset_hd));
}


void
sxsubset_header_to_c (header, file, name)
    sxsubset_header	*header;
    FILE		*file;
    char		*name;
{
    fprintf (file, "{\n");

    sxsubset_init_str (name);
    sxsubset_cat_str ("_subset_hd", 11);
    sxindex_header_to_c (&(header->subset_hd), file, sxsubset_str_);
    fprintf (file, " /* End subset_hd */,\n");

    fprintf (file, "%s_subsets /* subsets */,\n", name);

    fprintf (file, "%s_hash_tbl /* hash_tbl */,\n", name);

    fprintf (file, "%s_display /* display */,\n", name);

    fprintf (file, "%i /* hash_size */,\n", header->hash_size);

    fprintf (file, "NULL /* (*subset_oflw) () */,\n");

    fprintf (file, "NULL /* (*elem_oflw) () */\n");

    fprintf (file, "NULL /* stat_file */\n");

    fprintf (file, "}");
}



void
sxsubset_to_c (header, file, name, is_static)
    sxsubset_header	*header;
    FILE		*file;
    char		*name;
    BOOLEAN		is_static;
{
    sxsubset_array_to_c (header, file, name);
    fprintf (file, "\n\n%ssxsubset_header %s =\n", is_static ? "static " : "", name);
    sxsubset_header_to_c (header, file, name);
    fprintf (file, ";\n");
}



void
sxsubset_reuse (header, user_subset_oflw, user_elem_oflw, stat_file)
    sxsubset_header	*header;
    int			(*user_subset_oflw) (), (*user_elem_oflw) ();
    FILE		*stat_file;
{
    header->subset_oflw = user_subset_oflw;
    header->elem_oflw = user_elem_oflw;
    header->stat_file = stat_file;
}



