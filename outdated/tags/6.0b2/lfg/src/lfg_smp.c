/* ********************************************************************
   *  This program has been generated from lfg.at                     *
   *  on Fri Nov 17 11:27:04 2006                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */

static char     ME [] = "lfg_smp";

/* Les valeurs atomiques sont qcq et on peut faire de la disjonction sur les "..." */
#define ESSAI

/*   I N C L U D E S   */
#define NODE struct lfg_node
#include "varstr.h"
#include "sxunix.h"
#include <ctype.h>
char WHAT_LFGSMP[] = "@(#)SYNTAX - $Id: lfg_smp.c 651 2007-06-21 15:40:36Z syntax $" WHAT_DEBUG;

 /* definies ds le main */
extern     FILE              *bnf_file, *vocabulary_file;
extern     char              *bnf_file_name, *vocabulary_file_name, *path_name, *prgent_name, *language_name;
extern     unsigned long int lfg_time;

struct lfg_node {
  SXNODE_HEADER_S VOID_NAME;

  /*
    your attribute declarations...
  */
  int id;
  unsigned char kind;
};

/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ADD_OPER_n 2
#define AMALGAM_n 3
#define AMALGAM_S_n 4
#define ARGUMENT_n 5
#define ARGUMENT_S_n 6
#define ATOM_CONCAT_n 7
#define ATOM_MINUS_n 8
#define ATOM_PLUS_n 9
#define ATOM_S_n 10
#define ATTRIBUTE_n 11
#define ATTRIBUTE_PASS_NB_n 12
#define ATTRIBUTE_S_n 13
#define ATTR_ARG_STRUCT_n 14
#define ATTR_ATOMS_n 15
#define ATTR_DOTS_n 16
#define ATTR_SET_OF_STRUCT_n 17
#define ATTR_STRUCT_n 18
#define ATTR_VAL_n 19
#define ATTR_VAL_S_n 20
#define CATEGORY_n 21
#define CHAINE_n 22
#define COMPLETE_n 23
#define COMPOUND_n 24
#define COMPOUND_S_n 25
#define CONTRAINTE_OPER_n 26
#define DANS_OPER_n 27
#define DECLARATION_S_n 28
#define DISJONCTION_S_n 29
#define EMPTY_ADJUNCT_n 30
#define EMPTY_ATOM_n 31
#define EMPTY_NAMED_ADJUNCT_n 32
#define EMPTY_NAMED_ATOM_n 33
#define EMPTY_NAMED_STRUCT_n 34
#define EMPTY_SOUS_CAT_n 35
#define EMPTY_STRUCT_n 36
#define ENTIER_n 37
#define EQUATION_S_n 38
#define ETIQUETTE_n 39
#define EXISTE_n 40
#define FIELD_n 41
#define FIELD_CONSTRAINT_n 42
#define FIELD_EQUAL_n 43
#define FIELD_OPTIONAL_n 44
#define IDENT_n 45
#define IDENT_S_n 46
#define KLEENE_n 47
#define LEXEME_NODE_REF_n 48
#define LEX_NODE_REF_n 49
#define LFG_n 50
#define MACRO_n 51
#define MACRO_NAME_n 52
#define MACRO_STRUCTURE_n 53
#define MOT_1COL_n 54
#define NAMED_ADJUNCT_n 55
#define NAMED_ATOM_n 56
#define NAMED_STRUCTURE_n 57
#define NODE_REF_n 58
#define NON_n 59
#define NONEXISTE_n 60
#define OPTION_OPER_n 61
#define OPT_DISJONCTION_n 62
#define OPT_FIELD_n 63
#define OPT_NON_TERMINAL_n 64
#define OU_S_n 65
#define PAIR_n 66
#define PAIR_S_n 67
#define POSSIBLE_n 68
#define PRED_n 69
#define PRED_FIELD_n 70
#define RULE_n 71
#define RULES_S_n 72
#define SOUS_CAT_n 73
#define STRUCTURE_S_n 74
#define TERME_CHAMP_n 75
#define TERME_CHAMP_RE_n 76
#define TERME_CHEMIN_n 77
#define TERME_CHEMIN_DOT_n 78
#define TERME_CHEMIN_RE_n 79
#define TERME_CHEMIN_RE_DOT_n 80
#define TERMINAL_n 81
#define UNIFIER_n 82
#define UNIF_OPER_n 83
#define VOCABULARY_S_n 84
#define VOID_n 85
/*
  E N D   N O D E   N A M E S
*/

#include "sxword.h"
#include "XH.h"
#include "XxY.h"

static int                FIELDS_nb, RULES_nb, EQUATION_nb;

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */

static struct sxtables *sxtab_ptr;
static sxword_header   field_names, atom_names, suffix_names, complete_names;
static int             *complete_name2ste;
/* Si la valeur d'un champ atomique unbounded de nom "champ" (donc declare' par champ = {Aij, Pij ...})
   prend la valeur Aij ou Pij (par une equation comme $$ champ = Aij), alors l'atom prend la valeur de la
   lhs de la production $$ si Aij ou le numero de la prod instanciee si Pij */
static int             ste_Aij, ste_Pij, atom_Aij, atom_Pij;
static unsigned char   *field_id2kind;
static SXBA            argument_set;
static XH_header       compound_fields_hd;
static int             pred_id, max_field_id, max_atom_id, rhs_length;
static SXBA            *field_id2atom_set, working_atom_set;
static int             current_field_id;
static VARSTR          vstr;

#ifndef ESSAI
static int             max_local_atom_id;
static int             **fieldXatom2local_id, **fieldXlocal2atom_id, *local_id_area, *local_id_area2;
#else /* ESSAI */
static XH_header       atom_hd; 
#endif /* ESSAI */
static int             last_lhs_atom_field_id;

static SXBA            working_field_set, used_field_set;
static int             maxrhs;

static XxY_header      dfield_pairs,datom_pairs;
static int             *dfield_pairs2field_id,*datom_pairs2atom_id;

static int             doli;


#define ERROR_KIND             0
/* Structure statique du "kind" d'un field */
#define ATOM_KIND              1
#define STRUCT_KIND            2
#define UNBOUNDED_KIND         4
#define Aij_KIND               8

/* Structure dynamique du "kind" d'un field */
#define ASSIGN_CONSTRAINT_KIND 1

#define STRUCTURE_NAME_shift   1

#if 0
#define OPTIONAL_FIELD         1
#define ASSIGN_CONSTRAINT_KIND (1<<1)

#define STRUCTURE_NAME_shift   2
#endif /* 0 */

#define MAX_PRIORITY 0x7fffffff

static SXBA          working_compound_field_set, working2_compound_field_set;

static BOOLEAN       is_OPERATOR_LEXEME_REF, is_OPERATOR_LEX_REF;

static void
atom_names_oflw (old_size, new_size)
     int old_size, new_size;
{
  field_id2atom_set = sxbm_resize (field_id2atom_set, FIELDS_nb+1, FIELDS_nb+1, new_size+1);
  working_atom_set = sxba_resize (working_atom_set, new_size+1);
}

static void
compound_fields_oflw (old_size, new_size)
     int old_size, new_size;
{
  if (working_compound_field_set) {
    working_compound_field_set = sxba_resize (working_compound_field_set, max_field_id+new_size+1);
    working2_compound_field_set = sxba_resize (working2_compound_field_set, max_field_id+new_size+1);
  }
}


static void
dfield_pairs_oflw (old_size, new_size)
     int		old_size, new_size;
{
  dfield_pairs2field_id = (int*) sxrealloc (dfield_pairs2field_id, new_size+1, sizeof (int));
}

static void
datom_pairs_oflw (old_size, new_size)
     int		old_size, new_size;
{
  datom_pairs2atom_id = (int*) sxrealloc (datom_pairs2atom_id, new_size+1, sizeof (int));
}


static void
fill_leaves_field_kind (visited)
     NODE *visited;
{
  int field_id;

  switch (visited->name) {
  case CHAINE_n:
  case ENTIER_n:
  case IDENT_n: 
    field_id = sxword_2retrieve (&field_names,
				 sxstrget (visited->token.string_table_entry),
				 sxstrlen (visited->token.string_table_entry));
    break;
  case PRED_n: 
    field_id = sxword_2retrieve (&field_names, "pred", 4);
    break;
  case NODE_REF_n:
    sxput_error (visited->token.source_index,
		 "%sIllegal f_structure reference: ignored.",
		 sxtab_ptr->err_titles [2]);
    visited->kind = ERROR_KIND;
  default:
    return;
  }

  if (field_id == ERROR_STE) {
    sxput_error (visited->token.source_index,
		 "%sUnknown attribute name: ignored.",
		 sxtab_ptr->err_titles [2]);
  }

  visited->kind = field_id2kind [field_id];
  visited->id = field_id;
  SXBA_1_bit (used_field_set, field_id);
}

/* Les 2 arg sont des ATOM_KIND */
/* On regarde s'ils sont declares avec les memes atomes et si ces atomes ont les memes valeurs locales */
/* Il faudrait memoiser */
static BOOLEAN
check_identical_atom_type (atom_field_id1, atom_field_id2)
     int atom_field_id1, atom_field_id2;
{
  SXBA atom_set;

  atom_set = field_id2atom_set [atom_field_id1];

  if (sxba_first_difference (atom_set, field_id2atom_set [atom_field_id2]) != -1)
    return FALSE;

#ifndef ESSAI
  atom2local_id1 = fieldXatom2local_id [atom_field_id1];
  atom2local_id2 = fieldXatom2local_id [atom_field_id2];
		      
  atom_id = -1;

  while ((atom_id = sxba_scan (atom_set, atom_id)) >= 0) {
    if (atom2local_id1 [atom_id] != atom2local_id2 [atom_id])
      return FALSE;
  }
#endif /* ESSAI */
  
  return TRUE;
}



static void lfg_pi () {

  /*
    I N H E R I T E D
  */
  int  field_id, atom_id, field_top;
  NODE *father;

  switch ((father = VISITED->father)->name) {

  case ERROR_n :
    break;

  case AMALGAM_n :/* VISITED->name = AMALGAM_S_n */
    /* On saute le sous-arbre */
    sxat_snv (DERIVED, VISITED->father);
    break;

  case AMALGAM_S_n :/* VISITED->name = IDENT_S_n */
    break;

  case ARGUMENT_n :/* VISITED->name = ARGUMENT_S_n */
    /* On saute le sous-arbre */
    sxat_snv (DERIVED, VISITED->father);
    break;

  case ARGUMENT_S_n :/* VISITED->name = {CHAINE_n, IDENT_n, PRED_n} */
    break;

  case ATOM_CONCAT_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */
      
      switch (VISITED->name) {
      case NODE_REF_n:
	sxput_error (father->son->token.source_index,
		     "%sMust be an atomic field name: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
      break;
      case TERME_CHAMP_n:
	sxput_error (father->son->token.source_index,
		     "%sImplementation restriction, only binary compound names are allowed: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
      break;
      case TERME_CHEMIN_n:
      case TERME_CHEMIN_DOT_n:
      break;
      default:
#if EBUG
	sxtrap(ME,"unknown switch case #1");
#endif
	break;
      }
      break;
      
    case 2 :/* VISITED->name = {CHAINE_n, ENTIER_n, IDENT_n, NODE_REF_n, PRED_n, TERME_CHAMP_n, TERME_CHEMIN_n, 
	       TERME_CHEMIN_DOT_n} */
      if (father->son->kind != ATOM_KIND) {
	sxput_error (father->son->token.source_index,
		     "%sMust be an atomic value reference: ignored.",
		     sxtab_ptr->err_titles [2]);

	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      }

      switch (VISITED->name) {
      case CHAINE_n:
      case ENTIER_n:
      case IDENT_n: 
      case PRED_n: 
	VISITED->id = VISITED->token.string_table_entry;
	VISITED->kind = ATOM_KIND; /* Abusif!! */
	break;
      case NODE_REF_n:
	sxput_error (VISITED->token.source_index,
		     "%sIllegal f_structure reference: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      case TERME_CHAMP_n:
      case TERME_CHEMIN_n:
      case TERME_CHEMIN_DOT_n:
	sxput_error (VISITED->token.source_index,
		     "%sImplementation restriction, must be an atomic value: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      default:
	sxtrap (ME, "lfg_pi");
      }
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #2");
#endif
      break;
      
    }
    
    break;

  case ATOM_S_n :/* VISITED->name = {ATOM_CONCAT_n, ATOM_MINUS_n, ATOM_PLUS_n, CHAINE_n, ENTIER_n, IDENT_n, PRED_n} */
    if (father->father->name == ATTR_VAL_n || father->father->name == UNIFIER_n) {
      /* Utilisation d'un atome comme valeur de champ */
      if (father->father->name == ATTR_VAL_n)
	/* Pas d'analyse de structure ds la partie syntaxique */
	sxtrap (ME, "lfg_pi (ATOM_S)");

      switch (VISITED->name) {
      case ATOM_MINUS_n:  
	VISITED->id = sxword_2retrieve (&atom_names, "-", 1);
	break;
      case ATOM_PLUS_n:  
	VISITED->id = sxword_2retrieve (&atom_names, "+", 1);
	break;
      case PRED_n: 
	VISITED->id = sxword_2retrieve (&atom_names, "pred", 4);
	break;
      case CHAINE_n: 
      case ENTIER_n: 
      case IDENT_n: 
	VISITED->id = sxword_2retrieve (&atom_names,
					sxstrget (VISITED->token.string_table_entry),
					sxstrlen (VISITED->token.string_table_entry));
	break;
      case ATOM_CONCAT_n:
	break;
      default:
	sxtrap (ME, "lfg_pi (ATOM_S)");
      }
      break;
    }

    if (VISITED->name == ATOM_CONCAT_n) 
      sxtrap (ME, "lfg_pi (ATOM_S)");
    
    father = father->father;

    while (father->name == OPT_DISJONCTION_n)
      father = father->father;

    if (father->name != DISJONCTION_S_n)
      sxtrap (ME, "lfg_pi (ATOM_S)");

    if (father->father->name == ATTR_ATOMS_n || father->father->name == ATTR_DOTS_n) {
      /* definition d'un atome */
      if (current_field_id == 0)
	sxtrap (ME, "lfg_pi");

      if (VISITED->father->degree > 1) {
	sxput_error (VISITED->token.source_index,
		     "%sDisjunctions are not allowed in declarations: ignored.",
		     sxtab_ptr->err_titles [2]);
	
	sxat_snv (DERIVED, VISITED->father);
      }
      else {
	switch (VISITED->name) {
	case ATOM_MINUS_n:  
	  atom_id = sxword_2save (&atom_names, "-", 1);
	  break;
	case ATOM_PLUS_n:  
	  atom_id = sxword_2save (&atom_names, "+", 1);
	  break;
	case PRED_n: 
	  atom_id = sxword_2save (&atom_names, "pred", 4);
	  break;
	case CHAINE_n: 
	case ENTIER_n: 
	case IDENT_n:
	  atom_id = sxword_2save (&atom_names,
				  sxstrget (VISITED->token.string_table_entry),
				  sxstrlen (VISITED->token.string_table_entry));

	  if (VISITED->token.string_table_entry == (unsigned)ste_Aij || VISITED->token.string_table_entry == (unsigned)ste_Pij) {
	    if (father->father->name != ATTR_DOTS_n) {
	      sxput_error (VISITED->token.source_index,
			   "%sMisuse of this special c-structure atom: ignored.",
			   sxtab_ptr->err_titles [2]);
	
	      sxat_snv (DERIVED, VISITED->father);
	    }
	    else {
	      if (VISITED->token.string_table_entry == (unsigned)ste_Aij)
		atom_Aij = atom_id;
	      else
		atom_Pij = atom_id;
	    }
	  }

	  break;
	default:
	  sxtrap (ME, "lfg_pi (ATOM_S)");
	  sxinitialise(atom_id); /* pour faire taire gcc -Wuninitialized */
	}

	VISITED->id = atom_id;
	SXBA_1_bit (field_id2atom_set [current_field_id], atom_id);
      }

      break;
    }

    if (father->father->name == SOUS_CAT_n
	|| father->father->name == PRED_FIELD_n) {
      /* definition d'un pred */
      /* Pas d'analyse de pred ds la partie syntaxique */
      sxtrap (ME, "lfg_pi (ATOM_S)");

      switch (VISITED->name) {
      case ATOM_MINUS_n: 
      case ATOM_PLUS_n:
      case PRED_n: 
      case CHAINE_n: 
      case ENTIER_n:
	sxput_error (VISITED->token.source_index,
		     "%sIllegal sub-categorization: ignored.",
		     sxtab_ptr->err_titles [2]);
	field_id = ERROR_STE;
	break; 
      case IDENT_n: 
	field_id = sxword_2retrieve (&field_names,
				     sxstrget (VISITED->token.string_table_entry),
				     sxstrlen (VISITED->token.string_table_entry));

	if (field_id == ERROR_STE) {
	  sxput_error (VISITED->token.source_index,
		       "%sUnknown attribute name: ignored.",
		       sxtab_ptr->err_titles [2]);
	}
	else {
	  if (!SXBA_bit_is_set (argument_set, field_id))
	    sxput_error (VISITED->token.source_index,
			 "%sMust be an argument name: ignored.",
			 sxtab_ptr->err_titles [2]);
	}
	break;
      default:
	sxtrap (ME, "lfg_pi (ATOM_S)");
	sxinitialise(field_id); /* pour faire taire gcc -Wuninitialized */
      }

      VISITED->id = field_id;
      break;
    }
    else
      sxtrap (ME, "lfg_pi (ATOM_S)");

    break;

  case ATTRIBUTE_n :/* VISITED->name = ATTRIBUTE_S_n */
    break;

  case ATTRIBUTE_PASS_NB_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {ATTR_ARG_STRUCT_n, ATTR_ATOMS_n, ATTR_DOTS_n, ATTR_SET_OF_STRUCT_n, ATTR_STRUCT_n} */
      break;

    case 2 :/* VISITED->name = {ENTIER_n, VOID_n} */
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #3");
#endif
      break;

    }

    break;

  case ATTRIBUTE_S_n :/* VISITED->name = ATTRIBUTE_PASS_NB_n */
    break;

  case ATTR_ATOMS_n :
    /* <attribut> = %IDENT "=" "{" <disjonction_LIST> "}" ; "ATTR_ATOMS" */
    switch (VISITED->position) {
    case 1 :/* VISITED->name = IDENT_n */
      field_top = SXWORD_top (field_names);
      field_id = sxword_2save (&field_names,
			      sxstrget (VISITED->token.string_table_entry),
			      sxstrlen (VISITED->token.string_table_entry));

      if (SXWORD_top (field_names) == (unsigned)field_top) {
	/* Redeclaration */
	sxput_error (VISITED->token.source_index,
		     "%sAttribute name already defined: ignored.",
		     sxtab_ptr->err_titles [2]);
	  
	sxat_snv (DERIVED, VISITED->father);
      }
      else {
	field_id2kind [field_id] = ATOM_KIND;
	current_field_id = field_id;
      }
      break;

    case 2 :/* VISITED->name = DISJONCTION_S_n */
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #4");
#endif
      break;

    }

    break;

  case ATTR_DOTS_n :
    /* <attribut> = %IDENT "=" "{" <disjonction*> "." "." "." "}" ; "ATTR_DOTS" */
    /* Les atomes de <disjonction*> sont ceux que l'on va trouver ds la lfg ds les equations definissant les f-structures */
    /* Les atomes de ... sont ceux du niveau lexical */
    switch (VISITED->position) {
    case 1 :/* VISITED->name = IDENT_n */
      field_top = SXWORD_top (field_names);
      field_id = sxword_2save (&field_names,
			      sxstrget (VISITED->token.string_table_entry),
			      sxstrlen (VISITED->token.string_table_entry));

      if (SXWORD_top (field_names) == (unsigned)field_top) {
	/* Redeclaration */
	sxput_error (VISITED->token.source_index,
		     "%sAttribute name already defined: ignored.",
		     sxtab_ptr->err_titles [2]);
	  
	sxat_snv (DERIVED, VISITED->father);
      }
      else {
	field_id2kind [field_id] = UNBOUNDED_KIND+ATOM_KIND;
	/* L'ensemble des atomes est (sous/non) specifie' */
	current_field_id = field_id;
      }
      break;

    case 2 :/* VISITED->name = {DISJONCTION_S_n, VOID_n} */
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #5");
#endif
      break;

    }

    break;

  case ATTR_VAL_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {FIELD_n, OPT_FIELD_n} */
      break;

    case 2 :/* VISITED->name = {FIELD_CONSTRAINT_n, FIELD_EQUAL_n, FIELD_OPTIONAL_n} */
      break;

    case 3 :/* VISITED->name = {ATOM_S_n, ATTR_VAL_S_n, EMPTY_ADJUNCT_n, EMPTY_ATOM_n, EMPTY_NAMED_ADJUNCT_n, 
	       EMPTY_NAMED_ATOM_n, EMPTY_NAMED_STRUCT_n, EMPTY_STRUCT_n, NAMED_ADJUNCT_n, NAMED_ATOM_n, NAMED_STRUCTURE_n, STRUCTURE_S_n} */
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #6");
#endif
      break;

    }

    break;

  case ATTR_VAL_S_n :/* VISITED->name = {ATTR_VAL_n, MACRO_n, PRED_FIELD_n} */
    break;

  case CATEGORY_n :/* VISITED->name = ARGUMENT_S_n */
    /* On saute le sous-arbre */
    sxat_snv (DERIVED, VISITED->father);
    break;

  case COMPLETE_n :/* VISITED->name = PAIR_S_n */
    /* Le 20/06/05 Ca permet specifier la liste des non-terminaux dont les f_structures doivent etre complete et coherentes */
    {
      int  lfg_pi_nt_id;
      NODE *son, *son2;
  
      sxword_alloc (&complete_names, "complete_names", VISITED->degree+2, 1, 16,
		    sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

      complete_name2ste = (int*) sxcalloc (VISITED->degree+2, sizeof (int));

      for (son = VISITED->son; son != NULL; son = son->brother) {
	if (son->name == PAIR_n)
	  son2 = son->son;
	else
	  son2 = son;

	lfg_pi_nt_id = sxword_2save (&complete_names,
			      sxstrget (son2->token.string_table_entry),
			      sxstrlen (son2->token.string_table_entry));
	
	complete_name2ste [lfg_pi_nt_id] = (son->name == PAIR_n) ? son2->brother->token.string_table_entry : ERROR_STE;
      }
    }

    break;

  case COMPOUND_n :/* VISITED->name = COMPOUND_S_n */
    /* On saute le sous-arbre */
    sxat_snv (DERIVED, VISITED->father);
    break;

  case COMPOUND_S_n :/* VISITED->name = IDENT_S_n */
    break;

  case DECLARATION_S_n :/* VISITED->name = {AMALGAM_n, ARGUMENT_n, ATTRIBUTE_n, CATEGORY_n, COMPLETE_n, COMPOUND_n, MACRO_STRUCTURE_n, 
			   TERMINAL_n} */
    current_field_id = 0; /* A priori, on n'est pas ds une liste d'attributs */
    break;

  case DISJONCTION_S_n :/* VISITED->name = {ATOM_S_n, OPT_DISJONCTION_n} */
    break;

  case EQUATION_S_n :/* VISITED->name = {ETIQUETTE_n, EXISTE_n, NON_n, NONEXISTE_n, POSSIBLE_n, UNIFIER_n} */
    break;

  case ETIQUETTE_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = NODE_REF_n */
      break;

    case 2 :/* VISITED->name = {ETIQUETTE_n, EXISTE_n, NON_n, NONEXISTE_n, POSSIBLE_n, UNIFIER_n} */
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #7");
#endif
      break;

    }

    break;

  case EXISTE_n :/* VISITED->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
    if (VISITED->name == NODE_REF_n) {
      sxput_error (VISITED->token.source_index,
		   "%sIllegal $ ref occurrence: ignored.",
		   sxtab_ptr->err_titles [2]);
      VISITED->kind = ERROR_KIND;
      sxat_snv (DERIVED, father);
    }

    break;

  case IDENT_S_n :/* VISITED->name = {CHAINE_n, IDENT_n} */
    break;

  case KLEENE_n :/* VISITED->name = {CHAINE_n, ENTIER_n, IDENT_n, NODE_REF_n, OU_S_n, PRED_n, TERME_CHAMP_n, 
		    TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
    fill_leaves_field_kind (VISITED);
    break;

  case LFG_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = DECLARATION_S_n */
      break;

    case 2 :/* VISITED->name = RULES_S_n */
      current_field_id = 0;
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #8");
#endif
      break;

    }

    break;

  case MACRO_STRUCTURE_n :
    /* On saute le sous-arbre */
    sxat_snv (DERIVED, VISITED->father);
    switch (VISITED->position) {
    case 1 :/* VISITED->name = MACRO_NAME_n */
      break;

    case 2 :/* VISITED->name = {ATTR_VAL_S_n, EMPTY_NAMED_STRUCT_n, EMPTY_STRUCT_n, NAMED_STRUCTURE_n} */
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #9");
#endif
      break;

    }

    break;

case NAMED_ADJUNCT_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = STRUCTURE_S_n */
	break;

	case 2 :/* VISITED->name = ENTIER_n */
	break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #10");
#endif
      break;

	}

break;

case NAMED_ATOM_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = ATOM_S_n */
	break;

	case 2 :/* VISITED->name = ENTIER_n */
	break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #11");
#endif
      break;

	}

break;

  case NAMED_STRUCTURE_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = ATTR_VAL_S_n */
      break;

    case 2 :/* VISITED->name = ENTIER_n */
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #12");
#endif
      break;

    }

    break;

  case NON_n :/* VISITED->name = {EXISTE_n, NON_n, NONEXISTE_n, UNIFIER_n} */
    break;

  case NONEXISTE_n :/* VISITED->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
    if (VISITED->name == NODE_REF_n) {
      sxput_error (VISITED->token.source_index,
		   "%sIllegal $ ref occurrence: ignored.",
		   sxtab_ptr->err_titles [2]);
      VISITED->kind = ERROR_KIND;
      sxat_snv (DERIVED, father);
    }

    break;

  case OPT_DISJONCTION_n :/* VISITED->name = {ATOM_S_n, OPT_DISJONCTION_n} */
    break;

  case OU_S_n :/* VISITED->name = {CHAINE_n, ENTIER_n, IDENT_n, KLEENE_n, NODE_REF_n, OU_S_n, PRED_n, TERME_CHAMP_n, 
		  TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
    if (VISITED->name == NODE_REF_n) {
      sxput_error (VISITED->token.source_index,
		   "%sIllegal $ ref occurrence: ignored.",
		   sxtab_ptr->err_titles [2]);
      VISITED->kind = ERROR_KIND;
      sxat_snv (DERIVED, father);
      break;
    }
      
    fill_leaves_field_kind (VISITED);
    break;

  case PAIR_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = IDENT_n */
      break;

    case 2 :/* VISITED->name = CHAINE_n */
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #13");
#endif
      break;

    }

    break;

  case PAIR_S_n :/* VISITED->name = {IDENT_n, PAIR_n} */
    break;

  case POSSIBLE_n :/* VISITED->name = {EXISTE_n, NON_n, NONEXISTE_n, UNIFIER_n} */
    break;

  case PRED_FIELD_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {FIELD_CONSTRAINT_n, FIELD_EQUAL_n, FIELD_OPTIONAL_n} */
      break;

    case 2 :/* VISITED->name = {CHAINE_n, IDENT_n, PRED_n} */
      break;

    case 3 :/* VISITED->name = {DISJONCTION_S_n, EMPTY_SOUS_CAT_n, SOUS_CAT_n} */
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #14");
#endif
      break;

    }

    break;

  case RULE_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = VOCABULARY_S_n */
      break;

    case 2 :/* VISITED->name = EQUATION_S_n */
      EQUATION_nb += VISITED->degree;
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #15");
#endif
      break;

    }

    break;

  case RULES_S_n :/* VISITED->name = RULE_n */
    break;

  case SOUS_CAT_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = DISJONCTION_S_n */
      break;

    case 2 :/* VISITED->name = DISJONCTION_S_n */
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #16");
#endif
      break;

    }

    break;

  case STRUCTURE_S_n :/* VISITED->name = {ATTR_VAL_S_n, EMPTY_NAMED_STRUCT_n, EMPTY_STRUCT_n, NAMED_STRUCTURE_n} */
    break;

  case TERME_CHAMP_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */
      
      switch (VISITED->name) {
      case NODE_REF_n:
	sxput_error (father->son->token.source_index,
		     "%sMust be an atomic field name: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
      break;
      case TERME_CHAMP_n:
	sxput_error (father->son->token.source_index,
		     "%sImplementation restriction, only binary compound names are allowed: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
      break;
      case TERME_CHEMIN_n:
      case TERME_CHEMIN_DOT_n:
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #17");
#endif
      break;
      }
      break;

    case 2 :/* VISITED->name = {CHAINE_n, ENTIER_n, IDENT_n, NODE_REF_n, PRED_n, TERME_CHAMP_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */
      if (father->son->kind != ATOM_KIND) {
	sxput_error (father->son->token.source_index,
		     "%sMust be an atomic value reference: ignored.",
		     sxtab_ptr->err_titles [2]);

	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      }

      switch (VISITED->name) {
      case CHAINE_n:
      case ENTIER_n:
      case IDENT_n: 
      case PRED_n: 
	VISITED->id = VISITED->token.string_table_entry;
	VISITED->kind = ATOM_KIND; /* Abusif!! */
	break;
      case NODE_REF_n:
	sxput_error (VISITED->token.source_index,
		     "%sIllegal f_structure reference: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      case TERME_CHAMP_n:
      case TERME_CHEMIN_n:
      case TERME_CHEMIN_DOT_n:
	sxput_error (VISITED->token.source_index,
		     "%sImplementation restriction, must be an atomic value: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      default:
	sxtrap (ME, "lfg_pi");
      }

      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #18");
#endif
      break;
    }

    break;

  case TERME_CHAMP_RE_n :
    /* En fait, ds l'implantation actuelle, on interdit les expressions regulieres comme operandes
       de l'operateur "-" qui fabrique des noms de champ.
       Autrement dit, si on entre ici, on n'en sort pas vivant !! et le noeud TERME_CHAMP_RE_n est donc
       interdit ds une analyse correcte. */
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
      switch (VISITED->name) {
      case NODE_REF_n:
	sxput_error (father->son->token.source_index,
		     "%sMust be an atomic field name: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
      break;
      case TERME_CHAMP_n:
	sxput_error (father->son->token.source_index,
		     "%sImplementation restriction, only binary compound names are allowed: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
      break;
      case TERME_CHEMIN_n:
      case TERME_CHEMIN_DOT_n:
      break;
      case TERME_CHAMP_RE_n:
      case TERME_CHEMIN_RE_n:
      case TERME_CHEMIN_RE_DOT_n:
	sxput_error (father->son->token.source_index,
		     "%sImplementation restriction, regular expressions are not allowed, only binary compound names are allowed: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #19");
#endif
      break;
      }
      break;

    case 2 :/* VISITED->name = {CHAINE_n, ENTIER_n, IDENT_n, KLEENE_n, NODE_REF_n, OU_S_n, PRED_n, TERME_CHAMP_n, 
	       TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
      switch (VISITED->name) {
      case  KLEENE_n:
      case  OU_S_n:
      case  TERME_CHAMP_RE_n:
      case  TERME_CHEMIN_RE_n:
      case  TERME_CHEMIN_RE_DOT_n:
	sxput_error (father->son->token.source_index,
		       "%sImplementation restriction, regular expressions are not allowed, must be a fixed arity atomic value reference: ignored.",
		       sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;

      case  CHAINE_n:
      case  ENTIER_n:
      case  IDENT_n:
      case  NODE_REF_n:
      case  PRED_n:
      case  TERME_CHAMP_n:
      case  TERME_CHEMIN_n:
      case  TERME_CHEMIN_DOT_n:
	if (father->son->kind != ATOM_KIND) {
	  sxput_error (father->son->token.source_index,
		       "%sMust be an atomic value reference: ignored.",
		       sxtab_ptr->err_titles [2]);

	  VISITED->kind = ERROR_KIND;
	  sxat_snv (DERIVED, father);
	  break;
	}
	break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #20");
#endif
      break;
      }

      switch (VISITED->name) {
      case CHAINE_n:
      case ENTIER_n:
      case IDENT_n: 
      case PRED_n: 
	VISITED->id = VISITED->token.string_table_entry;
	VISITED->kind = ATOM_KIND; /* Abusif!! */
	break;
      case NODE_REF_n:
	sxput_error (VISITED->token.source_index,
		     "%sIllegal f_structure reference: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      case TERME_CHAMP_n:
      case TERME_CHEMIN_n:
      case TERME_CHEMIN_DOT_n:
	sxput_error (VISITED->token.source_index,
		     "%sImplementation restriction, must be an atomic value: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      default:
	sxtrap (ME, "lfg_pi");
      }

      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #21");
#endif
      break;
    }

    break;

  case TERME_CHEMIN_n :
  case TERME_CHEMIN_DOT_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */
      if (VISITED->name == NODE_REF_n)
	VISITED->kind = STRUCT_KIND;
      break;

    case 2 :/* VISITED->name = {CHAINE_n, ENTIER_n, IDENT_n, NODE_REF_n, PRED_n, TERME_CHAMP_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */
      if (father->son->kind != ERROR_KIND && father->son->kind != STRUCT_KIND) {
	sxput_error (father->son->token.source_index,
		     "%sMust be a f-structure reference: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      }

      if (VISITED->name == NODE_REF_n) {
	sxput_error (VISITED->token.source_index,
		     "%sIllegal $ ref occurrence: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      }

      fill_leaves_field_kind (VISITED);

      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #22");
#endif
      break;
    }

    break;

  case TERME_CHEMIN_RE_n :
  case TERME_CHEMIN_RE_DOT_n:
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
      if (VISITED->name == NODE_REF_n)
	VISITED->kind = STRUCT_KIND;
      break;

    case 2 :/* VISITED->name = {CHAINE_n, ENTIER_n, IDENT_n, KLEENE_n, NODE_REF_n, OU_S_n, PRED_n, TERME_CHAMP_n, 
	       TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
      if (father->son->kind != ERROR_KIND && father->son->kind != STRUCT_KIND) {
	sxput_error (father->son->token.source_index,
		     "%sMust be a f-structure reference: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      }

      if (VISITED->name == NODE_REF_n) {
	sxput_error (VISITED->token.source_index,
		     "%sIllegal $ ref occurrence: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	sxat_snv (DERIVED, father);
	break;
      }

      fill_leaves_field_kind (VISITED);

      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #23");
#endif
      break;
    }

    break;

  case TERMINAL_n :/* VISITED->name = ARGUMENT_S_n */
    /* On saute le sous-arbre */
    sxat_snv (DERIVED, VISITED->father);
    break;

  case UNIFIER_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
      if (VISITED->name == NODE_REF_n)
	VISITED->kind = STRUCT_KIND;
      break;

    case 2 :/* VISITED->name = {ADD_OPER_n, CONTRAINTE_OPER_n, DANS_OPER_n, OPTION_OPER_n, UNIF_OPER_n} */
      break;

    case 3 :/* VISITED->name = {ATOM_S_n, EMPTY_ADJUNCT_n, EMPTY_ATOM_n, EMPTY_STRUCT_n, LEXEME_NODE_REF_n, LEX_NODE_REF_n
	       , NODE_REF_n, TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
      if (VISITED->father->son->kind & ATOM_KIND)
	last_lhs_atom_field_id = VISITED->father->son->id;

      if (VISITED->name == NODE_REF_n)
	VISITED->kind = STRUCT_KIND;

      doli = -1;
      break;
    default:
#if EBUG
      sxtrap(ME,"unknown switch case #24");
#endif
      break;

    }

    break;

  case VOCABULARY_S_n :/* VISITED->name = {IDENT_n, MOT_1COL_n, OPT_NON_TERMINAL_n} */
    if (VISITED->name == MOT_1COL_n) {
      /* LHS */
      if (islower (*sxstrget (VISITED->token.string_table_entry))) {
	/* terminal */
	sxput_error (VISITED->token.source_index, 
		     "%sIllegal terminal symbol as LHS of a production.",
		     sxtab_ptr->err_titles [2]);
      }
    }

    break;



    /*
      Z Z Z Z
    */

  default:
    break;
  }
  /* end lfg_pi */
}

static void lfg_pd () {

  /*
    D E R I V E D
  */

  int           field_top, field_id, atom_id, oper_name, i;
  SXBA          current_atom_set;
#ifndef ESSAI
  int           *int_ptr, *atom2local_id, *local2atom_id;
#endif /* ESSAI */
  SXNODE        *brother, *son, *father;
  unsigned char field_kind, prev_field_kind;

  switch (VISITED->name) {

  case ERROR_n :
    break;

  case ADD_OPER_n :
    break;

  case AMALGAM_n :
    break;

  case AMALGAM_S_n :
    break;

  case ARGUMENT_n :
    break;

  case ARGUMENT_S_n :
    break;

  case ATOM_CONCAT_n :
    /* la passe heritee assure que l'on a c,a... */
    son = VISITED->son;
    /* son->name == {TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */
    /* son->kind == ATOM_KIND */
    brother = son->brother;/* brother->name = {CHAINE_n, ENTIER_n, IDENT_n, PRED_n} */

    if (son->kind != ERROR_KIND && brother->kind != ERROR_KIND) {
      /* On suppose pour l'instant que l'on n'a que 2 composants */
      /* ($i path+)-obj */
      int datom_id, suffix_id, complex_atom_id;
#if 0
      SXBA last_lhs_atom_set;
      BOOLEAN invalid_complex_atom_detected;
#endif /* 0 */
  
      if (last_lhs_atom_field_id == 0) {
	sxput_error (VISITED->token.source_index, 
		     "%sComplex atomic value assigned to an LHS path referencing a non-atomic field.",
		     sxtab_ptr->err_titles [2]);
	break;
      }
#if 0
      last_lhs_atom_set = field_id2atom_set [last_lhs_atom_field_id];
#endif /* 0 */

      if (datom_pairs2atom_id == NULL) {
	XxY_alloc (&datom_pairs, "datom_pairs", max_atom_id+1, 1, 0, 0, datom_pairs_oflw, NULL);
	datom_pairs2atom_id = (int*) sxalloc (XxY_size (datom_pairs)+1, sizeof (int));
	sxword_alloc (&suffix_names, "suffix_names", FIELDS_nb+1, 1, 8,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
      }

      current_atom_set = field_id2atom_set [son->id];

      suffix_id = sxword_2save (&suffix_names, sxstrget (brother->id), sxstrlen (brother->id));

#if 0
      invalid_complex_atom_detected = FALSE;
#endif /* 0 */

      atom_id = 0;
      sxinitialise (complex_atom_id);

      while ((atom_id = sxba_scan (current_atom_set, atom_id)) > 0) {
	varstr_raz (vstr);
	vstr = varstr_catenate (vstr, SXWORD_get (atom_names, atom_id));
	vstr = varstr_catchar (vstr, '-');
	vstr = varstr_catenate (vstr, sxstrget (brother->id));

	complex_atom_id = sxword_2retrieve (&atom_names, varstr_tostr (vstr), varstr_length (vstr));

#if 0
	if (complex_atom_id == 0) {
	  invalid_complex_atom_detected = TRUE;
	  sxput_error (VISITED->token.source_index, 
		       "%sComplex atomic value may result into \"%s\", which is not declared as a possible atomic value.",
		       sxtab_ptr->err_titles [2], varstr_tostr (vstr));
	} else {
	  if (!SXBA_bit_is_set (last_lhs_atom_set,complex_atom_id)) {
	    invalid_complex_atom_detected = TRUE;
	    sxput_error (VISITED->token.source_index, 
			 "%sComplex atomic value may result into \"%s\", which is not declared as a possible atomic value for field %s.",
			 sxtab_ptr->err_titles [2], varstr_tostr (vstr), SXWORD_get (field_names, last_lhs_atom_field_id) );
	  } else {
#endif /* 0 */
	    if (!XxY_set (&datom_pairs, atom_id, suffix_id, &datom_id))
	      datom_pairs2atom_id [datom_id] = complex_atom_id; /* peut valoir 0 si la combinaison n'est pas un atome d�clar� */
#if 0
	  }
	}
#endif /* 0 */

      }

      brother->id = suffix_id;
      VISITED->kind = ATOM_KIND; /* ?? */
      VISITED->id = complex_atom_id; /* Pb, le nom est dynamique, j'en prend 1!! */
    }
    break;
    
  case ATOM_MINUS_n :
    break;

  case ATOM_PLUS_n :
    break;

  case ATOM_S_n :
    father = VISITED->father;

    while (father->name == OPT_DISJONCTION_n)
      father = father->father;

    /* father->name == DISJONCTION_S_n */
    if (father->father->name == ATTR_ATOMS_n) {
      /* definition d'un atome */
      if (current_field_id == 0)
	sxtrap (ME, "lfg_pd");
    
      break;
    }

    if (father->father->name == SOUS_CAT_n
	|| father->father->name == PRED_FIELD_n) {
      /*  <attr_val> = pred <field_oper> "\"" <graphie> <sous_cat> "\"" ; "PRED_FIELD"
	  <sous_cat> = "<" <disjonction_LIST> ">" ;
	  <sous_cat> = "<" <disjonction_LIST> ">" <disjonction_LIST> ; "SOUS_CAT"
	  <disjonction_LIST> = <disjonction_LIST> "," <disjonction> ;
	  <disjonction_LIST> = <disjonction> ; "DISJONCTION_S"
	  <disjonction> = "(" <disjonction> ")" ; "OPT_DISJONCTION"
	  <disjonction> = <atom_LIST> ;
	  <atom_LIST> = <atom_LIST> "|" <atome> ;
	  <atom_LIST> = <atome> ; "ATOM_S"
      */
      /* On est ds la definition d'un pred */
      if (VISITED->degree > 1) {
	/* ... disjonction non triviale */
	for (brother = VISITED->son; brother != 0; brother = brother->brother) {
	  SXBA_1_bit (working_field_set, brother->id);
	}

	field_id = -1;

	while ((field_id = sxba_scan_reset (working_field_set, field_id)) >= 0) {
	  XH_push (compound_fields_hd, field_id);
	}
	
	XH_set (&compound_fields_hd, &field_id);
	field_id += max_field_id; /* Pour reconnaitre les disjonctions */
      }
      else {
	field_id = VISITED->son->id;
      }

      VISITED->id = field_id;
    }
    break;

  case ATTRIBUTE_n :
    /* On vient de terminer la partie declaration des noms d'attributs, ils sont ranges ds field_names */
    /* On alloue les variables de travail ... */ 
    max_field_id = SXWORD_top (field_names)-1; 
    max_atom_id = SXWORD_top (atom_names)-1;

    working_field_set = sxba_calloc (max_field_id+1);
    used_field_set = sxba_calloc (max_field_id+1);
    argument_set = sxba_calloc (max_field_id+1);

#ifndef ESSAI
    fieldXatom2local_id = (int**) sxalloc (max_field_id+1, sizeof (int*));
    int_ptr = local_id_area = (int*) sxcalloc ((max_field_id+1)*(max_atom_id+1), sizeof (int));

    for (field_id = 0; field_id <= max_field_id; field_id++) {
      fieldXatom2local_id [field_id] = int_ptr;
      int_ptr += max_atom_id+1;
    }

    fieldXlocal2atom_id = (int**) sxalloc (max_field_id+1, sizeof (int*));
    int_ptr = local_id_area2 = (int*) sxcalloc ((max_field_id+1)*(max_local_atom_id+1), sizeof (int));

    for (field_id = 0; field_id <= max_field_id; field_id++) {
      fieldXlocal2atom_id [field_id] = int_ptr;
      int_ptr += max_local_atom_id+1;
    }

    max_local_atom_id = 0;

    for (field_id = EMPTY_STE+1; field_id <= max_field_id; field_id++) {
      field_kind = field_id2kind [field_id];

      if ((field_kind & ATOM_KIND) && ((field_kind & Aij_KIND) == 0)) {
	current_atom_set = field_id2atom_set [field_id];
	atom2local_id = fieldXatom2local_id [field_id];
	local2atom_id = fieldXlocal2atom_id [field_id];
	current_field_id = field_id;
	local_atom_id = 0;
	atom_id = 0;

	while ((atom_id = sxba_scan (current_atom_set, atom_id)) > 0) {
	  local_atom_id++;
	  atom2local_id [atom_id] = local_atom_id;
	  local2atom_id [local_atom_id] = atom_id;
	}

	if (field_kind == ATOM_KIND) {
	  max_atom_field_nb++;

	  if (local_atom_id > max_local_atom_id)
	    max_local_atom_id = local_atom_id;
	}
	else
	  max_undef_atom_field_nb++;
      }
    }

    if (max_local_atom_id > 31) {
      sxhmsg (VISITED->token.source_index.file_name, 
	      "%sImplementation restriction : for some types, the number of their atomic values exceeds 31.",
	      sxtab_ptr->err_titles [2]);
      sxat_snv (DERIVED, sxatcvar.atc_lv.abstract_tree_root);
    }
#endif /* ESSAI */

    break;

  case ATTRIBUTE_PASS_NB_n :
    break;

  case ATTRIBUTE_S_n :
    break;

  case ATTR_ARG_STRUCT_n :
    /* <attribut> = %IDENT "=" "<" "[" "]" ">" ; "ATTR_ARG_STRUCT" */
    field_top = SXWORD_top (field_names);
    field_id = sxword_2save (&field_names,
			     sxstrget (VISITED->token.string_table_entry),
			     sxstrlen (VISITED->token.string_table_entry));

    if (SXWORD_top (field_names) == (unsigned)field_top) {
      /* Redeclaration */
      sxput_error (VISITED->token.source_index,
		   "%sAttribute name already defined: ignored.",
		   sxtab_ptr->err_titles [2]);
      /* On continue la visite normale */
    }
    else {
      field_id2kind [field_id] = STRUCT_KIND;
    }
    break;

  case ATTR_ATOMS_n :
    /* <attribut> = %IDENT "=" "{" <disjonction_LIST> "}" ; "ATTR_ATOMS" */
#ifndef ESSAI
    if (VISITED->son->brother->degree > max_local_atom_id)
      /* un peu grossier */
      max_local_atom_id = VISITED->son->brother->degree;
#endif /* ESSAI */

    break;

  case ATTR_DOTS_n :
    /* <attribut> = %IDENT "=" "{" <disjonction*> "." "." "." "}" ; "ATTR_DOTS" */
#ifndef ESSAI
    if (VISITED->son->brother->degree > max_local_atom_id)
      /* un peu grossier */
      max_local_atom_id = VISITED->son->brother->degree;
#endif /* ESSAI */

    if ((atom_Aij || atom_Pij /* definis */)
	&& (SXBA_bit_is_set (field_id2atom_set [current_field_id], atom_Aij) || SXBA_bit_is_set (field_id2atom_set [current_field_id], atom_Pij))) {
      /* En fait le champ est du type Aij_KIND */
      field_id = sxword_2retrieve (&field_names,
				   sxstrget (VISITED->son->token.string_table_entry),
				   sxstrlen (VISITED->son->token.string_table_entry));
      field_id2kind [field_id] = Aij_KIND+UNBOUNDED_KIND+ATOM_KIND;
    }

    break;

  case ATTR_SET_OF_STRUCT_n :
    /* <attribut> = %IDENT "=" "{" "[" "]" "}" ; "ATTR_SET_OF_STRUCT" */
    field_top = SXWORD_top (field_names);
    field_id = sxword_2save (&field_names,
			     sxstrget (VISITED->token.string_table_entry),
			     sxstrlen (VISITED->token.string_table_entry));

    if (SXWORD_top (field_names) == (unsigned)field_top) {
      /* Redeclaration */
      sxput_error (VISITED->token.source_index,
		   "%sAttribute name already defined: ignored.",
		   sxtab_ptr->err_titles [2]);
      /* On continue la visite normale */
    }
    else {
      field_id2kind [field_id] = UNBOUNDED_KIND+STRUCT_KIND;
    }
    break;

  case ATTR_STRUCT_n :
    /* <attribut> = %IDENT "=" "[" "]" ; "ATTR_STRUCT" */
    field_top = SXWORD_top (field_names);
    field_id = sxword_2save (&field_names,
			     sxstrget (VISITED->token.string_table_entry),
			     sxstrlen (VISITED->token.string_table_entry));

    if (SXWORD_top (field_names) == (unsigned)field_top) {
      /* Redeclaration */
      sxput_error (VISITED->token.source_index,
		   "%sAttribute name already defined: ignored.",
		   sxtab_ptr->err_titles [2]);
      /* On continue la visite normale */
    }
    else {
      field_id2kind [field_id] = STRUCT_KIND;
    }
    break;

  case ATTR_VAL_n :
    break;

  case ATTR_VAL_S_n :
    break;

  case CATEGORY_n :
    break;

  case CHAINE_n :
    break;

  case COMPLETE_n :
    break;

  case COMPOUND_n :
    break;

  case COMPOUND_S_n :
    break;

  case CONTRAINTE_OPER_n :
    break;

  case DANS_OPER_n :
    break;

  case DECLARATION_S_n :
    break;

  case DISJONCTION_S_n :
    break;

case EMPTY_ADJUNCT_n :
break;

case EMPTY_ATOM_n :
break;

case EMPTY_NAMED_ADJUNCT_n :
break;

case EMPTY_NAMED_ATOM_n :
break;

  case EMPTY_NAMED_STRUCT_n :
    break;

  case EMPTY_SOUS_CAT_n :
    break;

  case EMPTY_STRUCT_n :
    break;

  case ENTIER_n :
    break;

  case EQUATION_S_n :
    break;

  case ETIQUETTE_n :
    /* VISITED->son->name = NODE_REF_n */
    if (atoi (sxstrget (VISITED->son->token.string_table_entry)) == 0)
      sxput_error (VISITED->son->token.source_index,
		   "%sIllegal LHS reference.",
		   sxtab_ptr->err_titles [2]);
    break;

  case EXISTE_n :
    break;

  case FIELD_n :
    break;

  case FIELD_CONSTRAINT_n :
    break;

  case FIELD_OPTIONAL_n :
    break;

  case FIELD_EQUAL_n :
    break;

  case IDENT_n :
    break;

  case IDENT_S_n :
    break;

  case KLEENE_n :
    if (VISITED->son->kind != STRUCT_KIND) {
      sxput_error (VISITED->son->token.source_index,
		   "%sMust be a structure type: ignored.",
		   sxtab_ptr->err_titles [2]);
      VISITED->kind = ERROR_KIND;
    }
    else {
      VISITED->kind = VISITED->son->kind;
      VISITED->id = VISITED->son->id;
    }
    break;

  case LEXEME_NODE_REF_n :
  case LEX_NODE_REF_n :
    i = atoi (sxstrget (VISITED->token.string_table_entry));

    if (i == 0 || i > rhs_length)
      sxput_error (VISITED->token.source_index,
		   "%sIllegal reference.",
		   sxtab_ptr->err_titles [2]);

    break;

  case LFG_n :
    break;

  case MACRO_n :
    break;

  case MACRO_NAME_n :
    break;

  case MACRO_STRUCTURE_n :
    break;

  case MOT_1COL_n :
    break;

case NAMED_ADJUNCT_n :
break;

case NAMED_ATOM_n :
break;

  case NAMED_STRUCTURE_n :
    break;

  case NODE_REF_n :
    i = atoi (sxstrget (VISITED->token.string_table_entry));

    if (i > rhs_length)
      sxput_error (VISITED->token.source_index,
		   "%sOut of rule reference.",
		   sxtab_ptr->err_titles [2]);

    if (doli == -1)
      doli = i;

    break;

  case NON_n :
    break;

  case NONEXISTE_n :
    break;

  case OPTION_OPER_n :
    break;

  case OPT_DISJONCTION_n :
    /* <disjonction> = "(" <disjonction> ")" ; "OPT_DISJONCTION" */
    VISITED->id = VISITED->son->id;
    break;

  case OPT_FIELD_n :
    break;

  case OPT_NON_TERMINAL_n :
    break;

  case OU_S_n :
    {
      BOOLEAN is_error = FALSE;

      field_kind = ERROR_KIND;

      for (brother = VISITED->son; brother != NULL; brother = brother->brother) {
	if (field_kind == ERROR_KIND)
	  field_kind = brother->kind;
	else {
	  if (brother->kind != ERROR_KIND) {
	    if (field_kind != brother->kind) {
	      sxput_error (brother->token.source_index,
			   "%sType mismatch in disjunctions: ignored.",
			   sxtab_ptr->err_titles [2]);
	      is_error = TRUE;
	    }
	  }
	  else
	    is_error = TRUE;
	}
      }

      VISITED->kind = (is_error) ? ERROR_KIND : field_kind;
      VISITED->id = (is_error) ? 0 : VISITED->son->id; /* Pourquoi pas!! */
    }
    break;

  case PAIR_n :
    break;

  case PAIR_S_n :
    break;

  case POSSIBLE_n :
    break;

  case PRED_n :
    break;

  case PRED_FIELD_n :
    break;

  case RULE_n :
    break;

  case RULES_S_n :
    break;

  case SOUS_CAT_n :
    break;

  case STRUCTURE_S_n :
    break;

  case TERME_CHAMP_n :
    /* la passe heritee assure que l'on a c,a... */
    son = VISITED->son;
    /* son->name == {TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */
    /* son->kind == ATOM_KIND */
    brother = son->brother;/* brother->name = {CHAINE_n, ENTIER_n, IDENT_n, PRED_n} */

    if (son->kind != ERROR_KIND && brother->kind != ERROR_KIND) {
      /* On suppose pour l'instant que l'on n'a que 2 composants */
      /* ($i path+)-obj */
      int dfield_id, suffix_id;
  
      if (dfield_pairs2field_id == NULL) {
	XxY_alloc (&dfield_pairs, "dfield_pairs", FIELDS_nb+1, 1, 0, 0, dfield_pairs_oflw, NULL);
	dfield_pairs2field_id = (int*) sxalloc (XxY_size (dfield_pairs)+1, sizeof (int));
	sxword_alloc (&suffix_names, "suffix_names", FIELDS_nb+1, 1, 8,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
      }

      current_atom_set = field_id2atom_set [son->id];

      atom_id = 0;
      field_kind = prev_field_kind = ERROR_KIND;

      suffix_id = sxword_2save (&suffix_names, sxstrget (brother->id), sxstrlen (brother->id));

      while ((atom_id = sxba_scan (current_atom_set, atom_id)) > 0) {
	varstr_raz (vstr);
	vstr = varstr_catenate (vstr, SXWORD_get (atom_names, atom_id));
	vstr = varstr_catchar (vstr, '-');
	vstr = varstr_catenate (vstr, sxstrget (brother->id));

	field_id = sxword_2retrieve (&field_names, varstr_tostr (vstr), varstr_length (vstr));

	if (field_id == ERROR_STE) {
	  sxput_error (VISITED->token.source_index,
		       "%sUnknown dynamic field name \"%s\": ignored.",
		       sxtab_ptr->err_titles [2],
		       varstr_tostr (vstr));
	  /* pour avoir un message unique!! */
	  field_id = sxword_2save (&field_names, varstr_tostr (vstr), varstr_length (vstr));

	  /* Attention, la taille de field_id2kind n'est + bonne !! */
	  if (field_id > FIELDS_nb) {
	    field_id2atom_set = sxbm_resize (field_id2atom_set, FIELDS_nb+1, 2*FIELDS_nb+1, SXWORD_size (atom_names)+1);
	    FIELDS_nb *= 2;
	    /* prudence */
	    field_id2kind = (unsigned char *) sxrealloc (field_id2kind, FIELDS_nb+1, sizeof (unsigned char));
	    /* Il faut reinitialiser current_atom_set */
	    current_atom_set = field_id2atom_set [son->id];
	  }

	  field_id2kind [field_id] = ERROR_KIND;
	}
	else {
	  SXBA_1_bit (used_field_set, field_id);
	  field_kind = field_id2kind [field_id];

	  if (field_kind != ERROR_KIND) {
	    if (prev_field_kind == ERROR_KIND /* 1er non errone' */
		|| prev_field_kind == field_kind) {
	      if (!XxY_set (&dfield_pairs, atom_id, suffix_id, &dfield_id))
		dfield_pairs2field_id [dfield_id] = field_id;

	      prev_field_kind = field_kind;
	    }
	    else {
	      if (prev_field_kind != ERROR_KIND)
		sxput_error (VISITED->token.source_index,
			     "%sThe types of this dynamic field name \"%s\" is incompatible: ignored.",
			     sxtab_ptr->err_titles [2],
			     varstr_tostr (vstr));
	    }
	  }
	}
      }

      brother->id = suffix_id;
      VISITED->kind = field_kind;
      VISITED->id = field_id; /* Pb, le nom est dynamique, j'en prend 1!! */
    }
    break;

  case TERME_CHAMP_RE_n :
    /* la passe heritee assure que ce noeud est toujours en erreur !! */
    VISITED->kind = ERROR_KIND;
    break;

  case TERME_CHEMIN_n :
  case TERME_CHEMIN_DOT_n :
    son = VISITED->son;
    /* son->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */
    brother = son->brother;
    /* brother->name = {CHAINE_n, ENTIER_n, IDENT_n, PRED_n, TERME_CHAMP_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */

    if (brother->name == TERME_CHEMIN_n || brother->name == TERME_CHEMIN_DOT_n) {
      /* Cas "$i path1* ($j path2+)" */
      /* OPERATOR_MINUS avec 2eme operande vide le 07/07/05 */
      /* ($j path2+) doit designer un champ atomique a valeurs finies dont toutes les valeurs sont des noms
	 de champ ayant le meme type (ce sera le type du resultat) */
      int           dfield_id, suffix_id;

      if (brother->kind != ATOM_KIND) {
	sxput_error (brother->token.source_index,
		     "%sMust be an atomic type: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	break;
      }
  
      if (dfield_pairs2field_id == NULL) {
	XxY_alloc (&dfield_pairs, "dfield_pairs", FIELDS_nb+1, 1, 0, 0, dfield_pairs_oflw, NULL);
	dfield_pairs2field_id = (int*) sxalloc (XxY_size (dfield_pairs)+1, sizeof (int));
	sxword_alloc (&suffix_names, "suffix_names", FIELDS_nb+1, 1, 8,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
      }

      current_atom_set = field_id2atom_set [brother->id];

      atom_id = 0;
      prev_field_kind = field_kind = ERROR_KIND;

      suffix_id = EMPTY_STE; /* 2eme operande vide */

      while ((atom_id = sxba_scan (current_atom_set, atom_id)) >= 0) {
	field_id = sxword_2retrieve (&field_names, 
				     SXWORD_get (atom_names, atom_id),
				     SXWORD_len (atom_names, atom_id));

	if (field_id == ERROR_STE) {
	  sxput_error (son->token.source_index,
		       "%sUnknown dynamic field name \"%s\": ignored.",
		       sxtab_ptr->err_titles [2],
		       SXWORD_get (atom_names, atom_id));
	  /* pour avoir un message unique!! */
	  field_id = sxword_2save (&field_names, varstr_tostr (vstr), varstr_length (vstr));

	  /* Attention, la taille de field_id2kind n'est + bonne !! */
	  if (field_id > FIELDS_nb) {
	    field_id2atom_set = sxbm_resize (field_id2atom_set, FIELDS_nb+1, 2*FIELDS_nb+1, SXWORD_size (atom_names)+1);
	    FIELDS_nb *= 2;
	    /* prudence */
	    field_id2kind = (unsigned char *) sxrealloc (field_id2kind, FIELDS_nb+1, sizeof (unsigned char));
	    /* Il faut reinitialiser current_atom_set */
	    current_atom_set = field_id2atom_set [brother->id];
	  }

	  field_id2kind [field_id] = ERROR_KIND;
	}
	else {
	  SXBA_1_bit (used_field_set, field_id);
	  field_kind = field_id2kind [field_id];

	  if (field_kind != ERROR_KIND) {
	    if (prev_field_kind == ERROR_KIND /* 1er non errone' */
		|| prev_field_kind == field_kind) {
	      if (!XxY_set (&dfield_pairs, atom_id, suffix_id, &dfield_id))
		dfield_pairs2field_id [dfield_id] = field_id;

	      prev_field_kind = field_kind;
	    }
	    else {
	      if (prev_field_kind != ERROR_KIND) {
		sxput_error (son->token.source_index,
			     "%sThe value \"%s\" of this atomic type must all be field names of the same type: ignored.",
			     sxtab_ptr->err_titles [2],
			     SXWORD_get (atom_names, atom_id));
		field_kind = ERROR_KIND;
		VISITED->kind = ERROR_KIND;
		break;
	      }
	    }
	  }
	}
      }

      if (atom_id < 0) {
	VISITED->kind = field_kind;
	VISITED->id = field_id; /* Pb, le nom est dynamique, j'en prend 1!! */
      }
    }
    else {
      /* Le cas brother->name == TERME_CHAMP_n est traite' ici aussi */
      VISITED->kind = brother->kind;
      VISITED->id = brother->id;
    }
    break;

  case TERME_CHEMIN_RE_n :
  case TERME_CHEMIN_RE_DOT_n :
    son = VISITED->son;
    /* son->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
    brother = son->brother;
    /* brother->name = {CHAINE_n, ENTIER_n, IDENT_n, KLEENE_n, NODE_REF_n, OU_S_n, PRED_n, TERME_CHAMP_n, 
       TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */

    if (brother->name == TERME_CHEMIN_n
	|| brother->name == TERME_CHEMIN_DOT_n
	|| brother->name == TERME_CHEMIN_RE_n
	|| brother->name == TERME_CHEMIN_RE_DOT_n) {
      /* A FAIRE si TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n, brother->id ne designe pas toutes les possibilites de l'ER !! */
      /* Cas "$i path1* ($j path2+)" */
      /* ($j path2+) doit designer un champ atomique a valeurs finies dont toutes les valeurs sont des noms
	 de champ ayant le meme type (ce sera le type du resultat) */
      if (brother->kind != ATOM_KIND) {
	sxput_error (brother->token.source_index,
		     "%sMust be an atomic type: ignored.",
		     sxtab_ptr->err_titles [2]);
	VISITED->kind = ERROR_KIND;
	break;
      }

      current_atom_set = field_id2atom_set [brother->id];
      field_kind = ERROR_KIND;

      atom_id = 0;

      while ((atom_id = sxba_scan (current_atom_set, atom_id)) >= 0) {
	field_id = sxword_2retrieve (&field_names, 
				     SXWORD_get (atom_names, atom_id),
				     SXWORD_len (atom_names, atom_id));

	if (field_id != ERROR_STE) {
	  SXBA_1_bit (used_field_set, field_id);

	  if (field_kind == ERROR_KIND)
	    field_kind = field_id2kind [field_id];
	  else {
	    if (field_kind != field_id2kind [field_id])
	      field_id = ERROR_STE;
	  }
	}

	if (field_id == ERROR_STE) {
	  sxput_error (brother->token.source_index,
		       "%sThe values of this atomic type must be field names of the same type: ignored.",
		       sxtab_ptr->err_titles [2]);
	  VISITED->kind = ERROR_KIND;
	  break;
	}
      }

      if (atom_id < 0) {
	VISITED->kind = field_kind;
	VISITED->id = field_id; /* Pb, le nom est dynamique, j'en prend 1!! */
      }
    }
    else {
      /* Le cas brother->name == TERME_CHAMP_n est traite' ici aussi */
      VISITED->kind = brother->kind;
      VISITED->id = brother->id;
    }

    break;

  case TERMINAL_n :
    break;

  case UNIFIER_n :
    /* <unifier> = <lhs> <eq_oper> <rhs> ; "UNIFIER" */
    brother = VISITED->son;
    /* brother->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
    field_kind = brother->kind;

    if (field_kind != ERROR_KIND) {
      field_id = brother->id;
      brother = brother->brother;
      /* brother->name = {ADD_OPER_n, CONTRAINTE_OPER_n, DANS_OPER_n, OPTION_OPER_n, UNIF_OPER_n} */
      oper_name = brother->name;

      brother = brother->brother;
      /* brother->name = {ATOM_S_n, EMPTY_ADJUNCT_n, EMPTY_ATOM_n, EMPTY_STRUCT_n, LEXEME_NODE_REF_n, LEX_NODE_REF_n, NODE_REF_n, 
	       TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */

      if (oper_name == DANS_OPER_n) {
	if (!(field_kind == STRUCT_KIND && brother->kind == (STRUCT_KIND+UNBOUNDED_KIND))) {
	  sxput_error (VISITED->son->brother->token.source_index, 
		       "%sOperand types are incompatible with the inclusion operator.",
		       sxtab_ptr->err_titles [2]);
	  break;
	}

#if 0
	/* Nelle version, on peut avoir du $i */
	if (doli != 0) {
	  sxput_error (brother->token.source_index, 
		       "%s\"$$\" is mandatory in this RHS.",
		       sxtab_ptr->err_titles [2]);
	  break;
	}
#endif /* 0 */
      }
      else {
	if (brother->name == ATOM_S_n) {
	  /* Utilisation d'un atome comme valeur d'un champ */
	  unsigned long int atom_value;
	  int               lfg_pd_local_atom_id;
	      
#ifndef ESSAI
	  if ((field_kind & UNBOUNDED_KIND) && brother->degree > 1) {
	    sxput_error (brother->token.source_index, 
			 "%sImplementation restriction: disjunctions are not allowed on unbounded atomic types.",
			 sxtab_ptr->err_titles [2]);
	    break;
	  }
#endif /* ESSAI */

	  if ((field_kind & ATOM_KIND) == 0) {
	    sxput_error (brother->token.source_index,
			 "%sMust be an atom value: ignored.",
			 sxtab_ptr->err_titles [2]);
	    atom_value = 0UL;
	  }
	  else {
	    /* <atom_LIST> = <atom_LIST> "|" <atome> ;
	       <atom_LIST> = <atome> ; "ATOM_S" */
#ifndef ESSAI
	    current_atom_set = field_id2atom_set [field_id];
	    atom2local_id = fieldXatom2local_id [field_id];
	    atom_value = 0;

	    if (brother->degree > 1) {
	      /* ... disjonction non triviale */
	      /* On verifie */
	      /* On suppose que tous les composants d'une disjonction sont aussi des atomes simples ... */
	      for (son = brother->son; son != NULL; son = son->brother) {
		atom_id = son->id;

		if (!SXBA_bit_is_set (current_atom_set, atom_id)) {
		  sxput_error (son->token.source_index,
			       "%sUndeclared atom: ignored.",
			       sxtab_ptr->err_titles [2]);
		  lfg_pd_local_atom_id = atom_id = ERROR_STE;
		}
		else
		  lfg_pd_local_atom_id = atom2local_id [atom_id];

		atom_value |= (1 << lfg_pd_local_atom_id);
	      }
	    }
	    else {
	      atom_id = brother->son->id;

	      if (!SXBA_bit_is_set (current_atom_set, atom_id)) {
		sxput_error (brother->son->token.source_index,
			     "%sUndeclared atom: ignored.",
			     sxtab_ptr->err_titles [2]);
		atom_value = ERROR_STE;
	      }
	      else {
		/* Les codes des atomes des types non bornes definis ds la grammaire doivent figurer ds
		   la def du type : {a, b, c ...}" */
		/* Ce n'est pas le cas de ceux du niveau lexical */
		atom_value = (field_kind & UNBOUNDED_KIND) ? atom_id : (1 << atom2local_id [atom_id]);
	      }
	    }
#else /* ESSAI */
	    current_atom_set = field_id2atom_set [field_id];

	    atom_id = 0;

	    for (son = brother->son; son != NULL; son = son->brother) {
	      if (son->name != ATOM_CONCAT_n) {
		atom_id = son->id;

		if (!SXBA_bit_is_set (current_atom_set, atom_id)) {
		  sxput_error (son->token.source_index,
			       "%sUndeclared atom: ignored.",
			       sxtab_ptr->err_titles [2]);
		  lfg_pd_local_atom_id = atom_id = ERROR_STE;
		}
		else {
		  SXBA_1_bit (working_atom_set, atom_id);
		}
	      }
	    }

	    if (atom_id) { /* sinon il n'y a que des atomes complexes de type (chemin)-suffixe */
	      atom_id = 0;
	      while ((atom_id = sxba_scan_reset (working_atom_set, atom_id)) > 0)
		XH_push (atom_hd, atom_id);

	      XH_set (&atom_hd, &atom_value);
	    } else
	      atom_value=0;
#endif /* ESSAI */
	  }

	  /* !!
	  if (atom_value & 1)
	    break;
	  */

	  brother->id = atom_value;
	  brother->kind = field_kind;
	}
	else {
	  if (brother->name == NODE_REF_n || brother->name == EMPTY_STRUCT_n) {
	    /* field_kind doit reperer une structure */
	    if (field_kind != STRUCT_KIND) {
	      sxput_error (VISITED->son->token.source_index, 
			   "%sThis LHS operand must denote an f-structure.",
			   sxtab_ptr->err_titles [2]);
	      break;
	    }
	  }
	  else {
	    if (brother->name == EMPTY_ATOM_n) {
	      /* field_kind doit reperer un atome */
	      if (field_kind != ATOM_KIND) {
		sxput_error (VISITED->son->token.source_index, 
			     "%sThis LHS operand must denote an atom field.",
			     sxtab_ptr->err_titles [2]);
		break;
	      }
	    }
	    else {
	      if (brother->name == EMPTY_ADJUNCT_n) {
		/* field_kind doit reperer un adjunct */
		if (field_kind != STRUCT_KIND+UNBOUNDED_KIND) {
		  sxput_error (VISITED->son->token.source_index, 
			       "%sThis LHS operand must denote an adjunct field.",
			       sxtab_ptr->err_titles [2]);
		  break;
		}
	      }
	      else {
		if (brother->name == LEXEME_NODE_REF_n || brother->name == LEX_NODE_REF_n) {
		  if (oper_name == CONTRAINTE_OPER_n || oper_name == DANS_OPER_n || oper_name == OPTION_OPER_n || field_kind != STRUCT_KIND) {
		    sxput_error (VISITED->son->token.source_index, 
				 "%sIllegal usage of the \"lex\" or \"lexeme\" operator.",
				 sxtab_ptr->err_titles [2]);
		    break;
		  }
		}
		else {
		  if (field_kind != brother->kind) {
		    sxput_error (VISITED->son->brother->token.source_index, 
				 "%sThe LHS and RHS types are incompatible.",
				 sxtab_ptr->err_titles [2]);
		    break;
		  }

		  if (field_kind & ATOM_KIND) {
		    if (field_id != brother->id) {
		      /* On regarde si ces 2 champs sont du meme "type" */
#ifndef ESSAI
		      if (field_kind != ATOM_KIND /* UNBOUNDED */
			  || !check_identical_atom_type (field_id, brother->id)) {
			sxput_error (VISITED->son->brother->token.source_index, 
				     "%sThe LHS and RHS types are incompatible.",
				     sxtab_ptr->err_titles [2]);
			break;
		      }
#else /* ESSAI */
		      /* Les bases doivent etre identiques */
		      if (!check_identical_atom_type (field_id, brother->id)) {
			sxput_error (VISITED->son->brother->token.source_index, 
				     "%sThe LHS and RHS types are incompatible.",
				     sxtab_ptr->err_titles [2]);
			break;
		      }
#endif /* ESSAI */
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
    last_lhs_atom_field_id = 0;
    break;

  case UNIF_OPER_n :
    break;

  case VOCABULARY_S_n :
    rhs_length = VISITED->degree-1;

    if (rhs_length > maxrhs)
      maxrhs = rhs_length;

    break;

  case VOID_n :
    break;



    /*
      Z Z Z Z
    */

  default:
    break;
  }
  /* end lfg_pd */
}


/* ************************************************************************************** */
/* Passe ou on fait la generation */

#define OPERATOR_ERROR      0X80000000
#define OPERATOR_SPACE      -1
#define OPERATOR_MINUS      -2
#define OPERATOR_DOT        -3
#define OPERATOR_REF        -4
#define OPERATOR_ATOM       -5
#define OPERATOR_UNIFY      -6
#define OPERATOR_CONSTRAINT -7
#define OPERATOR_IN         -8
#define OPERATOR_KLEENE     -9
#define OPERATOR_OR         -10
#define OPERATOR_POSSIBLE   -11
#define OPERATOR_FIELD      -12
#define OPERATOR_ETIQUETTE  -13
#define OPERATOR_RE         -14
#define OPERATOR_EXIST      -15
#define OPERATOR_NONEXIST   -16
#define OPERATOR_IDENTITY   -17
#define OPERATOR_LEX_REF    -18
#define OPERATOR_EMPTY_STRUCT    -19
#define OPERATOR_ADD        -20
#define OPERATOR_LEXEME_REF -21
#define OPERATOR_OPTIONAL   -22
#define OPERATOR_COMPLEX_ATOM       -23
#define OPERATOR_ATOM_MINUS      -24

#define OPERATOR_string "\
#define OPERATOR_ERROR      0X80000000\n\
#define OPERATOR_SPACE      -1\n\
#define OPERATOR_MINUS      -2\n\
#define OPERATOR_DOT        -3\n\
#define OPERATOR_REF        -4\n\
#define OPERATOR_ATOM       -5\n\
#define OPERATOR_UNIFY      -6\n\
#define OPERATOR_CONSTRAINT -7\n\
#define OPERATOR_IN         -8\n\
#define OPERATOR_KLEENE     -9\n\
#define OPERATOR_OR         -10\n\
#define OPERATOR_POSSIBLE   -11\n\
#define OPERATOR_FIELD      -12\n\
#define OPERATOR_ETIQUETTE  -13\n\
#define OPERATOR_RE         -14\n\
#define OPERATOR_EXIST      -15\n\
#define OPERATOR_NONEXIST   -16\n\
#define OPERATOR_IDENTITY   -17\n\
#define OPERATOR_LEX_REF    -18\n\
#define OPERATOR_EMPTY_STRUCT    -19\n\
#define OPERATOR_ADD        -20\n\
#define OPERATOR_LEXEME_REF -21\n\
#define OPERATOR_OPTIONAL   -22\n\
#define OPERATOR_COMPLEX_ATOM   -23\n\
#define OPERATOR_ATOM_MINUS     -24\n\
"

static char *operator_code2name [] = {
"OPERATOR_ERROR",
"OPERATOR_SPACE",
"OPERATOR_MINUS",
"OPERATOR_DOT",
"OPERATOR_REF",
"OPERATOR_ATOM",
"OPERATOR_UNIFY",
"OPERATOR_CONSTRAINT",
"OPERATOR_IN",
"OPERATOR_KLEENE",
"OPERATOR_OR",
"OPERATOR_POSSIBLE",
"OPERATOR_FIELD",
"OPERATOR_ETIQUETTE",
"OPERATOR_RE",
"OPERATOR_EXIST",
"OPERATOR_NONEXIST",
"OPERATOR_IDENTITY",
"OPERATOR_LEX_REF",
"OPERATOR_EMPTY_STRUCT",
"OPERATOR_ADD",
"OPERATOR_LEXEME_REF",
"OPERATOR_OPTIONAL",
"OPERATOR_COMPLEX_ATOM",
"OPERATOR_ATOM_MINUS",
};
				       
#if 0
/* %include "sxstack.h" */
#endif /* 0 */

static XH_header       code_stack, equation_stack, prod_hd;
static SXBA            code_set, operator_in_set, default_set;
static sxword_header   t_names, nt_names;
static int             *nt_names2nt, *nt2nt_names;
static int             *rule2equation, *rule2next_rule, *prod2rule, *new_prod2rule;
static int             rule_nb;
static FILE            *output_file;
static BOOLEAN         has_optional;


static void
code_stack_oflw (old_size, new_size)
     int old_size, new_size;
{
  code_set = sxba_resize (code_set, new_size+1);
  operator_in_set = sxba_resize (operator_in_set, new_size+1);
}

static void
prod_hd_oflw (old_size, new_size)
     int old_size, new_size;
{
  prod2rule = (int*) sxrealloc (prod2rule, new_size+1, sizeof (int));
}

static void
new_prod_hd_oflw (old_size, new_size)
     int old_size, new_size;
{
  new_prod2rule = (int*) sxrealloc (new_prod2rule, new_size+1, sizeof (int));
}

/* on vient de generer la lhs et la rhs d'une equation ds code_stack, on verifie qu'elles different */
static BOOLEAN
lhs_equal_rhs_code ()
{
  int top, bot1, bot2, l;

  top = *code_stack.list;
  bot1 = code_stack.display [code_stack.display->lnk].X;

  if ((l = top-bot1) & 1)
    /* taille impaire ... */
    return FALSE;

  bot2 = bot1+(l>>1);

  do {
    if (XH_list_elem (code_stack, bot1) != XH_list_elem (code_stack, bot2))
      return FALSE;
  } while (++bot1, ++bot2 < top);

  return TRUE;
}

static int main_operator;

static BOOLEAN
gen_code (visited)
     NODE *visited;
{
  int     x, prod_no, id, i, ldoli, rdoli, top;
  NODE    *son, *brother;
  char    *name;
  static int complex_atom_cardinal;

  switch (visited->name) {
  case ADD_OPER_n :
    return FALSE;

  case ATOM_CONCAT_n :
    son = visited->son; /* son->name == {TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */ /* son->kind == ATOM_KIND */

    if (!gen_code (son))
      return FALSE;

    brother = son->brother;/* brother->name = {CHAINE_n, ENTIER_n, IDENT_n, PRED_n} */
    XH_push (code_stack, brother->id);

    XH_push (code_stack, OPERATOR_ATOM_MINUS);
    complex_atom_cardinal++;

    return TRUE;

  case ATOM_MINUS_n :
    return FALSE;

  case ATOM_PLUS_n :
    return FALSE;

  case ATOM_S_n :
    for (son = visited->son; son != NULL; son = son->brother)
      if (son->name == ATOM_CONCAT_n)
	gen_code (son);

    if (visited->id) {
      XH_push (code_stack, visited->id); /* (disjonction of) local atomic value */
      
      if (visited->kind == ATOM_KIND)
	XH_push (code_stack, visited->father->son->id); /* field_id */
      else /* visited->kind == UNBOUNDED_KIND[+Aij_KIND] */
	XH_push (code_stack, 0); /* field_id */
      
      XH_push (code_stack, OPERATOR_ATOM);
    }

    if (complex_atom_cardinal) {
      XH_push (code_stack, complex_atom_cardinal + (visited->id ? 1 : 0) );
      XH_push (code_stack, OPERATOR_COMPLEX_ATOM);
      complex_atom_cardinal = 0;
    }

    return TRUE;

  case CHAINE_n :
    return FALSE;

  case CONTRAINTE_OPER_n :
    return FALSE;

  case DANS_OPER_n :
    return FALSE;

  case EMPTY_ADJUNCT_n :
  case EMPTY_ATOM_n :
  case EMPTY_STRUCT_n :
    XH_push (code_stack, OPERATOR_EMPTY_STRUCT); /* Regroupe les 3 cas ... !! */
    return TRUE;

  case ENTIER_n :
    return FALSE;

  case EQUATION_S_n :
    /* Attention code_set peut deja contenir la semantique par defaut due aux noms commencant par "_" */
    for (son = visited->son; son != NULL; son = son->brother) {
      if (gen_code (son)) {
	/* Le code de l'equation courante de rule_nb se trouve ds code_stack */
	XH_set (&code_stack, &x);
#if 0
	x = XH_X (code_stack, x+1)-1; /* top */
#endif /* 0 */

	if (main_operator == OPERATOR_IN)
	  /* On va les generer en dernier */
	  SXBA_1_bit (operator_in_set, x);
      }
      else {
	x = 0;
	XH_raz (code_stack);
      }
      
      if (!SXBA_bit_is_reset_set (code_set, x) && x != 0) {
	sxput_error (son->token.source_index, 
		     "%sDuplicate equation: ignored.",
		     sxtab_ptr->err_titles [1]);
      }
    }

    x = -1;

    while ((x = sxba_scan_reset (code_set, x)) >= 0) {
      if (!SXBA_bit_is_set (operator_in_set, x))
	XH_push (equation_stack, x);
    }

    /* On genere les in en dernier ... */
    x = -1;

    while ((x = sxba_scan_reset (operator_in_set, x)) >= 0) {
	XH_push (equation_stack, x);
    }

    XH_push (equation_stack, -1); /* Marqueur de fin */
    XH_set (&equation_stack, rule2equation+rule_nb);

    return TRUE;

  case ETIQUETTE_n :
    son = visited->son; /* son->name = NODE_REF_n */
    brother = son->brother; /* brother->name = {ETIQUETTE_n, EXISTE_n, NON_n, NONEXISTE_n, POSSIBLE_n, UNIFIER_n} */

    if (!gen_code (brother))
      return FALSE;

    rdoli = doli;

    if (!gen_code (son))
      return FALSE;

    doli = rdoli;

    XH_push (code_stack, OPERATOR_ETIQUETTE);
    return TRUE;

  case EXISTE_n :
    son = visited->son; /* son->name = {TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */

    if (!gen_code (son))
      return FALSE;
    
    if (son->name == TERME_CHAMP_RE_n || son->name == TERME_CHEMIN_RE_n || son->name == TERME_CHEMIN_RE_DOT_n)
      XH_push (code_stack, OPERATOR_RE);

    XH_push (code_stack, OPERATOR_EXIST);

    return TRUE;

  case IDENT_n :
    return FALSE;

  case KLEENE_n :
    if (!gen_code (visited->son))
      return FALSE;

    XH_push (code_stack, OPERATOR_KLEENE);

    return TRUE;

  case LEXEME_NODE_REF_n :
    i = atoi (sxstrget (visited->token.string_table_entry));
      
    XH_push (code_stack, i); /* i de $i (i>0 est deja ve'rifie') */
    XH_push (code_stack, OPERATOR_LEXEME_REF); /* lex $i */

    is_OPERATOR_LEXEME_REF = TRUE;

    if (doli == -1)
      doli = i;

    return TRUE;

  case LEX_NODE_REF_n :
    i = atoi (sxstrget (visited->token.string_table_entry));
      
    XH_push (code_stack, i); /* i de $i (i>0 est deja ve'rifie') */
    XH_push (code_stack, OPERATOR_LEX_REF); /* lex $i */

    is_OPERATOR_LEX_REF = TRUE;

    if (doli == -1)
      doli = i;

    return TRUE;

  case MOT_1COL_n :
    return FALSE;

  case NODE_REF_n :
    i = atoi (sxstrget (visited->token.string_table_entry));

    if (i > 0 && visited->father->name != ETIQUETTE_n && SXBA_bit_is_set (default_set, i)) {
      sxput_error (visited->token.source_index, 
		   "%sConflict with default: ignored.",
		   sxtab_ptr->err_titles [1]);
      return FALSE;
    }
      
    XH_push (code_stack, i); /* i de $i (i==$) */
    XH_push (code_stack, OPERATOR_REF); /* $ de $i */

    if (doli == -1)
      doli = i;

    return TRUE;

  case NON_n :
    return FALSE;

  case NONEXISTE_n :
    son = visited->son; /* son->name = {TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */

    if (!gen_code (son))
      return FALSE;
    
    if (son->name == TERME_CHAMP_RE_n || son->name == TERME_CHEMIN_RE_n || son->name == TERME_CHEMIN_RE_DOT_n)
      XH_push (code_stack, OPERATOR_RE);

    XH_push (code_stack, OPERATOR_NONEXIST);

    return TRUE;

  case OPTION_OPER_n :
    return FALSE;

  case OU_S_n :
    son = visited->son;/* son->name = {CHAINE_n, ENTIER_n, IDENT_n, KLEENE_n, OU_S_n, PRED_n, TERME_CHAMP_n, 
			  TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */

    if (son->degree != 0) {
      if (!gen_code (son))
	return FALSE;
    }
    else {
      XH_push (code_stack, son->id);
      XH_push (code_stack, OPERATOR_FIELD);
    }

    while ((son = son->brother) != NULL) {
      if (son->degree != 0) {
	if (!gen_code (son))
	  return FALSE;
      }
      else {
	XH_push (code_stack, son->id);
	XH_push (code_stack, OPERATOR_FIELD);
      }

      XH_push (code_stack, OPERATOR_OR);
    }

    return TRUE;

  case POSSIBLE_n :
    son = visited->son; /* son->name = {EXISTE_n, NON_n, NONEXISTE_n, UNIFIER_n} */

    if (!gen_code (son))
      return FALSE;

    XH_push (code_stack, OPERATOR_POSSIBLE);

    return TRUE;

  case PRED_n :
    return  FALSE;

  case RULE_n :
    rule_nb++;
    son = visited->son; /* son->name == VOCABULARY_S_n */
    brother = son->brother; /* brother->name == EQUATION_S_n */
    sxba_empty (default_set);

    for (son = son->son; son != NULL; son = son->brother) {
      /* son->position == 1 => son->name = MOT_1COL_n */
      /* son->position > 1 => son->name = {IDENT_n, OPT_NON_TERMINAL_n} */
      if (son->name == OPT_NON_TERMINAL_n) {
	XH_push (prod_hd, 0); /* Marque l'option!! */
	has_optional = TRUE;
      }
	
      name = sxstrget (son->token.string_table_entry);

      if (islower (*name)) {
	/* terminal */
	id = -(sxword_save (&t_names, name)-1);
      }
      else {
	/* non-terminal */
	id = (sxword_save (&nt_names, name)-1);

	if (*name == '_' && son->position > 1) {
	  /* Merci Lionel ... */
	  /* La semantique par defaut de ces nt de la rhs est $$ = $i */
	  if (visited->son->degree == 2 /* A -> _B */
	      && brother->degree == 0 /* Pas de sem explicite */) {
	    /* $$ = $1 sera donc la seule equation du bloc */
	    XH_push (code_stack, OPERATOR_IDENTITY);
	  }
	  else {
	    XH_push (code_stack, son->position-1);
	    XH_push (code_stack, OPERATOR_REF); /* $i */
	    XH_push (code_stack, 0);
	    XH_push (code_stack, OPERATOR_REF); /* $$ */
	    XH_push (code_stack, 2); /* taille du code $$ */ 
	    XH_push (code_stack, OPERATOR_UNIFY);
	  }

	  XH_set (&code_stack, &x);
	  SXBA_1_bit (code_set, x);
	  SXBA_1_bit (default_set, son->position-1);
	}
      }

      XH_push (prod_hd, id);
    }

    rule2next_rule [rule_nb] = (XH_set (&prod_hd, &prod_no)) ? prod2rule [prod_no] : 0; 

    prod2rule [prod_no] = rule_nb;

    return gen_code (brother /* EQUATION_S_n */);

  case RULES_S_n :
    for (son = visited->son; son != NULL; son = son->brother)
      gen_code (son);

    return TRUE;

  case TERME_CHAMP_n :
  case TERME_CHAMP_RE_n :
    son = visited->son; /* son->name == {TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */ /* son->kind == ATOM_KIND */

    if (!gen_code (son))
      return FALSE;

    brother = son->brother;/* brother->name = {CHAINE_n, ENTIER_n, IDENT_n, PRED_n} */
    XH_push (code_stack, brother->id);

    XH_push (code_stack, OPERATOR_MINUS);
    XH_push (code_stack, OPERATOR_FIELD);

    return TRUE;

  case TERME_CHEMIN_n :
  case TERME_CHEMIN_DOT_n :
    /* son->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */
    /* brother->name = {CHAINE_n, ENTIER_n, IDENT_n, PRED_n, TERME_CHAMP_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n} */
  case TERME_CHEMIN_RE_n :
  case TERME_CHEMIN_RE_DOT_n :
    /* son->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
    /* brother->name = {CHAINE_n, ENTIER_n, IDENT_n, KLEENE_n, OU_S_n, PRED_n, TERME_CHAMP_n, 
       TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */

    son = visited->son;
    brother = son->brother;

    if (brother->degree != 0) {
      if (!gen_code (brother))
	return FALSE;

      /* Le 07/07/05 */
      if (brother->name == TERME_CHEMIN_n || brother->name == TERME_CHEMIN_DOT_n ||
	  brother->name == TERME_CHEMIN_RE_n || brother->name == TERME_CHEMIN_RE_DOT_n) {
	/* On est ds le cas $i X+ ($j Y+) avec son == $i X+ et brother == $j Y+ */
	/* Il faut transformer ($j Y+) en ($j Y+)-"" (operateur minus avec 2eme champ vide) */
	XH_push (code_stack, EMPTY_STE); /* id du vide */
	XH_push (code_stack, OPERATOR_MINUS);
	XH_push (code_stack, OPERATOR_FIELD); /* Le resultat est un champ */
      }
    }
    else {
      XH_push (code_stack, brother->id);
      XH_push (code_stack, OPERATOR_FIELD);
    }

    doli = -1;

    if (!gen_code (son))
      return FALSE;

    if (visited->name == TERME_CHEMIN_n || visited->name == TERME_CHEMIN_RE_n)
      XH_push (code_stack, OPERATOR_SPACE);
    else
      XH_push (code_stack, OPERATOR_DOT);

    return TRUE;

  case UNIFIER_n :
    /* <unifier> = <lhs> <eq_oper> <rhs> ; "UNIFIER" */
    son = visited->son; /* son->name = {NODE_REF_n, TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n,
			   TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
    brother = son->brother; /* brother->name = {ADD_OPER_n, CONTRAINTE_OPER_n, DANS_OPER_n, OPTION_OPER_n, UNIF_OPER_n} */

    /* On genere la RHS en 1er pour avoir la LHS juste sous OPERATOR_IN!! */
    doli = -1;

    if (!gen_code (brother->brother) /* RHS */)
      return FALSE;

    rdoli = doli;
      
    if (brother->brother->name == TERME_CHAMP_RE_n || brother->brother->name == TERME_CHEMIN_RE_n || brother->brother->name == TERME_CHEMIN_RE_DOT_n)
      XH_push (code_stack, OPERATOR_RE);

      /* brother->brother->name = {ATOM_S_n, EMPTY_ADJUNCT_n, EMPTY_ATOM_n, EMPTY_STRUCT_n, LEXEME_NODE_REF_n, LEX_NODE_REF_n, NODE_REF_n, 
	 TERME_CHAMP_n, TERME_CHAMP_RE_n, TERME_CHEMIN_n, TERME_CHEMIN_DOT_n, TERME_CHEMIN_RE_n, TERME_CHEMIN_RE_DOT_n} */
    /* Pour reperer le debut du code de la RHS */
    top = XH_list_top (code_stack);
    doli = -1;

    if (gen_code (son) /* LHS */) {
      if (lhs_equal_rhs_code ()) {
	sxput_error (brother->token.source_index, 
		     "%slhs and rhs are identical: ignored.",
		     sxtab_ptr->err_titles [1]);
	return FALSE;
      }

      ldoli = doli;

#if 0
      /* Nelle version : on peut modifier les fils */
      if (brother->name == UNIF_OPER_n && ldoli > 0 && rdoli != 0) {
	sxput_error (son->token.source_index, 
		     "%sillegal RHS update: equation ignored.",
		     sxtab_ptr->err_titles [1]);
	return FALSE;
      }
#endif /* 0 */

      if (son->name == TERME_CHAMP_RE_n || son->name == TERME_CHEMIN_RE_n || son->name == TERME_CHEMIN_RE_DOT_n)
	XH_push (code_stack, OPERATOR_RE);

      top = XH_list_top (code_stack)-top; /* taille du code de la LHS */
      XH_push (code_stack, top);

      if (brother->name == UNIF_OPER_n) {
	main_operator = OPERATOR_UNIFY;
      }
      else {
	if (brother->name == CONTRAINTE_OPER_n) {
	  main_operator = OPERATOR_CONSTRAINT;
	}
	else {
	  if (brother->name == DANS_OPER_n) {
	    main_operator = OPERATOR_IN;
	  }
	  else {
	    if (brother->name == OPTION_OPER_n)
	      main_operator = OPERATOR_OPTIONAL;
	    else
	      /* brother->name == ADD_OPER_n */
	      main_operator = OPERATOR_ADD;
	  }
	}
      }

      XH_push (code_stack, main_operator);
      

      return TRUE;
    }

    return FALSE;

  case UNIF_OPER_n :
    return FALSE;

  default:
    return FALSE;
  }
}


static void
output_vocabulary_header ()
{
  long	date_time;
    
  date_time = time (0);

  fprintf (vocabulary_file,
	   "\
##########################################################################################\n\
#\tThis list of terminal and non-terminal symbols extracted from \"%s\" has been output to \"%s\"\n\
#\ton %s\
#\tby the SYNTAX(*) LFG processor\n\
##########################################################################################\n\
#\t(*) SYNTAX is a trademark of INRIA.\n\
##########################################################################################\n\n\n",
	   (path_name == NULL) ? "stdin" : path_name,
	   vocabulary_file_name,
	   ctime (&date_time));
}

static void
output_vocabulary ()
{
  /* Du fait de remove_optional (), l'ordre de t_names, nt_names n'est peut etre plus le bon... */
  int  prod, ntmax, tmax, prodmax, nt, t, bot, cur, top, X, field_id;
  SXBA nt_set, t_set;

  output_vocabulary_header ();

  ntmax = SXWORD_top (nt_names)-2;
  tmax = SXWORD_top (t_names)-2;
  prodmax = XH_top (prod_hd)-1;

  /* En fait, comme le code des nt ne sert a rien, on commente le genere'
     De plus il peut y avoir des nt de la forme "[A]" qui ne sont pas accepte's par le scanner du .lex !! */
  fputs ("\n\n\
######################################################\n\
# d�claration des cat�gories syntaxiques (auxiliaires)\n\
######################################################\n\
\n\
# CATEGORY\n", vocabulary_file);  
  
  nt_set = sxba_calloc (ntmax+1);
  nt = 0;

  for (prod = 1; prod <= prodmax; prod++) {
    bot = XH_X (prod_hd, prod);
    top = XH_X (prod_hd, prod+1);

    /* On parcourt la rhs */
    for (cur = bot; cur < top; cur++) {
      X = XH_list_elem (prod_hd, cur);

      if (X > 0 && SXBA_bit_is_reset_set (nt_set, X)) {
	nt++;
	/* nt_names2nt [X+1] = nt; calcul de'place' ds le calcul de complete_names */
	nt2nt_names [nt] = X+1;
	fprintf (vocabulary_file, "# /* %i */ %s;\n", nt, SXWORD_get (nt_names, X+1));
      }
    }
  }

  sxfree (nt_set);

  if (used_field_set && (field_id = sxba_scan (used_field_set, 0)) > 0) {
    /* ... Et on en profile pour utiliser CATEGORY pour autre chose ... */fputs ("\n\n\
################################################################\n\
# Liste des champs r�ellement utilis�s dans la partie syntaxique\n\
################################################################\n\
\n\
CATEGORY\n", vocabulary_file);

    do {
      fprintf (vocabulary_file, "\t%s;\n", SXWORD_get (field_names, field_id));
    } while ((field_id = sxba_scan (used_field_set, field_id)) > 0);
  }

  fputs ("\n\n\
###################################################\n\
# d�claration des cat�gories lexicales (terminales)\n\
###################################################\n\
\n\
TERMINAL\n", vocabulary_file);  
  
  t_set = sxba_calloc (tmax+1);
  t = 0;

  for (prod = 1; prod <= prodmax; prod++) {
    bot = XH_X (prod_hd, prod);
    top = XH_X (prod_hd, prod+1);

    /* On parcourt la rhs */
    for (cur = bot; cur < top; cur++) {
      X = XH_list_elem (prod_hd, cur);

      if (X < 0 && SXBA_bit_is_reset_set (t_set, -X))
	fprintf (vocabulary_file, "/* %i */ %s;\n", ++t, SXWORD_get (t_names, -X+1));
    }
  }

  sxfree (t_set);
}


static void
remove_optional ()
{
  SXBA      has_nt_opt, has_nt_non_opt, has_t_opt, *rhsntXprod, is_empty_nt, local_rhs_nt_set, line;
  XH_header new_prod_hd;
  int       *prod2non_empty_nb, *empty_nt_stack, *t2opt_t, *nt2opt_nt;
  int       ntmax, tmax, prodmax, non_empty_nb;
  int       prod, bot, lhsnt, top, cur, X, Y, nt; 
  int       new_prod, new_prodmax, doldol_nb, doldol_dol1, doldol_dol1_equation_set_id;
  int       *new_rule2equation, *new_rule2next_rule, *doldol_stack;

  ntmax = SXWORD_top (nt_names)-2;
  tmax = SXWORD_top (t_names)-2;
  prodmax = XH_top (prod_hd)-1;

  has_nt_opt = sxba_calloc (ntmax+1);
  has_nt_non_opt = sxba_calloc (ntmax+1);
  has_t_opt = sxba_calloc (tmax+1);
  rhsntXprod = sxbm_calloc (ntmax+1, prodmax+1);
  is_empty_nt = sxba_calloc (ntmax+1);
  prod2non_empty_nb = (int*) sxalloc (prodmax+1, sizeof (int));
  empty_nt_stack =  (int*) sxalloc (ntmax+1, sizeof (int)), empty_nt_stack [0] = 0;
  local_rhs_nt_set = sxba_calloc (ntmax+1);

  for (prod = 1; prod <= prodmax; prod++) {
    bot = XH_X (prod_hd, prod);
    lhsnt = XH_list_elem (prod_hd, bot);
    top = XH_X (prod_hd, prod+1);
    non_empty_nb = 0; /* tout ce qui n'est pas entre () */

    /* On parcourt la rhs */
    for (cur = bot+1; cur < top; cur++) {
      X = XH_list_elem (prod_hd, cur);

      if (X == 0)
	  /* opt */
	Y = XH_list_elem (prod_hd, ++cur);
      else
	/* non opt */
	Y = X;
	
      if (Y > 0) {
	/* nt */
	if (X == 0)
	  /* opt */
	  SXBA_1_bit (has_nt_opt, Y);
	else {
	  /* non_opt */
	  SXBA_1_bit (has_nt_non_opt, Y);
	  /* Y est en rhs de prod et ne derive pas (encore) ds le vide */
	  SXBA_1_bit (rhsntXprod [Y], prod);

	  if (SXBA_bit_is_reset_set (local_rhs_nt_set, Y))
	    /* On compte 1 seule occurrence des nt */
	    non_empty_nb++;
	}
      }
      else {
	if (X == 0)
	  /* opt */
	  SXBA_1_bit (has_t_opt, -Y);
	else
	  non_empty_nb++;
      }
    }

    if (non_empty_nb == 0) {
      /* La prod prod derive (directement) ds le vide */
      if (SXBA_bit_is_reset_set (is_empty_nt, lhsnt))
	empty_nt_stack [++*empty_nt_stack] = lhsnt;
    }
    else {
      /* Cette X-prod ne derive pas (encore) ds le vide */
      sxba_empty (local_rhs_nt_set);
    }

    prod2non_empty_nb [prod] = non_empty_nb;
  }
  
  sxfree (local_rhs_nt_set);

  /* Un nt A derive ds le vide si au moins une de ses A-prod derive ds le vide */
  while (empty_nt_stack [0] != 0) {
    nt = empty_nt_stack [(*empty_nt_stack)--]; /* pop */
    line = rhsntXprod [nt];

    prod = 0;

    while ((prod = sxba_scan (line, prod)) > 0) {
      /* nt intervient en rhs de prod */
      if (--prod2non_empty_nb [prod] == 0) {
	/* prod derive ds le vide */
	lhsnt = XH_list_elem (prod_hd, XH_X (prod_hd, prod));
      
	if (SXBA_bit_is_reset_set (is_empty_nt, lhsnt))
	  empty_nt_stack [++*empty_nt_stack] = lhsnt; /* push nouveau nt */
      }
    }
  }
  
  sxfree (empty_nt_stack);
  sxfree (prod2non_empty_nb);
  sxbm_free (rhsntXprod);

  XH_alloc (&new_prod_hd, "prod_hd", RULES_nb+1, 1, 10, new_prod_hd_oflw, NULL);
  new_prod2rule = (int*) sxalloc (XH_size (new_prod_hd)+1, sizeof (int));

  nt2opt_nt = (int*) sxcalloc (ntmax+1, sizeof (int));
  t2opt_t = (int*) sxcalloc (tmax+1, sizeof (int));
  
  doldol_nb = 0;

  for (prod = 1; prod <= prodmax; prod++) {
    bot = XH_X (prod_hd, prod);
    top = XH_X (prod_hd, prod+1);

    /* On parcourt la rhs */
    for (cur = bot; cur < top; cur++) {
      X = XH_list_elem (prod_hd, cur);

      if (X == 0) {
	/* opt */
	X = XH_list_elem (prod_hd, ++cur);

	if (X > 0) {
	  /* nt */
	    /* X =>+ epsilon, on remplace (X) par X */
	  if (!SXBA_bit_is_set (is_empty_nt, X)) {
	    /* Si toutes les occur de X en RHS sont de la forme (X), on
	       remplace (X) par X (et il faudra generer la regle "<X> = ;") */
	    if (SXBA_bit_is_set (has_nt_non_opt, X)) {
	      /* Il existe des occur de la forme X */
	      /* On remplace toutes les occur de la forme (X) par [X] et on generera + tard
		 les regles "<[X]> = ; <[X]> = <X> ; $$ = $1;" */
	      if ((Y = nt2opt_nt [X]) == 0) {
		varstr_raz (vstr);
		vstr = varstr_catchar (vstr, '[');
		vstr = varstr_catenate (vstr, SXWORD_get (nt_names, X+1));
		vstr = varstr_catchar (vstr, ']');
		vstr = varstr_complete (vstr);
		Y = sxword_2save (&nt_names, varstr_tostr (vstr), varstr_length (vstr))-1;
		nt2opt_nt [X] = Y;
		doldol_nb++;
	      }
		
	      X = Y;
	    }
	  }
	}
	else {
	  /* terminal */
	  /* (t) */
	  /* On remplace par le nt [t] */
	  if ((Y = t2opt_t [-X]) == 0) {
	    varstr_raz (vstr);
	    vstr = varstr_catchar (vstr, '[');
	    vstr = varstr_catenate (vstr, SXWORD_get (t_names, -X+1));
	    vstr = varstr_catchar (vstr, ']');
	    vstr = varstr_complete (vstr);
	    Y = sxword_2save (&nt_names, varstr_tostr (vstr), varstr_length (vstr))-1;
	    t2opt_t [-X] = Y;
	    doldol_nb++;
	  }
		
	  X = Y;
	}
      }
	
      XH_push (new_prod_hd, X);
    }

    /* 2 old_prod differentes (par exemple "(A)" ds l'une et "A" ds l'autre) peuvent donner
       la meme new_prod, ds ce cas, il faut concatener la semantique des 2 */
    if (XH_set (&new_prod_hd, &new_prod)) {
      /* 2 old_prod donnent la meme new_prod */
      /* On concatene new_prod2rule [new_prod] avec prod2rule [prod] */
      for (Y = new_prod2rule [new_prod]; (Y = rule2next_rule [X = Y]) != 0;);

      rule2next_rule [X] = prod2rule [prod];
    }
    else {
      new_prod2rule [new_prod] = prod2rule [prod];
    }
  }

  doldol_stack = (int*) sxalloc (doldol_nb+1, sizeof (int));
  doldol_stack [0] = 0;

  doldol_dol1 = 0;
  
  for (X = 1; X <= tmax; X++) {
    /* Pour chaque terminal X de la forme (X), on genere
       <[X]> = ;
       <[X]> = "X" ;
    */
    Y = t2opt_t [X];

    if (Y != 0) {
      XH_push (new_prod_hd, Y);
      XH_set (&new_prod_hd, &new_prod);
      new_prod2rule [new_prod] = ++rule_nb;
      XH_push (new_prod_hd, Y);
      XH_push (new_prod_hd, -X);
      XH_set (&new_prod_hd, &new_prod);
      new_prod2rule [new_prod] = ++rule_nb;
      /* ... puis "$$ = $1" */
	
      if (doldol_dol1 == 0) {
	/* $$ = $1 est la seule equation du bloc */
	XH_push (code_stack, OPERATOR_IDENTITY);
#if 0
	XH_push (code_stack, 1);
	XH_push (code_stack, OPERATOR_REF); /* $1 */
	XH_push (code_stack, 0);
	XH_push (code_stack, OPERATOR_REF);
	XH_push (code_stack, OPERATOR_UNIFY); /* $$ */
#endif /* 0 */
	XH_set (&code_stack, &doldol_dol1);

	XH_push (equation_stack, doldol_dol1);
	XH_push (equation_stack, -1);
	XH_set (&equation_stack, &doldol_dol1_equation_set_id);
      }

      /* Il faut se souvenir des [new_]rule_nb ou stocker doldol_dol1_equation_set_id */
      doldol_stack [++*doldol_stack] = rule_nb;
    }
  }
  X = 0;

  while ((X = sxba_0_lrscan (is_empty_nt, X)) > 0) {
    /* X ne derive pas ds le vide */
    if (SXBA_bit_is_set (has_nt_opt, X)) {
      /* Il y a du (X) */
      rule_nb++;

      if (!SXBA_bit_is_set (has_nt_non_opt, X)) {
	/* Il n'y a que du (X) */
	/* On genere "<X> = ;" */
	XH_push (new_prod_hd, X);
      }
      else {
	/* Il y a aussi du X */
	/* On genere "<[X]> = ;"... */
	Y = nt2opt_nt [X];
	XH_push (new_prod_hd, Y);
	XH_set (&new_prod_hd, &new_prod);
	new_prod2rule [new_prod] = rule_nb;

	 /* ... et "<[X]> = <X> ;" */
	rule_nb++;
	XH_push (new_prod_hd, Y);
	XH_push (new_prod_hd, X);

	/* ... puis "$$ = $1" */
	if (doldol_dol1 == 0) {
	/* $$ = $1 est la seule equation du bloc */
	XH_push (code_stack, OPERATOR_IDENTITY);
#if 0
	  XH_push (code_stack, 1);
	  XH_push (code_stack, OPERATOR_REF); /* $1 */
	  XH_push (code_stack, 0);
	  XH_push (code_stack, OPERATOR_REF);
	  XH_push (code_stack, OPERATOR_UNIFY); /* $$ */
#endif /* 0 */
	  XH_set (&code_stack, &doldol_dol1);

	  XH_push (equation_stack, doldol_dol1);
	  XH_push (equation_stack, -1);
	  XH_set (&equation_stack, &doldol_dol1_equation_set_id);
	}

	/* Il faut se souvenir des [new_]rule_nb ou stocker doldol_dol1_equation_set_id */
	doldol_stack [++*doldol_stack] = rule_nb;
      }
	
      XH_set (&new_prod_hd, &new_prod);
      new_prod2rule [new_prod] = rule_nb;
    }
  }

  sxfree (has_nt_opt);
  sxfree (has_nt_non_opt);
  sxfree (has_t_opt);
  sxfree (is_empty_nt);

  XH_free (&prod_hd);
  sxfree (nt2opt_nt);
  sxfree (t2opt_t);

  prod_hd = new_prod_hd;

  new_prodmax = XH_top (prod_hd)-1;

  new_rule2next_rule = (int*) sxcalloc (rule_nb+1, sizeof (int));

  for (prod = rule_nb - (new_prodmax-prodmax); prod >= 0; prod--)
    new_rule2next_rule [prod] = rule2next_rule [prod];

  sxfree (prod2rule);
  prod2rule = new_prod2rule;
  sxfree (rule2next_rule);
  rule2next_rule = new_rule2next_rule;

  new_rule2equation = (int*) sxcalloc (rule_nb+1, sizeof (int));

  for (prod = 0; prod <= RULES_nb; prod++)
    new_rule2equation [prod] = rule2equation [prod];

  while ((X = *doldol_stack) != 0) {
    new_rule2equation [doldol_stack [X]] = doldol_dol1_equation_set_id;
    --*doldol_stack;
  }

  sxfree (doldol_stack);
  sxfree (rule2equation);
  rule2equation = new_rule2equation;
}

static void
output_bnf_header ()
{
  long	date_time;
    
  date_time = time (0);

  fprintf (bnf_file,
	   "\
******************************************************************************************\n\
*\tThis CFG in BNF form, extracted from \"%s\" has been output to \"%s\"\n\
*\ton %s\
*\tby the SYNTAX(*) LFG processor\n\
******************************************************************************************\n\
*\t(*) SYNTAX is a trademark of INRIA.\n\
******************************************************************************************\n\n\n",
	   (path_name == NULL) ? "stdin" : path_name,
	   bnf_file_name,
	   ctime (&date_time));
}

static void
output_bnf ()
{
  int prodmax, prod, bot, top, cur, X;

  output_bnf_header ();

  prodmax = XH_top (prod_hd)-1;

  for (prod = 1; prod <= prodmax; prod++) {
    bot = XH_X (prod_hd, prod);
    top = XH_X (prod_hd, prod+1);

    fprintf (bnf_file, "<%s> = ", SXWORD_get (nt_names, XH_list_elem (prod_hd, bot)+1));

    /* On parcourt la rhs */
    for (cur = bot+1; cur < top; cur++) {
      X = XH_list_elem (prod_hd, cur);

      if (X < 0)
	fprintf (bnf_file, "\"%s\" ", SXWORD_get (t_names, -X+1));
      else
	fprintf (bnf_file, "<%s> ", SXWORD_get (nt_names, X+1));
    }

    fputs (";\n", bnf_file);
  }
}


static void
output_code_header ()
{
  long	date_time;
    
  date_time = time (0);

  fprintf (output_file,
	   "\
/* ********************************************************************\n\
\tThis semantic code of the LFG equations of \"%s\" has been generated\n\
\ton %s\
\tby the SYNTAX(*) LFG processor\n\
   ********************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
   ******************************************************************** */\n\n\n",
	   (path_name == NULL) ? "stdin" : path_name,
	   ctime (&date_time));

  /* lfg_time == date/heure actuelle si le source est lu sur stdin ou la date/heure de construction
     du fichier contenant la grammaire LFG */
  fprintf (output_file, "\n#define LFG_TIME\t%lu\n\n", lfg_time);
}


static VOID
out_ifdef (char *name)
{
  printf ("\n\n#ifdef def_%s\n", name);
}

static VOID

out_endif (char *name)
{
  printf ("#endif /* def_%s */\n", name);
}

static void
output_code ()
{
  int i, j, v, bot, top, x, maxrhs_shift;

  output_code_header ();

  fprintf (output_file, "%s", OPERATOR_string);

  fprintf (output_file, "#define RULES_NB %i\n", rule_nb);
  fprintf (output_file, "#define PRODS_NB %i\n", XH_top (prod_hd)-1);
  fprintf (output_file, "#define MAXRHS %i\n", maxrhs);
  fprintf (output_file, "#define MAXRHS_SHIFT %i\n", maxrhs_shift = sxlast_bit (maxrhs));
  fprintf (output_file, "#define MAXRHS_AND %i\n", ~((~0) << maxrhs_shift));
  /* fprintf (output_file, "#define EQUATION_NB %i\n", EQUATION_nb); */
  fprintf (output_file, "#define IS_OPERATOR_LEX_REF %i\n", (int) is_OPERATOR_LEX_REF);
  fprintf (output_file, "#define IS_OPERATOR_LEXEME_REF %i\n", (int) is_OPERATOR_LEXEME_REF);

  fputs ("\n#include \"XxY.h\"\n", output_file);

  out_ifdef ("dfield_pairs");
  if (dfield_pairs2field_id) {
    XxY_lock (&dfield_pairs); /* size == top */
    XxY_to_c (&dfield_pairs, output_file, "dfield_pairs", TRUE /* static */);
    out_endif ("dfield_pairs");

    top = XxY_top (dfield_pairs);
    out_ifdef ("dfield_pairs2field_id");
    fprintf (output_file, "\nstatic int dfield_pairs2field_id [%i] = {\n\
/* 0 */ 0, ", top+1);

    for (i = 1; i <= top; i++) {
      if (i%10 == 0)
	fprintf (output_file, "\n/* %i */ ", i);

      fprintf (output_file, "%i, ", dfield_pairs2field_id [i]);
    }

    fputs ("\n};\n", output_file);

  }
  else {
    fputs ("static XxY_header dfield_pairs;\n", output_file);
    fputs ("static int *dfield_pairs2field_id;\n", output_file);
  }
  out_endif ("dfield_pairs2field_id");

  out_ifdef ("datom_pairs");
  if (datom_pairs2atom_id) {
    XxY_lock (&datom_pairs); /* size == top */
    XxY_to_c (&datom_pairs, output_file, "datom_pairs", TRUE /* static */);
    out_endif ("datom_pairs");

    top = XxY_top (datom_pairs);
    out_ifdef ("datom_pairs2atom_id");
    fprintf (output_file, "\nstatic int datom_pairs2atom_id [%i] = {\n\
/* 0 */ 0, ", top+1);

    for (i = 1; i <= top; i++) {
      if (i%10 == 0)
	fprintf (output_file, "\n/* %i */ ", i);

      fprintf (output_file, "%i, ", datom_pairs2atom_id [i]);
    }

    fputs ("\n};\n", output_file);
  }
  else {
    fputs ("static XxY_header datom_pairs;\n", output_file);
    fputs ("static int *datom_pairs2atom_id;\n", output_file);
  }
  out_endif ("datom_pairs2atom_id");

    out_ifdef ("suffix_id2string");
  if (dfield_pairs2field_id || datom_pairs2atom_id) {
    top = SXWORD_top (suffix_names);
    printf ("\nstatic char *suffix_id2string [%i] = {", top);

    for (i = 0; i < top; i++) {
      if (i%10 == 0 || i%10 == 5)
	printf ("\n/* %i */", i);

      printf ("\"%s\", ", SXWORD_get (suffix_names, i));
    }

    fputs ("};\n", stdout);
  } else {
    fputs ("static char *suffix_id2string [1];\n", output_file);
  }
  out_endif ("suffix_id2string");


#ifdef ESSAI
  out_ifdef ("operator_atom2atom_list");
  printf ("\nstatic int operator_atom2atom_list [%i] = {0, ", XH_top (atom_hd)+1);
  sxinitialise (top);

  for (i = 1; i <= XH_top (atom_hd);  i++) {
    if (i%10==1)
      printf ("\n/* %i */ ", i);

    top = XH_X (atom_hd, i);
    printf ("%i, ", top);
  }

  fputs ("\n};\n", stdout);
  out_endif ("operator_atom2atom_list");

  out_ifdef ("atom_list");
  printf ("\nstatic int atom_list [%i] = {0,", top+1);

  for (i = 1; i < XH_top (atom_hd);  i++) {
    bot = XH_X (atom_hd, i);
    top = XH_X (atom_hd, i+1);

    printf ("\n/* %i (%i..%i) */ ", i, bot, top-1);

    while (bot < top) {
      printf ("%i, ", XH_list_elem (atom_hd, bot));
      bot++;
    }
  }

  fputs ("\n0,\n};\n", stdout);
  out_endif ("atom_list");
#endif /* ESSAI */

  out_ifdef ("prod2rule");
  fputs ("\n/* Permet le mapping entre la \"prod\" de BNF et les �quations associ�es � la \"rule\" de la sp�cif LFG */\n\
static int prod2rule [PRODS_NB+1] = {\n\
/* 0 */ 0, ", output_file);

  for (i = 1; i < XH_top (prod_hd); i++) {
    if (i%10 == 0)
      fprintf (output_file, "\n/* %i */ ", i);

    fprintf (output_file, "%i, ", prod2rule [i]);
  }

  fputs ("\n};\n", output_file);
  out_endif ("prod2rule");

  out_ifdef ("rule2next_rule");
  fputs ("\nstatic int rule2next_rule [RULES_NB+1] = {\n\
/* 0 */ 0, ", output_file);

  for (i = 1; i <= rule_nb; i++) {
    if (i%10 == 0)
      fprintf (output_file, "\n/* %i */ ", i);

    fprintf (output_file, "%i, ", rule2next_rule [i]);
  }

  fputs ("\n};\n", output_file);
  out_endif ("rule2next_rule");

  out_ifdef ("rule2equation");
  fputs ("\nstatic int rule2equation [RULES_NB+1] = {\n\
/* 0 */ 0, ", output_file);

  for (i = 1; i <= rule_nb; i++) {
    if (i%10 == 0)
      fprintf (output_file, "\n/* %i */ ", i);

    x = rule2equation [i];

    if (x != 0)
      x = XH_X (equation_stack, x);

    fprintf (output_file, "%i, ", x);
  }

  fputs ("\n};\n", output_file);
  out_endif ("rule2equation");


  out_ifdef ("equation_stack");
  fprintf (output_file, "\nstatic int equation_stack [%i] = {\n\
/* 0 */ 0, ", XH_list_top (equation_stack));

  for (i = 1; i < XH_top (equation_stack); i++) {
    bot = XH_X (equation_stack, i);
    top = XH_X (equation_stack, i+1);
    fprintf (output_file, "\n/* equation list #%i: [%i..%i] */ ", i, bot, top-1);

    for (j = bot; j < top; j++) {
      x = XH_list_elem (equation_stack, j); /* x est l'identifiant ds code_stack */
      
      if (x > 0)
	x = XH_X (code_stack, x+1)-1; /* On prend l'adresse du sommet */
      /* else 0 equation(s) non traitee(s), ou -1 fin de liste */
      
      fprintf (output_file, "%i, ", x);
    }
  }

  fputs ("\n};\n", output_file);
  out_endif ("equation_stack");

  out_ifdef ("code_stack");
  fprintf (output_file, "\nstatic int code_stack [%i] = {\n\
/* 0 */ 0, ", XH_list_top (code_stack));

  for (i = 1; i < XH_top (code_stack); i++) {
    bot = XH_X (code_stack, i);
    top = XH_X (code_stack, i+1);
    fprintf (output_file, "\n/* code #%i: [%i..%i] */ ", i, bot, top-1);

    for (j = bot; j < top; j++) {
      if ((v = XH_list_elem (code_stack, j)) >= 0)
	fprintf (output_file, "%i, ", v);
      else
	fprintf (output_file, "%s, ", operator_code2name [-v]);
    }
  }

  fputs ("\n};\n", output_file);
  out_endif ("code_stack");

  {
    int    nt_id, complete_id, nt, ntmax, ste, nb, prod, output_code_bot, output_code_top, cur, X;
    int    *nt2complete_id;
    SXBA   complete_set, nt_set;
    int    complete_name_length, nt_name_length;
    char   *complete_name, *nt_name;

    fputs ("\n /* Liste des codes des non-terminaux pour lesquels les f_structures associees doivent etre coherentes et completes */\n", output_file);
    ntmax = SXWORD_top (nt_names)-2;

    complete_set = sxba_calloc (ntmax+1);
    nt2complete_id = (int*) sxalloc (ntmax+1, sizeof (int));

    if (complete_names.table) {
      /* calcul de nt_names2nt */
      nt_set = sxba_calloc (ntmax+1);
      nt = 0;

      for (prod = 1; prod < XH_top (prod_hd); prod++) {
	output_code_bot = XH_X (prod_hd, prod);
	output_code_top = XH_X (prod_hd, prod+1);

	/* On parcourt la rhs */
	for (cur = output_code_bot; cur < output_code_top; cur++) {
	  X = XH_list_elem (prod_hd, cur);

	  if (X > 0 && SXBA_bit_is_reset_set (nt_set, X)) {
	    nt++;
	    nt_names2nt [X+1] = nt;
	  }
	}
      }

      sxfree (nt_set);

      for (complete_id = SXWORD_top (complete_names)-1; complete_id >= 2; complete_id--) {
	complete_name = SXWORD_get (complete_names, complete_id);
	complete_name_length = SXWORD_len (complete_names, complete_id);

	if (complete_name [complete_name_length-1] ==  '*') {
	  for (nt = 1; nt <= ntmax; nt++) {
	    nt_id = nt2nt_names [nt];
	    nt_name = SXWORD_get (nt_names, nt_id);
	    nt_name_length = SXWORD_len (nt_names, nt_id);
	    if (nt_name_length >= complete_name_length
		&& strncmp (nt_name, complete_name, complete_name_length-1) == 0) {
	      nt2complete_id [nt] = complete_id;
	      SXBA_1_bit (complete_set, nt);
	    }
	  }

	}
	else if (strcmp (complete_name, "default") == 0 /* cha�nes �gales */) {
	  nt2complete_id [0] = complete_id;
	  SXBA_1_bit (complete_set, 0);
	}
	else {
	  nt_id = sxword_2retrieve (&nt_names, complete_name, complete_name_length);
	  
	  if (nt_id > 1) {
	    nt = nt_names2nt [nt_id];
	    nt2complete_id [nt] = complete_id;
	    SXBA_1_bit (complete_set, nt);
	  } else {
	    fprintf (stderr,"Warning: ranking associated with non-existent non-terminal symbol \"%s\". Ignored.\n",complete_name);
	  }
	}
      }
    }

    out_ifdef ("complete_id_set");
    sxba2c (complete_set, stdout, "complete_id_set", "", TRUE, vstr);
    out_endif ("complete_id_set");

    out_ifdef ("ranks");
    fprintf (output_file, "\nstatic char *ranks [%i] = {\n", ntmax+1);

    nb = 0;

    for (nt = 0; nt <= ntmax; nt++) {
      if (!SXBA_bit_is_set (complete_set, nt)) {
	if (nb == 10) {
	  nb = 0;
	  fputs ("\n", output_file);
	}

	fputs ("NULL, ", output_file);
	nb++;
      }
      else {
	nt_id = nt2nt_names [nt];
	ste = complete_name2ste [nt2complete_id [nt]];

	if (nb)
	  fputs ("\n", output_file);

	fprintf (stdout, "/* %i: %s */ \"%s\", \n", nt, SXWORD_get (nt_names, nt_id), sxstrget (ste));
	nb = 0;
      }

      if (sxba_scan (complete_set, nt) < 0)
	break;
    }
  
    fputs ("\n};\n", output_file);
    out_endif ("ranks");

    sxfree (complete_set);
    sxfree (nt2complete_id);
  }
}
/* ************************************************************************************** */
static void smpopen ()
{
  sxatcvar.atc_lv.node_size = sizeof (struct lfg_node);
}

static void smppass ()
{
  NODE *visited;

  /*   I N I T I A L I S A T I O N S   */
  /* ........... */
  ste_Aij = sxstrretrieve ("Aij");
  ste_Pij = sxstrretrieve ("Pij");

  /* Pour avoir une idee des tailles!! */
  visited = sxatcvar.atc_lv.abstract_tree_root->son->son;

  while (visited != NULL) {
    if (visited->name == ATTRIBUTE_n) {
      FIELDS_nb = visited->son->degree+2/* pred et EMPTY_STE */;
      break;
    }

    visited = visited->brother;
  }

  RULES_nb = sxatcvar.atc_lv.abstract_tree_root->son->brother->degree;

  sxword_alloc (&field_names, "field_names", FIELDS_nb+1, 1, 16,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
  field_id2kind = (unsigned char *) sxalloc (FIELDS_nb+1, sizeof (unsigned char));
  XH_alloc (&compound_fields_hd, "compound_fields_hd", 50, 1, 3, compound_fields_oflw, NULL);
  pred_id = sxword_2save (&field_names, "pred", 4); /* Initialisation a priori */
  field_id2kind [ERROR_STE] = ERROR_KIND;
  field_id2kind [pred_id] = ATOM_KIND;

#ifdef ESSAI
  XH_alloc (&atom_hd, "atom_hd", 100+1, 1, 2, NULL, NULL);
#endif /* ESSAI */

  sxword_alloc (&atom_names, "atom_names", 100, 1, 16,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, atom_names_oflw, NULL);
  field_id2atom_set = sxbm_calloc (FIELDS_nb+1, SXWORD_size (atom_names)+1);
  working_atom_set = sxba_calloc (SXWORD_size (atom_names)+1);
  vstr = varstr_alloc (128);

  /*   A T T R I B U T E S    E V A L U A T I O N   */
  sxsmp (sxatcvar.atc_lv.abstract_tree_root, lfg_pi, lfg_pd);

  /*   F I N A L I S A T I O N S   */
  /* ........... */

  if (!IS_ERROR) {
    /* 2eme passe (recursive) pour generer le code des equations */
    XH_alloc (&code_stack, "code_stack", EQUATION_nb+1, 1, 10, code_stack_oflw, NULL);
    code_set = sxba_calloc (XH_size (code_stack)+1);
    operator_in_set = sxba_calloc (XH_size (code_stack)+1);
    rule2equation = (int*) sxalloc (RULES_nb+2, sizeof (int));
    rule2equation [0] = 0;
    XH_alloc (&equation_stack, "equation_stack", RULES_nb, 1, 10, NULL, NULL);

    sxword_alloc (&t_names, "t_names",RULES_nb+1, 1, 8,
		  sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
    sxword_alloc (&nt_names, "nt_names",RULES_nb+1, 1, 12,
		  sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
    nt_names2nt = (int*) sxalloc (2*RULES_nb+1, sizeof (int));
    nt2nt_names = (int*) sxalloc (2*RULES_nb+1, sizeof (int));
    XH_alloc (&prod_hd, "prod_hd", RULES_nb+1, 1, 10, prod_hd_oflw, NULL);
    prod2rule = (int*) sxalloc (XH_size (prod_hd)+1, sizeof (int));
    rule2next_rule = (int*) sxalloc (RULES_nb+1, sizeof (int)); 
    default_set = sxba_calloc (maxrhs+1);

    gen_code (sxatcvar.atc_lv.abstract_tree_root->son->brother /* RULES_S_n */);
      
    sxfree (default_set), default_set = NULL;

    if (has_optional)
      /* Le squelette contient des symboles optionnels de la forme (X), on reecrit la grammaire... */
      remove_optional ();

    if (vocabulary_file != NULL)
      output_vocabulary ();

    if (bnf_file != NULL)
      output_bnf ();

    /* On sort le code... */
    output_file = stdout;
    output_code ();

    /* On libere le tout... */
    XH_free (&code_stack);
    sxfree (code_set), code_set = NULL;
    sxfree (operator_in_set), operator_in_set = NULL;
    sxfree (rule2equation), rule2equation = NULL;
    XH_free (&equation_stack);
    sxword_free (&t_names);
    sxword_free (&nt_names);
    sxfree (nt_names2nt), nt_names2nt = NULL;
    sxfree (nt2nt_names), nt2nt_names = NULL;
    XH_free (&prod_hd);
    sxfree (rule2next_rule), rule2next_rule = NULL; 
    sxfree (prod2rule), prod2rule = NULL; 
  }

  if (complete_names.table) {
    sxword_free (&complete_names);
    sxfree (complete_name2ste), complete_name2ste = NULL;
  }

  sxword_free (&field_names);
  sxfree (field_id2kind), field_id2kind = NULL;
  XH_free (&compound_fields_hd);

#ifdef ESSAI
  XH_free (&atom_hd);
#endif /* ESSAI */

  sxword_free (&atom_names);
  sxbm_free (field_id2atom_set), field_id2atom_set = NULL;
  sxfree (working_atom_set), working_atom_set = NULL;
  varstr_free (vstr), vstr = NULL;

  if (working_field_set) sxfree (working_field_set), working_field_set = NULL;
  if (used_field_set) sxfree (used_field_set), used_field_set = NULL;
  if (argument_set) sxfree (argument_set), argument_set = NULL;

  if (working_compound_field_set) {
    sxfree (working_compound_field_set), working_compound_field_set = NULL;
    sxfree (working2_compound_field_set), working2_compound_field_set = NULL;
  }

#ifndef ESSAI
  if (fieldXatom2local_id) {
    sxfree (fieldXatom2local_id), fieldXatom2local_id = NULL;
    sxfree (fieldXlocal2atom_id), fieldXlocal2atom_id = NULL;
    sxfree (local_id_area), local_id_area = NULL;
    sxfree (local_id_area2), local_id_area2 = NULL;
  }
#endif /* ESSAI */

  if (dfield_pairs2field_id) {
    XxY_free (&dfield_pairs);
    sxfree (dfield_pairs2field_id), dfield_pairs2field_id = NULL;
    sxword_free (&suffix_names);
  }
}

void
lfg_smp (what, sxtables_ptr)int what;
struct sxtables *sxtables_ptr;
{
  switch (what) {
  case OPEN:
    sxtab_ptr = sxtables_ptr;
    smpopen ();
    break;
  case SEMPASS: /* For [sub-]tree evaluation during parsing */
    break;
  case ACTION:
    if (!sxatcvar.atc_lv.abstract_tree_is_error_node && sxerrmngr.nbmess [2] == 0)
      smppass ();

    break;
  case CLOSE:
    break;
  default:
    break;
  }
}
