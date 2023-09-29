/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2005 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */

/* Actions sémantiques permettant le filtrage de la forêt par des poids lexicaux */


/* L'analyseur est de type Earley */
/* Le source est sous forme de SDAG */

/* ********************************************************
   *                                                      *
   *          Produit de l'equipe ATOLL.                  *
   *                                                      *
   ******************************************************** */



static char	ME [] = "weights_semact";

#include "sxunix.h"
#include "earley.h"
#include "dag_scanner.h"
#include "sxword.h"
#include "sxstack.h"
#include "varstr.h"
#include "XH.h"

#ifdef lfg_h

#define def_priority_hd
#define def_priority_list

#include lfg_h
#endif /* lfg_h */


#ifdef lex_lfg_h

#define def_if_id2t_list
#define def_inflected_form2display

#define def_structure_args_list
#include lex_lfg_h

#endif /* lex_lfg_h */

#ifdef mp_h
#  define def_mp_prod_table
#  define def_mp_nt_table
#  define def_mp_t_table
#  include mp_h
#  define PROD2RULE(production)       prod2rule [mp_prod_table [production]]
#  define IS_COMPLETE(nt)     SXBA_bit_is_set (complete_id_set, mp_nt_table [nt])
#  define RANKS(nt)           ranks [mp_nt_table [nt]]
#  define TERM_CONVERT(t)           mp_t_table [t]
#else /* mp_h */
#  define PROD2RULE(production)       prod2rule [production]
#  define IS_COMPLETE(nt)     SXBA_bit_is_set (complete_id_set, nt)
#  define RANKS(nt)           ranks [nt]
#  define TERM_CONVERT(t)           t
#endif /* mp_h */

char WHAT_WEIGHTS_SEMACT[] = "@(#)SYNTAX - $Id$" WHAT_DEBUG;

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

  if (*arg == NUL)
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

/* retourne la chaine des arguments possibles propres a weights */
static char*
weights_args_usage ()
{
  return Usage;
}

/* decode les arguments specifiques a weights */
/* l'option argv [*parg_num] est inconnue du parseur earley */
static BOOLEAN
weights_args_decode (pargnum, argc, argv)
     int  *pargnum, argc;
     char *argv [];
{
  switch (option_get_kind (argv [*pargnum])) {

  case UNKNOWN_ARG:
    return FALSE;
  }

  return TRUE;
}





static int               maxxt; /* nb des terminaux instancies Tij, y compris ceux couverts par les nt de la rcvr == -spf.outputG.maxt */

static double *Pij2weight,*Xpq2weight;
static double *Tij2highest_prio;
static double        *spf_count;

static char    *priority_vector;
static char    priority_vector_default [] = "1";
static int     priority_vector_lgth;

BOOLEAN       WEIGHTS_SEM_PASS_ARG;

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
weights_sem_final()
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




/* Le 13/11/06 une liste de priorites est associee a une f_structure lexicale, on recupere la posieme (on commence a 0 ) */
static int
priority_id2priority (int priority_id)
{
  int size, bot, top, priority;
  int *cur_plist, *top_plist;
  char *cur_pvect;
  /* Defaut priority_vector = "10..."
     Defaut priority_list = (100, 0, ...) */

  if ((priority_id != 0)) {
    priority = 0;
    size = priority_vector_lgth;
    bot = priority_hd [priority_id];
    top = priority_hd [priority_id+1];

    if (top-bot < size)
      size = top-bot;

    cur_plist = priority_list + bot;
    top_plist = cur_plist + size;
    cur_pvect = priority_vector;

    while (cur_plist < top_plist) {
      priority += *cur_plist * (*cur_pvect-'0');

      cur_plist++;
      cur_pvect++;
    }
  }
  else if (priority_id == -1)
    priority = 0;
    
  else
    priority = 100;
    
  return priority;
}

/* On instancie la structure terminale statique reperee par x sur Tpq */
static int
get_priority (x)
     int  x;
{
  struct structure_args_list *ptr2;

  ptr2 = structure_args_list+x;

  return priority_id2priority (ptr2->priority);
}



/* On instancie le terminal Tpq de la forme flechie if_id */
static void
terminal_instantiation (Tij, t, if_id/* , terminal_string */)
     int  Tij, t, if_id;
     /* char *terminal_string; */
{
  int                        bot, top, bot2, top2, x;
  int                        lexeme_id, struct_id, head, temp_prio, prio;
  struct if_id2t_list        *ptr;
#if EBUG
  BOOLEAN                    done = FALSE;
#endif /* EBUG */

  prio = 0;

  for (bot = inflected_form2display [if_id], top = inflected_form2display [if_id+1];
       bot < top;
       bot++) {
    ptr = if_id2t_list+bot;

    if (ptr->t == TERM_CONVERT (t)) {
#if EBUG
      done = TRUE;
#endif /* EBUG */

      bot2 = ptr->list;
      top2 = if_id2t_list [bot+1].list;

      while (bot2 < top2) {
	temp_prio = get_priority (bot2);
	if (prio < temp_prio)
	  prio = temp_prio;
	bot2++;
      }
    }
  }

  Tij2highest_prio [Tij] = prio ;

#if EBUG
  if (!done) {
    char string[256];
    sprintf (string, 
	     "terminal_instantiation (aucun des homonymes de la forme flechie en [%i%s%i] n'a la categorie terminale %s !!)", 
	     mlstn2dag_state (Tij2i (Tij)), 
	     (spf.outputG.Tpq_rcvr_set && SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tij)) ? "::" : "..",
	     mlstn2dag_state (Tij2j (Tij)),
	     spf.inputG.tstring [t]);
    /* aucun des homonymes de la forme flechie if_id n'a la categorie terminale t !! */
    sxtrap (ME, string);
  }
#endif /* EBUG */
}

/* On commence par instancier les feuilles terminales */
/* Nelle version qui utilise parser_Tpq2tok (Tpq) */
static void
set_terminal_leaves ()
{
  int              Tpq, T, p, q, if_id;
  char             *terminal_string, *comment;
  struct sxtoken   *tok_ptr;

#if LOG
  fputs ("Terminal leaves\n", stdout);
#endif /* LOG */

  for (Tpq = 1; Tpq <= maxxt; Tpq++) {
    tok_ptr = parser_Tpq2tok (Tpq);

    if (SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tpq))
      /* terminal de l'error-recovery */
      if_id = 0;
    else
      if_id = tok_ptr->lahead;
    
    T = -Tij2T (Tpq);
    /* On peut donc instancier les f-structures lexicales associees a (if_id, T) */
#if LOG
    p = Tij2i (Tpq);
    q = Tij2j (Tpq);
    p = mlstn2dag_state (p);
    q = mlstn2dag_state (q);
    terminal_string = sxstrget (tok_ptr->string_table_entry);
    comment = tok_ptr->comment;
    printf ("<%i, %s%s/%s, %i>(if_id=%i, Tpq=%i)\n",
	    p, (comment == NULL) ? "" : comment, terminal_string, spf.inputG.tstring [T], q, if_id, Tpq);
#endif /* LOG */
    
    if (if_id)
      terminal_instantiation (Tpq, T, if_id/* , terminal_string */);
    
  }
}


static double
locally_select_optimal_trees (Aij)
     int Aij;
{
  int hook, init_hook, cur_Pij;
  double localmax;
  BOOLEAN found_best_hook;


  found_best_hook = FALSE;
  init_hook = hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;     
  localmax=0.0;

  /* pour pouvoir garder ou bien un seul arbre ou bien tous les arbres de poids optimal, on fait 2 passes
     sur la liste des productions de partie gauche Aij:
     1. passe 1 pour déterminer le poids optimal
     2. passe 2 pour élaguer
  */

  /* passe 1 */
  while ((cur_Pij = spf.outputG.rhs [hook].lispro) != 0) {
    if (cur_Pij > 0) { /* il ne s'agit pas d'une branche elaguee */
#if EBUG
      spf_print_iprod(stdout,cur_Pij,"","");
      fprintf(stdout," %e\n",Pij2weight[cur_Pij]);
#endif
      if (localmax < Pij2weight[cur_Pij]) { /* si cette production est meilleure que ce qu'on a déjà vu... */
	localmax = Pij2weight[cur_Pij]; /*... on stocke la nouvelle valeur optimale... */
      }
    }
    hook++;
  }

  /* passe 2 */
  hook = init_hook;
  found_best_hook = FALSE;
  while ((cur_Pij = spf.outputG.rhs [hook].lispro) != 0) {
    if (cur_Pij > 0) { /* il ne s'agit pas d'une branche elaguee */
      if (localmax > Pij2weight[cur_Pij]-0.000001 /* si cette production n'est pas optimale... */
	  || (found_best_hook && WEIGHTS_SEM_PASS_ARG == 2)
	  /* ou alors on a déjà trouvé une prod optimale, celle-ci l'est aussi mais on ne veut qu'un seul arbre*/
	  ) { 
	spf.outputG.rhs [hook].lispro = -cur_Pij; /*...on l'élague ;... */
	spf.outputG.is_proper = FALSE;
      } else {
	found_best_hook = TRUE; /* on se souvient qu'on a trouvé au moins une prod optimale */
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
  char tempChar, *tempString, *word, *tstring, *comment, *cfirst;
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
    if (Xpq > 0) /* nt */
      Pij2weight[Pij] += Xpq2weight[Xpq];
    else /* terminal */
      Pij2weight[Pij] += Tij2highest_prio[-Xpq];
  }
  return 1;
}



static int
weights_sem_pass ()
{
  double tree_nb,new_tree_nb;
  int maxeid;
  char msg[60], qualifier[10];
  BOOLEAN is_not_empty, temp_bool;

#if EBUG
  fprintf(stdout,"\n");
  fprintf(stdout,"*************************************************************************\n");
  fprintf(stdout,"************* S T A R T I N G   W E I G H T S   M O D U L E *************\n");
  fprintf(stdout,"*************************************************************************\n");
#endif /* EBUG */

  qualifier[0] = NUL;
    
  if (priority_vector == NULL)
    priority_vector = &(priority_vector_default [0]);

  priority_vector_lgth = strlen (priority_vector);

  maxxt = -spf.outputG.maxt;

  allocate_all ();
  tree_nb = spf_ptn();

#if EBUG
  print_ptn();
#endif /* EBUG */

  set_terminal_leaves();

  spf_topological_walk (spf.outputG.start_symbol, NULL, select_optimal_trees);
  
  new_tree_nb = spf_ptn();

  if (new_tree_nb == 0) {
    if (is_print_time) {
      sprintf(msg,"\tWeights filtering (FALSE)");
      msg [1] = sxtoupper (msg [1]);
      sxtime (ACTION, msg);
    }
    return 0;
  }

  if (is_print_time) {
    sprintf(msg,"\tWeights filtering (TRUE ");
    msg [1] = sxtoupper (msg [1]);
    if (tree_nb < 1.0E6)
      sprintf(msg,"%s%.f -> ",msg,tree_nb);
    else
      sprintf(msg,"%s%.2le -> ",msg,tree_nb);
    
    if (new_tree_nb < 1.0E6)
      sprintf(msg,"%s%.f)",msg,new_tree_nb);
    else
      sprintf(msg,"%s%.2le)",msg,new_tree_nb);

    sxtime (ACTION, msg);
  }

#if EBUG
  fprintf(stdout,"\n");
  fprintf(stdout,"*********************************************************************\n");
  fprintf(stdout,"************* E N D   O F   W E I G H T S   M O D U L E *************\n");
  fprintf(stdout,"*********************************************************************\n");
#endif /* EBUG */

  return 1;
}
 
void
weights_semact ()
{
  for_semact.sem_final = weights_sem_final;
  for_semact.sem_pass = weights_sem_pass;
  for_semact.process_args = weights_args_decode;
  for_semact.string_args = weights_args_usage;
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
