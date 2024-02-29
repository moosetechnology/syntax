static char ME [] = "Typedef resolution module";

# include "sxunix.h"
# include "sxba.h"
# include "XxY.h"
# include "XxYxZ.h"
# include "XH.h"
# include "SS.h"
# include "sxnd.h"
# include "varstr.h"
# include "sxsymbol_table.h"

#define enter_block(b)	SS_push (levels_stack, current_block_level), current_block_level = b
#define quit_block()	current_block_level = SS_pop (levels_stack)

#define	UNDEF_NAME	0
#define	LABEL_NAME	1
#define	TAG_NAME	2
#define	MEMBER_NAME	3
#define	ID_NAME		4
#define	TYPEDEF_NAME	5



static int	current_block_level, new_block_level, *levels_stack, *id_stack;

static int	name_def_kind, sub_tree_root;
static bool	is_a_typedef_spec, is_a_function_definition, is_a_function_body,
    is_a_typedef_struct;


static struct sxsymbol_table_hd identifiers;

static int	*id_kind;

static struct attributes {
    int		name_def_kind, sub_tree_root;
    bool	is_a_function_body;
} *attributes;
static int attributes_size, attributes_top;

static void identifiers_oflw (old_size, new_size)
    int	old_size, new_size;
{
    id_kind = (int*) sxrealloc (id_kind, new_size + 1, sizeof (int));
}


static void alloc_attributes (size)
    int size;
{
    attributes = (struct attributes*) sxalloc ((attributes_size = size) + 1,
					       sizeof (struct attributes));
    attributes_top = 0;
}

static void oflw_attributes ()
{
    attributes = (struct attributes*) sxrealloc (attributes,
						 (attributes_size *= 2) + 1,
						 sizeof (struct attributes));
}

static void free_attributes ()
{
    sxfree (attributes);
}

static void push_attributes ()
{
    struct attributes *pattr;

    if (++attributes_top > attributes_size)
	oflw_attributes ();

    pattr = attributes + attributes_top;
    pattr->name_def_kind = name_def_kind;
    pattr->sub_tree_root = sub_tree_root;
    pattr->is_a_function_body = is_a_function_body;
}

static void pop_attributes ()
{
    struct attributes *pattr;

    pattr = attributes + attributes_top--;
    name_def_kind = pattr->name_def_kind;
    sub_tree_root = pattr->sub_tree_root;
    is_a_function_body = pattr->is_a_function_body;
}

static void get_attributes ()
{
    struct attributes *pattr;

    pattr = attributes + attributes_top;
    name_def_kind = pattr->name_def_kind;
    sub_tree_root = pattr->sub_tree_root;
    is_a_function_body = pattr->is_a_function_body;
}

static void lgjmp ()
{
    /* Abandon du sous-arbre (hook) courant.
       On supprime les id qui y ont ete declares.
       On repositionne les niveaux. */
    new_block_level = SS_pop (id_stack);
    current_block_level = SS_pop (id_stack);

    while (!SS_is_empty (id_stack)) {
	sxsymbol_table_erase (identifiers, new_block_level, true);
	new_block_level = current_block_level;
	current_block_level = SS_pop (id_stack);
    }

    while (!SS_is_empty (levels_stack) &&
	   SS_get_top (levels_stack) >= current_block_level)
	SS_decr (levels_stack);
    
    sxndtw_node_erase (sub_tree_root);
    sxndtw.nv.node = sub_tree_root;
    sxndtw.nv.kind = SXDERIVED;
}



static void pass_inherited ()
{
    int 		reduce_nb, father_rule, visited_rule;
    int 		id, i, son, kind, lhs_item;
    struct sxtoken	*atok;

    father_rule = NDFATHER_SYMBOL;
    reduce_nb = NDFATHER_REDNO;

    if (father_rule >= parse_stack.hook_rule ) {
	SS_open (id_stack);
	SS_push (id_stack, current_block_level);
	SS_push (id_stack, new_block_level);
	get_attributes ();
	
	/* On regarde si le sous-arbre courant pourra etre invalide', on
	   en conserve au moins un. */
	if (sub_tree_root == 0 /* aucune autre possibilite */ &&
	    NDVISITED_POSITION == sxndtw_symb_to_arity (father_rule) /* Dernier fils */ ) {
	    lhs_item = sxndtw_symb_to_lhsitem (father_rule);

	    for (i = 1; i < NDVISITED_POSITION; i++) {
		sxndtw_node_to_son (NDFATHER, lhs_item + i, son);
		
		if (!sxndtw_is_node_erased (son)) {
		    i = 0;
		    break;
		}
	    }
	    
	    /* i > 0 => tous les freres de gauche sont elimines. */
	}
	else
	    i = 0;
	
	sub_tree_root = i == 0 ? NDVISITED : 0;
    }
    else {
	/* Normal inherited pass. */
	switch (reduce_nb) {

	case 85: /* <storage-class-specifier> = typedef ; */
	    is_a_typedef_spec = true;
	    break;

	case 101: /* <struct-or-union-specifier> = <struct-or-union> <d-identifier> "{" <struct-declaration+> "}" ; */
	    if (NDVISITED_POSITION == 2) {
		is_a_typedef_struct = is_a_typedef_spec;
		name_def_kind = TAG_NAME;
	    }
	    else if (NDVISITED_POSITION == 3) {
		name_def_kind = MEMBER_NAME;
	    }
	    else if (NDVISITED_POSITION == 5) {
		    is_a_typedef_spec = is_a_typedef_struct;
		}

	    break;


	case 102: /* <struct-or-union-specifier> = <struct-or-union> "{" <struct-declaration+> "}" ; */
	    if (NDVISITED_POSITION == 2) {
		is_a_typedef_struct = is_a_typedef_spec;
	    }
	    else if (NDVISITED_POSITION == 3) {
		name_def_kind = MEMBER_NAME;
	    }
	    else if (NDVISITED_POSITION == 4) {
		    is_a_typedef_spec = is_a_typedef_struct;
		}

	    break;


	case 103: /* <struct-or-union-specifier> = <struct-or-union> <d-identifier> ; */
	    if (NDVISITED_POSITION == 2) {
		name_def_kind = TAG_NAME;
	    }

	    break;

	case 100: /* <struct-declaration> = <specifier-qualifier+-0> <struct-declarator+> ";" ; */
	case 109: /* <struct-declaration> = <specifier-qualifier+-1> <struct-declarator+> ";" ; */
	    switch (NDVISITED_POSITION) {
	    case 1:
		is_a_typedef_spec = false;
		break;

	    case 2:
		name_def_kind = MEMBER_NAME;
		break;

	    case 3:
		is_a_typedef_spec = false;
		break;
	    }

	    break;


	case 118: /* <struct-declarator+> = <struct-declarator+> "," <struct-declarator> ; */
	    if (NDVISITED_POSITION == 2) {
		name_def_kind = MEMBER_NAME;
	    }

	    break;

	case 123: /* <enum-specifier> = enum <d-identifier> "{" <enumerator+> "}" ; */
	    if (NDVISITED_POSITION == 2) {
		name_def_kind = TAG_NAME;
	    }

	    break;

	case 125: /* <enum-specifier> = enum <d-identifier> ; */
	    if (NDVISITED_POSITION == 2) {
		name_def_kind = TAG_NAME;
	    }

	    break;

	case 138: /* <parameters> = "(" ")" ; */
	    if (is_a_function_definition && NDVISITED_POSITION == 1) {
		enter_block (++new_block_level);
		sxsymbol_table_open (identifiers, current_block_level);
	    }

	    break;

	case 139: /* <parameters> = "(" <parameter-type+> ")" ; */
	case 140: /* <parameters> = "(" <identifier+> ")" ; */
	    if (NDVISITED_POSITION == 1) {
		enter_block (++new_block_level);
		sxsymbol_table_open (identifiers, current_block_level);
		is_a_typedef_spec = false;
	    }
	    else if (NDVISITED_POSITION == 3) {
		if (!is_a_function_definition) {
		    sxsymbol_table_close (identifiers, current_block_level, true);
		    quit_block ();
		}

		is_a_typedef_spec = false;
	    }

	    break;

	case 151: /* <parameter-declaration> = <declaration-specifiers-0> <declarator> ; */
	case 152: /* <parameter-declaration> = <declaration-specifiers-1> <declarator> ; */
	case 153: /* <parameter-declaration> = <declaration-specifiers-0> <function-declarator> ; */
	case 154: /* <parameter-declaration> = <declaration-specifiers-1> <function-declarator> ; */
	case 155: /* <parameter-declaration> = <declaration-specifiers-0> <[abstract-declarator]> ; */
	case 156: /* <parameter-declaration> = <declaration-specifiers-1> <[abstract-declarator]> ; */
	    if (NDVISITED_POSITION == 2) {
		is_a_typedef_spec = false;
	    }

	    break;

	case 172: /* <typedef-name> = %identifier ; */
	    atok = &(SXGET_TOKEN (-NDVISITED_SYMBOL));
	    kind = id_kind [sxsymbol_table_get_nested (identifiers, atok->string_table_entry)];
	    
	    if (kind != TYPEDEF_NAME) {
		if (sub_tree_root != 0) {
		    lgjmp ();
		}
		else
		    sxput_error (atok->source_index,
				 "%sNot a typedef name.",
				 sxplocals.sxtables->err_titles [2]);
		
	    }

	    break;

	case 178: /* <identifier> = %identifier ; */
	    atok = &(SXGET_TOKEN (-NDVISITED_SYMBOL));
	    kind = id_kind [sxsymbol_table_get_nested (identifiers, atok->string_table_entry)];

	    if (kind == TYPEDEF_NAME) {
		if (sub_tree_root != 0) {
		    lgjmp ();
		    }
		else
		    sxtrap (ME, "Strange occurrence of a typedef name.");

	    }

	    break;

	case 179: /* <d-identifier> = %identifier ; */
	    if (name_def_kind == UNDEF_NAME)
		sxtrap (ME, "Undef name in a definition");

	    if (name_def_kind == ID_NAME) {
		int x;
		bool is_ok;

		atok = &(SXGET_TOKEN (-NDVISITED_SYMBOL));
		is_ok = sxsymbol_table_put (&identifiers,
					    atok->string_table_entry,
					    current_block_level,
					    &x);

		if (is_ok) {
		    /* New id */
		    id_kind [x] = is_a_typedef_spec ? TYPEDEF_NAME : ID_NAME;
		    SS_push (id_stack, x);
		}
		else {
		    /* An id at the same block level has already been entered. */
		    kind = id_kind [x];

		    if (kind == TYPEDEF_NAME) {
			if (sub_tree_root != 0) {
			    lgjmp ();
			}
			else
			    sxput_error (atok->source_index,
					 "%sAlready defined in the same scope.",
					 sxplocals.sxtables->err_titles [2]);
		    }
		    /* else redefinition supposee correcte... */
		}
	    }
	    else
		name_def_kind = ID_NAME;

	    break;

	case 189: /* <compound-statement> = "{" <declaration*-and-statement+> "}" ; */
	case 190: /* <compound-statement> = "{" <declaration+> "}" ; */
	    if (NDVISITED_POSITION == 1) {
		if (is_a_function_body)
		    is_a_function_body = false;
		else {
		    enter_block (++new_block_level);
		    sxsymbol_table_open (identifiers, current_block_level);
		}
	    }
	    else if (NDVISITED_POSITION == 3) {
		sxsymbol_table_close (identifiers, current_block_level, true);
		quit_block ();
	    }

	    break;

	case 191: /* <compound-statement> = "{" "}" ; */
	    if (NDVISITED_POSITION == 1) {
		if (is_a_function_body)
		    is_a_function_body = false;
		else {
		    enter_block (++new_block_level);
		    sxsymbol_table_open (identifiers, current_block_level);
		}
	    }
	    else if (NDVISITED_POSITION == 2) {
		sxsymbol_table_close (identifiers, current_block_level, true);
		quit_block ();
	    }

	    break;

	case 194: /* <internal-declaration> = <declaration-specifiers-0> <init-declarator+> ";" ; */
	case 195: /* <internal-declaration> = <declaration-specifiers-1> <init-declarator+> ";" ; */
	    if (NDVISITED_POSITION == 3) {
		is_a_typedef_spec = false;
	    }

	    break;

	case 196: /* <internal-declaration> = <declaration-specifiers-0> ";" ; */
	case 197: /* <internal-declaration> = <declaration-specifiers-1> ";" ; */
	    if (NDVISITED_POSITION == 2) {
		is_a_typedef_spec = false;
	    }

	    break;

	case 219: /* <function-definition> = <declaration-specifiers-0> <function-declarator> <declaration+> <compound-statement> ; */
	case 220: /* <function-definition> = <declaration-specifiers-1> <function-declarator> <declaration+> <compound-statement> ; */
	case 222: /* <function-definition> = <declaration-specifiers-0> <function-declarator> <compound-statement> ; */
	case 223: /* <function-definition> = <declaration-specifiers-1> <function-declarator> <compound-statement> ; */
	    switch (NDVISITED_POSITION) {
	    case 1:
		if (current_block_level != 0)
		    sxtrap (ME, "Block level error");

		is_a_typedef_spec = false;
		break;

	    case 2:
		is_a_function_definition = true;
		break;

	    case 3:
		is_a_typedef_spec = false;
		is_a_function_definition = false;
		is_a_function_body = true;
		break;
	    }

	    break;

	case 221: /* <function-definition> = <function-declarator> <declaration+> <compound-statement> ; */
	case 224: /* <function-definition> = <function-declarator> <compound-statement> ; */
	    if (NDVISITED_POSITION == 1) {
		if (current_block_level != 0)
		    sxtrap (ME, "Block level error");

		is_a_function_definition = true;
	    }
	    else {
		is_a_typedef_spec = false;
		is_a_function_definition = false;
		is_a_function_body = true;
	    }

	    break;

	default:
	    break;
	}
    }

    visited_rule = NDVISITED_SYMBOL;

    if (visited_rule >= parse_stack.hook_rule ) {
	push_attributes ();
    }
}



static void pass_derived ()
{
    int			rule;

    if (NDVISITED_SYMBOL <= 0) {
	/* On a terminal symbol */
    }
    else {
	if (NDFATHER_SYMBOL >= parse_stack.hook_rule) {
	    SS_close (id_stack);
	}

	rule = NDVISITED_SYMBOL;

	if (rule >= parse_stack.hook_rule) {
	    /* Hook processing */ 
	    pop_attributes ();	
	}
	else {
	    /* Normal derived pass. */
	}
    }
}


void c_desambig (kind)
    int kind;
{
    switch (kind) {
    case SXOPEN:
	levels_stack = SS_alloc (10);
	id_stack = SS_alloc (10);
	sxndtw_open (pass_inherited, pass_derived, NULL, NULL, NULL, NULL);
	sxsymbol_table_alloc (&identifiers, 2, 2, identifiers_oflw); /* test de l'overflow. */
	id_kind = (int*) sxalloc (sxsymbol_table_size (identifiers) + 1, sizeof (int));
	id_kind [0] = UNDEF_NAME;
	alloc_attributes (10);
	break;

    case SXINIT:
	sxndtw_init ();
	break;

    case SXACTION:
	current_block_level = new_block_level = 0;
	name_def_kind = ID_NAME;
	is_a_typedef_spec = false;
	sxndtw_walk ();

	if (current_block_level != 0)
	    sxtrap (ME, "Block level error");

	sxndtw_check_grammar ();

	if (parse_stack.is_ambiguous) {
	    sxtmsg (sxsrcmngr.source_coord.file_name,
		    "%sDesambiguation phase failed.",
		    sxplocals.sxtables->err_titles [2]);
	}

	break;

    case SXFINAL:
	sxndtw_final ();
	break;

    case SXCLOSE:
	sxfree (id_kind);
	sxsymbol_table_free (&identifiers);
	SS_free (levels_stack);
	SS_free (id_stack);
	free_attributes ();
	sxndtw_close ();
	break;
    }
}
