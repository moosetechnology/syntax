/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2005 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */

/* PCFG */
/* Actions semantiques ne conservant que les n meilleures analyses */
/* L'analyseur est de type Earley */
/* Le source est sous forme de DAG */


/* ********************************************************
   *                                                      *
   *          Produit de l'equipe ATOLL.                  *
   *                                                      *
   ******************************************************** */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 26-10-05 11:30 (pb&an):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/



static char	ME [] = "nbest_semact";

#include "sxunix.h"
#include "earley.h"
#include "XH.h"
#include "nbest.h"
#include "sxstack.h"
#include "SS.h"
#include <ctype.h>
#include <math.h>
#include <float.h>

char WHAT_NBEST_SEMACT[] = "@(#)SYNTAX - $Id: nbest_semact.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

/* Ensemble des triplets (A, prod, val) ou
   - prod est une B-prod, 
   - A est un pere de B (il existe une A-prod t.q. B est ds sa partie droite)
   - val est un entier qui indique si les nt en RHS de prod qui peuvent deriver ds le vide, derivent ds le vide
     Ex si prod = B -> X1 X2 X3 X4 X5 et si X1, X3, X4 et X5 peuvent deriver ds le vide, val=2 (0010) indique
     que ne l'on considere que les Pij de prod ds lesquelles X1, X3 et X5 derivent (effectivement) ds le vide
     et X4 ne derive pas ds le vide
*/

/* valeur par defaut */
static int     nbest = 1;
static int     beam, contextual_proba_kind, cpk;
static BOOLEAN is_print_nbest_trees, is_filtering, is_print_nbest_forest, is_output_nbest_value;
static char    *best_weights_file_name;
static double  min_weight_arg;

static char	Usage [] = "\
\t\t-n #nbest, -nbest #nbest (default -nbest 1)\n\
\t\t-cp #kind|[a]ll, -contextual_proba #kind|[a]ll\n\
\t\t-b #beam, -beam #beam\n\
\t\t-pnt, -print_nbest_trees\n\
\t\t-pnf, -print_nbest_forest\n\
\t\t-f, -filter\n\
\t\t-obw pathname, -output_best_weights pathname\n\
\t\t-mw weight, -min_weight weight\n\
\t\t-on, -output_nbest_value\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	   0
#define NBEST 	           1
#define CONTEXTUAL_PROBA   2
#define BEAM 	           3
#define PRINT_NBEST_TREES  4
#define PRINT_NBEST_FOREST 5
#define FILTER 	           6
#define OUTPUT_BEST_WEIGHTS 7
#define MIN_WEIGHT         8
#define OUTPUT_NBEST_VALUE 9

static char	*option_tbl [] = {
    "",
    "n", "nbest",
    "cp", "contextual_proba",
    "b", "beam",
    "pnt", "print_nbest_trees",
    "pnf", "print_nbest_forest",
    "f", "filter",
    "obw", "output_best_weights",
    "mw", "min_weight",
    "on", "output_nbest_value",
};

static int	option_kind [] = {
    UNKNOWN_ARG,
    NBEST, NBEST,
    CONTEXTUAL_PROBA, CONTEXTUAL_PROBA,
    BEAM, BEAM,
    PRINT_NBEST_TREES, PRINT_NBEST_TREES,
    PRINT_NBEST_FOREST, PRINT_NBEST_FOREST,
    FILTER, FILTER,
    OUTPUT_BEST_WEIGHTS, OUTPUT_BEST_WEIGHTS,
    MIN_WEIGHT, MIN_WEIGHT,
    OUTPUT_NBEST_VALUE, OUTPUT_NBEST_VALUE,
};

static int	option_get_kind (arg)
    register char	*arg;
{
  register char	**opt;

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
    register int	kind;
{
  register int	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}


/* retourne le ME */
static char*
nbest_ME ()
{
  return ME;
}

/* retourne la chaine des arguments possibles propres a nbest */
static char*
nbest_args_usage ()
{
  return Usage;
}

/* decode les arguments specifiques a lfg */
/* l'option argv [*parg_num] est inconnue du parseur earley */
static BOOLEAN
nbest_args_decode (pargnum, argc, argv)
     int  *pargnum, argc;
     char *argv [];
{
  switch (option_get_kind (argv [*pargnum])) {
  case NBEST:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (NBEST));
      return FALSE;
    }

    nbest = atoi (argv [*pargnum]);
    break;

  case CONTEXTUAL_PROBA:
    /* On utilise les proba contextuelles */
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (CONTEXTUAL_PROBA));
      return FALSE;
    }

    if (!isdigit (argv [*pargnum] [0]))
      /* all */
      cpk = -1;
    else {
      cpk = contextual_proba_kind = atoi (argv [*pargnum]);

      if (contextual_proba_kind > EP_POS_EPCFG)
	cpk = contextual_proba_kind = EP_POS_EPCFG; /* prudence */
    }

    /*
      Si A -> X1 ... Xp est une prod et si Y1 ... Yk =>+ epsilon, avec Yj=Xi, on lui associe 2^k eprods
      representant pour les Yj le fait qu'il y derive ou non ds le vide

      non contextuel
      0=PCFG         : PCFG classique
      1=EPCFG        : PCFG dont les productions sont des eprod

      contextuel
      2=A_EPCFG      : (A, eprod) eprod dont le pere est une A-prod
      3=P_EPCFG      : (p, eprod) eprod dont le pere est la prod p
      4=EP_EPCFG     : (eprod', eprod) eprod dont le pere est eprod'
      5=EP_POS_EPCFG : (eprod', pos, eprod) eprod dont le pere est eprod' et eprod est en position pos ds la rhs de eprod'
     */

    break;

  case BEAM:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (BEAM));
      return FALSE;
    }

    beam = atoi (argv [*pargnum]);
    break;

  case PRINT_NBEST_TREES:
    is_print_nbest_trees = TRUE;
    break;

  case PRINT_NBEST_FOREST:
    is_print_nbest_forest = TRUE;
    is_filtering = TRUE;
    break;

  case FILTER:
    is_filtering = TRUE;
    break;

  case OUTPUT_BEST_WEIGHTS:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (OUTPUT_BEST_WEIGHTS));
      return FALSE;
    }
	      
    best_weights_file_name = argv [*pargnum];

    break;

  case MIN_WEIGHT:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a (floating) number must follow the \"%s\" option;\n", ME, option_get_text (MIN_WEIGHT));
      return FALSE;
    }

    min_weight_arg = atof (argv [*pargnum]);

    break;


  case OUTPUT_NBEST_VALUE:
    is_output_nbest_value = TRUE;
    break;

  case UNKNOWN_ARG:
    return FALSE;
  default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap(ME,"unknown switch case #1");
#endif
    break;
  }

  return TRUE;
}


static int nbest_sem_pass ();
static BOOLEAN Aij_or_item_kth_best ();


void
nbest_semact ()
{
  min_logprob = -log10 (DBL_MAX);

#if 0
  /* finalement non car la semantique peut appeler nbest a` la demande :
     appeler avec -nbest 0
     et la sem appelle
     nbest_allocate ()
     
     perform_nbest (Aij, k)
     nbest_filter_forest ()

     nbest_free ()
  */
  is_filtering = TRUE; /* defaut si semantique */
#endif /* 0 */

  min_weight_arg = 1; /* pas de min weight */

  for_semact.sem_pass = nbest_sem_pass;
  for_semact.process_args = nbest_args_decode; /* decodage des options de nbest */
  for_semact.string_args = nbest_args_usage; /* decodage des options de nbest */
  for_semact.ME = nbest_ME; /* decodage des options de nbest */
}

/*
  Il faut ajouter du "contexte" ds la definition d'un hyperarc : ctxt(H, e) ou
  H est un hypergraphe, e un hyperarc et ctxt(H, e) est un ensemble d'entiers (chaque entier c
  est appele' contexte)
  

  Un hypergraphe H d'une fore^t partage'e est le quadruplet <V, E, t, R> ou`
    - V (les vertices) est l'ensemble des symboles instancie's Xpq
    - t (target vertex) est l'axiome 
    - E (hyperarcs) est l'ensemble des hyperarcs
    - R (re'els) est un ensemble de poids

  Un hyperarc e \in E est un couple (h(e), p(e)) ou` h(e) est un symbole instancie' appele' la tete
  et p(e) est un sous-ensemble de productions instanciees (connexes) de la fore^t.  De plus
    - il y a une queue T(e)={A | Aij =>* A} (ensemble ordonne' des symboles derivables depuis la tete par p(e))
    - e est connexe (pour toute production p de e de la forme A0 -> X1 ... Xp, on a A0 \in T(e)
    - Pour un contexte c, on a sur e une fonction fe qui est une fonction de poids de IxR^{|T(e)|} vers R
      Le 1er arg de I est c et |T(e)| est l'arite' de e.
      On suppose que les fe sont monotones (on a un ordre total <= sur R et les fe sont monotones
      sur chacun de ses arguments).  On suppose doncl'existence d'une fonction de comparaison min<=
      t.q. min<=(a, b) vaut a si a <= b sinon b
      Si h(e) est un symbole terminal instancie', on a p(e)=\emptyset et fe() \in R est une constante.

  L'arite' d'un hypergraphe est l'arite' maximale de ses hyperarcs.

  Le BS(Xpq) (Backward-star) d'un symbole Xpq est l'ensemble des hyperarcs dans lesquels Xpq est une tete
  {e \in E | h(e)=Xpq} Le degre' d'entre'e de Xpq est |BS(Xpq)|.

  Une de'rivation D d'un symbole instancie' Aij d'un hypergraphe H,sa taille |D| et son poids w(D) sont
  de'finis re'cursivement comme suit :
    - Si e \in BS(Aij) et si |p(e)|=0, alors D=<e,\varepsilon> est une de'rivation de Aij, sa taille |D|=1 et
      son poids w(D)=fe()
    - Si e \in BS(Aij) et si |p(e)|>0 et si Di est une de'rivation de Xi pour chaque symbole de T(e), alors
      D=<e,D1 ...D_{|T(e)|}> est une de'rivation de Aij, de taille |D|=1+\sigma_{i=1}^{|T(e)|} et de poids
      w(D)=fe(w(D1), ..., w(D_{|T(e)|}))

  L'ordre sur les poids induit un ordre sur les de'rivations D<=D' ssi w(D)<=w(D')

  On de'finit Di(Aij) comme etant la meilleur ieme derivation de Aij.
  \bf{D}(Aij) est le vecteur des n meilleures derivations <D1(Aij), ..., Dn(Aij)>

  Trouver les n-meilleures derivations pour une foret donnee revient donc a calculer \bf{D}(t)

  Une derivation a pointeurs arriere dbp (derivation with back-pointers) \tilde{D} de Aij est le tuple
  <e,\bf{j}> avec e \in BS(Aij) et \bf{j} \in {1,2, ..., n}^{|T(e)|}
 */

/* A chaque Aij est associe dedication_list, liste chainee de k dedicaces */
/* la meilleure dedicace a le numero 1, la moins bonne le numero k */
/* On peut acceder a la moins bonne (de numero k) par Aij2last_dedication [Aij]
   puis en suivant la liste chainee a la k-1eme, ..., 1ere
   On peut ausi acceder directement a la peme (1 <= p <= k) par le couple (Aij,p) dont la valeur
   se calcule par le XxY dedication_hd.  dedication_list est en // avec dedication_hd */
/* une dedicace est un quadruplet (weight, Pij, signature, k)
   il s'agit du keme meilleure choix et il est realise par Pij (c'est une Aij-prod), de signature signature et de poids weight
   Si non nul, next designe la k-1eme meilleure */

/* Cas contextual_proba_kind */
struct PijKs_elem {
  int PijK;
};

static int               **item2PijKs_stack_list, *cur_PijKs_stack, **item2PijKs_list;
static int               cur_PijKs_stack_list_size, cur_PijKs_stack_list_top, cur_PijKs_stack_size, cur_PijKs_stack_top;
static int               *item2cur_contextual_proba_kind;

static XxY_header        dedication_hd;
static struct dedication {
  double total_weight;
  int    PijK, signature, k, next;
}                        *dedication_list;

static int                *Aij_or_item2last_dedication, *Aij2last_dedication, *item2last_dedication;

static double             *Pij2one_best_local_weight;

static XxY_header         PijxItem_hd;
static struct ctxt_elem {
  double             ctxt_weight;
  struct signet_elem *signet;
}                         *PijK2ctxt_elem_list, empty_ctxt_elem;

static double             best_Aij_local_weight;
static int                best_Aij_Pij, PijK_orig;

/* Associe a chaque Pij on a un signet */
/* Un signet est une pile ordonnee qui contient un ensemble de signatures actives */
static struct signet_elem **Pij2signet;

/* Poids des terminaux instancies */
static double            *Tpq_item2weight;


/* Associe a chaque Pij on a un signet accessible par Pij2signet [Pij] */
/* C'est une liste de couples (weight, signature) ordonnee par poids decroissants (en fait non croissants)
   Deux couples differents d'un signet ont des signatures differentes.
   C'est Pijxsignature_hd qui assure que les signatures sont differentes (l'essai avec des SXBA a ete abandonne'
   car beaucoup trop volumineux, quoique + rapide si pas de reallocations) */

/* A un moment donne' un signet contient les meilleurs resultats, le meilleur en tete.
   Il s'agit du meilleur relatif, des encore meilleurs (des coups d'avant) ayant pu se transformer.
   Cette transformation se fait comme suit.  Supposons que pour Aij le keme meilleur soit obtenu
   pour Pij et signature et que l'on cherche a calculer, pour Aij le k+1eme meilleur.  La tete du signet de Pij
   correspond a cette signature.  On va enlever cette tete du signet de Pij et la remplacer par un sous-signet
   qui correspond a toutes les evolutions possibles de signature sur la partie droite de Pij.  Le tri de ce sous-signet
   s'effectue en utilisant local_signet_stack.  La fusion avec le signet principal (apres decapitation) s'effectue
   par la procedure merge().  Pour accelerer cette fusion, un signet est dichotomise' par Pij2attr, Pijxsignpost_hd
   et Pijxsignpost2signet.  La taille des sous-signets est fixee, apres essais a 2**LOG2_SLICE_SIZE.
*/
static int                signet_elem_area_size;
static struct signet_elem {
  double             total_weight;
  int                signature;
  struct signet_elem *next;
}                         *signet_elem_free_list, *signet_elem_list;

/* Apres essais, c'est la meilleure valeur!! */
#define LOG2_SLICE_SIZE   6
static struct Pij_attr {
  int signature_nb, signpost;
}                         *Pij2attr;
static XxY_header         Pijxsignpost_hd;
static struct signet_elem **Pijxsignpost2signet;

/* Pile de travail de next_signet() de taille maxrhs */
static struct local_signet_elem {
  double             weight;
  int                signature;
}                        *local_signet_stack;

/* Technique car les calculs sont recursifs */
static int               *SS_signet_stack;


/* Une signature associee a une Pij A -> X0 X1 ... Xp est un vecteur (h0, h1, ..., hp) d'entiers hi
   (0 <= hi < nbest) qui repere un indice dans la sous-pile associee a` Xi */
/* Attention, il est possible d'obtenir la meme signature de plusieurs fac,ons differentes :
                 [1, 1]
                 /    \
            [2, 1]    [1, 2]
            /    \    /    \
        [3, 1]   [2, 2]    [1, 3]

   Il faut donc verifier que lorsqu'on met une signature ds un signet, elle ne s'y trouve pas deja */
static XH_header         signatures_hd;

/* Pij et Aij dont on a epuise' toutes les possibilites d'evolution */
static SXBA              fully_computed_Aij_or_item_set;
static SXBA              best_Pij_set;

/* ... et qq petits trucs */
#if 0
static int               best_Aij_signature, best_Pij;
static double            best_Aij_weight = -DBL_MAX;
#endif /* 0 */
static int               *unit_vector, unit_vector_signature;

/* Cas contextual_proba_kind */
#if LLOG
static int               total_cur_PijKs_stack_size;
#endif /* LLOG */
#if 0
  static struct weight_stack_list {
    double                   *weight_stack;
    struct weight_stack_list *next;
  } *weight_stack_list_hd;
#endif


/* Attention, ds le cas contextual_proba_kind > EPCFG, Aij_or_item2last_dedication et fully_computed_Aij_or_item_set sont indices par des
   items et pas par des Aij */
/* *************************************************************************************************** */
static void
signet_elem_alloc ()
{
  struct signet_elem *ptr;

  ptr = (struct signet_elem*) sxalloc (signet_elem_area_size+1, sizeof (struct signet_elem));
  ptr->signature = 0; /* top */
  ptr->total_weight = 0; /* pour etre propre */
  ptr->next = signet_elem_list;

  signet_elem_list = ptr;
}

static void
signet_elem_free ()
{
#if LLOG
  int                nb = 0;
#endif /* LLOG */
  struct signet_elem *ptr;

  while (signet_elem_list) {
#if LLOG
    nb++;
#endif /* LLOG */
    ptr = signet_elem_list;
    signet_elem_list = ptr->next;
    sxfree (ptr);
  }

  signet_elem_list = NULL;

#if LLOG
  printf ("signet_elem : size=%i (%i*%i)\n", (signet_elem_area_size+1)*nb, signet_elem_area_size+1, nb);
#endif /* LLOG */
}


static void
PijKs_stack_list_hd_free ()
{
#if LLOG
  int                nb = 0;
#endif /* LLOG */
  int                i;

  for (i = 0; i <= cur_PijKs_stack_list_top; i++) {
#if LLOG
    nb++;
#endif /* LLOG */
    sxfree (item2PijKs_stack_list [i]);
  }

  sxfree (item2PijKs_stack_list), item2PijKs_stack_list = NULL;

#if LLOG
  if (total_cur_PijKs_stack_size)
    printf ("total_cur_PijKs_stack_size = %i in %i bags\n", total_cur_PijKs_stack_size, nb);
#endif /* LLOG */
}


static struct signet_elem*
get_new_signet_elem ()
{
  int                top;
  struct signet_elem *ptr;

  if (signet_elem_free_list) {
    ptr = signet_elem_free_list;
    signet_elem_free_list = ptr->next;

    return ptr;
  }

  if ((top = ++signet_elem_list->signature) > signet_elem_area_size) {
    signet_elem_alloc ();
    top = ++signet_elem_list->signature;
  }

  return signet_elem_list+top;
}

static void
release_signet_elem (signet_elem_ptr)
  struct signet_elem       *signet_elem_ptr;
{
  signet_elem_ptr->next = signet_elem_free_list;
  signet_elem_free_list = signet_elem_ptr;
}


static void
dedication_oflw (old_size, new_size)
     int old_size, new_size;
{
  dedication_list = (struct dedication*) sxrealloc (dedication_list, new_size+1, sizeof (struct dedication));

#if LLOG
  printf ("\nReallocation of dedication_list [%i]/[%i]\n", old_size+1, new_size+1);
#endif /* LLOG */
}


static void
PijxItem_oflw (old_size, new_size)
     int old_size, new_size;
{
  PijK2ctxt_elem_list = (struct ctxt_elem*) sxrealloc (PijK2ctxt_elem_list, new_size+1, sizeof (struct ctxt_elem));
  Pij2attr = (struct Pij_attr*) sxrealloc (Pij2attr, new_size+1, sizeof (struct Pij_attr));

#if LLOG
  printf ("\nReallocation of PijK2ctxt_elem_list [%i]/[%i] and Pij2attr [%i]/[%i]\n", old_size+1, new_size+1, old_size+1, new_size+1);
#endif /* LLOG */
}

static void
Pijxsignpost_oflw (old_size, new_size)
     int old_size, new_size;
{
  Pijxsignpost2signet = (struct signet_elem **) sxrealloc (Pijxsignpost2signet, new_size+1, sizeof (struct signet_elem *));

#if LLOG
  printf ("\nReallocation of Pijxsignpost2signet [%i]/[%i]\n", old_size+1, new_size+1);
#endif /* LLOG */
}

static void
fill_Pij2eprod ()
{
  int Pij, val, init_prod, eprod;

  spf_fill_Pij2eval ();

  /* reuse */
  XxY_reuse (&prodXval2eprod_hd, "prodXval2eprod_hd", NULL, NULL);

  /* alloc */
  Pij2eprod = (int*) sxalloc (spf.outputG.maxxprod+1, sizeof (int));
  Pij2eprod [0] = 1; /* prodXval2eprod_hd.display [1] = (0, 0) */

  for (Pij = 1; Pij <= spf.outputG.maxxprod; Pij++) {
    init_prod = spf.outputG.lhs [Pij].init_prod;
    val = spf.outputG.Pij2eval [Pij];
    eprod = XxY_is_set (&prodXval2eprod_hd, init_prod, val); /* peut etre nul */
    Pij2eprod [Pij] = eprod;
  }
}

/* Peut erte appele' depuis l'exterieur (par exemple lfg_semact pour pouvoir
   utiliser get_Tpq_logprob (item, cur_contextual_proba_kind) */
void
nbest_open (int cur_contextual_proba_kind)
{  /* On "reactive" les XxY statiques */
  XxY_reuse (&ff_idXt2lex_id_hd, "ff_idXt2lex_id_hd", NULL, NULL);

  switch (cur_contextual_proba_kind) {
  case EP_POS_EPCFG:
    XxY_reuse (&full_ctxtXeprod_hd, "full_ctxtXeprod_hd", NULL, NULL);
    XxY_reuse (&full_ctxt_hd, "full_ctxt_hd", NULL, NULL);
    XxY_reuse (&full_ctxtXlex_id_hd, "full_ctxtXlex_id_hd", NULL, NULL);
  case EP_EPCFG:
    XxY_reuse (&eprod_ctxtXeprod_hd, "eprod_ctxtXeprod_hd", NULL, NULL);
    XxY_reuse (&eprod_ctxtXlex_id_hd, "eprod_ctxtXlex_id_hd", NULL, NULL);
  case P_EPCFG:
    XxY_reuse (&prod_ctxtXeprod_hd, "prod_ctxtXeprod_hd", NULL, NULL);
    XxY_reuse (&prod_ctxtXlex_id_hd, "prod_ctxtXlex_id_hd", NULL, NULL);
  case A_EPCFG:
    XxY_reuse (&A_ctxtXeprod_hd, "A_ctxtXeprod_hd", NULL, NULL);
    XxY_reuse (&A_ctxtXlex_id_hd, "A_ctxtXlex_id_hd", NULL, NULL);
  case EPCFG:
    fill_Pij2eprod ();
  case PCFG:
    break;
  default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap(ME,"unknown switch case #2");
#endif
    break;
  }

  if (Tpq_item2weight == NULL)
    Tpq_item2weight = (double*) sxcalloc ((unsigned long int)spf.outputG.lhs [spf.outputG.maxxprod+1].prolon+1, sizeof (double));
  /* sinon deja alloue' ... */
}

void
nbest_close ()
{
  if (Tpq_item2weight) sxfree (Tpq_item2weight), Tpq_item2weight = NULL;
  if (Pij2eprod) sxfree (Pij2eprod), Pij2eprod = NULL;
}


/* Peut etre appele' un nb qqcq de fois sur des environnements qqc */
void
nbest_allocate (int nbest_allocate_nbest, int cur_contextual_proba_kind)
{
  int i, size, Aij_or_item_nb;

  /* Peut etre appele' plusieurs fois => RAZ des variables statiques */
  signet_elem_free_list = signet_elem_list = NULL;
#if LLOG
  total_cur_PijKs_stack_size = 0;
#endif /* LLOG */

  nbest_open (cur_contextual_proba_kind);

  if (cur_contextual_proba_kind > EPCFG)
    Aij_or_item_nb = spf.outputG.lhs [spf.outputG.maxxprod+1].prolon; /* les items des Pij */
  else
    Aij_or_item_nb = spf.outputG.maxxnt;

  if (Aij_or_item2last_dedication) sxfree (Aij_or_item2last_dedication); /* un peu grossier */
  Aij_or_item2last_dedication = (int*) sxcalloc ((unsigned long int)Aij_or_item_nb+1, sizeof (int));

  if (cur_contextual_proba_kind > EPCFG) {
    item2last_dedication = Aij_or_item2last_dedication;
    Aij2last_dedication = NULL;
  }
  else {
    Aij2last_dedication = Aij_or_item2last_dedication;
    item2last_dedication = NULL;
  }

  if (!XxY_is_allocated (dedication_hd) ) {
    size = spf.outputG.maxxnt*(2+nbest_allocate_nbest/200); /* essai */

    XxY_alloc (&dedication_hd, "dedication_hd", size+1, 1, 0 /* pas de foreach */, 0, dedication_oflw, statistics);
    dedication_list = (struct dedication*) sxalloc (XxY_size (dedication_hd)+1, sizeof (struct dedication));

    size = spf.outputG.maxxprod*(2+nbest_allocate_nbest/1000);

    XH_alloc (&signatures_hd, "signatures_hd", size+1, 1, spf.inputG.maxrhs/2 +1,
	      /* (nbest_allocate_nbest > 1) ? signatures_oflw :*/ NULL, statistics);
    unit_vector = (int*) sxalloc (spf.inputG.maxrhs+1, sizeof (int));

    for (i = 0; i <= spf.inputG.maxrhs; i++) {
      unit_vector [i] = 1;
      XH_push (signatures_hd, 1);
    }

    XH_set (&signatures_hd, &unit_vector_signature);

    Pij2one_best_local_weight = (double*) sxalloc (spf.outputG.maxxprod+1, sizeof (double));

    if (nbest_allocate_nbest > 1) {
      XxY_alloc (&Pijxsignpost_hd, "Pijxsignpost_hd", spf.outputG.maxxprod+1, 1, 0 /* pas de foreach */, 0, Pijxsignpost_oflw, statistics);
      Pijxsignpost2signet = (struct signet_elem **) sxalloc (XxY_size (Pijxsignpost_hd)+1, sizeof (struct signet_elem *));

      local_signet_stack = (struct local_signet_elem*) sxalloc (spf.inputG.maxrhs+1, sizeof (struct local_signet_elem));
      SS_signet_stack = SS_alloc (100);
    }

    if (nbest_allocate_nbest > 1 || cur_contextual_proba_kind > EPCFG) {
      signet_elem_area_size = 2*spf.outputG.maxxprod; /* a voir */
      signet_elem_alloc ();

      fully_computed_Aij_or_item_set = sxba_calloc (Aij_or_item_nb+1);

      if (cur_contextual_proba_kind > EPCFG) {
	XxY_alloc (&PijxItem_hd, "PijxItem_hd", size+1, 1, 0 /* pas de foreach */, 0, PijxItem_oflw, statistics);
	XxY_set (&PijxItem_hd, 0, 0, &PijK_orig);
	PijK2ctxt_elem_list = (struct ctxt_elem*) sxalloc (XxY_size (PijxItem_hd)+1, sizeof (struct ctxt_elem));
	PijK2ctxt_elem_list [PijK_orig] = empty_ctxt_elem;
	Pij2attr = (struct Pij_attr*) sxalloc (XxY_size (PijxItem_hd)+1, sizeof (struct Pij_attr));
      }
      else {
	Pij2signet = (struct signet_elem**) sxcalloc ((unsigned long int)spf.outputG.maxxprod+1, sizeof (struct signet_elem*));
	Pij2attr = (struct Pij_attr*) sxalloc (spf.outputG.maxxprod+1, sizeof (struct Pij_attr));
      }
    }

    if (is_filtering)
      best_Pij_set = sxba_calloc (spf.outputG.maxxprod+1);

    if (cur_contextual_proba_kind > EPCFG) {
      item2cur_contextual_proba_kind = (int *) sxcalloc ((unsigned long int)Aij_or_item_nb+1, sizeof (int));

      cur_PijKs_stack_list_size = 1;
      cur_PijKs_stack_list_top = -1;

      item2PijKs_stack_list = (int**) sxalloc (cur_PijKs_stack_list_size, sizeof (int*));

      cur_PijKs_stack_size = Aij_or_item_nb;
      cur_PijKs_stack_top = cur_PijKs_stack_size+1; /* Pour faire la 1ere alloc comme les suivantes */

      item2PijKs_list = (int **) sxalloc (Aij_or_item_nb+1, sizeof (int *));
    }
  }
  else {
    XxY_clear (&dedication_hd);
    XH_clear (&signatures_hd);

    for (i = 0; i <= spf.inputG.maxrhs; i++) {
      unit_vector [i] = 1;
      XH_push (signatures_hd, 1);
    }

    XH_set (&signatures_hd, &unit_vector_signature);

    if (nbest_allocate_nbest > 1) {
      if (Pijxsignpost2signet == NULL) {
	XxY_alloc (&Pijxsignpost_hd, "Pijxsignpost_hd", spf.outputG.maxxprod+1, 1, 0 /* pas de foreach */, 0, Pijxsignpost_oflw, statistics);
	Pijxsignpost2signet = (struct signet_elem **) sxalloc (XxY_size (Pijxsignpost_hd)+1, sizeof (struct signet_elem *));

	local_signet_stack = (struct local_signet_elem*) sxalloc (spf.inputG.maxrhs+1, sizeof (struct local_signet_elem));
	SS_signet_stack = SS_alloc (100);
      }
      else {
	XxY_clear (&Pijxsignpost_hd);
	SS_clear_ss (SS_signet_stack);
      }
    }

    if (nbest_allocate_nbest > 1 || cur_contextual_proba_kind > EPCFG) {
      if (fully_computed_Aij_or_item_set == NULL) {
	signet_elem_area_size = 2*spf.outputG.maxxprod; /* a voir */
	signet_elem_alloc ();

	fully_computed_Aij_or_item_set = sxba_calloc (Aij_or_item_nb+1);

	if (cur_contextual_proba_kind > EPCFG) {
	  if (PijK2ctxt_elem_list == NULL) {
	    size = spf.outputG.maxxprod*(2+nbest_allocate_nbest/1000);
	    XxY_alloc (&PijxItem_hd, "PijxItem_hd", size+1, 1, 0 /* pas de foreach */, 0, PijxItem_oflw, statistics);
	    XxY_set (&PijxItem_hd, 0, 0, &PijK_orig);
	    PijK2ctxt_elem_list = (struct ctxt_elem*) sxalloc (XxY_size (PijxItem_hd)+1, sizeof (struct ctxt_elem));
	    PijK2ctxt_elem_list [PijK_orig] = empty_ctxt_elem;
	    Pij2attr = (struct Pij_attr*) sxalloc (XxY_size (PijxItem_hd)+1, sizeof (struct Pij_attr));
	  }
	  else {
	    XxY_clear (&PijxItem_hd);
	    XxY_set (&PijxItem_hd, 0, 0, &PijK_orig);
	  }
	}
	else {
	  if (Pij2signet == NULL) {
	    Pij2signet = (struct signet_elem**) sxcalloc ((unsigned long int)spf.outputG.maxxprod+1, sizeof (struct signet_elem*));
	    Pij2attr = (struct Pij_attr*) sxalloc (spf.outputG.maxxprod+1, sizeof (struct Pij_attr));
	  }
	  else {
	    sxfree (Pij2signet); /* grossier */
	    Pij2signet = (struct signet_elem**) sxcalloc ((unsigned long int)spf.outputG.maxxprod+1, sizeof (struct signet_elem*));
	  }
	}
      }
      else {
	/* grossier */
	signet_elem_free ();
	signet_elem_alloc ();

	sxfree (fully_computed_Aij_or_item_set); /* prudence */
	fully_computed_Aij_or_item_set = sxba_calloc (Aij_or_item_nb+1);

	if (cur_contextual_proba_kind > EPCFG) {
	  if (PijK2ctxt_elem_list == NULL) {
	    size = spf.outputG.maxxprod*(2+nbest_allocate_nbest/1000);
	    XxY_alloc (&PijxItem_hd, "PijxItem_hd", size+1, 1, 0 /* pas de foreach */, 0, PijxItem_oflw, statistics);
	    XxY_set (&PijxItem_hd, 0, 0, &PijK_orig);
	    PijK2ctxt_elem_list = (struct ctxt_elem*) sxalloc (XxY_size (PijxItem_hd)+1, sizeof (struct ctxt_elem));
	    PijK2ctxt_elem_list [PijK_orig] = empty_ctxt_elem;
	    Pij2attr = (struct Pij_attr*) sxalloc (XxY_size (PijxItem_hd)+1, sizeof (struct Pij_attr));
	  }
	  else {
	    XxY_clear (&PijxItem_hd);
	    XxY_set (&PijxItem_hd, 0, 0, &PijK_orig);
	  }
	}
	else {
	  if (Pij2signet == NULL) {
	    Pij2signet = (struct signet_elem**) sxcalloc ((unsigned long int)spf.outputG.maxxprod+1, sizeof (struct signet_elem*));
	    Pij2attr = (struct Pij_attr*) sxalloc (spf.outputG.maxxprod+1, sizeof (struct Pij_attr));
	  }
	  else {
	    sxfree (Pij2signet); /* grossier */
	    Pij2signet = (struct signet_elem**) sxcalloc ((unsigned long int)spf.outputG.maxxprod+1, sizeof (struct signet_elem*));
	  }
	}
      }
    }

    if (is_filtering) {
      if (best_Pij_set == NULL)
	best_Pij_set = sxba_calloc (spf.outputG.maxxprod+1);
      else
	sxba_empty (best_Pij_set);
    }

    if (cur_contextual_proba_kind > EPCFG) {
      if (item2cur_contextual_proba_kind) sxfree (item2cur_contextual_proba_kind);
      item2cur_contextual_proba_kind = (int *) sxcalloc ((unsigned long int)Aij_or_item_nb+1, sizeof (int));

      cur_PijKs_stack_list_size = 1;
      cur_PijKs_stack_list_top = -1;

      if (item2PijKs_stack_list == NULL)
	item2PijKs_stack_list = (int**) sxalloc (cur_PijKs_stack_list_size, sizeof (int*));

      cur_PijKs_stack_size = Aij_or_item_nb;
      cur_PijKs_stack_top = cur_PijKs_stack_size+1; /* Pour faire la 1ere alloc comme les suivantes */

      if (item2PijKs_list) sxfree (item2PijKs_list);
      item2PijKs_list = (int **) sxalloc (Aij_or_item_nb+1, sizeof (int *));
    }
  }
}


void
nbest_free ()
{

  nbest_close ();

  sxfree (Aij_or_item2last_dedication), Aij_or_item2last_dedication = NULL;

  XxY_free (&dedication_hd);
  sxfree (dedication_list), dedication_list = NULL;

  XH_free (&signatures_hd);
  sxfree (unit_vector), unit_vector = NULL;

  sxfree (Pij2one_best_local_weight), Pij2one_best_local_weight = NULL;

  if (local_signet_stack) {
    XxY_free (&Pijxsignpost_hd);
    sxfree (Pijxsignpost2signet), Pijxsignpost2signet = NULL;

    sxfree (local_signet_stack), local_signet_stack = NULL;
    SS_free (SS_signet_stack), SS_signet_stack = NULL;
  }

  if (fully_computed_Aij_or_item_set) {
    signet_elem_free ();
    sxfree (fully_computed_Aij_or_item_set), fully_computed_Aij_or_item_set = NULL;

    if (PijK2ctxt_elem_list) {
      XxY_free (&PijxItem_hd);
      sxfree (PijK2ctxt_elem_list), PijK2ctxt_elem_list = NULL;
    }

    if (Pij2signet) sxfree (Pij2signet), Pij2signet = NULL;

    sxfree (Pij2attr), Pij2attr = NULL;
  }

  if (best_Pij_set) sxfree (best_Pij_set), best_Pij_set = NULL;

  if (item2cur_contextual_proba_kind) {
    sxfree (item2cur_contextual_proba_kind), item2cur_contextual_proba_kind = NULL;

    PijKs_stack_list_hd_free ();

    sxfree (item2PijKs_list), item2PijKs_list = NULL;
  }
}


#if LOG
static void
print_signature (signature)
     int signature;
{
  int cur, bot, top, level;

  if (signature == unit_vector_signature) {
    fputs ("[1, ..., 1]", stdout);
  }
  else {
    top = XH_X (signatures_hd, signature+1);
    bot = XH_X (signatures_hd, signature);

    putchar ('[');

    for (cur = bot; cur < top; cur++) {
      if (cur > bot)
	fputs (", ", stdout);

      level = XH_list_elem (signatures_hd, cur);
      printf ("%i", level);
    }

    putchar (']');
  }
}
#endif /* LOG */


/* retourne le pointeur vers la bonne dedication    */
static struct dedication*
get_dedication_ptr (item_or_Aij, k)
     int item_or_Aij, k;
{
  int               dedication, top_k;
#if LLLOG
  struct dedication *dedication_ptr;
  int Pij, PijK, signature;
#endif

#if LLLOG
  printf ("********** Entering get_dedication_ptr (item_or_Aij=%i, k=%i) **********\n", item_or_Aij, k);
#endif /* LOG */

  dedication = Aij_or_item2last_dedication [item_or_Aij];

  if (dedication) {
    if ((top_k = dedication_list [dedication].k) >= k) {
      /* deja calculee */
      if (top_k != k) {
	/* Ce n'est pas celle du sommet */
	dedication = XxY_is_set (&dedication_hd, item_or_Aij, k);
      }
    }
    else
      dedication = 0;
  }

#if LLLOG
  printf ("********** Leaving get_dedication_ptr (item_or_Aij=%i, k=%i) ", item_or_Aij, k);

  if (dedication == 0)
    fputs ("NULL **********\n", stdout);
  else {
    dedication_ptr = dedication_list+dedication;
    PijK = dedication_ptr->PijK;

    if (PijK < 0)
      Pij = -PijK;
    else
      Pij = XxY_X (PijxItem_hd, PijK);

    signature = dedication_ptr->signature;
    printf ("dedication = %i: Pij=%i, signature=%i", dedication, Pij, signature);
    print_signature (signature);
    printf (", k=%i, weight=%f **********\n", dedication_ptr->k, dedication_ptr->total_weight);
  }
#endif /* LOG */

  return dedication ? dedication_list+dedication : NULL;
}


static void
print_X (X)
     int X;
{
  if (X > 0)
    printf ("<%s> ", spf.inputG.ntstring [X]);
  else
    printf ("\"%s\" ", spf.inputG.tstring [-X]);
}

static void
print_init_prod (init_prod)
     int init_prod;
{
  int item, X;

  printf ("%i : ", init_prod);

  print_X (spf.inputG.lhs [init_prod]);

  fputs ("= ", stdout);

  item = spf.inputG.prolon [init_prod];

  while ((X = spf.inputG.lispro [item++]) != 0) {
    print_X (X);
  }

  putchar (';');
}

static void
print_ctxt (item)
     int item;
{
  int Pij, pos, init_prod, A, eprod, val, cur_contextual_proba_kind;

  sxinitialise(val); /* pour faire taire "gcc -Wuninitialized" */
  cur_contextual_proba_kind = item2cur_contextual_proba_kind [item];

  Pij = spf.outputG.rhs [item].prolis;
  init_prod = spf.outputG.lhs [Pij].init_prod;

  if (cur_contextual_proba_kind >= EP_EPCFG) {
    eprod = Pij2eprod [Pij];

#if EBUG
    if (eprod == 0 || XxY_X (prodXval2eprod_hd, eprod) != init_prod)
      sxtrap (ME, "print_ctxt");
#endif /* EBUG */

    val = XxY_Y (prodXval2eprod_hd, eprod);
  }

  fputs (" in ctxt ", stdout);

  switch (cur_contextual_proba_kind) {
  case EP_POS_EPCFG:
    pos = item-spf.outputG.lhs [Pij].prolon;
    print_init_prod (init_prod);
    printf ("[val=%i, pos=%i]", val, pos);
    break;
  case EP_EPCFG:
    print_init_prod (init_prod);
    printf ("[val=%i]", val);
    break;
  case P_EPCFG:
    print_init_prod (init_prod);
    break;
  case A_EPCFG:
    A = spf.inputG.lhs [init_prod];
    printf ("<%s>", spf.inputG.ntstring [A]);
    break;
  default :
    fputs ("[no ctxt]", stdout);
    break;
  }
}



/* lispro [item]=Xpq et son pere est le nt A
   Xpq est un nt instancie'
   weight_list_ptr pointe vers un tableau // aux Ppq des Xpq-prods et contient les probas contextuelles
   On calcule et range la meilleure proba de la contribution "item" et on retourne sa valeur
   Cette contrib peut deja etre calculee
*/
static void
set_ctxt_item_weight (int item, int *best_PijKs_ptr, int k)
{
  int                Xpq, hook, Ppq, next, top_k, Pij; 
  int                PijK, dedication, signature;
  double             best_total_weight, total_weight;
  struct rhs         *prhs;
  struct dedication  *dedication_ptr;
  int  *PijKs_ptr;
  struct signet_elem *signet_elem_ptr;
#if LOG
  double ctxt_weight, local_weight;
#endif

  sxinitialise(dedication_ptr); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(signature); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(PijK); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(best_total_weight); /* pour faire taire "gcc -Wuninitialized" */
  next = item2last_dedication [item];

  if (next && (top_k = dedication_list [next].k) >= k) {
    /* deja calculee */
    return;
#if 0
    if (top_k != k) {
      /* Ce n'est pas celle du sommet */
      next = XxY_is_set (&dedication_hd, item, k);
    }

    return dedication_list [next].total_weight;
#endif /* 0 */
  }

#if EBUG
  if (next && k != top_k+1)
    sxtrap (ME, "set_ctxt_item_weight");
#endif /* EBUG */

#if LOG
  printf ("********** Entering set_ctxt_item_weight (item=%i, k=%i)", item, k);
  print_ctxt (item);
  fputs (" **********\n", stdout);
#endif /* LOG */

  if (best_PijKs_ptr == NULL) {
    /* L'appelant ne connait pas le meilleur, on le recherche */
    PijKs_ptr = item2PijKs_list [item];

    prhs = spf.outputG.rhs + item;
    Xpq = prhs->lispro;
    /* On parcourt les definitions de Xpq */
    hook = spf.outputG.lhs [spf.outputG.maxxprod+Xpq].prolon;

    while ((Ppq = spf.outputG.rhs [hook++].lispro) != 0) {
      if (Ppq > 0) {
	PijK = *PijKs_ptr;

	if (PijK && (signet_elem_ptr = PijK2ctxt_elem_list [PijK].signet)) {
	  /* Il reste sur PijK des signets ... */
	  total_weight = signet_elem_ptr->total_weight;
	
	  if (best_PijKs_ptr == NULL || total_weight > best_total_weight) {
	    best_total_weight = total_weight;
	    best_PijKs_ptr = PijKs_ptr;
	  }
	}

	PijKs_ptr++;
      }
    }
  }

  if (best_PijKs_ptr) {
	    
#if LOG
    PijKs_ptr = item2PijKs_list [item];

    prhs = spf.outputG.rhs + item;
    Xpq = prhs->lispro;
    /* On parcourt les definitions de Xpq */
    hook = spf.outputG.lhs [spf.outputG.maxxprod+Xpq].prolon;

    while ((Ppq = spf.outputG.rhs [hook++].lispro) != 0) {
      if (Ppq > 0) {
	PijK = *PijKs_ptr;

	if (PijK && (signet_elem_ptr = PijK2ctxt_elem_list [PijK].signet)) {
	  /* Il reste sur PijK des signets ... */
	  total_weight = signet_elem_ptr->total_weight;
	  ctxt_weight = PijK2ctxt_elem_list [PijK].ctxt_weight;
	  local_weight = total_weight-ctxt_weight;
	  signature = signet_elem_ptr->signature;
	  spf_print_iprod (stdout, Ppq, "", "");
	  print_ctxt (item);
	  printf (", ctxt weight=%f, local weight=%f, total weight=%f, signature=%i", ctxt_weight, local_weight, total_weight, signature);
	  print_signature (signature);
	  fputs ("\n", stdout);
	}

	PijKs_ptr++;
      }
    }
#endif /* LOG */

    PijK = *best_PijKs_ptr;
    signet_elem_ptr = PijK2ctxt_elem_list [PijK].signet;
    signature = signet_elem_ptr->signature;
    best_total_weight = signet_elem_ptr->total_weight;

    XxY_set (&dedication_hd, item, k, &dedication);
    dedication_ptr = dedication_list+dedication;

    dedication_ptr->signature = signature;
    dedication_ptr->PijK = PijK;
    dedication_ptr->k = k;
    dedication_ptr->next = next;
    dedication_ptr->total_weight = best_total_weight;

    item2last_dedication [item] = dedication;

    if (is_filtering) {
      if (PijK < 0)
	Pij = -PijK;
      else
	Pij = XxY_X (PijxItem_hd, PijK);

      SXBA_1_bit (best_Pij_set, Pij);
    }
  }
  else {
    SXBA_1_bit (fully_computed_Aij_or_item_set, item);
  }

#if LOG
  printf ("********** Leaving set_ctxt_item_weight (item=%i, k=%i)", item, k);

  if (best_PijKs_ptr) {
    print_ctxt (item);
    Pij = XxY_X (PijxItem_hd, PijK);
    printf (": dedication=%i: total_weight=%f, Pij=%i, signature=%i",
	    dedication, dedication_ptr->total_weight, Pij, signature);
    print_signature (signature);
    putchar (')');
  }
  else {
    fputs (" COMPLETE", stdout);
  }

  fputs (" **********\n", stdout);
#endif /* LOG */
}


static double
set_item_weight_list (int item)
{
  int                      Xpq, Ppq, A, hook, base_hook, top, init_prod, Pij, father_eprod, son_eprod, pos, full_ctxt, couple; 
  int                      cur_contextual_proba_kind, PpqK;
  double                   min_ctxt_weight, prev_min_ctxt_weight, best_total_weight, ctxt_weight, local_weight, total_weight;
  int        *base_ptr, *PijKs_ptr, *best_PijKs_ptr; 
  BOOLEAN                  not_in_stats, is_first_time;
  struct signet_elem       *signet_elem_ptr, signet_elem;

  sxinitialise(best_PijKs_ptr);  /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(best_total_weight); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(total_weight); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(full_ctxt); /* pour faire taire "gcc -Wuninitialized" */
  Xpq = spf.outputG.rhs [item].lispro;
  Pij = spf.outputG.rhs [item].prolis;
  init_prod = spf.outputG.lhs [Pij].init_prod;
  A = spf.inputG.lhs [init_prod];
  pos = item-spf.outputG.lhs [Pij].prolon;
    
  hook = base_hook = spf.outputG.lhs [spf.outputG.maxxprod+Xpq].prolon;

  /* On regarde si ca va tenir ... */
  top = spf.outputG.lhs [spf.outputG.maxxprod+Xpq+1].prolon;

  if ((cur_PijKs_stack_top + top-hook) >= cur_PijKs_stack_size) {
    cur_PijKs_stack_size += top-hook;

    if (++cur_PijKs_stack_list_top >= cur_PijKs_stack_list_size) {
      cur_PijKs_stack_list_size *= 2;
      item2PijKs_stack_list = (int**) sxrealloc (item2PijKs_stack_list, cur_PijKs_stack_list_size, sizeof (int*));
    }

    item2PijKs_stack_list [cur_PijKs_stack_list_top] = cur_PijKs_stack =
      (int *) sxalloc (cur_PijKs_stack_size+1, sizeof (int));
    cur_PijKs_stack_top = 0;

#if LLOG
    total_cur_PijKs_stack_size += cur_PijKs_stack_size+1;
#endif /* LLOG */
  }

  /* Resultat que l'on va remplir */
  base_ptr = item2PijKs_list [item] = cur_PijKs_stack+cur_PijKs_stack_top;

  PijKs_ptr = base_ptr;
  min_ctxt_weight = 0;
  is_first_time = TRUE;
  not_in_stats = FALSE;

  father_eprod = Pij2eprod [Pij];

  cur_contextual_proba_kind = contextual_proba_kind;

  if (father_eprod == 0) {
    if (contextual_proba_kind >= EP_EPCFG)
      cur_contextual_proba_kind = P_EPCFG;
  }
  else {
    if (cur_contextual_proba_kind == EP_POS_EPCFG) {
      full_ctxt = XxY_is_set (&full_ctxt_hd, father_eprod, pos);

      if (full_ctxt == 0)
	cur_contextual_proba_kind = EP_EPCFG;
    }
  }

  /* 1ere passe pour remplir les proba contextuelles */
  while ((Ppq = spf.outputG.rhs [hook++].lispro) != 0) {
    if (Ppq > 0) {
      XxY_set (&PijxItem_hd, Ppq, item, &PpqK);

      son_eprod = Pij2eprod [Ppq];

      switch (cur_contextual_proba_kind) {
      case EP_POS_EPCFG:
	couple = XxY_is_set (&full_ctxtXeprod_hd, full_ctxt, son_eprod);
	ctxt_weight = couple ? full_ctxtXeprod2logprobs [couple] : 1;
	break;
      case EP_EPCFG:
	couple = XxY_is_set (&eprod_ctxtXeprod_hd, father_eprod, son_eprod);
	ctxt_weight = couple ? eprod_ctxtXeprod2logprobs [couple] : 1;
	break;
      case P_EPCFG:
	couple = XxY_is_set (&prod_ctxtXeprod_hd, init_prod, son_eprod);
	ctxt_weight = couple ? prod_ctxtXeprod2logprobs [couple] : 1;
	break;
      case A_EPCFG:
	couple = XxY_is_set (&A_ctxtXeprod_hd, A, son_eprod);
	ctxt_weight = couple ? A_ctxtXeprod2logprobs [couple] : 1;
	break;
      default: /* pour faire taire gcc -Wswitch-default */
	sxinitialise(ctxt_weight); /* pour faire taire gcc -Wuninitialized */
	sxinitialise(couple); /* pour faire taire gcc -Wuninitialized */
#if EBUG
	sxtrap(ME,"unknown switch case #3");
#endif
	break;
      }

      if (couple) {
	local_weight = Pij2one_best_local_weight [Ppq];
	total_weight = local_weight+ctxt_weight;

	if (is_first_time || (total_weight > best_total_weight)) {
	  is_first_time = FALSE;
	  best_total_weight = total_weight;
	  best_PijKs_ptr = PijKs_ptr;
	}
      }
      else {
	/* Les stats utilisees ne sont pas suffisamment completes, on va utiliser des moins contraintes ...*/
	/* Ces moins contraintes sont examinees dans l'ordre, leurs probas etant reajustees par la proba
	   min de l'essai precedant */
	not_in_stats = TRUE;
      }

      *PijKs_ptr = PpqK;
      PijK2ctxt_elem_list [PpqK].ctxt_weight = ctxt_weight;

      PijK2ctxt_elem_list [PpqK].signet = signet_elem_ptr = get_new_signet_elem ();

      signet_elem.total_weight = couple ? total_weight : 0; /* Le 16/04/07 */
      signet_elem.signature = unit_vector_signature;
      signet_elem.next = NULL;
      *signet_elem_ptr = signet_elem;
      Pij2attr [PpqK].signature_nb = 1;
      Pij2attr [PpqK].signpost = 0;

      PijKs_ptr++;
    }
  }

  cur_PijKs_stack_top = PijKs_ptr-cur_PijKs_stack;

  if (not_in_stats) {
    do {
      /* On met a jour les not_in_stats dans l'ordre */
      not_in_stats = FALSE;
      prev_min_ctxt_weight = min_ctxt_weight;
      min_ctxt_weight = 0;
      PijKs_ptr = base_ptr;
      hook = base_hook;

      cur_contextual_proba_kind--;

#if EBUG
      if (cur_contextual_proba_kind < PCFG)
	sxtrap (ME, "set_item_weight_list");
#endif /* EBUG */

      while ((Ppq = spf.outputG.rhs [hook++].lispro) != 0) {
	if (Ppq > 0) {
	  PpqK = *PijKs_ptr;

	  if (PijK2ctxt_elem_list [PpqK].ctxt_weight == 1) {
	    switch (cur_contextual_proba_kind) {
	    case EP_EPCFG:
	      son_eprod = Pij2eprod [Ppq];
	      couple = XxY_is_set (&eprod_ctxtXeprod_hd, father_eprod, son_eprod);
	      ctxt_weight = eprod_ctxtXeprod2logprobs [couple];
	      break;

	    case P_EPCFG:
	      son_eprod = Pij2eprod [Ppq];
	      couple = XxY_is_set (&prod_ctxtXeprod_hd, init_prod, son_eprod);
	      ctxt_weight = prod_ctxtXeprod2logprobs [couple];
	      break;

	    case A_EPCFG:
	      son_eprod = Pij2eprod [Ppq];
	      couple = XxY_is_set (&A_ctxtXeprod_hd, A, son_eprod);
	      ctxt_weight = A_ctxtXeprod2logprobs [couple];
	      break;

	    case EPCFG:
	      couple = Pij2eprod [Ppq];
	      ctxt_weight = eprod2logprobs [couple];
	      break;

	    case PCFG:
	      couple = spf.outputG.lhs [Ppq].init_prod;
	      ctxt_weight = prod2logprobs [couple];
	      break;
	    default: /* pour faire taire gcc -Wswitch-default */
	      sxinitialise(couple); /* pour faire taire gcc -Wuninitialized */
	      sxinitialise(ctxt_weight); /* pour faire taire gcc -Wuninitialized */
#if EBUG
	      sxtrap(ME,"unknown switch case #4");
#endif
	      break;
	    }

	    if (couple) {
	      local_weight = Pij2one_best_local_weight [Ppq];
	      PijK2ctxt_elem_list [PpqK].ctxt_weight = ctxt_weight += prev_min_ctxt_weight;
	      PijK2ctxt_elem_list [PpqK].signet->total_weight = total_weight = local_weight+ctxt_weight;


	      if (is_first_time || (total_weight > best_total_weight)) {
		is_first_time = FALSE;
		best_total_weight = total_weight;
		best_PijKs_ptr = PijKs_ptr;
	      }
	    }
	    else {
	      not_in_stats = TRUE;
	    }
	  }

	  PijKs_ptr++;
	}
      }
    } while (not_in_stats);
  }

#if EBUG
  if (is_first_time)
    sxtrap (ME, "set_item_weight_list");
#endif /* EBUG */

  item2cur_contextual_proba_kind [item] = cur_contextual_proba_kind;

#if LOG
  PijKs_ptr = base_ptr;
  hook = base_hook;
  fputs ("<<<<<<", stdout);
  print_ctxt (item);
  fputs (" >>>>>>\n", stdout);

  while ((Ppq = spf.outputG.rhs [hook++].lispro) != 0) {
    if (Ppq > 0) {
      PpqK = *PijKs_ptr;

      printf ("\tctxt weight=%f for ", PijK2ctxt_elem_list [PpqK].ctxt_weight);
      spf_print_iprod (stdout, XxY_X (PijxItem_hd, PpqK), "", "\n");

      PijKs_ptr++;
    }
  }
#endif /* LOG */

  /* Ici best_total_weight et best_PijKs_ptr sont positionnes */
  set_ctxt_item_weight (item, best_PijKs_ptr, 1 /* k */);

  return best_total_weight;
}



/* item repere un Tpq en rhs d'une Pij.  calcule sa proba */
double
get_Tpq_logprob (int item, int cur_contextual_proba_kind)
{
  int        Pij, Tpq, t, ff_id, lex_id, init_prod, A, full_ctxt, couple;
  double     weight;
  struct rhs *prhs;
  struct lhs *plhs;

  weight = Tpq_item2weight [item];

  if (weight != 0)
    return weight == 1 ? 0 : weight;

  prhs = spf.outputG.rhs + item;
  Tpq = -prhs->lispro;
  t = -Tij2T (Tpq);
  ff_id = parser_Tpq2tok (Tpq)->lahead;
  lex_id = XxY_is_set (&ff_idXt2lex_id_hd, ff_id, t);

  if (lex_id == 0) {
    /* Le couple (ff_id, t) n'est pas dans les tables statiques :
       - soit il n'a jamais ete trouve' ds le corpus
       - soit ff_id n'est pas ambigu */
    Tpq_item2weight [item] = 1;

    return 0;
  }

  Pij = prhs->prolis;
  plhs = spf.outputG.lhs + Pij;
  init_prod = plhs->init_prod;

  switch (cur_contextual_proba_kind) {
  case EP_POS_EPCFG:
    full_ctxt = XxY_is_set (&full_ctxt_hd, Pij2eprod [Pij], item - plhs->prolon);

    if (full_ctxt && (couple = XxY_is_set (&full_ctxtXlex_id_hd, full_ctxt, lex_id))) {
      weight = full_ctxtXlex_id2logprobs [couple];
      break;
    }

  case EP_EPCFG:
    if ((couple = XxY_is_set (&eprod_ctxtXlex_id_hd, Pij2eprod [Pij], lex_id))) {
      weight = eprod_ctxtXlex_id2logprobs [couple];
      break;
    }

  case P_EPCFG:
    if ((couple = XxY_is_set (&prod_ctxtXlex_id_hd, init_prod, lex_id))) {
      weight = prod_ctxtXlex_id2logprobs [couple];
      break;
    }

  case A_EPCFG:
    A = spf.inputG.lhs [init_prod];

    if ((couple = XxY_is_set (&A_ctxtXlex_id_hd, A, lex_id))) {
      weight = A_ctxtXlex_id2logprobs [couple];
      break;
    }

  case EPCFG:
  case PCFG:
    weight = lex_id2logprobs [lex_id];
    break;
  default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap(ME,"unknown switch case #5");
#endif
    break;
  }

  Tpq_item2weight [item] = (weight == 0) ? 1 : weight;

  return weight;
}


/* On est ds le cas contextual_proba_kind > EPCFG
   Si Pij = Aij -> ... Bpq ....
   On va associer a l'item reperant Bpq en position pos ds la RHS, une dedicace de niveau k=extract_vector[pos]
   Le poids de cette dedicace sera la valeur max des probas associees aux triplets (A, prod, val)
   Ou prod designe une CFG production des Bpq-prods (Ppq) et val le Pij2eprod [Ppq] associe'
 */
static double
compute_Pij_ctxt_weight (Pij)
     int Pij;
{
  int               item, Xpq;
  double            weight, Xpq_weight;

#if LLLOG
  printf ("********** Entering compute_Pij_ctxt_weight (Pij=%i) **********\n", Pij);
#endif /* LOG */

  item = spf.outputG.lhs [Pij].prolon;
  weight = 0;

  while ((Xpq = spf.outputG.rhs [item].lispro) != 0) {
    if (Xpq < 0) {
      /* cas particulier d'un terminal */
      Xpq_weight = get_Tpq_logprob (item, contextual_proba_kind);
    }
    else {
      /* On va associer a item la liste (// aux Ppq valides de Xpq) des poids du
	 triplet (A, init_prod(Ppq), val(Ppq)) */
      Xpq_weight = set_item_weight_list (item);
    }

    weight += Xpq_weight;
    item++;
  }  

#if LLLOG
  printf ("********** Leaving compute_Pij_ctxt_weight (Pij=%i) son's total ctxt weight=%f **********\n", Pij, weight);
#endif /* LOG */

  return weight;
}

/* a chaque fils de Pij en position pos, extract_vector [pos] contient un indice
   qui permet de recuperer le poids du fils */
static double
compute_Pij_local_weight (Pij, signature, ctxt_weight_ptr)
     int    Pij, signature;
     double *ctxt_weight_ptr;
{
  int               item, Xpq, pos;
  int               *extract_vector;
  double            weight, Xpq_weight;
  struct dedication *dedication_ptr;
  struct lhs        *plhs;

#if LLLOG
  printf ("********** Entering compute_Pij_local_weight (Pij=%i, vector=%i", Pij, signature);
  print_signature (signature);
  fputs (") **********\n", stdout);
#endif /* LOG */

  extract_vector = &(XH_list_elem (signatures_hd, XH_X (signatures_hd, signature)));

  plhs = spf.outputG.lhs + Pij;
  item = plhs->prolon;
  weight = 0;
  pos = 0;

  while ((Xpq = spf.outputG.rhs [item].lispro) != 0) {
    if (Xpq < 0) {
      /* cas particulier d'un terminal */
      Xpq_weight = get_Tpq_logprob (item, contextual_proba_kind);
    }
    else {
      dedication_ptr = get_dedication_ptr ((contextual_proba_kind > EPCFG) ? item : Xpq, extract_vector [pos]);
      Xpq_weight = (dedication_ptr) ? dedication_ptr->total_weight : 0;
    }

    weight += Xpq_weight;
    pos++;
    item++;
  }

  if (ctxt_weight_ptr == NULL)
    weight += (contextual_proba_kind == PCFG) ? prod2logprobs [plhs->init_prod] : eprod2logprobs [Pij2eprod [Pij]];
  else
    weight += *ctxt_weight_ptr;

#if EBUG
  if (weight > 0)
    sxtrap (ME, "compute_Pij_local_weight");
#endif /* EBUG */  

#if LLLOG
  printf ("********** Leaving compute_Pij_local_weight (Pij=%i, vector=%i", Pij, signature);
  print_signature (signature);
  printf (") weight=%f **********\n", weight);
#endif /* LOG */

  return weight;
}


/* Pour toutes les Aij_prods, selectionne le meilleur poids et stocke le resultat ds dedication_list */
static BOOLEAN
get_Aij_or_item_best_weight (Aij_or_item, k)
     int Aij_or_item, k;
{
  int                Aij, hook, Pij, best_signature, best_Pij, couple, next;
  double             best_weight;
  struct dedication  *dedication_ptr;
  struct signet_elem *signet_elem_ptr;
  BOOLEAN            is_fully_computed;
#if LOG
  int signature;
#endif

  sxinitialise(best_weight);  /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(best_signature);  /* pour faire taire "gcc -Wuninitialized" */
  Aij = (contextual_proba_kind > EPCFG) ? spf.outputG.rhs [Aij_or_item].lispro : Aij_or_item;

#if LOG
  fputs ("********** Entering get_Aij_or_item_best_weight (", stdout);

  spf_print_Xpq (stdout, Aij);

  printf (", k=%i", k);

  if (contextual_proba_kind > EPCFG) {
    print_ctxt (Aij_or_item);
  }

  fputs (")\n", stdout);
#endif /* LOG */

  if (contextual_proba_kind > EPCFG) {
    set_ctxt_item_weight (Aij_or_item, NULL, k);
  }
  else {
    hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij_or_item].prolon;

    best_Pij = 0;

    while ((Pij = spf.outputG.rhs [hook++].lispro) != 0) {
      if (Pij > 0) {
	signet_elem_ptr = Pij2signet [Pij];

	if (signet_elem_ptr) {
#if LOG
	  spf_print_iprod (stdout, Pij, "", "");
	  printf (" total weight=%f, signature=%i", signet_elem_ptr->total_weight, signet_elem_ptr->signature);
	  print_signature (signet_elem_ptr->signature);
	  putchar ('\n');
#endif /* LOG */

	  if (best_Pij == 0 || signet_elem_ptr->total_weight > best_weight) {
	    best_weight = signet_elem_ptr->total_weight;
	    best_Pij = Pij; /* A FAIRE, maintenant on stocke PijS */
	    best_signature = signet_elem_ptr->signature;
	  }
	}
      }
    }

    if (best_Pij) {
#if EBUG
      if (Aij_or_item2last_dedication [Aij_or_item] == 0)
	sxtrap (ME, "get_Aij_or_item_best_weight");
#endif /* EBUG */

      next = Aij_or_item2last_dedication [Aij_or_item];

#if EBUG
      if (k != dedication_list [next].k+1)
	sxtrap (ME, "get_Aij_or_item_best_weight");
#endif /* EBUG */

      XxY_set (&dedication_hd, Aij, k, &couple);
      dedication_ptr = dedication_list+couple;

      dedication_ptr->PijK = -best_Pij; /* Pour LOG */
      dedication_ptr->signature = best_signature;
      dedication_ptr->k = k;
      dedication_ptr->next = next;
      dedication_ptr->total_weight = best_weight;
      Aij_or_item2last_dedication [Aij_or_item] = couple;

      if (is_filtering)
	SXBA_1_bit (best_Pij_set, best_Pij);

#if LOG
      spf_print_Xpq (stdout, Aij);
      printf (", couple(Aij=%i, k=%i)=%i: k=%i, cur_best_weight=%f, Pij=%i, signature=%i",
	      Aij, k, couple, dedication_ptr->k, best_weight, best_Pij, best_signature);
      print_signature (best_signature);
      putchar ('\n');
#endif /* LOG */
    }
    else {
      SXBA_1_bit (fully_computed_Aij_or_item_set, Aij_or_item);
    }
  }

  is_fully_computed = SXBA_bit_is_set (fully_computed_Aij_or_item_set, Aij_or_item);

#if LOG
  fputs ("********** Leaving get_Aij_or_item_best_weight (", stdout);

  spf_print_Xpq (stdout, Aij);

  printf (", k=%i", k);

  if (contextual_proba_kind > EPCFG) {
    print_ctxt (Aij_or_item);
  }

  putchar (')');

  if (is_fully_computed) {
    fputs (" (erased)", stdout);
  }

  couple = Aij_or_item2last_dedication [Aij_or_item];
  dedication_ptr = dedication_list+couple;

  if (contextual_proba_kind > EPCFG)
    Pij = XxY_X (PijxItem_hd, dedication_ptr->PijK);
  else
    Pij = -dedication_ptr->PijK;

  signature = dedication_ptr->signature;

  printf (", couple=%i(Pij, signature): k=(%i/%f), weight=%f, Pij=%i, signature=%i", couple, dedication_ptr->k, spf.tree_count.nt2nb [Aij],
	  dedication_ptr->total_weight, Pij, signature);
  print_signature (signature);
  putchar ('\n');
#endif /* LOG */

  return !is_fully_computed;
}



/*
  The contents of the array are sorted in ascending order according to  a
  comparison  function  pointed  to  by  compar, which is called with two
  arguments that point to the objects being compared.

  int(*compar)(const void *, const void *))

  The comparison function must return an integer less than, equal to,  or
  greater  than  zero  if  the first argument is considered to be respec-
  tively less than, equal to, or greater than the second.  If two members
  compare as equal, their order in the sorted array is undefined.
*/
static int
signet_cmp (p1, p2)
     struct local_signet_elem *p1, *p2;
{
  double w = p1->weight - p2->weight;

  return (w <= 0) ? ((w == 0) ? 0 : 1) : -1;
}


/* On fabrique (ou on reorganise) la dichotomie de Pij */
/* Soit p tq (Pij, 0), ..., (Pij, p) existe avant la reorg
   On va rajouter un (Pij, p+1)
   La taille de chaque segment sera donc segment = signature_nb/(p+1)

   On parcourt sequenciellement les signatures et sur la k*segment (k = 1, ... p+1) on met
   un ptr vers elle associe' a (Pij, k) (qui existe deja sauf si k==p+1)
*/
static void
reorg_dicho (PijK)
     int PijK;
{
  int                      signature_nb, old_signpost, new_signpost, new_slice_size, cur_signpost, cur_slice_lgth, couple;
  double                   prev_weight, cur_weight;
  struct signet_elem       *signet_elem_ptr, *prev_signet_elem_ptr;
  struct Pij_attr          *Pij_attr_ptr;

  Pij_attr_ptr = Pij2attr+PijK;

  signature_nb = Pij_attr_ptr->signature_nb;
  old_signpost = Pij_attr_ptr->signpost;
  new_signpost = signature_nb>>LOG2_SLICE_SIZE;
  /* On peut avoir new_signpost == old_signpost si le ptr de tete et le pointeur de la 2eme tranche sont identiques */

  /* Il faut faire de la reorganisation */
  new_slice_size = (signature_nb+1)/(new_signpost+1);
  Pij_attr_ptr->signpost = new_signpost;

  cur_signpost = 0;
  signet_elem_ptr = Pij2signet ? Pij2signet [PijK] : PijK2ctxt_elem_list [PijK].signet;;
  sxinitialise(prev_signet_elem_ptr); /* pour faire taire "gcc -Wuninitialized" */

  do {
    cur_slice_lgth = 0;

    while (++cur_slice_lgth <= new_slice_size) {
      if (signet_elem_ptr == NULL)
	/* garde-fou car on ne decoupe pas les poids identiques */
	break;

      prev_signet_elem_ptr = signet_elem_ptr;
      signet_elem_ptr = signet_elem_ptr->next;
    }

    if (signet_elem_ptr) {
      /* on ne decoupe pas les poids identiques */
      prev_weight = prev_signet_elem_ptr->total_weight;
      cur_weight = signet_elem_ptr->total_weight;

      while (prev_weight == cur_weight) {
	signet_elem_ptr = signet_elem_ptr->next;

	if (signet_elem_ptr == NULL)
	  /* garde-fou car on ne decoupe pas les poids identiques */
	  break;

	cur_weight = signet_elem_ptr->total_weight;
      }
    }

    XxY_set (&Pijxsignpost_hd, PijK, ++cur_signpost, &couple);

    Pijxsignpost2signet [couple] = signet_elem_ptr; /* peut etre NULL ds les cas tordus */

  } while (cur_signpost < new_signpost);
}


/* Retourne un ptr tq weight <= ptr->weight, on peut donc inserer weight derriere ptr */
/* On a *lower_signpost_ptr < upper_signpost */
/* On est sur que le ptr retourne' repere un poids strictement inferieur au poids repere' par *lower_signpost_ptr */
static struct signet_elem*
signpost_seek (PijK, lower_signpost_ptr, upper_signpost, weight)
     int    PijK, *lower_signpost_ptr, upper_signpost;
     double weight;
{
  int                      couple, signpost, lower_signpost;
  struct signet_elem       *signet_elem_ptr, *lower_signet_elem_ptr, *upper_signet_elem_ptr;

  lower_signpost = *lower_signpost_ptr;

  if (lower_signpost == 0)
    lower_signet_elem_ptr = Pij2signet ? Pij2signet [PijK] : PijK2ctxt_elem_list [PijK].signet;
  else {
    couple = XxY_is_set (&Pijxsignpost_hd, PijK, lower_signpost);
    lower_signet_elem_ptr = Pijxsignpost2signet [couple];
  }

#if EBUG
  if (lower_signet_elem_ptr == NULL || lower_signpost == upper_signpost)
    sxtrap (ME, "signpost_seek");
#endif /* EBUG */

  couple = XxY_is_set (&Pijxsignpost_hd, PijK, upper_signpost);
  upper_signet_elem_ptr = Pijxsignpost2signet [couple];

  for (;;) {
    signpost = (lower_signpost+upper_signpost)/2;

    if (signpost == lower_signpost) {
      if (upper_signet_elem_ptr == NULL || weight > upper_signet_elem_ptr->total_weight) {
	*lower_signpost_ptr = lower_signpost;
	return (weight <= lower_signet_elem_ptr->total_weight) ? lower_signet_elem_ptr : NULL;
      }

      *lower_signpost_ptr = upper_signpost;
      return upper_signet_elem_ptr;
    }

    couple = XxY_is_set (&Pijxsignpost_hd, PijK, signpost);
    signet_elem_ptr = Pijxsignpost2signet [couple];

    if (signet_elem_ptr == NULL || weight > signet_elem_ptr->total_weight) {
      upper_signpost = signpost;
      upper_signet_elem_ptr = signet_elem_ptr;
    }
    else {
      lower_signpost = signpost;
      lower_signet_elem_ptr = signet_elem_ptr;
    }
  }
}
     

/* calcule par fusion avec local_signet_stack [0..nb-1] et recherche dichotomique un nouveau signet_stack */
static void
merge (PijK, nb)
     int PijK, nb;
{
  int                      cur, lower_signpost, upper_signpost, prev_lower_signpost;
  struct signet_elem       *signet_elem_ptr, *ptr, signet_elem, **prev_signet_elem_ptr;
  struct Pij_attr          *Pij_attr_ptr;

  Pij_attr_ptr = Pij2attr+PijK;

  upper_signpost = Pij_attr_ptr->signpost;
  lower_signpost = 0;

  /* On enleve le 1er elem */
  Pij_attr_ptr->signature_nb--;
  /*  prev_signet_elem_ptr = Pij2signet+Pij; */
  prev_signet_elem_ptr = Pij2signet ? Pij2signet+PijK : &(PijK2ctxt_elem_list [PijK].signet);

  *prev_signet_elem_ptr = (signet_elem_ptr = *prev_signet_elem_ptr)->next;
  release_signet_elem (signet_elem_ptr);
  signet_elem_ptr = *prev_signet_elem_ptr;
  
  /* ... et on y insere les elems de local_signet_stack */
  for (cur = 0; cur < nb; cur++) {
    signet_elem.signature = local_signet_stack [cur].signature;
    signet_elem.total_weight = local_signet_stack [cur].weight;

    if (signet_elem_ptr && (lower_signpost < upper_signpost)) {
      /* Y'a de la dicho ds l'air */
      prev_lower_signpost = lower_signpost;

      ptr = signpost_seek (PijK, &lower_signpost, upper_signpost, signet_elem.total_weight);

      if (ptr && (ptr->signature == signet_elem.signature)) {
	/* elem deja existant, on continue */
	continue;
      }

      if (ptr && (prev_lower_signpost < lower_signpost)) {
	/* On redemarre sur une nouvelle tranche */
	prev_signet_elem_ptr = &(ptr->next);
	signet_elem_ptr = *prev_signet_elem_ptr;
      }
    }

    while (signet_elem_ptr && (signet_elem.total_weight <= signet_elem_ptr->total_weight)) {
      if (signet_elem_ptr->signature == signet_elem.signature) {
	/* elem deja existant, on continue */
	break;
      }

      prev_signet_elem_ptr = &(signet_elem_ptr->next);
      signet_elem_ptr = *prev_signet_elem_ptr;
    }

    if (signet_elem_ptr && (signet_elem.total_weight <= signet_elem_ptr->total_weight))
      /* elem deja existant, on continue */
      continue;

    signet_elem.next = signet_elem_ptr;

    ptr = get_new_signet_elem ();

    Pij_attr_ptr->signature_nb++;

    *ptr = signet_elem;
    *prev_signet_elem_ptr = ptr;
    prev_signet_elem_ptr = &(ptr->next);
  }

  if ((Pij_attr_ptr->signature_nb>>LOG2_SLICE_SIZE) != Pij_attr_ptr->signpost) {
    /* On ne reorganise que si la taille a augmente' ... */
    /* ... Essai, si le nb de tranches a change' ... */
    reorg_dicho (PijK);
  }
  else {
    /* ... ou si Pij2signet[Pij] et Pijxsignpost2signet[(Pij, 1)] reperent tous les 2 le debut */
    if (upper_signpost) {
      /* Y'a de la dicho ... */
      signet_elem_ptr = Pijxsignpost2signet [XxY_is_set (&Pijxsignpost_hd, PijK, 1)];

      if (signet_elem_ptr == (Pij2signet ? Pij2signet [PijK] : PijK2ctxt_elem_list [PijK].signet)) {
	reorg_dicho (PijK);
      }
    }
  }
}


/* signet est l'ensemble des signatures associees a Pij */
/* si une signature est (i0, i1, ..., ip) son successeur est l'ensemble {(j0, j1, ..., jp)}
   ou pour tout k, 0<= k <= p on a si h != k alors jh==ih sinon jk=ik+1, (si ik+1 < nbest) */
/* Fabrique un nouveau signet par l'operation successeur */
/* Nelle version, les signets sont tries par poids decroissant */
static void
next_signet (dedication_ptr)
     struct dedication *dedication_ptr;
{
  int                      PijK, Pij, prev_signature, signature, signature_nb, cur1, bot2, cur2, top2, item, top_index, Xpq, hi, nb, level;
  int                      Xpq_or_item, cur_item, bot_item, top_item;
  double                   ctxt_weight, *ctxt_weight_ptr;
  struct signet_elem       *signet_elem_ptr, **signet_elem_ptr_ptr;
  struct local_signet_elem local_signet_elem;
  struct Pij_attr          *Pij_attr_ptr;

  if (contextual_proba_kind > EPCFG) {
    PijK = dedication_ptr->PijK;
    signet_elem_ptr = PijK2ctxt_elem_list [PijK].signet;
    ctxt_weight_ptr = &ctxt_weight;
    *ctxt_weight_ptr = PijK2ctxt_elem_list [PijK].ctxt_weight;
    Pij = XxY_X (PijxItem_hd, PijK);
  }
  else {
    PijK = Pij = -dedication_ptr->PijK;
    signet_elem_ptr = Pij2signet [Pij];
    ctxt_weight_ptr = NULL;
  }

  if (signet_elem_ptr) {
    prev_signature = signet_elem_ptr->signature;

#if LOG
    printf ("********** Entering next_signet (Pij=%i) **********\n", Pij);
    spf_print_iprod (stdout, Pij, "", "");
    printf (" signature=%i", prev_signature);
    print_signature (prev_signature);
    printf (", weight=%f\n", signet_elem_ptr->total_weight);
#endif /* LOG */

#if EBUG
    if (signet_elem_ptr->signature != prev_signature)
      /* le meilleur est au debut */
      sxtrap (ME, "next_signet");
#endif /* EBUG */

    /* Ne pas tenir compte de la taille d'une signature (elle a au moins la bonne taille mais
     elle peut etre + longue, cas signature==unit_vector_signature) */
    cur2 = bot2 = XH_X (signatures_hd, prev_signature);
    item = bot_item = spf.outputG.lhs [Pij].prolon;
    top_item = spf.outputG.lhs [Pij+1].prolon-1;

    SS_open (SS_signet_stack);

    while ((Xpq = spf.outputG.rhs [item].lispro) != 0) {
      /* On suppose que les terminaux n'ont qu'un element */
      if (Xpq > 0) {
	Xpq_or_item = (contextual_proba_kind > EPCFG) ? item : Xpq;

	if (!SXBA_bit_is_set (fully_computed_Aij_or_item_set, Xpq_or_item)) {
	  /* Attention Xpq [hi] peut deja etre calcule' */
	  hi = XH_list_elem (signatures_hd, cur2)+1;

	  if (Aij_or_item_kth_best (Xpq_or_item, hi)) {
	    /* du nouveau */
	    for (cur_item = bot_item, cur1 = bot2; cur_item < top_item; cur_item++, cur1++) {
	      if (cur1 == cur2)
		XH_push (signatures_hd, hi);
	      else {
		level = XH_list_elem (signatures_hd, cur1);
		XH_push (signatures_hd, level);
	      }
	    }

	    XH_set (&signatures_hd, &signature);
	    SS_push (SS_signet_stack, signature);
	  }
	}
      }
	  
      cur2++;
      item++;
    }

    top2 = SS_top (SS_signet_stack);
    bot2 = SS_bot (SS_signet_stack);
    nb = top2-bot2;

    if (nb) {
      /* non vide */
      if (nb > 1) {
	/* On le trie */
	/*
	  NAME
	  qsort - sorts an array

	  SYNOPSIS
	  #include <stdlib.h>

	  void qsort(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *));

	  DESCRIPTION
	  The  qsort()  function sorts an array with nmemb elements of size size.
	  The base argument points to the start of the array.

	  RETURN VALUE
	  The qsort() function returns no value.
	*/
	top_index = 0;

	for (cur2 = bot2; cur2 < top2; cur2++) {
	  local_signet_elem.signature = SS_signet_stack [cur2];
	  local_signet_elem.weight = compute_Pij_local_weight (Pij, local_signet_elem.signature, ctxt_weight_ptr);
	  local_signet_stack [top_index++] = local_signet_elem;
	}

	qsort (local_signet_stack, (size_t)nb, sizeof (struct local_signet_elem), (int(*)(const void *, const void *)) signet_cmp);
      }
      else {
	local_signet_stack->signature = signature = SS_signet_stack [bot2];
	local_signet_stack->weight = compute_Pij_local_weight (Pij, signature, ctxt_weight_ptr);
      }

      /* On imbrique les 2 structures triees */
      merge (PijK, nb);
    }
    else {
      /* On enleve le 1er elem */
      Pij_attr_ptr = Pij2attr+PijK;

      signature_nb = --Pij_attr_ptr->signature_nb;

      signet_elem_ptr_ptr = Pij2signet ? Pij2signet+PijK : &(PijK2ctxt_elem_list [PijK].signet);
      *signet_elem_ptr_ptr = (signet_elem_ptr = *signet_elem_ptr_ptr)->next;

      release_signet_elem (signet_elem_ptr);

      signet_elem_ptr = *signet_elem_ptr_ptr;

      if (signet_elem_ptr) {
	/* On reorg si Pij2signet[Pij] et Pijxsignpost2signet[(Pij, 1)] reperent tous les 2 le debut */
	if (Pij_attr_ptr->signpost) {
	  /* Y'a de la dicho ... */
	  if ((signature_nb>>LOG2_SLICE_SIZE) != Pij_attr_ptr->signpost) {
	    /* On ne reorganise que si le nb de tranches a change' ... */
	    reorg_dicho (PijK);
	  }
	  else {
	    if (signet_elem_ptr == Pijxsignpost2signet [XxY_is_set (&Pijxsignpost_hd, PijK, 1)]) {
	      reorg_dicho (PijK);
	    }
	  }
	}
      }
    }

    SS_close (SS_signet_stack);

#if LOG
    printf ("********** Leaving next_signet (Pij=%i)", Pij);

    if (signet_elem_ptr == NULL)
      fputs (" (erased)", stdout);
    else {
      printf (" signature=%i", signet_elem_ptr->signature);
      print_signature (signet_elem_ptr->signature);
      printf (", total weight=%f", signet_elem_ptr->total_weight);
    }

    fputs (" **********\n", stdout);
#endif /* LOG */
  }
}



/* Retourne le kieme meilleur resultat associe a Aij_or_item */
static BOOLEAN
Aij_or_item_kth_best (Aij_or_item, k)
     int Aij_or_item, k;
{
  struct dedication *dedication_ptr;

  if (SXBA_bit_is_set (fully_computed_Aij_or_item_set, Aij_or_item))
    return FALSE;

#if EBUG
  if (Aij_or_item2last_dedication [Aij_or_item] == 0)
    sxtrap (ME, "Aij_or_item_kth_best");
#endif /* EBUG */

  dedication_ptr = dedication_list+Aij_or_item2last_dedication [Aij_or_item];

  if (dedication_ptr->k >= k)
    /* La valeur demandee est deja calculee */
    return TRUE;

  /* ici non encore calculee, on le fait */
#if EBUG
  if (k > dedication_ptr->k+1)
    sxtrap (ME, "Aij_or_item_kth_best");
#endif /* EBUG */  

  next_signet (dedication_ptr);

  /* Ici Pij a pu disparaitre */

  /* Pour toutes les Aij_prods (dont Pij), selectionner le meilleur poids et donc retourner un new_best = (new_Pij, new_best_signature)
     que l'on push ds local_dedication_stack */
  return get_Aij_or_item_best_weight (Aij_or_item, k);
}

static int
one_best_post_bu_walk (Aij)
     int Aij;
{
  int               dedication;
#if LOG
  double            tree_nb;
#endif /* LOG */
  struct dedication *dedication_ptr;

  XxY_set (&dedication_hd, Aij, 1, &dedication);
  Aij2last_dedication [Aij] = dedication;
  dedication_ptr = dedication_list+dedication;
  dedication_ptr->PijK = -best_Aij_Pij;
  dedication_ptr->signature = unit_vector_signature;
  dedication_ptr->k = 1;
  dedication_ptr->next = 0;
  dedication_ptr->total_weight = best_Aij_local_weight;

  if (is_filtering)
    SXBA_1_bit (best_Pij_set, best_Aij_Pij);

#if 0
  /* On n'appelle + spf_tree_count si pas LOG */
  tree_nb = spf.tree_count.nt2nb [Aij];

  if (nbest > 1 && tree_nb == 1)
    SXBA_1_bit (fully_computed_Aij_or_item_set, Aij);
#endif /* 0 */

#if LOG
  spf_print_Xpq (stdout, Aij);
  printf (" : 1st_best_weight=%f for Pij=%i (signature=%i",
	  dedication_ptr->total_weight, best_Aij_Pij, unit_vector_signature);
  print_signature (unit_vector_signature);
  tree_nb = spf.tree_count.nt2nb [Aij];
  printf (", tree_nb=%f)\n", tree_nb);
#endif /* LOG */

  best_Aij_local_weight = -DBL_MAX; /* pour le coup d'apres */

  return 1;
}


/* Fait la 1ere passe sur la foret et calcule le one best */
static int
one_best_bu_walk (Pij)
     int Pij;
{
  double                   local_weight;
  struct signet_elem       *signet_elem_ptr, signet_elem;

  if (contextual_proba_kind > EPCFG) {
    /* Calcule l'influence du contexte (Pij) sur les poids des fils de Pij */
    /* Cette influence est calculee une fois pour toute lors du calcul de one_best
       car elle ne depend pas des signatures des fils de Pij */
    /* Le resultat, pour chaque item de la RHS qui repere un Apq est range' ds item2PijKs_list [item]
       qui est un vecteur de couples (PijK, signature) dont les elements sont // aux Apq-prods
       PijK=(Pij, item) permet d'acceder au poids contextuel PijK2ctxt_elem_list [PijK].ctxt_weight qui est fixe
       et au signet.
    */
    local_weight = compute_Pij_ctxt_weight (Pij);

    if (nbest > 1) {
      Pij2attr [Pij].signature_nb = 1;
      Pij2attr [Pij].signpost = 0;
    }
  }
  else {
    local_weight = compute_Pij_local_weight (Pij, unit_vector_signature, NULL);

    if (best_Aij_local_weight < local_weight) {
      /* Pour one_best_post_bu_walk () */
      best_Aij_local_weight = local_weight;
      best_Aij_Pij = Pij;
    }

    if (nbest > 1) {
      /* On prepare les passes suivantes */
      Pij2signet [Pij] = signet_elem_ptr = get_new_signet_elem ();
      signet_elem.total_weight = local_weight;
      signet_elem.signature = unit_vector_signature;
      signet_elem.next = NULL;
      *signet_elem_ptr = signet_elem;
      Pij2attr [Pij].signature_nb = 1;
    }
  }

  Pij2one_best_local_weight [Pij] = local_weight;

#if LOG
  fputs ("one_best_bu_walk: ", stdout);
  spf_print_iprod (stdout, Pij, "", "");
  printf (" total local weight=%f", local_weight);
  printf (" signature=%i", unit_vector_signature);
  print_signature (unit_vector_signature);
  putchar ('\n');
#endif /* LOG */

  return 1;
}


/* On sort le kieme meilleur sous-arbre issu de Aij */
static void
output_kth_best_tree (Aij_or_item, k)
     int Aij_or_item, k;
{
  int               Pij, signature, bot, item, Xpq, PijK;
  double            weight;
  struct dedication *dedication_ptr;
  
  if (contextual_proba_kind > EPCFG) {
    /* Aij_or_item est un item */
    print_ctxt (Aij_or_item);
    putchar ('\n');
  }

  dedication_ptr = get_dedication_ptr (Aij_or_item, k);

  if (dedication_ptr) {
    weight = dedication_ptr->total_weight;

    if (weight != 0) {
      PijK = dedication_ptr->PijK;

      if (PijK < 0) {
	Pij = -PijK;
	signature = unit_vector_signature;
      }
      else {
	Pij = XxY_X (PijxItem_hd, PijK);
	signature = dedication_ptr->signature;
      }

      spf_print_iprod (stdout, Pij, "", "");
      printf (" total weight=%f\n", weight);

      bot = XH_X (signatures_hd, signature);
      item = spf.outputG.lhs [Pij].prolon;
    
      while ((Xpq = spf.outputG.rhs [item].lispro) != 0) {
	if (Xpq > 0)
	  output_kth_best_tree ((contextual_proba_kind > EPCFG) ? item : Xpq, XH_list_elem (signatures_hd, bot));

	bot++;
	item++;
      }
    }
  }
}


static int
print_decorated_Aij (Aij_or_item)
     int Aij_or_item;
{
  int               Aij, next, signature, bot, item, Xpq, Pij, PijK;
  struct dedication *dedication_ptr;

  next = Aij_or_item2last_dedication [Aij_or_item];

  while (next) {
    dedication_ptr = dedication_list+next;

    PijK = dedication_ptr->PijK;

    if (PijK < 0) {
      Pij = -PijK;
      signature = unit_vector_signature;
    }
    else {
      Pij = XxY_X (PijxItem_hd, PijK);
      signature = dedication_ptr->signature;
    }

    Aij = spf.outputG.lhs [Pij].lhs;

    printf ("\t%i: ", Pij);
    spf_print_Xpq (stdout, Aij);
    fputs (" = ", stdout);

    bot = XH_X (signatures_hd, signature);
    item = spf.outputG.lhs [Pij].prolon;
    
    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      spf_print_Xpq (stdout, Xpq);

      if (Xpq > 0)
	printf ("[%i] ", XH_list_elem (signatures_hd, bot));

      bot++;
    }

    printf ("; weight = %f\n", dedication_ptr->total_weight);

    next = dedication_ptr->next;
  }
  
  putchar ('\n');

  return 1;
}

/* dummy */
static int
print_decorated_iprod (Pij)
     int Pij;
{
#if LOG
  spf_print_iprod (stdout, Pij, "", "\n");
#endif /* LOG */

  return 1;
}

static void
print_decorated_item (item)
     int item;
{
  int Xpq;

  Xpq = spf.outputG.rhs [item].lispro;

  if (Xpq < 0) {
    fputs (" in no context\t", stdout);
    spf_print_Xpq (stdout, Xpq);
    printf ("weight=%f\n\n", Tpq_item2weight [item]);
  }
  else {
    print_ctxt (item);
    fputs ("\n", stdout);
    print_decorated_Aij (item);
  }
}

static int
print_decorated_ctxt_prod (Pij)
     int Pij;
{
  int item, Xpq;

  item = spf.outputG.lhs [Pij].prolon;
    
  while ((Xpq = spf.outputG.rhs [item].lispro) != 0) {
    print_decorated_item (item);
    item++;
  }

  return 1;
}


void
nbest_print_forest (int Aij_or_item, int cur_contextual_proba_kind, int k)
{
  printf ("\n************** #%i best forest **************\n", k);

  if (cur_contextual_proba_kind > EPCFG) {
    /* Aij_or_item est un item */
    print_decorated_item (Aij_or_item);

    spf_topological_top_down_walk (spf.outputG.rhs [Aij_or_item].lispro, print_decorated_ctxt_prod, NULL, NULL);
  }
  else
    spf_topological_top_down_walk (Aij_or_item, print_decorated_iprod, print_decorated_Aij, NULL);
}


/* On utilise le resultat de nbest pour filtrer la foret */
void
nbest_filter_forest ()
{
  sxba_not (best_Pij_set);
  SXBA_0_bit (best_Pij_set, 0);

  spf_erase (best_Pij_set);
}

static FILE *best_weights_file;

extern int  sentence_id; /* ds lfg_semact */
extern int  easy_get_sentence_no ();

static void
output_best_weights (k, weight)
     int    k;
     double weight;
{
  if (best_weights_file == NULL) {
    best_weights_file = fopen (best_weights_file_name, "a");

    if (best_weights_file == NULL)
      sxtrap (ME, "output_best_weights (open best_weights file)");

    if (sentence_id == 0)
      sentence_id = easy_get_sentence_no ();
  }

  fprintf (best_weights_file, "#%i %i %i %f\n", sentence_id, k, contextual_proba_kind, weight);
}


static int
nbest_sem_pass_with_cpk (int root_Aij, int kind)
{
  int      k, real_nbest, start_symbol_or_item;
  double   threshold, best_forest_weight, prev_weight, weight;
  char     str [128];
  BOOLEAN ret_val;

  sxinitialise(threshold);  /* pour faire taire "gcc -Wuninitialized" */
  /* L'appel a nbest_allocate a du etre fait de l'exterieur */

  contextual_proba_kind = kind;

  best_Aij_local_weight = -DBL_MAX;
  /* fill_Tij2tok_no (); inutile */

#if LOG
  fputs ("\n************** 1st_best_weight computation **************\n", stdout);
#endif /* LOG */

  ret_val = spf_topological_bottom_up_walk (root_Aij, one_best_bu_walk, NULL, (contextual_proba_kind>EPCFG) ? NULL : one_best_post_bu_walk);

#if EBUG
  if (!ret_val)
    sxtrap (ME, "nbest_sem_pass_with_cpk");
#endif /* EBUG */

  if (contextual_proba_kind > EPCFG) {
    /* Cas particulier, on est sur une prod instanciee qui definit l'axiome */
    /* Le contexte est 0 et l'item est 0 (0 -> S1n) */
    best_forest_weight = set_item_weight_list (0);
  }
  else
    best_forest_weight = dedication_list [Aij2last_dedication [root_Aij]].total_weight;

  prev_weight = best_forest_weight;

#if LOG
  printf ("\n\t\t*************** 1st_best_weight=%f ***************\n\n", best_forest_weight);
#endif /* LOG */

  if (best_weights_file_name)
    output_best_weights (1, best_forest_weight);

  start_symbol_or_item = contextual_proba_kind > EPCFG ? 0 : root_Aij;

  if (min_weight_arg != 1 && prev_weight < min_weight_arg)
    /* Il peut y avoir plusieurs arbres de meme poids */
    real_nbest = 1;
  else {
    if (beam != 0)
      threshold = best_forest_weight - log10 ((double)beam);

    for (k = 2; k <= nbest; k++) {
#if LOG
      printf ("\n************** %i%s_best_weight computation **************\n", k, (k == 2) ? "nd" : ((k == 3) ? "rd" : "th"));
#endif /* LOG */

      if (Aij_or_item_kth_best (start_symbol_or_item, k)) {
	weight = dedication_list [Aij_or_item2last_dedication [start_symbol_or_item]].total_weight;

#if LOG
	printf ("\n\t\t*************** %i%s_best_weight=%f *************** \n\n", k, (k == 2) ? "nd" : ((k == 3) ? "rd" : "th"),
		weight);
#endif /* LOG */

	if (best_weights_file_name)
	  output_best_weights (k, weight);

	if (beam && weight < threshold)
	  break;

	if (min_weight_arg != 1 && weight < min_weight_arg)
	  break;

	prev_weight = weight;
      }
      else {
#if LOG
	printf ("\n\t\tThere are only %i_best_weight in this forest\n\n", k-1);
#endif /* LOG */

	break;
      }
    }

    real_nbest = k-1;
  }

  if (is_print_nbest_trees) {
    /* On sort les real_nbest meilleurs arbres */
    for (k = 1; k <= real_nbest; k++) {
      printf ("\n************** %i%s best tree **************\n", k, (k == 1) ? "st": ((k == 2) ? "nd" : ((k == 3) ? "rd" : "th")));
      output_kth_best_tree (start_symbol_or_item, k);
    }
  }

  if (is_filtering) {
    nbest_filter_forest ();

#if LOG
    spf_print_signature (stdout, "Output nbest (after filtering): ");
#endif /* LOG */
  }

#if LOG

  nbest_print_forest (start_symbol_or_item, contextual_proba_kind, real_nbest);
#else
    if (is_print_nbest_forest)
      nbest_print_forest (start_symbol_or_item, contextual_proba_kind, real_nbest);
#endif /* LOG */

  if (is_output_nbest_value) {
    printf ("best_weight = %f, exit at step %i with weight = %f\n", best_forest_weight, real_nbest, prev_weight);
  }

  if (is_print_time) {
#if LOG
    struct dedication *dedication_ptr;
#endif

    sprintf (str, "\t%i best, -cp=%i, best_weight = %f (TRUE)", real_nbest, contextual_proba_kind, best_forest_weight);
    sxtime (ACTION, str);

#if LOG
    fputs ("\t\t{", sxtty);

    for (k = 1; k <= 5; k++) {
      if (k > real_nbest)
	break;
  
      dedication_ptr = get_dedication_ptr (start_symbol_or_item, k);

      if (k > 1)
	fputs (", ", sxtty);

      fprintf (sxtty, "%f", dedication_ptr->total_weight);
    }

    if (k > real_nbest)
      fputs ("}\n", sxtty);
    else
      fputs (", ...}\n", sxtty);
#endif /* LOG */
  }

  
#if 0
  /* fait a l'exterieur */
  nbest_free (contextual_proba_kind);
  free_lex_proba ();
#endif /* 0 */

  return real_nbest;
}


static int
nbest_sem_pass ()
{
  int    kind;
  char   str [32];

  if (logprobs_prod_nb != spf.inputG.maxprod || logprobs_nt_nb != spf.inputG.maxnt) {
    fputs ("\nThe \"logprob\" values are out of date with respect to the current CFG specification.\n", sxstderr);
    abort ();
  }

  if (nbest) {
    best_Aij_local_weight = -DBL_MAX;

#if LOG
    spf_print_signature (stdout, "Input nbest: ");
#endif /* LOG */

    if (cpk < 0) {
      for (kind = PCFG; kind <= EP_POS_EPCFG; kind++) {
	nbest_allocate (nbest, kind);
	nbest_sem_pass_with_cpk (spf.outputG.start_symbol, kind);
	nbest_free ();
      }
    }
    else {
      nbest_allocate (nbest, cpk);
      nbest_sem_pass_with_cpk (spf.outputG.start_symbol, cpk);
      nbest_free ();
    }

    if (best_weights_file)
      fclose (best_weights_file), best_weights_file = NULL;
  }
  else {
    /* nbest == 0 => pas de nbest */
    if (is_print_time) {
      sprintf (str, "\t%i best (SKIPPED)", nbest);
      sxtime (ACTION, str);
    }
  }

  return 1;
}


/* Peut etre appelee depuis de la semantique */
/* nbest_allocate (cur_contextual_proba_kind) et nbest_free () sont a la charge de l'utilisateur */
int
nbest_perform (int root_Aij, int h /* k */, int b /* beam */ , int cur_contextual_proba_kind, BOOLEAN do_filtering)
{
  int     old_nbest, old_beam, old_contextual_proba_kind, real_nbest;
  BOOLEAN old_is_filtering;

  old_nbest = nbest;
  old_beam = beam;
  old_contextual_proba_kind = contextual_proba_kind ;
  old_is_filtering = is_filtering;

  nbest = h;
  beam = b;
  is_filtering = do_filtering;

  real_nbest = nbest_sem_pass_with_cpk (root_Aij, cur_contextual_proba_kind);

  nbest = old_nbest;
  beam = old_beam;
  contextual_proba_kind = old_contextual_proba_kind;
  is_filtering = old_is_filtering;

  return real_nbest;
}

/* permet de recuperer le h ieme meilleur poids de la sous-foret de racine root_Aij qui vient d'etre calcule' ds
   le context cur_contextual_proba_kind */
double
nbest_get_weight (int root_Aij, int h , int cur_contextual_proba_kind)
{
  return get_dedication_ptr (cur_contextual_proba_kind > EPCFG ? 0 : root_Aij, h)->total_weight;
}

/* permet de recuperer le h ieme meilleur Pij de la sous-foret de racine root_Aij qui vient d'etre calcule' ds
   le context cur_contextual_proba_kind */
int
nbest_get_Pij (int root_Aij, int h , int cur_contextual_proba_kind)
{
  int                Pij;
  struct dedication  *dedication_ptr;

  dedication_ptr = get_dedication_ptr (cur_contextual_proba_kind > EPCFG ? 0 : root_Aij, h);

  if (cur_contextual_proba_kind > EPCFG)
    Pij = XxY_X (PijxItem_hd, dedication_ptr->PijK);
  else
    Pij = -dedication_ptr->PijK;

  return Pij;
}
