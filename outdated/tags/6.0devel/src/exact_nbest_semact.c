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

/* PCFG */
/* Actions semantiques ne conservant que les n meilleures analyses */
/* L'analyseur est de type Earley */
/* Le source est sous forme de DAG */




static char	ME [] = "exact_nbest_semact";

#define SX_GLOBAL_VAR_EXACT_NBEST

#include "sxversion.h"
#include "sxunix.h"
#include "earley.h"
#include "XH.h"
#include "nbest.h"
#include "sxstack.h"
#include "sxspf.h"
#include "SS.h"
#include <ctype.h>
#include <math.h>
#include <float.h>
#include "udag_scanner.h"
#define GET_HOOK(Aij)(spf.outputG.lhs [spf.outputG.maxprod+(Aij)].prolon)


char WHAT_EXACT_NBEST_SEMACT[] = "@(#)SYNTAX - $Id: exact_nbest_semact.c 1594 2008-09-22 14:39:31Z syntax $" WHAT_DEBUG;

/* Ensemble des triplets (A, prod, val) ou
   - prod est une B-prod, 
   - A est un pere de B (il existe une A-prod t.q. B est ds sa partie droite)
   - val est un entier qui indique si les nt en RHS de prod qui peuvent deriver ds le vide, derivent ds le vide
     Ex si prod = B -> X1 X2 X3 X4 X5 et si X1, X3, X4 et X5 peuvent deriver ds le vide, val=2 (0010) indique
     que ne l'on considere que les Pij de prod ds lesquelles X1, X3 et X5 derivent (effectivement) ds le vide
     et X4 ne derive pas ds le vide
*/


extern struct dedication* get_dedication_ptr (SXINT item_or_Aij, SXINT k);
extern SXINT              get_dedication_top (SXINT item_or_Aij);


/* valeur par defaut */
static SXINT     exact_nbest = 1;

static char	Usage [] = "\
\t\t-N #exact_nbest, -en #exact_nbest, -exact_nbest #exact_nbest (default -exact_nbest 1)\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	   (SXINT)0
#define EXACT_NBEST 	           (SXINT)1

static char	*option_tbl [] = {
    "",
    "N", "en", "exact_nbest",
};

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    EXACT_NBEST, EXACT_NBEST, EXACT_NBEST,
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


/* retourne le ME */
static char*
exact_nbest_ME (void)
{
  return ME;
}

/* retourne la chaine des arguments possibles propres a exact_nbest */
static char*
exact_nbest_args_usage (void)
{
  return Usage;
}

/* decode les arguments specifiques a lfg */
/* l'option argv [*parg_num] est inconnue du parseur earley */
static SXBOOLEAN
exact_nbest_args_decode (int *pargnum, int argc, char *argv[])
{
  exact_nbest = nbest; /* par défaut, valeur de nbest_semact, qui a déjà lu ses options avant exact_nbest_semact.c (cf sxsem_mngr.c) */

  switch (option_get_kind (argv [*pargnum])) {
  case EXACT_NBEST:
    if (++*pargnum >= argc) {
      fprintf (stderr, "%s: an SXINTeger number must follow the \"%s\" option;\n", ME, option_get_text (EXACT_NBEST));
      return SXFALSE;
    }

    exact_nbest = atoi (argv [*pargnum]);

    if (exact_nbest > nbest) {
      fprintf (stderr, "%s: the exact_nbest parameter (%ld) cannot be greater that the nbest parameter (%ld); exact_nbest was forced to be equal to nbest;\n", ME, option_get_text (EXACT_NBEST), (long) exact_nbest, (long) nbest);
      exact_nbest = nbest;
    }

    break;

  case UNKNOWN_ARG:
    return SXFALSE;
  default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap(ME,"unknown switch case #1");
#endif
    break;
  }

  return SXTRUE;
}

#define pb SXTRUE
#define an SXTRUE
#define print_forest SXTRUE


static SXBA       Pij_set;
static SXBA       *Pij2rankset, *Aij2rankset;
static XxY_header AijXid, nbest_forest_hd;
static XH_header  XH_rhs, XH_nbest_set_hd;
static XxY_header AijXrhs;
static SXBA       multiple_AijXrhs_set, rhs_set;
static SXINT      *Aij_id2prod;
static SXINT      *Aij_id_stack;
static XH_header  XH_rhs_set;
static SXINT      *XH_rhs_set2Aij_id;
#if 0
static SXINT      *Aij_saturated2Aij_id, *Aij_saturated2id;
#endif /* 0 */



static SXINT kth, kth_id, start_symbol_id;

/* (an)*/


typedef struct rectangle{
  SXINT x_min, x_max, y_min, y_max;
  struct rectangle *suiv;
} RECTANGLE;

#define INVALIDE -1

typedef struct {
  SXINT x,y;
} couple_SXINT;



static SXINT  *Pij_i_max;
static SXINT  *Pij_j_max;
static SXINT  *Pij_or;

/* permet de connaitre la position relative d'un Pij : */
/* Pij est le Pij_rel[Pij] ieme Pij de son Aij         */

static SXINT  *Pij_rel;

static SXINT  ***Pij_mat;


static XxYxZ_header Aij_m_n_hd;
static couple_SXINT **Aij_m_n;
static XxYxZ_header Pij_m_n_hd;
static RECTANGLE **Pij_m_n;






static void
AijXrhs_oflw (SXINT old_size, SXINT new_size)
{
  multiple_AijXrhs_set = sxba_resize (multiple_AijXrhs_set, new_size+1);
}

static void
XH_rhs_oflw (SXINT old_size, SXINT new_size)
{
  if (rhs_set)
    rhs_set = sxba_resize (rhs_set, new_size+1);
}


static void
XH_rhs_set_oflw (SXINT old_size, SXINT new_size)
{
  XH_rhs_set2Aij_id = (SXINT *) sxrealloc (XH_rhs_set2Aij_id, new_size+1, sizeof (SXINT));
}


static void
AijXid_oflw (SXINT old_size, SXINT new_size)
{
  Aij_id2prod = (SXINT *) sxrealloc (Aij_id2prod, new_size+1, sizeof (SXINT));
}


static SXINT exact_nbest_sem_pass (void);
static void exact_nbest_sem_init (void);
static void exact_nbest_sem_final (void);
static void exact_nbest_sem_close (void);
static SXBOOLEAN Aij_or_item_kth_best (SXINT Aij_or_item, SXINT k);


void
exact_nbest_semact (void)
{
  for_semact.sem_init = exact_nbest_sem_init; /* avant chaque phrase */
  for_semact.sem_pass = exact_nbest_sem_pass;
  for_semact.process_args = exact_nbest_args_decode; /* decodage des options de exact_nbest */
  for_semact.string_args = exact_nbest_args_usage; /* decodage des options de exact_nbest */
  for_semact.ME = exact_nbest_ME; /* decodage des options de exact_nbest */
  for_semact.sem_final = exact_nbest_sem_final; /* à la fin de chaque phrase */
  for_semact.sem_close = exact_nbest_sem_close; /* à la fin globale */
}

SXINT **alloue_mat(SXINT x, SXINT y)
{
  SXINT **mat;
  SXINT i,j;

  mat = sxalloc(x, sizeof(SXINT *));
  for(i=0; i < x; i++){
    mat[i] = sxalloc(y , sizeof(SXINT));
    for(j=0; j < y; j++)
      mat[i][j] = -1;
  }
  return mat;
}

static void
Aij_m_n_hd_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  Aij_m_n = (couple_SXINT **) sxrealloc (Aij_m_n, new_size+1, sizeof (couple_SXINT *));
}

static void
Pij_m_n_hd_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  Pij_m_n = (RECTANGLE **) sxrealloc (Pij_m_n, new_size+1, sizeof (RECTANGLE *));
}



/* Peut etre appele' un nb qqcq de fois sur des environnements qqc */
void
exact_nbest_allocate (SXINT exact_nbest_allocate_exact_nbest)
{
    SXINT Aij_m_n_hd_foreach [] = {0, 0, 0, 0, 0, 0}; /* A voir */
    SXINT Pij_m_n_hd_foreach [] = {0, 0, 0, 0, 0, 0}; /* A voir */

  if(pb){
    Pij_set = sxba_calloc (spf.outputG.maxxprod+1);
    Pij2rankset = sxbm_calloc (spf.outputG.maxxprod+1, exact_nbest_allocate_exact_nbest+1);
    Aij2rankset = sxbm_calloc (spf.outputG.maxxnt+1, exact_nbest_allocate_exact_nbest+1);
    
    XxY_alloc (&AijXid, "AijXid", spf.outputG.maxxnt*exact_nbest_allocate_exact_nbest+1, 1, 1, 0, AijXid_oflw, statistics);
    XH_alloc (&XH_rhs, "XH_rhs", spf.outputG.maxxprod+1, 1, spf.inputG.maxrhs, XH_rhs_oflw, statistics);
    XxY_alloc (&nbest_forest_hd, "nbest_forest_hd", spf.outputG.maxxprod+1, 1, 1, 1 /* pour retrouver les lhs ayant meme rhs pour bu optim */, NULL, statistics);
    XH_alloc (&XH_nbest_set_hd, "XH_nbest_set_hd", spf.outputG.maxxprod+1, 1, spf.inputG.maxrhs, NULL, statistics);

    XxY_alloc (&AijXrhs, "AijXrhs", spf.outputG.maxxprod+1, 1, 0, 0, AijXrhs_oflw, statistics);
    multiple_AijXrhs_set = sxba_calloc (XxY_size (AijXrhs)+1);
    Aij_id2prod = (SXINT *) sxalloc (XxY_size (AijXid)+1, sizeof (SXINT));
    DALLOC_STACK (Aij_id_stack, spf.outputG.maxxprod);
    XH_alloc (&XH_rhs_set, "XH_rhs_set", spf.outputG.maxxprod+1, 1, spf.inputG.maxrhs, XH_rhs_set_oflw, statistics);
    XH_rhs_set2Aij_id = (SXINT *) sxalloc (XH_size (XH_rhs_set)+1, sizeof (SXINT));

#if 0
    Aij_saturated2id = (SXINT *) sxcalloc (spf.outputG.maxxnt+1, sizeof (SXINT));
    Aij_saturated2Aij_id = (SXINT *) sxcalloc (spf.outputG.maxxnt+1, sizeof (SXINT));
#endif /* 0 */
  }

  if(an){
    Pij_i_max = (SXINT *) sxcalloc(spf.outputG.maxxprod + 1, sizeof(SXINT));
    Pij_j_max = (SXINT *) sxcalloc(spf.outputG.maxxprod + 1, sizeof(SXINT));
    Pij_or    = (SXINT *) sxcalloc(spf.outputG.maxxprod + 1, sizeof(SXINT));
    Pij_rel   = (SXINT *) sxcalloc(spf.outputG.maxxprod + 1, sizeof(SXINT));
    Pij_mat   = (SXINT ***) sxcalloc(spf.outputG.maxxprod + 1, sizeof(SXINT**));
    
    XxYxZ_alloc (&Aij_m_n_hd, "Aij_m_n_hd", 500, 1, Aij_m_n_hd_foreach, Aij_m_n_hd_oflw, NULL);
    Aij_m_n = (couple_SXINT **) sxcalloc (XxYxZ_size(Aij_m_n_hd)+1, sizeof (couple_SXINT *));
    
    XxYxZ_alloc (&Pij_m_n_hd, "Pij_m_n_hd", 500, 1, Pij_m_n_hd_foreach, Pij_m_n_hd_oflw, NULL);
    Pij_m_n = (RECTANGLE **) sxcalloc (XxYxZ_size(Pij_m_n_hd)+1, sizeof (RECTANGLE *));
  }
}


void exact_nbest_sem_init(void)
{
    exact_nbest_allocate (exact_nbest);
}


void libere_mat(SXINT **mat, SXINT x)
{
  SXINT i;
  for(i=0; i < x; i++){
    sxfree(mat[i]);
  }
  sxfree(mat);
}




void
exact_nbest_free (void)
{
  SXINT Pij;

  if(pb){
    sxfree (Pij_set), Pij_set = NULL;
    sxbm_free (Pij2rankset), Pij2rankset = NULL;
    sxbm_free (Aij2rankset), Aij2rankset = NULL;
    
    XxY_free (&AijXid);
    XH_free (&XH_rhs);
    XxY_free (&nbest_forest_hd);
    XH_free (&XH_nbest_set_hd);
    
    XxY_free (&AijXrhs);
    sxfree (multiple_AijXrhs_set), multiple_AijXrhs_set = NULL;
    sxfree (Aij_id2prod), Aij_id2prod = NULL;
    DFREE_STACK (Aij_id_stack);
    XH_free (&XH_rhs_set);
    sxfree (XH_rhs_set2Aij_id), XH_rhs_set2Aij_id = NULL;

#if 0
    sxfree (Aij_saturated2id), Aij_saturated2id = NULL;
    sxfree (Aij_saturated2Aij_id), Aij_saturated2Aij_id = NULL;
#endif /* 0 */
  }

  if(an){
    for(Pij=0; Pij <= spf.outputG.maxxprod; Pij++) 
      if(Pij_mat[Pij])
	libere_mat(Pij_mat[Pij], Pij_i_max[Pij] + 1);
    sxfree (Pij_mat), Pij_mat = NULL;
    sxfree (Pij_i_max), Pij_i_max = NULL;
    sxfree (Pij_j_max), Pij_j_max = NULL;
    sxfree (Pij_or), Pij_or = NULL;
    

  }
}


static void
exact_nbest_sem_final (void)
{
  exact_nbest_free ();
}

static void
exact_nbest_sem_close (void)
{
}



static void
nbest_forest_print_Aij_id (SXINT Aij_id)
{
  SXINT Aij, id;

  Aij = XxY_X (AijXid, Aij_id);
  fputs ("[", stdout);
  spf_print_Xpq (stdout, Aij);

  id = XxY_Y (AijXid, Aij_id);
  printf (", %ld]", (long)id);
}

static void
nbest_forest_print_id_set (SXINT id)
{
  SXINT i, bot, cur, top;

  printf ("%ld = {", (long)id);

  bot = XH_X (XH_nbest_set_hd, id);
  top = XH_X (XH_nbest_set_hd, id+1);

  for (cur = bot; cur < top; cur++) {
    if (cur > bot)
      fputs (", ", stdout);

    i = XH_list_elem (XH_nbest_set_hd, cur);
    printf ("%ld", (long)i);
  }

  fputs ("}\n", stdout);
}

static void
nbest_forest_print_prod (SXINT prod, SXINT i)
{
  SXINT lhs, rhs, bot, top, Xpq;

  printf ("%ld: ", (long)i);

  lhs = XxY_X (nbest_forest_hd, prod);
  nbest_forest_print_Aij_id (lhs);

  fputs ("\t =", stdout);

  rhs = XxY_Y (nbest_forest_hd, prod);
  bot = XH_X (XH_rhs, rhs);
  top = XH_X (XH_rhs, rhs+1);

  while (bot < top) {
    fputs (" ", stdout);

    Xpq = XH_list_elem (XH_rhs, bot);

    if (Xpq > 0)
      nbest_forest_print_Aij_id (Xpq);
    else
      spf_print_Xpq (stdout, Xpq);
      
    bot++;
  }
    
  fputs (" ;\n", stdout);
}

static void
nbest_forest_print_forest (char *string)
{
  SXINT prod, id, lhs, rhs, bot, top, Xpq_id, i = 0;
  SXBA id_set;

  id_set = sxba_calloc (XH_top (XH_nbest_set_hd)+1);

  printf ("%s optimized forest:\n", string);

  for (prod = 1; prod <= XxY_top (nbest_forest_hd); prod++) {
    if (!XxY_is_erased (nbest_forest_hd, prod)) {
      nbest_forest_print_prod (prod, ++i);
      lhs = XxY_X (nbest_forest_hd, prod);
      id = XxY_Y (AijXid, lhs);
      SXBA_1_bit (id_set, id); /* pour l'axiome */

      rhs = XxY_Y (nbest_forest_hd, prod);

      bot = XH_X (XH_rhs, rhs);
      top = XH_X (XH_rhs, rhs+1);

      while (bot < top) {
	Xpq_id = XH_list_elem (XH_rhs, bot);

	if (Xpq_id > 0) {
	  id = XxY_Y (AijXid, Xpq_id);
	  SXBA_1_bit (id_set, id);
	}
	
	bot++;
      }
    }
  }

  fputs ("\nwith:\n", stdout);

  id = 0;

  while ((id = sxba_scan (id_set, id)) > 0)
    nbest_forest_print_id_set (id);

  sxfree (id_set);
}


#ifdef MEASURES
static struct size_of_G {
  SXINT N, T, P, G;
} baselineG, top_downG, bottom_upG;


static void
fill_measures (struct size_of_G *G)
{
  SXINT prod, rhs, bot, top, Xpq_id, Tij, Aij_id;
  SXBA  Tij_set, Aij_id_set;

  Tij_set = sxba_calloc (-spf.outputG.maxt+1);
  Aij_id_set = sxba_calloc (XxY_top (AijXid)+1);

  for (prod = 1; prod <= XxY_top (nbest_forest_hd); prod++) {
    if (!XxY_is_erased (nbest_forest_hd, prod)) {
      G->P++;

      Aij_id = XxY_X (nbest_forest_hd, prod);

      if (SXBA_bit_is_reset_set (Aij_id_set, Aij_id))
	G->N++;

      rhs = XxY_Y (nbest_forest_hd, prod);

      G->G++; /* la lhs */

      bot = XH_X (XH_rhs, rhs);
      top = XH_X (XH_rhs, rhs+1);

      while (bot < top) {
	Xpq_id = XH_list_elem (XH_rhs, bot);
	G->G++; /* le id */

	if (Xpq_id < 0) {
	  Tij = -Xpq_id;

	  if (SXBA_bit_is_reset_set (Tij_set, Tij))
	    G->T++;
	}
	else {
	  if (SXBA_bit_is_reset_set (Aij_id_set, Xpq_id))
	    G->N++;
	}
	
	bot++;
      }
    }
  }

  sxfree (Tij_set);
  sxfree (Aij_id_set);
}
#endif /* MEASURES */

/* Tous les Aij_id en rhs sont remplaces par new_Aij_id */
static void
replace_prod (SXINT Aij_id, SXINT new_Aij_id)
{
  SXINT prod, lhs, rhs, bot, top, rhs_Xpq, new_rhs, new_prod, Aij_rhs, new_couple;

  prod = Aij_id2prod [Aij_id];
  /* Aij_id se trouve ds la rhs de prod, on le remplace par new_Aij_id */
  lhs = XxY_X (nbest_forest_hd, prod);
  rhs = XxY_Y (nbest_forest_hd, prod);

  bot = XH_X (XH_rhs, rhs);
  top = XH_X (XH_rhs, rhs+1);

  while (bot < top) {
    rhs_Xpq = XH_list_elem (XH_rhs, bot);

    if (rhs_Xpq == Aij_id)
      rhs_Xpq = new_Aij_id;

    XH_push (XH_rhs, rhs_Xpq);

    bot++;
  }

  XH_set (&XH_rhs, &new_rhs);

  XxY_set (&nbest_forest_hd, lhs, new_rhs, &new_prod);

  if (XxY_set (&AijXrhs, XxY_X (AijXid, lhs), new_rhs, &Aij_rhs))
    /* Multiple prods with identical Aij in lhs and identical rhs */
    /* Aij_rhs est superieur au + grand indice qui existait avant ds multiple_AijXrhs_set (d'ou la boucle unique ds bottom_upG_optimization) */
    SXBA_1_bit (multiple_AijXrhs_set, Aij_rhs);

  bot = XH_X (XH_rhs, new_rhs);
  top = XH_X (XH_rhs, new_rhs+1);

  while (bot < top) {
    rhs_Xpq = XH_list_elem (XH_rhs, bot);

    if (rhs_Xpq > 0)
      Aij_id2prod [rhs_Xpq] = new_prod;

    bot++;
  }

  XxY_erase (nbest_forest_hd, prod); /* Supprimee */
}

/* L'optimisation top_down a deja ete faite nbest_forest_hd, XH_rhs et AijXid sont remplis */
/* Si les (Aij, k)-prods et les (Aij, k')-prods ont des rhs egales 2 a 2, alors on forme un seul groupe des (Aij, k U k')-prods
   et on change toutes les occur de (Aij, k) et (Aij, k') en rhs par (Aij, kUk') */
static void
bottom_upG_optimization (SXINT nbest)
{
  SXINT Aij_rhs, rhs, Aij, prod, Aij_id, id, prod2, rhs2, rhs_set_id, prev_Aij_id, bot, top, cur, i, new_id, new_Aij_id, new_prod, couple, old_prod, lhs, rhs_Xpq, new_rhs;
  SXBA  nbest_set;

  nbest_set = sxba_calloc (nbest+1);
  rhs_set = sxba_calloc (XH_size (XH_rhs)+1);

  Aij_rhs = 0;

  while ((Aij_rhs = sxba_scan_reset (multiple_AijXrhs_set, Aij_rhs)) > 0) {
    rhs = XxY_Y (AijXrhs, Aij_rhs);
    Aij = XxY_X (AijXrhs, Aij_rhs);
    /* Il y a au moins 2 prods de la forme (Aij, k) -> rhs et (Aij, k') -> rhs */
    
    XxY_Yforeach (nbest_forest_hd, rhs, prod) {
      Aij_id = XxY_X (nbest_forest_hd, prod);

      if (XxY_X (AijXid, Aij_id) == Aij) {
	DPUSH (Aij_id_stack, Aij_id); /* candidat */
      }
    }

    /* Aij_id_stack contient les (Aij, k) des prods compatibles ayant la meme rhs */
    while (!IS_EMPTY (Aij_id_stack)) {
      /* On a (Aij, k) -> rhs */
      Aij_id = DPOP (Aij_id_stack);

      /* prod est l'une d'entre elles */
      /* Je cherche toutes les prods qui ont (Aij, k) en lhs et je construis leur ensemble de rhs */
      XxY_Xforeach (nbest_forest_hd, Aij_id, prod2) {
	rhs2 = XxY_Y (nbest_forest_hd, prod2);
	SXBA_1_bit (rhs_set, rhs2);
      }

      rhs2 = 0;

      while ((rhs2 = sxba_scan_reset (rhs_set, rhs2)) > 0)
	XH_push (XH_rhs_set, rhs2);

      if (XH_set (&XH_rhs_set, &rhs_set_id)) {
	/* Ce n'est pas la 1ere fois que l'on trouve cet ensemble de rhs */
	/* S'il y a egalite des ensembles de rhs => les (Aij, k)-prods et les (Aij, k')-prods peuvent se fusionner */
	prev_Aij_id = XH_rhs_set2Aij_id [rhs_set_id];

#if EBUG
	if (prev_Aij_id > 0 && Aij_id == prev_Aij_id || prev_Aij_id < 0 && Aij_id == -prev_Aij_id)
	  sxtrap (ME, bottom_upG_optimization);
#endif /* EBUG */

	/* On peut fusionner prev_Aij_id avec Aij_id */
	id = XxY_Y (AijXid, Aij_id);
	bot = XH_X (XH_nbest_set_hd, id);
	top = XH_X (XH_nbest_set_hd, id+1);

	for (cur = bot; cur < top; cur++) {
	  i = XH_list_elem (XH_nbest_set_hd, cur);
	  SXBA_1_bit (nbest_set, i);
	}

	id = XxY_Y (AijXid, prev_Aij_id);
	bot = XH_X (XH_nbest_set_hd, id);
	top = XH_X (XH_nbest_set_hd, id+1);

	for (cur = bot; cur < top; cur++) {
	  i = XH_list_elem (XH_nbest_set_hd, cur);
	  SXBA_1_bit (nbest_set, i);
	}

	i = 0;

	while ((i = sxba_scan_reset (nbest_set, i)) > 0) {
	  XH_push (XH_nbest_set_hd, i);
	}
  
	XH_set (&XH_nbest_set_hd, &new_id);
	XxY_set (&AijXid, Aij, new_id, &new_Aij_id);

	/* DPUSH (Aij_id_stack, new_Aij_id); Supprime' le 07/05/09 */

	/* On supprime les prods ayant Aij_id et prev_Aij_id en lhs pour les remplacer par
	   new_Aij_id en lhs */
	XxY_Xforeach (nbest_forest_hd, Aij_id, prod2) {
	  rhs2 = XxY_Y (nbest_forest_hd, prod2);
	  XxY_erase (nbest_forest_hd, prod2);

	  XxY_set (&nbest_forest_hd, new_Aij_id, rhs2, &new_prod);
	  /* Il est inutile de dire que les Xpq de rhs sont ds new_prod !! */

	  if (XxY_set (&AijXrhs, Aij, rhs2, &couple))
	    /* on vient de le traiter !! */
	    SXBA_0_bit (multiple_AijXrhs_set, couple);
	}

	XxY_Xforeach (nbest_forest_hd, prev_Aij_id, prod2) {
	  XxY_erase (nbest_forest_hd, prod2);
	}

	replace_prod (Aij_id, new_Aij_id); /* Remplace toutes les prods ayant Aij_id en rhs par new_Aij_id */
	replace_prod (prev_Aij_id, new_Aij_id); /* Remplace toutes les prods ayant Aij_id en rhs par new_Aij_id */

	XxY_erase (AijXid, Aij_id);
	XxY_erase (AijXid, prev_Aij_id);

	XH_rhs_set2Aij_id [rhs_set_id] = new_Aij_id; /* Nouvel identifiant de la fusion */
      }
      else
	XH_rhs_set2Aij_id [rhs_set_id] = Aij_id;
    }

    XH_clear (&XH_rhs_set);
  }

  sxfree (nbest_set);
}

#if 0
static void
fill_kth_best_tree (SXINT Pij, SXFLOAT weight)
{
  SXINT Aij;

  /* Pour l'instant je ne fais rien de weight ? */
  /* Pij appartient au kth arbre */

  SXBA_1_bit (Pij_set, Pij);
  SXBA_1_bit (Pij2rankset [Pij], kth);
  Aij = spf.outputG.lhs [Pij].lhs;

#ifdef MEASURES
 {
   SXINT init_prod, item, X;
   
   if (Aij == spf.outputG.start_symbol) {
     /* On est sur une Pij qui definit l'axiome */
     /* L'axiome est partage' entre tous les arbres, on ne le compte pas, il sera ajoute' en fin */
   }
   else {
     baselineG.N++;
   }

   baselineG.P++;
   baselineG.G++; /* La lhs */
   init_prod = spf.outputG.lhs [Pij].init_prod;
   item = spf.inputG.prolon [init_prod];

   while ((X = spf.inputG.lispro [item++])) {
     baselineG.G++;

     if (X < 0)
       /* On decompte separement chaque occur de chaque terminal */
       baselineG.T++;
   }
 }
#endif /* MEASURES */
}
#endif /* 0 */


static SXINT
td_Pij_first_pb_pass (SXINT Pij)
{
  return 1;
}

static SXINT
td_Aij_pre_first_pb_pass (SXINT Aij)
{
  SXINT             rank, k, i, Pij, signature, Aij_id, top_k, id, Xpq_id;
  SXINT             Aijk, item, Xpq, Xpqk, rhs, Pijk, couple, bot, top, rhs_Xpq ;
  SXBA              Aij_rankset, Xpq_rankset;
  SXFLOAT            Aij_tree_count_nb;
  struct dedication *dedication_ptr;

#if 0
  Aij_tree_count_nb = spf.tree_count.nt2nb [Aij];

  top_k = get_dedication_top (Aij);

#if EBUG
  if (top_k == 0)
    sxtrap (ME, "td_Aij_pre_first_pb_pass");
#endif /* EBUG */

  if (top_k == (SXINT) Aij_tree_count_nb) {
    /* Tous les sous arbres issus d'Aij sont satures */
    id = Aij_saturated2id [Aij];

    if (id == 0) {
      /* 1ere fois */
      Aij_rankset = Aij2rankset [Aij];

      for (k = 1; k <= top_k; k++) {
	SXBA_1_bit (Aij_rankset, k);
	XH_push (XH_nbest_set_hd, k);
      }

      XH_set (&XH_nbest_set_hd, &id);
      Aij_saturated2id [Aij] = id;
      XxY_set (&AijXid, Aij, id, &Aij_id);
      Aij_saturated2Aij_id [Aij] = Aij_id;
    }
    else
      Aij_id = Aij_saturated2Aij_id [Aij];

    /* 1ere passe pour remplir tous les Xpq avec les bons k */
    k = 0;

    while (dedication_ptr = get_dedication_ptr (Aij, ++k)) {
      Pij = -dedication_ptr->PijK;
      SXBA_1_bit (Pij2rankset [Pij], k);
      signature = dedication_ptr->signature;
      bot = XH_X(signatures_hd, signature);

      item = spf.outputG.lhs [Pij].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	if (Xpq > 0) {
	  if (Aij_saturated2Aij_id [Xpq] == 0) {
	    /* pas encore calcule' */
	    if (get_dedication_top (Xpq) == (SXINT) spf.tree_count.nt2nb [Xpq]) {
	      /* sature' */
	      Xpq_rankset = Aij2rankset [Xpq];

	      for (i = 1; i <= top_k; i++) {
		SXBA_1_bit (Xpq_rankset, i);
		XH_push (XH_nbest_set_hd, i);
	      }

	      XH_set (&XH_nbest_set_hd, &id);
	      Aij_saturated2id [Xpq] = id;
	      XxY_set (&AijXid, Xpq, id, &Xpq_id);
	      Aij_saturated2Aij_id [Xpq] = Xpq_id;
	    }
	    else
	      Aij_saturated2id [Xpq] = XH_list_elem(signatures_hd, bot);
	  }
	}

	bot++;
      }
    }

    /* 2eme passe */
    k = 0;

    while (dedication_ptr = get_dedication_ptr (Aij, ++k)) {
      Pij = -dedication_ptr->PijK;
      item = spf.outputG.lhs [Pij].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	if (Xpq < 0)
	  XH_push (XH_rhs, Xpq);
	else {
	  if ((Xpq_id = Aij_saturated2Aij_id [Xpq]) == 0) {
	    top_k = Aij_saturated2id [Xpq];
	    Xpq_rankset = Aij2rankset [Xpq];

	    for (i = 1; i <= top_k; i++) {
	      SXBA_1_bit (Xpq_rankset, i);
	      XH_push (XH_nbest_set_hd, i);
	    }

	    XH_set (&XH_nbest_set_hd, &id);
	    Aij_saturated2id [Xpq] = id;
	    XxY_set (&AijXid, Xpq, id, &Xpq_id);
	    Aij_saturated2Aij_id [Xpq] = Xpq_id;
	  }

	  XH_push (XH_rhs, Xpq_id);
	}
      }

      XH_set (&XH_rhs, &rhs);

      XxY_set (&nbest_forest_hd, Aij_id, rhs, &Pijk);

      if (XxY_set (&AijXrhs, Aij, rhs, &couple))
	/* Multiple prods with identical Aij in lhs and identical rhs */
	SXBA_1_bit (multiple_AijXrhs_set, couple);

      bot = XH_X (XH_rhs, rhs);
      top = XH_X (XH_rhs, rhs+1);

      while (bot < top) {
	rhs_Xpq = XH_list_elem (XH_rhs, bot);

	if (rhs_Xpq > 0)
	  /* Un rhs_Xpq = (Xpq, k) correspond a une prod unique :
	     Il y a au plus un Xpq par arbre */
	  Aij_id2prod [rhs_Xpq] = Pijk;

	bot++;
      }
    }
  }
  else
#endif /* 0 */
    {
    Aij_rankset = Aij2rankset [Aij];
    k = 0;

    while (dedication_ptr = get_dedication_ptr (Aij, ++k)) {
      SXBA_1_bit (Aij_rankset, k);

      if (Aij == spf.outputG.start_symbol) {
	/* On est sur une Pij qui definit l'axiome */
	/* L'axiome est partage' entre tous les arbres, on ne le compte pas, il sera ajoute' en fin */
	Aijk = start_symbol_id;
      }
      else {
	XxY_set (&AijXid, Aij, k, &Aijk);
      }

      Pij = -dedication_ptr->PijK;
      SXBA_1_bit (Pij2rankset [Pij], k);
      signature = dedication_ptr->signature;
      bot = XH_X(signatures_hd, signature);

      item = spf.outputG.lhs [Pij].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	if (Xpq < 0)
	  XH_push (XH_rhs, Xpq);
	else {
#if 0
	  if (get_dedication_top (Xpq) == (SXINT) spf.tree_count.nt2nb [Xpq]) {
	    /* sature' */
	    if ((Xpqk = Aij_saturated2Aij_id [Xpq]) == 0) {
	      /* pas encore calcule' */
	      Xpq_rankset = Aij2rankset [Xpq];

	      for (i = 1; i <= top_k; i++) {
		SXBA_1_bit (Xpq_rankset, i);
		XH_push (XH_nbest_set_hd, i);
	      }

	      XH_set (&XH_nbest_set_hd, &id);
	      Aij_saturated2id [Xpq] = id;
	      XxY_set (&AijXid, Xpq, id, &Xpq_id);
	      Aij_saturated2Aij_id [Xpq] = Xpqk = Xpq_id;
	    }
	  }
	  else 
#endif /* 0 */
	    {
	    i = XH_list_elem(signatures_hd, bot);
	    XxY_set (&AijXid, Xpq, i, &Xpqk);
	  }

	  XH_push (XH_rhs, Xpqk);
	}

	bot++;
      }

      XH_set (&XH_rhs, &rhs);

      XxY_set (&nbest_forest_hd, Aijk, rhs, &Pijk);

      if (XxY_set (&AijXrhs, Aij, rhs, &couple))
	/* Multiple prods with identical Aij in lhs and identical rhs */
	SXBA_1_bit (multiple_AijXrhs_set, couple);

      bot = XH_X (XH_rhs, rhs);
      top = XH_X (XH_rhs, rhs+1);

      while (bot < top) {
	rhs_Xpq = XH_list_elem (XH_rhs, bot);

	if (rhs_Xpq > 0)
	  /* Un rhs_Xpq = (Xpq, k) correspond a une prod unique :
	     Il y a au plus un Xpq par arbre */
	  Aij_id2prod [rhs_Xpq] = Pijk;

	bot++;
      }
    }
  }
  
  return 1;
}

#if 0
static SXINT
td_Aij_pre_first_pb_pass (SXINT Aij)
{
  SXINT             rank, k, Pij;
  SXINT             Aijk, item, Xpq, Xpqk, rhs, Pijk, couple, bot, top, rhs_Xpq ;
  SXBA              Aij_rankset;
  struct dedication *dedication_ptr;

  Aij_rankset = Aij2rankset [Aij];

  rank = k = 0;

  while ((k = sxba_scan (Aij_rankset, k)) > 0) {
    if (Aij == spf.outputG.start_symbol) {
      /* On est sur une Pij qui definit l'axiome */
      /* L'axiome est partage' entre tous les arbres, on ne le compte pas, il sera ajoute' en fin */
      Aijk = start_symbol_id;
    }
    else {
      XxY_set (&AijXid, Aij, k, &Aijk);
    }

    rank++;
    dedication_ptr = get_dedication_ptr (Aij, rank);

#if EBUG
    if (dedication_ptr == NULL)
      sxtrap (ME, "td_Aij_pre_first_pb_pass");
#endif /* EBUG */

    Pij = -dedication_ptr->PijK;
    SXBA_1_bit (Pij2rankset [Pij], k);

    item = spf.outputG.lhs [Pij].prolon;

    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      if (Xpq < 0)
	XH_push (XH_rhs, Xpq);
      else {
	SXBA_1_bit (Aij2rankset [Xpq], k);
	XxY_set (&AijXid, Xpq, k, &Xpqk);
	XH_push (XH_rhs, Xpqk);
      }
    }

    XH_set (&XH_rhs, &rhs);

    XxY_set (&nbest_forest_hd, Aijk, rhs, &Pijk);

    if (XxY_set (&AijXrhs, Aij, rhs, &couple))
      /* Multiple prods with identical Aij in lhs and identical rhs */
      SXBA_1_bit (multiple_AijXrhs_set, couple);

    bot = XH_X (XH_rhs, rhs);
    top = XH_X (XH_rhs, rhs+1);

    while (bot < top) {
      rhs_Xpq = XH_list_elem (XH_rhs, bot);

      if (rhs_Xpq > 0)
	/* Un rhs_Xpq = (Xpq, k) correspond a une prod unique :
	   Il y a au plus un Xpq par arbre */
	Aij_id2prod [rhs_Xpq] = Pijk;

      bot++;
    }

  

#ifdef MEASURES
    {
      SXINT init_prod, item, X;
   
      if (Aij == spf.outputG.start_symbol) {
	/* On est sur une Pij qui definit l'axiome */
	/* L'axiome est partage' entre tous les arbres, on ne le compte pas, il sera ajoute' en fin */
      }
      else {
	baselineG.N++;
      }

      baselineG.P++;
      baselineG.G++; /* La lhs */
      init_prod = spf.outputG.lhs [Pij].init_prod;
      item = spf.inputG.prolon [init_prod];

      while ((X = spf.inputG.lispro [item++])) {
	baselineG.G++;

	if (X < 0)
	  /* On decompte separement chaque occur de chaque terminal */
	  baselineG.T++;
      }
    }
#endif /* MEASURES */
  }
  
  return 1;
}
#endif /* 0 */

#ifdef MEASURES
/* Pij appartient au kth arbre dont l'id est kth_id */
static void
fill_unfolded_forest (SXINT Pij, SXFLOAT weight)
{
  SXINT Aij, init_prod, item, X;

  Aij = spf.outputG.lhs [Pij].lhs;
   
  if (Aij == spf.outputG.start_symbol) {
    /* On est sur une Pij qui definit l'axiome */
    /* L'axiome est partage' entre tous les arbres, on ne le compte pas, il sera ajoute' en fin */
  }
  else {
    baselineG.N++;
  }

  baselineG.P++;
  baselineG.G++; /* La lhs */
  init_prod = spf.outputG.lhs [Pij].init_prod;
  item = spf.inputG.prolon [init_prod];

  while ((X = spf.inputG.lispro [item++])) {
    baselineG.G++;

    if (X < 0)
      /* On decompte separement chaque occur de chaque terminal */
      baselineG.T++;
  }

}
#endif /* MEASURES */


#if 0
/* Ca ne marche pas car le changement en RHS des Aij^k1 et Aij^k2 en Aij^(k1 U k2) font qu'il sera possible d'atteindre
   le hieme best tree par un chemin interdit */
/* Nouvelle version : Soit (Aij, {k1, ..., kp}) ou les k sont des rank sets.
   Plutot que de generer les Aij^k1-prods, ..., Aij^kp-prods on va essayer de regrouper certains k.
   Supposons que les Aij^k1-prods selectionnent les prods instanciees {p1, ..., pm} et que les Aij^k1-prods
   selectionnent exactement le meme ensemble {p1, ..., pm}.  Alors il est possible de regrouper k1 et k2 et de fabriquer
   les Aij^(k1 U k2)-prods. */

static SXBA      rank_set, Aij_Pij_set;
static XH_header XH_Pij_set;
static SXINT     *XH_Pij_set2Aij_id;


static void
XH_Pij_set_oflw (SXINT old_size, SXINT new_size)
{
  XH_Pij_set2Aij_id = (SXINT *) sxrealloc (XH_Pij_set2Aij_id, new_size+1, sizeof (SXINT));
}


static SXINT
generate_top_down_nbest_forest (SXINT Pij)
{
  if (!SXBA_bit_is_set (Pij_set, Pij))
    /* Pij ne fait partie d'aucun des nbest arbres */
    return 0;

  SXBA_1_bit (Aij_Pij_set, Pij);

  return 1;
}

/* Toutes les Aij-prods sont ds Aij_Pij_set */
static SXINT
generate_top_down_nbest_forest_post (SXINT Aij, SXBOOLEAN must_be_kept)
{
  SXINT Aij_id, id, bot, top, cur, i, Pij, Pij_set_id, prev_Aij_id, prev_id, new_id, new_Aij_id;

  XxY_Xforeach (AijXid, Aij, Aij_id) {
    /* A chaque Aij_id on associe le sous-ensemble de Aij_Pij_set qui correspond a un rank set non vide */
    id = XxY_Y (AijXid, Aij_id);
    bot = XH_X (XH_nbest_set_hd, id);
    top = XH_X (XH_nbest_set_hd, id+1);

    for (cur = bot; cur < top; cur++) {
      i = XH_list_elem (XH_nbest_set_hd, cur);
      SXBA_1_bit (rank_set, i);
    }

    Pij = 0;

    while ((Pij = sxba_scan (Aij_Pij_set, Pij)) > 0) {
      if (sxba_2op (NULL, SXBA_OP_IS, Pij2rankset [Pij], SXBA_OP_AND, rank_set)) {
	/* Non vide => Pij est valide pour (Aij, id) */
	XH_push (XH_Pij_set, Pij);
      }
    }

    if (XH_set (&XH_Pij_set, &Pij_set_id)) {
      /* Ce n'est pas la 1ere fois que l'on trouve cet ensemble de Pij */
      prev_Aij_id = XH_Pij_set2Aij_id [Pij_set_id];
      prev_id = XxY_Y (AijXid, prev_Aij_id);

      /* On peut fusionner prev_id et id */
      bot = XH_X (XH_nbest_set_hd, prev_id);
      top = XH_X (XH_nbest_set_hd, prev_id+1);

      for (cur = bot; cur < top; cur++) {
	i = XH_list_elem (XH_nbest_set_hd, cur);
	SXBA_1_bit (rank_set, i);
      }

      i = 0;

      while ((i = sxba_scan_reset (rank_set, i)) > 0) {
	XH_push (XH_nbest_set_hd, i);
      }
  
      XH_set (&XH_nbest_set_hd, &new_id);

      XxY_set (&AijXid, Aij, new_id, &new_Aij_id); /* Compatible avec le XxY_Xforeach (AijXid, Aij, Aij_id) */

      replace_prod (Aij_id, new_Aij_id); /* Remplace toutes les prods ayant Aij_id en rhs par new_Aij_id */
      replace_prod (prev_Aij_id, new_Aij_id); /* Remplace toutes les prods ayant Aij_id en rhs par new_Aij_id */

      XxY_erase (AijXid, Aij_id);
      XxY_erase (AijXid, prev_Aij_id);

      XH_Pij_set2Aij_id [Pij_set_id] = new_Aij_id;
    }
    else {
      XH_Pij_set2Aij_id [Pij_set_id] = Aij_id;
      sxba_empty (rank_set);
    }
  }

  sxba_empty (Aij_Pij_set);
  XH_clear (&XH_Pij_set);
}



#endif /* 0 */

static SXINT
generate_nbest_Pij (SXINT Pij, SXINT lhs_id, SXBA nbest_set)
{
  SXINT      Aij, Aij_id, id, item, Xpq, rhs_Xpq, rhs, prod, i, bot, cur, top, rhs_id, couple;
  SXBOOLEAN  is_empty;

  Aij = spf.outputG.lhs [Pij].lhs;

  if (lhs_id) {
    Aij_id = lhs_id;
    id = XxY_Y (AijXid, Aij_id);

    /* On doit faire l'intersection entre id et nbest_set */
    is_empty = SXTRUE;
    bot = XH_X (XH_nbest_set_hd, id);
    top = XH_X (XH_nbest_set_hd, id+1);

    for (cur = bot; cur < top; cur++) {
      i = XH_list_elem (XH_nbest_set_hd, cur);

      if (SXBA_bit_is_set (nbest_set, i)) {
	is_empty = SXFALSE;
	XH_push (XH_nbest_set_hd, i);
      }
    }

    if (is_empty)
      return 0;
  }
  else {
    XxY_set (&AijXid, Aij, 0, &Aij_id);

    i = 0;

    while ((i = sxba_scan (nbest_set, i)) > 0) {
      XH_push (XH_nbest_set_hd, i);
    }
  }

  XH_set (&XH_nbest_set_hd, &rhs_id);

  item = spf.outputG.lhs [Pij].prolon;

  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    if (Xpq < 0)
      XH_push (XH_rhs, Xpq);
    else {
      XxY_set (&AijXid, Xpq, rhs_id, &rhs_Xpq);
      XH_push (XH_rhs, rhs_Xpq);
    }
  }

  XH_set (&XH_rhs, &rhs);

  XxY_set (&nbest_forest_hd, Aij_id, rhs, &prod);

  if (XxY_set (&AijXrhs, Aij, rhs, &couple))
    /* Multiple prods with identical Aij in lhs and identical rhs */
    SXBA_1_bit (multiple_AijXrhs_set, couple);

  bot = XH_X (XH_rhs, rhs);
  top = XH_X (XH_rhs, rhs+1);

  while (bot < top) {
    rhs_Xpq = XH_list_elem (XH_rhs, bot);

    if (rhs_Xpq > 0)
      /* Un rhs_Xpq = (Xpq, k) correspond a une prod unique :
	 Il y a au plus un Xpq par arbre */
      Aij_id2prod [rhs_Xpq] = prod;

    bot++;
  }

  return prod;
}

/* A chaque Aij est associe un ensemble {N1, N2, ...} de sous-ensembles disjoints de {i | 1<=i<=nbest} */
/* Si la prod Pij : Aij -> X1 X2 ... est valide pour N sous-ensemble de {i | 1<=i<=nbest}
   Si M = Np inter N est non vide on genere
   (Aij, Np) -> (X1, M) (X2, M) ...
 */

static SXINT
generate_nbest_forest (SXINT Pij)
{
  SXINT Aij, Aij_id, id, new_id;
  SXBA  Pij_nbest_set, result_set;


  if (!SXBA_bit_is_set (Pij_set, Pij))
    /* Pij ne fait partie d'aucun des nbest arbres */
    return 0; 

  Pij_nbest_set = Pij2rankset [Pij];
  Aij = spf.outputG.lhs [Pij].lhs;

  if (Aij == spf.outputG.start_symbol) {
    /* prod definissant l'axiome */
    /* Il faut fabriquer l'ensemble complet */
    generate_nbest_Pij (Pij, 0, Pij_nbest_set);
  }
  else {
    XxY_Xforeach (AijXid, Aij, Aij_id) {
      generate_nbest_Pij (Pij, Aij_id, Pij_nbest_set);
    }
  }

  return 1;
}



SXINT traite_Pij(SXINT Pij)
{
  return 1;
}




/*---------------------------------*/
/* code (an) */
/*---------------------------------*/

int affiche_Aij(int Aij)
{
  if(Aij > 0)
    printf("%s[%d..%d]", spf.inputG.ntstring[Aij2A(Aij)], Aij2i(Aij), Aij2j(Aij));
  else
/*     printf("Tij"); */
    printf("%s[%d..%d]", spf.inputG.tstring[Tij2T(-Aij)], Tij2i(-Aij), Tij2j(-Aij)); 
  return 1;
}


int affiche_Pij(int Pij){
  SXINT Aij, Bik, Ckj;
  SXINT i,k,j;
  char *A_str, *B_str, *C_str;
  struct lhs *plhs;
  struct rhs *prhs;
  SXINT P,l;
  
  Aij = spf.outputG.lhs [Pij].lhs;
  plhs = spf.outputG.lhs+Pij; /* on accede a l'entree de la production dans lhs */
  prhs = spf.outputG.rhs+plhs->prolon; /* on accede a la partie droite de prod dans rhs */
  Bik = (prhs)->lispro;
  Ckj = (prhs + 1)->lispro;

  P = spf.outputG.lhs[Pij].init_prod;
  l = spf.inputG.prolon[P+1] - spf.inputG.prolon[P] - 1;
  
  affiche_Aij(Aij); 
  printf(" -> ");
  affiche_Aij(Bik); 
  if(l > 1){
    printf(" ");
    affiche_Aij(Ckj); 
  }
  return 1;
}


RECTANGLE *decompose_matrice_x(SXINT k_min, SXINT k_max, SXINT **mat, SXINT XMAX, SXINT YMAX);
RECTANGLE *decompose_matrice_y(SXINT k_min, SXINT k_max, SXINT **mat, SXINT XMAX, SXINT YMAX);
void affiche_rectangles(RECTANGLE *l);
SXINT nombre_rectangles(RECTANGLE *l);
void free_rectangles(RECTANGLE *l);

/* etant donne un Pij ayant la forme suivante : Aij -> Bik Ckj            */
/* la fonction remplit une matrice mat decrivant la maniere de construire */
/* les n meilleures analyses ayant Aij pour racine. mat[x][y] = k         */
/* s'SXINTerprete de la facon suivante : en combinant la x-ieme meilleure   */
/* analyse ayant Bik pour racine et la y ieme meilleure analyse ayant     */
/* Ckj pour racine, on obtient la k ieme meilleure analyse ayant Aij pour */
/* racine */

SXINT remplis_matrices_Pij(SXINT Aij);


/* permet de savoir comment consturire les [m,n] meilleures analyses de Pij */
/* en d'autres termes il s'agit de determiner m',n', m'' et n'' dans        */
/* l'equation suivante : Aij[m,n] -> Cik[m',n'] Bkj[m'',n''] */

void Pij_get_m_n_best(SXINT Pij, SXINT m, SXINT n);


/* permet de distribuer les [m,n] meilleures analyses de Aij sur ses Pij  */
/* en d'autres termes, si on a                                            */
/* Aij : Pij                                                              */
/* Aij : P'ij                                                             */
/* on veut déterminer m,n,m',n' dans les equations suivantes              */
/* Aij[m,n] : Pij[m',n']                                                  */
/* Aij[m,n] : P'ij[m'',n'']                                               */

void Aij_get_m_n_best(SXINT Aij, SXINT m, SXINT n); 

/*------------------------------------------------------------------------------------*/
/* decomposition d'une matrice nbest en rectangles                                    */
/* on construit une collection de rectangles [x_min .. x_max] x [y_min .. ymax]       */
/* qui couvre les cases mat[x][y] de la matrice mat telles que                        */
/* k_min <= mat[x][y] <= k_max                                                        */
/*------------------------------------------------------------------------------------*/

RECTANGLE *decompose_matrice_x(SXINT k_min, SXINT k_max, SXINT **mat, SXINT XMAX, SXINT YMAX)
{
  SXINT x,y;
  SXINT y_min, y_max;
  RECTANGLE *head = NULL;
  RECTANGLE *current = NULL;
  RECTANGLE *p = NULL;
  SXINT rectangle_en_cours = 0;
  SXINT EBUG_FCT = 0;

  EBUG_FCT && printf("decompose_matrice_x(%d, %d, mat, %d, %d)\n", k_min, k_max, XMAX, YMAX);
  /* on parcours les colonnes */
  for(x=1; x <= XMAX; x++){
    EBUG_FCT && fprintf(stderr, "x = %d\n", x);
    for(y=1; ((mat[x][y] < k_min) || (mat[x][y] > k_max)) && (y <= YMAX) && (mat[x][y] != INVALIDE); y++);

    if((y > YMAX) || (mat[x][y] == INVALIDE)){ 
      /* on n'a pas trouve une case x y telle que k_min <= mat[x][y] <= k_max */ 
      EBUG_FCT && fprintf(stderr, "on a rien trouve dans la colonne\n");
      if(rectangle_en_cours){
	EBUG_FCT && fprintf(stderr, "on ferme le rectangle en cours\n");
	current->x_max = x-1;
	rectangle_en_cours = 0;
      }
    }
 

    else {
      /*     if(((mat[x][y] >= k_min) && (mat[x][y] <= k_max))){ */
      /* on a trouve une case mat[x][y] telle que k_min <= mat[x][y] <= k_max */ 
      y_min = y;
      for(; (y <= YMAX) && (mat[x][y] <= k_max) && (mat[x][y] != INVALIDE); y++);
      y_max = y-1;
      /*  k_min <= mat[x][y_min] et mat[x][y_max] <= k_max */
      EBUG_FCT && fprintf(stderr, "on a trouve une zone SXINTeressante [%d .. %d]\n", y_min, y_max);
      if(rectangle_en_cours){
	/* il existe un rectangle en cours de construction */
	EBUG_FCT && fprintf(stderr, "il y a un rectangle en cours\n");
	if((y_min != current->y_min) || (y_max != current->y_max)){
	  /* le rectangle en cours de construction n'a pas les memes bornes y_min et y_max */
	  /* on le ferme donc */
	  EBUG_FCT && fprintf(stderr, "on le ferme\n");
	  current->x_max = x-1;
	  rectangle_en_cours = 0;
	}
      }
      if(!rectangle_en_cours){
	/* il n'existe pas de rectangle en cours de construction */
	/* on en cree un nouveau */
	EBUG_FCT && fprintf(stderr, "il n'y a pas de rectangle en cours -> on en cree un nouveau\n");
	if((p = malloc(sizeof(RECTANGLE))) == NULL){
	  fprintf(stderr, "erreur d'allocation, on ferme!\n");
	  exit;
	}
	if(!head)
	  current = head = p;
	else
	  current = current->suiv = p;
	current->x_min = x;
	current->y_min = y_min;
	current->y_max = y_max;
	current->suiv = NULL;
	rectangle_en_cours = 1;
      }
    }
    

 }
  if(rectangle_en_cours){
    /* il y avait un rectangle en cours de construction, on le termine */
    EBUG_FCT && fprintf(stderr, "on ferme le rectangle en cours\n");
    current->x_max = x-1;
  }
  return head;
}
/*------------------------------------------------------------------------------------*/

RECTANGLE *decompose_matrice_y(SXINT k_min, SXINT k_max, SXINT **mat, SXINT XMAX, SXINT YMAX)
{
  SXINT x,y;
  SXINT x_min, x_max;
  RECTANGLE *head = NULL;
  RECTANGLE *current = NULL;
  RECTANGLE *p = NULL;
  SXINT rectangle_en_cours = 0;
  SXINT EBUG_FCT = 0;


  /* on parcours les colonnes */
  for(y=1; y <= YMAX; y++){
    EBUG_FCT && fprintf(stderr, "y = %d\n", y);
    for(x=1; (x <= XMAX) && ((mat[x][y] < k_min) || (mat[x][y] > k_max)) && (mat[x][y] != INVALIDE); x++);


    if((x > XMAX) || (mat[x][y] == INVALIDE)){ 
      /* on n'a pas trouve une case x y telle que k_min <= mat[x][y] <= k_max */ 
      EBUG_FCT && fprintf(stderr, "on a rien trouve dans la ligne\n");
      if(rectangle_en_cours){
	EBUG_FCT && fprintf(stderr, "on ferme le rectangle en cours\n");
	current->y_max = y-1;
	rectangle_en_cours = 0;
      }
    }





    else{ 
      /*     if((mat[x][y] >= k_min) & (mat[x][y] <= k_max)){ */
      /* on a trouve une case x y telle que k_min <= mat[x][y] <= k_max */ 
      x_min = x;
      for(; (x <= XMAX) && (mat[x][y] <= k_max) && (mat[x][y] != INVALIDE); x++);
      x_max = x-1;
      /*  k_min <= mat[x][y_min] et mat[x][y_max] <= k_max */
      EBUG_FCT && fprintf(stderr, "on a trouve une zone SXINTeressante [%d .. %d]\n", x_min, x_max);
      if(rectangle_en_cours){
	/* il existe un rectangle en cours de construction */
	EBUG_FCT && fprintf(stderr, "il y a un rectangle en cours\n");
	if((x_min != current->x_min) || (x_max != current->x_max)){
	  /* le rectangle en cours de construction n'a pas les memes bornes x_min et x_max */
	  /* on le ferme donc */
	  EBUG_FCT && fprintf(stderr, "on le ferme\n");
	  current->y_max = y-1;
	  rectangle_en_cours = 0;
	}
      }
      if(!rectangle_en_cours){
	/* il n'existe pas de rectangle en cours de construction */
	/* on en cree un nouveau */
	  EBUG_FCT && fprintf(stderr, "il n'y a pas de rectangle en cours -> on en cree un nouveau\n");
	  if((p = malloc(sizeof(RECTANGLE))) == NULL){
	    fprintf(stderr, "erreur d'allocation, on ferme!\n");
	    exit;
	  }
	  if(!head)
	    current = head = p;
	  else
	    current = current->suiv = p;
	  current->y_min = y;
	  current->x_min = x_min;
	  current->x_max = x_max;
	  current->suiv = NULL;
	  rectangle_en_cours = 1;
      }
    }


  }
  if(rectangle_en_cours){
    /* il y avait un rectangle en cours de construction, on le termine */
    current->y_max = y-1;
  }
  return head;
}


/*------------------------------------------------------------------------------------*/

void affiche_rectangles(RECTANGLE *l)
{
  if(l){
    printf("(X[%d, %d] x  Y[%d, %d]), ", l->x_min, l->x_max, l->y_min, l->y_max);
    affiche_rectangles(l->suiv);
  }
}

SXINT nombre_rectangles(RECTANGLE *l)
{
  RECTANGLE *l2;
  SXINT n = 0;
  for(l2=l; l2; l2 = l2->suiv)
    n++;
  return n;
}

void free_rectangles(RECTANGLE *l){
  if(l){
    free_rectangles(l->suiv);
    free(l);
  }
}

/*------------------------------------------------------------------------------------*/

/*---------------------------------*/
/* remplissage des matrices des Pij */
/*---------------------------------*/
SXINT remplis_matrices_Pij(SXINT Aij)
{
  SXINT k=0;
  struct dedication *dedication_ptr;
  SXINT signature;
  SXINT Pij;
  SXINT bot;
  SXINT i,j;
  SXINT hook; 


  EBUG && printf("remplis matrice Pij (");
  EBUG && affiche_Aij(Aij);
  EBUG && printf(")\n");

  while(dedication_ptr = get_dedication_ptr(Aij, ++k)){
    Pij = -dedication_ptr->PijK;
    signature = dedication_ptr->signature;
    bot = XH_X(signatures_hd, signature);
    i = XH_list_elem(signatures_hd, bot);
    j = XH_list_elem(signatures_hd, bot + 1);
    if(i > Pij_i_max[Pij]) Pij_i_max[Pij] = i;
    if(j > Pij_j_max[Pij]) Pij_j_max[Pij] = j;
  }

/*   Pij_i_max[Pij]++; */
/*   Pij_j_max[Pij]++; */

  k = 0;
  while(dedication_ptr = get_dedication_ptr(Aij, ++k)){
    Pij = -dedication_ptr->PijK;
    signature = dedication_ptr->signature;
    bot = XH_X(signatures_hd, signature);
    i = XH_list_elem(signatures_hd, bot);
    j = XH_list_elem(signatures_hd, bot + 1);
    if(Pij > 0){
      if(Pij_mat[Pij] == NULL){
	Pij_mat[Pij] = alloue_mat( Pij_i_max[Pij] + 1,  Pij_j_max[Pij] + 1);
      }
      Pij_mat[Pij][i][j] = ++Pij_or[Pij];
      EBUG && printf("Pij_mat[%d] [%d][%d] = %d\n", Pij, i, j, Pij_or[Pij]); 
    }
  } 
  

  /* stocke la position relative d'un Pij dans la liste */
  /* des differents Pij d'un Aij */
  hook = GET_HOOK(Aij);
  i = 0;
  while ((Pij = spf.outputG.rhs [hook++].lispro) != 0) {
    if(Pij > 0){
      Pij_rel[Pij] = i++;
    }
  }
  return 1;
}

/*------------------------------------------------------------------------------------*/

void Pij_get_m_n_best(SXINT Pij, SXINT m, SXINT n)
{
  RECTANGLE *l1, *l2, *l;
  SXINT indice;
  SXINT Aij; /* partie gauche de Pij */
  SXINT Bik; /* premier symbole de la partie droite de Pij */
  SXINT Ckj; /* deuxieme symbole de la partie droite de Pij */
  struct rhs *prhs, *p;
  struct lhs *plhs;
  SXINT Aij_nb;
  SXINT P;

  EBUG && printf("Pij_get_m_n_best(");
  EBUG && affiche_Pij(Pij);
  EBUG && printf(", %d, %d)\n", m, n);

  P = spf.outputG.lhs[Pij].init_prod;
  Aij_nb = spf.inputG.prolon[P+1] - spf.inputG.prolon[P] - 1;
  
  plhs = spf.outputG.lhs+Pij; /* on accede a l'entree de la production dans lhs */
  Aij = plhs->lhs; /* on recupere le sym de la partie gauche */
  prhs = spf.outputG.rhs+plhs->prolon; /* on accede a la partie droite de prod dans rhs */
  Bik = (prhs)->lispro;


  if(Aij_nb == 1){ /* on est sur une regle unaire */
    Aij_get_m_n_best(Bik, m,n);
    XxYxZ_set(&Pij_m_n_hd, Pij, m, n, &indice);
    Pij_m_n[indice] = NULL;
  }
  else{

    Ckj = (prhs + 1)->lispro;

    l1 = decompose_matrice_x(m, n, Pij_mat[Pij], Pij_i_max[Pij], Pij_j_max[Pij]);
    l2 = decompose_matrice_y(m, n, Pij_mat[Pij], Pij_i_max[Pij], Pij_j_max[Pij]);
  
    /* heuristique un peu naive */
    if(nombre_rectangles(l1) < nombre_rectangles(l2)){
      free_rectangles(l2);
      l = l1;
    }
    else{
      free_rectangles(l1);
      l = l2;
    }

    XxYxZ_set(&Pij_m_n_hd, Pij, m, n, &indice);
    Pij_m_n[indice] = l;

    for(l1 = l; l1 != NULL; l1 = l1->suiv){
      Aij_get_m_n_best(Bik, l1->x_min, l1->x_max);
      Aij_get_m_n_best(Ckj, l1->y_min, l1->y_max);
    }
  }
}

/*------------------------------------------------------------------------------------*/

void Aij_get_m_n_best(SXINT Aij, SXINT m, SXINT n)
{
  SXINT i,Pij_relatif;
  SXINT x,y;
  couple_SXINT *table_couples;
  SXINT indice;
  SXINT Pij_nb = 0;
  SXINT hook;
  struct dedication *dedication_ptr;
  SXINT signature;
  SXINT Pij;
  SXINT bot;
  SXINT ordre_relatif;

  EBUG && printf("Aij_get_m_n_best(");
  EBUG && affiche_Aij(Aij);  
  EBUG && printf(", %d, %d)\n",m,n);

  if(Aij < 0)
    return;

  for(hook = GET_HOOK(Aij); Pij = spf.outputG.rhs [hook].lispro; hook++)
    if(Pij > 0)
      Pij_nb++;

  /* on est sur un symbole terminal */
  /*  if(Pij_nb == 0){ 
    EBUG && printf("symbole terminal\n");
    return;
    }*/

  /* <A,i,j,m,n> existe deja, on ne fait rien */
  XxYxZ_set(&Aij_m_n_hd, Aij, m, n, &indice);
  if(Aij_m_n[indice]){
    EBUG && printf("l'entree existe deja\n");
    return;
  }

  
  table_couples = malloc(Pij_nb * sizeof(couple_SXINT));
  for(i=0; i < Pij_nb; i++){
    table_couples[i].x = table_couples[i].y = -1;
  }
 
  for(i=m; (dedication_ptr = get_dedication_ptr(Aij, i)) && (i <= n); i++){
    Pij = -dedication_ptr->PijK;
    signature = dedication_ptr->signature;
    bot = XH_X(signatures_hd, signature);
    x = XH_list_elem(signatures_hd, bot);
    y = XH_list_elem(signatures_hd, bot + 1);
    ordre_relatif = Pij_mat[Pij][x][y];

    Pij_relatif = Pij_rel[Pij];

    if(table_couples[Pij_relatif].x == -1)
      table_couples[Pij_relatif].x = ordre_relatif;    
    if(table_couples[Pij_relatif].y < ordre_relatif)
      table_couples[Pij_relatif].y = ordre_relatif;
  }


  /*  printf("[%d .. %d] = ", m, n);
  for(i= 0 ; i < Pij_nb; i++){
    printf("[%d ..%d] ", table_couples[i].x, table_couples[i].y);
  }
  printf("\n");*/

  
  XxYxZ_set(&Aij_m_n_hd, Aij, m, n, &indice); 
  Aij_m_n[indice] = table_couples;
  
  i = 0;
  for(hook = GET_HOOK(Aij); Pij = spf.outputG.rhs [hook].lispro; hook++){
    if(Pij > 0){
      if((table_couples[i].x != INVALIDE) && (table_couples[i].y != INVALIDE)){
	Pij_get_m_n_best(Pij, table_couples[i].x, table_couples[i].y);
	i++;
      }
    }
  }
}


void affiche_foret_reduite(void){
  SXINT indice, indice2, Aij, Pij, Bik, Ckj;
  SXINT i,k,j,m,n, pos;
  char *A_str, *B_str, *C_str;
  RECTANGLE *liste_rectangles, *rectangle;
  couple_SXINT *table_couples;
  SXINT hook;
  SXINT rhs_length, X;
  struct lhs *plhs;
  struct rhs *prhs;

  XxYxZ_foreach(Aij_m_n_hd, indice){
    table_couples = Aij_m_n[indice];
    Aij = XxYxZ_X(Aij_m_n_hd, indice);
    m = XxYxZ_Y(Aij_m_n_hd, indice);
    n = XxYxZ_Z(Aij_m_n_hd, indice);

    i = Aij2i(Aij);
    j = Aij2j(Aij);
    A_str = spf.inputG.ntstring[Aij2A(Aij)];
/*     printf("-------------------------------------------\n");  */
/*     affiche_Aij(Aij); */
/*     printf("\n-------------------------------------------\n");  */
	
    for(hook = GET_HOOK(Aij), pos=0; Pij = spf.outputG.rhs [hook].lispro; hook++){
      if(Pij > 0){

	X = spf.outputG.lhs[Pij].init_prod;
	rhs_length = spf.inputG.prolon[X+1] - spf.inputG.prolon[X] - 1;

	plhs = spf.outputG.lhs+Pij; /* on accede a l'entree de la production dans lhs */
	prhs = spf.outputG.rhs+plhs->prolon; /* on accede a la partie droite de prod dans rhs */
	
	Bik = (prhs)->lispro;

	if(Bik > 0){
	  B_str = spf.inputG.ntstring[Aij2A(Bik)];
	  k = Aij2j(Bik);
	}
	else{
  	  B_str = spf.inputG.tstring[-Tij2T(-Bik)];  
/*  	  B_str = sxstrget (idag.p2ff_ste [Tij2i (-Bik)]);  */
	  k = Tij2j(-Bik);

	}

	if(rhs_length == 1){
 	  printf("%s[%d..%d][%d..%d] ->\tt[%d..%d][%d..%d]\n", A_str, i, j, m,n , i,j,m,n);  
/*  	  printf("%s[%d..%d][%d..%d] ->\t%s[%d..%d][%d..%d]\n", A_str, i, j, m,n , B_str, i,j,m,n);   */
	}

	Ckj = (prhs + 1)->lispro;
	C_str = spf.inputG.ntstring[Aij2A(Ckj)];


	
	XxYxZ_set(&Pij_m_n_hd, Pij, table_couples[pos].x, table_couples[pos].y, &indice2);
	liste_rectangles = Pij_m_n[indice2];


	for(rectangle = liste_rectangles; rectangle; rectangle = rectangle->suiv){
	  printf("%s[%d..%d][%d..%d] ->", A_str, i, j, m, n);
	  printf("\t%s[%d..%d][%d..%d]", B_str, i, k, rectangle->x_min, rectangle->x_max);
	  printf("\t%s[%d..%d][%d..%d]\n", C_str, k, j, rectangle->y_min, rectangle->y_max);
	}
	pos++;
      }
    }
  }
}

void affiche_measures(void){
  SXINT indice, indice2, Aij, Pij, Bik, Ckj;
  SXINT i,k,j,m,n, pos;
  char *A_str, *B_str, *C_str;
  RECTANGLE *liste_rectangles, *rectangle;
  couple_SXINT *table_couples;
  SXINT hook;
  struct lhs *plhs;
  struct rhs *prhs;
  SXINT N = 0;
  SXINT T = 0;
  SXINT P = 0;
  SXINT G = 0;
  SXINT rhs_length;
  SXINT X;

  XxYxZ_foreach(Aij_m_n_hd, indice){
    table_couples = Aij_m_n[indice];
    Aij = XxYxZ_X(Aij_m_n_hd, indice);
    m = XxYxZ_Y(Aij_m_n_hd, indice);
    n = XxYxZ_Z(Aij_m_n_hd, indice);

    i = Aij2i(Aij);
    j = Aij2j(Aij);

    N++;
    
    for(hook = GET_HOOK(Aij), pos=0; Pij = spf.outputG.rhs [hook].lispro; hook++){
      if(Pij > 0){

	X = spf.outputG.lhs[Pij].init_prod;
	rhs_length = spf.inputG.prolon[X+1] - spf.inputG.prolon[X] - 1;


	XxYxZ_set(&Pij_m_n_hd, Pij, table_couples[pos].x, table_couples[pos].y, &indice2);
	liste_rectangles = Pij_m_n[indice2];

	plhs = spf.outputG.lhs+Pij; /* on accede a l'entree de la production dans lhs */
	prhs = spf.outputG.rhs+plhs->prolon; /* on accede a la partie droite de prod dans rhs */

	Bik = (prhs)->lispro;
	B_str = spf.inputG.ntstring[Aij2A(Bik)];
	k = Aij2j(Bik);

	Ckj = (prhs + 1)->lispro;
	C_str = spf.inputG.ntstring[Aij2A(Ckj)];

	
	if(rhs_length == 1){
	  P++;
	  G += 2;
	  if(Bik < 0) T++;
	  else N++;
	}


	
	for(rectangle = liste_rectangles; rectangle; rectangle = rectangle->suiv){
	  G += rhs_length + 1;
	  P++;
	}
	pos++;
      }
    }
  }
  printf("MEASURES: rectangles = (|N|=%d, |T|=%d, |P|=%d, |G|=%d)\n",N, T, P, G); 
}


#if 0
static SXINT
exact_nbest_sem_pass (void)
{
  SXINT    kind;

  if (pb) {
    if (is_print_time)
      sxtime (SXACTION, "\tExact-N-best - pb (start)");
    
#if MEASURES
    spf_print_signature (stdout, "MEASURES: prunedG (goldG)");
#endif /* MEASURES */
    
    /* kth est global */
    for (kth = 1; kth <= exact_nbest; kth++)
      nbest_seek_kth_best_tree (spf.outputG.start_symbol, kth, fill_kth_best_tree);
    
    if (is_print_time)
      sxtime (SXACTION, "\t\tReading unfolded trees (done)");

#ifdef MEASURES
    printf ("MEASURES: baselineG = (|N|=%ld, |T|=%ld, |P|=%ld, |G|=%ld)\n", baselineG.N+1 /* l'axiome */, baselineG.T, baselineG.P, baselineG.G);
#endif /* MEASURES */

#if 0
    Aij_Pij_set = sxba_calloc (spf.outputG.maxxprod+1);
    rank_set = sxba_calloc (exact_nbest+1);
    XH_alloc (&XH_Pij_set, "XH_Pij_set", (spf.outputG.maxxprod/spf.outputG.maxxnt)+1, 1, (exact_nbest/(spf.outputG.maxxprod/spf.outputG.maxxnt))+1, XH_Pij_set_oflw, statistics);
    XH_Pij_set2Aij_id = (SXINT *) sxalloc (XH_size (XH_Pij_set)+1, sizeof (SXINT));

    spf_topological_top_down_walk (spf.outputG.start_symbol, generate_top_down_nbest_forest, NULL, generate_top_down_nbest_forest_post);

    sxfree (Aij_Pij_set), Aij_Pij_set = NULL;
    sxfree (rank_set), rank_set = NULL;
    XH_free (&XH_Pij_set);
    sxfree (XH_Pij_set2Aij_id), XH_Pij_set2Aij_id = NULL;
#endif /* 0 */

    /* Appel ds tous les cas */
    spf_topological_top_down_walk (spf.outputG.start_symbol, generate_nbest_forest, NULL, NULL);
    
#if MEASURES
    fill_measures (&top_downG);
    printf ("MEASURES: top_downG = (|N|=%ld, |T|=%ld, |P|=%ld, |G|=%ld)\n", top_downG.N, top_downG.T, top_downG.P, top_downG.G);
#endif /* MEASURES */

#if EBUG
    nbest_forest_print_forest ("top_down");
#else
    if (print_forest)
      nbest_forest_print_forest ("top_down");
#endif /* EBUG */

    if (is_print_time)
      sxtime (SXACTION, "\t\tTop_down optimization (done)");

    bottom_upG_optimization (exact_nbest);

#if MEASURES
    fill_measures (&bottom_upG);
    printf ("MEASURES: bottom_upG = (|N|=%ld, |T|=%ld, |P|=%ld, |G|=%ld)\n", bottom_upG.N, bottom_upG.T, bottom_upG.P, bottom_upG.G);
#endif /* MEASURES */

#if EBUG
    nbest_forest_print_forest ("bottom_up");
#else
    if (print_forest)
      nbest_forest_print_forest ("bottom_up");
#endif /* EBUG */

    if (is_print_time)
      sxtime (SXACTION, "\t\tBottom_up optimization (done)");

    if (is_print_time)
      sxtime (SXACTION, "\tExact-N-best - pb (done)");
  }

  if (an) {
    if (is_print_time)
      sxtime (SXACTION, "\tExact-N-best - an (start)");
    
    spf_topological_top_down_walk (spf.outputG.start_symbol,
				   traite_Pij,
				   remplis_matrices_Pij,
				   NULL);
    Aij_get_m_n_best(spf.outputG.start_symbol, 1, exact_nbest); 
/*       affiche_foret_reduite();      */

#if MEASURES
    affiche_measures();  
#endif /* MEASURES */


    if (is_print_time)
      sxtime (SXACTION, "\tExact-N-best - an (done)");
  }

  return 1;
 }
#endif /*0 */

/* Nouvelle version do 07/05/09 */
static SXINT
exact_nbest_sem_pass (void)
{
  SXINT    kind, full_id;
  SXFLOAT   tree_count_nb;

  if (pb) {
    if (is_print_time)
      sxtime (SXACTION, "\tExact-N-best - pb (start)");
    
#if MEASURES
    spf_print_signature (stdout, "MEASURES: prunedG (goldG)");
#endif /* MEASURES */
    
#if MEASURES
    /* kth est global */
    for (kth = 1; kth <= exact_nbest; kth++) {
      nbest_seek_kth_best_tree (spf.outputG.start_symbol, kth, fill_unfolded_forest);
    }

    /* On ajoute l'axiome */
    baselineG.N++;

    printf ("MEASURES: baselineG = (|N|=%ld, |T|=%ld, |P|=%ld, |G|=%ld)\n", baselineG.N, baselineG.T, baselineG.P, baselineG.G);

    if (is_print_time)
      sxtime (SXACTION, "\t\tUnfolded forest reading (done)");
#endif /* MEASURES */

    if (spf.tree_count.prod2nb == NULL) {
      /* On compte le nb de sous-arbres */
      spf_tree_count (&tree_count_nb);
    }

    for (kth = 1; kth <= exact_nbest; kth++) {
      XH_push (XH_nbest_set_hd, kth);
      XH_set (&XH_nbest_set_hd, &kth_id); /* kth == kth_id */
    }
    
    for (kth = 1; kth <= exact_nbest; kth++) {
      XH_push (XH_nbest_set_hd, kth);
    }

    XH_set (&XH_nbest_set_hd, &full_id);

    XxY_set (&AijXid, spf.outputG.start_symbol, full_id, &start_symbol_id);
    
    spf_topological_top_down_walk (spf.outputG.start_symbol,
				   td_Pij_first_pb_pass,
				   td_Aij_pre_first_pb_pass,
				   NULL);
    
#if MEASURES
    fill_measures (&top_downG);
    printf ("MEASURES: top_downG = (|N|=%ld, |T|=%ld, |P|=%ld, |G|=%ld)\n", top_downG.N, top_downG.T, top_downG.P, top_downG.G);
#endif /* MEASURES */

#if EBUG
    nbest_forest_print_forest ("top_down");
#else
    if (print_forest)
      nbest_forest_print_forest ("top_down");
#endif /* EBUG */

    if (is_print_time)
      sxtime (SXACTION, "\t\tTop-down reading (done)");

    bottom_upG_optimization (exact_nbest);

#if MEASURES
    fill_measures (&bottom_upG);
    printf ("MEASURES: bottom_upG = (|N|=%ld, |T|=%ld, |P|=%ld, |G|=%ld)\n", bottom_upG.N, bottom_upG.T, bottom_upG.P, bottom_upG.G);
#endif /* MEASURES */

#if EBUG
    nbest_forest_print_forest ("bottom_up");
#else
    if (print_forest)
      nbest_forest_print_forest ("bottom_up");
#endif /* EBUG */

    if (is_print_time)
      sxtime (SXACTION, "\t\tBottom_up optimization (done)");

    if (is_print_time)
      sxtime (SXACTION, "\tExact-N-best - pb (done)");
  }

  if (an) {
    if (is_print_time)
      sxtime (SXACTION, "\tExact-N-best - an (start)");
    
    spf_topological_top_down_walk (spf.outputG.start_symbol,
				   traite_Pij,
				   remplis_matrices_Pij,
				   NULL);
    Aij_get_m_n_best(spf.outputG.start_symbol, 1, exact_nbest); 
/*       affiche_foret_reduite();      */

#if MEASURES
    affiche_measures();  
#endif /* MEASURES */


    if (is_print_time)
      sxtime (SXACTION, "\tExact-N-best - an (done)");
  }

  return 1;
 }



