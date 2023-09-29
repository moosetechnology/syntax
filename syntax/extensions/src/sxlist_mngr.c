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

/* Fonctionnalites similaires aux packages "XH" et "XT".

   Une liste est identifie'e par un entier positif ou nul.
   Une liste de longueur n occupe a peu pres (8.03 + 2.03n) cellules.

   Comparaison (sur 10000 listes aleatoires de taille max 200 prenant leurs
                elements sur [0..499]):
                XH		XT		sxlist
   Temps	1		6		3
   Place	1		4		2

   Avantages :
      - Recupere effectivement la place occupee par les listes qui ont ete
        "erased", sans changer les identifiants des listes actives.
      - Supporte la construction simultanee de plusieurs listes.

   Inconvenients :
      - Ne partage pas les prefixes des listes
      - Plus gourmand en place et temps que XH.
*/

#define SX_GLOBAL_VAR_LIST_HEADER

#include        "sxversion.h"
#include        "sxcommon.h"
#include	"sxlist.h"
#include	"SS.h"
#include	"sxalloc.h"
#include 	<memory.h>
#include        <unistd.h>
#include        <string.h>

char WHAT_SXLIST_MNGR[] = "@(#)SYNTAX - $Id: sxlist_mngr.c 3312 2023-05-28 06:06:26Z garavel $" WHAT_DEBUG;


static void
sxlist_list_oflw (sxindex_header *list_hd, SXINT old_line_nb, SXINT old_size)
{
    SXINT new_size = sxindex_size (*list_hd);

    sxuse(old_line_nb); /* pour faire taire gcc -Wunused */
    if (sxindex_is_static (_sxlist_header->list_hd)) {
	char *old = (char*) _sxlist_header->list_to_attr;

	memcpy ((char*)(_sxlist_header->list_to_attr =
			 (struct sxlist_to_attr*) sxalloc (new_size + 1,
							   sizeof (struct sxlist_to_attr))),
		 old,
		 (old_size + 1) * sizeof (struct sxlist_to_attr));
    }
    else
	_sxlist_header->list_to_attr = (struct sxlist_to_attr*)
	    sxrealloc (_sxlist_header->list_to_attr,
		       new_size + 1,
		       sizeof (struct sxlist_to_attr));

    if (_sxlist_header->list_oflw != NULL)
	(*_sxlist_header->list_oflw) (old_size, new_size);
}

static void
sxlist_hd_oflw (SXINT old_size, SXINT new_size)
{
    if (X_is_static (_sxlist_header->hd)) {
	SXINT *old = _sxlist_header->hd_to_list;

	X_root_int_cpy (old,
			_sxlist_header->hd_to_list = (SXINT*) sxalloc (new_size + 1,
								     sizeof (SXINT)),
			old_size);
    }
    else
	_sxlist_header->hd_to_list = (SXINT*) sxrealloc (_sxlist_header->hd_to_list,
						       new_size + 1,
						       sizeof (SXINT));
}

static void
sxlist_elem_oflw (sxindex_header *elem_hd, SXINT old_line_nb, SXINT old_size)
{
    SXINT new_size = sxindex_size (*elem_hd);

    sxuse(old_line_nb); /* pour faire taire gcc -Wunused */
    if (sxindex_is_static (_sxlist_header->elem_hd)) {
	char *old = (char*) _sxlist_header->buckets;

	memcpy ((char*)(_sxlist_header->buckets = (struct sxlist_bucket*)
			 sxalloc (new_size + 1,
				  sizeof (struct sxlist_bucket))),
		 old,
		 (old_size + 1) * sizeof (struct sxlist_to_attr));
    }
    else
	_sxlist_header->buckets = (struct sxlist_bucket*)
	    sxrealloc (_sxlist_header->buckets,
		       new_size + 1,
		       sizeof (struct sxlist_bucket));

    if (_sxlist_header->elem_oflw != NULL)
	(*_sxlist_header->elem_oflw) (old_size, new_size);
}

void
sxlist_alloc (sxlist_header *h,
	      char *name,
	      SXINT init_elem_nb,
	      SXINT average_list_nb_per_bucket,
	      SXINT average_list_size,
	      SXINT (*user_list_oflw) (SXINT, SXINT), 
	      SXINT (*user_elem_oflw) (SXINT, SXINT),
	      FILE *stat_file)
{
    sxindex_alloc (&(h->list_hd), 0, init_elem_nb, sxlist_list_oflw);
    h->list_to_attr = (struct sxlist_to_attr*)
	sxalloc (sxindex_size (h->list_hd) + 1, sizeof (struct sxlist_to_attr));

    X_alloc (&(h->hd), name, sxindex_size (h->list_hd),
	     average_list_nb_per_bucket, sxlist_hd_oflw, stat_file);
    h->hd_to_list = (SXINT*) sxalloc (X_size (h->hd) + 1, sizeof (SXINT));

    sxindex_alloc (&(h->elem_hd), 0, init_elem_nb * average_list_size, sxlist_elem_oflw);
    h->buckets = (struct sxlist_bucket*)
	sxalloc (sxindex_size (h->elem_hd) + 1, sizeof (struct sxlist_bucket));

    h->list_oflw = user_list_oflw;
    h->elem_oflw = user_elem_oflw;

    sxindex_seek (&(h->elem_hd)); /* L'element 0 est reserve' pour indiquer la fin
				     de la liste courante. */
    h->stat_file = stat_file;
}

void
sxlist_stat (FILE *stat_file, sxlist_header *header)
{
    SXINT total_sizeof, used_sizeof;

    total_sizeof = used_sizeof = sizeof (sxlist_header);
    total_sizeof += X_total_sizeof (header->hd) +
	sizeof (SXINT)*(X_size (header->hd) + 1) +
	    sxindex_total_sizeof(header->list_hd) +
		sizeof (struct sxlist_to_attr) * (sxindex_size (header->list_hd) + 1) +
	    sxindex_total_sizeof(header->elem_hd) +
		sizeof (struct sxlist_bucket) * (sxindex_size (header->elem_hd) + 1);

    used_sizeof += X_used_sizeof (header->hd) +
	sizeof (SXINT)*(X_top (header->hd) + 1) +
	    sxindex_used_sizeof(header->list_hd) +
		sizeof (struct sxlist_to_attr) * (sxindex_top (header->list_hd) + 1) +
	    sxindex_used_sizeof(header->elem_hd) +
		sizeof (struct sxlist_bucket) * (sxindex_top (header->elem_hd) + 1);

    fprintf (stat_file, "sxlist_header: Total_sizeof (byte) = %ld (%ld%%)\n",
	     (SXINT) total_sizeof, (SXINT) (100*used_sizeof)/total_sizeof);
}

void
sxlist_free (sxlist_header *h)
{
    if (h->stat_file != NULL)
	sxlist_stat (h->stat_file, h);

    if (!sxindex_is_static (h->elem_hd))
    {
	sxfree (h->buckets);
	sxindex_free (h->elem_hd);
    }

    if (!X_is_static (h->hd))
    {
	sxfree (h->hd_to_list);
	X_free (&(h->hd));
    }

    if (!sxindex_is_static (h->list_hd))
    {
	sxfree (h->list_to_attr);
	sxindex_free (h->list_hd);
    }
}

 
SXBOOLEAN
sxlist_set (sxlist_header *h, SXINT scrmbl, SXINT *elem_stack, SXINT *result)
{
    /* Cree la liste courante contenue dans la SS_stack "elem_stack" de scramble "scrmbl". */
    /* Retourne son identifiant "result". */
    /* Retourne SXTRUE ssi la liste est nouvelle */
    SXINT				r, nl, in, *pp, list_lgth, xelem;
    struct sxlist_to_attr	*pn;

    _sxlist_header = h; /* Pour les oflw */

    list_lgth = SS_top (elem_stack) - (xelem = SS_bot (elem_stack));

    if (X_set (&(h->hd), scrmbl, &r))
    {
	/* Le scramble existe deja */
	for (nl = *(pp = &(h->hd_to_list [r]));
	     nl > 0;
	     nl = *(pp = &(pn->next_list)))
	{
	    pn = &(h->list_to_attr [nl]);
	    
	    if (pn->list_lgth >= list_lgth)
	    {
		if (pn->list_lgth > list_lgth)
		    /* nouveau */
		    break;

		for (in = pn->first_elem;
		     in > 0;
		     in = h->buckets [in].next, xelem++)
		{
		    if (SS_get (elem_stack, xelem) != h->buckets [in].elem)
			/* nouveau */
			break;
		}

		if (in <= 0) {
		    /* ancien */
		    *result = nl;
		    SS_clear (elem_stack);
		    return SXFALSE;
		}
	    }
	}
    }
    else
    {
	*(pp = &(h->hd_to_list [r])) = 0;
    }

    /* nouveau */
    nl = sxindex_seek (&(h->list_hd));
    pn = &(h->list_to_attr [nl]);
    pn->next_list = *pp;
    *pp = nl;
    pn->scrmbl = scrmbl;
    pn->list_lgth = list_lgth;
    r = 0;

    while (!SS_is_empty (elem_stack)) {
        in = sxindex_seek (&(h->elem_hd));
	h->buckets [in].elem = SS_pop (elem_stack);
	h->buckets [in].next = r;
	r = in;
    }

    pn->first_elem = r;
    *result = nl;
    return SXTRUE;
}

void
sxlist_erase (sxlist_header *h, SXINT l)
{
    /* Supprime la liste "l" et recupere la place correspondante. */
    SXINT				r, nl, i, *pp;
    struct sxlist_to_attr	*p = &(h->list_to_attr [l]);

    if ((r = X_is_set (&(h->hd), p->scrmbl)) > 0)
    {
	/* On commence par la rechercher. */
	for (nl = *(pp = &(h->hd_to_list [r]));
	     nl > 0;
	     nl = *(pp = &(h->list_to_attr [nl].next_list)))
	{
	    if (nl == l)
	    {
		/* On l'a retrouvee */
		*pp = p->next_list;

		if (h->hd_to_list [r] == 0)
		    /* C'etait la seule liste qui avait p->scrmbl */
		    X_erase (h->hd, r);

		/* On recupere les elements de la liste */
		for (i = p->first_elem;
		     i > 0;
		     i = h->buckets [i].next)
		    sxindex_release (h->elem_hd, i);

		/* ... et la liste */
		sxindex_release (h->list_hd, l);

		break;
	    }
	}
    }
}
    

#define WRITE(p,l)	((bytes=(l))>0&&(write(file_descr, p, (size_t)bytes) == bytes))
#define READ(p,l)	((bytes=(l))>0&&(read (file_descr, p, (size_t)bytes) == bytes))
static SXINT	bytes;

SXBOOLEAN
sxlist_write (sxlist_header *header, sxfiledesc_t file_descr)
{
    return
	X_write (&(header->hd), file_descr)
	&& WRITE (header->hd_to_list, sizeof (SXINT) * (X_top (header->hd) + 1))
	&& sxindex_write (&(header->list_hd), file_descr)
	&& WRITE (header->list_to_attr, sizeof (struct sxlist_to_attr) * (sxindex_size (header->list_hd)))
	&& sxindex_write (&(header->elem_hd), file_descr)
	&& WRITE (header->buckets, sizeof (struct sxlist_bucket) * (sxindex_size (header->elem_hd)))
	;
}


SXBOOLEAN
sxlist_read (sxlist_header *header,
	     sxfiledesc_t file_descr,
	     char *name,
	     SXINT (*list_oflw) (SXINT, SXINT),
	     SXINT (*elem_oflw) (SXINT, SXINT),
	     FILE *stat_file)
{
    SXBOOLEAN b_ret =
	X_read (&(header->hd), file_descr, name, sxlist_hd_oflw, stat_file)
	&& READ (header->hd_to_list = (SXINT*) sxalloc (X_size (header->hd) + 1, sizeof (SXINT)),
		 sizeof (SXINT) * (X_top (header->hd) + 1))
	&& sxindex_read (&(header->list_hd), file_descr, (sxoflw2_t) NULL)
	&& READ (header->list_to_attr =
		 (struct sxlist_to_attr*) sxalloc (sxindex_size (header->list_hd),
						   sizeof (struct sxlist_to_attr)),
		 sizeof (struct sxlist_to_attr) * (sxindex_size (header->list_hd)))
	&& sxindex_read (&(header->elem_hd), file_descr, (sxoflw2_t) NULL)
	&& READ (header->buckets =
		 (struct sxlist_bucket*) sxalloc (sxindex_size (header->elem_hd),
						  sizeof (struct sxlist_bucket)),
		 sizeof (struct sxlist_bucket) * (sxindex_size (header->elem_hd)));
	if ( b_ret ) {
	  header->list_oflw = list_oflw;
	  header->elem_oflw = elem_oflw;
	  header->stat_file = stat_file;
	}
	return b_ret;
}

static sxlist_header	*_p_hd;
static FILE             *_file;

static void
print_list_to_attr (SXINT x)
{
    if (sxindex_is_released (_p_hd->list_hd, x))
	fprintf (_file, "{0, 0, 0, 0}, ");
    else
	fprintf (_file, "{%ld, %ld, %ld, %ld}, ",
		 (SXINT) _p_hd->list_to_attr [x].next_list,    
		 (SXINT) _p_hd->list_to_attr [x].scrmbl,    
		 (SXINT) _p_hd->list_to_attr [x].list_lgth,    
		 (SXINT) _p_hd->list_to_attr [x].first_elem);    
}


static void
print_buckets (SXINT x)
{
    if (sxindex_is_released (_p_hd->elem_hd, x))
	fprintf (_file, "{0, 0}, ");
    else
	fprintf (_file, "{%ld, %ld}, ",
	 (SXINT) _p_hd->buckets [x].next,    
	 (SXINT) _p_hd->buckets [x].elem);    
}


#define SXLIST_STR1_SIZE_	128
#define SXLIST_STR2_SIZE_	14
static char			sxlist_str_ [SXLIST_STR1_SIZE_ + SXLIST_STR2_SIZE_];
static SXINT			sxlist_str_lgth_;
#define sxlist_init_str(s)	(((sxlist_str_lgth_ = strlen (s)) > SXLIST_STR1_SIZE_)		\
                                   ? sxlist_str_lgth_ = SXLIST_STR1_SIZE_ : 0,			\
                                memcpy (sxlist_str_, s, (size_t)sxlist_str_lgth_))
#define sxlist_cat_str(s,n)	memcpy (sxlist_str_ + sxlist_str_lgth_, s, n)

static void
sxlist_out_tab_int (FILE *file, char *name, SXINT *tab, SXINT size, SXINT top)
{
    SXINT i;

    fprintf (file, "\n\nstatic SXINT %s [%ld] = {", name, (SXINT) size + 1);

    for (i = 0; i <= top; i++) {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %ld */ ", (SXINT) i);

	fprintf (file, "%ld, ", (SXINT) tab [i]);
    } 

    fprintf (file, "\n} /* End %s */;\n", name);
}

static void
sxlist_out_tab_struct (FILE *file,
		       char *struct_name,
		       char *name,
		       void (*print) (SXINT),
		       SXINT size,
		       SXINT top)
{
    SXINT i;

    fprintf (file, "\n\nstatic %s %s [%ld] = {", struct_name, name, (SXINT) size + 1);

    for (i = 0; i <= top; i++) {
	if ((i & 3) == 0)
	    fprintf (file, "\n/* %ld */ ", (SXINT) i);

	print (i);
    } 

    fprintf (file, "\n} /* End %s %s */;\n", struct_name, name);
}

void
sxlist_array_to_c (sxlist_header *header, FILE *file, char *name)
{
    _p_hd = header;
    _file = file;

    sxlist_init_str (name);
    sxlist_cat_str ("_hd", 4);

    X_array_to_c (&(header->hd), file, sxlist_str_);

    sxlist_cat_str ("_hd_to_list", 12);
    sxlist_out_tab_int (file, sxlist_str_, header->hd_to_list,
		 X_size (header->hd), X_top (header->hd));
    sxlist_cat_str ("_list_hd", 9);
    sxindex_array_to_c (&(header->list_hd), file, sxlist_str_);
    sxlist_cat_str ("_list_to_attr", 14);
    sxlist_out_tab_struct (file, "struct sxlist_to_attr", sxlist_str_, print_list_to_attr,
		    sxindex_size (header->list_hd) - 1, sxindex_top (header->list_hd) - 1);
    sxlist_cat_str ("_elem_hd", 9);
    sxindex_array_to_c (&(header->elem_hd), file, sxlist_str_);
    sxlist_cat_str ("_buckets", 9);
    sxlist_out_tab_struct (file, "struct sxlist_bucket", sxlist_str_, print_buckets,
		    sxindex_size (header->elem_hd) - 1, sxindex_top (header->elem_hd) - 1);
}


void
sxlist_header_to_c (sxlist_header *header, FILE *file, char *name)
{
    sxlist_init_str (name);
    sxlist_cat_str ("_hd", 4);

    fprintf (file, "{\n");

    X_header_to_c (&(header->hd), file, sxlist_str_);
    fprintf (file, " /* End X_header hd */,\n");

    fprintf (file, "%s_hd_to_list /* hd_to_list */,\n", name);

    sxlist_cat_str ("_list_hd", 9);
    sxindex_header_to_c (&(header->list_hd), file, sxlist_str_);
    fprintf (file, " /* End list_hd */,\n");

    fprintf (file, "%s_list_to_attr /* list_to_attr */,\n", name);

    sxlist_cat_str ("_elem_hd", 9);
    sxindex_header_to_c (&(header->elem_hd), file, sxlist_str_);
    fprintf (file, " /* End elem_hd */,\n");

    fprintf (file, "%s_buckets /* buckets */,\n", name);

    fprintf (file, "NULL /* (*list_oflw) () */,\n");

    fprintf (file, "NULL /* (*elem_oflw) () */,\n");

    fprintf (file, "NULL /* stat_file */\n");

    fprintf (file, "}");
}



void
sxlist_to_c (sxlist_header *header, FILE *file, char *name, SXBOOLEAN is_static)
{
    sxlist_array_to_c (header, file, name);
    fprintf (file, "\n\n%ssxlist_header %s =\n", is_static ? "static " : "", name);
    sxlist_header_to_c (header, file, name);
    fprintf (file, ";\n");
}



void
sxlist_reuse (sxlist_header *header,
	      char *name,
	      SXINT (*user_list_oflw) (SXINT, SXINT),
	      SXINT (*user_elem_oflw) (SXINT, SXINT),
	      FILE *stat_file)
{
    X_reuse (&(header->hd), name, sxlist_hd_oflw, stat_file);
    sxindex_reuse (&(header->list_hd), sxlist_list_oflw);
    sxindex_reuse (&(header->elem_hd), sxlist_elem_oflw);
    header->list_oflw = user_list_oflw;
    header->elem_oflw = user_elem_oflw;
}


