/* ********************************************************************
 *  This program has been generated from dc.at                      *
 *  on Wed Jan 13 16:45:14 1993                                     *
 *  by the SYNTAX (*) abstract tree constructor SEMAT               *
 ********************************************************************
 *  (*) SYNTAX is a trademark of INRIA.                             *
 ******************************************************************** */



/* Debut de table des symboles et d'identification pour C (ansi).
   Il reste encore (beaucoup) a faire...
   A permis de tester le nouveau "sxatc.c" et "sxsymbol_table.h"
*/



/*   I N C L U D E S   */
#define SXNODE struct dc_node
#include "sxunix.h"
# include "sxba.h"
# include "XxY.h"
# include "XH.h"
# include "SS.h"
# include "sxsymbol_table.h"
#include "dc_tdef.h"

struct dc_node {
    SXNODE_HEADER_S SXVOID_NAME;
    
    /*
      your attribute declarations...
      */
};
/*
  N O D E   N A M E S
*/
#define ERROR_n 1
#define DECLARATIONS_n 2
#define DECLARATIONS_STRUCT_n 3
#define DECLARATION_INTERNAL_n 4
#define DECLARATION_PARAMETER_n 5
#define DECLARATION_SPECIFIERS_0_n 6
#define DECLARATION_SPECIFIERS_1_n 7
#define DECLARATION_STRUCT_n 8
#define DECLARATOR_n 9
#define DECLARATORS_DIRECT_ABSTRACT_n 10
#define DECLARATORS_INIT_n 11
#define DECLARATORS_STRUCT_n 12
#define DECLARATOR_ABSTRACT_n 13
#define DECLARATOR_ARRAY_n 14
#define DECLARATOR_DIRECT_FUNCTION_n 15
#define DECLARATOR_FUNCTION_n 16
#define DEFINITION_FUNCTION_n 17
#define DEFINITION_FUNCTION_NO_DECLARATION_n 18
#define DEFINITION_FUNCTION_NO_SPECIFIER_n 19
#define DEFINITION_FUNCTION_NO_SPECIFIER_NO_DECLARATION_n 20
#define D_IDENTIFIER_n 21
#define ENUMERATOR_n 22
#define ENUMERATORS_n 23
#define ENUM_NAME_n 24
#define EXTERNAL_DECLARATION_n 25
#define FIELD_n 26
#define IDENTIFIER_n 27
#define IDENTIFIERS_n 28
#define INITIALIZER_n 29
#define INITIALIZERS_n 30
#define OPERATOR_ADDITIVE_n 31
#define OPERATOR_ASSIGNMENT_n 32
#define OPERATOR_BITWISE_n 33
#define OPERATOR_CAST_n 34
#define OPERATOR_COMMA_n 35
#define OPERATOR_CONDITIONAL_n 36
#define OPERATOR_DOT_n 37
#define OPERATOR_FUNCTION_CALL_n 38
#define OPERATOR_INDEXING_n 39
#define OPERATOR_LOGICAL_n 40
#define OPERATOR_MULTIPLICATIVE_n 41
#define OPERATOR_POST_DECR_n 42
#define OPERATOR_POST_INCR_n 43
#define OPERATOR_PRE_DECR_n 44
#define OPERATOR_PRE_INCR_n 45
#define OPERATOR_PTR_n 46
#define OPERATOR_RELATIONAL_n 47
#define OPERATOR_SHIFT_n 48
#define OPERATOR_SIZEOF_EXPR_n 49
#define OPERATOR_SIZEOF_TYPE_n 50
#define OPERATOR_UNARY_n 51
#define PARAMETERS_n 52
#define POINTER_n 53
#define SPECIFIER_ENUM_n 54
#define SPECIFIER_NAMED_ENUM_n 55
#define SPECIFIER_NAMED_STRUCT_n 56
#define SPECIFIER_QUALIFIERS_0_n 57
#define SPECIFIER_QUALIFIERS_1_n 58
#define SPECIFIER_STRUCT_n 59
#define STMT_n 60
#define STMT_BREAK_n 61
#define STMT_CASE_n 62
#define STMT_COMPOUND_n 63
#define STMT_CONTINUE_n 64
#define STMT_DEFAULT_n 65
#define STMT_DO_WHILE_n 66
#define STMT_FOR_n 67
#define STMT_GOTO_n 68
#define STMT_IF_THEN_n 69
#define STMT_IF_THEN_ELSE_n 70
#define STMT_LABELED_n 71
#define STMT_RETURN_n 72
#define STMT_SWITCH_n 73
#define STMT_WHILE_n 74
#define TAG_NAME_n 75
#define TRANSLATION_UNITS_n 76
#define TYPEDEF_NAME_n 77
#define TYPE_NAME_n 78
#define TYPE_QUALIFIERS_n 79
#define VOID_n 80
#define character_constant_n 81
#define floating_constant_n 82
#define integer_constant_n 83
#define string_literal_n 84
/*
E N D   N O D E   N A M E S
  */

#define	UNDEF_NAME	0
#define	LABEL_NAME	1
#define	TAG_NAME	2
#define	MEMBER_NAME	3
#define	ID_NAME		4
#define	TYPEDEF_NAME	5


#define	UNDEF_TYPE_SPECIFIER_		0

/* basic_type */
#define	BASIC_TYPE_RANGE_		0xf
#define	VOID_		0x1
#define	CHAR_		0x2
#define	INT_		0x3
#define	FLOAT_		0x4
#define	DOUBLE_		0x5
#define	STRUCT_		0x6
#define	UNION_		0x7
#define	ENUM_		0x8

/* storage_length */
#define	STORAGE_LENGTH_RANGE_		0xf0
#define	SHORT_		0x10
#define	LONG_		0x20

/* signedness */
#define	SIGNEDNESS_RANGE_		0xf00
#define	SIGNED_		0x100
#define	UNSIGNED_	0x200

/* storage_class */
#define	STORAGE_CLASS_RANGE_		0xf000
#define	EXTERN_		0x1000
#define	STATIC_		0x2000
#define	AUTO_		0x3000
#define	REGISTER_	0x4000

/* type_qualifier */
#define	TYPE_QUALIFIER_RANGE_		0xf0000
#define	CONST_		0x10000
#define	VOLATILE_	0x20000

/* typedef */
#define	TYPEDEF_RANGE_			0xf00000
#define	TYPEDEF_	0x100000

/* parameter */
#define	PARAMETER_		0x1000000
#define	FUNCTION_DECLARATION_	0x2000000
#define	FUNCTION_DEFINITION_	0x4000000


#define IN_PARAMETERS_			1
#define IN_PARAMETER_DECLARATIONS_	2


#define enter_block(b)	SS_push (block_level_stack, current_block_level), current_block_level = b
#define quit_block()	(current_block_level = SS_pop (block_level_stack))
#define leave_block()	(current_block_level = SS_pop (block_level_stack)), --new_block_level
    
#define enter_struct(b)	SS_push (struct_level_stack, current_struct_level), current_struct_level = b
#define quit_struct()	current_struct_level = SS_pop (struct_level_stack)

static int	type_specifier, name_id;
static int	current_block_level, new_block_level, current_struct_level, new_struct_level;
static SXNODE	*declarator_n_ptr;


#define GET_ATTR(x)	((x>=0) ? ids_attr + x : members_attr - x)

static struct sxsymbol_table_hd ids, tags, members;
struct attribute {
    int		type_specifier;
    int		name_id; 	/* typedef, union or struct */
    SXNODE	*declarator; 	/* DECLARATOR_n ou DECLARATOR_FUNCTION_n */
};
static struct attribute	*ids_attr, *members_attr, *pattr;

struct tags_attr {
    int	kind;
    int	level;
};
static struct tags_attr	*tags_attr;

static int		*block_level_stack, *type_specifier_stack, *struct_level_stack,
                        *name_id_stack;
static int		*t_to_class_type, *t_to_decl_spec;

static bool		has_declarator, is_in_type_name;
static int		where_is_declaration_specifier;

struct operand_type {
    bool	is_lvalue;
    int		name_or_type; /* Si <0 designe une entree ds members. */
    int		indirection_level;
};

static struct operand_type	operand_type, *operand_type_stack;
static int	ots_top, ots_size;

#define PUSH_OTS()	(++ots_top > ots_size) ? ots_oflw() : 0, operand_type_stack [ots_top] = operand_type
#define POP_OTS()	(operand_type = operand_type_stack [ots_top--])

static int ots_oflw ()
{
    operand_type_stack = (struct operand_type*)
	sxrealloc (operand_type_stack, (ots_size *= 2) + 1, sizeof (struct operand_type));
    return 0;
}


static void ids_oflw (old_size, new_size)
    int	old_size, new_size;
{
    ids_attr = (struct attribute*) sxrealloc (ids_attr, new_size + 1, sizeof (struct attribute));
}

static void tags_oflw (old_size, new_size)
    int	old_size, new_size;
{
    tags_attr = (struct tags_attr*) sxrealloc (tags_attr, new_size + 1, sizeof (struct tags_attr));
}

static void members_oflw (old_size, new_size)
    int	old_size, new_size;
{
    members_attr = (struct attribute*) sxrealloc (members_attr, new_size + 1, sizeof (struct attribute));
}




static void init_for_types ()
{
    int tmax;

    tmax = sxeof_code (sxplocals.sxtables);
    t_to_class_type = (int*) sxcalloc (tmax + 1, sizeof (int));
    t_to_decl_spec = (int*) sxcalloc (tmax + 1, sizeof (int));

/* basic_type */
    t_to_class_type [void_la] = BASIC_TYPE_RANGE_, t_to_decl_spec [void_la] = VOID_;
    t_to_class_type [char_la] = BASIC_TYPE_RANGE_, t_to_decl_spec [char_la] = CHAR_;
    t_to_class_type [int_la] = BASIC_TYPE_RANGE_, t_to_decl_spec [int_la] = INT_;
    t_to_class_type [float_la] = BASIC_TYPE_RANGE_, t_to_decl_spec [float_la] = FLOAT_;
    t_to_class_type [double_la] = BASIC_TYPE_RANGE_, t_to_decl_spec [double_la] = DOUBLE_;
    t_to_class_type [struct_la] = BASIC_TYPE_RANGE_, t_to_decl_spec [struct_la] = STRUCT_;
    t_to_class_type [union_la] = BASIC_TYPE_RANGE_, t_to_decl_spec [union_la] = UNION_;
    t_to_class_type [enum_la] = BASIC_TYPE_RANGE_, t_to_decl_spec [enum_la] = ENUM_;

/* storage_length */
    t_to_class_type [short_la] = STORAGE_LENGTH_RANGE_, t_to_decl_spec [short_la] = SHORT_;
    t_to_class_type [long_la] = STORAGE_LENGTH_RANGE_, t_to_decl_spec [long_la] = LONG_;

/* signedness */
    t_to_class_type [signed_la] = SIGNEDNESS_RANGE_, t_to_decl_spec [signed_la] = SIGNED_;
    t_to_class_type [unsigned_la] = SIGNEDNESS_RANGE_, t_to_decl_spec [unsigned_la] = UNSIGNED_;

/* storage_class */
    t_to_class_type [extern_la] = STORAGE_CLASS_RANGE_, t_to_decl_spec [extern_la] = EXTERN_;
    t_to_class_type [static_la] = STORAGE_CLASS_RANGE_, t_to_decl_spec [static_la] = STATIC_;
    t_to_class_type [auto_la] = STORAGE_CLASS_RANGE_, t_to_decl_spec [auto_la] = AUTO_;
    t_to_class_type [register_la] = STORAGE_CLASS_RANGE_, t_to_decl_spec [register_la] = REGISTER_;

/* type_qualifier */
    t_to_class_type [const_la] = TYPE_QUALIFIER_RANGE_, t_to_decl_spec [const_la] = CONST_;
    t_to_class_type [volatile_la] = TYPE_QUALIFIER_RANGE_, t_to_decl_spec [volatile_la] = VOLATILE_;

/* typedef */
    t_to_class_type [typedef_la] = TYPEDEF_RANGE_, t_to_decl_spec [typedef_la] = TYPEDEF_;
}


static void final_for_type ()
{
    sxfree (t_to_decl_spec);
    sxfree (t_to_class_type);
}


static int t_to_type (type, t, si)
    int type, t;
    struct sxsource_coord	*si;
{
    /* t is the internal code of a terminal symbol (type keyword) */
    /* check its validity and returns a new type. */
    if (type & t_to_class_type [t])
	sxput_error (*si,
		     "%sIllegal declaration specifier occurrence.",
		     sxplocals.sxtables->err_titles [2]);
    else
	type |= t_to_decl_spec [t];

    return type;
}


static int process_tag_name (string_table_entry, kind, struct_level, source_index)
    int		string_table_entry;
    int		kind; /* STRUCT_ or UNION_ */
    int		struct_level;
    struct sxsource_coord	*source_index;
{
    bool	is_ok;
    int		x;
    struct tags_attr	*attr;
    
    /* Si struct_level == 0 => declaration
    /* Si string_table_entry < 0 => definition anonyme (|string_table_entry| == struct_level) */
    is_ok = sxsymbol_table_put (&tags,
				string_table_entry,
				current_block_level,
				&x);
    attr = tags_attr + x;
    
    if (is_ok) {
	attr->kind = kind;
	attr->level = struct_level;
    }
    else {
	/* Deja declare au meme niveau. */
	if (attr->kind != kind)
	    sxput_error (*source_index,
			 "%sIllegal redeclaration of a%s specifier.",
			 sxplocals.sxtables->err_titles [2],
			 attr->kind == STRUCT_ ? " struct" : "n union");
	else if (attr->level == 0 && struct_level > 0) {
	    /* definition apres declaration => OK */
	    attr->level = struct_level;
	}
	else 
	    sxput_error (*source_index,
			 "%sIllegal rede%stion of a%s specifier.",
			 sxplocals.sxtables->err_titles [2],
			 struct_level == 0 ? "clara" : "fini",
			 attr->kind == STRUCT_ ? " struct" : "n union");
    }
    
    
    return x;
}

static int get_indirection_level (n, node)
    int		n;
    SXNODE 	*node;
{
    /* Parcourt le sous-arbre "node" retourne le nombre cumule d'indirections. */
    SXNODE	*son1, *son2;

    if (node->name == DECLARATOR_n) {
	son1 = node->son;
	son2 = son1->brother;

	if (son1->name == POINTER_n)
	    n = get_indirection_level (n + 1, son1);

	if (son2->name != D_IDENTIFIER_n) /* DECLARATOR_n or DECLARATOR_ARRAY_n */
	    n = get_indirection_level (n, son2);
    }
    else if (node->name == POINTER_n) {
	son2 = node->son->brother;

	if (son2->name == POINTER_n)
	    n = get_indirection_level (n + 1, son2);
    }
    else /* DECLARATOR_ARRAY_n */ {
	son1 = node->son;

	if (son1->name == DECLARATOR_n || son1->name == DECLARATOR_ARRAY_n)
	    n = get_indirection_level (n + 1, son1);
	else /* DECLARATOR_DIRECT_FUNCTION_n, DECLARATOR_FUNCTION_n, D_IDENTIFIER_n */
	    n++;
    }

    return n;
}


static int get_closed_il (ppattr)
    struct attribute **ppattr;
{
    int 		type_specifier, n;
    struct attribute	*pattr = *ppattr;
    
    type_specifier = pattr->type_specifier;
    n = get_indirection_level (0, pattr->declarator);
    
    while (pattr->name_id != 0 &&
	   (type_specifier & BASIC_TYPE_RANGE_) != STRUCT_ &&
	    (type_specifier & BASIC_TYPE_RANGE_) != UNION_) {
	/* pattr->name_id designe un typedef */
	pattr = ids_attr + pattr->name_id;
	type_specifier = pattr->type_specifier;
	n += get_indirection_level (0, pattr->declarator);
    }

    *ppattr = pattr;
    return n;
}


static void check_unary_operator (visited)
    SXNODE	*visited;
{
    /* Visite synthetisee sur "OPERATOR_UNARY_n",
       Son fils de gauche est un operateur unaire (noeud VOID_n).
       Verifie et calcule le nouveau operand_type. */
    if (operand_type.name_or_type == 0)
	return;
    
    pattr = GET_ATTR (operand_type.name_or_type);

    switch (visited->son->token.lahead) {
    case oper_and_la:
	/* The operand of the unary "&" operator shall be either a function
	   designator or an lvalue that designates an object that is not a
	   bit-field and is not declared with the "register" storage_class specifier. */
	if (!operand_type.is_lvalue ||
	    --operand_type.indirection_level < -1 ||
	    (pattr->type_specifier & REGISTER_) == REGISTER_)
	    operand_type.name_or_type = 0;
	
	break;

    case oper_mult_la:
	/* The operand of the unary "*" operator shall have pointer type. */
	if (!operand_type.is_lvalue)
	    operand_type.name_or_type = 0;
	else if (++operand_type.indirection_level > 0) {
	    if (operand_type.indirection_level > get_closed_il (&pattr))
		operand_type.name_or_type = 0;
	}

	break;

    case oper_plus_la:
	break;

    case oper_minus_la:
	break;

    case oper_tilde_la:
	break;

    case oper_not_la:
	break;
	
    default:
	break;
    }

    if (operand_type.name_or_type == 0)
	sxput_error (visited->son->token.source_index,
		     "%sIllegal prefixed unary operator.",
		     sxplocals.sxtables->err_titles [2]);

}

static void dc_pi () {
    
    /*
      I N H E R I T E D
      */
    
    switch (SXVISITED->father->name) {
	
    case ERROR_n :
	break;
	
    case DECLARATIONS_n :/* SXVISITED->name = {DECLARATION_INTERNAL_n, DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n} */
	if (SXVISITED->position == 1) {
	    enter_block (++new_block_level);
	    sxsymbol_table_open (ids, current_block_level);
	}
	
	type_specifier = UNDEF_TYPE_SPECIFIER_;
	where_is_declaration_specifier = IN_PARAMETER_DECLARATIONS_;
	name_id = 0;
	break;
	
    case DECLARATIONS_STRUCT_n :/* SXVISITED->name = DECLARATION_STRUCT_n */
	type_specifier = UNDEF_TYPE_SPECIFIER_;
	name_id = 0;
	break;
	
    case DECLARATION_INTERNAL_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n} */
	    has_declarator = true;
	    break;
	    
	case 2 :/* SXVISITED->name = DECLARATORS_INIT_n */
	    has_declarator = false;
	    break;
	    
	}
	
	break;
	
    case DECLARATION_PARAMETER_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {DECLARATOR_n, DECLARATOR_ABSTRACT_n, DECLARATOR_FUNCTION_n, POINTER_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case DECLARATION_SPECIFIERS_0_n :/* SXVISITED->name = VOID_n */
	type_specifier = t_to_type (type_specifier, SXVISITED->token.lahead, &(SXVISITED->token.source_index));
	break;
	
    case DECLARATION_SPECIFIERS_1_n :/* SXVISITED->name = {DECLARATION_SPECIFIERS_0_n, ENUM_NAME_n, SPECIFIER_ENUM_n, 
					SPECIFIER_NAMED_ENUM_n, SPECIFIER_NAMED_STRUCT_n, SPECIFIER_STRUCT_n, TAG_NAME_n, TYPEDEF_NAME_n, VOID_n} */
	if (SXVISITED->name == VOID_n)
	    type_specifier = t_to_type (type_specifier, SXVISITED->token.lahead, &(SXVISITED->token.source_index));

	break;
	
    case DECLARATION_STRUCT_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {SPECIFIER_QUALIFIERS_0_n, SPECIFIER_QUALIFIERS_1_n} */
	    has_declarator = true;
	    break;
	    
	case 2 :/* SXVISITED->name = DECLARATORS_STRUCT_n */
	    has_declarator = false;
	    break;
	    
	}
	
	break;
	
    case DECLARATOR_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {POINTER_n, VOID_n} */
	    if (declarator_n_ptr == NULL)
		declarator_n_ptr = SXVISITED->father;

	    break;
	    
	case 2 :/* SXVISITED->name = {DECLARATOR_n, DECLARATOR_ARRAY_n, D_IDENTIFIER_n} */
	    break;
	    
	}
	
	break;
	
    case DECLARATORS_DIRECT_ABSTRACT_n :/* SXVISITED->name = {DECLARATOR_ABSTRACT_n, IDENTIFIER_n, OPERATOR_ADDITIVE_n, 
					   OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, 
					   OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, 
					   OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, 
					   OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, PARAMETERS_n, POINTER_n, VOID_n, character_constant_n, 
					   floating_constant_n, integer_constant_n, string_literal_n} */
	break;
	
    case DECLARATORS_INIT_n :/* SXVISITED->name = {DECLARATOR_n, DECLARATOR_FUNCTION_n, INITIALIZER_n} */
	break;
	
    case DECLARATORS_STRUCT_n :/* SXVISITED->name = {DECLARATOR_n, DECLARATOR_FUNCTION_n, FIELD_n, IDENTIFIER_n, OPERATOR_ADDITIVE_n, 
				  OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, 
				  OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, 
				  OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, 
				  OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
				  , string_literal_n} */
	break;
	
    case DECLARATOR_ABSTRACT_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {POINTER_n, VOID_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = DECLARATORS_DIRECT_ABSTRACT_n */
	    break;
	    
	}
	
	break;
	
    case DECLARATOR_ARRAY_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {DECLARATOR_n, DECLARATOR_ARRAY_n, DECLARATOR_DIRECT_FUNCTION_n, DECLARATOR_FUNCTION_n, 
		   D_IDENTIFIER_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, VOID_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case DECLARATOR_DIRECT_FUNCTION_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {DECLARATOR_n, DECLARATOR_ARRAY_n, DECLARATOR_DIRECT_FUNCTION_n, DECLARATOR_FUNCTION_n, 
		   D_IDENTIFIER_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIERS_n, PARAMETERS_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case DECLARATOR_FUNCTION_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {POINTER_n, VOID_n} */
	    if ((type_specifier & FUNCTION_DEFINITION_) == 0)
		type_specifier |= FUNCTION_DECLARATION_;

	    if (declarator_n_ptr == NULL)
		declarator_n_ptr = SXVISITED->father;

	    break;
	    
	case 2 :/* SXVISITED->name = {DECLARATOR_ARRAY_n, DECLARATOR_DIRECT_FUNCTION_n, DECLARATOR_FUNCTION_n} */
	    break;
	    
	}
	
	break;
	
    case DEFINITION_FUNCTION_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = DECLARATOR_FUNCTION_n */
	    type_specifier |= FUNCTION_DEFINITION_;
	    break;
	    
	case 3 :/* SXVISITED->name = DECLARATIONS_n */
	    break;
	    
	case 4 :/* SXVISITED->name = {STMT_COMPOUND_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case DEFINITION_FUNCTION_NO_DECLARATION_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = DECLARATOR_FUNCTION_n */
	    type_specifier |= FUNCTION_DEFINITION_;
	    break;
	    
	case 3 :/* SXVISITED->name = {STMT_COMPOUND_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case DEFINITION_FUNCTION_NO_SPECIFIER_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = DECLARATOR_FUNCTION_n */
	    type_specifier |= FUNCTION_DEFINITION_;
	    break;
	    
	case 2 :/* SXVISITED->name = DECLARATIONS_n */
	    break;
	    
	case 3 :/* SXVISITED->name = {STMT_COMPOUND_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case DEFINITION_FUNCTION_NO_SPECIFIER_NO_DECLARATION_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = DECLARATOR_FUNCTION_n */
	    type_specifier |= FUNCTION_DEFINITION_;
	    break;
	    
	case 2 :/* SXVISITED->name = {STMT_COMPOUND_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case ENUMERATOR_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = D_IDENTIFIER_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case ENUMERATORS_n :/* SXVISITED->name = {D_IDENTIFIER_n, ENUMERATOR_n} */
	break;
	
    case ENUM_NAME_n :/* SXVISITED->name = IDENTIFIER_n */
	type_specifier = t_to_type (type_specifier, SXVISITED->token.lahead, &(SXVISITED->father->token.source_index));
	break;
	
    case EXTERNAL_DECLARATION_n :/* SXVISITED->name = {DECLARATION_INTERNAL_n, DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n
				    , DECLARATORS_INIT_n, DEFINITION_FUNCTION_n, DEFINITION_FUNCTION_NO_DECLARATION_n, DEFINITION_FUNCTION_NO_SPECIFIER_n, 
				    DEFINITION_FUNCTION_NO_SPECIFIER_NO_DECLARATION_n} */
	type_specifier = UNDEF_TYPE_SPECIFIER_;
	name_id = 0;
	break;
	
    case FIELD_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = DECLARATOR_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case IDENTIFIERS_n :/* SXVISITED->name = IDENTIFIER_n */
	if (SXVISITED->position == 1) {
	    enter_block (++new_block_level);
	    sxsymbol_table_open (ids, current_block_level);
	}

	type_specifier = PARAMETER_;
	name_id = 0;
	break;
	
    case INITIALIZER_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = DECLARATOR_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, INITIALIZERS_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, 
		   OPERATOR_BITWISE_n, OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
		   OPERATOR_INDEXING_n, OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, 
		   OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, 
		   OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    /* Pour le cas ou il y a un <type-name> (cast ou sizeof) ds l'expression. */
	    SS_push (type_specifier_stack, type_specifier);
	    SS_push (name_id_stack, name_id);
	    break;
	    
	}
	
	break;
	
    case INITIALIZERS_n :/* SXVISITED->name = {IDENTIFIER_n, INITIALIZERS_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, 
			    OPERATOR_BITWISE_n, OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
			    OPERATOR_INDEXING_n, OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, 
			    OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, 
			    OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;
	
    case OPERATOR_ADDITIVE_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_ASSIGNMENT_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
		   OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
		   OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
		   , string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = VOID_n */
	    break;
	    
	case 3 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    PUSH_OTS(); /* On sauve le tableau. */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_BITWISE_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_CAST_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = TYPE_NAME_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_COMMA_n :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
			      OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
			      OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
			      OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
			      OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;
	
    case OPERATOR_CONDITIONAL_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = OPERATOR_COMMA_n */
	    break;
	    
	case 3 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_DOT_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
		   OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
		   OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
		   , string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = IDENTIFIER_n */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_FUNCTION_CALL_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
		   OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
		   OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
		   , string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {OPERATOR_COMMA_n, VOID_n} */
	    PUSH_OTS(); /* On sauve le nom de la fonction. */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_INDEXING_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
		   OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
		   OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
		   , string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = OPERATOR_COMMA_n */
	    PUSH_OTS(); /* On sauve le tableau. */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_LOGICAL_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_MULTIPLICATIVE_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_POST_DECR_n :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
				  OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
				  OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
				  , string_literal_n} */
	break;
	
    case OPERATOR_POST_INCR_n :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
				  OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
				  OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
				  , string_literal_n} */
	break;
	
    case OPERATOR_PRE_DECR_n :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
				 OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
				 OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
				 , string_literal_n} */
	break;
	
    case OPERATOR_PRE_INCR_n :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
				 OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
				 OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
				 , string_literal_n} */
	break;
	
    case OPERATOR_PTR_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
		   OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
		   OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
		   , string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = IDENTIFIER_n */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_RELATIONAL_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_SHIFT_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case OPERATOR_SIZEOF_EXPR_n :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
				    OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
				    OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
				    , string_literal_n} */
	break;
	
    case OPERATOR_SIZEOF_TYPE_n :/* SXVISITED->name = TYPE_NAME_n */
	break;
	
    case OPERATOR_UNARY_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = VOID_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
		   OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
		   OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
		   , string_literal_n} */
	    break;
	    
	}
	
	break;
	
    case PARAMETERS_n :/* SXVISITED->name = DECLARATION_PARAMETER_n */
	if (SXVISITED->position == 1) {
	    enter_block (++new_block_level);
	    sxsymbol_table_open (ids, current_block_level);
	}
	
	type_specifier = PARAMETER_;
	where_is_declaration_specifier = IN_PARAMETERS_;
	name_id = 0;
	break;
	
    case POINTER_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = TYPE_QUALIFIERS_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {POINTER_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case SPECIFIER_ENUM_n :/* SXVISITED->name = ENUMERATORS_n */
	type_specifier = t_to_type (type_specifier, SXVISITED->token.lahead, &(SXVISITED->token.source_index));
	break;
	
    case SPECIFIER_NAMED_ENUM_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = IDENTIFIER_n */
	    type_specifier = t_to_type (type_specifier, SXVISITED->token.lahead, &(SXVISITED->token.source_index));
	    break;
	    
	case 2 :/* SXVISITED->name = ENUMERATORS_n */
	    break;
	    
	}
	
	break;
	
    case SPECIFIER_NAMED_STRUCT_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = VOID_n */
	    type_specifier = t_to_type (type_specifier, SXVISITED->token.lahead, &(SXVISITED->token.source_index));
	    break;
	    
	case 2 :/* SXVISITED->name = IDENTIFIER_n */
	    break;
	    
	case 3 :/* SXVISITED->name = DECLARATIONS_STRUCT_n */
	    SS_push (type_specifier_stack, type_specifier);
	    SS_push (name_id_stack, name_id);
	    type_specifier = name_id = 0;
	    enter_struct (++new_struct_level);
	    break;
	    
	}
	
	break;
	
    case SPECIFIER_QUALIFIERS_0_n :/* SXVISITED->name = VOID_n */
	break;
	
    case SPECIFIER_QUALIFIERS_1_n :/* SXVISITED->name = {ENUM_NAME_n, SPECIFIER_ENUM_n, SPECIFIER_NAMED_ENUM_n, 
				      SPECIFIER_NAMED_STRUCT_n, SPECIFIER_QUALIFIERS_0_n, SPECIFIER_STRUCT_n, TAG_NAME_n, TYPEDEF_NAME_n, VOID_n} */
	break;
	
    case SPECIFIER_STRUCT_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = VOID_n */
	    type_specifier = t_to_type (type_specifier, SXVISITED->token.lahead, &(SXVISITED->token.source_index));
	    break;
	    
	case 2 :/* SXVISITED->name = DECLARATIONS_STRUCT_n */
	    name_id = process_tag_name (-(new_struct_level + 1),
			      type_specifier & BASIC_TYPE_RANGE_,
			      new_struct_level + 1,
			      &(SXLEFT->token.source_index));
	    SS_push (type_specifier_stack, type_specifier);
	    SS_push (name_id_stack, name_id);
	    type_specifier = name_id = 0;
	    enter_struct (++new_struct_level);
	    break;
	    
	}
	
	break;
	
    case STMT_n :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, STMT_DEFAULT_n, 
		    STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, STMT_SWITCH_n, 
		    STMT_WHILE_n, VOID_n} */
	break;
	
    case STMT_CASE_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
		   OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
		   OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
		   OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
		   OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, 
		   STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, 
		   STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case STMT_COMPOUND_n :/* SXVISITED->name = {DECLARATION_INTERNAL_n, DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n, 
			     OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, 
			     STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	if (SXVISITED->position == 1) {
	    enter_block (++new_block_level);
	    sxsymbol_table_open (ids, current_block_level);
	}

	type_specifier = UNDEF_TYPE_SPECIFIER_;
	name_id = 0;
	break;
	
    case STMT_DEFAULT_n :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, 
			    STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, 
			    STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	break;
	
    case STMT_DO_WHILE_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, 
		   STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, 
		   STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = OPERATOR_COMMA_n */
	    break;
	    
	}
	
	break;
	
    case STMT_FOR_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {OPERATOR_COMMA_n, VOID_n} */
	    break;
	    
	case 2 :/* SXVISITED->name = {OPERATOR_COMMA_n, VOID_n} */
	    break;
	    
	case 3 :/* SXVISITED->name = {OPERATOR_COMMA_n, VOID_n} */
	    break;
	    
	case 4 :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, 
		   STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, 
		   STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case STMT_GOTO_n :/* SXVISITED->name = IDENTIFIER_n */
	break;
	
    case STMT_IF_THEN_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = OPERATOR_COMMA_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, 
		   STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, 
		   STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case STMT_IF_THEN_ELSE_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = OPERATOR_COMMA_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, 
		   STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, 
		   STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	    break;
	    
	case 3 :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, 
		   STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, 
		   STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case STMT_LABELED_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = IDENTIFIER_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, 
		   STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, 
		   STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case STMT_RETURN_n :/* SXVISITED->name = {OPERATOR_COMMA_n, VOID_n} */
	break;
	
    case STMT_SWITCH_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = OPERATOR_COMMA_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, 
		   STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, 
		   STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case STMT_WHILE_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = OPERATOR_COMMA_n */
	    break;
	    
	case 2 :/* SXVISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, 
		   STMT_DEFAULT_n, STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, 
		   STMT_SWITCH_n, STMT_WHILE_n, VOID_n} */
	    break;
	    
	}
	
	break;
	
    case TAG_NAME_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = VOID_n */
	    type_specifier = t_to_type (type_specifier, SXVISITED->token.lahead, &(SXVISITED->token.source_index));
	    break;
	    
	case 2 :/* SXVISITED->name = IDENTIFIER_n */
	    break;
	    
	}
	
	break;
	
    case TRANSLATION_UNITS_n :/* SXVISITED->name = {DECLARATION_INTERNAL_n, DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n, 
				 DECLARATORS_INIT_n, DEFINITION_FUNCTION_n, DEFINITION_FUNCTION_NO_DECLARATION_n, DEFINITION_FUNCTION_NO_SPECIFIER_n, 
				 DEFINITION_FUNCTION_NO_SPECIFIER_NO_DECLARATION_n, VOID_n} */
	break;
	
    case TYPE_NAME_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {SPECIFIER_QUALIFIERS_0_n, SPECIFIER_QUALIFIERS_1_n} */
	    type_specifier = UNDEF_TYPE_SPECIFIER_;
	    name_id = 0;
	    is_in_type_name = true;
	    break;
	    
	case 2 :/* SXVISITED->name = {DECLARATOR_ABSTRACT_n, POINTER_n, VOID_n} */
	    is_in_type_name = false;
	    break;
	    
	}
	
	break;
	
    case TYPE_QUALIFIERS_n :/* SXVISITED->name = VOID_n */
	break;
	
	
	
	/*
	  Z Z Z Z
	  */
	
    default:
	break;
    }
    /* end dc_pi */
}

static void dc_pd () {
    
    /*
      D E R I V E D
      */
    
    switch (SXVISITED->name) {
	
    case ERROR_n :
	break;
	
    case DECLARATIONS_n :
	leave_block();
	where_is_declaration_specifier = 0;
	break;
	
    case DECLARATIONS_STRUCT_n :
	break;
	
    case DECLARATION_INTERNAL_n :
	break;
	
    case DECLARATION_PARAMETER_n :
	break;
	
    case DECLARATION_SPECIFIERS_0_n :
	break;
	
    case DECLARATION_SPECIFIERS_1_n :
	break;
	
    case DECLARATION_STRUCT_n :
	break;
	
    case DECLARATOR_n :
	break;
	
    case DECLARATORS_DIRECT_ABSTRACT_n :
	break;
	
    case DECLARATORS_INIT_n :
	break;
	
    case DECLARATORS_STRUCT_n :
	break;
	
    case DECLARATOR_ABSTRACT_n :
	break;
	
    case DECLARATOR_ARRAY_n :
	break;
	
    case DECLARATOR_DIRECT_FUNCTION_n :
	break;
	
    case DECLARATOR_FUNCTION_n :
	if (SXVISITED->father->name != DEFINITION_FUNCTION_n &&
	    SXVISITED->father->name != DEFINITION_FUNCTION_NO_SPECIFIER_n &&
	    SXVISITED->father->name != DEFINITION_FUNCTION_NO_DECLARATION_n &&
	    SXVISITED->father->name != DEFINITION_FUNCTION_NO_SPECIFIER_NO_DECLARATION_n) {
	    new_block_level++;
	    sxsymbol_table_close (ids, new_block_level, true);
	}

	break;
	
    case DEFINITION_FUNCTION_n :
	break;
	
    case DEFINITION_FUNCTION_NO_DECLARATION_n :
	break;
	
    case DEFINITION_FUNCTION_NO_SPECIFIER_n :
	break;
	
    case DEFINITION_FUNCTION_NO_SPECIFIER_NO_DECLARATION_n :
	break;
	
    case D_IDENTIFIER_n :
	if (current_struct_level == 0) {
	    bool	is_ok;
	    int		x;
	    struct attribute	*attr;

	    is_ok = sxsymbol_table_put (&ids,
					SXVISITED->token.string_table_entry,
					current_block_level,
					&x);
	    attr = ids_attr + x;

	    if (!is_ok) {
		if (where_is_declaration_specifier == IN_PARAMETER_DECLARATIONS_) {
		    /* Nom existant au meme niveau, c'est obligatoirement un
		       parametre formel. */
		    is_ok = true;
		}
		else if (attr->type_specifier & FUNCTION_DECLARATION_) {
		    /* On peut la redeclarer ou la definir. */
		    if (type_specifier & (FUNCTION_DECLARATION_ | FUNCTION_DEFINITION_))
			is_ok = true;
		}
	    }

	    if (is_ok) {
		if (current_block_level == 0 &&
		    name_id == 0 &&
		    (type_specifier & BASIC_TYPE_RANGE_) == 0)
		    type_specifier |= INT_;

		attr->type_specifier = type_specifier;
		attr->name_id = name_id;
		attr->declarator = declarator_n_ptr;
	    }
	    else {
		/* Deja defini au meme niveau. */
		sxput_error (SXVISITED->token.source_index,
			     attr->type_specifier & TYPEDEF_ ?
			     "%sIllegal redeclaration of a typedef name." :
			     "%sIllegal redeclaration.",
			     sxplocals.sxtables->err_titles [2]);
	    }
	}
	else {
	    /* member definition */
	    bool	is_ok;
	    int		x;
	    struct attribute	*attr;

	    is_ok = sxsymbol_table_put (&members,
					SXVISITED->token.string_table_entry,
					current_struct_level,
					&x);
	    attr = members_attr + x;

	    if (is_ok) {
		attr->type_specifier = type_specifier;
		attr->name_id = name_id;
		attr->declarator = declarator_n_ptr;
	    }
	    else {
		/* Deja defini au meme niveau. */
		sxput_error (SXVISITED->token.source_index,
			     "%sIllegal redeclaration of a member name.",
			     sxplocals.sxtables->err_titles [2]);
	    }
	}

	declarator_n_ptr = NULL;
	break;
	
    case ENUMERATOR_n :
	break;
	
    case ENUMERATORS_n :
	break;
	
    case ENUM_NAME_n :
	break;
	
    case EXTERNAL_DECLARATION_n :
	break;

    case FIELD_n :
	break;
	
    case IDENTIFIER_n :
	if (SXVISITED->father->name == OPERATOR_PTR_n &&
	    SXVISITED->position == 2) {
	    /* name_id -> id */
	    /* The first operand of the "->" operator shall have  type "pointer
	       to qualified or unqualified structure" or "pointer to qualified
	       or unqualified union", and the second operand shall name a member
	       of the type pointed to. */
	    if (operand_type.name_or_type != 0) {
		pattr = GET_ATTR (operand_type.name_or_type);

		if (!operand_type.is_lvalue)
		    operand_type.name_or_type = 0;
		else {
		    if (operand_type.indirection_level + 1 != get_closed_il (&pattr))
			operand_type.name_or_type = 0;
		}

		if (operand_type.name_or_type == 0)
		    sxput_error (SXVISITED->token.source_index,
				 "%sThe first operand of this \"->\" operator is illegal.",
				 sxplocals.sxtables->err_titles [2]);
		else {
		    int struct_level;

		    struct_level = tags_attr [pattr->name_id].level;
		    operand_type.name_or_type = -sxsymbol_table_get_scoped (members,
						    SXVISITED->token.string_table_entry,
						    struct_level);
		    operand_type.indirection_level = 0;

		    if (operand_type.name_or_type == 0)
			sxput_error (SXVISITED->token.source_index,
				     "%sNot a member of a structure or union type.",
				     sxplocals.sxtables->err_titles [2]);

		}
	    }
	}
	else if (SXVISITED->father->name == OPERATOR_DOT_n &&
		 SXVISITED->position == 2) {
	    /* name_id . id */
	    /* The first operand of the "." operator shall have a qualified
	       or unqualified structure or union type, and the second operand
	       shall name a member of that type. */
	    if (operand_type.name_or_type != 0) {
		pattr = GET_ATTR (operand_type.name_or_type);

		if (!operand_type.is_lvalue)
		    operand_type.name_or_type = 0;
		else {
		    if (operand_type.indirection_level != get_closed_il (&pattr))
			operand_type.name_or_type = 0;
		}

		if (operand_type.name_or_type == 0)
		    sxput_error (SXVISITED->token.source_index,
				 "%sThe first operand of this \".\" operator is illegal.",
				 sxplocals.sxtables->err_titles [2]);
		else {
		    int struct_level;

		    struct_level = tags_attr [pattr->name_id].level;
		    operand_type.name_or_type = -sxsymbol_table_get_scoped (members,
						    SXVISITED->token.string_table_entry,
						    struct_level);
		    operand_type.indirection_level = 0;

		    if (operand_type.name_or_type == 0)
			sxput_error (SXVISITED->token.source_index,
				     "%sNot a member of a structure or union type.",
				     sxplocals.sxtables->err_titles [2]);

		}
	    }
	}
	else if (SXVISITED->father->name == TAG_NAME_n) {
	    int x, kind;

	    if (where_is_declaration_specifier == IN_PARAMETERS_ ||
		where_is_declaration_specifier == IN_PARAMETER_DECLARATIONS_ ||
		is_in_type_name) {
		/* struct s doit deja avoir ete de'clare'. */
		x = SXVISITED->token.string_table_entry;
		name_id = sxsymbol_table_get_nested (tags, x);
		    
		if (name_id == 0)
		    sxput_error (SXVISITED->token.source_index,
				 "%sUnknown %s name.",
				 sxplocals.sxtables->err_titles [2],
				 (type_specifier & BASIC_TYPE_RANGE_) == STRUCT_ ?
				 "struct" :
				 "union");
	    }
	    else {
		if (has_declarator) {
		    /* case : struct s declarator;
		       Si "struct s" existe deja, on declare declarator de type struct s.
		       Sinon c'est une declaration (incomplete), le declarator est
		       contraint (la taille de s est inconnue). */
		    x = SXVISITED->token.string_table_entry;
		    name_id = sxsymbol_table_get_nested (tags, x);
		    
		    if (name_id != 0) {
			/* Utilisation du type struct s. */
			/* Il doit y avoir compatibilite struct/union. */
			if ((kind = tags_attr [name_id].kind) !=
			    (type_specifier & BASIC_TYPE_RANGE_))
			    sxput_error (SXVISITED->token.source_index,
					 "%sAlready declared as a %s type.",
					 sxplocals.sxtables->err_titles [2],
					 kind == STRUCT_ ?
					 "struct" :
					 "union");
		    }
		    /* else Rien de visible => declaration */
		}
		else
		    /* case : struct s ; C'est une declaration */
		    name_id = 0;
		
		if (name_id == 0) {
		    name_id = process_tag_name (SXVISITED->token.string_table_entry,
						type_specifier & BASIC_TYPE_RANGE_,
						0, /* declaration */
						&(SXLEFT->token.source_index));
		}
	    }
	}
	else if (SXVISITED->father->name == SPECIFIER_NAMED_STRUCT_n) {
	    name_id = process_tag_name (SXVISITED->token.string_table_entry,
					type_specifier & BASIC_TYPE_RANGE_,
					new_struct_level + 1,
					&(SXLEFT->token.source_index));
	}
	else {
	    int x, type_spec;

	    x = SXVISITED->token.string_table_entry;
	    name_id = sxsymbol_table_get_nested (ids, x);

	    if (((type_spec = ids_attr [name_id].type_specifier) & TYPEDEF_RANGE_) == TYPEDEF_) {
		sxput_error (SXVISITED->token.source_index,
			     type_spec == UNDEF_TYPE_SPECIFIER_ ? "%sNot declared." : "%sShould not be a type name.",
			     sxplocals.sxtables->err_titles [2]);

	    }

	    operand_type.is_lvalue = true;
	    operand_type.name_or_type = name_id;
	    operand_type.indirection_level = 0;
	}

	break;
	
    case IDENTIFIERS_n :
	leave_block();
	break;
	
    case INITIALIZER_n :
	break;
	
    case INITIALIZERS_n :
	/* On remet en place pour le declarator (eventuel) suivant. */
	type_specifier = SS_pop (type_specifier_stack);
	name_id = SS_pop (name_id_stack);
	break;
	
    case OPERATOR_ADDITIVE_n :
	break;
	
    case OPERATOR_ASSIGNMENT_n :
	POP_OTS (); /* On remet la partie gauche. */
	break;
	
    case OPERATOR_BITWISE_n :
	break;
	
    case OPERATOR_CAST_n :
	break;
	
    case OPERATOR_COMMA_n :
	break;
	
    case OPERATOR_CONDITIONAL_n :
	break;
	
    case OPERATOR_DOT_n :
	break;
	
    case OPERATOR_FUNCTION_CALL_n :
	POP_OTS (); /* On remet le nom de la fonction. */
	break;
	
    case OPERATOR_INDEXING_n :
	POP_OTS (); /* Verifier l'index, on remet le tableau. */

	if (operand_type.name_or_type != 0) {
	    pattr = GET_ATTR (operand_type.name_or_type);

	    if (!operand_type.is_lvalue)
		operand_type.name_or_type = 0;
	    else if (++operand_type.indirection_level > 0) {
		if (operand_type.indirection_level > get_closed_il (&pattr))
		    operand_type.name_or_type = 0;
	    }

	    if (operand_type.name_or_type == 0)
		sxput_error (SXLAST_ELEM->token.source_index,
			     "%sIllegal indexing.",
			     sxplocals.sxtables->err_titles [2]);
	}

	break;
	
    case OPERATOR_LOGICAL_n :
	break;
	
    case OPERATOR_MULTIPLICATIVE_n :
	break;
	
    case OPERATOR_POST_DECR_n :
	break;
	
    case OPERATOR_POST_INCR_n :
	break;
	
    case OPERATOR_PRE_DECR_n :
	break;
	
    case OPERATOR_PRE_INCR_n :
	break;
	
    case OPERATOR_PTR_n :
	break;
	
    case OPERATOR_RELATIONAL_n :
	break;
	
    case OPERATOR_SHIFT_n :
	break;
	
    case OPERATOR_SIZEOF_EXPR_n :
	break;
	
    case OPERATOR_SIZEOF_TYPE_n :
	break;
	
    case OPERATOR_UNARY_n :
	check_unary_operator (SXVISITED);
	break;
	
    case PARAMETERS_n :
	leave_block();
	where_is_declaration_specifier = 0;
	break;
	
    case POINTER_n :
	break;
	
    case SPECIFIER_ENUM_n :
	break;
	
    case SPECIFIER_NAMED_ENUM_n :
	break;
	
    case SPECIFIER_NAMED_STRUCT_n :
	type_specifier = SS_pop (type_specifier_stack);
	name_id = SS_pop (name_id_stack);
	quit_struct ();
	break;
	
    case SPECIFIER_QUALIFIERS_0_n :
	break;
	
    case SPECIFIER_QUALIFIERS_1_n :
	break;
	
    case SPECIFIER_STRUCT_n :
	type_specifier = SS_pop (type_specifier_stack);
	name_id = SS_pop (name_id_stack);
	quit_struct ();
	break;
	
    case STMT_n :
	sxatc_sub_tree_erase (SXVISITED);
	break;
	
    case STMT_BREAK_n :
	break;
	
    case STMT_CASE_n :
	break;
	
    case STMT_COMPOUND_n :
	sxsymbol_table_close (ids, current_block_level, true);
	quit_block ();
	break;
	
    case STMT_CONTINUE_n :
	break;
	
    case STMT_DEFAULT_n :
	break;
	
    case STMT_DO_WHILE_n :
	break;
	
    case STMT_FOR_n :
	break;
	
    case STMT_GOTO_n :
	break;
	
    case STMT_IF_THEN_n :
	break;
	
    case STMT_IF_THEN_ELSE_n :
	break;
	
    case STMT_LABELED_n :
	break;
	
    case STMT_RETURN_n :
	break;
	
    case STMT_SWITCH_n :
	break;
	
    case STMT_WHILE_n :
	break;
	
    case TAG_NAME_n :
	break;
	
    case TRANSLATION_UNITS_n :
	break;
	
    case TYPEDEF_NAME_n :
    {
	int x, type_spec;

	x = SXVISITED->token.string_table_entry;
	name_id = sxsymbol_table_get_nested (ids, x);

	if (((type_spec = ids_attr [name_id].type_specifier) & TYPEDEF_) == 0) {
	    name_id = 0;
	    sxput_error (SXVISITED->token.source_index,
			 type_spec == UNDEF_TYPE_SPECIFIER_ ? "%sUndefined type name." : "%sNot a type name.",
			 sxplocals.sxtables->err_titles [2]);

	}
    }

	break;
	
    case TYPE_NAME_n :
	break;
	
    case TYPE_QUALIFIERS_n :
	break;
	
    case VOID_n :
	break;
	
    case character_constant_n :
	break;
	
    case floating_constant_n :
	break;
	
    case integer_constant_n :
	break;
	
    case string_literal_n :
	break;
	
	
	
	/*
	  Z Z Z Z
	  */
	
    default:
	break;
    }
    /* end dc_pd */
}

static void smpopen (sxtables_ptr)
    SXTABLES *sxtables_ptr;
{
    sxatcvar.atc_lv.node_size = sizeof (struct dc_node);

    sxatcvar.atc_lv.early_visit_set = sxba_calloc (string_literal_n + 1);
    SXBA_1_bit (sxatcvar.atc_lv.early_visit_set, EXTERNAL_DECLARATION_n);
    SXBA_1_bit (sxatcvar.atc_lv.early_visit_set, STMT_n);

    sxsymbol_table_alloc (&ids, 200, 200, ids_oflw);
    ids_attr = (struct attribute*) sxalloc (sxsymbol_table_size (ids) + 1, sizeof (struct attribute));
    ids_attr [0].type_specifier = UNDEF_TYPE_SPECIFIER_;

    sxsymbol_table_alloc (&tags, 200, 200, tags_oflw);
    tags_attr = (struct tags_attr*) sxalloc (sxsymbol_table_size (tags) + 1, sizeof (struct tags_attr));

    sxsymbol_table_alloc (&members, 0, 200, members_oflw);
    members_attr = (struct attribute*) sxalloc (sxsymbol_table_size (members) + 1, sizeof (struct attribute));
    members_attr [0].type_specifier = UNDEF_TYPE_SPECIFIER_;

    operand_type_stack = (struct operand_type*)
	sxalloc ((ots_size = 10) + 1, sizeof (struct operand_type));
    ots_top = 0;

    block_level_stack = SS_alloc (20);
    struct_level_stack = SS_alloc (20);
    type_specifier_stack = SS_alloc (10);
    name_id_stack = SS_alloc (10);
    current_struct_level = new_struct_level = current_block_level = new_block_level = 0;

    init_for_types ();
}

static void smppass ()
{
    
    /*   I N I T I A L I S A T I O N S   */
    
    /*   A T T R I B U T E S    E V A L U A T I O N   */
    sxsmp (sxatcvar.atc_lv.abstract_tree_root, dc_pi, dc_pd);
    
    /*   F I N A L I S A T I O N S   */
}

static void smpclose ()
{
    final_for_type ();
    SS_free (name_id_stack);
    SS_free (type_specifier_stack);
    SS_free (struct_level_stack);
    SS_free (block_level_stack);
    sxfree (operand_type_stack);
    sxfree (tags_attr);
    sxfree (members_attr);
    sxfree (ids_attr);
    sxsymbol_table_free (&members);
    sxsymbol_table_free (&tags);
    sxsymbol_table_free (&ids);
    sxfree (sxatcvar.atc_lv.early_visit_set), sxatcvar.atc_lv.early_visit_set = NULL;
}

void dc_smp (what, sxtables_ptr)
    int what;
    SXTABLES *sxtables_ptr;
{
    switch (what) {
    case SXOPEN:
	smpopen (sxtables_ptr);
	break;

    case SXSEMPASS:
	/* Sub-tree evaluation during parsing. */
	sxsmp (sxatcvar.atc_lv.abstract_tree_root, dc_pi, dc_pd);
	break;

    case SXACTION:
	/* smppass (); */
	break;

    case SXCLOSE:
	smpclose ();
	break;
    }
}
