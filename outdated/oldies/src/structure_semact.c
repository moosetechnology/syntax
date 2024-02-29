/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2005 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */

/* Actions sémantiques permettant le filtrage de la forêt sur des critères structurels */


/* L'analyseur est de type Earley */
/* Le source est sous forme de SDAG */

/* ********************************************************
   *                                                      *
   *          Produit de l'equipe ATOLL.                  *
   *                                                      *
   ******************************************************** */

static char	ME [] = "structure_semact";

#include "sxunix.h"
#include "earley.h"
#include "dag_scanner.h"
#include "sxword.h"
#include "sxstack.h"
#include "varstr.h"
#include "XH.h"

char WHAT_STRUCTURE_SEMACT[] = "@(#)SYNTAX - $Id: structure_semact.c 3678 2024-02-06 08:38:24Z garavel $" WHAT_DEBUG;

static char	Usage [] = "";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	   0

static char	*option_tbl [] = {
    "",
};

static int	option_kind [] = {
    UNKNOWN_ARG,
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


/* retourne le ME */
static char*
output_ME ()
{
  return ME;
}

/* retourne la chaine des arguments possibles propres a structure */
static char*
structure_args_usage ()
{
  return Usage;
}

/* decode les arguments specifiques a structure */
/* l'option argv [*parg_num] est inconnue du parseur earley */
static bool
structure_args_decode (pargnum, argc, argv)
     int  *pargnum, argc;
     char *argv [];
{
  switch (option_get_kind (argv [*pargnum])) {

  case UNKNOWN_ARG:
    return false;
  }

  return true;
}





static int               maxxt; /* nb des terminaux instancies Tij, y compris ceux couverts par les nt de la rcvr == -spf.outputG.maxt */

static double *Pij2weight,*Xpq2weight;
static double *Tij2highest_prio;
static double        *spf_count;

static char    *priority_vector;
static char    priority_vector_default [] = "1";
static int     priority_vector_lgth;

static void (*specific_contribution)();
static int  tolerance_level;


bool       STRUCTURE_SEM_PASS_ARG;

static void
allocate_all ()
{
  Pij2weight = (double*) sxcalloc (spf.outputG.maxxprod+1, sizeof (double));
  Xpq2weight = (double*) sxcalloc (spf.outputG.maxxnt+1, sizeof (double));
  Tij2highest_prio = (double*) sxcalloc (-spf.outputG.maxt+1, sizeof (double));
}

static void
free_all ()
{  
  sxfree (Pij2weight), Pij2weight = NULL;
  sxfree (Xpq2weight), Xpq2weight = NULL;
  sxfree (Tij2highest_prio), Tij2highest_prio = NULL;
}

static void
structure_sem_final()
{
  free_all();
}

/* calcule récursivement le nombre de sous-arbres de même racine */
static double
spf_ptn_count (Aij)
     int Aij;
{
  int              output_prod, output_item, Xpq, hook;
  double           val, Aij_val;

  if ((Aij_val = spf_count [Aij]) == -1.0L) {
    /* 1ere visite de Aij => non calcule */
    Aij_val = spf_count [Aij] = 0;
    
    hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;
    
    while ((output_prod = spf.outputG.rhs [hook++].lispro) != 0) {
      /* On parcourt les productions dont la lhs est Aij */
      if (output_prod > 0) { /* et qui ne sont pas filtrées */
	output_item = spf.outputG.lhs [output_prod].prolon;
	
	val = 1.0L;
	
	while ((Xpq = spf.outputG.rhs [output_item++].lispro) != 0) {
	  /* On parcourt la RHS de la  prod instanciee output_prod */
	  if (Xpq > 0 && Xpq <= spf.outputG.maxxnt) { /* y compris ceux en erreur */
	    /* nt */
	    val *= spf_ptn_count (Xpq);
	  }
	}
	Aij_val += val;
      }
    }
    spf_count [Aij] = Aij_val; /* Aij est entierement calcule' (et non cyclique) */
  }

  return Aij_val;
}

/* calcule le nombre d'arbres de la forêt */
static double
spf_ptn ()
{
  double val;
  int i;

  if (spf.outputG.start_symbol == 0) {
    val = 0.0L;
  }
  else {
    spf_count = (double*) sxalloc (spf.outputG.maxxnt+1, sizeof (double));
    for (i=0;i<=spf.outputG.maxxnt;i++)
      spf_count[i]=-1.0L;

    val = spf_ptn_count (spf.outputG.start_symbol);

    sxfree (spf_count), spf_count = NULL;
  }
  return val;

}

/* imprime le nombre d'arbres de la forêt */
static void
print_ptn ()
{
  double val=spf_ptn();

  /*  fprintf (sxstdout, "|N|=%i, |T|=%i, |P|=%i, S=%i, |outpuG|=%i, TreeCount=", 
      spf.outputG.maxxnt, -spf.outputG.maxt, spf.outputG.maxxprod, spf.outputG.start_symbol, spf.outputG.maxitem);*/
  fprintf(stdout, "TreeCount\t");
  
  if (val < 1.0E9)
    fprintf (stdout, "%.f\n", val);
  else
    fprintf (stdout, "%e\n", val);
}

static void
prefer_less_appos(Pij,cur_Aij,Xpq)
     int Pij, cur_Aij, Xpq;
{
  char *ntstring;
  
  if (Xpq > 0) {/* nt */
    Pij2weight[Pij] += Xpq2weight[Xpq];
    ntstring=spf.inputG.ntstring[Aij2A (Xpq)];
    if (strstr(ntstring,"appos") && Aij2j (Xpq) - Aij2i (Xpq) > 0) {
      Pij2weight[Pij] -= 1;
    }
  }
}

static void
prefer_local_COO(Pij,cur_Aij,Xpq)
     int Pij, cur_Aij, Xpq;
{
  char *ntstring;
  
  if (Xpq > 0) {/* nt */
    Pij2weight[Pij] += Xpq2weight[Xpq];
    ntstring=spf.inputG.ntstring[Aij2A (Xpq)];
    if (strncmp(ntstring,"COO",3) == 0) {
      Pij2weight[Pij] += 100+100/(Aij2j (cur_Aij) - Aij2i (cur_Aij));
    }
  }
}

static void
prefer_ADV_MOD(Pij,cur_Aij,Xpq)
     int Pij, cur_Aij, Xpq;
{
  char *ntstring;
  
  if (Xpq > 0) {/* nt */
    Pij2weight[Pij] += Xpq2weight[Xpq];
    ntstring=spf.inputG.ntstring[Aij2A (Xpq)];
    if (strncmp(ntstring,"ADV_MOD",7) == 0 &&  Aij2j (Xpq) - Aij2i (Xpq) > 0) {
      Pij2weight[Pij] += 1 ;
    }
  }
}

static void
prefer_Easy_NVA(Pij,cur_Aij,Xpq)
     int Pij, cur_Aij, Xpq;
{
  char *ntstring;
  
  if (Xpq > 0) {/* nt */
    Pij2weight[Pij] += Xpq2weight[Xpq];
    ntstring=spf.inputG.ntstring[Aij2A (Xpq)];
    if (strncmp(ntstring,"Easy_NVA",8) == 0 &&  Aij2j (Xpq) - Aij2i (Xpq) > 0) {
      Pij2weight[Pij] += 1 ;
    }
  }
}

static void
prefer_REAL(Pij,cur_Aij,Xpq)
     int Pij, cur_Aij, Xpq;
{
  char *ntstring;
  
  if (Xpq > 0) {/* nt */
    Pij2weight[Pij] += Xpq2weight[Xpq];
    ntstring=spf.inputG.ntstring[Aij2A (Xpq)];
    if (strncmp(ntstring,"REAL__",6) == 0 &&  Aij2j (Xpq) - Aij2i (Xpq) > 0) {
      if (strncmp(ntstring,"REAL__Suj_",10) == 0) {
	Pij2weight[Pij] += 3 ;
      }
      if (strncmp(ntstring,"REAL__Obj_",10) == 0) {
	Pij2weight[Pij] += 3 ;
      }
      if (strncmp(ntstring,"REAL__Obja",10) == 0 || strncmp(ntstring,"REAL__Objde",11) == 0) {
	Pij2weight[Pij] += 2 ;
      }
      if (strncmp(ntstring,"REAL__Obl",9) == 0) {
	Pij2weight[Pij] += 1 ;
      }
      if (strncmp(ntstring,"REAL__Att",9) == 0) {
	Pij2weight[Pij] += 1 ;
      }
      if (strncmp(ntstring,"REAL__Loc",9) == 0 || strncmp(ntstring,"REAL__Dloc",10) == 0) {
	Pij2weight[Pij] += 1 ;
      }
    }
  }
}

static double
locally_select_optimal_trees (Aij)
     int Aij;
{
  int hook, init_hook, cur_Pij;
  double localmax;
  bool found_best_hook;


  found_best_hook = false;
  init_hook = hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;     
  localmax=LONG_MIN; /* on aimerait un DOUBLE_MIN, mais bon, le LONG_MIN va très bien */

  /* pour pouvoir garder ou bien un seul arbre ou bien tous les arbres de poids optimal, on fait 2 passes
     sur la liste des productions de partie gauche Aij:
     1. passe 1 pour déterminer le poids optimal
     2. passe 2 pour élaguer
  */

  /* passe 1 */
  while ((cur_Pij = spf.outputG.rhs [hook].lispro) != 0) {
    if (cur_Pij > 0) { /* il ne s'agit pas d'une branche elaguee */
      if (localmax < Pij2weight[cur_Pij]) { /* si cette production est meilleure que ce qu'on a déjà vu... */
	localmax = Pij2weight[cur_Pij]; /*... on stocke la nouvelle valeur optimale... */
      }
    }
    hook++;
  }

  /* passe 2 */
  hook = init_hook;
  found_best_hook = false;
  while ((cur_Pij = spf.outputG.rhs [hook].lispro) != 0) {
    if (cur_Pij > 0) { /* il ne s'agit pas d'une branche elaguee */
#if EBUG
      spf_print_iprod(stdout,cur_Pij,"","");
      fprintf(stdout," %e\n",Pij2weight[cur_Pij]);
#endif
      if (Pij2weight[cur_Pij] < (localmax-0.00001)*tolerance_level /* si cette production n'est pas optimale... */
	  || (found_best_hook && STRUCTURE_SEM_PASS_ARG == 2)
	  /* ou alors on a déjà trouvé une prod optimale, celle-ci l'est aussi mais on ne veut qu'un seul arbre*/
	  ) { 
	spf.outputG.rhs [hook].lispro = -cur_Pij; /*...on l'élague ;... */
	spf.outputG.is_proper = false;
#if EBUG
	fprintf(stdout," - eliminated\n");
#endif
      } else {
	found_best_hook = true; /* on se souvient qu'on a trouvé au moins une prod optimale */
#if EBUG
	fprintf(stdout," - kept\n");
#endif
      }
    }
    hook++;
  }

  return localmax;
}

static int
select_optimal_trees (Pij)
     int Pij;
{
  int Xpq, cur_Aij, prev_Aij, hook, best_hook, sntid, id, Tij, lb, ub;
  char *ntstring, tempChar, *tempString, *word, *tstring, *comment, *cfirst;
  double localweight, tweight;
  struct snt *c;
  struct lhs *plhs;
  struct rhs *prhs;
  
  cur_Aij=spf.walk.cur_Aij;
  prev_Aij=spf.walk.prev_Aij;

  plhs = spf.outputG.lhs+Pij; /* on accede a l'entree de la prod dans lhs */
  prhs = spf.outputG.rhs+plhs->prolon; /* on accede a la partie droite de la prod dans rhs */
  
  if (cur_Aij != prev_Aij && prev_Aij != 0)
    /* si on est sur la première prod d'un nouvel Aij, on peut faire le calcul sur prev_Aij */
    /* on donne à prev_Aij le poids de la rhs de sa Pij optimale */
    /* (contrib nulle de prev_Aij lui-même, puisqu'on fait remonter les poids des terminaux) */
    Xpq2weight[prev_Aij] = locally_select_optimal_trees(prev_Aij);
  
  if (cur_Aij == spf.outputG.start_symbol) /* tout pareil, mais si on est sur l'axiome on travaille aussi sur cur_Aij */
    Xpq2weight[cur_Aij] = locally_select_optimal_trees(cur_Aij);
  
  /* on bosse mainenant sur la Pij courante */

  while ((Xpq = (prhs++)->lispro) != 0) { /* on parcourt la partie droite */
    (*specific_contribution)(Pij,cur_Aij,Xpq);
  }
  return 1;
}

static int heuristics_nb;
double new_tree_nb_array[12];

static void
apply_heuristics (heuristics, tl)
     void (*heuristics)();
     int tl;
{
  int i;

#if EBUG
  fprintf(stdout,"\n############## HEURISTICS %i : START ######## input tree nb = %e\n\n",heuristics_nb,spf_ptn());  
#endif  /* EBUG */
  specific_contribution = heuristics;
  tolerance_level = tl;
  spf_topological_walk (spf.outputG.start_symbol, NULL, select_optimal_trees);
  new_tree_nb_array[heuristics_nb]=spf_ptn();
  for (i = 0; i<=spf.outputG.maxxnt; i++)
    Xpq2weight[i] = 0;
  for (i = 0; i<=spf.outputG.maxxprod; i++)
    Pij2weight[i] = 0;
#if EBUG
  fprintf(stdout,"\n############## HEURISTICS %i : END ######## output tree nb = %e\n\n",heuristics_nb,spf_ptn());  
#endif  /* EBUG */
  heuristics_nb++;
}

static int
structure_sem_pass ()
{
  double tree_nb,new_tree_nb;
  int maxeid, i;
  char msg[128], qualifier[10];
  bool is_not_empty, temp_bool;

#if EBUG
  fprintf(stdout,"\n");
  fprintf(stdout,"*************************************************************************\n");
  fprintf(stdout,"************* S T A R T I N G   S T R U C T S   M O D U L E *************\n");
  fprintf(stdout,"*************************************************************************\n");
#endif /* EBUG */

  qualifier[0] = SXNUL;
    
  if (priority_vector == NULL)
    priority_vector = &(priority_vector_default [0]);

  priority_vector_lgth = strlen (priority_vector);

  maxxt = -spf.outputG.maxt;

  allocate_all ();
  tree_nb = spf_ptn();

  apply_heuristics(prefer_less_appos, 1);
  apply_heuristics(prefer_ADV_MOD, 1);
  apply_heuristics(prefer_local_COO, 1);
  apply_heuristics(prefer_Easy_NVA, 1);
  apply_heuristics(prefer_REAL, 1);
  
  new_tree_nb = spf_ptn();

  if (new_tree_nb == 0) {
    if (is_print_time) {
      sprintf(msg,"\tStructure filtering (FALSE)");
      msg [1] = sxtoupper (msg [1]);
      sxtime (SXACTION, msg);
    }
    return 0;
  }

  if (is_print_time) {
    sprintf(msg,"\tStructure filtering (true ");
    msg [1] = sxtoupper (msg [1]);
    if (tree_nb < 1.0E6)
      sprintf(msg,"%s%.f -> ",msg,tree_nb);
    else
      sprintf(msg,"%s%.2le -> ",msg,tree_nb);
    for (i = 0; i<heuristics_nb-1; i++) {
      if (tree_nb < 1.0E6)
	sprintf(msg,"%s%.f -> ",msg,new_tree_nb_array[i]);
      else
	sprintf(msg,"%s%.2le -> ",msg,new_tree_nb_array[i]);
    }
    
    if (new_tree_nb < 1.0E6)
      sprintf(msg,"%s%.f)",msg,new_tree_nb);
    else
      sprintf(msg,"%s%.2le)",msg,new_tree_nb);

    sxtime (SXACTION, msg);
  }

#if EBUG
  fprintf(stdout,"\n");
  fprintf(stdout,"*********************************************************************\n");
  fprintf(stdout,"************* E N D   O F   S T R U C T S   M O D U L E *************\n");
  fprintf(stdout,"*********************************************************************\n");
#endif /* EBUG */

  return 1;
}
 
void
structure_semact ()
{
  for_semact.sem_final = structure_sem_final;
  for_semact.sem_pass = structure_sem_pass;
  for_semact.process_args = structure_args_decode;
  for_semact.string_args = structure_args_usage;
  for_semact.ME = output_ME;
  

  for_semact.rcvr = NULL;
  
  rcvr_spec.range_walk_kind = MIXED_FORWARD_RANGE_WALK_KIND;

}

/* si  on veut sa propre stratégie:
   for_semact.rcvr_range_walk = mafonction;
   où mafonction a 5 arguments:
   1=le pt de détection de l'erreur
   2,3=le range précédent (qui a raté,  sinon on serait pas appelé)
   4,5=le prochain range à essayer: ce sont des pointeurs, à moi leur donner la valeur que je veux
*/
