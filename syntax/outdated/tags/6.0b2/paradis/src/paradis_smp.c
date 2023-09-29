/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1984 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs. (pb)*
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 20030515 14:43 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 31-08-88 17:10 (bl):         Ajout de #ifdef MSDOS			*/
/************************************************************************/
/* 08-04-88 14:40 (pb):         Adaptation aux XNT			*/
/************************************************************************/
/* 18-12-87 14:03 (pb):         Les tables sont creees si warning(s)    */
/************************************************************************/
/* 03-12-87 12:10 (pb&phd):	Preparation a la suppression de tous	*/
/*				les noeuds des reductions simples	*/
/************************************************************************/
/* 04-09-87 15:12 (pb):         Bug sur "INH"                           */
/************************************************************************/
/* 25-05-87 12:40 (phd):	Ajout d'un message d'erreur	   	*/
/************************************************************************/
/* 02-03-87 09:02 (pb):		Suppression des '~' si verbose	  	*/
/************************************************************************/
/* 29-01-87 18:00 (pb):		Liberation conditionnelle!	  	*/
/************************************************************************/
/* 10-12-86 17:36 (phd):	Modification d'un message d'erreur	*/
/************************************************************************/
/* 26-11-86 17:09 (phd):	Ajout de cette rubrique "modifications"	*/
/*				Optimisations diverses...		*/
/************************************************************************/

/**************************************************************/
/*                                                            */
/*                                                            */
/*  This program has been translated from paradis_main.pl1    */
/*  on Tuesday the twenty-sixth of August, 1986, at 14:29:03, */
/*  on the Multics system at INRIA,                           */
/*  by the release 3.3i PL1_C translator of INRIA,            */
/*         developped by the "Langages et Traducteurs" team,  */
/*         using the SYNTAX (*), FNC and Paradis systems.     */
/*                                                            */
/*                                                            */
/**************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                        */
/**************************************************************/



/*     Historique des modifications
  
   Version 2.1 le 25 Mars 1983
   -option is_list.
  
   Version 2.2 le 20 Juin 1983
   - CALL (terminal_non_generic)  change en PRINT ("terminal_non_generic")
   - correction bug sur les productions simples.
  
   Version 3.0 le 17 Janvier 1985
   - Mise a niveau avec la nouvelle version de syntax.
  
   Version 3.1 le 15 Octobre 1985
   - Recompilation (modif des intro_tables).

   Le 5 Fevrier 1987
   - Message d'erreur mal place sur INH.

*/



/*   I N C L U D E S   */
#define NODE struct paradis_node
#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"
#include "put_edit.h"

#include "PP_tables.h"

#include "sem_by.h"
#include "varstr.h"
char WHAT_PARADISSMP[] = "@(#)SYNTAX - $Id: paradis_smp.c 609 2007-06-04 14:02:26Z rlacroix $" WHAT_DEBUG;


extern SXVOID    bnf_get_rule_tail (unsigned long int rule_no, unsigned long int *tline, unsigned int *tcol);

struct paradis_node {
    NODE_HEADER_S	VOID_NAME;
};

/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ACTION_n 2
#define BACKWARD_n 3
#define BLANKS_n 4
#define COLUMN_n 5
#define FORM_FEED_n 6
#define FORWARD_n 7
#define GENERIC_TERMINAL_n 8
#define INHIBITION_n 9
#define LHS_n 10
#define MARGIN_n 11
#define NEW_LINE_n 12
#define NON_TERMINAL_n 13
#define PP_SPEC_n 14
#define PREDICATE_n 15
#define RULE_S_n 16
#define SPACE_n 17
#define TABULATION_n 18
#define TERMINAL_n 19
#define VOCABULARY_S_n 20
#define VOID_n 21
/*
E N D   N O D E   N A M E S
*/


#define	NOP	0
#define	SKIP	1
#define	PAGE	2
#define	SPACE	3
#define	TAB	4
#define	MARGIN	5
#define	CALL	6
#define	COL	7
#define	RETURN	8
#define	INH	9
#define	PRINT	10

static char	*codop_name [11] = {"", "SKIP", "PAGE", "SPACE", "TAB", "MARGIN", "CALL", "COL", "RETURN", "INH", "PRINT"
     };

#define	NL '\n'
#define	FF '\f'
#define	HT '\t'
#define	VT '\013'
#define	SP ' '
#define	BS '\b'

/* E X T E R N   */

struct sxtables		paradis_tables;
extern VARSTR	cat_t_string ();
extern BOOLEAN	is_check;

/*   S T A T I C   */

static int	line_lgth, xline;
static char	*line;
static BOOLEAN	SUCCESS;
static int	x_prod, column, hash_value, x_check, x_schema, PP_schema_size;
static int	x_lispro, margin, prev_column, EOL_nb;
static BOOLEAN	is_a_spec, is_inh;
static int	current_char;
static int	xtok, mtok;
static struct PP_ag_item	PP_ag;
static int	hash [32];
static struct sxsource_coord inh_source_index;
static struct check {
	   int	link, lgth, indx;
       }	*check /* 1:nbpro */ ;
static struct coords {
	   short	line, column;
       }	*coords /* 1:mtok */ ;




/* P R O C E D U R E S    */

static SHORT	skip_sem ()
{
    register SHORT	c = sxsrcmngr.current_char;

    while (c != EOF) {
	if (c == '<' && sxsrcmngr.source_coord.column == 1)
	    return '<';

	c = sxnext_char ();
    }

    return EOF;
}



#if 0
static SXVOID	put_c_in_line (c)
    char	c;
{
    line [xline] = c;

    if (++xline > line_lgth) {
	line = (char*) sxrealloc (line, (line_lgth <<= 1) + 1, sizeof (char));
    }
}
#else
# define put_c_in_line(c)	\
do {										\
    line [xline] = c;								\
										\
    if (++xline > line_lgth) {							\
	line = (char*) sxrealloc (line, (line_lgth <<= 1) + 1, sizeof (char));	\
    }										\
} while(0)
#endif



static VOID	gen_code (codop, move)
    int		codop, move;
{

    if (is_inh) {
	is_inh = FALSE /* un seul message */;
	sxput_error (inh_source_index, "%sINH can only be the last specification of a rule.",
		 paradis_tables.err_titles [2]);
	SUCCESS = FALSE;
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



static VOID	optimize ()
{
    register int	l, y, y1;
    register int	x, z, size;

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




static VOID	smppass (visited)
    register struct paradis_node	*visited;
{
    register struct paradis_node	*brother, *son;

    sxinitialise(son); /* pour faire taire gcc -Wuninitialized */
    switch (visited->name) {
    case TERMINAL_n:
	is_a_spec = FALSE;

	{
	    register int	t = bnf_ag.WS_INDPRO [x_lispro++].lispro;

	    gen_code (CALL, -XT_TO_T (t));
	}

	break;

    case NON_TERMINAL_n:
    case GENERIC_TERMINAL_n:
	gen_code (CALL, 0);

    case ACTION_n:
	x_lispro++;

    case PREDICATE_n:
	is_a_spec = FALSE;
	break;

    case LHS_n:
	margin = (prev_column = coords [++xtok].column) + 1;
	is_a_spec = FALSE;
	return;

    case RULE_S_n:
	for (son = visited->son; son != NULL; son = son->brother) {
	    smppass (son);

	    if (PP_ag.PP_indx [x_prod] > x_schema /* vide */ || PP_ag.SXPP_schema [x_schema].PP_codop != RETURN)
		gen_code (RETURN, 0);


/* if !(CALL 0; RETURN 0) => bprosimpl=FALSE */

	    if (bnf_ag.WS_NBPRO [x_prod].bprosimpl) {
		if (x_schema - PP_ag.PP_indx [x_prod] != 1 || PP_ag.SXPP_schema [x_schema].PP_act != 0 /* INH */) {
		    bnf_ag.WS_NBPRO [x_prod].bprosimpl = FALSE;
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
	is_inh = FALSE;

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
	is_a_spec = TRUE;

	{
	    register int	codop, move_val;

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

		    sxput_error (son->token.source_index,
			 "%sA move specification must be a non zero integer less than 256.", paradis_tables.err_titles [2
			 ]);
		    SUCCESS = FALSE;
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
		    is_inh = TRUE;
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



BOOLEAN		paradis_sem ()
{
    static struct {
	       struct sxsvar	sxsvar;
	       struct sxplocals		sxplocals;
	       struct sxatcvar	sxatcvar;
	   }	bnf;

    if (sxverbosep)
	fputs ("   Paradis\n", sxtty);


/* I N I T I A L I Z A T I O N S    */

    SUCCESS = TRUE;
    x_schema = x_check = 0;

    {
	register int	i;

	for (i = 31; i >= 0; i--)
	    hash [i] = 0;
    }


/* les bnf tables sont globales, "write" et "free" sont faits dans "main" */

/* A L L O C A T I O N S   */

    PP_ag.PP_constants.PP_schema_lgth = 0;
    PP_ag.PP_constants.PP_indx_lgth = bnf_ag.WS_TBL_SIZE.nbpro;
    check = (struct check*) sxalloc (bnf_ag.WS_TBL_SIZE.nbpro + 1, sizeof (struct check));
    PP_ag.PP_indx = (int*) sxalloc (PP_ag.PP_constants.PP_indx_lgth + 1, sizeof (int));
    PP_schema_size = 2 * bnf_ag.WS_TBL_SIZE.indpro;
    /* pourquoi pas? */
    PP_ag.SXPP_schema = (struct SXPP_schema*) sxalloc (PP_schema_size + 1, sizeof (struct SXPP_schema));

/* On memorise les variables des processeurs utilises par bnf */

    bnf.sxsvar = sxsvar;
    bnf.sxplocals = sxplocals;
    bnf.sxatcvar = sxatcvar;

/* Reinitialisation du source   */

    rewind (sxsrcmngr.infile);
    sxsrc_mngr (INIT, sxsrcmngr.infile, sxsrcmngr.source_coord.file_name);

/* On reutilise la string table de bnf */

    syntax (OPEN, &paradis_tables) /* Initialisation de SYNTAX */ ;
    syntax (ACTION, &paradis_tables);

/*   F I N A L I Z A T I O N   */

/* The current tables are closed. */

    syntax (CLOSE, &paradis_tables);

/* On restaure les variables des processeurs utilises par bnf */

    sxsvar = bnf.sxsvar;
    sxplocals = bnf.sxplocals;
    sxatcvar = bnf.sxatcvar;
    sxsrc_mngr (FINAL);
    PP_ag.bnf_modif_time = bnf_ag.bnf_modif_time;
    PP_ag.PP_constants.PP_schema_lgth = x_schema;

    if (sxerrmngr.nbmess [2] == 0 /* no error */ ) {
	if (!is_check) {
	    /* On cree les tables permanentes */
	    if (!paradis_write (&PP_ag, prgentname))
		SUCCESS = FALSE;
	    else
		bnf_ag.WS_TBL_SIZE.sem_kind = sem_by_paradis;
	}
    }
    else {
	SUCCESS = FALSE;
	sxtmsg (sxsrcmngr.source_coord.file_name, "%sTables are not generated.", paradis_tables.err_titles [2]);
    }

    sxfree (check);
    return SUCCESS;
}




/* L I S T I N G _ O U T P U T     */
extern unsigned long bnf_get_line_no (int);

VOID	paradis_lo ()
{
    if (is_list && SUCCESS) {
	unsigned long	debut, tline;
	unsigned int	tcol;
	VARSTR	vstr;


#define get_nt_string(nt)  bnf_ag.NT_STRING + bnf_ag.ADR [nt]

	vstr = varstr_alloc (128);
	line_lgth = 128;
	line = (char*) sxalloc (line_lgth + 1, sizeof (char));

/* Reinitialisation du source   */

	rewind (sxsrcmngr.infile);
	sxsrc_mngr (INIT, sxsrcmngr.infile, sxsrcmngr.source_coord.file_name);
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

	    put_c_in_line (NUL);
	    put_edit_nl (1);
	    put_edit_fnb (1, 4, x_prod);
	    put_edit (9, line);


/* Ecriture du genere pour la regle courante */

	    {
		register int	codop, move_val;
		register int	i;
		register int	paradis_lo_x_lispro = bnf_ag.WS_NBPRO [x_prod].prolon;
		BOOLEAN		return_not_found = TRUE;

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

			return_not_found = FALSE;
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



VOID	paradis_smp (what, paradis_smp_paradis_tables)
    int		what;
    struct sxtables	*paradis_smp_paradis_tables;
{
    switch (what) {
    case OPEN:
	sxatcvar.atc_lv.node_size = sizeof (struct node_header_s);
	break;

    case ACTION:
	/*   C O D E   G E N E R A T I O N    */

	xtok = 0;

	if (!sxatcvar.atc_lv.abstract_tree_is_error_node)
	    smppass (sxatcvar.atc_lv.abstract_tree_root);

	break;

    case CLOSE:
	break;

    default:
	fputs ("The function \"paradis_smp\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}


SXVOID
paradis_scan_act (code, act_no)
    int		code;
    int		act_no;
{
    switch (code) {
    case OPEN:
	coords = (struct coords*) sxalloc ((mtok = 3 * bnf_ag.WS_TBL_SIZE.indpro) + 1, sizeof (struct coords));
	return;

    case CLOSE:
	if (coords != NULL) {
	    sxfree (coords);
	}

	return;

    case INIT:
	xtok = 0;

	if (skip_sem () == EOF) {
	    fprintf (sxstderr, "\n\tThis text is not a grammar... see you later.\n");
	    SXEXIT (3);
	}

	return;

    case FINAL:
	return;

    case ACTION:
	switch (act_no) {
	case 1:
	    skip_sem ();
	    return;

	case 2:
	    /* post-action : on memorise les coordonnees du debut du token suivant */
	    if (++xtok > mtok) {
		coords = (struct coords*) sxrealloc (coords, (mtok += bnf_ag.WS_TBL_SIZE.indpro) + 1, sizeof (struct coords
		     ));
	    }

	    coords [xtok].line = sxsvar.sxlv_s.counters [1];
	    sxsvar.sxlv_s.counters [1] = 0;
	    coords [xtok].column = sxsvar.sxlv_s.counters [2];
	    return;

	case 3:
	    /* HT */
	    {

#define tabs 8

		register long	*a;

		a = &(sxsvar.sxlv_s.counters [2]);
		*a += tabs - (*a - 1) % tabs;
	    }

	    return;
	default:
#if EBUG
	  sxtrap("paradis_smp","unknown switch case #2");
#endif
	  break;
	}

    default:
	fputs ("\nThe function \"paradis_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
