/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1984 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Lun  4 Oct 1999 10:44 (pb):	Utilisation des nouveaux CHAR_TO_STRING,*/
/*				sxtolower et sxtoupper			*/
/************************************************************************/
/* Lun 25 Nov 1996 11:27 (pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 22-10-93 10:00 (pb):		Adaptation a OSF			*/
/************************************************************************/
/* 05-05-93 17:11 (pb) :	On ne fait rien si l'automate est non	*/
/*				deterministe.				*/
/************************************************************************/
/* 27-07-92 17:20 (pb) :	Bug ds l'utilisation de is_ts_empty.	*/
/************************************************************************/
/* 02-04-92 10:20 (pb) :	Bug ds l'exploitations des actions et	*/
/* 				predicats pour la collecte des infos	*/
/*				globales.				*/
/*				Bug dans gen_body sur les actions	*/
/************************************************************************/
/* 01-04-92 16:25 (pb) :	Bug ds la generation des post-actions	*/
/************************************************************************/
/* 23-03-92 12:07 (pb) :	char_to_simple_class est genere ds le	*/
/*				code (au lieu d'etre pris ds les tables)*/
/*				ca permet (plus facilement) d'utiliser	*/
/*				le scanner en stand-alone.		*/
/*				sxcheck_magic_number n'est plus appele	*/
/************************************************************************/
/* 02-06-88 10:15 (pb) :	BITSTR => SXBA				*/
/************************************************************************/
/* 02-05-88 10:25 (pb) :	Appel nouveau check_keyword		*/
/************************************************************************/
/* 29-07-87 16:17 (phd) :	Suppression d'affectations inutiles	*/
/************************************************************************/
/* 22-06-87 11:32 (pb) :	Calcul de current_class apres @Release	*/
/************************************************************************/
/* 06-05-87 17:22 (pb) :	Complementaire des predicats utilisateur*/
/************************************************************************/
/* 24-04-87 11:21 (pb) :	Ajout de @Incr et @Decr	   	   	*/
/************************************************************************/
/* 15-04-87 11:15 (pb) :	Adaptation aux nouveaux codop	   	*/
/************************************************************************/
/* 26-03-87 11:05 (pb) :	Ajout du parametre what a recovery  	*/
/************************************************************************/
/* 20-03-87 16:19 (pb) :	Recovery appele via les tables	  	*/
/************************************************************************/
/* 09-03-87 13:27 (pb) :	Maj de keep_and_scan et scan depuis les	*/
/*				les predicats				*/
/************************************************************************/
/* 02-03-87 15:14 (phd) :	Exportation de sxcheck_keyword par les	*/
/*				analyseurs engendres.  Optimisations	*/
/************************************************************************/
/* 13-02-87 09:02 (pb) :	Qq ";" qui manquaient			*/
/************************************************************************/
/* 11-02-87 11:19 (pb) :	Test de argc				*/
/************************************************************************/
/* 04-02-87 16:27 (pb) :	Nouveaux "scramble": 11, 12, 13 et 14	*/
/************************************************************************/
/* 30-01-87 15:15 (pb) :	Introduction de EMPTY_STE et sxstr2save	*/
/************************************************************************/
/* 23-01-87 08:45 (pb) :	Etat appele depuis act-prdct par goto 	*/
/*				(non genere in_line, boucle possible)   */
/************************************************************************/
/* 20-01-87 15:36 (pb&phd) :	Modification de Mark, Release, Erase  	*/
/************************************************************************/
/* 16-01-87 09:58 (pb) :	Is_First_Col, comments_put	  	*/
/************************************************************************/
/* 23-12-86 16:52 (pb) :	Changement du scramble (cas 6, 7, 8)	*/
/************************************************************************/
/* 01-12-86 11:52 (pb&phd) :	Appel de sxcheck_magic_number		*/
/************************************************************************/
/* 01-12-86 11:48 (phd&pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/*****************************************************************/
/*                                                               */
/*                                                               */
/*  This program has been translated from gen_scanner_in_pl1.pl1 */
/*  on Friday the third of October, 1986, at 10:58:03,           */
/*  on the Multics system at INRIA,                              */
/*  by the release 3.3l PL1_C translator of INRIA,               */
/*         developped by the "Langages et Traducteurs" team,     */
/*         using the SYNTAX (*), FNC and Paradis systems.        */
/*                                                               */
/*                                                               */
/*****************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                           */
/*****************************************************************/

/* Le 24/01/2003
   Pour avoir les bonnes macros pour STMT, KEEP, SCAN, CODOP, NEXT ... */
#define SXS_MAX (SHRT_MAX+1)
#include "varstr.h"
#include "sxunix.h"
#include "sxsstmt.h"
#include "S_tables.h"
#include "put_edit.h"
char WHAT_LECLSTTOC[] = "@(#)SYNTAX - $Id: st_to_c.c 607 2007-06-04 09:35:31Z syntax $" WHAT_DEBUG;

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

BOOLEAN  sxverbosep;
extern int	equality_sort ();
extern VOID     lecl_free();
extern BOOLEAN  lecl_read ();

#define max(x,y) ((x)<(y)) ? (y) : (x)


static void	gen_prdct_seq ();
static void	gen_state ();
/* les codop sont reperes de droite a gauche, le codop (fictif) 0 correspond 
   au bit de droite */

static BOOLEAN	is_shift [] = {FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE};
static BOOLEAN	is_hash_reduce [] = {FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE};

#define char_max 256

#define max_state_no lecl_tables.S_tbl_size.S_last_state_no

static SXBA	/* char_max */ char_set = NULL;
static struct lecl_tables_s	lecl_tables;
static VARSTR	string;
static char	*prgentname;
static int	*ref_no /* 1:S_last_state_no */ ;
static SXBA	is_ref, scan_in_la, scan, keep_and_scan, first_scan_in_la /* S_last_state_no */ ;
static SXBA	is_ts_empty /* max_state_no + S_xprod */ ;
static int	*to_be_sorted /* 1:S_last_simple_class_no */ ;
static transition_matrix_s	stmt;
static transition_matrix_s	*transition_matrix_line;
static struct action_or_prdct_code_s	fe;
static int	i, j;
static BOOLEAN	is_erase, is_include, is_mark, is_release, is_dummy_hash, is_user_action_or_prdct,
     is_la, is_first_col, is_last_col, is_stmt_5_0, is_post_action, is_user_prdct, is_keep_shr, is_shr, is_keep_sr, is_sr
     , is_ascii, is_keep_sr_to_comments, is_sr_to_comments;
static SXBA	/* S_last_char_code+1 */ *sc_to_char_set
		/* 1:S_last_simple_class_no */
							;
static int	nbndef, max_t_code_name_lgth;




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


static VOID	out_S_char_to_simple_class ()
{
    register int	out_S_char_to_simple_class_j, out_S_char_to_simple_class_i;
    register int	d, nbt;
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

	putchar (NEWLINE);
    }

    for (out_S_char_to_simple_class_j = d; (unsigned)out_S_char_to_simple_class_j < (sizeof (sctsc) / sizeof (unsigned char)); out_S_char_to_simple_class_j++) {
	printf ("%d,", sctsc [out_S_char_to_simple_class_j]);
    }

    puts ("};");
}



static VARSTR	gen_sc_name (gen_sc_name_char_set, varstr_ptr)
    SXBA	gen_sc_name_char_set;
    VARSTR	varstr_ptr;
{
    int		gen_sc_name_i, gen_sc_name_j, gen_sc_name_l;
    BOOLEAN	is_first, is_slice;

    is_slice = FALSE;
    is_first = TRUE;

    for (gen_sc_name_i = sxba_scan (gen_sc_name_char_set, 0); gen_sc_name_i != -1; gen_sc_name_i = sxba_scan (gen_sc_name_char_set, gen_sc_name_i)) {
	gen_sc_name_l = sxba_0_lrscan (gen_sc_name_char_set, gen_sc_name_i);

	if (gen_sc_name_l == -1)
	    gen_sc_name_l = char_max - gen_sc_name_i + 1;
	else
	    gen_sc_name_l -= gen_sc_name_i;

	if (gen_sc_name_l > 7) {
	    /* On genere une slice */
	    if (is_first)
		is_first = FALSE;
	    else {
		if (!is_slice)
		    varstr_catenate (varstr_ptr, "\"");

		varstr_catenate (varstr_ptr, " + ");
	    }

	    varstr_catenate (varstr_catenate (varstr_catenate (varstr_catenate (varstr_catenate (varstr_ptr, "\""),
		 SXCHAR_TO_STRING (gen_sc_name_i - 1)), "\"..\""), SXCHAR_TO_STRING (gen_sc_name_i + gen_sc_name_l - 2)), "\"");
	    is_slice = TRUE;
	}
	else {
	    if (is_slice)
		varstr_catenate (varstr_ptr, " + \"");
	    else if (is_first) {
		varstr_catenate (varstr_ptr, "\"");
		is_first = FALSE;
	    }

	    for (gen_sc_name_j = 0; gen_sc_name_j < gen_sc_name_l; gen_sc_name_j++)
		varstr_catenate (varstr_ptr, SXCHAR_TO_STRING (gen_sc_name_i + gen_sc_name_j - 1));

	    is_slice = FALSE;
	}

	gen_sc_name_i += gen_sc_name_l - 1;
    }

    if (!is_slice)
	varstr_catenate (varstr_ptr, "\"");

    return varstr_ptr;
}


static VARSTR	gen_cc_name (SC_TO_CHAR_SET, sc_set, varstr_ptr)
    SXBA	*SC_TO_CHAR_SET;
    SXBA	sc_set;
    VARSTR	varstr_ptr;
{
    int		gen_cc_name_i;
    BOOLEAN	not_empty;

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



static BOOLEAN	end_of_token (xprod)
    int		xprod;
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
	    return TRUE;

	default:
	    return FALSE;
	}
    }
}



static VOID	goto_state (state_no, is_keep, is_scan)
    int		state_no;
    BOOLEAN	is_keep, is_scan;
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



static VOID	reduce (tok, is_keep, is_scan, is_hash, is_empty, reduce_is_post_action)
    int		tok;
    BOOLEAN	is_keep, is_scan, is_hash, is_empty, reduce_is_post_action;
{
    if (tok == 0 && is_empty /* plus precis que lecl_tables.S_tbl_size.S_are_comments_erased */	 && !is_hash)
	goto_state (1, is_keep, is_scan);
    else {
	if (tok != 0) {
	    char	t [65];

	    put_edit_apnnl ("sxsvar.sxlv.terminal_token.lahead = ");
	    put_edit_apnb (tok);
	    put_edit_apnnl (" /* ");

	    if (is_hash)
		put_edit_apnnl ("HASH + ");

	    strncpy (t, lecl_tables.S_terlis + lecl_tables.SXS_adrp [tok].xterlis, lecl_tables.SXS_adrp [tok].lgth);
	    t [lecl_tables.SXS_adrp [tok].lgth] = NUL;
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



static VOID	gen_error (state_no)
    int		state_no;
{
    put_edit_apnnl ("if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, ");
    put_edit_apnb (state_no);
    put_edit_apnnl (", &current_class");

    if (NEXT (lecl_tables.S_transition_matrix [state_no] [1]) != 0)
	put_edit_apnnl ("_in_la");
	
    put_edit_ap (")) {");

    put_edit_apnnl ("goto STATE_");
    put_edit_apnb (state_no);
    put_edit_ap (";}\nreturn;");
}



static	SXVOID gen_label (is_case, sc_set)
    BOOLEAN	is_case;
    SXBA	sc_set;
{
    register int	gen_label_i, gen_label_j;
    register char	*s, *t, c;

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

    for (c = *s++; c != NUL; c = *s++)
	if (c == '*' && *s == '/') {
	    *s = NUL;
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



static VOID	gen_body (state_no, is_empty, gen_body_stmt)
    int		state_no;
    BOOLEAN	is_empty;
    transition_matrix_s		gen_body_stmt;
{
    BOOLEAN	is_keep, is_scan, is_ts_null_done;
    int		codop, next;

    is_ts_null_done = FALSE;

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
	reduce (next, is_keep, is_scan, FALSE, is_empty, FALSE);
	break;

    case HashReduce:
	reduce (next, is_keep, is_scan, TRUE, is_empty, FALSE);
	break;

    case ActPrdct:
	fe = lecl_tables.SXS_action_or_prdct_code [next];

	if (fe.kind == IsPredicate) {
	    gen_prdct_seq (state_no, next);
	}
	else {
	    /* ACTION */
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
			put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) { register unsigned char last_char = sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1];");
			put_edit_ap ("sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1] = sxtoupper (last_char);");
			put_edit_ap ("}");
			break;
			
		    case UpperToLower:
			put_edit_ap ("/* Upper_To_Lower (last char) */");
			put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) { register unsigned char last_char = sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1];");
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
			put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) { register unsigned char a_char;");
			put_edit_ap ("register int i;");
			put_edit_ap ("for (i=0;i<sxsvar.sxlv.ts_lgth;i++) {");
			put_edit_apnnl ("a_char = sxsvar.sxlv_s.token_string [i])");
			put_edit_ap ("sxsvar.sxlv_s.token_string [i] = sxtoupper (a_char);");
			put_edit_ap ("}");
			put_edit_ap ("}");
			break;
			
		    case LowerCase:
			put_edit_ap ("/* Lower_Case (token_string) */");
			put_edit_ap ("if (sxsvar.sxlv.mode.with_system_act) { register unsigned char a_char;");
			put_edit_ap ("register int i;");
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
				is_ts_null_done = TRUE;
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
				is_ts_null_done = TRUE;
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
			is_ts_null_done = TRUE;
		    }
		    
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_user_act)");
		    put_edit_apnnl ("(*sxsvar.SXS_tables.scanact) (ACTION, ");
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
	reduce (next, is_keep, is_scan, FALSE, is_empty, TRUE);
	break;
	
    case HashReducePost:
	reduce (next, is_keep, is_scan, TRUE, is_empty, TRUE);
	break;
    default:
#if EBUG
      sxtrap("st_to_c","unknown switch case #2");
#endif
      break;
    }
}



static	void gen_prdct_seq (state_no, y)
    int		state_no, y;
{
    int		x;
    BOOLEAN	is_empty;
    BOOLEAN	is_in_la = NEXT (lecl_tables.S_transition_matrix [state_no] [1]) != 0;

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
		put_edit_ap ("{register int sxchar;");

		if (is_in_la)
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_prdct && ((sxchar = sxlanext_char ()) == '\\n' || sxchar == EOF)) {");
		else
		    put_edit_ap ("if (sxsvar.sxlv.mode.with_system_prdct && ((sxchar = sxlafirst_char ()) == '\\n' || sxchar == EOF)) {\nsxlaback (1);");

		gen_body (state_no, is_empty, fe.stmt);
		put_edit_ap ("}");
		break;

	    case NotIsLastCol:
		put_edit_ap ("{register int sxchar;");

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

	    put_edit_apnnl ("(*sxsvar.SXS_tables.scanact) (PREDICATE, ");
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



static BOOLEAN	less_equal (less_equal_i, less_equal_j)
    int		less_equal_i, less_equal_j;
{
    return transition_matrix_line [less_equal_i] <= transition_matrix_line [less_equal_j];
}



static BOOLEAN	equal (equal_i, equal_j)
    int		equal_i, equal_j;
{
    return transition_matrix_line [equal_i] == transition_matrix_line [equal_j];
}



static void	gen_state (state_no)
    int		state_no;
{
    BOOLEAN	is_error, is_mref, is_empty;
    register int	gen_state_i;
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
	if (is_mref) {
	    put_edit_apnnl ("STATE_");
	    put_edit_apnb (state_no);
	    put_edit_ap ("_scan:");
	}

	put_edit_ap ("current_class = char_to_class (sxnextchar ());");
    }
    else if (sxba_bit_is_set (scan_in_la, state_no)) {
	if (is_mref) {
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

    is_error = FALSE;
    put_edit_ap ("#ifdef LDBG");
    put_edit_apnnl ("printf (\"state_no = %d, current_char = '%s', current_class = %d\\n\", ");
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
		gen_label (TRUE, sc_set);
		sxba_or (sc1_set, sc_set);
		gen_body (state_no, is_empty, stmt);
	    }
	    else
		is_error = TRUE;
	}
    }

    if (is_error) {
	sxba_not (sc1_set);
	gen_label (FALSE, sc1_set);
	gen_error (state_no);
    }

    put_edit_ap ("}");
    put_edit_nl (1);
    put_edit_ap ("/**************************************************************************/");
    put_edit_nl (1);
    sxfree (sc1_set);
    sxbm_free (sc_to_sc_set);
}



static int	through (xprod)
    register int	xprod;
{
    BOOLEAN	is_keep = FALSE;
    transition_matrix_s		through_stmt;

    for (;;) {
	through_stmt = lecl_tables.SXS_action_or_prdct_code [xprod].stmt;

	if (KEEP (through_stmt)) {
	    is_keep = TRUE;
	}

	switch (CODOP (through_stmt)) {
	case State:
	    if (is_keep) {
		return NEXT (through_stmt);
	    }

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
    long	date_time;
    char	*pct;
    int		init_state, codop, next;
    BOOLEAN	state_status, next_state_status;
    static char		ME [] = "st_to_c";
    static char		Usage [] = "Usage:\t%s language\n";

    sxinitialise(next_state_status); /* pour faire taire gcc -Wuninitialized */
    if (sxstdout == NULL) {
      sxstdout = stdout;
    }
    if (sxstderr == NULL) {
      sxstderr = stderr;
    }

    if (argc != 2) {
	fprintf (sxstderr, Usage, ME);
	SXEXIT (3);
    }

    prgentname = argv [1];

    if (!lecl_read (&lecl_tables, prgentname))
	SXEXIT (2);

    if (lecl_tables.S_tbl_size.S_is_non_deterministic) {
	fprintf (sxstderr, "The NDFSA processing is not yet implemented.\n");
	SXEXIT (2);
    }

    string = varstr_alloc (256);

    to_be_sorted = (int*) sxalloc (lecl_tables.S_tbl_size.S_last_simple_class_no + 1, sizeof (int));
    ref_no = (int*) sxcalloc (max_state_no + 1, sizeof (int));
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

    is_dummy_hash = FALSE;
    is_first_col = FALSE;
    is_last_col = FALSE;
    is_user_action_or_prdct = FALSE;
    is_stmt_5_0 = FALSE;
    is_la = FALSE;
    is_post_action = FALSE;
    is_user_prdct = FALSE;
    is_keep_shr = FALSE;
    is_shr = FALSE;
    is_keep_sr = FALSE;
    is_sr = FALSE;
    is_ascii = TRUE;
    is_keep_sr_to_comments = FALSE;
    is_sr_to_comments = FALSE;
    is_erase = FALSE;
    is_include = FALSE;
    is_mark = FALSE;
    is_release = FALSE;

    for (i = 128; i <= lecl_tables.S_tbl_size.S_last_char_code; i++)
	if (lecl_tables.S_char_to_simple_class [i] != 1) {
	    is_ascii = FALSE;
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
		    is_dummy_hash = TRUE;

	    if (next == 0)
		if (codop == Reduce)
		    is_stmt_5_0 = TRUE;

	    switch (codop) {
	    case Reduce:
	    if (next == 0) /* comments */ {
		BOOLEAN	is_empty = sxba_bit_is_set (is_ts_empty, i);

		if (SCAN (stmt)) {
		    if (is_empty) {
			sxba_1_bit (scan, 1);
			sxba_1_bit (is_ref, 1);
		    }

		    if (KEEP (stmt))
			is_keep_sr_to_comments = TRUE;
		    else if (!is_empty)
			is_sr_to_comments = TRUE;
		}
		else {
		    if (is_empty)
			sxba_1_bit (is_ref, 1);
		}
	    }
	    else
		if (SCAN (stmt)) {
		    if (KEEP (stmt))
			is_keep_sr = TRUE;
		    else
			is_sr = TRUE;
		}

		break;

	    case HashReduce:
		if (SCAN (stmt)) {
		    if (KEEP (stmt))
			is_keep_shr = TRUE;
		    else
			is_shr = TRUE;
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

		case NextLookAhead:
		    is_la = TRUE;
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
	    is_user_action_or_prdct = TRUE;

	    if (fe.kind == IsPredicate)
		is_user_prdct = TRUE;
	}

	if (is_hash_reduce [codop])
	    if (next == 0)
		is_dummy_hash = TRUE;

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
	    is_post_action = TRUE;

	case Reduce:
	    if (next == 0) {
		if (SCAN (stmt)) {
		    if (KEEP (stmt))
			is_keep_sr_to_comments = TRUE;
		    else
			is_sr_to_comments = TRUE;
		}
	    }
	    else
		if (SCAN (stmt)) {
		    if (KEEP (stmt))
			is_keep_sr = TRUE;
		    else
			is_sr = TRUE;
		}

	    break;

	case HashReducePost:
	    is_post_action = TRUE;

	case HashReduce:
		if (SCAN (stmt)) {
		    if (KEEP (stmt))
			is_keep_shr = TRUE;
		    else
			is_shr = TRUE;
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

	case Error:
	case Reduce:
	case HashReduce:
	case ReducePost:
	case HashReducePost:
	case ActPrdct:
	  if (fe.kind == IsPredicate && fe.is_system) {
		if (fe.action_or_prdct_no == 1 /* is_first_col */  || fe.action_or_prdct_no == 4 /* ^is_first_col */ )
		    is_first_col = TRUE;
		else if (fe.action_or_prdct_no == 5 /* is_last_col */  || fe.action_or_prdct_no == 6 /* ^is_last_col */ )
		    is_last_col = TRUE;
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
			is_mark = TRUE;
			break;

		    case 11:
			/* Release */
			is_release = TRUE;
			break;

		    case 5:
			/* Erase */
			is_erase = TRUE;
			break;

		    case 6:
			/* include */
			is_include = TRUE;
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
    put_edit_apnnl ("   *  on ");
    date_time = time (0);
    pct = ctime (&date_time);
    *(pct + 24) = '\0';
    put_edit_apnnl (pct);
    put_edit (71, "*");
    put_edit_ap ("   *  by the SYNTAX (*) lexical constructor LECL                      *");
    put_edit_ap ("   ********************************************************************");
    put_edit_ap ("   *  (*) SYNTAX is a trademark of INRIA.                             *");
    put_edit_ap ("   ******************************************************************** */");
    put_edit_nl (2);
    put_edit_apnnl ("static char what[]=\"@(#)");
    put_edit_apnnl (prgentname);
    put_edit_apnnl ("_scnr.c\t- SYNTAX [unix] - ");
    put_edit_apnnl (pct);
    put_edit_ap ("\";");
    put_edit_nl (2);
    put_edit_ap ("#include \"sxunix.h\"");
    put_edit_ap ("#include <ctype.h>");
    put_edit_ff ();
    put_edit_ap ("/*     S T A T I C S     */");
    put_edit_nl (1);
    out_S_char_to_simple_class ();
    put_edit_nl (1);
    put_edit_ff ();
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
	put_edit_ap ("#define ts_null()	(sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth] = NUL)");
    }

    put_edit_nl (1);
    put_edit_ap ("/* computes the class of the character \'c\' */");
    put_edit_ap ("#define char_to_class(c)    ((S_char_to_simple_class + 128) [c])");

    if (!lecl_tables.S_tbl_size.S_are_comments_erased) {
	put_edit_ff ();
	put_edit_ap ("/*     P R O C E D U R E    */");
    }

    if (!lecl_tables.S_tbl_size.S_are_comments_erased) {
	put_edit_ap ("/* comments processing */");
	put_edit_nl (1);
	put_edit_ap ("static void comments_put (str, lgth)");
	put_edit_ap ("char   *str;");
	put_edit_ap ("int     lgth;");
	put_edit_ap ("{");
	put_edit_ap ("register int lgth_comment;");
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


    put_edit_ff ();
    put_edit_ap ("/*     M A J O R    E N T R Y     */");
    put_edit_apnnl ("SXVOID ");
    put_edit_apnnl (prgentname);
    put_edit_ap ("_scan_it () {");
    put_edit_ap ("unsigned char current_class;");

    if (is_la)
	put_edit_ap ("unsigned char current_class_in_la;");

    if (is_post_action)
	put_edit_ap ("int post_action_no;");

    if (!lecl_tables.S_tbl_size.S_are_comments_erased)
	put_edit_ap ("sxsvar.sxlv.terminal_token.comment = NULL;");

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
    put_edit_ap ("sxsvar.sxlv.terminal_token.string_table_entry = EMPTY_STE;");

    if (sxba_bit_is_set (scan, 1)) {
	put_edit_ap ("goto STATE_1;");
    }


/* On genere les etats */

    for (i = 1; i <= max_state_no; i++) {
	if (ref_no [i] != 1 || i == 1)
	    gen_state (i);
    }


/* generation de l'epilogue */

    put_edit_ff ();

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
	put_edit_ap ("{register int look_ahead;");
	put_edit_ap ("if ((look_ahead = (*sxsvar.SXS_tables.check_keyword) (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth)) != 0) {");
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
	    put_edit_ap ("sxput_error (sxsvar.sxlv.terminal_token.source_index,");
	    put_edit_ap ("sxsvar.SXS_tables.S_global_mess [2],");
	    put_edit_ap ("sxsvar.sxtables->err_titles [2]);");
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
	put_edit_ap ("(*sxsvar.SXS_tables.scanact) (ACTION, post_action_no);");
	put_edit_ap ("if (sxsvar.sxlv.terminal_token.lahead == 0) {");
	put_edit_ap ("if (sxsvar.sxlv.terminal_token.string_table_entry == ERROR_STE)");
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
    put_edit_ff ();
    put_edit_ap ("SXVOID ");
    put_edit_apnnl (prgentname);
    put_edit_ap ("_scanner (what_to_do, arg)");
    put_edit_ap ("int what_to_do;");
    put_edit_ap ("struct sxtables *arg;");
    put_edit_ap ("{ switch (what_to_do) {");
    put_edit_ap ("case OPEN:");
    put_edit_ap ("/* new language, new tables: prepare new local variables */");
#if 0
    put_edit_apnnl ("sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, \"");
    put_edit_apnnl (prgentname);
    put_edit_ap ("_scanner\");");
#endif
    put_edit_ap ("sxsvar.sxtables = arg;");
    put_edit_ap ("sxsvar.SXS_tables = arg->SXS_tables;");
    put_edit_ap ("sxsvar.sxlv_s.token_string = (char *) sxalloc (sxsvar.sxlv_s.ts_lgth_use");
    put_edit_ap ("= 120, sizeof (char));");
#if 0
    put_edit_ap ("sxsvar.SXS_tables.S_char_to_simple_class [EOF] = 2 /* EOF class */ ;");
#endif

    if (lecl_tables.S_tbl_size.S_counters_size != 0) {
	put_edit_apnnl ("sxsvar.sxlv_s.counters = (long *)sxcalloc (");
	put_edit_apnb (lecl_tables.S_tbl_size.S_counters_size);
	put_edit_ap (", sizeof (long));");
    }
    else
	put_edit_ap ("sxsvar.sxlv_s.counters = NULL;");

	put_edit_ap ("sxsvar.sxlv.mode.errors_nb = 0;");
	put_edit_ap ("sxsvar.sxlv.mode.is_silent = FALSE;");
	put_edit_ap ("sxsvar.sxlv.mode.with_system_act = TRUE;");
	put_edit_ap ("sxsvar.sxlv.mode.with_user_act = TRUE;");
	put_edit_ap ("sxsvar.sxlv.mode.with_system_prdct = TRUE;");
	put_edit_ap ("sxsvar.sxlv.mode.with_user_prdct = TRUE;");


    put_edit_ap ("break;");
    put_edit_nl (1);
    put_edit_ap ("case INIT:");
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

    put_edit_ap ("case ACTION:");
    put_edit_apnnl (prgentname);
    put_edit_ap ("_scan_it ();");
    put_edit_ap ("break;");
    put_edit_nl (1);

    put_edit_ap ("case CLOSE:");
    put_edit_ap ("/* end of language, local variables are freed */");
    put_edit_ap ("sxfree (sxsvar.sxlv_s.token_string);");
    put_edit_ap ("sxsvar.sxlv_s.token_string = NULL;");
    put_edit_ap ("(*sxsvar.SXS_tables.recovery) (CLOSE);");
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
    SXEXIT (0);

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
