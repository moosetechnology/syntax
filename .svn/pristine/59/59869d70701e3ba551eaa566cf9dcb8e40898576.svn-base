/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/



/*
  Regroupe les operations sur la foret d'analyse partagee produite
  par les analyseurs non-deterministes
*/

/* Les parcours de la rcg_spf */
/* Pour l'utilisateur la rcg_spf est non cyclique.  Si elle l'etait apres la phase
   d'analyse, elle a ete elaguee (les productions cycliques ont ete eliminees par
   une passe synthetisee) */
/* Ds le cas de la visite heritee (*pi)(Pij), si Pij est une Aij-iprod,
   on est su^r que :
      - Pij est appele'e (si elle est accessible depuis l'axiome et non elimine'e) une 
         et une seule fois
      - toutes les productions qui contiennent Aij en rhs ont deja ete appelees
      - toutes les Aij-iprods sont appelees en sequence : si l'appel suivant est de
         la forme (*pi)(P'ij), alors soit P'ij est une Aij-iprod ou, si ce n'est
         pas le cas plus aucun des appels ulterieurs ne se fera avec une Aij-iprod */
/* Ds le cas de la visite synthetisee (*pd)(Pij), si Pij est une Aij-iprod,
   on est su^r que :
      - Pij est appele'e (si elle est accessible depuis l'axiome et non elimine'e) une 
         et une seule fois
      - toutes les Aij-iprods sont appelees en sequence
      - et que si Xpq est en rhs de Pij, toutes les Xpq-iprods ont deja ete appelees */
/* Attention soit pi, soit pd est non NULL (on ne peut par parcourir la foret haut-bas
   gauche-droit avec les contraintes precedentes) */
/* Les valeurs entieres retournees par les ft (*pi)(Pij) ou (*pd)(Pij) de l'utilisateur ont 3 formes :
   1  => rien (cas standard)
   0  => la production Pij est saute'e
   -1 => la production Pij est elimine'e de la rcg_spf
*/



#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXRCG_SPF_MNGR[] = "@(#)SYNTAX - $Id: sxrcg_spf_mngr.c 1491 2008-07-23 16:13:15Z sagot $" WHAT_DEBUG;

static char	ME [] = "sxrcg_spf_mngr";

/* Pour ne pas avoir ici la definition et l'initialisation de rcg_spf proposée par sxrcg_spf.h,
   on ne fait pas #define SX_DFN_EXT_VAR2 */
#include "XxYxZ.h"
#include "XH.h"
#include "varstr.h"

#define SX_DFN_EXT_VAR_RCG3
#include "rcg_glbl.h"

#include "fsa.h"
#include "sxstack.h"
#include "udag_scanner.h"

static char                             **glbl_t2string;
static SXINT                            eof_Tpq_code; /* MAX_Tpq()+1 */

static SXINT *input_dag_state2p;


static void
rcg_spf_allocate_area (struct area_hd *area_hd)
{
  struct area_block	*new_block, **prev;

  if ((new_block = area_hd->empty)) {
      /* Reutilisation de blocks vides */
      prev = &(area_hd->empty);

      do {
	  if (new_block->size >= area_hd->size)
	      break;
      } while (prev = &(new_block->next), new_block = new_block->next);

      if (new_block) {
	  *prev = new_block->next;
	  area_hd->size = new_block->size;
      }
  }
      
  if (new_block == NULL) {
      new_block = (struct area_block*) sxalloc (1, sizeof(SXINT)+sizeof(struct area_block*)+area_hd->size);
      new_block->size = area_hd->size;
  }

  new_block->next = area_hd->bot;
  area_hd->bot = new_block;
  area_hd->top = &(new_block->node [0]);
  area_hd->room = area_hd->size;
}

static struct spf_node *
rcg_spf_book_area (struct area_hd *area_hd, SXINT int_nb)
{
    struct spf_node	*node_ptr;
    SXINT			char_size;

    /* char_size = sizeof (struct spf_node*) + sizeof (SXINT)*int_nb; */
    char_size = sizeof (struct spf_node) + sizeof (SXINT)*(int_nb-1); /* Ca permet de modifier la def de struct spf_node */

    if (char_size > area_hd->room) {
	while (char_size > area_hd->size)
	    area_hd->size *= 2 ;

	rcg_spf_allocate_area (area_hd);
    }

    area_hd->room -= char_size;
#if EBUG4
  if (area_hd->room < 0)
      sxtrap (ME, "rcg_spf_book_area");
#endif

    node_ptr = area_hd->top;

    area_hd->top = (struct spf_node *) ((char *) area_hd->top + char_size);

    return node_ptr;
}

static void
rcg_spf_free_area (struct area_hd *area_hd)
{
  struct area_block	*block_ptr, *next_block_ptr;

  for (block_ptr = area_hd->bot; block_ptr != NULL; block_ptr = next_block_ptr) {
    next_block_ptr = block_ptr->next;
    sxfree (block_ptr);
  }

  for (block_ptr = area_hd->empty; block_ptr != NULL; block_ptr = next_block_ptr) {
    next_block_ptr = block_ptr->next;
    sxfree (block_ptr);
  }

  area_hd->bot = area_hd->empty = NULL;
}


static void
rcg_spf_clear_area (struct area_hd *area_hd)
{
  struct area_block	*block_ptr, *next_block_ptr, **prev, *empty_ptr;

  block_ptr = area_hd->bot;
  next_block_ptr = block_ptr->next;
  block_ptr->next = NULL;

  if ((empty_ptr = area_hd->empty)) {
      while (prev = &(empty_ptr->next), empty_ptr = *prev);

      *prev = next_block_ptr;
  }
  else
      area_hd->empty = next_block_ptr;

  area_hd->top = &(block_ptr->node [0]);
  area_hd->room = block_ptr->size;
  /* bot et size sont bons */
}

#if PID > 1
/* cas PID > 1: on a un XxY Aij,pid->Aij_pid ; sem_disp est indexé par des Aij_pid et déborde avec ce XxY */
static void
AijXpid_hd_oflw (SXINT old_size, SXINT new_size)
{
  if (new_size > rcg_spf.cur_AijXxxx_hd_size) {
    rcg_spf.sem_disp = (SEM_TYPE*) sxrecalloc (rcg_spf.sem_disp, old_size+1, new_size+1, sizeof (SEM_TYPE));
    rcg_spf.cur_AijXxxx_hd_size = new_size;
  }
}
#endif  /* PID > 1 */

void
memo_oflw_second_part (SXINT old_size, SXINT new_size)
{
  /* cas PID = 1 (un seul module): on utilise les Aij comme des Aij_pid ; sem_disp est indexé par des Aij et déborde avec rhoA_hd dont l'overflow est memo_oflw dans le généré */
#if PID == 1
    rcg_spf.sem_disp = (SEM_TYPE*) sxrecalloc (rcg_spf.sem_disp, old_size+1, new_size+1, sizeof (SEM_TYPE));
#else /* PID == 1 */
    sxuse (old_size);
    sxuse (new_size);
#endif /* PID == 1 */
}


/* Derniere version :
   procedure unique pour le parcourt de la foret partagee
   pi ou pd peuvent etre NULL
   la valeur retournee peut etrwe -1, 0 ou 1
     -1 : abondon de la visite et suppression de la sous-foret
      0 : abondon de la visite
      1 : on continue
   la visite peut commencer sur un noeud qqc root
*/

/* A mettre ds topo (et changer topo en walk) */

/* Nelle implementation de la "suppression" de Pij et de la "suppression" de toutes
   les Pij d'un Aij.
   p=GET_RCG_SPF_HEAD_NODE_PTR     q=GET_RCG_SPF_TAIL_NODE_PTR
        NULL                  NULL                => Pas de Aij-clauses depuis la construction
        NULL                  non-NULL            => q pointe vers le 1er de la liste des supprimes
	non-NULL              NULL                => impossible
	non-NULL              non_NULL            => p pointe vers le 1er de la liste des actifss
                                                     q pointe vers le dernier de la liste des actifs
						     et le next est le 1er de la liste des supprimes
*/
   
/* Procedure qui passe un noeud "actif" ds la liste des noeuds "erased" */
static void
rcg_spf_erase_an_iclause (struct spf_node **prev_rcg_spf_node_ptr_address,
			  struct spf_node **erased_head_spf_node_ptr_address)
{
  struct spf_node *cur_rcg_spf_node_ptr;

  cur_rcg_spf_node_ptr = *prev_rcg_spf_node_ptr_address;

  *prev_rcg_spf_node_ptr_address = cur_rcg_spf_node_ptr->next;

  cur_rcg_spf_node_ptr->next = *erased_head_spf_node_ptr_address;

  *erased_head_spf_node_ptr_address = cur_rcg_spf_node_ptr;

  rcg_spf.is_proper = SXFALSE;
}

static void
rcg_spf_reactivate_an_iclause (struct spf_node **base_rcg_spf_node_ptr_address,
			       struct spf_node **erased_rcg_spf_node_ptr_address)
{
  struct spf_node *erased_rcg_spf_node_ptr, *next_erased_rcg_spf_node_ptr, *base_rcg_spf_node_ptr;
  struct spf_node **next_erased_rcg_spf_node_ptr_address;

  erased_rcg_spf_node_ptr = *erased_rcg_spf_node_ptr_address;
  next_erased_rcg_spf_node_ptr = *(next_erased_rcg_spf_node_ptr_address = &(erased_rcg_spf_node_ptr->next));
  base_rcg_spf_node_ptr = *base_rcg_spf_node_ptr_address;

  *base_rcg_spf_node_ptr_address = erased_rcg_spf_node_ptr;
  *next_erased_rcg_spf_node_ptr_address = base_rcg_spf_node_ptr;
  *erased_rcg_spf_node_ptr_address = next_erased_rcg_spf_node_ptr;
}

static void
rcg_spf_erase_all_Aij_clauses (struct spf_node **base_rcg_spf_node_ptr_address,
			       struct spf_node **erased_head_spf_node_ptr_address)
{
  struct spf_node *cur_rcg_spf_node_ptr;
  struct spf_node **cur_rcg_spf_node_ptr_address;
 
  cur_rcg_spf_node_ptr_address = base_rcg_spf_node_ptr_address;
  
  if ((cur_rcg_spf_node_ptr = *cur_rcg_spf_node_ptr_address)) {
#if LLOG
    fputs ("All valid ", stdout);
    rcg_spf_print_Xpq (stdout, cur_rcg_spf_node_ptr->rho [1]);
    fputs ("-clauses are erased:\n", stdout);
#endif /* LLOG */

    /* On cherche la fin de la liste des "actifs" */
    do {
#if LLOG
      rcg_spf_print_total_instantiated_clause (stdout,
					       cur_rcg_spf_node_ptr->G,
					       &(cur_rcg_spf_node_ptr->rho[0]),
					       "\n");
#endif /* LLOG */
    } while ((cur_rcg_spf_node_ptr = *(cur_rcg_spf_node_ptr_address = &(cur_rcg_spf_node_ptr->next))));

    *cur_rcg_spf_node_ptr_address = *erased_head_spf_node_ptr_address;
    *erased_head_spf_node_ptr_address = *base_rcg_spf_node_ptr_address;
    *base_rcg_spf_node_ptr_address = NULL;

    rcg_spf.is_proper = SXFALSE;
  }
}


/* Corps recursif */
static SXBOOLEAN
rcg_spf_walk (SXINT lhs_iprdct)
{
  SXINT             Aij_pid, Xpq, result_kind, pd_kind, pi_kind, clause, son_nb, son;
  SXINT             *rho;
  SXBOOLEAN         must_be_kept, Pij_kept, Xpq_kept, is_a_non_erased_Aij;
  struct spf_node   *cur_rcg_spf_node_ptr, *next_rcg_spf_node_ptr;
  struct spf_node   **base_rcg_spf_node_ptr_address,**prev_rcg_spf_node_ptr_address,**next_rcg_spf_node_ptr_address,**erased_head_spf_node_ptr_address;
  struct G          *G;

  sxinitialise(result_kind); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(pi_kind); /* pour faire taire "gcc -Wuninitialized" */

  if (lhs_iprdct > 0) {
    is_a_non_erased_Aij = SXTRUE;
    Aij_pid = lhs_iprdct;
  }
  else {
    is_a_non_erased_Aij = SXFALSE;
    Aij_pid = -lhs_iprdct;
  }

  base_rcg_spf_node_ptr_address = &(GET_RCG_SPF_HEAD_NODE_PTR (Aij_pid));
  erased_head_spf_node_ptr_address = &(GET_RCG_SPF_ERASED_HEAD_NODE_PTR (Aij_pid));
  G = (*base_rcg_spf_node_ptr_address)->G;

  if (rcg_spf.walk.pass_inherited && is_a_non_erased_Aij) {
    /* On fait les appels herites des Aij-prods sans autres appels intercales */
    /* Aij est accessible */
    if (rcg_spf.walk.pre_pass) {
      /* Debut des appels top-down des Aij_prods */
      pi_kind = (*rcg_spf.walk.pre_pass) (Aij_pid);

#if LLOG
      fputs ("The pre top_down visit of ", stdout);
      rcg_spf_print_Xpq (stdout, Aij_pid);
      printf (" is %s\n", pi_kind > 0 ? "licensed" : (pi_kind == 0 ? "skipped" : "erased"));
#endif /* LLOG */

      if (pi_kind <= 0) {
	if ((cur_rcg_spf_node_ptr = *base_rcg_spf_node_ptr_address)) {
	  /* Il y a des noeuds non supprimes */
	  do {
	    if ((rcg_spf.walk.user_filtering_function == 0)
		|| (*rcg_spf.walk.user_filtering_function) (cur_rcg_spf_node_ptr)
		/* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
		) {
	      cur_rcg_spf_node_ptr->wcell = pi_kind;
	    }
	  } while ((cur_rcg_spf_node_ptr = cur_rcg_spf_node_ptr->next));
	}
	result_kind = pi_kind;
      }
    }
    else
      pi_kind = 1;

    if (pi_kind > 0) {
      result_kind = -1;
      if ((cur_rcg_spf_node_ptr = *base_rcg_spf_node_ptr_address)) {
	do {
	  /* On parcourt les clauses dont la lhs est Aij */
	  /* On appelle pass_inherited sur chaque clause, Ca permet eventuellement
	     de "repartir" les attributs herites de Aij sur chacune de ses Aij-prods */
	  /* Si rcg_spf.walk.pass_inherited est NULL, ces appels "a vide" permettent d'armer les appels de pass_derived */
	  if ((rcg_spf.walk.user_filtering_function == 0)
	      || (*rcg_spf.walk.user_filtering_function) (cur_rcg_spf_node_ptr)
	      /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	      ) {
	    /* Non elimine' (d'un coup precedent) */
	    /* cet Aij est "non elimine'" */
	    pi_kind = cur_rcg_spf_node_ptr->wcell = (*rcg_spf.walk.pass_inherited) (cur_rcg_spf_node_ptr);

#if LLOG
	    rcg_spf_print_total_instantiated_clause (stdout, G, &(cur_rcg_spf_node_ptr->rho [0]), "");
	    printf (" ... top_down visit completed%s\n", pi_kind > 0 ? "" : (pi_kind == 0 ? " (to be skipped)" : " (to be erased)"));
#endif /* LLOG */

	    if (pi_kind >= 0) {
	      if (pi_kind == 0) {
		if (result_kind == -1)
		  /* on doit sauter Pij et c'est la 1ere Aij-prod que l'on saute */
		  result_kind = 0;
	      }
	      else {
		/* Au moins un Pij correct */
		result_kind = 1;
	      }
	    }
	  }
	} while ((cur_rcg_spf_node_ptr = cur_rcg_spf_node_ptr->next));
      }
    }
    
    if (rcg_spf.walk.post_pass) {
      /* Debut des appels top-down des Aij_prods */
      pi_kind = (*rcg_spf.walk.post_pass) (lhs_iprdct, (SXBOOLEAN) (result_kind > 0));

#if LLOG
      fputs ("The post top_down visit of ", stdout);
      rcg_spf_print_Xpq (stdout, Aij_pid);
      printf ("%s\n", pi_kind > 0 ? "succeeds" : (pi_kind == 0 ? "is strange!" : "fails"));
#endif /* LLOG */

      if (result_kind > 0 && pi_kind < 0) {
	/* On vire les Aij-clauses qui restent */
	if ((cur_rcg_spf_node_ptr = *base_rcg_spf_node_ptr_address)) {
	  do {
	    /* On parcourt les clauses dont la lhs est Aij */
	    /* On appelle pass_inherited sur chaque clause, Ca permet eventuellement
	       de "repartir" les attributs herites de Aij sur chacune de ses Aij-prods */
	    /* Si rcg_spf.walk.pass_inherited est NULL, ces appels "a vide" permettent d'armer les appels de pass_derived */
	    if ((rcg_spf.walk.user_filtering_function == 0)
		|| (*rcg_spf.walk.user_filtering_function) (cur_rcg_spf_node_ptr)
		/* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
		) {
	      /* Non elimine' (d'un coup precedent) */
	      /* On prepare l'elimination ... */
	      cur_rcg_spf_node_ptr->wcell = -1;
	    }
	  } while ((cur_rcg_spf_node_ptr = cur_rcg_spf_node_ptr->next));
	}
	result_kind = -1;
      }
    }
  }
  else
    result_kind = 1;

  if (*base_rcg_spf_node_ptr_address) {
    next_rcg_spf_node_ptr_address = base_rcg_spf_node_ptr_address;

    while ((cur_rcg_spf_node_ptr = *(prev_rcg_spf_node_ptr_address = next_rcg_spf_node_ptr_address))) {
      /* On parcourt les productions dont la lhs est Aij */
      /* On appelle pass_inherited sur chaque prod, Ca permet eventuellement
	 de "repartir" les attributs herites de Aij sur chacune de ses Aij-prods */
      /* Si rcg_spf.walk.pass_inherited est NULL, ces appels "a vide" permettent d'armer les appels de pass_derived */
      Pij_kept = SXTRUE;
      next_rcg_spf_node_ptr_address = &(cur_rcg_spf_node_ptr->next);

      if ((rcg_spf.walk.user_filtering_function == 0)
	  || (*rcg_spf.walk.user_filtering_function) (cur_rcg_spf_node_ptr)
	  /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	  ) {
	/* Non elimine' (d'un coup precedent) */
	clause = *(rho = &(cur_rcg_spf_node_ptr->rho [0]));

	if (rcg_spf.walk.pass_inherited && is_a_non_erased_Aij) {
	  pi_kind = cur_rcg_spf_node_ptr->wcell;

	  if (pi_kind < 0) {
	    /* A eliminer */
	    /* Attention, ne peut pas etre fait ds la boucle rcg_spf.walk.pass_inherited */
	    Pij_kept = SXFALSE;
	    next_rcg_spf_node_ptr_address = prev_rcg_spf_node_ptr_address;
	    rcg_spf_erase_an_iclause (prev_rcg_spf_node_ptr_address, erased_head_spf_node_ptr_address);
	  }
	}

	rho++;
	son_nb = G->clause2son_nb [clause];

	if (son_nb) {
	  for (son = 1; son <= son_nb; son++) {
	    Xpq = rho [son];

	    if (rcg_spf.walk.pass_inherited) {
	      if (is_a_non_erased_Aij && pi_kind > 0)
		SXBA_1_bit (rcg_spf.walk.Aij_set, Xpq);

	      if (--rcg_spf.walk.Aij2nyp_nb [Xpq] == 0 /* ...calcule' */) {
		must_be_kept = rcg_spf_walk (SXBA_bit_is_set (rcg_spf.walk.Aij_set, Xpq) /* a au moins un pere valide */ ? Xpq : -Xpq);

		if (!must_be_kept) {
		  /* Echec de l'evaluation sur Xpq */
		  /* On elimine la Pij courante */
#if LOG
		  rcg_spf_print_total_instantiated_clause (stdout, G, rho-1, " ... is dead-end erased\n");
#endif /* LOG */
		  Pij_kept = SXFALSE;
		  next_rcg_spf_node_ptr_address = prev_rcg_spf_node_ptr_address;
		  rcg_spf_erase_an_iclause (prev_rcg_spf_node_ptr_address, erased_head_spf_node_ptr_address);
		  /* Ds ce cas il faut continuer sur les autres fils pour avoir un calcul correct ds rcg_spf.walk.Aij2nyp_nb */
		}
	      }
	    }
	    else {
	      /* passe synthetisees */
	      if (SXBA_bit_is_reset_set (rcg_spf.walk.Aij_set, Xpq)) {
		/* ...non encore visite' */
		SXBA_1_bit (rcg_spf.walk.cyclic_Aij_set, Xpq);
		must_be_kept = rcg_spf_walk (Xpq);
		SXBA_0_bit (rcg_spf.walk.cyclic_Aij_set, Xpq);

		if (!must_be_kept) {
		  /* Echec de l'evaluation sur Xpq */
		  SXBA_1_bit (rcg_spf.walk.invalid_Aij_set, Xpq);
		  /* On elimine la Pij courante */
		  Pij_kept = SXFALSE;
		  next_rcg_spf_node_ptr_address = prev_rcg_spf_node_ptr_address;
		  rcg_spf_erase_an_iclause (prev_rcg_spf_node_ptr_address, erased_head_spf_node_ptr_address);
#if LLOG	
		  rcg_spf_print_total_instantiated_clause (stdout, G, rho-1, " ... is dead-end erased\n");
#endif /* LLOG */
		  /* Ds ce cas (pas de passe heritee demandee) inutile de continuer sur les autres fils */
		  break;
		}
	      }
	      else {
		/* deja visite' ... */
		if (SXBA_bit_is_set (rcg_spf.walk.cyclic_Aij_set, Xpq)
		    /* ... et cyclique
		       On est ds le cas 
		       Xpq =>* ... =>Pij \alpha Xpq \beta (on a ij == pq)
		       on va donc eliminer Pij mais Xpq est peut-etre quand meme valide par une autre P'ij */
		    || SXBA_bit_is_set (rcg_spf.walk.invalid_Aij_set, Xpq)
		    /* ... et a echoue' */
		    ) {
		  /* On elimine la Pij courante */
		  Pij_kept = SXFALSE;
		  next_rcg_spf_node_ptr_address = prev_rcg_spf_node_ptr_address;
		  rcg_spf_erase_an_iclause (prev_rcg_spf_node_ptr_address, erased_head_spf_node_ptr_address);
#if LLOG	
		  rcg_spf_print_total_instantiated_clause (stdout, G, rho-1, SXBA_bit_is_set (rcg_spf.walk.cyclic_Aij_set, Xpq) ? " ... is cycle erased\n" : " ... is dead-end erased\n");
#endif /* LLOG */

		  /* Ds ce cas (pas de passe heritee demandee) inutile de continuer sur les autres fils */
		  break;
		}
	      }
	    }
	  }
	  /* Cas hérité: élimination de la Pij complète si l'un des Xpq n'est pas gardable */
	  if (rcg_spf.walk.pass_inherited) {
	    if (!Pij_kept) {
	      /* Echec de l'evaluation d'au moins un Xpq de ce Pij */
	      cur_rcg_spf_node_ptr->rho [0] = -cur_rcg_spf_node_ptr->rho [0]; /* On elimine la Pij courante */
#if LLOG	
	      rcg_spf_print_total_instantiated_clause (stdout, G, &(cur_rcg_spf_node_ptr->rho [0]), " ... is dead-end erased\n");
#endif /* LLOG */
	    }
	  }
	}

	if (Pij_kept) { /* Pij non éliminée */
	  /* filtre supplementaire utilisateur sur les Pij sans modifier la foret: */
	  if (!(
		(rcg_spf.walk.user_filtering_function == 0)
		|| (*rcg_spf.walk.user_filtering_function) (cur_rcg_spf_node_ptr)
		)) {
	  /* Pij finalement à éliminer */
	    Pij_kept = SXFALSE;
	  }
	}
	
	if (Pij_kept)
	  must_be_kept = SXTRUE;
      }
    }
  }

  if (result_kind > 0 && !must_be_kept)
    /* L'evaluation de la sous-foret a echoue' */
    result_kind = -1;

  /* On lance les passes synthetisees sur les Aij-prods, sans intercalage!! */
  if (rcg_spf.walk.pass_derived && is_a_non_erased_Aij && result_kind > 0) {
    if (*base_rcg_spf_node_ptr_address) {
      if (rcg_spf.walk.pre_pass) {
	/* Debut des appels bottom_up des Aij_prods */
	pd_kind = (*rcg_spf.walk.pre_pass) (Aij_pid);

#if LLOG
	fputs ("The pre bottom_up visit of ", stdout);
	rcg_spf_print_Xpq (stdout, Aij_pid);
	printf ("is %s\n", pd_kind > 0 ? "licensed" : (pd_kind == 0 ? "skipped" : "erased"));
#endif /* LLOG */

	if (pd_kind < 0)
	  rcg_spf_erase_all_Aij_clauses (base_rcg_spf_node_ptr_address, erased_head_spf_node_ptr_address);
      }
      else
	pd_kind = 1;

      if (pd_kind == 1) {
	must_be_kept = SXFALSE;
	next_rcg_spf_node_ptr_address = base_rcg_spf_node_ptr_address;

	while ((cur_rcg_spf_node_ptr = *(prev_rcg_spf_node_ptr_address = next_rcg_spf_node_ptr_address))) {
	  /* On parcourt les clauses dont la lhs est Aij */
	  /* On appelle pass_inherited sur chaque clause, Ca permet eventuellement
	     de "repartir" les attributs herites de Aij sur chacune de ses Aij-prods */
	  /* Si rcg_spf.walk.pass_inherited est NULL, ces appels "a vide" permettent d'armer les appels de pass_derived */

	  next_rcg_spf_node_ptr_address = &(cur_rcg_spf_node_ptr->next);

	  if ((rcg_spf.walk.user_filtering_function == 0)
		  || (*rcg_spf.walk.user_filtering_function) (cur_rcg_spf_node_ptr)
	      /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	      ) {
	    /* Non elimine' (d'un coup precedent) */
	    /* cet Aij est "non elimine'" */
	    pd_kind = (*rcg_spf.walk.pass_derived) (cur_rcg_spf_node_ptr);

#if LLOG
	    rcg_spf_print_total_instantiated_clause (stdout, G, &(cur_rcg_spf_node_ptr->rho [0]), NULL);
	    printf (" ... bottom-up visit completed%s\n", pd_kind > 0 ? "" : (pd_kind == 0 ? " (to be skipped)" : " (to be erased)"));
#endif /* LLOG */

	    if (pd_kind < 0) {
	      next_rcg_spf_node_ptr_address = prev_rcg_spf_node_ptr_address;
	      rcg_spf_erase_an_iclause (prev_rcg_spf_node_ptr_address, erased_head_spf_node_ptr_address);
	    }
	    else {
	      /* Ce Pij n'est pas elimine' */
	      must_be_kept = SXTRUE;
	    }
	  }
	}
      }
      else
	must_be_kept = (pd_kind >= 0);
    
      if (rcg_spf.walk.post_pass) {
	/* Fin des appels bottom-up des Aij_prods */
	/* Faut-il appeler post_pass_derived si toutes les Aij-prods ont ete eliminees ? */
	pd_kind = (*rcg_spf.walk.post_pass) (lhs_iprdct, must_be_kept);

#if LLOG
	fputs ("The post bottom_up visit of ", stdout);
	rcg_spf_print_Xpq (stdout, Aij_pid);
	printf ("%s\n", pd_kind > 0 ? "succeeds" : (pd_kind == 0 ? "is strange!" : "fails"));
#endif /* LLOG */

	if (must_be_kept && pd_kind < 0) {
	  /* On vire les Aij-prods qui restent */
	  next_rcg_spf_node_ptr_address = base_rcg_spf_node_ptr_address;

	  while ((cur_rcg_spf_node_ptr = *(prev_rcg_spf_node_ptr_address = next_rcg_spf_node_ptr_address))) {
	    /* On parcourt les productions dont la lhs est Aij */
	    /* On appelle pass_inherited sur chaque prod, Ca permet eventuellement
	       de "repartir" les attributs herites de Aij sur chacune de ses Aij-prods */
	    /* Si rcg_spf.walk.pass_inherited est NULL, ces appels "a vide" permettent d'armer les appels de pass_derived */
	    next_rcg_spf_node_ptr_address = &(cur_rcg_spf_node_ptr->next);

	    if ((rcg_spf.walk.user_filtering_function == 0)
		|| (*rcg_spf.walk.user_filtering_function) (cur_rcg_spf_node_ptr)
		/* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
		) {
	      /* On l'elimine ... */
	      next_rcg_spf_node_ptr_address = prev_rcg_spf_node_ptr_address;
	      rcg_spf_erase_an_iclause (prev_rcg_spf_node_ptr_address, erased_head_spf_node_ptr_address);
	    }
	  }
	  must_be_kept = SXFALSE;
	}
      }
    }
  }
  else
    must_be_kept = result_kind >= 0;

  return must_be_kept;
}





/* Retourne faux ssi toute la foret a disparue !! */
/* Wrappeur pour rcg_spf_topological_[top_down_|bottom_up_]walk */
static SXBOOLEAN
rcg_spf_call_walk_core (SXINT root)
{
  SXINT             Xpq, Aij, clause, pid, son_nb, son;
  SXINT             *rho;
  struct G          *G;
  struct spf_node   *cur_rcg_spf_node_ptr;
  SXBOOLEAN         ret_val;

  if (rcg_spf.walk.invalid_Aij_set == NULL)
    rcg_spf.walk.invalid_Aij_set = sxba_calloc (rcg_spf.max_instantiated_prdct_nb+1);
  else
    sxba_empty (rcg_spf.walk.invalid_Aij_set);

  if (rcg_spf.walk.pass_inherited) {
    /* Passe heritee demandee.  Les Aij-prods ne sont appelees que lorsque toutes les prods (de la sous-foret root)
       ayant un Aij en partie droite ont ete appelees */
    /* init ... */
    if (rcg_spf.walk.Aij2nyp_nb == NULL)
      rcg_spf.walk.Aij2nyp_nb = (SXINT*) sxcalloc ((SXUINT)rcg_spf.max_instantiated_prdct_nb+1, sizeof (SXINT));
    /* else
       On suppose que les appels precedents ont laisses rcg_spf.walk.Aij2nyp_nb vide */

    if (root == rcg_spf.start_symbol) {
      for (Aij = 1; Aij <= rcg_spf.max_instantiated_prdct_nb; Aij++) {
	rcg_spf.walk.Aij2nyp_nb [Aij] = rcg_spf.Aij2rhs_nb [Aij];
      }
    }
    else {
      /* cas de la visite d'une sous foret => calcul "dynamique" de rcg_spf.walk.Aij2nyp_nb */
      if (rcg_spf.walk.Aij_stack == NULL)
	rcg_spf.walk.Aij_stack = (SXINT*) sxalloc (rcg_spf.max_instantiated_prdct_nb+1, sizeof (SXINT));

      rcg_spf.walk.Aij_stack [0] = 0; /* prudence sur les appels multiples */

      PUSH (rcg_spf.walk.Aij_stack, root);

      while (!IS_EMPTY (rcg_spf.walk.Aij_stack)) {
	Aij = POP (rcg_spf.walk.Aij_stack);

	for (cur_rcg_spf_node_ptr = GET_RCG_SPF_HEAD_NODE_PTR (Aij);
	     cur_rcg_spf_node_ptr != NULL;
	     cur_rcg_spf_node_ptr = cur_rcg_spf_node_ptr->next) {
	  if ((rcg_spf.walk.user_filtering_function == 0)
	      || (*rcg_spf.walk.user_filtering_function) (cur_rcg_spf_node_ptr)
	      /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	      ) {
	    rho = &(cur_rcg_spf_node_ptr->rho [0]);
	    clause = *rho++;
	    pid = Aij_pid2pid (Aij);
	    G = Gs [pid];
	    son_nb = G->clause2son_nb [clause];
	
	    for (son = 1; son <= son_nb; son++) {
	      Xpq = rho [son];

	      if (Xpq > 0) {
		/* Appel positif */
		if (rcg_spf.walk.Aij2nyp_nb [Xpq]++ == 0)
		  /* 1ere occur */
		  PUSH (rcg_spf.walk.Aij_stack, Xpq);
	      }
	    }
	  }
	}
      }
    }
  }

  if (rcg_spf.walk.Aij_set == NULL)
    rcg_spf.walk.Aij_set = sxba_calloc (rcg_spf.max_instantiated_prdct_nb+1);
  else
    sxba_empty (rcg_spf.walk.Aij_set);

  if (rcg_spf.walk.cyclic_Aij_set == NULL)
    rcg_spf.walk.cyclic_Aij_set = sxba_calloc (rcg_spf.max_instantiated_prdct_nb+1);

  SXBA_1_bit (rcg_spf.walk.Aij_set, root);
  SXBA_1_bit (rcg_spf.walk.cyclic_Aij_set, root);

  ret_val = rcg_spf_walk (root);

  SXBA_0_bit (rcg_spf.walk.cyclic_Aij_set, root);

#if EBUG
  /* à faire: vérif similaire si root != start_symbol */
  if (rcg_spf.walk.pass_inherited && root == rcg_spf.start_symbol) {
    for (Aij = 1; Aij <= rcg_spf.max_instantiated_prdct_nb; Aij++) {
      if (rcg_spf.walk.Aij2nyp_nb [Aij] != 0) {
	fputs ("The inherited visit of the sub-forest rooted in ", stderr);
	rcg_spf_print_Xpq (stderr, Aij);
	fprintf (stderr, " missed %ld visits.\n", rcg_spf.walk.Aij2nyp_nb [Aij]);
	sxtrap (ME, "rcg_spf_call_walk_core");
      }
    }
  }
#endif /* EBUG */

  return ret_val;
}


static SXBOOLEAN
rcg_spf_call_walk (SXINT root)
{
  SXBOOLEAN ret_val;

#if EBUG
  SXBA              valid_set, erased_set;
  SXINT             Aij, iclause, clause;
  struct spf_node   *rcg_spf_node_ptr;

  if (!rcg_spf.is_proper)
    sxtrap (ME, "rcg_spf_call_walk (can't call a forest walk on a non-proper forest)");

  valid_set = sxba_calloc (rcg_spf.max_instantiated_clause_nb+1);
  erased_set = sxba_calloc (rcg_spf.max_instantiated_clause_nb+1);
  
  for (Aij = 1; Aij <= rcg_spf.max_instantiated_prdct_nb; Aij++) {
    for (rcg_spf_node_ptr = GET_RCG_SPF_HEAD_NODE_PTR (Aij);
	 rcg_spf_node_ptr != NULL;
	 rcg_spf_node_ptr = rcg_spf_node_ptr->next) {
      /* On marque les bonnes */
      iclause = rcg_spf_node_ptr->instantiated_clause_nb;
      SXBA_1_bit (valid_set, iclause);
    }

    for (rcg_spf_node_ptr = GET_RCG_SPF_ERASED_HEAD_NODE_PTR (Aij);
	 rcg_spf_node_ptr != NULL;
	 rcg_spf_node_ptr = rcg_spf_node_ptr->next) {
      /* On marque les bonnes */
      iclause = rcg_spf_node_ptr->instantiated_clause_nb;
      SXBA_1_bit (erased_set, iclause);
    }
  }
#endif /* EBUG */

  ret_val = rcg_spf_call_walk_core (root);

#if EBUG
  if (rcg_spf.is_proper) {
    for (Aij = 1; Aij <= rcg_spf.max_instantiated_prdct_nb; Aij++) {
      for (rcg_spf_node_ptr = GET_RCG_SPF_HEAD_NODE_PTR (Aij);
	   rcg_spf_node_ptr != NULL;
	   rcg_spf_node_ptr = rcg_spf_node_ptr->next) {
	iclause = rcg_spf_node_ptr->instantiated_clause_nb;

	if (!SXBA_bit_is_set (valid_set, iclause))
	  sxtrap (ME, "rcg_spf_call_walk (forest walk changed validity status of some instanciated productions, but the forest has not been declared not-proper)");
      }

      for (rcg_spf_node_ptr = GET_RCG_SPF_ERASED_HEAD_NODE_PTR (Aij);
	   rcg_spf_node_ptr != NULL;
	   rcg_spf_node_ptr = rcg_spf_node_ptr->next) {
	iclause = rcg_spf_node_ptr->instantiated_clause_nb;

	if (!SXBA_bit_is_set (erased_set, iclause))
	  sxtrap (ME, "rcg_spf_call_walk (forest walk changed validity status of some instanciated productions, but the forest has not been declared not-proper)");
      }
    }
  } 
  
  sxfree (valid_set);
  sxfree (erased_set);
#endif /* EBUG */

  if (!rcg_spf.is_proper) {
    if (GET_RCG_SPF_HEAD_NODE_PTR (root) == NULL) /* l'axiome a été erasé, la forêt est donc devenue vide à cause des intersections de DAGs */
      return SXFALSE;

    ret_val = rcg_spf_make_proper (root);
  }

  return ret_val;
}


SXBOOLEAN
rcg_spf_topological_walk (SXINT root,
			  SXINT (*pi)(struct spf_node *),
			  SXINT (*pd)(struct spf_node *))
{
  SXBOOLEAN ret_val;

#if EBUG
  if (pi && pd)
    /* On ne peut pas simultanement parcourir une foret top-down et bottom-up avec les contraintes afferentes 
     (on peut sortir d'un sous-arbre sans avoir visite' ses fils bottom-up !!) */
    sxtrap (ME, "rcg_spf_topological_walk");
#endif /* EBUG */

#if LOG
  fputs ("*** Entering rcg_spf_topological_walk (", stdout);
  rcg_spf_print_Xpq (stdout, root);
  printf (", %s, %s)\n", pi == NULL ? "NULL" : "pi", pd == NULL ? "NULL" : "pd");
#endif /* LOG */

  rcg_spf.walk.pass_inherited = pi;
  rcg_spf.walk.pass_derived = pd;
  rcg_spf.walk.pre_pass = NULL;
  rcg_spf.walk.post_pass = NULL;

  ret_val = rcg_spf_call_walk (root);

#if LOG
  fputs ("*** Leaving rcg_spf_topological_walk (", stdout);
  rcg_spf_print_Xpq (stdout, root);
  printf (", %s, %s)\n", pi == NULL ? "NULL" : "pi", pd == NULL ? "NULL" : "pd");
#endif /* LOG */

  return ret_val;
}



SXBOOLEAN
rcg_spf_topological_top_down_walk (SXINT root,
			       SXINT (*pi)(struct spf_node *),
			       SXINT (*pre_pass)(SXINT),
			       SXINT (*post_pass)(SXINT, SXBOOLEAN))
{
  SXBOOLEAN ret_val;

#if LOG
  fputs ("*** Entering rcg_spf_topological_top_down_walk (", stdout);
  rcg_spf_print_Xpq (stdout, root);
  printf (", %s, %s, %s)\n", pi == NULL ? "NULL" : "pi", pre_pass == NULL ? "NULL" : "pre_pass", post_pass == NULL ? "NULL" : "post_pass");
#endif /* LOG */

  rcg_spf.walk.pass_inherited = pi;
  rcg_spf.walk.pass_derived = NULL;
  rcg_spf.walk.pre_pass = pre_pass;
  rcg_spf.walk.post_pass = post_pass;

  ret_val = rcg_spf_call_walk (root);

#if LOG
  fputs ("*** Leaving rcg_spf_topological_top_down_walk (", stdout);
  rcg_spf_print_Xpq (stdout, root);
  printf (", %s, %s, %s)\n", pi == NULL ? "NULL" : "pi", pre_pass == NULL ? "NULL" : "pre_pass", post_pass == NULL ? "NULL" : "post_pass");
#endif /* LOG */

  return ret_val;
}

SXBOOLEAN
rcg_spf_topological_bottom_up_walk (SXINT root,
				SXINT (*pd)(struct spf_node *),
				SXINT (*pre_pass)(SXINT),
				SXINT (*post_pass)(SXINT, SXBOOLEAN))
{
  SXBOOLEAN ret_val;

#if LOG
  fputs ("*** Entering rcg_spf_topological_bottom_up_walk (", stdout);
  rcg_spf_print_Xpq (stdout, root);
  printf (", %s, %s, %s)\n", pd == NULL ? "NULL" : "pd", pre_pass == NULL ? "NULL" : "pre_pass", post_pass == NULL ? "NULL" : "post_pass");
#endif /* LOG */

  rcg_spf.walk.pass_inherited = NULL;
  rcg_spf.walk.pass_derived = pd;
  rcg_spf.walk.pre_pass = pre_pass;
  rcg_spf.walk.post_pass = post_pass;

  ret_val = rcg_spf_call_walk (root);

#if LOG
  fputs ("*** Leaving rcg_spf_topological_bottom_up_walk (", stdout);
  rcg_spf_print_Xpq (stdout, root);
  printf (", %s, %s, %s)\n", pd == NULL ? "NULL" : "pd", pre_pass == NULL ? "NULL" : "pre_pass", post_pass == NULL ? "NULL" : "post_pass");
#endif /* LOG */

  return ret_val;
}



static char* predefined_predictate_ic2str [1-(CUT_ic)] = { "ASLAN",
							   "StrEq",
							   "StrEqLen",
							   "StrLen",
							   "False",
							   "First",
							   "Last",
							   "True",
							   "Lex",
							   "Cut",
};


void
rcg_spf_print_Xpq (FILE *out_file, SXINT Xpq_pid)
{
  SXINT                 Xpq, pid, arity, pos;
  SXBOOLEAN             is_neg_call;
  struct Aij_struct	*Aij_struct_ptr;

    if (Xpq_pid < 0) {
      /* le 09/02/04 */
      /* Appel negatif */
      Xpq_pid = -Xpq_pid;
      is_neg_call = SXTRUE;
    }
    else
      is_neg_call = SXFALSE;

    pid = Aij_pid2pid (Xpq_pid);
    Xpq = Aij_pid2Aij (Xpq_pid);

#if EBUG
    if (Xpq == 0)
      sxtrap (ME, "rcg_spf_print_Xpq (Xpq == 0)");
#endif /* EBUG */

    if (Xpq > 0) { // prédicat standard
      Aij_struct_ptr = (*(Gs [pid]->Aij2struct))(Xpq);
      fprintf (out_file, "%s%s (", is_neg_call ? "!" : "", Aij_struct_ptr->Astr);
      arity = Aij_struct_ptr->arity;
      
      for (pos = 0; pos < arity; pos++) {
	if (pos > 0)
	  fputs (", ", out_file);
	
	fprintf (out_file, "%ld..%ld", (long)Aij_struct_ptr->lb [pos], (long)Aij_struct_ptr->ub [pos]);
      }
      
      fputs (")", out_file);
    }
    else { // prédicat prédéfini
      SXINT couple = X_X (*(Gs [pid /* == 0 */]->prhoA_hd), -Xpq);
      SXINT lognt = Gs [pid]->lognt;
      SXINT nt = couple & ((1<<lognt)-1);
      fprintf (out_file, "%s&%s (", is_neg_call ? "!" : "", predefined_predictate_ic2str [nt]);

      switch (-nt) {
      case STRLEN_ic:
	/* afficher la longueur */

	fputs (", ", out_file);
	
	couple >>= lognt;
	fprintf (out_file, "%ld..%ld", (long)range2p (couple), (long)range2q (couple));
	break;
	
      default:
	sxtrap (ME, "rcg_spf_print_Xpq (unknown or unhandled predefined predicate)");
      }
      
      fputs (")", out_file);
    }
}



SXBOOLEAN
rcg_spf_print_partial_instantiated_clause (FILE *out_file, struct G *G, SXINT *rho, SXBOOLEAN is_G_name, SXBOOLEAN is_clause_nb, SXBOOLEAN is_lhs, SXBOOLEAN is_rhs, char *suffix)
  /* Pij_ptr est un tableau de taille son_nb+2
     Pij_ptr[0] contient le numero p de la clause courante.  Si p est <0 la clause est -p et
     l'appel est cyclique
     Pij_ptr[1] contient l'identifiant du vecteur de range de la LHS
     Pij_ptr[2], ..., Pij_ptr[son_nb+1] contient les identifiants des vecteurs de range de la RHS */
  /* Pij_ptr[son_nb+2] contient l'identifiant ds rhs_ranges des olb,oub */
  /* Si G->clause2lhst_disp est non NULL, Pij_ptr[son_nb+3], ... contient la liste des indexes
     ds le source des terminaux de la LHS */
  /* sons est un tableau de taille sons_nb+1, il matche clause.
       Si clause = A0 --> A1 ... Ap, sons[i] est l'identifiant du module ds lequel est defini Ai */
{
  SXINT			clause, orig_clause, Aij_pid, son_nb, bot, top, cur, item, son, pos, identical_clause;	
  struct Aij_struct	*Aij_struct_ptr;
  SXBOOLEAN		is_set, is_neg_call;
  SXINT			pid;
  struct G		*sonG;

  clause = orig_clause = *rho++;

  if (clause < 0)
    clause = -clause;

  Aij_pid = *rho++;
  pid = Aij_pid2pid (Aij_pid);
  G = Gs [pid];

  if (is_G_name)
    fprintf (out_file, "%s ", G->name);

  if (is_clause_nb) {
    if (pid != 0)
      fprintf (out_file, "%ld.", (long)pid);

    is_set = G->clause2identical_disp != NULL &&
      ((bot = G->clause2identical_disp [clause]) < (top = G->clause2identical_disp [clause+1]));

    if (is_set)
      fputs ("{", out_file);

    fprintf (out_file, "%ld", (long)orig_clause);

    if (is_set) {
      while (bot < top) {
	identical_clause = G->clause2identical [bot];
	fputs (" ", out_file);
	fprintf (out_file, "%ld", (long)identical_clause);
	bot++;
      }

      fputs ("}", out_file);
    }

    fputs (": ", out_file);
  }


  if (is_lhs) {
    rcg_spf_print_Xpq (out_file, Aij_pid);
  }

  if (is_rhs) {
    fputs (" --> ", out_file);

    son_nb = G->clause2son_nb ? G->clause2son_nb [clause] : 0;

    if (son_nb > 0) {
      for (son = 0; son < son_nb; son++) {
	if (son > 0)
	  fputs (", ", out_file);

	rcg_spf_print_Xpq (out_file, *rho++);
      }

      fputs (" .", out_file);
    }
    else
      fputs ("true .", out_file);
  }

  if (suffix != NULL)
    fprintf (out_file, "%s", suffix);
    
  return SXTRUE; /* Ca peut etre utilise comme de la semantique */
}



SXBOOLEAN
rcg_spf_print_total_instantiated_clause (FILE *out_file, struct G *G, SXINT *rho, char *suffix)
{
  return rcg_spf_print_partial_instantiated_clause (out_file, G, rho, SXTRUE, SXTRUE, SXTRUE, SXTRUE, suffix);
}


SXINT
rcg_spf_print_instantiated_clause_in_outfile (FILE *out_file, struct G *G, SXINT *rho)
{
  rcg_spf_print_total_instantiated_clause (out_file, G, rho, "\n");

  return 1;
}


SXINT
rcg_spf_print_instantiated_clause (struct G *G, SXINT *rho)
{
  rcg_spf_print_instantiated_clause_in_outfile (stdout, G, rho);
  return 1;
}


SXINT
rcg_spf_print_spf_node (struct spf_node *cur_rcg_spf_node_ptr)
{
  rcg_spf_print_instantiated_clause_in_outfile (stdout, cur_rcg_spf_node_ptr->G, &(cur_rcg_spf_node_ptr->rho [0]));
  return 1;
}




static FILE         *rcg_spf_print_forest_out_file;

static SXINT
call_rcg_spf_print_instantiated_clause (struct spf_node *cur_rcg_spf_node_ptr)
{
  rcg_spf_print_instantiated_clause_in_outfile (rcg_spf_print_forest_out_file,
						cur_rcg_spf_node_ptr->G,
						&(cur_rcg_spf_node_ptr->rho [0]));
  return 1;
}

/* Imprime sur out_file la [sous-]foret de racine Aij a partir de Aij vers les feuilles */
void
rcg_spf_print_td_forest (FILE *out_file, SXINT Aij)
{
  rcg_spf_print_forest_out_file = out_file;
  rcg_spf_topological_walk (Aij, call_rcg_spf_print_instantiated_clause, NULL);
}

/* Imprime sur out_file la [sous-]foret de racine Aij des feuilles vers Aij */
void
rcg_spf_print_bu_forest (FILE *out_file, SXINT Aij)
{
  rcg_spf_print_forest_out_file = out_file;
  rcg_spf_topological_walk (Aij, NULL, call_rcg_spf_print_instantiated_clause);
}


SXINT
rcg_spf_print_forest (void)
{
  rcg_spf_print_td_forest (stdout, rcg_spf.start_symbol);

  return 1;
}

/* ***************************************************************************************** */


/* Si is_parse_tree_number */
/* Utilisable soit en parallele avec l'analyse syntaxique, soit au cours
   de la seconde passe (la 1ere passe a utilise store_elem_tree) */
/* calcule le nombre d'arbres de la foret partagee */
/* Peut s'activer, qq soit la semantique associee aux RCG sources par l'option "-ptn" */
#define HUGE_VAL SXINT_MAX

/* Le Jeu 14 Dec 2000 09:46:31, pid est remplace par G pour toutes les fonctions semantiques */
/* Le Mer  5 Jul 2000 11:36:23, suppression du param first_call */
/* Le Mar  7 Nov 2000 14:07:00, pour chaque clause le nombre de fils et chacun de leur pid sont
   stockes ds clause2son_nb et clause2son_pid */

#if PID > 1
static void
tree_count_AijXpid_hd_oflw (SXINT old_size, SXINT new_size)
{
  if (new_size > rcg_spf.cur_AijXxxx_hd_size) {
    rcg_spf.tree_count_disp = (double*) sxrecalloc (rcg_spf.tree_count_disp, old_size+1, new_size+1, sizeof (double));

    /* La "semantique" de la 1ere passe a besoin de connaitre le debordement de AijXpid_hd */
    if (rcg_spf.semact.oflw) (*rcg_spf.semact.oflw)(old_size, new_size);
    
    rcg_spf.cur_AijXxxx_hd_size = new_size;
  }

}
#endif /* PID > 1 */

static void
tree_count_in_first_pass_mem_mngr (struct G *G, SXINT old_size, SXINT new_size)
{
  static SXINT total_new_size, total_old_size;

  if (old_size == 0) {
    /* ALLOCATION */
    /* Le decompte des arbres se fait a` la construction */
    /* Ce n'est pas le decompte exact car certains arbres decomptes peuvent ne pas
       etre relies a l'axiome !! */
    if (G->pid == 0)
      total_new_size = new_size;
    else
      total_new_size += new_size;
      
    if (G->pid == PID-1) {
#if PID > 1
      XxY_alloc (&rcg_spf.AijXpid_hd, "rcg_spf.AijXpid_hd", total_new_size+1, 1, 0, 0,
		 tree_count_AijXpid_hd_oflw, statistics);
      rcg_spf.cur_AijXxxx_hd_size = XxY_size (rcg_spf.AijXpid_hd);
#else /* PID > 1 */
      rcg_spf.cur_AijXxxx_hd_size = X_size (*(G->prhoA_hd));
#endif /* PID > 1 */
      rcg_spf.tree_count_disp = (double*) sxcalloc (rcg_spf.cur_AijXxxx_hd_size+1, sizeof (double));
#if 0
      /* Il se peut (cas is_dag et grammaire non lineaire) que RCG_parser ait besoin de structures supplementaires */
      rcg_more_spf_alloc ();
#endif /* 0 */
    }
  }
  else {
    if (new_size == 0) {
      /* FREE */
      if (rcg_spf.tree_count_disp)
	sxfree (rcg_spf.tree_count_disp), rcg_spf.tree_count_disp = NULL;

#if PID > 1
      if (XxY_is_allocated (rcg_spf.AijXpid_hd))
	XxY_free (&rcg_spf.AijXpid_hd);
#endif /* PID > 1 */
      rcg_spf.cur_AijXxxx_hd_size = 0;
    }
    else {
      /* REALLOCATION */
      /* Rien */
    }
  }
}


#if 0
static void
tree_count_alloc ()
{
  rcg_spf.tree_count_disp = (double*) sxcalloc (XxY_size (rcg_spf.AijXpid_hd)+1, sizeof (double));
}

static void
tree_count_free ()
{
  sxfree (rcg_spf.tree_count_disp), rcg_spf.tree_count_disp = NULL;
}
#endif /* 0 */

static void
tree_count_loop (struct G *G, SXINT *rho, SXINT cycle_kind)
{
    /* Le calcul de toutes les instantiations des Arho-clauses est termine, au moins une
       clause valide est impliquee ds un cycle (ou une boucle) */
    /* Le nombre d'arbres (valides) issus de Arho est donc infini */
    rcg_spf.tree_count_disp [rho [1]] = HUGE_VAL;

    process_loop (G, rho, NULL, cycle_kind);
}

SXINT
rcg_spf_tree_count_action (struct G *G, SXINT *rho/*, son_nb, sons[], first_call */)
    /* rho est un tableau de taille son_nb+2
       rho[0] contient le numero p de la clause courante.  Si p est <0 la clause est -p et
       l'appel est cyclique
       rho[1] contient l'identifiant du vecteur de range de la LHS
       rho[2], ..., rho[son_nb+1] contient les identifiants des vecteurs de range de la RHS */
    /* sem_disp est un tableau global de taille PID dont chaque element est un pointeur
       vers un tableau local a chaque module ds lequel est stocke la semantique (element
       de type SEM_TYPE, c'est une union) */

    /* sons est un tableau de taille sons_nb+1, il matche clause.
       Si clause = A0 --> A1 ... Ap, sons[i] est l'identifiant du module ds lequel est defini Ai */

    /* Si first_call==-1, c'est la premiere fois qu'on fait une reduc vers (A, rho[0]), clause est une A_clause */
{
  SXINT		Xpq_pid, son, clause, son_nb, bot, top, Aij;
  double	nb;

#if LOG
  rcg_spf_print_instantiated_clause (G, rho);
#endif /* LOG */

  clause = *rho++;
  Aij = *rho;

#if EAL_WITH_GUIDE
 {
#include <string.h>

   struct Aij_struct   *Aij_struct_ptr;
   SXINT               length;

   Aij_struct_ptr = call_instantiated_prdct2struct (Aij);

   length = strlen(Aij_struct_ptr->Astr);

   if (length > 4 && strncmp(Aij_struct_ptr->Astr + length - 4, "__NF", 4) == 0) {
#if EBUG
     printf("%s-instantiated clause (instance of clause #%ld) not stored in the parse forest\n", Aij_struct_ptr->Astr, (long)clause);
#endif /* EBUG */
     
     rcg_spf.tree_count_disp [Aij] = 1;

     return 1 ;
   }
 }
#endif /* EAL_WITH_GUIDE */

  son_nb = G->clause2son_nb ? G->clause2son_nb [clause] : 0;
  nb = 1;

  for (son = son_nb; son > 0; son--) {
    if ((Xpq_pid = rho [son]) > 0) {
      /* si son_pid < 0 => appel de &True */
      /* Si rhoi < 0 => appel de &StrEq ("a", X) et -rhoi est l'indice de "a"
	 ds le source
	 ou (le 09/02/04) appel negatif qui a marche, on peut donc sauter */
      /* A VOIR : comment differencier les appels negatifs des appels de &StrEq ("a", X) !! */
      nb *= rcg_spf.tree_count_disp [Xpq_pid];
    }
  }

  if (G->clause2identical_disp != NULL &&
      ((bot = G->clause2identical_disp [clause]) < (top = G->clause2identical_disp [clause+1]))) {
    nb *= top-bot+1;
    rcg_spf.max_instantiated_clause_nb += top-bot;
  }

  rcg_spf.tree_count_disp [Aij] += nb;

  return 1;
}


static SXINT
tree_count_post ()
{
  /* ret_val different de -1 ssi l'analyse a marche' */
  /* S0n est l'identifiant du couple axiome, (vecteur de) range [0..n] */
  double		nb;

  if (rcg_spf.start_symbol != 0 && rcg_spf.sem_disp != NULL && GET_RCG_SPF_HEAD_NODE_PTR (rcg_spf.start_symbol) != NULL) {
    nb = rcg_spf.tree_count_disp [rcg_spf.start_symbol];

    if (nb == 1)
      fprintf (stdout, "%% This #%ld transition input DAG is non ambiguous.\n", (long)idag.ptq_nb);
    else {
      if (nb < 1.0E9)
	fprintf (stdout, "%% There are %.f trees in the shared parse forest of this #%ld transition input DAG.\n", nb, (long)idag.ptq_nb);
      else {
	if (nb == HUGE_VAL)
	  fprintf (stdout, "%% The shared parse forest of this #%ld transition input DAG is cyclic.\n", (long)idag.ptq_nb);
	else
	  fprintf (stdout, "%% There are %e trees in the shared parse forest of this #%ld transition input DAG.\n", nb, (long)idag.ptq_nb);
      }
    }
  
    return 1;
  }
  else {
    fprintf (stdout, "This #%ld transition input DAG is not a sentence.\n", (long)idag.ptq_nb);
    return -1;
  }
}

static SXINT
rcg_spf_tree_count_call_action (struct spf_node *cur_rcg_spf_node_ptr)
{
#if LOG
  printf ("spf [%ld] = ", (long)cur_rcg_spf_node_ptr->instantiated_clause_nb);
  /* rcg_spf_print_instantiated_clause (G, rho); sera fait ds rcg_spf_tree_count_action */
#endif /* LOG */

  return rcg_spf_tree_count_action (cur_rcg_spf_node_ptr->G, &(cur_rcg_spf_node_ptr->rho [0]));
}

SXINT
rcg_spf_tree_count (void)
{
  if (rcg_spf_topological_bottom_up_walk (rcg_spf.start_symbol, rcg_spf_tree_count_call_action, NULL, NULL))
    return 1;

  return -1;
}

void
rcg_spf_tree_count_in_first_pass_begins ()
{
  SXINT pid;

  /* Evaluation du nombre d'arbres pendant la 1ere passe en // avec l'analyse syntaxique */

  /* On change les affectations par defaut */
    for (pid = 0; pid < PID; pid++) {
      Gs [pid]->semact.first_pass_init = tree_count_in_first_pass_mem_mngr;
      Gs [pid]->semact.first_pass = rcg_spf_tree_count_action;
      Gs [pid]->semact.first_pass_loop = tree_count_loop;
      Gs [pid]->semact.first_pass_final = tree_count_in_first_pass_mem_mngr;
      Gs [pid]->semact.oflw = tree_count_in_first_pass_mem_mngr;
    }

    rcg_spf.semact.oflw = NULL;
    rcg_spf.semact.second_pass_init = NULL;
    rcg_spf.semact.second_pass = NULL;
    rcg_spf.semact.second_pass_final = NULL;

    rcg_spf.semact.last_pass = tree_count_post;
}

void
rcg_spf_tree_count_begins ()
{
  /* Evaluation du nombre d'arbres sur rcg_spf (reduite) */

    rcg_spf.semact.oflw = NULL;

    rcg_spf.semact.second_pass_init = NULL; //tree_count_alloc;
    rcg_spf.semact.second_pass = NULL;
    rcg_spf.semact.second_pass_final = NULL;
    rcg_spf.semact.last_pass = rcg_spf_print_tree_count;
}

void
rcg_spf_print_forest_begins ()
{
  /* Evaluation du nombre d'arbres sur rcg_spf (reduite) */

    rcg_spf.semact.oflw = NULL;

    rcg_spf.semact.second_pass_init = NULL; //tree_count_alloc;
    rcg_spf.semact.second_pass = NULL;
    rcg_spf.semact.second_pass_final = NULL;
    rcg_spf.semact.last_pass = rcg_spf_print_forest;
}

/* ***************************************************************************************** */


/* Procedure qui permet de stocker les arbres elementaires au fur et a mesure de leur
   reconnaissance et de fabriquer ainsi la foret partagee d'analyse qui sera utilisee
   au cours de la seconde passe */

static struct area_hd	spf_area_hd;

void
rcg_spf_spf_alloc (struct G *G, SXINT old_size, SXINT new_size)
{
  static SXINT total_new_size;

#if EBUG
  if (old_size != 0)
    sxtrap (ME, "instantiated");
#endif /* EBUG */

  /* ALLOCATION */
  if (G->pid == 0) {
    /* Le parseur principal appelle "rcg_spf_spf_alloc" en premier */
    total_new_size = new_size;
    /* On en profite pour initialiser qq variables qui peuvent etre utilisees par la suite */
    glbl_t2string = idag_get_t2string ();
    eof_Tpq_code = MAX_pqt () + 1;
  }
  else
    total_new_size += new_size;

  if (G->pid == PID-1) {
    if (spf_area_hd.bot == NULL) {
      spf_area_hd.size = new_size*sizeof(struct spf_node);/* Pourquoi pas */
      rcg_spf_allocate_area (&spf_area_hd);
    }

#if PID > 1
    XxY_alloc (&rcg_spf.AijXpid_hd, "rcg_spf.AijXpid_hd", total_new_size+1, 1, 0, 0, AijXpid_hd_oflw, statistics);
    rcg_spf.cur_AijXxxx_hd_size = XxY_size (rcg_spf.AijXpid_hd);
#else /* PID > 1 */
    rcg_spf.cur_AijXxxx_hd_size = X_size (*(G->prhoA_hd));
#end /* PID > 1 */
    rcg_spf.sem_disp = (SEM_TYPE*) sxcalloc (rcg_spf.cur_AijXxxx_hd_size+1, sizeof (SEM_TYPE));

#if 0
    /* Il se peut (cas is_dag et grammaire non lineaire) que RCG_parser ait besoin de structures supplementaires */
    rcg_more_spf_alloc ();
#endif /* 0 */
  }
}

/* Appel direct depuis RCG_parser.c */
void
rcg_spf_spf_free (struct G *G, SXINT old_size, SXINT new_size)
{
  /* On libere tout rcg_spf */
  if (G->pid == 0) {
    if (spf_area_hd.bot)
      rcg_spf_free_area (&spf_area_hd);

    if (rcg_spf.sem_disp) {
#if PID > 1
      XxY_free (&rcg_spf.AijXpid_hd);
#endif /* PID > 1 */
      cur_AijXxxx_hd_size = 0;
      sxfree (rcg_spf.sem_disp), rcg_spf.sem_disp = NULL;

      if (rcg_spf.Aij2rhs_nb) sxfree (rcg_spf.Aij2rhs_nb), rcg_spf.Aij2rhs_nb = NULL;
      if (rcg_spf.tree_count.iclause2nb) sxfree (rcg_spf.tree_count.iclause2nb), rcg_spf.tree_count.iclause2nb = NULL;
      if (rcg_spf.tree_count.nt2nb) sxfree (rcg_spf.tree_count.nt2nb), rcg_spf.tree_count.nt2nb = NULL;
      if (rcg_spf.walk.Aij2nyp_nb) sxfree (rcg_spf.walk.Aij2nyp_nb), rcg_spf.walk.Aij2nyp_nb = NULL;
      if (rcg_spf.walk.Aij_stack) sxfree (rcg_spf.walk.Aij_stack), rcg_spf.walk.Aij_stack = NULL;
      if (rcg_spf.walk.iclause2spf_node) sxfree (rcg_spf.walk.iclause2spf_node), rcg_spf.walk.iclause2spf_node = NULL;
      if (rcg_spf.walk.Aij_set) sxfree (rcg_spf.walk.Aij_set), rcg_spf.walk.Aij_set = NULL;
      if (rcg_spf.walk.cyclic_Aij_set) sxfree (rcg_spf.walk.cyclic_Aij_set), rcg_spf.walk.cyclic_Aij_set = NULL;
      if (rcg_spf.walk.invalid_Aij_set) sxfree (rcg_spf.walk.invalid_Aij_set), rcg_spf.walk.invalid_Aij_set = NULL;
      if (rcg_spf.walk.status) sxfree (rcg_spf.walk.status), rcg_spf.walk.status = NULL;
    }
  }
}

void
rcg_spf_store_instantiated_clause_in_spf_loop (struct G *G, SXINT *rho, SXINT cycle_kind)
{
    /* Arho = rho [1]
       Le calcul de toutes les instantiations des Arho-clauses est termine, au moins une
       clause valide est impliquee ds un cycle (ou une boucle) */
    /* rho [0] est une A-clause */

    /* G->pass_no == 1 */
    /* GET_SEM_HAS_LOOP (G->pid, rho [1]) = 1; */
    process_loop (G, rho, NULL, cycle_kind);
}

/* Attention rho peut etre suffixee par une Tpq_stack qui doit aussi etre stockee
   ds la spf */
SXINT
rcg_spf_store_instantiated_clause_in_spf (struct G *G, SXINT *rho)
{    
  SXINT		        *p, i, nb, clause, son_nb, bot, top, Aij;
  struct spf_node	*node_ptr, **disp_ptr;

#if EBUG
  if (rho [0] <= 0 /* clause */ || rho [1] <= 0 /* Aij */)
    sxtrap (ME, "rcg_spf_store_instantiated_clause_in_spf");

  if (G->clause2son_nb) {
    for (i = G->clause2son_nb [rho [0]]; i >= 1; i--) {
      if ((rho+1) [i] <= 0)
	sxtrap (ME, "rcg_spf_store_instantiated_clause_in_spf");
    }
  }
#endif /* EBUG */

  rcg_spf.max_instantiated_clause_nb++;

#if LOG
  printf ("spf [%ld] = ", (long)rcg_spf.max_instantiated_clause_nb);
  rcg_spf_print_instantiated_clause (G, rho);
#endif /* LOG */

  clause = *rho++;
  Aij = *rho++;

#if EAL_WITH_GUIDE
  {
#include <string.h>

    struct Aij_struct	*Aij_struct_ptr;
    SXINT               length;

    Aij_struct_ptr = call_instantiated_prdct2struct (Aij);

    length = strlen(Aij_struct_ptr->Astr);

    if (length > 4 && strncmp(Aij_struct_ptr->Astr + length - 4, "__NF", 4) == 0) {
#if EBUG
      printf("%s-instantiated clause (instance of clause #%ld) not stored in the parse forest\n", Aij_struct_ptr->Astr , (long)clause);
#endif /* EBUG */
      return 1 ;
    }
  }
#endif /* EAL_WITH_GUIDE */

  if (G->clause2identical_disp != NULL &&
      ((bot = G->clause2identical_disp [clause]) < (top = G->clause2identical_disp [clause+1]))) {
    rcg_spf.max_instantiated_clause_nb += top-bot;
  }

  son_nb = G->clause2son_nb ? G->clause2son_nb [clause] : 0;
  son_nb += 1+rho [son_nb] /* taille de la Tpq_stack */;

  node_ptr = rcg_spf_book_area (&spf_area_hd, son_nb+2);

  if (*(disp_ptr = &GET_RCG_SPF_TAIL_NODE_PTR (Aij)) == NULL)
    GET_RCG_SPF_HEAD_NODE_PTR (Aij) = node_ptr;
  else
    (*disp_ptr)->next = node_ptr;

  node_ptr->next = NULL;
  *disp_ptr = node_ptr;

  node_ptr->G = G;
  node_ptr->instantiated_clause_nb = rcg_spf.max_instantiated_clause_nb;

  p = &(node_ptr->rho [0]);

  *p++ = clause;
  *p++ = Aij;

  for (i = 0; i < son_nb; i++) {
    /* rcg_spf.Aij2rhs_nb n'est pas alloue' il sera rempli + tard au de'but de la seconde passe */
    *p++ = *rho++;
  }
    
  return 1;
}

/* On vient de construire la spf (par rcg_spf_store_instantiated_clause_in_spf)
   GET_RCG_SPF_TAIL_NODE_PTR () ne peut plus etre utilise'
   Le champ top_node_ptr de rcg_spf.sem_disp change de signification
   il sera utilise par GET_RCG_SPF_ERASED_HEAD_NODE_PTR () pour contenir la liste des noeuds
   "supprime's" de la foret */
void
rcg_spf_spf_completes (void)
{
  SXINT Aij;

#if PID > 1
  rcg_spf.max_instantiated_prdct_nb = XxY_top (rcg_spf.AijXpid_hd);
#else /* PID > 1 */
  rcg_spf.max_instantiated_prdct_nb = X_top (*(Gs[0]->prhoA_hd));
#endif /* PID > 1 */

  for (Aij = rcg_spf.max_instantiated_prdct_nb; Aij >= 0; Aij--)
    GET_RCG_SPF_ERASED_HEAD_NODE_PTR (Aij) = NULL;

  rcg_spf.Aij2rhs_nb = (SXINT*) sxalloc (rcg_spf.max_instantiated_prdct_nb+1, sizeof (SXINT));

  /* On rend la spf connexe */
  if (!rcg_spf_make_proper (rcg_spf.start_symbol))
    /* Il ne reste rien !! */
    sxtrap (ME, "rcgparse_it (empty shared parse forest)");
}


/* ************************************************************************************ */


/* Les noeuds de la foret a eliminer sont stockes ds spf_node_stack */
SXBOOLEAN
rcg_spf_erase (struct spf_node **spf_node_stack)
{
  SXINT     i, clause, stack_top;
  struct    spf_node *cur_rcg_spf_node_ptr;
  SXBOOLEAN ret_val;

#if LOG
  fputs ("*** Entering rcg_spf_erase ***\n", stdout);
#endif /* LOG */

  stack_top = (SXINT) spf_node_stack [0];

  for (i = 1; i <= stack_top; i++) {
    cur_rcg_spf_node_ptr = spf_node_stack [i];

    if ((clause = cur_rcg_spf_node_ptr->rho [0]) > 0) {
      rcg_spf.is_proper = SXFALSE;
      cur_rcg_spf_node_ptr->rho [0] = -clause;
    }
  }

  if (!rcg_spf.is_proper)
    ret_val = rcg_spf_make_proper (rcg_spf.start_symbol /* global */);
  else
    ret_val = SXTRUE;

#if LOG
  fputs ("*** Leaving rcg_spf_erase ***\n", stdout);
#endif /* LOG */

  return ret_val;
}

/* ************************************************************************************ */
static SXBA         count_Aij_set;
static SXINT        count_maxclause, count_maxG;



static SXINT
rcg_spf_tree_count_pass_derived (struct spf_node *cur_rcg_spf_node_ptr)
{
  double     tree_count = (double)1;
  SXINT      Xpq, Aij, clause, iclause, son_nb, son;
  SXINT      *rho;

  iclause = cur_rcg_spf_node_ptr->instantiated_clause_nb;
  rho = &(cur_rcg_spf_node_ptr->rho [0]);

#if LLOG
    printf ("%ld: ", count_maxclause);
    rcg_spf_print_spf_node (cur_rcg_spf_node_ptr);
#endif /* LLOG */

  clause = *rho++;
  Aij = rho [0];

  if (count_Aij_set) {
    /* On veut des stats completes sur la grammaire */
    count_maxclause++;
  
    SXBA_1_bit (count_Aij_set, Aij);
  }

  son_nb = cur_rcg_spf_node_ptr->G->clause2son_nb [clause];
  count_maxG += 1+son_nb;

  if (son_nb) {
    for (son = 1; son <= son_nb; son++) {
      Xpq = rho [son];

      if (Xpq > 0) {
	/* Appel positif */
	tree_count *= rcg_spf.tree_count.nt2nb [Xpq];
      }
    }
  }

  rcg_spf.tree_count.iclause2nb [iclause] = rcg_spf.tree_count.nt2nb [Aij] += tree_count;
  /* Cumule ds iclause2nb les nb associes aux prods precedentes */
  /* ... c,a permet de la recherche dichotomique ds rcg_spf_dag2tree */
  
  return 1;
}



SXBOOLEAN
rcg_spf_set_tree_count (double *count)
{
  SXINT i, pid, size;

  /* A partir du 13/01/06 Les cycles eventuels ds la foret ont ete elimines */
  if (rcg_spf.start_symbol == 0 /* || rcg_spf.inputG.has_cycles */) {
    *count = 0L;
    return SXFALSE;
  }

  if (rcg_spf.tree_count.iclause2nb == NULL) {
    rcg_spf.tree_count.iclause2nb = (double *) sxcalloc (rcg_spf.max_instantiated_clause_nb+1, sizeof (double));
    rcg_spf.tree_count.nt2nb = (double *) sxcalloc ((SXUINT)rcg_spf.max_instantiated_prdct_nb+1, sizeof (double));
  }
  else {
    size = (rcg_spf.max_instantiated_clause_nb+1)*sizeof (double);
    memset ((char*) rcg_spf.tree_count.iclause2nb, 0, size);

    size = (rcg_spf.max_instantiated_prdct_nb+1)*sizeof (double);
    memset ((char*) rcg_spf.tree_count.nt2nb, 0, size);
  }

  rcg_spf_topological_walk (rcg_spf.start_symbol, NULL, rcg_spf_tree_count_pass_derived);

  *count = rcg_spf.tree_count.nt2nb [rcg_spf.start_symbol];

  return SXTRUE;
}


/* On "compte" les elements de la grammaire apres du filtrage */
double
rcg_spf_print_signature (FILE *out_file)
{
  double            tree_count;

  count_maxclause = count_maxG;

  count_Aij_set = sxba_calloc (rcg_spf.max_instantiated_prdct_nb+1);

#if LOG
  fputs ("\nrcg_spf_print_signature: pi = count_td_walk\n", stdout);
#endif /* LOG */

  rcg_spf_set_tree_count (&tree_count);

  fprintf (out_file, "(|N|=%li, |P|=%ld, S=%ld, |outpuG|=%ld, TreeCount=", 
	   (long)(sxba_cardinal (count_Aij_set)), (long)count_maxclause, (long)rcg_spf.start_symbol,
    (long)count_maxG);

  if (tree_count < 1.0E9)
    fprintf (out_file, "%.f)\n", tree_count);
  else
    fprintf (out_file, "%e)\n", tree_count);

  sxfree (count_Aij_set), count_Aij_set = NULL;

  return tree_count;
}



SXINT
rcg_spf_print_tree_count (void)
{
  double val;

  /* Le 14/09/06 j'utilise rcg_spf_print_signature car la rcg_spf a pu etre
     reduite (cycle, erreurs, ...) */
  fputs ("Shared Parse Forest = ", stdout);

  val = rcg_spf_print_signature (stdout);

  return 1;
}


/* ************************************************************************************ */
/* calcule rcg_spf.Aij2rhs_nb */
static SXBOOLEAN
make_proper_second_td_walk (SXINT Aij)
{
  SXINT             Xpq, clause, pid, son, son_nb;
  SXINT             *rho;
  struct spf_node   *cur_rcg_spf_node_ptr;
  SXBOOLEAN         retval = SXFALSE;

  /* on n'est appelés qu'une seule fois par Xpq grâce au contrôle sur rcg_spf.Aij2rhs_nb */
  for (cur_rcg_spf_node_ptr = GET_RCG_SPF_HEAD_NODE_PTR (Aij);
       cur_rcg_spf_node_ptr != NULL;
       cur_rcg_spf_node_ptr = cur_rcg_spf_node_ptr->next) {
    rho = &(cur_rcg_spf_node_ptr->rho [0]);
    clause = *rho++;
    pid = Aij_pid2pid (Aij);
    son_nb = Gs [pid]->clause2son_nb ? Gs [pid]->clause2son_nb [clause] : 0;
	
    for (son = 1; son <= son_nb; son++) {
      Xpq = rho [son];

      if (Xpq > 0 && SXBA_bit_is_set (rcg_spf.walk.invalid_Aij_set, Xpq))
	break;
    }

    if (son <= son_nb) {
      /* Il est invalide, on le vire */
      rho [-1] = -clause;
    }
    else { /* aucun des symboles de rhs n'est invalide: on la garde */
      rcg_spf.cur_max_instantiated_clause_nb++;

      retval = SXTRUE; /* au moins une Pij de Aij a donc une rhs valide: on a au moins un sous-arbre */

#if LLLOG
      fputs ("make_proper_second_td_walk: ", stdout);
      rcg_spf_print_total_instantiated_clause (stdout, cur_rcg_spf_node_ptr->G, &(cur_rcg_spf_node_ptr->rho [0]), (son <= son_nb) ? "(erased)\n" : "(kept)\n");
#endif /* LLOG */
	
      for (son = 1; son <= son_nb; son++) {
	Xpq = rho [son];

	if (Xpq > 0 && Xpq != rcg_spf.start_symbol && rcg_spf.Aij2rhs_nb [Xpq]++ == 0)
	  make_proper_second_td_walk (Xpq);
      }
    }
  }

  return retval; /* valeur utilisée seulement à l'extérieur, càd sur l'axiome */
}


#if EBUG
static SXBA         rcg_spf_make_proper_check_td_set [PID], rcg_spf_make_proper_check_bu_set [PID];


static SXINT
rcg_spf_make_proper_check_td (struct spf_node *rcg_spf_node_ptr)
{
  SXINT clause, pid;
  SXINT *rho;

  rho = &(rcg_spf_node_ptr->rho [0]);
  clause = *rho++;
  pid = Aij_pid2pid (*rho);

  SXBA_1_bit (rcg_spf_make_proper_check_td_set [pid], clause);

  return 1;
}

static SXINT
rcg_spf_make_proper_check_bu (struct spf_node *rcg_spf_node_ptr)
{
  SXINT clause, pid;
  SXINT *rho;

  rho = &(rcg_spf_node_ptr->rho [0]);
  clause = *rho++;
  pid = Aij_pid2pid (*rho);

  SXBA_1_bit (rcg_spf_make_proper_check_bu_set [pid], clause);

  return 1;
}



static void
rcg_spf_check_consistency (void)
{
  SXINT clause, pid;

#if LOG
  fputs ("*** Entering rcg_spf_check_consistency\n", stdout);
#endif /* LOG */

  /* On verifie la coherence de la nelle foret */
  for (pid = 0; pid < PID; pid++) {
    rcg_spf_make_proper_check_td_set [pid] = sxba_calloc (Gs [pid]->clause_nb+1);
    rcg_spf_make_proper_check_bu_set [pid] = sxba_calloc (Gs [pid]->clause_nb+1);
  }

  rcg_spf_topological_walk (rcg_spf.start_symbol, rcg_spf_make_proper_check_td, NULL);
  rcg_spf_topological_walk (rcg_spf.start_symbol, NULL, rcg_spf_make_proper_check_bu);

  /* On verifie la coherence de la nelle foret */
  for (pid = 0; pid < PID; pid++) {
    if ((clause = sxba_first_difference (rcg_spf_make_proper_check_td_set [pid],
					 rcg_spf_make_proper_check_bu_set [pid])) != -1)
      sxtrap (ME, "rcg_spf_check_consistency (inconsistent forest)");
  }

  for (pid = 0; pid < PID; pid++) {
    sxfree (rcg_spf_make_proper_check_td_set [pid]), rcg_spf_make_proper_check_td_set [pid] = NULL;
    sxfree (rcg_spf_make_proper_check_bu_set [pid]), rcg_spf_make_proper_check_bu_set [pid] = NULL;
  }

#if LOG
  fputs ("*** Leaving rcg_spf_check_consistency\n", stdout);
#endif /* LOG */
}
#endif /* EBUG */

static SXINT
rcg_make_proper_keep_valid_Aijs (SXINT Aij, SXBOOLEAN must_be_kept)
{
  if (must_be_kept)
    SXBA_0_bit (rcg_spf.walk.invalid_Aij_set, Aij);

  return must_be_kept ? 1 : -1;
}

/* Nelle version du 03/07/07 */
/* PB : Les parties de la foret non visitees a cause de user_filtering_function ne doivent pas etre supprimees !! (voir la solution) */
/* Elle peut etre appelee par l'utilisateur */
/* Elle "desactive" toutes les productions inaccessibles (grammaire pas propre et/ou qq Pij mis en negatif)
   et recalcule Aij2rhs_nb pour les passes heritees eventuelles */
SXBOOLEAN
rcg_spf_make_proper (SXINT root_Aij)
{
  SXINT             Xpq, clause;
  SXINT             *rho;
  struct spf_node   *cur_rcg_spf_node_ptr;
  SXBOOLEAN         ret_val;

  sxuse(root_Aij); /* pour faire taire gcc -Wunused */
#if LOG
  fputs ("*** Entering rcg_spf_make_proper ()\n", stdout);
#endif /* LOG */

  rcg_spf.cur_max_instantiated_clause_nb = 0;

  if (rcg_spf.walk.Aij_set == NULL)
    rcg_spf.walk.Aij_set = sxba_calloc (rcg_spf.max_instantiated_prdct_nb+1);
  else
    sxba_empty (rcg_spf.walk.Aij_set);

  if (rcg_spf.walk.invalid_Aij_set == NULL)
    rcg_spf.walk.invalid_Aij_set = sxba_calloc (rcg_spf.max_instantiated_prdct_nb+1);

  sxba_fill (rcg_spf.walk.invalid_Aij_set);

  rcg_spf.walk.pass_inherited = NULL;
  rcg_spf.walk.pass_derived = sxivoid_int;
  rcg_spf.walk.pre_pass = NULL;
  rcg_spf.walk.post_pass = rcg_make_proper_keep_valid_Aijs;

  if ((ret_val = rcg_spf_call_walk_core (rcg_spf.start_symbol)))
    SXBA_0_bit (rcg_spf.walk.invalid_Aij_set, rcg_spf.start_symbol);


  for (Xpq = 1; Xpq <= rcg_spf.max_instantiated_prdct_nb; Xpq++)
    rcg_spf.Aij2rhs_nb [Xpq] = 0;
  
  /* Attention : Soit A -> B C ; telle que make_proper_first_td_walk a trouve' que
     B est non invalide et que C est invalide.  Donc, si c'est la seule A_prod, A est aussi trouve' invalide.
     make_proper_second_td_walk (A) dit que A -> B C ; est invalidee (et ne descend pas ds ses fils).  Si la seule occur de B
     est ds A -> B C ; alors B reste non invalide alors que les B_prods doivent etre supprimees (avec eventuellement les descendants).
     On ne peut donc pas utiliser rcg_spf.walk.invalid_Aij_set pour connaitre exactement les Aij invalides.
     On va utiliser rcg_spf.Aij2rhs_nb */
  for (Xpq = (make_proper_second_td_walk (rcg_spf.start_symbol) ? 2 : 1); /* make_proper_second_td_walk
									       rend SXTRUE ss'il reste
									       au moins 1 arbre:
									       dans ce cas on
									       n'efface pas l'axiome
									       ; sinon on l'efface,
									       comme tous les
									       autres, puisque son
									       Aij2rhs_nb est
									       évidemment nul */
       Xpq <= rcg_spf.max_instantiated_prdct_nb;
       Xpq++) {
    if (rcg_spf.Aij2rhs_nb [Xpq] == 0) {
      rcg_spf_erase_all_Aij_clauses (&(GET_RCG_SPF_HEAD_NODE_PTR (Xpq)),
				     &(GET_RCG_SPF_ERASED_HEAD_NODE_PTR (Xpq)));
    }
  }

  rcg_spf.is_proper = SXTRUE;

#if LOG
  fputs ("*** Leaving rcg_spf_make_proper()\n", stdout);
#endif /* LOG */

#if EBUG
  rcg_spf_check_consistency ();
#endif /* EBUG */

#if LOG
  if (!ret_val)
    fputs ("*** WARNING: the whole forest has vanished!! ***\n", stdout);
#endif /* LOG */

  return ret_val;
}

/* ************************************************************************************ */

/* Extrait de rcg_spf le p ieme sous-arbre enracine' en Aij
   (et appelle la fonction utilisateur f) */
SXINT
rcg_spf_dag2tree (SXUINT p,
		  SXINT Aij,
		  SXINT (*f)(struct spf_node *))
{
  SXUINT          Xpq_tree_count, cur_lr_subtree_nb, prev_lr_subtree_nb;
  SXINT           son, son_nb, pid, Xpq, clause, iclause;
  SXINT           *rho;
  struct spf_node *cur_rcg_spf_node_ptr;

  prev_lr_subtree_nb = 0;
  pid = Aij_pid2pid (Aij);

  for (cur_rcg_spf_node_ptr = GET_RCG_SPF_HEAD_NODE_PTR (Aij);
       cur_rcg_spf_node_ptr != NULL;
       cur_rcg_spf_node_ptr = cur_rcg_spf_node_ptr->next) {
    iclause = cur_rcg_spf_node_ptr->instantiated_clause_nb;
    cur_lr_subtree_nb = rcg_spf.tree_count.iclause2nb [iclause];

    if (p == cur_lr_subtree_nb) {
      cur_lr_subtree_nb -= prev_lr_subtree_nb;
	
      break;
    }

    if (p < cur_lr_subtree_nb)
      sxtrap (ME, "rcg_spf_dag2tree");

    prev_lr_subtree_nb = cur_lr_subtree_nb;
  }

  if (cur_rcg_spf_node_ptr) {
    /* On decompose p sur les fils non-terminaux de prod */
    rho = &(cur_rcg_spf_node_ptr->rho [0]);
    clause = *rho++;
    son_nb = cur_rcg_spf_node_ptr->G->clause2son_nb [clause];
	    
    for (son = 1; son <= son_nb; son++) {
      Xpq = rho [son];

      if (Xpq > 0) {
	/* Appel positif */
	if (rcg_spf.tree_count.nt2nb [Xpq] > (double)(ULONG_MAX))
	  sxtrap (ME, "rcg_spf_dag2tree (this function can not be called on a forest containing more than ULONG_MAX trees)");

	Xpq_tree_count = (SXUINT)rcg_spf.tree_count.nt2nb [Xpq];

	if (!rcg_spf_dag2tree (cur_lr_subtree_nb%Xpq_tree_count, Xpq, f))
	  return SXFALSE;

	cur_lr_subtree_nb /= Xpq_tree_count;
      }
    }
  
    return (*f) (cur_rcg_spf_node_ptr); /* Appel de la semantique, elle peut ranger ses infos en // avec les Aij... */
  }

  return -1;
}

SXINT
rcg_spf_unfold (void)
{
  double double_tree_count;
  SXUINT tree_count, tree_nb;

  if (rcg_spf_set_tree_count (&double_tree_count)) {
    tree_count = (SXUINT) double_tree_count;

    for (tree_nb = 0; tree_nb < tree_count; tree_nb++) {
      printf ("\nParse tree #%lu\n", tree_nb);
      rcg_spf_dag2tree (tree_nb, rcg_spf.start_symbol, rcg_spf_print_spf_node);
    }

    return 1;
  }
      
  fputs ("\nThe parse forest is empty.\n", stdout);

  return 0;
}


/* ************************************************************************************ */


SXBA
rcg_spf_save_status (SXBA save_set)
{
  if (save_set == NULL)
    /* C'est l'utilisateur qui se charge du free */
    save_set = sxba_calloc (rcg_spf.max_instantiated_prdct_nb+1);

  sxba_copy (save_set, rcg_spf.walk.status);

  return save_set;
}

void
rcg_spf_restore_status (SXBA save_set)
{
  sxba_copy (rcg_spf.walk.status, save_set);
}



/* On se souvient de l'etat courant de la rcg_spf.  Cet etat pourra etre restaure' par rcg_spf_pop_status () */
void
rcg_spf_push_status (void)
{
  SXINT        Aij, iclause;
  struct spf_node   *cur_rcg_spf_node_ptr;

  if (rcg_spf.walk.status == NULL) {
    rcg_spf.walk.status = sxba_calloc (rcg_spf.max_instantiated_prdct_nb+1);
    rcg_spf.walk.iclause2spf_node = (struct spf_node **) sxalloc (rcg_spf.max_instantiated_prdct_nb+1,
								  sizeof (struct spf_node *));
  }
  else
    sxba_empty (rcg_spf.walk.status);
      
  for (Aij = 1; Aij <= rcg_spf.max_instantiated_prdct_nb; Aij++) {
    for (cur_rcg_spf_node_ptr = GET_RCG_SPF_HEAD_NODE_PTR (Aij);
	 cur_rcg_spf_node_ptr != NULL;
	 cur_rcg_spf_node_ptr = cur_rcg_spf_node_ptr->next) {
      iclause = cur_rcg_spf_node_ptr->instantiated_clause_nb;
      /* On marque les bonnes */
      SXBA_1_bit (rcg_spf.walk.status, iclause);
      rcg_spf.walk.iclause2spf_node [iclause] = cur_rcg_spf_node_ptr;
    }
  }
}


/* On suppose que l'etat precedent et que l'etat courant sont coherents :
   contient les bonnes valeurs */
/* De plus on suppose que l'etat courant est une sous-foret de l'etat precedent (des Pij ont ete vires) */
void
rcg_spf_pop_status (void)
{
  SXINT             iclause, clause, Aij, Xpq, son_nb, son;
  SXINT             *rho;
  struct spf_node   *cur_rcg_spf_node_ptr, *rcg_spf_erased_node_ptr;
  struct spf_node   **base_rcg_spf_node_ptr_address, **rcg_spf_erased_node_ptr_address;
      
  iclause = 0;

  while ((iclause = sxba_scan (rcg_spf.walk.status, iclause)) > 0) {
    /* active ds l'etat precedent ... */
    cur_rcg_spf_node_ptr = rcg_spf.walk.iclause2spf_node [iclause];
    Aij = cur_rcg_spf_node_ptr->rho [1];
  
    base_rcg_spf_node_ptr_address = &(GET_RCG_SPF_HEAD_NODE_PTR (Aij));
    rcg_spf_erased_node_ptr_address = &(GET_RCG_SPF_ERASED_HEAD_NODE_PTR (Aij));

    while ((rcg_spf_erased_node_ptr = *rcg_spf_erased_node_ptr_address)) {;
      iclause = rcg_spf_erased_node_ptr->instantiated_clause_nb;

      if (SXBA_bit_is_set_reset (rcg_spf.walk.status, iclause)) {
	/* On trouve toutes les iclauses d'un meme Aij au cours du meme parcourt */
	/* ... et inactive ds le courant */
	/* On la repasse en actif */
	rcg_spf_reactivate_an_iclause (base_rcg_spf_node_ptr_address, rcg_spf_erased_node_ptr_address);

	/* On met a jour Aij2rhs_nb */
	rho = &(cur_rcg_spf_node_ptr->rho [0]);
	clause = *rho++;
	son_nb = cur_rcg_spf_node_ptr->G->clause2son_nb [clause];

	if (son_nb) {
	  for (son = 1; son <= son_nb; son++) {
	    Xpq = rho [son];
	    if (Xpq > 0) {
	      /* Xpq est accessible par un chemin suppl;ementaire */
	      rcg_spf.Aij2rhs_nb [Xpq]++;
	    }
	  }
	}
      }

      rcg_spf_erased_node_ptr_address = &(rcg_spf_erased_node_ptr->next);
    }
  }

#if EBUG
  rcg_spf_check_consistency ();
#endif /* EBUG */
}


/* ************************************************************************************ */
/* Partie permettant, dans le cas is_dag, de construire pour chaque predicat instancie' Aij de degre' k,
   k DAGs ou a chaque range de Aij est associe' un DAG qui represente le langage selectionne par ce
   range
*/

#define nl_threshold                    5 /* nb de trans epsilon ds un dag qui declanche la minimisation */

static SXINT                            *nl_Aij_disp;
static SXBA                             nl_dag_Aij_k_set, nl_lhs_arg_set;

struct nl_dag_exec_stack_struct {
  SXINT p, q, kind /* mal nommé: stocke en fait le nombre d'epsilon-transitions dans le DAG */, minp, maxp;
} empty_pq;

static SXINT                            *nl_dag_Aij_k_root_stack;
static struct nl_dag_exec_stack_struct  *nl_dag_Aij_k_root2pq;

static struct nl_dag_exec_stack_struct  *nl_dag_exec_stack, *nl_dag_Aij_k2pq;
static SXBA                             *nl_dag_exec_stack2arg_pos;

static SXINT                            nl_dag_exec_stack_top, nl_dag_exec_stack_size, nl_dag_last_state;
static XxYxZ_header                     nl_dag_hd;
static SXINT                            *nl_dag_arg_pos2Aij, *nl_dag_arg_pos2k;
static SXBA                             tbp_nl_dag_arg_pos;
static SXINT                            min_nl_subdag_final_state, min_nl_subdag_min_state, min_nl_subdag_max_state;
static SXINT                            and_nl_subdag_min_op1_state, and_nl_subdag_min_op2_state, and_nl_subdag_max_op1_state, and_nl_subdag_max_op2_state;

#if LOG
static SXINT                            *nl_dag_state_stack;
static SXBA                             nl_dag_state_set;
#endif /* LOG */

/* Variables et structures pour/de la 2eme passe */
static SXINT                            *nl_dag_iclause2iclause_k_disp;
static SXINT                            nl_dag_iclause2iclause_k_disp_size;
static struct nl_dag_exec_stack_struct  *nl_dag_iclause_k2pq;

struct nl_dag_what_struct {
  SXINT Aij, i, j, k;
  struct nl_dag_exec_stack_struct *pq_ptr;
};

static struct nl_dag_what_struct        *nl_dag_arg_pos2what;
static SXINT                            *nl_dag_lb_stack, *nl_dag_ub_stack, *nl_dag_minp_stack, *nl_dag_maxp_stack;

#if LOG
/* Imprime le sous-dag de dag_hd_ptr entre les etats p et q */
static void
print_sub_dag (XxYxZ_header *dag_hd_ptr, struct nl_dag_exec_stack_struct *pq_ptr)
{
  SXINT p, q, ptq, Tpq, r;

  p = pq_ptr->p;
  q = pq_ptr->q;

  if (p==0 && q==0)
    fputs (" an Empty DAG\n", stdout);
  else {
    /* nl_dag_last_state est suppose' etre bien positionne' */
    printf (" the subdag [%ld..%ld] whose span is [%ld..%ld]\n", pq_ptr->p, pq_ptr->q, pq_ptr->minp, pq_ptr->maxp);

    nl_dag_state_stack = (SXINT *) sxalloc (nl_dag_last_state+1, sizeof (SXINT));
    RAZ (nl_dag_state_stack);
    nl_dag_state_set = sxba_calloc (nl_dag_last_state+1);

    PUSH (nl_dag_state_stack, p);
    SXBA_1_bit (nl_dag_state_set, p);

    while (!IS_EMPTY (nl_dag_state_stack)) {
      p = POP (nl_dag_state_stack);
      XxYxZ_Xforeach (*dag_hd_ptr, p, ptq) {
	Tpq = XxYxZ_Y (*dag_hd_ptr, ptq);
	r = XxYxZ_Z (*dag_hd_ptr, ptq);

	if (Tpq)
	  printf ("%ld[%ld]\t%s\t%ld[%ld]%s\n",
		  p, pqt2p (Tpq), glbl_t2string [pqt2t (Tpq)], r, pqt2q (Tpq), (r == q) ? " (final)" : "");
	else
	  /* transition epsilon */
	  printf ("%ld\t%s\t%ld%s\n",
		  p, "\"<EPSILON>\"", r, (r == q) ? " (final)" : "");

	if (r != q && SXBA_bit_is_reset_set (nl_dag_state_set, r))
	  PUSH (nl_dag_state_stack, r);
      }
    }

    /* ATTENTION: les deux sxfree ci-dessous étaient dans l'ordre inverse. Ça cause un plantage incompréhensible, tout à
fait ailleurs, sur un XxY_free. Faire les sxfree dans l'ordre ci-dessous fait que ça ne plante plus, pour l'instant du
moins. Configuration concernée (testée):
   Configured with: --prefix=/Applications/Xcode.app/Contents/Developer/usr --with-gxx-include-dir=/usr/include/c++/4.2.1
   Apple LLVM version 5.1 (clang-503.0.40) (based on LLVM 3.4svn)
   Target: x86_64-apple-darwin13.1.0
   Thread model: posix
sur Mavericks (MacOS X 10.9.2)*/
    sxfree (nl_dag_state_set), nl_dag_state_set = NULL;
    sxfree (nl_dag_state_stack), nl_dag_state_stack = NULL;
  }
}
#endif /* LOG */

static SXBOOLEAN
nl_dag_empty_trans (SXINT state, SXBA epsilon_reachable_next_states)
{
  SXINT transition, next_state = 0;

  XxYxZ_Xforeach (nl_dag_hd, state+min_nl_subdag_min_state, transition) {
    if (XxYxZ_Y (nl_dag_hd, transition) == 0) {
      /* transition sur epsilon */
      next_state = XxYxZ_Z (nl_dag_hd, transition)-min_nl_subdag_min_state;
      SXBA_1_bit (epsilon_reachable_next_states, next_state);
    }
  }

  return (next_state != 0);
}


static void
nl_dag_nfa_extract_trans (SXINT state, void (*output_trans) (SXINT, SXINT, SXINT))
{
  SXINT ste, transition;

  XxYxZ_Xforeach (nl_dag_hd, state+min_nl_subdag_min_state, transition) {
    if ((ste = XxYxZ_Y (nl_dag_hd, transition)) > 0) {
      /* transition pas sur epsilon */
      (*output_trans) (state, ste, XxYxZ_Z (nl_dag_hd, transition)-min_nl_subdag_min_state);
    }
  }
}

static void
min_nl_subdag_fill_trans (SXINT dfa_state, SXINT t, SXINT next_dfa_state)
{
  SXINT triple;

  if (t == eof_Tpq_code) {
    if (dfa_state+1 == next_dfa_state)
      /* On supprime la "derniere" trans sur eof */
      return;

    /* on garde les autres mais l'etat final next_dfa_state est repositionne a sa vraie valeur */
    next_dfa_state--;
    /* et la trans vers l'etat final se fera sur epsilon
       on conserve donc la minimalite' de l'automate mais avec possibilite de transitions epsilon vers l'etat final */
    t = 0;
  }

  dfa_state += nl_dag_last_state;
  next_dfa_state += nl_dag_last_state;

  if (min_nl_subdag_final_state < next_dfa_state)
    min_nl_subdag_final_state = next_dfa_state;

  if (dfa_state < next_dfa_state)
    XxYxZ_set (&nl_dag_hd, dfa_state, t, next_dfa_state, &triple);    
}

static struct nl_dag_exec_stack_struct
nl_dag_minimize (struct nl_dag_exec_stack_struct pq)
{
  SXINT  nl_dag_state, dfa_state, id, bot, top, cur_bound, old_state;
  struct nl_dag_exec_stack_struct pq_result;

#if LOG
  fputs ("The minimization of", stdout);
  print_sub_dag (&nl_dag_hd, &pq);
  fputs ("results in", stdout);
#endif /* LOG */

  pq_result.p = nl_dag_last_state+1;

  min_nl_subdag_final_state = 0;
  min_nl_subdag_min_state = pq.minp-1;
  min_nl_subdag_max_state = pq.maxp;

  /* On translate */
  nfa2dfa (pq.p - min_nl_subdag_min_state,
	   pq.q - min_nl_subdag_min_state,
	   min_nl_subdag_max_state - min_nl_subdag_min_state,
	   eof_Tpq_code, 
	   nl_dag_empty_trans,
	   nl_dag_nfa_extract_trans, 
	   NULL, 
	   min_nl_subdag_fill_trans, 
	   SXTRUE /* to_be_normalized */
	   );

#if 0
  nfa2dfa (pq.p, pq.q, nl_dag_last_state, eof_Tpq_code, 
	   nl_dag_empty_trans,
	   nl_dag_nfa_extract_trans, 
	   NULL, 
	   min_nl_subdag_fill_trans, 
	   SXTRUE /* to_be_normalized */
	   );
#endif /* 0 */

  pq_result.q = nl_dag_last_state = min_nl_subdag_final_state;
  pq_result.kind = 0; /* Il ne contient pas de trans sur epsilon (sauf les indispensables) */
  pq_result.minp = pq_result.p; /* Apres minimisation l'etat initial est le min */
  pq_result.maxp = pq_result.q; /* Apres minimisation l'etat final est le max */

#if LOG
  print_sub_dag (&nl_dag_hd, &pq_result);
#endif /* LOG */

  return pq_result;
}


static void
nl_dag_cat (struct nl_dag_exec_stack_struct *pq_result_ptr, struct nl_dag_exec_stack_struct pq)
{
  SXINT nl_dag_trans;

#if LOG
  fputs ("The concatenation of", stdout);
  print_sub_dag (&nl_dag_hd, pq_result_ptr);
  fputs ("and", stdout);
  print_sub_dag (&nl_dag_hd, &pq);
  fputs ("results in", stdout);
#endif /* LOG */

  XxYxZ_set (&nl_dag_hd, pq_result_ptr->q, 0 /* trans eps */, pq.p, &nl_dag_trans);
  pq_result_ptr->q = pq.q;
  pq_result_ptr->kind += pq.kind + 1;

  if (pq.minp < pq_result_ptr->minp)
    pq_result_ptr->minp = pq.minp;

  if (pq.maxp > pq_result_ptr->maxp)
    pq_result_ptr->maxp = pq.maxp;

#if LOG
  print_sub_dag (&nl_dag_hd, pq_result_ptr);
#endif /* LOG */

  if (pq_result_ptr->kind >= nl_threshold)
    *pq_result_ptr = nl_dag_minimize (*pq_result_ptr);
}

#if 0
/* Copie "multiple" d'un {p} -> {q} A FAIRE */ 
static struct nl_dag_exec_stack_struct
nl_dag_multi_copy (SXINT *lb_stack, SXINT *ub_stack)
{
}
#endif /* 0 */

/* Copie "simple" d'un p -> q */
static struct nl_dag_exec_stack_struct
nl_dag_copy (struct nl_dag_exec_stack_struct pq)
{
  SXINT                           p, q, result_p, result_q, triple, nl_dag_trans;
  SXINT                           *p2result_p, *p_stack, *triple_stack;
  SXBA                            p_set;
  struct nl_dag_exec_stack_struct pq_result;

#if LOG
  fputs ("The copy of", stdout);
  print_sub_dag (&nl_dag_hd, &pq);
  fputs ("results in", stdout);
#endif /* LOG */

  p = pq.p;

  p2result_p = (SXINT*) sxalloc (nl_dag_last_state+1, sizeof (SXINT));
  p_stack = (SXINT*) sxalloc (nl_dag_last_state+1, sizeof (SXINT)), RAZ (p_stack);
  p_set = sxba_calloc (nl_dag_last_state+1);
  triple_stack = (SXINT*) sxalloc (XxYxZ_top (nl_dag_hd)+1, sizeof (SXINT)), RAZ (triple_stack);

  p2result_p [p] = pq_result.minp = pq_result.p = ++nl_dag_last_state;

  PUSH (p_stack, p);
  SXBA_1_bit (p_set, p);

  while (!IS_EMPTY (p_stack)) {
    p = POP (p_stack);
    /* p n'est pas l'etat final */
    result_p = p2result_p [p];

    XxYxZ_Xforeach (nl_dag_hd, p, triple) {
      q = XxYxZ_Z (nl_dag_hd, triple);

      if (q == pq.q) {
	/* On differe la mise en table des transitions vers l'etat final */
	PUSH (triple_stack, triple);
      }
      else {
	if (SXBA_bit_is_reset_set (p_set, q)) {
	  result_q = p2result_p [q] = ++nl_dag_last_state;
	  PUSH (p_stack, q);
	}
	else
	  result_q = p2result_p [q];

	XxYxZ_set (&nl_dag_hd, result_p, XxYxZ_Y (nl_dag_hd, triple), result_q, &nl_dag_trans);
      }
    }
  }

  result_q = pq_result.maxp = pq_result.q = ++nl_dag_last_state; /* L'etat final est le + grand */

  while (!IS_EMPTY (triple_stack)) {
    triple = POP (triple_stack);
    XxYxZ_set (&nl_dag_hd, p2result_p [XxYxZ_X (nl_dag_hd, triple)], XxYxZ_Y (nl_dag_hd, triple), result_q, &nl_dag_trans);
  }

  pq_result.kind = pq.kind /* IS_A_COPY */;

  sxfree (p2result_p);
  sxfree (p_stack);
  sxfree (p_set);
  sxfree (triple_stack);

#if LOG
  print_sub_dag (&nl_dag_hd, &pq_result);
#endif /* LOG */

  return pq_result;
}


static struct nl_dag_exec_stack_struct
nl_dag_or (struct nl_dag_exec_stack_struct pq1, struct nl_dag_exec_stack_struct pq2)
{
  /* pq1 est non vide */
  /* On fait l'union entre pq1 et pq2 et on retourne le resultat */
  /* On ne doit pas ajouter de transition partant de pq1.p ou pq2.p */
  SXINT                           triple;
  struct nl_dag_exec_stack_struct pq_result;


  if (pq1.p == pq2.p
      && pq1.q == pq2.q
      && pq1.minp == pq2.minp
      && pq1.maxp == pq2.maxp) {
    if (pq1.kind != pq2.kind)
      sxtrap(ME, "nl_dag_and (trying to intersect two subdags that are identical except for their 'kind')");
#if LOG
    fputs ("The union of", stdout);
    print_sub_dag (&nl_dag_hd, &pq1);
    fputs ("with itself results in", stdout);
    print_sub_dag (&nl_dag_hd, &pq1);
#endif /* LOG */

    return pq1;
  }
      

#if LOG
  fputs ("The union of", stdout);
  print_sub_dag (&nl_dag_hd, &pq1);
  fputs ("and", stdout);
  print_sub_dag (&nl_dag_hd, &pq2);
  fputs ("results in", stdout);
#endif /* LOG */

  pq_result.p = ++nl_dag_last_state;

  XxYxZ_set (&nl_dag_hd, nl_dag_last_state, 0, pq1.p, &triple);  
  XxYxZ_set (&nl_dag_hd, nl_dag_last_state, 0, pq2.p, &triple);  
  
  pq_result.q = ++nl_dag_last_state;
  /* On pourrait n'utiliser que 3 trans epsilon, la` on est su^r que l'etat
     final est le + grand de tous les etats de l'union */
  XxYxZ_set (&nl_dag_hd, pq1.q, 0, pq_result.q, &triple); 
  XxYxZ_set (&nl_dag_hd, pq2.q, 0, pq_result.q, &triple); 


  pq_result.minp = pq1.minp < pq2.minp ? pq1.minp : pq2.minp;
  pq_result.maxp = nl_dag_last_state;
  pq_result.kind = pq1.kind + pq2.kind + 4 /* ... transitions epsilon */;

#if LOG
  print_sub_dag (&nl_dag_hd, &pq_result);
#endif /* LOG */

  if (pq_result.kind >= nl_threshold)
    pq_result = nl_dag_minimize (pq_result);

  return pq_result;
}

static SXBOOLEAN
nl_dag_and_eps_op1_trans (SXINT p, SXBA q_set)
{
  SXINT     q, pq, Tij, ptq;
  SXBOOLEAN ret_val;

  XxYxZ_Xforeach (nl_dag_hd, p+and_nl_subdag_min_op1_state, ptq) {
    Tij = XxYxZ_Y (nl_dag_hd, ptq);

    if (Tij == 0) {
      q = XxYxZ_Z (nl_dag_hd, ptq)-and_nl_subdag_min_op1_state;

      if (SXBA_bit_is_reset_set (q_set, q))
	ret_val = SXTRUE;
      
    }
  }

  return ret_val;
}

static SXBOOLEAN
nl_dag_and_eps_op2_trans (SXINT p, SXBA q_set)
{
  SXINT     q, pq, Tij, ptq;
  SXBOOLEAN ret_val;

  XxYxZ_Xforeach (nl_dag_hd, p+and_nl_subdag_min_op2_state, ptq) {
    Tij = XxYxZ_Y (nl_dag_hd, ptq);

    if (Tij == 0) {
      q = XxYxZ_Z (nl_dag_hd, ptq)-and_nl_subdag_min_op2_state;

      if (SXBA_bit_is_reset_set (q_set, q))
	ret_val = SXTRUE;
      
    }
  }

  return ret_val;
}


static void
nl_dag_and_non_eps_op1_trans (SXINT p, void (*f)(SXINT, SXINT, SXINT))
{
  SXINT     Tij, ptq;

  XxYxZ_Xforeach (nl_dag_hd, p+and_nl_subdag_min_op1_state, ptq) {
    Tij = XxYxZ_Y (nl_dag_hd, ptq);

    if (Tij)
      (*f) (p, Tij, XxYxZ_Z (nl_dag_hd, ptq)-and_nl_subdag_min_op1_state);
  }
}


static void
nl_dag_and_non_eps_op2_trans (SXINT p, void (*f)(SXINT, SXINT, SXINT))
{
  SXINT     Tij, ptq;

  XxYxZ_Xforeach (nl_dag_hd, p+and_nl_subdag_min_op2_state, ptq) {
    Tij = XxYxZ_Y (nl_dag_hd, ptq);

    if (Tij)
      (*f) (p, Tij, XxYxZ_Z (nl_dag_hd, ptq)-and_nl_subdag_min_op2_state);
  }
}


static struct nl_dag_exec_stack_struct nl_and_pq_result;


static void
nl_dag_fill_trans (SXINT p, SXINT t, SXINT q, SXBOOLEAN is_final)
{
  SXINT triple;

  if (t == eof_Tpq_code)
    t = 0; /* On met une eps trans */

  p += nl_dag_last_state;
  q += nl_dag_last_state;

  if (nl_and_pq_result.p == 0)
    nl_and_pq_result.p = p;

  if (is_final)
    nl_and_pq_result.q = q;

  XxYxZ_set (&nl_dag_hd, p, t, q, &triple); 
}

static SXBOOLEAN
nl_dag_and (struct nl_dag_exec_stack_struct *pq_result_ptr, struct nl_dag_exec_stack_struct pq)
{
  /* On fait l'inter entre *pq_result_ptr et pq
     Si vide on retourne faux
     Sinon, on met cette intersection ds *pq_result_ptr et on retourne vrai */
  SXINT                  nl_dag_state, inter_state, id, bot, top, cur_bound, old_state;
  SXBOOLEAN              ret_val;

  if (pq_result_ptr->p == pq.p
      && pq_result_ptr->q == pq.q
      && pq_result_ptr->minp == pq.minp
      && pq_result_ptr->maxp == pq.maxp) {
    if (pq_result_ptr->kind != pq.kind)
      sxtrap(ME, "nl_dag_and (trying to intersect two subdags that are identical except for their 'kind')");
#if LOG
    fputs ("The intersection of", stdout);
    print_sub_dag (&nl_dag_hd, pq_result_ptr);
    fputs ("with itself results in", stdout);
    print_sub_dag (&nl_dag_hd, pq_result_ptr);
#endif /* LOG */

    return SXTRUE;
  }


#if LOG
  fputs ("The intersection of", stdout);
  print_sub_dag (&nl_dag_hd, pq_result_ptr);
  fputs ("and", stdout);
  print_sub_dag (&nl_dag_hd, &pq);
  fputs ("results in", stdout);
#endif /* LOG */

  nl_and_pq_result.p = 0;
  and_nl_subdag_min_op1_state = pq_result_ptr->minp-1;
  and_nl_subdag_min_op2_state = pq.minp-1;
  and_nl_subdag_max_op1_state = pq_result_ptr->maxp;
  and_nl_subdag_max_op2_state = pq.maxp;


  ret_val =  nfa_intersection (pq_result_ptr->p - and_nl_subdag_min_op1_state,
			       pq_result_ptr->q - and_nl_subdag_min_op1_state,
			       and_nl_subdag_max_op1_state - and_nl_subdag_min_op1_state,  /* en général, == 0 ; si > 0, on est dans le cas DAG (pas de trans
						  sortant de final_state) ; en contrepartie, on a le droit d'avoir
						  final_state != max_state, et on a le droit d'avoir des transitions
						  vers final_state qui ne sont pas sur eof_Tpq_code */
			       eof_Tpq_code, 
			       pq.p - and_nl_subdag_min_op2_state, 
			       pq.q - and_nl_subdag_min_op2_state, 
			       and_nl_subdag_max_op2_state - and_nl_subdag_min_op2_state,
			       eof_Tpq_code, 
			       nl_dag_and_eps_op1_trans,
			       nl_dag_and_eps_op2_trans,
			       nl_dag_and_non_eps_op1_trans,
			       nl_dag_and_non_eps_op2_trans, 
			       nl_dag_fill_trans
			       );

#if 0
  ret_val =  nfa_intersection (pq_result_ptr->p,
			       pq_result_ptr->q,
			       nl_dag_last_state,  /* en général, == 0 ; si > 0, on est dans le cas DAG (pas de trans
						  sortant de final_state) ; en contrepartie, on a le droit d'avoir
						  final_state != max_state, et on a le droit d'avoir des transitions
						  vers final_state qui ne sont pas sur eof_Tpq_code */
			       eof_Tpq_code, 
			       pq.p, 
			       pq.q, 
			       nl_dag_last_state,
			       eof_Tpq_code, 
			       nl_dag_and_eps_trans,
			       nl_dag_and_eps_trans,
			       nl_dag_and_non_eps_trans,
			       nl_dag_and_non_eps_trans, 
			       nl_dag_fill_trans
			       );
#endif /* 0 */


  if (ret_val) {
    nl_dag_last_state = nl_and_pq_result.q;

    nl_and_pq_result.kind = 0; /* pas d'eps en general */
    nl_and_pq_result.minp = nl_and_pq_result.p;
    nl_and_pq_result.maxp = nl_and_pq_result.q;
    *pq_result_ptr = nl_and_pq_result;
  }
  else
    *pq_result_ptr = empty_pq;

#if LOG
  print_sub_dag (&nl_dag_hd, pq_result_ptr);
#endif /* LOG */

  return ret_val;
}


static void
nl_dag_exec_stack_push (struct nl_dag_exec_stack_struct pq)
{
  SXINT old_line_nb;

  if (++nl_dag_exec_stack_top >= nl_dag_exec_stack_size) {
    old_line_nb = nl_dag_exec_stack_size;
    nl_dag_exec_stack = (struct nl_dag_exec_stack_struct*) sxrealloc (nl_dag_exec_stack, (nl_dag_exec_stack_size *= 2)+1, sizeof (struct nl_dag_exec_stack_struct));
    nl_dag_exec_stack2arg_pos = sxbm_resize (nl_dag_exec_stack2arg_pos, old_line_nb+1, nl_dag_exec_stack_size+1, global_max_arg_nb_per_clause+1);
  }

  nl_dag_exec_stack [nl_dag_exec_stack_top] = pq;
}

static struct nl_dag_exec_stack_struct
nl_dag_exec_stack_pop (void)
{
  return nl_dag_exec_stack [nl_dag_exec_stack_top--];
}


/* Pour la passe top-down, sur chaque racine Aij_k on va noter une liste de couples
   (pq, {Aij_k}) qui va permettre d'initialiser chaque racine Aij_k avec l'union de ses pq */
static void
nl_dag_exec_stack_back_and (SXINT stack_index)
{
  SXINT                           arg_pos, cur_Aij, cur_k, Aij_k, old_size, new_size;
  SXBA                            arg_pos_set;
  struct nl_dag_exec_stack_struct pq;

  pq = nl_dag_exec_stack [stack_index];

  /* pq est le resultat d'une intersection que l'on repercute sur ses constituants */
  arg_pos_set = nl_dag_exec_stack2arg_pos [stack_index];
  arg_pos = -1;

  while ((arg_pos = sxba_scan (arg_pos_set, arg_pos)) >= 0) {
    /* arg_pos >= lhs_arity */
    cur_Aij = nl_dag_arg_pos2Aij [arg_pos];
    cur_k = nl_dag_arg_pos2k [arg_pos];
    Aij_k = nl_Aij_disp [cur_Aij] + cur_k;
    /* Cet Aij_k est un arg d'une intersection qui ne sera pas memorisee en lhs,
       c'est donc une root, on memorise donc le couple (pq , Aij_k) */
    
    old_size = DSIZE (nl_dag_Aij_k_root_stack);
    DPUSH (nl_dag_Aij_k_root_stack, Aij_k);
    new_size = DSIZE (nl_dag_Aij_k_root_stack);
    
    if (new_size > old_size)
      nl_dag_Aij_k_root2pq = (struct nl_dag_exec_stack_struct *) sxrealloc (nl_dag_Aij_k_root2pq, new_size+1, sizeof (struct nl_dag_exec_stack_struct ));
    
    nl_dag_Aij_k_root2pq [DTOP (nl_dag_Aij_k_root_stack)] = pq;
  }
}


static void
nl_dag_exec_stack_dup (SXINT stack_index)
{
  nl_dag_exec_stack_push (nl_dag_exec_stack [stack_index]);
}

static void
nl_dag_load_symb (SXINT Tpq)
{
  /* On fait un dag du terminal symb et on push le resultat sur la pile d'execution */
  SXINT                           nl_dag_trans;
  struct nl_dag_exec_stack_struct pq;

  pq.minp = pq.p = ++nl_dag_last_state;
  pq.maxp = pq.q = ++nl_dag_last_state;
  pq.kind = 0; /* pas d'eps */

  XxYxZ_set (&nl_dag_hd, pq.p, Tpq, pq.q, &nl_dag_trans);
  nl_dag_exec_stack_push (pq);

#if LOG
  fputs ("Creation of", stdout);
  print_sub_dag (&nl_dag_hd, &pq);
#endif /* LOG */
}

static SXBOOLEAN
nl_dag_exec_stack_and (void)
{
  /* On fait l'intersection entre le sommet et le sous-sommet et si non vide on range
     le resultat au sous sommet */
  SXINT old_top, new_top;

  old_top = nl_dag_exec_stack_top;
  new_top = --nl_dag_exec_stack_top;

  sxba_or (nl_dag_exec_stack2arg_pos [new_top], nl_dag_exec_stack2arg_pos [old_top]);
  return nl_dag_and (&(nl_dag_exec_stack [new_top]), nl_dag_exec_stack [old_top]);
}

static void
nl_dag_exec_stack_cat (void)
{
  /* On fait la concatenation entre le sous-sommet et le sommet et on range
     le resultat au sous-sommet */
  SXINT old_top, new_top;

  old_top = nl_dag_exec_stack_top;
  new_top = --nl_dag_exec_stack_top;

  nl_dag_cat (&(nl_dag_exec_stack [new_top]), nl_dag_exec_stack [old_top]);
}


/* On est ds la 1ere passe et AijXpid_hd vient de deborder et on a demande' a etre prevenu */
static void
nl_Aij_oflw (SXINT old_size, SXINT new_size)
{
  if (new_size > cur_AijXxxx_hd_size) {
    nl_Aij_disp = (SXINT *) sxrecalloc (nl_Aij_disp, old_size+1, new_size+1, sizeof (SXINT));
    nl_dag_Aij_k2pq = (struct nl_dag_exec_stack_struct*) sxrealloc (nl_dag_Aij_k2pq, new_size*global_arity+1, sizeof (struct nl_dag_exec_stack_struct));
    nl_dag_Aij_k_set = sxba_resize (nl_dag_Aij_k_set, new_size*global_arity+1);
    cur_AijXxxx_hd_size = new_size;
  }
}


static struct nl_dag_exec_stack_struct
create_pq_subdag_from_input_dag (SXINT init_state, SXINT final_state)
{
  SXINT i, j, t, bot_ij, top_ij, nl_dag_trans, p, q;
  SXINT *cur_t_ptr, *tstack;
  struct nl_dag_exec_stack_struct pq_result;

  if (input_dag_state2p == NULL)
    input_dag_state2p = (SXINT*) sxcalloc (idag.final_state+1, sizeof (SXINT));

  input_dag_state2p [init_state] = pq_result.minp = pq_result.p = ++nl_dag_last_state;

  for (i = init_state; i < final_state; i++) {
    if (SXBA_bit_is_set(idag.path[i], final_state)) { // si final_state est accessible depuis i
      bot_ij = idag.p2pq_hd [i]; // début de la sous-pile des transitions sortant de i
      top_ij = idag.p2pq_hd [i+1]; // début de la suivante, i.e., juste après la dernière de celles sortant de i
      p = input_dag_state2p [i]; // forcément déjà rempli à une passe précédente où i était un j
      while (bot_ij < top_ij) {
	j = idag.pq2q [bot_ij];

	if (j == final_state || SXBA_bit_is_set(idag.path[j], final_state)) { // si final_state est accessible depuis j
	  tstack = idag.t_stack + idag.pq2t_stack_hd [bot_ij];
	  for (cur_t_ptr = tstack + *tstack; cur_t_ptr > tstack; cur_t_ptr--) {
	    t = *cur_t_ptr; /* code global */
	    if ((q = input_dag_state2p[j]) == 0)
	      input_dag_state2p [j] = q = ++nl_dag_last_state;
	    XxYxZ_set (&nl_dag_hd, p, p_q_t2pqt (i,j,t), q, &nl_dag_trans);
	  }
	}
	bot_ij++;
      }
    }
  }
  pq_result.maxp = pq_result.q = nl_dag_last_state;
  pq_result.kind = 0; // pas d'epsilon-transition

  for (i = init_state; i <= final_state; i++) {
    input_dag_state2p[i] = 0;
  }

  return pq_result;
}

/* Depuis le 30/04/09 toujours appele' avec une spf construite */
/* Peut etre appele' depuis rcg_spf_output_dag_yield (avec TRUE) ou depuis RCG_parser
   (avec FALSE) ds le cas d'une grammaire non-lineaire */
void
rcg_spf_dag_yield_alloc (SXBOOLEAN is_spf_already_built, SXBOOLEAN is_td_pass_needed)
{
  SXINT Aij_size, Pij_size;
  static SXINT nl_dag_foreach [] = {1, 0, 0, 0, 0, 0};

  if (is_spf_already_built) {
    /* Certaines tailles sont connues ... */
    Aij_size = rcg_spf.max_instantiated_prdct_nb;
    Pij_size = rcg_spf.max_instantiated_clause_nb;
  }
  else {
    /* Les tailles sont estimees */
    /* Au moins un des modules est non lineaire, on fait les alloc */

    /* Pour reagir au debordement de rcg_spf.AijXpid_hd (et de ses camarades aussi indexés par AijXpid; si PID == 1, pas de AijXpid, mais quelques camardes, et les index sont sur Aij */
    rcg_spf.semact.oflw = nl_Aij_oflw;

    Pij_size = Aij_size = rcg_spf.cur_AijXxxx_hd_size;
  }

  nl_Aij_disp = (SXINT *) sxcalloc (Aij_size+1, sizeof (SXINT));
  nl_dag_Aij_k2pq = (struct nl_dag_exec_stack_struct*) sxalloc (Aij_size*global_arity+1, sizeof (struct nl_dag_exec_stack_struct)),
    nl_dag_Aij_k2pq [0].p = 1 /* top */, nl_dag_Aij_k2pq [0].q = Aij_size /* size */;
  nl_dag_Aij_k_set = sxba_calloc (Aij_size*global_arity+1);

  DALLOC_STACK (nl_dag_Aij_k_root_stack, 50);
  nl_dag_Aij_k_root2pq = (struct nl_dag_exec_stack_struct*) sxalloc (DSIZE (nl_dag_Aij_k_root_stack)+1, sizeof (struct nl_dag_exec_stack_struct));

  nl_dag_arg_pos2Aij = (SXINT*) sxalloc (global_max_arg_nb_per_clause+1, sizeof (SXINT));
  nl_dag_arg_pos2k = (SXINT*) sxalloc (global_max_arg_nb_per_clause+1, sizeof (SXINT));
  tbp_nl_dag_arg_pos = sxba_calloc (global_max_arg_nb_per_clause+1);

  nl_dag_exec_stack_size = 50;

  nl_dag_exec_stack = (struct nl_dag_exec_stack_struct*) sxalloc (nl_dag_exec_stack_size+1, sizeof (struct nl_dag_exec_stack_struct));
  nl_dag_exec_stack2arg_pos = sxbm_calloc (nl_dag_exec_stack_size+1, global_max_arg_nb_per_clause+1);


  XxYxZ_alloc (&nl_dag_hd, "nl_dag_hd", Aij_size+1, 1, nl_dag_foreach, NULL, NULL);
  nl_dag_last_state = 0;

  nl_lhs_arg_set = sxba_calloc (global_arity+1);

  if (is_td_pass_needed) {
    /* On est ds le cas non-lineaire, on veut aussi une passe top_down */
    /* taille doit etre le nb de clauses instanciees */
    nl_dag_iclause2iclause_k_disp = (SXINT*) sxalloc ((nl_dag_iclause2iclause_k_disp_size = Pij_size)+1, sizeof (SXINT));
    nl_dag_iclause_k2pq = (struct nl_dag_exec_stack_struct*) sxalloc (nl_dag_iclause2iclause_k_disp_size*global_arity+1, sizeof (struct nl_dag_exec_stack_struct)),
      nl_dag_iclause_k2pq [0].p = 1 /* top */, nl_dag_iclause_k2pq [0].q = nl_dag_iclause2iclause_k_disp_size*global_arity /* size */;
  }
}

void
rcg_spf_dag_yield_free (void)
{
  sxfree (nl_Aij_disp), nl_Aij_disp = NULL;
  sxfree (nl_dag_Aij_k2pq), nl_dag_Aij_k2pq = NULL;
  sxfree (nl_dag_Aij_k_set), nl_dag_Aij_k_set = NULL;
  sxfree (nl_dag_arg_pos2Aij), nl_dag_arg_pos2Aij = NULL;
  sxfree (tbp_nl_dag_arg_pos), tbp_nl_dag_arg_pos = NULL;
  sxfree (nl_dag_arg_pos2k), nl_dag_arg_pos2k = NULL;
  sxfree (nl_dag_exec_stack), nl_dag_exec_stack = NULL;
  sxbm_free (nl_dag_exec_stack2arg_pos), nl_dag_exec_stack2arg_pos = NULL;
  XxYxZ_free (&nl_dag_hd);
  sxfree (nl_lhs_arg_set), nl_lhs_arg_set = NULL;

  if (nl_dag_iclause2iclause_k_disp) {
    sxfree (nl_dag_iclause2iclause_k_disp), nl_dag_iclause2iclause_k_disp = NULL;
    sxfree (nl_dag_iclause_k2pq), nl_dag_iclause_k2pq = NULL;
  }

  if (input_dag_state2p)
    sxfree (input_dag_state2p), input_dag_state2p = NULL;
}

/* Point d'entree principal qui peut etre appele (aussi) depuis RCG_parser */

  /* la clause instanciee courante necessite des calculs de sous-dags du dag d'entree */
  /* stmt_stack est de la forme p X1 X2 ... Xp */

  /* La clause A(a b X Y, X Z) --> b(X) C(X) D(Y) E(Z) a le genere' :
                   0       1         2    3    4    5

    |     NL_OR        |  On effectue l'union (le resultat n'est pas mis en pile) entre les sous-dag [2] et ...
    |        1         |  ... l'arg 1 de la lhs
    |     NL_CAT       |  On concatene les sous-dag en [2] et [3] et on met le resultat en [2]
    |     NL_LOAD      |  On charge (une copie minimisee si pas la 1ere fois) en sommet de pile [3] d'execution le sous-dag associe au ...
    |        5         |  ... Z de la position 5
    |     NL_DUP       |  On fait une copie (ce n'est pas la 1ere fois qu'on fait un dup de -1) que l'on met en [2]
    |       -1         |  ... du dag de [1] 
    |   NL_OR_START    |  Marque le debut du traitement de l'arg ...
    |        1         |  ... 1
    |     NL_OR        |  On effectue l'union (le resultat n'est pas mis en pile) entre les sous-dag [2] et ...
    |        0         |  ... l'arg 0 de la lhs
    |     NL_CAT       |  On concatene les sous-dag en [2] et [3] et on met le resultat en [2]
    |     NL_LOAD      |  On charge (une copie minimisee si pas la 1ere fois) en sommet de pile [3] d'execution le sous-dag associe au ...
    |        4         |  ... Y de la position 4
    |     NL_CAT       |  On concatene les sous-dag en [2] et [3] et on met le resultat en [2]
    |     NL_DUP       |  On copie l'identifiant (c'est la 1ere fois) en [3] ...
    |       -1         |  ... du dag de [1] 
    |     NL_CAT       |  On concatene les sous-dag en [2] et [3] et on met le resultat en [2]
    |     NL_TERM      |  On fait un sous-dag que l'on met en [3] avec ...
    |       -2         |  opposée de l'indice ds Tpq_stack Tpq_stack [1] = le Tpq de 'b'
    |     NL_TERM      |  On fait un sous-dag que l'on met en [2] avec ...
    |       -1         |  opposée de l'indice ds Tpq_stack Tpq_stack [1] = le Tpq de 'a'
    |   NL_OR_START    |  Marque le debut du traitement de l'arg ...
    |        0         |  ... 0
    |     NL_AND       |  On fait l'intersection/minimisation des (copies) des sous-dag en [1] et [2] et on met le resultat en [1] (retour echec eventuel)
    |     NL_LOAD      |  On charge en sommet de pile [2] d'execution le sous-dag associe au ...
    |        3         |  ... X de la position 3
    |     NL_LOAD      |  On charge en sommet de pile [1] d'execution le sous-dag associe au ...
    |        2         |  ... X de la position 2
    |       29         |  nb d'instructions
    --------------------
  */
/* Si !unconditional on n'execute cette procedure que sur les arguments non lineaires
   sinon (cas yield) on l'execute tout le temps */
SXINT
rcg_spf_process_dag_yield (struct G *G, SXINT *rho, SXBOOLEAN unconditional)
{
  SXINT                           clause, pid, A, Aij, Aij_k_bot, lhs_arity, arg_pos, k, son_nb, son, bot, son_Aij_pid, ret_val;
  SXINT                           rhs_prdct_arity, stmt_nb, operand, operator, cur_Aij, cur_k, Aij_k, adr, lb, ub;
  SXINT                           son_pid, rhs_prdct, rhs_prdct_k, Ak, cur_lhs_arg_pos, nl_dag_exec_stack_and_top;
  SXINT                           *stmt_stack, *Tpq_stack;
  struct Aij_struct               *Aij_struct_ptr;
  struct nl_dag_exec_stack_struct pq, *pq_ptr;
  SXBOOLEAN                       FIRST_TIME;

#if LOG
  fputs ("rcg_spf_process_dag_yield: ", stdout);
  rcg_spf_print_instantiated_clause (G, rho);
#endif /* LOG */

  clause = *rho++;

  if (!unconditional && !G->is_a_complete_terminal_grammar && !SXBA_bit_is_set (G->non_right_linear_clause_set, clause))
    /* On est ds le cas non lineaire et cette clause ne participe pas aux calculs de non linearite' */
    return 1;

  /* La`, il y a qqchose a faire */

  FIRST_TIME = SXTRUE;

  stmt_stack = G->nl_stmt_stack + G->clause2nl_stmt_stack [clause];

  if (clause <= 0)
    sxtrap (ME, "rcg_spf_process_dag_yield");

  pid = G->pid;

  /* Calcul unique sur les clauses multiples */
  A = G->clause2lhs_nt [clause];
  lhs_arity = G->nt2arity ? G->nt2arity [A] : 1;

  Aij = *rho;
	    
  if ((Aij_k_bot = nl_Aij_disp [Aij]) == 0) {
    /* 1ere fois qu'on trouve Aij */
    pq_ptr = nl_dag_Aij_k2pq + (Aij_k_bot = nl_Aij_disp [Aij] = nl_dag_Aij_k2pq [0].p);
    nl_dag_Aij_k2pq [0].p += lhs_arity;
  }
  else
    pq_ptr = NULL;

  for (arg_pos = 0, k = 0, Ak = G->A2A0 [A], Aij_k = Aij_k_bot;
       arg_pos < lhs_arity;
       arg_pos++, k++, Ak++, Aij_k++) {
    nl_dag_arg_pos2Aij [arg_pos] = Aij;
    nl_dag_arg_pos2k [arg_pos] = k;

    if (pq_ptr)
      pq_ptr++->p = 0; /* vide */

    if (unconditional || SXBA_bit_is_set (G->nl_Ai_set, Ak)) {
      /* Il faut obligatoirement calculer le FSA associe' a` l'arg k de la lhs */
      SXBA_1_bit (tbp_nl_dag_arg_pos, arg_pos);
    }
  }

  /* pbs sur les predefinis */
  son_nb = G->clause2son_nb ? G->clause2son_nb [clause] : 0;

  Tpq_stack = rho+1;

  if (son_nb > 0) {
    Tpq_stack += son_nb;

    for (son = 1; son <= son_nb; son++) {
      son_Aij_pid = rho [son];

      if (son_Aij_pid < 0)
	/* neg call !! */
	sxtrap (ME, "rcg_spf_process_dag_yield");
		    
      Aij_struct_ptr = call_instantiated_prdct2struct (son_Aij_pid);
      son_pid = Aij_pid2pid (son_Aij_pid);
      
      rhs_prdct_arity = Aij_struct_ptr->arity /* marche aussi sur les predefinis */;
      rhs_prdct = Aij_struct_ptr->A;

      for (k = 0, rhs_prdct_k = Gs [son_pid]->A2A0 [rhs_prdct]; k < rhs_prdct_arity; k++, arg_pos++, rhs_prdct_k++) {
	nl_dag_arg_pos2Aij [arg_pos] = son_Aij_pid;
	nl_dag_arg_pos2k [arg_pos] = k;

	if (unconditional || SXBA_bit_is_set (Gs [son_pid]->nl_Ai_set, rhs_prdct_k)) {
	  /* Il faut obligatoirement utiliser le FSA associe' a` l'arg k de la rhs */
	  SXBA_1_bit (tbp_nl_dag_arg_pos, arg_pos);
	}
      }
    }
  }
    
  if (nl_dag_iclause_k2pq && (nl_dag_iclause_k2pq [0].p+lhs_arity >= nl_dag_iclause_k2pq [0].q))
    nl_dag_iclause_k2pq = (struct nl_dag_exec_stack_struct*) sxrealloc (nl_dag_iclause_k2pq, (nl_dag_iclause_k2pq [0].q *= 2)+1, sizeof (struct nl_dag_exec_stack_struct));

  stmt_nb = *stmt_stack++;
  nl_dag_exec_stack_top = 0;
  cur_lhs_arg_pos = -1; /* a priori ds les 'and" */

  while (stmt_nb > 0) {
    operand = *stmt_stack++;
    stmt_nb--;
    
    if (operand < NL_AND) {
      /* operande */
      operator = *stmt_stack++;
      stmt_nb--;
    }
    else
      operator = operand;

    if (operand < NL_AND) {
      /* operande */
      if (operand >= 0) {
	/* c'est une position ... */
	if (operand >= lhs_arity) {
	  /* C'est un arg de la RHS */
	  if (SXBA_bit_is_set_reset (tbp_nl_dag_arg_pos, operand)) {
	    /* ... interessant */
	    cur_Aij = nl_dag_arg_pos2Aij [operand];
	    cur_k = nl_dag_arg_pos2k [operand];

	    if ((Aij_k_bot = nl_Aij_disp [cur_Aij]) == 0)
	      sxtrap (ME, "rcg_spf_process_dag_yield");

	    Aij_k = Aij_k_bot+cur_k;
	    pq = nl_dag_Aij_k2pq [Aij_k];
	  }
	}
	else {
#if EBUG
	  if (operator != NL_OR && operator != NL_OR_START && operator != NL_LOAD)
	    sxtrap (ME, "rcg_spf_process_dag_yield (found operator that is neither NL_OR, NL_OR_START or NL_LOAD on an LHS operand, which is not allowed)");
#endif /* EBUG */
	  if (operator == NL_LOAD) { /* argument de LHS top-down erasing, nécessairement mono-variable (sinon rcg_gen_parser a trappé) */
	    if (SXTRUE || unconditional || SXBA_bit_is_set_reset (tbp_nl_dag_arg_pos, cur_lhs_arg_pos)) {
	      if ((cur_Aij /* en fait c'est un Aij_pid */ = nl_dag_arg_pos2Aij [operand]) == 0)
		sxtrap (ME, "rcg_spf_process_dag_yield");
	      cur_k = nl_dag_arg_pos2k [operand];
	      Aij_k = (nl_Aij_disp [cur_Aij] + cur_k);

	      Aij_struct_ptr = (*(Gs [Aij_pid2pid (cur_Aij)]->Aij2struct))(Aij_pid2Aij (cur_Aij));
	      pq = create_pq_subdag_from_input_dag (Aij_struct_ptr->lb [cur_k], Aij_struct_ptr->ub [cur_k]);

	      nl_dag_exec_stack_push (pq);

	      if (cur_lhs_arg_pos < 0) {
		/* operande d'une intersection */
		sxba_empty (nl_dag_exec_stack2arg_pos [nl_dag_exec_stack_top]);
		SXBA_1_bit (nl_dag_exec_stack2arg_pos [nl_dag_exec_stack_top], operand);
	      }
	    }
	  } else {
	    if (operator == NL_OR_START) {
	      /* On est en train de traiter les stmt de cur_lhs_arg_pos */
	      if (cur_lhs_arg_pos == -1) {
		/* On est en train de traiter le 1er arg non vide.  On note l'indice de la pile
		   d'execution, ce qui se trouve au fond concerne les intersections */
		nl_dag_exec_stack_and_top = nl_dag_exec_stack_top;
	      }
	      
	      cur_lhs_arg_pos = operand;
	    }
	    else {
	      /* operator == NL_OR */
	      if (SXBA_bit_is_set_reset (tbp_nl_dag_arg_pos, cur_lhs_arg_pos)) {
		/* On est en train de traiter un arg de la lhs interessant */
		if ((cur_Aij = nl_dag_arg_pos2Aij [operand]) == 0)
		  sxtrap (ME, "rcg_spf_process_dag_yield");
		cur_k = nl_dag_arg_pos2k [operand];
		Aij_k = (nl_Aij_disp [cur_Aij] + cur_k);
		pq_ptr = nl_dag_Aij_k2pq + Aij_k;
		
		pq = nl_dag_exec_stack_pop ();
		
		if (nl_dag_iclause_k2pq)
		  nl_dag_iclause_k2pq [nl_dag_iclause_k2pq [0].p + cur_k] = pq;
		
		if (pq_ptr->p == 0)
		  /* 1ere fois qu'on trouve Aij_k */
		  *pq_ptr = pq;
		else
		  *pq_ptr =  nl_dag_or (*pq_ptr, pq);
		
#if LOG
		printf ("The subdag associated with the %ldth arg of ", cur_k+1);
		rcg_spf_print_Xpq (stdout, Aij);
		fputs (" is", stdout);
		print_sub_dag (&nl_dag_hd, &(nl_dag_Aij_k2pq [Aij_k]));
#endif /* LOG */
	      }
	    }
	  }

	  continue;
	}
      }
      else {
	/* On a un terminal ou une ref au resultat d'une intersection */
#if EBUG
	if ((operator != NL_TERM) && (operator != NL_DUP))
	  sxtrap (ME, "rcg_spf_process_dag_yield");
#endif /* EBUG */

	operand = -operand;

	if (operator == NL_TERM) {
	  /* operand est l'indice dand Tpq_stack du Tpq de ce terminal */
	  if (SXBA_bit_is_set (tbp_nl_dag_arg_pos, cur_lhs_arg_pos)) {
	    /* On est en train de traiter un arg de la lhs interessant */
	    nl_dag_load_symb (Tpq_stack [operand]);
	  }
	}
	else {
	  /* operator == NL_DUP */
	  if (FIRST_TIME) {
	    /* 1ere dup, on raze nl_lhs_arg_set */
	    FIRST_TIME = SXFALSE;
	    sxba_empty (nl_lhs_arg_set);
	  }

	  /* est-ce la 1ere fois qu'on duplique cet operande ? */
	  if (SXBA_bit_is_reset_set (nl_lhs_arg_set, operand)) {
	    /* oui */
	    SXBA arg_pos_set = nl_dag_exec_stack2arg_pos [operand];
	    SXINT tmp_arg_pos = sxba_scan (arg_pos_set, -1);
	    if (tmp_arg_pos >= 0 && sxba_scan (arg_pos_set, tmp_arg_pos) > 0)
	      /* il y a donc au moins 2 occurrences de la même variable dont les sous-DAG ont été intersectés en RHS
	       on va donc stocker le sous-DAG résultat de l'intersection en tant que sous-DAG "racine" pour cette variable,
	      c'est-à-dire pour chacun des Aij_k correspondant */
		nl_dag_exec_stack_back_and (operand);

	    if (SXBA_bit_is_set (tbp_nl_dag_arg_pos, cur_lhs_arg_pos)) {
	      /* Ici le resultat d'une intersection stocke' a l'indice operand de la
		 pile d'execution va etre utilise' dans la construction du FSA associe'
		 a l'arg no cur_lhs_arg_pos de la lhs */
	      nl_dag_exec_stack_dup (operand);
	    }
	  }
	  else {
	    if (SXBA_bit_is_set (tbp_nl_dag_arg_pos, cur_lhs_arg_pos)) {
	      nl_dag_exec_stack_push (nl_dag_copy (nl_dag_exec_stack [operand]));
	    }
	  }
	}

	continue;
      }
    }

    /* OPERATEUR */
    switch (operator >> 16) {
    case 1:
      /* #define NL_AND                  ((SXINT)0X10000) */
      if (!nl_dag_exec_stack_and ())
	return -1;

      break;

    case 2:
      /* #define NL_CAT                  ((SXINT)0X20000) */
      if (SXBA_bit_is_set (tbp_nl_dag_arg_pos, cur_lhs_arg_pos)) {
	nl_dag_exec_stack_cat ();
      }

      break;

    case 4:
      /* #define NL_LOAD                 ((SXINT)0X40000) */
      /* Le cas "terminal" a deja ete traite' */
      /* Si on charge un arg de la rhs, il faut le minimiser/copier si ce n'est pas la 1ere fois qu'on l'utilise
	 sauf s'il va etre utilise' ds un AND */
#if EBUG
      if (operand < lhs_arity)
	sxtrap (ME, "rcg_spf_process_dag_yield");
#endif /* EBUG */
	    
      if (cur_lhs_arg_pos < 0 || SXBA_bit_is_set (tbp_nl_dag_arg_pos, cur_lhs_arg_pos)) {
	if ((Aij_k = nl_Aij_disp [nl_dag_arg_pos2Aij [operand]]) == 0)
	  sxtrap (ME, "rcg_spf_process_dag_yield");

	Aij_k += nl_dag_arg_pos2k [operand];

	if (!SXBA_bit_is_reset_set (nl_dag_Aij_k_set, Aij_k)) {
	  /* Pas la 1ere fois */
	  /* On le copy */
	  pq = nl_dag_copy (pq);
	}

	nl_dag_exec_stack_push (pq);

	if (cur_lhs_arg_pos < 0) {
	  /* operande d'une intersection */
	  sxba_empty (nl_dag_exec_stack2arg_pos [nl_dag_exec_stack_top]);
	  SXBA_1_bit (nl_dag_exec_stack2arg_pos [nl_dag_exec_stack_top], operand);
	}
      }

      break;

    case 3:
      /* #define NL_OR                  ((SXINT)0X30000) */ 
      /* fait ds la foulee */
    case 5:
      /* #define NL_DUP                  ((SXINT)0X50000) */
      /* fait ds la foulee */
    default:
      sxtrap (ME, "rcg_spf_process_dag_yield");
    }
  }

#if EBUG
  if (!sxba_is_empty (tbp_nl_dag_arg_pos))
    sxtrap (ME, "rcg_spf_process_dag_yield");
#endif /* EBUG */

  rho--; /* On repointe vers l'adresse nominale */

  return 1;
}

static SXINT   *tok_no_stack;
static VARSTR   local_vstr;

/* Petit intermediaire local */
static SXINT
process_dag_yield (struct spf_node *cur_rcg_spf_node_ptr)
{
  return rcg_spf_process_dag_yield (cur_rcg_spf_node_ptr->G, cur_rcg_spf_node_ptr->rho, SXTRUE /* unconditional */);
}


static SXBOOLEAN
rcg_spf_is_a_suffix (char *string, SXINT string_lgth, char *suffix, SXINT suffix_lgth)
{
  /* On regarde si string se termine par "__" || suffix */
  char *top;

  if (string_lgth <= suffix_lgth+2)
    /* le prefixe doit etre non vide */
    return SXFALSE;

  top = suffix + suffix_lgth;
  string += string_lgth;

  while (--top >= suffix) {
    if (*top != *--string)
      return SXFALSE;
  }

  return string [-1] == '_' && string [-2] == '_';
}

static VARSTR
rcg_spf_concat_ff_tstr (VARSTR local_vstr, char *ff, SXINT t)
{
  SXINT lgth;
  char* tstr;


  tstr = glbl_t2string [t];
  lgth = strlen (tstr);

  local_vstr = varstr_catenate_with_escape (local_vstr, ff, "%()[]{|\\");

  if (!rcg_spf_is_a_suffix (ff, strlen (ff), tstr, lgth)) {
    local_vstr = varstr_catchar (local_vstr, '_');
    local_vstr = varstr_catchar (local_vstr, '_');
    local_vstr = varstr_catenate (local_vstr, tstr);
  }
  return local_vstr;
}

extern SXINT   get_SEMLEX_lahead (void); /* Ds dag_scanner, retourne 0 si pas de SEMLEX */

static VARSTR
rcg_spf_tok_no2varstr_catenate (VARSTR local_vstr, SXINT tok_no, SXINT t)
{
  struct sxtoken *ptoken;
  char           *comment = NULL, *ff_or_tstring;

  ptoken = &(SXGET_TOKEN (tok_no));
  
  comment = ptoken->comment;
  
  if (comment) {
    local_vstr = varstr_catenate (local_vstr, comment);
    local_vstr = varstr_catchar (local_vstr, ' ');
  }
  
  ff_or_tstring = sxstrget (ptoken->string_table_entry);
  
  if (t == 0) /* ff_or_tstring est une tstring */
    local_vstr = varstr_catenate (local_vstr, ff_or_tstring);
  else /* ff_or_tstring est une ff */
    local_vstr = rcg_spf_concat_ff_tstr (local_vstr, ff_or_tstring, t);	/* Ca permet eventuellement de suffixer ff par des "__cat" ou autre */
  
  if (tok_no < idag.toks_buf_top) {
    /* pas eof ni tok_no de la rcvr */
    /* Traitement des %SEMLEX ; devrait ne jamais être utilisé dans le cas is_dag, en raison du fait
       que SEMLEX_lahead, récupéré par get_SEMLEX_lahead (), est alors toujours nul */
    ptoken = &(SXGET_TOKEN (tok_no+1));

    if (ptoken->lahead == get_SEMLEX_lahead ()) {
      /* Oui */ 
      local_vstr = varstr_catchar (local_vstr, ' ');
    
      if (ptoken->comment) {
	local_vstr = varstr_catenate (local_vstr, ptoken->comment);
	local_vstr = varstr_catchar (local_vstr, ' '); 
      }
    
      local_vstr = varstr_catchar (local_vstr, '['); 
      local_vstr = varstr_catchar (local_vstr, '|'); 
      local_vstr = varstr_catenate (local_vstr, sxstrget (ptoken->string_table_entry));
      local_vstr = varstr_catchar (local_vstr, '|');
      local_vstr = varstr_catchar (local_vstr, ']');
      local_vstr = varstr_complete (local_vstr);
    }
  }

  return local_vstr;
}

/* Procedure appelee par dag2re qui permet de recuperer le "texte" associe' aux transitions qui sont ici des tok_no */
/* Marche avec le (vrai) source et non pas le source lexicalise' ds le cas insideG */
static char*
rcg_spf_get_yield_trans_name (SXINT pqt)
{
  SXINT t, nb, tok_no, *top_tok_no_stack, *cur_tok_no_ptr;

  varstr_raz (local_vstr);
    
  /* ici on a donc un dico ff->terminaux, une transition du yield de la forêt peut devoir prendre la forme d'une saucisse locale avec toutes les ff ayant le bon terminal parmi leurs terminaux */
  
  DRAZ (tok_no_stack);
  idag_p_t_q2tok_no_stack (&tok_no_stack, pqt2p (pqt), pqt2t (pqt), pqt2q (pqt));
  nb = DTOP (tok_no_stack);
  
#if EBUG
  if (nb == 0)
    sxtrap (ME, "rcg_spf_get_yield_trans_name (no token found for a given pqt)");
#endif /* EBUG */	
  
  cur_tok_no_ptr = tok_no_stack;

  tok_no = *++cur_tok_no_ptr;
  if (input_dag_transition_names_are_ffs())
    t = pqt2t (pqt);
  else 
    t = 0;
    
  if (nb == 1) {
    local_vstr = rcg_spf_tok_no2varstr_catenate (local_vstr, tok_no, t);
  } 
  else {
    top_tok_no_stack = tok_no_stack + nb;
    local_vstr = varstr_catenate (local_vstr, "(");
    
    do {
      tok_no = *cur_tok_no_ptr;
      
      local_vstr = rcg_spf_tok_no2varstr_catenate (local_vstr, tok_no, t);
      local_vstr = varstr_catenate (local_vstr, (cur_tok_no_ptr < top_tok_no_stack) ? " | " : ")");
    } while (++cur_tok_no_ptr <= top_tok_no_stack);
  }    

  return varstr_tostr (local_vstr);
}

/* Procedure appelee par dag2re qui permet d'extraire de spf_dag_yield_hd les transitions depuis l'etat p */
static void
rcg_spf_dag_yield_extract_trans (SXINT p, void (*f) (SXINT p, SXINT t, SXINT q))
{
  SXINT triple, pqt, q;

  XxYxZ_Xforeach (nl_dag_hd, p, triple) {
    pqt = XxYxZ_Y (nl_dag_hd, triple);
    q = XxYxZ_Z (nl_dag_hd, triple);
    
    (*f) (p, pqt, q);
  }
}


#define UDAG_wanted SXFALSE

/* Sort ds le fichier dag_yield_file le DAG passe' en parametre */
static void
rcg_spf_output_dag_yield (struct nl_dag_exec_stack_struct *pq_ptr)
{
  if (UDAG_wanted) {
    /* A FAIRE */
  }
  else {
    VARSTR vstr;

    vstr = varstr_alloc (256);
    local_vstr = varstr_alloc (64);

    DALLOC_STACK (tok_no_stack, idag.ptq_nb - idag.toks_buf_top + 1 /* calculé au plus juste ;-) */);

    vstr = dag2re (vstr,
		   pq_ptr->p /* etat initial */,
		   pq_ptr->q /* etat final */,
		   MAX_pqt()+1 /* yield_eof_ste */,
		   rcg_spf_dag_yield_extract_trans,
		   rcg_spf_get_yield_trans_name);
    fprintf (dag_yield_file, "%s\n", varstr_tostr (vstr));
    varstr_free (local_vstr);
    varstr_free (vstr);
    DFREE_STACK (tok_no_stack);
  }
}

/* sortie du yield de la spf sous forme de [U]DAG */
void
rcg_spf_dag_yield (void)
{
  struct nl_dag_exec_stack_struct *pq_ptr;

  /* Allocation des variables utilisees */
  rcg_spf_dag_yield_alloc (SXTRUE, SXFALSE);

  /* La spf est parcourue bottom-up et l'on synthetise le DAG du yield */
  rcg_spf_topological_bottom_up_walk (rcg_spf.start_symbol, process_dag_yield, NULL, NULL);

  pq_ptr = nl_dag_Aij_k2pq + nl_Aij_disp [rcg_spf.start_symbol];
  
  /* Si pas det/min on le fait */
  if (pq_ptr->kind)
    *pq_ptr = nl_dag_minimize (*pq_ptr);

  /* On sort le DAG associe a l'axiome */
  rcg_spf_output_dag_yield (pq_ptr);

  /* On libere le tout */
  rcg_spf_dag_yield_free ();

  if (is_print_time) {
    sxtime (TIME_FINAL, "\t\tDag yield output (done)");
  }
}


/* Procedures de la seconde passe top-down, cas non-lineaire */
static SXINT Aij_iclause_nb;

/* On regarde s'il y a plusieurs Aij-iclauses */
static SXINT
rcg_spf_nl_second_pass_td_pre (SXINT Aij)
{
  struct spf_node   *cur_rcg_spf_node_ptr;

  Aij_iclause_nb = 0;

  for (cur_rcg_spf_node_ptr = GET_RCG_SPF_HEAD_NODE_PTR (Aij);
       cur_rcg_spf_node_ptr != NULL;
       cur_rcg_spf_node_ptr = cur_rcg_spf_node_ptr->next) {
    if (++Aij_iclause_nb > 1)
      break;
  }
  
  return Aij_iclause_nb == 0 ? -1 : 1;
}

/* On cherche recursivement l'etat final qui correspond a l'etat j de idag */
static SXINT
seek_j (SXINT *minp_ptr, SXINT *maxp_ptr,
	SXINT i /* pos dans l'input correspondant à l'état r */, SXINT r,
	SXINT j /* pos dans l'input pour laquelle on veut trouver un état accessible depuis r */,
	SXINT q /* état tel que si on l'atteint on est trop loin */,
	SXINT global_minp, SXINT global_maxp)
{
  SXINT v, ptq, Tij, s, u;
  SXINT i2;

  if (r < global_minp) /* prudence: aucune idée de si ce cas peut effectivement se produire */
    /* On est sur un chemin qui déborde par le bas */
    /* Echec */
    return 0;

  if (r > global_maxp) /* prudence: aucune idée de si ce cas peut effectivement se produire */
    /* On est sur un chemin qui déborde par le haut */
    /* Echec */
    return 0;

  if (i == j) {
    if (r < *minp_ptr)
      *minp_ptr = r;
    if (r > *maxp_ptr)
      *maxp_ptr = r;
    return r;
  }
  if (i > j) /* on déborde côté source: échec */
    return 0;

  if (r == q) /* on a atteint le bout et ledit bout ne correspond pas au j recherché: échec */
    return 0;

  v = 0;

  XxYxZ_Xforeach (nl_dag_hd, r, ptq) {
    Tij = XxYxZ_Y (nl_dag_hd, ptq);
    s = XxYxZ_Z (nl_dag_hd, ptq);
    i2 = pqt2q (Tij);

    u = seek_j (minp_ptr, maxp_ptr, i2, s, j, q, global_minp, global_maxp);

    if (u < 0)
      /* multiple */
      return -1;

    if (u) {
      /* On a un bon chemin */
      if (v == 0 || v == u)
	v = u; /* Le 1er */
      else
	/* Multiple !! */
	return -1;

      if (s < *minp_ptr)
	*minp_ptr = s;

      if (s > *maxp_ptr)
	*maxp_ptr = s;
    }
  }

  return v; /* >= 0 */
}


/* "instancie" le DAG associe' a RHS pos */
static SXBOOLEAN
process_rhs_arg_pos (SXINT lhs_arity, SXINT rhs_arg_pos)
{
  SXINT                           lhs_arg_pos, p, q, i, j, init_state_nb, final_state_nb, r, r2, ptq, Tij;
  SXINT                           minp, maxp, global_minp, global_maxp; 
  struct nl_dag_exec_stack_struct pq, pq2;
  struct nl_dag_what_struct       *lhs_what_ptr, *rhs_what_ptr;
  SXBOOLEAN                       ret_val = SXTRUE;

  rhs_what_ptr = nl_dag_arg_pos2what+rhs_arg_pos;
  /* rhs_what_ptr repere l'ensemble des infos necessaires a ce calcul qui doit remplir/completer
   *(rhs_what_ptr->pq_ptr) */
  /* Les bornes du range de cet arg */
  /* Ce range est un sous-range d'au moins un range (arg) de la LHS, on recherche ce/ces args */
  i = rhs_what_ptr->i;
  j = rhs_what_ptr->j;

  /* prudence... */
  RAZ (nl_dag_lb_stack);
  RAZ (nl_dag_ub_stack);
  RAZ (nl_dag_minp_stack);
  RAZ (nl_dag_maxp_stack);

  /* on va chercher dans chaque arg de la lhs si son range a un recouvrement non nul avec le range couvert par l'arg de rhs à traiter. Le cas échéant, on extrait de l'arg lhs la sous partie du sous-DAG qui lui associé qui correspond au range de l'arg rhs. On accumule les sous-DAG ainsi obtenus via des piles */
  for (lhs_arg_pos = 0; lhs_arg_pos < lhs_arity; lhs_arg_pos++) {
    lhs_what_ptr = nl_dag_arg_pos2what+lhs_arg_pos;

    if (lhs_what_ptr->pq_ptr /* argument utilisable */
	&& !(lhs_what_ptr->pq_ptr->p == 0 && lhs_what_ptr->pq_ptr->q == 0) /* et qu'il a été renseigné (car il véhicule des contraintes) */
	&& lhs_what_ptr->i <= i && lhs_what_ptr->j >= j) {
      /* sub_range de lhs_arg_pos */
      /* On extrait de lhs_arg_pos le sous-dag de range [i..j] */
      if (lhs_what_ptr->pq_ptr->kind != 0)
	/* Ce sous-dag ne doit plus avoir de trans epsilon (il a ete reduit/minimise'/normalise' avant) */
	sxtrap (ME, "process_rhs_arg_pos (the reduced and minimized working sub-DAG at hand should not contain any epsilon transitions)");
      
      /* p et q sont son etat initial et final */
      p = lhs_what_ptr->pq_ptr->p;
      q = lhs_what_ptr->pq_ptr->q;

      /* Traitement des Empty ranges A FAIRE */
      /* On cherche les etats initiaux et finals de ce DAG qui correspondent a i et j */
      init_state_nb = final_state_nb = 0;
      
      global_minp = lhs_what_ptr->pq_ptr->minp;
      global_maxp = lhs_what_ptr->pq_ptr->maxp;

      minp = p; /* inutile, pour la beauté du geste */
      maxp = 0; /* inutile, pour la beauté du geste */
      r = seek_j (&minp /* pas grave, on le réécrase juste en dessous */, &maxp /* idem */, lhs_what_ptr->i, p, i, q, global_minp, global_maxp);
      if (r > 0) {
	/* On a trouvé un et un seul r qui correspond à la position i dans le source */
	minp = r;
	maxp = 0;
	r2 = seek_j (&minp, &maxp, i, r, j, q, global_minp, global_maxp);

	if (r2 < 0) /* prudence: aucune idée de si ce cas peut effectivement se produire */
	  /* Multiple (plusieurs états finaux possibles) !! */
	  final_state_nb = 2;
	
	if (r2 > 0) {
	  /* ... qui conduit a un s unique correspondant à la position j dans le source */
	  init_state_nb = 1;
	  final_state_nb = 1;
	  PUSH (nl_dag_lb_stack, r);
	  PUSH (nl_dag_ub_stack, r2);
	  PUSH (nl_dag_minp_stack, minp);
	  PUSH (nl_dag_maxp_stack, maxp);
	}
      }
      else {
	if (r < 0) /* prudence: aucune idée de si ce cas peut effectivement se produire */
	  /* Multiple (plusieurs états initiaux possibles) !! */
	  init_state_nb = 2;
      }

      if (init_state_nb == 0)
	/* Y'en a pas !! */
	sxtrap (ME, "process_rhs_arg_pos (no init state)");

      if (final_state_nb == 0)
	/* Y'en a pas !! */
	sxtrap (ME, "process_rhs_arg_pos (no final state)");

      if (init_state_nb > 1 || final_state_nb > 1) {
	/* damned, il faut la jouer fine */
	/* Je ne suis meme pas sur que c,a puisse se produire !!
	   Pour l'instant detection et trap */
#if 0
	pq = nl_dag_multi_copy (nl_dag_lb_stack, nl_dag_ub_stack);
#endif /* 0 */

	sxtrap (ME, "process_rhs_arg_pos (more than one init or final state)");
      }
    }
  }

  init_state_nb = TOP (nl_dag_lb_stack);

#if EBUG
  final_state_nb = TOP (nl_dag_ub_stack);

  if (init_state_nb != final_state_nb)
    sxtrap (ME, "process_rhs_arg_pos (not the same number of init and final states)");
#endif /* EBUG */

  if (init_state_nb) {
    pq.p = POP (nl_dag_lb_stack);
    pq.q = POP (nl_dag_ub_stack);
    pq.minp = POP (nl_dag_minp_stack);
    pq.maxp = POP (nl_dag_maxp_stack);
    pq.kind = 0; /* pas d'eps trans */

    if (init_state_nb > 1) {
      /* clause est non lineaire a gauche */
      /* Il faut faire les intersections ... */
      /* ... ds un pq local */
      pq = nl_dag_copy (pq);

      while (!IS_EMPTY (nl_dag_lb_stack)) {
	/* Elles se correspondent 1 a 1 */
	pq2.p = POP (nl_dag_lb_stack);
	pq2.q = POP (nl_dag_ub_stack);
	pq2.minp = POP (nl_dag_minp_stack);
	pq2.maxp = POP (nl_dag_maxp_stack);
	pq2.kind = 0; /* pas d'eps trans */

	if (!nl_dag_and (&pq, pq2)) {
	  /* vide */
	  ret_val = SXFALSE;

	  break;
	}
      }
    }
  }

  /* 
     Ds le cas general pq est un sous-dag d'un dag normalise', il est donc aussi
     normalise'.  On le reutilise donc tel quel en RHS
   */
  if (init_state_nb == 0) /* aucune contrainte n'a été trouvée en LHS, tout va bien */
    return SXTRUE;

  if (ret_val) {
    if (rhs_what_ptr->pq_ptr->p == 0) {
      /* C'est la 1ere fois qu'on trouve cet arg (de ce predicat instancie) en rhs */
      *rhs_what_ptr->pq_ptr = pq;
    }
    else {
      if (!nl_dag_and (rhs_what_ptr->pq_ptr, pq)) /* était incompréhensiblement un nl_dag_or à un moment... */
	/* vide */
	ret_val = SXFALSE;
    }
  }

#if LOG
  printf ("The argument #%ld of ", (long) rhs_what_ptr->k+1);
  rcg_spf_print_Xpq (stdout, rhs_what_ptr->Aij);
  fputs (" is associated with", stdout);
  print_sub_dag (&nl_dag_hd, rhs_what_ptr->pq_ptr);
#endif /* LOG */

  return ret_val;
}



/* La spf et les sous-dag ont ete construits bottom_up et la foret est connexe */
/* Fonction principale appele'e par la passe top-down sur la spf et qui va propager les sous-dags associes a chaque Aij */
static SXINT
rcg_spf_nl_second_pass_td (struct spf_node *cur_rcg_spf_node_ptr)
{
  SXINT                           clause, iclause, Aij, lhs_arity, k, Xpq, x, son_nb, arg_pos, son, rhs_prdct_arity;
  SXINT                           A, Ak, son_pid, rhs_prdct, rhs_prdct_k;
  SXINT                           *rho;
  struct nl_dag_exec_stack_struct *Aij_pq_stack, *iclause_pq_stack;
  struct G                        *G;
  struct nl_dag_what_struct       *what_ptr;
  struct Aij_struct               *Aij_struct_ptr;
  struct nl_dag_exec_stack_struct pq, *pq_ptr;

#if LOG
  fputs ("rcg_spf_nl_second_pass_td: ", stdout);
  rcg_spf_print_spf_node (cur_rcg_spf_node_ptr);
#endif /* LOG */

  G = cur_rcg_spf_node_ptr->G;
  rho = &(cur_rcg_spf_node_ptr->rho [0]);
  clause = *rho++;

  if (!G->is_a_complete_terminal_grammar && !SXBA_bit_is_set (G->non_right_linear_clause_set, clause))
    /* On est ds le cas non lineaire et cette clause ne participe pas aux calculs de non linearite' */
    return 1;

  /* La`, il y a qqchose a faire */

  son_nb = G->clause2son_nb ? G->clause2son_nb [clause] : 0;
  /* lhs_prdct_name = G->clause2lhs_nt [clause]; */

  /* Meme si la RHS est vide, il faut distribuer le sous-dag associe a Aij sur ses Aij-prods */
  iclause = cur_rcg_spf_node_ptr->instantiated_clause_nb;
  Aij = *rho;
  Aij_struct_ptr = call_instantiated_prdct2struct (Aij);
  A = G->clause2lhs_nt [clause];

#if EBUG
  if (A != Aij_struct_ptr->A)
    sxtrap (ME, "rcg_spf_nl_second_pass_td (inconsistent information about left-hand predicate name)");
#endif /* EBUG */
  
  lhs_arity = Aij_struct_ptr->arity;

  Aij_pq_stack = nl_dag_Aij_k2pq + nl_Aij_disp [Aij];
  iclause_pq_stack = nl_dag_iclause_k2pq + nl_dag_iclause2iclause_k_disp [iclause];

  /* Pour chaque arg de la LHS, nl_dag_Aij_k2pq repere le sous dag construit bu au cours de
     la 1ere passe */
  /* Pour chaque arg on doit faire l'intersection entre les sous-dags
     du predicat Aij (qui viennent d'"en haut" top-down, et sont le
     résultat de la passe bottom-up) et ceux de la iclause courante */
  /* Cette intersection est inutile si la iclause courante est la seule Aij-clause */
  for (arg_pos = 0, Ak = G->A2A0 [A]; arg_pos < lhs_arity; arg_pos++, Ak++) {
    if (SXBA_bit_is_set (G->nl_Ai_set, Ak)) {
      /* On a calcule' un FSA au cours de la 1ere passe bu sur l'arg arg_pos du prdct en lhs */
      if (Aij_iclause_nb /* Positionnee par pre */ == 1) {
	pq = Aij_pq_stack [arg_pos];

	if (pq.kind) /* i.e., if il y a des epsilon-trans */
	  /* On vire les trans epsilon */
	  pq = nl_dag_minimize (pq) ;

	iclause_pq_stack [arg_pos] = pq;
      }
      else {
	if (!nl_dag_and (&(iclause_pq_stack [arg_pos]), Aij_pq_stack [arg_pos]))
	  /* Echec sur tout le sous-arbre */
	  return -1;
      }
    }

    if (son_nb) {
      what_ptr = nl_dag_arg_pos2what+arg_pos;
      what_ptr->Aij = Aij;
      what_ptr->i = Aij_struct_ptr->lb [arg_pos];
      what_ptr->j = Aij_struct_ptr->ub [arg_pos];
      what_ptr->k = arg_pos;
      what_ptr->pq_ptr = SXBA_bit_is_set (G->nl_Ai_set, Ak) ? &(iclause_pq_stack [arg_pos]) : NULL;
    }
  }

  if (son_nb) {
    /* La rhs est non vide */
    /* en fait il faudrait etre + fin : on ne doit rien faire s'il n'y a en RHS
       que des args qui ne supportent pas de DAGs (compteurs, predicats predefinis, ...) */
    /* On n'utilise ni G->nl_clause2rhs_pos_disp ni G->nl_rhs_pos2pos_stack_disp  ni G->nl_pos_stack !! */
    /* Ca ne gagnerait pas grand chose ? */
    for (son = 1; son <= son_nb; son++) {
      Xpq = rho [son];

      if (Xpq < 0)
	/* neg call !! */
	sxtrap (ME, "bu_nl_rcg_processing (negative clause found while parsing a DAG: this is not implemented yet)");
		    
      Aij_struct_ptr = call_instantiated_prdct2struct (Xpq);
      son_pid = Aij_pid2pid (Xpq);
      
      /* rhs_prdct_name = Aij_struct_ptr->A; */
      rhs_prdct_arity = Aij_struct_ptr->arity /* marche aussi sur les predefinis */;
      rhs_prdct = Aij_struct_ptr->A;
      pq_ptr = nl_dag_Aij_k2pq + nl_Aij_disp [Xpq];

      for (k = 0, rhs_prdct_k = Gs [son_pid]->A2A0 [rhs_prdct]; k < rhs_prdct_arity; k++, arg_pos++, rhs_prdct_k++) { /* pour chaque argument de RHS... */
	if (SXBA_bit_is_set (Gs [son_pid]->nl_Ai_set, rhs_prdct_k)) {
	  what_ptr = nl_dag_arg_pos2what+arg_pos;
	  what_ptr->Aij = Xpq;
	  what_ptr->k = k;
	  what_ptr->i = Aij_struct_ptr->lb [k];
	  what_ptr->j = Aij_struct_ptr->ub [k];
	  what_ptr->pq_ptr = pq_ptr++;

	  if (!process_rhs_arg_pos (lhs_arity, arg_pos))
	    return -1;
	}
      }
    }
  }

  return 1;
}


static SXINT
rcg_spf_nl_pass_bu (struct spf_node *cur_rcg_spf_node_ptr)
{
  SXINT ret_val, iclause, clause, A, lhs_arity;
  struct G *G;

  ret_val = rcg_spf_process_dag_yield (cur_rcg_spf_node_ptr->G, cur_rcg_spf_node_ptr->rho,
				       /* !unconditional */ SXFALSE
				       );
  /* On est ds le cas non-lineaire, on note qq petites choses pour la passe top-down */

  /* 
     Si la clause est non-lineaire a droite on a plusieurs args de la RHS qui sont de la forme disons variable "X"
     qui s'instancie dans le range [i..j].  A chacun de ces arg arg_pos est associe un dag dag_pos dont le langage est un sous-langage
     du langage d'entree du sous-dag <i..j>.  L'intersection de ces dag_pos a ete repercutee dans les args de la LHS.  Cette intersection
     ne doit PAS etre repercutee aussi sur les arg_pos de la RHS car un des dag_pos peut reintervenir ulterieurement ds une autre RHS
  */

  /*
    Supposons qu'on ait la clause
    GN_MS(X) --> GN_M(X) GN_S(X) .
    Un groupe nominal masculin singulier est l'intersection d'un groupe nominal masculin et d'un groupe nominal singulier.
    et que X s'instancie dans un range qui correspond au source (un chat | les chats | une chatte | les chattes) et que 
    les dags de GN_M(X) et GN_S(X) soient respectivement (un chat | les chats) et (un chat | une chatte)
    Le DAG de la LHS sera donc (un chat)
  */


  /* Pour chaque clause instanciee on stocke une liste de doublets
     (range, id)
     avec la signification suivante :
     Un range correspond a une variable disons X qui est un arg en RHS
     G->nl_pos_stack + id est une stack qui repere (de gauche a droite) les arg de la clause courante
     ds lequel intervient X */

  /* On note qq petites choses pour la passe td */

  if (ret_val != -1) {
    iclause = cur_rcg_spf_node_ptr->instantiated_clause_nb;
    nl_dag_iclause2iclause_k_disp [iclause] = nl_dag_iclause_k2pq [0].p;

    clause = cur_rcg_spf_node_ptr->rho [0];
    G = cur_rcg_spf_node_ptr->G;
    A = G->clause2lhs_nt [clause];
    lhs_arity = G->nt2arity ? G->nt2arity [A] : 1;

    nl_dag_iclause_k2pq [0].p += lhs_arity;
  }

  return ret_val;
}



SXBOOLEAN
rcg_spf_nl_pass_bu_do_it (void)
{
  return rcg_spf_topological_bottom_up_walk (rcg_spf.start_symbol, rcg_spf_nl_pass_bu, NULL, NULL);
}


SXINT
rcg_spf_nl_pass_td_do_it (void)
{
  SXINT                           Aij_k, top;
  struct nl_dag_exec_stack_struct pq;
  SXINT                           ret_val;
#if LOG
  SXINT                           Aij, A, A_arity, pid, k;
  SXINT                           *Aij_k2Aij;
  struct Aij_struct               *Aij_struct_ptr;
#endif /* LOG */

#if LOG
  fputs ("\nEntering rcg_spf_nl_pass_td_do_it ()\n", stdout);
#endif /* LOG */

#if LOG
  Aij_k2Aij = (SXINT *) sxalloc (nl_dag_Aij_k2pq [0].p, sizeof (SXINT));

  for (Aij = 1; Aij <= rcg_spf.max_instantiated_prdct_nb; Aij++) {
    if ((Aij_k = nl_Aij_disp [Aij])) {
      Aij_struct_ptr = call_instantiated_prdct2struct (Aij);
      A = Aij_struct_ptr->A;
      pid = Aij_pid2pid (Aij);
      A_arity = Gs [pid]->nt2arity ? Gs [pid]->nt2arity [A] : 1;

      for (k = 0; k < A_arity; k++, Aij_k++) {
	Aij_k2Aij [Aij_k] = Aij;
      }
    }
  }
#endif /* LOG */

  /* On raz nl_dag_Aij_k2pq, on va recalculer complètement les DAGs partout, sauf au niveau des racines, dont on pourra récupérer les DAGs via nl_dag_Aij_k_root_stack */
  for (Aij_k = nl_dag_Aij_k2pq [0].p /* le TOP (l'élément 0 est spécial) */ - 1 /* pour garder celui de l'unique argument de l'axiome! (???) */; Aij_k > 0; Aij_k--) {
    nl_dag_Aij_k2pq [Aij_k] = empty_pq;
  }

  top = DTOP (nl_dag_Aij_k_root_stack);

  while (top) {
    Aij_k = DPOP (nl_dag_Aij_k_root_stack);
    pq = nl_dag_Aij_k2pq [Aij_k];

#if LOG
    Aij = Aij_k2Aij [Aij_k];
    k = Aij_k-nl_Aij_disp [Aij];
    printf ("The subdag associated with the %ldth 'root' arg of ", k+1);
    rcg_spf_print_Xpq (stdout, Aij);
    fputs (" is ", stdout);
#endif /* LOG */

    if (pq.p == 0) {
      /* vide */
      nl_dag_Aij_k2pq [Aij_k] = nl_dag_Aij_k_root2pq [top];
#if LOG
      print_sub_dag (&nl_dag_hd, &(nl_dag_Aij_k2pq [Aij_k]));
#endif /* LOG */
    }
    else {
      nl_dag_Aij_k2pq [Aij_k] = nl_dag_or (pq, nl_dag_Aij_k_root2pq [top]);
    }

    top--;
  }
  
  DFREE_STACK (nl_dag_Aij_k_root_stack);
  sxfree (nl_dag_Aij_k_root2pq), nl_dag_Aij_k_root2pq = NULL;

  /* On alloue les structures necessaires a cette seconde passe */
  nl_dag_arg_pos2what = (struct nl_dag_what_struct*) sxalloc (global_max_arg_nb_per_clause+1, sizeof (struct nl_dag_what_struct));
  nl_dag_lb_stack = (SXINT*) sxalloc (global_arity+1, sizeof (SXINT)), TOP (nl_dag_lb_stack) = 0;
  nl_dag_ub_stack = (SXINT*) sxalloc (global_arity+1, sizeof (SXINT)), TOP (nl_dag_ub_stack) = 0;
  nl_dag_minp_stack = (SXINT*) sxalloc (global_arity+1, sizeof (SXINT)), TOP (nl_dag_minp_stack) = 0;
  nl_dag_maxp_stack = (SXINT*) sxalloc (global_arity+1, sizeof (SXINT)), TOP (nl_dag_maxp_stack) = 0;

  /* C'est parti ... */
  rcg_spf_topological_top_down_walk (rcg_spf.start_symbol, rcg_spf_nl_second_pass_td, rcg_spf_nl_second_pass_td_pre, NULL);

  /* On libere les structures necessaires a cette seconde passe */
  sxfree (nl_dag_arg_pos2what), nl_dag_arg_pos2what = NULL;
  sxfree (nl_dag_lb_stack), nl_dag_lb_stack = NULL;
  sxfree (nl_dag_ub_stack), nl_dag_ub_stack = NULL;
  sxfree (nl_dag_minp_stack), nl_dag_minp_stack = NULL;
  sxfree (nl_dag_maxp_stack), nl_dag_maxp_stack = NULL;

  if (!rcg_spf.is_proper) {
    if (GET_RCG_SPF_HEAD_NODE_PTR (rcg_spf.start_symbol) == NULL) /* l'axiome a été erasé, la forêt est donc devenue vide à cause des intersections de DAGs */
      ret_val = -1;
    else
      ret_val = rcg_spf_make_proper (rcg_spf.start_symbol) ? 1 : -1;
  } else 
    ret_val = 1;

#if LOG
  fprintf (stdout, "Leaving rcg_spf_nl_pass_td_do_it (), ret_val=%ld\n", ret_val);
#endif /* LOG */

  return ret_val;
}



/* ************************************************************************************ */
