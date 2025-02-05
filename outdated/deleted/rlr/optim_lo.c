/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                        (PB)                          *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* Lun 25 Nov 1996 10:46 (pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 19-08-94 14:34 (pb):		Les "codes" des sequences non		*/
/*				deterministes sont superieures a 20000	*/
/*				(et non plus a 10000)			*/
/************************************************************************/
/* 05-08-94 10:51 (pb):		Modif xt_to_str				*/
/************************************************************************/
/* 31-03-93 10:50 (pb):		Ajout de prdct_list dans le cas non	*/
/*				deterministe et predicats		*/
/************************************************************************/
/* 30-04-92 18:07 (pb):		Nouveau codop NonDeter			*/
/************************************************************************/
/* 13-04-92 17:42 (pb):		Modif ds soritem pour placer le "="	*/
/************************************************************************/
/* 24-08-88 18:53 (bl):		Ajout de #ifdef MSDOS			*/
/************************************************************************/
/* 15-02-88 11:00 (pb):		Adaptation a la V3.2 (XNT)		*/
/************************************************************************/
/* 16-10-87 10:50 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT_OPTIM_LO[] = "@(#)OPTIM_LO.c - SYNTAX [unix] - Lundi 25 Novembre 1996";

#include "P_tables.h"

#include "rlr_optim.h"


#include "bstr.h"

extern struct bstr	*options_text ();
extern void		sxtime ();

static int	*xt_to_ate;
static VARSTR	wvstr;
static char	small_string [12];


static void alloc_alpha_table ()
{
    xt_to_ate = (int *) sxcalloc (bnf_ag.WS_TBL_SIZE.xtmax + 1, sizeof (int));
    sxstr_mngr (SXBEGIN);
    wvstr = varstr_alloc (32);
}


void free_alpha_table ()
{
    if (xt_to_ate != NULL) {
	sxfree (xt_to_ate), xt_to_ate = NULL;
	sxstr_mngr (SXEND);
	varstr_free (wvstr);
    }
}





char	*xt_to_str (code)
    int		code;
{
    /* code est negatif */
    register int	i, ate, prdct;

    if (xt_to_ate == NULL) {
	alloc_alpha_table ();
    }

    if ((ate = xt_to_ate [-code]) == 0) {
	prdct = XT_TO_PRDCT_CODE (-code);
	code = -XT_TO_T_CODE (-code);

	varstr_raz (wvstr);

	if (SXBA_bit_is_set (bnf_ag.IS_A_GENERIC_TERMINAL, -code)) {
	    varstr_catenate (wvstr, get_t_string (code));
	}
	else {
	    varstr_quote (wvstr, get_t_string (code));
	}

	if (prdct >= 0) {
	    sprintf (small_string, " &%d", prdct);
	    varstr_catenate (wvstr, small_string);
	}

	ate = sxstrsave (varstr_tostr (wvstr));
    }

    return sxstrget (ate);
}


VARSTR	cat_t_string (vstr, code)
    VARSTR	vstr;
    int		code;
{
    return varstr_catenate (vstr, xt_to_str (code));
}



static char	*t_to_str (code)
    int		code;
{
    if (code < 0 && code >= -bnf_ag.WS_TBL_SIZE.xtmax)
	return xt_to_str (code);

    return "Any";
}



static char	*nt_to_str (code)
    int		code;
{
    if (code > 0 && code <= bnf_ag.WS_TBL_SIZE.xntmax)
	return get_nt_string (code);

    return "Any";
}



static char	*prdct_to_str (code)
    int		code;
{
    if (code >= 0) {
	sprintf (small_string, "&%d", code);
	return small_string;
    }

    return "&Any";
}



void	soritem (x, d, f, c1, s2)
    int		x, d, f;
    char	*c1, *s2;
{
    register char	car;
    register int	i, c;
    int		noprod, addeb, adfin, xegal, j, l;
    char	tail [64], *t, *t_string;
    bool	is_first_time;

    noprod = bnf_ag.WS_INDPRO [x].prolis;
    addeb = bnf_ag.WS_NBPRO [noprod].prolon;
    adfin = bnf_ag.WS_NBPRO [noprod + 1].prolon - 2;

    if (*c1 != ' ') {
	put_edit_nnl (d, c1);
	c = d + 1;
    }
    else
	c = d;

    put_edit_fnb (c, 4, noprod);

    if ((i = PROD_TO_RED (noprod)) != noprod) {
	put_edit_apnnl ("(");
	put_edit_apnb (i);
	put_edit_apnnl (")");
    }

    put_edit_apnnl (":");
    c = put_edit_get_col () + 1;

    if (noprod > 0) {
	int	nt;

	nt = bnf_ag.WS_NBPRO [noprod].reduc;
	put_edit_nnl (c, get_nt_string (nt));
	c += get_nt_length (nt) + 1;
    }

    if (c > 24)  {
	put_edit_nnl (f, c1);

	if (*s2 != SXNUL) {
	    t = tail;

	    while ((car = *s2++) != SXNUL && car != '\n') {
		*t++ = car;
	    }

	    if (car == SXNUL) {
		s2--;
	    }

	    *t = SXNUL;
	    put_edit_nnl (f + 1, tail);
	}

	put_edit_nl (1);
	put_edit_nnl (d, c1);
    }

    c = 24;
    put_edit_nnl (c, "=");
    xegal = c += 2;

    for (i = addeb; i <= adfin; i++) {
	if (i == x) {
	    put_edit_nnl (c, ".");
	    c += 2;
	}

	j = bnf_ag.WS_INDPRO [i].lispro;
	l = ((j > 0) ? get_nt_length (j) : strlen (t_string = xt_to_str (j))) + 1;
	c += l;

	if (c >= f) {
	    put_edit_nnl (f, c1);

	    if (*s2 != SXNUL) {
		t = tail;

		while ((car = *s2++) != SXNUL && car != '\n') {
		    *t++ = car;
		}

		if (car == SXNUL) {
		    s2--;
		}

		*t = SXNUL;
		put_edit_nnl (f + 1, tail);
	    }

	    put_edit_nl (1);
	    put_edit_nnl (d, c1);
	    c = xegal + l;
	}

	put_edit_nnl (c - l, (j > 0) ? get_nt_string (j) : t_string);
    }

    if (x > adfin)
	put_edit_nnl (c, ".");

    if (*c1 != SXNUL)
	put_edit_nnl (f, c1);

    is_first_time = true;

    while (*s2 != SXNUL) {
	if (is_first_time) {
	    is_first_time = false;
	}
	else if (*c1 != SXNUL) {
	    put_edit_nnl (d, c1);
	    put_edit_nnl (f, c1);
	}

	t = tail;

	while ((car = *s2++) != SXNUL && car != '\n') {
	    *t++ = car;
	}

	if (car == SXNUL) {
	    s2--;
	}

	*t = SXNUL;
	put_edit_nnl (f + 1, tail);
    }

    put_edit_nl (1); /* end soritem */
}



char	*laset (varstring, maxi, look_ahead_set)
    VARSTR	varstring;
    int		maxi;
    SXBA	look_ahead_set;
{
    register int	t, col, l;
    bool		is_first = true;
    register char	*t_string;

    varstr_raz (varstring);
    varstr_catenate (varstring, " {");
    col = 3;
    t = 0;

    while ((t = sxba_scan (look_ahead_set, t)) > 0) {
	if (is_first)
	    is_first = false;
	else {
	    varstr_catenate (varstring, ", ");
	    col += 2;
	}

	t_string = xt_to_str (-t);
	col += (l = strlen (t_string));

	if (col > maxi) {
	    varstr_catenate (varstring, "\n ");
	    col = l + 2;
	}

	varstr_catenate (varstring, t_string);
    }

    return varstr_tostr (varstr_catenate (varstring, "}"));
}



VARSTR	prod_to_str (vstr, prod)
    VARSTR	vstr;
    int		prod;
{
    register int	i, j, adfin, addeb;

    addeb = bnf_ag.WS_NBPRO [prod].prolon;
    adfin = bnf_ag.WS_NBPRO [prod + 1].prolon - 2;
    sprintf (small_string, "%-d :\t", prod);
    varstr_catenate (varstr_catenate (varstr_catenate (varstr_raz (vstr), small_string), get_nt_string (bnf_ag.WS_NBPRO [prod].
	 reduc)), " = ");

    for (i = addeb; i <= adfin; i++) {
	((j = bnf_ag.WS_INDPRO [i].lispro) > 0) ? varstr_catenate (vstr, get_nt_string (j)) : cat_t_string (vstr, j);
	varstr_catenate (vstr, " ");
    }

    return varstr_catenate (vstr, ";\n");
}



bool		open_listing (kind, language_name, ME, component_name)
    int		kind;
    char	*language_name, *ME, *component_name;
{
    struct bstr	*bstr;
#include <time.h>

    if (is_listing_opened)
	return true;

    {
	char	listing_name [32];

	if ((listing = fopen (strcat (strcpy (listing_name, language_name), component_name), "w")) == NULL) {
	    if (sxverbosep && !sxttycol1p) {
		fputc ('\n', sxtty);
		sxttycol1p = true;
	    }

	    fprintf (sxstderr, "%s: cannot open (create) ", ME);
	    sxperror (listing_name);
	    return false;
	}
    }

    is_listing_opened = true;
    put_edit_initialize (listing);
    put_edit_nnl (9, "Listing of:");
    put_edit (25, language_name);
    put_edit_nnl (9, "By:");
    put_edit (25, ME);
    put_edit (25, release_mess);
    put_edit_nnl (9, "Options:");
    bstr = bstr_alloc (128, 25, 79);
    put_edit (25, bstr_tostr (options_text (kind, bstr)));
    bstr_free (bstr);
    put_edit_nnl (9, "Generated on:");

    {
	long	date_time;

	date_time = time (0);
	put_edit (25, ctime (&date_time));
    }

    put_edit_nl (3);

    if (xt_to_ate == NULL) {
	alloc_alpha_table ();
    }

    return true;
}



void	output_fe (kind, xa, test, afe)
    int		kind, xa, test;
    register struct floyd_evans		*afe;
{
    int		i;

    put_edit_nb (1, xa);
    put_edit_nnl (5, ":");

    switch (kind) {
    case Terminal:
	put_edit_nnl (11, t_to_str (-test));
	break;

    case NonTerminal:
	put_edit_nnl (11, nt_to_str (test));
	break;

    case Predicate:
	put_edit_nnl (11, prdct_to_str (test));
	break;

    case NonDeter:
	break;
    }

    switch (afe->codop) {
    case LaScanShift:
	put_edit_nnl (30, "*");

    case ScanShift:
    case ScanReduce:
	put_edit_nnl (31, "*");

    default:
	break;
    }

    switch (afe->codop) {
    case ScanReduce:
    case Reduce:
	if (afe->reduce != 0)
	    put_edit_fnb (40, 5, afe->reduce);

    default:
	break;
    }

    switch (afe->codop) {
    case ScanReduce:
    case Reduce:
	if (afe->reduce != 0) {
	    put_edit_nnl (51, "");

	    for (i = afe->pspl; i > 0; i--)
		put_edit_apnnl ("|");
	}

	break;

    case ScanShift:
    case Shift:
	i = nt_state_equiv_class [afe->pspl];

	if (i > 0)
	    put_edit_nnl (60, "(");

	put_edit_nb (61, (i < 0) ? -i : i);

	if (i > 0)
	    put_edit_apnnl (")");

    default:
	break;
    }

    switch (afe->codop) {
    case ScanReduce:
    case Reduce:
	if (afe->reduce != 0) {
	    i = nt_to_ad [afe->next];

	    if (i <= 0) {
		put_edit_nnl (71, "Goto Branch_Table (");
		put_edit_apnnl (get_nt_string (nt_to_nt [afe->next]));
		put_edit_ap (", Top_Stack_State)");
	    }
	    else {
		put_edit_nnl (71, "Goto ");
		put_edit_apnb (i);
		put_edit_nl (1);
	    }
	}
	else
	    put_edit (71, "Halt");

	break;

    case LaScanShift:
    case ScanShift:
    case Shift:
	put_edit_nnl (71, "Goto Etq ");
	put_edit_apnb (t_state_equiv_class [afe->next]);
	put_edit_nl (1);
	break;

    case Prdct:
	put_edit_nnl (71, "Goto Prdct ");
	put_edit_apnb (afe->next);
	put_edit_nl (1);
	break;

    case NonDeter:
	put_edit_nnl (71, "Goto NonDeter ");
	put_edit_apnb (afe->next);
	put_edit_nl (1);
	break;

    case Error:
	put_edit (71, "Error");
	break;

    case ForceError:
	put_edit (71, "Force-Error");
	break;

    default:
	break;
    }
}




/*   L I S T I N G   O U T P U T   */

void	optim_lo (language_name, ME)
    char	*language_name, *ME;
{
    static char		hyph60 [] = "-------------------------------------------------------------";
    static char		dot46 [] = ".............................................";
    register int	i, x, y, l;
    register struct P_item	*aitem, *lim, *avector;
    register struct P_base	*abase;

    if (sxverbosep) {
	fprintf (sxtty, "\tListing Output ... ");
	sxttycol1p = false;
    }

    open_listing (2, language_name, ME, ".op.l");
    stats.nt.c.cols = 0;

    for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xntmax; i++) {
	if (nt_to_nt [i] == i)
	    stats.nt.c.cols++;
    }

    put_edit_nl (1);
    put_edit (21, "I N I T I A L   V A L U E S");
    put_edit_nl (1);
    fprintf (listing, "\t\t\tlines\tcolumns\tcells\n");
    fprintf (listing, "\tT_tables:\t%d\t%d\t%d\n", stats.t.i.lines, stats.t.i.cols, x = stats.t.i.lines * stats.t.i.cols)
	 ;
    fprintf (listing, "\tNT_tables:\t%d\t%d\t%d\n", stats.nt.i.lines, stats.nt.i.cols, y = stats.nt.i.lines * stats.nt.i.
	 cols);
    put_edit_nl (3);
    put_edit (21, "F S A   R E D U C T I O N");
    put_edit_nl (1);
    fprintf (listing, "\t\t\tlines\tcolumns\tcells\tgain (%%)\n");
    i = stats.t.r.lines * stats.t.i.cols;
    fprintf (listing, "\tT_tables:\t%d\t%d\t%d\t%d\n", stats.t.r.lines, stats.t.i.cols, i, 100 * (x - i) / x);
    i = stats.nt.r.lines * stats.nt.i.cols;
    fprintf (listing, "\tNT_tables:\t%d\t%d\t%d\t%d\n", stats.nt.r.lines, stats.nt.i.cols, i, 100 * (y - i) / y);
    put_edit_nl (3);
    put_edit (21, "N T _ T A B L E S   C O M P A C T I O N");
    put_edit_nl (1);
    *nt_to_ad = 0 /* nombre de branchements directs */ ;

    for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xnbpro; i++) {
	if (!SXBA_bit_is_set (single_prod, i) && nt_to_ad [bnf_ag.WS_NBPRO [i].reduc] != -1)
	    ++*nt_to_ad;
    }

    fprintf (listing, "\t\t\tlines\tcolumns\tcells\tgain (%%)\tdirect branching\n");
    i = stats.nt.c.lines * stats.nt.c.cols;
    fprintf (listing, "\t\t\t%d\t%d\t%d\t%d\t\t%d over %d\n", stats.nt.c.lines, stats.nt.c.cols, i, 100 * (y - i) / y, *
	 nt_to_ad, bnf_ag.WS_TBL_SIZE.xnbpro);
    put_edit_nl (3);

    if ((x = sxba_cardinal (single_prod)) != 0) {
	put_edit (21, "S I N G L E    P R O D U C T I O N S   E L I M I N A T I O N");
	put_edit_nl (3);
	y = 0;

	for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xnbpro; i++) {
	    if (bnf_ag.WS_NBPRO [i].bprosimpl)
		y++;
	}

	put_edit_nnl (11, "Production number (a)");
	put_edit_nnl (51, "|");
	put_edit_fnb (53, 4, bnf_ag.WS_TBL_SIZE.xnbpro);
	put_edit_nl (1);
	put_edit (11, hyph60 + 13);
	put_edit_nnl (11, "Single production number (b)");
	put_edit_nnl (51, "|");
	put_edit_fnb (53, 4, y);
	put_edit_nl (1);
	put_edit_nnl (11, "   (w.r.t. semantics)");
	put_edit_nnl (51, "|");
	put_edit (11, hyph60 + 13);
	put_edit_nnl (11, "Single production eliminated (c)");
	put_edit_nnl (51, "|");
	put_edit_fnb (53, 4, x);
	put_edit_nl (1);
	put_edit_nnl (11, "     (cf list below)");
	put_edit (51, "|");
	put_edit_nl (2);
	put_edit (21, "|   b/a   |   c/b   |   c/a");
	put_edit (11, hyph60 + 20);
	put_edit_nnl (11, "Ratio (%)");
	put_edit_nnl (21, "|");
	put_edit_fnb (25, 3, 100 * y / bnf_ag.WS_TBL_SIZE.xnbpro);
	put_edit_nnl (31, "|");
	put_edit_fnb (35, 3, 100 * x / y);
	put_edit_nnl (41, "|");
	put_edit_fnb (45, 3, 100 * x / bnf_ag.WS_TBL_SIZE.xnbpro);
	put_edit_nl (3);
	i = 0;

	while ((i = sxba_scan (single_prod, i)) > 0) {
	    put_edit_fnb (11, 4, i);
	    put_edit_nnl (16, ":    ");
	    put_edit_apnnl (get_nt_string (bnf_ag.WS_NBPRO [i].reduc));
	    put_edit_nnl (51, "= ");
	    put_edit_apnnl (get_nt_string (bnf_ag.WS_INDPRO [bnf_ag.WS_NBPRO [i].prolon].lispro));
	    put_edit_ap (" ;");
	}

	put_edit_nl (3);
    }

    put_edit (21, "M A T R I X   L I N E A R I Z A T I O N");
    put_edit_nl (1);
    fprintf (listing, "\t\t\tbase\tvector\tcells\tgain (%%)\n");
    x = stats.t.i.lines * stats.t.i.cols;
    y = stats.t.r.lines * 3 + t_vector_size * 2;
    fprintf (listing, "\tT_tables:\t%d\t%d\t%d\t%d\n", stats.t.r.lines, t_vector_size, y, 100 * (x - y) / x);
    x = stats.nt.i.lines * stats.nt.i.cols;
    y = stats.nt.c.lines * 3 + nt_vector_size * 2;
    fprintf (listing, "\tNT_tables:\t%d\t%d\t%d\t%d\n", stats.nt.c.lines, nt_vector_size, y, 100 * (x - y) / x);

    if (is_floyd_evans || is_long_listing) {
	put_edit_nl (2);
	put_edit_nnl (11, "Floyd-Evans size = ");
	put_edit_apnb (tnt_lnks [0]);
	put_edit_nl (2);
	put_edit_ap ("************ T _ T A B L E S *************");
	put_edit_nl (2);
	put_edit_nnl (11, "Parsing starts in Etq ");
	put_edit_apnb (init_state);
	put_edit_nl (2);

	for (i = 1; i < xe1; i++) {
	    abase = t_bases + i;

	    if (abase->defaut != 0 /* etat non reduit */ ) {
		put_edit_nl (1);
		put_edit_nnl (1, "Etq ");
		put_edit_apnb (i);

		if ((x = abase->commun) >= grand) {
		    if ((x -= grand) > 0)
			output_fe (Terminal, x, abase->propre - grand, tnt_fe + x);
		}
		else {
		    x += delta;
		    y = (abase->propre == grand) ? 0 : abase->propre + delta;

		    for (l = 1; l <= -bnf_ag.WS_TBL_SIZE.tmax; l++) {
			if (x != 0 && (avector = vector + x + l)->check == l) {
			    output_fe (Terminal, avector->action, l, tnt_fe + avector->action);
			}

			if (y != 0 && (avector = vector + y + l)->check == l) {
			    output_fe (Terminal, avector->action, l, tnt_fe + avector->action);
			}
		    }
		}

		output_fe (Terminal, abase->defaut, 0, tnt_fe + abase->defaut);
	    }
	}

	if (is_long_listing) {
	    put_edit_nl (1);
	    put_edit_ap ("*********** D E F A U L T - O W N - S H A R E ***************");
	    put_edit_nl (2);
	    put_edit_nnl (11, "I");
	    put_edit_nnl (21, "DEFAULT");
	    put_edit_nnl (31, "OWN");
	    put_edit (61, "SHARE");

	    for (i = xe1 - 1; i > 0; i--) {
		register struct P_base	*abase = t_bases + i;

		put_edit_nb (11, i);

		if (abase->defaut != 0) {
		    put_edit_nb (21, abase->defaut);

		    if ((x = abase->propre) >= grand) {
			if ((x -= grand) > 0)
			    put_edit_nnl (31, t_to_str (-x));
			else
			    put_edit_nnl (31, ".");
		    }
		    else
			put_edit_nb (31, x + delta);

		    if ((x = abase->commun) >= grand) {
			if ((x -= grand) > 0)
			    put_edit_nb (61, x);
			else
			    put_edit_nnl (61, ".");
		    }
		    else
			put_edit_nb (61, x + delta);
		}
		else
		    put_edit_nnl (21, dot46);
	    }

	    put_edit_nl (1);
	}

	put_edit_nl (1);
	put_edit_ap ("************ N T _ T A B L E S *************");
	put_edit_nl (2);

	for (i = 1; i < xac2; i++) {
	    abase = nt_bases + i;

	    if (abase->defaut != 0 /* etat non reduit */ ) {
		put_edit_nl (1);
		put_edit_nnl (1, "State ");
		put_edit_apnb (i);

		if ((x = abase->commun) >= grand) {
		    if ((x -= grand) > 0)
			output_fe (NonTerminal, x, abase->propre - grand, tnt_fe + x);
		}
		else {
		    x += delta;
		    y = (abase->propre == grand) ? 0 : abase->propre + delta;

		    for (l = 1; l <= NTMAX; l++) {
			if (x != 0 && (avector = vector + x + l)->check == l) {
			    output_fe (NonTerminal, avector->action, l, tnt_fe + avector->action);
			}

			if (y != 0 && (avector = vector + y + l)->check == l) {
			    output_fe (NonTerminal, avector->action, l, tnt_fe + avector->action);
			}
		    }
		}

		output_fe (NonTerminal, abase->defaut, 0, tnt_fe + abase->defaut);
	    }
	}

	put_edit_nl (1);

	for (i = 1; i <= nt_fe_size; i++) {
	    output_fe (NonTerminal, i, 0, tnt_fe + i);
	}

	if (is_long_listing) {
	    put_edit_nl (1);
	    put_edit_ap ("*********** D E F A U L T - O W N - S H A R E ***************");
	    put_edit_nl (2);
	    put_edit_nnl (11, "I");
	    put_edit_nnl (21, "DEFAULT");
	    put_edit_nnl (31, "OWN");
	    put_edit (61, "SHARE");

	    for (i = xac2 - 1; i > 0; i--) {
		register struct P_base	*abase = nt_bases + i;

		put_edit_nb (11, i);

		if (abase->defaut != 0) {
		    put_edit_nb (21, abase->defaut);

		    if ((x = abase->propre) >= grand) {
			if ((x -= grand) > 0)
			    put_edit_nnl (31, get_nt_string (x));
			else
			    put_edit_nnl (31, ".");
		    }
		    else
			put_edit_nb (31, x + delta);

		    if ((x = abase->commun) >= grand) {
			if ((x -= grand) > 0)
			    put_edit_nb (61, x);
			else
			    put_edit_nnl (61, ".");
		    }
		    else
			put_edit_nb (61, x + delta);
		}
		else
		    put_edit_nnl (21, dot46);
	    }

	    put_edit_nl (1);
	}

	if (xpredicates > 0) {
	    register struct P_prdct	*aitem, *lim;
	    register struct state	*astate;

	    put_edit_nl (1);
	    put_edit_ap ("************ P R D C T _ T A B L E S *************");
	    put_edit_nl (2);

	    for (i = 1; i <= xpredicates; i++) {
		astate = predicates + i;

		if (astate->lgth != 0 /* etat non vide */ ) {
		    aitem = prdcts + astate->start;
		    put_edit_nl (1);
		    put_edit_nnl (1, aitem->prdct >= 20000 ? "NonDeter " : "Prdct ");
		    put_edit_apnb (i);

		    for (lim = aitem + astate->lgth; aitem < lim; aitem++) {
			output_fe (aitem->prdct >= 20000 ? NonDeter : Predicate, aitem->action,
				   aitem->prdct, tnt_fe + aitem->action);
		    }
		}
	    }

	    put_edit_nl (1);
	}

	if (is_long_listing) {
	    put_edit_nl (1);
	    put_edit_ap ("*********** C H E C K - A C T I O N ***************");
	    put_edit_nl (2);
	    put_edit_nnl (11, "I");
	    put_edit_nnl (21, "CHECK");
	    put_edit_nnl (61, "ACTION");

	    for (i = MAX; i >= MIN; i--) {
		put_edit_nb (11, i);

		if ((x = vector [i].action) != 0) {
		    if ((unsigned) (y = i - t_delta - MIN) < t_vector_size && SXBA_bit_is_set (t_check_set, y + 1))
			put_edit_nnl (21, t_to_str (-vector [i].check));
		    else
			put_edit_nnl (21, get_nt_string (vector [i].check));

		    put_edit_nb (61, x);
		}
		else {
		    put_edit_nnl (21, dot46);
		}
	    }

	    put_edit_nl (1);
	}
    }

    if (sxverbosep) {
	sxtime (SXFINAL, "done");
	sxttycol1p = true;
    }
}



void	optim_ll ()
{
    register int	i;

    put_edit_nl (1);
    put_edit (21, "********** T H E   A C T U A L   T A B L E S **********");
    put_edit_nl (3);
    put_edit_nnl (11, "deltavec = ");
    put_edit_apnb (deltavec);
    put_edit_nl (1);
    put_edit_ap ("\tmMrednt\tMred\tMprdct\tMfe\tM0ref\tMref");
    fprintf (listing, "\t%d\t%d\t%d\t%d\t%d\t%d\n", mMrednt, Mred, Mprdct, Mfe, M0ref, Mref);
    put_edit_nl (3);
    put_edit_ap ("********** T _ B A S E S **********");
    put_edit_nl (1);
    put_edit_nnl (11, "old_state");
    put_edit (21, "eq_state");

    for (i = xe1 - 1; i >= 1; i--) {
	put_edit_nb (11, i);
	put_edit_nb (21, t_state_equiv_class [i]);
    }

    put_edit_nl (1);
    put_edit_nnl (11, "new_state");
    put_edit_nnl (21, "eq_state");
    put_edit_nnl (31, "default");
    put_edit_nnl (41, "own");
    put_edit (51, "share");

    for (i = Mtstate; i >= 1; i--) {
	put_edit_nb (11, i);
	put_edit_nb (21, XxY_Y (ref_to_ee, i));
	put_edit_nb (31, t_bases [i].defaut);
	put_edit_nb (41, t_bases [i].propre);
	put_edit_nb (51, t_bases [i].commun);
    }

    put_edit_nl (3);
    put_edit_ap ("********** N T _ B A S E S **********");
    put_edit_nl (1);
    put_edit_nnl (11, "old_state");
    put_edit (21, "eq_state");

    for (i = xac2 - 1; i >= 1; i--) {
	put_edit_nb (11, i);
	put_edit_nb (21, nt_state_equiv_class [i]);
    }

    put_edit_nl (1);
    put_edit_nnl (11, "new_state");
    put_edit_nnl (21, "eq_state");
    put_edit_nnl (31, "default");
    put_edit_nnl (41, "own");
    put_edit (51, "share");

    for (i = Mntstate; i >= 1; i--) {
	put_edit_nb (11, i);
	put_edit_nb (21, XxY_X (ref_to_ee, i));
	put_edit_nb (31, nt_bases [i].defaut);
	put_edit_nb (41, nt_bases [i].propre);
	put_edit_nb (51, nt_bases [i].commun);
    }

    put_edit_nl (3);
    put_edit_ap ("********** V E C T O R **********");
    put_edit_nl (1);
    put_edit_nnl (11, "X");
    put_edit_nnl (21, "check");
    put_edit (31, "action");

    for (i = Mvec; i >= 1; i--) {
	put_edit_nb (11, deltavec + i);
	put_edit_nb (21, vector [i].check);
	put_edit_nb (31, vector [i].action);
    }

    if (xprdct_items > 0) {
	put_edit_nl (3);
	put_edit_ap ("************ P R E D I C A T E S *************");
	put_edit_nl (1);
	put_edit_nnl (11, "X");
	put_edit_nnl (21, "prdct");
	put_edit (31, "action");

	for (i = 1; i <= xprdct_items; i++) {
	    put_edit_nb (11, -(Mred + i));
	    put_edit_nb (21, prdcts [i].prdct);
	    put_edit_nb (31, prdcts [i].action);
	}
    }

    if (Mgerme > mgerme) {
	put_edit_nl (3);
	put_edit_ap ("************ G E R M E _ T O _ G R _ A C T *************");
	put_edit_nl (1);
	put_edit_nnl (11, "state");
	put_edit_nnl (21, "ref");

	for (i = Mgerme; i > mgerme; i--) {
	    put_edit_nb (11, i);
	    put_edit_nb (21, germe_to_gr_act [i - mgerme - 1]);
	}
    }

    put_edit_nl (3);
    put_edit_ap ("********** R E D U C T I O N S **********");
    put_edit_nl (1);
    put_edit_nnl (11, "X");
    put_edit_nnl (21, "action");
    put_edit_nnl (31, "reduce");
    put_edit_nnl (41, "lgth");
    put_edit (51, "lhs");

    for (i = Mred; i >= 1; i--) {
	put_edit_nb (11, i);
	put_edit_nb (21, reductions [i].action);
	put_edit_nb (31, reductions [i].reduce);
	put_edit_nb (41, reductions [i].lgth);
	put_edit_nb (51, reductions [i].lhs);
    }

    if (prdct_list_top > 0) {
	put_edit_nl (3);
	put_edit_ap ("********** P R D C T _ L I S T **********");
	put_edit_nl (1);
	put_edit_nnl (11, "X");
	put_edit_nnl (21, "red_no");
	put_edit_nnl (31, "pos");
	put_edit (41, "prdct");
	
	for (i = prdct_list_top; i >= 1; i--) {
	    put_edit_nb (11, i);
	    put_edit_nb (21, prdct_list [i].red_no);
	    put_edit_nb (31, prdct_list [i].pos);
	    put_edit_nb (41, prdct_list [i].prdct);
	}
	
    }

    put_edit_nl (1);
}


void	optim_sizes ()
{
    put_edit_nl (3);
    put_edit (21, "********** T H E   A C T U A L   S I Z E S **********");
    put_edit_nl (2);
    put_edit_ap ("tables_size = (Mtstate + Mntstate)*3 + Mvec*2 + Mred*4 + Mprdct*2 + Mgerme + Mprdct_list*3");
    fprintf (listing, "tables_size = (%d + %d)*3 + %d*2 + %d*4 + %d*2 + %d + %d*3 = %d\n", Mtstate, Mntstate, Mvec, Mred, xprdct_items, Mgerme - mgerme, prdct_list_top, (
	 Mtstate + Mntstate) * 3 + Mvec * 2 + Mred * 4 + xprdct_items * 2
	     + Mgerme - mgerme + prdct_list_top * 3);
    put_edit_nl (1);
}
