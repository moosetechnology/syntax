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
#define NODE struct dag_node

#define sxparser sxparser_dag_tcut
#define sxtables dag_tables
#define SCANACT  sxvoid
#define PARSACT  sxvoid
#define sempass  dag_smp
#include "sxcommon.h"
#include "dag_t.h"
#include "dag_scanner.h"
#include <stdarg.h>

char WHAT_READADAG[] = "@(#)SYNTAX - $Id: read_a_dag.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

struct dag_node {
  SXNODE_HEADER_S VOID_NAME;

  /*
    your attribute declarations...
  */
  SXINT             d_seq_nb, lpos, rpos, ltok_no, rtok_no, inside_seq_pos;
  struct dag_node *left_brother;
  SXBA            lpos_set, rpos_set;
  BOOLEAN         is_empty;
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

extern void (*main_parser)(void);

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
  switch (VISITED->father->name) {

  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* VISITED->name = SEQUENCE_S_n */
    if (VISITED->brother) {
      VISITED->brother->left_brother = VISITED;
    }

    VISITED->ltok_no = (VISITED->position == 1) ? VISITED->father->ltok_no : VISITED->left_brother->rtok_no;
       
    break;

  case EXPRESSION_n :/* VISITED->name = ALTERNATIVE_S_n */
    VISITED->ltok_no = VISITED->father->ltok_no;
    break;

  case OPTION_n :/* VISITED->name = ALTERNATIVE_S_n */
    VISITED->ltok_no = VISITED->father->ltok_no;
    break;

  case PROGRAM_ROOT_DAG_n :/* VISITED->name = SIMPLE_REGULAR_EXPRESSION_n */
    VISITED->ltok_no = VISITED->father->ltok_no;
    break;

  case SEQUENCE_S_n :/* VISITED->name = {EXPRESSION_n, OPTION_n, WORD_n} */
    if (VISITED->brother)
      VISITED->brother->left_brother = VISITED;

    if (VISITED->position == 1)
      VISITED->ltok_no = VISITED->father->ltok_no;
    else {
      VISITED->ltok_no = VISITED->left_brother->rtok_no;
      VISITED->inside_seq_pos = ++glbl_inside_seq_pos;
    }

    VISITED->ltok_no = (VISITED->position == 1) ? VISITED->father->ltok_no : VISITED->left_brother->rtok_no;
    break;

  case SIMPLE_REGULAR_EXPRESSION_n :/* VISITED->name = ALTERNATIVE_S_n */
    VISITED->ltok_no = VISITED->father->ltok_no;
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
  BOOLEAN                is_empty;

  switch (VISITED->name) {

  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :
    is_empty = FALSE;
    d_seq_nb = 0;

    for (brother = VISITED->son; brother != NULL; brother = brother->brother) {
      d_seq_nb += brother->d_seq_nb;

      if (brother->is_empty)
	is_empty = TRUE;
    }

    VISITED->d_seq_nb = d_seq_nb;
    VISITED->is_empty = is_empty;
    VISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case EXPRESSION_n :
    VISITED->d_seq_nb = LAST_SON->d_seq_nb;
    VISITED->is_empty = LAST_SON->is_empty;
    VISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case OPTION_n :
    VISITED->d_seq_nb = LAST_SON->d_seq_nb;
    VISITED->is_empty = TRUE;
    VISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case PROGRAM_ROOT_DAG_n :
    VISITED->d_seq_nb = LAST_SON->d_seq_nb;
    VISITED->is_empty = LAST_SON->is_empty;
    VISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case SEQUENCE_S_n :
    is_empty = TRUE;
    d_seq_nb = 0;

    for (brother = VISITED->son; brother != NULL; brother = brother->brother) {
      d_seq_nb += brother->d_seq_nb;

      if (!brother->is_empty)
	is_empty = FALSE;
    }

    VISITED->d_seq_nb = d_seq_nb + VISITED->degree - 1;
    VISITED->is_empty = is_empty;
    VISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case SIMPLE_REGULAR_EXPRESSION_n :
    VISITED->d_seq_nb = LAST_SON->d_seq_nb;
    VISITED->is_empty = LAST_SON->is_empty;
    VISITED->rtok_no = LAST_SON->rtok_no;
    break;

  case WORD_n :
    VISITED->d_seq_nb = 0;
    VISITED->is_empty = FALSE;
    VISITED->rtok_no = VISITED->ltok_no+1;
    break;

  default:
    break;
  }

  LAST_SON = VISITED;
  /* end dag1_pd */
}


/* IIeme passe : On herite les positions lpos dpos */

static void dag2_pi (void)
{
  struct dag_node        *brother, *last_brother;

  switch (VISITED->father->name) {

  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :/* VISITED->name = SEQUENCE_S_n */
    VISITED->lpos = VISITED->father->lpos;
    VISITED->lpos_set = VISITED->father->lpos_set;
    VISITED->rpos = VISITED->father->rpos;
    VISITED->rpos_set = VISITED->father->rpos_set;  
    break;

  case EXPRESSION_n :/* VISITED->name = ALTERNATIVE_S_n */
    VISITED->lpos = VISITED->father->lpos;
    VISITED->rpos = VISITED->father->rpos;
    VISITED->lpos_set = VISITED->father->lpos_set;
    VISITED->rpos_set = VISITED->father->rpos_set;
    break;

  case OPTION_n :/* VISITED->name = ALTERNATIVE_S_n */
    VISITED->lpos = VISITED->father->lpos;
    VISITED->rpos = VISITED->father->rpos;
    VISITED->lpos_set = VISITED->father->lpos_set;
    VISITED->rpos_set = VISITED->father->rpos_set;
    break;

  case PROGRAM_ROOT_DAG_n :/* VISITED->name = SIMPLE_REGULAR_EXPRESSION_n */
    VISITED->lpos = init_pos; /* valeur initiale */
    VISITED->rpos = final_pos; /* valeur finale */
    VISITED->lpos_set = left_pos_sets [VISITED->lpos];
    SXBA_1_bit (VISITED->lpos_set, VISITED->lpos);
    VISITED->rpos_set = right_pos_sets [VISITED->rpos];
    SXBA_1_bit (VISITED->rpos_set, VISITED->rpos);
    break;

  case SEQUENCE_S_n :/* VISITED->name = {EXPRESSION_n, OPTION_n, WORD_n} */
    if (VISITED->position == 1) {
      /* On balaie tous les fils de gauche a droite... */
      for (last_brother = brother = VISITED; brother != NULL; brother = (last_brother = brother)->brother) {
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

  case SIMPLE_REGULAR_EXPRESSION_n :/* VISITED->name = ALTERNATIVE_S_n */
    VISITED->lpos = VISITED->father->lpos;
    VISITED->rpos = VISITED->father->rpos;
    VISITED->lpos_set = VISITED->father->lpos_set;
    VISITED->rpos_set = VISITED->father->rpos_set;
    break;

  default:
    break;
  }
  /* end dag2_pi */
}

static void dag2_pd (void)
{
  switch (VISITED->name) {

  case ERROR_n :
    break;

  case ALTERNATIVE_S_n :
    break;

  case EXPRESSION_n :
    break;

  case OPTION_n :
    break;

  case PROGRAM_ROOT_DAG_n :
    if (VISITED->is_empty) {
      /* LAST_ELEM->lpos_set et LAST_ELEM->rpos_set ne servent (eventuellement) qu'a donner la valeur de final_pos */
      store_dag (LAST_ELEM->lpos_set, 0, NULL, LAST_ELEM->rpos_set);
    }

    break;

  case SEQUENCE_S_n :
    break;

  case SIMPLE_REGULAR_EXPRESSION_n :
    break;

  case WORD_n :
    store_dag (VISITED->lpos_set, VISITED->rtok_no, &(VISITED->token), VISITED->rpos_set);

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
SXINT dag_smp (SXINT what, struct sxtables *sxtables_ptr)
{
  sxuse(sxtables_ptr); /* pour faire taire gcc -Wunused */
  switch (what) {
  case OPEN:
    smpopen ();
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

  return 0;
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

  /* sxstr_mngr (BEGIN); Fait depuis l'exterieur */
  /* sxsrc_mngr (INIT, source_file, source_file_name); Fait depuis l'exterieur */

  (*(dag_tables.analyzers.parser)) (BEGIN, &dag_tables) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &dag_tables) /* Initialisation de SYNTAX */ ;

  /* sxerr_mngr (BEGIN); Fait depuis l'exterieur */
  syntax (ACTION, &dag_tables);

  for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
    ;

  /* sxerr_mngr (END); Fait a` l'exterieur */
  syntax (CLOSE, &dag_tables);
  (*(dag_tables.analyzers.parser)) (END, &dag_tables);
  /* sxstr_mngr (END); Fait a` l'exterieur */
  /* sxsrc_mngr (FINAL); Fait a` l'exterieur */

  return severity;
}

#define eol_code 1

#undef sxparser
extern SXINT sxparser  (SXINT	what_to_do, struct sxtables *arg);

extern SXINT sxivoid  (SXINT what, ...);
extern SXINT is_print_time, n;

SXINT
sxparser_dag_tcut  (SXINT what_to_do, struct sxtables *arg)
{
  BOOLEAN ret_val;
  SXINT     lahead, store_print_time;
  struct sxtoken *ptoken;
  SXINT tmax = -arg->SXP_tables.P_tmax;
  struct sxsvar sxsvar_saved;
  struct sxtoken **sxplocals_toks_buf_saved;
  SXINT (*semact_saved) (SXINT what, ...) = NULL;

  switch (what_to_do) {
  case ACTION:
    /* on fait un "TCUT" sur les fins-de-ligne */
    ret_val = TRUE;
    store_print_time = is_print_time;
    //    is_print_time = FALSE;

    do {
      do {
	(*(sxplocals.SXP_tables.scanit)) ();
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

	ret_val &= sxparser (what_to_do, arg);
	(*(arg->SXP_tables.semact)) (SEMPASS, arg);

	sxsvar_saved = sxsvar; // car ce qui suit fait appel à un "autre" scanner si #if no_dico
	sxplocals_toks_buf_saved = sxplocals.toks_buf;
	
	if (after_last_trans)
	  (*after_last_trans)();
	(*fill_dag_infos)();

	if (main_parser && (!check_dag || ((*check_dag)() == 0)))
	  (*main_parser)(); // earley, ou un autre...

	sxsvar = sxsvar_saved; // on restore le scanner de DAG
	{
	  // et on le prépare à parser un nouveau DAG
	  /* (équiv. à (*(arg->analyzers.scanner)) (INIT, arg) sans le sxnext_char final, car déjà fait en lisant le eol */
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

      if (lahead == eol_code)
	sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;

    } while (lahead != -tmax);

    semact_saved = arg->SXP_tables.semact;
    arg->SXP_tables.semact = sxivoid; // pour ne pas faire 2 fois semact (i.e. dag_smp) sur le dernier dag

    if (store_print_time) {
      is_print_time = TRUE;
      sxtime (ACTION, "\tTotal parse time");
    }
    
    break;
  case CLOSE:
    arg->SXP_tables.semact = semact_saved; // prudence, on restore le semact initial après l'avoir sxvoidé
    /* pas de break */
  default:
    sxparser (what_to_do, arg);
    break;
  }
  
  return TRUE;
}
