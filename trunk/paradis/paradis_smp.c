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

#define SXNODE struct paradis_node
#include "sxversion.h"
#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"
#include "put_edit.h"
#include "PP_tables.h"
#include "sem_by.h"
#include "varstr.h"

char WHAT_PARADISSMP[] = "@(#)SYNTAX - $Id: paradis_smp.c 4124 2024-07-29 10:58:45Z garavel $" WHAT_DEBUG;


extern void    bnf_get_rule_tail (SXUINT rule_no, SXUINT *tline, SXUINT *tcol);

struct paradis_node {
    SXNODE_HEADER_S	SXVOID_NAME;
};

/*  N O D E   N A M E S  */
#define ERROR_n ((SXINT)1)
#define ACTION_n ((SXINT)2)
#define BACKWARD_n ((SXINT)3)
#define BLANKS_n ((SXINT)4)
#define COLUMN_n ((SXINT)5)
#define FORM_FEED_n ((SXINT)6)
#define FORWARD_n ((SXINT)7)
#define GENERIC_TERMINAL_n ((SXINT)8)
#define INHIBITION_n ((SXINT)9)
#define LHS_n ((SXINT)10)
#define MARGIN_n ((SXINT)11)
#define NEW_LINE_n ((SXINT)12)
#define NON_TERMINAL_n ((SXINT)13)
#define PP_SPEC_n ((SXINT)14)
#define PREDICATE_n ((SXINT)15)
#define RULE_S_n ((SXINT)16)
#define SPACE_n ((SXINT)17)
#define TABULATION_n ((SXINT)18)
#define TERMINAL_n ((SXINT)19)
#define VOCABULARY_S_n ((SXINT)20)
#define VOID_n ((SXINT)21)
/*  E N D   N O D E   N A M E S  */


#define	NOP	((SXINT)0)
#define	SKIP	((SXINT)1)
#define	PAGE	((SXINT)2)
#define	SPACE	((SXINT)3)
#define	TAB	((SXINT)4)
#define	MARGIN	((SXINT)5)
#define	CALL	((SXINT)6)
#define	COL	((SXINT)7)
#define	RETURN	((SXINT)8)
#define	INH	((SXINT)9)
#define	PRINT	((SXINT)10)

static char	*codop_name [11] = {"", "SKIP", "PAGE", "SPACE", "TAB", "MARGIN", "CALL", "COL", "RETURN", "INH", "PRINT"
     };

#define	NL '\n'
#define	FF '\f'
#define	HT '\t'
#define	VT '\013'
#define	SP ' '
#define	BS '\b'

/* E X T E R N   */

extern struct sxtables		paradis_tables;
extern VARSTR	cat_t_string (VARSTR, SXINT);
extern bool	is_check;

/*   S T A T I C   */

static SXINT	line_lgth, xline;
static char	*line;
static bool	SUCCESS;
static SXINT	x_prod, column, hash_value, x_check, x_schema, PP_schema_size;
static SXINT	x_lispro, margin, prev_column, EOL_nb;
static bool	is_a_spec, is_inh;
static char	current_char;
static SXINT	xtok, mtok;
static struct PP_ag_item	PP_ag;
static SXINT	hash [32];
static struct sxsource_coord inh_source_index;
static struct check {
	   SXINT	link, lgth, indx;
       }	*check /* 1:nbpro */ ;
static struct coords {
	   short	line, column;
       }	*coords /* 1:mtok */ ;




/* P R O C E D U R E S    */

static short	skip_sem (void)
{
    short	c = sxsrcmngr.current_char;

    while (c != EOF) {
	if (c == '<' && sxsrcmngr.source_coord.column == 1)
	    return '<';

	c = sxnext_char ();
    }

    return EOF;
}



#define put_c_in_line(c)	\
do {										\
    line [xline] = c;								\
										\
    if (++xline > line_lgth) {							\
	line = (char*) sxrealloc (line, (line_lgth <<= 1) + 1, sizeof (char));	\
    }										\
} while(0)



static void	gen_code (SXINT codop, SXINT move)
{

    if (is_inh) {
	is_inh = false /* un seul message */;
	sxerror (inh_source_index,
		 paradis_tables.err_titles [2][0],
		 "%sINH can only be the last specification of a rule.",
		 paradis_tables.err_titles [2]+1);
	SUCCESS = false;
	}

    x_schema++;

    if (x_schema > PP_schema_size) {
	PP_schema_size *= 2;
	PP_ag.SXPP_schema = (struct SXPP_schema*) sxrealloc (PP_ag.SXPP_schema, PP_schema_size + 1, sizeof (struct
	     SXPP_schema));
    }

    PP_ag.SXPP_schema [x_schema].PP_codop = codop;
    PP_ag.SXPP_schema [x_schema].PP_act = move;
    hash_value += codop;
}



static void	optimize (void)
{
    SXINT	l, y, y1;
    SXINT	x, z, size;

    size = x_schema - (y = PP_ag.PP_indx [x_prod]) + 1;

    for (z = 0, x = hash [hash_value %= 32]; x != 0; x = check [z = x].link) {
	if (check [x].lgth != size)
	    continue;

	y1 = check [x].indx;
	l = size - 1;

	while (PP_ag.SXPP_schema [y1 + l].PP_codop == PP_ag.SXPP_schema [y + l].PP_codop && PP_ag.SXPP_schema [y1 + l].
	     PP_act == PP_ag.SXPP_schema [y + l].PP_act) {
	    if (l-- == 0) {
		x_schema = PP_ag.PP_indx [x_prod] - 1;
		PP_ag.PP_indx [x_prod] = y1;
		return;
	    }
	}
    }

    *((z == 0) ? &(hash [hash_value]) : &(check [z].link)) = ++x_check;
    check [x_check].link = 0;
    check [x_check].lgth = size;
    check [x_check].indx = PP_ag.PP_indx [x_prod];
}



static void	smppass (struct paradis_node *visited)
{
    struct paradis_node	*brother, *son;

    sxinitialise(son); /* pour faire taire gcc -Wuninitialized */
    switch (visited->name) {
    case TERMINAL_n:
	is_a_spec = false;

	{
	    SXINT	t = bnf_ag.WS_INDPRO [x_lispro++].lispro;

	    gen_code (CALL, -XT_TO_T (t));
	}

	break;

    case NON_TERMINAL_n:
    case GENERIC_TERMINAL_n:
	gen_code (CALL, (SXINT) 0);
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	/* absence de "break" (intentionnelle ?) */
	__attribute__ ((fallthrough));
#endif

    case ACTION_n:
	x_lispro++;
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	/* absence de "break" (intentionnelle ?) */
	__attribute__ ((fallthrough));
#endif

    case PREDICATE_n:
	is_a_spec = false;
	break;

    case LHS_n:
	margin = (prev_column = coords [++xtok].column) + 1;
	is_a_spec = false;
	return;

    case RULE_S_n:
	for (son = visited->son; son != NULL; son = son->brother) {
	    smppass (son);

	    if (PP_ag.PP_indx [x_prod] > x_schema /* vide */ || PP_ag.SXPP_schema [x_schema].PP_codop != RETURN)
		gen_code (RETURN, (SXINT)0);


/* if !(CALL 0; RETURN 0) => bprosimpl=false */

	    if (bnf_ag.WS_NBPRO [x_prod].bprosimpl) {
		if (x_schema - PP_ag.PP_indx [x_prod] != 1 || PP_ag.SXPP_schema [x_schema].PP_act != 0 /* INH */) {
		    bnf_ag.WS_NBPRO [x_prod].bprosimpl = false;
		    bnf_ag.WS_NBPRO [x_prod].action = 1;
		}
	    }
	    else
		bnf_ag.WS_NBPRO [x_prod].action = 1;

	    optimize ();
	}

	return;

    case VOCABULARY_S_n:
	x_lispro = bnf_ag.WS_NBPRO [x_prod = visited->position].prolon;
	hash_value = 0;
	PP_ag.PP_indx [x_prod] = x_schema + 1;
	is_inh = false;

	for (son = visited->son; son != NULL; son = son->brother)
	    smppass (son);

	return;

    case BLANKS_n:
	column = coords [++xtok].column;

	if ((EOL_nb = coords [xtok].line) != 0) {
	    gen_code (SKIP, EOL_nb);
	}

	brother = visited->brother;
	
	if (brother == NULL || (brother != NULL && brother->name != PP_SPEC_n)) {
	    if (EOL_nb != 0) {
		gen_code (MARGIN, column - margin);
	    }
	    else {
		if (!is_a_spec && column - prev_column - 1 > 0) {
		    gen_code (SPACE, column - prev_column - 1);
		}
	    }
	}

	return;

    case PP_SPEC_n:
	is_a_spec = true;

	{
	    SXINT	codop, move_val;

	    sxinitialise(move_val); /* pour faire taire gcc -Wuninitialized */
	    visited = visited->son;

	    switch (visited->name) {
	    case ERROR_n:
	    case BACKWARD_n:
	    case FORWARD_n:
	    case VOID_n:
		break;

	    case INHIBITION_n:
		move_val = 1;
		break;

	    default:
		if ((son = visited->son)->name == VOID_n)
		    /* default value */
		    move_val = (visited->name == MARGIN_n) ? 0 : 1;
		else {
		    if (sxstrlen (son->token.string_table_entry) < 4) {
			move_val = atoi (sxstrget (son->token.string_table_entry));

			if (move_val > 0 && move_val < 256)
			    break;
		    }

		    sxerror (son->token.source_index,
			     paradis_tables.err_titles [2][0],
			     "%sA move specification must be a non zero integer less than 256.",
			     paradis_tables.err_titles [2]+1);
		    SUCCESS = false;
		    move_val = 1;
		}
	    }

	    switch (visited->name) {
	    case COLUMN_n:
		codop = COL;
		break;

	    case FORM_FEED_n:
		codop = PAGE;
		break;

	    case INHIBITION_n:
		codop = RETURN;
		/* move_val=1 */
		break;

	    case MARGIN_n:
		if (son->name == BACKWARD_n)
		    move_val = -move_val;

		codop = MARGIN;
		break;

	    case NEW_LINE_n:
		codop = SKIP;
		break;

	    case SPACE_n:
		if (son->name == BACKWARD_n)
		    move_val = -move_val;

		codop = SPACE;
		break;

	    case TABULATION_n:
		codop = TAB;
		break;

	    default:
		codop = NOP;
		break;
	    }

	    if (codop != NOP) {
		gen_code (codop, move_val);
	
		if (visited->name == INHIBITION_n) {
		    is_inh = true;
		    inh_source_index = visited->token.source_index;
		}
	    }
	
	}

	break;


/*
Z Z Z Z
*/

    default:
	return;
    }

    prev_column = coords [++xtok].column;
}



bool		paradis_sem (void)
{
    static struct {
	       struct sxsvar	sxsvar;
	       struct sxplocals		sxplocals;
	       struct sxatcvar	sxatcvar;
	   }	bnf;

    if (sxverbosep)
	fputs ("   Paradis\n", sxtty);


/* I N I T I A L I Z A T I O N S    */

    SUCCESS = true;
    x_schema = x_check = 0;

    {
	SXINT	i;

	for (i = 31; i >= 0; i--)
	    hash [i] = 0;
    }


/* les bnf tables sont globales, "write" et "free" sont faits dans "main" */

/* A L L O C A T I O N S   */

    PP_ag.PP_constants.PP_schema_lgth = 0;
    PP_ag.PP_constants.PP_indx_lgth = bnf_ag.WS_TBL_SIZE.nbpro;
    check = (struct check*) sxalloc (bnf_ag.WS_TBL_SIZE.nbpro + 1, sizeof (struct check));
    PP_ag.PP_indx = (SXINT*) sxcalloc (PP_ag.PP_constants.PP_indx_lgth + 1, sizeof (SXINT));
    PP_schema_size = 2 * bnf_ag.WS_TBL_SIZE.indpro;
    /* pourquoi pas? */
    PP_ag.SXPP_schema = (struct SXPP_schema*) sxcalloc (PP_schema_size + 1, sizeof (struct SXPP_schema));

/* On memorise les variables des processeurs utilises par bnf */

    bnf.sxsvar = sxsvar;
    bnf.sxplocals = sxplocals;
    bnf.sxatcvar = sxatcvar;

/* Reinitialisation du source   */

    rewind (sxsrcmngr.infile);
    sxsrc_mngr (SXINIT, sxsrcmngr.infile, sxsrcmngr.source_coord.file_name);

/* On reutilise la string table de bnf */

    syntax (SXOPEN, &paradis_tables) /* Initialisation de SYNTAX */ ;
    syntax (SXACTION, &paradis_tables);

/*   F I N A L I Z A T I O N   */

/* The current tables are closed. */

    syntax (SXCLOSE, &paradis_tables);

/* On restaure les variables des processeurs utilises par bnf */

    sxsvar = bnf.sxsvar;
    sxplocals = bnf.sxplocals;
    sxatcvar = bnf.sxatcvar;
    sxsrc_mngr (SXFINAL);
    PP_ag.bnf_modif_time = bnf_ag.bnf_modif_time;
    PP_ag.PP_constants.PP_schema_lgth = x_schema;

    if (sxerrmngr.nbmess [2] == 0 /* no error */ ) {
	if (!is_check) {
	    /* On cree les tables permanentes */
	    if (!paradis_write (&PP_ag, prgentname))
		SUCCESS = false;
	    else
		bnf_ag.WS_TBL_SIZE.sem_kind = sem_by_paradis;
	}
    }
    else {
	SUCCESS = false;
	sxtmsg (sxsrcmngr.source_coord.file_name, "%sTables are not generated.", paradis_tables.err_titles [2]+1);
    }

    sxfree (check);
    return SUCCESS;
}




/* L I S T I N G _ O U T P U T     */
extern SXUINT bnf_get_line_no (SXINT);

void	paradis_lo (void)
{
    if (is_list && SUCCESS) {
	SXUINT	debut, tline;
	SXUINT	tcol;
	VARSTR	vstr;


#define get_nt_string(nt)  bnf_ag.NT_STRING + bnf_ag.ADR [nt]

	vstr = varstr_alloc (128);
	line_lgth = 128;
	line = (char*) sxalloc (line_lgth + 1, sizeof (char));

/* Reinitialisation du source   */

	rewind (sxsrcmngr.infile);
	sxsrc_mngr (SXINIT, sxsrcmngr.infile, sxsrcmngr.source_coord.file_name);
	put_edit_ap ("\f");
	put_edit (21, "***** PRETTY_PRINTER SPECIFICATION : GENERATED CODE *****");
	put_edit_nl (2);

	for (x_prod = 1; x_prod <= bnf_ag.WS_TBL_SIZE.nbpro; x_prod++) {
	    debut = bnf_get_line_no (x_prod);
	    bnf_get_rule_tail (x_prod, &tline, &tcol);
	    xline = 0;

	    while ((current_char = sxnext_char (), sxsrcmngr.source_coord.line) < debut)
		;

	    while (sxsrcmngr.source_coord.line < tline) {
		put_c_in_line (current_char);

		if (current_char == '\n') {
		    put_c_in_line ('\t');
		}
		
		current_char = sxnext_char ();
	    }

	    while (sxsrcmngr.source_coord.column < tcol) {
		put_c_in_line (current_char);
		current_char = sxnext_char ();
	    }

	    put_c_in_line (SXNUL);
	    put_edit_nl (1);
	    put_edit_fnb (1, 4, x_prod);
	    put_edit (9, line);


/* Ecriture du genere pour la regle courante */

	    {
		SXINT	codop, move_val;
		SXINT	i;
		SXINT	paradis_lo_x_lispro = bnf_ag.WS_NBPRO [x_prod].prolon;
		bool		return_not_found = true;

		for (i = PP_ag.PP_indx [x_prod]; return_not_found; i++) {
		    move_val = PP_ag.SXPP_schema [i].PP_act;

		    switch (codop = PP_ag.SXPP_schema [i].PP_codop) {
		    case CALL:
			if (move_val == 0) {
			    while ((move_val = bnf_ag.WS_INDPRO [paradis_lo_x_lispro].lispro) > 0 && IS_A_PARSACT (move_val)) {
				paradis_lo_x_lispro++;
			    }

			    move_val = (move_val < 0) ? XT_TO_T (move_val) : XNT_TO_NT (move_val);
			}
			else {
			    move_val = -move_val;
			    codop = PRINT;
			}

			paradis_lo_x_lispro++;
			break;

		    case RETURN:
			if (move_val == 1) {
			    codop = INH;
			    move_val = 0;
			}

			return_not_found = false;
			break;
		    default:
#if EBUG
		      sxtrap("paradis_smp","unknown switch case #1");
#endif
		      break;
		    }

		    put_edit_fnb (15, 4, i);
		    put_edit_nnl (25, codop_name [codop]);

		    if (move_val != 0) {
			switch (codop) {
			case MARGIN:
			    put_edit_apnnl ((move_val >= 0) ? "+" : "");
			    put_edit_apnb (move_val);
			    put_edit_nl (1);
			    break;

			case PRINT:
			    put_edit_apnnl ("(");
			    put_edit_apnnl (varstr_tostr (cat_t_string (varstr_raz (vstr), move_val)));
			    put_edit_ap (")");
			    break;

			case CALL:
			    put_edit_apnnl ("(");

			    if (move_val >= 0)
				put_edit_apnnl (get_nt_string (move_val));
			    else
				put_edit_apnnl (varstr_tostr (cat_t_string (varstr_raz (vstr), move_val)));

			    put_edit_ap (")");
			    break;

			default:
			    put_edit_apnnl ("(");
			    put_edit_apnb (move_val);
			    put_edit_ap (")");
			}
		    }
		}
	    }

	    put_edit_nl (1);

	}

	sxfree (line);
	varstr_free (vstr);
    }

    paradis_free (&PP_ag);
}



void	paradis_smp (SXINT what, struct sxtables *paradis_smp_paradis_tables)
{
    sxuse(paradis_smp_paradis_tables); /* pour faire taire gcc -Wunused */
    switch (what) {
    case SXOPEN:
	sxatcvar.atc_lv.node_size = sizeof (struct sxnode_header_s);
	break;

    case SXACTION:
	/*   C O D E   G E N E R A T I O N    */

	xtok = 0;

	if (!sxatcvar.atc_lv.abstract_tree_is_error_node)
	    smppass (sxatcvar.atc_lv.abstract_tree_root);

	break;

    case SXCLOSE:
	break;

    default:
	fputs ("The function \"paradis_smp\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}


bool paradis_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    case SXOPEN:
	coords = (struct coords*) sxalloc ((mtok = 3 * bnf_ag.WS_TBL_SIZE.indpro) + 1, sizeof (struct coords));
	return SXANY_BOOL;

    case SXCLOSE:
	if (coords != NULL) {
	    sxfree (coords);
	    coords = NULL;
	}

	return SXANY_BOOL;

    case SXINIT:
	xtok = 0;

	if (skip_sem () == EOF) {
	    fprintf (sxstderr, "\n\tThis text is not a grammar... see you later.\n");
	    sxexit (3);
	}

	return SXANY_BOOL;

    case SXFINAL:
	return SXANY_BOOL;

    case SXACTION:
	switch (act_no) {
	case 1:
	    skip_sem ();
	    return SXANY_BOOL;

	case 2:
	    /* post-action : on memorise les coordonnees du debut du token suivant */
	    if (++xtok > mtok) {
		coords = (struct coords*) sxrealloc (coords, (mtok += bnf_ag.WS_TBL_SIZE.indpro) + 1, sizeof (struct coords
		     ));
	    }

	    coords [xtok].line = sxsvar.sxlv_s.counters [1];
	    sxsvar.sxlv_s.counters [1] = 0;
	    coords [xtok].column = sxsvar.sxlv_s.counters [2];
	    return SXANY_BOOL;

	case 3:
	    /* HT */
	    {

#define tabs 8

		SXINT	*a;

		a = &(sxsvar.sxlv_s.counters [2]);
		*a += tabs - (*a - 1) % tabs;
	    }

	    return SXANY_BOOL;
	default:
#if EBUG
	  sxtrap("paradis_smp","unknown switch case #2");
#endif
	  break;
	}
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	__attribute__ ((fallthrough));
#endif

    default:
	fputs ("\nThe function \"paradis_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
