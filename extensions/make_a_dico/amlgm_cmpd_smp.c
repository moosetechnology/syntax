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

/* ********************************************************************
   *  This program has been generated from amlgm_cmpd.at              *
   *  on Fri Jan  4 15:28:09 2008                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */

/*   I N C L U D E S   */
#define SXNODE struct amlgm_cmpd_node
#include "sxversion.h"
#include "varstr.h"
#include "sxunix.h"

struct amlgm_cmpd_node {
  SXNODE_HEADER_S SXVOID_NAME;

  /*
    your attribute declarations...
  */
  SXINT id;
  SXNODE  *next_visited;
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define AMLGM_CMPD_ROOT_n 2
#define CHAINE_n 3
#define IDENT_n 4
#define LHS_S_n 5
#define RHS_S_n 6
#define RULE_n 7
#define RULE_S_n 8
/*
E N D   N O D E   N A M E S
*/

#include "XH.h"
#include "XxY.h"
#include "fsa.h"

extern SXBOOLEAN                 is_static;
extern char                    *LANGUAGE_NAME;


static struct sxtables         *sxtab_ptr;
static SXINT                   rule_nb, lhs_id, rhs_id, ste_top;
static SXINT                   *word_stack, *acc_name_stack;
static SXNODE                    **lhs_id2first_visited;
static XH_header               lhs_hd, rhs_hd, rhs_list_hd;
static XxY_header              rule_hd;

static struct word_tree_struct lhs_acc_name_tree, lhs_tree;
static SXINT                   *ste2lhs_id, *ste2rhs_id, *rhs_id2ste;
static struct sxdfa_struct     lhs_acc_name_sxdfa, sxdfa_acc_lhs;
static struct sxdfa_comb       lhs_acc_name_comb;
static SXBA                    acc_entrance_set, acc_exit_set, rhs_id_set;


static void
gen_header (void)
{
  time_t date_time;
    
  date_time = time (0);

  printf ("\n\
/* ********************************************************************\n\
\tThis C file for the amalgam and compound word specification \"%s\" has been generated\n\
\tby the SYNTAX(*) amlgm_cmpd processor\n\
   ********************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
   ******************************************************************** */\n\n\n",
	  LANGUAGE_NAME);

  /* On sort la date actuelle  */
  printf ("#define AMLGM_CMPND_TIME %ld\n", date_time);

  fputs ("\n#include \"fsa.h\"\n",stdout);
}



static void
string2stack (SXINT *dstack, char *string, SXINT string_lgth)
{
  DRAZ (dstack);

  while (string_lgth) {
    DSPUSH (dstack, (SXINT)(unsigned char)(*string++));
    string_lgth--;
  }
}



static void
lhs_acc_name_tree_oflw (SXINT old_size, SXINT new_size)
{
  word_tree_oflw (&lhs_acc_name_tree, old_size, new_size);
}



static void
lhs_tree_oflw (SXINT old_size, SXINT new_size)
{
  word_tree_oflw (&lhs_tree, old_size, new_size);
}




static void
amlgm_cmpd_pi (void)
{
  /*
    I N H E R I T E D
  */
  switch (SXVISITED->father->name) {

  case ERROR_n :
    break;

  case AMLGM_CMPD_ROOT_n :/* SXVISITED->name = RULE_S_n */
    break;

  case LHS_S_n :/* SXVISITED->name = {CHAINE_n, IDENT_n} */
    break;

  case RHS_S_n :/* SXVISITED->name = {CHAINE_n, IDENT_n} */
    break;

  case RULE_n :
    switch (SXVISITED->position) {
    case 1 :/* SXVISITED->name = LHS_S_n */
      break;

    case 2 :/* SXVISITED->name = RHS_S_n */
      break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("make_a_dico/amlgm_cmpd_smp","unknown switch case #1");
#endif
      break;
    }

    break;

  case RULE_S_n :/* SXVISITED->name = RULE_n */
    break;

  default:
    break;
  }
  /* end amlgm_cmpd_pi */
}

static void
amlgm_cmpd_pd (void)
{
  /*
    D E R I V E D
  */
  SXINT                  ste, lgth, path;
  struct amlgm_cmpd_node *son;

  switch (SXVISITED->name) {
  case ERROR_n :
    break;

  case AMLGM_CMPD_ROOT_n :
    break;

  case CHAINE_n :
    break;

  case IDENT_n :
    break;

  case LHS_S_n :
    for (son = SXVISITED->son; son != 0; son = son->brother) {
      ste = son->token.string_table_entry;

      if ((son->id = ste2lhs_id [ste]) == 0) {
	son->id = ste2lhs_id [ste] = ++lhs_id;
	lgth = sxstrlen (ste);

	DCHECK (acc_name_stack, lgth);

	string2stack (acc_name_stack, sxstrget (ste), lgth);

	path = word_tree_add_a_word (&lhs_acc_name_tree, acc_name_stack, lhs_id);
      }

      XH_push (lhs_hd, son->id);
    }

    /* On relie les LHS_S_n qui on meme partie gauche (reecriture multiple de la meme partie gauche) */
    if (!XH_set (&lhs_hd, &SXVISITED->id)) {
      /* On retarde l'appel car on va mettre comme id un index qui repere une pile des rhs */
      SXVISITED->next_visited = NULL;
    }
    else {
      SXVISITED->next_visited = lhs_id2first_visited [SXVISITED->id];
    }
      
    /* liste des LHS_S_n qui ont meme partie gauche */
    lhs_id2first_visited [SXVISITED->id] = SXVISITED;

    break;

  case RHS_S_n :
    for (son = SXVISITED->son; son != 0; son = son->brother) {
      ste = son->token.string_table_entry;

      if ((son->id = ste2rhs_id [ste]) == 0) {
	son->id = ste2rhs_id [ste] = ++rhs_id;
	rhs_id2ste [rhs_id] = ste;
      }

      XH_push (rhs_hd, son->id);
    }

    if (XH_set (&rhs_hd, &SXVISITED->id)) {
      /* partie droite multiple */
    }

    break;

  case RULE_n :
    if (XxY_set (&rule_hd, SXVISITED->son->id, SXVISITED->son->brother->id, &SXVISITED->id)) {
      sxerror (SXVISITED->token.source_index,
	       sxtab_ptr->err_titles [2][0],
	       "%sMultiple rules are ignored.",
	       sxtab_ptr->err_titles [2]+1);
      SXVISITED->id = 0; /* Multiple */
    }

    break;

  case RULE_S_n :
    break;

  default:
    break;
  }
  /* end amlgm_cmpd_pd */
}




static void
smpopen (struct sxtables *sxtables_ptr)
{
  sxuse ( sxtables_ptr );
  sxatcvar.atc_lv.node_size = sizeof (struct amlgm_cmpd_node);
}


static void
escape_printf (char *str)
{
  while (*str != SXNUL) {
    if (*str == '\\' || *str == '"')
      printf ("\\%c",*str);
    else
      printf ("%c",*str);
    str++;
  }
}

static void
smppass (void)
{
  SXINT                  lhs_component_size = 0, rhs_component_size = 0, max_lhs_lgth = 0, degree, top, lgth, id, path, list_top, x, y, z, ste;
  char                   *string;
  struct amlgm_cmpd_node *node, *visited, *son;
  VARSTR                 vstr;
     
  node = sxatcvar.atc_lv.abstract_tree_root->son;
  rule_nb = node->degree;

  for (node = node->son; node != NULL; node = node->brother) {
    degree = node->son->degree;

    if (degree > max_lhs_lgth)
      max_lhs_lgth = degree;

    lhs_component_size += degree;
    rhs_component_size += node->son->brother->degree;
  }

  /*   I N I T I A L I S A T I O N S   */
  XH_alloc (&lhs_hd, "lhs_hd", rule_nb, 1, (lhs_component_size/rule_nb)+1, NULL, NULL);
  XH_alloc (&rhs_hd, "rhs_hd", rule_nb, 1, (rhs_component_size/rule_nb)+1, NULL, NULL);

  word_tree_alloc (&lhs_acc_name_tree, "lhs_acc_name_tree", rule_nb*((lhs_component_size/rule_nb)+1), 10, 1 /* Xforeach */, 0 /* Yforeach */, SXTRUE /* from_left_to_right */,
		   SXTRUE /* with_id */,
		   lhs_acc_name_tree_oflw,
#if EBUG
		   stdout
#else
		   NULL
#endif /* EBUG */
		   );

  word_tree_alloc (&lhs_tree, "lhs_tree", rule_nb, (lhs_component_size/rule_nb)+1, 1 /* Xforeach */, 0 /* Yforeach */, SXTRUE /* from_left_to_right */,
		   SXTRUE,
		   lhs_tree_oflw,
#if EBUG
		   stdout
#else
		   NULL
#endif /* EBUG */
		   );

  ste_top = SXSTRtop ();

  ste2lhs_id = (SXINT *) sxcalloc (ste_top+1, sizeof (SXINT));
  ste2rhs_id = (SXINT *) sxcalloc (ste_top+1, sizeof (SXINT));
  rhs_id2ste = (SXINT *) sxalloc (ste_top+1, sizeof (SXINT)), rhs_id2ste [0] = 0;

  word_stack = (SXINT*) sxalloc (max_lhs_lgth+1, sizeof (SXINT)), RAZ (word_stack);

  XxY_alloc (&rule_hd, "rule_hd", rule_nb, 1, 1 /* Xforeach */, 0, NULL, NULL);
  lhs_id2first_visited = (SXNODE **) sxalloc (rule_nb+1, sizeof (SXNODE *));

  DALLOC_STACK (acc_name_stack, 32);


  /*   A T T R I B U T E S    E V A L U A T I O N   */
  sxsmp (sxatcvar.atc_lv.abstract_tree_root, amlgm_cmpd_pi, amlgm_cmpd_pd);

  sxfree (ste2lhs_id), ste2lhs_id = NULL;


  gen_header ();


  /* On fabrique un sxdfa minimal */
  word_tree2sxdfa (&lhs_acc_name_tree, &lhs_acc_name_sxdfa, "lhs_acc_name_sxdfa", NULL, SXTRUE /* make_a_min_dag */);

  /* ...que l'on convertit en comb */
  sxdfa2comb_vector (&lhs_acc_name_sxdfa, 2 /* comb_kind */, 10000 /* comb_vector_threshold */, &lhs_acc_name_comb);
  sxdfa_free (&lhs_acc_name_sxdfa);

  /* ... et que l'on sort */
  sxdfa_comb2c (&lhs_acc_name_comb, stdout, "lhs_acc_name_comb", is_static);

  /* Il faut faire les acc_entrance_set et acc_exit_set */
  acc_entrance_set = sxba_calloc (lhs_id+1);
  acc_exit_set = sxba_calloc (lhs_id+1);
  
  XH_alloc (&rhs_list_hd, "rhs_list_hd", rule_nb, 1, (lhs_component_size/rule_nb)+1, NULL, NULL);
  rhs_id_set = sxba_calloc (XH_top (rhs_hd)+1);

  top = XH_top (lhs_hd);

  for (lhs_id = 1; lhs_id < top; lhs_id++) {
    lgth = 0;
    visited = lhs_id2first_visited [lhs_id]; /* vers LHS_S_n */
    son = visited->son;
    id = son->id; /* 1er fils */
    SXBA_1_bit (acc_entrance_set, id);

    while (son->brother) {
      son = son->brother;
    }

    id = son->id; /* dernier fils */
    SXBA_1_bit (acc_exit_set, id);

    while (visited != NULL) {
      if (visited->father->id != 0) {
	lgth++;
	SXBA_1_bit (rhs_id_set, visited->brother->id);
      }

      visited = visited->next_visited;		    
    }

    XH_push (rhs_list_hd, lgth);

    id = 0;

    while ((id = sxba_scan_reset (rhs_id_set, id)) > 0)
      XH_push (rhs_list_hd, id);

    XH_set (&rhs_list_hd, &id);
    /* id est l'identifiant que l'on va associer au chemin lhs_id */
    /* C'est l'identifiant de la liste ordonnee des rhs ayant lhs_id en LHS */
    RAZ (word_stack);

    for (son = lhs_id2first_visited [lhs_id]->son; son != 0; son = son->brother) {
      PUSH (word_stack, son->id);
    }
      
    path = word_tree_add_a_word (&lhs_tree, word_stack, XH_X (rhs_list_hd, id));
  }

  XH_free (&lhs_hd);
  sxfree (rhs_id_set), rhs_id_set = NULL;

  /* On sort les SXBA qui definissent les points d'entree et de sortie des LHS */
  vstr = varstr_alloc (32);
  sxba2c (acc_entrance_set, stdout, "", "acc_entrance_set", is_static, vstr);
  sxba2c (acc_exit_set, stdout, "", "acc_exit_set", is_static, vstr);
  varstr_free (vstr);

  /* On fabrique un sxdfa minimal */
  word_tree2sxdfa (&lhs_tree, &sxdfa_acc_lhs, "sxdfa_acc_lhs", NULL, SXTRUE /* make_a_min_dag */);
  /* Attention, il est a priori pas possible de le transformer en comb car les transitions sont des ids (et non des chars) */
  /* ... et que l'on sort */
  sxdfa2c (&sxdfa_acc_lhs, stdout, "sxdfa_acc_lhs", is_static);
  sxdfa_free (&sxdfa_acc_lhs);

  /* On ne sort que la partie "liste" de rhs_list_hd */
  list_top = XH_list_top (rhs_list_hd);
  top = XH_top (rhs_list_hd);
  
 printf ("\n\n\
/* A chaque lhs de regle (retournee par sxdfa_acc_lhs), correspond une liste de rhs organisee ici comme une stack */\n\
%sSXINT rhs_list [%ld] = {\n", is_static ? "static " : "", (SXINT) (list_top));
  printf ("/* list_top */ %ld,", (SXINT) (list_top-1));

  for (x = 1; x < top; x++) {
    y = rhs_list_hd.display [x].X;
    z = rhs_list_hd.display [x+1].X;
    printf ("\n/* %ld */ ", (SXINT) x);

    while (y < z) {
      printf ("%ld, ", (SXINT) rhs_list_hd.list [y]);
      y++;
    } 
  }

  fputs ("\n} /* End rhs_list */;\n\n", stdout);

  XH_free (&rhs_list_hd);

  /* ... et on sort rhs_hd */
  top = XH_top (rhs_hd);

   printf ("\n\n\
/* Chaque rhs est une liste (de lhs_acc ds rhs_id_list ou de strings ds rhs_str_list) */\n\
%sSXINT rhs_str_list_display [%ld] = {\n", is_static ? "static " : "", (SXINT) top+1);
  printf ("/* top */ %ld,\n", (SXINT) top);

  for (x = 1; x <= top; x++) {
    printf ("/* %ld */ %ld,\n", (SXINT) x, (SXINT)  rhs_hd.display [x].X); 
  } 

  fputs ("} /* End rhs_str_list_display */;\n", stdout);


  printf ("\n\n%schar *rhs_str_list [%ld] = {NULL,", is_static ? "static " : "", (SXINT) list_top+1);

  for (x = 1; x < top; x++) {
    y = rhs_hd.display [x].X;
    z = rhs_hd.display [x+1].X;
    printf ("\n/* [%ld..%ld] */ ", (SXINT) x, (SXINT) (z-1));
    while (y < z) {
      id = rhs_hd.list [y];
      ste = rhs_id2ste [id];


      printf ("\"");
      escape_printf (sxstrget (ste));
      printf ("\", ");
      y++;
    } 
  }

  printf ("\n/* list_top==%ld */ NULL,\n", (SXINT) list_top);
  fputs ("} /* End rhs_str_list */;\n", stdout);


  printf ("\n\n\
/* Si id est non nul, le mot est un lhs_acc de code id  */\n\
%sSXINT rhs_id_list [%ld] = {0,", is_static ? "static " : "", (SXINT) list_top+1);

  for (x = 1; x < top; x++) {
    y = rhs_hd.display [x].X;
    z = rhs_hd.display [x+1].X;
    printf ("\n/* [%ld..%ld] */ ", (SXINT) x, (SXINT) (z-1));
    while (y < z) {
      id = rhs_hd.list [y];
      ste = rhs_id2ste [id];
      string = sxstrget (ste);
      lgth = strlen (string);
      id = sxdfa_comb_seek_a_string (&lhs_acc_name_comb, string, &lgth);

      if (lgth != 0)
	id = 0;

      printf ("%ld, ", id);

      y++;
    } 
  }

  printf ("\n/* list_top==%ld */ 0,\n", (SXINT) list_top);
  fputs ("} /* End rhs_id_list */;\n", stdout);

  /*   F I N A L I S A T I O N S   */
  sxdfa_comb_free (&lhs_acc_name_comb);

  sxfree (ste2rhs_id), ste2rhs_id = NULL;
  sxfree (rhs_id2ste), rhs_id2ste = NULL;
  XH_free (&rhs_hd);
  sxfree (word_stack), word_stack = NULL;
  XxY_free (&rule_hd);
  sxfree (lhs_id2first_visited), lhs_id2first_visited = NULL;
  DFREE_STACK (acc_name_stack);
  sxfree (acc_entrance_set), acc_entrance_set = NULL;
  sxfree (acc_exit_set), acc_exit_set = NULL;
}

int
amlgm_cmpd_smp (int what, struct sxtables *sxtables_ptr)
{
  switch (what) {
  case SXOPEN:
    sxtab_ptr = sxtables_ptr;
    smpopen (sxtables_ptr);
    break;
  case SXSEMPASS: /* For [sub-]tree evaluation during parsing */
    break;
  case SXACTION:
    if (!sxatcvar.atc_lv.abstract_tree_is_error_node && sxerrmngr.nbmess [2] == 0)
      smppass ();
    break;
  case SXCLOSE:
    break;
  default:
    break;
  }

  return 1;
}
