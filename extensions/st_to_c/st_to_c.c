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

#define SX_DFN_EXT_VAR_LECL

/* Le 24/01/2003
   Pour avoir les bonnes macros pour STMT, KEEP, SCAN, CODOP, NEXT ... */
#define SXS_MAX (SHRT_MAX+1)
#include "sxversion.h"
#include "varstr.h"
#include "sxunix.h"
#include "sxsstmt.h"
#include "S_tables.h"
#include "put_edit.h"

char WHAT_LECLSTTOC[] = "@(#)SYNTAX - $Id: st_to_c.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

extern SXINT	equality_sort (SXINT *t, SXINT bi, SXINT bs, bool (*less_equal) (SXINT, SXINT), bool (*equal) (SXINT, SXINT));
extern void     lecl_free(struct lecl_tables_s *lecl_tables_ptr);
extern bool  lecl_read (struct lecl_tables_s *lecl_tables_ptr, char *langname);

#define max(x,y) ((x)<(y)) ? (y) : (x)


static void	gen_prdct_seq (SXINT state_no, SXINT y);
static void	gen_state (SXINT state_no);
/* les codop sont reperes de droite a gauche, le codop (fictif) 0 correspond 
   au bit de droite */

static bool	is_shift [] = {false, true, true, false, false, false, false, true, true, false};
static bool	is_hash_reduce [] = {false, false, false, true, false, true, false, false, false, false};

#define char_max 256

#define max_state_no lecl_tables.S_tbl_size.S_last_state_no

static SXBA	/* char_max */ char_set = NULL;
static struct lecl_tables_s	lecl_tables;
static VARSTR	string;
static char	*prgentname;
static SXINT	*ref_no /* 1:S_last_state_no */ ;
static SXBA	is_ref, scan_in_la, scan, keep_and_scan, first_scan_in_la /* S_last_state_no */ ;
static SXBA	is_ts_empty /* max_state_no + S_xprod */ ;
static SXINT	*to_be_sorted /* 1:S_last_simple_class_no */ ;
static transition_matrix_s	stmt;
static transition_matrix_s	*transition_matrix_line;
static struct action_or_prdct_code_s	fe;
static SXINT	i, j;
static bool	is_erase, is_include, is_mark, is_release, is_dummy_hash, is_user_action_or_prdct,
  is_la, is_first_col, is_last_col, is_stmt_5_0, is_post_action, is_user_prdct, is_keep_shr, is_shr, is_keep_sr, is_sr, 
  is_ascii, is_keep_sr_to_comments, is_sr_to_comments;
static SXBA	/* S_last_char_code+1 */ *sc_to_char_set
		/* 1:S_last_simple_class_no */
							;
static SXINT	nbndef, max_t_code_name_lgth;




/*
  
   etiq	codop	test	del	scan	goto
  
   A	0	T|&0			k      Si k = 1 => look_ahead state
   A	1	T		*	A
   A	2	T	-	*	A
   A	3	T|&n		*	B
   A	4	T|&n	-	*	B
   A	5	T|&n			=> TER
   A	6	T|&n			=> TER+THSH
   A	7	T|&n		*	=> TER
   A	8	T|&n		*	=> TER+THSH
   A	9	T|&n	-	*	=> TER
   A	10	T|&n	-	*	=> TER+THSH
   A	11	T|&n		**	B	1ere transition
   A	12	T|&n		**	B	suivantes
   A	13	T|&n			B	goto predicate sequence
   A	14	T|&n			B	goto action
   A	15	T|&n		*	B	goto action
   A	16	T|&n	-	*	B	goto action
   A	17	@n			B	B est un etat
   A	18	@n			B	goto action
   A	19	T		*	A	traite par search
   A	20	T	-	*	A	traite par search
   A	21	@n	 		=> TER
   A	22	@n			=> TER+THSH
   A	23	@n (Post-Action)		=> TER
   A	24	@n (Post-Action)		=> TER+THSH
  
*/


static void	out_S_char_to_simple_class (void)
{
    SXINT	out_S_char_to_simple_class_j, out_S_char_to_simple_class_i;
    SXINT	d, nbt;
    unsigned char	sctsc [128 + 1 + 255] /* -128:255 */ ;

#define slice_length	25

    if (lecl_tables.S_tbl_size.S_last_char_code > 255) {
	lecl_tables.S_tbl_size.S_last_char_code = 255;
    }

    for (out_S_char_to_simple_class_i = lecl_tables.S_tbl_size.S_last_char_code; out_S_char_to_simple_class_i >= 0; out_S_char_to_simple_class_i--)
	(sctsc + 128) [out_S_char_to_simple_class_i] = lecl_tables.S_char_to_simple_class [out_S_char_to_simple_class_i];

    for (out_S_char_to_simple_class_i = lecl_tables.S_tbl_size.S_last_char_code + 1; out_S_char_to_simple_class_i <= 255; out_S_char_to_simple_class_i++)
	(sctsc + 128) [out_S_char_to_simple_class_i] = 1 /* Class of Illegal Char */ ;

    for (out_S_char_to_simple_class_i = 128; out_S_char_to_simple_class_i <= 255; out_S_char_to_simple_class_i++)
	(sctsc - 128) [out_S_char_to_simple_class_i] = (sctsc + 128) [out_S_char_to_simple_class_i];

    (sctsc + 128) [EOF] = 2 /* EOF class */ ;

    puts ("\nstatic unsigned char S_char_to_simple_class[]={");
    nbt = (sizeof (sctsc) / sizeof (unsigned char)) / slice_length;
    d = 0;

    for (out_S_char_to_simple_class_i = 1; out_S_char_to_simple_class_i <= nbt; out_S_char_to_simple_class_i++) {
	for (out_S_char_to_simple_class_j = d, d += slice_length; out_S_char_to_simple_class_j < d; out_S_char_to_simple_class_j++) {
	    printf ("%d,", sctsc [out_S_char_to_simple_class_j]);
	}

	putchar (SXNEWLINE);
    }

    for (out_S_char_to_simple_class_j = d; (SXUINT)out_S_char_to_simple_class_j < (sizeof (sctsc) / sizeof (unsigned char)); out_S_char_to_simple_class_j++) {
	printf ("%d,", sctsc [out_S_char_to_simple_class_j]);
    }

    puts ("};");
}



static VARSTR	gen_sc_name (SXBA gen_sc_name_char_set, VARSTR varstr_ptr)
{
    SXINT		gen_sc_name_i, gen_sc_name_j, gen_sc_name_l;
    bool	is_first, is_slice;

    is_slice = false;
    is_first = true;

    for (gen_sc_name_i = sxba_scan (gen_sc_name_char_set, 0); gen_sc_name_i != -1; gen_sc_name_i = sxba_scan (gen_sc_name_char_set, gen_sc_name_i)) {
	gen_sc_name_l = sxba_0_lrscan (gen_sc_name_char_set, gen_sc_name_i);

	if (gen_sc_name_l == -1)
	    gen_sc_name_l = char_max - gen_sc_name_i + 1;
	else
	    gen_sc_name_l -= gen_sc_name_i;

	if (gen_sc_name_l > 7) {
	    /* On genere une slice */
	    if (is_first)
		is_first = false;
	    else {
		if (!is_slice)
		    varstr_catenate (varstr_ptr, "\"");

		varstr_catenate (varstr_ptr, " + ");
	    }

	    varstr_catenate (varstr_catenate (varstr_catenate (varstr_catenate (varstr_catenate (varstr_ptr, "\""),
		 SXCHAR_TO_STRING (gen_sc_name_i - 1)), "\"..\""), SXCHAR_TO_STRING (gen_sc_name_i + gen_sc_name_l - 2)), "\"");
	    is_slice = true;
	}
	else {
	    if (is_slice)
		varstr_catenate (varstr_ptr, " + \"");
	    else if (is_first) {
		varstr_catenate (varstr_ptr, "\"");
		is_first = false;
	    }

	    for (gen_sc_name_j = 0; gen_sc_name_j < gen_sc_name_l; gen_sc_name_j++)
		varstr_catenate (varstr_ptr, SXCHAR_TO_STRING (gen_sc_name_i + gen_sc_name_j - 1));

	    is_slice = false;
	}

	gen_sc_name_i += gen_sc_name_l - 1;
    }

    if (!is_slice)
	varstr_catenate (varstr_ptr, "\"");

    return varstr_ptr;
}


static VARSTR	gen_cc_name (SXBA *SC_TO_CHAR_SET, SXBA sc_set, VARSTR varstr_ptr)
{
    SXINT		gen_cc_name_i;
    bool	not_empty;

    if (char_set == NULL)
	char_set = sxba_calloc (char_max + 1);
    else
	sxba_empty (char_set);

    for (gen_cc_name_i = sxba_scan (sc_set, 0); gen_cc_name_i != -1; gen_cc_name_i = sxba_scan (sc_set, gen_cc_name_i)) {
	sxba_or (char_set, SC_TO_CHAR_SET [gen_cc_name_i]);
    }

    not_empty = !sxba_is_empty (char_set);

    if (sxba_bit_is_set (sc_set, 2) /* EOF */ ) {
	varstr_catenate (varstr_ptr, "Eof");

	if (not_empty)
	    varstr_catenate (varstr_ptr, " + ");
    }

    if (not_empty)
	gen_sc_name (char_set, varstr_ptr);

    return varstr_ptr;
}



static bool	end_of_token (SXINT xprod)
{
    for (;;) {
	switch (CODOP (lecl_tables.SXS_action_or_prdct_code [xprod].stmt)) {
	case ActPrdct:
	    xprod = NEXT (lecl_tables.SXS_action_or_prdct_code [xprod].stmt);
	    break;

	case Reduce:
	case HashReduce:
	case ReducePost:
	case HashReducePost:
	    return true;

	default:
	    return false;
	}
    }
}



static void	goto_state (SXINT state_no, bool is_keep, bool is_scan)
{
    if (ref_no [state_no] == 1)
	gen_state (state_no);
    else {
	put_edit_apnnl ("goto STATE_");
	put_edit_apnb (state_no);

	if (is_keep)
	    put_edit_apnnl ("_keep_and_scan");
	else if (is_scan)
	    put_edit_apnnl ("_scan");

	put_edit_ap (";");
    }
}



static void	reduce (SXINT tok, bool is_keep, bool is_scan, bool is_hash, bool is_empty, bool reduce_is_post_action)
{
    if (tok == 0 && is_empty /* plus precis que lecl_tables.S_tbl_size.S_are_comments_erased */	 && !is_hash)
	goto_state ((SXINT)1, is_keep, is_scan);
    else {
	if (tok != 0) {
	    char	t [65];

	    put_edit_apnnl ("sxsvar.sxlv.terminal_token.lahead = ");
	    put_edit_apnb (tok);
	    put_edit_apnnl (" /* ");

	    if (is_hash)
		put_edit_apnnl ("HASH + ");

	    strncpy (t, lecl_tables.S_terlis + lecl_tables.SXS_adrp [tok].xterlis, lecl_tables.SXS_adrp [tok].lgth);
	    t [lecl_tables.SXS_adrp [tok].lgth] = SXNUL;
	    put_edit_apnnl (t);
	    put_edit_ap (" */;");
	}

	if (is_empty && !is_keep && !is_hash) {
	    if (is_scan) {
		put_edit_ap ("sxnextchar ();");
	    }

	    if (reduce_is_post_action)
		put_edit_ap ("goto post_action;");
	    else
		put_edit_ap ("goto done;");
	}
	else {
	    put_edit_apnnl ("goto ");

	    if (is_keep)
		put_edit_apnnl ("keep_");

	    if (is_scan)
		put_edit_apnnl ("scan_");

	    if (is_hash)
		put_edit_apnnl ("hash_");

	    put_edit_apnnl ("reduce");

	    if (tok == 0 && !is_hash)
		put_edit_apnnl ("_comments");

	    put_edit_ap (";");
	}
    }
}



static void	gen_error (SXINT state_no)
{
    put_edit_apnnl ("if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.S_recovery) (SXACTION, ");
    put_edit_apnb (state_no);
    put_edit_apnnl (", &current_class");

    if (NEXT (lecl_tables.S_transition_matrix [state_no] [1]) != 0)
	put_edit_apnnl ("_in_la");
	
    put_edit_ap (")) {");

    put_edit_apnnl ("goto STATE_");
    put_edit_apnb (state_no);
    put_edit_ap (";}\nreturn;");
}



static	void gen_label (bool is_case, SXBA sc_set)
{
    SXINT	gen_label_i, gen_label_j;
    char	*s, *t, c;

    put_edit_nl (1);

    if (is_case)
	for (gen_label_j = 0, gen_label_i = sxba_scan (sc_set, 0); gen_label_i != -1; gen_label_i = sxba_scan (sc_set, gen_label_i)) {
	    gen_label_j++;

	    if (gen_label_j == 10) {
		put_edit_nl (1);
		gen_label_j = 0;
	    }

	    put_edit_apnnl ("case ");
	    put_edit_apnb (gen_label_i);
	    put_edit_apnnl (": ");
	}
    else
	put_edit_apnnl ("default: ");

    varstr_raz (string);
    string = gen_cc_name (sc_to_char_set, sc_set, string);
    t = s = varstr_tostr (string);
    put_edit_apnnl ("/* ");

    for (c = *s++; c != SXNUL; c = *s++)
	if (c == '*' && *s == '/') {
	    *s = SXNUL;
	    put_edit_apnnl (t);
	    put_edit_apnnl ("\" + \"");
	    *s = '/';
	    put_edit_apnnl (s);
	    put_edit_ap (" */");
	    return;
	}

    put_edit_apnnl (t);
    put_edit_ap (" */");
}



static void	gen_body (SXINT state_no, bool is_empty, transition_matrix_s gen_body_stmt)
{
    bool	is_keep, is_scan, is_ts_null_done;
    SXINT		codop, next;

    is_ts_null_done = false;

goto_field:
    is_keep = KEEP (gen_body_stmt) != 0;
    is_scan = SCAN (gen_body_stmt) != 0;
    codop = CODOP (gen_body_stmt);
    next = NEXT (gen_body_stmt);
    is_empty = is_empty && !is_keep;

    switch (codop) {
    case SameState:
    case State:
    case FirstLookAhead:
    case NextLookAhead:
	goto_state (next, is_keep, is_scan);
	break;

    case Reduce:
	reduce (next, is_keep, is_scan, false, is_empty, false);
	break;

    case HashReduce:
	reduce (next, is_keep, is_scan, true, is_empty, false);
	break;

    case ActPrdct:
	fe = lecl_tables.SXS_action_or_prdct_code [next];

	if (fe.kind == IsPredicate) {
	    gen_prdct_seq (state_no, next);
	}
	else {
	    /* SXACTION */
	    codop = CODOP (fe.stmt);

		if (is_keep) {
		    put_edit_ap ("put_a_char (sxsrcmngr.current_char);");
		}
		
		if (is_scan) {
		    if (end_of_token (next)) {
			put_edit_ap ("sxnextchar ();");
		    }
		    else {
			put_edit_ap ("current_class = char_to_class (sxnextchar ());");
		    }
		}
		
		if (fe.is_system)
		    switch (fe.action_or_prdct_no) {
		    case LowerToUpper:
			put_edit_ap ("/* Lower_To_Upper (last char) */");
			put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) { unsigned char last_char = sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1];");
			put_edit_ap ("sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1] = sxtoupper (last_char);");
			put_edit_ap ("}");
			break;
			
		    case UpperToLower:
			put_edit_ap ("/* Upper_To_Lower (last char) */");
			put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) { unsigned char last_char = sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1];");
			put_edit_ap ("sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1] = sxtolower (last_char);");
			put_edit_ap ("}");
			break;
			
		    case Set:
			put_edit_apnnl ("if (sxsvar.sxlv.mode.with_system_act) sxsvar.sxlv_s.counters [");
			put_edit_apnb (fe.param);
			put_edit_ap ("] = 1;");
			break;
			
		    case Reset:
			put_edit_apnnl ("if (sxsvar.sxlv.mode.with_system_act) sxsvar.sxlv_s.counters [");
			put_edit_apnb (fe.param);
			put_edit_ap ("] = 0;");
			break;
			
		    case Erase:
			if (is_mark) {
			    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) {");
			    put_edit_ap ("if (sxsvar.sxlv.ts_lgth > sxsvar.sxlv.mark.index)");
			    put_edit_ap ("sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index == -1 ? 0 : sxsvar.sxlv.mark.index;");
			    put_edit_ap ("sxsvar.sxlv.mark.index = -1 /* no more Mark */;\n}");
			}
			else {
			    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) sxsvar.sxlv.ts_lgth = 0;");
			}
			
			break;
			
		    case Include:
			fprintf (sxstderr, "\"@Include\" is not yet implemented.");
			break;
			
		    case UpperCase:
			put_edit_ap ("/* Upper_Case (token_string) */");
			put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) { unsigned char a_char;");
			put_edit_ap ("SXINT i;");
			put_edit_ap ("for (i=0;i<sxsvar.sxlv.ts_lgth;i++) {");
			put_edit_apnnl ("a_char = sxsvar.sxlv_s.token_string [i])");
			put_edit_ap ("sxsvar.sxlv_s.token_string [i] = sxtoupper (a_char);");
			put_edit_ap ("}");
			put_edit_ap ("}");
			break;
			
		    case LowerCase:
			put_edit_ap ("/* Lower_Case (token_string) */");
			put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) { unsigned char a_char;");
			put_edit_ap ("SXINT i;");
			put_edit_ap ("for (i=0;i<sxsvar.sxlv.ts_lgth;i++) {");
			put_edit_apnnl ("a_char = sxsvar.sxlv_s.token_string [i])");
			put_edit_ap ("sxsvar.sxlv_s.token_string [i] = sxtolower (a_char);");
			put_edit_ap ("}");
			put_edit_ap ("}");
			break;
			
		    case Put:
			put_edit_apnnl ("if (sxsvar.sxlv.mode.with_system_act) put_a_char ('");
			put_edit_apnnl (SXCHAR_TO_STRING (fe.param));
			put_edit_ap ("');");
			break;
			
		    case Mark:
			/* Always do it, for the user's benefit */
			put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) {"); 
			put_edit_ap ("sxsvar.sxlv.mark.source_coord = sxsrcmngr.source_coord;");
			put_edit_ap ("sxsvar.sxlv.mark.index = sxsvar.sxlv.ts_lgth;");
			put_edit_ap ("sxsvar.sxlv.mark.previous_char = sxsrcmngr.previous_char;");
			put_edit_ap ("}");
			break;
			
		    case Release:
			if (is_mark) {
			    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) {");
			    put_edit_ap ("if (sxsvar.sxlv.ts_lgth >= sxsvar.sxlv.mark.index) {");
			    
			    if (!is_ts_null_done) {
				put_edit_ap ("ts_null ();");
				is_ts_null_done = true;
			    }
			    
			    put_edit_ap ("if (sxsvar.sxlv.mark.index == -1) {");
			    put_edit_ap ("sxsrcpush (sxsvar.sxlv.previous_char,");
			    put_edit_ap ("&(sxsvar.sxlv_s.token_string[sxsvar.sxlv.ts_lgth = 0]),");
			    put_edit_ap ("sxsvar.sxlv.terminal_token.source_index);");
			    put_edit_ap ("}");
			    put_edit_ap ("else {");
			    put_edit_ap ("sxsrcpush (sxsvar.sxlv.mark.previous_char,");
			    put_edit_ap ("&(sxsvar.sxlv_s.token_string[sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index]),");
			    put_edit_ap ("sxsvar.sxlv.mark.source_coord);");
			    put_edit_ap ("}");
			    put_edit_ap ("current_class = char_to_class (sxsrcmngr.current_char);");
			    put_edit_ap ("}");
			    put_edit_ap ("sxsvar.sxlv.mark.index = -1 /* no more Mark */;");
			    put_edit_ap ("}");
			}
			else {
			    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) {");
			    if (!is_ts_null_done) {
				put_edit_ap ("ts_null ();");
				is_ts_null_done = true;
			    }
			    
			    put_edit_ap ("sxsrcpush (sxsvar.sxlv.previous_char,");
			    put_edit_ap ("&(sxsvar.sxlv_s.token_string[sxsvar.sxlv.ts_lgth = 0]),");
			    put_edit_ap ("sxsvar.sxlv.terminal_token.source_index);");
			    put_edit_ap ("current_class = char_to_class (sxsrcmngr.current_char);");
			    put_edit_ap ("}");
			}
			
			break;
			
		    case Incr:
			put_edit_apnnl ("if (sxsvar.sxlv.mode.with_system_act) sxsvar.sxlv_s.counters [");
			put_edit_apnb (fe.param);
			put_edit_ap ("]++;");
			break;
			
		    case Decr:
			put_edit_apnnl ("if (sxsvar.sxlv.mode.with_system_act) sxsvar.sxlv_s.counters [");
			put_edit_apnb (fe.param);
			put_edit_ap ("]--;");
			break;
		    default:
#if EBUG
		      sxtrap("st_to_c","unknown switch case #1");
#endif
		      break;
		    }
		else 
		    if (codop == ReducePost || codop == HashReducePost)
		    {
			put_edit_apnnl ("post_action_no = ");
			put_edit_apnb (fe.action_or_prdct_no);
			put_edit_ap (";");
		    }
		    else {
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) {");
		    if (!is_ts_null_done) {
			put_edit_ap ("ts_null ();");
			is_ts_null_done = true;
		    }
		    
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_user_act)");
		    put_edit_apnnl ("(*sxsvar.SXS_tables.S_scanact) (SXACTION, ");
		    put_edit_apnb (fe.action_or_prdct_no);
		    put_edit_ap (");");
		    put_edit_ap ("current_class = char_to_class (sxsrcmngr.current_char) /* for tricky cases */;");
		    put_edit_ap ("}");
		}

	    is_empty = sxba_bit_is_set (is_ts_empty, max_state_no + next);
	    gen_body_stmt = fe.stmt;
	    goto goto_field;
	}
	
	break;
	
    case ReducePost:
	reduce (next, is_keep, is_scan, false, is_empty, true);
	break;
	
    case HashReducePost:
	reduce (next, is_keep, is_scan, true, is_empty, true);
	break;
    default:
#if EBUG
      sxtrap("st_to_c","unknown switch case #2");
#endif
      break;
    }
}



static	void gen_prdct_seq (SXINT state_no, SXINT y)
{
    SXINT		x;
    bool	is_empty;
    bool	is_in_la = NEXT (lecl_tables.S_transition_matrix [state_no] [1]) != 0;

    sxinitialise (is_empty);
    x = y;
    fe = lecl_tables.SXS_action_or_prdct_code [x];

    while (fe.action_or_prdct_no != 0 || !fe.is_system) {
        is_empty = sxba_bit_is_set (is_ts_empty, max_state_no + x);

	if (fe.is_system)
	    switch (fe.action_or_prdct_no) {
	    case IsFirstCol:
		if (is_in_la)
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_prdct && sxsrcmngr.buffer[sxsrcmngr.labufindex-1] == '\\n') {");
		else
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_prdct && sxsrcmngr.previous_char == '\\n') {");

		gen_body (state_no, is_empty, fe.stmt);
		break;

	    case IsSet:
		put_edit_apnnl ("if (sxsvar.sxlv.mode.with_system_prdct && sxsvar.sxlv_s.counters [");
		put_edit_apnb (fe.param);
		put_edit_ap ("] != 0) {");
		gen_body (state_no, is_empty, fe.stmt);
		break;

	    case IsReset:
		put_edit_apnnl ("if (sxsvar.sxlv.mode.with_system_prdct && sxsvar.sxlv_s.counters [");
		put_edit_apnb (fe.param);
		put_edit_ap ("] == 0) {");
		gen_body (state_no, is_empty, fe.stmt);
		break;

	    case NotIsFirstCol:
		if (is_in_la)
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_prdct && sxsrcmngr.buffer[sxsrcmngr.labufindex-1] != '\\n') {");
		else
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_prdct && sxsrcmngr.previous_char != '\\n') {");

		gen_body (state_no, is_empty, fe.stmt);
		break;

	    case IsLastCol:
		put_edit_ap ("{SXINT sxchar;");

		if (is_in_la)
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_prdct && ((sxchar = sxlanext_char ()) == '\\n' || sxchar == EOF)) {");
		else
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_prdct && ((sxchar = sxlafirst_char ()) == '\\n' || sxchar == EOF)) {\nsxlaback (1);");

		gen_body (state_no, is_empty, fe.stmt);
		put_edit_ap ("}");
		break;

	    case NotIsLastCol:
		put_edit_ap ("{SXINT sxchar;");

		if (is_in_la)
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_prdct && ((sxchar = sxlanext_char ()) != '\\n' && sxchar != EOF)) {");
		else
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_prdct && ((sxchar = sxlafirst_char ()) != '\\n' && sxchar != EOF)) {\nsxlaback (1);");

		gen_body (state_no, is_empty, fe.stmt);
		put_edit_ap ("}");
		break;
	    default:
#if EBUG
	      sxtrap("st_to_c","unknown switch case #3");
#endif
	      break;
	    }
	else {
	    put_edit_apnnl ("if (ts_null (), sxsvar.sxlv.current_state_no = ");
	    put_edit_apnb (state_no);
	    put_edit_apnnl (", sxsvar.sxlv.mode.with_user_prdct &&");

	    if (fe.param !=0)
		put_edit_apnnl ("!");

	    put_edit_apnnl ("(*sxsvar.SXS_tables.S_scanact) (SXPREDICATE, ");
	    put_edit_apnb (fe.action_or_prdct_no);
	    put_edit_apnnl (")) {");
	    gen_body (state_no, is_empty, fe.stmt);
	}

	put_edit_ap ("}");
	x++;
	fe = lecl_tables.SXS_action_or_prdct_code [x];
    }

    if (CODOP (fe.stmt) == Error)
	gen_error (state_no);
    else
	gen_body (state_no, is_empty, fe.stmt);
}



static bool	less_equal (SXINT less_equal_i, SXINT less_equal_j)
{
    return transition_matrix_line [less_equal_i] <= transition_matrix_line [less_equal_j];
}



static bool	equal (SXINT equal_i, SXINT equal_j)
{
    return transition_matrix_line [equal_i] == transition_matrix_line [equal_j];
}



static void	gen_state (SXINT state_no)
{
    bool	is_error, is_mref, is_empty;
    SXINT	gen_state_i;
    transition_matrix_s		*tml;
    SXBA	/* S_last_simple_class_no */ *sc_to_sc_set /* 1:S_last_simple_class_no */ ;
    SXBA	/* S_last_simple_class_no */ sc_set, sc1_set;

    sc_to_sc_set = sxbm_calloc (lecl_tables.S_tbl_size.S_last_simple_class_no + 1, lecl_tables.S_tbl_size.
	 S_last_simple_class_no + 1);
    sc1_set = sxba_calloc (lecl_tables.S_tbl_size.S_last_simple_class_no + 1);
    tml = transition_matrix_line = lecl_tables.S_transition_matrix [state_no];
    is_mref = (ref_no [state_no] != 1);
    is_empty = sxba_bit_is_set (is_ts_empty, state_no);
    
    put_edit_nl (1);
    put_edit_ap ("/**************************************************************************/");
    put_edit_nl (1);

    if (sxba_bit_is_set (keep_and_scan, state_no)) {
	if (is_mref) {
	    put_edit_apnnl ("STATE_");
	    put_edit_apnb (state_no);
	    put_edit_ap ("_keep_and_scan:");
	}

	put_edit_ap ("put_a_char (sxsrcmngr.current_char);");
    }

    if (sxba_bit_is_set (scan, state_no)) {
	if (is_mref && !sxba_bit_is_set (keep_and_scan, state_no)) {
	    put_edit_apnnl ("STATE_");
	    put_edit_apnb (state_no);
	    put_edit_ap ("_scan:");
	}

	put_edit_ap ("current_class = char_to_class (sxnextchar ());");
    }
    else if (sxba_bit_is_set (scan_in_la, state_no)) {
	if (is_mref && !sxba_bit_is_set (keep_and_scan, state_no)) {
	    put_edit_apnnl ("STATE_");
	    put_edit_apnb (state_no);
	    put_edit_ap ("_scan:");
	}

	if (sxba_bit_is_set (first_scan_in_la, state_no))
	    put_edit_ap ("current_class_in_la = char_to_class (sxlafirst_char ());");
	else
	    put_edit_ap ("current_class_in_la = char_to_class (sxlanext_char ());");
    }

    put_edit_apnnl ("STATE_");
    put_edit_apnb (state_no);
    put_edit_ap (":");

    if (state_no == 1) {
	put_edit_ap ("sxsvar.sxlv.terminal_token.source_index = sxsrcmngr.source_coord;");

	if (is_mark)
	    put_edit_ap ("sxsvar.sxlv.mark.index = -1 /* no explicit Mark yet */ ;");

	if (is_release)
	    put_edit_ap ("sxsvar.sxlv.previous_char = sxsrcmngr.previous_char;");
    }

    for (gen_state_i = 1; gen_state_i <= lecl_tables.S_tbl_size.S_last_simple_class_no; gen_state_i++) {
	to_be_sorted [gen_state_i] = gen_state_i;
    }

    equality_sort (to_be_sorted, 1, lecl_tables.S_tbl_size.S_last_simple_class_no, less_equal, equal);

    for (gen_state_i = 1; gen_state_i <= lecl_tables.S_tbl_size.S_last_simple_class_no; gen_state_i++) {
	sxba_1_bit (sc_to_sc_set [to_be_sorted [gen_state_i]], gen_state_i);
    }

    is_error = false;
    put_edit_ap ("#ifdef LDBG");
    put_edit_apnnl ("printf (\"state_no = %ld, current_char = '%s', current_class = %d\\n\", ");
    put_edit_apnb (state_no);
    put_edit_apnnl (", SXCHAR_TO_STRING (sxsrcmngr.current_char), ");

    if (sxba_bit_is_set (scan_in_la, state_no))
	put_edit_ap ("current_class_in_la);");
    else
	put_edit_ap ("current_class);");

    put_edit_ap ("#endif");

    if (sxba_bit_is_set (scan_in_la, state_no))
	put_edit_ap ("switch (current_class_in_la) {");
    else
	put_edit_ap ("switch (current_class) {");

    for (gen_state_i = 1; gen_state_i <= lecl_tables.S_tbl_size.S_last_simple_class_no; gen_state_i++) {
	sc_set = sc_to_sc_set [gen_state_i];

	if (!sxba_is_empty (sc_set)) {
	    stmt = tml [gen_state_i];

	    if (CODOP (stmt) != Error) {
		gen_label (true, sc_set);
		sxba_or (sc1_set, sc_set);
		gen_body (state_no, is_empty, stmt);
	    }
	    else
		is_error = true;
	}
    }

    if (is_error) {
	sxba_not (sc1_set);
	gen_label (false, sc1_set);
	gen_error (state_no);
    }

    put_edit_ap ("}");
    put_edit_nl (1);
    put_edit_ap ("/**************************************************************************/");
    put_edit_nl (1);
    sxfree (sc1_set);
    sxbm_free (sc_to_sc_set);
}



static SXINT	through (SXINT xprod)
{
    bool	is_keep = false;
    transition_matrix_s		through_stmt;

    for (;;) {
	through_stmt = lecl_tables.SXS_action_or_prdct_code [xprod].stmt;

	if (KEEP (through_stmt)) {
	    is_keep = true;
	}

	switch (CODOP (through_stmt)) {
	case State:
	    if (is_keep) {
		return NEXT (through_stmt);
	    } else
		return 0;

	default:
	    return 0;

	case ActPrdct:
	    xprod = NEXT (fe.stmt);
	}
    }
}

/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
    SXINT		init_state, codop, next;
    bool	state_status, next_state_status;
    static char		ME [] = "st_to_c";
    static char		Usage [] = "Usage:\t%s language\n";

    sxinitialise(next_state_status); /* pour faire taire gcc -Wuninitialized */
    sxopentty ();

    if (argc != 2) {
	fprintf (sxstderr, Usage, ME);
	sxexit (3);
    }

    prgentname = argv [1];

    if (!lecl_read (&lecl_tables, prgentname))
	sxexit (2);

    if (lecl_tables.S_tbl_size.S_is_non_deterministic) {
	fprintf (sxstderr, "The NDFSA processing is not yet implemented.\n");
	sxexit (2);
    }

    string = varstr_alloc (256);

    to_be_sorted = (SXINT*) sxalloc (lecl_tables.S_tbl_size.S_last_simple_class_no + 1, sizeof (SXINT));
    ref_no = (SXINT*) sxcalloc (max_state_no + 1, sizeof (SXINT));
    is_ref = sxba_calloc (max_state_no + 1);
    scan_in_la = sxba_calloc (max_state_no + 1);
    first_scan_in_la = sxba_calloc (max_state_no + 1);
    scan = sxba_calloc (max_state_no + 1);
    keep_and_scan = sxba_calloc (max_state_no + 1);
    sc_to_char_set = sxbm_calloc (lecl_tables.S_tbl_size.S_last_simple_class_no + 1, lecl_tables.S_tbl_size.
	 S_last_char_code + 1 + 1);
    is_ts_empty = sxba_calloc (max_state_no + lecl_tables.S_tbl_size.S_xprod + 1);
    put_edit_initialize (stdout);


/* On remplit sc_to_char_set */

    for (i = 0; i <= lecl_tables.S_tbl_size.S_last_char_code; i++) {
	sxba_1_bit (sc_to_char_set [lecl_tables.S_char_to_simple_class [i]], i + 1);
    }

    nbndef = sxba_cardinal (lecl_tables.S_is_a_keyword);
    max_t_code_name_lgth = 0;

    for (i = 0; i <= lecl_tables.S_tbl_size.S_last_static_ste; i++) {
	max_t_code_name_lgth = max (max_t_code_name_lgth, lecl_tables.SXS_adrp [i].lgth);
    }


/* On calcule le nombre de references a un etat */

    is_dummy_hash = false;
    is_first_col = false;
    is_last_col = false;
    is_user_action_or_prdct = false;
    is_stmt_5_0 = false;
    is_la = false;
    is_post_action = false;
    is_user_prdct = false;
    is_keep_shr = false;
    is_shr = false;
    is_keep_sr = false;
    is_sr = false;
    is_ascii = true;
    is_keep_sr_to_comments = false;
    is_sr_to_comments = false;
    is_erase = false;
    is_include = false;
    is_mark = false;
    is_release = false;

    for (i = 128; i <= lecl_tables.S_tbl_size.S_last_char_code; i++)
	if (lecl_tables.S_char_to_simple_class [i] != 1) {
	    is_ascii = false;
	    break;
	}


/* calcul de is_ts_empty */

    sxba_fill (is_ts_empty) /* a priori */ , sxba_0_bit (is_ts_empty, 0);
    init_state = 1;

new_try:
    for (i = init_state; i <= max_state_no; i++) {
	state_status = sxba_bit_is_set (is_ts_empty, i);

	for (j = 1; j <= lecl_tables.S_tbl_size.S_last_simple_class_no; j++) {
	    stmt = lecl_tables.S_transition_matrix [i] [j];
	    codop = CODOP (stmt);
	    next = NEXT (stmt);

	    if (is_shift [codop]) {
		next_state_status = sxba_bit_is_set (is_ts_empty, next);
	    }

	    switch (codop) {
	    case SameState:
	    case State:
	    case FirstLookAhead:
	    case NextLookAhead:
		if (!state_status || KEEP (stmt)) {
		    if (next_state_status) {
			sxba_0_bit (is_ts_empty, next);

			if (next <= i) {
			    init_state = next;
			    goto new_try;
			}
		    }
		}

		break;

	    case ActPrdct:
		if (lecl_tables.SXS_action_or_prdct_code [next].kind == IsAction
		    && (!state_status || KEEP (stmt))) {
		    if ((init_state = through (next)) != 0) {
			if (sxba_bit_is_set (is_ts_empty, init_state)) {
			    sxba_0_bit (is_ts_empty, init_state);

			    if (init_state <= i) {
				goto new_try;
			    }
			}
		    }
		}

		break;

	    default:
		break;
	    }
	}
    }

    ref_no [1] = 1 /* par construction */ ;

    for (i = 1; i <= max_state_no; i++) {
	sxba_empty (is_ref);

	for (j = 1; j <= lecl_tables.S_tbl_size.S_last_simple_class_no; j++) {
	    stmt = lecl_tables.S_transition_matrix [i] [j];
	    codop = CODOP (stmt);
	    next = NEXT (stmt);

	    if (is_hash_reduce [codop])
		if (next == 0)
		    is_dummy_hash = true;

	    if (next == 0)
		if (codop == Reduce)
		    is_stmt_5_0 = true;

	    switch (codop) {
	    case Reduce:
		if (next == 0) /* comments */ {
		    bool	is_empty = sxba_bit_is_set (is_ts_empty, i);

		    if (SCAN (stmt)) {
			if (is_empty) {
			    sxba_1_bit (scan, 1);
			    sxba_1_bit (is_ref, 1);
			}

			if (KEEP (stmt))
			    is_keep_sr_to_comments = true;
			else if (!is_empty)
			    is_sr_to_comments = true;
		    }
		    else {
			if (is_empty)
			    sxba_1_bit (is_ref, 1);
		    }
	        }
	        else if (SCAN (stmt)) {
		    if (KEEP (stmt))
			is_keep_sr = true;
		    else
			is_sr = true;
		}

		break;

	    case HashReduce:
		if (SCAN (stmt)) {
		    if (KEEP (stmt))
			is_keep_shr = true;
		    else
			is_shr = true;
		}

		break;

	    case ActPrdct:
		if (KEEP (stmt) || !sxba_bit_is_set (is_ts_empty, i)) {
		    if (lecl_tables.SXS_action_or_prdct_code [next].kind == IsPredicate) {
			while (lecl_tables.SXS_action_or_prdct_code [next].action_or_prdct_no != 0) {
			    sxba_0_bit (is_ts_empty, max_state_no + next++);
			}
		    }

		    sxba_0_bit (is_ts_empty, max_state_no + next);
		}

		break;

	    default:
		break;
	    }

	    if (is_shift [codop]) {
		sxba_1_bit (is_ref, next);

		switch (codop) {
		case State:
		case SameState:
		    if (KEEP (stmt)) {
			sxba_1_bit (keep_and_scan, next);
		    }

		    if (SCAN (stmt)) {
			sxba_1_bit (scan, next);
		    }

		    break;

		case FirstLookAhead:
		    sxba_1_bit (first_scan_in_la, next);
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
		    /* absence de "break" (probablement intentionnelle) */
		    __attribute__ ((fallthrough));
#endif

		case NextLookAhead:
		    is_la = true;
		    sxba_1_bit (scan_in_la, next);
		    break;

		default:
		    break;
		}
	    }
	}

	for (j = sxba_scan (is_ref, 0); j != -1; j = sxba_scan (is_ref, j)) {
	    (ref_no [j])++;
	}
    }

    init_state = 1;

once_more:
    for (i = init_state; i <= lecl_tables.S_tbl_size.S_xprod; i++) {
	stmt = lecl_tables.SXS_action_or_prdct_code [i].stmt;
	codop = CODOP (stmt);
	next = NEXT (stmt);

	if (codop == ActPrdct && (KEEP (stmt) || !sxba_bit_is_set (is_ts_empty, max_state_no + i)) && sxba_bit_is_set (is_ts_empty,
	     max_state_no + next)) {
	    sxba_0_bit (is_ts_empty, max_state_no + next);

	    if (next <= i) {
		init_state = next;
		goto once_more;
	    }
	}
    }

    for (i = 1; i <= lecl_tables.S_tbl_size.S_xprod; i++) {
	fe = lecl_tables.SXS_action_or_prdct_code [i];
	stmt = fe.stmt;
	codop = CODOP (stmt);
	next = NEXT (stmt);

	if (!fe.is_system) {
	    is_user_action_or_prdct = true;

	    if (fe.kind == IsPredicate)
		is_user_prdct = true;
	}

	if (is_hash_reduce [codop])
	    if (next == 0)
		is_dummy_hash = true;

	switch (codop) {
	case State:
	    if (KEEP (stmt)) {
		sxba_1_bit (keep_and_scan, next);
	    }

	    if (SCAN (stmt)) {
		sxba_1_bit (scan, next);
	    }

	    break;

	case ReducePost:
	    is_post_action = true;
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	    /* absence de "break" (probablement intentionnelle) */
	    __attribute__ ((fallthrough));
#endif

	case Reduce:
	    if (next == 0) {
		if (SCAN (stmt)) {
		    if (KEEP (stmt))
			is_keep_sr_to_comments = true;
		    else
			is_sr_to_comments = true;
		}
	    }
	    else
		if (SCAN (stmt)) {
		    if (KEEP (stmt))
			is_keep_sr = true;
		    else
			is_sr = true;
		}

	    break;

	case HashReducePost:
	    is_post_action = true;
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	    /* absence de "break" (probablement intentionnelle) */
	    __attribute__ ((fallthrough));
#endif

	case HashReduce:
		if (SCAN (stmt)) {
		    if (KEEP (stmt))
			is_keep_shr = true;
		    else
			is_shr = true;
		}

	    break;

	default:
	    break;
	}

	switch (codop) {
	case State:
	case FirstLookAhead:
	case NextLookAhead:
	    ref_no [next] += 2 /* On force la non extension in_line */ ;
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	    /* l'avertissement GCC 7 pourrait indiquer un bogue */
	    __attribute__ ((fallthrough));
#endif

	case Error:
	case Reduce:
	case HashReduce:
	case ReducePost:
	case HashReducePost:
	case ActPrdct:
	  if (fe.kind == IsPredicate && fe.is_system) {
		if (fe.action_or_prdct_no == 1 /* is_first_col */  || fe.action_or_prdct_no == 4 /* ^is_first_col */ )
		    is_first_col = true;
		else if (fe.action_or_prdct_no == 5 /* is_last_col */  || fe.action_or_prdct_no == 6 /* ^is_last_col */ )
		    is_last_col = true;
	  }
	    if (fe.kind == IsAction && codop == Reduce && KEEP (stmt) == 0
		&& SCAN (stmt) == 0 && next == 0 && sxba_bit_is_set (
		 is_ts_empty, max_state_no + i)) {
		/* @n		=> Comments */
		(ref_no [1])++;
	    }

	    if (fe.kind == IsAction)
		if (fe.is_system)
		    switch (fe.action_or_prdct_no) {
		    case 10:
			/* Mark */
			is_mark = true;
			break;

		    case 11:
			/* Release */
			is_release = true;
			break;

		    case 5:
			/* Erase */
			is_erase = true;
			break;

		    case 6:
			/* include */
			is_include = true;
			break;
		    default:
#if EBUG
		      sxtrap("st_to_c","unknown switch case #4");
#endif
		      break;

		    }

	    break;
	default:
#if EBUG
	  sxtrap("st_to_c","unknown switch case #5");
#endif
	  break;
	}
    }


/* on genere le prologue */

    put_edit_ap ("/* ********************************************************************");
    put_edit_apnnl ("   *  This program has been generated from ");
    put_edit_apnnl (prgentname);
    put_edit_apnnl (".lecl");
    put_edit (71, "*");
    put_edit_ap ("   *  by the SYNTAX (*) lexical constructor LECL                      *");
    put_edit_ap ("   ********************************************************************");
    put_edit_ap ("   *  (*) SYNTAX is a trademark of INRIA.                             *");
    put_edit_ap ("   ******************************************************************** */");
    put_edit_nl (2);
    put_edit_apnnl ("static char WHAT_");
    put_edit_apnnl (prgentname);
    put_edit_apnnl ("_scnr []=\"@(#)");
    put_edit_apnnl (prgentname);
    put_edit_apnnl ("_scnr.c\t- SYNTAX [unix]");
    put_edit_ap ("\";");
    put_edit_nl (2);
    put_edit_ap ("#include \"sxunix.h\"");
    put_edit_ap ("#include <ctype.h>");
    printf ("\n/**************************************************/\n");
    put_edit_ap ("/*     S T A T I C S     */");
    put_edit_nl (1);
    out_S_char_to_simple_class ();
    put_edit_nl (1);
    printf ("\n/**************************************************/\n");
    put_edit_ap ("/*     D E F I N E     */");
    put_edit_nl (1);
    put_edit_ap ("#ifdef LDBG");
    put_edit_ap ("#define DBG");
    put_edit_ap ("#endif");
    put_edit_nl (1);
    put_edit_ap ("/* a new character is catenated to token_string */");
    put_edit_ap ("#define put_a_char(c)							\\");
    put_edit_ap ("(    (sxsvar.sxlv.ts_lgth+2 == sxsvar.sxlv_s.ts_lgth_use)			\\");
    put_edit_ap ("	? sxsvar.sxlv_s.token_string = (char *) sxrealloc (sxsvar.sxlv_s.	\\");
    put_edit_ap ("	    token_string, sxsvar.sxlv_s.ts_lgth_use *= 2, sizeof (char))	\\");
    put_edit_ap ("	: NULL,								\\");
    put_edit_ap ("     sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth ++] = c		\\");
    put_edit_ap (")");
    put_edit_nl (1);
    put_edit_ap ("/* token_string becomes a c string */");

    if (is_user_action_or_prdct || is_release || !lecl_tables.S_tbl_size.S_are_comments_erased) {
	put_edit_ap ("#define ts_null()	(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth] = SXNUL)");
    }

    put_edit_nl (1);
    put_edit_ap ("/* computes the class of the character \'c\' */");
    put_edit_ap ("#define char_to_class(c)    ((S_char_to_simple_class + 128) [c])");

    if (!lecl_tables.S_tbl_size.S_are_comments_erased) {
      printf ("\n/**************************************************/\n");
	put_edit_ap ("/*     P R O C E D U R E    */");
    }

    if (!lecl_tables.S_tbl_size.S_are_comments_erased) {
	put_edit_ap ("/* comments processing */");
	put_edit_nl (1);
	put_edit_ap ("static void comments_put (str, lgth)");
	put_edit_ap ("char   *str;");
	put_edit_ap ("SXINT     lgth;");
	put_edit_ap ("{");
	put_edit_ap ("SXINT lgth_comment;");
	put_edit_ap ("if (sxsvar.sxlv.terminal_token.comment == NULL)");
	put_edit_ap ("strcpy (sxsvar.sxlv.terminal_token.comment = (char *) sxalloc (lgth + 1, sizeof (char)), str);");
	put_edit_ap ("else {");
	put_edit_ap ("lgth_comment  = strlen (sxsvar.sxlv.terminal_token.comment);");
	put_edit_ap ("strcpy ((sxsvar.sxlv.terminal_token.comment = ");
	put_edit_ap ("(char *) sxrealloc (sxsvar.sxlv.terminal_token.comment,");
	put_edit_ap ("lgth_comment + lgth + 1,");
	put_edit_ap ("sizeof (char))");
	put_edit_ap (") + lgth_comment,");
	put_edit_ap ("str);");
	put_edit_ap ("}");
	put_edit_ap ("}");
    }


    printf ("\n/**************************************************/\n");
    put_edit_ap ("/*     M A J O R    E N T R Y     */");
    put_edit_apnnl ("void ");
    put_edit_apnnl (prgentname);
    put_edit_ap ("_scan_it (void) {");
    put_edit_ap ("unsigned char current_class;");

    if (is_la)
	put_edit_ap ("unsigned char current_class_in_la;");

    if (is_post_action)
	put_edit_ap ("SXINT post_action_no;");

    if (!lecl_tables.S_tbl_size.S_are_comments_erased)
	put_edit_ap ("sxsvar.sxlv.terminal_token.comment = NULL;");

    put_edit_apnnl ("sxuse (WHAT_");
    put_edit_apnnl (prgentname);
    put_edit_ap ("_scnr); /* to shut the mouth of 'gcc -Wunused' */");
    put_edit_ap ("/* let's go for a new lexical token */");
    put_edit_ap ("#ifdef LDBG");
    put_edit_ap ("printf (\"\\n******************* SCAN_IT ********************\\n\");");
    put_edit_ap ("#endif");

    if (is_post_action) {
	put_edit_ap ("newlextok:");
	put_edit_ap ("post_action_no = -1;");
    }

    put_edit_ap ("sxsvar.sxlv.ts_lgth = 0;");

    put_edit_ap ("current_class = char_to_class (sxsrcmngr.current_char);");
    put_edit_ap ("sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;");

    if (sxba_bit_is_set (scan, 1)) {
	put_edit_ap ("goto STATE_1;");
    }


/* On genere les etats */

    for (i = 1; i <= max_state_no; i++) {
	if (ref_no [i] != 1 || i == 1)
	    gen_state (i);
    }


/* generation de l'epilogue */

    printf ("\n/**************************************************/\n");

    if (!lecl_tables.S_tbl_size.S_are_comments_erased) {
	put_edit_ap ("/*   R E D U C E   T O   C O M M E N T S   */");
	put_edit_nl (1);

	if (is_keep_sr_to_comments) {
	    put_edit_ap ("keep_scan_reduce_comments:");
	    put_edit_ap ("put_a_char (sxsrcmngr.current_char);");

	    if (is_sr_to_comments)
		put_edit_ap ("scan_reduce_comments:");

	    put_edit_ap ("current_class = char_to_class (sxnextchar ());");
	}
	else if (is_sr_to_comments) {
	    put_edit_ap ("scan_reduce_comments:");
	    put_edit_ap ("current_class = char_to_class (sxnextchar ());");
	}

	put_edit_ap ("reduce_comments:");
	put_edit_ap ("if (sxsvar.sxlv.ts_lgth != 0) {");
	put_edit_ap ("ts_null ();");
	put_edit_ap ("comments_put(sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);");
	put_edit_ap ("sxsvar.sxlv.ts_lgth=0;");
	put_edit_ap ("}");
	put_edit_ap ("goto STATE_1;");
    }

    if (nbndef != 0) {
	put_edit_ap ("/*     H A S H   A N D   R E D U C E     */");
	put_edit_nl (1);

	if (is_keep_shr) {
	    put_edit_ap ("keep_scan_hash_reduce:");
	    put_edit_ap ("put_a_char (sxsrcmngr.current_char);");

	    if (is_shr)
		put_edit_ap ("scan_hash_reduce:");

	    put_edit_ap ("sxnextchar ();");
	}
	else if (is_shr) {
	    put_edit_ap ("scan_hash_reduce:");
	    put_edit_ap ("sxnextchar ();");
	}

	put_edit_ap ("hash_reduce:");
	put_edit_ap ("{SXINT look_ahead;");
	put_edit_ap ("if ((look_ahead = (*sxsvar.SXS_tables.S_check_keyword) (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth)) != 0) {");
	put_edit_ap ("sxsvar.sxlv.terminal_token.lahead = look_ahead;");

	if (is_post_action)
	    put_edit_ap ("goto post_action;");
	else {
	    put_edit_ap ("#ifdef DBG");
	    put_edit_ap (
	    "printf (\"\\t\\t\\t\\ttoken = \\\"%s\\\"\\n\", sxttext (sxsvar.sxtables, sxsvar.sxlv.terminal_token.lahead));"
		 );
	    put_edit_ap ("#endif");
	    put_edit_ap ("goto done;");
	}

	put_edit_ap ("}");
	put_edit_ap ("else {");

	if (is_dummy_hash) {
	    put_edit_ap ("if (sxsvar.sxlv.terminal_token.lahead == 0) {");
	    put_edit_ap ("sxsvar.sxlv.mode.errors_nb++;");
	    put_edit_ap ("if (!sxsvar.sxlv.mode.is_silent)");
	    put_edit_ap ("sxerror (sxsvar.sxlv.terminal_token.source_index,");
	    put_edit_ap ("sxsvar.sxtables->err_titles [2][0],");
	    put_edit_ap ("sxsvar.SXS_tables.S_global_mess [2],");
	    put_edit_ap ("sxsvar.sxtables->err_titles [2]+1);");
	    put_edit_ap ("sxsvar.sxlv.ts_lgth = 0;");
	    put_edit_ap ("goto STATE_1;");
	    put_edit_ap ("}");
	}

	if (is_keep_sr || is_sr)
	    put_edit_ap ("goto reduce;");

	put_edit_ap ("}\n}");
	put_edit_nl (3);
    }

    put_edit_ap ("/*   R E D U C E   */");
    put_edit_nl (1);

    if (is_keep_sr) {
	put_edit_ap ("keep_scan_reduce:");
	put_edit_ap ("put_a_char (sxsrcmngr.current_char);");

	if (is_sr)
	    put_edit_ap ("scan_reduce:");

	put_edit_ap ("sxnextchar ();");
    }
    else {
	if (is_sr) {
	    put_edit_ap ("scan_reduce:");
	    put_edit_ap ("sxnextchar ();");
	}
    }

    put_edit_ap ("reduce:");
    put_edit_ap ("if (sxsvar.sxlv.ts_lgth != 0) {");
    put_edit_ap ("sxsvar.sxlv.terminal_token.string_table_entry =");
    put_edit_ap ("sxstr2save (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);");
    put_edit_ap ("#ifdef DBG");
    put_edit_ap (
	 "printf (\"\\t\\t\\t\\tstring = \\\"%s\\\"\\n\", sxstrget (sxsvar.sxlv.terminal_token.string_table_entry));");
    put_edit_ap ("#endif");
    put_edit_ap ("}");
    put_edit_nl (1);

    if (is_post_action) {
	put_edit_ap ("/*    P O S T _ A C T I O N   */");
	put_edit_nl (1);
	put_edit_ap ("post_action:");
	put_edit_ap ("if (post_action_no >= 0) {");
	put_edit_ap ("ts_null ();");
	put_edit_ap ("if (sxsvar.sxlv.mode.with_user_act)");
	put_edit_ap ("(*sxsvar.SXS_tables.S_scanact) (SXACTION, post_action_no);");
	put_edit_ap ("if (sxsvar.sxlv.terminal_token.lahead == 0) {");
	put_edit_ap ("if (sxsvar.sxlv.terminal_token.string_table_entry == SXERROR_STE)");
	put_edit_ap ("/* La post-action a decide' de retourner a l'appelant sans rien faire... */");
        put_edit_ap ("return;");
        put_edit_ap ("/* La post-action a decide' de reboucler... */");
        put_edit_ap ("current_class = char_to_class (sxsrcmngr.current_char);");
        put_edit_ap ("goto newlextok;");
	put_edit_ap ("}}");
    }

    put_edit_ap ("done:sxput_token(sxsvar.sxlv.terminal_token);");

    put_edit_ap ("#ifdef DBG");
    put_edit_ap (
	 "printf (\"\\t\\t\\t\\ttoken = \\\"%s\\\"\\n\", sxttext (sxsvar.sxtables, sxsvar.sxlv.terminal_token.lahead));");
    put_edit_ap ("#endif");
    put_edit_apnnl ("/* end ");
    put_edit_apnnl (prgentname);
    put_edit_ap ("_scan_it */");
    put_edit_nl (1);
    put_edit_ap ("}");
    printf ("\n/**************************************************/\n");
    put_edit_ap ("void ");
    put_edit_apnnl (prgentname);
    put_edit_ap ("_scanner (SXINT what_to_do, SXTABLES *arg)");
    put_edit_ap ("{ switch (what_to_do) {");
    put_edit_ap ("case SXOPEN:");
    put_edit_ap ("/* new language, new tables: prepare new local variables */");
    put_edit_ap ("sxsvar.sxtables = arg;");
    put_edit_ap ("sxsvar.SXS_tables = arg->SXS_tables;");
    put_edit_ap ("sxsvar.sxlv_s.token_string = (char *) sxalloc (sxsvar.sxlv_s.ts_lgth_use");
    put_edit_ap ("= 120, sizeof (char));");

    if (lecl_tables.S_tbl_size.S_counters_size != 0) {
	put_edit_apnnl ("sxsvar.sxlv_s.counters = (SXINT *) sxcalloc (");
	put_edit_apnb (lecl_tables.S_tbl_size.S_counters_size);
	put_edit_ap (", sizeof (SXINT));");
    } else {
	put_edit_ap ("sxsvar.sxlv_s.counters = NULL;");
    }

    put_edit_ap ("sxsvar.sxlv.mode.errors_nb = 0;");
    put_edit_ap ("sxsvar.sxlv.mode.is_silent = false;");
    put_edit_ap ("sxsvar.sxlv.mode.with_system_act = true;");
    put_edit_ap ("sxsvar.sxlv.mode.with_user_act = true;");
    put_edit_ap ("sxsvar.sxlv.mode.with_system_prdct = true;");
    put_edit_ap ("sxsvar.sxlv.mode.with_user_prdct = true;");

    put_edit_ap ("break;");
    put_edit_nl (1);
    put_edit_ap ("case SXINIT:");
    put_edit_ap ("/* new source text */");

    if (is_include)
	put_edit_ap ("sxsvar.sxlv_s.include_no = 0;");

    if ((i = lecl_tables.S_tbl_size.S_counters_size) != 0) {
	while (--i >=0) {
	    put_edit_apnnl ("sxsvar.sxlv_s.counters[");
	    put_edit_apnb (i);
	    put_edit_ap ("] = 0;");
	}
    }

    if (lecl_tables.S_tbl_size.S_are_comments_erased)
	put_edit_ap ("sxsvar.sxlv.terminal_token.comment = NULL;");

    put_edit_ap ("/*  the first char of the source text is read */");
    put_edit_ap ("sxnextchar ();");
    put_edit_ap ("break;");
    put_edit_nl (1);

    put_edit_ap ("case SXACTION:");
    put_edit_apnnl (prgentname);
    put_edit_ap ("_scan_it ();");
    put_edit_ap ("break;");
    put_edit_nl (1);

    put_edit_ap ("case SXCLOSE:");
    put_edit_ap ("/* end of language, local variables are freed */");
    put_edit_ap ("sxfree (sxsvar.sxlv_s.token_string);");
    put_edit_ap ("sxsvar.sxlv_s.token_string = NULL;");
    put_edit_ap ("(*sxsvar.SXS_tables.S_recovery) (SXCLOSE, (SXINT) 0 /* dummy */, NULL /* dummy */);");
    put_edit_ap ("break;");
    put_edit_nl (1);
    put_edit_ap ("default:");
    put_edit_ap ("break;");
    put_edit_ap ("}");
    put_edit_apnnl ("/* end ");
    put_edit_apnnl (prgentname);
    put_edit_ap ("_scanner */");
    put_edit_ap ("}");
    sxfree (is_ts_empty);
    sxbm_free (sc_to_char_set);
    sxfree (keep_and_scan);
    sxfree (scan);
    sxfree (first_scan_in_la);
    sxfree (scan_in_la);
    sxfree (is_ref);
    sxfree (ref_no);
    sxfree (to_be_sorted);
    lecl_free (&lecl_tables);

    if (char_set)
	sxfree (char_set);

    varstr_free (string);
    sxexit (0);

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

