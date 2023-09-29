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
#ifndef sxrcg_spf_h
#define sxrcg_spf_h

#ifndef SX_GLOBAL_VAR_RCG3
#  ifdef SX_DFN_EXT_VAR_RCG3
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#    define SX_GLOBAL_VAR_RCG3   /* rien */
#    define SX_INIT_VAL_RCG3(v)  = v
#    define SX_DFN_EXT_VAR_RCG3
#  else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#    define SX_GLOBAL_VAR_RCG3   extern
#    define SX_INIT_VAL_RCG3(v)  /*rien*/
#  endif /* #ifndef SX_DFN_EXT_VAR_RCG3 */
#endif /* #ifndef SX_GLOBAL_VAR_RCG3 */



#include "SXante.h"


#include "XH.h"
#include "XxY.h"

struct spf_node {
  struct spf_node   *next;
  struct G          *G;
  SXINT             instantiated_clause_nb; /* Identifiant global des clauses instanciees */
  SXINT             wcell; /* cellule de travail pour les "topological walk" */    
  SXINT		    rho [1];
};

typedef struct {
    struct spf_node	*bot_node_ptr, *top_node_ptr;
} SEM_TYPE;

struct area_block {
    SXINT		size;
    struct area_block	*next;
    struct spf_node	node [1];
};

struct area_hd {
    struct area_block	*empty, *bot;
    struct spf_node	*top;
    SXINT		room, size;
};

struct rcg_spf /* shared_parse_forest */ {
  SXINT          start_symbol, max_instantiated_clause_nb, cur_max_instantiated_clause_nb /* apres make_proper */, max_instantiated_prdct_nb;
  SXBOOLEAN      is_proper;
  SXINT          *Aij2rhs_nb; /* Pour les passes heritees */

  SXINT          cur_AijXxxx_hd_size; /* pour stocker la taille courante de AijXpid_hd (si PID > 1) et/ou d'autres structures du même genre, pour que la sémantique sache quand elle doit réallouer ses structures indexées sur des Aij_pid (qui sont des Aij si PID == 1) */
  XxY_header     AijXpid_hd; /* Identifie de facon unique (cas PID > 1) les predicats instancies */

  SEM_TYPE	 *sem_disp; /* Indexe' par des Aij_pid */

  double	*tree_count_disp; /* Si option -parse_tree_number (-ptn) */

  struct tree_count {
    double *iclause2nb, *nt2nb;
  } tree_count; /* Si appel de rcg_spf_set_tree_count */

  /* Variables de rcg_spf_topological_walk () */
  struct walk {
    SXINT         *Aij2nyp_nb /* variable locale */
    , *Aij_stack /* variable locale */
      ;

    struct spf_node **iclause2spf_node; 

    SXBA        Aij_set /* variable locale */
    , cyclic_Aij_set /* variable locale */
      , invalid_Aij_set /* Contient les Aij invalides */
      , status /* Contient le resultat de rcg_spf_save_status */
      ; 

    SXINT 	(*pass_inherited) (struct spf_node *) /* variable locale */
	 , (*pass_derived) (struct spf_node *) /* variable locale */
	 , (*pre_pass) (SXINT) /* variable locale */
	 , (*post_pass) (SXINT, SXBOOLEAN) /* variable locale */
	 , (*user_filtering_function) (struct spf_node *) /* filtre supplementaire utilisateur sur les Pij */
	 ;
  } walk;

  struct fun_calls semact;
};

#ifndef SX_DFN_EXT_VAR_RCG3
extern struct rcg_spf rcg_spf;
#else /* SX_DFN_EXT_VAR_RCG3 */
/* Cas de la definition et de l'initialisation de la structure externe rcg_spf
 a priori ds RCG_parser.c */
struct rcg_spf rcg_spf;
#endif /* SX_DFN_EXT_VAR_RCG3 */

static SXINT _Aij_pid;
#define MAKE_Aij_pid(Aij,pid)           (rcg_spf.AijXpid_hd == NULL ? Aij : (XxY_set (&rcg_spf.AijXpid_hd, Aij, pid, &_Aij_pid), _Aij_pid))

#define Aij_pid2Aij(Aij_pid)            (rcg_spf.AijXpid_hd == NULL ? Aij_pid : (XxY_X (rcg_spf.AijXpid_hd, Aij_pid)))
#define Aij_pid2pid(Aij_pid)            (rcg_spf.AijXpid_hd == NULL ? 0 : (XxY_Y (rcg_spf.AijXpid_hd, Aij_pid)))

#define GET_RCG_SPF_HEAD_NODE_PTR(p)	        (rcg_spf.sem_disp[p].bot_node_ptr)
#define GET_RCG_SPF_TAIL_NODE_PTR(p)	        (rcg_spf.sem_disp[p].top_node_ptr)
#define GET_RCG_SPF_ERASED_HEAD_NODE_PTR(p)	(rcg_spf.sem_disp[p].top_node_ptr)
	
#if 0
#define GET_SEM_NODE_PTR(p)		(rcg_spf.sem_disp[p].bot_node_ptr)
#define GET_SEM_TOP_NODE_PTR(p)	        (rcg_spf.sem_disp[p].top_node_ptr)
#endif /* 0 */

#ifndef no_sxrcg_spf_function_declarations
/* Construction de la rcg_spf */

/* Les parcours de la rcg_spf */
/* Ds le cas de la visite heritee (*pi)(Pij), si Pij est une Aij-iprod,
   on est su^r que :
      toutes les productions qui contiennent Aij en rhs ont deja ete appelees
      toutes les Aij-iprods sont appelees en sequence : si l'appel suivant est de
      la forme (*pi)(P'ij), alors soit P'ij est une Aij-iprod ou, si ce n'est
      pas le cas plus aucun des appels ulterieurs ne se fera avec une Aij-iprod */
/* Ds le cas de la visite synthetisee (*pd)(Pij), si Pij est une Aij-iprod,
   on est su^r que si Xpq est en rhs:
      toutes les Xpq-iprods ont deja ete appelees
      toutes les Aij-iprods sont appelees en sequence */
/* Attention soit pi, soit pd est non NULL (on ne peut par parcourir la foret haut-bas
   gauche-droit avec les contraintes precedentes) */
SX_GLOBAL_VAR_RCG3 SXBOOLEAN        rcg_spf_topological_walk (SXINT root,
							      SXINT (*pi)(struct spf_node *),
							      SXINT (*pd)(struct spf_node *)
							      );
/* Permettent des pre et/ou des post visites sur les noeuds Aij des Aij-iprods */
SX_GLOBAL_VAR_RCG3 SXBOOLEAN        rcg_spf_topological_top_down_walk (SXINT root,
								       SXINT (*pi)(struct spf_node *),
								       SXINT (*pre_pass)(SXINT),
								       SXINT (*post_pass)(SXINT, SXBOOLEAN)
								       );
SX_GLOBAL_VAR_RCG3 SXBOOLEAN        rcg_spf_topological_bottom_up_walk (SXINT root,
									SXINT (*pd)(struct spf_node *),
									SXINT (*pre_pass)(SXINT),
									SXINT (*post_pass)(SXINT, SXBOOLEAN)
									);

/* Elimine les iclauses inaccessibles dues aux valeurs -1 retournees */
SX_GLOBAL_VAR_RCG3 SXBOOLEAN        rcg_spf_make_proper (SXINT root_Aij);


/* Ecritures */
/* Ecriture d'un symbole instancie */
SX_GLOBAL_VAR_RCG3 void             rcg_spf_print_Xpq (FILE *out_file,
						       SXINT Xpq
						       );
/* Ecriture d'un bout d'une clause instanciee */
SX_GLOBAL_VAR_RCG3 SXBOOLEAN        rcg_spf_print_partial_instantiated_clause (FILE *out_file,
									       struct G *G,
									       SXINT *rho,
									       SXBOOLEAN is_G_name,
									       SXBOOLEAN is_clause_nb,
									       SXBOOLEAN is_lhs,
									       SXBOOLEAN is_rhs,
									       char *suffix
									       );

/* Ecriture d'une clause instanciee */
SX_GLOBAL_VAR_RCG3 SXBOOLEAN        rcg_spf_print_total_instantiated_clause (FILE *out_file,
									     struct G *G,
									     SXINT *rho,
									     char *suffix
									     );

/* Ecriture d'une clause instanciee avec '\nl' ds out_file */
SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_print_instantiated_clause_in_outfile (FILE *out_file,
										  struct G *G,
										  SXINT *rho);


/* Ecriture d'une clause instanciee avec '\nl' ds stdout */
SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_print_instantiated_clause (struct G *G,
								       SXINT *rho
								       );

SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_print_spf_node (struct spf_node *cur_rcg_spf_node_ptr);

/* Ecriture en top_down de la sous-foret ancree sur Aij */
SX_GLOBAL_VAR_RCG3 void             rcg_spf_print_td_forest (FILE *out_file,
							     SXINT Aij
							     ); /* ... de Aij vers les feuilles */
/* Ecriture en bottom-up de la sous-foret ancree sur Aij */
SX_GLOBAL_VAR_RCG3 void             rcg_spf_print_bu_forest (FILE *out_file,
							     SXINT Aij
							     ); /* ... des feuilles vers Aij */

/* Gestionnaire des clauses instantiees */
/* Appele' par les _if () si la semantique dit que la 1ere passe construit une spf */
#if 0
SX_GLOBAL_VAR_RCG3 void             rcg_spf_begins (struct G *G);
#endif /* 0 */
SX_GLOBAL_VAR_RCG3 void             rcg_spf_spf_alloc (struct G *G, SXINT old_size, SXINT new_size);
SX_GLOBAL_VAR_RCG3 void             rcg_spf_spf_free (struct G *G, SXINT old_size, SXINT new_size);
SX_GLOBAL_VAR_RCG3 void             rcg_spf_store_instantiated_clause_in_spf_loop (struct G *G, SXINT *rho, SXINT cycle_kind);
SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_store_instantiated_clause_in_spf (struct G *G, SXINT *rho);
SX_GLOBAL_VAR_RCG3 void             rcg_spf_spf_completes (void);

SX_GLOBAL_VAR_RCG3 void             rcg_spf_tree_count_in_first_pass_begins (void);
SX_GLOBAL_VAR_RCG3 void             rcg_spf_tree_count_begins (void);
SX_GLOBAL_VAR_RCG3 void             rcg_spf_print_forest_begins (void);
SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_tree_count (void);
/* Pour le decompte des arbres en // avec analyse syntaxique */
SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_tree_count_action (struct G *G, SXINT *rho);

/* Elimine de la foret les noeuds pointes par les elem de la pile */
SX_GLOBAL_VAR_RCG3 SXBOOLEAN        rcg_spf_erase (struct spf_node **spf_node_stack);

/* Positionne ds count (et pour chaque Aij ds rcg_spf.tree_count.nt2nb [Aij]) le
   nombre d'arbres de la [sous-]foret partagee [de racine Aij] */
SX_GLOBAL_VAR_RCG3 SXBOOLEAN        rcg_spf_set_tree_count (double *count);
/* Ecriture de la "taile" courante de la rcg_spf (dont le nombre d'arbres) */
SX_GLOBAL_VAR_RCG3 double           rcg_spf_print_signature (FILE *out_file); /* sort la taille actuelle de la foret et retourne le nb d'arbres */
/* Appelle rcg_spf_print_signature mais peut etre utilise' comme valeur de for_semact.sem_pass */
SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_print_tree_count (void);

/* sortie du yield de la spf sous forme de [U]DAG */
SX_GLOBAL_VAR_RCG3 void             rcg_spf_dag_yield_alloc (SXBOOLEAN is_spf_already_built, SXBOOLEAN is_td_pass_needed);
SX_GLOBAL_VAR_RCG3 void             rcg_spf_dag_yield (void);
SX_GLOBAL_VAR_RCG3 void             rcg_spf_dag_yield_free (void);
SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_process_dag_yield (struct G *G, SXINT *rho, SXBOOLEAN unconditional);
SX_GLOBAL_VAR_RCG3 SXBOOLEAN        rcg_spf_nl_pass_bu_do_it (void);
SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_nl_pass_td_do_it (void);


/* Deplie la foret */
SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_unfold (void);
SX_GLOBAL_VAR_RCG3 SXINT            rcg_spf_dag2tree (SXUINT p, SXINT Aij, SXINT (*f)(struct spf_node *));


/* Permet de revenir a un etat precedant de la foret */
SX_GLOBAL_VAR_RCG3 void             rcg_spf_push_status (void); /* calcule l'etat courant de la rcg_spf et le sauve ds une SXBA locale (rcg_spf.walk.status) */
SX_GLOBAL_VAR_RCG3 void             rcg_spf_pop_status (void); /* Utilise rcg_spf.walk.status pour remettre la rcg_spf dans cet etat */
SX_GLOBAL_VAR_RCG3 void             rcg_spf_restore_status (SXBA save_set); /* copie save_set ds rcg_spf.walk.status */
SX_GLOBAL_VAR_RCG3 SXBA             rcg_spf_save_status (SXBA save_set) /* copie rcg_spf.walk.status dans le SXBA utilisateur save_set (qui peut etre NULL) */;

#endif /* no_sxrcg_spf_function_declarations */
#endif /* sxrcg_spf_h */
