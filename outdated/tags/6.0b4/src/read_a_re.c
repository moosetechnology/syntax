/* ********************************************************************
   *  This program has been generated from re.at                      *
   *  on Fri Aug 24 11:49:08 2007                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */


static char ME [] = "read_a_re";


/*   I N C L U D E S   */
/* Les tables des analyseurs du langage re */
#define NODE struct re_node

#define sxparser sxparser_re_tcut
#define sxtables re_tables
#define SCANACT  sxvoid
#define PARSACT  sxvoid
#define sempass  re_smp

#include "sxcommon.h"
#include "re_t.h"
#include "fsa.h"
#include "varstr.h"

char WHAT_READARE[] = "@(#)SYNTAX - $Id: read_a_re.c 752 2007-08-10 16:06:52Z boullier $" WHAT_DEBUG;


struct re_node {
    SXNODE_HEADER_S VOID_NAME;

/*
your attribute declarations...
*/
/* Commun */
  SXINT ste;

/* CAS RE2NFA et RE2DFA */
  struct re_node *left_brother;

/* CAS RE2NFA */
  SXINT max_nb;

/* CAS RE2DFA */
  SXINT     node_id, cur_position;
  BOOLEAN nullable;
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ALTERNATIVE_S_n 2
#define KLEENE_PLUS_n 3
#define KLEENE_STAR_n 4
#define OPTION_n 5
#define RE_ROOT_n 6
#define SEQUENCE_S_n 7
#define WORD_n 8
#define eof_n 9
/*
E N D   N O D E   N A M E S
*/


extern void             (*main_parser)(void); // earley eventuel ou autre ...


static void             (*prelude_re)(BOOLEAN, SXINT, SXINT, SXINT, SXINT), 
                        (*store_re)(SXINT, struct sxtoken **, SXINT, SXINT),
                        (*postlude_re)(SXINT);
static SXINT            what_to_do_re, eof_ste;
static BOOLEAN          not_a_dag;
static VARSTR           vstr;

/* *********************************************** CAS RE2NFA *********************************** */
static struct sxtoken *re2nfa_token_ptr_stack [3];
static SXINT            re_node_nb, re_leaf_nb;

static SXINT
first_re2nfa_pass (struct re_node *visited)
{
  struct re_node *son;
  SXINT            max_nb, son_nb, ste;

  sxinitialise (max_nb);

  switch (visited->name) {
  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    re_node_nb++;
    son_nb = max_nb = 0;

    for (son = visited->son; son != NULL; son = son->brother) {
      max_nb += first_re2nfa_pass (son);
      son_nb++;
    }

    max_nb -= (son_nb-1);

    break;

  case KLEENE_PLUS_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
  case KLEENE_STAR_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    if (what_to_do_re & RE_IS_A_DAG) {
      /* Le source doit etre un DAG, KLEENE est donc interdit */
      not_a_dag = TRUE;
      sxput_error (visited->token.source_index,
		   "%sThe regular expression must define a DAG.",
		   re_tables.err_titles [2]);
      /* On continue en sequence */
    }

    re_node_nb++;
    max_nb = first_re2nfa_pass (visited->son)+1; /* pour la transition vide de pre_pos a pre_pos+1 */
    break;

  case OPTION_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    re_node_nb++;
    max_nb = first_re2nfa_pass (visited->son);
    break;

  case RE_ROOT_n :/* son[1]->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    re_node_nb++;
    max_nb = first_re2nfa_pass (visited->son);
    /* son[2]->name = eof_n */
    max_nb += first_re2nfa_pass (visited->son->brother);
    break;

  case SEQUENCE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    re_node_nb++;
    max_nb = 0;

    for (son = visited->son; son != NULL; son = son->brother)
      max_nb += first_re2nfa_pass (son);

    break;

  case WORD_n :
    re_node_nb++;
    re_leaf_nb++;
    max_nb = 1;

    /* On quote les terminaux uniquement si on a demande' RE_USE_COMMENTS */
    ste = visited->token.string_table_entry;

    if (what_to_do_re & RE_USE_COMMENTS) {
      varstr_raz (vstr);

      if (visited->token.comment) {
	/* On fabrique un nouveau ste qui est la concat avec comment */
	vstr = varstr_catenate (varstr_catenate (vstr, visited->token.comment), " ");
      }

      vstr = varstr_catenate (varstr_lcatenate_with_escape (varstr_catenate (vstr, "\""), sxstrget (ste), sxstrlen (ste), "\"\\"), "\"");
      ste = sxstrsave (varstr_tostr (vstr));
    }

    visited->ste = ste;

    break;

  case eof_n :
    re_node_nb++;
    re_leaf_nb++;
    max_nb = 1;
    varstr_raz (vstr);
    if (what_to_do_re & RE_USE_COMMENTS) {

      if (visited->token.comment) {
	/* On fabrique un nouveau ste qui est la concat avec comment */
	vstr = varstr_catenate (varstr_catenate (vstr, visited->token.comment), " ");
      }

      vstr = varstr_catenate (vstr, "\"_End_Of_Sentence_\"");
    }
    else
      vstr = varstr_catenate (vstr, "_End_Of_Sentence_");

    visited->ste = eof_ste = sxstrsave (varstr_tostr (vstr));

    break;

  default:
    sxtrap (ME, "first_re2nfa_pass");
    break;
  }

  return visited->max_nb = max_nb;
}

static SXINT
second_re2nfa_pass (struct re_node *visited, SXINT pre_pos, SXINT cur_pos, SXINT post_pos)
{
  struct re_node *son;
  SXINT            ret_cur_pos, arg_post_pos;

  ret_cur_pos = cur_pos;

  switch (visited->name) {
  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    for (son = visited->son; son != NULL; son = son->brother) {
      ret_cur_pos = second_re2nfa_pass (son, pre_pos, ret_cur_pos, post_pos);
    }

    break;

  case KLEENE_PLUS_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    /* pre_pos ->epsilon pre_pos+1 */
    ret_cur_pos++;
    (*store_re)(pre_pos, NULL, 0, ret_cur_pos);
    /* post_pos ->epsilon pre_pos+1 */
    (*store_re)(post_pos, NULL, 0, ret_cur_pos);

    ret_cur_pos = second_re2nfa_pass (visited->son, ret_cur_pos, ret_cur_pos, post_pos);

    break;

  case KLEENE_STAR_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    /* pre_pos ->epsilon post_pospre_pos+1 */
    (*store_re)(pre_pos, NULL, 0, post_pos);
    /* post_pos ->epsilon pre_pos+1 */
    ret_cur_pos++;
    (*store_re)(post_pos, NULL, 0, ret_cur_pos);

    ret_cur_pos = second_re2nfa_pass (visited->son, ret_cur_pos, ret_cur_pos, post_pos);

    break;

  case OPTION_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    /* pre_pos ->epsilon post_pos */
    (*store_re)(pre_pos, NULL, 0, post_pos);

    ret_cur_pos = second_re2nfa_pass (visited->son, pre_pos, ret_cur_pos, post_pos);

    break;

  case RE_ROOT_n :/* son[1]->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    son = visited->son;
    arg_post_pos = post_pos;
    post_pos = ret_cur_pos + son->max_nb;
    second_re2nfa_pass (son, pre_pos, ret_cur_pos, post_pos);
    ret_cur_pos = pre_pos = post_pos;

    /* son[2]->name = eof_n */
    ret_cur_pos = second_re2nfa_pass (son->brother, pre_pos, ret_cur_pos, arg_post_pos);

    break;

  case SEQUENCE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    son = visited->son;

    /* ... au moins 2 fils */
    arg_post_pos = post_pos;

    while (son->brother != NULL) {
      post_pos = ret_cur_pos + son->max_nb;
      second_re2nfa_pass (son, pre_pos, ret_cur_pos, post_pos);
      ret_cur_pos = pre_pos = post_pos;
      son = son->brother;
    }

    ret_cur_pos = second_re2nfa_pass (son, pre_pos, ret_cur_pos, arg_post_pos);

    break;

  case WORD_n :
  case eof_n :
    /* pre_pos ->t post_pos */
    re2nfa_token_ptr_stack [1] = &(visited->token);
    (*store_re)(pre_pos, re2nfa_token_ptr_stack, visited->ste, post_pos);
    break;

  default:
    sxtrap (ME, "second_re2nfa_pass");
    break;
  }

  return ret_cur_pos;
}


/* *********************************************** CAS RE2DFA *********************************** */
#include "XH.h"
#include "sxstack.h"

static SXINT            position, node_nb, dfa_init_state, eof_position;
static SXBA           *firstpos_sets, *lastpos_sets, *followpos;
static struct re_node **position2visited;


static void
first_re2dfa_pass (struct re_node *visited)
{
  struct re_node *son;
  SXINT            ste;
  BOOLEAN        nullable = FALSE;

  switch (visited->name) {
  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    for (son = visited->son; son != NULL; son = son->brother) {
      first_re2dfa_pass (son);

      if (son->nullable)
	nullable = TRUE;
    }

    break;

  case KLEENE_PLUS_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
  case KLEENE_STAR_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    if (what_to_do_re & RE_IS_A_DAG) {
      /* Le source doit etre un DAG, KLEENE est donc interdit */
      not_a_dag = TRUE;
      sxput_error (visited->token.source_index,
		   "%sThe regular expression must define a DAG.",
		   re_tables.err_titles [2]);
      /* On continue en sequence */
    }
    /* On continue en sequence */

  case OPTION_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    first_re2dfa_pass (visited->son);
    nullable = visited->name == KLEENE_PLUS_n ? FALSE : TRUE;
    break;

  case RE_ROOT_n :/* son[1]->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    son = visited->son;
    first_re2dfa_pass (son);

    /* son[2]->name = eof_n */
    son = son->brother;
    first_re2dfa_pass (son);

    visited->cur_position = ++position; /* $ Fin de re */
    /* nullable = FALSE; $ */
    break;

  case SEQUENCE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    nullable = TRUE;

    for (son = visited->son; son != NULL; son = son->brother) {
      first_re2dfa_pass (son);

      if (!(son->nullable))
	nullable = FALSE;
    }

    break;

  case WORD_n :
    visited->cur_position = ++position;

    /* On quote les terminaux uniquement si on a demande' RE_USE_COMMENTS */
    ste = visited->token.string_table_entry;

    if (what_to_do_re & RE_USE_COMMENTS) {
      varstr_raz (vstr);

      if (visited->token.comment) {
	/* On fabrique un nouveau ste qui est la concat avec comment */
	vstr = varstr_catenate (varstr_catenate (vstr, visited->token.comment), " ");
      }

      vstr = varstr_catenate (varstr_lcatenate_with_escape (varstr_catenate (vstr, "\""), sxstrget (ste), sxstrlen (ste), "\"\\"), "\"");
      ste = sxstrsave (varstr_tostr (vstr));
    }

    visited->ste = ste;

    break;

  case eof_n :
    eof_position = visited->cur_position = ++position;
    varstr_raz (vstr);

    if (what_to_do_re & RE_USE_COMMENTS) {
      if (visited->token.comment) {
	/* On fabrique un nouveau ste qui est la concat avec comment */
	vstr = varstr_catenate (varstr_catenate (vstr, visited->token.comment), " ");
      }

      vstr = varstr_catenate (vstr, "\"_End_Of_Sentence_\"");
    }
    else
      vstr = varstr_catenate (vstr, "_End_Of_Sentence_");

    visited->ste = eof_ste = sxstrsave (varstr_tostr (vstr));

    break;

  default:
    sxtrap (ME, "first_re2dfa_pass");
    break;
  }

  visited->nullable = nullable;
  visited->node_id = ++node_nb/* le noeud courant */;
}

static void
second_re2dfa_pass (struct re_node *visited)
{
  struct re_node *son, *brother;
  SXBA           visited_firstpos_set, visited_lastpos_set, brother_firstpos_set;
  BOOLEAN        sons_are_nullable;

  switch (visited->name) {
  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    visited_firstpos_set = firstpos_sets [visited->node_id];
    visited_lastpos_set = lastpos_sets [visited->node_id];

    for (son = visited->son; son != NULL; son = son->brother) {
      second_re2dfa_pass (son);
      sxba_or (visited_firstpos_set, firstpos_sets [son->node_id]);
      sxba_or (visited_lastpos_set, lastpos_sets [son->node_id]);
    }

    break;

  case KLEENE_STAR_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
  case KLEENE_PLUS_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
  case OPTION_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    son = visited->son;
    second_re2dfa_pass (son);
    sxba_copy (firstpos_sets [visited->node_id], firstpos_sets [son->node_id]);
    sxba_copy (lastpos_sets [visited->node_id], lastpos_sets [son->node_id]);
    break;

  case RE_ROOT_n :/* son[1]->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} *//* visited->son->name = REGULAR_EXPRESSION_n */
    /* nullable = FALSE; $ */
    son = visited->son;
    second_re2dfa_pass (son);

    sxba_copy (firstpos_sets [visited->node_id], firstpos_sets [son->node_id]);

    if (son->nullable)
      SXBA_1_bit (firstpos_sets [visited->node_id], son->brother->cur_position);

    /* son[2]->name = eof_n */
    son = son->brother;
    second_re2dfa_pass (son);

    SXBA_1_bit (lastpos_sets [visited->node_id], visited->cur_position);

    break;

  case SEQUENCE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    visited_firstpos_set = firstpos_sets [visited->node_id];

    son = visited->son;
    /* 1er fils */
    son->left_brother = NULL;
    second_re2dfa_pass (son);
    sxba_copy (visited_firstpos_set, firstpos_sets [son->node_id]);
    sons_are_nullable = son->nullable;

    while ((brother = son->brother)) {
      brother->left_brother = son;
      son = brother;
      second_re2dfa_pass (son);

      if (sons_are_nullable) {
	sxba_or (visited_firstpos_set, firstpos_sets [son->node_id]);
	sons_are_nullable = son->nullable;
      }
    }

    visited_lastpos_set = lastpos_sets [visited->node_id];
    /* son est le dernier fils */
    /* On va aussi cumuler les first des freres droits si nullable */
    brother_firstpos_set = firstpos_sets [son->node_id];
    sxba_copy (visited_lastpos_set, lastpos_sets [son->node_id]);
    sons_are_nullable = son->nullable; /* L'ensemble des freres droits d'une position */
    
    while ((brother = son->left_brother)) {
      son = brother;

      if (sons_are_nullable) {
	sxba_or (visited_lastpos_set, lastpos_sets [son->node_id]);
	sons_are_nullable = son->nullable;
      }

      if (son->left_brother) {
	/* inutile sur le 1er fils de gauche */
	if (son->nullable) {
	  sxba_or (firstpos_sets [son->node_id], brother_firstpos_set);
	}

	brother_firstpos_set = firstpos_sets [son->node_id];
      }
    }

    break;

  case WORD_n :
  case eof_n :
    SXBA_1_bit (firstpos_sets [visited->node_id], visited->cur_position);
    SXBA_1_bit (lastpos_sets [visited->node_id], visited->cur_position);
    position2visited [visited->cur_position] = visited;
    break;

  default:
    sxtrap (ME, "second_re2dfa_pass");
    break;
  }
}



static void
third_re2dfa_pass (struct re_node *visited)
{
  SXINT            pos;
  struct re_node *son, *brother;
  SXBA           lastpos_set, firstpos_set;

  switch (visited->name) {
  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    for (son = visited->son; son != NULL; son = son->brother) {
      third_re2dfa_pass (son);
    }

    break;

  case KLEENE_PLUS_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
  case KLEENE_STAR_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    third_re2dfa_pass (visited->son);
    /* Toutes les positions de firstpos sont des followpos des positions de lastpos */
    lastpos_set = lastpos_sets [visited->node_id];
    firstpos_set = firstpos_sets [visited->node_id];

    pos = 0;
    
    while ((pos = sxba_scan (lastpos_set, pos)) > 0) {
      sxba_or (followpos [pos], firstpos_set);
    }

    break;

  case OPTION_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    third_re2dfa_pass (visited->son);
    break;

  case RE_ROOT_n :/* son[1]->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    /* son[2]->name = eof_n */
    /* nullable = FALSE; $ */
    son = visited->son;
    third_re2dfa_pass (son);

    brother = son->brother;
	
    /* le cur_position de brother est un followpos des lastpos de son */
    lastpos_set = lastpos_sets [son->node_id];

    pos = 0;

    while ((pos = sxba_scan (lastpos_set, pos)) > 0) {
      SXBA_1_bit (followpos [pos], brother->cur_position);
    }

    third_re2dfa_pass (brother);

    /* On ajoute $ aux followset des lastpos de eof */
    lastpos_set = lastpos_sets [brother->node_id];

    pos = 0;

    while ((pos = sxba_scan (lastpos_set, pos)) > 0) {
      SXBA_1_bit (followpos [pos], visited->cur_position);
    }

    break;

  case SEQUENCE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_n} */
    for (son = visited->son; son != NULL; son = brother) {
      third_re2dfa_pass (son);

      if ((brother = son->brother)) {
	/* les firstpos de brother sont des followpos des lastpos de son */
	lastpos_set = lastpos_sets [son->node_id];
	firstpos_set = firstpos_sets [brother->node_id];

	pos = 0;

	while ((pos = sxba_scan (lastpos_set, pos)) > 0) {
	  sxba_or (followpos [pos], firstpos_set);
	}
      }
    }

    break;

  case WORD_n :
    break;

  case eof_n :
    break;

  default:
    sxtrap (ME, "third_re2dfa_pass");
    break;
  }
}



/* On extrait un epsilon-free nfa */
static void
extract_efnfa (void)
{
  SXINT            pos, fpos, efnfa_state, prev_efnfa_state, next_efnfa_state, ste, init_state_component_nb, final_efnfa_state, x, y;
  SXINT            *pos_stack, *final_pos_stack;
  SXBA           firstpos_set, followpos_set;
  struct re_node *visited;
  struct sxtoken *local_re2nfa_token_ptr_stack [3] = {NULL, NULL, NULL};
  SXINT            *pos2efnfa_state;
  BOOLEAN        has_init_component;

  pos2efnfa_state = (SXINT *) sxcalloc (position+1, sizeof (SXINT));
  pos_stack = (SXINT *) sxalloc (2*position+1, sizeof (SXINT)), RAZ (pos_stack);
  final_pos_stack = (SXINT *) sxalloc (position+1, sizeof (SXINT)), RAZ (final_pos_stack);

  /* L'etat initial est forme' des firstpos de la racine */
  firstpos_set = firstpos_sets [sxatcvar.atc_lv.abstract_tree_root->node_id];

  /* Assure que les transitions avant conduisent a des etats croissants */
  init_state_component_nb = 0;
  efnfa_state = 1;

  /* l'etat final recevra son id + tard */ 
  for (pos = 1; pos < position; pos++) {
    if (SXBA_bit_is_set (firstpos_set, pos)) {
      init_state_component_nb++;
      PUSH (pos_stack, pos);
    }
  }

  for (pos = 1; pos < position; pos++) {
    if (!SXBA_bit_is_set (firstpos_set, pos)) {
      pos2efnfa_state [pos] = ++efnfa_state;
      PUSH (pos_stack, pos);
    }
  }

  for (x = 1; x <= TOP (pos_stack); x++) {
    pos = pos_stack [x];

    if (x <= init_state_component_nb)
      /* Comme (composant de l'etat initial) */
      prev_efnfa_state = 1;
    else
      prev_efnfa_state = pos2efnfa_state [pos];

#if EBUG
    if (prev_efnfa_state == 0)
      sxtrap (ME, "extract_efnfa");
#endif /* EBUG */

    visited = position2visited [pos];
    local_re2nfa_token_ptr_stack [1] = &(visited->token);
    ste = visited->ste;
    followpos_set = followpos [pos];
    
    has_init_component = FALSE;
    next_efnfa_state = 0;
    fpos = 0;

    while ((fpos = sxba_scan (followpos_set, fpos)) > 0) {
      if (fpos < position) {
	if (SXBA_bit_is_set (firstpos_set, fpos)) {
	  has_init_component = TRUE;
	}
	else {
	  next_efnfa_state = pos2efnfa_state [fpos];
	  (*store_re)(prev_efnfa_state, local_re2nfa_token_ptr_stack, ste, next_efnfa_state);
	}
      }
      else {
	/* transition vers l'etat final qui est retardee */
	PUSH (final_pos_stack, x);
      }
    }

    if (next_efnfa_state == 0) {
      /* followpos_set est inclus ds firstpos_set */
      if (sxba_first_difference (followpos_set, firstpos_set) == -1)
	/* followpos_set == firstpos_set */
	(*store_re)(prev_efnfa_state, local_re2nfa_token_ptr_stack, ste, 1);
      else
	next_efnfa_state = 1;
    }

    if (next_efnfa_state && has_init_component) {
      fpos = 0;

      while ((fpos = sxba_scan (followpos_set, fpos)) > 0) {
	if (SXBA_bit_is_set (firstpos_set, fpos)) {
	  next_efnfa_state = pos2efnfa_state [fpos];

	  if (next_efnfa_state == 0) {
	    next_efnfa_state = pos2efnfa_state [fpos] = ++efnfa_state;
	    PUSH (pos_stack, fpos);
	  }

	  (*store_re)(prev_efnfa_state, local_re2nfa_token_ptr_stack, ste, next_efnfa_state);
	}
      }
    }
  }

  final_efnfa_state = ++efnfa_state;
      
  for (y = 1; y <= TOP (final_pos_stack); y++) {
    x = final_pos_stack [y];
    pos = pos_stack [x];

    if (x <= init_state_component_nb)
      /* Comme (composant de l'etat initial) */
      prev_efnfa_state = 1;
    else
      prev_efnfa_state = pos2efnfa_state [pos];

#if EBUG
    if (prev_efnfa_state == 0)
      sxtrap (ME, "extract_efnfa");
#endif /* EBUG */

    visited = position2visited [pos];
    local_re2nfa_token_ptr_stack [1] = &(visited->token);
    ste = visited->ste;


    visited = position2visited [pos];
    local_re2nfa_token_ptr_stack [1] = &(visited->token);
    ste = visited->ste;
    (*store_re)(prev_efnfa_state, local_re2nfa_token_ptr_stack, ste, final_efnfa_state);
  }

  sxfree (pos2efnfa_state);
  sxfree (final_pos_stack);
  sxfree (pos_stack);
}


#if 0
/* On extrait un epsilon-free nfa */
static void
extract_efnfa ()
{
  SXINT            pos, fpos, efnfa_state, prev_efnfa_state, next_efnfa_state, ste;
  SXBA           firstpos_set, followpos_set, ste_set;
  struct re_node *visited;
  struct sxtoken *re2nfa_token_ptr_stack [3] = {NULL, NULL, NULL};
  SXINT            *pos2efnfa_state;

  pos2efnfa_state = (SXINT *) sxcalloc (position+1, sizeof (SXINT));
  ste_set = sxba_calloc (SXSTRtop ()+1);

  /* L'etat initial est forme' des firstpos de la racine */
  firstpos_set = firstpos_sets [sxatcvar.atc_lv.abstract_tree_root->node_id];

  /* Assure que les transition avant conduisent a des etats croissants */
  efnfa_state = 1;

  for (pos = 1; pos <= position; pos++) {
    if (SXBA_bit_is_set (firstpos_set, pos))
      pos2efnfa_state [pos] = 1;
    else
      pos2efnfa_state [pos] = ++efnfa_state;
  }

  for (pos = 1; pos < position; pos++) {
    prev_efnfa_state = pos2efnfa_state [pos];

    visited = position2visited [pos];
    re2nfa_token_ptr_stack [1] = &(visited->token);
    ste = visited->ste;

    followpos_set = followpos [pos];
    fpos = 0;

    while ((fpos = sxba_scan (followpos_set, fpos)) > 0) {
      next_efnfa_state = pos2efnfa_state [fpos];

      if (prev_efnfa_state != 1 || next_efnfa_state != 1 || SXBA_bit_is_reset_set (ste_set, ste))
	(*store_re)(prev_efnfa_state, re2nfa_token_ptr_stack, ste, next_efnfa_state);
    }
  }

  sxfree (pos2efnfa_state);
  sxfree (ste_set);
}
#endif /* 0 */

static XH_header      XH_dfa;

static int
compare (const void *pi, const void *pj)
{
  SXINT i, j, boti, curi, topi, botj, curj, topj, posi, posj;

  i = (SXINT) * (SXINT * const *) pi;
  j = (SXINT) * (SXINT * const *) pj;

  boti = XH_X (XH_dfa, i);
  topi = XH_X (XH_dfa, i+1);

  botj = XH_X (XH_dfa, j);
  topj = XH_X (XH_dfa, j+1);

  for (curi = boti, curj = botj; curi < topi && curj < topj; curi++, curj++) {
    posi = XH_list_elem (XH_dfa, curi);
    posj = XH_list_elem (XH_dfa, curj);

    if (posi != posj)
      return posi < posj ? -1 : 1;
  }

  return curj < topj ? -1 : 1;
}

#include "XxY.h"
static SXINT            *XxY_dfa2next_state;
static SXBA           *XxY_dfa2pos_set;

static void
XxY_dfa_oflw (SXINT old_size, SXINT new_size)
{
  XxY_dfa2next_state = (SXINT*) sxrealloc (XxY_dfa2next_state, new_size+1, sizeof (SXINT));
  XxY_dfa2pos_set = sxbm_resize (XxY_dfa2pos_set, old_size+1, new_size+1, node_nb+1);
}

/* On extrait directement un dfa */
static void
extract_dfa (void)
{
  SXINT            dfa_state, bot, top, pos, ste, next_dfa_state, x, final_dfa_state;
  SXBA           followpos_set, pos_set, firstpos_set;
  struct sxtoken **re2dfa_token_ptr_stack;
  SXINT            *dfa_state_stack, *final_trans_stack;
  SXBA           *ste2followpos_sets, *ste2pos_sets, ste_set;

  SXINT            i, j, couple;
  SXINT            *to_be_sorted, *dfa_state2i;
  XxY_header     XxY_dfa;
  BOOLEAN        already_found;
  
  re2dfa_token_ptr_stack = (struct sxtoken **) sxalloc (position+2, sizeof (struct sxtoken *)), re2dfa_token_ptr_stack [0] = 0;
  XH_alloc (&XH_dfa, "XH_dfa", node_nb+1, 1, position, NULL, NULL);
  DALLOC_STACK (dfa_state_stack, node_nb+1);
  ste_set = sxba_calloc (SXSTRtop ()+1);
  ste2followpos_sets = sxbm_calloc (SXSTRtop ()+1, position+1);
  ste2pos_sets = sxbm_calloc (SXSTRtop ()+1, node_nb+1);
  DALLOC_STACK (final_trans_stack, node_nb+1);

  if (what_to_do_re & RE_INCREASE_ORDER) {
    XxY_alloc (&XxY_dfa, "XxY_dfa", node_nb+1, 1, 1, 0, XxY_dfa_oflw, NULL);
    XxY_dfa2next_state = (SXINT*) sxalloc (XxY_size (XxY_dfa)+1, sizeof (SXINT));
    XxY_dfa2pos_set = sxbm_calloc (XxY_size (XxY_dfa)+1, node_nb+1);
  }

  /* L'etat initial est forme' des firstpos de la racine */
  firstpos_set = firstpos_sets [sxatcvar.atc_lv.abstract_tree_root->node_id];

  pos = 0;
  
  while ((pos = sxba_scan (firstpos_set, pos)) > 0) {
    XH_push (XH_dfa, pos);
  }

  XH_set (&XH_dfa, &dfa_init_state);

#if EBUG
  if (SXBA_bit_is_set (firstpos_set, position))
    /* Impossible avec <EOF> */
    sxtrap (ME, "extract_dfa");
#endif /* EBUG */

  DPUSH (dfa_state_stack, dfa_init_state);

  while (!IS_EMPTY (dfa_state_stack)) {
    dfa_state = DPOP (dfa_state_stack);

    bot = XH_X (XH_dfa, dfa_state);
    top = XH_X (XH_dfa, dfa_state+1)-1;

    if (XH_list_elem (XH_dfa, top) == position)
      top--;

    if (bot <= top) {
      do {
	pos = XH_list_elem (XH_dfa, bot);
	ste = position2visited [pos]->ste;
	followpos_set = followpos [pos];

	if (SXBA_bit_is_reset_set (ste_set, ste)) {
	  sxba_empty (ste2pos_sets [ste]);
	}

	sxba_or (ste2followpos_sets [ste], followpos_set);
	SXBA_1_bit (ste2pos_sets [ste], pos);

	bot++;
      } while (bot <= top);

      ste = EMPTY_STE;

      while ((ste = sxba_scan_reset (ste_set, ste)) > 0) {
	followpos_set = ste2followpos_sets [ste];

	if (SXBA_bit_is_set_reset (followpos_set, position)) {
	  /* Etat final */
	  /* On differe la sortie pour avoir l'etat max */
	  DPUSH (final_trans_stack, dfa_state);
	}
	else {
	  pos = 0;

	  while ((pos = sxba_scan_reset (followpos_set, pos)) > 0)
	    XH_push (XH_dfa, pos);

	  if (!XH_set (&XH_dfa, &next_dfa_state))
	    DPUSH (dfa_state_stack, next_dfa_state);

	  pos_set = ste2pos_sets [ste];

	  /* Ici on a une transition de dfa_state sur ste vers next_dfa_state */
	  if (what_to_do_re & RE_INCREASE_ORDER) {
	    already_found = XxY_set (&XxY_dfa, dfa_state, ste, &couple);

#if EBUG
	    if (already_found)
	      sxtrap (ME, "extract_dfa");
#endif /* EBUG */

	    XxY_dfa2next_state [couple] = next_dfa_state;
	    sxba_copy (XxY_dfa2pos_set [couple], pos_set);
	  }
	  else {
	    pos = 0;
	    x = 0;

	    while ((pos = sxba_scan_reset (pos_set, pos)) > 0) {
	      re2dfa_token_ptr_stack [++x] = &(position2visited [pos]->token);
	    }

	    re2dfa_token_ptr_stack [++x] = NULL; /* fin de la liste */

	    (*store_re)(dfa_state, re2dfa_token_ptr_stack, ste, next_dfa_state);
	  }
	}
      }
    }
  }

  /* On fabrique l'etat final ... */
  XH_push (XH_dfa, position);
  XH_set (&XH_dfa, &final_dfa_state);

  if (what_to_do_re & RE_INCREASE_ORDER) {
    /* On trie les etats du fsa */
    /*
SYNOPSIS
       #include <stdlib.h>

       void qsort(void *base, size_t nmemb, size_t size,
                  int(*compar)(const void *, const void *));

DESCRIPTION
       The  qsort()  function sorts an array with nmemb elements of size size.
       The base argument points to the start of the array.

       The contents of the array are sorted in ascending order according to  a
       comparison  function  pointed  to  by  compar, which is called with two
       arguments that point to the objects being compared.

       The comparison function must return an integer less than, equal to,  or
       greater  than  zero  if  the first argument is considered to be respec-
       tively less than, equal to, or greater than the second.  If two members
       compare as equal, their order in the sorted array is undefined.
    */
#include <stdlib.h>
    to_be_sorted = (SXINT *) sxalloc (final_dfa_state+1, sizeof (SXINT));
    dfa_state2i = (SXINT *) sxalloc (final_dfa_state+1, sizeof (SXINT));

    for (i = 1; i <= final_dfa_state; i++)
      to_be_sorted [i] = i;

    qsort ((void *) (to_be_sorted+1), final_dfa_state, sizeof (SXINT), compare);

    for (i = 1; i <= final_dfa_state; i++)
      dfa_state2i [to_be_sorted [i]] = i;
    
    for (i = 1; i <= final_dfa_state; i++) {
      dfa_state = to_be_sorted [i];

      XxY_Xforeach (XxY_dfa, dfa_state, couple) {
	ste = XxY_Y (XxY_dfa, couple);
	next_dfa_state = XxY_dfa2next_state [couple];
	pos_set = XxY_dfa2pos_set [couple];
	j = dfa_state2i [next_dfa_state];

	/* Ici on a une transition de i sur ste vers j */

	pos = 0;
	x = 0;

	while ((pos = sxba_scan (pos_set, pos)) > 0) {
	  re2dfa_token_ptr_stack [++x] = &(position2visited [pos]->token);
	}

	re2dfa_token_ptr_stack [++x] = NULL; /* fin de la liste */

	(*store_re)(i, re2dfa_token_ptr_stack, ste, j);
      }
    }

    sxfree (to_be_sorted);

    XxY_free (&XxY_dfa);
    sxfree (XxY_dfa2next_state), XxY_dfa2next_state = NULL;
    sxbm_free (XxY_dfa2pos_set), XxY_dfa2pos_set = NULL;
  }
  else
    dfa_state2i = NULL;
  

  /* ... et finalement on sort toutes les transitions vers l'etat final */
  re2dfa_token_ptr_stack [1] = &(position2visited [eof_position]->token);
  re2dfa_token_ptr_stack [2] = NULL; /* fin de la liste */

  top = DTOP (final_trans_stack);

  for (x = 1; x <= top; x++) {
    dfa_state = final_trans_stack [x];

    if (dfa_state2i)
      dfa_state = dfa_state2i [dfa_state];

    (*store_re)(dfa_state, re2dfa_token_ptr_stack, eof_ste, final_dfa_state);
  }
  
  sxfree (re2dfa_token_ptr_stack);
  XH_free (&XH_dfa);
  DFREE_STACK (dfa_state_stack);
  sxbm_free (ste2pos_sets);
  sxbm_free (ste2followpos_sets);
  sxfree (ste_set);
  DFREE_STACK (final_trans_stack);

  if (dfa_state2i) 
    sxfree (dfa_state2i);
}


/* *************************************************************************************************** */

static void
smpopen (struct sxtables *sxtables_ptr)
{
  sxuse (sxtables_ptr);
  sxatcvar.atc_lv.node_size = sizeof (struct re_node);
}



static void
smppass (void)
{
  /*   I N I T I A L I S A T I O N S   */
  /* ........... */

  /*   A T T R I B U T E S    E V A L U A T I O N   */

  vstr = varstr_alloc (64);

  if (what_to_do_re & RE2NFA) {
    /* Cas re2nfa */
    /* Calcule, a partir d'une expression reguliere, l'automate non-deterministe associe' (avec transitions epsilon) */
    /* premiere passe synthetisee qui calcule max_nb la "longueur" max de la sous-expression */
    first_re2nfa_pass (sxatcvar.atc_lv.abstract_tree_root);

    (*prelude_re) (!not_a_dag /* RE OK */, eof_ste, re_node_nb, re_leaf_nb, RE2NFA);

    if (!not_a_dag) {
      /* seconde passe heritee qui calcule les pre et post pos de la sous-expression */
      second_re2nfa_pass (sxatcvar.atc_lv.abstract_tree_root, 1, 1, 1+sxatcvar.atc_lv.abstract_tree_root->max_nb);
      (*postlude_re)(RE2NFA);
    }
  }

  if (!not_a_dag && (what_to_do_re & (RE2EFNFA | RE2DFA))) {
    /* cas re2dfa */
    /* Calcule, a partir d'une expression reguliere, l'automate non-deterministe associe' sans transitions epsilon, ou l'automate deterministe associe' (non minimal) */
    node_nb = position = 0;
    first_re2dfa_pass (sxatcvar.atc_lv.abstract_tree_root);

    if (not_a_dag)
      (*prelude_re) (FALSE /* RE pas OK */, eof_ste, node_nb, position, 0);
    else {
      firstpos_sets = sxbm_calloc (node_nb+1, position+1);
      lastpos_sets = sxbm_calloc (node_nb+1, position+1);
      position2visited = (struct re_node **) sxalloc (position+1, sizeof (struct re_node *));

      second_re2dfa_pass (sxatcvar.atc_lv.abstract_tree_root);

      followpos = sxbm_calloc (position+1, position+1);

      third_re2dfa_pass (sxatcvar.atc_lv.abstract_tree_root);

      sxbm_free (lastpos_sets), lastpos_sets = NULL;

      if (what_to_do_re & RE2EFNFA) {
	/* Cas automate non-deterministe sans transitions epsilon */
	(*prelude_re) (TRUE /* RE OK */, eof_ste, node_nb, position, RE2EFNFA);
	extract_efnfa ();
	(*postlude_re)(RE2EFNFA);
      }

      if (what_to_do_re & RE2DFA) {
	/* Cas automate deterministe non minimal */
	(*prelude_re) (TRUE /* RE OK */, eof_ste, node_nb, position, RE2DFA);
	extract_dfa ();
	(*postlude_re)(RE2DFA);
      }

      sxbm_free (firstpos_sets), firstpos_sets = NULL;
      sxbm_free (followpos), followpos = NULL;
      sxfree (position2visited), position2visited = NULL;
    }
  }

  /*   F I N A L I S A T I O N S   */
  /* ........... */
  
  varstr_free (vstr);
}



SXINT
re_smp (SXINT what, struct sxtables *sxtables_ptr)
{
  sxuse(sxtables_ptr); /* pour faire taire gcc -Wunused */

  switch (what) {
  case OPEN:
    smpopen (sxtables_ptr);
    break;
  case SEMPASS: /* For [sub-]tree evaluation during parsing */
    break;
  case ACTION:
    if (!sxatcvar.atc_lv.abstract_tree_is_error_node)
      smppass ();
    break;
  case CLOSE:
    break;
  default:
    break;
  }
  return 1;
}

int
read_a_re (void (*prelude)(BOOLEAN, SXINT, SXINT, SXINT, SXINT), 
	   void (*store)(SXINT, struct sxtoken **, SXINT, SXINT), 
	   void (*postlude)(SXINT), 
	   SXINT what_to_do)
{
  /* On suppose que source_file est non NULL */
  /* Si la lecture se fait sur stdin, l'appelant l'a recopie' ds un fichier temporaire */
  int	severity;

  prelude_re = prelude;
  store_re = store;
  postlude_re = postlude;

  what_to_do_re = what_to_do;

  /* sxstr_mngr (BEGIN); Fait depuis l'exterieur */
  /* sxsrc_mngr (INIT, source_file, source_file_name); Fait depuis l'exterieur */
  /* sxerr_mngr (BEGIN); Fait depuis l'exterieur */

  (*(re_tables.analyzers.parser)) (BEGIN, &re_tables) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &re_tables) /* Initialisation de SYNTAX */ ;

  syntax (ACTION, &re_tables); /* Appel de l'"analyseur" sxparser_re_tcut */

  for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
    ;

  syntax (CLOSE, &re_tables);
  (*(re_tables.analyzers.parser)) (END, &re_tables);

  /* sxerr_mngr (END); Fait a` l'exterieur */
  /* sxstr_mngr (END); Fait a` l'exterieur */
  /* sxsrc_mngr (FINAL); Fait ds re_reader */

  return severity;
}

/* laid il faudrait passer par un tdef */
#define eol_code 1

#undef sxparser
/* Le vrai parseur qui traite les RE est le parseur deterministe de SYNTAX */
extern SXINT sxparser  (SXINT	what_to_do, struct sxtables *arg);

static SXINT sxvoid_parser  (SXINT what, ...)
{
  sxuse (what);
  return 0;
}

/* C'est le parseur de re_tables.analyzers.parser */
SXINT
sxparser_re_tcut  (SXINT what_to_do, struct sxtables *arg)
{
  BOOLEAN        ret_val;
  SXINT            lahead;
  struct sxtoken *ptoken;
  SXINT            tmax;
  struct sxsvar  sxsvar_saved;
  struct sxtoken **sxplocals_toks_buf_saved;
  SXINT            (*semact_saved) (SXINT what, ...) = NULL;

  switch (what_to_do) {
  case ACTION:
    /* on fait un "TCUT" sur les fins-de-ligne */
    tmax = -re_tables.SXP_tables.P_tmax;
    ret_val = TRUE;

    do {
      /* Les appels du scanner se font manuellement */
      sxerr_mngr (INIT);
      do {
	(*(sxplocals.SXP_tables.scanit)) ();
	ptoken = &SXGET_TOKEN (sxplocals.Mtok_no);
	lahead = ptoken->lahead;
      } while (lahead != -tmax && lahead != eol_code);

      /* n = sxplocals.Mtok_no - 1;*/

      if (sxplocals.Mtok_no > 1) { /* on n'a pas trouvé une ligne vide, i.e. dont le eol est en position 1 */
	sxput_token (*ptoken); /* Eof */

	if (lahead == -tmax) {
	  /* On a trouve' la fin de fichier sans eol_code devant, on le simule */
	  ptoken->lahead = eol_code; /* on change le precedent */
	}
	else {
	  /* lahead == eol_code */
	  /* On force EOF derriere eol */
	    SXGET_TOKEN (sxplocals.Mtok_no).lahead = -tmax /* c'est-à-dire EOF */;
	}
	
	ret_val &= sxparser (what_to_do, arg); /* On parse la RE courante et on construit l'arbre abstrait ... */

	(*(re_tables.SXP_tables.semact)) (SEMPASS, arg); /* ... On exploite l'arbre abstrait et on appelle prelude_re, store_re et postlude_re  */

	sxsvar_saved = sxsvar; // car ce qui suit fait appel à un "autre" scanner si #if no_dico
	sxplocals_toks_buf_saved = sxplocals.toks_buf;

	if (main_parser)
	  (*main_parser) (); // earley, ou un autre...

	sxsvar = sxsvar_saved; // on restore le scanner de RE
	{
	  // et on le prépare à parser un nouveau RE
	  /* (équiv. à (*(re_tables.analyzers.scanner)) (INIT, arg) sans le sxnext_char final, car déjà fait en lisant le eol */
	  SXINT i;
	    
	  sxsvar.sxlv_s.include_no = 0;

	  if (sxsvar.sxlv_s.counters != NULL) {
	    for (i = sxsvar.SXS_tables.S_counters_size - 1; i >= 0; i--)
	      sxsvar.sxlv_s.counters[i] = 0;
	  }
	}

	sxplocals.toks_buf = sxplocals_toks_buf_saved;
	sxstr_mngr (BEGIN); // vidange du sxstr_mngr
      }

      sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;
    } while (lahead != -tmax);

    semact_saved = re_tables.SXP_tables.semact;
    re_tables.SXP_tables.semact = sxvoid_parser; // pour ne pas faire 2 fois semact (i.e. re_smp) sur le dernier re
      
    break;
  case CLOSE:
    re_tables.SXP_tables.semact = semact_saved; // prudence, on restore le semact initial après l'avoir sxvoidé
    /* pas de break */
  default:
    sxparser (what_to_do, arg);
    break;
  }

  return TRUE;
}


/* Point d'entree principal, si from_file, le source est dans un fichier de nom pathname_or_string (NULL si stdin) 
                             sinon, le source est ds la chaine C pathname_or_string */
/* re_process est la fonction de l'utilisateur qui appelle read_a_re () avec les bons parametres */
int
re_reader (char *pathname_or_string, BOOLEAN from_file, int (*re_process)(void))
{
  FILE	*infile = NULL;
  int   severity;

  if (from_file) {
    if (pathname_or_string == NULL) {
      int	c;

      if (sxverbosep) {
	fputs ("\"stdin\":\n", sxtty);
      }

      if ((infile = sxtmpfile ()) == NULL) {
	fprintf (sxstderr, "%s: Unable to create ", ME);
	sxperror ("temp file");
	sxerrmngr.nbmess [2]++;
	return 2;
      }

      while ((c = getchar ()) != EOF) {
	putc (c, infile);
      }

      rewind (infile);
      sxsrc_mngr (INIT, infile, "");
    }
    else {
      if ((infile = sxfopen (pathname_or_string, "r")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	sxperror (pathname_or_string);
	sxerrmngr.nbmess [2]++;
	return 2;
      }
      else {
	if (sxverbosep) {
	  fprintf (sxtty, "%s:\n", pathname_or_string);
	}

	sxsrc_mngr (INIT, infile, pathname_or_string);
      }
    }
  }
  else
    /* Le source est une chaine C */
    sxsrc_mngr (INIT, NULL, pathname_or_string);

  /* re_process est la fonction de l'utilisateur qui appelle read_a_re () avec les bons parametres */
  severity = (*re_process) ();

  if (from_file)
    fclose (infile);

  sxsrc_mngr (FINAL);

  return severity;
}

