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

#define SXNODE struct lecl_node

#define get_brother(x) (x->brother)

#define get_father(x) (x->father)

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "varstr.h"

#include "lecl_node.h"
#include "lecl_nn.h"
#include "lecl_ag.h"
#include "lecl_pcn.h"

char WHAT_LECLSTC[] = "@(#)SYNTAX - $Id: lecl_stc.c 3603 2023-09-23 20:02:36Z garavel $" WHAT_DEBUG;

static SXINT	retrieve (SXINT alpha_table_entry,
			  struct sxsource_coord designator);
static SXINT	st_octal (char *string);
static SXINT	node_no, item_no, i, xers, xs, xsl, x_cc_stack, synonym_lgth;
static SXBA	/* char_max */ *cc_stack /* 1 : max_bucket_no */ ;
static SXBA	/* char_max */ char_set;
static SXBOOLEAN	is_class_definition, is_a_predicate_name, is_comments_defined, is_include_defined;

SXBOOLEAN		lecl_check_octal_code (SXINT ate, struct sxsource_coord designator, SXINT *val)
{
    if (ate == 0)
	return (SXFALSE);

    *val = st_octal (sxstrget (ate));

    if (*val <= 255)
	return (SXTRUE);

    sxerror (designator,
	     err_titles [2][0] /* error */,
	     "%sThis internal value cannot exceed 255.",
	     err_titles [2]+1 /* error */ );
    return (SXFALSE);
}



static SXBOOLEAN	is_a_son (struct lecl_node *node_ptr,
			  SXINT node_name)
{
    /* retourne SXTRUE ssi (au moins) un des fils de "node_ptr" est un noeud de
       nom "node_name". */
    struct lecl_node	*son;

    for (son = node_ptr->son; son != NULL; son = son->brother) {
	if (son->name == node_name)
	    return SXTRUE;
    }

    return SXFALSE;
}



static SXBOOLEAN	check_environments (struct lecl_node *node_ptr,
				    SXINT name)
{
    struct lecl_node	*son;

    for (son = node_ptr->father->son; son != node_ptr; son = son->brother) {
	switch (son->name) {
	case KEYWORDS_SPEC_n:
	case POST_ACTION_n:
	case PRIORITY_KIND_S_n:
	    if (son->name == name)
		return SXFALSE;

	    break;

	case CONTEXT_n:
	case RESTRICTED_CONTEXT_n:
	case UNBOUNDED_CONTEXT_n:
	case UNBOUNDED_RESTRICTED_CONTEXT_n:
	    if (name == CONTEXT_n)
		return SXFALSE;

	    break;

	default:
	    break;
	}
    }

    return SXTRUE;
}



static SXBOOLEAN	check_component (struct lecl_node *node_ptr,
				 SXINT *val)
{
    SXINT		alpha, class_no;

    alpha = node_ptr->token.string_table_entry;

    if (alpha == 0)
	return (SXFALSE);

    if (node_ptr->name == OCTAL_CODE_n)
	return (lecl_check_octal_code (node_ptr->token.string_table_entry, node_ptr->token.source_index, val));

    if (node_ptr->name == ID_n) {
	class_no = retrieve (alpha, node_ptr->token.source_index);

	if (class_no < 0 /* abbrev */ ) {
	    sxerror (node_ptr->token.source_index,
		     err_titles [2][0] /* error */,
		     "%sIllegal occurrence of an abbreviation name.",
		     err_titles [2]+1 /* error */ );
	    return (SXFALSE);
	}

	if (sxba_cardinal (class_to_char_set [class_no]) != 1) {
	    sxerror (node_ptr->token.source_index,
		     err_titles [2][0] /* error */ ,
		     "%sThis class name must reference a single character set.",
		     err_titles [2]+1 /* error */ );
	    return (SXFALSE);
	}

	*val = sxba_scan (class_to_char_set [class_no], 0) - 1;
	return (SXTRUE);
    }
    else {
	char	*s;


/* character_string */

	if (sxstrlen (alpha) == 3) {
	    s = sxstrget (alpha);
	    *val = ((unsigned char)*++s);
	    return (SXTRUE);
	}

	sxerror (node_ptr->token.source_index,
		 err_titles [2][0] /* error */,
		 "%sOnly a single character set is allowed as component of a slice.",
		 err_titles [2]+1 /* error */ );
	return (SXFALSE);
    }
}



static SXBOOLEAN	st_check_slice (struct lecl_node *ptr1, 
				struct lecl_node *ptr2, 
				SXINT *bi, 
				SXINT *bs)
{
    if (!check_component (ptr1, bi) || !check_component (ptr2, bs))
	return (SXFALSE);

    if (*bs >= *bi)
	return (SXTRUE);

    sxerror (ptr1->token.source_index,
	     err_titles [2][0] /* error */,
	     "%sEmpty slices are not allowed.",
	     err_titles [2]+1 /* error */ );
    return (SXFALSE);
}



static SXINT	look_in_table (SXBA look_in_table_char_set, 
			       SXINT *alpha_table_entry, 
			       SXBOOLEAN is_not)
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

    printf ("CED (look_in_table): class_size=%ld, cmax=%ld, name=%s\n", (SXINT) class_size, (SXINT) cmax, sxstrget (*alpha_table_entry));

#endif

    abbrev_or_class_to_ate [cmax] = *alpha_table_entry;
    sxba_copy (class_to_char_set [cmax], look_in_table_char_set);
    return (cmax);
}



static SXINT	define_class_by_name (SXINT alpha_table_entry,
				      /* struct sxsource_coord designator, */
				      SXBA val
				      /* SXBOOLEAN is_not */)
{
    SXINT		define_class_by_name_i;

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

    printf ("CED (define_class_by_name): class_size=%ld, cmax=%ld, name=%s\n", (SXINT) class_size, (SXINT) cmax, sxstrget (
	 alpha_table_entry));

#endif

    abbrev_or_class_to_ate [cmax] = alpha_table_entry;
    sxba_copy (class_to_char_set [cmax], val);
    return (cmax);
    /* 
end define_class_by_name */
}



static SXINT	define_class_by_string (SXINT *alpha_table_entry, 
					struct sxsource_coord designator, 
					SXBOOLEAN is_not)
{
    SXINT 		car, delta;
    char		*string, *new_string, *old_string;
    char			s [259];
    struct sxsource_coord	pos;
    SXBOOLEAN			is_warning;

    if (*alpha_table_entry == 0)
	return (0);

    old_string = string = sxstrget (*alpha_table_entry);
    sxba_empty (char_set);
    is_warning = SXFALSE;
    delta = 0;

    while (*(++string + 1) != SXNUL) {
	++delta;
	/* Attention, *string peut etre negatif si son code interne depasse 0127 */
	car = ((unsigned char)*string) + 1;

	if (!SXBA_bit_is_reset_set (char_set, car)) {
	    is_warning = SXTRUE;
	    pos = designator;
	    pos.column += delta;
	    sxerror (pos,
		     err_titles [1][0] /* warning */,
		     "%sDuplicate character.",
		     err_titles [1]+1 /* warning */ );
	}
    }

    if (is_warning) {
	/* On refabrique une bonne "alpha_table_entry". */
	new_string = s;
	string = old_string;
	*new_string++ = '"';
	sxba_empty (char_set);

	while (*(++string + 1) != SXNUL) {
	    car = ((unsigned char)*string) + 1;

	    if (SXBA_bit_is_reset_set (char_set, car)) {
		*new_string++ = *string;
	    }
	}

	*new_string++ = '"';
	*new_string = SXNUL;
	*alpha_table_entry = sxstrsave (s);
    }

    return look_in_table (char_set, alpha_table_entry, is_not);
}



static SXINT	define_class_by_octal_code (SXINT *alpha_table_entry, 
					    struct sxsource_coord designator, 
					    SXBOOLEAN is_not)
{
    SXINT		codofcar;

    if (!lecl_check_octal_code (*alpha_table_entry, designator, &codofcar))
	return (0);

    sxba_empty (char_set);
    codofcar++;
    SXBA_1_bit (char_set, codofcar);
    return look_in_table (char_set, alpha_table_entry, is_not);
}



static SXINT	define_class_by_slice (struct lecl_node *ptr1, 
				       struct lecl_node *ptr2, 
				       SXINT *alpha_table_entry, 
				       SXBOOLEAN is_not)
{
    SXINT	define_class_by_slice_i;
    SXINT		bi, bs;

    if (!st_check_slice (ptr1, ptr2, &bi, &bs))
	return (0);

    sxba_empty (char_set);

    for (define_class_by_slice_i = bi + 1; define_class_by_slice_i <= bs + 1; define_class_by_slice_i++)
	SXBA_1_bit (char_set, define_class_by_slice_i);

    return look_in_table (char_set, alpha_table_entry, is_not);
}



static SXINT	ref_class_by_name (SXINT *alpha_table_entry, 
				   SXINT class, 
				   SXBOOLEAN is_not)
{
    /* is_not always true */
    sxba_copy (char_set, class_to_char_set [class]);
    return look_in_table (char_set, alpha_table_entry, is_not);
}



static SXINT	commun (SXINT alpha_table_entry, 
			SXINT t_code, 
			SXINT *ref, 
			SXINT tok_no, 
			struct sxsource_coord designator)
{
    SXINT		commun_i;
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
    ers_disp_ptr->is_a_user_defined_context = SXFALSE;
    ers_disp_ptr->post_action = 0;
    return (current_token_no);
}



static SXINT	define_lex_name (SXINT alpha_table_entry,
				 struct sxsource_coord designator)
{
    SXINT		ref, t_code, tok_no;

    sxinitialise (t_code);
    tok_no = current_token_no + 1;

    if (alpha_table_entry > 0) {
	t_code = ate_to_t [alpha_table_entry];

	if (t_code == -2) {
	    /* dummy token name */
	    char	*s;

	    s = sxstrget (alpha_table_entry);

	    if (*s == '%' || *s == '\"')
		sxerror (designator,
			 err_titles [1][0] /* warning */,
			 "%sThis unknown terminal denotation is supposed to be a dummy token name.",
			 err_titles [1]+1 /* warning */ );

	    ref = -alpha_table_entry;
	}
	else if (t_code == -1) {
	    /* ambiguite entre tokens */
	    sxerror (designator,
		     err_titles [2][0] /* error */ ,
		     "%sAmbiguous definition: use a string literal denotation.",
		     err_titles [2]+1 /* error */ );
	    ref = 0;
	}
	else {
	    ref = alpha_table_entry;

	    if (t_code > 0) {
		if (!SXBA_bit_is_reset_set (t_is_defined_by_a_token, t_code))
		    sxerror (designator,
			     err_titles [2][0] /* error */,
			     "%sIllegal redefinition of a lexical token.",
			     err_titles [2]+1 /* error */ );
	    }
	}
    }
    else {
       ref = 0;
    }
    return (commun (alpha_table_entry, t_code, &ref, tok_no, designator));
}



static SXINT	define_kw (SXINT alpha_table_entry, struct sxsource_coord designator)
{
    SXINT		t_code;

    if (alpha_table_entry == 0)
	return 0;

    if ((t_code = ate_to_t [alpha_table_entry]) <= 0) {
	if (t_code == -1) {
	    /* ambiguite entre tokens */
	    sxerror (designator,
		     err_titles [1][0] /* error */,
		     "%sThis ambiguous syntactic level terminal reference is ignored:\n\
\tuse a string literal denotation.",
		     err_titles [1]+1 /* error */);
	}
	else {
	    /* Not a syntactic level terminal (Or Comments). */
	    sxerror (designator,
		     err_titles [1][0] /* warning */,
		     "%sThis unknown syntactic level terminal denotation is ignored.",
		     err_titles [1]+1 /* warning */ );
	}
	
	return 0;
    }

    /* On verifie que le terminal est non generique. */
    if (SXBA_bit_is_set (is_a_generic_terminal, t_code)) {
	    sxerror (designator,
		     err_titles [1][0] /* warning */,
		     "%sA generic terminal cannot be a keyword, it is ignored.",
		     err_titles [1]+1 /* warning */ );
	    return 0;
    }

    return t_code;
}



static SXINT	define_component_name (SXINT token_no, SXINT alpha_table_entry, struct sxsource_coord designator)
{
    SXINT		define_component_name_i, ref, t_code, tok_no;

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



static SXVOID	finalize_re (SXINT re_no, SXINT finalize_re_item_no, SXBOOLEAN is_abbrev, struct lecl_node *node_ptr)
{
    SXINT		x;

    x = re_no;

    if (is_abbrev) {
	abbrev [re_no].subtree_ptr = node_ptr;
	x--;
	xers = abbrev [re_no].pront + finalize_re_item_no + 1;
	abbrev [x].pront = xers;
	abbrev [x].is_used = SXFALSE;
    }
    else {
	ers_disp [re_no].subtree_ptr = node_ptr;
	x++;
	xers = ers_disp [re_no].prolon + finalize_re_item_no + 1;
	ers_disp [x].prolon = xers;
    }
}



static SXINT	set_action_or_prdct (SXBOOLEAN is_action, SXINT alpha_table_entry)
{
    SXINT		set_action_or_prdct_i, x;

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

    printf ("CED (set_action_or_prdct): action_or_prdct_to_ate [-%ld <= %ld <= %ld] =%s\n", (SXINT) action_size, (SXINT) x, (SXINT) predicate_size,
	 sxstrget (alpha_table_entry));

#endif

    action_or_prdct_to_ate [x] = alpha_table_entry;
    return (x);
}



static SXINT	retrieve (SXINT alpha_table_entry,
			  struct sxsource_coord designator)
{
    SXINT		retrieve_i;


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
	    abbrev [retrieve_i].is_used = SXTRUE;
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
    sxerror (designator,
	     err_titles [2][0] /* error */,
	     "%sNot declared.",
	     err_titles [2]+1 /* error */ );
    return (0);
}



static SXINT	wa_set_d_item_no (SXINT wa_set_d_item_no_item_no, SXINT item_code)
{
    if (item_code >= 0)
	return (wa_set_d_item_no_item_no);
    else
	return (wa_set_d_item_no_item_no + abbrev [item_code - 1].pront - abbrev [item_code].pront - 2);
}



static SXINT	st_define_abbrev (SXINT alpha_table_entry)
{
    /* il est possible de redefinir une abreviation qcq */
    abbrev_or_class_to_ate [--current_abbrev_no] = alpha_table_entry;
    abbrev [current_abbrev_no].prdct_no = 0;
    return (current_abbrev_no);
}



static SXINT	st_octal (char *string)
{
    SXINT		value;

    value = 0;

    for (string++; *string != SXNUL; string++) {
	value = value * 8 + *string - '0';
    }

    return (value);
}



static char	set_priority_kind (char old, char current, struct sxsource_coord designator)
{
    if (old & current) {
	sxerror (designator,
		 err_titles [1][0] /* warning */,
		 "%sThis priority kind is already specified.",
		 err_titles [1]+1 /* warning */ );
	return (old);
    }

    if (current == Reduce_Reduce)
	return (Reduce_Reduce | old);

    if (old & (Shift_Reduce + Reduce_Shift)) {
	sxerror (designator,
		 err_titles [1][0] /* warning */,
		 "%sThis priority kind is incompatible with a previous one, it is ignored.",
		 err_titles [1]+1 /* warning */ );
	return (old);
    }

    return (old | current);
}



static SXVOID	st_put_in_exclude (SXINT name, SXINT component, struct sxsource_coord designator)
{
    struct exclude_item		*exclude_ptr;

    exclude_ptr = exclude + xexclude++;
    exclude_ptr->name = name;
    exclude_ptr->component = component;
    exclude_ptr->source_index = designator;
}



static SXVOID	st_put_in_kw (SXINT t_code)
{
    keyword [xkw++] = t_code;
}



static SXVOID	st_finalize_exclude (void)
{
    ers_disp [current_token_no + 1].pexcl = xexclude;
}


static SXVOID	st_finalize_kw (void)
{
    ers_disp [current_token_no + 1].pkw = xkw;
}



static SXVOID	st_finalize_context (SXBOOLEAN is_unbounded, SXBOOLEAN kind)
{
    struct ers_disp_item	*ers_disp_ptr;

    ers_disp_ptr = ers_disp + current_token_no;
    ers_disp_ptr->restricted_context = kind;
    ers_disp_ptr->is_a_user_defined_context = SXTRUE;
    ers_disp_ptr->is_unbounded_context = is_unbounded;
}



static SXINT	predicate_processing (SXINT ate, 
				      struct sxsource_coord designator, 
				      SXBOOLEAN is_in_re, 
				      SXBOOLEAN is_an_abbrev)
{
    SXINT		abbrev_no, prdct_no;

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

	    printf ("CED (predicate_processing): action_or_prdct_to_ate [-%ld <= %ld <= %ld] =%s\n", (SXINT) action_size,
		 (SXINT) xprdct_to_ate, (SXINT) predicate_size, sxstrget (ate));

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
	prdct_no = set_action_or_prdct (SXFALSE, ate);
	prdct_to_ers [prdct_no] = 0;
	/* sera rempli dans lecl.symbol_table */
	return (prdct_no);
    }
}



static SXVOID	st_ced (struct lecl_node *visited,
			SXBOOLEAN	is_del)
{
    /* collect explicit declarations of abbreviations and tokens */
    struct lecl_node	*brother;
    SXINT		val, st_ced_i, st_ced_j;


#ifdef DEBUG
    printf ("CED visited_node = %d\n", visited->name);
#endif

    switch (visited->name) {
    case ABBREVIATION_n:
	st_ced (sxson (visited, 2), SXFALSE);
	st_ced (sxson (visited, 1), SXFALSE);
	finalize_re (visited->item_code, item_no, SXTRUE, sxson (visited, 2));
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

	    sprintf (s, "@Put (%c)", (char)val);
	    visited->token.string_table_entry = sxstrsave (s);
	}
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	/* absence de "break" (probablement intentionnelle) */
	__attribute__ ((fallthrough));
#endif

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
	visited->item_code = set_action_or_prdct (SXTRUE, visited->token.string_table_entry);
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
	st_ced (sxson (visited, 2), SXFALSE);
	st_ced (sxson (visited, 1), SXFALSE);
	return;

    case CLASS_NAME_n:
      visited->item_code = define_class_by_name (visited->token.string_table_entry, /* visited->token.source_index, */
						 cc_stack [1]/* , SXFALSE */);
	return;

    case COMMENTS_n:
	if (is_comments_defined)
	    sxerror (visited->token.source_index,
		     err_titles [2][0] /* error */,
		     "%sIllegal redefinition of \"Comments\".",
		     err_titles [2]+1 /* error */ );
	else
	    is_comments_defined = SXTRUE;

	get_father (visited)->item_code = comments_re_no = define_lex_name (t_to_ate [0], visited->token.source_index);
	return;

    case COMPONENT_n:
	visited->item_code = get_father (visited)->item_code;
	st_ced (sxson (visited, 1), SXFALSE);
	/* environment_s sera visite depuis component_def */
	return;

    case COMPONENTS_S_n:
	visited->item_code = get_father (visited)->item_code;
	finalize_re (visited->item_code, (SXINT)-1, SXFALSE, (struct lecl_node *)NULL);
	goto list;

    case COMPONENT_DEF_n:
	visited->item_code = get_father (visited)->item_code;
	brother = sxson (visited, 1);
	st_ced (brother, SXFALSE);

	/* Les composants "heritent" des specifs d'environnement du "master" si
	   la specif existe ds le "master" et n'est pas redefinie dans le composant. */
	{
	    struct ers_disp_item	*p1, *p2;

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

	st_ced (get_brother (visited), SXFALSE);
	/* environment_s */
	st_finalize_exclude ();
	st_finalize_kw ();
	brother = get_brother (brother);
	st_ced (brother, SXFALSE);
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
	    st_ced (sxson (visited, 1), SXFALSE);
	    st_finalize_context ((SXBOOLEAN) (visited->name == UNBOUNDED_CONTEXT_n || visited->name == UNBOUNDED_RESTRICTED_CONTEXT_n),
				 (SXBOOLEAN) (visited->name == RESTRICTED_CONTEXT_n || visited->name == UNBOUNDED_RESTRICTED_CONTEXT_n));
	}
	else
	    sxerror (visited->token.source_index,
		     err_titles [1][0] /* warning */ ,
		     "%sThis illegal context clause redefinition is ignored.",
		     err_titles [1]+1 /* warning */ );

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
	st_ced (sxson (visited, 1), SXTRUE);
	visited->d_item_no = item_no;
	return;

    case EXTENDED_CLASS_REF_n:

/* Pour positionner correctement item_code */

	st_ced (get_brother (sxson (visited, 1)), SXFALSE);
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	/* absence de "break" (probablement intentionnelle) */
	__attribute__ ((fallthrough));
#endif

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
		sxerror (visited->token.source_index,
			 err_titles [2][0] /* error */,
			 "%sThe operator \"^\" cannot be applied to abbreviations.",
			 err_titles [2]+1 /* error */ );
	    else
		st_ced_i = ref_class_by_name (&(visited->token.string_table_entry), st_ced_i, SXTRUE);
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
	    sxerror (visited->token.source_index,
		     err_titles [2][0] /* error */,
		     "%sIllegal redefinition of \"Include\".",
		     err_titles [2]+1 /* error */ );
	else {
	    is_include_defined = SXTRUE;
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
	is_a_predicate_name = SXFALSE;

predicate:
	if (get_father (visited)->name != EXTENDED_CLASS_REF_n) {
	    visited->node_no = ++node_no;
	    visited->i_item_no = item_no;
	    visited->item_code = predicate_processing (visited->token.string_table_entry, visited->token.source_index,
		 SXFALSE, is_a_predicate_name);
	    visited->d_item_no = item_no = wa_set_d_item_no (visited->i_item_no, visited->item_code);
	}
	else
	    visited->item_code = predicate_processing (visited->token.string_table_entry, visited->token.source_index,
		 SXTRUE, is_a_predicate_name);

	return;

    case IS_TRUE_n:
	visited->item_code = prdct_true_code;
	return;

    case KEYWORD_NAME_n:
    {
	SXINT t_code;

	if ((t_code = define_kw (visited->token.string_table_entry, visited->token.source_index)) > 0)
	    st_put_in_kw (t_code);
    }

	return;

    case KEYWORDS_SPEC_n:
	if (check_environments (visited, KEYWORDS_SPEC_n)) {
	    struct ers_disp_item	*p1 = ers_disp + current_token_no;

	    p1->is_keywords_spec = SXTRUE;
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
		st_ced (brother, SXFALSE);
	    }
	}
	else
	    sxerror (visited->token.source_index,
		     err_titles [1][0] /* warning */,
		     "%sThis illegal redefinition of a \"KEYWORD\" clause is ignored.",
		     err_titles [1]+1 /* warning */ );

	return;


    case LEXICAL_UNIT_NAME_n:
	get_father (visited)->item_code = define_lex_name (visited->token.string_table_entry, visited->token.source_index
	     );
	return;

    case MINUS_n:
	brother = sxson (visited, 1);
	st_ced (brother, SXFALSE);
	st_ced (get_brother (brother), SXFALSE);
	x_cc_stack--;
	sxba_minus (cc_stack [x_cc_stack], cc_stack [x_cc_stack + 1]);
	return;

    case OCTAL_CODE_n:
	visited->item_code = define_class_by_octal_code (&(visited->token.string_table_entry), visited->token.
							 source_index, (SXBOOLEAN) (get_father (visited)->name == NOT_n));
	goto a_leaf;

    case PLUS_n:
	brother = sxson (visited, 1);
	st_ced (brother, SXFALSE);
	st_ced (get_brother (brother), SXFALSE);
	x_cc_stack--;
	sxba_or (cc_stack [x_cc_stack], cc_stack [x_cc_stack + 1]);
	return;

    case POST_ACTION_n:
	if (current_token_no == comments_re_no)
	    sxerror (visited->token.source_index,
		     err_titles [1][0] /* warning */,
		     "%sThis illegal occurrence of a post action is ignored.",
		     err_titles [1]+1 /* warning */ );
	else if (check_environments (visited, POST_ACTION_n)) {
	    post_action_no++;
	    ers_disp [current_token_no].post_action = set_action_or_prdct (SXTRUE, visited->token.string_table_entry);
	}
	else
	    sxerror (visited->token.source_index,
		     err_titles [1][0] /* warning */ ,
		     "%sThis illegal post action redefinition is ignored.",
		     err_titles [1]+1 /* warning */ );

	return;

    case PRDCT_EXPRESSION_n:
	node_no = 1;
	item_no = 1;
	visited->node_no = 1;
	visited->i_item_no = 1;
	st_ced (sxson (visited, 1), SXFALSE);
	visited->d_item_no = item_no;
	max_node_no_in_re = max_node_no_in_re < node_no ? node_no : max_node_no_in_re;
	max_re_lgth = max_re_lgth < item_no + 2 ? item_no + 2 : max_re_lgth;
	return;

    case PRDCT_NOT_n:
	/* traite comme is_erased */
	visited->node_no = ++node_no;
	visited->i_item_no = item_no;
	st_ced (sxson (visited, 1), SXTRUE);
	visited->d_item_no = item_no;
	return;

    case PREDICATE_NAME_n:
	is_a_predicate_name = SXTRUE;
	goto predicate;

    case PRIORITY_KIND_S_n:
	if (check_environments (visited, PRIORITY_KIND_S_n))
	    goto list;
	else
	    sxerror (visited->token.source_index,
		     err_titles [1][0] /* warning */,
		     "%sThis illegal priority clause redefinition is ignored.",
		     err_titles [1]+1 /* warning */ );

	return;

    case PROGRAM_ROOT_LECL_n:
	cc_stack = sxbm_calloc (max_bucket_no + 1, char_max + 1);

	brother = sxson (visited, 1);
	/*  classes */
	is_class_definition = SXTRUE;
	st_ced (brother, is_del);
	is_class_definition = SXFALSE;
	xers = 1;
	/* au cas ou il n'y a pas d'abrev */

	brother = sxson (visited, 2);
	/* abbreviations */
	st_ced (brother, is_del);

	brother = get_brother (brother);
	/* zombies */
	st_ced (brother, is_del);

	brother = get_brother (brother);
	/* tokens */
	ers_disp [1].prolon = xers;
	st_ced (brother, is_del);

	for (st_ced_i = current_abbrev_no; st_ced_i <= -1; st_ced_i++) {
	    if (!abbrev [st_ced_i].is_used)
		sxerror (abbrev [st_ced_i].subtree_ptr->father->son->token.source_index,
			 err_titles [1][0] /* warning */,
			 "%sUnused abbreviation.",
			 err_titles [1]+1 /* warning */ );
	}

	brother = get_brother (brother);
	/* synonyms */
	st_ced (brother, is_del);

/* On calcule un majorant de la taille cumulee des E.R. */
/* les predicats dynamiques (systeme ou utilisateurs) sont stockes dans ers 
   afin de permettre un traitement homogene avec les noms des predicats 
   expression */

	for (st_ced_i = prdct_false_code + 1; st_ced_i <= xprdct_to_ate; st_ced_i++) {
	    if (prdct_to_ers [st_ced_i] == 0)
		xers = xers + 2; /* calcul de la taille */
	}

	ers_size = xers + terlis_lgth + termax /* gap entre les E.R. */	 + synonym_lgth;

	st_ced (get_brother (brother), SXFALSE);
	/* REPR_SPEC_s */
	sxbm_free (cc_stack);
	return;

    case REDUCE_REDUCE_n:
	ers_disp [current_token_no].priority_kind = set_priority_kind (ers_disp [current_token_no].priority_kind,
	     (char)Reduce_Reduce, visited->token.source_index);
	return;

    case REDUCE_SHIFT_n:
	ers_disp [current_token_no].priority_kind = set_priority_kind (ers_disp [current_token_no].priority_kind,
	     (char)Reduce_Shift, visited->token.source_index);
	return;

    case REGULAR_EXPRESSION_n:
	node_no = 1;

	if (visited->father->son->name == EOF_n)
	    item_no = 2;
	else
	    item_no = 1;

	visited->node_no = 1;
	visited->i_item_no = 1;
	st_ced (sxson (visited, 1), SXFALSE);
	visited->d_item_no = item_no;

	if (get_father (visited)->name != ABBREVIATION_n)
	    finalize_re (get_father (visited)->item_code, item_no, SXFALSE, visited);

	max_node_no_in_re = max_node_no_in_re < node_no ? node_no : max_node_no_in_re;
	max_re_lgth = max_re_lgth < item_no + 2 ? item_no + 2 : max_re_lgth;
	return;

    case SHIFT_REDUCE_n:
	ers_disp [current_token_no].priority_kind = set_priority_kind (ers_disp [current_token_no].priority_kind,
	     (char)Shift_Reduce, visited->token.source_index);
	return;

    case SLICE_n:
	visited->item_code = define_class_by_slice (sxson (visited, 1), sxson (visited, 2),
						    &(visited->token.string_table_entry), (SXBOOLEAN) (get_father (visited)->name == NOT_n));
	goto a_leaf;

    case STRING_n:
	visited->item_code = define_class_by_string (&(visited->token.string_table_entry), visited->token.source_index,
						     (SXBOOLEAN) (get_father (visited)->name == NOT_n));
	goto a_leaf;

    case SYNONYM_S_n:
	synonym_list = (SXINT*) sxalloc ((SXINT) synonym_list_no + 1, sizeof (SXINT));
	synonym = (struct synonym_item*) sxalloc ((SXINT) synonym_no + 1, sizeof (struct synonym_item));
	synonym_list [1] = 1;
	xs = xsl = 1;
	goto list;

    case TOKEN_n:
	st_ced (sxson (visited, 1), SXFALSE);
	/* environment_s sera visite depuis token_def */
	return;

    case TOKEN_DEF_n:
	brother = sxson (visited, 1);
	st_ced (brother, SXFALSE);
	st_ced (get_brother (visited), SXFALSE);
	/* environment_s */
	st_finalize_exclude ();
	st_finalize_kw ();
	brother = get_brother (brother);
	st_ced (brother, SXFALSE);
	return;

    case ZOMBIE_NAME_n :
        return;

    case ZOMBIE_S_n :
       {
	 SXINT t_code;

	 brother = sxson (visited, 1);

	 while (brother != NULL) {
	   if ((t_code = ate_to_t [brother->token.string_table_entry]) <= 0)
	     sxerror (brother->token.source_index,
		      err_titles [1][0] /* warning */,
		      "This illegal zombie must be a bnf terminal name: ignored.",
		      err_titles [1]+1 /* warning */);
	   else
	     SXBA_1_bit (zombies_tset, t_code);
	  
	   brother = get_brother (brother);
	 }
       }
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



SXVOID	lecl_ced (struct lecl_node *visited)
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
    is_include_defined = is_comments_defined = SXFALSE;
    char_set = sxba_calloc (char_max + 1);
    st_ced (visited, SXFALSE);
    sxfree (char_set);
}
