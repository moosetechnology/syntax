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
/* 09-08-94 (pb):	Creation					*/
/************************************************************************/

char WHAT_LIG_SACT[] = "@(#)lig_sact.c\t- SYNTAX [unix] - Mar 9 Aou 1994 10:11:16";

#include "sxunix.h"
#include "lig.h"


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

static char	**comments;

unsigned long	lig_get_line_no (int rule_no)
{
    return rule_no > xprod ? 0 : coords [rule_no].head;
}



#define ts_put(c)							\
(    (sxsvar.sxlv.ts_lgth+2 == sxsvar.sxlv_s.ts_lgth_use)			\
	? sxsvar.sxlv_s.token_string = (char *) sxrealloc (sxsvar.sxlv_s.	\
	    token_string, sxsvar.sxlv_s.ts_lgth_use *= 2, sizeof (char))	\
	: NULL,								\
     sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth ++] = c		\
)


#define ts_null()	(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth] = SXNUL)


static void	lig_comment_put (int x, char *str, int lgth)
{
    comments [x] = lgth == 0 ? NULL : strcpy ((char*) sxalloc (lgth + 1, sizeof (char)), str);
}


char *lig_comment_get (int x)
{
    return comments [x];
}

void lig_get_rule_tail (unsigned long rule_no, unsigned long *tline, unsigned int *tcol)
{
    *tline = coords [rule_no].tail.line;
    *tcol = coords [rule_no].tail.column;
}



void	lig_skip_rule ()
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



void	lig_found_bad_beginning_of_rule ()
{
    struct sxsource_coord	less_coord;

    less_coord = sxsrcmngr.source_coord, less_coord.column = 1;
    sxerror (less_coord, 
             sxsvar.sxtables->err_titles [2][0] /* warning */,
             "%sIllegal occurrence of \"<\"; the head of a new grammar rule is assumed.",
             sxsvar.sxtables->err_titles [2]+1 /* warning */);
    sxsrcpush ('\n', "<", less_coord);
}



static void	gripe ()
{
    fputs ("\nThe function \"sxscanner_action\" for lig is out of date with respect to its specification.\n", sxstderr);
    abort ();
}



void	(*more_scan_act) (SXINT, SXINT);



bool sxscanner_action (SXINT code, SXINT act_no)
{
    switch (code) {
    case SXOPEN:
	coords = (struct span*) sxalloc ((rules_no = rule_slice) + 1, sizeof (struct span));
	comments = (char **) sxalloc (rules_no + 1, sizeof (char*));

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
	    sxfree (comments);
	}

	return SXANY_BOOL;

    case SXINIT:
	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

        {
	    register short	c;

	    c = sxsrcmngr.current_char;

	    while (c != EOF)
	    {
		if (c == '<' && sxsrcmngr.source_coord.column == 1) {
		    coords [xprod = 1].head = sxsrcmngr.source_coord.line;

		    ts_null ();
		    lig_comment_put (xprod - 1, sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);

		    return SXANY_BOOL;
		}
		else {
		    ts_put (c);
		    c = sxnext_char ();
		}
	    }
	}

	fprintf (sxstderr, "\n\tThis text is not a linear indexed grammar... see you later.\n");
	sxexit (3);

    case SXFINAL:
	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

	return SXANY_BOOL;

    case SXACTION:
	switch (act_no) {

	case 1:
	    {
	        register short	c;
		coords [xprod].tail.line = sxsrcmngr.source_coord.line;
		coords [xprod].tail.column = sxsrcmngr.source_coord.column;

		if (more_scan_act != NULL) {
		    (*more_scan_act) (code, act_no);
		}

		c = sxsrcmngr.current_char;

		while (c != EOF)
		{
		    if (c == '<' && sxsrcmngr.source_coord.column == 1) {
			if (++xprod > rules_no) {
			    coords = (struct span*) sxrealloc (coords,
							       (rules_no += rule_slice) + 1,
							       sizeof (struct span));
			    comments = (char**) sxrealloc (comments, rules_no + 1, sizeof (char*));
			}

			coords [xprod].head = sxsrcmngr.source_coord.line;

			break;
		    }
		    else {
			ts_put (c);
			c = sxnext_char ();
		    }
		}

		ts_null ();
		lig_comment_put (xprod - (c != EOF ? 1 : 0),
				 sxsvar.sxlv_s.token_string,
				 sxsvar.sxlv.ts_lgth);
	    }

	    return SXANY_BOOL;

	case 2:
	    /* \nnn => char */
	    {
		register int	val;
		register char	c, *s, *t;

		t = s = sxsvar.sxlv_s.token_string + sxsvar.sxlv.mark.index;

		for (val = *s++ - '0'; (c = *s++) != SXNUL; ) {
		    val = (val << 3) + c - '0';
		}

		*t = val;
		sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index + 1;
		sxsvar.sxlv.mark.index = -1;
	    }

	    return SXANY_BOOL;

            default:
               /* default case unspecified in the original code */
               /* we continue to gripe() */ ;
	}
        /* FALLTHROUGH */

    default:
	gripe ();
        /* NOTREACHED */
        return SXANY_BOOL;
    }
}
