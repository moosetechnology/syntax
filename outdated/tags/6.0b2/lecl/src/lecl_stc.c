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




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 20030516 13:07 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from lecl_stc.pl1       */
/*  on Monday the seventeenth of March, 1986, at 17:21:20,   */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3g PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/



#define NODE struct lecl_node

#define get_brother(x) (x->brother)

#define get_father(x) (x->father)

#include "sxunix.h"
#include "sxba.h"
#include "varstr.h"

#include "lecl_node.h"

#include "lecl_nn.h"
#include "lecl_ag.h"
#include "lecl_pcn.h"
char WHAT_LECLSTC[] = "@(#)SYNTAX - $Id: lecl_stc.c 607 2007-06-04 09:35:31Z syntax $" WHAT_DEBUG;


static int	retrieve ();
static int	st_octal ();
static int	node_no, item_no, i, xers, xs, xsl, x_cc_stack, synonym_lgth;
static SXBA	/* char_max */ *cc_stack /* 1 : max_bucket_no */ ;
static SXBA	/* char_max */ char_set;
static BOOLEAN	is_class_definition, is_a_predicate_name, is_comments_defined, is_include_defined;




BOOLEAN		lecl_check_octal_code (ate, designator, val)
    int		ate;
    int		*val;
    struct sxsource_coord	designator;
{
    if (ate == 0)
	return (FALSE);

    *val = st_octal (sxstrget (ate));

    if (*val <= 255)
	return (TRUE);

    sxput_error (designator, "%sThis internal value cannot exceed 255.", err_titles [2] /* error */ );
    return (FALSE);
}



static BOOLEAN	is_a_son (node_ptr, node_name)
    struct lecl_node	*node_ptr;
    int		node_name;
{
    /* retourne TRUE ssi (au moins) un des fils de "node_ptr" est un noeud de
       nom "node_name". */
    register struct lecl_node	*son;

    for (son = node_ptr->son; son != NULL; son = son->brother) {
	if (son->name == node_name)
	    return TRUE;
    }

    return FALSE;
}



static BOOLEAN	check_environments (node_ptr, name)
    struct lecl_node	*node_ptr;
    int		name;
{
    struct lecl_node	*son;

    for (son = node_ptr->father->son; son != node_ptr; son = son->brother) {
	switch (son->name) {
	case KEYWORDS_SPEC_n:
	case POST_ACTION_n:
	case PRIORITY_KIND_S_n:
	    if (son->name == name)
		return FALSE;

	    break;

	case CONTEXT_n:
	case RESTRICTED_CONTEXT_n:
	case UNBOUNDED_CONTEXT_n:
	case UNBOUNDED_RESTRICTED_CONTEXT_n:
	    if (name == CONTEXT_n)
		return FALSE;

	    break;

	default:
	    break;
	}
    }

    return TRUE;
}



static BOOLEAN	check_component (node_ptr, val)
    struct lecl_node	*node_ptr;
    int		*val;
{
    int		alpha, class_no;

    alpha = node_ptr->token.string_table_entry;

    if (alpha == 0)
	return (FALSE);

    if (node_ptr->name == OCTAL_CODE_n)
	return (lecl_check_octal_code (node_ptr->token.string_table_entry, node_ptr->token.source_index, val));

    if (node_ptr->name == ID_n) {
	class_no = retrieve (alpha, node_ptr->token.source_index);

	if (class_no < 0 /* abbrev */ ) {
	    sxput_error (node_ptr->token.source_index, "%sIllegal occurrence of an abbreviation name.", err_titles [2]
			 /* error */
														      );
	    return (FALSE);
	}

	if (sxba_cardinal (class_to_char_set [class_no]) != 1) {
	    sxput_error (node_ptr->token.source_index, "%sThis class name must reference a single character set.",
		 err_titles [2] /* error */ );
	    return (FALSE);
	}

	*val = sxba_scan (class_to_char_set [class_no], 0) - 1;
	return (TRUE);
    }
    else {
	char	*s;


/* character_string */

	if (sxstrlen (alpha) == 3) {
	    s = sxstrget (alpha);
	    *val = ((unsigned char)*++s);
	    return (TRUE);
	}

	sxput_error (node_ptr->token.source_index, "%sOnly a single character set is allowed as component of a slice.",
	     err_titles [2] /* error */ );
	return (FALSE);
    }
}



static BOOLEAN	st_check_slice (ptr1, ptr2, bi, bs)
    struct lecl_node	*ptr1, *ptr2;
    int		*bi, *bs;
{
    if (!check_component (ptr1, bi) || !check_component (ptr2, bs))
	return (FALSE);

    if (*bs >= *bi)
	return (TRUE);

    sxput_error (ptr1->token.source_index, "%sEmpty slices are not allowed.", err_titles [2] /* error */ );
    return (FALSE);
}



static int	look_in_table (look_in_table_char_set, alpha_table_entry, is_not)
    SXBA	look_in_table_char_set;
    unsigned int		*alpha_table_entry;
    BOOLEAN	is_not;
{
    if (is_not) {
	char	s [64];

	sxba_and (sxba_not (look_in_table_char_set), class_to_char_set [any]);
	/* Moins */
	*alpha_table_entry = sxstrsave (strcat (strcpy (s, "^"), sxstrget (*alpha_table_entry)));
    }

    for (i = 1; i <= cmax; i++) {
	if (sxba_first_difference (class_to_char_set [i], look_in_table_char_set) == -1)
	    return (i);
    }

    cmax++;

#ifdef DEBUG

    printf ("CED (look_in_table): class_size=%d, cmax=%d, name=%s\n", class_size, cmax, sxstrget (*alpha_table_entry));

#endif

    abbrev_or_class_to_ate [cmax] = *alpha_table_entry;
    sxba_copy (class_to_char_set [cmax], look_in_table_char_set);
    return (cmax);
}



static int	define_class_by_name (alpha_table_entry, /* designator, */val/* , is_not */)
    int		alpha_table_entry;
    SXBA	val;
    /* BOOLEAN	is_not;
       struct sxsource_coord	designator; */
{
    int		define_class_by_name_i;

    if (alpha_table_entry == 0)
	return (0);

    for (define_class_by_name_i = 1; define_class_by_name_i <= cmax; define_class_by_name_i++) {
	if (abbrev_or_class_to_ate [define_class_by_name_i] == alpha_table_entry) {
	    sxba_copy (class_to_char_set [define_class_by_name_i], val);
	    return (define_class_by_name_i);
	}
    }

    cmax++;

#ifdef DEBUG

    printf ("CED (define_class_by_name): class_size=%d, cmax=%d, name=%s\n", class_size, cmax, sxstrget (
	 alpha_table_entry));

#endif

    abbrev_or_class_to_ate [cmax] = alpha_table_entry;
    sxba_copy (class_to_char_set [cmax], val);
    return (cmax);
    /* 
end define_class_by_name */
}



static int	define_class_by_string (alpha_table_entry, designator, is_not)
    unsigned int		*alpha_table_entry;
    BOOLEAN	is_not;
    struct sxsource_coord	designator;
{
    register int 		car, delta;
    register char		*string, *new_string, *old_string;
    char			s [259];
    struct sxsource_coord	pos;
    BOOLEAN			is_warning;

    if (*alpha_table_entry == 0)
	return (0);

    old_string = string = sxstrget (*alpha_table_entry);
    sxba_empty (char_set);
    is_warning = FALSE;
    delta = 0;

    while (*(++string + 1) != NUL) {
	++delta;
	/* Attention, *string peut etre negatif si son code interne depasse 0127 */
	car = ((unsigned char)*string) + 1;

	if (!SXBA_bit_is_reset_set (char_set, car)) {
	    is_warning = TRUE;
	    pos = designator;
	    pos.column += delta;
	    sxput_error (pos, "%sDuplicate character.", err_titles [1] /* warning */ );
	}
    }

    if (is_warning) {
	/* On refabrique une bonne "alpha_table_entry". */
	new_string = s;
	string = old_string;
	*new_string++ = '"';
	sxba_empty (char_set);

	while (*(++string + 1) != NUL) {
	    car = ((unsigned char)*string) + 1;

	    if (SXBA_bit_is_reset_set (char_set, car)) {
		*new_string++ = *string;
	    }
	}

	*new_string++ = '"';
	*new_string = NUL;
	*alpha_table_entry = sxstrsave (s);
    }

    return look_in_table (char_set, alpha_table_entry, is_not);
}



static int	define_class_by_octal_code (alpha_table_entry, designator, is_not)
    unsigned int		*alpha_table_entry;
    BOOLEAN	is_not;
    struct sxsource_coord	designator;
{
    int		codofcar;

    if (!lecl_check_octal_code (*alpha_table_entry, designator, &codofcar))
	return (0);

    sxba_empty (char_set);
    codofcar++;
    SXBA_1_bit (char_set, codofcar);
    return look_in_table (char_set, alpha_table_entry, is_not);
}



static int	define_class_by_slice (ptr1, ptr2, alpha_table_entry, is_not)
    struct lecl_node	*ptr1, *ptr2;
    unsigned int		*alpha_table_entry;
    BOOLEAN	is_not;
{
    register int	define_class_by_slice_i;
    int		bi, bs;

    if (!st_check_slice (ptr1, ptr2, &bi, &bs))
	return (0);

    sxba_empty (char_set);

    for (define_class_by_slice_i = bi + 1; define_class_by_slice_i <= bs + 1; define_class_by_slice_i++)
	SXBA_1_bit (char_set, define_class_by_slice_i);

    return look_in_table (char_set, alpha_table_entry, is_not);
}



static int	ref_class_by_name (alpha_table_entry, class, is_not)
    unsigned int		*alpha_table_entry;
    int		class;
    BOOLEAN	is_not;
{
    /* is_not always true */
    sxba_copy (char_set, class_to_char_set [class]);
    return look_in_table (char_set, alpha_table_entry, is_not);
}



static int	commun (alpha_table_entry, t_code, ref, tok_no, designator)
    int		alpha_table_entry, t_code, *ref, tok_no;
    struct sxsource_coord	designator;
{
    int		commun_i;
    struct ers_disp_item	*ers_disp_ptr;

    current_token_no++;

    if (alpha_table_entry == 0)
	*ref = 0;
    else {
	commun_i = ate_to_token_no [alpha_table_entry];

	if (commun_i == 0)
	    ate_to_token_no [alpha_table_entry] = current_token_no;
	else if (commun_i > 0)
	    ate_to_token_no [alpha_table_entry] = -commun_i;
	/* ambiguite si reference directement */
    }

    if (t_code > 0)
	SXBA_1_bit (RE_to_T [current_token_no], t_code);


/* si le nom n'est pas un terminal du niveau syntaxique 
   reduc(current_token_no)=-alpha_table_entry */

    ers_disp_ptr = ers_disp + current_token_no;
    ers_disp_ptr->reduc = *ref;
    ers_disp_ptr->component_no = 0;
    ers_disp_ptr->priority_kind = 0;
    ers_disp_ptr->master_token = tok_no;
    ers_disp_ptr->lex_name_source_index = designator;
    ers_disp_ptr->is_a_user_defined_context = FALSE;
    ers_disp_ptr->post_action = 0;
    return (current_token_no);
}



static int	define_lex_name (alpha_table_entry, designator)
    int		alpha_table_entry;
    struct sxsource_coord	designator;
{
    int		ref, t_code, tok_no;

    sxinitialise (t_code);
    tok_no = current_token_no + 1;

    if (alpha_table_entry > 0) {
	t_code = ate_to_t [alpha_table_entry];

	if (t_code == -2) {
	    /* dummy token name */
	    char	*s;

	    s = sxstrget (alpha_table_entry);

	    if (*s == '%' || *s == '\"')
		sxput_error (designator, "%sThis unknown terminal denotation is supposed to be a dummy token name.",
		     err_titles [1] /* warning */ );

	    ref = -alpha_table_entry;
	}
	else if (t_code == -1) {
	    /* ambiguite entre tokens */
	    sxput_error (designator, "%sAmbiguous definition: use a string literal denotation.", err_titles [2]
			 /* error */
													       );
	    ref = 0;
	}
	else {
	    ref = alpha_table_entry;

	    if (t_code > 0) {
		if (!SXBA_bit_is_reset_set (t_is_defined_by_a_token, t_code))
		    sxput_error (designator, "%sIllegal redefinition of a lexical token.", err_titles [2] /* error */ );
	    }
	}
    }

    return (commun (alpha_table_entry, t_code, &ref, tok_no, designator));
}



static int	define_kw (alpha_table_entry, designator)
    int		alpha_table_entry;
    struct sxsource_coord	designator;
{
    register int		t_code;

    if (alpha_table_entry == 0)
	return 0;

    if ((t_code = ate_to_t [alpha_table_entry]) <= 0) {
	if (t_code == -1) {
	    /* ambiguite entre tokens */
	    sxput_error (designator, "%sThis ambiguous syntactic level terminal reference is ignored:\n\
\tuse a string literal denotation.", err_titles [1] /* error */);
	}
	else {
	    /* Not a syntactic level terminal (Or Comments). */
	    sxput_error (designator, "%sThis unknown syntactic level terminal denotation is ignored.",
			 err_titles [1] /* warning */ );
	}
	
	return 0;
    }

    /* On verifie que le terminal est non generique. */
    if (SXBA_bit_is_set (is_a_generic_terminal, t_code)) {
	    sxput_error (designator, "%sA generic terminal cannot be a keyword, it is ignored.",
			 err_titles [1] /* warning */ );
	    return 0;
    }

    return t_code;
}



static int	define_component_name (token_no, alpha_table_entry, designator)
    int		token_no;
    int		alpha_table_entry;
    struct sxsource_coord	designator;
{
    int		define_component_name_i, ref, t_code, tok_no;

    tok_no = token_no;
    ers_disp [token_no].component_no++;
    t_code = 0;
    define_component_name_i = ers_disp [token_no].reduc;

    if (define_component_name_i > 0)
	t_code = ate_to_t [define_component_name_i];

    if (ers_disp [token_no].reduc < 0)
	ref = -alpha_table_entry;
    else
	ref = alpha_table_entry;

    return (commun (alpha_table_entry, t_code, &ref, tok_no, designator));
}



static VOID	finalize_re (re_no, finalize_re_item_no, is_abbrev, node_ptr)
    int		re_no, finalize_re_item_no;
    BOOLEAN	is_abbrev;
    struct lecl_node	*node_ptr;
{
    int		x;

    x = re_no;

    if (is_abbrev) {
	abbrev [re_no].subtree_ptr = node_ptr;
	x--;
	xers = abbrev [re_no].pront + finalize_re_item_no + 1;
	abbrev [x].pront = xers;
	abbrev [x].is_used = FALSE;
    }
    else {
	ers_disp [re_no].subtree_ptr = node_ptr;
	x++;
	xers = ers_disp [re_no].prolon + finalize_re_item_no + 1;
	ers_disp [x].prolon = xers;
    }
}



static int	set_action_or_prdct (is_action, alpha_table_entry)
    int		alpha_table_entry;
    BOOLEAN	is_action;
{
    int		set_action_or_prdct_i, x;

    if (alpha_table_entry == 0)
	return (0);

    if (is_action) {
	for (set_action_or_prdct_i = xactmax; set_action_or_prdct_i <= xactmin - 1; set_action_or_prdct_i++) {
	    if (action_or_prdct_to_ate [set_action_or_prdct_i] == alpha_table_entry)
		return (set_action_or_prdct_i);
	}

	x = --xactmax;
    }
    else {
	for (set_action_or_prdct_i = 1; set_action_or_prdct_i <= xprdct_to_ate; set_action_or_prdct_i++) {
	    if (action_or_prdct_to_ate [set_action_or_prdct_i] == alpha_table_entry)
		return (set_action_or_prdct_i);
	}

	x = ++xprdct_to_ate;
    }


#ifdef DEBUG

    printf ("CED (set_action_or_prdct): action_or_prdct_to_ate [-%d <= %d <= %d] =%s\n", action_size, x, predicate_size,
	 sxstrget (alpha_table_entry));

#endif

    action_or_prdct_to_ate [x] = alpha_table_entry;
    return (x);
}



static int	retrieve (alpha_table_entry, designator)
    int		alpha_table_entry;
    struct sxsource_coord	designator;
{
    int		retrieve_i;


/* Dans les E.R. parties droites des abreviations current_abbrev_no reference 
   l'abreviation en cours de definition.Une utilisation recursive provoque 
   donc la sortie du message "Not declared.".
   Lorsque la liste des abreviations est terminee,current_abbrev_no reference 
   sous la derniere abreviation entree,tous les noms sont donc examines par 
   retrieve. */

    if (alpha_table_entry == 0)
	return (0);


/* recherche de l'abrev la plus recente */

    for (retrieve_i = current_abbrev_no; retrieve_i <= -1; retrieve_i++) {
	if (abbrev_or_class_to_ate [retrieve_i] == alpha_table_entry) {
	    abbrev [retrieve_i].is_used = TRUE;
	    return (retrieve_i);
	}
    }

    for (retrieve_i = 0; retrieve_i <= cmax; retrieve_i++) {
	if (abbrev_or_class_to_ate [retrieve_i] == alpha_table_entry)
	    return (retrieve_i);
    }

    for (retrieve_i = 1; retrieve_i <= xnd; retrieve_i++) {
	if (not_declared [retrieve_i] == alpha_table_entry)
	    return (0);
    }

    not_declared [++xnd] = alpha_table_entry;
    sxput_error (designator, "%sNot declared.", err_titles [2] /* error */ );
    return (0);
}



static int	wa_set_d_item_no (wa_set_d_item_no_item_no, item_code)
    int		wa_set_d_item_no_item_no, item_code;
{
    if (item_code >= 0)
	return (wa_set_d_item_no_item_no);
    else
	return (wa_set_d_item_no_item_no + abbrev [item_code - 1].pront - abbrev [item_code].pront - 2);
}



static int	st_define_abbrev (alpha_table_entry)
    int		alpha_table_entry;
{
    /* il est possible de redefinir une abreviation qcq */
    abbrev_or_class_to_ate [--current_abbrev_no] = alpha_table_entry;
    abbrev [current_abbrev_no].prdct_no = 0;
    return (current_abbrev_no);
}



static int	st_octal (string)
    char	*string;
{
    int		value;

    value = 0;

    for (string++; *string != NUL; string++) {
	value = value * 8 + *string - '0';
    }

    return (value);
}



static char	set_priority_kind (old, current, designator)
    char	old, current;
    struct sxsource_coord	designator;
{
    if (old & current) {
	sxput_error (designator, "%sThis priority kind is already specified.", err_titles [1] /* warning */ );
	return (old);
    }

    if (current == Reduce_Reduce)
	return (Reduce_Reduce | old);

    if (old & (Shift_Reduce + Reduce_Shift)) {
	sxput_error (designator, "%sThis priority kind is incompatible with a previous one, it is ignored.", err_titles [
	     1] /* warning */ );
	return (old);
    }

    return (old | current);
}



static VOID	st_put_in_exclude (name, component, designator)
    int		name, component;
    struct sxsource_coord	designator;
{
    register struct exclude_item		*exclude_ptr;

    exclude_ptr = exclude + xexclude++;
    exclude_ptr->name = name;
    exclude_ptr->component = component;
    exclude_ptr->source_index = designator;
}



static VOID	st_put_in_kw (t_code)
    int		t_code;
{
    keyword [xkw++] = t_code;
}



static VOID	st_finalize_exclude ()
{
    ers_disp [current_token_no + 1].pexcl = xexclude;
}


static VOID	st_finalize_kw ()
{
    ers_disp [current_token_no + 1].pkw = xkw;
}



static VOID	st_finalize_context (is_unbounded, kind)
    BOOLEAN	is_unbounded, kind;
{
    struct ers_disp_item	*ers_disp_ptr;

    ers_disp_ptr = ers_disp + current_token_no;
    ers_disp_ptr->restricted_context = kind;
    ers_disp_ptr->is_a_user_defined_context = TRUE;
    ers_disp_ptr->is_unbounded_context = is_unbounded;
}



static int	predicate_processing (ate, designator, is_in_re, is_an_abbrev)
    int		ate;
    BOOLEAN	is_in_re, is_an_abbrev;
    struct sxsource_coord	designator;
{
    int		abbrev_no, prdct_no;

    if (is_an_abbrev) {
	abbrev_no = retrieve (ate, designator);

	if (!is_in_re || abbrev_no == 0)
	    return (abbrev_no);

	prdct_no = abbrev [abbrev_no].prdct_no;

	if (prdct_no == 0) {
	    /* premiere utilisation de ce predicate_name ds une ER, on alloue un nouveau
   code interne */
	    abbrev [abbrev_no].prdct_no = ++xprdct_to_ate;

#ifdef DEBUG

	    printf ("CED (predicate_processing): action_or_prdct_to_ate [-%d <= %d <= %d] =%s\n", action_size,
		 xprdct_to_ate, predicate_size, sxstrget (ate));

#endif

	    action_or_prdct_to_ate [xprdct_to_ate] = ate;
	    prdct_to_ers [xprdct_to_ate] = abbrev [abbrev_no].pront;
	    return (xprdct_to_ate);
	}
	else
	    return (prdct_no);
    }
    else {
	/* occurrence d'un prdct systeme ou utilisateur */
	prdct_no = set_action_or_prdct (FALSE, ate);
	prdct_to_ers [prdct_no] = 0;
	/* sera rempli dans lecl.symbol_table */
	return (prdct_no);
    }
}




static VOID	st_ced (visited, is_del)
    register struct lecl_node	*visited;
    BOOLEAN	is_del;
{
    /* collect explicit declarations of abbreviations and tokens */
    struct lecl_node	*brother;
    int		val, st_ced_i, st_ced_j;


#ifdef DEBUG
    printf ("CED visited_node = %d\n", visited->name);
#endif

    switch (visited->name) {
    case ABBREVIATION_n:
	st_ced (sxson (visited, 2), FALSE);
	st_ced (sxson (visited, 1), FALSE);
	finalize_re (visited->item_code, item_no, TRUE, sxson (visited, 2));
	return;

    case ABBREVIATION_PRDCT_NAME_n:
    case ABBREVIATION_RE_NAME_n:
	get_father (visited)->item_code = st_define_abbrev (visited->token.string_table_entry);
	return;

    case ABBREVIATION_S_n:
    case CLASS_S_n:
    case ENVIRONMENT_S_n:
    case KEYWORDS_n:
    case NOT_KEYWORDS_n:
    case TOKEN_REF_S_n:
    case TOKEN_S_n:
list:	for (brother = sxson (visited, 1); brother != NULL; brother = get_brother (brother)) {
	    st_ced (brother, is_del);
	}

	return;

    case PUT_n:
	if (!check_component (sxson (visited, 1), &val))
	    val = 0;

	{
	    char	s [12];

	    sprintf (s, "@Put (%c)", val);
	    visited->token.string_table_entry = sxstrsave (s);
	}

    case ACTION_ERASE_n:
    case ACTION_NO_n:
    case DECR_n:
    case INCR_n:
    case LOWER_CASE_n:
    case LOWER_TO_UPPER_n:
    case MARK_n:
    case RELEASE_n:
    case RESET_n:
    case SET_n:
    case UPPER_CASE_n:
    case UPPER_TO_LOWER_n:
	visited->node_no = ++node_no;
	visited->i_item_no = item_no;
	visited->item_code = set_action_or_prdct (TRUE, visited->token.string_table_entry);
	visited->d_item_no = item_no;
	return;

    case ALTERNATIVE_n:
    case PRDCT_ET_n:
    case PRDCT_OR_n:
    case SEQUENCE_n:
	visited->node_no = ++node_no;
	visited->i_item_no = item_no;
	brother = sxson (visited, 1);
	st_ced (brother, is_del);
	item_no++;
	st_ced (get_brother (brother), is_del);
	visited->d_item_no = item_no;
	return;

    case CLASS_n:
	x_cc_stack = 0;
	st_ced (sxson (visited, 2), FALSE);
	st_ced (sxson (visited, 1), FALSE);
	return;

    case CLASS_NAME_n:
      visited->item_code = define_class_by_name (visited->token.string_table_entry, /* visited->token.source_index, */
						 cc_stack [1]/* , FALSE */);
	return;

    case COMMENTS_n:
	if (is_comments_defined)
	    sxput_error (visited->token.source_index, "%sIllegal redefinition of \"Comments\".", err_titles [2]
			 /* error */
													       );
	else
	    is_comments_defined = TRUE;

	get_father (visited)->item_code = comments_re_no = define_lex_name (t_to_ate [0], visited->token.source_index);
	return;

    case COMPONENT_n:
	visited->item_code = get_father (visited)->item_code;
	st_ced (sxson (visited, 1), FALSE);
	/* environment_s sera visite depuis component_def */
	return;

    case COMPONENTS_S_n:
	visited->item_code = get_father (visited)->item_code;
	finalize_re (visited->item_code, -1, FALSE, NULL);
	goto list;

    case COMPONENT_DEF_n:
	visited->item_code = get_father (visited)->item_code;
	brother = sxson (visited, 1);
	st_ced (brother, FALSE);

	/* Les composants "heritent" des specifs d'environnement du "master" si
	   la specif existe ds le "master" et n'est pas redefinie dans le composant. */
	{
	    register struct ers_disp_item	*p1, *p2;

	    st_ced_i = get_father (visited)->item_code;
	    p2 = ers_disp + st_ced_i;
	    p1 = ers_disp + current_token_no;
	    p1->post_action = p2->post_action /* pas gravem! */;

	    if (p2->priority_kind && !is_a_son (get_brother (visited), PRIORITY_KIND_S_n))
		p1->priority_kind = p2->priority_kind;

	    if (p2->is_a_user_defined_context &&
		!is_a_son (get_brother (visited), CONTEXT_n) &&
		!is_a_son (get_brother (visited), RESTRICTED_CONTEXT_n) &&
		!is_a_son (get_brother (visited), UNBOUNDED_CONTEXT_n) &&
		!is_a_son (get_brother (visited), UNBOUNDED_RESTRICTED_CONTEXT_n)) {
		p1->is_a_user_defined_context = p2->is_a_user_defined_context;
		p1->restricted_context = p2->restricted_context;
		p1->is_unbounded_context = p2->is_unbounded_context;

		for (st_ced_j = p2->pexcl; st_ced_j < ers_disp [st_ced_i + 1].pexcl; st_ced_j++) {
		    struct exclude_item		*exclude_ptr;

		    exclude_ptr = exclude + st_ced_j;
		    st_put_in_exclude (exclude_ptr->name, exclude_ptr->component, exclude_ptr->source_index);
		}
	    }

	    if (p2->is_keywords_spec &&
		!is_a_son (get_brother (visited), KEYWORDS_SPEC_n)) {
		p1->is_keywords_spec = p2->is_keywords_spec;
		p1->kw_kind = p2->kw_kind;

		for (st_ced_j = p2->pkw; st_ced_j < ers_disp [st_ced_i + 1].pkw; st_ced_j++) {
		    st_put_in_kw (keyword [st_ced_j]);
		}
	    }
	}

	st_ced (get_brother (visited), FALSE);
	/* environment_s */
	st_finalize_exclude ();
	st_finalize_kw ();
	brother = get_brother (brother);
	st_ced (brother, FALSE);
	return;

    case COMPONENT_NAME_DEF_n:
	get_father (visited)->item_code = define_component_name (get_father (visited)->item_code, visited->token.
	     string_table_entry, visited->token.source_index);
	return;

    case COMPONENT_NAME_REF_n:
	return;

    case COMPONENT_REF_n:
        brother = sxson (visited, 1);

	if (brother->name == CONTEXT_COMMENTS_n)
	    st_ced_i = t_to_ate [0];
	else if (brother->name == CONTEXT_EOF_n)
	    st_ced_i = t_to_ate [termax];
	else if (brother->name == CONTEXT_INCLUDE_n)
	    st_ced_i = sxstrsave ("Include");
	else
	    st_ced_i = brother->token.string_table_entry;

	if (get_brother (brother)->name == VOID_n)
	    st_ced_j = 0;
	else
	    st_ced_j = get_brother (brother)->token.string_table_entry;

	st_put_in_exclude (st_ced_i, st_ced_j, brother->token.source_index);
	return;

    case CONTEXT_n:
    case RESTRICTED_CONTEXT_n:
    case UNBOUNDED_CONTEXT_n:
    case UNBOUNDED_RESTRICTED_CONTEXT_n:
	if (check_environments (visited, CONTEXT_n)) {
	    xexclude = ers_disp [current_token_no].pexcl;
	    st_ced (sxson (visited, 1), FALSE);
	    st_finalize_context (visited->name == UNBOUNDED_CONTEXT_n || visited->name == UNBOUNDED_RESTRICTED_CONTEXT_n,
		 visited->name == RESTRICTED_CONTEXT_n || visited->name == UNBOUNDED_RESTRICTED_CONTEXT_n);
	}
	else
	    sxput_error (visited->token.source_index, "%sThis illegal context clause redefinition is ignored.",
		 err_titles [1] /* warning */ );

	return;

    case DENOTATION_S_n:
	synonym_list [++xsl] = xs + visited->degree;
	goto list;

    case EOF_n:
	get_father (visited)->item_code = define_lex_name (t_to_ate [termax], visited->token.source_index);
	return;

    case ERASE_n:
	visited->node_no = ++node_no;
	visited->i_item_no = item_no;
	st_ced (sxson (visited, 1), TRUE);
	visited->d_item_no = item_no;
	return;

    case EXTENDED_CLASS_REF_n:

/* Pour positionner correctement item_code */

	st_ced (get_brother (sxson (visited, 1)), FALSE);

    case EXPRESSION_n:
    case NOT_n:
    case OPTION_n:
    case REF_TRANS_CLOSURE_n:
    case TRANS_CLOSURE_n:
	visited->node_no = ++node_no;
	visited->i_item_no = item_no;
	st_ced (sxson (visited, 1), is_del);
	visited->d_item_no = item_no;
	return;

    case ID_n:
	st_ced_i = retrieve (visited->token.string_table_entry, visited->token.source_index);

	if (get_father (visited)->name == NOT_n) {
	    if (st_ced_i < 0)
		/* abbrev */
		sxput_error (visited->token.source_index, "%sThe operator \"^\" cannot be applied to abbreviations.",
		     err_titles [2] /* error */ );
	    else
		st_ced_i = ref_class_by_name (&(visited->token.string_table_entry), st_ced_i, TRUE);
	}
	visited->item_code = st_ced_i;
	goto a_leaf;

    case IDENTIFIER_n :
	return;

    case ID_DENOTATION_n:
    case STRING_DENOTATION_n:
	synonym_lgth = synonym_lgth + sxstrlen (visited->token.string_table_entry);
	synonym [xs].string_table_entry = visited->token.string_table_entry;
	synonym [xs].source_index = visited->token.source_index;
	xs++;
	return;

    case INCLUDE_n:
	if (is_include_defined)
	    sxput_error (visited->token.source_index, "%sIllegal redefinition of \"Include\".", err_titles [2]
			 /* error */
													      );
	else {
	    is_include_defined = TRUE;
	    visited->token.string_table_entry = sxstrsave ("Include");
	    ate_to_t [visited->token.string_table_entry] = 0;
	}

	get_father (visited)->item_code = include_re_no = define_lex_name (visited->token.string_table_entry, visited->
	     token.source_index);
	return;

    case IS_FALSE_n:
	visited->item_code = prdct_false_code;
	return;

    case IS_FIRST_COL_n:
    case IS_LAST_COL_n:
    case IS_RESET_n:
    case IS_SET_n:
    case PREDICATE_NO_n:
	is_a_predicate_name = FALSE;

predicate:
	if (get_father (visited)->name != EXTENDED_CLASS_REF_n) {
	    visited->node_no = ++node_no;
	    visited->i_item_no = item_no;
	    visited->item_code = predicate_processing (visited->token.string_table_entry, visited->token.source_index,
		 FALSE, is_a_predicate_name);
	    visited->d_item_no = item_no = wa_set_d_item_no (visited->i_item_no, visited->item_code);
	}
	else
	    visited->item_code = predicate_processing (visited->token.string_table_entry, visited->token.source_index,
		 TRUE, is_a_predicate_name);

	return;

    case IS_TRUE_n:
	visited->item_code = prdct_true_code;
	return;

    case KEYWORD_NAME_n:
    {
	int t_code;

	if ((t_code = define_kw (visited->token.string_table_entry, visited->token.source_index)) > 0)
	    st_put_in_kw (t_code);
    }

	return;

    case KEYWORDS_SPEC_n:
	if (check_environments (visited, KEYWORDS_SPEC_n)) {
	    register struct ers_disp_item	*p1 = ers_disp + current_token_no;

	    p1->is_keywords_spec = TRUE;
	    p1->kw_kind = 0; /* Ca peut etre un "COMPONENT" qui a herite de
				l'environnement global. */

	    if (sxson (visited, 1)->name != VOID_n)
		p1->kw_kind |= KW_NOT_;

	    if ((brother = sxson (visited, 3))->name == NOT_KEYWORDS_n)
		p1->kw_kind |= KW_EXCLUDE_ | KW_LIST_;
	    else if (brother->name == KEYWORDS_n)
		p1->kw_kind |= KW_LIST_;

	    if (p1->kw_kind & KW_LIST_) {
		xkw = p1->pkw;
		st_ced (brother, FALSE);
	    }
	}
	else
	    sxput_error (visited->token.source_index, "%sThis illegal redefinition of a \"KEYWORD\" clause is ignored.",
		 err_titles [1] /* warning */ );

	return;


    case LEXICAL_UNIT_NAME_n:
	get_father (visited)->item_code = define_lex_name (visited->token.string_table_entry, visited->token.source_index
	     );
	return;

    case MINUS_n:
	brother = sxson (visited, 1);
	st_ced (brother, FALSE);
	st_ced (get_brother (brother), FALSE);
	x_cc_stack--;
	sxba_minus (cc_stack [x_cc_stack], cc_stack [x_cc_stack + 1]);
	return;

    case OCTAL_CODE_n:
	visited->item_code = define_class_by_octal_code (&(visited->token.string_table_entry), visited->token.
	     source_index, get_father (visited)->name == NOT_n);
	goto a_leaf;

    case PLUS_n:
	brother = sxson (visited, 1);
	st_ced (brother, FALSE);
	st_ced (get_brother (brother), FALSE);
	x_cc_stack--;
	sxba_or (cc_stack [x_cc_stack], cc_stack [x_cc_stack + 1]);
	return;

    case POST_ACTION_n:
	if (current_token_no == comments_re_no)
	    sxput_error (visited->token.source_index, "%sThis illegal occurrence of a post action is ignored.",
		 err_titles [1] /* warning */ );
	else if (check_environments (visited, POST_ACTION_n)) {
	    post_action_no++;
	    ers_disp [current_token_no].post_action = set_action_or_prdct (TRUE, visited->token.string_table_entry);
	}
	else
	    sxput_error (visited->token.source_index, "%sThis illegal post action redefinition is ignored.", err_titles [
		 1] /* warning */ );

	return;

    case PRDCT_EXPRESSION_n:
	node_no = 1;
	item_no = 1;
	visited->node_no = 1;
	visited->i_item_no = 1;
	st_ced (sxson (visited, 1), FALSE);
	visited->d_item_no = item_no;
	max_node_no_in_re = max_node_no_in_re < node_no ? node_no : max_node_no_in_re;
	max_re_lgth = max_re_lgth < item_no + 2 ? item_no + 2 : max_re_lgth;
	return;

    case PRDCT_NOT_n:
	/* traite comme is_erased */
	visited->node_no = ++node_no;
	visited->i_item_no = item_no;
	st_ced (sxson (visited, 1), TRUE);
	visited->d_item_no = item_no;
	return;

    case PREDICATE_NAME_n:
	is_a_predicate_name = TRUE;
	goto predicate;

    case PRIORITY_KIND_S_n:
	if (check_environments (visited, PRIORITY_KIND_S_n))
	    goto list;
	else
	    sxput_error (visited->token.source_index, "%sThis illegal priority clause redefinition is ignored.",
		 err_titles [1] /* warning */ );

	return;

    case PROGRAM_ROOT_LECL_n:
	cc_stack = sxbm_calloc (max_bucket_no + 1, char_max + 1);
	brother = sxson (visited, 1);
	/*  classes */
	is_class_definition = TRUE;
	st_ced (brother, is_del);
	is_class_definition = FALSE;
	xers = 1;
	/* au cas ou il n'y a pas d'abrev */
	brother = sxson (visited, 2);
	st_ced (brother, is_del);
	/* abbreviations */
	brother = get_brother (brother);
	ers_disp [1].prolon = xers;
	st_ced (brother, is_del);


/* tokens */

	for (st_ced_i = current_abbrev_no; st_ced_i <= -1; st_ced_i++) {
	    if (!abbrev [st_ced_i].is_used)
		sxput_error (abbrev [st_ced_i].subtree_ptr->father->son->token.source_index, "%sUnused abbreviation.",
		     err_titles [1]
			     /* warning */
				   );
	}

	brother = get_brother (brother);
	st_ced (brother, is_del);


/* SYNONYMS */

/* On calcule un majorant de la taille cumulee des E.R. */
/* les predicats dynamiques (systeme ou utilisateurs) sont stockes dans ers 
   afin de permettre un traitement homogene avec les noms des predicats 
   expression */

	for (st_ced_i = prdct_false_code + 1; st_ced_i <= xprdct_to_ate; st_ced_i++) {
	    if (prdct_to_ers [st_ced_i] == 0)
		xers = xers + 2; /* calcul de la taille */
	}

	ers_size = xers + terlis_lgth + termax /* gap entre les E.R. */	 + synonym_lgth;
	st_ced (get_brother (brother), FALSE);
	/* REPR_SPEC_s */
	sxbm_free (cc_stack);
	return;

    case REDUCE_REDUCE_n:
	ers_disp [current_token_no].priority_kind = set_priority_kind (ers_disp [current_token_no].priority_kind,
	     Reduce_Reduce, visited->token.source_index);
	return;

    case REDUCE_SHIFT_n:
	ers_disp [current_token_no].priority_kind = set_priority_kind (ers_disp [current_token_no].priority_kind,
	     Reduce_Shift, visited->token.source_index);
	return;

    case REGULAR_EXPRESSION_n:
	node_no = 1;

	if (visited->father->son->name == EOF_n)
	    item_no = 2;
	else
	    item_no = 1;

	visited->node_no = 1;
	visited->i_item_no = 1;
	st_ced (sxson (visited, 1), FALSE);
	visited->d_item_no = item_no;

	if (get_father (visited)->name != ABBREVIATION_n)
	    finalize_re (get_father (visited)->item_code, item_no, FALSE, visited);

	max_node_no_in_re = max_node_no_in_re < node_no ? node_no : max_node_no_in_re;
	max_re_lgth = max_re_lgth < item_no + 2 ? item_no + 2 : max_re_lgth;
	return;

    case SHIFT_REDUCE_n:
	ers_disp [current_token_no].priority_kind = set_priority_kind (ers_disp [current_token_no].priority_kind,
	     Shift_Reduce, visited->token.source_index);
	return;

    case SLICE_n:
	visited->item_code = define_class_by_slice (sxson (visited, 1), sxson (visited, 2), &(visited->token.
	     string_table_entry), get_father (visited)->name == NOT_n);
	goto a_leaf;

    case STRING_n:
	visited->item_code = define_class_by_string (&(visited->token.string_table_entry), visited->token.source_index,
	     get_father (visited)->name == NOT_n);
	goto a_leaf;

    case SYNONYM_S_n:
	synonym_list = (int*) sxalloc ((int) synonym_list_no + 1, sizeof (int));
	synonym = (struct synonym_item*) sxalloc ((int) synonym_no + 1, sizeof (struct synonym_item));
	synonym_list [1] = 1;
	xs = xsl = 1;
	goto list;

    case TOKEN_n:
	st_ced (sxson (visited, 1), FALSE);
	/* environment_s sera visite depuis token_def */
	return;

    case TOKEN_DEF_n:
	brother = sxson (visited, 1);
	st_ced (brother, FALSE);
	st_ced (get_brother (visited), FALSE);
	/* environment_s */
	st_finalize_exclude ();
	st_finalize_kw ();
	brother = get_brother (brother);
	st_ced (brother, FALSE);
	return;


/* zzzz */

    default:
	return;
    }

a_leaf:
    if (is_class_definition) {
	x_cc_stack++;
	sxba_copy (cc_stack [x_cc_stack], class_to_char_set [visited->item_code]);
    }
    else {
	/*    a_leaf_in_re */
	visited->node_no = ++node_no;
	visited->i_item_no = item_no;
	visited->d_item_no = item_no = wa_set_d_item_no (visited->i_item_no, visited->item_code);
	visited->is_erased = is_del;
    }
}




VOID	lecl_ced (visited)
    struct lecl_node	*visited;
{
    /* This entrypoint processes the first pass on Abbreviations and Tokens
   It computes:
   - i_item_no
   - d_item_no
   - node-no
   - is_erased
   - max_node_no_in_re and max_re_lgth
   - ers_size
*/
    synonym_lgth = 0;
    is_include_defined = is_comments_defined = FALSE;
    char_set = sxba_calloc (char_max + 1);
    st_ced (visited, FALSE);
    sxfree (char_set);
}
