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
#include "B_tables.h"
#include "yax_vars.h"

char WHAT_TAXDEFACT[] = "@(#)SYNTAX - $Id: ydef_act.c 4441 2024-10-13 10:50:56Z garavel $" WHAT_DEBUG;

static bool	has_defining_occurrence = false, has_semantics = false;
static SXINT	attribute_needed = 0;
static struct sxsource_coord	RULE_source_coord;
static SXINT	xprod = 0;

extern SXTABLES	bnf_tables;

/* forward */ SXSCANACT_FUNCTION def_scanact;

extern void    bnf_found_bad_beginning_of_rule (void);
extern void    bnf_skip_rule (void);

/* dans yax_put.c : */
extern void      put_postlude (void);

/* dans yax.c : */

extern SXINT	chercher_attr (SXINT ste);

/*   --------------------------------------------------------- */

static bool	pos_ok (SXINT pos, bool is_typed)
{
    if (pos > LGPROD (xprod) || (pos <= 0 && !is_typed)) {
	sxerror (sxsvar.sxlv.terminal_token.source_index,
		 bnf_tables.err_titles [2][0],
		 "%s%s.",
		 bnf_tables.err_titles [2]+1,
		 pos <= 0 ?
		 "This attribute must be typed" : "No such symbol in this rule");
	is_err = true;
	return false;
    }

    return true;
}

/*   --------------------------------------------------------- */

static bool	nt_ok (SXINT nt)
{
    if (nt <= 0 || IS_A_PARSACT (nt) /* action */ ) {
	sxerror (sxsvar.sxlv.terminal_token.source_index,
		 bnf_tables.err_titles [2][0],
		 "%sThis refers to \"%s\", which is not a non-terminal.",
		 bnf_tables.err_titles [2]+1,
		 sxstrget (nt <= 0 ? t_to_ste [-nt] : nt_to_ste [nt]));
	is_err = true;
	return false;
    }
 
    return true;
}

/*   --------------------------------------------------------- */

static void	put_named_type (SXINT ste)
{
    nb_occurrences++;
    /* LINTED this cast from long to int is needed by printf() */
    fprintf (sxstdout, ".%.*s", (int) sxstrlen (ste) - 2, sxstrget (ste) + 1);
}

static void	put_type (SXINT nt)
{
    SXINT	attr;

    for (attr = 1; attr <= max_attr; attr++) {
	if (attr_nt [attr] [nt]) {
	    put_named_type (attr_to_ste [attr]);
	    return;
	}
    }

    sxerror (sxsvar.sxlv.terminal_token.source_index,
	     bnf_tables.err_titles[2][0],
	     "%sNo attribute has been declared for %s.",
	     bnf_tables.err_titles[2]+1,
	     sxstrget (nt_to_ste [nt]));
    is_err = true;
}

/*   --------------------------------------------------------- */

static void	found_text (void)
{
    if (!has_semantics) {
	has_semantics = true;
	fprintf (sxstdout, "\n#line 73 \"yax_pattern\"\ncase %ld:\n#line %lu \"%s\"\n", (SXINT) xprod, sxsvar.sxlv.terminal_token.source_index.line, sxsvar.
	     sxlv.terminal_token.source_index.file_name);
    }
}

/*   --------------------------------------------------------- */

static void	default_identity (void)
{
    SXINT	nt;

    if ((nt = WI [WN [xprod].prolon].lispro) <= 0 || !attr_nt [attribute_needed] [XNT_TO_NT (nt)]) {
	SXINT	rtx;
	char	*fmt;

	if (has_semantics) {
	    fmt = "%sThis non-terminal has a <%s attribute, but the semantics seems not to define it.";
	    rtx = 1;
	}
	else {
	    fmt = "%sThe default \"$$ = $1\" action does not define the <%s attribute of this non-terminal.";
	    rtx = 2;
	    is_err = true;
	}

	sxerror (RULE_source_coord,
		 bnf_tables.err_titles [rtx][0],
		 fmt,
		 bnf_tables.err_titles [rtx]+1,
		 sxstrget (attr_to_ste [attribute_needed]) + 1);
    }
}

/*   --------------------------------------------------------- */

static void	found_rule (void)
{
    SXINT	nt;
    SXINT	attr;

    if (attribute_needed != 0) {
	if (!has_defining_occurrence)
	    default_identity ();

	attribute_needed = 0;
    }

    if (has_semantics) {
	if (has_defining_occurrence) {
	    has_defining_occurrence = false;
	    nb_definitions++;
	}

	nb_sem_rules++;
	fputs ("\n#line 76 \"yax_pattern\"\nbreak;", sxstdout);
	WN [xprod].action = xprod;
	WN [xprod].bprosimpl = false;
	has_semantics = false;
    }

    nt = WN [++xprod].reduc;

    for (attr = 1; attr <= max_attr; attr++) {
	if (attr_nt [attr] [nt]) {
	    attribute_needed = attr;
	    RULE_source_coord = sxsvar.sxlv.terminal_token.source_index;
	    break;
	}
    }
}

/*   --------------------------------------------------------- */

static void	gripe (void)
#ifdef __GNUC__
     __attribute__ ((noreturn))
#endif
     ;

static void	gripe (void)
{
    fputs ("\nThe function \"def_act\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}

/*   --------------------------------------------------------- */

/*     A C T I O N     */


bool def_scanact (SXINT code, SXINT numact)
{
    switch (code) {
    default:
	gripe ();

    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
    case SXSEMPASS:
	return SXANY_BOOL;

    case SXERROR:
	is_err = true;
	return SXANY_BOOL;

    case SXACTION:
	switch (numact) {
	    static SXINT	type_ste, xattr;
	    SXINT	pos, nt;

	default:
	    gripe ();

	case 0:
	    /* "<"&Is_First_Col when it should not */
	    bnf_found_bad_beginning_of_rule ();
	    return SXANY_BOOL;

	case 1:
	    /* RULE */
	    found_rule ();
	    break;

	case 2:
	    /* C_CODE */
	    found_text ();
	    fputs (sxsvar.sxlv_s.token_string, sxstdout);
	    break;

	case 3:
	    bnf_skip_rule ();
	    return SXANY_BOOL;

	case 5:
	    /* $$ */
	    found_text ();
	    has_defining_occurrence = true;
	    fputs ("yyval", sxstdout);
	    put_type (WN [xprod].reduc);
	    break;

	case 6:
	    /* $ {DIGIT}+ */
	case 7:
	    /* $ - {DIGIT}+ */
	    found_text ();
	    fprintf (sxstdout, "yypv[%s]", sxsvar.sxlv_s.token_string + 1);

	    if (pos_ok (pos = atoi (sxsvar.sxlv_s.token_string + 1), false) && nt_ok (nt = WI [WN [xprod].prolon + pos -
		 1].lispro)) {
		put_type (XNT_TO_NT (nt));
	    }

	    break;

	case 8:
	    /* $ TYPE $ */
	    has_defining_occurrence = true;
	    fputs ("yyval", sxstdout);

	    if (xattr >= 0) {
		put_named_type (type_ste);
	    }
	    else {
		sxerror (sxsvar.sxlv.terminal_token.source_index,
			 bnf_tables.err_titles [2][0],
			 "%sIllegal use of a terminal attribute.",
			 bnf_tables.err_titles [2]+1);
		is_err = true;
	    }

	    break;

	case 9:
	    /* $ TYPE [-] {DIGIT}+ */
	    fprintf (sxstdout, xattr >= 0 ? "yypv[%s]" : "SXSTACKtoken(SXSTACKnewtop()+(%s-1))", sxsvar.sxlv_s.
		 token_string);

	    if (pos_ok (pos = atoi (sxsvar.sxlv_s.token_string), true)) {
		if (xattr >= 0) {
		    if (pos <= 0 || nt_ok (WI [WN [xprod].prolon + pos - 1].lispro)) {
			put_named_type (type_ste);
		    }
		}
	    }

	    break;

	case 10:
	    /* $ TYPE ... */
	    found_text ();
	    terminal_token = &sxsvar.sxlv.terminal_token;
	    xattr = chercher_attr (type_ste = sxstr2save (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth));
	    /* Don't touch the current source coordinates */
	    sxsvar.sxlv.ts_lgth = 0;
	    return SXANY_BOOL;

	case 11:
	    /* BNF_COMMENT */
	    if (has_semantics) {
		putc (SXNEWLINE, sxstdout);
	    }

	    break;
	}

	sxsvar.sxlv.terminal_token.source_index = sxsrcmngr.source_coord;
	sxsvar.sxlv.ts_lgth = 0;
	return SXANY_BOOL;
    }
}


void def_semact (SXINT code, SXINT numact, SXTABLES *arg)
{
    (void) arg;
    switch (code) {
    case SXERROR:
      is_err = true;
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
    case SXSEMPASS:
	return;

    case SXACTION:
	switch (numact) {

	default:
	    gripe ();

	case 12:
	    /* <defs> = ; */
	    found_rule ();
	    put_postlude ();
	    return;
	}

    default:
	gripe ();
    }
}
