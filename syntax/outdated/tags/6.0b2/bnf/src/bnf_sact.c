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
   *  Produit de l'equipe Langages et Traducteurs.	  *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 11-02-88 (pb):	Ajout de la post-action @3 pour bnf_symtab	*/
/************************************************************************/
/* 18-05-87 (phd&pb):	Adaptation au bnf nouveau			*/
/************************************************************************/


#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"
char WHAT_BNFSACT[] = "@(#)SYNTAX - $Id: bnf_sact.c 588 2007-05-25 12:32:11Z rlacroix $" WHAT_DEBUG;

#define rule_slice 100

static struct span {
	   unsigned long	head;
	   struct tail {
	       unsigned long	line;
	       unsigned int	column;
	   }	tail;
       }	*coords;
static int	xprod, rules_no;
static int	current_rule_no;



unsigned long	bnf_get_line_no (rule_no)
    int		rule_no;
{
    return rule_no > xprod ? 0 : coords [rule_no].head;
}



SXVOID	bnf_get_rule_tail (rule_no, tline, tcol)
    unsigned long       rule_no, *tline;
    unsigned int	*tcol;
{
    *tline = coords [rule_no].tail.line;
    *tcol = coords [rule_no].tail.column;
}



SXVOID	bnf_skip_rule ()
{
    register struct tail	*tail_coord;
    register unsigned long	line;
    register unsigned int	column;

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



SXVOID	bnf_found_bad_beginning_of_rule ()
{
    struct sxsource_coord	less_coord;

    less_coord = sxsrcmngr.source_coord, less_coord.column = 1;
    sxput_error (less_coord, "%sIllegal occurrence of \"<\"; the head of a new grammar rule is assumed.", sxsvar.sxtables->err_titles [2]);
    sxsrcpush ('\n', "<", less_coord);
}



static VOID	gripe ()
{
    fputs ("\nThe function \"bnf_scan_act\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
}



SXVOID	(*more_scan_act) ();


SXVOID
bnf_scan_act (code, act_no)
    int		code;
    int		act_no;
{
    switch (code) {
    case OPEN:
	coords = (struct span*) sxalloc ((rules_no = rule_slice) + 1, sizeof (struct span));

	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

	return;

    case CLOSE:
	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

	if (coords != NULL) {
	    sxfree (coords);
	}

	return;

    case INIT:
	is_predicate = FALSE;

	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

	{
	    register SHORT	c;

	    c = sxsrcmngr.current_char;

	    while (c != EOF)
		if (c == '<' && sxsrcmngr.source_coord.column == 1) {
		    coords [xprod = 1].head = sxsrcmngr.source_coord.line;
		    return;
		}
		else
		    c = sxnext_char ();
	}

	fprintf (sxstderr, "\n\tThis text is not a grammar... see you later.\n");
	SXEXIT (3);

    case FINAL:
	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

	return;

    case ACTION:
	switch (act_no) {
	    register SHORT	c;

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
			return;
		    }
		    else
			c = sxnext_char ();
	    }

	    return;

	case 2:
	    /* \nnn => char */
	    {
		register int	val;
		register char	bnf_scan_act_c, *s, *t;

		t = s = sxsvar.lv_s.token_string + sxsvar.lv.mark.index;

		for (val = *s++ - '0'; (bnf_scan_act_c = *s++) != NUL; ) {
		    val = (val << 3) + bnf_scan_act_c - '0';
		}

		*t = val;
		sxsvar.lv.ts_lgth = sxsvar.lv.mark.index + 1;
		sxsvar.lv.mark.index = -1;
	    }

	    return;

	case 3:
	    /* Post-action de %PREDICATE : is_predicate est utilise dans bnf_symtab */
	    is_predicate = TRUE;
	    return;
	default:
#if EBUG
	  sxtrap("bnf_sact","unknown switch case #1");
#endif
	  break;
	}

    default:
	gripe ();
    }
}
