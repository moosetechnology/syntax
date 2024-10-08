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

/*   I N C L U D E S   */

#include "sxversion.h"
#include "sxunix.h"
#include "put_edit.h"
#include "B_tables.h"
#include "D_tables.h"
#include "varstr.h"

char WHAT_PRIOACTION[] = "@(#)SYNTAX - $Id: prio_action.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

/*   E X T E R N S   */

extern bool	prio_write (SXINT t_priorities_size, 
                            SXINT r_priorities_size,
                            struct priority *t_priorities,
                            struct priority *r_priorities,
                            char *langname);
extern void	prio_free (struct priority **t_priorities, struct priority **r_priorities);
extern bool	is_source;
extern bool	is_listing;
extern char	by_mess [], *prgentname;

/*  C O N S T A N T S   */

#define undef		0
#define	left		1
#define	right		2
#define	nonassoc	3


/*  S T A T I C     V A R I A B L E S   */

static VARSTR	vstr;
static char	**err_titles;
static SXINT	alpha_no, symbol_table_lgth, height, assoc_kind;
static bool	is_error, is_first_time;
static struct bnf_ag_item	bnf_ag;
static struct priority	*t_priorities /* 1:xtmax */ ;
static struct priority	*r_priorities /* 1:xnbpro */ ;
static struct priority	*named_priorities /* 1:alpha_no */ ;
static SXINT	*t_symbol_table, *nt_symbol_table /* 0:symbol_table_lgth */ ;
static SXBA	/* indpro(+1): bit 0 is unused */ occur_set;
static struct sxsource_coord	*designator;



/*   P R O C E D U R E S   */

static void	header (void)
{
    put_edit_nnl (9, "Listing of:");
    put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
    put_edit_nnl (9, "By:");
    put_edit (25, by_mess);
    put_edit (25, release_mess);

    {
	extern char	*options_text (char *);
	char	line [132];

	put_edit_nnl (9, "Options:");
	put_edit (25, options_text (line));
    }

    {
	time_t	date_time;

	put_edit_nnl (9, "Generated on:");
	date_time = time (0);
	put_edit (25, ctime (&date_time));
    }

    put_edit_nl (2);
}

static void priority_dump (FILE *listing, struct priority *p)
{
  if (p == NULL) {
    fprintf (listing, "<nil>\n");
    return;
  }
  fprintf (listing, "<");
  switch (p->assoc) {
  case undef:
    fprintf (listing, "undef");
    break;
  case left:
    fprintf (listing, "left");
    break;
  case right:
    fprintf (listing, "right");
    break;
  case nonassoc:
    fprintf (listing, "nonassoc");
    break;
  default:
    fprintf (listing, "?(%ld)", p->assoc);
    break;
  }
  fprintf (listing, ", %ld>\n", p->value);
}

static void prio_dump (FILE *listing)
{
  SXINT i;

  fprintf (listing, "generated tables:\n");
  fprintf (listing, "height == %ld\n", height);
  
  for (i = 0; i < bnf_ag.WS_TBL_SIZE.xtmax + 1; i++) {
    fprintf (listing, "t_prio[%ld] == ", i);
    priority_dump (listing, &(t_priorities[i]));
  }
          
  for (i = 0; i < bnf_ag.WS_TBL_SIZE.xnbpro + 1; i++) {
    fprintf (listing, "r_prio[%ld] == ", i);
    priority_dump (listing, &(r_priorities[i]));
  }
}

static void	listing_output (void)
{
    FILE	*listing;
    char	*lst_name;

    if (sxverbosep)
	fputs ("\tListing Output\n", sxtty);

    vstr = varstr_alloc (32);

    vstr = varstr_catenate (vstr, prgentname);
    vstr = varstr_catenate (vstr, ".pr.l");
    lst_name = varstr_tostr (vstr);

    if ((listing = sxfopen (lst_name, "w")) == NULL) {
      fprintf (sxstderr, "listing_output: cannot open (create) ");
      sxperror (lst_name);
      return;
    }

    varstr_free (vstr), vstr = NULL;

    put_edit_initialize (listing);
    header ();
    sxlisting_output (listing);
    if (is_listing) {
      prio_dump (listing);
    }
    put_edit_finalize ();
}



static void	set_next (SXINT tnt)
{
    SXINT	i;

    i = -1;

    while ((i = sxba_scan (occur_set, i)) >= 0) {
	sxba_0_bit (occur_set, i);

	if (bnf_ag.WS_INDPRO [i].lispro == tnt)
	    sxba_1_bit (occur_set, ++i); /* Stays set! */
    }
}



static void	set_priority (struct priority *priorities,
			      SXINT xp)
{
    struct priority	*priorities_xp = priorities + xp;

    if (priorities_xp->assoc == 0 && priorities_xp->value == 0) {
	priorities_xp->assoc = assoc_kind;
	priorities_xp->value = height;
    }
    else if (priorities_xp->assoc != assoc_kind || priorities_xp->value != height)
	sxerror (SXSTACKtoken (SXSTACKtop ()).source_index,
		 err_titles [1][0],
		 "%sAlready defined: associativity ignored.",
		 err_titles [1]+1);
}



static SXINT	get_ate (SXINT d)
{
    SXINT		ate;

    ate = SXSTACKtoken (SXSTACKtop () - d).string_table_entry;

    while (ate > symbol_table_lgth) {
	SXINT	i;

	named_priorities = (struct priority*) sxrealloc (named_priorities, symbol_table_lgth + alpha_no + 1, sizeof (
	     struct priority));
	t_symbol_table = (SXINT*) sxrealloc (t_symbol_table, symbol_table_lgth + alpha_no + 1, sizeof (SXINT));

	if (nt_symbol_table != NULL) {
	    nt_symbol_table = (SXINT*) sxrealloc (nt_symbol_table, symbol_table_lgth + alpha_no + 1, sizeof (SXINT));

	    for (i = 1; i <= alpha_no; i++) {
		nt_symbol_table [symbol_table_lgth + i] = 0;
	    }
	}

	for (i = 1; i <= alpha_no; i++) {
	    named_priorities [symbol_table_lgth + i].assoc = 0;
	    named_priorities [symbol_table_lgth + i].value = 0;
	    t_symbol_table [symbol_table_lgth + i] = 0;
	}

	symbol_table_lgth += alpha_no;
    }

    return ate;
}



static void	init (void)
{
    SXINT	i;

    occur_set = NULL;
    named_priorities = NULL;
    nt_symbol_table = NULL;
    t_symbol_table = NULL;
    r_priorities = NULL;
    t_priorities = NULL;
    vstr = NULL;


/* On lit les "bnf_tables" */

    if (!bnf_read (&bnf_ag, prgentname))
	sxexit (SXSEVERITIES - 1);

    vstr = varstr_alloc (32);
    t_priorities = (struct priority*) sxcalloc (bnf_ag.WS_TBL_SIZE.xtmax + 1, sizeof (struct priority));
    r_priorities = (struct priority*) sxcalloc (bnf_ag.WS_TBL_SIZE.xnbpro + 1, sizeof (struct priority));
    /*  Symbol_table processing   */
    alpha_no = 20;
    symbol_table_lgth = alpha_no + bnf_ag.WS_TBL_SIZE.xntmax + bnf_ag.WS_TBL_SIZE.xtmax;
    t_symbol_table = (SXINT*) sxcalloc (symbol_table_lgth + 1, sizeof (SXINT));
    named_priorities = (struct priority*) sxcalloc (symbol_table_lgth + 1, sizeof (struct priority));

    for (i = -bnf_ag.WS_TBL_SIZE.xtmax; i < 0; i++) {
	t_symbol_table [sxstrsave (bnf_ag.T_STRING + bnf_ag.ADR [i])] = i;
    }

    height = 0;
    is_error = false;
    is_first_time = true;
}



static void	action (SXINT action_no)
{
    if (is_error)
	return;

    switch (action_no) {
    default:
	fputs ("The function \"action\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);

    case 0:
	return;


/* <PRIORITY_KIND>	= "%left"				; 1 */

    case 1:
	height++;
	assoc_kind = left;
	break;


/* <PRIORITY_KIND>	= "%right"				; 2 */

    case 2:
	height++;
	assoc_kind = right;
	break;


/* <PRIORITY_KIND>	= "%nonassoc"				; 3 */

    case 3:
	height++;
	assoc_kind = nonassoc;
	break;


/* <OPERATOR>		= <X_TERMINAL>				; 4 */

    case 4:
	{
	    SXINT	t, ate = get_ate ((SXINT)0);

	    if ((t = t_symbol_table [ate]) == 0)
		set_priority (named_priorities, ate);
	    else
		set_priority (t_priorities, -t);
	}

	break;


/* <RULE_PRIORITY_LIST>	=					; 5 */

    case 5:
	{
	    /* initialisation de r_priorities */
	    SXINT	j, i;

	    for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xnbpro; i++) {
		for (j = bnf_ag.WS_NBPRO [i + 1].prolon - 1; j >= bnf_ag.WS_NBPRO [i].prolon; j--) {
		    SXINT	t = bnf_ag.WS_INDPRO [j].lispro;

		    if (t < 0) {
			r_priorities [i] = t_priorities [-t];
			break;
		    }
		}
	    }
	}

	break;


/* <VOCABULARY_LIST>	= %LHS_NON_TERMINAL  =	 		; 6 */

    case 6:
	if (is_first_time) {
	    SXINT	i;

	    is_first_time = false;
	    nt_symbol_table = (SXINT*) sxcalloc (symbol_table_lgth + 1, sizeof (SXINT));

	    for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xntmax; i++) {
		nt_symbol_table [sxstrsave (bnf_ag.NT_STRING + bnf_ag.ADR [i])] = i;
	    }

	    occur_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.indpro + 1);
	}
	else
	    sxba_empty (occur_set);

	designator = &(SXSTACKtoken (SXSTACKtop () - 1).source_index);

	{
	    SXINT	ate = get_ate ((SXINT)1), nt = nt_symbol_table [ate];

	    if (nt <= 0)
		sxerror (*designator,
			 err_titles [2][0],
			 "%sUnknown non terminal symbol.",
			 err_titles [2]+1);
	    else {
		SXINT	i;

		for (i = bnf_ag.WS_NTMAX [nt].npg; i < bnf_ag.WS_NTMAX [nt + 1].npg; i++) {
		    sxba_1_bit (occur_set, bnf_ag.WS_NBPRO [bnf_ag.WS_NBPRO [i].numpg].prolon);
		}
	    }
	}

	break;


/* <VOCABULARY>		= <X_TERMINAL>				; 7 */

    case 7:
	{
	    SXINT	ate = get_ate ((SXINT)0), t = t_symbol_table [ate];

	    if (t >= 0)
		sxerror (SXSTACKtoken (SXSTACKtop ()).source_index,
			 err_titles [1][0],
			 "%sUnknown terminal symbol.",
			 err_titles [1]+1);
	    else
		set_next (t);
	}

	break;


/* <VOCABULARY>		= <X_NON_TERMINAL> 			; 8 */

    case 8:
	{
	    SXINT	ate = get_ate ((SXINT)0), nt = nt_symbol_table [ate];

	    if (nt <= 0)
		sxerror (SXSTACKtoken (SXSTACKtop ()).source_index,
			 err_titles [1][0],
			 "%sUnknown non terminal symbol.",
			 err_titles [1]+1);
	    else
		set_next (nt);
	}

	break;


/* <PRIORITY>		= "%prec"   <X_TERMINAL>		; 9 */

    case 9:
	{
	    static SXINT	val, ass;

	    {
		SXINT	t, ate = get_ate ((SXINT)0);

		if ((t = t_symbol_table [ate]) == 0) {
		    val = named_priorities [ate].value;
		    ass = named_priorities [ate].assoc;
		}
		else {
		    val = t_priorities [-t].value;
		    ass = t_priorities [-t].assoc;
		}
	    }

	    if (val == 0)
		sxerror (SXSTACKtoken (SXSTACKtop ()).source_index,
			 err_titles [1][0],
			 "%sThere is no priority assigned to this symbol.",
			 err_titles [1]+1);

	    {
		SXINT	i;

		i = -1;

		while ((i = sxba_scan (occur_set, i)) >= 0) {
		    if (bnf_ag.WS_INDPRO [i].lispro == 0) {
			SXINT	j = bnf_ag.WS_INDPRO [i].prolis;

			r_priorities [j].value = val;
			r_priorities [j].assoc = ass;
			return;
		    }
		}
	    }
	}

	sxerror (*designator, 
		 err_titles [1][0],
		 "%sThis unknown grammar rule is ignored.", 
		 err_titles [1]+1);
	break;


/* <X_TERMINAL>		= <TERMINAL>  %SXPREDICATE		; 10 */
/* <X_NON_ TERMINAL>	= %NON_TERMINAL  %SXPREDICATE		; 11 */
    case 10:
    case 11:
	SXSTACKtoken (SXSTACKtop () - 1).string_table_entry = sxstrsave (varstr_tostr (varstr_catenate (
	     varstr_catenate (varstr_catenate (varstr_raz (vstr), sxstrget (SXSTACKtoken (SXSTACKtop () - 1).
	     string_table_entry)), " "), sxstrget (SXSTACKtoken (SXSTACKtop ()).string_table_entry))));
	break;
    }
}

static void	final (void)
{
  if (occur_set != NULL) {
    sxfree (occur_set);
    occur_set = NULL;
  }

  if (nt_symbol_table != NULL) {
    sxfree (nt_symbol_table);
    nt_symbol_table = NULL;
  }

  if (t_symbol_table != NULL) {
    sxfree (t_symbol_table);
    t_symbol_table = NULL;
  }

  if (named_priorities != NULL) {
    sxfree (named_priorities);
    named_priorities = NULL;
  }

  if (vstr != NULL) {
    varstr_free (vstr);
    vstr = NULL;
  }

  if (!prio_write (bnf_ag.WS_TBL_SIZE.xtmax + 1, bnf_ag.WS_TBL_SIZE.xnbpro + 1, t_priorities, r_priorities, prgentname))
    sxexit (SXSEVERITIES - 1);

  if (is_source) {
    listing_output ();
  }

  prio_free (&t_priorities, &r_priorities);
  bnf_free (&bnf_ag);
}


void prio_action (SXINT what, SXINT action_no, SXTABLES *arg)
{
    (void) arg;
    switch (what) {
    case SXOPEN:
	err_titles = arg->err_titles;
	break;

    case SXINIT:
	init ();
	break;

    case SXACTION:
	action (action_no);
	break;

    case SXSEMPASS:
	break;

    case SXERROR:
	is_error = true;
	break;

    case SXFINAL:
	final ();
	break;

    case SXCLOSE:
	break;

    default:
	fputs ("The function \"prio_action\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
