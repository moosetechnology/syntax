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

/*
  Regroupe les operations sur la foret d'analyse partagee produite
  par les analyseurs non-deterministes
*/

/* Les parcours de la spf */
/* Pour l'utilisateur la spf est non cyclique.  Si elle l'etait apres la phase
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
   -1 => la production Pij est elimine'e de la spf
*/

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXSPF_MNGR[] = "@(#)SYNTAX - $Id: sxspf_mngr.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

static char	ME [] = "sxspf_mngr";

/* Pour ne pas avoir ici la definition et l'initialisation de spf proposée par sxspf.h,
   on ne fait pas #define SX_DFN_EXT_VAR2 */
#include "XxYxZ.h"
#include "XH.h"
#include "earley.h"
#include "varstr.h"
#include "sxspf.h"
#include "fsa.h"
#include "sxstack.h"
#include "udag_scanner.h"


static SXINT          Axixj2Aij_hd_foreach [6] /* Pas de foreach */
                    , Txixj2Tij_hd_foreach [6] =  {0, 1, 0, 0, 0, 0} /* Yforeach par Tij_iforeach */
                    ;

static SXINT        outputG_lhs_size, outputG_rhs_size;
static SXINT        *Aij2lhs_nb;
static SXINT        count_maxprod, count_maxitem;
static SXBA         count_Tij_set, count_Aij_set;
static FILE         *spf_print_forest_out_file;

static VARSTR       wvstr;

#if EBUG
static SXBA         spf_make_proper_check_td_set, spf_make_proper_check_bu_set;
#endif /* EBUG */

static void
Axixj2Aij_hd_oflw (SXINT old_size, SXINT new_size)
{
  if (Aij2lhs_nb) Aij2lhs_nb = (SXINT*) sxrecalloc (Aij2lhs_nb, old_size+1, new_size+1, sizeof (SXINT));
  if (spf.outputG.Aij2rhs_nb) spf.outputG.Aij2rhs_nb = (SXINT*) sxrecalloc (spf.outputG.Aij2rhs_nb, old_size+1, new_size+1, sizeof (SXINT));

  /* Truc pour eviter que ds spf_print_iprod on ecrive des nt instancies du
     traitement d'erreurs */
  spf.outputG.maxnt = new_size+1;
}

static void
Txixj2Tij_hd_oflw (SXINT old_size, SXINT new_size)
{
  sxuse(old_size); /* pour faire taire gcc -Wunused */
  if (spf.outputG.Tpq_rcvr_set) spf.outputG.Tpq_rcvr_set = sxba_resize (spf.outputG.Tpq_rcvr_set, new_size+1);
}


static SXINT
spf_print_td_or_bu_prod (SXINT Pij)
{
  spf_print_iprod (spf_print_forest_out_file, Pij, "", "\n");

  return 1;
}


static SXINT
call_spf_print_iprod (SXINT prod)
{
  spf_print_iprod (stdout, prod, "", "\n");
  return 1;
}


static SXINT
spf_tree_count_pass_derived (SXINT Pij)
{
  double     tree_nb = (double)1;
  SXINT        item, Xpq, Tpq, Aij;
  struct lhs *plhs;

  plhs = spf.outputG.lhs+Pij;

  if (count_Aij_set) {
    /* On veut des stats completes sur la grammaire */
    count_maxprod++;
    count_maxitem++;

#if LLOG
    printf ("%ld: ", count_maxprod);
    spf_print_iprod (stdout, Pij, "", "\n");
#endif /* LLOG */
  
    Aij = plhs->lhs /* static */;
    SXBA_1_bit (count_Aij_set, Aij); /* prudence */
  }

  item = spf.outputG.lhs [Pij].prolon;

  while ((Xpq = spf.outputG.rhs [item].lispro) != 0) {
    if (count_Aij_set) {
      count_maxitem++;

      if (Xpq < 0) {
	/* terminal */
	Tpq = -Xpq;
	SXBA_1_bit (count_Tij_set, Tpq);
      }
      else
	SXBA_1_bit (count_Aij_set, Xpq);
    }

    if (Xpq > 0 /* nt ... */ && (Xpq <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Xpq)) /* ... productif */)
      tree_nb *= spf.tree_count.nt2nb [Xpq];

    item++;
  }

  spf.tree_count.prod2nb [Pij] = spf.tree_count.nt2nb [spf.outputG.lhs [Pij].lhs] += tree_nb;
  /* Cumule ds prod2nb les nb associes aux prods precedentes */
  /* ... c,a permet de la recherche dichotomique ds spf_dag2tree */
  
  return 1;
}



#if EBUG
static SXINT
spf_make_proper_check_td (SXINT Pij)
{
  SXBA_1_bit (spf_make_proper_check_td_set, Pij);

  return 1;
}

static SXINT
spf_make_proper_check_bu (SXINT Pij)
{
  SXBA_1_bit (spf_make_proper_check_bu_set, Pij);

  return 1;
}
#endif /* EBUG */


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

/* Corps recursif */
static bool
spf_walk (SXINT Aij)
{
  SXINT             item, Xpq, hook, base_hook, result_kind, pd_kind, pi_kind, Pij;
  bool         must_be_kept, Pij_kept;

  sxinitialise(result_kind); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(pi_kind); /* pour faire taire "gcc -Wuninitialized" */
  base_hook = spf.outputG.lhs [spf.outputG.maxxprod+(Aij>0 ? Aij : -Aij)].prolon;

  if (spf.walk.pass_inherited && Aij > 0) {
    /* On fait les appels herites des Aij-prods sans autres appels intercales */
    hook = base_hook;
    spf.walk.cur_Aij = Aij;

    /* Aij est accessible */
    if (spf.walk.pre_pass) {
      /* Debut des appels top-down des Aij_prods */
      pi_kind = (*spf.walk.pre_pass) (Aij);

#if LLOG
      fputs ("The pre top_down visit of ", stdout);
      spf_print_Xpq (stdout, Aij);
      printf ("is %s\n", pi_kind > 0 ? "licensed" : (pi_kind == 0 ? "skipped" : "erased"));
#endif /* LLOG */

      if (pi_kind <= 0) {
	while ((Pij = spf.outputG.rhs [hook].lispro) != 0) {
	  if (Pij > 0
	      && ((spf.walk.user_filtering_function == 0) || (*spf.walk.user_filtering_function) (Pij))
	      /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	      ) {
	    spf.outputG.rhs [hook].prolis = pi_kind;
	  }

	  hook++;
	}
    
	hook = base_hook;
	result_kind = pi_kind;
      }
    }
    else
      pi_kind = 1;

    if (pi_kind > 0) {
      result_kind = -1;

      while ((spf.walk.cur_Pij = spf.outputG.rhs [hook].lispro) != 0) {
	/* On parcourt les productions dont la lhs est Aij */
	/* On appelle pass_inherited sur chaque prod, Ca permet eventuellement
	   de "repartir" les attributs herites de Aij sur chacune de ses Aij-prods */
	/* Si spf.walk.pass_inherited est NULL, ces appels "a vide" permettent d'armer les appels de pass_derived */
	if (spf.walk.cur_Pij > 0
	    && ((spf.walk.user_filtering_function == 0) || (*spf.walk.user_filtering_function) (spf.walk.cur_Pij))
	    /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	    ) {
	  /* Non elimine' (d'un coup precedent) */
	  /* cet Aij est "non elimine'" */
	  spf.walk.prev_Aij = spf.walk.prev_td_Aij;
	  pi_kind = spf.outputG.rhs [hook].prolis = (*spf.walk.pass_inherited) (spf.walk.cur_Pij); /* ATTENTION utilisation de prolis !! */

#if LLOG
	  spf_print_iprod (stdout, spf.walk.cur_Pij, "", "");
	  printf (" ... top_down visit completed%s\n", pi_kind > 0 ? "" : (pi_kind == 0 ? " (to be skipped)" : " (to be erased)"));
#endif /* LLOG */

	  spf.walk.prev_td_Aij = Aij;

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

	hook++;
      }
    }
    
    if (spf.walk.post_pass) {
      /* Debut des appels top-down des Aij_prods */
      pi_kind = (*spf.walk.post_pass) (Aij, (bool) (result_kind > 0));

#if LLOG
      fputs ("The post top_down visit of ", stdout);
      spf_print_Xpq (stdout, Aij);
      printf ("%s\n", pi_kind > 0 ? "succeeds" : (pi_kind == 0 ? "is strange!" : "fails"));
#endif /* LLOG */

      if (result_kind > 0 && pi_kind < 0) {
	/* On vire les Aij-prods qui restent */
	hook = base_hook;

	while ((Pij = spf.outputG.rhs [hook].lispro) != 0) {
	  if (Pij > 0
	      && ((spf.walk.user_filtering_function == 0) || (*spf.walk.user_filtering_function) (Pij))
	      /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	      ) {
	    spf.outputG.rhs [hook].prolis = -1; /* On prepare l'elimination ... */
	  }

	  hook++;
	}
    
	result_kind = -1;
      }
    }
  }
  else
    result_kind = 1;

  hook = base_hook;
  Pij_kept = false;

  while ((Pij = spf.outputG.rhs [hook].lispro) != 0) {
    /* On parcourt les productions dont la lhs est Aij */
    /* On appelle pass_inherited sur chaque prod, Ca permet eventuellement
       de "repartir" les attributs herites de Aij sur chacune de ses Aij-prods */
    /* Si spf.walk.pass_inherited est NULL, ces appels "a vide" permettent d'armer les appels de pass_derived */
    if (Pij > 0
	&& ((spf.walk.user_filtering_function == 0) || (*spf.walk.user_filtering_function) (Pij))
	/* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	) {
      /* Non elimine' (d'un coup precedent) */
      if (spf.walk.pass_inherited && Aij > 0) {
	pi_kind = spf.outputG.rhs [hook].prolis;

	if (pi_kind < 0) {
	  /* A eliminer */
	  /* Attention, ne peut pas etre fait ds la boucle spf.walk.pass_inherited */
	  spf.outputG.rhs [hook].lispro = -Pij; /* On elimine */
	  spf.outputG.is_proper = false;
	}
      }

      item = spf.outputG.lhs [Pij].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	/* On parcourt la RHS de la prod instanciee */
	if (Xpq > 0 /* nt ... */ && (Xpq <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Xpq)) /* ... productif */) {
	  if (spf.walk.pass_inherited) {
	    if (Aij > 0 && pi_kind > 0)
	      SXBA_1_bit (spf.walk.Aij_set, Xpq);

	    if (--spf.walk.Aij2nyp_nb [Xpq] == 0 /* ...calcule' */) {
	      must_be_kept = spf_walk (SXBA_bit_is_set (spf.walk.Aij_set, Xpq) /* a au moins un pere valide */ ? Xpq : -Xpq);

	      if (!must_be_kept) {
		/* Echec de l'evaluation sur Xpq */
		spf.outputG.rhs [hook].lispro = -Pij; /* On elimine la Pij courante */
		/* Ds ce cas il faut continuer sur les autres fils pour avoir un calcul correct ds spf.walk.Aij2nyp_nb */
#if LLOG	
		spf_print_iprod (stdout, Pij, "", " ... is dead-end erased\n");
#endif /* LLOG */
	      }
	    }
	  }
	  else {
	    /* passe synthetisees */
	    if (SXBA_bit_is_reset_set (spf.walk.Aij_set, Xpq)) {
	      /* ...non encore visite' */
	      SXBA_1_bit (spf.walk.cyclic_Aij_set, Xpq);
	      must_be_kept = spf_walk (Xpq);
	      SXBA_0_bit (spf.walk.cyclic_Aij_set, Xpq);

	      if (!must_be_kept) {
		/* Echec de l'evaluation sur Xpq */
		SXBA_1_bit (spf.walk.invalid_Aij_set, Xpq);
		spf.outputG.rhs [hook].lispro = -Pij; /* On elimine la Pij courante */
#if LLOG	
		spf_print_iprod (stdout, Pij, "", " ... is dead-end erased\n");
#endif /* LLOG */
		/* Ds ce cas (pas de passe heritee demandee) inutile de continuer sur les autres fils */
		break;
	      }
	    }
	    else {
	      /* deja visite' ... */
	      if (SXBA_bit_is_set (spf.walk.cyclic_Aij_set, Xpq)
		/* ... et cyclique
		   On est ds le cas 
		   Xpq =>* ... =>Pij \alpha Xpq \beta (on a ij == pq)
		   on va donc eliminer Pij mais Xpq est peut-etre quand meme valide par une autre P'ij */
		  || SXBA_bit_is_set (spf.walk.invalid_Aij_set, Xpq)
		/* ... et a echoue' */
		  ) {
		spf.outputG.is_proper = false; /* Pour le cas cycliqye */
		spf.outputG.rhs [hook].lispro = -Pij; /* On elimine la Pij courante */

#if LLOG	
		spf_print_iprod (stdout, Pij, "", SXBA_bit_is_set (spf.walk.cyclic_Aij_set, Xpq) ? " ... is cycle erased\n" : " ... is dead-end erased\n");
#endif /* LLOG */

		/* Ds ce cas (pas de passe heritee demandee) inutile de continuer sur les autres fils */
		break;
	      }
	    }
	  }
	}
      }

      if ((Pij = spf.outputG.rhs [hook].lispro) > 0
	  && ((spf.walk.user_filtering_function == 0) || (*spf.walk.user_filtering_function) (Pij))
	  /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	  ) {
	/* Pij est gardee */
	Pij_kept = true;
      }
    }

    hook++;
  }

  if (result_kind > 0 && !Pij_kept)
    /* L'evaluation de la sous-foret a echoue' */
    result_kind = -1;

  /* On lance les passes synthetisees sur les Aij-prods, sans intercalage!! */
  if (spf.walk.pass_derived && Aij > 0 && result_kind > 0) {
    hook = base_hook;
    spf.walk.cur_Aij = Aij;

    if (spf.walk.pre_pass) {
      /* Debut des appels bottom_up des Aij_prods */
      pd_kind = (*spf.walk.pre_pass) (Aij);

#if LLOG
      fputs ("The pre bottom_up visit of ", stdout);
      spf_print_Xpq (stdout, Aij);
      printf ("is %s\n", pd_kind > 0 ? "licensed" : (pd_kind == 0 ? "skipped" : "erased"));
#endif /* LLOG */

      if (pd_kind < 0) {
	while ((Pij = spf.outputG.rhs [hook].lispro) != 0) {
	  if (Pij > 0
	      && ((spf.walk.user_filtering_function == 0) || (*spf.walk.user_filtering_function) (Pij))
	      /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	      ) {
	    spf.outputG.rhs [hook].lispro = -Pij; /* On l'elimine ... */
	    spf.outputG.is_proper = false;
	  }

	  hook++;
	}
      }

      if (pd_kind > 0)
	hook = base_hook;
    }
    else
      pd_kind = 1;

    if (pd_kind == 1) {
      must_be_kept = false;

      while ((spf.walk.cur_Pij = spf.outputG.rhs [hook].lispro) != 0) {
	/* On parcourt les productions dont la lhs est Aij */

	/* La`, on est su^r que tous les sous-arbres de prod ont ete visites bottom-up */
	/* De plus toutes les Aij-productions peuvent se visiter en consecutivement */
	/* C'est la passe heritee qui a pu invalider Pij */
	if (spf.walk.cur_Pij > 0
	    && ((spf.walk.user_filtering_function == 0) || (*spf.walk.user_filtering_function) (spf.walk.cur_Pij))
	    /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	    ) {
	  spf.walk.prev_Aij = spf.walk.prev_bu_Aij;
	  pd_kind = (*spf.walk.pass_derived) (spf.walk.cur_Pij);

#if LLOG
	  spf_print_iprod (stdout, spf.walk.cur_Pij, "", "");
	  printf (" ... bottom-up visit completed%s\n", pd_kind > 0 ? "" : (pd_kind == 0 ? " (to be skipped)" : " (to be erased)"));
#endif /* LLOG */

	  spf.walk.prev_bu_Aij = Aij;

	  if (pd_kind < 0) {
	    spf.outputG.rhs [hook].lispro = -spf.walk.cur_Pij; /* On l'elimine ... */
	    spf.outputG.is_proper = false;
	  }
	  else {
	    /* Ce Pij n'est pas elimine' */
	    must_be_kept = true;
	  }
	}

	hook++;
      }
    }
    else
      must_be_kept = (pd_kind >= 0);
    
    if (spf.walk.post_pass) {
      /* Fin des appels bottom-up des Aij_prods */
      /* Faut-il appeler post_pass_derived si toutes les Aij-prods ont ete eliminees ? */
      pd_kind = (*spf.walk.post_pass) (Aij, must_be_kept);

#if LLOG
      fputs ("The post bottom_up visit of ", stdout);
      spf_print_Xpq (stdout, Aij);
      printf ("%s\n", pd_kind > 0 ? "succeeds" : (pd_kind == 0 ? "is strange!" : "fails"));
#endif /* LLOG */

      if (must_be_kept && pd_kind < 0) {
	/* On vire les Aij-prods qui restent */
	hook = base_hook;

	while ((Pij = spf.outputG.rhs [hook].lispro) != 0) {
	  if (Pij > 0
	      && ((spf.walk.user_filtering_function == 0) || (*spf.walk.user_filtering_function) (Pij))
	      /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	      ) {
	    spf.outputG.rhs [hook].lispro = -Pij; /* On l'elimine ... */
	    spf.outputG.is_proper = false;
	  }

	  hook++;
	}
    
	must_be_kept = false;
      }
    }
  }
  else
    must_be_kept = result_kind >= 0;

  return must_be_kept;
}


/* Retourne faux ssi toute la foret a disparue !! */
/* Wrappeur pour spf_topological_[top_down_|bottom_up_]walk */
static bool
spf_call_walk_core (SXINT root)
{
  SXINT     hook, item, Xpq, Aij, Pij;
  bool ret_val;

  if (spf.walk.invalid_Aij_set == NULL)
    spf.walk.invalid_Aij_set = sxba_calloc (spf.walk.Aij_top+1);
  else
    sxba_empty (spf.walk.invalid_Aij_set);

  if (spf.walk.pass_inherited) {
    /* Passe heritee demandee.  Les Aij-prods ne sont appelees que lorsque toutes les prods (de la sous-foret root)
       ayant un Aij en partie droite ont ete appelees */
    /* init ... */
    if (spf.walk.Aij2nyp_nb == NULL)
      spf.walk.Aij2nyp_nb = (SXINT*) sxcalloc ((SXUINT)spf.walk.Aij_top+1, sizeof (SXINT));
    /* else
       On suppose que les appels precedents ont laisses spf.walk.Aij2nyp_nb vide */

    if (root == spf.outputG.start_symbol) {
      for (Aij = 1; Aij <= spf.walk.Aij_top; Aij++) {
	spf.walk.Aij2nyp_nb [Aij] = spf.outputG.Aij2rhs_nb [Aij];
      }
    }
    else {
      /* cas de la visite d'une sous foret => calcul "dynamique" de spf.walk.Aij2nyp_nb */
      if (spf.walk.Aij_stack == NULL)
	spf.walk.Aij_stack = (SXINT*) sxalloc (spf.walk.Aij_top+1, sizeof (SXINT));

      spf.walk.Aij_stack [0] = 0; /* prudence sur les appels multiples */

      PUSH (spf.walk.Aij_stack, root);

      while (!IS_EMPTY (spf.walk.Aij_stack)) {
	Aij = POP (spf.walk.Aij_stack);
	hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;
    
	while ((Pij = spf.outputG.rhs [hook++].lispro) != 0) {
	  if (Pij > 0
	      && ((spf.walk.user_filtering_function == 0) || (*spf.walk.user_filtering_function) (Pij))
	      /* filtre supplementaire utilisateur sur les Pij sans modifier la foret */
	      ) {
	    /* Non supprime' */
	    item = spf.outputG.lhs [Pij].prolon;

	    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	      if (Xpq > 0 /* nt ... */ &&
		  (Xpq <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Xpq)) /* ... productif */) {
		if (spf.walk.Aij2nyp_nb [Xpq]++ == 0)
		  /* 1ere occur */
		  PUSH (spf.walk.Aij_stack, Xpq);
	      }
	    }
	  }
	}
      }
    }
  }

  if (spf.walk.Aij_set == NULL)
    spf.walk.Aij_set = sxba_calloc (spf.walk.Aij_top+1);
  else
    sxba_empty (spf.walk.Aij_set);

  if (spf.walk.cyclic_Aij_set == NULL)
    spf.walk.cyclic_Aij_set = sxba_calloc (spf.walk.Aij_top+1);

  spf.walk.prev_td_Aij = spf.walk.prev_bu_Aij = 0;

  SXBA_1_bit (spf.walk.Aij_set, root);
  SXBA_1_bit (spf.walk.cyclic_Aij_set, root);

  ret_val = spf_walk (root);

  SXBA_0_bit (spf.walk.cyclic_Aij_set, root);

#if EBUG
  /* à faire: vérif similaire si root != start_symbol */
  if (spf.walk.pass_inherited && root == spf.outputG.start_symbol) {
    for (Aij = 1; Aij <= spf.walk.Aij_top; Aij++) {
      if (spf.walk.Aij2nyp_nb [Aij] != 0
	  && (Aij <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Aij)) /* ... productif */) {
	sxtrap (ME, "spf_call_walk_core (inherited pass didn't visit all nodes the appropriate number of times)");
      }
    }
  }
#endif /* EBUG */

  return ret_val;
}

static bool
spf_call_walk (SXINT root)
{
  bool ret_val;

#if EBUG
  SXBA status;
  SXINT Aij, Pij;
  struct rhs *prhs;

  if (!spf.outputG.is_proper)
    sxtrap (ME, "spf_call_walk (can't call a forest walk on a non-proper forest)");

  status = sxba_calloc (spf.outputG.maxxprod+1);
  
  for (Aij = 1; Aij <= spf.walk.Aij_top; Aij++) {
    if (Aij <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Aij)) {
      /* ... productif */
      prhs = spf.outputG.rhs+spf.outputG.lhs[spf.outputG.maxxprod+Aij].prolon;
      
      while ((Pij = (prhs++)->lispro) != 0) {
	if (Pij > 0)
	  /* On marque les bonnes */
	  SXBA_1_bit (status, Pij);
      }
    }
  } 
#endif /* EBUG */

  ret_val = spf_call_walk_core (root);

#if EBUG
  if (spf.outputG.is_proper) {
    for (Aij = 1; Aij <= spf.walk.Aij_top; Aij++) {
      if (Aij <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Aij)) {
	/* ... productif */
	prhs = spf.outputG.rhs+spf.outputG.lhs[spf.outputG.maxxprod+Aij].prolon;
	
	while ((Pij = (prhs++)->lispro) != 0) {
	  if ((Pij > 0 && !SXBA_bit_is_set (status, Pij))
	      || (Pij < 0 && SXBA_bit_is_set (status, -Pij)))
	    sxtrap (ME, "spf_call_walk (forest walk changed validity status of some instanciated productions, but the forest has not been declared not-proper)");
	}
      }
    } 
  }
  
  sxfree (status);
#endif /* EBUG */

  if (!spf.outputG.is_proper)
    ret_val = spf_make_proper (root);

  return ret_val;
}


/*  ****************** Debut des fonctions externes  *************************  */

/* L'allocation des Aij est separee de l'allocation de la foret partagee
   elle-meme c,a permet (cas earley) de pre'-calculer les Aij avant
   de calculer les productions instanciees */
void
spf_allocate_Aij (SXINT sxmaxnt, SXINT sxeof)
{
  XxYxZ_alloc (&spf.outputG.Axixj2Aij_hd, "Axixj2Aij_hd",
	       n*sxmaxnt+1, 1, Axixj2Aij_hd_foreach,
	       Axixj2Aij_hd_oflw, NULL);

  /* Truc pour eviter que ds spf_print_iprod on ecrive des nt instancies du
     traitement d'erreurs */
  spf.outputG.maxnt = XxYxZ_size (spf.outputG.Axixj2Aij_hd)+1;

  XxYxZ_alloc (&spf.outputG.Txixj2Tij_hd, "Txixj2Tij_hd",
	       n*sxeof+1, 1, Txixj2Tij_hd_foreach,
	       Txixj2Tij_hd_oflw, NULL);
}

void
spf_reallocate_lhs_rhs (SXINT lhs_size, SXINT rhs_size)
{
  if (lhs_size > outputG_lhs_size) {
    outputG_lhs_size = lhs_size;
    spf.outputG.lhs = (struct lhs*) sxrealloc (spf.outputG.lhs, outputG_lhs_size+2, sizeof (struct lhs));
  }

  if (rhs_size > outputG_rhs_size) {
    outputG_rhs_size = rhs_size;
    spf.outputG.rhs = (struct rhs*) sxrealloc (spf.outputG.rhs, outputG_rhs_size+2, sizeof (struct rhs));
  }
}

void
spf_allocate_spf (SXINT sxmaxprod)
{
  sxprepare_for_possible_reset (spf.outputG);
  sxprepare_for_possible_reset (spf.walk);
  sxprepare_for_possible_reset (spf.tree_count);

  outputG_lhs_size = n*sxmaxprod; /* Pij_nb_guessing () */
  outputG_rhs_size = outputG_lhs_size*(spf.inputG.maxrhs+1);

  /* "+1+1" Pour que la regle bidon tienne si outputG_lhs_size et outputG_rhs_size sont nuls */
  spf.outputG.lhs = (struct lhs*) sxalloc (outputG_lhs_size+1+1, sizeof (struct lhs));
  spf.outputG.rhs = (struct rhs*) sxalloc (outputG_rhs_size+1+1, sizeof (struct rhs));

  /* on ajoute une regle bidon de numero 0 dont la lhs est 0 et la rhs contient spf.outputG.start_symbol
     a` l'item 0 (et 0 a` l'item 1) */
  spf.outputG.rhs [0].lispro = 0; /* spf.outputG.start_symbol quand il sera connu */
  spf.outputG.rhs [0].prolis = 0;
  spf.outputG.rhs [1].lispro = 0;
  spf.outputG.rhs [1].prolis = 0;
  spf.outputG.lhs [0].prolon = 0;
  spf.outputG.lhs [0].lhs = 0;
  spf.outputG.lhs [0].init_prod = 0;
  spf.outputG.lhs [1].prolon = spf.outputG.maxitem = 2;
  spf.outputG.maxprod = 0;

  /* Pour le calcul des "hook" */
  Aij2lhs_nb = (SXINT*) sxcalloc ((SXUINT)XxYxZ_size (spf.outputG.Axixj2Aij_hd)+1, sizeof (SXINT));
  spf.outputG.Aij2rhs_nb = (SXINT*) sxcalloc ((SXUINT)XxYxZ_size (spf.outputG.Axixj2Aij_hd)+1, sizeof (SXINT));

  spf.outputG.maxt = 0;
  spf.outputG.is_proper = true;
}

void
spf_allocate_insideG (struct insideG *insideG_ptr, bool has_lex2, bool has_set_automaton)
{
  sxprepare_ptr_for_possible_reset (insideG_ptr, sizeof (struct insideG));
  insideG_ptr->is_allocated = true;

  insideG_ptr->lispro = (SXINT*) sxalloc (insideG_ptr->maxitem+1, sizeof (SXINT));
  insideG_ptr->prolis = (SXINT*) sxalloc (insideG_ptr->maxitem+1, sizeof (SXINT));

  insideG_ptr->prolon = (SXINT*) sxalloc (insideG_ptr->maxprod+2, sizeof (SXINT));
  insideG_ptr->lhs = (SXINT*) sxalloc (insideG_ptr->maxprod+1, sizeof (SXINT));

  /* faut-il renommer les t et nt ?? */
  insideG_ptr->t2init_t = (SXINT*) sxalloc (-insideG_ptr->maxt+1, sizeof (SXINT)), insideG_ptr->t2init_t [0] = 0;
  insideG_ptr->nt2init_nt = (SXINT*) sxalloc (insideG_ptr->maxnt+1, sizeof (SXINT)), insideG_ptr->nt2init_nt [0] = 0;
  insideG_ptr->prod2init_prod = (SXINT*) sxalloc (insideG_ptr->maxprod+1, sizeof (SXINT)), insideG_ptr->prod2init_prod [0] = 0;

  /* Le super-axiome 0 est defini par la super-production 0 */
  insideG_ptr->npg = (SXINT*) sxalloc (insideG_ptr->maxnt+2, sizeof (SXINT)), insideG_ptr->npg [0] = 0, insideG_ptr->npg [1] = 1;/* Pas d'occur du "super axiome" en rhs *//* Debut de la tranche de l'axiome */
  insideG_ptr->numpg = (SXINT*) sxalloc (insideG_ptr->maxprod+2, sizeof (SXINT)), insideG_ptr->numpg [0] = insideG_ptr->numpg [insideG_ptr->maxprod+1] = 0;
  insideG_ptr->npd = (SXINT*) sxalloc (insideG_ptr->maxnt+2, sizeof (SXINT)), insideG_ptr->npd [0] = 0, insideG_ptr->npd [1] = 1;/* Pas d'occur du "super axiome" en rhs *//* Debut de la tranche de l'axiome */
  insideG_ptr->numpd = (SXINT*) sxalloc (insideG_ptr->sizeofnumpd, sizeof (SXINT)), insideG_ptr->numpd [0] = 0;
  insideG_ptr->tpd = (SXINT*) sxalloc (-insideG_ptr->maxt+2, sizeof (SXINT)), insideG_ptr->tpd [0] = insideG_ptr->tpd [1] = 1; /* Init + Assure qu'il n'y a pas d'occur du terminal 0 en rhs */
  insideG_ptr->tnumpd = (SXINT*) sxalloc (insideG_ptr->sizeoftnumpd, sizeof (SXINT)), insideG_ptr->tnumpd [0] = 0;

  insideG_ptr->rhs_nt2where = (SXINT*) sxcalloc ((SXUINT)insideG_ptr->maxnt+1, sizeof (SXINT));
  insideG_ptr->lhs_nt2where = (SXINT*) sxcalloc ((SXUINT)insideG_ptr->maxnt+1, sizeof (SXINT));

  if (insideG_ptr->is_eps) {
    insideG_ptr->bvide = sxba_calloc (insideG_ptr->maxnt+1);
    insideG_ptr->empty_prod_item_set = sxba_calloc (insideG_ptr->maxitem+1);
  }

  insideG_ptr->titem_set = sxba_calloc (insideG_ptr->maxitem+1);

  if (has_set_automaton) {
    insideG_ptr->rc_titem_set = sxba_calloc (insideG_ptr->maxitem+1);
  }

  if (has_lex2 || has_set_automaton) {
    /* On remplit insideG_ptr->t2prod_item_set */
    insideG_ptr->t2prod_item_set = sxbm_calloc (-insideG_ptr->maxt+1, insideG_ptr->maxitem+1);
    sxba_range (insideG_ptr->t2prod_item_set [-insideG_ptr->maxt], 0, 4); /* Occur de eof ds la super_prod */
  }
}

void
spf_free_insideG (struct insideG *insideG_ptr)
{
  if (insideG_ptr->is_allocated) {
    sxfree (insideG_ptr->lispro), insideG_ptr->lispro = NULL;
    sxfree (insideG_ptr->prolis), insideG_ptr->prolis = NULL;

    sxfree (insideG_ptr->prolon), insideG_ptr->prolon = NULL;
    sxfree (insideG_ptr->lhs), insideG_ptr->lhs = NULL;

    sxfree (insideG_ptr->t2init_t), insideG_ptr->t2init_t = NULL;
    sxfree (insideG_ptr->nt2init_nt), insideG_ptr->nt2init_nt = NULL;
    sxfree (insideG_ptr->prod2init_prod), insideG_ptr->prod2init_prod = NULL;

    sxfree (insideG_ptr->npg), insideG_ptr->npg = NULL;
    sxfree (insideG_ptr->numpg), insideG_ptr->numpg = NULL;
    sxfree (insideG_ptr->npd), insideG_ptr->npd = NULL;
    sxfree (insideG_ptr->numpd), insideG_ptr->numpd = NULL;
    sxfree (insideG_ptr->tpd), insideG_ptr->tpd = NULL;
    sxfree (insideG_ptr->tnumpd), insideG_ptr->tnumpd = NULL;

    sxfree (insideG_ptr->rhs_nt2where), insideG_ptr->rhs_nt2where = NULL;
    sxfree (insideG_ptr->lhs_nt2where), insideG_ptr->lhs_nt2where = NULL;

    if (insideG_ptr->is_eps) {
      sxfree (insideG_ptr->bvide), insideG_ptr->bvide = NULL;
      sxfree (insideG_ptr->empty_prod_item_set), insideG_ptr->empty_prod_item_set = NULL;
    }

    sxfree (insideG_ptr->titem_set), insideG_ptr->titem_set = NULL;

    if (insideG_ptr->rc_titem_set) {
      sxfree (insideG_ptr->rc_titem_set), insideG_ptr->rc_titem_set = NULL;
    }

    if (insideG_ptr->t2prod_item_set) sxfree (insideG_ptr->t2prod_item_set), insideG_ptr->t2prod_item_set = NULL;

    if (insideG_ptr->left_corner) {
      /* Appel de LC_construction () */
      sxbm_free (insideG_ptr->left_corner), insideG_ptr->left_corner = NULL;
      sxbm_free (insideG_ptr->nt2item_set), insideG_ptr->nt2item_set = NULL;
      if (insideG_ptr->left_recursive_set) sxfree (insideG_ptr->left_recursive_set), insideG_ptr->left_recursive_set = NULL;
    }

    if (insideG_ptr->right_corner) {
      /* Appel de RC_construction () */
      sxbm_free (insideG_ptr->right_corner), insideG_ptr->right_corner = NULL;
      sxbm_free (insideG_ptr->nt2rc_item_set), insideG_ptr->nt2rc_item_set = NULL;
      if (insideG_ptr->right_recursive_set) sxfree (insideG_ptr->right_recursive_set), insideG_ptr->right_recursive_set = NULL;
    }

    if (insideG_ptr->nt2min_gen_lgth) sxfree (insideG_ptr->nt2min_gen_lgth), insideG_ptr->nt2min_gen_lgth = NULL;
    if (insideG_ptr->t2item_set) sxbm_free (insideG_ptr->t2item_set), insideG_ptr->t2item_set = NULL;

    insideG_ptr->is_allocated = false;
  }
}

void
spf_finalize (void)
{
  SXINT              Aij, Pij, size;
  struct lhs       *plhs;

  /* On laisse les parseurs gerer directement
     spf.outputG.start_symbol
     spf.outputG.maxnt
     spf.outputG.maxxnt
     spf.outputG.maxxprod
     ... et tout ce qui a trait au traitement des erreurs
  */
  size = spf.outputG.maxxprod+spf.outputG.maxxnt;

  if (size + 2 < outputG_lhs_size) {
    outputG_lhs_size = size;
    spf.outputG.lhs = (struct lhs*) sxrealloc (spf.outputG.lhs, size+2, sizeof (struct lhs));
  }

  size = spf.outputG.maxitem+spf.outputG.maxxprod+spf.outputG.maxxnt;

  if (size + 2 < outputG_rhs_size) {
    outputG_rhs_size = size;
    spf.outputG.rhs = (struct rhs*) sxrealloc (spf.outputG.rhs, size+2, sizeof (struct rhs));
  }
      
  spf.outputG.maxt = -XxYxZ_top (spf.outputG.Txixj2Tij_hd); /* ca contient les xt eventuels !! */
  spf.outputG.rhs [0].lispro = spf.outputG.start_symbol;

  /* Remplissage des "hooks" qui permettent pour un nt donne' A
     de trouver toutes les A-prods qui le definissent */
  plhs = spf.outputG.lhs+spf.outputG.maxxprod+1; /* plhs->prolon == spf.outputG.maxitem */

  for (Aij = 1; Aij <= spf.outputG.maxxnt; Aij++) {
    plhs->prolon += Aij2lhs_nb [Aij];
    plhs [1].prolon = plhs->prolon+1; /* pour le coup d'apres */
    spf.outputG.rhs [plhs->prolon].lispro = 0;
    spf.outputG.rhs [plhs->prolon].prolis = 0; /* inutilise pour l'instant */
    plhs->lhs = Aij;
    plhs->init_prod = 0; /* inutilise pour l'instant */
    plhs++;
  }

  for (Pij = 1; Pij <= spf.outputG.maxxprod; Pij++) {
    Aij = spf.outputG.lhs [Pij].lhs;
    plhs = spf.outputG.lhs+spf.outputG.maxxprod+Aij;
    plhs->prolon--;
    spf.outputG.rhs [plhs->prolon].lispro = Pij;
    spf.outputG.rhs [plhs->prolon].prolis = 0; /* inutilise pour l'instant */
  }

  /* Aij_top sert ds les topological pour savoir si on regarde ou non un xnt */
  /* Doit-on le mettre ds outputG ou ds walk ? */
  if (spf.outputG.maxxprod > spf.outputG.maxprod)
    spf.walk.Aij_top = spf.outputG.maxxnt;
  else
    spf.walk.Aij_top = spf.outputG.maxnt;

  sxfree (Aij2lhs_nb), Aij2lhs_nb = NULL;

  if (wvstr)
    varstr_free (wvstr), wvstr = NULL;
}

void
spf_free (void)
{
  if (XxYxZ_is_allocated (spf.outputG.Axixj2Aij_hd)) {
    /* Cas is_parse_forest */
    XxYxZ_free (&spf.outputG.Axixj2Aij_hd);
    XxYxZ_free (&spf.outputG.Txixj2Tij_hd);
    if (spf.outputG.Tpq_rcvr_set) sxfree (spf.outputG.Tpq_rcvr_set), spf.outputG.Tpq_rcvr_set = NULL;
    if (spf.outputG.Pij2eval) sxfree (spf.outputG.Pij2eval), spf.outputG.Pij2eval = NULL;
    if (spf.outputG.non_productive_Aij_rcvr_set) sxfree (spf.outputG.non_productive_Aij_rcvr_set), spf.outputG.non_productive_Aij_rcvr_set = NULL;
    if (spf.outputG.xPij2lgth) sxfree (spf.outputG.xPij2lgth), spf.outputG.xPij2lgth = NULL;
  }

  if (spf.outputG.Tij2tok_no_stack) {
    sxfree (spf.outputG.Tij2tok_no_stack), spf.outputG.Tij2tok_no_stack = NULL;
    sxfree (spf.outputG.tok_no2some_Tij), spf.outputG.tok_no2some_Tij = NULL;
    DFREE_STACK (spf.outputG.tok_no_stack);
    sxfree (spf.outputG.Tij2comment), spf.outputG.Tij2comment = NULL;
  }

#if 0
  if (XH_is_allocated (spf.outputG.Tij2XH_tok_no))
    XH_free (&spf.outputG.Tij2XH_tok_no);

  if (spf.outputG.Tij2tok_no) 
    sxfree (spf.outputG.Tij2tok_no), spf.outputG.Tij2tok_no = NULL;  

  if (spf.outputG.tok_no2some_Tij) 
    sxfree (spf.outputG.tok_no2some_Tij), spf.outputG.tok_no2some_Tij = NULL;  

  if (spf.outputG.Tij2comment) sxfree (spf.outputG.Tij2comment), spf.outputG.Tij2comment = NULL;
#endif /* 0 */

  if (spf.outputG.lhs) {
    /* Cas is_parse_forest */
    sxfree (spf.outputG.lhs), spf.outputG.lhs = NULL;
    sxfree (spf.outputG.rhs), spf.outputG.rhs = NULL;
    sxfree (spf.outputG.Aij2rhs_nb), spf.outputG.Aij2rhs_nb = NULL;
  }

  if (spf.tree_count.prod2nb) {
    sxfree (spf.tree_count.prod2nb), spf.tree_count.prod2nb = NULL;
    sxfree (spf.tree_count.nt2nb), spf.tree_count.nt2nb = NULL;
  }

  if (spf.walk.Aij_stack) sxfree (spf.walk.Aij_stack), spf.walk.Aij_stack = NULL;
  if (spf.walk.Aij2nyp_nb) sxfree (spf.walk.Aij2nyp_nb), spf.walk.Aij2nyp_nb = NULL;
  if (spf.walk.invalid_Aij_set) sxfree (spf.walk.invalid_Aij_set), spf.walk.invalid_Aij_set = NULL;
  if (spf.walk.Aij_set) sxfree (spf.walk.Aij_set), spf.walk.Aij_set = NULL;
  if (spf.walk.cyclic_Aij_set) sxfree (spf.walk.cyclic_Aij_set), spf.walk.cyclic_Aij_set = NULL;
  if (spf.walk.status) sxfree (spf.walk.status), spf.walk.status = NULL;

  spf.outputG.start_symbol = 0;
}

/* stack contient en son sommet :
   | prod | top
   | Xik  |
   | ...  |
   | Xpq  |
   | ...  |
   | Xrj  |
   | Aij  |
   | ...  |
   | top  | 0
   |______|
*/
void
spf_put_an_iprod (SXINT *stack)
{
  SXINT              prod, lgth, Aij, Xpq;
  SXINT              *top_stack, *bot_stack;
  struct lhs       *plhs;
  struct rhs       *prhs;

  top_stack = stack+TOP (stack);
  prod = *top_stack;
  lgth = spf.inputG.prolon [prod+1]-spf.inputG.prolon [prod];
  bot_stack = top_stack-lgth;

  /* Attention Aij n'est pas forcement en stack [1] */
  Aij = *bot_stack++;
  Aij2lhs_nb [Aij]++;

  if (++spf.outputG.maxprod >= outputG_lhs_size) {
    spf.outputG.lhs = (struct lhs*) sxrealloc (spf.outputG.lhs,
					       (outputG_lhs_size *= 2)+1+1,
					       sizeof (struct lhs));
  }

  while (spf.outputG.maxitem+lgth > outputG_rhs_size) {
    spf.outputG.rhs = (struct rhs*) sxrealloc (spf.outputG.rhs,
					       (outputG_rhs_size *= 2)+1+1,
					       sizeof (struct rhs));
  }

  plhs = spf.outputG.lhs+spf.outputG.maxprod;

  plhs->init_prod = prod;
  plhs->lhs = Aij;
  /* plhs->prolon = spf.outputG.maxitem; deja fait... */

  prhs = spf.outputG.rhs + spf.outputG.maxitem;

  while (bot_stack < top_stack) {
    Xpq = *--top_stack;
    
    if (Xpq > 0)
      spf.outputG.Aij2rhs_nb [Xpq]++;

    prhs->lispro = Xpq;
    prhs->prolis = spf.outputG.maxprod;
    spf.outputG.maxitem++;
    prhs++;
  }

  prhs->lispro = 0; /* fin de la rhs */
  prhs->prolis = spf.outputG.maxprod;
  spf.outputG.maxitem++;

  plhs [1].prolon = spf.outputG.maxitem;
}

#if 0
extern VARSTR   concat_ff_tstr (VARSTR vstr, char* ff, int Tpq);
extern struct sxtoken *tok_no2tok (SXINT tokno);
extern SXINT      get_SEMLEX_lahead (void);
extern void     local_fill_Tij2tok_no (void);
#endif /* 0 */

extern VARSTR  rcvr_out_range    (VARSTR vstr, SXINT lb, SXINT ub); /* Ds earley */
extern SXINT   special_Apq_name  (char *string, SXINT lgth); /* Ds earley */
extern SXINT   special_Tpq_name  (char *string, SXINT lgth); /* Ds earley */
extern SXINT   get_SEMLEX_lahead (void); /* Ds dag_scanner, retourne 0 si pas de SEMLEX */


static bool
spf_is_a_suffix (char *string, SXINT string_lgth, char *suffix, SXINT suffix_lgth)
{
  /* On regarde si string se termine par "__" || suffix */
  //  SXINT  suffix_lgth;
  char *top;

  // suffix_lgth = strlen (suffix);

  if (string_lgth <= suffix_lgth+2)
    /* le prefixe doit etre non vide */
    return false;

  top = suffix + suffix_lgth;
  string += string_lgth;

  while (--top >= suffix) {
    if (*top != *--string)
      return false;
  }

  return string [-1] == '_' && string [-2] == '_';
}

VARSTR
spf_concat_ff_tstr (VARSTR local_vstr, char *ff, SXINT Tpq)
{
  SXINT lgth, which_case;
  char* tstr;


  if (spf.outputG.Tpq_rcvr_set == NULL || !SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tpq)) {
    /* ce test est à relâcher, ou à conditionner par une option, si on veut dans le DAG sorti les
       _error__(terminal) insérés par le rcvr */
    tstr = spf.inputG.tstring [-Tij2T (Tpq)];
    lgth = strlen (tstr);

    which_case = special_Tpq_name (tstr, lgth);

    if (which_case == 0)
      local_vstr = varstr_catenate_with_escape (local_vstr, ff, "%()[]{|\\");
    else if (which_case == 1) {
      /* terminal special */
      SXINT tstr_len_minus_1 = lgth - 1;

      if (!spf_is_a_suffix (ff, strlen (ff), tstr, strlen (tstr))) {
	/* le cas repair est deja fait */
	local_vstr = varstr_catenate_with_escape (local_vstr, ff, "%()[]{|\\");
      }
      else {
	char *ff_suf, *ff_suf2;

	if ((ff_suf = strstr (ff, "__"))) {
	  /* On prend le + a droite pour permettre des "le__det__det" */
	  /* Attention, on peut avoir anti-___nc */
	  while ((ff_suf2 = strstr (ff_suf+1, "__"))) {
	    ff_suf = ff_suf2;
	  }
	}
	local_vstr = varstr_lcatenate_with_escape (local_vstr, ff, ff_suf - ff,"%()[]{|\\");	
      }
      /* on sort la categorie */
      local_vstr = varstr_catchar (local_vstr, '_');
      local_vstr = varstr_catchar (local_vstr, '_');
      local_vstr = varstr_lcatenate (local_vstr, tstr, tstr_len_minus_1);
    }
    else if (which_case == -1) {
      if (!spf_is_a_suffix (ff, strlen (ff), tstr, strlen (tstr))) {
	/* le cas repair est deja fait */	
	local_vstr = varstr_catenate_with_escape (local_vstr, ff, "%()[]{|\\");
	/* ... et la categorie */
	local_vstr = varstr_catchar (local_vstr, '_');
	local_vstr = varstr_catchar (local_vstr, '_');
	local_vstr = varstr_catenate (local_vstr, tstr);
      }
    }
#if EBUG
    else
      sxtrap (ME, "spf_concat_ff_tstr (which_case returned something else than -1, 0 or 1)");
#endif /* EBUG */
  }
  return local_vstr;
}




static VARSTR
spf_tok_no2varstr_catenate (VARSTR local_vstr, SXINT tok_no, SXINT Tij)
{
  struct sxtoken *ptoken;
  char           *comment = NULL, *ff;
  SXINT semlex_ste = 0;

  ptoken = &(SXGET_TOKEN (tok_no));
  
  if (spf.outputG.Tij2comment)
    /* prio aux composites */
    comment = spf.outputG.Tij2comment [Tij];
  
  if (comment == NULL)
    comment = ptoken->comment;
  
  if (comment) {
    local_vstr = varstr_catenate (local_vstr, comment);
    local_vstr = varstr_catchar (local_vstr, ' ');
    local_vstr = varstr_complete (local_vstr);
  }

  ff = sxstrget (ptoken->string_table_entry);
  
  local_vstr = spf_concat_ff_tstr (local_vstr, ff, Tij);	/* Ca permet eventuellement de suffixer ff par des "__cat" ou autre */
  
  if (tok_no < idag.toks_buf_top) {
    /* pas eof ni tok_no de la rcvr */
    /* Traitement des %SEMLEX ; devrait ne jamais être utilisé dans le cas is_dag, en raison du fait
       que SEMLEX_lahead, récupéré par get_SEMLEX_lahead (), est alors toujours nul */

    if (idag.dag_kind != SDAG_KIND)
      semlex_ste = idag.tok_no2semlex_ste [tok_no];
    else {
      ptoken = &(SXGET_TOKEN (tok_no+1));
      if (ptoken->lahead == get_SEMLEX_lahead ())
	semlex_ste = ptoken->string_table_entry;
    }

    if (semlex_ste) {
      /* Oui */ 
      local_vstr = varstr_catchar (local_vstr, ' ');
      
      local_vstr = varstr_catchar (local_vstr, '['); 
      local_vstr = varstr_catchar (local_vstr, '|'); 
      local_vstr = varstr_catenate (local_vstr, sxstrget (semlex_ste));
      local_vstr = varstr_catchar (local_vstr, '|');
      local_vstr = varstr_catchar (local_vstr, ']');
      local_vstr = varstr_complete (local_vstr);
    }
  }

  return local_vstr;
}

VARSTR
spf_Tpq2varstr_catenate (VARSTR local_vstr, SXINT Tij)
{
  SXINT tok_no, nb;
  SXINT *tok_no_stack, *top_tok_no_stack;

  tok_no_stack = spf_get_Tij2tok_no_stack (Tij);

  nb = *tok_no_stack;
  tok_no = tok_no_stack [1];

  if (nb == 1) {
    local_vstr = spf_tok_no2varstr_catenate (local_vstr, tok_no, Tij);
  } 
  else {
    top_tok_no_stack = tok_no_stack + nb;
    local_vstr = varstr_catenate (local_vstr, "(");

    while (++tok_no_stack <= top_tok_no_stack) {
      tok_no = *tok_no_stack;

      local_vstr = spf_tok_no2varstr_catenate (local_vstr, tok_no, Tij);
      local_vstr = varstr_catenate (local_vstr, (tok_no_stack < top_tok_no_stack) ? " | " : ")");
    }
  }

  return local_vstr;
}

void
spf_print_Xpq (FILE *out_file, SXINT Xpq)
{
  SXINT  X, p, q;
  char *t;

  if (Xpq < 0) {
    Xpq = -Xpq;

    X = -Tij2T (Xpq);
    p = Tij2i (Xpq);
    q = Tij2j (Xpq);

    t = spf.inputG.tstring [X];

    if (wvstr)
      varstr_raz (wvstr);

    if (spf.outputG.Tij2tok_no_stack == NULL) {
      /* Non (encore) affecte' On prend t tout seul !! */
    }
    else {
      if (spf.outputG.Tpq_rcvr_set && SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Xpq)) {
	/* Terminal en erreur.  On pourrait prendre spf_Tpq2varstr_catenate ? */
      }
      else {
	/* ste = parser_Tpq2tok (Xpq)->string_table_entry; */
	if (wvstr == NULL) {
	  sxprepare_for_possible_reset (wvstr);
	  wvstr = varstr_alloc (32);
	}

	wvstr = spf_Tpq2varstr_catenate (wvstr, Xpq);
      }
    }
    
    if (wvstr == NULL || varstr_length (wvstr) == 0)
      fprintf (out_file, "\"%s\"[", t);
    else
      fprintf (out_file, "\"%s/%s\"[", varstr_tostr (wvstr), t);

    if (p <= 0) {
      if (p == 0)
	fputs ("*", out_file);
      else
	fprintf (out_file, "%ld", (SXINT) -p);
    }
    else
      fprintf (out_file, "%ld", (SXINT) p);

    if (spf.outputG.Tpq_rcvr_set && SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Xpq))
      fputs ("::", out_file);
    else
      fputs ("..", out_file);

    if (q <= 0) {
      if (q == 0)
	fputs ("*] ", out_file);
      else
	fprintf (out_file, "%ld] ", (SXINT) -q);
    }
    else
      fprintf (out_file, "%ld] ", (SXINT) q);
  }
  else {
    X = Aij2A (Xpq);
    p = Aij2i (Xpq);
    q = Aij2j (Xpq);

    fprintf (out_file, "<%s[", spf.inputG.ntstring [X]);

    if (p <= 0) {
      if (p == 0)
	fputs ("*", out_file);
      else
	fprintf (out_file, "%ld", (SXINT) -p);
    }
    else
      fprintf (out_file, "%ld", (SXINT) p);

    if (Xpq > spf.outputG.maxnt)
      fputs ("::", out_file);
    else
      fputs ("..", out_file);

    if (q <= 0) {
      if (q == 0)
	fputs ("*]> ", out_file);
      else
	fprintf (out_file, "%ld]> ", (SXINT) -q);
    }
    else
      fprintf (out_file, "%ld]> ", (SXINT) q);
  }
}



bool
spf_print_iprod (FILE *out_file, SXINT prod, char *prefix, char *suffix)
{
  SXINT Aij, Xpq, cur_item, i, j, p, q;

  sxinitialise(j); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(i); /* pour faire taire "gcc -Wuninitialized" */
  Aij = spf.outputG.lhs [prod].lhs;

  if (spf.outputG.is_error_detected) {
    i = Aij2i (Aij);
    j = Aij2j (Aij);
  }

  fprintf (out_file, "%s%ld/%ld:", prefix, (SXINT) prod, (SXINT) spf.outputG.lhs [prod].init_prod);
  spf_print_Xpq (out_file, Aij);
  fputs ("\t= ", out_file);

  cur_item = spf.outputG.lhs [prod].prolon;

  while ((Xpq = spf.outputG.rhs [cur_item++].lispro) != 0) {
    if (spf.outputG.is_error_detected) {
      if (Xpq < 0) {
	p = Tij2i (-Xpq);
	q = Tij2j (-Xpq);
      }
      else {
	p = Aij2i (Xpq);
	q = Aij2j (Xpq);
      }

      if (p < 0)
	p = -p;

      if (i != p && p > 0)
	fprintf (out_file, "[%ld::%ld] ", (SXINT) i, (SXINT) p);

      if (q < 0)
	q = -q;

      if (q > 0)
	i = q;
    }

    spf_print_Xpq (out_file, Xpq);
  }

  if (spf.outputG.is_error_detected && i != j)
    fprintf (out_file, "[%ld::%ld] ", (SXINT) i, (SXINT) j);

  fprintf (out_file, ";%s", suffix);
    
  return true; /* Ca peut etre utilise comme de la semantique */
}

/* Extrait de spf le p ieme sous-arbre enracine' en Aij
   (et appelle la fonction utilisateur f) */
SXINT
spf_dag2tree (SXUINT p,
	      SXINT Aij,
	      SXINT (*f)(SXINT))
{
  SXUINT     Xpq_tree_nb;
  SXINT      item, prod, Xpq, bot_hook, left_hook, right_hook, mid_hook;

  /* Recherche dichotomique de la Aij_prod selectionnee par p */
  bot_hook = left_hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;
  right_hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij+1].prolon-2;

  while ((mid_hook = (left_hook+right_hook)/2) < right_hook) {
    /* Il reste au moins 2 arbres */
    /* left_hook <= mid_hook < right_hook */
    if (p >= spf.tree_count.prod2nb [spf.outputG.rhs [mid_hook].lispro]) {
      /* Le bon arbre se trouve dans [mid_hook+1..right_hook] */
      left_hook = mid_hook+1;
    }
    else {
      /* Le bon arbre se trouve dans [left_hook..mid_hook] */
      right_hook = mid_hook;
    }
  }

  /* ici left_hook==mid_hook==right_hook */
  if (mid_hook > bot_hook)
    p -= spf.tree_count.prod2nb [spf.outputG.rhs [mid_hook-1].lispro];

  prod = spf.outputG.rhs [mid_hook].lispro;
    
  item = spf.outputG.lhs [prod].prolon;

  /* On decompose p sur les fils non-terminaux de prod */
  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    /* On parcourt la RHS de la prod instanciee */
    if (Xpq > 0 /* nt ... */ && (Xpq <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Xpq)) /* ... productif */) {
      /* nt */
      if (spf.tree_count.nt2nb [Xpq] > (double) (SXUINT) (-1) /* i.e., ULONG_MAX (or ULLONG_MAX on win64) */)
	sxtrap (ME, "spf_dag2tree (this function can not be called on a forest containing more than ULONG_MAX trees)");
      Xpq_tree_nb = (SXUINT)spf.tree_count.nt2nb [Xpq];

      if (!spf_dag2tree (p%Xpq_tree_nb, Xpq, f))
	return false;

      p /= Xpq_tree_nb;
    }
  }

  /* On vient de parcourir le "bon" sous-arbre de prod */
  return (*f) (prod); /* Appel de la semantique, elle peut ranger ses infos en // avec les Aij... */
}


/* Imprime sur out_file la [sous-]foret de racine Aij a partir de Aij vers les feuilles */
void
spf_print_td_forest (FILE *out_file, SXINT Aij)
{
  spf_print_forest_out_file = out_file;
  spf_topological_walk (Aij, spf_print_td_or_bu_prod, NULL);
}

/* Imprime sur out_file la [sous-]foret de racine Aij des feuilles vers Aij */
void
spf_print_bu_forest (FILE *out_file, SXINT Aij)
{
  spf_print_forest_out_file = out_file;
  spf_topological_walk (Aij, NULL, spf_print_td_or_bu_prod);
}

SXINT
spf_unfold (void)
{
  double            double_tree_count;
  SXUINT tree_count, tree_nb;

  if (spf_tree_count (&double_tree_count)) {
    tree_count = (SXUINT) double_tree_count;

    for (tree_nb = 0; tree_nb < tree_count; tree_nb++) {
      printf ("\nParse tree #%lu\n", tree_nb);
      spf_dag2tree (tree_nb, spf.outputG.start_symbol, call_spf_print_iprod);
    }

    return 1;
  }
      
  fputs ("\nThe parse forest is empty.\n", stdout);

  return 0;
}


/* On "compte" les elements de la grammaire apres du filtrage */
double
spf_print_signature (FILE *out_file, char *prefix_str)
{
  double            tree_count;

  count_maxprod = count_maxitem = 0;

  count_Tij_set = sxba_calloc (-spf.outputG.maxt+1);
  count_Aij_set = sxba_calloc (spf.outputG.maxxnt+1);

#if LOG
  fputs ("\nspf_print_signature: pi = count_td_walk\n", stdout);
#endif /* LOG */

  spf_tree_count (&tree_count);

  fprintf (out_file, "%s(|N|=%li, |T|=%li, |P|=%ld, S=%ld, |outpuG|=%ld, TreeCount=", 
	  prefix_str, (SXINT) sxba_cardinal (count_Aij_set), (SXINT) sxba_cardinal (count_Tij_set), (SXINT) count_maxprod, (SXINT) spf.outputG.start_symbol, (SXINT) count_maxitem);

  if (tree_count < 1.0E9)
    fprintf (out_file, "%.f)\n", tree_count);
  else
    fprintf (out_file, "%e)\n", tree_count);

  sxfree (count_Tij_set), count_Tij_set = NULL;
  sxfree (count_Aij_set), count_Aij_set = NULL;

  return tree_count;
}


extern SXINT get_repair_Tpq2tok_no (SXINT Tpq); /* Ds earley ou assimile' */

/* A cause de la rcvr eventuelle, des bouts du source ont pu etre supprime's entre p et q.
   Leurs commentaires (ou autre) ont ete rassembles ds vstr (par exemple par rcvr_out_range)
   On les ajoute aux commentaires des Tij2comment (spf_fill_Tij2tok_no a deja ete appele') et non
   individuellement sur les comment des tok_no qui partent de q, sauf si q est l'etat final (la trans sur eof n'est pas sortie().
   Auquel cas on on les ajoute aux commentaires des Tij2comment qui arrivent a p (le 21/02/08) */
static void
spf_complete_Tpq2comment_body (VARSTR vstr, SXINT Tpq)
{
  SXINT lgth, ste;
  char  *comment;

  sxinitialise (lgth);
  comment = spf.outputG.Tij2comment [Tpq];

  if (comment) {
    varstr_pop (vstr, 1); /* On enleve le "}" final */
    vstr = varstr_catchar (vstr, ' ');
    lgth = strlen (comment);
    vstr = varstr_lcatenate (vstr, comment+1, lgth-1); /* On enleve le "{" initial du suffixe */
  }

  ste = sxstr2save (varstr_tostr (vstr), varstr_length (vstr));
  spf.outputG.Tij2comment [Tpq] = sxstrget (ste); /* ne pas remplacer ste par sa definition (bug/feature de gcc qui optimise, avec risque d'erreur si réallocation de sxstrmngr */

  if (comment) {
    /* On remet en etat */
    varstr_pop (vstr, lgth+1);
    vstr = varstr_catchar (vstr, '}');
    (void) varstr_complete (vstr);
  }
}

static void
spf_complete_Tpq2comment (VARSTR vstr, SXINT p, SXINT q)
{
  SXINT Tpq;

  if (q == idag.final_state) {
    /* Damned, on n'a pas de Tij_jforeach !! */
    /* On va etre grossier (cas rare) */
    for (Tpq = -spf.outputG.maxt; Tpq > 0; Tpq--) {
      if (Tij2j (Tpq) == p) {
	/* Tous les Tpq qui arrivent en p */
	spf_complete_Tpq2comment_body (vstr, Tpq);
      }
    }
  }
  else {
    Tij_iforeach (q, Tpq) {
      /* Tous les Tpq qui partent de q */
      spf_complete_Tpq2comment_body (vstr, Tpq);
    }
  }
}


/* Appele' depuis la semantique si elle a besoin de cette correspondance */
/* Doit etre appele' (si besoin est) apres distribute_comment qui positionne dag2comment */
void
spf_fill_Tij2tok_no ()
{

  SXINT   Tpq, maxTpq, nb, tok_no, size, ste;
  SXINT   *tok_no_stack, *top_tok_no_stack;
  bool is_first_time;
  char    *comment, *new_comment;
  VARSTR  vstr;

  if (spf.outputG.Tij2tok_no_stack == NULL) {
#if LOG
    fputs ("\n**** spf_fill_Tij2tok_no () ****\n", stdout);
#endif /* LOG */

    maxTpq = -spf.outputG.maxt;
    spf.outputG.Tij2tok_no_stack = (SXINT *) sxcalloc (maxTpq + 1, sizeof (SXINT));
    spf.outputG.tok_no2some_Tij = (SXINT *) sxcalloc (/* get_toks_buf_size () */idag.repair_toks_buf_top + 1, sizeof (SXINT));
    size = idag.repair_toks_buf_top + TOP (idag.tok_no_stack) - spf.outputG.maxt;
    DALLOC_STACK (spf.outputG.tok_no_stack, size+1);
    spf.outputG.Tij2comment = (char **) sxcalloc (maxTpq+1, sizeof (char *));
    vstr = varstr_alloc (64);

    for (Tpq = 1; Tpq <= maxTpq; Tpq++) {
      DPUSH (spf.outputG.tok_no_stack, 0);
      nb = spf.outputG.Tij2tok_no_stack [Tpq] = DTOP (spf.outputG.tok_no_stack);

      if (spf.outputG.Tpq_rcvr_set == NULL || !SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tpq)) {
	/* T est un terminal */
	idag_p_t_q2tok_no_stack (&(spf.outputG.tok_no_stack), Tij2i (Tpq) /* p */, -Tij2T (Tpq) /* t */, Tij2j (Tpq)); /* Ds dag_scanner */
	tok_no_stack = spf.outputG.tok_no_stack+nb;

	spf.outputG.tok_no_stack [nb] = DTOP (spf.outputG.tok_no_stack) - nb; /* MAJ du compte local */
#if EBUG
	if (*tok_no_stack == 0)
	  sxtrap (ME, "spf_fill_Tij2tok_no (no token found for a given Tpq)");
#endif /* EBUG */	

	tok_no = *++tok_no_stack;
	comment = SXGET_TOKEN (tok_no).comment;

	if (nb > 1) {
	  top_tok_no_stack = spf.outputG.tok_no_stack + DTOP (spf.outputG.tok_no_stack);
	  varstr_raz (vstr);

	  if (comment) {
	    vstr = varstr_catenate (vstr, comment);
	    is_first_time = false;
	  }
	  else
	    is_first_time = true;

	  /* On s'occupe des commentaires */
	  while (++tok_no_stack <= top_tok_no_stack) {
	    tok_no = *tok_no_stack;
	    new_comment = SXGET_TOKEN (tok_no).comment;

	    if (new_comment && strcmp (comment, new_comment) != 0) {
	      if (is_first_time) {
		is_first_time = false;
		vstr = varstr_catenate (vstr, new_comment);
	      }
	      else {
		varstr_pop (vstr, 1); /* On enleve le "}" final */
		vstr = varstr_catchar (vstr, ' ');
		vstr = varstr_lcatenate (vstr, new_comment+1, strlen (new_comment)-1); /* On enleve le "{" initial du suffixe */
	      }
	    }
	  }
	  
	  if ((nb = varstr_length (vstr)) > 0) {
	    ste = sxstr2save (varstr_tostr (vstr), nb);
	    comment = sxstrget (ste); /* ne pas remplacer ste par sa definition (bug/feature de gcc qui optimise, avec risque d'erreur si réallocation de sxstrmngr */
	  }
	  else
	    comment = NULL;
	}
      }
      else {
	/* Cas d'un terminal en erreur */
	tok_no = get_repair_Tpq2tok_no (Tpq);
	DPUSH (spf.outputG.tok_no_stack, tok_no);
	spf.outputG.tok_no_stack [nb] = 1;
	comment = SXGET_TOKEN (tok_no).comment;
      }

      spf.outputG.Tij2comment [Tpq] = comment;
      spf.outputG.tok_no2some_Tij [tok_no] = Tpq;

#if LOG
      printf ("Tij=%ld, ", (SXINT) Tpq);
      spf_print_Xpq (stdout, -Tpq);
#endif /* LOG */
    }

    varstr_free (vstr);

#if LOG
    fputs ("**** End of spf_fill_Tij2tok_no () ****\n\n", stdout);
#endif /* LOG */
  }
}

/* Retourne une "stack" qui contient la liste des tok_no correspondant a Tij */
SXINT *
spf_get_Tij2tok_no_stack (SXINT Tij)
{
  if (spf.outputG.Tij2tok_no_stack == NULL)
    spf_fill_Tij2tok_no ();

  return spf.outputG.tok_no_stack + spf.outputG.Tij2tok_no_stack [Tij];
}


#if 0
/* Ds dag_scanner.c si is_dag ou ds sxearley_main.c sinon */
extern void fill_Tij2tok_no (SXINT Tpq, SXINT maxTpq);
/* Appele' depuis la semantique si elle a besoin de cette correspondance */
/* Doit etre appele' (si besoin est) apres distribute_comment qui positionne dag2comment */
void
spf_fill_Tij2tok_no (void)
{
  SXINT  Tpq, maxt;

  maxt = -spf.outputG.maxt;

  /* En cas d'appel precedent depuis get_[s]dag_sentence_str (), on recommence tout car il y'a pu
     y avoir un appel a distribute_comment entre temps */
  if (XH_is_allocated (spf.outputG.Tij2XH_tok_no))
    XH_free (&spf.outputG.Tij2XH_tok_no);

  if (spf.outputG.Tij2tok_no)
    sxfree (spf.outputG.Tij2tok_no), spf.outputG.Tij2tok_no = NULL;

  spf.outputG.Tij2tok_no = sxcalloc (maxt + 1, sizeof (SXINT));

  if (spf.outputG.tok_no2some_Tij)
    sxfree (spf.outputG.tok_no2some_Tij), spf.outputG.tok_no2some_Tij = NULL;

  spf.outputG.tok_no2some_Tij = sxcalloc (get_toks_buf_size() + 1, sizeof (SXINT));

#if LOG
  fputs ("\n**** spf_fill_Tij2tok_no () ****\n", stdout);
#endif /* LOG */

  for (Tpq = 1; Tpq <= maxt; Tpq++) {
    if (spf.outputG.Tpq_rcvr_set == NULL || !SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tpq)) {
      /* T est un terminal */
      fill_Tij2tok_no (Tpq, maxt);
    }

#if LOG
    printf ("Tij=%ld, ", (SXINT) Tpq);
    spf_print_Xpq (stdout, -Tpq);
#endif /* LOG */
  }

#if LOG
  fputs ("**** End of spf_fill_Tij2tok_no () ****\n\n", stdout);
#endif /* LOG */
}
#endif /* 0 */


#if 0
/* Ca permet d'eviter que la compil de l'utilisateur de'pende de is_dag */
struct sxtoken*
spf_Tpq2tok (Tpq)
     SXINT Tpq;
{
  return parser_Tpq2tok (Tpq);
}
#endif /* 0 */


/* appele depuis nbest, lfg_semact ... */
/* Si Pij = X1 ... Xp et
   si p   = Y1 ... Yp est la production de la CFG squelette et
   si Z1 ... Zk, Zj = Yi, peut deriver ds le vide soit
      T1 ... Tk, Tj = Xi la chaine associee construite sur la rhs de Pij qui ne contient que des nt
      instancies dont la base peut deriver ds le vide.  A cette chaine on associe le nombre binaire
      eval = E1 ... Ek, tq Ej=1 <=> Xi ne derive pas dans le vide */
void
spf_fill_Pij2eval (void)
{
  SXINT Pij, item, val, Xpq, X, p, q;

  if (spf.outputG.Pij2eval) return; /* deja rempli ... */

  /* alloc */
  spf.outputG.Pij2eval = (SXINT*) sxalloc (spf.outputG.maxxprod+1, sizeof (SXINT));
  spf.outputG.Pij2eval [0] = 0;

  for (Pij = 1; Pij <= spf.outputG.maxxprod; Pij++) {
    item = spf.outputG.lhs [Pij].prolon; 
    val = 0;

    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      if (Xpq > 0) {
	X = Aij2A (Xpq);

	if (SXBA_bit_is_set (spf.inputG.BVIDE, X)) {
	  /* derive ds le vide */
	  val <<= 1;

	  p = Aij2i (Xpq);
	  q = Aij2j (Xpq);

	  if (p != q) {
	    /* ne derive pas ds le vide (un peu grossier ds le cas correction!!) */
	    val += 1;
	  }
	}
      }
    }

    spf.outputG.Pij2eval [Pij] = val;
  }
}



bool
spf_tree_count (double *count)
{
  SXINT i;

  /* A partir du 13/01/06 Les cycles eventuels ds la foret ont ete elimines */
  if (spf.outputG.start_symbol == 0 /* || spf.inputG.has_cycles */) {
    *count = 0;
    return false;
  }

  if (spf.tree_count.prod2nb == NULL) {
    spf.tree_count.prod2nb = (double *) sxcalloc ((SXUINT)spf.outputG.maxxprod+1, sizeof (double));
    spf.tree_count.nt2nb = (double *) sxcalloc ((SXUINT)spf.walk.Aij_top+1, sizeof (double));
  }
  else {
    for (i = spf.outputG.maxxprod; i >= 0; i--)
      spf.tree_count.prod2nb [i] = 0;

    for (i = spf.walk.Aij_top; i >= 0; i--)
      spf.tree_count.nt2nb [i] = 0;
  }

  spf_topological_walk (spf.outputG.start_symbol, NULL, spf_tree_count_pass_derived);

  *count = spf.tree_count.nt2nb [spf.outputG.start_symbol];

  return true;
}

#if 0
/* calcule spf.walk.invalid_Aij_set */
/* Un Aij n'est pas invalide s'il existe dans la sous-foret Aij au moins un arbre qui ne contient que des Apq valides. */
static bool
make_proper_first_td_walk (SXINT Aij)
{
  SXINT     Xpq, hook, Pij, item, bot_item;
  bool ret_val, keep_Pij;

  if (SXBA_bit_is_reset_set (spf.walk.Aij_set, Aij)) {
    /* C'est la 1ere fois qu'on appelle make_proper_first_td_walk sur Aij */
    ret_val = false;
    hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;
    
    while ((Pij = spf.outputG.rhs [hook++].lispro) != 0) {
      /* On parcourt les productions dont la lhs est Aij */
      if (Pij > 0) { /* et qui ne sont pas filtrées */
	bot_item = spf.outputG.lhs [Pij].prolon;

	keep_Pij = true;
	item = bot_item;
	
	while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	  /* On parcourt la RHS de la  prod instanciee Pij */
	  if (Xpq > 0 && Xpq <= spf.outputG.maxxnt) { /* y compris ceux en erreur */
	    /* nt */
	    if (!make_proper_first_td_walk (Xpq)) {
	      /* Pij sera invalide'e ds make_proper_second_td_walk */
	      keep_Pij = false;
	      /* Il est inutile d'aller examiner le reste de la rhs ... */
	      break;
	    }
	  }
	}

	if (keep_Pij)
	  ret_val = true; /* Un Pij est garde' */
      }
    }

    if (ret_val)
      /* ... non invalide */
      SXBA_0_bit (spf.walk.invalid_Aij_set, Aij);

    return ret_val;
  }

  return !SXBA_bit_is_set (spf.walk.invalid_Aij_set, Aij);
}
#endif /* 0 */

/* calcule spf.outputG.Aij2rhs_nb */
static bool
make_proper_second_td_walk (SXINT Aij)
{
  SXINT     hook, Pij, item, bot_item, Xpq;
  bool retval = false;

  /* on n'est appelés qu'une seule fois par Xpq grâce au contrôle sur spf.outputG.Aij2rhs_nb */
  hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;
    
  while ((Pij = spf.outputG.rhs [hook/* ++ en fin de boucle */].lispro) != 0) {
    /* On parcourt les productions dont la lhs est Aij */
    if (Pij > 0) { /* et qui ne sont pas filtrées */
      item = bot_item = spf.outputG.lhs [Pij].prolon;
	
      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	if (Xpq > 0 && SXBA_bit_is_set (spf.walk.invalid_Aij_set, Xpq))
	  break;
      }

      if (Xpq) {
	/* Il est invalide, on le vire */
	spf.outputG.rhs [hook].lispro = -Pij;
      }
      else { /* aucun des symboles de rhs n'est invalide: on le garde */
	retval = true; /* au moins une Pij de Aij a donc une rhs valide: on a au moins un sous-arbre */
	item = bot_item;
	  
	while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	  if (Xpq > spf.outputG.start_symbol && spf.outputG.Aij2rhs_nb [Xpq]++ == 0)
	    make_proper_second_td_walk (Xpq);
	}
      }
    }
    hook++;
  }
  return retval; /* valeur utilisée seulement à l'extérieur, càd sur l'axiome */
}

#if EBUG
static void
spf_check_consistency (void)
{
  SXINT Pij;

#if LOG
  fputs ("*** Entering spf_check_consistency\n", stdout);
#endif /* LOG */

  /* On verifie la coherence de la nelle foret */
  spf_make_proper_check_td_set = sxba_calloc (spf.outputG.maxxprod+1);
  spf_make_proper_check_bu_set = sxba_calloc (spf.outputG.maxxprod+1);

  spf_topological_walk (spf.outputG.start_symbol, spf_make_proper_check_td, NULL);
  spf_topological_walk (spf.outputG.start_symbol, NULL, spf_make_proper_check_bu);

  /* On verifie la coherence de la nelle foret */
  if ((Pij = sxba_first_difference (spf_make_proper_check_td_set, spf_make_proper_check_bu_set)) != -1)
    sxtrap (ME, "spf_check_consistency (inconsistent forest)");

  sxfree (spf_make_proper_check_td_set), spf_make_proper_check_td_set = NULL;
  sxfree (spf_make_proper_check_bu_set), spf_make_proper_check_bu_set = NULL;

#if LOG
  fputs ("*** Leaving spf_check_consistency\n", stdout);
#endif /* LOG */
}
#endif /* EBUG */

/* Nelle version du 03/07/07 */
/* PB : Les parties de la foret non visitees a cause de user_filtering_function ne doivent pas etre supprimees !! (voir la solution) */
/* Elle peut etre appelee par l'utilisateur */
/* Elle "desactive" toutes les productions inaccessibles (grammaire pas propre et/ou qq Pij mis en negatif)
   et recalcule Aij2rhs_nb pour les passes heritees eventuelles */
static SXINT
make_proper_keep_valid_Aijs (SXINT Aij, bool must_be_kept)
{
  if (must_be_kept)
    SXBA_0_bit (spf.walk.invalid_Aij_set, Aij);

  return must_be_kept ? 1 : -1;
}

bool
spf_make_proper (SXINT root_Aij)
{
  SXINT        Xpq, hook, Pij;
  bool    ret_val;

  sxuse(root_Aij); /* pour faire taire gcc -Wunused */
#if LOG
  fputs ("*** Entering spf_make_proper ()\n", stdout);
#endif /* LOG */

  if (spf.walk.Aij_set == NULL)
    spf.walk.Aij_set = sxba_calloc (spf.walk.Aij_top+1);
  else
    sxba_empty (spf.walk.Aij_set);

  if (spf.walk.invalid_Aij_set == NULL)
    spf.walk.invalid_Aij_set = sxba_calloc (spf.walk.Aij_top+1);
  sxba_fill (spf.walk.invalid_Aij_set);

#if 0
  if ((ret_val = make_proper_first_td_walk (spf.outputG.start_symbol /* On se moque de root_Aij !! */)))
    SXBA_0_bit (spf.walk.invalid_Aij_set, spf.outputG.start_symbol);
#endif /* 0 */ /* remplacé par ce qui suit */
  spf.walk.pass_inherited = NULL;
  spf.walk.pass_derived = sxivoid_int;
  spf.walk.pre_pass = NULL;
  spf.walk.post_pass = make_proper_keep_valid_Aijs;
  if ((ret_val = spf_call_walk_core (spf.outputG.start_symbol)))
    SXBA_0_bit (spf.walk.invalid_Aij_set, spf.outputG.start_symbol);
  
  
  for (Xpq = 1; Xpq <= spf.walk.Aij_top; Xpq++)
    spf.outputG.Aij2rhs_nb [Xpq] = 0;
  
  /* Attention : Soit A -> B C ; telle que make_proper_first_td_walk a trouve' que
     B est non invalide et que C est invalide.  Donc, si c'est la seule A_prod, A est aussi trouve' invalide.
     make_proper_second_td_walk (A) dit que A -> B C ; est invalidee (et ne descend pas ds ses fils).  Si la seule occur de B
     est ds A -> B C ; alors B reste non invalide alors que les B_prods doivent etre supprimees (avec eventuellement les descendants).
     On ne peut donc pas utiliser spf.walk.invalid_Aij_set pour connaitre exactement les Aij invalides.
     On va utiliser spf.outputG.Aij2rhs_nb */
  for (Xpq = (make_proper_second_td_walk (spf.outputG.start_symbol) ? 2 : 1); /* make_proper_second_td_walk
									       rend true ss'il reste
									       au moins 1 arbre:
									       dans ce cas on
									       n'efface pas l'axiome
									       ; sinon on l'efface,
									       comme tous les
									       autres, puisque son
									       Aij2rhs_nb est
									       évidemment nul */
       Xpq <= spf.walk.Aij_top;
       Xpq++) {
    if (spf.outputG.Aij2rhs_nb [Xpq] == 0) {
      hook = spf.outputG.lhs [spf.outputG.maxxprod+Xpq].prolon;    

      while ((Pij = spf.outputG.rhs [hook /* ++ en fin de boucle */].lispro) != 0) {
	/* On parcourt les productions dont la lhs est Aij */
	if (Pij > 0) /* et qui ne sont pas encore filtrées */
	  spf.outputG.rhs [hook].lispro = -Pij; /* on les filtre car elles ont une lhs invalide */

	hook++;
      }
    }
  }

  spf.outputG.is_proper = true;

#if LOG
  fputs ("*** Leaving spf_make_proper()\n", stdout);
#endif /* LOG */

#if EBUG
  spf_check_consistency ();
#endif /* EBUG */

#if LOG
  if (!ret_val)
    fputs ("*** WARNING: the whole forest has vanished!! ***\n", stdout);
#endif /* LOG */

  return ret_val;
}


SXBA
spf_save_status (SXBA save_set)
{
  sxba_copy (save_set, spf.walk.status);

  return save_set;
}

void
spf_restore_status (SXBA save_set)
{
  sxba_copy (spf.walk.status, save_set);
}



/* On se souvient de l'etat courant de la spf.  Cet etat pourra etre restaure' par spf_pop_status () */
void
spf_push_status (void)
{
  SXINT        Aij, Pij;
  struct rhs *prhs;

  if (spf.walk.status == NULL) {
    spf.walk.status = sxba_calloc (spf.outputG.maxxprod+1);
  }
  else
    sxba_empty (spf.walk.status);
      
  for (Aij = 1; Aij <= spf.walk.Aij_top; Aij++) {
    if (Aij <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Aij)) {
      /* ... productif */
      prhs = spf.outputG.rhs+spf.outputG.lhs[spf.outputG.maxxprod+Aij].prolon;

      while ((Pij = (prhs++)->lispro) != 0) {
	if (Pij > 0)
	  /* On marque les bonnes */
	  SXBA_1_bit (spf.walk.status, Pij);
      }
    }
  }
}


/* On suppose que l'etat precedent et que l'etat courant sont coherents :
   toutes les prod inaccessibles sont marquees en negatif et Aij2rhs_nb contient les bonnes valeurs */
/* De plus on suppose que l'etat courant est une sous-foret de l'etat precedent (des Pij ont ete vires) */
void
spf_pop_status (void)
{
  SXINT        Aij, Pij, item, Xpq;
  struct rhs *prhs;
      
  for (Aij = 1; Aij <= spf.walk.Aij_top; Aij++) {
    if (Aij <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Aij)) {
      prhs = spf.outputG.rhs+spf.outputG.lhs[spf.outputG.maxxprod+Aij].prolon;

      while ((Pij = prhs->lispro) != 0) {
	if (Pij < 0) {
	  /* inactive ds le courant ... */
	  Pij = -Pij;
	
	  if (SXBA_bit_is_set (spf.walk.status, Pij)) {
	    /* ... et active ds l'etat precedent */
	    /* On la passe en positif */
	    prhs->lispro = Pij;

	    /* On met a jour Aij2rhs_nb */
	    item = spf.outputG.lhs [Pij].prolon;

	    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	      /* On parcourt la RHS de la prod instanciee */
	      if (Xpq > 0 && (Xpq <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Xpq)) /* ... productif */) {
		/* Xpq est accessible par un chemin suppl;ementaire */
		spf.outputG.Aij2rhs_nb [Xpq]++;
	      }
	    }
	  }
	}

	prhs++;
      }
    }
  }

#if EBUG
  spf_check_consistency ();
#endif /* EBUG */
}



bool
spf_topological_walk (SXINT root,
		      SXINT (*pi)(SXINT),
		      SXINT (*pd)(SXINT))
{
  bool ret_val;

#if EBUG
  if (pi && pd)
    /* On ne peut pas simultanement parcourir une foret top-down et bottom-up avec les contraintes afferentes 
     (on peut sortir d'un sous-arbre sans avoir visite' ses fils bottom-up !!) */
    sxtrap (ME, "spf_topological_walk");
#endif /* EBUG */

#if LOG
  fputs ("*** Entering spf_topological_walk (", stdout);
  spf_print_Xpq (stdout, root);
  printf (", %s, %s)\n", pi == NULL ? "NULL" : "pi", pd == NULL ? "NULL" : "pd");
#endif /* LOG */

  spf.walk.pass_inherited = pi;
  spf.walk.pass_derived = pd;
  spf.walk.pre_pass = NULL;
  spf.walk.post_pass = NULL;

  ret_val = spf_call_walk (root);

#if LOG
  fputs ("*** Leaving spf_topological_walk (", stdout);
  spf_print_Xpq (stdout, root);
  printf (", %s, %s)\n", pi == NULL ? "NULL" : "pi", pd == NULL ? "NULL" : "pd");
#endif /* LOG */

  return ret_val;
}



bool
spf_topological_top_down_walk (SXINT root,
			       SXINT (*pi)(SXINT),
			       SXINT (*pre_pass)(SXINT),
			       SXINT (*post_pass)(SXINT, bool))
{
  bool ret_val;

#if LOG
  fputs ("*** Entering spf_topological_top_down_walk (", stdout);
  spf_print_Xpq (stdout, root);
  printf (", %s, %s, %s)\n", pi == NULL ? "NULL" : "pi", pre_pass == NULL ? "NULL" : "pre_pass", post_pass == NULL ? "NULL" : "post_pass");
#endif /* LOG */

  spf.walk.pass_inherited = pi;
  spf.walk.pass_derived = NULL;
  spf.walk.pre_pass = pre_pass;
  spf.walk.post_pass = post_pass;

  ret_val = spf_call_walk (root);

#if LOG
  fputs ("*** Leaving spf_topological_top_down_walk (", stdout);
  spf_print_Xpq (stdout, root);
  printf (", %s, %s, %s)\n", pi == NULL ? "NULL" : "pi", pre_pass == NULL ? "NULL" : "pre_pass", post_pass == NULL ? "NULL" : "post_pass");
#endif /* LOG */

  return ret_val;
}

bool
spf_topological_bottom_up_walk (SXINT root,
				SXINT (*pd)(SXINT),
				SXINT (*pre_pass)(SXINT),
				SXINT (*post_pass)(SXINT, bool))
{
  bool ret_val;

#if LOG
  fputs ("*** Entering spf_topological_bottom_up_walk (", stdout);
  spf_print_Xpq (stdout, root);
  printf (", %s, %s, %s)\n", pd == NULL ? "NULL" : "pd", pre_pass == NULL ? "NULL" : "pre_pass", post_pass == NULL ? "NULL" : "post_pass");
#endif /* LOG */

  spf.walk.pass_inherited = NULL;
  spf.walk.pass_derived = pd;
  spf.walk.pre_pass = pre_pass;
  spf.walk.post_pass = post_pass;

  ret_val = spf_call_walk (root);

#if LOG
  fputs ("*** Leaving spf_topological_bottom_up_walk (", stdout);
  spf_print_Xpq (stdout, root);
  printf (", %s, %s, %s)\n", pd == NULL ? "NULL" : "pd", pre_pass == NULL ? "NULL" : "pre_pass", post_pass == NULL ? "NULL" : "post_pass");
#endif /* LOG */

  return ret_val;
}


/* attention Pij_set est vide au retour */
bool
spf_erase (SXBA Pij_set)
{
  SXINT     Pij, prod, Aij, hook;
  bool ret_val;

#if LOG
  fputs ("*** Entering spf_erase ***\n", stdout);
#endif /* LOG */

  Pij = 0;

  while ((Pij = sxba_scan (Pij_set, Pij)) > 0) {
    Aij = spf.outputG.lhs [Pij].lhs;
    hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;

    while ((prod = spf.outputG.rhs [hook].lispro) != 0) {
      if (prod > 0 && SXBA_bit_is_set_reset (Pij_set, prod)) {
	/* finesse */
	spf.outputG.is_proper = false;
	spf.outputG.rhs [hook].lispro = -prod;
      }

      hook++;
    }
  }

  if (!spf.outputG.is_proper)
    ret_val = spf_make_proper (spf.outputG.start_symbol /* global */);
  else
    ret_val = true;

#if LOG
  fputs ("*** Leaving spf_erase ***\n", stdout);
#endif /* LOG */

  return ret_val;
}


SXINT
spf_print_tree_count (void)
{
  double val;

  /* Le 14/09/06 j'utilise spf_print_signature car la spf a pu etre
     reduite (cycle, erreurs, ...) */
  val = spf_print_signature (sxstdout, "Shared Parse Forest = ");
  (void) val; /* pour faire taire gcc -Wall */

  if (is_print_time)
    sxtime (TIME_FINAL, "\tPrint Tree Count");

  return 1;
}




/* Nouvelle version de la transformation de la frontiere terminale d'une foret partagee en fsa */
/* On va faire qqchose d'analogue a ce qui se fait ds read_a_re pour fabriquer un dfa puis appeler dfa_minimize 
   et sortir un UDAG ou appeler fsa_to_re suivant le cas */

#include "sxword.h"
#include "fsa.h"

#define SUBDAG_THRESHOLD 12
#define SUBDAG_THRESHOLD_2 100

bool UDAG_wanted;

static SXINT           yield_eof_Tpq, maxxnt, yield_eof_ste, *Aij2rhs_occ_nb, *Aij2visited_rhs_occ_nb;
static SXBA            special_Aij_set, yield_Tpq_set, yield_valid_Aij_set;
static sxword_header   transition_names;
static SXINT *Tpq2ste;
static XxYxZ_header    yield_fsa_XxYxZ_hd;
static struct subDAG_struct {
  SXINT init_state, final_state, minp, maxp, eps_trans_nb;
} *Aij2subDAG, empty_subDAG_struct;

#if 0
static SXINT           *Aij2subDAG_init_state, *Aij2subDAG_final_state;
#endif /* 0 */

#if LOG
static SXBA subDAG_p_is_visited;

static void
rec_print_subDAG (SXINT p, SXINT r)
{
  SXINT trans, ste, q;

  if (SXBA_bit_is_reset_set (subDAG_p_is_visited, p)) {
    XxYxZ_Xforeach (yield_fsa_XxYxZ_hd, p, trans) {
      q = XxYxZ_Z (yield_fsa_XxYxZ_hd, trans);
      ste = XxYxZ_Y (yield_fsa_XxYxZ_hd, trans);

      fprintf (stderr, "%ld --%s--> %ld\n", p, SXWORD_get (transition_names, ste), q);
    }  
    XxYxZ_Xforeach (yield_fsa_XxYxZ_hd, p, trans) {
      q = XxYxZ_Z (yield_fsa_XxYxZ_hd, trans);

      if (q != r)
	rec_print_subDAG (q, r);

    }  
  }
}

static void
print_subDAG (struct subDAG_struct *subDAG)
{
  subDAG_p_is_visited = sxba_calloc (subDAG->maxp + 1);

  fprintf (stderr, "===== subDAG (%ld, %ld) [min=%ld, max=%ld] <#eps=%ld> =====\n",
	   subDAG->init_state, subDAG->final_state,
	   subDAG->minp, subDAG->maxp,
	   subDAG->eps_trans_nb);
  rec_print_subDAG (subDAG->init_state, subDAG->final_state);
  fprintf (stderr, "============================================================\n");

  sxfree (subDAG_p_is_visited), subDAG_p_is_visited = NULL;
}
#endif /* LOG */


/* Tpq est un identifiant des symboles terminaux ds la foret
   ste est l'identifiant d'une transition dans le fsa
   Typiquement Tpq = {comment} (t, p, q) et ste = "{comment} t"
   Des Tpq differents peuvent donc avoir meme ste
*/
static void
spf_fill_Tpq2ste (SXINT Tpq /* un vrai terminal ou un NT special au format tnt */)
{
  SXINT   Aij, ntstr_len_minus_1, i, j, p, which_case, lgth, tok_no;
  char    *comment, *ntstr;

  varstr_raz (wvstr);

  if (Tpq <= -spf.outputG.maxt) {
    /* Vrai Tpq, fait-on qqchose s'il son nom a une forme particuliere ? */
    wvstr = spf_Tpq2varstr_catenate (wvstr, Tpq);
  }
  else {
    /* Tpq est un non-terminal special */
    Aij = Tpq+spf.outputG.maxt;
    ntstr = spf.inputG.ntstring [Aij2A (Aij)];
    lgth = strlen (ntstr);
    ntstr_len_minus_1 = lgth - 1;
    i = Aij2i (Aij);
    j = Aij2j (Aij);
    which_case = special_Apq_name (ntstr, lgth);

    if ((which_case & (4+2))) {/* balise (ouvrante et/ou fermante) */

      if ((which_case & 4)) { /* balise fermante: on récupère un commentaire d'un arc arrivant au noeud i */
	/* On suppose que l'appelant de out_sentence2re a appele' fill_idag_q2p_stack () */
	p = range2p (idag.q2pq_stack [idag.q2pq_stack_hd [i]+1]);
      }
      else { /* balise ouvrante: on récupère un commentaire d'un arc partant du noeud j */
	p = j;
      }
	    
      tok_no = idag.pq2tok_no [idag.p2pq_hd [p]];

      if (tok_no < 0)
	tok_no = idag.tok_no_stack [-tok_no+1];

      comment = SXGET_TOKEN (tok_no).comment;

      if (comment) {
	wvstr = varstr_catenate (wvstr, comment);
	wvstr = varstr_catchar (wvstr, ' ');
      }

      wvstr = varstr_lcatenate (wvstr, "_MARKUP", 7);
	    
      if (!spf_is_a_suffix (varstr_tostr (wvstr), varstr_length (wvstr), ntstr, ntstr_len_minus_1)) {
	wvstr = varstr_catchar (wvstr, '_');
	wvstr = varstr_catchar (wvstr, '_');
	wvstr = varstr_lcatenate (wvstr, ntstr, ntstr_len_minus_1);
      }
    }
    else { /* entité nommée: on va concaténer les commentaires du source spanné par Aij et sortir {ce commentaire} ntstr  (au '!' final près) */
      wvstr = sub_dag_to_comment (wvstr, i, j);

      if (varstr_length (wvstr)) {
	wvstr = varstr_catchar (wvstr, ' ');
      }

#if EBUG
      if ((which_case & 1) == 0)
	sxtrap (ME, "(trying to output named_entity from an NT whose name doesn't seem to end with '!')");
#endif /* EBUG */

      wvstr = varstr_lcatenate_with_escape (wvstr, ntstr, ntstr_len_minus_1, "%()[]{|\\");
    }
  }

  Tpq2ste [Tpq] = sxword_2save (&transition_names, varstr_tostr (wvstr), varstr_length (wvstr));

}


/* 1ere passe bottom-up sur la foret pour extraire le dag des feuilles de ses arbres */
/* Calcule pour chaque Aij le nombre d'occurrences en rhs et remplit Tpq2ste pour chaque terminal ou non-terminal spécial */
static SXINT
spf_first_yield_td_walk (SXINT Pij)
{
  SXINT             item, Xpq, Tpq, Aij, special_Aij, lgth;
  SXINT             ub, p, q, j;
  char              *ntstring;
  bool           is_OK;

  sxinitialise (ub);
  Aij = spf.walk.cur_Aij;

  ntstring = spf.inputG.ntstring [Aij2A (Aij)];
  lgth = strlen (ntstring);  

  SXBA_1_bit (yield_valid_Aij_set, Aij);

  if (special_Apq_name (ntstring, lgth)) {
    SXBA_1_bit (special_Aij_set, Aij);
    special_Aij = -spf.outputG.maxt+Aij;
    if (SXBA_bit_is_reset_set (yield_Tpq_set, special_Aij)) /* première fois qu'on le voit */
      spf_fill_Tpq2ste (special_Aij);
    return 0;
  }

  item = spf.outputG.lhs [Pij].prolon;
    
  if (spf.outputG.has_repair)
    ub = Aij2i (Aij); /* initialement la lower-bound du pere */

  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    if (spf.outputG.has_repair) {
      /* Le parseur a fait de la reparation */
      p = (Xpq > 0) ? Aij2i (Xpq) : Tij2i (-Xpq);
      q = (Xpq > 0) ? Aij2j (Xpq) : Tij2j (-Xpq);

      is_OK = p > 0 && q > 0;

      if (p > 0) {
	/* Xpq est bon */
	if (ub < p) {
	  /* On traite la correction entre [ub] .. [p] */
	  wvstr = rcvr_out_range (wvstr, ub, p);

	  if (varstr_length (wvstr))
	    spf_complete_Tpq2comment (wvstr, ub, p);

	  ub = p;
	}
      }
      else {
	if (p < 0)
	  /* Debut d'une rcvr */
	  ub = -p;
      }

      if (q < 0) {
	/* Fin d'une rcvr */
	/* On traite la correction entre [ub] .. [-q] */
	wvstr = rcvr_out_range (wvstr, ub, -q);

	if (varstr_length (wvstr))
	  spf_complete_Tpq2comment (wvstr, ub, -q);

	ub = -q;
      }
      else {
	if (q > 0)
	  ub = q;
      }
    }
    else
      is_OK = true;

    if (is_OK) {
      if (Xpq > 0) {
	Aij2rhs_occ_nb [Xpq] ++;
      }
      else {
	Tpq = -Xpq;
	if (SXBA_bit_is_reset_set (yield_Tpq_set, Tpq)) /* première fois qu'on le voit */
	  spf_fill_Tpq2ste (Tpq);
      }
    }
  }
    
  if (spf.outputG.has_repair) {
    if (ub < (j = Aij2j (Aij))) {
      /* On traite la correction entre [ub] .. [j] de fin de production */
      wvstr = rcvr_out_range (wvstr, ub, j);

      if (varstr_length (wvstr))
	spf_complete_Tpq2comment (wvstr, ub, j);
    }
  }

  return 1; /* On ne touche pas a la foret partagee */
}

static SXINT            cur_max_state;
static SXINT            minDAG_final_state;
static SXINT            minDAG_min_state, minDAG_max_state;
static XxYxZ_header     spf_dag_yield_hd;

static bool spf_empty_trans (SXINT state, SXBA epsilon_reachable_next_states);
static void spf_nfa_extract_trans (SXINT state, void (*output_trans) (SXINT, SXINT, SXINT));

#if LOG
static SXINT ntrans, nepstrans;
#endif /* LOG */

static void
minsubDAG_fill_trans (SXINT dfa_state, SXINT t, SXINT next_dfa_state)
{
  SXINT triple;

  if (t == yield_eof_ste)
    t = 0;

  dfa_state += cur_max_state;
  next_dfa_state += cur_max_state;

  if (minDAG_final_state < next_dfa_state)
    minDAG_final_state = next_dfa_state;

  if (dfa_state < next_dfa_state /* && t != yield_eof_ste */)
    XxYxZ_set (&yield_fsa_XxYxZ_hd, dfa_state, t, next_dfa_state, &triple);    
}

static SXBA state_already_duplicated;
static SXINT *oldDAG2newDAG;
static SXINT state_already_duplicated_size;

static void
allocate_subDAG_helper_structures (void)
{
  if (state_already_duplicated == NULL) {
    sxprepare_for_possible_reset (state_already_duplicated);

    state_already_duplicated = sxba_calloc (cur_max_state + 1);
    oldDAG2newDAG = (SXINT*) sxalloc (cur_max_state + 1, sizeof (SXINT));
    state_already_duplicated_size = cur_max_state;
  }
  else {
    if (cur_max_state > state_already_duplicated_size) {
      state_already_duplicated = sxba_resize (state_already_duplicated, cur_max_state + 1);
      oldDAG2newDAG = (SXINT*) sxrealloc (oldDAG2newDAG, cur_max_state + 1, sizeof (SXINT));
      state_already_duplicated_size = cur_max_state;
    }
  }
}

static void
free_subDAG_helper_structures (void)
{
  if (state_already_duplicated != NULL) {
    sxfree (state_already_duplicated), state_already_duplicated = NULL;
    sxfree (oldDAG2newDAG), oldDAG2newDAG = NULL;
    state_already_duplicated_size = 0;
  }
}

static void
reduce_subDAG (/* source */ struct subDAG_struct *subDAG_struct_source_ptr,
	       /* cible de la copie, on récupère les valeurs */ struct subDAG_struct *subDAG_struct_target_ptr)
{
  SXINT source_init_state, source_final_state;

  if ((source_init_state = subDAG_struct_source_ptr->init_state)) {
    source_final_state = subDAG_struct_source_ptr->final_state;

#if LOG
    printf("reduce_subDAG (%ld, %ld) -- START\n", source_init_state, source_final_state);
#endif /* LOG */
    subDAG_struct_target_ptr->init_state = cur_max_state + 1;

    minDAG_final_state = 0;
    minDAG_min_state = subDAG_struct_source_ptr->minp-1;
    minDAG_max_state = subDAG_struct_source_ptr->maxp;

    nfa2dfa (source_init_state - minDAG_min_state,
	     source_final_state - minDAG_min_state,
	     minDAG_max_state - minDAG_min_state/* avant y'avait cur_max_state */,
	     yield_eof_ste, 
	     spf_empty_trans,
	     spf_nfa_extract_trans, 
	     NULL, 
	     minsubDAG_fill_trans, 
	     true /* to_be_normalized */
#ifdef ESSAI_INVERSE_MAPPING
	     , NULL
#endif /* ESSAI_INVERSE_MAPPING */
	     );

    cur_max_state = subDAG_struct_target_ptr->final_state = minDAG_final_state;
    subDAG_struct_target_ptr->minp = subDAG_struct_target_ptr->init_state; /* Apres minimisation l'etat initial est le min */
    subDAG_struct_target_ptr->maxp = cur_max_state; /* Apres minimisation l'etat final est le max */
    subDAG_struct_target_ptr->eps_trans_nb = 0; /* Apres minimisation il n'y a plus d'eps trans */

#if LOG
    printf("reduce_subDAG (%ld, %ld) -- DONE (-> %ld, %ld)\n", source_init_state, source_final_state, subDAG_struct_target_ptr->init_state, subDAG_struct_target_ptr->final_state);
#endif /* LOG */
  }
  else
    *subDAG_struct_target_ptr = empty_subDAG_struct;
}

static void
duplicate_subDAG (/* source */ struct subDAG_struct *subDAG_struct_source_ptr,
		  /* cible de la copie, on récupère les valeurs */ struct subDAG_struct *subDAG_struct_target_ptr)
{
  SXINT source_init_state, source_final_state;
  SXINT transition, triple, ste;
  struct subDAG_struct source_next, target_next;

  if ((source_init_state = subDAG_struct_source_ptr->init_state)) {
    source_final_state = subDAG_struct_source_ptr->final_state;

#if LOG
    printf("duplicate_subDAG (%ld, %ld) -- START\n", source_init_state, source_final_state);
#endif /* LOG */

    if (subDAG_struct_target_ptr->init_state == 0)
      subDAG_struct_target_ptr->init_state = ++cur_max_state;

    subDAG_struct_target_ptr->minp = subDAG_struct_target_ptr->init_state; /* Ds tous les cas */

    if (subDAG_struct_target_ptr->final_state == 0)
      subDAG_struct_target_ptr->final_state = ++cur_max_state;

    source_next.final_state = source_final_state;
    target_next.final_state = subDAG_struct_target_ptr->final_state;

    XxYxZ_Xforeach (yield_fsa_XxYxZ_hd, source_init_state, transition) {
      source_next.init_state = XxYxZ_Z (yield_fsa_XxYxZ_hd, transition);
      ste = XxYxZ_Y (yield_fsa_XxYxZ_hd, transition);

      if (source_next.init_state == source_final_state) {
	XxYxZ_set (&yield_fsa_XxYxZ_hd, subDAG_struct_target_ptr->init_state, ste, subDAG_struct_target_ptr->final_state, &triple);
      } 
      else {
	if (SXBA_bit_is_reset_set (state_already_duplicated, source_next.init_state)) {
	  target_next.init_state = oldDAG2newDAG [source_next.init_state] = ++cur_max_state;
	  duplicate_subDAG (&source_next, &target_next);
	} else
	  target_next.init_state = oldDAG2newDAG [source_next.init_state];

	XxYxZ_set (&yield_fsa_XxYxZ_hd, subDAG_struct_target_ptr->init_state, ste, target_next.init_state, &triple);
      }      
    }

    subDAG_struct_target_ptr->maxp = cur_max_state;
    subDAG_struct_target_ptr->eps_trans_nb = subDAG_struct_source_ptr->eps_trans_nb;

#if LOG
    printf("duplicate_subDAG (%ld, %ld) -- DONE (-> %ld, %ld)\n", source_init_state, source_final_state
	   , subDAG_struct_target_ptr->init_state, subDAG_struct_target_ptr->final_state);
#endif /* LOG */
  }
  else
    *subDAG_struct_target_ptr = empty_subDAG_struct;
}

static void
create_trivial_subDAG (SXINT ste, struct subDAG_struct *subDAG_struct_source_ptr)
{
  SXINT triple;

  if (subDAG_struct_source_ptr->init_state == 0)
    subDAG_struct_source_ptr->init_state = ++cur_max_state;

  subDAG_struct_source_ptr->minp = subDAG_struct_source_ptr->init_state; /* Ds tous les cas */

  if (subDAG_struct_source_ptr->final_state == 0)
    subDAG_struct_source_ptr->final_state = ++cur_max_state;

  subDAG_struct_source_ptr->maxp = subDAG_struct_source_ptr->final_state; /* Ds tous les cas */
  subDAG_struct_source_ptr->eps_trans_nb = 0;

  XxYxZ_set (&yield_fsa_XxYxZ_hd, subDAG_struct_source_ptr->init_state, ste, subDAG_struct_source_ptr->final_state, &triple);
#if LOG
  printf("create_trivial_subDAG (%ld:%s) -- OK (-> %ld, %ld)\n", ste, sxstrget (ste), subDAG_struct_source_ptr->init_state, subDAG_struct_source_ptr->final_state);
#endif /* LOG */
}


/* 2eme passe bottom-up sur la foret pour extraire le dag des feuilles de ses arbres */
/* les firstpos de item+1 sont des followpos des lastpos de item */
/* calcule pour chaque Tpq ds followpos [Tpq] l'ensemble des Tpq qui sont des suivants possibles */
static SXINT
spf_second_yield_bu_walk (SXINT Pij)
{
  SXINT                cur_Xpq, next_Xpq, item, ste, Aij, triple, global_first_state;
  struct subDAG_struct cur_Xpq_subDAG, next_Xpq_subDAG;

  Aij = spf.walk.cur_Aij;

  if (!SXBA_bit_is_set (yield_valid_Aij_set, Aij))
    /* on est en dessous d'un NT spécial: on zappe */
    return 1;

  item = spf.outputG.lhs [Pij].prolon;

  while ((cur_Xpq = spf.outputG.rhs [item++].lispro) != 0
	 && (cur_Xpq > 0
	     && !SXBA_bit_is_set (special_Aij_set, cur_Xpq)
	     && Aij2subDAG [cur_Xpq].init_state == 0 /* déjà traité mais associé à aucun sous-DAG: yield vide */));
  
  /* Nelle version */
  if (cur_Xpq != 0) {
    /* Pij est non vide */
    /* ici cur_Xpq est le premier symbole de partie droite */
    cur_Xpq_subDAG = empty_subDAG_struct;

    if (cur_Xpq > 0 && !SXBA_bit_is_set (special_Aij_set, cur_Xpq)) {
      /* NT non-spécial */
      if (++Aij2visited_rhs_occ_nb [cur_Xpq] < Aij2rhs_occ_nb [cur_Xpq]) {
	/* Si le sous-dag a trop de transitions epsilon, on le minimise */
	if (Aij2subDAG [cur_Xpq].eps_trans_nb > SUBDAG_THRESHOLD)
	  reduce_subDAG (Aij2subDAG+cur_Xpq,
			 /* cible de la copie, on récupère les valeurs */ Aij2subDAG+cur_Xpq);

	/* on doit recopier le sous-DAG correspondant à cur_Xpq */
	allocate_subDAG_helper_structures ();
	duplicate_subDAG (/* source */ Aij2subDAG+cur_Xpq,
			  /* cible de la copie, on récupère les valeurs */ &cur_Xpq_subDAG);
	sxba_empty (state_already_duplicated);
      }
      else
	cur_Xpq_subDAG = Aij2subDAG [cur_Xpq];
    }
    else {
      /* T ou NT spécial */
      if (cur_Xpq < 0)
	ste = Tpq2ste [-cur_Xpq];
      else 
	ste = Tpq2ste [-spf.outputG.maxt+cur_Xpq];

      create_trivial_subDAG (ste, &cur_Xpq_subDAG);
    }

    /* Ici cur_Xpq_subDAG contient (une copie de) le sub_dag du 1er symbole */

    while ((next_Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      next_Xpq_subDAG = empty_subDAG_struct;

      if (next_Xpq > 0 && !SXBA_bit_is_set (special_Aij_set, next_Xpq)) {
	/* NT non-spécial */
	if (++Aij2visited_rhs_occ_nb [next_Xpq] < Aij2rhs_occ_nb [next_Xpq]) {
	  /* Si le sous-dag a trop de transitions epsilon, on le minimise */
	  if (Aij2subDAG [next_Xpq].eps_trans_nb > SUBDAG_THRESHOLD)
	    reduce_subDAG (Aij2subDAG+next_Xpq,
			   /* cible de la copie, on récupère les valeurs */ Aij2subDAG+next_Xpq);

	  /* on doit recopier le sous-DAG correspondant à next_Xpq */
	  allocate_subDAG_helper_structures ();
	  duplicate_subDAG (/* source */ Aij2subDAG+next_Xpq,
			    /* cible de la copie, on récupère les valeurs */ &next_Xpq_subDAG);
	  sxba_empty (state_already_duplicated);
	}
	else
	  next_Xpq_subDAG = Aij2subDAG [next_Xpq];
      }
      else {
	/* T ou NT spécial */
	if (next_Xpq < 0)
	  ste = Tpq2ste [-next_Xpq];
	else 
	  ste = Tpq2ste [-spf.outputG.maxt+next_Xpq];

	create_trivial_subDAG (ste, &next_Xpq_subDAG);
      }

      /* On doit concatener cur_Xpq_subDAG et next_Xpq_subDAG */
      /* version grossiere (on pourrait le faire avec la duplication) */
      if (cur_Xpq_subDAG.final_state && next_Xpq_subDAG.init_state) {
	XxYxZ_set (&yield_fsa_XxYxZ_hd, cur_Xpq_subDAG.final_state, 0, next_Xpq_subDAG.init_state, &triple);
	cur_Xpq_subDAG.final_state = next_Xpq_subDAG.final_state;

	if (next_Xpq_subDAG.minp < cur_Xpq_subDAG.minp) cur_Xpq_subDAG.minp = next_Xpq_subDAG.minp;
	if (next_Xpq_subDAG.maxp > cur_Xpq_subDAG.maxp) cur_Xpq_subDAG.maxp = next_Xpq_subDAG.maxp;

	cur_Xpq_subDAG.eps_trans_nb += next_Xpq_subDAG.eps_trans_nb+1;
      }
      else {
	if (next_Xpq_subDAG.init_state)
	  cur_Xpq_subDAG = next_Xpq_subDAG;
      }
      
      /* Ici cur_Xpq_subDAG contient le resultat de la concatenation */
    }

    /* On fait le ou avec la LHS */
    if ((global_first_state = Aij2subDAG [Aij].init_state) == 0) { // première visite d'un Pij ayant ce Aij en lhs
      Aij2subDAG [Aij] = cur_Xpq_subDAG;
    }
    else {
      if (cur_Xpq_subDAG.init_state) {
	XxYxZ_set (&yield_fsa_XxYxZ_hd, global_first_state, 0, cur_Xpq_subDAG.init_state, &triple);
	XxYxZ_set (&yield_fsa_XxYxZ_hd, cur_Xpq_subDAG.final_state, 0, Aij2subDAG [Aij].final_state, &triple);

	if (cur_Xpq_subDAG.minp < Aij2subDAG [Aij].minp) Aij2subDAG [Aij].minp = cur_Xpq_subDAG.minp;
	if (cur_Xpq_subDAG.maxp > Aij2subDAG [Aij].maxp) Aij2subDAG [Aij].maxp = cur_Xpq_subDAG.maxp;

	Aij2subDAG [Aij].eps_trans_nb += cur_Xpq_subDAG.eps_trans_nb+2;
      }
    }

    if (Aij2subDAG [Aij].eps_trans_nb > SUBDAG_THRESHOLD_2)    
      reduce_subDAG (Aij2subDAG+Aij,
		     /* cible de la copie, on récupère les valeurs */ Aij2subDAG+Aij);

  }

#if 0
  if (cur_Xpq != 0) {
    /* Pij est non vide */
    /* ici cur_Xpq est le premier symbole de partie droite */
    if (cur_Xpq > 0 && !SXBA_bit_is_set (special_Aij_set, cur_Xpq)
	&& Aij2visited_rhs_occ_nb [cur_Xpq] + 1 == Aij2rhs_occ_nb [cur_Xpq]) {
      /* cas particulier: cur_Xpq est un NT non-spécial que l'on verra pour la dernière fois: pas besoin de recopier son sous-DAG */
      cur_Xpq_subDAG.final_state = Aij2subDAG [cur_Xpq].init_state;
      cur_Xpq_subDAG = Aij2subDAG [cur_Xpq];
    }
    else {
      cur_Xpq_subDAG.final_state = 0;
      cur_Xpq_subDAG = empty_subDAG_struct;
    }

    first_state = cur_Xpq_subDAG.final_state;

    while ((next_Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      if (next_Xpq < 0 || SXBA_bit_is_set (special_Aij_set, next_Xpq) || Aij2subDAG [next_Xpq].init_state != 0 /* == 0 veut dire déjà traité mais associé à aucun sous-DAG: yield vide */) {
	cur_Xpq_subDAG.init_state /* prev de ce coup ci */ = cur_Xpq_subDAG.final_state /* prev du coup d'avant */;

	if (next_Xpq > 0 && !SXBA_bit_is_set (special_Aij_set, next_Xpq)
	    && Aij2visited_rhs_occ_nb [next_Xpq] + 1 == Aij2rhs_occ_nb [next_Xpq]) {
	  /* cas particulier: next_Xpq est un NT non-spécial que l'on verra pour la dernière fois: pas besoin de recopier son sous-DAG */
	  next_Xpq_subDAG_init_state = Aij2subDAG [next_Xpq].init_state;
	  next_Xpq_subDAG = Aij2subDAG [next_Xpq];
	}
	else {
	  next_Xpq_subDAG_init_state = 0;
	  next_Xpq_subDAG = empty_subDAG_struct;
	}

#if 0
	if (cur_Xpq > 0 && !SXBA_bit_is_set (special_Aij_set, cur_Xpq)
	    && Aij2visited_rhs_occ_nb [cur_Xpq] + 1 == Aij2rhs_occ_nb [cur_Xpq]) {
	  /* cas particulier: cur_Xpq est un NT non-spécial que l'on verra pour la dernière fois: pas besoin de recopier son sous-DAG */
	  cur_Xpq_subDAG.final_state = Aij2subDAG [cur_Xpq].final_state;
	}
	else {
	  cur_Xpq_subDAG.final_state = 0;
	}
#endif /* 0 */

	if (next_Xpq_subDAG.init_state > 0) {
	  if (cur_Xpq_subDAG.final_state == 0)
	    cur_Xpq_subDAG.final_state = next_Xpq_subDAG.init_state;
	  else {
	    /* eps-trans entre le sousDAG de cur_Xpq et celui de next_Xpq, puisqu'aucun des deux n'a besoin d'être recopié */
	    XxYxZ_set (&yield_fsa_XxYxZ_hd, cur_Xpq_subDAG.final_state, 0, next_Xpq_subDAG.init_state, &triple);
	  }
	}

#if 0
	if (next_Xpq_subDAG_init_state > 0) {
	  if (cur_Xpq_subDAG.final_state == 0)
	    cur_Xpq_subDAG.final_state = next_Xpq_subDAG_init_state;
	  else {
	    /* eps-trans entre le sousDAG de cur_Xpq et celui de next_Xpq, puisqu'aucun des deux n'a besoin d'être recopié */
	    XxYxZ_set (&yield_fsa_XxYxZ_hd, cur_Xpq_subDAG.final_state, 0, next_Xpq_subDAG_init_state, &triple);
	  }
	}
#endif /* 0 */

	if (cur_Xpq > 0 && !SXBA_bit_is_set (special_Aij_set, cur_Xpq)) {
	  /* NT non-spécial */
	  if (++Aij2visited_rhs_occ_nb [cur_Xpq] < Aij2rhs_occ_nb [cur_Xpq]) {
	    /* la première fois qu'on doit dupliquer un sous-dag, on le réduit */
	    if (Aij2visited_rhs_occ_nb [cur_Xpq] == 1)
	      reduce_subDAG (Aij2subDAG+cur_Xpq,
			     /* cible de la copie, on récupère les valeurs */ Aij2subDAG+cur_Xpq);

	    /* on doit recopier le sous-DAG correspondant à cur_Xpq */
	    allocate_subDAG_helper_structures ();
	    duplicate_subDAG (/* source */ Aij2subDAG+cur_Xpq,
			      /* cible de la copie, on récupère les valeurs */ &cur_Xpq_subDAG);
	    sxba_empty (state_already_duplicated);
	  }
	}
	else {
	  /* T ou NT spécial */
	  if (cur_Xpq < 0)
	    ste = Tpq2ste [-cur_Xpq];
	  else 
	    ste = Tpq2ste [-spf.outputG.maxt+cur_Xpq];

	  create_trivial_subDAG (ste, &cur_Xpq_subDAG);
	}

	if (first_state == 0) 
	  first_state = cur_Xpq_subDAG.init_state;

	cur_Xpq = next_Xpq;
	cur_Xpq_subDAG = next_Xpq_subDAG;
      }
    }

    /* des trucs sur le dernier */
#if 0
    cur_Xpq_subDAG.init_state = cur_Xpq_subDAG.final_state;

    if (cur_Xpq > 0 && !SXBA_bit_is_set (special_Aij_set, cur_Xpq)
	&& Aij2visited_rhs_occ_nb [cur_Xpq] + 1 == Aij2rhs_occ_nb [cur_Xpq])
      /* cas particulier: cur_Xpq est un NT non-spécial que l'on verra pour la dernière fois: pas besoin de recopier son sous-DAG */
      cur_Xpq_subDAG.final_state = Aij2subDAG [cur_Xpq].final_state;
    else
      cur_Xpq_subDAG.final_state = 0;
#endif /* 0 */
    
    if (cur_Xpq > 0 && !SXBA_bit_is_set (special_Aij_set, cur_Xpq)) {
      /* NT non-spécial */
      if (Aij2subDAG [cur_Xpq].init_state != 0 /* == 0 veut dire déjà traité mais associé à aucun sous-DAG: yield vide */ &&
	  ++Aij2visited_rhs_occ_nb [cur_Xpq] < Aij2rhs_occ_nb [cur_Xpq]) {
	/* la première fois qu'on doit dupliquer un sous-dag, on le réduit */
	if (Aij2visited_rhs_occ_nb [cur_Xpq] == 1)
	  reduce_subDAG (Aij2subDAG+cur_Xpq,
			 /* cible de la copie, on récupère les valeurs */ Aij2subDAG+cur_Xpq);

	/* on doit recopier le sous-DAG correspondant à cur_Xpq */
	allocate_subDAG_helper_structures ();
	duplicate_subDAG (/* source */ Aij2subDAG+cur_Xpq,
			  /* cible de la copie, on récupère les valeurs */ &cur_Xpq_subDAG);
	sxba_empty (state_already_duplicated);
      }      
    }
    else {
      /* T ou NT spécial */
      if (cur_Xpq < 0)
	ste = Tpq2ste [-cur_Xpq];
      else 
	ste = Tpq2ste [-spf.outputG.maxt+cur_Xpq];
      
      create_trivial_subDAG (ste, &cur_Xpq_subDAG);
    }

    if (first_state == 0) 
      first_state = cur_Xpq_subDAG.init_state;
    
    if ((global_first_state = Aij2subDAG [Aij].init_state) == 0) { // première visite d'un Pij ayant ce Aij en lhs
      Aij2subDAG [Aij].init_state = first_state;
      Aij2subDAG [Aij].final_state = cur_Xpq_subDAG.final_state;
    }
    else {
      XxYxZ_set (&yield_fsa_XxYxZ_hd, global_first_state, 0, first_state, &triple);
      XxYxZ_set (&yield_fsa_XxYxZ_hd, cur_Xpq_subDAG.final_state, 0, Aij2subDAG [Aij].final_state, &triple);
    }
  }
#endif /* 0 */

  return 1; /* On ne touche pas a la foret partagee */
}


static void
spf_nfa_extract_trans (SXINT state, void (*output_trans) (SXINT, SXINT, SXINT))
{
  SXINT next_state, ste, transition;

  XxYxZ_Xforeach (yield_fsa_XxYxZ_hd, state+minDAG_min_state, transition) {
    if ((ste = XxYxZ_Y (yield_fsa_XxYxZ_hd, transition)) > 0) { /* transition pas sur epsilon */
#if LOG
      ntrans ++;
#endif /* LOG */
      next_state = XxYxZ_Z (yield_fsa_XxYxZ_hd, transition)-minDAG_min_state;
      (*output_trans) (state, ste, next_state);
    }
  }
}


static SXINT          *pseudofinal_trans_to_state_stack;
static SXINT          *notpseudofinal_trans_to_state_stack;

/* Caracteristique de 2 appels successifs : spf_mindfa_fill_trans (dfa_state1, t1, next_dfa_state1) et spf_mindfa_fill_trans (dfa_state2, t2, next_dfa_state2)
     - dfa_state1 <= dfa_state2
     - Si dfa_state1 == dfa_state2 alors t1 < t2
   Si DAG on a dfa_state1 < next_dfa_state1
 */
static void
spf_mindfa_fill_trans (SXINT dfa_state, SXINT t, SXINT next_dfa_state)
{
  SXINT triple;

  if (t == yield_eof_ste)
    /* On change l'etat final en supprimant la derniere transition */
    next_dfa_state--;
  
  if (minDAG_final_state < next_dfa_state)
    minDAG_final_state = next_dfa_state;

  if (dfa_state < next_dfa_state) {
    if (t == yield_eof_ste) {
      if (pseudofinal_trans_to_state_stack == NULL)
	DALLOC_STACK (pseudofinal_trans_to_state_stack, minDAG_final_state);
      DPUSH (pseudofinal_trans_to_state_stack, dfa_state);
    } else {
      XxYxZ_set (&spf_dag_yield_hd, dfa_state, t, next_dfa_state, &triple);    
      if (notpseudofinal_trans_to_state_stack == NULL)
	DALLOC_STACK (notpseudofinal_trans_to_state_stack, minDAG_final_state);
      DPUSH (notpseudofinal_trans_to_state_stack, dfa_state);
    }
  }  
}

static void
sxspf_yielddfa_eliminate_eof_transitions (void)
{
  SXINT triple, newtriple, p, t, q;
  SXINT maxtriple = XxYxZ_top (spf_dag_yield_hd);
  SXBA pseudofinal_trans_to_state_set = NULL, notpseudofinal_trans_to_state_set = NULL;

  if (pseudofinal_trans_to_state_stack != NULL) {
    pseudofinal_trans_to_state_set = sxba_calloc (minDAG_final_state + 1);
    
    while (!IS_EMPTY (pseudofinal_trans_to_state_stack)) {
      p = DPOP (pseudofinal_trans_to_state_stack);
      SXBA_1_bit (pseudofinal_trans_to_state_set, p);
    }
  }

  if (notpseudofinal_trans_to_state_stack != NULL) {
    notpseudofinal_trans_to_state_set = sxba_calloc (minDAG_final_state + 1);

    while (!IS_EMPTY (notpseudofinal_trans_to_state_stack)) {
      p = DPOP (notpseudofinal_trans_to_state_stack);
      SXBA_1_bit (notpseudofinal_trans_to_state_set, p);
    }
  }

  for (p = 1; p < minDAG_final_state; p++) {
    XxYxZ_Xforeach (spf_dag_yield_hd, p, triple) {
      if (triple <= maxtriple) {
	/* c'est pas une transition qu'on a rajouté dans cette fonction (ça marche parce que
	   XxYxZ_erase ne libère pas les valeurs de triple pour réutilisation par XxYxZ_set) */
	t = XxYxZ_Y (spf_dag_yield_hd, triple);
	q = XxYxZ_Z (spf_dag_yield_hd, triple);
	if (pseudofinal_trans_to_state_set && SXBA_bit_is_set (pseudofinal_trans_to_state_set, q)) {
	  // q est pseudo-final
	  if (UDAG_wanted) {
	    printf ("%ld\t%s\t%ld\n", (SXINT) p, SXWORD_get (transition_names, t), (SXINT) minDAG_final_state);
	    if (notpseudofinal_trans_to_state_set && SXBA_bit_is_set (notpseudofinal_trans_to_state_set, q))
	      printf ("%ld\t%s\t%ld\n", (SXINT) p, SXWORD_get (transition_names, t), (SXINT) q);
	  } else {
	    if (notpseudofinal_trans_to_state_set == NULL || !SXBA_bit_is_set (notpseudofinal_trans_to_state_set, q))
	      XxYxZ_erase (spf_dag_yield_hd, triple);
	    XxYxZ_set (&spf_dag_yield_hd, p, t, minDAG_final_state, &newtriple);
	    /* newtriple est donc supérieur à maxtriple */
	  }	  
	} else {
	  if (UDAG_wanted)
	    printf ("%ld\t%s\t%ld\n", (SXINT) p, SXWORD_get (transition_names, t), (SXINT) q);
	}
      }
    }
  }

  if (pseudofinal_trans_to_state_set)
    sxfree (pseudofinal_trans_to_state_set);

  if (notpseudofinal_trans_to_state_set)
    sxfree (notpseudofinal_trans_to_state_set);
  
}

/* Procedure appelee par dag2re qui permet de recuperer le "texte" associe' aux transitions qui sont ici des tok_no */
/* Marche avec le (vrai) source et non pas le source lexicalise' ds le cas insideG */
static char*
spf_get_yield_trans_name (SXINT ste)
{
  return SXWORD_get (transition_names, ste);
}

/* Procedure appelee par dag2re qui permet d'extraire de spf_dag_yield_hd les transitions depuis l'etat p */
static void
spf_dag_yield_extract_trans (SXINT p, void (*f) (SXINT p, SXINT t, SXINT q))
{
  SXINT triple, t, q;

  XxYxZ_Xforeach (spf_dag_yield_hd, p, triple) {
    t = XxYxZ_Y (spf_dag_yield_hd, triple);
    q = XxYxZ_Z (spf_dag_yield_hd, triple);
    
    (*f) (p, t, q);
  }
}

static bool
spf_empty_trans (SXINT state, SXBA epsilon_reachable_next_states)
{
  SXINT transition, next_state = 0;

  XxYxZ_Xforeach (yield_fsa_XxYxZ_hd, state+minDAG_min_state, transition) {
    if (XxYxZ_Y (yield_fsa_XxYxZ_hd, transition) == 0) { /* transition sur epsilon */
#if LOG
      nepstrans ++;
#endif /* LOG */
      next_state = XxYxZ_Z (yield_fsa_XxYxZ_hd, transition)-minDAG_min_state;
      SXBA_1_bit (epsilon_reachable_next_states, next_state);
    }
  }

  return (next_state != 0);
}

/* On extrait directement un dfa */
static void
spf_yield_extract_dfa (void)
{
  SXINT          spf_dag_yield_hd_foreach [] = {1, 0, 0, 0, 0, 0}; /* Xforeach */
  struct subDAG_struct *subDAG_struct_source_ptr = Aij2subDAG + spf.outputG.start_symbol;
  SXINT source_init_state, source_final_state;
  

  if (subDAG_struct_source_ptr->final_state == 0) {
    if (UDAG_wanted)
      puts ("##DAG BEGIN\n##DAG END");
    else
      puts ("% empty DAG");
    return;
  }

  minDAG_final_state = 0;

  if (UDAG_wanted) {
    puts ("##DAG BEGIN");
  }
  /* il faut pouvoir retrouver les t tels que p ->t q a partir des (p, q) */
  XxYxZ_alloc (&spf_dag_yield_hd, "spf_dag_yield_hd", cur_max_state, 1, spf_dag_yield_hd_foreach, NULL, NULL);

  /* On déterminise et minimize le DAG obtenu */
  /* extern void nfa2dfa (SXINT init_state,
		     SXINT final_state,
		     SXINT 0,
		     SXINT eof_ste,
		     bool (*empty_trans)(SXINT, SXBA), 
		     void (*nfa_extract_trans)(SXINT, void (*nfa_fill_trans)(SXINT, SXINT, SXINT) ), 
		     void (*dfa_fill_trans)(SXINT, SXINT, SXINT, bool), 
		     void (*mindfa_fill_trans)(SXINT, SXINT, SXINT), 
		     bool to_be_normalized);
  */
#if LOG
  print_subDAG (subDAG_struct_source_ptr);

  ntrans = 0;
  nepstrans = 0;
#endif /* LOG */
    minDAG_min_state = subDAG_struct_source_ptr->minp-1;
    minDAG_max_state = subDAG_struct_source_ptr->maxp;
    source_init_state = subDAG_struct_source_ptr->init_state;
    source_final_state = subDAG_struct_source_ptr->final_state;

  /*  nfa2dfa (1, cur_max_state, 0, yield_eof_ste, 
	   spf_empty_trans,
	   spf_nfa_extract_trans, 
	   NULL, */
  nfa2dfa (source_init_state - minDAG_min_state,
	   source_final_state - minDAG_min_state,
	   minDAG_max_state - minDAG_min_state/* avant y'avait cur_max_state */,
	   yield_eof_ste, 
	   spf_empty_trans,
	   spf_nfa_extract_trans, 
	   NULL,
 	   spf_mindfa_fill_trans, 
	   true /* to_be_normalized */
#ifdef ESSAI_INVERSE_MAPPING
	   , NULL
#endif /* ESSAI_INVERSE_MAPPING */
	   );
#if LOG
  printf("ntrans=%ld, nepstrans=%ld\n", ntrans, nepstrans);
#endif /* LOG */


  sxspf_yielddfa_eliminate_eof_transitions ();

  if (UDAG_wanted) {
    puts ("##DAG END");
  }
  else {
    /* On sort le DAG */
    wvstr = dag2re (varstr_raz (wvstr), 1 /* etat initial */, minDAG_final_state /* etat final */, yield_eof_ste, spf_dag_yield_extract_trans, spf_get_yield_trans_name);
    printf ("%s\n", varstr_tostr (wvstr));
  }

  XxYxZ_free (&spf_dag_yield_hd);

  if (notpseudofinal_trans_to_state_stack)
    DFREE_STACK (notpseudofinal_trans_to_state_stack), notpseudofinal_trans_to_state_stack = NULL;

  if (pseudofinal_trans_to_state_stack)
    DFREE_STACK (pseudofinal_trans_to_state_stack), pseudofinal_trans_to_state_stack = NULL;
}

void
spf_yield2dfa (bool which_form)
{
  SXINT maxtnt;
  SXINT yield_fsa_XxYxZ_foreach[6] = {1, 0, 0, 0, 0, 0};
	
  UDAG_wanted = which_form;

  if (wvstr == NULL) {
    sxprepare_for_possible_reset (wvstr);
    wvstr = varstr_alloc (64);
  }

  if (spf.outputG.Tij2tok_no_stack == NULL)
    spf_fill_Tij2tok_no ();

  yield_eof_Tpq = -spf.outputG.maxt+1;
  maxxnt = spf.outputG.maxxnt;
  maxtnt = -spf.outputG.maxt+spf.outputG.maxxnt;

  special_Aij_set = sxba_calloc (maxxnt+1);
  yield_valid_Aij_set = sxba_calloc (maxxnt+1);
  yield_Tpq_set = sxba_calloc (maxtnt+1); /* Il peut y avoir des nt speciaux */
  Tpq2ste = (SXINT*) sxcalloc (maxtnt+1, sizeof (SXINT));
  Aij2rhs_occ_nb = (SXINT*) sxcalloc (maxxnt+1, sizeof (SXINT));
  Aij2visited_rhs_occ_nb = (SXINT*) sxcalloc (maxxnt+1, sizeof (SXINT));
  Aij2subDAG = (struct subDAG_struct *) sxcalloc (maxxnt+1, sizeof (struct subDAG_struct));
  XxYxZ_alloc (&yield_fsa_XxYxZ_hd, "yield_fsa_XxYxZ_hd", maxxnt+1, 1, yield_fsa_XxYxZ_foreach, NULL, NULL);
  sxword_alloc (&transition_names, "transition_names", yield_eof_Tpq+1, 1, 32, sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL /* transition_names_oflw */, NULL);

  fill_idag_q2pq_stack ();

  spf_topological_top_down_walk (spf.outputG.start_symbol, spf_first_yield_td_walk, NULL, NULL);

  yield_eof_ste = Tpq2ste [yield_eof_Tpq] = sxword_2save (&transition_names, "<EOF>", 5);

  cur_max_state = 1; /* car 1 sera utilisé comme état initial */
  spf_topological_bottom_up_walk (spf.outputG.start_symbol, spf_second_yield_bu_walk, NULL, NULL);

  free_subDAG_helper_structures ();

#if 0
  /* on relie 1 au vrai état initial et le vrai état final à ++cur_max_state de sorte que l'état
     initial soit vraiment 1 et l'état final soit le plus grand des états (nécessaire pour
     nfa2dfa) */
  XxYxZ_set (&yield_fsa_XxYxZ_hd, 1, 0, Aij2subDAG [spf.outputG.start_symbol].init_state, &triple);
  XxYxZ_set (&yield_fsa_XxYxZ_hd, Aij2subDAG [spf.outputG.start_symbol].final_state, yield_eof_ste, ++cur_max_state, &triple);
#endif /* 0 */

  spf_yield_extract_dfa ();

  sxfree (special_Aij_set), special_Aij_set = NULL;
  sxfree (yield_valid_Aij_set), yield_valid_Aij_set = NULL;
  sxfree (yield_Tpq_set), yield_Tpq_set = NULL;
  sxfree (Tpq2ste), Tpq2ste = NULL;
  sxfree (Aij2rhs_occ_nb), Aij2rhs_occ_nb = NULL;
  sxfree (Aij2visited_rhs_occ_nb), Aij2visited_rhs_occ_nb = NULL;
  sxfree (Aij2subDAG), Aij2subDAG = NULL;
  XxYxZ_free (&yield_fsa_XxYxZ_hd);

  sxword_free (&transition_names);

  varstr_raz (wvstr);
}
