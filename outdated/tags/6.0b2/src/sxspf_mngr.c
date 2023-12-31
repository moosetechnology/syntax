/* ********************************************************
 *                                                      *
 *                                                      *
 * Copyright (c) 2006 by Institut National de Recherche *
 *                    en Informatique et en Automatique *
 *                                                      *
 *                                                      *
 ******************************************************** */




/* ********************************************************
 *                                                        *
 *     Produit de l'equipe ATOLL.			  *
 *                                                        *
 ******************************************************** */

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

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Jeu 14 sep 2006 16:50:	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "sxunix.h"

char WHAT_SXSPF_MNGR[] = "@(#)SYNTAX - $Id: sxspf_mngr.c 565 2007-05-23 10:05:12Z syntax $" WHAT_DEBUG;

#if EBUG
static char	ME [] = "sxspf_mngr";
#endif

/* Pour ne pas avoir ici la definition et l'initialisation de spf propos�e par sxspf.h,
   on ne fait pas #define SX_DFN_EXT_VAR2 */
#include "XxYxZ.h"
#include "earley.h"
#define SX_DFN_EXT_VAR
#include "sxspf.h"
#undef SX_DFN_EXT_VAR
#include "sxstack.h"

static int          Axixj2Aij_hd_foreach [6] /* Pas de foreach */
                    , Txixj2Tij_hd_foreach [6] =  {0, 1, 0, 0, 0, 0} /* Yforeach par Tij_iforeach */
                    ;

static int          outputG_lhs_size, outputG_rhs_size;
static int          *Aij2lhs_nb;
static int          count_maxprod, count_maxitem;
static SXBA         count_Tij_set, count_Aij_set;
static FILE         *spf_print_forest_out_file;

#if EBUG
static SXBA         spf_make_proper_check_td_set, spf_make_proper_check_bu_set;
#endif /* EBUG */

static void
Axixj2Aij_hd_oflw (old_size, new_size)
     int old_size, new_size;
{
  if (Aij2lhs_nb) Aij2lhs_nb = (int*) sxrecalloc (Aij2lhs_nb, old_size+1, new_size+1, sizeof (int));
  if (spf.outputG.Aij2rhs_nb) spf.outputG.Aij2rhs_nb = (int*) sxrecalloc (spf.outputG.Aij2rhs_nb, old_size+1, new_size+1, sizeof (int));

  /* Truc pour eviter que ds spf_print_iprod on ecrive des nt instancies du
     traitement d'erreurs */
  spf.outputG.maxnt = new_size+1;
}

static void
Txixj2Tij_hd_oflw (old_size, new_size)
     int old_size, new_size;
{
  if (spf.outputG.Tpq_rcvr_set) spf.outputG.Tpq_rcvr_set = sxba_resize (spf.outputG.Tpq_rcvr_set, new_size+1);
}


static int
spf_print_td_or_bu_prod (Pij)
     int Pij;
{
  spf_print_iprod (spf_print_forest_out_file, Pij, "", "\n");

  return 1;
}


static int
call_spf_print_iprod (prod)
     int prod;
{
  spf_print_iprod (stdout, prod, "", "\n");
  return 1;
}


static int
spf_tree_count_pass_derived (int Pij)
{
  double     tree_nb = (double)1;
  int        item, Xpq, Tpq, Aij;
  struct lhs *plhs;

  plhs = spf.outputG.lhs+Pij;

  if (count_Aij_set) {
    /* On veut des stats completes sur la grammaire */
    count_maxprod++;
    count_maxitem++;

#if LLOG
    printf ("%i: ", count_maxprod);
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


#if 0
/* Le 22/02/07 count_td_walk est fusionne avec spf_tree_count_pass_derived */
/* Valide entre la racine et les noeuds maximaux */
static int
count_td_walk (prod)
     int prod;
{
  int        Aij, Xpq, Tpq;
  struct lhs *plhs;
  struct rhs *p;

  count_maxprod++;

#if LLOG
  printf ("%i: ", count_maxprod);
  spf_print_iprod (stdout, prod, "", "\n");
#endif /* LLOG */

  count_maxitem++;

  plhs = spf.outputG.lhs+prod;
  Aij = plhs->lhs /* static */;

  SXBA_1_bit (count_Aij_set, Aij);
  
  p = spf.outputG.rhs+plhs->prolon-1;

  while ((Xpq = (++p)->lispro) != 0) {
    count_maxitem++;

    if (Xpq < 0) {
      /* terminal */
      Tpq = -Xpq;
      SXBA_1_bit (count_Tij_set, Tpq);
    }
    else
      SXBA_1_bit (count_Aij_set, Xpq);
  }

  return 1;
}

static int
spf_tree_count_pass_derived (prod)
     int prod;
{
  double tree_nb = (double)1;
  int    item, Xpq;

  item = spf.outputG.lhs [prod].prolon;

  while ((Xpq = spf.outputG.rhs [item].lispro) != 0) {
    if (Xpq > 0 /* nt ... */ && (Xpq <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Xpq)) /* ... productif */)
      tree_nb *= spf.tree_count.nt2nb [Xpq];

    item++;
  }

  spf.tree_count.prod2nb [prod] = spf.tree_count.nt2nb [spf.outputG.lhs [prod].lhs] += tree_nb;
  /* Cumule ds prod2nb les nb associes aux prods precedentes */
  /* ... c,a permet de la recherche dichotomique ds spf_dag2tree */
  
  return 1;
}
#endif /* 0 */

#if LOG
static int
spf_make_proper_check_td (Pij)
     int Pij;
{
#if EBUG
  SXBA_1_bit (spf_make_proper_check_td_set, Pij);
#endif /* EBUG */

  return 1;
}

static int
spf_make_proper_check_bu (Pij)
     int Pij;
{
#if EBUG
  SXBA_1_bit (spf_make_proper_check_bu_set, Pij);
#endif /* EBUG */

  return 1;
}
#endif /* LOG */

#if 0
/* Voir l'appelant */
static int
spf_pre_bu_reachable (int Aij)
{
  spf.outputG.Aij2rhs_nb [Aij] = 0;

  return 1;
}
#endif /* 0 */

static int
spf_bu_reachable (Pij)
     int Pij;
{
  int item, Xpq;

  SXBA_1_bit (spf.walk.reachable_Pij_set, Pij);

  item = spf.outputG.lhs [Pij].prolon;

  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    /* On parcourt la RHS de la prod instanciee */
    if (Xpq > 0 && (Xpq <= spf.outputG.maxnt || !SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Xpq)) /* ... productif */) {
      /* Xpq est accessible */
      spf.outputG.Aij2rhs_nb [Xpq]++;
    }
  }

  return 1;
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

/* Corps recursif */
static BOOLEAN
spf_walk (Aij)
     int Aij;
{
  int             item, Xpq, hook, base_hook, result_kind, pd_kind, pi_kind, Pij;
  BOOLEAN         must_be_kept, Pij_kept;

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
      pi_kind = (*spf.walk.post_pass) (Aij, result_kind > 0);

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
	    spf.outputG.rhs [hook].lispro = -Pij; /* On l'elimine ... */
	    spf.outputG.is_proper = FALSE;
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
  Pij_kept = FALSE;

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
	  spf.outputG.is_proper = FALSE;
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
		spf.outputG.is_proper = FALSE; /* Pour le cas cycliqye */
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
	Pij_kept = TRUE;
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
	    spf.outputG.is_proper = FALSE;
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
      must_be_kept = FALSE;

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
	    spf.outputG.is_proper = FALSE;
	  }
	  else {
	    /* Ce Pij n'est pas elimine' */
	    must_be_kept = TRUE;
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
	    spf.outputG.is_proper = FALSE;
	  }

	  hook++;
	}
    
	must_be_kept = FALSE;
      }
    }
  }
  else
    must_be_kept = result_kind >= 0;

  return must_be_kept;
}


/* Retourne faux ssi toute la foret a disparue !! */
/* Wrappeur pour spf_topological_[top_down_|bottom_up_]walk */
static BOOLEAN
spf_call_walk (root)
     int root;
{
  int     hook, item, Xpq, Aij, Pij;
  BOOLEAN ret_val;

  if (spf.walk.invalid_Aij_set == NULL)
    spf.walk.invalid_Aij_set = sxba_calloc (spf.walk.Aij_top+1);
  else
    sxba_empty (spf.walk.invalid_Aij_set);

  if (spf.walk.pass_inherited) {
    /* Passe heritee demandee.  Les Aij-prods ne sont appelees que lorsque toutes les prods (de la sous-foret root)
       ayant un Aij en partie droite ont ete appelees */
    /* init ... */
    if (spf.walk.Aij2nyp_nb == NULL)
      spf.walk.Aij2nyp_nb = (int*) sxcalloc ((unsigned long int)spf.walk.Aij_top+1, sizeof (int));
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
	spf.walk.Aij_stack = (int*) sxalloc (spf.walk.Aij_top+1, sizeof (int));

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

  if (spf.walk.Aij_set == NULL) {
    spf.walk.Aij_set = sxba_calloc (spf.walk.Aij_top+1);
    spf.walk.cyclic_Aij_set = sxba_calloc (spf.walk.Aij_top+1);
  }
  else
    sxba_empty (spf.walk.Aij_set);

  spf.walk.prev_td_Aij = spf.walk.prev_bu_Aij = 0;

  SXBA_1_bit (spf.walk.Aij_set, root);
  SXBA_1_bit (spf.walk.cyclic_Aij_set, root);

  ret_val = spf_walk (root);

  SXBA_0_bit (spf.walk.cyclic_Aij_set, root);

  if (!spf.outputG.is_proper)
    ret_val = spf_make_proper (root);

  return ret_val;
}


/*  ****************** Debut des fonctions externes  *************************  */

/* L'allocation des Aij est separee de l'allocation de la foret partagee
   elle-meme c,a permet (cas earley) de pre'-calculer les Aij avant
   de calculer les productions instanciees */
void
spf_allocate_Aij (int sxmaxnt, int sxeof)
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
spf_reallocate_lhs_rhs (int lhs_size, int rhs_size)
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
spf_allocate_spf (int sxmaxprod)
{
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
  Aij2lhs_nb = (int*) sxcalloc ((unsigned long int)XxYxZ_size (spf.outputG.Axixj2Aij_hd)+1, sizeof (int));
  spf.outputG.Aij2rhs_nb = (int*) sxcalloc ((unsigned long int)XxYxZ_size (spf.outputG.Axixj2Aij_hd)+1, sizeof (int));

  spf.outputG.maxt = 0;
  spf.outputG.is_proper = TRUE;
}

void
spf_allocate_insideG (struct insideG *insideG_ptr, BOOLEAN has_lex2, BOOLEAN has_set_automaton)
{ 
  insideG_ptr->is_allocated = TRUE;

  insideG_ptr->lispro = (int*) sxalloc (insideG_ptr->maxitem+1, sizeof (int));
  insideG_ptr->prolis = (int*) sxalloc (insideG_ptr->maxitem+1, sizeof (int));

  insideG_ptr->prolon = (int*) sxalloc (insideG_ptr->maxprod+2, sizeof (int));
  insideG_ptr->lhs = (int*) sxalloc (insideG_ptr->maxprod+1, sizeof (int));

  /* faut-il renommer les t et nt ?? */
  insideG_ptr->t2init_t = (int*) sxalloc (-insideG_ptr->maxt+1, sizeof (int)), insideG_ptr->t2init_t [0] = 0;
  insideG_ptr->nt2init_nt = (int*) sxalloc (insideG_ptr->maxnt+1, sizeof (int)), insideG_ptr->nt2init_nt [0] = 0;
  insideG_ptr->prod2init_prod = (int*) sxalloc (insideG_ptr->maxprod+1, sizeof (int)), insideG_ptr->prod2init_prod [0] = 0;

  /* Le super-axiome 0 est defini par la super-production 0 */
  insideG_ptr->npg = (int*) sxalloc (insideG_ptr->maxnt+2, sizeof (int)), insideG_ptr->npg [0] = 0, insideG_ptr->npg [1] = 1;/* Pas d'occur du "super axiome" en rhs *//* Debut de la tranche de l'axiome */
  insideG_ptr->numpg = (int*) sxalloc (insideG_ptr->maxprod+2, sizeof (int)), insideG_ptr->numpg [0] = insideG_ptr->numpg [insideG_ptr->maxprod+1] = 0;
  insideG_ptr->npd = (int*) sxalloc (insideG_ptr->maxnt+2, sizeof (int)), insideG_ptr->npd [0] = 0, insideG_ptr->npd [1] = 1;/* Pas d'occur du "super axiome" en rhs *//* Debut de la tranche de l'axiome */
  insideG_ptr->numpd = (int*) sxalloc (insideG_ptr->sizeofnumpd, sizeof (int)), insideG_ptr->numpd [0] = 0;
  insideG_ptr->tpd = (int*) sxalloc (-insideG_ptr->maxt+2, sizeof (int)), insideG_ptr->tpd [0] = insideG_ptr->tpd [1] = 1; /* Init + Assure qu'il n'y a pas d'occur du terminal 0 en rhs */
  insideG_ptr->tnumpd = (int*) sxalloc (insideG_ptr->sizeoftnumpd, sizeof (int)), insideG_ptr->tnumpd [0] = 0;

  insideG_ptr->rhs_nt2where = (int*) sxcalloc ((unsigned long int)insideG_ptr->maxnt+1, sizeof (int));
  insideG_ptr->lhs_nt2where = (int*) sxcalloc ((unsigned long int)insideG_ptr->maxnt+1, sizeof (int));

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
  }
}

void
spf_finalize ()
{
  int              Aij, Pij, size;
  struct lhs       *plhs;

  /* On laisse les parseurs gerer directement
     spf.outputG.start_symbol
     spf.outputG.maxnt
     spf.outputG.maxxnt
     spf.outputG.maxxprod
     ... et tout ce qui a trait au traitement des erreurs
  */
  size = spf.outputG.maxxprod+spf.outputG.maxxnt;

  if (size > outputG_lhs_size) {
    outputG_lhs_size = size;
    spf.outputG.lhs = (struct lhs*) sxrealloc (spf.outputG.lhs, outputG_lhs_size+2, sizeof (struct lhs));
  }

  size = spf.outputG.maxitem+spf.outputG.maxxprod+spf.outputG.maxxnt;

  if (size > outputG_rhs_size) {
    outputG_rhs_size = size;
    spf.outputG.rhs = (struct rhs*) sxrealloc (spf.outputG.rhs, outputG_rhs_size+2, sizeof (struct rhs));
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
}

void
spf_free ()
{
  if (XxYxZ_is_allocated (spf.outputG.Axixj2Aij_hd)) {
    /* Cas is_parse_forest */
    XxYxZ_free (&spf.outputG.Axixj2Aij_hd);
    XxYxZ_free (&spf.outputG.Txixj2Tij_hd);
    if (spf.outputG.Tpq_rcvr_set) sxfree (spf.outputG.Tpq_rcvr_set), spf.outputG.Tpq_rcvr_set = NULL;
    if (spf.outputG.Tij2tok_no) sxfree (spf.outputG.Tij2tok_no), spf.outputG.Tij2tok_no = NULL;
    if (spf.outputG.Pij2eval) sxfree (spf.outputG.Pij2eval), spf.outputG.Pij2eval = NULL;
    if (spf.outputG.Tij2comment) sxfree (spf.outputG.Tij2comment), spf.outputG.Tij2comment = NULL;
    if (spf.outputG.non_productive_Aij_rcvr_set) sxfree (spf.outputG.non_productive_Aij_rcvr_set), spf.outputG.non_productive_Aij_rcvr_set = NULL;
    if (spf.outputG.xPij2lgth) sxfree (spf.outputG.xPij2lgth), spf.outputG.xPij2lgth = NULL;
  }

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

  if (spf.walk.reachable_Pij_set) sxfree (spf.walk.reachable_Pij_set), spf.walk.reachable_Pij_set = NULL;
  if (spf.walk.Pij2hook) sxfree (spf.walk.Pij2hook), spf.walk.Pij2hook = NULL;
  if (spf.walk.status) sxfree (spf.walk.status), spf.walk.status = NULL;
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
spf_put_an_iprod (int *stack)
{
  int              prod, lgth, Aij, Xpq;
  int              *top_stack, *bot_stack;
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


void
spf_print_Xpq (out_file, Xpq)
     FILE  *out_file;
     int   Xpq;
{
  int  ste, X, p, q;
  char *t;

  if (Xpq < 0) {
    Xpq = -Xpq;

    X = -Tij2T (Xpq);
    p = Tij2i (Xpq);
    q = Tij2j (Xpq);

    t = spf.inputG.tstring [X];

    if (spf.outputG.Tij2tok_no == NULL)
      /* Non encore affecte' */
      ste = ERROR_STE;
    else
      ste = parser_Tpq2tok (Xpq)->string_table_entry;

    if (ste <= EMPTY_STE)
      fprintf (out_file, "\"%s\"[", t);
    else
      fprintf (out_file, "\"%s/%s\"[", sxstrget (ste), t);

    if (p <= 0) {
      if (p == 0)
	fputs ("*", out_file);
      else
	fprintf (out_file, "%i", -p);
    }
    else
      fprintf (out_file, "%i", p);

    if (spf.outputG.Tpq_rcvr_set && SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Xpq))
      fputs ("::", out_file);
    else
      fputs ("..", out_file);

    if (q <= 0) {
      if (q == 0)
	fputs ("*] ", out_file);
      else
	fprintf (out_file, "%i] ", -q);
    }
    else
      fprintf (out_file, "%i] ", q);
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
	fprintf (out_file, "%i", -p);
    }
    else
      fprintf (out_file, "%i", p);

    if (Xpq > spf.outputG.maxnt)
      fputs ("::", out_file);
    else
      fputs ("..", out_file);

    if (q <= 0) {
      if (q == 0)
	fputs ("*]> ", out_file);
      else
	fprintf (out_file, "%i]> ", -q);
    }
    else
      fprintf (out_file, "%i]> ", q);
  }
}



BOOLEAN
spf_print_iprod (FILE *out_file, int prod, char *prefix, char *suffix)
{
  int Aij, Xpq, cur_item, i, j, p, q;

  sxinitialise(j); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(i); /* pour faire taire "gcc -Wuninitialized" */
  Aij = spf.outputG.lhs [prod].lhs;

  if (spf.outputG.is_error_detected) {
    i = Aij2i (Aij);
    j = Aij2j (Aij);
  }

  fprintf (out_file, "%s%i/%i:", prefix, prod, spf.outputG.lhs [prod].init_prod);
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
	fprintf (out_file, "[%i::%i] ", i, p);

      if (q < 0)
	q = -q;

      if (q > 0)
	i = q;
    }

    spf_print_Xpq (out_file, Xpq);
  }

  if (spf.outputG.is_error_detected && i != j)
    fprintf (out_file, "[%i::%i] ", i, j);

  fprintf (out_file, ";%s", suffix);
    
  return TRUE; /* Ca peut etre utilise comme de la semantique */
}


/* Extrait de spf le p ieme sous-arbre enracine' en Aij
   (et appelle la fonction utilisateur f) */
int
spf_dag2tree (unsigned long int p, int Aij, int (*f)())
{
  unsigned long int     prev_tree_nb, Xpq_tree_nb;
  int                   item, prod, Xpq, bot_hook, left_hook, right_hook, mid_hook;

  prev_tree_nb = 0L;

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
      Xpq_tree_nb = spf.tree_count.nt2nb [Xpq];

      if (!spf_dag2tree (p%Xpq_tree_nb, Xpq, f))
	return FALSE;

      p /= Xpq_tree_nb;
    }
  }

  /* On vient de parcourir le "bon" sous-arbre de prod */
  return (*f) (prod); /* Appel de la semantique, elle peut ranger ses infos en // avec les Aij... */
}


/* Imprime sur out_file la [sous-]foret de racine Aij a partir de Aij vers les feuilles */
void
spf_print_td_forest (out_file, Aij)
     FILE    *out_file;
     int     Aij;
{
  spf_print_forest_out_file = out_file;
  spf_topological_walk (Aij, spf_print_td_or_bu_prod, NULL);
}

/* Imprime sur out_file la [sous-]foret de racine Aij des feuilles vers Aij */
void
spf_print_bu_forest (out_file, Aij)
     FILE    *out_file;
     int     Aij;
{
  spf_print_forest_out_file = out_file;
  spf_topological_walk (Aij, NULL, spf_print_td_or_bu_prod);
}

int
spf_unfold ()
{
  double            double_tree_count;
  unsigned long int tree_count, tree_nb;

  if (spf_tree_count (&double_tree_count)) {
    tree_count = (unsigned long int) double_tree_count;

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

#if 0
  /* Ds spf_tree_count */
  spf_topological_walk (spf.outputG.start_symbol, count_td_walk, NULL);
#endif /* 0 */

  spf_tree_count (&tree_count);

  fprintf (out_file, "%s(|N|=%i, |T|=%i, |P|=%i, S=%i, |outpuG|=%i, TreeCount=", 
	  prefix_str, sxba_cardinal (count_Aij_set), sxba_cardinal (count_Tij_set), count_maxprod, spf.outputG.start_symbol, count_maxitem);

  if (tree_count < 1.0E9)
    fprintf (out_file, "%.f)\n", tree_count);
  else
    fprintf (out_file, "%e)\n", tree_count);

  sxfree (count_Tij_set), count_Tij_set = NULL;
  sxfree (count_Aij_set), count_Aij_set = NULL;

  return tree_count;
}



/* Ds dag_scanner.c si is_dag ou ds sxearley_main.c sinon */
extern void fill_Tij2tok_no (int Tpq, int maxTpq);
/* Appele' depuis la semantique si elle a besoin de cette correspondance */
/* Doit etre appele' (si besoin est) apres distribute_comment qui positionne dag2comment */
void
spf_fill_Tij2tok_no ()
{
  int  Tpq, maxt;

  if (spf.outputG.Tij2tok_no) 
    /* Cas d'un appel precedent depuis get_[s]dag_sentence_str () */
    /* On recommence car il y'a pu y avoir un appel a distribute_comment entre temps */
    sxfree (spf.outputG.Tij2tok_no);

  maxt = -spf.outputG.maxt;

  spf.outputG.Tij2tok_no = (int *) sxalloc (maxt+1, sizeof (int));
  spf.outputG.Tij2tok_no [0] = 0;

#if LOG
  fputs ("\n**** spf_fill_Tij2tok_no () ****\n", stdout);
#endif /* LOG */

  for (Tpq = 1; Tpq <= maxt; Tpq++) {
#if LOG
    printf ("Tij=%i, ", Tpq);
    spf_print_Xpq (stdout, -Tpq);
#endif /* LOG */

    if (spf.outputG.Tpq_rcvr_set == NULL || !SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tpq)) {
      /* T est un terminal */
      fill_Tij2tok_no (Tpq, maxt);
    }
  }

#if LOG
  fputs ("**** End of spf_fill_Tij2tok_no () ****\n\n", stdout);
#endif /* LOG */
}


/* Ca permet d'eviter que la compil de l'utilisateur de'pende de is_dag */
struct sxtoken*
spf_Tpq2tok (Tpq)
     int Tpq;
{
  return parser_Tpq2tok (Tpq);
}



/* appele depuis nbest, lfg_semact ... */
/* Si Pij = X1 ... Xp et
   si p   = Y1 ... Yp est la production de la CFG squelette et
   si Z1 ... Zk, Zj = Yi, peut deriver ds le vide soit
      T1 ... Tk, Tj = Xi la chaine associee construite sur la rhs de Pij qui ne contient que des nt
      instancies dont la base peut deriver ds le vide.  A cette chaine on associe le nombre binaire
      eval = E1 ... Ek, tq Ej=1 <=> Xi ne derive pas dans le vide */
void
spf_fill_Pij2eval ()
{
  int Pij, item, val, Xpq, X, p, q;

  if (spf.outputG.Pij2eval) return; /* deja rempli ... */

  /* alloc */
  spf.outputG.Pij2eval = (int*) sxalloc (spf.outputG.maxxprod+1, sizeof (int));
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



BOOLEAN
spf_tree_count (double *count)
{
  int i;

  /* A partir du 13/01/06 Les cycles eventuels ds la foret ont ete elimines */
  if (spf.outputG.start_symbol == 0 /* || spf.inputG.has_cycles */) {
    *count = 0L;
    return FALSE;
  }

  if (spf.tree_count.prod2nb == NULL) {
    spf.tree_count.prod2nb = (double *) sxcalloc ((unsigned long int)spf.outputG.maxxprod+1, sizeof (double));
    spf.tree_count.nt2nb = (double *) sxcalloc ((unsigned long int)spf.walk.Aij_top+1, sizeof (double));
  }
  else {
    for (i = spf.outputG.maxxprod; i >= 0; i--)
      spf.tree_count.prod2nb [i] = 0L;

    for (i = spf.walk.Aij_top; i >= 0; i--)
      spf.tree_count.nt2nb [i] = 0L;
  }

  spf_topological_walk (spf.outputG.start_symbol, NULL, spf_tree_count_pass_derived);

  *count = spf.tree_count.nt2nb [spf.outputG.start_symbol];

  return TRUE;
}

/* Nelle version */
/* Nelle version 04/08/06 : on peut ne traiter qu'une sous-foret */
/* PB : Les parties de la foret non visitees a cause de user_filtering_function ne doivent pas etre supprimees !! (voir la solution) */
/* Elle peut etre appelee par l'utilisateur */
/* Elle "desactive" toutes les productions inaccessibles (grammaire pas propre et/ou qq Pij mis en negatif)
   et recalcule Aij2rhs_nb pour les passes heritees eventuelles */
BOOLEAN
spf_make_proper (int root_Aij)
{
  int        Aij, hook, Pij, root_Aij_rhs_nb;
#if 0
  int prod;
#endif
  struct rhs *prhs, *top_prhs;
  BOOLEAN    ret_val;
  int        (*spf_make_proper_user_filtering_function) ();
  static BOOLEAN called_from_spf_make_proper;

  if (called_from_spf_make_proper)
    /* appel recursif !! */
    return TRUE;

  called_from_spf_make_proper = TRUE;

#if LOG
  fputs ("*** Entering spf_make_proper ()\n", stdout);
#endif /* LOG */

  /* Petite traversee bu au cours de laquelle on note les prods accessibles et on recalcule Aij2rhs_nb */
  if (spf.walk.reachable_Pij_set == NULL) {
    spf.walk.reachable_Pij_set = sxba_calloc (spf.outputG.maxxprod+1);
    /* Ajoute' le 21/02/07 */
    spf.walk.Pij2hook = (int*) sxalloc (spf.outputG.maxxprod+1, sizeof (int));

    top_prhs = spf.outputG.rhs+spf.outputG.lhs [spf.outputG.maxxprod+spf.outputG.maxxnt+1].prolon;
    hook = spf.outputG.lhs [spf.outputG.maxxprod+1].prolon;
    prhs = spf.outputG.rhs+hook;

    while (prhs < top_prhs) {
      Pij = prhs->lispro;

      if (Pij != 0) {
	if (Pij< 0)
	  Pij = -Pij;

	spf.walk.Pij2hook [Pij] = hook;
      } 

      prhs++;
      hook++;
    }
  }
  else
    sxba_empty (spf.walk.reachable_Pij_set);

#if 0
  /* ancienne version */
  for (Aij = 1; Aij <= spf.walk.Aij_top; Aij++)
    spf.outputG.Aij2rhs_nb [Aij] = 0;

  ret_val = spf_topological_walk (spf.outputG.start_symbol, NULL, spf_bu_reachable);
#endif /* 0 */

  root_Aij_rhs_nb = spf.outputG.Aij2rhs_nb [root_Aij];
  /* On inhibe la user_filtering_function eventuelle pour ne pas supprimer les Pij dont elle a interdit la visite */
  spf_make_proper_user_filtering_function = spf.walk.user_filtering_function;
  spf.walk.user_filtering_function = NULL;

  for (Aij = 1; Aij <= spf.walk.Aij_top; Aij++)
    spf.outputG.Aij2rhs_nb [Aij] = 0;

  ret_val = spf_topological_bottom_up_walk (root_Aij, spf_bu_reachable, NULL, NULL);

#if 0
  /* anciennemnt nouvelle version, mais �a chie */
  ret_val = spf_topological_bottom_up_walk (root_Aij, spf_bu_reachable, spf_pre_bu_reachable, NULL);
#endif /* 0 */
  
  spf.walk.user_filtering_function = spf_make_proper_user_filtering_function;
  spf.outputG.Aij2rhs_nb [root_Aij] = root_Aij_rhs_nb;

  ret_val = sxba_scan (spf.walk.reachable_Pij_set, 0) > 0;
  /* au moins 1 Pij d'accessible */

  Pij = 0;

  while ((Pij = sxba_0_lrscan (spf.walk.reachable_Pij_set, Pij)) > 0) {
    spf.outputG.rhs [spf.walk.Pij2hook [Pij]].lispro = -Pij; /* Il peut deja etre negatif, tant pis */
  }

#if 0
  /* tres tres couteux !!  Remplace' le 21/02/07 par le bloc ci-dessus */
  Pij = 0;

  while ((Pij = sxba_0_lrscan (spf.walk.reachable_Pij_set, Pij)) > 0) {
    /* Pij est inaccessible */
    Aij = spf.outputG.lhs [Pij].lhs;
    hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;
    prhs = spf.outputG.rhs+hook;

    while ((prod = prhs->lispro) != 0) {
      /* On en profite pour traiter toutes les Aij-prods */
      if (prod > 0 && SXBA_bit_is_reset_set (spf.walk.reachable_Pij_set, prod) /* pour le sxba_0_lrscan */)
	/* prod devient inacessible */
	prhs->lispro = -prod;

      prhs++;
    }
  }
#endif /* 0 */

  /* A faire ici (et non pas avant l'appel a spf_topological_bottom_up_walk) car
     si la grammaire est cyclique, spf.outputG.is_proper a ete repositionne' a faux */
  spf.outputG.is_proper = TRUE;

#if LOG
  fputs ("*** Leaving spf_make_proper()\n", stdout);
  fputs ("*** Entering spf_make_proper () checking\n", stdout);
#if EBUG
  /* On verifie la coherence de la nelle foret */
  spf_make_proper_check_td_set = sxba_calloc (spf.outputG.maxxprod+1);
  spf_make_proper_check_bu_set = sxba_calloc (spf.outputG.maxxprod+1);
#endif /* EBUG */

#if 0
  spf_topological_walk (spf.outputG.start_symbol, spf_make_proper_check_td, NULL);
  spf_topological_walk (spf.outputG.start_symbol, NULL, spf_make_proper_check_bu);
#endif /* 0 */

  spf_topological_walk (root_Aij, spf_make_proper_check_td, NULL);
  spf_topological_walk (root_Aij, NULL, spf_make_proper_check_bu);

#if EBUG
  /* On verifie la coherence de la nelle foret */
  if ((Pij = sxba_first_difference (spf_make_proper_check_td_set, spf_make_proper_check_bu_set)) != -1)
    sxtrap (ME, "spf_make_proper");

  sxfree (spf_make_proper_check_td_set), spf_make_proper_check_td_set = NULL;
  sxfree (spf_make_proper_check_bu_set), spf_make_proper_check_bu_set = NULL;
#endif /* EBUG */

  fputs ("*** Leaving spf_make_proper () checking\n", stdout);

  if (!ret_val)
    fputs ("*** WARNING: the whole forest has vanished!! ***\n", stdout);
#endif /* LOG */

  called_from_spf_make_proper = FALSE;

  return ret_val;
}


SXBA
spf_save_status (SXBA save_set)
{
  if (save_set == NULL)
    /* C'est l'utilisateur qui se charge du free */
    save_set = sxba_calloc (spf.outputG.maxxprod+1);

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
spf_push_status ()
{
  int        Aij, Pij;
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
spf_pop_status ()
{
  int        Aij, Pij, item, Xpq;
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
}



BOOLEAN
spf_topological_walk (int root, int (*pi)(), int (*pd)())
{
  BOOLEAN ret_val;

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



BOOLEAN
spf_topological_top_down_walk (int root, int (*pi)(), int (*pre_pass)(), int (*post_pass)())
{
  BOOLEAN ret_val;

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

BOOLEAN
spf_topological_bottom_up_walk (int root, int (*pd)(), int (*pre_pass)(), int (*post_pass)())
{
  BOOLEAN ret_val;

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
BOOLEAN
spf_erase (SXBA Pij_set)
{
  int     Pij, prod, Aij, hook;
  BOOLEAN ret_val;

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
	spf.outputG.is_proper = FALSE;
	spf.outputG.rhs [hook].lispro = -prod;
      }

      hook++;
    }
  }

  if (!spf.outputG.is_proper)
    ret_val = spf_make_proper (spf.outputG.start_symbol /* global */);
  else
    ret_val = TRUE;

#if LOG
  fputs ("*** Leaving spf_erase ***\n", stdout);
#endif /* LOG */

  return ret_val;
}


int
spf_print_tree_count ()
{
  double val;

  /* Le 14/09/06 j'utilise spf_print_signature car la spf a pu etre
     reduite (cycle, erreurs, ...) */
  val = spf_print_signature (sxstdout, "Shared Parse Forest = ");
    
  if (is_print_time)
    sxtime (TIME_FINAL, "\tPrint Tree Count");

#if 0
  /* Le 13/01/06, j'utilise spf_tree_count */
  spf_tree_count (&val);

  fprintf (sxstdout, "(|N|=%i, |T|=%i, |P|=%i, S=%i, |outpuG|=%i, TreeCount=", 
	   spf.outputG.maxxnt, -spf.outputG.maxt, spf.outputG.maxxprod, spf.outputG.start_symbol, spf.outputG.maxitem);

  if (val < 1.0E9)
    fprintf (sxstdout, "%.f)\n", val);
  else
    fprintf (sxstdout, "%e)\n", val);
#endif /* 0 */

  return 1;
}
