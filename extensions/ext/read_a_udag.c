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
static char ME [] = "read_a_udag";
#endif

/* Lire le commentaire en tete de dag_scanner.c */

/*   I N C L U D E S   */

#include <stdarg.h>
#include <setjmp.h>

/* Les tables des analyseurs du langage udag */

#define sxparser sxparser_udag_tcut
#define sxtables udag_tables
#define SCANACT  sxjvoid
#define PARSACT  sxjvoid
#define SEMACT   udag_action
#define SEMPASS  sxsvoid
#include "sxversion.h"
#include "sxcommon.h"
#include "udag_t.c"
#include "udag_scanner.h"
#include "earley.h"

char WHAT_READAUDAG[] = "@(#)SYNTAX - $Id: read_a_udag.c 4277 2024-09-09 09:29:54Z garavel $" WHAT_DEBUG;

static bool is_error;

extern void (*main_parser)(SXINT what);

static void (*store_dag)(SXINT, ...),
            (*after_last_trans)(void), 
            (*fill_dag_infos)(void), 
            (*raz_dag)(void);
static SXINT   (*check_dag)(void);
static SXPARSER_FUNCTION *udag_parser;

static void	action (SXINT action_no)
{
    SXINT                ste ;
    SXINT                 from_state,to_state;

    if (is_error)
	return;

    switch (action_no) {
    case 0:
	return;

    case 1:
    /* <TRANSITION>      = %state <FORM> %state ; 1 */
      ste = SXSTACKtoken (SXSTACKtop ()).string_table_entry; // récupère le symbole le plus à droite (ici, le terminal %state)
      to_state = atoi (sxstrget (ste));

      ste = SXSTACKtoken (SXSTACKtop () - 2).string_table_entry;
      from_state = atoi (sxstrget (ste));

      (*store_dag)(from_state, sxplocals.atok_no - 2, to_state);

      // remplir glbl_source

      return;

    default:
	fputs ("The function \"action\" is out of date with respect to its specification.\n", sxstderr);
	sxexit (SXSEVERITIES);
    }
}


void udag_action (SXINT what, SXINT numarg, SXTABLES *arg)
{
    (void) arg;
    switch (what) {
    case SXOPEN:
	break;

    case SXINIT:
        break;

    case SXACTION:
        action (numarg);
        break;

    case SXERROR:
	is_error = true;
	break;

    case SXFINAL:
	break;

    case SXCLOSE:
    case SXSEMPASS:
	break;

    default:
	fputs ("The function \"udag_action\" is out of date with respect to its specification.\n", sxstderr);
	sxexit (SXSEVERITIES);
    }
}

int read_a_udag (void (*store)(SXINT, ...) /* store_dag */, 
		   void (*pre_fill)(void), 
		   void (*fill)(void), 
		   SXINT (*check)(void), 
		   void (*raz)(void) /* raz_dag */,
		   SXPARSER_FUNCTION *parser)
{
  /* On suppose que source_file est non NULL */
  /* Si la lecture se fait sur stdin, l'appelant l'a recopie' ds un fichier temporaire */
  int	severity;

  store_dag = store; /* ici c'est store_udag en principe */
  after_last_trans = pre_fill;
  fill_dag_infos = fill;
  check_dag = check;
  raz_dag = raz;
  udag_parser = parser; /* Le parseur de udag (sxparser en general) */

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

/* la grammaire a la forme
<UDAG_GRAMMAR>    = bodag <TRANSITIONS> eodag ;

<TRANSITIONS>     = <TRANSITIONS> <TRANSITION> ;
<TRANSITIONS>     = <TRANSITION> ;

<FORM>            = %form ;
<FORM>            = %state ;

<TRANSITION>      = %state <FORM> %state ; 1
*/
#define eodag_code 2


SXINT get_eodag_code (void)
{
  return eodag_code;
}

void sxparser_udag_tcut  (SXINT what, SXTABLES *arg)
{
  SXINT   lahead = 0, store_print_time;
  struct  sxtoken *ptoken;
  SXINT   tmax = -arg->SXP_tables.P_tmax;
  SXSEMACT_FUNCTION *semact_saved = NULL;
  int /* pas SXINT */ jmp_buf_ret_val;
  static SXINT     call_nb;

  switch (what) {
  case SXACTION:
    /* on fait un "TCUT" sur les fins-de-dag */
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

      if (sxplocals.Mtok_no > 1) { /* on n'a pas trouvé un udag vide, i.e. dont le eodag est en position 1 */
	if (lahead == eodag_code) {
	  sxput_token (*ptoken);
	  SXGET_TOKEN (sxplocals.Mtok_no).lahead = -tmax /* c'est-à-dire EOF */;
	}

	if (sxverbosep)
	  fprintf (sxstderr, "\n### Sentence %ld starting line %ld ###\n", (SXINT)  ++call_nb, (SXINT)  SXGET_TOKEN (1).source_index.line);

	if (free_after_long_jmp)
	  mem_signature_mode = true; /* on active le mécanisme permettant de libérer ce qu'il faut après un longjump */

	if (time_out)
	  sxcaught_timeout (time_out, for_semact.timeout_mngr);
	
	if (vtime_out_s+vtime_out_us)
	  sxcaught_virtual_timeout (vtime_out_s, vtime_out_us, for_semact.vtimeout_mngr);

	if (raz_dag)
	  (*raz_dag) ();
	
	if (udag_parser)
	  (*udag_parser) (what, arg);

	if (after_last_trans)
	  (*after_last_trans) ();

	if (fill_dag_infos)
	  (*fill_dag_infos) ();

	if (main_parser && (!check_dag || ((*check_dag)() == 0)))
	  (*main_parser) (SXACTION); // earley, ou un autre...

	if (free_after_long_jmp)
	  mem_signature_mode = false; /* on active le mécanisme permettant de libérer ce qu'il faut après un longjump */

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
    /* FALLTHROUGH */

  case SXINIT:
    if (main_parser)
      (*main_parser) (what);
    if (udag_parser)
      (*udag_parser) (SXINIT, arg);
    sxplocals.mode.look_back = 0; /* Sinon, ça vaut 1, et sxtknzoom va chercher à récupérer de la
				     place dans toks_buf (0 le lui interdit), alors que (1) on va
				     vouloir y accéder plus tard, et (2) on va avoir un savant
				     mélange de tokens issus du parsing par RE et de tokens créés
				     quand on appelle LECL */
    break;

  default:
    if (main_parser)
      (*main_parser) (what);
    if (udag_parser)
      (*udag_parser) (what, arg);
    break;
  }
}
