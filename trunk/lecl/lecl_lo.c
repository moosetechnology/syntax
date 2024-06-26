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

#define SXS_MAX (SHRT_MAX+1)
#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "sxsstmt.h"
#include "varstr.h"
#include "lecl_ag.h"
#include "put_edit.h"

char WHAT_LECLLO[] = "@(#)SYNTAX - $Id: lecl_lo.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;



/*   L I S T I N G _ O U T P U T   */

extern char	*options_text (char *line);
extern VARSTR	lecl_gen_cc (SXBA *SC_TO_CHAR_SET, 
			     SXBA esc_set, 
			     VARSTR varstr_ptr);
extern VARSTR	lecl_gen_sc (SXBA *SC_TO_CHAR_SET, 
			     SXINT simple_class, 
			     VARSTR varstr_ptr);
extern void	equality_sort (SXINT *t, SXINT bi, SXINT bs, bool (*less_equal) (SXINT, SXINT), bool (*equal) (SXINT, SXINT));
extern char	by_mess [];
static SXINT	context_table_slice_size, i, j, k, l, x, max_slice_no, slice_no, tmin, tmax, min_re, origine, prev,
     state_no, resize, maxsize;
static char	*s;
static char	sk /* bit (6) */ ;
static char	opt_set /* bit (3) */ ;
static char	line [132];
static char	string [64];
static SXBA	/* max_re_lgth */ next;
static struct ers_item	*erse_ptr;
static struct action_or_prdct_code_item		fe;
static bool	is_error;
static SXINT	*to_be_sorted /* 1:Smax */ ;
static SXBA	/* Smax */ *sc_to_sc_set /* 1:Smax */ ;
static SXBA	/* Smax */ sc_set;
static transition_matrix_item	*transition_matrix_line, stmt;



static void	header (void)
{
    time_t	date_time;

    put_edit_nnl (9, "Listing of:");
    put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
    put_edit_nnl (9, "By:");
    put_edit (25, by_mess);
    put_edit (25, release_mess);
    put_edit_nnl (9, "Options:");
    put_edit (25, options_text (line));
    put_edit_nnl (9, "Generated on:");
    date_time = time (0);
    put_edit (25, ctime (&date_time));
    put_edit_nl (2);
}



static char	*cat_red_name (char *cat_red_name_s, SXINT cat_red_name_j)
{
    if (cat_red_name_j < 0)
	cat_red_name_j = -cat_red_name_j;

    return strcat (cat_red_name_s, sxstrget (cat_red_name_j));
}

static char	*get_master_name (char *get_master_name_s, SXINT ner)
{
    *get_master_name_s = SXNUL;
    return (cat_red_name (get_master_name_s, ers_disp [ers_disp [ner].master_token].reduc));
}

static char	*get_re_name (char *get_re_name_s, SXINT ner)
{
    SXINT		mner;

    mner = ers_disp [ner].master_token;
    *get_re_name_s = SXNUL;

    if (mner == ner)
	return (cat_red_name (get_re_name_s, ers_disp [ner].reduc));
    else
	return cat_red_name (strcat (cat_red_name (get_re_name_s, ers_disp [mner].reduc), "."), ers_disp [ner].reduc);
}



static char	*cv_x (char *cv_x_s, SXINT cv_x_x)
{
    if (ers [cv_x_x].lispro == -1 || ers [cv_x_x].lispro == -2)
	/* End_Of_Token */
	return strcpy (cv_x_s, "EOT");

    sprintf (cv_x_s, "%-ld", (SXINT) cv_x_x);
    return cv_x_s;
}



static char	*cv_delete (char *cv_delete_s, bool del)
{
    if (del)
	return strcpy (cv_delete_s, "-");

    *cv_delete_s = SXNUL;
    return cv_delete_s;
}



static char	*cv_priority (char *cv_priority_s, char kind /* bit (3) */)
{
    *cv_priority_s = SXNUL;

    if (kind) {
	if (kind & Reduce_Reduce) {
	    strcat (cv_priority_s, "R>R");
	    kind -= Reduce_Reduce;

	    if (kind)
		strcat (cv_priority_s, ", ");
	}

	if (kind & Reduce_Shift) {
	    kind -= Reduce_Shift;
	    strcat (cv_priority_s, "R>S");

	    if (kind)
		strcat (cv_priority_s, ", ");
	}

	if (kind & Shift_Reduce)
	    strcat (cv_priority_s, "S>R");
    }

    return (cv_priority_s);
}



static SXINT	cv_to_t_code (SXINT ner)
{
    SXINT		ate, cv_to_t_code_t;

    ate = ers_disp [ers_disp [ner].master_token].reduc;

    if (ate <= 0)
	return (0);
    else {
	cv_to_t_code_t = ate_to_t [ate];

	if (cv_to_t_code_t >= 0)
	    return (cv_to_t_code_t);
	else
	    return (0);
    }
}



static bool	less_equal (SXINT less_equal_i, SXINT less_equal_j)
{
    return transition_matrix_line [less_equal_i] <= transition_matrix_line [less_equal_j];
}



static bool	equal (SXINT equal_i, SXINT equal_j)
{
    return transition_matrix_line [equal_i] == transition_matrix_line [equal_j];
}



static void	put_body (transition_matrix_item put_body_stmt)
{
    char	message [64];
    SXINT		put_body_next = NEXT (put_body_stmt);
    SXINT		codop = CODOP (put_body_stmt);

    if (SCAN (put_body_stmt) && !KEEP (put_body_stmt)) {
	put_edit_nnl (61, "-");
    }

    if (SCAN (put_body_stmt)) {
	put_edit_nnl (65, "*");
    }

    switch (codop) {
    case Error:
	strcpy (message, (put_body_next == 0) ? "ERROR" : "ERROR_IN_LOOK_AHEAD");

	break;

    case FirstLookAhead:
    case NextLookAhead:
	put_edit_nnl (65, "**");
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	/* absence de "break" (intentionnelle ?) */
	__attribute__ ((fallthrough));
#endif

    case SameState:
    case State:
	sprintf (message, "GOTO STATE %-ld", (SXINT) put_body_next);
	break;

    case ActPrdct:
	switch (action_or_prdct_code [put_body_next].kind) {
	case IsPredicate:
	    sprintf (message, "GOTO PRDCT %-ld", (SXINT) put_body_next);
	    break;

	case IsAction:
	    sprintf (message, "GOTO ACTION %-ld", (SXINT) put_body_next);
	    break;

	case IsNonDeter:
	    sprintf (message, "GOTO NONDETER %-ld", (SXINT) put_body_next);
	    break;
	default:
#if EBUG
	  sxtrap("lecl_lo","unknown switch case #1");
#endif
	  break;
	}

	break;

    case Reduce:
    case ReducePost:
	sprintf (message, "=> %s", sxstrget (t_to_ate [put_body_next]));

	if (codop == ReducePost)
	    strcat (message, " (Post Action)");

	break;

    case HashReduce:
    case HashReducePost:
	if (put_body_next == 0)
	    strcpy (message, "=> ERROR+THSH");
	else {
	    sprintf (message, "=> %s+THSH", sxstrget (t_to_ate [put_body_next]));

	    if (codop == HashReducePost)
		strcat (message, " (Post Action)");
	}
	break;
    default:
#if EBUG
      sxtrap("lecl_lo","unknown switch case #2");
#endif
      break;
    }

    put_edit (70, message);
}



static char	*gen_action_or_prdct_name_from_fe (char *gen_action_or_prdct_name_from_fe_s, 
						   struct action_or_prdct_code_item gen_action_or_prdct_name_from_fe_fe)
{
    if (gen_action_or_prdct_name_from_fe_fe.kind == IsAction) {
	/* action */
	if (gen_action_or_prdct_name_from_fe_fe.is_system) {
	    switch (gen_action_or_prdct_name_from_fe_fe.action_or_prdct_no) {
	    case 1:
		/* Lower_To_Upper */
		strcpy (gen_action_or_prdct_name_from_fe_s, "@Lower_To_Upper");
		break;

	    case 2:
		/* Upper_To_Lower */
		strcpy (gen_action_or_prdct_name_from_fe_s, "@Upper_To_Lower");
		break;

	    case 3:
		/* Set */
		sprintf (gen_action_or_prdct_name_from_fe_s, "@Set (%-ld)", (SXINT) gen_action_or_prdct_name_from_fe_fe.param);
		break;

	    case 4:
		/* Reset */
		sprintf (gen_action_or_prdct_name_from_fe_s, "@Reset (%-ld)", (SXINT) gen_action_or_prdct_name_from_fe_fe.param);
		break;

	    case 5:
		/* Erase */
		strcpy (gen_action_or_prdct_name_from_fe_s, "@Erase");
		break;

	    case 6:
		/* Include */
		if (gen_action_or_prdct_name_from_fe_fe.param == 0)
		    strcpy (gen_action_or_prdct_name_from_fe_s, "@Include");
		else
		    sprintf (gen_action_or_prdct_name_from_fe_s, "@Include (Post_Action : @%-ld)", gen_action_or_prdct_name_from_fe_fe.param);

		break;

	    case 7:
		/* Upper_Case */
		strcpy (gen_action_or_prdct_name_from_fe_s, "@Upper_Case");
		break;

	    case 8:
		/* Lower_Case */
		strcpy (gen_action_or_prdct_name_from_fe_s, "@Lower_Case");
		break;

	    case 9:
		/* Put */
		strcat (strcat (strcpy (gen_action_or_prdct_name_from_fe_s, "@Put (\""), SXCHAR_TO_STRING (gen_action_or_prdct_name_from_fe_fe.param)), "\")");
		break;

	    case 10:
		/* Mark */
		strcpy (gen_action_or_prdct_name_from_fe_s, "@Mark");
		break;

	    case 11:
		/* Release */
		strcpy (gen_action_or_prdct_name_from_fe_s, "@Release");
		break;

	    case 12:
		/* Incr */
		sprintf (gen_action_or_prdct_name_from_fe_s, "@Incr (%-ld)", gen_action_or_prdct_name_from_fe_fe.param);
		break;

	    case 13:
		/* Decr */
		sprintf (gen_action_or_prdct_name_from_fe_s, "@Decr (%-ld)", gen_action_or_prdct_name_from_fe_fe.param);
		break;
	    default:
#if EBUG
	      sxtrap("lecl_lo","unknown switch case #3");
#endif
	      break;

	    }
	}
	else {
	    sprintf (gen_action_or_prdct_name_from_fe_s, "@%-ld", (SXINT) gen_action_or_prdct_name_from_fe_fe.action_or_prdct_no);
	}
    }
    else if (gen_action_or_prdct_name_from_fe_fe.kind == IsPredicate) {
	/* predicate */
	if (gen_action_or_prdct_name_from_fe_fe.is_system) {
	    switch (gen_action_or_prdct_name_from_fe_fe.action_or_prdct_no) {
	    case 0:
		/* &0 */
		strcpy (gen_action_or_prdct_name_from_fe_s, "&True");
		break;

	    case 1:
		/* Is_First_Col */
		strcpy (gen_action_or_prdct_name_from_fe_s, "&Is_First_Col");
		break;

	    case 2:
		/* Is_Set */
		sprintf (gen_action_or_prdct_name_from_fe_s, "&Is_Set (%-ld)", gen_action_or_prdct_name_from_fe_fe.param);
		break;

	    case 3:
		/* Is_Reset */
		sprintf (gen_action_or_prdct_name_from_fe_s, "&Is_Reset (%-ld)", gen_action_or_prdct_name_from_fe_fe.param);
		break;

	    case 4:
		/* ^Is_First_Col */
		strcpy (gen_action_or_prdct_name_from_fe_s, "^&Is_First_Col");
		break;

	    case 5:
		/* Is_Last_Col */
		strcpy (gen_action_or_prdct_name_from_fe_s, "&Is_Last_Col");
		break;

	    case 6:
		/* ^Is_Last_Col */
		strcpy (gen_action_or_prdct_name_from_fe_s, "^&Is_Last_Col");
		break;
	    default:
#if EBUG
	      sxtrap("lecl_lo","unknown switch case #4");
#endif
	      break;
	    }
	}
	else {
	    if (gen_action_or_prdct_name_from_fe_fe.action_or_prdct_no < 0)
		sprintf (gen_action_or_prdct_name_from_fe_s, "^&%-ld", (SXINT) -gen_action_or_prdct_name_from_fe_fe.action_or_prdct_no);
	    else
		sprintf (gen_action_or_prdct_name_from_fe_s, "&%-ld", (SXINT) gen_action_or_prdct_name_from_fe_fe.action_or_prdct_no);
	}
    }
    else {
	sprintf (gen_action_or_prdct_name_from_fe_s, "ND%-ld", (SXINT) gen_action_or_prdct_name_from_fe_fe.action_or_prdct_no);
    }

    return gen_action_or_prdct_name_from_fe_s;
}


static bool	less_prdct (SXINT less_prdct_i, SXINT less_prdct_j)
{
    return (prdct_to_ers [less_prdct_i] < prdct_to_ers [less_prdct_j]);
}



static bool	compare_kw (SXINT compare_kw_i, SXINT compare_kw_j)
{
    char	*si, *sj;

    for (si = sxstrget (keywords [compare_kw_i].string_table_entry), sj = sxstrget (keywords [compare_kw_j].string_table_entry); *si != SXNUL &&
	 *si == *sj; si++, sj++)
	;

    return *si < *sj;
}



char	*strrevcpy (char *destination, char *source, SXINT size)

/* destination = reverse (source) || blancs
   suppose &destination != &source */

{
    char	*d, *d1;

    d = destination + size;
    *d = SXNUL;
    d1 = destination + strlen (source);

    for (d--; d >= d1; d--)
	*d = ' ';

    for (; *source; source++, d--)
	*d = *source;

    return destination;
}



void	lecl_list_out (void)
{
    VARSTR	varstr_ptr;
    FILE	*listing;
    char	*lst_name;

    if (sxverbosep) {
	if (!sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = true;
	}

	fputs ("Listing Output\n", sxtty);
    }

    varstr_ptr = varstr_alloc (128);

    varstr_ptr = varstr_catenate (varstr_ptr, prgentname);
    varstr_ptr = varstr_catenate (varstr_ptr, ".lc.l");
    lst_name = varstr_tostr (varstr_ptr);

    if ((listing = sxfopen (lst_name, "w")) == NULL) {
	fprintf (sxstderr, "listing_output: cannot open (create) ");
	sxperror (lst_name);
	return;
    }

    put_edit_initialize (listing);
    header ();
    sxlisting_output (listing);
    opt_set = 0;

    if (is_object)
	/* generated_code */
	opt_set += 4;

    if (is_table)
	/* symbol table */
	opt_set += 2;

    if (is_list)
	/* all */
	opt_set += 1;

    if (opt_set == 0 || !st_done)
	return;

    varstr_raz (varstr_ptr);

    if (opt_set & 3) {
	put_edit_ap ("\n");
	put_edit (21, "S Y M B O L    T A B L E");
	put_edit_nl (3);
	put_edit_nnl (2, "re_code");
	put_edit_nnl (11, "t_code");
	put_edit_nnl (21, "name");
	put_edit_nnl (35, "index");
	put_edit_nnl (45, "del");
	put_edit_nnl (51, "class_name");
	put_edit_nnl (71, "followers");
	put_edit_nnl (81, "post_action");
	put_edit_nnl (95, "priority");
	put_edit (107, "unbounded");

	for (i = 1; i <= max_re; i++) {
	    origine = ers_disp [i].prolon;

	    if (ers_disp [i + 1].prolon != origine) {
		next = suivant [origine];
		prev = sxba_scan (next, 0);
		put_edit_nl (1);
		put_edit_fnb (2, 4, i);
		put_edit_fnb (11, 4, cv_to_t_code (i));
		put_edit_nnl (21, get_re_name (string, i));
		put_edit_fnb (35, 4, origine);
		put_edit_nnl (71, cv_x (string, origine + prev));

		if (ers_disp [i].post_action != 0)
		    put_edit_nnl (81, sxstrget (action_or_prdct_to_ate [ers_disp [i].post_action]));

		put_edit_nnl (95, cv_priority (string, ers_disp [i].priority_kind));

		if (ers_disp [i].is_unbounded_context)
		    put_edit (111, "X");
		else
		    put_edit_nl (1);

		while ((prev = sxba_scan (next, prev)) > 0) {
		    put_edit (71, cv_x (string, origine + prev));
		}

		for (x = origine + 1; x < ers_disp [i + 1].prolon; x++) {
		    erse_ptr = ers + x;
		    next = suivant [x];
		    prev = sxba_scan (next, 0);
		    put_edit_fnb (35, 4, x);
		    put_edit_nnl (45, cv_delete (string, erse_ptr->is_erased));

		    if (erse_ptr->lispro < 0) {
			char	*lecl_list_out_s = sxstrget (action_or_prdct_to_ate [erse_ptr->lispro]);

			if (*(lecl_list_out_s + 1) == 'P') {
			    /* @Put */
			    put_edit_nnl (51, "@Put (\"");
			    put_edit_apnnl (SXCHAR_TO_STRING (*(lecl_list_out_s + 6)));
			    put_edit_apnnl ("\")");
			}
			else
			    put_edit_nnl (51, lecl_list_out_s);
		    }
		    else {
			char *lecl_list_out_s = sxstrget (abbrev_or_class_to_ate [erse_ptr->lispro]);

			if (*lecl_list_out_s == '^') {
			    put_edit_nnl (51, "^");
			    lecl_list_out_s++;
			}
			else {
			    put_edit_nnl (51, "");
			}
			put_edit_apnnl (varstr_tostr (varstr_cat_cstring (varstr_raz (varstr_ptr), lecl_list_out_s)));
		    }

		    if (erse_ptr->prdct_no != 0)
			put_edit_apnnl (sxstrget (action_or_prdct_to_ate [erse_ptr->prdct_no]));

		    put_edit (71, cv_x (string, origine + prev));

		    while ((prev = sxba_scan (next, prev)) > 0) {
			put_edit (71, cv_x (string, origine + prev));
		    }
		}
	    }
	}

	if (xprdct_to_ate > prdct_false_code) {
	    SXINT		*lecl_list_out_to_be_sorted /* 1:xprdct_to_ate */ ;
	    SXINT		prdct_no, lecl_list_out_i;
	    bool	done;

	    lecl_list_out_to_be_sorted = (SXINT*) sxalloc ((SXINT) (xprdct_to_ate + 1), sizeof (SXINT));

	    for (prdct_no = 1; prdct_no <= xprdct_to_ate; prdct_no++) {
		lecl_list_out_to_be_sorted [prdct_no] = prdct_no;
	    }

	    if (xprdct_to_ate > prdct_false_code + 1)
		sort_by_tree (lecl_list_out_to_be_sorted, prdct_false_code + 1, xprdct_to_ate, less_prdct);

	    put_edit_nl (3);
	    put_edit (11, "PREDICATES");
	    put_edit_nnl (21, "name");
	    put_edit_nnl (35, "index");
	    put_edit_nnl (51, "prdct");
	    put_edit (71, "followers");

	    for (lecl_list_out_i = prdct_false_code + 1; lecl_list_out_i <= xprdct_to_ate; lecl_list_out_i++) {
		prdct_no = lecl_list_out_to_be_sorted [lecl_list_out_i];
		origine = prdct_to_ers [prdct_no];
		put_edit_nl (1);
		put_edit_nnl (21, sxstrget (action_or_prdct_to_ate [prdct_no]));
		done = false;

		for (x = origine; !done; x++) {
		    erse_ptr = ers + x;

		    if (x > origine && erse_ptr->lispro < 0)
			done = true;
		    else {
			put_edit_fnb (35, 4, x);

			if (x != origine)
			    put_edit_nnl (51, sxstrget (action_or_prdct_to_ate [erse_ptr->lispro]));

			next = suivant [x];
			prev = 0;

			while ((prev = sxba_scan (next, prev)) > 0) {
			    put_edit (71, cv_x (string, origine + prev));
			}
		    }
		}
	    }

	    sxfree (lecl_list_out_to_be_sorted);
	}
    }


/* sortie des mot-cles */

    if (nbndef > 0) {
	SXINT	*sort_kw /* 1:nbndef */ ;
	SXINT	*tk_defining_kw /* 1:current_token_no+1 */ ;
	SXINT	kwsize, tksize, lecl_list_out_x, lecl_list_out_t, xtk, ner;

	sort_kw = (SXINT*) sxalloc ((SXINT) (nbndef + 1), sizeof (SXINT));
	tk_defining_kw = (SXINT*) sxalloc ((SXINT) (current_token_no + 2), sizeof (SXINT));
	xtk = 0;

	for (ner = 1; ner <= current_token_no; ner++) {
	    lecl_list_out_x = ers_disp [ner].prolon;

	    if (lecl_list_out_x != ers_disp [ner + 1].prolon)
		if (ers [lecl_list_out_x].lispro == -2)
		    tk_defining_kw [++xtk] = ner;
	}


/* on calcule la taille du plus grand mot-cle */

	kwsize = 8;
	lecl_list_out_x = 0;

	for (lecl_list_out_t = 1; lecl_list_out_t <= nbndef; lecl_list_out_t++) {
	    l = sxstrlen (keywords [lecl_list_out_t].string_table_entry);
	    kwsize = (kwsize > l) ? kwsize : l;
	    sort_kw [++lecl_list_out_x] = lecl_list_out_t;
	}


/* on calcule la taille du plus grand nom de token */

	tksize = 0;

	for (lecl_list_out_x = 1; lecl_list_out_x <= xtk; lecl_list_out_x++) {
	    l = strlen (get_re_name (string, tk_defining_kw [lecl_list_out_x]));
	    tksize = (tksize > l) ? tksize : l;
	}


/* on trie les mot-cles par ordre alphabetique */

	sort_by_tree (sort_kw, 1, nbndef, compare_kw);

	{
	    char	**tk /* 0:xtk *//* tksize */ ;

	    tk = (char**) sxalloc ((SXINT) (xtk + 1), sizeof (char*));

	    for (i = 0; i <= xtk; i++)
		tk [i] = (char*) sxalloc ((SXINT) (tksize + 1), sizeof (char));

	    put_edit_ap ("\n");
	    put_edit (21, "K E Y W O R D S    D E F I N I T I O N");
	    put_edit_nl (3);

	    for (lecl_list_out_x = 1; lecl_list_out_x <= xtk; lecl_list_out_x++) {
		strrevcpy (tk [lecl_list_out_x], get_re_name (string, tk_defining_kw [lecl_list_out_x]), tksize);
	    }

	    for (i = 0, s = tk [0]; i < tksize; i++, s++)
		*s = '|';

	    *s = SXNUL;

	    for (i = tksize - 1; i >= 0; i--) {
		k = kwsize + 8;

		if (i == 0)
		    strcpy (line, "code | keywords ");
		else
		    strcpy (line, "                ");

		for (j = 16; j < k; j++)
		    line [j] = ' ';

		for (j = 0; j <= xtk; j++) {
		    line [k++] = tk [j] [i];
		    line [k++] = ' ';
		}

		line [k] = SXNUL;
		put_edit_ap (line);
	    }

	    l = (xtk + 1) * 2 + kwsize + 7;

	    for (i = 0; i < l; i++)
		line [i] = '-';

	    line [l] = SXNUL;
	    put_edit_ap (line);

	    for (i = 1; i <= nbndef; i++) {
		l = sort_kw [i];
		/* traitement tres approximatif des synonymes */
		lecl_list_out_t = keywords [l].t_code;

		if (lecl_list_out_t < 0)
		    lecl_list_out_t = -lecl_list_out_t;
		  /* LINTED this cast from long to int is needed by printf() */
		sprintf (line, "%3ld  | %-*s | ", (SXINT) lecl_list_out_t, (int)kwsize, sxstrget (keywords [l].string_table_entry));
		k = kwsize + 10;

		for (j = 1; j <= xtk; j++) {
		    if (sxba_bit_is_set (RE_to_T [tk_defining_kw [j]], lecl_list_out_t))
			line [k++] = 'X';
		    else
			line [k++] = '.';

		    line [k++] = ' ';
		}

		line [k] = SXNUL;
		put_edit_ap (line);
	    }

	    for (i = xtk; i >= 0; i--)
		sxfree (tk [i]);

	    sxfree (tk);
	}

	sxfree (tk_defining_kw);
	sxfree (sort_kw);
    }

/* context_table_edition: */
    resize = 0;
    min_re = 1;

    for (i = min_re; i <= max_re; i++)
	if (ers_disp [i].prolon != ers_disp [i + 1].prolon) {
	    l = strlen (get_re_name (string, i));
	    resize = (resize > l) ? resize : l;
	}

    context_table_slice_size = (128 - resize) >> 1;
    max_slice_no = (max_re - min_re) / context_table_slice_size + 1;

    for (slice_no = 1; slice_no <= max_slice_no; slice_no++) {
	put_edit_ap ("\n");
	put_edit_nnl (21, "C O N T E X T    T A B L E");

	if (slice_no < max_slice_no)
	    if (slice_no == 1)
		put_edit_ap ("   ( T O   B E   C O N T I N U E D )");
	    else
		put_edit_ap ("   ( C O N T I N U E D )");
	else if (slice_no > 1)
	    put_edit_ap ("   ( E N D )");

	put_edit_nl (3);
	tmin = context_table_slice_size * (slice_no - 1) + min_re;
	tmax = context_table_slice_size * slice_no + min_re - 1;
	tmax = (tmax < max_re) ? tmax : max_re;
	/* on calcule la taille du plus grand nom d'ER */
	maxsize = 0;

	for (i = tmin; i <= tmax; i++)
	    if (ers_disp [i].prolon != ers_disp [i + 1].prolon) {
		l = strlen (get_re_name (string, i));
		maxsize = (maxsize > l) ? maxsize : l;
	    }

	{
	    char	**ren /* tmin-1:tmax *//* maxsize */ ;
	    SXINT		lecl_list_out_i, lecl_list_out_j, lecl_list_out_k, tn, lecl_list_out_x;

	    ren = (char**) sxalloc ((SXINT) (tmax - tmin + 2), sizeof (char*)) + 1;

	    for (lecl_list_out_i = tmin - 1; lecl_list_out_i <= tmax; lecl_list_out_i++)
		ren [lecl_list_out_i - tmin] = (char*) sxalloc ((SXINT) (maxsize + 1), sizeof (char));

	    tn = 1;

	    for (lecl_list_out_x = tmin; lecl_list_out_x <= tmax; lecl_list_out_x++) {
		if (ers_disp [lecl_list_out_x].prolon != ers_disp [lecl_list_out_x + 1].prolon) {
		    tn++;
		    strrevcpy (ren [lecl_list_out_x - tmin], get_re_name (string, lecl_list_out_x), maxsize);
		}
	    }

	    s = ren [- 1];

	    for (lecl_list_out_i = 0; lecl_list_out_i < maxsize; lecl_list_out_i++)
		*s++ = '|';

	    *s = SXNUL;

	    for (lecl_list_out_i = maxsize; lecl_list_out_i >= 1; lecl_list_out_i--) {
		for (lecl_list_out_k = 0; lecl_list_out_k <= resize; lecl_list_out_k++)
		    line [lecl_list_out_k] = ' ';

		for (lecl_list_out_j = tmin - 1; lecl_list_out_j <= tmax; lecl_list_out_j++) {
		    if (ers_disp [lecl_list_out_j].prolon != ers_disp [lecl_list_out_j + 1].prolon) {
			line [lecl_list_out_k++] = ren [lecl_list_out_j - tmin] [lecl_list_out_i - 1];
			line [lecl_list_out_k++] = ' ';
		    }
		}

		line [lecl_list_out_k] = SXNUL;
		put_edit_ap (line);
	    }

	    l = tn * 2 + resize;

	    for (lecl_list_out_i = 0; lecl_list_out_i < l; lecl_list_out_i++)
		line [lecl_list_out_i] = '-';

	    line [lecl_list_out_i] = SXNUL;
	    put_edit_ap (line);

	    for (lecl_list_out_i = min_re; lecl_list_out_i <= max_re; lecl_list_out_i++) {
		if (ers_disp [lecl_list_out_i].prolon != ers_disp [lecl_list_out_i + 1].prolon) {
		  /* LINTED this cast from long to int is needed by printf() */
		    sprintf (line, "%-*s | ", (int)resize, get_re_name (line, lecl_list_out_i));
		    lecl_list_out_k = resize + 3;

		    for (lecl_list_out_j = tmin; lecl_list_out_j <= tmax; lecl_list_out_j++) {
			if (ers_disp [lecl_list_out_j].prolon != ers_disp [lecl_list_out_j + 1].prolon) {
			    if (!sxba_bit_is_set (ctxt [lecl_list_out_i], lecl_list_out_j))
				line [lecl_list_out_k++] = '.';
			    else
				line [lecl_list_out_k++] = 'X';

			    line [lecl_list_out_k++] = ' ';
			}
		    }

		    line [lecl_list_out_k] = SXNUL;
		    put_edit_ap (line);
		}
	    }

	    for (lecl_list_out_i = tmax; lecl_list_out_i >= tmin - 1; lecl_list_out_i--)
		sxfree (ren [lecl_list_out_i - tmin]);

	    sxfree (ren - 1);
	}
    }

    if (is_check || !fsa_done)
	goto close_free;

    if (opt_set & 1) {
	put_edit_ap ("\n");
	put_edit (21, "F I N I T E   S T A T E   A U T O M A T O N");

	for (i = 1; i < xfsa2; i++) {
	    sk = fsa [i].state_kind;
	    put_edit_nl (3);
	    put_edit_nnl (1, "****");
	    put_edit_fnb (8, 3, i);
	    put_edit_nnl (15, "[");

	    if (sk & LA)
		put_edit_apnnl ("la ");

	    if (sk & MIXTE)
		put_edit_apnnl ("mixte ");

	    if (sk & PRDCT)
		put_edit_apnnl ("prdct ");

	    if (sk & ACT)
		put_edit_apnnl ("action ");

	    if (sk & SHIFT)
		put_edit_apnnl ("shift ");

	    if (sk & REDUCE)
		put_edit_apnnl ("reduce ");

	    if (sk & NONDETER)
		put_edit_apnnl ("nondeter ");

	    put_edit_apnnl ("]");
	    put_edit_nnl (31, "[seed_state=");
	    put_edit_apnb (fsa [i].seed_state);
	    put_edit_apnnl (",shift_state=");
	    put_edit_apnb (fsa [i].shift_state);
	    put_edit_apnnl (",else_prdct=");
	    put_edit_apnb (fsa [i].else_prdct);
	    put_edit_apnnl (",del=");
	    put_edit_apnb ((SXINT) fsa [i].del);
	    put_edit_apnnl (",scan=");
	    put_edit_apnb ((SXINT) fsa [i].scan);
	    put_edit_ap ("]");
	    put_edit_nl (1);
	    put_edit_nnl (10, "index");
	    put_edit_nnl (16, "origine");
	    put_edit_nnl (24, "ctxt_no");
	    put_edit (32, "kind");

	    for (j = fsa [i].item_ref; j < fsa [i + 1].item_ref; j++) {
		put_edit_nnl (9, "[");
		put_edit_nb (11, item [j].index);
		put_edit_apnnl (",");
		put_edit_nb (18, item [j].attributes.origine_index);
		put_edit_apnnl (",");
		put_edit_nb (26, item [j].attributes.ctxt_no);
		put_edit_apnnl (",");
		put_edit_nb (33, (SXINT) item [j].attributes.kind);
		put_edit_ap ("]");
	    }

	    put_edit_nl (1);

	    for (j = fsa [i].transition_ref; j < fsa [i + 1].transition_ref; j++) {
		k = fsa_trans [j].cc_ref;

		if (k != 0) {
		    if (sk & NONDETER)
			put_edit_nnl (15, "Any");
		    else if (k < 0 || (k > 0 && (sk & PRDCT))) {
			char	*lecl_list_out_s = sxstrget (action_or_prdct_to_ate [k]);

			if (*(lecl_list_out_s + 1) == 'P') {
			    /* @Put */
			    put_edit_nnl (15, "@Put (\"");
			    put_edit_apnnl (SXCHAR_TO_STRING (*(lecl_list_out_s + 6)));
			    put_edit_apnnl ("\")");
			}
			else
			    put_edit_nnl (15, lecl_list_out_s);
		    }
		    else {
			varstr_raz (varstr_ptr);
			put_edit_nnl (15, lecl_gen_cc (sc_to_char_set, compound_classes [k], varstr_ptr)->first);
		    }

		    sprintf (line, " %c %c => ",fsa_trans [j].is_del ? '-' : ' ', fsa_trans [j].is_scan ? '*' : ' ');
		    put_edit_nnl (41, line);
		    x = fsa_trans [j].next_state_no;

		    if (x < 1)
			sprintf (line, "%s%s", get_master_name (string, -x), ers [ers_disp [-x].prolon].lispro == -2 ? "+THSH" : "");
		    else
			sprintf (line, "%-ld", x);

		    put_edit_ap (line);
		}
	    }
	}

	put_edit_nl (1);
    }

    put_edit_ap ("\n");
    put_edit (31, "S I M P L E    C L A S S E S");
    put_edit_nl (3);
    put_edit_nnl (9, "No");
    put_edit (31, "Character Set");
    put_edit_nl (2);
    put_edit_nnl (9, "1 (Illegal Chars)");

    if (!sxba_is_empty (new_sc_to_char_set [1])) {
	varstr_raz (varstr_ptr);
	put_edit (31, lecl_gen_sc (new_sc_to_char_set, 1, varstr_ptr)->first);
    }

    put_edit (9, "2 (Eof)");

    for (i = 3; i <= Smax; i++) {
	put_edit_nb (9, i);
	varstr_raz (varstr_ptr);
	put_edit (31, lecl_gen_sc (new_sc_to_char_set, i, varstr_ptr)->first);
    }

    put_edit_ap ("\n");
    put_edit (21, "G E N E R A T E D   C O D E");
    put_edit_nl (2);

    if (has_nondeterminism)
	put_edit (5, "The following automaton is nondeterministic.");

    if (max_la == -1)
	put_edit_nnl (5, "An unbounded number of");
    else
	put_edit_nb (5, max_la);

    put_edit_apnnl (" character");

    if (max_la == -1 || max_la > 1)
	put_edit_apnnl ("s");

    put_edit_apnnl (" of look-ahead ");

    if (max_la > 1)
	put_edit_apnnl ("are");
    else
	put_edit_apnnl ("is");

    put_edit_ap (" needed in the scanner.");
    put_edit_nl (2);
    sc_to_sc_set = sxbm_calloc (Smax + 1, Smax + 1);
    to_be_sorted = (SXINT*) sxalloc (Smax + 1, sizeof (SXINT));

    for (state_no = 1; state_no <= last_state_no; state_no++) {
	transition_matrix_line = transition_matrix [state_no];
	sprintf (line, "**** STATE %-ld ****", (SXINT) state_no);
	put_edit (1, line);

	for (i = 1; i <= Smax; i++) {
	    to_be_sorted [i] = i;
	}

	equality_sort (to_be_sorted, 1, Smax, less_equal, equal);

	for (i = 1; i <= Smax; i++) {
	    SXBA_1_bit (sc_to_sc_set [to_be_sorted [i]], i);
	}

	is_error = false;

	for (i = 1; i <= Smax; i++) {
	    sc_set = sc_to_sc_set [i];

	    if (!sxba_is_empty (sc_set)) {
		stmt = transition_matrix_line [i];

		if (CODOP (stmt) != Error) {
		    /* not an error */
		    varstr_raz (varstr_ptr);
		    put_edit_nnl (11, lecl_gen_cc (new_sc_to_char_set, sc_set, varstr_ptr)->first);
		    put_body (stmt);
		}
		else
		    is_error = true;

		sxba_empty (sc_set);
	    }
	}

	if (is_error) {
	    put_edit_nnl (11, "Else");
	    put_body (transition_matrix_line [1]) /* illegal chars */ ;
	}

	put_edit_nl (1);
    }

    sxfree (to_be_sorted);
    sxbm_free (sc_to_sc_set);

    if (xprod > 0) {
	put_edit_ap ("\n");
	put_edit (1, "A C T I O N S    A N D    P R E D I C A T E S    P R O C E S S I N G    C O D E");

	if (has_nondeterminism) {
	    put_edit (37, "A N D");
	    put_edit (11, "N O N    D E T E R M I N I S M    S E Q U E N C E S");
	}

	put_edit_nl (2);

	for (x = 1; x <= xprod; x++) {
	    fe = action_or_prdct_code [x];
	    put_edit_nl (1);
	    
	    switch (fe.kind) {
	    case IsPredicate:
		put_edit_nnl (1, "PRDCT ");
		break;

	    case IsAction:
		put_edit_nnl (1, "ACTION ");
		break;

	    case IsNonDeter:
		put_edit_nnl (1, "NONDETER ");
		break;
	    default:
#if EBUG
	      sxtrap("lecl_lo","unknown switch case #5");
#endif
	      break;
	    }

	    put_edit_apnb (x);
	    put_edit_nl (1);
	    put_edit_fnb (1, 3, x);
	    put_edit_nnl (5, ":");
	    put_edit_nnl (11, gen_action_or_prdct_name_from_fe (string, fe));
	    put_body (fe.stmt);

	    if (fe.kind != IsAction)
		/* predicate or Nondeterminism sequence */
		while (fe.action_or_prdct_no != 0) {
		    fe = action_or_prdct_code [++x];
		    put_edit_fnb (1, 3, x);
		    put_edit_nnl (5, ":");
		    put_edit_nnl (11, gen_action_or_prdct_name_from_fe (string, fe));
		    put_body (fe.stmt);
		}
	}
    }

/* hash_ft_output: */
    put_edit_ap ("\n");
    put_edit (11, "C H E C K    K E Y W O R D S");
    put_edit_nl (2);

    if (nbndef == 0) {
	sprintf (line, "There is no keyword in %s.", prgentname);
	put_edit (11, line);
    }
    else
	put_edit_ap (varstr_tostr (check_kw));

close_free:
    varstr_free (varstr_ptr);
    put_edit_finalize ();
}
