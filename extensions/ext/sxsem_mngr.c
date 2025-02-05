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

/* Programme qui organise les modules s�mantiques eventuels appliqu�s sur une for�t partag�e d'analyse CFG */

static char	ME [] = "sxsem_mngr";

#include "sxversion.h"
#include "sxunix.h"
#include "earley.h"
#include "sxspf.h"
#include "varstr.h"
#include <setjmp.h>

#if defined(TIMEOUT_HANDLER) || defined(MEMOFLW_HANDLER) || defined(SXTRAP_HANDLER)
#define LONGJUMP_SUPPORT
#endif /* TIMEOUT_HANDLER || MEMOFLW_HANDLER || SXTRAP_HANDLER */

#if defined(TIMEOUT_HANDLER) && TIMEOUT_HANDLER != 0
extern void TIMEOUT_HANDLER (void)
#endif

char WHAT_SXSEM_MNGR[] = "@(#)SYNTAX - $Id: sxsem_mngr.c 3910 2024-04-22 07:49:03Z garavel $" WHAT_DEBUG;

static struct for_semact weights_for_semact, structure_for_semact, chunker_for_semact;
static struct for_semact proper_for_semact, nbest_for_semact, exact_nbest_for_semact;;
static struct for_semact lfg_for_semact, output_for_semact, usersem_for_semact;
static VARSTR            vstr_usage, vstr_ME;
static char              *map_spec;
char                     *cur_map_spec;

char              cur_spec = '\1';
SXBA              *status_stack;
static SXINT             status_stack_size;

#if defined(exact_nbest) && !defined(nbest)
#define nbest
#endif /* defined(exact_nbest) && !defined(nbest) */

#ifdef output
#ifdef lfg
extern void              lfg_output_semact ();
#else /* lfg */
extern void              output_semact ();
#endif /* lfg */
#endif /* output */
#ifdef lfg
extern void              lfg_semact ();
#endif /* lfg */
#ifdef nbest
extern void              nbest_semact ();
#endif /* nbest */
#ifdef exact_nbest
extern void              exact_nbest_semact ();
#endif /* exact_nbest */
#ifdef weights
extern void              weights_semact ();
#endif /* weights */
#ifdef structure
extern void              structure_semact ();
#endif /* structure */
#ifdef chunker
extern void              chunker_semact ();
#endif /* chunker */
#ifdef proper
extern void              proper_semact ();
#endif /* proper */
#ifdef usersem
extern void              usersem ();
#endif /* usersem */

#ifndef DEFAULT_MAP
#define DEFAULT_MAP "l"
#endif /* ndef DEFAULT_MAP */
static char              map_spec_default [] = DEFAULT_MAP; /* par d�faut on sort la for�t */

static char	Usage [] = "\
\t\t-m spec, -map spec (default -map " DEFAULT_MAP ")\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	   0
#define MAP 	           1

bool     weights_sem_pass_arg;
bool     structure_sem_pass_arg;
bool     chunker_sem_pass_arg;
bool     proper_sem_pass_arg;
char          output_sem_pass_arg;

static char	*option_tbl [] = {
    "",
    "m", "map",
};

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    MAP, MAP,
};

static SXINT	option_get_kind (char *arg)
{
  char	**opt;

  if (*arg++ != '-')
    return UNKNOWN_ARG;

  if (*arg == SXNUL)
    return UNKNOWN_ARG;

  for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
    if (strcmp (*opt, arg) == 0 /* egalite */ )
      break;
  }

  return option_kind [opt - option_tbl];
}



static char	*option_get_text (SXINT kind)
{
  SXINT	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}

static bool call_sem_pass (struct for_semact *call_sem_pass_for_semact)
{
  bool ret_val;

  if (call_sem_pass_for_semact->sem_pass) {
#ifdef LONGJUMP_SUPPORT
    mem_signatures = &(call_sem_pass_for_semact->mem_signatures);
#endif /* LONGJUMP_SUPPORT */

    if (call_sem_pass_for_semact->pass_nb && call_sem_pass_for_semact->sem_final) {
      (*(call_sem_pass_for_semact->sem_final)) ();
#ifdef LONGJUMP_SUPPORT
      sxmem_signatures_raz ();
#endif /* LONGJUMP_SUPPORT */
    }

    call_sem_pass_for_semact->pass_nb++;

    if (call_sem_pass_for_semact->sem_init)
      (*(call_sem_pass_for_semact->sem_init)) ();

    ret_val = (*(call_sem_pass_for_semact->sem_pass)) ();
#ifdef LONGJUMP_SUPPORT
    mem_signatures = &(for_semact.mem_signatures);
#endif /* LONGJUMP_SUPPORT */
    return ret_val;
  }
  return false;
}

static bool call_sem_final (struct for_semact *call_sem_final_for_semact)
{
  if (call_sem_final_for_semact->sem_pass && call_sem_final_for_semact->pass_nb && call_sem_final_for_semact->sem_final) {
#ifdef LONGJUMP_SUPPORT
    mem_signatures = &(call_sem_final_for_semact->mem_signatures);
#endif /* LONGJUMP_SUPPORT */

    (*(call_sem_final_for_semact->sem_final)) ();
    call_sem_final_for_semact->pass_nb = 0;
#ifdef LONGJUMP_SUPPORT
    sxmem_signatures_raz ();
    mem_signatures = &(for_semact.mem_signatures);
#endif /* LONGJUMP_SUPPORT */
    return true;
  }
  return false;
}

static bool call_sem_close (struct for_semact *call_sem_close_for_semact)
{
  if (call_sem_close_for_semact->sem_pass) {
#ifdef LONGJUMP_SUPPORT
    mem_signatures = &(call_sem_close_for_semact->mem_signatures);
#endif /* LONGJUMP_SUPPORT */

    if (call_sem_close_for_semact->sem_close)
      (*(call_sem_close_for_semact->sem_close)) ();

#ifdef LONGJUMP_SUPPORT
    sxmem_signatures_free ();
    mem_signatures = &(for_semact.mem_signatures);
#endif /* LONGJUMP_SUPPORT */
    return true;
  }
  return false;
}


static bool call_sem_open (struct for_semact *call_sem_open_for_semact)
{
  if (call_sem_open_for_semact->sem_pass) {
#ifdef LONGJUMP_SUPPORT
    mem_signatures = &(call_sem_open_for_semact->mem_signatures);
#endif /* LONGJUMP_SUPPORT */

    if (call_sem_open_for_semact->sem_open)
      (*(call_sem_open_for_semact->sem_open)) ();
    call_sem_open_for_semact->pass_nb = 0;

#ifdef LONGJUMP_SUPPORT
    sxmem_signatures_allocate (250);
    mem_signatures = &(for_semact.mem_signatures);
#endif /* LONGJUMP_SUPPORT */
    return true;
  }
  return false;
}

#ifdef LONGJUMP_SUPPORT
struct mem_signatures parser_mem_signatures;

void perform_free_after_long_jmp (void)
{
  mem_signatures = &(parser_mem_signatures);
  sxfree_mem_signatures_content ();

#ifdef weights
  mem_signatures = &(weights_for_semact.mem_signatures);
  weights_for_semact.pass_nb = 0;
  sxfree_mem_signatures_content ();
#endif /* weights */

#ifdef structure
  mem_signatures = &(structure_for_semact.mem_signatures);
  structure_for_semact.pass_nb = 0;
  sxfree_mem_signatures_content ();
#endif /* structure */

#ifdef chunker
  mem_signatures = &(chunker_for_semact.mem_signatures);
  chunker_for_semact.pass_nb = 0;
  sxfree_mem_signatures_content ();
#endif /* chunker */

#ifdef proper
  mem_signatures = &(proper_for_semact.mem_signatures);
  proper_for_semact.pass_nb = 0;
  sxfree_mem_signatures_content ();
#endif /* proper */

#ifdef usersem
  mem_signatures = &(usersem_for_semact.mem_signatures);
  usersem_for_semact.pass_nb = 0;
  sxfree_mem_signatures_content ();
#endif /* usersem */

#ifdef nbest
  mem_signatures = &(nbest_for_semact.mem_signatures);
  nbest_for_semact.pass_nb = 0;
  sxfree_mem_signatures_content ();
#endif /* nbest */

#ifdef exact_nbest
  mem_signatures = &(exact_nbest_for_semact.mem_signatures);
  exact_nbest_for_semact.pass_nb = 0;
  sxfree_mem_signatures_content ();
#endif /* exact_nbest */

#ifdef lfg
  mem_signatures = &(lfg_for_semact.mem_signatures);
  lfg_for_semact.pass_nb = 0;
  sxfree_mem_signatures_content ();
#endif /* lfg */
  
#ifdef output
  mem_signatures = &(output_for_semact.mem_signatures);
  output_for_semact.pass_nb = 0;
  sxfree_mem_signatures_content ();
#endif /* output */  

  sxerrmngr.err_size = 0;

  cur_spec = '\1';
}
#endif /* LONGJUMP_SUPPORT */

/* retourne le ME */
static char*
sxsem_mngr_ME (void)
{
  vstr_ME = varstr_alloc (64);

  if (weights_for_semact.ME) {
    if (varstr_length_not_null (vstr_ME))
      vstr_ME = varstr_catenate (vstr_ME, ", ");

    vstr_ME = varstr_catenate (vstr_ME, (*weights_for_semact.ME) ());
  }

  if (structure_for_semact.ME) {
    if (varstr_length_not_null (vstr_ME))
      vstr_ME = varstr_catenate (vstr_ME, ", ");

    vstr_ME = varstr_catenate (vstr_ME, (*structure_for_semact.ME) ());
  }

  if (chunker_for_semact.ME) {
    if (varstr_length_not_null (vstr_ME))
      vstr_ME = varstr_catenate (vstr_ME, ", ");

    vstr_ME = varstr_catenate (vstr_ME, (*chunker_for_semact.ME) ());
  }

  if (proper_for_semact.ME) {
    if (varstr_length_not_null (vstr_ME))
      vstr_ME = varstr_catenate (vstr_ME, ", ");

    vstr_ME = varstr_catenate (vstr_ME, (*proper_for_semact.ME) ());
  }

  if (usersem_for_semact.ME) {
    if (varstr_length_not_null (vstr_ME))
      vstr_ME = varstr_catenate (vstr_ME, ", ");

    vstr_ME = varstr_catenate (vstr_ME, (*usersem_for_semact.ME) ());
  }

  if (nbest_for_semact.ME) {
    if (varstr_length_not_null (vstr_ME))
      vstr_ME = varstr_catenate (vstr_ME, ", ");

    vstr_ME = varstr_catenate (vstr_ME, (*nbest_for_semact.ME) ());
  }

  if (exact_nbest_for_semact.ME) {
    if (varstr_length_not_null (vstr_ME))
      vstr_ME = varstr_catenate (vstr_ME, ", ");

    vstr_ME = varstr_catenate (vstr_ME, (*exact_nbest_for_semact.ME) ());
  }

  if (lfg_for_semact.ME) {
    if (varstr_length_not_null (vstr_ME))
      vstr_ME = varstr_catenate (vstr_ME, ", ");

    vstr_ME = varstr_catenate (vstr_ME, (*lfg_for_semact.ME) ());
  }
    
  if (output_for_semact.ME) {
    if (varstr_length_not_null (vstr_ME))
      vstr_ME = varstr_catenate (vstr_ME, ", ");

    vstr_ME = varstr_catenate (vstr_ME, (*output_for_semact.ME) ());
  }

  return varstr_tostr (vstr_ME);
}

/* retourne la chaine des arguments possibles propres a l'ensemble */
static char*
sxsem_mngr_args_usage (void)
{
  vstr_usage = varstr_alloc (512);

  vstr_usage = varstr_catenate (vstr_usage, Usage);

  if (weights_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*weights_for_semact.string_args) ());

  if (structure_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*structure_for_semact.string_args) ());

  if (chunker_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*chunker_for_semact.string_args) ());

  if (proper_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*proper_for_semact.string_args) ());

  if (usersem_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*usersem_for_semact.string_args) ());

  if (nbest_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*nbest_for_semact.string_args) ());

  if (exact_nbest_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*exact_nbest_for_semact.string_args) ());

  if (lfg_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*lfg_for_semact.string_args) ());

  if (output_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*output_for_semact.string_args) ());

  return varstr_tostr (vstr_usage);
}

/* decode les arguments specifiques au filtre */
/* l'option argv [*parg_num] est inconnue du parseur earley */
static bool
sxsem_mngr_args_decode (int *pargnum, int argc, char *argv[])
{
  switch (option_get_kind (argv [*pargnum])) {
  case MAP:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a map specification string must follow the \"%s\" option;\n", ME, option_get_text (MAP));
      return false;
    }

    map_spec = argv [*pargnum];
    break;
    
  case UNKNOWN_ARG:
    if (weights_for_semact.process_args && (*weights_for_semact.process_args) (pargnum, argc, argv))
      return true;

    if (structure_for_semact.process_args && (*structure_for_semact.process_args) (pargnum, argc, argv))
      return true;

    if (chunker_for_semact.process_args && (*chunker_for_semact.process_args) (pargnum, argc, argv))
      return true;

    if (proper_for_semact.process_args && (*proper_for_semact.process_args) (pargnum, argc, argv))
      return true;

    if (usersem_for_semact.process_args && (*usersem_for_semact.process_args) (pargnum, argc, argv))
      return true;

    if (nbest_for_semact.process_args && (*nbest_for_semact.process_args) (pargnum, argc, argv))
      return true;

    if (exact_nbest_for_semact.process_args && (*exact_nbest_for_semact.process_args) (pargnum, argc, argv))
      return true;

    if (lfg_for_semact.process_args && (*lfg_for_semact.process_args) (pargnum, argc, argv))
      return true;

    if (output_for_semact.process_args && (*output_for_semact.process_args) (pargnum, argc, argv))
      return true;

    return false;
  default: /* pour faire taire gcc -Wswitch-default */
    break;
  }

  return true;
}

static SXINT sxsem_mngr_sem_pass (void);
static void sxsem_mngr_init (void);

static void
sxsem_mngr_final (void)
{
#ifdef LONGJUMP_SUPPORT
  mem_signatures = &(for_semact.mem_signatures);
#endif /* LONGJUMP_SUPPORT */

    call_sem_final (&weights_for_semact);
    call_sem_final (&structure_for_semact);
    call_sem_final (&chunker_for_semact);
    call_sem_final (&nbest_for_semact);
    call_sem_final (&exact_nbest_for_semact);
    call_sem_final (&lfg_for_semact);
    call_sem_final (&output_for_semact);
    call_sem_final (&usersem_for_semact);

  cur_spec = '\1'; /* on pr�pare la phrase suivante */

#ifdef LONGJUMP_SUPPORT
  // on s'appr�te � rendre la main au parseur, on remet mem_signatures en cons�quence
  sxmem_signatures_raz ();
  mem_signatures = &(parser_mem_signatures);
#endif /* LONGJUMP_SUPPORT */
}

static void
sxsem_mngr_close (void)
{
#ifdef LONGJUMP_SUPPORT
  mem_signatures = &(for_semact.mem_signatures);
#endif /* LONGJUMP_SUPPORT */

    call_sem_close (&weights_for_semact);
    call_sem_close (&structure_for_semact);
    call_sem_close (&chunker_for_semact);
    call_sem_close (&nbest_for_semact);
    call_sem_close (&exact_nbest_for_semact);
    call_sem_close (&lfg_for_semact);
    call_sem_close (&output_for_semact);
    call_sem_close (&usersem_for_semact);

#ifdef LONGJUMP_SUPPORT
  // c'est fini, on arr�te le mode sp�cial
  sxmem_signatures_free ();
  mem_signatures = &(parser_mem_signatures);
  sxmem_signatures_free ();
  mem_signature_mode = false;
  mem_signatures = NULL;
#endif /* LONGJUMP_SUPPORT */
}

static void
raz_for_semact (void)
{
  for_semact.sem_open = NULL;
  for_semact.sem_init = NULL;
  for_semact.semact = NULL;
  for_semact.parsact = NULL;
  for_semact.prdct = NULL;
  for_semact.constraint = NULL;
  for_semact.sem_pass = NULL;
  for_semact.scanact = NULL;
  for_semact.rcvr = NULL;
  for_semact.rcvr_range_walk = NULL;
  for_semact.process_args = NULL;
  for_semact.string_args = NULL;
  for_semact.ME = NULL;
  for_semact.sem_final = NULL;
  for_semact.sem_close = NULL;
  for_semact.timeout_mngr = sxexit_timeout;
  for_semact.vtimeout_mngr = sxexit_timeout;
}

char*
get_cur_sem_name (void)
{
  switch (cur_spec) {
  case '\1':
    return "Earley";
  case 'W':
    return "Weights";
  case 'Z':
    return "Structure";
  case 'c':
  case 'C':
  case 'K':
    return "Chunker";
  case 'P':
    return "Proper";
  case 'l':
    return "LFG";
  case 'n':
    return "n-best";
#ifdef lfg
  case 'o':
  case 'x':
  case 'X':
  case 'E':
  case 'p':
  case 'd':
  case 'f':
  case 'F':
  case 't':
  case 'T':
  case 'i':
  case 'e':
  case 'w':
    return "LFG Output";
#else /* lfg */
  case 'o':
  case 'x':
  case 'X':
  case 'E':
  case 'p':
  case 'd':
  case 'f':
  case 'F':
  case 't':
  case 'T':
  case 'i':
  case 'b': /* option for Penn Treebank output */
  case 'G': /* option for dotty format output */
    return "Output";
#endif /* lfg */
#ifdef usersem
  case 'u':
  case 'U':
    return "User semantics";
#endif /* usersem */
  case '+':
  case '-':
  case 'r':
  case 's':
    return "Status stack modification";
  default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    fprintf (sxstderr, "\tTimeout in unknown semantics '%c'\n", cur_spec);
#endif
    return "Unknown module";
  }
  return "Between 2 semantics";
}

#if 0
static void
memoflw_mngr (void)
{
  char msg [32];

  SYNTAX_is_in_critical_zone = false;

  if (is_print_time) {
    sprintf (msg, "\tMemory overflow (%s)", get_cur_sem_name ());
    sxtime (SXACTION, msg); 
  }
}

static void
sxtrap_mngr (void)
{
  char msg [32];

  SYNTAX_is_in_critical_zone = false;

  if (is_print_time) {
    sprintf (msg, "\tInternal error (%s)", get_cur_sem_name ());
    sxtime (SXACTION, msg); 
  }
}
#endif

void jump_to_next_sentence_because_of_timeout (int signal_val);

static void
timeout_mngr (int sigid)
{
  char msg [32];

  (void) sigid;
  if (SYNTAX_is_in_critical_zone) {
#if EBUG
    fprintf (stderr, "### Timeout during critical zone: tiny timeout extension allowed\n");
#endif /* EBUG */
    sxcaught_virtual_timeout (0, 1, for_semact.vtimeout_mngr);
    return;
  }

  if (!mem_signature_mode)
    sxtrap (ME, "timeout_mngr (timeout outside the memory protection mode)");

  /* Pour avertir qui de droit !! */
  is_virtual_timeout_signal_raised = true;

  if (is_print_time) {
    sprintf (msg, "\tTimeout (%s)", get_cur_sem_name ());
    sxtime (SXACTION, msg); 
  }

  /* Il y a des cas ou` la re'cuperation du timeout met le processus en e'tat "sleep". */
  /* C'est en particulier le cas si le timeout s'est produit ds un free (le syste`me ayant positionne' un
     se'maphore).  Si sxsem_mngr_sem_pass appelle (une proc qui appelle) un free, ce free va
     rester bloque' en attente de la libe'ration du semaphore */
  if (cur_spec != '\1') { /* si on timeout dans Earley, aucune s�mantique catastrophe possible */
    /* Essai : dans tous les cas on ne donne que 1s de temps reel pour terminer ...
       meme si on est en attente de la liberation d'un semaphore */
    sxcaught_virtual_timeout (1, 0, jump_to_next_sentence_because_of_timeout);

    /* On cherche s'il y a du rattrapage possible dans la map */
    while ((cur_spec = *cur_map_spec++, cur_spec) && (cur_spec != '/'));

    if (cur_spec == SXNUL)
      (*cur_map_spec)--;
    else {
      if (cur_spec == '/') {
	/* ... oui */
	if (*cur_map_spec == 'r') {
	  /* Le rattrapage est de continuer ce qu'on etait en train de faire (pour au plus 1s) */
	  cur_map_spec += 2; /* On suppose qu'on a "/r/"
				on continuera donc derriere le rattrapage comme si de rien n'etait !! */
	  is_virtual_timeout_signal_raised = false;

	  return;
	}

	/* A VOIR : faut-il repartir de la ou laisse-t-on l'utilisateur gerer C,a ? */
	spf_restore_status (status_stack [1]);
	spf_pop_status ();

	if (is_print_time) {
	  sprintf (msg, "\tPop status (depth %i)", 1);
	  sxtime (SXACTION, msg);
	}

	sxsem_mngr_sem_pass ();
      }
    }
  }

  jump_to_next_sentence_because_of_timeout (0);
}

extern void (*free_after_long_jmp) (void);
extern jmp_buf environment_before_current_sentence;

void
jump_to_next_sentence_because_of_timeout (int signal_val)
{
  sxuse(signal_val); /* pour faire taire gcc -Wunused */
  longjmp (environment_before_current_sentence, 1);
}

#ifdef MEMOFLW_HANDLER
static void
jump_to_next_sentence_because_of_memoflw (void)
{
  memoflw_mngr ();
  longjmp (environment_before_current_sentence, 2);
}
#endif /* MEMOFLW_HANDLER */

#ifdef SXTRAP_HANDLER
static void
jump_to_next_sentence_because_of_sxtrap (void)
{
  sxtrap_mngr ();
  longjmp (environment_before_current_sentence, 3);
}
#endif /* SXTRAP_HANDLER */


/* OPEN de la s�mantique g�n�rale, appel� 1 fois avant la premi�re phrases en cas de TCUT */
static void
sxsem_mngr_open (void)
{
#ifdef LONGJUMP_SUPPORT
  free_after_long_jmp = perform_free_after_long_jmp;
#ifdef MEMOFLW_HANDLER
  sxgc_recovery = jump_to_next_sentence_because_of_memoflw;
#endif /* MEMOFLW_HANDLER */
#ifdef SXTRAP_HANDLER
  sxtrap_recovery = jump_to_next_sentence_because_of_sxtrap;
#endif /* SXTRAP_HANDLER */

  mem_signatures = &(for_semact.mem_signatures);
  sxmem_signatures_allocate (250);
#endif /* LONGJUMP_SUPPORT */

  call_sem_open (&weights_for_semact);
  call_sem_open (&structure_for_semact);
  call_sem_open (&chunker_for_semact);
  call_sem_open (&nbest_for_semact);
  call_sem_open (&exact_nbest_for_semact);
  call_sem_open (&lfg_for_semact);
  call_sem_open (&output_for_semact);
  call_sem_open (&usersem_for_semact);
  
  cur_spec = '\1'; /* si on timeout pendant Earley, on s'en aper�evra gr�ce � cur_spec = '\1' */

#ifdef LONGJUMP_SUPPORT
  // on s'appr�te � rendre la main au parseur, on met mem_signatures en cons�quence et on alloue parser_mem_signatures
  mem_signatures = &(parser_mem_signatures);
  sxmem_signatures_allocate (250);
#endif /* LONGJUMP_SUPPORT */
}


void
sxsem_mngr_semact (void)
{
  /* Ds le cas contraire, on prend les rcvr_spec de SEMANTICS qui a donc priorite' */
  /* On doit remplir rcvr_spec, structure qui dirige la facon dont va proceder la rcvr syntaxique */
  rcvr_spec.range_walk_kind = MIXED_FORWARD_RANGE_WALK_KIND; 
  rcvr_spec.perform_repair = true; /* On fait de la correction d'erreur ... */
  rcvr_spec.repair_kind = 1; /* ... mais on ne genere qu'une chaine de la longueur min */
  rcvr_spec.perform_repair_parse = true; /* On analyse cette chaine ... */
  rcvr_spec.repair_parse_kind = 1; /* ... mais on ne genere qu'une seule analyse */

  //  rcvr_spec.try_kind = TRY_MEDIUM; /* temporaire, � supprimer */

  /* On change les valeurs par defaut de l'analyseur earley */
  is_parse_forest = true;

  map_spec = map_spec_default; /* default == "l", sauf si option de compilation ad�quate */

#ifdef weights
  raz_for_semact ();
  weights_semact ();
  weights_for_semact = for_semact;
#endif /* weights */

#ifdef structure
  raz_for_semact ();
  structure_semact ();
  structure_for_semact = for_semact;
#endif /* structure */

#ifdef chunker
  raz_for_semact ();
  chunker_semact ();
  chunker_for_semact = for_semact;
#endif /* chunker */

#ifdef proper
  raz_for_semact ();
  proper_semact ();
  proper_for_semact = for_semact;
#endif /* proper */

#ifdef usersem
  raz_for_semact ();
  usersem ();
  usersem_for_semact = for_semact;
#endif /* usersem */

#ifdef nbest
  raz_for_semact ();
  nbest_semact ();
  nbest_for_semact = for_semact;
#endif /* nbest */

#ifdef exact_nbest
  raz_for_semact ();
  exact_nbest_semact ();
  exact_nbest_for_semact = for_semact;
#endif /* exact_nbest */

#ifdef lfg
  raz_for_semact ();
  lfg_semact ();
  lfg_for_semact = for_semact;
#endif /* lfg */
  
#ifdef output
  raz_for_semact ();
#ifdef lfg
  lfg_output_semact ();
#else /* lfg */
  output_semact ();
#endif /* lfg */
  output_for_semact = for_semact;
#endif /* output */  

  /* valeurs pour earley */
  raz_for_semact ();
  for_semact.sem_open = sxsem_mngr_open;
  for_semact.sem_init = sxsem_mngr_init;
  for_semact.sem_pass = sxsem_mngr_sem_pass;
  for_semact.process_args = sxsem_mngr_args_decode;
  for_semact.string_args = sxsem_mngr_args_usage;
  for_semact.ME = sxsem_mngr_ME;
  for_semact.sem_final = sxsem_mngr_final;
  for_semact.sem_close = sxsem_mngr_close;
#if defined(TIMEOUT_HANDLER) && TIMEOUT_HANDLER != 0
  for_semact.timeout_mngr = TIMEOUT_HANDLER;
  for_semact.vtimeout_mngr = TIMEOUT_HANDLER;
#else
  for_semact.timeout_mngr = timeout_mngr;
  for_semact.vtimeout_mngr = timeout_mngr;
#endif
}


static void
sxsem_mngr_init (void)
{
  char       msg [30];

#ifdef LONGJUMP_SUPPORT
    mem_signatures = &(for_semact.mem_signatures);
#endif /* LONGJUMP_SUPPORT */

  if (spf.outputG.start_symbol > 0) {
    /* for�t non vide */
    /* cur_spec vaut '\1', Earley a fini (avant le timeout) */
    status_stack_size = 0;
    status_stack = sxbm_calloc (16, spf.outputG.maxxprod+1);
    /* on push tout l'�tat originel de la for�t, qui sera restaur� en cas de timeout */
    spf_push_status ();
    spf_save_status (status_stack [++status_stack_size]);

    if (is_print_time) {
      sprintf (msg, "\tPush status (depth %ld)", status_stack_size);
      sxtime (SXACTION, msg);
    }
  }

  cur_map_spec = map_spec;

#ifdef LONGJUMP_SUPPORT
  // on s'appr�te � rendre la main au parseur, on remet mem_signatures en cons�quence
  mem_signatures = &(parser_mem_signatures);
#endif /* LONGJUMP_SUPPORT */
}

static SXINT
sxsem_mngr_sem_pass (void)
{
  SXINT        ret_val [16], if_depth;
  bool    do_sthg [16];
#if 0
  bool    else_passed [16];
#endif
  char       msg [30];
  SXBA       sxba_tmp;
  static SXINT skip_next_semantics = 0;

#ifdef LONGJUMP_SUPPORT
    mem_signatures = &(for_semact.mem_signatures);
#endif /* LONGJUMP_SUPPORT */

  do_sthg [0] = true;
  if_depth = 0;

  /* On epluche map_spec */
  while ((cur_spec = *cur_map_spec++)) {
    if (skip_next_semantics) {
      if (cur_spec == '/')
	skip_next_semantics = 0;
    }
    else {
      switch (cur_spec) {
      case '/':
	if (is_virtual_timeout_signal_raised)
	  /* Ici le "/" marque la fin des actions du mode panique
	     on retourne ds timeout_mngr (qui va faire sxexit (4)) */
	  return 1;

	skip_next_semantics = 1;
	break;

      case ' ':
	break;

      case '?':
      case '(':
	if_depth++;

	if (if_depth>=16)
	  sxtrap (ME, "sxsem_mngr_sem_pass (too much recursion in argument of -m option)");

#if 0
	else_passed [if_depth] = false;
#endif
	do_sthg [if_depth] = do_sthg [if_depth-1] && ret_val [if_depth-1];
	ret_val [if_depth] = 0;
	break;

      case ':':
      case '|':
#if 0
	else_passed [if_depth] = true;
#endif
	do_sthg [if_depth] = !do_sthg [if_depth] && do_sthg [if_depth-1];
	break;
      
      case ';':
      case ')':
	if_depth--;
	break;
      
      default:
	if (!do_sthg [if_depth])
	  break;

	switch (cur_spec) {
	case 'W':
#ifdef weights
	  weights_sem_pass_arg = 1;
	  ret_val [if_depth] = call_sem_pass (&weights_for_semact);
#else /* weights */
	  sxtrap (ME, "sxsem_mngr_sem_pass (this executable has been compiled without the -Dweights option ; you can't call the weights filter)");
#endif /* weights */
	  break;
	
	case 'Z':
#ifdef structure
	  structure_sem_pass_arg = 1;
	  ret_val [if_depth] = call_sem_pass (&structure_for_semact);
#else /* structure */
	  sxtrap (ME, "sxsem_mngr_sem_pass (this executable has been compiled without the -Dstructure option ; you can't call the structure filter)");
#endif /* structure */
	  break;
	
	case 'c':
#ifdef chunker
	  chunker_sem_pass_arg = 0;
	  ret_val [if_depth] = call_sem_pass (&chunker_for_semact);
#else /* chunker */
	  sxtrap (ME, "sxsem_mngr_sem_pass (this executable has been compiled without the -Dchunker option ; you can't call the chunker)");
#endif /* chunker */
	  break;
	
	case 'C':
#ifdef chunker
	  chunker_sem_pass_arg = 1;
	  ret_val [if_depth] = call_sem_pass (&chunker_for_semact);
#else /* chunker */
	  sxtrap (ME, "sxsem_mngr_sem_pass (this executable has been compiled without the -Dchunker option ; you can't call the chunker)");
#endif /* chunker */
	  break;
	
	case 'K':
#ifdef chunker
	  chunker_sem_pass_arg = 2;
	  ret_val [if_depth] = call_sem_pass (&chunker_for_semact);
#else /* chunker */
	  sxtrap (ME, "sxsem_mngr_sem_pass (this executable has been compiled without the -Dchunker option ; you can't call the chunker)");
#endif /* chunker */
	  break;
	
	case 'P':
#ifdef proper
	  proper_sem_pass_arg = 1;
	  ret_val [if_depth] = call_sem_pass (&proper_for_semact);
#else /* proper */
	  sxtrap (ME, "sxsem_mngr_sem_pass (this executable has been compiled without the -Dproper option ; you can't call the proper)");
#endif /* proper */
	  break;
	
	case 'u':
	case 'U':
#ifdef usersem
	  ret_val [if_depth] = call_sem_pass (&usersem_for_semact);
#else /* usersem */
	  sxtrap (ME, "sxsem_mngr_sem_pass (this executable has been compiled with the -Dusersem=... option ; you can't call any user semantics)");
#endif /* usersem */
	  break;
	
	case 'n':
#ifdef nbest
	  ret_val [if_depth] = call_sem_pass (&nbest_for_semact);
#else /* nbest */
	  sxtrap (ME, "sxsem_mngr_sem_pass (this executable has been compiled without the -Dnbest option ; you can't call the nbest filter)");
#endif /* nbest */
	  break;
	
	case 'N':
#ifdef exact_nbest
	  ret_val [if_depth] = call_sem_pass (&nbest_for_semact) && call_sem_pass (&exact_nbest_for_semact);
#else /* exact_nbest */
	  sxtrap (ME, "sxsem_mngr_sem_pass (this executable has been compiled without the -Dexact_nbest option ; you can't call the exact_nbest filter)");
#endif /* exact_nbest */
	  break;
	
	case 'l':
#ifdef lfg
	  ret_val [if_depth] = call_sem_pass (&lfg_for_semact);
#else /* lfg */
	  sxtrap (ME, "sxsem_mngr_sem_pass (this executable has been compiled without the -Dlfg option ; you can't call the lfg f-structure computation module)");
#endif /* lfg */
	  break;
	
	case 'p':
	case 'S':
	case 'e':
	case 'E':
	case 'x':
	case 'X':
	case 'w':
	case 'd':
	case 'f':
	case 'F':
	case 'i':
	case 't':
	case 'b': /* option for Penn TreeBank */
	case 'G': /* option for graphical output dotty format */
	  output_sem_pass_arg = cur_spec;
	  ret_val [if_depth] = call_sem_pass (&output_for_semact);
	  break;
	
	case '+':
	  ret_val [if_depth] = 0;
	
	  if (status_stack_size<15) {
	    spf_push_status ();
	    spf_save_status (status_stack [++status_stack_size]);

	    if (is_print_time) {
	      sprintf (msg, "\tPush status (depth %ld)", status_stack_size);
	      sxtime (SXACTION, msg);
	    }

	    ret_val [if_depth] = 1;
	  }
	
	  break;
	
	case '-':
	  ret_val [if_depth] = 0;
	
	  if (status_stack_size>1) { /* on preserve toujours l'etat 1, qui stocke l'etat initial restore par le cas timeout */
	    spf_restore_status (status_stack [status_stack_size--]);
	    spf_pop_status ();

	    if (is_print_time) {
	      sprintf (msg, "\tPop status (depth %ld)", status_stack_size);
	      sxtime (SXACTION, msg);
	    }

	    ret_val [if_depth] = 1;
	  }
	
	  break;
	
	case 'r':
	  ret_val [if_depth] = 0;
	
	  if (status_stack_size>=3 && status_stack_size<16) {
	    sxba_tmp = status_stack [status_stack_size];
	    status_stack [status_stack_size] = status_stack [status_stack_size-1];
	    status_stack [status_stack_size-1] = status_stack [status_stack_size-2];
	    status_stack [status_stack_size-2] = sxba_tmp;
	  
	    if (is_print_time) {
	      sprintf (msg, "\tRot3 status (depth %ld)", status_stack_size);
	      sxtime (SXACTION, msg);
	    }

	    ret_val [if_depth] = 1;
	  }
	
	  break;
	
	case 's':
	  ret_val [if_depth] = 0;
	
	  if (status_stack_size>=2 && status_stack_size<16) {
	    sxba_tmp = status_stack [status_stack_size];
	    status_stack [status_stack_size] = status_stack [status_stack_size-1];
	    status_stack [status_stack_size-1] = sxba_tmp;
	  
	    if (is_print_time) {
	      sprintf (msg, "\tSwap status (depth %ld)", status_stack_size);
	      sxtime (SXACTION, msg);
	    }

	    ret_val [if_depth] = 1;
	  }
	
	  break;
	default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
	  fprintf (sxstderr, "ERROR: unknown semantics %c\n", cur_spec);
	  sxtrap("sxsem_mngr","unknown switch #3");
#endif
	  break;
	
	}
      }

#ifdef LONGJUMP_SUPPORT
      mem_signatures = &(for_semact.mem_signatures);
#endif /* LONGJUMP_SUPPORT */
    }
  }

  if (!is_virtual_timeout_signal_raised) {
    if (status_stack) sxbm_free (status_stack), status_stack = NULL;

    if (vstr_ME) varstr_free (vstr_ME), vstr_ME = 0;
    if (vstr_usage) varstr_free (vstr_usage), vstr_usage = 0;
  }
  /* Sinon on va faire EXIT (4).  De plus on peut se bloquer ds les free si timeout
     a ete leve' ds un free !! */

#ifdef LONGJUMP_SUPPORT
  // on s'appr�te � rendre la main au parseur, on remet mem_signatures en cons�quence
  mem_signatures = &(parser_mem_signatures);
#endif /* LONGJUMP_SUPPORT */

  return 1;
}

