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

static char	ME [] = "XH_mngr";

#include        "sxversion.h"
#include	<stdio.h>
#include	"XH.h"
#include        "sxcommon.h"

char WHAT_XH_MNGR[] = "@(#)SYNTAX - $Id: XH_mngr.c 2947 2023-03-29 17:06:41Z garavel $" WHAT_DEBUG;

extern void sxtrap (char *caller, char *message);
extern SXINT  sxnext_prime (SXINT germe);

static void XH_sat_mess (FILE *stat_file, char *header_name, char *field_name, SXINT old_size, SXINT new_size)
{
    fprintf (stat_file, "%s: %s.%s of size %ld overflowed: reallocation with size %ld.\n", ME, header_name, field_name, (SXINT) old_size, (SXINT) new_size);
}


void	XH_alloc (XH_header *header, 
		  char *name, 
		  SXINT init_elem_nb, 
		  SXINT average_list_nb_per_bucket, 
		  SXINT average_list_size, 
		  void (*oflw) (SXINT, SXINT), 
		  FILE *stat_file)
{
    /* Cette fonction "cree" un adresseur extensible de listes d'entiers */
    header->name = name;
    header->hash_size = sxnext_prime (init_elem_nb / average_list_nb_per_bucket);
    header->hash_display = (SXINT*) sxcalloc ((SXUINT)(header->hash_size), sizeof (SXINT));
    header->display = (struct XH_elem*) sxalloc (init_elem_nb + 1, sizeof (struct XH_elem));
    header->display->lnk = 1;
    header->display->X = init_elem_nb;
    header->display [1].X = 1;
    header->list = (SXINT*) sxalloc ((header->list_size = average_list_size * init_elem_nb) + 1, sizeof (SXINT));
    header->list [0] = 1;
    header->scrmbl = 0;
    header->is_locked = SXFALSE;
    header->oflw = oflw;
    header->stat_file = stat_file;
    header->is_allocated = SXTRUE;
}

/* Les structures sont "statiques", on les alloue/copie en dynamique */ 
static void
XH_oflw_copy (XH_header *header)
{
  struct XH_elem* old_display = header->display, *display, *top_display;
  SXINT *old_hash_display = header->hash_display, *hash_display, *top_hash_display;
  SXINT *old_list = header->list, *list, *top_list;

  header->hash_display = hash_display = (SXINT*) sxcalloc ((SXUINT)(header->hash_size), sizeof (SXINT));
  header->display = display = (struct XH_elem*) sxalloc (header->display->X + 1, sizeof (struct XH_elem));
  header->list = list = (SXINT*) sxalloc (header->list_size + 1, sizeof (SXINT));

  top_hash_display = hash_display+header->hash_size;

  while (hash_display < top_hash_display) 
    *hash_display++ = *old_hash_display++;

  top_display = display+header->display->X;

  while (display <= top_display) 
    *display++ = *old_display++;

  top_list = list+header->list_size;

  while (list <= top_list) 
    *list++ = *old_list++;
}

SXBOOLEAN	XH_oflw (XH_header *header)
{
    SXINT old_size;

    if (header->is_locked)
	sxtrap ("XH_push", header->name);

    header->list_size = (old_size = header->list_size) * 2;

    if (header->stat_file != NULL) {
	XH_sat_mess (header->stat_file, header->name, "list", old_size, header->list_size);
    }

    if (!header->is_allocated) {
      /* On suppose que c'est un XH produit par XH2c dont les structures sont initialisees statiquement
	 et sur lesquelles on retravaille ... */
      /* Il faut aussi allouer hash_display et display */
      XH_oflw_copy (header);
      header->is_allocated = SXTRUE;
    }
    else
      header->list = (SXINT*) sxrealloc (header->list, header->list_size + 1, sizeof (SXINT));

    return SXTRUE;
}


void	XH_clear (XH_header *header)
{
    SXINT *top, *bot;

    if (header->display->lnk != 1) {
	top = (bot = header->hash_display) + header->hash_size;

	while (--top >= bot)
	    *top = 0;

	header->display->lnk = 1;
	header->display [1].X = 1;
	header->list [0] = 1;
	header->is_locked = SXFALSE;
    }
}



void	XH_stat (FILE *stat_file, XH_header *header)
{
    SXINT	x, l, n;
    SXUINT      i;
    SXINT			minl, maxl, meanl, hash_size, size, top, maxll, minll,
                        list_size, list_top, ll;
    SXUINT              total_size, total_top;

#define min(x,y)	((x)<=(y) ? (x) : (y))
#define max(x,y)	((x)>=(y) ? (x) : (y))

    hash_size = header->hash_size;
    size = header->display->X;
    list_size = XH_list_size (*header);

    if ((top = header->display->lnk) > 1) {
	n = 0;
	meanl = 0;
	maxll = maxl = 0;
	minl = size;
	minll = list_size;
	list_top = XH_list_top (*header);
	
	for (x = hash_size - 1; x >= 0; x--) {
	    if ((i = header->hash_display [x]) != 0) {
		n++;
		l = 0;
		
		do {
		    l++;
		    ll = XH_X (*header, i + 1) - XH_X (*header, i);
		    minll = min (minll, ll);
		    maxll = max (maxll, ll);
		} while ((i = header->display [i].lnk & XH_7F) != 0);
		
		minl = min (minl, l);
		maxl = max (maxl, l);
		meanl += l;
	    }
	}
	
	meanl /= n;
	total_size = total_top = sizeof (XH_header) + sizeof (SXINT) * hash_size;
	total_size += sizeof (struct XH_elem) * (size + 1) + sizeof (SXINT) * (list_size + 1);
	total_top += sizeof (struct XH_elem) * (top + 1) + sizeof (SXINT) * (list_top + 1);

	fprintf (stat_file, "name = %s, hash_size = %ld (%ld%%), size = %ld (%ld%%), minl = %ld, maxl = %ld, meanl = %ld, list_size = %ld (%ld%%), minll = %ld, maxll = %ld, meanll = %ld, Total_size (byte) = %ld (%ld%%)\n",
		 header->name, (SXINT) hash_size, (SXINT) (100*n)/hash_size, (SXINT) size, (SXINT) (100*top)/size, (SXINT) minl, (SXINT) maxl, (SXINT) meanl, (SXINT) list_size, (SXINT) (100*list_top)/list_size, (SXINT) minll, (SXINT) maxll, (SXINT) list_top/top, (SXINT) total_size, (SXINT) (100*total_top)/total_size);
    }
    else
	fprintf (stat_file, "name = %s, hash_size = %ld, size = %ld, list_size = %ld is empty.\n", header->name, (SXINT) hash_size, (SXINT) size, (SXINT) list_size);
}
    

void	XH_free (XH_header *header)
{
    if (header->stat_file != NULL)
	XH_stat (header->stat_file, header);
    
    sxfree (header->list), header->list = NULL;
    sxfree (header->display), header->display = NULL;
    sxfree (header->hash_display), header->hash_display = NULL;
    header->is_allocated = SXFALSE;
}



SXINT	XH_is_set (XH_header *header)
{
    /* Recherche si la liste
           header->list[header->display[header->display->lnk].X .. *(header->list)]
       existe par ailleurs */
    /* Retourne l'indice dans display de la liste ou 0 */
    SXINT		x, z;
    SXUINT	scrmbl;
    struct XH_elem	*aelem, *display = header->display;
    SXINT				xl = display [display->lnk].X;

    if ((x = header->hash_display [header->scrmbl % header->hash_size]) != 0) {
	SXINT		*bot1, *bot2, l;
	SXINT			length = *(header->list) - xl;
	SXINT			*bottom = header->list + xl;

	do {
	    if ((scrmbl = (aelem = display + x)->scrmbl) >= header->scrmbl) {
		if (scrmbl == header->scrmbl) {
		    if ((z = aelem [1].X - aelem->X) >= length) {
			if (z == length) {
			    bot1 = header->list + aelem->X;
			    bot2 = bottom;
			    l = length;

			    while (l-- > 0) {
				if ((z = *bot1++ - *bot2++) != 0)
				    break;
			    }

			    if (l < 0) {
				if ((aelem->lnk & XH_80) != 0) /* erased item */
				    x = 0;

				break;
			    }

			    if (z > 0) {
				x = 0;
				break;
			    }
			}
			else {
			    x = 0;
			    break;
			}
		    }
		}
		else {
		    x = 0;
		    break;
		}
	    }
	} while ((x = aelem->lnk & XH_7F) != 0);
    }

    *(header->list) = xl;
    header->scrmbl = 0;
    return x;
}

SXBOOLEAN	XH_set (XH_header *header, SXINT *result)
{
    /* Recherche si la liste
           header->list[header->display[header->display->lnk].X .. *(header->list)]
       existe par ailleurs */
    /* Retourne l'indice dans display de la [nouvelle] liste */
    SXINT		x, z, hash_value_counter;
    SXUINT              hash_value = header->scrmbl % header->hash_size;
    struct XH_elem	*aelem, *prev = NULL, *display = header->display;
    SXINT		*hash_display = header->hash_display;
    SXINT	old_size;
    SXBOOLEAN			overflow;

    sxinitialise(old_size); /* pour faire taire gcc -Wuninitialized */
    if (header->is_locked)
	sxtrap ("XH_set", header->name);

    if ((x = hash_display [hash_value]) != 0) {
	SXINT		*bot1, *bot2, l;
	SXUINT	scrmbl;
	SXINT			xl = display [display->lnk].X;
	SXINT			length = *(header->list) - xl;
	SXINT			*bottom = header->list + xl;

	do {
	    if ((scrmbl = (aelem = display + x)->scrmbl) >= header->scrmbl) {
		if (scrmbl == header->scrmbl) {
		    if ((z = aelem [1].X - aelem->X) >= length) {
			if (z == length) {
			    bot1 = header->list + aelem->X;
			    bot2 = bottom;
			    l = length;

			    while (l-- > 0) {
				if ((z = *bot1++ - *bot2++) != 0)
				    break;
			    }

			    if (l < 0) {
				*(header->list) = xl;
				header->scrmbl = 0;
				*result = x;

				if ((aelem->lnk & XH_80) == 0) /* real item */
				    return SXTRUE;
				else /* erased */ {
				    aelem->lnk &= XH_7F;
				    return SXFALSE;
				}
			    }

			    if (z > 0)
				break;
			}
			else
			    break;
		    }
		}
		else
		    break;
	    }
	} while ((x = (prev = aelem)->lnk & XH_7F) != 0);
    }

/* Nouvelle liste */
    (aelem = display + (*result = z = display->lnk))->lnk = x;
    aelem->scrmbl = header->scrmbl;
    header->scrmbl = 0;

    if (prev == NULL)
	hash_display [hash_value] = z;
    else
	prev->lnk = (prev->lnk & XH_80) | z;

    if ((overflow = ((display->lnk = ++z) >= display->X))) {
	/* l'element display [lnk+1] est toujours disponible */
	if (!header->is_allocated) {
	  /* On suppose que c'est un XH produit par XH2c dont les structures sont initialisees statiquement
	     et sur lesquelles on retravaille ... */
	  /* Il faut aussi allouer hash_display et display */
	  XH_oflw_copy (header);
	  header->is_allocated = SXTRUE;
	  display = header->display;
	  hash_display = header->hash_display;
	}

	display->X = 2 * (old_size = display->X);
	display = header->display = (struct XH_elem*) sxrealloc (display, display->X + 1, sizeof(struct XH_elem));
    }

    display [z].X = *(header->list);

    if (overflow) {
	/* La longueur moyenne d'une liste depasse la valeur specifiee.
	   On modifie la taille de la table de hash. */
	/* La multiplication par 2 assure que si s%h != s'%h => s%2h != s'%2h */
	SXINT	*low, *high;
	SXBOOLEAN		is_low;

	header->hash_size = 2 * (z = header->hash_size);

	if (header->stat_file != NULL) {
	    XH_sat_mess (header->stat_file, header->name, "display", old_size, display->X);
	    XH_sat_mess (header->stat_file, header->name, "hash_display", z, header->hash_size);
	}

	hash_display = header->hash_display = (SXINT*) sxrealloc (hash_display, header->hash_size, sizeof (SXINT));

	for (hash_value_counter = z - 1; hash_value_counter >= 0; hash_value_counter--) {
	    *(high = (low = hash_display + hash_value_counter) + z) = 0;

	    if ((x = *low) != 0) {
		is_low = SXTRUE;

		do {
		    if (((aelem = display + x)->scrmbl) % header->hash_size == (SXUINT)hash_value_counter) {
			if (!is_low) {
			    *low &= XH_80;
			    *low += x;
			    is_low = SXTRUE;
			}

			x = *(low = &(aelem->lnk));
		    }
		    else {
			if (is_low) {
			    *high &= XH_80;
			    *high += x;
			    is_low = SXFALSE;
			}

			x = *(high = &(aelem->lnk));
		    }
		} while ((x &= XH_7F) != 0);

		if (is_low)
		    *high &= XH_80;
		else
		    *low &= XH_80;
	    }
	}

	if (header->oflw != NULL)
	    (*(header->oflw))(old_size, display->X);
    }

    return SXFALSE;
}

void	XH_lock (XH_header *header)
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure XH
	  - Interdit l'utilisation ulterieure de XH_set
	  - Appelle (si non NULL) la procedure utilisateur oflw. */
    SXINT old_size, new_size;

    if (header->is_locked)
	return;

    header->is_locked = SXTRUE;
    header->list = (SXINT*) sxrealloc (header->list,  (header->list_size = XH_list_top (*header)) + 1, sizeof (SXINT));
    old_size = XH_size (*header);
    header->display = (struct XH_elem*) sxrealloc (header->display, (new_size = header->display->lnk) + 1, sizeof (struct XH_elem));
    header->display->X = new_size;

    if (header->oflw != NULL)
	(*(header->oflw)) (old_size, new_size);
}


void	XH_pack (XH_header *header, SXINT n, void (*swap_ft) (SXINT, SXINT))
{
    /* Cette procedure tasse a partir de n la XH en supprimant
       physiquement les listes "erased".
       Si "swap_ft" est non nul, elle est appelee a chaque deplacement. */

    SXINT			lim, bx, x;
    SXINT			*blist, *p, *top, *bot;
    struct XH_elem	*belem, *celem;

    sxinitialise(blist); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(belem); /* pour faire taire "gcc -Wuninitialized" */
    lim = XH_top (*header);
    bx = 0;

    for (x = n; x < lim; x++) {
	if (XH_is_erased (*header, x)) {
	    if (bx == 0) {
		belem = header->display + (bx = x);
		blist = header->list + belem->X;
	    }

	    /* On enleve la liste de la chaine de collision */
	    for (p = header->hash_display + (header->display [x].scrmbl % header->hash_size);
		 (*p & XH_7F) != x;
		 p = &(header->display [*p].lnk));

	    if (*p & XH_80)
		*p = header->display [x].lnk;
	    else
		*p = header->display [x].lnk & XH_7F;
	}
	else if (bx != 0) {
	    /* On deplace de x vers bx */
	    /* Maj de la chaine de collision */
	    for (p = header->hash_display + (header->display [x].scrmbl % header->hash_size);
		 (*p & XH_7F) != x;
		 p = &(header->display [*p].lnk));

	    if (*p & XH_80)
		*p = bx | XH_80;
	    else
		*p = bx;

	    celem = header->display + x;
	    belem->scrmbl = celem->scrmbl;
	    belem->lnk = celem->lnk;

	    /* Maj de la liste */
	    top = header->list + (celem + 1)->X;
	    bot = header->list + celem->X;

	    while (bot < top)
		*blist++ = *bot++;

	    (++belem)->X = blist - header->list;
	    bx++;

	    if (swap_ft != NULL)
		(*swap_ft)(x, bx);
	}
    }

    if (bx != 0) {
	header->display->lnk = bx; /* top */
	header->list [0] = belem->X; /* list_top */
    }
}


void XH2c (XH_header *header, FILE *F_XH /* named output stream */, char *name, SXBOOLEAN is_static)
{
  SXINT hash_size, size, top, list_size, list_top, x, y, z;

  hash_size = header->hash_size;
  top = header->display->lnk;
  size = header->display->X;
  list_top = header->list [0];
  list_size = header->list_size;

  fprintf (F_XH, "/* Begin XH_header %s */;\n", name);

  fprintf (F_XH, "static SXINT %s_hash_display [%ld] = {", name, (SXINT) hash_size);
  for (x = 0; x < hash_size; x++) {
    if (x%10 == 0) fprintf (F_XH, "\n/* %ld */ ", (SXINT) x);
    fprintf (F_XH, "%ld, ", (SXINT) header->hash_display [x]);
  }
  fprintf (F_XH, "\n} /* End %s_hash_display */;\n", name);

  /* On definit "display" de taille size (et non pas de taille top) pour permettre des XH_is_set eventuels sans reallocation ... */
  /* L'appelant de XH2c peut toucher size avant l'appel */
  fprintf (F_XH, "static struct XH_elem %s_display [%ld] = {\n", name, (SXINT) size+1);
  fprintf (F_XH, "/* top, unused, size */ {%ld, 0, %ld},\n", (SXINT) top, (SXINT) size);
  for (x = 1; x < top; x++) {
    fprintf (F_XH, "/* %ld */ {%ld, %ld, %ld},\n", (SXINT) x, (SXINT) header->display [x].lnk, header->display [x].scrmbl, header->display [x].X); 
  }
  fprintf (F_XH, "/* top==%ld */ {0, 0, %ld},\n", (SXINT) top, (SXINT) header->display [top].X); 
  fprintf (F_XH, "} /* End %s_display */;\n", name);

  /* On definit "list" de taille list_size (et non pas de taille list_top) pour permettre des XH_is_set eventuels sans reallocation ... */
  /* L'appelant de XH2c peut toucher list_size avant l'appel */
  fprintf (F_XH, "static SXINT %s_list [%ld] = {\n", name, (SXINT) list_size+1);
  fprintf (F_XH, "/* list_top */ %ld,", (SXINT) list_top);
  for (x = 1; x < top; x++) {
    y = header->display [x].X;
    z = header->display [x+1].X;
    fprintf (F_XH, "\n/* %ld */ ", (SXINT) x);
    while (y < z) {
      fprintf (F_XH, "%ld, ", (SXINT) header->list [y]);
      y++;
    } 
  }
  fprintf (F_XH, "\n/* list_top==%ld */ 0,\n", (SXINT) list_top);
  fprintf (F_XH, "} /* End %s_list */;\n", name);

  fprintf (F_XH, "%sXH_header %s = {\n", is_static ? "static " : "", name);
  fprintf (F_XH, "/* name */ \"%s\",\n", name);
  fprintf (F_XH, "/* hash_display */ &(%s_hash_display [0]),\n", name);
  fprintf (F_XH, "/* display */ &(%s_display [0]),\n", name);
  fprintf (F_XH, "/* list */ &(%s_list [0]),\n", name);
  fprintf (F_XH, "/* hash_size */ %ld,\n", (SXINT) hash_size);
  fprintf (F_XH, "/* list_size */ %ld,\n", (SXINT) list_size);
  fprintf (F_XH, "/* scrmbl */ %i,\n", 0);
  fprintf (F_XH, "/* (*oflw)() */ %s,\n", "NULL");
  fprintf (F_XH, "/* stat_file */ %s,\n", "NULL");
  fprintf (F_XH, "/* is_locked */ %s,\n", "SXTRUE");
  fprintf (F_XH, "/* is_allocated */ %s,\n", "SXFALSE"); /* On ne peut pas faire de free !! */
  fprintf (F_XH, "} /* End %s */;\n", name);

  fprintf (F_XH, "/* End XH_header %s */;\n", name);
}
