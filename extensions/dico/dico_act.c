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

#include "sxunix.h"
#include "sxdico.h"

char WHAT_DICOACT[] = "@(#)SYNTAX - $Id: dico_act.c 4143 2024-08-02 08:50:12Z garavel $" WHAT_DEBUG;

extern SXINT             optim_kind, process_kind, print_on_sxtty;
extern char            *dico_name, *prgentname;
extern bool                static_kind, extern_kind;

static FILE	*dico_file = {NULL};

static struct mot2 mot2;
static struct dico dico;

static SXBA	word_list_set;

static SXINT	word_list_set_size;

static void
mot2_oflw (void)
{
    mot2.string = (char**) sxrealloc (mot2.string, (mot2.size *= 2) + 1, sizeof (char*));
    mot2.code = (SXINT*) sxrealloc (mot2.code, mot2.size + 1, sizeof (SXINT));
    mot2.lgth = (SXINT*) sxrealloc (mot2.lgth, mot2.size + 1, sizeof (SXINT));
}

static void
word_list_set_oflw (void)
{
    word_list_set = sxba_resize (word_list_set, (word_list_set_size *= 2) + 1);
}

static void     gripe (void)
#ifdef __GNUC__
     __attribute__ ((noreturn))
#endif
     ;

static void	gripe (void)
{
    fputs ("\nA function of \"dico\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}





static void
gen_header (void)
{
    time_t date_time;
    
    date_time = time (0);

    fprintf (dico_file,
"\n\
/* ********************************************************************\n\
\tThis dictionary for the words \"%s\" has been generated\n\
\ton %s\
\tby the SYNTAX(*) DICO processor\n\
   ********************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
   ******************************************************************** */\n\n\n",
	     prgentname, ctime (&date_time));
	
}





void dico_sem_act (SXINT code, SXINT numact, struct sxtables *arg)
{
  SXINT			ate;

  (void) arg;
  switch (code) {
  case SXOPEN:
    if (dico_file == NULL)
      dico_file = stdout;
    /* FALLTHROUGH */

  case SXCLOSE:
  case SXSEMPASS:
  case SXERROR:
    return;

  case SXINIT:
    mot2.string = (char**) sxalloc ((mot2.size = 100) + 1, sizeof (char*));
    mot2.string [0] = (char*) NULL;
    mot2.code = (SXINT*) sxalloc (mot2.size + 1, sizeof (SXINT));
    mot2.code [0] = 0;
    mot2.lgth = (SXINT*) sxalloc (mot2.size + 1, sizeof (SXINT));
    mot2.lgth [0] = 0;
    word_list_set = sxba_calloc ((word_list_set_size = 100) + 1);
    return;

  case SXFINAL:
    sxfree (mot2.string), mot2.string = NULL;
    sxfree (mot2.code), mot2.code = NULL;
    sxfree (mot2.lgth), mot2.lgth = NULL;
    return;

  case SXACTION:
    switch (numact) {
    case 6:
      /* <dico_input>	= <simple_list> 			; 6 */
    case 5:
      /* <dico_input>	= <double_list> 			; 5 */
      sxfree (word_list_set), word_list_set = NULL;

      if (sxerrmngr.nbmess [1] + sxerrmngr.nbmess [2] == 0) {
	mot2.optim_kind = optim_kind; /* TIME, SPACE, PREFIX_BOUND ou SUFFIX_BOUND.  Donne par option par l'utilisateur */
	mot2.process_kind = process_kind; /* TOTAL, PARTIEL.  Donne par option par l'utilisateur */
	mot2.print_on_sxtty = print_on_sxtty; /* un peu d'animation ? Donne par option par l'utilisateur */
	mot2.min = (SXINT*) sxalloc (mot2.nb + 1, sizeof (SXINT));
	mot2.max = (SXINT*) sxalloc (mot2.nb + 1, sizeof (SXINT));

	sxdico_process (mot2, &dico);

	if (( (~((SXUINT) 0)) >> dico.base_shift) >= dico.max) {
	  if (sxverbosep)
	    fputs ("Output dictionary ... ", sxtty);

	  gen_header ();
	  sxdico2c (&dico, dico_file, dico_name, static_kind);
#if 0
	  gen_includes ();
	  gen_dico ();
#endif /* 0 */

	  if (sxverbosep)
	    fputs ("done\n", sxtty);
	}
	else
	  sxtmsg ("%sToo many words, dictionary not produced\n", sxplocals.sxtables->err_titles [2]+1 /* Error */);

	sxfree (mot2.min), mot2.min = NULL;
	sxfree (mot2.max), mot2.max = NULL;
      }
      return;

    case 4:
      /* <simple_list>	= <simple_list> %string 		; 4 */
      ate = SXSTACKtoken (SXSTACKtop ()).string_table_entry;
	
      if (ate >= word_list_set_size) word_list_set_oflw ();

      if (!SXBA_bit_is_reset_set (word_list_set, ate))
	sxerror (SXSTACKtoken (SXSTACKtop ()).source_index,
		 sxplocals.sxtables->err_titles [1][0] /* Warning */,
		 "%sThis word is a duplicate: ignored.",
		 sxplocals.sxtables->err_titles [1]+1 /* Warning */);
      else {
	if (++mot2.nb > mot2.size) mot2_oflw ();

	mot2.string [mot2.nb] = sxstrget (ate);
	mot2.code [mot2.nb] = mot2.nb;
	mot2.total_lgth += mot2.lgth [mot2.nb] = sxstrlen (ate);
      }
      return;

    case 3:
      /* <simple_list>	= %string 				; 3 */
      ate = SXSTACKtoken (SXSTACKtop ()).string_table_entry;

      if (++mot2.nb > mot2.size) mot2_oflw ();
      if (ate > word_list_set_size) word_list_set_oflw ();

      mot2.string [mot2.nb] = sxstrget (ate);
      sxba_1_bit (word_list_set, ate);
      mot2.code [mot2.nb] = mot2.nb;
      mot2.lgth [mot2.nb] = mot2.total_lgth = sxstrlen (ate);
      return;

    case 2:
      /* <double_list>	= <double_list> %string %integer 	; 2 */
      ate = SXSTACKtoken (SXSTACKtop ()-1).string_table_entry;
	
      if (ate > word_list_set_size) word_list_set_oflw ();

      if (!SXBA_bit_is_reset_set (word_list_set, ate))
	sxerror (SXSTACKtoken (SXSTACKtop ()-1).source_index,
		 sxplocals.sxtables->err_titles [1][0] /* Warning */,
		 "%sThis word is a duplicate: ignored.",
		 sxplocals.sxtables->err_titles [1]+1 /* Warning */);
      else {
	if (++mot2.nb > mot2.size) mot2_oflw ();

	mot2.string [mot2.nb] = sxstrget (ate);
	mot2.code [mot2.nb] = atol (sxstrget (SXSTACKtoken (SXSTACKtop ()).string_table_entry));
	mot2.total_lgth += mot2.lgth [mot2.nb] = sxstrlen (ate);
      }
      return;

    case 1:
      /* <double_list>	= %string %integer 			; 1 */
      ate = SXSTACKtoken (SXSTACKnewtop ()).string_table_entry;

      if (++mot2.nb > mot2.size) mot2_oflw ();
      if (ate > word_list_set_size) word_list_set_oflw ();

      mot2.string [mot2.nb] = sxstrget (ate);
      sxba_1_bit (word_list_set, ate);
      mot2.code [mot2.nb] = atol (sxstrget (SXSTACKtoken (SXSTACKtop ()).string_table_entry));
      mot2.lgth [mot2.nb] = mot2.total_lgth = sxstrlen (ate);
      return;

    case 0:
      return;

    default:
      break;
    }
    /* FALLTHROUGH */

  default:
    gripe ();
    /*NOTREACHED*/
  }

  return;
}



extern struct sxtables	dico_tables;


bool dico_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    default:
	gripe ();
	    /*NOTREACHED*/

    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
	return SXANY_BOOL;

    case SXACTION:
	switch (act_no) {
	case 1:
	    /* \nnn => char */
	    {
		char	val;
		char	c, *s, *t;

		t = s = sxsvar.sxlv_s.token_string + sxsvar.sxlv.mark.index;

		for (val = *s++ - '0'; (c = *s++) != SXNUL; ) {
		    val = (val << 3) + c - '0';
		}

		*t = val;
		sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index + 1;
		sxsvar.sxlv.mark.index = -1;
	    }

	    return SXANY_BOOL;

	default:
	    break;

	}
    }

    return SXANY_BOOL;
}
