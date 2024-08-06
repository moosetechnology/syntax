static char ME [] = "Static Typedef Resolution Module";

# include "sxunix.h"
# include "sxba.h"
# include "XxYxZ.h"
# include "XxY.h"
# include "XH.h"
# include "SS.h"
# include "sxnd.h"
# include "varstr.h"
# include "sxsymbol_table.h"

#define	UNDEF_NAME	0
#define	LABEL_NAME	1
#define	TAG_NAME	2
#define	MEMBER_NAME	3
#define	ID_NAME		4
#define	TYPEDEF_NAME	5

static bool	is_typedef;
static SXBA	stack;
static int	stack_size, block_level, struct_level;

static struct sxsymbol_table_hd identifiers;
static int	*id_kind;


static void stack_oflw ()
{
    stack = sxba_resize (stack, (stack_size *= 2) + 1);
}

static void identifiers_oflw (old_size, new_size)
    int	old_size, new_size;
{
    id_kind = (int*) sxrealloc (id_kind, new_size + 1, sizeof (int));
}


bool dc_parsact (SXINT entry, SXINT action_nb)
{
    int x;
    bool is_ok;

    switch (entry) {
    case SXOPEN:
	stack = sxba_calloc ((stack_size = 100) + 1);
	sxsymbol_table_alloc (&identifiers, 200, 200, identifiers_oflw);
	id_kind = (int*) sxalloc (sxsymbol_table_size (identifiers) + 1, sizeof (int));
	id_kind [0] = UNDEF_NAME;
	return SXANY_BOOL;

    case SXCLOSE:
	sxfree (stack);
	sxfree (id_kind);
	sxsymbol_table_free (&identifiers);
	return SXANY_BOOL;

    case SXINIT:
	is_typedef = false;
	struct_level = block_level = 0;
	return SXANY_BOOL;

    case SXFINAL:
	return SXANY_BOOL;

    case SXPREDICATE:
	switch (action_nb) {
	case 1: /* <typedef-name> = %identifier &1 ; */
	    x = SXGET_TOKEN (sxplocals.ptok_no).string_table_entry;
	    return id_kind [sxsymbol_table_get_nested (identifiers, x)] == TYPEDEF_NAME;

	default:
	    break;
	}

	break;;

    case SXACTION:
	switch (action_nb) {
	case 1: /* <storage-class-specifier> = @1 typedef ; */
	    is_typedef = true;
	    return SXANY_BOOL;

	case 2:
/*
<struct-declaration> = <specifier-qualifier+-0> <struct-declarator+> ";" @2 ;
<struct-declaration> = <specifier-qualifier+-1> <struct-declarator+> ";" @2 ;
<internal-declaration> = <declaration-specifiers-0> <init-declarator+> ";" @2 ;
<internal-declaration> = <declaration-specifiers-1> <init-declarator+> ";" @2 ;
<internal-declaration> = <declaration-specifiers-0> ";" @2 ;
<internal-declaration> = <declaration-specifiers-1> ";" @2 ;
*/
	    is_typedef = false;
	    return SXANY_BOOL;

	case 3:
	case 9:
/*
<struct-or-union-specifier> = <struct-or-union> <identifier> "{" @9 <struct-declaration+> "}" @10 ;
<struct-or-union-specifier> = <struct-or-union> "{" @9 <struct-declaration+> "}" @10 ;
<enum-specifier> = enum <identifier> "{" @3 <enumerator+> "}" @4 ;
<enum-specifier> = enum "{" @3 <enumerator+> "}" @4 ;
*/
	    x = SXSTACKnewtop ();

	    while (x > stack_size )
		stack_oflw ();

	    is_typedef ? SXBA_1_bit (stack, x) : SXBA_0_bit (stack, x);
	    is_typedef = false;

	    if (action_nb == 9)
		struct_level++;

	    return SXANY_BOOL;

	case 6:
/*
<parameters> = "(" @6 <parameter-type+> ")" @7 ;
<parameters> = "(" @6 <identifier+> ")" @7 ;
<statement> = @6 <compound-statement> @11 ;
<function-definition> = <declaration-specifiers-0> <function-declarator> @6 <declaration+> @7 <compound-statement> ;
<function-definition> = <declaration-specifiers-1> <function-declarator> @6 <declaration+> @7 <compound-statement> ;
<function-definition> = <function-declarator> @6 <declaration+> @7 <compound-statement> ;
*/	    
	    block_level++;
	    sxsymbol_table_open (identifiers, block_level);
	    return SXANY_BOOL;

	case 4:
	case 10:
/*
<struct-or-union-specifier> = <struct-or-union> <identifier> "{" @9 <struct-declaration+> "}" @10 ;
<struct-or-union-specifier> = <struct-or-union> "{" @9 <struct-declaration+> "}" @10 ;
<enum-specifier> = enum <identifier> "{" @3 <enumerator+> "}" @4 ;
<enum-specifier> = enum "{" @3 <enumerator+> "}" @4 ;
*/
	    x = SXSTACKnewtop () - 3;
	    is_typedef = SXBA_bit_is_set (stack, x);

	    if (action_nb == 10)
		struct_level--;

	    return SXANY_BOOL;

	case 7:
	case 11:
/*
<parameters> = "(" @6 <parameter-type+> ")" @7 ;
<parameters> = "(" @6 <identifier+> ")" @7 ;
<statement> = @6 <compound-statement> @11 ;
<function-definition> = <declaration-specifiers-0> <function-declarator> @6 <declaration+> @7 <compound-statement> ;
<function-definition> = <declaration-specifiers-1> <function-declarator> @6 <declaration+> @7 <compound-statement> ;
<function-definition> = <function-declarator> @6 <declaration+> @7 <compound-statement> ;
*/	    
	    block_level--;

	    if (action_nb == 7)
		return SXANY_BOOL;
	    
	case 8:
/*
<init-declarator> = <function-declarator> @8 ;
<struct-declarator> = <function-declarator> @8 ;
<parameter-declaration> = <declaration-specifiers-0> <function-declarator> @8 ;
<parameter-declaration> = <declaration-specifiers-1> <function-declarator> @8 ;
*/	    
	    sxsymbol_table_close (&identifiers, block_level + 1, true);
	    return SXANY_BOOL;

	case 5: /* <d-identifier> = @5 %identifier ; */
	    if (struct_level == 0) {
		is_ok = sxsymbol_table_put (&identifiers,
					    SXGET_TOKEN (sxplocals.atok_no).string_table_entry,
					    block_level,
					    &x);
		
		if (is_ok)
		    id_kind [x] = is_typedef ? TYPEDEF_NAME : ID_NAME;
#if 0
/* Traite au cours de la passe semantique. */
		else {
		    /* Deja defini au meme niveau. */
		    if (id_kind [x] == TYPEDEF_NAME || is_typedef)
			sxput_error (SXGET_TOKEN (sxplocals.atok_no).source_index,
				     "%sAlready defined.",
				     sxplocals.sxtables->err_titles [2]);
		}
#endif
	    }
	    
	    return SXANY_BOOL;

	default:
	    break;
	}

	break;;

    default:
	return SXANY_BOOL;
    }

    fputs ("The function \"dc_parsact\" is out of date.\n", sxstderr);
    abort ();
}
