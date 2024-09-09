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

#include "sxversion.h"
#include "sxunix.h"
#include "B_tables.h"
#include "xbnf_vars.h"

char WHAT_XBNFSACT[] = "@(#)SYNTAX - $Id: xbnf_sact.c 4223 2024-09-03 08:03:50Z garavel $" WHAT_DEBUG;

#define rule_slice 100

static struct span {
	   SXUINT	head;
	   struct tail {
	       SXUINT	line;
	       SXUINT	column;
	   }	tail;
       }	*coords;
static SXINT	xprod, rules_no;
static SXINT	current_rule_no;



SXUINT	bnf_get_line_no (SXINT rule_no)
{
    return rule_no > xprod ? 0 : coords [rule_no].head;
}



void	bnf_get_rule_tail (SXUINT rule_no, SXUINT *tline, SXUINT *tcol)
{
    *tline = coords [rule_no].tail.line;
    *tcol = coords [rule_no].tail.column;
}



void	bnf_skip_rule (void)
{
    struct tail	*tail_coord;
    SXUINT	line;
    SXUINT	column;

    if (current_rule_no >= xprod) {
	current_rule_no = 0;
    }

    tail_coord = &coords [++current_rule_no].tail;
    line = tail_coord->line;
    column = tail_coord->column;

    while (sxsrcmngr.source_coord.line < line) {
	if (sxnext_char () == EOF)
	    return;
    }

    while (sxsrcmngr.source_coord.column < column) {
	if (sxnext_char () == EOF)
	    return;
    }
}



void	bnf_found_bad_beginning_of_rule (void)
{
    struct sxsource_coord	less_coord;

    less_coord = sxsrcmngr.source_coord, less_coord.column = 1;
    sxerror (less_coord,
	     sxsvar.sxtables->err_titles [2][0],
	     "%sIllegal occurrence of \"<\"; the head of a new grammar rule is assumed.",
	     sxsvar.sxtables->err_titles [2]+1);
    sxsrcpush ('\n', "<", less_coord);
}



static void	gripe (void)
{
    fputs ("\nThe function \"sxscanner_action\" for xbnf is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}



void	(*more_scan_act) (SXINT code, SXINT act_no);


bool sxscanner_action (SXINT code, SXINT act_no)
{
    switch (code) {
    case SXOPEN:
	coords = (struct span*) sxalloc ((rules_no = rule_slice) + 1, sizeof (struct span));

	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

	return SXANY_BOOL;

    case SXCLOSE:
	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

	if (coords != NULL) {
	    sxfree (coords);
	}

	return SXANY_BOOL;

    case SXINIT:
	is_predicate = false;

	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

	{
	    short	c;

	    c = sxsrcmngr.current_char;

	    while (c != EOF)
		if (c == '<' && sxsrcmngr.source_coord.column == 1) {
		    coords [xprod = 1].head = sxsrcmngr.source_coord.line;
		    return SXANY_BOOL;
		}
		else
		    c = sxnext_char ();
	}

	fprintf (sxstderr, "\n\tThis text is not a grammar... see you later.\n");
	sxexit (3);

    case SXFINAL:
	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

	return SXANY_BOOL;

    case SXACTION:
	switch (act_no) {
	    short	c;

	case 1:
	    {
		coords [xprod].tail.line = sxsrcmngr.source_coord.line;
		coords [xprod].tail.column = sxsrcmngr.source_coord.column;

		if (more_scan_act != NULL) {
		    (*more_scan_act) (code, act_no);
		}

		c = sxsrcmngr.current_char;

		while (c != EOF)
		    if (c == '<' && sxsrcmngr.source_coord.column == 1) {
			if (++xprod > rules_no) {
			    coords = (struct span*) sxrealloc (coords, (rules_no += rule_slice) + 1, sizeof (struct span));
			}

			coords [xprod].head = sxsrcmngr.source_coord.line;
			return SXANY_BOOL;
		    }
		    else
			c = sxnext_char ();
	    }
	    return SXANY_BOOL;

	case 2:
	    /* \nnn => char */
	    {
		char	val;
		char	xbnf_scan_act_c, *s, *t;

		t = s = sxsvar.sxlv_s.token_string + sxsvar.sxlv.mark.index;

		for (val = *s++ - '0'; (xbnf_scan_act_c = *s++) != SXNUL; ) {
		    val = (val << 3) + xbnf_scan_act_c - '0';
		}

		*t = val;
		sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index + 1;
		sxsvar.sxlv.mark.index = -1;
	    }
	    return SXANY_BOOL;

	case 3:
	    /* Post-action de %SXPREDICATE : is_predicate est utilise dans bnf_symtab */
	    is_predicate = true;
	    return SXANY_BOOL;

	default:
#if EBUG
	  sxtrap("xbnf_sact","unknown switch case #1");
#endif
	  break;
	}
        break;

    default:
	gripe ();
    }
    return SXANY_BOOL;
}
