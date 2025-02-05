/* ********************************************************************
   *  This program has been generated from dag.at                     *
   *  on Fri Feb 28 14:02:13 2003                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */

#if 0
static char ME [] = "read_a_dag";
#endif

/* Lire le commentaire en tete de dag_scanner.c */

/*   I N C L U D E S   */
/* Les tables des analyseurs du langage dag */
#define SXNODE struct dag_node

#define sxparser sxparser_dag_tcut
#define sxtables dag_tables
#define SCANACT  sxjvoid
#define PARSACT  sxjvoid
#define SEMPASS  dag_smp
#include "sxcommon.h"
#include "dag_t.c"
#include "dag_scanner.h"
#include <stdarg.h>

char WHAT_READADAG[] = "@(#)SYNTAX - $Id: read_a_dag.c 4285 2024-09-09 12:43:30Z garavel $" WHAT_DEBUG;

struct dag_node {
  SXNODE_HEADER_S SXVOID_NAME;

  /*
    your attribute declarations...
  */
  SXINT             d_seq_nb, lpos, rpos, ltok_no, rtok_no, inside_seq_pos;
  struct dag_node *left_brother;
  SXBA            lpos_set, rpos_set;
  bool         is_empty;
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ALTERNATIVE_S_n 2
#define EXPRESSION_n 3
#define OPTION_n 4
#define PROGRAM_ROOT_DAG_n 5
#define SEQUENCE_S_n 6
#define SIMPLE_REGULAR_EXPRESSION_n 7
#define WORD_n 8
/*
E N D   N O D E   N A M E S
*/

static SXINT        glbl_inside_seq_pos;
static SXBA       *left_pos_sets, *right_pos_sets;
static SXBA       pos_set;  

extern void (*main_parser)(SXINT what);

static void (*store_dag)(SXBA, SXINT, struct sxtoken *, SXBA),
            (*after_last_trans)(void),
            (*fill_dag_infos)(void),
            (*raz_dag)(void);
static SXINT   (*check_dag)(void);

/* Iere passe, qui calcule pour chaque noeud dans d_seq_nb le "nombre de sequences" : 
   SEQUENCE_S_n : si p fils d_seq_nb = \sigma des fils + p -1
   ALTERNATIVE_S_n : = \sigma des fils
   On calcule aussi is_empty et left_brother
*/
static void dag1_pi (void)
{
  switch (SXVISITED->father->name) {

  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* SXVISITED->name = SEQUENCE_S_n */
    if (SXVISITED->brother) {
      SXVISITED->brother->left_brother = SXVISITED;
    }

    SXVISITED->ltok_no = (SXVISITED->position == 1) ? SXVISITED->father->ltok_no : SXVISITED->left_brother->rtok_no;
       
    break;

  case EXPRESSION_n :/* SXVISITED->name = ALTERNATIVE_S_n */
    SXVISITED->ltok_no = SXVISITED->father->ltok_no;
    break;

  case OPTION_n :/* SXVISITED->name = ALTERNATIVE_S_n */
    SXVISITED->ltok_no = SXVISITED->father->ltok_no;
    break;

  case PROGRAM_ROOT_DAG_n :/* SXVISITED->name = SIMPLE_REGULAR_EXPRESSION_n */
    SXVISITED->ltok_no = SXVISITED->father->ltok_no;
    break;

  case SEQUENCE_S_n :/* SXVISITED->name = {EXPRESSION_n, OPTION_n, WORD_n} */
    if (SXVISITED->brother)
      SXVISITED->brother->left_brother = SXVISITED;

    if (SXVISITED->position == 1)
      SXVISITED->ltok_no = SXVISITED->father->ltok_no;
    else {
      SXVISITED->ltok_no = SXVISITED->left_brother->rtok_no;
      SXVISITED->inside_seq_pos = ++glbl_inside_seq_pos;
    }

    SXVISITED->ltok_no = (SXVISITED->position == 1) ? SXVISITED->father->ltok_no : SXVISITED->left_brother->rtok_no;
    break;

  case SIMPLE_REGULAR_EXPRESSION_n :/* SXVISITED->name = ALTERNATIVE_S_n */
    SXVISITED->ltok_no = SXVISITED->father->ltok_no;
    break;

  default:
    break;
  }
  /* end dag1_pi */
}

static void dag1_pd (void)
{
  static struct dag_node *LAST_SON;
  SXINT                    d_seq_nb;
  struct dag_node        *brother;
  bool                is_empty;

  switch (SXVISITED->name) {

  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :
    is_empty = false;
    d_seq_nb = 0;

    for (brother = SXVISITED->son; brother != NULL; brother = brother->brother) {
      d_seq_nb += brother->d_seq_nb;

      if (brother->is_empty)
	is_empty = true;
    }

    SXVISITED->d_seq_nb = d_seq_nb;
    SXVISITED->is_empty = is_empty;
    SXVISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case EXPRESSION_n :
    SXVISITED->d_seq_nb = LAST_SON->d_seq_nb;
    SXVISITED->is_empty = LAST_SON->is_empty;
    SXVISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case OPTION_n :
    SXVISITED->d_seq_nb = LAST_SON->d_seq_nb;
    SXVISITED->is_empty = true;
    SXVISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case PROGRAM_ROOT_DAG_n :
    SXVISITED->d_seq_nb = LAST_SON->d_seq_nb;
    SXVISITED->is_empty = LAST_SON->is_empty;
    SXVISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case SEQUENCE_S_n :
    is_empty = true;
    d_seq_nb = 0;

    for (brother = SXVISITED->son; brother != NULL; brother = brother->brother) {
      d_seq_nb += brother->d_seq_nb;

      if (!brother->is_empty)
	is_empty = false;
    }

    SXVISITED->d_seq_nb = d_seq_nb + SXVISITED->degree - 1;
    SXVISITED->is_empty = is_empty;
    SXVISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case SIMPLE_REGULAR_EXPRESSION_n :
    SXVISITED->d_seq_nb = LAST_SON->d_seq_nb;
    SXVISITED->is_empty = LAST_SON->is_empty;
    SXVISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case WORD_n :
    SXVISITED->d_seq_nb = 0;
    SXVISITED->is_empty = false;
    SXVISITED->rtok_no = SXVISITED->ltok_no+1;
    break;

  default:
    break;
  }

  LAST_SON = SXVISITED;
  /* end dag1_pd */
}


/* IIeme passe : On herite les positions lpos dpos */

static void dag2_pi (void)
{
  struct dag_node        *brother, *last_brother;

  switch (SXVISITED->father->name) {

  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* SXVISITED->name = SEQUENCE_S_n */
    SXVISITED->lpos = SXVISITED->father->lpos;
    SXVISITED->lpos_set = SXVISITED->father->lpos_set;
    SXVISITED->rpos = SXVISITED->father->rpos;
    SXVISITED->rpos_set = SXVISITED->father->rpos_set;  
    break;

  case EXPRESSION_n :/* SXVISITED->name = ALTERNATIVE_S_n */
    SXVISITED->lpos = SXVISITED->father->lpos;
    SXVISITED->rpos = SXVISITED->father->rpos;
    SXVISITED->lpos_set = SXVISITED->father->lpos_set;
    SXVISITED->rpos_set = SXVISITED->father->rpos_set;
    break;

  case OPTION_n :/* SXVISITED->name = ALTERNATIVE_S_n */
    SXVISITED->lpos = SXVISITED->father->lpos;
    SXVISITED->rpos = SXVISITED->father->rpos;
    SXVISITED->lpos_set = SXVISITED->father->lpos_set;
    SXVISITED->rpos_set = SXVISITED->father->rpos_set;
    break;

  case PROGRAM_ROOT_DAG_n :/* SXVISITED->name = SIMPLE_REGULAR_EXPRESSION_n */
    SXVISITED->lpos = init_pos; /* valeur initiale */
    SXVISITED->rpos = final_pos; /* valeur finale */
    SXVISITED->lpos_set = left_pos_sets [SXVISITED->lpos];
    SXBA_1_bit (SXVISITED->lpos_set, SXVISITED->lpos);
    SXVISITED->rpos_set = right_pos_sets [SXVISITED->rpos];
    SXBA_1_bit (SXVISITED->rpos_set, SXVISITED->rpos);
    break;

  case SEQUENCE_S_n :/* SXVISITED->name = {EXPRESSION_n, OPTION_n, WORD_n} */
    if (SXVISITED->position == 1) {
      /* On balaie tous les fils de gauche a droite... */
      for (last_brother = brother = SXVISITED; brother != NULL; brother = (last_brother = brother)->brother) {
	if (brother->position == 1) {
	  /* 1er fils */
	  brother->lpos = brother->father->lpos;
	  brother->lpos_set = brother->father->lpos_set;
	}
	else {
	  /* autre fils */
	  brother->lpos = brother->inside_seq_pos;
	  brother->lpos_set = left_pos_sets [brother->lpos];
	  SXBA_1_bit (brother->lpos_set, brother->lpos);

	  if (brother->left_brother->is_empty)
	    sxba_or (brother->lpos_set, brother->left_brother->lpos_set);
	}
      }

      /* ...puis de droite a gauche */
      for (brother = last_brother; brother != NULL; brother = brother->left_brother) {
	if (brother == last_brother) {
	  /* Dernier fils */
	  brother->rpos = brother->father->rpos;
	  brother->rpos_set = brother->father->rpos_set;
	}
	else {
	  /* fils ayant un frere droit */
	  brother->rpos = brother->brother->lpos;
	  brother->rpos_set = right_pos_sets [brother->rpos];
	  SXBA_1_bit (brother->rpos_set, brother->rpos);

	  if (brother->brother->is_empty)
	   sxba_or (brother->rpos_set, brother->brother->rpos_set);
	}
      }
    }
    break;

  case SIMPLE_REGULAR_EXPRESSION_n :/* SXVISITED->name = ALTERNATIVE_S_n */
    SXVISITED->lpos = SXVISITED->father->lpos;
    SXVISITED->rpos = SXVISITED->father->rpos;
    SXVISITED->lpos_set = SXVISITED->father->lpos_set;
    SXVISITED->rpos_set = SXVISITED->father->rpos_set;
    break;

  default:
    break;
  }
  /* end dag2_pi */
}

static void dag2_pd (void)
{
  switch (SXVISITED->name) {

  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :
    break;

  case EXPRESSION_n :
    break;

  case OPTION_n :
    break;

  case PROGRAM_ROOT_DAG_n :
    if (SXVISITED->is_empty) {
      /* SXLAST_ELEM->lpos_set et SXLAST_ELEM->rpos_set ne servent (eventuellement) qu'a donner la valeur de final_pos */
      store_dag (SXLAST_ELEM->lpos_set, 0, NULL, SXLAST_ELEM->rpos_set);
    }

    break;

  case SEQUENCE_S_n :
    break;

  case SIMPLE_REGULAR_EXPRESSION_n :
    break;

  case WORD_n :
    store_dag (SXVISITED->lpos_set, SXVISITED->rtok_no, &(SXVISITED->token), SXVISITED->rpos_set);

    break;

  default:
    break;
  }

  /* end dag2_pd */
}



static void smpopen (void)
{
  sxatcvar.atc_lv.node_size = sizeof (struct dag_node);
}

static void smppass (void)
{

  /*   I N I T I A L I S A T I O N S  (1) */
  glbl_inside_seq_pos = 1;

  /*   A T T R I B U T E S    E V A L U A T I O N  (1)  */

  sxsmp (sxatcvar.atc_lv.abstract_tree_root, dag1_pi, dag1_pd);
    
  /*   I N I T I A L I S A T I O N S  (2)  */

  init_pos = 1;
  final_pos = sxatcvar.atc_lv.abstract_tree_root->d_seq_nb+2;
  last_tok_no = sxatcvar.atc_lv.abstract_tree_root->rtok_no;

  left_pos_sets = sxbm_calloc (final_pos+1, final_pos+1);
  right_pos_sets = sxbm_calloc (final_pos+1, final_pos+1);

  pos_set = sxba_calloc (final_pos+1);

  /*   A T T R I B U T E S    E V A L U A T I O N  (2)  */

  sxsmp (sxatcvar.atc_lv.abstract_tree_root, dag2_pi, dag2_pd);

  /*   F I N A L I S A T I O N S   */
  if (left_pos_sets)
    sxbm_free (left_pos_sets), left_pos_sets = NULL;

  if (right_pos_sets)
    sxbm_free (right_pos_sets), right_pos_sets = NULL;

  sxfree (pos_set);

}

/* ARGSUSED1 */
void dag_smp (SXINT what, SXTABLES *sxtables_ptr)
{
  sxuse(sxtables_ptr); /* pour faire taire gcc -Wunused */
  switch (what) {
  case SXOPEN:
    smpopen ();
    break;
  case SXSEMPASS: /* For [sub-]tree evaluation during parsing */
    break;
  case SXACTION:
    if (!sxatcvar.atc_lv.abstract_tree_is_error_node)
      smppass ();
    break;
  case SXCLOSE:
    break;
  default:
    break;
  }
}

int read_a_dag ( void (*f)(SXBA, SXINT, struct sxtoken *, SXBA) /* store_dag */, 
		   void (*pre_fill)(void), 
		   void (*fill)(void), 
		   SXINT (*check)(void), 
		   void (*g)(void) /* raz_dag */)
{
  /* On suppose que source_file est non NULL */
  /* Si la lecture se fait sur stdin, l'appelant l'a recopie' ds un fichier temporaire */
  int	severity;

  store_dag = f;
  after_last_trans = pre_fill;
  fill_dag_infos = fill;
  check_dag = check;
  raz_dag = g;

  /* sxstr_mngr (SXBEGIN); Fait depuis l'exterieur */
  /* sxsrc_mngr (SXINIT, source_file, source_file_name); Fait depuis l'exterieur */

  (*(dag_tables.SX_parser)) (SXBEGIN, &dag_tables) /* Allocation des variables globales du parser */ ;
  syntax (SXOPEN, &dag_tables) /* Initialisation de SYNTAX */ ;

  /* sxerr_mngr (SXBEGIN); Fait depuis l'exterieur */
  syntax (SXACTION, &dag_tables);

  for (severity = SXSEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
    ;

  /* sxerr_mngr (SXEND); Fait a` l'exterieur */
  syntax (SXCLOSE, &dag_tables);
  (*(dag_tables.SX_parser)) (SXEND, &dag_tables);
  /* sxstr_mngr (SXEND); Fait a` l'exterieur */
  /* sxsrc_mngr (SXFINAL); Fait a` l'exterieur */

  return severity;
}

#define eol_code 1

#undef sxparser
extern SXPARSER_FUNCTION sxparser;

extern SXINT is_print_time, n;

void sxparser_dag_tcut  (SXINT what_to_do, SXTABLES *arg)
{
  SXINT     lahead, store_print_time;
  struct sxtoken *ptoken;
  SXINT tmax = -arg->SXP_tables.P_tmax;
  struct sxsvar sxsvar_saved;
  struct sxtoken **sxplocals_toks_buf_saved;
  SXSEMACT_FUNCTION *semact_saved = NULL;

  switch (what_to_do) {
  case SXACTION:
    /* on fait un "TCUT" sur les fins-de-ligne */
    store_print_time = is_print_time;
    //    is_print_time = false;

    do {
      do {
	(*(sxplocals.SXP_tables.P_scan_it)) ();
	ptoken = &SXGET_TOKEN (sxplocals.Mtok_no);
	lahead = ptoken->lahead;
      } while (lahead != -tmax && lahead != eol_code);

      /* n = sxplocals.Mtok_no - 1;*/

      if (sxplocals.Mtok_no > 1) { /* on n'a pas trouvé une ligne vide, i.e. dont le eol est en position 1 */
	if (lahead == eol_code) {
	  sxput_token (*ptoken);
	  SXGET_TOKEN (sxplocals.Mtok_no).lahead = -tmax /* c'est-à-dire EOF */;
	}

	(*raz_dag)();

	sxparser (what_to_do, arg);
	(*(arg->SXP_tables.P_semact)) (SXSEMPASS, SXANY_INT, arg);

	sxsvar_saved = sxsvar; // car ce qui suit fait appel à un "autre" scanner si #if no_dico
	sxplocals_toks_buf_saved = sxplocals.toks_buf;
	
	if (after_last_trans)
	  (*after_last_trans)();
	(*fill_dag_infos)();

	if (main_parser && (!check_dag || ((*check_dag)() == 0)))
	  (*main_parser)(what_to_do); // earley, ou un autre...

	sxsvar = sxsvar_saved; // on restore le scanner de DAG
	{
	  // et on le prépare à parser un nouveau DAG
	  /* (équiv. à (*(arg->SX_scanner)) (SXINIT, arg) sans le sxnext_char final, car déjà fait en lisant le eol */
	  SXINT i;
	    
	  sxsvar.sxlv_s.include_no = 0;

	  if (sxsvar.sxlv_s.counters != NULL) {
	    for (i = sxsvar.SXS_tables.S_counters_size - 1; i >= 0; i--)
	      sxsvar.sxlv_s.counters[i] = 0;
	  }
	}

	sxplocals.toks_buf = sxplocals_toks_buf_saved;
	sxstr_mngr (SXBEGIN); // vidange du sxstr_mngr
      }

      if (lahead == eol_code)
	sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;

    } while (lahead != -tmax);

    semact_saved = arg->SXP_tables.P_semact;
    arg->SXP_tables.P_semact = sxwvoid; // pour ne pas faire 2 fois semact (i.e. dag_smp) sur le dernier dag

    if (store_print_time) {
      is_print_time = true;
      sxtime (SXACTION, "\tTotal parse time");
    }
    
    break;
  case SXCLOSE:
    arg->SXP_tables.P_semact = semact_saved; // prudence, on restore le semact initial après l'avoir sxvoidé
    /* pas de break */
    /* FALLTHROUGH */
  default:
    sxparser (what_to_do, arg);
    break;
  }
}

