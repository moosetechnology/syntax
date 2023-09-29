/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2003 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */

/* Actions semantiques construisant la f_structure d'une analyse LFG */
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
/* 17-10-03 10:45 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

static char	ME [] = "lfg_semact";

/* Les valeurs atomiques sont qcq et on peut faire de la disjonction sur les "..." */
#define ESSAI


/* Evaluation d'une f_structure sur un arbre */
/* On suppose que les f_structures de toutes les feuilles lexicales ont ete remplies,
   Elles seront partagees entre toutes les analyses */
/* Soit forest_node_nb le nombre de noeuds de la foret partagee et
   forest_prod_nb son nb de prod instanciees */
/* L'evaluation va se faire en plusieurs passes */

#include "sxunix.h"
#include "earley.h"
#include "XH.h"
#if FS_PROBAS
#include "nbest.h"
#endif /* FS_PROBAS */

#include "dag_scanner.h"

char WHAT_LFG_SEMACT[] = "@(#)SYNTAX - $Id: lfg_semact.c 3488 2023-08-20 13:57:36Z garavel $" WHAT_DEBUG;

#ifdef lfg_h

#define def_code_stack
#define def_complete_id_set

#define def_dfield_pairs
#define def_dfield_pairs2field_id

#define def_datom_pairs
#define def_datom_pairs2atom_id

#define def_equation_stack

#if IS_OPERATOR_LEX_REF
#define def_lexeme_id2display
#define def_args_ref_list
#endif /* IS_OPERATOR_LEX_REF */

#if IS_OPERATOR_LEXEME_REF
#define def_atom_names
#endif /* IS_OPERATOR_LEXEME_REF */

#ifdef ESSAI
#define def_operator_atom2atom_list
#define def_atom_list
#endif /* ESSAI */

#define def_priority_hd
#define def_priority_list
#define def_prod2rule

#define def_ranks
#define def_rule2equation
#define def_rule2next_rule

#define def_suffix_id2string

#include lfg_h
#endif /* lfg_h */


#ifdef lex_lfg_h

#define def_argument_set
#define def_atom_id2string
#define def_attr_vals

#define def_compound_field
#define def_compound_field_list

#ifndef ESSAI
#define def_field_id2atom_field_id
#endif /* ESSAI */

#define def_field_id2kind
#define def_field_id2pass_nb
#define def_field_id2string

#define def_if_id2t_list
#define def_if_id2tok_list_id
#define def_inflected_form2display

#define def_lex_adjunct_hd
#define def_lex_adjunct_list
#define def_lex_atom_hd
#define def_lex_atom_list
#define def_lexeme_id2string

#define def_pred_vals

#define def_sous_cat
#define def_sous_cat_list
#define def_structure_args_list

#define def_tok_disp
#define def_tok_list

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


#undef SX_GLOBAL_VAR
#undef SX_INIT_VAL
# define SX_GLOBAL_VAR	/*rien*/
#include "lfg_semact.h"
#undef SX_GLOBAL_VAR
#undef SX_INIT_VAL
# define SX_GLOBAL_VAR	extern


#if IS_OPERATOR_LEXEME_REF
#define ATOM_ID2STRING(id) SXWORD_get (atom_names, id)
#else /* IS_OPERATOR_LEXEME_REF == 0 */
#define ATOM_ID2STRING(id) atom_id2string [id] 
#endif /* IS_OPERATOR_LEXEME_REF == 0 */


static SXBOOLEAN is_unfold_parse_forest, is_print_final_c_structure, is_print_final_cf_structure, is_sum_priority;
static char    *priority_vector;
static char    priority_vector_default [] = "1";
static int     priority_vector_lgth;
static char    *input_dag_string;
int            sentence_id; /* nbest peut en avoir besoin */


#ifdef EASY
/* Les codes des terminaux coo_code, sbound_code ponctw_code, poncts_code et np_code sont connus */
/* #include "easy_td.h" (BS) est donné dans les options de cc */
#include td_h


static int fs_id2tete ();
#endif /* EASY */

#define Tpq2str(Tpq)  (Tpq ? (spf.inputG.tstring [-Tij2T (Tpq-spf.outputG.maxxnt)]) : NULL)


/* Principe : quand un $$ est calcule', il ne changera plus.  Toutes les [sous-]structures des f-structures
   calculees peuvent donc e^tre partage'es.
   En fait chaque sous_structure va avoir 2 representations, une sous forme de XH et une autre sous forme de XxY.
     - La forme XH de nom XH_fs_hd est une liste ordonnee d'elements (val, kind, field) ou field est l'id du champ
     et val sa valeur.  Si field est champ atomique val est la valeur de l'atome, si field est un champ structure
     val est l'identifiant ds XH_fs_hd de cette sous_structure.
     - La forme XxY de nom XxY_fs_hd pour la sous_structure d'identifiant xfs ds XH_fs_hd est un ensemble de
     couples (xfs, field) et en XxY_fs_hd2val est stocke la valeur correspondante.
   Pour chaque symbole instancie' (traite') Xpq on a stocke ds le XxY heads tous les couples (Xpq, xfs) ou les
   xfs sont les identifiants (ds XH_fs_hd) de toutes les differentes f_structures associees a Xpq.
   Chaque evaluation d'un ensemble d'equations (sur les xfs des fils) se fait ds une structure de travail wfs.
   Si l'evaluation a reussie, ce wfs est transfere ds XH_fs_hd et recoit donc un identifiant disons fs_id.  
   S'il est nouveau, On fabrique alors le XxY_fs_hd correspondant. Puis on stocke le couple (Xpq, fs_id) ds heads.
 */
#define X7F             ((~0ul)>>1)
#define X80             (~X7F)
#define X40             (X80>>1)
#define X20             (X80>>2)
#define X10             (X80>>3)
#define X3F             ((~(0ul))>>2)
#define XC0             (~(X3F))
#define X0F             ((~(0ul))>>4) 
#define XF0             (~(X0F))

#define CONSTRAINT_VAL  (ASSIGN_CONSTRAINT_KIND<<MAX_FIELD_ID_SHIFT)
#define OPTIONAL_VAL    (ASSIGN_OPTION_KIND<<MAX_FIELD_ID_SHIFT)

#define FS_ID_SHIFT     (STRUCTURE_NAME_shift+MAX_FIELD_ID_SHIFT)

#if ATOM_Aij
static int              AIJ;
#ifdef ESSAI
static int              UFS_ID_Aij;
#endif /* ESSAI */
#endif /* ATOM_Aij */
#if ATOM_Pij
static int              PIJ;
#ifdef ESSAI
static int              UFS_ID_Pij;
#endif /* ESSAI */
#endif /* ATOM_Pij */

#define FIELD_AND       (~((~0)<<MAX_FIELD_ID_SHIFT))
#define KIND_AND        (~((~0)<<STRUCTURE_NAME_shift))
#define FS_ID_AND        (~((~0)<<FS_ID_SHIFT))

#include "sxstack.h"
#include "SS.h"

/* Pour eval () */
#define ATOM_VAL                  1
#define OLD_FIELD                 2
#define NEW_FIELD                 4
#define DOL_REF                   8
#define ATOM_FIELD               16
#define STRUCT_FIELD             32
#define UNBOUNDED_STRUCT_FIELD   64
#define EMPTY_STRUCT            128
#define REF_SHIFT                 8
/* $i est code par i<<REF_SHIFT */

/* Le 07/01/05 l'identifiant de la f_structure principale ($$) passe de 0 a MAIN_WFS_ID qui doit etre
   non nul.  La valeur 0 etant reservee a l'absence !! de structure */
#define MAIN_WFS_ID      1

static int               cur_pass_number;
static int               single_fs_id, valid_eq_nb, valid_single_fs_id;

#define is_executable(i) (field_id2pass_nb [i] <= cur_pass_number)

static int               maxxt; /* nb des terminaux instancies Tij, y compris ceux couverts par les nt de la rcvr == -spf.outputG.maxt */
static SXUINT tree_id;
static int               static_heads_top;
static X_header          fs_id_coherence_hd; /* Pour de la memoization */
static char              *fs_id_coherence2attr; /* contient 2 bits */

static X_header          shared_hd;

static int               rhs_pos2fs [MAXRHS+1];
static int               rhs_pos2digit [MAXRHS+1];

static int               *fs_id_ssstack, *ufs_id_ssstack;
static int               *eoaiu_stack;

/* Pour un Xpq donne', soit ptr = fs_id_dstack+Xpq2disp[Xpq], on a :
   - *ptr == nb de fs differentes associees a Xpq
   - pour tout i, 1 <= i <= *ptr, ptr [i] est un fs_id */
static int               *Xpq2disp, *fs_id_dstack, *ni_wfs_id_stack;
static SXBA              Xpq_set; /* Cas unfold pour un clear apres chaque arbre */
static SXBA              to_be_erased_Pij_set, erased_by_ranking_Pij_set, ranking_by_nbest_root_Pij_set;
static struct walk       nbest_save_cur_walk, nbest_save_nbest_walk;
static SXBA              consistent_Xpq_set, consistent_Pij_set;
static int               consistent_cur_pass_number;

/* Pour traiter le $$ courant */
static int               wfs_id, wfs_page_nb, left_father_wfs_id;
static int               *wfs_id2equiv;

/* Le 11/01/05 cette structure est separee de wfs_id_pred_list */
/* Le 17/03/05 ajout du champ kind, dol et field_id_path */
static struct adjunct_set_item {
  int id /* [w|u]fs_id ou 0 (vide) */,
    kind /* ufs_id, fs_id, wfs_id */,
    next;
} *adjunct_sets, empty_adjunct_set;

static struct field_val2adjunct_id {
  int     equiv, adjunct_set_id, ufs_id, pred_nb;
} *field_val2adjunct_id, empty_field_val2adjunct_id; /* [0].equiv == top; [0].adjunct_set_id == size */

#define ADJUNCT_KIND_EMPTY 0
#define ADJUNCT_KIND_UFS   1
#define ADJUNCT_KIND_FS    2
#define ADJUNCT_KIND_WFS   3
#define ADJUNCT_KIND_ATOM  4
#if ATOM_Aij || ATOM_Pij
#define ADJUNCT_KIND_Aij   5
#endif /* ATOM_Aij || ATOM_Pij */

struct path_instance {
  int wfs_id, fs_id_dol/*, kind */;
};

static XxY_header           field_paths, XxY_dol_fs_id;
static struct path_instance *path2wfs_id;
static int                  *dol_fs_id2wfs_id;
static XxY_header           XxY_memo_dol_fs_id;
static SXBA                 memo_dol_fs_id_set, cyclic_dol_fs_id_set;
static int                  *wfs_id2localdynweight;
static int                  *fs_id2localdynweight;


static SXBA              **wfs_field_set;
static int               ***wfs2attr; /* (val<<STRUCTURE_NAME_shift)+kind */

/* utilise ds fill_fs ().  Ca permet de voir si une wfs_id est close */
static int               wfs_id2sons_set_size;
static SXBA              wfs_id_set;
static SXBA              visited_fs_id_set /* utilise'ds le ranking 1 et ds les cycles des f_structures */;
static int               *visited_fs_id_stack;
static SXBA              *wfs_id2sons_set, *wfs_id2fathers_set, *wfs_id2pred_set;
static int               *wfs_id2son_nb, *wfs_id2pred_nb, *wfs_id_stack, *wfs_id_stack2;
static int               *simulated_wfs_id_stack;
static SXBA              *cyclic_M;

static int               field_val2wfs_id_set_size;
static SXBA              *field_val2wfs_id_set;

static SXBA_CST_ALLOC (doli_used_set, MAXRHS+1);
/* Fin pour traiter le $$ courant */

static int     *post_stack, *post_display, *post_eval_stack, *post_hook_stack; /* DSTACKs */

#define POST_STACK_fs_id_x -1
#define POST_STACK_x_y     -2


static SXBA_CST_ALLOC (warg_set, MAX_FIELD_ID+1);
static SXBA_CST_ALLOC (pred_arg_set, MAX_FIELD_ID+1);

static int               *pcs; /* pointeur courant ds code_stack */

static SXBA              failure_set;


static struct re_result {
  int     *ptr, kind, next, father_wfs_id, path_id, prev_result;
  SXBOOLEAN is_marked;
}                        *re_results, empty_re_result;

#ifdef ESSAI
static int      *operator_atom2ufs_id;
/* atom_id2local_atom_id_hd est global pour output_semact !! */
static SXBA     right_local_atom_id_set; /* left_local_atom_id_set est global pour output_semact !! */

static atom_id2local_atom_id_hd_oflw (int old_size, int new_size)
{
  left_local_atom_id_set = sxba_resize (left_local_atom_id_set, new_size+1);
  right_local_atom_id_set = sxba_resize (right_local_atom_id_set, new_size+1);
}
#endif /* ESSAI */

/* Petit gestionnaire qui alloue dynamiquement des entiers.
   On est su^r que les pointeurs retourne's resteront valides ... */
static struct int_tank_struct {
  int top, hd_size, bag_size;
  int **disp;
} int_tank_hd;

static void
int_tank_alloc (hd_size, bag_size)
     int hd_size, bag_size;
{
  int_tank_hd.top = 0;
  int_tank_hd.hd_size = hd_size;
  int_tank_hd.bag_size = bag_size;
  int_tank_hd.disp = (int**) sxcalloc (hd_size, sizeof (int));
  
  RAZ (int_tank_hd.disp [0] = (int*) sxalloc (bag_size+1, sizeof (int)));
}

static void
int_tank_free ()
{
  int i;

  for (i = int_tank_hd.top; i >= 0; i--)
    sxfree (int_tank_hd.disp [i]);

  sxfree (int_tank_hd.disp), int_tank_hd.disp = NULL;
}

static void
int_tank_clear ()
{
  RAZ (int_tank_hd.disp [int_tank_hd.top = 0]);
}

static int *
int_tank_get ()
{
  int *stack;

  stack = int_tank_hd.disp [int_tank_hd.top];

  if (TOP (stack) == int_tank_hd.bag_size) {
    if (++int_tank_hd.top == int_tank_hd.hd_size) {
      int_tank_hd.disp = (int**) sxrecalloc (int_tank_hd.disp, int_tank_hd.hd_size, 2*int_tank_hd.hd_size, sizeof (int));
      int_tank_hd.hd_size *= 2;
    }

    stack = int_tank_hd.disp [int_tank_hd.top];

    if (stack == NULL) {
      stack = int_tank_hd.disp [int_tank_hd.top] = (int*) sxalloc (int_tank_hd.bag_size+1, sizeof (int));
    }
      
    RAZ (stack);
  }

  return &(stack [++TOP (stack)]);
}

/* Version du 13/04/05 */
static int
field_id_path_catenate (field_id, prev_path, fs_id, dol)
     int field_id, prev_path, fs_id, dol;
{
  int                         new_path;
  static struct path_instance *path_ptr;
  
#if EBUG
  /* Ca peut etre un ufs_id */
  if (fs_id <= 0 || field_id > 0 &&
      (field_id2kind [field_id] == STRUCT_KIND && fs_id >= XH_top (XH_fs_hd) ||
      field_id2kind [field_id] != STRUCT_KIND && fs_id >= XH_top (XH_ufs_hd)))
    sxtrap (ME, "field_id_path_catenate");
#endif /* EBUG */

  if (!XxY_set (&field_paths, field_id, prev_path, &new_path)) {
    /* Nouveau */
    path_ptr = path2wfs_id+new_path;
    path_ptr->wfs_id = 0;
    path_ptr->fs_id_dol = (fs_id<<MAXRHS_SHIFT)+dol;
  }
#if EBUG
  else {
    if (path2wfs_id [new_path].fs_id_dol != (fs_id<<MAXRHS_SHIFT)+dol)
      sxtrap (ME, "field_id_path_catenate");
  }
#endif /* EBUG */

  return new_path;
}

/* On alloue par blocs de 128!! */
#define AREA_SHIFT    7
#define AREA_AND      (~((~0)<<AREA_SHIFT))
#define AREA_SIZE     (1<<AREA_SHIFT) 
#define AREA_PAGE(n)  ((n)>>AREA_SHIFT)
#define AREA_DELTA(n) ((n)&AREA_AND)

#include "varstr.h"
static VARSTR vstr;

#if EBUG
static VARSTR left_vstr, right_vstr;
#endif /* EBUG */
static int          prio_hd_assoc_list [6] = {0, 0, 0, 0, 0, 0}; /* X, Y, Z, XY, XZ, YZ */
static XxYxZ_header prio_hd;
static XxY_header dl_hd;
static int        *dl_id2fs_id;
static SXBA_ELT   index_set [NBLONGS (MAX_SHARED_F_STRUCTURE_ID+1)+1] = {MAX_SHARED_F_STRUCTURE_ID+1};
static SXBA_ELT   cyclic_index_set [NBLONGS (MAX_SHARED_F_STRUCTURE_ID+1)+1] = {MAX_SHARED_F_STRUCTURE_ID+1};
static int        cyclic_base_fs_id;
static int        index2struct_id [MAX_SHARED_F_STRUCTURE_ID+1];
static int        index2cyclic_fs_id [MAX_SHARED_F_STRUCTURE_ID+1];

static int        *working_heads_stack, *working_value_stack; /* des DSTACKs */
#if FS_PROBAS
static double     *working_double_stack;
#endif /* FS_PROBAS */
static int        *maximal_tree_Aij_span, *maximal_tree_Pij_span;
static SXBA       son_of_maximal_Xpq_set;

/* Pour la passe "is_forest_filtering" */
static int     doldol;
static SXBOOLEAN is_locally_unconsistent;


/* Pour tree_filter_td_walk */
static SXBA    valid_heads_set;

#include "sxword.h"

static int           Ei_lgth;

/* Pour (mieux) ge'rer les structures cycliques en evitant les occurrences multiples de structures identiques */
static XxY_header cyclic_signXfs_id_hd, cyclic_sizeXgrp_id_hd;
static int        *cyclic_wfs_id2fs_id, *cyclic_grp_stack /* DSTACK */, *cyclic_adjunct_stack /* DSTACK */;
static SXBOOLEAN    is_new_Pij_for_cyclic_fs_ids;
static SXBA       Aij_or_Pij_set;

#define include_qq_valeurs 1
#ifdef lfg_disamb_h
#include lfg_disamb_h
#endif /* lfg_disamb_h */
#undef include_qq_valeurs

static void
dl_hd_oflw (old_size, new_size)
     int old_size, new_size;
{
  dl_id2fs_id = (int*) sxrealloc (dl_id2fs_id, new_size+1, sizeof (int));
}

static void
wfs_area_finalize ()
{
  int             x, *q, **p;

  wfs_field_set [wfs_page_nb-1] = sxbm_calloc (AREA_SIZE, MAX_FIELD_ID+1);
  p = wfs2attr [wfs_page_nb-1] = (int**) sxalloc (AREA_SIZE, sizeof (int*));
  q = p [0] = (int*) sxalloc (AREA_SIZE*(MAX_FIELD_ID+1), sizeof (int));

  for (x = 1; x < AREA_SIZE; x++)
    p [x] = (q += MAX_FIELD_ID+1);
}

static void
wfs_area_allocate ()
{
  int struct_id;

  wfs_page_nb = 1;
  wfs_field_set = (SXBA **) sxalloc (wfs_page_nb, sizeof (SXBA*));
  wfs2attr = (int***) sxalloc (wfs_page_nb, sizeof (int**));

  wfs_id2sons_set_size = 1<<SXSHIFT_AMOUNT; /* taille raisonnable */
  wfs_id2sons_set = sxbm_calloc (wfs_id2sons_set_size, wfs_id2sons_set_size);
  wfs_id2fathers_set = sxbm_calloc (wfs_id2sons_set_size, wfs_id2sons_set_size);
  wfs_id2pred_set = sxbm_calloc (wfs_id2sons_set_size, wfs_id2sons_set_size);
  wfs_id2son_nb = (int*) sxalloc (wfs_id2sons_set_size, sizeof (int)), wfs_id2son_nb [0] = wfs_id2son_nb [MAIN_WFS_ID] = 0;
  wfs_id2pred_nb = (int*) sxalloc (wfs_id2sons_set_size, sizeof (int)), wfs_id2pred_nb [0] = wfs_id2pred_nb [MAIN_WFS_ID] = 0;
  wfs_id_stack = (int*) sxalloc (wfs_id2sons_set_size+1, sizeof (int)), wfs_id_stack [0] = 0;
  wfs_id_stack2 = (int*) sxalloc (wfs_id2sons_set_size+1, sizeof (int)), wfs_id_stack2 [0] = 0;
  simulated_wfs_id_stack = (int*) sxalloc (wfs_id2sons_set_size+1, sizeof (int)), simulated_wfs_id_stack [0] = 0;
  wfs_id_set = sxba_calloc (wfs_id2sons_set_size);

  field_val2wfs_id_set_size = 1<<SXSHIFT_AMOUNT; /* taille raisonnable */
  field_val2wfs_id_set = sxbm_calloc (field_val2wfs_id_set_size, wfs_id2sons_set_size);

  wfs_id2equiv = (int*) sxalloc (AREA_SIZE, sizeof (int)), wfs_id2equiv [0] = 0;
  wfs_id2localdynweight = (int*) sxcalloc (AREA_SIZE, sizeof (int)), wfs_id2localdynweight [0] = 0;

  adjunct_sets = (struct adjunct_set_item*) sxalloc (AREA_SIZE+1, sizeof (struct adjunct_set_item));
  adjunct_sets [0].id = 0; /* top */
  adjunct_sets [0].next = AREA_SIZE; /* size */
    
  field_val2adjunct_id = (struct field_val2adjunct_id*) sxalloc (AREA_SIZE+1, sizeof (struct field_val2adjunct_id));
  field_val2adjunct_id [0].equiv = 0; /* top */
  field_val2adjunct_id [0].adjunct_set_id = AREA_SIZE; /* size */
  field_val2adjunct_id [0].pred_nb = 0;
  field_val2adjunct_id [0].ufs_id = 0; /* pourquoi pas */

  wfs_area_finalize ();

#if EBUG
  left_vstr = varstr_alloc (32);
  right_vstr = varstr_alloc (32);
#endif /* EBUG */

  vstr = varstr_alloc (32);
}

static void
wfs_area_oflw ()
{
  int l;

  wfs_page_nb++;
  wfs_field_set = (SXBA **) sxrealloc (wfs_field_set, wfs_page_nb, sizeof (SXBA*));
  wfs2attr = (int***) sxrealloc (wfs2attr, wfs_page_nb, sizeof (int**));

  l = AREA_SIZE*wfs_page_nb;

  wfs_id2equiv = (int*) sxrealloc (wfs_id2equiv, l, sizeof (int));
  wfs_id2localdynweight = (int*) sxrecalloc (wfs_id2localdynweight, l-AREA_SIZE, l, sizeof (int));

  if (cyclic_wfs_id2fs_id)
    cyclic_wfs_id2fs_id = (int*) sxrealloc (cyclic_wfs_id2fs_id, l, sizeof (int));

  wfs_area_finalize ();
}

static void free_store_wfs ();

static void
wfs_area_free ()
{
  int x, struct_id, **p;
  /* BS static*/  void free_store_wfs ();

  for (x = wfs_page_nb-1; x >= 0; x--) {
    sxbm_free (wfs_field_set [x]);
    p =  wfs2attr [x];
    sxfree (*p);
    sxfree (p);
  }

  sxbm_free (wfs_id2sons_set), wfs_id2sons_set = NULL;
  sxbm_free (wfs_id2fathers_set), wfs_id2fathers_set = NULL;
  sxbm_free (wfs_id2pred_set), wfs_id2pred_set = NULL;

  sxbm_free (field_val2wfs_id_set), field_val2wfs_id_set = NULL;

  sxfree (wfs_field_set), wfs_field_set = NULL;
  sxfree (wfs2attr), wfs2attr = NULL;

  sxfree (wfs_id2son_nb), wfs_id2son_nb = NULL;
  sxfree (wfs_id2pred_nb), wfs_id2pred_nb = NULL;
  sxfree (wfs_id_stack), wfs_id_stack = NULL;
  sxfree (wfs_id_stack2), wfs_id_stack2 = NULL;
  sxfree (wfs_id_set), wfs_id_set = NULL;
  sxfree (simulated_wfs_id_stack), simulated_wfs_id_stack = NULL;

  if (cyclic_M) {
    sxbm_free (cyclic_M), cyclic_M = NULL;
  }

  sxfree (wfs_id2equiv), wfs_id2equiv = NULL;
  sxfree (wfs_id2localdynweight), wfs_id2localdynweight = NULL;
  sxfree (adjunct_sets), adjunct_sets = NULL;
  sxfree (field_val2adjunct_id), field_val2adjunct_id = NULL;

#if EBUG
  varstr_free (left_vstr), left_vstr = NULL;
  varstr_free (right_vstr), right_vstr = NULL;
#endif /* EBUG */

  varstr_free (vstr), vstr = NULL;

  free_store_wfs ();
}

static int
get_a_new_wfs_id ()
{
  int page_nb, delta_nb;

  wfs_id++;
  page_nb = AREA_PAGE (wfs_id);
		
  if (page_nb >= wfs_page_nb)
    wfs_area_oflw ();

  delta_nb = AREA_DELTA (wfs_id);

  wfs2attr [page_nb] [delta_nb] [0] = -1; /* vide */
  sxba_empty (wfs_field_set [page_nb] [delta_nb]); /* Le 01/06/05 => suppression des clear_wfs () et clear_wfs_id () */

  wfs_id2equiv [wfs_id] = wfs_id;

  return wfs_id;
}


#define has_multiple_in_wfs_id(id) (field_val2adjunct_id [id].pred_nb > 1)
static int get_equiv ();
#define get_top_adjunct_sets()  (adjunct_sets [0].id)

static int
get_new_top_adjunct_sets ()
{
  int top;

  if ((top = ++adjunct_sets [0].id) > adjunct_sets [0].next)
    adjunct_sets = (struct adjunct_set_item*) sxrealloc (adjunct_sets,
							 (adjunct_sets [0].next *= 2)+1,
							 sizeof (struct adjunct_set_item));

  /* adjunct_sets [top] = empty_adjunct_set A VOIR */
  return top;
}


static int
get_equiv (id)
     int id;
{
  int main_id;

  while (id >= 0 && (main_id = wfs_id2equiv [id]) != id) {
#if EBUG
    if (main_id <= 0 || main_id > wfs_id)
      sxtrap (ME, "get_equiv");
#endif /* EBUG */
    id = main_id;
  }

  return id;
}

static int
get_adjunct_equiv (x)
     int x;
{
  int main_x;

  if (x <= 0)
    return 0;

  while ((main_x = field_val2adjunct_id [x].equiv) != x) {
#if EBUG
    if (main_x <= 0 || main_x > field_val2adjunct_id [0].equiv)
      sxtrap (ME, "get_adjunct_equiv");
#endif /* EBUG */
    x = main_x;
  }

  return x;
}



#define get_top_field_val2adjunct_id()  (field_val2adjunct_id [0].equiv)

static int
get_new_top_field_val2adjunct_id ()
{
  int                         top;
  struct field_val2adjunct_id *ptr;

  if ((top = ++field_val2adjunct_id [0].equiv) > field_val2adjunct_id [0].adjunct_set_id) {
    field_val2adjunct_id = (struct field_val2adjunct_id*) sxrealloc (field_val2adjunct_id,
						       (field_val2adjunct_id [0].adjunct_set_id *= 2)+1,
						       sizeof (struct field_val2adjunct_id));
  }

  ptr = field_val2adjunct_id + top;
  *ptr = empty_field_val2adjunct_id;
  ptr->equiv = top;

  return top;
}


static int
get_a_new_empty_atom_id ()
{
  int                     top, adjunct_set_id;
  struct adjunct_set_item *adjunct_ptr;

  top = get_new_top_field_val2adjunct_id ();
  field_val2adjunct_id [top].adjunct_set_id = adjunct_set_id = get_new_top_adjunct_sets ();
  adjunct_ptr = adjunct_sets+adjunct_set_id;

  adjunct_ptr->next = 0;
  adjunct_ptr->id = 0; /* Ensemble vide */
  adjunct_ptr->kind = ADJUNCT_KIND_EMPTY;

  return top;
}


static void
XH_fs_hd_oflw (old_size, new_size)
     int old_size, new_size;
{
  fs_id_set = sxba_resize (fs_id_set, new_size+1);
  visited_fs_id_set = sxba_resize (visited_fs_id_set, new_size+1);
  fs_id_stack = (int*) sxrealloc (fs_id_stack, new_size+1, sizeof (int));
  visited_fs_id_stack = (int*) sxrealloc (visited_fs_id_stack, new_size+1, sizeof (int));
  fs_id2dynweight = (int*) sxrealloc (fs_id2dynweight, new_size+1, sizeof (int));
  fs_id2localdynweight = (int*) sxrealloc (fs_id2localdynweight, new_size+1, sizeof (int));
}

fs_id_coherence_hd_oflw (old_size, new_size)
     int old_size, new_size;
{
  fs_id_coherence2attr = (char*) sxrealloc (fs_id_coherence2attr, new_size+1, sizeof (char));
}


static void
field_paths_oflw (old_size, new_size)
     int old_size, new_size;
{
  path2wfs_id = (struct path_instance*) sxrealloc (path2wfs_id,
						   new_size+1,
						   sizeof (struct path_instance));
}

static void
XxY_dol_fs_id_oflw (old_size, new_size)
     int old_size, new_size;
{
  dol_fs_id2wfs_id = (int*) sxrealloc (dol_fs_id2wfs_id, new_size+1, sizeof (int));
}


static void
XxY_memo_dol_fs_id_oflw (old_size, new_size)
     int old_size, new_size;
{
  memo_dol_fs_id_set = sxba_resize (memo_dol_fs_id_set, new_size+1);
  cyclic_dol_fs_id_set = sxba_resize (cyclic_dol_fs_id_set, new_size+1);
}


static int
re_results_get_new_top ()
{
  int top;
  

  if ((top = ++re_results [0].kind) > re_results [0].next)
    re_results = (struct re_result*) sxrealloc (re_results, (re_results [0].next *= 2)+1, sizeof (struct re_result));

  re_results [top] = empty_re_result;
  
  return top;
}

static void
allocate_all ()
{
  int i, size;

  to_be_erased_Pij_set = sxba_calloc (spf.outputG.maxxprod+1);
  erased_by_ranking_Pij_set = sxba_calloc (spf.outputG.maxxprod+1);

  XH_alloc (&dynam_pred_vals, "dynam_pred_vals", 2*n+1, 1, 4, NULL, NULL);

  Xpq2disp = (int*) sxcalloc (Xpq_top+1, sizeof (int));
  Xpq_set = sxba_calloc (Xpq_top+1);
  DALLOC_STACK (fs_id_dstack, 3*Xpq_top);
  DALLOC_STACK (ni_wfs_id_stack, 100);

  fs_id_ssstack = SS_alloc (100);
  ufs_id_ssstack = SS_alloc (100);

  DALLOC_STACK (eoaiu_stack, 100);

  XxY_alloc (&heads, "heads", 2*Xpq_top+1, 1, 0 /* pas de foreach */, 0, NULL /* heads_oflw */, NULL);

  X_alloc (&shared_hd, "shared_hd", 20, 1, NULL, NULL);

  /* Liste (ordonnee) de couples (field, val) */
  /* Le 30/03/05 fs_id_set et fs_id_stack doivent pouvoir contenir les Pij ou Aij */
  size = spf.outputG.maxxnt+spf.outputG.maxxprod;
  XH_alloc (&XH_fs_hd, "XH_fs_hd", size+1, 1, 2*5, XH_fs_hd_oflw, NULL); /* A VOIR */
  XH_alloc (&XH_cyclic_fs_hd, "XH_cyclic_fs_hd", 16+1, 1, 2*5, NULL, NULL);
  XH_alloc (&XH_cyclic_ufs_hd, "XH_cyclic_ufs_hd", 16+1, 1, 2, NULL, NULL);
  fs_id_set = sxba_calloc (XH_size (XH_fs_hd)+1);
  visited_fs_id_set = sxba_calloc (XH_size (XH_fs_hd)+1);
  fs_id_stack = (int*) sxalloc (XH_size (XH_fs_hd)+1, sizeof (int)), fs_id_stack [0] = 0;
  visited_fs_id_stack = (int*) sxalloc (XH_size (XH_fs_hd)+1, sizeof (int)), visited_fs_id_stack [0] = 0;
  fs_id2dynweight = (int*) sxalloc (XH_size (XH_fs_hd)+1, sizeof (int)), fs_id2dynweight[0] = 0;
  fs_id2localdynweight = (int*) sxalloc (XH_size (XH_fs_hd)+1, sizeof (int)), fs_id2localdynweight[0] = 0;

  X_alloc (&fs_id_coherence_hd, "fs_id_coherence_hd", size+1, 1, fs_id_coherence_hd_oflw, NULL);
  fs_id_coherence2attr = (char*) sxalloc (X_size (fs_id_coherence_hd) + 1, sizeof (char));

#ifdef ESSAI
  /* L'operande d'un OPERATOR_ATOM est un index ds operator_atom2atom_list qui reference une liste
     d'atom_id ds atom_list. A la 1ere utilisation, cette liste doit etre convertie en une liste locale
     apres passage ds le X_header atom_id2local_atom_id_hd et rangee ds XH_ufs_hd. operator_atom2ufs_id
     memoize ce resultat */
  operator_atom2ufs_id = (int *) sxcalloc ((sizeof (operator_atom2atom_list)/sizeof (int))+1, sizeof (int));
  X_alloc (&atom_id2local_atom_id_hd, "atom_id2local_atom_id_hd", 2*n+1, 1, atom_id2local_atom_id_hd_oflw, NULL);
  left_local_atom_id_set = sxba_calloc (X_size (atom_id2local_atom_id_hd)+1);
  right_local_atom_id_set = sxba_calloc (X_size (atom_id2local_atom_id_hd)+1);
#endif /* ESSAI */

  XH_alloc (&XH_ufs_hd, "XH_ufs_hd", n+20+1, 1, 5, NULL, NULL); /* A VOIR */
  XH_set (&XH_ufs_hd, &empty_ufs_id);
  XxY_alloc (&field_paths, "field_paths", n*MAX_FIELD_ID+1, 1, 0, 0, field_paths_oflw, NULL);
  path2wfs_id = (struct path_instance*) sxalloc (XxY_size (field_paths)+1, sizeof (struct path_instance));
  XxY_alloc (&XxY_dol_fs_id, "XxY_dol_fs_id", MAX_FIELD_ID+1, 1, 0, 0, XxY_dol_fs_id_oflw, NULL);
  dol_fs_id2wfs_id = (int*) sxalloc (XxY_size (XxY_dol_fs_id)+1, sizeof (int));

  /* On alloue les f_structures de travail ... */
  wfs_area_allocate ();

  re_results = (struct re_result*) sxalloc (25+1, sizeof (struct re_result));
  re_results [0].kind = 0; /* top */
  re_results [0].next = 25; /* size */

  int_tank_alloc (1, AREA_SIZE);

  XxYxZ_alloc (&prio_hd, "prio_hd", n+1, 1, prio_hd_assoc_list /* pas de foreach */, NULL, NULL);
  XxY_alloc (&dl_hd, "dl_hd", 2*n+1, 1, 0 /* pas de foreach */, 0, dl_hd_oflw, NULL);
#if 0
  XxY_alloc (&dl_hd, "dl_hd", 200*n+1, 1, 0 /* pas de foreach */, 0, dl_hd_oflw, NULL);
#endif /* 0 */
  dl_id2fs_id = (int*) sxalloc (XxY_size (dl_hd)+1, sizeof (int));

  DALLOC_STACK (post_stack, 100);
  DALLOC_STACK (post_display, 10);
  DALLOC_STACK (post_eval_stack, 100);
  DALLOC_STACK (post_hook_stack, 100);

  Pij2disp = (int*) sxcalloc (spf.outputG.maxxprod+1, sizeof (int));
  DALLOC_STACK (fs_id_Pij_dstack, 5*spf.outputG.maxxprod+1);

#if FS_PROBAS
  if (is_proba_ranking) {
    XH_unlock (fs_signatures_hd);
  }
#endif /* FS_PROBAS */

  DALLOC_STACK (heads_stack, 100);

  maximal_tree_Aij_span = (int*) sxalloc (spf.outputG.maxxnt+1, sizeof (int));
  maximal_tree_Pij_span = (int*) sxalloc (spf.outputG.maxxprod+1, sizeof (int));

#if IS_OPERATOR_LEXEME_REF
  sxword_reuse (&atom_names, "atom_names", sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
#endif /* IS_OPERATOR_LEXEME_REF */
}

static void
free_all ()
{
  sxfree (to_be_erased_Pij_set), to_be_erased_Pij_set = NULL;
  sxfree (erased_by_ranking_Pij_set), erased_by_ranking_Pij_set = NULL;

  if (ranking_by_nbest_root_Pij_set) {
    sxfree (ranking_by_nbest_root_Pij_set), ranking_by_nbest_root_Pij_set = NULL;
    if (nbest_save_nbest_walk.Aij2nyp_nb) sxfree (nbest_save_nbest_walk.Aij2nyp_nb), nbest_save_nbest_walk.Aij2nyp_nb = NULL;
    if (nbest_save_nbest_walk.Aij_stack) sxfree (nbest_save_nbest_walk.Aij_stack), nbest_save_nbest_walk.Aij_stack = NULL;
    if (nbest_save_nbest_walk.Aij_set) sxfree (nbest_save_nbest_walk.Aij_set), nbest_save_nbest_walk.Aij_set = NULL;
    if (nbest_save_nbest_walk.cyclic_Aij_set) sxfree (nbest_save_nbest_walk.cyclic_Aij_set), nbest_save_nbest_walk.cyclic_Aij_set = NULL;
    if (nbest_save_nbest_walk.invalid_Aij_set) sxfree (nbest_save_nbest_walk.invalid_Aij_set), nbest_save_nbest_walk.invalid_Aij_set = NULL;
  }

  XH_free (&dynam_pred_vals);

  sxfree (Xpq2disp), Xpq2disp = NULL;
  sxfree (Xpq_set), Xpq_set = NULL;
  if (consistent_Xpq_set) sxfree (consistent_Xpq_set), consistent_Xpq_set = NULL;
  if (consistent_Pij_set) sxfree (consistent_Pij_set), consistent_Pij_set = NULL;
  DFREE_STACK (fs_id_dstack);
  DFREE_STACK (ni_wfs_id_stack);
  SS_free (fs_id_ssstack), fs_id_ssstack = NULL;
  SS_free (ufs_id_ssstack), ufs_id_ssstack = NULL;
  DFREE_STACK (eoaiu_stack);
  XxY_free (&heads);
  X_free (&shared_hd);
  XH_free (&XH_fs_hd);
  XH_free (&XH_cyclic_fs_hd);
  XH_free (&XH_cyclic_ufs_hd);
  X_free (&fs_id_coherence_hd);
  sxfree (fs_id_coherence2attr), fs_id_coherence2attr = NULL;
  sxfree (fs_id_set), fs_id_set = NULL;
  sxfree (visited_fs_id_set), visited_fs_id_set = NULL;
  sxfree (fs_id_stack), fs_id_stack = NULL;
  sxfree (visited_fs_id_stack), visited_fs_id_stack = NULL;
  sxfree (fs_id2dynweight), fs_id2dynweight = NULL;
  sxfree (fs_id2localdynweight), fs_id2localdynweight = NULL;
  DFREE_STACK (heads_stack);
  if (working_heads_stack) {
    DFREE_STACK (working_heads_stack);
    DFREE_STACK (working_value_stack);
#if FS_PROBAS
    if (working_double_stack)
      sxfree (working_double_stack), working_double_stack = NULL;
#endif /* FS_PROBAS */
  }
  XH_free (&XH_ufs_hd);

#ifdef ESSAI
  sxfree (operator_atom2ufs_id), operator_atom2ufs_id = NULL;
  X_free (&atom_id2local_atom_id_hd);
  sxfree (left_local_atom_id_set), left_local_atom_id_set = NULL;
  sxfree (right_local_atom_id_set), right_local_atom_id_set = NULL;
#endif /* ESSAI */

  XxY_free (&field_paths);
  sxfree (path2wfs_id), path2wfs_id = NULL;
  XxY_free (&XxY_dol_fs_id);
  sxfree (dol_fs_id2wfs_id), dol_fs_id2wfs_id = NULL;

  if (memo_dol_fs_id_set) {
    XxY_free (&XxY_memo_dol_fs_id);
    sxfree (memo_dol_fs_id_set), memo_dol_fs_id_set = NULL;
    sxfree (cyclic_dol_fs_id_set), cyclic_dol_fs_id_set = NULL;
  }

  wfs_area_free ();
  sxfree (re_results), re_results = NULL;

  int_tank_free ();

  if (failure_set) sxfree (failure_set), failure_set = NULL;

  XxYxZ_free (&prio_hd);
  XxY_free (&dl_hd);
  sxfree (dl_id2fs_id), dl_id2fs_id = NULL;

  DFREE_STACK (post_stack);
  DFREE_STACK (post_display);
  DFREE_STACK (post_eval_stack);
  DFREE_STACK (post_hook_stack);

  sxfree (Pij2disp), Pij2disp = NULL;
  DFREE_STACK (fs_id_Pij_dstack);

#if FS_PROBAS
  if (is_proba_ranking && XH_is_allocated (fs_signatures_hd))
    /* On a du malloquer fs_signatures_hd !! */
    XH_free (&fs_signatures_hd);
#endif /* FS_PROBAS */

  sxfree (maximal_tree_Aij_span), maximal_tree_Aij_span = NULL;
  sxfree (maximal_tree_Pij_span), maximal_tree_Pij_span = NULL;

  if (son_of_maximal_Xpq_set) sxfree (son_of_maximal_Xpq_set), son_of_maximal_Xpq_set = NULL;

  if (cyclic_wfs_id2fs_id) sxfree (cyclic_wfs_id2fs_id), cyclic_wfs_id2fs_id = NULL;

  if (XxY_is_allocated (cyclic_signXfs_id_hd)) {
    XxY_free (&cyclic_signXfs_id_hd);
    XxY_free (&cyclic_sizeXgrp_id_hd);
    DFREE_STACK (cyclic_grp_stack);
    DFREE_STACK (cyclic_adjunct_stack);
#if ATOM_Aij || ATOM_Pij
    sxfree (Aij_or_Pij_set);
#endif /* ATOM_Aij || ATOM_Pij */
  }

#define include_qq_valeurs_free 1
#ifdef lfg_disamb_h
#include lfg_disamb_h
#endif /* lfg_disamb_h */
#undef include_qq_valeurs_free

#if FS_PROBAS
  if (is_nbest_ranking)
    nbest_close ();

  if (nbest_save_cur_walk.Aij_top) {
    /* On est su^r qu'il y a eu un appel a nbest_allocate ()
     is_nbest_ranking ne suffit pas */
    nbest_free ();
  }
#endif /* FS_PROBAS */

#if IS_OPERATOR_LEXEME_REF
  sxword_free (&atom_names);
#endif /* IS_OPERATOR_LEXEME_REF */
}

static void
lfg_sem_final ()
{
  free_all ();
}

#ifndef ESSAI
static SXBOOLEAN
has_a_disjonctive_value (atom_val)
     int atom_val;
{
  int disjonction_nb = 0;

  do {
    if (atom_val & 1) {
      if (++disjonction_nb > 1)
	return SXTRUE;
    }
  } while ((atom_val >>= 1) != 0);

  return SXFALSE;
}
#endif /* ESSAI */


#if 0
/* Version du 06/06/05 pas de shared_struct_nb */
/* Recopie l'ensemble de structures cur_adjunct_top ds XH_ufs_id */
static int
uwfs2ufs_id (adjunct_set_id, must_be_closed, cur_wfs_id, static_field_kind)
     int           adjunct_set_id;
     SXBOOLEAN       must_be_closed;
     int           cur_wfs_id;
     unsigned char static_field_kind;
{
  int                         id, ufs_id, cur, bot, val, top, top_val, fs_id, top2, bot2, val2, flag, cur_adjunct_top, x;
  SXBOOLEAN                     is_empty, is_closed, is_shared, has_multiple_preds;
  struct adjunct_set_item     *adjunct_ptr;
  struct field_val2adjunct_id *field_val2adjunct_id_ptr;
  SXBA                        wfs_id_fathers_set;

  if (adjunct_set_id <= 0)
    return 0;

  adjunct_set_id = get_adjunct_equiv (adjunct_set_id);
  field_val2adjunct_id_ptr = field_val2adjunct_id + adjunct_set_id;
  ufs_id = field_val2adjunct_id_ptr->ufs_id;
    
  has_multiple_preds = has_multiple_in_wfs_id (adjunct_set_id);

  if (has_multiple_preds)
    wfs_id_fathers_set = wfs_id2fathers_set [cur_wfs_id];

  if (ufs_id)
    /* deja calcule' */
    return ufs_id;

  cur_adjunct_top = field_val2adjunct_id_ptr->adjunct_set_id;
  adjunct_ptr = adjunct_sets + cur_adjunct_top;

#if EBUG
  if (((static_field_kind == ATOM_KIND) || (static_field_kind == ATOM_KIND+UNBOUNDED_KIND)) && adjunct_ptr->next != 0)
    sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */

  if (adjunct_ptr->next == 0 &&
      (adjunct_ptr->kind == ADJUNCT_KIND_UFS || (static_field_kind & ATOM_KIND))) {
    /* Un seul element qui est un ufs, on ne fait (presque) rien */
    if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
      /* adjunct_ptr->id est un ufs_id */
      /* cet adjunct set est-il partage' ? */
      bot = XH_X (XH_ufs_hd, adjunct_ptr->id);
      top = XH_X (XH_ufs_hd, adjunct_ptr->id+1);

      if (bot == top) {
	/* provient d'un vide non partage ... */
	if (!has_multiple_preds)
	/* ... et ne change pas */
	  return adjunct_ptr->id;

	/* de vide non partage va passer a vide partage' */
	is_shared = SXTRUE;
	is_closed = SXTRUE;
      }
      else {
	top_val = XH_list_elem (XH_ufs_hd, top-1);

	if (top_val < 0) {
	  top--;
	  is_shared = ((top_val & X20) != 0);
	}
	else {
	  is_shared = SXFALSE;
	}

	/* Si un UFS reste partage', son nom peut changer d'une fois a l'autre !! */
	/* Tant pis, on garde celui du coup d'avant */
	/* D'ailleurs utilise t'on vraiment ce nom ? */
	if (is_shared && has_multiple_preds /* les 2 partages */ ||
	    !is_shared && !has_multiple_preds /* les 2 non partages */) {
	  /* l'ufs_id ne change pas */
	  if (has_multiple_preds && wfs_id_fathers_set) {
	    /* On ne repercute les predecesseurs multiples sur father que si l'atome peut encore
	       se modifier ulterieurement. Cad vide ou disjonction */
	    if ((static_field_kind & ATOM_KIND) == 0 /* pas atome */ ||
		bot == top /* vide */ ||
		static_field_kind == ATOM_KIND /* valeur atomique qui peut etre disjonctive ... */ &&
		has_a_disjonctive_value (XH_list_elem (XH_ufs_hd, bot)) /* ... et qui l'est */) {
	      sxba_or (wfs_id_fathers_set, field_val2wfs_id_set [adjunct_set_id]);
	    }
	  }

	  return adjunct_ptr->id;
	}

	/* ... sinon, on le change de categorie */
	if (top_val < 0)
	  is_closed = (top_val & X10) == 0;
	else
	  is_closed = SXTRUE; 

#if ATOM_Aij || ATOM_Pij
#if EBUG
	if (must_be_closed && !is_closed)
	  sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */
#endif /* ATOM_Aij || ATOM_Pij */
	      
	for (cur = bot;
	     cur < top;
	     cur++) {
	  fs_id = XH_list_elem (XH_ufs_hd, cur);
	  XH_push (XH_ufs_hd, fs_id);
	}
      }
	
      if (has_multiple_preds)
	flag = X80+X20;
      else
	flag = 0;

      if (!is_closed)
	flag |= X80+X10;
    }
    else {
      if (adjunct_ptr->kind != ADJUNCT_KIND_EMPTY)
	XH_push (XH_ufs_hd, adjunct_ptr->id);

      if (has_multiple_preds)
	flag = X80+X20;
      else
	flag = 0;
    }

    if (flag)
      XH_push (XH_ufs_hd, flag);

    XH_set (&XH_ufs_hd, &ufs_id);

    /* on s'en souvient */
    field_val2adjunct_id_ptr->ufs_id = ufs_id;
	  
    if (has_multiple_preds && wfs_id_fathers_set) {
      /* On ne repercute les predecesseurs multiples sur father que si l'atome peut encore
	 se modifier ulterieurement. Cad vide ou disjonction */
      if ((static_field_kind & ATOM_KIND) == 0 /* pas atome */)
	sxba_or (wfs_id_fathers_set, field_val2wfs_id_set [adjunct_set_id]);
      else {
	bot = XH_X (XH_ufs_hd, ufs_id);
	top = XH_X (XH_ufs_hd, ufs_id+1);

	if (flag)
	  top--;

	if (bot == top /* vide */ ||
	    static_field_kind == ATOM_KIND /* valeur atomique qui peut etre disjonctive ... */ &&
	    has_a_disjonctive_value (XH_list_elem (XH_ufs_hd, bot)) /* ... et qui l'est */) {
	  sxba_or (wfs_id_fathers_set, field_val2wfs_id_set [adjunct_set_id]);
	}
      }
    }

    return ufs_id;
  }

  is_empty = SXTRUE;
  is_closed = SXTRUE;
  is_shared = has_multiple_preds;
	  
  if (has_multiple_preds && wfs_id_fathers_set) {
    /* Pas valeur atomique ... */
    /* On met les predecesseurs multiples ds wfs_id_fathers_set */
    sxba_or (wfs_id_fathers_set, field_val2wfs_id_set [adjunct_set_id]);
  }

  while (cur_adjunct_top) {
    adjunct_ptr = adjunct_sets + cur_adjunct_top;
    cur_adjunct_top = adjunct_ptr->next;
    id = adjunct_ptr->id;

    if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
      /* id est un ufs_id */ 
      top = XH_X (XH_ufs_hd, id+1);
      bot = XH_X (XH_ufs_hd, id);

      if (is_closed && bot < top) {
	val = XH_list_elem (XH_ufs_hd, top-1);

	if (val < 0) {
	  if (val & X10)
	    is_closed = SXFALSE;
	  
	  top--;
	}
      }
	      
      for (cur = bot;
	   cur < top;
	   cur++) {
	fs_id = XH_list_elem (XH_ufs_hd, cur);
	SXBA_1_bit (fs_id_set, fs_id);
	is_empty = SXFALSE;
      }
    }
    else { 
#if ATOM_Aij || ATOM_Pij
#if EBUG
      if (must_be_closed && (adjunct_ptr->kind == ADJUNCT_KIND_FS || adjunct_ptr->kind == ADJUNCT_KIND_WFS))
	sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */
#endif /* ATOM_Aij || ATOM_Pij */

      if (adjunct_ptr->kind == ADJUNCT_KIND_FS) {
	if (is_closed) {
	  top2 = XH_X (XH_fs_hd, id+1);
	  bot2 = XH_X (XH_fs_hd, id);

	  if (bot2 < top2) {
	    val2 = XH_list_elem (XH_fs_hd, top2-1);

	    if (val2 & (X80+X40+X20+X10))
	      /* cyclique ou partagee ou non close */
	      is_closed = SXFALSE;
	  }
	}
      
	SXBA_1_bit (fs_id_set, id);
	is_empty = SXFALSE;
      }
      else {
	if (adjunct_ptr->kind == ADJUNCT_KIND_WFS) {
	  id = get_equiv (id);

	  fs_id = wfs2attr [AREA_PAGE (id)] [AREA_DELTA (id)] [0];

#if EBUG
	  if (fs_id == 0)
	    sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */

	  if (fs_id & X80)
	    /* Ici le fs_id a ete calcule' par simulation, et il vaut ... */
	    fs_id &= ~X80;

	  if (fs_id & X40) {
	    /* Ici cas cyclique, le fs_id est le fs_id simule' */
	    fs_id &= ~X40;

	    if (is_closed) {
	      /* On regarde si ce cycle contient cur_wfs_id : cas du vrai cycle */
	      if ((wfs2attr [AREA_PAGE (cur_wfs_id)] [AREA_DELTA (cur_wfs_id)] [0] & (X80+X40)) == X80+X40) {
		/* Le pere cur_wfs_id est cyclique ... */
		if (SXBA_bit_is_set (wfs_id2sons_set [id], cur_wfs_id))
		  /* ... et est un fils de id */
		  is_closed = SXFALSE;
	      }
	    }
	  }

	  if (fs_id > 0) {
	    /* En fait, ici, le wfs_id est traite et a deja ete range' ds un fs_id */
	    if (is_closed) {
	      top2 = XH_X (XH_fs_hd, fs_id+1);
	      bot2 = XH_X (XH_fs_hd, fs_id);

	      if (bot2 < top2) {
		val2 = XH_list_elem (XH_fs_hd, top2-1);
		/* non vide */

		if (val2 < 0) {
		  if (val2 & (X20+X10)) {
		    /* partagee ou non close */
		    /* On pourrait etre + fin et dire qu'il y a des partagees, des non closes */
		    is_closed = SXFALSE;
		  }
		  else {
		    /* cyclique */
		    /* On regarde si l'un de ses descendants est cur_wfs_id : cas du vrai cycle */
		    if (wfs2attr [AREA_PAGE (cur_wfs_id)] [AREA_DELTA (cur_wfs_id)] [0] < 0) {
		      /* Le pere cur_wfs_id est cyclique ... */
		      if (SXBA_bit_is_set (wfs_id2sons_set [id], cur_wfs_id))
			/* ... et est un fils de id */
			is_closed = SXFALSE;
		    }
		  }
		}
	      }
	    }
	  }
      
	  SXBA_1_bit (fs_id_set, fs_id);
	  is_empty = SXFALSE;
	}
	else {
#if ATOM_Aij || ATOM_Pij
	  if (adjunct_ptr->kind == ADJUNCT_KIND_Aij) {
#if EBUG
	    if (!must_be_closed)
	      sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */

	    SXBA_1_bit (fs_id_set, id);
	    is_empty = SXFALSE;
	  }
	  else
#endif /* ATOM_Aij || ATOM_Pij */
	    /* adjunct_ptr->kind == ADJUNCT_KIND_EMPTY */
	    sxtrap (ME, "uwfs2ufs_id");
	}
      }
    }
  } 

#if ATOM_Aij || ATOM_Pij
#if EBUG
  if (must_be_closed && !is_closed)
    sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */
#endif /* ATOM_Aij || ATOM_Pij */

  id = 0;

  while ((id = sxba_scan_reset (fs_id_set, id)) > 0) {
    XH_push (XH_ufs_hd, id);
  }
  
  if (is_shared || !is_closed) {
    flag = (is_shared) ? (X80+X20) : 0;

    if (!is_closed)
      /* Signifie que l'un au moins des fs_id est cyclique ou partage ou non clos */
      flag |= X80+X10;

    XH_push (XH_ufs_hd, flag);
  }

  XH_set (&XH_ufs_hd, &ufs_id);

  field_val2adjunct_id_ptr->ufs_id = ufs_id;

  return ufs_id;
}
#endif /* 0 */


/* Version du 06/06/05 pas de shared_struct_nb */
/* Recopie l'ensemble de structures cur_adjunct_top ds XH_ufs_id */
static int
uwfs2ufs_id (adjunct_set_id, must_be_closed, cur_wfs_id, static_field_kind)
     int           adjunct_set_id;
     SXBOOLEAN       must_be_closed;
     int           cur_wfs_id;
     unsigned char static_field_kind;
{
  int                         id, ufs_id, cur, bot, val, top, top_val, fs_id, top2, bot2, val2, flag, cur_adjunct_top, x;
  SXBOOLEAN                     is_empty, is_closed, is_shared, has_multiple_preds;
  struct adjunct_set_item     *adjunct_ptr;
  struct field_val2adjunct_id *field_val2adjunct_id_ptr;
  SXBA                        wfs_id_fathers_set;

  if (adjunct_set_id <= 0)
    return 0;

  adjunct_set_id = get_adjunct_equiv (adjunct_set_id);
  field_val2adjunct_id_ptr = field_val2adjunct_id + adjunct_set_id;
  ufs_id = field_val2adjunct_id_ptr->ufs_id;
    
  has_multiple_preds = has_multiple_in_wfs_id (adjunct_set_id);

  if (has_multiple_preds)
    wfs_id_fathers_set = wfs_id2fathers_set [cur_wfs_id];

  if (ufs_id)
    /* deja calcule' */
    return ufs_id;

  cur_adjunct_top = field_val2adjunct_id_ptr->adjunct_set_id;
  adjunct_ptr = adjunct_sets + cur_adjunct_top;

#ifndef ESSAI
#if EBUG
  if (((static_field_kind == ATOM_KIND) || (static_field_kind == ATOM_KIND+UNBOUNDED_KIND)) && adjunct_ptr->next != 0)
    sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */
#endif /* ESSAI */

  if (
#ifndef ESSAI
      ((static_field_kind == ATOM_KIND) || (static_field_kind == ATOM_KIND+UNBOUNDED_KIND)) && adjunct_ptr->next != 0
#else /* ESSAI */
      adjunct_ptr->next == 0 && adjunct_ptr->kind == ADJUNCT_KIND_UFS || (static_field_kind == ATOM_KIND) || (static_field_kind == ATOM_KIND+UNBOUNDED_KIND)
#if ATOM_Aij || ATOM_Pij
      || ((static_field_kind & Aij_KIND) && adjunct_ptr->next == 0 /* Les multiples sont pris ds le else */)
#endif /* ATOM_Aij || ATOM_Pij */
#endif /* ESSAI */
      ) {
    if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
      /* Un seul element qui est un ufs, on ne fait (presque) rien */
      /* adjunct_ptr->id est un ufs_id */
      /* cet adjunct set est-il partage' ? */
      bot = XH_X (XH_ufs_hd, adjunct_ptr->id);
      top = XH_X (XH_ufs_hd, adjunct_ptr->id+1);

      if (bot == top) {
	/* provient d'un vide non partage ... */
	if (!has_multiple_preds)
	  /* ... et ne change pas */
	  return adjunct_ptr->id;

	/* de vide non partage va passer a vide partage' */
	is_shared = SXTRUE;
	is_closed = SXTRUE;
      }
      else {
	top_val = XH_list_elem (XH_ufs_hd, top-1);

	if (top_val < 0) {
	  top--;
	  is_shared = ((top_val & X20) != 0);
	}
	else {
	  is_shared = SXFALSE;
	}

	/* Si un UFS reste partage', son nom peut changer d'une fois a l'autre !! */
	/* Tant pis, on garde celui du coup d'avant */
	/* D'ailleurs utilise t'on vraiment ce nom ? */
	if (is_shared && has_multiple_preds /* les 2 partages */ ||
	    !is_shared && !has_multiple_preds /* les 2 non partages */) {
	  /* l'ufs_id ne change pas */
	  if (has_multiple_preds && wfs_id_fathers_set) {
	    /* On ne repercute les predecesseurs multiples sur father que si l'atome peut encore
	       se modifier ulterieurement. Cad vide ou disjonction */
	    if ((static_field_kind & ATOM_KIND) == 0 /* pas atome */ ||
		bot == top /* vide */ ||
#ifndef ESSAI
		static_field_kind == ATOM_KIND /* valeur atomique qui peut etre disjonctive ... */ &&
		has_a_disjonctive_value (XH_list_elem (XH_ufs_hd, bot)) /* ... et qui l'est */
#else /* ESSAI */
		(static_field_kind == ATOM_KIND || (static_field_kind == ATOM_KIND+UNBOUNDED_KIND)) && (top > bot+1) /* disjonction non triviale */
#endif /* ESSAI */
		) {
	      sxba_or (wfs_id_fathers_set, field_val2wfs_id_set [adjunct_set_id]);
	    }
	  }

	  return adjunct_ptr->id;
	}

	/* ... sinon, on le change de categorie */
	if (top_val < 0)
	  is_closed = (top_val & X10) == 0;
	else
	  is_closed = SXTRUE; 

#if ATOM_Aij || ATOM_Pij
#if EBUG
	if (must_be_closed && !is_closed)
	  sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */
#endif /* ATOM_Aij || ATOM_Pij */
	      
	for (cur = bot;
	     cur < top;
	     cur++) {
	  fs_id = XH_list_elem (XH_ufs_hd, cur);
	  XH_push (XH_ufs_hd, fs_id);
	}
      }
	
      if (has_multiple_preds)
	flag = X80+X20;
      else
	flag = 0;

      if (!is_closed)
	flag |= X80+X10;
    }
    else {
      do {
	adjunct_ptr = adjunct_sets + cur_adjunct_top;

	if (adjunct_ptr->kind == ADJUNCT_KIND_ATOM
#if ATOM_Aij || ATOM_Pij
	    || adjunct_ptr->kind == ADJUNCT_KIND_Aij
#endif /* ATOM_Aij || ATOM_Pij */
	    )
	  XH_push (XH_ufs_hd, adjunct_ptr->id);
      } while (cur_adjunct_top = adjunct_ptr->next);

      if (has_multiple_preds)
	flag = X80+X20;
      else
	flag = 0;
    }

    if (flag)
      XH_push (XH_ufs_hd, flag);

    XH_set (&XH_ufs_hd, &ufs_id);

    /* on s'en souvient */
    field_val2adjunct_id_ptr->ufs_id = ufs_id;
	  
    if (has_multiple_preds && wfs_id_fathers_set) {
      /* On ne repercute les predecesseurs multiples sur father que si l'atome peut encore
	 se modifier ulterieurement. Cad vide ou disjonction */
      if ((static_field_kind & ATOM_KIND) == 0 /* pas atome */)
	sxba_or (wfs_id_fathers_set, field_val2wfs_id_set [adjunct_set_id]);
      else {
	bot = XH_X (XH_ufs_hd, ufs_id);
	top = XH_X (XH_ufs_hd, ufs_id+1);

	if (flag)
	  top--;

	if (bot == top /* vide */ ||
#ifndef ESSAI
	    static_field_kind == ATOM_KIND /* valeur atomique qui peut etre disjonctive ... */ &&
	    has_a_disjonctive_value (XH_list_elem (XH_ufs_hd, bot)) /* ... et qui l'est */
#else /* ESSAI */
	    (static_field_kind == ATOM_KIND || (static_field_kind == ATOM_KIND+UNBOUNDED_KIND)) && (top > bot+1) /* disjonction non triviale */
#endif /* ESSAI */
	    ) {
	  sxba_or (wfs_id_fathers_set, field_val2wfs_id_set [adjunct_set_id]);
	}
      }
    }

    return ufs_id;
  }

  is_empty = SXTRUE;
  is_closed = SXTRUE;
  is_shared = has_multiple_preds;
	  
  if (has_multiple_preds && wfs_id_fathers_set) {
    /* Pas valeur atomique ... */
    /* On met les predecesseurs multiples ds wfs_id_fathers_set */
    sxba_or (wfs_id_fathers_set, field_val2wfs_id_set [adjunct_set_id]);
  }

  while (cur_adjunct_top) {
    adjunct_ptr = adjunct_sets + cur_adjunct_top;
    cur_adjunct_top = adjunct_ptr->next;
    id = adjunct_ptr->id;

    if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
      /* id est un ufs_id */ 
      top = XH_X (XH_ufs_hd, id+1);
      bot = XH_X (XH_ufs_hd, id);

      if (is_closed && bot < top) {
	val = XH_list_elem (XH_ufs_hd, top-1);

	if (val < 0) {
	  if (val & X10)
	    is_closed = SXFALSE;
	  
	  top--;
	}
      }
	      
      for (cur = bot;
	   cur < top;
	   cur++) {
	fs_id = XH_list_elem (XH_ufs_hd, cur);
	SXBA_1_bit (fs_id_set, fs_id);
	is_empty = SXFALSE;
      }
    }
    else { 
#if ATOM_Aij || ATOM_Pij
#if EBUG
      if (must_be_closed && (adjunct_ptr->kind == ADJUNCT_KIND_FS || adjunct_ptr->kind == ADJUNCT_KIND_WFS))
	sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */
#endif /* ATOM_Aij || ATOM_Pij */

      if (adjunct_ptr->kind == ADJUNCT_KIND_FS) {
	if (is_closed) {
	  top2 = XH_X (XH_fs_hd, id+1);
	  bot2 = XH_X (XH_fs_hd, id);

	  if (bot2 < top2) {
	    val2 = XH_list_elem (XH_fs_hd, top2-1);

	    if (val2 & (X80+X40+X20+X10))
	      /* cyclique ou partagee ou non close */
	      is_closed = SXFALSE;
	  }
	}
      
	SXBA_1_bit (fs_id_set, id);
	is_empty = SXFALSE;
      }
      else {
	if (adjunct_ptr->kind == ADJUNCT_KIND_WFS) {
	  id = get_equiv (id);

	  fs_id = wfs2attr [AREA_PAGE (id)] [AREA_DELTA (id)] [0];

#if EBUG
	  if (fs_id == 0)
	    sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */

	  if (fs_id & X80)
	    /* Ici le fs_id a ete calcule' par simulation, et il vaut ... */
	    fs_id &= ~X80;

	  if (fs_id & X40) {
	    /* Ici cas cyclique, le fs_id est le fs_id simule' */
	    fs_id &= ~X40;

	    if (is_closed) {
	      /* On regarde si ce cycle contient cur_wfs_id : cas du vrai cycle */
	      if ((wfs2attr [AREA_PAGE (cur_wfs_id)] [AREA_DELTA (cur_wfs_id)] [0] & (X80+X40)) == X80+X40) {
		/* Le pere cur_wfs_id est cyclique ... */
		if (SXBA_bit_is_set (wfs_id2sons_set [id], cur_wfs_id))
		  /* ... et est un fils de id */
		  is_closed = SXFALSE;
	      }
	    }
	  }

	  if (fs_id > 0) {
	    /* En fait, ici, le wfs_id est traite et a deja ete range' ds un fs_id */
	    if (is_closed) {
	      top2 = XH_X (XH_fs_hd, fs_id+1);
	      bot2 = XH_X (XH_fs_hd, fs_id);

	      if (bot2 < top2) {
		val2 = XH_list_elem (XH_fs_hd, top2-1);
		/* non vide */

		if (val2 < 0) {
		  if (val2 & (X20+X10)) {
		    /* partagee ou non close */
		    /* On pourrait etre + fin et dire qu'il y a des partagees, des non closes */
		    is_closed = SXFALSE;
		  }
		  else {
		    /* cyclique */
		    /* On regarde si l'un de ses descendants est cur_wfs_id : cas du vrai cycle */
		    if (wfs2attr [AREA_PAGE (cur_wfs_id)] [AREA_DELTA (cur_wfs_id)] [0] < 0) {
		      /* Le pere cur_wfs_id est cyclique ... */
		      if (SXBA_bit_is_set (wfs_id2sons_set [id], cur_wfs_id))
			/* ... et est un fils de id */
			is_closed = SXFALSE;
		    }
		  }
		}
	      }
	    }
	  }
      
	  SXBA_1_bit (fs_id_set, fs_id);
	  is_empty = SXFALSE;
	}
	else {
#if ATOM_Aij || ATOM_Pij
	  if (adjunct_ptr->kind == ADJUNCT_KIND_Aij) {
#if EBUG
	    if (!must_be_closed)
	      sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */

	    SXBA_1_bit (fs_id_set, id);
	    is_empty = SXFALSE;
	  }
	  else
#endif /* ATOM_Aij || ATOM_Pij */
	    /* adjunct_ptr->kind == ADJUNCT_KIND_EMPTY */
	    sxtrap (ME, "uwfs2ufs_id");
	}
      }
    }
  } 

#if ATOM_Aij || ATOM_Pij
#if EBUG
  if (must_be_closed && !is_closed)
    sxtrap (ME, "uwfs2ufs_id");
#endif /* EBUG */
#endif /* ATOM_Aij || ATOM_Pij */

  id = 0;

  while ((id = sxba_scan_reset (fs_id_set, id)) > 0) {
    XH_push (XH_ufs_hd, id);
  }
  
  if (is_shared || !is_closed) {
    flag = (is_shared) ? (X80+X20) : 0;

    if (!is_closed)
      /* Signifie que l'un au moins des fs_id est cyclique ou partage ou non clos */
      flag |= X80+X10;

    XH_push (XH_ufs_hd, flag);
  }

  XH_set (&XH_ufs_hd, &ufs_id);

  field_val2adjunct_id_ptr->ufs_id = ufs_id;

  return ufs_id;
}



struct store_wfs {
  int                     wfs_id, wfs_page_nb, XH_fs_hd_size, XH_ufs_hd_size;
  SXBA                    **wfs_field_set;
  int                     ***wfs2attr; /* (val<<STRUCTURE_NAME_shift)+kind */
  /* int                  *wfs_id2son_nb; */
  int                     *wfs_id2equiv;
  /* struct pred_list_hd     *wfs_id2wfs_id_pred_list; */
  /* struct wfs_id_pred_item *wfs_id_pred_list; */
  int                     field_paths_size, field_paths_top, XxY_dol_fs_id_size, XxY_dol_fs_id_top;
  int                     adjunct_sets_top, field_val2adjunct_id_top;
  struct path_instance    *path2wfs_id;
  int                     *dol_fs_id2wfs_id;
  /* SXBA                 wfs_id_set; unused */
  SXBA                    doli_used_set;
  /* SXBA                    mdoli_used_set; */
};

static struct store_wfs_hd {
  int              wfs_size;
  struct store_wfs **wfs;
  SXBA             wfs_set;
} store_wfs_hd;

/* Procedure utilisee ds le cas d'expressions regulieres non triviales sur $$ */
/* Elle stocke la valeur actuelle de wfs */
static int
put_in_store_wfs ()
{
  int              i, size;
  int              cur_wfs_id, page_nb, delta_nb, id, fs_id, field_id;
  int              *attr_ptr, *store_attr_ptr;
  int              x, *q, **p;
  SXBA             field_set;
  struct store_wfs *store_wfs_ptr;

  if (store_wfs_hd.wfs == NULL) {
    /* 1er appel de put_in_store_wfs () */
    store_wfs_hd.wfs = (struct store_wfs **) sxcalloc (store_wfs_hd.wfs_size = 20, sizeof (struct store_wfs *));
    store_wfs_hd.wfs_set = sxba_calloc (store_wfs_hd.wfs_size);
  }

  /* On cherche le 1er libre ... */
  i = -1;

  while ((i = sxba_0_lrscan (store_wfs_hd.wfs_set, i)) >= 0) {
    /* ... i l'est ... */
    if ((store_wfs_ptr = store_wfs_hd.wfs [i]) == NULL) {
      /* ... et n'a jamais ete alloue' => OK */
      break;
    }

    if (store_wfs_ptr->wfs_page_nb >= wfs_page_nb) {
      /* deja alloue' et sa taille convient */
      break;
    }
  }

  if (i < 0) {
    /* Rien qui convient, on realloue */
    i = store_wfs_hd.wfs_size;
    store_wfs_hd.wfs = (struct store_wfs **) sxrecalloc (store_wfs_hd.wfs,
							 i,
							 store_wfs_hd.wfs_size *= 2,
							 sizeof (struct store_wfs *));
    store_wfs_hd.wfs_set = sxba_resize (store_wfs_hd.wfs_set, store_wfs_hd.wfs_size);
  }

  if ((store_wfs_ptr = store_wfs_hd.wfs [i]) == NULL) {
    store_wfs_ptr = store_wfs_hd.wfs [i] = (struct store_wfs*) sxalloc (1, sizeof (struct store_wfs));

    /* nb de pages de l'allocation courante */
    store_wfs_ptr->wfs_page_nb = wfs_page_nb;

    /* ... et on alloue */
    size = AREA_SIZE*wfs_page_nb;

    store_wfs_ptr->wfs_field_set = (SXBA **) sxalloc (wfs_page_nb, sizeof (SXBA*));
    store_wfs_ptr->wfs2attr = (int***) sxalloc (wfs_page_nb, sizeof (int**));
    store_wfs_ptr->wfs_id2equiv = (int*) sxalloc (size, sizeof (int));
    /* ATTENTION : la reutilisation peut necessiter une reallocation */

    store_wfs_ptr->path2wfs_id =
      (struct path_instance*) sxalloc ((store_wfs_ptr->field_paths_size = XxY_size (field_paths))+1,
				       sizeof (struct path_instance));
    store_wfs_ptr->dol_fs_id2wfs_id =
      (int*) sxalloc ((store_wfs_ptr->XxY_dol_fs_id_size = XxY_size (XxY_dol_fs_id))+1,
		     sizeof (int));
    
    /* store_wfs_ptr->wfs_id_set = sxba_calloc (size); unused */
    store_wfs_ptr->doli_used_set = sxba_calloc (MAXRHS+1);

    for (page_nb = 0; page_nb < wfs_page_nb; page_nb++) {
      store_wfs_ptr->wfs_field_set [page_nb] = sxbm_calloc (AREA_SIZE, MAX_FIELD_ID+1);
      p = store_wfs_ptr->wfs2attr [page_nb] = (int**) sxalloc (AREA_SIZE, sizeof (int*));
      q = p [0] = (int*) sxalloc (AREA_SIZE*(MAX_FIELD_ID+1), sizeof (int));

      for (x = 1; x < AREA_SIZE; x++)
	p [x] = (q += MAX_FIELD_ID+1);
    }
  }
  else {
    if (wfs_id > AREA_SIZE*store_wfs_ptr->wfs_page_nb) {
      /* il faut agrandir */
      size = AREA_SIZE*wfs_page_nb;

      store_wfs_ptr->wfs_field_set = (SXBA **) sxrealloc (store_wfs_ptr->wfs_field_set, wfs_page_nb, sizeof (SXBA*));
      store_wfs_ptr->wfs2attr = (int***) sxrealloc (store_wfs_ptr->wfs2attr, wfs_page_nb, sizeof (int**));
      store_wfs_ptr->wfs_id2equiv = (int*) sxrealloc (store_wfs_ptr->wfs_id2equiv, size, sizeof (int));

      for (page_nb = store_wfs_ptr->wfs_page_nb; page_nb < wfs_page_nb; page_nb++) {
	store_wfs_ptr->wfs_field_set [page_nb] = sxbm_calloc (AREA_SIZE, MAX_FIELD_ID+1);
	p = store_wfs_ptr->wfs2attr [page_nb] = (int**) sxalloc (AREA_SIZE, sizeof (int*));
	q = p [0] = (int*) sxalloc (AREA_SIZE*(MAX_FIELD_ID+1), sizeof (int));

	for (x = 1; x < AREA_SIZE; x++)
	  p [x] = (q += MAX_FIELD_ID+1);
      }
      
      store_wfs_ptr->wfs_page_nb = wfs_page_nb;
    }
  }

  store_wfs_ptr->adjunct_sets_top = adjunct_sets [0].id;
  store_wfs_ptr->field_val2adjunct_id_top = field_val2adjunct_id [0].equiv;

  store_wfs_ptr->field_paths_top = XxY_top (field_paths);

  if (store_wfs_ptr->field_paths_size < store_wfs_ptr->field_paths_top)
    store_wfs_ptr->path2wfs_id =
      (struct path_instance*) sxrealloc (store_wfs_ptr->path2wfs_id,
				      (store_wfs_ptr->field_paths_top = XxY_size (field_paths))+1,
				      sizeof (struct path_instance));

  for (id = store_wfs_ptr->field_paths_top; id >= 0 ; id--)
    store_wfs_ptr->path2wfs_id [id] = path2wfs_id [id];

  store_wfs_ptr->XxY_dol_fs_id_top = XxY_top (XxY_dol_fs_id);

  if (store_wfs_ptr->XxY_dol_fs_id_size < store_wfs_ptr->XxY_dol_fs_id_top)
    store_wfs_ptr->dol_fs_id2wfs_id =
      (int*) sxrealloc (store_wfs_ptr->dol_fs_id2wfs_id,
			(store_wfs_ptr->XxY_dol_fs_id_top = XxY_size (XxY_dol_fs_id))+1,
			sizeof (int));

  for (id = store_wfs_ptr->XxY_dol_fs_id_top; id >= 0 ; id--)
    store_wfs_ptr->dol_fs_id2wfs_id [id] = dol_fs_id2wfs_id [id];

  /* On peut recopier, on ne touche pas aux XxY */
    

  /* ici la structure store_wfs_ptr est prete a accueillir la copie (elle est allouee et sa taille est suffisante) */
  store_wfs_ptr->wfs_id = wfs_id;

  /* sxba_copy (store_wfs_ptr->wfs_id_set, wfs_id_set); unused */
  sxba_copy (store_wfs_ptr->doli_used_set, doli_used_set);

  for (cur_wfs_id = MAIN_WFS_ID; cur_wfs_id <= wfs_id; cur_wfs_id++) {
    page_nb = AREA_PAGE (cur_wfs_id);
    delta_nb = AREA_DELTA (cur_wfs_id);
    store_wfs_ptr->wfs_id2equiv [cur_wfs_id] = wfs_id2equiv [cur_wfs_id];
    sxba_copy (store_wfs_ptr->wfs_field_set [page_nb] [delta_nb], field_set = wfs_field_set [page_nb] [delta_nb]);
      
    attr_ptr = wfs2attr [page_nb] [delta_nb];
    store_attr_ptr = store_wfs_ptr->wfs2attr [page_nb] [delta_nb];

    field_id = 0;
    store_attr_ptr [field_id] = attr_ptr [field_id];

    while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
      store_attr_ptr [field_id] = attr_ptr [field_id];
    }
  }

  return i;
}

/*BS static*/ void
free_store_wfs ()
{
  int              i, x, **p;
  struct store_wfs *store_wfs_ptr;

  if (store_wfs_hd.wfs) {
    for (i = 0; i < store_wfs_hd.wfs_size; i++) {
      if (store_wfs_ptr = store_wfs_hd.wfs [i]) {
	for (x = store_wfs_ptr->wfs_page_nb-1; x >= 0; x--) {
	  sxbm_free (store_wfs_ptr->wfs_field_set [x]);
	  p =  store_wfs_ptr->wfs2attr [x];
	  sxfree (*p);
	  sxfree (p);
	}

	sxfree (store_wfs_ptr->wfs_field_set);
	sxfree (store_wfs_ptr->wfs2attr);

	sxfree (store_wfs_ptr->wfs_id2equiv);
	sxfree (store_wfs_ptr->doli_used_set);

	sxfree (store_wfs_ptr);
      }
    }

    sxfree (store_wfs_hd.wfs), store_wfs_hd.wfs = NULL;
    sxfree (store_wfs_hd.wfs_set), store_wfs_hd.wfs_set = NULL;
  }
}

/* Reinitialise wfs avec le contenu de store_wfs */
static void
set_wfs_from_store_wfs (i)
     int i;
{
  int              cur_wfs_id, page_nb, delta_nb, id, id_dol, fs_id, field_id, pos;
  int              *attr_ptr, *store_attr_ptr;
  SXBA             field_set;
  struct store_wfs *store_wfs_ptr;

  store_wfs_ptr = store_wfs_hd.wfs [i];

  /* wfs_page_nb = store_wfs_ptr->wfs_page_nb; NON on conserve le wfs_page_nb courant */
  wfs_id = store_wfs_ptr->wfs_id;

  /* sxba_copy (wfs_id_set, store_wfs_ptr->wfs_id_set); unused */
  sxba_copy (doli_used_set, store_wfs_ptr->doli_used_set);

  adjunct_sets [0].id = store_wfs_ptr->adjunct_sets_top;
  field_val2adjunct_id [0].equiv = store_wfs_ptr->field_val2adjunct_id_top;

  for (id = XxY_top (XxY_dol_fs_id); id > store_wfs_ptr->XxY_dol_fs_id_top; id--)
    XxY_erase (XxY_dol_fs_id, id);

  XxY_gc (&XxY_dol_fs_id);

  for (id = store_wfs_ptr->XxY_dol_fs_id_top; id >= 0 ; id--)
    dol_fs_id2wfs_id [id] = store_wfs_ptr->dol_fs_id2wfs_id [id];

  for (id = XxY_top (field_paths); id > store_wfs_ptr->field_paths_top; id--)
    XxY_erase (field_paths, id);

  XxY_gc (&field_paths);

  for (id = store_wfs_ptr->field_paths_top; id >= 0 ; id--)
    path2wfs_id [id] = store_wfs_ptr->path2wfs_id [id];

  for (cur_wfs_id = MAIN_WFS_ID; cur_wfs_id <= wfs_id; cur_wfs_id++) {
    page_nb = AREA_PAGE (cur_wfs_id);
    delta_nb = AREA_DELTA (cur_wfs_id);
    wfs_id2equiv [cur_wfs_id] = store_wfs_ptr->wfs_id2equiv [cur_wfs_id];
    sxba_copy (wfs_field_set [page_nb] [delta_nb], field_set = store_wfs_ptr->wfs_field_set [page_nb] [delta_nb]);
      
    attr_ptr = wfs2attr [page_nb] [delta_nb];
    store_attr_ptr = store_wfs_ptr->wfs2attr [page_nb] [delta_nb];

    field_id = 0;
    attr_ptr [field_id] = store_attr_ptr [field_id];

    while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
      attr_ptr [field_id] = store_attr_ptr [field_id];
    }
  }
}


static void
close_store_wfs_level (i)
     int i;
{
  SXBA_0_bit (store_wfs_hd.wfs_set, i); /* le niveau i peut etre reutilise' */
}

/* On travaille ds fs */
SXBOOLEAN
fs_is_set (fs_id, field_id, ret_val)
     int fs_id, field_id, **ret_val;
{
  /* Recherche dichotomique */
  int bot, top, cur, cur_val, cur_field_id, *cur_val_ptr;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);

  if (XH_list_elem (XH_fs_hd, top-1) < 0)
    /* On est sur une structure nommee */
    top--;
  
  while (bot < top) {
    cur = (bot+top)>>1;

    cur_val = *(cur_val_ptr = &(XH_list_elem (XH_fs_hd, cur)));
    cur_field_id = cur_val & FIELD_AND;

    if (cur_field_id == field_id) {
      *ret_val = cur_val_ptr;
      return SXTRUE;
    }

    if (field_id < cur_field_id)
      top = cur;
    else
      bot = cur+1;
  }

  return SXFALSE;
}



/* On verifie recursivement la coherence de cet fs_id
   Si ce fs_id est non partage' il ne peut pas etre instancie' et ne peut donc
   pas etre modifie' => la version ds fs est OK
   Si ce fs_id est partage', 2 cas :
   - Soit (fs_id, dol) est instancie' et il sera verifie' ds la foulee
   - Soit (fs_id, dol) est non instancie' et il n'a donc pas ete modifie'
     et il peut donc se verifier ds fs
*/
/* Le resultat de la coherence est retourne en 2 parties, le return et l'arg incoherence_locale
   incoherence_locale est uniquement positionne a SXTRUE (si c'est le cas)
   incoherence_locale n'est significatif que si la fonction retourne SXTRUE

   BS: suspect... je dirais plutôt:
     - le return indique la cohérence (return SXTRUE) ou l'incohérence (return SXFALSE) de la structure fs_id
     - incoherence_locale est positionné à SXTRUE ssi fs_id est incohérente (return SXFALSE) et de plus "localement" incohérente
         (le pred de fs_id est incohérent avec la liste de ses fils)
   Dans la mémoïsation:
     - cohérence -> on stocke 2 (3 n'a de sens que si pas de pred: cohérence malgré incohérence locale)
     - incohérence -> on stocke 0 ou 1: 1 si incohérence locale, 0 sinon
 */
static SXBOOLEAN
check_fs_id_coherence (fs_id, incoherence_locale)
     int     fs_id;
     SXBOOLEAN *incoherence_locale;
{	  
  int           bot, top, val, cur, field_id, i, sous_cat_id, cur2, top2, nb, cur3, top3, ref, pred_val; 
  int           *dpv_ptr, *cur_val_ptr;
  SXBOOLEAN       is_optional, has_pred, incoherence_des_fils = SXFALSE, incoherence_du_fils_courant;
  char          *str;
  unsigned char static_field_kind;
  
#if EBUG
  if (fs_id <= 0 || fs_id >= XH_top (XH_fs_hd))
    sxtrap (ME, "check_fs_id_coherence");
#endif /* EBUG */

  /* memoization globale le 04/08/05 */
  /* fs_id_coherence_hd[ref] : bit de poids faible = incoherence_locale ; bit de poids fort = coherence_globale (!!!) */ 
  if (X_set (&fs_id_coherence_hd, fs_id, &ref)) {
    val = fs_id_coherence2attr [ref];
    *incoherence_locale |= (val & 1);
    return (val & 2);
  }
  else
    fs_id_coherence2attr [ref] = 2; /* coherente + incoherence_locale==SXFALSE */

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);

  if (bot == top) {
    /* vide et donc pas de PRED */
    ////    fs_id_coherence2attr [ref] = 0;
    fs_id_coherence2attr [ref] = 1; /* incohérente + incohérence locale */
    *incoherence_locale = SXTRUE; //// BS
    return SXFALSE;
  }
	
  if ((val = XH_list_elem (XH_fs_hd, top-1)) < 0) {
    /* non standard */
    top--;
  }

  has_pred = SXFALSE;

  if (top > bot) {
    cur = bot;

    while (cur < top) {
      val = XH_list_elem (XH_fs_hd, cur);

      field_id = val & FIELD_AND;

      if (cur == bot && field_id == pred_id) {
	has_pred = SXTRUE;
	pred_val = val;
      }
      else {
	if ((val & OPTIONAL_VAL) == 0) {
	  /* On ne verifie pas les descendants des "=?" */
	  if (val & CONSTRAINT_VAL) {
	    /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
	    ////	  fs_id_coherence2attr [ref] = 0;
	    fs_id_coherence2attr [ref] = 1; /* incohérente + incohérence locale */
	    *incoherence_locale = SXTRUE; //// BS
	    return SXFALSE;
	  }
		  
	  /* On stocke aussi les elements des ensembles de f_structures (qui ne sont pas ds wfs) pour une verif
	     ulterieure */
	  static_field_kind = field_id2kind [field_id];

	  if (static_field_kind & STRUCT_KIND) {
	    val >>= FS_ID_SHIFT;

	    if (val > 0) {
	      if (static_field_kind == STRUCT_KIND) {
		/* val est un fs_id */
		incoherence_du_fils_courant = SXFALSE;
		if (!check_fs_id_coherence (val, &incoherence_du_fils_courant)) {
		  if (incoherence_du_fils_courant) {
		    /* màj de incohérence locale */
		    fs_id_coherence2attr [ref] = 1;
		    *incoherence_locale = SXTRUE;
		  } else
		    fs_id_coherence2attr [ref] &= 1; /* coherence (pas locale) = 0: on va rendre SXFALSE */
		  return SXFALSE;
		}
		incoherence_des_fils = incoherence_des_fils || incoherence_du_fils_courant;
	      }
	      else {
		/* val est un ufs_id */
		cur2 = XH_X (XH_ufs_hd, val);
		top2 = XH_X (XH_ufs_hd, val+1);
		      
		if (XH_list_elem (XH_ufs_hd, top2-1) < 0)
		  top2--;
			 
		while (cur2 < top2) {
		  if (!check_fs_id_coherence (XH_list_elem (XH_ufs_hd, cur2), &incoherence_du_fils_courant)) {
		    if (incoherence_du_fils_courant) {
		      /* màj de incohérence locale */
		      fs_id_coherence2attr [ref] = 1;
		      *incoherence_locale = SXTRUE;
		    } else
		      fs_id_coherence2attr [ref] &= 1; /* coherence (pas locale) = 0: on va rendre SXFALSE */
		    return SXFALSE;
		  }
		  incoherence_des_fils = incoherence_des_fils || incoherence_du_fils_courant;
		  
		  cur2++;
		}
	      }
	    }
	  }
	}
      }

      cur++;
    }
  }

  /* On verifie la coherence :
     s'il existe un arg local, et s'il existe un champ pred alors l'arg local doit avoir ete defini ds pred */
  /* BS: ... sauf si cet arg local n'est qu'un =? !!! */
  /* A cause de la gestion des SXBA sur les appels recursifs, on reparcourt le tout pour calculer les arguments */

  if (has_pred) {
    if ((pred_val & OPTIONAL_VAL) == 0) {
      val = get_fs_id_atom_val (XH_list_elem (XH_fs_hd, bot));
      dpv_ptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)));

      /*
	#ifdef EASY
      */
      str = Tpq2str (dpv_ptr [4]);

      if (str && ((strlen (str) == 3 && strncmp (str, "coo", 3) == 0 ||
		   strlen (str) == 6 && strncmp (str, "ponctw", 6) == 0))) {
	/* Pas d'echec de la coherence locale ds ce cas la !! */
      }
      else
	/*
	  #endif /* EASY
	*/
	{
	  /* Traitement du champ pred qui existe forcement */
	  sxba_empty (pred_arg_set);

	  for (i = 1; i <= 2; i++) {
	    sous_cat_id = (i == 1) ? dpv_ptr [1] /* sous_cat1 */ : dpv_ptr [2] /* sous_cat2 */;

	    if (sous_cat_id) {
	      for (cur2 = sous_cat [sous_cat_id], top2 = sous_cat [sous_cat_id+1];
		   cur2 < top2;
		   cur2++) {
		field_id = sous_cat_list [cur2];
	
		if (field_id < 0) {
		  field_id = -field_id;
		  is_optional = SXTRUE;
		}
		else
		  is_optional = SXFALSE;

		if (field_id > MAX_FIELD_ID) {
		  field_id -= MAX_FIELD_ID;
		  nb = 0;

		  for (cur3 = compound_field [field_id], top3 = compound_field [field_id+1];
		       cur3 < top3;
		       cur3++) {
		    field_id = compound_field_list [cur3];
		    SXBA_1_bit (pred_arg_set, field_id);

		    if (fs_is_set (fs_id, field_id, &cur_val_ptr) /* Ce champ existe ... */
			&& ((*cur_val_ptr)>>FS_ID_SHIFT) != 0 /* ... et il est non vide */)
		      nb++;
		  }

		  /* is_optional => nb == 0 || nb == 1 */
		  /* !is_optional => nb == 1 */
		  if (nb > 1 || nb == 0 && !is_optional) {
		    *incoherence_locale = SXTRUE;
		    ////		  fs_id_coherence2attr [ref] = 3;
		    fs_id_coherence2attr [ref] = 1;
		    return SXFALSE;
		  }
		}
		else {
		  SXBA_1_bit (pred_arg_set, field_id);

		  /* is_optional => nb == 0 || nb == 1 */
		  /* !is_optional => nb == 1 */
		  if (!is_optional) {
		    if (!fs_is_set (fs_id, field_id, &cur_val_ptr) /* non utilise' ... */
			|| ((*cur_val_ptr)>>FS_ID_SHIFT) == 0 /* ... ou vide */) {
		      *incoherence_locale = SXTRUE;
		      ////		    fs_id_coherence2attr [ref] = 3;
		      fs_id_coherence2attr [ref] = 1;
		      return SXFALSE;
		    }
		  }
		}
	      }
	    }
	  }

	  cur = bot;

	  while (++cur < top) {
	    val = XH_list_elem (XH_fs_hd, cur);

	    if ((val & OPTIONAL_VAL) == 0) { /* BS: cf mon précédent commentaire */
	      field_id = val & FIELD_AND;
	      
	      if (SXBA_bit_is_set (argument_set, field_id)) {
		/* ... c'est un argument ... */
		if (!SXBA_bit_is_set (pred_arg_set, field_id)) {
		  /* ... non prevu ds pred */
		  //// fs_id_coherence2attr [ref] = 0;
		  fs_id_coherence2attr [ref] = 1;
		  return SXFALSE;
		}
	      }
	    }
	  }
	}
    }
    /* else, y'a un pred mais pas de verif !! */
  }

  if (!has_pred || incoherence_des_fils) {
    fs_id_coherence2attr [ref] = 3;
    *incoherence_locale = SXTRUE;
    return SXTRUE;
  }

  return SXTRUE;
}


#ifndef ESSAI
static int
get_wfs_atom_val (val, is_shared)
     int     val;
     SXBOOLEAN *is_shared;
{
  int                     top, bot;
  struct adjunct_set_item *adjunct_ptr;

  if (val > 0) {
    /* C'est un  indice ds adjunct_sets */
    val = get_adjunct_equiv (val);

    /* Il se peut que que pred_nb ne soit pas calcule' => has_multiple_in_wfs_id retourne SXFALSE 
       mais ds ces cas, on se moque de cette valeur */
    *is_shared = has_multiple_in_wfs_id (val); /* Le 01/06/05 */

    adjunct_ptr = adjunct_sets + field_val2adjunct_id [val].adjunct_set_id;
    
    switch (adjunct_ptr->kind) {
    case ADJUNCT_KIND_EMPTY :
      val = 0;
      break;

    case ADJUNCT_KIND_UFS :
      top = XH_X (XH_ufs_hd, adjunct_ptr->id+1);
      bot = XH_X (XH_ufs_hd, adjunct_ptr->id);

      if (bot == top)
	val = 0;
      else {
	if (XH_list_elem (XH_ufs_hd, top-1) < 0)
	  top--;

	val = (bot == top) ? /* En fait, c'est vide */ 0 : XH_list_elem (XH_ufs_hd, bot);
      }

      break;

    case ADJUNCT_KIND_ATOM :
      val = adjunct_ptr->id;

      break;

    case ADJUNCT_KIND_FS :
    case ADJUNCT_KIND_WFS:
#if ATOM_Aij || ATOM_Pij
    case ADJUNCT_KIND_Aij :
#endif /* ATOM_Aij || ATOM_Pij */
      sxtrap (ME, "get_wfs_atom_val");
    }
  }
  else
    *is_shared = SXFALSE;

  return val;
}

/* retourne la valeur d'un champ field = (val, field_id)
   c'est la valeur si champ a valeur unique
   ou l'[u]fs_id si c'est un champ a valeurs multiples */
int
get_fs_id_atom_val (int field)
{
  int             val, bot;
  unsigned char   static_field_kind;

  val = field >> FS_ID_SHIFT;

  if (val > 0) {
    static_field_kind = field_id2kind [field & FIELD_AND];

    if ((static_field_kind & STRUCT_KIND) == 0
#if ATOM_Aij || ATOM_Pij
	&& (static_field_kind & Aij_KIND) == 0
#endif /* ATOM_Aij || ATOM_Pij */
	) {
      /* Ici val est un ufs_id a valeur unique */
      bot = XH_X (XH_ufs_hd, val);
      val = (bot == XH_X (XH_ufs_hd, val+1)) ? /* vide */ 0 : XH_list_elem (XH_ufs_hd, bot);
    }
    else
      /* val est un fs_id ou un ufs_id multiple */
      val = 0;
  }

  return val;
}


static int
get_ufs_atom_val (val, is_shared)
     int     val;
     SXBOOLEAN *is_shared;
{
  int bot, top, top_val;

  *is_shared = SXFALSE;

  if (val > 0) {
    /* C'est un ufs_id */
    top = XH_X (XH_ufs_hd, val+1);
    bot = XH_X (XH_ufs_hd, val);

    if (bot == top)
      /* vide !! */
      val = 0;
    else {
      if ((val = XH_list_elem (XH_ufs_hd, top-1)) < 0) {
	/* shared */
	top--;
	*is_shared = SXTRUE;
	val = (bot == top) ? 0 : XH_list_elem (XH_ufs_hd, bot);
      }
    }
  }

  return val;
}

/* field_val est une valeur de champ d'un wfs_id */
static int
get_wfs_id_atom_val (field_val)
     int field_val;
{
  int                     atom_val;
  struct adjunct_set_item *adjunct_ptr;
  SXBOOLEAN                 is_shared;

  adjunct_ptr = adjunct_sets + field_val2adjunct_id [field_val >> FS_ID_SHIFT].adjunct_set_id;
  atom_val = adjunct_ptr->id;

  if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
    /* On "recupere" la valeur */
#if EBUG
    if (atom_val == 0)
      sxtrap (ME, "get_wfs_id_atom_val");
#endif /* EBUG */

    atom_val = get_ufs_atom_val (atom_val, &is_shared);
    /* On peut avoir atom_val == 0 */
  }
  else {
    if (adjunct_ptr->kind != ADJUNCT_KIND_ATOM)
      /* adjunct_ptr->kind == ADJUNCT_KIND_EMPTY */
      atom_val = 0;
  }
}
#else /* ESSAI */
static int
get_wfs_pred_val (int pred_val, SXBOOLEAN *is_shared)
{
  int                     bot, top, next;
  struct adjunct_set_item *adjunct_ptr;

  if (pred_val > 0) {
    /* C'est un  indice ds adjunct_sets */
    pred_val = get_adjunct_equiv (pred_val);

    /* Il se peut que que pred_nb ne soit pas calcule' => has_multiple_in_wfs_id retourne SXFALSE 
       mais ds ces cas, on se moque de cette valeur */
    *is_shared = has_multiple_in_wfs_id (pred_val); /* Le 01/06/05 */

    next = field_val2adjunct_id [pred_val].adjunct_set_id;
    adjunct_ptr = adjunct_sets + next;

#if EBUG
    if (adjunct_ptr->next != 0)
      sxtrap (ME, "get_wfs_pred_val");
#endif /* EBUG */

    pred_val = 0;

    switch (adjunct_ptr->kind) {
    case ADJUNCT_KIND_EMPTY :
      break;

    case ADJUNCT_KIND_UFS :
      top = XH_X (XH_ufs_hd, adjunct_ptr->id+1);
      bot = XH_X (XH_ufs_hd, adjunct_ptr->id);

      if (top > bot) {
	if (XH_list_elem (XH_ufs_hd, top-1) < 0)
	  top--;

	if (top == bot+1) {
	  pred_val = XH_list_elem (XH_ufs_hd, bot);
	}
      }

      break;

    case ADJUNCT_KIND_ATOM :
      pred_val = adjunct_ptr->id;

      break;

    case ADJUNCT_KIND_FS :
    case ADJUNCT_KIND_WFS:
#if ATOM_Aij || ATOM_Pij
    case ADJUNCT_KIND_Aij :
#endif /* ATOM_Aij || ATOM_Pij */
      sxtrap (ME, "get_wfs_pred_val");
    }
  }
  else
    *is_shared = SXFALSE;

  return pred_val;
}

static int
get_wfs_atom_val (int val, SXBOOLEAN *is_shared, SXBA local_atom_id_set)
{
  int                     top, bot, next;
  struct adjunct_set_item *adjunct_ptr;
  SXBOOLEAN                 ret_val = SXFALSE;

  if (val > 0) {
    /* C'est un  indice ds adjunct_sets */
    val = get_adjunct_equiv (val);

    /* Il se peut que que pred_nb ne soit pas calcule' => has_multiple_in_wfs_id retourne SXFALSE 
       mais ds ces cas, on se moque de cette valeur */
    *is_shared = has_multiple_in_wfs_id (val); /* Le 01/06/05 */
    next = field_val2adjunct_id [val].adjunct_set_id;

    while (next) {
      adjunct_ptr = adjunct_sets + next;
      next = adjunct_ptr->next;
    
      switch (adjunct_ptr->kind) {
      case ADJUNCT_KIND_EMPTY :
	val = 0;
	break;

      case ADJUNCT_KIND_UFS :
	top = XH_X (XH_ufs_hd, adjunct_ptr->id+1);
	bot = XH_X (XH_ufs_hd, adjunct_ptr->id);

	if (top > bot) {
	  if (XH_list_elem (XH_ufs_hd, top-1) < 0)
	    top--;

	  if (top > bot) {
	    ret_val = SXTRUE;

	    do {
	      val = XH_list_elem (XH_ufs_hd, bot);
	      SXBA_1_bit (local_atom_id_set, val);
	      bot++;
	    } while (bot < top);
	  }
	}

	break;

      case ADJUNCT_KIND_ATOM :
	ret_val = SXTRUE;
	val = adjunct_ptr->id;
	SXBA_1_bit (local_atom_id_set, val);

	break;

      case ADJUNCT_KIND_FS :
      case ADJUNCT_KIND_WFS:
#if ATOM_Aij || ATOM_Pij
      case ADJUNCT_KIND_Aij :
#endif /* ATOM_Aij || ATOM_Pij */
	sxtrap (ME, "get_wfs_atom_val");
      }
    }
  }
  else
    *is_shared = SXFALSE;

  return ret_val ? sxba_scan (local_atom_id_set, 0) : 0;
}

/* retourne la valeur d'un champ field = (val, field_id)
   c'est la valeur si champ a valeur unique
   ou l'[u]fs_id si c'est un champ a valeurs multiples */
int
get_fs_id_atom_val (int field)
{
  int             val, bot, top;
  unsigned char   static_field_kind;

  val = field >> FS_ID_SHIFT;

  if (val > 0) {
    static_field_kind = field_id2kind [field & FIELD_AND];

    if ((static_field_kind & STRUCT_KIND) == 0
#if ATOM_Aij || ATOM_Pij
	&& (static_field_kind & Aij_KIND) == 0
#endif /* ATOM_Aij || ATOM_Pij */
	) {
      /* Ici val est un ufs_id a valeur unique */
      bot = XH_X (XH_ufs_hd, val);
      top = XH_X (XH_ufs_hd, val+1);

      if (top > bot && XH_list_elem (XH_ufs_hd, top-1) < 0)
	top--;

      if (top > bot) {
#if EBUG
	if (top > bot+1)
	  /* valeur multiple (disjonction non triviale) */
	  sxtrap (ME, "get_fs_id_atom_val");
#endif /* EBUG */

	val = XH_list_elem (XH_ufs_hd, bot);
      }
      else
	val = 0;
    }
  }

  return val;
}

/* Le 06/11/06 le resultat de get_[ufs|wfs_id]_atom_val est un SXBA de local_atom_id */
/* Retourne SXTRUE si on y a mis qqchose */
int
get_ufs_atom_val (int val, SXBOOLEAN *is_shared, SXBA local_atom_id_set)
{
  int bot, top, val2;

#if EBUG
  if (local_atom_id_set && !sxba_is_empty (local_atom_id_set))
    sxtrap (ME, "get_ufs_atom_val (local_atom_id_set is not empty)");
#endif /* EBUG */

  *is_shared = SXFALSE;

  if (val > 0) {
    /* C'est un ufs_id */
    top = XH_X (XH_ufs_hd, val+1);
    bot = XH_X (XH_ufs_hd, val);

    if (bot == top)
      /* vide !! */
      val = 0;
    else {
      if (XH_list_elem (XH_ufs_hd, top-1) < 0) {
	/* shared */
	top--;
	*is_shared = SXTRUE;
      }

      if (bot == top) {
	val = 0;
      }
      else {
	val = XH_list_elem (XH_ufs_hd, bot);

	if (local_atom_id_set) {
	  /* On les note tous */
	  SXBA_1_bit (local_atom_id_set, val);
	  bot++;

	  while (bot < top) {
	    val2 = XH_list_elem (XH_ufs_hd, bot);
	    SXBA_1_bit (local_atom_id_set, val2);
	    bot++;
	  }
	}
      }
    }
  }

  return val; /* 0 ou le 1er */
}

/* field_val est une valeur de champ d'un wfs_id */
static int
get_wfs_id_atom_val (int field_val, SXBA local_atom_id_set)
{
  int                     atom_val, next;
  struct adjunct_set_item *adjunct_ptr;
  SXBOOLEAN                 is_shared;

  next = field_val2adjunct_id [field_val >> FS_ID_SHIFT].adjunct_set_id;

  while (next) {
    adjunct_ptr = adjunct_sets + next;
    atom_val = adjunct_ptr->id;
    next = adjunct_ptr->next;

    if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
      /* On "recupere" la valeur */
#if EBUG
      if (atom_val == 0)
	sxtrap (ME, "get_wfs_id_atom_val");
#endif /* EBUG */

      atom_val = get_ufs_atom_val (atom_val, &is_shared, local_atom_id_set);
    }
    else {
      if (adjunct_ptr->kind == ADJUNCT_KIND_ATOM) {
	SXBA_1_bit (local_atom_id_set, atom_val);
      }
    }
  }

  atom_val = sxba_scan (local_atom_id_set, 0);
  
  if (atom_val < 0)
    atom_val = 0;

  return atom_val;
}
#endif /* ESSAI */



/* On verifie la coherence de la wfs:
   Si une structure a des arguments locaux et si elle a un pred alors ces arguments doivent
   etre definis ds pred */
/* on verifie aussi l'optionnalite */ 
/* Si is_root, on verifie de plus que cur_wfs_id a un pred et qu'il ne reste pas de contrainte non verifiee
   Cette verif est faite aussi sur les ensembles de structures.  Il faut cependant attendre d'etre sur une main
   car bien qu'on ne puisse pas "toucher" une f_structure element d'un ensemble, cet ensemble peut tres bien
   ne pas faire partie du resultat final a la racine
*/
static SXBOOLEAN
coherence (Xpq, is_root)
     int     Xpq;
     SXBOOLEAN is_root;
{
  int                     cur_wfs_id, local_wfs_id, page_nb, delta_nb, i, sous_cat_id, cur2, top2, X, field_val;
  int                     cur3, bot3, top3, field_id, nb, val, fs_id, top, bot, cur, ufs_id, id, field_id_path;
  int                     *attr_ptr0, *dpv_ptr, *cur_val_ptr;
  SXBA                    field_set;
  SXBOOLEAN                 is_optional, ret_val, is_full_check, is_shared;
  unsigned char           static_field_kind;
  struct adjunct_set_item *adjunct_ptr;
  char                    *str;

#if EBUG
  if (wfs_id_stack2 [0] != 0)
    sxtrap (ME, "coherence");
#endif /* EBUG */

  is_locally_unconsistent = SXFALSE;

  if (is_relaxed_run)
    return SXTRUE;

  /* Le 20/06/05 on peut specifier ds complete_id_set la liste des non-terminaux qui doivent etre
     completes et coherents */
  /* Le 27/07/06 is_root est passe' en param */

  ret_val = SXTRUE;

  /* Bidouille si MAIN_WFS_ID > 1 !! */
  if (MAIN_WFS_ID > 1) {
    for (cur_wfs_id = 1; cur_wfs_id <= MAIN_WFS_ID; cur_wfs_id++)
      PUSH (wfs_id_stack2, 0);
  }

  /* Attention on suppose que wfs_id ne change pas ds coherence (aucune nelle instanciation) */
  wfs_id_stack2 [MAIN_WFS_ID] = MAIN_WFS_ID; /* pourra etre mis a -cur_wfs_id */
  SXBA_1_bit (wfs_id_set, MAIN_WFS_ID);

  for (cur_wfs_id = MAIN_WFS_ID+1; cur_wfs_id <= wfs_id; cur_wfs_id++) {
    if (cur_wfs_id == wfs_id2equiv [cur_wfs_id]) {
      wfs_id_stack2 [cur_wfs_id] = 0; /* pourra etre mis a -cur_wfs_id */
    }
  }

#if 0
  for (cur_wfs_id = MAIN_WFS_ID; cur_wfs_id <= wfs_id; cur_wfs_id++) {
    if (cur_wfs_id == wfs_id2equiv [cur_wfs_id]) {
      PUSH (wfs_id_stack2, cur_wfs_id);
    }
    else {
      PUSH (wfs_id_stack2, 0); /* Pour acces direct */
    }
  }
#endif /* 0 */

  /* On les verifie en ordre inverse => on est su^r que wfs_id_stack2 ne va pas deborder */
  /* les negatifs sont soumis au full check */
  /* Le 21/11/06 utilisation de wfs_id_set */
  cur_wfs_id = 0;

  while ((cur_wfs_id = sxba_scan_reset (wfs_id_set, cur_wfs_id)) > 0) {
    if (wfs_id_stack2 [cur_wfs_id] < 0)
      is_full_check = SXTRUE;
    else
      is_full_check = is_root;

    /* On ne touche pas a wfs_id_stack2 [cur_wfs_id] */

    page_nb = AREA_PAGE (cur_wfs_id);
    delta_nb = AREA_DELTA (cur_wfs_id);
    attr_ptr0 = wfs2attr [page_nb] [delta_nb];

    if ((field_id_path = *attr_ptr0) > 0) {
      if (is_full_check) {
	fs_id = path2wfs_id [field_id_path].fs_id_dol;
	/* dol = fs_id & MAXRHS_AND; inutile */
	fs_id >>= MAXRHS_SHIFT;
	/* En fait cur_wfs_id est un fs_id non instancie' qu'il faut verifier */
	/* Si cet fs_id est clos, on peut le verifier (et ses descendants) directement ds fs */
	/* Si fs_id est close (elle ne risque pas d'avoir des sous-structures instanciees
	   et donc eventuellement modifiees par ailleurs, la verif pourra se faire directement ds fs */
	/* La fs_id n'est pas close, on doit l'instancier ... */
	/* ... NON, il n'est pas necessaire de l'instancier, on peut faire les verifs ds fs : 2 cas
	   - aucune sous-structure partagee n'est instanciee => aucune n'a change' et elles peuvent
	   donc se verifier ds fs
	   - si une structure partagee a ete instanciee, elle et ses descendants seront verifies comme
	   etant des wfs_id
	*/
	/* verif ds fs_id */
	if (!check_fs_id_coherence (fs_id, &is_locally_unconsistent)) {
	  ret_val = SXFALSE;
	  break;
	}
      }
    }
    else {
      /* Le 17/02/05 vrai wfs_id, pas -1 ni une reference directe a un fs */
      field_set = wfs_field_set [page_nb] [delta_nb];

      if (SXBA_bit_is_set (field_set, pred_id) && (field_val = attr_ptr0 [pred_id]) != 0 && (field_val & OPTIONAL_VAL) == 0 &&
#ifndef ESSAI 
	  (val = get_wfs_atom_val (field_val >> FS_ID_SHIFT, &is_shared)) != 0
#else /* ESSAI */
	  (val = get_wfs_pred_val (field_val >> FS_ID_SHIFT, &is_shared)) != 0
#endif /* ESSAI */
	  ) {
	sxba_empty (pred_arg_set);
	dpv_ptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)));

	for (i = 1; i <= 2; i++) {
	  sous_cat_id = (i == 1) ? dpv_ptr [1] /* sous_cat1 */ : dpv_ptr [2] /* sous_cat2 */;

	  if (sous_cat_id) {
	    for (cur2 = sous_cat [sous_cat_id], top2 = sous_cat [sous_cat_id+1];
		 cur2 < top2;
		 cur2++) {
	      field_id = sous_cat_list [cur2];
	
	      if (field_id < 0) {
		field_id = -field_id;
		is_optional = SXTRUE;
	      }
	      else
		is_optional = SXFALSE;

	      if (field_id > MAX_FIELD_ID) {
		field_id -= MAX_FIELD_ID;
		nb = 0;

		for (bot3 = cur3 = compound_field [field_id], top3 = compound_field [field_id+1];
		     cur3 < top3;
		     cur3++) {
		  field_id = compound_field_list [cur3];
		  SXBA_1_bit (pred_arg_set, field_id);

		  if (SXBA_bit_is_set (field_set, field_id) && (attr_ptr0 [field_id]>>FS_ID_SHIFT) != 0)
		    nb++;
		}

		/* is_root && is_optional => nb == 0 || nb == 1 */
		/* is_root && !is_optional => nb == 1 */
		/* !is_root && is_optional => nb == 0 || nb == 1 */
		/* !is_root && !is_optional => nb == 0 || nb == 1 */
		if (is_full_check && (nb > 1 || nb == 0 && !is_optional)) {
		  /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
		  is_locally_unconsistent = SXTRUE;
		}
	      }
	      else {
		SXBA_1_bit (pred_arg_set, field_id);
		/* is_root && is_optional => nb == 0 || nb == 1 */
		/* is_root && !is_optional => nb == 1 */
		/* !is_root && is_optional => nb == 0 || nb == 1 */
		/* !is_root && !is_optional => nb == 0 || nb == 1 */

		if (is_full_check && !is_optional) {
		  if (!SXBA_bit_is_set (field_set, field_id) /* non utilise' ... */
		      || (attr_ptr0 [field_id]>>FS_ID_SHIFT) == 0 /* ... ou vide */) {
		    /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
		    is_locally_unconsistent = SXTRUE;
		  }
		}
	      }
	    }
	  }
	}

	/* On verifie la coherence :
	   s'il existe un arg local, et s'il existe un champ pred alors l'arg local doit avoir ete defini ds pred */
	sxba_copy (warg_set, field_set);
	sxba_and (warg_set, argument_set);

	/* begin BS : on ne vérifie pas si un attribut de type "=?" est bien dans le pred, 
	   car il sera viré plus tard si jamais transformé en "=" */
	field_id = 0;
	while ((field_id = sxba_scan (warg_set, field_id)) >= 0) {
	  val = attr_ptr0 [field_id];
	  
	  if (val & OPTIONAL_VAL)
	    SXBA_0_bit (warg_set, field_id);
	}

	/* end BS */
      
	/* warg_set doit etre un sous-ensemble de pred_arg_set */
	if (!sxba_is_subset (warg_set, pred_arg_set)) {
	  /*
	    #ifdef EASY
	  */
	  str = Tpq2str (dpv_ptr [4]);

	  if (str && ((strlen (str) == 3 && strncmp (str, "coo", 3) == 0 ||
		       strlen (str) == 6 && strncmp (str, "ponctw", 6) == 0))) {
	    /* Pas d'echec de la coherence locale ds ce cas la !! */
	  }
	  else
	    /*
	      #endif /* EASY
	    */
	    {
	      ret_val = SXFALSE;
	      break;
	    }
	}
      }
      else {
	if (is_full_check) {
	  /* Il doit y avoir un pred */
	  /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
	  is_locally_unconsistent = SXTRUE;
	}
      }

      if (is_full_check) {
	/* On verifie qu'il ne reste pas de contrainte non verifiee */
	/* Le 01/04/05 on verifie aussi CONSTRAINT_VAL sur pred_id
	field_id = pred_id; */
	field_id = 0;

	while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
	  val = attr_ptr0 [field_id];

	  if ((val & OPTIONAL_VAL) == 0) {
	    /* On ne verifie pas la coherence sur les "=?" */
	    static_field_kind = field_id2kind [field_id];

	    if (val & CONSTRAINT_VAL) {
	      /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
	      is_locally_unconsistent = SXTRUE;
	    }
	    else {
	      /* Si check_adjunct cette verif a deja ete faite la 1ere fois
		 (sauf s'ils contiennent une sous-structure partagee) */
	      /* Ds un 1er temps, on la refait !! */
	      /* On stocke aussi les elements des ensembles de f_structures (qui ne sont pas ds wfs) pour une verif
		 ulterieure */
	      /* Ds le cas check_adjunct tous les adjuncts ont deja ete verifie's */
	      /* les [u]fs l'ont ete lors de constructions precedentes ... */
	      /* ... et les wfs_id le seront par la boucle externe */
	      if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
		/* Verification des adjuncts */
		/* val est un index ds adjunct_sets */
		/* Si check_adjunct, certains de ses elements ont deja pu etre verifies
		   on ne s'en occupe pas pour l'instant, on triera + tard */
		if ((val >>= FS_ID_SHIFT) > 0) {
		  val = get_adjunct_equiv (val);
		  val = field_val2adjunct_id [val].adjunct_set_id;

		  while (val) {
		    adjunct_ptr = adjunct_sets + val;
		    id = adjunct_ptr->id;
		    /* dol = adjunct_ptr->dol; */

		    /* Le 14/04/05 les adjuncts de la categorie ADJUNCT_KIND_UFS ont ete traites comme ci_dessous.
		       Ceux qui ne contiennent que des fs_id standards sont conserves sous forme UFS.
		       Ds le cas check_adjunct, ses elements ont deja ete verifies sur le coherence() des fils
		       Ceux qui contiennent des fs_id non standards (partages ou non clos) ont ete instancies.
		       - les fs_id standards donnent des fs_id
		       - les fs_id non standards donnent des wfs_id
		    */

		    if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
		      /* ici les fs_id du ufs_id n'ont pas ete verifies sur les fils, il faut
			 donc le faire */
		      /* id est un ufs_id */
		      /* qui provient de la rhs dol */
		      bot = XH_X (XH_ufs_hd, id);
		      top = XH_X (XH_ufs_hd, id+1);

		      if (bot < top && XH_list_elem (XH_ufs_hd, top-1) < 0)
			top--;
	      
		      for (cur = bot; cur < top; cur++) {
			fs_id = XH_list_elem (XH_ufs_hd, cur);
			/* C'est un fs_id  que l'on verifiera + tard */
			/* Si ce fs_id est non partage' il ne peut pas etre instancie' et ne peut donc
			   pas etre modifie' => la version ds fs est OK */
			/* Si ce fs_id est partage', 2 cas :
			   - Soit (fs_id, dol) est instancie' et il sera verifie' ds la foulee
			   - Soit (fs_id, dol) est non instancie' et il n'a donc pas ete modifie'
			   et il peut donc se verifier ds fs
			*/
			if (!check_fs_id_coherence (fs_id, &is_locally_unconsistent)) {
			  ret_val = SXFALSE;
			  break;
			}
		      }
		    }
		    else {
		      if (adjunct_ptr->kind == ADJUNCT_KIND_FS) {
			/* id est un fs_id */
			/* Il a peut-etre deja ete verifie' sur les fils, tant pis, on recommence !! */
			/* Car cet fs_id peut provenir d'un "$i X* adjunct < $$ Y* adjunct" */
			if (!check_fs_id_coherence (id, &is_locally_unconsistent)) {
			  ret_val = SXFALSE;
			  break;
			}
		      }
		      else {
			if (adjunct_ptr->kind == ADJUNCT_KIND_WFS) {
			  /* id est un wfs_id, il sera verifie par la boucle externe */
			  id = get_equiv (id);

			  /* On va le [re]verifier en full s'il ne l'a pas deja ete ou s'il n'a
			     pas ete encore verifie' */
			  if (wfs_id_stack2 [id] > 0) {
			    wfs_id_stack2 [id] = -id;
			    SXBA_1_bit (wfs_id_set, id);

			    if (id <= cur_wfs_id)
			      /* Pour le sxba_scan_reset englobant */
			      cur_wfs_id = 0;
			  }
			}
		      }
		    }
		  
		    val = adjunct_ptr->next;
		  }
		}
	      }
	      else {
		if (static_field_kind == STRUCT_KIND) {
		  val >>= FS_ID_SHIFT;

		  if (val) {
		    val = get_equiv (val);

		    if (wfs_id_stack2 [val] == 0) {
		      wfs_id_stack2 [val] = val;
		      SXBA_1_bit (wfs_id_set, val);

		      if (val <= cur_wfs_id)
			/* Pour le sxba_scan_reset englobant */
			cur_wfs_id = 0;
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
#if check_adjunct
      else {
	/* Le 6/12/04, on ajoute le test suivant */
	/* Sur les adjunct, on verifie que les nouveaux (les wfs_id) sont coherents
	   (comme s'ils etaient a la racine)
	   dans le cas ou aucune de leur sous-structure n'est partagee */
	/* Justification : si on prend la peine de les ranger, c'est qu'on en a besoin
	   (a la racine), or (sauf si partagee)
	   elle ne sera pas modifiee (on ne peut pas la designer), elle doit donc etre
	   totalement coherente ici !! */
	/* Ici is_root == SXFALSE */
	field_id = pred_id;

	while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
	  /* On stocke aussi les elements des ensembles de f_structures (qui ne sont pas ds wfs) pour une verif
	     ulterieure */
	  val = attr_ptr0 [field_id];

	  if ((val & OPTIONAL_VAL) == 0) {
	    /* On ne verifie pas la coherence sur les "=?" */
	    static_field_kind = field_id2kind [field_id];

	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	      val >>= FS_ID_SHIFT;

	      if (val > 0) {
		val = get_adjunct_equiv (val);
		val = field_val2adjunct_id [val].adjunct_set_id;

		while (val) {
		  adjunct_ptr = adjunct_sets + val;
		  val = adjunct_ptr->next;
		  id = adjunct_ptr->id;

		  if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
		    /* id est un ufs_id ... */
		    /* ... ses elements verifiables l'ont deja ete */
		  }
		  else {
		    if (adjunct_ptr->kind == ADJUNCT_KIND_FS) {
		      /* id est un fs_id ... */
		      /* ... que l'on verifie */
		      if (!check_fs_id_coherence (id, &is_locally_unconsistent)) {
			ret_val = SXFALSE;
			break;
		      }
		    }
		    else {
		      if (adjunct_ptr->kind == ADJUNCT_KIND_WFS) {
			/* id est un wfs_id => a verifier + tard */
			id = get_equiv (id);

			/* On va le [re]verifier en full s'il ne l'a pas deja ete ou s'il n'a
			   pas ete encore verifie' */
			if (wfs_id_stack2 [id] > 0) {
			  wfs_id_stack2 [id] = -id;
			  SXBA_1_bit (wfs_id_set, id);

			  if (id <= cur_wfs_id)
			    /* Pour le sxba_scan_reset englobant */
			    cur_wfs_id = 0;
			}
		      }
		    }
		  }
		}
	      }
	    }
	    else {
	      if (static_field_kind == STRUCT_KIND) {
		val >>= FS_ID_SHIFT;

		if (val) {
		  val = get_equiv (val);

		  if (wfs_id_stack2 [val] == 0) {
		    wfs_id_stack2 [val] = val;
		    SXBA_1_bit (wfs_id_set, val);

		    if (val <= cur_wfs_id)
		      /* Pour le sxba_scan_reset englobant */
		      cur_wfs_id = 0;
		  }
		}
	      }
	    }
	  }
	}
      }
#endif /* check_adjunct */
    }
  }


#if 0
  while (!IS_EMPTY (wfs_id_stack2)) {
    if ((cur_wfs_id = POP (wfs_id_stack2)) == 0)
      continue;

    if (cur_wfs_id < 0) {
      cur_wfs_id = -cur_wfs_id;
      is_full_check = SXTRUE;
    }
    else
      is_full_check = is_root;

    page_nb = AREA_PAGE (cur_wfs_id);
    delta_nb = AREA_DELTA (cur_wfs_id);
    attr_ptr0 = wfs2attr [page_nb] [delta_nb];

    if ((field_id_path = *attr_ptr0) > 0) {
      if (is_full_check) {
	fs_id = path2wfs_id [field_id_path].fs_id_dol;
	/* dol = fs_id & MAXRHS_AND; inutile */
	fs_id >>= MAXRHS_SHIFT;
	/* En fait cur_wfs_id est un fs_id non instancie' qu'il faut verifier */
	/* Si cet fs_id est clos, on peut le verifier (et ses descendants) directement ds fs */
	/* Si fs_id est close (elle ne risque pas d'avoir des sous-structures instanciees
	   et donc eventuellement modifiees par ailleurs, la verif pourra se faire directement ds fs */
	/* La fs_id n'est pas close, on doit l'instancier ... */
	/* ... NON, il n'est pas necessaire de l'instancier, on peut faire les verifs ds fs : 2 cas
	   - aucune sous-structure partagee n'est instanciee => aucune n'a change' et elles peuvent
	   donc se verifier ds fs
	   - si une structure partagee a ete instanciee, elle et ses descendants seront verifies comme
	   etant des wfs_id
	*/
	/* verif ds fs_id */
	if (!check_fs_id_coherence (fs_id, &is_locally_unconsistent)) {
	  ret_val = SXFALSE;
	  break;
	}
      }
    }
    else {
      /* Le 17/02/05 vrai wfs_id, pas -1 ni une reference directe a un fs */
      field_set = wfs_field_set [page_nb] [delta_nb];

      if (SXBA_bit_is_set (field_set, pred_id) &&
#ifndef ESSAI 
	  (val = get_wfs_atom_val (attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)) != 0
#else /* ESSAI */
	  (val = get_wfs_pred_val (attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)) != 0
#endif /* ESSAI */
	  ) {
	sxba_empty (pred_arg_set);
	dpv_ptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)));

	for (i = 1; i <= 2; i++) {
	  sous_cat_id = (i == 1) ? dpv_ptr [1] /* sous_cat1 */ : dpv_ptr [2] /* sous_cat2 */;

	  if (sous_cat_id) {
	    for (cur2 = sous_cat [sous_cat_id], top2 = sous_cat [sous_cat_id+1];
		 cur2 < top2;
		 cur2++) {
	      field_id = sous_cat_list [cur2];
	
	      if (field_id < 0) {
		field_id = -field_id;
		is_optional = SXTRUE;
	      }
	      else
		is_optional = SXFALSE;

	      if (field_id > MAX_FIELD_ID) {
		field_id -= MAX_FIELD_ID;
		nb = 0;

		for (bot3 = cur3 = compound_field [field_id], top3 = compound_field [field_id+1];
		     cur3 < top3;
		     cur3++) {
		  field_id = compound_field_list [cur3];
		  SXBA_1_bit (pred_arg_set, field_id);

		  if (SXBA_bit_is_set (field_set, field_id) && (attr_ptr0 [field_id]>>FS_ID_SHIFT) != 0)
		    nb++;
		}

		/* is_root && is_optional => nb == 0 || nb == 1 */
		/* is_root && !is_optional => nb == 1 */
		/* !is_root && is_optional => nb == 0 || nb == 1 */
		/* !is_root && !is_optional => nb == 0 || nb == 1 */
		if (is_full_check && (nb > 1 || nb == 0 && !is_optional)) {
		  /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
		  is_locally_unconsistent = SXTRUE;
		}
	      }
	      else {
		SXBA_1_bit (pred_arg_set, field_id);
		/* is_root && is_optional => nb == 0 || nb == 1 */
		/* is_root && !is_optional => nb == 1 */
		/* !is_root && is_optional => nb == 0 || nb == 1 */
		/* !is_root && !is_optional => nb == 0 || nb == 1 */

		if (is_full_check && !is_optional) {
		  if (!SXBA_bit_is_set (field_set, field_id) /* non utilise' ... */
		      || (attr_ptr0 [field_id]>>FS_ID_SHIFT) == 0 /* ... ou vide */) {
		    /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
		    is_locally_unconsistent = SXTRUE;
		  }
		}
	      }
	    }
	  }
	}

	/* On verifie la coherence :
	   s'il existe un arg local, et s'il existe un champ pred alors l'arg local doit avoir ete defini ds pred */
	sxba_copy (warg_set, field_set);
	sxba_and (warg_set, argument_set);
      
	/* warg_set doit etre un sous-ensemble de pred_arg_set */
	if (!sxba_is_subset (warg_set, pred_arg_set)) {
	  /*
	    #ifdef EASY
	  */
	  str = Tpq2str (dpv_ptr [4]);

	  if (str && ((strlen (str) == 3 && strncmp (str, "coo", 3) == 0 ||
		       strlen (str) == 6 && strncmp (str, "ponctw", 6) == 0))) {
	    /* Pas d'echec de la coherence locale ds ce cas la !! */
	  }
	  else
	    /*
	      #endif /* EASY
	    */
	    {
	      ret_val = SXFALSE;
	      break;
	    }
	}
      }
      else {
	if (is_full_check) {
	  /* Il doit y avoir un pred */
	  /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
	  is_locally_unconsistent = SXTRUE;
	}
      }

      if (is_full_check) {
	/* On verifie qu'il ne reste pas de contrainte non verifiee */
	/* Le 01/04/05 on verifie aussi CONSTRAINT_VAL sur pred_id
	field_id = pred_id; */
	field_id = 0;

	while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
	  val = attr_ptr0 [field_id];
	  static_field_kind = field_id2kind [field_id];

	  if (val & CONSTRAINT_VAL) {
	    /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
	    is_locally_unconsistent = SXTRUE;
	  }
	  else {
	    /* Si check_adjunct cette verif a deja ete faite la 1ere fois
	       (sauf s'ils contiennent une sous-structure partagee) */
	    /* Ds un 1er temps, on la refait !! */
	    /* On stocke aussi les elements des ensembles de f_structures (qui ne sont pas ds wfs) pour une verif
	       ulterieure */
	    /* Ds le cas check_adjunct tous les adjuncts ont deja ete verifie's */
	    /* les [u]fs l'ont ete lors de constructions precedentes ... */
	    /* ... et les wfs_id le seront par la boucle externe */
	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	      /* Verification des adjuncts */
	      /* val est un index ds adjunct_sets */
	      /* Si check_adjunct, certains de ses elements ont deja pu etre verifies
		 on ne s'en occupe pas pour l'instant, on triera + tard */
	      if ((val >>= FS_ID_SHIFT) > 0) {
		val = get_adjunct_equiv (val);
		val = field_val2adjunct_id [val].adjunct_set_id;

		while (val) {
		  adjunct_ptr = adjunct_sets + val;
		  id = adjunct_ptr->id;
		  /* dol = adjunct_ptr->dol; */

		  /* Le 14/04/05 les adjuncts de la categorie ADJUNCT_KIND_UFS ont ete traites comme ci_dessous.
		     Ceux qui ne contiennent que des fs_id standards sont conserves sous forme UFS.
		     Ds le cas check_adjunct, ses elements ont deja ete verifies sur le coherence() des fils
		     Ceux qui contiennent des fs_id non standards (partages ou non clos) ont ete instancies.
		        - les fs_id standards donnent des fs_id
			- les fs_id non standards donnent des wfs_id
		  */

		  if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
		    /* ici les fs_id du ufs_id n'ont pas ete verifies sur les fils, il faut
		       donc le faire */
		    /* id est un ufs_id */
		    /* qui provient de la rhs dol */
		    bot = XH_X (XH_ufs_hd, id);
		    top = XH_X (XH_ufs_hd, id+1);

		    if (bot < top && XH_list_elem (XH_ufs_hd, top-1) < 0)
		      top--;
	      
		    for (cur = bot; cur < top; cur++) {
		      fs_id = XH_list_elem (XH_ufs_hd, cur);
		      /* C'est un fs_id  que l'on verifiera + tard */
		      /* Si ce fs_id est non partage' il ne peut pas etre instancie' et ne peut donc
			 pas etre modifie' => la version ds fs est OK */
		      /* Si ce fs_id est partage', 2 cas :
			 - Soit (fs_id, dol) est instancie' et il sera verifie' ds la foulee
			 - Soit (fs_id, dol) est non instancie' et il n'a donc pas ete modifie'
			 et il peut donc se verifier ds fs
		      */
		      if (!check_fs_id_coherence (fs_id, &is_locally_unconsistent)) {
			ret_val = SXFALSE;
			break;
		      }
		    }
		  }
		  else {
		    if (adjunct_ptr->kind == ADJUNCT_KIND_FS) {
		      /* id est un fs_id */
		      /* Il a peut-etre deja ete verifie' sur les fils, tant pis, on recommence !! */
		      /* Car cet fs_id peut provenir d'un "$i X* adjunct < $$ Y* adjunct" */
		      if (!check_fs_id_coherence (id, &is_locally_unconsistent)) {
			ret_val = SXFALSE;
			break;
		      }
		    }
		    else {
		      if (adjunct_ptr->kind == ADJUNCT_KIND_WFS) {
			/* id est un wfs_id, il sera verifie par la boucle externe */
			id = get_equiv (id);
			
			if (wfs_id_stack2 [id] > 0)
			  wfs_id_stack2 [id] = -id;
		      }
		    }
		  }
		  
		  val = adjunct_ptr->next;
		}
	      }
	    }
	    /* else if (field_id2kind [field_id] == STRUCT_KIND) => cette struct est
	       obligatoirement ds un wfs_id (eventuellement non instancie') et sera donc
	       traite' par la boucle externe
	    */
	  }
	}
      }
#if check_adjunct
      else {
	/* Le 6/12/04, on ajoute le test suivant */
	/* Sur les adjunct, on verifie que les nouveaux (les wfs_id) sont coherents
	   (comme s'ils etaient a la racine)
	   dans le cas ou aucune de leur sous-structure n'est partagee */
	/* Justification : si on prend la peine de les ranger, c'est qu'on en a besoin
	   (a la racine), or (sauf si partagee)
	   elle ne sera pas modifiee (on ne peut pas la designer), elle doit donc etre
	   totalement coherente ici !! */
	/* Ici is_root == SXFALSE */
	field_id = pred_id;

	while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
	  /* On stocke aussi les elements des ensembles de f_structures (qui ne sont pas ds wfs) pour une verif
	     ulterieure */
	  if (field_id2kind [field_id] == STRUCT_KIND+UNBOUNDED_KIND) {
	    val = attr_ptr0 [field_id] >> FS_ID_SHIFT;

	    if (val > 0) {
	      val = get_adjunct_equiv (val);
	      val = field_val2adjunct_id [val].adjunct_set_id;

	      while (val) {
		adjunct_ptr = adjunct_sets + val;
		val = adjunct_ptr->next;
		id = adjunct_ptr->id;

		if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
		  /* id est un ufs_id ... */
		  /* ... ses elements verifiables l'ont deja ete */
		}
		else {
		  if (adjunct_ptr->kind == ADJUNCT_KIND_FS) {
		    /* id est un fs_id ... */
		    /* ... que l'on verifie */
		    if (!check_fs_id_coherence (id, &is_locally_unconsistent)) {
		      ret_val = SXFALSE;
		      break;
		    }
		  }
		  else {
		    if (adjunct_ptr->kind == ADJUNCT_KIND_WFS) {
		      /* id est un wfs_id => a verifier + tard */
		      id = get_equiv (id);

		      if (id >= cur_wfs_id) {
			/* id a deja ete examine', mais uniquement partiellement */
			/* il est note' en negatif */
			/* il sera reexamine completement + tard */
			PUSH (wfs_id_stack2, -id);
		      }
		      else {
			/* id n'a  pas encore ete examine' */
			/* il est note' en negatif pour un examen complet */
			/* On peut y acceder directement */
			wfs_id_stack2 [id] = -id;
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
#endif /* check_adjunct */
    }
  }
#endif /* 0 */

#if check_adjunct
  if (ret_val && !is_root && is_locally_unconsistent) {
    /* PB sur les adjuncts => echec de la coherence (A VOIR) */
    ret_val = SXFALSE;
  }
#endif /* check_adjunct */

#if LOG
  if (!ret_val) {
    fputs ("Unconsistent\n", stdout);
  }
#endif /* LOG */

  if (!ret_val) {
    wfs_id_stack2 [0] = 0;
    sxba_empty (wfs_id_set);
  }

  return ret_val;
}

static SXBA
fill_cycle (cur_wfs_id)
     int cur_wfs_id;
{
  int  x;
  SXBA cycle_set, son_set, father_set, line;

  line = cyclic_M [cur_wfs_id];

  /* On cherche les noeuds du cycle */
  cycle_set = cyclic_M [0];
  sxba_copy (cycle_set, line);
  sxba_minus (cycle_set, wfs_id_set);
  /* On cherche les fils du cycle et les peres de ces fils qui ne sont pas ds le cycle */
  son_set = wfs_id2sons_set [cur_wfs_id];
  //  sxba_empty (son_set);
  father_set = wfs_id2sons_set [cur_wfs_id];
  //  sxba_empty (father_set);

  x = 0;

  while ((x = sxba_scan (cycle_set, x)) > 0) {
    sxba_or (son_set, wfs_id2sons_set [x]);
    sxba_or (father_set, wfs_id2fathers_set [x]);
  }

  /* On ajoute le cycle */
  sxba_or (son_set, cycle_set);
  sxba_or (father_set, cycle_set);

  /* Tous les noeuds du cycle sont equivalents */
  x = 0;

  while ((x = sxba_scan (cycle_set, x)) > 0) {
    sxba_copy (wfs_id2sons_set [x], son_set);
    sxba_or (wfs_id2fathers_set [x], father_set);
  }
  

  return cycle_set;
}


static int instantiate_ufs_id ();
static int instantiate_fs_id ();
static void instantiate_wfs_id ();
static SXBOOLEAN must_we_instantiate_wfs_id ();

static void
oflw_wfs_id ()
{
  int old_size;

  /* Ca evite les reallocations successives sur des petits changements de taille tout en laissant wfs_id2sons_set
     de taille raisonnable */
  old_size = wfs_id2sons_set_size;
  wfs_id2sons_set_size = wfs_id+1;
  wfs_id2sons_set_size >>= SXSHIFT_AMOUNT;
  wfs_id2sons_set_size++;
  wfs_id2sons_set_size <<= SXSHIFT_AMOUNT;

  wfs_id2sons_set = sxbm_resize (wfs_id2sons_set, old_size, wfs_id2sons_set_size, wfs_id2sons_set_size);
  wfs_id2fathers_set = sxbm_resize (wfs_id2fathers_set, old_size, wfs_id2sons_set_size, wfs_id2sons_set_size);
  wfs_id2pred_set = sxbm_resize (wfs_id2pred_set, old_size, wfs_id2sons_set_size, wfs_id2sons_set_size);

  wfs_id_set = sxba_resize (wfs_id_set, wfs_id2sons_set_size);
  wfs_id_stack = sxrealloc (wfs_id_stack, wfs_id2sons_set_size, sizeof (int));
  wfs_id_stack2 = sxrealloc (wfs_id_stack2, wfs_id2sons_set_size, sizeof (int));
  wfs_id2son_nb = sxrealloc (wfs_id2son_nb, wfs_id2sons_set_size, sizeof (int));
  wfs_id2pred_nb = sxrealloc (wfs_id2pred_nb, wfs_id2sons_set_size, sizeof (int));
}

static void
oflw_field_val ()
{
  int old_line_nb, new_line_nb, new_column_nb;

  old_line_nb = field_val2wfs_id_set_size;
  new_line_nb = get_top_field_val2adjunct_id ();
  
  if (new_line_nb >= old_line_nb || wfs_id >= wfs_id2sons_set_size) {
    if (new_line_nb >= old_line_nb) {
      field_val2wfs_id_set_size = new_line_nb+1;
      field_val2wfs_id_set_size >>= SXSHIFT_AMOUNT;
      field_val2wfs_id_set_size++;
      field_val2wfs_id_set_size <<= SXSHIFT_AMOUNT;
    }

    if (wfs_id >= wfs_id2sons_set_size) {
      new_column_nb = wfs_id+1;
      new_column_nb >>= SXSHIFT_AMOUNT;
      new_column_nb++;
      new_column_nb <<= SXSHIFT_AMOUNT;
    }
    else
      new_column_nb = wfs_id2sons_set_size;

    field_val2wfs_id_set = sxbm_resize (field_val2wfs_id_set, old_line_nb, field_val2wfs_id_set_size, new_column_nb);
  }
}


#if 0
/* devenu inutile le 03/01/07 */
static int
eliminate_optional_attrs_in_ufs (int id);
#endif /* 0 */

static SXBOOLEAN is_fill_fs_on_start_symbol;

/* calcule pour chaque wfs_id l'ensemble de ses predecesseurs et le nombre de ses fils */
/* On regarde aussi s'il est accessible depuis MAIN_WFS_ID */
/* en plus on instancie ce qui doit l'etre et on calcule sons et son_nb */
/* Le 13/11/06 si on est sur une f_structure de l'axiome, on fait aussi disparaitre les champs qui sont unifies par =? */
static void
fill_preds ()
{  
  int                     x, son_nb, page_nb, delta_nb, field_id, val, next, path_id, fs_id_dol, fs_id, bot, top, top_val; 
  int                     *attr_ptr0, *attr_ptr;
  SXBA                    son_set, field_set;
  unsigned char           static_field_kind;
  struct adjunct_set_item *adjunct_ptr;
  SXBOOLEAN                 instantiated, must_we_instantiate_wfs_id_called;

  /* a faire avant oflw_wfs_id () */
  oflw_field_val ();

  if (wfs_id >= wfs_id2sons_set_size) {
    oflw_wfs_id ();
  }

#if EBUG
  if (!sxba_is_empty (wfs_id_set) || wfs_id_stack [0] != 0 || wfs_id_stack2 [0] != 0)
    sxtrap (ME, "fill_preds");
#endif /* EBUG */

  must_we_instantiate_wfs_id_called = SXFALSE;

  PUSH (wfs_id_stack2, MAIN_WFS_ID);
  SXBA_1_bit (wfs_id_set, MAIN_WFS_ID);
  sxba_empty (wfs_id2pred_set [MAIN_WFS_ID]);

  while (!IS_EMPTY (wfs_id_stack2)) {
    x = POP (wfs_id_stack2);

    page_nb = AREA_PAGE (x);
    delta_nb = AREA_DELTA (x);

    attr_ptr0 = wfs2attr [page_nb] [delta_nb];
    field_set = wfs_field_set [page_nb] [delta_nb];

    path_id = *attr_ptr0;

    if (path_id > 0) {
      /* non instancie' */
      instantiated = SXFALSE;

      if (sxba_scan (field_set, 0) > 0 /* non instancie' mais avec des chemins "individuels */) {
	instantiate_wfs_id (x);
	instantiated = SXTRUE;
      }
      else {
	fs_id_dol = path2wfs_id [path_id].fs_id_dol;
	fs_id = fs_id_dol >> MAXRHS_SHIFT;
	bot = XH_X (XH_fs_hd, fs_id);
	top = XH_X (XH_fs_hd, fs_id+1);

	if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0 || is_fill_fs_on_start_symbol) {
	  /* Non standard, on verifie */
	  if (!must_we_instantiate_wfs_id_called) {
	    must_we_instantiate_wfs_id_called = SXTRUE;

	    if (memo_dol_fs_id_set == NULL) {
	      XxY_alloc (&XxY_memo_dol_fs_id, "XxY_memo_dol_fs_id", 30, 1, 0, 0, XxY_memo_dol_fs_id_oflw, NULL);
	      memo_dol_fs_id_set = sxba_calloc (XxY_size (XxY_memo_dol_fs_id)+1);
	      cyclic_dol_fs_id_set = sxba_calloc (XxY_size (XxY_memo_dol_fs_id)+1);
	    }
	    else
	      XxY_clear (&XxY_memo_dol_fs_id);
	  }

	  if (must_we_instantiate_wfs_id (fs_id_dol & MAXRHS_AND, fs_id) || is_fill_fs_on_start_symbol) {
	    instantiate_wfs_id (x);
	    instantiated = SXTRUE;
	  }
	}
      }

      if (instantiated) {
	/* on verifie les debordements locaux eventuels */
	/* a faire avant oflw_wfs_id () */
	oflw_field_val ();

	if (wfs_id >= wfs_id2sons_set_size)
	  oflw_wfs_id ();
      }
    }

    son_set = wfs_id2sons_set [x];
    sxba_empty (son_set);
    sxba_empty (wfs_id2fathers_set [x]);
    son_nb = 0;

    field_id = 0;

    while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
      attr_ptr = attr_ptr0 + field_id;
	
      if (is_fill_fs_on_start_symbol && (*attr_ptr & OPTIONAL_VAL)) {
	/* Le 13/11/06, un =? ds une structure associee a l'axiome, elle disparait */
	SXBA_0_bit (field_set, field_id);
	*attr_ptr = 0;
      }
      else {
	static_field_kind = field_id2kind [field_id];
	val = (*attr_ptr)>>FS_ID_SHIFT;

	if (val) {
	  if (static_field_kind == STRUCT_KIND) {
	    /* val est un wfs_id */
	    val = get_equiv (val);

	    /* val est un fils de x ... */
	    if (SXBA_bit_is_reset_set (son_set, val))
	      son_nb++;

	    if (SXBA_bit_is_reset_set (wfs_id_set, val)) {
	      sxba_empty (wfs_id2pred_set [val]);
	      PUSH (wfs_id_stack2, val);
	    }

	    /* ... et x est un pere de val */
	    SXBA_1_bit (wfs_id2pred_set [val], x);
	  }
	  else {
	    /* val est un indice ds field_val2adjunct_id */
	    val = get_adjunct_equiv (val);
	    /* x est un predecesseur de val */

	    if (field_val2adjunct_id [val].pred_nb++ == 0)
	      sxba_empty (field_val2wfs_id_set [val]);

	    SXBA_1_bit (field_val2wfs_id_set [val], x);

	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	      next = field_val2adjunct_id [val].adjunct_set_id;

	      /* On ne s'occupe que des wfs_ids */
	      while (next) {
		adjunct_ptr = adjunct_sets + next;
		next = adjunct_ptr->next;

		if (adjunct_ptr->kind == ADJUNCT_KIND_WFS) {
		  val = adjunct_ptr->id;
		  val = get_equiv (val);

		  /* val est un wfs_id */
		  if (val) {
		    /* val est un fils de x ... */
		    if (SXBA_bit_is_reset_set (son_set, val))
		      son_nb++;

		    if (SXBA_bit_is_reset_set (wfs_id_set, val)) {
		      sxba_empty (wfs_id2pred_set [val]);
		      PUSH (wfs_id_stack2, val);
		    }

		    /* ... et x est un pere de val */
		    SXBA_1_bit (wfs_id2pred_set [val], x);
		  }
		}
#if EBUG
		else {
 		  if (is_fill_fs_on_start_symbol)
		    sxtrap(ME,"fill_preds (on the axiom, found an instantiated adjunct field which is not a wfs)");
		}
#endif /* EBUG */
	      }
	    }
	  }
	}
      }
    }

    wfs_id2son_nb [x] = son_nb;

    if (son_nb == 0)
      PUSH (wfs_id_stack, x);
  }

  for (x = MAIN_WFS_ID; x <= wfs_id; x++) {
    if (SXBA_bit_is_set_reset (wfs_id_set, x)) {
      wfs_id2pred_nb [x] = sxba_cardinal (wfs_id2pred_set [x]);
    }
    else
      wfs_id2pred_nb [x] = wfs_id2son_nb [x] = 0;
  }
}


#if 0
/* devenu inutile le 03/01/07 */
static int
eliminate_optional_attrs_in_fs (int id)
{
  int            top, bot, top_val, val, cur, fs_id, new_fs_id, field_id, struct_nb;
  int            *ptr;
  SXBOOLEAN        has_changed;
  unsigned char  static_field_kind;

  /* id est un fs_id */ 
  top = XH_X (XH_fs_hd, id+1);
  bot = XH_X (XH_fs_hd, id);

  if (bot == top) /* fs vide */
    return id;

  top_val = XH_list_elem (XH_fs_hd, top-1);

  if (top_val < 0) {
    top--;
  }

  has_changed = SXFALSE;
  struct_nb=0;

  for (cur = bot;
       cur < top;
       cur++) {
    val = XH_list_elem (XH_fs_hd, cur);

    if (val & OPTIONAL_VAL)
      has_changed = SXTRUE;
    else {
      field_id = val & FIELD_AND;
      static_field_kind = field_id2kind [field_id];

      if (static_field_kind & STRUCT_KIND) {
	fs_id /* c'est un fs_id ou un ufs_id */ = val >> FS_ID_SHIFT;
	
	if (static_field_kind == STRUCT_KIND)
	  new_fs_id = eliminate_optional_attrs_in_fs (fs_id);
	else
	  new_fs_id /* c'est un ufs_id */ = eliminate_optional_attrs_in_ufs (fs_id /* c'est un ufs_id */);

	if (new_fs_id != fs_id) {
	  has_changed = SXTRUE;
	  val = (new_fs_id << FS_ID_SHIFT) + (val & (KIND_AND << MAX_FIELD_ID_SHIFT)) + field_id;
	}

	DPUSH (eoaiu_stack, val);
	struct_nb++;
      }
    }
  }

  if (!has_changed) {
    DTOP (eoaiu_stack) -= struct_nb;
    return id;
  }
    
  /* sinon... id doit changer (new_fs_id != fs_id) */
  ptr = eoaiu_stack + DTOP (eoaiu_stack) - struct_nb;
  for (cur = bot;
       cur < top;
	 cur++) {
    val = XH_list_elem (XH_fs_hd, cur);
    
    if (val & OPTIONAL_VAL) {
    }
    else {
      field_id = val & FIELD_AND;
      static_field_kind = field_id2kind [field_id];

      if (static_field_kind & STRUCT_KIND) {
	val = *++ptr; /* on unshift la pile, mais on n'a pas DUNSHIFT :-( */
      }

      XH_push (XH_fs_hd, val);
    }
  }
  DTOP (eoaiu_stack) -= struct_nb;

  if (top_val < 0)
    XH_push (XH_fs_hd, top_val);

  XH_set (&XH_fs_hd, &id);

  return id;
}


static int
eliminate_optional_attrs_in_ufs (int id)
{
  int       top, bot, top_val, val, cur, fs_id, new_fs_id;
  SXBOOLEAN   has_changed;

  /* id est un ufs_id */ 
  top = XH_X (XH_ufs_hd, id+1);
  bot = XH_X (XH_ufs_hd, id);

  if (bot == top) /* ufs vide */
    return id;

  top_val = XH_list_elem (XH_ufs_hd, top-1);

  if (top_val < 0) {
    top--;
  }

  has_changed = SXFALSE;

  for (cur = bot;
       cur < top;
       cur++) {
    fs_id = XH_list_elem (XH_ufs_hd, cur);
    new_fs_id = eliminate_optional_attrs_in_fs (fs_id);

    if (new_fs_id != fs_id)
      has_changed = SXTRUE;

    DPUSH (eoaiu_stack, new_fs_id);
  }

  if (!has_changed) {
    DTOP (eoaiu_stack) -= top - bot;
    return id;
  }
    
  /* sinon... id doit changer (new_fs_id != fs_id) */
  for (cur = bot;
       cur < top;
	 cur++) {
    new_fs_id = DPOP (eoaiu_stack);
    SXBA_1_bit (fs_id_set, new_fs_id);
  }

  new_fs_id = 0;

  while ((new_fs_id = sxba_scan_reset (fs_id_set, new_fs_id)) > 0) {
    XH_push (XH_ufs_hd, new_fs_id);
  }

  if (top_val < 0)
    XH_push (XH_ufs_hd, top_val);

  XH_set (&XH_ufs_hd, &id);

  return id;
}
#endif /* 0 */



/* Le 31/07/06 */
static SXBOOLEAN
cycle_check_flag (int x, int fs_id)
{
  int     flag, bot, top, pred_nb, y;
  SXBA    wfs_id_pred_set, wfs_id_sons_set;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);

  if (bot == top)
    return SXFALSE;

  flag = X80+X40; /* cyclique */

  /* ds le cas cyclique on ne peut pas utiliser directement wfs_id2pred_nb [x] */
  wfs_id_pred_set = wfs_id2pred_set [x];
  wfs_id_sons_set = wfs_id2sons_set [x];

  y = pred_nb = 0;

  while ((y = sxba_scan (wfs_id_pred_set, y)) > 0) {
    if (y != x && !SXBA_bit_is_set (wfs_id_sons_set, y)) {
      if (++pred_nb > 1) {
	/* partage' */
	flag |= X20;
	break;
      }
    }
  }

  if (!sxba_is_subset (wfs_id2fathers_set [x], wfs_id_sons_set))
    /* non clos */
    flag |= X10;

  return flag == XH_list_elem (XH_fs_hd, top-1);
}

static SXBOOLEAN
are_wfs_id_and_fs_id_equiv (int x, int *attr_ptr0, SXBA field_set, int fs_id, SXBA cycle_set)
{
  int                     cur, bot, top, top_val, val, field_id, field_kind, fs_val, wfs_val, id, cur1, bot1, top1;
  int                     page_nb, delta_nb, local_signature, signXfs_id, cur2, bot2, top2, local_fs_id;
  int                     cyclic_adjunct_couple_nb, cas_bot, cas_cur, cas_top, cas_cur2;
  int                     *ptr, *attr_ptr, *local_attr_ptr0;
  SXBA                    local_field_set;
  unsigned char           static_field_kind;
  SXBOOLEAN                 ret_val, local_ret_val, is_shared, wfs_is_shared, fs_is_shared, wfs_is_cyclic, fs_is_cyclic;
  struct adjunct_set_item *adjunct_ptr;
#ifdef ESSAI
  int                     wfs_ret_val, fs_ret_val;
#endif /* ESSAI */

  ptr = cyclic_wfs_id2fs_id+x;

  if (*ptr == fs_id)
    return SXTRUE;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);

#if EBUG
  top_val = XH_list_elem (XH_fs_hd, top-1);

  if ((top_val & X40) == 0)
    /* Non cyclique */
    sxtrap (ME, "are_wfs_id_and_fs_id_equiv");
#endif /* EBUG */

  top--;

  if (top-bot != sxba_cardinal (field_set))
    /* nb de champs differents */
    return SXFALSE;

  /* 1ere passe */
  cur = bot;
      
  while (cur < top) {
    val = XH_list_elem (XH_fs_hd, cur);
    field_id = val & FIELD_AND;

    if (!SXBA_bit_is_set (field_set, field_id))
      /* Champ ds le fs_id qui n'existe pas ds le wfs_id */
      return SXFALSE;
    
    cur++;
  }

  *ptr = fs_id; /* a priori */
  /* Permet de traiter les cas cycliques (appel avec le meme couple (c, fs_id)) */

  /* 2eme passe */
  ret_val = SXTRUE;
  cur = bot;
      
  while (cur < top) {
    val = XH_list_elem (XH_fs_hd, cur);
    field_id = val & FIELD_AND;

    if (field_id != pred_id) {
      /* OK sur pred */
      val >>= MAX_FIELD_ID_SHIFT;
      field_kind = val & KIND_AND;
      attr_ptr = attr_ptr0 + field_id;

      if (((*attr_ptr) & CONSTRAINT_VAL) != (field_kind & ASSIGN_CONSTRAINT_KIND)) {
	/* Conflit sur le =c */
	ret_val = SXFALSE;
	break;
      }

      if (((*attr_ptr) & OPTIONAL_VAL) != (field_kind & ASSIGN_OPTION_KIND)) {
	/* Conflit sur le =c */
	ret_val = SXFALSE;
	break;
      }

      static_field_kind = field_id2kind [field_id];
      fs_val = val >> STRUCTURE_NAME_shift;
      wfs_val = (*attr_ptr)>>FS_ID_SHIFT;

      if (static_field_kind & ATOM_KIND) {
#if ATOM_Aij || ATOM_Pij
	if (static_field_kind & Aij_KIND) {
	  if (wfs_val && fs_val) {
	    wfs_val = get_adjunct_equiv (wfs_val);

	    wfs_val = field_val2adjunct_id [wfs_val].adjunct_set_id;
	    sxba_empty (Aij_or_Pij_set);

	    while (wfs_val) {
	      adjunct_ptr = adjunct_sets + wfs_val;
	      wfs_val = adjunct_ptr->next;
	      id = adjunct_ptr->id;

	      if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
		/* id est un ufs_id */
		top1 = XH_X (XH_ufs_hd, id+1);
		bot1 = XH_X (XH_ufs_hd, id);

		if (bot1 < top1 && XH_list_elem (XH_ufs_hd, top1-1) < 0)
		  top1--;
	      
		for (cur1 = bot1; cur1 < top1; cur1++) {
		  id = XH_list_elem (XH_ufs_hd, cur1);

		  SXBA_1_bit (Aij_or_Pij_set, id);
		}
	      }
	      else {
		SXBA_1_bit (Aij_or_Pij_set, id);
	      }
	    }

	    bot2 = XH_X (XH_ufs_hd, fs_val);
	    top2 = XH_X (XH_ufs_hd, fs_val+1);

	    for (cur2 = bot2; cur2 < top2; cur2++) {
	      id = XH_list_elem (XH_ufs_hd, cur2);

	      if (id < 0)
		break;

	      if (!SXBA_bit_is_set_reset (Aij_or_Pij_set, id)) {
		ret_val = SXFALSE;
		break;
	      }
	    }

	    if (!ret_val)
	      break;

	    if (!sxba_is_empty (Aij_or_Pij_set)) {
	      ret_val = SXFALSE;
	      break;
	    }
	      
	    is_shared = has_multiple_in_wfs_id (wfs_val);

	    if (is_shared != ((id & (X80+X20)) == (X80+X20))) {
	      ret_val = SXFALSE;
	      break;
	    }
	  }
	  else {
	    if (wfs_val + fs_val) {
	      /* L'un nul et pas l'autre */
	      ret_val = SXFALSE;
	      break;
	    }
	  }
	}
	else
#endif /* ATOM_Aij || ATOM_Pij */
	  {
#ifndef ESSAI
	    if (static_field_kind & UNBOUNDED_KIND) {
	      wfs_val = get_wfs_atom_val (wfs_val, &wfs_is_shared);
	      fs_val = get_ufs_atom_val (fs_val, &fs_is_shared);

	      if ((wfs_is_shared != fs_is_shared) || (fs_val != wfs_val)) {
		ret_val = SXFALSE;
		break;
	      }
	    }
	    else {
	      wfs_val = get_wfs_atom_val (wfs_val, &wfs_is_shared);
	      fs_val = get_ufs_atom_val (fs_val, &fs_is_shared);

	      if ((wfs_is_shared != fs_is_shared) || (fs_val != wfs_val)) {
		ret_val = SXFALSE;
		break;
	      }
	    }
#else /* ESSAI */
	    wfs_ret_val = get_wfs_atom_val (wfs_val, &wfs_is_shared, left_local_atom_id_set);
	    fs_ret_val = get_ufs_atom_val (fs_val, &fs_is_shared, right_local_atom_id_set);

	    if ((wfs_ret_val != fs_ret_val) ||
		(wfs_is_shared != fs_is_shared) ||
		(sxba_first_difference (left_local_atom_id_set, right_local_atom_id_set) >= 0)
		) {
	      ret_val = SXFALSE;
	    }

	    if (wfs_ret_val) sxba_empty (left_local_atom_id_set);
	    if (fs_ret_val) sxba_empty (right_local_atom_id_set);

	    if (!ret_val)
	      break;
#endif /* ESSAI */
	  }
      }
      else {
	/* Pas ATOM */
	if (static_field_kind == STRUCT_KIND) {
	  if (wfs_val == 0 || fs_val == 0) {
	    if (wfs_val + fs_val) {
	      ret_val = SXFALSE;
	      break;
	    }
	  }
	  else {
	    /* wfs_val et fs_val sont non nuls */
	    wfs_val = get_equiv (wfs_val);
	    wfs_is_cyclic = SXBA_bit_is_set (cycle_set, wfs_val);
	    fs_is_cyclic = SXBA_bit_is_set (fs_id_set, fs_val);

	    if (wfs_is_cyclic && (!fs_is_cyclic) || (!wfs_is_cyclic) && fs_is_cyclic) {
	      ret_val = SXFALSE;
	      break;
	    }

	    if ((!fs_is_cyclic) && (!wfs_is_cyclic)) {
	      val = wfs2attr [AREA_PAGE (wfs_val)] [AREA_DELTA (wfs_val)] [0];
	      /* Ici on suppose que val est un fs_id */

#if EBUG
	      if (val < 0)
		  /* implique' ds un cycle !! */
		sxtrap (ME, "are_wfs_id_and_fs_id_equiv");
#endif /* EBUG */

	      if (val != fs_val) {
		ret_val = SXFALSE;
		break;
	      }
	    }
	    else {
	      /* wfs_val et fs_val sont cycliques tous les 2 ... */
	      if (cyclic_wfs_id2fs_id [wfs_val] != fs_val) {
		/* non equivalents */
		if (cyclic_wfs_id2fs_id [wfs_val] == 0 /* Resultat inconnu */
		    && cycle_check_flag (wfs_val, fs_val) /* flags egaux */) {
		  page_nb = AREA_PAGE (wfs_val);
		  delta_nb = AREA_DELTA (wfs_val);
		  local_attr_ptr0 = wfs2attr [page_nb] [delta_nb];
		  local_field_set = wfs_field_set [page_nb] [delta_nb];

		  local_signature = SXBA_bit_is_set (local_field_set, pred_id)
		    ? 
#ifndef ESSAI 
		    get_wfs_atom_val (local_attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)
#else /* ESSAI */
		    get_wfs_pred_val (local_attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)
#endif /* ESSAI */
		    : 0;

		  if (XxY_is_set (&cyclic_signXfs_id_hd, local_signature, fs_val, &signXfs_id))
		    /* ... et ils ont le meme pred */
		    ret_val = are_wfs_id_and_fs_id_equiv (wfs_val, local_attr_ptr0, local_field_set, fs_val, cycle_set);
		  else
		    ret_val = SXFALSE;
		}
		else
		  ret_val = SXFALSE;

		if (!ret_val)
		  break;
	      }
	    }
	  }
	}
	else {
	  if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	    if (wfs_val && fs_val) {
	      bot2 = XH_X (XH_ufs_hd, fs_val);
	      top2 = XH_X (XH_ufs_hd, fs_val+1);
		
	      local_fs_id = XH_list_elem (XH_ufs_hd, top2-1);

	      fs_is_shared = (local_fs_id < 0) && (local_fs_id & X20);

	      wfs_val = get_adjunct_equiv (wfs_val);
	      wfs_is_shared = has_multiple_in_wfs_id (wfs_val);
		
	      if (fs_is_shared && (!wfs_is_shared) || (!fs_is_shared) && wfs_is_shared) {
		ret_val = SXFALSE;
	      }

	      if (ret_val) {
		for (cur2 = bot2; cur2 < top2; cur2++) {
		  local_fs_id = XH_list_elem (XH_ufs_hd, cur2);

		  if (local_fs_id < 0)
		    break;
		
		  if (SXBA_bit_is_reset_set (visited_fs_id_set, local_fs_id))
		    PUSH (visited_fs_id_stack, local_fs_id);
		}

		wfs_val = field_val2adjunct_id [wfs_val].adjunct_set_id;

		cyclic_adjunct_couple_nb = 0;
		cas_bot = DTOP (cyclic_adjunct_stack);

		while (ret_val && wfs_val) {
		  adjunct_ptr = adjunct_sets + wfs_val;
		  wfs_val = adjunct_ptr->next;
		  id = adjunct_ptr->id;
		  /* dol = adjunct_ptr->dol; */

		  if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
		    /* id est un ufs_id */
		    /* qui provient de la rhs dol */
		    top1 = XH_X (XH_ufs_hd, id+1);
		    bot1 = XH_X (XH_ufs_hd, id);

		    if (bot1 < top1 && XH_list_elem (XH_ufs_hd, top1-1) < 0)
		      top1--;
	      
		    for (cur1 = bot1; cur1 < top1; cur1++) {
		      local_fs_id = XH_list_elem (XH_ufs_hd, cur1);

		      if (!SXBA_bit_is_set_reset (visited_fs_id_set, local_fs_id)) {
			ret_val = SXFALSE;
			break;
		      }
		    }
		  }
		  else {
		    if (adjunct_ptr->kind == ADJUNCT_KIND_FS) {
		      /* id est un fs_id */
		      if (!SXBA_bit_is_set_reset (visited_fs_id_set, id))
			ret_val = SXFALSE;
		    }
		    else {
		      if (adjunct_ptr->kind == ADJUNCT_KIND_WFS) {
			/* id est un wfs_id */
			id = get_equiv (id);
			wfs_is_cyclic = SXBA_bit_is_set (cycle_set, id);

			if (!wfs_is_cyclic) {
			  val = wfs2attr [AREA_PAGE (id)] [AREA_DELTA (id)] [0];

#if EBUG
			  if (val < 0)
			    /* implique' ds un cycle !! */
			    sxtrap (ME, "are_wfs_id_and_fs_id_equiv");
#endif /* EBUG */
		    
			  fs_is_cyclic = SXBA_bit_is_set (fs_id_set, val);

			  if (fs_is_cyclic || !SXBA_bit_is_set_reset (visited_fs_id_set, val))
			    ret_val = SXFALSE;
			}
			else {
			  /* id est un wfs_id cyclique */
			  /* Il doit y avoir ds les elements de visited_fs_id_set un fs_id cyclique (il doit donc aussi etre ds
			     fs_id_set) avec lequel il est equivalent */
			  cas_cur = cyclic_adjunct_couple_nb;

			  for (cur2 = bot2; cur2 < top2; cur2++) {
			    local_fs_id = XH_list_elem (XH_ufs_hd, cur2);

			    if (local_fs_id < 0)
			      break;
		
			    if (SXBA_bit_is_set (visited_fs_id_set, local_fs_id)) {
			      /* non encore traite' ... */
			      if (SXBA_bit_is_set (fs_id_set, local_fs_id)) {
				/* ... et cyclique */
				/* candidat */
				/* id et fs_id sont cycliques tous les 2 ... */
				if (cyclic_wfs_id2fs_id [id] != local_fs_id) {
				  /* non equivalents */
				  if (cyclic_wfs_id2fs_id [id] == 0 /* Resultat inconnu */
				      && cycle_check_flag (id, local_fs_id) /* flags identiques */) {
				    page_nb = AREA_PAGE (id);
				    delta_nb = AREA_DELTA (id);
				    local_attr_ptr0 = wfs2attr [page_nb] [delta_nb];
				    local_field_set = wfs_field_set [page_nb] [delta_nb];

				    local_signature = SXBA_bit_is_set (local_field_set, pred_id)
				      ? 
#ifndef ESSAI 
				      get_wfs_atom_val (local_attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)
#else /* ESSAI */
				      get_wfs_pred_val (local_attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)
#endif /* ESSAI */
				      : 0;

				    if (XxY_is_set (&cyclic_signXfs_id_hd, local_signature, local_fs_id, &signXfs_id)) {
				      /* ... et ils ont le meme pred */
				      /* Attention on ne peut pas appeler recursivement are_wfs_id_and_fs_id_equiv d'ici a cause
					 de visited_fs_id_set qui est global */
				      /* On note les couples possibles */
				      DPUSH (cyclic_adjunct_stack, id);
				      DPUSH (cyclic_adjunct_stack, local_fs_id);
				      cyclic_adjunct_couple_nb++;
				    }
				  }
				}
			      }
			    }
			  }

			  /* Ici cyclic_adjunct_stack doit avoir grossi */
			  if (cas_cur == cyclic_adjunct_couple_nb)
			    ret_val = SXFALSE;
			}
		      }
		    }
		  }
		}

		while (!IS_EMPTY (visited_fs_id_stack)) {
		  id = POP (visited_fs_id_stack);
		  SXBA_0_bit (visited_fs_id_set, id);
		}

		if (ret_val) {
		  if (cyclic_adjunct_couple_nb) {
		    /* OK, on peut faire les appels recursifs */
		    cas_top = DTOP (cyclic_adjunct_stack);

		    for (cas_cur = cas_top;
			 cas_cur > cas_bot;
			 cas_cur -= 2) {
		      local_fs_id = cyclic_adjunct_stack [cas_cur];

		      if (local_fs_id) {
			id = cyclic_adjunct_stack [cas_cur-1];
			page_nb = AREA_PAGE (id);
			delta_nb = AREA_DELTA (id);
			local_attr_ptr0 = wfs2attr [page_nb] [delta_nb];
			local_field_set = wfs_field_set [page_nb] [delta_nb];
			local_ret_val = are_wfs_id_and_fs_id_equiv (id, local_attr_ptr0, local_field_set, local_fs_id, cycle_set);

			if (local_ret_val) {
			  /* Il faut enlever les (id, local_fs_id') et les (id', local_fs_id) */
			  for (cas_cur2 = cas_cur-2; cas_cur2 > cas_bot; cas_cur2 -= 2) {
			    if (cyclic_adjunct_stack [cas_cur2] == id || cyclic_adjunct_stack [cas_cur2-1] == local_fs_id)
			      cyclic_adjunct_stack [cas_cur2] = 0;
			  }
			}
			else {
			  /* Si c'est le dernier id => echec */
			  for (cas_cur2 = cas_cur-2; cas_cur2 > cas_bot; cas_cur2 -= 2) {
			    if (cyclic_adjunct_stack [cas_cur2] == id)
			      /* OK */
			      break;
			  }

			  if (cas_cur2 <= cas_bot) {
			    ret_val = SXFALSE;
			    break;
			  }
			}
		      }
		    }
		  }
		}

		/* On recupere la place */
		DTOP (cyclic_adjunct_stack) = cas_bot;
	      }
	    }
	    else {
	      if (wfs_val + fs_val) {
		ret_val = SXFALSE;
		break;
	      }
	    }
	  }
	}
      }
    }
    
    cur++;
  }

  if (!ret_val)
    *ptr = 0;

  return ret_val;
}

/* cycle_set contient des wfs_id et fs_id_set des fs_id en nombre egal.
   On doit trouver une bijection entre les 2.  Elle sera stockee ds cyclic_wfs_id2fs_id */

/* Les elements d'un cycle se trouvent ds cycle_set 
   On regarde si ce cycle n'a pas deja ete trouve', auquel cas on doit lui donner les memes fs_id.
   Pour l'instant on ne recherche que ds les fs_id cycliques trouve's au cours du traitement des
   equations de la production instanciee courante.
*/
static SXBOOLEAN
cycle_already_seen (SXBA cycle_set)
{
  int     x, page_nb, delta_nb, nb, sizeXgrp_id, grp_id, fs_id, signature, signXfs_id; 
  int     *attr_ptr0, *fs_id_cur_ptr, *fs_id_top_ptr, *fs_id_ptr;
  SXBA    field_set;
  SXBOOLEAN ret_val, is_shared;

  if (is_new_Pij_for_cyclic_fs_ids) {
    is_new_Pij_for_cyclic_fs_ids = SXFALSE;

    if (XxY_is_allocated (cyclic_signXfs_id_hd)) {
      XxY_clear (&cyclic_signXfs_id_hd);
      XxY_clear (&cyclic_sizeXgrp_id_hd);
      cyclic_grp_stack [0] = 0;
    }
    else {
      XxY_alloc (&cyclic_signXfs_id_hd, "cyclic_signXfs_id_hd", 20, 1, 1 /* Xforeach */, 0, NULL /* cyclic_signXfs_id_hd_oflw */, NULL);
      XxY_alloc (&cyclic_sizeXgrp_id_hd, "cyclic_sizeXgrp_id_hd", 20, 1, 1 /* Xforeach */, 0, NULL /* cyclic_sizeXgrp_id_hd_oflw */, NULL);
      DALLOC_STACK (cyclic_grp_stack, 200);
      DALLOC_STACK (cyclic_adjunct_stack, 200);
#if ATOM_Aij || ATOM_Pij
      Aij_or_Pij_set = sxba_calloc (spf.outputG.maxxnt+spf.outputG.maxxprod+1);
#endif /* ATOM_Aij || ATOM_Pij */
    }
  }

#if EBUG
  if (DTOP (cyclic_adjunct_stack) != 0)
    sxtrap (ME, "cycle_already_seen");
#endif /* EBUG */

  nb = sxba_cardinal (cycle_set);

  ret_val = SXFALSE;

  XxY_Xforeach (cyclic_sizeXgrp_id_hd, nb, sizeXgrp_id) {
    grp_id = XxY_Y (cyclic_sizeXgrp_id_hd, sizeXgrp_id);

    fs_id_cur_ptr = fs_id_ptr = cyclic_grp_stack + grp_id;
    fs_id_top_ptr = fs_id_ptr + nb;

    while (fs_id_cur_ptr < fs_id_top_ptr) {
      fs_id = *fs_id_cur_ptr;
      SXBA_1_bit (fs_id_set, fs_id);
      fs_id_cur_ptr++;
    }

    /* On prend le 1er wfs_id du cycle comme racine */
    x = sxba_scan (cycle_set, 0);

    /* On prend comme signature d'un [w]fs_id la valeur de son champ pred */
    page_nb = AREA_PAGE (x);
    delta_nb = AREA_DELTA (x);
    attr_ptr0 = wfs2attr [page_nb] [delta_nb];
    field_set = wfs_field_set [page_nb] [delta_nb];

    signature = SXBA_bit_is_set (field_set, pred_id)
      ? 
#ifndef ESSAI 
      get_wfs_atom_val (attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)
#else /* ESSAI */
      get_wfs_pred_val (attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)
#endif /* ESSAI */
      : 0;

    XxY_Xforeach (cyclic_signXfs_id_hd, signature, signXfs_id) {
      fs_id = XxY_Y (cyclic_signXfs_id_hd, signXfs_id);

      if (SXBA_bit_is_set (fs_id_set, fs_id) && cycle_check_flag (x, fs_id)) {
	/* candidat */
	/* PB ce n'est peut etre pas le seul */
	if (are_wfs_id_and_fs_id_equiv (x, attr_ptr0, field_set, fs_id, cycle_set)) {
	  ret_val = SXTRUE;
	  break;
	}
      }
    }

    fs_id_cur_ptr = fs_id_ptr;

    while (fs_id_cur_ptr < fs_id_top_ptr) {
      fs_id = *fs_id_cur_ptr;
      SXBA_0_bit (fs_id_set, fs_id);
      fs_id_cur_ptr++;
    }

    if (ret_val)
      break;
  }

  return ret_val;
}

static SXBOOLEAN cyclic_wfs_are_processed;

/* construit le fs_id associe' au wfs_id x */
/* retourne SXTRUE ssi le fs_id est nouveau */
static SXBOOLEAN
from_wfs_id_to_fs_id (int x, int *fs_id)
{
  int           preds_nb, page_nb, delta_nb, field_id, field_id_path, fs_id_orig, top, bot, top_val, cur, val, flag, predicted_fs_id, id;
  int           *attr_ptr0, *attr_ptr;
  SXBA          wfs_id_sons_set, wfs_id_fathers_set, wfs_id_pred_set, field_set; 
  SXBOOLEAN       is_x_closed, is_new; 
  unsigned char static_field_kind;

  page_nb = AREA_PAGE (x);
  delta_nb = AREA_DELTA (x);

  attr_ptr0 = wfs2attr [page_nb] [delta_nb];

  wfs_id_sons_set = wfs_id2sons_set [x];
  wfs_id_fathers_set = wfs_id2fathers_set [x];
  wfs_id_pred_set = wfs_id2pred_set [x];
  /* x est son propre fils */
  SXBA_1_bit (wfs_id_sons_set, x);

  if (!cyclic_wfs_are_processed && (field_id_path = *attr_ptr0) > 0) {
    fs_id_orig = path2wfs_id [field_id_path].fs_id_dol >> MAXRHS_SHIFT;
    /* Ce x est un wfs_id qui est une copie exacte, non instanciee de fs_id */
    /* right_dol = fs_id & MAXRHS_AND; */
    /* Il y a 2 cas ou` il ne peut etre utilise tel quel :
       -  Il est non partage' ds fs_id mais partage' ds wfs ($$ a = $i a ($i a = Fk); $$ b a = $$ a)
       -  Il est partage' ds fs_id (X80+X20) et non partage' ds wfs
    */

    /* 
       X80         == 1  <=> est partage ou cyclique ou non close
       X40         == 1  <=> cyclique
       X20         == 1  <=> partage
       X10         == 1  <=> non close
    */
    is_x_closed = SXTRUE; /* par construction */

    top = XH_X (XH_fs_hd, fs_id_orig+1)-1;
    bot = XH_X (XH_fs_hd, fs_id_orig);

    if (bot == top)
      /* structure vide !! */
      top_val = 0;
    else
      top_val = XH_list_elem (XH_fs_hd, top) & (X80+X40+X20+X10);

    preds_nb = wfs_id2pred_nb [x];

    if (is_fill_fs_on_start_symbol || (preds_nb > 1 && (top_val == 0)) || (preds_nb <= 1 && (top_val == X80+X20))) {
      /* Changement de categorie */
      for (cur = bot; cur < top; cur++) {
	val = XH_list_elem (XH_fs_hd, cur);
	
	if (is_fill_fs_on_start_symbol && (val & OPTIONAL_VAL)) {
	  /* Le 13/11/06, un =? ds une structure associee a l'axiome, elle disparait */
	}
	else
	  XH_push (XH_fs_hd, val);
      }

      if (preds_nb > 1) {
	/* ... il devient partage' */
	flag = X80+X20;
	XH_push (XH_fs_hd, flag);
      }
      /* else il devient non partage (et clos) */

      /* On fera une recherche dicho ds XH_fs_hd */
      is_new = !XH_set (&XH_fs_hd, fs_id);

      if (is_new) {
	/* nouveau */
	fs_id2dynweight [*fs_id] = fs_id2dynweight [fs_id_orig];
	fs_id2localdynweight [*fs_id] = fs_id2localdynweight [fs_id_orig];
	/* la structure fs_id_orig reçoit un nouveau nom, fs_id, mais ne change pas: on prend son dynweight sans le regarder */
      }
    }
    else
      *fs_id = fs_id_orig;
  }
  else {
    is_x_closed = SXTRUE;

    if (*attr_ptr0 != -1) {
      /* non vide */
      field_set = wfs_field_set [page_nb] [delta_nb];
      field_id = 0;

      while ((field_id = sxba_scan_reset (field_set, field_id)) >= 0) {
	attr_ptr = attr_ptr0 + field_id;
	static_field_kind = field_id2kind [field_id];
	val = (*attr_ptr)>>FS_ID_SHIFT;

	if (val) {
	  /* Finalement, un champ cre'e' et non affecte' n'est pas conserve' */
	  if (static_field_kind == STRUCT_KIND) {
	    /* ... est positionne ds tous les cas, meme cyclique !! */
	    val = get_equiv (val); /* Le 22/03/05 */
	    val = wfs2attr [AREA_PAGE (val)] [AREA_DELTA (val)] [0];

	    if (val < 0)
	      /* val est une valeur predite */
	      val &= ~(X80+X40);

#if 0
	    if (val < 0)
	      /* implique' ds un cycle */
	      /* val est une valeur predite */
	      val = -val;
#endif /* 0 */
	  }
	  else {
	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	      val = uwfs2ufs_id (val, SXFALSE, x, static_field_kind);
	    }
	    else {
#if ATOM_Aij || ATOM_Pij
	      if (static_field_kind & Aij_KIND) {
		/* val reference une liste ds wfs_id_pred_list */
		/* On stocke les ensembles de Aij/Pij ds XH_ufs_hd */
		/* C'est une bonne ref a un ensemble non vide */
		val = uwfs2ufs_id (val, SXTRUE, x, static_field_kind);
		/* val est l'identifiant d'une liste de ste ds XH_ufs_hd */
	      }
	      else
#endif /* ATOM_Aij || ATOM_Pij */

		/* Cas des valeurs atomiques et des pred */
		val = uwfs2ufs_id (val, SXTRUE, x, static_field_kind);
	    }
	  }

	  if (val) {
	    val = (val << FS_ID_SHIFT) + (*attr_ptr & (KIND_AND << MAX_FIELD_ID_SHIFT)) + field_id;
	    XH_push (XH_fs_hd, val);
	  }
	}
      }

      /* Le 04/05/05 le calcul de is_x_closed doit aussi marcher sur les structures cycliques */
      /* Les appels a uwfs2ufs_id () ont complete' wfs_id_fathers_set */
      is_x_closed = sxba_is_subset (wfs_id_fathers_set, wfs_id_sons_set);

#if EBUG
      if (x == MAIN_WFS_ID && !is_x_closed)
	sxtrap (ME, "from_wfs_id_to_fs_id");
#endif /* EBUG */
    }

    /* Traitement du flag des fs_id qui indique si une f_structure est
       cyclique, partagee ou non close */
    /* 
       X80         == 1  <=> est partage ou cyclique ou non close
       X40         == 1  <=> cyclique
       X20         == 1  <=> partage
       X10         == 1  <=> non close
    */
    if (cyclic_wfs_are_processed) {
      predicted_fs_id = *attr_ptr0;
      /* predicted_fs_id peut valoir 0 si from_wfs_id_to_fs_id a ete appele' depuis process_cyclic_wfs */

      if (predicted_fs_id & X40) {
	/* On est sur un cycle */
	flag = X80+X40; /* Le 02/08/06 */
	/* Ds le cas cyclique preds_nb doit etre le nb de predecesseurs hors du cycle = (preds-sons) */
	sxba_minus (wfs_id_pred_set, wfs_id_sons_set);
	preds_nb = sxba_cardinal (wfs_id_pred_set);
      }
      else {
	flag = 0;
	preds_nb = wfs_id2pred_nb [x];
      }

      /* flag = X80+X40-predicted_fs_id; */
    }
    else {
      flag = 0;
      preds_nb = wfs_id2pred_nb [x];
    }

    if (preds_nb > 1)
      flag |= X80+X20;
	  
    if (!is_x_closed)
      flag |= X80+X10;

    if (flag != 0)
      XH_push (XH_fs_hd, flag);

    is_new = !XH_set (&XH_fs_hd, fs_id);

    if (is_new) {
      /* nouveau */
      fs_id2localdynweight [*fs_id] = wfs_id2localdynweight [x];
      fs_id2dynweight [*fs_id] = 0;
      /* structure complètement nouvelle */
    } /* else {
	 La fs_id courante existait déjà, ella donc déjà son local dynweight */
    /* en toute rigueur elle pourrait dépendre du mode de calcul, mais l'attribution d'un fs_id à une structure ignore ce fait,
       et on peut donc l'ignorer aussi ici. C'est grossier, mais ça évite l'augmentation sans fin du dynweight des structures
       cycliques à chaque fois qu'on le calcule
       }  */

#if EBUG
    if (cyclic_wfs_are_processed && (predicted_fs_id & X80)) {
      predicted_fs_id &= ~(X80+X40);
 
      if (*fs_id != predicted_fs_id)
	/* Mauvaise prediction !! */
	sxtrap (ME, "from_wfs_id_to_fs_id (cycle_processing): bad prediction");
    }
#endif /* EBUG */
  }

  /* Ici, le wfs x a ete range' en fs_id, toute re'fe'rence ulte'rieure a` x sera changee
     en une reference a fs_id ... */
  *attr_ptr0 = *fs_id;
      
  /* Ds le cas cyclique, les wfs_id_(son | father)s_set ont deja ete calcules par process_cyclic_wfs () */
  if (!cyclic_wfs_are_processed) {
    if (x > MAIN_WFS_ID) {
      /* x n'est pas $$ */
      /* "x" est calcule, toutes les structures qui en dependent sont maj */
      /* Non nommee ou nommee non vide => utilisee a un seul endroit */
      sxba_or (wfs_id_fathers_set, wfs_id_pred_set);

      id = 0;

      while ((id = sxba_scan_reset (wfs_id_pred_set, id)) > 0) {
	sxba_or (wfs_id2sons_set [id], wfs_id_sons_set);
	sxba_or (wfs_id2fathers_set [id], wfs_id_fathers_set);
	/* fathers_set = les pères des fils stricts (soi-même exclu) !!! */

	if (--wfs_id2son_nb [id] == 0) {
	  /* entierement calculee */
	  /* wfs_id2equiv [id] = -1; il est traite' ... */
	  PUSH (wfs_id_stack, id);
	}
      }
    }
  }
  /* wfs_id2equiv [x] = -1; /* il est traite' ... */

  return is_new;
}

/* Le 22/12/04 : Debut de l'implantation des structures cycliques */
/* Par exemple :
   $$ subj < $$ subj adjunct
*/
/* A FAIRE : Si une structure cyclique est chargee depuis un fs_id et ne change pas ds la wfs, ce n'est pas la peine
   de la [re]sauver (elle aura un autre fs_id) */
/* + generalement, cette remarque s'applique a tous les fs_id */
/* Version du 31/05/05 */
static int
process_cyclic_wfs ()
{
  int                     val, page_nb, delta_nb, field_id, equiv_class, x, y, z, simulated_fs_id, old_size, i, fs_id, cycle_size;
  int                     next, id, bot, signature, couple;
  int                     *attr_ptr, *attr_ptr0;
  SXBA                    field_set, line, cycle_set;
  SXBA                    wfs_id_sons_set, wfs_id_fathers_set, cyclic_wfs_id_fathers_set;
  unsigned char           static_field_kind;
  SXBOOLEAN                 is_in_cycle, is_shared, is_a_simulated_fs_id;
  static int              cyclic_M_size;
  struct adjunct_set_item *adjunct_ptr;

#if LOG
  fputs ("Cyclic f_structure processing\n", stdout);
#endif /* LOG */

  /* On calcule la fermeture transitive des fils ds cyclic_M */
  if (cyclic_M == NULL) {
    cyclic_M_size = wfs_id+1;
    /* Ca evite les reallocations successives sur des petits changements de taille tout en laissant cyclic_M
       de taille raisonnable */
    cyclic_M_size >>= SXSHIFT_AMOUNT;
    cyclic_M_size++;
    cyclic_M_size <<= SXSHIFT_AMOUNT;
    
    cyclic_M = sxbm_calloc (cyclic_M_size, cyclic_M_size);
  }
  else {
    if (cyclic_M_size < wfs_id+1) {
      old_size = cyclic_M_size;
      cyclic_M_size = wfs_id+1;
      cyclic_M_size >>= SXSHIFT_AMOUNT;
      cyclic_M_size++;
      cyclic_M_size <<= SXSHIFT_AMOUNT;

      cyclic_M = sxbm_resize (cyclic_M, old_size, cyclic_M_size, cyclic_M_size);
    }
  }

  if (cyclic_wfs_id2fs_id == NULL)
    cyclic_wfs_id2fs_id = (int*) sxalloc (AREA_SIZE*wfs_page_nb, sizeof (int));

  /* Il se peut que process_cyclic_wfs () calcule entierement les fs_ids, retourne le dernier calcule'
     c'est peut-etre celui du main */
  fs_id = 0;

  /* Le 17/05/05 j'utilise les preds pour remplir cyclic_M */
  for (x = MAIN_WFS_ID; x <= wfs_id; x++) {
    if (x == wfs_id2equiv [x] && wfs_id2son_nb [x]) {
      /* non traites */
      cyclic_wfs_id2fs_id [x] = 0;
      line = wfs_id2pred_set [x];
      y = 0;

      while ((y = sxba_scan (line, y)) >= 0) {
	/* x est un fils de y */
	SXBA_1_bit (cyclic_M [y], x);
      }
    }
  }

  fermer (cyclic_M, wfs_id+1);

  simulated_fs_id = XH_top (XH_fs_hd)-1;

  /* tri topologique du DAG a partir des feuilles */
  /* Version du 22/03/05, on ne touche pas a wfs_id2equiv */
  while (!SXBA_bit_is_set (wfs_id_set, MAIN_WFS_ID)) {
    for (x = MAIN_WFS_ID; x <= wfs_id; x++) {
      if (x == wfs_id2equiv [x] && wfs_id2son_nb [x] /* non deja traite' */) {
	if (!SXBA_bit_is_set (wfs_id_set, x)) {
	  /* C'est un candidat */
	  line = cyclic_M [x];
	  /*	  sxba_or(wfs_id2sons_set[x], line);*/

	  y = 0;
	  is_a_simulated_fs_id = SXFALSE;

	  while ((y = sxba_scan (line, y)) >= 0) {
	    if (!SXBA_bit_is_set (wfs_id_set, y) /* pas une feuille ... */
		&& !SXBA_bit_is_set (cyclic_M [y], x) /* ... et pas ds le cycle de x */
		) {
	      /* y n'est pas ds le cycle de x, ce cycle n'est donc pas une feuille */
	      break;
	    }
		  
	    if (cyclic_wfs_id2fs_id [y] != 0)
	      /* Un des descendants est simule' */
	      is_a_simulated_fs_id = SXTRUE;
	  }

	  if (y < 0) {
	    /* Ici, on a un cycle feuille dont x est le representant, on va l'evaluer ... */
	    if (SXBA_bit_is_set (line, x)) {
	      /* cas du cycle feuille */
	      /* On examine les elements d'un cycle de droite a gauche car si MAIN_WFS_ID est dans le cycle, il
		 doit etre pushe' en dernier ds simulated_wfs_id_stack.  Comme ca il sera pope' en 1er et il
		 se retrouvera donc au fond de wfs_id_stack */

	      /* On calcule les sons et fathers des noeuds du cycle */
	      cycle_set = fill_cycle (x);

	      if (!is_a_simulated_fs_id && cycle_already_seen (cycle_set)) {
#if LOG
		printf ("Cycle equivalence detection:\n");
#endif /* LOG */
		y = 0;

		while ((y = sxba_scan (cycle_set, y)) > 0) {
		  fs_id = cyclic_wfs_id2fs_id [y];
		  wfs2attr [AREA_PAGE (y)] [AREA_DELTA (y)] [0] = fs_id;
		  wfs_id2son_nb [y] = 0; /* Pour coherence */
		  SXBA_1_bit (wfs_id_set, y); /* traite' */
		  cyclic_wfs_id2fs_id [y] = 0; /* connu */
		  sxba_empty (cyclic_M [y]);
#if LOG
		  printf ("wfs_id = %i, fs_id = F%i\n", y, fs_id);
#endif /* LOG */
		}
	      }
	      else {
		/* Il faut ranger le cycle ... */
		/* ... ds cyclic_signXfs_id_hd, cyclic_sizeXgrp_id_hd et cyclic_grp_stack ... */
		/* ... car on peut le retrouver + tard */
		/* On ne peut 
		/* ... et la traitement d'un nouveau cycle */
		cycle_size = 0;
		bot = TOP (cyclic_grp_stack)+1;
		y = wfs_id+1;

		while ((y = sxba_1_rlscan (cycle_set, y)) >= 0) {
		  /* y est ds le cycle de x */
		  cycle_size++;
		  cyclic_wfs_id2fs_id [y] = 1; /* inconnu */
		  wfs_id2son_nb [y] = 0; /* Pour coherence */
		  SXBA_1_bit (wfs_id_set, y);
		  PUSH (simulated_wfs_id_stack, y);
		  sxba_empty (cyclic_M [y]);
		  simulated_fs_id++;
		  page_nb = AREA_PAGE (y);
		  delta_nb = AREA_DELTA (y);
		  attr_ptr0 = wfs2attr [page_nb] [delta_nb];
		  *attr_ptr0 = X80+X40+simulated_fs_id;
		  DPUSH (cyclic_grp_stack, simulated_fs_id);
		  field_set = wfs_field_set [page_nb] [delta_nb];

		  /* doit-on tenir compte de is_shared !! */
		  signature = SXBA_bit_is_set (field_set, pred_id) ? 
#ifndef ESSAI 
		    get_wfs_atom_val (attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)
#else /* ESSAI */
		    get_wfs_pred_val (attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)
#endif /* ESSAI */
		    : 0;
    
		  XxY_set (&cyclic_signXfs_id_hd, signature, simulated_fs_id, &couple);
		}
      
		XxY_set (&cyclic_sizeXgrp_id_hd, cycle_size, bot, &couple);
	      }
	    }
	    else {
	      /* Cas de la feuille simple */
	      if (SXBA_bit_is_reset_set (wfs_id_set, x)) {
		/* maintenant (le 02/08/06), il est possible que les descendants de x aient tous des (vieux) fs_id */
		wfs_id_sons_set = wfs_id2sons_set [x];
		SXBA_1_bit (wfs_id_sons_set, x);
		wfs_id_fathers_set = wfs_id2fathers_set [x];

		y = 0;

		while ((y = sxba_scan (line, y)) >= 0) {
		  /* y est un fils de x (dont un descendant* est un cycle) */
		  if (SXBA_bit_is_set (wfs_id2pred_set [y], x)) {
		    /* ... direct */
		    sxba_or (wfs_id_sons_set, wfs_id2sons_set [y]);
		    sxba_or (wfs_id_fathers_set, wfs_id2fathers_set [y]);
		  }
		}

		wfs_id2son_nb [x] = 0; /* Pour coherence */
		sxba_empty (cyclic_M [x]);

		if (is_a_simulated_fs_id) {
		  simulated_fs_id++;
		  wfs2attr [AREA_PAGE (x)] [AREA_DELTA (x)] [0] = X80+simulated_fs_id;
		  PUSH (simulated_wfs_id_stack, x);
		}
		else {
		  /* Tous les descendants de x ont des vieux fs_id */
		  /* x peut donc lui aussi avoir deja ete calcule' */
		  if (from_wfs_id_to_fs_id (x, &fs_id)) {
		    simulated_fs_id++; /* nouveau fs_id (inconnu) */ 
#if EBUG
		    if (simulated_fs_id != fs_id)
		      sxtrap(ME,"process_cyclic_wfs (simulated_fs_id != max_fs_id)");
#endif /* EBUG */
		  }
		  else {
		    /* fs_id déjà connu */
		  }

		  cyclic_wfs_id2fs_id [x] = 0;
		}
	      }
	    }
	  }
	}
      }
    }
  }

  sxba_empty (wfs_id_set);

  if (!IS_EMPTY (simulated_wfs_id_stack)) {
    fs_id = 0;

    /* On inverse le sens de simulated_wfs_id_stack */
    do {
      x = POP (simulated_wfs_id_stack);
      PUSH (wfs_id_stack, x);
    } while (!IS_EMPTY (simulated_wfs_id_stack));
  }
  else
    /* Tout est calcule' */
    fs_id = wfs2attr [0] [MAIN_WFS_ID] [0];

#if 0
  simulated_fs_id = XH_top (XH_fs_hd) + TOP (simulated_wfs_id_stack);

  /* On simule les calculs pour donner les bons fs_id */
  /* On inverse le sens de simulated_wfs_id_stack */
  while (!IS_EMPTY (simulated_wfs_id_stack)) {
    x = POP (simulated_wfs_id_stack);

    if (is_in_cycle = ((x & X80) != 0)) {
      x &= X7F;
    }
#if EBUG
    else
      SXBA_0_bit (wfs_id_set, x);
#else
    SXBA_0_bit (wfs_id_set, x);
#endif /* EBUG */

    PUSH (wfs_id_stack, x);
    simulated_fs_id--;
    /* X80 => predits */
    /* X40 => cycle */
    wfs2attr [AREA_PAGE (x)] [AREA_DELTA (x)] [0] = X80+simulated_fs_id+(is_in_cycle ? X40 : 0);
#if 0
    /* Les fs_id d'un cycle sont notes en negatif */
    wfs2attr [AREA_PAGE (x)] [AREA_DELTA (x)] [0] = is_in_cycle ? -simulated_fs_id : simulated_fs_id;
#endif /* 0 */
    sxba_empty (cyclic_M [x]);
  }

#if EBUG
  {
    int  cycle_size, bot, page_nb, delta_nb, signature, couple;
    int  *attr_ptr0;
    SXBA field_set;

    while (cycle_id) {
      cycle_size = 0;
      bot = TOP (cyclic_grp_stack)+1;

      x = 0;

      while ((x = sxba_scan (wfs_id_set, x)) > 0) {
	if (cyclic_wfs_id2fs_id [x] == cycle_id) {
	  /* x est ds le cycle cycle_id */
	  cycle_size++;
	  page_nb = AREA_PAGE (x);
	  delta_nb = AREA_DELTA (x);
	  
	  attr_ptr0 = wfs2attr [page_nb] [delta_nb];

	  simulated_fs_id = -attr_ptr0 [0];
	  DPUSH (cyclic_grp_stack, simulated_fs_id);
	  field_set = wfs_field_set [page_nb] [delta_nb];

	  /* doit-on tenir compte de is_shared !! */
	  signature = SXBA_bit_is_set (field_set, pred_id) ? 
#ifndef ESSAI 
	    get_wfs_atom_val (attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared) 
#else /* ESSAI */
	    get_wfs_pred_val (attr_ptr0 [pred_id] >> FS_ID_SHIFT, &is_shared)
#endif /* ESSAI */
	    : 0;
    
	  XxY_set (&cyclic_signXfs_id_hd, signature, simulated_fs_id, &couple);
	}
      }

      XxY_set (&cyclic_sizeXgrp_id_hd, cycle_size, bot, &couple);

      cycle_id--;
    }

    sxba_empty (wfs_id_set);
  }
#endif /* EBUG */
#endif /* 0 */

  return fs_id;
}






/* Les unifications ont reussi */
/* On stocke ds les structures definitives tout ce qui se trouve ds wfs */
/* Evaluation paresseuse 
   Ds wfs il y a des f_structures qui sont instanciees ou non instanciees
   Les instanciees resultent de l'unification de 0, 1 ou plusieurs fs_id
   Les non instanciees correspondent chacune a un seul couple (fs_id, $i).  Si le fs_id est
   non partage', elles depandent aussi du chemin d'acces PATH ds "$i PATH fs_id". */  
/* Soit f une f_structure et D l'ensemble de ses descendants dont f est exclu (ds le cas cyclique) et soit P
   l'ensembles des peres de D
   f est close ssi P est inclus dans {f} U D */
/* En particulier, si on a un arbre de racine f, f est close */ 
/* Si une structure close est non instanciee, on est su^r qu'aucun de ses descendants stricts ne va
   changer.  Elle n'a pas besoin d'etre instanciee.  Il faudra cependant la recalculer si elle
   change de categorie (si elle passe de partagee a non partagee ou inversement) */
/* Ds le cas contraire (structure non close), on l'instancie car un de ses descendants partage' peut
   avoir ete instancie' et modifie'.  Il peut meme ne plus etre partage'.
   Du fait de l'evaluation paresseuse, cette instanciation est faite ds fill_fs (gain si unif a echoue') */
/* Version du 31/05/05 */
static int
fill_fs (Xpq)
     int Xpq;
{
  int     X, x, fs_id;
  SXBOOLEAN is_main_wfs_id_processed, is_root;

  /* Le 18/04/05 */
  /* Si le champ Z du chemin $i X Y Z ... a ete instancie', cad :
     - si Y est une f_structure et si son *attr_ptr0 est nul ou
     - Si Y designe un atome ou un aij ou un ensemble de f_structures 
     et si l'adjunct_set contient soit un type ADJUNCT_KIND_WFS, ADJUNCT_KIND_ATOM ou ADJUNCT_KIND_Aij
     et si $i X est utilise' (path2wfs_id [$i X].wfs_id != 0)
     alors il faut instancier (s'ils ne le sont pas) toutes les f_structures des champs $i X Y, $i X, ... */

  /* Le 20/06/05 on peut specifier ds complete_id_set la liste des non-terminaux qui doivent etre
     completes et coherents */
  X = Aij2A (Xpq);

  if (Xpq == spf.outputG.start_symbol)
    is_root = SXTRUE;
  else {
    if (IS_COMPLETE(X)) {
      is_root = (is_ranking && RANKS (X) [0] == '$');
    }
    else
      is_root = SXFALSE;
  }

  /* Le 04/08/05 */
  if (!is_root && valid_eq_nb == 1 && valid_single_fs_id) {
    /* La seule equation qui a marche' est une equation simple ($$ = $i X* ;) */
    /* Elle nest pas "is_root" => coherence inutile car deja faite */
    return valid_single_fs_id;
  }

  fill_preds ();

  if (!coherence (Xpq, is_root)) {
    wfs_id_stack [0] = 0; /* positionne' par fill_preds */
    return 0;
  }

  /* Le 27/07/06 */
  if (valid_eq_nb == 1 && valid_single_fs_id && !is_fill_fs_on_start_symbol) {
    /* cas is_root et wfs coherent */
    /* La seule equation qui a marche' est une equation simple ($$ = $i X* ;) */
    /* Le fs_id n'a pas pu changer ... */
    /* ... mais ds le cas is_fill_fs_on_start_symbol on doit supprimer les =? s'il y'en a */
    wfs_id_stack [0] = 0; /* positionne' par fill_preds */
    return valid_single_fs_id;
  }

  /* Tri topologique, on commence par isoler celles qui ne dependent de rien */
  /* De l'id le +eleve vers le +petit, c,a ne marche pas car les structures nommees posent un pb */
  /* Ca doit marcher pour les structures cycliques !! */

  is_main_wfs_id_processed = cyclic_wfs_are_processed = SXFALSE;

  while (!IS_EMPTY (wfs_id_stack)) {
    x = POP (wfs_id_stack);
      /* entierement connu */
      /* Attention : il est possible que la structure "x" soit vide ds le cas de structures
	 lexicales nommees toutes vides */

      /* un wfs_id est clos ssi l'union des ensembles des peres de ses fils est inclus ds l'ensemble de ses fils */
      /* Ca doit aussi marcher sur les structures cycliques */
      /* les feuilles sont donc closes */
    from_wfs_id_to_fs_id (x, &fs_id);

    if (x == MAIN_WFS_ID)
      is_main_wfs_id_processed = SXTRUE;
  }

  if (!is_main_wfs_id_processed) {
    /* La structure principale n'est pas traitee => cycle */
    cyclic_wfs_are_processed = SXTRUE;
    fs_id = process_cyclic_wfs ();

    /* Si fs_id est non nul, c'est celui du main */
    if (fs_id == 0) {
    /* Ici on a predit tous les fs_id non encore calcules */
      while (!IS_EMPTY (wfs_id_stack)) {
	x = POP (wfs_id_stack);
	from_wfs_id_to_fs_id (x, &fs_id);
      }
    }
  }

  return fs_id; /* C'est MAIN_WFS_ID qui est traite' en dernier */
}



static int
fill_heads (Aij, fs_id)
     int Aij, fs_id;
{
  int head, x;

  /* On est a la racine */
  /* Maintenant on stocke le couple (Aij, fs_id) */
  if (!XxY_set (&heads, Aij, fs_id, &head)) {
    /* nouveau, il a donc un nouveau numero */ 
    /* ... et les fs_id d'un Aij sont en sequence, merci au "forest walker" */
    if ((x = Xpq2disp [Aij]) == 0) {
      /* 1ere apparition de Aij */
      DPUSH (fs_id_dstack, 0);
      x = Xpq2disp [Aij] = DTOP (fs_id_dstack); /* pointe sur le nb de fs differentes associees a Aij */
    }
    
    /* Le 20/06/05 */
    if (is_locally_unconsistent)
      head = -head;
      
    DPUSH (fs_id_dstack, head);

    fs_id_dstack [x]++; /* et un fs_id de plus ... */
  }
  else {
    if (is_locally_unconsistent)
      head = -head;
  }

  return head;
}

/* get_priority (fs_id) est remplacé par get_dynweight */
#if 0
/* Pour l'instant la priorite' d'une f_structure est le max des priorites de ses sous-structures */
/* On ne note pas les prio des fs_id deja vus, on les recalcule */
static int
get_priority (fs_id)
     int fs_id;
{
  int     x, max_priority, priority, prio, local_priority, bot, top, val, field_id, static_field_kind, bot2, cur2, top2, atom_val;
  SXBOOLEAN is_shared;

  if (fs_id == 0)
    return 0;

  if (!SXBA_bit_is_reset_set (fs_id_set, fs_id))
    /* cyclique */
    return 0; /* element neutre */

  /* Pour l'instant, on ne memoize pas */

  max_priority = 0;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  while (bot < top) {
    val = XH_list_elem (XH_fs_hd, bot);

    field_id = val & FIELD_AND;

    if (is_executable (field_id)) {
      /* Ce champ est calcule' ds cette passe */
      val >>= FS_ID_SHIFT;

      if (field_id == pred_id) {
#ifndef ESSAI
	atom_val = get_ufs_atom_val (val, &is_shared);
#else /* ESSAI */
	atom_val = get_ufs_atom_val (val, &is_shared, NULL);
#endif /* ESSAI */
	priority = XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, atom_val)+3);
      }
      else {
	priority = 0;

	if (val > 0) {
	  static_field_kind = field_id2kind [field_id];

	  if (static_field_kind == STRUCT_KIND) {
	    priority = get_priority (val);
	  }
	  else {
	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	      for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		   cur2 < top2;
		   cur2++) {
		val = XH_list_elem (XH_ufs_hd, cur2);
	      
		if (val <0)
		  break;

		prio = get_priority (val);
	    
		if (prio == MAX_PRIORITY) {
		  priority = MAX_PRIORITY;
		  break;
		}

		if (is_sum_priority)
		  priority += prio;
		else {
		  if (prio > priority)
		    priority = prio;
		}
	      }
	    }
	  }
	}
      }
	    
      if (priority == MAX_PRIORITY) {
	max_priority = MAX_PRIORITY;
	break;
      }
    
      if (is_sum_priority)
	max_priority += priority;
      else {
	if (priority > max_priority)
	  max_priority = priority;
      }
    }

    bot++;
  }

  SXBA_0_bit (fs_id_set, fs_id);

  return max_priority;
}
#endif /* 0 */

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

/* Calcul du dynweight d'une fs: il s'agit de la somme:
 1. du localdynweight qui est la contribution de la tête de la structure
 2. des dynweights des arguments (bounded ou non, présents ou non dans l'éventuel pred) */
static int
compute_dynweight (fs_id)
     int fs_id;
{
  int             x, dynweight, bot, top, val, field_id, bot2, cur2, top2;
  unsigned char   static_field_kind;
  SXBOOLEAN         skip_field;

  if (fs_id <= 0)
    return 0;

  if (!SXBA_bit_is_reset_set (visited_fs_id_set, fs_id))
    /* déjà pris en compte */
    return 0; /* element neutre */

  PUSH (visited_fs_id_stack, fs_id);

  /* on vérifie si on a déjà calculé, i.e. si fs_id2dynweight est déjà rempli à >0 pour fs_id */
  dynweight = fs_id2dynweight [fs_id];
  if (dynweight > 0) /* déjà calculé... */
    return dynweight;

  if (!SXBA_bit_is_reset_set (fs_id_set, fs_id))
    /* cyclique */
    return 0; /* element neutre */

  /* seule la contrib de la tête est connue */
  dynweight = fs_id2localdynweight [fs_id];
  /*
  if (dynweight == 0) {
    dynweight = fs_id2localdynweight [fs_id] = 1;
  }
  */
  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  while (bot < top) {
    val = XH_list_elem (XH_fs_hd, bot);

    field_id = val & FIELD_AND;
    skip_field = (val & OPTIONAL_VAL);

    if (is_executable (field_id)) {
      /* Ce champ est calcule' ds cette passe */
      val >>= FS_ID_SHIFT;

      if (field_id == pred_id) {
      }
      else {
	/* le dynweight d'une structure sous =? est nul ; ATTENTION: quid de celles sous =c ??? */
	if (!skip_field) {
	  /* On va prendre tous les arguments, meme si non presents ds pred */
	  if (val > 0) {
	    static_field_kind = field_id2kind [field_id];
	    
	    if (static_field_kind == STRUCT_KIND) {
	      dynweight += compute_dynweight (val);
	    }
	    else {
	      if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
		/* On suppose que les "set of struct" ne sont pas des arguments !! */
		for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		     cur2 < top2;
		     cur2++) {
		  val = XH_list_elem (XH_ufs_hd, cur2);

		  if (val < 0)
		    break;

		  dynweight += compute_dynweight (val);
		}
	      }
	    }
	  }
	}
      }
    }

    bot++;
  }

  SXBA_0_bit (fs_id_set, fs_id);

  return fs_id2dynweight [fs_id] = dynweight;
}

static int
get_dynweight (fs_id)
     int fs_id;
{
  int dynweight;

  dynweight = compute_dynweight (fs_id);

  while (!IS_EMPTY (visited_fs_id_stack)) {
    fs_id = POP (visited_fs_id_stack);
    SXBA_0_bit (visited_fs_id_set, fs_id);
  }

  return dynweight;
}


static void print_f_structure ();

static void
print_f_structures (Pij)
     int Pij;
{
  int        i, nb, fs_id, head, Aij, rule;
  int        *local_valid_heads_stack;
  struct lhs *plhs;
    
  i = Pij2disp [Pij];

  if (i == 0)
    return;

  local_valid_heads_stack = fs_id_Pij_dstack + i;
    
  plhs = spf.outputG.lhs+Pij;
  Aij = plhs->lhs;
  
  for (rule = PROD2RULE (plhs->init_prod); rule != 0; rule = rule2next_rule [rule]) {
    nb = local_valid_heads_stack [0];

    if (nb) {
      i = 1;

      while (i <= nb) {
	head = local_valid_heads_stack [i];

	if (head > 0)
	  fs_id = XxY_Y (heads, head);
	else
	  fs_id = XxY_Y (heads, -head);

	printf ("#%i/%i: ", (i+1)/2, nb/2);
    
	print_f_structure (Aij, fs_id, head < 0, SXTRUE);
	
	/* sauter le doldol */
	i += 2;
      }
    }

    local_valid_heads_stack += nb+1;
  }
}

static int
print_cf_structures (Pij)
     int Pij;
{
  /* On imprime cette production... */
  spf_print_iprod (stdout, Pij, "", "\n" /* with nl */);

  print_f_structures (Pij);

  return 1;
}

int
print_c_structures (Pij)
     int Pij;
{
  /* On imprime cette production... */
  spf_print_iprod (stdout, Pij, "", "\n" /* with nl */);

  return 1;
}



static int
print_pred_id (int val, int field_kind)
{
  int     sous_cat_id, cur2, bot2, top2, field_id, cur3, bot3, top3, Tpq, T, p, q;
  int     *dpv_ptr;
  SXBOOLEAN is_first3, is_optional, is_shared;
      
  if (val < 0) {
    val = -val;
    is_shared = SXTRUE;
  }
  else
    is_shared = SXFALSE;

  fputs ("pred =", stdout);

  if (field_kind & ASSIGN_CONSTRAINT_KIND)
    fputs ("c", stdout);
  else {
    if (field_kind & ASSIGN_OPTION_KIND)
      fputs ("?", stdout);
  }

  if (is_shared)
    fputs (" (shared)", stdout);

  if (val == 0) {
    fputs (" \"\"", stdout);
    return 0; /* priority_id !! */
  }

  dpv_ptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)));
  printf (" \"%s", lexeme_id2string [dpv_ptr [0] /* lexeme_id */]);
	    
  is_first3 = SXTRUE;
  sous_cat_id = dpv_ptr [1] /* sous_cat1 */;

  while (sous_cat_id) {
    fputs (" ", stdout);

    if (is_first3) {
      fputs ("<", stdout);
    }
      
    for (bot2 = cur2 = sous_cat [sous_cat_id], top2 = sous_cat [sous_cat_id+1];
	 cur2 < top2;
	 cur2++) {
      if (cur2 > bot2)
	fputs (", ", stdout);

      field_id = sous_cat_list [cur2];
	
      if (field_id < 0) {
	fputs ("(", stdout);
	field_id = -field_id;
	is_optional = SXTRUE;
      }
      else
	is_optional = SXFALSE;

      if (field_id > MAX_FIELD_ID) {
	field_id -= MAX_FIELD_ID;

	for (bot3 = cur3 = compound_field [field_id], top3 = compound_field [field_id+1];
	     cur3 < top3;
	     cur3++) {
	  field_id = compound_field_list [cur3];
	  printf ("%s%s", (cur3 != bot3) ? "|" : "", field_id2string [field_id]);
	}
      }
      else {
	printf ("%s", field_id2string [field_id]);
      }

      if (is_optional)
	fputs (")", stdout);
    }

    if (is_first3) {
      is_first3 = SXFALSE;
      sous_cat_id = dpv_ptr [2] /* sous_cat2 */;
      fputs (">", stdout);
    }
    else
      sous_cat_id = 0;
  }

  if (Tpq = dpv_ptr [4]) {
    Tpq -=  spf.outputG.maxxnt;
    T = -Tij2T (Tpq);
    p = Tij2i (Tpq);
    q = Tij2j (Tpq);
    
    if (p >= 0 && q >= 0) {
      /* p et q sont des mlstn */
      /*
	p = mlstn2dag_state (p);
	q = mlstn2dag_state (q);
      */

      printf (", %s[%i..%i]", spf.inputG.tstring [T], p, q);
    }
  } 
	  
  fputs ("\"", stdout);

  return dpv_ptr [3] /* priority_id */;
}

#if LOG
static void
print_consistent_Xpq_set ()
{
  int Xpq, Tpq;

  Xpq = sxba_scan (consistent_Xpq_set, 0);

  if (Xpq < 0)
    fputs ("\nThere are no f_structures that can be reused in the second attempt\n", stdout);
  else {
    fputs ("\nThe f_structures associated with the following Aij's are kept unchanged in the second attempt\n", stdout);

    do {
      if (Xpq > spf.outputG.maxxnt) {
	/* Tpq */
	Tpq = Xpq - spf.outputG.maxxnt;
	spf_print_Xpq (stdout, -Tpq);
      }
      else {
	spf_print_Xpq (stdout, Xpq);
      }

      fputs ("\n", stdout);
    } while ((Xpq = sxba_scan (consistent_Xpq_set, Xpq)) > 0);
  }
}
#endif /* LOG */

static void
print_f_structure (Xpq, fs_id, is_unconsistent, is_main_struct)
     int     Xpq, fs_id;
     SXBOOLEAN is_unconsistent, is_main_struct;
{
  int             bot, top, bot2, cur2, top2, bot3, cur3, top3, max_priority, priority, priority_id, dynweight, top_val, atom_val;
  int             val, field_id, field_kind, sous_cat_id, atom_id, local_atom_id, x, Tpq, T, p, q, id, fs_id_shared_nb, ufs_id_shared_nb;
  int             *local2atom_id;
  SXBOOLEAN         is_optional, is_first, is_first2, is_first3, is_shared;
  struct pred_val *ptr2;
  unsigned char   static_field_kind;


  if (fs_id) {
    if (!is_relaxed_run && is_unconsistent)
      /* is_unconsistent => f_structure de la racine */
      fputs (" *** WARNING: Unconsistent main f_structure *** ", stdout);

    if (fs_id < 0) {
      printf ("F%i/", -fs_id);

      if (Xpq > spf.outputG.maxxnt) {
	/* Tpq */
	Xpq -= spf.outputG.maxxnt;
	spf_print_Xpq (stdout, -Xpq);
      }
      else {
	spf_print_Xpq (stdout, Xpq);
      }

      printf ("= %i/ = [...]\n", Xpq);
      return;
    }

    X_clear (&shared_hd);
    /* Pour l'instant la priorite' d'une f_structure est le max des priorites de ses sous-structures */
    max_priority = 0;
    /* is_main_struct = SXTRUE; Le 29/03/05 c'est devenu un arg */
    x = 0; /* indice ds fs_id_stack */
    SXBA_1_bit (fs_id_set, fs_id); /* Si le main est recursif */

    do {
      bot = XH_X (XH_fs_hd, fs_id);
      top = XH_X (XH_fs_hd, fs_id+1);

      printf ("F%i", fs_id);
	
      top_val = 0;

      if (bot < top && (top_val = XH_list_elem (XH_fs_hd, top-1)) < 0) {
	/* structure nommee ou cyclique */
	top--;

	/* 
	   X80         == 1  <=> est partage ou cyclique ou non close
	   X40         == 1  <=> cyclique
	   X20         == 1  <=> partage
	   X10         == 1  <=> non close
	*/
	if (top_val & (X40+X20+X10))
	  printf ("%s%s%s", (top_val & X40) ? "(cyclic)" : "", (top_val & X20) ? "(shared)" : "", (top_val & X10) ? "(unclosed)" : "");

	if (top_val & X20) {
	  /* shared */
	  X_set (&shared_hd, fs_id, &fs_id_shared_nb); 
	}
      }

      if (is_main_struct) {
	is_main_struct = SXFALSE;
	fputs ("/", stdout);

	if (Xpq > spf.outputG.maxxnt) {
	  /* Tpq */
	  Xpq -= spf.outputG.maxxnt;
	  spf_print_Xpq (stdout, -Xpq);
	}
	else {
	  spf_print_Xpq (stdout, Xpq);
	}

	printf ("= %i/", Xpq);
      }
	
      fputs (" = [", stdout);

      is_first = SXTRUE;

      while (bot < top) {
	val = XH_list_elem (XH_fs_hd, bot);

	field_id = val & FIELD_AND;
	static_field_kind = field_id2kind [field_id];
	val >>= MAX_FIELD_ID_SHIFT;
	field_kind = val & KIND_AND;
	val >>= STRUCTURE_NAME_shift;

	if (is_first)
	  is_first = SXFALSE;
	else
	  fputs (", ", stdout);

	if (field_id == pred_id) {
#ifndef ESSAI
	  atom_val = get_ufs_atom_val (val, &is_shared);
#else /* ESSAI */
	  atom_val = get_ufs_atom_val (val, &is_shared, NULL);
#endif /* ESSAI */

	  if (is_shared)
	    atom_val = -atom_val ; /* shared */

	  priority_id = print_pred_id (atom_val, field_kind);
      
	  priority = priority_id2priority (priority_id);

	  if (priority > max_priority)
	    max_priority = priority;
	}
	else {
	  printf ("%s ", field_id2string [field_id]);

	  if (field_kind & ASSIGN_CONSTRAINT_KIND)
	    fputs ("=c ", stdout);
	  else {
	    if (field_kind & ASSIGN_OPTION_KIND)
	      fputs ("=? ", stdout);
	    else
	      fputs ("= ", stdout);
	  }

	  if (static_field_kind & ATOM_KIND) {
	    if (val == 0) {
	      /* On supporte 0 comme etant l'atome vide ... */
	      fputs ("()", stdout);
	    }
	    else {
#if ATOM_Aij || ATOM_Pij
	      if (static_field_kind & Aij_KIND) {
		fputs ("{", stdout);

		for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		     cur2 < top2;
		     cur2++) {
		  id = XH_list_elem (XH_ufs_hd, cur2);

		  if (id < 0)
		    break;

		  if (cur2 > bot2)
		    fputs (", ", stdout);

		  if (id > spf.outputG.maxxnt) { 
		    id -= spf.outputG.maxxnt; /* Pij */
		    printf ("R_%i_%i", id, spf.outputG.lhs [id].init_prod);
		  }
		  else
		    printf ("N%i", id /* Aij */);
		}
		
		if ((id & (X80+X20)) == (X80+X20)) {
		  X_set (&shared_hd, -val, &ufs_id_shared_nb); 
		  printf ("}%i", ufs_id_shared_nb);
		}
		else
		  fputs ("}", stdout);
	      }
	      else
#endif /* ATOM_Aij || ATOM_Pij */
		{
#ifndef ESSAI
		  if (static_field_kind & UNBOUNDED_KIND) {
		    atom_val = get_ufs_atom_val (val, &is_shared);

		    if (atom_val == 0) {
		      if (is_shared) {
			/* shared */
			X_set (&shared_hd, -val, &ufs_id_shared_nb); 
			printf ("()%i", ufs_id_shared_nb);
		      }
		      else
			fputs ("()", stdout);
		    }
		    else {
		      if (is_shared) {
			/* shared */
			X_set (&shared_hd, -val, &ufs_id_shared_nb); 
			printf ("(%s)%i", ATOM_ID2STRING (atom_val), ufs_id_shared_nb);
		      }
		      else
			printf ("%s", ATOM_ID2STRING (atom_val));
		    }
		  }
		  else {
		    atom_val = get_ufs_atom_val (val, &is_shared);

		    if (atom_val == 0) {
		      if (is_shared) {
			/* shared */
			X_set (&shared_hd, -val, &ufs_id_shared_nb); 
			printf ("()%i", ufs_id_shared_nb);
		      }
		      else
			fputs ("()", stdout);
		    }
		    else {
		      if (is_shared) {
			/* shared */
			fputs ("(", stdout);
		      }

		      local2atom_id = field_id2atom_field_id [field_id];
		      local_atom_id = 0;
		      is_first2 = SXTRUE;

		      while ((atom_val >>= 1) != 0) {
			local_atom_id++;

			if (atom_val & 1) {
			  atom_id = local2atom_id [local_atom_id];

			  if (is_first2)
			    is_first2 = SXFALSE;
			  else
			    fputs ("|", stdout);

			  printf ("%s", ATOM_ID2STRING (atom_id));
			}
		      }

		      if (is_shared) {
			X_set (&shared_hd, -val, &ufs_id_shared_nb); 
			printf (")%i", ufs_id_shared_nb);
		      }
		    }
		  }
#else /* ESSAI */
		  if (local_atom_id = get_ufs_atom_val (val, &is_shared, left_local_atom_id_set)) {
		    if (is_shared) {
		      /* shared */
		      fputs ("(", stdout);
		    }

		    SXBA_0_bit (left_local_atom_id_set, local_atom_id);
		    atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id);
		    printf ("%s", ATOM_ID2STRING (atom_id));

		    while ((local_atom_id = sxba_scan_reset (left_local_atom_id_set, local_atom_id)) > 0) {
		      atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id);
		      printf ("|%s", ATOM_ID2STRING (atom_id));
		    }

		    if (is_shared) {
		      X_set (&shared_hd, -val, &ufs_id_shared_nb); 
		      printf (")%i", ufs_id_shared_nb);
		    }
		  }
		  else {
		    if (is_shared) {
		      /* shared */
		      X_set (&shared_hd, -val, &ufs_id_shared_nb); 
		      printf ("()%i", ufs_id_shared_nb);
		    }
		    else
		      fputs ("()", stdout);
		  }
#endif /* ESSAI */
		}
	    }
	  }
	  else {
	    if (static_field_kind == STRUCT_KIND) {
	      if (val == 0) {
		fputs ("[]", stdout);
	      }
	      else {
		if (SXBA_bit_is_reset_set (fs_id_set, val))
		  PUSH (fs_id_stack, val);
		
		printf ("[]F%i", val);
	      }
	    }
	    else {
	      if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
		fputs ("{", stdout);

		if (val) {
		  for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		       cur2 < top2;
		       cur2++) {
		    atom_val = XH_list_elem (XH_ufs_hd, cur2);

		    if (atom_val < 0)
		      break;

		    if (cur2 > bot2)
		      fputs (", ", stdout);
		
		    if (SXBA_bit_is_reset_set (fs_id_set, atom_val))
		      PUSH (fs_id_stack, atom_val);

		    printf ("[]F%i", atom_val);
		  }
		}
		
		fputs ("}", stdout);

		if (atom_val < 0) {
		  if (atom_val & X20) {
		    X_set (&shared_hd, -val, &ufs_id_shared_nb); 
		    printf ("%i", ufs_id_shared_nb);
		  }

		  if (atom_val & X40)
		    /* cyclique !! */
		    fputs (".c", stdout);

		  if (atom_val & X10)
		    /* non close */
		    fputs (".nc", stdout);
		}
	      }
	    }
	  }
	}

	bot++;
      }
 
      fputs ("]", stdout);

      if (top_val & X20) {
	/* shared */
	printf ("%i", fs_id_shared_nb);
      }

      if (dynweight = fs_id2dynweight [fs_id]) {
	if (dynweight == MAX_PRIORITY)
	  printf (", dynweight = maximum (local=%i)\n", fs_id2localdynweight [fs_id]);
	else
	  printf (", dynweight = %i (local=%i)\n", dynweight, fs_id2localdynweight [fs_id]);
      }
      else
	printf (", Unknown dynweight (local=%i)\n", fs_id2localdynweight [fs_id]);

      if (++x > fs_id_stack [0])
	/* Pas de nl pour Priority ... */
	fs_id = 0;
      else {
	fs_id = fs_id_stack [x];
      }

    } while (fs_id);

    if (max_priority) {
      if (max_priority == MAX_PRIORITY)
	fputs ("Priority = maximum\n", stdout);
      else
	printf ("Priority = %i\n", max_priority);
    }
    else
      fputs ("Unknown priority\n", stdout);

    sxba_empty (fs_id_set);
    fs_id_stack [0] = 0;
  }
  else {
    fputs ("F0/", stdout);

    if (Xpq > spf.outputG.maxxnt) {
      /* Tpq */
      Xpq -= spf.outputG.maxxnt;
      spf_print_Xpq (stdout, -Xpq);
    }
    else {
      spf_print_Xpq (stdout, Xpq);
    }

    printf ("= %i/ = []\n", Xpq);
  }
}


#if LOG
static void
print_wf_structures ()
{
  int                     bot, top, bot2, cur2, top2, bot3, cur3, top3, max_priority, priority, priority_id, top_val;
  int                     val, field_id, field_kind, sous_cat_id, atom_id, local_atom_id, x, Tpq, T, p, q, id;
  int                     page_nb, delta_nb, fs_id_dol, fs_id, dol, preds_nb, y, cur, fs_id_main_struct;
  int                     field_id_path;
  int                     *attr_ptr0, *attr_ptr;
  SXBA                    field_set;
  struct adjunct_set_item *adjunct_ptr;
  int                     *local2atom_id;
  SXBOOLEAN                 is_optional, is_first, is_first2, is_first3, is_shared;
  struct pred_val         *ptr2;
  unsigned char           static_field_kind;

  fs_id_main_struct = 0;

  for (x = MAIN_WFS_ID; x <= wfs_id; x++) {
    printf ("$$%i = ", x);

    if (x == wfs_id2equiv [x]) {
      page_nb = AREA_PAGE (x);
      delta_nb = AREA_DELTA (x);
      attr_ptr0 = wfs2attr [page_nb] [delta_nb];

      if ((field_id_path = *attr_ptr0) > 0) {
	/* Le wfs_id x est non instancie' */
	fs_id_dol = path2wfs_id [field_id_path].fs_id_dol;
	fs_id = fs_id_dol >> MAXRHS_SHIFT;
	dol = fs_id_dol & MAXRHS_AND;

	printf ("$%i = F%i\n", dol, fs_id);

	if (x == MAIN_WFS_ID)
	  fs_id_main_struct = fs_id;
	else {
	  if (SXBA_bit_is_reset_set (fs_id_set, fs_id))
	    PUSH (fs_id_stack, fs_id);
	}
      }
      else {
	preds_nb = wfs_id2pred_nb [x];
	
	if (preds_nb > 1)
	  fputs ("(shared) ", stdout);

	fputs ("[", stdout);


	is_first = SXTRUE;
	max_priority = 0;

	field_set = wfs_field_set [page_nb] [delta_nb];
	field_id = 0;

	while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
	  attr_ptr = attr_ptr0 + field_id;
	  val = (*attr_ptr)>>FS_ID_SHIFT;
	
	  if (is_first)
	    is_first = SXFALSE;
	  else
	    fputs (", ", stdout);

	  if (field_id == pred_id) {
#ifndef ESSAI
	    val = get_wfs_atom_val (val, &is_shared);
#else /* ESSAI */
	    val = get_wfs_pred_val (val, &is_shared);
#endif /* ESSAI */

	    if (is_shared)
	      val = -val;

	    priority_id = print_pred_id (val, ((*attr_ptr) >> MAX_FIELD_ID_SHIFT) & KIND_AND);

	    max_priority = priority_id2priority (priority_id);
	  }
	  else {
	    static_field_kind = field_id2kind [field_id];
	    printf ("%s ", field_id2string [field_id]);
	      
	    if ((*attr_ptr) & CONSTRAINT_VAL)
	      fputs ("=c ", stdout);
	    else {
	      if ((*attr_ptr) & OPTIONAL_VAL)
		fputs ("=? ", stdout);
	      else
		fputs ("= ", stdout);
	    }

	    if (static_field_kind & ATOM_KIND) {
#if ATOM_Aij || ATOM_Pij
	      if (static_field_kind & Aij_KIND) {
		if (val) {
		  is_first2 = SXTRUE;
		  val = get_adjunct_equiv (val);

		  if (has_multiple_in_wfs_id (val))
		    /* shared */
		    fputs ("(shared)", stdout);

		  val = field_val2adjunct_id [val].adjunct_set_id;
		      
		  fputs ("{", stdout);

		  while (val) {
		    if (is_first2)
		      is_first2 = SXFALSE;
		    else
		      fputs (", ", stdout);

		    adjunct_ptr = adjunct_sets + val;
		    val = adjunct_ptr->next;
		    id = adjunct_ptr->id;

		    if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
		      /* id est un ufs_id */
		      top = XH_X (XH_ufs_hd, id+1);
		      bot = XH_X (XH_ufs_hd, id);

		      if (bot < top && XH_list_elem (XH_ufs_hd, top-1) < 0)
			top--;
	      
		      for (cur = bot;
			   cur < top;
			   cur++) {
			id = XH_list_elem (XH_ufs_hd, cur);

			if (cur > bot)
			  fputs (", ", stdout);
			
			if (id > spf.outputG.maxxnt) { 
			  id -= spf.outputG.maxxnt; /* Pij */
			  printf ("R_%i_%i", id, spf.outputG.lhs [id].init_prod);
			}
			else
			  printf ("N%i", id /* Aij */);
		      }
		    }
		    else {
		      if (id > spf.outputG.maxxnt) { 
			id -= spf.outputG.maxxnt; /* Pij */
			printf ("R_%i_%i", id, spf.outputG.lhs [id].init_prod);
		      }
		      else
			printf ("N%i", id /* Aij */);
		    }
		  }

		  fputs ("}", stdout);
		}
		else
		  fputs ("{}", stdout);
	      }
	      else
#endif /* ATOM_Aij || ATOM_Pij */
		{
#ifndef ESSAI
		  if (static_field_kind & UNBOUNDED_KIND) {
		    val = get_wfs_atom_val (val, &is_shared);

		    if (val == 0) {
		      if (is_shared)
			/* shared */
			fputs ("()shared", stdout);
		      else
			fputs ("()", stdout);
		    }
		    else {
		      if (is_shared)
			printf ("(%s)shared", ATOM_ID2STRING (val));
		      else
			printf ("%s", ATOM_ID2STRING (val));
		    }
		  }
		  else {
		    val = get_wfs_atom_val (val, &is_shared);

		    if (val == 0) {
		      if (is_shared)
			/* shared */
			fputs ("()shared", stdout);
		      else
			fputs ("()", stdout);
		    }
		    else {
		      if (is_shared)
			fputs ("(", stdout);

		      local2atom_id = field_id2atom_field_id [field_id];
		      local_atom_id = 0;
		      is_first2 = SXTRUE;

		      while ((val >>= 1) != 0) {
			local_atom_id++;

			if (val & 1) {
			  atom_id = local2atom_id [local_atom_id];

			  if (is_first2)
			    is_first2 = SXFALSE;
			  else
			    fputs ("|", stdout);

			  printf ("%s", ATOM_ID2STRING (atom_id));
			}
		      }

		      if (is_shared)
			fputs (")shared", stdout);
		    }
		  }
#else /* ESSAI */
		  if (get_wfs_atom_val (val, &is_shared, left_local_atom_id_set)) {
		    if (is_shared)
		      fputs ("(", stdout);

		    local_atom_id = sxba_scan_reset (left_local_atom_id_set, 0);
		    atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id);
		    printf ("%s", ATOM_ID2STRING (atom_id));

		    while ((local_atom_id = sxba_scan_reset (left_local_atom_id_set, local_atom_id)) > 0) {
		      atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id);
		      printf ("|%s", ATOM_ID2STRING (atom_id));
		    }

		    if (is_shared)
		      fputs (")shared", stdout);
		  }
		  else {
		    if (is_shared)
		      /* shared */
		      fputs ("()shared", stdout);
		    else
		      fputs ("()", stdout);
		  }
#endif /* ESSAI */
		}
	    }
	    else {
	      if (static_field_kind == STRUCT_KIND) {
		if (val == 0) {
		  fputs ("[]", stdout);
		}
		else {
		  printf ("$$%i", val);
		}
	      }
	      else {
		if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
		  if (val) {
		    is_first2 = SXTRUE;
		    val = get_adjunct_equiv (val);

		    if (has_multiple_in_wfs_id (val))
		      /* shared */
		      fputs ("(shared)", stdout);
		
		    val = field_val2adjunct_id [val].adjunct_set_id;
		      
		    fputs ("{", stdout);

		    while (val) {
		      if (is_first2)
			is_first2 = SXFALSE;
		      else
			fputs (", ", stdout);

		      adjunct_ptr = adjunct_sets + val;
		      val = adjunct_ptr->next;
		      id = adjunct_ptr->id;
		      /* dol = adjunct_ptr->dol; */

		      if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
			/* id est un ufs_id */
			/* qui provient de la rhs dol */
			top = XH_X (XH_ufs_hd, id+1);
			bot = XH_X (XH_ufs_hd, id);

			if (bot < top && XH_list_elem (XH_ufs_hd, top-1) < 0)
			  top--;
	      
			for (cur = bot;
			     cur < top;
			     cur++) {
			  fs_id = XH_list_elem (XH_ufs_hd, cur);
			  /* C'est un fs_id  que l'on verifiera + tard */
			  /* Si ce fs_id est non partage' il ne peut pas etre instancie' et ne peut donc
			     pas etre modifie' => la version ds fs est OK */
			  /* Si ce fs_id est partage', 2 cas :
			     - Soit (fs_id, dol) est instancie' et il sera verifie' ds la foulee
			     - Soit (fs_id, dol) est non instancie' et il n'a donc pas ete modifie'
			     et il peut donc se verifier ds fs
			  */
			  printf ("F%i", fs_id);

			  if (SXBA_bit_is_reset_set (fs_id_set, fs_id))
			    PUSH (fs_id_stack, fs_id);
			}
		      }
		      else {
			if (adjunct_ptr->kind == ADJUNCT_KIND_FS) {
			  /* id est un fs_id */
			  printf ("F%i", id);

			  if (SXBA_bit_is_reset_set (fs_id_set, id))
			    PUSH (fs_id_stack, id);
			}
			else {
			  if (adjunct_ptr->kind == ADJUNCT_KIND_WFS) {
			    /* id est un wfs_id, il sera imprime' par la boucle externe */
			    printf ("$$%i", id);
			  }
			}
		      }
		    }

		    fputs ("}", stdout);
		  }
		  else
		    fputs ("{}", stdout);
		}
	      }
	    }
	  }
	}
      
	fputs ("]", stdout);

	if (max_priority) {
	  if (max_priority == MAX_PRIORITY)
	    fputs (", Priority = maximum\n", stdout);
	  else
	    printf (", Priority = %i\n", max_priority);
	}
	else
	  fputs ("\n", stdout);
      }
    }
    else {
      printf ("$$%i\n", wfs_id2equiv [x]);
    }
  }

  if (fs_id_main_struct)
    print_f_structure (0, fs_id_main_struct, SXFALSE, SXTRUE);
  else {
    if (!IS_EMPTY (fs_id_stack) ) {
      fs_id = POP (fs_id_stack);
      SXBA_0_bit (fs_id_set, fs_id);
	
      print_f_structure (0, fs_id, SXFALSE, SXFALSE);
    }
  }
}
#endif /* LOG */




/* on travaille ds wfs */
/* positionne le champ field_id au niveau cur_wfs_id de wfs */
/* retourne SXTRUE ssi field_id existe deja */
static SXBOOLEAN
wfs_set (cur_wfs_id, field_id, ret_val)
     int cur_wfs_id, field_id, **ret_val;
{
  int  page_nb, delta_nb, val, static_field_kind;
  int  *attr_ptr0;
  SXBA field_set;

  cur_wfs_id = get_equiv (cur_wfs_id); /* En fait on est la` !! */

  page_nb = AREA_PAGE (cur_wfs_id);
  delta_nb = AREA_DELTA (cur_wfs_id);

  attr_ptr0 = wfs2attr [page_nb] [delta_nb];

  if (*attr_ptr0 == -1) {
    /* vide et non initialise' */
    *attr_ptr0 = 0;
  }
  else {
    if (*attr_ptr0 > 0) {
      /* C'est un fs_id qu'il faut commencer par etendre */
      instantiate_wfs_id (cur_wfs_id);
    }
  }

  *ret_val = wfs2attr [page_nb] [delta_nb] + field_id;
  field_set = wfs_field_set [page_nb] [delta_nb];

  if (SXBA_bit_is_reset_set (field_set, field_id)) {
    /* Nouveau champ */
    if (field_id2kind [field_id] == STRUCT_KIND) {
      val = get_a_new_wfs_id (); /* wfs_id == val */
    }
    else {
      /* On cree un slot atomique vide */
      val = get_a_new_empty_atom_id ();
    }

    **ret_val = (val<<FS_ID_SHIFT)+field_id;

    return SXFALSE;
  }

  return SXTRUE;
}

static SXBOOLEAN
wfs_id_examine (cur_wfs_id, field_id, result_ptr)
     int              cur_wfs_id, field_id;
     struct re_result *result_ptr;
{
  int  page_nb, delta_nb, path_id, fs_id_dol, fs_id;
  int  *attr_ptr0, *val_ptr;
      
  cur_wfs_id = get_equiv (cur_wfs_id); /* En fait on est la` !! */

  page_nb = AREA_PAGE (cur_wfs_id);
  delta_nb = AREA_DELTA (cur_wfs_id);

  attr_ptr0 = wfs2attr [page_nb] [delta_nb];

  if (*attr_ptr0 == -1) {
    /* vide et non initialise' */
    return SXFALSE;
  }

  if ((path_id = *attr_ptr0 )> 0) {
    /* C'est un fs_id */
    fs_id_dol = path2wfs_id [path_id].fs_id_dol;
    fs_id = fs_id_dol >> MAXRHS_SHIFT;
    /* On passe de wfs_id a fs_id */
    result_ptr->kind = fs_id_dol & MAXRHS_AND /* dol */;

    if (fs_is_set (fs_id, field_id, &val_ptr)) {
      /* On recopie le champ de XH_fs_hd.list */
      *(result_ptr->ptr) = *val_ptr;
      return SXTRUE;
    }

    return SXFALSE;
  }

  if (!SXBA_bit_is_set (wfs_field_set [page_nb] [delta_nb], field_id))
    return SXFALSE;

  *(result_ptr->ptr) = wfs2attr [page_nb] [delta_nb] [field_id];

  return SXTRUE;
}

/* Comme wfs_set, excepte' qu'on ne cree pas de nouveau champ */
static SXBOOLEAN
re_wfs_set (cur_wfs_id, field_id, ret_val)
     int             cur_wfs_id, field_id, **ret_val;
{
  int  page_nb, delta_nb;
  int  *attr_ptr0;
#ifdef ESSAI
  int  bot, top, cur;
#endif /* ESSAI */
      
  cur_wfs_id = get_equiv (cur_wfs_id); /* En fait on est la` !! */

  page_nb = AREA_PAGE (cur_wfs_id);
  delta_nb = AREA_DELTA (cur_wfs_id);

  attr_ptr0 = wfs2attr [page_nb] [delta_nb];

  if (*attr_ptr0 == -1) {
    /* vide et non initialise' */
    return SXFALSE;
  }

  if (*attr_ptr0 > 0) {
    /* C'est un fs_id qu'il faut commencer par etendre */
    instantiate_wfs_id (cur_wfs_id);
  }

  if (!SXBA_bit_is_set (wfs_field_set [page_nb] [delta_nb], field_id))
    return SXFALSE;

  *ret_val = wfs2attr [page_nb] [delta_nb] + field_id;

  return SXTRUE;
}

#if LOG
static int *ppcs;

static VARSTR
code2vstr (vstr)
     VARSTR vstr;
{
  int     oper, suffix_id, field_id, val, local_atom_id, atom_id;
  int     *local2atom_id;
  char    string [12];
  SXBOOLEAN is_first;
#ifdef ESSAI
  int     bot, top, cur;
#endif /* ESSAI */

  oper = *(--ppcs);

  if (oper >= 0)
    sxtrap (ME, "code2vstr");

  switch (oper) {
  case OPERATOR_SPACE:
    vstr = code2vstr (vstr);
    vstr = varstr_catenate (vstr, " ");
    vstr = code2vstr (vstr);
    break;
    
  case OPERATOR_DOT:
    vstr = code2vstr (vstr);
    vstr = varstr_catenate (vstr, ".");
    vstr = code2vstr (vstr);
    break;

  case OPERATOR_ATOM_MINUS:
    vstr = varstr_catenate (vstr, "& ");
    /* pas de break */
    
  case OPERATOR_MINUS:
    suffix_id = *(--ppcs);
    vstr = varstr_catenate (vstr, "(");
    vstr = code2vstr (vstr);
    vstr = varstr_catenate (vstr, ")");

    if (suffix_id > SXEMPTY_STE) {
      vstr = varstr_catenate (vstr, "-");
      vstr = varstr_catenate (vstr, suffix_id2string [suffix_id]);
    }
    break;

  case OPERATOR_LEXEME_REF:
  case OPERATOR_LEX_REF:
    if (oper == OPERATOR_LEXEME_REF)
      vstr = varstr_catenate (vstr, "&lexeme ");
    else
      vstr = varstr_catenate (vstr, "&lex ");
    /* pas de break */

  case OPERATOR_REF: 
    vstr = varstr_catenate (vstr, "$");
    oper = *(--ppcs);

    if (oper == 0)
      vstr = varstr_catenate (vstr, "$");
    else {
      sprintf (string, "%i", oper);
      vstr = varstr_catenate (vstr, string);
    }

    break;

  case OPERATOR_FIELD:
    oper = ppcs [-1];

    if (oper >= 0) {
      --ppcs;
      vstr = varstr_catenate (vstr, field_id2string [oper]);
    }
    else {
      vstr = code2vstr (vstr);
    }
    
    break;

  case OPERATOR_ATOM:
    field_id = *(--ppcs);
    val = *(--ppcs);

#ifndef ESSAI
    if (oper == OPERATOR_ATOM_MINUS)
      sxtrap(ME,"Concatenation operator for atomic values is not implemented in this old version of sxlfg");

    if (field_id == 0) {
      /* UNBOUNDED_KIND */
      vstr = varstr_catenate (vstr, ATOM_ID2STRING (val));
    }
    else {
      /* ATOM_KIND */
      local2atom_id = field_id2atom_field_id [field_id];
      local_atom_id = 0;
      is_first = SXTRUE;

      while ((val >>= 1) != 0) {
	local_atom_id++;

	if (val & 1) {
	  atom_id = local2atom_id [local_atom_id];

	  if (is_first)
	    is_first = SXFALSE;
	  else
	    vstr = varstr_catenate (vstr, "|");

	  vstr = varstr_catenate (vstr, ATOM_ID2STRING (atom_id));
	}
      }
    }
#else /* ESSAI */
    /* val est un id ds operator_atom2atom_list */
    bot = operator_atom2atom_list [val];
    top = operator_atom2atom_list [val+1];

    for (cur = bot; cur < top; cur++) {
      atom_id = atom_list [cur];

      if (cur > bot)
	vstr = varstr_catenate (vstr, "|");
	  
      vstr = varstr_catenate (vstr, ATOM_ID2STRING (atom_id));
    }
#endif /* ESSAI */

    break;

  case OPERATOR_COMPLEX_ATOM:
    val = *(--ppcs);    

    while (val--) {
      vstr = code2vstr (vstr);
      if (val)
	vstr = varstr_catenate (vstr, "|");
    }

    break;

  case OPERATOR_KLEENE:
    vstr = varstr_catenate (vstr, "(");
    vstr = code2vstr (vstr);
    vstr = varstr_catenate (vstr, ")*");

    break;

  case OPERATOR_OR:
    vstr = code2vstr (vstr);
    vstr = varstr_catenate (vstr, "|");
    vstr = code2vstr (vstr);

    break;

  case OPERATOR_RE:
    vstr = code2vstr (vstr);

    break;

  case OPERATOR_EMPTY_STRUCT:
    /* Il faudrait pouvoir sortir "{}" ou "()" suivant le cas ... */
    vstr = varstr_catenate (vstr, "[]");

    break;

  default:
    sxtrap (ME, "code2vstr");
  }

  return vstr;
}

static void
print_equation_code (xcode)
     int xcode;
{
  int  oper;
  char *left_string, *right_string;

  left_vstr = varstr_raz (left_vstr);

  ppcs = code_stack+xcode;

  oper = *ppcs;

  while (oper == OPERATOR_ETIQUETTE) {
    left_vstr = code2vstr (left_vstr);
    left_vstr = varstr_catenate (left_vstr, ": ");
    oper = *(--ppcs);
  }
    
  if (oper == OPERATOR_POSSIBLE) {
    left_vstr = varstr_catenate (left_vstr, "? ");
    oper = *(--ppcs);
  }

  switch (oper) {
  case OPERATOR_UNIFY :
  case OPERATOR_CONSTRAINT:
  case OPERATOR_IN:
  case OPERATOR_ADD:
  case OPERATOR_OPTIONAL:
    --ppcs; /* On saute la taille du code de la LHS */
    left_vstr = code2vstr (left_vstr);

    right_vstr = varstr_raz (right_vstr);
    right_vstr = code2vstr (right_vstr);

    left_string = varstr_tostr (left_vstr);
    right_string = varstr_tostr (right_vstr);

    switch (oper) {
    case OPERATOR_UNIFY :
      printf ("%s = %s", left_string, right_string); 
      break;

    case OPERATOR_CONSTRAINT:
      printf ("%s =c %s", left_string, right_string); 
      break;

    case OPERATOR_IN:
      printf ("%s < %s", left_string, right_string); 
      break;

    case OPERATOR_ADD:
      printf ("%s += %s", left_string, right_string); 
      break;

    case OPERATOR_OPTIONAL:
      printf ("%s =? %s", left_string, right_string); 
      break;
    }

    break;
  
  case OPERATOR_NONEXIST:
  case OPERATOR_EXIST:
    left_vstr = varstr_catenate (left_vstr, oper == OPERATOR_EXIST ? "+" : "-");

    left_vstr = code2vstr (left_vstr);
    left_string = varstr_tostr (left_vstr);
    printf ("%s", left_string); 
      
    break;

  default:
    sxtrap (ME, "print_equation_code");
  }
}
#endif /* LOG */

#ifdef ESSAI
/* atom_list_id est un indice ds operator_atom2atom_list et repere une liste (disjonction) d'atomes
   ds la table statique atom_list.
   Cette ft memoisee ds operator_atom2ufs_id transforme cette liste en une liste dynamique locale rangee
   ds XH_ufs_hd et retourne cet identifiant */
static int
atom_list_id2ufs_id (int atom_list_id)
{
  int ufs_id, bot, top, atom_id, local_atom_id;

  if ((ufs_id = operator_atom2ufs_id [atom_list_id]) == 0) {
    bot = operator_atom2atom_list [atom_list_id];
    top = operator_atom2atom_list [atom_list_id+1];

    if (bot == top-1) {
      atom_id = atom_list [bot];
      X_set (&atom_id2local_atom_id_hd, atom_id, &local_atom_id);
      XH_push (XH_ufs_hd, local_atom_id);
      XH_set (&XH_ufs_hd, &ufs_id);

#if ATOM_Aij || ATOM_Pij
      if (atom_id == ATOM_Aij) {
	UFS_ID_Aij = ufs_id;
      }
      else {
	if (atom_id == ATOM_Pij) {
	  UFS_ID_Pij = ufs_id;
	}
      }
#endif /* ATOM_Aij || ATOM_Pij */
    }
    else {
      while (bot < top) {
	atom_id = atom_list [bot];
	X_set (&atom_id2local_atom_id_hd, atom_id, &local_atom_id);
	/* On suppose qu'on peut utiliser left_local_atom_id_set */
	/* La suite suppose que les id locaux sont croissants */
	SXBA_1_bit (left_local_atom_id_set, local_atom_id);
	bot++;
      }

      local_atom_id = 0;

      while ((local_atom_id = sxba_scan_reset (left_local_atom_id_set, local_atom_id)) > 0)
	XH_push (XH_ufs_hd, local_atom_id);

      XH_set (&XH_ufs_hd, &ufs_id);
    }

    operator_atom2ufs_id [atom_list_id] = ufs_id;
  }

  return ufs_id;
}
#endif /* ESSAI */

/*
  -1 : skip the current equation
   0 : SXFALSE
   1 : SXTRUE
 */
/* Le 11/04/05 un seul arg qui est un ptr vers une struct re_result */
static SXBOOLEAN concat_field ();

static int
eval (result_ptr)
  struct re_result *result_ptr;
{
  int              field_id, kind, i, ref, Xpq, left_kind, val, val2, head, oper, string_id, local_atom_id, atom_id, prev_path_id, fs_id;
  int              complex_atom_id, local_complex_atom_id;
  int              static_field_kind, cur_path_id, cur_wfs_id, dol;
  int              *val_ptr, *head_ptr, *field_id_ptr, *ref_ptr;
  int              ret_val, p, q;
  int              **local_ptr;
  struct re_result dum_lresult, dum_rresult;
  SXBOOLEAN          is_old_field;

  local_ptr = &(result_ptr->ptr);

  oper = *(--pcs);

  if (oper < 0) {
    /* C'est un operateur */ 
    switch (oper) {
    case OPERATOR_SPACE:
    case OPERATOR_DOT:
      dum_lresult.ptr = &head;

      if ((ret_val = eval (&dum_lresult)) <= 0)
	return ret_val;
      
      dum_rresult.ptr = &field_id;

      if ((ret_val = eval (&dum_rresult)) <= 0)
	return ret_val;
      
      /* Le 31/05/05 */
      if (concat_field (&dum_lresult, &dum_rresult, result_ptr, 0 /* prev_top */,
			oper == OPERATOR_SPACE /* can_create */, &is_old_field /* unused */, SXFALSE /* is_re */))
	return 1;

#if EBUG
      /* Champ inconnu ds un $i ... avec un OPERATOR_DOT (pas de creation de champ) */
      printf (" <The field $%i ... %s is unknown> ", dum_lresult.kind>>REF_SHIFT, field_id2string [field_id]);
#endif /* EBUG */

      return -1; /* L'equation est seulement abandonnee ... ?? */
      /* ... sauf si la valeur de ce champ sert a construire un nouveau champ auquel cas l'unification doit echouer ?? */

    case OPERATOR_ATOM_MINUS:
    case OPERATOR_MINUS:
      /* ($i X* f)-string */
      string_id = *(--pcs); /* C'est le ste de string ds le constructeur !! */ 

      dum_lresult.ptr = &ref;
      ret_val = eval (&dum_lresult);

      if (ret_val <= 0) { 
	/* return  ret_val le 01/06/05 ?? */
	return ret_val; 
	/* return  0 : echec le 04/08/04 */
      }

      ref = *dum_lresult.ptr; /* dum_lresult.ptr a pu changer et ne plus pointer vers ref!! */
      field_id = ref & FIELD_AND;
      
      if (!is_executable (field_id))
	/* Le champ field_id n'est pas rempli au cours de cette passe */
	return -1;

      kind = dum_lresult.kind;

#if EBUG
      if ((kind & ATOM_FIELD) == 0)
	sxtrap (ME, "eval");
#endif /* EBUG */

      if (kind & NEW_FIELD) {
	/* Le champ f de ($i X* f)-string vient d'etre cre'e' vide */
	/* Le champ doit avoir une valeur !! */
	/* PB ordre d'evaluation.  Sa valeur peut etre positionnee ds une equation executee + tard !! */
	/* On ne sait pas resoudre les equations cycliques !! */
	/* $$ ($$ b-a)-a = b !!! */
#if EBUG || LOG
	fprintf (sxstderr,
		 "The field $%i ... %s has just been created, you must perhaps consider the \".\" operator?\n", 
		 kind >> REF_SHIFT,
		 field_id2string [field_id]);
#endif /* EBUG || LOG */

	return -1; /* Le 01/06/05 */
      }

      /* Il semble que meme ds le cas ($i X* f), le contenu du champ f ait ete mis ds un wfs_id !! */
#ifndef ESSAI
      ref = get_wfs_id_atom_val (ref);
      
      if (ref == 0) {
	/* valeur atomique vide */
#if EBUG || LOG
	fprintf (sxstderr,
		 "The field $%i ... %s has a nul value (empty), you must perhaps consider the equation order?\n", 
		 dol,
		 field_id2string [field_id]);
#endif /* EBUG || LOG */

	return -1;
      }
      static_field_kind = field_id2kind [field_id];

      if (static_field_kind & UNBOUNDED_KIND) {
	atom_id = ref;
      }
      else {
	local_atom_id = 1;

	while ((ref >>= 1) && (ref & 1) == 0)
	  local_atom_id++;

	if (ref >> 1) {
	  /* Le champ doit avoir une valeur unique !! */
	  fprintf (sxstderr,
		   "The field $%i ... %s has an illegal conjonctive value.\n",
		   dol,
		   field_id2string [field_id]);
	  return -1;
	}

	atom_id = field_id2atom_field_id [field_id] [local_atom_id];
      }
#else /* ESSAI */
      if ((local_atom_id = get_wfs_id_atom_val (ref, left_local_atom_id_set)) == 0) {
	/* valeur atomique vide */
#if EBUG || LOG
	fprintf (sxstderr,
		 "The field $%i ... %s has a null value (empty), you must perhaps consider the equation order?\n", 
		 dol,
		 field_id2string [field_id]);
#endif /* EBUG || LOG */

	return -1; /* si la valeur atomique obtenue est non trouvée, on échoue l'unification */
      }

      SXBA_0_bit (left_local_atom_id_set, local_atom_id);

      if (oper == OPERATOR_MINUS) {
	if (sxba_scan (left_local_atom_id_set, local_atom_id) > 0) {
	  /* Le champ doit avoir une valeur unique !! */
	  fprintf (sxstderr,
		   "Implementation restriction: The field $%i ... %s has an illegal non trivial disjonctive value.\n",
		   dol,
		   field_id2string [field_id]);
	  sxba_empty (left_local_atom_id_set);

	  return -1;
	} 
      }
#endif /* ESSAI */

      if (oper == OPERATOR_MINUS) {
#ifdef ESSAI
	atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id);
#endif /* ESSAI */

	val = XxY_is_set (&dfield_pairs, atom_id, string_id);

	if (val == 0) {
	  /* ($i X* f)-string n'est pas un champ */
#if EBUG || LOG
	  fprintf (sxstderr,
		   "The string value of ($%i ... %s)-string is not a field name, grammar spec error?\n", 
		   dol,
		   field_id2string [field_id]);
#endif /* EBUG || LOG */

	  return -1;
	}

	**local_ptr = field_id = dfield_pairs2field_id [val];

	result_ptr->kind = (static_field_kind & STRUCT_KIND)
	  ? (static_field_kind & UNBOUNDED_KIND
	     ? UNBOUNDED_STRUCT_FIELD
	     : STRUCT_FIELD)
	  : ATOM_FIELD;

      }
      else { /* cas OPERATOR_ATOM_MINUS */
	/* on est donc en mode ESSAI */
	local_complex_atom_id = 0;

	do {
	  atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id);
	  
	  if (val = XxY_is_set (&datom_pairs, atom_id, string_id)) { /* sinon, on a construit une partie gauche de l'opérateur OPERATOR_ATOM_MINUS qui n'est
									pas une valeur connue statiquement pour le champ correspondant */
	    if (complex_atom_id = datom_pairs2atom_id [val]) {/* sinon, on a construit une valeur atomique inconnue, qui est donc non prise en compte */
	      X_set (&atom_id2local_atom_id_hd, complex_atom_id, &local_complex_atom_id);
	      SXBA_1_bit (right_local_atom_id_set, local_complex_atom_id);
	    }
	  }

	} while ((local_atom_id = sxba_scan_reset (left_local_atom_id_set, local_atom_id)) > 0);

	if (local_complex_atom_id == 0) {
	  /* aucune valeur acceptable trouvée par concaténation */
#if EBUG || LOG
	  fprintf (sxstderr,
		   "No possible complex atomic value found\n");
#endif /* EBUG || LOG */

	  return -1;
	}

	**local_ptr = 0;
	result_ptr->kind = ATOM_VAL;
      }

      return 1;
      
    case OPERATOR_LEXEME_REF:
    case OPERATOR_LEX_REF: 
      /* Ici, meme code que OPERATOR_REF */
    case OPERATOR_REF: 
      dum_lresult.ptr = &ref;
      
      if ((ret_val = eval (&dum_lresult))  <= 0)
	return ret_val;

      /* ref est OK */
      /* dum_lresult.kind est non utilise' */
      result_ptr->kind = (ref << REF_SHIFT)+DOL_REF+STRUCT_FIELD;

      if (ref > 0) {
	/* Si un fils est vide sa semantique est vide... */
	head = rhs_pos2fs [ref]; /* Le bon (Xpq, p) */
#if 0
	/* Le 15/02/06 on ne tient compte que de fs_id */
	Xpq = XxY_X (heads, head);

	if (Xpq > spf.outputG.maxxnt) {
	  /* Tpq */
	  Xpq -= spf.outputG.maxxnt;
	  p = Tij2i (Xpq);
	  q = Tij2j (Xpq);
	}
	else {
	  p = Aij2i (Xpq);
	  q = Aij2j (Xpq);
	}

	fs_id = XxY_Y (heads, head); /* fs_id */

	if (p < 0 || q < 0 || p == q || fs_id == 0) {
	  /*... erreur ou vide */
	  return -1;
	}
#endif /* 0 */

	fs_id = XxY_Y (heads, head); /* fs_id */

	if (fs_id == 0) {
	  /*... erreur ou vide */
	  return -1;
	}

	SXBA_1_bit (doli_used_set, ref); /* Pour connaitre les doli utilises */

	/* On prend dol en negatif comme field_id ... */
	cur_path_id = result_ptr->path_id = field_id_path_catenate (-ref /* field_id */, 0 /* prev_path */, fs_id, ref);
	cur_wfs_id = path2wfs_id [cur_path_id].wfs_id;

	if (cur_wfs_id == 0) {
	  /* C'est la 1ere fois qu'on trouve $ref */
	  cur_wfs_id = instantiate_fs_id (fs_id, ref, cur_path_id);
	}

	head = cur_wfs_id;
      }
      else {
	/* $$ */
	/* Le 10/01/05 */
	head = MAIN_WFS_ID;
	result_ptr->path_id = 0; /* On est ds $$ */
      }
	
      result_ptr->father_wfs_id = 0; /* Le 26/05/05 : le pere de MAIN_WFS_ID ou de $i */

      head <<= FS_ID_SHIFT;

      **local_ptr = head;

      return 1;

    case OPERATOR_EMPTY_STRUCT:
      /* On est ds le cas "$$ ... = []" */
      result_ptr->kind = EMPTY_STRUCT;
      **local_ptr = 0;
      return 1;

    case OPERATOR_FIELD:
      dum_lresult.ptr = &field_id;

      if ((ret_val = eval (&dum_lresult)) <= 0)
	return ret_val;

      if (!is_executable (field_id))
	/* Ce champ n'est pas calcule' ds cette passe */
	return -1;

      /* field_id est OK */
      /* dum_lresult.kind est non utilise' */
      static_field_kind = field_id2kind [field_id];

      result_ptr->kind = (static_field_kind & STRUCT_KIND)
	? (static_field_kind & UNBOUNDED_KIND
	   ? UNBOUNDED_STRUCT_FIELD
	   : STRUCT_FIELD)
	: ATOM_FIELD;

      **local_ptr = field_id;
      
      /* ATOM_KIND */

      return 1;

    case OPERATOR_ATOM:
      field_id = *(--pcs); /* inutilise' ici */

#if 0
      /* inutile sur les valeurs atomiques, le champ de la LHS a deja fait le test */
      if (field_id > 0 /* field_id == 0 : UNBOUNDED_KIND ou Aij_KIND */ && !is_executable (field_id))
	/* Ce champ n'est pas calcule' ds cette passe */
	return -1;
#endif /* 0 */

      dum_lresult.ptr = &val;

      if ((ret_val = eval (&dum_lresult)) <=0)
	return ret_val;

      /* val est OK */
      /* dum_lresult.kind est non utilise' */
      result_ptr->kind = ATOM_VAL;
#ifdef ESSAI
      val = atom_list_id2ufs_id (val);
      /* Ici val est un ufs_id */
#endif /* ESSAI */
      **local_ptr = val;

      return 1;

    case OPERATOR_COMPLEX_ATOM:
      val = *(--pcs);    

      if (pcs[-1] == OPERATOR_ATOM) { /* on rajoute l'atome simple correspondant dans right_local_atom_id_set */
	int bot,top;

	val--;

	dum_lresult.ptr = &val2;
	
	if ((ret_val = eval (&dum_lresult)) <= 0)
	  return ret_val;	//	on traite l'atome;

	bot = XH_X (XH_ufs_hd, val2);
	top = XH_X (XH_ufs_hd, val2 + 1);

	while (bot<top) {
	  local_atom_id = XH_list_elem (XH_ufs_hd, bot);
	  SXBA_1_bit (right_local_atom_id_set, local_atom_id);
	  bot++;
	}

      }
      
      while (val--) { /* on rajoute les bit arrays représentant les valeurs complexes du champ atomique dans right_local_atom_id_set */
	dum_lresult.ptr = &val2;
	
	if ((ret_val = eval (&dum_lresult)) <= 0)
	  return ret_val;
      }
      
      /* tout le monde est là, on peut fabriquer le résultat */
      local_complex_atom_id = 0;

      while ((local_complex_atom_id = sxba_scan_reset (right_local_atom_id_set, local_complex_atom_id)) > 0) {
	XH_push (XH_ufs_hd, local_complex_atom_id);
      }
      
      XH_set(&XH_ufs_hd, *local_ptr);
      result_ptr->kind = ATOM_VAL;
      
      return 1;

    default:
      sxtrap (ME, "eval");
    }
  }
  else {
    /* C'est un operande */
    **local_ptr = oper;

    return 1;
  }
}

/* comme eval mais on ne cree pas de champ.
   Ca permet de savoir si l'equation courante doit etre sautee.
   dans ce cas ca evite la creation de champs intermediaires qui n'ont pas de raison d'etre puisque l'equation
   va etre sautee
*/
static int
eval_examine (result_ptr)
  struct re_result *result_ptr;
{
  int              field_id, kind, i, ref, Xpq, left_kind, val, head, oper, string_id, local_atom_id, atom_id, prev_path_id;
  int              static_field_kind, cur_path_id, cur_wfs_id, dol, field_val, atom_val;
  int              *val_ptr, *head_ptr, *ref_ptr, *field_val_ptr;
  int              ret_val, p, q, lret_val, rret_val, val2;
  int              **local_ptr;
  struct re_result dum_lresult, dum_rresult;
  SXBOOLEAN          is_old_field;
#ifdef ESSAI
  SXBOOLEAN          is_shared;
#endif /* ESSAI */

  local_ptr = &(result_ptr->ptr);

  oper = *(--pcs);

  if (oper < 0) {
    /* C'est un operateur */ 
    switch (oper) {
    case OPERATOR_SPACE:
    case OPERATOR_DOT:
      dum_lresult.ptr = &head;

      /* attention au cas tordu
	 $$ obj adjunct
	 ou` obj n'est pas ds $$ => return 0 et adjunct n'est pas evalue' ds la passe courante => return -1
	 or -1 doit avoir priorite' sur 0
	 => ds ce cas on continue donc a examiner les parties droites
      */

      if ((lret_val = eval_examine (&dum_lresult)) < 0)
	return lret_val;
      
      head >>= FS_ID_SHIFT;
      /* head est un [w]fs_id */
      dol = dum_lresult.kind;

      dum_rresult.ptr = &field_id;

      if ((rret_val = eval_examine (&dum_rresult)) < 0)
	return rret_val;

      if (lret_val == 0 || rret_val == 0)
	return 0;

      result_ptr->kind = dol;
      
      if (dol == 0) {
	/* head est un wfs_id ... */
	/* ... mais il peut ne pas etre completement instancie' */
	/* On peut donc poursuivre la recherche ds un vrai fs_id !! */
	/* Ds ce cas result_ptr->kind est change' */
	if (wfs_id_examine (head, field_id, result_ptr))
	  /* field_id est un champ de head */
	  return 1;
      }
      else {
	/* head est un fs_id */
	if (fs_is_set (head, field_id, &field_val_ptr)) {
	  /* field_id est un champ de head */
	  /* On recopie le champ de XH_fs_hd.list */
	  *(result_ptr->ptr) = *field_val_ptr;
	  return 1;
	}
      }

      /* Ici le champ de la partie droite n'existe pas dans les structures ... */
      if (oper == OPERATOR_SPACE) {
	/* ... Il faudra (peut etre) le creer */
	return 0;
      }
      else {
	/* ... Il devrait exister */
	/* On saute l'equation courante */
	return -1;
      }

    case OPERATOR_ATOM_MINUS:
    case OPERATOR_MINUS:
      /* ($i X* f)-string */
      string_id = *(--pcs); /* C'est le ste de string ds le constructeur !! */ 

      dum_lresult.ptr = &ref;
      ret_val = eval_examine (&dum_lresult);

      if (ret_val <= 0) { 
	/* return  ret_val le 01/06/05 ?? */
	return ret_val; 
	/* return  0 : echec le 04/08/04 */
      }

      dol = dum_lresult.kind;
      /* ref = *dum_lresult.ptr;  ici dum_lresult.ptr n'a pas change' et pointe toujours sur ref!! */
      field_id = ref & FIELD_AND;
      /* ref >> FS_ID_SHIFT est un [w]fs_id */
      
      if (!is_executable (field_id))
	/* Le champ field_id n'est pas rempli au cours de cette passe */
	return -1;

#ifndef ESSAI
      if (dol > 0) {
	/* ref est un (ufs_id, field_id) */
	atom_val = get_fs_id_atom_val (ref);
      }
      else {
	atom_val = get_wfs_id_atom_val (ref);
      }
	
      if (atom_val == 0) {
	/* valeur atomique vide */
#if EBUG || LOG
	fprintf (sxstderr,
		 "The field $%i ... %s has a nul value (empty), you must perhaps consider the equation order?\n", 
		 dol,
		 field_id2string [field_id]);
#endif /* EBUG || LOG */

	return -1;
      }

      static_field_kind = field_id2kind [field_id];

      if ((static_field_kind & UNBOUNDED_KIND) == 0) {
	local_atom_id = 1;

	while ((atom_val >>= 1) && (atom_val & 1) == 0)
	  local_atom_id++;

	if (atom_val >> 1) {
	  /* Le champ doit avoir une valeur unique !! */
	  fprintf (sxstderr,
		   "The field $%i ... %s has an illegal conjonctive value.\n",
		   dol,
		   field_id2string [field_id]);
	  return -1;
	}

	atom_val = field_id2atom_field_id [field_id] [local_atom_id];
      }
#else /* ESSAI */
      if (dol > 0) {
	/* ref est un (ufs_id, field_id) */
	local_atom_id = get_ufs_atom_val (ref >> FS_ID_SHIFT, &is_shared, left_local_atom_id_set);
      }
      else {
	local_atom_id = get_wfs_id_atom_val (ref, left_local_atom_id_set);
      }
	
      if (local_atom_id == 0) {
	/* valeur atomique vide */
#if EBUG || LOG
	fprintf (sxstderr,
		 "The field $%i ... %s has a nul value (empty), you must perhaps consider the equation order?\n", 
		 dol,
		 field_id2string [field_id]);
#endif /* EBUG || LOG */

	return -1;
      }

      SXBA_0_bit (left_local_atom_id_set, local_atom_id);

      if (oper == OPERATOR_MINUS) {
	if (sxba_scan (left_local_atom_id_set, local_atom_id) > 0) {
	  /* Le champ doit avoir une valeur unique !! */
	  fprintf (sxstderr,
		   "Implementation restriction: The field $%i ... %s has an illegal non trivial disjonctive value.\n",
		   dol,
		   field_id2string [field_id]);
	  sxba_empty (left_local_atom_id_set);

	  return -1;
	}
      }
#endif /* ESSAI */

      if (oper == OPERATOR_MINUS) {
#ifdef ESSAI
	atom_val = X_X (atom_id2local_atom_id_hd, local_atom_id);
#endif /* ESSAI */

      val = XxY_is_set (&dfield_pairs, atom_val, string_id);

      if (val == 0) {
	/* ($i X* f)-string n'est pas un champ */
#if EBUG || LOG
	fprintf (sxstderr,
		 "The string value of ($%i ... %s)-%s is not a field name, grammar spec error?\n", 
		 dol,
		 field_id2string [field_id],
		 string_id);
#endif /* EBUG || LOG */

	return -1;
      }

      **local_ptr = field_id = dfield_pairs2field_id [val];
      }
      else {
	/* cas OPERATOR_ATOM_MINUS */
	/* on est donc en mode ESSAI */
	/* A VOIR */
	/* En mode "eval_examine", le fait que OPERATOR_COMPLEX_ATOM ne parvienne pas a "fabriquer" des atomes n'est pas un motif
	   de skip de l'equation courante, on ne fait donc rien.  En revanche c,a sera un motif d'echec de l'unification et
	   donc du rejet du calcul des f_structures associees a la production courante */
	**local_ptr = 0; /* pour ne pas remonter des cochoneries */
      }

      return 1;
      
    case OPERATOR_LEXEME_REF:
    case OPERATOR_LEX_REF: 
      /* Ici, meme code que OPERATOR_REF */
    case OPERATOR_REF: 
      dum_lresult.ptr = &ref;
      
      if ((ret_val = eval_examine (&dum_lresult))  <= 0)
	return ret_val;

      /* ref est OK */
      /* dum_lresult.kind est non utilise' */
      result_ptr->kind = ref; /* Le $[$|i] */

      if (ref > 0) {
	/* Si un fils est vide sa semantique est vide... */
	head = rhs_pos2fs [ref]; /* Le bon (Xpq, p) */
#if 0
	/* Le 15/02/06 je me fie au fs_id */
	Xpq = XxY_X (heads, head);

	if (Xpq > spf.outputG.maxxnt) {
	  /* Tpq */
	  Xpq -= spf.outputG.maxxnt;
	  p = Tij2i (Xpq);
	  q = Tij2j (Xpq);
	}
	else {
	  p = Aij2i (Xpq);
	  q = Aij2j (Xpq);
	}

	if (p <= 0 || q <= 0 || p == q) {
	  /*... erreur ou vide */
	  return -1;
	}
#endif /* 0 */

	head = XxY_Y (heads, head); /* fs_id */

	if (head == 0)
	  /* vide */
	  return -1;
      }
      else {
	/* $$ */
	head = MAIN_WFS_ID;
      }

      **local_ptr = head << FS_ID_SHIFT;

      return 1;

    case OPERATOR_EMPTY_STRUCT:
      /* On est ds le cas "$$ ... = []" */
      return 1;

    case OPERATOR_FIELD:
      dum_lresult.ptr = &field_id;

      if ((ret_val = eval_examine (&dum_lresult)) <= 0)
	return ret_val;

      if (!is_executable (field_id))
	/* Ce champ n'est pas calcule' ds cette passe */
	return -1;

      **local_ptr = field_id;

      return 1;

    case OPERATOR_ATOM:
      field_id = *(--pcs); /* inutilise' ici */

      dum_lresult.ptr = &val;

      if ((ret_val = eval_examine (&dum_lresult)) <= 0)
	return ret_val;

      return 1;

    case OPERATOR_COMPLEX_ATOM:
      val = *(--pcs);    
      
      while (val--) { /* on examine les operandes : [OPERATOR_ATOM] {OPERATOR_ATOM_MINUS}+ */
	dum_lresult.ptr = &val2;
	
	if ((ret_val = eval_examine (&dum_lresult)) <= 0)
	  return ret_val;
      }
#if 0
      /* TODO : à écrire correctement */
      pcs++; /* Pour reexaminer OPERATOR_COMPLEX_ATOM ds eval */
      dum_lresult.ptr = &ref;

      if ((ret_val = eval (&dum_lresult)) <= 0)
	return ret_val;
#endif /* 0 */

      return 1;

    default:
      sxtrap (ME, "eval_examine");
    }
  }
  else {
    /* C'est un operande */
    **local_ptr = oper;

    return 1;
  }
}


static SXBOOLEAN
concat_field (re_left_ptr, re_right_ptr, re_result_ptr, prev_top, can_create, is_old_field, is_re)
     struct re_result *re_left_ptr, *re_right_ptr, *re_result_ptr;
     int              prev_top;
     SXBOOLEAN          can_create, *is_old_field, is_re;
{
  int     left_wfs_id, left_kind, left_dol, field_id, right_kind, left_path_id, fs_id_dol, fs_id, page_nb, delta_nb;
  int     path_id, new_fs_id, cur_path_id, cur_wfs_id, access_kind;
  int     *attr_ptr0, *ptr;
  SXBA    field_set;

  left_wfs_id = *(re_left_ptr->ptr) >> FS_ID_SHIFT;
  left_kind = re_left_ptr->kind;

#if EBUG
  if (left_wfs_id < MAIN_WFS_ID || left_wfs_id > wfs_id || (left_kind & STRUCT_FIELD) == 0)
    sxtrap (ME, "concat_field");
#endif /* EBUG */
    
  left_wfs_id = get_equiv (left_wfs_id);
  left_dol = left_kind>>REF_SHIFT;

  field_id = *(re_right_ptr->ptr);
  right_kind = re_right_ptr->kind;

  if (left_dol > 0) {
    /* Le chemin de x est de la forme $i X* */
    left_path_id = re_left_ptr->path_id;
    fs_id_dol = path2wfs_id [left_path_id].fs_id_dol;
    fs_id = fs_id_dol >> MAXRHS_SHIFT;
    page_nb = AREA_PAGE (left_wfs_id);
    delta_nb = AREA_DELTA (left_wfs_id);
    attr_ptr0 = wfs2attr [page_nb] [delta_nb];
    field_set = wfs_field_set [page_nb] [delta_nb];

#if EBUG
    if (left_dol != (fs_id_dol & MAXRHS_AND) || get_equiv (path2wfs_id [left_path_id].wfs_id) != left_wfs_id)
      sxtrap (ME, "concat_field");
      
    path_id = *attr_ptr0;

    if (path_id == -1 || (path_id > 0 && path2wfs_id [path_id].fs_id_dol != fs_id_dol))
      sxtrap (ME, "concat_field");
#endif /* EBUG */

    if (fs_is_set (fs_id, field_id, &ptr)) {
      /* field_id existe ds $i */
      re_result_ptr->next = prev_top;
      re_result_ptr->kind = (left_dol<<REF_SHIFT) + right_kind + OLD_FIELD;
      re_result_ptr->ptr = attr_ptr0 + field_id; /* On pointe ds wfs */
      re_result_ptr->father_wfs_id = left_wfs_id; /* Le 26/05/05 on note le wfs_id du pere */
      /* Le 16/02/05 on construit l'arbre des chemins fs_id avec partage des prefixes */
      /* ... et on note ds wfs_id */
      new_fs_id = (*ptr) >> FS_ID_SHIFT; /* C'est un fs_id ou un ufs_id */
      cur_path_id = re_result_ptr->path_id = field_id_path_catenate (field_id, left_path_id, new_fs_id, left_dol);

      /* ... on cree ds tous les cas */
      if (SXBA_bit_is_reset_set (field_set, field_id)) {
	/* Nouveau champ */
	cur_wfs_id = path2wfs_id [cur_path_id].wfs_id;

	if (cur_wfs_id == 0) {
	  /* C'est la 1ere fois qu'on trouve le chemin cur_path_id == "$i X* field_id" */
	  if (field_id2kind [field_id] == STRUCT_KIND) {
	    /* field_id est une f_structure */
	    /* ... on l'instancie */
	    cur_wfs_id = instantiate_fs_id (new_fs_id, left_dol, cur_path_id);
	  }
	  else {
	    /* la valeur retournee access_kind est inutilisee */
	    /* C'est un index ds field_val2adjunct_id */
	    cur_wfs_id = instantiate_ufs_id (field_id, cur_path_id/*, left_wfs_id */, &access_kind);
	  }
	}
	/* else reutilisation du cur_wfs_id precedant */

	attr_ptr0 [field_id] = (cur_wfs_id<<FS_ID_SHIFT) + (*ptr & FS_ID_AND);

	*is_old_field = SXFALSE;
      } 
      else {
#if EBUG
	if (field_id2kind [field_id] == STRUCT_KIND) {
	  cur_wfs_id = path2wfs_id [cur_path_id].wfs_id;

	  if (cur_wfs_id == 0 || get_equiv (attr_ptr0 [field_id]>>FS_ID_SHIFT) != get_equiv (cur_wfs_id))
	    sxtrap (ME, "concat_field");
	}
#endif /* EBUG */

	*is_old_field = SXTRUE;
      }

      return SXTRUE;
    }

    /* field_id n'existe pas ds $i */
    /* ... mais il existe peut etre ds left_wfs_id  ? */
    if ((*is_old_field = SXBA_bit_is_set (field_set, field_id) /* ... oui */) || can_create /* ... on le  cree */) {
      re_result_ptr->next = prev_top;
      /* On fait comme si on etait ds wfs => pas de (i<<REF_SHIFT) ds re_result_ptr->kind */
      re_result_ptr->kind = right_kind + ((*is_old_field) ? OLD_FIELD : NEW_FIELD);
      re_result_ptr->ptr = attr_ptr0+field_id;
      re_result_ptr->father_wfs_id = left_wfs_id; /* Le 26/05/05 on note le wfs_id du pere */

      if (!*is_old_field) {
	SXBA_1_bit (field_set, field_id);

	if (field_id2kind [field_id] == STRUCT_KIND) {
	  cur_wfs_id = get_a_new_wfs_id (); /* wfs_id == cur_wfs_id */
	}
	else {
	  /* On cree un slot atomique vide */
	  cur_wfs_id = get_a_new_empty_atom_id ();
	}

	if (is_re) {
	  /* Pour post_evaluate eventuel */
	  re_result_ptr->is_marked = SXTRUE;
	  re_result_ptr->prev_result = re_left_ptr-re_results;
	}

	attr_ptr0 [field_id] = (cur_wfs_id<<FS_ID_SHIFT)+field_id;
      }

      return SXTRUE;
    }
  }
  else {
    /* Le chemin est de la forme $$ X* field_id */
    *is_old_field = SXFALSE;

    if (!can_create && re_wfs_set (left_wfs_id, field_id, &ptr)
	|| can_create && ((*is_old_field = wfs_set (left_wfs_id, field_id, &ptr))+1 /* Kolossale finesse */)) {
      /* field_id existe ds $$ */
      re_result_ptr->next = prev_top;
      re_result_ptr->kind = right_kind + ((can_create && !*is_old_field) ? NEW_FIELD : OLD_FIELD);
      re_result_ptr->ptr = ptr;
      re_result_ptr->father_wfs_id = left_wfs_id; /* Le 26/05/05 on note le wfs_id du pere */

      if (is_re && can_create && !*is_old_field) {
	/* Pour post_evaluate eventuel */
	re_result_ptr->is_marked = SXTRUE;
	re_result_ptr->prev_result = re_left_ptr-re_results;
      }

      return SXTRUE;
    }
  }

  return SXFALSE;
}

/* Version du 31/05/05  qui utilise concat_field () */
static int
re_concat (left_top, right_top, can_create)
     int     left_top, right_top;
     SXBOOLEAN can_create;
{
  int              prev_top, new_top, x, y;
  struct re_result *re_left_ptr, *re_right_ptr, *re_result_ptr;
  SXBOOLEAN          is_old_field;

  if (left_top == 0)
    return right_top;

  prev_top = 0;

  for (x = left_top; x != 0; x = re_results [x].next) {
    /* Ds tous les cas ($$ ou $i) head est un wfs_id */
    for (y = right_top; y != 0; y = re_results [y].next) {
      new_top = re_results_get_new_top (); /* Ca peut etre perdu !! */
      /* Ca  a pu deborder */
      re_left_ptr = re_results+x;
      re_right_ptr = re_results+y;
      re_result_ptr = re_results+new_top;
	
      if (concat_field (re_left_ptr, re_right_ptr, re_result_ptr, prev_top, can_create, &is_old_field, SXTRUE /* is_re */)) {
	prev_top = new_top;
      }
      else {
	/* On recupere new_top !!*/
	re_results [0].kind--;
      }
    }
  }

  return prev_top;
}



/*
  valeur retournee :
  -1 : skip the current equation
   0 : SXFALSE
   >0 : indice ds re_results
 */
/*
  input est l'identifiant de l'ensemble des chemins des prefixes
 */
/* Si can_create et is_re, on cree un champ, s'il n'existe pas sur le fils droit de l'operateur OPERATOR_SPACE */
static int
re_eval (input, can_create, is_re)
     int     input;
     SXBOOLEAN can_create, is_re;
{
  int              oper, left_ret_val, right_ret_val, string_id, prev_top, x, ref, kind, field_id, atom_id;
  int              local_atom_id, val, new_top;
  int              *store_pcs, next, last_ret_index, ret_val, prev_ret_val, static_field_kind;
  struct re_result *re_results_ptr;
#ifdef ESSAI
  SXBOOLEAN          is_shared;
#endif /* ESSAI */

  oper = *(--pcs);

  if (oper < 0) {
    /* C'est un operateur */ 
    switch (oper) {
    case OPERATOR_SPACE:
    case OPERATOR_DOT:
      if ((left_ret_val = re_eval (input, SXFALSE, is_re)) <= 0)
	return left_ret_val;

      return re_eval (left_ret_val, is_re && (oper == OPERATOR_SPACE), is_re);

    case OPERATOR_COMPLEX_ATOM:
    case OPERATOR_ATOM_MINUS:
      sxtrap(ME,"re_eval (concatenation to build atomic values is incompatible with regular expressions on paths)");

    case OPERATOR_MINUS:
      /* ($i X* f)-string */
      string_id = *(--pcs); /* C'est le ste de string ds le constructeur !! */ 

      if ((left_ret_val = re_eval (0, SXFALSE, is_re)) <= 0)
	return left_ret_val;

      prev_top = 0;

      for (x = left_ret_val; x != 0; x = re_results [x].next) {
	ref = *(re_results [x].ptr);
	kind = re_results [x].kind;

	/* !!?? Le 25/05/05 je testerais pluto^t (kind & OLD_FIELD) !!?? */
	if (kind & NEW_FIELD) {
	  /* Le champ doit avoir une valeur !! */
	  field_id = ref & FIELD_AND;

	  if (!is_executable (field_id))
	    /* Ce champ n'est pas calcule' ds cette passe */
	    continue;

#ifndef ESSAI
	  ref = get_fs_id_atom_val (ref);

#if EBUG
	  if ((kind & ATOM_FIELD) == 0 || ref == 0)
	    sxtrap (ME, "re_eval");
#endif /* EBUG */

	  if (field_id2kind [field_id] == UNBOUNDED_KIND) {
	    atom_id = ref;
	  }
	  else {
	    local_atom_id = 1;

	    while ((ref >>= 1) && (ref & 1) == 0)
	      local_atom_id++;

	    if (ref >> 1) {
	      /* Le champ doit avoir une valeur unique !! */
	      fprintf (sxstderr,
		       "The field $%i ... %s has an illegal disjonctive value.\n",
		       kind>>REF_SHIFT,
		       field_id2string [field_id]);
	      return 0;
	    }

	    atom_id = field_id2atom_field_id [field_id] [local_atom_id];
	  }
#else /* ESSAI */
#if EBUG
	  if ((kind & ATOM_FIELD) == 0)
	    sxtrap (ME, "re_eval");
#endif /* EBUG */
	    
	  local_atom_id = get_ufs_atom_val (ref >> FS_ID_SHIFT, &is_shared, left_local_atom_id_set);
	
	  if (local_atom_id == 0) {
	    /* valeur atomique vide */
#if EBUG || LOG
	    fprintf (sxstderr,
		     "The field $%i ... %s has a nul value (empty), you must perhaps consider the equation order?\n", 
		     kind>>REF_SHIFT,
		     field_id2string [field_id]);
#endif /* EBUG || LOG */

	    return 0;
	  }

	  SXBA_0_bit (left_local_atom_id_set, local_atom_id);

	  if (sxba_scan (left_local_atom_id_set, local_atom_id) > 0) {
	    /* Le champ doit avoir une valeur unique !! */
	    fprintf (sxstderr,
		     "Implementation restriction: The field $%i ... %s has an illegal non trivial disjonctive value.\n",
		     kind>>REF_SHIFT,
		     field_id2string [field_id]);
	    sxba_empty (left_local_atom_id_set);

	    return 0;
	  }

	  atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id);
#endif /* ESSAI */

	  val = XxY_is_set (&dfield_pairs, atom_id, string_id);

#if EBUG
	  if (val == 0)
	    sxtrap (ME, "re_eval");
#endif /* EBUG */

	  new_top = re_results_get_new_top ();
	  re_results [new_top].next = prev_top;
	  field_id = *(re_results [new_top].ptr = &(dfield_pairs2field_id [val]));

	  if (!is_executable (field_id))
	    /* Ce champ n'est pas calcule' ds cette passe */
	    continue;

	  static_field_kind = field_id2kind [field_id];

	  re_results [new_top].kind = (static_field_kind & STRUCT_KIND)
	    ? (static_field_kind & UNBOUNDED_KIND
	       ? UNBOUNDED_STRUCT_FIELD
	       : STRUCT_FIELD)
	    : ATOM_FIELD;

	  prev_top = new_top;
	}
      }

      return re_concat (input, prev_top, SXFALSE);    

    case OPERATOR_KLEENE:
      /* $i P (...)* */
      store_pcs = pcs;
      prev_ret_val = input;

      /* On evalue le bloc */
      for (;;) {
	pcs = store_pcs;

	if ((left_ret_val = re_eval (prev_ret_val, SXFALSE, is_re)) == -1)
	  return -1;

	if (left_ret_val == 0)
	  /* cette eval de (...)* n'a rien trouve */
	  break;

	/* Ne pas toucher a input */
	/* On fait l'union de prev_ret_val et left_ret_val ds left_ret_val */
	/* On parcourt prev_ret_val ... */
	next = prev_ret_val;

	while ((next = re_results [last_ret_index = next].next) != 0);
	  
	/* ... et on lui concatene left_ret_val !! */
	re_results [last_ret_index].next = left_ret_val;
	prev_ret_val = left_ret_val;
      }

      /* Le 2/12/04 */
      /* En mettant les + longs en tete de liste !! */
      /* J'inverse la liste */
      next = input;
      prev_ret_val = 0;

      while ((next = re_results [last_ret_index = next].next) != 0) {
	re_results [last_ret_index].next = prev_ret_val;
	prev_ret_val = last_ret_index;
      }

      re_results [last_ret_index].next = prev_ret_val;

      return last_ret_index;

    case OPERATOR_OR:
      if ((left_ret_val = re_eval (input, can_create, is_re)) == -1)
	return -1;

      if ((right_ret_val = re_eval (input, can_create, is_re)) == -1)
	return -1;
	  
      if (left_ret_val == 0) return right_ret_val;
      if (right_ret_val == 0) return left_ret_val;

      /* Les deux operandes sont non vides, on les concatene (c'est le resultat du |) */
      /* On parcourt l'operande droit ... */
      next = right_ret_val;

      while ((next = re_results [last_ret_index = next].next) != 0);
	  
      /* ... et on lui concatene le gauche !! */
      re_results [last_ret_index].next = left_ret_val;

      return right_ret_val;

    case OPERATOR_LEXEME_REF:
    case OPERATOR_LEX_REF: 
    case OPERATOR_REF: 
    case OPERATOR_FIELD:
    case OPERATOR_ATOM:
    case OPERATOR_EMPTY_STRUCT:
      /* On peut utiliser eval () !! */
      /* input ne sert a rien */
      pcs++;
      new_top = re_results_get_new_top ();

      re_results_ptr = re_results+new_top;
      re_results_ptr->ptr = int_tank_get ();
      /* re_results_ptr->next = 0; raze' par re_results_get_new_top () */

      if ((ret_val = eval (re_results_ptr)) <= 0)
	return ret_val;
      

#ifdef ESSAI
      if (oper == OPERATOR_ATOM) {
	re_results_ptr = re_results+new_top; /* prudence !! */
	*(re_results_ptr->ptr) = atom_list_id2ufs_id (*(re_results_ptr->ptr));
      }
      else
#endif /* ESSAI */
	if (oper == OPERATOR_FIELD && input != 0) {
	  new_top = re_concat (input, new_top, can_create);
	}

      return new_top;

    default:
      sxtrap (ME, "re_eval");
    }
  }
#if EBUG
  else {
    /* C'est un operande */
    sxtrap (ME, "re_eval");
  }
#endif /* EBUG */
}


/* Nelle nelle version du 07/04/05 */
/* C'est la 1ere utilisation de (right_dol, fs_id) par le chemin field_id_path */
static int
instantiate_fs_id (fs_id, right_dol, field_id_path)
     int     fs_id, right_dol, field_id_path;
{	  
  int     new_wfs_id, bot, top, top_val, dol_fs_id;

  /* Si fs_id est partage' ou cyclique, il se peut qu'il soit deja re'fe'rence' par un autre chemin de champs */

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
  top_val = (bot < top) ? XH_list_elem (XH_fs_hd, top-1) : 0;

  if (top_val < 0 && (top_val & (X40+X20))) {
    /* fs_id est partage' ou cyclique */
    /* on regarde s'il a deja ete reference' par le fils $right_dol */
    if (XxY_set (&XxY_dol_fs_id, right_dol, fs_id, &dol_fs_id) && (new_wfs_id = dol_fs_id2wfs_id [dol_fs_id])) {
      /* deja utilise' */
      new_wfs_id = get_equiv (new_wfs_id);
    }
    else {
      /* 1ere utilisation */
      path2wfs_id [field_id_path].wfs_id = new_wfs_id = dol_fs_id2wfs_id [dol_fs_id] = get_a_new_wfs_id ();
      wfs2attr [AREA_PAGE (new_wfs_id)] [AREA_DELTA (new_wfs_id)] [0] = field_id_path;
      
      /* si on est à la racine, on veut que l'instantiation soit récursive */
      if (is_fill_fs_on_start_symbol)
	instantiate_wfs_id (new_wfs_id);
    }
  }
  else {
    path2wfs_id [field_id_path].wfs_id = new_wfs_id = get_a_new_wfs_id ();
    wfs2attr [AREA_PAGE (new_wfs_id)] [AREA_DELTA (new_wfs_id)] [0] = field_id_path;

    /* si on est à la racine, on veut que l'instantiation soit récursive */
    if (is_fill_fs_on_start_symbol)
      instantiate_wfs_id (new_wfs_id);
  }

#if EBUG
  if (path2wfs_id [field_id_path].fs_id_dol != (fs_id << MAXRHS_SHIFT) + right_dol)
    sxtrap (ME, "instantiate_fs_id");
#endif /* EBUG */

  return new_wfs_id;
}


/* access_kind == 0 => 1ere instanciation
               == 1 => deja instancie par le chemin field_id_path
	       == 2 => deja instancie par partage
*/
/* On instancie un ufs_id qcq atteint par ($i X* field_id) == field_id_path dans le wfs father_wfs_id */
/* Version du 31/05/05 */
static int
instantiate_ufs_id (field_id, field_id_path, access_kind)
     int     field_id, field_id_path, *access_kind;
{	  
  int                     ufs_id, right_dol, adjunct_set_id, bot, cur, top, top_val, dol_fs_id, ufs_id_dol;
  int                     prev_top, pos, val, new_top, fs_id, bot2, top2, top2_val, new_wfs_id, new_path;
  struct adjunct_set_item *adjunct_ptr;
  unsigned char           static_field_kind;

#if EBUG
  if (field_id_path <= 0 || field_id <= 0)
    sxtrap (ME, "instantiate_ufs_id");
#endif /* EBUG */

  adjunct_set_id = path2wfs_id [field_id_path].wfs_id;

  if (adjunct_set_id) {
    /* deja instancie et reference' par ce chemin */
    *access_kind = 1;
    adjunct_set_id = get_adjunct_equiv (adjunct_set_id);

    return adjunct_set_id;
  }

  ufs_id_dol = path2wfs_id [field_id_path].fs_id_dol;
  ufs_id = ufs_id_dol >> MAXRHS_SHIFT;
  right_dol = ufs_id_dol & MAXRHS_AND;

  /* Si ufs_id est partage', il se peut qu'il soit deja re'fe'rence'
     par un autre chemin de champs */
  bot = XH_X (XH_ufs_hd, ufs_id);
  top = XH_X (XH_ufs_hd, ufs_id+1);

  top_val = (bot < top) ? XH_list_elem (XH_ufs_hd, top-1) : 0;

  if (top_val < 0) {
    if (top_val & X20) {
      /* ufs_id est partage'*/
      /* on regarde s'il a deja ete reference' par $right_dol */
      /* Les ufs_id sont utilises en negatif */
      /* A VOIR le remplacement de XH_ufs_hd par XH_fs_hd */
      if (XxY_set (&XxY_dol_fs_id, right_dol, -ufs_id, &dol_fs_id)) {
	/* deja instancie et reference' par un autre chemin */
	adjunct_set_id = path2wfs_id [field_id_path].wfs_id = dol_fs_id2wfs_id [dol_fs_id];
	*access_kind = 2;
	adjunct_set_id = get_adjunct_equiv (adjunct_set_id);
 
	return adjunct_set_id;
      }
    }
  }

  adjunct_set_id = get_new_top_field_val2adjunct_id ();

  if (top_val < 0) {
    if (top_val & X20)
      dol_fs_id2wfs_id [dol_fs_id] = adjunct_set_id;

    top--;
  }

  path2wfs_id [field_id_path].wfs_id = adjunct_set_id;
  *access_kind = 0; /* 1ere instanciation */
  static_field_kind = field_id2kind [field_id];

#if EBUG
  if (static_field_kind == STRUCT_KIND)
    sxtrap (ME, "instantiate_ufs_id");
#endif /* EBUG */

  prev_top = field_val2adjunct_id [adjunct_set_id].adjunct_set_id = get_new_top_adjunct_sets ();
  adjunct_ptr = adjunct_sets + prev_top;
  adjunct_ptr->next = 0;

  if (static_field_kind & ATOM_KIND) {
    /* On conserve l'ufs_id */
    adjunct_ptr->id = ufs_id;
    adjunct_ptr->kind = ADJUNCT_KIND_UFS;
  
    return adjunct_set_id;
  }

#if EBUG
  if (static_field_kind != STRUCT_KIND+UNBOUNDED_KIND)
    sxtrap (ME, "instantiate_ufs_id");
#endif /* EBUG */
  
  if (top_val < 0 && (top_val & X10) || is_fill_fs_on_start_symbol) {
    /* On n'instancie que les adjuncts qui contiennent des fs_id partages ou non clos */
    if (bot == top) {
      /* vide */
      adjunct_ptr->id = 0;
      adjunct_ptr->kind = ADJUNCT_KIND_EMPTY;
    }
    else {
      for (cur = bot, pos = 1; cur < top; cur++, pos++) {
	val = XH_list_elem (XH_ufs_hd, cur);

	if (adjunct_ptr == NULL) {
	  new_top = get_new_top_adjunct_sets ();
	  adjunct_sets [prev_top].next = new_top;
	  adjunct_ptr = adjunct_sets + new_top;
	  adjunct_ptr->next = 0;
	  prev_top = new_top;
	}

	/* val est un fs_id */
	/* Si val est non partage' on le laisse tel quel
	   sinon, on l'instancie ds un wfs_id */
	fs_id = val;

	bot2 = XH_X (XH_fs_hd, fs_id);
	top2 = XH_X (XH_fs_hd, fs_id+1);
	top2_val = (bot2 < top2) ? XH_list_elem (XH_fs_hd, top2-1) : 0;

	if ((top2_val < 0 && (top2_val & (X40+X20+X10))) || is_fill_fs_on_start_symbol) {
	  /* fs_id est cyclique, partage' ou non clos, on l'instancie */
	  /* on regarde s'il a deja ete reference' par $right_dol */
	  adjunct_ptr->kind = ADJUNCT_KIND_WFS;

	  if (XxY_set (&XxY_dol_fs_id, right_dol, fs_id, &dol_fs_id) && (new_wfs_id = dol_fs_id2wfs_id [dol_fs_id])) {
	    /* deja utilise' */
	    adjunct_ptr->id = new_wfs_id = get_equiv (new_wfs_id);

	    /* COMME CI-DESSOUS si new_wfs_id non instancie' */
	    if (wfs2attr [AREA_PAGE (new_wfs_id)] [AREA_DELTA (new_wfs_id)] [0] > 0)
	      instantiate_wfs_id (new_wfs_id);
	  }
	  else {
	    /* 1ere utilisation */
	    adjunct_ptr->id = new_wfs_id = dol_fs_id2wfs_id [dol_fs_id] = get_a_new_wfs_id ();
	    /* PB, on n'a pas de path, on en cree un bidon */

	    new_path = field_id_path_catenate (-pos /* field_id */, field_id_path /* prev_path */, fs_id, right_dol);
	    /* Il doit etre obligatoirement nouveau !! */
	    wfs2attr [AREA_PAGE (new_wfs_id)] [AREA_DELTA (new_wfs_id)] [0] = new_path;
	    
	    path2wfs_id [new_path].wfs_id = new_wfs_id;

	    /* FAIRE UN APPEL A instantiate_wfs_id () car il faut instancier tous les fils qui en ont besoin
	       ATTENTION, cet appel peut etre recursif.  Est-ce que instantiate_wfs_id () le supporte ?? */
	    instantiate_wfs_id (new_wfs_id);
	  }
	}
	else {
	  /* On laisse fs_id */
	  adjunct_ptr->id = fs_id;
	  adjunct_ptr->kind = ADJUNCT_KIND_FS;
	}

	adjunct_ptr = NULL;
      }
    }
  }
  else {
    /* on met l'ufs_id */
    adjunct_ptr->id = ufs_id;
    adjunct_ptr->kind = ADJUNCT_KIND_UFS;
  }

  return adjunct_set_id;
}


/* On instantie un fs_id en father_wfs_id */
static void
instantiate_wfs_id (father_wfs_id)
     int father_wfs_id;
{
  int                   fs_id, right_dol, page_nb, delta_nb, bot, top, top_val, cur, cur_val, field_id;
  int                   son_fs_id, son_wfs_id, father_field_id_path, son_field_id_path, fs_id_dol;
  int                   son_bot, son_top, son_top_val, access_kind;
  int                   *attr_ptr0, *attr_ptr;
  unsigned char         static_field_kind; 
  SXBA                  field_set;
  SXBOOLEAN               same_path, is_new_fs_id_dol;

#if EBUG
  if (father_wfs_id < MAIN_WFS_ID || father_wfs_id > wfs_id)
    sxtrap (ME, "instantiate_wfs_id");
#endif /* EBUG */

  page_nb = AREA_PAGE (father_wfs_id);
  delta_nb = AREA_DELTA (father_wfs_id);
  attr_ptr0 = wfs2attr [page_nb] [delta_nb];

  father_field_id_path = *attr_ptr0;

#if EBUG
  if (father_field_id_path <= 0)
    sxtrap (ME, "instantiate_wfs_id");
#endif /* EBUG */

  fs_id_dol = path2wfs_id [father_field_id_path].fs_id_dol;

#if EBUG
  if (fs_id_dol == 0 || path2wfs_id [father_field_id_path].wfs_id != father_wfs_id)
    sxtrap (ME, "instantiate_wfs_id");
#endif /* EBUG */

  fs_id = fs_id_dol >> MAXRHS_SHIFT;
  right_dol = fs_id_dol & MAXRHS_AND;
  
  /* la copie locale de fs_id récupère son dynweight */
  wfs_id2localdynweight[father_wfs_id] = fs_id2localdynweight[fs_id];

  /* Non sensible aux reallocations de wfs_field_set !! */
  field_set = wfs_field_set [page_nb] [delta_nb];
      
  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);

  top_val = (bot < top) ? XH_list_elem (XH_fs_hd, top-1) : 0;

  *attr_ptr0 = 0; /* instancie' */

  if (top_val < 0 /* non standard */) {
    top--;
  }

  for (cur = bot; cur < top; cur++) {
    cur_val = XH_list_elem (XH_fs_hd, cur);
    son_fs_id = cur_val >> FS_ID_SHIFT;

    field_id =  cur_val & FIELD_AND;

    if (!is_executable (field_id))
      /* Ce champ n'est pas calcule' ds cette passe */
      continue;

    if (is_fill_fs_on_start_symbol && (cur_val & OPTIONAL_VAL)) {
      /* Quand on traite des (sous-)structures associées à l'axiome, on élimine les =? */
      SXBA_0_bit (field_set, field_id);
      continue;
    }

    if (SXBA_bit_is_reset_set (field_set, field_id)) {
      /* Modif du 26/05/05 (certains champs peuvent deja etre positionnes) */
      static_field_kind = field_id2kind [field_id];
      attr_ptr = attr_ptr0+field_id;

      son_field_id_path = field_id_path_catenate (field_id, father_field_id_path, son_fs_id, right_dol);
      son_wfs_id = path2wfs_id [son_field_id_path].wfs_id;

      if (static_field_kind == STRUCT_KIND) {
	/* C'est une structure qu'il faut recopier de fs ds wfs */
	/* son_fs_id est ds fs */ 
	if (son_wfs_id == 0) {
	  /* C'est la 1ere fois qu'on utilise ce chemin de champs */
	  son_wfs_id = instantiate_fs_id (son_fs_id, right_dol, son_field_id_path);
	  /* si on est à la racine, cette instantiation sera récursive */	  
	}
      }
      else {
	/* la valeur retournee access_kind est inutilisee */
	/* C'est un index ds field_val2adjunct_id */
	if (son_wfs_id == 0) {
	  son_wfs_id = instantiate_ufs_id (field_id, son_field_id_path/*, father_wfs_id */, &access_kind);
	  /* si on est à la racine, cette instantiation sera récursive */

	}
      }

      *attr_ptr = (son_wfs_id << FS_ID_SHIFT) + (cur_val & FS_ID_AND);
    }
  }
}


/* Retourne vrai ssi une de ses sous-structure est (reellement) instanciee et a donc pu changer */
/* Le 14/10/05 nelle version : la memoization ne peut pas utiliser XxY_dol_fs_id */
/* Le 28/07/06 nelle version : changement sur le traitement des cycles */
static SXBOOLEAN
must_we_instantiate_wfs_id (right_dol, fs_id)
     int right_dol, fs_id;
{
  int                   memo_dol_fs_id, dol_fs_id, bot, top, cur, cur_val, field_id;
  int                   son_fs_id, son_ufs_id, cur_wfs_id, field_id_path;
  int                   cur2, bot2, top2;
  unsigned char         static_field_kind; 
  SXBOOLEAN               ret_val;
      
  /* Attention il faut detecter les cas cycliques */
  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);

  if (bot == top || XH_list_elem (XH_fs_hd, top-1) > 0)
    /* fs_is standard (non cyclique, clos, ...) */
    return SXFALSE;

  if (XxY_set (&XxY_memo_dol_fs_id, right_dol, fs_id, &memo_dol_fs_id)) {
    /* On a deja trouve' (right_dol, fs_id) ds must_we_instantiate_wfs_id */
    /* est-on ds le cas de la detection d'un cycle ? */
    if (SXBA_bit_is_set (cyclic_dol_fs_id_set, memo_dol_fs_id)) {
      /* oui */
      return SXFALSE; /* reponse inconnue, la bonne sera connue a la sortie du cycle */
    }

    return SXBA_bit_is_set (memo_dol_fs_id_set, memo_dol_fs_id);
  }
  else {
    /* 1ere utilisation */
    if (dol_fs_id = XxY_is_set (&XxY_dol_fs_id, right_dol, fs_id)) {
      /* Un wfs_id a deja ete assigne' par le traitement des e'quations fonctionnelles */
      cur_wfs_id = dol_fs_id2wfs_id [dol_fs_id];
      cur_wfs_id = get_equiv (cur_wfs_id);

      field_id_path = wfs2attr [AREA_PAGE (cur_wfs_id)] [AREA_DELTA (cur_wfs_id)] [0];

      if (field_id_path == 0) {
	/* instancie' (et donc susceptible d'avoir ete modifie) */
	SXBA_0_bit (cyclic_dol_fs_id_set, memo_dol_fs_id); /* non cyclique */
	SXBA_1_bit (memo_dol_fs_id_set, memo_dol_fs_id); /* instancie' */
	return SXTRUE;
      }
    }

    SXBA_1_bit (cyclic_dol_fs_id_set, memo_dol_fs_id); /* On commence la visite de dol_fs_id et de ses fils */
  }
    
  top--;
  ret_val = SXFALSE;

  for (cur = bot; cur < top; cur++) {
    cur_val = XH_list_elem (XH_fs_hd, cur);
    field_id =  cur_val & FIELD_AND;

    if (!is_executable (field_id))
      /* Ce champ n'est pas calcule' ds cette passe */
      continue;

    static_field_kind = field_id2kind [field_id];

    if (static_field_kind & STRUCT_KIND) {
      son_fs_id = cur_val >> FS_ID_SHIFT;

      if (son_fs_id
	  && son_fs_id != fs_id //// BS : vraiment??? attention aux cycles plus longs...
	  ) {
	if (static_field_kind == STRUCT_KIND) {
	  if (must_we_instantiate_wfs_id (right_dol, son_fs_id)) {
	    ret_val = SXTRUE;
	    break;
	  }
	}
	else {
	  son_ufs_id = son_fs_id;

	  for (cur2 = bot2 = XH_X (XH_ufs_hd, son_ufs_id), top2 = XH_X (XH_ufs_hd, son_ufs_id+1);
	       cur2 < top2;
	       cur2++) {
	    son_fs_id = XH_list_elem (XH_ufs_hd, cur2);

	    if (son_fs_id < 0)
	      break;
	  
	    if (must_we_instantiate_wfs_id (right_dol, son_fs_id)) {
	      ret_val = SXTRUE;
	      break;
	    }
	  }
	  if (ret_val)
	    break;
	}
      }
    }
  }

  /* On memoize la valeur calculee */
  if (ret_val)
    SXBA_1_bit (memo_dol_fs_id_set, memo_dol_fs_id);
  else
    SXBA_0_bit (memo_dol_fs_id_set, memo_dol_fs_id);

  SXBA_0_bit (cyclic_dol_fs_id_set, memo_dol_fs_id); /* On a termine' la visite de dol_fs_id et de ses fils */

  return ret_val;
}



#if 0
/* Retourne vrai ssi une de ses sous-structure est (reellement) instanciee et a donc pu changer */
/* Le 14/10/05 nelle version : la memoization ne peut pas utiliser XxY_dol_fs_id */
static SXBOOLEAN
must_we_instantiate_wfs_id (right_dol, fs_id)
     int right_dol, fs_id;
{
  int                   dol_fs_id, bot, top, cur, cur_val, field_id;
  int                   son_fs_id, son_ufs_id, cur_wfs_id, field_id_path;
  int                   cur2, bot2, top2;
  unsigned char         static_field_kind; 
  SXBOOLEAN               ret_val, memo_case;
      
  /* Attention il faut detecter les cas cycliques */
  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);

  if (bot == top || XH_list_elem (XH_fs_hd, top-1) > 0)
    return SXFALSE;

  if (dol_fs_id = XxY_is_set (&XxY_dol_fs_id, right_dol, fs_id)) {
    /* deja utilise' */
    cur_wfs_id = dol_fs_id2wfs_id [dol_fs_id];
    cur_wfs_id = get_equiv (cur_wfs_id);

    field_id_path = wfs2attr [AREA_PAGE (cur_wfs_id)] [AREA_DELTA (cur_wfs_id)] [0];

    if (field_id_path == 0)
      /* instancie' (et donc susceptible d'avoir ete modifie) */
      return SXTRUE;
    
    memo_case = SXFALSE;
  }
  else {
    /* 1ere utilisation de (right_dol, fs_id) */
    if (XxY_set (&XxY_memo_dol_fs_id, right_dol, fs_id, &dol_fs_id)) {
      /* On a deja trouve' (right_dol, fs_id) ds must */
      return SXBA_bit_is_set (memo_dol_fs_id_set, dol_fs_id);
    }

    SXBA_0_bit (memo_dol_fs_id_set, dol_fs_id); /* Pour la detection des cycles */
    memo_case = SXTRUE;
  }
    
  top--;
  ret_val = SXFALSE;

  for (cur = bot; cur < top; cur++) {
    cur_val = XH_list_elem (XH_fs_hd, cur);
    field_id =  cur_val & FIELD_AND;

    if (!is_executable (field_id))
      /* Ce champ n'est pas calcule' ds cette passe */
      continue;

    static_field_kind = field_id2kind [field_id];

    if (static_field_kind & STRUCT_KIND) {
      son_fs_id = cur_val >> FS_ID_SHIFT;

      if (son_fs_id
	  && son_fs_id != fs_id //// BS : vraiment??? attention aux cycles plus longs...
	  ) {
	if (static_field_kind == STRUCT_KIND) {
	  if (must_we_instantiate_wfs_id (right_dol, son_fs_id)) {
	    ret_val = SXTRUE;
	    break;
	  }
	}
	else {
	  son_ufs_id = son_fs_id;

	  for (cur2 = bot2 = XH_X (XH_ufs_hd, son_ufs_id), top2 = XH_X (XH_ufs_hd, son_ufs_id+1);
	       cur2 < top2;
	       cur2++) {
	    son_fs_id = XH_list_elem (XH_ufs_hd, cur2);

	    if (son_fs_id < 0)
	      break;
	  
	    if (must_we_instantiate_wfs_id (right_dol, son_fs_id)) {
	      ret_val = SXTRUE;
	      break;
	    }
	  }
	}
      }
    }
  }

  if (ret_val && memo_case) {
    SXBA_1_bit (memo_dol_fs_id_set, dol_fs_id); /* memoization de SXTRUE */
  }

  return ret_val;
}
#endif /* 0 */



static SXBOOLEAN is_constrained_unification, is_optional_unification;

/* On unifie les ensembles de f_structures (adjuncts) reperes par left_val_ptr et right_val_ptr */
/* ATTENTION, ca doit aussi marcher pour les aij !! ... */
/* ... et pour les atomes */
/* 2 ufs peuvent etre partages */
/* PB : si l'un est modifie', l'autre doit l'etre egalement or il a pu ne pas etre instancie' !! */
/* Si 2 ufs sont egales et non partagees, elles doivent s'instancier a des endroits differents
   sauf si elles ont des chemins de champs d'acces identiques */
/* Version du 30/05/05 */
static SXBOOLEAN
unbounded_structure_unification (left_result, right_result)
     int     left_result, right_result;
{
  int                     left_id, right_id, right_top, left_top, right_field, prev_top;
  int                     left_field_id, right_field_id, right_val, left_val, val;
  int                     *dpv_lptr, *dpv_rptr;
  struct adjunct_set_item *left_adjunct_ptr, *right_adjunct_ptr, *adjunct_ptr;
  unsigned char           left_static_field_kind;
  int                     *left_val_ptr, *right_val_ptr;
  SXBOOLEAN                 is_shared;
#ifdef ESSAI 
  int                     next, old_next, new_top;
#endif /* ESSAI */

  left_val_ptr = re_results [left_result].ptr;
  left_top = get_adjunct_equiv (*left_val_ptr >> FS_ID_SHIFT);
  /* left_top est un indice ds field_val2adjunct_id ... */

  right_val_ptr = re_results [right_result].ptr;
  right_top = get_adjunct_equiv (*right_val_ptr >> FS_ID_SHIFT);
  /* right_top est un indice ds field_val2adjunct_id ... */

#if EBUG
  /* ... non nuls */
  if (left_top == 0 || right_top == 0)
    sxtrap (ME, "unbounded_structure_unification");
#endif /* EBUG */

  if (left_top == right_top)
    /* !! */
    return SXTRUE;
  
  left_adjunct_ptr = adjunct_sets + field_val2adjunct_id [left_top].adjunct_set_id;
  right_adjunct_ptr = adjunct_sets + field_val2adjunct_id [right_top].adjunct_set_id;

  right_field = *right_val_ptr & FS_ID_AND;

  if (left_adjunct_ptr == right_adjunct_ptr) {
    /* !! */
    *right_val_ptr = (left_top << FS_ID_SHIFT) + (right_field);
    field_val2adjunct_id [right_top].equiv = left_top; /* droit devient equivalents a gauche */
    
    return SXTRUE;
  }
    
  if (right_adjunct_ptr->kind == ADJUNCT_KIND_EMPTY) {
    /* Ca marche aussi si les 2 sont vides */
    *right_val_ptr = (left_top << FS_ID_SHIFT) + (right_field);
    field_val2adjunct_id [right_top].equiv = left_top; /* droit devient equivalents a gauche */
    
    return SXTRUE;
  }

  if (left_adjunct_ptr->kind == ADJUNCT_KIND_EMPTY) {
    *left_val_ptr = (right_top << FS_ID_SHIFT) + (*left_val_ptr & FS_ID_AND);
    field_val2adjunct_id [left_top].equiv = right_top; /* gauche devient equivalents a droit */

    return SXTRUE;
  }

  /* Ici l'operande gauche et l'operande droit sont non vides */

  left_field_id = (*left_val_ptr & FIELD_AND);
  left_static_field_kind = field_id2kind [left_field_id];

  right_field_id = (*right_val_ptr & FIELD_AND);

  if (left_static_field_kind == ATOM_KIND) {
    /* Cas de l'unification de valeurs atomiques */
    /* (left|right)_adjunct_ptr sont positionnes */
    /* Le 18/04/05 on n'instancie pas les UFS (on manipule la valeur de l'atome)
       Ca permet ds la majorite des cas, a ne pas avoir a recalculer le XH_set correspondant */
    left_val = left_adjunct_ptr->id;
    right_val = right_adjunct_ptr->id;

    if (left_adjunct_ptr->kind == ADJUNCT_KIND_UFS &&
	right_adjunct_ptr->kind == ADJUNCT_KIND_UFS) {

#ifdef ESSAI
#if EBUG
      if (left_adjunct_ptr->next || right_adjunct_ptr->next)
	/* Sinon, c'est une liste de ADJUNCT_KIND_ATOM */
	sxtrap (ME, "unbounded_structure_unification");
#endif /* EBUG */
#endif /* ESSAI */

      if (left_val == right_val) {
	/* meme ufs_id, l'unif reussit */
	*right_val_ptr = (left_top << FS_ID_SHIFT) + right_field;
	field_val2adjunct_id [right_top].equiv = left_top; /* droit devient equivalents a gauche */
    
	return SXTRUE;
      }
    }
      
    if (right_adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
      /* On "recupere" la valeur */
#if EBUG
      if (right_val == 0)
	sxtrap (ME, "unbounded_structure_unification");
#endif /* EBUG */

#ifndef ESSAI
      right_val = get_ufs_atom_val (right_val, &is_shared);
#else /* ESSAI */
      right_val = get_ufs_atom_val (right_val, &is_shared, right_local_atom_id_set);
#endif /* ESSAI */
      /* On peut avoir right_val == 0 */
    }
    else {
#ifndef ESSAI
#if 0
      /* right_val est OK */
      if (right_adjunct_ptr->kind == ADJUNCT_KIND_ATOM)
	right_val = right_val;
      else
	/* right_adjunct_ptr->kind == ADJUNCT_KIND_EMPTY */
	right_val = 0;
#endif /* 0 */
#else /* ESSAI */

      if (right_val) {
	SXBA_1_bit (right_local_atom_id_set, right_val);

	adjunct_ptr = right_adjunct_ptr;
	while (next = adjunct_ptr->next) {
	  adjunct_ptr = adjunct_sets + next;
	  val = adjunct_ptr->id;
	  SXBA_1_bit (right_local_atom_id_set, val);
	}
      }
#endif /* ESSAI */
    }

    if (left_adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
      /* On "instancie" la valeur */
#if EBUG
      if (left_val == 0)
	sxtrap (ME, "unbounded_structure_unification");
#endif /* EBUG */

#ifndef ESSAI
      left_val = get_ufs_atom_val (left_val, &is_shared);
#else /* ESSAI */
      left_val = get_ufs_atom_val (left_val, &is_shared, left_local_atom_id_set);
#endif /* ESSAI */
      /* On peut avoir left_val == 0 */
    }
    else {
#ifndef ESSAI
#if 0
      /* left_val est OK */
      if (left_adjunct_ptr->kind == ADJUNCT_KIND_ATOM)
	left_val = left_val;
      else
	/* left_adjunct_ptr->kind == ADJUNCT_KIND_EMPTY */
	left_val = 0;
#endif /* 0 */
#else /* ESSAI */
      if (left_val) {
	SXBA_1_bit (left_local_atom_id_set, left_val);

	adjunct_ptr = left_adjunct_ptr;
	while (next = adjunct_ptr->next) {
	  adjunct_ptr = adjunct_sets + next;
	  val = adjunct_ptr->id;
	  SXBA_1_bit (left_local_atom_id_set, val);
	}
      }
#endif /* ESSAI */
    }

    if (left_field_id == pred_id) {
      /* l'unification doit echouer entre 2 pred (meme identiques) */
      /* Modif le 06/04/04 apre`s discussion avec Lionel :
	 On peut faire l'unification uniquement si (au moins) l'un des pred est contraint
	 l'unification reussit si les 2 lexemes sont identiques et si l'intersection des sous_cat
	 est non vide.  Exception si une sous-cat est vide, elle herite de l'autre.
	 A FAIRE (peut e^tre) prévoir l'extension pred = "lexeme <>" qui ne pourrait s'unifier
	 qu'avec une sous-cat vide et qui serait donc differente de pred = "lexeme" qui pourrait
	 s'unifier avec une sous-cat qqc */
#ifdef ESSAI
      /* Au plus unique !! */
      SXBA_0_bit (left_local_atom_id_set, left_val);
      SXBA_0_bit (right_local_atom_id_set, right_val);
#endif /* ESSAI */

      if (left_val && right_val && ((*left_val_ptr | *right_val_ptr) & CONSTRAINT_VAL)) {
	/* Au moins l'un des pred est contraint */
	dpv_lptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, left_val)));
	dpv_rptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, right_val)));

	if (dpv_lptr [0] == dpv_rptr [0]) {
	  if (dpv_rptr [1] == 0 && dpv_rptr [2] == 0) {
	    /* left est bon, il faut encore changer le champ contrainte */
	  }
	  else {
	    if (dpv_lptr [1] == 0 && dpv_rptr [2] == 0) {
	      /* c'est right qui est bon */
	      left_val = right_val;
	    }
	    else {
	      /* cas general, il faut unifier les sous_cat */
	      /* pour l'instant, on dit que ca echoue (que faire des options, des disjonctions ?) */
	      left_val = -1;
	    }
	  }
	}
	else
	  /* lexeme differents => echec */
	  left_val = -1;
      }
      else
	left_val = -1;
    }
    else {
      if (left_val == 0 && right_val == 0) {
	/* Les 2 vides => OK */
      }
      else {
	if (right_val == 0) {
	  /* => OK */
	  left_val = 0;
	  sxba_empty (left_local_atom_id_set);
	}
	else {
	  if (left_val == 0) {
	    /* OK */
	    *left_adjunct_ptr = *right_adjunct_ptr;
	  }
	  else {
#ifndef ESSAI
	    if (left_static_field_kind & UNBOUNDED_KIND) {
	      /* L'unification est l'egalite */
	      if (left_val != right_val)
		left_val = -1;
	    }
	    else {
	      /* L'unification est l'intersection des valeurs atomiques */
	      left_val &= right_val;

	      if (left_val == 0)
		/* echec */
		left_val = -1;
	    }
#else /* ESSAI */
	    /* L'unification est l'intersection des valeurs atomiques */
	    if (sxba_2op (left_local_atom_id_set, SXBA_OP_COPY, left_local_atom_id_set, SXBA_OP_AND, right_local_atom_id_set)) {
	      val = sxba_scan (left_local_atom_id_set, 0);
	      /* On reutilise left_top et ses "next" => gestion + aisee des equiv et economies */
	      adjunct_ptr = left_adjunct_ptr;
	      adjunct_ptr->id = val;
	      adjunct_ptr->kind = ADJUNCT_KIND_ATOM;
	      old_next = adjunct_ptr->next;
	      adjunct_ptr->next = 0;

	      while ((val = sxba_scan (left_local_atom_id_set, val)) > 0) {
		if (old_next) {
		  new_top = old_next;
		  old_next = adjunct_sets [new_top].next;
		}
		else
		  new_top = get_new_top_adjunct_sets ();

		adjunct_ptr->next = new_top;
		adjunct_ptr = adjunct_sets+new_top;
		adjunct_ptr->id = val;
		adjunct_ptr->kind = ADJUNCT_KIND_ATOM;
		adjunct_ptr->next = 0;
	      }

	      left_val = 0;
	    }
	    else {
	      /* echec => left_local_atom_id_set est vide */
	      left_val = -1;
	    }
#endif /* ESSAI */
	  }

#ifdef ESSAI
	  sxba_empty (left_local_atom_id_set);
#endif /* ESSAI */
	}

#ifdef ESSAI
	if (right_val)
	  sxba_empty (right_local_atom_id_set);
#endif /* ESSAI */
      }
    }

    if (left_val >= 0) {
      /* C'est la lhs qui recupere le resultat de l'unification ... */
      if (left_val > 0) {
	left_adjunct_ptr->id = left_val;
	left_adjunct_ptr->kind = ADJUNCT_KIND_ATOM;
      }
      
      *right_val_ptr = (left_top << FS_ID_SHIFT) + right_field;
      field_val2adjunct_id [right_top].equiv = left_top; /* droit devient equivalents a gauche */
    }

    return left_val >= 0;
  }

  /* Ici, on manipule de vrais ensembles adjuncts ou Aij ... */
  /* L'unification est la concatenation !! */
  *right_val_ptr = (left_top << FS_ID_SHIFT) + right_field;
  field_val2adjunct_id [right_top].equiv = left_top; /* droit devient equivalents a gauche */

  prev_top = left_adjunct_ptr-adjunct_sets;

  while (prev_top) {
    left_adjunct_ptr = adjunct_sets + prev_top;
    prev_top = left_adjunct_ptr->next;
  }

  /* On concatene ... */
  left_adjunct_ptr->next = right_adjunct_ptr-adjunct_sets;

  return SXTRUE;
}




/* On unifie les structures left_wfs_id et right_wfs_id dans left_wfs_id */
/* left_wfs_id est le representant de la classe d'equivalence.
   Il en est de meme de right_wfs_id si right_dol==0 */
/* Le 15/02/05 le 1er arg left_wfs_id devient left_wfs_id_ptr car il est possible que structure_unification ()
   change le wfs_id ds lequel se fait l'unification ds le cas de structures cycliques */
/* Le 17/02/05 ajout de right_field_id_path qui repere, si right_dol > 0 le chemin X* de $i X* */
/* Le 27/05/05 On peut modifier les fils et donc avoir du $i en lhs */
static SXBOOLEAN
structure_unification (left_result, right_result)
     int     left_result, right_result;
{
  int                   left_wfs_id, right_wfs_id, field_id, field_kind, static_field_kind;
  int                   left_page_nb, left_delta_nb, right_page_nb, right_delta_nb, left_result_id, right_result_id;
  int                   new_son_nb, adjunct_set_id;
  int                   *left_attr_ptr0, *left_attr_ptr, *right_attr_ptr0, *right_attr_ptr;
  SXBA                  left_field_set, right_field_set;
  SXBOOLEAN               ret_val;
  struct re_result      *re_result_left_ptr, *re_result_right_ptr, *left_result_ptr, *right_result_ptr;

  /* Ds tous les cas re_results [(left_ | right_)result].ptr >> FS_ID_SHIFT designe un wfs_id */
  /* Si le chemin est de la forme $$ X*, son wfs_id s'obtient par *(re_result_left_ptr->ptr) >> FS_ID_SHIFT */

  re_result_left_ptr = re_results+left_result;
  re_result_right_ptr = re_results+right_result;

  left_wfs_id = *(re_result_left_ptr->ptr) >> FS_ID_SHIFT;
  left_wfs_id = get_equiv (left_wfs_id); /* En fait on est la` !! */

  right_wfs_id = *(re_result_right_ptr->ptr) >> FS_ID_SHIFT;
  right_wfs_id = get_equiv (right_wfs_id); /* En fait on est la` !! */

#if EBUG
  if (left_wfs_id < MAIN_WFS_ID || left_wfs_id > wfs_id || right_wfs_id < MAIN_WFS_ID || right_wfs_id > wfs_id )
    sxtrap (ME, "structure_unification");
#endif /* EBUG */

  if (left_wfs_id == right_wfs_id)
    return SXTRUE;

  /* Le 09/08/05 */
  if (right_wfs_id == MAIN_WFS_ID) {
    /* MAIN_WFS_ID doit rester le principal => on echange gauche-droit */
    struct re_result      *re_result_ptr;

    right_wfs_id = left_wfs_id;
    left_wfs_id = MAIN_WFS_ID;
    re_result_ptr = re_result_left_ptr;
    re_result_left_ptr = re_result_right_ptr;
    re_result_right_ptr = re_result_ptr;
  }

  left_page_nb = AREA_PAGE (left_wfs_id);
  left_delta_nb = AREA_DELTA (left_wfs_id);
  left_attr_ptr0 = wfs2attr [left_page_nb] [left_delta_nb];

  right_page_nb = AREA_PAGE (right_wfs_id);
  right_delta_nb = AREA_DELTA (right_wfs_id);
  right_attr_ptr0 = wfs2attr [right_page_nb] [right_delta_nb];
    
  if (*left_attr_ptr0 == -1 || *right_attr_ptr0 == -1) {
    /* La partie gauche ou la partie droite (ou les 2) est de la forme $$ X* et $$ X* et est nouveau */
    /* On choisit un des 2 comme etant le representant */
    /* On done priorite a la partie gauche (pourquoi pas */
    if (*left_attr_ptr0 == -1 && *right_attr_ptr0 == -1) {
      /* priorite a gauche */
      wfs_id2equiv [right_wfs_id] = left_wfs_id;

      return SXTRUE;
    }
	
    if (*left_attr_ptr0 == -1) {
      /* La gauche est non calculee ... */
      if (left_wfs_id != MAIN_WFS_ID) {
	/* priorite a droite */
	wfs_id2equiv [left_wfs_id] = right_wfs_id;
	return SXTRUE;
      }
	    
      /* si c'est le main */
      /* Il faut faire une vraie unif car meme si *right_attr_ptr0 est positif, elle peut deja contenir des chemins instancies */
    }
    else {
      /* La droite est non calculee ... */
      /* priorite a gauche */
      wfs_id2equiv [right_wfs_id] = left_wfs_id;

      return SXTRUE;
    }
  }
      
  /* On pourrait regarder si par hasard left et right ne designent pas le meme fs_id ?? A priori peu frequent
     pour l'instant on les unifie */

  if (*left_attr_ptr0 > 0) {
    /* non instancie', on le fait ... */
    instantiate_wfs_id (left_wfs_id);
  }
  else
    /* Ca pouvait etre -1 */
    *left_attr_ptr0 = 0;
      
  /* On unifie donc right_wfs_id ds left_wfs_id */
  left_field_set = wfs_field_set [left_page_nb] [left_delta_nb];
  right_field_set = wfs_field_set [right_page_nb] [right_delta_nb];

  if (*right_attr_ptr0 > 0) {
    /* non instancié: on instancie */
    instantiate_wfs_id (right_wfs_id);

    /* on donne à la structure locale de p.g. le dynweight du représentant de la classe d'équiv de celle de p.d. */
    wfs_id2localdynweight[left_wfs_id] += wfs_id2localdynweight[get_equiv(right_wfs_id)];
  }
  
  /* et droite devient equivalente a gauche */
  wfs_id2equiv [right_wfs_id] = left_wfs_id;

  left_result_ptr = NULL; /* non initialise' */
  ret_val = SXTRUE;
  field_id = 0;

  while ((field_id = sxba_scan (right_field_set, field_id)) > 0) {
#if EBUG
    /* Les instanciations ont deja fait cette verif */
    if (!is_executable (field_id))
      /* Ce champ n'est pas calcule' ds cette passe */
      sxtrap (ME, "structure_unification");
#endif /* EBUG */

    static_field_kind = field_id2kind [field_id];
    left_attr_ptr = left_attr_ptr0+field_id;
    right_attr_ptr = right_attr_ptr0+field_id;

    if (SXBA_bit_is_reset_set (left_field_set, field_id)) {
      /* field_id est a droite et pas a gauche */
      /* Attention a la gestion des pred/son_nb */
      /* La pred des [unbounded_]f_structures reperees par field_id qui pointe vers right_wfs_id
	 peut continuer a le faire (on accedera a left_wfs_id par les equiv */
      /* En revanche il faut mettre a jour wfs_id2son_nb [left_wfs_id] */
#if 0 /* on est dans structure_unification, alors que les lignes ci-dessous ne seraient pertinentes qu'avec une val atomique en partie droite */
      if (!is_constrained_unification) {
	/* resultat non contraint */
	*right_attr_ptr &= ~CONSTRAINT_VAL;
      }
#endif /* 0 */
      *left_attr_ptr = *right_attr_ptr;
    }
    else {
      /* field_id est a gauche et a droite */
#if EBUG
      if ((*left_attr_ptr >> FS_ID_SHIFT) == 0 || (*right_attr_ptr >> FS_ID_SHIFT) == 0) {
	/* On suppose que ca veut dire que cette structure est vide */
	printf ("System error : $$ ... %s is null!!\n", field_id2string [field_id]);
	sxtrap (ME, "structure_unification");
      }
#endif /* EBUG */

      field_kind = ((static_field_kind & STRUCT_KIND)
		    ? (static_field_kind & UNBOUNDED_KIND
		       ? UNBOUNDED_STRUCT_FIELD
		       : STRUCT_FIELD)
		    : ATOM_FIELD) + /* (right_dol << REF_SHIFT) + car right_dol==0 */ OLD_FIELD;

      if (left_result_ptr == NULL) {
	/* initialisation independante du field_id */
	left_result_id = re_results_get_new_top ();
	right_result_id = re_results_get_new_top ();

	left_result_ptr = re_results + left_result_id;
	left_result_ptr->father_wfs_id = left_wfs_id;
	right_result_ptr = re_results + right_result_id;
	right_result_ptr->father_wfs_id = right_wfs_id;
      }

      left_result_ptr->ptr = left_attr_ptr;
      left_result_ptr->kind = field_kind;
	  
      right_result_ptr->ptr = right_attr_ptr;
      right_result_ptr->kind = field_kind;
	  
      if (static_field_kind == STRUCT_KIND) {
	if (!structure_unification (left_result_id, right_result_id))
	  ret_val = SXFALSE;
      }
      else {
	if (!unbounded_structure_unification (left_result_id, right_result_id))
	  ret_val = SXFALSE;
      }

      if (!ret_val)
	break;

      /* on est dans structure_unification: pas de is_constrained_unification possible ;
	 on doit ne laisser =c que ssi il y a =c des deux côtés */
      if ((*left_attr_ptr & CONSTRAINT_VAL) && (*right_attr_ptr & CONSTRAINT_VAL))
	;
      else {
	*left_attr_ptr &= ~CONSTRAINT_VAL;
	*right_attr_ptr &= ~CONSTRAINT_VAL;  // peut-être facultatif car on n'y touche pê plus jamais... ne peut pas faire de mal
      }

      /* on est dans structure_unification: pas de is_optional_unification possible ;
	 on doit ne laisser =c que ssi il y a =c des deux côtés */
      if ((*left_attr_ptr & OPTIONAL_VAL) && (*right_attr_ptr & OPTIONAL_VAL))
	;
      else {
	*left_attr_ptr &= ~OPTIONAL_VAL;
	*right_attr_ptr &= ~OPTIONAL_VAL;  // peut-être facultatif car on n'y touche pê plus jamais... ne peut pas faire de mal
      }

#if 0 // ancien passage à la place de ce qui précède: mauvais
      if (is_constrained_unification) {
	SXBOOLEAN is_left_c, is_right_c;

	is_left_c = (*left_attr_ptr & CONSTRAINT_VAL); /* left est contraint */
	is_right_c = (*right_attr_ptr & CONSTRAINT_VAL); /* right est contraint */
  
	if (is_left_c + is_right_c == 1 /* is_left_c && !is_right_c || !is_left_c && is_right_c */) {
	  if (is_left_c)
	    *left_attr_ptr &= ~CONSTRAINT_VAL;
	  else
	    *right_attr_ptr &= ~CONSTRAINT_VAL;
	}
      }
      else {
	/* => non contraints */
	*left_attr_ptr &= ~CONSTRAINT_VAL;
	*right_attr_ptr &= ~CONSTRAINT_VAL;
      }
#endif /* 0 */
    }
  }

  return ret_val;
}




/* Il peut y avoir des adjunct cycliques :
   $$ subj < $$ subj adjunct => [subj = [adjunct = {..., []Fk, ...}]Fk ]
   ou des adjunct partages :
   [subj = []Fk, adjunct = {..., []Fk, ...}]
*/

/* Il est donc possible de "modifier" le contenu d'un adjunct en accedant a subj */
/* Il est meme possible qu'une f_struct d'un adjunct qui etait partagee redevienne "normale"
   Par exemple si la struct precedente est en $1, l'equation $$ adjunct = $1 adjunct doit
   rendre Fk non partagee */

/* Ce qui veut dire que lorsqu'on instancie un adjunct, ses f_structures partagees/cycliques
   doivent etre pre'_instanciees ds un wfs_id pour e^tre reintegrees ds le re'sultat apre`s
   modif eventuelle */

/* Cas $i X* < $$ Y+ */
/* Version du 31/05/05 */
static SXBOOLEAN
unify_in (left_result, right_result)
     int     left_result, right_result;
{
  int                     top, right_id, field_id, left_dol, kind, new_top, prev_top;
  struct adjunct_set_item *adjunct_ptr;
     
  int                     right_kind, left_field_id_path, left_wfs_id, left_kind;
  int                     *left_val_ptr, *right_val_ptr;
  struct re_result        *re_result_left_ptr, *re_result_right_ptr;

  re_result_left_ptr = re_results+left_result;
  re_result_right_ptr = re_results+right_result;

  left_val_ptr = re_result_left_ptr->ptr;
  left_kind = re_result_left_ptr->kind;

  right_val_ptr = re_result_right_ptr->ptr;
  right_kind = re_result_right_ptr->kind;

  field_id = (*right_val_ptr & FS_ID_AND);
  right_id = (*right_val_ptr >> FS_ID_SHIFT);

  left_dol = left_kind>>REF_SHIFT;

#if EBUG
  if (right_kind & DOL_REF)
    /* Pas de $$ ou $i en rhs */
    sxtrap (ME, "unify_in");

  /* Pas de structure vide provenant d'un fils */
  if ((left_kind & NEW_FIELD) && left_dol)
    sxtrap (ME, "unify_in");
#endif /* EBUG */

  if (left_dol) {
    /* $i X* < $[$|i] Y* adjunct */
    /* La lhs est une f_structure */
    left_wfs_id = path2wfs_id [re_result_left_ptr->path_id].wfs_id;

#if EBUG
    if ((left_kind & DOL_REF) == 0) {
      /* Pas $i en rhs */
      if (*left_val_ptr >> FS_ID_SHIFT != left_wfs_id)
	sxtrap (ME, "unify_in");
    }
#endif /* EBUG */
  }
  else {
    /* $$ X* < $[$|i] Y* adjunct */
    left_wfs_id = (*left_val_ptr >> FS_ID_SHIFT); /* wfs_id */
    /* right_id ne pourra s'evaluer que si la lhs l'est */
  }
    
#if EBUG
  if (left_wfs_id < MAIN_WFS_ID || left_wfs_id > wfs_id)
    sxtrap (ME, "unify_in");
#endif /* EBUG */
  
  left_wfs_id = get_equiv (left_wfs_id); /* wfs_id de la lhs ds tous les cas */

  if (right_id == 0) {
    /* Nouveau */
#if EBUG
    if ((right_kind & NEW_FIELD) == 0)
     sxtrap (ME, "unify_in");
#endif /* EBUG */

    right_id = get_new_top_field_val2adjunct_id ();
    field_val2adjunct_id [right_id].adjunct_set_id = new_top = get_new_top_adjunct_sets ();
  }
  else {
    right_id = get_adjunct_equiv (right_id);
    prev_top = field_val2adjunct_id [right_id].adjunct_set_id;

    while ((prev_top = adjunct_sets [top = prev_top].next) != 0);

    if (adjunct_sets [top].kind == ADJUNCT_KIND_EMPTY)
      /* On "remplace le EMPTY" */
      new_top = top;
    else
      /* On "ajoute" */
      new_top = get_new_top_adjunct_sets ();

    adjunct_sets [top].next = new_top;
  }

  adjunct_ptr = adjunct_sets + new_top;
  adjunct_ptr->next = 0;
  adjunct_ptr->kind = ADJUNCT_KIND_WFS;
  adjunct_ptr->id = left_wfs_id;

  *right_val_ptr = (right_id << FS_ID_SHIFT) + field_id;

  return SXTRUE;
}


/* On a une valeur atomique en partie droite */
/* Attention On permet du "$i ... v_form =c infinitive" */
/* $$ ... X = "-" */
static SXBOOLEAN
atomic_value_unification (left_result, right_result)
     int     left_result, right_result;
{ 
  int                     bot, top, path_id; 
  int                     left_static_field_kind;
  int                     left_kind, left_father_wfs_id, left_wfs_id, left_field, left_ufs_id, left_val;
  int                     right_kind, right_father_wfs_id, right_wfs_id, right_field, right_val;
  int                     *left_val_ptr, *right_val_ptr;
  struct adjunct_set_item *adjunct_ptr;
  struct re_result        *left_result_ptr;
#ifdef ESSAI
  int                     left_local_atom_id, right_local_atom_id, old_next, val, new_top;
  SXBOOLEAN                 is_shared;
  struct adjunct_set_item *left_adjunct_ptr;
#endif /* ESSAI */

  left_result_ptr = re_results+left_result;
  left_val_ptr = left_result_ptr->ptr;
  left_kind = left_result_ptr->kind;

  right_val_ptr = re_results [right_result].ptr;
  right_kind = re_results [right_result].kind;

  if (right_kind & EMPTY_STRUCT)
    right_val = 0;
  else
    right_val = *right_val_ptr; /* La valeur de l'atome */
  
#if EBUG
  if ((left_kind & (ATOM_FIELD)) == 0 || (right_kind & ATOM_VAL) == 0)
    sxtrap (ME, "atomic_value_unification");
#endif /* EBUG */

  if (left_kind >> REF_SHIFT) {
    /* cas $i ... v_form (ATOM_KIND) */
    /* left_result_ptr->father_wfs_id est le wfs_id qui contient le champ "left_field_id" */
    left_wfs_id = left_result_ptr->father_wfs_id;

#if EBUG
    if (left_wfs_id < MAIN_WFS_ID || left_wfs_id > wfs_id)
      sxtrap (ME, "atomic_value_unification");
#endif /* EBUG */

    left_wfs_id = get_equiv (left_wfs_id);
    path_id = wfs2attr [AREA_PAGE (left_wfs_id)] [AREA_DELTA (left_wfs_id)] [0];

    if (path_id) {
      /* ... non instancie', on le fait */
      instantiate_wfs_id (left_wfs_id);
    }
  }

  /* left_wfs_id est un indice ds field_val2adjunct_id */
  left_wfs_id = (*left_val_ptr) >> FS_ID_SHIFT;

#if EBUG
  if (left_wfs_id == 0)
    sxtrap (ME, "atomic_value_unification");
#endif /* EBUG */

  if (right_kind & EMPTY_STRUCT) {
    /* L'unif marche toujours */
    /* Apres reflexion, on ne tient pas compte du =c (= <=> =c) */
#if 0
    if (!is_constrained_unification)
      /* RAZ du "champ" CONSTRAINT_VAL */
      *left_val_ptr &= ~CONSTRAINT_VAL;
#endif /* 0 */

    return SXTRUE;
  }

  left_wfs_id = get_adjunct_equiv (left_wfs_id);
  adjunct_ptr = adjunct_sets + field_val2adjunct_id [left_wfs_id].adjunct_set_id;
  left_val = adjunct_ptr->id;

#if ATOM_Aij || ATOM_Pij
  if (
#ifndef ESSAI
      right_val == ATOM_Aij || right_val == ATOM_Pij
#else /* ESSAI */
      right_val == UFS_ID_Aij || right_val == UFS_ID_Pij
#endif /* ESSAI */
      ) {
    /* $$ ... X = Aij | Pij et X est bien un champ atomique unbounded */
    if (adjunct_ptr->kind != ADJUNCT_KIND_EMPTY) {
      /* On insere derriere le 1er !! */
      top = get_new_top_adjunct_sets ();
      adjunct_sets [top].next = adjunct_ptr->next;
      adjunct_ptr->next = top;
      adjunct_ptr = adjunct_sets+top;
    }

    adjunct_ptr->id = (right_val == 
#ifndef ESSAI
		       ATOM_Aij
#else /* ESSAI */
		       UFS_ID_Aij
#endif /* ESSAI */
		       ) ? AIJ : (spf.outputG.maxxnt+PIJ);
    adjunct_ptr->kind = ADJUNCT_KIND_Aij;
    /* *left_val_ptr est bon ... */
  }
  else 
#endif /* ATOM_Aij || ATOM_Pij */
    {
      /* $$ ... X = "-" et X est bien un champ atomique */
      /* adjunct_ptr est correct */
#ifndef ESSAI
      if (adjunct_ptr->kind == ADJUNCT_KIND_EMPTY) {
	adjunct_ptr->kind = ADJUNCT_KIND_ATOM;
	adjunct_ptr->id = right_val; /* La valeur */

	if (is_constrained_unification)
	  /* Positionne le "champ" CONSTRAINT_VAL */
	  *left_val_ptr |= CONSTRAINT_VAL;
	else
	  /* RAZ du "champ" CONSTRAINT_VAL */
	  *left_val_ptr &= ~CONSTRAINT_VAL;

	if (is_optional_unification)
	  /* Positionne le "champ" OPTIONAL_VAL */
	  *left_val_ptr |= OPTIONAL_VAL;
	else
	  /* RAZ du "champ" OPTIONAL_VAL */
	  *left_val_ptr &= ~OPTIONAL_VAL;

	return SXTRUE;
      }
	
      if (adjunct_ptr->kind == ADJUNCT_KIND_UFS) {
	/* On instancie ... */
	top = XH_X (XH_ufs_hd, left_val->id+1);
	bot = XH_X (XH_ufs_hd, left_val->id);

#if EBUG
	if (bot == top)
	  /* Seule une structure partagee peut etre vide */
	  sxtrap (ME, "atomic_value_unification");
#endif /* EBUG */

	if (XH_list_elem (XH_ufs_hd, top-1) < 0)
	  top--;

	if (bot == top) {
	  /* En fait, c'est vide */
	  adjunct_ptr->id = right_val; /* La valeur */
	  adjunct_ptr->kind = ADJUNCT_KIND_ATOM;

	  if (is_constrained_unification)
	    /* Positionne le "champ" CONSTRAINT_VAL */
	    *left_val_ptr |= CONSTRAINT_VAL;
	  else
	    /* RAZ du "champ" CONSTRAINT_VAL */
	    *left_val_ptr &= ~CONSTRAINT_VAL;

	  if (is_optional_unification)
	    /* Positionne le "champ" OPTIONAL_VAL */
	    *left_val_ptr |= OPTIONAL_VAL;
	  else
	    /* RAZ du "champ" OPTIONAL_VAL */
	    *left_val_ptr &= ~OPTIONAL_VAL;

	  return SXTRUE;
	}
	    
	adjunct_ptr->id = XH_list_elem (XH_ufs_hd, bot);
	adjunct_ptr->kind = ADJUNCT_KIND_ATOM;
      }

#if EBUG
      if (adjunct_ptr->kind != ADJUNCT_KIND_ATOM)
	sxtrap (ME, "atomic_value_unification");
#endif /* EBUG */ 

      left_static_field_kind = field_id2kind [(*left_val_ptr) & FIELD_AND];

      if (left_static_field_kind & UNBOUNDED_KIND) {
	/* L'unification est l'egalite */
	if (left_val != right_val)
	  right_val = 0;
      }
      else {
	/* L'unification est l'intersection des valeurs atomiques */
	right_val = left_val & right_val;
      }
	
      adjunct_ptr->id = right_val;
#else /* ESSAI */
      if (adjunct_ptr->kind == ADJUNCT_KIND_EMPTY ||
	  (adjunct_ptr->kind == ADJUNCT_KIND_UFS && (left_local_atom_id = get_ufs_atom_val (left_val, &is_shared, left_local_atom_id_set)) == 0)) {
	adjunct_ptr->kind = ADJUNCT_KIND_UFS;
	adjunct_ptr->id = right_val; /* l'ufs_id */

	if (is_constrained_unification)
	  /* Positionne le "champ" CONSTRAINT_VAL */
	  *left_val_ptr |= CONSTRAINT_VAL;
	else
	  /* RAZ du "champ" CONSTRAINT_VAL */
	  *left_val_ptr &= ~CONSTRAINT_VAL;

	if (is_optional_unification)
	  /* Positionne le "champ" OPTIONAL_VAL */
	  *left_val_ptr |= OPTIONAL_VAL;
	else
	  /* RAZ du "champ" OPTIONAL_VAL */
	  *left_val_ptr &= ~OPTIONAL_VAL;

	return SXTRUE;
      }

      if ((right_local_atom_id = get_ufs_atom_val (right_val, &is_shared, right_local_atom_id_set)) == 0) {
	/* rhs vide, on ne change pas la LHS */
	sxba_empty (left_local_atom_id_set);

	if (is_constrained_unification)
	  /* Positionne le "champ" CONSTRAINT_VAL */
	  *left_val_ptr |= CONSTRAINT_VAL;
	else
	  /* RAZ du "champ" CONSTRAINT_VAL */
	  *left_val_ptr &= ~CONSTRAINT_VAL;

	if (is_optional_unification)
	  /* Positionne le "champ" OPTIONAL_VAL */
	  *left_val_ptr |= OPTIONAL_VAL;
	else
	  /* RAZ du "champ" OPTIONAL_VAL */
	  *left_val_ptr &= ~OPTIONAL_VAL;

	return SXTRUE;
      }
	
      if (adjunct_ptr->kind == ADJUNCT_KIND_ATOM) {
	/* On met la disjonction ds left_local_atom_id_set */
	left_local_atom_id = left_val;
	SXBA_1_bit (left_local_atom_id_set, left_local_atom_id);

	left_adjunct_ptr = adjunct_ptr;

	while (left_adjunct_ptr->next) {
	  left_adjunct_ptr = adjunct_sets + left_adjunct_ptr->next;
	  left_local_atom_id = left_adjunct_ptr->id;
	  SXBA_1_bit (left_local_atom_id_set, left_local_atom_id);
	}
      }

      /* ici left et right_local_atom_id_set sont non vides et l'unif est l'intersection */
      if (sxba_2op (left_local_atom_id_set, SXBA_OP_COPY, left_local_atom_id_set, SXBA_OP_AND, right_local_atom_id_set)) {
	val = sxba_scan_reset (left_local_atom_id_set, 0);
	/* On reutilise left_top et ses "next" => gestion + aisee des equiv et economies */
	left_adjunct_ptr = adjunct_ptr;
	adjunct_ptr->id = val;
	adjunct_ptr->kind = ADJUNCT_KIND_ATOM;
	old_next = adjunct_ptr->next;
	adjunct_ptr->next = 0;

	while ((val = sxba_scan_reset (left_local_atom_id_set, val)) > 0) {
	  if (old_next) {
	    new_top = old_next;
	    old_next = adjunct_sets [new_top].next;
	  }
	  else
	    new_top = get_new_top_adjunct_sets ();

	  adjunct_ptr->next = new_top;
	  adjunct_ptr = adjunct_sets+new_top;
	  adjunct_ptr->id = val;
	  adjunct_ptr->kind = ADJUNCT_KIND_ATOM;
	  adjunct_ptr->next = 0;
	}

	right_val = 1;
      }
      else
	/* Echec de l'unif */
	right_val = 0;

      sxba_empty (right_local_atom_id_set);
#endif /* ESSAI */
    }

  if (right_val) {
    if (!is_constrained_unification)
      /* RAZ du "champ" CONSTRAINT_VAL */
      *left_val_ptr &= ~CONSTRAINT_VAL;

    if (!is_optional_unification)
      /* RAZ du "champ" OPTIONAL_VAL */
      *left_val_ptr &= ~OPTIONAL_VAL;
  }

  return right_val != 0;
}

/* Le 26/05/05 on peut avoir du $i en LHS ou en RHS */
static SXBOOLEAN
unify (left_result, right_result)
     int     left_result, right_result;
{
  int                     right_dol, left_dol, main_head, val, left_id, right_id, new_wfs_id, top, left_top;
  int                     left_wfs_id, bot;
  int                     *dpv_lptr, *dpv_rptr;
  struct adjunct_set_item *adjunct_ptr;
     
  int                     field_id_path, left_kind, right_kind, single_wfs_id;
  int                     *left_val_ptr, *right_val_ptr;
  struct re_result        *re_result_left_ptr, *re_result_right_ptr;

  re_result_left_ptr = re_results+left_result;
  re_result_right_ptr = re_results+right_result;

  left_val_ptr = re_result_left_ptr->ptr;
  left_kind = re_result_left_ptr->kind;

  right_val_ptr = re_result_right_ptr->ptr;
  right_kind = re_result_right_ptr->kind;

#if EBUG
  if ((right_kind & ATOM_VAL))
    /* valeur atomique en rhs */
    sxtrap (ME, "unify");
#endif /* EBUG */

  if (right_kind & ATOM_FIELD) {
#if EBUG
    if ((left_kind & ATOM_FIELD) == 0)
      sxtrap (ME, "unify");
#endif /* EBUG */ 

    /* unbounded_structure_unification marche ds le cas des atomes !! */
    if (!unbounded_structure_unification (left_result, right_result))
      return SXFALSE;
  }
  else {
    if (left_kind & UNBOUNDED_STRUCT_FIELD) {
      /* Unification sur des ensembles de f_structures (adjuncts) : le resultat est l'union */
#if EBUG
      if ((right_kind & UNBOUNDED_STRUCT_FIELD) == 0)
	/* Pas d'adjunct en rhs !! */
	sxtrap (ME, "unify");
#endif /* EBUG */
    
      if (right_kind & EMPTY_STRUCT) {
      /* X* = {} */
      /* C,a marche toujours :
	 Soit X* a ete cree vide
	 Soit X* est non vide (et l'unif avec le vide marche) */
      }
      else {
	if (!unbounded_structure_unification (left_result, right_result))
	  return SXFALSE;
      }
    }
    else {
      if (right_kind & EMPTY_STRUCT) {
	/* $$ X* = [] */
	/* C,a marche toujours :
	   Soit X* a ete cree vide
	   Soit X* est non vide (et l'unif avec le vide marche) */
      }
      else {
	/* Ici on a des f_structures a gauche et a droite */
	if ((left_kind & DOL_REF) && (right_kind & DOL_REF) && (right_dol = right_kind>>REF_SHIFT) == (left_dol = left_kind>>REF_SHIFT)) {
	  /* $[$|i] = $[$|i]  !! */
	  /* => SXTRUE */
	}
	else {
	  /* $[$|i] X* = $[$|i] Y* */
	  /* Le 04/08/05 On repere les equations de la forme
	     $$ = $i
	     Si c'est la seule valide du groupe => le resultat est la fs_id de $i
	     Attention, ca ne marche pas pour $$ = $i X+ car $i X+ peut etre partage'e ou non close ds fs_hd
	  */
	  if ((left_kind & DOL_REF) && (right_kind & DOL_REF)) {
	    single_wfs_id = (*(re_results [(left_dol == 0) ? right_result : left_result].ptr)) >> FS_ID_SHIFT;
	    field_id_path = wfs2attr [AREA_PAGE (single_wfs_id)] [AREA_DELTA (single_wfs_id)] [0];
	    single_fs_id = path2wfs_id [field_id_path].fs_id_dol >> MAXRHS_SHIFT;
	  }

	  if (!structure_unification (left_result, right_result))
	    return SXFALSE;
	}
      }
    }
  }

  /* Traitement des contraintes d'unification */
  /* Gestion du champ "CONSTRAINT_VAL" */
  /* PB : on ne sait pas faire $i =c $j !! */
  if (is_constrained_unification) {
    /* $i X* =c $j Y* */
    /* Si left et right sont contraints => rien */
    /* Si left et right sont non contraints => rien */
    /* Si l'un est contraint et l'autre non => le contraint devient non contraint */
    SXBOOLEAN is_left_c, is_right_c;

    is_left_c = (left_kind & DOL_REF) == 0 && (*left_val_ptr & CONSTRAINT_VAL); /* left est contraint */
    is_right_c = (right_kind & EMPTY_STRUCT) == 0 && (right_kind & DOL_REF) == 0 && (*right_val_ptr & CONSTRAINT_VAL); /* right est contraint */
  
    if (is_left_c + is_right_c == 1 /* is_left_c && !is_right_c || !is_left_c && is_right_c */) {
      if (is_left_c)
	*left_val_ptr &= ~CONSTRAINT_VAL;
      else
	*right_val_ptr &= ~CONSTRAINT_VAL;
    }
  }
  else {
    /* $i X* = $j Y* */
    /* left et right (si approprie') deviennent des equal unification */
    if ((left_kind & DOL_REF) == 0)
      /* X* non vide => resultat non contraint */
      *left_val_ptr &= ~CONSTRAINT_VAL;

    if ((right_kind & EMPTY_STRUCT) == 0 && (right_kind & DOL_REF) == 0)
      /* $j Y* non de la forme [] ou {} et Y* non vide => resultat non contraint */
      *right_val_ptr &= ~CONSTRAINT_VAL;
  }

  /* Gestion du champ "OPTIONAL_VAL" */
  if (is_optional_unification) {
    /* $i X* =? $j Y* */
    /* Si left et right sont optionnels => rien */
    /* Si left et right sont non optionnels => rien */
    /* Si l'un est optionnel et l'autre non => le optionnel devient non optionnel  BS 30/05/07 NON NON NON */
    SXBOOLEAN is_left_c, is_right_c;

    is_left_c = (left_kind & DOL_REF) == 0 && (*left_val_ptr & OPTIONAL_VAL); /* left est contraint */
    is_right_c = (right_kind & EMPTY_STRUCT) == 0 && (right_kind & DOL_REF) == 0 && (*right_val_ptr & OPTIONAL_VAL); /* right est contraint */
  
#if 0
    if (is_left_c + is_right_c == 1 /* is_left_c && !is_right_c || !is_left_c && is_right_c */) {
      if (is_left_c)
	*left_val_ptr &= ~OPTIONAL_VAL;
      else
	*right_val_ptr &= ~OPTIONAL_VAL;
    }
#endif /* 0 */
  }
  else {
    /* $i X* = $j Y* */
    /* left et right (si approprie') deviennent des equal unification */
    if ((left_kind & DOL_REF) == 0)
      /* X* non vide => resultat non contraint */
      *left_val_ptr &= ~OPTIONAL_VAL;

    if ((right_kind & EMPTY_STRUCT) == 0 && (right_kind & DOL_REF) == 0)
      /* $j Y* non de la forme [] ou {} et Y* non vide => resultat non contraint */
      *right_val_ptr &= ~OPTIONAL_VAL;
  }

  return SXTRUE;
}


/* On unifie les ensembles de f_structures (adjuncts) reperes par left_val_ptr et right_val_ptr */
/* ATTENTION, ca doit aussi marcher pour les aij !! ... */
/* ... et pour les atomes */
/* 2 ufs peuvent etre partages */
/* PB : si l'un est modifie', l'autre doit l'etre egalement or il a pu ne pas etre instancie' !! */
/* Si 2 ufs sont egales et non partagees, elles doivent s'instancier a des endroits differents
   sauf si elles ont des chemins de champs d'acces identiques */
/* Version du 30/05/05 modifiee pour "add" le 22/09/05 */
static SXBOOLEAN
unbounded_structure_unification_add (left_result, right_result)
     int     left_result, right_result;
{
  int                     left_id, right_id, right_top, left_top, right_field;
  struct adjunct_set_item *left_adjunct_ptr, *right_adjunct_ptr;
  int                     *left_val_ptr, *right_val_ptr;

  left_val_ptr = re_results [left_result].ptr;
  left_top = get_adjunct_equiv (*left_val_ptr >> FS_ID_SHIFT);
  /* left_top est un indice ds field_val2adjunct_id ... */

  right_val_ptr = re_results [right_result].ptr;
  right_top = get_adjunct_equiv (*right_val_ptr >> FS_ID_SHIFT);
  /* right_top est un indice ds field_val2adjunct_id ... */

#if EBUG
  /* ... non nuls */
  if (left_top == 0 || right_top == 0)
    sxtrap (ME, "unbounded_structure_unification_add");
#endif /* EBUG */

  if (left_top == right_top)
    /* !! */
    return SXTRUE;
  
  left_adjunct_ptr = adjunct_sets + field_val2adjunct_id [left_top].adjunct_set_id;
  right_adjunct_ptr = adjunct_sets + field_val2adjunct_id [right_top].adjunct_set_id;

  right_field = *right_val_ptr & FS_ID_AND;

  if (left_adjunct_ptr == right_adjunct_ptr) {
    /* !! */
    *right_val_ptr = (left_top << FS_ID_SHIFT) + (right_field);
    field_val2adjunct_id [right_top].equiv = left_top; /* droit devient equivalents a gauche ?? */
    
    return SXTRUE;
  }
    
  if (right_adjunct_ptr->kind == ADJUNCT_KIND_EMPTY) {
#if 0
    /* Pour l'instant ??, rien */
    *right_val_ptr = (left_top << FS_ID_SHIFT) + (right_field);
    field_val2adjunct_id [right_top].equiv = left_top; /* droit devient equivalents a gauche */
#endif /* 0 */
    
    return SXTRUE;
  }

  if (left_adjunct_ptr->kind == ADJUNCT_KIND_EMPTY) {
    *left_val_ptr = (right_top << FS_ID_SHIFT) + (*left_val_ptr & FS_ID_AND);
    field_val2adjunct_id [left_top].equiv = right_top; /* gauche devient equivalents a droit */

    return SXTRUE;
  }

  /* Ici l'operande gauche et l'operande droit sont non vides */
  /* on ne fait rien */
  return SXTRUE;
}




/* On unifie les structures left_wfs_id et right_wfs_id dans left_wfs_id */
/* left_wfs_id est le representant de la classe d'equivalence.
   Il en est de meme de right_wfs_id si right_dol==0 */
/* Le 15/02/05 le 1er arg left_wfs_id devient left_wfs_id_ptr car il est possible que structure_unification ()
   change le wfs_id ds lequel se fait l'unification ds le cas de structures cycliques */
/* Le 17/02/05 ajout de right_field_id_path qui repere, si right_dol > 0 le chemin X* de $i X* */
/* Le 27/05/05 On peut modifier les fils et donc avoir du $i en lhs */
/* modifiee pour "add" le 22/09/05  */
static SXBOOLEAN
structure_unification_add (left_result, right_result)
     int     left_result, right_result;
{
  int                   left_wfs_id, right_wfs_id, field_id, field_kind, static_field_kind;
  int                   left_page_nb, left_delta_nb, right_page_nb, right_delta_nb, left_result_id, right_result_id;
  int                   new_son_nb, adjunct_set_id;
  int                   *left_attr_ptr0, *left_attr_ptr, *right_attr_ptr0, *right_attr_ptr;
  SXBA                  left_field_set, right_field_set;
  struct re_result      *re_result_left_ptr, *re_result_right_ptr, *left_result_ptr, *right_result_ptr;

  /* Attention, ds le cas "add", left et right ne deviennent pas equivalents */

  /* Ds tous les cas re_results [(left_ | right_)result].ptr >> FS_ID_SHIFT designe un wfs_id */
  /* Si le chemin est de la forme $$ X*, son wfs_id s'obtient par *(re_result_left_ptr->ptr) >> FS_ID_SHIFT */

  re_result_left_ptr = re_results+left_result;
  re_result_right_ptr = re_results+right_result;

  left_wfs_id = *(re_result_left_ptr->ptr) >> FS_ID_SHIFT;
  left_wfs_id = get_equiv (left_wfs_id); /* En fait on est la` !! */

  right_wfs_id = *(re_result_right_ptr->ptr) >> FS_ID_SHIFT;
  right_wfs_id = get_equiv (right_wfs_id); /* En fait on est la` !! */

#if EBUG
  if (left_wfs_id < MAIN_WFS_ID || left_wfs_id > wfs_id || right_wfs_id < MAIN_WFS_ID || right_wfs_id > wfs_id )
    sxtrap (ME, "structure_unification_add");
#endif /* EBUG */

  if (left_wfs_id == right_wfs_id)
    return SXTRUE;

#if 0
  /* ... pas ds le cas "add" */
  /* Le 09/08/05 */
  if (right_wfs_id == MAIN_WFS_ID) {
    /* MAIN_WFS_ID doit rester le principal => on echange gauche-droit */
    struct re_result      *re_result_ptr;

    right_wfs_id = left_wfs_id;
    left_wfs_id = MAIN_WFS_ID;
    re_result_ptr = re_result_left_ptr;
    re_result_left_ptr = re_result_right_ptr;
    re_result_right_ptr = re_result_ptr;
  }
#endif /* 0 */

  right_page_nb = AREA_PAGE (right_wfs_id);
  right_delta_nb = AREA_DELTA (right_wfs_id);
  right_attr_ptr0 = wfs2attr [right_page_nb] [right_delta_nb];  

  if (*right_attr_ptr0 == -1) {
    /* rhs vide => on ne fait rien */
    return SXTRUE;
  }

  left_page_nb = AREA_PAGE (left_wfs_id);
  left_delta_nb = AREA_DELTA (left_wfs_id);
  left_attr_ptr0 = wfs2attr [left_page_nb] [left_delta_nb];
      
  /* On pourrait regarder si par hasard left et right ne designent pas le meme fs_id ?? A priori peu frequent
     pour l'instant on les unifie */

  if (*left_attr_ptr0 > 0) {
    /* non instancie', on le fait ... */
    instantiate_wfs_id (left_wfs_id);
  }
  else
    /* Ca pouvait etre -1 */
    *left_attr_ptr0 = 0;
      
  if (*right_attr_ptr0 > 0) {
    /* non instancie', on le fait ... */
    instantiate_wfs_id (right_wfs_id);
  }
  
  /* On unifie donc par addition right_wfs_id ds left_wfs_id */
  left_field_set = wfs_field_set [left_page_nb] [left_delta_nb];
  right_field_set = wfs_field_set [right_page_nb] [right_delta_nb];
#if 0
  /* et droite devient equivalente a gauche */
  /* Non, pas ds le cas "add" */
  wfs_id2equiv [right_wfs_id] = left_wfs_id;
#endif /* 0 */

  left_result_ptr = NULL; /* non initialise' */
  field_id = 0;

  while ((field_id = sxba_scan (right_field_set, field_id)) > 0) {
#if EBUG
    /* Les instanciations ont deja fait cette verif */
    if (!is_executable (field_id))
      /* Ce champ n'est pas calcule' ds cette passe */
      sxtrap (ME, "structure_unification_add");
#endif /* EBUG */

    static_field_kind = field_id2kind [field_id];
    left_attr_ptr = left_attr_ptr0+field_id;
    right_attr_ptr = right_attr_ptr0+field_id;

    if (SXBA_bit_is_reset_set (left_field_set, field_id)) {
      /* field_id est a droite et pas a gauche */
      /* La pred des [unbounded_]f_structures reperees par field_id qui pointe vers right_wfs_id
	 peut continuer a le faire (on accedera a left_wfs_id par les equiv) */
      *left_attr_ptr = *right_attr_ptr;
    }
    else {
      /* field_id est a gauche et a droite */
#if EBUG
      if ((*left_attr_ptr >> FS_ID_SHIFT) == 0 || (*right_attr_ptr >> FS_ID_SHIFT) == 0) {
	/* On suppose que ca veut dire que cette structure est vide */
	printf ("System error : $$ ... %s is null!!\n", field_id2string [field_id]);
	sxtrap (ME, "structure_unification_add");
      }
#endif /* EBUG */

      field_kind = ((static_field_kind & STRUCT_KIND)
		    ? (static_field_kind & UNBOUNDED_KIND
		       ? UNBOUNDED_STRUCT_FIELD
		       : STRUCT_FIELD)
		    : ATOM_FIELD) + /* (right_dol << REF_SHIFT) + car right_dol==0 */ OLD_FIELD;

      if (left_result_ptr == NULL) {
	/* initialisation independante du field_id */
	left_result_id = re_results_get_new_top ();
	right_result_id = re_results_get_new_top ();

	left_result_ptr = re_results + left_result_id;
	left_result_ptr->father_wfs_id = left_wfs_id;
	right_result_ptr = re_results + right_result_id;
	right_result_ptr->father_wfs_id = right_wfs_id;
      }

      left_result_ptr->ptr = left_attr_ptr;
      left_result_ptr->kind = field_kind;
	  
      right_result_ptr->ptr = right_attr_ptr;
      right_result_ptr->kind = field_kind;
	  
      if (static_field_kind == STRUCT_KIND)
	structure_unification_add (left_result_id, right_result_id);
      else
	unbounded_structure_unification_add (left_result_id, right_result_id);
    }
  }

  return SXTRUE;
}



static SXBOOLEAN
unify_add (left_result, right_result)
     int left_result, right_result;
{
  int                     right_dol, left_dol, main_head, val, left_id, right_id, new_wfs_id, top, left_top;
  int                     left_wfs_id, bot;
  int                     *dpv_lptr, *dpv_rptr;
  struct adjunct_set_item *adjunct_ptr;
  int                     path_id; 
  int                     left_static_field_kind;
  int                     left_father_wfs_id, left_field, left_ufs_id, left_val;
  int                     right_father_wfs_id, right_wfs_id, right_field, right_val;
  int                     field_id_path, left_kind, right_kind, single_wfs_id;
  int                     *left_val_ptr, *right_val_ptr;
  struct re_result        *re_result_left_ptr, *re_result_right_ptr;

  /* Pas de contraintes d'unification */

  re_result_right_ptr = re_results+right_result;
  right_kind = re_result_right_ptr->kind;
    
  if (right_kind & EMPTY_STRUCT)
    /* C,a marche toujours */
    return SXTRUE;

  re_result_left_ptr = re_results+left_result;
  left_kind = re_result_left_ptr->kind;

  left_val_ptr = re_result_left_ptr->ptr;
  right_val_ptr = re_result_right_ptr->ptr;

  if (right_kind & ATOM_VAL) {
    /* cas $[$|i] X* += atom */
#if EBUG
    /* A FAIRE statiquement */
    if (left_kind & DOL_REF)
      sxtrap (ME, "unify_add");
#endif /* EBUG */

    right_val = *right_val_ptr; /* La valeur de l'atome */
  
#if EBUG
    if ((left_kind & (ATOM_FIELD)) == 0 || (right_kind & ATOM_VAL) == 0)
      sxtrap (ME, "unify_add");
#endif /* EBUG */

    if (left_kind >> REF_SHIFT) {
      /* cas $i ... v_form (ATOM_KIND) */
      /* re_result_left_ptr->father_wfs_id est le wfs_id qui contient le champ "left_field_id" */
      left_wfs_id = re_result_left_ptr->father_wfs_id;

#if EBUG
      if (left_wfs_id < MAIN_WFS_ID || left_wfs_id > wfs_id)
	sxtrap (ME, "unify_add");
#endif /* EBUG */

      left_wfs_id = get_equiv (left_wfs_id);
      path_id = wfs2attr [AREA_PAGE (left_wfs_id)] [AREA_DELTA (left_wfs_id)] [0];

      if (path_id) {
	/* ... non instancie', on le fait */
	instantiate_wfs_id (left_wfs_id);
      }
    }

    /* cas $$ ... v_form (ATOM_KIND) */
    /* left_wfs_id est un indice ds field_val2adjunct_id */
    left_wfs_id = (*left_val_ptr) >> FS_ID_SHIFT;

#if EBUG
    if (left_wfs_id == 0)
      sxtrap (ME, "unify_add");
#endif /* EBUG */

    left_wfs_id = get_adjunct_equiv (left_wfs_id);
    adjunct_ptr = adjunct_sets + field_val2adjunct_id [left_wfs_id].adjunct_set_id;

#if ATOM_Aij || ATOM_Pij
    if (
#ifndef ESSAI
      right_val == ATOM_Aij || right_val == ATOM_Pij
#else /* ESSAI */
      right_val == UFS_ID_Aij || right_val == UFS_ID_Pij
#endif /* ESSAI */
	) {
      /* Peut-on faire du "add" sur Aij ou Pij ?? */
      /* $$ ... X = Aij | Pij et X est bien un champ atomique unbounded */
      if (adjunct_ptr->kind == ADJUNCT_KIND_EMPTY) {
	/* lhs vide */
	/* on remplace */
	adjunct_ptr->id = (right_val == 
#ifndef ESSAI
			   ATOM_Aij
#else /* ESSAI */
			   UFS_ID_Aij
#endif /* ESSAI */
			   ) ? AIJ : (spf.outputG.maxxnt+PIJ);
	adjunct_ptr->kind = ADJUNCT_KIND_Aij;
	/* *left_val_ptr est bon ... */
      }
      /* else rien */
    }
    else 
#endif /* ATOM_Aij || ATOM_Pij */
      {
	/* $$ ... X = "-" et X est bien un champ atomique */
	/* adjunct_ptr est correct */
	if (adjunct_ptr->kind == ADJUNCT_KIND_EMPTY) {
	  adjunct_ptr->kind = ADJUNCT_KIND_ATOM;
	  adjunct_ptr->id = right_val;
	}
	/* else rien */
      }

    return SXTRUE;
  }

  /* cas $[$|i] X* += $[$|i] Y* */

  if (right_kind & ATOM_FIELD) {
#if EBUG
    if ((left_kind & ATOM_FIELD) == 0)
      sxtrap (ME, "unify_add");
#endif /* EBUG */ 

    /* unbounded_structure_unification marche ds le cas des atomes !! */
    return unbounded_structure_unification_add (left_result, right_result);
  }
    
  if (left_kind & UNBOUNDED_STRUCT_FIELD) {
    /* Unification sur des ensembles de f_structures (adjuncts) : le resultat est l'union */
#if EBUG
    if ((right_kind & UNBOUNDED_STRUCT_FIELD) == 0)
      /* Pas d'adjunct en rhs !! */
      sxtrap (ME, "unify_add");
#endif /* EBUG */

    return unbounded_structure_unification_add (left_result, right_result);
  }

  /* Ici on a des f_structures a gauche et a droite */
  if ((left_kind & DOL_REF) && (right_kind & DOL_REF) && (right_dol = right_kind>>REF_SHIFT) == (left_dol = left_kind>>REF_SHIFT)) {
    /* $[$|i] = $[$|i]  !! */
    return SXTRUE;
  }

  /* $[$|i] X* = $[$|i] Y* */
  /* Le 04/08/05 On repere les equations de la forme
     $$ = $i
     Si c'est la seule valide du groupe => le resultat est la fs_id de $i
     Attention, ca ne marche pas pour $$ = $i X+ car $i X+ peut etre partage'e ou non close ds fs_hd
  */
  if ((left_kind & DOL_REF) && (right_kind & DOL_REF)) {
    single_wfs_id = (*(re_results [(left_dol == 0) ? right_result : left_result].ptr)) >> FS_ID_SHIFT;
    field_id_path = wfs2attr [AREA_PAGE (single_wfs_id)] [AREA_DELTA (single_wfs_id)] [0];
    single_fs_id = path2wfs_id [field_id_path].fs_id_dol >> MAXRHS_SHIFT;
  }

  return structure_unification_add (left_result, right_result);
}


static SXBOOLEAN
call_unify (operator, left_result, right_result)
     int operator, left_result, right_result;
{
  int              dum;
  SXBOOLEAN          ret_val;
  
  switch (operator) {
  case OPERATOR_CONSTRAINT:
  case OPERATOR_OPTIONAL:
  case OPERATOR_UNIFY:
    is_constrained_unification = (operator == OPERATOR_CONSTRAINT);
    is_optional_unification = (operator == OPERATOR_OPTIONAL);

    if (re_results [right_result].kind & ATOM_VAL) {
      /* cas $[$|i] X* =[c|?] atom */
#if EBUG
      /* A FAIRE statiquement */
      if (re_results [left_result].kind & DOL_REF)
	sxtrap (ME, "call_unify");
#endif /* EBUG */

      ret_val = atomic_value_unification (left_result, right_result);
      break;
    }
      
    if ((re_results [left_result].kind >> REF_SHIFT) && (re_results [right_result].kind >> REF_SHIFT) == 0) {
      /* On met un $$ en LHS quand c'est possible */
      dum = left_result;
      left_result = right_result;
      right_result = dum;
    }
      
    ret_val = unify (left_result, right_result);

    break;

  case OPERATOR_IN:
    ret_val = unify_in (left_result, right_result);

    break;

  case OPERATOR_ADD:
    ret_val = unify_add (left_result, right_result);

    break;

  case OPERATOR_POSSIBLE:
  default:
    sxtrap (ME, "call_unify");
  }

  return ret_val;
}

static int call_tfs_instantiation ();

static SXBOOLEAN has_an_re_equation;

/* On essaie d'evaluer l'equation dont le code est repere par xcode sur "prod" */
static SXBOOLEAN
evaluate (xcode)
     int xcode;
{
  int              left_val, right_val, left_kind, right_kind, eval_result, operator, lhs_code_size, lfield_val, rfield_val;
  int              *right_val_ptr, *left_val_ptr; 
  int              *store_left_pcs, *store_right_pcs;
  int              left_eval_result, right_eval_result, x, y, l;
  int              prev_post_stack_top, nb, post_stack_kind;
  int              left_field_id_path, right_field_id_path, left_re_results_top, right_re_results_top, val;
  SXBOOLEAN          ret_val, is_left_re, is_right_re, is_operator_possible, is_skip, done;
  struct re_result *re_result_left_ptr, *re_result_right_ptr, *re_result_ptr, dum_result, lresult, rresult;

  single_fs_id = 0;
  is_skip = SXFALSE;
#if LOG
  print_equation_code (xcode);
#endif /* LOG */

  pcs = code_stack+xcode;
  operator = *pcs;

  while (operator == OPERATOR_ETIQUETTE) {
    dum_result.ptr = left_val_ptr = &left_val;

    if ((eval_result = eval (&dum_result)) <= 0) {
#if LOG
      printf (" ... $%i is empty : skip\n", dum_result.kind>>REF_SHIFT);
#endif /* LOG */
      return SXTRUE;
    }

    /* eval_result > 0 */
    /* Ici l'etiquette est de la forme $i (on sait que i est >0) et $i repe`re un range non vide ... */
    /* ... c'est OK, on continue */
    
    operator = *(--pcs);
  }
    
  if (operator == OPERATOR_POSSIBLE) {
    is_operator_possible = SXTRUE;
    operator = *(--pcs);
  }
  else
    is_operator_possible = SXFALSE;

  switch (operator) {
  case OPERATOR_UNIFY:
  case OPERATOR_CONSTRAINT:
  case OPERATOR_IN:
  case OPERATOR_ADD:
  case OPERATOR_OPTIONAL:
    /* Attention :
       Si un calcul de chemin echoue parcequ'un $i reference un range vide, l'equation ne doit
       produire aucun effet de bord et on doit executer l'equation suivante 
       Ex : $$ a b ($i pcas)-obj = ... Ne pas creer a b si $i est vide
       $$ obj = $i Ne pas creer obj si $i est vide */

    lhs_code_size = *(--pcs);

    if (is_left_re = (pcs [-1] == OPERATOR_RE)) {
#if EBUG
      /* Les equations qui contiennent des ER doivent s'executer en dernier */
      has_an_re_equation = SXTRUE;
#endif /* EBUG */
      pcs--;
      lhs_code_size--;
      store_left_pcs = pcs;

      left_eval_result = re_eval (0, SXFALSE, SXTRUE);

      if (left_eval_result <= 0) {
	is_skip = ret_val = (left_eval_result != 0);
	break;
      }

      /* La liste des resultats se trouve en re_results [left_eval_result] */
      re_result_left_ptr = re_results+left_eval_result;
      left_re_results_top = left_eval_result;

      if (re_result_left_ptr->next == 0) {
	/* Un seul resultat => cas standard ... */
	/* ... ou on prend le seul cree */
	/* Le resultat est ds re_results ... */
	/* ... et est disponible ds left_val_ptr et left_kind */
	left_eval_result = 0;
      }
      /* else left_eval_result > 0 <=> resultats multiples */
    }
    else {
      /* On commence par regarder le statut du chemin gauche de l'equation
	 pour ne pas creer de champs si jamais l'equation est sautee */
      store_left_pcs = pcs;

      lresult.ptr = &lfield_val;
      left_eval_result = eval_examine (&lresult);

      if (left_eval_result == -1) {
	is_skip = ret_val = SXTRUE;
	/* Si $i < $$ X+ et $i est vide on ne cree pas X+ ds $$ */
	break;
      }
    }
      
    is_right_re = (pcs [-1] == OPERATOR_RE);

    if (!is_right_re) {
      /* On commence par regarder le statut du chemin droit de l'equation
	 pour ne pas creer de champs si jamais l'equation est sautee */
      store_right_pcs = pcs;

      rresult.ptr = &rfield_val;
      right_eval_result = eval_examine (&rresult);

      if (right_eval_result == -1) {
	is_skip = ret_val = SXTRUE;
	/* Si $i < $$ X+ et $i est vide on ne cree pas X+ ds $$ */
	break;
      }
    }
#if EBUG
    else {
      /* Les equations qui contiennent des ER doivent s'executer en dernier */
      has_an_re_equation = SXTRUE;
    }
#endif /* EBUG */

#if EBUG
      /* Les equations qui contiennent des ER doivent s'executer en dernier */
      if (has_an_re_equation && !is_right_re && !is_left_re)
	sxtrap (ME, "evaluate");
#endif /* EBUG */


    if (!is_left_re) {
      /* On recommence la partie gauche pour de bon !! */
      /* qui peut creer des champs */
      pcs = store_left_pcs;

      left_re_results_top = re_results_get_new_top ();
      re_result_ptr = re_results+left_re_results_top;
      re_result_ptr->ptr = int_tank_get ();
	
      left_eval_result = eval (re_result_ptr);

      if (left_eval_result <= 0) {
	is_skip = ret_val = (left_eval_result != 0);
	/* Si $i < $$ X+ et $i est vide on ne cree pas X+ ds $$ */
	break;
      }

      left_eval_result = 0;
    }

    /* ... et la partie droite */
    if (is_right_re) {
      pcs--;
      store_right_pcs = pcs;

      right_eval_result = re_eval (0, SXFALSE, is_right_re);

      if (right_eval_result <= 0) {
	is_skip = ret_val = (right_eval_result != 0);
	break;
      }

      /* La liste des resultats se trouve en re_results [right_eval_result] */
      re_result_right_ptr = re_results+right_eval_result;
      right_re_results_top = right_eval_result;

      if (re_result_right_ptr->next == 0 && left_eval_result == 0) {
	/* Un seul resultat => cas standard */
	/* ... ou on prend le seul cree */
	/* Le resultat est ds re_results ... */
	/* ... et est disponible ds right_val_ptr et right_kind */
	right_eval_result = 0;
      }
      /* else right_eval_result > 0 <=> resultats multiples */
    }
    else {
      /* On met tout ds re_results */
      pcs = store_right_pcs;
      right_re_results_top = re_results_get_new_top ();
      re_result_ptr = re_results+right_re_results_top;
      re_result_ptr->ptr = int_tank_get ();

      right_eval_result = eval (re_result_ptr);

      if (right_eval_result <= 0) {
	is_skip = ret_val = (right_eval_result != 0);
	/* Si $i < $$ X+ et $i est vide on ne cree pas X+ ds $$ */
	break;
      }

      right_eval_result = 0;
    }

    /* Ici, left_eval_result > 0 <=> resultats multiples */

    prev_post_stack_top = DTOP (post_stack);
    done = SXFALSE;
      
#if IS_OPERATOR_LEX_REF || IS_OPERATOR_LEXEME_REF
    /* L'operateur OPERATOR_LEX_REF ou IS_OPERATOR_LEXEME_REF est ds le code genere pour la LFG */
    /* Si IS_OPERATOR_LEX_REF, les lexeme_id2display et args_ref_list de l'include lex_lfg_h sont visibles ... */
    if (store_right_pcs [-1] == OPERATOR_LEX_REF || store_right_pcs [-1] == OPERATOR_LEXEME_REF) {
      int  right_dol, fs_id, bot, top, field_id, val, lexeme_id, struct_id, right_val;
      int  *right_attr_ptr;

#if EBUG
      if (is_right_re)
	sxtrap (ME, "evaluate");
#endif /* EBUG */ 

      re_result_right_ptr = re_results+right_re_results_top;
      /* Normalement re_result_right_ptr->next == 0 */
      /* la syntaxe ne permet que ... = &lex $i ou ... = &lexeme $i */
      /* Un seul resultat => cas standard */
      right_val_ptr = re_result_right_ptr->ptr;
      right_kind = re_result_right_ptr->kind;

      right_dol = right_kind>>REF_SHIFT;

#if EBUG
      if (right_dol == 0)
	sxtrap (ME, "evaluate");
#endif /* EBUG */ 

      /* $$ X* = (&lex | &lexeme) $right_dol */
      /* Si $right_dol n'a pas de champ pred => echec de l'unification */
      fs_id = *right_val_ptr >> FS_ID_SHIFT;

      if (fs_id) {
	bot = XH_X (XH_fs_hd, fs_id);
	right_attr_ptr = &(XH_list_elem (XH_fs_hd, bot));
	field_id =  *right_attr_ptr & FIELD_AND;
      }
      else
	field_id == 0;

      if (field_id != pred_id) {
	/* Il n'y a pas de champ pred => echec de l'unification */
	/* Pour l'instant, le champ pred est evalue ds la 1ere passe (on ne sait pas specifier une autre passe !! ) */
	is_skip = ret_val = SXFALSE;
	break;
      }

      val = get_fs_id_atom_val (*right_attr_ptr);
      lexeme_id = XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val));

#if EBUG
      if (lexeme_id == 0)
	sxtrap (ME, "evaluate");
#endif /* EBUG */ 

#if IS_OPERATOR_LEX_REF
      if (store_right_pcs [-1] == OPERATOR_LEX_REF) {
	right_kind = (right_dol<<REF_SHIFT)+STRUCT_FIELD;

	/* On parcourt la liste des structures lexicales dont le lexeme a l'identifiant lexeme_id */
	bot = lexeme_id2display [lexeme_id];
	top = lexeme_id2display [lexeme_id+1];

	/* On parcourt la liste des structures lexicales dont le lexeme a l'identifiant lexeme_id */
	while (bot < top) {
	  /* On est ds le cas de disjonctions */
	  /* Appel avec Tpq == 0 => les fs_id trouves sont nouveaux ... */
	  fs_id =  call_tfs_instantiation (args_ref_list [bot], 0 /* Tpq */);
#if LOG
	  print_f_structure (0 /* pas de Xpq !! */, fs_id, SXFALSE, SXTRUE);
#endif /* LOG */

	  right_val = fs_id << FS_ID_SHIFT;

	  /* Le 17/02/05, changement pas de verif que l'unif va marcher !! */
	  if (left_eval_result == 0) {
	    /* Une reference unique en LHS */
	    /* On cherche les fs_id pour lesquels l'unif marche */
	    /* Normalement ici, [right_]father_wfs_id de la rhs est non significatif (on est ds fs) */
	    /* unifiable */
	    DCHECK (post_stack, 5);
	    DSPUSH (post_stack, right_kind);
	    DSPUSH (post_stack, right_val);

	    DSPUSH (post_stack, left_re_results_top);
	    DSPUSH (post_stack, operator);
	    DSPUSH (post_stack, POST_STACK_fs_id_x);
	  }
	  else {
	    for (x = left_eval_result; x != 0; x = re_results [x].next) {
	      re_result_left_ptr = re_results+x;
	      left_val_ptr = re_result_left_ptr->ptr;
	      left_kind = re_result_left_ptr->kind;

	      /* Sur le dernier couple x y, on ne fait pas le test avant d'unifier */
	      /* Normalement ici, [right_]father_wfs_id de la rhs est non significatif (on est ds fs) */
	      /* Pour l'instant, on conserve le 1er qui marche ... */
	      DCHECK (post_stack, 4);
	      DSPUSH (post_stack, right_val);
	      DSPUSH (post_stack, x);
	      DSPUSH (post_stack, operator);
	      DSPUSH (post_stack, POST_STACK_fs_id_x);
	    }
	  }

	  bot++;
	}
      }
#endif /* IS_OPERATOR_LEX_REF */

#if IS_OPERATOR_LEXEME_REF
      if (store_right_pcs [-1] == OPERATOR_LEXEME_REF) {
	/* Il faut transformer lexeme_id en atom_id */
	int  lexeme_string_lgth, atom_id, local_atom_id, ufs_id;
	char *lexeme_string;

	lexeme_string = lexeme_id2string [lexeme_id];
	/* lexeme_string == blabla_____i */
	lexeme_string_lgth = strlen (lexeme_string)-6;
	atom_id = sxword_2save (&atom_names, lexeme_string, lexeme_string_lgth);
	X_set (&atom_id2local_atom_id_hd, atom_id, &local_atom_id);
	XH_push (XH_ufs_hd, local_atom_id);
	XH_set (&XH_ufs_hd, &ufs_id);
	*(re_result_right_ptr->ptr) = ufs_id; /* A VOIR */
	re_result_right_ptr->kind = ATOM_VAL; /* A VOIR */

	if (left_eval_result == 0) {
	  /* Un seul resultat a gauche */
	  ret_val = call_unify (operator, left_re_results_top, right_re_results_top);
	  done = SXTRUE;
	}
	else {
	  /* Le 17/02/05 on stocke sans faire de check (il est un peu suspect) */
	  /* les resultats a gauche et a droite sont disponibles ds re_results */
	  for (x = left_re_results_top; x != 0; x = re_results [x].next) {
	    DCHECK (post_stack, 4);
	    DSPUSH (post_stack, x);
	    DSPUSH (post_stack, right_re_results_top);
	    DSPUSH (post_stack, operator);
	    DSPUSH (post_stack, POST_STACK_x_y);
	  }
	}
      }
#endif /* IS_OPERATOR_LEXEME_REF */
    }
    else 
#endif /* IS_OPERATOR_LEX_REF || IS_OPERATOR_LEXEME_REF */
      {
      /* Ici, right_eval_result > 0 <=> resultats multiples */
      if (left_eval_result == 0 && right_eval_result == 0) {
	/* Un seul resultat a gauche et a droite */
	/* Le 06/04/04 changement de l'appel */
	ret_val = call_unify (operator, left_re_results_top, right_re_results_top);
	done = SXTRUE;
      }
      else {
	/* Le 17/02/05 on stocke sans faire de check (il est un peu suspect) */
	/* les resultats a gauche et a droite sont disponibles ds re_results */
	for (x = left_re_results_top; x != 0; x = re_results [x].next) {
	  for (y = right_re_results_top; y != 0; y = re_results [y].next) {
	    DCHECK (post_stack, 4);
	    DSPUSH (post_stack, x);
	    DSPUSH (post_stack, y);
	    DSPUSH (post_stack, operator);
	    DSPUSH (post_stack, POST_STACK_x_y);
	  }
	}
      }
    }
	
    /* Le 17/02/05 toutes les disjonctions sont mise en reserve pour post_evaluate, sans verification qu'elles
       peuvent marcher */
    if (!done) {
      /* Ici plusieurs unif peuvent marcher */
      DPUSH (post_display, prev_post_stack_top);
    }

    break;

  case OPERATOR_NONEXIST:
  case OPERATOR_EXIST:
    if (pcs [-1] == OPERATOR_RE)
      /* On le saute ... */
      pcs--;

    left_eval_result = re_eval (0, SXFALSE, SXFALSE);

    /* La liste des resultats se trouve en re_results [left_eval_result] */
    /* Pour l'instant, on suppose que ca suffit !! */
    ret_val = (left_eval_result > 0);

    if (operator == OPERATOR_NONEXIST)
      ret_val = !ret_val;

    break;

  default:
    sxtrap (ME, "evaluate");
  }

  if (is_operator_possible)
    /* On execute quand meme l'equation suivante */
    ret_val = SXTRUE;

  /* ret_val == SXTRUE => executer l'equation suivante */
  /* ret_val == SXFALSE => Echec total */
#if LOG
  printf ("%s ... %s\n", single_fs_id ? " (single)" : "", ret_val ? (is_skip ? "skip" : "true") : "false");
#endif /* LOG */

  if (ret_val && !is_skip /* equation executee qui a marche' */) {
    valid_eq_nb++;
    /* On note le fs_id de $i ... si c'est une equation simple ($$ = $i)*/
    valid_single_fs_id = single_fs_id;
  }

  return ret_val;
}




/* On decroche les results marques */
static void
unhook_all ()
{
  int top, father_wfs_id, field_id;

  for (top = re_results [0].kind; top > 0; top--) {
    if (re_results [top].is_marked) {
      father_wfs_id = re_results [top].father_wfs_id;

      if (father_wfs_id) {
	father_wfs_id = get_equiv (father_wfs_id);
	field_id = *(re_results [top].ptr) & FIELD_AND;
	/* On "decroche" le champ field_id */
	SXBA_0_bit (wfs_field_set [AREA_PAGE (father_wfs_id)] [AREA_DELTA (father_wfs_id)], field_id);
      }
    }
  }
}

/* On re_acroche top (et tous ses ancetres marques) */
static SXBOOLEAN
rehook (top)
     int top;
{
  int     prev_top, father_wfs_id, field_id;
  SXBOOLEAN ret_val = SXFALSE;

  if (re_results [top].is_marked) {
    father_wfs_id = re_results [top].father_wfs_id;

    if (father_wfs_id) {
      father_wfs_id = get_equiv (father_wfs_id);
      field_id = *(re_results [top].ptr) & FIELD_AND;
      /* On "re_acroche" le champ field_id ... */
      /* ... s'il n'y est pas deja ... */
      /* ... et on le note pour unhook () */
      if (SXBA_bit_is_reset_set (wfs_field_set [AREA_PAGE (father_wfs_id)] [AREA_DELTA (father_wfs_id)], field_id)) {
	DPUSH (post_hook_stack, top);
	ret_val = SXTRUE;

	if (prev_top = re_results [top].prev_result)
	  rehook (prev_top);
      }
    }
  }

  return ret_val;
}

/* On decroche le contenu de post_hook_stack entre bot+1 et top */
static void
unhook (bot)
     int bot;
{
  int top, father_wfs_id, field_id;

  while (DTOP (post_hook_stack) > bot) {
    top = DPOP (post_hook_stack);
    father_wfs_id = re_results [top].father_wfs_id;
    father_wfs_id = get_equiv (father_wfs_id);
    field_id = *(re_results [top].ptr) & FIELD_AND;
    SXBA_0_bit (wfs_field_set [AREA_PAGE (father_wfs_id)] [AREA_DELTA (father_wfs_id)], field_id);
  }
}

/* On vient de terminer l'evaluation d'un bloc d'equations dont le resultat est ds wfs
   mais certaines equations correspondent a des disjonctions multiples (expressions regulieres, operateur lex)
   n'ont pas ete instanciees, on le fait */
/* PB Y'a des trucs qu'on ne sait pas faire : ($$ . (X|Y)* X = $$ . (X|Y)* Y) ;  !! */
/* On traite le "niveau" cur_post */
/* Nelle version qui utilise les "marques" */
/* rend SXFALSE ssi aucune f-structure n'a pu être construite (échecs d'unification) ; s'il y a des struct, rend SXTRUE qu'il y ait ou non des struct cohérentes */
static SXBOOLEAN
post_evaluate (Aij, cur_post)
     int    Aij, cur_post;
{
  int              x, y, xstore, fs_id, head, x2, y2;
  int              left_kind, right_kind, right_val, post_stack_kind, operator;
  int              top_post, cur, top;
  int              *right_val_ptr, *left_val_ptr;
  int              left_field_id_path, right_field_id_path;
  struct re_result *re_result_left_ptr, *re_result_right_ptr, *re_result_ptr;
  SXBOOLEAN          done, local_call, ret_val;

  ret_val = SXFALSE;

  if (cur_post > (top_post = DTOP (post_display))) {
    /* fin d'un spine */
    /* Les pred_nb on pu etre positionnes (par fill_preds) par le traitement d'un spine precedant */
    for (x = field_val2adjunct_id [0].equiv; x > 0; x--) {
      field_val2adjunct_id [x].pred_nb = 0;
    }

    if (fs_id = fill_fs (Aij)) {
      head = fill_heads (Aij, fs_id);

      SXBA_1_bit (Xpq_set, Aij);
#if LOG
      print_f_structure (Aij, fs_id, is_locally_unconsistent, SXTRUE);
      fputs ("\n", stdout);
#endif /* LOG */

      DPUSH (fs_id_Pij_dstack, head);
      DPUSH (fs_id_Pij_dstack, doldol);
      fs_id_Pij_dstack [Pij_cur_bot] += 2;
      ret_val = SXTRUE;
    }
#if LOG
    else
      print_wf_structures ();
#endif /* LOG */
  }
  else {
    /* On sauve le wfs precedent ... */
    xstore = put_in_store_wfs ();
    /* ... en xstore */

    if (cur_post = top_post)
      top = DTOP (post_stack);
    else
      top = post_display [cur_post+1]-1;

    cur = post_display [cur_post];

    while (cur < top) {
      post_stack_kind = post_stack [top--];
      operator = post_stack [top--];

      if (post_stack_kind == POST_STACK_x_y) {
	y = post_stack [top--];
	x = post_stack [top--];
      }
      else {
	/* References multiples en LHS */
	/* post_stack_kind == POST_STACK_fs_id_x */
	x = post_stack [top--];

	y = re_results_get_new_top ();
	re_result_ptr = re_results+y;
	right_val = post_stack [top--];
	right_kind = post_stack [top--];
	  
	re_result_ptr->ptr = &right_val;
	re_result_ptr->kind = right_kind;
      }

      /* On "racroche" x et y si necessaire ... */
      y2 = DTOP (post_hook_stack);

      if (!rehook (y))
	y2 = -1;

      x2 = DTOP (post_hook_stack);

      if (!rehook (x))
	x2 = -1;
			
      local_call = call_unify (operator, x, y);

      if (local_call) {
	if (post_evaluate (Aij, cur_post+1)) {
	  ret_val = SXTRUE;
	}
      }

      /* ... et on les "decroche" pour les autres couples */
      if (x2 >= 0)
	unhook (x2);

      if (y2 >= 0)
	unhook (y2);

      if (cur < top)
	/* On [re-]remplit wfs correctement */
	set_wfs_from_store_wfs (xstore);
    }

    close_store_wfs_level (xstore); /* On "recupere" xstore */
  }

  return ret_val;
}




static SXBOOLEAN
call_post_evaluate (Aij)
     int    Aij;
{
  SXBOOLEAN ret_val;

#if LOG
  fputs ("Post evaluation\n", stdout);
#endif /* LOG */

  unhook_all ();

  ret_val = post_evaluate (Aij, 1);

#if LOG
  printf ("%s of the post evaluation\n", ret_val ? "Success" : "Failure");
#endif /* LOG */

  DRAZ (post_display);
  DRAZ (post_stack);

  return ret_val;
}


static void
set_failure_set (prhs, pos, n)
     struct rhs *prhs;
     int        pos, n;
{
  int Xpq, fs_id_nb, i;

  if (pos == 0) {
    /* fini ... */
    SXBA_1_bit (failure_set, n);
  }
  else {
    Xpq = prhs [pos].lispro;

    if (Xpq < 0)
      /* terminal */
      Xpq = spf.outputG.maxxnt-Xpq;
		  
    fs_id_nb = fs_id_dstack [Xpq2disp [Xpq]];
    n *= fs_id_nb;

    if (SXBA_bit_is_set (doli_used_set, pos))
      set_failure_set (prhs, pos-1, n+rhs_pos2digit [pos]);
    else {
      for (i = 0; i < fs_id_nb; i++) {
	set_failure_set (prhs, pos-1, n+i);
      }
    }
  }
}


/* Cas UNFOLD */
static void
clear_Xpq (Xpq)
     int Xpq;
{
  int d, pair;
  int *base_ptr;

  base_ptr = fs_id_dstack + Xpq2disp [Xpq];
  d = *base_ptr++;
  Xpq2disp [Xpq] = 0;

  while (d--) {
    pair = *base_ptr++;

    if (pair > 0)
      XxY_erase (heads, pair);
    else {
      if (pair < 0)
	XxY_erase (heads, -pair);
    }
  }
}

static void
clear_Xpq_set ()
{
  int Xpq;

  Xpq= -1;

  while ((Xpq= sxba_scan_reset (Xpq_set, Xpq)) >= 0)
    clear_Xpq (Xpq);
}

static void
clear_for_relaxed_run ()
{
  int *p;

  p = Pij2disp+spf.outputG.maxxprod;

  do {
    *p = 0;
  } while (--p >= Pij2disp);

  DRAZ (fs_id_Pij_dstack);

  clear_Xpq_set ();
}

/* On "conserve" les Aij de consistent_Xpq_set et les Pij de consistent_Pij_set */
static void
partial_clear_for_relaxed_run ()
{
  int Xpq, nb, x, top, old_head, new_head, head, rule;
  int *new_fs_id_dstack, *old_p, *new_p;
  XxY_header new_heads;
  int Pij;
  int *new_Pij2disp, *new_fs_id_Pij_dstack;

#if 0
  /* Non, verification erronee : la somme du nb de heads stocke ds fs_id_Pij_dstack pour les Aij-prods
     n'est pas le nb de heads stocke ds fs_id_dstack pour Aij, car le meme head peut se retrouver plusieurs fois
     ds fs_id_Pij_dstack (prods differentes ou meme prod et doldol differents) */
  int *Xpq_nb;

  Xpq_nb = (int *) sxcalloc (spf.outputG.maxnt+1, sizeof (int));
#endif /* 0 */

  XxY_alloc (&new_heads, "heads", XxY_size (heads)+1, 1, 0 /* pas de foreach */, 0, NULL /* heads_oflw */, NULL);

  /* Le fond de new_heads doit etre identique a heads sur les valeurs statiques */
  for (old_head = 1; old_head <= static_heads_top; old_head++) {
    XxY_set (&new_heads, XxY_X (heads, old_head), XxY_Y (heads, old_head), &new_head);
  }

  new_Pij2disp = (int*) sxcalloc (spf.outputG.maxxprod+1, sizeof (int));
  DALLOC_STACK (new_fs_id_Pij_dstack, DSIZE (fs_id_Pij_dstack));

  Pij = -1;

  while ((Pij = sxba_scan (consistent_Pij_set, Pij)) > 0) {
#if EBUG
    Xpq = spf.outputG.lhs [Pij].lhs;

    if (!SXBA_bit_is_set (consistent_Xpq_set, Xpq))
      sxtrap (ME, "partial_clear_for_relaxed_run");
#endif /* EBUG */

    old_p = fs_id_Pij_dstack+Pij2disp [Pij];
    new_p = new_fs_id_Pij_dstack + (new_Pij2disp [Pij] = DTOP (new_fs_id_Pij_dstack)+1);
	
    for (rule = PROD2RULE (spf.outputG.lhs [Pij].init_prod); rule != 0; rule = rule2next_rule [rule]) {
      nb = *old_p++;
      DTOP (new_fs_id_Pij_dstack) += nb+1; /* ca tient */
      *new_p++ = nb;

      while (nb-- > 0) {
	old_head = *old_p++;

	head = (old_head < 0) ? -old_head : old_head;
	
	XxY_set (&new_heads, XxY_X (heads, head), XxY_Y (heads, head), &new_head);

	*new_p++ = (old_head < 0) ? -new_head : new_head;

	/* Les doldol ... */
	*new_p++ = *old_p++;
	nb--;
      }
    }
  }

  DFREE_STACK (fs_id_Pij_dstack);
  sxfree (Pij2disp);

  fs_id_Pij_dstack = new_fs_id_Pij_dstack;
  Pij2disp = new_Pij2disp;

  DALLOC_STACK (new_fs_id_dstack, DSIZE (fs_id_dstack));

#if 0
  if (spf.outputG.maxxnt > spf.outputG.maxnt)
    /* Y'a des nt en erreur */
    x = Xpq2disp [spf.outputG.maxxnt];
  else
    x = Xpq2disp [Xpq_top];
#endif /* 0 */

  x = Xpq2disp [Xpq_top]; /* Le 20/02/06 ds tous les cas */

  new_fs_id_dstack [0] = top = x + fs_id_dstack [x];

  /* On doit tout conserver jusqu'a top ds fs_id_dstack */
  for (x = 1; x <= top; x++)
    new_fs_id_dstack [x] = fs_id_dstack [x];

  /* ... et on met les trucs conserves */
  /* C'est deja fait pour les terminaux */
  for (Xpq = 1; Xpq <= spf.outputG.maxxnt; Xpq++) {
    if (SXBA_bit_is_set (consistent_Xpq_set, Xpq)) {
      nb = *(old_p = fs_id_dstack + Xpq2disp [Xpq]);
      new_p = new_fs_id_dstack + (Xpq2disp [Xpq] = DTOP (new_fs_id_dstack)+1);
      DTOP (new_fs_id_dstack) += nb+1; /* ca tient */
      *new_p++ = nb;
      old_p++;

      while (nb-- > 0) {
	old_head = *old_p++;

	head = (old_head < 0) ? -old_head : old_head;

	XxY_set (&new_heads, XxY_X (heads, head), XxY_Y (heads, head), &new_head);

	*new_p++ = (old_head < 0) ? -new_head : new_head;
      }
    }
    else
      Xpq2disp [Xpq] = 0;
  }

  DFREE_STACK (fs_id_dstack);
  fs_id_dstack = new_fs_id_dstack;

  XxY_free (&heads);
  heads = new_heads;

  sxba_empty (Xpq_set);
}


/* Pour chaque Xpq, On ne conserve que les f_structures de prio maximale */


/* Ft generique qui ne conserve que les heads de working_heads_stack telles que les f(fs_id) soient maximaux */
static SXBOOLEAN
max_ranking (int (*f)()
#if LOG
	     , char *f_name
#endif /* LOG */
	     )

{
  int     head_nb, max_value, head, fs_id, value, nb, dist;
  int     *bot_heads_stack, *top_heads_stack, *bot_value_stack, *top_value_stack;

  head_nb = working_heads_stack [0];
  top_heads_stack = working_heads_stack+head_nb;
  top_value_stack = working_value_stack+head_nb;
  max_value = 0;

  do {
    head = *top_heads_stack--;

#if EBUG
    if (head == 0)
      sxtrap (ME, "max_ranking");
#endif /* EBUG */

    if (head < 0)
      head = -head;

    fs_id = XxY_Y (heads, head);

    if (fs_id > 0) {
      *top_value_stack = value = f (fs_id);

#if LOG
      printf("max_ranking (%s) sur F%i donne %i\n", f_name, fs_id, value);
#endif /* LOG */

      if (value > max_value)
	max_value = value;
    }
    else
      *top_value_stack = 0;

    top_value_stack--;
  } while (top_heads_stack > working_heads_stack);

  working_heads_stack [0] = 0;

  bot_heads_stack = working_heads_stack;
  top_heads_stack = working_heads_stack+head_nb;
  bot_value_stack = working_value_stack;

  while (++bot_heads_stack <= top_heads_stack) {
    value = *++bot_value_stack;

    if (value == max_value) {
      head = *bot_heads_stack;
      DSPUSH (working_heads_stack, head);
    }
  }

  return working_heads_stack [0] < head_nb;
}


/* Ft generique qui ne conserve que les heads de working_heads_stack telles que les f(fs_id) soient minimaux */
static SXBOOLEAN
min_ranking (int (*f)()
#if LOG
	     , char *f_name
#endif /* LOG */
	     )
{
  /* Pour chaque f_structure Fk on recherche ses sous-structures Fj qui contiennent des nc ayant des det => poids = 0
     Si pas det ou det = - +> poids += 1.
     On prend le min  
   */
  int     head_nb, min_value, head, fs_id, value, nb, dist;
  int     *bot_heads_stack, *top_heads_stack, *bot_value_stack, *top_value_stack;

  head_nb = working_heads_stack [0];
  top_heads_stack = working_heads_stack+head_nb;
  top_value_stack = working_value_stack+head_nb;
  min_value = -1;

  do {
    head = *top_heads_stack--;

#if EBUG
    if (head == 0)
      sxtrap (ME, "min_ranking");
#endif /* EBUG */

    if (head < 0)
      head = -head;

    fs_id = XxY_Y (heads, head);

    if (fs_id > 0) {
      *top_value_stack = value = f (fs_id);

#if LOG
      printf("min_ranking (%s) sur F%i donne %i\n", f_name, fs_id, value);
#endif /* LOG */

      if (min_value == -1 || value < min_value)
	min_value = value;
    }
    else
      *top_value_stack = -1;

    top_value_stack--;
  } while (top_heads_stack > working_heads_stack);

  working_heads_stack [0] = 0;

  bot_heads_stack = working_heads_stack;
  top_heads_stack = working_heads_stack+head_nb;
  bot_value_stack = working_value_stack;

  while (++bot_heads_stack <= top_heads_stack) {
    value = *++bot_value_stack;

    if (value == min_value) {
      head = *bot_heads_stack;
      DSPUSH (working_heads_stack, head);
    }
  }

  return working_heads_stack [0] < head_nb;
}

#if FS_PROBAS
static double get_ctxt_weight ();

/* A partir de head, recherche la ou les Ppq qui ont head et recherche ds les tables d'apprentissage lepoids de (Ppq, fs_id) */
/* retourne l'index de la (meilleure) proba associee a head */
static double
get_head_weight (head)
     int head;
{
  int    item, hook, Ppq, x, nb, Ppq_head, rule, Xpq, fs_id;
  int    *p;
  double weight, cur_weight;

  if (head < 0) {
    Xpq = XxY_X (heads, -head);
    fs_id = XxY_Y (heads, -head);
  }
  else {
    Xpq = XxY_X (heads, head);
    fs_id = XxY_Y (heads, head);
  }

  weight = min_logprob;

  if (fs_id > 0) {
    hook = spf.outputG.lhs [spf.outputG.maxxprod+Xpq].prolon;

    while ((Ppq = spf.outputG.rhs [hook++].lispro) != 0) {
      if (Ppq > 0 && (x = Pij2disp [Ppq]) > 0) {
	p = fs_id_Pij_dstack+x;

	for (rule = PROD2RULE (spf.outputG.lhs [Ppq].init_prod); rule != 0; rule = rule2next_rule [rule]) {
	  nb = *p++;
	
	  while (nb-- > 0) {
	    Ppq_head = *p++;

	    if (*p++ != -1)
	      cur_weight = (Ppq_head == head) ? /* On en a trouve' un */ get_ctxt_weight (Ppq, fs_id) : min_logprob;
	    else
	      cur_weight = min_logprob_fs-1;

	    if (cur_weight > weight)
	      weight = cur_weight;

	    nb--;
	  }
	}
      }
    }

#if EBUG
    if (weight == min_logprob)
      sxtrap (ME, "get_Xpq_fs_id_weight");
#endif /* EBUG */
  }

  return weight;
}


/* On utilise les (inside) probas calculees sur les f_structures */
/* Pour l'instant on ne conserve que les f_structures de proba max (on pourrait faire du beam) */
static SXBOOLEAN
ranking_by_probabilities ()
{
  /* Pour chaque f_structure on recherche le ou les Pij qui l'on produite, on recupere les probas
     et finalement on ne garde que celle[s] de proba max */
  int     head_nb, head, fs_id, best_x;
  int     *bot_heads_stack, *top_heads_stack;
  double  best_weight, weight, *bot_double_stack, *top_double_stack;

  head_nb = working_heads_stack [0];
  top_heads_stack = working_heads_stack+head_nb;
  top_double_stack = working_double_stack+head_nb;

  best_weight = min_logprob;

  /* 1ere passe qui pour chaque head memorise le meilleur poids
     Ce meilleur poids peut etre atteint par des productions differentes */
  do {
    head = *top_heads_stack--;

#if EBUG
    if (head == 0)
      sxtrap (ME, "ranking_by_probabilities");
#endif /* EBUG */

    *top_double_stack = weight = get_head_weight (head);

#if LOG
    fs_id = (head > 0) ? XxY_Y (heads, head) : XxY_Y (heads, -head);
    printf("ranking_by_probabilities sur F%i donne %f\n", fs_id, weight);
#endif /* LOG */

    if (weight > best_weight)
      best_weight = weight;

    top_double_stack--;
  } while (top_heads_stack > working_heads_stack);

  /* 2eme passe qui supprime les references ne correspondant pas au meilleur poids */
  working_heads_stack [0] = 0;

  bot_heads_stack = working_heads_stack;
  top_heads_stack = working_heads_stack+head_nb;
  bot_double_stack = working_double_stack;

  while (++bot_heads_stack <= top_heads_stack) {
    weight = *++bot_double_stack;

    if (weight == best_weight) {
      head = *bot_heads_stack;
      DSPUSH (working_heads_stack, head);
    }
  }

  return working_heads_stack [0] < head_nb;
}

static int         ranking_by_nbest_root_Aij;

/* faut-il visiter Pij ? */
static int
ranking_filtering_function (int Pij)
{
  /* ATTENTION, spf.walk.cur_Aij n'est pas positionne' !! */
  if ( spf.outputG.lhs [Pij].lhs == ranking_by_nbest_root_Aij) {
    /* On est sur une Pij de la racine de la sous-foret que l'on traite */
    return SXBA_bit_is_set (ranking_by_nbest_root_Pij_set, Pij);
  }

  /* On ne visite pas les prods ecartees */
  return !SXBA_bit_is_set (to_be_erased_Pij_set, Pij) && !SXBA_bit_is_set (erased_by_ranking_Pij_set, Pij);
}

static int filter_Pij_heads (int *head_stack, int Aij, SXBOOLEAN called_from_perform_inside_ranking);
/* Si les f_structures restantes correspondent a plusieurs Pij, pour chacun de ces Pij on va calculer
   son poids par 1best et on ne va conserver que la/les f_structures associees au Pij de poids max */
static SXBOOLEAN
ranking_by_nbest (int Aij)
{  
  int     valid_Pij_nb, hook, base_hook, Pij, best_Pij, x, nb, head, rule, real_nbest, fs_id;
  int     *Pij_stack;

  /* On commence par appeler filter_Pij_heads pour prendre en compte les ranking precedents eventuels */
  valid_Pij_nb = filter_Pij_heads (working_heads_stack, Aij, SXTRUE /* modifie erased_by_ranking_Pij_set */);

  if (valid_Pij_nb <= 1) {
    /* Toutes les f_struct proviennent de la meme Pij */
#if LOG
    printf("No 'n' ranking (unique Pij)\n");
#endif /* LOG */

    return SXFALSE;
  }

  if (ranking_by_nbest_root_Pij_set == NULL)
    ranking_by_nbest_root_Pij_set = sxba_calloc (spf.outputG.maxxprod+1);

  ranking_by_nbest_root_Aij = Aij;

  hook = base_hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;

  while ((Pij = spf.outputG.rhs [hook++].lispro) != 0) {
    if (Pij > 0 && (x = Pij2disp [Pij]) > 0) {
      SXBA_1_bit (ranking_by_nbest_root_Pij_set, Pij);
    }
  }

  /* On sauve l'etat courant  de "topological" */
  nbest_save_cur_walk = spf.walk;
  spf.walk = nbest_save_nbest_walk; /* vide le 1er coup */

  if (spf.walk.Aij_top == 0) {
    spf.walk.Aij_top = nbest_save_cur_walk.Aij_top;
    spf.walk.user_filtering_function = ranking_filtering_function; /* Filtrage de la foret pour nbest */
  }

  /* la 1ere fois alloue les structureset les fois suivantes les nettoie pour une execution clean */
  nbest_allocate (1, EPCFG);

  real_nbest = nbest_perform (Aij, 1 /* nbest==1 */, 0 /* beam */, EPCFG, SXFALSE);

#if EBUG
  if (real_nbest != 1)
    sxtrap (ME, "ranking_by_nbest");
#endif /* EBUG */

  best_Pij = nbest_get_Pij (Aij, 1, EPCFG); /* On recupere le meilleur des Pij */

#if LOG
  printf("ranking_by_1best sur Pij=%i donne %f\n", best_Pij, nbest_get_weight (Aij, 1, EPCFG));
#endif /* LOG */

  /* On restore l'etat courant  de "topological" */
  nbest_save_nbest_walk = spf.walk;
  spf.walk = nbest_save_cur_walk;

  hook = base_hook;

  while ((Pij = spf.outputG.rhs [hook++].lispro) != 0) {
    if (Pij > 0 && (x = Pij2disp [Pij]) > 0) {
      SXBA_0_bit (ranking_by_nbest_root_Pij_set, Pij);

      if (Pij != best_Pij) {
	/* On l'elimine */
	SXBA_1_bit (erased_by_ranking_Pij_set, Pij);
	Pij2disp [Pij] = 0;
      }
    }
  }

  if (best_Pij) {
    /* On "recopie" le/les heads de best_Pij ds Aij */
    DRAZ (working_heads_stack);

    x = Pij2disp [best_Pij];
    Pij_stack = fs_id_Pij_dstack+x;
	
    for (rule = PROD2RULE (spf.outputG.lhs [best_Pij].init_prod); rule != 0; rule = rule2next_rule [rule]) {
      nb = *Pij_stack++;

      while (nb-- > 0) {
	head = *Pij_stack++;
	DSPUSH (working_heads_stack, head);

#if LOG
	fs_id = (head > 0) ? XxY_Y (heads, head) : XxY_Y (heads, -head);
	printf("ranking_by_1best garde F%i pour Pij=%i\n", fs_id, best_Pij);
#endif /* LOG */

	Pij_stack++; /* doldol */
	nb--;
      }
    }
  }
  
  return best_Pij > 0;
}
#endif /* FS_PROBAS */

/* static SXBA ranking_by_args_fs_id_set; */


/* retourne le "dpv_ptr" du champ pred de la f_structure fs_id */
static int*
seek_pred (fs_id)
     int fs_id;
{
  int  val;
  int  *val_ptr;
      
  if (fs_is_set (fs_id, pred_id, &val_ptr) && (val = get_fs_id_atom_val (*val_ptr))) {
    return &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)));
  }

  return NULL;
}

/* Retourne le code interne de la tete terminale d'un champ pred d'un f_structure */
static int
fs_id2tete (fs_id)
     int fs_id;
{
  int *dpv_ptr, Tpq, t_code;

  if ((dpv_ptr = seek_pred (fs_id)) && (Tpq = dpv_ptr [4])) {
    Tpq -=  spf.outputG.maxxnt;
    t_code = -Tij2T (Tpq);
  }
  else
    t_code = 0;

  return t_code;
}


/* Calcul du poids des args de fs_id (et des poids de ses sous structures) */
static int
get_args_weight (fs_id)
     int fs_id;
{
  int x, weight, bot, cur, top, val, field_id, bot2, cur2, top2, nb_arg, nb_mod, nb_aux, atom_name, tense_name;
  int nb_coo, t_code, val_orig;
  unsigned char   static_field_kind;
  SXBOOLEAN has_pred;

  if (fs_id == 0)
    return 0;

  if (!SXBA_bit_is_reset_set (fs_id_set, fs_id))
    /* cyclique */
    return 0; /* element neutre */

  /* Pour l'instant, on ne memoize pas */

  weight = 0;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  nb_arg = nb_coo = nb_mod = nb_aux = 0;

  cur = bot;

  while (cur < top) {
    val_orig = val = XH_list_elem (XH_fs_hd, cur);
    field_id = val & FIELD_AND;

    if (cur == bot) { // BS
      has_pred = (field_id == pred_id); // BS
    } // BS

    if (has_pred //BS
       && is_executable (field_id)) {
      /* Ce champ est calcule' ds cette passe */
      if (field_id == pred_id) {
	/* On va prendre tous les arguments, meme si non presents ds pred */
#ifdef np_code
	if ((t_code = fs_id2tete (fs_id)) == np_code) {
	  SXBA_0_bit (fs_id_set, fs_id);
	  return 0;
	}
#endif /* np_code */
      }
      else {
	val >>= FS_ID_SHIFT;

	if (val > 0) {
	  static_field_kind = field_id2kind [field_id];

	  if (static_field_kind == STRUCT_KIND) {
#ifdef EASY
	    if (field_id == arg1_id || field_id == arg2_id) {
	      nb_coo++;
	      weight += get_args_weight (val)/2;
	    }
	    else
#endif /* EASY */
	      {
		nb_arg++;

		if (SXBA_bit_is_set (argument_set, field_id)) {
		  /* C'est un argument */ 
		  weight += get_args_weight (val);
		}
	      }
	  }
	  else {
	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	      /* On suppose que les "set of struct" ne sont pas des arguments !! */
	      for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		   cur2 < top2;
		   cur2++) {
		val = XH_list_elem (XH_ufs_hd, cur2);

		if (val < 0)
		  break;

		nb_mod++;
		weight += get_args_weight (val)/4;
	      }
	    }
#ifdef EASY
	    else {
	      if (field_id == form_aux_id) {
		nb_aux++;
	      }
	      else {
		if (field_id == diathesis_id || field_id == aspect_id) {
		  val = get_fs_id_atom_val (val_orig);
#ifndef ESSAI
		  atom_name = field_id2atom_field_id [field_id] [atom_id2local_atom_id (val)];
#else /* ESSAI */
		  atom_name = X_X (atom_id2local_atom_id_hd, val);
#endif /* ESSAI */

		  if (field_id == diathesis_id) {
		    if (atom_name == atom_passive && (tense_name = get_atomic_field_val (fs_id, tense_id)) && tense_name != atom_none)
		      nb_aux++;
		  }
#if 0
		  /* BS: terminative semble avoir disparu */
		  else {
		    if (tense_name = get_atomic_field_val (fs_id, aspect_id) && tense_name == atom_terminative)
		      nb_aux++;
		  }
#endif /* 0 */
		}
	      }
	    }
#endif /* EASY */
	  }
	}
      }
    }

    cur++;
  }

  SXBA_0_bit (fs_id_set, fs_id);

  return (nb_arg+nb_mod+nb_aux+nb_coo) ? ((weight+4096*nb_aux+1024*nb_arg)/(nb_arg+nb_mod+nb_aux+(nb_coo ? 1 : 0))) : 0;
}

/* preferer les arguments aux modifieurs */



/* Calcul de la distance moyenne des args de fs_id (et de ses sous structures) */
/* retourne ds nb le nb de distances calculees et ds dist le cumul des distances */
static void
get_args_vicinity (fs_id, nb, dist)
     int fs_id, *nb, *dist;
{
  int     bot, top, val, field_id, bot2, cur2, top2, val_orig;
  int     Tpq, p, q, Tij, i, j, store_bot, son_val, d;
  int     *dpv_ptr;
  unsigned char   static_field_kind;

  if (fs_id == 0)
    return;

  if (!SXBA_bit_is_reset_set (fs_id_set, fs_id))
    /* cyclique */
    return;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  val = XH_list_elem (XH_fs_hd, bot);

  if ((val & FIELD_AND) == pred_id) {
    /* On va prendre tous les arguments, meme si non presents ds pred */
    val = get_fs_id_atom_val (val);
    Tpq = XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)+4);

    if (Tpq) {
      Tpq -= spf.outputG.maxxnt;
      p = Tij2i (Tpq);
      q = Tij2j (Tpq);
      /*
      p = mlstn2dag_state (p);
      q = mlstn2dag_state (q);
      */

      store_bot = bot;
  
      while (++bot < top) {
	val = XH_list_elem (XH_fs_hd, bot);
	field_id = val & FIELD_AND;

	if (!is_executable (field_id))
	  /* Ce champ n'est pas calcule' ds cette passe */
	  continue;

	if (SXBA_bit_is_set (argument_set, field_id)) {
	  val >>= FS_ID_SHIFT;

	  if (val > 0) {
	    /* val est un fs_id */
	    son_val =  XH_list_elem (XH_fs_hd, XH_X (XH_fs_hd, val));

	    if ((son_val & FIELD_AND) == pred_id) {
	      son_val = get_fs_id_atom_val (son_val);

	      Tij = XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, son_val)+4);
	      
	      if (Tij) {
		Tij -= spf.outputG.maxxnt;
		i = Tij2i (Tij);
		j = Tij2j (Tij);
		/*
		  i = mlstn2dag_state (i);
		  j = mlstn2dag_state (j);
		*/

		if (q <= i) {
		  /* Tpq est "avant" Tij */
		  d = i-q+1;
		}
		else {
		  if (j <= p) {
		    /* Tpq est "apres" Tij */
		    d = p-j+1;
		  }
		  else {
		    /* ?? on dit distance nulle, mais on compte ds la moyenne */
		    d = 0;
		  }
		}

#if 0
		/* BS :  ATTENTION: CE IF 0 EST PROVISOIRE, IL FAUT ETUDIER EN DETAILS */
#ifdef EASY
		if (SXBA_bit_is_set (acomp_ids_set, field_id)) {
		  /* On donne anti-priorite aux *acomp */
		  *nb += 1;
		  *dist += d;
		}
		else
#endif /* EASY */ 
#endif /* 0 */
		  {
		    /* On travaille en entier */
		    *nb += 2;
		    *dist += 2*d;
		  }
	      }
	    }
	    }
	}
      }

      bot = store_bot;
    }

    bot++; /* On saute pred */
  }

  /* visite sur les sous_structures */
  while (bot < top) {
    val = XH_list_elem (XH_fs_hd, bot);

    field_id = val & FIELD_AND;

    if (is_executable (field_id)) {
      /* Ce champ est calcule' ds cette passe */
      val >>= FS_ID_SHIFT;

      if (val > 0) {
	static_field_kind = field_id2kind [field_id];

	if (static_field_kind == STRUCT_KIND) {
	  get_args_vicinity (val, nb, dist);
	}
	else {
	  if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	    /* On suppose que les "set of struct" ne sont pas des arguments !! */
	    for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		 cur2 < top2;
		 cur2++) {
	      val = XH_list_elem (XH_ufs_hd, cur2);

	      if (val <0)
		break;

	      get_args_vicinity (val, nb, dist);
	    }
	  }
	}
      }
    }

    bot++;
  }

  SXBA_0_bit (fs_id_set, fs_id);
}


/* Preferer les arguments proches */
static SXBOOLEAN
ranking_by_vicinity ()
{
  /* Si F = [pred = "a<obj>"; ... obj = [pred = "b<...>", ...], ...] est une sous-structure, on calcule
     la distance entre les lexemes "a" et "b" (regarder quoi noter a l'instanciation des feuilles pour
     pouvoir le calculer ici).  Pour une structure, on calcule la moyenne de ses distances et on prefere
     les structures de distance moyenne minimale.
  */
  int     head_nb, min_vicinity, head, fs_id, vicinity, nb, dist;
  int     *bot_heads_stack, *top_heads_stack, *bot_value_stack, *top_value_stack;

  head_nb = working_heads_stack [0];
  top_heads_stack = working_heads_stack+head_nb;
  top_value_stack = working_value_stack+head_nb;
  min_vicinity = -1;

  do {
    head = *top_heads_stack--;

    if (head < 0)
      head = -head;

    fs_id = XxY_Y (heads, head);

    if (fs_id > 0) {
      nb = dist = 0;
      get_args_vicinity (fs_id, &nb, &dist);
      /* value_array [x] = vicinity = nb ? (1000*dist)/nb : 1000*n; */
      *top_value_stack = vicinity = nb ? dist : n;

#if LOG
      printf("ranking_by_vicinity sur F%i donne %i\n", fs_id, vicinity);
#endif /* LOG */

      if (min_vicinity == -1 || vicinity < min_vicinity)
	min_vicinity = vicinity;
    }
    else
      *top_value_stack = -1;

    top_value_stack--;
  } while (top_heads_stack > working_heads_stack);

  working_heads_stack [0] = 0;

  bot_heads_stack = working_heads_stack;
  top_heads_stack = working_heads_stack+head_nb;
  bot_value_stack = working_value_stack;

  while (++bot_heads_stack <= top_heads_stack) {
    vicinity = *++bot_value_stack;
    head = *bot_heads_stack;

    if (vicinity == min_vicinity) {
      DSPUSH (working_heads_stack, head);
    }
  }

  return working_heads_stack [0] < head_nb;
}

/* Calcul du niveau d'enchassement de fs_id */
static int
get_args_embedding (fs_id)
     int fs_id;
{
  int x, embedding, max_embedding, bot, top, val, field_id, bot2, cur2, top2;
  unsigned char   static_field_kind;

  if (fs_id == 0)
    return 0;

  if (!SXBA_bit_is_reset_set (fs_id_set, fs_id))
    /* cyclique */
    return 0; /* element neutre */

  /* Pour l'instant, on ne memoize pas */
  max_embedding = 0;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  while (bot < top) {
    val = XH_list_elem (XH_fs_hd, bot);

    field_id = val & FIELD_AND;

    if (is_executable (field_id)) {
      /* Ce champ est calcule' ds cette passe */
      val >>= FS_ID_SHIFT;

      if (field_id == pred_id) {
	/* On va prendre tous les arguments, meme si non presents ds pred */
      }
      else {
	if (val > 0 && (val & OPTIONAL_VAL) == 0) {
	  static_field_kind = field_id2kind [field_id];

	  if (static_field_kind == STRUCT_KIND) {
	    embedding = 1+get_args_embedding (val);

	    if (embedding > max_embedding)
	      max_embedding = embedding;
	  }
	  else {
	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	      /* On suppose que les "set of struct" ne sont pas des arguments !! */
	      for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		   cur2 < top2;
		   cur2++) {
		val = XH_list_elem (XH_ufs_hd, cur2);

		if (val < 0)
		  break;

		embedding = 1+get_args_embedding (val);
	      
		if (embedding > max_embedding)
		  max_embedding = embedding;
	      }
	    }
	  }
	}
      }
    }

    bot++;
  }

  SXBA_0_bit (fs_id_set, fs_id);

  return max_embedding;
}


static SXBOOLEAN
ranking_by_special_atomic_field (special_atomic_field_id)
     int special_atomic_field_id;
{
  int     head_nb, min_atom_val, head, fs_id, atom_val, nb, dist;
  int     *bot_heads_stack, *top_heads_stack, *bot_value_stack, *top_value_stack;
#ifdef ESSAI
  int     *val_ptr, objet_val;
  SXBOOLEAN is_shared;
#endif /* ESSAI */

  head_nb = working_heads_stack [0];
  top_heads_stack = working_heads_stack+head_nb;
  top_value_stack = working_value_stack+head_nb;
  min_atom_val = 0;

  do {
    head = *top_heads_stack--;

    if (head < 0)
      head = -head;

    fs_id = XxY_Y (heads, head);

    if (fs_id > 0) {
#ifndef ESSAI
      atom_val = get_atomic_field_val (fs_id, special_atomic_field_id);
#else /* ESSAI */
      if (fs_is_set (fs_id, special_atomic_field_id, &val_ptr) && (objet_val = get_ufs_atom_val (*val_ptr >> FS_ID_SHIFT, &is_shared, left_local_atom_id_set))) {
	/* Si disjonction non triviale, on ne prend que la 1ere */
	atom_val = X_X (atom_id2local_atom_id_hd, objet_val);
	sxba_empty (left_local_atom_id_set);
      }
      else
	atom_val = 0;
#endif /* ESSAI */

      *top_value_stack = atom_val;

#if LOG
      printf("ranking_by_special_atomic_field on F%i, field \"%s\" seeks \"%s\"\n", fs_id, field_id2string [special_atomic_field_id], atom_val == 0 ? "<no_atom>" : ATOM_ID2STRING (atom_val));
#endif /* LOG */

      if (min_atom_val == 0 || atom_val < min_atom_val)
	min_atom_val = atom_val;
    }
    else
      *top_value_stack = -1;

    top_value_stack--;
  } while (top_heads_stack > working_heads_stack);

#if LOG
  if (min_atom_val)
      printf("... The winner is \"%s\"\n", ATOM_ID2STRING (min_atom_val));
#endif /* LOG */

  working_heads_stack [0] = 0;

  bot_heads_stack = working_heads_stack;
  top_heads_stack = working_heads_stack+head_nb;
  bot_value_stack = working_value_stack;

  while (++bot_heads_stack <= top_heads_stack) {
    atom_val = *++bot_value_stack;
    head = *bot_heads_stack;

    if (atom_val == min_atom_val) {
      DSPUSH (working_heads_stack, head);
    }
  }

  return working_heads_stack [0] < head_nb;
}



/* On regarde si le nom du nt en lhs de Pij commence par Easy_A */
int
seek_prefix_Aij (Easy_A, Pij)
     char *Easy_A;
     int  Pij;
{
  int  lgth, Aij;
  char *str;

  lgth = strlen (Easy_A);
  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= lgth && strncmp (str, Easy_A, lgth) == 0) {
    return Aij;
  }
  
  return 0;
}
/* On regarde si le nom du nt en lhs de Pij est A */
int
seek_Aij (A, Pij)
     char *A;
     int  Pij;
{
  int  lgth, Aij;
  char *str;

  lgth = strlen (A);
  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) == lgth && strncmp (str, A, lgth) == 0) {
    return Aij;
  }
  
  return 0;
}

/* Calcul du poids (nb de couples (attr présent et non égal à "-", terminal) de fs_id (et de ses sous structures) */
static int
get_att_cat (fs_id, local_att_id, local_cat_id)
     int fs_id, local_att_id, local_cat_id;
{
  int     x, weight, bot, top, val, field_id, bot2, cur2, top2, cat_val, att_val, atom_id, local_atom_id, val_orig;
  int     *local2atom_id;
  char    *str;
  unsigned char   static_field_kind;

  if (fs_id == 0)
    return 0;

  if (!SXBA_bit_is_reset_set (fs_id_set, fs_id))
    /* cyclique */
    return 0; /* element neutre */

  /* Pour l'instant, on ne memoize pas */
  weight = 0;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  cat_val = att_val = 0;

  while (bot < top) {
    val_orig = val = XH_list_elem (XH_fs_hd, bot);
    
    if ((val & OPTIONAL_VAL) == 0) {
      field_id = val & FIELD_AND;
      
      if (is_executable (field_id)) {
	/* Ce champ est calcule' ds cette passe */
	val >>= FS_ID_SHIFT;
	
	if (field_id == pred_id) {
	  /* On va prendre tous les arguments, meme si non presents ds pred */
	}
	else {
#ifdef EASY
	  if (field_id == cat_id) {
	    if (val > 0) {
#ifndef ESSAI
	      val = get_fs_id_atom_val (val_orig);
	      local2atom_id = field_id2atom_field_id [field_id];
	      local_atom_id = 0;
	      
	      while ((val >>= 1) != 0) {
		local_atom_id++;
		
		if (val & 1) {
		  atom_id = local2atom_id [local_atom_id];
		  
		  if (atom_id == local_cat_id) {
		    cat_val = 1;
		    break;
		  }
		}
	      }
#else /* ESSAI */
	      {
		SXBOOLEAN is_shared;
		
		if (local_atom_id = get_ufs_atom_val (val_orig >> FS_ID_SHIFT, &is_shared, left_local_atom_id_set)) {
		  do {
		    atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id); /* conversion local -> global */
		    
		    if (atom_id == local_cat_id) {
		      cat_val = 1;
		      break;
		    }
		  } while ((local_atom_id = sxba_scan (left_local_atom_id_set, local_atom_id)) > 0);
		  
		  sxba_empty (left_local_atom_id_set);
		}
	      }
#endif /* ESSAI */
	    }
	  }
	  else
#endif /* EASY */
	    {
#ifdef EASY
	      if (field_id == local_att_id) {
#ifndef ESSAI
		att_val = get_fs_id_atom_val (val_orig);
#else /* ESSAI */
		att_val = val_orig;
#endif /* ESSAI */
	      }
	      else 
#endif /* EASY */
		{
		  if (val > 0) {
		    static_field_kind = field_id2kind [field_id];
		    
		    if (static_field_kind == STRUCT_KIND) {
		      if (SXBA_bit_is_set (argument_set, field_id)) {
			/* C'est un argument */ 
			weight += get_att_cat (val,local_att_id,local_cat_id);
		      }
		    }
		    else {
		      if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
			/* On suppose que les "set of struct" ne sont pas des arguments !! */
			for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
			     cur2 < top2;
			     cur2++) {
			  val = XH_list_elem (XH_ufs_hd, cur2);
			  
			  if (val < 0)
			    break;
			  
			  weight += get_att_cat (val,local_att_id,local_cat_id);
			}
		      }
		    }
		  }
		}
	    }
	}
      }
    }
      
    bot++;
  }

#ifdef EASY
  if (cat_val) {
    if (att_val) {
#ifndef ESSAI
      local2atom_id = field_id2atom_field_id [det_id];
      local_atom_id = 0;

      while ((att_val >>= 1) != 0) {
	local_atom_id++;

	if (att_val & 1) {
	  atom_id = local2atom_id [local_atom_id];

	  if (atom_id != atom_minus) {
	    weight++;
	    break;
	  }
	}
      }
#else /* ESSAI */
      {
	SXBOOLEAN is_shared;

	if (local_atom_id = get_ufs_atom_val (att_val >> FS_ID_SHIFT, &is_shared, left_local_atom_id_set)) {
	  do {
	    atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id); /* conversion local -> global */

	    if (atom_id != atom_minus) {
	      weight++;
	      break;
	    }
	  } while ((local_atom_id = sxba_scan (left_local_atom_id_set, local_atom_id)) > 0);

	  sxba_empty (left_local_atom_id_set);
	}
      }
#endif /* ESSAI */
    }
  }
#endif /* EASY */

  SXBA_0_bit (fs_id_set, fs_id);

  return weight;
}

/* -> easy.disamb
get_adv_weight
seek_Easy_NVij_weight
get_max_aij_weight
get_max_Easy_NV_weight
get_det_nc (génériquisé en get_att_cat ci-dessus, et défini dans easy.disamb pour get_det_nc)
get_nc_adjunct_nc
*/

/* call_inside_ranking est maintenant généré */
#define include_ranking_functions 1
#ifdef lfg_disamb_h
#include lfg_disamb_h
#endif /* lfg_disamb_h */
#undef include_ranking_functions


/* regarde si head est ds head_stack */
static SXBOOLEAN
head_search (head_stack, head)
     int *head_stack, head;
{
  int i;

  for (i = head_stack [0]; i > 0; i--) {
    if (head_stack [i] == head)
      return SXTRUE;
  }

  return SXFALSE;
}

/* met en conformite' les Pij2disp [Aij-prod] */
static int
filter_Pij_heads (int *head_stack, int Aij, SXBOOLEAN called_from_perform_inside_ranking)
{
  int     hook, Pij, nb, rule, head, x, valid_Pij_nb = 0;
  int     *old_p, *new_p;
  SXBOOLEAN is_empty;

  hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;

  while ((Pij = spf.outputG.rhs [hook++].lispro) != 0) {
    if (Pij > 0 && (x = Pij2disp [Pij]) > 0) {
      new_p = old_p = fs_id_Pij_dstack+x;
      is_empty = SXTRUE;
	
      for (rule = PROD2RULE (spf.outputG.lhs [Pij].init_prod); rule != 0; rule = rule2next_rule [rule]) {
	nb = *old_p++;
	*new_p = 0;

	while (nb-- > 0) {
	  head = *old_p++;

	  if (head_search (head_stack, head)) {
	    PUSH (new_p, head);
	    PUSH (new_p, *old_p);
	    is_empty = SXFALSE;
	  }

	  old_p++; /* doldol */
	  nb--;
	}

	new_p += *new_p+1;
      }

      if (is_empty) {
	Pij2disp [Pij] = 0;

	if (called_from_perform_inside_ranking)
	  /* ESSAI */
	  SXBA_1_bit (erased_by_ranking_Pij_set, Pij);

#if LOG
	spf_print_iprod (stdout, Pij, "", "" /* without nl */);
	fputs (" ... is filtered out\n", stdout);
#endif /* LOG */
      }
      else
	valid_Pij_nb++;
    }
  }

  return valid_Pij_nb;
}


/* On filtre Xpq2disp par coherence locale et ranking */
/* Positionne is_consistent */
static void
perform_inside_ranking (Aij)
     int Aij;
{
  int     A, x, d, i, old_d, new_d, head, fs_id, hook, Pij;
  int     *base_ptr, *init_base_ptr;
  char    *ranking;
  SXBOOLEAN has_only_neg, has_only_pos, use_default_ranking;
#if EBUG
  SXBOOLEAN test_bool;
#endif /* EBUG */
  
  /* Le 20/09/05 */
  /* Si un Aij a des f_structures, on note (pour suppression ulterieure) toutes les Aij-prod qui
     n'ont pas de f_structures (y'a eu echec de l'unification).
     On va donc supposer que ds les utilisations ulterieures, cette unif continuera a echouer et qu'il est
     donc inutile de la recommencer.
  */
  /* La remarque precedente est fausse car ds le cas is_relaxed_run de nouvelles f_structures (incoherentes ou pseudo-coherentes)
     peuvent avoir ete creees et donner une unif valide */
  /* On note ce Pij uniquement pour le $-nt */
  A = Aij2A (Aij);
  ranking = (is_ranking) ? RANKS (A) : NULL; /* L'option "-no_ranking" peut inhiber le ranking */
  x = Xpq2disp [Aij];
      
  use_default_ranking = SXFALSE; /* par défaut, pas de default_ranking */

  if (x) {
    base_ptr = fs_id_dstack + x;
    d = *base_ptr++;

    if (ranking == NULL && max_fs_nb > 0 && d > max_fs_nb) {
      ranking = ranks [0]; /* ranking de __default__ appelé qd il y trop de fs, sur option -mfsnb max_fs_nb */
      if (ranking) {
	use_default_ranking = SXTRUE;
#if LOG
	printf ("Too many f-structures (%i > %i) : applying default ranking\n", d, max_fs_nb);
#endif /* LOG */
      }
    }

    if (ranking && *ranking == '$') {
      hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;

#if EBUG
      test_bool = SXFALSE;
#endif /* EBUG */
      while ((Pij = spf.outputG.rhs [hook++].lispro) != 0) {
	if (Pij > 0)
	  if (Pij2disp [Pij] == 0)
	    SXBA_1_bit (to_be_erased_Pij_set, Pij);
#if EBUG	  
	  else
	    test_bool = SXTRUE;
#endif /* EBUG */
      }
#if EBUG
      if (!test_bool)
	sxtrap (ME, "perform_inside_ranking (no subtrees kept, whereas only those with inconsistent f-structures should have been removed, and other subtrees are known to have consistent f-structures");
#endif /* EBUG */
    }
    
    if (d > 1) {
      /* ... et f_structures multiples */
      /* On commence par filtrer les incoherences locales */
      old_d = d;
      init_base_ptr = base_ptr;
      has_only_pos = SXTRUE;
      has_only_neg = SXTRUE;

      while (d--) {
	head = *base_ptr++;

	if (head > 0)
	  has_only_neg = SXFALSE;
	else
	  has_only_pos = SXFALSE;
      }

      is_consistent = !has_only_neg;

      if (ranking) {
	if (*ranking == '$')
	  /* Coherence totale */
	  ranking++;

	if (*ranking == SXNUL)
	  ranking = NULL;
      }

      if (!(IS_COMPLETE(A) || use_default_ranking) || ranking == NULL && (has_only_pos || has_only_neg)) {
#if LOG
	  fputs ("No Ranking (", stdout);
	  spf_print_Xpq (stdout, Aij);
	  fputs (")", stdout);
	  printf (" |f_struct| = %i\n", fs_id_dstack [x]);
#endif /* LOG */

	  return;
      }

      if (has_only_pos || has_only_neg) {
	/* non mixte et ranking != NULL */
      }
      else {
	/* mixte */
	/* On ne conserve que les positifs */
	fs_id_dstack [0] -= old_d;
	fs_id_dstack [x] = 0;

	d = old_d;
	old_d = 0;
	base_ptr = init_base_ptr;

	while (d--) {
	  head = *base_ptr++;

	  if (head > 0) {
	    DPUSH (fs_id_dstack, head);
	    fs_id_dstack [x]++;
	    old_d++;
	  }
	}

	/* Il faut faire la meme chose sur les Pij */
	/* Le 23/08/05 */
	filter_Pij_heads (fs_id_dstack + x, Aij, SXTRUE);

	if (ranking == NULL)
	  /* pas de ranking */
	  return;
      }

      d = old_d;
      base_ptr = init_base_ptr;
  
      /* Ici ranking demande' ... */
      if (working_heads_stack == NULL) {
	DALLOC_STACK (working_heads_stack, 100+d);
	DALLOC_STACK (working_value_stack, 100+d);
#if FS_PROBAS
	if (is_proba_ranking)
	  working_double_stack = (double*) sxalloc (DSIZE (working_heads_stack)+1, sizeof (double));
#endif /* FS_PROBAS */
      }
      else {
	int size;

	DTOP (working_heads_stack) = 0;
	size = DSIZE (working_heads_stack);
	DCHECK (working_heads_stack, d);

	if (size != DSIZE (working_heads_stack)) {
	  /* y'a eu realloc */
	  DCHECK (working_value_stack, d);
#if FS_PROBAS
	  if (is_proba_ranking)
	    working_double_stack = (double*) sxrealloc (working_double_stack, DSIZE (working_heads_stack)+1, sizeof (double));
#endif /* FS_PROBAS */
	}
      }

      while (d--) {
	head = *base_ptr++;

	if (head > 0)
	  DSPUSH (working_heads_stack, head);
	else {
	  if (has_only_neg)
	    /* Ils sont tous localement incoherent, on travaille quand meme avec */
	    DSPUSH (working_heads_stack, head);
	}
      }

      if (call_inside_ranking (Aij, ranking)) {
	fs_id_dstack [x] = new_d = working_heads_stack [0];
	fs_id_dstack [0] -= old_d;
	with_ranking = SXTRUE;
#if LOG
	if (use_default_ranking)
	  fputs ("Default ranking (", stdout);
	else
	  fputs ("Ranking (", stdout);
	print_symb (stdout, A, Aij2i (Aij), Aij2j (Aij));
	printf (", %s) |f_struct| = %i/%i validates ", varstr_tostr (vstr), new_d, old_d);
#endif /* LOG */

#if EBUG
	if (new_d == 0) {
	  sxtrap (ME, "perform_inside_ranking");
	}
#endif /* EBUG */

	for (i = 1; i <= new_d; i++) {
	  head = working_heads_stack [i];
	  DSPUSH (fs_id_dstack, head);

#if LOG
	  if (head < 0)
	    head = -head;

	  fs_id = XxY_Y (heads, head);
	  printf ("F%i", fs_id);

	  if (i < new_d)
	    fputs (", ", stdout);
#endif /* LOG */
	}

#if LOG
	fputs ("\n", stdout);
#endif /* LOG */

	/* Il faut faire la meme chose sur les Pij */
	/* Le 23/08/05 */
	filter_Pij_heads (fs_id_dstack + x, Aij, SXTRUE);
      }
#if LOG
      else {
	if (use_default_ranking)
	  fputs ("Inoperative default ranking (", stdout);
	else
	  fputs ("Inoperative ranking (", stdout);
	print_symb (stdout, A, Aij2i (Aij), Aij2j (Aij));
	printf (", %s) |f_struct| = %i\n", ranking, old_d);
      }
#endif /* LOG */
    }
    else
      is_consistent = *base_ptr >= 0;
  }
  else
    is_consistent = SXFALSE;
    
  return;
}

/* On est sur le sous-arbre de "racine" prod du tree_id ieme arbre de la foret partagee */
/* REMARQUE : il est possible que la meme production instanciee soit utilisee plusieurs fois ds le meme arbre.
   Exemple 14/97:<[ADVP][3..3]> = ; est utilisee 2 fois ds l'arbre
   21/12:<S[1..4]> = <[pres][1..1]> <_PREVERB[1..2]> <[ADVP][2..2]> <[clneg][2..2]> <_VERB[2..3]> <[ADVP][3..3]> <_POSTVERB[3..4]> ;
                                                                                                  **************
   22/47:<_POSTVERB[3..4]> = <[NP][3..3]> <AP[3..4]> <[NP][4..4]> ;
   23/66:<AP[3..4]> = <[ADVP][3..3]> "a"[3..4] <[PP][4..4]> ;
                      **************
 */
/* A priori ca ne peut se produire que sur des productions vides (dont on suppose la semantique vide) */


#if LOG
/* On va visiter toutes les Aij-prods par tree_bu_walk */
/* On prepare ces visites */
static int
tree_pre_bu_walk (Aij)
     int     Aij;
{
  if (is_relaxed_run /* 2 eme tentative ... */
      && cur_pass_number == consistent_cur_pass_number /* ... de la bonne passe ... */
      ) {
    if (SXBA_bit_is_set (consistent_Xpq_set, Aij) /* ... sur une lhs coherente selectionnee ... */) {
      /* On est sur une Aij-prod telle que Aij a ete selectionnee et telle que les f_structures (coherentes) associees a Aij (qui ont ete calculees
	 ds une phase precedente) sont reutilisees telles quelles (et non recalculees).  

	 Attention l'Aij-prod courante a pu ne pas fournir
	 de f_structures, on ne la recalcule pas (bien que maintenant elle pourrait fournir des f_structures incoherentes).
	 Notons qu'il faut recalculer Les Xpq de sa partie droite, ce qui a ete fait (ils ont peut etre des f_structures incoherentes),
	 car ces Xpq peuvent etre partages et les valeurs calculees reellement utilisees. */
      /* Le 15/09/05 ces Aij-prod ont ete enlevees de la foret (voir le return -1 ds relaxed_dag_td_walk */

      /* On conserve les f_structures calculees */
      fputs ("\nRelaxed run: The following f_structures associated to ", sxstdout);
      print_symb (sxstdout, Aij2A (Aij), Aij2i (Aij), Aij2j (Aij));
      fputs (" are reused\n", sxstdout);
    }
  }

  return 1;
}
#endif /* LOG */


/* On vient juste de visiter toutes les Aij-prods par tree_bu_walk */
/* On finalise ces visites */
static int
tree_post_bu_walk (Aij, status)
     int     Aij;
     SXBOOLEAN status;
{
  int        d, fs_id, head;
  int        *bot_ptr;
  SXBOOLEAN    is_unc;

#if EBUG
  if (!status)
    /* Toutes les Aij-prods ont ete virees !! */
    sxtrap (ME, "tree_post_bu_walk");
#endif /* EBUG */
	  
  /* pour le RAZ des structures de travail qui permettent la detection des f-structures cycliques equivalentes */
  is_new_Pij_for_cyclic_fs_ids = SXTRUE /* Ajoute' le 31/07/06 pour les structures cycliques */;

  if (is_relaxed_run /* 2 eme tentative ... */
      && cur_pass_number == consistent_cur_pass_number /* ... de la bonne passe ... */
      ) {
#if LOG
    if (SXBA_bit_is_set (consistent_Xpq_set, Aij)) {
      /* fin du Aij */
      bot_ptr = fs_id_dstack + Xpq2disp [Aij];
      d = *bot_ptr++;

      while (d--) {
	head = *bot_ptr++;

	if (head < 0) {
	  is_unc = SXTRUE;
	  head = -head;
	}
	else
	  is_unc = SXFALSE;

#if EBUG
	if (XxY_X (heads, head) != Aij)
	  sxtrap (ME, "tree_post_bu_walk");
#endif /* EBUG */

	fs_id = XxY_Y (heads, head);
      
	print_f_structure (Aij, fs_id, is_unc, SXTRUE);

	if (d)
	  fputs ("\n", sxstdout);
      }

      fputs ("Relaxed run end\n", stdout);
    }
#endif /* LOG */

    if (SXBA_bit_is_set (consistent_Xpq_set, Aij) /* ... sur une lhs coherente selectionnee ... */) {
      /* On est sur une Aij-prod telle que Aij a ete selectionnee et telle que les f_structures (coherentes) associees a Aij (qui ont ete calculees
	 ds une phase precedente) sont reutilisees telles quelles (et non recalculees).  

	 Attention l'Aij-prod courante a pu ne pas fournir
	 de f_structures, on ne la recalcule pas (bien que maintenant elle pourrait fournir des f_structures incoherentes).
	 Notons qu'il faut recalculer Les Xpq de sa partie droite, ce qui a ete fait (ils ont peut etre des f_structures incoherentes),
	 car ces Xpq peuvent etre partages et les valeurs calculees reellement utilisees. */
      /* Le 15/09/05 ces Aij-prod ont ete enlevees de la foret (voir le return -1 ds relaxed_dag_td_walk */

      /* On conserve les f_structures calculees */
      SXBA_1_bit (Xpq_set, Aij);

      /* ... leur ranking a deja ete fait */
      return 1;
    }
  }

  /* Toutes Les f_structures associees a Aij ont ete calculee ... */
  /* ... et prev_A est un non-terminal sur lequel la verif de coherence et de completude a ete faite */
  /* On lance aussi du ranking */
  perform_inside_ranking (Aij);

  return 1;
}


static SXBOOLEAN
has_empty_rhs (Pij)
     int Pij;
{
  return (spf.outputG.lhs [Pij+1].prolon-spf.outputG.lhs [Pij].prolon) == 1;
}


static void
fs_id_set_clear ()
{
  int x;

  while (!IS_EMPTY (fs_id_stack)) {
    x = POP (fs_id_stack);
    SXBA_0_bit (fs_id_set, x);
  }
}

/* Le 23/06/06 changement de la signature, c'est une liste des groupes de champs precedes pour
   chaque groupe du nombre de champs qu'il contient */
/* Le push se fait par une fonction de l'utilisateur.  On lui passe de niveau d'imbrication et il
   a la possibilite' d'interrompre les appels en retournant SXFALSE */
SXBOOLEAN
set_fs_id_signature (int fs_id, int level, SXBOOLEAN (*f)())
{
  int           bot, cur, top, val, field_id, prev_field_id, cur2, bot2, top2, atom_val;
  unsigned char static_field_kind;
  SXBOOLEAN       ret_val = SXTRUE;

  if (fs_id > 0) {
    if (!SXBA_bit_is_reset_set (fs_id_set, fs_id))
      /* On sort au plus une fois un fs_id */
      return SXTRUE;
		  
    PUSH (fs_id_stack, fs_id); /* pour le RAZ */

    ret_val = (*f) (0 /* validation de level */, level);

    if (ret_val) {
      bot = XH_X (XH_fs_hd, fs_id);
      top = XH_X (XH_fs_hd, fs_id+1);

      if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
	/* structure nommee ou cyclique */
	top--;

      prev_field_id = 0;
      cur = bot;

      while (cur < top) {
	val = XH_list_elem (XH_fs_hd, cur);
	field_id = val & FIELD_AND;
	static_field_kind = field_id2kind [field_id];

	/* Plusieurs possibilites :
	   - on ne considere que les 'arguments"
	   - ou on considere tous les champs struct
	*/

	/* Essai avec les champs struct */
	if (static_field_kind & STRUCT_KIND /* SXBA_bit_is_set (argument_set, field_id) */) {
	  if (prev_field_id)
	    (*f) (prev_field_id, 0 /* f a "accepte'" le niveau courant */);

	  prev_field_id = field_id;
	}

	cur++;
      }

      /* on appelle f meme si prev_field_id==0. Ca permet de differencier
	 [a=[vide], b=[c=[...]]] de [a=[c=[...]], b=[vide]] */
      (*f) (-prev_field_id /* fin d'une sequence */, 0 /* f a "accepte'" le niveau courant */);

      if (prev_field_id) {
	cur = bot;

	while (cur < top) {
	  val = XH_list_elem (XH_fs_hd, cur);
	  field_id = val & FIELD_AND;
	  static_field_kind = field_id2kind [field_id];

	  /* Plusieurs possibilites :
	     - on ne considere que les 'arguments"
	     - ou on considere tous les champs struct
	  */

	  /* Essai avec les champs struct */
	  if (static_field_kind & STRUCT_KIND /* SXBA_bit_is_set (argument_set, field_id) */) {
	    val >>= FS_ID_SHIFT;

	    if (static_field_kind & UNBOUNDED_KIND) {
	      if (val) {
		for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		     cur2 < top2;
		     cur2++) {
		  atom_val = XH_list_elem (XH_ufs_hd, cur2);

		  if (!set_fs_id_signature (atom_val, level+1, f))
		    /* level+1 est trop profond */
		    break;
		}

		if (cur2 < top2)
		  break;
	      }
	    }
	    else {
	      if (!set_fs_id_signature (val, level+1, f))
		/* level+1 est trop profond */
		break;
	    }
	  }

	  cur++;
	}
      }

      ret_val = SXTRUE;
    }

    if (level == 1)
      /* On quitte la racine => nettoyage */
      fs_id_set_clear ();
  }

  return ret_val;
}

#if FS_PROBAS
/* Utilise' par le ranking 'p'.
   A chaque f_structure est associe' une signature
   Si la LFG a defini X comme etant un nt sur lequel on peut faire du ranking 'p'
   Une campagne d'apprentissage automatique a permis d'associer a chaque couple (X_eprod, signature) une proba.
   X_eprod est une X_prod dans laquelle on a tenu compte des nt de sa RHS qui derivent ds le vide
   
   On filtre pour ne conserver que les head de proba max */

extern int max_signature_depth; /* ds ..._logprob.c */

static SXBOOLEAN
push_in_fs_signatures_hd (int elem, int level)
{
  if (elem == 0 /* appel pour verification */ && max_signature_depth > 0 /* == 0 => profondeur non bornee */ && level > max_signature_depth)
    return SXFALSE;
	
  if (elem != 0)
    XH_push (fs_signatures_hd, elem);

  return SXTRUE;
}


/* On calcule la proba de fs_id ds le contexte Ppq  */
static double
get_ctxt_weight (Ppq, fs_id)
     int Ppq, fs_id;
{
  int        son_eprod, ctxt, signature, id;
  double     weight;

  weight = min_logprob_fs /* proba min pour les f_structures */ - 1 /* pourquoi pas */;

  /* Pij2eprod a ete positionne' par l'appel de nbest_open () */
  if (son_eprod = Pij2eprod [Ppq]) {
    /* les signatures se recherchent ds fs_signatures_hd */
    /* calcul de la signature de fs_id */
    set_fs_id_signature (fs_id, 1, push_in_fs_signatures_hd);
      
    signature = XH_is_set (&fs_signatures_hd);
      
    if (signature && (id = XxY_is_set (&ctxtXsign_hd, son_eprod, signature))) {
      /* Cette signature ds ce ctxt a ete trouvee au cours de l'apprentissage */
      weight = ctxtXsign2logprobs [id];
    }
  }

  return weight;
}
#endif /* FS_PROBAS */

static int
tree_bu_walk (prod)
     int prod;
{
  int        Aij, prev_Aij, rule, equation_block, xcode, struct_nb, rhs_pos, pos, q, d, Xpq, init_prod, head, x, fs_id;
  int        mod, cur_bot, id_dol;
  int        *ptr, *base_ptr, *bot_ptr, *equation_block_base_ptr;
  SXBOOLEAN    ret_val;
  struct lhs *plhs;
  struct rhs *prhs, *p;

#if LOG
    /* On imprime cette production... */
    spf_print_iprod (stdout, prod, "", "\n" /* with nl */);
#endif /* LOG */

  /* Aij et prhs sont statiques... */ 
  Aij = spf.walk.cur_Aij;

  if (is_relaxed_run /* 2 eme tentative ... */
      && cur_pass_number == consistent_cur_pass_number /* ... de la bonne passe ... */
      && SXBA_bit_is_set (consistent_Xpq_set, Aij)
      ) {
    /* Les f_structures associees a prod ont deja ete evaluees precedemment, on n'y touche pas */
#if EBUG
    if (Pij2disp [prod] == 0)
      sxtrap (ME, "tree_bu_walk");
#endif /* EBUG */

    return 1;
  }

  if (Aij2i (Aij) == Aij2j (Aij) && (!spf.outputG.has_repair || Aij <= spf.outputG.maxnt || has_empty_rhs (prod))) {
    /* Cette instanciation de prod a une rhs qui est ou derive ds le vide */
    /* On suppose que la semantique est vide... */
    /* On ne cree la structure vide associee a Aij qu'une seule fois */
    if (!XxY_set (&heads, Aij, 0, &head)) {
      /* On definit le fs_id d'une structure vide comme etant 0 */
      /* nouveau, il a donc un nouveau numero */ 
      if ((x = Xpq2disp [Aij]) == 0) {
	/* 1ere apparition de Aij */
	DPUSH (fs_id_dstack, 0);
	x = Xpq2disp [Aij] = DTOP (fs_id_dstack); /* pointe sur le nb de fs differentes associees a Xpq */
	SXBA_1_bit (Xpq_set, Aij);
      }

      DPUSH (fs_id_dstack, head);
      fs_id_dstack [x]++; /* et un fs_id de plus ... */

      /* Le 11/05/05 pour le traitement des passes multiples, il ne faut pas que
	 les productions vides soient eliminees de la foret partagee */
      bot_ptr = Pij2disp+prod;

#if EBUG
      if (*bot_ptr != 0)
	sxtrap (ME, "tree_bu_walk");
#endif /* EBUG */

      cur_bot = DTOP (fs_id_Pij_dstack);

      DPUSH (fs_id_Pij_dstack, 2);
      DPUSH (fs_id_Pij_dstack, head);
      DPUSH (fs_id_Pij_dstack, -1); /* Cas particulier */

      *bot_ptr = cur_bot+1;
    }

#if LOG
    print_f_structure (Aij, 0, SXFALSE, SXTRUE);
#endif /* LOG */

    return 1;
  }

  plhs = spf.outputG.lhs+prod;
  init_prod = plhs->init_prod;
  prhs = spf.outputG.rhs+plhs->prolon-1;

  /* 1ere tentative :  Les disjonctions sont remontees sur les racines Aij */
  /* On calcule le nombre maximal de f_structures pour une rule */
  struct_nb = 1;
  p = prhs;
  rhs_pos = 0;

  while ((Xpq = (++p)->lispro) != 0) {
    rhs_pos++;

    if (Xpq < 0)
      /* terminal */
      Xpq = spf.outputG.maxxnt-Xpq;

    if ((x = Xpq2disp [Xpq]) == 0) {
      /* Echec ds ce sous-arbre */
      struct_nb = 0;
      break;
    }

    struct_nb *= fs_id_dstack [x];
  }

  if (struct_nb) {
    /* On repere les echecs d'unification */
    ret_val = SXFALSE;

    bot_ptr = Pij2disp+prod;

#if EBUG
    if (*bot_ptr != 0)
      sxtrap (ME, "tree_bu_walk");
#endif /* EBUG */

    cur_bot = DTOP (fs_id_Pij_dstack);

#if ATOM_Aij
    AIJ = Aij; /* Pour unify () */
#endif /* ATOM_Aij */
#if ATOM_Pij
    PIJ = prod; /* Pour unify () */
#endif /* ATOM_Pij */

    for (rule = PROD2RULE (init_prod); rule != 0; rule = rule2next_rule [rule]) {
#if LOG
      printf ("lfg_rule #%i:\n", rule);
#endif /* LOG */

      equation_block = rule2equation [rule];
      equation_block_base_ptr = equation_stack+equation_block;

      if (equation_block == 0 || *equation_block_base_ptr < 0) {
	/* pas de semantique associee a rule !! */
	/* On ne cree la structure vide associee a Aij qu'une seule fois */
	if (!XxY_set (&heads, Aij, 0, &head)) {
	  /* On definit le fs_id d'une structure vide comme etant 0 */
	  /* nouveau, il a donc un nouveau numero */ 
	  if ((x = Xpq2disp [Aij]) == 0) {
	    /* 1ere apparition de Aij */
	    DPUSH (fs_id_dstack, 0);
	    x = Xpq2disp [Aij] = DTOP (fs_id_dstack); /* pointe sur le nb de fs differentes associees a Xpq */
	    SXBA_1_bit (Xpq_set, Aij);
	  }

	  DPUSH (fs_id_dstack, head);
	  fs_id_dstack [x]++; /* et un fs_id de plus ... */

#if LOG
	  print_f_structure (Aij, 0, SXFALSE, SXTRUE);
#endif /* LOG */
	}

	/* On est dans le cas particulier où l'on n'a pas de sémantique (eq. fonctionnelle) associée à la règle.
	 on  met ret_val = SXTRUE:
	1. parce que dans ce cas il n'y a jamais d'échec à proprement paler
	2. pour que Xpq2disp[Aij] (qui contient la structure vide) soit cohérente avec Pij2disp[prod] */
	DPUSH (fs_id_Pij_dstack, 2);
	DPUSH (fs_id_Pij_dstack, head);
	DPUSH (fs_id_Pij_dstack, -1);
	ret_val = SXTRUE;
      }
      else {
	/* Pour chaque combinaison des f_structures de la rhs on calcule le $$ correspondant... */
	if (failure_set) {
	  sxba_empty (failure_set);
		    
	  if (BASIZE (failure_set) < struct_nb)
	    failure_set = sxba_resize (failure_set, struct_nb);
	}

	DPUSH (fs_id_Pij_dstack, 0);
	Pij_cur_bot = DTOP (fs_id_Pij_dstack);

	for (doldol = 0; doldol < struct_nb/* && (fs_nb_threshold == 0 || doldol < fs_nb_threshold)*/; doldol++) {
	  xcode = 0;

	  /* Pour chaque Xpq de la rhs, on calcule son numero d'occurrence */
	  if (failure_set == NULL || !SXBA_bit_is_set (failure_set, doldol)) {
	    /* A priori correct */
	    q = doldol;

	    for (pos = 1; pos <= rhs_pos; pos++) {
	      Xpq = prhs [pos].lispro;

	      if (Xpq < 0)
		/* terminal */
		Xpq = spf.outputG.maxxnt-Xpq;

	      base_ptr = fs_id_dstack + Xpq2disp [Xpq];
	      d = *base_ptr++;
	      mod = rhs_pos2digit [pos] = q % d;
	      head = base_ptr [mod];

	      if (head < 0)
		head = -head;

	      rhs_pos2fs [pos] = head;

#if EBUG
	      if (XxY_X (heads, head) != Xpq)
		sxtrap (ME, "tree_bu_walk");
#endif /* EBUG */

	      q /= d;
	    }

	    /* Tous les "head" de la RHS existent */
	    ptr = equation_block_base_ptr;

	    if ((xcode = *ptr++) > 0 && code_stack [xcode] == OPERATOR_IDENTITY) {
	      /* On est ds le cas particulier ou la seule equation du bloc est $$ = $1.
		 On se contente donc de recopier le fs_id de la RHS */
	      ret_val = SXTRUE;
	      fs_id = XxY_Y (heads, rhs_pos2fs [1]);
	      is_locally_unconsistent = SXFALSE; /* head doit etre > 0 ds ce cas */
	      head = fill_heads (Aij, fs_id);
	      SXBA_1_bit (Xpq_set, Aij);
#if LOG
	      print_f_structure (Aij, fs_id, SXFALSE, SXTRUE);
#endif /* LOG */

	      DPUSH (fs_id_Pij_dstack, head);
	      DPUSH (fs_id_Pij_dstack, doldol);
	      fs_id_Pij_dstack [Pij_cur_bot] += 2;
	    }
	    else {
	      wfs_id = 0;
	      get_a_new_wfs_id ();

#if EBUG
	      if (wfs_id != MAIN_WFS_ID)
		sxtrap (ME, "tree_bu_walk");
#endif /* EBUG */

	      adjunct_sets [0].id = 0; /* top */
	      field_val2adjunct_id [0].equiv = 0; /* top */
	      sxba_empty (doli_used_set);

	      /* Clear de re_results & Co*/
	      re_results [0].kind = 0;
	      int_tank_clear ();

	      XxY_clear (&field_paths);
	      XxY_clear (&XxY_dol_fs_id);

	      valid_eq_nb = 0;
	      has_an_re_equation = SXFALSE;
	      is_fill_fs_on_start_symbol = (Aij == spf.outputG.start_symbol);

	      do {
		if (xcode > 0) {
		  /* xcode == 0 => non produite !! */
		  /* xcode est index dans code_stack qui repere le sommet du code associe a l'equation courante */
		  /* typiquement on a code_stack [xcode] == OPERATOR_UNIFY */
		  if (!evaluate (xcode))
		    /* L'equation "xcode" vient d'echouer */
		    break;
		}
	      } while ((xcode = *ptr++) >= 0);

	      if (xcode < 0) {
		/* Ca a marche */
		/* On regarde s'il y a des post traitements */
		if (post_display [0]) {
		  if (head = call_post_evaluate (Aij)) {
		    ret_val = SXTRUE;
		  }
		}
		else {
		  /* la lhs $$ est calculee ds wfs */
		  if (fs_id = fill_fs (Aij)) {
		    ret_val = SXTRUE;
		    head = fill_heads (Aij, fs_id); /* head peut etre < 0 si incoherence "locale" */

		    SXBA_1_bit (Xpq_set, Aij);
#if LOG
		    print_f_structure (Aij, fs_id, is_locally_unconsistent, SXTRUE);
#endif /* LOG */

		    DPUSH (fs_id_Pij_dstack, head);
		    DPUSH (fs_id_Pij_dstack, doldol);
		    fs_id_Pij_dstack [Pij_cur_bot] += 2;
		  }
		  else {
		    /* Test de coherence a e'choue' */
#if LOG
		    print_wf_structures ();
#endif /* LOG */
		    DRAZ (post_display);
		    DRAZ (post_stack);
		    head = 0;
		  }
		}
	      }
	      else {
		/* Echec */
		/* Il faut razer wfs */
		head = 0;

		/* Est-il possible de noter les i des $i qui ont fait echouer pour ne pas recommencer des essais inutiles
		   Ex : $$ -> $1 $2 $3 $4 et on detecte un echec alors que les seules equations utilisees mettent en jeu
		   $1 et $2 et que rhs_pos2fs [1] = base_ptr [j] et rhs_pos2fs [2] = base_ptr [k] => on est
		   su^r que toute valeur de doldol de la forme (j, k, ?, ?) echouera, il est donc inutile de les essayer */
		/* doli_used_set contient l'ensemble des $i utilises qui ont conduit a l'echec */
		if (doldol+1 < struct_nb && sxba_0_rlscan (doli_used_set, rhs_pos+1) > 0) {
		  /* On n'est pas en fin ... */
		  /* Si tous les $i sont utilises, ca ne sert a rien de les noter!! */
		  if (failure_set == NULL) {
		    failure_set = sxba_calloc (struct_nb);
		  }

		  set_failure_set (prhs, rhs_pos, 0);
		}
	      }
	    }
	  }

	  /* raz de wfs_id2localdynweight */
	  for (x = MAIN_WFS_ID; x <= wfs_id; x++)
	    wfs_id2localdynweight[x] = 0; /* ré-initialisation pour la prochaine fois... */

#if LOG
	  printf ("  Occur #%i/%i: (%s)\n", doldol+1, struct_nb, (head != 0) ? "success" : "failure");
#endif /* LOG */
	}
      }
    }

    if (ret_val)
      /* Non vide => on note */
      *bot_ptr = cur_bot+1;
    else
      /* On remet fs_id_Pij_dstack en etat */
      fs_id_Pij_dstack [0] = cur_bot;
  }
  else {
    /* On elimine uniquement les prod dont l'unif a ete tentee et a echouee */
    ret_val = SXTRUE;

#if LOG
    printf ("At least one rhs symbol has zero associated f-structures: skipping prod (prod=#%i, lfg_rules=", init_prod);

    for (rule = PROD2RULE (init_prod); rule != 0; rule = rule2next_rule [rule]) {
      printf ("#%i ", rule);
    }

    fputs (")\n", stdout);
#endif /* LOG */
  }

  /* ATTENTION : il se peut qu'une unif a echoue a cause (de l'absence) d'une f_structure incoherente qui a ete eliminee !! */
  /* if (ret_val) => au moins une des f_structures associee a la LHS de prod est correcte */
  /* On conserve la prod ds la foret, meme s'il y a eu echec de l'unif car on pourra reutiliser des sous-structures */
  /* en bottom-up retourner 1 ou 0, c'est kif-kif */
  return 1;
}


#if LOG
static void
print_static_structure (root_call, struct_id, lexeme_id)
     SXBOOLEAN root_call;
     int     struct_id, lexeme_id;
{
  int             cur, bot, top, top_val, bot2, cur2, top2, bot3, cur3, top3;
  int             field_id, field_kind, sous_cat_id, atom_id, local_atom_id;
  int             *local2atom_id;
  SXBOOLEAN         is_optional, is_first, is_first2, is_first3;
  struct attr_val *ptr;
  struct pred_val *ptr2;
  unsigned char   static_field_kind;

  fputs ("[", stdout);

  is_first = SXTRUE;

  while (struct_id) {
    ptr = attr_vals+struct_id;
    struct_id = ptr->val;

    if (struct_id < 0)
      /* speciale */
      break;

    field_id = ptr->attr;
    static_field_kind = field_id2kind [field_id];
    field_kind = ptr->kind;

    if (is_first)
      is_first = SXFALSE;
    else
      fputs (", ", stdout);


    if (field_id == pred_id) {
      if (static_field_kind != ATOM_KIND)
	sxtrap (ME, "print_static_structure");

      ptr2 = pred_vals+struct_id;
      
      fputs ("pred =", stdout);

      if (field_kind & ASSIGN_CONSTRAINT_KIND)
	fputs ("c", stdout);
      else {
	if (field_kind & ASSIGN_OPTION_KIND)
	  fputs ("?", stdout);
      }
      
      if (lexeme_id) {
	if (ptr2->lexeme != 0)
	  sxtrap (ME, "print_static_structure");

	printf (" \"%s", lexeme_id2string [lexeme_id]);
      }
      else {
	printf (" \"%s", lexeme_id2string [ptr2->lexeme]);
      }

      is_first3 = SXTRUE;
      sous_cat_id = ptr2->sous_cat1;

      while (sous_cat_id) {
	fputs (" ", stdout);

	if (is_first3) {
	  fputs ("<", stdout);
	}
      
	for (bot2 = cur2 = sous_cat [sous_cat_id], top2 = sous_cat [sous_cat_id+1];
	     cur2 < top2;
	     cur2++) {
	  if (cur2 > bot2)
	    fputs (", ", stdout);

	  field_id = sous_cat_list [cur2];
	
	  if (field_id < 0) {
	    fputs ("(", stdout);
	    field_id = -field_id;
	    is_optional = SXTRUE;
	  }
	  else
	    is_optional = SXFALSE;

	  if (field_id > MAX_FIELD_ID) {
	    field_id -= MAX_FIELD_ID;

	    for (bot3 = cur3 = compound_field [field_id], top3 = compound_field [field_id+1];
		 cur3 < top3;
		 cur3++) {
	      field_id = compound_field_list [cur3];
	      printf ("%s%s", (cur3 != bot3) ? "|" : "", field_id2string [field_id]);
	    }
	  }
	  else {
	    printf ("%s", field_id2string [field_id]);
	  }

	  if (is_optional)
	    fputs (")", stdout);
	}

	if (is_first3) {
	  is_first3 = SXFALSE;
	  sous_cat_id = ptr2->sous_cat2;
	  fputs (">", stdout);
	}
	else
	  sous_cat_id = 0;
      }
	  
      fputs ("\"", stdout);
    }
    else {
      printf ("%s ", field_id2string [field_id]);

      if (field_kind & ASSIGN_CONSTRAINT_KIND)
	fputs ("=c ", stdout);
      else {
	if (field_kind & ASSIGN_OPTION_KIND)
	  fputs ("=? ", stdout);
	else
	  fputs ("= ", stdout);
      }

      if (static_field_kind & ATOM_KIND) {
	if (struct_id == 0)
	  /* On supporte 0 comme etant l'atome vide ... */
	  fputs ("()", stdout);
	else {
	  bot = lex_atom_hd [struct_id];
	  top = lex_atom_hd [struct_id+1];

	  if (bot == top) {
	    /* ... ou une liste vide */
	    fputs ("()", stdout);
	  }
	  else {
	    top_val = lex_atom_list [top-1];

	    if (top_val < 0) {
	      /* atome partage' (nomme') de la forme X80+X20+name_id */
	      fputs ("(", stdout);
	      top--;
	    }

	    if (bot < top) {
#ifndef ESSAI
	      struct_id = lex_atom_list [bot];

	      if (static_field_kind & UNBOUNDED_KIND)
		printf ("%s", ATOM_ID2STRING (struct_id));
	      else {
		local2atom_id = field_id2atom_field_id [field_id];
		local_atom_id = 0;
		is_first2 = SXTRUE;

		while ((struct_id >>= 1) != 0) {
		  local_atom_id++;

		  if (struct_id & 1) {
		    atom_id = local2atom_id [local_atom_id];

		    if (is_first2)
		      is_first2 = SXFALSE;
		    else
		      fputs ("|", stdout);

		    printf ("%s", ATOM_ID2STRING (atom_id));
		  }
		}
	      }
#else /* ESSAI */
	      atom_id = lex_atom_list [bot];
	      printf ("%s", ATOM_ID2STRING (atom_id));

	      while (++bot < top) {
		printf (" | %s", ATOM_ID2STRING (lex_atom_list [bot]));
	      }
#endif /* ESSAI */
	    }

	    if (top_val < 0) {
	      printf (")%i", top_val & X0F);
	    }
	  }
	}
      }
      else {
	if (static_field_kind == STRUCT_KIND) {
	  print_static_structure (SXFALSE, struct_id, 0);
	}
	else {
	  /* static_field_kind == STRUCT_KIND+UNBOUNDED_KIND */
	  if (struct_id == 0)
	    /* On supporte 0 comme etant l'adjunct vide ... */
	    fputs ("{}", stdout);
	  else {
	    bot = lex_adjunct_hd [struct_id];
	    top = lex_adjunct_hd [struct_id+1];

	    if (bot == top) {
	      /* ... ou une liste vide */
	      fputs ("{}", stdout);
	    }
	    else {
	      top_val = lex_adjunct_list [top-1];

	      if (top_val < 0) {
		/* atome partage' (nomme') de la forme X80+X20+X10+name_id */
		top--;

		if (top_val & X10)
		  fputs ("(unclosed)", stdout);
	      }

	      fputs ("{", stdout);

	      cur = bot;

	      while (cur < top) {
		if (cur != bot)
		  fputs (", ", stdout);
		  
		print_static_structure (SXFALSE, lex_adjunct_list [cur], 0);
		
		cur++;
	      }

	      fputs ("}", stdout);

	      if (top_val < 0 && (top_val & X20)) {
		printf ("%i", top_val & X0F);
	      }
	    }
	  }
	}
      }
    }
    
    struct_id = ptr->next;
  }
      
  fputs ("]", stdout);

  if (struct_id < 0) {
    if (struct_id & X40)
      /* cyclique */
      fputs (".c", stdout);

    if (struct_id & X20)
      /* nommee */
      printf (".%i", struct_id & X0F);

    if (struct_id & X10)
      /* non close */
      fputs (".nc", stdout);
  }

  if (root_call)
    fputs ("\n", stdout);
}
#endif /* LOG */


/* Le 06/11/06 les codes locaux des atomes sont calcules dynamiquement */
static void
atom_list2ufs (int bot, int top)
{
  int atom_id, local_atom_id;

  while (bot < top) {
    atom_id = lex_atom_list [bot];
    X_set (&atom_id2local_atom_id_hd, atom_id, &local_atom_id);
    SXBA_1_bit (left_local_atom_id_set, local_atom_id);
    bot++;
  }

  local_atom_id = 0;

  while ((local_atom_id = sxba_scan_reset (left_local_atom_id_set, local_atom_id)) > 0)
    XH_push (XH_ufs_hd, local_atom_id);
}


/* similaire a tfs_instantiation ds le cas des structures cycliques.  Il permet de "deviner"
   les fs_id des structures cycliques */
static int
simulate_cyclic_tfs_instantiation (struct_id, lexeme_id, priority_id, Tpq)
     int  struct_id, lexeme_id, priority_id, Tpq;
{
  int             dl_id, dl_id_atom, dl_id_adjunct, field_id, field_kind, field_val, fs_id, index, flag, bot, top, top_val, val, ufs_id, x;
  struct attr_val *ptr, *ptr2;
  struct pred_val *pv_ptr;
  unsigned char   static_field_kind;
  SXBOOLEAN         has_a_cyclic_ref, is_final_computing;
  
  has_a_cyclic_ref = SXFALSE;

  if (field_val = struct_id) {
    /* Structure non vide */
    /* Il faut 2 passes a` cause de la gestion des listes ds XH (elles se construisent l'une apres l'autre et non pas simultanement) */
    /* Le 1er passage utilise les [u]fs_id_ssstack pour stocker les resultats intermediaires */
    /* 1er passage */
    SS_open (fs_id_ssstack);

    do {
      ptr = attr_vals+field_val;
      field_val = ptr->val;

      if (field_val < 0) {
	/* Structure non standard */
#if EBUG
	if (ptr->next != 0)
	  /* Ce doit etre la derniere */
	  sxtrap (ME, "simulate_cyclic_tfs_instantiation");
#endif /* EBUG */

	break;
      }

      field_id = ptr->attr;
      static_field_kind = field_id2kind [field_id];
      field_kind = ptr->kind;
      if (field_id == pred_id) {
#if EBUG
	if (static_field_kind != ATOM_KIND)
	  sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

	/* On stocke les attributs des pred ds dynam_pred_vals et c'est l'index ds dynam_pred_vals
	   qui est stocke ds attr_ptr. */
	/* On y met aussi la priority_id !! */
	if (lexeme_id == 0)
	  lexeme_id = pred_vals [field_val].lexeme;

	pv_ptr = pred_vals+field_val;
	XH_push (dynam_pred_vals, lexeme_id);
	XH_push (dynam_pred_vals, pv_ptr->sous_cat1);
	XH_push (dynam_pred_vals, pv_ptr->sous_cat2);
	XH_push (dynam_pred_vals, priority_id);
	XH_push (dynam_pred_vals, Tpq); /* On stocke la tete, ca peut etre tres utile */

	XH_set (&dynam_pred_vals, &field_val);

	XH_push (XH_ufs_hd, field_val);
	XH_set (&XH_ufs_hd, &field_val);
      }
      else {
	if (index = (field_kind >> STRUCTURE_NAME_shift)) {
	  /* structure nommee */
	  /* on lui fait correspondre la structure nommee non vide de meme indice, si elle existe */
	  field_val = index2struct_id [index];
	  /* ... on peut donc avoir field_val != ptr->val */
	  /* ... field_val est non nul meme si toutes les structures de nom index sont vides i.e. == []i */

#if EBUG
	  if (field_val <= 0)
	    sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

	  field_kind &= ASSIGN_CONSTRAINT_KIND; /* On enleve l'index */
	}

	if (static_field_kind & STRUCT_KIND) {
	  if (static_field_kind == STRUCT_KIND) {
	    if (!XxY_set (&dl_hd, field_val, 0, &dl_id)) {
	      /* Le quadruplet (field_val, 0, 0, 0) est nouveau */
	      /* on l'instancie */
	      /* Non encore calcule' pour les cas cycliques */
	      dl_id2fs_id [dl_id] = 0;
	      fs_id = dl_id2fs_id [dl_id] = simulate_cyclic_tfs_instantiation (field_val, 0, 0, 0); /* valeur "definitive" */
	    }
	    else {
	      if ((fs_id = dl_id2fs_id [dl_id]) == 0) {
		/* C'est la "1ere" des occurrences vides internes au cycle ... */
#if EBUG
		if (index == 0
		    || !SXBA_bit_is_set (cyclic_index_set, index))
		  sxtrap (ME, "simulate_cyclic_tfs_instantiation");
#endif /* EBUG */

		dl_id2fs_id [dl_id] = fs_id = -index2cyclic_fs_id [index]; /* valeur provisoire */
	      }
	      /* Sinon on utilise la valeur deja calculee */
	      /* C,a peut etre la "vraie" si on est hors du cycle ou la provisoire si on est retombe'
		 sur une occcurrence vide multiple a l'interieur du cycle */
	    }

	    if (fs_id < 0)
	      has_a_cyclic_ref = SXTRUE;

	    field_val = fs_id;
	  }
	  else {
	    /* adjunct le 27/04/05 */
	    /* field_val peut valoir 0 !! */
	    if (!XxY_set (&dl_hd, field_val, -1, &dl_id_adjunct)) {
	      /* ... nouveau */
	      dl_id2fs_id [dl_id_adjunct] = 0; /* ... aucun fils cyclique */
	      is_final_computing = SXTRUE;

	      /* ... on calcule ses f_structures et on positionne has_a_cyclic_ref */
	      if (field_val) {
		bot = lex_adjunct_hd [field_val];
		top = lex_adjunct_hd [field_val+1];

		if (bot < top) {
		  top_val = lex_adjunct_list [top-1];

		  if (top_val < 0)
		    top--;
		}
		else
		  top_val = 0;

		SS_open (ufs_id_ssstack);

		while (bot < top) {
		  val = field_val = lex_adjunct_list [bot];

		  do {
		    ptr2 = attr_vals+val;
		  } while (val = ptr2->next);

		  index = 0;

		  if (ptr2->val < 0) {
		    /* X80 + (X10 | X20+index) */
		    if (ptr2->val & X20) {
		      index = ptr2->val & X0F;
		      field_val = index2struct_id [index];
		      /* field_val designe la structure nommee de meme index non vide (si elle existe) */
		    }
		  }

		  if (!XxY_set (&dl_hd, field_val, 0, &dl_id)) {
		    /* La 1ere occur de la f_structure se trouve ds un adjunct (elle est vide ou non, mais field_val designe
		       la non vide associee, si elle existe) */
		    /* Le quadruplet (field_val, 0, 0, 0) est nouveau */
		    /* on l'instancie */
		    /* Non encore calcule' pour les cas cycliques */
		    dl_id2fs_id [dl_id] = 0;
		    fs_id = simulate_cyclic_tfs_instantiation (field_val, 0, 0, 0); /* valeur "definitive" */
		    dl_id2fs_id [dl_id] = fs_id;
		  }
		  else {
		    if ((fs_id = dl_id2fs_id [dl_id]) == 0) {
		      /* C'est la "1ere" des occurrences vides internes au cycle ... */
#if EBUG
		      if (index == 0
			  || !SXBA_bit_is_set (cyclic_index_set, index))
			sxtrap (ME, "simulate_cyclic_tfs_instantiation");
#endif /* EBUG */

		      dl_id2fs_id [dl_id] = fs_id = -index2cyclic_fs_id [index]; /* valeur provisoire */
		    }
		    /* Sinon on utilise la valeur deja calculee */
		    /* C,a peut etre la "vraie" si on est hors du cycle ou la provisoire si on est retombe'
		       sur une occcurrence vide multiple a l'interieur du cycle */
		  }

		  if (fs_id < 0) {
		    is_final_computing = SXFALSE; /* ... au moins un fils cyclique */
		    has_a_cyclic_ref = SXTRUE;
		  }

		  SS_push (ufs_id_ssstack, fs_id);
	      
		  bot++;
		}

		if (top_val < 0 && (top_val & X20)) {
		  top_val &= XF0;
		  SS_push (ufs_id_ssstack, top_val);
		}

		for (x = SS_bot (ufs_id_ssstack), top = SS_top (ufs_id_ssstack); x < top; x++) {
		  field_val = SS_get (ufs_id_ssstack, x);

		  if (is_final_computing)
		    XH_push (XH_ufs_hd, field_val);
		  else
		    XH_push (XH_cyclic_ufs_hd, field_val);
		}

		SS_close (ufs_id_ssstack);

		if (is_final_computing)
		  XH_set (&XH_ufs_hd, &field_val);
		else {
		  XH_set (&XH_cyclic_ufs_hd, &field_val);
		  field_val = -field_val;
		}
	      }

	      dl_id2fs_id [dl_id_adjunct] = field_val;
	    }
	    else {
	      /* Deja instancie' */
	      field_val = dl_id2fs_id [dl_id_adjunct];
	    }
	  }
	}
	else {
	  /* atome */
	  if (!XxY_set (&dl_hd, field_val, -2, &dl_id_atom)) {
	    if (field_val) {
	      bot = lex_atom_hd [field_val];
	      top = lex_atom_hd [field_val+1];

	      if (bot < top) {
		top_val = lex_atom_list [top-1];

		if (top_val < 0) {
		  top--;
		  top_val &= XF0;
		}
		else
		  top_val = 0;

#if EBUG
		/* Nelle version !! */
		/* pas de structure partagee non vide */
		if (bot < top && top_val < 0)
		  sxtrap (ME, "simulate_cyclic_tfs_instantiation");
#endif /* EBUG */

#ifndef ESSAI
		if (bot < top) {
		  field_val = lex_atom_list [bot];
		  XH_push (XH_ufs_hd, field_val);
		}
#else /* ESSAI */
		atom_list2ufs (bot, top);
#endif  /* ESSAI */

		if (top_val < 0)
		  XH_push (XH_ufs_hd, top_val);
	      }
	      
	      XH_set (&XH_ufs_hd, &field_val);
	    }

	    dl_id2fs_id [dl_id_atom] = field_val;
	  }
	  else
	    field_val = dl_id2fs_id [dl_id_atom];
	}
      }
      
      if (!(is_final_computing = (field_val >= 0))) {
	field_val = -field_val;
      }

      field_val <<= FS_ID_SHIFT;
      field_val += (field_kind << MAX_FIELD_ID_SHIFT) + field_id;

      if (!is_final_computing)
	field_val |= X80;

      SS_push (ufs_id_ssstack, field_val);
    } while (field_val = ptr->next);

    /* 2eme passage */
    /* On les extraits ds le meme ordre */
    for (x = SS_bot (ufs_id_ssstack), top = SS_top (ufs_id_ssstack); x < top; x++) {
      field_val = SS_get (ufs_id_ssstack, x);

      if (has_a_cyclic_ref)
	XH_push (XH_cyclic_fs_hd, field_val);
      else
	XH_push (XH_fs_hd, field_val);
    }
    
    SS_close (fs_id_ssstack);
      
    if (field_val < 0) {
      /* Structure non standard */
      if (field_val & X10)
	/* structure non close */
	flag = X80+X10;
      else
	flag = 0;

      if (field_val & X20) {
	/* structure nommee (vide ou non) */
	index = field_val & X0F;

	if (SXBA_bit_is_set (cyclic_index_set, index))
	  /* Si c'est une occurrence vide d'une cyclique avant la detection du cycle !! */
	  has_a_cyclic_ref = SXTRUE;

	flag |= X80+X20;
      }

      if (has_a_cyclic_ref)
	flag |= X80+X40;

      if (flag) {
	if (has_a_cyclic_ref) {
	  XH_push (XH_cyclic_fs_hd, flag);
	}
	else {
	  XH_push (XH_fs_hd, flag);
	}
      }
    }
  }
    
  if (has_a_cyclic_ref)
    XH_set (&XH_cyclic_fs_hd, &fs_id);
  else {
    if (!XH_set (&XH_fs_hd, &fs_id)) {
      /* nouveau */
      fs_id2localdynweight [fs_id] = fs_id2dynweight [fs_id] = 0;
    }
  }

  return has_a_cyclic_ref ? -fs_id : fs_id;
}

/* On "recopie" la structure lexicale (struct_id, lexeme_id, priority_id, Tpq) ds fs_id */
static int
tfs_instantiation (struct_id, lexeme_id, priority_id, Tpq)
     int  struct_id, lexeme_id, priority_id, Tpq;
{
  int             dl_id, dl_id_atom, dl_id_adjunct, field_id, field_kind, field_val, fs_id, index, flag, bot, top, top_val, val, x;
  struct attr_val *ptr, *ptr2;
  struct pred_val *pv_ptr;
  unsigned char   static_field_kind;
  
  if (field_val = struct_id) { /* bien "=", pas "==" */
    /* Structure non vide */
    /* Il faut 2 passes a` cause de la gestion des listes ds XH (elles se construisent l'une apres l'autre et non pas simultanement) */
    /* Le 1er passage utilise les [u]fs_id_ssstack pour stocker les resultats intermediaires */
    /* 1er passage */
    SS_open (fs_id_ssstack);

    do {
      ptr = attr_vals+field_val;
      field_val = ptr->val;

      if (field_val < 0) {
	/* Structure non standard */
#if EBUG
	if (ptr->next != 0)
	  /* Ce doit etre la derniere */
	  sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

	break;
      }

      field_id = ptr->attr;
      /* On ne tient pas compte de is_executable() pour instancier les terminaux.  Leurs instanciations serviront 
	 dans toutes les passes */
      static_field_kind = field_id2kind [field_id];
      field_kind = ptr->kind;

      if (field_id == pred_id) {
#if EBUG
	if (static_field_kind != ATOM_KIND)
	  sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

	/* On stocke les attributs des pred ds dynam_pred_vals et c'est l'index ds dynam_pred_vals
	   qui est stocke ds attr_ptr. */
	/* On y met aussi la priority_id !! */
	if (lexeme_id == 0)
	  lexeme_id = pred_vals [field_val].lexeme;

	pv_ptr = pred_vals+field_val;
	XH_push (dynam_pred_vals, lexeme_id);
	XH_push (dynam_pred_vals, pv_ptr->sous_cat1);
	XH_push (dynam_pred_vals, pv_ptr->sous_cat2);
	XH_push (dynam_pred_vals, priority_id);
	XH_push (dynam_pred_vals, Tpq); /* On stocke la tete, ca peut etre tres utile */

	XH_set (&dynam_pred_vals, &field_val);

	XH_push (XH_ufs_hd, field_val);
	XH_set (&XH_ufs_hd, &field_val);
      }
      else {
	if (index = (field_kind >> STRUCTURE_NAME_shift)) {
	  /* structure nommee */
	  /* on lui fait correspondre la structure nommee non vide de meme indice, si elle existe */
	  field_val = index2struct_id [index];
	  /* ... on peut donc avoir field_val != ptr->val */
	  /* ... field_val est non nul meme si toutes les structures de nom index sont vides i.e. == []i */

#if EBUG
	  if (field_val <= 0)
	    sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

	  field_kind &= ASSIGN_CONSTRAINT_KIND + ASSIGN_OPTION_KIND; /* On enleve l'index */
	}

	if (static_field_kind & STRUCT_KIND) {
	  if (static_field_kind == STRUCT_KIND) {
	    if (!XxY_set (&dl_hd, field_val, 0, &dl_id)) {
	      /* Le quadruplet (field_val, 0, 0, 0) est nouveau */
	      /* Il n'est pas cyclique */
	      /* on l'instancie */
#if EBUG
	      dl_id2fs_id [dl_id] = 0;
#endif /* EBUG */
	      fs_id = tfs_instantiation (field_val, 0, -1, 0);
	      dl_id2fs_id [dl_id] = fs_id;

#if EBUG
	      if (fs_id <= 0)
		sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */
	    }
	    else {
	      fs_id = dl_id2fs_id [dl_id];
#if EBUG
	      if (dl_id2fs_id [dl_id] == 0)
		sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

	      if (fs_id < 0) {
		/* descendants* cyclique */
		/* On positionne a la future valeur => reevaluation unique et les futures occurrences vont en profiter */
		dl_id2fs_id [dl_id] = cyclic_base_fs_id - fs_id;
		fs_id = tfs_instantiation (field_val, 0, -1, 0);
#if EBUG
		if (dl_id2fs_id [dl_id] != fs_id)
		  sxtrap (ME, "tfs_instantiation (bad prediction)");
#endif /* EBUG */
	      }
	    }

#if EBUG
	    /* On n'est pas ds le cas cyclique */
	    if (fs_id <= 0)
	      sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

	    field_val = fs_id;
	  }
	  else {
	    /* adjunct */
	    if (!XxY_set (&dl_hd, field_val, -1 /* adjunct */, &dl_id_adjunct) /* nouveau ... */
		|| (fs_id = dl_id2fs_id [dl_id_adjunct]) < 0 /* ... ou contient des structures cycliques */
		) {
	      /* on [re]evalue */
	      if (field_val) {
		bot = lex_adjunct_hd [field_val];
		top = lex_adjunct_hd [field_val+1];

		if (bot < top) {
		  top_val = lex_adjunct_list [top-1];

		  if (top_val < 0)
		    top--;
		}
		else
		  top_val = 0;

		SS_open (ufs_id_ssstack);

		while (bot < top) {
		  val = field_val = lex_adjunct_list [bot];

		  do {
		    ptr2 = attr_vals+val;
		  } while (val = ptr2->next);

		  index = 0;

		  if (ptr2->val < 0) {
		    /* X80 + (X10 | X20+index) */
		    if (ptr2->val & X20) {
		      index = ptr2->val & X0F;
		      field_val = index2struct_id [index];
		      /* field_val designe la structure nommee de meme index non vide (si elle existe) */
		    }
		  }

		  if (!XxY_set (&dl_hd, field_val, 0, &dl_id)) {
		    /* Le quadruplet (field_val, 0, 0, 0) est nouveau */
		    /* on l'instancie */
#if EBUG
		    dl_id2fs_id [dl_id] = 0;
#endif /* EBUG */
		    fs_id = tfs_instantiation (field_val, 0, -1, 0);
		    dl_id2fs_id [dl_id] = fs_id;

#if EBUG
		    if (dl_id2fs_id [dl_id] <= 0)
		      sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */
		  }
		  else {
		    fs_id = dl_id2fs_id [dl_id];
#if EBUG
		    if (dl_id2fs_id [dl_id] == 0)
		      sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

		    if (fs_id < 0) {
		      /* descendants* cyclique */
		      /* On positionne a la future valeur => reevaluation unique et les futures occurrences vont en profiter */
		      dl_id2fs_id [dl_id] = cyclic_base_fs_id - fs_id;
		      fs_id = tfs_instantiation (field_val, 0, -1, 0);
#if EBUG
		      if (dl_id2fs_id [dl_id] != fs_id)
			sxtrap (ME, "tfs_instantiation (bad prediction)");
#endif /* EBUG */
		    }
		  }

#if EBUG
		  /* On n'est pas ds le cas cyclique */
		  if (fs_id <= 0)
		    sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

		  /* On ne peut pas empiler directement ds XH_ufs_hd a cause des appels recursifs */
		  SS_push (ufs_id_ssstack, fs_id);

		  bot++;
		}

		for (x = SS_bot (ufs_id_ssstack), top = SS_top (ufs_id_ssstack); x < top; x++) {
		  XH_push (XH_ufs_hd, SS_get (ufs_id_ssstack, x));
		}
		
		SS_close (ufs_id_ssstack);

		if (top_val < 0 && (top_val & X20)) {
		  top_val &= XF0;
		  XH_push (XH_ufs_hd, top_val);
		}

		XH_set (&XH_ufs_hd, &field_val);
	      }

	      dl_id2fs_id [dl_id_adjunct] = field_val;
	    }
	    else
	      field_val = fs_id;
	  }
	}
	else {
	  /* atome */
	  if (!XxY_set (&dl_hd, field_val, -2, &dl_id_atom)) {
	    if (field_val) {
	      bot = lex_atom_hd [field_val];
	      top = lex_atom_hd [field_val+1];

	      if (bot < top) {
		top_val = lex_atom_list [top-1];

		if (top_val < 0) {
		  top--;
		  top_val &= XF0;
		}
		else
		  top_val = 0;

#if EBUG
		/* Nelle version !! */
		/* pas de structure partagee non vide */
		if (bot < top && top_val < 0)
		  sxtrap (ME, "simulate_cyclic_tfs_instantiation");
#endif /* EBUG */

#ifndef ESSAI
		if (bot < top) {
		  field_val = lex_atom_list [bot];
		  XH_push (XH_ufs_hd, field_val);
		}
#else /* ESSAI */
		atom_list2ufs (bot, top);
#endif  /* ESSAI */

		if (top_val < 0)
		  XH_push (XH_ufs_hd, top_val);
	      }
	      
	      XH_set (&XH_ufs_hd, &field_val);
	    }

	    dl_id2fs_id [dl_id_atom] = field_val;
	  }
	  else
	    field_val = dl_id2fs_id [dl_id_atom];
	}
      }
      
      /* Les resultats partiels sont ds field_val */
      field_val <<= FS_ID_SHIFT;
      field_val += (field_kind << MAX_FIELD_ID_SHIFT) + field_id;
      /* On ne peut pas empiler directement ds XH_ufs_hd a cause des appels recursifs */
      SS_push (fs_id_ssstack, field_val);
      /* XH_push (XH_fs_hd, field_val); */
    } while (field_val = ptr->next);

    /* 2eme passage */
    for (x = SS_bot (fs_id_ssstack), top = SS_top (fs_id_ssstack); x < top; x++) {
      XH_push (XH_fs_hd, SS_get (fs_id_ssstack, x));
    }
    
    SS_close (fs_id_ssstack);

    if (field_val < 0) {
      /* Structure non standard */
      if (field_val & X10)
	/* structure non close */
	flag = X80+X10;
      else
	flag = 0;

      if (index = (field_val & X0F)) {
	/* Structure nommee partagee (vide ou non) ... */
	flag |= X80;

	if (SXBA_bit_is_set (cyclic_index_set, index))
	  flag |= X40;
	else
	  flag |= X20;
      }

      if (flag)
	XH_push (XH_fs_hd, flag);
    }
  }

  if (!XH_set (&XH_fs_hd, &fs_id)) {
    /* nouveau */
    /* A voir */
    fs_id2dynweight [fs_id] = fs_id2localdynweight [fs_id] = priority_id2priority (priority_id);
  }

  return fs_id;
}

static int
process_named_structures (struct_id)
     int struct_id;
{
  int             bot, top, top_val, val, index = 0, field_nb = 0, field_val = struct_id;
  struct attr_val *ptr;
  unsigned char   static_field_kind;

  do {
    ptr = attr_vals+field_val;

    if ((val = ptr->val) < 0) {
      /* Le 16/03/05 changement :
	 X80 => non standard (et c'est la derniere de la liste)
	 X40 => non utilise (pourrait designer cyclique si fait a la construction)
	 X20 => nommee
	 X10 => non close */
      /* structure nommee */
      if (val & X20) {
	index = val & X0F;
	SXBA_1_bit (index_set, index);

	if (field_nb /* non vide */ || index2struct_id [index] == 0 /* 1ere fois */)
	  index2struct_id [index] = struct_id;
      }

      break;/* C'est le dernier */
    }

    static_field_kind = field_id2kind [ptr->attr];

    if (static_field_kind & STRUCT_KIND) {
      if (static_field_kind == STRUCT_KIND)
	process_named_structures (val);
      else {
	/* adjunct le 27/04/05 */
	if (val) {
	  bot = lex_adjunct_hd [val];
	  top = lex_adjunct_hd [val+1];

	  if (bot < top) {
	    top_val = lex_adjunct_list [top-1];

	    if (top_val < 0) {
	      /* On ne sait pas si une f_structure non nommee contient des structures nommees closes !! */
	      /* on regarde donc toute la liste */
	      top--;

	      if (top_val & X20) {
		index = top_val & X0F;
		SXBA_1_bit (index_set, index);

		if (bot < top /* non vide */ || index2struct_id [index] == 0 /* 1ere fois */)
		  index2struct_id [index] = val;
	      }
	    }
	  }

	  while (bot < top) {
	    process_named_structures (lex_adjunct_list [bot]);
	    bot++;
	  }
	}
      }
    }
    else {
      /* atome le 27/04/05 */
      if (val) {
	bot = lex_atom_hd [val];
	top = lex_atom_hd [val+1];

	if (bot < top) {
	  top_val = lex_atom_list [top-1];

	  if (top_val < 0) {
	    /* On ne sait pas si une f_structure non nommee contient des structures nommees closes !! */
	    /* on regarde donc toute la liste */
	    top--;

	    if (top_val & X20) {
	      index = top_val & X0F;
	      SXBA_1_bit (index_set, index);

	      if (bot < top /* non vide */ || index2struct_id [index] == 0 /* 1ere fois */)
		index2struct_id [index] = val;
	    }
	  }
	}
      }
    }
      
    field_nb++;
  } while (field_val = ptr->next);

  return index;
}


/* On regarde si ds les sous-structures de struct_id on trouve une structure nommee de nom index */
static SXBOOLEAN
is_named_tfs_cyclic (index, struct_id)
     int index, struct_id;
{
  int             bot, top, top_val;
  struct attr_val *ptr;
  unsigned char   static_field_kind;

  do {
    ptr = attr_vals+struct_id;
    static_field_kind = field_id2kind [ptr->attr];

    if (static_field_kind & STRUCT_KIND) {
      struct_id = ptr->val;

      if (static_field_kind == STRUCT_KIND) {
	if (index == (ptr->kind >> STRUCTURE_NAME_shift))
	  return SXTRUE;

	if (struct_id && is_named_tfs_cyclic (index, struct_id))
	  return SXTRUE;
      }
      else {
	/* adjunct le 27/04/05 */
	if (struct_id) {
	  bot = lex_adjunct_hd [struct_id];
	  top = lex_adjunct_hd [struct_id+1];

	  if (bot < top) {
	    top_val = lex_adjunct_list [top-1];

	    if (top_val < 0)
	      /* On ne sait pas si une f_structure non nommee contient des structures nommees closes !! */
	      /* on regarde donc toute la liste */
	      top--;
	  }

	  while (bot < top) {
	    if (is_named_tfs_cyclic (index, lex_adjunct_list [bot]))
	      return SXTRUE;
	      
	    bot++;
	  }
	}
      }
    }
  } while (struct_id = ptr->next);

  return SXFALSE;
}

/* On instancie la structure terminale statique reperee par x sur Tpq */
static int
call_tfs_instantiation (x, Tpq)
     int  x, Tpq;
{
  int                        struct_id, lexeme_id, priority_id, dl_id, fs_id, index, lex_id, main_index, dum_dl_id;
  int                        cyclic_fs_id, cyclic_struct_id;
  struct structure_args_list *ptr2;

  /* Essai */
  XxYxZ_clear (&prio_hd);
  XxY_clear (&dl_hd);

  ptr2 = structure_args_list+x;

  lexeme_id = ptr2->lexeme;
  struct_id = ptr2->struct_id & X3F; /* On enleve is_cyclic ou is_named eventuels */
  /* Si struct_id == 0, on a la definition d'une forme flechie sans structure comme : '.' poncts; */
  /* Les valeurs struct_id sont caracteristiques de leur contenu : si 2 structures sont differentes =>
     leurs struct_id sont differents. Si 2 structures sont identiques leurs struct_id sont egaux.
     Donc les structures [blabla]i et []i ont des struct_id differents, process_named_structures () conduira
     a leur donner des fs_id identiques.  Meme chose si ces structures sont cycliques */
  /* Si struct_id == 0 => structure vide []
     Si struct_id != 0 => soit v = attr_vals [struct_id]
        Si v.val est non vide, le champ v.attr est non vide.
           Si v.attr == []i, on a attr_vals [v.val].val == -i
           Si v.attr == [...]i, on a attr_vals [{attr_vals [v.val].next}+ (dernier next non nul)].val == -i
  */
  priority_id = ptr2->priority;

#if 0
  if (priority == 0)
    /* Le mot "neutre" a un poids de 100 */
    priority = 100;
#endif /* 0 */

#if LOG
  print_static_structure (SXTRUE, struct_id, lexeme_id);
#endif /* LOG */

  XxYxZ_set (&prio_hd, lexeme_id, priority_id, Tpq, &lex_id);

  if (XxY_set (&dl_hd, struct_id, lex_id, &dl_id)) {
    /* Le quadruplet (struct_id, lexeme_id, priority_id, Tpq) a deja ete instancie' */
    /* impossible dl_hd vient d'etre razee !! */
    sxtrap (ME, "call_tfs_instantiation");
  }
  
#if EBUG
  dl_id2fs_id [dl_id] = 0;
#endif /* EBUG */

  if (ptr2->struct_id & X40) {
    /* La structure principale struct_id a des structures partagees nommees */
    /* Les structures vides nommees doivent s'instancier vers la structure non vide de meme nom
       si elle existe */
    main_index = process_named_structures (struct_id);
    /* Si main_index est non nul, la structure principale est nommee (et donc cyclique) */
    /* index_set et index2struct_id [index] sont positionnes */
    /* index2struct_id [i] contient le struct_id de [blabla]i s'il y a un non vide, sinon le struct_id de []i */

    if (ptr2->struct_id & X80) {
      /* La structure principale struct_id contient des structures cycliques */
      /* cyclique => nommee */
      /* Les occurrences les + internes des structures cycliques (nommees) sont obligatoirement vides */
      /* si la structure nommee i est cyclique index2struct_id [i] contient le struct_id de [blabla]i (le non vide) */
      /* Si une structure est cyclique elle est nommee (i) et il y a une et une seule structure non vide ayant ce nom
	 dont le struct_id est index2struct_id [i].  Il y a egalement au moins une structure vide []i descendante
	 de la structure non vide [....]i */
      cyclic_base_fs_id = -1;
      index = 0;

      while ((index = sxba_scan (index_set, index)) > 0) {
	cyclic_struct_id = index2struct_id [index];

	if (cyclic_struct_id > 0 &&
	    attr_vals [cyclic_struct_id].val >= 0
	    && field_id2kind [attr_vals [cyclic_struct_id].attr] == STRUCT_KIND) {
	  /* structure nommee non vide, elle peut donc etre cyclique */
	  /* On regarde si elle est cyclique ... */
	  if (is_named_tfs_cyclic (index, cyclic_struct_id)) {
	    /* ... oui */
	    SXBA_1_bit (cyclic_index_set, index);
	    /* On leur donne des identifiants uniques en temps que cyclic_fs_id ... */
	    XH_push (XH_cyclic_fs_hd, -index);
	    XH_set (&XH_cyclic_fs_hd, &cyclic_fs_id);
	    index2cyclic_fs_id [index] = cyclic_fs_id;
	    cyclic_base_fs_id--; /* ... et on les [de]compte */
	  }
	}
      }

#if EBUG
      if (sxba_cardinal (cyclic_index_set) == 0 || XH_top (XH_cyclic_fs_hd) != -cyclic_base_fs_id)
	sxtrap (ME, "call_tfs_instantiation");
#endif /* EBUG */

      /* cyclic_index_set contient tous les indices des structures nommees cycliques */
      fs_id = simulate_cyclic_tfs_instantiation (struct_id, lexeme_id, priority_id, Tpq);
      dl_id2fs_id [dl_id] = fs_id;

      if (main_index) {
	/* Il faut positionner (technique) aussi le dl_id2fs_id de la structure (struct_id, 0) !! */
	if (XxY_set (&dl_hd, struct_id, 0, &dum_dl_id)) {
	  /* Le quadruplet (struct_id, 0, 0, 0) a deja ete instancie' */
	  /* impossible */
	  sxtrap (ME, "call_tfs_instantiation");
	}

	dl_id2fs_id [dum_dl_id] = dl_id2fs_id [dl_id];
      }

      cyclic_base_fs_id += XH_top (XH_fs_hd);
      /* Si une structure se trouve a l'indice x ds XH_cyclic_fs_hd, elle sera [re]cree a l'indice
	 x+cyclic_base_fs_id ds XH_fs_hd */
      fs_id = tfs_instantiation (struct_id, lexeme_id, priority_id, Tpq);

#if EBUG
      if (cyclic_base_fs_id-dl_id2fs_id [dl_id] != fs_id)
	sxtrap (ME, "call_tfs_instantiation (bad prediction)");
#endif /* EBUG */ 

      dl_id2fs_id [dl_id] = fs_id;
      sxba_empty (cyclic_index_set);
      XH_clear (&XH_cyclic_fs_hd);
      XH_clear (&XH_cyclic_ufs_hd);
    }
    else {
      fs_id = tfs_instantiation (struct_id, lexeme_id, priority_id, Tpq);
      dl_id2fs_id [dl_id] = fs_id;
    }

    index = 0;

    while ((index = sxba_scan_reset (index_set, index)) > 0)
      index2struct_id [index] = 0;
  }
  else {
    fs_id = tfs_instantiation (struct_id, lexeme_id, priority_id, Tpq);
    dl_id2fs_id [dl_id] = fs_id;
  }

  return dl_id2fs_id [dl_id];
}



/* On instancie le terminal Xpq de la forme flechie if_id */
static void
terminal_instantiation (Xpq, X, if_id/* , terminal_string */)
     int  Xpq, X, if_id;
     /* char *terminal_string; */
{
  int                        bot, top, bot2, top2, x;
  int                        lexeme_id, struct_id, head, fs_id;
  struct if_id2t_list        *ptr;
#if EBUG
  SXBOOLEAN                    done = SXFALSE;
#endif /* EBUG */

  for (bot = inflected_form2display [if_id], top = inflected_form2display [if_id+1];
       bot < top;
       bot++) {
    ptr = if_id2t_list+bot;

    if (ptr->t == TERM_CONVERT (X)) {
#if EBUG
      done = SXTRUE;
#endif /* EBUG */

      bot2 = ptr->list;
      top2 = if_id2t_list [bot+1].list;

      while (bot2 < top2) {
	fs_id = call_tfs_instantiation (bot2, Xpq);
	head = fill_heads (Xpq, fs_id);

#if LOG
	print_f_structure (Xpq, fs_id, SXFALSE, SXTRUE);
#endif /* LOG */

	bot2++;
      }
    }
  }

#if EBUG
  if (!done) {
    char string[256];
    int Tij;
    Tij = Xpq-spf.outputG.maxxnt;
    sprintf (string, 
	     "terminal_instantiation (aucun des homonymes de la forme flechie en [%i%s%i] n'a la categorie terminale %s !!)", 
	     mlstn2dag_state (Tij2i (Tij)), 
	     (spf.outputG.Tpq_rcvr_set && SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tij)) ? "::" : "..",
	     mlstn2dag_state (Tij2j (Tij)),
	     spf.inputG.tstring [X]);
    /* aucun des homonymes de la forme flechie if_id n'a la categorie terminale X !! */
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
      terminal_instantiation (spf.outputG.maxxnt+Tpq, T, if_id/* , terminal_string */);
    else {
      /* xt */
      fill_heads (spf.outputG.maxxnt+Tpq, 0);

#if LOG
      print_f_structure (spf.outputG.maxxnt+Tpq, 0, SXFALSE, SXTRUE);
#endif /* LOG */
    }
  }
}

#if 0
/* Nelle version qui utilisespf.outputG.Tij2tok_no [Tpq] */
static void
set_terminal_leaves ()
{
  int  Tpq, T, p, q, tok_no, if_id;
  char *terminal_string, *comment;

#if LOG
  fputs ("Terminal leaves\n", stdout);
#endif /* LOG */

  for (Tpq = 1; Tpq <= maxxt; Tpq++) {
    tok_no = spf.outputG.Tij2tok_no [Tpq];

    if (tok_no > 0) {
      /* La forme flechie correspondante a l'identifiant if_id */
      if_id = toks_buf [tok_no].lahead;
      T = -Tij2T (Tpq);
      /* On peut donc instancier les f-structures lexicales associees a (if_id, T) */
#if LOG
      p = Tij2i (Tpq);
      q = Tij2j (Tpq);
      p = mlstn2dag_state (p);
      q = mlstn2dag_state (q);
      terminal_string = sxstrget (toks_buf [tok_no].string_table_entry);
      comment = toks_buf [tok_no].comment;
      printf ("<%i, %s%s/%s, %i>(if_id=%i, Tpq=%i)\n",
	      p, (comment == NULL) ? "" : comment, terminal_string, spf.inputG.tstring [T], q, if_id, Tpq);
#endif /* LOG */
      terminal_instantiation (spf.outputG.maxxnt+Tpq, T, if_id/* , terminal_string */);
    }
    else {
      /* xt */
      fill_heads (spf.outputG.maxxnt+Tpq, 0);

#if LOG
      print_f_structure (spf.outputG.maxxnt+Tpq, 0, SXFALSE, SXTRUE);
#endif /* LOG */
    }
  }
}
#endif /* 0 */

/* ... et les non terminaux en erreur */
static void
set_non_terminal_error ()
{
  int Xpq;

  for (Xpq = spf.outputG.maxnt+1; Xpq <= spf.outputG.maxxnt; Xpq++) {
    if (SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Xpq)) {
      fill_heads (Xpq, 0);

#if LOG
      print_f_structure (Xpq, 0, SXFALSE, SXTRUE);
#endif /* LOG */
    }
  }

  /* Pour partial_clear_for_relaxed_run () */
  static_heads_top = XxY_top (heads);
}


static int
tree_sem_pass ()
{
  double            double_tree_count;
  SXUINT tree_count;
  int               x, leaves_top, nb, unconsistent_nb, f_struct_nb;
  SXBOOLEAN           is_OK;

  is_OK = spf_tree_count (&double_tree_count);

  tree_count = (SXUINT) double_tree_count;
    
  maxxt = -spf.inputG.maxt;

#if 0
  /* fait ds earley le 28/09/05 */
  /* On complete la structure "Tij" avec les terminaux couverts par les nt de la rcvr */
  t_completing ();
#endif /* 0 */

#if LOG
  spf_print_signature (stdout, "Input lfg_semact: ");
#endif /* LOG */

  f_struct_nb = 0;
  
  if (tree_count != 0) {
    Xpq_top = spf.outputG.maxxnt + maxxt;
    allocate_all ();

#if FS_PROBAS
    if (is_proba_ranking) {
      if (logprobs_prod_nb != spf.inputG.maxprod || logprobs_nt_nb != spf.inputG.maxnt) {
	fputs ("\nThe \"logprob\" values are out of date with respect to the current LFG specification.\n", sxstderr);
	sxexit(1);
      }

      nbest_open (EPCFG);
      XxY_reuse (&ctxtXsign_hd, "ctxtXsign_hd", NULL, NULL);
    }
#endif /* FS_PROBAS */

    set_terminal_leaves ();
    set_non_terminal_error ();
    leaves_top = DTOP (fs_id_dstack);

    for (tree_id = 0; tree_id < tree_count; tree_id++) {
#if LOG
      printf ("\nParse tree #%i\n", tree_id+1);
#endif /* LOG */

      DTOP (fs_id_dstack) = leaves_top;
      with_ranking = SXFALSE;
      is_new_Pij_for_cyclic_fs_ids = SXTRUE; /* Ajoute' le 31/07/06 pour les structures cycliques */
      is_OK = spf_dag2tree (tree_id, spf.outputG.start_symbol, tree_bu_walk) != 0;
      
      if (is_OK && (x = Xpq2disp [spf.outputG.start_symbol]) > 0) {
	perform_inside_ranking (spf.outputG.start_symbol);
	/* is_consistent est positionne' */
	nb = f_struct_nb = fs_id_dstack [x];
      }
      else {
	nb = 0;
	is_consistent = SXTRUE; /* !! */
      }

#if LOG
      printf ("After consistency check, there %s %i %sconsistent f_structure%s in the parse tree #%i\n",
	      (nb == 1) ? "is" : "are",
	      nb,
	      is_consistent ? "" : "un",
	      (nb == 1) ? "" : "s",
	      tree_id+1);
#endif /* LOG */

      if (is_print_f_structure) {
	int head, i;
	int *base_ptr;

	printf ("\n*** After consistency check, there %s %i %sconsistent f_structure%s in the parse tree #%i ***\n",
		(nb == 1) ? "is" : "are",
		nb,
		is_consistent ? "" : "un",
		(nb == 1) ? "" : "s",
		tree_id+1);

	if (nb) {
	  /* Si !is_consistent, on imprime tout le monde, sinon, on n'imprique que les coherents */
	  if (is_consistent)
	    nb += unconsistent_nb;

	  base_ptr = fs_id_dstack+x+1;

	  for (i = 1; i <= nb; i++) {
	    head = *base_ptr++;

	    if (head > 0 || !is_consistent) {
	      if (head < 0)
		head = -head;

	      printf ("\n#%i.%i: ", tree_id+1, i);
	      print_f_structure (XxY_X (heads, head) /* Xpq */,
				 XxY_Y (heads, head) /* fs_id */,
				 !is_consistent,
				 SXTRUE);
	    }
	  }
	}

	printf ("*** End of parse tree #%i ***\n", tree_id+1);
      }

      clear_Xpq_set ();
    }

#if FS_PROBAS
    if (is_proba_ranking)
      nbest_close ();

    if (is_proba_ranking)
      nbest_free ();
#endif /* FS_PROBAS */

    //    free_all ();
  }

#if LOG
  printf ("There %s %i consistent f_structure%s in this parse forest.\n",
	  (f_struct_nb == 1) ? "is" : "are",
	  f_struct_nb,
	  (f_struct_nb == 1) ? "" : "s");
#endif /* LOG */

  return 1;
}



#ifndef ESSAI
int
atom_id2local_atom_id (atom_id)
     int atom_id;
{
  /* 1<<local_atom_id == atom_id */
  int local_atom_id, byte;

  if (atom_id == 0)
    return 0;

  local_atom_id = 0;

  while ((byte = (atom_id & 0XFF)) == 0) {
    local_atom_id += 8;
    atom_id >>= 8;
  }

  if ((atom_id & 0XF) == 0) {
    local_atom_id += 4;
    atom_id >>= 4;
  }

  if ((atom_id & 0X3) == 0) {
    local_atom_id += 2;
    atom_id >>= 2;
  }

  if ((atom_id & 0X1) == 0) {
    local_atom_id += 1;
    atom_id >>= 1;
  }

  return local_atom_id;
}
#endif /* ESSAI */


int
get_atomic_field_val (fs_id, field_id)
     int fs_id, field_id;
{
  int *val_ptr, objet_val;

  if (fs_is_set (fs_id, field_id, &val_ptr) && (objet_val = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
    return field_id2atom_field_id [field_id] [atom_id2local_atom_id (objet_val)];
#else /* ESSAI */
    return X_X (atom_id2local_atom_id_hd,objet_val);
#endif /* ESSAI */
  }

  return 0;
}

/* parcourt successivement toutes les productions referencees par un champ aij de valeur aij_val.
   (aij_val de'note un ensemble de productions Pij)
   Sur chacune de ces productions Pij, elle appelle f(Pij).  Si f(Pij) retourne une valeur non nulle,
   le parcourt est abandonne' et walk_aij retourne cette valeur */
int
walk_aij (aij_val, f)
     int aij_val;
     int (*f) ();
{
  int cur, top, Pij, ret_val;

  for (cur =  XH_X (XH_ufs_hd, aij_val), top = XH_X (XH_ufs_hd, aij_val+1);
      cur < top;
      cur++) {
    Pij = XH_list_elem (XH_ufs_hd, cur);

    if (Pij < 0)
      break;

    if (Pij <= spf.outputG.maxxnt)
      sxtrap (ME, "walk_aij");

    Pij -= spf.outputG.maxxnt;

    if (ret_val = (*f)(Pij))
      return ret_val;
  }

  return 0;
}


static int
relaxed_dag_td_walk (Pij)
     int Pij;
{
  int        Aij, A, Xpq;
  struct lhs *plhs;
  struct rhs *prhs;

  Aij = spf.walk.cur_Aij;
  A = Aij2A (Aij);

  if (is_ranking && IS_COMPLETE(A) && RANKS (A) [0] == '$') {
    /* C'est un nt "principal" ... */
    if (Xpq2disp [Aij] > 0) {
      /* ... qui a des f_structures */
      /* On le marque, ainsi que tous ses descendants */
      SXBA_1_bit (consistent_Xpq_set, Aij);
    }
  }

  if (SXBA_bit_is_set (consistent_Xpq_set, Aij)) {
    /* La lhs est marquee, on marque la Pij et les fils */
    /* ... si elle contient des f_structures ou derive ds le vide */
    if ((Aij2i (Aij) == Aij2j (Aij)) || Pij2disp [Pij] != 0) {
      SXBA_1_bit (consistent_Pij_set, Pij);

      plhs = spf.outputG.lhs+Pij;
      prhs = spf.outputG.rhs+plhs->prolon-1;

      while ((Xpq = (++prhs)->lispro) != 0) {
	if (Xpq < 0)
	  /* terminal */
	  Xpq = spf.outputG.maxxnt-Xpq;

	SXBA_1_bit (consistent_Xpq_set, Xpq);
      }
    }
    else {
      /* Le 15/09/05 */
      /* Pij ne participe pas aux f_structures de Aij, il n'est donc pas necessaire de reevaluer la sous-foret de racine Pij */
      return -1;
    }
  }

  return 1;
}




/* Aucune f_structure a la racine ... */
/* ... on essaie de sauver les meubles */
/* Recherche de l'arbre maximal */
/* On va attaquer les Aij-prods */
static int
maximal_tree_seek_pre_bu_walk (Aij)
     int Aij;
{
  maximal_tree_Aij_span [Aij] = (Xpq2disp [Aij] > 0) ? (Aij2j (Aij) - Aij2i (Aij)) : 0;

  return 1;
}


static int
maximal_tree_seek_bu_walk (prod)
     int prod;
{
  int     item, Aij, Xpq, Tpq, span;

  Aij = spf.walk.cur_Aij;

#if 0
  if (Aij != spf.walk.prev_Aij) {
    /* 1ere Aij-prod */
    if (Xpq2disp [Aij] > 0) {
      maximal_tree_Aij_span [Aij] = Aij2j (Aij) - Aij2i (Aij);
    }
    else
      maximal_tree_Aij_span [Aij] = 0;
  }
#endif /* 0 */

  item = spf.outputG.lhs [prod].prolon;
  span = 0;

  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    /* ne pas compter les t */
    if (Xpq > 0) {
      /* nt ou xnt */
      if (Xpq <= spf.outputG.maxnt)
	/* nt */
	span += maximal_tree_Aij_span [Xpq];
    }    
  }

  maximal_tree_Pij_span [prod] = span;

  if (maximal_tree_Aij_span [Aij] < span)
    maximal_tree_Aij_span [Aij] = span;

  return 1; /* On ne touche pas a la foret partagee */
}

static void
clear_maximal_tree_spans ()
{
  int *p ;

  p = maximal_tree_Aij_span + spf.outputG.maxxnt+1;

  while (--p >= maximal_tree_Aij_span)
    *p = 0;

  p = maximal_tree_Pij_span + spf.outputG.maxxprod+1;

  while (--p >= maximal_tree_Pij_span)
    *p = 0;
}

/* Si heads_set == NULL, on prend la 1ere coherente (si elle existe) */
static void
filter_Aij_heads (Aij, heads_set)
     int  Aij;
     SXBA heads_set;
{
  int x, d, i, head, base_head;
  int *base_ptr;

  if (x = Xpq2disp [Aij]) {
    base_ptr = fs_id_dstack + x;
    d = *base_ptr;

    if (d > 1) {
      *base_ptr = 0;

      for (i = 1; i <= d; i++) {
	base_head = head = base_ptr [i];

	if (heads_set == NULL) {
	  if (head > 0)
	    /* On prend la 1ere coherente ... */
	    break;
	}
	else {
	  if (head < 0)
	    head = -head;

	  if (SXBA_bit_is_set (heads_set, head)) {
	    /* On garde */
	    PUSH (base_ptr, base_head);
	  }
	}
      }

      if (heads_set == NULL) {
	/* ... sinon, elles sont toutes incoherentes et on prend la derniere */
	PUSH (base_ptr, head);
      }

      filter_Pij_heads (base_ptr, Aij, SXFALSE);
    }
  }
}


static int
tree_filter_td_walk (prod)
     int prod;
{
  int        Aij_span, Pij_span, ret_val, Aij, x, head, base_head, rule, i, doldol, Xpq, d, nb;
  int        *rule_stack, *base_ptr;
  struct lhs *plhs;
  struct rhs *prhs, *p;
#if LOG
  SXBOOLEAN    printed;
#endif /* LOG */

  Aij = spf.walk.cur_Aij;

  if (Aij != spf.walk.prev_Aij /* 1ere Aij_prod */
      && !is_partial /* y'a des f_structures a la racine */) {
    Aij_span = 0;

#if EBUG
    if (Xpq2disp [Aij] == 0)
      sxtrap (ME, "tree_filter_td_walk");
#endif /* EBUG */
  }
  else
    Aij_span = maximal_tree_Aij_span [Aij];

  if (Aij_span == -1)
    ret_val = -1; /* on vire, une autre prod a ete choisie */
  else {
    if ((x = Xpq2disp [Aij]) == 0) {
      /* pas de f_struct */
      Pij_span = maximal_tree_Pij_span [prod];

      /* Si 2 Aij-prods ont le meme span, on ne conserve que la 1ere */
      if (Aij_span == Pij_span) {
	/* On choisit celle_la */
	ret_val = 1;
	maximal_tree_Aij_span [Aij] = -1;
      }
      else
	ret_val = -1; /* sinon on vire */ 
    }
    else {
      /* Est-ce un noeud max ou un de ses descendants ? */
      if (Aij != spf.walk.prev_Aij) {
	/* 1ere Aij_prod */
	if (SXBA_bit_is_set (son_of_maximal_Xpq_set, Aij)) {
	  /* fils d'un maximal */
	  /* On filtre en utilisant valid_heads_set qui a ete positionne sur tous les peres de Aij */
	  filter_Aij_heads (Aij, valid_heads_set);
	}
	else {
	  /* maximal */
	  /* On ne conserve qu'une f_structure !! */
	  if (is_unique_parse /* && cur_pass_number == MAX_PASS_NB */)
	    filter_Aij_heads (Aij, NULL);

	  nb = fs_id_dstack [x];
#if LOG
	  printed = SXFALSE;
#endif /* LOG */

	  DCHECK (heads_stack, nb);

	  while (nb > 0) {
	    base_head = head = fs_id_dstack [x+nb];

	    if (head < 0)
	      head = -head;
	
	    if (XxY_Y (heads, head) /* fs_id */ > 0) {
	      /* En particulier, on ne note pas les Aii */
	      DSPUSH (heads_stack, base_head);
      
#if LOG
	      if (!printed) {
		print_symb (sxstdout, Aij2A (Aij), Aij2i (Aij), Aij2j (Aij)); /* BUG: on n'a plus (pas?) spf.insideG.nt2init_nt, semble-t-il */
		fputs (" ... is maximal thus ...\n", stdout);
		printed = SXTRUE;
	      }
#endif /* LOG */
	    }

	    nb--;
	  }
	}
      }

      /* Ici Pij2disp [prod] est coherent avec Xpq2disp [Aij] */
      if (x = Pij2disp [prod]) {
	plhs = spf.outputG.lhs+prod;
	prhs = spf.outputG.rhs+plhs->prolon-1;

	/* On cherche, pour chaque fils, le[/les] head qui est responsable de static_base_head */
	for (rule = PROD2RULE (plhs->init_prod); rule != 0; rule = rule2next_rule [rule]) {
	  rule_stack = fs_id_Pij_dstack + x;
	  nb = rule_stack [0];
	  x += nb+1; /* pour la prochaine rule */

	  for (i = 1; i < nb; i += 2) {
	    /* cette rule a produit la lhs */
	    doldol = rule_stack [i+1];

	    /* On valide les head des fils */
	    /* doldol == -1 => bloc d'equations vide ou production vide */
	    p = prhs;

	    while ((Xpq = (++p)->lispro) != 0) {
	      base_ptr = fs_id_dstack + Xpq2disp [(Xpq < 0) ? spf.outputG.maxxnt-Xpq : Xpq];
	      d = *base_ptr++;

	      if (Xpq > 0) {
#if EBUG
		if (d == 0)
		  sxtrap (ME, "tree_filter_td_walk");
#endif /* EBUG */

		/* doldol == -1 => bloc d'equations vide ou production vide */
		head = doldol >= 0 ? base_ptr [doldol % d] : base_ptr [0]; /* a garder */

		if (head < 0)
		  head = -head;

		/* On ne peut pas filtrer Xpq tout de suite car il peut avoir plusieurs peres */
		/* valid_heads_set sert a differer ce filtrage */
		SXBA_1_bit (valid_heads_set, head);

		/* un fils d'un Aij attribue' n'est pas maximal */
		if (Xpq <= spf.outputG.maxnt)
		  SXBA_1_bit (son_of_maximal_Xpq_set, Xpq);

#if EBUG
		if (XxY_X (heads, head) != Xpq)
		  sxtrap (ME, "tree_filter_td_walk");
#endif /* EBUG */
	      }
				 
	      if (doldol >= 0)
		doldol /= d;
	    }
	  }
	}
      
	ret_val = 1;

	if (is_unique_parse /* && cur_pass_number == MAX_PASS_NB */) {
	  /* On choisit un seul arbre, ca inhibe les autres Aij-prods eventuelles */
	  maximal_tree_Aij_span [Aij] = -1;
	}
      }
      else
	ret_val = -1;
    }
  }

#if LOG
  spf_print_iprod (stdout, prod, "", "" /* without nl */);

  if (ret_val >= 0) {
    fputs (" ... kept\n", stdout);

    if (ret_val == 0)
      print_f_structures (prod);
  }
  else
    fputs (" ... erased\n", stdout);
#endif /* LOG */
 
  return ret_val;
}







static int
forest_sem_pass ()
{
  double            tree_count;
  int               x, cur_nb;
  int               f_struct_nb, unconsistent_f_struct_nb, total_f_struct_nb;
  char              pass_str [30];

  if (priority_vector == NULL)
    priority_vector = &(priority_vector_default [0]);

  priority_vector_lgth = strlen (priority_vector);

  maxxt = -spf.outputG.maxt;

#if 0
  /* fait ds earley le 28/09/05 */
  /* On complete la structure "Tij" avec les terminaux couverts par les nt de la rcvr */
  t_completing ();
#endif /* 0 */

  if (
#if LOG
      SXTRUE ||
#endif /* LOG */
      is_print_f_structure) {
    spf_print_signature (stdout, "Input state (lfg_semact): ");
  }

  /* Dans tous les cas on commence par instancier les feuilles terminales.  C,a permet d'avoir un xml
     minimal qui contiendra les symboles terminaux et les f_structures associees */
  Xpq_top = spf.outputG.maxxnt + maxxt;
  allocate_all ();
	    
#ifdef EASY
  qq_valeurs (); 
  /* On "repartit" les commentaires associes aux ff _EPSILON (qui sont optionnelles) dans les tokens de ses "voisins"
     distribute_comment est ds dag_scanner.c */
  //  distribute_comment (epsilon_code);
#endif /* EASY */
#if 0
  /* Fait ds set_terminal_leaves () */
  spf_fill_Tij2tok_no ();

#endif /* 0 */

#if FS_PROBAS
  if (is_proba_ranking) {
    if (logprobs_prod_nb != spf.inputG.maxprod || logprobs_nt_nb != spf.inputG.maxnt) {
      fputs ("\nThe \"logprob\" values are out of date with respect to the current LFG specification.\n", sxstderr);
      sxexit(1);
    }

    nbest_open (EP_POS_EPCFG);
    XxY_reuse (&ctxtXsign_hd, "ctxtXsign_hd", NULL, NULL);
  }
#endif /* FS_PROBAS */

  set_terminal_leaves ();

  set_non_terminal_error ();

  if (spf.outputG.start_symbol == 0) {
    fputs ("Parsing failed\n", stdout);
  }
  else {
    if (spf.inputG.has_cycles) {
      fputs ("Sorry, cyclic grammars cannot be f_structured.\n", stdout);
    }
    else {
      /* Le 10/05/05 on peut specifier la passe ds laquelle sont executee les unifications */
      for (cur_pass_number = 1; cur_pass_number <= MAX_PASS_NB; cur_pass_number++) {
	if (cur_pass_number > 1)
	  clear_for_relaxed_run ();

#if LOG       
	if (!is_print_f_structure) {
	  if (cur_pass_number <= 3)
	    printf ("\n\n\n\
****************** %s   P A S S ******************\n\n\n", (cur_pass_number == 1) ? "F I R S T" : ((cur_pass_number == 2) ? "S E C O N D" : "T H I R D"));
	  else
	    printf ("\n\n\n\
****************** %ith   P A S S ******************\n\n\n", cur_pass_number);
	}
#endif /* LOG */     

	if (is_print_f_structure) {
	  if (cur_pass_number <= 3)
	    printf ("\n\n\n\
****************** %s   P A S S ******************\n\n\n", (cur_pass_number == 1) ? "F I R S T" : ((cur_pass_number == 2) ? "S E C O N D" : "T H I R D"));
	  else
	    printf ("\n\n\n\
****************** %ith   P A S S ******************\n\n\n", cur_pass_number);
	}

	if (!is_relaxed_run) {
	  /* Si une passe precedente a positionne is_relaxed_run, toutes les passes suivantes se font en mode relaxe */
#if LOG
	  fputs ("\npd = tree_bu_walk\n", stdout);
#endif /* LOG */
	  

	  is_new_Pij_for_cyclic_fs_ids = SXTRUE /* Ajoute' le 31/07/06 pour les structures cycliques */,
	  with_ranking = SXFALSE, spf_topological_bottom_up_walk (spf.outputG.start_symbol,
								tree_bu_walk,
#if LOG
								tree_pre_bu_walk,
#else /* !LOG */
								NULL,
#endif /* !LOG */
								tree_post_bu_walk);

	  if (sxba_scan (to_be_erased_Pij_set, 0) > 0) {
	    /* tree_bu_walk a trouver des Pij a supprimer, on le fait */
	    /* On pourrait etre +fin et incorporer ca ds un spf_topological_walk ulterieur */
	    spf_erase (to_be_erased_Pij_set);
	    sxba_empty (to_be_erased_Pij_set);

#if LOG
	    spf_print_signature (stdout, "Current state (lfg_semact): ");
#endif /* LOG */
	  }

	  if ((x = Xpq2disp [spf.outputG.start_symbol]) > 0) {
	    /* On prend, apres ranking,  de la racine :
	       - soit uniquement les coherentes (s'il y en a)
	       - soit la totalite' (des incoherentes) */
	    is_partial = SXFALSE; /* y'a des f_struct a la racine */
	  }
	  else {
	    /* Echec de la cur_pass_number eme passe */
	    is_partial = SXTRUE; /* y'a pas de f_struct a la racine */

	    if (is_print_time) {
	      if (cur_pass_number <= 3)
		sprintf (pass_str, "\tLfg %s Pass (strict, SXFALSE)", (cur_pass_number == 1) ? "First" : ((cur_pass_number == 2) ? "Second" : "Third"));
	      else
		sprintf (pass_str, "\tLfg %ith Pass (strict, SXFALSE)", cur_pass_number);

	      sxtime (SXACTION, pass_str);
	    }

#if LOG        
	    if (!is_print_f_structure) {
	      if (cur_pass_number <= 3)
		printf ("\n\n\n\
****************** %s   P A S S    F A I L E D ******************\n\n\n", (cur_pass_number == 1) ? "F I R S T" : ((cur_pass_number == 2) ? "S E C O N D" : "T H I R D"));
	      else
		printf ("\n\n\n\
****************** %ith   P A S S    F A I L E D ******************\n\n\n", cur_pass_number);
	    }
#endif /* LOG */      
	    
	    if (is_print_f_structure) {
	      if (cur_pass_number <= 3)
		printf ("\n\n\n\
****************** %s   P A S S    F A I L E D ******************\n\n\n", (cur_pass_number == 1) ? "F I R S T" : ((cur_pass_number == 2) ? "S E C O N D" : "T H I R D"));
	      else
		printf ("\n\n\n\
****************** %ith   P A S S    F A I L E D ******************\n\n\n", cur_pass_number);
	    }

#if LOG
	    fputs ("\n***************** Current Structures Values *****************\n", stdout);
	    fputs ("pi = print_cf_structures\n", stdout);

	    spf_topological_walk (spf.outputG.start_symbol, print_cf_structures, NULL);

	    spf_print_signature (stdout, "Current state (lfg_semact): ");
	    fputs ("***************** End of the Current Structures Values *****************\n", stdout);
#endif /* LOG */

	    if (relaxed_run_allowed) {
	      /* On reessaie la meme passe (et les suivantes) sans faire les tests de coherence ... */
	      is_relaxed_run = SXTRUE;
      
	      /* Le 11/08/05
		 On va essayer de reutiliser (certaines) f_structures coherentes de la 1ere tentative.
		 Essai : seulement sur un ensemble de nt donnes par l'utilisateur (ceux qui ont un $ ds la specif COMPLETE)
		 On pourrait essayer de le faire sur tous ??
		 On fait donc une premiere passe top-down qui va marquer les bons Aij (et leurs descendants) qui contiennent des structures 
		 coherentes
	      */
	      if (consistent_Xpq_set == NULL) {
		consistent_Xpq_set = sxba_calloc (Xpq_top+1);
		consistent_Pij_set = sxba_calloc (spf.outputG.maxxprod+1);
	      }
	      else {
		sxba_empty (consistent_Xpq_set);
		sxba_empty (consistent_Pij_set);
	      }

	      consistent_cur_pass_number = cur_pass_number;

#if LOG
	      fputs ("\n\"$\"-filtering\n", stdout);
	      /* Si un Aij est un $-node et s'il a des f_structures on le note ds consistent_Xpq_set et consistent_Pij_set */
	      /* De plus si une telle Aij-prod n'a pas de f_structures, elle est supprimee de la foret */
	      fputs ("pi = relaxed_dag_td_walk\n", stdout);

	      spf_topological_walk (spf.outputG.start_symbol, relaxed_dag_td_walk, NULL);

	      print_consistent_Xpq_set ();
#endif /* LOG */
	    
	      if (sxba_is_empty (consistent_Xpq_set))
		clear_for_relaxed_run ();
	      else
		partial_clear_for_relaxed_run ();
	    }
	  }
	}

	if (is_relaxed_run) {
	  is_locally_unconsistent = SXFALSE;

#if LOG      
	  if (!is_print_f_structure) {
	    fputs ("\n\n\n<<<The first attempt failed>>>\n\
****************** S E C O N D   A T T E M P T ******************\n\
               (the consistency checks are relaxed)\n\n", stdout);
	  }
#endif /* LOG */
      
	  if (is_print_f_structure) {
	    fputs ("\n\n\n<<<The first attempt failed>>>\n\
****************** S E C O N D   A T T E M P T ******************\n\
               (the consistency checks are relaxed)\n\n", stdout);
	  }

#if LOG
	  fputs ("\npd = tree_bu_walk\n", stdout);
#endif /* LOG */
	  
	  is_new_Pij_for_cyclic_fs_ids = SXTRUE /* Ajoute' le 31/07/06 pour les structures cycliques */,
	  with_ranking = SXFALSE, spf_topological_bottom_up_walk (spf.outputG.start_symbol,
								tree_bu_walk,
#if LOG
								tree_pre_bu_walk,
#else /* !LOG */
								NULL,
#endif /* !LOG */
								tree_post_bu_walk);

	  if (sxba_scan (to_be_erased_Pij_set, 0) > 0) {
	    /* tree_bu_walk a trouve' des Pij a supprimer, on le fait */
	    /* On pourrait etre +fin et incorporer ca ds un spf_topological_walk ulterieur */
	    spf_erase (to_be_erased_Pij_set);
	    sxba_empty (to_be_erased_Pij_set);

#if LOG
	    spf_print_signature (stdout, "Current state (lfg_semact): ");
#endif /* LOG */
	  }

	  if ((x = Xpq2disp [spf.outputG.start_symbol]) > 0) {
	    /* Le run relax a marche' */
	    is_partial = SXFALSE; /* y'a des f_struct a la racine */
	    /* On prend toutes les f_structures de la racine */

#if LOG      
	    if (!is_print_f_structure) {
	      fputs ("\n\n\n****************** S E C O N D   A T T E M P T   S U C C E E D E D ******************\n\n\n", stdout);
	    }
#endif /* LOG */
      
	    if (is_print_f_structure) {
	      fputs ("\n\n\n****************** S E C O N D   A T T E M P T   S U C C E E D E D ******************\n\n\n", stdout);
	    }
	  }
	  else {
	    /* Echec : aucune f_structure (meme incoherente) a la racine */
	    is_partial = SXTRUE; /* y'a pas de f_struct a la racine */

#if LOG      
	    if (!is_print_f_structure) {
	      fputs ("\n\n\n****************** S E C O N D   A T T E M P T   F A I L E D ******************\n\n\n", stdout);
	    }
#endif /* LOG */
      
	    if (is_print_f_structure) {
	      fputs ("\n\n\n****************** S E C O N D   A T T E M P T   F A I L E D ******************\n\n\n", stdout);
	    }
	  }
	}

	if (is_partial /* pas de f_structure a la racine */ &&
	    (is_relaxed_run || cur_pass_number == MAX_PASS_NB /* derniere passe on recupere ce qu'on peut */)
	    ) {
	    /* On cherche le[/les] arbre de couverture maximale */

#if LOG
	    fputs ("\nMaximal tree(s) seek\n", stdout);
	    fputs ("pd = maximal_tree_seek_bu_walk\n", stdout);
#endif /* LOG */

	    /* On calcule les span maximaux de chaque Aij et Pij ... */
	    spf_topological_bottom_up_walk (spf.outputG.start_symbol, maximal_tree_seek_bu_walk, maximal_tree_seek_pre_bu_walk, NULL);
	}
	else {
	  clear_maximal_tree_spans ();
	}

	/* Forest filtering */
	heads_stack [0] = 0;

#if LOG
	fputs ("\n*** Tree filtering out for unused f_structures ***\n", stdout);
#endif /* LOG */

	valid_heads_set = sxba_calloc (XxY_top (heads)+1);

	if (son_of_maximal_Xpq_set == NULL)
	  son_of_maximal_Xpq_set = sxba_calloc (spf.outputG.maxxnt+1);
	else
	  sxba_empty (son_of_maximal_Xpq_set);

#if LOG
	fputs ("pd = tree_filter_td_walk\n", stdout);
#endif /* LOG */

	/* ... et on filtre */
	spf_topological_walk (spf.outputG.start_symbol, tree_filter_td_walk, NULL);

	/* heads_stack est rempli */
	sxfree (valid_heads_set), valid_heads_set = NULL;

#if LOG
	spf_print_signature (stdout, "Current state (lfg_semact): ");
	fputs ("*** End of Tree filtering out for unused f_structures ***\n\n", stdout);
#endif /* LOG */

	cur_nb = heads_stack [0]; /* Nombre de structures "valides" */

#if LOG
	if (!is_print_f_structure) {
	  printf ("%sWith%s ranking, %s consistency checks, there %s %i %sf_structure%s in this shared parse forest\n",
		  MAX_PASS_NB == 1 ? "" : ((cur_pass_number == 1) ? "(First pass) " : ((cur_pass_number == 2) ? "(Second pass) " : ((cur_pass_number == 3) ? "(Third pass) " : ("(n+1th pass) ")))),
		  with_ranking ? "" : "out",
		  is_relaxed_run ? "without" : "after",
		  (cur_nb == 1) ? "is" : "are",
		  cur_nb,
		  is_partial ? "partial " : (is_relaxed_run ? "" : (is_consistent ? "consistent " : "unconsistent ")),
		  (cur_nb == 1) ? "" : "s");
	}
#else /* !LOG */
	if (is_print_final_f_structure_number && !is_print_f_structure) {
	  printf ("\n*** %sWith%s ranking, %s consistency check, there %s %i %sf_structure%s in this shared parse forest ***\n",
		  MAX_PASS_NB == 1 ? "" : ((cur_pass_number == 1) ? "(First pass) " : ((cur_pass_number == 2) ? "(Second pass) " : ((cur_pass_number == 3) ? "(Third pass) " : ("(n+1th pass) ")))),
		  with_ranking ? "" : "out",
		  is_relaxed_run ? "without" : "after",
		  (cur_nb == 1) ? "is" : "are",
		  cur_nb,
		  is_partial ? "partial " : (is_relaxed_run ? "" : (is_consistent ? "consistent " : "unconsistent ")),
		  (cur_nb == 1) ? "" : "s");
	}
#endif /* !LOG */

	if (is_print_f_structure) {
	  int head, base_head, i;
	  int *base_ptr;

	  printf ("\n*** %sWith%s ranking, %s consistency check, there %s %i %sf_structure%s in this shared parse forest ***\n",
		  MAX_PASS_NB == 1 ? "" : ((cur_pass_number == 1) ? "(First pass) " : ((cur_pass_number == 2) ? "(Second pass) " : ((cur_pass_number == 3) ? "(Third pass) " : ("(n+1th pass) ")))),
		  with_ranking ? "" : "out",
		  is_relaxed_run ? "without" : "after",
		  (cur_nb == 1) ? "is" : "are",
		  cur_nb,
		  is_partial ? "partial " : (is_relaxed_run ? "" : (is_consistent ? "consistent " : "unconsistent ")),
		  (cur_nb == 1) ? "" : "s");

	  if (cur_nb) {
	    /* Si !is_consistent, on imprime tout le monde, sinon, on n'imprique que les coherents */
	    for (i = 1; i <= cur_nb; i++) {
	      base_head = head = heads_stack [i];

	      if (head < 0)
		head = -head;

	      if (i > 1)
		fputs ("\n", stdout);

	      printf ("#%i/%i: ", i, cur_nb);
	      print_f_structure (XxY_X (heads, head) /* Xpq */, XxY_Y (heads, head) /* fs_id */, base_head < 0, SXTRUE);
	    }
	  }

	  fputs ("*** End of the main f_structures ***\n\n", stdout);
	}

#if LOG
	fputs ("\n***************** Current Structures Values *****************\n", stdout);
	fputs ("pi = print_cf_structures\n", stdout);

	spf_topological_walk (spf.outputG.start_symbol, print_cf_structures, NULL);

	spf_print_signature (stdout, "Current state (lfg_semact): ");
	fputs ("***************** End of the Current Structures Values *****************\n", stdout);

	if (!is_print_f_structure) {
	  if (cur_pass_number <= 3)
	    printf ("\n\n\n\
****************** E N D   O F   T H E   %s   P A S S ******************\n\n\n", (cur_pass_number == 1) ? "F I R S T" : ((cur_pass_number == 2) ? "S E C O N D" : "T H I R D"));
	  else
	    printf ("\n\n\n\
****************** %ith   P A S S ******************\n\n\n", cur_pass_number);
	}
#endif /* LOG */

	if (is_print_f_structure) {
	  if (cur_pass_number <= 3)
	    printf ("\n\n\n\
****************** E N D   O F   T H E   %s   P A S S ******************\n\n\n", (cur_pass_number == 1) ? "F I R S T" : ((cur_pass_number == 2) ? "S E C O N D" : "T H I R D"));
	  else
	    printf ("\n\n\n\
****************** %ith   P A S S ******************\n\n\n", cur_pass_number);
	}

	if (is_print_time) {
	  if (is_relaxed_run) {
	    if (cur_pass_number <= 3)
	      sprintf (pass_str, "\tLfg %s Pass (relaxed, SXFALSE)", (cur_pass_number == 1) ? "First" : ((cur_pass_number == 2) ? "Second" : "Third"));
	    else
	      sprintf (pass_str, "\tLfg %ith Pass (relaxed, SXFALSE)", cur_pass_number);
	  }
	  else {
	    if (cur_pass_number <= 3)
	      sprintf (pass_str, "\tLfg %s Pass (%s)", (cur_pass_number == 1) ? "First" : ((cur_pass_number == 2) ? "Second" : "Third"), is_consistent ? "TRUE" : "FALSE");
	    else
	      sprintf (pass_str, "\tLfg %ith Pass (%s)", cur_pass_number, is_consistent ? "TRUE" : "FALSE");
	  }

	  sxtime (SXACTION, pass_str);
	}
      }
    }
  }

  if (
#if LOG
      SXTRUE ||
#endif /* LOG */
      is_print_final_c_structure || is_print_final_cf_structure) {

#if LOG
    spf_print_signature (stdout, "Final state (lfg_semact): ");
#endif /* LOG */

    if (is_print_final_cf_structure) {
#if LOG
      fputs ("pi = print_cf_structures\n", stdout);
#endif /* LOG */
      spf_topological_walk (spf.outputG.start_symbol, print_cf_structures, NULL);
    }
    else {
#if LOG
      fputs ("pi = print_c_structures\n", stdout);
#endif /* LOG */
      spf_topological_walk (spf.outputG.start_symbol, print_c_structures, NULL);
    }
  }

  //  free_all ();

  return is_consistent;
}



static char	Usage [] = "\
\t\t-pfs, -print_f_structure,\n\
\t\t-opf, -on_parse_forest,\n\
\t\t-opt, -on_parse_tree,\n\
\t\t-pfcs, -print_final_c_structure,\n\
\t\t-xml pathname,\n\
\t\t-pxml pathname,\n\
\t\t-s #sentence, -sentence_nb #sentence,\n\
\t\t-pfcfs, -print_final_cf_structure,\n\
\t\t-mp -max_priority, -sp -sum_priority,\n\
\t\t-rr, -relaxed_run,\n\
\t\t-up, -unique_parse,\n\
\t\t-is pathname, -input_sentence pathname,\n\
\t\t-id pathname, -input_dag pathname,\n\
\t\t-r, -ranking, -nr, -no_ranking,\n\
\t\t-pffsn, -print_final_f_structure_number,\n\
\t\t-rp #string, -repair #string,\n\
\t\t-rpp #parses, -repair_parses #parses,\n\
\t\t-pr, -proba_ranking,\n\
\t\t-nr, -nbest_ranking,\n\
\t\t-pv #string, -priority_vector #string,\n\
\t\t-mfsnb #maxfs,\n\
\t\t-fsnt #thsldfs,\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	  0
#define PRINT_F_STRUCTURE 1
#define ON_PARSE_FOREST	  2
#define ON_PARSE_TREE	  3
#define PFCS              4
#define EMPTY_ARG         5 /* position libre */
#define SENTENCE_NB       6
#define PFCFS     	  7
#define MAXPRIORITY 	  8
#define RELAXED_RUN       9
#define UNIQUE_PARSE      10
#define EMPTY_ARG2        11 /* position libre */
#define INPUT_SENTENCE 	  12
#define INPUT_DAG 	  13
#define RANKING 	  14
#define PFFSN    	  15
#define REPAIR    	  16
#define REPAIR_PARSES  	  17
#define PROBA_RANKING  	  18
#define NBEST_RANKING  	  19
#define PRIORITY_VECTOR	  20
#define MAX_FS_NB	  21
#define FS_NB_THRESHOLD	  22


static char	*option_tbl [] = {
    "",
    "pfs", "print_f_structure",
    "opf", "on_parse_forest",
    "opt", "on_parse_tree",
    "pfcs", "print_final_c_structure",
    "emtpy",
    "s", "sentence_nb",
    "pfcfs", "print_final_cf_structure",
    "mp",  "max_priority", "sp", "sum_priority",
    "rr", "relaxed_run",
    "up", "unique_parse",
    "empty2",
    "is", "input_sentence",
    "id", "input_dag",
    "r", "ranking", "nr", "no_ranking",
    "pffsn", "print_final_f_structure_number",
    "rp", "repair",
    "rpp", "repair_parses",
    "pr", "proba_ranking",
    "nr", "nbest_ranking",
    "pv", "priority_vector",
    "mfsnb",
    "fsnt",
};

static int	option_kind [] = {
    UNKNOWN_ARG,
    PRINT_F_STRUCTURE, PRINT_F_STRUCTURE,
    ON_PARSE_FOREST, ON_PARSE_FOREST,
    ON_PARSE_TREE, ON_PARSE_TREE,
    PFCS, PFCS,
    EMPTY_ARG,
    SENTENCE_NB, SENTENCE_NB,
    PFCFS, PFCFS,
    MAXPRIORITY, MAXPRIORITY, -MAXPRIORITY, -MAXPRIORITY,
    RELAXED_RUN, RELAXED_RUN,
    UNIQUE_PARSE, UNIQUE_PARSE,
    EMPTY_ARG2,
    INPUT_SENTENCE, INPUT_SENTENCE,
    INPUT_DAG, INPUT_DAG,
    RANKING, RANKING, -RANKING, -RANKING,
    PFFSN, PFFSN,
    REPAIR, REPAIR,
    REPAIR_PARSES, REPAIR_PARSES,
    PROBA_RANKING, PROBA_RANKING,
    NBEST_RANKING, NBEST_RANKING,
    PRIORITY_VECTOR, PRIORITY_VECTOR,
    MAX_FS_NB,
    FS_NB_THRESHOLD,
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

/* retourne la chaine des arguments possibles propres a lfg */
static char*
lfg_args_usage ()
{
  return Usage;
}

/* retourne le ME */
static char*
lfg_ME ()
{
  return ME;
}

/* decode les arguments specifiques a lfg */
/* l'option argv [*parg_num] est inconnue du parseur earley */
static SXBOOLEAN
lfg_args_decode (pargnum, argc, argv)
     int  *pargnum, argc;
     char *argv [];
{

  switch (option_get_kind (argv [*pargnum])) {
  case PRINT_F_STRUCTURE:
    is_print_f_structure = SXTRUE;
    break;

  case ON_PARSE_FOREST:
    is_parse_forest = SXTRUE;
    is_unfold_parse_forest = SXFALSE;
    break;

  case ON_PARSE_TREE:
    is_parse_forest = SXTRUE;
    is_unfold_parse_forest = SXTRUE;
    break;

  case PFCS:
    is_print_final_c_structure = SXTRUE;
    break;

  case SENTENCE_NB:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: an integer number must follow the \"%s\" option;\n", ME, option_get_text (SENTENCE_NB));
      return SXFALSE;
    }

    sentence_id = atoi (argv [*pargnum]);
    break;

  case PFCFS:
    is_print_final_cf_structure = SXTRUE;
    break;

  case MAXPRIORITY:
    is_sum_priority = SXFALSE;
    break;

  case -MAXPRIORITY:
    is_sum_priority = SXTRUE;
    break;

  case RELAXED_RUN:
    relaxed_run_allowed = SXTRUE;
    break;

  case UNIQUE_PARSE:
    is_unique_parse = SXTRUE;
    break;

  case INPUT_SENTENCE:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a (word) sentence must follow the \"%s\" option;\n", ME, option_get_text (INPUT_SENTENCE));
      return SXFALSE;
    }
	      
    input_sentence_string = argv [*pargnum];

    break;

  case INPUT_DAG:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a (dag) sentence must follow the \"%s\" option;\n", ME, option_get_text (INPUT_DAG));
      return SXFALSE;
    }
	      
    input_dag_string = argv [*pargnum];

    break;

  case RANKING:
    is_ranking = SXTRUE;
    break;

  case MAX_FS_NB:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a (maximum) number of feature structures must follow the \"%s\" option;\n", ME, option_get_text (MAX_FS_NB));
      return SXFALSE;
    }

    max_fs_nb = atoi (argv [*pargnum]);
    break;

  case FS_NB_THRESHOLD:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a number of feature structures must follow the \"%s\" option;\n", ME, option_get_text (MAX_FS_NB));
      return SXFALSE;
    }

    fs_nb_threshold = atoi (argv [*pargnum]);
    break;

  case -RANKING:
    is_ranking = SXFALSE;
    break;

  case PFFSN:
    is_print_final_f_structure_number = SXTRUE;
    break;

  case REPAIR:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a repair string length must follow the \"%s\" option;\n", ME, option_get_text (REPAIR));
      return SXFALSE;
    }

    rcvr_spec.perform_repair = SXTRUE;
    rcvr_spec.repair_kind = atoi (argv [*pargnum]);
    break;

  case REPAIR_PARSES:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a (maximum) number of parses for any repair string must follow the \"%s\" option;\n", ME, option_get_text (REPAIR_PARSES));
      return SXFALSE;
    }

    rcvr_spec.perform_repair_parse = SXTRUE;
    rcvr_spec.repair_parse_kind = atoi (argv [*pargnum]);
    break;

  case PROBA_RANKING:
    /* lance le calcul de probas sur les f_structures et permet, s'il est demande', le ranking correspondant */
    is_proba_ranking = SXTRUE;
    break;

  case NBEST_RANKING:
    /* Permet, si 'n' est un ranking demande' d'appeler nbest */
    is_nbest_ranking = SXTRUE;
    break;

  case PRIORITY_VECTOR:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a priority_vector string  must follow the \"%s\" option;\n", ME, option_get_text (PRIORITY_VECTOR));
      return SXFALSE;
    }

    priority_vector = argv [*pargnum];
    break;

  case UNKNOWN_ARG:
    return SXFALSE;
  }

  return SXTRUE;
}


/* Point d'entree initial, on positionne qq valeurs qui vont conditionner toute la suite */
void
lfg_semact ()
{
  if (dfield_pairs2field_id) {
    /* Y'a des noms de champs dynamiques, on reactive dfield_pairs */
    XxY_reuse (&dfield_pairs, "dfield_pairs", NULL /* oflw */, NULL /* stats */);
  }
  
  if (datom_pairs2atom_id) {
    /* Y'a des noms d'atomes dynamiques, on reactive datom_pairs */
    XxY_reuse (&datom_pairs, "datom_pairs", NULL /* oflw */, NULL /* stats */);
  }
  
  for_semact.sem_init = NULL;
  for_semact.sem_final = lfg_sem_final;
  for_semact.semact = NULL;
  for_semact.parsact = NULL;
  for_semact.prdct = NULL;
  for_semact.constraint = NULL;
  for_semact.sem_pass = is_unfold_parse_forest ? tree_sem_pass : forest_sem_pass;
  for_semact.scanact = NULL; /* pas de semantique lexicale derriere les ff : [| ... |] */
  for_semact.rcvr = NULL; /* C'est earley qui choisit la "meilleure" correction */
  for_semact.rcvr_range_walk = NULL; /* On utilise une strategie standard (ici MIXED_FORWARD_RANGE_WALK_KIND, voir +loin) */
  for_semact.process_args = lfg_args_decode; /* decodage des options de lfg */
  for_semact.string_args = lfg_args_usage; /* decodage des options de lfg */
  for_semact.ME = lfg_ME; /* decodage des options de lfg */

  /* Si earley fait de la rcvr, prendre cette strategie d'essai des points de reprise */  
  rcvr_spec.range_walk_kind = MIXED_FORWARD_RANGE_WALK_KIND; 

  /* On change les valeurs par defaut de l'analyseur earley car on est en lfg */
  is_parse_forest = SXTRUE;

  /* valeurs par defaut des options pour lfg */
  rcvr_spec.perform_repair = SXTRUE; /* On fait de la correction d'erreur ... */
  rcvr_spec.repair_kind = 1; /* ... mais on ne genere qu'une chaine de la longueur min */
  rcvr_spec.perform_repair_parse = SXTRUE; /* On analyse cette chaine ... */
  rcvr_spec.repair_parse_kind = 1; /* ... mais on ne genere qu'une seule analyse */

  is_unfold_parse_forest = SXFALSE; /* On evalue les f_structures sur la foret partagee */
  is_print_f_structure = SXFALSE; /* On n'ecrit pas les f_structures */
  is_print_final_c_structure = SXFALSE; /* On imprime la c_structure finale */
  is_print_final_cf_structure = SXFALSE; /* On imprime les c_structure et les f_structures finales */

  sentence_id = 1;
  is_sum_priority = SXFALSE; /* Si rank contient le chiffre 1 (tri par priorite'), par defaut, cette priorite est calculee en
			      prenant le max des prios des sous-structures.  Sinon cas -sum_priority c'est la somme qui
			      est utilisee */
  relaxed_run_allowed = SXFALSE;
  is_unique_parse = SXFALSE;

  is_print_output_dag = SXFALSE; /* peut etre different du is_print_output_dag de earley */

  is_ranking = SXTRUE; /* On fait le ranking specifie' par la grammaire */
  is_print_final_f_structure_number = SXFALSE; /* On n'imprime pas le nb de f_structures a la racine */
}
