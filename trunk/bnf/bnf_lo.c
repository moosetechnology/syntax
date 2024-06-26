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
#include "sxba.h"
#include "B_tables.h"
#include "bnf_vars.h"
#include "varstr.h"
#include "put_edit.h"

char WHAT_BNFLO[] = "@(#)SYNTAX - $Id: bnf_lo.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

extern char	by_mess [];
static FILE	*listing;
static SXINT	nbs;
static VARSTR	vstr;


void
no_tables (void)
{
    sxtmsg (sxsrcmngr.source_coord.file_name, "Construction failed : the tables are not generated.", sxtab_ptr->err_titles [2]+1);
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
	char	s [20];

	varstr_catenate (cat_t_string_vstr, " &");
	sprintf (s, "%ld", prdct);
	varstr_catenate (cat_t_string_vstr, s);
    }

    return cat_t_string_vstr;
}



static void	print_messages (void)
{
    SXINT	i, nt;

    put_edit_nl (1);

    if (proper [1]) {
	/* useless non_terminal */
	SXINT	undefined = 0;
	bool		*defined = sxalloc (proper [1] [0] + 1, sizeof (bool));

	for (i = 1; i <= proper [1] [0]; i++) {
	    nt = proper [1] [i];

	    if (!(defined [i] = (WX [nt + 1].npg > WX [nt].npg)))
		undefined++;
	}

	put_edit_nl (1);

	switch (undefined) {
	case 1:
	    put_edit_nnl (1, "The non terminal symbol ");

	    for (i = 1; true; i++)
		if (!defined [i]) {
		    put_edit_apnnl (get_nt_string (proper [1] [i]));
		    break;
		}

	    put_edit_ap (" is not defined.");
	    break;

	default:
	    put_edit (1, "The following non terminal symbols");
	    nbs = undefined;

	    for (i = 1; true; i++)
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

	    for (i = 1; true; i++)
		if (defined [i]) {
		    put_edit_apnnl (get_nt_string (proper [1] [i]));
		    break;
		}

	    put_edit_ap (" cannot generate a terminal string.");
	    break;

	default:
	    put_edit_nl (1);
	    put_edit (1, "The following non terminal symbols");

	    for (i = 1; true; i++)
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



static bool	less_t (SXINT i, SXINT j)
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



static bool	less_nt (SXINT i, SXINT j)
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



static SXINT	*sort (bool (*compare) (SXINT, SXINT), SXINT n)
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

    *(d = destination + size) = SXNUL;
    d1 = destination + strlen (source);

    while (--d >= d1)
	*d = ' ';

    d1 = source;

    while (*d1)
	*d-- = *d1++;

    return destination;
}



static void	print_tables (void)
{
  SXINT	i, j, k, col;
  SXINT		l, x, prev_rule_no, rule_no;
  SXINT		context_table_slice_size, max_slice_no, slice_no, bi, bs, maxsize;
  SXINT		*sort_symbols;

  if (is_proper) {

/*  NON_TERMINAL_SYMBOLS: */
    {
      SXINT		Mnt = M_nt + 3;

      /* tri non-terminaux */

      sort_symbols = sort (less_nt, W.xntmax);
      put_edit_apnnl ("\n\tCODE  NON_TERMINAL");

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


/*  TERMINAL_SYMBOLS: */
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

/*    context_table_edition: */
      if (follow) {
	/* tri des terminaux */
	if (-W.tmax != W.xtmax) {
	  sxfree (sort_symbols);
	  sort_symbols = sort (less_t, -W.tmax);
	}

	context_table_slice_size = (128 - print_tables_M_t) >> 1;
	max_slice_no = (-W.tmax - 1) / context_table_slice_size + 1;

	for (slice_no = 1; slice_no <= max_slice_no; slice_no++) {
	  put_edit_nl (1);
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
	    char	*line, *line_minus;
	    char	**ren /* bi:bs *//* maxsize */ ;

	    ren = (char**) sxalloc (bs - bi + 1, sizeof (char*));

	    for (i = bi; i <= bs; i++) {
	      ren [i - bi] = (char*) sxalloc (maxsize + 1, sizeof (char));
	      strrevcpy (ren [i - bi], varstr_tostr (cat_t_string (varstr_raz (vstr), -sort_symbols [i])), maxsize);
	    }

	    i = print_tables_M_t + 2 * (bs - bi + 1 + 1);

	    line = (char *) sxalloc (i+1, sizeof (char));
	    line_minus = (char *) sxalloc (i+1, sizeof (char));

	    line [i] = SXNUL;
	    line_minus [i] = SXNUL;

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
	      /* LINTED this cast from long to int is needed by printf */
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
	    sxfree (line);
	    sxfree (line_minus);
	  }
	}

	sxfree (sort_symbols);
      }
    }
  }
}



static FILE	*error_file /* Where error messages have been kept */ ;
static off_t	error_pos /* Current position in error_file */ ;
static char	*err_string = {NULL};
static SXINT	max_err_size = {128};



static void	bnf_output_error (SXINT bnf_output_error_errno)
{
    SXINT	err_size;
    off_t	filepos;

    if ((filepos = sxerrmngr.err_infos [bnf_output_error_errno].beginning) != error_pos)
	fseeko (error_file, filepos - error_pos, 1);

    error_pos = sxerrmngr.err_infos [bnf_output_error_errno].end;

    if ((err_size = (SXINT) (error_pos - filepos)) > max_err_size) {
	err_string = (char*) sxrealloc (err_string, (max_err_size = err_size) + 1, sizeof (char));
    }

    fread (err_string + 1, sizeof (char), err_size, error_file);
    fwrite (err_string, sizeof (char), err_size + 1, listing);
}



extern SXUINT bnf_get_line_no (SXINT);

static void	bnf_header (void)
{
    put_edit_nnl (9, "Listing of:");
    put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
    put_edit_nnl (9, "By:");
    put_edit (25, by_mess);
    put_edit (25, release_mess);

    {
	extern char	*options_text (char *);
	char	bnf_get_line_no_line [132];

	put_edit_nnl (9, "Options:");
	put_edit (25, options_text (bnf_get_line_no_line));
    }

    {
	time_t	date_time;

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
	SXINT	this_file_ste;
	extern void	sxerrsort (void);

	sxinitialise(this_file_ste); /* pour faire taire gcc -Wuninitialized */
	bnf_header_errno = 0, errline_no = ~((SXUINT) 0);

	if (sxerrmngr.err_kept > 0) {
	    sxerrsort ();

	    if ((error_file = sxerrmngr.scratch_file) != NULL) {
		rewind (error_file);
	    }

	    error_pos = 0;
	    this_file_ste = sxstrsave (sxsrcmngr.source_coord.file_name);
	    err_string = sxalloc (max_err_size + 1, sizeof (char)), err_string [0] = SXNEWLINE;

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

	    while (c != EOF && c != SXNEWLINE) {
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



void	bnf_lo (void)
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
      fprintf (sxstderr, "bnf_lo: cannot open (create) ");
      sxperror (lst_name);
      return;
    }
	
#if 0
    /* Nom de langage qui depace 32 le 27/04/04 (merci Benoit) !! */
    {
      char	lst_name [32];

      if ((listing = sxfopen (strcat (strcpy (lst_name, prgentname), ".bn.l"), "w")) == NULL) {
	fprintf (sxstderr, "bnf_lo: cannot open (create) ");
	sxperror (lst_name);
	return;
      }
    }
#endif /* 0 */

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
