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
   *  This program has been generated from re.at                      *
   *  on Fri Sep 19 10:33:12 2008                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */

static char ME [] = "read_a_re";

/*   I N C L U D E S   */
/* Les tables des analyseurs du langage re */
#define SXNODE struct re_node

#define sxparser sxparser_re_tcut
#define sxtables re_tables
#define SCANACT  sxjvoid
#define PARSACT  sxjvoid
#define SEMPASS  re_smp

#include "sxversion.h"
#include "sxcommon.h"
#include "re_t.h"
#include "udag_scanner.h"
#include "earley.h"
#include "fsa.h"
#include "varstr.h"
#include <setjmp.h>

char WHAT_READARE[] = "@(#)SYNTAX - $Id: read_a_re.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

extern VARSTR              cur_input_vstr;

struct re_node {
    SXNODE_HEADER_S SXVOID_NAME;

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
  bool nullable;
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
#define SEMLEX_n 7
#define SEQUENCE_S_n 8
#define WORD_SEMLEX_n 9
#define eof_n 10
#define word_n 11
/*
E N D   N O D E   N A M E S
*/


extern void             (*main_parser)(SXINT what); // earley eventuel ou autre ...


static void             (*prelude_re)(bool, SXINT, SXINT, SXINT, SXINT), 
                        (*store_re)(SXINT, struct sxtoken **, struct sxtoken **, SXINT, SXINT);
static SXINT            (*postlude_re)(SXINT);

static SXINT            what_to_do_re;
static SXINT            eof_ste;
static bool        not_a_dag;
extern bool        tmp_file_for_stdin;
static VARSTR           vstr;

/* *********************************************** CAS RE2NFA *********************************** */
static struct sxtoken *re2nfa_token_ptr_stack [3];
static struct sxtoken *re2nfa_semlex_token_ptr_stack [3];
static SXINT            re_node_nb, re_leaf_nb;

static SXINT
first_re2nfa_pass (struct re_node *visited)
{
  struct re_node *son;
  SXINT  max_nb, son_nb;
  SXINT ste;

  sxinitialise (max_nb);

  switch (visited->name) {
  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    re_node_nb++;
    son_nb = max_nb = 0;

    for (son = visited->son; son != NULL; son = son->brother) {
      max_nb += first_re2nfa_pass (son);
      son_nb++;
    }

    max_nb -= (son_nb-1);

    break;

  case KLEENE_PLUS_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
  case KLEENE_STAR_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    if (what_to_do_re & RE_IS_A_DAG) {
      /* Le source doit etre un DAG, KLEENE est donc interdit */
      not_a_dag = true;
      sxerror (visited->token.source_index,
	       re_tables.err_titles [2][0],
	       "%sThe regular expression must define a DAG.",
	       re_tables.err_titles [2]+1);
      /* On continue en sequence */
    }

    re_node_nb++;
    max_nb = first_re2nfa_pass (visited->son)+1; /* pour la transition vide de pre_pos a pre_pos+1 */
    break;

  case OPTION_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    re_node_nb++;
    max_nb = first_re2nfa_pass (visited->son);
    break;

  case RE_ROOT_n :/* son[1]->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    re_node_nb++;
    max_nb = first_re2nfa_pass (visited->son);
    /* son[2]->name = eof_n */
    max_nb += first_re2nfa_pass (visited->son->brother);
    break;

  case SEQUENCE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    re_node_nb++;
    max_nb = 0;

    for (son = visited->son; son != NULL; son = son->brother)
      max_nb += first_re2nfa_pass (son);

    break;

  case word_n :
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

  case WORD_SEMLEX_n:
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

  case ALTERNATIVE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    for (son = visited->son; son != NULL; son = son->brother) {
      ret_cur_pos = second_re2nfa_pass (son, pre_pos, ret_cur_pos, post_pos);
    }

    break;

  case KLEENE_PLUS_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    /* pre_pos ->epsilon pre_pos+1 */
    ret_cur_pos++;
    (*store_re)(pre_pos, NULL, NULL, 0, ret_cur_pos);
    /* post_pos ->epsilon pre_pos+1 */
    (*store_re)(post_pos, NULL, NULL, 0, ret_cur_pos);

    ret_cur_pos = second_re2nfa_pass (visited->son, ret_cur_pos, ret_cur_pos, post_pos);

    break;

  case KLEENE_STAR_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    /* pre_pos ->epsilon post_pospre_pos+1 */
    (*store_re)(pre_pos, NULL, NULL, 0, post_pos);
    /* post_pos ->epsilon pre_pos+1 */
    ret_cur_pos++;
    (*store_re)(post_pos, NULL, NULL, 0, ret_cur_pos);

    ret_cur_pos = second_re2nfa_pass (visited->son, ret_cur_pos, ret_cur_pos, post_pos);

    break;

  case OPTION_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    /* pre_pos ->epsilon post_pos */
    (*store_re)(pre_pos, NULL, NULL, 0, post_pos);

    ret_cur_pos = second_re2nfa_pass (visited->son, pre_pos, ret_cur_pos, post_pos);

    break;

  case RE_ROOT_n :/* son[1]->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    son = visited->son;
    arg_post_pos = post_pos;
    post_pos = ret_cur_pos + son->max_nb;
    second_re2nfa_pass (son, pre_pos, ret_cur_pos, post_pos);
    ret_cur_pos = pre_pos = post_pos;

    /* son[2]->name = eof_n */
    ret_cur_pos = second_re2nfa_pass (son->brother, pre_pos, ret_cur_pos, arg_post_pos);

    break;

  case SEQUENCE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
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

  case eof_n :
    re2nfa_token_ptr_stack [1] = &(visited->token);
    (*store_re)(pre_pos, re2nfa_token_ptr_stack, NULL, visited->ste, post_pos);
    break;

  case word_n :
    re2nfa_token_ptr_stack [1] = &(visited->token);
    if (visited->brother && visited->brother->name == SEMLEX_n) {
      re2nfa_semlex_token_ptr_stack [1] = &(visited->brother->token);
      (*store_re)(pre_pos, re2nfa_token_ptr_stack, re2nfa_semlex_token_ptr_stack, visited->ste, post_pos);
    }
    else
      (*store_re)(pre_pos, re2nfa_token_ptr_stack, NULL, visited->ste, post_pos);
    break;

  case WORD_SEMLEX_n:
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

extern SXINT  SEMLEX_lahead; /* defini ds dag_scanner */

static void
first_re2dfa_pass (struct re_node *visited)
{
  struct re_node   *son;
  SXINT           ste;
  bool        nullable = false;

  switch (visited->name) {
  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    for (son = visited->son; son != NULL; son = son->brother) {
      first_re2dfa_pass (son);

      if (son->nullable)
	nullable = true;
    }

    break;

  case KLEENE_PLUS_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
  case KLEENE_STAR_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    if (what_to_do_re & RE_IS_A_DAG) {
      /* Le source doit etre un DAG, KLEENE est donc interdit */
      not_a_dag = true;
      sxerror (visited->token.source_index,
		   re_tables.err_titles [2][0],
		   "%sThe regular expression must define a DAG.",
		   re_tables.err_titles [2]+1);
      /* On continue en sequence */
    }
    /* On continue en sequence */
    /* FALLTHROUGH */

  case OPTION_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    first_re2dfa_pass (visited->son);
    nullable = visited->name == KLEENE_PLUS_n ? false : true;
    break;

  case RE_ROOT_n :/* son[1]->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    son = visited->son;
    first_re2dfa_pass (son);

    /* son[2]->name = eof_n */
    son = son->brother;
    first_re2dfa_pass (son);

    visited->cur_position = ++position; /* $ Fin de re */
    /* nullable = false; $ */
    break;

  case SEQUENCE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    nullable = true;

    for (son = visited->son; son != NULL; son = son->brother) {
      first_re2dfa_pass (son);

      if (!(son->nullable))
	nullable = false;
    }

    break;

  case word_n :
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

  case WORD_SEMLEX_n:
    son = visited->son; /* word_n */
    first_re2dfa_pass (son);
    son = son->brother;
    first_re2dfa_pass (son);
    break;

  case SEMLEX_n:
    SEMLEX_lahead = visited->token.lahead; /* SEMLEX_n */
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
  bool        sons_are_nullable;

  switch (visited->name) {
  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    visited_firstpos_set = firstpos_sets [visited->node_id];
    visited_lastpos_set = lastpos_sets [visited->node_id];

    for (son = visited->son; son != NULL; son = son->brother) {
      second_re2dfa_pass (son);
      sxba_or (visited_firstpos_set, firstpos_sets [son->node_id]);
      sxba_or (visited_lastpos_set, lastpos_sets [son->node_id]);
    }

    break;

  case KLEENE_STAR_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
  case KLEENE_PLUS_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
  case OPTION_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    son = visited->son;
    second_re2dfa_pass (son);
    sxba_copy (firstpos_sets [visited->node_id], firstpos_sets [son->node_id]);
    sxba_copy (lastpos_sets [visited->node_id], lastpos_sets [son->node_id]);
    break;

  case RE_ROOT_n :/* son[1]->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} *//* visited->son->name = REGULAR_EXPRESSION_n */
    /* nullable = false; $ */
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

  case SEQUENCE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
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

  case word_n :
  case eof_n :
    SXBA_1_bit (firstpos_sets [visited->node_id], visited->cur_position);
    SXBA_1_bit (lastpos_sets [visited->node_id], visited->cur_position);
    position2visited [visited->cur_position] = visited;
    break;

  case WORD_SEMLEX_n:
    son = visited->son; /* word_n */
    second_re2dfa_pass (son);
    sxba_copy (firstpos_sets [visited->node_id], firstpos_sets [son->node_id]);
    sxba_copy (lastpos_sets [visited->node_id], lastpos_sets [son->node_id]);
    /*    son = son->brother;
	  first_re2dfa_pass (son);*/
    break;

    /*  case SEMLEX_n:
	break;*/

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

  case ALTERNATIVE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    for (son = visited->son; son != NULL; son = son->brother) {
      third_re2dfa_pass (son);
    }

    break;

  case KLEENE_PLUS_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
  case KLEENE_STAR_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    third_re2dfa_pass (visited->son);
    /* Toutes les positions de firstpos sont des followpos des positions de lastpos */
    lastpos_set = lastpos_sets [visited->node_id];
    firstpos_set = firstpos_sets [visited->node_id];

    pos = 0;
    
    while ((pos = sxba_scan (lastpos_set, pos)) > 0) {
      sxba_or (followpos [pos], firstpos_set);
    }

    break;

  case OPTION_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    third_re2dfa_pass (visited->son);
    break;

  case RE_ROOT_n :/* son[1]->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
    /* son[2]->name = eof_n */
    /* nullable = false; $ */
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

  case SEQUENCE_S_n :/* son->name = {ALTERNATIVE_S_n, KLEENE_PLUS_n, KLEENE_STAR_n, OPTION_n, SEQUENCE_S_n, WORD_SEMLEX_n, word_n} */
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

  case word_n :
    break;

  case eof_n :
    break;

  case WORD_SEMLEX_n:
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
  SXINT            pos, fpos, efnfa_state, prev_efnfa_state, next_efnfa_state, init_state_component_nb, final_efnfa_state, x, y;
  SXINT           ste;
  SXINT            *pos_stack, *final_pos_stack;
  SXBA           firstpos_set, followpos_set;
  struct re_node *visited;
  struct sxtoken *local_re2nfa_token_ptr_stack [3] = {NULL, NULL, NULL};
  struct sxtoken *local_re2nfa_semlex_token_ptr_stack [3] = {NULL, NULL, NULL};
  SXINT            *pos2efnfa_state;
  bool        has_init_component;

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

    if (visited->brother && visited->brother->name == SEMLEX_n) /* il a un frère, qui est donc le semlex associé */
      local_re2nfa_semlex_token_ptr_stack [1] = &(visited->brother->token);
      
    ste = visited->ste;
    followpos_set = followpos [pos];
    
    has_init_component = false;
    next_efnfa_state = 0;
    fpos = 0;

    while ((fpos = sxba_scan (followpos_set, fpos)) > 0) {
      if (fpos < position) {
	if (SXBA_bit_is_set (firstpos_set, fpos)) {
	  has_init_component = true;
	}
	else {
	  next_efnfa_state = pos2efnfa_state [fpos];
	  (*store_re)(prev_efnfa_state, local_re2nfa_token_ptr_stack, local_re2nfa_semlex_token_ptr_stack, ste, next_efnfa_state);
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
	(*store_re)(prev_efnfa_state, local_re2nfa_token_ptr_stack, local_re2nfa_semlex_token_ptr_stack, ste, 1);
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

	  (*store_re)(prev_efnfa_state, local_re2nfa_token_ptr_stack, local_re2nfa_semlex_token_ptr_stack, ste, next_efnfa_state);
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

    if (visited->brother && visited->brother->name == SEMLEX_n) /* il a un frère, qui est donc le semlex associé */
      local_re2nfa_semlex_token_ptr_stack [1] = &(visited->brother->token);

    ste = visited->ste;

    (*store_re)(prev_efnfa_state, local_re2nfa_token_ptr_stack, local_re2nfa_semlex_token_ptr_stack, ste, final_efnfa_state);
  }

  sxfree (pos2efnfa_state);
  sxfree (final_pos_stack);
  sxfree (pos_stack);
}



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
  SXINT          dfa_state, bot, top, pos, next_dfa_state, x, final_dfa_state;
  SXINT         ste;
  SXBA           followpos_set, pos_set, firstpos_set;
  struct sxtoken **re2dfa_token_ptr_stack;
  struct sxtoken **re2dfa_semlex_token_ptr_stack;
  SXINT            *dfa_state_stack, *final_trans_stack;
  SXBA           *ste2followpos_sets, *ste2pos_sets, ste_set;
  struct re_node *visited;

  SXINT            i, j, couple;
  SXINT            *to_be_sorted, *dfa_state2i;
  XxY_header     XxY_dfa;
  bool        already_found;
  
  re2dfa_token_ptr_stack = (struct sxtoken **) sxalloc (position+2, sizeof (struct sxtoken *)), re2dfa_token_ptr_stack [0] = 0;
  re2dfa_semlex_token_ptr_stack = (struct sxtoken **) sxalloc (position+2, sizeof (struct sxtoken *)), re2dfa_semlex_token_ptr_stack [0] = 0;
  XH_alloc (&XH_dfa, "XH_dfa", node_nb+1, 1, position, NULL, NULL);
  DALLOC_STACK (dfa_state_stack, node_nb+1);
  ste_set = sxba_calloc (SXSTRtop ()+1 +2 /* BS: ? */);
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

      ste = SXEMPTY_STE;

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
#else
            (void) already_found; /* pour faire taire gcc -Wall */
#endif /* EBUG */

	    XxY_dfa2next_state [couple] = next_dfa_state;
	    sxba_copy (XxY_dfa2pos_set [couple], pos_set);
	  }
	  else {
	    pos = 0;
	    x = 0;

	    while ((pos = sxba_scan_reset (pos_set, pos)) > 0) {
	      visited = position2visited [pos];
	      re2dfa_token_ptr_stack [++x] = &(visited->token);
	      
	      if (visited->brother && visited->brother->name == SEMLEX_n) /* il a un frère, c'est son semlex */
		re2dfa_semlex_token_ptr_stack [x] = &(visited->brother->token);
	      else
		re2dfa_semlex_token_ptr_stack [x] = NULL;
	    }

	    re2dfa_token_ptr_stack [++x] = NULL; /* fin de la liste */
	    re2dfa_semlex_token_ptr_stack [x] = NULL; /* fin de la liste */

	    (*store_re)(dfa_state, re2dfa_token_ptr_stack, re2dfa_semlex_token_ptr_stack, ste, next_dfa_state);
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
	  visited = position2visited [pos];
	  re2dfa_token_ptr_stack [++x] = &(visited->token);

	  if (visited->brother && visited->brother->name == SEMLEX_n) /* il a un frère, c'est son semlex */
	    re2dfa_semlex_token_ptr_stack [x] = &(visited->brother->token);
	  else
	    re2dfa_semlex_token_ptr_stack [x] = NULL;
	}

	re2dfa_token_ptr_stack [++x] = NULL; /* fin de la liste */
	re2dfa_semlex_token_ptr_stack [x] = NULL; /* fin de la liste */

	(*store_re)(i, re2dfa_token_ptr_stack, re2dfa_semlex_token_ptr_stack,  ste, j);
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

    (*store_re)(dfa_state, re2dfa_token_ptr_stack, NULL /* pas de semlex pour eof */, (SXINT) eof_ste, final_dfa_state);
  }
  
  sxfree (re2dfa_token_ptr_stack);
  sxfree (re2dfa_semlex_token_ptr_stack);
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
smpopen (SXTABLES *sxtables_ptr)
{
  sxuse (sxtables_ptr);
  sxatcvar.atc_lv.node_size = sizeof (struct re_node);
}



static SXINT
smppass (void)
{
  /*   I N I T I A L I S A T I O N S   */
  SXINT severity = 0;

  /*   A T T R I B U T E S    E V A L U A T I O N   */

  vstr = varstr_alloc (64);

  if (what_to_do_re & RE2NFA) {
    /* Cas re2nfa */
    /* Calcule, a partir d'une expression reguliere, l'automate non-deterministe associe' (avec transitions epsilon) */
    /* premiere passe synthetisee qui calcule max_nb la "longueur" max de la sous-expression */
    first_re2nfa_pass (sxatcvar.atc_lv.abstract_tree_root);

    if (prelude_re) (*prelude_re) ((bool) (!not_a_dag) /* RE OK */, (SXINT) eof_ste, re_node_nb, re_leaf_nb, RE2NFA);

    if (!not_a_dag) {
      /* seconde passe heritee qui calcule les pre et post pos de la sous-expression */
      second_re2nfa_pass (sxatcvar.atc_lv.abstract_tree_root, 1, 1, 1+sxatcvar.atc_lv.abstract_tree_root->max_nb);
      if (postlude_re)
	severity = (*postlude_re)(RE2NFA);
    }
  }

  if (!not_a_dag && (what_to_do_re & (RE2EFNFA | RE2DFA))) {
    /* cas re2dfa */
    /* Calcule, a partir d'une expression reguliere, l'automate non-deterministe associe' sans transitions epsilon, ou l'automate deterministe associe' (non minimal) */
    node_nb = position = 0;
    first_re2dfa_pass (sxatcvar.atc_lv.abstract_tree_root);

    if (not_a_dag) {
      if (prelude_re) (*prelude_re) (false /* RE pas OK */, (SXINT) eof_ste, node_nb, position, 0);
    }
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
	if (prelude_re)
	  (*prelude_re) (true /* RE OK */, (SXINT) eof_ste, node_nb, position, RE2EFNFA);

	extract_efnfa ();

	if (postlude_re)
	  severity = (*postlude_re)(RE2EFNFA);
      }

      if (what_to_do_re & RE2DFA) {
	/* Cas automate deterministe non minimal */
	if (prelude_re)
	  (*prelude_re) (true /* RE OK */, (SXINT) eof_ste, node_nb, position, RE2DFA);

	extract_dfa ();

	if (postlude_re)
	  severity = (*postlude_re)(RE2DFA);
      }

      sxbm_free (firstpos_sets), firstpos_sets = NULL;
      sxbm_free (followpos), followpos = NULL;
      sxfree (position2visited), position2visited = NULL;
    }
  }

  /*   F I N A L I S A T I O N S   */
  /* ........... */
  
  varstr_free (vstr);

  return severity;
}

void re_smp (SXINT what, SXTABLES *sxtables_ptr)
{
  sxuse(sxtables_ptr); /* pour faire taire gcc -Wunused */

  switch (what) {
  case SXOPEN:
    smpopen (sxtables_ptr);
    break;
  case SXSEMPASS: /* For [sub-]tree evaluation during parsing */
    break;
  case SXACTION:
    if (!sxatcvar.atc_lv.abstract_tree_is_error_node) {
      if (smppass ())
	sxtmsg (sxsvar.sxlv.terminal_token.source_index.file_name,
		"%sSentence too long w.r.t. the \"-mis\" option, abandoned\n",
		sxplocals.sxtables->err_titles [2]+1);
    }
    break;
  case SXCLOSE:
    break;
  default:
    break;
  }
}

SXINT
read_a_re (void (*prelude)(bool, SXINT, SXINT, SXINT, SXINT), 
	   void (*store)(SXINT, struct sxtoken **, struct sxtoken **, SXINT, SXINT), 
	   SXINT (*postlude)(SXINT), 
	   SXINT what_to_do)
{
  /* On suppose que source_file est non NULL */
  /* Si la lecture se fait sur stdin, l'appelant l'a recopie' ds un fichier temporaire */
  SXINT	severity;

  prelude_re = prelude;
  store_re = store;
  postlude_re = postlude;

  what_to_do_re = what_to_do;

  /* sxstr_mngr (SXBEGIN); Fait depuis l'exterieur */
  /* sxsrc_mngr (SXINIT, source_file, source_file_name); Fait depuis l'exterieur */
  /* sxerr_mngr (SXBEGIN); Fait depuis l'exterieur */
  
  (*(re_tables.SX_parser)) (SXBEGIN, &re_tables) /* Allocation des
                                                         * variables globales du
  parser */ ;

  syntax (SXOPEN, &re_tables) /* Initialisation de SYNTAX */ ;  

  syntax (SXACTION, &re_tables); /* Appel de l'"analyseur" sxparser_re_tcut */
  
  severity = sxerr_max_severity ();

  syntax (SXCLOSE, &re_tables);
  
  (*(re_tables.SX_parser)) (SXEND, &re_tables);
  
  /* sxerr_mngr (SXEND); Fait a` l'exterieur */
  /* sxstr_mngr (SXEND); Fait a` l'exterieur */
  /* sxsrc_mngr (SXFINAL); Fait ds re_reader */

  return severity;
}

/* laid il faudrait passer par un tdef */
#define eol_code 1

#undef sxparser
extern SXPARSER_FUNCTION sxparser;
/* La definition ci-dessus serait inutile si on incluait sxunix.h */

/* Le vrai parseur qui traite les RE est le parseur deterministe de SYNTAX */

/* C'est le parseur de re_tables.SX_parser */

void sxparser_re_tcut  (SXINT what, SXTABLES *arg)
{
  SXINT            lahead = 0, store_print_time;
  struct sxtoken   *ptoken;
  SXINT            tmax;
  SXSEMACT_FUNCTION *semact_saved = NULL;
  VARSTR           save_cur_input_vstr;
  char             cur_char;
  int /* pas SXINT */ jmp_buf_ret_val;
  static SXINT     call_nb;

  switch (what) {
  case SXACTION:
    /* on fait un "TCUT" sur les fins-de-ligne */
    tmax = -re_tables.SXP_tables.P_tmax;
    store_print_time = is_print_time;

    do { /* do pour chaque phrase */
      jmp_buf_ret_val = setjmp (environment_before_current_sentence);
      if (jmp_buf_ret_val != 0) {
	/* si > 0, on a planté au cours de l'analyse de la phrase courante,
	   on revient comme on était avant de commencer à l'analyser,
	   et on veut donc en sauter l'analyse cette fois-ci */
	if (free_after_long_jmp)
	  (*free_after_long_jmp) ();
	mem_signatures = &(parser_mem_signatures);
	lahead = 0; /* pour faire taire -Werror (variable ‘lahead’ might be clobbered by ‘longjmp’ or ‘vfork’) */

	if (sxplocals.Mtok_no > 1) {
	  (*(re_tables.SXP_tables.P_semact)) (SXCLOSE, SXANY_INT, arg); /* on libère l'arbre abstrait de la phrase=ligne qu'on vient de passer */
	  sxcomment_clear (&(sxplocals.sxcomment));
	  {
	    // et on le prépare à parser un nouveau RE
	    /* (équiv. à (*(re_tables.SX_scanner)) (SXINIT, arg) sans le sxnext_char final, car déjà fait en lisant le eol */
	    SXINT i;
	    
	    sxsvar.sxlv_s.include_no = 0;
	    
	    if (sxsvar.sxlv_s.counters != NULL) {
	      for (i = sxsvar.SXS_tables.S_counters_size - 1; i >= 0; i--)
		sxsvar.sxlv_s.counters[i] = 0;
	    }
	  }
	  sxstr_mngr (SXCLEAR); // vidange du sxstr_mngr
	}

	sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;
	mem_signature_mode = false;
	continue;
      }
	
      /*      if (time_out)
	sxcaught_timeout (time_out, for_semact.timeout_mngr);
      
      if (vtime_out_s+vtime_out_us)
	sxcaught_virtual_timeout (vtime_out_s, vtime_out_us, for_semact.vtimeout_mngr);
      */

      /* Les appels du scanner se font manuellement */
      sxerrmngr.err_kept = 0;
      sxerr_mngr (SXCLEAR);

      if (cur_input_vstr) {
	cur_char = '\0';
	if (varstr_length (cur_input_vstr))
	  cur_char = *(varstr_pop (cur_input_vstr, 1)); /* on a le symbole de lookahead, il faut le
							   stocker pour le mettre dans le
							   cur_input_vstr suivant */
	varstr_raz (cur_input_vstr);
	if (cur_char)
	  cur_input_vstr = varstr_catchar (cur_input_vstr, cur_char);
      }

      do {
	(*(sxplocals.SXP_tables.P_scan_it)) ();
	ptoken = &SXGET_TOKEN (sxplocals.Mtok_no);
	lahead = ptoken->lahead;
      } while (lahead != -tmax && lahead != eol_code);

      /* n = sxplocals.Mtok_no - 1;*/

      if (sxplocals.Mtok_no > 1) { /* on n'a pas trouvé une ligne vide, i.e. dont le eol est en position 1 */
	sxput_token (*ptoken); /* on prend un nouveau token */

	if (lahead == -tmax) {
	  /* On a trouve' la fin de fichier sans eol_code devant, on le simule */
	  ptoken->lahead = eol_code; /* on change le precedent */
	}
	else {
	  /* lahead == eol_code */
	  /* On force EOF derriere eol */
	    SXGET_TOKEN (sxplocals.Mtok_no).lahead = -tmax /* c'est-à-dire EOF */;
	}

	if (sxverbosep)
	  fprintf (sxstderr, "\n### Sentence %ld starting line %ld ###\n", (SXINT)  ++call_nb, (SXINT)  SXGET_TOKEN (1).source_index.line);

	sxparser (SXACTION, arg); /* On parse la RE courante et on alloue et construit l'arbre abstrait ... */
	
	if (free_after_long_jmp)
	  mem_signature_mode = true; /* on active le mécanisme permettant de libérer ce qu'il faut après un longjump */

	if (time_out)
	  sxcaught_timeout (time_out, for_semact.timeout_mngr);
	
	if (vtime_out_s+vtime_out_us)
	  sxcaught_virtual_timeout (vtime_out_s, vtime_out_us, for_semact.vtimeout_mngr);

	save_cur_input_vstr = cur_input_vstr;
	cur_input_vstr = NULL;
	(*(re_tables.SXP_tables.P_semact)) (SXSEMPASS, SXANY_INT, arg); /* ... On exploite l'arbre abstrait et on appelle prelude_re, store_re et postlude_re  */
	cur_input_vstr = save_cur_input_vstr;

	if (main_parser) {
	  if (sxerrmngr.nbmess [2] == 0)
	    (*main_parser) (SXACTION); // earley, ou un autre...
	  else {
	    if (cur_input_vstr) {
	      puts ("% Next sentence was not processed (too long)");
	      cur_char = *(varstr_pop (cur_input_vstr, 1)); /* on a le symbole de lookahead dont on ne veut pas */
	      cur_input_vstr = varstr_complete (cur_input_vstr);
	      puts (varstr_tostr (cur_input_vstr));
	      cur_input_vstr = varstr_catchar (cur_input_vstr, cur_char);
	    }
	    else
	      puts ("% Abandoned sentence was here");
	  }
	}

	if (free_after_long_jmp)
	  mem_signature_mode = false; /* on active le mécanisme permettant de libérer ce qu'il faut après un longjump */

	if (time_out)
	  sxcaught_timeout (0, for_semact.timeout_mngr);
	
	if (vtime_out_s+vtime_out_us)
	  sxcaught_virtual_timeout (0, 0, for_semact.vtimeout_mngr);

	(*(re_tables.SXP_tables.P_semact)) (SXCLOSE, SXANY_INT, arg); /* on libère l'arbre abstrait de la phrase=ligne qu'on vient de passer */
	sxcomment_clear (&(sxplocals.sxcomment));
	{
	  // et on le prépare à parser un nouveau RE
	  /* (équiv. à (*(re_tables.SX_scanner)) (SXINIT, arg) sans le sxnext_char final, car déjà fait en lisant le eol */
	  SXINT i;
	    
	  sxsvar.sxlv_s.include_no = 0;

	  if (sxsvar.sxlv_s.counters != NULL) {
	    for (i = sxsvar.SXS_tables.S_counters_size - 1; i >= 0; i--)
	      sxsvar.sxlv_s.counters[i] = 0;
	  }
	}
	sxstr_mngr (SXCLEAR); // vidange du sxstr_mngr
      }

      sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;

      if (free_after_long_jmp)
	sxmem_signatures_raz ();

    } while (lahead != -tmax);

    (*(re_tables.SXP_tables.P_semact)) (SXFINAL, SXANY_INT, arg);

    semact_saved = re_tables.SXP_tables.P_semact;
    re_tables.SXP_tables.P_semact = (SXSEMACT_FUNCTION *) NULL; // pour ne pas faire 2 fois semact (i.e. re_smp) sur le dernier re
      
    if (store_print_time) {
      is_print_time = true;
      sxtime (SXACTION, "\tTotal parse time");
    }
    return;
 
  case SXCLOSE:
    re_tables.SXP_tables.P_semact = semact_saved; // prudence, on restore le semact initial après l'avoir sxvoidé
    /* pas de break */
    /* FALLTHROUGH */
 
  case SXINIT:
    if (main_parser)
      (*main_parser) (what);
    sxparser (SXINIT, arg);
    sxplocals.mode.look_back = 0; /* Sinon, ça vaut 1, et sxtknzoom va chercher à récupérer de la
				     place dans toks_buf (0 le lui interdit), alors que (1) on va
				     vouloir y accéder plus tard, et (2) on va avoir un savant
				     mélange de tokens issus du parsing par RE et de tokens créés
				     quand on appelle LECL */
    break;
  default:
    if (main_parser)
      (*main_parser) (what);
    sxparser (what, arg);
    break;
  }
}


/* Point d'entree principal, si from_file, le source est dans un fichier de nom pathname_or_string (NULL si stdin) 
                             sinon, le source est ds la chaine C pathname_or_string */
/* re_process est la fonction de l'utilisateur qui appelle read_a_re () avec les bons parametres */
SXINT
re_reader (char *pathname_or_string, bool from_file, SXINT (*re_process)(void))
{
  FILE	*infile = NULL;
  SXINT severity;
  
  if (from_file) {
    if (pathname_or_string == NULL) {
      int	c;

      if (sxverbosep) {
	fputs ("\"stdin\":\n", sxtty);
      }      
      if (tmp_file_for_stdin) {          
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
	sxsrc_mngr (SXINIT, infile, "");
      } else {
          sxsrc_mngr (SXINIT, stdin, "");
      }
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

	sxsrc_mngr (SXINIT, infile, pathname_or_string);
      }
    }
  }
  else
    /* Le source est une chaine C */
    sxsrc_mngr (SXINIT, NULL, pathname_or_string);

  /* re_process est la fonction de l'utilisateur qui appelle read_a_re () avec les bons parametres */
  severity = (*re_process) ();

  if (infile)
    fclose (infile);

  sxsrc_mngr (SXFINAL);

  return severity;
}
