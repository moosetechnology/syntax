#if 0
static char ME [] = "read_a_udag";
#endif

/* Lire le commentaire en tete de dag_scanner.c */

/*   I N C L U D E S   */
/* Les tables des analyseurs du langage udag */

#define sxtables udag_tables
#define SCANACT  sxvoid
#define PARSACT  sxvoid
#define SEMACT   udag_action
#define sempass  sxvoid
#include "sxcommon.h"
#include "udag_t.h"
#include "dag_scanner.h"

char WHAT_READAUDAG[] = "@(#)SYNTAX - $Id: read_a_udag.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

static void (*store_udag)();
static BOOLEAN is_error;
static int udag_eof;

static VOID	action (action_no)
    long	action_no;
{
    register int	ste ;
    int                 from_state,to_state;


    if (is_error)
	return;

    switch (action_no) {
    case 0:
	return;

    case 1:
    /* <TRANSITION>      = %state <FORM> %state ; 1 */
      ste = SXSTACKtoken (SXSTACKtop ()).string_table_entry; // récupère le symbole le plus à droite (ici, le terminal %state)
      to_state = atoi(sxstrget(ste));

      ste = SXSTACKtoken (SXSTACKtop () - 2).string_table_entry;
      from_state = atoi(sxstrget(ste));

      (*store_udag)(from_state,&(SXGET_TOKEN (sxplocals.atok_no - 2)),to_state);

      // remplir glbl_source

      return;

    default:
	fputs ("The function \"action\" is out of date with respect to its specification.\n", sxstderr);
	sxexit (SEVERITIES);
    }
}




int udag_action (what, arg)
    int		what;
    struct sxtables	*arg;
{
    switch (what) {
    case OPEN:
	break;

    case INIT:
      udag_eof = sxeof_code(arg);
      break;

    case ACTION:
	action ((long) arg);
	break;

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

int get_udag_eof ()
{
  return udag_eof;
}

int read_a_udag (f)
     void (*f)();
{
  /* On suppose que source_file est non NULL */
  /* Si la lecture se fait sur stdin, l'appelant l'a recopie' ds un fichier temporaire */
  int	severity;

  store_udag = f;

  /* sxstr_mngr (BEGIN); Fait depuis l'exterieur */
  /* sxsrc_mngr (INIT, source_file, source_file_name); Fait depuis l'exterieur */

  (*(sxtables.analyzers.parser)) (BEGIN, &sxtables) /* Allocation des variables globales du parser */ ;
  syntax (OPEN, &sxtables) /* Initialisation de SYNTAX */ ;

  /* sxerr_mngr (BEGIN); Fait depuis l'exterieur */
  syntax (ACTION, &sxtables);

  for (severity = SEVERITIES - 1; severity > 0 && sxerrmngr.nbmess [severity] == 0; severity--)
    ;

  /* sxerr_mngr (END); Fait a` l'exterieur */
  syntax (CLOSE, &sxtables);
  (*(sxtables.analyzers.parser)) (END, &sxtables);
  /* sxstr_mngr (END); Fait a` l'exterieur */
  /* sxsrc_mngr (FINAL); Fait a` l'exterieur */

  return severity;
}

