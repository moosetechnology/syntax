/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
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
/* 20030515 14:43 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 27-01-93 15:55 (phd):	Adaptation au nouveau parse_stack	*/
/************************************************************************/
/* 20-05-92 16:10 (phd):	Declaration de options_text		*/
/*				Correction de l'ecriture de la date	*/
/************************************************************************/
/* 20-05-92 16:10 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from prio.action.pl1    */
/*  on Monday the twenty-fifth of August, 1986, at 10:27:39, */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3i PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/



/*   I N C L U D E S   */

#include "sxunix.h"
#include "put_edit.h"
#include "B_tables.h"
#include "D_tables.h"
#include "varstr.h"
char WHAT_PRIOACTION[] = "@(#)SYNTAX - $Id: prio_action.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;


/*   E X T E R N S   */

extern BOOLEAN	prio_write ();
extern VOID	prio_free ();
extern BOOLEAN	is_source;
extern char	by_mess [], release_mess [], *prgentname;

/*  C O N S T A N T S   */

#define undef		0
#define	left		1
#define	right		2
#define	nonassoc	3


/*  S T A T I C     V A R I A B L E S   */

static VARSTR	vstr;
static char	**err_titles;
static int	alpha_no, symbol_table_lgth, height, assoc_kind;
static BOOLEAN	is_error, is_first_time;
static struct bnf_ag_item	bnf_ag;
static struct priority	*t_priorities /* 1:xtmax */ ;
static struct priority	*r_priorities /* 1:xnbpro */ ;
static struct priority	*named_priorities /* 1:alpha_no */ ;
static int	*t_symbol_table, *nt_symbol_table /* 0:symbol_table_lgth */ ;
static SXBA	/* indpro(+1): bit 0 is unused */ occur_set;
static struct sxsource_coord	*designator;




/*   P R O C E D U R E S   */

static VOID	header ()
{
    put_edit_nnl (9, "Listing Of:");
    put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
    put_edit_nnl (9, "By:");
    put_edit (25, by_mess);
    put_edit (25, release_mess);

    {
	extern char	*options_text ();
	char	line [132];

	put_edit_nnl (9, "Options:");
	put_edit (25, options_text (line));
    }

    {
	long	date_time;

	put_edit_nnl (9, "Generated on:");
	date_time = time (0);
	put_edit (25, ctime (&date_time));
    }

    put_edit_nl (2);
}



static VOID	listing_output ()
{
    register FILE	*listing;
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
    put_edit_finalize ();
}



static VOID	set_next (tnt)
    int		tnt;
{
    register int	i;

    i = -1;

    while ((i = sxba_scan (occur_set, i)) >= 0) {
	sxba_0_bit (occur_set, i);

	if (bnf_ag.WS_INDPRO [i].lispro == tnt)
	    sxba_1_bit (occur_set, ++i); /* Stays set! */
    }
}



static VOID	set_priority (priorities, xp)
    struct priority	*priorities;
    int		xp;
{
    register struct priority	*priorities_xp = priorities + xp;

    if (priorities_xp->assoc == 0 && priorities_xp->value == 0) {
	priorities_xp->assoc = assoc_kind;
	priorities_xp->value = height;
    }
    else if (priorities_xp->assoc != assoc_kind || priorities_xp->value != height)
	sxput_error (SXSTACKtoken (SXSTACKtop ()).source_index,
	     "%sAlready defined: associativity ignored.", err_titles [1]);
}



static int	get_ate (d)
    int		d;
{
    int		ate;

    ate = SXSTACKtoken (SXSTACKtop () - d).string_table_entry;

    while (ate > symbol_table_lgth) {
	register int	i;

	named_priorities = (struct priority*) sxrealloc (named_priorities, symbol_table_lgth + alpha_no + 1, sizeof (
	     struct priority));
	t_symbol_table = (int*) sxrealloc (t_symbol_table, symbol_table_lgth + alpha_no + 1, sizeof (int));

	if (nt_symbol_table != NULL) {
	    nt_symbol_table = (int*) sxrealloc (nt_symbol_table, symbol_table_lgth + alpha_no + 1, sizeof (int));

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




static VOID	init ()
{
    register int	i;

    occur_set = NULL;
    named_priorities = NULL;
    nt_symbol_table = NULL;
    t_symbol_table = NULL;
    r_priorities = NULL;
    t_priorities = NULL;
    vstr = NULL;


/* On lit les "bnf_tables" */

    if (!bnf_read (&bnf_ag, prgentname))
	SXEXIT (SXSEVERITIES - 1);

    vstr = varstr_alloc (32);
    t_priorities = (struct priority*) sxcalloc (bnf_ag.WS_TBL_SIZE.xtmax + 1, sizeof (struct priority));
    r_priorities = (struct priority*) sxcalloc (bnf_ag.WS_TBL_SIZE.xnbpro + 1, sizeof (struct priority));
    /*  Symbol_table processing   */
    alpha_no = 20;
    symbol_table_lgth = alpha_no + bnf_ag.WS_TBL_SIZE.xntmax + bnf_ag.WS_TBL_SIZE.xtmax;
    t_symbol_table = (int*) sxcalloc (symbol_table_lgth + 1, sizeof (int));
    named_priorities = (struct priority*) sxcalloc (symbol_table_lgth + 1, sizeof (struct priority));

    for (i = -bnf_ag.WS_TBL_SIZE.xtmax; i < 0; i++) {
	t_symbol_table [sxstrsave (bnf_ag.T_STRING + bnf_ag.ADR [i])] = i;
    }

    height = 0;
    is_error = FALSE;
    is_first_time = TRUE;
}




static VOID	action (action_no)
    int		action_no;
{
    if (is_error)
	return;

    switch (action_no) {
    default:
	fputs ("The function \"action\" is out of date with respect to its specification.\n", sxstderr);
	abort ();

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
	    register int	t, ate = get_ate (0);

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
	    register int	j, i;

	    for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xnbpro; i++) {
		for (j = bnf_ag.WS_NBPRO [i + 1].prolon - 1; j >= bnf_ag.WS_NBPRO [i].prolon; j--) {
		    register int	t = bnf_ag.WS_INDPRO [j].lispro;

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
	    register int	i;

	    is_first_time = FALSE;
	    nt_symbol_table = (int*) sxcalloc (symbol_table_lgth + 1, sizeof (int));

	    for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xntmax; i++) {
		nt_symbol_table [sxstrsave (bnf_ag.NT_STRING + bnf_ag.ADR [i])] = i;
	    }

	    occur_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.indpro + 1);
	}
	else
	    sxba_empty (occur_set);

	designator = &(SXSTACKtoken (SXSTACKtop () - 1).source_index);

	{
	    register int	ate = get_ate (1), nt = nt_symbol_table [ate];

	    if (nt <= 0)
		sxput_error (*designator, "%sUnknown non terminal symbol.", err_titles [2]);
	    else {
		register int	i;

		for (i = bnf_ag.WS_NTMAX [nt].npg; i < bnf_ag.WS_NTMAX [nt + 1].npg; i++) {
		    sxba_1_bit (occur_set, bnf_ag.WS_NBPRO [bnf_ag.WS_NBPRO [i].numpg].prolon);
		}
	    }
	}

	break;


/* <VOCABULARY>		= <X_TERMINAL>				; 7 */

    case 7:
	{
	    register int	ate = get_ate (0), t = t_symbol_table [ate];

	    if (t >= 0)
		sxput_error (SXSTACKtoken (STACKtop ()).source_index, "%sUnknown terminal symbol.", err_titles [1]);
	    else
		set_next (t);
	}

	break;


/* <VOCABULARY>		= <X_NON_TERMINAL> 			; 8 */

    case 8:
	{
	    register int	ate = get_ate (0), nt = nt_symbol_table [ate];

	    if (nt <= 0)
		sxput_error (SXSTACKtoken (STACKtop ()).source_index, "%sUnknown non terminal symbol.", err_titles [1]);
	    else
		set_next (nt);
	}

	break;


/* <PRIORITY>		= "%prec"   <X_TERMINAL>		; 9 */

    case 9:
	{
	    static int	val, ass;

	    {
		register int	t, ate = get_ate (0);

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
		sxput_error (SXSTACKtoken (STACKtop ()).source_index, "%sThere is no priority assigned to this symbol.",
		     err_titles [1]);

	    {
		register int	i;

		i = -1;

		while ((i = sxba_scan (occur_set, i)) >= 0) {
		    if (bnf_ag.WS_INDPRO [i].lispro == 0) {
			register int	j = bnf_ag.WS_INDPRO [i].prolis;

			r_priorities [j].value = val;
			r_priorities [j].assoc = ass;
			return;
		    }
		}
	    }
	}

	sxput_error (*designator, "%sThis unknown grammar rule is ignored.", err_titles [1]);
	break;


/* <X_TERMINAL>		= <TERMINAL>  %PREDICATE		; 10 */
/* <X_NON_ TERMINAL>	= %NON_TERMINAL  %PREDICATE		; 11 */
    case 10:
    case 11:
	SXSTACKtoken (STACKtop () - 1).string_table_entry = sxstrsave (varstr_tostr (varstr_catenate (
	     varstr_catenate (varstr_catenate (varstr_raz (vstr), sxstrget (SXSTACKtoken (STACKtop () - 1).
	     string_table_entry)), " "), sxstrget (SXSTACKtoken (STACKtop ()).string_table_entry))));
	break;
    }
}



static VOID	final ()
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
    SXEXIT (SXSEVERITIES - 1);

  prio_free (&t_priorities, &r_priorities);
  bnf_free (&bnf_ag);

  if (is_source) {
    listing_output ();
  }
}


SXVOID
prio_action (what, arg)
    int		what;
    struct sxtables	*arg;
{
    switch (what) {
    case OPEN:
	err_titles = arg->err_titles;
	break;

    case INIT:
	init ();
	break;

    case ACTION:
	action ((int) (long) arg);
	break;

    case SEMPASS:
	break;

    case ERROR:
	is_error = TRUE;
	break;

    case FINAL:
	final ();
	break;

    case CLOSE:
	break;

    default:
	fputs ("The function \"prio_action\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
