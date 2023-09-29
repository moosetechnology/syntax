
/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from bnf_print.pl1      */
/*  on Wednesday the eigthteenth of June, 1986, at 9:11:33,  */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3g PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040210 10:59 (phd):	Modif pour calmer le compilo HP		*/
/************************************************************************/
/* 20030512 13:07 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 20010326 (pb&phd):		Fusion 3.9 + 4.1			*/
/************************************************************************/
/* 07-07-94 12:10 (pb):		Les predicats sont des terminaux comme	*/
/*				les autres.				*/
/************************************************************************/
/* 05-05-92 16:17 (pb):		Les messages sur le test de circularite	*/
/*				PROPER [3] ont ete passes ds LALR1	*/
/************************************************************************/
/* 18-10-90 11:20 (pb):		Sortie des regles dupliquees associees	*/
/*				aux non-terminaux etendus.		*/
/************************************************************************/
/* 18-06-90 10:26 (pb):		Utilisation de "sxerrmngr.scratch_file"	*/
/************************************************************************/
/* 10-04-90 14:30 (phd):	Suppression des "typedef" de bl		*/
/************************************************************************/
/* 06-04-89 17:40 (phd):	Change un char en SXINT pour Lichnewsky	*/
/************************************************************************/
/* 11-02-88 12:00 (pb):		Suppression du listing des numeros	*/
/*				d'actions				*/
/************************************************************************/
/* 18-05-87 12:32 (phd):	Optimisation				*/
/************************************************************************/
/* 15-05-87 09:07 (pb):		Les index des debuts de regle sont	*/
/*				connus					*/
/************************************************************************/
/* 27-04-87 14:17 (phd):	Modification de detail			*/
/************************************************************************/
/* 03-02-87 12:24 (phd):	Prise en compte correcte des options	*/
/************************************************************************/
/* 10-12-86 15:19 (phd):	Creation du listing au bon moment...	*/
/************************************************************************/
/* 09-12-86 13:18 (phd):	Mise au point et ameliorations		*/
/************************************************************************/
/* 08-12-86 17:23 (phd):	Ajout de la sortie des messages		*/
/************************************************************************/
/* 08-12-86 11:37 (phd):	Optimisations				*/
/************************************************************************/
/* 26-11-86 09:15 (phd):	Reecriture de quelques fonctions	*/
/************************************************************************/
/* 26-11-86 09:13 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
#include "sxba.h"
#include "B_tables.h"
#include "xbnf_vars.h"
#include "varstr.h"
#include "put_edit.h"
char WHAT_XBNFLO[] = "@(#)SYNTAX - $Id: xbnf_lo.c 1122 2008-02-28 15:55:03Z rlacroix $" WHAT_DEBUG;

extern char	by_mess [], release_mess [];
static char	line [132], line_minus [132];
static FILE	*listing;
static SXINT	nbs;
static VARSTR	vstr;


SXVOID
no_tables (void)
{
    sxtmsg (sxsrcmngr.source_coord.file_name, "Construction failed : the tables are not generated.", sxtab_ptr->err_titles [2]);
}




#define get_t_string(t)		(bnf_ag.T_STRING + bnf_ag.ADR [t])
#define get_nt_string(nt)	(bnf_ag.NT_STRING + bnf_ag.ADR [nt])


VARSTR	cat_t_string (VARSTR cat_t_string_vstr, SXINT code)
{
    SXINT	i, prdct = -1;

    if ((i = bnf_ag.WS_TBL_SIZE.tmax - code) > 0) {
	prdct = bnf_ag.XT_TO_T_PRDCT [i].prdct_no;
	code = -bnf_ag.XT_TO_T_PRDCT [i].v_code;
    }

    if (SXBA_bit_is_set (bnf_ag.IS_A_GENERIC_TERMINAL, -code)) {
	varstr_catenate (cat_t_string_vstr, get_t_string (code));
    }
    else {
	varstr_quote (cat_t_string_vstr, get_t_string (code));
    }

    if (prdct >= 0) {
	char	s [12];

	varstr_catenate (cat_t_string_vstr, " &");
	sprintf (s, "%ld", (long)prdct);
	varstr_catenate (cat_t_string_vstr, s);
    }

    return cat_t_string_vstr;
}



static VOID	print_messages (void)
{
    SXINT	i, nt;

    put_edit_nl (1);

    if (proper [1]) {
	/* useless non_terminal */
	SXINT	undefined = 0;
	BOOLEAN		*defined = sxalloc (proper [1] [0] + 1, sizeof (BOOLEAN));

	for (i = 1; i <= proper [1] [0]; i++) {
	    nt = proper [1] [i];

	    if (!(defined [i] = (WX [nt + 1].npg > WX [nt].npg)))
		undefined++;
	}

	put_edit_nl (1);

	switch (undefined) {
	case 1:
	    put_edit_nnl (1, "The non terminal symbol ");

	    for (i = 1; TRUE; i++)
		if (!defined [i]) {
		    put_edit_apnnl (get_nt_string (proper [1] [i]));
		    break;
		}

	    put_edit_ap (" is not defined.");
	    break;

	default:
	    put_edit (1, "The following non terminal symbols");
	    nbs = undefined;

	    for (i = 1; TRUE; i++)
		if (!defined [i]) {
		    put_edit (9, get_nt_string (proper [1] [i]));

		    if (--nbs == 0) {
			break;
		    }
		}

	    put_edit (1, "are not defined.");
	    break;

	case 0:
	    break;
	}

	switch (nbs = proper [1] [0] - undefined) {
	case 1:
	    put_edit_nl (1);
	    put_edit_nnl (1, "The non terminal symbol ");

	    for (i = 1; TRUE; i++)
		if (defined [i]) {
		    put_edit_apnnl (get_nt_string (proper [1] [i]));
		    break;
		}

	    put_edit_ap (" cannot generate a terminal string.");
	    break;

	default:
	    put_edit_nl (1);
	    put_edit (1, "The following non terminal symbols");

	    for (i = 1; TRUE; i++)
		if (defined [i]) {
		    put_edit (9, get_nt_string (proper [1] [i]));

		    if (--nbs == 0) {
			break;
		    }
		}

	    put_edit (1, "cannot generate a terminal string.");
	    break;

	case 0:
	    break;
	}

	sxfree (defined);
	sxfree (proper [1]);
    }

    if (proper [2]) {
	/* unaccessible non_terminals */
	put_edit_nl (1);

	switch (proper [2] [0]) {
	default:
	    put_edit (1, "The following non terminal symbols");

	    for (i = 1; i <= proper [2] [0]; i++)
		put_edit (9, get_nt_string (proper [2] [i]));

	    put_edit (1, "are not accessible from the axiom.");
	    break;

	case 1:
	    put_edit_nnl (1, "The non terminal symbol ");
	    put_edit_apnnl (get_nt_string (proper [2] [1]));
	    put_edit_ap (" is not accessible from the axiom.");

	case 0:
	    break;
	}

	sxfree (proper [2]);
    }
}



static BOOLEAN	less_t (SXINT i, SXINT j)
{
    char	*k1, *k2;


/* les symboles terminaux de t_string sont des chaines C differentes les
   unes des autres */

    k1 = get_t_string (-i);
    k2 = get_t_string (-j);

    while (*k1 == *k2)
	k1++, k2++;

    return *k1 < *k2;
}



static BOOLEAN	less_nt (SXINT i, SXINT j)
{
    char	*k1, *k2;


/* les symboles non terminaux de nt_string sont des chaines C differentes
   les unes des autres */

    k1 = get_nt_string (i);
    k2 = get_nt_string (j);

    while (*k1 == *k2)
	k1++, k2++;

    return *k1 < *k2;
}



static SXINT	*sort (BOOLEAN (*compare)(SXINT, SXINT), SXINT n)
{
    SXINT	i;
    SXINT	*code /* 1:n */ ;

    code = (SXINT*) sxalloc (n + 1, sizeof (SXINT));

    for (i = n; i >= 1; i--)
	code [i] = i;

    sort_by_tree (code, 1, n, compare);
    return code;
}



static char	*strrevcpy (char *destination, char *source, SXINT size)

/* destination = reverse (source) || blancs
   suppose &destination != &source */

{
    char	*d, *d1;

    *(d = destination + size) = NUL;
    d1 = destination + strlen (source);

    while (--d >= d1)
	*d = ' ';

    d1 = source;

    while (*d1)
	*d-- = *d1++;

    return destination;
}



static VOID	print_tables (void)
{
    SXINT	i, j, k, col;
    SXINT		l, x, prev_rule_no, rule_no;
    SXINT		context_table_slice_size, max_slice_no, slice_no, bi, bs, maxsize;
    SXINT		*sort_symbols;

    if (follow) {

/*    NON_TERMINAL_SYMBOLS: */
	{
	    SXINT		Mnt = M_nt + 3;

/* tri non-terminaux */

	    sort_symbols = sort (less_nt, W.xntmax);
	    put_edit_apnnl ("\f    CODE  NON_TERMINAL");

	    for (i = 1; i <= W.xntmax; i++) {
		k = sort_symbols [i];
		put_edit_nl (2);
		put_edit_fnb (5, 3, k);
		put_edit_nnl (11, get_nt_string (k));
		put_edit_nnl (14 + Mnt, "LHS:");
		col = 21 + Mnt;

		for (l = WX [k + 1].npg, x = WX [k].npg; x < l;x++) {
		    put_edit_fnb (col, 4, WN [x].numpg);

		    if ((col += 5) > 110)
			col = 21 + Mnt;
		}

		put_edit_nl (1);
		put_edit_nnl (14 + Mnt, "RHS:");
		prev_rule_no = 0;
		col = 21 + Mnt;

		for (l = WX [k + 1].npd, x = WX [k].npd; x < l;x++) {
		    if (prev_rule_no != (rule_no = WI [numpd [x]].prolis)) {
			put_edit_fnb (col, 4, rule_no);
			prev_rule_no = rule_no;

			if ((col += 5) > 110)
			    col = 21 + Mnt;
		    }
		}
	    }

	    sxfree (sort_symbols);
	}


/* TERMINAL_SYMBOLS: */
	{
	    SXINT		print_tables_M_t, ssj;

	    print_tables_M_t = 0;


/* On recalcule la taille du plus grand terminal apres "requotage" */

	    for (i = -1; i >= -W.xtmax; i--) {
		l = varstr_length (cat_t_string (varstr_raz (vstr), i));
		print_tables_M_t = (l > print_tables_M_t) ? l : print_tables_M_t;
	    }


/* tri des terminaux etendus */

	    sort_symbols = sort (less_t, W.xtmax);
	    put_edit_nl (1);
	    put_edit_ff ();
	    put_edit_apnnl ("    CODE  TERMINAL");
	    put_edit (21 + print_tables_M_t, "RHS:");


/* nombre de RHS dans la ligne */

	    for (i = -1; i >= -W.xtmax; i--) {
		k = -sort_symbols [-i];
		put_edit_nl (2);
		put_edit_fnb (5, 3, k);
		put_edit_nnl (11, varstr_tostr (cat_t_string (varstr_raz (vstr), k)));
		prev_rule_no = 0;
		col = 21 + print_tables_M_t;

		for (l = tpd [k - 1], x = tpd [k]; x < l;x++) {
		    if (prev_rule_no != (rule_no = WI [tnumpd [x]].prolis)) {
			put_edit_fnb (col, 4, rule_no);
			prev_rule_no = rule_no;

			if ((col += 5) > 110)
			    col = 21 + print_tables_M_t;
		    }
		}
	    }

/* context_table_edition: */
	    /* tri des terminaux */
	    if (-W.tmax != W.xtmax) {
		sxfree (sort_symbols);
		sort_symbols = sort (less_t, -W.tmax);
	    }

	    context_table_slice_size = (128 - print_tables_M_t) >> 1;
	    max_slice_no = (-W.tmax - 1) / context_table_slice_size + 1;

	    for (slice_no = 1; slice_no <= max_slice_no; slice_no++) {
		put_edit_nl (1);
		put_edit_ff ();
		put_edit_apnnl ("\t\tC O N T E X T    T A B L E");

		if (slice_no < max_slice_no)
		    put_edit_apnnl ((slice_no == 1) ? "\t( T O   B E   C O N T I N U E D )" : "\t( C O N T I N U E D )");
		else if (slice_no > 1)
		    put_edit_apnnl ("\t( E N D )");

		put_edit_nl (3);
		bi = context_table_slice_size * (slice_no - 1) + 1;

		if ((bs = context_table_slice_size * slice_no) > -W.tmax) {
		    bs = -W.tmax;
		}


/* on calcule la taille du plus grand nom de terminal */

		maxsize = 0;

		for (i = bi; i <= bs; i++) {
		    l = varstr_length (cat_t_string (varstr_raz (vstr), -sort_symbols [i]));
		    maxsize = (l > maxsize) ? l : maxsize;
		}

		{
		    char	**ren /* bi:bs *//* maxsize */ ;

		    ren = (char**) sxalloc (bs - bi + 1, sizeof (char*));

		    for (i = bi; i <= bs; i++) {
			ren [i - bi] = (char*) sxalloc (maxsize + 1, sizeof (char));
			strrevcpy (ren [i - bi], varstr_tostr (cat_t_string (varstr_raz (vstr), -sort_symbols [i])), maxsize);
		    }

		    line [i = print_tables_M_t + 2 * (bs - bi + 1 + 1)] = NUL;
		    line_minus [i] = NUL;

		    while (--i >= 0) {
			line [i] = ' ';
			line_minus [i] = '-';
		    }

		    line [print_tables_M_t + 1] = '|';

		    for (i = maxsize - 1; i >= 0; i--) {
			k = print_tables_M_t + 1;

			for (j = bi; j <= bs; j++) {
			    line [k += 2] = ren [j - bi] [i];
			}

			put_edit_ap (line);
		    }

		    put_edit_ap (line_minus);

		    for (i = 1; i <= -W.tmax; i++) {
			SXBA	linei;

			linei = follow [sort_symbols [i]];
			/* LINTED this cast from long to int is needed by printf() */
			sprintf (line, "%-*s | ", (int)print_tables_M_t, varstr_tostr (cat_t_string (varstr_raz (vstr), -sort_symbols [i])))
			     ;
			k = print_tables_M_t + 1;

			for (j = bi; j <= bs; j++) {
			    ssj = sort_symbols [j];
			    line [k += 2] = !SXBA_bit_is_set (linei, ssj) ? '.' : 'X';
			}

			put_edit_ap (line);
		    }

		    for (i = bs - bi; i >= 0; i--)
			sxfree (ren [i]);

		    sxfree (ren);
		}
	    }

	    sxfree (sort_symbols);
	}
    }
}



static FILE	*error_file /* Where error messages have been kept */ ;
static long	error_pos /* Current position in error_file */ ;
static char	*err_string = {NULL};
static SXINT	max_err_size = {128};



static VOID	bnf_output_error (SXINT bnf_output_error_errno)
{
    SXINT	err_size;
    long	filepos;

    if ((filepos = sxerrmngr.err_infos [bnf_output_error_errno].beginning) != error_pos)
	fseek (error_file, filepos - error_pos, 1);

    error_pos = sxerrmngr.err_infos [bnf_output_error_errno].end;

    if ((err_size = error_pos - filepos) > max_err_size) {
	err_string = (char*) sxrealloc (err_string, (max_err_size = err_size) + 1, sizeof (char));
    }

    fread (err_string + 1, sizeof (char), err_size, error_file);
    fwrite (err_string, sizeof (char), err_size + 1, listing);
}

extern SXUINT bnf_get_line_no (SXINT);

static VOID	bnf_header (void)
{
    put_edit_nnl (9, "Listing Of:");
    put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
    put_edit_nnl (9, "By:");
    put_edit (25, by_mess);
    put_edit (25, release_mess);

    {
	extern char	*options_text (char *);
	char	bnf_header_line [132];

	put_edit_nnl (9, "Options:");
	put_edit (25, options_text (bnf_header_line));
    }

    {
	long	date_time;

	put_edit_nnl (9, "Generated on:");
	date_time = time (0);
	put_edit (25, ctime (&date_time));
    }

    put_edit_nl (2);

    {
	int	c;
	SXINT	line_no, rule_no, next_rule_line_no;
	SXINT	bnf_header_errno, i, n;
	SXUINT	errline_no;
	SXUINT	this_file_ste;
	extern VOID	sxerrsort (void);

	sxinitialise(this_file_ste); /* pour faire taire gcc -Wuninitialized */
	bnf_header_errno = 0, errline_no = ~0;

	if (sxerrmngr.err_kept > 0) {
	    sxerrsort ();

	    if ((error_file = sxerrmngr.scratch_file) != NULL) {
		rewind (error_file);
	    }

	    error_pos = 0;
	    this_file_ste = sxstrsave (sxsrcmngr.source_coord.file_name);
	    err_string = sxalloc (max_err_size + 1, sizeof (char)), err_string [0] = NEWLINE;

	    for (bnf_header_errno = 0; bnf_header_errno < sxerrmngr.err_kept; bnf_header_errno++) {
		if (sxerrmngr.err_infos [bnf_header_errno].file_ste == this_file_ste) {
		    if (sxerrmngr.err_infos [bnf_header_errno].line != 0) {
			errline_no = sxerrmngr.err_infos [bnf_header_errno].line;
			break;
		    }

		    if (error_file != NULL) {
			bnf_output_error (bnf_header_errno);
			put_edit_nl (1);
		    }
		}
	    }
	}

	put_edit_nl (1);
	rewind (sxsrcmngr.infile);
	line_no = 0;
	next_rule_line_no = bnf_get_line_no (rule_no = 1);

	while ((c = getc (sxsrcmngr.infile)) != EOF) {
	    put_edit_fnb (1, 5, ++line_no);

	    if (line_no == next_rule_line_no) {
		put_edit_fnb (7, 5, rule_no);
		next_rule_line_no = bnf_get_line_no (++rule_no);
	    }

	    put_edit_nnl (15, ": ");

	    while (c != EOF && c != NEWLINE) {
		putc (c, listing);
		c = getc (sxsrcmngr.infile);
	    }

	    if (errline_no == (SXUINT)line_no) {
		static char	STARS [] = "\n\n******";

		if (sxerrmngr.err_infos [bnf_header_errno].severity != 0) {
		    fputs (STARS, listing);
		}

		do {
		    if (error_file != NULL)
			bnf_output_error (bnf_header_errno);

		    if (sxerrmngr.err_infos [bnf_header_errno].severity != 0) {
			fputs (STARS + 2, listing);
		    }

		    errline_no = (++bnf_header_errno < sxerrmngr.err_kept && sxerrmngr.err_infos [bnf_header_errno].file_ste == this_file_ste ?
			 sxerrmngr.err_infos [bnf_header_errno].line : (unsigned)~0);
		} while (errline_no == (SXUINT)line_no);

		put_edit_nl (1);
	    }

	    put_edit_nl (1);
	}

	if (W.xnbpro > W.nbpro) {
	    SXINT	x, col, l, tnt;
	    SXINT			left_margin, lim;
	    char	*str;

	    do {
		put_edit_nnl (5, "*");
		put_edit_fnb (7, 5, rule_no);
		put_edit_nnl (15, ": ");
		put_edit_nnl (17, get_nt_string (bnf_ag.WS_NBPRO [rule_no].reduc));
		put_edit_apnnl ("\t= ");
		col = left_margin = put_edit_get_col ();

		for (lim = WN [rule_no + 1].prolon - 1, x = WN [rule_no].prolon; x < lim;x++) {
		    str = ((tnt = WI [x].lispro) > 0) ? get_nt_string (tnt) : get_t_string (tnt);

		    if ((col += (l = strlen (str) + 1)) > 128) {
			put_edit_nnl (5, "*");
			put_edit_nnl (15, ": ");
			put_edit_nnl (left_margin, str);
			col = left_margin + l;
		    }
		    else
			put_edit_apnnl (str);

		    put_edit_apnnl (" ");
		}

		put_edit_ap (";");
	    } while (++rule_no <= W.xnbpro);
	}

	put_edit_nl (1);

	while (error_file != NULL && bnf_header_errno < sxerrmngr.err_kept && sxerrmngr.err_infos [bnf_header_errno].file_ste == this_file_ste) {
	    bnf_output_error (bnf_header_errno++);
	    put_edit_nl (1);
	}

	{
	  SXINT	msgnb [SXSEVERITIES+3];

	  for (n = 0, i = 1; i < SXSEVERITIES+3; i++)
	    n += msgnb [i] = (i < SXSEVERITIES) ? sxerrmngr.nbmess [i] : 0 ;
	
	  if (sxplocals.sxtables /* pas bien */ ->abstract != NULL && n != 0)
	    fprintf (listing, sxplocals.sxtables->abstract, msgnb [1], msgnb [2], msgnb [3], msgnb [4], msgnb [5]);
	}

	put_edit_nl (1);

	if (err_string != NULL) {
	    sxfree (err_string), err_string = NULL;
	}
    }
}



VOID	bnf_lo (void)
{
  char *lst_name;

  if (is_source || !is_proper) {
    if (sxverbosep)
      fputs ("   Listing Output\n", sxtty);

    vstr = varstr_alloc (32);

    vstr = varstr_catenate (vstr, prgentname);
    vstr = varstr_catenate (vstr, ".bn.l");
    lst_name = varstr_tostr (vstr);

    if ((listing = sxfopen (lst_name, "w")) == NULL) {
      fprintf (sxstderr, "xbnf_lo: cannot open (create) ");
      sxperror (lst_name);
      return;
    }

    varstr_raz (vstr);
    put_edit_initialize (listing);

    if (is_source)
      bnf_header ();

    if (!is_proper)
      print_messages ();

    if (is_list)
      print_tables ();

    varstr_free (vstr);
  }
}
