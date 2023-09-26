/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1998 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL (pb).    		  *
   *                                                      *
   ******************************************************** */

/* Analogue aux Definite Clause Grammars, excepte que le squelette syntaxique
   est une RANGE Concatenation Grammar */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030520 10:37 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* Jeu  9 Avr 1998 13:56(pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/



/*   I N C L U D E S   */
#define NODE struct drcg_node
#include "varstr.h"
#include "sxunix.h"
#include "sxba.h"
#include "rcg.h"
#include <ctype.h>
#include <string.h>
#include "dcg_cst.h"
#if EBUG
static char	ME [] = "drcg";
#endif
char WHAT_DRCGSMP[] = "@(#)SYNTAX - $Id: drcg_smp.c 627 2007-06-13 09:04:26Z rlacroix $" WHAT_DEBUG;

struct drcg_node {
    SXNODE_HEADER_S VOID_NAME;

/*
your attribute declarations...
*/
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ARGUMENT_S_n 2
#define CATENATE_n 3
#define CLAUSE_n 4
#define CLAUSE_S_n 5
#define DIV_n 6
#define EMPTY_LIST_n 7
#define IS_n 8
#define LESS_n 9
#define LESS_EQUAL_n 10
#define LIST_BODY_n 11
#define MINUS_n 12
#define MOD_n 13
#define MULT_n 14
#define PLUS_n 15
#define PREDICATE_n 16
#define PREDICATE_S_n 17
#define PROLOG_S_n 18
#define SUP_n 19
#define SUP_EQUAL_n 20
#define TERM_n 21
#define UNARY_MINUS_n 22
#define VOID_n 23
#define atom_n 24
#define external_predicate_n 25
#define floating_point_number_n 26
#define integer_number_n 27
#define internal_predicate_n 28
#define variable_n 29
/*
E N D   N O D E   N A M E S
*/


/* E X T E R N   */

struct sxtables	drcg_tables;

/*   S T A T I C   */

static int		max_ste;
static int		ste, lhs_or_rhs, drcg_lhs_part, drcg_rhs_part;

static int		xprod, rhs_symb_nb, clause_nb;
static XH_header	drcg_part [2];
static XxY_header	drcg_rules;

static int		*DRCGprolon, DRCGmaxitem;

static int		lhs_param_nb, max_param_nb;
static int		*DRCGlhs2param_nb, *DRCGrhs2param_nb;
static int		*DRCGlhs_functor2param, *DRCGrhs_functor2param, *DRCGmain_terms;
static int		DRCGmain_terms_top, DRCGmain_terms_size;

static int		SXDRCGmaxrhsnt, rhsnt, son_nb;

static int		SXDRCGmax_atom, SXDRCGterm_size;
static int		*ste2atom, *atom2ste;

static int		SXDRCGmax_var;
static int		*var_stack, var_stack_top, *prod2var_nb;
static int		_ste;

static XH_header	term_hd;
static XxY_header	list_hd;
static int		*term_stack, term_stack_size, term_stack_top, tss;
static int		max_arg_per_prod, lhs_arg_nb, rhs_arg_nb;

static int		TERM_lgth, TERM_nb, LIST_nb;


/* Gestion des chaines formant les noms des variables */
static int		SXDRCGmax_gvar, SXDRCGmax_gvarppp, SXDRCGmax_varppp, gvar_nb, ppp_var_nb;
static int		*SXDRCG2var_disp, *ste2gvar, *gvar2ste;
static XH_header	SXDRCGvar_names;
static SXBA		*prod2gvar_set;

/* On a des expression arithmetiques, elles sont stockees comme des termes */
static BOOLEAN		has_integer_op;

/* Le prolog accepte */
static BOOLEAN		is_prolog, has_prolog, is_prolog_tree;
/* static int		Dcg_constant_ste; unused */
#if 0
static BOOLEAN		is_dynam_constant;
static int		*t2kind;
static int		*DRCGprod2dynam_constant;
#endif
static int		*PPPprolon, *PPPlispro, PPPmaxitem, ppp_symb_nb, maxpppprod;
static int		*PPPmain_terms, PPPmain_terms_top, PPPmain_terms_size;

#define prolog_name_nb	12
#define prolog_code_nb	11
#define prolog_param_nb	3

static int prolog_name2prolog_code [prolog_name_nb+1] = {
    0,
/* 1 */    MEMBER		/* member */,
/* 2 */    MEMBER		/* in */,
/* 3 */    CONCAT		/* concat */,
/* 4 */    SIZE			/* size */,
/* 5 */    NTH			/* nth */,
/* 6 */    REVERSE		/* reverse */,
/* 7 */    LESS			/* < */,
/* 8 */    SUP			/* > */,
/* 9 */    LESS_EQUAL		/* =< */,
/* 10 */   SUP_EQUAL		/* >= */,
/* 11 */   RANDOM		/* random */,
/* 12 */   IS			/* is */,
};

#if 0
/* UNUSED */
static int prolog_code2prolog_name [prolog_code_nb+1] = {
    0,
/* 1 */    /* concat */ 3,
/* 2 */    /* member, in */ 1, /* member est la classe d'equivalence */
/* 3 */    /* size */ 4,
/* 4 */    /* nth */ 5,
/* 5 */    /* reverse */ 6,
/* 6 */    /* < */7,
/* 7 */    /* > */8,
/* 8 */    /* =< */9,
/* 9 */    /* >= */10,
/* 10 */    /* random */11,
/* 11 */    /* is */12,
};
#endif /* 0 */



static char *prolog_name2str [prolog_name_nb+1] = {
    "",
    "member",
    "in",
    "concat",
    "size",
    "nth",
    "reverse",
    "<",
    ">",
    "=<",
    ">=",
    "random",
    "is",
};


#if 0
/* UNUSED */
static char *prolog_code2nt_str [prolog_code_nb+1] = {
    "",
    "Concat",
    "Member",
    "Size",
    "Nth",
    "Reverse",
    "Less",
    "Sup",
    "Less_Equal",
    "Sup_Equal",
    "Random",
    "Is",
};
#endif /* 0 */

static char *ppp2str [] = {
    "",
    "concat",
    "member",
    "size",
    "nth",
    "reverse",
    "<",
    ">",
    "=<",
    ">=",
    "random",
    "is",
    "/",
    "-" /* binary minus */,
    "mod",
    "*",
    "+",
    "-" /* unary minus */,
};

static int prolog_code2param_nb [prolog_code_nb+1] = {
    0,
    3				/* CONCAT */,
    2				/* MEMBER */,
    2				/* SIZE */,
    3				/* NTH */,
    2				/* REVERSE */,
    2				/* LESS */,
    2				/* SUP */,
    2				/* LESS_EQUAL */,
    2				/* SUP_EQUAL */,
    2				/* RANDOM */,
    2				/* IS */,
};

static int prolog_code2param_type [prolog_code_nb+1] [prolog_param_nb] = {
{0},
/* CONCAT(List,List,List) */ {(1<<VARIABLE) + (1<<STATIC_LIST), (1<<VARIABLE) + (1<<STATIC_LIST) , (1<<VARIABLE) + (1<<STATIC_LIST)},
/* MEMBER(Elem,List) */ {(1<<ATOM) + (1<<VARIABLE) + (1<<TERM) + (1<<STATIC_LIST) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<STATIC_LIST)},
/* SIZE(List,Int) */ {(1<<VARIABLE) + (1<<STATIC_LIST), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* NTH(List,Int,Elem) */ {(1<<VARIABLE) + (1<<STATIC_LIST), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<ATOM) + (1<<VARIABLE) + (1<<TERM) + (1<<STATIC_LIST) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* REVERSE(List,List) */ {(1<<VARIABLE) + (1<<STATIC_LIST), (1<<VARIABLE) + (1<<STATIC_LIST)},
/* LESS(Int,Int) */  {(1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* SUP(Int,Int) */  {(1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* LESS_EQUAL(Int,Int) */  {(1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* SUP_EQUAL(Int,Int) */  {(1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* RANDOM(Int,Int) */  {(1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* IS(Var,Elem) */  {(1<<VARIABLE), (1<<ATOM) + (1<<VARIABLE) + (1<<TERM) + (1<<STATIC_LIST) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
};

static int	*ste2prolog_code;
static SXBA	*var2pppitem_set;

static int	plus_ste, minus_ste, uminus_ste, mult_ste, div_ste, mod_ste,
                is_ste, less_ste, less_equal_ste, sup_ste, sup_equal_ste;

/* P R O C E D U R E S    */




#define out_define(str,val)	fprintf(F_drcg, "#define %s\t%i\n",str,val)


static XH_header	sub;
static int		*sub2start, *clause2prdct_nb, *sorted, *clause2xseq;
static int		*seq;
static int		*sorted2, *x2param_nb, *x2pos, *x2xseq, *clause2y;
static int		*seq2;

static void
sub_oflw (old_size, new_size)
int old_size, new_size;
{
    sub2start = (int*) sxrealloc (sub2start, new_size+1, sizeof (int));
}

static int
where_is (seq_ptr, maxl)
    int	**seq_ptr, maxl;
{
    int		i, j, x, l, start, top, size, *pseq, seq_top, seq_size;
    int		*where_is_seq = *seq_ptr;
	
    if (XH_set (&sub, &x)) return sub2start [x];

    sub2start [x] = start = seq_top = where_is_seq [0];

    if (seq_top + maxl >= (seq_size = where_is_seq [-1])) {
	*seq_ptr = (int*) sxrealloc (where_is_seq-1, (seq_size *=2)+1, sizeof (int))+1;
	where_is_seq = *seq_ptr;
	where_is_seq [-1] = seq_size;
    }

    top = XH_X (sub, x);

    for (i = 0; i < maxl; i++)
	where_is_seq [seq_top++] = XH_list_elem (sub, top++);

    top = where_is_seq [0] = seq_top;
    l = maxl;

    while (l--) {
	size = top>maxl ? maxl : top;

	for (i = size; i > 0; i--) {
	    pseq = &(where_is_seq [top-i]);

	    for (j = i; j > 0; j--) {
		XH_push (sub, *pseq++);
	    }

	    if (!XH_set (&sub, &x))
		sub2start [x] = top-i;
	}

	top--;
    }

    return start;
}

static BOOLEAN
less_equal (l, r)
    int l, r;
{
    return clause2prdct_nb [l] <= clause2prdct_nb [r];
}

static BOOLEAN
leq_param_nb (l, r)
    int l, r;
{
    return x2param_nb [l] <= x2param_nb [r];
}


static BOOLEAN
drcg_write ()
{
    static char		drcg_write_ME [] = "drcg_write";
    long		lt;
    FILE		*F_drcg;
    char		file_name [64], istr [8];
    VARSTR		vstr;
    int			prod, item, t;
    int			param_nb, x, *term_ptr, term, list;
    int			bot, top, atom, j, clause;
    int			SXDRCGmax_term, SXDRCGmax_list;
    int			gvar;
    int			term_top, val, ref, kind, ppp, ppp_term_top, param;
    int			logsize, logvar, filter, y, pos, l;

    static char *kind2str [] = {
	"",
	/* ATOM		1 */ "A",
	/* VARIABLE	2 */ "V",
	/* TERM		3 */ "T",
	/* STATIC_LIST	4 */ "SL",
	/* DYNAM_LIST	5 */ "DL",
	/* INTEGER_CONSTANT	6 */ "IC",
	/* DYNAM_ATOM	7 */ "DA",
	/* INTEGER_OP	8 */ "IO",
    };
  

    if ((F_drcg = fopen (strcat (strcpy (file_name, prgentname), "_drcgt.c"), "w")) == NULL) {
	if (sxverbosep && !sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = TRUE;
	}

	fprintf (sxstderr, "%s: cannot open (create) ", drcg_write_ME);
	sxperror (file_name);
	return FALSE;
    }

    vstr = varstr_alloc (64);

    lt = time (NULL);
    /* *ctime() = "Wed Aug 21 11:02:26 1996\n\0" */
    /* the 25th character is a "\n" */
    fprintf (F_drcg, "static char	what [] = \"%c(#)Definite Range Concatenation Grammar Parser for \\\"%s\\\" - SYNTAX [unix] - %.24s\";\n",
	     '@', prgentname, ctime (&lt)); /* '@' pour ne pas avoir @(#) dans l'objet */
    
    fprintf (F_drcg, "static char	ME [] = \"%s_drcgt.c\";\n\n", prgentname);

    fputs ("#include \"sxba.h\"\n", F_drcg);
    fputs ("#include \"drcg_glbl.h\"\n", F_drcg);

    out_define ("SXDRCGmax_var", SXDRCGmax_var);
    out_define ("SXDRCGmax_gvar", SXDRCGmax_gvar);
    out_define ("SXDRCGrhs_maxnt", SXDRCGmaxrhsnt);
    out_define ("SXDRCGmax_atom", SXDRCGmax_atom);
    SXDRCGmax_term = term_top = XH_top (term_hd)-1;
    out_define ("SXDRCGmax_term", SXDRCGmax_term);
    SXDRCGmax_list = XxY_top (list_hd);
    out_define ("SXDRCGmax_list", SXDRCGmax_list);
    out_define ("SXDRCGhas_integer_op", has_integer_op ? 1 : 0);
    out_define ("SXDRCGmaxpppprod", maxpppprod);


    XH_alloc (&sub, "sub", rule_nb*SXDRCGmaxrhsnt+1, 1, SXDRCGmaxrhsnt, sub_oflw, NULL);
    sub2start = (int*) sxalloc (XH_size (sub)+1, sizeof (int));
    seq = (int*) sxalloc (rule_nb*SXDRCGmaxrhsnt+1+1, sizeof (int))+1;
    seq [-1] = rule_nb*SXDRCGmaxrhsnt+1;
    seq[0] = 1;
    clause2prdct_nb = (int*) sxalloc (rule_nb+1, sizeof (int));
    sorted = (int*) sxalloc (rule_nb+1, sizeof (int));
    clause2xseq = (int*) sxalloc (rule_nb+1, sizeof (int));

    for (prod = 1; prod <= rule_nb; prod++) {
	clause2prdct_nb [prod] = DRCGprolon [prod+1] - DRCGprolon [prod] + 1;
	sorted [prod] = prod;
    }

    sort_by_tree (sorted, 1, rule_nb, less_equal);

    for (prod = rule_nb; prod > 0; prod--) {
	clause = sorted [prod];
	XH_push (sub, DRCGlhs2param_nb [clause]);

	for (item = DRCGprolon [clause]; item < DRCGprolon [clause+1]; item++) {
	    XH_push (sub, DRCGrhs2param_nb [item]);
	}

	clause2xseq [clause] = where_is (&seq, clause2prdct_nb [clause]);
    }

    fprintf (F_drcg, "\n\nstatic int param_nb_seq [%i] = {0\n", seq [0]);

    for (bot = 1; bot < seq [0]; bot++) {
	fprintf (F_drcg, ", /* %i */ %i", bot, seq [bot]);
    }

    fputs ("};\n", F_drcg);

    fprintf (F_drcg, "\nstatic int *SXDRCG2param_nb [%i] = {\n\
/* 0 */ (int*)NULL,\n", rule_nb+1);

    for (prod = 1; prod <= rule_nb; prod++) {
	fprintf (F_drcg, "/* %i */ param_nb_seq+%i,\n", prod, clause2xseq [prod]);
    }

    fputs ("};\n", F_drcg);

#if 0
    fprintf (F_drcg, "static int SXDRCG2param_nb [%i][%i] = {\n\
/* 0 */ {0, 0,},\n", rule_nb+1, SXDRCGmaxrhsnt+1);

    for (prod = 1; prod <= rule_nb; prod++) {
	fprintf (F_drcg, "/* %i */ {%i, ", prod, DRCGlhs2param_nb [prod]);

	for (item = DRCGprolon [prod]; item < DRCGprolon [prod+1]; item++) {
	    fprintf (F_drcg, "%i, ", DRCGrhs2param_nb [item]);
	}

	fputs ("},\n", F_drcg);
    }

    fputs ("};\n", F_drcg);
#endif

    fprintf (F_drcg, "\n\nstatic int SXDRCG2var_nb [%i] = {\n\
/* 0 */ 0,\n", rule_nb+1);

    for (prod = 1; prod <= rule_nb; prod++) {
	fprintf (F_drcg, "/* %i */ %i,\n", prod, prod2var_nb [prod]);
    }

    fputs ("};", F_drcg);

    fprintf (F_drcg, "\n\nstatic int SXDRCG2gvar_disp [%i] = {\n\
/* 0 */ 0,\n", rule_nb+1);

    for (prod = 1; prod <= rule_nb; prod++) {
	x = SXDRCG2var_disp [prod];

	if (x != 0)
	    x = XH_X (SXDRCGvar_names, x);

	fprintf (F_drcg, "/* %i */ %i,\n", prod, x);
    }

    fputs ("};", F_drcg);

    fprintf (F_drcg, "\n\nstatic int SXDRCGgvar_name [%i] = {\n\
/* 0 */ 0,\n", XH_list_top (SXDRCGvar_names));

    for (x = 1; x < XH_top (SXDRCGvar_names); x++) {
	fprintf (F_drcg, "/* %i */ ", x);

	bot = XH_X (SXDRCGvar_names, x);
	top = XH_X (SXDRCGvar_names, x+1);

	do {
	    fprintf (F_drcg, "%i, ", XH_list_elem (SXDRCGvar_names, bot));
	} while (++bot < top);

	fputs ("\n", F_drcg);
    }

    fputs ("};", F_drcg);

    if (SXDRCGmax_term > 0) {
	fprintf (F_drcg, "\n\nstatic int SXDRCGterm_disp [%i] = {\n\
/* 0 */ 0,\n\
/* 1 */ 1,\n", term_top+2);
	top = 1;

	for (term = 2; term <= term_top+1; term++) {
	    top = XH_X (term_hd, term);
	    fprintf (F_drcg, "/* %i */ %i,\n", term, top);
	}
    
	fputs ("};", F_drcg);

	fprintf (F_drcg, "\n\nstatic int SXDRCGterm_list [%i] = {\n\
/* 0 */ 0,\n", top);

	for (term = 1; term <= term_top; term++) {
	    fprintf (F_drcg, "/* %i */ ", term);
	    top = XH_X (term_hd, term+1);

	    for (bot = XH_X (term_hd, term); bot < top; bot++) {
		ref = XH_list_elem (term_hd, bot);
		kind = REF2KIND (ref);
		val = REF2VAL (ref);
		fprintf (F_drcg, "%i /* (%s, %i) */, ", ref, kind2str [kind], val);
	    }

	    fputs ("\n", F_drcg);
	}
    
	fputs ("};", F_drcg);
    }

    if (SXDRCGmax_list > 0) {
	fprintf (F_drcg, "\n\nstatic int SXDRCGlist_list [%i] = {\n\
/* 0 */ 0,\n", 2*SXDRCGmax_list+1);
	for (list = 1; list <= SXDRCGmax_list; list++) {
	    ref = XxY_X (list_hd, list);
	    kind = REF2KIND (ref);
	    val = REF2VAL (ref);
	    fprintf (F_drcg, "/* %i */ %i /* (%s, %i) */, ", list, ref, kind2str [kind], val);

	    ref = XxY_Y (list_hd, list);
	    kind = REF2KIND (ref);
	    val = REF2VAL (ref);
	    fprintf (F_drcg, "%i /* (%s, %i) */,\n", ref, kind2str [kind], val);
	}
    
	fputs ("};", F_drcg);
    }

    /* Traitement de SXDRCG2term [prod][nt][param] = var|atom|term|list */
    logsize = sxlast_bit (SXDRCGmaxrhsnt+1);
    filter = (1 << logsize) - 1;
    seq2 = (int*) sxalloc (rule_nb*(SXDRCGmaxrhsnt+1)+1+1, sizeof (int))+1;
    seq2 [-1] = rule_nb*(SXDRCGmaxrhsnt+1)+1;
    seq2 [0] = 1;
    clause2y = (int*) sxalloc (rule_nb+1, sizeof (int));
    sorted2 = (int*) sxalloc (seq2 [-1]+1, sizeof (int));
    x2param_nb = (int*) sxalloc (seq2 [-1]+1, sizeof (int));
    x2pos = (int*) sxalloc (seq2 [-1]+1, sizeof (int));
    x2xseq = (int*) sxalloc (seq2 [-1]+1, sizeof (int));


    x = 0;
    
    for (prod = 1; prod <= rule_nb; prod++) {
	sorted2 [x] = x;
	x2param_nb [x] = DRCGlhs2param_nb [prod];
	x2pos [x] = prod<<logsize;
	x++;

	item = DRCGprolon [prod];
	j = 1;

	for (j = 1, item = DRCGprolon [prod]; item < DRCGprolon [prod+1]; j++, item++) {
	    sorted2 [x] = x;
	    x2param_nb [x] = DRCGrhs2param_nb [item];
	    x2pos [x] = (prod<<logsize)+j;
	    x++;
	}
    }

    sort_by_tree (sorted2, 0, x-1, leq_param_nb);
    XH_clear (&sub);

    while(--x >= 0) {
	y = sorted2 [x];
	param_nb = x2param_nb [y];
	pos = x2pos [y];
	j = pos & filter;
	prod = pos >> logsize;

	if (param_nb) {
	    if (j == 0) {
		/* LHS */
		term_ptr = &(DRCGmain_terms [DRCGlhs_functor2param [prod]]);

		for (l = 0; l < param_nb; l++) {
		    ref = *term_ptr++;
		    XH_push (sub, ref);
		}
	    }
	    else {
		/* RHS */
		item = DRCGprolon [prod]+j-1;
		term_ptr = &(DRCGmain_terms [DRCGrhs_functor2param [item]]);

		for (l = 0; l < param_nb; l++) {
		    ref = *term_ptr++;
		    XH_push (sub, ref);
		}
	    }

	    x2xseq [y] = where_is (&seq2, param_nb);
	}
	else
	    x2xseq [y] = 0;

	if (j == 0)
	    /* LHS */
	    clause2y [prod] = y;
    }

    seq [0] = 1;		/* CLEAR */
    XH_clear (&sub);

    for (prod = rule_nb; prod > 0; prod--) {
	clause = sorted [prod];
	y = clause2y [clause];
	XH_push (sub, x2xseq [y++]);

	for (item = DRCGprolon [clause]; item < DRCGprolon [clause+1]; item++) {
	    XH_push (sub, x2xseq [y++]);
	}

	clause2xseq [clause] = where_is (&seq, clause2prdct_nb [clause]);
    }

    fputs ("\n\n/* SXDRCG2term [prod][nt][param] = var|atom|term|list */\n", F_drcg);
    fprintf (F_drcg, "static int term_seq [%i] = {0\n", seq2 [0]);

    for (bot = 1; bot < seq2 [0]; bot++) {
	ref = seq2 [bot];
	kind = REF2KIND (ref);
	val = REF2VAL (ref);
	fprintf (F_drcg, ", /* %i */ %i /* (%s, %i) */", bot, ref, kind2str [kind], val);
    }

    fputs ("};\n", F_drcg);

    fprintf (F_drcg, "\n\nstatic int *prdct_seq [%i] = {(int*)NULL\n", seq [0]);

    for (bot = 1; bot < seq [0]; bot++) {
	fprintf (F_drcg, ", /* %i */ term_seq+%i\n", bot, seq [bot]);
    }

    fputs ("};\n", F_drcg);

    fprintf (F_drcg, "static int **SXDRCG2term [%i] = {\n\
/* 0 */ (int**)NULL\n", rule_nb+1);

    for (prod = 1; prod <= rule_nb; prod++) {
	fprintf (F_drcg, ", /* %i */ prdct_seq+%i\n", prod, clause2xseq [prod]);
    }

    fputs ("};\n", F_drcg);    


    XH_free (&sub);
    sxfree (sub2start), sub2start = NULL;
    sxfree (seq-1), seq = NULL;
    sxfree (sorted), sorted = NULL;
    sxfree (clause2prdct_nb), clause2prdct_nb = NULL;
    sxfree (clause2xseq), clause2xseq = NULL;

    sxfree (seq2-1), seq2 = NULL;
    sxfree (clause2y), clause2y = NULL;
    sxfree (sorted2), sorted2 = NULL;
    sxfree (x2param_nb), x2param_nb = NULL;
    sxfree (x2pos), x2pos = NULL;
    sxfree (x2xseq), x2xseq = NULL;

#if 0
    fputs ("\n\n/* SXDRCG2term [prod][nt][param] = var|atom|term|list */\n", F_drcg);
    fprintf (F_drcg, "static int SXDRCG2term [%i][%i][%i] = {\n\
/* 0 */ {{0}, },\n", rule_nb+1, SXDRCGmaxrhsnt+1, max_param_nb);

    for (prod = 1; prod <= rule_nb; prod++) {
	fprintf (F_drcg, "/* %i */ {", prod);

	fputs ("{", F_drcg);
	param_nb = DRCGlhs2param_nb [prod];

	if (param_nb > 0) {
	    term_ptr = &(DRCGmain_terms [DRCGlhs_functor2param [prod]]);

	    for (x = 0; x < param_nb; x++) {
		ref = *term_ptr++;
		kind = REF2KIND (ref);
		val = REF2VAL (ref);
		fprintf (F_drcg, "%i /* (%s, %i) */, ", ref, kind2str [kind], val);
	    }
	}
	else
	    fputs ("0", F_drcg);

	fputs ("}, ", F_drcg);

	item = DRCGprolon [prod];
	j = 0;

	for (item = DRCGprolon [prod]; item < DRCGprolon [prod+1]; item++) {
	    j++;
	    fputs ("{", F_drcg);
	    param_nb = DRCGrhs2param_nb [item];

	    if (param_nb > 0) {
		term_ptr = &(DRCGmain_terms [DRCGrhs_functor2param [item]]);

		for (x = 0; x < param_nb; x++) {
		    ref = *term_ptr++;
		    kind = REF2KIND (ref);
		    val = REF2VAL (ref);
		    fprintf (F_drcg, "%i /* (%s, %i) */, ", ref, kind2str [kind], val);
		}
	    }
	    else
		fputs ("0", F_drcg);

	    fputs ("}, ", F_drcg);
	}

	fputs ("},\n", F_drcg);
    }

    fputs ("};\n\n", F_drcg);
#endif

    fputs ("\nstatic char *SXDRCGatom_str [] = {\n\
/* 0 */ \"\",\n", F_drcg);

    for (atom = 1; atom <= SXDRCGmax_atom; atom++) {
	fprintf (F_drcg, "/* %i */ \"%s\",\n", atom, sxstrget (atom2ste [atom]));
    }

    fputs ("};\n", F_drcg);

    fputs ("\nstatic char *SXDRCGvar_str [] = {\n", F_drcg);

    for (gvar = 0; gvar <= SXDRCGmax_gvar; gvar++) {
	fprintf (F_drcg, "/* %i */ \"%s\",\n", gvar, sxstrget (gvar2ste [gvar]));
    }

    fputs ("};\n", F_drcg);

#if 0
    if (SXDRCGmax_var <= 1)
	fprintf (F_drcg, "\n#define SXDRCGclear_vars(var_nb) %s;\n", SXDRCGmax_var==0? "0" : "*var_ptr = 0");
    else {
	int var;

	fputs ("\n#define SXDRCGclear_vars(var_nb)\\\n\
switch (var_nb) {\\\n", F_drcg);

	for (var = SXDRCGmax_var; var > 0 ; var--) {
	    if (var > 1)
		fprintf (F_drcg, "\
case %i:\\\n\
   *var_ptr-- = 0;\\\n", var);
	    else
		fprintf (F_drcg, "\
case %i:\\\n\
   *var_ptr = 0;\\\n", var);
	}

	fputs ("}\n", F_drcg);
    }
#endif

    fputs ("\nstatic char *SXDRCGt2str [] = {\n\
/* 0 */ \"\",\n", F_drcg);

    for (t = 1; t <= max_t; t++) {
	fprintf (F_drcg, "/* %i */ \"%s\",\n", t, sxstrget (t2ste [t]));
    }

    fputs ("};\n", F_drcg);

    out_define ("SXDRCGis_dynam_atom", da_top > 0 ? 1 : 0);
    logvar = sxlast_bit (SXDRCGmax_var);
    out_define ("SXDRCGlogvar", logvar);

    if (da_top) {
	/* Il y a des atomes dynamiques */
	int xda, nb;


	fputs ("\nstatic int SXDRCGprod2da_disp [] = {\n\
/* 0 */ 0,\n", F_drcg);

	for (prod = 1; prod <= rule_nb+1; prod++)
	    fprintf (F_drcg, "/* %i */ %i,\n", prod, clause2da_disp [prod]/2);

	fputs ("};\n", F_drcg);
	    
	fputs ("\nstatic int SXDRCGprod2da [] = {\n", F_drcg);

	for (prod = 1; prod <= rule_nb; prod++) {
	    nb = clause2da_disp [prod+1]-clause2da_disp [prod];

	    if (nb) {
		fprintf (F_drcg, "/* %i */ ", prod);

		for (xda = clause2da_disp [prod]; xda < clause2da_disp [prod+1]; xda += 2) {
		    int X = clause2da [xda+1];
		    fprintf (F_drcg, "%i /* (%i, %i) */, ", (clause2da [xda]<<logvar)+X, clause2da [xda], X);
		}

		fputs ("\n", F_drcg);
	    }
	}

	fputs ("0};\n", F_drcg);
    }

#if 0
    out_define ("SXDRCGis_dynam_constant", is_dynam_constant ? 1 : 0);

    if (is_dynam_constant) {
	fputs ("\nstatic int SXDRCGprod2dc [] = {\n\
/* 0 */ 0,\n", F_drcg);

	for (prod = 1; prod <= rule_nb; prod++) {
	    fprintf (F_drcg, "/* %i */ %i,\n", prod, DRCGprod2dynam_constant [prod]);
	}

	fputs ("};\n", F_drcg);
    }
#endif

    out_define ("SXDRCGhas_prolog", has_prolog ? 1 : 0);
    out_define ("SXDRCGmax_gvarppp", SXDRCGmax_gvarppp);
    out_define ("SXDRCGmax_varppp", SXDRCGmax_varppp);

    if (has_prolog) {
	int	*line2main;

	fprintf (F_drcg, "\nstatic int SXDRCGppp_prolon [%i] = {\n", maxpppprod+2);

	for (prod = 0; prod <= maxpppprod+1; prod++) {
	    fprintf (F_drcg, "/* %i */ %i,\n", prod, PPPprolon [prod]);
	}

	fputs ("};\n", F_drcg);

	fprintf (F_drcg, "\nstatic int SXDRCGppp_lispro [%i] = {0,\n", PPPmaxitem+1);

	for (prod = 1; prod <= maxpppprod; prod++) {
	    top = PPPprolon [prod+1];
	    bot = PPPprolon [prod];

	    if (bot < top) {
		fprintf (F_drcg, "/* %i */ ", prod);

		do {
		    ppp = PPPlispro [bot];
		    fprintf (F_drcg, "%i /* %s */, ", ppp, ppp2str [ppp]);
		} while (++bot < top);

		fputs ("\n", F_drcg);
	    }
	}

	fputs ("};\n", F_drcg);

	fprintf (F_drcg, "\nstatic int SXDRCGppp_term_hd [%i] = {0,\n", PPPmaxitem+2);

	ppp_term_top = 0;

	for (prod = 1; prod <= maxpppprod; prod++) {
	    top = PPPprolon [prod+1];
	    bot = PPPprolon [prod];

	    if (bot < top) {
		fprintf (F_drcg, "/* %i */ ", prod);

		do {
		    fprintf (F_drcg, "%i, ", ppp_term_top);
		    ppp_term_top += prolog_code2param_nb [PPPlispro [bot]];
		} while (++bot < top);

		fputs ("\n", F_drcg);
	    }
	}

	fprintf (F_drcg, "/* %i */ %i\n};\n", prod, ppp_term_top);

	fprintf (F_drcg, "\nstatic int SXDRCGppp_term [%i] = {\n", ppp_term_top);

	ppp_term_top = 0;

	for (prod = 0; prod <= maxpppprod; prod++) {
	    top = PPPprolon [prod+1];
	    bot = PPPprolon [prod];

	    if (bot < top) {
		fprintf (F_drcg, "/* %i */ ", prod);

		do {
		    param_nb = prolog_code2param_nb [PPPlispro [bot]];

		    for (param = 1; param <= param_nb;param++) {
			ref = PPPmain_terms [ppp_term_top++];
			kind = REF2KIND (ref);
			val = REF2VAL (ref);
			fprintf (F_drcg, "%i /* (%s, %i) */, ", ref, kind2str [kind], val);
		    }
		} while (++bot < top);

		fputs ("\n", F_drcg);
	    }
	}

	fputs ("};\n", F_drcg);

	line2main = (int*) sxalloc (SXDRCGmax_varppp+1, sizeof (int));

	for (top = 1; top <= SXDRCGmax_varppp; top++) {
	    for (bot = 1; bot < top; bot++) {
		if (line2main [bot] == bot) {
		    if (sxba_first_difference (var2pppitem_set [bot], var2pppitem_set [top]) == -1)
			break;
		}
	    }

	    line2main [top] = (bot == top) ? top : bot;
	}

	for (bot = 1; bot <= SXDRCGmax_varppp; bot++) {
	    if (line2main [bot] == bot) {
		sprintf (istr, "%i", bot);
		sxba_to_c (var2pppitem_set [bot], F_drcg, "line_", istr, TRUE /* static */);
	    }
	}

	fprintf (F_drcg, "static SXBA_ELT *SXDRCGvar2ppp_item_set [%i] = {(SXBA_ELT *)NULL\n", SXDRCGmax_varppp+1);

	for (bot = 1; bot <= SXDRCGmax_varppp;bot++) {
	    fprintf (F_drcg, ", line_%i\n", line2main [bot]);
	}

	fputs ("};\n", F_drcg);

	sxfree (line2main), line2main = NULL;

	fputs ("#if 0\n", F_drcg);
	sxbm2c (F_drcg, var2pppitem_set, SXDRCGmax_varppp+1, "SXDRCGvar2ppp_item_set", "",
		TRUE		/* is_static */, vstr);
	fputs ("#endif\n", F_drcg);
    }

    if (has_prolog || has_integer_op) {
	fputs ("\nstatic char *SXDRCGppp2str [] = {\n\
/* 0 */ \"\",\n", F_drcg);

	for (ppp = 1; ppp <= LAST_PPP; ppp++) {
	    fprintf (F_drcg, "/* %i */ \"%s\",\n", ppp, ppp2str [ppp]);
	}

	fputs ("};\n", F_drcg);
    }


    fputs ("static struct drcg_struct drcg_struct = {\n", F_drcg);

    fputs ("{", F_drcg);
    fputs ("SXDRCGmax_var, ", F_drcg);
    fputs ("SXDRCGmax_gvar, ", F_drcg);
    fputs ("SXDRCGrhs_maxnt, ", F_drcg);
    fputs ("SXDRCGmax_atom, ", F_drcg);
    fputs ("SXDRCGmax_term, ", F_drcg);
    fputs ("SXDRCGmax_list, ", F_drcg);
    fputs ("SXDRCGhas_integer_op, ", F_drcg);
    fputs ("SXDRCGmaxpppprod, ", F_drcg);
    fputs ("SXDRCGis_dynam_atom, ", F_drcg);
    fputs ("SXDRCGlogvar, ", F_drcg);
    fputs ("SXDRCGhas_prolog, ", F_drcg);
    fputs ("SXDRCGmax_gvarppp, ", F_drcg);
    fputs ("SXDRCGmax_varppp", F_drcg);
    fputs ("},\n", F_drcg);

    fputs ("SXDRCG2param_nb,\n", F_drcg);
    fputs ("SXDRCG2var_nb,\n", F_drcg);
    fputs ("SXDRCG2gvar_disp,\n", F_drcg);
    fputs ("SXDRCGgvar_name,\n", F_drcg);

    if (SXDRCGmax_term > 0) {
	fputs ("SXDRCGterm_disp,\n", F_drcg);
	fputs ("SXDRCGterm_list,\n", F_drcg);
    }
    else {
	fputs ("NULL /* SXDRCGterm_disp */,\n", F_drcg);
	fputs ("NULL /* SXDRCGterm_list */,\n", F_drcg);
    }

    if (SXDRCGmax_list > 0) {
	fputs ("SXDRCGlist_list,\n", F_drcg);
    }
    else {
	fputs ("NULL /* SXDRCGlist_list */,\n", F_drcg);
    }

    fputs ("SXDRCG2term,\n", F_drcg);

    if (da_top) {
	fputs ("SXDRCGprod2da_disp,\n", F_drcg);
	fputs ("SXDRCGprod2da,\n", F_drcg);
    }
    else {
	fputs ("NULL /* SXDRCGprod2da_disp */,\n", F_drcg);
	fputs ("NULL /* SXDRCGprod2da */,\n", F_drcg);
    }

    if (has_prolog) {
	fputs ("SXDRCGppp_prolon,\n", F_drcg);
	fputs ("SXDRCGppp_lispro,\n", F_drcg);
	fputs ("SXDRCGppp_term_hd,\n", F_drcg);
	fputs ("SXDRCGppp_term,\n", F_drcg);
    }
    else {
	fputs ("NULL /* SXDRCGppp_prolon */,\n", F_drcg);
	fputs ("NULL /* SXDRCGppp_lispro */,\n", F_drcg);
	fputs ("NULL /* SXDRCGppp_term_hd */,\n", F_drcg);
	fputs ("NULL /* SXDRCGppp_term */,\n", F_drcg);
    }

    fputs ("SXDRCGatom_str,\n", F_drcg);
    fputs ("SXDRCGvar_str,\n", F_drcg);
    fputs ("SXDRCGt2str,\n", F_drcg);

    if (has_prolog || has_integer_op) {
	fputs ("SXDRCGppp2str,\n", F_drcg);
    }
    else {
	fputs ("NULL /* SXDRCGppp2str */,\n", F_drcg);
    }

    if (has_prolog) {
	fputs ("SXDRCGvar2ppp_item_set,\n", F_drcg);
    }
    else {
	fputs ("NULL /* SXDRCGvar2ppp_item_set */,\n", F_drcg);
    }

    fputs ("};\n", F_drcg);


    fprintf (F_drcg, "\nvoid SXDRCG_%s_sem_fun (for_semact,local_pid)\n", prgentname);
    fputs ("struct for_semact *for_semact;\n", F_drcg);
    fputs ("int local_pid;\n", F_drcg);
    fputs ("{\n", F_drcg);
    fputs ("#if is_multiple_pass\n", F_drcg);
    fputs ("for_semact->second_pass_init = DRCGsem_fun_init;\n", F_drcg);
    fputs ("for_semact->second_pass = DRCGsem_fun;\n", F_drcg);
    fputs ("for_semact->second_pass_final = DRCGsem_fun_final;\n", F_drcg);
    fputs ("#else\n", F_drcg);
    fputs ("for_semact->first_pass_init = DRCGsem_fun_init;\n", F_drcg);
    fputs ("for_semact->first_pass = DRCGsem_fun;\n", F_drcg);
    fputs ("for_semact->first_pass_final = DRCGsem_fun_final;\n", F_drcg);
    fputs ("#endif\n", F_drcg);
    fputs ("for_semact->last_pass = DRCGsem_fun_post;\n", F_drcg);
    fputs ("drcg [local_pid] = drcg_struct;\n", F_drcg);
    fputs ("}\n", F_drcg);

    varstr_free (vstr);

    fclose (F_drcg);

    return TRUE;
}


static void
prolog_init ()
{
    /* Il y a des tokens dont le nom est celui de predicats prolog predefinis, il s'agit
       peut etre de d'appels prolog  i.e. { ... } */
    int	name, prolog_init_ste, code;

    for (name = 1; name <= prolog_name_nb; name++) {
	prolog_init_ste = sxstrretrieve (prolog_name2str [name]);

	if (prolog_init_ste != ERROR_STE) {
	    code = prolog_name2prolog_code [name];
	    ste2prolog_code [prolog_init_ste] = code;
	}
    }
}

void
drcg_pass ()
{
    static struct {
	       struct sxsvar	sxsvar;
	       struct sxplocals	sxplocals;
	       struct sxatcvar	sxatcvar;
	   }	rcg;


/* I N I T I A L I Z A T I O N S    */


/* A L L O C A T I O N S   */

/* On memorise les variables des processeurs utilises par rcg */

    rcg.sxsvar = sxsvar;
    rcg.sxplocals = sxplocals;
    rcg.sxatcvar = sxatcvar;

/* Reinitialisation du source   */

    rewind (sxsrcmngr.infile);
    sxsrc_mngr (INIT, sxsrcmngr.infile, sxsrcmngr.source_coord.file_name);

/* On reutilise la string table de rcg */

    syntax (OPEN, &drcg_tables) /* Initialisation de SYNTAX */ ;
    syntax (ACTION, &drcg_tables);

/*   F I N A L I Z A T I O N   */

/* The current tables are closed. */

    syntax (CLOSE, &drcg_tables);

/* On restaure les variables des processeurs utilises par rcg */

    sxsvar = rcg.sxsvar;
    sxplocals = rcg.sxplocals;
    sxatcvar = rcg.sxatcvar;
    sxsrc_mngr (FINAL);

    if (IS_ERROR)
	sxtmsg (sxsrcmngr.source_coord.file_name, "%sTables are not generated.", drcg_tables.err_titles [2]);
}


static void
process_nt (process_nt_ste, is_nt)
    int 	process_nt_ste;
    BOOLEAN	is_nt;
{
    if (is_nt && lhs_or_rhs == 1) 
	rhs_symb_nb++;

    XH_push (drcg_part [lhs_or_rhs], process_nt_ste);

    if (tss > term_stack_size)
	term_stack_size = tss;
}

static void
drcg_pi ()
{
    /* I N H E R I T E D */

    switch (VISITED->father->name) {

    case ERROR_n :
	break;

    case ARGUMENT_S_n :		/* VISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, variable_n} */
	break;

    case CATENATE_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {CATENATE_n, EMPTY_LIST_n, LIST_BODY_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #1");
#endif
	  break;

	}

	break;

    case CLAUSE_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = PREDICATE_n */
	    lhs_or_rhs = 0;
	    lhs_arg_nb = 0;
	    clause_nb++;
	    break;

	case 2 :		/* VISITED->name = PREDICATE_S_n */
	    XH_set (&drcg_part [lhs_or_rhs], &drcg_lhs_part);
	    lhs_or_rhs = 1;
	    ppp_var_nb = 0;
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #3");
#endif
	  break;

	}

	break;

    case CLAUSE_S_n :		/* VISITED->name = CLAUSE_n */
	break;

    case DIV_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #4");
#endif
	  break;

	}

	break;

    case IS_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = variable_n */
	    break;

	case 2 :		/* VISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #5");
#endif
	  break;

	}

	break;

    case LESS_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #6");
#endif
	  break;

	}

	break;

    case LESS_EQUAL_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #7");
#endif
	  break;

	}

	break;

    case LIST_BODY_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {CATENATE_n, LIST_BODY_n, VOID_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #8");
#endif
	  break;

	}

	break;

    case MINUS_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #9");
#endif
	  break;

	}

	break;

    case MOD_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #10");
#endif
	  break;

	}

	break;

    case MULT_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #11");
#endif
	  break;

	}

	break;

    case PLUS_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #12");
#endif
	  break;

	}

	break;

    case PREDICATE_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {external_predicate_n, internal_predicate_n} */
	    break;

	case 2 :		/* VISITED->name = {ARGUMENT_S_n, VOID_n} */
	    tss = 0;
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #13");
#endif
	  break;

	}

	break;

    case PREDICATE_S_n :	/* VISITED->name = {PREDICATE_n, PROLOG_S_n} */
	break;

    case PROLOG_S_n :		/* VISITED->name = {IS_n, LESS_n, LESS_EQUAL_n, SUP_n, SUP_EQUAL_n, TERM_n} */
	tss = 0;
	is_prolog_tree = TRUE;
	break;

    case SUP_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #14");
#endif
	  break;

	}

	break;

    case SUP_EQUAL_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #15");
#endif
	  break;

	}

	break;

    case TERM_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = atom_n */
	    break;

	case 2 :		/* VISITED->name = ARGUMENT_S_n */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #16");
#endif
	  break;

	}

	break;

    case UNARY_MINUS_n :	/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end drcg_pi */
}


static void
drcg_pd ()
{
    /* D E R I V E D */

    switch (VISITED->name) {

    case ERROR_n :
	break;

    case ARGUMENT_S_n :
	if (lhs_or_rhs == 0)
	    lhs_arg_nb += VISITED->degree;
	else
	    rhs_arg_nb += VISITED->degree;

	tss += VISITED->degree;
	break;

    case CATENATE_n :
	LIST_nb++;
	break;

    case CLAUSE_n :
	break;

    case CLAUSE_S_n :
	break;

    case DIV_n :
	TERM_nb++;
	TERM_lgth += 3;
	tss++;
	XH_push (drcg_part [lhs_or_rhs], div_ste);
	break;

    case EMPTY_LIST_n :
	tss++;

	if (is_prolog_tree)
	    PPPmain_terms_size++;
	else
	    DRCGmain_terms_size++;

	break;

    case IS_n :
	process_nt (is_ste, FALSE);
	break;

    case LESS_n :
	process_nt (less_ste, FALSE);
	break;

    case LESS_EQUAL_n :
	process_nt (less_equal_ste, FALSE);
	break;

    case LIST_BODY_n :
	tss++;
	LIST_nb++;

	if (is_prolog_tree)
	    PPPmain_terms_size++;
	else
	    DRCGmain_terms_size++;

	break;

    case MINUS_n :
	TERM_nb++;
	TERM_lgth += 3;
	tss++;
	XH_push (drcg_part [lhs_or_rhs], minus_ste);
	break;

    case MOD_n :
	TERM_nb++;
	TERM_lgth += 3;
	tss++;
	XH_push (drcg_part [lhs_or_rhs], mod_ste);
	break;

    case MULT_n :
	TERM_nb++;
	TERM_lgth += 3;
	tss++;
	XH_push (drcg_part [lhs_or_rhs], mult_ste);
	break;

    case PLUS_n :
	TERM_nb++;
	TERM_lgth += 3;
	tss++;
	XH_push (drcg_part [lhs_or_rhs], plus_ste);
	break;

    case PREDICATE_n :
	process_nt (VISITED->son->token.string_table_entry, TRUE);
	break;

    case PREDICATE_S_n :
    {	
	int rule_no, nb;

	XH_set (&drcg_part [lhs_or_rhs], &drcg_rhs_part);
	XxY_set (&drcg_rules, drcg_lhs_part, drcg_rhs_part, &rule_no);

	if ((nb = lhs_arg_nb + rhs_arg_nb) > max_arg_per_prod)
	    max_arg_per_prod = nb;

	if (ppp_var_nb > SXDRCGmax_varppp)
	    SXDRCGmax_varppp = ppp_var_nb;
    }
	break;

    case PROLOG_S_n :
	is_prolog = TRUE;
	is_prolog_tree = FALSE;
	ppp_symb_nb += VISITED->degree;
	break;

    case SUP_n :
	process_nt (sup_ste, FALSE);
	break;

    case SUP_EQUAL_n :
	process_nt (sup_equal_ste, FALSE);
	break;

    case TERM_n :
	if (VISITED->father->name == PROLOG_S_n) {
	    ste = VISITED->son->token.string_table_entry;

	    if (ste2prolog_code [ste] == 0)
		/* ppp fonctionnel inconnu */
		sxput_error (VISITED->son->token.source_index,
			     "%sThis prolog predicate is not (yet?) implemented.",
			     sxtab_ptr->err_titles [2]);
	    else {
		process_nt (ste, FALSE);
	    }
	}
	else {
	    TERM_nb++;
	    TERM_lgth += VISITED->son->brother->degree + 1;
	    tss++;
	}
	break;

    case UNARY_MINUS_n :
	TERM_nb++;
	TERM_lgth += 2;
	tss++;
	XH_push (drcg_part [lhs_or_rhs], uminus_ste);
	break;

    case VOID_n :
	break;

    case atom_n :
	ste = VISITED->token.string_table_entry;
	XH_push (drcg_part [lhs_or_rhs], ste);

	if (is_prolog_tree)
	    PPPmain_terms_size++;
	else
	    DRCGmain_terms_size++;

	break;

    case external_predicate_n :
	break;

    case floating_point_number_n :
	break;

    case integer_number_n :
	ste = VISITED->token.string_table_entry;
	XH_push (drcg_part [lhs_or_rhs], ste);

	if (is_prolog_tree)
	    PPPmain_terms_size++;
	else
	    DRCGmain_terms_size++;

	break;

    case internal_predicate_n :
	break;

    case variable_n :
    {
	int	gvar;

	ste = VISITED->token.string_table_entry;
	XH_push (drcg_part [lhs_or_rhs], ste);
	
	if (is_prolog_tree) {
	    /* On assigne les noms des variables (globales) en commencant par celles impliquees dans des ppp */
	    PPPmain_terms_size++;

	    if (ste == _ste) {
		gvar2ste [0] = _ste;
		/* ste2gvar [_ste] == 0 */
		/* ste2var [_ste] == 0 */
	    }
	    else {
		if ((gvar = ste2gvar [ste]) == 0) {
		    ste2gvar [ste] = gvar = ++SXDRCGmax_gvarppp;
		    gvar2ste [SXDRCGmax_gvarppp] = ste;
		}

		/* La variable courante est utilisee ds un ppp de la production courante */
		if (SXBA_bit_is_reset_set (prod2gvar_set [clause_nb], gvar))
		    ppp_var_nb++;
	    }
	}
	else {
	    DRCGmain_terms_size++;
	    gvar_nb++;
	}
    }

	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end drcg_pd */
}




static void
gen_ppp (gen_ppp_ste, param_nb, node_ptr)
    int 	gen_ppp_ste, param_nb;
    NODE	*node_ptr;
{
    int		code, bot, i, type, param, kind, x;
    /* On verifie le "type" des parametres */
    code = ste2prolog_code [gen_ppp_ste];
    bot = term_stack_top - param_nb;

    for (i = 0; bot++, i < param_nb;i++) {
	type = prolog_code2param_type [code] [i];
	param = term_stack [bot];
	kind = REF2KIND (param);

	if ((type & (1<<kind)) == 0) {
	    sxput_error (node_ptr->token.source_index,
			 "%sArgument type mismatch.",
			 sxtab_ptr->err_titles [2]);
	    break;
	}

	node_ptr = node_ptr->brother;
    }

    if (i == param_nb) {
	/* C'est un bon predicat, il faut generer le code... */
	/* On est ds la regle xprod, derriere l'item DRCGmaxitem */
	for (x = 1; x <= term_stack_top; x++) {
	    PPPmain_terms [PPPmain_terms_top++] = term_stack [x];
	}
    }

    term_stack_top = 0; /* On est au top level */
}

static void
integer_op (op, param_nb)
    int op, param_nb;
{
    int 	bot, x, term, ref;

    has_integer_op = TRUE;

    ref = KV2REF (INTEGER_OP, op);
    XH_push (term_hd, ref);

    bot = term_stack_top - param_nb + 1;
#if EBUG
    if (bot <= 0)
	sxtrap (ME, "integer_op");
#endif

    for (x = bot; x <= term_stack_top; x++) {
	ref = term_stack [x];
	XH_push (term_hd, ref);
    }

    XH_set (&term_hd, &(term));
    /* Les expressions arithmetiques sont stockees avec les termes */
    term_stack [bot] = KV2REF (INTEGER_OP, term);
    term_stack_top = bot;
}

static BOOLEAN
is_streq_spcl (clause, is_streq_spcl_son_nb)
    int clause, is_streq_spcl_son_nb;
{
    /* Regarde si le predicat de la rhs en position is_streq_spcl_son_nb ds clause est un StrEq("a", X) */
    int bot, top;

    for (top = clause2da_disp [clause+1], bot = clause2da_disp [clause]; bot < top; bot += 2) {
	if (clause2da [bot] == is_streq_spcl_son_nb)
	    return TRUE;
    }

    return FALSE;
}

static void
drcg_pi2 ()
{
    /* I N H E R I T E D */

    switch (VISITED->father->name) {

    case ERROR_n :
	break;

    case ARGUMENT_S_n :		/* VISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, variable_n} */
	break;

    case CATENATE_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {CATENATE_n, EMPTY_LIST_n, LIST_BODY_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #17");
#endif
	  break;

	}

	break;

    case CLAUSE_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = PREDICATE_n */
	    xprod++;
	    lhs_or_rhs = 0;
	    var_stack_top = sxba_cardinal (prod2gvar_set [xprod]);
	    ppp_var_nb = 0;
	    break;

	case 2 :		/* VISITED->name = PREDICATE_S_n */
	    lhs_or_rhs = 1;
	    DRCGlhs2param_nb [xprod] = lhs_param_nb;
	    son_nb = rhsnt = 0;
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #18");
#endif
	  break;

	}

	break;

    case CLAUSE_S_n :		/* VISITED->name = CLAUSE_n */
	break;

    case DIV_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #19");
#endif
	  break;
	  
	}

	break;

    case IS_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = variable_n */
	    break;

	case 2 :		/* VISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #20");
#endif
	  break;

	}

	break;

    case LESS_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #21");
#endif
	  break;

	}

	break;

    case LESS_EQUAL_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #22");
#endif
	  break;

	}

	break;

    case LIST_BODY_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {CATENATE_n, LIST_BODY_n, VOID_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #23");
#endif
	  break;

	}

	break;

    case MINUS_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #24");
#endif
	  break;

	}

	break;

    case MOD_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #25");
#endif
	  break;

	}

	break;

    case MULT_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #26");
#endif
	  break;

	}

	break;

    case PLUS_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #27");
#endif
	  break;

	}

	break;

    case PREDICATE_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {external_predicate_n, internal_predicate_n} */
	    break;

	case 2 :		/* VISITED->name = {ARGUMENT_S_n, VOID_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #28");
#endif
	  break;

	}

	break;

    case PREDICATE_S_n :	/* VISITED->name = {PREDICATE_n, PROLOG_S_n} */
	break;

    case PROLOG_S_n :		/* VISITED->name = {IS_n, LESS_n, LESS_EQUAL_n, SUP_n, SUP_EQUAL_n, TERM_n} */
    {
	is_prolog_tree = TRUE;

	if (VISITED->name == IS_n)
	    ste = is_ste;
	else if (VISITED->name == LESS_n)
	    ste = less_ste;
	else if (VISITED->name == LESS_EQUAL_n)
	    ste = less_equal_ste;
	else if (VISITED->name == SUP_n)
	    ste = sup_ste;
	else if (VISITED->name == SUP_EQUAL_n)
	    ste = sup_equal_ste;
	else if (VISITED->name == TERM_n)
	    ste = VISITED->son->token.string_table_entry;

	PPPlispro [++PPPmaxitem] = ste2prolog_code [ste];
	maxpppprod = xprod;
    }

	break;

    case SUP_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #29");
#endif
	  break;

	}

	break;

    case SUP_EQUAL_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;

	case 2 :		/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #30");
#endif
	  break;

	}

	break;

    case TERM_n :
	switch (VISITED->position) {
	case 1 :		/* VISITED->name = atom_n */
	    break;

	case 2 :		/* VISITED->name = ARGUMENT_S_n */
	    break;
	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #31");
#endif
	  break;

	}

	break;

    case UNARY_MINUS_n :	/* VISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
				   integer_number_n, variable_n} */
	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end drcg_pi2 */
}

static void
drcg_pd2 ()
{
    /* D E R I V E D */
    int var_nb;
    
    switch (VISITED->name) {

    case ERROR_n :
	break;

    case ARGUMENT_S_n :
	break;

    case CATENATE_n :
    {
	int op1, op2, op;

	op1 = term_stack [term_stack_top-1];
	op2 = term_stack [term_stack_top];

#if EBUG
	if (REF2KIND (op1) != STATIC_LIST && REF2KIND (op2) != VARIABLE)
	    sxtrap (ME, "drcg_pd2");
#endif

	XxY_set (&list_hd, op1, op2, &op);

	term_stack [--term_stack_top] = KV2REF (STATIC_LIST, 2*op-1);
    }
	break;

    case CLAUSE_n :
	break;

    case CLAUSE_S_n :
	break;

    case DIV_n :
	integer_op (DIVIDE, 2);
	break;

    case EMPTY_LIST_n :
	term_stack [++term_stack_top] = EMPTY_LIST;
	break;

    case IS_n :
	gen_ppp (is_ste, 2, VISITED->son);
	break;

    case LESS_n :
	gen_ppp (less_ste, 2, VISITED->son);
	break;

    case LESS_EQUAL_n :
	gen_ppp (less_equal_ste, 2, VISITED->son);
	break;

    case LIST_BODY_n :
    {
	int op1, op2, op;

	if (VISITED->son->brother->name == VOID_n) {
	    term_stack [++term_stack_top] = EMPTY_LIST;
	}

	op1 = term_stack [term_stack_top-1];
	op2 = term_stack [term_stack_top];

#if EBUG
	if (REF2KIND (op1) != STATIC_LIST && REF2KIND (op2) != STATIC_LIST)
	    sxtrap (ME, "drcg_pd2");
#endif

	XxY_set (&list_hd, op1, op2, &op);

	term_stack [--term_stack_top] = KV2REF (STATIC_LIST, 2*op-1);
    }
	break;

    case MINUS_n :
	integer_op (MINUS, 2);
	break;

    case MOD_n :
	integer_op (MODULO, 2);
	break;

    case MULT_n :
	integer_op (MULTIPLY, 2);
	break;

    case PLUS_n :
	integer_op (PLUS, 2);
	break;

    case PREDICATE_n :
    {
	int x, param_nb;

	param_nb = VISITED->son->brother->degree;

	if (lhs_or_rhs == 0) {
	    lhs_param_nb = param_nb;
	    DRCGlhs_functor2param [xprod] = DRCGmain_terms_top;
	}
	else {
	    if (VISITED->son->brother->name == ARGUMENT_S_n || is_streq_spcl (xprod, son_nb+1)) {
		son_nb++;
		/* La longueur de la RHS d'une clause doit etre identique en RCG et en DRCG */
		rhsnt++;
		DRCGrhs2param_nb [++DRCGmaxitem] = param_nb;
		DRCGrhs_functor2param [DRCGmaxitem] = DRCGmain_terms_top;
	    }
	}

	if (param_nb > max_param_nb)
	    max_param_nb = param_nb;

	for (x = 1; x <= term_stack_top; x++) {
	    DRCGmain_terms [DRCGmain_terms_top++] = term_stack [x];
	}

	term_stack_top = 0;
    }
	break;

    case PREDICATE_S_n :
    {
	int x, gvar, bot;

	prod2var_nb [xprod] = var_nb = var_stack_top;

	if (var_nb > 0) {
	    for (x = 1; x <= var_nb;x++) {
		gvar = ste2gvar [var_stack [x]];
		XH_push (SXDRCGvar_names, gvar);
	    }

	    XH_set (&SXDRCGvar_names, &(SXDRCG2var_disp [xprod]));
	}
	else
	    SXDRCG2var_disp [xprod] = 0;


	if (var_nb > SXDRCGmax_var)
	    SXDRCGmax_var = var_nb;

	bot = clause2da_disp [xprod];

	for (x = clause2da_disp [xprod+1]-1; x >= bot; x -= 2) {
	    /* On change les ste en var */
	    clause2da [x] = ste2var [clause2da [x]];
	}

	while (var_nb > 0) {
	    ste = var_stack [var_nb];
	    ste2var [ste] = 0;
	    var_nb--;
	}

	DRCGprolon [xprod+1] = DRCGmaxitem+1;

	if (rhsnt > SXDRCGmaxrhsnt)
	    SXDRCGmaxrhsnt = rhsnt;

	if (is_prolog) {
	    PPPprolon [xprod+1] = PPPmaxitem+1;
	}
    }
	break;

    case PROLOG_S_n :
	is_prolog_tree = FALSE;
	break;

    case SUP_n :
	gen_ppp (sup_ste, 2, VISITED->son);
	break;

    case SUP_EQUAL_n :
	gen_ppp (sup_equal_ste, 2, VISITED->son);
	break;

    case TERM_n :
	if (VISITED->father->name == PROLOG_S_n) {
	    /* On verifie que l'on sait traiter (au moins grossierement) ce predicat prologue */
	    int		code, param_nb;
	    NODE	*son1;

	    son1 =  VISITED->son;
	    ste = son1->token.string_table_entry;

	    if ((code = ste2prolog_code [ste]) != 0) {
		/* Il est connu */
		param_nb = prolog_code2param_nb [code];

		if (param_nb == son1->brother->degree) {
		    gen_ppp (ste, param_nb, son1->brother->son);
		}
		else {
		    sxput_error (son1->token.source_index,
				 "%sWrong number of arguments for this prolog predicate.",
				 sxtab_ptr->err_titles [2]);
		}
	    }
	}
	else {
	    int 	bot, x, term, ref;

	    bot = term_stack_top - VISITED->son->brother->degree;
#if EBUG
	    if (bot <= 0)
		sxtrap (ME, "drcg_pd2");
#endif

	    for (x = bot; x <= term_stack_top; x++) {
		ref = term_stack [x];
		XH_push (term_hd, ref);
	    }

	    XH_set (&term_hd, &(term));
	    term_stack [bot] = KV2REF (TERM, term);
	    term_stack_top = bot;
	}
	break;

    case UNARY_MINUS_n :
	integer_op (UMINUS, 1);
	break;

    case VOID_n :
	break;

    case atom_n :
	if (VISITED->father->father->name != PROLOG_S_n) {
	    int	atom;

	    ste = VISITED->token.string_table_entry;

	    if ((atom = ste2atom [ste]) == 0) {
		atom = ste2atom [ste] = ++SXDRCGmax_atom;
		atom2ste [SXDRCGmax_atom] = ste;
	    }

	    term_stack [++term_stack_top] = KV2REF (ATOM, atom);
	}
	break;

    case external_predicate_n :
	break;

    case floating_point_number_n :
	break;

    case integer_number_n :
	{
	    int	val;

	    val = atoi (sxstrget (VISITED->token.string_table_entry));
	    term_stack [++term_stack_top] = KV2REF (INTEGER_CONSTANT, val);
	}
	break;

    case internal_predicate_n :
	break;

    case variable_n :
    {
	int var, gvar;

	ste = VISITED->token.string_table_entry;

	if (ste == _ste) {
	    gvar2ste [0] = _ste;
	    /* ste2gvar [_ste] == 0 */
	    /* ste2var [_ste] == 0 */
	    var_stack [++var_stack_top] = _ste;
	    var = 0;
	}
	else {
	    if ((gvar = ste2gvar [ste]) == 0) {
		ste2gvar [ste] = gvar = ++SXDRCGmax_gvar;
		gvar2ste [SXDRCGmax_gvar] = ste;
	    }

	    if ((var = ste2var [ste]) == 0) {
		if (SXBA_bit_is_set (prod2gvar_set [xprod], gvar))
		    /* La variable courante est utilisee ds un ppp de la production courante */
		    var = ++ppp_var_nb;
		else
		    var = ++var_stack_top;

		ste2var [ste] = var;
		var_stack [var] = ste;
	    }
	}

	if (is_prolog_tree) {
	    /* On est dans un sous arbre PROLOG_S_n */
	    /* On note le fait que la variable var intervient dans le ppp qui se trouve
	       a l'item DRCGmaxitem. */
	    SXBA_1_bit (var2pppitem_set [var], PPPmaxitem);
	}

	term_stack [++term_stack_top] = KV2REF (VARIABLE, var);
    }

	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end drcg_pd2 */
}

static void
smppass ()
{
    int			ml;

    if (sxatcvar.atc_lv.abstract_tree_is_error_node) return;

    if (sxverbosep) {
	fputs ("\tSemantic Pass .... ", sxtty);
	sxttycol1p = FALSE;
    }

    /*   I N I T I A L I S A T I O N S   */
    /* ........... */

    /* Traitement des variables comme terminaux */
    /* Dcg_constant_ste = sxstrsave ("Dcg_constant"); unused */

    /* On met aussi en string_table les operateurs binaires infixes de prolog,
       ils peuvent etre des foncteurs principayx (non-terminaux), mais pas de simples atomes */
    
    plus_ste = sxstrsave (ppp2str [PLUS]);
    uminus_ste = minus_ste = sxstrsave (ppp2str [MINUS]);
    mult_ste = sxstrsave (ppp2str [MULTIPLY]);
    div_ste = sxstrsave (ppp2str [DIVIDE]);
    mod_ste = sxstrsave (ppp2str [MODULO]);
    less_ste = sxstrsave (ppp2str [LESS]);
    less_equal_ste = sxstrsave (ppp2str [LESS_EQUAL]);
    sup_ste = sxstrsave (ppp2str [SUP]);
    sup_equal_ste = sxstrsave (ppp2str [SUP_EQUAL]);
    is_ste = sxstrsave (ppp2str [IS]);

    max_ste = SXSTRtop ();

    /* On suppose l'utilisation de tous les ppp! */
    rule_nb = sxatcvar.atc_lv.abstract_tree_root->degree;

    ml = 4;

    /* Ne doit pas deborder! */
    XH_alloc (&drcg_part[0], "drcg_rules[0]", rule_nb + 1, 1, 2*ml, NULL, NULL);
    XH_alloc (&drcg_part[1], "drcg_rules[1]", rule_nb + 1, 1, 2*ml, NULL, NULL);

    XxY_alloc (&drcg_rules, "drcg_rules", rule_nb + 1, 1, 0, 0, NULL, NULL);

    ste2prolog_code = (int*) sxcalloc (max_ste + 1, sizeof (int));
    prolog_init ();

    ste2gvar = (int*) sxcalloc (max_ste + 1, sizeof (int));
    gvar2ste = (int*) sxalloc (max_ste + 1, sizeof (int));
    /* Peut etre modifie par le ste de la variable bidon "_" */
    gvar2ste [0] = EMPTY_STE;

    prod2gvar_set = sxbm_calloc (rule_nb + 1, max_ste + 1);

    sxsmp (sxatcvar.atc_lv.abstract_tree_root, drcg_pi, drcg_pd);

    XH_free (&drcg_part[0]);
    XH_free (&drcg_part[1]);
    XxY_free (&drcg_rules);

#if 0
    if (is_prolog)
	/* On suppose l'utilisation de tous les ppp! */
	rule_nb += prolog_code_nb;

    if (is_dynam_constant) {
	rule_nb += BNFmaxt + 2;
	rhs_symb_nb += BNFmaxt + 2;
	/* Pour le param des productions traitant les affectations dynamiques des terminaux
	   aux variables. */
	DRCGmain_terms_size++;
    }
#endif

    DRCGlhs2param_nb = (int*) sxalloc (rule_nb+1, sizeof (int));
    DRCGlhs2param_nb [0] = 0;
    DRCGrhs2param_nb = (int*) sxalloc (rule_nb+rhs_symb_nb+1+4, sizeof (int));
    DRCGrhs2param_nb [0] = DRCGrhs2param_nb [1] = DRCGrhs2param_nb [2] = DRCGrhs2param_nb [3] = 0;

    DRCGlhs_functor2param = (int*) sxalloc (rule_nb+1, sizeof (int));
    DRCGrhs_functor2param = (int*) sxalloc (rule_nb+rhs_symb_nb+1+4, sizeof (int));
    DRCGmain_terms = (int*) sxalloc (DRCGmain_terms_size+1, sizeof (int));
    DRCGprolon  = (int*) sxalloc (rule_nb+2, sizeof (int));
    DRCGprolon [1] = 1;

    if (is_prolog) {
	PPPprolon  = (int*) sxalloc (rule_nb+2, sizeof (int));
	PPPlispro = (int*) sxalloc (ppp_symb_nb+1, sizeof (int));
	PPPmaxitem = 0;
	PPPprolon [0] = PPPprolon [xprod+1] = PPPmaxitem+1;
	PPPmain_terms = (int*) sxalloc (PPPmain_terms_size+1, sizeof (int));
    }

    ste2atom = (int*) sxcalloc (max_ste + 1, sizeof (int));
    atom2ste = (int*) sxalloc (max_ste + 1, sizeof (int));

    /* Il peut y avoir plusieurs "_" ds var_stack */
    var_stack = (int*) sxalloc (max_arg_per_prod + 1, sizeof (int));
    prod2var_nb = (int*) sxalloc (rule_nb+1, sizeof (int));

    /* La variable "_" joue un role particulier */
    _ste = sxstrretrieve ("_");

    SXDRCGterm_size = TERM_nb == 0 ? 0 : (TERM_lgth/TERM_nb)+1;

    XH_alloc (&term_hd, "term_hd", TERM_nb+1, 1, SXDRCGterm_size+1, NULL, NULL);
    term_stack = (int*) sxalloc (term_stack_size + 1 + 1 /* EMPTY_LIST */, sizeof (int));
	
    XxY_alloc (&list_hd, "list_hd", LIST_nb+1, 1, 0, 0, NULL, NULL);

    SXDRCG2var_disp = (int*) sxalloc (rule_nb+1, sizeof (int));

    gvar_nb += SXDRCGmax_gvarppp;
    SXDRCGmax_gvar = SXDRCGmax_gvarppp;

    XH_alloc (&SXDRCGvar_names, "SXDRCGvar_names", gvar_nb+1, 1, (SXDRCGmax_gvar/(rule_nb+1))+1, NULL, NULL);

    if (is_prolog) {
	var2pppitem_set = sxbm_calloc (SXDRCGmax_varppp+1, ppp_symb_nb+1);
    }

#if 0
    t2kind = (int*) sxalloc (BNFmaxt+1, sizeof (int));
#endif

    ste2var = (int*) sxcalloc (max_ste + 1, sizeof (int));

    sxsmp (sxatcvar.atc_lv.abstract_tree_root, drcg_pi2, drcg_pd2);

    sxbm_free (prod2gvar_set), prod2gvar_set = NULL;

#if 0
    if (is_dynam_constant) {
	DRCGprod2dynam_constant = (int*) sxcalloc (rule_nb+1, sizeof (int));
	process_dynam_constant ();
    }
#endif

    has_prolog = is_prolog;

    if (sxverbosep) {
	fputs ("done\n", sxtty);
	sxttycol1p = TRUE;
    }

    if (sxverbosep) {
	fprintf (sxtty, "\t%s_drcgt.c file output ... ", prgentname);
	sxttycol1p = FALSE;
    }

    drcg_write ();

    if (sxverbosep) {
	fputs ("done\n", sxtty);
	sxttycol1p = TRUE;
    }

    if (is_prolog) {
	sxfree (PPPprolon);
	sxfree (PPPlispro);
	sxfree (PPPmain_terms);
    }

    sxfree (DRCGlhs2param_nb);
    sxfree (DRCGrhs2param_nb);

    sxfree (DRCGlhs_functor2param);
    sxfree (DRCGrhs_functor2param);
    sxfree (DRCGmain_terms);
    sxfree (DRCGprolon);

    sxfree (ste2atom);
    sxfree (atom2ste);

    sxfree (ste2var);

    sxfree (var_stack);
    sxfree (prod2var_nb);
    sxfree (SXDRCG2var_disp);

    XH_free (&term_hd);
    sxfree (term_stack);
    XxY_free (&list_hd);

    XH_free (&SXDRCGvar_names);

#if 0
    if (is_dynam_constant) {
	sxfree (DRCGprod2dynam_constant);
    }
#endif

    if (is_prolog) {
	sxbm_free (var2pppitem_set);
    }

#if 0
    sxfree (t2kind);
#endif

    sxfree (ste2prolog_code);
    sxfree (ste2gvar);
    sxfree (gvar2ste);

    /*   F I N A L I S A T I O N S   */
    /* ........... */

}

VOID
drcg_smp (what, drcg_smp_drcg_tables)
    int			what;
    struct sxtables	*drcg_smp_drcg_tables;
{
    switch (what) {
    case OPEN:
	sxatcvar.atc_lv.node_size = sizeof (struct drcg_node);
	break;

    case ACTION:
	smppass ();
	break;

    case SEMPASS:		/* For [sub-]tree evaluation during parsing */
	break;

    case CLOSE:
	break;

    default:
	fputs ("The function \"drcg_smp\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}


