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

#if 0
static char ME [] = "read_a_sdag";
#endif

/* Lire le commentaire en tete de dag_scanner.c */

/*   I N C L U D E S   */

#include <stdarg.h>
#include <setjmp.h>

/* Les tables des analyseurs du langage sdag */

#define sxparser sxparser_sdag_tcut
#define sxtables sdag_tables
#define SCANACT  sxjvoid
#define PARSACT  sxjvoid
#define SEMACT   sdag_action
#define SEMPASS  sxsvoid
#include "sxversion.h"
#include "sxcommon.h"
#include "sdag_t.c"
#include "udag_scanner.h"
#include "earley.h"
#include "sxstack.h"

char WHAT_READASDAG[] = "@(#)SYNTAX - $Id: read_a_sdag.c 4278 2024-09-09 09:48:21Z garavel $" WHAT_DEBUG;

extern void (*main_parser)(SXINT what);

static void (*store_sdag_trans)(SXINT, SXINT, SXINT);
static void (*after_last_trans)(void);
static void (*fill_sdag_infos)(void);
static void (*raz_sdag)(void);

static SXINT (*check_sdag)(void);
static SXPARSER_FUNCTION *sdag_parser;


/*


<AXIOM>		= bodag <SDAG_GRAMMAR> eodag	;

<SDAG_GRAMMAR>	= <SDAG_GRAMMAR> <TOK_SPEC>	;
<SDAG_GRAMMAR>	= ;

<TOK_SPEC>	= <FF> ( <TERMINAL1+> )		; 5

<FF>		= %form				; 2
<FF>		=				; 6

<TERMINAL1+>	= <TERMINAL+>			; 4
<TERMINAL1+>	= <TERMINAL+> %SEMLEX		; 3

<TERMINAL+>	= <TERMINAL1+> <TERMINAL>	;
<TERMINAL+>	= <TERMINAL>			;

<TERMINAL>	= %form				; 1

*/

/* Vu la forme de la grammaire */
#define eodag_code 2

static SXINT  bot_stack_id;
static SXINT  *t_tok_no_stack;

extern SXINT  SEMLEX_lahead; /* defini ds dag_scanner */

static SXBA  ste_set;

static SXINT cur_state;

void sdag_action (SXINT code, SXINT numact, SXTABLES *arg)
{
  SXINT        tok_no;
  static SXINT prev_ste;

  (void) arg;
  switch (code) {
  case SXOPEN:
  case SXCLOSE:
  case SXFINAL:
  case SXSEMPASS:
  case SXERROR:
    break;

  case SXINIT:
    if (is_print_time)
      sxtime (SXINIT, NULL);
    break;

  case SXACTION:
    switch (numact) {
    case 0:
      break;

    case 1:
      /*
	<TERMINAL>	= %form				; 1
      */
      tok_no = sxplocals.atok_no-1;
      DPUSH (t_tok_no_stack, tok_no);
      prev_ste = SXGET_TOKEN (tok_no).string_table_entry;

      if (!SXBA_bit_is_reset_set (ste_set, prev_ste)) {
	sxerror (SXGET_TOKEN (tok_no).source_index,
		 sxplocals.sxtables->err_titles [1][0],
		 "%sDuplicate terminal symbol: skipped",
		 sxplocals.sxtables->err_titles [1]+1);
	prev_ste = 0;
      }
      break;

    case 2:
      /*
	<FF>		= %form				; 2
      */
      prev_ste = 0;
      DPUSH (t_tok_no_stack, 0);
      bot_stack_id = DTOP (t_tok_no_stack); /* vers la taille */
      tok_no = sxplocals.atok_no-1;
      DPUSH (t_tok_no_stack, tok_no);
      break;

    case 3:
      /*
	<TERMINAL1+>	= <TERMINAL+> %SEMLEX		; 3
      */
      if (prev_ste) {
	/* pas elimine' */
	tok_no = sxplocals.atok_no-1;
	DPUSH (t_tok_no_stack, tok_no);
	/* pour identifier les tokens qui sont des SEMLEX */
	SEMLEX_lahead = SXGET_TOKEN (tok_no).lahead; /* Ds dag_scanner */
      }
      break;

    case 4:
      /*
	<TERMINAL1+>	= <TERMINAL+>			; 4
      */
      if (prev_ste) {
	/* pas elimine' */
	/* pas de %SEMLEX */
	DPUSH (t_tok_no_stack, 0);
      }
      break;

    case 5:
      /*
	<TOK_SPEC>	= <FF> ( <TERMINAL1+> )		; 5
      */
      t_tok_no_stack [bot_stack_id] = DTOP (t_tok_no_stack)-bot_stack_id; /* taille */
      
      (*store_sdag_trans) (cur_state, bot_stack_id, cur_state+1);

      cur_state++;
      sxba_empty (ste_set);
      break;

    case 6:
      /*
	<FF>		= 			; 6
      */
      prev_ste = 0;
      DPUSH (t_tok_no_stack, 0);
      bot_stack_id = DTOP (t_tok_no_stack); /* vers la taille */
      DPUSH (t_tok_no_stack, 0);
      break;

    default:
      fputs ("The function \"sdag_action\" is out of date with respect to its specification.\n", sxstderr);
      sxexit (SXSEVERITIES);
    }
    break;

  default:
    fputs ("The function \"sdag_action\" is out of date with respect to its specification.\n", sxstderr);
    sxexit (SXSEVERITIES);
  }
}

SXINT
read_a_sdag (void (*store)(SXINT, SXINT, SXINT) /* store_sdag */, 
             void (*pre_fill)(void), 
             void (*fill)(void),
	     SXINT  (*check)(void), 
             void (*raz)(void) /* raz_sdag */, 
             SXPARSER_FUNCTION *parser)
{
  /* On suppose que source_file est non NULL */
  /* Si la lecture se fait sur stdin, l'appelant l'a recopie' ds un fichier temporaire */
  SXINT	severity;

  store_sdag_trans = store;
  after_last_trans = pre_fill;
  fill_sdag_infos = fill;
  check_sdag = check;
  raz_sdag = raz;
  sdag_parser = parser; /* Le parseur de sdag (sxparser en general) */

  /* sxstr_mngr (SXBEGIN); Fait depuis l'exterieur */
  /* sxsrc_mngr (SXINIT, source_file, source_file_name); Fait depuis l'exterieur */
  /* sxerr_mngr (SXBEGIN); Fait depuis l'exterieur */

  (*(sxtables.SX_parser)) (SXBEGIN, &sxtables) /* Allocation des variables globales du parser */ ;
  syntax (SXOPEN, &sxtables) /* Initialisation de SYNTAX */ ;

  syntax (SXACTION, &sxtables);

  severity = sxerr_max_severity ();

  syntax (SXCLOSE, &sxtables);
  (*(sxtables.SX_parser)) (SXEND, &sxtables);

  /* sxerr_mngr (SXEND); Fait a` l'exterieur */
  /* sxstr_mngr (SXEND); Fait a` l'exterieur */
  /* sxsrc_mngr (SXFINAL); Fait a` l'exterieur */

  return severity;
}

/* L'appelant est fill_idag_infos ds udag_scanner */
SXINT *
sdag_get_t_tok_no_stack (SXINT id)
{
  return t_tok_no_stack+id;
}

void sxparser_sdag_tcut  (SXINT what, SXTABLES *arg)
{
  SXINT   lahead = 0, store_print_time;
  struct  sxtoken *ptoken;
  SXINT   tmax = -arg->SXP_tables.P_tmax;
  SXSEMACT_FUNCTION *semact_saved = NULL;
  int /* pas SXINT */ jmp_buf_ret_val;
  static SXINT     call_nb;

  switch (what) {
  case SXACTION:
    /* on fait un "TCUT" sur les fins-de-chaque "phrase" */
    store_print_time = is_print_time;
    //    is_print_time = false;

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
	  sxcomment_clear (&(sxplocals.sxcomment));
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
	  sxstr_mngr (SXCLEAR); // vidange du sxstr_mngr
	}

	sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;
	mem_signature_mode = false;
	continue;
      }

      sxerrmngr.err_kept = 0;
      sxerr_mngr (SXCLEAR);

      do {
	(*(sxplocals.SXP_tables.P_scan_it)) ();
	ptoken = &SXGET_TOKEN (sxplocals.Mtok_no);
	lahead = ptoken->lahead;
      } while (lahead != -tmax && lahead != eodag_code);

      /* n = sxplocals.Mtok_no - 1;*/

      if (sxplocals.Mtok_no > 1) { /* on n'a pas trouvé un sdag vide, i.e. dont le eodag est en position 1 */
	if (lahead == eodag_code) {
	  sxput_token (*ptoken);
	  SXGET_TOKEN (sxplocals.Mtok_no).lahead = -tmax /* c'est-à-dire EOF */;
	}

	if (sxverbosep)
	  fprintf (sxstderr, "\n### Sentence %ld starting line %ld ###\n", (SXINT)  ++call_nb, (SXINT) SXGET_TOKEN (1).source_index.line);

	if (free_after_long_jmp)
	  mem_signature_mode = true; /* on active le mécanisme permettant de libérer ce qu'il faut après un longjump */

	if (time_out)
	  sxcaught_timeout (time_out, for_semact.timeout_mngr);
	
	if (vtime_out_s+vtime_out_us)
	  sxcaught_virtual_timeout (vtime_out_s, vtime_out_us, for_semact.vtimeout_mngr);

	if (raz_sdag)
	  (*raz_sdag) ();

	ste_set = sxba_calloc (SXSTRtop()+1);
	DALLOC_STACK (t_tok_no_stack, sxplocals.Mtok_no);
	
	cur_state = 1; /* Pour sdag_action (SXACTION, ...) */

	if (sdag_parser)
	  (*sdag_parser) (what, arg); // sxparser par exemple

	if (after_last_trans)
	  (*after_last_trans) ();

	if (fill_sdag_infos)
	  (*fill_sdag_infos) ();

	DFREE_STACK (t_tok_no_stack);
	sxfree (ste_set), ste_set = NULL;

	if (main_parser && (!check_sdag || ((*check_sdag)() == 0)))
	  (*main_parser) (SXACTION); // earley, ou un autre...

	if (free_after_long_jmp)
	  mem_signature_mode = false;

	if (time_out)
	  sxcaught_timeout (0, for_semact.timeout_mngr);
	
	if (vtime_out_s+vtime_out_us)
	  sxcaught_virtual_timeout (0, 0, for_semact.vtimeout_mngr);

	sxcomment_clear (&(sxplocals.sxcomment));
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
	sxstr_mngr (SXCLEAR); // vidange du sxstr_mngr
      }

      if (lahead == eodag_code)
	sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;

      if (free_after_long_jmp)
	sxmem_signatures_raz ();

    } while (lahead != -tmax);

    semact_saved = arg->SXP_tables.P_semact;
    arg->SXP_tables.P_semact = (SXSEMACT_FUNCTION *) NULL; // pour ne pas faire 2 fois semact (i.e. dag_smp) sur le dernier dag

    if (store_print_time) {
      is_print_time = true;
      sxtime (SXACTION, "\tTotal parse time");
    }
    return;

  case SXCLOSE:
    arg->SXP_tables.P_semact = semact_saved; // prudence, on restore le semact initial après l'avoir sxvoidé
    /* pas de break */
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
    __attribute__ ((fallthrough));
#endif

  case SXINIT:
    if (main_parser)
      (*main_parser) (what);
    if (sdag_parser)
      (*sdag_parser) (what /* était SXINIT, mais on comprend pas pourquoi on ferait SXINIT quand on vient depuis SXCLOSE */, arg);
    sxplocals.mode.look_back = 0; /* Sinon, ça vaut 1, et sxtknzoom va chercher à récupérer de la
				     place dans toks_buf (0 le lui interdit), alors que (1) on va
				     vouloir y accéder plus tard, et (2) on va avoir un savant
				     mélange de tokens issus du parsing par RE et de tokens créés
				     quand on appelle LECL */
    break;

  default:
    if (main_parser)
      (*main_parser) (what);
    if (sdag_parser)
      (*sdag_parser) (what, arg);
    break;
  }
}
