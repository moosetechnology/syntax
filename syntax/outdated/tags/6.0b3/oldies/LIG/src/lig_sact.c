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

#define WHAT	"@(#)lig_sact.c\t- SYNTAX [unix] - Mar 9 Aou 1994 10:11:16"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};


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

unsigned long	lig_get_line_no (rule_no)
    int		rule_no;
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


#define ts_null()	(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth] = NUL)


static SXVOID	lig_comment_put (x, str, lgth)
    int		x;
    char	*str;
    int		lgth;
{
    comments [x] = lgth == 0 ? NULL : strcpy ((char*) sxalloc (lgth + 1, sizeof (char)), str);
}


char *lig_comment_get (x)
    int x;
{
    return comments [x];
}

SXVOID	lig_get_rule_tail (rule_no, tline, tcol)
    unsigned long       rule_no, *tline;
    unsigned int	*tcol;
{
    *tline = coords [rule_no].tail.line;
    *tcol = coords [rule_no].tail.column;
}



SXVOID	lig_skip_rule ()
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



SXVOID	lig_found_bad_beginning_of_rule ()
{
    struct sxsource_coord	less_coord;

    less_coord = sxsrcmngr.source_coord, less_coord.column = 1;
    sxput_error (less_coord, "%sIllegal occurrence of \"<\"; the head of a new grammar rule is assumed.", sxsvar.sxtables
	 ->err_titles [2]);
    sxsrcpush ('\n', "<", less_coord);
}



static VOID	gripe ()
{
    fputs ("\nThe function \"lig_scan_act\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
}



SXVOID	(*more_scan_act) ();



lig_scan_act (code, act_no)
    int		code;
    int		act_no;
{
    switch (code) {
    case OPEN:
	coords = (struct span*) sxalloc ((rules_no = rule_slice) + 1, sizeof (struct span));
	comments = (char **) sxalloc (rules_no + 1, sizeof (char*));

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
	    sxfree (comments);
	}

	return;

    case INIT:
	if (more_scan_act != NULL) {
	    (*more_scan_act) (code, act_no);
	}

        {
	    register SHORT	c;

	    c = sxsrcmngr.current_char;

	    while (c != EOF)
	    {
		if (c == '<' && sxsrcmngr.source_coord.column == 1) {
		    coords [xprod = 1].head = sxsrcmngr.source_coord.line;

		    ts_null ();
		    lig_comment_put (xprod - 1, sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);

		    return;
		}
		else {
		    ts_put (c);
		    c = sxnext_char ();
		}
	    }
	}

	fprintf (sxstderr, "\n\tThis text is not a linear indexed grammar... see you later.\n");
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
		{
		    if (c == '<' && sxsrcmngr.source_coord.column == 1) {
			if (++xprod > rules_no) {
			    coords = (struct span*) sxrealloc (coords,
							       (rules_no += rule_slice) + 1,
							       sizeof (struct span));
			    comments = (char**) realloc (comments, rules_no + 1, sizeof (char*));
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

	    return;

	case 2:
	    /* \nnn => char */
	    {
		register int	val;
		register char	c, *s, *t;

		t = s = sxsvar.lv_s.token_string + sxsvar.lv.mark.index;

		for (val = *s++ - '0'; (c = *s++) != NUL; ) {
		    val = (val << 3) + c - '0';
		}

		*t = val;
		sxsvar.lv.ts_lgth = sxsvar.lv.mark.index + 1;
		sxsvar.lv.mark.index = -1;
	    }

	    return;

	}

    default:
	gripe ();
    }
}
