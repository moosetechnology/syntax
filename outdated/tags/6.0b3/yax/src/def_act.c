/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (bl)  *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 18-10-90 13:30 (pb):		Suppression du champ .lgprod de bnf_ag	*/
/************************************************************************/
/* 06-06-88 14:20 (pb):		BITSTR => SXBA				*/
/************************************************************************/
/* 11-04-88 10:16 (pb):		Adaptation actions et Predicats   	*/
/************************************************************************/
/* 11-12-87 11:12 (phd):	Ajout des #line ... "yax_pattern"   	*/
/************************************************************************/
/* 25-05-87 18:02 (phd):	Ajout de l'entree SEMPASS	   	*/
/************************************************************************/
/* 25-05-87 16:18 (phd):	Point d'entree ERROR			*/
/************************************************************************/
/* 18-05-87 (phd):	Adaptation au bnf nouveau			*/
/************************************************************************/
/* 28-04-87 (phd):	Emission de messages lorsque la semantique est	*/
/*			impure.						*/
/************************************************************************/
/* 23-04-87 (phd):	Remplacement de "-1+1" par "+(1-1)"...		*/
/************************************************************************/
/* 22-04-87 (phd):	Ajout de l'attribut terminal "sxtoken"		*/
/************************************************************************/
/* 21-04-87 (phd):	Refonte totale					*/
/************************************************************************/
/* 1*-04-87 (phd):	Reecritures, ameliorations et optimisations	*/
/************************************************************************/
/* 01/04/87 (bl):	Adaptation a partir de la version 1 de tabc	*/
/************************************************************************/

#include "sxunix.h"
#include "B_tables.h"
#include "yax_vars.h"
char WHAT_TAXDEFACT[] = "@(#)SYNTAX - $Id: def_act.c 1123 2008-02-29 11:27:32Z rlacroix $" WHAT_DEBUG;


static BOOLEAN	has_defining_occurrence = FALSE, has_semantics = FALSE;
static SXINT	attribute_needed = 0;
static struct sxsource_coord	RULE_source_coord;
static SXINT	xprod = 0;
extern struct sxtables	bnf_tables;
extern SXVOID    bnf_found_bad_beginning_of_rule (void);
extern SXVOID    bnf_skip_rule (void);

/* dans yax_put.c : */
extern VOID      put_postlude (void);

/* dans yax.c : */

extern SXINT	chercher_attr (SXINT ste);




/*   --------------------------------------------------------- */

static BOOLEAN	pos_ok (SXINT pos, BOOLEAN is_typed)
{
    if (pos > LGPROD (xprod) || (pos <= 0 && !is_typed)) {
	sxput_error (sxsvar.lv.terminal_token.source_index, "%s%s.", bnf_tables.err_titles [2], pos <= 0 ?
	     "This attribute must be typed" : "No such symbol in this rule");
	is_err = TRUE;
	return FALSE;
    }

    return TRUE;
}




/*   --------------------------------------------------------- */

static BOOLEAN	nt_ok (SXINT nt)
{
    if (nt <= 0 || IS_A_PARSACT (nt) /* action */ ) {
	sxput_error (sxsvar.lv.terminal_token.source_index, "%sThis refers to \"%s\", which is not a non-terminal.",
	     bnf_tables.err_titles [2], sxstrget (nt <= 0 ? t_to_ste [-nt] : nt_to_ste [nt]));
	is_err = TRUE;
	return FALSE;
    }
 
    return TRUE;
}




/*   --------------------------------------------------------- */

static VOID	put_named_type (SXINT ste)
{
    nb_occurrences++;
    /* LINTED this cast from long to int is needed by printf() */
    fprintf (sxstdout, ".%.*s", (int) sxstrlen (ste) - 2, sxstrget (ste) + 1);
}



static VOID	put_type (SXINT nt)
{
    SXINT	attr;

    for (attr = 1; attr <= max_attr; attr++) {
	if (attr_nt [attr] [nt]) {
	    put_named_type (attr_to_ste [attr]);
	    return;
	}
    }

    sxput_error (sxsvar.lv.terminal_token.source_index, "%sNo attribute has been declared for %s.", bnf_tables.err_titles
	 [2], sxstrget (nt_to_ste [nt]));
    is_err = TRUE;
}




/*   --------------------------------------------------------- */

static VOID	found_text (void)
{
    if (!has_semantics) {
	has_semantics = TRUE;
	fprintf (sxstdout, "\n#line 73 \"yax_pattern\"\ncase %ld:\n#line %lu \"%s\"\n", (long)xprod, sxsvar.sxlv.terminal_token.source_index.line, sxsvar.
	     sxlv.terminal_token.source_index.file_name);
    }
}




/*   --------------------------------------------------------- */

static VOID	default_identity (void)
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
	    is_err = TRUE;
	}

	sxput_error (RULE_source_coord, fmt, bnf_tables.err_titles [rtx], sxstrget (attr_to_ste [attribute_needed]) + 1);
    }
}




/*   --------------------------------------------------------- */

static VOID	found_rule (void)
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
	    has_defining_occurrence = FALSE;
	    nb_definitions++;
	}

	nb_sem_rules++;
	fputs ("\n#line 76 \"yax_pattern\"\nbreak;", sxstdout);
	WN [xprod].action = xprod;
	WN [xprod].bprosimpl = FALSE;
	has_semantics = FALSE;
    }

    nt = WN [++xprod].reduc;

    for (attr = 1; attr <= max_attr; attr++) {
	if (attr_nt [attr] [nt]) {
	    attribute_needed = attr;
	    RULE_source_coord = sxsvar.lv.terminal_token.source_index;
	    break;
	}
    }
}




/*   --------------------------------------------------------- */

static VOID	gripe (void)
{
    fputs ("\nThe function \"def_act\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}




/*   --------------------------------------------------------- */

/*     A C T I O N     */

VOID	def_act (SXINT code, SXINT numact)
{
    switch (code) {
    default:
	gripe ();

    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
    case SEMPASS:
	return;

    case ERROR:
	is_err = TRUE;
	return;

    case ACTION:
	switch (numact) {
	    static SXINT	type_ste, xattr;
	    SXINT	pos, nt;

	default:
	    gripe ();

	case 0:
	    /* "<"&Is_First_Col when it should not */
	    bnf_found_bad_beginning_of_rule ();
	    return;

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
	    return;

	case 5:
	    /* $$ */
	    found_text ();
	    has_defining_occurrence = TRUE;
	    fputs ("yyval", sxstdout);
	    put_type (WN [xprod].reduc);
	    break;

	case 6:
	    /* $ {DIGIT}+ */
	case 7:
	    /* $ - {DIGIT}+ */
	    found_text ();
	    fprintf (sxstdout, "yypv[%s]", sxsvar.sxlv_s.token_string + 1);

	    if (pos_ok (pos = atoi (sxsvar.sxlv_s.token_string + 1), FALSE) && nt_ok (nt = WI [WN [xprod].prolon + pos -
		 1].lispro)) {
		put_type (XNT_TO_NT (nt));
	    }

	    break;

	case 8:
	    /* $ TYPE $ */
	    has_defining_occurrence = TRUE;
	    fputs ("yyval", sxstdout);

	    if (xattr >= 0) {
		put_named_type (type_ste);
	    }
	    else {
		sxput_error (sxsvar.lv.terminal_token.source_index, "%sIllegal use of a terminal attribute.", bnf_tables.
		     err_titles [2]);
		is_err = TRUE;
	    }

	    break;

	case 9:
	    /* $ TYPE [-] {DIGIT}+ */
	    fprintf (sxstdout, xattr >= 0 ? "yypv[%s]" : "SXSTACKtoken(SXSTACKnewtop()+(%s-1))", sxsvar.sxlv_s.
		 token_string);

	    if (pos_ok (pos = atoi (sxsvar.sxlv_s.token_string), TRUE)) {
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
	    terminal_token = &sxsvar.lv.terminal_token;
	    xattr = chercher_attr (type_ste = sxstr2save (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth));
	    /* Don't touch the current source coordinates */
	    sxsvar.sxlv.ts_lgth = 0;
	    return;

	case 11:
	    /* BNF_COMMENT */
	    if (has_semantics) {
		putc (NEWLINE, sxstdout);
	    }

	    break;

	case 12:
	    /* <defs> = ; */
	    found_rule ();
	    put_postlude ();
	    return;
	}

	sxsvar.lv.terminal_token.source_index = sxsrcmngr.source_coord;
	sxsvar.sxlv.ts_lgth = 0;
	return;
    }
}
