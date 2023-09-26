/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/



#define SX_DFN_SXDG /* for SXDG */


#include "sxunix.h"
#include "tables.h"
#include "out.h"
#include "sxdynam_parser.h"
char WHAT_TABLESOUTDPTBL[] = "@(#)SYNTAX - $Id: out_DP_tbl.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

static char	ME [] = "DYNAMIC_PARSER_read";


static void out_tabint2 (FILE *file, char *name, SXINT *tab, SXINT size, SXINT top, char *elt_m1)
{
    SXINT i;

    fprintf (file, "\n\nstatic SXINT %s[%ld]={%s", name, (long)size + 1, elt_m1);

    for (i = 0; i <= top; i++) {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %ld */ ", (long)i);

	fprintf (file, "%ld, ", (long)tab [i]);
    } 

    fprintf (file, "\n} /* End %s */;\n", name);
}

static void out_tab_struct (FILE *file,
			    char *struct_name, 
			    char *name,
			    void (*print) (SXINT),
			    SXINT size, 
			    SXINT top)
{
    SXINT i;

    fprintf (file, "\n\nstatic %s %s [%ld] = {", struct_name, name, (long)size + 1);

    for (i = 0; i <= top; i++) {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %ld */ ", (long)i);

	print (i);
    } 

    fprintf (file, "\n} /* End %s %s */;\n", struct_name, name);
}


static void print_r_priorities (SXINT rule)
{
    fprintf (sxstdout, "{%ld, %ld}, ", (long)SXDG.r_priorities [rule].assoc,
	     (long)SXDG.r_priorities [rule].value);
}

static void print_t_priorities (SXINT rule)
{
    fprintf (sxstdout, "{%ld, %ld}, ", (long)SXDG.t_priorities [rule].assoc,
	     (long)SXDG.t_priorities [rule].value);
}

SXBOOLEAN
dp_read (char *ln)
{
    sxfiledesc_t	file_descr	/* file descriptor */ ;
    char	file_name [128];
    SXBOOLEAN     is_prio;

    if (sxverbosep)
	fputs ("\tDynamic Parser Tables\n", sxtty);

    if ((file_descr = open (strcat (strcpy (file_name, ln), ".dp"), 0)) < 0) {
	fprintf (sxstderr, "%s: cannot open ", ME);
	sxperror (file_name);
	return SXFALSE;
    }

    if (!sxdp_read (file_descr))
     {
	fprintf (sxstderr, "%s: cannot read ", ME);
	sxperror (file_name);
	return SXFALSE;
    }

    close (file_descr);

    is_prio = SXDG.r_priorities != NULL;

    fprintf (sxstdout, "\n#include \"sxdynam_parser.h\"\n");

    sxlist_array_to_c (&SXDG.states, sxstdout, "SXDG_states");

    XxY_array_to_c (&SXDG.P, sxstdout, "P");
    XxY_array_to_c (&SXDG.rhs, sxstdout, "rhs");
    XxY_array_to_c (&SXDG.items, sxstdout, "items");
    XxY_array_to_c (&SXDG.transitions, sxstdout, "transitions");
    XxY_array_to_c (&SXDG.item_occurs, sxstdout, "item_occurs");
    XxY_array_to_c (&SXDG.transitionxnext_state, sxstdout, "transitionxnext_state");
    XxY_array_to_c (&SXDG.xt, sxstdout, "xt");
    XxY_array_to_c (&SXDG.xnt, sxstdout, "xnt");

    sxba_to_c (SXDG.item_set, sxstdout, "SXDG", "_item_set", SXTRUE /* static */);
    sxba_to_c (SXDG.trans_has_prdct, sxstdout, "SXDG", "_trans_has_prdct", SXTRUE /* static */);
    sxba_to_c (SXDG.is_state_linked, sxstdout, "SXDG", "_is_state_linked", SXTRUE /* static */);
    sxba_to_c (SXDG.active_rule_set, sxstdout, "SXDG", "_active_rule_set", SXTRUE /* static */);

    out_tabint2 (sxstdout, "SXDG_rule_to_rhs_lgth", SXDG.rule_to_rhs_lgth,
		 XxY_size (SXDG.P), XxY_top (SXDG.P), "");
    out_tabint2 (sxstdout, "SXDG_rule_to_first_item", SXDG.rule_to_first_item,
		 XxY_size (SXDG.P), XxY_top (SXDG.P), "");
    out_tabint2 (sxstdout, "SXDG_rule_to_action", SXDG.rule_to_action,
		 XxY_size (SXDG.P), XxY_top (SXDG.P), "");
    out_tabint2 (sxstdout, "SXDG_state_to_action", SXDG.state_to_action,
		 sxlist_size (SXDG.states), sxlist_size (SXDG.states), "");
    out_tabint2 (sxstdout, "SXDG_state_to_in_symbol", SXDG.state_to_in_symbol,
		 sxlist_size (SXDG.states), sxlist_size (SXDG.states), "");
    out_tabint2 (sxstdout, "SXDG_state_to_used_lnk", SXDG.state_to_used_lnk,
		 sxlist_size (SXDG.states), sxlist_size (SXDG.states), "");
    out_tabint2 (sxstdout, "SXDG_item_to_next_item", SXDG.item_to_next_item,
		 XxY_size (SXDG.items), XxY_top (SXDG.items), "");
    out_tabint2 (sxstdout, "SXDG_trans_to_next_state", SXDG.trans_to_next_state,
		 XxY_size (SXDG.transitions), XxY_top (SXDG.transitions), "");

    if (SXDG.r_priorities != NULL) {
	out_tab_struct (sxstdout, "struct priority", "SXDG_r_priorities", print_r_priorities, XxY_size (SXDG.P), XxY_top (SXDG.P));
	out_tab_struct (sxstdout, "struct priority", "SXDG_t_priorities", print_t_priorities, XxY_size (SXDG.xt), XxY_top (SXDG.xt));
    }

    fprintf (sxstdout, "\n\nstruct sxdynam_parser SXDG = {\n");
    sxlist_header_to_c (&(SXDG.states), sxstdout, "SXDG_states");
    fprintf (sxstdout, " /* sxlist_header states */,\n");

    XxY_header_to_c (&SXDG.P, sxstdout, "P");
    fprintf (sxstdout, " /* End XxY_header P */,\n");
    XxY_header_to_c (&SXDG.rhs, sxstdout, "rhs");
    fprintf (sxstdout, " /* End XxY_header rhs */,\n");
    XxY_header_to_c (&SXDG.items, sxstdout, "items");
    fprintf (sxstdout, " /* End XxY_header items */,\n");
    XxY_header_to_c (&SXDG.transitions, sxstdout, "transitions");
    fprintf (sxstdout, " /* End XxY_header transitions */,\n");
    XxY_header_to_c (&SXDG.item_occurs, sxstdout, "item_occurs");
    fprintf (sxstdout, " /* End XxY_header item_occurs */,\n");
    XxY_header_to_c (&SXDG.transitionxnext_state, sxstdout, "transitionxnext_state");
    fprintf (sxstdout, " /* End XxY_header transitionxnext_state */,\n");
    XxY_header_to_c (&SXDG.xt, sxstdout, "xt");
    fprintf (sxstdout, " /* End XxY_header xt */,\n");
    XxY_header_to_c (&SXDG.xnt, sxstdout, "xnt");
    fprintf (sxstdout, " /* End XxY_header xnt */,\n");

    fprintf (sxstdout, "SXDG_item_set /* SXBA item_set */,\n");
    fprintf (sxstdout, "SXDG_trans_has_prdct /* SXBA trans_has_prdct */,\n");
    fprintf (sxstdout, "SXDG_is_state_linked /* SXBA is_state_linked */,\n");
    fprintf (sxstdout, "SXDG_active_rule_set /* SXBA active_rule_set */,\n");

    fprintf (sxstdout, "SXDG_rule_to_rhs_lgth /* SXINT *rule_to_rhs_lgth */,\n");
    fprintf (sxstdout, "SXDG_rule_to_first_item /* SXINT *rule_to_first_item */,\n");
    fprintf (sxstdout, "SXDG_rule_to_action /* SXINT *rule_to_action */,\n");
    fprintf (sxstdout, "SXDG_state_to_action /* SXINT *state_to_action */,\n");
    fprintf (sxstdout, "SXDG_state_to_in_symbol /* SXINT *state_to_in_symbol */,\n");
    fprintf (sxstdout, "SXDG_state_to_used_lnk /* SXINT *state_to_used_lnk */,\n");
    fprintf (sxstdout, "SXDG_item_to_next_item /* SXINT *item_to_next_item */,\n");
    fprintf (sxstdout, "SXDG_trans_to_next_state /* SXINT *trans_to_next_state */,\n");
    fprintf (sxstdout, "/* SXINT *prdct_stack */ %s\n", P0);
    fprintf (sxstdout, "/* SXINT *next_state_stack */ %s\n", P0);
    fprintf (sxstdout, "/* SXINT *item_stack */ %s\n", P0);

    fprintf (sxstdout, "0 /* SXINT lgt1 */,\n");
    fprintf (sxstdout, "%ld /* SXINT init_state */,\n", (long)SXDG.init_state);
    fprintf (sxstdout, "%ld /* SXINT init_transition */,\n", SXDG.init_transition);
    fprintf (sxstdout, "%ld /* SXINT super_start_symbol */,\n", (long)SXDG.super_start_symbol);
    fprintf (sxstdout, "%ld /* SXINT start_symbol */,\n", (long)SXDG.start_symbol);
    fprintf (sxstdout, "%ld /* SXINT super_rule */,\n", (long)SXDG.super_rule);
    fprintf (sxstdout, "%ld /* SXINT eof_code */,\n", (long)SXDG.eof_code);
    fprintf (sxstdout, "0 /* SXINT parse_stack */,\n");
    fprintf (sxstdout, "0 /* SXINT Mtok_no */,\n");
    fprintf (sxstdout, "0 /* SXINT ambig_stack_size */,\n");
    fprintf (sxstdout, "0 /* SXINT ambig_stack_top */,\n");

    fprintf (sxstdout, "/* struct priority *t_priorities */ %s\n", is_prio ? "SXDG_t_priorities," : P0);
    fprintf (sxstdout, "/* struct priority *r_priorities */ %s\n", is_prio ? "SXDG_r_priorities," : P0);

    fprintf (sxstdout, "/* ... */\n");
    fprintf (sxstdout, "} /* struct sxdynam_parser SXDG */;\n");

    return SXTRUE;
}

