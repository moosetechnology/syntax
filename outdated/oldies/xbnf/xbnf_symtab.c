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

#include "sxunix.h"
#include "sxba.h"
#include "B_tables.h"
#include "xbnf_vars.h"
#include "varstr.h"

char WHAT_XBNFSYMTAB[] = "@(#)SYNTAX - $Id: xbnf_symtab.c 3678 2024-02-06 08:38:24Z garavel $" WHAT_DEBUG;

/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ACTION_n 2
#define BNF_n 3
#define GENERIC_TERMINAL_n 4
#define LHS_NON_TERMINAL_n 5
#define NON_TERMINAL_n 6
#define PREDICATE_n 7
#define RULE_S_n 8
#define TERMINAL_n 9
#define VOCABULARY_S_n 10
#define X_LHS_n 11
#define X_NON_TERMINAL_n 12
#define X_TERMINAL_n 13
/*
E N D   N O D E   N A M E S
*/


/*------------------------------------------------------*/
/*							*/
/*   S Y M B O L    T A B L E    P R O C E S S I N G    */
/*							*/
/*------------------------------------------------------*/

/*-----------------*/
/* local variables */
/*-----------------*/

static bool	is_X_LHS;
static SXINT	st_top, x_rule, act_no, x_act_rule, x_act_lispro, tmax, xtmax, ntmax, xntmax,
                t_no, xt_no, nt_no, xnt_no;
static SXBA	t_set, nt_set, act_set, xnt_set, xt_set /* 1:st_top */ ;
static VARSTR	vstr;
static SXINT	*ste_to_code, *code_to_ste, *code_to_rhs_nb;
static struct lhs_disp {
    SXINT	head, tail;
}		*lhs_disp;
static SXINT	*lhs_to_rule;




static void	set_xsymbols (SXNODE *visited)
{
    /* Mise en alpha table des symbols etendus par appel recursif de set_xsymbols */

    for (;;)
	switch (visited->name) {
	case BNF_n:
	    visited = visited->son;
	    continue;

	case PREDICATE_n:
	    varstr_raz (vstr);
	    visited->father->token.string_table_entry =
		sxstrsave (varstr_tostr (varstr_catenate (varstr_catenate (varstr_catenate (vstr,
											    sxstrget (sxbrother (visited, 1)->token.string_table_entry)),
									   " "),
							  sxstrget (visited->token.string_table_entry))));
	    return;

	case VOCABULARY_S_n:
	case RULE_S_n:
	case X_TERMINAL_n:
	case X_NON_TERMINAL_n:
	    for (visited = visited->son; visited != NULL; visited = visited->brother)
		set_xsymbols (visited);

	    return;

	default:
	    return;
	}
}


static void put_LHS (SXINT nt, SXINT rule)
{
    SXINT *atail, *ahead;

    ahead = (*(atail = &(lhs_disp [nt].tail)) == 0) ?
	       &(lhs_disp [nt].head) :
	       &(lhs_to_rule [*atail]);
    *atail = *ahead = rule;
}
    


static SXINT	st_get_nt_code (SXINT ste)
{
    SXINT	nt;

    if ((nt = ste_to_code [ste]) == 0) {
	nt = ste_to_code [ste] = ++ntmax;
	code_to_ste [nt] = ste;
    }

    return nt;
}



static SXINT	st_get_xnt_code (SXINT ste, SXINT xste, SXINT pste)
{
    SXINT	nt, xnt, i;

    nt = st_get_nt_code (ste);
    code_to_rhs_nb [nt]++; /* xnt est compte aussi en tant que nt */

    if ((xnt = ste_to_code [xste]) == 0) {
	xnt = ste_to_code [xste] = ++xntmax;
	code_to_ste [xnt] = xste;
    }

    i = xnt - nt_no;
    bnf_ag.XNT_TO_NT_PRDCT [i].v_code = nt;
    bnf_ag.XNT_TO_NT_PRDCT [i].prdct_no = atoi (sxstrget (pste) + 1);

    return xnt;
}


static SXINT	st_get_t_code (struct sxsource_coord source_index, SXINT ste, bool is_gene)
{
    SXINT	t;
    bool		is_old_gene;

    if ((t = ste_to_code [-ste]) == 0) {
	t = --tmax;

	if (is_gene)
	    SXBA_1_bit (is_a_generic, -t);

	ste_to_code [-ste] = t;
	code_to_ste [t] = ste;
    }
    else {
	/* Un code terminal a deja ete affecte a ce ste, sont-ils du meme type ? */
	is_old_gene = SXBA_bit_is_set (is_a_generic, -t);

	/* Le terminal "%blabla" n'est pas generique. Il est interdit d'utiliser
	   dans la meme grammaire le terminal %blabla (generique) et "%blabla".
	   ... */
	if (is_old_gene != is_gene) {
	    sxerror (source_index,
		     sxtab_ptr->err_titles [2][0] /* error */ ,
		     "%sThis %s has already been defined as a %s; the previous definition is retained.",
		     sxtab_ptr->err_titles [2]+1 /* error */ ,
		     is_gene ? "generic terminal symbol" : "terminal symbol",
		     is_gene ? "non generic terminal symbol" : "generic terminal symbol");
	}
    }

    return t;
}



static SXINT	st_get_xt_code (struct sxsource_coord source_index,
				SXINT ste, 
				SXINT xste, 
				SXINT pste,
				bool	is_gene)
{
    SXINT	t, xt, i;

    t = st_get_t_code (source_index, ste, is_gene);

    if ((xt = ste_to_code [-(st_top + xste)]) == 0) {
	xt = -++xtmax;
	i = -xt - t_no;
	bnf_ag.XT_TO_T_PRDCT [i].v_code = -t;
	bnf_ag.XT_TO_T_PRDCT [i].prdct_no = atoi (sxstrget (pste) + 1);
	ste_to_code [-(st_top + xste)] = xt;
	code_to_ste [xt] = xste;
    }

    return xt;
}


static	void fill_symbol_strings (SXINT symbol_code, SXINT ste)
{
    SXINT	longueur = sxstrlen (ste);

    if (symbol_code > 0) {
	if (longueur > M_nt)
	    M_nt = longueur;
    }
    else {
	if (longueur > M_t)
	    M_t = longueur;
    }

    strcpy (((symbol_code > 0) ? nt_string : t_string) + adr [symbol_code], sxstrget (ste));
}



static void	pass_indpro_size (SXNODE *visited)
{
    SXINT ste;

    /* calcul recursif de indpro_size */

    for (;;) {
	ste = visited->token.string_table_entry;

	switch (visited->name) {
	case ACTION_n:
	    SXBA_1_bit (act_set, ste);
	    return;

	case BNF_n:
	    break;

	case X_LHS_n:
	    ste = visited->son->token.string_table_entry;
	    is_X_LHS = true;
	    /* FALLTHROUGH */ /* added by H. Garavel, confirmed by P. Boullier */

	case LHS_NON_TERMINAL_n:
	case NON_TERMINAL_n:
	    SXBA_1_bit (nt_set, ste);
	    return;

	case X_NON_TERMINAL_n:
	    sxba_1_bit (xnt_set, ste);
	    break;

	case X_TERMINAL_n:
	    sxba_1_bit (xt_set, ste);
	    break;

	case TERMINAL_n:
	case GENERIC_TERMINAL_n:
	    SXBA_1_bit (t_set, ste);
	    return;

	case VOCABULARY_S_n:
	    W.indpro += visited->degree;
	    /* FALLTHROUGH */ /* added by H. Garavel, confirmed by P. Boullier */

	case RULE_S_n:
	    for (visited = visited->son; visited != NULL; visited = visited->brother)
		pass_indpro_size (visited);
	    /* FALLTHROUGH */ /* added by H. Garavel, confirmed by P. Boullier */

	default:
	    return;
	}

	visited = visited->son;
    }
}



static void	pass_derived (void)
{

/*
D E R I V E D
*/

    SXINT	ste, tnt_code;
    SXNODE	*visited = SXVISITED;

    switch (visited->name) {
    case BNF_n:
	WN [x_act_rule].prolon = x_act_lispro;
	return;

    case ACTION_n:
	if (visited->father->name == VOCABULARY_S_n) {
	    tnt_code = WI [indpro_size].lispro =
		st_get_nt_code (ste = VS.string_table_entry);
	    code_to_rhs_nb [tnt_code]++;
	    WI [indpro_size++].prolis = x_rule;

	    if (SXBA_bit_is_reset_set (act_set, ste)) {
		put_LHS (tnt_code, x_act_rule);
		WI [x_act_lispro].prolis = x_act_rule;
		WN [x_act_rule].prolon = x_act_lispro++;
		WN [x_act_rule].reduc = tnt_code;
		WN [x_act_rule++].action = atoi (sxstrget (ste) + 1);
	    }
	}
	else
	{
	    /* visited->father->name == X_LHS_n */
	    /* La post-action est mise ds RULE_TO_REDUCE qui ne sert plus a rien ds cette version. */
	    bnf_ag.RULE_TO_REDUCE [x_rule] = atoi (sxstrget (VS.string_table_entry) + 1);
	}


/* ca peut etre une production simple */

	return;

    case LHS_NON_TERMINAL_n:
	put_LHS (WN [x_rule].reduc = st_get_nt_code (VS.string_table_entry),
		 x_rule);

	if (max_RHS != -1 && visited->father->degree > max_RHS + 1)
	    sxerror (VS.source_index, sxtab_ptr->err_titles [1][0], "%sThe right hand-side of production #%ld seems too long.  \
Check for a missing semi-colon.",
		     sxtab_ptr->err_titles [1]+1,
		     (SXINT) x_rule);

	WN [x_rule].prolon = ++indpro_size;
	    /* Si visited->father->name == X_LHS_n => il ne faut pas eliminer la production simple */
	WN [x_rule].bprosimpl = visited->father->name == VOCABULARY_S_n &&
	  visited->father->degree == 2 /* a priori */ ;
	return;

    case NON_TERMINAL_n:
	if (visited->father->name == X_NON_TERMINAL_n) {
	    tnt_code = st_get_xnt_code (VS.string_table_entry,
					visited->father->token.string_table_entry,
					visited->brother->token.string_table_entry);
	}
	else {
	    tnt_code = st_get_nt_code (VS.string_table_entry);
	}

	WI [indpro_size].lispro = tnt_code;
	code_to_rhs_nb [tnt_code]++;
	WI [indpro_size++].prolis = x_rule;
	return;
	    
    case GENERIC_TERMINAL_n:
    case TERMINAL_n:
	if (VS.string_table_entry == EOF_ste) {
	    sxerror (VS.source_index,
		     sxtab_ptr->err_titles [2][0],
		     "%sThe terminal symbol name \"%s\" is reserved, another name must be used.",
		     sxtab_ptr->err_titles [2]+1,
		     sxstrget (EOF_ste));
	}
	else {
	    if (visited->father->name == X_TERMINAL_n) {
		tnt_code= st_get_xt_code (VS.source_index,
					  VS.string_table_entry,
					  visited->father->token.string_table_entry,
					  visited->brother->token.string_table_entry,
					  (bool) (visited->name == GENERIC_TERMINAL_n));
	    }
	    else {
		tnt_code = st_get_t_code (VS.source_index,
					  VS.string_table_entry,
					  (bool) (visited->name == GENERIC_TERMINAL_n));
	    }

	    code_to_rhs_nb [tnt_code]++;
	    WI [indpro_size].lispro = tnt_code;
	    WI [indpro_size++].prolis = x_rule;
	}

	WN [x_rule].bprosimpl = false;
	return;

    case VOCABULARY_S_n:
	WI [indpro_size].prolis = x_rule;
	return;

    case X_LHS_n:
	return;

    case X_NON_TERMINAL_n:
	    /* Si predicat, il ne faut pas eliminer la production simple */
        WN [x_rule].bprosimpl = false;
	return;
	

/*
Z Z Z Z
*/

    default:
	return;
    }
}



static void	pass_inherited (void)
{

/*
I N H E R I T E D
*/

    SXNODE	*visited = SXVISITED;

    switch (visited->father->name) {
    case BNF_n:
	/* SXVISITED->name = RULE_S_n */
	x_act_rule = visited->degree + 1;
	return;

    case RULE_S_n:
	/* SXVISITED->name = VOCABULARY_S_n */
	x_rule = visited->position;

    default:
	return;
    }
}


/*-------------------------*/
/* symbol table processing */
/*-------------------------*/

void	symbol_table_processing (SXNODE *adam)
{

/*-----------------------------------------------*/
/*  A T T R I B U T E S    E V A L U A T I O N   */
/*-----------------------------------------------*/

    vstr = varstr_alloc (64);

    if (is_predicate /* Le scanner a trouve des predicats */ )
	set_xsymbols (adam);

    W.indpro = 3;
    st_top = sxstrmngr.top;
    act_set = sxba_calloc (st_top + 1);
    t_set = sxba_calloc (st_top + 1);
    xt_set = sxba_calloc (st_top + 1);
    nt_set = sxba_calloc (st_top + 1);
    xnt_set = sxba_calloc (st_top + 1);

    pass_indpro_size (adam);

    t_no = sxba_cardinal (t_set) + 1 /* eof */ ;
    xt_no = sxba_cardinal (xt_set);
    act_no = sxba_cardinal (act_set);
    nt_no = sxba_cardinal (nt_set) + act_no;
    xnt_no = sxba_cardinal (xnt_set);

    W.nbpro = adam->son->degree;
    W.xnbpro = W.nbpro + act_no;

    /* Bidouille pour compatibilite avec les B_tables sans avoir a changer de version. */
    if (is_X_LHS)
	W.actpro = 0;
    else
	W.actpro = W.xnbpro;

    W.tmax = -t_no;
    W.xtmax = t_no + xt_no;

    W.ntmax = nt_no;
    W.xntmax = nt_no + xnt_no;

    sxba_empty (act_set);

    sxfree (xnt_set);
    sxfree (xt_set);
    sxfree (nt_set);
    sxfree (t_set);

    is_a_generic = sxba_calloc (-W.tmax + 1);
    ste_to_code = (SXINT*) /* -2*st_top : st_top */
	sxcalloc (3 * st_top + 1, sizeof (SXINT)) + 2 * st_top;
    code_to_ste = (SXINT*) /* -xtmax:xntmax */
			 sxcalloc (W.xtmax + W.xntmax + 1, sizeof (SXINT)) + W.xtmax;
    code_to_rhs_nb = (SXINT*) /* -xtmax:xntmax */
			 sxcalloc (W.xtmax + W.xntmax + 1, sizeof (SXINT)) + W.xtmax;
    WI = (struct ws_indpro_s*) /* 0:indpro_size+act_no+1 */
			       sxcalloc (W.indpro + act_no + 2,
					 sizeof (struct ws_indpro_s));
    lhs_disp = (struct lhs_disp*) sxcalloc (W.xntmax + 1, sizeof (struct lhs_disp));
    lhs_to_rule = (SXINT*) sxcalloc (W.xnbpro + 1, sizeof (SXINT));

    bnf_ag.XT_TO_T_PRDCT = xt_no == 0 ? NULL :
	(struct xv_to_v_prdct_s*) sxcalloc (xt_no + 1, sizeof (struct xv_to_v_prdct_s));
    bnf_ag.XNT_TO_NT_PRDCT = xnt_no == 0 ? NULL :
	(struct xv_to_v_prdct_s*) sxcalloc (xnt_no + 1, sizeof (struct xv_to_v_prdct_s));

    WN = (struct ws_nbpro_s*) /* 0:xnbpro+1 */ sxcalloc (W.xnbpro + 2,
							 sizeof (struct ws_nbpro_s));

    if (is_X_LHS) {
	SXINT	*p;

	bnf_ag.RULE_TO_REDUCE = (SXINT*) sxalloc (W.xnbpro + 1, sizeof (SXINT));

	for (p = bnf_ag.RULE_TO_REDUCE + W.xnbpro; p > bnf_ag.RULE_TO_REDUCE; p--)
	    *p = -1;

	*p = 0;
    }

    tmax = ntmax = 0;
    xtmax = -W.tmax;
    xntmax = W.ntmax;
    indpro_size = 3;
    x_act_lispro = W.indpro + 1;

    sxsmp (adam, pass_inherited, pass_derived);

    W.indpro = x_act_lispro - 1;

    sxfree (act_set);


/*-----------------------------------------------------------*/
/*  D U M M Y   P R O D U C T I O N    P R O C E S S I N G   */
/*-----------------------------------------------------------*/

/* 0: <dummy_non_terminal> = <axiom> "End Of File"; */

    WI [1].lispro = 1;
    code_to_rhs_nb [1]++;
    /* <axiom> in the RHS of the dummy rule */
    /* ste_to_code [-EOF_ste] = W.tmax; unused */
    code_to_rhs_nb [W.tmax]++;
    code_to_ste [W.tmax] = EOF_ste;
    /* EOF in the RHS of the dummy rule 	 */
    WI [2].lispro = W.tmax;
    WN [0].prolon = 1;


/*--------------------------------------*/
/*  X R E F   C O N S T R U C T I O N   */
/*--------------------------------------*/

    {
	/* contruction of adr,ws_ntmax(WN),tpd */
	SXINT	ste;
	SXINT	xnumpd = 1;
	SXINT	xtnumpd = 1;
	SXINT	xnumpg = 1;

	W.nt_string_length = 0;
	W.t_string_length = 0;
	adr = (SXINT*) /* -W.xtmax-1,W.xntmax+1 */
		     sxalloc (W.xntmax + W.xtmax + 3, sizeof (SXINT)) + W.xtmax + 1;
	WX = (struct ws_ntmax_s*) /* 0:xntmax+1 */
				  sxalloc (W.xntmax + 2, sizeof (struct ws_ntmax_s));
	tpd = (SXINT*) /* -xtmax-1:-1 */
		     sxalloc (W.xtmax + 1, sizeof (SXINT)) + W.xtmax + 1;
	bvide = sxba_calloc (W.xntmax + 1);

	{
	    SXINT	code, rule_no;

	    for (code = 1; code <= W.xntmax; code++) {
		ste = code_to_ste [code];
		adr [code] = W.nt_string_length;
		W.nt_string_length += sxstrlen (ste) + 1;
		/* on ajoute 0 en fin de chaine */

		WX [code].npg = xnumpg;

		for (rule_no = lhs_disp [code].head;rule_no != 0; rule_no = lhs_to_rule [rule_no]) {
		    WN [xnumpg++].numpg = rule_no;
		}

		WX [code].npd = xnumpd += code_to_rhs_nb [code];
	    }

	    numpd = (SXINT*) /* 1:maxnpd */ sxalloc ((W.maxnpd = xnumpd) + 1, sizeof (SXINT));

	    for (code = -1; code >= -W.xtmax; code--) {
		ste = code_to_ste [code];
		adr [code] = W.t_string_length;
		W.t_string_length += sxstrlen (ste) + 1;
		/* on ajoute 0 en fin de chaine */
		tpd [code] = xtnumpd += code_to_rhs_nb [code];
	    }

	    tnumpd = (SXINT*) /* 1:maxtpd */ sxalloc ((W.maxtpd = xtnumpd) + 1, sizeof (SXINT));
	}

	adr [W.xntmax + 1] = W.nt_string_length;
	adr [-W.xtmax - 1] = W.t_string_length;
	t_string = (char*) sxalloc (W.t_string_length + 1, sizeof (char));
	nt_string = (char*) sxalloc (W.nt_string_length + 1, sizeof (char));
	WX [W.xntmax + 1].npg = xnumpg;
	WX [W.xntmax + 1].npd = xnumpd;
	tpd [-W.xtmax - 1] = xtnumpd;
    }

    {
	/* construction of ws_nbpro(WN)[].tnumpd,numpd   */
	/* updating of ws_ntmax(WX)			 */
	SXINT	i, code;

	for (i = W.indpro; i >= 1; i--) {
	    if ((code = WI [i].lispro) > 0) {
		numpd [--WX [code].npd] = i;

		if (code > W.ntmax)
		    /* Le nt associe a xnt occurre aussi en i */
		    numpd [--WX [XNT_TO_NT (code)].npd] = i;
	    }
	    else if (code < 0)
		tnumpd [--tpd [code]] = i;
	}
    }

    {
	/* construction of t_string, nt_string */
	M_nt = 0;
	M_t = 0;

	{
	    SXINT	code;

	    for (code = 1; code <= W.xntmax; code++) {
		fill_symbol_strings (code, code_to_ste [code]);
	    }

	    for (code = -1; code >= -W.xtmax; code--) {
		fill_symbol_strings (code, code_to_ste [code]);
	    }
	}
    }

    WN [W.xnbpro + 1].reduc = 0;
    WN [W.xnbpro + 1].numpg = 0;
    adr [0] = 0;
    numpd [W.maxnpd] = 0;
    varstr_free (vstr);
    sxfree (code_to_rhs_nb - W.xtmax);
    sxfree (code_to_ste - W.xtmax);
    sxfree (ste_to_code - 2 * st_top);
    sxfree (lhs_disp);
    sxfree (lhs_to_rule);
}
