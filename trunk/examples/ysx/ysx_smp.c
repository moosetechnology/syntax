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

/* Convertisseur de sources YACC en sources YAX */

static char	ME [] = "ysx_smp";

#include <stdarg.h>

#define SXNODE struct ysx_node

#include "sxunix.h"
#include "put_edit.h"
#include "varstr.h"
#include "ysx_vars.h"

char WHAT_YSXSMP[] = "@(#)SYNTAX - $Id: ysx_smp.c 4249 2024-09-05 16:08:02Z garavel $" WHAT_DEBUG;

struct ysx_node {
    SXNODE_HEADER_S	SXVOID_NAME;
};

extern SXTABLES	yaction_tables;

/* forward */ SXSCANACT_FUNCTION yaction_scanact;

/* STATIC VARIABLES */

static SXTABLES	*ysxtables;
static FILE	*yax_file, *prio_file, *tdef_file;
static bool	needs_prio, needs_tdef;

#define NON_TERMINAL	0
#define TERMINAL	1
#define GENERIC		2
#define USER_ACTION	3

static char	*token_type /* Symbols Table: string_table_entry=>{NON_TERMINAL, TERMINAL, GENERIC, USER_ACTION} */ ;
static SXINT	*attribute_type /* Symbols Table: string_table_entry=>string_table_entry of the attribute */ ;
static SXINT	current_type /* string_table_entry of the current type */ ;
static SXINT	start /* Axiom, then LHS */ ;
static VARSTR	rule /* Text of current rule to be output */ ;
static SXINT	action_no /* Empty action number for dummy non-terminal */ ;
static SXNODE	*rule_post_action /* When not NULL, pointer to the semantics */ ;
static SXINT	action_offset /* Distance from the LHS */ ;




/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ACTION_n 2
#define C_ID_n 3
#define DEFS_n 4
#define FOLLOWING_RULE_n 5
#define ID_n 6
#define ID_LIST_n 7
#define ID_NUM_n 8
#define LCURL_n 9
#define LEADING_RULE_n 10
#define LEFT_PRIO_n 11
#define LIT_n 12
#define LIT_NUM_n 13
#define MARK_n 14
#define NONASSOC_PRIO_n 15
#define POST_ACTION_n 16
#define RIGHT_HAND_SIDE_n 17
#define RIGHT_PRIO_n 18
#define RULES_n 19
#define START_n 20
#define TOKENS_n 21
#define TOKEN_LIST_n 22
#define TYPE_n 23
#define TYPED_TOKENS_n 24
#define UNION_n 25
#define VOID_n 26
#define YACC_SPEC_n 27
/*
E N D   N O D E   N A M E S
*/

static void	gripe (char *who)
#ifdef __GNUC__
     __attribute__ ((noreturn))
#endif
     ;

static void	gripe (char *who)
{
    fprintf (sxstderr, "The function \"%s\" does not correspond to its specification.\n", who);
    sxexit(1);
}



SXINLINE static void	found_id (SXNODE *node)
{
    SXINT	ste;

    if (token_type [ste = node->token.string_table_entry] != NON_TERMINAL) {
	token_type [ste] = GENERIC;
    }
    else {
	attribute_type [ste] = current_type;
    }
}



SXINLINE static void	found_token (SXNODE *node)
{
    SXINT	ste;

    if (token_type [ste = node->token.string_table_entry] == NON_TERMINAL) {
	token_type [ste] = TERMINAL;
    }
}



SXINLINE static void	found_typed_token (SXNODE *node)
{
    token_type [node->token.string_table_entry] = GENERIC;
}



SXINLINE static void	error_typed_token (SXNODE *node)
{
    sxerror (node->token.source_index,
	     ysxtables->err_titles [1][0],
	     "%sLexical tokens do not have semantic attributes in YAX.",
	     ysxtables->err_titles [1]+1);
}



SXINLINE static void	found_action (SXNODE *node)
{
    token_type [node->token.string_table_entry] = USER_ACTION;
}



SXINLINE static void	ysx_st_i (void)
{
    switch (SXVISITED->father->name) {
    default:
fault:	gripe ("ysx_st_i");

    case ERROR_n:
	break;

    case DEFS_n:
/* SXVISITED->name = {LCURL_n, LEFT_PRIO_n, NONASSOC_PRIO_n, RIGHT_PRIO_n, START_n, TOKENS_n, TYPE_n, TYPED_TOKENS_n, UNION_n} */
	break;

    case FOLLOWING_RULE_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = RIGHT_HAND_SIDE_n */
	    break;

	case 2:
	    /* SXVISITED->name = {ID_n, LIT_n, POST_ACTION_n, VOID_n} */
	    break;
	}

	break;

    case ID_LIST_n:
	/* SXVISITED->name = ID_n */
	found_id (SXVISITED);
	break;

    case LEADING_RULE_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = C_ID_n */
	    break;

	case 2:
	    /* SXVISITED->name = RIGHT_HAND_SIDE_n */
	    break;

	case 3:
	    /* SXVISITED->name = {ID_n, LIT_n, POST_ACTION_n, VOID_n} */
	    break;
	}

	break;

    case LEFT_PRIO_n:
	/* SXVISITED->name = TOKEN_LIST_n */
	break;

    case NONASSOC_PRIO_n:
	/* SXVISITED->name = TOKEN_LIST_n */
	break;

    case POST_ACTION_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = {ID_n, LIT_n} */
	    break;

	case 2:
	    /* SXVISITED->name = ACTION_n */
	    break;
	}

	break;

    case RIGHT_HAND_SIDE_n:
	/* SXVISITED->name = {ACTION_n, ID_n, LIT_n} */
	break;

    case RIGHT_PRIO_n:
	/* SXVISITED->name = TOKEN_LIST_n */
	break;

    case RULES_n:
	/* SXVISITED->name = {FOLLOWING_RULE_n, LEADING_RULE_n} */
	break;

    case TOKENS_n:
	/* SXVISITED->name = TOKEN_LIST_n */
	break;

    case TOKEN_LIST_n:
	/* SXVISITED->name = {ID_n, ID_NUM_n, LIT_n, LIT_NUM_n} */
	switch (SXVISITED->father->father->name) {
	default:
	    goto fault;

	case LEFT_PRIO_n:
	case NONASSOC_PRIO_n:
	case RIGHT_PRIO_n:
	case TOKENS_n:
	    found_token (SXVISITED);
	    break;

	case TYPED_TOKENS_n:
	    found_typed_token (SXVISITED);
	    break;
	}

	break;

    case TYPE_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = ID_n */
	    current_type = SXVISITED->token.string_table_entry;
	    break;

	case 2:
	    /* SXVISITED->name = ID_LIST_n */
	    break;
	}

	break;

    case TYPED_TOKENS_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = ID_n */
	    break;

	case 2:
	    /* SXVISITED->name = TOKEN_LIST_n */
	    break;
	}

	break;

    case YACC_SPEC_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = DEFS_n */
	    break;

	case 2:
	    /* SXVISITED->name = MARK_n */
	    break;

	case 3:
	    /* SXVISITED->name = RULES_n */
	    if (start == SXERROR_STE) {
		start = sxson (sxson (SXVISITED, 1), 1)->token.string_table_entry;
	    }

	    break;

	case 4:
	    /* SXVISITED->name = {MARK_n, VOID_n} */
	    break;
	}

	break;
    }
}



SXINLINE static void	ysx_st_d (void)
{
    switch (SXVISITED->name) {
    default:
/* fault: */
	gripe ("ysx_st_d"); 

    case ERROR_n:
	break;

    case ACTION_n:
	found_action (SXVISITED);
	break;

    case C_ID_n:
	break;

    case DEFS_n:
	break;

    case FOLLOWING_RULE_n:
	break;

    case ID_n:
	break;

    case ID_LIST_n:
	break;

    case ID_NUM_n:
	needs_tdef = true;
	break;

    case LCURL_n:
	break;

    case LEADING_RULE_n:
	break;

    case LEFT_PRIO_n:
	needs_prio = true;
	break;

    case LIT_n:
	found_token (SXVISITED);
	break;

    case LIT_NUM_n:
	needs_tdef = true;
	found_token (SXVISITED);
	break;

    case MARK_n:
	break;

    case NONASSOC_PRIO_n:
	needs_prio = true;
	break;

    case POST_ACTION_n:
	break;

    case RIGHT_HAND_SIDE_n:
	if (SXVISITED->brother->name != VOID_n) {
	    needs_prio = true;
	}

	break;

    case RIGHT_PRIO_n:
	needs_prio = true;
	break;

    case RULES_n:
	break;

    case START_n:
	start = SXVISITED->token.string_table_entry;
	break;

    case TOKENS_n:
	break;

    case TOKEN_LIST_n:
	break;

    case TYPE_n:
	break;

    case TYPED_TOKENS_n:
	break;

    case UNION_n:
	break;

    case VOID_n:
	break;

    case YACC_SPEC_n:
	break;
    }
}



SXINLINE static void	out_union (SXNODE *node)
{
    fprintf (yax_file, "typedef union <%s>\tYYSTYPE;\n\n", sxstrget (node->token.string_table_entry));
}



SXINLINE static void	out_lcurl (SXNODE *node)
{
    /* LINTED this cast from long to int is needed by printf() */
    fprintf (yax_file, "%.*s\n\n", (int) sxstrlen (node->token.string_table_entry) - 4, sxstrget (node->token.
	 string_table_entry) + 2);
}



SXINLINE static void	out_tail (SXNODE *node)
{
    fprintf (yax_file, "%s\n\n", sxstrget (node->token.string_table_entry));
}



SXINLINE static void	out_start (void)
{
    fprintf (yax_file, "<$start>\t= <%s>\t;\n\n", sxstrget (start));
}



static VARSTR	out_NON_TERMINAL (SXINT ste)
{
    return varstr_catenate (varstr_catenate (varstr_catenate (rule, "<"), sxstrget (ste)), ">");
}



static VARSTR	out_TERMINAL (SXINT ste)
{
    char	*string;

    if ((string = sxstrget (ste)) [0] == '"') {
	/* %LIT */
	return varstr_catenate (rule, string);
    }

    return varstr_quote (rule, string);
}



static VARSTR	out_GENERIC (SXINT ste)
{
    return varstr_catenate (varstr_catenate (rule, "%"), sxstrget (ste));
}



SXINLINE static void	out_lhs (SXINT ste)
{
    varstr_raz (rule);
    varstr_catenate (out_NON_TERMINAL (ste), "\t=");
}



SXINLINE static void	out_id (SXNODE *node)
{
    SXINT	ste;

    if (!node->first_list_element) {
	fprintf (yax_file, ", ");
    }

    switch (token_type [ste = node->token.string_table_entry]) {
    default:
	gripe ("out_id");

    case NON_TERMINAL:
	fprintf (yax_file, "<%s>", sxstrget (ste));
	break;

    case GENERIC:
	error_typed_token (node);
	fprintf (yax_file, "%%%s", sxstrget (ste));
	break;
    }
}



SXINLINE static void	out_action (SXNODE *node)
{
    action_offset = node->position - 1;
    sxsrcpush (EOF, sxstrget (node->token.string_table_entry), node->token.source_index);
    syntax (SXACTION, &yaction_tables);
}



bool yaction_scanact (SXINT what, SXINT numact)
{
    switch (what) {
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
    case SXSEMPASS:
	return SXANY_BOOL;

    case SXACTION:
	switch (numact) {
	case 0:
	    /* <action> = ; */
	    fprintf (yax_file, "\n");
	    return SXANY_BOOL;

	case 1:
	    /* {C_CODE | ATTRIBUTE}+ */
	    /* $ [TYPE] */
	    fprintf (yax_file, "%s", sxsvar.sxlv_s.token_string);
	    break;

	case 2:
	    /* (notype) {DIGIT}+ */
	    {
		SXINT	brother;

		brother = atoi (sxsvar.sxlv_s.token_string);
		fprintf (yax_file, "<%s>%ld", sxstrget (attribute_type [sxbrother (SXVISITED, brother)->token.
		     string_table_entry]), (SXINT) brother - action_offset);
	    }

	    break;

	case 3:
	    /* [-] {DIGIT}+ */
	    fprintf (yax_file, "%ld", (SXINT) atoi (sxsvar.sxlv_s.token_string) - action_offset);
	    break;

	default:
	    gripe ("yaction");
	}

	sxsvar.sxlv.terminal_token.source_index = sxsrcmngr.source_coord;
	sxsvar.sxlv.ts_lgth = 0;
	return SXANY_BOOL;

    default:
	gripe ("yaction");
    }
}

void yaction_semact (SXINT what, SXINT act_no, SXTABLES *arg)
{
    (void) act_no;
    (void) arg;
    yaction_scanact (what, (SXINT) 0);
}


SXINLINE static void	out_rhs (SXNODE *node)
{
    SXINT	ste;

    varstr_catenate (rule, " ");

    switch (token_type [ste = node->token.string_table_entry]) {
    default:
	gripe ("out_rhs");

    case NON_TERMINAL:
	out_NON_TERMINAL (ste);
	break;

    case TERMINAL:
	out_TERMINAL (ste);
	break;

    case GENERIC:
	out_GENERIC (ste);
	break;

    case USER_ACTION:
	if (node->brother != NULL || node->father->brother->name == POST_ACTION_n) {
	    char	string [32];

	    sprintf (string, "<{%ld}>", (SXINT) ++action_no);
	    varstr_catenate (rule, string);
	    fprintf (yax_file, "%s\t=\t;\n\t=", string);
	    out_action (node);
	}
	else
	    rule_post_action = node;

	break;
    }
}



SXINLINE static void	out_prio (char *string)
{
    fprintf (prio_file, "%s", string);
}



SXINLINE static void	end_prio (void)
{
    fprintf (prio_file, "\n");
}



SXINLINE static void	out_token (FILE *file, SXNODE *node)
{
    SXINT	ste;

    switch (token_type [ste = node->token.string_table_entry]) {
    default:
	gripe ("out_token");

    case TERMINAL:
	fprintf (file, " \"%s\"", sxstrget (ste));
	break;

    case GENERIC:
	fprintf (file, " %%%s", sxstrget (ste));
	break;
    }
}



SXINLINE static void	out_attribute (SXNODE *node)
{
    fprintf (yax_file, "$<%s> (", sxstrget (node->token.string_table_entry));
}



SXINLINE static void	end_type (void)
{
    fprintf (yax_file, ")\n");
}



SXINLINE static void	end_rhs (void)
{
    varstr_catenate (rule, "\t;");
    fprintf (yax_file, "%s\n", varstr_tostr (rule));
}



SXINLINE static void	out_post_action (void)
{
    SXINT	column;

    if ((column = rule_post_action->token.source_index.column) <= 4) {
	column = column * 8 - 8 + 1;
    }

    put_edit_initialize (yax_file);
    put_edit (column, sxstrget (rule_post_action->token.string_table_entry));
    rule_post_action = NULL;
}



SXINLINE static void	out_prec (SXNODE *node)
{
    fprintf (prio_file, "%s\t%%prec", varstr_tostr (rule));
    out_token (prio_file, node);
    fprintf (prio_file, "\n");
}



SXINLINE static void	out_tdef (SXNODE *node)
{
    fprintf (tdef_file, "%s\t=", sxstrget (node->token.string_table_entry));
    out_token (tdef_file, node);
    fprintf (tdef_file, " ;\n");
}



SXINLINE static void	ysx_pi (void)
{
    switch (SXVISITED->father->name) {
    default:
fault:	gripe ("ysx_pi");

    case ERROR_n:
	break;

    case DEFS_n:
/* SXVISITED->name = {LCURL_n, LEFT_PRIO_n, NONASSOC_PRIO_n, RIGHT_PRIO_n, START_n, TOKENS_n, TYPE_n, TYPED_TOKENS_n, UNION_n} */
	break;

    case FOLLOWING_RULE_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = RIGHT_HAND_SIDE_n */
	    out_lhs (start);
	    break;

	case 2:
	    /* SXVISITED->name = {ID_n, LIT_n, POST_ACTION_n, VOID_n} */
	    break;
	}

	break;

    case ID_LIST_n:
	/* SXVISITED->name = ID_n */
	out_id (SXVISITED);
	break;

    case LEADING_RULE_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = C_ID_n */
	    break;

	case 2:
	    /* SXVISITED->name = RIGHT_HAND_SIDE_n */
	    out_lhs (start);
	    break;

	case 3:
	    /* SXVISITED->name = {ID_n, LIT_n, POST_ACTION_n, VOID_n} */
	    break;
	}

	break;

    case LEFT_PRIO_n:
	/* SXVISITED->name = TOKEN_LIST_n */
	out_prio ("%left");
	break;

    case NONASSOC_PRIO_n:
	/* SXVISITED->name = TOKEN_LIST_n */
	out_prio ("%nonassoc");
	break;

    case POST_ACTION_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = {ID_n, LIT_n} */
	    break;

	case 2:
	    /* SXVISITED->name = ACTION_n */
	    rule_post_action = SXVISITED;
	    out_post_action ();
	    break;
	}

	break;

    case RIGHT_HAND_SIDE_n:
	/* SXVISITED->name = {ACTION_n, ID_n, LIT_n} */
	out_rhs (SXVISITED);
	break;

    case RIGHT_PRIO_n:
	/* SXVISITED->name = TOKEN_LIST_n */
	out_prio ("%right");
	break;

    case RULES_n:
	/* SXVISITED->name = {FOLLOWING_RULE_n, LEADING_RULE_n} */
	break;

    case TOKENS_n:
	/* SXVISITED->name = TOKEN_LIST_n */
	break;

    case TOKEN_LIST_n:
	/* SXVISITED->name = {ID_n, ID_NUM_n, LIT_n, LIT_NUM_n} */
	switch (SXVISITED->father->father->name) {
	default:
	    goto fault;

	case LEFT_PRIO_n:
	case NONASSOC_PRIO_n:
	case RIGHT_PRIO_n:
	    out_token (prio_file, SXVISITED);
	    break;

	case TOKENS_n:
	    break;

	case TYPED_TOKENS_n:
	    break;
	}

	break;

    case TYPE_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = ID_n */
	    out_attribute (SXVISITED);
	    break;

	case 2:
	    /* SXVISITED->name = ID_LIST_n */
	    break;
	}

	break;

    case TYPED_TOKENS_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = ID_n */
	    error_typed_token (SXVISITED);
	    break;

	case 2:
	    /* SXVISITED->name = TOKEN_LIST_n */
	    break;
	}

	break;

    case YACC_SPEC_n:
	switch (SXVISITED->position) {
	default:
	    goto fault;

	case 1:
	    /* SXVISITED->name = DEFS_n */
	    break;

	case 2:
	    /* SXVISITED->name = MARK_n */
	    break;

	case 3:
	    /* SXVISITED->name = RULES_n */
	    if (SXVISITED->brother->name != VOID_n) {
		out_tail (SXVISITED->brother);
	    }

	    out_start ();
	    break;

	case 4:
	    /* SXVISITED->name = {MARK_n, VOID_n} */
	    break;
	}

	break;
    }
}



SXINLINE static void	ysx_pd (void)
{
    switch (SXVISITED->name) {
    default:
fault:	gripe ("ysx_pd");

    case ERROR_n:
	break;

    case ACTION_n:
	break;

    case C_ID_n:
	fprintf (yax_file, "\n");
	start = SXVISITED->token.string_table_entry;
	break;

    case DEFS_n:
	if (needs_prio) {
	    fprintf (prio_file, "\n");
	}

	break;

    case FOLLOWING_RULE_n:
	break;

    case ID_n:
	break;

    case ID_LIST_n:
	break;

    case ID_NUM_n:
	out_tdef (SXVISITED);
	break;

    case LCURL_n:
	out_lcurl (SXVISITED);
	break;

    case LEADING_RULE_n:
	break;

    case LEFT_PRIO_n:
	end_prio ();
	break;

    case LIT_n:
	break;

    case LIT_NUM_n:
	out_tdef (SXVISITED);
	break;

    case MARK_n:
	break;

    case NONASSOC_PRIO_n:
	end_prio ();
	break;

    case POST_ACTION_n:
	break;

    case RIGHT_HAND_SIDE_n:
	end_rhs ();

	if (rule_post_action != NULL) {
	    out_post_action ();
	}

	switch (SXVISITED->brother->name) {
	default:
	    goto fault;

	case VOID_n:
	    break;

	case ID_n:
	case LIT_n:
	    out_prec (SXVISITED->brother);
	    break;

	case POST_ACTION_n:
	    out_prec (SXVISITED->brother->son);
	    break;
	}

	break;

    case RIGHT_PRIO_n:
	end_prio ();
	break;

    case RULES_n:
	break;

    case START_n:
	break;

    case TOKENS_n:
	break;

    case TOKEN_LIST_n:
	break;

    case TYPE_n:
	end_type ();
	break;

    case TYPED_TOKENS_n:
	break;

    case UNION_n:
	out_union (SXVISITED);
	break;

    case VOID_n:
	break;

    case YACC_SPEC_n:
	break;
    }
}



SXINLINE static void	smpopen (SXTABLES *sxtables_ptr)
{
    sxatcvar.atc_lv.node_size = sizeof (struct ysx_node);
    ysxtables = sxtables_ptr;
}



static bool	init_yax (void)
{
    char	name [32];

    if ((yax_file = sxfopen (strcat (strcpy (name, prgentname), ".yax"), "w")) == NULL) {
nope:	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	sxperror (name);
	return false;
    }

    if (needs_prio && (prio_file = sxfopen (strcat (strcpy (name, prgentname), ".prio"), "w")) == NULL) {
	goto nope;
    }

    if (needs_tdef && (tdef_file = sxfopen (strcat (strcpy (name, prgentname), ".tdef"), "w")) == NULL) {
	goto nope;
    }

    return true;
}



SXINLINE static void	smppass (void)
{
    bool stack_is_already_freed;
    struct {
	struct sxsvar	sxsvar;
	struct sxplocals	sxplocals;
    }	ysx /* pour sauver les tables de ysx */ ;

    if (sxverbosep) {
	fputs ("Semantic Pass\n", sxtty);
    }

    ysx.sxsvar = sxsvar;
    ysx.sxplocals = sxplocals;
    syntax (SXOPEN, &yaction_tables);

    if (sxverbosep) {
	fputs ("    Symbols Table\n", sxtty);
    }

    needs_prio = false;
    needs_tdef = false;
    start = SXERROR_STE;
    token_type = sxcalloc (sxstrmngr.top, sizeof (*token_type));
    attribute_type = (SXINT*) sxcalloc (sxstrmngr.top, sizeof (*attribute_type));
    sxsmp (sxatcvar.atc_lv.abstract_tree_root, ysx_st_i, ysx_st_d);

    if (sxverbosep) {
	fputs ("    YAX Generation\n", sxtty);
    }

    if (init_yax ()) {
	rule = varstr_alloc (1024);
	action_no = 0;
	rule_post_action = NULL;
	sxsmp (sxatcvar.atc_lv.abstract_tree_root, ysx_pi, ysx_pd);

	if (sxverbosep) {
	    fputs ("    Cleanup\n", sxtty);
	}

	if (needs_tdef) {
	    fclose (tdef_file);
	}

	if (needs_prio) {
	    fclose (prio_file);
	}

	fclose (yax_file);
	varstr_free (rule);
    }

    sxfree (attribute_type);
    sxfree (token_type);
    syntax (SXCLOSE, &yaction_tables);
    sxsvar = ysx.sxsvar;
    
    /* si sxplocals.rcvr.stack == NULL, alors on a deja appele
         sxfree ( sxplocals.rcvr.stack )
       la copie ysx.sxplocals peut avoir une ancienne valeur pour
       le pointeur .rcvr.stack . Si cette valeur n'est pas NULL
       alors, il y aura un probleme lorsqu'on essaiera de faire
       sxfree( sxplocals.rcvr.stack ) une deuxieme fois (ce pointeur
       n'etant plus valide). 

       C'est pourquoi nous verifions si le pointeur .rcvr.stack vaut
       deja NULL, ce qui voudrait dire qu'il a deja ete libere, et
       qu'il ne faut pas refaire cette liberation

       Voici comment on pouvait provoquer le probleme
       (avant cette reparation : avant la revision 1425) :

         ysx -v /dev/null

    */
       
    stack_is_already_freed = (sxplocals.rcvr.stack == NULL);
      
    sxplocals = ysx.sxplocals;

    if (stack_is_already_freed)
      sxplocals.rcvr.stack = NULL;
}


void
ysx_smp (SXINT what, SXTABLES *sxtables_ptr)
{
    switch (what) {
    default:
	gripe ("ysx_smp");

    case SXOPEN:
	smpopen (sxtables_ptr);
	break;

    case SXACTION:
	smppass ();
	break;

    case SXCLOSE:
	break;
    }
}
