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
/* Ven 15 Dec 2000 13:59 (pb):	Ajout de "is_allocated"			*/
/************************************************************************/
/* 22-03-94 14:23 (pb):		Modif "XH_stat"				*/
/************************************************************************/
/* 01-10-92 16:10 (pb):		Ajout de "pack" 			*/
/************************************************************************/
/* 23-01-92 08:50 (pb):		"SCRMBL" devient un "unsigned int"	*/
/************************************************************************/
/* 11-03-91 12:15 (pb):		Ajout de "XH_is_set"	 		*/
/************************************************************************/
/* 13-09-90 10:50 (pb):		Ajout de "_lock" et "_unlock" 		*/
/************************************************************************/
/* 04-09-90 10:41 (pb):		Ajout de "XH_stat"		 	*/
/************************************************************************/
/* 10-05-89 09:52 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/



static char	ME [] = "XH_mngr";

#include	<stdio.h>
#include	"XH.h"
#include        "sxcommon.h"

char WHAT_XH_MNGR[] = "@(#)SYNTAX - $Id: XH_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;


extern void sxtrap (char *caller, char *message);
extern int  sxnext_prime (int germe);

static void XH_sat_mess (stat_file, header_name, field_name, old_size, new_size)
    FILE	*stat_file;
    char	*header_name, *field_name;
    int		old_size, new_size;
{
    fprintf (stat_file, "%s: %s.%s of size %d overflowed: reallocation with size %d.\n", ME, header_name, field_name, old_size, new_size);
}


void	XH_alloc (header, name, init_elem_nb, average_list_nb_per_bucket, average_list_size, oflw, stat_file)
    XH_header	*header;
    char	*name;
    int		init_elem_nb, average_list_nb_per_bucket, average_list_size;
    int		(*oflw) ();
    FILE	*stat_file;
{
    /* Cette fonction "cree" un adresseur extensible de listes d'entiers */

    header->name = name;
    header->hash_size = sxnext_prime (init_elem_nb / average_list_nb_per_bucket);
    header->hash_display = (int*) sxcalloc ((unsigned long int)(header->hash_size), sizeof (int));
    header->display = (struct XH_elem*) sxalloc (init_elem_nb + 1, sizeof (struct XH_elem));
    header->display->lnk = 1;
    header->display->X = init_elem_nb;
    header->display [1].X = 1;
    header->list = (int*) sxalloc ((header->list_size = average_list_size * init_elem_nb) + 1, sizeof (int));
    header->list [0] = 1;
    header->scrmbl = 0;
    header->is_locked = FALSE;
    header->oflw = oflw;
    header->stat_file = stat_file;
    header->is_allocated = TRUE;
}

/* Les structures sont "statiques", on les alloue/copie en dynamique */ 
static void
XH_oflw_copy (header)
     XH_header	*header;
{
  struct XH_elem* old_display = header->display, *display, *top_display;
  int *old_hash_display = header->hash_display, *hash_display, *top_hash_display;
  int *old_list = header->list, *list, *top_list;

  header->hash_display = hash_display = (int*) sxcalloc ((unsigned long int)(header->hash_size), sizeof (int));
  header->display = display = (struct XH_elem*) sxalloc (header->display->X + 1, sizeof (struct XH_elem));
  header->list = list = (int*) sxalloc (header->list_size + 1, sizeof (int));

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

BOOLEAN	XH_oflw (header)
    XH_header	*header;
{
    register int old_size;

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
      header->is_allocated = TRUE;
    }
    else
      header->list = (int*) sxrealloc (header->list, header->list_size + 1, sizeof (int));

    return TRUE;
}


void	XH_clear (header)
    XH_header	*header;
{
    register int *top, *bot;

    if (header->display->lnk != 1) {
	top = (bot = header->hash_display) + header->hash_size;

	while (--top >= bot)
	    *top = 0;

	header->display->lnk = 1;
	header->display [1].X = 1;
	header->list [0] = 1;
	header->is_locked = FALSE;
    }
}



void	XH_stat (stat_file, header)
    FILE	*stat_file;
    XH_header		*header;
{
    register int	x, i, l, n;
    int			minl, maxl, meanl, hash_size, size, top, maxll, minll,
                        list_size, list_top, ll, total_size, total_top;

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
	total_size = total_top = sizeof (XH_header) + sizeof (int) * hash_size;
	total_size += sizeof (struct XH_elem) * (size + 1) + sizeof (int) * (list_size + 1);
	total_top += sizeof (struct XH_elem) * (top + 1) + sizeof (int) * (list_top + 1);

	fprintf (stat_file, "name = %s, hash_size = %d (%d%%), size = %d (%d%%), minl = %d, maxl = %d, meanl = %d, list_size = %d (%d%%), minll = %d, maxll = %d, meanll = %d, Total_size (byte) = %d (%d%%)\n",
		 header->name, hash_size, (100*n)/hash_size, size, (100*top)/size, minl, maxl, meanl, list_size, (100*list_top)/list_size, minll, maxll, list_top/top, total_size, (100*total_top)/total_size);
    }
    else
	fprintf (stat_file, "name = %s, hash_size = %d, size = %d, list_size = %d is empty.\n", header->name, hash_size, size, list_size);
}
    
    
    
void	XH_free (header)
    XH_header	*header;
{
    if (header->stat_file != NULL)
	XH_stat (header->stat_file, header);
    
    sxfree (header->list), header->list = NULL;
    sxfree (header->display), header->display = NULL;
    sxfree (header->hash_display), header->hash_display = NULL;
    header->is_allocated = FALSE;
}



int	XH_is_set (header)
    XH_header	*header;
{
    /* Recherche si la liste
           header->list[header->display[header->display->lnk].X .. *(header->list)]
       existe par ailleurs */
    /* Retourne l'indice dans display de la liste ou 0 */
    register int		x, z;
    register unsigned int	scrmbl;
    register struct XH_elem	*aelem, *display = header->display;
    int				xl = display [display->lnk].X;

    if ((x = header->hash_display [header->scrmbl % header->hash_size]) != 0) {
	register int		*bot1, *bot2, l;
	int			length = *(header->list) - xl;
	int			*bottom = header->list + xl;

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

BOOLEAN	XH_set (header, result)
    XH_header	*header;
    int	*result;
{
    /* Recherche si la liste
           header->list[header->display[header->display->lnk].X .. *(header->list)]
       existe par ailleurs */
    /* Retourne l'indice dans display de la [nouvelle] liste */
    register int		x, z, hash_value = header->scrmbl % header->hash_size;
    register struct XH_elem	*aelem, *prev = NULL, *display = header->display;
    register int		*hash_display = header->hash_display;
    int	old_size;
    BOOLEAN			overflow;

    sxinitialise(old_size); /* pour faire taire gcc -Wuninitialized */
    if (header->is_locked)
	sxtrap ("XH_set", header->name);

    if ((x = hash_display [hash_value]) != 0) {
	register int		*bot1, *bot2, l;
	register unsigned int	scrmbl;
	int			xl = display [display->lnk].X;
	int			length = *(header->list) - xl;
	int			*bottom = header->list + xl;

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
				    return TRUE;
				else /* erased */ {
				    aelem->lnk &= XH_7F;
				    return FALSE;
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
	  header->is_allocated = TRUE;
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
	register int	*low, *high;
	BOOLEAN		is_low;

	header->hash_size = 2 * (z = header->hash_size);

	if (header->stat_file != NULL) {
	    XH_sat_mess (header->stat_file, header->name, "display", old_size, display->X);
	    XH_sat_mess (header->stat_file, header->name, "hash_display", z, header->hash_size);
	}

	hash_display = header->hash_display = (int*) sxrealloc (hash_display, header->hash_size, sizeof (int));

	for (hash_value = z - 1; hash_value >= 0; hash_value--) {
	    *(high = (low = hash_display + hash_value) + z) = 0;

	    if ((x = *low) != 0) {
		is_low = TRUE;

		do {
		    if (((aelem = display + x)->scrmbl) % header->hash_size == (unsigned int)hash_value) {
			if (!is_low) {
			    *low &= XH_80;
			    *low += x;
			    is_low = TRUE;
			}

			x = *(low = &(aelem->lnk));
		    }
		    else {
			if (is_low) {
			    *high &= XH_80;
			    *high += x;
			    is_low = FALSE;
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

    return FALSE;
}

void	XH_lock (header)
    XH_header		*header;
{
    /* Cette procedure
          - Ajuste au minimum la taille de la structure XH
	  - Interdit l'utilisation ulterieure de XH_set
	  - Appelle (si non NULL) la procedure utilisateur oflw. */
    register int old_size, new_size;

    if (header->is_locked)
	return;

    header->is_locked = TRUE;
    header->list = (int*) sxrealloc (header->list,  (header->list_size = XH_list_top (*header)) + 1, sizeof (int));
    old_size = XH_size (*header);
    header->display = (struct XH_elem*) sxrealloc (header->display, (new_size = header->display->lnk) + 1, sizeof (struct XH_elem));
    header->display->X = new_size;

    if (header->oflw != NULL)
	(*(header->oflw)) (old_size, new_size);
}


void	XH_pack (header, n, swap_ft)
    XH_header		*header;
    int n;
    void (*swap_ft)();
{
    /* Cette procedure tasse a partir de n la XH en supprimant
       physiquement les listes "erased".
       Si "swap_ft" est non nul, elle est appelee a chaque deplacement. */

    int			lim, bx, x;
    int			*blist, *p, *top, *bot;
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
		 (*p & XH_7F) != (unsigned int)x;
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
		 (*p & XH_7F) != (unsigned int)x;
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


void XH2c (header, F_XH, name, is_static)
    XH_header		*header;
    FILE		*F_XH /* named output stream */;
    char		*name; 
    BOOLEAN		is_static;
{
  int hash_size, size, top, list_size, list_top, x, y, z;

  hash_size = header->hash_size;
  top = header->display->lnk;
  size = header->display->X;
  list_top = header->list [0];
  list_size = header->list_size;

  fprintf (F_XH, "/* Begin XH_header %s */;\n", name);

  fprintf (F_XH, "static int %s_hash_display [%i] = {", name, hash_size);
  for (x = 0; x < hash_size; x++) {
    if (x%10 == 0) fprintf (F_XH, "\n/* %i */ ", x);
    fprintf (F_XH, "%i, ", header->hash_display [x]);
  }
  fprintf (F_XH, "\n} /* End %s_hash_display */;\n", name);

  /* On definit "display" de taille size (et non pas de taille top) pour permettre des XH_is_set eventuels sans reallocation ... */
  /* L'appelant de XH2c peut toucher size avant l'appel */
  fprintf (F_XH, "static struct XH_elem %s_display [%i] = {\n", name, size+1);
  fprintf (F_XH, "/* top, unused, size */ {%i, 0, %i},\n", top, size);
  for (x = 1; x < top; x++) {
    fprintf (F_XH, "/* %i */ {%i, %i, %i},\n", x, header->display [x].lnk, header->display [x].scrmbl, header->display [x].X); 
  }
  fprintf (F_XH, "/* top==%i */ {0, 0, %i},\n", top, header->display [top].X); 
  fprintf (F_XH, "} /* End %s_display */;\n", name);

  /* On definit "list" de taille list_size (et non pas de taille list_top) pour permettre des XH_is_set eventuels sans reallocation ... */
  /* L'appelant de XH2c peut toucher list_size avant l'appel */
  fprintf (F_XH, "static int %s_list [%i] = {\n", name, list_size+1);
  fprintf (F_XH, "/* list_top */ %i,", list_top);
  for (x = 1; x < top; x++) {
    y = header->display [x].X;
    z = header->display [x+1].X;
    fprintf (F_XH, "\n/* %i */ ", x);
    while (y < z) {
      fprintf (F_XH, "%i, ", header->list [y]);
      y++;
    } 
  }
  fprintf (F_XH, "\n/* list_top==%i */ 0,\n", list_top);
  fprintf (F_XH, "} /* End %s_list */;\n", name);

  fprintf (F_XH, "%sXH_header %s = {\n", is_static ? "static " : "", name);
  fprintf (F_XH, "/* name */ \"%s\",\n", name);
  fprintf (F_XH, "/* hash_display */ &(%s_hash_display [0]),\n", name);
  fprintf (F_XH, "/* display */ &(%s_display [0]),\n", name);
  fprintf (F_XH, "/* list */ &(%s_list [0]),\n", name);
  fprintf (F_XH, "/* hash_size */ %i,\n", hash_size);
  fprintf (F_XH, "/* list_size */ %i,\n", list_size);
  fprintf (F_XH, "/* scrmbl */ %i,\n", 0);
  fprintf (F_XH, "/* (*oflw)() */ %s,\n", "NULL");
  fprintf (F_XH, "/* stat_file */ %s,\n", "NULL");
  fprintf (F_XH, "/* is_locked */ %s,\n", "TRUE");
  fprintf (F_XH, "/* is_allocated */ %s,\n", "FALSE"); /* On ne peut pas faire de free !! */
  fprintf (F_XH, "} /* End %s */;\n", name);

  fprintf (F_XH, "/* End XH_header %s */;\n", name);
}
