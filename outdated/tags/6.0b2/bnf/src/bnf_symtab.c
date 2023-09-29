/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (bl)  *
   *							  *
   ******************************************************** */





/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030512 13:07 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* Lun 25 Nov 1996 10:25 (pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 17-10-90 09:55 (pb):		Pour chaque xnt <A>&1, creation des	*/
/*				regles bidon :				*/
/*				{<A>&1 = rhs | <A> = rhs dans P}	*/
/************************************************************************/
/* 10-04-90 14:30 (phd):	Suppression des "typedef" de bl		*/
/*				Ajout du copyright			*/
/************************************************************************/
/* 07-04-88 15:01 (pb):		Adaptation aux XNT		 	*/
/************************************************************************/
/* 11-02-88 11:55 (pb):		L'axiome peut apparaitre en partie 	*/
/*				droite					*/
/************************************************************************/
/* 11-02-88 11:55 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/



#include "sxunix.h"
#include "sxba.h"
#include "B_tables.h"
#include "bnf_vars.h"
#include "varstr.h"
char WHAT_BNFSYMTAB[] = "@(#)SYNTAX - $Id: bnf_symtab.c 582 2007-05-25 08:25:51Z rlacroix $" WHAT_DEBUG;

static int	st_get_nt_code ();
static int	st_get_t_code ();
static int	st_get_xnt_code ();
static int	st_get_xt_code ();

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
#define X_NON_TERMINAL_n 11
#define X_TERMINAL_n 12
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

static int	x_rule, act_no, t_no, xt_no, nt_no, xnt_no, x_act_rule, x_act_lispro, xntmax, xtmax;
static SXBA	t_set, nt_set, act_set, xt_set, xnt_set /* 1:st_top */ ;
static VARSTR	vstr;
static int	*ste_to_code, *code_to_ste, *code_to_rhs_nb;
static BOOLEAN	is_X_NON_TERMINAL;
static int	*NT_to_lhs_nb, *NT_to_rhs_lgth, *XNT_to_ntste;
static int	xnt_rules_nb, xnt_rhs_lgth;
static struct lhs_disp {
    int	head, tail;
}		*lhs_disp;
static int	*lhs_to_rule;



static VOID put_LHS (nt, rule)
    int nt, rule;
{
    register int *atail, *ahead;

    ahead = (*(atail = &(lhs_disp [nt].tail)) == 0) ?
	       &(lhs_disp [nt].head) :
	       &(lhs_to_rule [*atail]);
    *atail = *ahead = rule;
}
    


static VOID	set_xsymbols (visited)
    register NODE	*visited;
{

/*
D E R I V E D
*/

    /* Mise en alpha table des symbols etendus par appel recursif de set_xsymbols */

    for (;;)
	switch (visited->name) {
	case BNF_n:
	    visited = visited->son;
	    continue;

	case PREDICATE_n:
	    varstr_raz (vstr);
	    visited->father->token.string_table_entry = sxstrsave (varstr_tostr (varstr_catenate (varstr_catenate (
		 varstr_catenate (vstr, sxstrget (sxbrother (visited, 1)->token.string_table_entry)), " "), sxstrget (
		 visited->token.string_table_entry))));
	    return;

	case X_NON_TERMINAL_n:
	    is_X_NON_TERMINAL = TRUE;

	case VOCABULARY_S_n:
	case RULE_S_n:
	case X_TERMINAL_n:
	    for (visited = visited->son; visited != NULL; visited = visited->brother)
		set_xsymbols (visited);

	default:
	    return;
	}
}




static VOID	pass_indpro_size (visited)
    register NODE	*visited;
{
    register int ste;
/*
D E R I V E D
*/

    /* calcul recursif de indpro_size */

    for (;;) {
	ste = visited->token.string_table_entry;

	switch (visited->name) {
	case ACTION_n:
	    SXBA_1_bit (act_set, ste);
	    return;

	case BNF_n:
	    W.nbpro = visited->son->degree;
	    break;

	case LHS_NON_TERMINAL_n:
	case NON_TERMINAL_n:
	    SXBA_1_bit (nt_set, ste);
	    return;

	case TERMINAL_n:
	case GENERIC_TERMINAL_n:
	    SXBA_1_bit (t_set, ste);
	    return;

	case X_NON_TERMINAL_n:
	    SXBA_1_bit (xnt_set, ste);
	    XNT_to_ntste [ste] = visited->son->token.string_table_entry;
	    break;

	case X_TERMINAL_n:
	    SXBA_1_bit (xt_set, ste);
	    break;

	case VOCABULARY_S_n:
	    indpro_size += visited->degree;

	    if (is_X_NON_TERMINAL) {
		++NT_to_lhs_nb [ste = visited->son->token.string_table_entry];
		NT_to_rhs_lgth [ste] += visited->degree;
	    }

	case RULE_S_n:
	    for (visited = visited->son; visited != NULL; visited = visited->brother)
		pass_indpro_size (visited);

	default:
	    return;
	}

	visited = visited->son;
    }
}




static VOID	pass_derived ()
{

/*
D E R I V E D
*/

    register int	ste, tnt_code;
    register NODE	*visited = VISITED;

    switch (visited->name) {
    case BNF_n:
	WN [x_act_rule].prolon = x_act_lispro;
	W.indpro = x_act_lispro - 1;
	return;

    case ACTION_n:
	tnt_code = WI [W.indpro].lispro =
	  st_get_nt_code (/* VS.source_index, */ste = VS.string_table_entry, FALSE);
	code_to_rhs_nb [tnt_code]++;
	WI [W.indpro++].prolis = x_rule;

	if (SXBA_bit_is_reset_set (act_set, ste)) {
	    put_LHS (tnt_code, x_act_rule);
	    WI [x_act_lispro].prolis = x_act_rule;
	    WN [x_act_rule].prolon = x_act_lispro++;
	    WN [x_act_rule].reduc = tnt_code;
	    WN [x_act_rule++].action = atoi (sxstrget (ste) + 1);
	}


/* ca peut etre une production simple */

	return;

    case LHS_NON_TERMINAL_n:
      put_LHS (WN [x_rule].reduc = st_get_nt_code (/* VS.source_index, */
						     VS.string_table_entry,
						     FALSE),
		    x_rule);

	if (max_RHS != -1 && visited->father->degree > max_RHS + 1)
	    sxput_error (VS.source_index, "%sThe right hand-side of production #%d seems too long.  \
Check for a missing semi-colon.", sxtab_ptr->err_titles [1], x_rule);

	WN [x_rule].prolon = ++W.indpro;
	WN [x_rule].bprosimpl = visited->father->degree == 2 /* a priori */ ;
	return;

    case NON_TERMINAL_n:
	if (visited->father->name == X_NON_TERMINAL_n) {
	  tnt_code = st_get_xnt_code (/* VS.source_index, */VS.string_table_entry,
				      visited->father->token.string_table_entry,
				      visited->brother->token.string_table_entry);
	}
	else {
	  tnt_code = st_get_nt_code (/* VS.source_index, */VS.string_table_entry, FALSE);
	}
	
	WI [W.indpro].lispro = tnt_code;
	code_to_rhs_nb [tnt_code]++;
	WI [W.indpro++].prolis = x_rule;
	return;
	    
    case GENERIC_TERMINAL_n:
    case TERMINAL_n:
	if (VS.string_table_entry == (unsigned)EOF_ste) {
	    sxput_error (VS.source_index, "%sThe terminal symbol name \"%s\" is reserved, another name must be used.",
			 sxtab_ptr->err_titles [2], sxstrget (EOF_ste));
	}
	else {
	    if (visited->father->name == X_TERMINAL_n) {
		tnt_code = st_get_xt_code (VS.source_index, VS.string_table_entry, visited->father->token.
		     string_table_entry, visited->brother->token.string_table_entry, visited->name == GENERIC_TERMINAL_n);
	    }
	    else {
		tnt_code = st_get_t_code (VS.source_index, VS.string_table_entry, visited->name ==
		     GENERIC_TERMINAL_n, FALSE);
	    }

	    code_to_rhs_nb [tnt_code]++;
	    WI [W.indpro].lispro = tnt_code;
	    WI [W.indpro++].prolis = x_rule;
	}

	WN [x_rule].bprosimpl = FALSE;
	return;

    case VOCABULARY_S_n:
	WI [W.indpro].prolis = x_rule;
	return;


/*
Z Z Z Z
*/

    default:
	return;
    }
}




static VOID	pass_inherited ()
{

/*
I N H E R I T E D
*/

    register NODE	*visited = VISITED;

    switch (visited->father->name) {
    case BNF_n:
	/* VISITED->name = RULE_S_n */
	x_act_rule = visited->degree + 1;

	bnf_ag.XT_TO_T_PRDCT = xt_no == 0 ? NULL : (struct xv_to_v_prdct_s*) sxcalloc (xt_no + 1, sizeof (struct
	     xv_to_v_prdct_s));
	bnf_ag.XNT_TO_NT_PRDCT = xnt_no == 0 ? NULL : (struct xv_to_v_prdct_s*) sxcalloc (xnt_no + 1, sizeof (struct
	     xv_to_v_prdct_s));

	WN = (struct ws_nbpro_s*) /* 0:xnbpro+1 */ sxcalloc (W.xnbpro + 2, sizeof (struct ws_nbpro_s));
	return;

    case RULE_S_n:
	/* VISITED->name = VOCABULARY_S_n */
	x_rule = visited->position;

    default:
	return;
    }
}




static int	st_get_nt_code (/* source_index, */ste, has_prdct)
     /* struct sxsource_coord	source_index; */
    int		ste;
    BOOLEAN	has_prdct;
{
    register int	nt;

    if ((nt = ste_to_code [ste]) == 0) {
	nt = ste_to_code [ste] = has_prdct ? ++xntmax : ++W.ntmax;
	code_to_ste [nt] = ste;
    }

    return nt;
}



static int	st_get_t_code (source_index, ste, is_gene, has_prdct)
    struct sxsource_coord	source_index;
    int		ste;
    BOOLEAN	is_gene, has_prdct;
{
    register int	t;

    if ((t = ste_to_code [-ste]) == 0) {
	t = ste_to_code [-ste] = has_prdct ? -++xtmax : --W.tmax;

	if (!has_prdct && is_gene)
	    SXBA_1_bit (is_a_generic, -t);

	code_to_ste [t] = ste;
    }
    else {
	/* Il est interdit d'utiliser dans la meme grammaire le terminal %blabla (generique) et "%blabla". */
        /* Ds le cas d'un XT, On ne fait le test que sur l'appel avec has_prdct==FALSE */
	if (!has_prdct && (SXBA_bit_is_set (is_a_generic, -t) != is_gene)) {
	    sxput_error (source_index,
		 "%sThis %sgeneric terminal symbol has already been defined as %sgeneric; the previous definition is retained.",
		 sxtab_ptr->err_titles [2] /* error */ , is_gene ? "" : "non ", is_gene ? "non " : "");
	}

	/* Le terminal t &1 est un terminal etendu. Il est interdit d'utiliser
	   dans la meme grammaire le terminal "t &1" (normal) et t &1. */
	if ((t < -t_no) != has_prdct) {
	    sxput_error (source_index,
	  "%sThis %s terminal symbol has already been defined as %s terminal symbol; the previous definition is retained."
		 , sxtab_ptr->err_titles [2] /* error */ , has_prdct ? "extended" : "simple", has_prdct ? "a simple" :
		 "an extended");
	}
    }

    return t;
}



static int	st_get_xnt_code (/* source_index, */ste, xste, pste)
     /* struct sxsource_coord	source_index; */
    int		ste, xste, pste;
{
    register int	nt, xnt, i;

    nt = st_get_nt_code (/* source_index, */ste, FALSE);
    xnt = st_get_nt_code (/* source_index, */xste, TRUE);
    i = xnt - nt_no;
    bnf_ag.XNT_TO_NT_PRDCT [i].v_code = nt;
    bnf_ag.XNT_TO_NT_PRDCT [i].prdct_no = atoi (sxstrget (pste) + 1);
    return xnt;
}



static int	st_get_xt_code (source_index, ste, xste, pste, is_gene)
    struct sxsource_coord	source_index;
    int		ste, xste, pste;
    BOOLEAN	is_gene;
{
    register int	t, xt, i;

    t = st_get_t_code (source_index, ste, is_gene, FALSE);
    xt = st_get_t_code (source_index, xste, is_gene, TRUE);
    i = -xt - t_no;
    bnf_ag.XT_TO_T_PRDCT [i].v_code = -t;
    bnf_ag.XT_TO_T_PRDCT [i].prdct_no = atoi (sxstrget (pste) + 1);
    return xt;
}



static SXVOID	fill_symbol_strings (symbol_code, ste)
    int		symbol_code, ste;
{
    register int	longueur = sxstrlen (ste);

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




/*-------------------------*/
/* symbol table processing */
/*-------------------------*/

VOID	symbol_table_processing (adam)
    NODE	*adam;
{
    int		st_top;


/*-----------------------------------------------*/
/*  A T T R I B U T E S    E V A L U A T I O N   */
/*-----------------------------------------------*/

    vstr = varstr_alloc (64);
    is_X_NON_TERMINAL = FALSE;

    if (is_predicate /* Le scanner a trouve des predicats */ )
	set_xsymbols (adam);

    W.tmax = 0;
    W.ntmax = 0;
    W.indpro = indpro_size = 3;
    st_top = sxstrmngr.top;
    act_set = sxba_calloc (st_top + 1);
    t_set = sxba_calloc (st_top + 1);
    nt_set = sxba_calloc (st_top + 1);
    xt_set = sxba_calloc (st_top + 1);
    xnt_set = sxba_calloc (st_top + 1);

    if (is_X_NON_TERMINAL) {
	NT_to_lhs_nb = (int*) sxcalloc (st_top + 1, sizeof (int));
	NT_to_rhs_lgth = (int*) sxcalloc (st_top + 1, sizeof (int));
	XNT_to_ntste = (int*) sxcalloc (st_top + 1, sizeof (int));
    }

    pass_indpro_size (adam);
    xnt_rules_nb = xnt_rhs_lgth = 0;

    if (is_X_NON_TERMINAL) {
	/* On calcule la place prise par les regles bidons dues a la presence
	   de non-terminaux etendus. */
	register int xntste = 0, ntste;

	while ((xntste = sxba_scan (xnt_set, xntste)) > 0) {
	    ntste = XNT_to_ntste [xntste];
	    xnt_rules_nb += NT_to_lhs_nb [ntste];
	    xnt_rhs_lgth += NT_to_rhs_lgth [ntste];
	}

	sxfree (NT_to_lhs_nb);
	sxfree (NT_to_rhs_lgth);
	sxfree (XNT_to_ntste);
    }

    xtmax = t_no = sxba_cardinal (t_set) + 1 /* eof */ ;
    xt_no = sxba_cardinal (xt_set);
    act_no = sxba_cardinal (act_set);
    xntmax = nt_no = sxba_cardinal (nt_set) + act_no;
    xnt_no = sxba_cardinal (xnt_set);
    W.xtmax = t_no + xt_no;
    W.xntmax = nt_no + xnt_no;
    W.actpro = W.nbpro + act_no;
    W.xnbpro = W.actpro + xnt_rules_nb;
    sxba_empty (act_set);
    sxfree (xnt_set);
    sxfree (xt_set);
    sxfree (nt_set);
    sxfree (t_set);
    x_act_lispro = indpro_size + 1;
    is_a_generic = sxba_calloc (t_no + 1);
    ste_to_code = (int*) /* -st_top : st_top */
	sxcalloc (2 * st_top + 1, sizeof (int)) + st_top;
    code_to_ste = (int*) /* -xtmax:xntmax */
			 sxcalloc (W.xtmax + W.xntmax + 1, sizeof (int)) + W.xtmax;
    code_to_rhs_nb = (int*) /* -xtmax:xntmax */
			 sxcalloc (W.xtmax + W.xntmax + 1, sizeof (int)) + W.xtmax;
    WI = (struct ws_indpro_s*) /* 0:indpro_size+act_no+xnt_rhs_lgth+1 */
			       sxcalloc (indpro_size + act_no + xnt_rhs_lgth + 2, sizeof (struct ws_indpro_s));
    lhs_disp = (struct lhs_disp*) sxcalloc (W.xntmax + 1, sizeof (struct lhs_disp));
    lhs_to_rule = (int*) sxcalloc (W.xnbpro + 1, sizeof (int));
    sxsmp (adam, pass_inherited, pass_derived);
    sxfree (act_set);

/*-----------------------------------------------------------*/
/*  X N T   P R O D U C T I O N    P R O C E S S I N G       */
/*-----------------------------------------------------------*/

    if (is_X_NON_TERMINAL) {
	/* Pour chaque (nt, prdct), on duplique les regles ou nt apparait
	   en partie gauche. */
	register int 	xnt, rule_no, x_xnt_rule, x;
	int		tnt, lim;

	rule_to_reduce = (int*) sxalloc (xnt_rules_nb + 1, sizeof (int));
	/* Le 21/06/2002, pour essayer de prevenir csynt_lc qu'il faut utiliser xbnf!! */
	rule_to_reduce [0] = 21062002;

	x_xnt_rule = x_act_rule;

	for (xnt = W.ntmax + 1; xnt <= W.xntmax; xnt++) {
	    for (rule_no = lhs_disp [XNT_TO_NT (xnt)].head;rule_no != 0; rule_no = lhs_to_rule [rule_no]) {
		/* on duplique rule_no */
		put_LHS (xnt, x_xnt_rule);
		WN [x_xnt_rule].reduc = xnt;
		WN [x_xnt_rule].prolon = ++W.indpro;
		WN [x_xnt_rule].bprosimpl = WN [rule_no].bprosimpl;
		rule_to_reduce [x_xnt_rule - W.actpro] = rule_no;

		for (lim = WN [rule_no + 1].prolon - 1, x = WN [rule_no].prolon; x < lim;x++) {
		    tnt = WI [W.indpro].lispro = WI [x].lispro;
		    WI [W.indpro++].prolis = x_xnt_rule;
		    code_to_rhs_nb [tnt]++;
		}

		WI [W.indpro].lispro = 0;
		WI [W.indpro].prolis = x_xnt_rule;
		x_xnt_rule++;
	    }
	}

	WN [x_xnt_rule].prolon = W.indpro + 1;
    }
    else
	rule_to_reduce = NULL;

/*-----------------------------------------------------------*/
/*  D U M M Y   P R O D U C T I O N    P R O C E S S I N G   */
/*-----------------------------------------------------------*/

/* 0: <dummy_non_terminal> = <axiom> "End Of File"; */

    WI [1].lispro = 1;
    code_to_rhs_nb [1]++;
    /* <axiom> in the RHS of the dummy rule */
    ste_to_code [-EOF_ste] = --W.tmax;
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
	register int	ste;
	int	xnumpd = 1;
	int	xtnumpd = 1;
	int	xnumpg = 1;

	W.nt_string_length = 0;
	W.t_string_length = 0;
	adr = (int*) /* -W.xtmax-1,W.xntmax+1 */
		     sxalloc (W.xntmax + W.xtmax + 3, sizeof (int)) + W.xtmax + 1;
	WX = (struct ws_ntmax_s*) /* 0:xntmax+1 */
				  sxalloc (W.xntmax + 2, sizeof (struct ws_ntmax_s));
	tpd = (int*) /* -xtmax-1:-1 */
		     sxalloc (W.xtmax + 1, sizeof (int)) + W.xtmax + 1;
	bvide = sxba_calloc (W.xntmax + 1);

	{
	    register int	code, rule_no;

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

	    numpd = (int*) /* 1:maxnpd */ sxalloc ((W.maxnpd = xnumpd) + 1, sizeof (int));

	    for (code = -1; code >= -W.xtmax; code--) {
		ste = code_to_ste [code];
		adr [code] = W.t_string_length;
		W.t_string_length += sxstrlen (ste) + 1;
		/* on ajoute 0 en fin de chaine */
		tpd [code] = xtnumpd += code_to_rhs_nb [code];
	    }

	    tnumpd = (int*) /* 1:maxtpd */ sxalloc ((W.maxtpd = xtnumpd) + 1, sizeof (int));
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
	register int	i, code;

	for (i = W.indpro; i >= 1; i--) {
	    if ((code = WI [i].lispro) > 0)
		numpd [--WX [code].npd] = i;
	    else if (code < 0)
		tnumpd [--tpd [code]] = i;
	}
    }

    {
	/* construction of t_string, nt_string */
	M_nt = 0;
	M_t = 0;

	{
	    register int	code;

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
    sxfree (ste_to_code - st_top);
    sxfree (lhs_disp);
    sxfree (lhs_to_rule);
}
