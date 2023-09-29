/*   C O D E _ G E N E R A T I O N   */ 

/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */

/*******************************************************************/
/*                                                                 */
/*                                                                 */
/*  This program has been translated from lecl_code_generation.pl1 */
/*  on Friday the eigthteenth of April, 1986, at 15:57:46,         */
/*  on the Multics system at INRIA,                                */
/*  by the release 3.3g PL1_C translator of INRIA,                 */
/*         developped by the "Langages et Traducteurs" team,       */
/*         using the SYNTAX (*), FNC and Paradis systems.          */
/*                                                                 */
/*                                                                 */
/*******************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                             */
/*******************************************************************/

/* Le 24/01/2003
   Pour avoir les bonnes macros pour STMT, KEEP, SCAN, CODOP, NEXT ... */
#define SXS_MAX (SHRT_MAX+1)
#include "sxunix.h"
#include "sxba.h"
#include "varstr.h"
#include "lecl_ag.h"
#include "sxsstmt.h"
char WHAT_LECLCG[] = "@(#)SYNTAX - $Id: lecl_cg.c 650 2007-06-21 15:39:55Z syntax $" WHAT_DEBUG;

static int	gen_action_code ();
static int	install_action ();
static BOOLEAN	is_unique () /* bullshit */ ;


int	tmhb1, tmhb2, predhb1;

#define Dummy 10
/* Codop bidon pour les transitions vers des post-actions */

static BOOLEAN	is_a_shift_code [Dummy + 1] = {FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE};

#define max(x,y) ((x)>(y))?(x):(y)

extern int	get_a_partition ();
extern int	equality_sort ();
static SXBA	/* max_re */ is_re_erased;
static transition_matrix_item	**tm /* 1:tmhb1, 1:tmhb2 */ ;
static transition_matrix_item	*tm_line;
static SXBA	/* tmhb1 */ major_state_set, to_be_processed, eq, prev, already_processed,
    has_predicate, has_action, has_nondeter, accessible_states_set, has_la, la_checked,
    scan_reached, pure_action_set;
static SXBA	/* smax */ sc_set;
static int	i, j, k, new_simple_class, old_simple_class, x, y, z, t, xcc, sc, state_no, next_state_no,
     partition_no, tm_tilde_lgth, max_ref, ref_lgth, codop_lgth, keep_lgth, scan_lgth, stmt_lgth, re_no, action_no, prod_size,
     new_state_no, old_state_no, prdct;
static char	current_state_kind, next_state_kind /* bit (6) */ ;
static SXBA	/* tmhb1 */ *pred, *equiv /* 1:predhb1 */ ;
static SXBA	/* predhb1 */ valid_action, del_action, scan_action;
static struct equiv_class_item {
	   int	repr, eq_lnk;
       }	*equiv_class /* 0:tmhb1 */ ;
static int	*state_to_action, *old_state_to_new_state_no, *new_state_to_old_state_no,
    *state_to_nondeter /* 1:tmhb1 */ ;
static SXBA	/* tm_tilde_lgth */ *tm_tilde /* 1:smax */ ;
static struct action_or_prdct_code_item		*productions /* 1:prod_size */ ;
static int	*to_be_sorted, *old_sc_to_new_sc /* 1:smax */ ;
static transition_matrix_item	*transition_matrix_line;
static transition_matrix_item	stmt;
static struct action_or_prdct_code_item		floyd_evans;
static SXBA	 /* xprdct_to_ate */ is_user_prdct_in_la;

/* Pour get_la_length */

static SXBA	/* tmhb1 */ is_processed, is_cycle;
static int	*la_value /* 1:tmhb1 */ ;




static BOOLEAN	less_equal (less_equal_i, less_equal_j)
    int		less_equal_i, less_equal_j;
{
    register SXBA	lj;
    register int	less_equal_x;

    if ((less_equal_x = sxba_first_difference (tm_tilde [less_equal_i], lj = tm_tilde [less_equal_j])) == -1)
	return TRUE;

    return sxba_bit_is_set (lj, less_equal_x);
}



static BOOLEAN	equal (equal_i, equal_j)
    int		equal_i, equal_j;
{
    return sxba_first_difference (tm_tilde [equal_i], tm_tilde [equal_j]) == -1;
}



static VARSTR	token_name_list (token_set, string)
    SXBA	token_set;
    VARSTR	string;
{
    int		token_name_list_x, ner;

    token_name_list_x = sxba_scan (token_set, 0);
    SXBA_0_bit (token_set, token_name_list_x);
    ner = ers_disp [token_name_list_x].reduc;

    if (ner < 0)
	ner = -ner;

    string = varstr_catenate (string, sxstrget (ner));

    if (!sxba_is_empty (token_set))
	return token_name_list (token_set, varstr_catenate (string, ", "));
    else
	return string;
}



static BOOLEAN	re_erased (re_erased_re_no)
    int		re_erased_re_no;
{
    int		origine, delta, re_erased_l, re_erased_t, re_erased_x, erase_ate;
    char	kind /* bit (2) */ ;
    SXBA	/* max_re_lgth */ re_erased_follow;
    char	/* bit (2) */ *erase_kind /* 1:max_re_lgth */ ;


/*
erase_kind = "00"b	=>	il existe un chemin de re_erased_x a EOT qui n'est pas supprime
erase_kind = "01"b	=>	tous les chemins de re_erased_x a EOT sont supprimes
erase_kind = "11"b	=>	tous les chemins de re_erased_x a EOT sont supprimes et il y
			a un erase sur chacun des chemins
*/

    sxinitialise (kind);
    erase_kind = (char*) sxalloc (max_re_lgth + 1, sizeof (char));
    origine = ers_disp [re_erased_re_no].prolon;
    delta = ers_disp [re_erased_re_no + 1].prolon - origine;
    erase_kind [delta] = 1 /* "01" b */ ;
    erase_ate = sxstrsave ("@ERASE");

    for (delta = delta - 1; delta >= 0; delta--) {
	re_erased_x = origine + delta;
	re_erased_l = ers [re_erased_x].lispro;
	re_erased_follow = suivant [re_erased_x];
	re_erased_t = delta;
	kind = 3 /* "11" b */ ;

	while ((re_erased_t = sxba_scan (re_erased_follow, re_erased_t)) > 0) {
	    kind = kind & erase_kind [re_erased_t];
	}

	if (delta > 0) {
	  if (kind == 1 /* "01" b */ ) {
	    if (!ers [re_erased_x].is_erased) {
		    if (re_erased_l >= 0)
			kind = 0 /* "00" b */ ;
		    else if (action_or_prdct_to_ate [re_erased_l] == erase_ate)
			kind = 3 /* "11" b */ ;
	    }
	  }
	  erase_kind[delta] = kind;
	}
    }

    sxfree (erase_kind);
    return kind != 0;
}





static transition_matrix_item	test_ref (is_keep, is_scan, codop, ref)
    BOOLEAN	is_keep, is_scan;
    int		codop, ref;
{
    int		test_ref_re_no, re_ref, test_ref_x;

    if (ref < 0) {
	re_ref = ref = -ref;

	if (ers [ers_disp [re_ref].prolon].lispro == -2) {
	    codop = HashReduce;
	}

	test_ref_re_no = ers_disp [re_ref].master_token;
	SXBA_1_bit (is_re_generated, test_ref_re_no);


/* if (test_ref_re_no = comments_re_no) && sxba_bit_is_set (is_re_erased, re_ref) 
   & codop = Reduce on ne transforme plus la reduction vers comments
   en un "goto State 1" pour ne pas alourdir le traitement des codop SameState
   et State (il faut tester si on n'est pas dans l'etat 1 afin de positionner
   terminal_token.source_index a la bonne valeur) */

	if (test_ref_re_no == include_re_no) {
	    floyd_evans.stmt = STMT (0, 0, State, 1);
	    floyd_evans.action_or_prdct_no = Include;
	    floyd_evans.is_system = TRUE;
	    /* floyd_evans.is_predicate = FALSE; */
	    floyd_evans.kind = IsAction;
	    floyd_evans.param = test_ref_x = ers_disp [re_ref].post_action;


/* goto State 1 */

/* La post_action eventuelle associee a include est traitee differemment 
   (passee dans le parametre de l'action include ) */

	    if (test_ref_x != 0) {
		floyd_evans.param = atoi (sxstrget (action_or_prdct_to_ate [test_ref_x]) + 1);
	    }

	    codop = Dummy;
	    ref = install_action (floyd_evans);
	}
	else {
	    ref = ers_disp [test_ref_re_no].reduc;

	    if (ref < 0)
		ref = 0;
	    else
		ref = ate_to_t [ref];

	    if (ref == termax && is_scan)
		/* On enleve le scan */
		is_keep = is_scan = FALSE;


/* Traitement des post_actions */

	    if (ers_disp [re_ref].post_action != 0) {
		ref = gen_action_code (ers_disp [re_ref].post_action, 0, 0, ref, (codop == HashReduce) ? HashReducePost :
		     ReducePost);
		codop = Dummy;
	    }
	}
    }

    return STMT (is_keep, is_scan, codop, ref);
}



static transition_matrix_item	gen_code (current_state_no, gen_code_current_state_kind,
					  gen_code_next_state_no, is_del, is_scan)
    int		current_state_no, gen_code_next_state_no;
    BOOLEAN	is_del, is_scan;
    char	gen_code_current_state_kind;
{
    BOOLEAN	is_keep;
    int		codop, ref;
    char	gen_code_next_state_kind;

    ref = gen_code_next_state_no;
    
    /* Next line expanded for the bugged apollo C compiler */
    /* is_keep = is_scan ? !is_del : FALSE; */
    if (is_scan)
	is_keep = !is_del;
    else
	is_keep = FALSE;

    if (gen_code_next_state_no > 0) {
	/* Shift */
	gen_code_next_state_kind = fsa [gen_code_next_state_no].state_kind;

	if (gen_code_next_state_kind == 0)
	    ref = 0, /* transition vers un etat vide */ codop = Error;
	else if ((gen_code_next_state_kind & ACT) && !(gen_code_next_state_kind & MIXTE)) {
	    codop = ActPrdct;

	    if (!is_scan) {
#if 0
		if (!(gen_code_current_state_kind & ACT) || (gen_code_current_state_kind & MIXTE)) {
		    /* On memorise les points de depart */
		    SXBA_1_bit (la_checked, gen_code_next_state_no);
		}
#endif
		max_la = 1;
	    }
#if 0
	    else {
		/* Atteint sans test du look_ahead (gen_code_current_state_kind & ACT == FALSE) */
		SXBA_1_bit (scan_reached, gen_code_next_state_no);
	    }
#endif
	}
	else if (gen_code_next_state_kind & (PRDCT + NONDETER))
	    is_keep = is_scan = FALSE /* prudence */, codop = ActPrdct;
	else if ((gen_code_next_state_kind & REDUCE) && !(gen_code_next_state_kind & MIXTE) && is_optimize) {
	    /* Shift_Reduce */
	    ref = fsa_trans [fsa [gen_code_next_state_no].transition_ref].next_state_no;
	    codop = Reduce;
	    SXBA_0_bit (major_state_set, gen_code_next_state_no);
	}
	else {
	    /* Shift */
	    if (gen_code_next_state_kind & LA) {

		SXBA_1_bit (has_la, current_state_no);
		is_keep = is_scan = FALSE;

		if (gen_code_current_state_kind & LA)
		    codop = NextLookAhead;
		else
		    codop = FirstLookAhead;
	    }
	    else
		codop = State;
	}
    }
    else {
	/* Reduce */
	is_keep = is_scan = FALSE;
	codop = Reduce;
	max_la = 1;
    }

    return test_ref (is_keep, is_scan, codop, ref);
}



static transition_matrix_item	gen_action (/* gen_action_action_no, */gen_action_next_state_no)
     int		/* gen_action_action_no, */gen_action_next_state_no;
{
    int		codop, ref;
    char	gen_action_next_state_kind;

    gen_action_next_state_kind = fsa [gen_action_next_state_no].state_kind;

    if ((gen_action_next_state_kind & REDUCE) && !(gen_action_next_state_kind & MIXTE) && is_optimize) {
	ref = fsa_trans [fsa [gen_action_next_state_no].transition_ref].next_state_no;
	codop = Reduce;
	SXBA_0_bit (major_state_set, gen_action_next_state_no);
    }
    else {
	ref = gen_action_next_state_no;

	if ((gen_action_next_state_kind & ACT) && !(gen_action_next_state_kind & MIXTE))
	    codop = ActPrdct;
	else
	    codop = State;
    }

    return test_ref (0, 0, codop, ref);
}



static VOID gen_nondeter_code (V, is_keep, is_scan, ref, codop)
    BOOLEAN	is_keep, is_scan;
    int		V, ref, codop;
{
    struct action_or_prdct_code_item	fe;

    fe.is_system = TRUE;
    fe.param = 0;
    fe.action_or_prdct_no = V;
    fe.kind = IsNonDeter;
    fe.stmt = STMT (is_keep, is_scan, codop, (ref < 0) ? -ref : ref);
    productions [++xprod] = fe;
}



static VOID gen_prdct_code (V, is_keep, is_scan, ref, codop)
    BOOLEAN	is_keep, is_scan;
    int		V, ref, codop;
{
    struct action_or_prdct_code_item	fe;
    char	*string;
    char	first;
    BOOLEAN	is_not;

    fe.is_system = TRUE;
    fe.param = 0;
    sxinitialise (fe.action_or_prdct_no); /* pour faire taire gcc -Wuninitialized */

    if (V != 0) {
	string = sxstrget (action_or_prdct_to_ate [V]);

	if (*string == '^') {
	    is_not = TRUE;
	    string++;
	}
	else
	    is_not = FALSE;

	string++;

	if (*string == 'T')
	    fe.action_or_prdct_no = IsTrue;
	else if (*string == 'I') {
	    /* System */
	    string += 3;
	    first = *string;

	    if (first == 'F' /* Is_First_Col */ )
		fe.action_or_prdct_no = is_not ? NotIsFirstCol : IsFirstCol;
	    else if (first == 'L' /* Is_Last_Col */ )
		fe.action_or_prdct_no = is_not ? NotIsLastCol : IsLastCol;
	    else if (first == 'S' /* Is_Set */ ) {
		fe.action_or_prdct_no = is_not ? IsReset : IsSet;
		fe.param = atoi (string + 5);
		counters_size = max (counters_size, fe.param + 1);
	    }
	    else if (first == 'R' /* Is_Reset */ ) {
		fe.action_or_prdct_no = is_not ? IsSet : IsReset;
		fe.param = atoi (string + 7);
		counters_size = max (counters_size, fe.param + 1);
	    }
	}
	else {
	    fe.is_system = FALSE;
	    fe.action_or_prdct_no = atoi (string);

	    if (is_not)
		fe.param = 1;
	}
    }

    /* fe.is_predicate = TRUE; */
    fe.kind = IsPredicate;
    fe.stmt = STMT (is_keep, is_scan, codop, (ref < 0) ? -ref : ref);
    productions [++xprod] = fe;
}



static int	gen_action_code (V, is_keep, is_scan, ref, codop)
    BOOLEAN	is_keep, is_scan;
    int		V, ref, codop;
{
    struct action_or_prdct_code_item	fe;
    char	*string;
    char	first;

    fe.is_system = TRUE;
    fe.param = 0;
    string = sxstrget (action_or_prdct_to_ate [V]) + 1;
    first = *string;

    if (first == 'L' /* Lower_To_Upper or Lower_Case */ ) {
	string += 6;

	if (*string == 'T')
	    fe.action_or_prdct_no = LowerToUpper;
	else
	    fe.action_or_prdct_no = LowerCase;
    }
    else if (first == 'U' /* Upper_To_Lower or Upper_Case */ ) {
	string += 6;

	if (*string == 'T')
	    fe.action_or_prdct_no = UpperToLower;
	else
	    fe.action_or_prdct_no = UpperCase;
    }
    else if (first == 'S' /* Set */ ) {
	fe.action_or_prdct_no = Set;
	fe.param = atoi (string + 5);
	counters_size = max (counters_size, fe.param + 1);
    }
    else if (first == 'R' /* Reset or Release */ ) {
	string += 2;

	if (*string == 'l' /* Release */ )
	    fe.action_or_prdct_no = Release;
	else {
/* Reset */
	    fe.action_or_prdct_no = Reset;
	    fe.param = atoi (string + 5);
	    counters_size = max (counters_size, fe.param + 1);
	}
    }
    else if (first == 'I' /* Incr */ ) {
	fe.action_or_prdct_no = Incr;
	fe.param = atoi (string + 6);
	counters_size = max (counters_size, fe.param + 1);
    }
    else if (first == 'D' /* Decr */ ) {
	fe.action_or_prdct_no = Decr;
	fe.param = atoi (string + 6);
	counters_size = max (counters_size, fe.param + 1);
    }
    else if (first == 'E' /* Erase */ )
	fe.action_or_prdct_no = Erase;
    else if (first == 'P' /* Put */ ) {
	fe.action_or_prdct_no = Put;
	string += 5;
	fe.param = *string;
    }
    else if (first == 'M' /* Mark */ )
	fe.action_or_prdct_no = Mark;
    else {
	fe.is_system = FALSE;
	fe.action_or_prdct_no = atoi (string);
    }

    /* fe.is_predicate = FALSE; */
    fe.kind = IsAction;
    fe.stmt = STMT (is_keep, is_scan, codop, (ref < 0) ? -ref : ref);
    return install_action (fe);
}




static VOID la_reduc (/* max_state_no, */max_extended_state_no, la_reduc_pred, action_set, state_set, la_reduc_eq)
     int		/* max_state_no, */max_extended_state_no;
    SXBA	*la_reduc_eq, *la_reduc_pred, action_set, state_set;
{
    /* substr(la_reduc_pred(i),j,1) ssi kind(j)="111"b & i est dans succ(j)
   action_set = ensemble des actions valides apres look_ahead
   state_set = ensemble des etats a traiter (kind="111"b)

   out: la_reduc_eq(j) , j est dans action_set : ensemble des etats equivalents a j */
    register int		action, la_reduc_x, la_reduc_y;
    register SXBA	/* max_state_no */ la_reduc_equiv, la_reduc_prev;
    register SXBA	/* max_extended_state_no */ not_equiv, la_reduc_already_processed;

    not_equiv = sxba_calloc (max_extended_state_no + 1);
    la_reduc_already_processed = sxba_calloc (max_extended_state_no + 1);
    action = 0;

    while ((action = sxba_scan (action_set, action)) > 0) {
	la_reduc_equiv = la_reduc_eq [action];
	sxba_copy (la_reduc_equiv, state_set);
	sxba_copy (not_equiv, action_set);
	SXBA_0_bit (not_equiv, action);
	sxba_empty (la_reduc_already_processed);
	

/* Tous les etats de la_reduc_equiv qui referencent des etats de not_equiv sont 
   transferes de la_reduc_equiv a not_equiv, on s'arrete quand c'est stable */

	while ((la_reduc_x = sxba_scan (not_equiv, 0)) > 0) {
	    SXBA_1_bit (la_reduc_already_processed, la_reduc_x);
	    la_reduc_prev = la_reduc_pred [la_reduc_x];

	    /* simulation de sxba_or (not_equiv, la_reduc_pred [la_reduc_x]) */
	    la_reduc_y = 0;

	    while ((la_reduc_y = sxba_scan (la_reduc_prev, la_reduc_y)) > 0) {
		SXBA_1_bit (not_equiv, la_reduc_y);
	    }
	    
	    sxba_minus (not_equiv, la_reduc_already_processed);
	    sxba_minus (la_reduc_equiv, la_reduc_prev);
	}
    }

    sxfree (la_reduc_already_processed);
    sxfree (not_equiv);
}



static VOID put_in_tm_tilde (put_in_tm_tilde_i, put_in_tm_tilde_j, put_in_tm_tilde_stmt)
    transition_matrix_item	put_in_tm_tilde_stmt;
    int		put_in_tm_tilde_i, put_in_tm_tilde_j;
{
    register SXBA	linei;
    register int	put_in_tm_tilde_x, n, codop;

    if (put_in_tm_tilde_stmt == 0)
	return;
    
    put_in_tm_tilde_x = (put_in_tm_tilde_j - 1) * stmt_lgth;
    linei = tm_tilde [put_in_tm_tilde_i];

    /* On suppose linei initialement vide */
    if ((codop = CODOP (put_in_tm_tilde_stmt)) != 0) {
	for (n = 1; n <= codop_lgth; n++) {
	    put_in_tm_tilde_x++;

	    /* assign1bit (linei, put_in_tm_tilde_x + n, codop & (1 << (codop_lgth - n))); */
	    if ((codop & (1 << (codop_lgth - n))) != 0) {
		SXBA_1_bit (linei, put_in_tm_tilde_x);
	    }
	}
    }

    /* Le 21/12/04 */
    /* On tient compte de "KEEP" ds tm_tilde sinon on obtient des equivalences (meme classe) entre
       des caracteres qui sont supprimes et gardes par transition depuis le meme etat */ 
    put_in_tm_tilde_x++;

    if (KEEP (put_in_tm_tilde_stmt))
      SXBA_1_bit (linei, put_in_tm_tilde_x);

    /* Le 27/12/04 */
    /* On tient compte de "SCAN" ds tm_tilde sinon on obtient des equivalences (meme classe) entre
       des caracteres qui sont supprimes et gardes par transition depuis le meme etat */ 
    put_in_tm_tilde_x++;

    if (SCAN (put_in_tm_tilde_stmt))
      SXBA_1_bit (linei, put_in_tm_tilde_x);
    /* Fin ajout ... */

    if (NEXT (put_in_tm_tilde_stmt) != 0) {
	for (n = 1; n <= ref_lgth; n++) {
	    put_in_tm_tilde_x++;

	    /* assign1bit (linei, put_in_tm_tilde_x + n, put_in_tm_tilde_stmt & (1 << (ref_lgth - n))); */
	    if ((put_in_tm_tilde_stmt & (1 << (ref_lgth - n))) != 0) {
		SXBA_1_bit (linei, put_in_tm_tilde_x);
	    }
	}
    }
}



static	int retrieve_prdct_or_nondeter_sequence (retrieve_prdct_or_nondeter_sequence_x, kind)
    int		retrieve_prdct_or_nondeter_sequence_x;
    char	kind; /* IsPredicate ou IsNonDeter */
{
    /* Cette procedure recherche si la sequence de predicats ou nondeter comprise entre
       retrieve_prdct_or_nondeter_sequence_x et xprod ne figure pas deja dans productions.
       retourne 0 si nouvelle ou l'index initial si elle existe deja.
       Cette procedure est necessaire Ex  .... (a b&1 | c d&1) ... */
    register int				retrieve_prdct_or_nondeter_sequence_i, retrieve_prdct_or_nondeter_sequence_y, retrieve_prdct_or_nondeter_sequence_z, l1, retrieve_prdct_or_nondeter_sequence_l;
    register struct action_or_prdct_code_item	*xi, *yi;

    retrieve_prdct_or_nondeter_sequence_l = xprod - retrieve_prdct_or_nondeter_sequence_x;

    for (retrieve_prdct_or_nondeter_sequence_y = 1; retrieve_prdct_or_nondeter_sequence_y < retrieve_prdct_or_nondeter_sequence_x - retrieve_prdct_or_nondeter_sequence_l; retrieve_prdct_or_nondeter_sequence_y++) {
	if ((retrieve_prdct_or_nondeter_sequence_z = productions [retrieve_prdct_or_nondeter_sequence_y].kind) == kind) {
	    l1 = 0;

	    for (retrieve_prdct_or_nondeter_sequence_z = retrieve_prdct_or_nondeter_sequence_y; productions [retrieve_prdct_or_nondeter_sequence_z].action_or_prdct_no != 0; retrieve_prdct_or_nondeter_sequence_z++) {
		l1++;
	    }

	    if (retrieve_prdct_or_nondeter_sequence_l == l1) {
		xi = &(productions [retrieve_prdct_or_nondeter_sequence_x]);
		yi = &(productions [retrieve_prdct_or_nondeter_sequence_y]);

		for (retrieve_prdct_or_nondeter_sequence_i = 0; retrieve_prdct_or_nondeter_sequence_i <= retrieve_prdct_or_nondeter_sequence_l; retrieve_prdct_or_nondeter_sequence_i++) {
		    if (xi->stmt != yi->stmt
			|| xi->action_or_prdct_no != yi->action_or_prdct_no
			|| xi->param != yi->param
			|| xi->is_system != yi->is_system)
			break;

		    xi++, yi++;
		}

		if (retrieve_prdct_or_nondeter_sequence_i > retrieve_prdct_or_nondeter_sequence_l)
		    return retrieve_prdct_or_nondeter_sequence_y;
	    }

	    retrieve_prdct_or_nondeter_sequence_y = retrieve_prdct_or_nondeter_sequence_z;
	}
	else if (retrieve_prdct_or_nondeter_sequence_z != IsAction) {
	    while (productions [retrieve_prdct_or_nondeter_sequence_y].action_or_prdct_no != 0)
		retrieve_prdct_or_nondeter_sequence_y++;
	}
    }

    return 0;
}



static int	skip_true (skip_true_state_no)
    int		skip_true_state_no;
{
    int		skip_true_x;

    if (skip_true_state_no < 0)
	return skip_true_state_no;

    if (!fsa [skip_true_state_no].state_kind & PRDCT)
	return skip_true_state_no;

    skip_true_x = fsa [skip_true_state_no].transition_ref;

    if (skip_true_x == fsa [skip_true_state_no + 1].transition_ref - 1 /* Une seule transition */
	&& fsa_trans [skip_true_x].cc_ref == prdct_true_code /* sur &True */ ) {
	SXBA_0_bit (major_state_set, skip_true_state_no);
	return skip_true (fsa_trans [skip_true_x].next_state_no);
    }
    else
	return skip_true_state_no;
}



static int	install_action (fe)
    struct action_or_prdct_code_item	fe;
{
    /* On recherche dans productions si fe existe deja, sinon on l'y met */
    int		install_action_x;
    struct action_or_prdct_code_item	*xp;

    for (install_action_x = 1; install_action_x <= xprod; install_action_x++) {
	xp = productions + install_action_x;

	/* if (!(xp->is_predicate)) */
	if (xp->kind == IsAction)
	    /* action */
	    if (xp->stmt == fe.stmt
		&& xp->action_or_prdct_no == fe.action_or_prdct_no
		&& xp->param == fe.param
		&& xp->is_system == fe.is_system)
		return install_action_x;
    }

    productions [++xprod] = fe;
    return xprod;
}




static int	get_la_length (get_la_length_state_no)
    int		get_la_length_state_no;
{
    int		get_la_length_i, la_val, la;

    if (!sxba_bit_is_set (has_la, get_la_length_state_no))
	return 1;

    if (sxba_bit_is_set (is_cycle, get_la_length_state_no))
	return -1;

    SXBA_1_bit (is_cycle, get_la_length_state_no);

    if (sxba_bit_is_set (is_processed, get_la_length_state_no))
	return la_value [get_la_length_state_no];

    la_val = 0;

    for (get_la_length_i = 1; get_la_length_i <= tmhb2; get_la_length_i++)
	if (CODOP (tm [get_la_length_state_no] [get_la_length_i]) == NextLookAhead) {
	    la = get_la_length (NEXT (tm [get_la_length_state_no] [get_la_length_i])) + 1;

	    if (la == 0)
		return -1;

	    la_val = (la > la_val) ? la : la_val;
	}

    SXBA_1_bit (is_processed, get_la_length_state_no);
    la_value [get_la_length_state_no] = la_val;
    return la_val;
}



static BOOLEAN	is_compatible (statei, statej, symbole)
    int		statei, statej, symbole;
{
    /* Rend vrai ssi les actions (statei, symbole) et (statej, symbole)
       sont 0-indistinguables */
    register transition_matrix_item	stmti, stmtj;

    /* Bug corrige' le 3/8/94
       On teste aussi le state_kind, sinon on pouvait confondre le code a executer
       sur une action avec le code associe' a une transition terminale...
       Bug de'tecte' par HGaravel sur une grammaire de lotos. */
    if (fsa [statei].state_kind != fsa [statej].state_kind)
	return FALSE;
  
    if ((stmti = tm [statei] [symbole]) == (stmtj = tm [statej] [symbole])) {
	return TRUE;
    }

    if (KSC (stmti) != KSC (stmtj)) {
	return FALSE;
    }

    if (is_a_shift_code [CODOP (stmti)]) {
	return TRUE;
    }

    return FALSE;
}



static int	get_next_state (get_next_state_state, symbole)
    int		get_next_state_state, symbole;
{
    /* si l'action (get_next_state_state, symbole) est une transition vers un autre etat
       next alors rend next sinon 0 */
    register transition_matrix_item	get_next_state_stmt = tm [get_next_state_state] [symbole];

    if (is_a_shift_code [CODOP (get_next_state_stmt)])
	return NEXT (get_next_state_stmt);

    return 0;
}



static BOOLEAN	is_in_a_lane (action)
    int		action;
{
    register int	is_in_lane_state_no, is_in_lane_x;
    register BOOLEAN	is_action;
    register transition_matrix_item	is_in_lane_stmt;

    is_in_lane_state_no = 0;

    while ((is_in_lane_state_no = sxba_scan (scan_reached, is_in_lane_state_no)) > 0) {
	is_action = TRUE;

	for (is_in_lane_x = is_in_lane_state_no;
	     is_action && is_in_lane_x != action;
	     is_action = (CODOP (is_in_lane_stmt) != ActPrdct)
	     ? FALSE
	     : sxba_bit_is_set (has_action, is_in_lane_x = NEXT (is_in_lane_stmt))) {
	    is_in_lane_stmt = tm [is_in_lane_x] [state_to_action [is_in_lane_x]];
	}

	if (is_in_lane_x == action) {
	    return TRUE;
	}
    }

    return FALSE;
}



static BOOLEAN	is_utp (is_utp_stmt)
    transition_matrix_item	*is_utp_stmt;
{
    if (SCAN (*is_utp_stmt)) {
	return TRUE;
    }

    switch (CODOP (*is_utp_stmt)) {
    case FirstLookAhead:
    case NextLookAhead:
	return TRUE;

    default:
	return is_unique (is_utp_stmt);
    }
}



static BOOLEAN	is_unique (is_unique_stmt)
    transition_matrix_item	*is_unique_stmt;
{
    register BOOLEAN	is_first, is_equal;
    transition_matrix_item	*is_unique_tm_line, stmt1, stmt_ref;
    register int	is_unique_x, next;
    static transition_matrix_item empty_stmt_ref;

    if (SCAN (*is_unique_stmt)) {
	return FALSE;
    }

    switch (CODOP (*is_unique_stmt)) {
    case State:
    case SameState:
    case ActPrdct:
	next = NEXT (*is_unique_stmt);
	is_unique_tm_line = tm [next];

	if (sxba_bit_is_set (has_action, next)) {


/* Les sequences d'actions n'etant pas generees "dans l'ordre",
   verification dynamique */
	    if (!is_in_a_lane (next)) {
		is_unique (&(is_unique_tm_line [state_to_action [next]]));
	    }

	    return FALSE;
	}

	is_first = TRUE;
	is_equal = TRUE;
	stmt_ref = empty_stmt_ref;  /* pour faire taire gcc -Wuninitialized */

	for (is_unique_x = 1; is_equal && is_unique_x <= tmhb2; is_unique_x++) {
	    stmt1 = is_unique_tm_line [is_unique_x];

	    if (CODOP (stmt1) != Error) {
		if (is_first) {
		    is_first = FALSE;
		    is_equal = is_utp (&stmt1);
		    stmt_ref = stmt1;
		}
		else {
		    if (!is_utp (&stmt1) || stmt_ref != stmt1) {
			is_equal = FALSE;
		    }
		}
	    }
	}

	if (is_equal) {
	    *is_unique_stmt = stmt_ref;
	}

	return is_equal;

    default:
	return FALSE;
    }
}

static BOOLEAN is_covered (is_covered_x)
int is_covered_x;
{
    /* Cette fonction regarde si la sequence de predicats comprise entre is_covered_x
       et xprod couvre tout le referentiel */

    register struct action_or_prdct_code_item *fe1, *fe2;
    
    if (is_covered_x+1 != xprod) {
        /* On ne traite que les doublets */
	return FALSE;
    }
    
    fe1 = &productions [is_covered_x];
    fe2 = &productions [xprod];

    if (!fe1->is_system || !fe2->is_system) {
        /* de predicats systeme */
	return FALSE;
    }
    
    if ((fe1->action_or_prdct_no == IsSet && fe2->action_or_prdct_no == IsReset)
	|| (fe1->action_or_prdct_no == IsReset && fe2->action_or_prdct_no == IsSet)) {
        return fe1->param == fe2->param;
    }

    if ((fe1->action_or_prdct_no == IsFirstCol && fe2->action_or_prdct_no == NotIsFirstCol)
	|| (fe1->action_or_prdct_no == NotIsFirstCol && fe2->action_or_prdct_no == IsFirstCol)) {
        return TRUE;
    }

    if ((fe1->action_or_prdct_no == IsLastCol && fe2->action_or_prdct_no == NotIsLastCol)
	|| (fe1->action_or_prdct_no == NotIsLastCol && fe2->action_or_prdct_no == IsLastCol)) {
        return TRUE;
    }
    
    return FALSE;
}

static struct fsa_trans_item	unique_fsa_trans;
static BOOLEAN			is_unique_fsa_trans_set;

static BOOLEAN non_unique (xtrans)
    int xtrans;
{
    /* Gere unique_fsa_trans */
    struct fsa_trans_item	*p = &(fsa_trans [xtrans]);

    if (is_unique_fsa_trans_set) {
	return (p->next_state_no != unique_fsa_trans.next_state_no
		|| p->is_del != unique_fsa_trans.is_del
		|| p->is_scan != unique_fsa_trans.is_scan);
    }

    is_unique_fsa_trans_set = TRUE;
    unique_fsa_trans = *p;
    return FALSE;
}


static BOOLEAN	forward_look_ahead (current_state)
    int	current_state;
{
    /* next_state est un etat en look_ahead */
    /* Retourne TRUE ssi le sous-automate issu de current_state a un traitement unique. */
    int bot, top, next_state;

    if (SXBA_bit_is_reset_set (to_be_processed, current_state)) {
	bot = fsa [current_state].transition_ref;
	top = fsa [current_state + 1].transition_ref;

	while (bot < top) {
	    if (fsa_trans [bot].cc_ref != 0) {
		next_state = fsa_trans [bot].next_state_no;

		if (next_state < 0) {
		    /* Reduce */
		    if (non_unique (bot))
			return FALSE;
		}
		else {
		    if (fsa [next_state].state_kind & LA) {
			if (!forward_look_ahead (next_state))
			    return FALSE;
		    }
		    else if (non_unique (bot))
			return FALSE;
		}
	    }

	    bot++;
	}
    }

    return TRUE;
}



VOID	lecl_code_gen ()
{
    if (sxverbosep) {
	if (!sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = TRUE;
	}

	fputs ("Code Generation\n", sxtty);
    }

    tmhb1 = xfsa2 - 1;
/*  tmhb2 = smax + xprdct_to_ate; */
    tmhb2 = smax > xprdct_to_ate ? smax : xprdct_to_ate;
    tmhb2 = (tmhb2 > -xactmax) ? tmhb2 : -xactmax;
    tmhb2 = (tmhb2 > xnondetermax) ? tmhb2 : xnondetermax;
    
    predhb1 = tmhb1 + max_re;
    is_re_erased = sxba_calloc (max_re + 1);
    tm = (transition_matrix_item**) sxalloc (tmhb1 + 1, sizeof (transition_matrix_item*));

    {
	register int	lecl_code_gen_i;

	for (lecl_code_gen_i = 1; lecl_code_gen_i <= tmhb1; lecl_code_gen_i++)
	    tm [lecl_code_gen_i] = (transition_matrix_item*) sxcalloc (tmhb2 + 1, sizeof (transition_matrix_item));
	/* init a error */
    }

    major_state_set = sxba_calloc (tmhb1 + 1);
    to_be_processed = sxba_calloc (tmhb1 + 1);
    prev = sxba_calloc (tmhb1 + 1);
    already_processed = sxba_calloc (tmhb1 + 1);
    has_predicate = sxba_calloc (tmhb1 + 1);
    has_nondeter = sxba_calloc (tmhb1 + 1);
    has_action = sxba_calloc (tmhb1 + 1);
    accessible_states_set = sxba_calloc (tmhb1 + 1);
    has_la = sxba_calloc (tmhb1 + 1);
    pred = sxbm_calloc (predhb1 + 1, tmhb1 + 1);
    equiv = sxbm_calloc (predhb1 + 1, tmhb1 + 1);
    valid_action = sxba_calloc (predhb1 + 1);
    del_action = sxba_calloc (predhb1 + 1);
    scan_action = sxba_calloc (predhb1 + 1);
    equiv_class = (struct equiv_class_item*) sxcalloc (tmhb1 + 1, sizeof (struct equiv_class_item));
    state_to_action = (int*) sxalloc (tmhb1 + 1, sizeof (int));
    old_state_to_new_state_no = (int*) sxalloc (tmhb1 + 1, sizeof (int));
    new_state_to_old_state_no = (int*) sxalloc (tmhb1 + 1, sizeof (int));
    to_be_sorted = (int*) sxalloc (smax + 1, sizeof (int));
    old_sc_to_new_sc = (int*) sxalloc (smax + 1, sizeof (int) + 1);
    is_user_prdct_in_la = sxba_calloc (xprdct_to_ate + 1);
    la_checked = sxba_calloc (tmhb1 + 1);
    scan_reached = sxba_calloc (tmhb1 + 1);
    pure_action_set = sxba_calloc (tmhb1 + 1);
    state_to_nondeter = (int*) sxcalloc (tmhb1 + 1, sizeof (int));

/* On calcule tous les etats accessibles depuis l'etat initial (la resolution
   des conflits a pu invalider certaines transitions). 
   On en profite pour calculer une borne max de prod_size */

    prod_size = post_action_no;

    {
	int	*as /* 1:xfsa2 */ ;
	int	in, out, next_state, lecl_code_gen_x, current_state, lim;

	as = (int*) sxalloc (xfsa2 + 1, sizeof (int));
	SXBA_1_bit (accessible_states_set, 1);
	as [1] = 1;
	in = 2;


/* On parcourt tous les etats */

	for (out = 1; out < in; out++) {
	    current_state = as [out];
	    current_state_kind = fsa [current_state].state_kind;
	    state_to_nondeter [current_state] = current_state;

	    if ((current_state_kind & ACT) && !(current_state_kind & MIXTE))
		SXBA_1_bit (pure_action_set, current_state);

	    lim = fsa [current_state + 1].transition_ref;
	    lecl_code_gen_x = fsa [current_state].transition_ref;

	    if (current_state_kind & (PRDCT + NONDETER))
		prod_size += lim - lecl_code_gen_x + 1; /* pour &True eventuel */
	    else if (current_state_kind & ACT)
		prod_size++;


/* toutes les transitions */

	    while (lecl_code_gen_x < lim) {
		next_state = fsa_trans [lecl_code_gen_x].next_state_no;


/* transition Shift valide */

		if (next_state > 0 && fsa_trans [lecl_code_gen_x].cc_ref != 0) {
		    next_state_kind = fsa [next_state].state_kind;


/* non encore rencontree */

		    if (next_state_kind /* etat non vide */ ) {
			if ((next_state_kind & REDUCE) 
			    && !(next_state_kind & MIXTE)
			    && is_optimize) {
			    ; /* traite avec current_state */
			}
			else {
			    if (next_state_kind & LA) {
/* On regarde si, apres resolution par le [sous-]automate de look-ahead
   le traitement est unique. Auquel cas cet automate est ignore'. */
				is_unique_fsa_trans_set = FALSE; /* vide */
				sxba_empty (to_be_processed);

				if (forward_look_ahead (next_state)) {
				    /* On ignore le sous-automate. */
				    next_state =
					fsa_trans [lecl_code_gen_x].next_state_no =
					    unique_fsa_trans.next_state_no;
				   fsa_trans [lecl_code_gen_x].is_del = unique_fsa_trans.is_del ;
				   fsa_trans [lecl_code_gen_x].is_scan = unique_fsa_trans.is_scan ;
				   next_state_kind = fsa [next_state].state_kind;
				}
			    }

			    /* fabrication de la_checked et scan_reached */
			    if (fsa_trans [lecl_code_gen_x].is_scan) 
				SXBA_1_bit (scan_reached, next_state);
			    else {
				if ((current_state_kind & ACT)
				    && (current_state_kind & MIXTE)
				    && !(current_state_kind & LA)
				    && (next_state_kind & NONDETER)) {
				    /* recopie */
				    if (SXBA_bit_is_set (la_checked, current_state))
					SXBA_1_bit (la_checked, next_state);

				    if (current_state != 1)
					state_to_nondeter [current_state] = next_state;
				}
				else if (current_state_kind & SHIFT)
				    SXBA_1_bit (la_checked, next_state);
				else if (SXBA_bit_is_set (la_checked, current_state))
				    SXBA_1_bit (la_checked, next_state);
			    }
			    
			    if (SXBA_bit_is_reset_set (accessible_states_set, next_state))
				as [in++] = next_state;
			}
		    }
		}

		lecl_code_gen_x++;
	    }
	}

	sxfree (as);
    }


/* On regarde les attributs des commentaires */

    if (are_comments_defined) {
	if (ers_disp [comments_re_no].component_no == 0) {
	    are_comments_erased = re_erased (comments_re_no);

	    /* assign1bit (is_re_erased, comments_re_no, are_comments_erased); */
	    if (are_comments_erased)
		SXBA_1_bit (is_re_erased, comments_re_no);
	    else
		SXBA_0_bit (is_re_erased, comments_re_no);
	}
	else {
	    are_comments_erased = TRUE;

	    for (j = 1; j <= ers_disp [comments_re_no].component_no; j++) {
		re_no = comments_re_no + j;

		/* assign1bit (is_re_erased, re_no, re_erased (re_no)); */
		if (re_erased (re_no))
		    SXBA_1_bit (is_re_erased, re_no);
		else
		    SXBA_0_bit (is_re_erased, re_no);

		are_comments_erased = are_comments_erased && sxba_bit_is_set (is_re_erased, re_no);
	    }
	}
    }

/* Production de code */

    productions = (struct action_or_prdct_code_item*)
	sxalloc (prod_size + 1, sizeof (struct action_or_prdct_code_item));
    xprod = 0;
    max_la = 0;
    sxba_copy (major_state_set, accessible_states_set);
    state_no = 0;
    
    while ((state_no = sxba_scan (major_state_set, state_no)) > 0) {
	if (state_to_nondeter [state_no] == state_no) {
	tm_line = tm [state_no];
	current_state_kind = fsa [state_no].state_kind;


/* Pour les etats en look_ahead on met tm_line(illegal_chars) a 1
   pour get_a_partition afin de differencier dans tous
   les cas les etats normaux des etats en look_ahead */

	if (current_state_kind & LA)
	    tm_line [1] = 1;

	for (x = fsa [state_no].transition_ref; x < fsa [state_no + 1].transition_ref; x++) {
	    next_state_no = fsa_trans [x].next_state_no;

	    if (next_state_no > 0) {
		if ((j = state_to_nondeter [next_state_no]) != next_state_no
		    && j > 0)
		    /* On change next_state_no */
		    next_state_no = fsa_trans [x].next_state_no = j;

		next_state_kind = fsa [next_state_no].state_kind;

		if ((current_state_kind & MIXTE)
		    && !(current_state_kind & LA)
		    && (next_state_kind & LA)) {


/* On part en look_ahead, on va essayer de fusionner certains de ces etats en 
   look_ahead.
   Cette fusion est decoupee par transition pour assurer la compatibilite 
   vis a vis des "erase" de la transition vers l'action valide apres 
   look_ahead. */
		    sxba_empty (to_be_processed);

		    {
			int	lecl_code_gen_i_2;

			for (lecl_code_gen_i_2 = 1; lecl_code_gen_i_2 <= predhb1; lecl_code_gen_i_2++)
			    sxba_empty (pred [lecl_code_gen_i_2]);
		    }

		    SXBA_1_bit (pred [next_state_no], state_no);


/* valid_action contiendra les actions valides apres look_ahead et del_action et scan_action
   les "erase" et "scan" associes */
		    sxba_empty (valid_action);
		    sxba_empty (del_action);
		    sxba_empty (scan_action);
		    sxba_empty (already_processed);

		    for (y = next_state_no; y > 0; y = sxba_scan (to_be_processed, 0)) {
			SXBA_1_bit (already_processed, y);

			for (z = fsa [y].transition_ref; z < fsa [y + 1].transition_ref; z++) {
			    t = fsa_trans [z].next_state_no;

			    if (t > 0)
				SXBA_1_bit (pred [t], y);
				/* existe transition de y vers t */
			    else
				SXBA_1_bit (pred [tmhb1 - t], y);

			    if (t > 0 && (fsa [t].state_kind & LA))    
				SXBA_1_bit (to_be_processed, t);
			    else {
				if (t <= 0)
				    t = tmhb1 - t;

				SXBA_1_bit (valid_action, t);

				if (fsa_trans [z].is_del)
				    SXBA_1_bit (del_action, t);
				else
				    SXBA_0_bit (del_action, t);

				if (fsa_trans [z].is_scan)
				    SXBA_1_bit (scan_action, t);
				else
				    SXBA_0_bit (scan_action, t);
			    }
			}

			sxba_minus (to_be_processed, already_processed);
		    }

		    la_reduc (/* tmhb1, */predhb1, pred, valid_action, already_processed, equiv);
		    y = 0;

		    while ((y = sxba_scan (valid_action, y)) > 0) {
			/* y est une action valide apres look_ahead */
			eq = equiv [y];

			if (!sxba_is_empty (eq)) {
			    /* il va y avoir fusion */
			    sxba_empty (prev);


/* On calcule l'ensemble prev des etats tels que si x est dans prev alors 
   trans(x) est dans eq */

			    z = 0;

			    while ((z = sxba_scan (eq, z)) > 0) {
				sxba_or (prev, pred [z]);
			    }


/* On ne s'interesse qu'aux predecesseurs n'appartenant pas a eq */

			    sxba_minus (prev, eq);


/* Les transitions concernees des etats de prev sont repositionnees vers
   l'action valide y */

			    z = 0;

			    while ((z = sxba_scan (prev, z)) > 0) {
				for (t = fsa [z].transition_ref; t < fsa [z + 1].transition_ref; t++) {
				    if ((i = fsa_trans [t].next_state_no) > 0 && sxba_bit_is_set (eq, i)) {
					/* une bonne transition */
					if (y <= tmhb1)
					    fsa_trans [t].next_state_no = y;
					else
					    fsa_trans [t].next_state_no = -(y - tmhb1);

					fsa_trans [t].is_del = sxba_bit_is_set (del_action, y);
					fsa_trans [t].is_scan = sxba_bit_is_set (scan_action, y);
				    }
				}
			    }


/* Les etats de eq sont enleves des etats a traiter */

			    sxba_minus (major_state_set, eq);
			}
		    }
		}
		else if (next_state_kind & PRDCT)
		    /* On saute les prdct &True */
		    fsa_trans [x].next_state_no = skip_true (next_state_no);
	    }
	}

	for (x = fsa [state_no].transition_ref; x < fsa [state_no + 1].transition_ref; x++) {
	    xcc = fsa_trans [x].cc_ref;

	    if (xcc < 0) {
		/* Action */
		if ((current_state_kind & ACT) && !(current_state_kind & MIXTE)) {
		    SXBA_1_bit (has_action, state_no);
		    state_to_action [state_no] = -xcc;
		    tm_line [-xcc] = gen_action (/* -xcc, */fsa_trans [x].next_state_no);
		}
	    }
	    else {
		/* Shift sur class ou predicat, reduce */
		stmt = gen_code (state_no, current_state_kind, fsa_trans [x].next_state_no,
				 fsa_trans [x].is_del, fsa_trans [x].is_scan);

		if (current_state_kind & (PRDCT + NONDETER)) {
		    /* tm_line [smax + xcc] = stmt; */
		    tm_line [xcc] = stmt;
		    
		    if (current_state_kind & PRDCT)
			SXBA_1_bit (has_predicate, state_no);
		    else
			SXBA_1_bit (has_nondeter, state_no);
		}
		else {
		    sc_set = compound_classes [xcc];
		    sc = 0;

		    while ((sc = sxba_scan (sc_set, sc)) > 0) {
			tm_line [sc] = stmt;
		    }
		}
	    }
	}
    }
	else
	    SXBA_0_bit (major_state_set, state_no);
    }

    sxfree (state_to_nondeter);

/* On essaie de ne pas retester le look_ahead d'une action lorsque ca a
   deja ete fait (pour decider de l'action a effectuer) */

    sxba_and (scan_reached, pure_action_set);
    sxba_and (pure_action_set, la_checked);

#if 0
    sxba_minus (la_checked, scan_reached);
#endif

/* On ne conserve que les actions dont le look-ahead a toujours ete teste */

#if 0
    if (!sxba_is_empty (la_checked)) {
#endif

    if (!sxba_is_empty (pure_action_set)) {
	/* il y a de telle(s) action(s) */
	state_no = 0;

#if 0
	while ((state_no = sxba_scan (la_checked, state_no)) > 0) {
#endif

	while ((state_no = sxba_scan (pure_action_set, state_no)) > 0) {
	    is_unique (&(tm [state_no] [state_to_action [state_no]]));
	}


/* On supprime les etats inaccessibles; ca ne peut etre fait pendant
   is_unique car les etats sautes peuvent(?) etre references par ailleurs */

	{
	    register int	*asa, xasa1, xasa2, lecl_code_gen_x_2, next;
	    transition_matrix_item	lecl_code_gen_stmt;

	    asa = (int*) sxalloc (tmhb1 + 1, sizeof (int));
	    asa [1] = 1;
	    xasa1 = 0;
	    xasa2 = 1;
	    sxba_empty (major_state_set);
	    SXBA_1_bit (major_state_set, 1);

	    while (++xasa1 <= xasa2) {
		state_no = asa [xasa1];
		tm_line = tm [state_no];

		for (lecl_code_gen_x_2 = 1; lecl_code_gen_x_2 <= tmhb2; lecl_code_gen_x_2++) {
		    lecl_code_gen_stmt = tm_line [lecl_code_gen_x_2];

		    switch (CODOP (lecl_code_gen_stmt)) {
		    case State:
		    case FirstLookAhead:
		    case NextLookAhead:
		    case ActPrdct:
			next = NEXT (lecl_code_gen_stmt);

			if (!sxba_bit_is_set (major_state_set, next)) {
			    SXBA_1_bit (major_state_set, next);
			    asa [++xasa2] = next;
			}

			break;

		    default:
			break;
		    }
		}
	    }

	    sxfree (asa);
	}

	sxfree (scan_reached);
	sxfree (la_checked);
	sxfree (pure_action_set);
    }

    sxba_and (has_la, major_state_set);

/* Calcul des classes d'equivalences */

    get_a_partition (is_compatible, get_next_state, major_state_set, tmhb1, tmhb2, equiv_class, &partition_no);
    max_ref = (tmhb1 > termax) ? tmhb1 : termax;
    max_ref = (prod_size > max_ref) ? prod_size : max_ref;

    for (i = max_ref, ref_lgth = 0; i; i >>= 1, ref_lgth++)
	;

    /* codop_lgth = 6; */
    codop_lgth = 4; /* Le 21/12/04 */
    keep_lgth = 1; /* Le 21/12/04 */
    scan_lgth = 1; /* Le 27/12/04 */
    stmt_lgth = codop_lgth + keep_lgth + scan_lgth + ref_lgth;
    tm_tilde_lgth = partition_no * stmt_lgth;
    tm_tilde = sxbm_calloc (smax + 1, tm_tilde_lgth + 1);


/* Generation des actions et des predicats.
   Creation de la matrice de transition transposee
   et de la correspondance entre les etats actions et predicats et leur code 
   (xprod) */
    new_state_no = 0;
    old_state_no = 0;
    has_nondeterminism = FALSE;

    while ((old_state_no = sxba_scan (major_state_set, old_state_no)) > 0) {
	if (equiv_class [old_state_no].repr != old_state_no)
	    SXBA_0_bit (major_state_set, old_state_no);
	else {
	    tm_line = tm [old_state_no];

	    if (sxba_bit_is_set (has_action, old_state_no)) {
		int	ref, codop;

		action_no = state_to_action [old_state_no];
		stmt = tm_line [action_no];
		ref = NEXT (stmt);
		codop = CODOP (stmt);
		old_state_to_new_state_no [old_state_no] =
		    gen_action_code (-action_no, KEEP (stmt) != 0, SCAN (stmt) != 0,
				     is_a_shift_code [codop] ? equiv_class [ref].repr : ref,
				     codop);
		SXBA_0_bit (major_state_set, old_state_no);
	    }
	    else if (sxba_bit_is_set (has_predicate, old_state_no)) {
		x = xprod;

		for (prdct = xprdct_to_ate; prdct >= 1; prdct--) {
		    /* stmt = tm_line [smax + prdct]; */
		    stmt = tm_line [prdct];

		    if (prdct == prdct_true_code && is_covered (x + 1)) {
			/* Derniere instruction d'une sequence de predicats et la liste des
			   predicats precedents realise une partition, le test du dernier predicat
			   est donc inutile, on l'ecrase par &True. */
			productions [xprod].action_or_prdct_no = IsTrue;
		    }
		    else if (stmt != 0) {
			int	codop, ref;

			codop = CODOP (stmt);
			ref = NEXT (stmt);
			gen_prdct_code (prdct,
					KEEP (stmt) != 0,
					SCAN (stmt) != 0,
					is_a_shift_code [codop] ? equiv_class [ref].repr : ref,
					codop);

			if (tm_line [1] == 1 && !productions [xprod].is_system) {
			    SXBA_1_bit (is_user_prdct_in_la, prdct);
			}
		    }
		    else if (prdct == prdct_true_code) {
			gen_prdct_code (0, 0, 0, 0, 0);
		    }
		}

		if ((y = retrieve_prdct_or_nondeter_sequence (x + 1, IsPredicate)))
		    /* la sequence existait deja */
		    xprod = x;
		else
		    y = x + 1;

		old_state_to_new_state_no [old_state_no] = y;
		SXBA_0_bit (major_state_set, old_state_no);
	    }
	    else if (sxba_bit_is_set (has_nondeter, old_state_no)) {
		x = xprod;

		for (j = xnondetermax; j >= 1; j--) {
		    if ((stmt = tm_line [j]) != 0) {
			int	codop, ref;

			codop = CODOP (stmt);
			ref = NEXT (stmt);
			gen_nondeter_code (j - 1,
					KEEP (stmt) != 0,
					SCAN (stmt) != 0,
					is_a_shift_code [codop] ? equiv_class [ref].repr : ref,
					codop);
		    }
		}

		if ((y = retrieve_prdct_or_nondeter_sequence (x + 1, IsNonDeter)))
		    /* la sequence existait deja */
		    xprod = x;
		else {
		    has_nondeterminism = TRUE;
		    y = x + 1;
		}

		old_state_to_new_state_no [old_state_no] = y;
		SXBA_0_bit (major_state_set, old_state_no);
	    }
	    else {
		new_state_no++;

		for (j = 1; j <= smax; j++) {
		    stmt = tm_line [j];

		    if (is_a_shift_code [CODOP (stmt)]) {
			stmt = tm_line [j] = KSC (stmt) | equiv_class [NEXT (stmt)].repr;
		    }

		    put_in_tm_tilde (j, new_state_no, stmt);
		}
	    }
	}
    }


/* Calcul de max_la */

    if (!sxba_is_empty (has_la)) {
	int	lecl_code_gen_x_3, lecl_code_gen_sc, la;

	is_processed = sxba_calloc (tmhb1 + 1);
	is_cycle = sxba_calloc (tmhb1 + 1);
	la_value = (int*) sxalloc (tmhb1 + 1, sizeof (int));
	lecl_code_gen_x_3 = 0;

	while ((lecl_code_gen_x_3 = sxba_scan (has_la, lecl_code_gen_x_3)) > 0 && max_la != -1) {
	    if (!sxba_bit_is_set (is_processed, lecl_code_gen_x_3)) {
		tm_line = tm [lecl_code_gen_x_3];

		for (lecl_code_gen_sc = 1; lecl_code_gen_sc <= tmhb2 && max_la != -1; lecl_code_gen_sc++) {
		    if (CODOP (tm_line [lecl_code_gen_sc]) == FirstLookAhead) {
			sxba_empty (is_cycle);
			la = get_la_length (NEXT (tm_line [lecl_code_gen_sc])) + 1;

			if (la == 0)
			    max_la = -1;
			else
			    max_la = (la > max_la) ? la : max_la;
		    }
		}
	    }
	}

	sxfree (la_value);
	sxfree (is_cycle);
	sxfree (is_processed);
    }

    if (has_nondeterminism) {
	sxtmsg (sxsrcmngr.source_coord.file_name,
	    "%sThe generated finite state automaton is nondeterministic.", err_titles [1]);
    }

    if (!sxba_is_empty (is_user_prdct_in_la)) {

/* On previent l'utilisateur que certains de ses predicats vont s'executer
   en look-ahead, le code doit donc en tenir compte... */

        VARSTR vstr = varstr_alloc (128);
	BOOLEAN is_first = TRUE, found;
	char *s;
	
	prdct = 0;

	while ((prdct = sxba_scan (is_user_prdct_in_la, prdct)) > 0) {
	    s = sxstrget (action_or_prdct_to_ate [prdct]);
	    found = FALSE;

	    if (*s == '^') {
		s++;
		x = 0;

		while ((x = sxba_scan (is_user_prdct_in_la, x)) < prdct)
		    if (strcmp (sxstrget (action_or_prdct_to_ate [x]), s) == 0) {
		        found = TRUE;
			break;
		}
		
	    }
	    else {
		x = 0;

		while ((x = sxba_scan (is_user_prdct_in_la, x)) < prdct)
		    if (strcmp (sxstrget (action_or_prdct_to_ate [x]) + 1, s) == 0) {
		        found = TRUE;
			break;
		}
		
	    }

	    if (found)
		continue;

	    if (is_first)
	        is_first = FALSE;
	    else
	        varstr_catenate (vstr, ", ");

	    varstr_catenate (vstr, s);
	}

	sxtmsg (sxsrcmngr.source_coord.file_name,
	    "%sThe following user's predicates will be executed in look-ahead: %s.", err_titles [1], varstr_tostr (vstr));
	varstr_free (vstr);
    }
    

/* Construction des structures pour search et verify et des tables de 
   correspondance entre les etats anciens et nouveaux */

    last_state_no = 0;
    old_state_no = 0;

    while ((old_state_no = sxba_scan (major_state_set, old_state_no)) > 0) { 
	last_state_no++;
	old_state_to_new_state_no [old_state_no] = last_state_no;
	new_state_to_old_state_no [last_state_no] = old_state_no;
    }


/* On compacte les classes simples */

    new_sc_to_char_set = sxbm_calloc (smax + 1, char_max + 1);

    for (i = 1; i <= smax; i++) {
	to_be_sorted [i] = i;
    }

    equality_sort (to_be_sorted, 3
		   /* On force illegal chars et eof a etre differents */
				  , smax, less_equal, equal);
    Smax = 0;

    for (i = 1; i <= smax; i++) {
	j = to_be_sorted [i];

	if (i == j) {
	    Smax++;
	    old_sc_to_new_sc [i] = Smax;
	    sxba_copy (new_sc_to_char_set [Smax], sc_to_char_set [i]);
	}
	else {
	    k = old_sc_to_new_sc [j];
	    old_sc_to_new_sc [i] = k;
	    sxba_or (new_sc_to_char_set [k], sc_to_char_set [i]);
	}
    }

    for (i = 0; i < target_collate_length; i++) {
	classe [i] = old_sc_to_new_sc [classe [i]];
    }


/* Mise a jour de la matrice de transition et du floyd_evans */
/* Transformation de l'unite lexicale bidon "include" en action systeme */

    transition_matrix = (transition_matrix_item**) sxalloc (last_state_no + 1, sizeof (transition_matrix_item*));

    {
	int	lecl_code_gen_i_3;

	for (lecl_code_gen_i_3 = 1; lecl_code_gen_i_3 <= last_state_no; lecl_code_gen_i_3++)
	    transition_matrix [lecl_code_gen_i_3] = (transition_matrix_item*) sxalloc (Smax + 1, sizeof (transition_matrix_item));
    }

    for (new_state_no = 1; new_state_no <= last_state_no; new_state_no++) {
	old_state_no = new_state_to_old_state_no [new_state_no];
	tm_line = tm [old_state_no];
	transition_matrix_line = transition_matrix [new_state_no];
	new_simple_class = 0;

	for (old_simple_class = 1; old_simple_class <= smax; old_simple_class++) {
	    if (to_be_sorted [old_simple_class] == old_simple_class) {
		int	codop;

		new_simple_class++;
		stmt = tm_line [old_simple_class];
		codop = CODOP (stmt);

		if (codop == Dummy) {
		    stmt = STMT (KEEP (stmt) != 0, SCAN (stmt) != 0, ActPrdct, NEXT (stmt));
		}
		else if (is_a_shift_code [codop]) {
		    stmt = KSC (stmt) | old_state_to_new_state_no [NEXT (stmt)];

		    if (codop == State && NEXT (stmt) == (unsigned)new_state_no)
			stmt = STMT (KEEP (stmt) != 0, 1, SameState, new_state_no);
		}

		transition_matrix_line [new_simple_class] = stmt;
	    }
	}
    }

    action_or_prdct_code = (struct action_or_prdct_code_item*) sxalloc ((int) (xprod + 1), sizeof (struct
	 action_or_prdct_code_item));

    for (x = 1; x <= xprod; x++) {
	int	codop;

	floyd_evans = productions [x];
	stmt = floyd_evans.stmt;
	codop = CODOP (stmt);

	if (codop == Dummy) {
	    floyd_evans.stmt = STMT (KEEP (stmt) != 0, SCAN (stmt) != 0, ActPrdct, NEXT (stmt));
	}
	else if (is_a_shift_code [codop])
	    floyd_evans.stmt = KSC (floyd_evans.stmt) | old_state_to_new_state_no [NEXT (floyd_evans.stmt)];

	action_or_prdct_code [x] = floyd_evans;
    }


/* On verifie que tous les tokens d'interet sont reconnus */

    sxba_not (is_re_generated), SXBA_0_bit (is_re_generated, 0);

    if ((x = sxba_cardinal (is_re_generated)) > 0) {
	VARSTR	string;

	string = token_name_list (is_re_generated, varstr_alloc (256));
	sxtmsg (sxsrcmngr.source_coord.file_name,
		"%sThe following regular expression%s not been generated :\n%s.",
		err_titles [1]	/* warning */ ,
		(x == 1) ? " has" : "s have",
		varstr_tostr (string));
	varstr_free (string);
    }

    sxbm_free (tm_tilde);
    sxfree (productions);
    sxfree (is_user_prdct_in_la);
    sxfree (old_sc_to_new_sc);
    sxfree (to_be_sorted);
    sxfree (new_state_to_old_state_no);
    sxfree (old_state_to_new_state_no);
    sxfree (state_to_action);
    sxfree (equiv_class);
    sxfree (scan_action);
    sxfree (del_action);
    sxfree (valid_action);
    sxbm_free (equiv);
    sxbm_free (pred);
    sxfree (has_la);
    sxfree (accessible_states_set);
    sxfree (has_action);
    sxfree (has_predicate);
    sxfree (has_nondeter);
    sxfree (already_processed);
    sxfree (prev);
    sxfree (to_be_processed);
    sxfree (major_state_set);

    for (i = tmhb1; i > 0; i--)
	sxfree (tm [i]);

    sxfree (tm);
    sxfree (is_re_erased);
}
