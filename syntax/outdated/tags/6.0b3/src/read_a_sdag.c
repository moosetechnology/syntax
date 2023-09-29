#if 0
static char ME [] = "read_a_sdag";
#endif

/* Lire le commentaire en tete de dag_scanner.c */

/*   I N C L U D E S   */
/* Les tables des analyseurs du langage sdag */

#define sxparser sxparser_sdag_tcut
#define sxtables sdag_tables
#define SCANACT  sxvoid
#define PARSACT  sxvoid
#define SEMACT   sdag_action
#define sempass  sxvoid
#include "sxcommon.h"
#include "sdag_t.h"
#include "sxstack.h"
#include <stdarg.h>
char WHAT_READASDAG[] = "@(#)SYNTAX - $Id: read_a_sdag.c 874 2007-11-08 10:12:16Z rlacroix $" WHAT_DEBUG;

extern void (*main_parser)(void);

extern SXINT sxivoid  (SXINT what, ...);
extern SXINT is_print_time, n;

static void (*store_sdag_trans)(SXINT, SXINT, SXINT), 
            (*after_last_trans)(void), 
            (*fill_sdag_infos)(void), 
            (*raz_sdag)(void);
static SXINT   (*check_sdag)(void),
               (*sdag_parser)(SXINT what_to_do, struct sxtables *arg);

/*

<AXIOM>		= <SDAG_GRAMMAR> end_of_source	;
<AXIOM>		= <SDAG_GRAMMAR>		;

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
#define end_of_source_code 1

static SXINT  bot_stack_id;
static SXINT  *t_tok_no_stack;

extern SXINT  SEMLEX_lahead; /* defini ds dag_scanner */

SXINT
sdag_action (SXINT code, struct sxtables *numact)
{
  SXINT        tok_no;
  static SXINT cur_state, ff_tok_no, prev_ste;
  static SXBA  ste_set;

  sxuse (ff_tok_no);
  switch (code) {
  case OPEN:
  case CLOSE:
  case SEMPASS:
  case ERROR:
    break;

  case INIT:
    if (is_print_time)
      sxtime (INIT, NULL);

    cur_state = 1;
    ste_set = sxba_calloc (SXSTRtop()+1);

    break;

  case FINAL:
    break;

  case ACTION:
    switch ((SXINT)numact) {
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
	sxput_error (SXGET_TOKEN (tok_no).source_index,
		     "%sDuplicate terminal symbol: skipped",
		     sxplocals.sxtables->err_titles [1]);
	prev_ste = 0;
      }
      
      break;

    case 2:
      /*
	<FF>		= %form				; 2
      */
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
      DPUSH (t_tok_no_stack, 0);
      bot_stack_id = DTOP (t_tok_no_stack); /* vers la taille */
      DPUSH (t_tok_no_stack, 0);
      break;

    default:
      fputs ("The function \"sdag_action\" is out of date with respect to its specification.\n", sxstderr);
      sxexit (SEVERITIES);
    }

    break;

  default:
    fputs ("The function \"sdag_action\" is out of date with respect to its specification.\n", sxstderr);
    sxexit (SEVERITIES);
  }

  return 0;
}

int
read_a_sdag (void (*store)(SXINT, SXINT, SXINT) /* store_sdag */, 
             void (*pre_fill)(void), 
             void (*fill)(void),
	     SXINT  (*check)(void), 
             void (*raz)(void) /* raz_sdag */, 
             SXINT (*parser)(SXINT what_to_do, struct sxtables *arg))
{
  /* On suppose que source_file est non NULL */
  /* Si la lecture se fait sur stdin, l'appelant l'a recopie' ds un fichier temporaire */
  int	severity;

  store_sdag_trans = store;
  after_last_trans = pre_fill;
  fill_sdag_infos = fill;
  check_sdag = check;
  raz_sdag = raz;
  sdag_parser = parser; /* Le parseur de sdag (sxparser en general) */

  /* sxstr_mngr (BEGIN); Fait depuis l'exterieur */
  /* sxsrc_mngr (INIT, source_file, source_file_name); Fait depuis l'exterieur */
  /* sxerr_mngr (BEGIN); Fait depuis l'exterieur */

  (*(sxtables.analyzers.parser)) (BEGIN, &sxtables) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &sxtables) /* Initialisation de SYNTAX */ ;

  syntax (ACTION, &sxtables);

  for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
    ;

  syntax (CLOSE, &sxtables);
  (*(sxtables.analyzers.parser)) (END, &sxtables);

  /* sxerr_mngr (END); Fait a` l'exterieur */
  /* sxstr_mngr (END); Fait a` l'exterieur */
  /* sxsrc_mngr (FINAL); Fait a` l'exterieur */

  return severity;
}

/* L'appelant est fill_idag_infos ds udag_scanner */
SXINT *
sdag_get_t_tok_no_stack (SXINT id)
{
  return t_tok_no_stack+id;
}

SXINT
sxparser_sdag_tcut  (SXINT what_to_do, struct sxtables *arg )
{
  BOOLEAN ret_val;
  SXINT   lahead, store_print_time;
  struct  sxtoken *ptoken;
  SXINT   tmax = -arg->SXP_tables.P_tmax;
  struct  sxsvar sxsvar_saved;
  struct  sxtoken **sxplocals_toks_buf_saved;
  SXINT   (*semact_saved) (SXINT what, ...) = NULL;

  switch (what_to_do) {
  case ACTION:
    /* on fait un "TCUT" sur les fins-de-chaque "phrase" */
    ret_val = TRUE;
    store_print_time = is_print_time;
    //    is_print_time = FALSE;
    do {
      do {
	(*(sxplocals.SXP_tables.scanit)) ();
	ptoken = &SXGET_TOKEN (sxplocals.Mtok_no);
	lahead = ptoken->lahead;
      } while (lahead != -tmax && lahead != end_of_source_code);

      /* n = sxplocals.Mtok_no - 1;*/

      if (sxplocals.Mtok_no > 1) { /* on n'a pas trouvé un sdag vide, i.e. dont le eodag est en position 1 */
	if (lahead == end_of_source_code) {
	  sxput_token (*ptoken);
	  SXGET_TOKEN (sxplocals.Mtok_no).lahead = -tmax /* c'est-à-dire EOF */;
	}

	if (raz_sdag)
	  (*raz_sdag) ();

	DALLOC_STACK (t_tok_no_stack, sxplocals.Mtok_no);
	
	if (sdag_parser)
	  ret_val &= (*sdag_parser) (what_to_do, arg); // sxparser par exemple

	sxsvar_saved = sxsvar; // car ce qui suit fait appel à un "autre" scanner si #if no_dico
	sxplocals_toks_buf_saved = sxplocals.toks_buf;
	
	if (after_last_trans)
	  (*after_last_trans) ();

	if (fill_sdag_infos)
	  (*fill_sdag_infos) ();

	DFREE_STACK (t_tok_no_stack);

	if (main_parser && (!check_sdag || ((*check_sdag)() == 0)))
	  (*main_parser) (); // earley, ou un autre...

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

      if (lahead == end_of_source_code)
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
    if (sdag_parser)
      (*sdag_parser) (what_to_do, arg);

    break;
  }

  return TRUE;
}
