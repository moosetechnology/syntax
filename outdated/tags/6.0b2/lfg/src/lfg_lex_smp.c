/* ********************************************************************
   *  This program has been generated from lfg_lex.at                 *
   *  on Mon Nov 13 13:57:42 2006                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */

static char     ME [] = "lfg_lex_smp";

/* Les valeurs atomiques sont qcq et on peut faire de la disjonction sur les "..." */
#define ESSAI

/*   I N C L U D E S   */
#define NODE struct lfg_lex_node
#include "varstr.h"
#include "sxunix.h"
char WHAT_LFGLEXSMP[] = "@(#)SYNTAX - $Id: lfg_lex_smp.c 643 2007-06-21 09:35:02Z rlacroix $" WHAT_DEBUG;

extern BOOLEAN  use_a_dico; /* TRUE => sxdico, FALSE => sxword */
extern BOOLEAN  make_proper_will_be_used;

struct lfg_lex_node {
  SXNODE_HEADER_S VOID_NAME;

  /*
    your attribute declarations...
  */
  int id;
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define AMALGAM_n 2
#define AMALGAM_S_n 3
#define ARGUMENT_n 4
#define ARGUMENT_S_n 5
#define ATOM_MINUS_n 6
#define ATOM_PLUS_n 7
#define ATOM_S_n 8
#define ATTRIBUTE_n 9
#define ATTRIBUTE_PASS_NB_n 10
#define ATTRIBUTE_S_n 11
#define ATTR_ARG_STRUCT_n 12
#define ATTR_ATOMS_n 13
#define ATTR_DOTS_n 14
#define ATTR_SET_OF_STRUCT_n 15
#define ATTR_STRUCT_n 16
#define ATTR_VAL_n 17
#define ATTR_VAL_S_n 18
#define CATEGORY_n 19
#define CHAINE_n 20
#define COMPLETE_n 21
#define COMPOUND_n 22
#define COMPOUND_S_n 23
#define DECLARATION_S_n 24
#define DISJONCTION_S_n 25
#define EMPTY_ADJUNCT_n 26
#define EMPTY_ATOM_n 27
#define EMPTY_NAMED_ADJUNCT_n 28
#define EMPTY_NAMED_ATOM_n 29
#define EMPTY_NAMED_STRUCT_n 30
#define EMPTY_SOUS_CAT_n 31
#define EMPTY_STRUCT_n 32
#define ENTIER_n 33
#define FIELD_n 34
#define FIELD_CONSTRAINT_n 35
#define FIELD_EQUAL_n 36
#define FIELD_OPTIONAL_n 37
#define IDENT_n 38
#define IDENT_S_n 39
#define LEXICAL_GRAMMAR_n 40
#define MACRO_n 41
#define MACRO_NAME_n 42
#define MACRO_STRUCTURE_n 43
#define MOT_1COL_n 44
#define NAMED_ADJUNCT_n 45
#define NAMED_ATOM_n 46
#define NAMED_STRUCTURE_n 47
#define OPT_DISJONCTION_n 48
#define OPT_FIELD_n 49
#define PAIR_n 50
#define PAIR_S_n 51
#define PRED_n 52
#define PRED_FIELD_n 53
#define PRIORITY_n 54
#define PRIORITY_MAX_n 55
#define PRIORITY_S_n 56
#define RULE_n 57
#define RULES_S_n 58
#define SOUS_CAT_n 59
#define STRUCTURE_S_n 60
#define TERMINAL_n 61
#define VOID_n 62
/*
E N D   N O D E   N A M E S
*/

#include "sxword.h"
#include "XH.h"
#include "X.h"

#include "B_tables.h"

#define X80 (~((~(0u))>>1))
#define X40 (X80>>1)
#define X20 (X80>>2)
#define X10 (X80>>3)
#define X3F ((~(0u))>>2)
#define X0F ((~(0u))>>4)
#define XC0 (~(X3F)) 

static sxword_header      t_names;
static int                *t_id2t;
static int                tmax;
static SXBA               t_id_set;

static int                TERMINAL_nb, FIELDS_nb, MACROS_nb, RULES_nb;

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */

static struct sxtables *sxtab_ptr;
static sxword_header   field_names, atom_names, lexeme_names;
/* Si la valeur d'un champ atomique unbounded de nom "champ" (donc declare' par champ = {Aij, Pij ...})
   prend la valeur Aij ou Pij (par une equation comme $$ champ = Aij), alors l'atom prend la valeur de la
   lhs de la production $$ si Aij ou le numero de la prod instanciee si Pij */
static int             ste_Aij, ste_Pij, atom_Aij, atom_Pij;
static unsigned char   *field_id2kind;
static int             *field_id2pass_nb;
static int             largest_lexical_structure_size, main_structure_nb, cur_lexeme;
static SXBA            argument_set, referenced_field_set, pred_arg_set, warg_set;
static XH_header       compound_fields_hd, sous_cat_hd;
static int             pred_id, max_field_id, max_static_atom_id, max_unbounded_atom_id, max_atom_field_nb, max_undef_atom_field_nb;
static int             max_shared_f_structure_id;
static SXBA            *field_id2atom_set;
static int             current_field_id;
static VARSTR          vstr;

static XH_header       XH_priority_hd;

#ifndef ESSAI
static int             max_local_atom_id;
static int             **fieldXatom2local_id, **fieldXlocal2atom_id, *local_id_area, *local_id_area2;
#endif /* ESSAI */

static sxword_header   macro_names;
static unsigned int    *macro_id2structure;
static NODE            **macro_id2visited;
static BOOLEAN         has_named_structure_inside_macro;

static struct pred_value {
  int graphie, sous_cat1, sous_cat2;
} *pred_values, *pred_value_ptr;

static SXBA            *field_sets, used_field_set, working_field_set;
static int             **field_values, *field_value, current_level, max_level, field_sets_size;
static NODE            ***field_nodes, **field_node;
static int             **field_shapes, *field_shape;
static XH_header       structure_hd, pred_val_hd, adjunct_hd, atom_hd; 
static int             empty_struct_id;
      
/* Structure statique du "kind" d'un field */
#define ATOM_KIND              1
#define STRUCT_KIND            2
#define UNBOUNDED_KIND         4
#define Aij_KIND               8

/* Structure dynamique du "kind" d'un field */
#define ASSIGN_CONSTRAINT_KIND 1
#define ASSIGN_OPTION_KIND     2

#define STRUCTURE_NAME_shift   2

#define MAX_PRIORITY 0x7fffffff

#define KIND_string "\
/* Structure statique du \"kind\" d'un field */\n\
#define ATOM_KIND              1\n\
#define STRUCT_KIND            2\n\
#define UNBOUNDED_KIND         4\n\
#define Aij_KIND               8\n\
\n\
/* Structure dynamique du \"kind\" d'un field */\n\
#define ASSIGN_CONSTRAINT_KIND 1\n\
#define ASSIGN_OPTION_KIND     2\n\
\n\
#define STRUCTURE_NAME_shift   2\n\
\n\
#define MAX_PRIORITY 0x7fffffff\n\
"
static SXBA          working_compound_field_set, working2_compound_field_set;

static int           inflected_form_nb, lexical_rule_nb;
static XH_header     lexical_rule_stack_hd;
static sxword_header inflected_form_names;
static int           *if_id2lr_id, *lr_id2prev, *lr_id2priority;

static NODE          *AMALGAM_VISITED, **amalgam2visited, *COMPOUND_VISITED, **compound2visited;
static sxword_header amalgam_or_compound_component_names;
static SXBA          amalgam_id_set, compound_component_id_set;
static int           amalgam_size, compound_size, amalgam_id_top; 

/* On suppose que used_field_set et field_value contiennent les valeurs a stocker ds structure_hd */
/* Si 0 \in used_field_set <=> field_value [0] designe une macro */
/* Si i> 0 field_value [i] est soit une structure, soit un atom dependant du kind de i */
/* field_shape [i] contient l'index x des valeurs indexee (x << STRUCTURE_NAME_shift */
/* Si i est UNBOUNDED_KIND+STRUCT_KIND, 
     si field_value [i] == 0 => ADJUNCT vide : {} eventuellement indexe'
     sinon field_node [i] contient un pointeur vers STRUCTURE_S_n */
static void
alloc_level ()
{
  field_sets_size = 10;
  field_sets = sxbm_calloc (field_sets_size+1, max_field_id+1);
  field_values = (int**) sxalloc (field_sets_size+1, sizeof (int*));
  field_nodes = (NODE***) sxalloc (field_sets_size+1, sizeof (NODE**));
  field_shapes = (int**) sxalloc (field_sets_size+1, sizeof (int*));
  pred_values = (struct pred_value*) sxalloc (field_sets_size+1, sizeof (struct pred_value));
}

static void
free_level ()
{
  if (field_sets) {
    sxbm_free (field_sets), field_sets = NULL;

    if (max_level) {
      do {
	sxfree (field_values [max_level]);
	sxfree (field_nodes [max_level]);
	sxfree (field_shapes [max_level]);
      } while (--max_level > 0);
    }

    sxfree (field_values), field_values = NULL;
    sxfree (field_nodes), field_nodes = NULL;
    sxfree (pred_values), pred_values = NULL;
    sxfree (field_shapes), field_shapes = NULL;
  }
}

static void
incr_level ()
{
  if (field_sets ==NULL)
    alloc_level ();

  if (++current_level > field_sets_size) {
    field_sets = sxbm_resize (field_sets, field_sets_size+1, 2*field_sets_size+1, max_field_id+1);
    field_sets_size *= 2;
    field_values = (int**) sxrealloc (field_values, field_sets_size+1, sizeof (int*));
    field_nodes = (NODE***) sxrealloc (field_nodes, field_sets_size+1, sizeof (NODE**));
    field_shapes = (int**) sxrealloc (field_shapes, field_sets_size+1, sizeof (int*));
    pred_values = (struct pred_value*) sxrealloc (pred_values, field_sets_size+1, sizeof (struct pred_value));
  }

  if (current_level > max_level) {
    max_level = current_level;
    field_values [current_level] = (int*) sxalloc (max_field_id+1, sizeof (int));
    field_nodes [current_level] = (NODE**) sxalloc (max_field_id+1, sizeof (NODE*));
    field_shapes [current_level] = (int*) sxalloc (max_field_id+1, sizeof (int));
  }

  used_field_set = field_sets [current_level];
  field_value = field_values [current_level];
  field_node = field_nodes [current_level];
  field_shape = field_shapes [current_level];
  pred_value_ptr = pred_values+current_level;
  pred_value_ptr->graphie = 0;
}

static void
decr_level ()
{
  current_level--;
  used_field_set = field_sets [current_level];
  field_value = field_values [current_level];
  field_node = field_nodes [current_level];
  field_shape = field_shapes [current_level];
  pred_value_ptr = pred_values+current_level;
}

static int        max_struct_index;
static SXBA       struct_index_set, struct_index_set2, cyclic_index_set;
static int        *struct_index2struct_id, *struct_index2field_id;


static BOOLEAN
less_equal_struct (i, j)
     int i, j;
{
  int topi, topj, li, lj, xi, xj, d;

  topi = XH_X (structure_hd, i+1);
  li = topi-XH_X (structure_hd, i);
  topj = XH_X (structure_hd, j+1);
  lj = topj-XH_X (structure_hd, j);

  d = (li < lj) ? li : lj;

  while (d-- > 0) {
    xi = XH_list_elem (structure_hd, --topi);
    xj = XH_list_elem (structure_hd, --topj);

    if (xi != xj)
      return xi < xj;
  }

  /* l'un est un suffixe de l'autre */
  return li <= lj;
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

static int     *struct_id2head;

static void
gen_structure_hd ()
{
  int     struct_id, struct_id_top, bot, top, boti, topi, botj, topj, xattr_val, yattr_val, x, j, adjunct_id, atom_id, val;
  int     *sorted, *struct_elem2xattr_val;

  struct attr_val {
    int attr, kind, val, next;
  } *attr_vals, *pattr_val, *pattr_vali;

  struct_id_top = XH_top (structure_hd);

  if (struct_id_top == 1)
    return;


  /* On partage les suffixes */
  /* On commence par trier */
  sorted = (int*) sxalloc (struct_id_top, sizeof (int));

  for (struct_id = 0; struct_id < struct_id_top; struct_id++)
    sorted [struct_id] = struct_id;

  sort_by_tree (sorted, 1, struct_id_top-1, less_equal_struct);

  /* On les met par groupe de 3 (attr, kind, val) */
  attr_vals = (struct attr_val*) sxalloc (((XH_list_top (structure_hd)+struct_id_top)/3)+1, sizeof (struct attr_val));
  struct_elem2xattr_val = (int*) sxalloc (XH_list_top (structure_hd)+1, sizeof (int));
  struct_id2head = (int*) sxalloc (struct_id_top, sizeof (int));
  struct_id2head [0] = 0;

  pattr_val = attr_vals;
  pattr_val->next = pattr_val->val = pattr_val->kind = pattr_val->attr = 0;
  xattr_val = 0;

  struct_id = sorted [1]; /* le 1er */
  bot = botj = XH_X (structure_hd, struct_id);
  top = topj = XH_X (structure_hd, struct_id+1);
  /* Normalement on a struct_id == empty_struct_id et bot == top == 1 */

  if (bot == top)
    struct_id2head [struct_id] = 0; /* nelle def */
  else {
    /* bloc inutile */
    pattr_val++;
    xattr_val++;
    pattr_val->next = 0;
    pattr_val->val = XH_list_elem (structure_hd, --top);
    pattr_val->kind = XH_list_elem (structure_hd, --top);
    pattr_val->attr =  XH_list_elem (structure_hd, --top);
    struct_elem2xattr_val [top] = xattr_val;

    while (bot < top) {
      pattr_val++;
      pattr_val->next = xattr_val++;
      pattr_val->val = XH_list_elem (structure_hd, --top);
      pattr_val->kind = XH_list_elem (structure_hd, --top);
      pattr_val->attr = XH_list_elem (structure_hd, --top);
      struct_elem2xattr_val [top] = xattr_val;
    }

    struct_id2head [struct_id] = xattr_val; /* nelle def */
  }

  for (struct_id = 2; struct_id < struct_id_top; struct_id++) {
    boti = botj;
    topi = topj;
    j = sorted [struct_id];
    botj = bot = XH_X (structure_hd, j);
    topj = top = XH_X (structure_hd, j+1);

    yattr_val = 0;

    /* Tant que j partage des (pairs) de suffixe avec i, on fait rien */
    while (boti < topi && bot < top) {
      topi -= 3;
      x = struct_elem2xattr_val [topi];
      pattr_vali = attr_vals+x;

      if (pattr_vali->val == XH_list_elem (structure_hd, top-1)
	  && pattr_vali->kind == XH_list_elem (structure_hd, top-2)
	  && pattr_vali->attr == XH_list_elem (structure_hd, top-3)) {
	/* Les triplets attr-val sont egaux */
	top -= 3;
	struct_elem2xattr_val [top] = yattr_val = x;
      }
      else {
	/* differents => on range le prefixe */
	break;
      }
    }

    while (bot < top) {
      pattr_val++;
      pattr_val->next = yattr_val;
      yattr_val = ++xattr_val;
      pattr_val->val = XH_list_elem (structure_hd, --top);
      pattr_val->kind = XH_list_elem (structure_hd, --top);
      pattr_val->attr = XH_list_elem (structure_hd, --top);
      struct_elem2xattr_val [top] = xattr_val;
    }

    struct_id2head [j] = yattr_val; /* nelle def ou suffixe de la precedente */
  }

  /* On change les ref aux struct par leurs nelles defs */
  while (pattr_val > attr_vals) {
    if (field_id2kind [pattr_val->attr] == STRUCT_KIND) {
      if ((struct_id = pattr_val->val)) {
	pattr_val->val = struct_id2head [struct_id];
      }
    }

    pattr_val--;
  }

  /* ... et les ref ds adjunct_hd */
  for (adjunct_id = XH_top (adjunct_hd)-1; adjunct_id > 0;  adjunct_id--) {
    bot = XH_X (adjunct_hd, adjunct_id);
    top = XH_X (adjunct_hd, adjunct_id+1);

    while (bot < top) {
      struct_id = XH_list_elem (adjunct_hd, bot);

      if (struct_id < 0)
	break;

      /* On change la reference */
      XH_list_elem (adjunct_hd, bot) = struct_id2head [struct_id];

      bot++;
    }
  }

  out_ifdef ("lex_atom_hd");
  printf ("\nstatic int lex_atom_hd [%i] = {0, ", XH_top (atom_hd)+1);

  for (atom_id = 1; atom_id <= XH_top (atom_hd);  atom_id++) {
    if (atom_id%10==1)
      printf ("\n/* %i */ ", atom_id);

    top = XH_X (atom_hd, atom_id);
    printf ("%i, ", top);
  }

  fputs ("\n};\n", stdout);
  out_endif ("lex_atom_hd");

  out_ifdef ("lex_atom_list");
  printf ("\nstatic int lex_atom_list [%i] = {0,", top+1);

  for (atom_id = 1; atom_id < XH_top (atom_hd);  atom_id++) {
    bot = XH_X (atom_hd, atom_id);
    top = XH_X (atom_hd, atom_id+1);

    printf ("\n/* %i (%i..%i) */ ", atom_id, bot, top-1);

    while (bot < top) {
      val = XH_list_elem (atom_hd, bot);
      
      if (val < 0)
	printf ("0X%X, ", val);
      else
	printf ("%i, ", val);

      bot++;
    }
  }

  fputs ("\n0,\n};\n", stdout);
  out_endif ("lex_atom_list");

  out_ifdef ("lex_adjunct_hd");
  printf ("\nstatic int lex_adjunct_hd [%i] = {0, ", XH_top (adjunct_hd)+1);

  for (adjunct_id = 1; adjunct_id <= XH_top (adjunct_hd);  adjunct_id++) {
    if (adjunct_id%10==1)
      printf ("\n/* %i */ ", adjunct_id);

    top = XH_X (adjunct_hd, adjunct_id);
    printf ("%i, ", top);
  }

  fputs ("\n};\n", stdout);
  out_endif ("lex_adjunct_hd");

  out_ifdef ("lex_adjunct_list");
  printf ("\nstatic int lex_adjunct_list [%i] = {0,", top+1);

  for (adjunct_id = 1; adjunct_id < XH_top (adjunct_hd);  adjunct_id++) {
    bot = XH_X (adjunct_hd, adjunct_id);
    top = XH_X (adjunct_hd, adjunct_id+1);

    printf ("\n/* %i (%i..%i) */ ", adjunct_id, bot, top-1);

    while (bot < top) {
      val = XH_list_elem (adjunct_hd, bot);
      
      if (val < 0)
	printf ("0X%X, ", val);
      else
	printf ("%i, ", val);

      bot++;
    }
  }

  fputs ("\n0,\n};\n", stdout);
  out_endif ("lex_adjunct_list");

#if 0 // parti dans lex_lfg.h
  printf ("\nstatic struct attr_val {\n\
    int attr, kind, val, next;\n\
  } attr_vals [%i] = { {0, 0, 0, 0},", xattr_val+1);
#endif /* 0 */

  out_ifdef ("attr_vals");
  printf ("\nstatic struct attr_val attr_vals [%i] = { {0, 0, 0, 0},", xattr_val+1);

  for (x = 1, pattr_val = attr_vals+1; x <= xattr_val; x++, pattr_val++) {
    if (x%5==1)
      printf ("\n/* %i */ ", x);
    if (pattr_val->val < 0)
      printf ("{%i, %i, 0X%X, %i}, ", pattr_val->attr, pattr_val->kind, pattr_val->val, pattr_val->next);
    else
      printf ("{%i, %i, %i, %i}, ", pattr_val->attr, pattr_val->kind, pattr_val->val, pattr_val->next);
  }

  fputs ("\n};\n", stdout);
  out_endif ("attr_vals");
  
  sxfree (sorted);
  sxfree (struct_elem2xattr_val);
  sxfree (attr_vals);
}



#if EBUG
static void
print_structure (visited, struct_id, lexeme_id)
     SXNODE  *visited;
     int     struct_id, lexeme_id;
{
  int           bot, top, top_val, bot2, cur2, top2, bot3, cur3, top3;
  int           field_id, field_kind, sous_cat_id, atom_id;
#ifndef ESSAI
  int           local_atom_id;
  int           *local2atom_id;
#endif /* ESSAI */
  BOOLEAN       is_optional, is_first;
  unsigned char static_field_kind;


  if (visited) {
  /* Pour que la sortie soit de C correct */
    printf ("/* struct #%i at line %lu: ", struct_id, visited->token.source_index.line);
  }

  fputs ("[", stdout);

  is_first = TRUE;

  for (bot = XH_X (structure_hd, struct_id), top = XH_X (structure_hd, struct_id+1);
       bot < top;
       bot++) {

    field_id = XH_list_elem (structure_hd, bot);
    field_kind = XH_list_elem (structure_hd, ++bot);
    struct_id = XH_list_elem (structure_hd, ++bot);

    if (struct_id < 0)
      /* nommee */
      break;

    if (is_first)
      is_first = FALSE;
    else
      fputs (", ", stdout);

    static_field_kind = field_id2kind [field_id];

    if (field_id == pred_id) {
      int x, id;
      BOOLEAN print_structure_is_first;

      if (static_field_kind != ATOM_KIND)
	sxtrap (ME, "print_structure");

      x = XH_X (pred_val_hd, struct_id);
      id = XH_list_elem (pred_val_hd, x++);
      
      fputs ("pred =", stdout);

      if (field_kind & ASSIGN_CONSTRAINT_KIND)
	fputs ("c", stdout);
      else {
	if (field_kind & ASSIGN_OPTION_KIND)
	  fputs ("?", stdout);
      }

      if (visited) {
	if (id != 0)
	  sxtrap (ME, "print_structure");

	printf (" \"%s", SXWORD_get (lexeme_names, lexeme_id));
      }
      else {
	printf (" \"%s", SXWORD_get (lexeme_names, id));
      }

      sous_cat_id = XH_list_elem (pred_val_hd, x++);
      print_structure_is_first = TRUE;

      while (sous_cat_id) {
	fputs (" ", stdout);

	if (print_structure_is_first) {
	  fputs ("<", stdout);
	}
      
	for (bot2 = cur2 = XH_X (sous_cat_hd, sous_cat_id), top2 = XH_X (sous_cat_hd, sous_cat_id+1);
	     cur2 < top2;
	     cur2++) {
	  if (cur2 > bot2)
	    fputs (", ", stdout);

	  field_id = XH_list_elem (sous_cat_hd, cur2);
	
	  if (field_id < 0) {
	    fputs ("(", stdout);
	    field_id = -field_id;
	    is_optional = TRUE;
	  }
	  else
	    is_optional = FALSE;

	  if (field_id > max_field_id) {
	    field_id -= max_field_id;

	    for (bot3 = cur3 = XH_X (compound_fields_hd, field_id), top3 = XH_X (compound_fields_hd, field_id+1);
		 cur3 < top3;
		 cur3++) {
	      field_id = XH_list_elem (compound_fields_hd, cur3);
	      printf ("%s%s", (cur3 != bot3) ? "|" : "", SXWORD_get (field_names, field_id));
	    }
	  }
	  else {
	    printf ("%s", SXWORD_get (field_names, field_id));
	  }

	  if (is_optional)
	    fputs (")", stdout);
	}

	if (print_structure_is_first) {
	  print_structure_is_first = FALSE;
	  sous_cat_id = XH_list_elem (pred_val_hd, x);
	  fputs (">", stdout);
	}
	else
	  sous_cat_id = 0;
      }
	  
      fputs ("\"", stdout);
    }
    else {
	printf ("%s ", SXWORD_get (field_names, field_id));

      if (field_kind & ASSIGN_CONSTRAINT_KIND)
	fputs ("=c ", stdout);
      else {
	if (field_kind & ASSIGN_OPTION_KIND)
	  fputs ("=?", stdout);
	else
	  fputs ("= ", stdout);
      }

      if (static_field_kind & ATOM_KIND) {
	if (struct_id == 0)
	  /* On supporte 0 comme etant l'atome vide ... */
	  fputs ("()", stdout);
	else {
	  bot2 = XH_X (atom_hd, struct_id);
	  top2 = XH_X (atom_hd, struct_id+1);

	  if (bot2 == top2) {
	    /* ... ou une liste vide */
	    fputs ("()", stdout);
	  }
	  else {
	    top_val = XH_list_elem (atom_hd, top2-1);

	    if (top_val < 0) {
	      /* atome partage' (nomme') de la forme X80+X20+name_id */
	      fputs ("(", stdout);
	      top2--;
	    }

#ifndef ESSAI
	    if (bot2 < top2) {
	      struct_id = XH_list_elem (atom_hd, bot2);

	      if (static_field_kind & UNBOUNDED_KIND)
		/* Couvre le cas Aij_KIND */
		printf ("%s", SXWORD_get (atom_names, struct_id));
	      else {
		local2atom_id = fieldXlocal2atom_id [field_id];
		local_atom_id = 0;
		is_first2 = TRUE;

		while ((struct_id >>= 1) != 0) {
		  local_atom_id++;

		  if (struct_id & 1) {
		    atom_id = local2atom_id [local_atom_id];

		    if (is_first2)
		      is_first2 = FALSE;
		    else
		      fputs ("|", stdout);

		    printf ("%s", SXWORD_get (atom_names, atom_id));
		  }
		}
	      }
	    }
#else /* ESSAI */
	    if (bot2 < top2) {
	      printf ("%s", SXWORD_get (atom_names, XH_list_elem (atom_hd, bot2)));

	      while (++bot2 < top2) {
		atom_id = XH_list_elem (atom_hd, bot2);
		printf (" | %s", SXWORD_get (atom_names, atom_id));
	      }
	    }
#endif /* ESSAI */

	    if (top_val < 0)
	      printf (")%i", top_val & X0F);
	  }
	}
      }
      else {
	if (static_field_kind == STRUCT_KIND) {
	  print_structure (NULL, struct_id, 0);
	}
	else {
	  /* static_field_kind == STRUCT_KIND+UNBOUNDED_KIND */
	  if (struct_id == 0)
	    /* On supporte 0 comme etant l'adjunct vide ... */
	    fputs ("{}", stdout);
	  else {
	    bot2 = XH_X (adjunct_hd, struct_id);
	    top2 = XH_X (adjunct_hd, struct_id+1);

	    if (bot2 == top2) {
	      /* ... ou une liste vide */
	      fputs ("{}", stdout);
	    }
	    else {
	      top_val = XH_list_elem (adjunct_hd, top2-1);

	      if (top_val < 0) {
		/* atome partage' (nomme') de la forme X80+X20+X10+name_id */
		top2--;

		if (top_val & X10)
		  fputs ("(unclosed)", stdout);
	      }

	      fputs ("{", stdout);

	      cur2 = bot2;

	      while (cur2 < top2) {
		if (cur2 != bot2)
		  fputs (", ", stdout);
		  
		print_structure (FALSE, XH_list_elem (adjunct_hd, cur2), 0);
		
		cur2++;
	      }

	      fputs ("}", stdout);

	      if (top_val < 0 && (top_val & X20))
		printf ("%i", top_val & X0F);
	    }
	  }
	}
      }
    }
  }
      
  fputs ("]", stdout);

  if (struct_id < 0) {
    /* non standard */
    if (struct_id & X40)
      /* cyclique */
      fputs (".c", stdout);

    if (struct_id & X20)
      /* nommee */
      printf (".%i", struct_id & X0F);

    if (struct_id & X10)
      /* non close */
      fputs (".nc", stdout);
  }

  if (visited)
    fputs (" */\n", stdout);
}
#endif /* EBUG */


static BOOLEAN
check_structure_hd (struct_id)
     int struct_id;
{
  int           bot, top, bot2, top2, cur2, field_id, check_structure_hd_index, old_struct_id;
  unsigned char static_field_kind;

  for (bot = XH_X (structure_hd, struct_id), top = XH_X (structure_hd, struct_id+1);
       bot < top;
       bot++) {
    field_id = XH_list_elem (structure_hd, bot);
    ++bot /* field_kind = XH_list_elem (structure_hd, bot) */;
    struct_id = XH_list_elem (structure_hd, ++bot);

    if (struct_id <= 0)
      break;

    if (struct_id != empty_struct_id) {
      static_field_kind = field_id2kind [field_id];

      if (static_field_kind == STRUCT_KIND) {
	top2 = XH_X (structure_hd, struct_id+1);
	bot2 = XH_X (structure_hd, struct_id);
	check_structure_hd_index = XH_list_elem (structure_hd, top2-1);

	if (check_structure_hd_index < 0) {
	  /* C'est une structure non standard ... */
	  if (check_structure_hd_index & X20) {
	    /* ... partagee */
	    check_structure_hd_index &= X0F;
	    SXBA_1_bit (struct_index_set, check_structure_hd_index);

	    if ((old_struct_id = struct_index2struct_id [check_structure_hd_index]) == 0)
	      struct_index2struct_id [check_structure_hd_index] = struct_id;
	    else {
	      if (old_struct_id < 0) {
		return FALSE;
	      }

	      if (top2-bot2 > 3) {
		/* struct_id est non vide ... */
		if (XH_X (structure_hd, old_struct_id+1) - XH_X (structure_hd, old_struct_id) > 3)
		  /* ... et old_struct_id aussi */
		  return FALSE;

		struct_index2struct_id [check_structure_hd_index] = struct_id; /* On met le non vide */
	      }
	    }
	  }
	  else
	    check_structure_hd_index = 0;
	}
	else
	  check_structure_hd_index = 0;

	if (check_structure_hd_index) {
	  if (!SXBA_bit_is_reset_set (struct_index_set2, check_structure_hd_index)) {
	    if (struct_index2struct_id [check_structure_hd_index] & X80) {
	      /* Pour l'instant !!, on interdit les adjuncts cycliques car il semble qu'on ne sache pas le faire
		 avec des equations ?? */
	      return FALSE;
	    }

	    SXBA_1_bit (cyclic_index_set, check_structure_hd_index);
	    /* Detection d'un cycle */
	  }
	}

	if ((check_structure_hd_index == 0 || top2-bot2 > 3) /* On examine la structure non vide struct_id */
	    && !check_structure_hd (struct_id))
	  return FALSE;

	if (check_structure_hd_index && ! SXBA_bit_is_set (cyclic_index_set, check_structure_hd_index))
	  SXBA_0_bit (struct_index_set2, check_structure_hd_index);
      }
      else {
	/* adjunct ou atome */
	if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	  top2 = XH_X (adjunct_hd, struct_id+1);
	  bot2 = XH_X (adjunct_hd, struct_id);
	  check_structure_hd_index = XH_list_elem (adjunct_hd, top2-1);
	}
	else {
	  top2 = XH_X (atom_hd, struct_id+1);
	  bot2 = XH_X (atom_hd, struct_id);
	  check_structure_hd_index = XH_list_elem (atom_hd, top2-1);
	}

	if (check_structure_hd_index < 0) {
	  /* C'est un atome ou un adjunct non standard ... */
	  top2--;

	  if (check_structure_hd_index & X20) {
	    /* ... partagee */
	    check_structure_hd_index &= X0F;
	    SXBA_1_bit (struct_index_set, check_structure_hd_index);

	    if ((old_struct_id = struct_index2struct_id [check_structure_hd_index]) == 0)
	      struct_index2struct_id [check_structure_hd_index] = X80+struct_id
		+((static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) ? 0 : X40); /* Les ref ds adjunct_hd sont mises en neg */
	    else {
	      if (old_struct_id > 0
		  || (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND && (old_struct_id & X40) != 0)
		  || (static_field_kind != STRUCT_KIND+UNBOUNDED_KIND && (old_struct_id & X40) == 0)) {
		/* pas le bon type */
		return FALSE;
	      }

	      if (top2 > bot2) {
		/* struct_id est non vide ... */
		old_struct_id &= ~(X80+X40);

		if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND && XH_X (adjunct_hd, old_struct_id+1) - XH_X (adjunct_hd, old_struct_id) > 1) {
		  /* Il est aussi non standard */
		  /* ... et old_struct_id aussi */
		  return FALSE;
		}

		if (static_field_kind != STRUCT_KIND+UNBOUNDED_KIND && XH_X (atom_hd, old_struct_id+1) - XH_X (atom_hd, old_struct_id) > 1) {
		  /* Il est aussi non standard */
		  /* ... et old_struct_id aussi */
		  return FALSE;
		}

		struct_index2struct_id [check_structure_hd_index] = X80+struct_id
		  +((static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) ? 0 : X40); /* On met le non vide */
	      }
	    }
	  }
	  else
	    check_structure_hd_index = 0;
	}
	else
	  check_structure_hd_index = 0;
	
	if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	  if (check_structure_hd_index)
	    SXBA_1_bit (struct_index_set2, check_structure_hd_index);

	  if (check_structure_hd_index == 0 || top2 > bot2) {
	    for (cur2 = bot2; cur2 < top2; cur2++) {
	      /* On verifie chaque structure de l'adjunct */
	      if (!check_structure_hd (XH_list_elem (adjunct_hd, cur2))) {
		return FALSE;
	      }
	    }
	  }

	  if (check_structure_hd_index)
	    SXBA_0_bit (struct_index_set2, check_structure_hd_index);
	}
      }
    }
  }

  return TRUE;
}

static void
clear_struct_index ()
{
  int clear_struct_index_index;

  if (struct_index_set) {
    clear_struct_index_index = -1;

    while ((clear_struct_index_index = sxba_scan_reset (struct_index_set, clear_struct_index_index)) >= 0) {
      struct_index2struct_id [clear_struct_index_index] = 0;
    }

    sxba_empty (struct_index_set2);
    sxba_empty (cyclic_index_set);
  }
}

#if 0
static int **T, T_size, T_unique;
#endif /* 0 */


static int  glbl_id2sons_size, glbl_id;
static SXBA *glbl_id2sons, *glbl_id2fathers, glbl_id2not_closed;

static int  named_structure2visited_size, named_structure2visited_top;
static SXBA named_structure_set, empty_named_structure_set;
#if 0
static SXBA *named_structure2sons, *named_structure2fathers, named_structure2not_closed;
#endif /* 0 */
static SXBA named_structure2already_visited;
static NODE **named_structure2visited;
static int  *named_structure2occur_nb, *named_structure2id;


/* visited->name == ATTR_VAL_S_n */
static int
fill_structure_hd (visited, lexeme, is_main_structure)
     SXNODE  *visited;
     int     *lexeme;
     BOOLEAN *is_main_structure;
{
  int           field_id, field_kind, field_nb, sous_cat_id, fill_structure_hd_index, struct_id, old_struct_id, named_structure, flag, cur2;
  int           bot, top, bot2, top2;
  BOOLEAN       is_closed;
  SXNODE        *father;
  unsigned char static_field_kind;

  *lexeme = 0;
  father = visited->father;

  if (glbl_id2not_closed)
    is_closed = !SXBA_bit_is_set (glbl_id2not_closed, visited->id);
  else
    is_closed = TRUE;

  if (father->name == NAMED_STRUCTURE_n) {
    named_structure = atoi (sxstrget (visited->brother->token.string_table_entry)); /* > 0 */
#if 0
    is_closed = !SXBA_bit_is_set (named_structure2not_closed, named_structure);
#endif /* 0 */
    father = father->father;
  }
  else {
    named_structure = 0;
#if 0
    is_closed = TRUE; /* a priori */
#endif /* 0 */
    /* On va regarder ses fils */
  }

  if (father->father->name == RULES_S_n) {
    /* Structure principale */
#if 0    
    /* finalement les structures principales nommees (donc cycliques) sont permises !! */
    if (visited->father->name == NAMED_STRUCTURE_n) {
      sxput_error (visited->brother->token.source_index,
		   "%sIndexed structures are illegal at top level: ignored.",
		   sxtab_ptr->err_titles [2]);
      return 0;
    }
#endif /* 0 */

    if (named_structure) {
      /* structure principale nommee => cyclique */
      SXBA_1_bit (struct_index_set, named_structure);
      SXBA_1_bit (cyclic_index_set, named_structure);
    }

    /* On verifie les structures indexees */
    field_id = -1;
  
    while ((field_id = sxba_scan (used_field_set, field_id)) >= 0) {
      struct_id = field_value [field_id];

      if (struct_id > 0 && struct_id != empty_struct_id) {
	static_field_kind = field_id2kind [field_id];

	if (static_field_kind == STRUCT_KIND) {
	  top2 = XH_X (structure_hd, struct_id+1);
	  bot2 = XH_X (structure_hd, struct_id);
	  fill_structure_hd_index = XH_list_elem (structure_hd, top2-1);

	  if (fill_structure_hd_index < 0) {
	    /* C'est une structure non standard ... */
#if 0
	    /* Le 03/08/05 */
	    /* is_closed est positionne en utilisant glbl_id2not_closed.  Une sous-structure peut etre non close
	     et la structure la contenant close */
	    if (named_structure == 0 && (fill_structure_hd_index & X10))
	      is_closed = FALSE;
#endif /* 0 */

	    if (fill_structure_hd_index & X20) {
	      /* ... partagee */
	      fill_structure_hd_index &= X0F;
	      SXBA_1_bit (struct_index_set, fill_structure_hd_index);

	      if ((old_struct_id = struct_index2struct_id [fill_structure_hd_index]) == 0)
		struct_index2struct_id [fill_structure_hd_index] = struct_id;
	      else {
		if (old_struct_id < 0) {
		  sxput_error (visited->token.source_index,
			       "%sFunctional structure index already used.",
			       sxtab_ptr->err_titles [2]);
		  clear_struct_index ();
		  return 0;
		}

		if (top2-bot2 > 3) {
		  /* struct_id est non vide ... */
		  if (XH_X (structure_hd, old_struct_id+1) - XH_X (structure_hd, old_struct_id) > 3) {
		    /* ... et old_struct_id aussi */
		    sxput_error (visited->token.source_index,
				 "%sMultiple non empty functional structures cannot be shared.",
				 sxtab_ptr->err_titles [2]);
		    clear_struct_index ();
		    return 0;
		  }

		  struct_index2struct_id [fill_structure_hd_index] = struct_id; /* On met le non vide */
		}
	      }
	    }
	    else
	      fill_structure_hd_index = 0;
	  }
	  else
	    fill_structure_hd_index = 0;

	  if (fill_structure_hd_index)
	    SXBA_1_bit (struct_index_set2, fill_structure_hd_index);

	  if ((fill_structure_hd_index == 0 || top2-bot2 > 3) /* On examine la structure struct_id qui est non vide */
	      && !check_structure_hd (struct_id)) {
	    sxput_error (visited->token.source_index,
			 "%sMultiple non empty functional structures cannot be shared.",
			 sxtab_ptr->err_titles [2]);
	    clear_struct_index ();
	    return 0;
	  }

	  if (fill_structure_hd_index)
	    SXBA_0_bit (struct_index_set2, fill_structure_hd_index);
	}
	else {
	  /* adjunct ou atome */
	  if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	    top2 = XH_X (adjunct_hd, struct_id+1);
	    bot2 = XH_X (adjunct_hd, struct_id);
	    fill_structure_hd_index = XH_list_elem (adjunct_hd, top2-1);
	  }
	  else {
	    top2 = XH_X (atom_hd, struct_id+1);
	    bot2 = XH_X (atom_hd, struct_id);
	    fill_structure_hd_index = XH_list_elem (atom_hd, top2-1);
	  }

	  if (bot2 == top2)
	    fill_structure_hd_index = 0;
	  else {
	    if (fill_structure_hd_index < 0) {
	      /* C'est un adjunct ou un atome non standard ... */
	      top2--;

	      if (fill_structure_hd_index & X20) {
		/* ... partagee */
		fill_structure_hd_index &= X0F;
		SXBA_1_bit (struct_index_set, fill_structure_hd_index);

		if ((old_struct_id = struct_index2struct_id [fill_structure_hd_index]) == 0)
		  struct_index2struct_id [fill_structure_hd_index] = X80+struct_id
		    +((static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) ? 0 : X40 /* atome partage */); /* X80 [+X40] + struct_id */
		else {
		  if (old_struct_id > 0 /* deja utilise' par un STRUCT_KIND */
		      || (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND && (old_struct_id & X40) != 0) /* deja utilise' par un ATOM_KIND */
		      || ((static_field_kind & ATOM_KIND) && (old_struct_id & X40) == 0) /* deja utilise' par un STRUCT_KIND+UNBOUNDED_KIND */
		      ) {
		    sxput_error (visited->token.source_index,
				 "%sIllegal index (already used).",
				 sxtab_ptr->err_titles [2]);
		    clear_struct_index ();
		    return 0;
		  }

		  if (top2 > bot2) {
		    /* struct_id est non vide ... */
		    old_struct_id &= ~(X80+X40);

		    if ((static_field_kind == STRUCT_KIND+UNBOUNDED_KIND && XH_X (adjunct_hd, old_struct_id+1) - XH_X (adjunct_hd, old_struct_id) > 1) ||
			((static_field_kind & ATOM_KIND) && XH_X (atom_hd, old_struct_id+1) - XH_X (atom_hd, old_struct_id) > 1)) {
		      /* Il est aussi non standard */
		      /* ... et old_struct_id aussi */
		      sxput_error (visited->token.source_index,
				   "%sMultiple non empty adjuncts or atoms cannot be shared.",
				   sxtab_ptr->err_titles [2]);
		      clear_struct_index ();
		      return 0;
		    }

		    struct_index2struct_id [fill_structure_hd_index] = X80+struct_id
		      +((static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) ? 0 : X40); /* On met le non vide */
		  }
		}
	      }
	      else
		fill_structure_hd_index = 0;
	    }
	    else
	      fill_structure_hd_index = 0;
	  }

	  if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	    if (fill_structure_hd_index)
	      SXBA_1_bit (struct_index_set2, fill_structure_hd_index);

	    if (fill_structure_hd_index == 0 || top2 > bot2) {
	      for (cur2 = bot2; cur2 < top2; cur2++) {
		/* On verifie chaque structure de l'adjunct */
		struct_id = XH_list_elem (adjunct_hd, cur2);

		if (!check_structure_hd (struct_id)) {
		  sxput_error (visited->token.source_index,
			       "%sIllegal named functional structures.",
			       sxtab_ptr->err_titles [2]);
		  clear_struct_index ();
		  return 0;
		}

#if 0
		/* Le 03/08/05 */
		/* is_closed est positionne en utilisant glbl_id2not_closed.  Une sous-structure peut etre non close
		   et la structure la contenant close */
		if (named_structure == 0) {
		  bot = XH_X (structure_hd, struct_id);
		  top = XH_X (structure_hd, struct_id+1);

		  if (bot < top && (XH_list_elem (structure_hd, top-1) & (X80+X10)) == (X80+X10)) {
		    /* non clos */
		    is_closed = FALSE;
		  }
		}
#endif /* 0 */
	      }
	    }

	    if (fill_structure_hd_index)
	      SXBA_0_bit (struct_index_set2, fill_structure_hd_index);
	  }
	}
      }
    }

    *is_main_structure = TRUE;
  }
  else
    *is_main_structure = FALSE;

  if (pred_value_ptr->graphie) {
    /* Y'a un pred */
    /* On verifie la coherence :
       s'il existe un arg local, et s'il existe un champ pred alors l'arg local doit avoir ete defini ds pred */
    sxba_copy (warg_set, used_field_set);
    sxba_and (warg_set, argument_set);

    if (!sxba_is_empty (warg_set)) {
      /* Il existe des arg locaux ... */

      /* On recolte les args de pred ds pred_arg_set */
      sxba_empty (pred_arg_set);
      sous_cat_id = pred_value_ptr->sous_cat1;

      while (sous_cat_id) {
	for (bot = XH_X (sous_cat_hd, sous_cat_id), top = XH_X (sous_cat_hd, sous_cat_id+1);
	     bot < top;
	     bot++) {
	  field_id = XH_list_elem (sous_cat_hd, bot);

	  if (field_id < 0) {
	    field_id = -field_id;
	  }

	  if (field_id > max_field_id) {
	    field_id -= max_field_id;

	    for (bot2 = XH_X (compound_fields_hd, field_id), top2 = XH_X (compound_fields_hd, field_id+1);
		 bot2 < top2;
		 bot2++) {
	      field_id = XH_list_elem (compound_fields_hd, bot2);
	      SXBA_1_bit (pred_arg_set, field_id);
	    }
	  }
	  else {
	    SXBA_1_bit (pred_arg_set, field_id);
	  }
	}

	if (sous_cat_id == pred_value_ptr->sous_cat1)
	  sous_cat_id = pred_value_ptr->sous_cat2;
	else
	  sous_cat_id = 0;
      }

      /* warg_set doit etre un sous-ensemble de pred_arg_set */
      if (!sxba_is_subset (warg_set, pred_arg_set)) {
	sxput_error (visited->token.source_index,
		     "%sThis structure is unconsistent.",
		     sxtab_ptr->err_titles [2]);   
  
	if (*is_main_structure) 
	  clear_struct_index ();

	sxba_empty (used_field_set);

	return 0;
      }
    }

    {
      int pred_val;

      XH_push (structure_hd, pred_id);
      XH_push (structure_hd, field_shape [pred_id]);
    
      /* Tous les champs tiennent sur des triplets */
      if (*is_main_structure) {
	*lexeme = pred_value_ptr->graphie;
	XH_push (pred_val_hd, 0);
      }
      else
	XH_push (pred_val_hd, pred_value_ptr->graphie);

      XH_push (pred_val_hd, pred_value_ptr->sous_cat1);
      XH_push (pred_val_hd, pred_value_ptr->sous_cat2);

      XH_set (&pred_val_hd, &pred_val);
      XH_push (structure_hd, pred_val);
    }
  }

  field_id = -1;
  field_nb = 0;

  while ((field_id = sxba_scan_reset (used_field_set, field_id)) >= 0) {
    field_nb++;
    field_kind = field_shape [field_id];
    struct_id = field_value [field_id];

    XH_push (structure_hd, field_id);
    XH_push (structure_hd, field_kind);
    XH_push (structure_hd, struct_id);
  }

  if (field_nb > largest_lexical_structure_size)
    largest_lexical_structure_size = field_nb;

  if (named_structure || !is_closed) {
    XH_push (structure_hd, 0 /* field_id */);
    XH_push (structure_hd, 0 /* field_kind */);

    if (!is_closed)
      flag = X80+X10;
    else
      flag = 0;

    if (named_structure)
      flag |= X80+X20+named_structure;
    
    XH_push (structure_hd, flag);
  }

  if (!XH_set (&structure_hd, &struct_id) && *is_main_structure)
    main_structure_nb++;

  if (*is_main_structure) {
    if (struct_index_set) {
      if (sxba_scan (struct_index_set, 0) > 0) {
	/* Il y a des structures nommes */
	struct_id |= X40;

	if (sxba_scan (cyclic_index_set, 0) > 0)
	  /* ... dont certaines sont cycliques */
	  struct_id |= X80;
      }

      clear_struct_index ();
    }
  }

  return struct_id;
}

static void
atom_names_oflw (old_size, new_size)
     int old_size, new_size;
{
  field_id2atom_set = sxbm_resize (field_id2atom_set, FIELDS_nb+1, FIELDS_nb+1, new_size+1);
}

static void
macro_names_oflw (old_size, new_size)
     int old_size, new_size;
{
  macro_id2structure = (unsigned int *) sxrealloc (macro_id2structure, new_size+1, sizeof (unsigned int));
  macro_id2visited = (NODE **) sxrealloc (macro_id2visited, new_size+1, sizeof (NODE *));
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
inflected_form_names_oflw (old_size, new_size)
     int old_size, new_size;
{
  if_id2lr_id = (int*) sxrealloc (if_id2lr_id, new_size+1, sizeof (int));
}

static void
lexical_rule_stack_oflw (old_size, new_size)
     int old_size, new_size;
{
  lr_id2prev = (int*) sxrealloc (lr_id2prev, new_size+1, sizeof (int));
  lr_id2priority = (int*) sxrealloc (lr_id2priority, new_size+1, sizeof (int));
}

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
#define OPERATOR_MACRO      -12

#include "SS.h"

static int             *SS_path;

#if 0
/* On est sur un nom de macro, on exploite SS_path */
/* On ne fait rien s'il y a OPERATOR_ERROR en pile */
static int
close_path ()
{
  int bot, cur, top, oper;

  bot = SS_bot (SS_path), top = SS_top (SS_path);

  for (cur = bot; cur < top; cur++) {
    oper = SS_get (SS_path, cur);
    
    if ((unsigned)oper == OPERATOR_ERROR) {
      /* Erreur ds le traitement de l'equation, abandon */
      SS_close (SS_path);
      return 0;
    }
  }

  /* ... on memorise */
  /* On se contente de retourner top, bot sera retrouve' par SS_path [top] */
  SS_path [top] = bot; /* Truc pas prevu ds SS.h */
  return top;
}
#endif /* 0 */


/* Nelle version, les structures principales peuvent etre nommees */
/* visited->name == {
   ATTR_VAL_S_n,  NAMED_STRUCTURE_n, EMPTY_STRUCT_n, EMPTY_NAMED_STRUCT_n,
   ATOM_S_n, NAMED_ATOM_n, EMPTY_ATOM_n, EMPTY_NAMED_ATOM_n,  
   STRUCTURE_S_n, NAMED_ADJUNCT_n, EMPTY_ADJUNCT_n, EMPTY_NAMED_ADJUNCT_n
   } */
/* visited->name = ATTR_VAL_S_n || NAMED_STRUCTURE_n || EMPTY_NAMED_STRUCT_n*/
/* On verifie, que si ATTR_VAL_S_n a des structures nommees, pour chaque nom i, au plus une structure
   est non vide.  De plus on remplit named_structure2visited avec le bon pointeur */
/* Le 25/04/05, on etend ca aux adjuncts nommes et aux atomes nommes */
/*
  X80 => erreur
  0   => OK pas de nommees
  1   => OK avec des nommees
*/
static int
process_named_structure (visited)
     NODE *visited;
{
  int     i, macro_id;
  NODE    *son, *brother;
  int     ret_val;

  ret_val = 0;
  son = brother = visited;
  i = 0;

  switch (brother->name) {
  case NAMED_STRUCTURE_n:
  case NAMED_ATOM_n:
  case NAMED_ADJUNCT_n:
    son = brother->son;
    brother = son->brother;

  case EMPTY_NAMED_STRUCT_n:
  case EMPTY_NAMED_ATOM_n:
  case EMPTY_NAMED_ADJUNCT_n:
    i = atoi (sxstrget (brother->token.string_table_entry));

    if (i == 0 || i > named_structure2visited_size) {
      sxput_error (brother->token.source_index,
		   "%sImplementation restriction, the number of named structure is bounded by #%i.",
		   sxtab_ptr->err_titles [2],
		   named_structure2visited_size);
      ret_val |= X80;
    }
    else {
      if (i > named_structure2visited_top)
	named_structure2visited_top = i;

      ret_val |= 1;
      named_structure2occur_nb [i]++;
    }
    
    if ((son->id = named_structure2id [i]) == 0) {
      /* 1ere rencontre */
      /* son->name == EMPTY_NAMED_STRUCT_n | EMPTY_NAMED_ATOM_n | EMPTY_NAMED_ADJUNCT_n | ATOM_S_n | ATTR_VAL_S_n | STRUCTURE_S_n */
      son->id = named_structure2id [i] = ++glbl_id;
    }

    break;

  case EMPTY_STRUCT_n:
  case EMPTY_ATOM_n:
  case EMPTY_ADJUNCT_n:
    return ret_val; /* 0 */

  case ATTR_VAL_S_n:
  case STRUCTURE_S_n:
    if (son->brother == NULL) {
      /* pas nommee */
      son->id = ++glbl_id;
    }

  case ATOM_S_n:
    /* Rien sur les atomes non nommes */
    break;
  default:
#if EBUG
    sxtrap(ME,"unknown switch case #1");
#endif
    break;
  }

  switch (visited->name) {
  case EMPTY_NAMED_STRUCT_n:
  case EMPTY_NAMED_ATOM_n:
  case EMPTY_NAMED_ADJUNCT_n:
    if (i > 0 && i <= named_structure2visited_size) {
      SXBA_1_bit (empty_named_structure_set, i);
    }

    return ret_val;

  case ATOM_S_n:
    return ret_val;

  case ATTR_VAL_S_n:
    son = visited;
    /* son->name == ATTR_VAL_S_n */

    for (son = son->son; son != NULL; son = son->brother) {
      if (son->name == ATTR_VAL_n) {
	/* son->son->brother->brother->name == {
	   ATTR_VAL_S_n,  NAMED_STRUCTURE_n, EMPTY_STRUCT_n, EMPTY_NAMED_STRUCT_n,
	   ATOM_S_n, NAMED_ATOM_n, EMPTY_ATOM_n, EMPTY_NAMED_ATOM_n,  
	   STRUCTURE_S_n, NAMED_ADJUNCT_n, EMPTY_ADJUNCT_n, EMPTY_NAMED_ADJUNCT_n
	   } */
	ret_val |= process_named_structure (son->son->brother->brother);
      }
      else {
	if (son->name == MACRO_n) {
	  /* <attr_val> = "@" %IDENT ; "MACRO" */
	  /* Utilisation d'une macro de structure, on l'etend, ca va permettre des verifs + faciles
	     sur les structures. */
	  macro_id = sxword_2retrieve (&macro_names,
				       sxstrget (son->token.string_table_entry),
				       sxstrlen (son->token.string_table_entry));

	  if (macro_id == ERROR_STE) {
	    /* Macro inconnue */
	    sxput_error (son->token.source_index,
			 "%sUnknown macro name: ignored.",
			 sxtab_ptr->err_titles [2]);
	    ret_val |= X80;
	  }
	  else {
	    if ((brother = macro_id2visited [macro_id])) {
	      /* La definition de la macro contient des structures nommees => on fait leurs verifications en contexte */
	      /* brother->name = ATTR_VAL_S_n */
	      /* struct_id = macro_id2structure [macro_id]; */
	      ret_val |= process_named_structure (brother);
	    }
	  }
	}
	/* else son->name = PRED_FIELD_n */
      }
    } 

    return ret_val;
    
  case STRUCTURE_S_n:
    for (son = visited->son; son != NULL; son = son->brother) {
      ret_val |= process_named_structure (son);
    }

    return ret_val;

  case NAMED_ATOM_n:
    /* visited->son->name = ATOM_S_n */
  case NAMED_ADJUNCT_n:
    /* visited->son->name = STRUCTURE_S_n */
  case NAMED_STRUCTURE_n:
    /* visited->son->name = ATTR_VAL_S_n */
    son = visited->son;

    ret_val |= process_named_structure (son);
    brother = son->brother;
    /* brother->name = ENTIER_n */

    if (i > 0 && i <= named_structure2visited_size) {
      if (!SXBA_bit_is_reset_set (named_structure_set, i)) {
	sxput_error (brother->token.source_index,
		     "%sThis non-empty named field is multiply defined.",
		     sxtab_ptr->err_titles [2]);
	ret_val |= X80;
      }
      else {
	/* Si i est le nom d'une valeur de champ, named_structure2visited [i] repere le noeud non vide (ATOM_S_n, STRUCTURE_S_n ou ATTR_VAL_S_n)
	   associe' s'il y en a un (SXBA_bit_is_set (named_structure_set, i)) */
	named_structure2visited [i] = son;
      }
    }
      
    return ret_val;

  default:
#if EBUG
    sxtrap(ME,"unknown switch case #2");
#endif
    /* NOTREACHED */
    return X80;     /* pour faire taire gcc -Wreturn-type */
  }
}

#if 0
/* Fusion des 2 tables ds la 1ere */
static void
merge_is_closed (unique_visited, unique_son)
     int unique_visited, unique_son;
{
  int *visited_ptr, *son_ptr, named_structure, nb;

  visited_ptr = T [unique_visited];
  son_ptr = T [unique_son];
      
  named_structure = 0;

  while ((named_structure = sxba_scan (empty_named_structure_set, named_structure)) > 0) {
    if (nb = son_ptr [named_structure])
      visited_ptr [named_structure] += nb;
  }
}

/* On verifie si T [unique_visited] est clos */
static void
set_is_closed (unique_visited)
     int unique_visited;
{
  int     *visited_ptr, named_structure = 0;

  visited_ptr = T [unique_visited];

  while ((named_structure = sxba_scan (empty_named_structure_set, named_structure)) > 0) {
    if (visited_ptr [named_structure] != named_structure2occur_nb [named_structure]) {
      visited_ptr [0] = 1; /* non clos */
      return;
    }
  }
}
#endif /* 0 */

/* Le 16/03/05 */
/* visited->name = ATTR_VAL_S_n || NAMED_STRUCTURE_n || EMPTY_NAMED_STRUCT_n*/
/* Y'a des f_structures nommees ds le coup */
/* une f_structure F est close ssi
   - aucune de ses sous-structures (strictes) n'est partagees (ou cyclique) ou
   - pour chaque sous-structures partagee ou cyclique le plus petit ancetre commun est un descendant
   non strict de F */
/* named_structure2occur_nb [named_structure] est rempli */
/* retourne le nom si visited est nomme' */


static void
check_glbl_id2fathers_sons ()
{
  int new_size;

  if (glbl_id2sons == NULL) {
    glbl_id2sons_size = 16+glbl_id; /* pourquoi pas !! */
    glbl_id2sons = sxbm_calloc (glbl_id2sons_size+1, glbl_id2sons_size+1);
    glbl_id2fathers = sxbm_calloc (glbl_id2sons_size+1, glbl_id2sons_size+1);
    glbl_id2not_closed = sxba_calloc (glbl_id2sons_size+1);
  }
  else {
    if (glbl_id > glbl_id2sons_size) {
      new_size = glbl_id+glbl_id2sons_size;
      glbl_id2sons = sxbm_resize (glbl_id2sons, glbl_id2sons_size+1, new_size+1, new_size+1);
      glbl_id2fathers = sxbm_resize (glbl_id2fathers, glbl_id2sons_size+1, new_size+1, new_size+1);
      glbl_id2sons_size = new_size;
      glbl_id2not_closed = sxba_resize (glbl_id2not_closed, glbl_id2sons_size+1);
    }
  }
}

static void
fill_glbl_id2fathers_sons (visited, father_id)
     NODE *visited;
     int father_id;
{
  int                  macro_id, son_id, i;
  NODE                 *son, *brother;

  if (visited->name == EMPTY_ATOM_n || visited->name == EMPTY_STRUCT_n || visited->name == EMPTY_ADJUNCT_n) {
    return;
  }

  if (visited->name == ATOM_S_n || visited->name == EMPTY_NAMED_ATOM_n) {
    /* Le 02/05/05 */
    /* la notion de fermeture s'etend aux atomes */
    /* ... mais bien sur, on ne descend pas ds les fils */
    son_id = visited->id;

    if (son_id) {
      /* nomme' */
      SXBA_1_bit (glbl_id2sons [father_id], son_id);
      SXBA_1_bit (glbl_id2fathers [son_id], father_id);
    }

    return;
  }


  if (visited->name == NAMED_ATOM_n) {
    /* Le 02/05/05 */
    /* la notion de fermeture s'etend aux atomes */
    fill_glbl_id2fathers_sons (visited->son, father_id);

    return;
  }

  if (visited->name == EMPTY_NAMED_STRUCT_n
      || visited->name == EMPTY_NAMED_ADJUNCT_n) {
    i = atoi (sxstrget (visited->token.string_table_entry));

    if (SXBA_bit_is_set (named_structure_set, i)) {
      /* Il y a une structure nommee non vide associee, on fait comme si c'etait elle ... */
      son = named_structure2visited [i];
      fill_glbl_id2fathers_sons (son, father_id);
    }
    else {
      son_id = visited->id;
      SXBA_1_bit (glbl_id2sons [father_id], son_id);
      SXBA_1_bit (glbl_id2fathers [son_id], father_id);
    }

    return;
  }

  if (visited->name == NAMED_STRUCTURE_n || visited->name == NAMED_ADJUNCT_n) {
    son = visited->son;
    /* son->name = ATTR_VAL_S_n  || son->name = STRUCTURE_S_n */
    fill_glbl_id2fathers_sons (son, father_id);

    return;
  }

  son = visited;

  if (son->name == STRUCTURE_S_n) {
    son_id = son->id;

    SXBA_1_bit (glbl_id2sons [father_id], son_id);
    SXBA_1_bit (glbl_id2fathers [son_id], father_id);

    if (son->brother) {
      i = atoi (sxstrget (son->brother->token.string_table_entry));

      if (!SXBA_bit_is_reset_set (named_structure2already_visited, i))
	/* visite unique d'une structure nommee => on ne boucle pas sur les cycliques
	   et on la visiterait sur toutes ses occurrences meme vides : []i */
	return;
    }

    for (son = son->son; son != NULL; son = son->brother) {
      fill_glbl_id2fathers_sons (son, son_id);
    } 

    return;
  }

  if (son->name == ATTR_VAL_S_n) {
    son_id = son->id;

    if (father_id) {
      SXBA_1_bit (glbl_id2sons [father_id], son_id);
      SXBA_1_bit (glbl_id2fathers [son_id], father_id);
    }

    if (son->brother) {
      i = atoi (sxstrget (son->brother->token.string_table_entry));

      if (!SXBA_bit_is_reset_set (named_structure2already_visited, i))
	/* visite unique d'une structure nommee => on ne boucle pas sur les cycliques
	   et on la visiterait sur toutes ses occurrences meme vides : []i */
	return;
    }

    for (son = son->son; son != NULL; son = son->brother) {
      if (son->name == ATTR_VAL_n) {
	fill_glbl_id2fathers_sons (son->son->brother->brother, son_id);
      }
      else {
	if (son->name == MACRO_n) {
	  /* <attr_val> = "@" %IDENT ; "MACRO" */
	  /* Utilisation d'une macro de structure, on l'etend, ca va permettre des verifs + faciles. */
	  macro_id = sxword_2retrieve (&macro_names,
				       sxstrget (son->token.string_table_entry),
				       sxstrlen (son->token.string_table_entry));

	  if ((brother = macro_id2visited [macro_id])) {
	    /* La definition de la macro contient des structures nommees */
	    /* brother->name = ATTR_VAL_S_n */
	    fill_glbl_id2fathers_sons (brother, son_id);
	  }
	}
	/* else son->name = PRED_FIELD_n */
      }
    } 

    return;
  }
}


static void
fill_not_closed ()
{
  int  son, i;
  SXBA sons, fathers;

  fermer (glbl_id2sons, glbl_id+1);
    
  fathers = glbl_id2fathers [0]; /* ensemble de travail */

  for (i = 1; i <= glbl_id; i++) {
    sons = glbl_id2sons [i];
    sxba_empty (fathers);
    son = glbl_id+1;

    while ((son = sxba_1_rlscan (sons, son)) > 0) {
      /* son est un fils de i */
      sxba_or (fathers, glbl_id2fathers [son]);
    }

    SXBA_1_bit (sons, i); /* ... au sens large */

    /* i est clos ssi les peres des descendants (stricts) sont des descendants (larges) */
    if (!sxba_is_subset (fathers, sons))
      SXBA_1_bit (glbl_id2not_closed, i);
  }
}



#if 0
static void
fill_named_structure2fathers_sons (visited, father_shared_id)
     NODE *visited;
     int father_shared_id;
{
  int                  macro_id, son_shared_id, i;
  NODE                 *son, *brother;

  if (visited->name == ATOM_S_n || visited->name == EMPTY_ATOM_n || visited->name == EMPTY_STRUCT_n || visited->name == EMPTY_ADJUNCT_n) {
    return;
  }

  if (visited->name == NAMED_ATOM_n || visited->name == EMPTY_NAMED_ATOM_n ) {
    /* Le 02/05/05 */
    /* la notion de fermeture s'etend aux atomes */
    if (visited->name == NAMED_ATOM_n)
      son = visited->son->brother;
    else
      son = visited;

    son_shared_id = atoi (sxstrget (son->token.string_table_entry));

    if (son_shared_id && father_shared_id) {
      SXBA_1_bit (named_structure2sons [father_shared_id], son_shared_id);
      SXBA_1_bit (named_structure2fathers [son_shared_id], father_shared_id);
    }

    return;
  }

  if (visited->name == EMPTY_NAMED_STRUCT_n
      || visited->name == EMPTY_NAMED_ADJUNCT_n) {
    i = atoi (sxstrget (visited->token.string_table_entry));

    if (SXBA_bit_is_set (named_structure_set, i)) {
      /* Il y a une structure nommee non vide associee, on fait comme si c'etait elle ... */
      son = named_structure2visited [i];
      fill_named_structure2fathers_sons (son, father_shared_id);
    }
    else {
      if (i && father_shared_id) {
	SXBA_1_bit (named_structure2sons [father_shared_id], i);
	SXBA_1_bit (named_structure2fathers [i], father_shared_id);
      }
    }

    return;
  }

  if (visited->name == NAMED_STRUCTURE_n || visited->name == NAMED_ADJUNCT_n) {
    son = visited->son;
    /* son->name = ATTR_VAL_S_n  || son->name = STRUCTURE_S_n */
    fill_named_structure2fathers_sons (son, father_shared_id);

    return;
  }

  son = visited;

  if (son->name == STRUCTURE_S_n) {
    son_shared_id = (son->brother) ? atoi (sxstrget (son->brother->token.string_table_entry)) : 0;

    if (son_shared_id && father_shared_id) {
      /* On ne met en relation que les structures nommees */
      SXBA_1_bit (named_structure2sons [father_shared_id], son_shared_id);
      SXBA_1_bit (named_structure2fathers [son_shared_id], father_shared_id);
    }

    if (son_shared_id) {
      if (!SXBA_bit_is_reset_set (named_structure2already_visited, son_shared_id))
	/* visite unique d'une structure nommee => on ne boucle pas sur les cycliques
	   et on la visiterait sur toutes ses occurrences meme vides : []i */
	return;
    }
    else
      son_shared_id = father_shared_id;

    for (son = son->son; son != NULL; son = son->brother) {
      fill_named_structure2fathers_sons (son, son_shared_id);
    } 

    return;
  }

  if (son->name == ATTR_VAL_S_n) {
    son_shared_id = (son->brother) ? atoi (sxstrget (son->brother->token.string_table_entry)) : 0;

    if (son_shared_id && father_shared_id) {
      /* On ne met en relation que les structures nommees */
      SXBA_1_bit (named_structure2sons [father_shared_id], son_shared_id);
      SXBA_1_bit (named_structure2fathers [son_shared_id], father_shared_id);
    }

    if (son_shared_id) {
      if (!SXBA_bit_is_reset_set (named_structure2already_visited, son_shared_id))
	/* visite unique d'une structure nommee => on ne boucle pas sur les cycliques
	   et on la visiterait sur toutes ses occurrences meme vides : []i */
	return;
    }
    else
      son_shared_id = father_shared_id;

    for (son = son->son; son != NULL; son = son->brother) {
      if (son->name == ATTR_VAL_n) {
	fill_named_structure2fathers_sons (son->son->brother->brother, son_shared_id);
      }
      else {
	if (son->name == MACRO_n) {
	  /* <attr_val> = "@" %IDENT ; "MACRO" */
	  /* Utilisation d'une macro de structure, on l'etend, ca va permettre des verifs + faciles. */
	  macro_id = sxword_2retrieve (&macro_names,
				       sxstrget (son->token.string_table_entry),
				       sxstrlen (son->token.string_table_entry));

	  if (brother = macro_id2visited [macro_id]) {
	    /* La definition de la macro contient des structures nommees */
	    /* brother->name = ATTR_VAL_S_n */
	    fill_named_structure2fathers_sons (brother, son_shared_id);
	  }
	}
	/* else son->name = PRED_FIELD_n */
      }
    } 

    return;
  }
}

static void
fill_not_closed ()
{
  int  son, i;
  SXBA sons, fathers;

  fermer (named_structure2sons, named_structure2visited_top+1);

  while ((i = sxba_1_rlscan (named_structure_set, i)) > 0) {
    if (named_structure2visited [i]->name == ATTR_VAL_S_n) {
      sons = named_structure2sons [i];
      fathers = named_structure2fathers [i];
      son = named_structure2visited_top+1;

      while ((son = sxba_1_rlscan (sons, son)) > 0) {
	/* son est un fils de i */
	sxba_or (fathers, named_structure2fathers [son]);
      }

      SXBA_1_bit (sons, i); /* ... au sens large */

      /* i est clos ssi les peres des descendants (stricts) sont des descendants (larges) */
      if (!sxba_is_subset (fathers, sons))
	SXBA_1_bit (named_structure2not_closed, i);
    }
  }
}
#endif /* 0 */

#if 0
static int
fill_is_closed (visited, unique)
     NODE *visited;
     int  *unique;
{
  int     i, macro_id, unique_visited, unique_son;
  NODE    *son, *brother;
  int     named_structure;

  if (visited->name == ATOM_S_n || visited->name == EMPTY_STRUCT_n || visited->name == EMPTY_ATOM_n || visited->name == EMPTY_ADJUNCT_n) {
    *unique = 0;
    return 0;
  }

  if (visited->name == EMPTY_NAMED_STRUCT_n
      || visited->name == EMPTY_NAMED_ATOM_n || visited->name == EMPTY_NAMED_ADJUNCT_n
      || visited->name == NAMED_ATOM_n) {
    *unique = 0;
    return atoi (sxstrget (visited->token.string_table_entry));
  }

  if (visited->name == NAMED_STRUCTURE_n || visited->name == NAMED_ADJUNCT_n) {
    son = visited->son;
    /* son->name = ATTR_VAL_S_n */
    /* son->name = STRUCTURE_S_n */

    /* L'identifiant unique du fils est passe' a l'appelant */
    named_structure = fill_is_closed (son, unique);

    brother = son->brother;
    named_structure = atoi (sxstrget (brother->token.string_table_entry));

    return named_structure;
  }

  son = visited;

  if (son->name == STRUCTURE_S_n) {
    /* Attention, on utilise visited->id en temporaire */
    visited->id = *unique = unique_visited = ++T_unique;
    is_closed = TRUE;

    for (son = son->son; son != NULL; son = son->brother) {
      named_structure = fill_is_closed (son, &unique_son);
      /* unique_son est l'identifiant du ATTR_VAL_S_n [petit-]fils */

      if (unique_son) {
	if (T [unique_son] [0])
	  is_closed = FALSE;

	merge_is_closed (unique_visited, unique_son);
      }
    } 

    named_structure = 0; /* non nommee */

    if (!is_closed)
      T [unique_visited] [0] = 1; /* L'adjunct contient une f_structure nonclose */
  }
  else {
    /* son->name == ATTR_VAL_S_n */
    /* Attention, on utilise visited->id en temporaire */
    visited->id = *unique = unique_visited = ++T_unique;

    for (son = son->son; son != NULL; son = son->brother) {
      if (son->name == ATTR_VAL_n) {
	named_structure = fill_is_closed (son->son->brother->brother, &unique_son);
	/* unique_son est l'identifiant du ATTR_VAL_S_n [petit-]fils */

	if (unique_son)
	  merge_is_closed (unique_visited, unique_son);

	if (named_structure)
	  T [unique_visited] [named_structure] += 1;
      }
      else {
	if (son->name == MACRO_n) {
	  /* <attr_val> = "@" %IDENT ; "MACRO" */
	  /* Utilisation d'une macro de structure, on l'etend, ca va permettre des verifs + faciles
	     sur les structures. */
	  macro_id = sxword_2retrieve (&macro_names,
				       sxstrget (son->token.string_table_entry),
				       sxstrlen (son->token.string_table_entry));

	  if (brother = macro_id2visited [macro_id]) {
	    /* La definition de la macro contient des structures nommees */
	    /* brother->name = ATTR_VAL_S_n */
	    named_structure = fill_is_closed (brother, &unique_son);
	    /* unique_son est l'identifiant du ATTR_VAL_S_n [petit-]fils */
	
	    if (unique_son)
	      merge_is_closed (unique_visited, unique_son);

	    if (named_structure)
	      T [unique_visited] [named_structure] += 1;
	  }
	}
	/* else son->name = PRED_FIELD_n */
      }
    } 

    named_structure = 0; /* non nommee */
    /* verifier si clos */
    set_is_closed (unique_visited);
  }

  return named_structure;
}
#endif /* 0 */

/* visited->name = {ATTR_VAL_S_n, NAMED_STRUCTURE_n} */
static BOOLEAN
call_process_named_structure (visited)
     NODE *visited;
{
  int  named_structure, id;
  int  ret_val;
  NODE *son;

#if 0
  sxba_empty (named_structure2not_closed);
#endif /* 0 */
  named_structure2visited_top = 0;
  glbl_id = 0;

  ret_val = process_named_structure (visited);
      
  if (glbl_id2not_closed)
    sxba_empty (glbl_id2not_closed);

  if (ret_val == 1) {
    /* OK et y'a des structures nommees */
    /* named_structure_set doit etre inclus ds empty_named_structure_set */
    if (!sxba_is_subset (named_structure_set, empty_named_structure_set)) {
#if 0
      sxba_or (empty_named_structure_set, named_structure_set); /* pour raz */
#endif /* 0 */
      ret_val |= X80;
    }
    else {
      named_structure = 0;

      while ((named_structure = sxba_scan (empty_named_structure_set, named_structure)) > 0) {
	if (named_structure2occur_nb [named_structure] == 1) {
	  /* Une seule occurrence du nom named_structure */
	    ret_val |= X80;
	    break;
	}
      }
    }

    if (ret_val & X80)
      sxput_error (visited->token.source_index,
		   "%sUnconsistent naming of shared sub-structures.",
		   sxtab_ptr->err_titles [2]);
    else {
      /* on regarde si les structures sont closes */
      check_glbl_id2fathers_sons ();

      /* visited->name = {ATTR_VAL_S_n, NAMED_STRUCTURE_n} */
      if (visited->name == NAMED_STRUCTURE_n)
	son = visited->son;
      else
	son = visited;

#if EBUG
      if (son->id == 0)
	sxtrap (ME, "call_process_named_structure");
#endif /* EBUG */

      fill_glbl_id2fathers_sons (son, 0 /* pere bidon */);

#if 0
      father_shared_id = 0;
      fill_named_structure2fathers_sons (visited, father_shared_id);
#endif /* 0 */

      fill_not_closed ();

#if 0
      fill_is_closed (visited, &unique);
#endif /* 0 */
    }
  }

  for (named_structure = 1; named_structure <= named_structure2visited_top; named_structure++) {
#if 0
    sxba_empty (named_structure2sons [named_structure]);
    sxba_empty (named_structure2fathers [named_structure]);
#endif /* 0 */
    named_structure2visited [named_structure] = NULL;
    named_structure2occur_nb [named_structure] = 0;
    named_structure2id [named_structure] = 0;
  }

  if (glbl_id2sons) {
    for (id = 1; id <= glbl_id; id++) {
      sxba_empty (glbl_id2sons [id]);
      sxba_empty (glbl_id2fathers [id]);
    }
  }

  sxba_empty (empty_named_structure_set);
  sxba_empty (named_structure_set);
  sxba_empty (named_structure2already_visited);

  return ret_val >= 0; /* pas d'erreur */
}


static void lfg_lex_pi () {

  /*
    I N H E R I T E D
  */
  int        field_id, atom_id, field_top, macro_top, macro_id;
  NODE       *father;

  switch ((father = VISITED->father)->name) {

  case ERROR_n :
    break;

  case AMALGAM_n :/* VISITED->name = AMALGAM_S_n */
    AMALGAM_VISITED = VISITED;
    /* On saute le sous-arbre */
    /* On le visitera + tard a partir de AMALGAM_VISITED quand on en saura + sur le dico des formes flechies (simples) */
    sxat_snv (DERIVED, VISITED->father);
    break;

  case AMALGAM_S_n :/* VISITED->name = IDENT_S_n */
    break;

  case ARGUMENT_n :/* VISITED->name = ARGUMENT_S_n */
    break;

  case ARGUMENT_S_n :/* VISITED->name = {CHAINE_n, IDENT_n, PRED_n} */
    /* 
       <argument_LIST> = <argument_LIST> <graphie> ";" ;
       <argument_LIST> = <graphie> ";" ;	"ARGUMENT_S"
    */      
    if (father->father->name == ARGUMENT_n) {
      /* <declaration> = ARGUMENT <argument_LIST> ; "ARGUMENT" */
      /* Les IDENT_n sont des noms de structure fonctionnelle */
      if (VISITED->name == IDENT_n || VISITED->name == CHAINE_n)
	field_id = sxword_2retrieve (&field_names,
				     sxstrget (VISITED->token.string_table_entry),
				     sxstrlen (VISITED->token.string_table_entry));
      else
	field_id = pred_id;

      if (field_id == ERROR_STE) {
	sxput_error (VISITED->token.source_index,
		     "%sUndeclared field name.",
		     sxtab_ptr->err_titles [2]);
      }
      else {
	if ((field_id2kind [field_id] & STRUCT_KIND) == 0)
	  sxput_error (VISITED->token.source_index,
		       "%sThis field name must be a functional attribute.",
		       sxtab_ptr->err_titles [1]);
	else {
	  /* Depuis le 02/08/04 les arguments peuvent aussi se declarer par "<[]>"
	     Le paragraphe "ARGUMENT" est cependant conserve'. On ne detecte donc plus les
	     declarations multiples */
	  SXBA_1_bit (argument_set, field_id);
#if 0
	  if (!SXBA_bit_is_reset_set (argument_set, field_id))
	    sxput_error (VISITED->token.source_index,
			 "%sMultiple functional attribute declaration.",
			 sxtab_ptr->err_titles [1]);
#endif /* 0 */
	}
      }
      break;
    }

    break;

  case ATOM_S_n :/* VISITED->name = {ATOM_MINUS_n, ATOM_PLUS_n, CHAINE_n, ENTIER_n, IDENT_n, PRED_n} */
    if (father->father->name == ATTR_VAL_n) {
      /* Utilisation d'un atome comme valeur de champ */
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
      default:
	sxtrap (ME, "lfg_lex_pi (ATOM_S)");
      }
      break;
    }

    father = father->father;

    while (father->name == OPT_DISJONCTION_n)
      father = father->father;

    if (father->name != DISJONCTION_S_n)
      sxtrap (ME, "lfg_lex_pi (ATOM_S)");

    if (father->father->name == ATTR_ATOMS_n || father->father->name == ATTR_DOTS_n) {
      /* definition d'un atome */
      if (current_field_id == 0)
	sxtrap (ME, "lfg_lex_pi");

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
	  sxtrap (ME, "lfg_lex_pi (ATOM_S)");
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
	  else
	    /* On dit qu'il est utilise' !! */
	    SXBA_1_bit (referenced_field_set, field_id);
	}
	break;
      default:
	sxinitialise(field_id); /* pour faire taire gcc -Wuninitialized */
	sxtrap (ME, "lfg_lex_pi (ATOM_S)");
      }

      VISITED->id = field_id;
      break;
    }
    else
      sxtrap (ME, "lfg_lex_pi (ATOM_S)");

    break;

  case ATTRIBUTE_n :/* VISITED->name = ATTRIBUTE_S_n */
    break;

  case ATTRIBUTE_PASS_NB_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {ATTR_ARG_STRUCT_n, ATTR_ATOMS_n, ATTR_DOTS_n, ATTR_SET_OF_STRUCT_n, ATTR_STRUCT_n} */
      break;
      
    case 2 :/* VISITED->name = {ENTIER_n, VOID_n} */
      field_id2pass_nb [father->son->id] = (VISITED->name == VOID_n) ? 1 : atoi (sxstrget (VISITED->token.string_table_entry));
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
      father->id = field_id = sxword_2save (&field_names,
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
      father->id = field_id = sxword_2save (&field_names,
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
    if (VISITED->position == 1)
      /* On penetre un nouveau niveau de structure */
      incr_level ();
    break;

  case CATEGORY_n :/* VISITED->name = ARGUMENT_S_n */
    /* inutilise' ici */
    sxat_snv (DERIVED, VISITED->father);
    break;

  case COMPLETE_n :/* VISITED->name = PAIR_S_n */
    /* inutilise' ici */
    sxat_snv (DERIVED, VISITED->father);
    break;

  case COMPOUND_n :/* VISITED->name = COMPOUND_S_n */
    COMPOUND_VISITED = VISITED;
    /* On saute le sous-arbre */
    /* On le visitera + tard a partir de COMPOUND_VISITED quand on en saura + sur le dico des formes flechies (simples) */
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

  case IDENT_S_n :/* VISITED->name = {CHAINE_n, IDENT_n} */
    break;

  case LEXICAL_GRAMMAR_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = DECLARATION_S_n */
      break;

    case 2 :/* VISITED->name = RULES_S_n */
      current_field_id = 0;
      break;

    default:
#if EBUG
      sxtrap(ME,"unknown switch case #7");
#endif
      break;
    }

    break;

  case MACRO_STRUCTURE_n :
    /* <declaration> = "@" %IDENT "=" <structure> ";" ; "MACRO_STRUCTURE" */
    switch (VISITED->position) {
    case 1 :/* VISITED->name = MACRO_NAME_n */
      macro_top = SXWORD_top (macro_names);
      macro_id = sxword_2save (&macro_names,
			      sxstrget (VISITED->token.string_table_entry),
			      sxstrlen (VISITED->token.string_table_entry));

      if (SXWORD_top (macro_names) == (unsigned)macro_top) {
	/* Redeclaration */
	sxput_error (VISITED->token.source_index,
		     "%sMacro name already defined: ignored.",
		     sxtab_ptr->err_titles [2]);
	  
	sxat_snv (DERIVED, father);
      }
      else {
	VISITED->id = macro_id;
      }
      break;

    case 2 :/* VISITED->name = {ATTR_VAL_S_n, EMPTY_NAMED_STRUCT_n, EMPTY_STRUCT_n, NAMED_STRUCTURE_n} */
      has_named_structure_inside_macro = FALSE;

      if (VISITED->name != ATTR_VAL_S_n && VISITED->name != EMPTY_STRUCT_n) {
	/* On ne permet que des vraies structures non nommees ... */
	sxput_error (VISITED->token.source_index,
		     "%sIllegal macro definition: ignored.",
		     sxtab_ptr->err_titles [2]);
	  
	father->son->id = 0; /* ERREUR */
	sxat_snv (DERIVED, father);
      }
      else {
	/* Le 02/08/05 */
	/* Il peut y avoir des structures nommees et donc des non closes ds les macros */
	if (VISITED->name == ATTR_VAL_S_n) {
	  if (!call_process_named_structure (VISITED))
	    sxat_snv (DERIVED, father);
	}
      }
      break;

    default:
#if EBUG
      sxtrap(ME,"unknown switch case #8");
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
      sxtrap(ME,"unknown switch case #9");
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
	  sxtrap(ME,"unknown switch case #10");
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
      sxtrap(ME,"unknown switch case #11");
#endif
      break;
    }

    break;

  case OPT_DISJONCTION_n :/* VISITED->name = {ATOM_S_n, OPT_DISJONCTION_n} */
    break;

  case PAIR_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {CHAINE_n, IDENT_n, PRED_n} */
      break;

    case 2 :/* VISITED->name = CHAINE_n */
      break;

    default:
#if EBUG
      sxtrap(ME,"unknown switch case #12");
#endif
      break;
    }

    break;

  case PAIR_S_n :/* VISITED->name = {CHAINE_n, IDENT_n, PAIR_n, PRED_n} */
    break;

  case PRED_FIELD_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {FIELD_CONSTRAINT_n, FIELD_EQUAL_n, FIELD_OPTIONAL_n} */
      break;

    case 2 :/* VISITED->name = {CHAINE_n, IDENT_n, PRED_n} */
      if (VISITED->name == PRED_n)
	VISITED->id = sxword_2save (&lexeme_names, "pred", 4);
      else {
	char *lexeme_string;

	lexeme_string = sxstrget (VISITED->token.string_table_entry);

#if 0 /* incompr�hensible, p� issu de Lionel, g�n�re des bugs et ne sert plus � rien. */
	if (*lexeme_string == '_') {
	  gpa = father->father->father;

	  if (gpa->name != RULE_n && gpa->name != RULE_n) {
	    sxput_error (VISITED->token.source_index,
			 "%sOnly lexeme names in main structures can be prefixed by '_'.",
			 sxtab_ptr->err_titles [2]);
	    lsxat_snv (DERIVED, father);
	  }
	  else {
	    i = lexeme_string [1]-'0';

	    if (isdigit (lexeme_string [2])) {
	      i *= 10;
	      i += lexeme_string [2]-'0';
	      d = 3;
	    }
	    else
	      d = 2;
  
	    if_node = gpa->son;

	    if (if_node->name == MOT_1COL_n)
	      terminal_string = sxstrget (if_node->token.string_table_entry);
	    else
	      terminal_string = sxttext (sxtab_ptr, if_node->token.lahead);

	    varstr_raz (vstr);
	    vstr = varstr_catenate (vstr, terminal_string);
	    *(vstr->current = vstr->first+i) = NUL; /* !! */
	    vstr = varstr_catenate (vstr, lexeme_string+d);
	    lexeme_string = varstr_tostr (vstr);
	  }
	}
#endif /* 0 */

	VISITED->id = sxword_2save (&lexeme_names,
				    lexeme_string,
				    strlen (lexeme_string));
      }

      break;

    case 3 :/* VISITED->name = {DISJONCTION_S_n, EMPTY_SOUS_CAT_n, SOUS_CAT_n} */
      break;

    default:
#if EBUG
      sxtrap(ME,"unknown switch case #13");
#endif
      break;
    }

    break;

  case PRIORITY_S_n :/* VISITED->name = {PRIORITY_n, PRIORITY_MAX_n} */
    break;

  case RULE_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = {MOT_1COL_n} */
      break;

    case 2 :/* VISITED->name = PRIORITY_S_n */
      break;

    case 3 :/* VISITED->name = {IDENT_n, PRED_n} */
      break;

    case 4 :/* VISITED->name = {ATTR_VAL_S_n, EMPTY_STRUCT_n, NAMED_STRUCTURE_n, VOID_n} */
      /* La grammaire interdit EMPTY_NAMED_STRUCT_n */
      if (VISITED->name != EMPTY_STRUCT_n && VISITED->name != VOID_n) {
	if (!call_process_named_structure (VISITED))
	  sxat_snv (DERIVED, father);
      }
      break;

    default:
#if EBUG
      sxtrap(ME,"unknown switch case #14");
#endif
      break;
    }

    break;

  case RULES_S_n :/* VISITED->name = {RULE_n} */
    break;

  case SOUS_CAT_n :
    switch (VISITED->position) {
    case 1 :/* VISITED->name = DISJONCTION_S_n */
      break;

    case 2 :/* VISITED->name = DISJONCTION_S_n */
      break;

    default:
#if EBUG
      sxtrap(ME,"unknown switch case #15");
#endif
      break;
    }

    break;

  case STRUCTURE_S_n :/* VISITED->name = {ATTR_VAL_S_n, EMPTY_NAMED_STRUCT_n, EMPTY_STRUCT_n, NAMED_STRUCTURE_n} */
    break;

  case TERMINAL_n :/* VISITED->name = ARGUMENT_S_n */
    /* Ci-dessous, ds ARGUMENT_S_n on a la liste des terminaux de la langue decrite */
    /* Il faut lire les tables bnf (langue.bt) construites par bnf passant sur langue.lfg 
       Ca va permettre de connaitre leur code interne
       Verifier que les listes ci_dessous et bnf sont identiques
       Verifier, lors de la construction du dico que le terminal associe a chaque forme flechie
       est bien un terminal de la lfg (et on peut signaler en warning les terminaux non couverts
       par des formes flechies)
     */
    if (tmax == 0) {
      /* on croit ce qui est dit ici, l'ordre donne le code interne!! */
      int  t_id, prev_t_id;
      NODE *son;

      sxword_alloc (&t_names, "t_names", VISITED->degree+2, 1, 16,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
      t_id2t = (int *) sxalloc (VISITED->degree+2, sizeof (int));
      t_id2t [ERROR_STE] = t_id2t [EMPTY_STE] = 0;
      t_id_set = sxba_calloc (VISITED->degree+2);
      prev_t_id = 0;
      
      for (son = VISITED->son; son != NULL; son = son->brother) {
	t_id = sxword_2save (&t_names,
			    sxstrget (son->token.string_table_entry),
			    sxstrlen (son->token.string_table_entry));

	if (t_id > prev_t_id) {
	  t_id2t [prev_t_id = t_id] = ++tmax;
	}
	else {
	  sxput_error (son->token.source_index,
		       "%sDuplicate: ignored.",
		       sxtab_ptr->err_titles [1]);
	}
      }

      TERMINAL_nb = tmax;
    }
    break;



    /*
      Z Z Z Z
    */

  default:
    break;
  }
  /* end lfg_lex_pi */
}


static void
alloc_struct_index_set (alloc_struct_index_set_index)
     int alloc_struct_index_set_index;
{
  if (struct_index_set == NULL) {
    max_struct_index = alloc_struct_index_set_index+10;
    struct_index_set = sxba_calloc (max_struct_index+1);
    struct_index_set2 = sxba_calloc (max_struct_index+1);
    cyclic_index_set = sxba_calloc (max_struct_index+1);
    struct_index2struct_id = (int*) sxcalloc (max_struct_index+1, sizeof (int));
    struct_index2field_id = (int*) sxalloc (max_struct_index+1, sizeof (int));
  }
  else {
    if (alloc_struct_index_set_index > max_struct_index) {
      struct_index_set = sxba_resize (struct_index_set, alloc_struct_index_set_index+10+1);
      struct_index_set2 = sxba_resize (struct_index_set2, alloc_struct_index_set_index+10+1);
      cyclic_index_set = sxba_resize (cyclic_index_set, alloc_struct_index_set_index+10+1);
      struct_index2struct_id = (int*) sxrecalloc (struct_index2struct_id, max_struct_index+1,
						  alloc_struct_index_set_index+10+1, sizeof (int));
      struct_index2field_id = (int*) sxrealloc (struct_index2field_id, alloc_struct_index_set_index+10+1, sizeof (int));
      max_struct_index = alloc_struct_index_set_index+10;
    }
  }

  if (alloc_struct_index_set_index > max_shared_f_structure_id)
    max_shared_f_structure_id = alloc_struct_index_set_index;
}



static void lfg_lex_pd () {

  /*
    D E R I V E D
  */

  int           field_top, field_id, atom_id, macro_id, compound_field_id, struct_id, name;
  SXBA          current_atom_set;
#ifndef ESSAI
  int           *int_ptr, *atom2local_id, *local2atom_id;
#endif /* ESSAI */
  SXNODE        *brother, *son, *father;
  BOOLEAN       is_main_structure;
  unsigned char static_field_kind;

  sxinitialise(is_main_structure); /* pour faire taire gcc -Wuninitialized */
  switch (VISITED->name) {

  case ERROR_n :
    break;

  case AMALGAM_n :
    break;

  case AMALGAM_S_n :
    break;

  case ARGUMENT_n :
    break;

  case ARGUMENT_S_n :
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
	sxtrap (ME, "lfg_lex_pd");

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
    max_static_atom_id = SXWORD_top (atom_names)-1;
    alloc_level ();
    working_field_set = sxba_calloc (max_field_id+1);
    pred_arg_set = sxba_calloc (max_field_id+1);
    warg_set = sxba_calloc (max_field_id+1);

#ifndef ESSAI
    fieldXatom2local_id = (int**) sxalloc (max_field_id+1, sizeof (int*));
    int_ptr = local_id_area = (int*) sxcalloc ((max_field_id+1)*(max_static_atom_id+1), sizeof (int));

    for (field_id = 0; field_id <= max_field_id; field_id++) {
      fieldXatom2local_id [field_id] = int_ptr;
      int_ptr += max_static_atom_id+1;
    }

    fieldXlocal2atom_id = (int**) sxalloc (max_field_id+1, sizeof (int*));
    int_ptr = local_id_area2 = (int*) sxcalloc ((max_field_id+1)*(max_local_atom_id+1), sizeof (int));

    for (field_id = 0; field_id <= max_field_id; field_id++) {
      fieldXlocal2atom_id [field_id] = int_ptr;
      int_ptr += max_local_atom_id+1;
    }

    max_local_atom_id = 0;

    for (field_id = EMPTY_STE+1; field_id <= max_field_id; field_id++) {
      static_field_kind = field_id2kind [field_id];

      if ((static_field_kind & ATOM_KIND) && ((static_field_kind & Aij_KIND) == 0)) {
	current_atom_set = field_id2atom_set [field_id];
	atom2local_id = fieldXatom2local_id [field_id];
	local2atom_id = fieldXlocal2atom_id [field_id];
	local_atom_id = 0;
	atom_id = 0;

	while ((atom_id = sxba_scan (current_atom_set, atom_id)) > 0) {
	  local_atom_id++;
	  atom2local_id [atom_id] = local_atom_id;
	  local2atom_id [local_atom_id] = atom_id;
	}

	if (static_field_kind == ATOM_KIND) {
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
#else /* ESSAI */
    for (field_id = EMPTY_STE+1; field_id <= max_field_id; field_id++) {
      static_field_kind = field_id2kind [field_id];

      if ((static_field_kind & ATOM_KIND) && ((static_field_kind & Aij_KIND) == 0)) {
	if (static_field_kind == ATOM_KIND)
	  max_atom_field_nb++;
	else
	  max_undef_atom_field_nb++;
      }
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
    VISITED->id = field_id = sxword_2save (&field_names,
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
      SXBA_1_bit (argument_set, field_id); /* C'est un argument */
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
    /* Les atomes de <disjonction*> sont ceux que l'on va trouver ds la lfg ds les equations definissant les f-structures */
    /* Les atomes de ... sont ceux du niveau lexical */
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
    VISITED->id = field_id = sxword_2save (&field_names,
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
      field_id2kind [field_id] = (STRUCT_KIND+UNBOUNDED_KIND);
    }
    break;

  case ATTR_STRUCT_n :
    /* <attribut> = %IDENT "=" "[" "]" ; "ATTR_STRUCT" */
    field_top = SXWORD_top (field_names);
    VISITED->id = field_id = sxword_2save (&field_names,
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
    /* <attr_val> = <attr> <field_oper> <val> ; "ATTR_VAL" 
       <attr> = %IDENT ; "FIELD"	
       <attr> = "(" %IDENT ")" ; "OPT_FIELD"
       <field_oper> = "=" ; "FIELD_EQUAL"
       <field_oper> = CONTRAINT ; "FIELD_CONSTRAINT" 
       <val> = <structure> ;
       <val> = <atom_LIST> ;
       <val> = "(" <atom_LIST> ")" %ENTIER ; "NAMED_ATOM"
       <val> = "(" ")" ; "EMPTY_ATOM"
       <val> = "(" ")" %ENTIER ; "EMPTY_NAMED_ATOM"
       <val> = "{" <structure_LIST> "}" ;
       <val> = "{" <structure_LIST> "}" %ENTIER ; "NAMED_ADJUNCT"
       <val> = "{" "}" ; "EMPTY_ADJUNCT"
       <val> = "{" "}" %ENTIER ; "EMPTY_NAMED_ADJUNCT"
       <structure_LIST> = <structure_LIST> "," <structure> ;
       <structure_LIST> = <structure> ; "STRUCTURE_S"
       <structure> = <main_structure> ;
       <structure> = "[" <attr_val_LIST> "]" %ENTIER ; "NAMED_STRUCTURE"
       <structure> = "[" "]" %ENTIER ;	"EMPTY_NAMED_STRUCT"
       <main_structure>	= "[" <attr_val_LIST> "]" ;
       <main_structure>	= "[" "]" ; "EMPTY_STRUCT" 
    */
    /* C'est le champ d'une structure */
    brother = VISITED->son;
    field_id = sxword_2retrieve (&field_names,
				 sxstrget (brother->token.string_table_entry),
				 sxstrlen (brother->token.string_table_entry));

    if (field_id == ERROR_STE) {
      sxput_error (brother->token.source_index,
		   "%sUnknown field name: ignored.",
		   sxtab_ptr->err_titles [2]);
    }
    else {
      /* On dit qu'il est utilise' !! */
      SXBA_1_bit (referenced_field_set, field_id);
      static_field_kind = field_id2kind [field_id];

      brother = brother->brother;
      field_shape [field_id] = (brother->name == FIELD_EQUAL_n) ? 0 : (brother->name == FIELD_CONSTRAINT_n ? ASSIGN_CONSTRAINT_KIND : ASSIGN_OPTION_KIND /* FIELD_OPTIONAL_n */);

      brother = brother->brother;

      if (((brother->name == ATOM_S_n || brother->name == NAMED_ATOM_n || brother->name == EMPTY_ATOM_n || brother->name == EMPTY_NAMED_ATOM_n)
	   && (static_field_kind & ATOM_KIND))
	  || ((brother->name == EMPTY_STRUCT_n || brother->name == ATTR_VAL_S_n || brother->name == NAMED_STRUCTURE_n || brother->name == EMPTY_NAMED_STRUCT_n)
	      && (static_field_kind == STRUCT_KIND))
	  || ((brother->name == EMPTY_ADJUNCT_n || brother->name == STRUCTURE_S_n || brother->name == NAMED_ADJUNCT_n || brother->name == EMPTY_NAMED_ADJUNCT_n)
	  && (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND))) {
	/* Type du champ coherant avec la valeur */
	char              *str;
	int               name_id;


#ifndef ESSAI
	if (static_field_kind & ATOM_KIND) {
	  /* brother->name == ATOM_S_n || brother->name == NAMED_ATOM_n || brother->name == EMPTY_ATOM_n || brother->name == EMPTY_NAMED_ATOM_n */
	  /* Utilisation d'un atome comme valeur d'un attribut */
	  /* <atom_LIST> = <atom_LIST> "|" <atome> ;
	     <atom_LIST> = <atome> ; "ATOM_S" */
	  unsigned long int atom_value;
	  int               local_atom_id;
	  int               *atom2local_id;

	  if (brother->name == ATOM_S_n && (static_field_kind & UNBOUNDED_KIND) && brother->degree > 1) {
	    sxput_error (brother->token.source_index, 
			 "%sImplementation restriction: disjunctions are not allowed on unbounded atomic types.",
			 sxtab_ptr->err_titles [2]);
	    break;
	  }
	    
	  if (brother->name == NAMED_ATOM_n || brother->name == EMPTY_NAMED_ATOM_n) {
	    if (brother->name == NAMED_ATOM_n) {
	      brother = brother->son; /* brother->name == ATOM_S_n */
	      str = sxstrget (brother->brother->token.string_table_entry);
	    }
	    else {
	      /* brother->name == EMPTY_NAMED_ATOM_n */
	      str = sxstrget (brother->token.string_table_entry);
	    }

	    name_id = atoi (str);
	    alloc_struct_index_set (name_id);

	    /* On continue a mettre l'index ds kind !! */
	    /* il est aussi en fin de structure (en negatif) */
	    field_shape [field_id] |= name_id << STRUCTURE_NAME_shift;
	  }
	  else {
	    /* brother->name == ATOM_S_n || brother->name == EMPTY_ATOM_n */
	    name_id = 0;
	  }
	    
	  atom_value = 0;

	  if (brother->name == ATOM_S_n) {
	    current_atom_set = field_id2atom_set [field_id];
	    atom2local_id = fieldXatom2local_id [field_id];

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
		  local_atom_id = atom_id = ERROR_STE;
		}
		else
		  local_atom_id = atom2local_id [atom_id];

		atom_value |= (1 << local_atom_id);
	      }
	    }
	    else {
	      if (brother->son->id == ERROR_STE && (static_field_kind & UNBOUNDED_KIND)) {
		/* ... on le definit */
		brother->son->id = sxword_2save (&atom_names,
						 sxstrget (brother->son->token.string_table_entry),
						 sxstrlen (brother->son->token.string_table_entry));
	      }

	      atom_id = brother->son->id;

	      if (!SXBA_bit_is_set (current_atom_set, atom_id)) {
		if (static_field_kind & UNBOUNDED_KIND)
		  atom_value = atom_id;
		else {
		  sxput_error (brother->son->token.source_index,
			       "%sUndeclared atom: ignored.",
			       sxtab_ptr->err_titles [2]);
		  atom_value = ERROR_STE;
		}
	      }
	      else {
		/* Les codes des atomes des types non bornes definis ds la grammaire doivent figurer ds
		   la def du type : {a, b, c ...}" */
		/* Ce n'est pas le cas de ceux du niveau lexical */
		if (static_field_kind & UNBOUNDED_KIND) {
		  if (atom_id == atom_Aij || atom_id == atom_Pij) {
		    sxput_error (brother->son->token.source_index,
				 "%sCannot be used in lexical structures: ignored.",
				 sxtab_ptr->err_titles [2]);
		    atom_value = ERROR_STE;
		  }
		  else
		    atom_value = atom_id;
		}
		else
		  atom_value = (1 << atom2local_id [atom_id]);
	      }
	    }
	  }
	    
	  if (brother->name == EMPTY_ATOM_n)
	    struct_id = 0;
	  else {
	    /* Les atomes vides, partages ont des struct_id non vides */
	    /* Le 04/05/05 On ne marque comme partagee que les atomes vides =>
	       si on a [... = ()1 .... = (a)1 ... = a ...], c'est equivalent a [... = a .... = a ... = a ...] */
	    if (atom_value)
	      XH_push (atom_hd, atom_value);
	    else {
	      if (name_id)
		XH_push (atom_hd, X80+X20+name_id);
	    }

	    XH_set (&atom_hd, &struct_id);
	  }
	}
#else  /* ESSAI */
	if (static_field_kind & ATOM_KIND) {
	  /* brother->name == ATOM_S_n || brother->name == NAMED_ATOM_n || brother->name == EMPTY_ATOM_n || brother->name == EMPTY_NAMED_ATOM_n */
	  /* Utilisation d'un atome comme valeur d'un attribut */
	  /* <atom_LIST> = <atom_LIST> "|" <atome> ;
	     <atom_LIST> = <atome> ; "ATOM_S" */

	  /* Maintenant les disjonctions sont permises sur les "unbounded atomic types" */
	    
	  if (brother->name == NAMED_ATOM_n || brother->name == EMPTY_NAMED_ATOM_n) {
	    if (brother->name == NAMED_ATOM_n) {
	      brother = brother->son; /* brother->name == ATOM_S_n */
	      str = sxstrget (brother->brother->token.string_table_entry);
	    }
	    else {
	      /* brother->name == EMPTY_NAMED_ATOM_n */
	      str = sxstrget (brother->token.string_table_entry);
	    }

	    name_id = atoi (str);
	    alloc_struct_index_set (name_id);

	    /* On continue a mettre l'index ds kind !! */
	    /* il est aussi en fin de structure (en negatif) */
	    field_shape [field_id] |= name_id << STRUCTURE_NAME_shift;
	  }
	  else {
	    /* brother->name == ATOM_S_n || brother->name == EMPTY_ATOM_n */
	    name_id = 0;
	  }
	    
	  if (brother->name == ATOM_S_n) {
	    current_atom_set = field_id2atom_set [field_id];

	    if (brother->degree) {
	      for (son = brother->son; son != NULL; son = son->brother) {
		atom_id = son->id;

		if (static_field_kind & UNBOUNDED_KIND) {
		  if (atom_id == ERROR_STE) {
		    /* atom_id est une valeur unbounded donc non encore definie */
		       /* ... on la definit */
		    atom_id = son->id = sxword_2save (&atom_names,
						      sxstrget (son->token.string_table_entry),
						      sxstrlen (son->token.string_table_entry));
		  }
		  else {
		    if (atom_id == atom_Aij || atom_id == atom_Pij) {
		      sxput_error (son->token.source_index,
				   "%sCannot be used in this context: ignored.",
				   sxtab_ptr->err_titles [2]);
		      atom_id = ERROR_STE;
		    }
		  }
		}
		else {
		  if (atom_id != ERROR_STE && !SXBA_bit_is_set (current_atom_set, atom_id)) {
		    sxput_error (son->token.source_index,
				 "%sUndeclared atom: ignored.",
				 sxtab_ptr->err_titles [2]);
		    atom_id = ERROR_STE;
		  }
		}

		XH_push (atom_hd, atom_id);
	      }
	    }
	    else {
	      /* vide */
	      /* Les atomes vides, partages ont des struct_id non vides */
	      /* Le 04/05/05 On ne marque comme partagee que les atomes vides =>
		 si on a [... = ()1 .... = (a)1 ... = a ...], c'est equivalent a [... = a .... = a ... = a ...] */
	      if (name_id)
		XH_push (atom_hd, X80+X20+name_id);
	    }
	    
	    XH_set (&atom_hd, &struct_id);
	  }
	  else {
	    /* brother->name == EMPTY_ATOM_n */
	    struct_id = 0;
	  }
	}
#endif /* ESSAI */
	else {
	  /* (static_field_kind & STRUCT_KIND) != 0 */
	  /* structure */
	  if (static_field_kind == (UNBOUNDED_KIND+STRUCT_KIND)) {
	    int     top2, bot2, flag;
	    BOOLEAN is_closed;

	    if (brother->name == NAMED_ADJUNCT_n || brother->name == EMPTY_NAMED_ADJUNCT_n) {
	      if (brother->name == NAMED_ADJUNCT_n) {
		brother = brother->son; /* brother->name == STRUCTURE_S_n */
		str = sxstrget (brother->brother->token.string_table_entry);
	      }
	      else {
		/* brother->name == EMPTY_NAMED_ADJUNCT_n */
		str = sxstrget (brother->token.string_table_entry);
	      }

	      name_id = atoi (str);
	      alloc_struct_index_set (name_id);

	      /* On continue a mettre l'index ds kind !! */
	      /* il est aussi en fin de structure (en negatif) */
	      field_shape [field_id] |= name_id << STRUCTURE_NAME_shift;
	    }
	    else {
	      /* brother->name == STRUCTURE_S_n || brother->name == EMPTY_ADJUNCT_n */
	      name_id = 0;
	    }

	    is_closed = TRUE;

	    if (brother->name == STRUCTURE_S_n) {
	      /* On range les id ds adjunct_hd */
	      for (son = brother->son; son != NULL; son = son->brother) {
		XH_push (adjunct_hd, son->id);

		top2 = XH_X (structure_hd, son->id+1);
		bot2 = XH_X (structure_hd, son->id);

		if (bot2 < top2 && (XH_list_elem (structure_hd, top2-1) & (X20+X10))) {
		  /* partage' ou non-clos */
		  is_closed = FALSE;
		}
	      }
	    }

	    if (brother->name == EMPTY_ADJUNCT_n)
	      struct_id = 0;
	    else {
	      if (name_id || !is_closed) {
		flag = (name_id) ? X80+X20+name_id : 0;
		flag += (is_closed) ? 0 : X10;
		XH_push (adjunct_hd, flag);
	      }

	      XH_set (&adjunct_hd, &struct_id);
	    }
	  }
	  else {
	    /* static_field_kind == STRUCT_KIND */
	    if (brother->name == NAMED_STRUCTURE_n || brother->name == EMPTY_NAMED_STRUCT_n) {
	      if (brother->name == NAMED_STRUCTURE_n) {
		brother = brother->son; /* brother->name == ATTR_VAL_S_n */
		str = sxstrget (brother->brother->token.string_table_entry);

		if (struct_id == ERROR_STE)
		  break;
	      }
	      else {
		/* brother->name == EMPTY_NAMED_STRUCT_n */
		str = sxstrget (brother->token.string_table_entry);
	      }

	      struct_id = brother->id;
	      name_id = atoi (str);
	      alloc_struct_index_set (name_id);

	      /* On continue a mettre l'index ds kind !! */
	      /* il est aussi en fin de structure (en negatif) */
	      field_shape [field_id] |= name_id << STRUCTURE_NAME_shift;
	    }
	    else {
	      /* brother->name == EMPTY_STRUCT_n || brother->name == ATTR_VAL_S_n */
	      if (brother->name == ATTR_VAL_S_n) {
		struct_id = brother->id;

		if (struct_id == ERROR_STE)
		  break;
	      }
	      else
		/* brother->name == EMPTY_STRUCT_n */
		struct_id = 0; /* vide */
	    }
	  }
	}

	/* Ajoute' le 22/04/05 */
	field_node [field_id] = brother;
	/* ATOM_S_n || EMPTY_ATOM_n || EMPTY_NAMED_ATOM_n*/
	/* STRUCTURE_S_n || EMPTY_ADJUNCT_n || EMPTY_NAMED_ADJUNCT_n */
	/* ATTR_VAL_S_n || EMPTY_STRUCT_n || EMPTY_NAMED_STRUCT_n */

	if (SXBA_bit_is_reset_set (used_field_set, field_id)) {
	  field_value [field_id] = struct_id;
	}
	else {
	  sxput_error (VISITED->token.source_index,
		       "%sMultiple attribute name definition: ignored.",
		       sxtab_ptr->err_titles [2]);
	}
      }
      else {
	sxput_error (brother->token.source_index,
		     "%sValue type mismatch: ignored.",
		     sxtab_ptr->err_titles [2]);
      }
    }
    break;

  case ATTR_VAL_S_n :
    /*
      <attr_val_LIST> = <attr_val_LIST> "," <attr_val> ;
      <attr_val_LIST> = <attr_val> ; "ATTR_VAL_S"
    */
    /* On vient de reconnaitre une structure */
    VISITED->id = fill_structure_hd (VISITED, &cur_lexeme, &is_main_structure);

#if EBUG
    if (is_main_structure)
      print_structure (VISITED, VISITED->id & X3F, cur_lexeme);
#endif /* EBUG */

    /* On positionne les variables du niveau precedent */
    decr_level ();
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

  case DECLARATION_S_n :
    break;

  case DISJONCTION_S_n :
    if (VISITED->father->name == SOUS_CAT_n || VISITED->father->name == PRED_FIELD_n) {
      /* Declaration d'un pred */
      /* VISITED->son->{brother->}name = {ATOM_S_n, OPT_DISJONCTION_n} */
      if (VISITED->degree > 1) {
	if (working_compound_field_set == NULL) {
	  working_compound_field_set = sxba_calloc (max_field_id+XH_size (compound_fields_hd)+1);
	  working2_compound_field_set = sxba_calloc (max_field_id+XH_size (compound_fields_hd)+1);
	}
	
	for (brother = VISITED->son; brother != NULL; brother = brother->brother) {
	  SXBA_1_bit (working_compound_field_set, brother->id);

	  if (brother->name == OPT_DISJONCTION_n)
	    SXBA_1_bit (working2_compound_field_set, brother->id);
	}

	compound_field_id = 0;

	while ((compound_field_id = sxba_scan_reset (working_compound_field_set, compound_field_id)) >= 0) {
	  if (SXBA_bit_is_set_reset (working2_compound_field_set, compound_field_id))
	    XH_push (sous_cat_hd, -compound_field_id);
	  else
	    XH_push (sous_cat_hd, compound_field_id);
	}
      }
      else {
	if (VISITED->son->name == ATOM_S_n)
	  XH_push (sous_cat_hd, VISITED->son->id);
	else
	  /* Les listes optionnelles sont notees en negatif */
	  XH_push (sous_cat_hd, -VISITED->son->id);
      }
	
      XH_set (&sous_cat_hd, &(VISITED->id));
    }
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
    has_named_structure_inside_macro = TRUE; /* au cas ou ... */

    if (VISITED->father->father->name == RULES_S_n) {
      /* top level */
      sxput_error (VISITED->token.source_index,
		   "%sIndexed structures are illegal at top level: ignored.",
		   sxtab_ptr->err_titles [2]);
      VISITED->id = 0;
    }
    else {
      name = atoi (sxstrget (VISITED->token.string_table_entry)); /* > 0 */
      XH_push (structure_hd, 0 /* field_id */);
      XH_push (structure_hd, 0 /* field_kind */);
      /* Le 16/03/05 */
      XH_push (structure_hd, X80+X20+name/* struct_id */); /* no de la structure partagee */
      XH_set (&structure_hd, &(VISITED->id));
    }

    break;

  case EMPTY_SOUS_CAT_n :
    break;

  case EMPTY_STRUCT_n :
    /* <structure> = "[" "]" ; "EMPTY_STRUCT" */
    VISITED->id = empty_struct_id;
    break;

  case ENTIER_n :
    break;

  case FIELD_n :
    break;

  case FIELD_CONSTRAINT_n :
    break;

  case FIELD_EQUAL_n :
    break;

  case FIELD_OPTIONAL_n :
    break;

  case IDENT_n :
    break;

  case IDENT_S_n :
    break;

  case LEXICAL_GRAMMAR_n :
    break;

  case MACRO_n :
    if (VISITED->father->name == ATTR_VAL_S_n) {
      /* <attr_val> = "@" %IDENT ; "MACRO" */
      /* Utilisation d'une macro de structure, on l'etend, ca va permettre des verifs + faciles
	 sur les structures. */
      macro_id = sxword_2retrieve (&macro_names,
				   sxstrget (VISITED->token.string_table_entry),
				   sxstrlen (VISITED->token.string_table_entry));

      if (macro_id == ERROR_STE) {
	/* Macro inconnue */
	sxput_error (VISITED->token.source_index,
		     "%sUnknown macro name: ignored.",
		     sxtab_ptr->err_titles [2]);
      }
      else {
	int bot, top, val, field_kind, sous_cat2;

	field_id = macro_id2structure [macro_id];

#if EBUG
	if (field_id == ERROR_STE)
	  sxtrap (ME, "lfg_lex_pd (MACRO)");
#endif /* EBUG */

	for (bot = XH_X (structure_hd, field_id), top = XH_X (structure_hd, field_id+1);
	     bot < top;
	     bot++) {
	  field_id = XH_list_elem (structure_hd, bot);
	  field_kind = XH_list_elem (structure_hd, ++bot);
	  val = XH_list_elem (structure_hd, ++bot);

	  if (val < 0)
	    break;

	  if (field_id == pred_id) {
	    sous_cat2 = XH_list_elem (structure_hd, ++bot);

	    if (pred_value_ptr->graphie) {
	      /* Plusieurs pred au meme niveau */
	      sxput_error (VISITED->token.source_index,
			   "%sMacro expansion results in multiple pred definition: ignored.",
			   sxtab_ptr->err_titles [2]);
	    }
	    else {
	      pred_value_ptr->graphie = field_kind;
	      pred_value_ptr->sous_cat1 = val;
	      pred_value_ptr->sous_cat2 = sous_cat2;
	    }
	  }
	  else {
	    if (SXBA_bit_is_reset_set (used_field_set, field_id)) {
	      field_shape [field_id] = field_kind;
	      field_value [field_id] = val;
	    }
	    else {
	      sxput_error (VISITED->token.source_index,
			   "%sMacro expansion results in multiple \"%s\" definition: ignored.",
			   sxtab_ptr->err_titles [2],
			   SXWORD_get (field_names, field_id));
	    }
	  }
	}
      }
    }
    break;

  case MACRO_NAME_n :
    break;

  case MACRO_STRUCTURE_n :
    /* <declaration>		= "@" %IDENT "=" <structure> ";" ; "MACRO_STRUCTURE"" */
    son = VISITED->son;

    if ((macro_id = son->id)) {
      son = son->brother;
      /* son->name = {ATTR_VAL_S_n, EMPTY_NAMED_STRUCT_n, EMPTY_STRUCT_n, NAMED_STRUCTURE_n} */

      if (son->id) {
	macro_id2structure [macro_id] = son->id;

	macro_id2visited [macro_id] = (has_named_structure_inside_macro) ? son : NULL;
      }
    }
    break;

  case MOT_1COL_n :
    break;

  case NAMED_ADJUNCT_n :
    break;

  case NAMED_ATOM_n :
    break;

  case NAMED_STRUCTURE_n :
    has_named_structure_inside_macro = TRUE; /* au cas ou ... */
    break;

  case OPT_DISJONCTION_n :
    /* <disjonction> = "(" <disjonction> ")" ; "OPT_DISJONCTION" */
    VISITED->id = VISITED->son->id;
    break;

  case OPT_FIELD_n :
    break;

  case PAIR_n :
    break;

  case PAIR_S_n :
    break;

  case PRED_n :
    break;

  case PRED_FIELD_n :
    /*
      <attr_val> = pred <field_oper> "\"" <graphie> <sous_cat> "\"" ; "PRED_FIELD"
    */
    if (pred_value_ptr->graphie) {
      /* Plusieurs pred au meme niveau */
      sxput_error (VISITED->token.source_index,
		   "%sMultiple pred attribute: ignored.",
		   sxtab_ptr->err_titles [2]);
    }
    else {
      field_shape [pred_id] = 0;

      brother = VISITED->son;
      field_shape [pred_id] = (brother->name == FIELD_EQUAL_n) ? 0 : (brother->name == FIELD_CONSTRAINT_n ? ASSIGN_CONSTRAINT_KIND : ASSIGN_OPTION_KIND /* FIELD_OPTIONAL_n */);

      brother = brother->brother;
      pred_value_ptr->graphie = brother->id /* lexeme_names_id */;
      brother = brother->brother;
      /* brother->name = {DISJONCTION_S_n, EMPTY_SOUS_CAT_n, SOUS_CAT_n} */
      if (brother->name == EMPTY_SOUS_CAT_n) {
	pred_value_ptr->sous_cat1 = pred_value_ptr->sous_cat2 = 0;
      }
      else {
	if (brother->name == DISJONCTION_S_n) {
	  pred_value_ptr->sous_cat1 = brother->id;
	  pred_value_ptr->sous_cat2 = 0;
	}
	else {
	  /* brother->name = SOUS_CAT_n */
	  brother = brother->son;
	  /* brother->name = DISJONCTION_S_n */
	  pred_value_ptr->sous_cat1 = brother->id;

	  brother = brother->brother;
	  /* brother->name = DISJONCTION_S_n */
	  pred_value_ptr->sous_cat2 = brother->id;
	}
      }
    }
    break;

  case PRIORITY_n :
    break;

  case PRIORITY_MAX_n :
    break;

  case PRIORITY_S_n :
    if (VISITED->degree) {
      int priority;

      for (brother = VISITED->son; brother != NULL; brother = brother->brother) {
	/* brother->name = {PRIORITY_n, PRIORITY_MAX_n} */
	priority = brother->name == PRIORITY_MAX_n
	  ? MAX_PRIORITY
	  : atoi (sxstrget (brother->token.string_table_entry));
	XH_push (XH_priority_hd, priority);
      }

      XH_set (&XH_priority_hd, &VISITED->id);
    }
    else
      VISITED->id = 0;

    break;

  case RULE_n :
    /* <rule> = %MOT_1COL <priority_LIST> <IDENT> <[main_structure]> ; "RULE" */
    if (sxson (VISITED, VISITED->degree)->name == ATTR_VAL_S_n)
      VISITED->id = cur_lexeme;

    break;

  case RULES_S_n : /* VISITED->son{brother->}*name = {RULE_n} */
    {
      int  ste, lfg_lex_pd_t_id, t, size, priority, top, new_top, if_id, lr_id;
    
      size = VISITED->degree;

      sxword_alloc (&inflected_form_names, "inflected_form_names", size+1, 1, 10,
		    sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, inflected_form_names_oflw, NULL);
      if_id2lr_id = (int*) sxalloc (SXWORD_size (inflected_form_names)+1, sizeof (int));
      XH_alloc (&lexical_rule_stack_hd, "lexical_rule_stack_hd", size+1, 1, 4, lexical_rule_stack_oflw, NULL);
      lr_id2prev = (int*) sxalloc (XH_size (lexical_rule_stack_hd)+1, sizeof (int));
      lr_id2priority = (int*) sxalloc (XH_size (lexical_rule_stack_hd)+1, sizeof (int));

      for (brother = VISITED->son; brother != NULL; brother = brother->brother) {
	/* traitement des formes flechies */
	son = brother->son; /* son->name reference la forme flechie */
	/* son->name = {MOT_1COL_n} */
	ste = son->token.string_table_entry;
	son = son->brother;
	/* son->name = {PRIORITY_S_n} */

	/* Traitement de la priorite */
	priority = son->id; /* id ds XH_priority_hd */
#if 0
	if (brother->name == RULE_PRIO_n) {
	  priority = son->name == PRIORITY_MAX_n
	    ? MAX_PRIORITY
	    : atoi (sxstrget (son->token.string_table_entry));
	  son = son->brother;
	}
	else
	  priority = 0;
#endif /* 0 */

	son = son->brother;
	/* son->name = {IDENT_n, PRED_n} */

	/* Traitement du terminal */
	/* Pour ne pas planter si pas de rubrique TERMINAL... */
	if (t_id2t) {
	  /* allouee */
	  if (son->name == IDENT_n)
	    lfg_lex_pd_t_id = sxword_2retrieve (&t_names,
				     sxstrget (son->token.string_table_entry),
				     sxstrlen (son->token.string_table_entry));
	  else
	    lfg_lex_pd_t_id = sxword_2retrieve (&t_names, "pred", 4);
	}
	else
	  lfg_lex_pd_t_id = ERROR_STE;

	if (lfg_lex_pd_t_id == ERROR_STE)
	  sxput_error (son->token.source_index,
		       "%sUnknown terminal symbol: ignored.",
		       sxtab_ptr->err_titles [2]);
	else {
	  SXBA_1_bit (t_id_set, lfg_lex_pd_t_id);

	  son = son->brother; /* son->name == ATTR_VAL_S_n || NAMED_STRUCTURE_n || EMPTY_STRUCT_n */
	  /* traitement de la structure */
	  
	  if (son->name == NAMED_STRUCTURE_n)
	    son = son->son;

	  /* son->name == ATTR_VAL_S_n || EMPTY_STRUCT_n */
	  /* brother->id est le lexeme de la structure et son->id est l'identifiant ds structure_hd de la structure */

	  if (son->id || son->name == EMPTY_STRUCT_n) {
	    /* Pas d'erreur ds la structure */
	    if (son->name == EMPTY_STRUCT_n) 
	      son->id = ERROR_STE;

	    top = SXWORD_top (inflected_form_names);
	    if_id = sxword_2save (&inflected_form_names,
				  sxstrget (ste),
				  sxstrlen (ste));
	    new_top = SXWORD_top (inflected_form_names);

	    if (new_top > top) {
	      /* Nouvelle forme flechie */
	      if_id2lr_id [if_id] = 0;
	    }

	    /* Il faut verifier qu'il n'y a pas duplication de la structure */
	    t = t_id2t [lfg_lex_pd_t_id];
	    
	    XH_push (lexical_rule_stack_hd, if_id);
	    XH_push (lexical_rule_stack_hd, t);
	    XH_push (lexical_rule_stack_hd, brother->id);
	    XH_push (lexical_rule_stack_hd, son->id);

	    if (XH_set (&lexical_rule_stack_hd, &lr_id)) {
	      sxput_error (brother->token.source_index,
			   "%sDuplicate lexical rule definition: ignored.",
			   sxtab_ptr->err_titles [2]);
	      son = NULL;
	    }
	    else {
	      lr_id2prev [lr_id] = if_id2lr_id [if_id];
	      lr_id2priority [lr_id] = priority; /* id ds XH_priority_hd */
	      if_id2lr_id [if_id] = lr_id;
	    }
	  }
	}
      }

      if (AMALGAM_VISITED || COMPOUND_VISITED) {
	int lfg_lex_pd_size = 0;

	if (AMALGAM_VISITED)
	  lfg_lex_pd_size += AMALGAM_VISITED->degree;

	if (COMPOUND_VISITED)
	  lfg_lex_pd_size += 4*COMPOUND_VISITED->degree;
	
	sxword_alloc (&amalgam_or_compound_component_names, "amalgam_or_compound_component_names", lfg_lex_pd_size+1, 1, 16,
		    sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

	amalgam_id_top = SXWORD_top (amalgam_or_compound_component_names);
      }

      if (AMALGAM_VISITED) {
	/* Y'a des amalgames */
	/* AMALGAM_VISITED->name = AMALGAM_S_n */
	/* Ex : au � le */
	int lfg_lex_pd_top;
	XH_header amalgam_stack_hd;

	amalgam_id_set = sxba_calloc (AMALGAM_VISITED->degree+3);
	XH_alloc (&amalgam_stack_hd, "amalgam_stack_hd", AMALGAM_VISITED->degree+1, 1, 2, NULL, NULL);
	amalgam2visited = (NODE**) sxalloc (AMALGAM_VISITED->degree+1, sizeof (NODE*));
	amalgam_size = 1;

	for (brother = AMALGAM_VISITED->son; brother != NULL; brother = brother->brother) {
	  /* brother->name == IDENT_S_n */
#if 0
	  for (son = brother->son->brother; son != NULL; son = son->brother) {
	    ste = son->token.string_table_entry;

	    if (sxword_2retrieve (&inflected_form_names,
				  sxstrget (ste),
				  sxstrlen (ste)) == ERROR_STE) {
	      sxput_error (son->token.source_index,
			   "%sThis inflected form must be defined by a lexical rule: ignored.",
			   sxtab_ptr->err_titles [1]);
	      break;
	    }
	  }

	  if (son == NULL) {
	    ste = brother->son->token.string_table_entry; /* ste de "au" */
	    /* On permet q'un amalgame soit aussi une forme flechie normale et q'un amalgame ait
	       plusieurs expansions (differentes) */
	    XH_push (amalgam_stack_hd, ste);

	    for (son = brother->son->brother; son != NULL; son = son->brother) {
	      XH_push (amalgam_stack_hd, son->token.string_table_entry);
	    }

	    if (XH_set (&amalgam_stack_hd, &lfg_lex_pd_top)) {
	      sxput_error (brother->token.source_index,
			   "%sDuplicate amalgam definition: ignored.",
			   sxtab_ptr->err_titles [1]);
	    }
	    else {
	      /* OK */
	      brother->son->id = sxword_2save (&amalgam_or_compound_names,
					       sxstrget (ste),
					       sxstrlen (ste)); /* au */

	      amalgam2visited [amalgam_size] = brother->son; /* Pour + tard, pointe sur le "au" de "au � le" */
	      amalgam_size++;
	    }
	  }
#endif /* 0 */
	  ste = brother->son->token.string_table_entry; /* ste de "au" */
	  /* On permet q'un amalgame soit aussi une forme flechie normale et q'un amalgame ait
	     plusieurs expansions (differentes) */
	  XH_push (amalgam_stack_hd, ste);

	  for (son = brother->son->brother; son != NULL; son = son->brother) {
	    XH_push (amalgam_stack_hd, son->token.string_table_entry);
	  }

	  if (XH_set (&amalgam_stack_hd, &lfg_lex_pd_top)) {
	    sxput_error (brother->token.source_index,
			 "%sDuplicate amalgam definition: ignored.",
			 sxtab_ptr->err_titles [1]);
	  }
	  else {
	    /* OK */
	    brother->son->id = sxword_2save (&amalgam_or_compound_component_names, sxstrget (ste), sxstrlen (ste)); /* au */
	    SXBA_1_bit (amalgam_id_set, brother->son->id);
	    amalgam2visited [amalgam_size] = brother->son; /* Pour + tard, pointe sur le "au" de "au � le" */
	    amalgam_size++;
	  }
	}

	XH_free (&amalgam_stack_hd);

	amalgam_id_top = SXWORD_top (amalgam_or_compound_component_names);
      }

      if (COMPOUND_VISITED) {
	/* traitement des mots composes */
	/* Y'a des amalgames */
	/* COMPOUND_VISITED->name = COMPOUND_S_n */
	/* Ex : "pomme-de-terre pomme de terre" */
	int       lfg_lex_pd_top, lfg_lex_pd_size;
	XH_header compound_stack_hd;

	lfg_lex_pd_size = 0;

	for (brother = COMPOUND_VISITED->son; brother != NULL; brother = brother->brother)
	  lfg_lex_pd_size += brother->degree-1;
	
	compound_component_id_set = sxba_calloc (amalgam_id_top+lfg_lex_pd_size+1);
	XH_alloc (&compound_stack_hd, "compound_stack_hd", COMPOUND_VISITED->degree+1, 1, 4, NULL, NULL);
	compound2visited = (NODE**) sxalloc (COMPOUND_VISITED->degree+1, sizeof (NODE*));
	compound_size = 1;

	for (brother = COMPOUND_VISITED->son; brother != NULL; brother = brother->brother) {
	  /* brother->name == IDENT_S_n */
	  son = brother->son;
	  /* son pointe vers "pomme-de-terre" */
	  ste = son->token.string_table_entry;

	  if (sxword_2retrieve (&inflected_form_names, sxstrget (ste), sxstrlen (ste)) == ERROR_STE &&
	      sxword_2retrieve (&amalgam_or_compound_component_names, sxstrget (ste), sxstrlen (ste)) == ERROR_STE) {
	    /* Ca peut etre aussi un element d'un compound ?? */
	    sxput_error (son->token.source_index,
			 "%sThis compound word must be an inflected form or an amalgam name: ignored.",
			 sxtab_ptr->err_titles [1]);
	    son->id = ERROR_STE;
	  }
	  else {
	    son->id = ste; /* le ste de pomme_de_terre ds "sxstrget (ste)" */

	    XH_push (compound_stack_hd, son->id);

	    while ((son = son->brother) != NULL) {
	      ste = son->token.string_table_entry;
	      son->id = sxword_2save (&amalgam_or_compound_component_names, sxstrget (ste), sxstrlen (ste));
	      XH_push (compound_stack_hd, son->id);
	      SXBA_1_bit (compound_component_id_set, son->id);
	    }

	    if (XH_set (&compound_stack_hd, &lfg_lex_pd_top)) {
	      sxput_error (brother->token.source_index,
			   "%sDuplicate compound word definition: ignored.",
			   sxtab_ptr->err_titles [1]);
	      brother->son->id = ERROR_STE;
	    }
	    else {
	      /* OK */
#if 0
	      for (son = brother->son->brother; son != NULL; son = son->brother) {
		ste = son->token.string_table_entry;
		son->id = sxword_2save (&amalgam_or_compound_names,
					sxstrget (ste),
					sxstrlen (ste));
	      }
#endif /* 0 */
	      compound2visited [compound_size] = brother->son; /* Pour + tard, pointe sur le "pomme-de-terre" de
								  "pomme-de-terre pomme de terre" */
	      compound_size++;
	    }
	  }
	}

	XH_free (&compound_stack_hd);
      }
    }
    break;

  case SOUS_CAT_n :
    break;

  case STRUCTURE_S_n :
    /*
      <structure_LIST> = <structure_LIST> "," <structure> ;
      <structure_LIST> = <structure> ; "STRUCTURE_S"	
    */	
    /* On vient de reconnaitre un ADJUNCT (ensemble de structure) */
    break;

  case TERMINAL_n :
    break;

  case VOID_n :
    break;



    /*
      Z Z Z Z
    */

  default:
    break;
  }
  /* end lfg_lex_pd */
}


static void
check_referenced_field_set ()
{
  /* On sort la liste des champs (declares) mais jamais utilises */
  int     field_id, nb;
  SXNODE  *node;

  SXBA_1_bit (referenced_field_set, pred_id); /* On le met a priori... */

  /* On ajoute tous les champs references ds la partie syntaxique, ils se trouvent sous CATEGORY */

  for (node = sxatcvar.atc_lv.abstract_tree_root->son->son; node != NULL; node = node->brother) {
    if (node->name == CATEGORY_n)
      /* Il peut y avoir plusieurs noeuds CATEGORY_n, seul le + a gauche est conside're'.  Les autres sous-arbres
	 sont ignore's */
      break;
  }

  if (node) {
    for (node = node->son->son; node != NULL; node = node->brother) {
      if (node->name == IDENT_n)
	field_id = sxword_2retrieve (&field_names,
				     sxstrget (node->token.string_table_entry),
				     sxstrlen (node->token.string_table_entry));
      else
	field_id = pred_id;

      if (field_id == ERROR_STE) {
	sxput_error (node->token.source_index,
		     "%sUnknown field name: ignored.",
		     sxtab_ptr->err_titles [2]);
      }
      else {
	SXBA_1_bit (referenced_field_set, field_id);
      }
    }
  }

  if (sxba_cardinal (referenced_field_set) == max_field_id-1)
    return;

  varstr_raz (vstr);
  field_id = sxba_0_lrscan (referenced_field_set, 1 /* EMPTY_STE */);

  if (field_id>=0) {
    nb = 0;

    do {
      nb++;
      vstr = varstr_catenate (vstr, SXWORD_get (field_names, field_id));
      field_id = sxba_0_lrscan (referenced_field_set, field_id);
      
      if (field_id > 0)
	vstr = varstr_catenate (vstr, ", ");
      
    } while (field_id >= 0);
    
    sxtmsg (sxatcvar.atc_lv.abstract_tree_root->token.source_index.file_name, 
	    "%sThe following field name%s never used:\n\
\"%s\".",
	    sxtab_ptr->err_titles [1],
	    nb == 1 ? " is" : "s are",
	    varstr_tostr (vstr));
  }
}

#include "sxdico.h"

static void
gen_header ()
{
  long	        date_time;
    
  date_time = time (0);

  printf (
	  "\n\
/* ********************************************************************\n\
\tThis LFG lexicon for \"%s\" has been generated\n\
\ton %s\
\tby the SYNTAX(*) LFG_LEX processor\n\
   ********************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
   ******************************************************************** */\n\n\n",
	  sxsrcmngr.source_coord.file_name,
	  ctime (&date_time));

  /* On sort la date actuelle (et non la date de derniere modif du .lex) car la
     sortie depend aussi de fichiers produits par l'execution de lfg */
  printf ("#define LFG_LEX_TIME %ld\n", date_time);

  printf ("#define USE_A_DICO %i\n", use_a_dico ? 1 : 0);

  if (use_a_dico)
    fputs ("\n#include \"sxdico.h\"\n",stdout);
  else
    fputs ("\n#include \"sxword.h\"\n",stdout);

  fputs ("\n#include \"lex_lfg.h\"\n",stdout);
}


static void smpopen ()
{
  sxatcvar.atc_lv.node_size = sizeof (struct lfg_lex_node);
}

static void
smppass ()
{

  /*   I N I T I A L I S A T I O N S   */
  /* ........... */
  ste_Aij = sxstrretrieve ("Aij");
  ste_Pij = sxstrretrieve ("Pij");

  /* Pour avoir une idee des tailles!! */
 {
     SXNODE  *node;

     for (node = sxatcvar.atc_lv.abstract_tree_root->son->son; node != NULL; node = node->brother) {
       if (node->name == ATTRIBUTE_n)
	 break;
     }

     if (node == NULL || node->son->name != ATTRIBUTE_S_n)
       FIELDS_nb = 2; /* pred et EMPTY_STE */
     else
       FIELDS_nb = node->son->degree+2 /* pred et EMPTY_STE */;
 }

  MACROS_nb = sxatcvar.atc_lv.abstract_tree_root->son->degree;
  RULES_nb = sxatcvar.atc_lv.abstract_tree_root->son->brother->degree;

  referenced_field_set = sxba_calloc (FIELDS_nb+1);
  sxword_alloc (&field_names, "field_names", FIELDS_nb+1, 1, 16,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
  field_id2kind = (unsigned char *) sxalloc (FIELDS_nb+1, sizeof (unsigned char));
  field_id2pass_nb = (int *) sxalloc (FIELDS_nb+1, sizeof (int));
  field_id2pass_nb [0] = field_id2pass_nb [1] = 0; /* inutilise' : ERROR_STE et EMPTY_STE */
  argument_set = sxba_calloc (FIELDS_nb+1);
  XH_alloc (&compound_fields_hd, "compound_fields_hd", 50, 1, 3, compound_fields_oflw, NULL);
  XH_alloc (&sous_cat_hd, "sous_cat_hd", 50, 1, 5, NULL, NULL);
  pred_id = sxword_2save (&field_names, "pred", 4); /* Initialisation a priori */
  field_id2kind [pred_id] = ATOM_KIND;
  field_id2pass_nb [pred_id] = 1; /* Pourquoi pas ? */

  XH_alloc (&XH_priority_hd, "XH_priority_hd", 50, 1, 2, NULL, NULL);

  sxword_alloc (&atom_names, "atom_names", 100, 1, 16,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, atom_names_oflw, NULL);
  field_id2atom_set = sxbm_calloc (FIELDS_nb+1, SXWORD_size (atom_names)+1);
  vstr = varstr_alloc (128);

  sxword_alloc (&macro_names, "macro_names", MACROS_nb+1, 1, 8,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, macro_names_oflw, NULL);
  macro_id2structure = (unsigned int *) sxalloc (SXWORD_size (macro_names)+1, sizeof (unsigned int));
  macro_id2visited = (NODE **) sxalloc (SXWORD_size (macro_names)+1, sizeof (NODE *));

  sxword_alloc (&lexeme_names, "lexeme_names", RULES_nb+1, 1, 12,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

  XH_alloc (&structure_hd, "structure_hd", MACROS_nb+RULES_nb+1, 1, 3*6, NULL, NULL);
  XH_set (&structure_hd, &empty_struct_id);
  XH_alloc (&adjunct_hd, "adjunct_hd", 31+1, 1, 2, NULL, NULL);
  XH_alloc (&atom_hd, "atom_hd", 100+1, 1, 2, NULL, NULL);
  XH_set (&structure_hd, &empty_struct_id); /* ce sont les 2 memes ... */
  XH_alloc (&pred_val_hd, "pred_val_hd", MACROS_nb+RULES_nb+1, 1, 3, NULL, NULL);

  SS_path = SS_alloc (MACROS_nb+RULES_nb+1);
    
  named_structure2visited_size = 31;
  empty_named_structure_set = sxba_calloc (named_structure2visited_size+1);
  named_structure_set = sxba_calloc (named_structure2visited_size+1);
  named_structure2already_visited = sxba_calloc (named_structure2visited_size+1);
#if 0
  named_structure2sons = sxbm_calloc (named_structure2visited_size+1, named_structure2visited_size+1);
  named_structure2fathers = sxbm_calloc (named_structure2visited_size+1, named_structure2visited_size+1);
  named_structure2not_closed = sxba_calloc (named_structure2visited_size+1);
#endif /* 0 */
  named_structure2visited = (NODE**) sxalloc (named_structure2visited_size+1, sizeof (NODE*));
  named_structure2occur_nb = (int*) sxcalloc (named_structure2visited_size+1, sizeof (int));
  named_structure2id = (int*) sxcalloc (named_structure2visited_size+1, sizeof (int));

  /*   A T T R I B U T E S    E V A L U A T I O N   */
  sxsmp (sxatcvar.atc_lv.abstract_tree_root, lfg_lex_pi, lfg_lex_pd);

#if 0
  if (T) {
    sxfree (T [0]);
    sxfree (T), T = NULL, T_size = 0;
  }
#endif /* 0 */
    
  sxfree (empty_named_structure_set), empty_named_structure_set = NULL;
  sxfree (named_structure_set), named_structure_set = NULL;
  sxfree (named_structure2already_visited), named_structure2already_visited = NULL;
#if 0
  sxbm_free (named_structure2sons), named_structure2sons = NULL;
  sxbm_free (named_structure2fathers), named_structure2fathers = NULL;
  sxfree (named_structure2not_closed), named_structure2not_closed = NULL;
#endif /* 0 */
  sxfree (named_structure2visited), named_structure2visited = NULL;
  sxfree (named_structure2occur_nb), named_structure2occur_nb = NULL;
  sxfree (named_structure2id), named_structure2id = NULL;

  if (glbl_id2sons) {
    sxbm_free (glbl_id2sons), glbl_id2sons = NULL;
    sxbm_free (glbl_id2fathers), glbl_id2fathers = NULL;
    sxfree (glbl_id2not_closed), glbl_id2not_closed = NULL;
  }

  check_referenced_field_set ();

  sxfree (referenced_field_set), referenced_field_set = NULL;

  if (t_id2t == NULL) {
    sxtmsg (sxatcvar.atc_lv.abstract_tree_root->token.source_index.file_name, 
	    "%sThe \"TERMINAL\" chapter is missing.\n",
	    sxtab_ptr->err_titles [2]);
  }
  else {
    if (sxba_cardinal (t_id_set) != tmax && !make_proper_will_be_used) {
      /* Tous les terminaux ne sont pas couverts par des formes flechies, on emet un warning */
      int t_id, nb;

      varstr_raz (vstr);

      t_id = sxba_0_lrscan (t_id_set, EMPTY_STE);
      nb = 0;

      do {
	nb++;
	vstr = varstr_catenate (vstr, SXWORD_get (t_names, t_id));
	t_id = sxba_0_lrscan (t_id_set, t_id);

	if (t_id > 0)
	  vstr = varstr_catenate (vstr, ", ");
    
      } while (t_id >= 0);

      sxtmsg (sxatcvar.atc_lv.abstract_tree_root->token.source_index.file_name, 
	      "%sThe following terminal name%s not defined:\n\
\"%s\".",
	      sxtab_ptr->err_titles [1],
	      nb == 1 ? " is" : "s are",
	      varstr_tostr (vstr));
    }
  }

  inflected_form_nb = SXWORD_top (inflected_form_names)-1;
  lexical_rule_nb = XH_top (lexical_rule_stack_hd)-1;

  fprintf (sxtty, "There are %i static fields (%i+%i atom types).\n", SXWORD_top (field_names) <= 3 ? 0 : max_field_id-1,
	  max_atom_field_nb, max_undef_atom_field_nb);
  
  max_unbounded_atom_id = SXWORD_top (atom_names)-1;
#ifndef ESSAI
  fprintf (sxtty, "There are %i static atoms (max_local = %i).\n", SXWORD_top (atom_names) == 2 ? 0 : max_static_atom_id-1,
	  max_local_atom_id);
#else /* ESSAI */
  fprintf (sxtty, "There are %i static and %i unbounded atoms.\n", max_static_atom_id-2, max_unbounded_atom_id-max_static_atom_id);
#endif /* ESSAI */
  fprintf (sxtty, "There are %i structures.\n", XH_top (structure_hd)-1);
  fprintf (sxtty, "There are %i main structures.\n", main_structure_nb);
  fprintf (sxtty, "Largest lexical structure size: %i fields.\n", largest_lexical_structure_size);
  fprintf (sxtty, "There are %i inflected forms out of %i valid lexical definitions.\n", inflected_form_nb-1, lexical_rule_nb);

  /* ON GENERE ... */
  fprintf (sxtty, "Computing tables... (this may take a while)\n");
  if (!IS_ERROR) {
    static struct mot2	mot2;
    static struct dico	dico;
	
    gen_header ();

    if (amalgam_size > 1 || compound_size > 1) {
      /* Y'a des amalgames valides */
      int     id, ac_id, size, ste, x, nb, y;
      NODE    *visited, *brother;

      SXBA      if_id_set, initial_compound_component_id_set, final_compound_component_id_set;
      struct    ac_list {
	int hd, next;
      }         *amlgm_list, *cmpnd_list;

      int       max_cc_list_lgth, lgth;
      int       *ac_id2amlgm_list, *cc2cmpnd_list;
      XH_header XH_cc_list;
      int       *amlgm_component_stack, amalgam_component_nb;
      BOOLEAN   dico_OK = TRUE;

#define PUSH(s,x)	(s)[++*(s)]=(x)

      if_id_set = NULL;
      amlgm_list = NULL;
      amlgm_component_stack = NULL;
      cmpnd_list = NULL;
      ac_id2amlgm_list = NULL;
      cc2cmpnd_list = NULL;
      initial_compound_component_id_set = NULL;
      final_compound_component_id_set = NULL;
      XH_is_allocated (XH_cc_list) = FALSE;

      size = SXWORD_top (amalgam_or_compound_component_names);

      nb = 0;

      for (x = 1; x < amalgam_size; x++) {
	visited = amalgam2visited [x];

	nb +=  visited->father->degree-1;

	for (brother = visited->brother; brother != NULL; brother = brother->brother) {
	  ste = brother->token.string_table_entry;

	  if (sxword_2retrieve (&inflected_form_names, sxstrget (ste), sxstrlen (ste)) == ERROR_STE
	      && sxword_2retrieve (&amalgam_or_compound_component_names, sxstrget (ste), sxstrlen (ste)) == ERROR_STE) {
	    sxput_error (brother->token.source_index,
			 "%sThis amalgam component must be an inflected form (or a compound component) : ignored.",
			 sxtab_ptr->err_titles [1]);
	    ste = ERROR_STE;
	    SXBA_0_bit (amalgam_id_set, visited->id);
	  }

	  brother->id = ste;
	}
      }

      amalgam_component_nb = nb;

      nb += compound_size-1;

      /* Le dico va comprendre les noms des amalgames et les noms des composants des mots compose's */
      if (use_a_dico) {
	mot2.size = size+1;
	mot2.string = (char**) sxalloc (mot2.size, sizeof (char*));
	mot2.string [0] = (char*) NULL;
	mot2.code = (int*) sxalloc (mot2.size, sizeof (int));
	mot2.code [0] = 0;
	mot2.lgth = (int*) sxalloc (mot2.size, sizeof (int));
	mot2.lgth [0] = 0;
	mot2.optim_kind = SPACE; /* Priorite a l'espace sur le temps */
	mot2.process_kind = TOTAL; /* La chaine totale est prise en compte et pas seulement les prefixe et suffixe
				      discriminants minimaux de chaque chaine sont entres ds l'automate */
	mot2.print_on_sxtty = FALSE; /* on travaille ds le silence */
	mot2.min = (int*) sxalloc (mot2.size, sizeof (int));
	mot2.max = (int*) sxalloc (mot2.size, sizeof (int));

	id = 0;

	for (ac_id = 2; (unsigned)ac_id <SXWORD_top (amalgam_or_compound_component_names); ac_id++) {
	  id++;
	  mot2.string [id] = SXWORD_get (amalgam_or_compound_component_names, ac_id);
	  mot2.lgth [id] = SXWORD_len (amalgam_or_compound_component_names, ac_id);
	  mot2.code [id] = ac_id;
	}

	mot2.nb = id;

	sxdico_process (mot2, &dico);

	if (((((unsigned int) (~0))>>dico.base_shift) < dico.max)) {
	  sxtmsg (SXGET_TOKEN (0).source_index.file_name,
		  "%sToo many amalgam definitions, dictionary not produced\n",
		  sxtab_ptr->err_titles [2]);
	  dico_OK = FALSE;
	}
      }

      if (dico_OK) {
#if 0
	fputs ("\n#ifdef include_full_lexicon\n", stdout);
	fputs ("\n#ifdef ICO_COMPOUND\n", stdout);
#endif /* 0 */

	if (use_a_dico) {
	  out_ifdef ("dico_cmpnd");
	  sxdico2c (&dico, stdout, "dico_cmpnd", TRUE);
	  out_endif ("dico_cmpnd");
	}
	else {
	  out_ifdef ("amalgam_or_compound_component_names");
	  sxword_to_c (&amalgam_or_compound_component_names, stdout, "amalgam_or_compound_component_names", TRUE);
	  out_endif ("amalgam_or_compound_component_names");
	}

#if 0
	fputs ("#endif /* ICO_COMPOUND */\n", stdout);
	fputs ("\n#endif /* include_full_lexicon */\n", stdout);
#endif /* 0 */

#if 0
	fputs ("\n#ifdef COMPOUND\n", stdout);
#endif /* 0 */

#if 0 /* pour �tre encore plus d�sagr�able que Pierre -- h�h� */
	fputs ("#include \"XH.h\"\n\n", stdout);
#endif /* 0 */

	fputs ("#define HAS_CMPD_OR_AMLGM\n", stdout);

	/* ... maintenant, on met tout ds amalgam_or_compound_component_names */
	for (x = 1; x < amalgam_size; x++) {
	  visited = amalgam2visited [x];

	  for (brother = visited->brother; brother != NULL; brother = brother->brother) {
	    ste = brother->token.string_table_entry;
	    brother->id = sxword_2save (&amalgam_or_compound_component_names, sxstrget (ste), sxstrlen (ste));
	  }
	}

	nb = 0;

	for (x = 1; x < compound_size; x++) {
	  visited = compound2visited [x];
	  nb += visited->father->degree-1;
	  ste = visited->token.string_table_entry;
	  visited->id = sxword_2save (&amalgam_or_compound_component_names, sxstrget (ste), sxstrlen (ste));
	}

	size = SXWORD_top (amalgam_or_compound_component_names);

	ac_id2amlgm_list = (int*) sxcalloc (size, sizeof (int));
	amlgm_list = (struct ac_list*) sxcalloc (amalgam_size, sizeof (struct ac_list));
	amlgm_component_stack = (int*) sxalloc (amalgam_component_nb+amalgam_size+1, sizeof (int)), amlgm_component_stack [0] = 0;

	cmpnd_list = (struct ac_list*) sxcalloc (compound_size, sizeof (struct ac_list));
	XH_alloc (&XH_cc_list, "XH_cc_list", compound_size+1+1/* pour l'utilisation dynamique */,
		  1, (nb/compound_size)+1, NULL, NULL);
	cc2cmpnd_list = (int*) sxcalloc (XH_size (XH_cc_list)+1, sizeof (int)); /* ne deborde pas */

	/* On leur donne tous la meme taille */
	amalgam_id_set = sxba_resize (amalgam_id_set, size);
	compound_component_id_set = sxba_resize (compound_component_id_set, size);
	initial_compound_component_id_set = sxba_calloc (size);
	final_compound_component_id_set = sxba_calloc (size);
	if_id_set = sxba_calloc (size);

	for (x = 1; x < amalgam_size; x++) {
	  visited = amalgam2visited [x];

	  if ((y = ac_id2amlgm_list [visited->id]))
	    amlgm_list [x].next = y;

	  ac_id2amlgm_list [visited->id] = x;

	  amlgm_list [x].hd = amlgm_component_stack [0]+1;

	  for (brother = visited->brother; brother != NULL; brother = brother->brother) {
	    PUSH (amlgm_component_stack, brother->id);
	  }

	  PUSH (amlgm_component_stack, 0); /* fin */
	}

	max_cc_list_lgth = 0;

	for (x = 1; x < compound_size; x++) {
	  visited = compound2visited [x];
	  SXBA_1_bit (initial_compound_component_id_set, visited->brother->id);
	  lgth = 0;

	  for (brother = visited->brother; brother != NULL; brother = brother->brother) {
	    XH_push (XH_cc_list, brother->id);
	    lgth++;

	    if (brother->brother == NULL)
	      SXBA_1_bit (final_compound_component_id_set, brother->id);
	  }

	  if (XH_set (&XH_cc_list, &y)) {
	    /* la meme liste de composants correspond a plusieurs mots composes */
	    cmpnd_list [x].next = cc2cmpnd_list [y];
	  }

	  cc2cmpnd_list [y] = x;
	  cmpnd_list [x].hd = visited->id;

	  if (lgth > max_cc_list_lgth)
	    max_cc_list_lgth = lgth;
	}

	printf ("\n#define MAX_CC_LIST_LGTH %i\n", max_cc_list_lgth);

#if 0
	fputs ("\n#endif /* COMPOUND */\n", stdout);
#endif /* 0 */

#if 0
	fputs ("\n#ifdef include_full_lexicon\n", stdout);
	fputs ("\n#ifdef COMPOUND\n", stdout);
#endif /* 0 */

	out_ifdef ("ac_id2string");
	printf ("\nstatic char *ac_id2string [%i] = {\n\
/* 0 */ \"ERROR\",\n\
/* 1 */ \"\",\n", size);

	for (id = 2; id < size; id++) {
	  printf ("/* %i */ \"%s\",\n", id, SXWORD_get (amalgam_or_compound_component_names, id));

	  /* On en profite pour remplir if_id_set, l'ensemble des formes flechies */
	  if (sxword_2retrieve (&inflected_form_names,
				SXWORD_get (amalgam_or_compound_component_names, id),
				SXWORD_len (amalgam_or_compound_component_names, id)) != ERROR_STE)
	    SXBA_1_bit (if_id_set, id);
	}

	fputs ("};\n", stdout);
	out_endif ("ac_id2string");

	/* et on les sort */
	out_ifdef ("amalgam_id_set");
	sxba2c (amalgam_id_set, stdout, "amalgam_id_set", "", TRUE, vstr);
	out_endif ("amalgam_id_set");
	out_ifdef ("compound_component_id_set");
	sxba2c (compound_component_id_set, stdout, "compound_component_id_set", "", TRUE, vstr);
	out_endif ("compound_component_id_set");
	out_ifdef ("if_id_set");
	sxba2c (if_id_set, stdout, "if_id_set", "", TRUE, vstr);
	out_endif ("if_id_set");
	out_ifdef ("initial_compound_component_id_set");
	sxba2c (initial_compound_component_id_set, stdout, "initial_compound_component_id_set", "", TRUE, vstr);
	out_endif ("initial_compound_component_id_set");
	out_ifdef ("final_compound_component_id_set");
	sxba2c (final_compound_component_id_set, stdout, "final_compound_component_id_set", "", TRUE, vstr);
	out_endif ("final_compound_component_id_set");

	out_ifdef ("XH_cc_list");
	XH2c (&XH_cc_list, stdout, "XH_cc_list", TRUE /* static */);
	out_endif ("XH_cc_list");

#if 0 // parti dans lex_lfg.h
	fputs ("struct ac_list {\n\
int hd, next;\n\
} ;\n", stdout);
#endif /* 0 */

	out_ifdef ("amlgm_component_stack");
	printf ("static int amlgm_component_stack [%i] = {", amlgm_component_stack [0]+1);
 
	for (x = 0; x <= amlgm_component_stack [0]; x++) {
	  if (x % 10 == 0)
	    printf ("\n/* %i */ ", x);

	  printf ("%i, ", amlgm_component_stack [x]);
	}

	fputs ("};\n", stdout);
	out_endif ("amlgm_component_stack");

	out_ifdef ("amlgm_list");
	printf ("static struct ac_list amlgm_list [%i] = {\n", amalgam_size);
 
	for (x = 0; x < amalgam_size; x++) {
	  printf ("/* %i */ {%i, %i},\n", x, amlgm_list [x].hd, amlgm_list [x].next);
	}

	fputs ("};\n", stdout);
	out_endif ("amlgm_list");

	out_ifdef ("ac_id2amlgm_list");
	printf ("static int ac_id2amlgm_list [%i] = {", size);
 
	for (id = 0; id < size; id++) {
	  if (id % 10 == 0)
	    printf ("\n/* %i */ ", id);

	  printf ("%i, ", ac_id2amlgm_list [id]);
	}

	fputs ("\n};\n", stdout);
	out_endif ("ac_id2amlgm_list");

	out_ifdef ("cmpnd_list");
	printf ("static struct ac_list cmpnd_list [%i] = {\n", compound_size);
 
	for (x = 0; x < compound_size; x++) {
	  printf ("/* %i */ {%i, %i},\n", x, cmpnd_list [x].hd, cmpnd_list [x].next);
	}

	fputs ("};\n", stdout);
	out_endif ("cmpnd_list");

	out_ifdef ("cc2cmpnd_list");
	printf ("static int cc2cmpnd_list [%i] = {", XH_top (XH_cc_list));
 
	for (id = 0; id <  XH_top (XH_cc_list); id++) {
	  if (id % 10 == 0)
	    printf ("\n/* %i */ ", id);

	  printf ("%i, ", cc2cmpnd_list [id]);
	}

	fputs ("\n};\n", stdout);
	out_endif ("cc2cmpnd_list");

	if (if_id_set) sxfree (if_id_set);
	if (amlgm_list) sxfree (amlgm_list);
	if (amlgm_component_stack) sxfree (amlgm_component_stack);
	if (cmpnd_list) sxfree (cmpnd_list);
	if (ac_id2amlgm_list) sxfree (ac_id2amlgm_list);
	if (cc2cmpnd_list) sxfree (cc2cmpnd_list);
	if (XH_is_allocated (XH_cc_list)) XH_free (&XH_cc_list);
	if (initial_compound_component_id_set) sxfree (initial_compound_component_id_set);
	if (final_compound_component_id_set) sxfree (final_compound_component_id_set);

#if 0
	fputs ("#endif /* COMPOUND */\n\n", stdout);
	fputs ("\n#endif /* include_full_lexicon */\n", stdout);
#endif /* 0 */
      }

      if (use_a_dico) {
	sxfree (mot2.string), mot2.string = NULL;
	sxfree (mot2.code), mot2.code = NULL;
	sxfree (mot2.lgth), mot2.lgth = NULL;
	sxfree (mot2.min), mot2.min = NULL;
	sxfree (mot2.max), mot2.max = NULL;

	if (dico.comb_vector) sxfree (dico.comb_vector), dico.comb_vector = NULL;
      }
    }
    
    if (amalgam_size > 1) {
      /* Ajouter le 30/06/2004 pour la campagne easy ... */
      /* On doit etre capable, a partir de l'expansion d'un amalgam "de le" de trouver "du" */
      int           am_id, am_name, ste, ac_id, if_id, id, top, single;
      XH_header     amalgam_list;
      X_header      X_if_hd;
      NODE          *visited, *brother;
      int           *amalgam_ids;
      sxword_header amalgam_names;

      XH_alloc (&amalgam_list, "amalgam_list", amalgam_size, 1, 3, NULL, NULL);
      amalgam_ids = (int*) sxalloc (amalgam_size, sizeof (int));
      sxword_alloc (&amalgam_names, "amalgam_names", amalgam_size, 1, 12,
		    sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
      X_alloc (&X_if_hd, "X_if", amalgam_size*3+1, 1, NULL, NULL);

      for (id = 1; id < amalgam_size; id++) {
	visited = amalgam2visited [id];
	ac_id = visited->id;
	/* ac_id est le ste de l'amalgame "au" ds amalgam_or_compound_names */
	am_name = sxword_2save (&amalgam_names,
				SXWORD_get (amalgam_or_compound_component_names, ac_id),
				SXWORD_len (amalgam_or_compound_component_names, ac_id));

	for (brother = visited->brother; brother != NULL; brother = brother->brother) {
	  ste = brother->token.string_table_entry;
	  if_id = sxword_2retrieve (&inflected_form_names,
				    sxstrget (ste),
				    sxstrlen (ste));
	  XH_push (amalgam_list, if_id-1);
	  /* L'id stocke' ds le dico des formes flechies aura la valeur if_id-1 */
	  X_set (&X_if_hd, if_id-1, &single); /* Cette ff est un composant d'un amalgam */
	}

	/* PB si 2 amalgam differents ont la meme expansion !! */
	XH_set (&amalgam_list, &am_id);
	amalgam_ids [am_id] = am_name;
      }

#if 0
      fputs ("\n#ifdef include_full_lexicon\n", stdout);
#endif /* 0 */

#if 0
      fputs ("#ifdef EASY\n", stdout);
#endif /* 0 */
      out_ifdef ("amalgam_list");
      XH2c (&amalgam_list, stdout, "amalgam_list", TRUE /* static */);
      out_endif ("amalgam_list");

      top = XH_top (amalgam_list)-1;
	
      out_ifdef ("amalgam_names");
      printf ("\nstatic char *amalgam_names [%i] = {\"\",\n", top+1);

      for (am_id = 1; am_id <= top; am_id++) {
	printf ("/* %i */ \"%s\",\n", am_id, SXWORD_get (amalgam_names, amalgam_ids [am_id]));
      }

      fputs ("};\n", stdout);
      out_endif ("amalgam_names");

      out_ifdef ("X_amalgam_if_id_hd");
      X_to_c (&X_if_hd, stdout, "X_amalgam_if_id_hd", TRUE);
      out_endif ("X_amalgam_if_id_hd");

#if 0
      fputs ("#endif /* EASY */\n", stdout);
      fputs ("\n#endif /* include_full_lexicon */\n", stdout);
#endif /* 0 */

      XH_free (&amalgam_list);
      sxfree (amalgam_ids);
      sxword_free (&amalgam_names);
      X_free (&X_if_hd);
    }

    if (amalgam2visited || compound2visited)
      sxword_free (&amalgam_or_compound_component_names);

    if (amalgam2visited) {
      sxfree (amalgam2visited), amalgam2visited = NULL;
      sxfree (amalgam_id_set), amalgam_id_set = NULL;
    }

    if (compound2visited) {
      sxfree (compound2visited), amalgam2visited = NULL;
      sxfree (compound_component_id_set), compound_component_id_set = NULL;
    }


    {
      /* Le 22/12/05 */
      /* Je sors le "dico" des symboles terminaux, ca va permettre sur des ff de la forme
	 pref__suff inconnues ds le dico des if de les decouper en 2 et de rechercher
	 pref ds le dico des if et suff ds le dico des terminaux et donc de forcer les occurrences
	 de pref de la categorie suff */
      int       id;
      BOOLEAN   dico_OK = TRUE;

      if (use_a_dico) {
	mot2.size = tmax+1;
	mot2.string = (char**) sxalloc (mot2.size, sizeof (char*));
	mot2.string [0] = (char*) NULL;
	mot2.code = (int*) sxalloc (mot2.size, sizeof (int));
	mot2.code [0] = 0;
	mot2.lgth = (int*) sxalloc (mot2.size, sizeof (int));
	mot2.lgth [0] = 0;
	mot2.optim_kind = SPACE; /* Priorite a l'espace sur le temps */
	mot2.process_kind = TOTAL; /* La chaine totale est prise en compte et pas seulement les prefixe et suffixe
				      discriminants minimaux de chaque chaine sont entres ds l'automate */
	mot2.print_on_sxtty = FALSE; /* on travaille ds le silence */
	mot2.min = (int*) sxalloc (mot2.size, sizeof (int));
	mot2.max = (int*) sxalloc (mot2.size, sizeof (int));

	for (id = 1; id <= tmax; id++) {
	  mot2.string [id] = SXWORD_get (t_names, id+1);
	  mot2.lgth [id] = SXWORD_len (t_names, id+1);
	  mot2.code [id] = id;
	}

	mot2.nb = tmax;

	sxdico_process (mot2, &dico);

	if ((((unsigned int) (~0))>>dico.base_shift) < dico.max) {
	  sxtmsg (SXGET_TOKEN (0).source_index.file_name,
		  "%sToo many terminal definitions, dictionary not produced\n",
		  sxtab_ptr->err_titles [2]);
	  dico_OK = FALSE;
	}
      }

      if (dico_OK) {
#if 0
	fputs ("\n#ifdef include_full_lexicon\n", stdout);
	fputs ("\n#ifdef ICO\n", stdout);
#endif /* 0 */

	if (use_a_dico) {
	  out_ifdef ("dico_t");
	  sxdico2c (&dico, stdout, "dico_t", TRUE);
	  out_endif ("dico_t");
	}
	else {
	  out_ifdef ("t_names");
	  sxword_to_c (&t_names, stdout, "t_names", TRUE);
	  out_endif ("t_names");
	}

#if 0
	fputs ("#endif /* ICO */\n", stdout);
	fputs ("\n#endif /* include_full_lexicon */\n", stdout);
#endif /* 0 */
      }

      if (use_a_dico) {
	sxfree (mot2.string), mot2.string = NULL;
	sxfree (mot2.code), mot2.code = NULL;
	sxfree (mot2.lgth), mot2.lgth = NULL;
	sxfree (mot2.min), mot2.min = NULL;
	sxfree (mot2.max), mot2.max = NULL;
	    
	if (dico.comb_vector) sxfree (dico.comb_vector), dico.comb_vector = NULL;
      }
    }

    {
      int lr_id, if_id, id, if_id2t_list_top, x, y, field_id, field_kind;
      int *id2display, *t2t_list;

      struct if_id2t_list {
	int t, structure_arg;
      } *if_id2t_list;

      struct structure_args_list {
	int lexeme, struct_id, priority;
      } *structure_args_list;

      int       syno, t, struct_id;
      int       *t2lexeme_nb;
      SXBA      lexical_rule_set, t_set;
      BOOLEAN   dico_OK = TRUE;

      t_set = sxba_calloc (TERMINAL_nb+1);
      t2lexeme_nb = (int*) sxcalloc (TERMINAL_nb+1, sizeof (int));
      lexical_rule_set = sxba_calloc (lexical_rule_nb+1);
      t2t_list = (int*) sxalloc (TERMINAL_nb+1, sizeof (int));

      id2display = (int*) sxalloc (inflected_form_nb+2, sizeof (int));
      id2display [0] = 0;
      if_id2t_list = (struct if_id2t_list*) sxalloc (lexical_rule_nb+2, sizeof (struct if_id2t_list));
      if_id2t_list [0].t = if_id2t_list [0].structure_arg = 0;
      id2display [1] = if_id2t_list_top = if_id2t_list [1].structure_arg = 1;
      structure_args_list = (struct structure_args_list*) sxalloc (lexical_rule_nb+2, sizeof (struct structure_args_list));
      structure_args_list [0].lexeme = structure_args_list [0].struct_id = structure_args_list [0].priority = 0;

      if (use_a_dico) {
	mot2.size = inflected_form_nb;
	mot2.string = (char**) sxalloc (inflected_form_nb+1, sizeof (char*));
	mot2.string [0] = (char*) NULL;
	mot2.code = (int*) sxalloc (inflected_form_nb+1, sizeof (int));
	mot2.code [0] = 0;
	mot2.lgth = (int*) sxalloc (inflected_form_nb+1, sizeof (int));
	mot2.lgth [0] = 0;
	mot2.optim_kind = SPACE; /* Priorite a l'espace sur le temps */
	mot2.process_kind = TOTAL; /* La chaine totale est prise en compte et pas seulement les prefixe et suffixe
				      discriminants minimaux de chaque chaine sont entres ds l'automate */
	mot2.print_on_sxtty = FALSE; /* on travaille ds le silence */
	mot2.min = (int*) sxalloc (inflected_form_nb+1, sizeof (int));
	mot2.max = (int*) sxalloc (inflected_form_nb+1, sizeof (int));

	id = 0;

	for (lr_id = 1; lr_id <= lexical_rule_nb; lr_id++) {
	  if (!SXBA_bit_is_set (lexical_rule_set, lr_id)) {
	    y = XH_X (lexical_rule_stack_hd, lr_id);
	    if_id = XH_list_elem (lexical_rule_stack_hd, y++);
	    t = XH_list_elem (lexical_rule_stack_hd, y);

	    for (syno = if_id2lr_id [if_id]; syno != 0; syno = lr_id2prev [syno]) {
	      SXBA_1_bit (lexical_rule_set, syno);
	    }

	    id++;
	    /* Ici on a id+1 !== if_id */

	    mot2.string [id] = SXWORD_get (inflected_form_names, if_id);
	    mot2.lgth [id] = SXWORD_len (inflected_form_names, if_id);	
	    mot2.code [id] = id;

	    for (syno = if_id2lr_id [if_id]; syno != 0; syno = lr_id2prev [syno]) {
	      SXBA_1_bit (lexical_rule_set, syno); /* vu */
	      t = XH_list_elem (lexical_rule_stack_hd, XH_X (lexical_rule_stack_hd, syno)+1);

	      if (SXBA_bit_is_reset_set (t_set, t)) {
		t2lexeme_nb [t] = 1;
		if_id2t_list [if_id2t_list_top].t = t;
		t2t_list [t] = if_id2t_list_top++;
	      }
	      else
		t2lexeme_nb [t]++;
	    }

	    for (x = id2display [id]; x < if_id2t_list_top; x++) {
	      t = if_id2t_list [x].t;
	      SXBA_0_bit (t_set, t);
	      /* finesse !! */
	      if_id2t_list [x+1].structure_arg = if_id2t_list [x].structure_arg += t2lexeme_nb [t];
	    }

	    for (syno = if_id2lr_id [if_id]; syno != 0; syno = lr_id2prev [syno]) {
	      y = XH_X (lexical_rule_stack_hd, syno)+1;
	      t = XH_list_elem (lexical_rule_stack_hd, y++);
	      x = --(if_id2t_list [t2t_list [t]].structure_arg);
	      structure_args_list [x].lexeme = XH_list_elem (lexical_rule_stack_hd, y++);
	      structure_args_list [x].struct_id = XH_list_elem (lexical_rule_stack_hd, y);
	      structure_args_list [x].priority = lr_id2priority [syno];
	    }
      
	    id2display [id+1] = if_id2t_list_top;
	  }
	}
    
	mot2.nb = id;

	sxdico_process (mot2, &dico);

	if ((((unsigned int) (~0))>>dico.base_shift) < dico.max) {
	  sxtmsg (SXGET_TOKEN (0).source_index.file_name,
		  "%sToo many lexical definitions, dictionary not produced\n",
		  sxtab_ptr->err_titles [2]);
	  dico_OK = FALSE;
	}
      }
      else {
	id2display [2] = if_id2t_list [2].structure_arg = 1;
	mot2.nb = inflected_form_nb;

	for (if_id = 2; if_id <= inflected_form_nb; if_id++) {
	  for (lr_id = if_id2lr_id [if_id]; lr_id != 0; lr_id = lr_id2prev [lr_id]) {
	    t = XH_list_elem (lexical_rule_stack_hd, XH_X (lexical_rule_stack_hd, lr_id)+1);

	    if (SXBA_bit_is_reset_set (t_set, t)) {
	      t2lexeme_nb [t] = 1;
	      if_id2t_list [if_id2t_list_top].t = t;
	      t2t_list [t] = if_id2t_list_top++;
	    }
	    else
	      t2lexeme_nb [t]++;
	  }

	  for (x = id2display [if_id]; x < if_id2t_list_top; x++) {
	    t = if_id2t_list [x].t;
	    SXBA_0_bit (t_set, t);
	    /* finesse !! */
	    if_id2t_list [x+1].structure_arg = if_id2t_list [x].structure_arg += t2lexeme_nb [t];
	  }

	  for (lr_id = if_id2lr_id [if_id]; lr_id != 0; lr_id = lr_id2prev [lr_id]) {
	    y = XH_X (lexical_rule_stack_hd, lr_id)+1;
	    t = XH_list_elem (lexical_rule_stack_hd, y++);
	    x = --(if_id2t_list [t2t_list [t]].structure_arg);
	    structure_args_list [x].lexeme = XH_list_elem (lexical_rule_stack_hd, y++);
	    structure_args_list [x].struct_id = XH_list_elem (lexical_rule_stack_hd, y);
	    structure_args_list [x].priority = lr_id2priority [lr_id];
	  }
      
	  id2display [if_id+1] = if_id2t_list_top;
	}
      }

      if (dico_OK) {
	XH_header stack;
	int cur, smppass_bot, smppass_top, smppass_if_id, smppass_t, t_list_id, lim, i, lgth, smppass_x;

#if 0
	fputs ("\n#ifdef include_full_lexicon\n", stdout);
	fputs ("\n#ifdef ICO_INFLECTED_FORMS\n", stdout);
#endif /* 0 */

	if (use_a_dico) {
	  out_ifdef ("dico_if");
	  sxdico2c (&dico, stdout, "dico_if", TRUE);
	  out_endif ("dico_if");
	}
	else {
	  out_ifdef ("inflected_form_names");
	  sxword_to_c (&inflected_form_names, stdout, "inflected_form_names", TRUE);
	  out_endif ("inflected_form_names");
	}
	
#if 0
	fputs ("#endif /* ICO_INFLECTED_FORMS */\n", stdout);
	fputs ("\n#endif /* include_full_lexicon */\n", stdout);
#endif /* 0 */

#if 0
	fputs ("\n\n#ifdef ICO\n", stdout);
#endif /* 0 */

	/* Ca permet ds le traitement du source sous forme de DAG d'utiliser if_id2tok_list_id  */
	fputs ("\n#define is_LFG\n", stdout);

	XH_alloc (&stack, "stack", mot2.nb+1, 1, (id2display [mot2.nb+1]/mot2.nb)+1, NULL, NULL);

	out_ifdef ("if_id2tok_list_id");
	printf ("\n/* Ce tableau n'existe que ds le cas ou le lexique provient d'une LFG */\n\
static int if_id2tok_list_id [%i] = {\n\
/* 0 */ 0, ", mot2.nb+1);

	smppass_top = id2display [1];

	for (smppass_if_id = 1; smppass_if_id <= mot2.nb; smppass_if_id++) {
	  smppass_bot = smppass_top;
	  smppass_top = id2display [smppass_if_id+1];

	  for (cur = smppass_bot; cur < smppass_top; cur++) {
	    smppass_t = if_id2t_list [cur].t;
	    SXBA_1_bit (t_set, smppass_t);
	  }

	  smppass_t = 0;

	  while ((smppass_t = sxba_scan_reset (t_set, smppass_t)) > 0) {
	    XH_push (stack, smppass_t);
	  }

	  XH_set (&stack, &t_list_id);

	  if (smppass_if_id%10 == 0)
	    printf ("\n/* %i */", smppass_if_id);

	  printf ("%i, ", t_list_id);
	}

	fputs ("\n};\n", stdout);
	out_endif ("if_id2tok_list_id");

	lim = XH_top (stack);
	out_ifdef ("tok_disp");
	printf ("static int tok_disp [%i] = {\n\
/* 0 */ 0, 1, ", lim+1);
	smppass_x = 1;

	for (i = 1; i < lim; i++) {
	  if ((i % 10) == 0)
	    printf ("\n/* %i */", i);

	  lgth = XH_list_lgth (stack, i);
	  smppass_x += lgth;
	  printf ("%i, ", smppass_x);
	}

	fputs ("\n};\n", stdout);
	out_endif ("tok_disp");

	out_ifdef ("tok_list");
	printf ("static int tok_list [%i] = {0,\n", smppass_x);
	smppass_top = XH_X (stack, 1);
	smppass_x = 1;

	for (i = 1; i < lim; i++) {
	  printf ("/* %i */", i);

	  smppass_bot = smppass_top;
	  smppass_top = XH_X (stack, i+1);

	  for (cur = smppass_bot; cur < smppass_top; cur++) {
	    printf ("%i, ", XH_list_elem (stack, cur));
	  }

	  fputs ("\n", stdout);
	}

	fputs ("};\n", stdout);
	out_endif ("tok_list");

	XH_free (&stack);

	out_ifdef ("tok_str");
	printf ("static char *tok_str [%i] = {\n\
/* 0 */ \"\",\n", tmax+1);

	for (i = 1; i <= tmax; i++) {
	  printf ("/* %i */ \"%s\",\n", i, SXWORD_get (t_names, i+1));
	}

	fputs ("};\n", stdout);
	out_endif ("tok_str");

#if 0 // sinon �a va hurler h�h�h�
	fputs ("static int *code_stack;\n", stdout);
#endif /* 0 */

#if 0
	fputs ("#endif /* ICO */\n", stdout);
#endif /* 0 */

	printf ("\n\n%s", KIND_string);

	printf ("#define MAX_FIELD_ID %i\n", max_field_id);
	printf ("#define MAX_FIELD_ID_SHIFT %i\n", sxlast_bit (max_field_id));
	printf ("#define MAX_SHARED_F_STRUCTURE_ID %i\n", max_shared_f_structure_id);
	printf ("#define ATOM_Aij %i\n", atom_Aij);
	printf ("#define ATOM_Pij %i\n\n", atom_Pij);

#if 0
	fputs ("\n\n#ifdef LEXICON\n", stdout);
#endif /* 0 */
	
	{
	  /* Mettre ca ds sxc_to_str.c */
	  /* SXD : pour supprimer les diacritics CHAR+128 */
	  char SXD [] =
	    "\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\
\240�������������������������������AAAAAAACEEEEIIIIDNOOOOO��UUUUY��aaaaaaaceeeeiiiidnooooo��uuuuy�\377\
\000\001\002\003\004\005\006\007\b\t\n\013\f\r\016\017\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037\
 !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\177\
\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\
\240�������������������������������AAAAAAACEEEEIIIIDNOOOOO��UUUUY��aaaaaaaceeeeiiiidnooooo��uuuuy�\377";

	  /* mettre ds sxu2.h avec 
	     extern char SXD [384]; */
#define sxtonodiacritics(c) ((SXD+128)[c])

	  int smppass_top_2, smppass_id_2, str_lgth, smppass_i_2;
	  char cur_char, *str;

	  smppass_top_2 = SXWORD_top (field_names);

	  for (smppass_id_2 = pred_id; smppass_id_2 < smppass_top_2; smppass_id_2++) {
	    str = SXWORD_get (field_names, smppass_id_2);
	    str_lgth = SXWORD_len (field_names, smppass_id_2);
	    varstr_raz (vstr);

	    for (smppass_i_2 = 0; smppass_i_2 < str_lgth; smppass_i_2++) {
	      cur_char = str [smppass_i_2];

	      if (cur_char == '-')
		cur_char = '_';
	      else
		cur_char = sxtonodiacritics (sxtolower (cur_char));

	      *(vstr->current)++ = cur_char;
	    }

	    varstr_complete (vstr);
	    printf ("#define %s_id %i\n",  varstr_tostr (vstr), smppass_id_2);
	  }
	}

#if 0
	fputs ("\n#ifdef include_full_lexicon\n", stdout);
#endif /* 0 */

	gen_structure_hd ();

#if 0 // parti dans lex_lfg.h
	printf ("\nstatic struct pred_val {\n\
int lexeme, sous_cat1, sous_cat2;\n\
} pred_vals [%i] = {{0, 0, 0},", XH_top (pred_val_hd));
#endif /* 0 */

	out_ifdef ("pred_vals");
	printf ("\nstatic struct pred_val pred_vals [%i] = {{0, 0, 0},", XH_top (pred_val_hd));

	for (id = 1 ; id < XH_top (pred_val_hd); id++) {
	  if (id%5==1)
	    printf ("\n/* %i */ ", id);

	  smppass_bot = XH_X (pred_val_hd, id);
	  printf ("{%i, %i, %i}, ",
		  XH_list_elem (pred_val_hd, smppass_bot), XH_list_elem (pred_val_hd, smppass_bot+1), XH_list_elem (pred_val_hd, smppass_bot+2));
	}

	fputs ("};\n", stdout);
	out_endif ("pred_vals");

	out_ifdef ("sous_cat");
	printf ("\nstatic int sous_cat [%i] = {0,", XH_top (sous_cat_hd)+1);

	for (id = 1 ; id <= XH_top (sous_cat_hd); id++) {
	  if (id%10==1)
	    printf ("\n/* %i */ ", id);

	  printf ("%i, ", XH_X (sous_cat_hd, id));
	}

	fputs ("};\n", stdout);
	out_endif ("sous_cat");

	out_ifdef ("sous_cat_list");
	printf ("\nstatic int sous_cat_list [%i] = {0,", XH_list_top (sous_cat_hd));

	for (id = 1 ; id <= XH_top (sous_cat_hd); id++) {
	  smppass_bot = XH_X (sous_cat_hd, id);
	  smppass_top = XH_X (sous_cat_hd, id+1);
	  printf ("\n/* %i */ ", id);

	  for (smppass_bot = XH_X (sous_cat_hd, id), smppass_top = XH_X (sous_cat_hd, id+1); smppass_bot < smppass_top; smppass_bot++) {
	    printf ("%i, ", XH_list_elem (sous_cat_hd, smppass_bot));
	  }
	}

	fputs ("};\n", stdout);
	out_endif ("sous_cat_list");

	out_ifdef ("compound_field");
	printf ("\nstatic int compound_field [%i] = {0,", XH_top (compound_fields_hd)+1);

	for (id = 1 ; id <= XH_top (compound_fields_hd); id++) {
	  if (id%10==1)
	    printf ("\n/* %i */ ", id);

	  printf ("%i, ", XH_X (compound_fields_hd, id));
	}

	fputs ("};\n", stdout);
	out_endif ("compound_field");

	out_ifdef ("compound_field_list");
	printf ("\nstatic int compound_field_list [%i] = {0,", XH_list_top (compound_fields_hd)+1);

	for (id = 1 ; id < XH_top (compound_fields_hd); id++) {
	  printf ("\n/* %i */ ", id);

	  for (smppass_bot = XH_X (compound_fields_hd, id), smppass_top = XH_X (compound_fields_hd, id+1); smppass_bot < smppass_top; smppass_bot++) {
	    printf ("%i, ", XH_list_elem (compound_fields_hd, smppass_bot));
	  }
	}

	printf ("\n/* %i */ 0,\n", id);

	fputs ("};\n", stdout);
	out_endif ("compound_field_list");

	out_ifdef ("priority_hd");
	printf ("\nstatic int priority_hd [%i] = {0,", XH_top (XH_priority_hd)+1);

	for (id = 1 ; id <= XH_top (XH_priority_hd); id++) {
	  if (id%10==1)
	    printf ("\n/* %i */ ", id);

	  printf ("%i, ", XH_X (XH_priority_hd, id));
	}

	fputs ("};\n", stdout);
	out_endif ("priority_hd");

	out_ifdef ("priority_list");
	printf ("\nstatic int priority_list [%i] = {0,", XH_list_top (XH_priority_hd)+1);

	for (id = 1 ; id < XH_top (XH_priority_hd); id++) {
	  printf ("\n/* %i */ ", id);

	  for (smppass_bot = XH_X (XH_priority_hd, id), smppass_top = XH_X (XH_priority_hd, id+1); smppass_bot < smppass_top; smppass_bot++) {
	    printf ("%i, ", XH_list_elem (XH_priority_hd, smppass_bot));
	  }
	}

	printf ("\n/* %i */ 0,\n", id);

	fputs ("};\n", stdout);
	out_endif ("priority_list");

	out_ifdef ("argument_set");
	sxba2c (argument_set, stdout, "argument_set", "", TRUE, vstr);
	out_endif ("argument_set");

	out_ifdef ("inflected_form2display");
	printf ("\nstatic int inflected_form2display [%i] = {", mot2.nb+2);

	for (id = 0; id <= mot2.nb+1; id++) {
	  if (id%10 == 0)
	    printf ("\n/* %i */", id);

	  printf ("%i, ", id2display [id]);
	}

	fputs ("};\n", stdout);
	out_endif ("inflected_form2display");

	smppass_top = id2display [mot2.nb+1];
#if 0
	printf ("\nstatic struct if_id2t_list {\n\
int t, list;\n\
} if_id2t_list [%i] = {", smppass_top+1);
#endif /* 0 */

	out_ifdef ("if_id2t_list");
	printf ("\nstatic struct if_id2t_list if_id2t_list [%i] = {", smppass_top+1);
	if_id2t_list [smppass_top].t = 0;

	for (smppass_x = 0; smppass_x <= smppass_top; smppass_x++) {
	  if (smppass_x%10 == 0)
	    printf ("\n/* %i */", smppass_x);

	  printf ("{%i, %i}, ", if_id2t_list [smppass_x].t, if_id2t_list [smppass_x].structure_arg);
	}

	fputs ("};\n", stdout);
	out_endif ("if_id2t_list");

	smppass_top = if_id2t_list [smppass_top].structure_arg;
#if 0
	printf ("\nstatic struct structure_args_list {\n\
int lexeme, struct_id, priority;\n\
} structure_args_list [%i] = {", smppass_top+1);
#endif /* 0 */

	out_ifdef ("structure_args_list");
	printf ("\nstatic struct structure_args_list structure_args_list [%i] = {", smppass_top+1);
	structure_args_list [smppass_top].lexeme = structure_args_list [smppass_top].struct_id = structure_args_list [smppass_top].priority = 0;

	for (smppass_x = 0; smppass_x <= smppass_top; smppass_x++) {
	  if (smppass_x%10 == 0)
	    printf ("\n/* %i */", smppass_x);

	  struct_id = structure_args_list [smppass_x].struct_id;
	  struct_id = (struct_id&XC0) | struct_id2head [struct_id&X3F];

	  printf ("{%i, %i, %i}, ",
		  structure_args_list [smppass_x].lexeme,
		  struct_id,
		  structure_args_list [smppass_x].priority);
	}

	fputs ("};\n", stdout);
	out_endif ("structure_args_list");

	{
	  /* On prepare et on sort les structures qui permettent, a partir d'un lexeme_id de
	     retrouver toutes les structures principales ayant ce lexeme_id comme pred */
	  int top2, nb, lexeme_id, smppass_x_2;
	  int *lexeme_id2display, *args_ref_list;

	  top2 = SXWORD_top (lexeme_names);
	  lexeme_id2display = (int*) sxcalloc (top2+1, sizeof (int));

	  for (smppass_x_2 = 0; smppass_x_2 <= smppass_top; smppass_x_2++) {
	    lexeme_id = structure_args_list [smppass_x_2].lexeme;

	    if (lexeme_id) {
	      lexeme_id2display [lexeme_id]++;
	    }
	  }

	  nb = 1;

	  for (lexeme_id = 2; lexeme_id < top2; lexeme_id++) {
	    lexeme_id2display [lexeme_id] = nb += lexeme_id2display [lexeme_id];
	  }

	  lexeme_id2display [top2] = nb;

	  args_ref_list = (int*) sxalloc (nb+1, sizeof (int));
	  args_ref_list [0] = args_ref_list [nb] = 0;

	  for (smppass_x_2 = 0; smppass_x_2 <= smppass_top; smppass_x_2++) {
	    lexeme_id = structure_args_list [smppass_x_2].lexeme;

	    if (lexeme_id) {
	      args_ref_list [--lexeme_id2display [lexeme_id]] = smppass_x_2;
	    }
	  }
	
#if 0
	  fputs ("#if IS_OPERATOR_LEX_REF\n\
/* The operator \"lex\" is used within the associated LFG */\n", stdout);
#endif /* 0 */

	  out_ifdef ("lexeme_id2display");
	  printf ("\nstatic int lexeme_id2display [%i] = {", top2+1);

	  for (lexeme_id = 0; lexeme_id <= top2; lexeme_id++) {
	    if (lexeme_id%10 == 0)
	      printf ("\n/* %i */ ", lexeme_id);

	    printf ("%i, ", lexeme_id2display [lexeme_id]);
	  }

	  fputs ("};\n", stdout);
	  out_endif ("lexeme_id2display");
	
	  out_ifdef ("args_ref_list");
	  printf ("\nstatic int args_ref_list [%i] = {", nb+1);

	  for (smppass_x_2 = 0; smppass_x_2 <= nb; smppass_x_2++) {
	    if (smppass_x_2%10 == 0)
	      printf ("\n/* %i */ ", smppass_x_2);

	    printf ("%i, ", args_ref_list [smppass_x_2]);
	  }

	  fputs ("};\n", stdout);
	  out_endif ("args_ref_list");

#if 0
	  fputs ("#endif /* IS_OPERATOR_LEX_REF */\n", stdout);
#endif /* 0 */
	}

#ifndef ESSAI
	{
	  SXBA       current_atom_set;

	  out_ifdef ("field_id2atom_field_id");
	  for (field_id = EMPTY_STE+1; field_id <= max_field_id; field_id++) {
	    field_kind = field_id2kind [field_id];

	    if (field_kind == ATOM_KIND) {
	      printf ("\nstatic int af_%i [] = {\n\
/* 0 */ 0, ", field_id);
	      current_atom_set = field_id2atom_set [field_id];
	      local_atom_id = 0;
	      atom_id = 0;

	      while ((atom_id = sxba_scan (current_atom_set, atom_id)) > 0) {
		local_atom_id++;

		if (local_atom_id%10 == 0)
		  printf ("\n/* %i */ ", local_atom_id);

		printf ("%i, ", atom_id);
	      }

	      fputs ("\n};\n", stdout);
	    }
	  }

	  printf ("\nstatic int *field_id2atom_field_id [%i] = {\n\
/* 0 */ NULL, NULL, ", max_field_id+1);

	  for (field_id = EMPTY_STE+1; field_id <= max_field_id; field_id++) {
	    if (field_id%10 == 0)
	      printf ("\n/* %i */ ", field_id);

	    field_kind = field_id2kind [field_id];

	    if (field_kind == ATOM_KIND)
	      printf ("af_%i ,", field_id);
	    else
	      fputs ("NULL ,", stdout);
	  }

	  fputs ("};\n", stdout);
	  out_endif ("field_id2atom_field_id");
	}
#endif /* ESSAI */

	smppass_top = SXWORD_top (field_names);

	out_ifdef ("field_id2kind");
	printf ("\nstatic unsigned char field_id2kind [%i] = {", smppass_top);
	field_id2kind [0] = field_id2kind [1] = 0;

	for (id = 0; id < smppass_top; id++) {
	  if (id%5 == 0)
	    printf ("\n/* %i */", id);

	  printf ("0X%X, ", field_id2kind [id]);
	}

	fputs ("};\n", stdout);
	out_endif ("field_id2kind");

	smppass_top = SXWORD_top (field_names);
	out_ifdef ("field_id2string");
	printf ("\nstatic char *field_id2string [%i] = {", smppass_top);

	for (id = 0; id < smppass_top; id++) {
	  if (id%5 == 0)
	    printf ("\n/* %i */", id);

	  printf ("\"%s\", ", SXWORD_get (field_names, id));
	}

	fputs ("};\n", stdout);
	out_endif ("field_id2string");

	{
	  int pass_nb, max_pass_nb;

	  max_pass_nb = 0;

	  out_ifdef ("field_id2pass_nb");
	  printf ("\nstatic int field_id2pass_nb [%i] = {", smppass_top);

	  for (id = 0; id < smppass_top; id++) {
	    if (id%10 == 0)
	      printf ("\n/* %i */", id);

	    pass_nb = field_id2pass_nb [id];

	    if (pass_nb > max_pass_nb)
	      max_pass_nb = pass_nb;

	    printf ("%i, ", pass_nb);
	  }

	  fputs ("};\n", stdout);
	  out_endif ("field_id2pass_nb");

	  out_ifdef ("atom_id2string");
	  printf ("\nstatic char *atom_id2string [%i] = {", max_unbounded_atom_id+1);
	  
	  for (id = 0; id <= max_unbounded_atom_id; id++) {
	    if (id%5 == 0)
	      printf ("\n/* %i */", id);
	    
	    printf ("\"%s\", ", SXWORD_get (atom_names, id));
	  }
	  
	  fputs ("};\n", stdout);
	  out_endif ("atom_id2string");
	  
	  /* On en aura besoin si IS_OPERATOR_LEXEME_REF */
	  out_ifdef ("atom_names");
	  sxword_to_c (&atom_names, stdout, "atom_names", TRUE);
	  out_endif ("atom_names");


	  out_ifdef ("field_id2atom_id_set");
	  for (field_id = EMPTY_STE+2 /* On saute pred */; field_id <= max_field_id; field_id++) {
	    field_kind = field_id2kind [field_id];

	    if (field_kind & ATOM_KIND) {
	      sxba2c (field_id2atom_set [field_id], stdout, "atom_id_set_for_", SXWORD_get (field_names, field_id), TRUE, vstr);
	    }
	  }
    
	  printf ("\n\
static SXBA_ELT field_id2atom_id_set [%i] = {\n\
/* ERROR_STE = 0 */ NULL,\n\
/* EMPTY_STE = 1 */ NULL,\n\
/* pred = 2 */ NULL,\n\
", max_field_id+1);
	  for (field_id = EMPTY_STE+2; field_id <= max_field_id; field_id++) {
	    field_kind = field_id2kind [field_id];

	    if (field_kind & ATOM_KIND) {
	      printf ("/* %s = %i */ &(atom_id_set_for_%s [0]),\n", SXWORD_get (field_names, field_id), field_id, SXWORD_get (field_names, field_id));
	    }
	    else
	      printf ("/* %s = %i */ NULL,\n", SXWORD_get (field_names, field_id), field_id);
	  }

	  fputs ("};\n", stdout);
	  out_endif ("field_id2atom_id_set");


	  smppass_top = SXWORD_top (lexeme_names);
	  out_ifdef ("lexeme_id2string");
	  printf ("\nstatic char *lexeme_id2string [%i] = {", smppass_top);
	  
	  for (id = 0; id < smppass_top; id++) {
	    if (id%10 == 0 || id%10 == 5)
	      printf ("\n/* %i */", id);
	    
	    printf ("\"%s\", ", SXWORD_get (lexeme_names, id));
	  }
	  
	  fputs ("};\n", stdout);
	  out_endif ("lexeme_id2string");

#if 0
	  fputs ("\n#endif /* include_full_lexicon */\n", stdout);
#endif /* 0 */

	  printf ("\n#define MAX_STATIC_ATOM_ID %i\n", max_static_atom_id);
	  printf ("\n#define MAX_UNBOUNDED_ATOM_ID %i\n", max_unbounded_atom_id);

	  printf ("\n#define MAX_PASS_NB %i\n\n", max_pass_nb);
	}

#if 0
	fputs ("#endif /* LEXICON */\n", stdout);
#endif /* 0 */
      }

      sxfree (lexical_rule_set);
      sxfree (t_set);
      sxfree (t2lexeme_nb);
      sxfree (id2display);
      sxfree (if_id2t_list);
      sxfree (structure_args_list);
      sxfree (t2t_list);

      if (if_id2lr_id) {
	sxword_free (&inflected_form_names);
	sxfree (if_id2lr_id), if_id2lr_id = NULL;
	XH_free (&lexical_rule_stack_hd);
	sxfree (lr_id2prev), lr_id2prev = NULL;
	sxfree (lr_id2priority), lr_id2priority = NULL;
      }
      
      if (use_a_dico) {
	sxfree (mot2.string), mot2.string = NULL;
	sxfree (mot2.code), mot2.code = NULL;
	sxfree (mot2.lgth), mot2.lgth = NULL;
	sxfree (mot2.min), mot2.min = NULL;
	sxfree (mot2.max), mot2.max = NULL;
	    
	if (dico.comb_vector) sxfree (dico.comb_vector), dico.comb_vector = NULL;
      }
    }

#if 0
    /* En fait, j'en ai pas besoin */
    /* Dictionnaire qui permet le traitement de l'operateur "lex" */
    /* Ses mots sont des lexemes */
    {
      int lexeme_nb, id, top, x, struct_id;

      lexeme_nb = SXWORD_top (lexeme_names)-1;

      mot2.size = lexeme_nb;
      mot2.string = (char**) sxalloc (lexeme_nb+1, sizeof (char*));
      mot2.string [0] = (char*) NULL;
      mot2.code = (int*) sxalloc (lexeme_nb+1, sizeof (int));
      mot2.code [0] = 0;
      mot2.lgth = (int*) sxalloc (lexeme_nb+1, sizeof (int));
      mot2.lgth [0] = 0;
      mot2.optim_kind = SPACE; /* Priorite a l'espace sur le temps */
      mot2.process_kind = TOTAL; /* La chaine totale est prise en compte et pas seulement les prefixe et suffixe
				    discriminants minimaux de chaque chaine sont entres ds l'automate */
      mot2.print_on_sxtty = FALSE; /* on travaille ds le silence */
      mot2.min = (int*) sxalloc (lexeme_nb+1, sizeof (int));
      mot2.max = (int*) sxalloc (lexeme_nb+1, sizeof (int));

      for (id = 1; id <= lexeme_nb; id++) {
	mot2.string [id] = SXWORD_get (lexeme_names, id);
	mot2.lgth [id] = SXWORD_len (lexeme_names, id);	
	mot2.code [id] = id;
      }

      mot2.nb = lexeme_nb;

      sxdico_process (mot2, &dico);

      if ((((unsigned int) (~0))>>dico.base_shift) < dico.max)
	sxtmsg (SXGET_TOKEN (sxplocals.Mtok_no).source_index.file_name,
		"%sToo many lexeme definitions, dictionary not produced\n",
		sxtab_ptr->err_titles [2]);
      else {
	fputs ("\n\n#ifdef LEXICON\n", stdout);

	print_dico (dico);

	fputs ("#endif /* LEXICON */\n", stdout);
      }
 
      sxfree (mot2.string), mot2.string = NULL;
      sxfree (mot2.code), mot2.code = NULL;
      sxfree (mot2.lgth), mot2.lgth = NULL;
      sxfree (mot2.min), mot2.min = NULL;
      sxfree (mot2.max), mot2.max = NULL;
	    
      if (dico.comb_vector) sxfree (dico.comb_vector), dico.comb_vector = NULL;
    }
#endif /* 0 */

    if (struct_id2head) sxfree (struct_id2head), struct_id2head = NULL;
  }

  /*   F I N A L I S A T I O N S   */
  /* ........... */
	
  if (amalgam_id_set) sxfree (amalgam_id_set);
  if (compound_component_id_set) sxfree (compound_component_id_set);

  if (t_id2t) {
    sxword_free (&t_names);
    sxfree (t_id2t), t_id2t = NULL;
    sxfree (t_id_set), t_id_set = NULL;
  }

  sxword_free (&field_names);
  sxfree (field_id2kind), field_id2kind = NULL;
  sxfree (field_id2pass_nb), field_id2pass_nb = NULL;
  XH_free (&compound_fields_hd);
  XH_free (&sous_cat_hd);
  sxword_free (&atom_names);
  sxbm_free (field_id2atom_set), field_id2atom_set = NULL;
  varstr_free (vstr), vstr = NULL;
  sxword_free (&macro_names);
  sxfree (macro_id2structure), macro_id2structure = NULL;
  sxfree (macro_id2visited), macro_id2visited = NULL;

  XH_free (&XH_priority_hd);

  sxword_free (&lexeme_names);

  XH_free (&structure_hd);
  XH_free (&pred_val_hd);
  XH_free (&adjunct_hd);
  XH_free (&atom_hd);
  free_level ();
  if (working_field_set) sxfree (working_field_set), working_field_set = NULL;
  if (argument_set) sxfree (argument_set), argument_set = NULL;
  if (pred_arg_set) sxfree (pred_arg_set), pred_arg_set = NULL;
  if (warg_set) sxfree (warg_set), warg_set = NULL;

  SS_free (SS_path), SS_path = NULL;

  if (working_compound_field_set) {
    sxfree (working_compound_field_set), working_compound_field_set = NULL;
    sxfree (working2_compound_field_set), working2_compound_field_set = NULL;
  }

  if (struct_index_set != NULL) {
    sxfree (struct_index_set), struct_index_set = NULL;
    sxfree (struct_index_set2), struct_index_set2 = NULL;
    sxfree (cyclic_index_set), cyclic_index_set = NULL;
    sxfree (struct_index2struct_id), struct_index2struct_id = NULL;
    sxfree (struct_index2field_id), struct_index2field_id = NULL;
  }

#ifndef ESSAI
  if (fieldXatom2local_id) {
    sxfree (fieldXatom2local_id), fieldXatom2local_id = NULL;
    sxfree (fieldXlocal2atom_id), fieldXlocal2atom_id = NULL;
    sxfree (local_id_area), local_id_area = NULL;
    sxfree (local_id_area2), local_id_area2 = NULL;
  }
#endif /* ESSAI */

  if (if_id2lr_id) {
    sxword_free (&inflected_form_names);
    sxfree (if_id2lr_id), if_id2lr_id = NULL;
    XH_free (&lexical_rule_stack_hd);
    sxfree (lr_id2prev), lr_id2prev = NULL;
    sxfree (lr_id2priority), lr_id2priority = NULL;
  }
}

void
lfg_lex_smp (what, sxtables_ptr)
     int what;
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
