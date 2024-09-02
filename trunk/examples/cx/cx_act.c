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

/*  I N C L U D E S  */

#include "sxversion.h"
#include "sxunix.h"
#include "put_edit.h"

char WHAT_CXACT[] = "@(#)SYNTAX - $Id: cx_act.c 4166 2024-08-19 09:00:49Z garavel $";

/*  E X T E R N A L   E N T R I E S  */

extern char	by_mess [];
extern char	*prgentname;

/*  E X T E R N A L   V A R I A B L E S  */

SXINT	line_length;

/*   C O N S T A N T S   */

#define FF "\f"

#define NL '\n'

#define area_size 500


/*  S T A T I C   V A R I A B L E S  */

static SXINT	ID_no = 200;
static SXINT	area_index, ID_max, line_no;
static struct area	*area_head;
static SXINT	*to_be_sorted /* 1:ID_max */ ;
static struct ID_s {
	   struct area_item	*head, *tail;
       }	*ID_s /* 1:ID_no */ ;
struct area_item {
    struct area_item	*next;
    SXINT		line;
};
static struct area {
	   struct area	*lnk;
	   struct area_item	item [area_size];
       }	*area;




/*   P R O C E D U R E S   */

static bool	compare_less (SXINT ate1, SXINT ate2)
{
    return strcmp (sxstrget (ate1), sxstrget (ate2)) < 0;
}



static void	listing_output (void)
{
    SXINT		mls;

    put_edit_initialize (sxstdout);
    put_edit_nnl (9, "Listing of:");
    put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
    put_edit_nnl (9, "By:");
    put_edit (25, by_mess);
    put_edit (25, release_mess);

    {
	time_t	date_time;

	put_edit_nnl (9, "Generated on:");
	date_time = time (0);
	put_edit (25, ctime (&date_time));
    }

    put_edit_nl (2);
    sxlisting_output (sxstdout);

    {
	char	s [8];

	sprintf (s, "%ld", (SXINT) line_no);
	mls = strlen (s);
    }

    put_edit_ap (FF);
    put_edit (21, "C R O S S   R E F E R E N C E   T A B L E");
    put_edit_nl (2);

    {
	SXINT	i, margin, col, id, l;
	bool		is_first;
	struct area_item	*area_item;

	sxinitialise (col); /* pour faire taire gcc -Wuninitialized */
	to_be_sorted = (SXINT*) sxalloc (ID_max + 1, sizeof (SXINT));
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
		is_first = true;

		for (; area_item != NULL; area_item = area_item->next) {
		    if (is_first) {
			is_first = false;
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



static struct area_item		*fetch_item (void)
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



static void	open_act (void)
{
    area_head = area = (struct area*) sxcalloc (1, sizeof (struct area));
    area->lnk = NULL;
    ID_s = (struct ID_s*) sxcalloc (ID_no + 1, sizeof (struct ID_s));
}



static void	initialize_act (void)
{
    area = area_head;
    ID_max = line_no = 0;
    area_index = -1;
}



static void	action_act (SXINT act)
{
    SXINT	id;
    struct sxtoken	*tok;
    struct ID_s	*ID;
    struct area_item	*area_item;

    if (act == 0)
	return;

    tok = &(SXSTACKtoken (SXSTACKtop ()));
    area_item = fetch_item ();
    id = tok->string_table_entry;
    line_no = area_item->line = tok->source_index.line;
    area_item->next = NULL;

    if (id > ID_no) {
	SXINT	i;

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



static void	close_act (void)
{
    struct area	*p;

    sxfree (ID_s);

    for (area = area_head; area != NULL; area = p) {
	p = area->lnk;
	sxfree (area);
    }
}


void cx_act (SXINT what, SXINT action_no, SXTABLES *arg)
{
    (void) arg;
    switch (what) {
    case SXOPEN:
	open_act ();
	break;

    case SXINIT:
	initialize_act ();
	break;

    case SXACTION:
	action_act (action_no);
	break;

    case SXSEMPASS:
	break;

    case SXFINAL:
	listing_output ();
	break;

    case SXCLOSE:
	close_act ();
	break;

    default:
	fputs ("The function \"cx_act\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
