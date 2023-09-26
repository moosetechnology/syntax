#if 0
static char ME [] = "read_a_udag";
#endif

/* Lire le commentaire en tete de dag_scanner.c */

/*   I N C L U D E S   */
/* Les tables des analyseurs du langage udag */

#define sxparser sxparser_udag_tcut
#define sxtables udag_tables
#define SCANACT  sxvoid
#define PARSACT  sxvoid
#define SEMACT   udag_action
#define sempass  sxvoid
#include "sxcommon.h"
#include "udag_t.h"
#include "dag_scanner.h"
#include <stdarg.h>
char WHAT_READAUDAG[] = "@(#)SYNTAX - $Id: read_a_udag.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

static BOOLEAN is_error;
static SXINT udag_eof;

extern void (*main_parser)(void);

static void (*store_dag)(SXINT, ...),
            (*after_last_trans)(void), 
            (*fill_dag_infos)(void), 
            (*raz_dag)(void);
static SXINT   (*check_dag)(void),
	(*udag_parser)(SXINT what_to_do, struct sxtables *arg);

static VOID	action (long action_no)
{
    SXINT	ste ;
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
	sxexit (SEVERITIES);
    }
}




SXINT
udag_action (SXINT what, struct sxtables *arg)
{
    switch (what) {
    case OPEN:
	break;

    case INIT: {
      udag_eof = sxeof_code(arg);
      break;
    }
    case ACTION: {
      action ((long)arg);
      break;
    }

    case ERROR:
	is_error = TRUE;
	break;

    case FINAL:
	break;

    case CLOSE:
    case SEMPASS:
	break;

    default:
	fputs ("The function \"udag_action\" is out of date with respect to its specification.\n", sxstderr);
	sxexit (SEVERITIES);
    }

    return 0;
}

int read_a_udag (void (*store)(SXINT, ...) /* store_dag */, 
		   void (*pre_fill)(void), 
		   void (*fill)(void), 
		   SXINT (*check)(void), 
		   void (*raz)(void) /* raz_dag */,
		   SXINT (*parser)(SXINT, struct sxtables *))
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

/* truc, la grammaire a la forme
<UDAG_GRAMMAR>    = <TRANSITIONS> ;
<UDAG_GRAMMAR>    = bodag <TRANSITIONS> eodag ;
*/
#define eodag_code 2

#if 0
SXINT get_udag_eof ()
{
  return udag_eof;
}
#endif /* 0 */

SXINT get_eodag_code (void)
{
  return eodag_code;
}



extern SXINT sxivoid  (SXINT what, ...);
extern SXINT is_print_time, n;

SXINT
sxparser_udag_tcut  (SXINT what_to_do, struct sxtables *arg)
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
    /* on fait un "TCUT" sur les fins-de-dag */
    ret_val = TRUE;
    store_print_time = is_print_time;
    //    is_print_time = FALSE;
    do {
      do {
	(*(sxplocals.SXP_tables.scanit)) ();
	ptoken = &SXGET_TOKEN (sxplocals.Mtok_no);
	lahead = ptoken->lahead;
      } while (lahead != -tmax && lahead != eodag_code);

      /* n = sxplocals.Mtok_no - 1;*/

      if (sxplocals.Mtok_no > 1) { /* on n'a pas trouvé un udag vide, i.e. dont le eodag est en position 1 */
	if (lahead == eodag_code) {
	  sxput_token (*ptoken);
	  SXGET_TOKEN (sxplocals.Mtok_no).lahead = -tmax /* c'est-à-dire EOF */;
	}

	if (raz_dag)
	  (*raz_dag) ();
	
	if (udag_parser)
	  ret_val &= (*udag_parser) (what_to_do, arg);

	sxsvar_saved = sxsvar; // car ce qui suit fait appel à un "autre" scanner si #if no_dico
	sxplocals_toks_buf_saved = sxplocals.toks_buf;
	
	if (after_last_trans)
	  (*after_last_trans) ();

	if (fill_dag_infos)
	  (*fill_dag_infos) ();

	if (main_parser && (!check_dag || ((*check_dag)() == 0)))
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

      if (lahead == eodag_code)
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
    if (udag_parser)
      (*udag_parser) (what_to_do, arg);

    break;
  }

  return TRUE;
}
