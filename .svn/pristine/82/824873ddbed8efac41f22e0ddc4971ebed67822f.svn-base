/* Ici, aucune action et aucun predicat n'est execute' en // avec l'analyse
   syntaxique non-deterministe. Si le mode est SXPARSER, les predicats retournent
   donc VRAI. */

/* Contrairement aux predicats executes en // avec l'analyse, il ne doit pas
   exister de defaut sur les differentes alternatives (le defaut est toujours
   valide'). Il faut donc des predicats explicites. Ca n'a pas ete fait ici. */


/* Non deterministic parsing is done and the shared parse forest is built.
   This module executes the parsing actions and predicates in order to cut
   this dag into a tree. */

static char ME [] = "Dynamic Typedef Resolution Module";

# include "sxunix.h"
# include "sxba.h"
# include "XxYxZ.h"
# include "XxY.h"
# include "XH.h"
# include "SS.h"
# include "sxnd.h"
# include "varstr.h"
# include "sxsymbol_table.h"


extern int sxgetprdct ();

#define	UNDEF_NAME	0
#define	LABEL_NAME	1
#define	TAG_NAME	2
#define	MEMBER_NAME	3
#define	ID_NAME		4
#define	TYPEDEF_NAME	5

struct palv {
   int		block_level, struct_level; 
   SXBOOLEAN	is_typedef; 
};


static struct palv	palv, *palvs;

static int		palvs_top, palvs_size;

static struct sxsymbol_table_hd identifiers;
static int	*id_kind;


static void identifiers_oflw (old_size, new_size)
    int	old_size, new_size;
{
    id_kind = (int*) sxrealloc (id_kind, new_size + 1, sizeof (int));
}


static SXVOID push_palvs ()
{
    if (++palvs_top > palvs_size)
	palvs = (struct palv*) sxrealloc (palvs, (palvs_size *= 2) + 1, sizeof (struct palv));

    palvs [palvs_top] = palv;
}


#define pop_palvs()	palv = palvs [--palvs_top]

static void parsact (action_nb)
    int action_nb;
{
    int 	x, tok_no;
    SXBOOLEAN	is_ok;

    switch (action_nb) {
    case 1: /* <storage-class-specifier> = @1 typedef ; */
	palv.is_typedef = SXTRUE;
	return;
	
    case 2:
	/*
	  <struct-declaration> = <specifier-qualifier+-0> <struct-declarator+> ";" @2 ;
	  <struct-declaration> = <specifier-qualifier+-1> <struct-declarator+> ";" @2 ;
	  <internal-declaration> = <declaration-specifiers-0> <init-declarator+> ";" @2 ;
	  <internal-declaration> = <declaration-specifiers-1> <init-declarator+> ";" @2 ;
	  <internal-declaration> = <declaration-specifiers-0> ";" @2 ;
	  <internal-declaration> = <declaration-specifiers-1> ";" @2 ;
	  */
	palv.is_typedef = SXFALSE;
	return;
	
    case 3:
    case 9:
	/*
	  <struct-or-union-specifier> = <struct-or-union> <identifier> "{" @9 <struct-declaration+> "}" @10 ;
	  <struct-or-union-specifier> = <struct-or-union> "{" @9 <struct-declaration+> "}" @10 ;
	  <enum-specifier> = enum <identifier> "{" @3 <enumerator+> "}" @4 ;
	  <enum-specifier> = enum "{" @3 <enumerator+> "}" @4 ;
	  */
	push_palvs ();
	palv.is_typedef = SXFALSE;
	
	if (action_nb == 9)
	    palv.struct_level++;
	
	return;
	
    case 6:
	/*
	  <parameters> = "(" @6 <parameter-type+> ")" @7 ;
	  <parameters> = "(" @6 <identifier+> ")" @7 ;
	  <statement> = @6 <compound-statement> @11 ;
	  <function-definition> = <declaration-specifiers-0> <function-declarator> @6 <declaration+> @7 <compound-statement> ;
	  <function-definition> = <declaration-specifiers-1> <function-declarator> @6 <declaration+> @7 <compound-statement> ;
	  <function-definition> = <function-declarator> @6 <declaration+> @7 <compound-statement> ;
	  */	    
	push_palvs ();
	palv.block_level++;
	sxsymbol_table_open (identifiers, palv.block_level);
	return;
	
    case 4:
    case 10:
	/*
	  <struct-or-union-specifier> = <struct-or-union> <identifier> "{" @9 <struct-declaration+> "}" @10 ;
	  <struct-or-union-specifier> = <struct-or-union> "{" @9 <struct-declaration+> "}" @10 ;
	  <enum-specifier> = enum <identifier> "{" @3 <enumerator+> "}" @4 ;
	  <enum-specifier> = enum "{" @3 <enumerator+> "}" @4 ;
	  */
	pop_palvs ();
	return;
	
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
	pop_palvs ();
	
	if (action_nb == 7)
	    return;
	
    case 8:
	/*
	  <init-declarator> = <function-declarator> @8 ;
	  <struct-declarator> = <function-declarator> @8 ;
	  <parameter-declaration> = <declaration-specifiers-0> <function-declarator> @8 ;
	  <parameter-declaration> = <declaration-specifiers-1> <function-declarator> @8 ;
	  */	    
	sxsymbol_table_close (&identifiers, palv.block_level + 1, SXTRUE);
	return;
	
    case 5: /* <d-identifier> = @5 %identifier ; */
	if (palv.struct_level == 0) {
	    tok_no = -sxndtw_item_to_symb (sxndtw_symb_to_lhsitem (NDFATHER_SYMBOL) + 2);
	    is_ok = sxsymbol_table_put (&identifiers,
					SXGET_TOKEN (tok_no).string_table_entry,
					palv.block_level,
					&x);
	    
	    if (is_ok)
		id_kind [x] = palv.is_typedef ? TYPEDEF_NAME : ID_NAME;
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
	
	return;
	
    default:
	break;
    }
}

static SXBOOLEAN parsprdct (prdct)
    int prdct;
{
    int x, tok_no;

    switch (prdct) {
    case 1: /* <typedef-name> = %identifier &1 ; */
	tok_no = -NDVISITED_SYMBOL;
	x = SXGET_TOKEN (tok_no).string_table_entry;
	return id_kind [sxsymbol_table_get_nested (identifiers, x)] == TYPEDEF_NAME;
	
    default:
	break;
    }
}

static int pass_derived ()
{
    /* On parcourt la parse forest en executant les actions et les predicats. */
    /* Si un predicat retourne faux, on abandonne et on invalide les regles
       imbriquees jusqu'a remonter sur un hook ou la racine. Si tous les sous-arbres
       d'un hook sont invalides, on continue la remontee/destruction. */
    int		prdct, visited_reduce_nb;

    if (NDVISITED_SYMBOL > 0) {
	visited_reduce_nb = sxndtw_symb_to_red_no (NDVISITED_SYMBOL);

	if (visited_reduce_nb > sxplocals.SXP_tables.P_nbpro)
	    parsact (parse_stack.semact [visited_reduce_nb]);
    }

    if ((prdct = sxgetprdct (NDFATHER_REDNO, NDVISITED_POSITION)) >= 0)
	return parsprdct (prdct) ? 0 : 1;

    return 0 /* go on */;
}


static void open_hook ()
{
    push_palvs ();
    palv.block_level++;
    sxsymbol_table_open (identifiers, palv.block_level);
}

static void close_hook ()
{
    sxsymbol_table_close (&identifiers, palv.block_level, SXTRUE);
    pop_palvs ();
}



int ndc_parsact (entry, action_nb)
    int entry, action_nb;
{
    switch (entry) {
    case SXOPEN:
	palvs = (struct palv*) sxalloc ((palvs_size = 100) + 1, sizeof (struct palv));
	sxsymbol_table_alloc (&identifiers, 200, 200, identifiers_oflw);
	id_kind = (int*) sxalloc (sxsymbol_table_size (identifiers) + 1, sizeof (int));
	id_kind [0] = UNDEF_NAME;
	return 1;

    case SXCLOSE:
	sxfree (palvs);
	sxfree (id_kind);
	sxsymbol_table_free (&identifiers);
	return 1;

    case SXINIT:
	palvs_top = 0;
	palv.is_typedef = SXFALSE;
	palv.struct_level = palv.block_level = 0;

	sxndtw_open (NULL /* pass_inherited */,
		     pass_derived,
		     NULL /* cycle_processing */,
		     open_hook,
		     close_hook,
		     NULL /* nodes_oflw */);

	return 1;

    case SXFINAL:
	return 1;

    case SXPREDICATE:
    case SXACTION:
	return 1;

    default:
	break;
    }

    fputs ("The function \"ndc_parsact\" is out of date.\n", sxstderr);
    abort ();
}
