/* ********************************************************
 *                                                      *
 *                                                      *
 * Copyright (c) 2007 by Institut National de Recherche *
 *                    en Informatique et en Automatique *
 *                                                      *
 *                                                      *
 ******************************************************** */




/* ********************************************************
 *                                                        *
 *     Produit de l'equipe ATOLL.			  *
 *                                                        *
 ******************************************************** */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Lun 13 Aout 2007 09:30:	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"

char WHAT_SXDAG_MNGR[] = "@(#)SYNTAX - $Id: fsa_mngr.c 1214 2008-03-13 12:49:51Z rlacroix $" WHAT_DEBUG;

static char	ME [] = "fsa_mngr";

#include "varstr.h"
#include "XxY.h"
#include "fsa.h"
#include "sxstack.h"
#include "XH.h"
#include "sxdico.h"


static SXINT     *old_state2max_path_lgth, *old_state2new_state, normalized_fsa_final_state;
static SXBA      state_set;
static void      (*normalized_fsa_fill_trans)(SXINT , SXINT, SXINT);
static void      (*normalized_fsa_forward_trans) (SXINT , void (*fill_old_state2max_path_lgth)(SXINT, SXINT, SXINT) );
static BOOLEAN   normalized_is_cyclic;

static void
fill_old_state2max_path_lgth (SXINT prev_old_state, SXINT t, SXINT next_old_state)
{
  SXINT old_prev_max_path_lgth, old_next_max_path_lgth;

  sxuse (t);
  if (SXBA_bit_is_reset_set (state_set, next_old_state)) {
    /* Pas de cycle sur next_old_state */
    old_prev_max_path_lgth = old_state2max_path_lgth [prev_old_state];
    old_next_max_path_lgth = old_state2max_path_lgth [next_old_state];

    if (old_next_max_path_lgth == 0 || old_next_max_path_lgth <= old_prev_max_path_lgth) {
      /* C'est la 1ere fois qu'on tombe sur next_old_state */
      /* On a deja atteint next_old_state par un autre chemin "normal" mais plus court... */
      old_state2max_path_lgth [next_old_state] = old_prev_max_path_lgth+1;
      (*normalized_fsa_forward_trans)(next_old_state, fill_old_state2max_path_lgth);
    }
    /*
      else
      rien
    */

    SXBA_0_bit (state_set, next_old_state);
  }
  else {
    /*
      Detection d'un cycle ds l'automate
      On ne touche pas a old_state2max_path_lgth [next_old_state]
    */
    normalized_is_cyclic = TRUE;
  }
}


#if EBUG
static void
check_old_state2max_path_lgth (SXINT prev_old_state, SXINT t, SXINT next_old_state)
{
  sxuse (t);
  if (old_state2max_path_lgth [prev_old_state] >= old_state2max_path_lgth [next_old_state])
    sxtrap (ME, "check_old_state2max_path_lgth (erroneous path length)");
}
#endif /* EBUG */

static int
cmp_old_state (const void *p1, const void *p2)
{  
  SXINT state1, state2, lgth1, lgth2;

  /* The actual arguments to this function are "pointers to SXINT", hence the following cast plus dereference */
  state1 = (SXINT) * (SXINT * const *) p1;
  state2 = (SXINT) * (SXINT * const *) p2;

  lgth1 = old_state2max_path_lgth [state1];
  lgth2 = old_state2max_path_lgth [state2];

  return (lgth1 < lgth2) ? -1 /* Le nouvel etat de state1 va etre inferieur au nouvel etat de state2 */
    : ((lgth1 > lgth2) ? 1 /* Le nouvel etat de state1 va etre superieur au nouvel etat de state2 */
       : ((state1 < state2) ? -1 : 1) /* Ici state1 et state2 se trouvent a la meme distance de init_state (sur des chemins differents) On conserve l'ordre initial */
       );
}


static SXINT prev_from_new_state;
static SXBA  to_new_state_set, *to_new_state2t_set;
static SXINT *to_new_state2t;

/* On accumule les trans depuis prev_from_new_state pour pouvoir les sortir par q croissant */
static void
save_a_trans (SXINT t, SXINT q)
{
  SXINT old_t;
  SXBA  t_set;

  if (SXBA_bit_is_reset_set (to_new_state_set, q)) {
    to_new_state2t [q] = t;
  }
  else {
    /* transition multiple vers le meme q, on les trie par t croissant */
    old_t = to_new_state2t [q];
    t_set = to_new_state2t_set [q];

    if (old_t) {
      to_new_state2t [q] = 0;
      SXBA_1_bit (t_set, old_t);
      
    }

    SXBA_1_bit (t_set, t);
  }
}

static void
output_from_prev (void)
{
  SXINT q, t;
  SXBA  t_set;

  q = -1;

  while ((q = sxba_scan_reset (to_new_state_set, q)) >= 0) {
    if ((t = to_new_state2t [q]))
      (*normalized_fsa_fill_trans) (prev_from_new_state, t, q);
    else {
      t_set = to_new_state2t_set [q];
      t = -1;

      while ((t = sxba_scan_reset (t_set, t)) >= 0)
	(*normalized_fsa_fill_trans) (prev_from_new_state, t, q);
    }
  }
}


static SXINT
output_new_trans (SXINT from_old_state, SXINT t, SXINT to_old_state)
{
  SXINT        from_new_state, to_new_state;

  from_new_state = old_state2new_state [from_old_state];
  to_new_state = old_state2new_state [to_old_state];

  //  if (to_new_state == normalized_fsa_final_state)
  //    t = -1;

  if (prev_from_new_state == from_new_state) {
    /* On a deja trouve' une transition depuis from_new_state */
  }
  else {
    /* 1ere trans depuis from_old_state */
    if (prev_from_new_state) {
      /* On traite donc les transitions depuis prev_from_new_state */
      output_from_prev ();
    }

    prev_from_new_state = from_new_state;
  }

  save_a_trans (t, to_new_state);

  return 1;
}


/* Permet une renumerotation raisonnable des etats d'un FSA a partir de init_state>= 0
   En particulier pour un DAG si p ->t q, on a p < q
*/
/* fsa_forward_trans est la fonction de l'appelant qui permet de parcourir le vieil FSA 
   fsa_fill_trans permet de construire le FSA renumerote' */
/* (*fsa_forward_trans)(p, f)  appelle (*f)(p, t, q) pour tout p ->t q
   Pour les valeurs croissantes de p', fsa_normalize () appelle (*fsa_fill_trans)(p', t', q') pour toute transition p' ->t' q' renumerotee
*/
void
fsa_normalize (SXINT init_state, 
	       SXINT final_state, 
	       SXINT eof_ste,
	       void (*fsa_forward_trans)(SXINT ,void (*)(SXINT, SXINT, SXINT)), 
	       void (*fsa_fill_trans)(SXINT , SXINT, SXINT))
{
  SXINT state, new_state;
  SXINT *to_be_sorted;

  normalized_fsa_forward_trans = fsa_forward_trans;
  normalized_fsa_fill_trans = fsa_fill_trans;
  normalized_fsa_final_state = final_state;

  old_state2max_path_lgth = (SXINT*) sxcalloc (final_state+1, sizeof (SXINT));
  state_set = sxba_calloc (final_state+1);

  SXBA_1_bit (state_set, init_state);
  old_state2max_path_lgth [init_state] = 1; /* pour reserver 0 a non encore traite' */

  /* On calcule la longueur maximale des chemins qui conduisent depuis l'etat initial a chaque etat de l'automate ... */
  normalized_is_cyclic = FALSE;
  (*normalized_fsa_forward_trans)(init_state, fill_old_state2max_path_lgth);

  /* Attention, si l'automate en entree contient des cycles (c'est pas un DAG), l'algo precedent n'assure pas
     que l'etat final sera a` la distance max de l'etat initial, on y remedie */
  old_state2max_path_lgth [final_state] = final_state;

#if EBUG
  /* petite verif */
  if (!normalized_is_cyclic)
    (*normalized_fsa_forward_trans)(init_state, check_old_state2max_path_lgth);
#endif /* EBUG */

  /* ... On trie ces etats par ces longueurs ... */
  sxfree (state_set), state_set = NULL;
  to_be_sorted = (SXINT*) sxalloc (final_state+1, sizeof (SXINT));

  for (state = 0; state <= final_state; state++)
    to_be_sorted [state] = state;

  /* Ici old_state2max_path_lgth est rempli, on va donc trier */
  /*
    NAME
    qsort - sorts an array

    SYNOPSIS
    #include <stdlib.h>

    void qsort(void *base, size_t nmemb, size_t size,
    int(*compar)(const void *, const void *));

    DESCRIPTION
    The  qsort()  function sorts an array with nmemb elements of size size.
    The base argument points to the start of the array.

    The contents of the array are sorted in ascending order according to  a
    comparison  function  pointed  to  by  compar, which is called with two
    arguments that point to the objects being compared.

    The comparison function must return an integer less than, equal to,  or
    greater  than  zero  if  the first argument is considered to be respec-
    tively less than, equal to, or greater than the second.  If two members
    compare as equal, their order in the sorted array is undefined.

    RETURN VALUE
    The qsort() function returns no value.
  */

  qsort(to_be_sorted+init_state, final_state-init_state+1, sizeof (SXINT), cmp_old_state);

  sxfree (old_state2max_path_lgth), old_state2max_path_lgth = NULL;

  /* Pour des questions d'ordre de sortie, on a besoin de la correspondance old_state -> new_state */
  old_state2new_state = (SXINT*) sxalloc (final_state+1, sizeof (SXINT));

  for (new_state = init_state; new_state <= final_state; new_state++) {
    state = to_be_sorted [new_state];
    old_state2new_state [state] = new_state;
  }

  /* ... et on sort l'automate "normalise' */
  /* par ordre croissant des etats d'origine de chaque transition (pour chaque p, on sort tous les p ->t q) */
  to_new_state_set = sxba_calloc (final_state+1);
  to_new_state2t = (SXINT *) sxalloc (final_state+1, sizeof (SXINT));
  to_new_state2t_set = sxbm_calloc (final_state+1, eof_ste+1);

  for (new_state = init_state; new_state < final_state; new_state++) {
    (*normalized_fsa_forward_trans)(
				    to_be_sorted [new_state], 
				    (void (*)(SXINT, SXINT, SXINT))output_new_trans);
    /* nous utilisons un cast pour le pointeur de fonction 'output_new_trans' car son prototype
       ne correspond pas parfaitement a l'argument #2 de 'normalized_fsa_forward_trans' :
       il retourne 'SXINT' au lieu de 'void' */
  }

  /* ... Et les dernieres transitions */
  output_from_prev ();

  sxfree (to_new_state_set), to_new_state_set = NULL;
  sxfree (to_new_state2t), to_new_state2t = NULL;
  sxbm_free (to_new_state2t_set), to_new_state2t_set = NULL;
    
  sxfree (to_be_sorted), to_be_sorted = NULL;
  sxfree (old_state2new_state);
}

/* ******************************************* nfa2dfa ******************************************* */
static SXINT        nfa_init_state, nfa_final_state, dfa_init_state, dfa_final_state;
static BOOLEAN    (*nfa_empty_trans) (SXINT, SXBA);
static SXINT        last_partition, dfa_state_nb;
static SXINT        *nfa_ec_stack, *nfa_t_stack, *nfa_state_stack;
static SXBA       nfa_wstate_set, t_set;
static SXBA       *t2nfa_state_set;
static XH_header  XH_dfa_states;


static SXINT        *t2pos, *pos2state, *part_set_id2part;
static XH_header  XH_trans_list_id, XH_t_trans, XH_part_set;
static SXINT        *dfa_state2pred_nb;
static XxY_header XxY_out_trans;
static SXINT        *XxY_out_trans2next_state;
static SXINT        **dfa_state2pred_stack;

static struct state2attr {
  SXINT next_state, prev_state, part, init_part, trans_list_id, temp_id;
} *state2attr;

static struct part2attr {
  SXINT state, card, sub_card;
  SXINT *tbp_state_stack;
} *part2attr;

/* L'etat state change de partition et vient ds part */
static void
move (SXINT state, SXINT part)
{
  SXINT               next_state, prev_state;
  struct state2attr *attr_ptr;
  struct part2attr  *old_part_ptr, *new_part_ptr;

  attr_ptr = state2attr+state;
  next_state = attr_ptr->next_state;
  prev_state = attr_ptr->prev_state;
  
  old_part_ptr = part2attr + attr_ptr->part;

  old_part_ptr->card--;

  if (prev_state == 0)
    old_part_ptr->state = next_state;
  else
    state2attr [prev_state].next_state = next_state;

  if (next_state)
    state2attr [next_state].prev_state = prev_state;

  new_part_ptr = part2attr + part;
  new_part_ptr->card++;

  next_state = new_part_ptr->state;

  new_part_ptr->state = state;

  attr_ptr->next_state = next_state;
  attr_ptr->prev_state = 0;
  state2attr [next_state].prev_state = state;

  attr_ptr->part = part;
}


/* On met l'etat state dans la partition part */
static void
init_set (SXINT state, SXINT part)
{
  SXINT               old_state;
  struct state2attr *attr_ptr;
  struct part2attr  *part_ptr;

  part_ptr = part2attr + part;
  old_state = part_ptr->state;
  part_ptr->card++;
  part_ptr->state = state;

  attr_ptr = state2attr+state;
  attr_ptr->part = part;
  attr_ptr->next_state = old_state;
  attr_ptr->prev_state = 0;

  if (old_state)
    state2attr [old_state].prev_state = state;
}


/* On a un NFSA qui a des transitions vides */
/* On passe en arg un ensembles d'etats, on complete cet ensemble avec les etats accessibles par transition vide
   l'ensemble des etats atteint par transition vide depuis l'etat p est donne' par (*empty_trans)(p, reached_set) */
/* Retourne vrai ssi state_set a change' */
static BOOLEAN
epsilon_closure (SXBA cur_state_set)
{
  SXINT     state, next_state;
  BOOLEAN ret_val = FALSE;

  state = nfa_init_state-1;

  while ((state = sxba_scan (cur_state_set, state)) >= 0)
    PUSH (nfa_ec_stack, state);

  while (!IS_EMPTY (nfa_ec_stack)) {
    state = POP (nfa_ec_stack);

    if ((*nfa_empty_trans)(state, nfa_wstate_set)) {
      /* wstate_set est non vide */
      next_state = nfa_init_state-1;

      while ((next_state = sxba_scan_reset (nfa_wstate_set, next_state)) >= 0) {
	if (SXBA_bit_is_reset_set (cur_state_set, next_state)) {
	  ret_val = TRUE;
	  PUSH (nfa_ec_stack, next_state);
	}
      }
    }
  }

  return ret_val;
}

static void
dfa_extract_trans (SXINT dfa_state, void (*dfa_fill_trans) (SXINT, SXINT, SXINT))
{
  SXINT stateXt;

  XxY_Xforeach (XxY_out_trans, dfa_state, stateXt)
    (*dfa_fill_trans) (dfa_state, XxY_Y (XxY_out_trans, stateXt), XxY_out_trans2next_state [stateXt]);
}

static void
dfa_fill_t_trans (SXINT dfa_state, SXINT t, SXINT next_dfa_state)
{
  sxuse (dfa_state);
  sxuse (next_dfa_state);
  SXBA_1_bit (t_set, t);
}

static void
dfa_fill_full_trans (SXINT dfa_state, SXINT t, SXINT next_dfa_state)
{
  sxuse (dfa_state);
  pos2state [t2pos [t]] = next_dfa_state;

  if (dfa_state2pred_nb) {
    /* On en profite pour compter les predecesseurs de next_dfa_state */
    dfa_state2pred_nb [next_dfa_state]++;
    dfa_state2pred_nb [0]++;
  }
}

static void
dfa_fill_dfa_state2pred_stack (SXINT dfa_state, SXINT t, SXINT next_dfa_state)
{
  sxuse (t);
  PUSH (dfa_state2pred_stack [next_dfa_state], dfa_state);
}

static void
fill_init_min (SXINT state, 
	       void (*cur_dfa_extract_trans)(SXINT, void (*dfa_fill_t_trans) (SXINT dfa_state, SXINT t, SXINT next_dfa_state)))
{
  SXINT t, pos, i, t_trans;

  (*cur_dfa_extract_trans)(state, dfa_fill_t_trans);

  t = -1;
  pos = 0;

  while ((t = sxba_scan_reset (t_set, t)) >= 0) {
    XH_push (XH_t_trans, t);
    t2pos [t] = ++pos;
  }

  XH_set (&XH_t_trans, &t_trans);

  (*cur_dfa_extract_trans)(state, dfa_fill_full_trans);

  XH_push (XH_trans_list_id, -t_trans);

  for (i = 1; i <= pos; i++)
    XH_push (XH_trans_list_id, pos2state [i]);

  /* A chaque etat state on lui associe la liste de ses successeurs ds un XH
     l'ordre des successeurs est donne' par l'ordre des terminaux des transitions
     chaque element d'un groupe a le meme ensemble de symboles terminaux, leurs successeurs
     peuvent donc se comparer simplement */
  XH_set (&XH_trans_list_id, &(state2attr [state].trans_list_id));

  state2attr [state].init_part = t_trans;
}

static void
XH_part_set_oflw (SXINT old_size, SXINT new_size)
{
  sxuse (old_size);
  part_set_id2part = (SXINT *) sxrealloc (part_set_id2part, new_size+1, sizeof (SXINT));
}

#if LLOG
static SXINT configuration_nb;

static void
print_configuration (void)
{
  SXINT part, state;

  printf ("\nConfiguration = %ld\n", (long) ++configuration_nb);

  for (part = 1; part <= last_partition; part++) {
    state = part2attr [part].state;
    printf ("Partition %ld = {%ld", (long)part, (long)state);

    while ((state = state2attr [state].next_state)) {
      printf (", %ld", (long)state);
    }

    fputs ("}\n", stdout);
  }
}
#endif /* LLOG */



/* Structures qui sont utilisees par fsa2normalize appele' depuis dfa_minimize () */
static SXINT **forward_stacks;

static void
forward (SXINT part, void (*f)(SXINT p, SXINT t, SXINT q))
{
  SXINT next_part, t;
  SXINT *cur_forward_stack, *top_forward_stack;

  cur_forward_stack = forward_stacks [part];
  top_forward_stack = cur_forward_stack+TOP (cur_forward_stack);

  while (cur_forward_stack < top_forward_stack) {
    t = *++cur_forward_stack;
    next_part = *++cur_forward_stack;
    (*f)(part, t, next_part);
  }
}


/* Voici les resultats obtenus sur une RE dont le nb d'etats de l'automate deterministe est une exponentielle de la taille de la RE

Cas ds lequel dfa_minimize utilise les transitions entrantes (dfa_state2pred_nb != NULL)
echo "(a|b)*a(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)" | ./re2dfa.out
"stdin":
RE2NFA: state_nb = 21 (0ms)
NFA2DFA: state_nb = 131074, total_trans_nb = 327682 (23581ms)
constant_time_step_nb (with backward transitions) = DFA2min_DFA: state_nb = 131073, total_trans_nb = 327680 (1785ms)

RE2EFNFA: state_nb = 35 (0ms)
EFNFA2DFA: state_nb = 131073, total_trans_nb = 327680 (9364ms)
constant_time_step_nb (with backward transitions) = 4718599
EFDFA2min_DFA: state_nb = 131073, total_trans_nb = 327680 (1841ms)

RE2DFA: state_nb = 131073 (13609ms)
constant_time_step_nb (with backward transitions) = 4718599
DFA2min_DFA: state_nb = 131073, total_trans_nb = 327680 (1857ms)


Cas ds lequel dfa_minimize n'utilise pas les transitions entrantes (dfa_state2pred_nb == NULL)
echo "(a|b)*a(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)" | ./re2dfa.out
"stdin":
RE2NFA: state_nb = 21 (0ms)
NFA2DFA: state_nb = 131074, total_trans_nb = 327682 (23565ms)
constant_time_step_nb (without backward transitions) = 7888972
DFA2min_DFA: state_nb = 131073, total_trans_nb = 327680 (2004ms)

RE2EFNFA: state_nb = 35 (0ms)
EFNFA2DFA: state_nb = 131073, total_trans_nb = 327680 (9364ms)
constant_time_step_nb (without backward transitions) = 8086047
EFDFA2min_DFA: state_nb = 131073, total_trans_nb = 327680 (2461ms)

RE2DFA: state_nb = 131073 (13713ms)
constant_time_step_nb (without backward transitions) = 8086047
DFA2min_DFA: state_nb = 131073, total_trans_nb = 327680 (2977ms)

Il semble que l'utilisation en "backward" des transitions entrantes soit plus rapide !!

*/


/* Peut aussi s'appeler en stand alone */
/* Algo "standard" on rafine les partitions */
/* A partir d'une configuration initiale qui realise une partition de l'ensemble des etats avec la relation d'equiv :
   2 etats sont equiv ssi leurs transitions (t_set) sont egales
   L'algo assure qu'un etat est deplace' (change' de classe d'equiv) au plus log n fois
*/
void
dfa_minimize (SXINT cur_dfa_init_state, 
	      SXINT cur_dfa_final_state, 
	      SXINT eof_ste, 
	      void (*cur_dfa_extract_trans)(SXINT, 
					    void (*dfa_fill_dfa_state2pred_stack) (SXINT dfa_state, 
										   SXINT t, 
										   SXINT next_dfa_state)), 
	      void (*mindfa_fill_trans)(SXINT ,SXINT ,SXINT), 
	      BOOLEAN to_be_normalized)
{
  SXINT                 state, next_state, partition, next_partition, bot, top, part_set_id, cur_part, bot2, t_trans, t, trans_list_id, new_part;
  SXINT                 size, pred_state, cur_state, next_part, cur_part_card, nb_max, id, nb, id_max, last_part_set_id, old_last_partition;
  SXINT                 *part2new_part, *new_part2part, *area, *base_area, *working_stack, *w2_stack;
  struct part2attr    *cur_part_ptr;
  struct state2attr   *cur_state_ptr;
#if LOG
  SXINT                 constant_time_step_nb = 0;             
#endif /* LOG */

  sxinitialise (id_max);

  XH_alloc (&XH_t_trans, "XH_t_trans", cur_dfa_final_state+1, 1, eof_ste, NULL, NULL);

  XH_alloc (&XH_trans_list_id, "XH_trans_list_id", cur_dfa_final_state+1, 1, eof_ste, NULL, NULL);

  XH_alloc (&XH_part_set, "XH_part_set", cur_dfa_final_state+1, 1, eof_ste, XH_part_set_oflw, NULL);
  part_set_id2part = (SXINT *) sxalloc (XH_size (XH_part_set)+1, sizeof (SXINT));

  working_stack = (SXINT*) sxalloc (2*cur_dfa_final_state+1, sizeof (SXINT)), working_stack [0] = 0;

  /* dfa_state2pred_nb = NULL; On n'a pas besoin des predecesseurs */
  dfa_state2pred_nb = (SXINT*) sxcalloc (cur_dfa_final_state+1, sizeof (SXINT)); /* On utilise les predecesseurs (voir commentaire de tete) */

  state2attr = (struct state2attr*) sxalloc (cur_dfa_final_state+1, sizeof (struct state2attr));
  part2attr = (struct part2attr*) sxcalloc (cur_dfa_final_state+1, sizeof (struct part2attr));

  w2_stack = (SXINT*) sxalloc (cur_dfa_final_state+1, sizeof (SXINT)), w2_stack [0] = 0;

  /* initialisation ... */
  last_partition = 0;

  t2pos = (SXINT *) sxalloc (eof_ste+1, sizeof (SXINT));
  pos2state = (SXINT *) sxalloc (eof_ste+1, sizeof (SXINT));
  t_set = sxba_calloc (eof_ste+1);

  for (state = cur_dfa_init_state; state <= cur_dfa_final_state ; state++) {
    fill_init_min (state, cur_dfa_extract_trans);
  }

  sxfree (t_set), t_set = NULL;
  sxfree (t2pos), t2pos = NULL;
  sxfree (pos2state), pos2state = NULL;

  /* Ici chaque etat a recu une "partition" pre_initiale qui est son t_trans On fabrique une vraie partition initiale */
  for (state = cur_dfa_init_state; state <= cur_dfa_final_state ; state++) {
    trans_list_id = state2attr [state].trans_list_id;
    bot = XH_X (XH_trans_list_id, trans_list_id);
    top = XH_X (XH_trans_list_id, trans_list_id+1);

    t_trans = XH_list_elem (XH_trans_list_id, bot++);
    XH_push (XH_part_set, t_trans);

    while (bot < top) {
      next_state = XH_list_elem (XH_trans_list_id, bot);
      next_part = state2attr [next_state].init_part;
      XH_push (XH_part_set, -next_part); /* Il ne faut pas les confondre plus tard avec des "vraies" partition */

      bot++;
    }
	  
    if (!XH_set (&XH_part_set, &part_set_id))
      /* Nouveau */
      part_set_id2part [part_set_id] = next_part = ++last_partition;
    else
      next_part = part_set_id2part [part_set_id];

    /* On installe state ds next_part */
    init_set (state, next_part);
  }

#if LLOG
  configuration_nb = 0;
#endif /* LLOG */
    
  dfa_state2pred_stack = NULL;
  base_area = NULL;

  if (dfa_state2pred_nb) {
    SXINT              pred_part, cur_last_partition, cur_part_sub_card;
    SXINT              *w_state_stack, *top_state_ptr, *state_ptr, *tbp_part_stack, *tbp_state_stacks, *tbp_state_stack, *top_part_ptr, *part_ptr, *pred_stack;
    SXBA             w_state_set;
    struct part2attr *pred_part_ptr;

    /* version de minimize dans laquelle on a besoin des predecesseurs */
    size = dfa_state2pred_nb [0] + cur_dfa_final_state /* les comptes */;

    dfa_state2pred_stack = (SXINT **) sxalloc (cur_dfa_final_state+1, sizeof (SXINT*));
    area = base_area = (SXINT*) sxalloc (size+1, sizeof (SXINT));
    w_state_set = sxba_calloc (cur_dfa_final_state+1);
    w_state_stack = (SXINT *) sxalloc (cur_dfa_final_state+1, sizeof (SXINT));
    tbp_part_stack = (SXINT *) sxalloc (cur_dfa_final_state+1, sizeof (SXINT)), RAZ (tbp_part_stack);
    tbp_state_stacks = (SXINT *) sxalloc (2*cur_dfa_final_state+1, sizeof (SXINT)), RAZ (tbp_state_stacks);

    sxba_fill (w_state_set), SXBA_0_bit (w_state_set, 0);
    w_state_stack [0] = cur_dfa_final_state;

    for (state = 1; state <= cur_dfa_final_state; state++) {
      w_state_stack [state] = state; /* PUSH */
      dfa_state2pred_stack [state] = area;
      *area = 0;
      area += dfa_state2pred_nb [state]+1;
    }

    for (state = cur_dfa_init_state; state < cur_dfa_final_state ; state++)
      /* On remplit dfa_state2pred_stack */
      (*cur_dfa_extract_trans)(state, dfa_fill_dfa_state2pred_stack);

    sxfree (dfa_state2pred_nb), dfa_state2pred_nb = NULL;

    for(;;) {
#if LLOG
      print_configuration ();
#endif /* LLOG */

      /* Ici, les etats a traiter ont ete mis ds (w_state_stack, w_state_set) */
      /* On decoupe les etats a traiter par leur appartenance a la meme partition */
      top_state_ptr = w_state_stack+TOP(w_state_stack);

      /* On compte */
      for (state_ptr = top_state_ptr; state_ptr > w_state_stack; state_ptr--) {
	pred_state = *state_ptr;
	pred_part = state2attr [pred_state].part;

	if (part2attr [pred_part].sub_card++ == 0) {
	  /* 1ere fois que pred_part est trouve' */
	  PUSH (tbp_part_stack, pred_part);
	}
      }
	
      /* On prepare les piles */
      RAZ (tbp_state_stacks);

      top_part_ptr = tbp_part_stack+TOP(tbp_part_stack);

      for (part_ptr = top_part_ptr; part_ptr > tbp_part_stack; part_ptr--) {
	pred_part_ptr = part2attr + *part_ptr;
	PUSH (tbp_state_stacks, 0);
	pred_part_ptr->tbp_state_stack = tbp_state_stacks + TOP (tbp_state_stacks);
	TOP (tbp_state_stacks) += pred_part_ptr->sub_card;
	pred_part_ptr->sub_card = 0;
      }

      /* On stocke */
      for (state_ptr = top_state_ptr; state_ptr > w_state_stack; state_ptr--) {
	pred_state = *state_ptr;
	SXBA_0_bit (w_state_set, pred_state);
	pred_part = state2attr [pred_state].part;
	tbp_state_stack = part2attr [pred_part].tbp_state_stack;
	PUSH (tbp_state_stack, pred_state);
      }

      RAZ (w_state_stack);

      /* On traite */
      cur_last_partition = last_partition;

      while (!IS_EMPTY (tbp_part_stack)) {
#if LOG
	constant_time_step_nb++;             
#endif /* LOG */
	cur_part = POP (tbp_part_stack);
	cur_part_ptr = part2attr+cur_part;
	cur_part_card = cur_part_ptr->card;

	if (cur_part_card > 1) {
	  tbp_state_stack = cur_part_ptr->tbp_state_stack;
	  cur_part_sub_card = TOP (tbp_state_stack);
#if EBUG
	  if (cur_part_sub_card > cur_part_card)
	    sxtrap (ME, "dfa_minimize");
#endif /* EBUG */

	  RAZ (w2_stack);
	  nb_max = cur_part_card-cur_part_sub_card;
	  PUSH (w2_stack, nb_max); /* Pour les etats dont les successeurs n'ont pas bouge's */
	  id_max = 1; /* Le TOP */
	  last_part_set_id = XH_top (XH_part_set)-1;
	
	  for (state_ptr = tbp_state_stack+TOP(tbp_state_stack); state_ptr > tbp_state_stack; state_ptr--) {
	    cur_state = *state_ptr;
	    cur_state_ptr = state2attr + cur_state;

	    /* On recalcule ses etats successeurs */
	    trans_list_id = cur_state_ptr->trans_list_id;
	    bot = XH_X (XH_trans_list_id, trans_list_id);
	    top = XH_X (XH_trans_list_id, trans_list_id+1);

	    t_trans = XH_list_elem (XH_trans_list_id, bot++);
	    XH_push (XH_part_set, t_trans);

	    while (bot < top) {
#if LOG
	      constant_time_step_nb++;             
#endif /* LOG */
	      next_state = XH_list_elem (XH_trans_list_id, bot);
	      next_partition = state2attr [next_state].part;
	      XH_push (XH_part_set, next_partition);

	      bot++;
	    }

	    if (!XH_set (&XH_part_set, &part_set_id)) {
	      /* Nouveau */
	      PUSH (w2_stack, 0);
	      id = TOP (w2_stack);

#if EBUG 
	      if (TOP (w2_stack) != part_set_id-last_part_set_id+1)
		sxtrap (ME, "dfa_minimize");
#endif /* EBUG */
	    }
	    else {
	      id = (part_set_id <= last_part_set_id) ? 1 : part_set_id-last_part_set_id+1;
	    }

	    nb = ++(w2_stack [id]);

	    if (nb > nb_max) {
	      nb_max = nb;
	      id_max = id;
	    }

	    cur_state_ptr->temp_id = part_set_id;
	  }

	  /* Si nb_max == cur_part_card, on ne change rien */
	  if (nb_max < cur_part_card) {
	    old_last_partition = last_partition;

	    /* Ds cur_part on ne va pas deplacer le + grand groupe =>
	       chacun des groupes deplaces a une taille bornee par cur_part_card/2 */

	    /* C'est le sous-groupe qui n'a pas ete touche' qui est le plus populaire
	       On deplace les etats du complementaire ... */
	    for (state_ptr = tbp_state_stack+TOP(tbp_state_stack); state_ptr > tbp_state_stack; state_ptr--) {
#if LOG
	      constant_time_step_nb++;             
#endif /* LOG */
	      cur_state = *state_ptr;

	      if (id_max != 1 && w2_stack [1] != 0)
		/* Etat implique' par le rafinement precedant */
		SXBA_1_bit (w_state_set, cur_state);

	      cur_state_ptr = state2attr + cur_state;
	      part_set_id = cur_state_ptr->temp_id;
	      id = (part_set_id <= last_part_set_id) ? 1 : part_set_id-last_part_set_id+1;

	      if (id != id_max) {
		/* On bouge cur_state */
		if ((next_part = w2_stack [id]) > 0) {
		  /* 1ere fois */
		  next_part = ++last_partition;
		  w2_stack [id] = -next_part; /* Pour les coups d'apres */
		}
		else
		  next_part = -next_part;

		/* cur_state est change' de partition */
		/* Ce changement ne peut etre valide' au cours de ce run, il le sera + tard sinon les futurs calculs ds XH_part_set peuvent etre faux */
		/* state2part [cur_state] = next_part; doit etre retarde' sinon les futurs calculs ds  XH_part_set peuvent etre faux */
		PUSH (working_stack, cur_state);
		PUSH (working_stack, next_part);
	      }
	      else
		/* On ne bouge pas la sous-part maximale */
		next_part = cur_part;

#if EBUG
	      if (next_part != cur_part && next_part <= old_last_partition)
		sxtrap (ME, "dfa_minimize");
#endif /* EBUG */

	      /* part_set_id2part [part_set_id] = next_part; semble inutile */
	    }
	    
	    if (id_max != 1 && w2_stack [1] != 0) {
	      /* Le sous-groupe non touche' par le rafinement pre'ce'dant n'est pas le plus populaire, il va donc etre de'place' aussi car il est non vide */
	      next_part = ++last_partition;

	      for (cur_state = cur_part_ptr->state; cur_state != 0; cur_state = state2attr [cur_state].next_state) { 
		if (!SXBA_bit_is_set_reset (w_state_set, cur_state)) {
		  PUSH (working_stack, cur_state);
		  PUSH (working_stack, next_part);
		}
	      }
	    }
	  }
	}
      }

      if (!IS_EMPTY (working_stack)) {
	do {
	  cur_part = POP (working_stack);
	  cur_state = POP (working_stack);
	  move (cur_state, cur_part);

	  pred_stack = dfa_state2pred_stack [cur_state];

	  for (state_ptr = pred_stack+TOP(pred_stack); state_ptr > pred_stack; state_ptr--) {
#if LOG
	    constant_time_step_nb++;             
#endif /* LOG */
	    pred_state = *state_ptr;

	    if (SXBA_bit_is_reset_set (w_state_set, pred_state))
	      PUSH (w_state_stack, pred_state);
	  }
	} while (!IS_EMPTY (working_stack));
      }
      else
	break;
    }

    sxfree (w_state_stack);
    sxfree (w_state_set);
    sxfree (tbp_part_stack);
    sxfree (tbp_state_stacks);
  }
  else {
    /* version de minimize dans laquelle on n'utilise pas les predecesseurs */
    for(;;) {
#if LLOG
      print_configuration ();
#endif /* LLOG */

      for (cur_part = last_partition; cur_part >= 1; cur_part--) {
#if LOG
	constant_time_step_nb++;             
#endif /* LOG */
	cur_part_ptr = part2attr+cur_part;
	cur_part_card = cur_part_ptr->card;

	if (cur_part_card > 1) {
	  RAZ (w2_stack);
	  PUSH (w2_stack, 0); /* Pour les etats dont les successeurs n'ont pas bouge's */
	  nb_max = 0;
	  last_part_set_id = XH_top (XH_part_set)-1;

	  for (cur_state = cur_part_ptr->state; cur_state != 0; cur_state = cur_state_ptr->next_state) {
	    cur_state_ptr = state2attr + cur_state;

	    /* On recalcule ses etats successeurs */
	    trans_list_id = cur_state_ptr->trans_list_id;
	    bot = XH_X (XH_trans_list_id, trans_list_id);
	    top = XH_X (XH_trans_list_id, trans_list_id+1);

	    t_trans = XH_list_elem (XH_trans_list_id, bot++);
	    XH_push (XH_part_set, t_trans);

	    while (bot < top) {
#if LOG
	      constant_time_step_nb++;             
#endif /* LOG */
	      next_state = XH_list_elem (XH_trans_list_id, bot);
	      next_partition = state2attr [next_state].part;
	      XH_push (XH_part_set, next_partition);

	      bot++;
	    }

	    if (!XH_set (&XH_part_set, &part_set_id)) {
	      /* Nouveau */
	      PUSH (w2_stack, 0);
	      id = TOP (w2_stack);

#if EBUG 
	      if (TOP (w2_stack) != part_set_id-last_part_set_id+1)
		sxtrap (ME, "dfa_minimize");
#endif /* EBUG */
	    }
	    else {
	      id = (part_set_id <= last_part_set_id) ? 1 : part_set_id-last_part_set_id+1;
	    }

	    nb = ++(w2_stack [id]);

	    if (nb > nb_max) {
	      nb_max = nb;
	      id_max = id;
	    }

	    cur_state_ptr->temp_id = part_set_id;
	  }

	  /* Si nb_max == cur_part_card, on ne change rien */
	  if (nb_max < cur_part_card) {
	    old_last_partition = last_partition;

	    /* Ds cur_part on ne va pas deplacer le + grand groupe =>
	       chacun des groupes deplaces a une taille bornee par cur_part_card/2 */
	    for (cur_state = cur_part_ptr->state; cur_state != 0; cur_state = cur_state_ptr->next_state) { 
#if LOG
	      constant_time_step_nb++;             
#endif /* LOG */
	      cur_state_ptr = state2attr + cur_state;
	      part_set_id = cur_state_ptr->temp_id;
	      id = (part_set_id <= last_part_set_id) ? 1 : part_set_id-last_part_set_id+1;

	      if (id != id_max) {
		/* On bouge cur_state */
		if ((next_part = w2_stack [id]) > 0) {
		  /* 1ere fois */
		  next_part = ++last_partition;
		  w2_stack [id] = -next_part; /* Pour les coups d'apres */
		}
		else
		  next_part = -next_part;

		/* cur_state est change' de partition */
		/* Ce changement ne peut etre valide' au cours de ce run, il le sera + tard sinon les futurs calculs ds XH_part_set peuvent etre faux */
		/* state2part [cur_state] = next_part; doit etre retarde' sinon les futurs calculs ds  XH_part_set peuvent etre faux */
		PUSH (working_stack, cur_state);
		PUSH (working_stack, next_part);
	      }
	      else
		/* On ne bouge pas la sous-part maximale */
		next_part = cur_part;

#if EBUG
	      if (next_part != cur_part && next_part <= old_last_partition)
		sxtrap (ME, "dfa_minimize");
#endif /* EBUG */

	      /* part_set_id2part [part_set_id] = next_part; semble inutile */
	    }
	  }
	}
      }

      if (!IS_EMPTY (working_stack)) {
	do {
#if LOG
	  constant_time_step_nb++;             
#endif /* LOG */
	  cur_part = POP (working_stack);
	  cur_state = POP (working_stack);
	  move (cur_state, cur_part);
	} while (!IS_EMPTY (working_stack));
      }
      else
	break;
    }
  }

#if LOG
  printf ("constant_time_step_nb (with%s backward transitions) = %ld\n", dfa_state2pred_stack ? "" : "out", (long)constant_time_step_nb);
#endif /* LOG */
    
  sxfree (working_stack);
  sxfree (w2_stack);
  XH_free (&XH_part_set);
  sxfree (part_set_id2part), part_set_id2part = NULL;

  if (dfa_state2pred_stack) {
    sxfree (dfa_state2pred_stack), dfa_state2pred_stack = NULL;
    sxfree (base_area);
  }

  /* On assure seulement que l'etat initial a la valeur min
     et l'etat final la valeur max */
  /* On en a aussi besoin ds fsa_normalize () */
  part2new_part = (SXINT *) sxcalloc (last_partition+1, sizeof (SXINT));
  new_part2part = (SXINT *) sxalloc (last_partition+1, sizeof (SXINT));
  new_part = 1;

  for (state = cur_dfa_init_state; state <= cur_dfa_final_state; state++) {
    partition = state2attr [state].part;

    if (part2new_part [partition] == 0) {
      part2new_part [partition] = new_part;
      new_part2part [new_part] = partition;
      new_part++;
    }
  }

  if (!to_be_normalized) {
    for (new_part = 1; new_part <= last_partition; new_part++) {
      partition = new_part2part [new_part];
    
      state = part2attr [partition].state;
      trans_list_id = state2attr [state].trans_list_id;
      bot = XH_X (XH_trans_list_id, trans_list_id);
      top = XH_X (XH_trans_list_id, trans_list_id+1);

      t_trans = -XH_list_elem (XH_trans_list_id, bot++);
      bot2 = XH_X (XH_t_trans, t_trans);

      while (bot < top) {
	next_state = XH_list_elem (XH_trans_list_id, bot);
	next_partition = state2attr [next_state].part;
	t = XH_list_elem (XH_t_trans, bot2);

	//	if (t == eof_ste)
	//	  t = -1;

	(*mindfa_fill_trans)(new_part, t, part2new_part [next_partition]);

	bot++;
	bot2++;
      }
    }
  }
  else {
    /* Ici on assure en plus que si p ->t q alors, q > p (si possible : pas de cycle impliquant p) */
    /* ... on utilise fsa_normalize (SXINT init_state, SXINT final_state, void (*fsa_forward_trans)(), void (*fsa_fill_trans)()); */
    /* (*fsa_forward_trans)(p, f)  appelle (*f)(p, t, q) pour tout p ->t q
       fsa_normalize () appelle (*fsa_fill_trans)(p', t', q') pour toute transition p' ->t' q' renumerotee
    */
    SXINT trans_nb;
    SXINT *forward_list, *cur_forward_stack;

    /* on fabrique les structures pour forward () */
    /* On commence par calculer le nb de transitions */
    trans_nb = 0;

    for (partition = 1; partition <= last_partition; partition++) {
      state = part2attr [partition].state;
      trans_list_id = state2attr [state].trans_list_id;
      bot = XH_X (XH_trans_list_id, trans_list_id)+1;
      top = XH_X (XH_trans_list_id, trans_list_id+1);
      trans_nb += top-bot;
    }

    /* Structures a remplir qui seront utilisees par forward () */
    forward_stacks = (SXINT **) sxalloc (last_partition+1, sizeof (SXINT *)), forward_stacks [0] = NULL;
    cur_forward_stack = forward_list = (SXINT *) sxalloc (2*trans_nb+last_partition+1, sizeof (SXINT));

    for (new_part = 1; new_part <= last_partition; new_part++) {
      forward_stacks [new_part] = cur_forward_stack;
      partition = new_part2part [new_part];
      *cur_forward_stack = 0;

      state = part2attr [partition].state;
      trans_list_id = state2attr [state].trans_list_id;
      bot = XH_X (XH_trans_list_id, trans_list_id);
      top = XH_X (XH_trans_list_id, trans_list_id+1);

      t_trans = -XH_list_elem (XH_trans_list_id, bot++);
      bot2 = XH_X (XH_t_trans, t_trans);
      cur_forward_stack = forward_stacks [new_part];

      while (bot < top) {
	next_state = XH_list_elem (XH_trans_list_id, bot);
	next_partition = state2attr [next_state].part;
	t = XH_list_elem (XH_t_trans, bot2);

	PUSH (cur_forward_stack, t);
	PUSH (cur_forward_stack, part2new_part [next_partition]);

	bot++;
	bot2++;
      }

      cur_forward_stack += *cur_forward_stack + 1;
    }

    fsa_normalize (1, last_partition, eof_ste, forward, mindfa_fill_trans);

    sxfree (forward_stacks), forward_stacks = NULL;
    sxfree (forward_list);
  }

  sxfree (part2new_part);
  sxfree (new_part2part);

  sxfree (part2attr), part2attr = NULL;
  sxfree (state2attr), state2attr = NULL;

  XH_free (&XH_t_trans);
  XH_free (&XH_trans_list_id);
}


/* calcule new_state et ... */
/* ... retourne TRUE ssi new_state existait deja */
static BOOLEAN
get_state (SXBA old_state_set, SXINT *new_state)
{
  SXINT state = -1;

  while ((state = sxba_scan_reset (old_state_set, state)) >= 0)
    XH_push (XH_dfa_states, state);

  return XH_set (&XH_dfa_states, new_state);
}

/* Cette fonction est appelee pour chaque transition nfa_state ->t next_nfa_state */
static void
nfa_fill_trans (SXINT nfa_state, SXINT t, SXINT next_nfa_state)
{
  sxuse (nfa_state);
  SXBA_1_bit (t2nfa_state_set [t], next_nfa_state);

  if (SXBA_bit_is_reset_set (t_set, t))
    PUSH (nfa_t_stack, t);
}

static void
XxY_out_trans_oflw (SXINT old_size, SXINT new_size)
{
  sxuse (old_size);
  XxY_out_trans2next_state = (SXINT*) sxrealloc (XxY_out_trans2next_state, new_size+1, sizeof (SXINT));
}

/* Point d'entree principal qui calcule l'automate deterministe */
/* dfa_fill_trans est appele' pour sortir le dfa non minimal */
/* et mindfa_fill_trans pour sortir l'automate minimal */
void
nfa2dfa (SXINT init_state, 
	 SXINT final_state, 
	 SXINT eof_ste, 
	 BOOLEAN (*empty_trans)(SXINT, SXBA), 
	 void (*nfa_extract_trans)(SXINT, void (*nfa_fill_trans)(SXINT, SXINT, SXINT) ), 
	 void (*dfa_fill_trans)(SXINT, SXINT, SXINT, BOOLEAN), 
	 void (*mindfa_fill_trans)(SXINT, SXINT, SXINT), 
	 BOOLEAN to_be_normalized)
{
  SXINT     dfa_state, new_dfa_state, bot, top, nfa_state, t, id;
  SXINT     *final_dfa_state_stack;
  SXBA      cur_state_set;

  nfa_init_state = init_state;
  nfa_final_state = final_state;
  nfa_empty_trans = empty_trans; /* Appels multiples */

  dfa_init_state = 1;

  if (empty_trans) {
    /* Il y a des transitions sur epsilon, on alloue qq structres */
    nfa_ec_stack = (SXINT*) sxalloc (nfa_final_state+1, sizeof (SXINT)), nfa_ec_stack [0] = 0;
    nfa_wstate_set = sxba_calloc (nfa_final_state+1);
  }

  nfa_t_stack = (SXINT*) sxalloc (eof_ste+1, sizeof (SXINT)), nfa_t_stack [0] = 0;
  DALLOC_STACK (final_dfa_state_stack, final_state);
  DALLOC_STACK (nfa_state_stack, nfa_final_state);
  t_set = sxba_calloc (eof_ste+1);
  t2nfa_state_set = sxbm_calloc (eof_ste+1, nfa_final_state+1);
  XH_alloc (&(XH_dfa_states), "XH_dfa_states", nfa_final_state+1, 1, 10, NULL, NULL);

  if (mindfa_fill_trans) {
    XxY_alloc (&XxY_out_trans, "XxY_out_trans", final_state+1, 1, 1, 0, XxY_out_trans_oflw, NULL);
    XxY_out_trans2next_state = (SXINT*) sxalloc (XxY_size (XxY_out_trans)+1, sizeof (SXINT));
  }

  /* traitement de l'etat initial */
  cur_state_set = t2nfa_state_set [0];

  SXBA_1_bit (cur_state_set, nfa_init_state);

  if (nfa_empty_trans)
    epsilon_closure (cur_state_set);

  get_state (cur_state_set, &dfa_state);

  DPUSH (nfa_state_stack, dfa_state);

  while (!IS_EMPTY (nfa_state_stack)) {
    dfa_state = DPOP (nfa_state_stack);

    bot = XH_X (XH_dfa_states, dfa_state);
    top = XH_X (XH_dfa_states, dfa_state+1);

    while (bot < top) {
      nfa_state = XH_list_elem (XH_dfa_states, bot);
      (*nfa_extract_trans) (nfa_state, nfa_fill_trans);
      bot++;
    }

    while (!IS_EMPTY (nfa_t_stack)) {
      t = POP (nfa_t_stack);
      SXBA_0_bit (t_set, t);

      if (t != eof_ste) {
	cur_state_set = t2nfa_state_set [t];

	if (nfa_empty_trans)
	  epsilon_closure (cur_state_set);

	if (!get_state (cur_state_set, &new_dfa_state)) {
	  /* nouveau */
	  DPUSH (nfa_state_stack, new_dfa_state);
	}

	/* Ici on a une transition dfa_state ->t new_dfa_state */
	/* C'est l'appelant qui se charge de stocker l'automate deterministe */
	if (dfa_fill_trans)
	  (*dfa_fill_trans) (dfa_state, t, new_dfa_state, FALSE /* !is_final */);

	if (mindfa_fill_trans) {
	  XxY_set (&XxY_out_trans, dfa_state, t, &id);
	  XxY_out_trans2next_state [id] = new_dfa_state;
	}
      }
      else {
	/* transition vers l'etat final unique, on la differe pour que l'etat final ait le + grand id */
	DPUSH (final_dfa_state_stack, dfa_state);
      }
    }
  }

  /* Calcul de l'etat final */
  get_state (t2nfa_state_set [eof_ste], &dfa_final_state);
  dfa_state_nb = dfa_final_state;
  
  while (!IS_EMPTY (final_dfa_state_stack)) {
    dfa_state = DPOP (final_dfa_state_stack);

    if (dfa_fill_trans)
      (*dfa_fill_trans) (dfa_state, eof_ste, dfa_final_state, TRUE /* is_final */);

    if (mindfa_fill_trans) {
      XxY_set (&XxY_out_trans, dfa_state, eof_ste, &id);
      XxY_out_trans2next_state [id] = new_dfa_state;
    }
  }

  if (nfa_empty_trans) {
    sxfree (nfa_ec_stack), nfa_ec_stack = NULL;
    sxfree (nfa_wstate_set), nfa_wstate_set = NULL;
  }

  sxfree (nfa_t_stack), nfa_t_stack = NULL;
  DFREE_STACK (final_dfa_state_stack);
  DFREE_STACK (nfa_state_stack);
  sxbm_free (t2nfa_state_set), t2nfa_state_set = NULL;
  XH_free (&XH_dfa_states);
  sxfree (t_set), t_set = NULL;

  if (mindfa_fill_trans) {
    dfa_minimize (dfa_init_state, dfa_final_state, eof_ste, dfa_extract_trans, mindfa_fill_trans, to_be_normalized);

    XxY_free (&XxY_out_trans);
    sxfree (XxY_out_trans2next_state), XxY_out_trans2next_state = NULL;
  }
}


/* Le module fsa_to_re.c a e'te' inse're' ici dans fsa_mngr.c Le 03/10/07 */



/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */


/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */


/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from fsa_to_re.pl1      */
/*  on Monday the fourteenth of April, 1986, at 13:44:02,    */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3g PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030512 13:07 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* Ven 22 Nov 1996 17:25(pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 15-04-92 14:20 (pb):		Utilisation des SXBA standards		*/
/************************************************************************/
/* 11-10-90 14:45 (pb):		L'etat "0" est accepte.			*/
/*				Optimisations (sur un exemple le temps	*/
/*				de construction d'ERs est passe de	*/
/*				80.7s a 14.4s!)				*/
/************************************************************************/
/* 11-10-90 14:45 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/




char WHAT_FSA_TO_RE[] = "@(#)SYNTAX - $Id: fsa_mngr.c 1214 2008-03-13 12:49:51Z rlacroix $" WHAT_DEBUG;


#define leaf (SXINT)1
#define concat (SXINT)2
#define star (SXINT)3
#define or (SXINT)4
#define plus (SXINT)5
#define option (SXINT)6

#define head_s_no (SXINT)67
#define node_number_per_area (SXINT)20

static char     **meta_op; /* op [6] = {"(", ")*", "(", ")+", "(", ")?"}; */

static SXINT	S [7] [7] = {{0, 0, 0, 0, 0, 0, 0},
			     { /* bidon */ 0, 1, 0, 2, 1, 1, 1},
			     { /* leaf */ 0, 0, 0, 0, 0, 0, 0},
			     { /* concat */ 0, 3, 0, 4, 3, 5, 6},
			     { /* star */ 0, 1, 0, 2, 1, 1, 1},
			     { /* or */ 0, 1, 0, 6, 1, 1, 6},
			     { /* plus */ 0, 1, 0, 5, 1, 5, 1} /* option */ };
static SXINT      SIZE, INITIAL_S;
static SXBA	/* SIZE */ *M /* 0:SIZE-1 */ ;
struct node {
    struct node		*link;
    SXINT		name;
    union {
	struct {
	    struct node		*left;
	    struct node		*right;
	}	brother;
	char	*string_ptr;
    }	body;
};
struct tree_area {
    struct tree_area	*lnk;
    struct node		node_s [node_number_per_area];
};
static struct tree_area		*tree_area_ptr;
static SXINT	cur_node_no;
static struct node	***re_ptrs /* 0:SIZE-1, 0:SIZE-1 */ ;

/* re_ptrs(i,j) = pointeur vers l'arbre representant
l'expression reguliere entre les noeuds i et j si M (i, j) */

static SXINT	*in, *out /* 1:SIZE */ ;

/* pour chaque noeud arite en entree et sortie */

static SXBA	/* SIZE */ to_be_processed, reached_set, loop_set;
static struct node	**P_S /* 1:P_S_size */ ;

struct tete {
    SXINT size;
    struct node		**T;
};
static struct tete	*tete;
static SXINT       size_of_tete, top_of_tete, x_of_tete;
#define tete_pop(n)	(x_of_tete -= n)

struct sets {
    SXINT 	size;
    SXBA	set;	
};
static struct sets	*sets;
static SXINT		size_of_sets, top_of_sets, x_of_sets;
#define sets_pop(n)	(x_of_sets -= n)

struct ints {
    SXINT 	size;
    SXINT		*T;	
};
static struct ints	*ints;
static SXINT		size_of_ints, top_of_ints, x_of_ints;
#define ints_pop(n)	(x_of_ints -= n)

static SXINT		*to_be_sorted /* 1:M */ ;
static SXINT		size_of_to_be_sorted;

static SXBA		succ_set;

static SXBA_INDEX       prev, current;
static struct node	*head_s [head_s_no];
static struct node	*leafs_hd [head_s_no];
static struct node	*canon (SXINT oper, struct node *left, struct node *right);
static struct node	*reduce_or_son (struct node *visited);
static struct node	*gen_prefixe (struct node ***p_s, SXBA set, struct node *node_ptr, SXINT or_no, SXINT *min_s, SXINT *MAX_s);
static struct node	*gen_suffixe (struct node ***p_s, SXBA set, struct node *node_ptr, SXINT or_no, SXINT *min_s, SXINT *MAX_s);




static struct node **tete_open (SXINT size)
{
    struct tete	*atete;

    if (tete == NULL) {
	tete = (struct tete*) sxalloc ((size_of_tete = 5) + 1, sizeof (struct tete));
	top_of_tete = x_of_tete = 0;
	/* x_of_tete <= top-of_tete */
    }
    else if (x_of_tete > size_of_tete) {
	tete = (struct tete*) sxrealloc (tete, (size_of_tete *= 2) + 1, sizeof (struct tete));
    }

    atete = tete + x_of_tete;

    if (x_of_tete == top_of_tete) {
	atete->T = (struct node**) sxalloc (size + 1, sizeof (struct node*));
	atete->size = size;
	top_of_tete++;
    }
    else if (size > atete->size) {
	    atete->T = (struct node**) sxrealloc (atete->T, size + 1, sizeof (struct node*));
	    atete->size = size;
	}

    x_of_tete++;
    return atete->T;
}

static VOID tete_free (void)
{
    if (tete != NULL) {
	struct tete	*atete = tete + top_of_tete;

	while (--atete >= tete)
	    sxfree (atete->T);

	sxfree (tete);
    }
}


static SXINT *ints_open (SXINT size)
{
    struct ints	*aints;

    if (ints == NULL) {
	ints = (struct ints*) sxalloc ((size_of_ints = 5) + 1, sizeof (struct ints));
	top_of_ints = x_of_ints = 0;
	/* x_of_ints <= top-of_ints */
    }
    else if (x_of_ints > size_of_ints) {
	ints = (struct ints*) sxrealloc (ints, (size_of_ints *= 2) + 1, sizeof (struct ints));
    }

    aints = ints + x_of_ints;

    if (x_of_ints == top_of_ints) {
	aints->T = (SXINT*) sxalloc (size + 1, sizeof (SXINT));
	aints->size = size;
	top_of_ints++;
    }
    else if (size > aints->size) {
	    aints->T = (SXINT*) sxrealloc (aints->T, size + 1, sizeof (SXINT));
	    aints->size = size;
	}

    x_of_ints++;
    return aints->T;
}

static VOID ints_free (void)
{
    if (ints != NULL) {
	struct ints	*aints = ints + top_of_ints;

	while (--aints >= ints)
	    sxfree (aints->T);

	sxfree (ints);
    }
}


static SXBA sets_open (SXINT size)
{
    struct sets	*asets;

    if (sets == NULL) {
	sets = (struct sets*) sxalloc ((size_of_sets = 4) + 1, sizeof (struct sets));
	top_of_sets = x_of_sets = 0;
	/* x_of_sets <= top-of_sets */
    }
    else if (x_of_sets > size_of_sets) {
	sets = (struct sets*) sxrealloc (sets, (size_of_sets *= 2) + 1, sizeof (struct sets));
    }

    asets = sets + x_of_sets;

    if (x_of_sets == top_of_sets) {
	asets->set = sxba_calloc (size + 1);
	asets->size = size;
	top_of_sets++;
    }
    else {
	sxba_empty (asets->set);

	if (size != asets->size) {
	    asets->set = sxba_resize (asets->set, size + 1);
	    asets->size = size;
	}
    }

    x_of_sets++;
    return asets->set;
}

static VOID sets_free (void)
{
    if (sets != NULL) {
	struct sets	*asets = sets + top_of_sets;

	while (--asets >= sets)
	    sxfree (asets->set);

	sxfree (sets);
    }
}



static struct node	*alloc_a_node (void)
{
    struct tree_area	*p;

    if (cur_node_no == node_number_per_area) {
	p = tree_area_ptr;
	tree_area_ptr = (struct tree_area*) sxcalloc (1, sizeof (struct tree_area));
	tree_area_ptr->lnk = p;
	cur_node_no = 0;
    }

    return &(tree_area_ptr->node_s [cur_node_no++]);
}



static SXVOID	free_tree_areas (void)
{
    struct tree_area	*p;

    while ((p = tree_area_ptr) , (p != NULL)) {
	tree_area_ptr = p->lnk;
	sxfree (p);
    }
}



static struct node	*create_leaf (SXBA_INDEX i, SXBA_INDEX j, SXINT (*get_name_refs) (SXBA_INDEX, SXBA_INDEX, char ** ))
{
    struct node	*leaf_node_ptr, **orig;
    char			*p;

    (*get_name_refs) (i, j, &p);

    for (leaf_node_ptr = *(orig = leafs_hd + (((long) p) % head_s_no)); leaf_node_ptr != NULL; leaf_node_ptr = leaf_node_ptr->link) {
	if (p == leaf_node_ptr->body.string_ptr)
	    return (leaf_node_ptr);
    }

    leaf_node_ptr = alloc_a_node ();
    leaf_node_ptr->name = leaf;
    leaf_node_ptr->link = *orig;
    leaf_node_ptr->body.string_ptr = p;
    return *orig = leaf_node_ptr;
}



static struct node	*create_op (SXINT op, struct node *l, struct node *r)
{
    struct node	*p, **orig;

    for (p = *(orig = head_s + ((op + (((long) (l) ^ (long) (r)) >> 1)) % head_s_no)); p != NULL; p = p->link) {
	if (p->name == op && p->body.brother.left == l && p->body.brother.right == r)
	    return (p);
    }

    p = alloc_a_node ();
    p->name = op;
    p->body.brother.left = l;
    p->body.brother.right = r;
    p->link = *orig;
    return *orig = p;
}



static struct node	*create_list (struct node **p_s, SXINT m, SXINT create_list_M, SXINT op)
{
    SXINT		x;
    struct node	*p, *q;

    x = create_list_M;

    for (p = p_s [x]; p == NULL; p = p_s [x]) {
	x--;
    }

    for (x--; x >= m; x--) {
	q = p_s [x];

	if (q != NULL)
	    p = create_op (op, q, p);
    }

    return (p);
}



static BOOLEAN	is_tail (struct node *p1, struct node *p2)
{
    /* Si p2 est une concatenation, regarde si le dernier composant est p1 */
    struct node	*p;

    if (p2->name != concat)
	return (FALSE);

    for (p = p2; p->name == concat; p = p->body.brother.right) {
    }

    return (p == p1);
}



static SXINT	nb_op_sons (struct node *p, SXINT op)
{
    if (p->name != op)
	return (1);

    return (1 + nb_op_sons (p->body.brother.right, op));
}



static SXVOID	gather_op_sons (struct node **p_s, SXINT *xp, struct node *p, SXINT op)
{
    if (p->name != op) {
	p_s [++*xp] = p;
    }
    else {
	p_s [++*xp] = p->body.brother.left;
	gather_op_sons (p_s, xp, p->body.brother.right, op);
    }
}


static SXVOID	process_substr (struct node **p_s, SXINT xstar, SXINT xconc, SXINT process_substr_M)
{
    struct node		*star_son_ptr;
    SXINT		d, y;

    star_son_ptr = p_s [xstar]->body.brother.left;

    if (star_son_ptr == p_s [xconc]) {
	p_s [xconc] = NULL;
	p_s [xstar] = canon (plus, star_son_ptr, NULL);
    }
    else if (star_son_ptr->name == concat) {
	y = nb_op_sons (star_son_ptr, concat);
	d = xconc - ((xconc < xstar) ? y : 1);

	if (xconc < xstar || d + y <= process_substr_M) {
	    struct node	**q_s, **r_s /* 1:y */ ;
	    SXINT		i, j;
	    SXINT				x;

	    q_s = tete_open (y);
	    x = 0;
	    gather_op_sons (q_s, &x, star_son_ptr, concat);

	    if (xconc < xstar) {
		for (i = y; i >= 1; i--, xconc--) {
		    if (xconc == 0 || q_s [i] != p_s [xconc]) {
			if (i == y) {
			    tete_pop (1);
			    return;
			}

			/* egalite entre i+1 .. y;normalisation
			...UW{VW}X... => ...U{WV}WX... */

			r_s = tete_open (y);

			for (j = 1; j <= y; j++) {
			    r_s [j - i + ((j <= i) ? y : 0)] = q_s [j];
			}

			xconc++;

			for (; xstar > xconc; xstar--) {
			    p_s [xstar] = p_s [xstar - 1];
			}

			p_s [xconc] = canon (star, create_list (r_s, (SXINT)1, y, concat), NULL);
			tete_pop (2);
			process_substr (p_s, xconc, xconc + 1, process_substr_M);
			return;
		    }
		}
	    }
	    else {
		for (i = 1; i <= y; i++) {
		    if (q_s [i] != p_s [d + i]) {
			tete_pop (1);
			return;
		    }
		}
	    }

	    for (i = d + 1; i <= d + y; i++) {
		p_s [i] = NULL;
	    }

	    tete_pop (1);
	    p_s [xstar] = canon (plus, star_son_ptr, NULL);
	}
    }
}



static BOOLEAN	is_an_element (struct node *head_ptr, struct node *elem_ptr, SXINT oper)
{
    struct node	*p;

    for (p = head_ptr; p->name == oper; p = p->body.brother.right) {
	if (p->body.brother.left == elem_ptr)
	    return (TRUE);
    }

    return (p == elem_ptr);
}



static BOOLEAN	is_a_subset (struct node **p_s, SXINT m1, SXINT M1, struct node **q_s, SXINT m2, SXINT M2)
{
    SXINT		x1, x2;
    struct node	*p2;

    x1 = m1 - 1;

    for (x2 = m2; x2 <= M2; x2++) {
	if ((p2 = q_s [x2]) != NULL) {
	    for (x1++; x1 <= M1; x1++) {
		if (p2 == p_s [x1])
		    break;
	    }

	    if (x1 > M1)
		return FALSE;
	}
    }

    return TRUE;
}



static BOOLEAN	is_a_member (struct node *p1, struct node *p2)
{
    /* Check if the left son of p1 is
	 - the left son of p2
	 - or a [sub-]alternative of p2 */
    struct node		*p1_son, *p2_son;
    SXINT		xp1, xp2;

    if ((p1_son = p1->body.brother.left) == (p2_son = p2->body.brother.left))
	return (TRUE);

    if (p2_son->name != or)
	return (FALSE);

    if (p1_son->name != or)
	return (is_an_element (p2_son, p1_son, or));

    if ((xp1 = nb_op_sons (p1_son, or)) > (xp2 = nb_op_sons (p2_son, or)))
	return (FALSE);
    
    {
	struct node	**p1_s /* 1:xp1 */ , **p2_s /* 1:xp2 */ ;
	SXINT		x;
	BOOLEAN	subsetp;
	
	p1_s = tete_open (xp1);
	p2_s = tete_open (xp2);
	x = 0;
	gather_op_sons (p1_s, &x, p1_son, or);
	x = 0;
	gather_op_sons (p2_s, &x, p2_son, or);
	subsetp = is_a_subset (p2_s, (SXINT)1, xp2, p1_s, (SXINT)1, xp1);
	tete_pop (2);
	return subsetp;
    }
}



static BOOLEAN	is_a_staror_elem (struct node *node_ptr, struct node **result_ptr)
{
    SXINT			concat_no;

    *result_ptr = NULL;

    if (node_ptr->name != concat)
	return FALSE;

    concat_no = nb_op_sons (node_ptr, concat);

    {
	struct node	**p_s /* 1:concat_no */, *or_ptr, *p ;
	SXINT	x, or_no;

	p_s = tete_open (concat_no);
	x = 0;
	gather_op_sons (p_s, &x, node_ptr, concat);
	p = p_s [concat_no];

	if (p->name != star) {
	    tete_pop (1);
	    return (FALSE);
	}

	or_ptr = p->body.brother.left;

	if (or_ptr->name != or) {
	    tete_pop (1);
	    return (FALSE);
	}

	p = create_list (p_s, (SXINT)1, concat_no - 1, concat);
	or_no = nb_op_sons (or_ptr, or);

	{
	    struct node	**q_s /* 1:or_no */ ;

	    q_s = tete_open (or_no);
	    x = 0;
	    gather_op_sons (q_s, &x, or_ptr, or);

	    for (x = 1; x <= or_no; x++) {
		if (q_s [x] == p) {
		    q_s [x] = NULL;
		    goto next;
		}
	    }

	    tete_pop (2);
	    return (FALSE);

next:	    *result_ptr = create_op (concat, p, canon (star, create_list (q_s, (SXINT)1, or_no, or), NULL));
	    tete_pop (2);
	    return (TRUE);
	}
    }
}



static BOOLEAN	is_a_sublanguage (struct node *left, struct node *right)
{
    struct node	*left_son, *right_son;
    SXINT				or_no, concat_no;

    left_son = left->body.brother.left;
    right_son = right->body.brother.left;

    if (left_son->name != or || right_son->name != concat)
	return (FALSE);

    or_no = nb_op_sons (left_son, or);
    concat_no = nb_op_sons (right_son, concat);

    {
	struct node		**or_son_ptrs, **concat_son_ptrs;
	struct node	*p;
	SXINT	x, cn;
	BOOLEAN		elementp;

	or_son_ptrs = tete_open (or_no);
	concat_son_ptrs = tete_open (concat_no);
	x = 0;
	gather_op_sons (or_son_ptrs, &x, left_son, or);
	x = 0;
	gather_op_sons (concat_son_ptrs, &x, right_son, concat);

	for (x = concat_no; x >= 1; x--) {
	    p = concat_son_ptrs [x];

	    if (((p->name != star) && (p->name != option)) || !is_an_element (left_son, p->body.brother.left, or))
		break;
	}

	if (x == concat_no) {
	    tete_pop (2);
	    return (FALSE);
	}

	if (x == 0) {
	    tete_pop (2);
	    return (TRUE);
	}

	if (x == 1) {
	    p = concat_son_ptrs [1];

	    if (p->name == or) {
		cn = nb_op_sons (p, or);

		{
		    struct node		**c_s /* 1:cn */ ;
		    SXINT		y;
		    BOOLEAN	subsetp;

		    c_s = tete_open (cn);
		    y = 0;
		    gather_op_sons (c_s, &y, p, or);
		    subsetp = is_a_subset (or_son_ptrs, (SXINT)1, or_no, c_s, (SXINT)1, cn);
		    tete_pop (3);
		    return subsetp;
		}
	    }
	    else {
		elementp = is_an_element (left_son, p, or);
		tete_pop (2);
		return elementp;
	    }
	}
	else {
	    /* x>1 */
	    elementp = is_an_element (left_son, create_list (concat_son_ptrs, (SXINT)1, x, concat), or);
	    tete_pop (2);
	    return elementp;
	}
    }
}



static SXVOID	transform (struct node **left, struct node **right)
{
    struct node	*left_son, *right_son;
    SXINT				cn1, cn2;

    left_son = (*left)->body.brother.left;
    right_son = (*right)->body.brother.left;

    if (left_son->name != concat || right_son->name != concat)
	return;

    cn1 = nb_op_sons (left_son, concat);
    cn2 = nb_op_sons (right_son, concat);

    {
	struct node	**c1_s /* 1:cn1 */ , **c2_s /* 1:cn2 */ ;
	SXINT	x, y;
	struct node	*p, *or_ptr;

	c2_s = tete_open (cn2);
	x = 0;
	gather_op_sons (c2_s, &x, right_son, concat);

	if (c2_s [cn2]->name != star) {
	    tete_pop (1);
	    return;
	}

	or_ptr = c2_s [cn2]->body.brother.left;

	if (or_ptr->name != or) {
	    tete_pop (1);
	    return;
	}

	c1_s = tete_open (cn1);
	x = 0;
	gather_op_sons (c1_s, &x, left_son, concat);

	if (c1_s [cn1]->name != star) {
	    tete_pop (2);
	    return;
	}

	for (x = cn1 - 1; x >= 1; x--) {
	    p = c1_s [x];

	    if (p->name != star)
		break;
	}

	if (x == 0) {
	    tete_pop (2);
	    return;
	}


/* garde_fou */

	if (nb_op_sons (or_ptr, or) != cn1 - x + 1) {
            tete_pop (2);
	    return;
	}

	for (y = x + 1; y <= cn1; y++) {
	    if (!is_an_element (or_ptr, c1_s [y]->body.brother.left, or)) {
		tete_pop (2);
		return;
	    }
	}

	p = create_list (c1_s, (SXINT)1, x, concat);

	if (!is_an_element (or_ptr, p, or)) {
	    tete_pop (2);
	    return;
	}

	*right = NULL;
	c1_s [x] = canon (or, p, create_list (c2_s, (SXINT)1, cn2 - 1, concat));
	*left = canon (star, create_list (c1_s, x, cn1, concat), NULL);
	tete_pop (2);
    }
}



static BOOLEAN	less (SXINT i, SXINT j)
{
    return (long) (P_S [i]) < (long) (P_S [j]);
}



static struct node	*create_or_list (struct node **p_s, SXINT m, SXINT create_or_list_M)
{
    SXINT	x;
    struct node		*v, *w;

    P_S = p_s;

    if (to_be_sorted == NULL)
	to_be_sorted = (SXINT*) sxalloc ((size_of_to_be_sorted = create_or_list_M) + 1, sizeof (SXINT));
    else if (create_or_list_M > size_of_to_be_sorted)
	to_be_sorted = (SXINT*) sxrealloc (to_be_sorted, (size_of_to_be_sorted = create_or_list_M) + 1, sizeof (SXINT));
    
    for (x = 1; x <= create_or_list_M; x++) {
	to_be_sorted [x] = x;
    }

    sort_by_tree (to_be_sorted, m, create_or_list_M, less);
    /* Suppression des alternatives identiques et construction du nouvel arbre */
    v = p_s [create_or_list_M];

    for (x = create_or_list_M - 1; x >= m; x--) {
	w = p_s [x];

	if (v != w)
	    v = w;
	else
	    p_s [x] = NULL;
    }

    return (create_list (p_s, m, create_or_list_M, or));
}



static struct node	*canon (SXINT oper, struct node *left, struct node *right)
{
    SXINT		concat_no, or_no, left_name, cn;
    struct node		*p;

    switch (oper) {
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("fsa_to_re","unknown switch case #1");
#endif
      break;
    case leaf:
	/* leaf (error) */
	return NULL;

    case concat:
	/* op = concat */
/* On canonicalise */
	concat_no = nb_op_sons (left, concat) + nb_op_sons (right, concat);

	{
	    struct node	**p_s /* 1:concat_no */ ;
	    SXINT		x, xp, l_name, r_name, canon_cn;
	    struct node		*l_ptr, *r_ptr, *rl_ptr;

	    p_s = tete_open (concat_no);
	    x = 0;
	    gather_op_sons (p_s, &x, left, concat);
	    xp = x;
	    gather_op_sons (p_s, &x, right, concat);
	    l_ptr = p_s [xp];
	    r_ptr = p_s [xp + 1];
	    l_name = l_ptr->name;
	    r_name = r_ptr->name;

	    switch (S [l_name] [r_name]) {
	    case 0:
		/* error */
		fprintf (sxstderr, "Constructor error in fsa_to_re: unexpected node operator.");
		tete_pop (1);
		sxexit (1);
		/* FALLTHRU */
	    case 1:
		/* general case */
		break;

	    case 2:
		/* XYZ{YZ}V => X{YZ}+V */
		process_substr (p_s, xp + 1, xp, concat_no);
		break;

	    case 3:
		/* V{XY}XYZ => V{XY}+Z */
		process_substr (p_s, xp, xp + 1, concat_no);
		break;

	    case 4:
		/* ...{X}{Y}... */
		if (is_a_member (l_ptr, r_ptr) /* {U} {U|V} => {U|V} */ )
		    p_s [xp] = NULL;
		else if (is_a_member (r_ptr, l_ptr) /* {U|V} {U} => {U|V} */ )
		    p_s [xp + 1] = NULL;
		else if (is_tail (l_ptr, r_ptr->body.brother.left)
			 /* {U} {V{U}} => {U|V} */
								  ) {
		    rl_ptr = r_ptr->body.brother.left;
		    canon_cn = nb_op_sons (rl_ptr, concat);

		    {
			struct node	**q_s /* 1:canon_cn */ ;
			SXINT	canon_x;

			q_s = tete_open (canon_cn);
			canon_x = 0;
			gather_op_sons (q_s, &canon_x, rl_ptr, concat);
			p_s [xp] = NULL;
			p_s [xp + 1] = create_op (star, canon (or, l_ptr->body.brother.left, create_list (q_s, (SXINT)1, canon_cn - 1,
			     concat)), (struct node*)NULL);
			tete_pop (1);
		    }
		}
		else if (is_tail (r_ptr, l_ptr->body.brother.left)
			 /* {U{V}}{V} => {U{V}} */
								  )
		    p_s [xp + 1] = NULL;
		else if (is_a_sublanguage (l_ptr, r_ptr)
			 /* {X|Y|Z|T}{X[Z]{T}} => {X|Y|Z|T} */
							)
		    p_s [xp + 1] = NULL;
		else
		    transform (&(p_s [xp]), &(p_s [xp + 1]));
		/* {X{Y}{Z}}{T{X|Y|Z}} => {(X|T){Y}{Z}} */

		break;

	    case 5:
		/* [X] {Y} or {X} {Y}+ or [X] {Y}+ */
		if (is_a_member (l_ptr, r_ptr))
		    p_s [xp] = NULL;

		break;

	    case 6:
		/* {X} [Y] or {X}+ {Y} or {X}+ [Y] */
		if (is_a_member (l_ptr, r_ptr))
		    p_s [xp + 1] = NULL;

		break;
	    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
	      sxtrap("fsa_to_re","unknown switch case #2");
#endif
	      break;
	    }

	    l_ptr = create_list (p_s, (SXINT)1, concat_no, concat);
	    tete_pop (1);
	    return l_ptr;
	}

    case star:
	/* star */
	left_name = left->name;

	if (left_name == star /* {{X}} => {X} */ )
	    return (left);

	if (left_name == option /* {[X]} => {X} */  || left_name == plus
	    /* {{X}+} => {X} */
									)
	    return (create_op (star, left->body.brother.left, (struct node*)NULL));

	if (is_a_staror_elem (left, &p) /* {X{X|Y}} => {X{Y}} */ )
	    return (canon (star, p, (struct node*)NULL));

	if (left_name == concat) {
	    /* {[X] {Y}} => {X|Y} */
	    cn = nb_op_sons (left, concat);

	    {
		struct node	**q_s /* 1:cn */ ;
		SXINT	x;
		struct node	*l_ptr;

		q_s = tete_open (cn);
		x = 0;
		gather_op_sons (q_s, &x, left, concat);

		for (x = 1; x <= cn; x++) {
		    if (q_s [x]->name == star || q_s [x]->name == option)
			q_s [x] = q_s [x]->body.brother.left;
		    else {
			tete_pop (1);
			return (create_op (oper, left, right));
		    }
		}

		l_ptr = create_op (star, create_or_list (q_s, (SXINT)1, cn), (struct node*)NULL);
		tete_pop (1);
		return l_ptr;
	    }
	}

	break;

    case or:
	/* or */
	{
	    struct node	**p_s /* 1:or_no */ ;
	    struct node		*l_ptr;
	    SXINT		x;
	    BOOLEAN is_option = FALSE;

	    /* [X]|Y or X|[Y] or [X]|[Y] => [X|Y] */
	    if (left->name == option) {
		left = left->body.brother.left;
		is_option = TRUE;
	    }

	    if (right->name == option) {
		right = right->body.brother.left;
		is_option = TRUE;
	    }

	    or_no = nb_op_sons (left, or) + nb_op_sons (right, or);
	    p_s = tete_open (or_no);
	    x = 0;
	    gather_op_sons (p_s, &x, left, or);
	    gather_op_sons (p_s, &x, right, or);
	    l_ptr = create_or_list (p_s, (SXINT)1, or_no);
	    tete_pop (1);
	    return is_option ? create_op (option, l_ptr, (struct node*)NULL) : l_ptr;
	}

    case plus:
	/* plus */
	left_name = left->name;

	if (left_name == plus /* {{X}+}+ => {X}+ */  || left_name == star
	    /* {{X}}+ => {X} */
									 )
	    return (left);

	if (left_name == option /* {[X]}+ => {X} */ )
	    return (create_op (star, left->body.brother.left, (struct node*)NULL));

	break;

    case option:
	/* option */
	left_name = left->name;

	if (left_name == star /* [{X}] => {X} */  || left_name == option
	    /* [[X]] => [X] */
									)
	    return (left);

	if (left_name == plus /* [{X}+] => {X} */ )
	    return (create_op (star, left->body.brother.left, (struct node*)NULL));

	if (is_a_staror_elem (left, &p) /* [X{X|Y}] => {X{Y}} */ )
	    return (canon (star, p, (struct node*)NULL));

	break;
    }

    return (create_op (oper, left, right));
}



static SXBA_INDEX	get_a_pred (SXBA_INDEX n)
{
    SXBA_INDEX	i = -1;

    while ((i = sxba_scan (to_be_processed, i)) >= 0) {
	if (SXBA_bit_is_set (M [i], n))
	    break;
    }

    return i;
}



static SXVOID	gen_a_loop (SXBA_INDEX pred, SXBA_INDEX next, SXBA_INDEX loop)
{
    /* Il y a une boucle elementaire sur l'etat loop
soit t=trans(pred,next) et l=trans(loop,loop)
si t=l alors 
    si pred=next=loop alors trans(pred,next)={t}
    sinon trans(pred,next)={t}+
sinon si next=loop alors trans(pred,next)=t {l}
      sinon si pred=loop alors trans(pred,next)= {l} t
            sinon erreur
*/
    struct node		*t, *l;

    if ((l = re_ptrs [loop] [loop]) == (t = re_ptrs [pred] [next]))
	if (pred == next && next == loop) {
	    re_ptrs [pred] [next] = canon (star, l, (struct node*)NULL);
	}
	else {
	    re_ptrs [pred] [next] = canon (plus, l, (struct node*)NULL);
	}
    else if (next == loop)
	re_ptrs [pred] [next] = canon (concat, t, canon (star, l, (struct node*)NULL));
    else
	re_ptrs [pred] [next] = canon (concat, canon (star, l, (struct node*)NULL), t);
}


static SXVOID	gen_splits (SXBA_INDEX pred, SXBA_INDEX next)
{
    /* Calcule les chemins entre pred et succ(next)*/
    SXBA_INDEX		succ;
    struct node	*p1, *p2, *q;

    if (succ_set == NULL)
	succ_set = sxba_calloc (SIZE);

    sxba_copy (succ_set, M [next]);
    /* M [next] est peut etre change dans la boucle.. */
    p1 = re_ptrs [pred] [next];

    succ = -1;


    while ((succ = sxba_scan (succ_set, succ)) >= 0) {
	p2 = re_ptrs [next] [succ];

	if (SXBA_bit_is_set (M [pred], succ)) {
	    /* Il y a deja une ER */
	    if ((q = re_ptrs [pred] [succ]) == p2)
		re_ptrs [pred] [succ] = canon (concat, canon (option, p1, (struct node*)NULL), q);
	    else
		re_ptrs [pred] [succ] = canon (or, canon (concat, p1, p2), q);
	}
	else {
	    re_ptrs [pred] [succ] = canon (concat, p1, p2);
	    SXBA_1_bit (M [pred], succ);
	    ++(out [pred]);
	    ++(in [succ]);
	}

	if (pred == succ)
	    if (pred != (SXBA_INDEX)INITIAL_S)
		SXBA_1_bit (loop_set, pred);
    }
}



static VOID	erase (SXBA_INDEX pred, SXBA_INDEX next)
{
    /* Supprime la transition entre pred et next */
    SXBA_0_bit (M [pred], next);
    --(out [pred]);
    --(in [next]);
}



static SXBA_INDEX	get_current_node (SXBA get_current_node_to_be_processed)
{
    SXINT s, n, e;
    SXBA_INDEX i, cur;

    s = e = SIZE;
    cur = -1;
    i = -1;

    while ((i = sxba_scan (get_current_node_to_be_processed, i)) >= 0) {
	if ((n = in [i]) < e) {
	    cur = i;
	    e = n;
	}
	else if (n == e)
	    if ((n = out [i]) < s) {
		cur = i;
		s = n;
	    }
    }

    return (cur);
}


static	void chose_larger_set (struct node ***p_s, SXBA set, SXINT *mM, SXINT *best_card, SXBA best_set, struct node **best_p, SXINT or_no);
static struct node	*process (struct node ***p_s, SXBA set, SXINT or_no, SXINT *min_s, SXINT *MAX_s)
{
    SXBA	/* or_no */ pre_set, post_set, middle_set;
    SXINT		pre_card, post_card;
    struct node		*pre_p, *post_p, *ret_ptr;

    pre_set = sets_open (or_no);
    post_set = sets_open (or_no);
    middle_set = sets_open (or_no);
    chose_larger_set (p_s, set, min_s, &pre_card, pre_set, &pre_p, or_no);
    chose_larger_set (p_s, set, MAX_s, &post_card, post_set, &post_p, or_no);

    if (pre_card >= post_card) {
	sxba_minus (sxba_copy (middle_set, set), pre_set);
	pre_p = gen_prefixe (p_s, pre_set, pre_p, or_no, min_s, MAX_s);

	if (!sxba_is_empty (middle_set))
	    ret_ptr = canon (or, pre_p, process (p_s, middle_set, or_no, min_s, MAX_s));
	else
	    ret_ptr = pre_p;
    }
    else {
	sxba_minus (sxba_copy (middle_set, set), post_set);
	post_p = gen_suffixe (p_s, post_set, post_p, or_no, min_s, MAX_s);

	if (!sxba_is_empty (middle_set))
	    ret_ptr = canon (or, process (p_s, middle_set, or_no, min_s, MAX_s), post_p);
	else
	    ret_ptr = post_p;
    }

    sets_pop (3);
    return ret_ptr;
}



static struct node	*factorize (struct node *or_ptr)
{
    SXINT	or_no;

    or_no = nb_op_sons (or_ptr, or);

    {
	SXINT	*min_s, *MAX_s /* 1:or_no */ ;
	SXINT	i;
	SXINT		concat_no, j;
	struct node	*p;

	min_s = ints_open (or_no);
	MAX_s = ints_open (or_no);
	i = concat_no = 0;

	for (p = or_ptr; p->name == or; p = p->body.brother.right) {
	    min_s [++i] = 1;
	    MAX_s [i] = j = nb_op_sons (p->body.brother.left, concat);
	    concat_no = (j > concat_no) ? j : concat_no;
	}

	min_s [++i] = 1;
	MAX_s [i] = j = nb_op_sons (p, concat);
	concat_no = (j > concat_no) ? j : concat_no;

	{
	    struct node		***p_s /* 1:or_no, 1:concat_no */ ;
	    SXBA	/* or_no */ set;
	    SXINT		x;
	    struct node		*ret_ptr;

	    p_s = (struct node***) sxcalloc ((SXUINT) (or_no + 1), sizeof (struct node**));
	    p_s [0] = NULL;

	    for (i = 1; i <= or_no; i++)
		p_s [i] = tete_open (concat_no);

	    i = 0;

	    for (p = or_ptr; p->name == or; p = p->body.brother.right) {
		x = 0;
		gather_op_sons (p_s [++i], &x, p->body.brother.left, concat);
	    }

	    x = 0;
	    gather_op_sons (p_s [++i], &x, p, concat);
	    set = sets_open (or_no);
	    sxba_fill (set), SXBA_0_bit (set, 0);
	    ret_ptr = process (p_s, set, or_no, min_s, MAX_s);
	    sets_pop (1);
	    tete_pop (or_no);
	    sxfree (p_s);
	    ints_pop (2);
	    return ret_ptr;
	}
    }
}



static struct node	*reduce (struct node *visited)
{
    switch (visited->name) {
    case or:
	return (factorize (canon (or, reduce (visited->body.brother.left), reduce_or_son (visited->body.brother.right))))
	     ;

    case concat:
	return (canon (concat, reduce (visited->body.brother.left), reduce (visited->body.brother.right)));

    case star:
    case plus:
    case option:
	return (canon (visited->name, reduce (visited->body.brother.left), (struct node*)NULL));

    case leaf:
	return (visited);
    default:
      return (struct node*)NULL; /* Pour compilos tatillons... */
    }
}



static struct node	*reduce_or_son (struct node *visited)
{
    if (visited->name == or)
	return (canon (or, reduce (visited->body.brother.left), reduce_or_son (visited->body.brother.right)));
    else
	return (reduce (visited));
}



static	void chose_larger_set (struct node ***p_s /* 1:or_no, 1:concat_no */, SXBA set, SXINT *mM, SXINT *best_card, SXBA best_set, struct node **best_p, SXINT or_no)
{
    SXBA	/* or_no */ working_set, c_set;
    SXBA_INDEX	i, j;
    SXINT        c_card;
    struct node		*c_p;

    working_set = sets_open (or_no);
    c_set = sets_open (or_no);
    sxba_copy (working_set, set);
    *best_card = 0;
    *best_p = NULL;
    sxba_empty (best_set);
    i = 0;

    while ((i = sxba_scan_reset (working_set, 0)) > 0) {
	sxba_empty (c_set);
	SXBA_1_bit (c_set, i);
	c_card = 1;
	c_p = p_s [i] [mM [i]];
	j = i;

	while ((j = sxba_scan (working_set, j)) >= 0) {
	    if (c_p == p_s [j] [mM [j]]) {
		SXBA_1_bit (c_set, j);
		c_card++;
		SXBA_0_bit (working_set, j);
	    }
	}

	if (c_card > *best_card) {
	    *best_card = c_card;
	    *best_p = c_p;
	    sxba_copy (best_set, c_set);
	}
    }

    sets_pop (2);
}



static struct node	*gen_prefixe (struct node ***p_s, SXBA set, struct node *node_ptr, SXINT or_no, SXINT *min_s, SXINT *MAX_s)
{
    SXBA_INDEX         i;
    SXINT		j;
    BOOLEAN	is_option, are_the_same;
    struct node		*c_ptr, *ret_ptr;
    SXBA	/* or_no */ c_set;

    c_set = sets_open (or_no);
    is_option = FALSE;
    sxba_copy (c_set, set);
    i = sxba_scan (set, 0); /* Element 0 non utilise. */
    j = ++(min_s [i]);

    if (j > MAX_s [i]) {
	is_option = TRUE;
	SXBA_0_bit (c_set, i);
	c_ptr = NULL;
    }
    else
	c_ptr = p_s [i] [j];

    are_the_same = TRUE;

    while ((i = sxba_scan (c_set, i)) >= 0) {
	j = ++(min_s [i]);

	if (j > MAX_s [i]) {
	    is_option = TRUE;
	    SXBA_0_bit (c_set, i);
	}

	if (c_ptr != p_s [i] [j])
	    are_the_same = FALSE;
    }

    if (is_option) {
	if (!sxba_is_empty (c_set))
	    ret_ptr = canon (concat, node_ptr, canon (option, process (p_s, c_set, or_no, min_s, MAX_s), (struct node*)NULL));
	else
	    ret_ptr = node_ptr;
    }
    else if (are_the_same)
	ret_ptr = canon (concat, node_ptr, gen_prefixe (p_s, set, c_ptr, or_no, min_s, MAX_s));
    else
	ret_ptr = canon (concat, node_ptr, process (p_s, c_set, or_no, min_s, MAX_s));

    sets_pop (1);
    return ret_ptr;
}



static struct node	*gen_suffixe (struct node ***p_s, SXBA set, struct node *node_ptr, SXINT or_no, SXINT *min_s, SXINT *MAX_s)
{
    SXBA_INDEX i;
    SXINT	j;
    BOOLEAN	is_option, are_the_same;
    struct node		*c_ptr, *ret_ptr;
    SXBA	/* or_no */ c_set;

    c_set = sets_open (or_no);
    is_option = FALSE;
    sxba_copy (c_set, set);
    i = sxba_scan (set, -1);
    j = --(MAX_s [i]);

    if (j < min_s [i]) {
	is_option = TRUE;
	SXBA_0_bit (c_set, i);
	c_ptr = NULL;
    }
    else
	c_ptr = p_s [i] [j];

    are_the_same = TRUE;

    while ((i = sxba_scan (c_set, i)) >= 0) {
	j = --(MAX_s [i]);

	if (j < min_s [i]) {
	    is_option = TRUE;
	    SXBA_0_bit (c_set, i);
	}

	if (c_ptr != p_s [i] [j])
	    are_the_same = FALSE;
    }

    if (is_option) {
	if (!sxba_is_empty (c_set))
	    ret_ptr = canon (concat, canon (option, process (p_s, c_set, or_no, min_s, MAX_s), (struct node*)NULL), node_ptr);
	else
	    ret_ptr = node_ptr;
    }
    else if (are_the_same)
	ret_ptr = canon (concat, gen_suffixe (p_s, set, c_ptr, or_no, min_s, MAX_s), node_ptr);
    else
	ret_ptr = canon (concat, process (p_s, c_set, or_no, min_s, MAX_s), node_ptr);

    sets_pop (1);
    return ret_ptr;
}




static struct node	*cut (struct node *node_ptr)
{
    if (node_ptr == NULL) {
	/* sous arbre deja coupe par ailleurs */
	return (struct node*)NULL;
    }

    switch (node_ptr->name) {
    case leaf:
	return (*(node_ptr->body.string_ptr) == NUL) ? (NULL) : (node_ptr);

    case concat:
    case or:
	node_ptr->body.brother.left = cut (node_ptr->body.brother.left);
	node_ptr->body.brother.right = cut (node_ptr->body.brother.right);

	return (node_ptr->body.brother.left == NULL && node_ptr->body.brother.right == NULL) ? (NULL) : (node_ptr);

    case star:
    case plus:
    case option:
	node_ptr->body.brother.left = cut (node_ptr->body.brother.left);

	return (node_ptr->body.brother.left == NULL) ? (NULL) : (node_ptr);
    default:
      return (struct node*)NULL; /* Pour compilos tatillons... */
    }
}



static VARSTR	tree_to_string (VARSTR varstr_ptr, struct node *node_ptr)
{
  VARSTR	ret_ptr;
  struct node           *left, *right;

  if (node_ptr == NULL)
    return (varstr_ptr);

  switch (node_ptr->name) {
  case leaf:
    ret_ptr = varstr_catenate (varstr_ptr, node_ptr->body.string_ptr);
    break;

  case concat:
    /* version du 17/01/06 */
    left = node_ptr->body.brother.left;
    right = node_ptr->body.brother.right;

    if (left) {
      if (left->name == or)
	varstr_ptr = varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr,
								       "("),
						      left),
				      ")");
      else
	varstr_ptr = tree_to_string (varstr_ptr, left);

      if (right)
	varstr_ptr = varstr_catchar (varstr_ptr, ' ');
    }

    if (right) {
      if (right->name == or)
	varstr_ptr = varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr,
								       "("),
						      right),
				      ")");
      else
	varstr_ptr = tree_to_string (varstr_ptr, right);
    }

    return varstr_ptr;

    /* NOTREACHED break; */

  case star:
    ret_ptr = varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr, meta_op ? meta_op [0] : "{"), node_ptr->body.brother.left), meta_op ? meta_op [1] : "}");
    break;

  case or:
    if (node_ptr->body.brother.left == NULL)
      ret_ptr = tree_to_string (varstr_ptr, node_ptr->body.brother.right);
    else if (node_ptr->body.brother.right == NULL)
      ret_ptr = tree_to_string (varstr_ptr, node_ptr->body.brother.left);
    else
      ret_ptr = tree_to_string (varstr_catenate (tree_to_string (varstr_ptr, node_ptr->body.brother.left), " | "),
				node_ptr->body.brother.right);

    break;

  case plus:
    ret_ptr = varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr, meta_op ? meta_op [2] : "{"), node_ptr->body.brother.left), meta_op ? meta_op [3] : "}+")
      ;
    break;

  case option:
    ret_ptr = varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr, meta_op ? meta_op [4] : "["), node_ptr->body.brother.left), meta_op ? meta_op [5] : "]");
    break;
  default: /* pour faire taire gcc -Wswitch-default */
    sxinitialise(ret_ptr); /* pour faire taire gcc -Wuninitialized */			      
#if EBUG
    sxtrap("fsa_to_re","unknown switch case #3");
#endif
    break;
  }

  return ret_ptr;
}




static VARSTR
fsa2re_body (VARSTR varstr_ptr, SXBA *R, SXBA *R_plus, SXINT size, SXINT initial_s, SXINT final_s, SXINT (*get_name_refs) (SXBA_INDEX, SXBA_INDEX, char **))
{
    SXBA_INDEX i, j;
    SXBA	and_set;
    struct node		**re_ptrs_base;

/* Cette fonction transforme l'ensemble des chemins entre initial_s et final_s
   de l'automate R en une expression reguliere. Le nom de la transition
   entre l'etat i et l'etat j est donne par la procedure
   utilisateur get_name_refs (i, j, string_ptr)
*/
/* Les etats de l'automate sont dans [0..size]. */
 /* Automate sous forme de vecteur de bits et sa fermeture transitive */

    if (!SXBA_bit_is_set (R_plus [initial_s], final_s))
	return varstr_ptr;

    SIZE = size + 1;
    INITIAL_S = initial_s;
    tree_area_ptr = NULL;
    cur_node_no = node_number_per_area;
    M = sxbm_calloc (SIZE, SIZE);
    re_ptrs = (struct node***) sxalloc ( SIZE, sizeof (struct node**));

    {
	struct node	**p;

	re_ptrs_base = p = (struct node**) sxcalloc ((SXUINT) (SIZE * SIZE), sizeof (struct node*));

	for (i = 0; i <= (SXBA_INDEX)size; i++) {
	    re_ptrs [i] = p;
	    p += SIZE;
	}
    }

    in = (SXINT*) sxcalloc ((SXUINT) SIZE, sizeof (SXINT));
    out = (SXINT*) sxcalloc ((SXUINT) SIZE, sizeof (SXINT));
    ints = NULL;
    sets = NULL;
    to_be_processed = sets_open (size);
    loop_set = sets_open (size);
    and_set = sets_open (size);
    tete = NULL;
    to_be_sorted = NULL;
    succ_set = NULL;

    for (i = 0; i < head_s_no; i++)
	leafs_hd [i] = head_s [i] = NULL;

    SXBA_1_bit (to_be_processed, initial_s);
    SXBA_1_bit (to_be_processed, final_s);

    for (i = 0; i <= (SXBA_INDEX)size; i++) {
	if (SXBA_bit_is_set (R_plus [initial_s], i) && SXBA_bit_is_set (R_plus [i], final_s))
	    SXBA_1_bit (to_be_processed, i);
    }

    i = -1;

    while ((i = sxba_scan (to_be_processed, i)) >= 0) {
	sxba_and (sxba_copy (M [i], R [i]), to_be_processed);
	reached_set = M [i];
	j = -1;

	while ((j = sxba_scan (reached_set, j)) >= 0) {
	    re_ptrs [i] [j] = create_leaf (i, j, get_name_refs);
	    (out [i])++;
	    (in [j])++;

	    if (i == j)
		SXBA_1_bit (loop_set, i);
	}
    }

    SXBA_0_bit (loop_set, initial_s);

    sxba_copy (and_set, to_be_processed);
    SXBA_0_bit (and_set, initial_s);
    SXBA_0_bit (and_set, final_s);

    while (!sxba_is_empty (and_set)) {
	i = -1;

	while ((i = sxba_scan_reset (loop_set, i)) >= 0) {
	    j = -1;

	    while ((j = sxba_scan (to_be_processed, j)) >= 0) {
		if (SXBA_bit_is_set (M [j], i) && i != j)
		    gen_a_loop (j, i, i);
	    }

	    erase (i, i);
	}

	current = get_current_node (and_set);
	prev = get_a_pred (current);
	gen_splits (prev, current);
	erase (prev, current);

	if (in [current] == 0) {
	    SXBA_0_bit (to_be_processed, current);
	    out [current] = 0;
	    reached_set = M [current];
	    i = -1;

	    while ((i = sxba_scan (reached_set, i)) >= 0)
		(in [i])--;

	    sxba_empty (M [current]);
	}

	sxba_copy (and_set, to_be_processed);
	SXBA_0_bit (and_set, initial_s);
	SXBA_0_bit (and_set, final_s);
    }


/* Cas particulier de boucle sur l'etat final */

    if (SXBA_bit_is_set (M [final_s], final_s)) {
	gen_a_loop ((SXBA_INDEX)initial_s, (SXBA_INDEX)final_s, (SXBA_INDEX)final_s);
	erase ((SXBA_INDEX)final_s, (SXBA_INDEX)final_s);
    }


/* Cas particulier de la transition de l'etat final a l'etat initial */

    if (SXBA_bit_is_set (M [final_s], initial_s))
	gen_splits ((SXBA_INDEX)initial_s, (SXBA_INDEX)final_s);


/* Cas particulier de boucle sur l'etat initial */

    if (SXBA_bit_is_set (M [initial_s], initial_s)) {
	gen_a_loop ((SXBA_INDEX)initial_s, (SXBA_INDEX)final_s, (SXBA_INDEX)initial_s);
	erase ((SXBA_INDEX)initial_s, (SXBA_INDEX)initial_s);
    }


/* Factorisation */

    re_ptrs [initial_s] [final_s] = reduce (re_ptrs [initial_s] [final_s]);
    /* on supprime les sous arbres ne produisant que la chaine vide */
    re_ptrs [initial_s] [final_s] = cut (re_ptrs [initial_s] [final_s]);
    varstr_ptr = tree_to_string (varstr_ptr, re_ptrs [initial_s] [final_s]);
    sxfree (out);
    sxfree (in);
    tete_free ();
    sets_free ();
    ints_free ();

    if (to_be_sorted != NULL)
	sxfree (to_be_sorted);

    if (succ_set != NULL)
	sxfree (succ_set);

    sxfree (re_ptrs_base);
    sxfree (re_ptrs);
    sxbm_free (M);
    free_tree_areas ();
    return varstr_ptr;
}


/* Version analogue a fsa_to_re, mais on passe en plus les operateurs kleene et option */
VARSTR
fsa2re (VARSTR varstr_ptr, SXBA *R, SXBA *R_plus, SXINT size, SXINT initial_s, SXINT final_s, SXINT (*get_name_refs) (SXBA_INDEX, SXBA_INDEX, char **), char *op [6] /* op [6] = {"(", ")*", "(", ")+", "(", ")?"}; */)
{
  meta_op = op;
  return fsa2re_body (varstr_ptr, R, R_plus, size, initial_s, final_s, get_name_refs);
}


/* Version analogue a fsa_to_re, mais on passe en plus les operateurs kleene et option */
VARSTR
fsa_to_re (VARSTR varstr_ptr, SXBA *R, SXBA *R_plus, SXINT size, SXINT initial_s, SXINT final_s, SXINT (*get_name_refs) (SXBA_INDEX, SXBA_INDEX, char **))
{
  meta_op = NULL;
  return fsa2re_body (varstr_ptr, R, R_plus, size, initial_s, final_s, get_name_refs);
}



static SXUINT          eof_trans;
static SXINT           dag_hd_trans_nb, final_state;
#if EBUG
static VARSTR          wvstr;
static SXINT            make_code_level_call;
#endif /* EBUG */
static XxY_header      dag_hd;
static XH_header       code_hd;
static SXINT             *code2size;

static struct dag_attr {
  SXINT   trans;
} *dag_id2attr;

static char *(*dag_get_trans_name)(SXINT);

#define ATOMIC_TRANS   (SXUINT)1
#define CAT_TRANS      (SXUINT)2
#define OR_TRANS       (SXUINT)3
#define OPTION_TRANS   (SXUINT)4
#define FOR_TRANS      (SXUINT)5
#define MAX_TRANS      (SXUINT)15

#define SHIFT          (SXBITS_PER_LONG-4)


#define ATOMIC_CODE   (ATOMIC_TRANS << SHIFT)
#define CAT_CODE      (CAT_TRANS << SHIFT)
#define OR_CODE       (OR_TRANS << SHIFT)
#define OPTION_CODE   (OPTION_TRANS << SHIFT)
#define FOR_CODE      (FOR_TRANS << SHIFT)
#define ID_CODE       (~(MAX_TRANS << SHIFT))

#define code2id(c)    (SXINT)((c) & ID_CODE)
#define code2trans(c) ((c)>> SHIFT)

static VARSTR
print_code (VARSTR vstr, SXINT code)
{
  SXINT   trans_kind, id, bot, top, cur, code1, trans1_kind;
  BOOLEAN atomic;

  trans_kind = code2trans (code);
  id = code2id (code);
  
  if (trans_kind == ATOMIC_TRANS)
    vstr = varstr_catenate (vstr, (*dag_get_trans_name) (id));
  else {
    bot = XH_X (code_hd, id);
    top = XH_X (code_hd, id+1);

    switch (trans_kind) {
    case OPTION_TRANS :
      code1 = XH_list_elem (code_hd, bot);
      atomic = bot == top-1 && code2trans (code1) == ATOMIC_TRANS;

      if (!atomic)
	vstr = varstr_lcatenate (vstr, "(", 1);

      vstr = print_code (vstr, code1);
      
      for (cur = bot+1; cur < top; cur++) {
	vstr = varstr_lcatenate (vstr, " ", 1);
	vstr = print_code (vstr, XH_list_elem (code_hd, cur));
      }

      if (atomic)
	vstr = varstr_lcatenate (vstr, "?", 1);
      else
	vstr = varstr_lcatenate (vstr, ")?", 2);

      break;

    case CAT_TRANS :
      for (cur = bot; cur < top; cur++) {
	if (cur > bot)
	  vstr = varstr_lcatenate (vstr, " ", 1);

	code1 = XH_list_elem (code_hd, cur);
	trans1_kind = code2trans (code1);

	if (trans1_kind == OR_TRANS|| trans1_kind == FOR_TRANS)
	  vstr = varstr_lcatenate (vstr, "(", 1);
	  
	vstr = print_code (vstr, code1);

	if (trans1_kind == OR_TRANS|| trans1_kind == FOR_TRANS)
	  vstr = varstr_lcatenate (vstr, ")", 1);
      }

      break;

    case OR_TRANS :
    case FOR_TRANS :
      /* L'appelant a sorti les parentheses eventuelles */
      for (cur = bot; cur < top; cur++) {
	if (cur > bot)
	  vstr = varstr_lcatenate (vstr, " | ", 3);

	code1 = XH_list_elem (code_hd, cur);

	/* Pas de OR/FOR ds un OR/FOR */
#if EBUG
	trans1_kind = code2trans (code1);

	if (trans1_kind == OR_TRANS || trans1_kind == FOR_TRANS)
	  sxtrap (ME, "print_code");
#endif /* EBUG */
	  
	vstr = print_code (vstr, code1);
      }

      break;

    default :
      sxtrap (ME, "print_code");
      break;
    }
  }

  return vstr;
}



static XxY_header paths, branches, pathXbranch, pathXtree;
static XH_header  trees;
static SXINT      *path2lgth, *path2tree, *branch2occur_nb, *tree2size, *tree2occur_nb, *code_stack;
static SXINT      *paths_stack;
static SXBA       branches_set, paths_set, final_paths_set;

static void
branches_oflw (SXINT old_size, SXINT new_size)
{
  sxuse (old_size);
  branch2occur_nb = (SXINT *) sxrealloc (branch2occur_nb, new_size+1, sizeof (SXINT));
  branches_set = sxba_resize (branches_set, new_size+1);
}

static void
paths_oflw (SXINT old_size, SXINT new_size)
{
  sxuse (old_size);
  path2lgth = (SXINT *) sxrealloc (path2lgth, new_size+1, sizeof (SXINT));
  path2tree = (SXINT *) sxrealloc (path2tree, new_size+1, sizeof (SXINT));
  paths_set = sxba_resize (paths_set, new_size+1);
  final_paths_set = sxba_resize (final_paths_set, new_size+1);
  paths_stack = (SXINT *) sxrealloc (paths_stack, new_size+1, sizeof (SXINT));
}

static void
trees_oflw (SXINT old_size, SXINT new_size)
{
  sxuse (old_size);
  tree2size = (SXINT *) sxrealloc (tree2size, new_size+1, sizeof (SXINT));
  tree2occur_nb = (SXINT *) sxrealloc (tree2occur_nb, new_size+1, sizeof (SXINT));
}

static BOOLEAN
set_code (SXINT *id)
{
  SXINT   size, bot, cur, top, code;
  BOOLEAN ret_val;

  ret_val = XH_set (&code_hd, id);

  if (!ret_val) {
    /* 1ere fois, on calcul size */
    size = 0;
    bot = XH_X (code_hd, *id);
    top = XH_X (code_hd, *id+1);

    for (cur = bot; cur < top; cur++) {
      code = XH_list_elem (code_hd, cur);

      if (code2trans (code) == ATOMIC_TRANS)
	size++;
      else
	size += code2size [code2id (code)];
    }

    code2size [*id] = size;
  }

  return ret_val;
}

static SXINT gen_branch (SXINT branch, SXINT path, SXBA set, SXBA final_set);

#if 0
/* Reecrit un chemin path dans code_stack 
   L'element gauche de path se trouvera en sommet de pile
   retourne le top de code_stack avant le stockage */
static SXINT
gen_path (SXINT path)
{
  SXINT code, bot, prev_path;

  bot = DTOP (code_stack);

  if (path) {
    prev_path = path;

    while (prev_path) {
      code = XxY_Y (paths, prev_path);
      DPUSH (code_stack, code);
      prev_path = XxY_X (paths, prev_path);
    }
  }

  return bot;
}
#endif /* 0 */


static SXINT
gen_tree (SXINT tree, SXINT path, SXBA set, SXBA final_set)
{
  SXINT   bot, cur, top, branch, code2, top_tree_pos, branch_pos, code, nb, lgth, prev_branch_pos, prev_lgth;
  BOOLEAN is_leaf;

  sxinitialise (prev_lgth);

  top_tree_pos = DTOP (code_stack);

  if (tree) {
    bot = XH_X (trees, tree);
    top = XH_X (trees, tree+1);
    is_leaf = FALSE;

    if (final_set && SXBA_bit_is_set (final_set, path))
      is_leaf = TRUE;

    branch = XH_list_elem (trees, bot);

    if (branch == 0) {
      is_leaf = TRUE;
      bot++;
    }
  
    nb = 0;

    for (cur = bot; cur < top; cur++) {
      branch = XH_list_elem (trees, cur);

      branch_pos = gen_branch (branch, path, set, final_set);

      lgth = DTOP (code_stack)-branch_pos;

      if (lgth) {
	if (++nb == 1) {
	  prev_branch_pos = branch_pos;
	  prev_lgth = lgth;
	}
	else {
	  if (nb == 2) {
	    if (lgth > 1) {
	      /* Vraie cat */
	      while (DTOP (code_stack) > branch_pos) {
		code = DPOP (code_stack);
		XH_push (code_hd, code);
	      }

	      set_code (&code);
	      code |= CAT_CODE;
	    }
	    else
	      code = DPOP (code_stack);

	    if (prev_lgth > 1) {
	      while (prev_lgth-- > 0 ) {
		code2 = DPOP (code_stack);
		XH_push (code_hd, code2);
	      }

	      set_code (&code2);
	      code2 |= CAT_CODE;
	      DPUSH (code_stack, code2);
	    }
	      
	    DPUSH (code_stack, code);
	  }
	  else {
	    if (lgth > 1) {
	      /* Vraie cat */
	      while (DTOP (code_stack) > branch_pos) {
		code = DPOP (code_stack);
		XH_push (code_hd, code);
	      }

	      set_code (&code);
	      code |= CAT_CODE;
	      DPUSH (code_stack, code); /* stockage temporaire */
	    }
	  }
	}
      }
    }

    if (nb) {
      if (is_leaf) {
	/* Il faut faire une OPTION */
	if (nb == 1) {
	  if (prev_lgth > 1) {
	    while (prev_lgth-- > 0 ) {
	      code = DPOP (code_stack);
	      XH_push (code_hd, code);
	    }

	    set_code (&code);
	    code |= CAT_CODE;
	  }
	  else
	    code = DPOP (code_stack);
	}
	else {
	  while (nb-- > 0) {
	    code = DPOP (code_stack);
	    XH_push (code_hd, code);
	  }
      
	  set_code (&code);
	  code |= FOR_CODE;
	}
	    
	XH_push (code_hd, code);
	set_code (&code);
	code |= OPTION_CODE;
	DPUSH (code_stack, code);
      }
      else {
	if (nb == 1) {
	  /* On ne fait rien, meme sur la concat eventuelle (si prev_lgth > 1) */
	}
	else {
	  /* alternatives multiples, on en fait un vrai FOR */
	  while (nb-- > 0) {
	    code = DPOP (code_stack);
	    XH_push (code_hd, code);
	  }
      
	  set_code (&code);
	  code |= FOR_CODE;
	  DPUSH (code_stack, code);
	}
      }
    }
  }

  return top_tree_pos;
}


/* branch = (id, tree), on met tree puis id ds code_stack et on retourne l'ancien sommet de code_stack */
static SXINT
gen_branch (SXINT branch, SXINT path, SXBA set, SXBA final_set)
{
  SXINT tree, tree_pos, id, pos, next_path;

  pos = DTOP (code_stack);
  id = XxY_X (branches, branch);

  if (set) {
    next_path = XxY_is_set (&paths, path, id);

    if (!SXBA_bit_is_set (set, next_path))
      return pos;
  }
  else
    next_path = 0;

  tree = XxY_Y (branches, branch);

  if (tree)
    tree_pos = gen_tree (tree, next_path, set, final_set);

  DPUSH (code_stack, id);

  return pos;
}


/* On a decide de factoriser "branch" */
/* Si path1, ... pathp sont les chemins menant a branch on va produire qqchose de la forme
   (path1 | ... | pathp) branch
*/
static SXINT
factorize_branch (SXINT branch)
{
  SXINT   bot_branch_pos;

  bot_branch_pos = gen_branch (branch, 0, NULL, NULL);

  gen_tree (path2tree [0] /* arbre a la racine */, 0 /* path de la racine */, paths_set /* Il faut passer par la */, final_paths_set);

  /* c'est l'appelant qui va gerer le contenu de code_stack */
  return bot_branch_pos;
}

/* On complete path_set avec tous les maillons */
static void
expand_paths (void)
{
  SXINT path, prev_path;

  while (!IS_EMPTY (paths_stack)) {
    path = POP (paths_stack);
    prev_path = XxY_X (paths, path);

    if (prev_path && SXBA_bit_is_reset_set (paths_set, prev_path))
      PUSH (paths_stack, prev_path);
  }
}


static SXINT
path2cost (SXINT path)
{
  SXINT next_path;

  if (path == 0)
    return 0;

  XxY_Xforeach (paths, path, next_path) {
    if (!SXBA_bit_is_set (paths_set, next_path)) {
      /* next_path n'est pas un chemin de path_set, il va donc falloir
	 dupliquer path */
      return path2lgth [path];
    }
  }

  return path2cost (XxY_X (paths, path));
}


/* On "genere" ds code_stack (path1 | ... | pathn) tree */
/* Les "bons" pathi sont ds paths_set */
static SXINT
factorize_tree (SXINT tree)
{
  SXINT bot_tree_pos;

  bot_tree_pos = gen_tree (tree, 0, NULL, NULL);

  gen_tree (path2tree [0] /* arbre a la racine */, 0 /* path de la racine */, paths_set, final_paths_set);

  /* c'est l'appelant qui va gerer le contenu de code_stack */
  return bot_tree_pos;
}

/* Un tree associe' a un noeud n est une liste ordonnee de couple n_tree = (a, p_tree)
   si n a une transition sur a conduisant au noeud p
   et p_tree est l'arbre associe' au noeud p */
static SXINT
make_tree (SXINT path)
{
  SXINT   size, next_path, next_tree, next_branch, path_branch, tree, path_tree, code, bot, top;
  BOOLEAN is_leaf = FALSE;

  size = 0;
  bot = DTOP (code_stack);

  XxY_Xforeach (paths, path, next_path) {
    code = XxY_Y (paths, next_path);

    if (code) {
      next_tree = make_tree (next_path);
      size += tree2size [next_tree] /* tree2size [0] == 0 */ + ((code2trans (code) == ATOMIC_TRANS) ? 1 : code2size [code2id (code)]);
    
      if (XxY_set (&branches, code, next_tree, &next_branch))
	branch2occur_nb [next_branch]++;
      else
	branch2occur_nb [next_branch] = 1;

      DPUSH (code_stack, next_branch);
    
      XxY_set (&pathXbranch, path, next_branch, &path_branch);
    }
    else
      is_leaf = TRUE;
  }

  if (size) {
    for (top = DTOP (code_stack); top > bot; top--) {
      next_branch = DPOP (code_stack);
      SXBA_1_bit (branches_set, next_branch);
    }

    /* Il faut tous les mettre ds le meme ordre */
    if (is_leaf)
      XH_push (trees, 0); /* branche qui indique que la racine de l'arbre est (aussi) une feuille */
	
    next_branch = 0;

    while ((next_branch = sxba_scan_reset (branches_set, next_branch)) > 0) {
      XH_push (trees, next_branch);
    }

    if (!XH_set (&trees, &tree)) {
      tree2size [tree] = size;
      tree2occur_nb [tree] = 1;
    }
    else
      tree2occur_nb [tree]++;
  }
  else {
    tree = 0;
    tree2size [0] = 0;
  }

  path2tree [path] = tree;
  XxY_set (&pathXtree, path, tree, &path_tree);

  return tree;
}

/* ESSAI */
static void
erase_paths (SXBA cur_paths_set)
{
  SXINT path;

  path = XxY_top (paths)+1;

  while ((path = sxba_0_rlscan (cur_paths_set, path)) >= 0) {
    XxY_erase (paths, path);
    path2lgth [path] = 0; /* prudence */
  }
}

/* On a supprime' des maillons ds paths, On note tout ce qui reste accessible */
static BOOLEAN
valid_paths (SXINT path)
{
  SXINT   next_path;
  BOOLEAN ret_val = FALSE;

  XxY_Xforeach (paths, path, next_path) {
    if (SXBA_bit_is_reset_set (paths_set, next_path)) {
      valid_paths (next_path);
      ret_val = TRUE;
    }
  }

  return ret_val;
}

static BOOLEAN
remake_structures (void)
{
  sxba_empty (paths_set);

  if (!valid_paths (0))
    /* Tout a disparu */
    return FALSE;

  /* paths_set est l'ensemble des paths accessibles */
  erase_paths (paths_set);

  XxY_clear (&branches);
  XxY_clear (&pathXbranch);
  XxY_clear (&pathXtree);
  XH_clear (&trees);

  make_tree (0 /* racine */);

  return TRUE;
}

/* Supprime le maillon strategique */
static void
clear_path (SXINT path)
{
  SXINT prev_path, next_path;

  prev_path = XxY_X (paths, path);

  if (prev_path == 0) {
    XxY_erase (paths, path);
    path2lgth [path] = 0; /* prudence */
  }
  else {
    XxY_Xforeach (paths, prev_path, next_path) {
      if (next_path != path) {
	/* embranchement => point strategique */
	XxY_erase (paths, path);
	path2lgth [path] = 0; /* prudence */
	break;
      }
    }

    if (next_path == 0)
      /* Un seul descendant, on va voir + haut */
      clear_path (prev_path);
  }
}

/* Supprime "tree" des structures et les met a jour
   Retourne TRUE ssi elles sont non vides */
static BOOLEAN
clear_tree (SXINT tree)
{
  SXINT   path_tree, path;
  BOOLEAN ret_val;

  ret_val = TRUE;

  if (tree) {
    XxY_Yforeach (pathXtree, tree, path_tree) {
      path = XxY_X (pathXtree, path_tree);

      if (path)
	clear_path (path);
    }

    ret_val = remake_structures ();
  }

  return ret_val;
}


/* Supprime "branch" des structures et les met a jour
   Retourne TRUE ssi elles sont non vides */
static BOOLEAN
clear_branch (SXINT branch)
{
  SXINT   path_branch, prev_path, next_path, code;
  BOOLEAN ret_val;

  ret_val = TRUE;

  if (branch) {
    XxY_Yforeach (pathXbranch, branch, path_branch) {
      prev_path = XxY_X (pathXbranch, path_branch);
      code = XxY_X (branches, branch);
      next_path = XxY_is_set (&paths, prev_path, code);

      /* On coupe la branche ... */
      /* ... sur le maillon strategique */
      clear_path (next_path);
    }

    ret_val = remake_structures ();
  }

  return ret_val;
}



/* Realise la meilleure factorisation, la supprime des structures et retourne TRUE
   sinon (structure vide ou plus de factorisation) retourne FALSE
*/
static void
extract_factor (void)
{
  SXINT top, best_tree, best_tree_gain, tree, nb, gain, path_tree, path, best_branch, best_branch_gain, branch;
  SXINT code, code_size, path_branch, next_path, bot_tree, bot_branch;

  /* On cherche la meilleure factorisation
     Elle peut impliquer soit un tree soit une branch
  */

  /* On regarde les tree */
  top = XH_top (trees);

  best_tree = 0;
  best_tree_gain = 0;

  for (tree = top-1; tree >= 0; tree--) {
    if ((nb = tree2occur_nb [tree]) > 1) {
      /* tree est partage' entre plusieurs path */
      /* gain de la factorisation de tree */
      gain = (nb-1) * (tree2size [tree]);
      /* Les prefixes vont peut etre devoir etre dupliques */

      sxba_empty (paths_set);

      XxY_Yforeach (pathXtree, tree, path_tree) {
	path = XxY_X (pathXtree, path_tree);

	if (path && SXBA_bit_is_reset_set (paths_set, path))
	  PUSH (paths_stack, path);
      }

      expand_paths ();

      XxY_Yforeach (pathXtree, tree, path_tree) {
	path = XxY_X (pathXtree, path_tree);

	if (path)
	  /* On ne compte pas le dernier maillon du chemin path */
	  gain -= path2cost (XxY_X (paths, path));
      }

      if (gain > best_tree_gain) {
	best_tree = tree;
	best_tree_gain = gain;
      }
    }
  }

  /* ... et les branches */
  top = XxY_top (branches);

  best_branch = 0;
  best_branch_gain = 0;

  for (branch = top; branch >= 0; branch--) {
    if ((nb = branch2occur_nb [branch]) > 1) {
      /* branch est partage' entre plusieurs path */
      /* gain de la factorisation de branch */
      code = XxY_X (branches, branch);
      tree = XxY_Y (branches, branch);

      /* Si on factorize, on va gagner ... */
      if (code2trans (code) == ATOMIC_TRANS)
	code_size = 1;
      else
	code_size = code2size [code2id (code)];

      gain = (nb-1) * (code_size+tree2size [tree]);

      /* Les prefixes vont peut etre devoir etre dupliques */
      sxba_empty (paths_set);

      XxY_Yforeach (pathXbranch, branch, path_branch) {
	path = XxY_X (pathXbranch, path_branch);
	next_path = XxY_is_set (&paths, path, code);

	if (SXBA_bit_is_reset_set (paths_set, next_path))
	  PUSH (paths_stack, next_path);
      }

      expand_paths ();

      XxY_Yforeach (pathXbranch, branch, path_branch) {
	path = XxY_X (pathXbranch, path_branch);
	
	if (path)
	  gain -= path2cost (path);
      }

      if (gain > best_branch_gain) {
	best_branch = branch;
	best_branch_gain = gain;
      }
    }
  }

  if (best_tree || best_branch) {
    sxba_empty (paths_set);
    sxba_empty (final_paths_set);

    if (best_tree_gain >= best_branch_gain) {
      /* factorisation d'arbres */
      XxY_Yforeach (pathXtree, best_tree, path_tree) {
	path = XxY_X (pathXtree, path_tree);

	if (path && SXBA_bit_is_reset_set (paths_set, path)) {
	  SXBA_1_bit (final_paths_set, path);
	  PUSH (paths_stack, path);
	}
      }

      expand_paths ();

      bot_tree = factorize_tree (best_tree);

      /* On en fait un identifiant unique */
      if (DTOP (code_stack) > bot_tree+1) {
	/* Vraie cat */
	while (DTOP (code_stack) > bot_tree) {
	  code = DPOP (code_stack);
	  XH_push (code_hd, code);
	}

	set_code (&code);
	code |= CAT_CODE;
	DPUSH (code_stack, code);
      }

      /* On nettoie et on met a jour toutes les structures impactees par la generation precedente */
      if (clear_tree (best_tree))
	extract_factor ();

      return;
    }

    /* factorisation de branches */
    /* On va faire cette factorisation */
    XxY_Yforeach (pathXbranch, best_branch, path_branch) {
      path = XxY_X (pathXbranch, path_branch);

      if (path && SXBA_bit_is_reset_set (paths_set, path)) {
	SXBA_1_bit (final_paths_set, path);
	PUSH (paths_stack, path);
      }
    }

    expand_paths ();

    bot_branch = factorize_branch (best_branch);
      
    /* On en fait un identifiant unique */
    if (DTOP (code_stack) > bot_branch+1) {
      /* Vraie cat */
      while (DTOP (code_stack) > bot_branch) {
	code = DPOP (code_stack);
	XH_push (code_hd, code);
      }

      set_code (&code);
      code |= CAT_CODE;
      DPUSH (code_stack, code);
    }

    /* On nettoie et on met a jour toutes les structures impactees par la generation precedente */
    if (clear_branch (best_branch))
      extract_factor ();

    return;
  }

  /* On a atteint la racine avec une structure non vide et non factorisable (sauf peut-etre a gauche) */
  bot_tree = gen_tree (path2tree [0], 0, NULL, NULL);
	
  /* On en fait un identifiant unique */
  if (DTOP (code_stack) > bot_tree+1) {
    /* Vraie cat */
    while (DTOP (code_stack) > bot_tree) {
      code = DPOP (code_stack);
      XH_push (code_hd, code);
    }

    set_code (&code);
    code |= CAT_CODE;
    DPUSH (code_stack, code);
  }
}


static SXINT
factorize_OR (SXINT or_code)
{
  SXINT id, bot, cur, top, cur_code, cur_trans_kind, path, cur_id, bot1, cur1, top1, lgth, code1, code;
  BOOLEAN has_common;

  /* On commence par fabriquer dans paths une factorisation gauche (arbre des prefixes) */
  XxY_clear (&paths);

  id = code2id (or_code);
  bot = XH_X (code_hd, id);
  top = XH_X (code_hd, id+1);
  has_common = FALSE;

  for (cur = bot; cur < top; cur++) {
    cur_code = XH_list_elem (code_hd, cur);
    cur_trans_kind = code2trans (cur_code);

    if (cur_trans_kind == ATOMIC_TRANS || cur_trans_kind == OPTION_TRANS) {
      if (XxY_set (&paths, 0, cur_code, &path))
	has_common = TRUE;

      XxY_set (&paths, path, 0, &path); /* "etat" final */
    }
    else {
      cur_id = code2id (cur_code);
      bot1 = XH_X (code_hd, cur_id);
      top1 = XH_X (code_hd, cur_id+1);
      path = 0;
      lgth = 0;

      for (cur1 = bot1; cur1 < top1; cur1++) {
	code1 = XH_list_elem (code_hd, cur1);
	lgth++;
	
	if (!XxY_set (&paths, path, code1, &path))
	  path2lgth [path] = lgth;
	else
	  has_common = TRUE;
      }
	
      XxY_set (&paths, path, 0, &path); /* "etat" final */
    }
  }

  /* Il y a de la factorisation gauche */
  /* ... on regarde s'il y a aussi de la factorisation droite sur les memes brins ... */
  /* ... que l'on genere ... */
  /* on fait ce qui suit */
  XxY_clear (&branches);
  XxY_clear (&pathXbranch);
  XxY_clear (&pathXtree);
  XH_clear (&trees);

  /* On fabrique maintenant les sous-arbres commun de l'arbre des prefixes */
  /* un arbre est un ensemble (un XH) de branches (une feuille est un arbre vide de code 0)
     une branche est un couple (XxY) (trans, arbre) */
  make_tree (0 /* racine */);

  /* exploiter les tailles pour obtenir la taille min */
  extract_factor ();
  /* Le resultat est dans code_stack */
      
  if (DTOP (code_stack) > 1) {
    /* Un FOR */
    while (!IS_EMPTY (code_stack)) {
      code = DPOP (code_stack);
      XH_push (code_hd, code);
    }
  
    set_code (&code);
    code |= FOR_CODE;
  }
  else {
    code = DPOP (code_stack);
  }
      
  return code;
}


static SXINT
make_code (SXINT code1, SXINT code2, SXINT what_to_do)
{
  SXINT trans1_kind, trans2_kind, code, id1, id2, bot1, cur1, top1, bot2, cur2, top2;


#if EBUG
  if (make_code_level_call == 0)
    varstr_raz (wvstr);

  switch (what_to_do) {
  case ATOMIC_TRANS:
    wvstr = print_code (wvstr, code1);
    break;
  case CAT_TRANS:
    wvstr = varstr_lcatenate (wvstr, "(", 1);
    wvstr = print_code (wvstr, code1);
    wvstr = varstr_lcatenate (wvstr, ") . (", 5);
    wvstr = print_code (wvstr, code2);
    wvstr = varstr_lcatenate (wvstr, ")", 1);
    break;
  case OR_TRANS:
    wvstr = varstr_lcatenate (wvstr, "(", 1);
    wvstr = print_code (wvstr, code1);
    wvstr = varstr_lcatenate (wvstr, ") | (", 5);
    wvstr = print_code (wvstr, code2);
    wvstr = varstr_lcatenate (wvstr, ")", 1);
    break;
  case OPTION_TRANS:
    wvstr = varstr_lcatenate (wvstr, "(", 1);
    wvstr = print_code (wvstr, code1);
    wvstr = varstr_lcatenate (wvstr, ")?", 2);
    break;
  case FOR_TRANS:
    /* make_code ne fait jamais directement de FOR_TRANS */
    sxtrap (ME, "make_code (FOR_TRANS)");
    break;
  default:
    sxtrap (ME, "make_code");
    break;
  }

  wvstr = varstr_lcatenate (wvstr, " => ", 4);
#endif /* EBUG */

  switch (what_to_do) {
  case ATOMIC_TRANS:
    code = code1 | ATOMIC_CODE;

    break;

  case CAT_TRANS:
    trans1_kind = code2trans (code1);
    trans2_kind = code2trans (code2);

    if (trans1_kind == OR_TRANS) {
      /* On le factorise */
      code1 = factorize_OR (code1);
      trans1_kind = code2trans (code1);
    }

    if (trans2_kind == OR_TRANS) {
      /* On le factorise */
      code2 = factorize_OR (code2);
      trans2_kind = code2trans (code2);
    }

    if (trans1_kind == CAT_TRANS) {
      id1 = code2id (code1);
      bot1 = XH_X (code_hd, id1);
      top1 = XH_X (code_hd, id1+1);

      for (cur1 = bot1; cur1 < top1; cur1++) {
	XH_push (code_hd, XH_list_elem (code_hd, cur1));
      }
    }
    else {
      /* trans1_kind == ATOMIC_TRANS || trans1_kind == FOR_TRANS || trans1_kind == OPTION_TRANS */
      XH_push (code_hd, code1);
    }

    if (trans2_kind == CAT_TRANS) {
      id2 = code2id (code2);
      bot2 = XH_X (code_hd, id2);
      top2 = XH_X (code_hd, id2+1);

      for (cur2 = bot2; cur2 < top2; cur2++) {
	XH_push (code_hd, XH_list_elem (code_hd, cur2));
      }
    }
    else {
      /* trans2_kind == ATOMIC_TRANS || trans2_kind == FOR_TRANS || trans2_kind == OPTION_TRANS */
      XH_push (code_hd, code2);
    }

    set_code (&code);
    code |= CAT_CODE;

    break;

  case OR_TRANS:
    if (code1 == code2) {
      /* Au cas ou ... */
      code = code1;
    }
    else {
      trans1_kind = code2trans (code1);
      trans2_kind = code2trans (code2);

#if EBUG
      if (trans1_kind == FOR_TRANS || trans2_kind == FOR_TRANS)
	/* FOR_TRANSes are never OR'ed */
	sxtrap (ME, "make_code (FOR_TRANS)");
#endif /* EBUG */

      /* Ici, code1 et code2 sont issus du meme etat, si l'automate est de'terministe, ils ne commencent donc pas par le meme terminal */
      /* Soit code1 = X T et code2 = Y T, (suffixe de X et Y differents), T pouvant etre vide */
      /* Si T vide => X | Y
	 Si T non vide
	 Si X non vide et Y non vide => (X | Y) T
	 Si X vide et Y non vide     =>  [Y] T
	 Si X non vide et Y vide     =>  [X] T
	 Si X vide et Y vide         => impossible
      */
	
      switch (trans1_kind) {
      case ATOMIC_TRANS:
      case CAT_TRANS:
      case OPTION_TRANS:
	XH_push (code_hd, code1);
	break;

      case OR_TRANS:
	id1 = code2id (code1);
	bot1 = XH_X (code_hd, id1);
	top1 = XH_X (code_hd, id1+1);
	    
	for (cur1 = bot1; cur1 < top1; cur1++) {
	  XH_push (code_hd, XH_list_elem (code_hd, cur1));
	}

	break;

      default :
	sxtrap (ME, "make_code");
	break;
      }
	
      switch (trans2_kind) {
      case ATOMIC_TRANS:
      case CAT_TRANS:
      case OPTION_TRANS:
	XH_push (code_hd, code2);
	break;

      case OR_TRANS:
	id2 = code2id (code2);
	bot2 = XH_X (code_hd, id2);
	top2 = XH_X (code_hd, id2+1);
	    
	for (cur2 = bot2; cur2 < top2; cur2++) {
	  XH_push (code_hd, XH_list_elem (code_hd, cur2));
	}

	break;

      default :
	sxtrap (ME, "make_code");
	break;
      }

      set_code (&code);
      code |= OR_CODE;
    }

    break;

  case OPTION_TRANS:
    trans1_kind = code2trans (code1);

    if (trans1_kind == OPTION_TRANS)
      return code1;

    if (trans1_kind == OR_TRANS)
      code1 = factorize_OR (code1);

    XH_push (code_hd, code1);
    set_code (&code);
    
    code |= OPTION_CODE;

    break;

  case FOR_TRANS:
    /* make_code ne fait jamais directement de FOR_TRANS */
    sxtrap (ME, "make_code (FOR_TRANS)");

    break;

  default :
    sxtrap (ME, "make_code");
    break;
  }

#if EBUG
  wvstr = print_code (wvstr, code);

  if (make_code_level_call == 0)
    printf ("%s\n", varstr_tostr (wvstr));

  trans1_kind = code2trans (code);

  if (trans1_kind != ATOMIC_TRANS) {
    id1 = code2id (code);
    bot1 = XH_X (code_hd, id1);
    top1 = XH_X (code_hd, id1+1);

    if (trans1_kind == OPTION_TRANS) {
      /* Si unique ca ne doit pas etre une OPTION */
      if (bot1+1 == top1 && code2trans (XH_list_elem (code_hd, bot1)) == OPTION_TRANS)
	  sxtrap (ME, "make_code");
    }
    else {
      if (trans1_kind == CAT_TRANS) {
	/* On verifie qu'un cat ne contient pas de cat */
	for (cur1 = bot1; cur1 < top1; cur1++) {
	  if (code2trans (XH_list_elem (code_hd, cur1)) == CAT_TRANS)
	    sxtrap (ME, "make_code");
	}
      }
      else {
	/* trans1_kind == OR_TRANS || trans1_kind == FOR_TRANS */
	/* On verifie qu'un or ne contient ni or ni for */
	for (cur1 = bot1; cur1 < top1; cur1++) {
	  trans1_kind = code2trans (XH_list_elem (code_hd, cur1));

	  if (trans1_kind == OR_TRANS || trans1_kind == FOR_TRANS)
	    sxtrap (ME, "make_code");
	}
      }
    }
  }
#endif /* EBUG */

  return code;
}



static void
fill_dag_hd (SXINT p, SXINT t, SXINT q)
{
  SXINT           dag_id;
  BOOLEAN         first_time;
  struct dag_attr *attr_ptr;

  first_time = !XxY_set (&dag_hd, p, q, &dag_id);

  attr_ptr = dag_id2attr+dag_id;

  if (first_time) {
    dag_hd_trans_nb++;
    attr_ptr->trans = ATOMIC_CODE | t;
  }
  else {
    /* On fait un or */
    attr_ptr->trans = make_code (attr_ptr->trans, ATOMIC_CODE | t, OR_TRANS);
  }
}


/*
    ab|b      => [a]b
    [a]b|b    => [a]b
    a[b]|[b]  => [a][b]
    ab|[b]    => [[a]b]
    a[b]|b    => rien

    ab|a      => a[b]
    a[b]|a    => a[b]
    [a]b|[a]  => [a][b]
    ab|[a]    => [a[b]]
    [a]b|a    => rien
*/

/* On forme, a partir de la concatenation de dag1_id et de dag2_id une transition unique
   qui vient s'inserer ds dag_hd */
static SXINT
make_a_new_trans (SXINT dag1_id, SXINT dag2_id)
{
  SXINT           p, q, dag_id, code, code1, code2;
  struct dag_attr *attr_ptr;
  BOOLEAN         first_time;

#if EBUG
  if (XxY_Y (dag_hd, dag1_id) != XxY_X (dag_hd, dag2_id))
    sxtrap (ME, "make_a_new_trans");
#endif /* EBUG */
  
  p = XxY_X (dag_hd, dag1_id);
  q = XxY_Y (dag_hd, dag2_id);

  code1 = dag_id2attr [dag1_id].trans;
  code2 = dag_id2attr [dag2_id].trans;

  if ((SXUINT)code2 == (eof_trans | ATOMIC_CODE)) {
    if (code2trans (code1) == OR_TRANS) {
      /* On le factorise */
      code = factorize_OR (code1);
    }
    else
      code = code1;
  }
  else
    code = make_code (code1, code2, CAT_TRANS);

  first_time = !XxY_set (&dag_hd, p, q, &dag_id);
  attr_ptr = dag_id2attr+dag_id;

  if (first_time) {
    dag_hd_trans_nb++;
    attr_ptr->trans = code;
  }
  else {
    /* On fait un or */
    if ((SXUINT)attr_ptr->trans == (eof_trans | ATOMIC_CODE)) {
      attr_ptr->trans = make_code (code, 0, OPTION_TRANS);
    }
    else
      attr_ptr->trans = make_code (attr_ptr->trans, code, OR_TRANS);
  }

  return dag_id;
}


static SXINT
dag_reduce (void)
{
  SXINT return_dag_id = 0, dag_id, trans_id, trans_id2, q;

  for (q = 2; q < final_state; q++) {
    dag_id = 0;

    XxY_Yforeach (dag_hd, q, trans_id) {
      /* p -> q */
      XxY_Xforeach (dag_hd, q, trans_id2) {
	/* q -> r */
	/* p != q != r (car DAG) */
	dag_id = make_a_new_trans (trans_id, trans_id2);
      }
    }

    if (dag_id) {
      return_dag_id = dag_id;

      XxY_Yforeach (dag_hd, q, trans_id) {
	XxY_erase (dag_hd, trans_id);
	dag_hd_trans_nb--;
      }
      
      XxY_Xforeach (dag_hd, q, trans_id2) {
	XxY_erase (dag_hd, trans_id2);
	dag_hd_trans_nb--;
      }
    }
  }

  return return_dag_id;
}


static void
code_hd_oflw (SXINT old_size, SXINT new_size)
{
  sxuse (old_size);
  code2size = (SXINT*) sxrealloc (code2size, new_size+1, sizeof (SXINT));
}


static void
dag_hd_oflw (SXINT old_size, SXINT new_size)
{
  sxuse (old_size);
  dag_id2attr = (struct dag_attr*) sxrealloc (dag_id2attr, new_size+1, sizeof (struct dag_attr));
}


VARSTR
dag2re (VARSTR varstr_ptr, 
	SXINT dag_init_state, 
	SXINT dag_final_state, 
	SXINT eof_ste, 
	void (*dag_extract_trans) (SXINT, void (*fill_dag_hd) (SXINT p, SXINT t, SXINT q)), 
	char *(*get_trans_name) (SXINT))
{
  SXINT p, single_trans, code;

  eof_trans = (SXUINT)eof_ste;
  dag_get_trans_name = get_trans_name;
  final_state = dag_final_state;

  XH_alloc (&code_hd, "code_hd", dfa_final_state+1, 1, dfa_final_state/2 + 1, code_hd_oflw, NULL);
  code2size = (SXINT*) sxalloc (XH_size (code_hd)+1, sizeof (SXINT));

#if EBUG
  wvstr = varstr_alloc (256);
#endif /* EBUG */
  XxY_alloc (&dag_hd, "dag_hd", dag_final_state+1, 1, 1 /* Xforeach */, 1 /* Yforeach */, dag_hd_oflw, NULL);
  dag_id2attr = (struct dag_attr*) sxalloc (XxY_size (dag_hd)+1, sizeof (struct dag_attr));

  dag_hd_trans_nb = 0;

  for (p = dag_init_state; p < dag_final_state; p++) {
    (*dag_extract_trans) (p, fill_dag_hd);
  }

  /* Nelle factorisation des OR */
  XxY_alloc (&paths, "paths", 30, 1, 1 /* Xforeach */, 0, paths_oflw, NULL);
  path2lgth = (SXINT *) sxalloc (XxY_size (paths)+1, sizeof (SXINT));
  path2tree = (SXINT *) sxalloc (XxY_size (paths)+1, sizeof (SXINT));
  paths_set = sxba_calloc (XxY_size (paths)+1);
  final_paths_set = sxba_calloc (XxY_size (paths)+1);
  paths_stack = (SXINT *) sxalloc (XxY_size (paths)+1, sizeof (SXINT)), paths_stack [0] = 0;
  XxY_alloc (&branches, "branches", 30, 1, 0, 0, branches_oflw, NULL);
  branch2occur_nb = (SXINT *) sxalloc (XxY_size (branches)+1, sizeof (SXINT));
  branches_set = sxba_calloc (XxY_size (branches)+1);
  XxY_alloc (&pathXbranch, "pathXbranch", 30, 1, 0, 1 /* Yforeach */, NULL, NULL);
  XxY_alloc (&pathXtree, "pathXtree", 30, 1, 0, 1 /* Yforeach */, NULL, NULL);
  XH_alloc (&trees, "trees", 30, 1, 4, trees_oflw, NULL);
  tree2size = (SXINT *) sxalloc (XH_size (trees)+1, sizeof (SXINT));
  tree2occur_nb = (SXINT *) sxalloc (XH_size (trees)+1, sizeof (SXINT));
  DALLOC_STACK (code_stack, 100);

  single_trans = 1; /* Si trans unique ds le DAG */

  while (dag_hd_trans_nb > 1) {
    single_trans = dag_reduce ();
  }

  code = dag_id2attr [single_trans].trans;

  if (code2trans (code) == OR_TRANS)
    code = factorize_OR (code);
  
  XxY_free (&paths);
  sxfree (paths_set), paths_set = NULL;
  sxfree (final_paths_set), final_paths_set = NULL;
  sxfree (paths_stack), paths_stack = NULL;
  sxfree (path2tree), path2tree = NULL;
  sxfree (path2lgth), path2lgth = NULL;
  XxY_free (&branches);
  sxfree (branch2occur_nb), branch2occur_nb = NULL;
  sxfree (branches_set), branches_set = NULL;
  XxY_free (&pathXbranch);
  XxY_free (&pathXtree);
  XH_free (&trees);
  sxfree (tree2size), tree2size = NULL;
  sxfree (tree2occur_nb), tree2occur_nb = NULL;
  DFREE_STACK (code_stack);

  varstr_ptr = print_code (varstr_ptr, code);

#if EBUG
  varstr_free (wvstr);
#endif /* EBUG */
  XxY_free (&dag_hd);
  sxfree (dag_id2attr), dag_id2attr = NULL;
  XH_free (&code_hd);
  sxfree (code2size), code2size = NULL;

  return varstr_ptr;
}



/* construction "rapide" d'un "dico" qui pourrait etre utilise' (lentement) ds sxspell */

void
word_tree_oflw (struct word_tree_struct *word_tree_ptr, SXINT old_size, SXINT new_size)
{
  if (word_tree_ptr->final_path_set)
    word_tree_ptr->final_path_set = sxba_resize (word_tree_ptr->final_path_set, new_size+1);
  else
    word_tree_ptr->path2id = (SXINT *) sxrecalloc (word_tree_ptr->path2id, old_size+1, new_size+1, sizeof (SXINT));
}


/* Si to_be_mininized est TRUE, dico2 subira ulterieurement une phase de minimisation
   pour ce faire on a besoin d'une structure supplementaire (dico2_lgth2top_path_hd et dico2_lgth2path) */
void
word_tree_alloc (struct word_tree_struct *word_tree_ptr, 
		 char *name, 
		 SXINT word_nb, 
		 SXINT word_lgth,
		 SXINT Xforeach, 
		 SXINT Yforeach, 
		 BOOLEAN from_left_to_right, 
		 BOOLEAN with_path2id, 
		 void (*oflw) (SXINT, SXINT), 
		 FILE *stats)
{
  SXINT size;

  word_tree_ptr->name = name;
  word_tree_ptr->stats = stats;
  word_tree_ptr->private.from_left_to_right = from_left_to_right; /* Les mots sont lus de gauche a droite */
  word_tree_ptr->is_static = FALSE;

  size = word_nb*word_lgth;

  XxY_alloc (&word_tree_ptr->paths, word_tree_ptr->name, size+1, 1, Xforeach, Yforeach, oflw, NULL /* Pas word_tree_ptr->stats !! */);
  XxY_set (&word_tree_ptr->paths, 0, -1, &word_tree_ptr->root); /* word_tree_ptr->root == 1 */

  /* Il y a 2 facons de designer les etats finals */
  /* word_tree_ptr->path2id ne peut etre utilise' que si to_be_mininized == FALSE */

  /* Attention, les 2 qui suivent peuvent etre utilises ds une struct word_tree_ptr->struct
     Ils ne doivent donc pas etre liberes sans precaution */
  if (with_path2id) {
    word_tree_ptr->path2id = (SXINT *) sxcalloc (XxY_size (word_tree_ptr->paths)+1, sizeof (SXINT));
    word_tree_ptr->final_path_set = NULL;
  }
  else {
    word_tree_ptr->final_path_set = sxba_calloc (XxY_size (word_tree_ptr->paths)+1);
    word_tree_ptr->path2id = NULL;
  }

  word_tree_ptr->max_final_state_nb = word_tree_ptr->max_path_lgth = word_tree_ptr->max_trans = 0;
}


void
word_tree_free (struct word_tree_struct *word_tree_ptr)
{
  if (word_tree_ptr->stats) {
    fprintf (word_tree_ptr->stats, "// Statistics for \"%s\": total size (in bytes) = %ld\n", word_tree_ptr->name,
	     sizeof (struct word_tree_struct)
	     /* Le XxY */
	     + sizeof (word_tree_ptr->paths.hash_display [0]) * (word_tree_ptr->paths.hash_size+1)
	     + sizeof (word_tree_ptr->paths.hash_lnk [0]) * (word_tree_ptr->paths.top+1)
	     + sizeof (word_tree_ptr->paths.display [0]) * (word_tree_ptr->paths.top+1)
	     /* Pour l'instant, je suppose qu'il y-a pas de foreach !! */
	     + ((word_tree_ptr->path2id) ? sizeof (SXINT) * (word_tree_ptr->max_final_state_nb + 1) : 0)
	     + ((word_tree_ptr->final_path_set) ? (sizeof (SXBA_ELT) * (NBLONGS (BASIZE (word_tree_ptr->final_path_set))+1)) : (sizeof (SXINT) * (word_tree_ptr->max_final_state_nb + 1)))
	     );
  }

  if (!word_tree_ptr->is_static) {
    XxY_free (&word_tree_ptr->paths);

    if (word_tree_ptr->final_path_set) sxfree (word_tree_ptr->final_path_set), word_tree_ptr->final_path_set = NULL;
    if (word_tree_ptr->path2id) sxfree (word_tree_ptr->path2id), word_tree_ptr->path2id = NULL;
  }
}


static SXUINT
string_get_next_symb (struct sxdfa_private *sxdfa_private_ptr)
{
  return sxdfa_private_ptr->from_left_to_right ? (SXUINT)(unsigned char)*sxdfa_private_ptr->ptr.char_ptr++ : (SXUINT)(unsigned char)*sxdfa_private_ptr->ptr.char_ptr--;
}

static SXUINT
word_get_next_symb (struct sxdfa_private *sxdfa_private_ptr)
{
  return sxdfa_private_ptr->from_left_to_right ? (SXUINT)*sxdfa_private_ptr->ptr.SXINT_ptr++ : (SXUINT)*sxdfa_private_ptr->ptr.SXINT_ptr--;
}


/* On ajoute le chemin contenu ds word_stack au DFA acyclique */
/* Un etat f est final ssi il existe depuis f une transition sur -1 */
SXINT
word_tree_add_a_string (struct word_tree_struct *word_tree_ptr, char *string, SXINT string_lgth, SXINT id)
{
  word_tree_ptr->private.ptr.char_ptr = (word_tree_ptr->private.from_left_to_right) ? string : string+string_lgth-1;

  return word_tree_add (word_tree_ptr, string_get_next_symb, string_lgth, id);
}



SXINT
word_tree_add_a_word (struct word_tree_struct *word_tree_ptr, SXINT *input_stack, SXINT id)
{
  word_tree_ptr->private.ptr.SXINT_ptr = (word_tree_ptr->private.from_left_to_right) ? input_stack + 1 : input_stack + *input_stack;

  return word_tree_add (word_tree_ptr, word_get_next_symb, *input_stack, id);
}

SXINT
word_tree_add (struct word_tree_struct *word_tree_ptr, SXUINT (*get_next_symb) (struct sxdfa_private*), SXINT lgth, SXINT id)
{
  SXINT trans, path, new_path, cur_lgth;

  path = word_tree_ptr->root; /* Racine */

  for (cur_lgth = 1; cur_lgth <= lgth; cur_lgth++) {
    trans = (SXINT)(*get_next_symb) (&(word_tree_ptr->private));
    
    if (!XxY_set (&(word_tree_ptr->paths), path, trans, &new_path)) {
      /* new_path est nouveau */
      if (trans > word_tree_ptr->max_trans)
	word_tree_ptr->max_trans = trans;
    }

    path = new_path;
  }

  if (lgth > word_tree_ptr->max_path_lgth)
    word_tree_ptr->max_path_lgth = lgth;

  /* marque d'un etat final */
  if (word_tree_ptr->final_path_set) {
    if (SXBA_bit_is_reset_set (word_tree_ptr->final_path_set, path))
      word_tree_ptr->max_final_state_nb++;
  }
  else {
    if (word_tree_ptr->path2id) {
      if (word_tree_ptr->path2id [path] == 0) {
	word_tree_ptr->max_final_state_nb++;
	word_tree_ptr->path2id [path] = id;
      }
    }
  }

  return path;
}


/* Recherche kw ds le fsa repre'sente' sous forme d'un sxdfa_struct et retourne l'etat final ou 0 si kw n'est pas reconnu */
/* Si echec, kwl indique la partie suffixe non reconnue. */
SXINT
word_tree_seek_a_string (struct word_tree_struct *word_tree_ptr, char *kw, SXINT *kwl)
{
  SXINT		lgth, trans, path, id;

  lgth = *kwl;

  if (lgth < 0)
    return 0;

  path = word_tree_ptr->root; /* Racine */

  while (lgth > 0) {
    trans = (SXINT) *kw++;
    
    if ((path = XxY_is_set (&(word_tree_ptr->paths), path, trans)) == 0) {
      /* Echec */
      *kwl = lgth;
      return 0;
    }

    lgth--;
  }

  /* Ici lgth == 0 et on a atteint path */
  /* C'est OK, uniquement s'il est final */
  *kwl = 0;

  if (word_tree_ptr->final_path_set)
    id = SXBA_bit_is_set (word_tree_ptr->final_path_set, path) ? path : 0;
  else
    id = word_tree_ptr->path2id [path];

  return id;
}


static SXINT      *sxdfa_lgth2path, *sxdfa_path2next;


/* met ds sxdfa_path2next un chemin p de longueur l */
/* 2 appels successifs de sxdfa_lp_push sont tels que path1 < path2
   il faut, pour une longueur donnee les recuperer ds le meme ordre */
static void
sxdfa_lp_push (SXINT path, SXINT lgth)
{
  sxdfa_path2next [path] = sxdfa_lgth2path [lgth];
  sxdfa_lgth2path [lgth] = path;
}


static void
sxdfa_lgth_alloc (SXINT max_path_lgth, SXINT max_path_nb)
{
  sxdfa_lgth2path = (SXINT *) sxcalloc (max_path_lgth+1, sizeof (SXINT));
  sxdfa_path2next = (SXINT*) sxalloc (max_path_nb+1, sizeof (SXINT)), sxdfa_path2next [0] = max_path_lgth;
#if 0
  sxdfa_lp_push (root, 0); /* pour le chemin vide */
#endif /* 0 */
}



static void
sxdfa_lgth_free (void)
{
  sxfree (sxdfa_lgth2path), sxdfa_lgth2path = NULL;
  sxfree (sxdfa_path2next), sxdfa_path2next = NULL;
}

/* Tous les appels a dico2_add_a_word sont termines
   On remplit la sxdfa_struct */
void
word_tree2sxdfa (struct word_tree_struct *word_tree_ptr, struct sxdfa_struct *sxdfa_ptr, char *name, FILE *stats, BOOLEAN to_be_minimized)
{
  SXINT     number_of_trans, max_trans_nb, arity, max_arity, last_path, path, next_path, t, trans_id, new_lgth, max_path_lgth, id;
  SXINT     where, last_new_path, new_path, next_new_path;
  SXBA      cur_t_set, leaf_set, inside_final_set;
  SXINT     *t2next_path, *new_path2lgth, *old_path2new_path;
  XH_header trans_list_hd;

  SXINT     *old_path2id = NULL;

  sxinitialise (new_lgth);

  last_path = XxY_top (word_tree_ptr->paths);
  leaf_set = sxba_calloc (last_path+1);
  inside_final_set = sxba_calloc (last_path+1);
  old_path2new_path = (SXINT *) sxalloc (last_path+1, sizeof (SXINT));

  if (word_tree_ptr->path2id && to_be_minimized) {
    /* On va considerer l'id caracteristique de la reconnaissance d'un mot comme etant une transition particuliere
       que l'on va remonter au + haut (des qu'un chemin a un fils unique) */
    SXINT   selected_path, prev_path;
    SXBA    tbp_path_set;

    old_path2id = (SXINT *) sxcalloc (last_path+1, sizeof (SXINT));
    tbp_path_set = sxba_calloc  (last_path+1);

    path = last_path+1;

    while ((path = sxba_0_rlscan (tbp_path_set, path)) > 0) {
      if ((id = word_tree_ptr->path2id [path])) {
	/* path est final ... */
	XxY_Xforeach (word_tree_ptr->paths, path, next_path) {
	  break;
	}

	if (next_path == 0) {
	  /* ... et c'est une feuille */
	  /* On remonte au +loin pour trouver le premier noeud non unaire */
	  SXBA_1_bit (leaf_set, path);
	  prev_path = path;

	  while ((selected_path = prev_path) != 1) {
	    prev_path = XxY_X (word_tree_ptr->paths, selected_path);
	    SXBA_1_bit (tbp_path_set, prev_path); /* pour ne pas le reexaminer */

	    arity = 0;

	    XxY_Xforeach (word_tree_ptr->paths, prev_path, next_path) {
	      if (++arity > 1)
		break;
	    }

	    if (word_tree_ptr->path2id [prev_path]) {
	      SXBA_1_bit (inside_final_set, prev_path);
	      /* On ne remonte pas au-dela du 1er inside */

	      if (arity == 1)
		selected_path = prev_path;

	      break;
	    }

	    if (arity > 1) {
	      /* arret */
	      break;
	    }
	  }

	  if (selected_path != path) {
	    old_path2id [selected_path] = id;
	    word_tree_ptr->path2id [path] = -id; /* deja utilisee */
	  }
	}
	else
	  SXBA_1_bit (inside_final_set, path);
      }
    }

    sxfree (tbp_path_set);

    new_path = 0;

    for (path = 1; path <= last_path; path++) {
      if (SXBA_bit_is_set (leaf_set, path)) {
	old_path2new_path [path] = 0;
      }
      else {
	old_path2new_path [path] = ++new_path;
      }
    }

    /* Ici Si word_tree_ptr->path2id [path] == -id alors il existe ds le chemin path un prefixe strict selected_path t.q. old_path2id [selected_path] = id
       (Le id doit etre remonte' de path a selected_path) */
  }
  else {
    /* On calcule leaf_set */
    if (word_tree_ptr->path2id) {
      for (path = 1; path <= last_path; path++) {
	if (word_tree_ptr->path2id [path]) {
	  /* path est final */
	  XxY_Xforeach (word_tree_ptr->paths, path, next_path) {
	    break;
	  }
      
	  if (next_path == 0)
	    /* ... et c'est une feuille */
	    SXBA_1_bit (leaf_set, path);
	  else
	    SXBA_1_bit (inside_final_set, path);
	}
      }
    }
    else {
      path = 0;

      while ((path = sxba_scan (word_tree_ptr->final_path_set, path)) > 0) {
	/* path est final */
	XxY_Xforeach (word_tree_ptr->paths, path, next_path) {
	  break;
	}
      
	if (next_path == 0)
	  /* ... et c'est une feuille */
	  SXBA_1_bit (leaf_set, path);
	else
	  SXBA_1_bit (inside_final_set, path);
      }
    }

    /* ... et old_path2new_path */
    new_path = 0;

    for (path = 1; path <= last_path; path++) {
      if (SXBA_bit_is_set (leaf_set, path)) {
	old_path2new_path [path] = 0;
      }
      else {
	old_path2new_path [path] = ++new_path;
      }
    }
  }

  last_new_path = new_path;

  sxfree (leaf_set);

  cur_t_set = sxba_calloc (word_tree_ptr->max_trans+1);
  t2next_path = (SXINT*) sxalloc (word_tree_ptr->max_trans+1, sizeof (SXINT));
  
  max_path_lgth = word_tree_ptr->max_path_lgth;

  if (word_tree_ptr->path2id) max_path_lgth++;

  if (to_be_minimized) {
    new_path2lgth = (SXINT*) sxalloc (last_new_path+1, sizeof (SXINT)), new_path2lgth [0] = new_path2lgth [1] = 0;

    sxdfa_lgth_alloc (max_path_lgth, last_new_path);
  }
  else {
    new_path2lgth = NULL;
    sxdfa_lgth2path = NULL;
  }

  max_trans_nb = last_path/word_tree_ptr->max_final_state_nb + 1;

  /* Truc pour le cas ou *sxdfa_ptr est non vide (ca peut etre du garbage si structure declaree en automatique!!) */
  sxdfa_ptr->name = NULL;
  sxdfa_ptr->stats = NULL;

  sxdfa_alloc (sxdfa_ptr, last_new_path, last_new_path*max_trans_nb, name, stats);
  XH_alloc (&trans_list_hd, "trans_list_hd", last_new_path+1, 1, max_trans_nb, NULL, NULL);

  max_arity = number_of_trans = 0;

  for (path = 1; path <= last_path; path++) {
    new_path = old_path2new_path [path];

    if (new_path) {
      where = (DTOP (sxdfa_ptr->next_state_list)+1)<<1;

      if (SXBA_bit_is_set (inside_final_set, path))
	DPUSH (sxdfa_ptr->states, where|1);
      else
	DPUSH (sxdfa_ptr->states, where);

      if (new_path2lgth) {
	new_lgth = new_path2lgth [new_path];
	sxdfa_lp_push (new_path, new_lgth++);
      }
      
      if (word_tree_ptr->path2id) {
	if (to_be_minimized) {
	  if ((id = word_tree_ptr->path2id [path])) {
	    /* path est final */
	    if (id > 0) {
	      /* on fait une transition sur -id (sans successeur) */
	      DPUSH (sxdfa_ptr->next_state_list, -id);
	      sxdfa_ptr->states [DTOP (sxdfa_ptr->states)] |= 1; /* Il est final */
	    }
	    /* Sinon id a deja ete traite' */
	  }

	  if ((id = old_path2id [path])) {
	    /* Transition remontee */
	    DPUSH (sxdfa_ptr->next_state_list, -id);
	    /* ... et on continue en sequence pour traiter les t transitions depuis path */
	  }
	}
	else {
	  if ((id = word_tree_ptr->path2id [path])) {
	    /* path est final */
	    /* on fait une transition sur -id (sans successeur) */
	    DPUSH (sxdfa_ptr->next_state_list, -id);
	    sxdfa_ptr->states [DTOP (sxdfa_ptr->states)] |= 1; /* Il est final */
	    /* ... et on continue en sequence pour traiter les t transitions depuis path ... */
	    /* ... Si path est inside final */
	  }
	}
      }

      arity = 0;

      XxY_Xforeach (word_tree_ptr->paths, path, next_path) {
	arity++;
	t = XxY_Y (word_tree_ptr->paths, next_path);
	number_of_trans++;

	SXBA_1_bit (cur_t_set, t);

	if (word_tree_ptr->path2id && (id = word_tree_ptr->path2id [next_path]) > 0 && !SXBA_bit_is_set (inside_final_set, next_path)) {
	  /* path est final non interne avec id */
	  next_new_path = -id;
	}
	else {
	  next_new_path = old_path2new_path [next_path]; /* Ca peut etre 0 (Etat final commun des feuilles) */
	}

	t2next_path [t] = next_new_path;

	if (new_path2lgth && (next_new_path > 0)) new_path2lgth [next_new_path] = new_lgth;
      }

      if (arity) {
	if (arity > max_arity)
	  max_arity = arity;

	DPUSH (sxdfa_ptr->next_state_list, 0); /* bidon, place du next_trans */
	where = DTOP (sxdfa_ptr->next_state_list);
	t = -1;

	while ((t = sxba_scan_reset (cur_t_set, t)) >= 0) {
	  XH_push (trans_list_hd, t);
	  DPUSH (sxdfa_ptr->next_state_list, t2next_path [t]);
	}

	XH_set (&trans_list_hd, &trans_id);

	sxdfa_ptr->next_state_list [where] = XH_X (trans_list_hd, trans_id);
      }
    }
  }

  DPUSH (sxdfa_ptr->next_state_list, 0); /* place du next_trans */
  DPUSH (sxdfa_ptr->states, (DTOP (sxdfa_ptr->next_state_list))<<1);

  /* Les feuilles ont des transitions vides */

  /* ATTENTION truc */
  sxdfa_ptr->trans_list = trans_list_hd.list;
  sxdfa_ptr->trans_list[0]--; // l'offset 0 du .list est (nbre d'elt)+1
  trans_list_hd.list = (SXINT *) sxalloc (1, sizeof (SXINT));
  XH_free (&trans_list_hd);

  sxdfa_ptr->init_state = 1;
  sxdfa_ptr->last_state = sxdfa_ptr->states [0] - 1;
  sxdfa_ptr->number_of_out_trans = sxdfa_ptr->next_state_list [0] - 1;
  sxdfa_ptr->final_state_nb = word_tree_ptr->max_final_state_nb;
  sxdfa_ptr->max_path_lgth = max_path_lgth;
  sxdfa_ptr->max_arity = max_arity;
  sxdfa_ptr->max_t_val = word_tree_ptr->max_trans;

  sxdfa_ptr->is_static = FALSE;
  sxdfa_ptr->is_a_dag = TRUE;
  sxdfa_ptr->private.from_left_to_right = word_tree_ptr->private.from_left_to_right;

  sxfree (cur_t_set);
  sxfree (t2next_path);

  if (new_path2lgth) sxfree (new_path2lgth);

  word_tree_free (word_tree_ptr);

  sxfree (old_path2new_path);
  sxfree (inside_final_set);

  if (old_path2id) {
    sxfree (old_path2id);
  }

  if (to_be_minimized) {
    sxdfa_tree2min_dag (sxdfa_ptr);
    sxdfa_lgth_free ();
  }
}


void
sxdfa_alloc (struct sxdfa_struct *sxdfa_ptr, SXINT state_nb, SXINT trans_nb, char *name, FILE *stats)
{
  /* Ces tailles sont des majorants, on ne remplit donc pas les valeurs scalaires definitives */
  sxdfa_ptr->is_static = FALSE;

  if (name) sxdfa_ptr->name = name;
  if (stats) sxdfa_ptr->stats = stats;

  /* On en fait des dstack au cas ou */
  DALLOC_STACK (sxdfa_ptr->states, state_nb+1);
  DALLOC_STACK (sxdfa_ptr->next_state_list, state_nb+trans_nb+1);
  /* sxdfa_ptr->trans_list sera alloue' par ailleurs !! */
}


void
sxdfa_free (struct sxdfa_struct *sxdfa_ptr)
{
  if (sxdfa_ptr->stats) {
    fprintf (sxdfa_ptr->stats, "// Statistics for \"%s\": total size (in bytes) = %ld\n", sxdfa_ptr->name,
	     sizeof (struct sxdfa_struct)
	     + sizeof (SXINT) * (sxdfa_ptr->states [0] + 1)
	     + sizeof (SXINT) * (sxdfa_ptr->next_state_list [0] + 1)
	     + sizeof (SXINT) * (sxdfa_ptr->trans_list [0] + 1)
	     );
  }

  if (!sxdfa_ptr->is_static) {
    DFREE_STACK (sxdfa_ptr->states);
    DFREE_STACK (sxdfa_ptr->next_state_list);
  }
}


/* On a un arbre en entree stocke ds io_dag sous la forme d'une struct sxdfa_struct
   Cet arbre a ete construit a` partir de la liste de ses chemins (mots) (par dico2_add_a_word () puis word_tree2sxdfa ())
   Le Dag minimal resultant est stocke' ds la meme structure.
*/
/* On fabrique pour chaque sous-arbre de cet arbre un identifiant unique.
   Cet identifiant (c'est un XH) est une liste ordonnee de branches
   une branche est un couple (trans, tree) ou tree est l'identifiant du sous-arbre qui est atteint par transition sur trans
*/
void
sxdfa_tree2min_dag (struct sxdfa_struct *sxdfa_ptr)
{
  SXINT      lgth, path, next_path, branch, tree, state, bot, top, next_tree, dico2_max_tree, is_final, bot_trans, where;
  SXINT      *state_ptr, *next_states_ptr, *top_next_state_ptr;
  SXINT      *cur_path2tree;
  XH_header  dico2_trees;
   
  if (sxdfa_lgth2path == NULL)
    sxtrap (ME, "sxdfa_dag_minimize");

  cur_path2tree = (SXINT*) sxalloc (sxdfa_ptr->last_state+1, sizeof (SXINT));

  XH_alloc (&dico2_trees, "dico2_trees", sxdfa_ptr->last_state+1, 1, sxdfa_ptr->number_of_out_trans/sxdfa_ptr->last_state + 1, NULL, NULL);

  /* On traite les path par longueur decroissante.  Comme ca on est sur que si path est un composant
     de next_path, les tree associes aux successeurs de path sont tous calcules */
  
  /* On suppose que l'automate d'entree a ete construit incrementalement par dico2_add_a_word () */
  for (lgth = sxdfa_path2next [0]; lgth >= 0; lgth--) {
    for (path = sxdfa_lgth2path [lgth]; path > 0; path = sxdfa_path2next [path]) {
      /* Obligatoirement non encore calcule' */
      state_ptr = sxdfa_ptr->states + path;
      bot = state_ptr [0];

      is_final = bot & 1;
      bot >>= 1;
      
      next_states_ptr = sxdfa_ptr->next_state_list + bot;
      top_next_state_ptr = sxdfa_ptr->next_state_list + (state_ptr [1]>>1);
      bot_trans = *next_states_ptr++;

      if (is_final) {
	/* Un prefixe du mot est dans le dico */
	/* path est final */
	XH_push (dico2_trees, 0); /* branche "vide" */
      }

      if (bot_trans < 0) {
	if (is_final) {
	  /* L'etat state est final et -bot_trans est l'identifiant associe' ... */
	  /* bot_trans est la branche ... */
	  /* ... que l'on stocke ... */
	  XH_push (dico2_trees, bot_trans);

	  bot_trans = *next_states_ptr++;

	  if (bot_trans < 0) {
	    /* ici -bot_trans est l'identifiant associe' au mot unique qui sera reconnu sur
	       le futur etat final */
	    /* ... que l'on stocke ... */
	    /* comme une branche */
	    XH_push (dico2_trees, bot_trans);
	    bot_trans = *next_states_ptr++; /* le trans_id */
	  }
	}
	else {
	  /* ici -bot_trans est l'identifiant associe' au mot unique qui sera reconnu sur
	     le futur etat final */
	  /* ... que l'on stocke ... */
	  /* comme une branche */
	  XH_push (dico2_trees, bot_trans);
	  bot_trans = *next_states_ptr++; /* le trans_id */
	}
      }

      /* Ici bot_trans est un trans_id */
      XH_push (dico2_trees, bot_trans);
      
      while (next_states_ptr < top_next_state_ptr) {
	next_path = *next_states_ptr++;
	/* On fabrique la branche ... */
	branch = (next_path > 0) ? cur_path2tree [next_path] : next_path;
	/* ... que l'on stocke ... */
	XH_push (dico2_trees, branch);
      }

      /* ... pour fabriquer tree */
      XH_set (&(dico2_trees), &tree);

      cur_path2tree [path] = tree;
    }
  }

  /* Les id  des trees ont des valeurs croissantes des feuilles vers la racine, on ne les utilise donc pas directement
     pour faire les etats de l'automate */
  /* On fait donc cette passe supple'mentaire */
  dico2_max_tree = XH_top (dico2_trees);

  sxdfa_free (sxdfa_ptr);

  /* Ne touche pas a trans_list qui est reutilise' entre "i' et "o" */

  sxdfa_alloc (sxdfa_ptr, dico2_max_tree, XH_list_top (dico2_trees), NULL, NULL);

  for (tree = dico2_max_tree-1; tree > 0; tree--) {
    /* Non vide */
    state = dico2_max_tree-tree;

    bot = XH_X (dico2_trees, tree);
    top = XH_X (dico2_trees, tree+1);

    where = (TOP (sxdfa_ptr->next_state_list)+1)<<1;
    branch = XH_list_elem (dico2_trees, bot++);

    if (branch == 0) {
      /* tree est un etat final */
      PUSH (sxdfa_ptr->states, where|1);
      branch = XH_list_elem (dico2_trees, bot++);
    }
    else
      PUSH (sxdfa_ptr->states, where);

    if (branch < 0) {
      PUSH (sxdfa_ptr->next_state_list, branch);
      branch = XH_list_elem (dico2_trees, bot++);

      if (branch < 0) {
	PUSH (sxdfa_ptr->next_state_list, branch);
	branch = XH_list_elem (dico2_trees, bot++);
      }
    }

    /* ici branch est un trans_id */
    PUSH (sxdfa_ptr->next_state_list, branch);

    while (bot < top) {
      next_tree = XH_list_elem (dico2_trees, bot++);
      if (next_tree > 0) next_tree = dico2_max_tree-next_tree;
      PUSH (sxdfa_ptr->next_state_list, next_tree);
    }
  }

  PUSH (sxdfa_ptr->next_state_list, 0); /* place du next_trans */
  PUSH (sxdfa_ptr->states, (DTOP (sxdfa_ptr->next_state_list))<<1); /* celui de path */

  /* sxdfa_ptr->init_state conserve sa valeur */
  sxdfa_ptr->last_state = sxdfa_ptr->states [0] - 1;
  sxdfa_ptr->number_of_out_trans = sxdfa_ptr->next_state_list [0] - sxdfa_ptr->last_state - 1;
  /* sxdfa_ptr->final_state_nb conserve sa valeur */
  /* sxdfa_ptr->max_path_lgth conserve sa valeur */
  /* sxdfa_ptr->is_static conserve sa valeur (FALSE) */
  /* sxdfa_ptr->is_a_dag conserve sa valeur */

  sxfree (cur_path2tree), cur_path2tree = NULL;

  XH_free (&dico2_trees);
  /* dico2_free () est appele' par l'appelant (celui qui a appele' dico2_alloc ()) */
}





/* Cette fonction est appelee pour chaque transition nfa_state ->t next_nfa_state  ds nfa2sxdfa ()*/
static void
sxnfa_fill_trans (SXINT nfa_state, SXINT t, SXINT next_nfa_state)
{
  sxuse (nfa_state);
  SXBA_1_bit (t2nfa_state_set [t], next_nfa_state);
  SXBA_1_bit (t_set, t);
}


void
nfa2sxdfa (SXINT init_state, 
	   SXINT cur_final_state, 
	   SXINT eof_ste, 
	   SXINT trans_nb, 
	   BOOLEAN (*empty_trans)(SXINT, SXBA), 
	   void (*nfa_extract_trans)(SXINT, void (*sxnfa_fill_trans) (SXINT nfa_state, SXINT t, SXINT next_nfa_state)), 
	   struct sxdfa_struct *sxdfa_ptr, 
	   BOOLEAN to_be_minimized, 
	   BOOLEAN to_be_normalized)
{
  SXINT     dfa_state, new_dfa_state, bot, top, nfa_state, t, max_trans_value, nb, prev_t, t_trans;
  SXBA      cur_state_set;
  BOOLEAN   is_final_state;
  XH_header trans_list_hd;

  sxinitialise (prev_t);

  nfa_init_state = init_state;
  nfa_final_state = cur_final_state;
  nfa_empty_trans = empty_trans; /* Appels multiples */

  dfa_init_state = 1;

  if (nfa_empty_trans) {
    /* Il y a des transitions sur epsilon, on alloue qq structres */
    nfa_ec_stack = (SXINT*) sxalloc (nfa_final_state+1, sizeof (SXINT)), nfa_ec_stack [0] = 0;
    nfa_wstate_set = sxba_calloc (nfa_final_state+1);
  }

  sxdfa_alloc (sxdfa_ptr, cur_final_state, trans_nb, "nfa2sxdfa" /* name */, NULL /* stats */);
  /* a serait bien d'avoir en arg le nb de transitions de l'automate non-deterministe pour calculer
     une taille moyenne */
  XH_alloc (&trans_list_hd, "trans_list_hd", cur_final_state+1, 1, trans_nb/cur_final_state + 1, NULL, NULL);

  DALLOC_STACK (nfa_state_stack, nfa_final_state);
  t_set = sxba_calloc (eof_ste+1);
  t2nfa_state_set = sxbm_calloc (eof_ste+1, nfa_final_state+1);
  XH_alloc (&(XH_dfa_states), "XH_dfa_states", nfa_final_state+1, 1, 10, NULL, NULL);

  /* traitement de l'etat initial */
  cur_state_set = t2nfa_state_set [0];

  SXBA_1_bit (cur_state_set, nfa_init_state);

  if (nfa_empty_trans)
    epsilon_closure (cur_state_set);

  get_state (cur_state_set, &dfa_state);

  sxdfa_ptr->init_state = dfa_state;
  sxdfa_ptr->final_state_nb = sxdfa_ptr->number_of_out_trans = sxdfa_ptr->max_arity = 0;
  max_trans_value = 0;

  DPUSH (nfa_state_stack, dfa_state);

  while (!IS_EMPTY (nfa_state_stack)) {
    dfa_state = DPOP (nfa_state_stack);

    bot = XH_X (XH_dfa_states, dfa_state);
    top = XH_X (XH_dfa_states, dfa_state+1);

    while (bot < top) {
      nfa_state = XH_list_elem (XH_dfa_states, bot);
      (*nfa_extract_trans) (nfa_state, sxnfa_fill_trans);
      bot++;
    }

    t = -1;
    nb = 0;
    is_final_state = FALSE;

    while ((t = sxba_scan (t_set, t)) >= 0) {
      if (t != eof_ste) {
	/* pas trans sur <EOW> */
	nb++;
	XH_push (trans_list_hd, t);
	prev_t = t;
      }
      else {
	is_final_state = TRUE;
	SXBA_0_bit (t_set, t);
	sxdfa_ptr->final_state_nb++; /* C'est la 1ere fois que dfa_state est examine' */
      }
    }

    if (prev_t > max_trans_value)
      max_trans_value = prev_t;

    if (!XH_set (&trans_list_hd, &t_trans)) {
      /* Nelle trans */
      if (nb > sxdfa_ptr->max_arity)
	sxdfa_ptr->max_arity = nb;
    }

    /* On empile t_trans */
    DCHECK (sxdfa_ptr->next_state_list, nb+1);
    DSPUSH (sxdfa_ptr->next_state_list, t_trans);

    /* ... et "dfa_state" */
    if (is_final_state)
      DPUSH (sxdfa_ptr->states, (DTOP (sxdfa_ptr->next_state_list)<<1)+1);
    else
      DPUSH (sxdfa_ptr->states, (DTOP (sxdfa_ptr->next_state_list)<<1));

    t = -1;

    while ((t = sxba_scan_reset (t_set, t)) >= 0) { 
      cur_state_set = t2nfa_state_set [t];

      if (nfa_empty_trans)
	epsilon_closure (cur_state_set);

      if (!get_state (cur_state_set, &new_dfa_state)) {
	/* nouveau */
	DPUSH (nfa_state_stack, new_dfa_state);
	sxdfa_ptr->last_state = new_dfa_state; /* On ne "conserve" que le + grand */
      }

      /* Ici on a une transition dfa_state ->t new_dfa_state */
      DSPUSH (sxdfa_ptr->next_state_list, new_dfa_state);
      sxdfa_ptr->number_of_out_trans++;
    }
  }

  /* On finalise */
  DPUSH (sxdfa_ptr->next_state_list, 0);
  DPUSH (sxdfa_ptr->states, DTOP (sxdfa_ptr->next_state_list));

  sxdfa_ptr->max_t_val = max_trans_value;

  sxdfa_ptr->private.from_left_to_right = TRUE;
  sxdfa_ptr->is_a_dag = FALSE; /* En fait on sait pas */
  sxdfa_ptr->max_path_lgth = 0;/* sans objet si pas DAG et si DAG sera calcule' + tard !! */ 


  if (nfa_empty_trans) {
    sxfree (nfa_ec_stack), nfa_ec_stack = NULL;
    sxfree (nfa_wstate_set), nfa_wstate_set = NULL;
  }

  /* ATTENTION truc */
  sxdfa_ptr->trans_list = trans_list_hd.list;
  trans_list_hd.list = (SXINT *) sxalloc (1, sizeof (SXINT));
  XH_free (&trans_list_hd);

  DFREE_STACK (nfa_state_stack);
  sxbm_free (t2nfa_state_set), t2nfa_state_set = NULL;
  XH_free (&XH_dfa_states);
  sxfree (t_set), t_set = NULL;

  if (to_be_minimized) {
    sxdfa_minimize (sxdfa_ptr, to_be_normalized);
  }
}


void
sxdfa_minimize (struct sxdfa_struct *sxdfa_ptr, BOOLEAN to_be_normalized)
{
  SXINT                 state, next_state, partition, next_partition, bot, top, part_set_id, cur_part, t_trans, trans_list_id, new_part;
  SXINT                 size, pred_state, cur_state, next_part, cur_part_card, nb_max, id, nb, id_max, last_part_set_id, old_last_partition, trans_nb, is_final_state;
  SXINT                 *part2new_part, *new_part2part, *area, *base_area, *working_stack, *w2_stack;
  SXBA                  new_part_final_set;
  struct part2attr      *cur_part_ptr;
  struct state2attr     *cur_state_ptr;
#if LOG
  SXINT                 constant_time_step_nb = 0;             
#endif /* LOG */

  sxinitialise (id_max);

  XH_alloc (&XH_trans_list_id, "XH_trans_list_id", sxdfa_ptr->last_state+1, 1, sxdfa_ptr->max_arity, NULL, NULL);

  XH_alloc (&XH_part_set, "XH_part_set", sxdfa_ptr->last_state+1, 1, sxdfa_ptr->max_arity, XH_part_set_oflw, NULL);
  part_set_id2part = (SXINT *) sxalloc (XH_size (XH_part_set)+1, sizeof (SXINT));

  working_stack = (SXINT*) sxalloc (2*sxdfa_ptr->last_state+1, sizeof (SXINT)), working_stack [0] = 0;

  /* dfa_state2pred_nb = NULL; On n'a pas besoin des predecesseurs */
  dfa_state2pred_nb = (SXINT*) sxcalloc (sxdfa_ptr->last_state+1, sizeof (SXINT)); /* On utilise les predecesseurs (voir commentaire de tete) */

  state2attr = (struct state2attr*) sxalloc (sxdfa_ptr->last_state+1, sizeof (struct state2attr));
  part2attr = (struct part2attr*) sxcalloc (sxdfa_ptr->last_state+1, sizeof (struct part2attr));

  w2_stack = (SXINT*) sxalloc (sxdfa_ptr->last_state+1, sizeof (SXINT)), w2_stack [0] = 0;

  /* initialisation ... */
  last_partition = 0;

  for (state = sxdfa_ptr->init_state; state <= sxdfa_ptr->last_state ; state++) {
    /* Analogue de fill_init_min () */
    bot = sxdfa_ptr->states [state];
    is_final_state = bot & 1;
    bot >>= 1;
    top = sxdfa_ptr->states [state+1]>>1;

    t_trans = sxdfa_ptr->next_state_list [bot++];

    if (is_final_state)
      XH_push (XH_trans_list_id, -t_trans);
    else
      XH_push (XH_trans_list_id, t_trans);

    while (bot < top) {
      next_state = sxdfa_ptr->next_state_list [bot++];
      XH_push (XH_trans_list_id, next_state);

      if (dfa_state2pred_nb) {
	/* On en profite pour compter les predecesseurs de next_dfa_state */
	dfa_state2pred_nb [next_state]++;
	dfa_state2pred_nb [0]++;
      }
    }

    /* A chaque etat state on lui associe la liste de ses successeurs ds un XH
       l'ordre des successeurs est donne' par l'ordre des terminaux des transitions
       chaque element d'un groupe a le meme ensemble de symboles terminaux, leurs successeurs
       peuvent donc se comparer simplement */
    XH_set (&XH_trans_list_id, &(state2attr [state].trans_list_id));

    state2attr [state].init_part = t_trans;
  }

  /* Ici chaque etat a recu une "partition" pre_initiale qui est son t_trans On fabrique une vraie partition initiale */
  for (state = sxdfa_ptr->init_state; state <= sxdfa_ptr->last_state ; state++) {
    trans_list_id = state2attr [state].trans_list_id;
    bot = XH_X (XH_trans_list_id, trans_list_id);
    top = XH_X (XH_trans_list_id, trans_list_id+1);

    t_trans = XH_list_elem (XH_trans_list_id, bot++); /* peut etre <0 ou >0 suivant que les etats de la partition sont finals ou pas */
    XH_push (XH_part_set, t_trans);

    while (bot < top) {
      next_state = XH_list_elem (XH_trans_list_id, bot);
      next_part = state2attr [next_state].init_part;
      XH_push (XH_part_set, -next_part); /* Il ne faut pas les confondre plus tard avec des "vraies" partition */

      bot++;
    }
	  
    if (!XH_set (&XH_part_set, &part_set_id))
      /* Nouveau */
      part_set_id2part [part_set_id] = next_part = ++last_partition;
    else
      next_part = part_set_id2part [part_set_id];

    /* On installe state ds next_part */
    init_set (state, next_part);
  }

#if LLOG
  configuration_nb = 0;
#endif /* LLOG */
    
  dfa_state2pred_stack = NULL;
  base_area = NULL;

  if (dfa_state2pred_nb) {
    SXINT              pred_part, cur_last_partition, cur_part_sub_card;
    SXINT              *w_state_stack, *top_state_ptr, *state_ptr, *tbp_part_stack, *tbp_state_stacks, *tbp_state_stack, *top_part_ptr, *part_ptr, *pred_stack;
    SXBA             w_state_set;
    struct part2attr *pred_part_ptr;

    /* version de minimize dans laquelle on a besoin des predecesseurs */
    size = dfa_state2pred_nb [0] + sxdfa_ptr->last_state /* les comptes */;

    dfa_state2pred_stack = (SXINT **) sxalloc (sxdfa_ptr->last_state+1, sizeof (SXINT*));
    area = base_area = (SXINT*) sxalloc (size+1, sizeof (SXINT));
    w_state_set = sxba_calloc (sxdfa_ptr->last_state+1);
    w_state_stack = (SXINT *) sxalloc (sxdfa_ptr->last_state+1, sizeof (SXINT));
    tbp_part_stack = (SXINT *) sxalloc (sxdfa_ptr->last_state+1, sizeof (SXINT)), RAZ (tbp_part_stack);
    tbp_state_stacks = (SXINT *) sxalloc (2*sxdfa_ptr->last_state+1, sizeof (SXINT)), RAZ (tbp_state_stacks);

    sxba_fill (w_state_set), SXBA_0_bit (w_state_set, 0);
    w_state_stack [0] = sxdfa_ptr->last_state;

    for (state = 1; state <= sxdfa_ptr->last_state; state++) {
      w_state_stack [state] = state; /* PUSH */
      dfa_state2pred_stack [state] = area;
      *area = 0;
      area += dfa_state2pred_nb [state]+1;
    }

    for (state = sxdfa_ptr->init_state; state < sxdfa_ptr->last_state ; state++)
      /* On remplit dfa_state2pred_stack */
      (*dfa_extract_trans)(state, dfa_fill_dfa_state2pred_stack);

    sxfree (dfa_state2pred_nb), dfa_state2pred_nb = NULL;

    for(;;) {
#if LLOG
      print_configuration ();
#endif /* LLOG */

      /* Ici, les etats a traiter ont ete mis ds (w_state_stack, w_state_set) */
      /* On decoupe les etats a traiter par leur appartenance a la meme partition */
      top_state_ptr = w_state_stack+TOP(w_state_stack);

      /* On compte */
      for (state_ptr = top_state_ptr; state_ptr > w_state_stack; state_ptr--) {
	pred_state = *state_ptr;
	pred_part = state2attr [pred_state].part;

	if (part2attr [pred_part].sub_card++ == 0) {
	  /* 1ere fois que pred_part est trouve' */
	  PUSH (tbp_part_stack, pred_part);
	}
      }
	
      /* On prepare les piles */
      RAZ (tbp_state_stacks);

      top_part_ptr = tbp_part_stack+TOP(tbp_part_stack);

      for (part_ptr = top_part_ptr; part_ptr > tbp_part_stack; part_ptr--) {
	pred_part_ptr = part2attr + *part_ptr;
	PUSH (tbp_state_stacks, 0);
	pred_part_ptr->tbp_state_stack = tbp_state_stacks + TOP (tbp_state_stacks);
	TOP (tbp_state_stacks) += pred_part_ptr->sub_card;
	pred_part_ptr->sub_card = 0;
      }

      /* On stocke */
      for (state_ptr = top_state_ptr; state_ptr > w_state_stack; state_ptr--) {
	pred_state = *state_ptr;
	SXBA_0_bit (w_state_set, pred_state);
	pred_part = state2attr [pred_state].part;
	tbp_state_stack = part2attr [pred_part].tbp_state_stack;
	PUSH (tbp_state_stack, pred_state);
      }

      RAZ (w_state_stack);

      /* On traite */
      cur_last_partition = last_partition;

      while (!IS_EMPTY (tbp_part_stack)) {
#if LOG
	constant_time_step_nb++;             
#endif /* LOG */
	cur_part = POP (tbp_part_stack);
	cur_part_ptr = part2attr+cur_part;
	cur_part_card = cur_part_ptr->card;

	if (cur_part_card > 1) {
	  tbp_state_stack = cur_part_ptr->tbp_state_stack;
	  cur_part_sub_card = TOP (tbp_state_stack);
#if EBUG
	  if (cur_part_sub_card > cur_part_card)
	    sxtrap (ME, "dfa_minimize");
#endif /* EBUG */

	  RAZ (w2_stack);
	  nb_max = cur_part_card-cur_part_sub_card;
	  PUSH (w2_stack, nb_max); /* Pour les etats dont les successeurs n'ont pas bouge's */
	  id_max = 1; /* Le TOP */
	  last_part_set_id = XH_top (XH_part_set)-1;
	
	  for (state_ptr = tbp_state_stack+TOP(tbp_state_stack); state_ptr > tbp_state_stack; state_ptr--) {
	    cur_state = *state_ptr;
	    cur_state_ptr = state2attr + cur_state;

	    /* On recalcule ses etats successeurs */
	    trans_list_id = cur_state_ptr->trans_list_id;
	    bot = XH_X (XH_trans_list_id, trans_list_id);
	    top = XH_X (XH_trans_list_id, trans_list_id+1);

	    t_trans = XH_list_elem (XH_trans_list_id, bot++); /* peut etre <0 ou >0 suivant que les etats de la partition sont finals ou pas */
	    XH_push (XH_part_set, t_trans);

	    while (bot < top) {
#if LOG
	      constant_time_step_nb++;             
#endif /* LOG */
	      next_state = XH_list_elem (XH_trans_list_id, bot);
	      next_partition = state2attr [next_state].part;
	      XH_push (XH_part_set, next_partition);

	      bot++;
	    }

	    if (!XH_set (&XH_part_set, &part_set_id)) {
	      /* Nouveau */
	      PUSH (w2_stack, 0);
	      id = TOP (w2_stack);

#if EBUG 
	      if (TOP (w2_stack) != part_set_id-last_part_set_id+1)
		sxtrap (ME, "dfa_minimize");
#endif /* EBUG */
	    }
	    else {
	      id = (part_set_id <= last_part_set_id) ? 1 : part_set_id-last_part_set_id+1;
	    }

	    nb = ++(w2_stack [id]);

	    if (nb > nb_max) {
	      nb_max = nb;
	      id_max = id;
	    }

	    cur_state_ptr->temp_id = part_set_id;
	  }

	  /* Si nb_max == cur_part_card, on ne change rien */
	  if (nb_max < cur_part_card) {
	    old_last_partition = last_partition;

	    /* Ds cur_part on ne va pas deplacer le + grand groupe =>
	       chacun des groupes deplaces a une taille bornee par cur_part_card/2 */

	    /* C'est le sous-groupe qui n'a pas ete touche' qui est le plus populaire
	       On deplace les etats du complementaire ... */
	    for (state_ptr = tbp_state_stack+TOP(tbp_state_stack); state_ptr > tbp_state_stack; state_ptr--) {
#if LOG
	      constant_time_step_nb++;             
#endif /* LOG */
	      cur_state = *state_ptr;

	      if (id_max != 1 && w2_stack [1] != 0)
		/* Etat implique' par le rafinement precedant */
		SXBA_1_bit (w_state_set, cur_state);

	      cur_state_ptr = state2attr + cur_state;
	      part_set_id = cur_state_ptr->temp_id;
	      id = (part_set_id <= last_part_set_id) ? 1 : part_set_id-last_part_set_id+1;

	      if (id != id_max) {
		/* On bouge cur_state */
		if ((next_part = w2_stack [id]) > 0) {
		  /* 1ere fois */
		  next_part = ++last_partition;
		  w2_stack [id] = -next_part; /* Pour les coups d'apres */
		}
		else
		  next_part = -next_part;

		/* cur_state est change' de partition */
		/* Ce changement ne peut etre valide' au cours de ce run, il le sera + tard sinon les futurs calculs ds XH_part_set peuvent etre faux */
		/* state2part [cur_state] = next_part; doit etre retarde' sinon les futurs calculs ds  XH_part_set peuvent etre faux */
		PUSH (working_stack, cur_state);
		PUSH (working_stack, next_part);
	      }
	      else
		/* On ne bouge pas la sous-part maximale */
		next_part = cur_part;

#if EBUG
	      if (next_part != cur_part && next_part <= old_last_partition)
		sxtrap (ME, "dfa_minimize");
#endif /* EBUG */

	      /* part_set_id2part [part_set_id] = next_part; semble inutile */
	    }
	    
	    if (id_max != 1 && w2_stack [1] != 0) {
	      /* Le sous-groupe non touche' par le rafinement pre'ce'dant n'est pas le plus populaire, il va donc etre de'place' aussi car il est non vide */
	      next_part = ++last_partition;

	      for (cur_state = cur_part_ptr->state; cur_state != 0; cur_state = state2attr [cur_state].next_state) { 
		if (!SXBA_bit_is_set_reset (w_state_set, cur_state)) {
		  PUSH (working_stack, cur_state);
		  PUSH (working_stack, next_part);
		}
	      }
	    }
	  }
	}
      }

      if (!IS_EMPTY (working_stack)) {
	do {
	  cur_part = POP (working_stack);
	  cur_state = POP (working_stack);
	  move (cur_state, cur_part);

	  pred_stack = dfa_state2pred_stack [cur_state];

	  for (state_ptr = pred_stack+TOP(pred_stack); state_ptr > pred_stack; state_ptr--) {
#if LOG
	    constant_time_step_nb++;             
#endif /* LOG */
	    pred_state = *state_ptr;

	    if (SXBA_bit_is_reset_set (w_state_set, pred_state))
	      PUSH (w_state_stack, pred_state);
	  }
	} while (!IS_EMPTY (working_stack));
      }
      else
	break;
    }

    sxfree (w_state_stack);
    sxfree (w_state_set);
    sxfree (tbp_part_stack);
    sxfree (tbp_state_stacks);
  }
  else {
    /* version de minimize dans laquelle on n'utilise pas les predecesseurs */
    for(;;) {
#if LLOG
      print_configuration ();
#endif /* LLOG */

      for (cur_part = last_partition; cur_part >= 1; cur_part--) {
#if LOG
	constant_time_step_nb++;             
#endif /* LOG */
	cur_part_ptr = part2attr+cur_part;
	cur_part_card = cur_part_ptr->card;

	if (cur_part_card > 1) {
	  RAZ (w2_stack);
	  PUSH (w2_stack, 0); /* Pour les etats dont les successeurs n'ont pas bouge's */
	  nb_max = 0;
	  last_part_set_id = XH_top (XH_part_set)-1;

	  for (cur_state = cur_part_ptr->state; cur_state != 0; cur_state = cur_state_ptr->next_state) {
	    cur_state_ptr = state2attr + cur_state;

	    /* On recalcule ses etats successeurs */
	    trans_list_id = cur_state_ptr->trans_list_id;
	    bot = XH_X (XH_trans_list_id, trans_list_id);
	    top = XH_X (XH_trans_list_id, trans_list_id+1);

	    t_trans = XH_list_elem (XH_trans_list_id, bot++); /* peut etre <0 ou >0 suivant que les etats de la partition sont finals ou pas */
	    XH_push (XH_part_set, t_trans);

	    while (bot < top) {
#if LOG
	      constant_time_step_nb++;             
#endif /* LOG */
	      next_state = XH_list_elem (XH_trans_list_id, bot);
	      next_partition = state2attr [next_state].part;
	      XH_push (XH_part_set, next_partition);

	      bot++;
	    }

	    if (!XH_set (&XH_part_set, &part_set_id)) {
	      /* Nouveau */
	      PUSH (w2_stack, 0);
	      id = TOP (w2_stack);

#if EBUG 
	      if (TOP (w2_stack) != part_set_id-last_part_set_id+1)
		sxtrap (ME, "dfa_minimize");
#endif /* EBUG */
	    }
	    else {
	      id = (part_set_id <= last_part_set_id) ? 1 : part_set_id-last_part_set_id+1;
	    }

	    nb = ++(w2_stack [id]);

	    if (nb > nb_max) {
	      nb_max = nb;
	      id_max = id;
	    }

	    cur_state_ptr->temp_id = part_set_id;
	  }

	  /* Si nb_max == cur_part_card, on ne change rien */
	  if (nb_max < cur_part_card) {
	    old_last_partition = last_partition;

	    /* Ds cur_part on ne va pas deplacer le + grand groupe =>
	       chacun des groupes deplaces a une taille bornee par cur_part_card/2 */
	    for (cur_state = cur_part_ptr->state; cur_state != 0; cur_state = cur_state_ptr->next_state) { 
#if LOG
	      constant_time_step_nb++;             
#endif /* LOG */
	      cur_state_ptr = state2attr + cur_state;
	      part_set_id = cur_state_ptr->temp_id;
	      id = (part_set_id <= last_part_set_id) ? 1 : part_set_id-last_part_set_id+1;

	      if (id != id_max) {
		/* On bouge cur_state */
		if ((next_part = w2_stack [id]) > 0) {
		  /* 1ere fois */
		  next_part = ++last_partition;
		  w2_stack [id] = -next_part; /* Pour les coups d'apres */
		}
		else
		  next_part = -next_part;

		/* cur_state est change' de partition */
		/* Ce changement ne peut etre valide' au cours de ce run, il le sera + tard sinon les futurs calculs ds XH_part_set peuvent etre faux */
		/* state2part [cur_state] = next_part; doit etre retarde' sinon les futurs calculs ds  XH_part_set peuvent etre faux */
		PUSH (working_stack, cur_state);
		PUSH (working_stack, next_part);
	      }
	      else
		/* On ne bouge pas la sous-part maximale */
		next_part = cur_part;

#if EBUG
	      if (next_part != cur_part && next_part <= old_last_partition)
		sxtrap (ME, "dfa_minimize");
#endif /* EBUG */

	      /* part_set_id2part [part_set_id] = next_part; semble inutile */
	    }
	  }
	}
      }

      if (!IS_EMPTY (working_stack)) {
	do {
#if LOG
	  constant_time_step_nb++;             
#endif /* LOG */
	  cur_part = POP (working_stack);
	  cur_state = POP (working_stack);
	  move (cur_state, cur_part);
	} while (!IS_EMPTY (working_stack));
      }
      else
	break;
    }
  }

#if LOG
  printf ("constant_time_step_nb (with%s backward transitions) = %ld\n", dfa_state2pred_stack ? "" : "out", (long)constant_time_step_nb);
#endif /* LOG */
    
  sxfree (working_stack);
  sxfree (w2_stack);
  XH_free (&XH_part_set);
  sxfree (part_set_id2part), part_set_id2part = NULL;

  if (dfa_state2pred_stack) {
    sxfree (dfa_state2pred_stack), dfa_state2pred_stack = NULL;
    sxfree (base_area);
  }

  /* On assure seulement que l'etat initial a la valeur min
     et l'etat final la valeur max */
  /* On en a aussi besoin ds fsa_normalize () */
  part2new_part = (SXINT *) sxcalloc (last_partition+1, sizeof (SXINT));
  new_part2part = (SXINT *) sxalloc (last_partition+1, sizeof (SXINT));
  new_part = 1;
  trans_nb = 0;

  for (state = sxdfa_ptr->init_state; state <= sxdfa_ptr->last_state; state++) {
    partition = state2attr [state].part;
    state = part2attr [partition].state;
    trans_nb += sxdfa_ptr->states [state+1]-sxdfa_ptr->states [state]-1;

    if (part2new_part [partition] == 0) {
      part2new_part [partition] = new_part;
      new_part2part [new_part] = partition;
      new_part++;
    }
  }

  /* On prepare le nouveau sxdfa_ptr */
  sxdfa_free (sxdfa_ptr);

  sxdfa_alloc (sxdfa_ptr, last_partition, trans_nb, NULL, NULL);
  new_part_final_set = sxba_calloc (last_partition+1);

  sxdfa_ptr->final_state_nb = sxdfa_ptr->number_of_out_trans = 0;
    
  for (new_part = 1; new_part <= last_partition; new_part++) {
    partition = new_part2part [new_part];
    state = part2attr [partition].state;

    trans_list_id = state2attr [state].trans_list_id;
    bot = XH_X (XH_trans_list_id, trans_list_id);
    top = XH_X (XH_trans_list_id, trans_list_id+1);

    t_trans = XH_list_elem (XH_trans_list_id, bot++);

    if (t_trans < 0) {
      /* etat final */
      PUSH (sxdfa_ptr->next_state_list, -t_trans);
      PUSH (sxdfa_ptr->states, (TOP (sxdfa_ptr->next_state_list)<<1)+1);

      if (SXBA_bit_is_reset_set (new_part_final_set, new_part))
	sxdfa_ptr->final_state_nb++;
    }
    else {
      PUSH (sxdfa_ptr->next_state_list, t_trans);
      PUSH (sxdfa_ptr->states, (TOP (sxdfa_ptr->next_state_list)<<1));
    }

    sxdfa_ptr->number_of_out_trans += top-bot;

    while (bot < top) {
      next_state = XH_list_elem (XH_trans_list_id, bot);
      next_partition = state2attr [next_state].part;
      PUSH (sxdfa_ptr->next_state_list, part2new_part [next_partition]);
      bot++;
    }
  }
    
  PUSH (sxdfa_ptr->next_state_list, 0);
  PUSH (sxdfa_ptr->states, TOP (sxdfa_ptr->next_state_list));

  sxdfa_ptr->init_state = 1;
  sxdfa_ptr->last_state = last_partition;
  sxdfa_ptr->final_state_nb = sxdfa_ptr->number_of_out_trans = sxdfa_ptr->max_arity = 0;
  /* INCHANGE'S : 
     sxdfa_ptr->max_path_lgth;
     sxdfa_ptr->max_arity;
     sxdfa_ptr->max_t_val;
     sxdfa_ptr->private.from_left_to_right;
     sxdfa_ptr->is_a_dag = FALSE;
  */

  sxfree (new_part_final_set);

  sxfree (part2new_part);
  sxfree (new_part2part);

  sxfree (part2attr), part2attr = NULL;
  sxfree (state2attr), state2attr = NULL;

  XH_free (&XH_trans_list_id);

  if (to_be_normalized)
    sxdfa_normalize (sxdfa_ptr);
}


static void
sxdfa_fill_old_state2max_path_lgth (struct sxdfa_struct *sxdfa_ptr, SXINT state)
{
  SXINT bot, top, next_state, prev_max_path_lgth, next_max_path_lgth, max_path_lgth;

  if (SXBA_bit_is_reset_set (state_set, state)) {
    /* Pas de cycle sur state */
    prev_max_path_lgth = old_state2max_path_lgth [state];

    bot = sxdfa_ptr->states [state]>>1;
    top = sxdfa_ptr->states [state+1]>>1;

    while (++bot < top) {
      next_state = sxdfa_ptr->next_state_list [bot];
      next_max_path_lgth = old_state2max_path_lgth [next_state];

      if (next_max_path_lgth == 0 || next_max_path_lgth < prev_max_path_lgth) {
	/* C'est la 1ere fois qu'on tombe sur next_old_state */
	/* On a deja atteint next_old_state par un autre chemin "normal" mais plus court... */
	max_path_lgth = prev_max_path_lgth+1;
	old_state2max_path_lgth [next_state] = max_path_lgth;

	if (max_path_lgth > old_state2max_path_lgth [0])
	  old_state2max_path_lgth [0] = max_path_lgth;

	sxdfa_fill_old_state2max_path_lgth (sxdfa_ptr, next_state);
      }
      /*
	else
	rien
      */

    }

    SXBA_0_bit (state_set, state);
  }
  else {
    /*
      Detection d'un cycle ds l'automate
      On ne touche pas a old_state2max_path_lgth [state]
    */
    normalized_is_cyclic = TRUE;
  }
}


void
sxdfa_normalize (struct sxdfa_struct *sxdfa_ptr)
{
  SXINT state, new_state, old_state, bot, top, next_old_state, next_new_state, is_final_state;
  SXINT *cur_to_be_sorted, *old_states, *old_next_state_list;

  old_state2max_path_lgth = (SXINT*) sxcalloc (sxdfa_ptr->last_state+1, sizeof (SXINT));
  state_set = sxba_calloc (sxdfa_ptr->last_state+1);

  SXBA_1_bit (state_set, sxdfa_ptr->init_state);
  old_state2max_path_lgth [sxdfa_ptr->init_state] = 1; /* pour reserver 0 a non encore traite' */

  /* On calcule la longueur maximale des chemins qui conduisent depuis l'etat initial a chaque etat de l'automate ... */
  normalized_is_cyclic = FALSE;

  sxdfa_fill_old_state2max_path_lgth (sxdfa_ptr, sxdfa_ptr->init_state);

  sxdfa_ptr->is_a_dag = !normalized_is_cyclic;

  if (sxdfa_ptr->is_a_dag)
    sxdfa_ptr->max_path_lgth = old_state2max_path_lgth [0];

  /* Attention, si l'automate en entree contient des cycles (c'est pas un DAG), l'algo precedent n'assure pas
     que l'etat final sera a` la distance max de l'etat initial, on y remedie */
  old_state2max_path_lgth [sxdfa_ptr->last_state] = sxdfa_ptr->last_state;

  /* ... On trie ces etats par ces longueurs ... */
  sxfree (state_set), state_set = NULL;
  cur_to_be_sorted = (SXINT*) sxalloc (sxdfa_ptr->last_state+1, sizeof (SXINT));

  for (state = 0; state <= sxdfa_ptr->last_state; state++)
    cur_to_be_sorted [state] = state;

  qsort(cur_to_be_sorted+sxdfa_ptr->init_state, sxdfa_ptr->last_state-sxdfa_ptr->init_state+1, sizeof (SXINT), cmp_old_state);

  sxfree (old_state2max_path_lgth), old_state2max_path_lgth = NULL;

  /* Pour des questions d'ordre de sortie, on a besoin de la correspondance old_state -> new_state */
  old_state2new_state = (SXINT*) sxalloc (sxdfa_ptr->last_state+1, sizeof (SXINT));

  for (new_state = sxdfa_ptr->init_state; new_state <= sxdfa_ptr->last_state; new_state++) {
    state = cur_to_be_sorted [new_state];
    old_state2new_state [state] = new_state;
  }

  /* ... et on sort l'automate "normalise' */
  /* par ordre croissant des etats d'origine de chaque transition (pour chaque p, on sort tous les p ->t q) */
  old_states = sxdfa_ptr->states;
  old_next_state_list = sxdfa_ptr->next_state_list;

  sxdfa_alloc (sxdfa_ptr, sxdfa_ptr->last_state, sxdfa_ptr->number_of_out_trans, NULL, NULL);

  for (new_state = sxdfa_ptr->init_state; new_state < sxdfa_ptr->last_state; new_state++) {
    old_state = cur_to_be_sorted [new_state];

    bot = old_states [old_state];
    is_final_state = bot & 1;
    bot >>= 1;
    top = old_states [old_state+1]>>1;

    PUSH (sxdfa_ptr->next_state_list, old_next_state_list [bot++]);

    if (is_final_state)
      PUSH (sxdfa_ptr->states, (TOP (sxdfa_ptr->next_state_list)<<1)+1);
    else
      PUSH (sxdfa_ptr->states, (TOP (sxdfa_ptr->next_state_list)<<1));

    while (bot < top) {
      next_old_state = old_next_state_list [bot++];
      next_new_state = old_state2new_state [next_old_state];
      PUSH (sxdfa_ptr->next_state_list, next_new_state);
    }
  }

  PUSH (sxdfa_ptr->next_state_list, 0);
  PUSH (sxdfa_ptr->states, TOP (sxdfa_ptr->next_state_list));

  /* Le reste de sxdfa_ptr est inchange' */
    
  sxfree (cur_to_be_sorted), cur_to_be_sorted = NULL;
  sxfree (old_state2new_state);

  sxfree (old_states);
  sxfree (old_next_state_list);
}


static BOOLEAN
sxdfa_fill_dag_hd (struct sxdfa_struct *sxdfa_ptr, SXINT p, SXINT t, SXINT q)
{
  SXINT           dag_id;
  BOOLEAN         first_time;
  struct dag_attr *attr_ptr;

  first_time = !XxY_set (&dag_hd, p, q, &dag_id);

  attr_ptr = dag_id2attr+dag_id;

  if (first_time) {
    dag_hd_trans_nb++;
    attr_ptr->trans = ATOMIC_CODE | t;
  }
  else {
    /* On fait un or */
    attr_ptr->trans = make_code (attr_ptr->trans, ATOMIC_CODE | t, OR_TRANS);
  }

  if (sxdfa_ptr->states [q] & 1) {
    /* etat final */
    /* Traitement des EOW */
    first_time = !XxY_set (&dag_hd, q, sxdfa_ptr->last_state+1, &dag_id);

    attr_ptr = dag_id2attr+dag_id;

    if (first_time) {
      dag_hd_trans_nb++;
      attr_ptr->trans = ATOMIC_CODE; /* t == 0 !! */
    }
    else {
      /* On fait un or */
      attr_ptr->trans = make_code (attr_ptr->trans, ATOMIC_CODE, OR_TRANS);
    }
  }

  return TRUE;
}



/* appelle la fonction f sur toutes les transitions issues de state
   Si f retourne FALSE, arret
*/
void
sxdfa_extract_trans (struct sxdfa_struct *sxdfa_ptr, SXINT state, BOOLEAN (*f)(struct sxdfa_struct *, SXINT, SXINT, SXINT))
{
  SXINT trans_nb, bot, top, bot_trans, is_final;
  SXINT *state_ptr, *next_states_ptr, *bot_trans_list_ptr, *top_trans_list_ptr;

  state_ptr = sxdfa_ptr->states + state;
  bot = state_ptr [0];
  top = state_ptr [1]>>1;

  is_final = bot & 1;
  bot >>= 1;

  trans_nb = top-bot;

#if EBUG
  if (trans_nb <= 0)
    sxtrap (ME, "sxdfa_extract_trans");
#endif
  
  next_states_ptr = sxdfa_ptr->next_state_list + bot;
  bot_trans = *next_states_ptr++;

  if (bot_trans < 0) {
    if (is_final) {
      /* L'etat state est final et -bot_trans est l'identifiant associe' */
      if (!(*f)(sxdfa_ptr, state, bot_trans, 0 /* next_state */) || trans_nb == 1)
	return;

      bot_trans = *next_states_ptr++;

      if (bot_trans < 0) {
	/* ici -bot_trans est l'identifiant associe' au mot unique qui sera reconnu sur
	   le futur etat final */
#if EBUG
	if (trans_nb != 3)
	  sxtrap (ME, "sxdfa_extract_trans");
#endif /* EBUG */

	(*f)(sxdfa_ptr, state, bot_trans, *next_states_ptr);
	return; /* Ds tous les cas */
      }

      /* Ici bot_trans > 0 est l'identifiant d'un ensemble de transitions */
      trans_nb--;
    }
    else {
      /* ici -bot_trans est l'identifiant associe' au mot unique qui sera reconnu sur
	 le futur etat final */
#if EBUG
      if (trans_nb != 2)
	sxtrap (ME, "sxdfa_extract_trans");
#endif /* EBUG */

      (*f)(sxdfa_ptr, state, bot_trans, *next_states_ptr);
      return; /* Ds tous les cas */
    }
  }
  else {
    if (is_final) {
      /* L'etat state est final (mais n'est pas une feuille) */
      if (!(*f)(sxdfa_ptr, state, -1, 0 /* next_state */) || trans_nb == 1)
	return;
    }
  }

  bot_trans_list_ptr = sxdfa_ptr->trans_list + bot_trans;
  top_trans_list_ptr = bot_trans_list_ptr + trans_nb -1;

  while (bot_trans_list_ptr < top_trans_list_ptr) {
    if (!(*f)(sxdfa_ptr, state, *bot_trans_list_ptr++, *next_states_ptr++))
      break;
  }
}


VARSTR
sxdfadag2re (VARSTR varstr_ptr, struct sxdfa_struct *sxdfa_ptr, char *(*get_trans_name) (SXINT))
{
  SXINT p, single_trans, code;

  dag_get_trans_name = get_trans_name;
  final_state = sxdfa_ptr->last_state;

  XH_alloc (&code_hd, "code_hd", final_state+1, 1, sxdfa_ptr->number_of_out_trans/final_state + 1, code_hd_oflw, NULL);
  code2size = (SXINT*) sxalloc (XH_size (code_hd)+1, sizeof (SXINT));

#if EBUG
  wvstr = varstr_alloc (256);
#endif /* EBUG */
  XxY_alloc (&dag_hd, "dag_hd", sxdfa_ptr->last_state+1, 1, 1 /* Xforeach */, 1 /* Yforeach */, dag_hd_oflw, NULL);
  dag_id2attr = (struct dag_attr*) sxalloc (XxY_size (dag_hd)+1, sizeof (struct dag_attr));

  dag_hd_trans_nb = 0;

  for (p = sxdfa_ptr->init_state; p < sxdfa_ptr->last_state; p++) {
    sxdfa_extract_trans (sxdfa_ptr, p, sxdfa_fill_dag_hd);
  }

  /* Nelle factorisation des OR */
  XxY_alloc (&paths, "paths", 30, 1, 1 /* Xforeach */, 0, paths_oflw, NULL);
  path2lgth = (SXINT *) sxalloc (XxY_size (paths)+1, sizeof (SXINT));
  path2tree = (SXINT *) sxalloc (XxY_size (paths)+1, sizeof (SXINT));
  paths_set = sxba_calloc (XxY_size (paths)+1);
  final_paths_set = sxba_calloc (XxY_size (paths)+1);
  paths_stack = (SXINT *) sxalloc (XxY_size (paths)+1, sizeof (SXINT)), paths_stack [0] = 0;
  XxY_alloc (&branches, "branches", 30, 1, 0, 0, branches_oflw, NULL);
  branch2occur_nb = (SXINT *) sxalloc (XxY_size (branches)+1, sizeof (SXINT));
  branches_set = sxba_calloc (XxY_size (branches)+1);
  XxY_alloc (&pathXbranch, "pathXbranch", 30, 1, 0, 1 /* Yforeach */, NULL, NULL);
  XxY_alloc (&pathXtree, "pathXtree", 30, 1, 0, 1 /* Yforeach */, NULL, NULL);
  XH_alloc (&trees, "trees", 30, 1, 4, trees_oflw, NULL);
  tree2size = (SXINT *) sxalloc (XH_size (trees)+1, sizeof (SXINT));
  tree2occur_nb = (SXINT *) sxalloc (XH_size (trees)+1, sizeof (SXINT));
  DALLOC_STACK (code_stack, 100);

  single_trans = 1; /* Si trans unique ds le DAG */

  while (dag_hd_trans_nb > 1) {
    single_trans = dag_reduce ();
  }

  code = dag_id2attr [single_trans].trans;

  if (code2trans (code) == OR_TRANS)
    code = factorize_OR (code);
  
  XxY_free (&paths);
  sxfree (paths_set), paths_set = NULL;
  sxfree (final_paths_set), final_paths_set = NULL;
  sxfree (paths_stack), paths_stack = NULL;
  sxfree (path2tree), path2tree = NULL;
  sxfree (path2lgth), path2lgth = NULL;
  XxY_free (&branches);
  sxfree (branch2occur_nb), branch2occur_nb = NULL;
  sxfree (branches_set), branches_set = NULL;
  XxY_free (&pathXbranch);
  XxY_free (&pathXtree);
  XH_free (&trees);
  sxfree (tree2size), tree2size = NULL;
  sxfree (tree2occur_nb), tree2occur_nb = NULL;
  DFREE_STACK (code_stack);

  varstr_ptr = print_code (varstr_ptr, code);

#if EBUG
  varstr_free (wvstr);
#endif /* EBUG */
  XxY_free (&dag_hd);
  sxfree (dag_id2attr), dag_id2attr = NULL;
  XH_free (&code_hd);
  sxfree (code2size), code2size = NULL;

  return varstr_ptr;

}



void
sxdfa2c (struct sxdfa_struct *sxdfa_ptr, FILE *file, char *name, BOOLEAN is_static)
{
  SXINT  i;


  fprintf (file, "\nstatic SXINT %s_states [%ld] = {\n", name, sxdfa_ptr->states [0]+1);

  for (i = 0; i <= sxdfa_ptr->states [0]; i++) {
    fprintf (file, "/* %ld */ %ld,\n", i, sxdfa_ptr->states [i]);
  }

  fprintf (file, "};\n");

  fprintf (file, "\nstatic SXINT %s_next_state_list [%ld] = {\n", name, sxdfa_ptr->next_state_list [0]+1);

  for (i = 0; i <= sxdfa_ptr->next_state_list [0]; i++) {
    fprintf (file, "/* %ld */ %ld,\n", i, sxdfa_ptr->next_state_list [i]);
  }

  fprintf (file, "};\n");

  fprintf (file, "\nstatic SXINT %s_trans_list [%ld] = {\n", name, sxdfa_ptr->trans_list [0]+1);

  for (i = 0; i <= sxdfa_ptr->trans_list [0]; i++) {
    fprintf (file, "/* %ld */ %ld,\n", i, sxdfa_ptr->trans_list [i]);
  }

  fprintf (file, "};\n");

  fprintf (file, "\n%sstruct sxdfa_struct %s = {\n", is_static ? "static " : "", name);
  fprintf (file, "/* init_state */ %ld,\n\
/* last_state */ %ld,\n\
/* number_of_out_trans */ %ld,\n\
/* final_state_nb */ %ld,\n\
/* max_path_lgth */ %ld,\n\
/* max_arity */ %ld,\n\
/* max_t_val */ %ld,\n\
/* name */ \"%s\",\n\
/* stats */ NULL,\n\
%s_states,\n\
%s_next_state_list,\n\
%s_trans_list,\n\
/* is_static */ TRUE,\n\
/* is_a_dag */ %s,\n\
 {\n\
 /* from_left_to_right */ %s,\
 }\n",
	   sxdfa_ptr->init_state,
	   sxdfa_ptr->last_state,
	   sxdfa_ptr->number_of_out_trans,
	   sxdfa_ptr->final_state_nb,
	   sxdfa_ptr->max_path_lgth,
	   sxdfa_ptr->max_arity,
	   sxdfa_ptr->max_t_val,
	   name,
	   name,
	   name,
	   name,
	   sxdfa_ptr->is_a_dag ? "TRUE" : "FALSE",
	   sxdfa_ptr->private.from_left_to_right ? "TRUE" : "FALSE"
	   );

  fprintf (file, "};\n");
}




/* Valeurs retournees :
   Soit state pour sxdfa_seek ()
   et lgth = *input_stack
   (la valeur initiale de *input_stack = input_lgth)

   lgth\state |  >0    |   0
   ------------------------------
        0     |   I    |   II
   ------------------------------
        >0    | III    |   IV
   ------------------------------

   I  : input est reconnu en entier
   II : input est un prefixe (resp. suffixe) d'un ou plusieurs word (inconnus)
   III: word est un prefixe de longueur input_lgth-lgth de input
   IV : input_lgth-lgth symboles du prefixe (resp. suffixe) de input ont ete reconnus
*/


/* kw ds le fsa repre'sente' sous forme d'un sxdfa_struct et retourne l'etat final ou 0 si kw n'est pas reconnu */
/* Si echec, kwl indique la partie suffixe non reconnue. */
/* Si pas from_left_to_right les mots sont ranges en sens inverse, on reconnait donc kw de droite a gauche */
/* Si seek_prefix_or_suffix on regarde si le plus long prefixe (resp. suffixe si !from_left_to_right) de kw est un mot du dico
   ds ce cas on retourne l'identifiant ds le dico de ce prefixe (et *kwl est la longueur du suffixe (resp. prefixe) non reconnu */
SXINT
sxdfa_seek_a_string (struct sxdfa_struct *sxdfa_ptr, char *kw, SXINT *kwl)
{
  sxdfa_ptr->private.ptr.char_ptr = (sxdfa_ptr->private.from_left_to_right) ? kw : kw+*kwl-1;

  return sxdfa_seek (sxdfa_ptr, string_get_next_symb, kwl);
}



SXINT
sxdfa_seek_a_word (struct sxdfa_struct *sxdfa_ptr, SXINT *input_stack)
{
  sxdfa_ptr->private.ptr.SXINT_ptr = (sxdfa_ptr->private.from_left_to_right) ? input_stack + 1 : input_stack + *input_stack;

  return sxdfa_seek (sxdfa_ptr, word_get_next_symb, input_stack);
}

/*  
    ret_val | lgth
   ---------|-------
     id>0   |   0    => OK
   -----------------
     id>0   |   >0   => le + long prefixe du mot est ds le dico (id), le suffixe est de longueur lgth
   -----------------
     id==0  |   0    => le mot est un prefixe de mots (non identifies) du dico
   -----------------
     id<0   |   0    => unused
   -----------------
     id==0  |   >0   => Echec total, le suffixe est de longueur lgth
   -----------------
     id<0   |   >0   => le mot est un prefixe du mot -id du dico

*/
SXINT
sxdfa_seek (struct sxdfa_struct *sxdfa_ptr, SXUINT (*get_next_symb) (struct sxdfa_private *), SXINT *input_lgth)
{
  SXINT		lgth, cur_index, state, trans_nb, trans, mid_trans, bot, is_final, bot_trans, final_id, last_final_id, final_lgth;
  SXINT         *state_ptr, *next_states_ptr, *trans_list_ptr, *bot_trans_list_ptr, *top_trans_list_ptr, *mid_trans_list_ptr, *top_next_state_ptr;

  lgth = *input_lgth;

  if (lgth < 0)
    return 0;

  sxinitialise (final_lgth);

  final_id = last_final_id = 0;
  state = sxdfa_ptr->init_state;

  while (lgth > 0 && state > 0) {
    state_ptr = sxdfa_ptr->states + state;
    bot = state_ptr [0];

    is_final = bot & 1;
  
    next_states_ptr = sxdfa_ptr->next_state_list + (bot>>1);
    bot_trans = *next_states_ptr++;
    top_next_state_ptr = sxdfa_ptr->next_state_list + (state_ptr [1]>>1);

    if (bot_trans < 0) {
      if (is_final) {
	/* L'etat state est final et -bot_trans est l'identifiant associe' ... */
	/* ... mais lgth > 0, on n'est donc pas interesse' par cette transition */
	/* On note le +long prefixe du mot en entree reconnu */
	final_id = -bot_trans;
	final_lgth = lgth;

	if (next_states_ptr == top_next_state_ptr) {
	  /* pas de transition => echec car lgth non nul */
	  break;
	}

	bot_trans = *next_states_ptr++;

	if (bot_trans < 0) {
	  /* ici -bot_trans est l'identifiant associe' au mot unique qui sera reconnu sur
	     le futur etat final */
	  last_final_id = -bot_trans;
	  bot_trans = *next_states_ptr++; /* C'est un trans_id */
	}
      }
      else {
      /* ici -bot_trans est l'identifiant associe' au mot unique qui sera reconnu sur
	 le futur etat final */
	  last_final_id = -bot_trans;
	  bot_trans = *next_states_ptr++; /* C'est un trans_id */
      }
    }
    else {
      if (is_final) {
	/* Un prefixe du mot est dans le dico */
	final_id = 1;
	final_lgth = lgth;
      }
    }

    cur_index = -1;
    bot_trans_list_ptr = trans_list_ptr = sxdfa_ptr->trans_list + bot_trans;
    top_trans_list_ptr = bot_trans_list_ptr + (top_next_state_ptr-next_states_ptr);

    trans = (*get_next_symb) (&(sxdfa_ptr->private));
    /* On cherche a quel index se trouve trans dans la "pile" trans_list_ptr ... */
    /* ... par dicho */

    while ((trans_nb = top_trans_list_ptr-bot_trans_list_ptr)) {
      mid_trans_list_ptr = bot_trans_list_ptr+trans_nb/2;
      mid_trans = *mid_trans_list_ptr;

      if (mid_trans == trans) {
	cur_index = mid_trans_list_ptr-trans_list_ptr;
	break;
      }

      if (mid_trans < trans)
	bot_trans_list_ptr = mid_trans_list_ptr+1;
      else
	top_trans_list_ptr = mid_trans_list_ptr;
    }

    if (cur_index < 0)
      /* Echec */
      break;

    lgth--;
    state = next_states_ptr [cur_index];
  }

  *input_lgth = lgth;

  if (lgth == 0) {
    /* On a reconnu le mot en entier ... */
    /* ... c'est OK uniquement si l'etat atteint est final */
    if (state == 0 || state < 0/* etat final feuille */) {
      /* Ok */
      if (state == 0)
	return last_final_id ? last_final_id : 1; /* OK : valeur retournee >0 et lgth == 0 */

      return -state;
    }

    bot = sxdfa_ptr->states [state];

    if (bot & 1 /* etat final "interne" */) {
      /* Ok */
#if EBUG
      if (last_final_id != 0)
	sxtrap (ME, "sxdfa_seek");
#endif /* EBUG */

      bot_trans = sxdfa_ptr->next_state_list [bot>>1];

      return bot_trans < 0 ? -bot_trans : 1; /* OK : valeur retournee >0 et lgth == 0 */
    }

    /* Ici le mot est un prefixe de mots du dico */
    /* On peut retourner plusieurs choses */
    /* le seul mot unique eventuel ou
       le +long prefixe du mot qui est ds le dico */
    /* On choisit le 2eme cas */
    if (final_id) {
      *input_lgth = final_lgth;
      return final_id; /* le + long prefixe du mot est ds le dico (final_id), le suffixe est de longueur final_lgth */
    }

    return 0; /* le mot est un prefixe de mots (non identifies) du dico */
  }

  /* Ici le mot n'a pas ete reconnu en entier */
  if (state == 0 /* etat final feuille */ || (sxdfa_ptr->states [state] & 1) /* etat final "interne" */) {
    /* Il y a un mot du dico qui est un prefixe du mot d'entree */
    return final_id ? -final_id : -1; /* le mot est un prefixe du mot -final_id du dico (il reste lgth char a reconnaitre ds -final_id) */
  }
    
  if (final_id) {
    *input_lgth = final_lgth;
    return final_id; /* le + long prefixe du mot est ds le dico (final_id), le suffixe est de longueur final_lgth */
  }

  return 0; /* Echec total, le suffixe est de longueur lgth */
}




/*  ***********************************************  COMB_VECTOR  ****************************************************************** */
static SXINT base_comb_vector, max_class_value;


/* retourne l'index ou est installe' comb
   et modifie comb_vector en consequence */
static SXINT
install_a_comb (SXBA *comb_vector_ptr, SXBA comb, SXBA *base_pos_set_ptr)
{
  SXINT  base_value;
  SXUINT comb_size, comb_vector_size, first_comb_elem, first_comb_word_index, free_comb_vector_elem, free_comb_vector_word_index, right_shift, left_shift, cword, cvword;
  SXUINT *last_comb_word, *first_comb_word, *free_comb_vector_word, *comb_word, *comb_vector_word;
  SXBA   comb_vector, base_pos_set;
  int    kind;

  sxinitialise (left_shift);
  sxinitialise (right_shift);

  comb_vector = *comb_vector_ptr;
  base_pos_set = *base_pos_set_ptr;

  comb_size = BASIZE (comb);
  last_comb_word = comb + NBLONGS (comb_size);
  comb_vector_size = BASIZE (comb_vector);

  first_comb_elem = sxba_scan (comb, -1);
  first_comb_word = comb + DIV (first_comb_elem) + 1; /* first_comb_elem se trouve ds ce mot ... */
  first_comb_word_index = MOD (first_comb_elem); /* ... ds cette position */

  if (base_comb_vector)
    free_comb_vector_elem = base_comb_vector;
  else
    free_comb_vector_elem = first_comb_elem-1;

  for (;;) {
    free_comb_vector_elem = sxba_0_lrscan (comb_vector, free_comb_vector_elem);
    if (free_comb_vector_elem == (SXUINT)(-1)) {
      /* possibilite' franchement improbable de debordement */
      comb_vector_size *= 2;
      *comb_vector_ptr = comb_vector = sxba_resize (comb_vector, comb_vector_size);
      *base_pos_set_ptr = base_pos_set = sxba_resize (base_pos_set, comb_vector_size);
      free_comb_vector_elem = sxba_0_lrscan (comb_vector, free_comb_vector_elem);
    }

    base_value = free_comb_vector_elem-first_comb_elem;

    if (!SXBA_bit_is_set (base_pos_set, base_value)) {
      /* On n'implante pas 2 combs differents sur la meme base_value */
      if (free_comb_vector_elem+comb_size >= comb_vector_size) {
	/* possibilite' de debordement */
	comb_vector_size *= 2;
	*comb_vector_ptr = comb_vector = sxba_resize (comb_vector, comb_vector_size);
	*base_pos_set_ptr = base_pos_set = sxba_resize (base_pos_set, comb_vector_size);
      }

      free_comb_vector_word = comb_vector + DIV (free_comb_vector_elem) + 1; /* first_comb_elem se trouve ds ce mot ... */
      free_comb_vector_word_index = MOD (free_comb_vector_elem); /* ... ds cette position */

      if (first_comb_word_index == free_comb_vector_word_index) {
	kind = 0;
      }
      else {
	if (first_comb_word_index > free_comb_vector_word_index) {
	  kind = 1;
	  right_shift = first_comb_word_index - free_comb_vector_word_index;
	  left_shift = SXBITS_PER_LONG - right_shift;
	}
	else {
	  kind = -1;
	  left_shift = free_comb_vector_word_index - first_comb_word_index;
	  right_shift = SXBITS_PER_LONG - left_shift;
	}
      }

      comb_word = first_comb_word;
      comb_vector_word = free_comb_vector_word;

      while (comb_word <= last_comb_word) {
	if ((cword = *comb_word)) {
	  if ((cvword = *comb_vector_word)) {
	    if (kind == 1) {
	      if ((cword>>right_shift) & cvword)
		/* Echec */
		break;

	      if ((cword = cword<<left_shift) && (cword & comb_vector_word [-1] /* existe toujours si word est non nul */))
		/* Echec */
		break;
	    }
	    else {
	      if (kind == -1) {
		if ((cword<<left_shift) & cvword)
		  /* Echec */
		  break;

		if ((cword>>right_shift) & (comb_vector_word [1] /* existe toujours */))
		  /* Echec */
		  break;
	      }
	      else {
		if (cword & cvword)
		  /* Echec */
		  break;
	      }
	    }
	  }
	  else {
	    if (kind) {
	      if (kind == 1) {
		if ((cword = cword<<left_shift) && (cword & comb_vector_word [-1] /* existe toujours si word est non nul */))
		  /* Echec */
		  break;
	      }
	      else {
		if ((cword>>right_shift) & (comb_vector_word [1] /* existe toujours */))
		  /* Echec */
		  break;
	      }
	    }
	  }
	}

	comb_word++;
	comb_vector_word++;
      }

      if (comb_word > last_comb_word) {
	/* Ca a marche' */
	comb_word = first_comb_word;
	comb_vector_word = free_comb_vector_word;

	while (comb_word <= last_comb_word) {
	  if ((cword = *comb_word++)) {
	    if (kind == 1) {
	      *comb_vector_word |= cword>>right_shift;

	      if ((cword = cword<<left_shift))
		comb_vector_word [-1] |= cword;
	    }
	    else {
	      if (kind == -1) {
		*comb_vector_word |= cword<<left_shift;

		if ((cword = cword>>right_shift))
		  comb_vector_word [1] |= cword;
	      }
	      else {
		*comb_vector_word |= cword;
	      }
	    }
	  }

	  comb_vector_word++;
	}

	SXBA_1_bit (base_pos_set, base_value);

	return  base_value;
      }
    }
  }
}

#if 0
/* static a cause de cyclic_class_q_check */
static struct classXq2attr {
  SXINT card, next, prev, working_nb, working_next; 
} *classXq2attr;

#if EBUG
static void
classXq_hd_oflw (SXINT old_size, SXINT new_size)
{
  sxtrap (ME, "classXq_hd_oflw");
#if 0
  classXq2attr = (struct classXq2attr *) sxrealloc (classXq2attr, new_size+1, sizeof (struct classXq2attr));
  classXq2p_set = sxbm_resize (classXq2p_set, old_size+1, new_size+1, sxdfa_last_state+1);
#endif /* 0 */
}
#endif /* EBUG */
#endif /* 0 */

void
sxdfa2comb_vector (struct sxdfa_struct *sxdfa_ptr, SXINT optim_kind, SXINT comb_vector_threshold, struct sxdfa_comb *sxdfa_comb_ptr)
{
  SXINT         state, base_pos, size, max_base_pos, t, class, tooth, next_state, is_final_state, bot, bot_trans, base_shift, stop_mask;
  SXINT         *state_ptr, *next_states_ptr, *top_next_states_ptr, *trans_list_ptr;
  SXINT         *state2base_pos;
  SXUINT        *comb_vector_ptr;
  unsigned char *char2class;
  SXBA          comb, comb_vector, base_pos_set;

  if (sxdfa_ptr->max_t_val >= 256)
    sxtrap (ME, "sxdfa2comb_vector");

  /* On transforme les caracteres en classe */
  char2class = (unsigned char*) sxcalloc (256, sizeof (unsigned char));

  {
    SXBA  char_set;
    SXINT *cur_trans_list_ptr;


    char_set = sxba_calloc (256);
    trans_list_ptr = sxdfa_ptr->trans_list;
    cur_trans_list_ptr = trans_list_ptr + *trans_list_ptr;

    while (cur_trans_list_ptr > trans_list_ptr) {
      t = *cur_trans_list_ptr--;
      SXBA_1_bit (char_set, t);
    }

#if 0
    if (optim_kind == 0)
      max_class_value = 1; /* Les classes 0 et 1 sont reservees (pour y mettre des id eventuels) */
    else
#endif /* 0 */
      max_class_value = 2; /* Les classes 0 et 1 sont reservees (pour y mettre des id eventuels)  et la classe 2 pour la partie propre */

    t = -1;

    while ((t = sxba_scan (char_set, t)) >= 0) {
      /* l'ordre lexicographique est pre'serve' sur les classes */
      char2class [t] = ++max_class_value;
    }

    sxfree (char_set);
  }

  /* On remplit sxdfa_comb_ptr */
  {
    SXINT k;

    k = sxlast_bit (max_class_value);

    base_shift = sxdfa_comb_ptr->base_shift = k + 1 /* stop_mask */;
    stop_mask = sxdfa_comb_ptr->stop_mask = 1 << k;
    sxdfa_comb_ptr->class_mask = stop_mask-1;
  }

  sxdfa_comb_ptr->is_static = FALSE;
  sxdfa_comb_ptr->is_a_dag = sxdfa_ptr->is_a_dag ;
  sxdfa_comb_ptr->private.from_left_to_right = sxdfa_ptr->private.from_left_to_right ;
  sxdfa_comb_ptr->char2class = char2class;

  comb = sxba_calloc (max_class_value+1);

  size = sxdfa_ptr->max_arity*sxdfa_ptr->last_state + max_class_value + 1;
  comb_vector = sxba_calloc (size);
  base_comb_vector = 0;
  base_pos_set = sxba_calloc (size);

  max_base_pos = 0;
  state2base_pos = (SXINT *) sxalloc (sxdfa_ptr->last_state+1, sizeof (SXINT)), state2base_pos [0] = 0;

  switch (optim_kind) {
  case 0 :
    /* Essai (tres) simple chaque etat est pris ds l'ordre et est implante sur des bases differentes */
    for (state = sxdfa_ptr->init_state; state <= sxdfa_ptr->last_state; state++) {
      if ((state % comb_vector_threshold) == 0) {
	/* Tous les threshold, on repart comme si comb_vector etait vide */
	base_comb_vector = BASIZE (comb_vector);
	base_comb_vector = sxba_1_rlscan (comb_vector, base_comb_vector);
      }
  
      state_ptr = sxdfa_ptr->states + state;

      bot = state_ptr [0];
      is_final_state = bot & 1;

      next_states_ptr = sxdfa_ptr->next_state_list + (bot>>1);
      top_next_states_ptr = sxdfa_ptr->next_state_list + (state_ptr [1]>>1);
      
      bot_trans = *next_states_ptr++;

      if (bot_trans < 0) {
	if (is_final_state) {
	  /* state est final et il lui est associe' l'id -bot_trans */
	  SXBA_1_bit (comb, 0); /* On reserve donc la classe 0 */
	  bot_trans = *next_states_ptr++;

	  if (bot_trans < 0) {
	    /* -bot_trans est un id remonte' */
	    SXBA_1_bit (comb, 1); /* On reserve donc la classe 1 */
	    bot_trans = *next_states_ptr++;
	  }
	}
	else {
	  /* -bot_trans est un id remonte' */
	  SXBA_1_bit (comb, 0); /* On reserve donc la classe 0 */
	  bot_trans = *next_states_ptr++;
	}
      }
      else {
	if (is_final_state)
	  /* state est final  */
	  SXBA_1_bit (comb, 0); /* On reserve donc la classe 0 */
      }

      trans_list_ptr = sxdfa_ptr->trans_list + bot_trans;

      while (next_states_ptr++ < top_next_states_ptr) {
	t = *trans_list_ptr++;
	class = (SXINT) char2class [t];
	SXBA_1_bit (comb, class);
      }

      state2base_pos [state] = base_pos = install_a_comb (&comb_vector, comb, &base_pos_set);
      sxba_empty (comb);

      if (base_pos > max_base_pos)
	max_base_pos = base_pos;
    }

    sxdfa_comb_ptr->max = max_base_pos+max_class_value;
    /* cas particulier si l'etat initial est aussi un etat final (la chaine vide est ds le dico) */

    sxdfa_comb_ptr->init_base = 0;
    
    sxdfa_comb_ptr->comb_vector = (SXUINT *) sxcalloc (sxdfa_comb_ptr->max+1, sizeof (SXUINT));

    for (state = sxdfa_ptr->init_state; state <= sxdfa_ptr->last_state; state++) {
      state_ptr = sxdfa_ptr->states + state;
      base_pos = state2base_pos [state];
      comb_vector_ptr = sxdfa_comb_ptr->comb_vector+base_pos;

      bot = state_ptr [0];
      is_final_state = bot & 1;

      next_states_ptr = sxdfa_ptr->next_state_list + (bot>>1);
      top_next_states_ptr = sxdfa_ptr->next_state_list + (state_ptr [1]>>1);
      
      bot_trans = *next_states_ptr++;

      if (bot_trans < 0) {
	if (is_final_state) {
	  /* state est final et il lui est associe' l'id -bot_trans */
#if EBUG
	  if (comb_vector_ptr [0] != 0)
	    sxtrap (ME, "sxdfa2comb_vector");
#endif /* EBUG */

	  comb_vector_ptr [0] = ((-bot_trans) << base_shift) + stop_mask;

	  bot_trans = *next_states_ptr++;

	  if (bot_trans < 0) {
	    /* -bot_trans est un id remonte' */
#if EBUG
	    if (comb_vector_ptr [1] != 0)
	      sxtrap (ME, "sxdfa2comb_vector");
#endif /* EBUG */

	    comb_vector_ptr [1] = ((-bot_trans) << base_shift) + 1 /* classe reservee */;
	    bot_trans = *next_states_ptr++;
	  }
	}
	else {
	  /* -bot_trans est un id remonte' */
#if EBUG
	  if (comb_vector_ptr [0] != 0)
	    sxtrap (ME, "sxdfa2comb_vector");
#endif /* EBUG */

	  comb_vector_ptr [0] = ((-bot_trans) << base_shift) /* o classe reservee */;
	  bot_trans = *next_states_ptr++;
	}
      }
      else {
	if (is_final_state)
	  comb_vector_ptr [0] = (1 << base_shift) + stop_mask;
      }

      /* On implante les vraies transitions */
      trans_list_ptr = sxdfa_ptr->trans_list + bot_trans;

      while (next_states_ptr < top_next_states_ptr) {
	next_state = *next_states_ptr++;
	t = *trans_list_ptr++;
	class = (SXINT) char2class [t];

	if (next_state <= 0) {
	  /* final */
	  tooth = stop_mask;

	  if (next_state < 0)
	    tooth |= (-next_state) << base_shift;
	}
	else {
	  tooth = state2base_pos [next_state] << base_shift;

#if 0
	  if (sxdfa_ptr->states [next_state] & 1)
	    /* Next-state est final, on le dit aussi sur l'appelant */
	    tooth |= stop_mask;
#endif /*0 */
	}


#if EBUG
	if (comb_vector_ptr [class] != 0)
	  sxtrap (ME, "sxdfa2comb_vector");
#endif /* EBUG */

	comb_vector_ptr [class] = tooth + class;
      }
    }

#if EBUG
    {
      SXINT nb;

      nb = sxba_cardinal (comb_vector);

      printf ("// comb_vector : size = %ld, used = %ld (%ld%%), size (bytes) = %ld\n", sxdfa_comb_ptr->max, nb, (100*nb)/sxdfa_comb_ptr->max,
	      sizeof (unsigned char)*(256) + sizeof (SXINT)*(sxdfa_comb_ptr->max+1));
    }
#endif /* EBUG */

    break;

  case 1:
    /* Partage des transitions communes entre plusieurs etats */
    {
      SXINT             top, trans_id_nb, trans_id, pos, min_class_value, nb, x, common_base_pos, gain, gain_total, class_set_card;
      SXINT             *pos2trans_id, *trans_id2state_list_hd, *state2next_state_with_same_trans_id, *ptr, *ptr2, *state_stack, *state_nb_bag, *state_nb_bag2, *save_next_states_ptr, *common_class2next_state;
      SXINT             *save_trans_list_ptr;
      SXBA              start_pos_set, common_comb, *class2next_state_set, *common_class2next_state_set, class_set, next_state_set;
      struct class_attr {
	SXINT *state2nb;
      }                 *class2attr, *attr_ptr, *common_class2attr;
      BOOLEAN           now;
      struct {
	SXINT nb, next_state, class;
      } popular, second_popular, void_popular;

      /* Les classes 0,1 et 2 sont reservees */
      min_class_value = 3;

      /* On trie les etats en clusters.  Chaque etat d'un cluster a le meme transition id */

      /* On commence par marquer ou commence chaque trans_id */
      top = sxdfa_ptr->trans_list [0];
      start_pos_set = sxba_calloc (top+1);

      trans_id_nb = 0;

      for (state = sxdfa_ptr->init_state; state <= sxdfa_ptr->last_state; state++) {
	state_ptr = sxdfa_ptr->states + state;

	bot = state_ptr [0];
	is_final_state = bot & 1;

	next_states_ptr = sxdfa_ptr->next_state_list + (bot>>1);
      
	pos = *next_states_ptr++;

	if (pos < 0) {
	  if (is_final_state) {
	    pos = *next_states_ptr++;

	    if (pos < 0) {
	      pos = *next_states_ptr++;
	    }
	  }
	  else {
	    pos = *next_states_ptr++;
	  }
	}

	if (SXBA_bit_is_reset_set (start_pos_set, pos))
	  trans_id_nb++;
      }

      pos2trans_id = (SXINT *) sxalloc (top+1, sizeof (SXINT)), pos2trans_id [0] = 0;

      trans_id2state_list_hd = (SXINT *) sxcalloc (trans_id_nb+2, sizeof (SXINT)), trans_id2state_list_hd [0] = 0;
      state2next_state_with_same_trans_id = (SXINT *) sxalloc (sxdfa_ptr->last_state+1, sizeof (SXINT)), state2next_state_with_same_trans_id [0] = 0;

      state_stack = (SXINT *) sxalloc (sxdfa_ptr->last_state+1, sizeof (SXINT)), state_stack [0] = 0;
      common_comb = sxba_calloc (max_class_value+1);
      class2next_state_set = sxbm_calloc (max_class_value+1, sxdfa_ptr->last_state+1);
      common_class2next_state_set = sxbm_calloc (max_class_value+1, sxdfa_ptr->last_state+1);
      common_class2next_state = (SXINT *) sxalloc (max_class_value+1, sizeof (SXINT));
      class_set = sxba_calloc (max_class_value+1);

      pos = 0;

      for (trans_id = 1; trans_id <= trans_id_nb; trans_id++) {
	pos = sxba_scan (start_pos_set, pos);
	pos2trans_id [pos] = trans_id;
      }

      sxfree (start_pos_set);

      for (state = sxdfa_ptr->init_state; state <= sxdfa_ptr->last_state; state++) {
	state_ptr = sxdfa_ptr->states + state;
	bot = state_ptr [0];
	is_final_state = bot & 1;
	next_states_ptr = sxdfa_ptr->next_state_list + (bot>>1);
	pos = *next_states_ptr++;

	if (pos < 0) {
	  if (is_final_state) {
	    pos = *next_states_ptr++;

	    if (pos < 0) {
	      pos = *next_states_ptr++;
	    }
	  }
	  else {
	    pos = *next_states_ptr++;
	  }
	}

	trans_id = pos2trans_id [pos];
	state2next_state_with_same_trans_id [state] = trans_id2state_list_hd [trans_id];
	trans_id2state_list_hd [trans_id] = state;
      }

      /* Ici pour chaque trans_id state = trans_id2state_list_hd [trans_id] et state2next_state_with_same_trans_id [state] contiennent
	 la liste des state ayant meme trans_id */
      /* On implante chacun de ces clusters l'un apres l'autre */
      class2attr = (struct class_attr*) sxcalloc (max_class_value+1, sizeof (struct class_attr));
      common_class2attr = (struct class_attr*) sxcalloc (max_class_value+1, sizeof (struct class_attr));
      ptr = state_nb_bag = (SXINT*) sxcalloc (max_class_value*(sxdfa_ptr->last_state+1)+1, sizeof (SXINT));
      ptr2 = state_nb_bag2 = (SXINT*) sxcalloc (max_class_value*(sxdfa_ptr->last_state+1)+1, sizeof (SXINT));

      for (class = min_class_value; class <= max_class_value; class++) {
	class2attr [class].state2nb = ptr;
	common_class2attr [class].state2nb = ptr2;
	ptr += sxdfa_ptr->last_state+1;
	ptr2 += sxdfa_ptr->last_state+1;
      }

      common_comb = sxba_calloc (max_class_value+1);

      /* Attention, ds le cas 2, sxdfa_comb_ptr->comb_vector peut deborder */
      sxdfa_comb_ptr->comb_vector = (SXUINT *) sxcalloc (size+1, sizeof (SXUINT));
      gain_total = 0;

      for (trans_id = 1;  trans_id <= trans_id_nb; trans_id++) {
	sxba_empty (class_set), class_set_card = 0;

	for (state = trans_id2state_list_hd [trans_id]; state != 0; state = state2next_state_with_same_trans_id [state]) {
	  state_ptr = sxdfa_ptr->states + state;

	  PUSH (state_stack, state);

	  bot = state_ptr [0];
	  is_final_state = bot & 1;

	  next_states_ptr = sxdfa_ptr->next_state_list + (bot>>1);
	  top_next_states_ptr = sxdfa_ptr->next_state_list + (state_ptr [1]>>1);
      
	  bot_trans = *next_states_ptr++;

	  if (bot_trans < 0) {
	    if (is_final_state) {
	      bot_trans = *next_states_ptr++;

	      if (bot_trans < 0) {
		bot_trans = *next_states_ptr++;
	      }
	    }
	    else {
	      bot_trans = *next_states_ptr++;
	    }
	  }

	  trans_list_ptr = sxdfa_ptr->trans_list + bot_trans;

	  while (next_states_ptr < top_next_states_ptr) {
	    next_state = *next_states_ptr++;
	    t = *trans_list_ptr++;
	    class = (SXINT) char2class [t];

	    if (SXBA_bit_is_reset_set (class_set, class))
	      class_set_card++;

	    if (next_state >= 0) {
	      attr_ptr = class2attr+class;
	      nb = ++(attr_ptr->state2nb [next_state]);

	      if (nb == 1)
		SXBA_1_bit (class2next_state_set [class], next_state);
	    }
	  }
	}

	void_popular.nb = void_popular.next_state = void_popular.class = 0;

	for (;;) {
	  top = TOP (state_stack);

	  if (top == 0)
	    break;

	  TOP (state_stack) = 0;

	  if (top > 1 && class_set_card > 1) {
	    /* On calcule les max pour chaque class */
	    /* ... et le max des max ds popular */
	    popular = void_popular;
	    class = 0;

	    while ((class = sxba_scan (class_set, class)) > 0) {
	      attr_ptr = class2attr+class;
	      next_state_set = class2next_state_set [class];

	      next_state = -1;

	      while ((next_state = sxba_scan (next_state_set, next_state)) >= 0) {
		nb = attr_ptr->state2nb [next_state];

		if (nb > popular.nb) {
		  popular.nb = nb;
		  popular.next_state = next_state;
		  popular.class = class;
		}
	      }
	    }

	    if (popular.nb > 1) {
	      second_popular.nb = 0;

	      class2attr [popular.class].state2nb [popular.next_state] = 0;
	      SXBA_0_bit (class2next_state_set [popular.class], popular.next_state);

	      /* On selectionne les etats qui ont une transition sur popular */
	      /* ... et parmi ceux-la on fabrique second_popular */
	      for (x = 1; x <= top; x++) {
		/* On installe les partie propres de chaque state */
		state = state_stack [x];
		state_ptr = sxdfa_ptr->states + state;

		bot = state_ptr [0];
		is_final_state = bot & 1;

		next_states_ptr = sxdfa_ptr->next_state_list + (bot>>1);
		top_next_states_ptr = sxdfa_ptr->next_state_list + (state_ptr [1]>>1);
      
		bot_trans = *next_states_ptr++;

		if (bot_trans < 0) {
		  if (is_final_state) {
		    bot_trans = *next_states_ptr++;

		    if (bot_trans < 0) {
		      bot_trans = *next_states_ptr++;
		    }
		  }
		  else {
		    bot_trans = *next_states_ptr++;
		  }
		}

		save_trans_list_ptr = trans_list_ptr = sxdfa_ptr->trans_list + bot_trans;
		save_next_states_ptr = next_states_ptr;

		while (next_states_ptr < top_next_states_ptr) {
		  next_state = *next_states_ptr++;
		  t = *trans_list_ptr++;
		  class = (SXINT) char2class [t];

		  if (class == popular.class && next_state == popular.next_state) {
		    nb = ++common_class2attr [class].state2nb [next_state];

		    if (nb == 1)
		      SXBA_1_bit (common_class2next_state_set [class], next_state);

		    while (save_next_states_ptr < top_next_states_ptr) {
		      next_state = *save_next_states_ptr++;
		      t = *save_trans_list_ptr++;
		      class = (SXINT) char2class [t];

		      if (class != popular.class && next_state >= 0) {
			nb = ++common_class2attr [class].state2nb [next_state];

			if (nb == 1)
			  SXBA_1_bit (common_class2next_state_set [class], next_state);
		      
			if (nb > second_popular.nb) {
			  second_popular.nb = nb;
			  second_popular.next_state = next_state;
			  second_popular.class = class;
			}
		      }
		    }

		    break;
		  }
		}
	      }

	      /* Le "gain" d'une utilisation de popular.next_state est -1 */
	      /* Le "gain" d'une utilisation de second_popular.next_state est second_popular.nb-1 */
	      if (second_popular.nb-2 > 0) {
		/* On fait un common_comb */
		sxba_empty (common_comb);
		class = 0;
		gain = 0;

		while ((class = sxba_scan (class_set, class)) > 0) {
		  attr_ptr = common_class2attr+class;
		  next_state_set = common_class2next_state_set [class];

		  next_state = -1;

		  while ((next_state = sxba_scan_reset (next_state_set, next_state)) >= 0) {
		    nb = attr_ptr->state2nb [next_state];

		    if (nb > 1) {
		      SXBA_1_bit (common_comb, class);
		      common_class2next_state [class] = next_state;
		      gain += nb-1;
		    }

		    attr_ptr->state2nb [next_state] = 0;
		  }
		}

		gain -= popular.nb;

#if EBUG
		if (gain <= 0)
		  sxtrap (ME, "sxdfa2comb_vector");
#endif /* EBUG */

		gain_total += gain;

		/* On installe common_comb */
		common_base_pos = install_a_comb (&comb_vector, common_comb, &base_pos_set);

		if (common_base_pos > max_base_pos) {
		  max_base_pos = common_base_pos;
	  
		  if (max_base_pos+max_class_value > size) {
		    sxdfa_comb_ptr->comb_vector = (SXUINT *) sxrecalloc (sxdfa_comb_ptr->comb_vector, size+1, 2*size+1, sizeof (SXUINT));
		    size *= 2;
		  }
		}
	    
		comb_vector_ptr = sxdfa_comb_ptr->comb_vector+common_base_pos;
		class = 0;

		while ((class = sxba_scan (common_comb, class)) > 0) {
#if EBUG
		  if (comb_vector_ptr [class] != 0)
		    sxtrap (ME, "sxdfa2comb_vector");
#endif /* EBUG */

		  next_state = common_class2next_state [class];

		  if (next_state < 0) {
		    /* Normalement on ne passe jamais par la car un id (negatif) ne peut jamais etre le + populaire */
		    /* final */
		    tooth = ((-next_state) << base_shift) + stop_mask;
		  }
		  else {
		    if (next_state == 0) {
		      /* final */
		      tooth = stop_mask;
		    }
		    else {
		      tooth = next_state << base_shift;
		    }
		  }

		  comb_vector_ptr [class] = tooth + class;
		}
	      }
	      else {
		common_base_pos = -1;
		/* Tout le (reste) du cluster est implante' ds propre */
		/* Il faut raze' les structures precedentes */
		class = 0;

		while ((class = sxba_scan (class_set, class)) > 0) {
		  attr_ptr = common_class2attr+class;
		  next_state_set = common_class2next_state_set [class];

		  next_state = -1;

		  while ((next_state = sxba_scan_reset (next_state_set, next_state)) >= 0) {
		    attr_ptr->state2nb [next_state] = 0;
		  }
		}
	      }
	    }
	    else
	      common_base_pos = -1;
	  }
	  else
	    common_base_pos = -1;
	  
	  for (x = 1; x <= top; x++) {
	    /* On installe les partie propres de chaque state */
	    state = state_stack [x];
	    state_ptr = sxdfa_ptr->states + state;

	    bot = state_ptr [0];
	    is_final_state = bot & 1;

	    next_states_ptr = sxdfa_ptr->next_state_list + (bot>>1);
	    top_next_states_ptr = sxdfa_ptr->next_state_list + (state_ptr [1]>>1);
      
	    bot_trans = *next_states_ptr++;

	    if (bot_trans < 0) {
	      if (is_final_state) {
		SXBA_1_bit (comb, 0); /* On reserve donc la classe 0 */
		bot_trans = *next_states_ptr++;

		if (bot_trans < 0) {
		  SXBA_1_bit (comb, 1); /* On reserve donc la classe 1 */
		  bot_trans = *next_states_ptr++;
		}
	      }
	      else {
		SXBA_1_bit (comb, 0); /* On reserve donc la classe 0 */
		bot_trans = *next_states_ptr++;
	      }
	    }
	    else {
	      if (is_final_state)
		/* state est final  */
		SXBA_1_bit (comb, 0); /* On reserve donc la classe 0 */
	    }

	    trans_list_ptr = sxdfa_ptr->trans_list + bot_trans;
	    now = common_base_pos == -1;

	    while (next_states_ptr < top_next_states_ptr) {
	      next_state = *next_states_ptr++;
	      t = *trans_list_ptr++;
	      class = (SXINT) char2class [t];

	      if (common_base_pos == -1) {
		/* Pas de partie commune */
		SXBA_1_bit (comb, class);
	      }
	      else {
		if (next_state >= 0) {
		  if (SXBA_bit_is_set (common_comb, class) && common_class2next_state [class] == next_state)
		    /* Ds la partie commune */
		    now = TRUE;
		  else {
		    /* en propre */
		    SXBA_1_bit (comb, class);
		  }
		}
		else
		  /* Obligatoirement en propre */
		  SXBA_1_bit (comb, class);
	      }
	    }

	    if (now) {
	      /* state doit etre implante' maintenant, il partage des parties communes avec common_comb */
	      /* On doit aussi le faire disparaitre de class2attr */
	      if (common_base_pos >= 0)
		SXBA_1_bit (comb, 2); /* ref vers la partie commune */

	      base_pos = state2base_pos [state] = install_a_comb (&comb_vector, comb, &base_pos_set);

	      if (state == sxdfa_ptr->init_state)
		/* La reconnaissance va commencer la */
		sxdfa_comb_ptr->init_base = base_pos << base_shift;

	      if (base_pos > max_base_pos) {
		max_base_pos = base_pos;
	  
		if (max_base_pos+max_class_value > size) {
		  sxdfa_comb_ptr->comb_vector = (SXUINT *) sxrecalloc (sxdfa_comb_ptr->comb_vector, size+1, 2*size+1, sizeof (SXUINT));
		  size *= 2;
		}
	      }

	      /* On remplit effectivement sxdfa_comb_ptr->comb_vector */

	      comb_vector_ptr = sxdfa_comb_ptr->comb_vector+base_pos;
	      next_states_ptr = sxdfa_ptr->next_state_list + (bot>>1);

	      bot_trans = *next_states_ptr++;

	      if (bot_trans < 0) {
		if (is_final_state) {
		  /* state est final et il lui est associe' l'id -bot_trans */
#if EBUG
		  if (comb_vector_ptr [0] != 0)
		    sxtrap (ME, "sxdfa2comb_vector");
#endif /* EBUG */

		  comb_vector_ptr [0] = ((-bot_trans) << base_shift) + stop_mask;

		  bot_trans = *next_states_ptr++;

		  if (bot_trans < 0) {
		    /* -bot_trans est un id remonte' */
#if EBUG
		    if (comb_vector_ptr [1] != 0)
		      sxtrap (ME, "sxdfa2comb_vector");
#endif /* EBUG */

		    comb_vector_ptr [1] = ((-bot_trans) << base_shift) + 1 /* classe reservee */;
		    bot_trans = *next_states_ptr++;
		  }
		}
		else {
		  /* -bot_trans est un id remonte' */
#if EBUG
		  if (comb_vector_ptr [0] != 0)
		    sxtrap (ME, "sxdfa2comb_vector");
#endif /* EBUG */

		  comb_vector_ptr [0] = ((-bot_trans) << base_shift) /* o classe reservee */;
		  bot_trans = *next_states_ptr++;
		}
	      }
	      else {
		if (is_final_state)
		  comb_vector_ptr [0] = (1 << base_shift) + stop_mask;
	      }

	      if (common_base_pos >= 0)
		comb_vector_ptr [2] = (common_base_pos << base_shift) + 2 /* 2 classe reservee pour la partie commune */;

	      /* On implante les vraies transitions */
	      trans_list_ptr = sxdfa_ptr->trans_list + bot_trans;

	      while (next_states_ptr < top_next_states_ptr) {
		next_state = *next_states_ptr++;
		t = *trans_list_ptr++;
		class = (SXINT) char2class [t];

		if (next_state < 0) {
		  /* final */
		  tooth = ((-next_state) << base_shift) + stop_mask;
		}
		else {
		  if (common_base_pos >= 0 /* Y'a une partie commune */
		      && SXBA_bit_is_set (common_comb, class) /* et class est ds cette partie commune */
		      && common_class2next_state [class] == next_state /* et transition vers le bon etat */)
		    /* Implante' ds la partie commune => disponible pour qcqu'un d'autre */
		    tooth = 0;
		  else {
		    /* Implante' ds la parie propre */
		    if (next_state == 0)
		      /* etat final */
		      tooth = stop_mask;
		    else
		      tooth = next_state << base_shift; /* state2base_pos n'est pas complet, on laisse next_state et on fera une derniere passe */
		  }

		  nb = --(class2attr [class].state2nb [next_state]);

		  if (nb == 0)
		    SXBA_0_bit (class2next_state_set [class], next_state);
		}

#if EBUG
		if (tooth && comb_vector_ptr [class] != 0)
		  sxtrap (ME, "sxdfa2comb_vector");
#endif /* EBUG */

		if (tooth)
		  comb_vector_ptr [class] = tooth + class;
	      }
	    }
	    else {
	      /* On est sur un etat qui n'a rien a voir avec le common_comb courant, on le reserve donc pour le passage suivant */
	      PUSH (state_stack, state);
	    }
	    
	    sxba_empty (comb);
	  }
	}
      }

      sxdfa_comb_ptr->max = max_base_pos+max_class_value;

      /* Derniere passe pour mettre a jour les "next_state" */
      for (pos = sxdfa_comb_ptr->max; pos >= 0; pos--) {
	comb_vector_ptr = sxdfa_comb_ptr->comb_vector+pos;
	tooth = *comb_vector_ptr;

	if ((tooth & stop_mask) == 0 && (class = (tooth & sxdfa_comb_ptr->class_mask)) >= min_class_value /* vraie classe */) {
	  base_pos = state2base_pos [tooth >>= base_shift];
	  *comb_vector_ptr = (base_pos << base_shift) + class;
	}
      }


#if EBUG
      nb = sxba_cardinal (comb_vector);

      printf ("// comb_vector : size = %ld, used = %ld (%ld%%), size (bytes) = %ld, gain_total (bytes) = %ld\n", sxdfa_comb_ptr->max, nb, (100*nb)/sxdfa_comb_ptr->max,
	      sizeof (struct sxdfa_comb)
	      + sizeof (unsigned char) * 256
	      + sizeof (SXINT) * (sxdfa_comb_ptr->max+1),
	      gain_total*sizeof (SXINT));
#endif /* EBUG */

      sxfree (pos2trans_id);
      sxfree (trans_id2state_list_hd);
      sxfree (state2next_state_with_same_trans_id);
      sxfree (state_stack);
      sxfree (common_comb);
      sxbm_free (class2next_state_set);
      sxbm_free (common_class2next_state_set);
      sxfree (common_class2next_state);
      sxfree (class_set);
      sxfree (class2attr);
      sxfree (common_class2attr);
      sxfree (state_nb_bag);
      sxfree (state_nb_bag2);
    }

    break;

  default:
    sxtrap (ME, "sxdfa2comb_vector");
    break;
  }

  sxfree (comb_vector);
  sxfree (comb);
  sxfree (state2base_pos);
}


void
sxdfa_comb2c (struct sxdfa_comb *sxdfa_comb_ptr, FILE *file, char *dico_name, BOOLEAN is_static)
{
  unsigned char	class;
  SXUINT i;

  fprintf (file, "\nstatic SXUINT %s_comb_vector [%ld] = {", dico_name == NULL ? "" : dico_name, (long)sxdfa_comb_ptr->max+1);

  for (i = 0; i <= sxdfa_comb_ptr->max; i++) {
    if ((i % 10) == 0)
      fprintf (file, "\n/* %ld */ ", i);

    fprintf (file, "%ld, ", (long)sxdfa_comb_ptr->comb_vector [i]);
  }

  fprintf (file, "\n};\nstatic unsigned char %s_char2class [256] = {", dico_name == NULL ? "" : dico_name);

  for (i = 0; i <= 255; i++) {
    if ((i % 10) == 0)
      fprintf (file, "\n/* %ld */ ", i);

    class = sxdfa_comb_ptr->char2class [i];
    fputs ("\'", file);

    if (class == '\'')
      fputs ("\\'", file);
    else
      fprintf (file, "%s", SXCHAR_TO_STRING (class));

    fputs ("\', ", file);
  }

  fprintf (file, "\n};\n%sstruct sxdfa_comb %s = {\n", is_static ? "static " : "", dico_name == NULL ? "sxdfa_comb_dico_hd" : dico_name);
  fprintf (file, "%ld /* max */, %ld /* init_base */, %ld /* class_mask */, %ld /* stop_mask */, %ld /* base_shift */,\n",
	  (long)sxdfa_comb_ptr->max, (long)sxdfa_comb_ptr->init_base, (long)sxdfa_comb_ptr->class_mask, (long)sxdfa_comb_ptr->stop_mask, (long)sxdfa_comb_ptr->base_shift);
	    
  fprintf (file, "%s_char2class, %s_comb_vector,\n", dico_name == NULL ? "" : dico_name, dico_name == NULL ? "" : dico_name);
  fprintf (file, "1 /* is_static */, %i /* is_a_dag */,{%i /* from_left_to_right */,}\n};\n", (int)sxdfa_comb_ptr->is_a_dag, (int)sxdfa_comb_ptr->private.from_left_to_right);
}


SXINT
sxdfa_comb_seek_a_string (struct sxdfa_comb *sxdfa_comb_ptr, char *kw, SXINT *kwl)
{
  sxdfa_comb_ptr->private.ptr.char_ptr = (sxdfa_comb_ptr->private.from_left_to_right) ? kw : kw+*kwl-1;

  return sxdfa_comb_seek (sxdfa_comb_ptr, string_get_next_symb, kwl);
}



SXINT
sxdfa_comb_seek_a_word (struct sxdfa_comb *sxdfa_comb_ptr, SXINT *input_stack)
{
  sxdfa_comb_ptr->private.ptr.SXINT_ptr = (sxdfa_comb_ptr->private.from_left_to_right) ? input_stack + 1 : input_stack + *input_stack;

  return sxdfa_comb_seek (sxdfa_comb_ptr, word_get_next_symb, input_stack);
}


SXINT
sxdfa_comb_seek (struct sxdfa_comb *sxdfa_comb_ptr, SXUINT (*get_next_symb) (struct sxdfa_private *), SXINT *input_lgth)
{
  SXUINT        base, tooth = sxdfa_comb_ptr->init_base;
  SXUINT        base_shift = sxdfa_comb_ptr->base_shift;
  SXUINT        class_mask = sxdfa_comb_ptr->class_mask, stop_mask = sxdfa_comb_ptr->stop_mask;
  SXUINT        *comb_vector = sxdfa_comb_ptr->comb_vector, *comb_vector_ptr;
  unsigned char	class, *char2class = sxdfa_comb_ptr->char2class;
  SXINT		lgth = *input_lgth;
  SXUINT        last_final_id, final_id, final_lgth;

  if (/* tooth == 0 || modif du 07/12/07 (pour nelle version) */ lgth < 0)
    return 0;

  /* base_shift = sxdfa_comb_ptr->base_shift; deja fait !! */

  base = tooth >> base_shift;

  if (sxdfa_comb_ptr->max == 0)
    /* dico vide ne specifie que la chaine vide */
    return lgth == 0 ? base : 0;

  last_final_id = final_id = final_lgth = 0;

  for (;;) {
    /* on est sur une base */
    comb_vector_ptr = comb_vector + base;
    tooth = *comb_vector_ptr;

    if (tooth /* non vide */ && (tooth & class_mask) == 0 /* il est bien à nous ;) */) {
      if (tooth & stop_mask) {
	tooth >>= base_shift;
	
	/* on est sur une base (des transitions en partent) qui est aussi un état final: on n'a pas
	   pu connaître l'id par anticipation, et tooth est bien l'id */
	if (lgth == 0)
	  /* OK */
	  return tooth;

	/* Ici, un prefixe strict de l'entree est un mot du dico */
	/* On note le +long au cas ou */
	final_id = tooth;
	final_lgth = lgth;

	/* On traite la trans suivante qui peut etre l'id du mot unique reconnu par le suffixe */
	tooth = comb_vector_ptr [1];

	if ((tooth & class_mask) == 1) {
	  last_final_id = tooth >> base_shift;
	}
      }
      else {
	if (lgth == 0) {
	  /* Le mot en entree est un prefixe strict du mot d'identifiant tooth du dico ... */
	  break;
	}
	/* on ne devrait pas arriver ici, car ça voudrait dire que la transition-id n'est pas
	   remontée au maximum... on laisse just in case ;) */
	last_final_id = tooth >> base_shift;
      }
    }
    else {
      /* la base tooth ne contient aucune info sur la transition courante car elle n'a rien de
	 particulier (son offset 0 ne lui appartient pas)... on poursuit sagement le parcours... */
      if (lgth == 0) {
	/* ...sauf si le mot est fini: le mot en entree est alors un prefixe strict de mots du dico */
	break;
      }
    }

    class = char2class [(*get_next_symb) (&(sxdfa_comb_ptr->private))];
    tooth = comb_vector_ptr [class];

    if (class == 0) {
       /* Le caractere courant est invalide */ 
      break;
    }

    if ((tooth & class_mask) != class) { /* on est tombé sur un endroit qui n'appartient pas à notre base : */
      /* Pas de transition sur class depuis l'etat courant */
      /* On regarde s'il existe une "partie commune" a l'etat courant (pour -comb 1)*/
      tooth = comb_vector_ptr [2];

      if ((tooth & class_mask) != 2)
	/* ... non (pas -comb 1)*/
	break;

      /* ... oui, partie commune (-comb 1)*/
      /* On y va */
      comb_vector_ptr = comb_vector + (tooth >> base_shift);
      /* Ds la partie commune, on ne peut trouver que des "vraies" transitions */
      tooth = comb_vector_ptr [class];
    
      if ((tooth & class_mask) != class)
	/* echec ds la partie commune */
	break;
    }

    lgth = --*input_lgth; /* transition OK */

    if (tooth & stop_mask) {
      /* on est donc sur un état final qui n'est pas une base: c'est fini */
      if ((tooth >>= base_shift) == 0)
	tooth = (last_final_id) ? last_final_id : 1;
      /* else
	 id = tooth
      */

      /* Si lgth == 0 => OK */
      /* Sinon on a atteint un etat final du dico sans epuiser le mot en entree */
      /* Le mot tooth du dico est donc un prefixe strict du mot en entree */
      return tooth;
    }

    base = tooth >> base_shift;
  }

  /* Echec, mais il y a (peut etre) un mot du dico qui est est un prefixe strict de l'entree
     on choisit de retourner ce mot */
  *input_lgth = final_lgth;
  return final_id;
}

void
sxdfa_comb_free (struct sxdfa_comb *sxdfa_comb_ptr)
{
  if (!sxdfa_comb_ptr->is_static) {
    if (sxdfa_comb_ptr->char2class) sxfree (sxdfa_comb_ptr->char2class), sxdfa_comb_ptr->char2class = NULL;
    if (sxdfa_comb_ptr->comb_vector) sxfree (sxdfa_comb_ptr->comb_vector), sxdfa_comb_ptr->comb_vector = NULL;
  }
}

/* ***************************************** SXDFA_PACKED ************************************************ */
extern void
sxdfa2sxdfa_packed (struct sxdfa_struct *sxdfa_ptr, struct sxdfa_packed_struct *sxdfa_packed_ptr, char *name, FILE *stats)
{

  SXUINT k, base_shift, stop_mask, id_mask, top, t, trans_nb;
  SXUINT *delta_ptr;

  SXINT  state, next_state, bot_trans, bot, is_final;
  SXINT  *state_ptr, *next_states_ptr, *top_next_state_ptr, *trans_list_ptr;


  k = sxlast_bit (sxdfa_ptr->max_t_val);
  base_shift = k + 1 /* stop_mask */ + 1 /* id_mask */;
  top = sxdfa_ptr->next_state_list [0];

  if (((top<<base_shift)>>base_shift) != top) {
    /* oflw */
    /* On change de mode de representation, A FAIRE */
    /* Pour l'instant */
    sxtrap (ME, "sxdfa2sxdfa_packed");
  }

  /* Ici tout tient ds un SXUINT  [id || next] [stop] [taille || t] */
  sxdfa_packed_ptr->base_shift = base_shift;
  sxdfa_packed_ptr->stop_mask = stop_mask = 1 << k;
  sxdfa_packed_ptr->id_mask = id_mask = 1 << (k+1);
  sxdfa_packed_ptr->t_mask = stop_mask-1;

  sxdfa_packed_ptr->init_state = sxdfa_ptr->init_state;
  sxdfa_packed_ptr->last_state = sxdfa_ptr->last_state;
  sxdfa_packed_ptr->number_of_out_trans = sxdfa_ptr->number_of_out_trans;
  sxdfa_packed_ptr->final_state_nb = sxdfa_ptr->final_state_nb;
  sxdfa_packed_ptr->max_path_lgth = sxdfa_ptr->max_path_lgth;
  sxdfa_packed_ptr->max_arity = sxdfa_ptr->max_arity;
  sxdfa_packed_ptr->max_t_val = sxdfa_ptr->max_t_val;
  sxdfa_packed_ptr->name = name;
  sxdfa_packed_ptr->stats = stats;

  sxdfa_packed_ptr->is_static = FALSE;
  sxdfa_packed_ptr->is_a_dag = sxdfa_ptr->is_a_dag;
  sxdfa_packed_ptr->private.from_left_to_right = sxdfa_ptr->private.from_left_to_right;

  sxdfa_packed_ptr->delta = (SXUINT*) sxalloc (top+1, sizeof (SXINT));

  delta_ptr = sxdfa_packed_ptr->delta;

  for (state = sxdfa_ptr->init_state; state <= sxdfa_ptr->last_state; state++) {
    state_ptr = sxdfa_ptr->states + state;
    bot = state_ptr [0];

    is_final = bot & 1;
    bot >>= 1;
  
    next_states_ptr = sxdfa_ptr->next_state_list + bot;
    bot_trans = *next_states_ptr++;
    top_next_state_ptr = sxdfa_ptr->next_state_list + (state_ptr [1]>>1);

    if (bot_trans < 0) {
      if (is_final) {
	/* L'etat state est final et -bot_trans est l'identifiant associe' ... */
	/* ... mais lgth > 0, on n'est donc pas interesse' par cette transition */
	/* On note le +long prefixe du mot en entree reconnu */
	*++delta_ptr = ((-bot_trans)<<base_shift) + id_mask + stop_mask;

	bot_trans = *next_states_ptr++;

	if (bot_trans < 0) {
	  /* ici -bot_trans est l'identifiant associe' au mot unique qui sera reconnu sur
	     le futur etat final */
	  *++delta_ptr = ((-bot_trans)<<base_shift) + id_mask;
	  bot_trans = *next_states_ptr++; /* C'est un trans_id */
	}
      }
      else {
      /* ici -bot_trans est l'identifiant associe' au mot unique qui sera reconnu sur
	 le futur etat final */
	  *++delta_ptr = ((-bot_trans)<<base_shift) + id_mask;
	  bot_trans = *next_states_ptr++; /* C'est un trans_id */
      }

      trans_nb = top_next_state_ptr - next_states_ptr;
      *++delta_ptr = (trans_nb<<base_shift);
    }
    else {
      trans_nb = (top_next_state_ptr - next_states_ptr)<<base_shift;

      if (is_final)
	/* Un prefixe du mot est dans le dico */
	*++delta_ptr = trans_nb + stop_mask;
      else
	*++delta_ptr = trans_nb;
    }

    trans_list_ptr = sxdfa_ptr->trans_list + bot_trans;

    while (next_states_ptr < top_next_state_ptr) {
      t = *trans_list_ptr++;
      next_state = *next_states_ptr++;

      if (next_state <= 0) {
	if (next_state < 0)
	  *++delta_ptr = ((-next_state)<<base_shift) + id_mask + stop_mask + t;
	else
	  *++delta_ptr = stop_mask + t;
      }
      else {
	next_state = sxdfa_ptr->states [next_state]>>1;
	*++delta_ptr = (next_state<<base_shift) + t;
      }
    }
  }

  sxdfa_packed_ptr->delta [0] = delta_ptr - sxdfa_packed_ptr->delta;

#if EBUG
  if (top-1 != sxdfa_packed_ptr->delta [0])
    sxtrap (ME, "sxdfa2sxdfa_packed");
#endif /* EBUG */

  sxdfa_free (sxdfa_ptr);

  if (sxdfa_ptr->trans_list) sxfree (sxdfa_ptr->trans_list), sxdfa_ptr->trans_list = NULL;
}




SXINT
sxdfa_packed_seek_a_string (struct sxdfa_packed_struct *sxdfa_packed_ptr, char *kw, SXINT *kwl)
{
  sxdfa_packed_ptr->private.ptr.char_ptr = (sxdfa_packed_ptr->private.from_left_to_right) ? kw : kw+*kwl-1;

  return sxdfa_packed_seek (sxdfa_packed_ptr, string_get_next_symb, kwl);
}



SXINT
sxdfa_packed_seek_a_word (struct sxdfa_packed_struct *sxdfa_packed_ptr, SXINT *input_stack)
{
  sxdfa_packed_ptr->private.ptr.SXINT_ptr = (SXINT *) ((sxdfa_packed_ptr->private.from_left_to_right) ? input_stack + 1 : input_stack + *input_stack);

  return sxdfa_packed_seek (sxdfa_packed_ptr, word_get_next_symb, input_stack);
}



/*
  <0  => reconnaissance d'un prefixe
  ==0 => echec
  >0  => succes
*/
SXINT
sxdfa_packed_seek (struct sxdfa_packed_struct *sxdfa_packed_ptr, SXUINT (*get_next_symb) (struct sxdfa_private *), SXINT *input_lgth)
{
  SXUINT	lgth, base_shift, stop_mask, t_mask, tooth, trans_nb, id_mask, is_final, last_final_id, final_id, final_lgth, trans, mid_trans;
  SXUINT        *delta, *delta_ptr, *bot_delta_ptr, *top_delta_ptr, *mid_delta_ptr;

  sxinitialise (final_lgth);

  lgth = *input_lgth;
  
  base_shift = sxdfa_packed_ptr->base_shift;
  stop_mask = sxdfa_packed_ptr->stop_mask;
  t_mask = sxdfa_packed_ptr->t_mask;
  id_mask = sxdfa_packed_ptr->id_mask;

  last_final_id = final_id = 0;

  delta = sxdfa_packed_ptr->delta;

  delta_ptr = delta+sxdfa_packed_ptr->init_state;
  tooth = *delta_ptr++;

  while (lgth > 0) {
    is_final = tooth & stop_mask; /* si TRUE : forcement interne */

    if (tooth & id_mask) {
      if (is_final) {
	final_id = tooth>>base_shift;
	final_lgth = lgth;

	tooth = *delta_ptr++;

	if (tooth & id_mask) {
	  last_final_id = tooth>>base_shift;
	  tooth = *delta_ptr++;
	}
      }
      else {
	last_final_id = tooth>>base_shift;
	  tooth = *delta_ptr++;
      }
    }
    else {
      if (is_final) {
	/* Un prefixe du mot est dans le dico */
	final_id = 1;
	final_lgth = lgth;
      }
    }

    trans = (*get_next_symb) (&(sxdfa_packed_ptr->private));

    /* On cherche a quel index se trouve trans dans la "pile" delta_ptr ... */
    /* ... par dicho */
    trans_nb = tooth>>base_shift;
    bot_delta_ptr = delta_ptr;
    top_delta_ptr = delta_ptr+trans_nb;

    for (;;) {
      mid_delta_ptr = bot_delta_ptr+trans_nb/2;
      tooth = *mid_delta_ptr;

      if ((mid_trans = tooth&t_mask) == trans)
	/* OK */
	break;

      if (mid_trans < trans)
	bot_delta_ptr = mid_delta_ptr+1;
      else
	top_delta_ptr = mid_delta_ptr;

      trans_nb = top_delta_ptr-bot_delta_ptr;

      if (trans_nb == 0) {
	tooth = 0;
	break; /* echec */
      }
    }

    if (tooth == 0)
      /* Echec */
      break;

    lgth--;

    if (tooth & stop_mask)
      break;

    delta_ptr = delta+(tooth>>base_shift);
    tooth = *delta_ptr++;
  }

  *input_lgth = lgth;

  if (lgth == 0) {
    /* On a reconnu le mot en entier ... */
    /* ... c'est OK uniquement si l'etat atteint est final */
    if (tooth & stop_mask) {
      if (last_final_id)
	return last_final_id;

      if (tooth & id_mask)
	return tooth>>base_shift;

      return 1; /* OK */
    }

    /* Ici le mot est un prefixe de mots du dico */
    /* On peut retourner plusieurs choses */
    /* le seul mot unique eventuel ou
       le +long prefixe du mot qui est ds le dico */
    /* On choisit le 2eme cas */
    if (final_id) {
      *input_lgth = final_lgth;
      return final_id; /* le + long prefixe du mot est ds le dico (final_id), le suffixe est de longueur final_lgth */
    }

    return 0; /* le mot est un prefixe de mots (non identifies) du dico */
  }

  if (tooth) {
    /* Il y a un mot du dico qui est un prefixe du mot d'entree */
    if (tooth & id_mask)
      return -(tooth>>base_shift);

    if (last_final_id)
      return -last_final_id;

    return -1;
  }

  /* Ici le mot n'a pas ete reconnu en entier */
  if (final_id)
    *input_lgth = final_lgth;
    
  return final_id; /* le + long prefixe du mot est ds le dico (final_id), le suffixe est de longueur final_lgth */
}





void
sxdfa_packed2c (struct sxdfa_packed_struct *sxdfa_packed_ptr, FILE *file, char *name, BOOLEAN is_static)
{
  SXUINT  i;

  fprintf (file, "\nstatic SXINT %s_delta [%ld] = {\n", name, sxdfa_packed_ptr->delta [0]+1);

  for (i = 0; i <= sxdfa_packed_ptr->delta [0]; i++) {
    fprintf (file, "/* %ld */ %ld,\n", i, sxdfa_packed_ptr->delta [i]);
  }

  fprintf (file, "};\n");

  fprintf (file, "\n%sstruct sxdfa_packed_struct %s = {\n", is_static ? "static " : "", name);
  fprintf (file, "/* init_state */ %ld,\n\
/* last_state */ %ld,\n\
/* number_of_out_trans */ %ld,\n\
/* final_state_nb */ %ld,\n\
/* max_path_lgth */ %ld,\n\
/* max_arity */ %ld,\n\
/* max_t_val */ %ld,\n\
/* name */ \"%s\",\n\
/* stats */ NULL,\n\
/* base_shift */ %ld,\n\
/* stop_mask */ %ld,\n\
/* t_mask */ %ld,\n\
/* id_mask */ %ld,\n\
%s_delta,\n\
/* is_static */ TRUE,\n\
/* is_a_dag */ %s,\n\
 {\n\
 /* from_left_to_right */ %s,\
 }\n",
	   sxdfa_packed_ptr->init_state,
	   sxdfa_packed_ptr->last_state,
	   sxdfa_packed_ptr->number_of_out_trans,
	   sxdfa_packed_ptr->final_state_nb,
	   sxdfa_packed_ptr->max_path_lgth,
	   sxdfa_packed_ptr->max_arity,
	   sxdfa_packed_ptr->max_t_val,
	   name,
	   sxdfa_packed_ptr->base_shift,
	   sxdfa_packed_ptr->stop_mask,
	   sxdfa_packed_ptr->t_mask,
	   sxdfa_packed_ptr->id_mask,
	   name,
	   sxdfa_packed_ptr->is_a_dag ? "TRUE" : "FALSE",
	   sxdfa_packed_ptr->private.from_left_to_right ? "TRUE" : "FALSE"
	   );

  fprintf (file, "};\n");
}


void
sxdfa_packed_free (struct sxdfa_packed_struct *sxdfa_packed_ptr)
{
  if (sxdfa_packed_ptr->stats) {
    fprintf (sxdfa_packed_ptr->stats, "// Statistics for \"%s\": total size (in bytes) = %ld\n", sxdfa_packed_ptr->name,
	     sizeof (struct sxdfa_packed_struct)
	     + sizeof (SXUINT) * (sxdfa_packed_ptr->delta [0] + 1)
	     );
  }

  if (!sxdfa_packed_ptr->is_static) {
    sxfree (sxdfa_packed_ptr->delta), sxdfa_packed_ptr->delta = NULL;
  }
}


#if 0

/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 2007 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   * Produit de l'équipe PASSAGE.                         *
   *							  *
   ******************************************************** */



/* This package allows the handling of (small) integers that can be used 
   to index unknown external structures.
   The two mains macros are
   sxindex_next () qui retourne le prochain index disponnible
   sxsindex_release () qui indique qu'un index est de nouveau disponible
*/


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20070816 16:20 (pb):	Creation                   		        */
/************************************************************************/

#ifndef sxsindex_h
#define sxsindex_h
#include "SXante.h"

#include "sxalloc.h"
#include "sxstack.h"

typedef struct {
  SXINT max_size /* current max_size */
  , index /* greater index ever returned */
    ;
  SXINT *free_stack /* Stack of already released sindexes Pile des index*/ ;
  void	(*oflw) (SXINT old_size, SXINT new_size);
} sxsindex_header;


extern void	sxindex_alloc (sxsindex_header *header,
			       SXINT initial_size,
			       void (*user_oflw) ());



#define sxsindex_alloc(h,l,o)  ((h).max_size=l\
                               , (h).index=0\
                               , (h).oflw=o\
                               , (h).free_stack=NULL\
                              )

#define sxsindex_next(h)      (((h).free_stack && (h).free_stack[0]) ? POP((h).free_stack)\
                                                                     : (((++(h).index == (h).max_size) ? (*(h).oflw)((h).max_size,(h).max_size*2), (h).max_size *= 2\
                                                                                                       : NULL)\
                                                                       , (h).index)\
                              )

#define sxsindex_release h,x)  (((h).free_stack == NULL) ? (DALLOC_STACK((h).free_stack, ((h).max_size/10)+1))\
                                                         : NULL\
                                , DPUSH((h).free_stack,x)\
                               )

#define sxsindex_free(h)       ((h).free_stack ? (DFREE_STACK((h).free_stack))\
                                               : NULL\
                               )

#include "SXpost.h"
#endif /* sxsindex_h */
#endif /* 0 */

/*
 * Local Variables:
 * mode: C
 * version-control: yes
 * End:
 */
