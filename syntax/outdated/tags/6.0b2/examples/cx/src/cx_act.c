/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */

/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from cx_act.pl1         */
/*  on Thursday the second of October, 1986, at 11:06:35,    */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3l PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030515 14:43 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 20-05-92 16:10 (phd):	Correction de l'ecriture de la date	*/
/************************************************************************/
/* 20-05-92 16:10 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/*  I N C L U D E S  */

#include "sxunix.h"
#include "put_edit.h"
char WHAT_CXACT[] = "@(#)SYNTAX - $Id: cx_act.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;


/*  E X T E R N A L   E N T R I E S  */

extern VOID	sort_by_tree ();
extern char	by_mess [], release_mess [];
extern char	*prgentname;

/*  E X T E R N A L   V A R I A B L E S  */

int	line_length;

/*   C O N S T A N T S   */

#define FF "\f"

#define NL '\n'

#define area_size 500


/*  S T A T I C   V A R I A B L E S  */

static int	ID_no = 200;
static int	area_index, ID_max, line_no;
static struct area	*area_head;
static int	*to_be_sorted /* 1:ID_max */ ;
static struct ID_s {
	   struct area_item	*head, *tail;
       }	*ID_s /* 1:ID_no */ ;
struct area_item {
    struct area_item	*next;
    int		line;
};
static struct area {
	   struct area	*lnk;
	   struct area_item	item [area_size];
       }	*area;




/*   P R O C E D U R E S   */

static BOOLEAN	compare_less (ate1, ate2)
    register int	ate1, ate2;
{
    return strcmp (sxstrget (ate1), sxstrget (ate2)) < 0;
}



static VOID	listing_output ()
{
    int		mls;

    put_edit_initialize (sxstdout);
    put_edit_nnl (9, "Listing Of:");
    put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
    put_edit_nnl (9, "By:");
    put_edit (25, by_mess);
    put_edit (25, release_mess);

    {
	extern char	*ctime ();
	long	date_time;

	put_edit_nnl (9, "Generated on:");
	date_time = time (0);
	put_edit (25, ctime (&date_time));
    }

    put_edit_nl (2);
    sxlisting_output (sxstdout);

    {
	char	s [8];

	sprintf (s, "%d", line_no);
	mls = strlen (s);
    }

    put_edit_ap (FF);
    put_edit (21, "C R O S S   R E F E R E N C E   T A B L E");
    put_edit_nl (2);

    {
	register int	i, margin, col, id, l;
	BOOLEAN		is_first;
	register struct area_item	*area_item;

	to_be_sorted = (int*) sxalloc (ID_max + 1, sizeof (int));
	margin = 0;

	for (i = 1; i <= ID_max; i++) {
	    to_be_sorted [i] = i;
	    l = sxstrlen (i);
	    margin = margin < l ? l : margin;
	}

	sort_by_tree (to_be_sorted, 1, ID_max, compare_less);
	margin += 2;

	for (i = 1; i <= ID_max; i++) {
	    id = to_be_sorted [i];
	    area_item = ID_s [id].head;
	    ID_s [id].head = NULL;

	    if (area_item != NULL) {
		put_edit_nnl (1, sxstrget (id));
		is_first = TRUE;

		for (; area_item != NULL; area_item = area_item->next) {
		    if (is_first) {
			is_first = FALSE;
			col = margin;
		    }
		    else {
			put_edit_nnl (col, ", ");
			col += 2;

			if (col > line_length)
			    col = margin;
		    }

		    put_edit_nb (col, area_item->line);
		    col += mls;
		}
	    }
	}

	put_edit_nl (1);
	sxfree (to_be_sorted);
    }
}



static struct area_item		*fetch_item ()
{
    if (++area_index == area_size) {
	if (area->lnk == NULL) {
	    area = area->lnk = (struct area*) sxcalloc (1, sizeof (struct area));
	}
	else
	    area = area->lnk;

	area_index = 0;
    }

    return &(area->item [area_index]);
}



static SXVOID	open_act ()
{
    area_head = area = (struct area*) sxcalloc (1, sizeof (struct area));
    area->lnk = NULL;
    ID_s = (struct ID_s*) sxcalloc (ID_no + 1, sizeof (struct ID_s));
}



static SXVOID	initialize_act ()
{
    area = area_head;
    ID_max = line_no = 0;
    area_index = -1;
}



static SXVOID	action_act (act)
    int		act;
{
    register int	id;
    register struct sxtoken	*tok;
    register struct ID_s	*ID;
    register struct area_item	*area_item;

    if (act == 0)
	return;

    tok = &(STACKtoken (STACKtop ()));
    area_item = fetch_item ();
    id = tok->string_table_entry;
    line_no = area_item->line = tok->source_index.line;
    area_item->next = NULL;

    if (id > ID_no) {
	register int	i;

	ID_no *= 2;
	ID_s = (struct ID_s*) sxrealloc (ID_s, ID_no + 1, sizeof (struct ID_s));

	for (i = id; i <= ID_no; i++)
	    ID_s [i].head = NULL;
    }

    ID_max = ID_max < id ? id : ID_max;
    ID = &(ID_s [id]);

    if (ID->head == NULL)
	ID->tail = ID->head = area_item;
    else {
	ID->tail->next = area_item;
	ID->tail = area_item;
    }
}



static SXVOID	close_act ()
{
    register struct area	*p;

    sxfree (ID_s);

    for (area = area_head; area != NULL; area = p) {
	p = area->lnk;
	sxfree (area);
    }
}


SXVOID
cx_act (what, arg)
    int		what, arg;
{
    switch (what) {
    case OPEN:
	open_act ();
	break;

    case INIT:
	initialize_act ();
	break;

    case ACTION:
	action_act (arg);
	break;

    case SEMPASS:
	break;

    case FINAL:
	listing_output ();
	break;

    case CLOSE:
	close_act ();
	break;

    default:
	fputs ("The function \"cx_act\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
