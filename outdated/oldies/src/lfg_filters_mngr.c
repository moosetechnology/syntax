/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2006 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */

/* Programme qui organise les filtres eventuels entre un analyseur earley et l'analyseur LFG */
/* Le source est sous forme de DAG */


/* ********************************************************
   *                                                      *
   *          Produit de l'equipe ATOLL.                  *
   *                                                      *
   ******************************************************** */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 31-03-06 11:00 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

static char	ME [] = "lfg_filter_mngr";

#include "sxunix.h"
#include "earley.h"
#include "sxspf.h"
#include "varstr.h"

char WHAT_LFG_FILTERS_MNGR[] = "@(#)SYNTAX - $Id: lfg_filters_mngr.c 3678 2024-02-06 08:38:24Z garavel $" WHAT_DEBUG;

static struct for_semact weights_for_semact, structure_for_semact, chunker_for_semact, proper_for_semact, nbest_for_semact, lfg_for_semact, output_for_semact;
static VARSTR            vstr_usage, vstr_ME;
static char              *map_spec;
static char              map_spec_default [] = "l"; /* uniquement lfg */

static char              cur_spec = '\1';
static SXBA              *status_stack;

extern void              sxexit_timeout ();
extern void              output_semact ();
#ifndef no_lfg
extern void              lfg_semact ();
#endif /* !no_lfg */
#ifndef no_nbest
extern void              nbest_semact ();
#endif /* !no_nbest */
#ifndef no_weights
extern void              weights_semact ();
#endif /* !no_weights */
#ifndef no_structure
extern void              structure_semact ();
#endif /* !no_structure */
#ifndef no_chunker
extern void              chunker_semact ();
#endif /* !no_chunker */
#ifndef no_proper
extern void              proper_semact ();
#endif /* !no_proper */

static char	Usage [] = "\
\t\t-m spec, -map spec (default -map l)\n\
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
char        output_sem_pass_arg;

static char	*option_tbl [] = {
    "",
    "m", "map",
};

static int	option_kind [] = {
    UNKNOWN_ARG,
    MAP, MAP,
};

static int	option_get_kind (arg)
    char	*arg;
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



static char	*option_get_text (kind)
    int	kind;
{
  int	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}

static bool call_sem_pass (call_sem_pass_for_semact)
     struct for_semact    *call_sem_pass_for_semact;
{
  if (call_sem_pass_for_semact->sem_pass) {
    if (call_sem_pass_for_semact->pass_nb && call_sem_pass_for_semact->sem_final)
      (*(call_sem_pass_for_semact->sem_final)) ();
    call_sem_pass_for_semact->pass_nb++;
    return (*(call_sem_pass_for_semact->sem_pass)) ();
  }
  return false;
}

static bool call_sem_final (call_sem_final_for_semact)
     struct for_semact    *call_sem_final_for_semact;
{
  if (call_sem_final_for_semact->sem_pass && call_sem_final_for_semact->pass_nb && call_sem_final_for_semact->sem_final) {
    (*(call_sem_final_for_semact->sem_final)) ();
    return true;
  }
  return false;
}

/* retourne le ME */
static char*
lfg_filter_mngr_ME ()
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

  if (nbest_for_semact.ME) {
    if (varstr_length_not_null (vstr_ME))
      vstr_ME = varstr_catenate (vstr_ME, ", ");

    vstr_ME = varstr_catenate (vstr_ME, (*nbest_for_semact.ME) ());
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
lfg_filter_mngr_args_usage ()
{
  vstr_usage = varstr_alloc (512);

  vstr_usage = varstr_catenate (vstr_usage, Usage);

  if (weights_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*weights_for_semact.string_args) ());

  if (structure_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*structure_for_semact.string_args) ());

  if (chunker_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*chunker_for_semact.string_args) ());

  if (nbest_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*nbest_for_semact.string_args) ());

  if (lfg_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*lfg_for_semact.string_args) ());

  if (output_for_semact.string_args)
    vstr_usage = varstr_catenate (vstr_usage, (*output_for_semact.string_args) ());

  return varstr_tostr (vstr_usage);
}

/* decode les arguments specifiques au filtre */
/* l'option argv [*parg_num] est inconnue du parseur earley */
static bool
lfg_filter_mngr_args_decode (pargnum, argc, argv)
     int  *pargnum, argc;
     char *argv [];
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

    if (nbest_for_semact.process_args && (*nbest_for_semact.process_args) (pargnum, argc, argv))
      return true;

    if (lfg_for_semact.process_args && (*lfg_for_semact.process_args) (pargnum, argc, argv))
      return true;

    if (output_for_semact.process_args && (*output_for_semact.process_args) (pargnum, argc, argv))
      return true;

    return false;
  default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap(ME,"unknown switch case #1");
#endif
    break;
  }

  return true;
}

static int lfg_filter_mngr_sem_pass ();

void sem_final ()
{
    call_sem_final (&weights_for_semact);
    call_sem_final (&structure_for_semact);
    call_sem_final (&chunker_for_semact);
    call_sem_final (&nbest_for_semact);
    call_sem_final (&lfg_for_semact);
    call_sem_final (&output_for_semact);
}

static void
raz_for_semact ()
{
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
  for_semact.timeout_mngr = sxexit_timeout;
  for_semact.vtimeout_mngr = sxexit_timeout;
}

void
timeout_mngr ()
{
  char semname [32] = "Between 2 semantics";
  char msg [32];

  /* Pour avertir qui de droit !! */
  is_virtual_timeout_signal_raised = true;

  if (is_print_time) {
    switch (cur_spec) {
    case '\1':
      sprintf (semname, "Earley");
      break;
    case 'W':
      sprintf (semname, "Weights");
      break;
    case 'Z':
      sprintf (semname, "Structure");
      break;
    case 'c':
    case 'C':
    case 'K':
      sprintf (semname, "Chunker");
      break;
    case 'P':
      sprintf (semname, "Proper");
      break;
    case 'l':
      sprintf (semname, "LFG");
      break;
    case 'n':
      sprintf (semname, "n-best");
      break;
    case 'o':
    case 'x':
    case 'X':
    case 'e':
    case 'E':
    case 'p':
    case 'd':
    case 'f':
      sprintf (semname, "Output");
      break;
    case '+':
    case '-':
    case 'r':
    case 's':
      sprintf (semname, "Status stack modification");
      break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap(ME,"unknown switch case #2");
#endif
      break;
    }

    sprintf (msg, "\tTimeout (%s)", semname);
    sxtime (SXACTION, msg); 
  }

  /* Il y a des cas ou` la re'cuperation du timeout met le processus en e'tat "sleep". */
  /* C'est en particulier le cas ou le timeout s'est produit ds un free (le syste`me ayant positionne' un
     se'maphore).  Si lfg_filter_mngr_sem_pass appelle (une proc qui appelle) un free, ce free va
     rester bloque' en attente de la libe'ration du semaphore */
  if (cur_spec != '\1') { /* si on timeout dans Earley, aucune sémantique catastrophe possible */
    /* Essai : dans tous les cas on ne donne que 1s de temps reel pour terminer ...
       meme si on est en attente de la liberation d'un semaphore */
    sxtimeout (1);

    /* On cherche s'il y a du rattrapage */
    while ((cur_spec = *map_spec++, cur_spec) && (cur_spec != '/'));

    if (cur_spec == '/') {
      /* ... oui */
      if (*map_spec == 'r') {
	/* Le rattrapage est de continuer ce qu'on etait en train de faire (pour au plus 1s) */
	map_spec += 2; /* On suppose qu'on a "/r/"
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

      lfg_filter_mngr_sem_pass ();
    }
  }

  sxexit (4);
}

void
lfg_filter_mngr_semact ()
{

  /* Ds le cas contraire, on prend les rcvr_spec de SEMANTICS qui a donc priorite' */
  /* On doit remplir rcvr_spec, structure qui dirige la facon dont va proceder la rcvr syntaxique */
  rcvr_spec.range_walk_kind = MIXED_FORWARD_RANGE_WALK_KIND; 
  rcvr_spec.perform_repair = true; /* On fait de la correction d'erreur ... */
  rcvr_spec.repair_kind = 1; /* ... mais on ne genere qu'une chaine de la longueur min */
  rcvr_spec.perform_repair_parse = false; /* On analyse cette chaine ... */
  rcvr_spec.repair_parse_kind = 1; /* ... mais on ne genere qu'une seule analyse */

  //  rcvr_spec.try_kind = TRY_MEDIUM; /* temporaire, à supprimer */

  /* On change les valeurs par defaut de l'analyseur earley */
  is_parse_forest = true;

  map_spec = map_spec_default; /* default == "l" */

#ifndef no_weights
  raz_for_semact ();
  weights_semact ();
  weights_for_semact = for_semact;
#endif /* !no_weights */

#ifndef no_structure
  raz_for_semact ();
  structure_semact ();
  structure_for_semact = for_semact;
#endif /* !no_structure */

#ifndef no_chunker
  raz_for_semact ();
  chunker_semact ();
  chunker_for_semact = for_semact;
#endif /* !no_chunker */

#ifndef no_proper
  raz_for_semact ();
  proper_semact ();
  proper_for_semact = for_semact;
#endif /* !no_proper */

#ifndef no_nbest
  raz_for_semact ();
  nbest_semact ();
  nbest_for_semact = for_semact;
#endif /* !no_nbest */

#ifndef no_lfg
  raz_for_semact ();
  lfg_semact ();
  lfg_for_semact = for_semact;
#endif /* !no_lfg */
  
  raz_for_semact ();
  output_semact ();
  output_for_semact = for_semact;
  
  /* valeurs pour earley */
  raz_for_semact ();
  for_semact.sem_pass = lfg_filter_mngr_sem_pass;
  for_semact.process_args = lfg_filter_mngr_args_decode;
  for_semact.string_args = lfg_filter_mngr_args_usage;
  for_semact.ME = lfg_filter_mngr_ME;
  for_semact.sem_final = sem_final;
  for_semact.timeout_mngr = timeout_mngr;
  for_semact.vtimeout_mngr = timeout_mngr;
}


static int
lfg_filter_mngr_sem_pass ()
{
  int        ret_val [16], if_depth;
  static int status_stack_size;
  bool    do_sthg [16], else_passed [16];
  char       msg [30];
  SXBA       sxba_tmp;
  static int skip_next_semantics = 0;

  do_sthg [0] = true;
  if_depth = 0;

  if (cur_spec == '\1') { /* initialisation des sémantiques ; Earley a fini (avant le timeout) */
    status_stack_size = 0;
    status_stack = sxbm_calloc (16, spf.outputG.maxxprod+1);
    /* on push tout l'état originel de la forêt, qui sera restauré en cas de timeout */
    spf_push_status ();
    spf_save_status (status_stack [++status_stack_size]);

    if (is_print_time) {
      sprintf (msg, "\tPush status (depth %i)", status_stack_size);
      sxtime (SXACTION, msg);
    }
  }

  /* On epluche map_spec */
  while ((cur_spec = *map_spec++)) {
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
	  sxtrap (ME, "lfg_filter_mngr_sem_pass (too much recursion in argument of -m option)");

	else_passed [if_depth] = false;
	do_sthg [if_depth] = do_sthg [if_depth-1] && ret_val [if_depth-1];
	ret_val [if_depth] = 0;
	break;

      case ':':
      case '|':
	else_passed [if_depth] = true;
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
#ifndef no_weights
	  weights_sem_pass_arg = 1;
	  ret_val [if_depth] = call_sem_pass (&weights_for_semact);
#else /* !no_weights */
	  sxtrap (ME, "lfg_filter_mngr_sem_pass (this executable has been compiled with the -Dno_weights option ; you can't call the weights filter)");
#endif /* !no_weights */
	  break;
	
	case 'Z':
#ifndef no_structure
	  structure_sem_pass_arg = 1;
	  ret_val [if_depth] = call_sem_pass (&structure_for_semact);
#else /* !no_structure */
	  sxtrap (ME, "lfg_filter_mngr_sem_pass (this executable has been compiled with the -Dno_structure option ; you can't call the structure filter)");
#endif /* !no_structure */
	  break;
	
	case 'c':
#ifndef no_chunker
	  chunker_sem_pass_arg = 0;
	  ret_val [if_depth] = call_sem_pass (&chunker_for_semact);
#else /* !no_chunker */
	  sxtrap (ME, "lfg_filter_mngr_sem_pass (this executable has been compiled with the -Dno_chunker option ; you can't call the chunker)");
#endif /* !no_chunker */
	  break;
	
	case 'C':
#ifndef no_chunker
	  chunker_sem_pass_arg = 1;
	  ret_val [if_depth] = call_sem_pass (&chunker_for_semact);
#else /* !no_chunker */
	  sxtrap (ME, "lfg_filter_mngr_sem_pass (this executable has been compiled with the -Dno_chunker option ; you can't call the chunker)");
#endif /* !no_chunker */
	  break;
	
	case 'K':
#ifndef no_chunker
	  chunker_sem_pass_arg = 2;
	  ret_val [if_depth] = call_sem_pass (&chunker_for_semact);
#else /* !no_chunker */
	  sxtrap (ME, "lfg_filter_mngr_sem_pass (this executable has been compiled with the -Dno_chunker option ; you can't call the chunker)");
#endif /* !no_chunker */
	  break;
	
	case 'P':
#ifndef no_proper
	  proper_sem_pass_arg = 1;
	  ret_val [if_depth] = call_sem_pass (&proper_for_semact);
#else /* !no_proper */
	  sxtrap (ME, "lfg_filter_mngr_sem_pass (this executable has been compiled with the -Dno_proper option ; you can't call the proper)");
#endif /* !no_proper */
	  break;
	
	case 'n':
#ifndef no_nbest
	  ret_val [if_depth] = call_sem_pass (&nbest_for_semact);
#else /* !no_nbest */
	  sxtrap (ME, "lfg_filter_mngr_sem_pass (this executable has been compiled with the -Dno_nbest option ; you can't call the nbest filter)");
#endif /* !no_nbest */
	  break;
	
	case 'l':
#ifndef no_lfg
	  ret_val [if_depth] = call_sem_pass (&lfg_for_semact);
#else /* !no_lfg */
	  sxtrap (ME, "lfg_filter_mngr_sem_pass (this executable has been compiled with the -Dno_lfg option ; you can't call the lfg f-structure computation module)");
#endif /* !no_lfg */
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
	  output_sem_pass_arg = cur_spec;
	  ret_val [if_depth] = call_sem_pass (&output_for_semact);
	  break;
	
	case '+':
	  ret_val [if_depth] = 0;
	
	  if (status_stack_size<15) {
	    spf_push_status ();
	    spf_save_status (status_stack [++status_stack_size]);

	    if (is_print_time) {
	      sprintf (msg, "\tPush status (depth %i)", status_stack_size);
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
	      sprintf (msg, "\tPop status (depth %i)", status_stack_size);
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
	      sprintf (msg, "\tRot3 status (depth %i)", status_stack_size);
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
	      sprintf (msg, "\tSwap status (depth %i)", status_stack_size);
	      sxtime (SXACTION, msg);
	    }

	    ret_val [if_depth] = 1;
	  }
	
	  break;
	default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
	  sxtrap("lfg_filters_mngr","unknown switch #3");
#endif
	  break;
	
	}
      }
    }
  }

  if (!is_virtual_timeout_signal_raised) {
    sxbm_free (status_stack);

    if (vstr_ME) varstr_free (vstr_ME), vstr_ME = 0;
    if (vstr_usage) varstr_free (vstr_usage), vstr_usage = 0;
  }
  /* Sinon on va faire sxexit (4).  De plus on peut se bloquer ds les free si timeout
     a ete leve' ds un free !! */

  return 1;
}

